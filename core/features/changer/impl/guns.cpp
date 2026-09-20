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
#include <type_traits>
#include <core/features/changer/skin_sync.hpp>
#include <unordered_set>
namespace features::changer {

	namespace {
		constexpr std::uint64_t gun_faux_item_id = 0xf000000000000010ull;

		cosmetic_cache::identity visual_identity( std::uintptr_t weapon )
		{
			cosmetic_cache::identity result{};
			result.weapon = weapon;
			if ( !weapon ) return result;
			result.scene = memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
			result.owner = memory::safe_read<std::uint32_t>( weapon + SCHEMA( "C_BaseEntity", "m_hOwnerEntity"_hash ) ).value_or( 0 );
			if ( result.scene )
				result.model = memory::safe_read<std::uintptr_t>( result.scene + SCHEMA( "CSkeletonInstance", "m_modelState"_hash ) + SCHEMA( "CModelState", "m_hModel"_hash ) ).value_or( 0 );
			return result;
		}
	}

	std::optional<guns::skin_selection> guns::select_skin( std::uintptr_t weapon, std::uintptr_t iv,
		std::uint32_t handle, std::uint16_t definition, std::uint32_t holder_account,
		const settings::changer::skin_map_field::map_type& skins )
	{
		const auto saved = this->m_original_weapons.find( handle );
		if ( saved != this->m_original_weapons.end( ) )
		{
			const auto item = memory::safe_read<std::uint64_t>( iv + SCHEMA( "C_EconItemView", "m_iItemID"_hash ) );
			const auto& original = saved->second;
			const bool same_item = original.weapon == weapon && original.def_index == definition &&
				systems::g_entities.lookup( handle ) == weapon && item &&
				( *item == gun_faux_item_id || *item == original.item_id );
			if ( !same_item )
			{
				this->m_original_weapons.erase( saved );
				this->m_applied_weapons.erase( handle );
			}
			else if ( original.cosmetic && original.cosmetic->account_id != holder_account )
			{
				// A pickup does not turn this physical item into the holder's loadout.
				// Keep the last successfully applied source until the entity expires
				// or returns to that source account. Do not consult the HTTP echo.
				return original.cosmetic;
			}
		}
		const auto selected = skins.find( definition );
		if ( selected == skins.end( ) ) return std::nullopt;
		return skin_selection{ cosmetic_attributes::normalize( selected->second ), holder_account };
	}

