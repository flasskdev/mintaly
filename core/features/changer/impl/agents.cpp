#include <pch/pch.hpp>
#include "../preview_scene.hpp"
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>
#include <utilities/cosmetic_model.hpp>
#include <utilities/steam/steam.hpp>
#include <core/features/changer/skin_sync.hpp>

namespace features::changer {

	std::string g_last_applied_model{};

namespace {
	// Keep SEH in a frame without C++ objects requiring unwinding (MSVC C2712).
	bool apply_model_safe( const entity_guard::stamp& entity, const char* model, bool precache )
	{
		struct buffer_string {
			std::uint32_t m_unknown1 {};
			std::uint32_t m_unknown2 { 0xc00000c8 };
			union { std::uintptr_t m_str_ptr; std::uint8_t data[ 0xc8 ]; };
			std::uintptr_t m_unknown3 {};
			std::uintptr_t m_unknown4 {};
		} buffer;
		if ( !model || !*model ) return false;
		__try
		{
			if ( !entity_guard::current( entity ) ) return false;
			const auto init = PATTERN( patterns::init_particle_path_buffer );
			const auto cache = PATTERN( patterns::resource_system_precache );
			if ( precache && init && cache && addresses::globals::resource_system )
			{
				memory::call<void>( init, &buffer, model );
				buffer.m_unknown4 = 'ldmv';
				memory::call<void>( cache, addresses::globals::resource_system, &buffer, "" );
			}
			// Precache can re-enter the engine. Check the original full handle again.
			return entity_guard::set_model( entity, model );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER ) { return false; }
	}

	bool replace_agent_model( std::uintptr_t pawn, const std::string& path )
	{
		const auto entity = entity_guard::capture( pawn );
		if ( !entity || path.empty( ) ) return false;
		const auto collision_offset = SCHEMA( "C_BaseModelEntity", "m_Collision"_hash );
		const auto mins_offset = SCHEMA( "CCollisionProperty", "m_vecMins"_hash );
		const auto maxs_offset = SCHEMA( "CCollisionProperty", "m_vecMaxs"_hash );
		const auto collision = collision_offset ? pawn + collision_offset : 0;
		const auto mins = ( collision && mins_offset ) ? memory::safe_read<math::vector3>( collision + mins_offset ) : std::nullopt;
		const auto maxs = ( collision && maxs_offset ) ? memory::safe_read<math::vector3>( collision + maxs_offset ) : std::nullopt;
		const bool applied = apply_model_safe( *entity, path.c_str( ), path.ends_with( ".vmdl" ) );
		if ( !entity_guard::current( *entity ) ) return false;
		// Do not overwrite a crouched hull or write through a recycled pawn.
		if ( collision && mins && maxs )
		{
			memory::safe_write<math::vector3>( collision + mins_offset, *mins );
			memory::safe_write<math::vector3>( collision + maxs_offset, *maxs );
		}
		return applied;
	}

	std::uintptr_t agent_model_state( std::uintptr_t pawn )
	{
		const auto node = memory::safe_read<std::uintptr_t>( pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
		return node ? node + SCHEMA( "CSkeletonInstance", "m_modelState"_hash ) : 0;
	}

	std::uintptr_t agent_model_handle( std::uintptr_t pawn )
	{
		const auto state = agent_model_state( pawn );
		return state ? memory::safe_read<std::uintptr_t>( state + SCHEMA( "CModelState", "m_hModel"_hash ) ).value_or( 0 ) : 0;
	}

	bool agent_model_matches( std::uintptr_t pawn, const std::string& wanted )
	{
		const auto model_state = agent_model_state( pawn );
		if ( !model_state ) return false;
		const auto name = memory::safe_read<std::uintptr_t>( model_state + SCHEMA( "CModelState", "m_ModelName"_hash ) ).value_or( 0 );
		return name && cosmetic_model::matches( memory::read_string( name ), wanted );
	}

	struct remote_agent_state {
		std::int16_t def_index{ 0 };
		std::uintptr_t model_handle{ 0 };
		int team{ 0 };
	};
	static std::unordered_map<std::uintptr_t, remote_agent_state> s_remote_agents;

	bool is_preview_player( const char* schema_name )
	{
		if ( !schema_name ) return false;
		const auto hash = fnv1a::runtime_hash( schema_name );
		if ( hash == "C_CSGO_PreviewPlayer"_hash ||
			 hash == "C_CSGO_TeamPreviewModel"_hash ||
			 hash == "C_CSGO_PreviewPlayerAlias_csgo_player_previewmodel"_hash ||
			 hash == "csgo_player_previewmodel"_hash ||
			 hash == "csgo_previewplayer"_hash )
		{
			return true;
		}
		const std::string_view sv( schema_name );
		return ( sv.find( "PreviewPlayer" ) != std::string_view::npos ||
				 sv.find( "preview_player" ) != std::string_view::npos ||
				 sv.find( "player_preview" ) != std::string_view::npos ||
				 sv.find( "TeamPreviewModel" ) != std::string_view::npos );
	}
}

