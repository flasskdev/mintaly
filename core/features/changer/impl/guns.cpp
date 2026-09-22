#include <pch/pch.hpp>
#include "../preview_scene.hpp"
#include "../preview_item.hpp"
#include "../hud_weapon.hpp"
#include <utilities/cosmetic_model.hpp>
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
#include <unordered_map>
#include <string_view>
#include <chrono>
#include <utilities/diag.hpp>
namespace features::changer {

	namespace {
		constexpr std::uint64_t gun_faux_item_id = 0xf000000000000010ull;

		struct schema_offsets {
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
			int subclass_id{};
			int scene_node{};
			int model_state{};
			int model_handle{};
			int owner_entity{};
			int weapon_services{};
			int my_weapons{};
			int active_weapon{};
			int game_rules{};
			int round_start_time{};
			int attribute_manager{};
			int item{};
			int original_owner_low{};
			int original_owner_high{};
			int steam_id{};
			int name_tag{};
			int custom_name{};
			int m_flFallbackWear{};
			int m_nFallbackStatTrak{};
			int m_iEntityQuality{};
			int m_iAccountID{};
			int m_bInitialized{};
			int m_bDisallowSOC{};
			int m_iItemIDHigh{};
			int m_iItemIDLow{};
			int m_iItemID{};
			int m_iItemDefinitionIndex{};
			int m_nSubclassID{};
			int m_pGameSceneNode{};
			int m_modelState{};
			int m_hModel{};
			int m_hOwnerEntity{};
			int m_pWeaponServices{};
			int m_hMyWeapons{};
			int m_hActiveWeapon{};
			int m_fRoundStartTime{};
			int C_EconEntity{};
			int C_AttributeContainer{};
			int C_EconItemView{};
			int C_BaseEntity{};
			int CSkeletonInstance{};
			int CModelState{};
			int CBasePlayerPawn{};
			int CPlayer_WeaponServices{};
			int CBasePlayerController{};
			int C_CSGameRules{};

