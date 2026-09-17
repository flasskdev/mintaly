#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include "../movement.hpp"
#include "../jumpbug_timing.hpp"
#include <protection/game_addresses.hpp>

namespace features::movement {
    namespace {
        constexpr float standing_height = 72.0f;
        constexpr float ground_probe = 2.0f;

        bool finite(const math::vector3& v) {
            return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
        }
    }

    void jumpbug::on_create_move(systems::input::usercmd* cmd, std::uint64_t original_buttons) {
        this->m_active_this_tick = false;
        const bool fired_previous = this->m_fired_last_tick;
        this->m_fired_last_tick = false;
        const auto& config = settings::g_movement.jumpbug;
        if (!cmd || !(config.value || (config.bind.key != 0 && config.bind.active))) return;

        const auto local = systems::g_local.get();
        const auto& pre = systems::g_prediction.pre();
        if (!local.pawn || !local.is_alive || !pre.movement_valid || pre.pawn != local.pawn) return;
        const auto move_type = memory::read<std::uint8_t>(local.pawn + SCHEMA("C_BaseEntity", "m_nActualMoveType"_hash));
        if (move_type == cstypes::move_type::ladder || move_type == cstypes::move_type::noclip) return;

        if (fired_previous && !(original_buttons & cstypes::command_buttons::in_jump)) {
            cmd->buttons.value &= ~cstypes::command_buttons::in_jump;
            cmd->buttons.value_changed |= cstypes::command_buttons::in_jump;
            cmd->buttons.value_scroll &= ~cstypes::command_buttons::in_jump;
        }
        if ((pre.flags & cstypes::entity_flags::on_ground) || !finite(pre.networked_origin) ||
            !finite(pre.networked_velocity) || pre.networked_velocity.z >= 0.0f) return;
        if (!finite(pre.collision_mins) || !finite(pre.collision_maxs)) return;
        const float height = pre.collision_maxs.z - pre.collision_mins.z;
        if (height <= 0.0f || height > standing_height || pre.collision_mins.x >= pre.collision_maxs.x ||
            pre.collision_mins.y >= pre.collision_maxs.y) return;

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
        if (!std::isfinite(pre.gravity_scale) || pre.gravity_scale < 0.0f) return;
        // Source movement uses the default multiplier for an unset (zero) scale.
        const float gravity = gravity_var->get<float>() * (pre.gravity_scale > 0.0f ? pre.gravity_scale : 1.0f);
        const float standable = normal_var->get<float>();
        if (!std::isfinite(dt) || dt <= 0.0f || !std::isfinite(gravity) || gravity < 0.0f ||
            !std::isfinite(standable) || standable <= 0.0f || standable > 1.0f) return;

        const auto filter = systems::g_tracing.make_player_movement_filter(local.pawn, mask, 11);
        const systems::tracing::bbox_collision current{pre.collision_mins, pre.collision_maxs};
        auto expanded = current;
        expanded.mins.z -= standing_height - height;
        const auto position = [&](float when) {
            const float time = when * dt;
            auto pos = pre.networked_origin + pre.networked_velocity * time;
            pos.z -= 0.5f * gravity * time * time;
            return pos;
        };
        bool fire = false;
        float fire_when = 0.0f;

        const auto usable = [](const systems::tracing::result& trace) {
            return !trace.all_solid && std::isfinite(trace.fraction) &&
                trace.fraction >= 0.0f && trace.fraction <= 1.0f && finite(trace.end_pos);
        };
        // Stay crouched in flight. Only the release position needs to fit the
        // expanded hull; sweeping that hull over the entire approach rejects
        // paths which the actual crouched player can traverse.
        if (pre.ducked && standing_height - height > ground_probe) {
            auto probe_hull = current;
            // A downward probe on the CURRENT hull reaches one unit below the
            // eventual standing feet. It brackets the middle of the 2-unit window.
            const float probe_depth = standing_height - height + ground_probe * 0.5f;
            const auto when = jumpbug_timing::find_contact_time([&](float time) -> std::optional<bool> {
                const auto pos = position(time);
                if (!finite(pos)) return std::nullopt;
                // The movement segment uses only the current (crouched) hull.
                const auto path = systems::g_tracing.trace_player_bbox(pre.networked_origin, pos, current, filter, movement);
                if (!usable(path)) return std::nullopt;
                if (path.fraction < 1.0f) {
                    // At high fall speed the end of the tick can cross the floor.
                    // It is an upper bound for refinement, NOT a valid release.
                    if (!finite(path.normal) || path.normal.z < standable) return std::nullopt;
                    return true;
                }
                auto below = pos;
                below.z -= probe_depth;
                const auto ground = systems::g_tracing.trace_player_bbox(pos, below, probe_hull, filter, movement);
                if (!usable(ground)) return std::nullopt;
                if (ground.fraction == 1.0f) return false;
                if (!finite(ground.normal) || ground.normal.z < standable) return std::nullopt;
                return true;
            });
            const auto safe_release = [&](float time) {
                const auto pos = position(time);
                if (!finite(pos)) return false;
                const auto path = systems::g_tracing.trace_player_bbox(pre.networked_origin, pos, current, filter, movement);
                if (!usable(path) || path.fraction < 1.0f) return false;
                const auto clearance = systems::g_tracing.trace_player_bbox(pos, pos, expanded, filter, movement);
                if (!usable(clearance) || clearance.fraction != 1.0f) return false;
                auto below = pos;
                below.z -= ground_probe;
                const auto ground = systems::g_tracing.trace_player_bbox(pos, below, expanded, filter, movement);
                return usable(ground) && ground.fraction > 0.0f && ground.fraction < 1.0f &&
                    finite(ground.normal) && ground.normal.z >= standable;
            };
            if (when && safe_release(*when) && safe_release(*when + jumpbug_timing::event_gap)) {
                fire = true;
                fire_when = *when;
            }
        }

        const auto base = cmd->csgo_user_cmd.mutable_base();
        const auto moves = base ? base->mutable_subtick_moves() : nullptr;
        if (!moves) return;
        constexpr auto jump = cstypes::command_buttons::in_jump;
        constexpr auto duck = cstypes::command_buttons::in_duck;
        constexpr auto controlled = jump | duck;
        const int old_size = moves->m_current_size;
        std::array<proto::subtick_move_step*, 4> steps{};
        const int needed = fire ? 4 : 2;
        // Allocate everything before modifying existing input. Failed allocation
        // must not leave half an unduck/jump sequence in the command.
        for (int i = 0; i < needed; ++i) {
            steps[i] = systems::g_input.acquire_subtick_step(moves);
            if (!steps[i]) {
                moves->m_current_size = old_size;
                return;
            }
        }
        for (int i = 0; i < old_size; ++i) {
            const auto step = base->mutable_subtick_moves(i);
            if (step && (step->button() & controlled)) {
                step->set_button(step->button() & ~controlled);
                if (!step->button()) step->set_pressed(false);
            }
        }
        const auto event = [](proto::subtick_move_step* step, std::uint64_t button, bool pressed, float when) {
            step->set_button(button);
            step->set_pressed(pressed);
            step->set_when(when);
            step->set_analog_forward_delta(0.0f);
            step->set_analog_left_delta(0.0f);
        };
        // Hold crouch until the verified window; release jump before pressing it.
        // Distinct times prevent unduck and jump being collapsed into one state.
        event(steps[0], jump, false, 0.0f);
        event(steps[1], duck, true, 0.0f);
        if (fire) {
            event(steps[2], duck, false, fire_when);
            event(steps[3], jump, true, fire_when + jumpbug_timing::event_gap);
        }
        cmd->buttons.value = (cmd->buttons.value & ~controlled) | (fire ? jump : duck);
        cmd->buttons.value_changed |= controlled;
        cmd->buttons.value_scroll &= ~controlled;
        this->m_fired_last_tick = fire;
        this->m_active_this_tick = true;
    }
}
