#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/math/math.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/diag.hpp>
#include <utilities/hooking/hooking.hpp>
#include <utilities/logging/logging.hpp>
#include <utilities/security/security.hpp>
#include <core/rendering/rendering.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <protection/game_addresses.hpp>
#include <external/xdraw/xui/xui.hpp>
#include <utilities/lifecycle.hpp>
#include <utilities/loader_session.hpp>
#include <utilities/steam/steam.hpp>
#include <utilities/lobby_music_queue.hpp>
#include <utilities/tls/tls.hpp>
#include <core/features/changer/preview_scene.hpp>
#include <core/features/changer/preview_item.hpp>
#include <core/features/esp/primitive_buffer.hpp>
#include "../hooks.hpp"

namespace hooks {

	namespace detail {
		inline lobby_music::queue g_lobby_music_requests;
		inline std::atomic_bool g_generate_primitives_player_chams_applied{};
		// The recovered prototypes disagree between void*, __int64 and no
		// declared return. uintptr_t preserves the 64-bit RAX value for either
		// pointer- or integer-style declarations without changing the argument ABI.
		using generate_primitives_fn = std::uintptr_t( __fastcall* )( std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t );
		struct generate_primitives_thread_state
		{
			generate_primitives_fn original{};
			std::uintptr_t result{};
		};
		inline utilities::tls::local<generate_primitives_thread_state> g_generate_primitives_state{};

		std::uintptr_t __fastcall call_generate_primitives_original(
			std::uintptr_t desc, std::uintptr_t scene_object, std::uintptr_t scene_view, std::uintptr_t primitive_buffer )
		{
			auto* state = g_generate_primitives_state.get( );
			if ( !state || !state->original )
				return 0;

			state->result = state->original(
				desc, scene_object, scene_view, primitive_buffer );
			return state->result;
		}

		[[nodiscard]] inline bool valid_runtime_pointer( std::uintptr_t address ) noexcept
		{
			return address >= 0x100000000ull &&
				address <= 0x00007FFFFFFFFFFFull &&
				( address & ( alignof( std::uintptr_t ) - 1 ) ) == 0;
		}

		[[nodiscard]] inline bool readable_runtime_range( std::uintptr_t address, std::size_t size ) noexcept
		{
			constexpr auto max_user_address = std::uintptr_t{ 0x00007FFFFFFFFFFFull };
			if ( !valid_runtime_pointer( address ) || !size || size - 1 > max_user_address - address )
				return false;

			const auto end = address + size;
			auto current = address;
			while ( current < end )
			{
				MEMORY_BASIC_INFORMATION mbi{};
				if ( VirtualQuery( reinterpret_cast<const void*>( current ), &mbi, sizeof( mbi ) ) != sizeof( mbi ) ||
					mbi.State != MEM_COMMIT || ( mbi.Protect & ( PAGE_GUARD | PAGE_NOACCESS ) ) )
				{
					return false;
				}

				const auto protection = mbi.Protect & 0xffu;
				const bool readable = protection == PAGE_READONLY || protection == PAGE_READWRITE ||
					protection == PAGE_WRITECOPY || protection == PAGE_EXECUTE_READ ||
					protection == PAGE_EXECUTE_READWRITE || protection == PAGE_EXECUTE_WRITECOPY;
				const auto region_end = reinterpret_cast<std::uintptr_t>( mbi.BaseAddress ) + mbi.RegionSize;
				if ( !readable || region_end <= current )
					return false;

				current = ( std::min )( end, region_end );
			}

			return true;
		}

		struct render_owner_cache_entry
		{
			std::uintptr_t owner{};
			std::uint32_t schema_hash{};
			std::uint32_t owner_value{ 0xffffffffu };
			std::size_t owner_offset{ 0x178 };
			std::chrono::steady_clock::time_point expires{};
		};

		struct render_entity_cache_entry
		{
			std::uintptr_t entity{};
			std::uint32_t schema_hash{};
			std::chrono::steady_clock::time_point expires{};
		};

		// GeneratePrimitives is called for every scene draw. Keep its worker-local
		// cache in FLS so manual mapping does not depend on static TLS callbacks.
		struct render_thread_cache
		{
			std::unordered_map<std::uintptr_t, render_owner_cache_entry> owner_cache{};
			bool owner_cache_reserved{};
			std::array<std::uintptr_t, 8192> owner_cache_order{};
			std::size_t owner_cache_cursor{};
			std::unordered_map<std::uint32_t, render_entity_cache_entry> entity_cache{};
			std::array<std::uint32_t, 512> entity_cache_order{};
			std::size_t entity_cache_cursor{};
			std::chrono::steady_clock::time_point chams_refresh{};
			bool chams_active{};
		};
		inline utilities::tls::local<render_thread_cache> g_render_thread_cache{};
		inline std::atomic_bool g_render_owner_index_logged{};
		inline std::atomic_bool g_render_owner_miss_logged{};

		[[nodiscard]] inline bool is_player_pawn_hash( std::uint32_t hash ) noexcept
		{
			return hash == "C_CSPlayerPawn"_hash || hash == "C_CSPlayerPawnBase"_hash ||
				hash == "C_BasePlayerPawn"_hash || hash == "C_CSGO_PreviewPlayer"_hash ||
				hash == "C_CSGO_PreviewPlayerAlias_csgo_player_previewmodel"_hash ||
				hash == "C_CSGO_TeamPreviewModel"_hash;
		}

		[[nodiscard]] inline bool is_renderable_owner_hash( std::uint32_t hash )
		{
			return is_player_pawn_hash( hash ) || hash == "C_CS2HudModelArms"_hash ||
				hash == "C_CS2HudModelWeapon"_hash ||
				features::esp::item::g_chams.get_item_group( hash ) != UINT32_MAX;
		}

		[[nodiscard]] inline bool is_viewmodel_owner_hash( std::uint32_t hash ) noexcept
		{
			return hash == "C_CS2HudModelArms"_hash || hash == "C_CS2HudModelWeapon"_hash;
		}

		struct render_owner_info
		{
			std::uintptr_t entity{};
			std::uint32_t schema_hash{};
			std::uint32_t owner_value{ 0xffffffffu };
			std::size_t owner_offset{ 0x178 };
		};

		// Pawns are intentionally absent from the gameplay entity cache: that cache's
		// player entries are controllers. Validate an uncached entity through its live
		// identity handle before walking its schema chain.
		[[nodiscard]] inline std::optional<std::uint32_t> get_render_owner_schema_hash( std::uintptr_t entity )
		{
			if ( !valid_runtime_pointer( entity ) || !readable_runtime_range( entity, 0x18 ) )
				return std::nullopt;

			const auto identity = memory::safe_read<std::uintptr_t>( entity + 0x10 ).value_or( 0 );
			if ( !valid_runtime_pointer( identity ) || !readable_runtime_range( identity, 0x18 ) )
				return std::nullopt;

			const auto handle = memory::safe_read<std::uint32_t>( identity + 0x10 ).value_or( 0 );
			if ( !handle || handle == 0xffffffffu || systems::g_entities.lookup( handle ) != entity )
				return std::nullopt;

			if ( const auto cached_hash = systems::g_entities.get_cached_schema_hash( entity );
				cached_hash && *cached_hash )
			{
				if ( !is_renderable_owner_hash( *cached_hash ) )
					return std::nullopt;
				return cached_hash;
			}

			const auto schema_name = systems::g_entities.get_schema_name( entity );
			if ( !schema_name )
				return std::nullopt;

			const auto schema_hash = fnv1a::runtime_hash( schema_name );
			if ( !is_renderable_owner_hash( schema_hash ) )
				return std::nullopt;

			return schema_hash;
		}

		[[nodiscard]] inline render_owner_info resolve_render_owner_entity(
			std::uint32_t owner_value, std::size_t owner_offset, std::chrono::steady_clock::time_point now )
		{
			if ( !owner_value || owner_value == 0xffffffffu )
				return {};

			const auto entity_index = static_cast<std::int32_t>( owner_value & 0x7fffu );
			std::uintptr_t entity{};
			if ( owner_value <= 0x7fffu )
			{
				entity = systems::g_entities.get_by_index( entity_index );
			}
			else
			{
				entity = systems::g_entities.lookup( owner_value );
				if ( !entity )
					entity = systems::g_entities.get_by_index( entity_index );
			}

			if ( !valid_runtime_pointer( entity ) )
				return {};

			auto* thread_cache = g_render_thread_cache.get( );
			if ( !thread_cache )
				return {};
			auto& g_render_entity_cache = thread_cache->entity_cache;
			auto& g_render_entity_cache_order = thread_cache->entity_cache_order;
			auto& g_render_entity_cache_cursor = thread_cache->entity_cache_cursor;

			if ( const auto cached = g_render_entity_cache.find( owner_value );
				cached != g_render_entity_cache.end( ) && cached->second.entity == entity &&
				now < cached->second.expires && cached->second.schema_hash )
			{
				return { entity, cached->second.schema_hash, owner_value, owner_offset };
			}

			const auto schema_hash = get_render_owner_schema_hash( entity );
			if ( !schema_hash || !*schema_hash )
				return {};

			constexpr std::size_t cache_capacity = 512;
			const auto cached = g_render_entity_cache.find( owner_value );
			if ( cached != g_render_entity_cache.end( ) )
			{
				cached->second = { entity, *schema_hash, now + std::chrono::milliseconds( 250 ) };
			}
			else
			{
				if ( g_render_entity_cache.size( ) >= cache_capacity )
				{
					const auto evicted = g_render_entity_cache_order[ g_render_entity_cache_cursor ];
					if ( evicted )
						g_render_entity_cache.erase( evicted );
				}
				g_render_entity_cache_order[ g_render_entity_cache_cursor ] = owner_value;
				g_render_entity_cache_cursor = ( g_render_entity_cache_cursor + 1 ) % cache_capacity;
				g_render_entity_cache.emplace( owner_value,
					render_entity_cache_entry{ entity, *schema_hash, now + std::chrono::milliseconds( 250 ) } );
			}

			return { entity, *schema_hash, owner_value, owner_offset };
		}

		// CSceneAnimatableObject::m_hOwnerIndex is a 15-bit entity slot on some
		// builds and a serial-bearing handle on others. Resolve known owner fields
		// only: probing every aligned dword in a render object performs hundreds
		// of entity/schema lookups per miss and can stall the render thread.
		[[nodiscard]] inline render_owner_info resolve_render_owner_uncached(
			std::uintptr_t scene_object, std::chrono::steady_clock::time_point now )
		{
			// Each owner field is guarded below. Querying the full object pages on
			// every uncached model adds a costly kernel transition to this hot path.
			if ( !valid_runtime_pointer( scene_object ) )
				return {};

			render_owner_info owner{};
			std::uint32_t last_owner_value{ 0xffffffffu };
			std::size_t last_owner_offset{ 0x178 };
			const auto resolve_owner_value = [ & ]( std::size_t offset )
			{
				const auto value = memory::safe_read<std::uint32_t>( scene_object + offset ).value_or( 0xffffffffu );
				last_owner_value = value;
				last_owner_offset = offset;
				if ( !value || value == 0xffffffffu )
					return render_owner_info{};
				return resolve_render_owner_entity( value, offset, now );
			};
			const auto log_owner = [ & ]( const render_owner_info& resolved )
			{
				if ( !g_render_owner_index_logged.exchange( true, std::memory_order_relaxed ) )
				{
					const auto entity_index = static_cast<std::int32_t>( resolved.owner_value & 0x7fffu );
					diag::writef( diag::level::info,
						"[chams-owner] render owner resolved at scene+0x%zX value=0x%X index=0x%X entity=%p schema=0x%08X",
						resolved.owner_offset, resolved.owner_value, static_cast<unsigned>( entity_index ),
						reinterpret_cast<void*>( resolved.entity ), resolved.schema_hash );
				}
			};

			// A viewmodel scene can expose its local pawn at +0x178 and the HUD
			// model entity at +0xc0. Prefer the model entity when both handles are
			// valid, or arms/weapon chams get routed through player chams.
			const auto current_owner = resolve_owner_value( 0x178 );
			if ( is_viewmodel_owner_hash( current_owner.schema_hash ) )
			{
				log_owner( current_owner );
				return current_owner;
			}

			// The legacy field is only needed when the current field is absent or
			// resolves to the carrying pawn. Avoid a second entity/schema lookup for
			// ordinary player and item scene objects on this hot path.
			const auto should_probe_legacy_owner = !current_owner.entity || is_player_pawn_hash( current_owner.schema_hash );
			const auto legacy_owner = should_probe_legacy_owner ? resolve_owner_value( 0xc0 ) : render_owner_info{};
			if ( is_viewmodel_owner_hash( legacy_owner.schema_hash ) )
			{
				log_owner( legacy_owner );
				return legacy_owner;
			}

			owner = current_owner.entity ? current_owner : legacy_owner;
			if ( owner.entity )
			{
				log_owner( owner );
				return owner;
			}

			if ( !g_render_owner_miss_logged.exchange( true, std::memory_order_relaxed ) )
			{
				constexpr std::size_t diagnostic_owner_offset = 0x178;
				const auto owner_value = memory::safe_read<std::uint32_t>( scene_object + diagnostic_owner_offset ).value_or( 0xffffffffu );
				const auto owner_index = owner_value == 0xffffffffu ? -1 : static_cast<int>( owner_value & 0x7fffu );
				const auto indexed_entity = owner_index >= 0 ? systems::g_entities.get_by_index( owner_index ) : 0;
				const auto indexed_schema = indexed_entity ? get_render_owner_schema_hash( indexed_entity ) : std::nullopt;
				diag::writef( diag::level::warning,
					"[chams-owner] no renderable owner scene=%p offset=0x%zX owner_value=0x%08X index=%d slot_entity=%p slot_schema=0x%08X",
					reinterpret_cast<void*>( scene_object ), diagnostic_owner_offset, owner_value, owner_index,
					reinterpret_cast<void*>( indexed_entity ), indexed_schema.value_or( 0 ) );
			}

			return { 0, 0, last_owner_value, last_owner_offset };
		}

		[[nodiscard]] inline render_owner_info resolve_render_owner( std::uintptr_t scene_object )
		{
			if ( !valid_runtime_pointer( scene_object ) )
				return {};

			auto* thread_cache = g_render_thread_cache.get( );
			if ( !thread_cache )
				return {};
			auto& g_render_owner_cache = thread_cache->owner_cache;
			auto& g_render_owner_cache_reserved = thread_cache->owner_cache_reserved;
			auto& g_render_owner_cache_order = thread_cache->owner_cache_order;
			auto& g_render_owner_cache_cursor = thread_cache->owner_cache_cursor;

			constexpr std::size_t cache_capacity = 8192;

			const auto now = std::chrono::steady_clock::now();
			if ( !g_render_owner_cache_reserved )
			{
				g_render_owner_cache.reserve(cache_capacity);
				g_render_owner_cache_reserved = true;
			}
			auto cached = g_render_owner_cache.find( scene_object );
			if ( cached != g_render_owner_cache.end( ) )
			{
				if ( now < cached->second.expires )
				{
					return { cached->second.owner, cached->second.schema_hash,
						cached->second.owner_value, cached->second.owner_offset };
				}

				// Scene objects are queried every render pass. Validate the owner
				// handle only when the cache expires instead of safe-reading it on
				// every draw for every world object.
				const auto current_owner_value = memory::safe_read<std::uint32_t>(
					scene_object + cached->second.owner_offset ).value_or( 0xffffffffu );
				if ( current_owner_value == cached->second.owner_value )
				{
					cached->second.expires = now + std::chrono::milliseconds( 250 );
					return { cached->second.owner, cached->second.schema_hash,
						cached->second.owner_value, cached->second.owner_offset };
				}
			}

			const auto owner = resolve_render_owner_uncached( scene_object, now );
			if ( cached != g_render_owner_cache.end( ) )
			{
				cached->second = { owner.entity, owner.schema_hash, owner.owner_value,
					owner.owner_offset, now + std::chrono::milliseconds(250) };

			}
			else
			{
				
				if ( g_render_owner_cache.size( ) >= cache_capacity )
				{
					const auto evicted = g_render_owner_cache_order[ g_render_owner_cache_cursor ];
					if ( evicted )
						g_render_owner_cache.erase( evicted );
				}
				g_render_owner_cache_order[ g_render_owner_cache_cursor ] = scene_object;
				g_render_owner_cache_cursor = ( g_render_owner_cache_cursor + 1 ) % cache_capacity;
				g_render_owner_cache.emplace( scene_object,
					render_owner_cache_entry{ owner.entity, owner.schema_hash, owner.owner_value,
						owner.owner_offset, now + std::chrono::milliseconds(250) });
			}
			return owner;
		}

