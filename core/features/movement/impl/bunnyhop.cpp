#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>

#include "../movement.hpp"
#include <protection/game_addresses.hpp>

// Reverse engineered from skeet's bunnyhop:
//   FUN_18007f9b0 - landing prediction + timed duck/jump subtick sequence
//   FUN_18007d100 - "hold jump" button fix while sv_autobunnyhopping is disabled
namespace features::movement {

	namespace {

		// Constants read from the binary (skeet_payload.dll).
		constexpr auto k_landing_flat_normal{ 0.985f }; // DAT_1801bec80
		constexpr auto k_min_when{ 0.001f };            // DAT_1801b8740
		constexpr auto k_max_when{ 0.99f };             // DAT_1801beb48
		constexpr auto k_standing_height{ 72.0f };      // DAT_1801b8464
		constexpr auto k_trace_extra_drop{ 2.0f };      // DAT_1801b8a38

		struct landing_prediction
		{
			float when{};
			float normal_z{};
		};

		// skeet traces the hull along the predicted movement of the current tick
		// and returns the fraction at which the player would touch the ground.
		[[nodiscard]] std::optional<landing_prediction> predict_landing_fraction(
			std::uintptr_t local_pawn,
			std::uintptr_t movement_services,
			const systems::prediction::state& prestate,
			bool holding_duck )
		{
			if ( !valid_runtime_address( local_pawn ) || !valid_runtime_address( movement_services ) || prestate.networked_velocity.z > 0.0f )
			{
				return std::nullopt;
			}

			const auto duck_amount_offset = SCHEMA( "CCSPlayer_MovementServices", "m_flDuckAmount"_hash );
			const auto collision_offset = SCHEMA( "C_BaseModelEntity", "m_Collision"_hash );
			const auto mins_offset = SCHEMA( "CCollisionProperty", "m_vecMins"_hash );
			const auto maxs_offset = SCHEMA( "CCollisionProperty", "m_vecMaxs"_hash );
			if ( !duck_amount_offset || !collision_offset || !mins_offset || !maxs_offset )
				return std::nullopt;

			const auto duck_amount = memory::safe_read<float>( movement_services + duck_amount_offset ).value_or( 0.0f );
			const auto mins_value = memory::safe_read<math::vector3>( local_pawn + collision_offset + mins_offset );
			auto maxs_value = memory::safe_read<math::vector3>( local_pawn + collision_offset + maxs_offset );
			if ( !mins_value || !maxs_value )
				return std::nullopt;
			const auto mins = *mins_value;
			auto maxs = *maxs_value;

			auto trace_origin = prestate.networked_origin;
			if ( holding_duck && duck_amount > 0.0f )
			{
				const auto duck_hull_diff = k_standing_height - maxs.z;
				trace_origin.z -= duck_hull_diff * 0.5f;
				maxs.z = k_standing_height;
			}

			auto trace_mask{ 0ull };
			{
				const auto pawn_ptr = memory::safe_read<std::uintptr_t>( movement_services + 56 ).value_or( 0 );
				if ( !valid_runtime_address( pawn_ptr ) )
					return std::nullopt;

				trace_mask = memory::safe_read<std::uintptr_t>( pawn_ptr + 0xd48 ).value_or( 0 );

				if ( memory::safe_read<std::uint32_t>( pawn_ptr + 0x3f8 ).value_or( 0 ) & 0x10 )
				{
					trace_mask |= 0x20;
				}
			}

			const auto filter = systems::g_tracing.make_player_movement_filter( local_pawn, trace_mask, 11 );
			const auto gravity_cvar = CONVAR( "sv_gravity" );
			const auto standable_cvar = CONVAR( "sv_standable_normal" );
			if ( !gravity_cvar || !standable_cvar )
				return std::nullopt;
			const auto sv_gravity = gravity_cvar->get<float>( );
			const auto sv_standable_normal = standable_cvar->get<float>( );
			const auto gravity_scale_offset = SCHEMA( "C_BaseEntity", "m_flGravityScale"_hash );
			if ( !gravity_scale_offset )
				return std::nullopt;
			const auto gravity_scale = memory::safe_read<float>( local_pawn + gravity_scale_offset ).value_or( 1.0f );
			if ( !std::isfinite( sv_gravity ) || !std::isfinite( sv_standable_normal ) || sv_standable_normal <= 0.0f || sv_standable_normal > 1.0f )
				return std::nullopt;

			auto velocity = prestate.networked_velocity;
			velocity.z -= ( gravity_scale * sv_gravity * cstypes::tick_interval ) * 0.5f;

			const math::vector3 trace_start = trace_origin;
			math::vector3 trace_end{};

			trace_end.x = trace_origin.x + velocity.x * cstypes::tick_interval;
			trace_end.y = trace_origin.y + velocity.y * cstypes::tick_interval;
			trace_end.z = trace_origin.z + velocity.z * cstypes::tick_interval;
			trace_end.z -= k_trace_extra_drop;

			const auto result = systems::g_tracing.trace_player_bbox( trace_start, trace_end, { mins, maxs }, filter, movement_services );
			if ( result.fraction <= 0.0f || result.fraction >= 1.0f || result.normal.z < sv_standable_normal )
			{
				return std::nullopt;
			}

			// No 1/64 quantization here: skeet clamps the raw fraction into
			// [0.001, 0.99] and feeds the subtick steps with it.
			landing_prediction prediction{};
			prediction.when = std::clamp( result.fraction, k_min_when, k_max_when );
			prediction.normal_z = result.normal.z;
			return prediction;
		}

