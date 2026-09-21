#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <array>
#include <utilities/diag.hpp>
#include <chrono>
#include <cmath>
#include "../movement.hpp"
#include "../jumpbug_timing.hpp"
#include "../jumpbug_command.hpp"
#include <protection/game_addresses.hpp>

namespace features::movement {
    namespace {
        bool finite(const math::vector3& v) {
            return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
        }
    }
    void jumpbug::on_create_move(systems::input::usercmd* cmd, std::uint64_t original_buttons) {
        m_active_this_tick = false;
        if (!cmd) return;
        constexpr auto jump = cstypes::command_buttons::in_jump;
        constexpr auto duck = cstypes::command_buttons::in_duck;
        constexpr auto controlled = jump | duck;
        const auto base = cmd->csgo_user_cmd.mutable_base();
        const auto moves = base ? base->mutable_subtick_moves() : nullptr;
        const auto local = systems::g_local.get();
        const auto& pre = systems::g_prediction.pre();
        if (m_pawn != local.pawn || !local.is_alive) {
            m_cycle = {}; m_owned_duck = false; m_fired_last_tick = false;
            m_pawn = local.pawn;
        }
        // Release only input owned by this feature. Preserve a physical key
        // held by the player and reserve events before mutating the command.
        const auto release_owned = [&]() {
            std::uint64_t mask = 0;
            if (m_owned_duck && !(original_buttons & duck)) mask |= duck;
            if (m_fired_last_tick && !(original_buttons & jump)) mask |= jump;
            if (!mask) { m_owned_duck = false; m_fired_last_tick = false; return; }
            if (!moves) return;
            const int old_size = moves->m_current_size;
            std::array<proto::subtick_move_step*, 2> events{};
            int count = 0;
            for (const auto button : {jump, duck}) {
                if (!(mask & button)) continue;
                auto* event = systems::g_input.acquire_subtick_step(moves);
                if (!event) { moves->m_current_size = old_size; return; }
                events[count++] = event;
            }
            for (int i = 0; i < old_size; ++i) {
                if (auto* step = base->mutable_subtick_moves(i); step && (step->button() & mask)) {
                    step->set_button(step->button() & ~mask);
                    if (!step->button()) step->set_pressed(false);
                }
            }
            count = 0;
            for (const auto button : {jump, duck}) {
                if (!(mask & button)) continue;
                auto* event = events[count++]; *event = {};
                event->set_button(button); event->set_pressed(false); event->set_when(0.0f);
            }
            cmd->buttons.value &= ~mask;
            cmd->buttons.value_changed |= mask;
            cmd->buttons.value_scroll &= ~mask;
            m_owned_duck = false; m_fired_last_tick = false;
        };
        const auto& config = settings::g_movement.jumpbug;
        const bool bound = config.bind.key != 0 && config.bind.active;
        float probe_fraction = -1.0f, support_fraction = -1.0f;
        bool probe_solid = false, support_solid = false;
        int probe_calls = 0;
        // Diagnostic only: do not fake immunity by writing health/fall velocity.
        // One status per 250 ms, plus actual release attempts. Remove after
        // reproducing, or define MINTALY_JUMPBUG_DIAGNOSTICS=0 in the build.
        const auto report = [&](const char* reason, float when = -1.0f) {
#if !defined(MINTALY_JUMPBUG_DIAGNOSTICS) || MINTALY_JUMPBUG_DIAGNOSTICS
            if ((!bound && !config.value) || !local.is_alive) return;
            static auto next_report = std::chrono::steady_clock::time_point{};
            const auto now = std::chrono::steady_clock::now();
            if (when < 0.0f && now < next_report) return;
            next_report = now + std::chrono::milliseconds(250);
            diag::writef(diag::level::info,
                "[jumpbug] reason=%s valid=%d flags=%u vz=%.3f ducked=%d duck=%.3f hull=%.3f release=%.6f probes=%d probe=%.6f probe_solid=%d support=%.6f support_solid=%d jump=%d",
                reason, static_cast<int>(pre.movement_valid), static_cast<unsigned>(pre.flags),
                pre.networked_velocity.z, static_cast<int>(pre.ducked), pre.duck_amount,
                pre.collision_maxs.z - pre.collision_mins.z, when, probe_calls,
                probe_fraction, static_cast<int>(probe_solid), support_fraction,
                static_cast<int>(support_solid), 1);
#else
            (void)reason; (void)when;
#endif
        };
        if (!local.pawn || !local.is_alive || !pre.movement_valid || pre.pawn != local.pawn) {
            report("invalid_prestate"); release_owned(); return;
        }
        const bool grounded = (pre.flags & cstypes::entity_flags::on_ground) != 0;
        const bool can_attempt = m_cycle.available(grounded, pre.networked_velocity.z);
        if ((!bound && !config.value) || grounded || !can_attempt) { release_owned(); return; }
        const auto move_type = memory::read<std::uint8_t>(local.pawn + SCHEMA("C_BaseEntity", "m_nActualMoveType"_hash));
        if (move_type == cstypes::move_type::ladder || move_type == cstypes::move_type::noclip) {
            release_owned(); return;
        }
        if (!finite(pre.networked_origin) || !finite(pre.networked_velocity) || pre.networked_velocity.z >= 0.0f) {
            release_owned(); return;
        }
        // Prepare the crouched hull early. Waiting until damage speed to crouch
        // would leave too little time for the duck transition near the floor.
        const auto movement = memory::read<std::uintptr_t>(local.pawn + SCHEMA("C_BasePlayerPawn", "m_pMovementServices"_hash));
        if (!moves || !movement || !PATTERN(patterns::trace_hull) || !PATTERN(patterns::trace_filter_set_collision)) { report("missing_trace_dependency"); release_owned(); return; }
        const auto pawn = memory::safe_read<std::uintptr_t>(movement + 56).value_or(0);
        if (!pawn) { report("missing_movement_pawn"); release_owned(); return; }
        auto mask = memory::read<std::uint64_t>(pawn + 0xd48);
        if (memory::read<std::uint32_t>(pawn + 0x3f8) & 0x10) mask |= 0x20;
        const auto gravity_var = CONVAR("sv_gravity");
        const auto normal_var = CONVAR("sv_standable_normal");
        if (!gravity_var || !normal_var) { report("missing_convar"); release_owned(); return; }
        const float gravity = gravity_var->get<float>() * pre.gravity_scale;
        const float standable = normal_var->get<float>();
        const auto vz = jumpbug_timing::movement_velocity_z(pre.networked_velocity.z, gravity, cstypes::tick_interval);
        if (!vz || !std::isfinite(standable) || standable <= 0.0f || standable > 1.0f) { report("invalid_gravity_or_normal"); release_owned(); return; }
        const auto mins = pre.collision_mins;
        const auto maxs = pre.collision_maxs;
        if (!finite(mins) || !finite(maxs) || maxs.x <= mins.x || maxs.y <= mins.y || maxs.z <= mins.z) { report("invalid_hull"); release_owned(); return; }
        const auto expansion = jumpbug_timing::airborne_unduck_expansion(maxs.z - mins.z, 72.0f);
        if (!expansion) { report("invalid_unduck_expansion"); release_owned(); return; }
        auto velocity = pre.networked_velocity;
        velocity.z = *vz;
        const auto travel = velocity * cstypes::tick_interval;
        const auto filter = systems::g_tracing.make_player_movement_filter(local.pawn, mask, 11);
        std::optional<float> release;
        const bool include_jump = true; // A jumpbug needs the landing jump edge, including on blank/legacy CFGs.
        // Keep crouching until the standing feet enter the ground categorization
        // window, instead of releasing at tick zero and landing normally.
        if (pre.ducked && pre.duck_amount > 0.0f && *expansion > 0.0f) {
            auto standing_mins = mins;
            auto standing_maxs = maxs;
            standing_mins.z -= *expansion;
            standing_maxs.z += *expansion;
            auto probe_mins = standing_mins;
            probe_mins.z -= 1.5f;
            const auto safe_at = [&](float t) {
                const auto pos = pre.networked_origin + travel * t;
                auto check_pos = pos;
                const auto crouched = systems::g_tracing.trace_player_bbox(pre.networked_origin, pos,
                    {mins, maxs}, filter, movement);
                if (crouched.all_solid || !std::isfinite(crouched.fraction)) return false;

                auto clear = systems::g_tracing.trace_player_bbox(check_pos, check_pos,
                    {standing_mins, standing_maxs}, filter, movement);
                // When evaluating landing clearance (including subtick step t + event_gap or immediate landing),
                // sub-unit ground penetration must be projected to the standable surface so valid hops are not rejected.
                if (clear.all_solid) {
                    const auto ground_probe = systems::g_tracing.trace_player_bbox(pre.networked_origin, check_pos,
                        {standing_mins, standing_maxs}, filter, movement);
                    if (!ground_probe.all_solid && ground_probe.fraction < 1.0f && ground_probe.normal.z >= standable) {
                        check_pos = pre.networked_origin + (check_pos - pre.networked_origin) * ground_probe.fraction;
                        check_pos.z += 0.03125f;
                        clear = systems::g_tracing.trace_player_bbox(check_pos, check_pos,
                            {standing_mins, standing_maxs}, filter, movement);
                    } else {
                        // If starting probe was also solid, probe down with crouched hull to find exact floor height
                        auto floor_probe_end = pre.networked_origin;
                        floor_probe_end.z -= 64.0f;
                        const auto floor_probe = systems::g_tracing.trace_player_bbox(pre.networked_origin, floor_probe_end,
                            {mins, maxs}, filter, movement);
                        if (!floor_probe.all_solid && floor_probe.fraction < 1.0f && floor_probe.normal.z >= standable) {
                            const float floor_z = pre.networked_origin.z + mins.z - 64.0f * floor_probe.fraction;
                            check_pos.z = floor_z - standing_mins.z + 0.03125f;
                            clear = systems::g_tracing.trace_player_bbox(check_pos, check_pos,
                                {standing_mins, standing_maxs}, filter, movement);
                        }
                    }
                }
                auto below = check_pos;
                below.z -= 2.0f;
                const auto support = systems::g_tracing.trace_player_bbox(check_pos, below,
                    {standing_mins, standing_maxs}, filter, movement);
                support_fraction = support.fraction;
                support_solid = support.all_solid;

                return !clear.all_solid && std::isfinite(clear.fraction) && clear.fraction == 1.0f &&
                    !support.all_solid && std::isfinite(support.fraction) && support.fraction >= 0.0f &&
                    support.fraction < 1.0f && finite(support.normal) && support.normal.z >= standable &&
                    !crouched.all_solid && std::isfinite(crouched.fraction) && crouched.fraction == 1.0f;
            };
            release = jumpbug_timing::find_release_time([&](float t) -> std::optional<bool> {
                const auto hit = systems::g_tracing.trace_player_bbox(pre.networked_origin,
                    pre.networked_origin + travel * t, {probe_mins, standing_maxs}, filter, movement);
                ++probe_calls; probe_fraction = hit.fraction; probe_solid = hit.all_solid;
                if (hit.all_solid || !std::isfinite(hit.fraction) || hit.fraction < 0.0f ||
                    hit.fraction > 1.0f || !finite(hit.normal)) return std::nullopt;
                if (hit.fraction == 1.0f) return false;
                if (hit.normal.z < standable) return std::nullopt;
                return true;
            }, [&](float t) {
                return safe_at(t) && (!include_jump || safe_at(t + jumpbug_timing::event_gap));
            });
        }
        const auto plan = jumpbug_command::make(cmd->buttons.value, jump, duck, release, include_jump);
        if (!plan) { release_owned(); return; }
        // Reserve all events before removing any existing jump/duck input.
        const int original_size = moves->m_current_size;
        std::array<proto::subtick_move_step*, 4> events{};
        for (int i = 0; i < plan->count; ++i) {
            events[i] = systems::g_input.acquire_subtick_step(moves);
            if (!events[i]) { report("subtick_allocation_failed"); moves->m_current_size = original_size; return; }
        }
        for (int i = 0; i < original_size; ++i) {
            if (const auto step = base->mutable_subtick_moves(i); step && (step->button() & controlled)) {
                step->set_button(step->button() & ~controlled);
                if (!step->button()) step->set_pressed(false);
            }
        }
        for (int i = 0; i < plan->count; ++i) {
            *events[i] = {};
            events[i]->set_button(plan->events[i].button);
            events[i]->set_pressed(plan->events[i].pressed);
            events[i]->set_when(plan->events[i].when);
        }
        cmd->buttons.value = plan->final_buttons;
        cmd->buttons.value_changed |= controlled;
        cmd->buttons.value_scroll &= ~controlled;
        m_owned_duck = plan->owns_duck;
        m_fired_last_tick = plan->owns_jump;
        report(release ? "release" : (pre.ducked ? "prepare_no_window" : "prepare_crouch"),
            release.value_or(-1.0f));
        if (release) m_cycle.fired();
        m_active_this_tick = true;
    }
}
