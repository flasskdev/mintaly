#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>

#include "../movement.hpp"

namespace features::movement {

	namespace {

		[[nodiscard]] inline bool valid_runtime_pointer( std::uintptr_t address ) noexcept
		{
			return address >= 0x10000ull &&
				address != ( std::numeric_limits<std::uintptr_t>::max )( ) &&
				address <= 0x00007FFFFFFFFFFFull;
		}

		constexpr auto k_step_count{ 32 };
		constexpr auto k_min_speed{ 0.0001f };
		constexpr auto k_angle_epsilon{ 0.01f };
		constexpr auto k_button_epsilon{ 0.001f };
		constexpr auto k_rad_to_deg{ 180.0f / std::numbers::pi_v<float> };

		template <typename T>
		[[nodiscard]] std::optional<T> read_convar( c_convar* cvar )
		{
			if ( !cvar )
				return std::nullopt;
			return cvar->get<T>( );
		}

	} // namespace

	[[nodiscard]] bool test_strafer::is_active( ) const
	{
		return settings::g_movement.airstrafe.value || settings::g_movement.m_test_strafer.enabled.value;
	}

	bool test_strafer::emit_step( proto::base_usercmd_pb* base, float when, float yaw_delta ) const
	{
		if ( !base || !std::isfinite( when ) || !std::isfinite( yaw_delta ) )
			return false;

		const auto moves = base->mutable_subtick_moves( );
		if ( !moves )
			return false;

		const auto step = systems::g_input.acquire_subtick_step( moves );
		if ( !step || !valid_runtime_pointer( reinterpret_cast<std::uintptr_t>( step ) ) )
			return false;

		step->set_when( std::clamp( when, 0.0f, 0.999f ) );
		step->set_button( 0 );
		step->set_pressed( false );
		step->set_analog_forward_delta( 0.0f );
		step->set_analog_left_delta( 0.0f );
		step->set_pitch_delta( 0.0f );
		step->set_yaw_delta( yaw_delta );
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
			this->m_last_buttons = 0;
			this->m_last_pressed = 0;
			this->m_side_toggle = false;
			return;
		}
		if ( !cmd || features::movement::g_bhop.landing_ok_this_tick( ) ||
			features::movement::g_jumpbug.active_this_tick( ) )
			return;

		const auto base = cmd->csgo_user_cmd.has_base( ) ? cmd->csgo_user_cmd.mutable_base( ) : nullptr;
		if ( !base )
			return;

		const auto angles = base->viewangles( );
		if ( !angles )
			return;

		const auto local = systems::g_local.get( );
		if ( !local.pawn )
			return;

		if ( const auto move_type_offset = SCHEMA( "C_BaseEntity", "m_nActualMoveType"_hash ) )
		{
			const auto move_type = memory::safe_read<std::uint8_t>( local.pawn + move_type_offset );
			if ( move_type && ( *move_type == cstypes::move_type::ladder || *move_type == cstypes::move_type::noclip ) )
				return;
		}

		const auto buttons = cmd->buttons.value;
		this->check_button( buttons, cstypes::command_buttons::in_forward );
		this->check_button( buttons, cstypes::command_buttons::in_back );
		this->check_button( buttons, cstypes::command_buttons::in_moveleft );
		this->check_button( buttons, cstypes::command_buttons::in_moveright );
		this->m_last_buttons = buttons;

		const auto& prestate = systems::g_prediction.pre( );
		if ( prestate.flags & cstypes::entity_flags::on_ground )
			return;

		auto intent_forward = base->forwardmove( );
		auto intent_left = base->leftmove( );
		if ( !std::isfinite( intent_forward ) || !std::isfinite( intent_left ) )
			return;

		// Prefer analog axes, then reconstruct the full WASD direction from button
		// state for command paths that only populate button bits.
		if ( std::fabsf( intent_forward ) <= k_button_epsilon && std::fabsf( intent_left ) <= k_button_epsilon )
		{
			const bool forward_down = ( buttons & cstypes::command_buttons::in_forward ) != 0;
			const bool back_down = ( buttons & cstypes::command_buttons::in_back ) != 0;
			const bool left_down = ( buttons & cstypes::command_buttons::in_moveleft ) != 0;
			const bool right_down = ( buttons & cstypes::command_buttons::in_moveright ) != 0;

			if ( forward_down != back_down )
				intent_forward = forward_down ? 1.0f : -1.0f;
			else if ( forward_down && back_down )
				intent_forward = ( this->m_last_pressed & cstypes::command_buttons::in_forward ) ? 1.0f : -1.0f;

			if ( left_down != right_down )
				intent_left = left_down ? 1.0f : -1.0f;
			else if ( left_down && right_down )
				intent_left = ( this->m_last_pressed & cstypes::command_buttons::in_moveleft ) ? 1.0f : -1.0f;
		}

		if ( std::fabsf( intent_forward ) <= k_button_epsilon && std::fabsf( intent_left ) <= k_button_epsilon )
			return;

		const auto airaccelerate = read_convar<float>( CONVAR( "sv_airaccelerate" ) ).value_or( 12.0f );
		const auto air_max_wishspeed = read_convar<float>( CONVAR( "sv_air_max_wishspeed" ) ).value_or( 30.0f );
		const auto maxspeed_cvar = read_convar<float>( CONVAR( "sv_maxspeed" ) ).value_or( 320.0f );
		if ( !std::isfinite( airaccelerate ) || !std::isfinite( air_max_wishspeed ) || !std::isfinite( maxspeed_cvar ) ||
			airaccelerate <= 0.0f || air_max_wishspeed <= 0.0f || maxspeed_cvar <= 0.0f )
			return;

