#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>

#include "../movement.hpp"

namespace features::movement {

	namespace {

		[[nodiscard]] bool predict_landing_jump( systems::input::usercmd* cmd, std::uintptr_t pawn,
			const systems::prediction::state& prestate, std::uintptr_t jump )
		{
			if ( !cmd || !valid_runtime_address( pawn ) || !prestate.movement_valid || prestate.pawn != pawn ||
				(prestate.flags & cstypes::entity_flags::on_ground) || prestate.networked_velocity.z >= 0.0f )
				return false;

			const auto dt = cstypes::tick_interval;
			const auto gravity_scale = prestate.gravity_scale;
			if ( !std::isfinite( dt ) || dt <= 0.0f || !std::isfinite( gravity_scale ) || gravity_scale <= 0.0f )
				return false;

			const auto gravity_cvar = CONVAR( "sv_gravity" );
			const auto server_gravity = gravity_cvar ? gravity_cvar->get<float>( ) : 800.0f;
			const auto gravity = server_gravity * gravity_scale;
			if ( !std::isfinite( gravity ) || gravity <= 0.0f )
				return false;

			const auto origin = prestate.networked_origin;
			const auto velocity = prestate.networked_velocity;
			const auto mins = prestate.collision_mins;
			const auto maxs = prestate.collision_maxs;
			if ( !std::isfinite( origin.x ) || !std::isfinite( origin.y ) || !std::isfinite( origin.z ) ||
				!std::isfinite( velocity.x ) || !std::isfinite( velocity.y ) || !std::isfinite( velocity.z ) ||
				!std::isfinite( mins.x ) || !std::isfinite( mins.y ) || !std::isfinite( mins.z ) ||
				!std::isfinite( maxs.x ) || !std::isfinite( maxs.y ) || !std::isfinite( maxs.z ) ||
				maxs.x <= mins.x || maxs.y <= mins.y || maxs.z <= mins.z )
				return false;

			const auto movement_services_offset = SCHEMA( "C_BasePlayerPawn", "m_pMovementServices"_hash );
			if ( !movement_services_offset )
				return false;
			const auto movement_services = memory::safe_read<std::uintptr_t>( pawn + movement_services_offset ).value_or( 0 );
			if ( !valid_runtime_address( movement_services ) )
				return false;

			const auto movement_pawn = memory::safe_read<std::uintptr_t>( movement_services + 56 ).value_or( 0 );
			if ( !valid_runtime_address( movement_pawn ) )
				return false;

			auto mask = memory::safe_read<std::uint64_t>( movement_pawn + 0xd48 ).value_or( 0 );
			const auto collision_flags = memory::safe_read<std::uint32_t>( movement_pawn + 0x3f8 ).value_or( 0 );
			if ( collision_flags & 0x10 )
				mask |= 0x20;
			if ( !mask )
				mask = 0x1c3003;

			const auto filter = systems::g_tracing.make_player_movement_filter( pawn, mask, 11 );
			const auto filter_word = memory::safe_read<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( filter.data ) ).value_or( 0 );
			if ( !valid_runtime_address( filter_word ) )
				return false;

			auto end = origin + velocity * dt;
			end.z -= 0.5f * gravity * dt * dt;
			const auto hit = systems::g_tracing.trace_player_bbox( origin, end, { mins, maxs }, filter, movement_services );
			if ( hit.all_solid || !std::isfinite( hit.fraction ) || hit.fraction <= 0.0f || hit.fraction >= 1.0f ||
				!std::isfinite( hit.normal.z ) || hit.normal.z < 0.7f )
				return false;

			const auto base = cmd->csgo_user_cmd.has_base( ) ? cmd->csgo_user_cmd.mutable_base( ) : nullptr;
			const auto moves = base ? base->mutable_subtick_moves( ) : nullptr;
			const auto step = systems::g_input.acquire_subtick_step( moves );
			if ( !step || !valid_runtime_address( reinterpret_cast<std::uintptr_t>( step ) ) )
				return false;

			// Fire just after predicted contact so the engine has categorized the
			// player as grounded when it consumes the jump edge.
			step->set_button( jump );
			step->set_pressed( true );
			step->set_when( std::clamp( hit.fraction + 0.002f, 0.0f, 0.999f ) );
			step->set_analog_forward_delta( 0.0f );
			step->set_analog_left_delta( 0.0f );
			step->set_pitch_delta( 0.0f );
			step->set_yaw_delta( 0.0f );
			cmd->buttons.value |= jump;
			cmd->buttons.value_changed |= jump;
			return true;
		}

	} // namespace

	void bhop::on_create_move( systems::input::usercmd* cmd ) const
	{
		this->m_landing_ok = false;

		constexpr auto jump = cstypes::command_buttons::in_jump;
		if ( !cmd || !settings::g_movement.bhop.value || !( cmd->buttons.value & jump ) )
			return;

		const auto local = systems::g_local.get( );
		if ( !local.pawn )
			return;

		// If the schema entry is unavailable, keep the normal jump behavior. Only
		// skip movement types that we can positively identify as non-walk modes.
		if ( const auto move_type_offset = SCHEMA( "C_BaseEntity", "m_nActualMoveType"_hash ) )
		{
			const auto move_type = memory::safe_read<std::uint8_t>( local.pawn + move_type_offset );
			if ( move_type && ( *move_type == cstypes::move_type::ladder || *move_type == cstypes::move_type::noclip ) )
				return;
		}

		const auto& prestate = systems::g_prediction.pre( );
		const bool on_ground = ( prestate.flags & cstypes::entity_flags::on_ground ) != 0;

		// Jumpbug owns the falling edge when it is armed. Let it keep the jump
		// state for this command rather than fighting over the same button.
		const auto& jumpbug = settings::g_movement.jumpbug;
		const bool jumpbug_active =
			( jumpbug.value && prestate.networked_velocity.z < -200.0f ) ||
			( jumpbug.bind.key != 0 && jumpbug.bind.active );
		if ( jumpbug_active && prestate.networked_velocity.z < 0.0f )
			return;

		if ( on_ground )
		{
			// The physical key is still held. Mark it as a fresh press so landing
			// on this command immediately starts the next hop.
			cmd->buttons.value |= jump;
			cmd->buttons.value_changed |= jump;
			return;
		}

		// Release jump for every airborne command. The ground branch above then
		// turns the held key into a new press at the first grounded command.
		cmd->buttons.value &= ~jump;
		cmd->buttons.value_changed |= jump;

		// If this command also contains the landing, schedule the next jump at
		// contact time. Otherwise the release above is the safe fallback and the
		// grounded branch will jump on the next command.
		this->m_landing_ok = predict_landing_jump( cmd, local.pawn, prestate, jump );
	}

} // namespace features::movement
