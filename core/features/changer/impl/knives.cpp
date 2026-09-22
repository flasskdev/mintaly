#include <pch/pch.hpp>
#include "../preview_scene.hpp"
#include "../preview_item.hpp"
#include "../hud_weapon.hpp"
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <utilities/steam/steam.hpp>
#include <protection/game_addresses.hpp>
#include <utilities/cosmetic_model.hpp>
#include <core/features/changer/skin_sync.hpp>
#include <unordered_set>
namespace features::changer {

	namespace detail {

		inline static std::uint32_t murmurhash2_lower( const char* str, int len, std::uint32_t seed )
		{
			constexpr auto m{ 0x5bd1e995 };
			constexpr auto r{ 24 };

			auto h = seed ^ len;
			auto i{ 0 };

			while ( len >= 4 )
			{
				auto k =
					static_cast< std::uint32_t >( ( str[ i ] >= 'A' && str[ i ] <= 'Z' ) ? str[ i ] + 32 : str[ i ] ) |
					( static_cast< std::uint32_t >( ( str[ i + 1 ] >= 'A' && str[ i + 1 ] <= 'Z' ) ? str[ i + 1 ] + 32 : str[ i + 1 ] ) << 8 ) |
					( static_cast< std::uint32_t >( ( str[ i + 2 ] >= 'A' && str[ i + 2 ] <= 'Z' ) ? str[ i + 2 ] + 32 : str[ i + 2 ] ) << 16 ) |
					( static_cast< std::uint32_t >( ( str[ i + 3 ] >= 'A' && str[ i + 3 ] <= 'Z' ) ? str[ i + 3 ] + 32 : str[ i + 3 ] ) << 24 );

				k *= m;
				k ^= k >> r;
				k *= m;

				h *= m;
				h ^= k;

				i += 4;
				len -= 4;
			}

			switch ( len )
			{
			case 3: h ^= static_cast< std::uint32_t >( ( str[ i + 2 ] >= 'A' && str[ i + 2 ] <= 'Z' ) ? str[ i + 2 ] + 32 : str[ i + 2 ] ) << 16; [[fallthrough]];
			case 2: h ^= static_cast< std::uint32_t >( ( str[ i + 1 ] >= 'A' && str[ i + 1 ] <= 'Z' ) ? str[ i + 1 ] + 32 : str[ i + 1 ] ) << 8; [[fallthrough]];
			case 1: h ^= static_cast< std::uint32_t >( ( str[ i ] >= 'A' && str[ i ] <= 'Z' ) ? str[ i ] + 32 : str[ i ] ); h *= m;
			}

			h ^= h >> 13;
			h *= m;
			h ^= h >> 15;

			return h;
		}

		inline static std::uint32_t make_subclass_token( std::int16_t def_index )
		{
			const auto s = std::to_string( def_index );
			return murmurhash2_lower( s.c_str( ), static_cast< int >( s.length( ) ), 0x31415926 );
		}

		struct remote_knife_state {
			std::uint16_t def_index{ 0 };
			int paint_kit{ 0 };
			std::uint32_t subclass{ 0 };
			std::uint64_t item_id{ 0 };
			int quality{ 0 };
			bool disallow_soc{ false };
			bool initialized{ false };
		};
		inline static std::unordered_map<std::uint32_t, remote_knife_state> s_remote_knives;

		struct schema_offsets {
			int subclass_id{};
			int fallback_paint_kit{};
			int fallback_seed{};
			int fallback_wear{};
			int fallback_stattrak{};
			int item_id{};
			int item_def_index{};
			int account_id{};
			int entity_quality{};
			int initialized{};
			int disallow_soc{};
			int item_id_high{};
			int item_id_low{};
			int scene_node{};
			int model_state{};
			int model_handle{};
			int weapon_services{};
			int my_weapons{};
			int active_weapon{};
			int hud_model_arms{};
			int need_reapply{};
			int steam_id{};
			int game_rules{};
			int round_start_time{};
			int attribute_manager{};
			int item{};
			int original_owner_low{};
			int original_owner_high{};

