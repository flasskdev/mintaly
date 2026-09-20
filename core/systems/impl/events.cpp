#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <core/features/features.hpp>
#include <protection/game_addresses.hpp>

#include "../systems.hpp"

namespace systems {

	bool events::initialize( )
	{
		// Resolve hot-path addresses before listeners receive their first event.
		const protection::addresses::address_t* const hot_patterns[] =
		{
			&patterns::game_event_get_controller,
			&patterns::game_event_get_float,
			&patterns::game_event_get_int,
			&patterns::game_event_get_pawn,
			&patterns::game_event_get_string,
			&patterns::base_fire_guns_get_inaccuracy,
			&patterns::find_hud_element,
			&patterns::set_voice_data,
			&patterns::print_hud_chat,
			&patterns::play_sound,
			&patterns::init_particle_path_buffer,
			&patterns::resource_system_precache,
			&patterns::particle_create_effect,
			&patterns::particle_set_control_point,
			&patterns::particle_set_entity_binding,
			&patterns::econ_item_view_set_attribute,
			&patterns::engine_client_cmd
		};

		const auto warmup_started = std::chrono::steady_clock::now( );
		std::size_t resolved{};
		for ( const auto* pattern : hot_patterns )
		{
			if ( memory::resolve_pattern_cached( pattern->data.data ) )
			{
				++resolved;
			}
		}

		const auto warmup_ms = std::chrono::duration<double, std::milli>(
			std::chrono::steady_clock::now( ) - warmup_started ).count( );
		diag::writef( diag::level::info,
			"event signature warmup: %zu/%zu resolved in %.2f ms",
			resolved, sizeof( hot_patterns ) / sizeof( hot_patterns[ 0 ] ), warmup_ms );

		const bool registered =
			register_listener( xs( "bullet_impact" ), [ ]( void* event ) { features::misc::g_impacts.on_bullet_impact( reinterpret_cast<std::uintptr_t>( event ) ); } ) &&
			register_listener( xs( "player_hurt" ), [ ]( void* event ) { features::misc::g_impacts.on_player_hurt( reinterpret_cast<std::uintptr_t>( event ) ); } ) &&
			register_listener( xs( "round_start" ), [ ]( void* event ) { features::esp::player::g_overlay.reset_sounds( ); features::misc::g_other.on_round_start( ); features::changer::g_music.clear_mvp( ); } ) &&
			register_listener( xs( "player_death" ), [ ]( void* event ) { features::misc::g_other.on_player_death( reinterpret_cast<std::uintptr_t>( event ) ); } ) &&
			register_listener( xs( "vote_cast" ), [ ]( void* event ) { features::misc::g_vote_logs.on_vote_cast( reinterpret_cast<std::uintptr_t>( event ) ); } ) &&
			register_listener( xs( "vote_failed" ), [ ]( void* event ) { features::misc::g_vote_logs.on_vote_failed_event( reinterpret_cast<std::uintptr_t>( event ) ); } ) &&
			register_listener( xs( "round_mvp" ), [ ]( void* event ) { features::changer::g_music.on_round_mvp( event ); } );
		if ( !registered )
		{
			shutdown( );
			return false;
		}
		// Optional events must not prevent the core listeners from working.
		for ( const auto* name : { "player_footstep", "weapon_fire" } )
		{
			if ( !register_listener( name, [ ]( void* event ) { features::esp::player::g_overlay.on_sound_event( event ); } ) )
			{
				diag::writef( diag::level::warning, "ESP sound event unavailable: %s", name );
			}
		}
		return true;
	}

	void events::shutdown( )
	{
		for ( auto& entry : m_listeners )
		{
			if ( entry->registered )
			{
				memory::call_vfunc<void>( addresses::globals::game_event_manager, 5, &entry->listener );
				entry->registered = false;
			}
		}
		m_listeners.clear( );
	}