		[[nodiscard]] inline bool has_active_chams( const settings::esp::chams_config& cfg )
		{
			return cfg.enabled.value && ( cfg.primary.enabled.value || cfg.secondary.enabled.value || cfg.overlay.enabled.value );
		}

		[[nodiscard]] inline bool scan_render_chams_active( )
		{
			const auto& esp = settings::g_esp;
			const auto& player = esp.m_player.m_chams;
			if ( has_active_chams( player.enemy ) || has_active_chams( player.team ) ||
				has_active_chams( player.local ) || has_active_chams( player.enemy_ragdoll ) ||
				has_active_chams( player.team_ragdoll ) || has_active_chams( player.local_ragdoll ) ||
				has_active_chams( player.backtrack ) || has_active_chams( player.onshot ) )
				return true;

			if ( has_active_chams( esp.m_viewmodel.arms ) || has_active_chams( esp.m_viewmodel.weapon ) )
				return true;
			for ( const auto& weapon : esp.m_viewmodel.individual.weapons )
				if ( weapon.override_default.value && has_active_chams( weapon.cfg ) )
					return true;

			const auto& item = esp.m_item.m_chams;
			if ( item.enabled.value )
			{
				for ( std::uint32_t group = 0; group < item.groups.size( ); ++group )
					if ( item.is_active( group ) && has_active_chams( item.groups[ group ] ) )
						return true;
				for ( const auto& weapon : item.individual.weapons )
					if ( weapon.override_default.value && has_active_chams( weapon.cfg ) )
						return true;
			}

			return false;
		}

		[[nodiscard]] inline bool any_render_chams_active( )
		{
			static constexpr auto refresh_interval = std::chrono::milliseconds( 100 );
			auto* thread_cache = g_render_thread_cache.get( );
			if ( !thread_cache )
				return scan_render_chams_active( );

			const auto now = std::chrono::steady_clock::now( );
			if ( now < thread_cache->chams_refresh )
				return thread_cache->chams_active;

			thread_cache->chams_active = scan_render_chams_active( );
			thread_cache->chams_refresh = now + refresh_interval;
			return thread_cache->chams_active;
		}

		// Preview scenes are engine-owned. Do not automatically apply loadout
		// overrides to lobby/party or detached presentation models. Actual match
		// pawns use the frame-stage changers; explicit Inspect is separate.

		// Isolated SEH frame: no std::string/optional destructors here (MSVC C2712).
		bool dispatch_lobby_music_guarded( std::uintptr_t stop, std::uintptr_t update, const char* name )
		{
			if ( !stop || !update ) return false;
			__try
			{
				const auto* p = reinterpret_cast<const std::uint8_t*>( stop );
				// These offsets come from stop_item_preview_music's existing signature.
				// Never decode an unrelated instruction after a game update.
				if ( p[4] != 0xe8 || p[24] != 0xe9 ) return false;
				const auto get_manager = stop + 9 + *reinterpret_cast<const std::int32_t*>( p + 5 );
				const auto set_background = stop + 29 + *reinterpret_cast<const std::int32_t*>( p + 25 );
				void* manager = memory::call<void*>( get_manager );
				if ( !manager ) return false;
				memory::call<void>( stop );
				if ( name && *name ) {
					memory::call<void>( set_background, manager, name, static_cast<const char*>( nullptr ), 0.0f );
				}
				// Standard music also needs an update after stopping the override.
				memory::call<void>( update, manager );
				return true;
			}
			__except ( EXCEPTION_EXECUTE_HANDLER ) { return false; }
		}

		using present_fn = HRESULT( __fastcall* )( IDXGISwapChain*, UINT, UINT );
		using resize_fn = HRESULT( __fastcall* )(
			IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT );

		inline present_fn g_original_present{};
		inline resize_fn g_original_resize_buffers{};
		inline std::uintptr_t g_patched_vtable{};
		constexpr std::size_t k_swapchain_vtable_slots{ 14 };
		// Present and ResizeBuffers touch the same D3D11 resources. DXGI may call
		// them from different threads during a mode/map transition.
		inline std::recursive_mutex g_swapchain_mutex{};

		bool install_swapchain_vtable( IDXGISwapChain* swap_chain )
		{
			if ( !swap_chain )
				return false;

			auto* const vtable = *reinterpret_cast<void***>( swap_chain );
			if ( !vtable )
				return false;

			if ( !g_original_present )
				g_original_present = reinterpret_cast<present_fn>( vtable[ 8 ] );
			if ( !g_original_resize_buffers )
				g_original_resize_buffers = reinterpret_cast<resize_fn>( vtable[ 13 ] );

			if ( !g_original_present || !g_original_resize_buffers )
				return false;

			const auto present_detour = reinterpret_cast<std::uintptr_t>( &cheat::present );
			const auto resize_detour = reinterpret_cast<std::uintptr_t>( &cheat::resize_buffers );
			if ( vtable[ 8 ] == reinterpret_cast<void*>( present_detour ) &&
				vtable[ 13 ] == reinterpret_cast<void*>( resize_detour ) )
			{
				g_patched_vtable = reinterpret_cast<std::uintptr_t>( vtable );
				return true;
			}

			DWORD old_protect{};
			const auto bytes = sizeof( void* ) * k_swapchain_vtable_slots;
			if ( !VirtualProtect( vtable, bytes, PAGE_READWRITE, &old_protect ) )
				return false;

			vtable[ 13 ] = reinterpret_cast<void*>( resize_detour );
			vtable[ 8 ] = reinterpret_cast<void*>( present_detour );
			VirtualProtect( vtable, bytes, old_protect, &old_protect );
			FlushInstructionCache( GetCurrentProcess(), vtable, bytes );
			g_patched_vtable = reinterpret_cast<std::uintptr_t>( vtable );

			diag::writef(
				diag::level::info,
				"DXGI swap-chain vtable patched: vtable=%p present=%p resize=%p",
				vtable,
				reinterpret_cast<void*>( g_original_present ),
				reinterpret_cast<void*>( g_original_resize_buffers ) );
			return true;
		}

		bool is_vtable_patched( IDXGISwapChain* swap_chain )
		{
			return swap_chain &&
				*reinterpret_cast<void***>( swap_chain ) ==
				reinterpret_cast<void**>( g_patched_vtable );
		}
		void uninstall_swapchain_vtable( )
		{
			if ( !g_patched_vtable || !g_original_present || !g_original_resize_buffers )
				return;

			auto* const vtable = reinterpret_cast<void**>( g_patched_vtable );
			DWORD old_protect{};
			const auto bytes = sizeof( void* ) * k_swapchain_vtable_slots;
			if ( VirtualProtect( vtable, bytes, PAGE_READWRITE, &old_protect ) )
			{
				vtable[ 13 ] = reinterpret_cast<void*>( g_original_resize_buffers );
				vtable[ 8 ] = reinterpret_cast<void*>( g_original_present );
				VirtualProtect( vtable, bytes, old_protect, &old_protect );
				FlushInstructionCache( GetCurrentProcess(), vtable, bytes );
			}
			g_patched_vtable = 0;
		}



		// SEH-safe swap chain validation: returns true if the swap chain is valid.
		// Must be in a separate function without C++ destructors (MSVC C2712).
		bool is_swap_chain_valid( IDXGISwapChain* swap_chain )
		{
			if ( !swap_chain )
				return false;

			// Check vtable pointer
			__try
			{
				if ( !*reinterpret_cast<std::uintptr_t*>( swap_chain ) )
					return false;
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				return false;
			}

			// Check device pointer - a destroyed swap chain may have valid vtable but NULL device
			ID3D11Device* device = nullptr;
			__try
			{
				if ( FAILED( swap_chain->GetDevice( __uuidof( ID3D11Device ), reinterpret_cast< void** >( &device ) ) ) || !device )
					return false;
				device->Release( );
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				return false;
			}

			return true;
		}

		// SEH-safe original Present call: wraps the trampoline call in SEH to catch
		// any access violations inside the original Present function.
		// Must be in a separate function without C++ destructors (MSVC C2712).
		HRESULT call_present_safe( hooking::jmp& hook, IDXGISwapChain* thisptr, UINT sync_interval, UINT flags )
		{
			__try
			{
				return hook.call<HRESULT>( thisptr, sync_interval, flags );
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				// Mark Present as permanently broken so we never call it again
				extern bool g_present_permanently_broken;
				g_present_permanently_broken = true;
				return E_FAIL;
			}
		}

		// Global flag: set to true if original Present ever crashes
		bool g_present_permanently_broken = false;

		struct viewmodel_anim_state
		{
			bool initialized{ false };
			float current_x{ 0.0f };
			float current_y{ 0.0f };
			float current_z{ 0.0f };
			float current_fov{ 68.0f };
			float sway_x{ 0.0f };
			float sway_z{ 0.0f };
			math::vector3 last_view_angles{};
			std::chrono::steady_clock::time_point last_time{};
		};

		inline viewmodel_anim_state g_vm_anim{};

		struct shadow_state_t {
			bool saved{ false };
			bool orig_r_shadows{ true };
			bool orig_lb_shadow_casting{ true };
			bool orig_lb_baked_shadows{ true };
			float orig_csm_max_dist{ -1.0f };
			bool orig_csm_vm_shadows{ true };
			bool orig_microshadowing{ true };
			bool orig_particle_shadows{ true };
			bool orig_smoke_shadow{ true };

			void apply_fullbright( )
			{
				const auto cvar_shadows = CONVAR( "r_shadows" );
				const auto cvar_lb_casting = CONVAR( "lb_enable_shadow_casting" );
				const auto cvar_lb_baked = CONVAR( "lb_enable_baked_shadows" );
				const auto cvar_csm_dist = CONVAR( "csm_max_shadow_dist_override" );
				const auto cvar_csm_vm = CONVAR( "csm_viewmodel_shadows" );
				const auto cvar_micro = CONVAR( "r_csgo_microshadowing" );
				const auto cvar_particle = CONVAR( "r_particle_shadows" );
				const auto cvar_smoke = CONVAR( "r_csgo_smoke_shadow" );

				if ( !saved )
				{
					if ( cvar_shadows ) orig_r_shadows = cvar_shadows->m_value.i1;
					if ( cvar_lb_casting ) orig_lb_shadow_casting = cvar_lb_casting->m_value.i1;
					if ( cvar_lb_baked ) orig_lb_baked_shadows = cvar_lb_baked->m_value.i1;
					if ( cvar_csm_dist ) orig_csm_max_dist = cvar_csm_dist->m_value.fl;
					if ( cvar_csm_vm ) orig_csm_vm_shadows = cvar_csm_vm->m_value.i1;
					if ( cvar_micro ) orig_microshadowing = cvar_micro->m_value.i1;
					if ( cvar_particle ) orig_particle_shadows = cvar_particle->m_value.i1;
					if ( cvar_smoke ) orig_smoke_shadow = cvar_smoke->m_value.i1;
					saved = true;
				}

				if ( cvar_shadows ) { cvar_shadows->m_value.i1 = false; ++cvar_shadows->m_change_count; }
				if ( cvar_lb_casting ) { cvar_lb_casting->m_value.i1 = false; ++cvar_lb_casting->m_change_count; }
				if ( cvar_lb_baked ) { cvar_lb_baked->m_value.i1 = false; ++cvar_lb_baked->m_change_count; }
				if ( cvar_csm_dist ) { cvar_csm_dist->m_value.fl = 0.0f; ++cvar_csm_dist->m_change_count; }
				if ( cvar_csm_vm ) { cvar_csm_vm->m_value.i1 = false; ++cvar_csm_vm->m_change_count; }
				if ( cvar_micro ) { cvar_micro->m_value.i1 = false; ++cvar_micro->m_change_count; }
				if ( cvar_particle ) { cvar_particle->m_value.i1 = false; ++cvar_particle->m_change_count; }
				if ( cvar_smoke ) { cvar_smoke->m_value.i1 = false; ++cvar_smoke->m_change_count; }
			}

			void restore( )
			{
				if ( !saved )
					return;

				const auto cvar_shadows = CONVAR( "r_shadows" );
				const auto cvar_lb_casting = CONVAR( "lb_enable_shadow_casting" );
				const auto cvar_lb_baked = CONVAR( "lb_enable_baked_shadows" );
				const auto cvar_csm_dist = CONVAR( "csm_max_shadow_dist_override" );
				const auto cvar_csm_vm = CONVAR( "csm_viewmodel_shadows" );
				const auto cvar_micro = CONVAR( "r_csgo_microshadowing" );
				const auto cvar_particle = CONVAR( "r_particle_shadows" );
				const auto cvar_smoke = CONVAR( "r_csgo_smoke_shadow" );

				if ( cvar_shadows ) { cvar_shadows->m_value.i1 = orig_r_shadows; ++cvar_shadows->m_change_count; }
				if ( cvar_lb_casting ) { cvar_lb_casting->m_value.i1 = orig_lb_shadow_casting; ++cvar_lb_casting->m_change_count; }
				if ( cvar_lb_baked ) { cvar_lb_baked->m_value.i1 = orig_lb_baked_shadows; ++cvar_lb_baked->m_change_count; }
				if ( cvar_csm_dist ) { cvar_csm_dist->m_value.fl = orig_csm_max_dist; ++cvar_csm_dist->m_change_count; }
				if ( cvar_csm_vm ) { cvar_csm_vm->m_value.i1 = orig_csm_vm_shadows; ++cvar_csm_vm->m_change_count; }
				if ( cvar_micro ) { cvar_micro->m_value.i1 = orig_microshadowing; ++cvar_micro->m_change_count; }
				if ( cvar_particle ) { cvar_particle->m_value.i1 = orig_particle_shadows; ++cvar_particle->m_change_count; }
				if ( cvar_smoke ) { cvar_smoke->m_value.i1 = orig_smoke_shadow; ++cvar_smoke->m_change_count; }

				saved = false;
			}
		};

		inline shadow_state_t g_shadow_state{};
		inline bool g_was_fullbright{ false };

		inline void update_fullbright_shadows( )
		{
			const bool is_fullbright = settings::g_world.m_scene.fullbright.value;
			if ( is_fullbright != g_was_fullbright )
			{
				if ( is_fullbright )
					g_shadow_state.apply_fullbright( );
				else
					g_shadow_state.restore( );

				g_was_fullbright = is_fullbright;
			}
		}

		inline void reset_fullbright_shadows( )
		{
			g_shadow_state.restore( );
			g_was_fullbright = false;
		}
	}