	void agents::on_frame_stage_notify( )
	{
		const auto local = systems::g_local.get( );
		if ( !local.controller ) return;

		const auto local_ctrl = local.controller;
		const auto player_pawn_offset = SCHEMA( "CCSPlayerController", "m_hPlayerPawn"_hash );
		const auto player_handle = player_pawn_offset
			? memory::safe_read<std::uint32_t>( local_ctrl + player_pawn_offset ).value_or( 0 ) : 0;
		auto local_pawn = player_handle && player_handle != 0xffffffff
			? systems::g_entities.lookup( player_handle ) : 0;
		if ( !local_pawn && local.pawn )
			local_pawn = local.pawn;
		const auto local_class = local_pawn ? systems::g_entities.get_schema_name( local_pawn ) : nullptr;

		// Player pawns remain valid in intro/end scenes. Never substitute an observer
		// pawn merely because it is the controller's current m_hPawn.
		if ( local_pawn && local_class && fnv1a::runtime_hash( local_class ) == "C_CSPlayerPawn"_hash )
		{
			const auto apply_local = [&]() {
				auto team = memory::safe_read<std::uint8_t>( local_pawn + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) ).value_or( 0 );
				if ( team != 2 && team != 3 )
					team = memory::safe_read<std::uint8_t>( local_ctrl + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) ).value_or( 0 );
				if ( team != 2 && team != 3 )
					team = local.team;
				if ( team != 2 && team != 3 )
					team = this->m_tracked_team;
				const auto selected_def_index = ( team == 3 ) ? settings::g_changer.agents.ct_def : ( team == 2 ) ? settings::g_changer.agents.t_def : static_cast< std::int16_t >( 0 );

				const econ_item_system::item_def* selected{ nullptr };
				if ( selected_def_index != 0 )
				{
					selected = g_econ_item_system.find_def( selected_def_index );
				}

				// custom player model (agent) ?
				std::string model_path;
				{
					const auto& custom_agents = settings::g_changer.custom_agents;
					const auto custom_idx = ( team == 3 ) ? custom_agents.selected_ct
					                     : ( team == 2 ) ? custom_agents.selected_t : -1;

					if ( custom_idx >= 0 && custom_idx < static_cast< int >( custom_agents.entries.size( ) ) )
					{
						const auto& entry = custom_agents.entries[ custom_idx ];
						if ( ( entry.team == team || entry.team == 0 ) && !entry.model_path.empty( ) )
							model_path = entry.model_path;
					}
				}

				if ( model_path.empty( ) && selected )
					model_path = selected->model_player;
				model_path = cosmetic_model::canonical( model_path );

				if ( this->m_tracked_pawn != local_pawn )
				{
					this->m_original_model.clear( );
					this->m_applied_model.clear( );
					this->m_overridden = false;
					this->m_applied_handle = 0;
					this->m_applied_def = 0;
					this->m_tracked_team = 0;
					this->m_tracked_pawn = local_pawn;
				}

				const auto game_scene_node = memory::read<std::uintptr_t>( local_pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );
				if ( game_scene_node )
				{
					const auto model_state = game_scene_node + SCHEMA( "CSkeletonInstance", "m_modelState"_hash );
					const auto cur_hModel = memory::read<std::uintptr_t>( model_state + SCHEMA( "CModelState", "m_hModel"_hash ) );

					if ( cur_hModel != 0 )
					{
						if ( model_path.empty( ) )
						{
							if ( this->m_overridden && !this->m_original_model.empty( ) )
							{
								if ( !replace_agent_model( local_pawn, cosmetic_model::canonical( this->m_original_model ) ) ||
									!agent_model_matches( local_pawn, this->m_original_model ) ) return;
								this->cycle_weapon_owners( local_pawn );
								this->m_applied_handle = 0;
								this->m_applied_def = 0;
								this->m_applied_model.clear( );
								this->m_tracked_team = 0;
								this->m_overridden = false;
							}
						}
						else
						{
							if ( !this->m_overridden && this->m_original_model.empty( ) )
							{
								const auto model_name_ptr = memory::read<std::uintptr_t>( model_state + SCHEMA( "CModelState", "m_ModelName"_hash ) );
								if ( model_name_ptr )
								{
									this->m_original_model = memory::read_string( model_name_ptr );
								}
							}

							const auto current_handle = memory::read<std::uintptr_t>( model_state + SCHEMA( "CModelState", "m_hModel"_hash ) );
							const auto selection_matches = ( this->m_applied_def == ( selected ? selected->def_index : 0 ) ) && this->m_applied_model == model_path;
							const auto team_matches = ( this->m_tracked_team == team );
							const auto handle_matches = ( this->m_applied_handle != 0 && current_handle == this->m_applied_handle ) &&
								agent_model_matches( local_pawn, model_path );

							if ( !( this->m_overridden && selection_matches && team_matches && handle_matches ) )
							{
								if ( this->m_overridden && !team_matches )
								{
									this->m_original_model.clear( );
									this->m_applied_model.clear( );
									this->m_overridden = false;

									const auto model_name_ptr = memory::read<std::uintptr_t>( model_state + SCHEMA( "CModelState", "m_ModelName"_hash ) );
									if ( model_name_ptr )
									{
										this->m_original_model = memory::read_string( model_name_ptr );
									}
								}

								if ( model_path.size( ) >= 7 && model_path.substr( model_path.size( ) - 7 ) == ".vmdl_c" )
									model_path = model_path.substr( 0, model_path.size( ) - 2 );

								auto to_lower = []( unsigned char c ) { return ( c >= 'A' && c <= 'Z' ) ? static_cast< char >( c + 32 ) : static_cast< char >( c ); };
								auto icontains = [&]( const std::string& s, const char* needle, std::size_t nlen )
								{
									if ( s.size( ) < nlen ) return false;
									for ( std::size_t i = 0; i + nlen <= s.size( ); ++i )
									{
										bool ok = true;
										for ( std::size_t j = 0; j < nlen; ++j )
											if ( to_lower( static_cast< unsigned char >( s[ i + j ] ) ) != to_lower( static_cast< unsigned char >( needle[ j ] ) ) )
											{ ok = false; break; }
										if ( ok ) return true;
									}
									return false;
								};

								const bool bad =
									icontains( model_path, "_arm", 4 ) ||
									icontains( model_path, "arms", 4 ) ||
									icontains( model_path, "viewmodel", 8 ) ||
									icontains( model_path, "/arm.", 5 ) ||
									icontains( model_path, "\\arm.", 5 );

								if ( bad )
								{
									this->m_applied_model.clear( );
									auto& ca = settings::g_changer.custom_agents;
									if ( team == 3 ) ca.selected_ct = -1; else ca.selected_t = -1;
								}
								else
								{
									// Do not cache an old handle while the requested model is loading.
									if ( !replace_agent_model( local_pawn, model_path ) ||
										!agent_model_matches( local_pawn, model_path ) ) return;
									g_last_applied_model = model_path;

									this->cycle_weapon_owners( local_pawn );

									this->m_applied_handle = agent_model_handle( local_pawn );
									this->m_applied_def = selected ? selected->def_index : 0;
									this->m_applied_model = model_path;
									this->m_tracked_team = team;
									this->m_overridden = true;
								}
							}
						}
					}
				}
			};
			apply_local( ); // A pending local resource must not skip remote players.

