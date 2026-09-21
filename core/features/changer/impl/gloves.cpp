#include <pch/pch.hpp>
#include "../preview_scene.hpp"
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <utilities/steam/steam.hpp>
#include <protection/game_addresses.hpp>
#include <core/features/changer/cosmetic_attributes.hpp>

namespace features::changer {

	namespace detail {

		constexpr std::array<std::uint16_t, 3> glove_attribute_indices{ 6, 7, 8 };
		constexpr std::array<const char*, 3> glove_attribute_names
		{
			"set item texture prefab",
			"set item texture seed",
			"set item texture wear"
		};

		// Current C_EconItemView local-attribute vector layout, mirrored from Artisan.
		constexpr std::ptrdiff_t item_view_attribute_count_offset{ 0x210 };
		constexpr std::ptrdiff_t item_view_attribute_data_offset{ 0x218 };
		constexpr std::ptrdiff_t item_attribute_stride{ 0x48 };
		constexpr std::ptrdiff_t item_attribute_definition_offset{ 0x30 };
		constexpr std::ptrdiff_t item_attribute_value_offset{ 0x34 };
		constexpr auto maximum_attribute_count{ 16384 };
		constexpr std::size_t post_data_update_index{ 10 };
		constexpr std::uint64_t faux_item_id{ 0xf000000000000010ull };

		[[nodiscard]] std::uint32_t attribute_value_bits( float value )
		{
			std::uint32_t result{};
			static_assert( sizeof( result ) == sizeof( value ) );
			std::memcpy( &result, &value, sizeof( result ) );
			return result;
		}

		struct remote_glove_state {
			std::uint16_t def_index{ 0 };
			std::uint64_t item_id{ 0 };
			std::uint32_t id_high{ 0 };
			std::uint32_t id_low{ 0 };
			std::uint32_t account_id{ 0 };
			bool restore_custom_material{ false };
			bool initialized{ false };
			bool disallow_soc{ false };
			struct attr { float value{}; bool present{}; };
			std::array<attr, 3> attributes{};
			int team{ 0 };
		};
		inline static std::unordered_map<std::uintptr_t, remote_glove_state> s_remote_gloves;

	} // namespace detail