	bool cheat::initialize () {
		m_level_shutting_down.store( false, std::memory_order_release );
		m_was_connected = false;
		m_seen_disconnected = false;
		diag::writef(
			diag::level::info,
			"DXGI bootstrap target: get_desc=%p present=%p resize=%p",
			reinterpret_cast<void*>( addresses::functions::get_desc ),
			reinterpret_cast<void*>( addresses::functions::present ),
			reinterpret_cast<void*>( addresses::functions::resize_buffers ) );
		if (!hooking::manager::create ({
			{ &m_get_desc, &get_desc, xs ("get_desc"), addresses::functions::get_desc }
			})) {
			return false;
		}
		diag::writef(
			diag::level::info,
			"DXGI bootstrap hook created: target=%p len=%zu trampoline=%p",
			m_get_desc.get_target(),
			m_get_desc.get_original_length(),
			m_get_desc.get_trampoline() );

		const hooking::manager::entry feature_hooks[] {
			{ &m_cmd_interpreter, &cmd_interpreter, "cmd_interpreter", PATTERN (patterns::cmd_interpreter) },
			{ &m_frame_stage_notify, &frame_stage_notify, "frame_stage_notify", PATTERN (patterns::frame_stage_notify) },
			{ &m_create_move, &create_move, "create_move", PATTERN (patterns::create_move) },
			{ &m_handle_view_angles, &handle_view_angles, "handle_view_angles", PATTERN (patterns::handle_view_angles) },
			{ &m_add_entity, &add_entity, "add_entity", PATTERN (patterns::add_entity) },
			{ &m_remove_entity, &remove_entity, "remove_entity", PATTERN (patterns::remove_entity) },
			{ &m_render_view, &render_view, "render_view", PATTERN (patterns::render_view) },
			{ &m_draw_skybox_array, &draw_skybox_array, "draw_skybox_array", PATTERN (patterns::draw_skybox_array) },
			{ &m_light_scene_object, &light_scene_object, "light_scene_object", PATTERN (patterns::light_scene_object) },
			{ &m_draw_scene_object, &draw_scene_object, "draw_scene_object", PATTERN (patterns::draw_scene_object) },
			{ &m_is_glowing, &is_glowing, "is_glowing", PATTERN (patterns::is_glowing) },
			{ &m_get_glow_color, &get_glow_color, "get_glow_color", PATTERN (patterns::get_glow_color) },
			{ &m_generate_primitives, &generate_primitives, "generate_primitives", PATTERN (patterns::generate_primitives) },
			{ &m_generate_animatable_primitives, &generate_animatable_primitives, "generate_animatable_primitives", PATTERN (patterns::generate_animatable_primitives) },
			{ &m_parse_report_hit, &parse_report_hit, "parse_report_hit", PATTERN (patterns::parse_report_hit) },
			{ &m_vote_start, &vote_start, "vote_start", PATTERN (patterns::vote_start) },
			{ &m_vote_pass, &vote_pass, "vote_pass", PATTERN (patterns::vote_pass) },
			{ &m_vote_failed, &vote_failed, "vote_failed", PATTERN (patterns::vote_failed) },
			{ &m_panorama_event, &panorama_event, "panorama_event", PATTERN (patterns::panorama_event) },
			{ &m_setup_fog, &setup_fog, "setup_fog", PATTERN (patterns::setup_fog) },
			{ &m_set_shader_param, &set_shader_param, "set_shader_param", PATTERN (patterns::set_shader_param) },
			{ &m_set_postprocess_vec, &set_postprocess_vec, "set_postprocess_vec", PATTERN (patterns::set_postprocess_vec) },
			{ &m_override_view, &override_view, "override_view", PATTERN (patterns::override_view) },
			{ &m_update_fov_sensitivity, &update_fov_sensitivity, "update_fov_sensitivity", PATTERN (patterns::update_fov_sensitivity) },
			{ &m_render_scope, &render_scope, "render_scope", PATTERN (patterns::render_scope) },
			{ &m_render_crosshair, &render_crosshair, "render_crosshair", PATTERN (patterns::render_crosshair) },
			{ &m_prepare_scene_material, &prepare_scene_material, "prepare_scene_material", PATTERN (patterns::prepare_scene_material) },
			{ &m_post_network_data_received, &post_network_data_received, "post_network_data_received", PATTERN (patterns::post_network_data_received) },
			{ &m_draw_overhead, &draw_overhead, "draw_overhead", PATTERN (patterns::draw_overhead) },
			{ &m_draw_legs, &draw_legs, "draw_legs", PATTERN (patterns::draw_legs) },
			{ &m_get_transforms_for_hitbox_list, &get_transforms_for_hitbox_list, "get_transforms_for_hitbox_list", PATTERN (patterns::get_transforms_for_hitbox_list) },
			{ &m_sort_primitives, &sort_primitives, "sort_primitives", PATTERN (patterns::sort_primitives) },
			{ &m_get_interpolated_shoot_position, &get_interpolated_shoot_position, "get_interpolated_shoot_position", PATTERN (patterns::get_interpolated_shoot_position) },
			{ &m_level_initialization, &level_initialization, "level_initialization", PATTERN (patterns::level_initialization) },
			{ &m_level_shutdown, &level_shutdown, "level_shutdown", PATTERN (patterns::level_shutdown) },
			{ &m_read_frame_input, &read_frame_input, "read_frame_input", PATTERN (patterns::read_frame_input) },
			{ &m_process_input_event, &process_input_event, "process_input_event", PATTERN (patterns::process_input_event) },
			{ &m_render_decals, &render_decals, "render_decals", PATTERN (patterns::render_decals) },
			{ &m_render_smoke, &render_smoke, "render_smoke", PATTERN (patterns::render_smoke) },
			{ &m_draw_flash_effect, &draw_flash_effect, "draw_flash_effect", PATTERN (patterns::draw_flash_effect) },
			{ &m_set_info, &set_info, "set_info", PATTERN (patterns::set_info) },
			{ &m_calculate_viewmodel, &calculate_viewmodel, "calculate_viewmodel", PATTERN (patterns::calculate_viewmodel) },
			{ &m_spec_cmds_handler, &spec_cmds_handler, "spec_cmds_handler", PATTERN (patterns::spec_cmds_handler) },
			{ &m_collect_attached_entities, &collect_attached_entities, "collect_attached_entities", PATTERN (patterns::collect_attached_entities) },
			{ &m_play_music, &play_music, "play_music", PATTERN (patterns::play_music) }
		};

		auto unavailable_hooks = 0u;
		for (const auto& entry : feature_hooks) {
			if (!hooking::manager::create ({ entry })) {
				++unavailable_hooks;
				logging::console::print (xs ("skipping unavailable hook: {}"), entry.name);
			}
		}

		features::changer::g_inspect_preview.set_available(m_frame_stage_notify.is_enabled());
		systems::g_model_preview.set_capture_available(m_frame_stage_notify.is_enabled());
		features::changer::preview_scene::set_lifecycle_observed(
            m_add_entity.is_enabled() && m_remove_entity.is_enabled());
		diag::writef( diag::level::info,
			"[hooks] generate_primitives base=%d target=%p trampoline=%p animatable=%d target=%p trampoline=%p",
			static_cast<int>( m_generate_primitives.is_enabled( ) ),
			reinterpret_cast<void*>( m_generate_primitives.get_target( ) ),
			reinterpret_cast<void*>( m_generate_primitives.get_trampoline( ) ),
			static_cast<int>( m_generate_animatable_primitives.is_enabled( ) ),
			reinterpret_cast<void*>( m_generate_animatable_primitives.get_target( ) ),
			reinterpret_cast<void*>( m_generate_animatable_primitives.get_trampoline( ) ) );

		if (unavailable_hooks) {
			logging::console::print (
				xs ("feature hooks initialized with {} unavailable"),
				unavailable_hooks);
		}

		return true;
	}

	void cheat::shutdown( )
	{
		detail::uninstall_swapchain_vtable( );
		m_wnd_proc.reset( );
		m_wnd_proc.reset( );
		m_get_desc.reset( );
		m_present.reset( );
		m_resize_buffers.reset( );
		m_cmd_interpreter.reset( );
		m_frame_stage_notify.reset( );
		m_create_move.reset( );
		m_handle_view_angles.reset( );
		m_add_entity.reset( );
		m_remove_entity.reset( );
		m_render_view.reset( );
		m_draw_skybox_array.reset( );
		m_light_scene_object.reset( );
		m_draw_scene_object.reset( );
		m_is_glowing.reset( );
		m_get_glow_color.reset( );
		m_generate_primitives.reset( );
		m_generate_animatable_primitives.reset( );
		m_preview_resource_view.reset( );
		systems::g_model_preview.set_capture_available(false);
		m_parse_report_hit.reset( );
		m_vote_start.reset( );
		m_vote_pass.reset( );
		m_vote_failed.reset( );
		m_panorama_event.reset( );
		m_setup_fog.reset( );
		m_set_shader_param.reset( );
		m_set_postprocess_vec.reset( );
		m_override_view.reset( );
		m_update_fov_sensitivity.reset( );
		m_render_scope.reset( );
		m_render_crosshair.reset( );
		m_prepare_scene_material.reset( );
		m_post_network_data_received.reset( );
		m_draw_overhead.reset( );
		m_draw_legs.reset( );
		m_get_transforms_for_hitbox_list.reset( );
		m_sort_primitives.reset( );
		m_get_inaccuracy.reset( );
		m_get_interpolated_shoot_position.reset( );
		m_level_initialization.reset( );
		m_level_shutdown.reset( );
		m_read_frame_input.reset( );
		m_process_input_event.reset( );
		m_render_decals.reset( );
		m_render_smoke.reset( );
		m_render_smoke_map.reset( );
		m_render_smoke_unmap.reset( );
		m_draw_flash_effect.reset( );
		m_set_info.reset( );
		m_calculate_viewmodel.reset( );
		m_spec_cmds_handler.reset( );
		m_collect_attached_entities.reset( );
		m_play_music.reset( );
		detail::reset_fullbright_shadows( );
	}

	HRESULT __fastcall cheat::get_desc( IDXGISwapChain* thisptr, DXGI_SWAP_CHAIN_DESC* desc )
	{
		diag::exception_scope scope{ "get_desc" };
		const auto result = m_get_desc.call<HRESULT>( thisptr, desc );
		if ( SUCCEEDED( result ) )
		{
			detail::install_swapchain_vtable( thisptr );
		}
		return result;
	}

	namespace detail { void autosave_setting_changes( ); }

	HRESULT __fastcall cheat::present( IDXGISwapChain* thisptr, UINT sync_interval, UINT flags )
	{
		diag::exception_scope scope{ "present" };
		const std::lock_guard lock( detail::g_swapchain_mutex );
		if ( lifecycle::is_unloading( ) || !thisptr || !detail::g_original_present )
		{
			return detail::g_original_present
				? detail::g_original_present( thisptr, sync_interval, flags )
				: E_FAIL;
		}

		// Render overlays before the real Present. The real Present must run on
		// every frame: returning S_OK without calling it breaks DXGI frame
		// pacing and leaves CS2 waiting on a frame that was never submitted.
		rendering::g_context.on_present( thisptr );
		detail::autosave_setting_changes( );
		features::misc::g_auto_accept.run( );

		if ( !memory::safe_read<std::uintptr_t>( addresses::globals::local_player_controller ).value_or( 0 ) )
		{
			const auto configured = settings::g_changer.music.id;
			trigger_lobby_music( configured > 0 && configured < 0xffff
				? static_cast<std::uint16_t>( configured ) : 0 );
		}

		if ( !m_wnd_proc.is_enabled( ) && rendering::g_context.get_window( ) )
		{
			if ( m_wnd_proc.create( reinterpret_cast< void* >( GetWindowLongPtrW( rendering::g_context.get_window( ), GWLP_WNDPROC ) ), &wnd_proc ) )
			{
				m_wnd_proc.enable( );
			}
		}

		return detail::g_original_present( thisptr, sync_interval, flags );
	}


	HRESULT __fastcall cheat::resize_buffers( IDXGISwapChain* thisptr, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags )
	{
		const std::lock_guard lock( detail::g_swapchain_mutex );
		if ( lifecycle::is_unloading( ) || !thisptr || !detail::g_original_resize_buffers )
		{
			return detail::g_original_resize_buffers
				? detail::g_original_resize_buffers( thisptr, buffer_count, width, height, new_format, swap_chain_flags )
				: E_FAIL;
		}

		systems::g_model_preview.reset();
		rendering::g_context.on_resize_buffers( );

		const auto result = detail::g_original_resize_buffers( thisptr, buffer_count, width, height, new_format, swap_chain_flags );
		if ( SUCCEEDED( result ) )
		{
			rendering::g_context.on_resize_buffers_post( thisptr );
		}

		return result;
	}

	LRESULT __stdcall cheat::wnd_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
	{
		if ( lifecycle::is_unloading( ) )
		{
			return m_wnd_proc.call<LRESULT>( hwnd, msg, wparam, lparam );
		}
		if ( msg == WM_ACTIVATE && LOWORD( wparam ) != WA_INACTIVE && rendering::g_menu.is_open( ) && rendering::g_context.get_window( ) == hwnd )
		{
			if ( addresses::globals::input_system )
			{
				memory::call_vfunc<void>( addresses::globals::input_system, 76, false );
			}

			SetCursor( LoadCursor( nullptr, IDC_ARROW ) );
			rendering::g_menu.apply_saved_cursor( );
		}

		if ( msg == WM_KEYDOWN && !( lparam & ( 1 << 30 ) ) && static_cast< int >( wparam ) == settings::g_misc.menu_key )
		{
			rendering::g_menu.toggle( );
			return 0;
		}

		xui::wndproc( msg, wparam, lparam );


		if ( rendering::g_menu.is_open( ) )
		{
			const auto& ui_ctx = xui::ctx( );
			const bool has_active_input = ui_ctx.active_text_input != xui::null_id
				|| ui_ctx.active_keybind != xui::null_id
				|| ui_ctx.active_slider_edit != xui::null_id;

			switch ( msg )
			{
			case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
			case WM_MOUSEWHEEL: case WM_MOUSEHWHEEL:
			case WM_MOUSEMOVE:
				return 0;

			case WM_KEYDOWN: case WM_KEYUP:
			case WM_SYSKEYDOWN: case WM_SYSKEYUP:
			case WM_CHAR:
				if ( has_active_input )
					return 0;
				break;

			default:
				break;
			}
		}
		else if ( features::misc::g_camera.is_freecam_active( ) || features::misc::g_camera.is_spec_thirdperson_active( ) )
		{
			if ( msg == WM_INPUT )
			{
				RAWINPUT raw{};
				UINT size = sizeof( raw );
				if ( GetRawInputData( reinterpret_cast< HRAWINPUT >( lparam ), RID_INPUT, &raw, &size, sizeof( RAWINPUTHEADER ) ) != static_cast< UINT >( -1 ) )
				{
					if ( raw.header.dwType == RIM_TYPEMOUSE && ( raw.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE ) == 0 )
					{
						const long dx = raw.data.mouse.lLastX;
						const long dy = raw.data.mouse.lLastY;
						if ( dx != 0 || dy != 0 )
						{
							float sens = 1.0f;
							if ( const auto cvar = CONVAR( "sensitivity" ) ) sens = cvar->get< float >( );
							sens = std::clamp( sens, 0.001f, 100.0f );

							float m_pitch = 0.022f;
							if ( const auto cvar = CONVAR( "m_pitch" ) ) m_pitch = cvar->get< float >( );
							float m_yaw = 0.022f;
							if ( const auto cvar = CONVAR( "m_yaw" ) ) m_yaw = cvar->get< float >( );

							const float d_pitch = static_cast< float >( dy ) * m_pitch * sens;
							const float d_yaw = -static_cast< float >( dx ) * m_yaw * sens;

							if ( features::misc::g_camera.is_freecam_active( ) )
							{
								features::misc::g_camera.on_mouse_delta( d_pitch, d_yaw );
							}
							else if ( features::misc::g_camera.is_spec_thirdperson_active( ) )
							{
								features::misc::g_camera.on_spec_thirdperson_mouse_delta( d_pitch, d_yaw );
							}
						}
					}
				}

				if ( features::misc::g_camera.is_freecam_active( ) )
				{
					if ( settings::g_misc.m_camera.freecam_block_input.value )
					{
						return DefWindowProcW( hwnd, msg, wparam, lparam );
					}
				}
				else if ( features::misc::g_camera.is_spec_thirdperson_active( ) )
				{
					return DefWindowProcW( hwnd, msg, wparam, lparam );
				}
			}
		}

		return m_wnd_proc.call<LRESULT>( hwnd, msg, wparam, lparam );
	}

