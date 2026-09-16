#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include "../movement.hpp"
#include <protection/game_addresses.hpp>

namespace features::movement {
    namespace {
        constexpr float standing_height = 72.0f;
        constexpr float ground_probe = 2.0f;
        constexpr int samples = 128;

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
        const float gravity = gravity_var->get<float>() * pre.gravity_scale;
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

        // A jumpbug needs a completed crouch and room to stand up. A swept
        // hull hitting the floor is NOT success: at that point it may be too late.
        if (pre.ducked && standing_height - height > ground_probe) {
            auto end = position(1.0f);
            end.z -= ground_probe;
            const auto approach = systems::g_tracing.trace_player_bbox(pre.networked_origin, end, expanded, filter, movement);
            if (!approach.all_solid && std::isfinite(approach.fraction) && approach.fraction >= 0.0f &&
                approach.fraction < 1.0f && approach.normal.z >= standable) {
                for (int sample = 0; sample < samples; ++sample) {
                    const float when = static_cast<float>(sample) / samples;
                    const auto pos = position(when);
                    if (!finite(pos)) break;
                    // Reject collisions before the planned release (walls, ceilings,
                    // and penetrated floors). Never use a standing-landing fallback.
                    const auto path = systems::g_tracing.trace_player_bbox(pre.networked_origin, pos, expanded, filter, movement);
                    if (path.all_solid || !std::isfinite(path.fraction) || path.fraction < 1.0f) continue;
                    auto below = pos;
                    below.z -= ground_probe;
                    const auto ground = systems::g_tracing.trace_player_bbox(pos, below, expanded, filter, movement);
                    if (!ground.all_solid && std::isfinite(ground.fraction) && ground.fraction > 0.0f &&
                        ground.fraction < 1.0f && ground.normal.z >= standable) {
                        fire = true;
                        fire_when = when;
                        break;
                    }
                }
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
        // input::apply stable-sorts these events, preserving equal-time order.
        event(steps[0], jump, false, 0.0f);
        event(steps[1], duck, true, 0.0f);
        if (fire) {
            event(steps[2], duck, false, fire_when);
            event(steps[3], jump, true, fire_when);
        }
        cmd->buttons.value = (cmd->buttons.value & ~controlled) | (fire ? jump : duck);
        cmd->buttons.value_changed |= controlled;
        cmd->buttons.value_scroll &= ~controlled;
        this->m_fired_last_tick = fire;
        this->m_active_this_tick = true;
    }
}