			void init() {
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
				subclass_id = SCHEMA("C_BaseEntity", "m_nSubclassID"_hash);
				scene_node = SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash);
				model_state = SCHEMA("CSkeletonInstance", "m_modelState"_hash);
				model_handle = SCHEMA("CModelState", "m_hModel"_hash);
				owner_entity = SCHEMA("C_BaseEntity", "m_hOwnerEntity"_hash);
				weapon_services = SCHEMA("C_BasePlayerPawn", "m_pWeaponServices"_hash);
				my_weapons = SCHEMA("CPlayer_WeaponServices", "m_hMyWeapons"_hash);
				active_weapon = SCHEMA("CPlayer_WeaponServices", "m_hActiveWeapon"_hash);
				game_rules = SCHEMA("C_CSGameRules", "m_fRoundStartTime"_hash);
				round_start_time = SCHEMA("C_CSGameRules", "m_fRoundStartTime"_hash);
				attribute_manager = SCHEMA("C_EconEntity", "m_AttributeManager"_hash);
				item = SCHEMA("C_AttributeContainer", "m_Item"_hash);
				original_owner_low = SCHEMA("C_EconEntity", "m_OriginalOwnerXuidLow"_hash);
				original_owner_high = SCHEMA("C_EconEntity", "m_OriginalOwnerXuidHigh"_hash);
				steam_id = SCHEMA("CBasePlayerController", "m_steamID"_hash);
				name_tag = SCHEMA("C_EconItemView", "m_iCustomName"_hash);
				custom_name = SCHEMA("C_EconItemView", "m_iCustomName"_hash);
				m_flFallbackWear = SCHEMA("C_EconEntity", "m_flFallbackWear"_hash);
				m_nFallbackStatTrak = SCHEMA("C_EconEntity", "m_nFallbackStatTrak"_hash);
				m_iEntityQuality = SCHEMA("C_EconItemView", "m_iEntityQuality"_hash);
				m_iAccountID = SCHEMA("C_EconItemView", "m_iAccountID"_hash);
				m_bInitialized = SCHEMA("C_EconItemView", "m_bInitialized"_hash);
				m_bDisallowSOC = SCHEMA("C_EconItemView", "m_bDisallowSOC"_hash);
				m_iItemIDHigh = SCHEMA("C_EconItemView", "m_iItemIDHigh"_hash);
				m_iItemIDLow = SCHEMA("C_EconItemView", "m_iItemIDLow"_hash);
				m_iItemID = SCHEMA("C_EconItemView", "m_iItemID"_hash);
				m_iItemDefinitionIndex = SCHEMA("C_EconItemView", "m_iItemDefinitionIndex"_hash);
				m_nSubclassID = SCHEMA("C_BaseEntity", "m_nSubclassID"_hash);
				m_pGameSceneNode = SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash);
				m_modelState = SCHEMA("CSkeletonInstance", "m_modelState"_hash);
				m_hModel = SCHEMA("CModelState", "m_hModel"_hash);
				m_hOwnerEntity = SCHEMA("C_BaseEntity", "m_hOwnerEntity"_hash);
				m_pWeaponServices = SCHEMA("C_BasePlayerPawn", "m_pWeaponServices"_hash);
				m_hMyWeapons = SCHEMA("CPlayer_WeaponServices", "m_hMyWeapons"_hash);
				m_hActiveWeapon = SCHEMA("CPlayer_WeaponServices", "m_hActiveWeapon"_hash);
				m_fRoundStartTime = SCHEMA("C_CSGameRules", "m_fRoundStartTime"_hash);
				C_EconEntity = SCHEMA("C_EconEntity", "m_AttributeManager"_hash);
				C_AttributeContainer = SCHEMA("C_AttributeContainer", "m_Item"_hash);
				C_EconItemView = SCHEMA("C_EconItemView", "m_iItemDefinitionIndex"_hash);
				C_BaseEntity = SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash);
				CSkeletonInstance = SCHEMA("CSkeletonInstance", "m_modelState"_hash);
				CModelState = SCHEMA("CModelState", "m_hModel"_hash);
				CBasePlayerPawn = SCHEMA("C_BasePlayerPawn", "m_pWeaponServices"_hash);
				CPlayer_WeaponServices = SCHEMA("CPlayer_WeaponServices", "m_hMyWeapons"_hash);
				CBasePlayerController = SCHEMA("CBasePlayerController", "m_steamID"_hash);
				C_CSGameRules = SCHEMA("C_CSGameRules", "m_fRoundStartTime"_hash);
			}
		};

		inline schema_offsets& get_offsets() {
			static schema_offsets offsets;
			static bool init = (offsets.init(), true);
			return offsets;
		}

		void report_skin_failure( std::string_view reason, std::uint32_t handle, int paint )
		{
			// Game-thread only; bounded by the fixed reason strings below, not by entities.
			static std::unordered_map<std::string_view, std::chrono::steady_clock::time_point> next_report;
			const auto now = std::chrono::steady_clock::now( );
			auto& next = next_report[reason];
			if ( now < next ) return;
			next = now + std::chrono::seconds( 5 );
			if ( reason == "hud-pending" )
			{
				const auto local = systems::g_local.get( );
				const auto pawn = local.observer_pawn ? local.observer_pawn : preview_scene::player_pawn( local.controller );
				const auto hud = hud_weapon::locate( pawn );
				diag::writef( diag::level::warning, "[skin-hud] lookup=%s candidates=%u found=%d",
					hud.reason, static_cast<unsigned>( hud.candidates ), hud.entity != 0 );
			}
			diag::writef( diag::level::warning,
				"[skin-apply] reason=%s handle=%u paint=%d hud_weapon_offset=%u attribute_list=%u attribute_vector=%u attribute_index=%u attribute_value=%u",
				reason.data( ), static_cast<unsigned>( handle ), paint,
				static_cast<unsigned>( hud_weapon::weapon_handle_offset( ) ),
				static_cast<unsigned>( SCHEMA( "C_EconItemView", "m_AttributeList"_hash ) ),
				static_cast<unsigned>( SCHEMA( "CAttributeList", "m_Attributes"_hash ) ),
				static_cast<unsigned>( SCHEMA( "CEconItemAttribute", "m_iAttributeDefinitionIndex"_hash ) ),
				static_cast<unsigned>( SCHEMA( "CEconItemAttribute", "m_flValue"_hash ) ) );
		}

		cosmetic_cache::identity visual_identity( std::uintptr_t weapon )
		{
			cosmetic_cache::identity result{};
			result.weapon = weapon;
			if ( !weapon ) return result;
			const auto& off = get_offsets();
			result.scene = memory::safe_read<std::uintptr_t>( weapon + off.scene_node ).value_or( 0 );
			result.owner = memory::safe_read<std::uint32_t>( weapon + off.owner_entity ).value_or( 0 );
			if ( result.scene )
				result.model = memory::safe_read<std::uintptr_t>( result.scene + off.model_state + off.model_handle ).value_or( 0 );
			return result;
		}
	}

	std::optional<guns::skin_selection> guns::select_skin( std::uintptr_t weapon, std::uintptr_t iv,
		std::uint32_t handle, std::uint16_t definition, std::uint32_t holder_account,
		const settings::changer::skin_map_field::map_type& skins )
	{
		if ( !entity_guard::ready( weapon ) ) return std::nullopt;
		const auto saved = this->m_original_weapons.find( handle );
		if ( saved != this->m_original_weapons.end( ) )
		{
			const auto item = memory::safe_read<std::uint64_t>( iv + SCHEMA( "C_EconItemView", "m_iItemID"_hash ) );
			const auto& original = saved->second;
			const bool same_item = original.weapon == weapon && original.def_index == definition &&
				systems::g_entities.lookup( handle ) == weapon && item &&
				( *item == gun_faux_item_id || *item == original.item_id || original.item_id == 0 || original.cosmetic.has_value() );
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
		// Network provenance survives pickups even when this client never saw the donor holding it.
		// Never use m_iAccountID here: apply() replaces that field locally.
		const auto owner_low = SCHEMA( "C_EconEntity", "m_OriginalOwnerXuidLow"_hash );
		const auto owner_high = SCHEMA( "C_EconEntity", "m_OriginalOwnerXuidHigh"_hash );
		const auto source_account = owner_low ? memory::safe_read<std::uint32_t>( weapon + owner_low ).value_or( 0 ) : 0;
		if ( source_account && source_account != holder_account )
		{
			const auto local = systems::g_local.get( );
			auto local_sid = steam::user::get_steam_id( );
			if ( !local_sid && local.controller )
				local_sid = memory::safe_read<std::uint64_t>( local.controller + SCHEMA( "CBasePlayerController", "m_steamID"_hash ) ).value_or( 0 );
			if ( source_account == static_cast<std::uint32_t>( local_sid ) )
			{
				const auto& own = settings::g_changer.skins.for_team( preview_scene::team( preview_scene::player_pawn( local.controller ) ) );
				const auto selected = own.find( definition );
				if ( selected == own.end( ) ) return std::nullopt;
				return skin_selection{ cosmetic_attributes::normalize( selected->second ), source_account };
			}
			if ( !g_skin_sync.is_enabled( ) ) return std::nullopt;
			const auto high = owner_high ? memory::safe_read<std::uint32_t>( weapon + owner_high ).value_or( 0 ) : 0;
			const auto source_sid = high ? ( static_cast<std::uint64_t>( high ) << 32 ) | source_account
				: 76561197960265728ull + source_account;
			const auto profile = g_skin_sync.get_remote_skin( source_sid );
			if ( !profile ) return std::nullopt;
			const auto selected = profile->skins.find( definition );
			if ( selected == profile->skins.end( ) ) return std::nullopt;
			return skin_selection{ cosmetic_attributes::normalize( selected->second ), source_account };
		}
		const auto selected = skins.find( definition );
		// The local caller already supplies for_team(), including its configured fallback.
		// A missing remote selection must never fall back to this client's loadout.
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

        const bool has_local_work = !active_skins.empty() || !this->m_original_weapons.empty() || g_skin_sync.is_enabled();
        const auto hud_model = has_local_work ? this->find_hud_model_weapon(local_pawn) : 0;
		const auto rules = memory::safe_read<std::uintptr_t>( addresses::globals::game_rules ).value_or( 0 );
		const auto round_offset = SCHEMA( "C_CSGameRules", "m_fRoundStartTime"_hash );
		const auto round_time = rules && round_offset ? memory::safe_read<float>( rules + round_offset ).value_or( 0.0f ) : 0.0f;
		if ( round_time != this->m_last_round_start_time )
		{
			this->m_applied_weapons.clear( );
			// Surviving entities keep their source skin across round boundaries.
			this->m_last_active_handle = 0;
			this->m_last_hud_model = 0;
			this->m_last_round_start_time = round_time;
		}
		const bool hud_changed = ( hud_model != this->m_last_hud_model );
		if ( hud_changed )
		{
			this->m_last_hud_model = hud_model;
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

						if ( !entity_guard::ready( weapon ) )
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

						if ( applied_it != this->m_applied_weapons.end( ) )
						{
							if ( applied_it->second.skin.paint_kit_id == skin.paint_kit_id
								&& applied_it->second.skin.seed == skin.seed
								&& applied_it->second.skin.wear == skin.wear
								&& applied_it->second.skin.stattrak == skin.stattrak
								&& applied_it->second.skin.name_tag == skin.name_tag
								&& applied_it->second.skin.stattrak_count != skin.stattrak_count )
							{
								applied_it->second.skin.stattrak_count = skin.stattrak_count;
								memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ), skin.stattrak ? skin.stattrak_count : -1 );
								if ( changer::cosmetic_attributes::available( ) )
								{
									const auto set = PATTERN( patterns::econ_item_view_set_attribute );
									const auto count_val = std::bit_cast<float>( static_cast<std::int32_t>( skin.stattrak_count ) );
									memory::safe_call<void>( set, iv, "kill eater", count_val );
								}
								if ( const auto orig = this->m_original_weapons.find( handle ); orig != this->m_original_weapons.end( ) && orig->second.cosmetic )
								{
									orig->second.cosmetic->skin.stattrak_count = skin.stattrak_count;
								}
							}
						}

						if ( applied_it != this->m_applied_weapons.end( ) 
							&& applied_it->second.visual.weapon == weapon 
							&& applied_it->second.skin == skin
							// HUD changes are handled separately at render start.
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
							// Render-start reconciles the HUD once, independently of world paint.
							continue;
						}

						const auto subclass_ptr = memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) + 0x8 ).value_or( 0 );
						if ( !subclass_ptr )
						{
							report_skin_failure( "subclass-unavailable", handle, skin.paint_kit_id );
							continue;
						}

						if (this->apply( weapon, iv, handle, active_handle, local_pawn, &skin, skin_account ))
							this->m_applied_weapons[ handle ] = { visual_identity( weapon ), skin, hud_visual };
					}

					if ( active_handle != this->m_last_active_handle || hud_changed )
					{
						const bool hud_ready = ( this->find_hud_model_weapon( local_pawn ) != 0 );
						if ( hud_ready )
						{
							this->m_last_active_handle = active_handle;
						}
						else
						{
							this->m_last_active_handle = 0;
						}

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
									this->update_view_model( local_pawn, g_econ_item_system.find_paint_kit( selected->skin.paint_kit_id ), true );
								}
								else
								{
									const auto paint_kit_id = memory::safe_read<int>( active_weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) ).value_or( 0 );
									const auto pk = g_econ_item_system.find_paint_kit( paint_kit_id );
									this->update_view_model( local_pawn, pk, true );
								}
							}
						}
					}
				}
			}
		}

		// Restore synced guns for remote players if sync was disabled
		if ( !g_skin_sync.is_enabled( ) )
		{
			auto steam_id = steam::user::get_steam_id( );
			if ( !steam_id && local_ctrl )
			{
				steam_id = memory::read<std::uint64_t>( local_ctrl + SCHEMA( "CBasePlayerController", "m_steamID"_hash ) );
			}
			const auto local_account_id = static_cast< std::uint32_t >( steam_id & 0xffffffff );

			std::vector<std::pair<std::uint32_t, std::uintptr_t>> remote_entries;
			for ( const auto& [handle, orig] : this->m_original_weapons )
			{
				if ( orig.cosmetic && orig.cosmetic->account_id != local_account_id )
				{
					remote_entries.emplace_back( handle, orig.weapon );
				}
			}
			for ( const auto& [handle, weapon_ptr] : remote_entries )
			{
				const auto weapon = systems::g_entities.lookup( handle );
				if ( weapon )
				{
					const auto iv = weapon + SCHEMA( "C_EconEntity", "m_AttributeManager"_hash ) + SCHEMA( "C_AttributeContainer", "m_Item"_hash );
					this->restore( weapon, iv, handle, 0, weapon );
				}
				else
				{
					this->m_original_weapons.erase( handle );
					this->m_applied_weapons.erase( handle );
				}
			}
		}

		// Apply synced skins for remote players (frame-throttled)
		if ( g_skin_sync.is_enabled( ) )
		{
			// Do not restart an unfinished batch at player zero on every frame.
			if ( this->m_remote_frame_counter == 0 && this->m_remote_player_index == 0 )
			{
				this->m_remote_controllers.clear();
				const auto all_players = systems::g_entities.get_by_type( systems::entities::type::player );
				for ( const auto& p : all_players )
				{
					const auto ctrl = p.ptr;
					if ( !ctrl || ctrl == local_ctrl ) continue;
					const auto sid = memory::safe_read<std::uint64_t>( ctrl + SCHEMA( "CBasePlayerController", "m_steamID"_hash ) ).value_or( 0 );
					constexpr std::uint64_t steam_id_base = 76561197960265728ull;
					if ( sid < steam_id_base && !g_skin_sync.m_bot_sync_test.load( ) ) continue;
					const auto pawn = preview_scene::player_pawn( ctrl );
					if ( !preview_scene::player_ready( pawn ) ) continue;
					this->m_remote_controllers.push_back( ctrl );
				}
				this->m_remote_player_index = 0;
			}

			// Process up to 2 remote players per frame
			constexpr std::size_t PLAYERS_PER_FRAME = 2;
			const auto& controllers = this->m_remote_controllers;
			const std::size_t start = this->m_remote_player_index;
			const std::size_t end = std::min(start + PLAYERS_PER_FRAME, controllers.size());

			for ( std::size_t idx = start; idx < end; ++idx )
			{
				const auto ctrl = controllers[idx];
				const auto sid = memory::safe_read<std::uint64_t>( ctrl + SCHEMA( "CBasePlayerController", "m_steamID"_hash ) ).value_or( 0 );
				const auto remote_skin_data = g_skin_sync.get_remote_skin( sid );
				const settings::changer::skin_map_field::map_type empty_skins{};
				const auto& remote_skins = remote_skin_data ? remote_skin_data->skins : empty_skins;

				const auto pawn = preview_scene::player_pawn( ctrl );
				if ( !preview_scene::player_ready( pawn ) ) continue;

				const auto remote_weapon_services = memory::safe_read<std::uintptr_t>( pawn + SCHEMA( "C_BasePlayerPawn", "m_pWeaponServices"_hash ) ).value_or( 0 );
				if ( !remote_weapon_services ) continue;

				const auto remote_weapons_base = remote_weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hMyWeapons"_hash );
				const auto remote_weapons_size = memory::safe_read<int>( remote_weapons_base ).value_or( 0 );
				const auto remote_weapons_data = memory::safe_read<std::uintptr_t>( remote_weapons_base + 0x8 ).value_or( 0 );
				if ( !remote_weapons_data || remote_weapons_size <= 0 || remote_weapons_size > 64 ) continue;

				const auto remote_active_handle = memory::safe_read<std::uint32_t>( remote_weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hActiveWeapon"_hash ) ).value_or( 0 );
				const auto remote_account_id = static_cast< std::uint32_t >( sid & 0xffffffff );
				const auto remote_hud = this->find_hud_model_weapon( pawn );

				for ( auto i = 0; i < remote_weapons_size; ++i )
				{
					const auto handle = memory::safe_read<std::uint32_t>( remote_weapons_data + i * sizeof( std::uint32_t ) ).value_or( 0 );
					if ( !handle ) continue;
					const auto weapon = systems::g_entities.lookup( handle );
					if ( !weapon || weapon < 0x10000 ) continue;
					const auto iv = weapon + SCHEMA( "C_EconEntity", "m_AttributeManager"_hash ) + SCHEMA( "C_AttributeContainer", "m_Item"_hash );
					const auto current_def_index = memory::safe_read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) ).value_or( 0 );
					const auto current_def = g_econ_item_system.find_def( static_cast< std::int16_t >( current_def_index ) );
					if ( !current_def || current_def->category != econ_item_system::item_category::gun ) continue;
					if ( !entity_guard::ready( weapon ) ) continue;

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

					if ( applied_it != this->m_applied_weapons.end( ) )
					{
						if ( applied_it->second.skin.paint_kit_id == skin.paint_kit_id
							&& applied_it->second.skin.seed == skin.seed
							&& applied_it->second.skin.wear == skin.wear
							&& applied_it->second.skin.stattrak == skin.stattrak
							&& applied_it->second.skin.name_tag == skin.name_tag
							&& applied_it->second.skin.stattrak_count != skin.stattrak_count )
						{
							applied_it->second.skin.stattrak_count = skin.stattrak_count;
							memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ), skin.stattrak ? skin.stattrak_count : -1 );
							if ( changer::cosmetic_attributes::available( ) )
							{
								const auto set = PATTERN( patterns::econ_item_view_set_attribute );
								const auto count_val = std::bit_cast<float>( static_cast<std::int32_t>( skin.stattrak_count ) );
								memory::safe_call<void>( set, iv, "kill eater", count_val );
							}
							if ( const auto orig = this->m_original_weapons.find( handle ); orig != this->m_original_weapons.end( ) && orig->second.cosmetic )
							{
								orig->second.cosmetic->skin.stattrak_count = skin.stattrak_count;
							}
						}
					}

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

			// Advance frame counter for next batch
			this->m_remote_player_index = end;
			if ( this->m_remote_player_index >= controllers.size() )
			{
				this->m_remote_player_index = 0;
				this->m_remote_frame_counter = (this->m_remote_frame_counter + 1) % 3; // Rebuild controller list every 3 frames
			}
		}
	}

	bool guns::apply( std::uintptr_t weapon, std::uintptr_t iv, std::uint32_t handle, std::uint32_t active_handle, std::uintptr_t pawn, const settings::changer::applied_skin* skin, std::uint32_t account_id )
	{
		this->m_pending_hud_iv = 0;
		const auto fail = [&]( const char* reason ) {
			report_skin_failure( reason, handle, skin ? skin->paint_kit_id : 0 );
			return false;
		};
		const auto soc_offset = SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash );
		if ( !skin || !soc_offset || !cosmetic_attributes::available( ) ) return fail( "attribute-dependencies" );
		if ( !PATTERN( patterns::weapon_update_skin ) || !PATTERN( patterns::weapon_update_composite_material ) ) return fail( "material-patterns" );
		if ( !memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 ) ) return fail( "weapon-scene" );
		// HUD readiness must not prevent item writes or rebuilding the real weapon.
		// A successful world apply queues a separate retry through hud_refresh_pending.
		if ( !this->capture_original( weapon, iv, handle ) ) return fail( "original-snapshot" );
		if ( !name_tag::apply( iv, skin->name_tag ) ) return fail( "name-attribute" );
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
		if ( !cosmetic_attributes::apply( iv, *skin ) ) return fail( "attribute-readback" );

		const auto pk = g_econ_item_system.find_paint_kit( skin->paint_kit_id );

		if (!this->rebuild_paint( weapon, handle, active_handle, pawn, pk )) return fail( "world-material-rebuild" );
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
				( *item_id == gun_faux_item_id || *item_id == it->second.item_id || it->second.item_id == 0 || it->second.cosmetic.has_value() ) ) return true;
			this->m_original_weapons.erase( it );
		}
		// If the weapon already has a changer override from a previous round/session, re-synthesize rather than failing.
		if ( *item_id == gun_faux_item_id )
		{
			original_weapon synth{};
			synth.weapon = weapon;
			synth.def_index = *definition;
			synth.item_id = 0;
			synth.paint_kit = 0;
			synth.seed = 0;
			synth.wear = 0.0f;
			synth.stattrak = -1;
			this->m_original_weapons.emplace( handle, std::move( synth ) );
			return true;
		}
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
		const auto entity = entity_guard::capture( weapon );
		if ( !entity || entity->handle != handle || !entity_guard::ready( pawn ) ) return false;
		// Engine callbacks can clear the cache. Do not keep references/iterators across them.
		const auto saved = it->second;
		const auto definition = memory::safe_read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
		const auto item_id = memory::safe_read<std::uint64_t>( iv + SCHEMA( "C_EconItemView", "m_iItemID"_hash ) );
		if ( !definition || !item_id ) return false;
		if ( saved.weapon != weapon || saved.def_index != *definition || ( *item_id != gun_faux_item_id && *item_id != saved.item_id ) )
		{
			this->m_original_weapons.erase( handle );
			this->m_applied_weapons.erase( handle );
			return true;
		}
		if ( !visual_identity( weapon ).ready( ) ||
			 !memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) + 8 ).value_or( 0 ) ||
			 !PATTERN( patterns::weapon_update_skin ) || !PATTERN( patterns::weapon_update_composite_material ) ) return false;
		const auto hud = this->find_hud_model_weapon( pawn );
		if ( handle == active_handle && ( pawn == systems::g_local.get( ).pawn || hud ) && !entity_guard::ready( hud ) ) return false;
		if ( !cosmetic_attributes::restore( iv, saved.attributes ) || !entity_guard::current( *entity ) ) return false;
		if ( saved.custom_name && !name_tag::restore( iv, *saved.custom_name ) ) return false;
		if ( !entity_guard::current( *entity ) ) return false;
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
		if ( !this->rebuild_paint( weapon, handle, active_handle, pawn, g_econ_item_system.find_paint_kit( saved.paint_kit ) ) || !entity_guard::current( *entity ) ) return false;
		this->schedule_hud_clear( iv );
		if ( !entity_guard::current( *entity ) ) return false;
		this->m_applied_weapons.erase( handle );
		this->m_original_weapons.erase( handle );
		return true;
	}

	bool guns::rebuild_paint( std::uintptr_t weapon, std::uint32_t handle, std::uint32_t active_handle, std::uintptr_t pawn, const econ_item_system::paint_kit* pk )
	{
		const auto entity = entity_guard::capture( weapon );
		if ( !entity || entity->handle != handle || !entity_guard::ready( pawn ) || !visual_identity( weapon ).ready( ) ) return false;
		const auto subclass = SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash );
		if ( !subclass || !memory::safe_read<std::uintptr_t>( weapon + subclass + 8 ).value_or( 0 ) ) return false;
		const auto mesh = pk && pk->legacy_model ? std::uint64_t{2} : std::uint64_t{1};
		const bool needs_hud = handle == active_handle &&
			( pawn == systems::g_local.get( ).pawn || this->find_hud_model_weapon( pawn ) != 0 );
		// World materials must update even if HUD lookup/binding is not ready.
		if ( !entity_guard::rebuild_materials( *entity, mesh ) ) return false;
		if ( needs_hud && !this->update_view_model(pawn, pk, true) )
			report_skin_failure( "hud-pending", handle, pk ? pk->id : 0 );
		// apply() caches only the completed world update; HUD completion remains pending.
		return entity_guard::current( *entity );
	}

	bool guns::update_view_model( std::uintptr_t pawn, const econ_item_system::paint_kit* pk, bool force )
	{
		// Do not rebuild world materials again on the unchanged-skin fast path.
		// if (force || !current_name || !cosmetic_model::matches(memory::read_string(current_name), target))
		return hud_weapon::update( pawn, pk && pk->legacy_model ? std::uint64_t{2} : std::uint64_t{1}, force );
	}

	void guns::on_render_start( )
	{
		const auto local = systems::g_local.get( );
		const auto pawn = local.observer_pawn ? local.observer_pawn : preview_scene::player_pawn(local.controller);
		if (!preview_scene::player_ready(pawn)) return;
		const auto services = memory::safe_read<std::uintptr_t>(pawn + SCHEMA("C_BasePlayerPawn", "m_pWeaponServices"_hash)).value_or(0);
		if (!services) return;
		const auto handle = memory::safe_read<std::uint32_t>(services + SCHEMA("CPlayer_WeaponServices", "m_hActiveWeapon"_hash)).value_or(0);
		const auto found = this->m_applied_weapons.find(handle);
		if (found == this->m_applied_weapons.end()) return;
		const auto weapon = systems::g_entities.lookup(handle);
		if (!weapon || !cosmetic_cache::reusable(found->second.visual, visual_identity(weapon))) return;
		const auto skin = found->second.skin;
		const auto hud = visual_identity(this->find_hud_model_weapon(pawn));
		const bool refresh = found->second.hud_refresh_pending || found->second.hud != hud;
		const auto pk = g_econ_item_system.find_paint_kit(skin.paint_kit_id);
		if (!refresh) {
			// Preserve mesh-reset recovery without repeated model-path lookups,
			// attachment walks and material callbacks for an unchanged HUD.
			if (const auto entity = entity_guard::capture(hud.weapon))
				(void)entity_guard::set_mesh(*entity, pk && pk->legacy_model ? std::uint64_t{2} : std::uint64_t{1});
			return;
		}
		if (!this->update_view_model(pawn, pk, refresh)) {
			report_skin_failure( "hud-pending", handle, skin.paint_kit_id );
			return; // Leave hud_refresh_pending set until a successful retry.
		}
		// Engine callbacks may invalidate the map: reacquire, never reuse found.
		const auto current = this->m_applied_weapons.find(handle);
		if (current != this->m_applied_weapons.end() && current->second.skin == skin &&
			cosmetic_cache::reusable(current->second.visual, visual_identity(weapon))) {
			current->second.hud_refresh_pending = false;
			current->second.hud = visual_identity(this->find_hud_model_weapon(pawn));
		}
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

				const auto entity = entity_guard::capture( weapon );
				if ( !entity ) continue;
				if ( preview_item::matches( weapon, iv, skin, account, quality ) ) continue;
				if ( !preview_item::write( weapon, iv, skin, account, quality ) ) continue;
				const auto pk = g_econ_item_system.find_paint_kit( skin.paint_kit_id );
				if ( !entity_guard::rebuild_materials( *entity, pk && pk->legacy_model ? std::uint64_t{2} : std::uint64_t{1} ) ) continue;
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