	void __fastcall cheat::cmd_interpreter( std::uintptr_t render_thread, std::uintptr_t item, std::uint8_t flag )
	{
		m_cmd_interpreter.call<void>( render_thread, item, flag );
	}

	void __fastcall cheat::frame_stage_notify( std::uintptr_t thisptr, int stage )
	{
		diag::exception_scope scope{ "frame_stage_notify" };
		diag::note_heartbeat( ); // TEMP-DIAG: hang triage.
		if ( lifecycle::is_unloading( ) )
		{
			m_frame_stage_notify.call<void>( thisptr, stage );
			return;
		}

		settings::enforce_safe_mode();
		const auto local_player_controller = memory::safe_read<std::uintptr_t>( addresses::globals::local_player_controller ).value_or( 0 );
		// TEMP-DIAG: controller-gate triage (remove after).
		{
			static auto next = std::chrono::steady_clock::time_point{};
			const auto now = std::chrono::steady_clock::now( );
			if ( now >= next )
			{
				next = now + std::chrono::seconds( 5 );
				diag::writef( diag::level::warning,
					"[fsn-diag] stage=%d ctlptr=0x%llx ctl=0x%llx shutdown=%d",
					stage, (unsigned long long)addresses::globals::local_player_controller,
					(unsigned long long)local_player_controller, (int)is_level_shutting_down( ) );
			}
		}
		if ( !local_player_controller )
		{
			m_seen_disconnected = true;
			if ( m_was_connected )
			{
				// The controller is already gone. Engine-owned scene/particle
				// handles must be forgotten, not destroyed through stale managers.
				do_level_shutdown( false );
			}
			m_was_connected = false;
		}
		else if ( m_seen_disconnected )
		{
			// Disconnected -> connected edge: a new map is loading/loaded.
			// Do not depend on the level-init hook alone: its vtable slot was
			// hollowed out by a game update (the hook installs on a stub that
			// never fires), which would leave m_level_shutting_down set forever
			// and disable every feature gated on it.
			m_seen_disconnected = false;
			m_level_shutting_down.store( false, std::memory_order_release );
		}

		systems::g_model_preview.update( );
		if (stage == 6 || stage == 7)
			features::changer::g_inspect_preview.on_frame_stage_notify();
		features::misc::g_auto_accept.run( );
		if ( !local_player_controller || is_level_shutting_down( ) )
		{
			systems::g_local.reset( );
			systems::g_view.reset( );
			systems::g_frame_data.reset( );
			m_frame_stage_notify.call<void>( thisptr, stage );
			// Lobby work does not require a map-owned controller or camera.
			// Main menu does not receive net updates (stages 6/7), only frame/render stages (0/12).
			process_lobby_music( );
			if ( !lifecycle::is_unloading( ) &&
				!memory::safe_read<std::uintptr_t>( addresses::globals::local_player_controller ).value_or( 0 ) )
				features::changer::g_inspect_preview.on_frame_stage_notify( );

			return;
		}

		m_was_connected = true;
		// The AddEntity callback can miss entities that were created before it
		// went live, and a stale cache never heals on its own: get_by_type(player)
		// then stays empty for the whole session, which leaves reveal radar with
		// "controllers=0" and every controller-driven feature dead. Rebuild from
		// the entity list whenever the player entries are missing.
		{
			const auto missing_players =
				systems::g_entities.count_of( systems::entities::type::player ) == 0;
			if ( systems::g_entities.is_empty( ) || missing_players )
			{
				static auto next_rebuild = std::chrono::steady_clock::time_point{};
				static auto next_report = std::chrono::steady_clock::time_point{};
				const auto now = std::chrono::steady_clock::now( );
				if ( systems::g_entities.is_empty( ) || now >= next_rebuild )
				{
					next_rebuild = now + std::chrono::seconds( 2 );
					systems::g_entities.force_update( );
					if ( now >= next_report )
					{
						next_report = now + std::chrono::seconds( 5 );
						diag::writef( diag::level::warning,
							"[entities] rebuild players=%zu items=%zu projectiles=%zu",
							systems::g_entities.count_of( systems::entities::type::player ),
							systems::g_entities.count_of( systems::entities::type::item ),
							systems::g_entities.count_of( systems::entities::type::projectile ) );
					}
				}
			}
		}
		systems::g_local.update( );

		if ( stage == 6 || stage == 7 )
		{
			features::changer::g_skin_sync.on_frame_stage_notify( );
		}

		// Music belongs to the controller and must work while dead or without a camera.
		if ( stage == 6 || stage == 7 )
			features::changer::g_music.on_frame_stage_notify( );

		if ( systems::g_local.get( ).is_valid( ) && systems::g_view.has_camera( ) )
		{
			// Weapon cosmetics are applied after the original callback below.
			if ( stage == 7 )
			{
				features::world::g_scene.on_frame_stage_notify( );
				features::world::g_smoke.on_frame_stage_notify( );
				features::misc::g_other.on_frame_stage_notify( );
			}
		}

		{
			static auto was_active{ false };
			const auto is_active = settings::g_misc.m_removals.skybox_3d.value;

			if ( is_active != was_active )
			{
				const auto cvar = CONVAR ("r_draw3dskybox");
				if ( cvar )
				{
					cvar->m_value.i1 = is_active;
					++cvar->m_change_count;
				}
				was_active = is_active;
			}
		}

		detail::update_fullbright_shadows( );

		// Source 2 copies dynamic-light entries into scene objects during this stage.
		// Publish our entry first, while keeping all manager mutations on the game thread.
		if ( stage == 6 )
		{
			features::misc::g_dlight.on_frame_stage_notify( );
			features::world::g_smoke.on_frame_stage_notify( );
		}

		// Some client builds clear EntitySpottedState while dispatching the
		// network-stage callback. Publish once before and once after the original
		// callback so radar reveal is not dependent on that ordering detail.
		if ( stage == 6 || stage == 7 )
		{
			features::misc::g_other.do_reveal_radar( );
		}

		m_frame_stage_notify.call<void>( thisptr, stage );

		// The original callback may itself trigger LevelShutdown.
		if ( is_level_shutting_down( ) )
		{
			return;
		}

		// Radar state is consumed after the network update. Refresh it on both
		// update stages so the engine cannot immediately clear the spotted bit,
		// and do it independently of the camera/HUD feature pass.
		if ( stage == 6 || stage == 7 )
		{
			features::misc::g_other.do_reveal_radar( );
		}

		// One cosmetic pass after render-start has committed the network/model state.
		// A presentation controller alone must not enable lobby cosmetics.
		if ( stage == 12 && memory::safe_read<std::uintptr_t>( addresses::globals::game_rules ).value_or( 0 ) )
		{
			// Agent/model binding can recreate arms. Reconcile gloves last, without UI state.
			features::changer::g_agents.on_frame_stage_notify( );
			features::changer::g_knives.on_frame_stage_notify( );
			features::changer::g_guns.on_frame_stage_notify( );
			features::changer::g_guns.on_render_start( );
			features::changer::g_gloves.on_frame_stage_notify( );
		}

		// Process impacts after event dispatch, even when no camera is available.
		if ( stage == 7 )
		{
			features::misc::g_impacts.on_frame_stage_notify( );
		}

		// The current frame's world-to-projection matrix is published by the
		// engine during render-start stage 12.
		if ( stage == 12 )
		{
            // Expire hit highlights every render frame, not only on network updates.
            features::esp::player::g_chams.os().update();
			systems::g_view.update_matrix( );
			systems::g_frame_data.update( );
			features::world::g_weather.on_frame_stage_notify( );
			features::world::g_smoke.on_frame_stage_notify( );
		}

		if ( stage == 6 )
		{
			features::misc::g_scoreboard_weapons.on_frame_stage_notify( );
		}

		if (systems::g_local.get ().is_valid () && systems::g_view.has_camera ()) {
			if (stage == 6) {
				// Capture lag records only after Source 2 has committed this network update,
				// so the simulation timestamp, world origin and evaluated bones agree.
				features::combat::g_shared.lc( ).run( );
				features::esp::player::g_chams.bt( ).update( );

				features::misc::g_other.do_kill_feed_preservation( );
			}
		}
	}

	void __fastcall cheat::create_move( std::uintptr_t thisptr, int slot, bool active )
	{
		settings::enforce_safe_mode();
		if ( lifecycle::is_unloading( ) || is_level_shutting_down( ) )
		{
			return m_create_move.call<void>( thisptr, slot, active );
		}

		const auto local = systems::g_local.get( );

		if ( !local.pawn || !local.controller )
		{
			return m_create_move.call<void>( thisptr, slot, active );
		}

		m_create_move.call<void>( thisptr, slot, active );

		{
			features::combat::g_shared.invalidate_if_needed( );
			features::combat::g_legit.invalidate_if_needed( );
			features::combat::g_misc.quickpeek( ).reset_if_needed( );
		}

		if ( !local.is_alive || !systems::g_view.has_camera( ) )
		{
			return;
		}

		const auto movement_services = memory::safe_read<std::uintptr_t>( local.pawn + SCHEMA( "C_BasePlayerPawn", "m_pMovementServices"_hash ) ).value_or( 0 );
		if ( movement_services < 0x10000ull ||
			movement_services == ( std::numeric_limits<std::uintptr_t>::max )( ) ||
			movement_services > 0x00007FFFFFFFFFFFull )
		{
			return;
		}

		const auto last_cmd_processed = memory::read<std::uint32_t>( movement_services + SCHEMA( "CPlayer_MovementServices", "m_nLastCommandNumberProcessed"_hash ) );
		if ( !last_cmd_processed )
		{
			return;
		}

		systems::g_input.update( );
		{
			const auto current_cmd = systems::g_input.get( );
			if ( !current_cmd || !current_cmd->csgo_user_cmd.has_base( ) )
			{
				return;
			}

			static std::atomic_bool first_create_move_traced{};
			const auto trace = !first_create_move_traced.exchange( true, std::memory_order_relaxed );
			diag::exception_scope exception_scope{ "create_move: desubtick" };
			if ( trace )
			{
				diag::step( "create_move: feature pipeline begin" );
			}

			const auto original_movement_buttons = current_cmd->buttons.value | current_cmd->buttons.value_scroll;
			systems::g_input.desubtick( current_cmd );
			systems::g_prediction.capture_prestate( local.pawn, movement_services );

			if ( features::misc::g_camera.is_freecam_active( ) )
			{
				features::misc::g_camera.on_create_move( current_cmd );
				systems::g_input.apply( );
				return;
			}

			{
				diag::set_exception_phase( "create_move: shared update" );
				features::combat::g_shared.update( );

				diag::set_exception_phase( "create_move: pre-combat movement" );
				features::movement::g_slowwalk.on_create_move( current_cmd );
				features::movement::g_edgejump.on_create_move( current_cmd );
				features::movement::g_quickstop.on_create_move( current_cmd );
				features::movement::g_bhop.on_create_move( current_cmd );
				features::movement::g_fastladder.on_create_move( current_cmd );

				diag::set_exception_phase( "create_move: combat misc" );
				features::combat::g_misc.antiaim( ).on_create_move( current_cmd );
				features::combat::g_misc.autostop( ).on_create_move( current_cmd );
			}
			if ( trace )
			{
				diag::step( "create_move: shared, movement and combat misc ready" );
				diag::step( "create_move: rage begin" );
			}

			{
				diag::set_exception_phase( "create_move: rage" );
				features::combat::g_rage.on_create_move( current_cmd );
				if ( trace )
				{
					diag::step( "create_move: rage end" );
					diag::step( "create_move: legit begin" );
				}
				diag::set_exception_phase( "create_move: legit" );
				features::combat::g_legit.on_create_move( current_cmd );
				if ( trace )
				{
					diag::step( "create_move: legit end" );
				}
			}

			if ( ( current_cmd->buttons.value & cstypes::command_buttons::in_attack ) != 0 && features::combat::g_misc.antiaim( ).has_modified_angles( ) )
			{
				if ( const auto base = current_cmd->csgo_user_cmd.mutable_base( ) )
				{
					if ( const auto angles = base->mutable_viewangles( ) )
					{
						const auto va = systems::g_input.get_view_angles( );
						angles->set_x( va.x );
						angles->set_y( va.y );
					}
				}
			}

			diag::set_exception_phase( "create_move: duckpeek" );
			features::combat::g_misc.duckpeek( ).on_create_move( current_cmd );
			diag::set_exception_phase( "create_move: airstrafe" );
			features::movement::g_test_strafer.on_create_move( current_cmd, original_movement_buttons );
			if ( trace )
			{
				diag::step( "create_move: post-combat movement end" );
			}

			diag::set_exception_phase( "create_move: quickpeek" );
			features::combat::g_misc.quickpeek( ).on_create_move( current_cmd );
			if ( trace )
			{
				diag::step( "create_move: quickpeek end" );
				diag::step( "create_move: final subtick begin" );
			}

			// Run last: duckpeek, bhop and strafing must not overwrite this command.
			features::movement::g_jumpbug.on_create_move( current_cmd, original_movement_buttons );

			diag::set_exception_phase( "create_move: final subtick" );
			const auto final_base = current_cmd->csgo_user_cmd.mutable_base( );
			if ( final_base && systems::g_input.has_analog_subticks( final_base )
				&& !features::movement::g_test_strafer.handled_this_tick( )
				&& !features::movement::g_slowwalk.active_this_tick( ) )
			{
				final_base->set_forwardmove( 0.0f );
				final_base->set_leftmove( 0.0f );
			}
            // Predict the same movement that will actually be serialized,
            // including the final base/analog-subtick normalization above.
            features::misc::g_projectile_trajectory.on_create_move( current_cmd );
			if ( trace )
			{
				diag::step( "create_move: final subtick end" );
			}

			//systems::g_legit_input.on_create_move( current_cmd );
		}
		static std::atomic_bool first_input_apply_traced{};
		const auto trace_apply =
			!first_input_apply_traced.exchange( true, std::memory_order_relaxed );
		if ( trace_apply )
		{
			diag::step( "create_move: input apply begin" );
		}
		diag::exception_scope exception_scope{ "create_move: input apply" };
		systems::g_input.apply( );
		if ( trace_apply )
		{
			diag::step( "create_move: input apply end" );
		}
	}

	void __fastcall cheat::handle_view_angles( std::uintptr_t thisptr, int a2 )
	{
		if ( lifecycle::is_unloading( ) || !systems::g_local.get( ).is_valid( ) )
		{
			m_handle_view_angles.call<void>( thisptr, a2 );
			return;
		}

		const auto view_angles = systems::g_input.get_view_angles( );

		m_handle_view_angles.call<void>( thisptr, a2 );

		systems::g_input.set_view_angles( view_angles );
	}