			// The engine may reset the player model (e.g. during intro/endgame cutscenes).
			// If the model handle or applied model no longer matches, clear the
			// cache so the agent is re-verified and re-applied next frame.
			if ( this->m_overridden && ( ( this->m_applied_handle != 0 && agent_model_handle( local_pawn ) != this->m_applied_handle ) ||
				( !this->m_applied_model.empty( ) && !agent_model_matches( local_pawn, this->m_applied_model ) ) ) )
			{
				this->m_applied_handle = 0;
			}
		}

		// Apply synced agents for remote players
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
			if ( !remote_skin_data )
			{
				continue;
			}

			auto pawn_handle = player_pawn_offset
				? memory::safe_read<std::uint32_t>( ctrl + player_pawn_offset ).value_or( 0 ) : 0;
			if ( !pawn_handle || pawn_handle == 0xffffffff )
			{
				const auto hpawn_off = SCHEMA( "CBasePlayerController", "m_hPawn"_hash );
				pawn_handle = hpawn_off ? memory::safe_read<std::uint32_t>( ctrl + hpawn_off ).value_or( 0 ) : 0;
			}
			if ( !pawn_handle || pawn_handle == 0xffffffff )
			{
				continue;
			}

			const auto pawn = systems::g_entities.lookup( pawn_handle );
			if ( !pawn || pawn < 0x10000 )
			{
				continue;
			}