			void init() {
				subclass_id = SCHEMA("C_BaseEntity", "m_nSubclassID"_hash);
				fallback_paint_kit = SCHEMA("C_EconEntity", "m_nFallbackPaintKit"_hash);
				fallback_seed = SCHEMA("C_EconEntity", "m_nFallbackSeed"_hash);
				fallback_wear = SCHEMA("C_EconEntity", "m_flFallbackWear"_hash);
				fallback_stattrak = SCHEMA("C_EconEntity", "m_nFallbackStatTrak"_hash);
				item_id = SCHEMA("C_EconItemView", "m_iItemID"_hash);
				item_def_index = SCHEMA("C_EconItemView", "m_iItemDefinitionIndex"_hash);
				account_id = SCHEMA("C_EconItemView", "m_iAccountID"_hash);
				entity_quality = SCHEMA("C_EconItemView", "m_iEntityQuality"_hash);
				initialized = SCHEMA("C_EconItemView", "m_bInitialized"_hash);
				disallow_soc = SCHEMA("C_EconItemView", "m_bDisallowSOC"_hash);
				item_id_high = SCHEMA("C_EconItemView", "m_iItemIDHigh"_hash);
				item_id_low = SCHEMA("C_EconItemView", "m_iItemIDLow"_hash);
				scene_node = SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash);
				model_state = SCHEMA("CSkeletonInstance", "m_modelState"_hash);
				model_handle = SCHEMA("CModelState", "m_hModel"_hash);
				weapon_services = SCHEMA("C_BasePlayerPawn", "m_pWeaponServices"_hash);
				my_weapons = SCHEMA("CPlayer_WeaponServices", "m_hMyWeapons"_hash);
				active_weapon = SCHEMA("CPlayer_WeaponServices", "m_hActiveWeapon"_hash);
				hud_model_arms = SCHEMA("C_CSPlayerPawn", "m_hHudModelArms"_hash);
				need_reapply = SCHEMA("C_CSPlayerPawn", "m_bNeedToReApplyGloves"_hash);
				steam_id = SCHEMA("CBasePlayerController", "m_steamID"_hash);
				game_rules = SCHEMA("C_CSGameRules", "m_fRoundStartTime"_hash);
				round_start_time = SCHEMA("C_CSGameRules", "m_fRoundStartTime"_hash);
				attribute_manager = SCHEMA("C_EconEntity", "m_AttributeManager"_hash);
				item = SCHEMA("C_AttributeContainer", "m_Item"_hash);
				original_owner_low = SCHEMA("C_EconEntity", "m_OriginalOwnerXuidLow"_hash);
				original_owner_high = SCHEMA("C_EconEntity", "m_OriginalOwnerXuidHigh"_hash);
			}
		};