	void __fastcall cheat::add_entity( std::uintptr_t thisptr, std::uintptr_t entity, std::uint32_t handle )
	{
		if ( lifecycle::is_unloading( ) )
		{
			m_add_entity.call<void>( thisptr, entity, handle );
			return;
		}

		systems::g_entities.on_add_entity( entity, handle );

		m_add_entity.call<void>( thisptr, entity, handle );
        features::changer::preview_scene::on_entity_changed(entity);
	}

	void __fastcall cheat::remove_entity( std::uintptr_t thisptr, std::uintptr_t entity, std::uint32_t handle )
	{
		if ( lifecycle::is_unloading( ) )
		{
			m_remove_entity.call<void>( thisptr, entity, handle );
			return;
		}

		features::changer::preview_scene::on_entity_changed(entity);
		systems::g_entities.on_remove_entity( entity, handle );

		m_remove_entity.call<void>( thisptr, entity, handle );
	}

	void __fastcall cheat::render_view( std::uintptr_t thisptr )
	{
		m_render_view.call<void>( thisptr );

		if ( lifecycle::is_unloading( ) || is_level_shutting_down( ) || !systems::g_local.get( ).is_valid( ) )
		{
			systems::g_view.reset( );
			systems::g_frame_data.reset( );
			return;
		}

		systems::g_view.update( thisptr + 0x10 );
		systems::g_frame_data.update( );
	}

	void __fastcall cheat::draw_skybox_array( std::uintptr_t thisptr, std::uintptr_t a2, std::uintptr_t mesh_array, int mesh_count, int a5, std::uintptr_t a6, std::uintptr_t a7, std::uintptr_t a8 )
	{
		if ( lifecycle::is_unloading( ) )
		{
			m_draw_skybox_array.call<void>( thisptr, a2, mesh_array, mesh_count, a5, a6, a7, a8 );
			return;
		}

		features::world::g_scene.on_draw_skybox_array_pre( mesh_array, mesh_count );

		m_draw_skybox_array.call<void>( thisptr, a2, mesh_array, mesh_count, a5, a6, a7, a8 );

		features::world::g_scene.on_draw_skybox_array_post( );
	}

	std::uintptr_t __fastcall cheat::light_scene_object( std::uintptr_t thisptr, std::uintptr_t object, std::uintptr_t a3 )
	{
		if ( lifecycle::is_unloading( ) || !object )
		{
			return m_light_scene_object.call<std::uintptr_t>( thisptr, object, a3 );
		}

		const auto& scene = settings::g_world.m_scene;
		const bool scene_lighting = scene.fullbright.value || scene.lighting.value;
		const bool is_dlight = features::misc::g_dlight.is_target_object( object );

		if ( !scene_lighting && !is_dlight )
		{
			return m_light_scene_object.call<std::uintptr_t>( thisptr, object, a3 );
		}

		const auto original_color = memory::safe_read<std::array<float, 3>>( object + 0xe4 );
		if ( original_color )
		{
			features::world::g_scene.on_light_scene_object_pre( object );
			features::misc::g_dlight.apply_scene_color( object );
		}

		const auto result = m_light_scene_object.call<std::uintptr_t>( thisptr, object, a3 );

		features::world::g_scene.on_light_scene_object_post( object );
		if ( original_color )
		{
			(void) memory::safe_write( object + 0xe4, *original_color );
		}

		return result;
	}

	namespace detail {
		inline bool read_batch_colors( std::uintptr_t batch, int count, std::pair<std::uintptr_t, std::uint32_t>* out ) noexcept
		{
			__try
			{
				for ( int i = 0; i < count; ++i )
				{
					const auto address = batch + static_cast<std::size_t>( i ) * features::esp::detail::primitive_size + features::esp::detail::primitive_color_offset;
					if ( address < 0x10000ull || address > 0x00007FFFFFFFFFFFull )
					{
						return false;
					}
					out[ i ] = { address, *reinterpret_cast<const std::uint32_t*>( address ) };
				}
				return true;
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				return false;
			}
		}

		inline void restore_batch_colors( const std::pair<std::uintptr_t, std::uint32_t>* in, std::size_t count ) noexcept
		{
			__try
			{
				for ( std::size_t i = 0; i < count; ++i )
				{
					const auto address = in[ i ].first;
					if ( address >= 0x10000ull && address <= 0x00007FFFFFFFFFFFull )
					{
						*reinterpret_cast<std::uint32_t*>( address ) = in[ i ].second;
					}
				}
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
			}
		}

		// The menu writes settings on this same (render) thread and there is no
		// central "changed" callback, so a throttled fingerprint scan stands in
		// for one. Without it a toggle only lived in memory: the profile on disk
		// kept its old values, so the next injection silently restored them.
		// The write waits for the edits to settle, because a slider drag would
		// otherwise rewrite the whole profile on every tick.
		inline void autosave_setting_changes( )
		{
			static bool seeded{ false };
			static bool pending{ false };
			static std::uint64_t baseline{};
			static auto next_check = std::chrono::steady_clock::now( );
			static auto next_write = std::chrono::steady_clock::time_point{};
			const auto now = std::chrono::steady_clock::now( );
			if ( seeded && now < next_check )
			{
				return;
			}
			next_check = now + std::chrono::seconds( 2 );

			const auto current = config::registry::fingerprint( );
			if ( !seeded )
			{
				seeded = true;
				baseline = current;
				return;
			}

			const bool changed = current != baseline;
			if ( changed )
			{
				baseline = current;
			}

			// A slider drag produces a new fingerprint every tick, so wait for a
			// quiet tick before writing. The rate limit keeps a setting that some
			// feature rewrites continuously from postponing the write forever.
			if ( changed && now < next_write )
			{
				pending = true;
				return;
			}
			if ( !changed && !pending )
			{
				return;
			}

			pending = false;
			if ( config::registry::save_active( ) )
			{
				next_write = now + std::chrono::seconds( 15 );
				diag::writef( diag::level::debug, "[config] settings saved to the active profile" );
			}
		}
	} // namespace detail

	std::uintptr_t __fastcall cheat::draw_scene_object( std::uintptr_t thisptr, std::uintptr_t object, std::uintptr_t batch, int batch_count, std::uintptr_t a5, std::uintptr_t a6, std::uintptr_t a7 )
	{
		static std::atomic_bool first_draw_call{};
		if ( !first_draw_call.exchange( true, std::memory_order_relaxed ) )
		{
			diag::writef( diag::level::info,
				"[chams-hook] draw_scene_object first call a1=%p a2=%p batch=%p count=%d a5=%p a6=%p a7=%p",
				reinterpret_cast<void*>( thisptr ), reinterpret_cast<void*>( object ),
				reinterpret_cast<void*>( batch ), batch_count, reinterpret_cast<void*>( a5 ), reinterpret_cast<void*>( a6 ), reinterpret_cast<void*>( a7 ) );
		}

		const auto& scene = settings::g_world.m_scene;
		const bool tint_active = ( scene.fullbright.value || scene.world_setting.value || scene.skybox.custom_color.value ||
			settings::g_misc.m_smoke_and_fire_color.custom_molotov.value );
		const auto& player_chams = settings::g_esp.m_player.m_chams;
		const bool player_chams_requested = player_chams.enemy.enabled.value || player_chams.team.enabled.value ||
			player_chams.local.enabled.value || player_chams.enemy_ragdoll.enabled.value ||
			player_chams.team_ragdoll.enabled.value || player_chams.local_ragdoll.enabled.value;
		// Never rewrite DrawSceneObject's transient mesh batch as a fallback. The
		// batch layout is not a stable player-primitive interface; repeated owner
		// probing and temporary mutation here can cause bad overlays, frame drops,
		// and render crashes.
		static std::atomic_bool chams_hook_warning_reported{};
		if ( player_chams_requested &&
			!detail::g_generate_primitives_player_chams_applied.load( std::memory_order_relaxed ) &&
			!chams_hook_warning_reported.exchange( true, std::memory_order_relaxed ) )
		{
			diag::writef( diag::level::warning,
				"[chams] player chams not applied: GeneratePrimitives has not confirmed a player draw; DrawSceneObject batch fallback is disabled" );
		}
		const auto local_pawn_for_state = systems::g_local.get( ).view_pawn( );
		const auto chams_state = ( player_chams.enemy.enabled.value ? 1u : 0u ) |
			( player_chams.team.enabled.value ? 1u << 1 : 0u ) |
			( player_chams.local.enabled.value ? 1u << 2 : 0u ) |
			( player_chams.enemy_ragdoll.enabled.value ? 1u << 3 : 0u ) |
			( player_chams.team_ragdoll.enabled.value ? 1u << 4 : 0u ) |
			( player_chams.local_ragdoll.enabled.value ? 1u << 5 : 0u ) |
			( local_pawn_for_state ? 1u << 6 : 0u );
		static std::atomic_uint32_t last_chams_state{ ( std::numeric_limits<std::uint32_t>::max )( ) };
		if ( last_chams_state.exchange( chams_state, std::memory_order_relaxed ) != chams_state )
		{
			detail::g_generate_primitives_player_chams_applied.store( false, std::memory_order_relaxed );
			diag::writef( diag::level::info,
				"[chams-state] enabled-mask=0x%02X local-pawn=%p generate-hook=%d",
				chams_state, reinterpret_cast<void*>( local_pawn_for_state ),
				static_cast<int>( m_generate_primitives.is_enabled( ) || m_generate_animatable_primitives.is_enabled( ) ) );
		}

		if ( lifecycle::is_unloading( ) || !batch || batch < 0x10000ull || batch > 0x00007FFFFFFFFFFFull || batch_count <= 0 || batch_count > ( 1 << 16 ) ||
			!tint_active )
		{
			return m_draw_scene_object.call<std::uintptr_t>( thisptr, object, batch, batch_count, a5, a6, a7 );
		}

		constexpr std::size_t k_stack_batch_capacity = 128;
		std::array<std::pair<std::uintptr_t, std::uint32_t>, k_stack_batch_capacity> stack_colors;
		std::vector<std::pair<std::uintptr_t, std::uint32_t>> heap_colors;

		auto* colors_data = stack_colors.data( );
		if ( static_cast<std::size_t>( batch_count ) > k_stack_batch_capacity )
		{
			heap_colors.resize( batch_count );
			colors_data = heap_colors.data( );
		}

		bool tinted = false;
		if ( tint_active && detail::read_batch_colors( batch, batch_count, colors_data ) )
		{
			diag::exception_scope exception_scope{ "world: primitive tint" };
			features::world::g_scene.on_draw_scene_object( batch, batch_count );
			tinted = true;
		}

		const auto result = m_draw_scene_object.call<std::uintptr_t>( thisptr, object, batch, batch_count, a5, a6, a7 );

		if ( tinted )
		{
			detail::restore_batch_colors( colors_data, static_cast<std::size_t>( batch_count ) );
		}

		return result;
	}

	bool __fastcall cheat::is_glowing( std::uintptr_t glow_property )
	{
		if ( lifecycle::is_unloading( ) || !glow_property )
		{
			return m_is_glowing.call<bool>( glow_property );
		}

		const auto& pcfg = settings::g_esp.m_player.m_glow;
		const auto& icfg = settings::g_esp.m_item.m_glow;
		const bool glow_active = pcfg.enemy.enabled.value || pcfg.enemy_ragdoll.enabled.value ||
			pcfg.team.enabled.value || pcfg.team_ragdoll.enabled.value ||
			pcfg.local.enabled.value || pcfg.local_ragdoll.enabled.value ||
			icfg.enabled.value;

		if ( !glow_active )
		{
			return m_is_glowing.call<bool>( glow_property );
		}

		const auto owner_entity = memory::safe_read<std::uintptr_t>( glow_property + 0x18 ).value_or( 0 );
		if ( owner_entity )
		{
			const auto schema_name = systems::g_entities.get_schema_name( owner_entity );
			if ( schema_name )
			{
				const auto owner_hash = fnv1a::runtime_hash( schema_name );
				if ( owner_hash )
				{
					if ( features::esp::player::g_glow.on_is_glowing( owner_entity, owner_hash ) )
					{
						return true;
					}

					if ( features::esp::item::g_glow.on_is_glowing( owner_entity, owner_hash ) )
					{
						return true;
					}
				}
			}
		}

		return m_is_glowing.call<bool>( glow_property );
	}

	void __fastcall cheat::get_glow_color( std::uintptr_t glow_property, float* color )
	{
		if ( lifecycle::is_unloading( ) || !glow_property )
		{
			m_get_glow_color.call<void>( glow_property, color );
			return;
		}

		const auto& pcfg = settings::g_esp.m_player.m_glow;
		const auto& icfg = settings::g_esp.m_item.m_glow;
		const bool glow_active = pcfg.enemy.enabled.value || pcfg.enemy_ragdoll.enabled.value ||
			pcfg.team.enabled.value || pcfg.team_ragdoll.enabled.value ||
			pcfg.local.enabled.value || pcfg.local_ragdoll.enabled.value ||
			icfg.enabled.value;

		if ( !glow_active )
		{
			m_get_glow_color.call<void>( glow_property, color );
			return;
		}

		const auto owner_entity = memory::safe_read<std::uintptr_t>( glow_property + 0x18 ).value_or( 0 );
		if ( owner_entity )
		{
			const auto schema_name = systems::g_entities.get_schema_name( owner_entity );
			if ( schema_name )
			{
				const auto owner_hash = fnv1a::runtime_hash( schema_name );
				if ( owner_hash )
				{
					if ( features::esp::player::g_glow.on_get_glow_color( owner_entity, owner_hash, color ) )
					{
						return;
					}

					if ( features::esp::item::g_glow.on_get_glow_color( owner_entity, owner_hash, color ) )
					{
						return;
					}
				}
			}
		}

		m_get_glow_color.call<void>( glow_property, color );
	}

	ID3D11ShaderResourceView* __fastcall cheat::preview_resource_view(
        std::uintptr_t context, std::uintptr_t handle, char view, char alternate,
        const char* name)
    {
        auto* srv = m_preview_resource_view.call<ID3D11ShaderResourceView*>(context, handle, view, alternate, name);
        if (!lifecycle::is_unloading()) {
            systems::g_model_preview.capture_resource(handle, alternate, srv, name);
        }
        return srv;
    }