	void gloves::on_frame_stage_notify( )
	{
		const auto local = systems::g_local.get( );
		if ( !local.controller ) return;

		const auto local_ctrl = local.controller;
		const auto local_pawn = preview_scene::player_pawn( local_ctrl );

		if ( preview_scene::player_ready( local_pawn ) )
		{
			const auto local_team = preview_scene::team( local_pawn );
			if ( local_team == 2 || local_team == 3 )
			{
				if ( this->m_tracked_pawn != local_pawn )
				{
					this->reset( );
					this->m_tracked_pawn = local_pawn;
				}

				const settings::changer::applied_skin* selected_skin{ nullptr };
				const econ_item_system::item_def* selected_glove_def{ nullptr };

				for ( const auto& [def_index, skin] : settings::g_changer.skins.for_team( local_team ) )
				{
					const auto def = g_econ_item_system.find_def( def_index );
					if ( !def || def->category != econ_item_system::item_category::glove )
					{
						continue;
					}

					selected_skin = &skin;
					selected_glove_def = def;
					break;
				}

				const auto item_view = local_pawn + SCHEMA( "C_CSPlayerPawn", "m_EconGloves"_hash );
				if ( !selected_glove_def )
				{
					if ( this->m_overridden )
					{
						this->restore( local_pawn, item_view, local_team );
					}
				}
				else if ( this->m_original.captured || this->capture_original( item_view ) )
				{
					const auto current_def = memory::read<std::uint16_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
					const auto current_id = memory::read<std::uint64_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemID"_hash ) );
					const auto needs_reapply = memory::read<bool>( local_pawn + SCHEMA( "C_CSPlayerPawn", "m_bNeedToReApplyGloves"_hash ) );

					if ( !( current_def == static_cast< std::uint16_t >( selected_glove_def->def_index ) &&
						current_id == detail::faux_item_id &&
						this->paint_attributes_match( item_view, *selected_skin ) &&
						!needs_reapply ) )
					{
						auto steam_id = steam::user::get_steam_id( );
						if ( !steam_id && local_ctrl )
						{
							steam_id = memory::read<std::uint64_t>( local_ctrl + SCHEMA( "CBasePlayerController", "m_steamID"_hash ) );
						}
						this->apply( local_pawn, item_view, local_team, *selected_glove_def, *selected_skin, static_cast< std::uint32_t >( steam_id ) );
					}
				}
			}
		}

		// Restore synced gloves if sync was disabled
		if ( !g_skin_sync.is_enabled( ) )
		{
			for ( auto it = detail::s_remote_gloves.begin( ); it != detail::s_remote_gloves.end( ); )
			{
				const auto pawn = it->first;
				const auto& state = it->second;
				if ( entity_guard::ready( pawn ) )
				{
					const auto item_view = pawn + SCHEMA( "C_CSPlayerPawn", "m_EconGloves"_hash );
					if ( item_view && item_view >= 0x10000 )
					{
						const auto set_attribute = PATTERN( patterns::econ_item_view_set_attribute );
						const auto remove_attribute = PATTERN( patterns::econ_item_view_remove_attribute );
						cosmetic_attributes::sanitize( item_view );
						for ( std::size_t slot = 0; slot < detail::glove_attribute_indices.size( ); ++slot )
						{
							if ( state.attributes[ slot ].present && set_attribute )
								memory::safe_call<void>( set_attribute, item_view, detail::glove_attribute_names[ slot ], state.attributes[ slot ].value );
							else if ( remove_attribute )
								memory::safe_call<void>( remove_attribute, item_view, static_cast< int >( detail::glove_attribute_indices[ slot ] ) );
						}
						memory::write<std::uint16_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ), state.def_index );
						memory::write<std::uint64_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemID"_hash ), state.item_id );
						memory::write<std::uint32_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ), state.id_high );
						memory::write<std::uint32_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ), state.id_low );
						memory::write<std::uint32_t>( item_view + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ), state.account_id );
						memory::write<bool>( item_view + SCHEMA( "C_EconItemView", "m_bRestoreCustomMaterialAfterPrecache"_hash ), state.restore_custom_material );
						memory::write<bool>( item_view + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), state.initialized );
						memory::write<bool>( item_view + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ), state.disallow_soc );
						this->refresh( pawn, item_view, state.team );
					}
				}
				it = detail::s_remote_gloves.erase( it );
			}
		}

		// Apply synced gloves for remote players
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

				const auto remote_team = memory::safe_read<std::uint8_t>( pawn + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) ).value_or( 0 );
				if ( remote_team != 2 && remote_team != 3 )
				{
					continue;
				}

				const auto remote_item_view = pawn + SCHEMA( "C_CSPlayerPawn", "m_EconGloves"_hash );
				if ( !remote_item_view || remote_item_view < 0x10000 )
				{
					continue;
				}

				const auto remote_skin_data = g_skin_sync.get_remote_skin( sid );
				settings::changer::applied_skin remote_glove_skin{};
				const econ_item_system::item_def* remote_glove_def{ nullptr };
				bool has_glove_skin{ false };
				if ( remote_skin_data && !remote_skin_data->skins.empty( ) )
				{
					for ( const auto& [def_index, skin] : remote_skin_data->skins )
					{
						const auto def = g_econ_item_system.find_def( def_index );
						if ( def && def->category == econ_item_system::item_category::glove )
						{
							remote_glove_skin = skin;
							remote_glove_def = def;
							has_glove_skin = true;
							break;
						}
					}
				}

				if ( !remote_glove_def || !has_glove_skin )
				{
					const auto it = detail::s_remote_gloves.find( pawn );
					if ( it != detail::s_remote_gloves.end( ) )
					{
						const auto& state = it->second;
						const auto set_attribute = PATTERN( patterns::econ_item_view_set_attribute );
						const auto remove_attribute = PATTERN( patterns::econ_item_view_remove_attribute );
						cosmetic_attributes::sanitize( remote_item_view );
						for ( std::size_t slot = 0; slot < detail::glove_attribute_indices.size( ); ++slot )
						{
							if ( state.attributes[ slot ].present && set_attribute )
								memory::safe_call<void>( set_attribute, remote_item_view, detail::glove_attribute_names[ slot ], state.attributes[ slot ].value );
							else if ( remove_attribute )
								memory::safe_call<void>( remove_attribute, remote_item_view, static_cast< int >( detail::glove_attribute_indices[ slot ] ) );
						}
						memory::write<std::uint16_t>( remote_item_view + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ), state.def_index );
						memory::write<std::uint64_t>( remote_item_view + SCHEMA( "C_EconItemView", "m_iItemID"_hash ), state.item_id );
						memory::write<std::uint32_t>( remote_item_view + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ), state.id_high );
						memory::write<std::uint32_t>( remote_item_view + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ), state.id_low );
						memory::write<std::uint32_t>( remote_item_view + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ), state.account_id );
						memory::write<bool>( remote_item_view + SCHEMA( "C_EconItemView", "m_bRestoreCustomMaterialAfterPrecache"_hash ), state.restore_custom_material );
						memory::write<bool>( remote_item_view + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), state.initialized );
						memory::write<bool>( remote_item_view + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ), state.disallow_soc );
						this->refresh( pawn, remote_item_view, state.team );
						detail::s_remote_gloves.erase( it );
					}
					continue;
				}

				std::array<attribute_state, 3> dummy_attrs{};
				if ( !this->read_paint_attributes( remote_item_view, dummy_attrs ) )
				{
					continue;
				}

				const auto current_def = memory::safe_read<std::uint16_t>( remote_item_view + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) ).value_or( 0 );
				const auto current_id = memory::safe_read<std::uint64_t>( remote_item_view + SCHEMA( "C_EconItemView", "m_iItemID"_hash ) ).value_or( 0 );

				if ( current_def == static_cast< std::uint16_t >( remote_glove_def->def_index ) &&
					 current_id == detail::faux_item_id &&
					 this->paint_attributes_match( remote_item_view, remote_glove_skin ) &&
					 !memory::safe_read<bool>( pawn + SCHEMA( "C_CSPlayerPawn", "m_bNeedToReApplyGloves"_hash ) ).value_or( true ) )
				{
					continue;
				}

				if ( detail::s_remote_gloves.find( pawn ) == detail::s_remote_gloves.end( ) )
				{
					detail::remote_glove_state saved{};
					saved.def_index = current_def;
					saved.item_id = current_id;
					saved.id_high = memory::safe_read<std::uint32_t>( remote_item_view + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ) ).value_or( 0 );
					saved.id_low = memory::safe_read<std::uint32_t>( remote_item_view + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ) ).value_or( 0 );
					saved.account_id = memory::safe_read<std::uint32_t>( remote_item_view + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ) ).value_or( 0 );
					saved.restore_custom_material = memory::safe_read<bool>( remote_item_view + SCHEMA( "C_EconItemView", "m_bRestoreCustomMaterialAfterPrecache"_hash ) ).value_or( false );
					saved.initialized = memory::safe_read<bool>( remote_item_view + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ) ).value_or( false );
					saved.disallow_soc = memory::safe_read<bool>( remote_item_view + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ) ).value_or( false );
					saved.team = remote_team;
					for ( std::size_t i = 0; i < 3; ++i )
					{
						saved.attributes[ i ] = { dummy_attrs[ i ].value, dummy_attrs[ i ].present };
					}
					detail::s_remote_gloves[ pawn ] = saved;
				}

				this->apply( pawn, remote_item_view, remote_team, *remote_glove_def, remote_glove_skin, static_cast< std::uint32_t >( sid ) );
			}
		}
	}

	bool gloves::capture_original( std::uintptr_t item_view )
	{
		if ( !this->read_paint_attributes( item_view, this->m_original_attributes ) )
		{
			return false;
		}

		this->m_original.def_index = memory::read<std::uint16_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
		this->m_original.item_id = memory::read<std::uint64_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemID"_hash ) );
		this->m_original.id_high = memory::read<std::uint32_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ) );
		this->m_original.id_low = memory::read<std::uint32_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ) );
		this->m_original.account_id = memory::read<std::uint32_t>( item_view + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ) );
		this->m_original.restore_custom_material = memory::read<bool>( item_view + SCHEMA( "C_EconItemView", "m_bRestoreCustomMaterialAfterPrecache"_hash ) );
		this->m_original.initialized = memory::read<bool>( item_view + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ) );
		this->m_original.disallow_soc = memory::read<bool>( item_view + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ) );
		this->m_original.captured = true;
		return true;
	}

	bool gloves::read_paint_attributes( std::uintptr_t item_view, std::array<attribute_state, 3>& attributes ) const
	{
		attributes = {};
		cosmetic_attributes::snapshot snapshot{};
		if ( !item_view || !cosmetic_attributes::capture( item_view, snapshot ) ) return false;
		for ( std::size_t i = 0; i < attributes.size( ); ++i )
		{
			attributes[i].present = snapshot[i].present;
			attributes[i].value = std::bit_cast<float>( snapshot[i].bits );
		}
		return true;
	}

	bool gloves::restore_paint_attributes( std::uintptr_t item_view ) const
	{
		const auto set_attribute = PATTERN( patterns::econ_item_view_set_attribute );
		const auto remove_attribute = PATTERN( patterns::econ_item_view_remove_attribute );
		if ( !set_attribute || !remove_attribute || !item_view )
		{
			return false;
		}

		cosmetic_attributes::sanitize( item_view );

		for ( std::size_t slot = 0; slot < detail::glove_attribute_indices.size( ); ++slot )
		{
			if ( this->m_original_attributes[ slot ].present )
			{
				memory::safe_call<void>( set_attribute, item_view, detail::glove_attribute_names[ slot ], this->m_original_attributes[ slot ].value );
			}
			else
			{
				memory::safe_call<void>( remove_attribute, item_view, static_cast< int >( detail::glove_attribute_indices[ slot ] ) );
			}
		}

		return true;
	}

	bool gloves::paint_attributes_match( std::uintptr_t item_view, const settings::changer::applied_skin& skin ) const
	{
		std::array<attribute_state, 3> attributes{};
		if ( !this->read_paint_attributes( item_view, attributes ) )
		{
			return false;
		}

		const std::array expected
		{
			static_cast< float >( skin.paint_kit_id ),
			static_cast< float >( skin.seed ),
			skin.wear
		};

		for ( std::size_t slot = 0; slot < expected.size( ); ++slot )
		{
			if ( !attributes[ slot ].present ||
				detail::attribute_value_bits( attributes[ slot ].value ) != detail::attribute_value_bits( expected[ slot ] ) )
			{
				return false;
			}
		}

		return true;
	}

	void gloves::apply( std::uintptr_t pawn, std::uintptr_t item_view, int team, const econ_item_system::item_def& def, const settings::changer::applied_skin& skin, std::uint32_t account_id )
	{
		if ( !item_view ) return;

		const bool is_local = ( pawn == systems::g_local.get( ).pawn );
		const auto set_attribute = PATTERN( patterns::econ_item_view_set_attribute );
		if ( !set_attribute ) return;

		// The engine owns attribute allocation and notifications for both local and
		// remote gloves. An empty valid vector is not a reason to skip application.
		const cosmetic_paint::values values{ static_cast<float>( skin.paint_kit_id ), static_cast<float>( skin.seed ), skin.wear };
		if ( !cosmetic_paint::ensure<std::array<attribute_state, 3>>( values,
			[&]( std::array<attribute_state, 3>& attributes ) { return this->read_paint_attributes( item_view, attributes ); },
			[&]( std::size_t slot, float value ) {
				memory::safe_call<void>( set_attribute, item_view, detail::glove_attribute_names[ slot ], value );
			} ) ) return;

		// Attribute setters can re-enter the engine; defer identity publication if unready.
		if ( !entity_guard::ready( pawn ) ) return;
		// Publish identity and refresh only after a fresh read confirms all attributes.
		memory::write<std::uint16_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ), static_cast< std::uint16_t >( def.def_index ) );
		memory::write<std::uint64_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemID"_hash ), detail::faux_item_id );
		memory::write<std::uint32_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ), static_cast< std::uint32_t >( detail::faux_item_id >> 32 ) );
		memory::write<std::uint32_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ), static_cast< std::uint32_t >( detail::faux_item_id ) );
		memory::write<std::uint32_t>( item_view + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ), account_id );
		memory::write<bool>( item_view + SCHEMA( "C_EconItemView", "m_bRestoreCustomMaterialAfterPrecache"_hash ), true );
		memory::write<bool>( item_view + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), true );
		memory::write<bool>( item_view + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ), true );

		if ( is_local ) this->m_overridden = true;
		this->refresh( pawn, item_view, team );
	}

	void gloves::restore( std::uintptr_t pawn, std::uintptr_t item_view, int team )
	{
		if ( !this->m_original.captured || !this->restore_paint_attributes( item_view ) )
		{
			return;
		}

		memory::write<std::uint16_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ), this->m_original.def_index );
		memory::write<std::uint64_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemID"_hash ), this->m_original.item_id );
		memory::write<std::uint32_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ), this->m_original.id_high );
		memory::write<std::uint32_t>( item_view + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ), this->m_original.id_low );
		memory::write<std::uint32_t>( item_view + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ), this->m_original.account_id );
		memory::write<bool>( item_view + SCHEMA( "C_EconItemView", "m_bRestoreCustomMaterialAfterPrecache"_hash ), this->m_original.restore_custom_material );
		memory::write<bool>( item_view + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), this->m_original.initialized );
		memory::write<bool>( item_view + SCHEMA( "C_EconItemView", "m_bDisallowSOC"_hash ), this->m_original.disallow_soc );

		this->refresh( pawn, item_view, team );
		this->m_original = {};
		this->m_original_attributes = {};
		this->m_overridden = false;
	}

	void gloves::refresh( std::uintptr_t pawn, std::uintptr_t item_view, int team ) const
	{
		const auto entity = entity_guard::capture( pawn );
		if ( !entity ) return;
		const auto invalidate = PATTERN( patterns::econ_item_view_invalidate_description );
		if ( invalidate && item_view ) memory::safe_call<void>( invalidate, item_view );
		if ( !entity_guard::current( *entity ) ) return;
		const auto reapply_offset = SCHEMA( "C_CSPlayerPawn", "m_bNeedToReApplyGloves"_hash );
		if ( reapply_offset ) memory::write<bool>( pawn + reapply_offset, true );
		if ( pawn == systems::g_local.get( ).pawn ) memory::call_vfunc<void>( pawn, detail::post_data_update_index, 1 );
		if ( !entity_guard::current( *entity ) ) return;
		const auto set_bodygroup = PATTERN( patterns::set_bodygroup );
		if ( set_bodygroup )
		{
			memory::safe_call<void>( set_bodygroup, pawn, 0, 1u );
			if ( !entity_guard::current( *entity ) ) return;
			memory::safe_call<void>( set_bodygroup, pawn, team == 2 ? 0 : 1, 1u );
		}
	}

	void gloves::on_lobby( )
	{
		// Preview entities are engine-owned. Match equipment is applied only
		// through on_frame_stage_notify, never through lobby/intro previews.
	}

	void gloves::reset( )
	{
		this->m_original = {};
		this->m_original_attributes = {};
		this->m_tracked_pawn = 0;
		this->m_overridden = false;
		detail::s_remote_gloves.clear( );
	}

} // namespace features::changer
