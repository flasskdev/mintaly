#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>

#include "../systems.hpp"
#include <protection/game_addresses.hpp>

namespace systems {

	void local::update( )
	{
		// TEMP-DIAG: pin down the dead-features chain (remove after triage).
		{
			static auto next = std::chrono::steady_clock::time_point{};
			const auto now = std::chrono::steady_clock::now( );
			if ( now >= next )
			{
				next = now + std::chrono::seconds( 5 );
				const auto ctl = memory::safe_read<std::uintptr_t>( addresses::globals::local_player_controller ).value_or( 0 );
				const auto elist = memory::safe_read<std::uintptr_t>( addresses::globals::entity_list ).value_or( 0 );
				const auto off_pawn = SCHEMA( "CBasePlayerController", "m_hPawn"_hash );
				const auto off_hp = SCHEMA( "C_BaseEntity", "m_iHealth"_hash );
				const auto off_team = SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash );
				const auto pawn_h = ctl && off_pawn ? memory::safe_read<std::uint32_t>( ctl + off_pawn ).value_or( 0 ) : 0;
				const auto pawn = pawn_h ? g_entities.lookup( pawn_h ) : 0;
				const auto hp = pawn && off_hp ? memory::safe_read<int>( pawn + off_hp ).value_or( -999 ) : -999;
				diag::writef( diag::level::warning,
					"[local-diag] ctl=0x%llx elist=0x%llx empty=%d off_pawn=%u off_hp=%u off_team=%u pawn_h=0x%x pawn=0x%llx hp=%d vm=0x%llx gv=0x%llx",
					(unsigned long long)ctl, (unsigned long long)elist, (int)g_entities.is_empty( ),
					(unsigned)off_pawn, (unsigned)off_hp, (unsigned)off_team,
					(unsigned)pawn_h, (unsigned long long)pawn, hp,
					(unsigned long long)addresses::globals::view_matrix,
					(unsigned long long)addresses::globals::global_vars );
			}
		}
		const auto local_player_controller = memory::safe_read<std::uintptr_t>( addresses::globals::local_player_controller ).value_or( 0 );
		if ( !local_player_controller )
		{
			this->reset( );
			return;
		}