	namespace detail
	{
		std::uintptr_t handle_generate_primitives(
			generate_primitives_fn original_fn,
			bool animatable,
			std::uintptr_t target,
			std::uintptr_t desc,
			std::uintptr_t object,
			std::uintptr_t a3,
			std::uintptr_t render_buffer )
		{
			if ( !original_fn )
				return 0;

			static std::atomic_bool first_base_generate_call{};
			static std::atomic_bool first_animatable_generate_call{};
			auto& first_generate_call = animatable
				? first_animatable_generate_call : first_base_generate_call;
			if ( !first_generate_call.exchange( true, std::memory_order_relaxed ) )
			{
				diag::writef( diag::level::info,
					"[chams-hook] GeneratePrimitives first call hook=%s target=%p desc=%p object=%p a3=%p buffer=%p",
					animatable ? "animatable" : "base", reinterpret_cast<void*>( target ), reinterpret_cast<void*>( desc ),
					reinterpret_cast<void*>( object ), reinterpret_cast<void*>( a3 ),
					reinterpret_cast<void*>( render_buffer ) );
			}

			if ( lifecycle::is_unloading( ) || cheat::is_level_shutting_down( ) )
				return original_fn( desc, object, a3, render_buffer );

			const bool render_chams_active = any_render_chams_active( );
			const bool preview_pose_active = systems::g_model_preview.wants_agent_pose( );
			if ( object && ( render_chams_active || preview_pose_active ) )
			{
				if ( render_chams_active && settings::g_esp.m_player.m_chams.backtrack.enabled.value &&
					features::esp::player::g_chams.bt( ).is_active( object ) )
					return 0;

				const auto owner = resolve_render_owner( object );
				if ( owner.entity && owner.schema_hash )
				{
					const auto owner_hash = owner.schema_hash;
					if ( preview_pose_active && owner_hash == "C_CSGO_PreviewPlayer"_hash )
					{
						systems::g_model_preview.capture_agent_pose( owner.entity );
					}

					if ( render_chams_active )
					{
						auto* generate_state = g_generate_primitives_state.get( );
						if ( generate_state )
						{
							generate_state->original = original_fn;
							generate_state->result = 0;
							if ( features::esp::player::g_chams.on_generate_primitives(
								owner.entity, owner_hash, object, render_buffer,
								&call_generate_primitives_original, desc, a3 ) )
							{
								if ( is_player_pawn_hash( owner_hash ) )
									g_generate_primitives_player_chams_applied.store( true, std::memory_order_relaxed );
								const auto result = generate_state->result;
								generate_state->original = nullptr;
								return result;
							}

							generate_state->result = 0;
							if ( features::esp::item::g_chams.on_generate_primitives(
								owner.entity, owner_hash, object, render_buffer,
								&call_generate_primitives_original, desc, a3 ) )
							{
								const auto result = generate_state->result;
								generate_state->original = nullptr;
								return result;
							}
							generate_state->original = nullptr;
						}
					}
				}
			}

			return original_fn( desc, object, a3, render_buffer );
		}
	}

	std::uintptr_t __fastcall cheat::generate_primitives( std::uintptr_t desc, std::uintptr_t object, std::uintptr_t a3, std::uintptr_t render_buffer )
	{
		return detail::handle_generate_primitives(
			m_generate_primitives.original<detail::generate_primitives_fn>( ),
			false, reinterpret_cast<std::uintptr_t>( m_generate_primitives.get_target( ) ),
			desc, object, a3, render_buffer );
	}

	std::uintptr_t __fastcall cheat::generate_animatable_primitives( std::uintptr_t desc, std::uintptr_t object, std::uintptr_t a3, std::uintptr_t render_buffer )
	{
		return detail::handle_generate_primitives(
			m_generate_animatable_primitives.original<detail::generate_primitives_fn>( ),
			true, reinterpret_cast<std::uintptr_t>( m_generate_animatable_primitives.get_target( ) ),
			desc, object, a3, render_buffer );
	}

	std::uintptr_t __fastcall cheat::parse_report_hit( std::uintptr_t thisptr, std::uint8_t deleting )
	{
		// A report's deleting destructor can also run while the level is torn down.
		if ( !lifecycle::is_unloading( ) && !is_level_shutting_down( ) && systems::g_local.get( ).is_valid( ) )
		{
			features::misc::g_impacts.on_report_hit( thisptr );
		}

		return m_parse_report_hit.call<std::uintptr_t>( thisptr, deleting );
	}

	void __fastcall cheat::vote_start( void* panel, std::uintptr_t msg )
	{
		if ( !lifecycle::is_unloading( ) )
		{
			features::misc::g_vote_logs.on_vote_start( msg );
		}
		m_vote_start.call<void>( panel, msg );
	}

	void __fastcall cheat::vote_pass( void* panel, std::uintptr_t msg )
	{
		if ( !lifecycle::is_unloading( ) )
		{
			features::misc::g_vote_logs.on_vote_pass( msg );
		}
		m_vote_pass.call<void>( panel, msg );
	}

	void __fastcall cheat::vote_failed( void* panel, std::uintptr_t msg )
	{
		if ( !lifecycle::is_unloading( ) )
		{
			features::misc::g_vote_logs.on_vote_failed( msg );
		}
		m_vote_failed.call<void>( panel, msg );
	}

	void* __fastcall cheat::panorama_event( void* thisptr, const char* event_name, void* p1, void* p2 )
	{
		if ( !lifecycle::is_unloading( ) && event_name )
		{
			features::misc::g_auto_accept.on_panorama_event( event_name );
		}

		return m_panorama_event.call<void*>( thisptr, event_name, p1, p2 );
	}

	std::uintptr_t __fastcall cheat::setup_fog( __m128i* output, int* mode )
	{
		if ( lifecycle::is_unloading( ) )
		{
			return m_setup_fog.call<std::uintptr_t>( output, mode );
		}

		if ( features::world::g_scene.on_setup_fog( output, mode ) )
		{
			return 0;
		}

		return m_setup_fog.call<std::uintptr_t>( output, mode );
	}

	std::uintptr_t __fastcall cheat::set_shader_param( __m128i* map, std::uint32_t hash, __m128i* value )
	{
		if ( lifecycle::is_unloading( ) )
		{
			return m_set_shader_param.call<std::uintptr_t>( map, hash, value );
		}

		features::world::g_scene.on_set_shader_param( value, hash );

		return m_set_shader_param.call<std::uintptr_t>( map, hash, value );
	}

	std::uintptr_t __fastcall cheat::set_postprocess_vec( __m128i* map, std::uint32_t hash, __m128i* value )
	{
		if ( lifecycle::is_unloading( ) )
		{
			return m_set_postprocess_vec.call<std::uintptr_t>( map, hash, value );
		}

		constexpr std::uint32_t dof_ranges{ 0x2ACAB07C };

		// The engine only publishes DofRanges when the active camera enables DOF.
		// Insert it alongside any post-process vector, matching Artisan's live path.
		if ( settings::g_world.m_scene.dof.value && hash != dof_ranges )
		{
			__m128i* dof_value{};
			features::world::g_scene.on_set_shader_param( dof_value, dof_ranges );
			if ( dof_value )
			{
				m_set_postprocess_vec.call<std::uintptr_t>( map, dof_ranges, dof_value );
			}
		}

		features::world::g_scene.on_set_shader_param( value, hash );
		return m_set_postprocess_vec.call<std::uintptr_t>( map, hash, value );
	}

