#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <algorithm>
#include <cmath>
#include "../movement.hpp"
#include <protection/game_addresses.hpp>

namespace features::movement {
    namespace {
        constexpr float standing_height = 72.0f;

        bool finite(const math::vector3& v) {
            return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
        }
    }

    void jumpbug::on_create_move(systems::input::usercmd* cmd, std::uint64_t original_buttons) {
        this->m_active_this_tick = false;
        const bool fired_previous = this->m_fired_last_tick;
        this->m_fired_last_tick = false;

        if (!cmd) return;

        const auto base = cmd->csgo_user_cmd.mutable_base();
        const auto moves = base ? base->mutable_subtick_moves() : nullptr;

        constexpr auto jump = cstypes::command_buttons::in_jump;
        constexpr auto duck = cstypes::command_buttons::in_duck;
        constexpr auto controlled = jump | duck;

        // Release jump from previous tick if user isn't holding jump manually
        if (fired_previous && !(original_buttons & jump)) {
            cmd->buttons.value &= ~jump;
            cmd->buttons.value_changed |= jump;
            cmd->buttons.value_scroll &= ~jump;
            if (moves) {
                if (const auto step = systems::g_input.acquire_subtick_step(moves)) {
                    step->set_button(jump);
                    step->set_pressed(false);
                    step->set_when(0.0f);
                    step->set_analog_forward_delta(0.0f);
                    step->set_analog_left_delta(0.0f);
                }
            }
        }

        const auto& config = settings::g_movement.jumpbug;
        const bool is_bound = (config.bind.key != 0 && config.bind.active);
        const bool is_enabled = config.value;
        if (!is_bound && !is_enabled) return;

        const auto local = systems::g_local.get();
        const auto& pre = systems::g_prediction.pre();
        if (!local.pawn || !local.is_alive || !pre.movement_valid || pre.pawn != local.pawn) return;

        const auto move_type = memory::read<std::uint8_t>(local.pawn + SCHEMA("C_BaseEntity", "m_nActualMoveType"_hash));
        if (move_type == cstypes::move_type::ladder || move_type == cstypes::move_type::noclip) return;

        // Must be in the air
        if (pre.flags & cstypes::entity_flags::on_ground) return;

        // Must be falling downwards
        if (!finite(pre.networked_origin) || !finite(pre.networked_velocity) || pre.networked_velocity.z >= 0.0f) return;

        // If only toggled without keybind held, auto-save as soon as falling downwards
        if (!is_bound && pre.networked_velocity.z > -200.0f) return;

        const auto movement = memory::read<std::uintptr_t>(local.pawn + SCHEMA("C_BasePlayerPawn", "m_pMovementServices"_hash));
        if (!movement || !PATTERN(patterns::trace_hull) || !PATTERN(patterns::trace_filter_set_collision)) return;

        const auto pawn = memory::safe_read<std::uintptr_t>(movement + 56).value_or(0);
        if (!pawn) return;

        auto mask = memory::read<std::uint64_t>(pawn + 0xd48);
        if (memory::read<std::uint32_t>(pawn + 0x3f8) & 0x10) mask |= 0x20;

        const auto gravity_var = CONVAR("sv_gravity");
        const auto normal_var = CONVAR("sv_standable_normal");
        if (!gravity_var || !normal_var) return;

        const float dt = cstypes::tick_interval;
        const float gravity_scale = pre.gravity_scale > 0.0f ? pre.gravity_scale : 1.0f;
        const float gravity = gravity_var->get<float>() * gravity_scale;
        const float standable = normal_var->get<float>();

        if (!std::isfinite(dt) || dt <= 0.0f || !std::isfinite(gravity) || gravity < 0.0f ||
            !std::isfinite(standable) || standable <= 0.0f || standable > 1.0f) return;

        const auto collision = local.pawn + SCHEMA("C_BaseModelEntity", "m_Collision"_hash);
        const auto mins = memory::read<math::vector3>(collision + SCHEMA("CCollisionProperty", "m_vecMins"_hash));
        auto maxs = memory::read<math::vector3>(collision + SCHEMA("CCollisionProperty", "m_vecMaxs"_hash));
        if (!finite(mins) || !finite(maxs)) return;

        const auto duck_amount = memory::read<float>(movement + SCHEMA("CCSPlayer_MovementServices", "m_flDuckAmount"_hash));
        const bool is_crouching = (cmd->buttons.value & duck) != 0 || (duck_amount > 0.0f);

        // Account for airborne unduck hull expansion:
        // Unducking expands the hull from crouch height to standing height (72.0f).
        // In the air, hull expansion preserves the center, extending the feet downwards by half the difference (9 units).
        auto trace_origin = pre.networked_origin;
        if (is_crouching && duck_amount > 0.0f) {
            const float duck_hull_diff = std::max(0.0f, standing_height - maxs.z);
            trace_origin.z -= duck_hull_diff * 0.5f;
            maxs.z = standing_height;
        }

        auto velocity = pre.networked_velocity;
        velocity.z -= (gravity * dt) * 0.5f;

        const auto filter = systems::g_tracing.make_player_movement_filter(local.pawn, mask, 11);

        const math::vector3 trace_start = trace_origin;
        math::vector3 trace_end{};
        trace_end.x = trace_start.x + velocity.x * dt;
        trace_end.y = trace_start.y + velocity.y * dt;
        trace_end.z = trace_start.z + velocity.z * dt - 2.0f;

        const auto result = systems::g_tracing.trace_player_bbox(trace_start, trace_end, { mins, maxs }, filter, movement);

        const bool landing_this_tick = !result.all_solid &&
                                       std::isfinite(result.fraction) &&
                                       result.fraction > 0.0f &&
                                       result.fraction < 1.0f &&
                                       finite(result.normal) &&
                                       result.normal.z >= standable;

        if (!base || !moves) return;

        // Strip existing jump/duck buttons from subtick moves (prevents bhop conflicts)
        for (int i = 0; i < moves->m_current_size; ++i) {
            const auto step = base->mutable_subtick_moves(i);
            if (step && (step->button() & controlled)) {
                step->set_button(step->button() & ~controlled);
                if (!step->button()) step->set_pressed(false);
            }
        }

        const auto add_event = [&](std::uint64_t button, bool pressed, float step_when) {
            if (const auto step = systems::g_input.acquire_subtick_step(moves)) {
                step->set_button(button);
                step->set_pressed(pressed);
                step->set_when(step_when);
                step->set_analog_forward_delta(0.0f);
                step->set_analog_left_delta(0.0f);
            }
        };

        if (!landing_this_tick) {
            // Still falling in the air: force crouch and suppress jump both in tick buttons and subtick
            cmd->buttons.value |= duck;
            cmd->buttons.value &= ~jump;
            cmd->buttons.value_changed |= controlled;
            cmd->buttons.value_scroll &= ~controlled;

            add_event(duck, true, 0.0f);
            this->m_active_this_tick = true;
            return;
        }

        // Landing detected this tick!
        // 1. Release duck at the very start of the tick (t = 0.0f) so the engine begins unducking
        add_event(duck, false, 0.0f);

        // 2. Schedule jump release before impact and jump press at ground impact
        const float when = std::clamp(std::round(result.fraction * 64.0f) / 64.0f, 1.0f / 64.0f, 63.0f / 64.0f);
        const float release_jump_when = std::clamp(when - 1.0f / 64.0f, 0.0f, 62.0f / 64.0f);

        if (release_jump_when < when) {
            add_event(jump, false, release_jump_when);
        }
        add_event(jump, true, when);

        // 3. Clear both duck and jump from command buttons so the subtick rising edge registers
        // (following the proven subtick pattern from bunnyhop.cpp)
        cmd->buttons.value &= ~controlled;
        cmd->buttons.value_changed |= controlled;
        cmd->buttons.value_scroll &= ~controlled;

        this->m_fired_last_tick = true;
        this->m_active_this_tick = true;
    }
}
