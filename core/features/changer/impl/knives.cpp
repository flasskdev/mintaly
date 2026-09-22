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