	void __fastcall cheat::override_view( std::uintptr_t thisptr, std::uintptr_t view_setup )
	{
		m_override_view.call<void>( thisptr, view_setup );

		if ( lifecycle::is_unloading( ) || !view_setup )
		{
			return;
		}

		__try
		{
			features::misc::g_camera.on_override_view( view_setup );
			features::misc::g_removals.on_override_view( view_setup );
			features::combat::g_misc.duckpeek( ).on_override_view( view_setup );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	void __fastcall cheat::update_fov_sensitivity( std::uintptr_t thisptr )
	{
		m_update_fov_sensitivity.call<void>( thisptr );

		if ( lifecycle::is_unloading( ) )
		{
			return;
		}

		features::misc::g_camera.update_fov_sensitivity( thisptr );
	}

	void __fastcall cheat::render_scope( std::uintptr_t a1, std::uintptr_t a2 )
	{
		m_render_scope.call<void>( a1, a2 );

		if ( lifecycle::is_unloading( ) )
		{
			return;
		}

		if ( settings::g_misc.m_removals.scope.value )
		{
			memory::write<std::uint8_t>( a2 + 4, 0 );
		}
	}

	bool __fastcall cheat::render_crosshair( std::uintptr_t a1 )
	{
		if ( !lifecycle::is_unloading( ) && settings::g_misc.m_removals.crosshair.value )
		{
			return false;
		}

		return m_render_crosshair.call<bool>( a1 );
	}

	float __fastcall cheat::prepare_scene_material( std::uintptr_t material, void* a2, float a3 )
	{
		if ( !lifecycle::is_unloading( ) )
		{
			features::misc::g_removals.on_prepare_scene_material( material );
		}

		return m_prepare_scene_material.call<float>( material, a2, a3 );
	}

	void __fastcall cheat::post_network_data_received( std::uintptr_t thisptr )
	{
		m_post_network_data_received.call<void>( thisptr );
	}

	bool __fastcall cheat::draw_overhead( std::uintptr_t pawn, std::uint32_t player_slot )
	{
		if ( !lifecycle::is_unloading( ) && settings::g_misc.m_removals.overhead.value && pawn == systems::g_local.get( ).pawn )
		{
			return false;
		}

		return m_draw_overhead.call<bool>( pawn, player_slot );
	}

	std::uintptr_t __fastcall cheat::draw_legs( std::uintptr_t a1, std::uintptr_t a2, std::uintptr_t a3, std::uintptr_t a4, std::uintptr_t a5 )
	{
		if ( !lifecycle::is_unloading( ) && settings::g_misc.m_removals.legs.value )
		{
			return 0;
		}

		return m_draw_legs.call<std::uintptr_t>( a1, a2, a3, a4, a5 );
	}

	bool __fastcall cheat::get_transforms_for_hitbox_list( std::uintptr_t a1, std::uintptr_t a2, int* a3 )
	{
		if ( lifecycle::is_unloading( ) || !features::combat::g_shared.autowalling( ) )
		{
			return m_get_transforms_for_hitbox_list.call<bool>( a1, a2, a3 );
		}

		const auto record = features::combat::g_shared.current_autowall_record( );
		if ( !record || !record->valid )
		{
			return m_get_transforms_for_hitbox_list.call<bool>( a1, a2, a3 );
		}

		const auto count = memory::safe_read<int>( reinterpret_cast< std::uintptr_t >( a3 ) ).value_or( 0 );
		const auto shape_array = memory::safe_read<std::uintptr_t>( reinterpret_cast< std::uintptr_t >( a3 ) + 8 ).value_or( 0 );
		const auto entity_bone_cache = memory::safe_read<std::uintptr_t>( a1 + 0x1c0 ).value_or( 0 );
		const auto model_handle = memory::safe_read<std::uintptr_t>( a1 + 0x1e0 ).value_or( 0 );
		const auto model = model_handle ? memory::safe_read<std::uintptr_t>( model_handle ).value_or( 0 ) : 0;

		if ( count <= 0 || count > 256 || !shape_array || !entity_bone_cache || !model )
		{
			return false;
		}

		static const auto get_bone_index = PATTERN( patterns::get_bone_index );
		if ( !get_bone_index )
		{
			return false;
		}

		// Reuse the mapping only within this invocation. No model/shape pointers
		// survive a trace, a model change or a new command.
		std::array<int, 256> bone_indices;
		// The client dereferences every resolved transform without checking the
		// backing cache. Reject a stale scene node instead of faulting in it.
		for ( auto i = 0; i < count; ++i )
		{
			const auto shape_ptr = shape_array + 16ull * i;
			const auto bone_index = memory::call<int>( get_bone_index, model, shape_ptr );
			bone_indices[ i ] = bone_index;

			if ( bone_index >= 0 &&
				( bone_index >= 256 ||
					!memory::safe_read<systems::bones::data>( entity_bone_cache + sizeof( systems::bones::data ) * bone_index ) ) )
			{
				return false;
			}
		}

		const auto target_scene = record->game_scene_node ? record->game_scene_node : memory::read<std::uintptr_t>( record->pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );
		const auto target_bone_cache = target_scene ? memory::safe_read<std::uintptr_t>( target_scene + SCHEMA( "CSkeletonInstance", "m_modelState"_hash ) + 0x80 ).value_or( 0 ) : 0;
		const auto result = m_get_transforms_for_hitbox_list.call<bool>( a1, a2, a3 );

		if ( !result )
		{
			return false;
		}

		const auto is_plausible = [ ]( std::uintptr_t ptr ) noexcept -> bool
			{
				return ptr >= 0x10000ull && ptr < 0x00007FFFFFFFFFFFull;
			};

		if ( !is_plausible( entity_bone_cache ) || !is_plausible( target_bone_cache ) || entity_bone_cache != target_bone_cache )
		{
			return result;
		}

		const auto output_array = memory::safe_read<std::uintptr_t>( a2 + 16 ).value_or( 0 );

		if ( !output_array || count <= 0 )
		{
			return result;
		}

		for ( auto i = 0; i < count; ++i )
		{
			const auto bone_index = bone_indices[ i ];

			if ( bone_index < 0 || bone_index >= record->bone_count )
			{
				continue;
			}

			const auto dst = output_array + 32ull * i;
			std::memcpy( reinterpret_cast< void* >( dst ), &record->bones[ bone_index ], 32 );
		}

		return true;
	}

	void __fastcall cheat::sort_primitives( std::uintptr_t thisptr, std::uintptr_t a2, std::uintptr_t a3, std::uint32_t a4 )
	{
		m_sort_primitives.call<void>( thisptr, a2, a3, a4 );

		if ( lifecycle::is_unloading( ) || cheat::is_level_shutting_down( ) )
			return;

		diag::exception_scope exception_scope{ "chams: sort primitives" };
		features::esp::player::g_chams.on_sort_primitives( a3, a4 );
	}

	float __fastcall cheat::get_inaccuracy( std::uintptr_t thisptr, float* a2, float* a3 )
	{
		const auto result = m_get_inaccuracy.call<float>( thisptr, a2, a3 );

		if ( lifecycle::is_unloading( ) )
		{
			return result;
		}

#if defined(__clang__) || defined(__GNUC__)
		const auto result_address = reinterpret_cast< std::uintptr_t >( __builtin_return_address( 0 ) );
#else
		const auto result_address = reinterpret_cast< std::uintptr_t >( _ReturnAddress( ) );
#endif

		static const auto base_fire_guns_get_inaccuracy = PATTERN( patterns::base_fire_guns_get_inaccuracy );
		if ( base_fire_guns_get_inaccuracy &&
			result_address > base_fire_guns_get_inaccuracy &&
			result_address < base_fire_guns_get_inaccuracy + 0x600 )
		{
			features::misc::g_impacts.on_base_fire_guns_get_inaccuracy( thisptr, result );
		}

		return result;
	}

	float* __fastcall cheat::get_interpolated_shoot_position( std::uintptr_t thisptr, float* out, int* tick_frac )
	{
		const auto result = m_get_interpolated_shoot_position.call<float*>( thisptr, out, tick_frac );

		if ( lifecycle::is_unloading( ) )
		{
			return result;
		}

		// Prediction calls this helper while building the next command as well.
		// Only the call made for an actual rage shot is its authoritative origin.
		if ( features::combat::g_rage.is_firing_this_tick( ) )
		{
			features::misc::g_impacts.on_get_interpolated_shoot_position( thisptr, out );
		}

		return result;
	}

	std::uintptr_t __fastcall cheat::level_initialization( std::uintptr_t a1, const char* new_map )
	{
		if ( lifecycle::is_unloading( ) )
		{
			return m_level_initialization.call<std::uintptr_t>( a1, new_map );
		}

		// If shutdown was missed, old map objects are no longer ours to delete.
		do_level_shutdown( false );

		if ( new_map && new_map[ 0 ] )
		{
			const char* leaf = std::strrchr( new_map, '/' );
			rendering::g_widgets.s_map_name = leaf ? leaf + 1 : new_map;
		}
		else
		{
			rendering::g_widgets.s_map_name.clear( );
		}
		settings::g_world.update_active( rendering::g_widgets.s_map_name );

		const auto result = m_level_initialization.call<std::uintptr_t>( a1, new_map );
		m_was_connected = false;
		m_seen_disconnected = false;
		m_level_shutting_down.store( false, std::memory_order_release );
		return result;
	}

	void cheat::do_level_shutdown( bool release_engine_resources )
	{
		// LevelShutdown and the controller-loss fallback share one cleanup state.
		if ( m_level_shutting_down.exchange( true, std::memory_order_acq_rel ) )
		{
			return;
		}
		m_was_connected = false;

		if ( !config::registry::flush_pending_save() )
		{
			diag::write( diag::level::warning, "failed to flush deferred config save on level shutdown" );
		}

		diag::exception_scope exception_scope{ "level shutdown: invalidate snapshots" };
		diag::step( release_engine_resources
			? "level shutdown: pre-engine cleanup"
			: "level shutdown: cache-only cleanup" );

		features::changer::preview_scene::reset( );
		features::changer::preview_item::reset( );
		detail::g_generate_primitives_player_chams_applied.store( false, std::memory_order_relaxed );
		// Stop publishing the old level before entering engine destructors.
		systems::g_local.reset( );
		systems::g_view.reset( );
		systems::g_frame_data.reset( );
		systems::g_model_preview.reset( );
		rendering::g_widgets.s_map_name.clear( );
		settings::g_world.update_active( "" );

		diag::set_exception_phase( "level shutdown: backtrack objects" );
		features::esp::player::g_chams.bt( ).shutdown( release_engine_resources );
		diag::set_exception_phase( "level shutdown: onshot objects" );
		features::esp::player::g_chams.os( ).shutdown( release_engine_resources );
		diag::set_exception_phase( "level shutdown: weather" );
		features::world::g_weather.release( release_engine_resources );
		diag::set_exception_phase( "level shutdown: dynamic light" );
		features::misc::g_dlight.on_level_shutdown( release_engine_resources );

		diag::set_exception_phase( "level shutdown: local caches" );
		systems::materials::clear_clones( );
		features::misc::g_vote_logs.reset( );
		// Do not restore view angles into input objects belonging to the old map.
		features::misc::g_camera.reset( false );
		// Motion-blur state is render-thread owned; its next Present resets it
		// after observing the invalid local snapshot.
		features::misc::g_impacts.on_level_change( );
		features::esp::player::g_overlay.reset_sounds( );
		features::misc::g_scoreboard_weapons.on_level_change( );
		features::world::g_scene.reset_skybox_state( );

		features::changer::g_guns.reset( );
		features::changer::g_knives.reset( );
		features::changer::g_gloves.reset( );
		features::changer::g_agents.reset( );
		features::changer::g_music.reset( );
		detail::g_lobby_music_requests.reset( );
		systems::g_entities.reset( );
		features::combat::g_shared.lc( ).clear( );
		detail::g_vm_anim.initialized = false;
		detail::reset_fullbright_shadows( );
		diag::step( "level shutdown: cleanup complete" );
	}

	std::uintptr_t __fastcall cheat::level_shutdown( std::uintptr_t a1 )
	{
		if ( lifecycle::is_unloading( ) )
		{
			return m_level_shutdown.call<std::uintptr_t>( a1 );
		}

		do_level_shutdown( true );
		diag::exception_scope exception_scope{ "level shutdown: engine" };
		return m_level_shutdown.call<std::uintptr_t>( a1 );
	}

	void __fastcall cheat::read_frame_input( std::uintptr_t a1, std::uint32_t a2 )
	{
		m_read_frame_input.call<void>( a1, a2 );
		// The menu must not wait for a match's FrameStageNotify/net update to
		// consume settings. Keep engine work on the client frame path, not Present.
		if ( lifecycle::is_unloading( ) ||
			memory::safe_read<std::uintptr_t>( addresses::globals::local_player_controller ).value_or( 0 ) ) return;
		if ( m_was_connected ) do_level_shutdown( false );
		m_seen_disconnected = true;
		process_lobby_music( );
	}

	void __fastcall cheat::process_input_event( std::uintptr_t csgo_input, int slot, float frametime )
	{
		if ( lifecycle::is_unloading( ) )
		{
			m_process_input_event.call<void>( csgo_input, slot, frametime );
			return;
		}

		if ( slot == 0 && csgo_input && features::misc::g_camera.is_freecam_active( ) )
		{
			if ( settings::g_misc.m_camera.freecam_block_input.value )
			{
				const auto saved = features::misc::g_camera.get_saved_viewangles( );
				const auto p_pitch = reinterpret_cast< float* >( csgo_input + 1672 );
				const auto p_yaw = reinterpret_cast< float* >( csgo_input + 1676 );
				if ( std::isfinite( saved.x ) && std::isfinite( saved.y ) )
				{
					*p_pitch = saved.x;
					*p_yaw = saved.y;
				}
			}

			m_process_input_event.call<void>( csgo_input, slot, frametime );

			if ( settings::g_misc.m_camera.freecam_block_input.value )
			{
				const auto saved = features::misc::g_camera.get_saved_viewangles( );
				const auto p_pitch = reinterpret_cast< float* >( csgo_input + 1672 );
				const auto p_yaw = reinterpret_cast< float* >( csgo_input + 1676 );
				if ( std::isfinite( saved.x ) && std::isfinite( saved.y ) )
				{
					*p_pitch = saved.x;
					*p_yaw = saved.y;
				}
			}
			return;
		}

		systems::g_legit_input.on_process_input_event( csgo_input, slot );
		m_process_input_event.call<void>( csgo_input, slot, frametime );
	}

	std::uintptr_t __fastcall cheat::render_decals( std::uintptr_t render_context, std::uintptr_t** render_view, bool pass_flag_a, bool pass_flag_b )
	{
		if ( !lifecycle::is_unloading( ) && settings::g_misc.m_removals.decals.value )
		{
			return 0;
		}

		return m_render_decals.call<std::uintptr_t>( render_context, render_view, pass_flag_a, pass_flag_b );
	}

	void __fastcall cheat::render_smoke( std::uintptr_t a1, std::uintptr_t a2, int a3, int a4, std::uintptr_t a5, std::uintptr_t a6 )
	{
		if ( lifecycle::is_unloading( ) )
		{
			m_render_smoke.call<void>( a1, a2, a3, a4, a5, a6 );
			return;
		}

		static std::once_flag alright;
		std::call_once( alright, [ & ]
			{
				if ( !a2 )
				{
					return;
				}

				if ( m_render_smoke_map.create( reinterpret_cast< void* >( memory::get_vfunc( a2, 32 ) ), &render_smoke_map ) )
				{
					m_render_smoke_map.enable( );
				}

				if ( m_render_smoke_unmap.create( reinterpret_cast< void* >( memory::get_vfunc( a2, 33 ) ), &render_smoke_unmap ) )
				{
					m_render_smoke_unmap.enable( );
				}
			} );

		if ( settings::g_misc.m_removals.smoke.value )
		{
			return;
		}

		m_render_smoke.call<void>( a1, a2, a3, a4, a5, a6 );
	}

	std::uintptr_t __fastcall cheat::render_smoke_map( std::uintptr_t thisptr, std::size_t size, std::uintptr_t* out_ptr )
	{
		const auto result = m_render_smoke_map.call<std::uintptr_t>( thisptr, size, out_ptr );

		if ( !lifecycle::is_unloading( ) )
		{
			features::world::g_smoke.on_map( result, size, out_ptr && *out_ptr ? *out_ptr : 0 );
		}

		return result;
	}

	void __fastcall cheat::render_smoke_unmap( std::uintptr_t thisptr, std::uintptr_t ctx, std::size_t size )
	{
		if ( !lifecycle::is_unloading( ) )
		{
			features::world::g_smoke.on_unmap( ctx );
		}
		m_render_smoke_unmap.call<void>( thisptr, ctx, size );
	}

	char __fastcall cheat::set_info( std::uintptr_t rcx, std::uintptr_t a2 )
	{
		if ( !lifecycle::is_unloading( ) )
		{
			const auto& cfg = settings::g_misc.m_name_changer;
			const auto should_override = cfg.clantag.value || cfg.override_name.value ||
				cfg.anim_nickname.value || features::misc::other::s_name_change_pending;
			if ( should_override && addresses::globals::cvar )
			{
				if ( const auto name_cvar = addresses::globals::cvar->find( "name"_hash ) )
				{
					constexpr std::uint64_t fcvar_protected = 1ull << 5;
					constexpr std::uint64_t fcvar_userinfo = 1ull << 9;
					constexpr std::uint64_t fcvar_registry_restricted = 1ull << 10;
					const auto flags_address = reinterpret_cast<std::uintptr_t>( name_cvar ) + offsetof( c_convar, m_flags );
					if ( const auto flags = memory::safe_read<std::uint64_t>( flags_address ) )
					{
						(void)memory::safe_write<std::uint64_t>( flags_address,
							( *flags | fcvar_userinfo ) & ~( fcvar_protected | fcvar_registry_restricted ) );
					}
				}
			}
		}

		// Do not inspect or rewrite a2: its layout was never verified, and the
		// old guessed pointer write corrupted the engine's KeyValues lookup.
		return m_set_info.call<char>( rcx, a2 );
	}

	void __fastcall cheat::draw_flash_effect( std::uintptr_t a1, int a2, std::uintptr_t* a3, std::uintptr_t a4, __m128* a5 )
	{
		if ( lifecycle::is_unloading( ) )
		{
			m_draw_flash_effect.call<void>( a1, a2, a3, a4, a5 );
			return;
		}

		if (safe_mode::active())
		{
			// Undo a previous partial-alpha override before the engine draws the flash.
			const auto pawn = systems::g_local.get().view_pawn();
			const auto offset = SCHEMA("C_CSPlayerPawnBase", "m_flFlashMaxAlpha"_hash);
			if (pawn && offset) memory::safe_write<float>(pawn + offset, 255.0f);
			m_draw_flash_effect.call<void>(a1, a2, a3, a4, a5);
			return;
		}

		if ( settings::g_misc.m_removals.flash_alpha.value < 100.0f && settings::g_misc.m_removals.flash_alpha.value != 0.0f )
		{
			const auto view_pawn = systems::g_local.get( ).view_pawn( );
			if ( view_pawn )
			{
				const auto max = settings::g_misc.m_removals.flash_alpha.value / 100.0f * 255.0f;
				memory::write<float>( view_pawn + SCHEMA( "C_CSPlayerPawnBase", "m_flFlashMaxAlpha"_hash ), max );
			}
		}

		if ( settings::g_misc.m_removals.flash_alpha.value != 0.0f )
		{
			m_draw_flash_effect.call<void>( a1, a2, a3, a4, a5 );
		}
	}

	void __fastcall cheat::calculate_viewmodel( std::uintptr_t thisptr, float* offsets, float* fov )
	{
		m_calculate_viewmodel.call<void>( thisptr, offsets, fov );

		if ( lifecycle::is_unloading( ) || !offsets || !fov )
			return;

		if ( !systems::g_local.get( ).is_valid( ) || !systems::g_view.has_camera( ) )
		{
			detail::g_vm_anim.initialized = false;
			return;
		}

		if ( features::misc::g_camera.is_freecam_active( ) )
		{
			offsets[ 0 ] = 0.0f;
			offsets[ 1 ] = -500.0f;
			offsets[ 2 ] = -500.0f;
			// Keep fov intact to avoid dividing by zero in engine projection matrix!
			if ( fov[ 0 ] < 10.0f || !std::isfinite( fov[ 0 ] ) )
			{
				fov[ 0 ] = 68.0f;
			}
			return;
		}

		if ( fov[ 0 ] < 10.0f || !std::isfinite( fov[ 0 ] ) )
		{
			fov[ 0 ] = 68.0f;
		}

		const auto& cfg = settings::g_misc.m_viewmodel_adjust;
		const auto now = std::chrono::steady_clock::now( );

		if ( !detail::g_vm_anim.initialized )
		{
			detail::g_vm_anim.current_x = offsets[ 0 ];
			detail::g_vm_anim.current_y = offsets[ 1 ];
			detail::g_vm_anim.current_z = offsets[ 2 ];
			detail::g_vm_anim.current_fov = fov[ 0 ];
			detail::g_vm_anim.last_time = now;
			detail::g_vm_anim.last_view_angles = systems::g_input.get_view_angles( );
			detail::g_vm_anim.initialized = true;
		}

		float dt = std::chrono::duration<float>( now - detail::g_vm_anim.last_time ).count( );
		detail::g_vm_anim.last_time = now;
		dt = std::clamp( dt, 0.0f, 0.1f );

		const auto current_angles = systems::g_input.get_view_angles( );
		auto delta_yaw = current_angles.y - detail::g_vm_anim.last_view_angles.y;
		auto delta_pitch = current_angles.x - detail::g_vm_anim.last_view_angles.x;
		detail::g_vm_anim.last_view_angles = current_angles;

		math::helpers::normalize_angle( delta_yaw );

		float target_x = offsets[ 0 ];
		float target_y = offsets[ 1 ];
		float target_z = offsets[ 2 ];
		float target_fov = fov[ 0 ];

		if ( cfg.enabled.value )
		{
			if ( std::isfinite( cfg.offset_x.value ) ) target_x = cfg.offset_x.value;
			if ( std::isfinite( cfg.offset_y.value ) ) target_y = cfg.offset_y.value;
			if ( std::isfinite( cfg.offset_z.value ) ) target_z = cfg.offset_z.value;
			if ( std::isfinite( cfg.fov.value ) && cfg.fov.value >= 10.0f ) target_fov = std::clamp( cfg.fov.value, 10.0f, 170.0f );
		}

		if ( dt > 0.0f )
		{
			constexpr float k_animation_speed = 15.0f;
			const float factor = 1.0f - std::exp( -k_animation_speed * dt );

			if ( cfg.enabled.value )
			{
				constexpr float k_sway_scale = 0.06f;
				const float target_sway_x = std::clamp( -delta_yaw * k_sway_scale, -1.5f, 1.5f );
				const float target_sway_z = std::clamp( delta_pitch * k_sway_scale, -1.5f, 1.5f );

				detail::g_vm_anim.sway_x = std::lerp( detail::g_vm_anim.sway_x, target_sway_x, factor );
				detail::g_vm_anim.sway_z = std::lerp( detail::g_vm_anim.sway_z, target_sway_z, factor );
			}
			else
			{
				detail::g_vm_anim.sway_x = std::lerp( detail::g_vm_anim.sway_x, 0.0f, factor );
				detail::g_vm_anim.sway_z = std::lerp( detail::g_vm_anim.sway_z, 0.0f, factor );
			}

			detail::g_vm_anim.current_x = std::lerp( detail::g_vm_anim.current_x, target_x, factor );
			detail::g_vm_anim.current_y = std::lerp( detail::g_vm_anim.current_y, target_y, factor );
			detail::g_vm_anim.current_z = std::lerp( detail::g_vm_anim.current_z, target_z, factor );
			detail::g_vm_anim.current_fov = std::lerp( detail::g_vm_anim.current_fov, target_fov, factor );

			const bool still_animating = cfg.enabled.value ||
				( std::abs( detail::g_vm_anim.current_x - target_x ) > 0.005f ||
				  std::abs( detail::g_vm_anim.current_y - target_y ) > 0.005f ||
				  std::abs( detail::g_vm_anim.current_z - target_z ) > 0.005f ||
				  std::abs( detail::g_vm_anim.current_fov - target_fov ) > 0.05f ||
				  std::abs( detail::g_vm_anim.sway_x ) > 0.005f ||
				  std::abs( detail::g_vm_anim.sway_z ) > 0.005f );

			if ( still_animating )
			{
				offsets[ 0 ] = detail::g_vm_anim.current_x + detail::g_vm_anim.sway_x;
				offsets[ 1 ] = detail::g_vm_anim.current_y;
				offsets[ 2 ] = detail::g_vm_anim.current_z + detail::g_vm_anim.sway_z;
				fov[ 0 ]     = detail::g_vm_anim.current_fov;
			}
			else
			{
				detail::g_vm_anim.current_x = target_x;
				detail::g_vm_anim.current_y = target_y;
				detail::g_vm_anim.current_z = target_z;
				detail::g_vm_anim.current_fov = target_fov;
			}
		}
		else if ( cfg.enabled.value )
		{
			detail::g_vm_anim.current_x = target_x;
			detail::g_vm_anim.current_y = target_y;
			detail::g_vm_anim.current_z = target_z;
			detail::g_vm_anim.current_fov = target_fov;

			offsets[ 0 ] = target_x;
			offsets[ 1 ] = target_y;
			offsets[ 2 ] = target_z;
			fov[ 0 ]     = target_fov;
		}

		const float vm_aspect_scale = features::misc::g_camera.aspect_viewmodel_scale( );
		if ( std::isfinite( vm_aspect_scale ) && vm_aspect_scale > 0.05f && std::abs( vm_aspect_scale - 1.0f ) > 0.001f )
		{
			constexpr float deg2rad = std::numbers::pi_v<float> / 360.0f;
			constexpr float rad2deg = 360.0f / std::numbers::pi_v<float>;
			const float current_vm_fov = fov[ 0 ];
			const float tan_half = std::tan( current_vm_fov * deg2rad );
			// To keep viewmodel projection untouched by custom aspect ratio:
			// tan(fov_comp / 2) * current_aspect = tan(fov_base / 2) * native_aspect
			// tan(fov_comp / 2) = tan(fov_base / 2) / (current_aspect / native_aspect) = tan(fov_base / 2) / vm_aspect_scale
			const float adjusted_fov = 2.0f * std::atan( tan_half / vm_aspect_scale ) * ( 180.0f / std::numbers::pi_v<float> );
			if ( std::isfinite( adjusted_fov ) && adjusted_fov >= 10.0f && adjusted_fov <= 170.0f )
			{
				fov[ 0 ] = adjusted_fov;
			}
		}
	}

	void __fastcall cheat::spec_cmds_handler( void* cmd )
	{
		if ( lifecycle::is_unloading( ) || !cmd || !settings::g_misc.m_camera.unlock_spectating.value )
		{
			m_spec_cmds_handler.call<void>( cmd );
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.is_valid( ) || local.is_alive )
		{
			m_spec_cmds_handler.call<void>( cmd );
			return;
		}

		// Always keep mp_forcecamera at 0 while unlock_spectating is enabled
		if ( const auto cvar = CONVAR( "mp_forcecamera" ) )
		{
			if ( cvar->m_value.i32 != 0 )
			{
				cvar->m_value.i32 = 0;
			}
		}

		const auto argc = memory::safe_read<std::int32_t>( reinterpret_cast<std::uintptr_t>( cmd ) + 0x438 ).value_or( 0 );
		const char* cmd_name = "";
		if ( argc > 0 )
		{
			const auto str_ptr = memory::safe_read<const char*>( reinterpret_cast<std::uintptr_t>( cmd ) + 0x10 ).value_or( nullptr );
			if ( str_ptr )
			{
				cmd_name = str_ptr;
			}
		}

		const bool is_next = ( std::strcmp( cmd_name, "spec_next" ) == 0 );
		const bool is_prev = ( std::strcmp( cmd_name, "spec_prev" ) == 0 );
		const bool is_player = ( std::strcmp( cmd_name, "spec_player" ) == 0 );

		if ( !is_next && !is_prev && !is_player )
		{
			m_spec_cmds_handler.call<void>( cmd );
			return;
		}

		// Collect all valid alive player pawns across all teams
		struct target_entry_t
		{
			std::uintptr_t pawn{};
			int index{};
		};

		std::vector<target_entry_t> candidates{};
		for ( const auto& p : systems::g_entities.get_by_type( systems::entities::type::player ) )
		{
			const auto controller = p.ptr;
			if ( !controller )
				continue;

			const auto is_alive = memory::read<bool>( controller + SCHEMA( "CCSPlayerController", "m_bPawnIsAlive"_hash ) );
			if ( !is_alive )
				continue;

			const auto pawn_handle = memory::read<std::uint32_t>( controller + SCHEMA( "CBasePlayerController", "m_hPawn"_hash ) );
			if ( !pawn_handle )
				continue;

			const auto pawn = systems::g_entities.lookup( pawn_handle );
			if ( !pawn )
				continue;

			if ( pawn == local.pawn )
				continue;

			const auto health = memory::read<int>( pawn + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) );
			const auto life_state = memory::read<std::uint8_t>( pawn + SCHEMA( "C_BaseEntity", "m_lifeState"_hash ) );
			if ( health <= 0 || life_state != 0 )
				continue;

			candidates.push_back( { pawn, static_cast<int>( p.index ) } );
		}

		if ( candidates.empty( ) )
		{
			m_spec_cmds_handler.call<void>( cmd );
			return;
		}

		std::sort( candidates.begin( ), candidates.end( ), []( const target_entry_t& a, const target_entry_t& b )
		{
			return a.index < b.index;
		} );

		std::uintptr_t target_pawn = 0;
		if ( is_player )
		{
			const auto argv = *reinterpret_cast<const char* const* const*>( reinterpret_cast<std::uintptr_t>( cmd ) + 0x440 );
			if ( argv && argc >= 2 && argv[ 1 ] )
			{
				const int requested_slot = std::atoi( argv[ 1 ] );
				for ( const auto& c : candidates )
				{
					if ( c.index == requested_slot )
					{
						target_pawn = c.pawn;
						break;
					}
				}
			}
		}
		else
		{
			const auto current = local.observer_pawn;
			int current_idx = -1;
			for ( size_t i = 0; i < candidates.size( ); ++i )
			{
				if ( candidates[ i ].pawn == current )
				{
					current_idx = static_cast<int>( i );
					break;
				}
			}

			if ( is_next )
			{
				const int next_idx = ( current_idx + 1 ) % static_cast<int>( candidates.size( ) );
				target_pawn = candidates[ next_idx ].pawn;
			}
			else if ( is_prev )
			{
				const int prev_idx = ( current_idx - 1 + static_cast<int>( candidates.size( ) ) ) % static_cast<int>( candidates.size( ) );
				target_pawn = candidates[ prev_idx ].pawn;
			}
		}

		if ( !target_pawn )
		{
			target_pawn = candidates[ 0 ].pawn;
		}

		const auto local_player_controller = memory::read<std::uintptr_t>( addresses::globals::local_player_controller );
		if ( !local_player_controller )
			return;

		const auto observer_pawn_handle = memory::read<std::uint32_t>( local_player_controller + SCHEMA( "CCSPlayerController", "m_hObserverPawn"_hash ) );
		if ( !observer_pawn_handle )
			return;

		const auto observer_pawn = systems::g_entities.lookup( observer_pawn_handle );
		if ( !observer_pawn )
			return;

		const auto observer_services = memory::read<std::uintptr_t>( observer_pawn + SCHEMA( "C_BasePlayerPawn", "m_pObserverServices"_hash ) );
		if ( !observer_services )
			return;

		const auto vtable = *reinterpret_cast<std::uintptr_t**>( observer_services );
		if ( vtable && vtable[ 35 ] )
		{
			using set_observer_target_fn = void( __fastcall* )( std::uintptr_t, std::uintptr_t );
			reinterpret_cast<set_observer_target_fn>( vtable[ 35 ] )( observer_services, target_pawn );
		}
	}

	int __fastcall cheat::collect_attached_entities( std::uintptr_t entity, std::uintptr_t out_vec )
	{
		if ( lifecycle::is_unloading( ) )
		{
			return m_collect_attached_entities.call<int>( entity, out_vec );
		}

		if ( !entity )
		{
			return out_vec ? memory::safe_read<int>( out_vec ).value_or( 0 ) : 0;
		}

		const auto scene_node = memory::safe_read<std::uintptr_t>(
			entity + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
		if ( !scene_node )
		{
			return out_vec ? memory::safe_read<int>( out_vec ).value_or( 0 ) : 0;
		}

		return m_collect_attached_entities.call<int>( entity, out_vec );
	}

	void cheat::trigger_lobby_music( std::uint16_t kit_id )
	{
		if ( lifecycle::is_unloading( ) || kit_id == 0xffff ) return;
        // Playback consumes the numeric kit ID, not the localized economy name.
        // A not-yet-loaded UI catalog must not silently discard the selection.
        detail::g_lobby_music_requests.submit( { kit_id, {} } );
	}

	void cheat::process_lobby_music( )
	{
		if ( lifecycle::is_unloading( ) ||
			memory::safe_read<std::uintptr_t>( addresses::globals::local_player_controller ).value_or( 0 ) ) return;
		// Teardown clears the queue. Restore the selection on lobby entry and
		// after a config load, without requiring another click on a music tile.
		// submit() deduplicates both pending and already-applied selections.
		const auto configured_kit = settings::g_changer.music.id;
		if ( configured_kit >= 0 && configured_kit < 0xffff )
			detail::g_lobby_music_requests.submit( { static_cast<std::uint16_t>( configured_kit ), {} } );
		const auto pending = detail::g_lobby_music_requests.next( );
		if ( !pending ) return;
		static auto next_retry = std::chrono::steady_clock::time_point{};
		static std::uint64_t attempted_generation{};
		static std::uint64_t abandoned_generation{};
		static unsigned bootstrap_attempts{};
		static unsigned dispatch_failures{};
		// A generation that faulted inside the game music code stays abandoned:
		// every fault aborts the whole frame_stage_notify via the exception
		// scope and kills all features for that frame. A new request generation
		// (kit/map change) re-arms the bootstrap.
		if ( pending->generation == abandoned_generation ) return;
		const auto now = std::chrono::steady_clock::now( );
		if ( pending->generation != attempted_generation )
		{
			attempted_generation = pending->generation;
			bootstrap_attempts = 0;
			dispatch_failures = 0;
			next_retry = {};
		}
		if ( now < next_retry ) return;
		// Re-select the active lobby track only on its observed engine thread.
		const auto source = detail::g_lobby_music_requests.source_for( GetCurrentThreadId( ) );
		if ( source && m_play_music.is_enabled( ) )
		{
			const auto kit = pending->value.kit ? pending->value.kit : source->original_kit;
			m_play_music.call<void>( reinterpret_cast<void*>( source->context ), 1, kit, source->volume );
			detail::g_lobby_music_requests.acknowledge( *pending );
			diag::writef( diag::level::info, "[lobby-music] replay dispatched generation=%llu kit=%u volume=%.3f",
				static_cast<unsigned long long>( pending->generation ), static_cast<unsigned>( kit ), source->volume );
			return;
		}
		// A late injection has not observed the original lobby playback yet.
		// Clear any item-preview override and ask the engine for normal menu
		// playback. A schema kit name is NOT a verified background sound event;
		// do not install it as an override with null metadata and zero volume.
		// Do not permanently abandon a selection when the audio system is not
		// ready during the first three frames/attempts. Back off instead, and
		// do not spend attempts on unavailable hooks or signatures.
		const auto stop = PATTERN( patterns::stop_item_preview_music );
		const auto update = PATTERN( patterns::update_bg_music );
		if ( !m_play_music.is_enabled( ) || !stop || !update )
		{
            diag::writef(diag::level::info,
                "[lobby-music] blocked generation=%llu hook=%d stop=%d update=%d",
                static_cast<unsigned long long>(pending->generation),
                static_cast<int>(m_play_music.is_enabled()),
                static_cast<int>(stop != 0), static_cast<int>(update != 0));
			next_retry = now + std::chrono::seconds( 2 );
			return;
		}
		next_retry = now + std::chrono::seconds( bootstrap_attempts >= 3 ? 10 : 2 );
		if ( bootstrap_attempts < 3 ) ++bootstrap_attempts;
		const bool refreshed = detail::dispatch_lobby_music_guarded( stop, update, nullptr );
		if ( !refreshed && ++dispatch_failures >= 5 )
		{
			abandoned_generation = pending->generation;
			diag::writef( diag::level::warning,
				"[lobby-music] bootstrap abandoned generation=%llu after %u failed dispatches; waiting for new request",
				static_cast<unsigned long long>( pending->generation ), dispatch_failures );
			return;
		}
		// Returning from update_bg_music is not evidence of playback. Only the
		// play_music callback below may consume this bootstrap request, including
		// when the engine invokes it on a different thread or asynchronously.
		const auto waiting = detail::g_lobby_music_requests.next( );
		if ( waiting && waiting->generation == pending->generation )
			diag::writef( diag::level::warning,
				"[lobby-music] awaiting playback generation=%llu kit=%u attempt=%u hook=%d refresh=%d thread=%lu",
				static_cast<unsigned long long>( pending->generation ), static_cast<unsigned>( pending->value.kit ),
				bootstrap_attempts, static_cast<int>( m_play_music.is_enabled( ) ), static_cast<int>( refreshed ), GetCurrentThreadId( ) );
	}

	void __fastcall cheat::play_music( void* thisptr, int track_type, std::uint16_t music_kit_id, float volume )
	{
		const bool lobby_track = track_type == 1 &&
			!memory::safe_read<std::uintptr_t>( addresses::globals::local_player_controller ).value_or( 0 );
		if ( lifecycle::is_unloading( ) || ( is_level_shutting_down( ) && !lobby_track ) )
		{
			m_play_music.call<void>( thisptr, track_type, music_kit_id, volume );
			return;
		}

		const auto local = systems::g_local.get( );
		const auto pending_lobby = lobby_track && thisptr
			? detail::g_lobby_music_requests.next( ) : std::optional<lobby_music::request>{};

		if ( track_type == 11 && local.controller )
		{
			if ( features::changer::g_music.queue_mvp_music( thisptr, track_type, music_kit_id, volume,
				[]( void* context, int track, std::uint16_t kit, float gain ) {
					if ( !lifecycle::is_unloading( ) && !is_level_shutting_down( ) && systems::g_local.get( ).controller )
						m_play_music.call<void>( context, track, kit, gain );
				} ) ) return;
			const auto winner_kit = features::changer::get_current_mvp_kit_id( );
			if ( winner_kit > 0 && winner_kit < 0xffff ) music_kit_id = static_cast<std::uint16_t>( winner_kit );
		}
		else if ( lobby_track )
		{
			detail::g_lobby_music_requests.observe( {
				reinterpret_cast<std::uintptr_t>( thisptr ), GetCurrentThreadId( ), music_kit_id, volume } );
			// Use the exact queued selection when completing a request. Kit zero
			// leaves the engine's original kit intact; preserve mute/volume too.
			const auto custom_kit = pending_lobby ? pending_lobby->value.kit : settings::g_changer.music.id;
			if ( custom_kit > 0 && custom_kit < 0xffff )
			{
				music_kit_id = static_cast<std::uint16_t>( custom_kit );
			}
		}
		else if ( !local.is_alive && local.observer_controller )
		{
			const auto spec_sid = memory::safe_read<std::uint64_t>( local.observer_controller + SCHEMA( "CBasePlayerController", "m_steamID"_hash ) ).value_or( 0 );
			if ( spec_sid )
			{
				const auto remote_spec_kit = features::changer::g_skin_sync.get_remote_music_kit( spec_sid );
				if ( remote_spec_kit > 0 && remote_spec_kit < 0xffff )
				{
					music_kit_id = static_cast<std::uint16_t>( remote_spec_kit );
				}
			}
		}
		m_play_music.call<void>( thisptr, track_type, music_kit_id, volume );
		if ( pending_lobby )
		{
			// Generation checking preserves a newer menu edit or level reset
			// while the engine is executing/re-entering the original callback.
			detail::g_lobby_music_requests.acknowledge( *pending_lobby );
			diag::writef( diag::level::info, "[lobby-music] callback dispatched generation=%llu kit=%u volume=%.3f thread=%lu",
				static_cast<unsigned long long>( pending_lobby->generation ), static_cast<unsigned>( music_kit_id ), volume, GetCurrentThreadId( ) );
		}
	}

} // namespace hooks