		const auto publish_controller_only = [&] {
			snapshot s{};
			s.controller = local_player_controller;
			const auto team_offset = SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash );
			s.team = team_offset ? memory::safe_read<std::uint8_t>( local_player_controller + team_offset ).value_or( 0 ) : 0;
			s.view_team = s.team;
			this->reset( );
			std::unique_lock lock( this->m_mtx );
			this->m_snapshot = s;
		};

		const auto pawn_handle = memory::safe_read<std::uint32_t>( local_player_controller + SCHEMA( "CBasePlayerController", "m_hPawn"_hash ) ).value_or( 0 );
		if ( !pawn_handle || pawn_handle == 0xffffffff )
		{
			publish_controller_only( );
			return;
		}

		const auto pawn = g_entities.lookup( pawn_handle );
		if ( !pawn )
		{
			publish_controller_only( );
			return;
		}

		snapshot s{};
		s.controller = local_player_controller;
		s.pawn = pawn;
		s.team = memory::safe_read<std::uint8_t>( pawn + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) ).value_or( 0 );

		// m_hPawn can refer to an observer pawn during team selection. Its health
		// does not make it a CCSPlayerPawn: do not run alive-only features on it.
		const auto player_pawn_offset = SCHEMA( "CCSPlayerController", "m_hPlayerPawn"_hash );
		const auto player_pawn_handle = player_pawn_offset
			? memory::safe_read<std::uint32_t>( local_player_controller + player_pawn_offset ).value_or( 0 ) : 0;
		const auto health = memory::safe_read<int>( pawn + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) ).value_or( 0 );
		const auto life_state = memory::safe_read<std::uint8_t>( pawn + SCHEMA( "C_BaseEntity", "m_lifeState"_hash ) ).value_or( 1 );
		s.is_alive = player_pawn_handle != 0 && player_pawn_handle != 0xffffffff
			&& player_pawn_handle == pawn_handle && health > 0 && life_state == 0;

		if ( s.is_alive )
		{
			s.view_team = s.team;
		}
		else
		{
			const auto observer_pawn_handle = memory::safe_read<std::uint32_t>( local_player_controller + SCHEMA( "CCSPlayerController", "m_hObserverPawn"_hash ) ).value_or( 0 );
			if ( observer_pawn_handle && observer_pawn_handle != 0xffffffff )
			{
				const auto observer_pawn = g_entities.lookup( observer_pawn_handle );
				if ( observer_pawn )
				{
					const auto observer_services = memory::safe_read<std::uintptr_t>( observer_pawn + SCHEMA( "C_BasePlayerPawn", "m_pObserverServices"_hash ) ).value_or( 0 );
					if ( observer_services && ( observer_services >> 48 ) == 0 )
					{
						const auto observer_target_handle = memory::safe_read<std::uint32_t>( observer_services + SCHEMA( "CPlayer_ObserverServices", "m_hObserverTarget"_hash ) ).value_or( 0 );
						if ( observer_target_handle && observer_target_handle != 0xffffffff )
						{
							const auto observer_target = g_entities.lookup( observer_target_handle );
							if ( observer_target )
							{
								s.observer_pawn = observer_target;
								s.view_team = memory::safe_read<int>( observer_target + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) ).value_or( s.team );

								const auto observer_target_controller_handle = memory::safe_read<std::uint32_t>( observer_target + SCHEMA( "C_BasePlayerPawn", "m_hController"_hash ) ).value_or( 0 );
								if ( observer_target_controller_handle && observer_target_controller_handle != 0xffffffff )
								{
									const auto observer_target_controller = g_entities.lookup( observer_target_controller_handle );
									if ( observer_target_controller )
									{
										s.observer_controller = observer_target_controller;
									}
								}
							}
						}
					}
				}
			}

			if ( !s.observer_pawn )
			{
				s.view_team = s.team;
			}
		}

		{
			const auto game_type_cvar = CONVAR ("game_type");
			const auto game_mode_cvar = CONVAR ("game_mode");
			const auto game_type = game_type_cvar ? game_type_cvar->get<int> () : 0;
			const auto game_mode = game_mode_cvar ? game_mode_cvar->get<int>( ) : 0;
			const auto is_ffa = ( game_type == 1 && game_mode == 2 ) || ( game_type == 2 && game_mode == 0 );

			s.is_team_mode = !is_ffa;

			this->m_is_deathmatch.store( game_type == 1 && game_mode == 2 );
		}

		{
			const auto game_rules = memory::safe_read<std::uintptr_t>( addresses::globals::game_rules ).value_or( 0 );
			const auto global_vars = memory::safe_read<std::uintptr_t>( addresses::globals::global_vars ).value_or( 0 );

			auto cinematic{ false };
			auto freezetime{ false };

			if ( game_rules && global_vars )
			{
				if ( memory::safe_read<bool>( game_rules + SCHEMA( "C_CSGameRules", "m_bTeamIntroPeriod"_hash ) ).value_or( false ) )
				{
					cinematic = true;
				}
				else if ( memory::safe_read<int>( game_rules + SCHEMA( "C_CSGameRules", "m_gamePhase"_hash ) ).value_or( 0 ) >= 4 )
				{
					cinematic = true;
				}

				if ( memory::safe_read<bool>( game_rules + SCHEMA( "C_CSGameRules", "m_bFreezePeriod"_hash ) ).value_or( false ) )
				{
					freezetime = true;
				}
				else
				{
					const auto round_start_time = memory::safe_read<float>( game_rules + SCHEMA( "C_CSGameRules", "m_fRoundStartTime"_hash ) ).value_or( 0.0f );
					const auto current_time = memory::safe_read<float>( global_vars + 0x30 ).value_or( 0.0f );

					if ( round_start_time > current_time )
					{
						freezetime = true;
					}
				}
			}

			this->m_is_in_cinematic.store( cinematic );
			this->m_is_in_time_freeze.store( freezetime );
		}

		{
			std::unique_lock lock( this->m_mtx );
			this->m_snapshot = s;
		}
	}

	void local::reset( )
	{
		{
			std::unique_lock lock( this->m_mtx );
			this->m_snapshot = {};
		}

		this->m_is_deathmatch.store( false );
		this->m_is_in_cinematic.store( false );
		this->m_is_in_time_freeze.store( false );
	}

} // namespace systems