	void guns::on_frame_stage_notify( )
	{
		this->process_hud_clear( );
		if ( this->m_invalidate_pending.exchange( false ) )
		{
			this->m_applied_weapons.clear( );
			this->m_last_active_handle = 0;
		}
		// Keep originals across HUD/round/pawn changes, but never across entity generations.
		std::erase_if( this->m_original_weapons, []( const auto& entry ) {
			return systems::g_entities.lookup( entry.first ) != entry.second.weapon;
		} );

		const auto local = systems::g_local.get( );
		if ( !local.controller ) return;

		const auto local_ctrl = local.controller;
		const auto local_pawn = preview_scene::player_pawn( local_ctrl );
		const auto local_team = local_pawn ? preview_scene::team( local_pawn ) : 0;
		const auto& active_skins = settings::g_changer.skins.for_team( local_team );

        const bool has_local_work = !active_skins.empty() || !this->m_original_weapons.empty();
        const auto hud_model = has_local_work ? this->find_hud_model_weapon(local_pawn) : 0;
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
			const auto weapon = systems::g_entities.lookup( entry.first );
			return weapon != entry.second.visual.weapon ||
				!cosmetic_cache::reusable( entry.second.visual, visual_identity( weapon ) );
		} );

		if ( has_local_work && preview_scene::player_ready( local_pawn ) )
		{
			const auto weapon_services = memory::read<std::uintptr_t>( local_pawn + SCHEMA( "C_BasePlayerPawn", "m_pWeaponServices"_hash ) );
			if ( weapon_services )
			{
				const auto weapons_base = weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hMyWeapons"_hash );
				const auto weapons_size = memory::read<int>( weapons_base );
				const auto weapons_data = memory::read<std::uintptr_t>( weapons_base + 0x8 );

				if ( weapons_data && weapons_size > 0 && weapons_size <= 64 )
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

						const auto selected = this->select_skin( weapon, iv, handle, current_def_index, account_id, active_skins );
						if ( !selected )
						{
							this->restore( weapon, iv, handle, active_handle, local_pawn );
							continue;
						}

						const auto& skin = selected->skin;
						const auto skin_account = selected->account_id;
						const auto hud_visual = handle == active_handle ? visual_identity( hud_model ) : cosmetic_cache::identity{};
						const auto applied_it = this->m_applied_weapons.find( handle );

						const auto current_pk = memory::safe_read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) ).value_or( -1 );
						const auto current_id_high = memory::safe_read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ) ).value_or( 0 );
						const auto current_seed = memory::safe_read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ) ).value_or( -1 );

						if ( applied_it != this->m_applied_weapons.end( ) 
							&& applied_it->second.visual.weapon == weapon 
							&& applied_it->second.skin == skin
							&& applied_it->second.hud == hud_visual
							&& current_pk == skin.paint_kit_id 
							&& current_id_high == 0xf0000000 
							&& current_seed == skin.seed
							&& memory::read<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ) ) == skin.wear
							&& memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ) ) == ( skin.stattrak ? skin.stattrak_count : -1 )
							&& memory::read<int>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ) ) == ( skin.stattrak ? 9 : 0 )
							&& memory::read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ) ) == skin_account
							&& memory::safe_read<bool>( iv + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ) ).value_or( false )
							&& memory::safe_read<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ) ).value_or( false )
							&& name_tag::matches( iv, skin.name_tag )
							&& cosmetic_attributes::matches( iv, skin ) )
						{
							continue;
						}

						const auto subclass_ptr = memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) + 0x8 ).value_or( 0 );
						if ( !subclass_ptr )
						{
							continue;
						}

						if (this->apply( weapon, iv, handle, active_handle, local_pawn, &skin, skin_account ))
							this->m_applied_weapons[ handle ] = { visual_identity( weapon ), skin, hud_visual };
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
								const auto selected = this->select_skin( active_weapon, iv, active_handle, def_index, account_id, active_skins );
								if ( selected )
								{
									// The inventory pass already applied/retried this weapon.
									// Do not rebuild its materials twice on a weapon switch.
									this->update_view_model( local_pawn, g_econ_item_system.find_paint_kit( selected->skin.paint_kit_id ) );
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
			const settings::changer::skin_map_field::map_type empty_skins{};
			const auto& remote_skins = remote_skin_data ? remote_skin_data->skins : empty_skins;
			// An empty profile (or pickup by a non-sync user) must restore our previous override.
            // Skip weapon traversal only when no original state needs restoring.
            if (remote_skins.empty() && this->m_original_weapons.empty()) continue;

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

			const auto remote_active_handle = memory::safe_read<std::uint32_t>( remote_weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hActiveWeapon"_hash ) ).value_or( 0 );
			const auto remote_account_id = static_cast< std::uint32_t >( sid & 0xffffffff );
			const auto remote_hud = this->find_hud_model_weapon( pawn );

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

				const auto selected = this->select_skin( weapon, iv, handle, current_def_index, remote_account_id, remote_skins );
				if ( !selected )
				{
					this->restore( weapon, iv, handle, remote_active_handle, pawn );
					continue;
				}

				const auto& skin = selected->skin;
				const auto skin_account = selected->account_id;
				const auto hud_visual = handle == remote_active_handle ? visual_identity( remote_hud ) : cosmetic_cache::identity{};
				const auto applied_it = this->m_applied_weapons.find( handle );

				const auto current_pk = memory::safe_read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) ).value_or( -1 );
				const auto current_id_high = memory::safe_read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ) ).value_or( 0 );
				const auto current_seed = memory::safe_read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ) ).value_or( -1 );

				if ( applied_it != this->m_applied_weapons.end( ) 
					&& applied_it->second.visual.weapon == weapon 
					&& applied_it->second.skin == skin
					&& applied_it->second.hud == hud_visual
					&& current_pk == skin.paint_kit_id 
					&& current_id_high == 0xf0000000 
					&& current_seed == skin.seed
					&& memory::read<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ) ) == skin.wear
					&& memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ) ) == ( skin.stattrak ? skin.stattrak_count : -1 )
					&& memory::read<int>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ) ) == ( skin.stattrak ? 9 : 0 )
					&& memory::read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ) ) == skin_account
					&& memory::safe_read<bool>( iv + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ) ).value_or( false )
					&& memory::safe_read<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ) ).value_or( false )
					&& name_tag::matches( iv, skin.name_tag )
					&& cosmetic_attributes::matches( iv, skin ) )
				{
					continue;
				}

				const auto subclass_ptr = memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) + 0x8 ).value_or( 0 );
				if ( !subclass_ptr )
				{
					continue;
				}

				if ( this->apply( weapon, iv, handle, remote_active_handle, pawn, &skin, skin_account ) )
				{
					this->m_applied_weapons[ handle ] = { visual_identity( weapon ), skin, hud_visual };
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
		if ( !memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 ) ) return false;
		if ( pawn == systems::g_local.get( ).pawn && handle == active_handle && !this->find_hud_model_weapon( pawn ) )
			return false; // Retry rather than cache success before the HUD model exists.
		if ( !this->capture_original( weapon, iv, handle ) ) return false;
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
		const auto original = this->m_original_weapons.find( handle );
		if ( original == this->m_original_weapons.end( ) ) return false;
		original->second.cosmetic = skin_selection{ *skin, account_id };
		return true;
	}

	bool guns::capture_original( std::uintptr_t weapon, std::uintptr_t iv, std::uint32_t handle )
	{
		const auto definition_offset = SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash );
		const auto id_offset = SCHEMA( "C_EconItemView", "m_iItemID"_hash );
		if ( !definition_offset || !id_offset || systems::g_entities.lookup( handle ) != weapon ) return false;
		const auto definition = memory::safe_read<std::uint16_t>( iv + definition_offset );
		const auto item_id = memory::safe_read<std::uint64_t>( iv + id_offset );
		if ( !definition || !item_id ) return false;
		if ( const auto it = this->m_original_weapons.find( handle ); it != this->m_original_weapons.end( ) )
		{
			if ( it->second.weapon == weapon && it->second.def_index == *definition &&
				( *item_id == gun_faux_item_id || *item_id == it->second.item_id ) ) return true;
			this->m_original_weapons.erase( it );
		}
		// Never mistake an override from a lost snapshot for a native item.
		if ( *item_id == gun_faux_item_id ) return false;
		original_weapon saved{};
		saved.weapon = weapon;
		saved.def_index = *definition;
		saved.item_id = *item_id;
		const auto read = []( auto& destination, std::uintptr_t base, int offset ) {
			if ( !offset ) return false;
			const auto value = memory::safe_read<std::remove_reference_t<decltype(destination)>>( base + offset );
			if ( !value ) return false;
			destination = *value;
			return true;
		};
		if ( !read( saved.id_high, iv, SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ) ) ||
			 !read( saved.id_low, iv, SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ) ) ||
			 !read( saved.account_id, iv, SCHEMA( "C_EconItemView", "m_iAccountID"_hash ) ) ||
			 !read( saved.quality, iv, SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ) ) ||
			 !read( saved.initialized, iv, SCHEMA( "C_EconItemView", "m_bInitialized"_hash ) ) ||
			 !read( saved.disallow_soc, iv, SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ) ) ||
			 !read( saved.paint_kit, weapon, SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) ) ||
			 !read( saved.seed, weapon, SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ) ) ||
			 !read( saved.wear, weapon, SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ) ) ||
			 !read( saved.stattrak, weapon, SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ) ) ||
			 !cosmetic_attributes::capture( iv, saved.attributes ) ) return false;
		saved.custom_name = name_tag::capture( iv );
		if ( name_tag::offsets( ) && !saved.custom_name ) return false;
		this->m_original_weapons.emplace( handle, std::move( saved ) );
		return true;
	}

	bool guns::restore( std::uintptr_t weapon, std::uintptr_t iv, std::uint32_t handle, std::uint32_t active_handle, std::uintptr_t pawn )
	{
		const auto it = this->m_original_weapons.find( handle );
		if ( it == this->m_original_weapons.end( ) ) return true;
		const auto& saved = it->second;
		const auto definition = memory::safe_read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
		const auto item_id = memory::safe_read<std::uint64_t>( iv + SCHEMA( "C_EconItemView", "m_iItemID"_hash ) );
		if ( !definition || !item_id ) return false;
		if ( saved.weapon != weapon || systems::g_entities.lookup( handle ) != weapon || saved.def_index != *definition ||
			( *item_id != gun_faux_item_id && *item_id != saved.item_id ) )
		{
			// A different server item/entity owns this slot now. Never write the old snapshot into it.
			this->m_original_weapons.erase( it );
			this->m_applied_weapons.erase( handle );
			return true;
		}
		if ( !visual_identity( weapon ).ready( ) ||
			 !memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) + 0x8 ).value_or( 0 ) ||
			 !PATTERN( patterns::weapon_update_skin ) || !PATTERN( patterns::weapon_update_composite_material ) ) return false;
		if ( pawn == systems::g_local.get( ).pawn && handle == active_handle && !this->find_hud_model_weapon( pawn ) ) return false;
		if ( !cosmetic_attributes::restore( iv, saved.attributes ) ) return false;
		if ( saved.custom_name && !name_tag::restore( iv, *saved.custom_name ) ) return false;
		const auto write = []( std::uintptr_t base, int offset, const auto& value ) {
			return offset && memory::safe_write( base + offset, value );
		};
		if ( !write( iv, SCHEMA( "C_EconItemView", "m_iAccountID"_hash ), saved.account_id ) ||
			 !write( iv, SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ), saved.quality ) ||
			 !write( iv, SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), saved.initialized ) ||
			 !write( iv, SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ), saved.disallow_soc ) ||
			 !write( weapon, SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ), saved.paint_kit ) ||
			 !write( weapon, SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ), saved.seed ) ||
			 !write( weapon, SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ), saved.wear ) ||
			 !write( weapon, SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ), saved.stattrak ) ||
			 !write( iv, SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ), saved.id_high ) ||
			 !write( iv, SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ), saved.id_low ) ||
			 !write( iv, SCHEMA( "C_EconItemView", "m_iItemID"_hash ), saved.item_id ) ) return false;
		this->rebuild_paint( weapon, handle, active_handle, pawn, g_econ_item_system.find_paint_kit( saved.paint_kit ) );
		this->schedule_hud_clear( iv );
		this->m_applied_weapons.erase( handle );
		this->m_original_weapons.erase( it );
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
		return hud_weapon::find( pawn );
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

	void guns::on_lobby( )
	{
		if ( !preview_item::available( ) ) return;
		const auto manager = SCHEMA( "C_EconEntity", "m_AttributeManager"_hash );
		const auto item = SCHEMA( "C_AttributeContainer", "m_Item"_hash );
		const auto definition = SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash );
		if ( !manager || !item || !definition ) return;
		for ( const auto& preview : preview_scene::players )
		{
			if ( !preview.steam_id || ( preview.team != 2 && preview.team != 3 ) ) continue;
			const auto remote = preview_scene::is_local( preview ) ? std::optional<remote_player_skin>{}
				: g_skin_sync.get_remote_skin( preview.steam_id );
			if ( !preview_scene::is_local( preview ) && !remote ) continue;
			const auto& skins = remote ? remote->skins : settings::g_changer.skins.for_team( preview.team );
			for ( const auto weapon : preview.weapons )
			{
				const auto iv = weapon + manager + item;
				const auto current_index = memory::safe_read<std::uint16_t>( iv + definition ).value_or( 0 );
				const auto current = g_econ_item_system.find_def( static_cast<std::int16_t>( current_index ) );
				if ( !current || current->category != econ_item_system::item_category::gun ) continue;
				const auto it = skins.find( current_index );
				if ( it == skins.end( ) ) continue;
				const auto skin = cosmetic_attributes::normalize( it->second );
				const auto account = static_cast<std::uint32_t>( preview.steam_id );
				const auto quality = skin.stattrak ? 9 : 0;

				if ( preview_item::matches( weapon, iv, skin, account, quality ) ) continue;
				if ( !preview_item::write( weapon, iv, skin, account, quality ) ) continue;
				const auto pk = g_econ_item_system.find_paint_kit( skin.paint_kit_id );
				const auto node = preview_item::identity( weapon ).scene;
				memory::call<void>( PATTERN( patterns::weapon_set_mesh_group_mask ), node, pk && pk->legacy_model ? std::uint64_t{2} : std::uint64_t{1} );
				memory::call<void>( PATTERN( patterns::weapon_update_composite_material ), weapon + 0x608, true );
				memory::call_vfunc<void>( weapon, 10, 1 );
				memory::call<void>( PATTERN( patterns::weapon_update_skin ), weapon, true );
				preview_item::remember( weapon, skin );
			}
		}
	}

	void guns::reset( )
	{
		this->m_applied_weapons.clear( );
		this->m_original_weapons.clear( );
		this->m_invalidate_pending = false;
		this->m_last_active_handle = 0;
		this->m_tracked_pawn = 0;
		this->m_last_hud_model = 0;
		this->m_last_round_start_time = 0;
	}

} // namespace features::changer
