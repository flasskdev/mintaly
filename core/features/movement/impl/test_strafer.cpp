#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>

// Reverse engineered from skeet's subtick strafer (FUN_18007be70).
//
// Per tick the strafer writes up to 32 subtick steps that only carry analog
// movement deltas. Every step steers the wish direction towards the ideal
// air-strafe angle, so the acceleration rotates inside a single tick instead
// of once per tick.
namespace features::movement {

	namespace {

		// Simple runtime address validation
		[[nodiscard]] inline bool valid_runtime_pointer( std::uintptr_t address ) noexcept
		{
			return address >= 0x10000ull &&
				address != ( std::numeric_limits<std::uintptr_t>::max )( ) &&
				address <= 0x00007FFFFFFFFFFFull;
		}

		// Constants read from the binary (skeet_payload.dll).
		constexpr auto k_step_count{ 32 };
		constexpr auto k_when_step{ 1.0f / 32.0f };    // DAT_1801be1a0
		constexpr auto k_subtick_dt{ 1.0f / 2048.0f }; // DAT_1801beac4
		constexpr auto k_min_speed{ 0.0001f };         // DAT_1801b861c
		constexpr auto k_angle_epsilon{ 0.01f };       // DAT_1801b7060
		constexpr auto k_dot_epsilon{ -0.0001f };      // DAT_1801beacc
		constexpr auto k_score_epsilon{ -0.001f };     // DAT_1801bead0
		constexpr auto k_button_epsilon{ 0.001f };     // DAT_1801b8740
		constexpr auto k_rad_to_deg{ 180.0f / std::numbers::pi_v<float> };

		constexpr auto k_intent_left{ 90.0f };       // DAT_1801b86d8
		constexpr auto k_intent_right{ -90.0f };     // DAT_1801b86d4
		constexpr auto k_intent_forward{ 0.5f };     // DAT_1801b8404
		constexpr auto k_intent_back_mul{ -0.5f };   // DAT_1801b862c
		constexpr auto k_intent_back_add{ 180.0f };  // DAT_1801b8648

		template <typename T>
		[[nodiscard]] std::optional<T> read_convar( c_convar* cvar )
		{
			if ( !cvar )
				return std::nullopt;
			return cvar->get<T>( );
		}

		[[nodiscard]] float wrap_degrees( float value )
		{
			return std::fmodf( value, 360.0f );
		}

	} // namespace

	[[nodiscard]] bool test_strafer::is_active( ) const
	{
		return settings::g_movement.airstrafe.value || settings::g_movement.m_test_strafer.enabled.value;
	}

	bool test_strafer::emit_step( proto::base_usercmd_pb* base, float when, float forward_delta, float left_delta ) const
	{
		if ( !base || !std::isfinite( when ) || !std::isfinite( forward_delta ) || !std::isfinite( left_delta ) )
			return false;

		const auto subtick_moves = base->mutable_subtick_moves( );
		if ( !subtick_moves )
			return false;

		const auto step = systems::g_input.acquire_subtick_step( subtick_moves );
		if ( !step || !valid_runtime_address( reinterpret_cast<std::uintptr_t>( step ) ) )
			return false;

		step->set_when( when );
		step->set_button( 0 );
		step->set_pressed( false );
		step->set_analog_forward_delta( forward_delta );
		step->set_analog_left_delta( left_delta );
		step->set_pitch_delta( 0.0f );
		step->set_yaw_delta( 0.0f );

		return true;
	}

	void test_strafer::check_button( std::uintptr_t current_buttons, std::uintptr_t button )
	{
		constexpr auto moveleft = static_cast<std::uintptr_t>( cstypes::command_buttons::in_moveleft );
		constexpr auto moveright = static_cast<std::uintptr_t>( cstypes::command_buttons::in_moveright );
		constexpr auto forward = static_cast<std::uintptr_t>( cstypes::command_buttons::in_forward );
		constexpr auto back = static_cast<std::uintptr_t>( cstypes::command_buttons::in_back );

		if ( current_buttons & button && ( !( this->m_last_buttons & button ) ||
			( button & moveleft && !( this->m_last_pressed & moveright ) ) ||
			( button & moveright && !( this->m_last_pressed & moveleft ) ) ||
			( button & forward && !( this->m_last_pressed & back ) ) ||
			( button & back && !( this->m_last_pressed & forward ) ) ) )
		{
			if ( button & moveleft )
				this->m_last_pressed &= ~moveright;
			else if ( button & moveright )
				this->m_last_pressed &= ~moveleft;
			else if ( button & forward )
				this->m_last_pressed &= ~back;
			else if ( button & back )
				this->m_last_pressed &= ~forward;

			this->m_last_pressed |= button;
		}
		else if ( !( current_buttons & button ) )
		{
			this->m_last_pressed &= ~button;
		}
	}

