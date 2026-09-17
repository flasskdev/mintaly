#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <utilities/steam/steam.hpp>
#include <protection/game_addresses.hpp>
namespace features::changer {

	void guns::on_frame_stage_notify( )
	{
		this->process_hud_clear( );

		const auto local = systems::g_local.get( );
		if ( !local.is_alive || systems::g_local.is_in_cinematic( ) || !local.pawn || !local.controller )
		{
			return;
		}

		const auto local_ctrl = local.controller;
		const auto local_pawn = local.pawn;
		const auto local_team = memory::read<int>( local_pawn + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) );
		const auto& active_skins = settings::g_changer.skins.for_team( local_team );

		const auto hud_model = this->find_hud_model_weapon( local_pawn );
		const auto rules = memory::safe_read<std::uintptr_t>( addresses::globals::game_rules ).value_or( 0 );
		const auto round_offset = SCHEMA( "C_CSGameRules", "m_fRoundStartTime"_hash );
		const auto round_time = rules && round_offset ? memory::safe_read<float>( rules + round_offset ).value_or( 0.0f ) : 0.0f;
		if ( hud_model != this->m_last_hud_model || round_time != this->m_last_round_start_time )
		{
			this->m_applied_weapons.clear( );
			this->m_last_active_handle = 0;
			this->m_last_hud_model = hud_model;
			this->m_last_round_start_time = round_time;
		}
		std::erase_if( this->m_applied_weapons, []( const auto& entry ) {
			if ( !systems::g_entities.exists( entry.second.weapon ) ) return true;
			const auto owner = memory::safe_read<std::uint32_t>( entry.second.weapon + SCHEMA( "C_BaseEntity", "m_hOwnerEntity"_hash ) ).value_or( 0 );
			return !systems::g_entities.lookup( owner );
		} );