		// skeet emits four steps at the predicted landing fraction:
		// duck press at 0.0, duck release at the fraction, jump release at the
		// fraction and jump press at the fraction so the jump edge lands exactly
		// on the landing subtick.
		void apply_landing_steps( proto::base_usercmd_pb* base, float when )
		{
			if ( !base || !std::isfinite( when ) )
				return;

			const auto subtick_moves = base->mutable_subtick_moves( );
			if ( !subtick_moves )
				return;

			const auto add_step = [ & ]( std::uint64_t button, bool pressed, float step_when ) -> void
			{
				const auto step = systems::g_input.acquire_subtick_step( subtick_moves );
				if ( !step || !valid_runtime_address( reinterpret_cast<std::uintptr_t>( step ) ) )
					return;

				step->set_button( button );
				step->set_pressed( pressed );
				step->set_when( step_when );
				step->set_analog_forward_delta( 0.0f );
				step->set_analog_left_delta( 0.0f );
			};

			add_step( cstypes::command_buttons::in_duck, true, 0.0f );
			add_step( cstypes::command_buttons::in_duck, false, when );
			add_step( cstypes::command_buttons::in_jump, false, when );
			add_step( cstypes::command_buttons::in_jump, true, when );
		}

	} // namespace

	void bhop::on_create_move( systems::input::usercmd* cmd ) const
	{
		this->m_landing_ok = false;

		if ( !cmd || !settings::g_movement.bhop.value )
			return;

		if ( !( cmd->buttons.value & cstypes::command_buttons::in_jump ) )
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
		const bool on_ground = ( prestate.flags & cstypes::entity_flags::on_ground ) != 0;

		// Another landing feature owns the tick when the edgebug is about to fire.
		const auto& jb = settings::g_movement.jumpbug;
		const bool jb_active = ( jb.value && prestate.networked_velocity.z < -200.0f ) || ( jb.bind.key != 0 && jb.bind.active );
		if ( jb_active && prestate.networked_velocity.z < 0.0f )
			return;

		if ( on_ground )
		{
			this->m_landing_ok = false;
			return;
		}

		// Do not call the movement-service hull trace from the command hook. The
		// internal trace entry point changed with the latest client update and a
		// stale movement-services subobject can turn a harmless bunnyhop into an
		// access violation. The normal command path already performs the landing
		// test; only suppress the jump while airborne.
		cmd->buttons.value &= ~cstypes::command_buttons::in_jump;
		cmd->buttons.value_changed |= cstypes::command_buttons::in_jump;

		// part 2: while the auto bunnyhop is unavailable the jump button is
		// stripped in the air (and marked as changed on the ground) so the
		// landing jump always registers as a fresh press (skeet FUN_18007d100).
		const auto auto_bhop_cvar = CONVAR( "sv_autobunnyhopping" );
		if ( auto_bhop_cvar && auto_bhop_cvar->get<bool>( ) )
			return;

		if ( !on_ground )
		{
			cmd->buttons.value &= ~cstypes::command_buttons::in_jump;
			cmd->buttons.value_changed &= ~cstypes::command_buttons::in_jump;
		}
		else
		{
			cmd->buttons.value_changed |= cstypes::command_buttons::in_jump;
		}
	}

} // namespace features::movement
