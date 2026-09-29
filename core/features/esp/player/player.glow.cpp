#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>

namespace features::esp::player {
	namespace {
		[[nodiscard]] bool is_preview_player_hash( std::uint32_t owner_hash )
		{
			// The custom MapPlayerPreviewPanel uses the base class. The alias and
			// TeamPreviewModel classes belong to the lobby and must keep native glow.
			return owner_hash == "C_CSGO_PreviewPlayer"_hash;
		}
	}

	bool glow::on_is_glowing( std::uintptr_t owner_entity, std::uint32_t owner_hash ) const
	{
		const bool is_preview_player = is_preview_player_hash( owner_hash );
		if ( owner_hash != "C_CSPlayerPawn"_hash && !is_preview_player )
		{
			return false;
		}
		if ( is_preview_player && !systems::g_model_preview.is_agent_preview_entity( owner_entity ) )
			return false;

		const auto& cfg = settings::g_esp.m_player.m_glow;
		if ( !cfg.enemy.enabled.value && !cfg.enemy_ragdoll.enabled.value && !cfg.team.enabled.value && !cfg.team_ragdoll.enabled.value && !cfg.local.enabled.value && !cfg.local_ragdoll.enabled.value )
		{
			return false;
		}

		const auto local = systems::g_local.get( );
		if ( !is_preview_player && !local.is_alive && !local.observer_pawn )
			return false;
		const auto team = is_preview_player ? 0 :
			memory::read<std::uint8_t>( owner_entity + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) );
		const auto health = is_preview_player ? 100 :
			memory::read<int>( owner_entity + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) );

		const auto is_other_team = is_preview_player
			? systems::g_model_preview.is_enemy_preview( local.team )
			: local.is_this_other_team( team );
		const auto is_local = !is_preview_player && owner_entity == local.view_pawn( );
		const auto is_dead = !is_preview_player && health <= 0;

		if ( is_dead )
		{
			if ( is_local )
			{
				return cfg.local_ragdoll.enabled.value;
			}

			if ( is_other_team )
			{
				return cfg.enemy_ragdoll.enabled.value;
			}

			return cfg.team_ragdoll.enabled.value;
		}

		if ( is_local && ( settings::g_misc.m_camera.thirdperson.value || features::misc::g_camera.is_freecam_active( ) ) )
		{
			return cfg.local.enabled.value;
		}

		if ( is_other_team )
		{
			return cfg.enemy.enabled.value;
		}

		return cfg.team.enabled.value;
	}

	bool glow::on_get_glow_color( std::uintptr_t owner_entity, std::uint32_t owner_hash, float* color ) const
	{
		const bool is_preview_player = is_preview_player_hash( owner_hash );
		if ( owner_hash != "C_CSPlayerPawn"_hash && !is_preview_player )
		{
			return false;
		}
		if ( is_preview_player && !systems::g_model_preview.is_agent_preview_entity( owner_entity ) )
			return false;

		const auto& cfg = settings::g_esp.m_player.m_glow;
		if ( !cfg.enemy.enabled.value && !cfg.enemy_ragdoll.enabled.value && !cfg.team.enabled.value && !cfg.team_ragdoll.enabled.value && !cfg.local.enabled.value && !cfg.local_ragdoll.enabled.value )
		{
			return false;
		}

		const auto local = systems::g_local.get( );
		if ( !is_preview_player && !local.is_alive && !local.observer_pawn )
			return false;
		const auto team = is_preview_player ? 0 :
			memory::read<std::uint8_t>( owner_entity + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) );
		const auto health = is_preview_player ? 100 :
			memory::read<int>( owner_entity + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) );

		const auto is_other_team = is_preview_player
			? systems::g_model_preview.is_enemy_preview( local.team )
			: local.is_this_other_team( team );
		const auto is_local = !is_preview_player && owner_entity == local.view_pawn( );
		const auto is_dead = !is_preview_player && health <= 0;

		const settings::esp::glow_target* target{ nullptr };

		if ( is_dead )
		{
			if ( is_local )
			{
				target = &cfg.local_ragdoll;
			}
			else if ( is_other_team )
			{
				target = &cfg.enemy_ragdoll;
			}
			else
			{
				target = &cfg.team_ragdoll;
			}
		}
		else
		{
			if ( is_local )
			{
				if ( settings::g_misc.m_camera.thirdperson.value || features::misc::g_camera.is_freecam_active( ) )
				{
					target = &cfg.local;
				}
			}
			else if ( is_other_team )
			{
				target = &cfg.enemy;
			}
			else
			{
				target = &cfg.team;
			}
		}

		if ( !target || !target->enabled.value )
		{
			return false;
		}

		color[ 0 ] = target->color.value.r / 255.0f;
		color[ 1 ] = target->color.value.g / 255.0f;
		color[ 2 ] = target->color.value.b / 255.0f;
		color[ 3 ] = target->color.value.a / 255.0f;

		return true;
	}

} // namespace features::esp::player