		if ( true )
		{
			const auto weapon_services = memory::read<std::uintptr_t>( local_pawn + SCHEMA( "C_BasePlayerPawn", "m_pWeaponServices"_hash ) );
			if ( weapon_services )
			{
				const auto weapons_base = weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hMyWeapons"_hash );
				const auto weapons_size = memory::read<int>( weapons_base );
				const auto weapons_data = memory::read<std::uintptr_t>( weapons_base + 0x8 );

				if ( weapons_data && weapons_size > 0 )
				{
					auto steam_id = steam::user::get_steam_id( );
					if ( !steam_id && local_ctrl )
					{
						steam_id = memory::read<std::uint64_t>( local_ctrl + SCHEMA( "CBasePlayerController", "m_steamID"_hash ) );
					}
					const auto account_id = static_cast< std::uint32_t >( steam_id & 0xffffffff );
					const auto active_handle = memory::read<std::uint32_t>( weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hActiveWeapon"_hash ) );
					const auto active_weapon = systems::g_entities.lookup( active_handle );

					if ( this->m_tracked_pawn != local_pawn )
					{
						this->m_applied_weapons.clear( );
						this->m_last_active_handle = 0;
						this->m_tracked_pawn = local_pawn;
					}

					for ( auto i = 0; i < weapons_size; ++i )
					{
						const auto handle = memory::safe_read<std::uint32_t>( weapons_data + i * sizeof( std::uint32_t ) ).value_or( 0 );
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
						const auto current_def_index = memory::safe_read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) ).value_or( 0 );
						const auto current_def = g_econ_item_system.find_def( static_cast< std::int16_t >( current_def_index ) );

						if ( !current_def || current_def->category != econ_item_system::item_category::gun )
						{
							continue;
						}

						const auto skin_it = active_skins.find( current_def_index );
						if ( skin_it == active_skins.end( ) )
						{
							continue;
						}

						const auto skin = cosmetic_attributes::normalize(skin_it->second);
						const auto applied_it = this->m_applied_weapons.find( handle );

						const auto current_pk = memory::safe_read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) ).value_or( -1 );
						const auto current_id_high = memory::safe_read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ) ).value_or( 0 );
						const auto current_seed = memory::safe_read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ) ).value_or( -1 );

						if ( applied_it != this->m_applied_weapons.end( ) 
							&& applied_it->second.weapon == weapon 
							&& applied_it->second.skin == skin 
							&& current_pk == skin.paint_kit_id 
							&& current_id_high == 0xf0000000 
							&& current_seed == skin.seed
							&& memory::read<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ) ) == skin.wear
							&& memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ) ) == ( skin.stattrak ? skin.stattrak_count : -1 )
							&& memory::read<int>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ) ) == ( skin.stattrak ? 9 : 0 )
							&& memory::read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ) ) == account_id
							&& name_tag::matches( iv, skin.name_tag ) )
						{
							continue;
						}

						const auto subclass_ptr = memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) + 0x8 ).value_or( 0 );
						if ( !subclass_ptr )
						{
							continue;
						}

						if (this->apply( weapon, iv, handle, active_handle, local_pawn, &skin, account_id ))
							this->m_applied_weapons[ handle ] = { weapon, skin };
					}

					if ( active_handle != this->m_last_active_handle )
					{
						this->m_last_active_handle = active_handle;

						if ( active_weapon )
						{
							const auto iv = active_weapon + SCHEMA( "C_EconEntity", "m_AttributeManager"_hash ) + SCHEMA( "C_AttributeContainer", "m_Item"_hash );
							const auto def_index = memory::safe_read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) ).value_or( 0 );
							const auto def = g_econ_item_system.find_def( static_cast< std::int16_t >( def_index ) );

							if ( def && def->category == econ_item_system::item_category::gun )
							{
								const auto skin_it = active_skins.find( def_index );
								if ( skin_it != active_skins.end( ) )
								{
									const auto skin = cosmetic_attributes::normalize( skin_it->second );
									if ( this->apply( active_weapon, iv, active_handle, active_handle, local_pawn, &skin, account_id ) )
										this->m_applied_weapons[ active_handle ] = { active_weapon, skin };
								}
								else
								{
									const auto paint_kit_id = memory::safe_read<int>( active_weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) ).value_or( 0 );
									const auto pk = g_econ_item_system.find_paint_kit( paint_kit_id );
									this->update_view_model( local_pawn, pk );
								}
							}
						}
					}
				}
			}
		}

		// Apply synced skins for remote players
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

			const auto remote_skin_data = g_skin_sync.get_remote_skin( sid );
			if ( !remote_skin_data || remote_skin_data->skins.empty( ) )
			{
				continue;
			}

			const auto pawn_handle = memory::safe_read<std::uint32_t>( ctrl + SCHEMA( "CBasePlayerController", "m_hPawn"_hash ) ).value_or( 0 );
			if ( !pawn_handle )
			{
				continue;
			}

			const auto pawn = systems::g_entities.lookup( pawn_handle );
			if ( !pawn || pawn < 0x10000 )
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
			if ( !remote_weapons_data || remote_weapons_size <= 0 )
			{
				continue;
			}

			const auto remote_active_handle = memory::safe_read<std::uint32_t>( remote_weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hActiveWeapon"_hash ) ).value_or( 0 );
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
				const auto current_def_index = memory::safe_read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) ).value_or( 0 );
				const auto current_def = g_econ_item_system.find_def( static_cast< std::int16_t >( current_def_index ) );
				if ( !current_def || current_def->category != econ_item_system::item_category::gun )
				{
					continue;
				}

				const auto skin_it = remote_skin_data->skins.find( current_def_index );
				if ( skin_it == remote_skin_data->skins.end( ) )
				{
					continue;
				}

				const auto skin = cosmetic_attributes::normalize( skin_it->second );
				const auto applied_it = this->m_applied_weapons.find( handle );

				const auto current_pk = memory::safe_read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) ).value_or( -1 );
				const auto current_id_high = memory::safe_read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ) ).value_or( 0 );
				const auto current_seed = memory::safe_read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ) ).value_or( -1 );

				if ( applied_it != this->m_applied_weapons.end( ) 
					&& applied_it->second.weapon == weapon 
					&& applied_it->second.skin == skin 
					&& current_pk == skin.paint_kit_id 
					&& current_id_high == 0xf0000000 
					&& current_seed == skin.seed
					&& memory::read<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ) ) == skin.wear
					&& memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ) ) == ( skin.stattrak ? skin.stattrak_count : -1 )
					&& memory::read<int>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ) ) == ( skin.stattrak ? 9 : 0 )
					&& memory::read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ) ) == remote_account_id
					&& name_tag::matches( iv, skin.name_tag ) )
				{
					continue;
				}

				const auto subclass_ptr = memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) + 0x8 ).value_or( 0 );
				if ( !subclass_ptr )
				{
					continue;
				}

				if ( this->apply( weapon, iv, handle, remote_active_handle, pawn, &skin, remote_account_id ) )
				{
					this->m_applied_weapons[ handle ] = { weapon, skin };
				}
			}
		}
	}

	bool guns::apply( std::uintptr_t weapon, std::uintptr_t iv, std::uint32_t handle, std::uint32_t active_handle, std::uintptr_t pawn, const settings::changer::applied_skin* skin, std::uint32_t account_id )
	{
		this->m_pending_hud_iv = 0;
		const auto soc_offset = SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash );
		if ( !skin || !soc_offset || !cosmetic_attributes::available( ) ) return false;
		if ( !PATTERN( patterns::weapon_update_skin ) || !PATTERN( patterns::weapon_update_composite_material ) ) return false;
		if ( pawn == systems::g_local.get( ).pawn && handle == active_handle && !this->find_hud_model_weapon( pawn ) )
			return false; // Retry rather than cache success before the HUD model exists.
		if ( !name_tag::apply( iv, skin->name_tag ) ) return false;
		memory::write<bool>( iv + soc_offset, true );

		memory::write<std::uint64_t>( iv + SCHEMA( "C_EconItemView", "m_iItemID"_hash ), 0xf000000000000010ull );
		memory::write<int>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ), skin->stattrak ? 9 : 0 );

		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ), 0xf0000000 );
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ), 0x10 );
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ), account_id );
		memory::write<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), true );

		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ), skin->paint_kit_id );
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ), skin->seed );
		memory::write<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ), skin->wear );
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ), skin->stattrak ? skin->stattrak_count : -1 );

		// Replace paint/seed/wear attributes too, not only the fallback fields.
		if ( !cosmetic_attributes::apply( iv, *skin ) ) return false;

		const auto pk = g_econ_item_system.find_paint_kit( skin->paint_kit_id );

		this->rebuild_paint( weapon, handle, active_handle, pawn, pk );
		this->schedule_hud_clear( iv );
		return true;
	}

	void guns::rebuild_paint( std::uintptr_t weapon, std::uint32_t handle, std::uint32_t active_handle, std::uintptr_t pawn, const econ_item_system::paint_kit* pk )
	{
		if ( !weapon || weapon < 0x10000 )
		{
			return;
		}

		const auto subclass_ptr = memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) + 0x8 ).value_or( 0 );
		if ( !subclass_ptr )
		{
			return;
		}

		const auto is_legacy = pk && pk->legacy_model;
		const auto mesh_group = is_legacy ? std::uint64_t{ 2 } : std::uint64_t{ 1 };

		if ( handle == active_handle )
		{
			this->update_view_model( pawn, pk );
		}

		const auto weapon_scene_node = memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
		if ( weapon_scene_node && PATTERN( patterns::weapon_set_mesh_group_mask ) )
		{
			memory::call<void>( PATTERN( patterns::weapon_set_mesh_group_mask ), weapon_scene_node, mesh_group );
		}

		if ( PATTERN( patterns::weapon_update_composite_material ) )
		{
			memory::call<void>( PATTERN( patterns::weapon_update_composite_material ), weapon + 0x608, true );
		}

		memory::call_vfunc<void>( weapon, 10, 1 );

		if ( PATTERN( patterns::weapon_update_skin ) )
		{
			memory::call<void>( PATTERN( patterns::weapon_update_skin ), weapon, true );
		}


	}

	void guns::update_view_model( std::uintptr_t pawn, const econ_item_system::paint_kit* pk )
	{
		const auto view_model = this->find_hud_model_weapon( pawn );
		if ( !view_model )
		{
			return;
		}

		const auto view_model_scene_node = memory::safe_read<std::uintptr_t>( view_model + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
		if ( !view_model_scene_node )
		{
			return;
		}

		const auto is_legacy = pk && pk->legacy_model;
		memory::call<void>( PATTERN( patterns::weapon_set_mesh_group_mask ), view_model_scene_node, is_legacy ? std::uint64_t{ 2 } : std::uint64_t{ 1 } );
	}

	std::uintptr_t guns::find_hud_model_weapon( std::uintptr_t pawn )
	{
		const auto arms_handle = memory::safe_read<std::uint32_t>( pawn + SCHEMA( "C_CSPlayerPawn", "m_hHudModelArms"_hash ) ).value_or( 0 );
		if ( !arms_handle )
		{
			return 0;
		}

		const auto arms = systems::g_entities.lookup( arms_handle );
		if ( !arms )
		{
			return 0;
		}

		const auto arms_scene_node = memory::safe_read<std::uintptr_t>( arms + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
		if ( !arms_scene_node )
		{
			return 0;
		}

		auto child = memory::safe_read<std::uintptr_t>( arms_scene_node + SCHEMA( "CGameSceneNode", "m_pChild"_hash ) ).value_or( 0 );

		while ( child && child > 0x10000 )
		{
			const auto owner = memory::safe_read<std::uintptr_t>( child + SCHEMA( "CGameSceneNode", "m_pOwner"_hash ) ).value_or( 0 );
			if ( owner && owner > 0x10000 )
			{
				const auto name = systems::g_entities.get_schema_name( owner );
				if ( name && fnv1a::runtime_hash( name ) == "C_CS2HudModelWeapon"_hash )
				{
					return owner;
				}
			}

			child = memory::safe_read<std::uintptr_t>( child + SCHEMA( "CGameSceneNode", "m_pNextSibling"_hash ) ).value_or( 0 );
		}

		return 0;
	}

	void guns::clear_hud_icon( std::uintptr_t iv )
	{
		const auto invalidate = PATTERN( patterns::econ_item_view_invalidate_description );
		if ( iv && invalidate )
		{
			memory::call<void>( invalidate, iv );
		}
	}

	void guns::schedule_hud_clear( std::uintptr_t iv )
	{
		this->clear_hud_icon( iv );
		this->m_pending_hud_iv = 0;
	}

	void guns::process_hud_clear( )
	{
		this->m_pending_hud_iv = 0;
	}

	void guns::reset( )
	{
		this->m_applied_weapons.clear( );
		this->m_last_active_handle = 0;
	}

} // namespace features::changer