		auto player_maxspeed = maxspeed_cvar;
		if ( const auto movement_services_offset = SCHEMA( "C_BasePlayerPawn", "m_pMovementServices"_hash ) )
		{
			const auto movement_services = memory::safe_read<std::uintptr_t>( local.pawn + movement_services_offset ).value_or( 0 );
			if ( valid_runtime_pointer( movement_services ) )
			{
				if ( const auto maxspeed_offset = SCHEMA( "CPlayer_MovementServices", "m_flMaxspeed"_hash ) )
				{
					const auto maxspeed = memory::safe_read<float>( movement_services + maxspeed_offset );
					if ( maxspeed && std::isfinite( *maxspeed ) && *maxspeed > 0.0f )
						player_maxspeed = *maxspeed;
				}
			}
		}

		auto surface_friction = prestate.surface_friction;
		if ( !std::isfinite( surface_friction ) || surface_friction <= 0.0f )
			surface_friction = 1.0f;
		if ( surface_friction > 4.0f )
			return;

		const auto& velocity = prestate.networked_velocity;
		if ( !std::isfinite( velocity.x ) || !std::isfinite( velocity.y ) || !std::isfinite( velocity.z ) )
			return;

		const auto command_yaw = angles->y( );
		if ( !std::isfinite( command_yaw ) )
			return;

		const auto wish_yaw_offset = std::atan2f( intent_left, intent_forward ) * k_rad_to_deg;
		auto intent_yaw = command_yaw + wish_yaw_offset;
		math::helpers::normalize_angle( intent_yaw );

		const auto wishspeed = std::fminf( maxspeed_cvar, player_maxspeed );
		const auto capped_wishspeed = std::fminf( wishspeed, air_max_wishspeed );
		const auto accel_per_tick = airaccelerate * wishspeed * surface_friction;
		if ( !std::isfinite( wishspeed ) || !std::isfinite( capped_wishspeed ) ||
			!std::isfinite( accel_per_tick ) || wishspeed <= 0.0f || capped_wishspeed <= 0.0f || accel_per_tick <= 0.0f )
			return;

		const auto move_count = base->subtick_moves_size( );
		const auto step_count = std::clamp( k_step_count - 2 - move_count, 0, k_step_count );
		const auto start_when = systems::g_input.max_subtick_when( base );
		if ( step_count <= 0 || !std::isfinite( start_when ) || start_when >= 0.998f )
			return;

		// Reserve one slot for input::apply's base movement delta and one to put
		// the final view yaw back at its starting value after steering the tick.
		const auto when_step = ( 0.999f - start_when ) / static_cast<float>( step_count + 1 );
		const auto subtick_dt = cstypes::tick_interval * when_step;
		const auto accel_per_subtick = accel_per_tick * subtick_dt;
		if ( !std::isfinite( subtick_dt ) || subtick_dt <= 0.0f ||
			!std::isfinite( accel_per_subtick ) || accel_per_subtick <= 0.0f )
			return;

		auto sim_x = velocity.x;
		auto sim_y = velocity.y;
		auto accumulated_yaw = command_yaw;
		auto injected{ 0 };

		for ( auto i = 1; i <= step_count; ++i )
		{
			const auto speed = std::sqrtf( sim_x * sim_x + sim_y * sim_y );
			const auto velocity_yaw = std::atan2f( sim_y, sim_x ) * k_rad_to_deg;
			auto wish_yaw = intent_yaw;
			bool alternate_side = false;

			if ( speed > capped_wishspeed && speed > k_min_speed )
			{
				auto intent_delta = intent_yaw - velocity_yaw;
				math::helpers::normalize_angle( intent_delta );
				alternate_side = std::fabsf( intent_delta ) <= k_angle_epsilon ||
					std::fabsf( std::fabsf( intent_delta ) - 180.0f ) <= k_angle_epsilon;
				const auto side_sign = alternate_side ? ( this->m_side_toggle ? 1.0f : -1.0f ) : ( intent_delta > 0.0f ? 1.0f : -1.0f );

				const auto cos_theta = std::clamp( ( capped_wishspeed - accel_per_subtick ) / speed, -1.0f, 1.0f );
				const auto theta = std::acosf( cos_theta ) * k_rad_to_deg;
				wish_yaw = velocity_yaw + theta * side_sign;
			}

			math::helpers::normalize_angle( wish_yaw );
			auto target_view_yaw = wish_yaw - wish_yaw_offset;
			math::helpers::normalize_angle( target_view_yaw );
			auto yaw_delta = target_view_yaw - accumulated_yaw;
			math::helpers::normalize_angle( yaw_delta );

			if ( std::fabsf( yaw_delta ) > k_angle_epsilon )
			{
				if ( !this->emit_step( base, start_when + static_cast<float>( i ) * when_step, yaw_delta ) )
					break;
				accumulated_yaw = target_view_yaw;
				++injected;
			}

			const auto wish_rad = math::helpers::deg_to_rad( wish_yaw );
			const auto wish_x = std::cosf( wish_rad );
			const auto wish_y = std::sinf( wish_rad );
			const auto dot = sim_x * wish_x + sim_y * wish_y;
			const auto add_speed = std::clamp( capped_wishspeed - dot, 0.0f, accel_per_subtick );
			sim_x += wish_x * add_speed;
			sim_y += wish_y * add_speed;

			if ( alternate_side )
				this->m_side_toggle = !this->m_side_toggle;
		}

		if ( injected > 0 )
		{
			auto restore_delta = command_yaw - accumulated_yaw;
			math::helpers::normalize_angle( restore_delta );
			if ( std::fabsf( restore_delta ) > k_angle_epsilon && this->emit_step( base, 0.999f, restore_delta ) )
				++injected;
		}

		this->m_handled_this_tick = injected > 0;
	}

} // namespace features::movement