		inline schema_offsets& get_offsets() {
			static schema_offsets offsets;
			static bool init = (offsets.init(), true);
			return offsets;
		}

	} // namespace detail

	void knives::on_frame_stage_notify( )
	{
		this->process_hud_clear( );

		// If settings changed (e.g. user selected a different knife in the skin
		// changer), clear the cached override so we re-apply immediately.
		if ( this->m_invalidate_pending.exchange( false ) )
		{
			this->m_overridden = false;
		}

		const auto local = systems::g_local.get( );
		if ( !local.controller )
		{
			return;
		}

		const auto local_ctrl = local.controller;
		const auto local_pawn = preview_scene::player_pawn( local_ctrl );
		const auto local_team = local_pawn ? preview_scene::team( local_pawn ) : 0;

		if ( preview_scene::player_ready( local_pawn ) )
		{
			const auto weapon_services = memory::read<std::uintptr_t>( local_pawn + SCHEMA( "C_BasePlayerPawn", "m_pWeaponServices"_hash ) );
			if ( weapon_services )
			{
				const settings::changer::applied_skin* selected_skin{ nullptr };
				const econ_item_system::item_def* selected_knife_def{ nullptr };

				for ( const auto& [def_idx, skin] : settings::g_changer.skins.for_team( local_team ) )
				{
					const auto def = g_econ_item_system.find_def( def_idx );
					if ( !def || def->category != econ_item_system::item_category::knife )
					{
						continue;
					}

					selected_skin = &skin;
					selected_knife_def = def;
					break;
				}

				const auto cur_knife_def = selected_knife_def ? selected_knife_def->def_index : 0;
				const auto cur_paint_kit = selected_skin ? selected_skin->paint_kit_id : 0;
				const auto cur_seed = selected_skin ? selected_skin->seed : 0;
				const auto cur_wear = selected_skin ? selected_skin->wear : 0.0f;
				if ( cur_knife_def != this->m_last_knife_def || cur_paint_kit != this->m_last_paint_kit ||
					cur_seed != this->m_last_seed || cur_wear != this->m_last_wear )
				{
					this->m_overridden = false;
					this->m_last_knife_def = cur_knife_def;
					this->m_last_paint_kit = cur_paint_kit;
					this->m_last_seed = cur_seed;
					this->m_last_wear = cur_wear;
				}

				const auto hud_model = this->find_hud_model_weapon( local_pawn );
				const auto rules = memory::safe_read<std::uintptr_t>( addresses::globals::game_rules ).value_or( 0 );
				const auto round_offset = SCHEMA( "C_CSGameRules", "m_fRoundStartTime"_hash );
				const auto round_time = rules && round_offset ? memory::safe_read<float>( rules + round_offset ).value_or( 0.0f ) : 0.0f;
				if ( hud_model != this->m_last_hud_model || round_time != this->m_last_round_start_time )
				{
					this->m_overridden = false;
					this->m_last_hud_model = hud_model;
					this->m_last_round_start_time = round_time;
				}

				const auto weapons_base = weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hMyWeapons"_hash );
				const auto weapons_size = memory::read<int>( weapons_base );
				const auto weapons_data = memory::read<std::uintptr_t>( weapons_base + 0x8 );

				if ( weapons_data && weapons_size > 0 && weapons_size <= 64 )
				{
					const auto active_handle = memory::read<std::uint32_t>( weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hActiveWeapon"_hash ) );
					const auto active_weapon = systems::g_entities.lookup( active_handle );

					if ( this->m_tracked_pawn != local_pawn )
					{
						this->m_original = {};
						this->m_overridden = false;
						this->m_last_active_handle = 0;
						this->m_tracked_pawn = local_pawn;
					}

					for ( auto i = 0; i < weapons_size; ++i )
					{
						const auto handle = memory::read<std::uint32_t>( weapons_data + i * sizeof( std::uint32_t ) );
						const auto weapon = systems::g_entities.lookup( handle );

						if ( !weapon )
						{
							continue;
						}

						const auto iv = weapon + SCHEMA( "C_EconEntity", "m_AttributeManager"_hash ) + SCHEMA( "C_AttributeContainer", "m_Item"_hash );
						const auto current_def_index = memory::read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
						const auto current_def = g_econ_item_system.find_def( static_cast< std::int16_t >( current_def_index ) );

						if ( !current_def || current_def->category != econ_item_system::item_category::knife )
						{
							continue;
						}

						if ( !memory::read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) + 0x8 ) )
						{
							continue;
						}

						if ( this->m_tracked_weapon_handle != handle )
						{
							this->m_original = {};
							this->m_overridden = false;
							this->m_tracked_weapon_handle = handle;
						}

						if ( !selected_knife_def )
						{
							this->restore( weapon, iv, active_weapon, local_pawn );
							break;
						}

						if ( !this->m_original.captured )
						{
							this->capture_original( weapon, iv );
						}

						if (!this->m_original.captured) continue;
						const auto normalized_skin = cosmetic_attributes::normalize(*selected_skin);
						selected_skin = &normalized_skin;
						const auto target_token = detail::make_subclass_token( selected_knife_def->def_index );
						const auto current_subclass = memory::read<std::uint32_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) );
						const auto current_pk = memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) );

						if ( this->m_overridden && preview_item::matches( weapon, iv, *selected_skin, static_cast<std::uint32_t>(steam::user::get_steam_id()), selected_skin->stattrak ? 9 : 3 ) && current_def_index == static_cast<std::uint16_t>( selected_knife_def->def_index )
							&& cosmetic_attributes::matches( iv, *selected_skin )
							&& current_subclass == target_token && current_pk == selected_skin->paint_kit_id
							&& memory::read<int>(weapon + SCHEMA("C_EconEntity", "m_nFallbackSeed"_hash)) == selected_skin->seed
							&& memory::read<float>(weapon + SCHEMA("C_EconEntity", "m_flFallbackWear"_hash)) == selected_skin->wear
							&& name_tag::matches(iv, selected_skin->name_tag)
							&& memory::read<int>(weapon + SCHEMA("C_EconEntity", "m_nFallbackStatTrak"_hash)) == (selected_skin->stattrak ? selected_skin->stattrak_count : -1) )
						{
							break;
						}

						auto steam_id = steam::user::get_steam_id( );
						if ( !steam_id && local_ctrl )
						{
							steam_id = memory::read<std::uint64_t>( local_ctrl + SCHEMA( "CBasePlayerController", "m_steamID"_hash ) );
						}
						const auto account_id = static_cast< std::uint32_t >( steam_id & 0xffffffff );

						this->apply( weapon, iv, selected_knife_def, selected_skin, account_id, active_weapon, local_pawn );
						break;
					}

					// A HUD weapon may be created AFTER the first apply without changing
					// m_hActiveWeapon. Reconcile it even when that handle is unchanged.
					{
						this->m_last_active_handle = active_handle;

						if ( this->m_overridden && active_weapon )
						{
							const auto iv = active_weapon + SCHEMA( "C_EconEntity", "m_AttributeManager"_hash ) + SCHEMA( "C_AttributeContainer", "m_Item"_hash );
							const auto def_index = memory::read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
							const auto def = g_econ_item_system.find_def( static_cast< std::int16_t >( def_index ) );

							if ( def && def->category == econ_item_system::item_category::knife )
							{
								const auto paint_kit_id = memory::read<int>( active_weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) );
								const auto pk = g_econ_item_system.find_paint_kit( paint_kit_id );
								this->update_view_model( local_pawn, pk );
							}
						}
					}
				}
			}
		}

		// Restore synced knives if sync was disabled
		if ( !g_skin_sync.is_enabled( ) )
		{
			for ( auto it = detail::s_remote_knives.begin( ); it != detail::s_remote_knives.end( ); )
			{
				const auto handle = it->first;
				const auto& state = it->second;
				const auto weapon = systems::g_entities.lookup( handle );
				if ( weapon )
				{
					const auto entity = entity_guard::capture( weapon );
					if ( entity )
					{
						const auto iv = weapon + SCHEMA( "C_EconEntity", "m_AttributeManager"_hash ) + SCHEMA( "C_AttributeContainer", "m_Item"_hash );
						const auto definition = SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash );
						if ( definition && iv )
						{
							memory::write<std::uint16_t>( iv + definition, state.def_index );
							memory::write<std::uint64_t>( iv + SCHEMA( "C_EconItemView", "m_iItemID"_hash ), state.item_id );
							memory::write<bool>( iv + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ), state.disallow_soc );
							memory::write<int>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ), state.quality );
							memory::write<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), state.initialized );
							memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ), state.paint_kit );
							this->update_model( weapon, iv, state.def_index, true );
							entity_guard::rebuild_materials( *entity, 1 );
						}
					}
				}
				it = detail::s_remote_knives.erase( it );
			}
		}

		// Apply synced knives for remote players
		if ( g_skin_sync.is_enabled( ) )
		{
			const auto all_players = systems::g_entities.get_by_type( systems::entities::type::player );
			for ( const auto& p : all_players )
			{
				const auto ctrl = p.ptr;
				if ( !ctrl || ctrl == local_ctrl )
				{
					continue;
				}

				const auto sid = memory::safe_read<std::uint64_t>( ctrl + SCHEMA( "CBasePlayerController", "m_steamID"_hash ) ).value_or( 0 );
				constexpr std::uint64_t steam_id_base = 76561197960265728ull;
				if ( sid < steam_id_base && !g_skin_sync.m_bot_sync_test.load( ) )
				{
					continue;
				}

				const auto pawn = preview_scene::player_pawn( ctrl );
				if ( !preview_scene::player_ready( pawn ) )
				{
					continue;
				}

				const auto remote_weapon_services = memory::safe_read<std::uintptr_t>( pawn + SCHEMA( "C_BasePlayerPawn", "m_pWeaponServices"_hash ) ).value_or( 0 );
				if ( !remote_weapon_services )
				{
					continue;
				}

				const auto remote_weapons_base = remote_weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hMyWeapons"_hash );
				const auto remote_weapons_size = memory::safe_read<int>( remote_weapons_base ).value_or( 0 );
				const auto remote_weapons_data = memory::safe_read<std::uintptr_t>( remote_weapons_base + 0x8 ).value_or( 0 );
				if ( !remote_weapons_data || remote_weapons_size <= 0 || remote_weapons_size > 64 )
				{
					continue;
				}

				const auto remote_skin_data = g_skin_sync.get_remote_skin( sid );
				settings::changer::applied_skin remote_knife_skin{};
				const econ_item_system::item_def* remote_knife_def{ nullptr };
				bool has_knife_skin{ false };
				if ( remote_skin_data && !remote_skin_data->skins.empty( ) )
				{
					for ( const auto& [def_idx, skin] : remote_skin_data->skins )
					{
						const auto def = g_econ_item_system.find_def( def_idx );
						if ( def && def->category == econ_item_system::item_category::knife )
						{
							remote_knife_skin = cosmetic_attributes::normalize( skin );
							remote_knife_def = def;
							has_knife_skin = true;
							break;
						}
					}
				}

				const auto remote_active_handle = memory::safe_read<std::uint32_t>( remote_weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hActiveWeapon"_hash ) ).value_or( 0 );
				const auto remote_active_weapon = systems::g_entities.lookup( remote_active_handle );
				const auto remote_account_id = static_cast< std::uint32_t >( sid & 0xffffffff );

				for ( auto i = 0; i < remote_weapons_size; ++i )
				{
					const auto handle = memory::safe_read<std::uint32_t>( remote_weapons_data + i * sizeof( std::uint32_t ) ).value_or( 0 );
					if ( !handle )
					{
						continue;
					}

					const auto weapon = systems::g_entities.lookup( handle );
					if ( !weapon || weapon < 0x10000 )
					{
						continue;
					}

					const auto iv = weapon + SCHEMA( "C_EconEntity", "m_AttributeManager"_hash ) + SCHEMA( "C_AttributeContainer", "m_Item"_hash );
					const auto current_def_index = memory::read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
					const auto current_def = g_econ_item_system.find_def( static_cast< std::int16_t >( current_def_index ) );
					if ( !current_def || current_def->category != econ_item_system::item_category::knife )
					{
						continue;
					}

					if ( !has_knife_skin || !remote_knife_def )
					{
						const auto it = detail::s_remote_knives.find( handle );
						if ( it != detail::s_remote_knives.end( ) )
						{
							const auto& state = it->second;
							const auto entity = entity_guard::capture( weapon );
							if ( entity )
							{
								const auto definition = SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash );
								if ( definition && iv )
								{
									memory::write<std::uint16_t>( iv + definition, state.def_index );
									memory::write<std::uint64_t>( iv + SCHEMA( "C_EconItemView", "m_iItemID"_hash ), state.item_id );
									memory::write<bool>( iv + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ), state.disallow_soc );
									memory::write<int>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ), state.quality );
									memory::write<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), state.initialized );
									memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ), state.paint_kit );
									this->update_model( weapon, iv, state.def_index, true );
									entity_guard::rebuild_materials( *entity, 1 );
								}
							}
							detail::s_remote_knives.erase( it );
						}
						continue;
					}

					const auto target_token = detail::make_subclass_token( remote_knife_def->def_index );
					const auto current_subclass = memory::safe_read<std::uint32_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) ).value_or( 0 );
					const auto current_pk = memory::safe_read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) ).value_or( 0 );

					if ( detail::s_remote_knives.find( handle ) == detail::s_remote_knives.end( ) )
					{
						detail::remote_knife_state saved{};
						saved.def_index = current_def_index;
						saved.paint_kit = current_pk;
						saved.subclass = current_subclass;
						saved.item_id = memory::read<std::uint64_t>( iv + SCHEMA( "C_EconItemView", "m_iItemID"_hash ) );
						saved.quality = memory::read<int>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ) );
						saved.disallow_soc = memory::read<bool>( iv + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ) );
						saved.initialized = memory::read<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ) );
						detail::s_remote_knives[ handle ] = saved;
					}

					if ( preview_item::matches( weapon, iv, remote_knife_skin, remote_account_id, remote_knife_skin.stattrak ? 9 : 3 ) &&
						current_def_index == static_cast<std::uint16_t>( remote_knife_def->def_index ) &&
						current_subclass == target_token && current_pk == remote_knife_skin.paint_kit_id &&
						cosmetic_attributes::matches( iv, remote_knife_skin ) && name_tag::matches( iv, remote_knife_skin.name_tag ) )
					{
						if ( weapon == remote_active_weapon && pawn == systems::g_local.get( ).observer_pawn )
						{
							const auto pk = g_econ_item_system.find_paint_kit( remote_knife_skin.paint_kit_id );
							this->update_view_model( pawn, pk );
						}
						break;
					}

					this->apply( weapon, iv, remote_knife_def, &remote_knife_skin, remote_account_id, remote_active_weapon, pawn );
					break;
				}
			}
		}
	}

	void knives::capture_original( std::uintptr_t weapon, std::uintptr_t iv )
	{
		if ( this->m_original.captured )
		{
			return;
		}

		if (!cosmetic_attributes::capture(iv, this->m_original.attributes)) return;
        this->m_original.custom_name = name_tag::capture(iv);
        this->m_original.item_id = memory::read<std::uint64_t>(iv + SCHEMA("C_EconItemView", "m_iItemID"_hash));
        this->m_original.quality = memory::read<int>(iv + SCHEMA("C_EconItemView", "m_iEntityQuality"_hash));
        this->m_original.disallow_soc = memory::read<bool>(iv + SCHEMA("C_EconItemView", "m_bDisallowSOC"_hash));
		this->m_original.def_index = memory::read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
		this->m_original.id_high = memory::read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ) );
		this->m_original.id_low = memory::read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ) );
		this->m_original.account_id = memory::read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ) );
		this->m_original.initialized = memory::read<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ) );
		this->m_original.paint_kit = memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) );
		this->m_original.seed = memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ) );
		this->m_original.wear = memory::read<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ) );
		this->m_original.stattrak = memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ) );
		this->m_original.captured = true;
	}

	void knives::apply( std::uintptr_t weapon, std::uintptr_t iv, const econ_item_system::item_def* def, const settings::changer::applied_skin* skin, std::uint32_t account_id, std::uintptr_t active_weapon, std::uintptr_t pawn )
	{
		this->m_pending_hud_iv = 0;
		const auto entity = entity_guard::capture( weapon );
		if ( !entity || !entity_guard::ready( pawn ) || !skin || !def || !preview_item::available( ) || !preview_item::identity( weapon ).ready( ) ) return;
		const auto definition = SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash );
		if ( !definition ) return;
		const auto local_sys = systems::g_local.get( );
		const bool local = pawn == local_sys.pawn;
		preview_item::applied.erase( weapon );
		if ( local ) this->m_overridden = false;
		memory::write<std::uint16_t>( iv + definition, static_cast<std::uint16_t>( def->def_index ) );
		// Remote match weapons also need subclass reconciliation, not the lobby shortcut.
		if ( !this->update_model( weapon, iv, static_cast<std::uint16_t>( def->def_index ) ) || !entity_guard::current( *entity ) ) return;
		memory::write<std::uint64_t>( iv + SCHEMA( "C_EconItemView", "m_iItemID"_hash ), 0xf000000000000010ull );
		memory::write<bool>( iv + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ), true );
		memory::write<int>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ), skin->stattrak ? 9 : 3 );
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ), 0xf0000000 );
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ), 0x10 );
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ), account_id );
		memory::write<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), true );
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ), skin->paint_kit_id );
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ), skin->seed );
		memory::write<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ), skin->wear );
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ), skin->stattrak ? skin->stattrak_count : -1 );
		if ( !cosmetic_attributes::apply( iv, *skin ) || !entity_guard::current( *entity ) ) return;
		if ( !name_tag::apply( iv, skin->name_tag ) || !entity_guard::current( *entity ) ) return;
		const auto pk = g_econ_item_system.find_paint_kit( skin->paint_kit_id );
		if ( !this->rebuild_paint( weapon, active_weapon, pawn, pk ) || !entity_guard::current( *entity ) ) return;
		this->schedule_hud_clear( iv );
		if ( !entity_guard::current( *entity ) ) return;
		preview_item::remember( weapon, *skin );
		if ( local ) this->m_overridden = true;
	}

	void knives::restore( std::uintptr_t weapon, std::uintptr_t iv, std::uintptr_t active_weapon, std::uintptr_t pawn )
	{
		if ( !this->m_overridden || !this->m_original.captured ) return;
		const auto entity = entity_guard::capture( weapon );
		if ( !entity || !entity_guard::ready( pawn ) ) return;
		this->m_pending_hud_iv = 0;
		if ( !cosmetic_attributes::restore( iv, this->m_original.attributes ) || !entity_guard::current( *entity ) ) return;
		if ( this->m_original.custom_name && !name_tag::restore( iv, *this->m_original.custom_name ) ) return;
		if ( !entity_guard::current( *entity ) ) return;
		memory::write<std::uint64_t>( iv + SCHEMA( "C_EconItemView", "m_iItemID"_hash ), this->m_original.item_id );
		memory::write<bool>( iv + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ), this->m_original.disallow_soc );
		memory::write<int>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ), this->m_original.quality );
		memory::write<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ), this->m_original.def_index );
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ), this->m_original.id_high );
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ), this->m_original.id_low );
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ), this->m_original.account_id );
		memory::write<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), this->m_original.initialized );
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ), this->m_original.paint_kit );
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ), this->m_original.seed );
		memory::write<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ), this->m_original.wear );
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ), this->m_original.stattrak );
		if ( !this->update_model( weapon, iv, this->m_original.def_index ) || !entity_guard::current( *entity ) ) return;
		const auto pk = g_econ_item_system.find_paint_kit( this->m_original.paint_kit );
		if ( !this->rebuild_paint( weapon, active_weapon, pawn, pk ) || !entity_guard::current( *entity ) ) return;
		this->schedule_hud_clear( iv );
		if ( !entity_guard::current( *entity ) ) return;
		preview_item::applied.erase( weapon );
		this->m_overridden = false;
	}

	bool knives::update_model( std::uintptr_t weapon, std::uintptr_t iv, std::uint16_t def_index, bool lobby )
	{
		const auto entity = entity_guard::capture( weapon );
		const auto subclass = SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash );
		const auto get_model = PATTERN( patterns::weapon_get_model_path );
		if ( !entity || !subclass || !get_model ) return false;
		const auto token = detail::make_subclass_token( static_cast<std::int16_t>( def_index ) );
		const bool changed = memory::safe_read<std::uint32_t>( weapon + subclass ).value_or( 0 ) != token;
		const auto bind = PATTERN( patterns::weapon_get_viewmodel );
		if ( !lobby && !bind ) return false;
		// World/player geometry must not be selected from the first-person model path.
		const auto def = g_econ_item_system.find_def( static_cast<std::int16_t>( def_index ) );
		auto target = def ? def->model_player : std::string{};
		if ( target.empty( ) )
		{
			const auto path = memory::call<const char*>( get_model, iv );
			if ( path ) target = memory::read_string( reinterpret_cast<std::uintptr_t>( path ) );
		}
		if ( target.empty( ) || !entity_guard::current( *entity ) ) return false;
		const auto state = SCHEMA( "CSkeletonInstance", "m_modelState"_hash );
		const auto name_offset = SCHEMA( "CModelState", "m_ModelName"_hash );
		const auto scene = memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
		if ( !state || !name_offset || !scene ) return false;
		const auto name = memory::safe_read<std::uintptr_t>( scene + state + name_offset ).value_or( 0 );
		const bool model_changed = !name || !cosmetic_model::matches( memory::read_string( name ), target );
		if ( model_changed && !entity_guard::set_model( *entity, target.c_str( ) ) ) return false;
		memory::write<std::uint32_t>( weapon + subclass, token );
		// SetModel can reset animation state. Rebind only after model replacement,
		// including remote players, so dual knives use their own subclass animations.
		if ( !lobby && ( changed || model_changed ) )
			memory::call<void>( bind, weapon );
		return entity_guard::current( *entity );
	}

	bool knives::rebuild_paint( std::uintptr_t weapon, std::uintptr_t active_weapon, std::uintptr_t pawn, const econ_item_system::paint_kit* pk )
	{
		const auto entity = entity_guard::capture( weapon );
		if ( !entity || !entity_guard::ready( pawn ) ) return false;
		const auto mesh = pk && pk->legacy_model ? std::uint64_t{2} : std::uint64_t{1};
		const auto local_sys = systems::g_local.get( );
		const bool is_viewed = ( pawn == local_sys.pawn || ( local_sys.observer_pawn && pawn == local_sys.observer_pawn ) );
		const bool needs_hud = weapon == active_weapon &&
			( is_viewed || this->find_hud_model_weapon( pawn ) != 0 );
		if ( !entity_guard::rebuild_materials( *entity, mesh ) ) return false;
		if ( needs_hud && !this->update_view_model( pawn, pk ) ) return false;
		return true;
	}

	bool knives::update_view_model( std::uintptr_t pawn, const econ_item_system::paint_kit* pk, bool force )
	{
		return hud_weapon::update( pawn, pk && pk->legacy_model ? std::uint64_t{2} : std::uint64_t{1}, force );
	}

	std::uintptr_t knives::find_hud_model_weapon( std::uintptr_t pawn )
	{
		return hud_weapon::find( pawn );
	}

	void knives::clear_hud_icon( std::uintptr_t iv )
	{
		const auto invalidate = PATTERN( patterns::econ_item_view_invalidate_description );
		if ( iv && invalidate )
		{
			memory::call<void>( invalidate, iv );
		}
	}

	void knives::schedule_hud_clear( std::uintptr_t iv )
	{
		this->clear_hud_icon( iv );
		this->m_pending_hud_iv = iv;
		this->m_hud_clear_time = std::chrono::steady_clock::now( ) + std::chrono::milliseconds( 200 );
	}

	void knives::process_hud_clear( )
	{
		this->m_pending_hud_iv = 0;
	}

	void knives::on_lobby( )
	{
		// Preview entities are engine-owned. Match equipment is applied only
		// through on_frame_stage_notify, never through lobby/intro previews.
	}

	void knives::reset( )
	{
		this->m_original = {};
		this->m_overridden = false;
		this->m_tracked_pawn = 0;
		this->m_tracked_weapon_handle = 0;
		this->m_last_active_handle = 0;
		this->m_last_hud_model = 0;
		this->m_last_round_start_time = 0.0f;
		this->m_last_knife_def = 0;
		this->m_last_paint_kit = 0;
		this->m_last_seed = 0;
		this->m_last_wear = 0.0f;
		this->m_pending_hud_iv = 0;
		this->m_hud_clear_time = {};
		this->m_invalidate_pending = false;
		detail::s_remote_knives.clear( );
	}

} // namespace features::changer