	void test_strafer::on_create_move( systems::input::usercmd* cmd )
	{
		this->m_handled_this_tick = false;

		if ( !this->is_active( ) )
		{
			// skeet resets the tracked state while the feature is disabled.
			this->m_last_buttons = 0;
			this->m_last_pressed = 0;
			this->m_side_toggle = false;
			return;
		}

		if ( !cmd )
			return;

		// A predicted landing jump already owns the subticks of this tick.
		if ( features::movement::g_bhop.landing_ok_this_tick( ) )
			return;

		if ( features::movement::g_jumpbug.active_this_tick( ) )
			return;

		const auto base = cmd->csgo_user_cmd.has_base( ) ? cmd->csgo_user_cmd.mutable_base( ) : nullptr;
		if ( !base || !valid_runtime_pointer( reinterpret_cast<std::uintptr_t>( base ) ) )
			return;

		const auto angles = base->viewangles( );
		if ( !angles || !valid_runtime_pointer( reinterpret_cast<std::uintptr_t>( angles ) ) )
			return;

		const auto local = systems::g_local.get( );
		if ( !local.pawn )
			return;

		const auto move_type_offset = SCHEMA( "C_BaseEntity", "m_nActualMoveType"_hash );
		if ( !move_type_offset )
			return;

		const auto move_type = memory::safe_read<std::uint8_t>( local.pawn + move_type_offset ).value_or( 0 );
		if ( move_type == cstypes::move_type::ladder || move_type == cstypes::move_type::noclip )
			return;

		const auto& prestate = systems::g_prediction.pre( );
		if ( prestate.flags & cstypes::entity_flags::on_ground )
			return;

		if ( features::combat::g_rage.is_firing_this_tick( ) )
			return;

		const auto buttons = cmd->buttons.value;
		this->check_button( buttons, cstypes::command_buttons::in_forward );
		this->check_button( buttons, cstypes::command_buttons::in_back );
		this->check_button( buttons, cstypes::command_buttons::in_moveleft );
		this->check_button( buttons, cstypes::command_buttons::in_moveright );
		this->m_last_buttons = buttons;

		const auto movement_mask = static_cast< std::uintptr_t >(
			cstypes::command_buttons::in_forward | cstypes::command_buttons::in_back |
			cstypes::command_buttons::in_moveleft | cstypes::command_buttons::in_moveright );

		if ( !( buttons & cstypes::command_buttons::in_sprint ) && !( this->m_last_pressed & movement_mask ) )
			return;

		// NOTE: do NOT touch subtick_moves->m_current_size here. The repeated
		// field may sit inside an unmapped/proxyed command copy while the game
		// rebuilds the message, which is exactly the sporadic crash we chased.
		// emit_step() acquires through the validated allocation path instead.
		const auto airaccelerate = read_convar<float>( CONVAR( "sv_airaccelerate" ) );
		const auto air_max_wishspeed = read_convar<float>( CONVAR( "sv_air_max_wishspeed" ) );
		const auto maxspeed_cvar = read_convar<float>( CONVAR( "sv_maxspeed" ) );
		if ( !airaccelerate || !air_max_wishspeed || !maxspeed_cvar ||
			!std::isfinite( *airaccelerate ) || !std::isfinite( *air_max_wishspeed ) || !std::isfinite( *maxspeed_cvar ) ||
			*airaccelerate < 0.0f || *air_max_wishspeed <= 0.0f || *maxspeed_cvar <= 0.0f )
			return;

		const auto movement_services_offset = SCHEMA( "C_BasePlayerPawn", "m_pMovementServices"_hash );
		const auto movement_services = movement_services_offset
			? memory::safe_read<std::uintptr_t>( local.pawn + movement_services_offset ).value_or( 0 )
			: 0;
		if ( !valid_runtime_address( movement_services ) )
			return;

		const auto maxspeed_offset = SCHEMA( "CPlayer_MovementServices", "m_flMaxspeed"_hash );
		if ( !maxspeed_offset )
			return;

		const auto player_maxspeed = memory::safe_read<float>( movement_services + maxspeed_offset ).value_or( 0.0f );
		if ( !std::isfinite( player_maxspeed ) || player_maxspeed <= 0.0f )
			return;

		const auto surface_friction = prestate.surface_friction;
		if ( !std::isfinite( surface_friction ) || surface_friction < 0.0f )
			return;

		const auto& velocity = prestate.networked_velocity;
		if ( !std::isfinite( velocity.x ) || !std::isfinite( velocity.y ) || !std::isfinite( velocity.z ) )
			return;

		const auto view_angles = systems::g_input.get_view_angles( );
		const auto state_yaw = view_angles.y;  // player yaw as captured before the antiaim
		const auto command_yaw = angles->y( ); // yaw that will be serialized
		if ( !std::isfinite( state_yaw ) || !std::isfinite( command_yaw ) )
			return;

		// Movement intent (held keys) -> target world yaw.
		auto intent_offset{ 0.0f };
		if ( this->m_last_pressed & cstypes::command_buttons::in_moveleft )
			intent_offset = k_intent_left;
		if ( this->m_last_pressed & cstypes::command_buttons::in_moveright )
			intent_offset += k_intent_right;
		if ( this->m_last_pressed & cstypes::command_buttons::in_forward )
			intent_offset *= k_intent_forward;
		else if ( this->m_last_pressed & cstypes::command_buttons::in_back )
			intent_offset = intent_offset * k_intent_back_mul + k_intent_back_add;

		auto intent_yaw = state_yaw + intent_offset;
		math::helpers::normalize_angle( intent_yaw );

		const bool sprinting = ( buttons & cstypes::command_buttons::in_sprint ) != 0;
		const bool quantize_mode = [ & ]
		{
			const auto quantize_cvar = CONVAR( "sv_quantize_movement_input" );
			const bool quantized = quantize_cvar ? quantize_cvar->get<bool>( ) : true;
			return quantized && settings::g_movement.airstrafe_fully_directional.value;
		}( );

		const auto wishspeed = std::fminf( *maxspeed_cvar, player_maxspeed );
		const auto capped_wishspeed = std::fminf( wishspeed, *air_max_wishspeed );
		const auto accel_per_subtick = *airaccelerate * wishspeed * surface_friction * k_subtick_dt;

		auto sim_x = velocity.x;
		auto sim_y = velocity.y;
		auto previous_forward{ 0.0f };
		auto previous_left{ 0.0f };
		auto injected{ 0 };

		for ( auto i = 0; i < k_step_count; ++i )
		{
			const auto saved_forward = previous_forward;
			const auto saved_left = previous_left;

			const auto speed = std::sqrtf( sim_x * sim_x + sim_y * sim_y );
			const auto velocity_yaw = std::atan2f( sim_y, sim_x ) * k_rad_to_deg;

			auto reference_yaw = intent_yaw;
			auto scale{ 1.0f };

			if ( !sprinting )
			{
				if ( speed > k_min_speed )
				{
					auto intent_delta = intent_yaw - velocity_yaw;
					math::helpers::normalize_angle( intent_delta );

					// Without a clear side skeet alternates the strafe direction
					// every subtick (W-style strafing).
					const auto side = ( std::fabsf( intent_delta ) > k_angle_epsilon ) ? ( intent_delta > 0.0f ) : this->m_side_toggle;
					const auto side_sign = side ? 1.0f : -1.0f;

					// Ideal air-strafe angle (skeet FUN_18007bd20).
					auto cos_theta = ( capped_wishspeed - capped_wishspeed * *airaccelerate * surface_friction * k_subtick_dt ) / speed;
					cos_theta = std::clamp( cos_theta, 0.0f, 1.0f );
					const auto theta = std::acosf( cos_theta ) * k_rad_to_deg;

					reference_yaw = velocity_yaw + theta * side_sign;
					scale = 1.0f;
				}
			}
			else
			{
				// +sprint in the air steers against the velocity while the
				// magnitude follows the current speed.
				reference_yaw = velocity_yaw + k_intent_back_add;
				const auto brake_divisor = std::fmaxf( accel_per_subtick, wishspeed );
				scale = std::clamp( speed / brake_divisor, 0.0f, 1.0f );
			}

			const auto reference_rad = math::helpers::deg_to_rad( reference_yaw - command_yaw );
			auto step_forward = std::cosf( reference_rad ) * scale;
			auto step_left = std::sinf( reference_rad ) * scale;
			auto step_yaw = reference_yaw;

			if ( quantize_mode && scale > 0.0f )
			{
				// Fully directional mode: the servers quantization snaps the
				// analog input to 8 directions, so the strafer scores every
				// candidate itself and picks the best speed gain.
				auto best_score = sprinting ? 0.0f : -std::numeric_limits<float>::infinity( );
				auto best_forward{ 0.0f };
				auto best_left{ 0.0f };
				auto best_yaw = reference_yaw;

				for ( auto axis_x = -1; axis_x <= 1; ++axis_x )
				{
					for ( auto axis_y = -1; axis_y <= 1; ++axis_y )
					{
						if ( axis_x == 0 && axis_y == 0 )
							continue;

						const auto candidate_yaw = command_yaw + std::atan2f( static_cast< float >( axis_y ), static_cast< float >( axis_x ) ) * k_rad_to_deg;
						const auto candidate_rad = math::helpers::deg_to_rad( candidate_yaw );
						const auto wish_x = std::cosf( candidate_rad );
						const auto wish_y = std::sinf( candidate_rad );

						const auto dot = sim_x * wish_x + sim_y * wish_y;
						if ( !sprinting && dot < k_dot_epsilon )
							continue;

						const auto add_speed = std::clamp( capped_wishspeed - dot, 0.0f, accel_per_subtick );
						const auto new_x = sim_x + wish_x * add_speed;
						const auto new_y = sim_y + wish_y * add_speed;

						auto score = ( new_x * new_x + new_y * new_y ) - speed * speed;
						if ( sprinting )
							score = -score;

						score += std::fabsf( wrap_degrees( candidate_yaw - reference_yaw ) ) * k_score_epsilon;

						if ( best_score < score )
						{
							best_score = score;
							best_forward = static_cast< float >( axis_x );
							best_left = static_cast< float >( axis_y );
							best_yaw = candidate_yaw;
						}
					}
				}

				step_forward = best_forward;
				step_left = best_left;
				step_yaw = best_yaw;
			}

			if ( !this->emit_step( base, static_cast< float >( i ) * k_when_step, step_forward - saved_forward, step_left - saved_left ) )
				break;

			previous_forward = step_forward;
			previous_left = step_left;
			++injected;

			const bool simulate = quantize_mode ? ( step_forward != 0.0f || step_left != 0.0f ) : ( scale > 0.0f );
			if ( simulate )
			{
				const auto sim_scale = quantize_mode ? 1.0f : scale;
				const auto sim_wishspeed = sim_scale * wishspeed;
				const auto sim_cap = std::fminf( sim_wishspeed, *air_max_wishspeed );
				const auto sim_accel = *airaccelerate * sim_wishspeed * surface_friction * k_subtick_dt;

				const auto step_rad = math::helpers::deg_to_rad( step_yaw );
				const auto wish_x = std::cosf( step_rad );
				const auto wish_y = std::sinf( step_rad );

				const auto dot = sim_x * wish_x + sim_y * wish_y;
				const auto add_speed = std::clamp( sim_cap - dot, 0.0f, sim_accel );
				sim_x += wish_x * add_speed;
				sim_y += wish_y * add_speed;
			}

			this->m_side_toggle = !this->m_side_toggle;
		}

		if ( injected == 0 )
			return;

		// All movement of this tick lives in the subtick analog deltas now.
		base->set_forwardmove( 0.0f );
		base->set_leftmove( 0.0f );
		this->m_handled_this_tick = true;

		// skeet "final subtick" pass: rebuild the movement buttons from the
		// analog values so the quantized (server side) input stays consistent.
		if ( quantize_mode )
		{
			const auto forward = base->forwardmove( );
			const auto left = base->leftmove( );

			auto rebuilt = buttons & ~movement_mask;
			if ( forward > k_button_epsilon )
				rebuilt |= cstypes::command_buttons::in_forward;
			else if ( forward < -k_button_epsilon )
				rebuilt |= cstypes::command_buttons::in_back;

			if ( left > k_button_epsilon )
				rebuilt |= cstypes::command_buttons::in_moveright;
			else if ( left < -k_button_epsilon )
				rebuilt |= cstypes::command_buttons::in_moveleft;

			cmd->buttons.value = rebuilt;
			cmd->buttons.value_changed = ( cmd->buttons.value_changed & ~movement_mask ) | ( ( rebuilt ^ buttons ) & movement_mask );
		}
	}

} // namespace features::movement