			const auto remote_class = systems::g_entities.get_schema_name( pawn );
			if ( !remote_class || fnv1a::runtime_hash( remote_class ) != "C_CSPlayerPawn"_hash ) continue;

			auto remote_team = memory::safe_read<std::uint8_t>( pawn + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) ).value_or( 0 );
			if ( remote_team != 2 && remote_team != 3 )
				remote_team = memory::safe_read<std::uint8_t>( ctrl + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) ).value_or( 0 );
			if ( remote_team != 2 && remote_team != 3 )
			{
				continue;
			}

			const auto remote_agent_def = ( remote_team == 3 ) ? remote_skin_data->agent_ct : remote_skin_data->agent_t;
			if ( !remote_agent_def )
			{
				continue;
			}

			const auto agent_item = g_econ_item_system.find_def( remote_agent_def );
			if ( !agent_item || agent_item->category != econ_item_system::item_category::agent || agent_item->model_player.empty( ) ||
				 ( agent_item->team( ) != 0 && agent_item->team( ) != remote_team ) )
			{
				continue;
			}

			// Reject any custom disk path models (must be standard game character model, no drive letters or traversals)
			const std::string& raw_model = agent_item->model_player;
			if ( raw_model.find( ":" ) != std::string::npos || raw_model.find( ".." ) != std::string::npos ||
				 raw_model.starts_with( "/" ) || raw_model.starts_with( "\\" ) )
			{
				continue;
			}

			std::string remote_model = agent_item->model_player;
			if ( remote_model.size( ) >= 7 && remote_model.substr( remote_model.size( ) - 7 ) == ".vmdl_c" )
				remote_model = remote_model.substr( 0, remote_model.size( ) - 2 );

			const auto remote_gsn = memory::safe_read<std::uintptr_t>( pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
			if ( !remote_gsn )
			{
				continue;
			}

			const auto remote_model_state = remote_gsn + SCHEMA( "CSkeletonInstance", "m_modelState"_hash );
			const auto remote_model_handle = memory::safe_read<std::uintptr_t>( remote_model_state + SCHEMA( "CModelState", "m_hModel"_hash ) ).value_or( 0 );
			if ( !remote_model_handle )
			{
				continue;
			}

			const auto it = s_remote_agents.find( pawn );
			if ( it != s_remote_agents.end( ) && it->second.def_index == remote_agent_def && it->second.team == remote_team && it->second.model_handle == remote_model_handle && agent_model_matches( pawn, remote_model ) )
			{
				continue;
			}

			if ( !replace_agent_model( pawn, remote_model ) ) continue;
			if ( !agent_model_matches( pawn, remote_model ) ) continue; // Resource still loading: retry.

			this->cycle_weapon_owners( pawn );

			const auto new_handle = agent_model_handle( pawn );
			s_remote_agents[ pawn ] = { remote_agent_def, new_handle, remote_team };
		}
	}

	void agents::cycle_weapon_owners( std::uintptr_t pawn )
	{
		const auto weapon_services = memory::safe_read<std::uintptr_t>( pawn + SCHEMA( "C_BasePlayerPawn", "m_pWeaponServices"_hash ) ).value_or( 0 );
		if ( weapon_services )
		{
			const auto weapons_base = weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hMyWeapons"_hash );
			const auto weapons_size = memory::read<int>( weapons_base );
			const auto weapons_data = memory::read<std::uintptr_t>( weapons_base + 0x8 );

			if ( weapons_data && weapons_size > 0 && weapons_size < 64 )
			{
				for ( auto i = 0; i < weapons_size; ++i )
				{
					const auto handle = memory::read<std::uint32_t>( weapons_data + i * sizeof( std::uint32_t ) );
					const auto weapon = systems::g_entities.lookup( handle );

					if ( !weapon )
					{
						continue;
					}

					const auto owner_off = SCHEMA( "C_BaseEntity", "m_hOwnerEntity"_hash );
					if ( !owner_off ) continue;
					const auto saved_owner = memory::read<std::uint32_t>( weapon + owner_off );

					memory::write<std::uint32_t>( weapon + owner_off, 0xffffffff );
					memory::write<std::uint32_t>( weapon + owner_off, saved_owner );
				}
			}
		}

	}

	void agents::on_lobby( )
	{
		for ( const auto& preview : preview_scene::players )
		{
			const auto ent = preview.pawn;
			const auto sid = preview.steam_id;
			const auto team = preview.team;
			if ( !sid || ( team != 2 && team != 3 ) ) continue;
			const bool is_local = preview_scene::is_local( preview );

			std::string target_model;
			if ( is_local )
			{
				const auto selected_def_index = ( team == 3 ) ? settings::g_changer.agents.ct_def : ( team == 2 ) ? settings::g_changer.agents.t_def : static_cast< std::int16_t >( 0 );
				const econ_item_system::item_def* selected{ nullptr };
				if ( selected_def_index != 0 )
				{
					selected = g_econ_item_system.find_def( selected_def_index );
				}

				const auto& custom_agents = settings::g_changer.custom_agents;
				const auto custom_idx = ( team == 3 ) ? custom_agents.selected_ct : custom_agents.selected_t;
				if ( custom_idx >= 0 && custom_idx < static_cast< int >( custom_agents.entries.size( ) ) )
				{
					const auto& entry = custom_agents.entries[ custom_idx ];
					if ( ( entry.team == team || entry.team == 0 ) && !entry.model_path.empty( ) )
						target_model = entry.model_path;
				}

				if ( target_model.empty( ) && selected )
				{
					target_model = selected->model_player;
				}
			}
			else
			{
				const auto remote_skin_data = g_skin_sync.get_remote_skin( sid );
				if ( remote_skin_data )
				{
					const auto remote_agent_def = ( team == 3 ) ? remote_skin_data->agent_ct : remote_skin_data->agent_t;
					if ( remote_agent_def != 0 )
					{
						const auto agent_item = g_econ_item_system.find_def( remote_agent_def );
						if ( agent_item && agent_item->category == econ_item_system::item_category::agent && !agent_item->model_player.empty( ) &&
							 ( agent_item->team( ) == 0 || agent_item->team( ) == team ) )
						{
							const std::string& raw_model = agent_item->model_player;
							if ( raw_model.find( ":" ) == std::string::npos && raw_model.find( ".." ) == std::string::npos &&
								 !raw_model.starts_with( "/" ) && !raw_model.starts_with( "\\" ) )
							{
								target_model = agent_item->model_player;
							}
						}
					}
				}
			}

			if ( target_model.empty( ) ) continue;

			target_model = cosmetic_model::canonical( target_model );
			if ( target_model.size( ) >= 7 && target_model.substr( target_model.size( ) - 7 ) == ".vmdl_c" )
				target_model = target_model.substr( 0, target_model.size( ) - 2 );

			auto to_lower = []( unsigned char c ) { return ( c >= 'A' && c <= 'Z' ) ? static_cast< char >( c + 32 ) : static_cast< char >( c ); };
			auto icontains = [&]( const std::string& s, const char* needle, std::size_t nlen )
			{
				if ( s.size( ) < nlen ) return false;
				for ( std::size_t j = 0; j + nlen <= s.size( ); ++j )
				{
					bool ok = true;
					for ( std::size_t k = 0; k < nlen; ++k )
						if ( to_lower( static_cast< unsigned char >( s[ j + k ] ) ) != to_lower( static_cast< unsigned char >( needle[ k ] ) ) )
						{ ok = false; break; }
					if ( ok ) return true;
				}
				return false;
			};

			const bool bad =
				icontains( target_model, "_arm", 4 ) ||
				icontains( target_model, "arms", 4 ) ||
				icontains( target_model, "viewmodel", 8 ) ||
				icontains( target_model, "/arm.", 5 ) ||
				icontains( target_model, "\\arm.", 5 );

			if ( bad ) continue;

			if ( agent_model_matches( ent, target_model ) ) continue;

			if ( replace_agent_model( ent, target_model ) )
			{
				if ( is_local )
				{
					g_last_applied_model = target_model;
				}
				this->cycle_weapon_owners( ent );
			}
		}
	}

	void agents::reset( )
	{
		this->m_original_model.clear( );
		this->m_applied_model.clear( );
		this->m_tracked_pawn = 0;
		this->m_applied_handle = 0;
		this->m_applied_def = 0;
		this->m_overridden = false;
		this->m_tracked_team = 0;
		s_remote_agents.clear( );
	}

} // namespace features::changer