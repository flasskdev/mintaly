#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <array>
#include <cmath>
#include "../movement.hpp"
#include "../jumpbug_timing.hpp"
#include <protection/game_addresses.hpp>

namespace features::movement {
    namespace {
        bool finite(const math::vector3& v) {
            return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
        }
    }
    void jumpbug::on_create_move(systems::input::usercmd* cmd, std::uint64_t original_buttons) {
        m_active_this_tick = false;
        const bool fired_previous = m_fired_last_tick;
        m_fired_last_tick = false;
        if (!cmd) return;
        constexpr auto jump = cstypes::command_buttons::in_jump;
        constexpr auto duck = cstypes::command_buttons::in_duck;
        constexpr auto controlled = jump | duck;
        if (fired_previous && !(original_buttons & jump)) {
            cmd->buttons.value &= ~jump;
            cmd->buttons.value_changed |= jump;
            cmd->buttons.value_scroll &= ~jump;
        }
        const auto& config = settings::g_movement.jumpbug;
        const bool bound = config.bind.key != 0 && config.bind.active;
        if (!bound && !config.value) return;
        const auto local = systems::g_local.get();
        const auto& pre = systems::g_prediction.pre();
        if (!local.pawn || !local.is_alive || !pre.movement_valid || pre.pawn != local.pawn ||
            (pre.flags & cstypes::entity_flags::on_ground)) return;
        const auto move_type = memory::read<std::uint8_t>(local.pawn + SCHEMA("C_BaseEntity", "m_nActualMoveType"_hash));
        if (move_type == cstypes::move_type::ladder || move_type == cstypes::move_type::noclip) return;
        if (!finite(pre.networked_origin) || !finite(pre.networked_velocity) || pre.networked_velocity.z >= 0.0f) return;
        if (!bound && pre.networked_velocity.z > -200.0f) return;
        const auto base = cmd->csgo_user_cmd.mutable_base();
        const auto moves = base ? base->mutable_subtick_moves() : nullptr;
        const auto movement = memory::read<std::uintptr_t>(local.pawn + SCHEMA("C_BasePlayerPawn", "m_pMovementServices"_hash));
        if (!moves || !movement || !PATTERN(patterns::trace_hull) || !PATTERN(patterns::trace_filter_set_collision)) return;
        const auto pawn = memory::safe_read<std::uintptr_t>(movement + 56).value_or(0);
        if (!pawn) return;
        auto mask = memory::read<std::uint64_t>(pawn + 0xd48);
        if (memory::read<std::uint32_t>(pawn + 0x3f8) & 0x10) mask |= 0x20;
        const auto gravity_var = CONVAR("sv_gravity");
        const auto normal_var = CONVAR("sv_standable_normal");
        if (!gravity_var || !normal_var) return;
        const float gravity = gravity_var->get<float>() * pre.gravity_scale;
        const float standable = normal_var->get<float>();
        const auto vz = jumpbug_timing::movement_velocity_z(pre.networked_velocity.z, gravity, cstypes::tick_interval);
        if (!vz || !std::isfinite(standable) || standable <= 0.0f || standable > 1.0f) return;
        const auto mins = pre.collision_mins;
        const auto maxs = pre.collision_maxs;
        if (!finite(mins) || !finite(maxs) || maxs.x <= mins.x || maxs.y <= mins.y || maxs.z <= mins.z) return;
        const auto expansion = jumpbug_timing::airborne_unduck_expansion(maxs.z - mins.z, 72.0f);
        if (!expansion) return;
        auto velocity = pre.networked_velocity;
        velocity.z = *vz;
        const auto travel = velocity * cstypes::tick_interval;
        const auto filter = systems::g_tracing.make_player_movement_filter(local.pawn, mask, 11);
        std::optional<float> release;
        // Keep crouching until the standing feet enter the ground categorization
        // window, instead of releasing at tick zero and landing normally.
        if (pre.duck_amount > 0.0f && *expansion > 0.0f) {
            auto standing_mins = mins;
            auto standing_maxs = maxs;
            standing_mins.z -= *expansion;
            standing_maxs.z += *expansion;
            auto probe_mins = standing_mins;
            probe_mins.z -= 1.0f;
            release = jumpbug_timing::find_contact_time([&](float t) -> std::optional<bool> {
                const auto hit = systems::g_tracing.trace_player_bbox(pre.networked_origin,
                    pre.networked_origin + travel * t, {probe_mins, standing_maxs}, filter, movement);
                if (hit.all_solid || !std::isfinite(hit.fraction) || !finite(hit.normal)) return std::nullopt;
                if (hit.fraction >= 1.0f) return false;
                if (hit.normal.z < standable) return std::nullopt;
                return true;
            });
            if (release) {
                for (float t : {*release, *release + jumpbug_timing::event_gap}) {
                    const auto pos = pre.networked_origin + travel * t;
                    const auto clear = systems::g_tracing.trace_player_bbox(pos, pos,
                        {standing_mins, standing_maxs}, filter, movement);
                    auto below = pos;
                    below.z -= 2.0f;
                    const auto support = systems::g_tracing.trace_player_bbox(pos, below,
                        {standing_mins, standing_maxs}, filter, movement);
                    const auto crouched = systems::g_tracing.trace_player_bbox(pre.networked_origin, pos,
                        {mins, maxs}, filter, movement);
                    if (clear.all_solid || !std::isfinite(clear.fraction) || clear.fraction < 1.0f ||
                        support.all_solid || !std::isfinite(support.fraction) || support.fraction <= 0.0f ||
                        support.fraction >= 1.0f || !finite(support.normal) || support.normal.z < standable ||
                        crouched.all_solid || !std::isfinite(crouched.fraction) || crouched.fraction < 1.0f) {
                        release.reset();
                        break;
                    }
                }
            }
        }
        // Reserve all events before removing any existing jump/duck input.
        const int original_size = moves->m_current_size;
        std::array<proto::subtick_move_step*, 4> events{};
        const int count = release ? 4 : 2;
        for (int i = 0; i < count; ++i) {
            events[i] = systems::g_input.acquire_subtick_step(moves);
            if (!events[i]) { moves->m_current_size = original_size; return; }
        }
        for (int i = 0; i < original_size; ++i) {
            if (const auto step = base->mutable_subtick_moves(i); step && (step->button() & controlled)) {
                step->set_button(step->button() & ~controlled);
                if (!step->button()) step->set_pressed(false);
            }
        }
        const auto event = [&](int i, std::uint64_t button, bool pressed, float when) {
            *events[i] = {};
            events[i]->set_button(button);
            events[i]->set_pressed(pressed);
            events[i]->set_when(when);
        };
        event(0, jump, false, 0.0f);
        event(1, duck, true, 0.0f);
        cmd->buttons.value = (cmd->buttons.value | duck) & ~jump;
        cmd->buttons.value_changed |= controlled;
        cmd->buttons.value_scroll &= ~controlled;
        if (release) {
            if (*release == 0.0f) {
                events[1]->set_button(0);
                events[1]->set_pressed(false);
            }
            event(2, duck, false, *release);
            event(3, jump, true, *release + jumpbug_timing::event_gap);
            m_fired_last_tick = true;
        }
        m_active_this_tick = true;
    }
}