	bool events::register_listener( const char* event_name, handler_fn handler )
	{
		if ( !event_name || !handler )
		{
			return false;
		}

		auto current_entry = std::make_unique<entry>( );
		current_entry->handler = handler;
		// Own the name: xs() may return storage in a temporary xor string.
		current_entry->name = event_name;
		current_entry->registered = false;
		current_entry->vtable_data[ 0 ] = nullptr;
		current_entry->vtable_data[ 1 ] = reinterpret_cast<void*>( &fire_event );
		current_entry->vtable_data[ 2 ] = reinterpret_cast<void*>( &get_debug_id );
		current_entry->listener.vtable = current_entry->vtable_data;
		current_entry->listener.debug_id = static_cast<int>( m_listeners.size( ) + 1 );

		// Allocate vector storage before publishing the listener to the engine.
		m_listeners.reserve( m_listeners.size( ) + 1 );
		const auto success = memory::call_vfunc<bool>( addresses::globals::game_event_manager, 3,
			&current_entry->listener, current_entry->name.c_str( ), false );
		if ( !success )
		{
			return false;
		}

		current_entry->registered = true;
		m_listeners.push_back( std::move( current_entry ) );
		return true;
	}

	void events::unregister_listener( const char* event_name )
	{
		if ( !event_name )
		{
			return;
		}
		for ( auto it = m_listeners.begin( ); it != m_listeners.end( ); ++it )
		{
			if ( ( *it )->name == event_name && ( *it )->registered )
			{
				memory::call_vfunc<void>( addresses::globals::game_event_manager, 5, &( *it )->listener );
				( *it )->registered = false;
				m_listeners.erase( it );
				return;
			}
		}
	}

	void* __fastcall events::fire_event( void* self, void* event )
	{
		if ( !self || !event )
		{
			return nullptr;
		}
		diag::exception_scope scope{ "game event callback" };
		for ( const auto& entry : m_listeners )
		{
			// Debug IDs can be reused after removal; listener addresses cannot.
			if ( &entry->listener == self && entry->registered && entry->handler )
			{
				const auto started = std::chrono::steady_clock::now( );
				entry->handler( event );
				const auto elapsed = std::chrono::duration<double, std::milli>(
					std::chrono::steady_clock::now( ) - started ).count( );
				if ( elapsed >= 5.0 )
					diag::writef( diag::level::warning, "slow game event: %s elapsed_ms=%.3f", entry->name.c_str( ), elapsed );
				break;
			}
		}
		return nullptr;
	}

	int __fastcall events::get_debug_id( void* self )
	{
		return self ? static_cast<listener*>( self )->debug_id : 0;
	}

	std::uintptr_t events::get_controller( void* event, const char* key_name )
	{
		if ( !event || !key_name )
		{
			return 0;
		}

		// The accessor already returns a controller. Reading pawn fields here
		// is type confusion, not a valid way to identify an entity's class.
		const auto key = cstypes::event_hash{ key_name };
		const auto fn = PATTERN( patterns::game_event_get_controller );
		if ( fn )
		{
			return memory::call<std::uintptr_t>( fn, event, &key );
		}
		return memory::call_vfunc<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( event ), 16, &key );
	}

	std::uintptr_t events::get_pawn( void* event, const char* key_name )
	{
		if ( !event || !key_name )
		{
			return 0;
		}

		const auto key = cstypes::event_hash{ key_name };
		const auto fn = PATTERN( patterns::game_event_get_pawn );
		const auto pawn = fn
			? memory::call<std::uintptr_t>( fn, event, &key )
			: memory::call_vfunc<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( event ), 17, &key );
		if ( pawn )
		{
			return pawn;
		}

		// A death event may arrive after m_hPawn switched to an observer pawn.
		const auto controller = get_controller( event, key_name );
		const auto offset = SCHEMA( "CCSPlayerController", "m_hPlayerPawn"_hash );
		if ( !controller || !offset )
		{
			return 0;
		}
		const auto handle = memory::safe_read<std::uint32_t>( controller + offset ).value_or( 0 );
		return g_entities.lookup( handle );
	}

} // namespace systems
