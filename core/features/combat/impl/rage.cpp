#include <pch/pch.hpp>
#include <cassert>
#include <limits>
#include <span>
#include <core/features/combat/ballistics.hpp>
#include <utilities/threadpool/threadpool.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <utilities/diag.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <protection/game_addresses.hpp>

namespace features::combat {

    // Helper for faster angle calculations
    namespace math_opt {
        [[nodiscard]] inline float deg_to_rad(float deg) { return deg * (std::numbers::pi_v<float> / 180.0f); }
        [[nodiscard]] inline float rad_to_deg(float rad) { return rad * (180.0f / std::numbers::pi_v<float>); }
    }

    void rage::on_create_move(systems::input::usercmd* cmd)
    {
        auto& ctx = g_shared.ctx();
        const auto local = systems::g_local.get();

        this->update_penetration_crosshair(local);
        this->m_should_stop = false;
        this->m_firing_this_tick = false;

        if (!ctx.valid)
        {
            this->m_revolver_cock_ticks = 0;
            return;
        }

        // Reset revolver logic if weapon changed
        if (ctx.item_def_idx != cstypes::item_definition_index::weapon_r8_revolver)
            this->m_revolver_cock_ticks = 0;

        // Duckpeek logic reset
        if (!settings::g_combat.m_duckpeek.enabled.value)
        {
            this->m_release_duck_for_shot = false;
            this->m_duckpeek_reduck = false;
            this->m_duckpeek_reduck_ticks = 0;
        }
        else if (this->m_duckpeek_reduck_ticks > 0)
        {
            --this->m_duckpeek_reduck_ticks;
        }

        // Zeus handling
        if (this->m_zeus_fired)
        {
            this->m_zeus_fired = false;
            if (settings::g_combat.m_zeusbot.drop_after && !systems::g_local.is_in_deathmatch())
                memory::call<void>(PATTERN(patterns::engine_client_cmd), addresses::globals::source2engine_to_client, 0, "drop", 0x7ffef001);
            return;
        }

        const auto is_knife = ctx.weapon_type == cstypes::weapon_type::knife;
        const auto is_taser = ctx.weapon_type == cstypes::weapon_type::taser;

        // Filter invalid weapon types early
        if (!is_knife && !is_taser && (ctx.weapon_type < cstypes::weapon_type::pistol || ctx.weapon_type > cstypes::weapon_type::lmg))
            return;

        // Prediction snapshots and accuracy updates are expensive. Disabled
        // features and unavailable shots must not enter speculative simulation.
        if ((is_knife && !settings::g_combat.m_knifebot.enabled) ||
            (is_taser && !settings::g_combat.m_zeusbot.enabled) ||
            (!is_knife && !is_taser && !settings::g_combat.m_ragebot.enabled))
        {
            this->m_revolver_cock_ticks = 0;
            return;
        }
        if (!g_shared.can_shoot(cmd, local.controller))
        {
            this->m_revolver_cock_ticks = 0;
            return;
        }

        auto aim_ctx = this->build_context(cmd, local);
        if (!ctx.valid)
            return;

        if (is_knife)
        {
            if (!g_shared.can_shoot(cmd, local.controller))
                return;
            this->run_knife(cmd, aim_ctx, local);
        }
        else if (is_taser)
        {
            if (!g_shared.can_shoot(cmd, local.controller))
                return;
            this->run_taser(cmd, aim_ctx, local);
        }
        else if (ctx.item_def_idx == cstypes::item_definition_index::weapon_r8_revolver)
        {
            if (!settings::g_combat.m_ragebot.enabled)
            {
                this->m_revolver_cock_ticks = 0;
                return;
            }

            if (settings::g_combat.m_autos.revolver_quick.value)
            {
                this->m_revolver_cock_ticks = 0;
                // Quick shot: just fire attack2
                if (cmd->buttons.value & cstypes::command_buttons::in_attack)
                {
                    cmd->buttons.value &= ~cstypes::command_buttons::in_attack;
                    cmd->buttons.value_changed |= cstypes::command_buttons::in_attack;
                    cmd->buttons.value_scroll &= ~cstypes::command_buttons::in_attack;
                    cmd->csgo_user_cmd.set_attack1_start_history_index(-1);
                }
                if (!g_shared.can_shoot(cmd, local.controller))
                    return;
                this->run_gun(cmd, aim_ctx, local);
            }
            else
            {
                this->auto_revolver(cmd, aim_ctx, local);
            }
        }
        else
        {
            this->m_revolver_cock_ticks = 0;
            if (!settings::g_combat.m_ragebot.enabled)
                return;
            if (!g_shared.can_shoot(cmd, local.controller))
                return;
            this->run_gun(cmd, aim_ctx, local);
        }
    }

    void rage::on_render(xdraw::draw_list& draw_list)
    {
        this->draw_penetration_crosshair(draw_list);

        const auto& config = settings::g_combat.m_ragebot.get_group(g_shared.ctx().weapon_type, g_shared.ctx().item_def_idx);
        if (!config.debug_multipoints.value)
            return;

        std::lock_guard lock(m_debug_mtx);
        for (const auto& pt : m_debug_points)
        {
            const auto screen = systems::g_view.project(pt.position);
            if (!systems::g_view.projection_valid(screen))
                continue;

            xdraw::color col{};
            switch (pt.hitbox_index)
            {
            case 0: col = { 255, 80, 80 }; break; // Head
            case 2: case 3: col = { 220, 220, 60 }; break; // Stomach
            case 4: case 5: case 6: col = { 255, 160, 60 }; break; // Chest
            case 7: case 8: case 9: case 10: case 11: case 12: col = { 80, 160, 255 }; break; // Legs
            case 13: case 14: case 15: case 16: case 17: case 18: col = { 180, 80, 255 }; break; // Arms
            default: col = { 200, 200, 200 }; break;
            }

            const auto alpha = pt.is_center ? std::uint8_t{ 255 } : std::uint8_t{ 160 };
            const auto radius = pt.is_center ? 3.5f : 2.0f;
            draw_list.circle_filled(screen.x, screen.y, radius, col.alpha(alpha));
        }
    }

    rage::aim_context rage::build_context(systems::input::usercmd* cmd, const systems::local::snapshot& local) const
    {
        auto& ctx = g_shared.ctx();
        const auto& prestate = systems::g_prediction.pre();

        aim_context out{};
        out.velocity = prestate.velocity;
        std::optional<shared::weapon_accuracy> accuracy;
        math::vector3 aim_punch{};
        // Accuracy updates mutate weapon state. Run them only inside the guard,
        // once, at the same predicted time as recoil and the shooting position.
        const auto predicted = systems::g_prediction.simulate(cmd, local, [&]
            {
                g_shared.sh().snapshot(local.pawn, ctx.weapon_services);
                out.velocity = memory::read<math::vector3>(local.pawn + SCHEMA("C_BaseEntity", "m_vecAbsVelocity"_hash));
                out.on_ground = (memory::read<std::uint32_t>(local.pawn + SCHEMA("C_BaseEntity", "m_fFlags"_hash)) & cstypes::entity_flags::on_ground) != 0;
                accuracy = g_shared.get_accuracy_state(true);
                // Preserve recoil decay at the simulated time before state_guard
                // restores the pawn. fire_gun must not read the older punch again.
                aim_punch = g_shared.get_aim_punch(local.pawn);
            });

        if (!predicted || !accuracy || !ballistics::finite(out.velocity) ||
            !std::isfinite(aim_punch.x) || !std::isfinite(aim_punch.y) || !std::isfinite(aim_punch.z))
        {
            ctx.valid = false;
            return out;
        }

        out.spread = accuracy->spread;
        out.predicted_inaccuracy = accuracy->inaccuracy;
        ctx.spread = accuracy->spread;
        ctx.inaccuracy = accuracy->inaccuracy;
        ctx.recoil_index = accuracy->recoil_index;
        ctx.aim_punch = aim_punch;
        out.view_angles = systems::g_input.get_view_angles();
        out.is_scoped = ctx.is_scoped;
        out.weapon_max_speed = ctx.weapon_max_speed;
        out.accurate_threshold = ctx.weapon_max_speed * 0.34f;

        return out;
    }

    std::optional<rage::stop_prediction> rage::predict_stop(const aim_context& ctx, const math::vector3& current_eye, const systems::local::snapshot& local) const
    {
        const auto& shared_ctx = g_shared.ctx();
        const auto& prestate = systems::g_prediction.pre();
        const auto speed = prestate.networked_velocity.length_2d();
        const auto threshold = ctx.is_scoped ? std::min(ctx.accurate_threshold, 1.0f) : ctx.accurate_threshold;

        if (!prestate.movement_valid || prestate.pawn != local.pawn || !ctx.on_ground ||
            !std::isfinite(speed) || !std::isfinite(threshold) || threshold < 0.0f || speed <= threshold)
            return std::nullopt;

        auto sim_vel = prestate.networked_velocity;
        sim_vel.z = 0.0f;

        const auto sv_friction = CONVAR("sv_friction")->get<float>();
        const auto sv_stopspeed = CONVAR("sv_stopspeed")->get<float>();
        const auto sv_accelerate = CONVAR("sv_accelerate")->get<float>();
        const auto surface_friction = prestate.surface_friction;
        const auto movement_services = memory::read<std::uintptr_t>(local.pawn + SCHEMA("C_BasePlayerPawn", "m_pMovementServices"_hash));
        const auto max_move_speed = movement_services ? memory::read<float>(movement_services + SCHEMA("CPlayer_MovementServices", "m_flMaxspeed"_hash)) : 250.0f;

        if (!std::isfinite(sv_friction) || sv_friction < 0.0f ||
            !std::isfinite(sv_stopspeed) || sv_stopspeed < 0.0f ||
            !std::isfinite(sv_accelerate) || sv_accelerate < 0.0f ||
            !std::isfinite(surface_friction) || surface_friction < 0.0f ||
            !std::isfinite(shared_ctx.weapon_max_speed) || shared_ctx.weapon_max_speed <= 0.0f ||
            !std::isfinite(max_move_speed) || max_move_speed <= 0.0f)
            return std::nullopt;

        math::vector3 displacement{};
        // Integrate displacement and accuracy to the same stopping tick. The
        // old average-velocity estimate mixed two different stop horizons.
        for (auto i = 0; i < 15; ++i)
        {
            const auto sim_speed = sim_vel.length_2d();
            if (sim_speed <= threshold)
                break;

            const auto control = std::fmaxf(sim_speed, sv_stopspeed);
            const auto drop = sv_friction * surface_friction * control * cstypes::tick_interval;
            auto new_speed = std::fmaxf(sim_speed - drop, 0.0f);

            auto accel = sv_accelerate;
            if (shared_ctx.is_scoped)
            {
                const auto weapon_ratio = std::fminf(1.0f, shared_ctx.weapon_max_speed / 250.0f);
                const auto scoped_max = std::fmaxf(250.0f, max_move_speed) * weapon_ratio * 0.52f;
                if (new_speed > scoped_max - 5.0f)
                {
                    const auto t = 1.0f - std::fmaxf(0.0f, new_speed - (scoped_max - 5.0f)) / std::fmaxf(0.01f, 5.0f);
                    accel *= std::clamp(t, 0.0f, 1.0f);
                }
            }

            const auto accel_speed = std::fminf(accel * shared_ctx.weapon_max_speed * surface_friction * cstypes::tick_interval, new_speed);
            new_speed = std::fmaxf(new_speed - accel_speed, 0.0f);

            sim_vel *= (new_speed / sim_speed);
            displacement += sim_vel * cstypes::tick_interval;
        }

        if (sim_vel.length_2d() > threshold)
            return std::nullopt;

        const auto finite = [](const math::vector3& v)
        {
            return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
        };
        if (!finite(displacement) || !finite(current_eye) || !finite(prestate.origin) ||
            !finite(prestate.collision_mins) || !finite(prestate.collision_maxs) ||
            prestate.collision_maxs.x <= prestate.collision_mins.x ||
            prestate.collision_maxs.y <= prestate.collision_mins.y ||
            prestate.collision_maxs.z <= prestate.collision_mins.z)
            return std::nullopt;

        if (displacement.length_sqr() > 0.0f)
        {
            // Counter-strafe displacement is collinear, so one swept hull
            // checks the entire path without tracing every simulated tick.
            const auto origin = prestate.origin + displacement;
            const auto path = systems::g_tracing.trace_hull(prestate.origin, origin,
                prestate.collision_mins, prestate.collision_maxs, local.pawn, 0x1c3003, 4);
            if (path.all_solid || path.fraction != 1.0f || !finite(path.end_pos))
                return std::nullopt;
            const auto ground = systems::g_tracing.trace_hull(origin, origin - math::vector3{0.0f, 0.0f, 2.0f},
                prestate.collision_mins, prestate.collision_maxs, local.pawn, 0x1c3003, 4);
            if (ground.all_solid || !std::isfinite(ground.fraction) ||
                ground.fraction < 0.0f || ground.fraction >= 1.0f ||
                !finite(ground.normal) || ground.normal.z <= 0.7f)
                return std::nullopt;
        }

        const auto inaccuracy = g_shared.get_inaccuracy_at_velocity(local.pawn, sim_vel);
        if (!std::isfinite(inaccuracy) || inaccuracy < 0.0f)
            return std::nullopt;
        return stop_prediction
        {
            .eye = current_eye + displacement,
            .inaccuracy = inaccuracy
        };
    }

    std::vector<rage::candidate> rage::gather_candidates(const systems::local::snapshot& local, float max_distance_sq) const
    {
        const auto& shared_ctx = g_shared.ctx();
        const auto players = systems::g_entities.get_by_type(systems::entities::type::player);

        std::vector<candidate> out;
        out.reserve(players.size());

        const_cast<rage*>(this)->m_extrapolated_records.clear();

        for (const auto& p : players)
        {
            if (!p.ptr || p.ptr == local.controller)
                continue;

            if (!memory::read<bool>(p.ptr + SCHEMA("CCSPlayerController", "m_bPawnIsAlive"_hash)))
                continue;

            const auto pawn_handle = memory::read<std::uint32_t>(p.ptr + SCHEMA("CBasePlayerController", "m_hPawn"_hash));
            const auto pawn = systems::g_entities.lookup(pawn_handle);
            if (!pawn || pawn == local.pawn)
                continue;

            const auto team = memory::read<int>(pawn + SCHEMA("C_BaseEntity", "m_iTeamNum"_hash));
            if (!local.is_this_other_team(team))
                continue;

            const auto health = memory::read<int>(pawn + SCHEMA("C_BaseEntity", "m_iHealth"_hash));
            if (health <= 0)
                continue;

            if (memory::read<bool>(pawn + SCHEMA("C_CSPlayerPawn", "m_bGunGameImmunity"_hash)))
                continue;

            candidate c{};
            c.record_snapshots = g_shared.lc().get_scan_records(pawn);
            if (c.record_snapshots.empty())
                continue;
            std::vector<shared::lagcomp::record*> records;
            records.reserve(c.record_snapshots.size());
            for (auto& record : c.record_snapshots)
                records.push_back(&record);

            const auto gun = shared_ctx.weapon_type >= cstypes::weapon_type::pistol &&
                shared_ctx.weapon_type <= cstypes::weapon_type::lmg;
            // Real direct hits already terminate the record scan. Avoid running
            // speculative movement physics for a pose that will never be read.
            // Distance-limited callers still need prediction before culling.
            const auto defer_extrapolation = gun && !(max_distance_sq > 0.0f);
            shared::lagcomp::record* predicted_record{};
            if (gun && !defer_extrapolation)
            {
                auto extrap = g_shared.lc().extrapolate(pawn);
                if (extrap)
                {
                    const_cast<rage*>(this)->m_extrapolated_records.push_back(std::move(*extrap));
                    predicted_record = &const_cast<rage*>(this)->m_extrapolated_records.back();
                }
            }

            // Distance cull includes the optional prediction and every real pose.
            if (max_distance_sq > 0.0f)
            {
                const auto& origin = systems::g_prediction.pre().origin;
                const auto real_in_range = std::any_of(records.begin(), records.end(), [&](const auto* rec)
                    {
                        return (rec->origin - origin).length_sqr() <= max_distance_sq;
                    });
                if (!real_in_range && (!predicted_record ||
                    (predicted_record->origin - origin).length_sqr() > max_distance_sq))
                    continue;
            }

            c.extrapolation_pending = defer_extrapolation;
            c.pawn = pawn;
            c.health = health;
            c.armor = memory::read<int>(pawn + SCHEMA("C_CSPlayerPawn", "m_ArmorValue"_hash));

            for (auto* record : records)
                c.records[c.record_count++] = record;
            // Keep speculative poses separate from the observed history.
            if (predicted_record)
                c.records[c.record_count++] = predicted_record;

            if (shared_ctx.weapon_type >= cstypes::weapon_type::pistol && shared_ctx.weapon_type <= cstypes::weapon_type::lmg)
            {
                const auto& config = settings::g_combat.m_ragebot.get_group(shared_ctx.weapon_type, shared_ctx.item_def_idx);
                c.min_damage = this->get_min_damage(config, health, config.min_damage_override.value);
            }

            out.push_back(std::move(c));
        }

        if (out.size() > 1)
        {
            const auto shoot_pos = g_shared.get_shoot_position();
            std::sort(out.begin(), out.end(), [&](const candidate& a, const candidate& b)
            {
                const auto pos_a = (a.record_count > 0 && a.records[0]) ? a.records[0]->origin : math::vector3{};
                const auto pos_b = (b.record_count > 0 && b.records[0]) ? b.records[0]->origin : math::vector3{};
                return (pos_a - shoot_pos).length_sqr() < (pos_b - shoot_pos).length_sqr();
            });
        }

        return out;
    }

    bool rage::run_gun(systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local, bool allow_fire)
    {
        if (!settings::g_combat.m_ragebot.enabled)
            return false;

        diag::exception_scope rage_scope{ "rage: run_gun / configuration" };
        auto& shared_ctx = g_shared.ctx();
        const auto& config = settings::g_combat.m_ragebot.get_group(shared_ctx.weapon_type, shared_ctx.item_def_idx);
        const auto autostop_enabled = config.autostop.value;

        diag::set_exception_phase("rage: run_gun / gather_candidates");
        auto candidates = this->gather_candidates(local);
        {
            std::lock_guard lock(m_debug_mtx);
            m_debug_points.clear();
        }

        if (candidates.empty())
            return false;

        diag::set_exception_phase("rage: run_gun / eye_candidates");
        auto eye_candidates = g_shared.sh().get_candidates();
        if (eye_candidates.count == 0)
        {
            eye_candidates.entries[0].position = g_shared.get_shoot_position();
            eye_candidates.entries[0].is_uninterpolated = true;
            eye_candidates.count = 1;
        }

        const auto scan_from_eye_candidates = [&](const math::vector3& eye_offset, float inaccuracy)
            {
                std::vector<scan_hit> hits_out;
                hits_out.reserve(candidates.size() * 24 * static_cast<std::size_t>(eye_candidates.count));
                for (auto i = 0; i < eye_candidates.count; ++i)
                {
                    const auto eye = eye_candidates.entries[i].position + eye_offset;
                    const auto first_hit = hits_out.size();
                    this->scan_players(eye, inaccuracy, ctx, candidates, local, hits_out);

                    auto source_eye = eye_candidates.entries[i];
                    source_eye.position = eye;
                    // Index after appending: vector growth cannot invalidate an
                    // iterator/reference to the previous eye's results.
                    for (auto h = first_hit; h < hits_out.size(); ++h)
                        hits_out[h].source_eye = source_eye;
                    // A direct center ray says nothing about hitchance from the
                    // other eye. Keep both candidates until probability ranking.
                }
                return hits_out;
            };

        diag::set_exception_phase("rage: run_gun / movement_and_selection");
        const auto& prestate = systems::g_prediction.pre();
        const auto duckpeek_active = settings::g_combat.m_duckpeek.enabled.value && ctx.on_ground;
        const auto movement_services = memory::read<std::uintptr_t>(local.pawn + SCHEMA("C_BasePlayerPawn", "m_pMovementServices"_hash));
        const auto duck_amount = movement_services ? memory::read<float>(movement_services + SCHEMA("CCSPlayer_MovementServices", "m_flDuckAmount"_hash)) : 0.0f;
        const auto is_ducking = (duck_amount > 0.05f) || ((prestate.flags & cstypes::entity_flags::ducking) != 0) || ((cmd->buttons.value & cstypes::command_buttons::in_duck) != 0);
        const auto effective_stand_z = is_ducking ? std::clamp(duck_amount * 18.0f, 0.0f, 18.0f) : 0.0f;

        if (duckpeek_active && this->m_duckpeek_reduck)
        {
            if ((this->m_duckpeek_reduck_ticks <= 0 && duck_amount >= 0.60f) || this->m_duckpeek_reduck_ticks <= -10)
                this->m_duckpeek_reduck = false;
        }

        // No-spread uses the same predicted state as scanning. fire_gun validates
        // the compensated trajectory instead of assuming compensation always hits.
        if (config.no_spread.value)
        {
            auto all_hits = scan_from_eye_candidates({}, shared_ctx.inaccuracy);
            auto best = all_hits.empty() ? target{} : this->select_best(ctx, all_hits, shared_ctx.inaccuracy);
            const auto had_target = best.valid;

            if (best.valid && allow_fire)
            {
                // A high-scoring point may have no seed-consistent solution for
                // this command. Try alternatives rather than stall on it forever.
                // Bound expensive solving/retracing even in a crowded scene.
                constexpr auto max_shot_attempts = 8;
                for (auto attempt = 0; attempt < max_shot_attempts && best.valid; ++attempt)
                {
                    this->fire_gun(cmd, best, false, best.hit.source_eye.position, local);
                    if (this->m_firing_this_tick)
                    {
                        if (duckpeek_active)
                        {
                            this->m_duckpeek_reduck = true;
                            this->m_duckpeek_reduck_ticks = 10;
                            this->m_release_duck_for_shot = false;
                        }
                        return true;
                    }
                    const auto rejected = best.hit;
                    std::erase_if(all_hits, [&](const scan_hit& hit)
                    {
                        return hit.pawn == rejected.pawn && hit.record == rejected.record &&
                            hit.position == rejected.position &&
                            hit.source_eye.position == rejected.source_eye.position &&
                            hit.source_eye.player_tick == rejected.source_eye.player_tick &&
                            hit.source_eye.player_frac == rejected.source_eye.player_frac &&
                            hit.source_eye.lerp_ticks_int == rejected.source_eye.lerp_ticks_int &&
                            hit.source_eye.lerp_ticks_frac == rejected.source_eye.lerp_ticks_frac &&
                            hit.source_eye.is_uninterpolated == rejected.source_eye.is_uninterpolated;
                    });
                    best = attempt + 1 < max_shot_attempts
                        ? this->select_best(ctx, all_hits, shared_ctx.inaccuracy) : target{};
                }
            }

            // Duckpeek standing scan
            if (duckpeek_active && is_ducking && !this->m_duckpeek_reduck)
            {
                const auto stand_offset = math::vector3{ 0.0f, 0.0f, effective_stand_z };
                auto standing_hits = scan_from_eye_candidates(stand_offset, shared_ctx.inaccuracy);
                const auto standing_best = standing_hits.empty() ? target{} : this->select_best(ctx, standing_hits, shared_ctx.inaccuracy);

                if (standing_best.valid)
                {
                    this->m_release_duck_for_shot = true;
                    if (autostop_enabled && this->should_stop_movement(ctx))
                        this->m_should_stop = true;
                }
                else
                {
                    this->m_release_duck_for_shot = false;
                }
            }
            else if (!duckpeek_active)
            {
                this->m_release_duck_for_shot = false;
            }
            // Compensation can fail at high movement inaccuracy. Reuse normal
            // stop planning instead of retrying the same moving shot indefinitely.
            if (allow_fire && autostop_enabled && this->should_stop_movement(ctx))
            {
                this->m_should_stop = had_target;
                if (!had_target)
                {
                    const auto primary_eye = eye_candidates.entries[0].position;
                    const auto stop = this->predict_stop(ctx, primary_eye, local);
                    if (stop)
                    {
                        const auto planned = scan_from_eye_candidates(stop->eye - primary_eye, stop->inaccuracy);
                        this->m_should_stop = this->select_best(ctx, planned, stop->inaccuracy).valid;
                    }
                }
            }
            return had_target;
        }

        // Standard hitchance-based logic
        const auto primary_eye = eye_candidates.entries[0].position;
        auto current_hits = scan_from_eye_candidates({}, ctx.predicted_inaccuracy);
        const auto best = this->select_best(ctx, current_hits, ctx.predicted_inaccuracy);

        // Auto-scope
        if (settings::g_combat.m_autos.scope.value && settings::g_combat.m_ragebot.enabled &&
            shared_ctx.weapon_type == cstypes::weapon_type::sniper && !shared_ctx.is_scoped &&
            best.valid && !(cmd->buttons.value & cstypes::command_buttons::in_second_attack))
        {
            cmd->buttons.value |= cstypes::command_buttons::in_second_attack;
            cmd->buttons.value_changed |= cstypes::command_buttons::in_second_attack;
            cmd->buttons.value_scroll |= cstypes::command_buttons::in_second_attack;
        }

        const auto needed_hc = config.hitchance_override.value ?
            static_cast<float>(config.hitchance_override_value) / 100.0f :
            static_cast<float>(config.hitchance) / 100.0f;

        const auto current_hc = best.valid ? best.hitchance : 0.0f;
        const auto accurate = best.valid && current_hc >= needed_hc;
        const auto max_acc = g_shared.is_max_accuracy(ctx.predicted_inaccuracy);
        const auto force = best.valid && (ctx.on_ground ? (config.force_shot.value && max_acc) : (config.force_shot_air.value && max_acc));
        const auto shot_viable = accurate || force;

        if (shot_viable && allow_fire)
        {
            this->fire_gun(cmd, best, !accurate && force, best.hit.source_eye.position, local);
            if (duckpeek_active && this->m_firing_this_tick)
            {
                this->m_duckpeek_reduck = true;
                this->m_duckpeek_reduck_ticks = 10;
                this->m_release_duck_for_shot = false;
            }
            return best.valid;
        }

        // Duckpeek logic: check if standing up reveals a better shot
        if (duckpeek_active && is_ducking && !this->m_duckpeek_reduck)
        {
            const auto standing_inaccuracy = this->get_standing_inaccuracy(local, ctx);
            const auto stand_offset = math::vector3{ 0.0f, 0.0f, effective_stand_z };
            auto standing_hits = scan_from_eye_candidates(stand_offset, standing_inaccuracy);
            const auto standing_best = standing_hits.empty() ? target{} : this->select_best(ctx, standing_hits, standing_inaccuracy);

            if (standing_best.valid)
            {
                // select_best used this same eye, pose and standing inaccuracy.
                const auto pred_accurate = standing_best.hitchance >= needed_hc;
                const auto pred_max_acc = g_shared.is_max_accuracy(standing_inaccuracy);
                const auto pred_force = config.force_shot.value && pred_max_acc;

                if (pred_accurate || pred_force)
                {
                    this->m_release_duck_for_shot = true;
                    if (autostop_enabled && this->should_stop_movement(ctx))
                        this->m_should_stop = true;

                    if (settings::g_combat.m_autos.scope.value && settings::g_combat.m_ragebot.enabled &&
                        shared_ctx.weapon_type == cstypes::weapon_type::sniper && !shared_ctx.is_scoped &&
                        !(cmd->buttons.value & cstypes::command_buttons::in_second_attack))
                    {
                        cmd->buttons.value |= cstypes::command_buttons::in_second_attack;
                        cmd->buttons.value_changed |= cstypes::command_buttons::in_second_attack;
                        cmd->buttons.value_scroll |= cstypes::command_buttons::in_second_attack;
                    }
                    return best.valid;
                }
            }
            this->m_release_duck_for_shot = false;
        }
        else if (!duckpeek_active)
        {
            this->m_release_duck_for_shot = false;
        }

        // Autostop planning
        if (autostop_enabled && !shot_viable && this->should_stop_movement(ctx))
        {
            const auto stop = this->predict_stop(ctx, primary_eye, local);
            if (stop)
            {
                const auto future_offset = stop->eye - primary_eye;
                auto planned_hits = scan_from_eye_candidates(future_offset, stop->inaccuracy);
                const auto planned = this->select_best(ctx, planned_hits, stop->inaccuracy);
                this->m_should_stop = planned.valid;
            }
            else
            {
                this->m_should_stop = best.valid;
            }
        }

        return best.valid;
    }

    void rage::run_taser(systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local)
    {
        if (!settings::g_combat.m_zeusbot.enabled)
            return;

        auto candidates = this->gather_candidates(local);
        if (candidates.empty())
            return;

        auto eye_candidates = g_shared.sh().get_candidates();
        if (eye_candidates.count == 0)
        {
            eye_candidates.entries[0].position = g_shared.get_shoot_position();
            eye_candidates.entries[0].is_uninterpolated = true;
            eye_candidates.count = 1;
        }

        std::vector<scan_hit> all_hits;
        for (auto i = 0; i < eye_candidates.count; ++i)
        {
            auto hits = this->scan_taser(eye_candidates.entries[i].position, ctx, candidates, local);
            for (auto& h : hits)
            {
                h.source_eye = eye_candidates.entries[i];
                all_hits.push_back(std::move(h));
            }
        }

        if (all_hits.empty())
            return;

        target best{};
        for (const auto& h : all_hits)
        {
            if (!best.valid || h.score > best.score)
            {
                best.hit = h;
                best.hitchance = 1.0f;
                best.score = h.score;
                best.valid = true;
            }
        }

        if (best.valid)
        {
            this->m_zeus_fired = true;
            this->fire_melee(cmd, best, local);
        }
    }

    void rage::run_knife(systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local)
    {
        if (!settings::g_combat.m_knifebot.enabled)
            return;

        const auto info = this->get_knife_info(local);
        if (!info.can_slash && !info.can_stab)
            return;

        constexpr auto max_knife_dist_sq = 150.0f * 150.0f;
        auto candidates = this->gather_candidates(local, max_knife_dist_sq);
        if (candidates.empty())
            return;

        auto eye_candidates = g_shared.sh().get_candidates();
        if (eye_candidates.count == 0)
        {
            eye_candidates.entries[0].position = g_shared.get_shoot_position();
            eye_candidates.entries[0].is_uninterpolated = true;
            eye_candidates.count = 1;
        }

        std::vector<scan_hit> all_hits;
        for (auto i = 0; i < eye_candidates.count; ++i)
        {
            auto hits = this->scan_knife(eye_candidates.entries[i].position, ctx, info, candidates, local);
            for (auto& h : hits)
            {
                h.source_eye = eye_candidates.entries[i];
                all_hits.push_back(std::move(h));
            }
        }

        if (all_hits.empty())
            return;

        target best{};
        target best_backstab{};

        for (const auto& h : all_hits)
        {
            auto& dest = h.is_backstab ? best_backstab : best;
            if (!dest.valid || h.score > dest.score)
            {
                dest.hit = h;
                dest.hitchance = 1.0f;
                dest.score = h.score;
                dest.valid = true;
            }
        }

        auto& chosen = best_backstab.valid ? best_backstab : best;
        if (!chosen.valid)
            return;

        this->m_knife_attack = static_cast<std::uint8_t>(chosen.hit.attack_type);
        this->fire_melee(cmd, chosen, local);
    }

    void rage::auto_revolver(systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local)
    {
        if (!settings::g_combat.m_ragebot.enabled)
        {
            this->m_revolver_cock_ticks = 0;
            return;
        }

        if (!g_shared.can_shoot(cmd, local.controller))
        {
            this->m_revolver_cock_ticks = 0;
            return;
        }

        if (!settings::g_combat.m_autos.revolver.value)
        {
            this->m_revolver_cock_ticks = 0;
            return;
        }

        constexpr auto cock_ticks{ 13 };
        if (this->m_revolver_cock_ticks >= cock_ticks)
        {
            // Fire!
            cmd->buttons.value &= ~cstypes::command_buttons::in_attack;
            cmd->buttons.value_changed |= cstypes::command_buttons::in_attack;
            cmd->buttons.value_scroll &= ~cstypes::command_buttons::in_attack;
            cmd->csgo_user_cmd.set_attack1_start_history_index(-1);

            this->m_revolver_cock_ticks = 0;
            this->run_gun(cmd, ctx, local);
            return;
        }

        // Keep holding attack to cock the hammer
        this->run_gun(cmd, ctx, local, false);

        cmd->buttons.value |= cstypes::command_buttons::in_attack;
        cmd->buttons.value_changed |= cstypes::command_buttons::in_attack;
        cmd->buttons.value_scroll |= cstypes::command_buttons::in_attack;

        const auto history_index = cmd->csgo_user_cmd.input_history_size() - 1;
        if (history_index >= 0)
            cmd->csgo_user_cmd.set_attack1_start_history_index(history_index);

        ++this->m_revolver_cock_ticks;
    }

    void rage::scan_players(const math::vector3& eye, float inaccuracy, const aim_context& ctx, std::vector<candidate>& candidates, const systems::local::snapshot& local, std::vector<scan_hit>& results) const
    {
        diag::exception_scope scan_scope{ "rage: scan_players / dispatch" };

        // Append in the original eye/candidate/record/point order. Only immutable
        // pose preparation is cached; each distinct eye still gets its own rays.
        for (auto& cand : candidates)
        {
            if (cand.prepared_records.empty())
                cand.prepared_records.reserve(static_cast<std::size_t>(cand.record_count) +
                    (cand.extrapolation_pending ? 1u : 0u));
            for (auto ri = 0; ri <= cand.record_count; ++ri)
            {
                if (ri == cand.record_count)
                {
                    if (!cand.extrapolation_pending ||
                        cand.record_count >= static_cast<int>(cand.records.size()))
                        break;
                    cand.extrapolation_pending = false;
                    auto extrap = g_shared.lc().extrapolate(cand.pawn);
                    if (!extrap)
                        break;

                    // gather_candidates reserves one slot per player. At most
                    // one pose is appended per candidate, keeping pointers stable.
                    auto& predicted = const_cast<rage*>(this)->m_extrapolated_records;
                    predicted.push_back(std::move(*extrap));
                    cand.records[cand.record_count++] = &predicted.back();
                }

                if (!cand.records[ri] || !cand.records[ri]->valid)
                    continue;

                if (cand.prepared_records.size() <= static_cast<std::size_t>(ri))
                    cand.prepared_records.resize(static_cast<std::size_t>(ri) + 1);
                auto& prepared = cand.prepared_records[ri];
                if (prepared.record != cand.records[ri] || prepared.target_pawn != cand.pawn)
                {
                    diag::set_exception_phase("rage: scan_players / prepare_target");
                    prepared = g_shared.pen().prepare_target(cand.pawn, cand.records[ri]);
                }
                this->scan_player(eye, inaccuracy, ctx, cand, prepared, local, results);
            }
            // Do not compare previous players' damage against this player's HP,
            // or discard another pose before its hit probability is evaluated.
        }
    }

    void rage::scan_player(const math::vector3& eye, float inaccuracy, const aim_context& ctx, const candidate& cand, const shared::penetration::run_context& pen_ctx, const systems::local::snapshot& local, std::vector<scan_hit>& results) const
    {
        diag::exception_scope player_scope{ "rage: scan_player / validation" };
        auto* record = pen_ctx.record;
        if (!cand.pawn || cand.record_count <= 0 || cand.health <= 0 ||
            pen_ctx.target_pawn != cand.pawn || !record || !record->valid || record->bone_count <= 0)
            return;

        const auto& shared_ctx = g_shared.ctx();
        const auto& config = settings::g_combat.m_ragebot.get_group(shared_ctx.weapon_type, shared_ctx.item_def_idx);

        // Preparation belongs to this run_gun invocation, never another command.
        const auto& hitbox_set = pen_ctx.hitboxes;
        if (hitbox_set.count <= 0)
            return;
        const auto hitboxes = ballistics::indexed_view<systems::hitboxes::entry, 20>{
            std::span<const systems::hitboxes::entry>{hitbox_set.entries.data(),
                static_cast<std::size_t>(hitbox_set.count)}};

        const auto bone_count = std::min(record->bone_count, static_cast<int>(std::size(record->bones)));
        const auto& skeleton = record->bones;
        diag::set_exception_phase("rage: scan_player / multipoints");

        const auto force_body = config.body_aim.value;
        std::array<int, 19> scan_order{};
        auto scan_count{ 0 };

        // Build scan order based on config
        if (!force_body && config.hitboxes.values[0])
            scan_order[scan_count++] = 0; // Head

        if (config.hitboxes.values[1])
        {
            scan_order[scan_count++] = 4; // Chest
            scan_order[scan_count++] = 5;
            scan_order[scan_count++] = 6;
        }

        if (config.hitboxes.values[2])
        {
            scan_order[scan_count++] = 3; // Stomach
            scan_order[scan_count++] = 2;
        }

        if (config.hitboxes.values[3])
        {
            for (auto idx : { 13, 14, 15, 16, 17, 18 }) // Arms
                scan_order[scan_count++] = idx;
        }

        if (config.hitboxes.values[4])
        {
            for (auto idx : { 7, 8, 9, 10 }) // Legs
                scan_order[scan_count++] = idx;
        }

        if (config.hitboxes.values[5])
        {
            for (auto idx : { 11, 12 }) // Feet
                scan_order[scan_count++] = idx;
        }

        // Fallback if nothing selected
        if (scan_count == 0)
        {
            if (!force_body)
                scan_order[scan_count++] = 0;
            for (auto idx : { 4, 5, 6, 3, 2 })
                scan_order[scan_count++] = idx;
        }

        struct candidate_point
        {
            math::vector3 position;
            int hitbox_index;
            int bone_index;
            systems::hitboxes::entry hitbox;
            bool is_center;
            math::vector3 aim_angle;
            float fov;
        };

        std::array<candidate_point, 19> center_storage{};
        std::size_t center_count{};
        std::vector<debug_point> debug_points;
        if (config.debug_multipoints.value)
            debug_points.reserve(static_cast<std::size_t>(scan_count) * 4);

        for (auto idx = 0; idx < scan_count; ++idx)
        {
            const auto hitbox_index = scan_order[idx];
            const auto* hb = hitboxes.find(hitbox_index);

            if (!hb || hb->bone < 0 || hb->bone >= bone_count)
                continue;

            const auto& bone = skeleton[hb->bone];
            if (bone.position.length_sqr() < 1.0f)
                continue;

            const auto hitbox_center = (hb->mins + hb->maxs) * 0.5f;
            const auto center = bone.rotation.rotate_vector(hitbox_center) + bone.position;

            const auto aim = math::helpers::calculate_angle(eye, center);
            const auto fov = math::helpers::angle_distance(ctx.view_angles, aim);
            if (fov > config.max_fov)
                continue;

            center_storage[center_count++] = { center, hitbox_index, hb->bone, *hb, true, aim, fov };
            if (config.debug_multipoints.value)
                debug_points.push_back({ center, hitbox_index, true });
        }

        if (center_count == 0)
            return;
        const auto centers = std::span<const candidate_point>{center_storage.data(), center_count};

        diag::set_exception_phase("rage: scan_player / penetration");
        std::array<bool, 19> center_sufficient{};

        // Phase 1: Test all hitbox centers first
        for (const auto& cp : centers)
        {
            shared::penetration::result pen{};
            if (!g_shared.pen().run(eye, cp.position, pen_ctx, local.pawn, local.team, pen) || pen.damage <= 0.0f)
            {
                continue;
            }

            if (pen.damage < cand.min_damage)
                continue;

            const auto* actual_hitbox = hitboxes.find(pen.hitbox);
            if (!actual_hitbox || actual_hitbox->bone < 0 || actual_hitbox->bone >= bone_count)
                continue;

            const auto actual_center = (pen.hitbox == cp.hitbox_index);
            const auto is_lethal = (pen.damage >= static_cast<float>(cand.health));
            if (actual_center && cp.hitbox_index >= 0 && cp.hitbox_index < static_cast<int>(center_sufficient.size()))
                center_sufficient[cp.hitbox_index] = !pen.penetrated || is_lethal;

            scan_hit h{};
            h.position = cp.position;
            h.aim_angle = cp.aim_angle;
            h.damage = pen.damage;
            h.fov = cp.fov;
            h.hitbox_index = actual_hitbox->index;
            h.hitgroup = pen.hitgroup;
            h.bone_index = actual_hitbox->bone;
            h.hitbox = *actual_hitbox;
            h.is_center = actual_center;
            h.penetrated = pen.penetrated;
            h.pawn = cand.pawn;
            h.health = cand.health;
            h.record = record;
            results.push_back(h);
        }

        // Closed centers do not imply closed edges: narrow cover is precisely
        // where multipoints are useful. Keep per-hitbox center pruning only.
        if (config.pointscale > 0.0f)
        {
            std::array<math::vector3, 3> multipoints{};
            // Inaccuracy/spread are fixed for this eye pass. Evaluate tan once,
            // not once per hitbox; nullopt preserves disabled dynamic pointscale.
            // Compensated shots are validated against their actual seed later;
            // the uncompensated cone must not erase exposed edge candidates.
            const auto cone_tangent = config.dynamic_pointscale.value && !config.no_spread.value
                ? std::optional<float>{std::tanf(std::max(inaccuracy + shared_ctx.spread, 0.0f))}
                : std::nullopt;

            for (const auto& cp : centers)
            {
                // Bound edge traces to the selected head and torso hitboxes.
                if (cp.hitbox_index != 0 && cp.hitbox_index != 2 && cp.hitbox_index != 3 && cp.hitbox_index != 4 && cp.hitbox_index != 5 && cp.hitbox_index != 6)
                    continue;

                // A reachable center may have no seed-consistent solution.
                // Preserve edges for the bounded nospread alternative-shot pass.
                if (!config.no_spread.value && cp.hitbox_index >= 0 &&
                    cp.hitbox_index < static_cast<int>(center_sufficient.size()) && center_sufficient[cp.hitbox_index])
                    continue;

                const auto& bone = skeleton[cp.bone_index];
                const auto point_count = this->generate_multipoints(cp.hitbox, cp.position,
                    bone.rotation, config.pointscale, eye, cone_tangent, multipoints);

                for (const auto& mp : std::span<const math::vector3>{multipoints.data(), point_count})
                {
                    const auto aim = math::helpers::calculate_angle(eye, mp);
                    const auto fov = math::helpers::angle_distance(ctx.view_angles, aim);
                    if (fov > config.max_fov)
                        continue;

                    shared::penetration::result pen{};
                    if (!g_shared.pen().run(eye, mp, pen_ctx, local.pawn, local.team, pen))
                        continue;

                    if (pen.damage < cand.min_damage)
                        continue;

                    if (cp.hitbox_index == 0 && pen.hitgroup != systems::g_hitboxes.hitgroup_from_hitbox(0))
                        continue;

                    const auto* actual_hitbox = hitboxes.find(pen.hitbox);
                    if (!actual_hitbox || actual_hitbox->bone < 0 || actual_hitbox->bone >= bone_count)
                        continue;

                    if (config.debug_multipoints.value)
                        debug_points.push_back({ mp, cp.hitbox_index, false });

                    scan_hit h{};
                    h.position = mp;
                    h.aim_angle = aim;
                    h.damage = pen.damage;
                    h.fov = fov;
                    h.hitbox_index = actual_hitbox->index;
                    h.hitgroup = pen.hitgroup;
                    h.bone_index = actual_hitbox->bone;
                    h.hitbox = *actual_hitbox;
                    h.is_center = false;
                    h.penetrated = pen.penetrated;
                    h.pawn = cand.pawn;
                    h.health = cand.health;
                    h.record = record;
                    results.push_back(h);

                    if (!config.no_spread.value && pen.damage >= static_cast<float>(cand.health))
                        break;
                }
            }
        }

        if (!debug_points.empty())
        {
            std::lock_guard lock(m_debug_mtx);
            m_debug_points.insert(m_debug_points.end(), debug_points.begin(), debug_points.end());
        }

    }

    rage::target rage::select_best(const aim_context& aim_ctx, const std::vector<scan_hit>& hits, float eval_inaccuracy) const
    {
        if (hits.empty())
            return {};

        auto hitgroup_priority = [](int hitbox_index) -> int
            {
                if (hitbox_index == 0) return 4; // Head
                if (hitbox_index >= 1 && hitbox_index <= 6) return 3; // Torso
                if (hitbox_index >= 13 && hitbox_index <= 18) return 2; // Arms
                if (hitbox_index >= 7 && hitbox_index <= 12) return 1; // Legs
                return 0;
            };

        // No probability queries or record grouping are needed for nospread.
        // This path is also used by every alternative-shot retry.
        const auto& fast_config = settings::g_combat.m_ragebot.get_group(g_shared.ctx().weapon_type, g_shared.ctx().item_def_idx);
        if (fast_config.no_spread.value)
        {
            target best{};
            for (const auto& hit : hits)
            {
                if (!hit.record || !hit.record->valid || hit.bone_index < 0 ||
                    hit.bone_index >= hit.record->bone_count ||
                    hit.bone_index >= static_cast<int>(std::size(hit.record->bones)) ||
                    hit.health <= 0 || !std::isfinite(hit.damage) || hit.damage <= 0.0f ||
                    !std::isfinite(hit.fov) || !ballistics::finite(hit.aim_angle) ||
                    !ballistics::finite(hit.source_eye.position))
                    continue;
                const auto score = ballistics::target_score(hit.damage, hit.health, 1.0f, 0.0f,
                    true, hit.penetrated, hit.is_center, hitgroup_priority(hit.hitbox_index), hit.fov);
                auto better = !best.valid || score > best.score;
                if (best.valid && std::fabsf(score - best.score) < 0.01f)
                {
                    if (hit.record->tick != best.hit.record->tick)
                        better = hit.record->tick > best.hit.record->tick;
                    else if (hit.is_center != best.hit.is_center)
                        better = hit.is_center;
                    else
                        better = hit.fov < best.hit.fov;
                }
                if (better)
                {
                    best.hit = hit;
                    best.hitchance = 1.0f;
                    best.score = score;
                    best.valid = true;
                }
            }
            return best;
        }

        struct record_group
        {
            shared::lagcomp::record* record;
            std::vector<int> hit_indices;
        };

        std::vector<record_group> groups;
        groups.reserve(16);
        std::unordered_map<shared::lagcomp::record*, std::size_t> group_indices;
        group_indices.reserve(16);

        for (auto i = 0; i < static_cast<int>(hits.size()); ++i)
        {
            auto rec = hits[i].record;
            const auto [it, inserted] = group_indices.try_emplace(rec, groups.size());
            if (inserted)
            {
                record_group g{};
                g.record = rec;
                g.hit_indices.reserve(16);
                groups.push_back(std::move(g));
            }
            // Iterate the vector, never the hash map, to preserve first-seen
            // record order and all existing near-tie selection semantics.
            groups[it->second].hit_indices.push_back(i);
        }

        // Keep every valid hit; a damage-only top-eight shortlist can remove
        // the sole point meeting hitchance. Prune by a score bound instead.

        // Select as we evaluate, preserving traversal order and tie-breaking
        // without an intermediate allocation or a second pass over the hits.
        target best{};

        const auto& config = settings::g_combat.m_ragebot.get_group(g_shared.ctx().weapon_type, g_shared.ctx().item_def_idx);
        const auto needed_hc = config.hitchance_override.value ?
            static_cast<float>(config.hitchance_override_value) / 100.0f :
            static_cast<float>(config.hitchance) / 100.0f;

        const auto score_for = [&](const scan_hit& hit, float hc)
        {
            return ballistics::target_score(hit.damage, hit.health, hc, needed_hc,
                config.no_spread.value, hit.penetrated, hit.is_center,
                hitgroup_priority(hit.hitbox_index), hit.fov);
        };
        const auto valid_hit = [](const scan_hit& hit)
        {
            return hit.health > 0 && std::isfinite(hit.damage) && hit.damage > 0.0f &&
                std::isfinite(hit.fov) && ballistics::finite(hit.aim_angle) &&
                ballistics::finite(hit.source_eye.position);
        };

        // Sort compact keys, not copies of every hitbox, bone and angle.
        // Evaluation is synchronous; snapshot the bone only for evaluated hits.
        struct hitchance_query
        {
            int hit_index{};
            float upper_score{};
        };

        std::vector<float> hitchances;
        if (!config.no_spread.value)
        {
            std::vector<hitchance_query> queries;
            queries.reserve(hits.size());
            for (const auto& group : groups)
            {
                if (!group.record || !group.record->valid)
                    continue;
                for (const auto idx : group.hit_indices)
                {
                    const auto& h = hits[idx];
                    if (!valid_hit(h) || !h.record || !h.record->valid || h.bone_index < 0 ||
                        h.bone_index >= h.record->bone_count ||
                        h.bone_index >= static_cast<int>(std::size(h.record->bones)))
                        continue;
                    queries.push_back({ idx, score_for(h, 1.0f) });
                }
            }
            if (queries.empty())
                return {};

            // Engine spread generation still runs once on the caller.
            const auto cache = g_shared.build_spread_cache(eval_inaccuracy, aim_ctx.spread);
            const auto range = g_shared.ctx().range;
            hitchances.assign(hits.size(), -1.0f);
            std::sort(queries.begin(), queries.end(), [](const auto& a, const auto& b)
            {
                return a.upper_score != b.upper_score ? a.upper_score > b.upper_score
                    : a.hit_index < b.hit_index;
            });
            struct worker_query
            {
                math::vector3 eye;
                math::vector3 angle;
                systems::hitboxes::entry hitbox;
                systems::bones::data bone;
                float result{};
            };
            constexpr std::size_t batch_size = 32;
            std::array<worker_query, batch_size> batch;
            auto best_evaluated_score = -std::numeric_limits<float>::infinity();
            for (std::size_t first = 0; first < queries.size();)
            {
                if (queries[first].upper_score + 0.01f < best_evaluated_score)
                    break;
                // Establish a useful pruning bound before queuing a large batch.
                const auto limit = first == 0 ? std::size_t{1} : batch_size;
                std::size_t count = 0;
                while (count < limit && first + count < queries.size() &&
                    queries[first + count].upper_score + 0.01f >= best_evaluated_score)
                {
                    const auto& hit = hits[queries[first + count].hit_index];
                    batch[count++] = {hit.source_eye.position, hit.aim_angle, hit.hitbox,
                        hit.record->bones[hit.bone_index], 0.0f};
                }
                const auto evaluate = [&](int begin, int end)
                {
                    for (auto i = begin; i < end; ++i)
                    {
                        auto& query = batch[i];
                        query.result = g_shared.calculate_hitchance(query.eye, query.angle,
                            query.hitbox, query.bone, cache, range);
                    }
                };
                // Small batches stay on the caller; workers see no pawn, record,
                // settings or engine RNG. parallel_for joins before reduction.
                if (count >= 16 && cache.count >= 64)
                    threadpool::parallel_for(0, static_cast<int>(count), evaluate, 8);
                else
                    evaluate(0, static_cast<int>(count));
                for (std::size_t i = 0; i < count; ++i)
                {
                    const auto index = queries[first + i].hit_index;
                    hitchances[index] = batch[i].result;
                    best_evaluated_score = std::max(best_evaluated_score,
                        score_for(hits[index], batch[i].result));
                }
                first += count;
            }
        }

        for (auto& group : groups)
        {
            if (!group.record || !group.record->valid)
                continue;

            for (const auto idx : group.hit_indices)
            {
                const auto& h = hits[idx];
                if (!valid_hit(h) || !h.record || !h.record->valid || h.bone_index < 0 ||
                    h.bone_index >= h.record->bone_count ||
                    h.bone_index >= static_cast<int>(std::size(h.record->bones)))
                    continue;

                const auto hc = config.no_spread.value ? 1.0f : hitchances[idx];
                if (hc < 0.0f)
                    continue;
                const auto score = score_for(h, hc);

                auto is_better = !best.valid || score > best.score;
                if (best.valid && std::fabsf(score - best.score) < 0.01f)
                {
                    if (h.record->tick != best.hit.record->tick)
                        is_better = h.record->tick > best.hit.record->tick;
                    else if (h.is_center != best.hit.is_center)
                        is_better = h.is_center;
                    else
                        is_better = h.fov < best.hit.fov;
                }

                if (is_better)
                {
                    best.hit = h;
                    best.hitchance = hc;
                    best.score = score;
                    best.valid = true;
                }
            }
        }
        return best;
    }

    float rage::evaluate_hitchance(const scan_hit& hit, const aim_context& ctx, float inaccuracy) const
    {
        if (!hit.record || !hit.record->valid || hit.bone_index < 0 ||
            hit.bone_index >= hit.record->bone_count ||
            hit.bone_index >= static_cast<int>(std::size(hit.record->bones)))
            return 0.0f;

        return g_shared.calculate_hitchance(hit.source_eye.position, hit.aim_angle, hit.hitbox, hit.record->bones[hit.bone_index], inaccuracy, ctx.spread);
    }

    float rage::get_standing_inaccuracy(const systems::local::snapshot& local, const aim_context& ctx) const
    {
        const auto& prestate = systems::g_prediction.pre();
        auto velocity = prestate.networked_velocity;
        velocity.z = 0.0f;
        const auto speed = velocity.length_2d();

        if (speed > ctx.accurate_threshold)
            return g_shared.get_inaccuracy_at_velocity(local.pawn, velocity);

        const auto& shared_ctx = g_shared.ctx();
        if (!shared_ctx.weapon_vdata)
            return ctx.predicted_inaccuracy;

        const auto inaccuracy_stand = memory::read<float>(shared_ctx.weapon_vdata + SCHEMA("CCSWeaponBaseVData", "m_flInaccuracyStand"_hash));
        return std::max(inaccuracy_stand, g_shared.get_inaccuracy_at_velocity(local.pawn, velocity));
    }

    std::vector<rage::scan_hit> rage::scan_taser(const math::vector3& eye, const aim_context& ctx, std::vector<candidate>& candidates, const systems::local::snapshot& local) const
    {
        const auto& shared_ctx = g_shared.ctx();
        std::vector<scan_hit> results;

        for (auto& cand : candidates)
        {
            for (auto ri = 0; ri < cand.record_count; ++ri)
            {
                auto* record = cand.records[ri];
                if (!record || !record->valid)
                    continue;

                const auto game_scene_node = memory::read<std::uintptr_t>(cand.pawn + SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash));
                if (!game_scene_node)
                    continue;

                const auto hitbox_set = systems::g_hitboxes.query(game_scene_node);
                if (hitbox_set.count <= 0)
                    continue;

                record->apply();
                const auto skeleton = g_shared.lc().get_skeleton(*record);

                for (auto i = 0; i < hitbox_set.count; ++i)
                {
                    const auto& hb = hitbox_set.entries[i];
                    if (hb.bone < 0 ||
                        hb.bone >= static_cast<int>(skeleton.size()) ||
                        hb.bone >= record->bone_count)
                        continue;

                    const auto& bone = skeleton[hb.bone];
                    if (bone.position.length_sqr() < 1.0f)
                        continue;

                    const auto center = bone.rotation.rotate_vector((hb.mins + hb.maxs) * 0.5f) + bone.position;
                    const auto aim = math::helpers::calculate_angle(eye, center);
                    const auto fov = math::helpers::angle_distance(ctx.view_angles, aim);

                    if (fov > settings::g_combat.m_zeusbot.max_fov)
                        continue;

                    math::vector3 forward{};
                    math::helpers::angle_vectors_left(aim, &forward);
                    const auto trace = this->trace_taser_hit(eye, forward, shared_ctx.range * 0.85f, cand.pawn, local.pawn);

                    if (trace.hit_entity != cand.pawn)
                        continue;

                    const auto dist = (center - eye).length();
                    const auto range_fraction = dist / shared_ctx.range;

                    scan_hit h{};
                    h.position = center;
                    h.aim_angle = aim;
                    h.damage = 500.0f;
                    h.score = (10000.0f - dist) * (range_fraction > 0.92f ? 0.8f : 1.0f);
                    h.fov = fov;
                    h.hitbox_index = hb.index;
                    h.hitgroup = systems::g_hitboxes.hitgroup_from_hitbox(hb.index);
                    h.bone_index = hb.bone;
                    h.hitbox = hb;
                    h.is_center = true;
                    h.pawn = cand.pawn;
                    h.health = cand.health;
                    h.record = record;
                    results.push_back(h);
                }
                record->restore();
            }
        }
        return results;
    }

    rage::knife_info rage::get_knife_info(const systems::local::snapshot& local) const
    {
        const auto& shared_ctx = g_shared.ctx();
        const auto tick_base = memory::read<int>(local.controller + SCHEMA("CBasePlayerController", "m_nTickBase"_hash));
        const auto next_primary = memory::read<int>(shared_ctx.weapon + SCHEMA("C_BasePlayerWeapon", "m_nNextPrimaryAttackTick"_hash));
        const auto next_secondary = memory::read<int>(shared_ctx.weapon + SCHEMA("C_BasePlayerWeapon", "m_nNextSecondaryAttackTick"_hash));
        const auto last_shot_time = memory::read<float>(shared_ctx.weapon + SCHEMA("C_CSWeaponBase", "m_fLastShotTime"_hash));
        const auto cur_time = static_cast<float>(tick_base) * cstypes::tick_interval;

        return knife_info
        {
            .can_slash = tick_base >= next_primary,
            .can_stab = tick_base >= next_secondary,
            .charged = (cur_time - last_shot_time) > 0.4f,
            .armor_ratio = memory::read<float>(shared_ctx.weapon_vdata + SCHEMA("CCSWeaponBaseVData", "m_flArmorRatio"_hash))
        };
    }

    std::vector<rage::scan_hit> rage::scan_knife(const math::vector3& eye, const aim_context& ctx, const knife_info& info, std::vector<candidate>& candidates, const systems::local::snapshot& local) const
    {
        constexpr auto stab_range{ 50.0f };
        constexpr auto slash_range{ 66.0f };
        std::vector<scan_hit> results;

        for (auto& cand : candidates)
        {
            const auto eye_angles = memory::read<math::vector3>(cand.pawn + SCHEMA("C_CSPlayerPawn", "m_angEyeAngles"_hash));
            const auto hp = static_cast<float>(cand.health);
            const auto frontal_slash_dmg = this->get_knife_damage(info.charged ? 40.0f : 25.0f, cand.armor, info.armor_ratio);
            const auto frontal_stab_dmg = this->get_knife_damage(65.0f, cand.armor, info.armor_ratio);
            const auto frontal_can_kill = (info.can_slash && frontal_slash_dmg >= hp) || (info.can_stab && frontal_stab_dmg >= hp);

            for (auto ri = 0; ri < cand.record_count; ++ri)
            {
                auto* record = cand.records[ri];
                if (!record || !record->valid)
                    continue;

                const auto game_scene_node = memory::read<std::uintptr_t>(cand.pawn + SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash));
                if (!game_scene_node)
                    continue;

                const auto hitbox_set = systems::g_hitboxes.query(game_scene_node);
                if (hitbox_set.count <= 0)
                    continue;

                record->apply();
                const auto skeleton = g_shared.lc().get_skeleton(*record);

                auto backstab{ false };
                {
                    const auto delta = record->origin - systems::g_prediction.pre().origin;
                    const auto dist_2d = std::sqrtf(delta.x * delta.x + delta.y * delta.y);
                    if (dist_2d > 0.001f)
                    {
                        const auto dir_x = delta.x / dist_2d;
                        const auto dir_y = delta.y / dist_2d;
                        math::vector3 body_forward{};
                        math::helpers::angle_vectors_left(record->rotation, &body_forward);
                        math::vector3 eye_forward{};
                        math::helpers::angle_vectors_left(eye_angles, &eye_forward);

                        backstab = (dir_x * body_forward.x + dir_y * body_forward.y) > 0.475f ||
                            (dir_x * eye_forward.x + dir_y * eye_forward.y) > 0.475f;
                    }
                }

                const auto wait_for_backstab = backstab && !frontal_can_kill;

                for (auto i = 0; i < hitbox_set.count; ++i)
                {
                    const auto& hb = hitbox_set.entries[i];
                    if (hb.bone < 0 ||
                        hb.bone >= static_cast<int>(skeleton.size()) ||
                        hb.bone >= record->bone_count)
                        continue;

                    const auto& bone = skeleton[hb.bone];
                    if (bone.position.length_sqr() < 1.0f)
                        continue;

                    const auto center = bone.rotation.rotate_vector((hb.mins + hb.maxs) * 0.5f) + bone.position;
                    const auto dist = (center - eye).length();
                    const auto max_reach = info.can_slash ? slash_range : stab_range;

                    if (dist > max_reach)
                        continue;

                    const auto aim = math::helpers::calculate_angle(eye, center);
                    const auto fov = math::helpers::angle_distance(ctx.view_angles, aim);

                    if (fov > settings::g_combat.m_knifebot.max_fov)
                        continue;

                    math::vector3 forward{};
                    math::helpers::angle_vectors_left(aim, &forward);

                    for (const auto try_stab : { true, false })
                    {
                        if (try_stab && !info.can_stab)
                            continue;
                        if (!try_stab && !info.can_slash)
                            continue;

                        const auto reach = try_stab ? stab_range : slash_range;
                        if (dist > reach)
                            continue;

                        const auto raw_dmg = try_stab ? (backstab ? 180.0f : 65.0f) : (backstab ? 90.0f : (info.charged ? 40.0f : 25.0f));
                        const auto damage = this->get_knife_damage(raw_dmg, cand.armor, info.armor_ratio);
                        const auto can_kill = damage >= hp;

                        if (wait_for_backstab && !can_kill)
                            continue;

                        const auto trace = this->trace_knife_hit(eye, forward, reach, cand.pawn, local.pawn);
                        if (trace.hit_entity != cand.pawn)
                            continue;

                        const auto reach_margin = 1.0f - (dist / reach);
                        scan_hit h{};
                        h.position = center;
                        h.aim_angle = aim;
                        h.damage = damage;
                        h.score = can_kill ? (10000.0f + damage * reach_margin) : (damage * 100.0f * reach_margin);
                        h.fov = fov;
                        h.hitbox_index = hb.index;
                        h.hitgroup = systems::g_hitboxes.hitgroup_from_hitbox(hb.index);
                        h.bone_index = hb.bone;
                        h.hitbox = hb;
                        h.is_center = true;
                        h.is_backstab = backstab;
                        h.attack_type = try_stab ? 1 : 0;
                        h.pawn = cand.pawn;
                        h.health = cand.health;
                        h.record = record;
                        results.push_back(h);
                        break;
                    }
                }
                record->restore();
            }
        }
        return results;
    }

    void rage::fire_gun(systems::input::usercmd* cmd, const target& tgt, bool was_forced, const math::vector3& shoot_eye, const systems::local::snapshot& local)
    {
        this->m_firing_this_tick = false;
        if (!cmd || !tgt.hit.record || !tgt.hit.record->valid)
            return;

        const auto base = cmd->csgo_user_cmd.mutable_base();
        const auto history_size = cmd->csgo_user_cmd.input_history_size();
        if (!base || !base->mutable_viewangles() || history_size <= 0 ||
            !cmd->csgo_user_cmd.mutable_input_history(history_size - 1))
            return;

        const auto tick_base = memory::read<int>(local.controller + SCHEMA("CBasePlayerController", "m_nTickBase"_hash));
        const auto& shared_ctx = g_shared.ctx();
        const auto& config = settings::g_combat.m_ragebot.get_group(shared_ctx.weapon_type, shared_ctx.item_def_idx);
        const auto aim_punch = shared_ctx.aim_punch;

        auto aim_angle = config.no_spread.value ?
            math::helpers::calculate_angle(shoot_eye, tgt.hit.position) :
            tgt.hit.aim_angle;

        auto stamp_tick = tick_base;
        auto stamp_frac = 0.0f;

        if (!tgt.hit.source_eye.is_uninterpolated)
        {
            const auto stamp = ballistics::normalize_stamp(tgt.hit.source_eye.player_tick,
                tgt.hit.source_eye.player_frac, tgt.hit.source_eye.lerp_ticks_int,
                tgt.hit.source_eye.lerp_ticks_frac);
            if (!stamp)
                return;
            stamp_tick = stamp->tick;
            stamp_frac = stamp->fraction;
        }

        if (!std::isfinite(aim_angle.x) || !std::isfinite(aim_angle.y) ||
            !std::isfinite(aim_punch.x) || !std::isfinite(aim_punch.y))
            return;

        if (config.no_spread.value)
        {
            if (!std::isfinite(shared_ctx.inaccuracy) || shared_ctx.inaccuracy < 0.0f ||
                !std::isfinite(shared_ctx.spread) || shared_ctx.spread < 0.0f ||
                !std::isfinite(shared_ctx.recoil_index))
                return;

            const auto solution = g_shared.solve_spread_correction(aim_angle, stamp_tick);
            if (!solution)
                return;
            const auto corrected = *solution;

            const auto shot_command = ballistics::command_angles(corrected, aim_punch);
            const auto seed = g_shared.get_spread_seed(shot_command, stamp_tick);
            const auto spread = g_shared.calculate_spread(seed, shared_ctx.inaccuracy,
                shared_ctx.spread, shared_ctx.recoil_index, shared_ctx.item_def_idx, shared_ctx.num_bullets);
            if (!std::isfinite(spread.x) || !std::isfinite(spread.y) ||
                !std::isfinite(shared_ctx.range) || shared_ctx.range <= 0.0f)
                return;

            // Validate this seed's actual trajectory, not a hitchance threshold.
            // penetration::run already checks the target's recorded hitboxes.
            math::vector3 forward{}, left{}, up{};
            math::helpers::angle_vectors_left(corrected, &forward, &left, &up);
            const auto direction = (forward + left * spread.x + up * spread.y).normalized();
            const auto pen_ctx = g_shared.pen().prepare_target(tgt.hit.pawn, tgt.hit.record);
            shared::penetration::result pen{};
            if (!g_shared.pen().run(shoot_eye, shoot_eye + direction * shared_ctx.range,
                    pen_ctx, local.pawn, local.team, pen) ||
                pen.damage < this->get_min_damage(config, tgt.hit.health, config.min_damage_override.value) ||
                pen.hitgroup != tgt.hit.hitgroup)
                return;

            aim_angle = corrected;
        }

        auto shot_command = ballistics::command_angles(aim_angle, aim_punch);
        if (!config.no_spread.value)
            shot_command.z = 0.0f;
        if (!ballistics::finite(shot_command))
            return;

        this->m_firing_this_tick = true;
        g_shared.last_shoot_tick() = tick_base;

        if (settings::g_misc.m_impacts.console_log.value)
        {
            const auto hitgroup_name = systems::g_hitboxes.hitgroup_to_name(tgt.hit.hitgroup);
            const auto bt_delta = g_shared.ctx().current_tick - tgt.hit.record->tick;
            const auto is_extrap = tgt.hit.record && tgt.hit.record->extrapolated;
            logging::console::print(
                xs("[rage] shot target hp {} for {:.0f} in {} (hc {:.0f}%, bt {}t{}{})"),
                tgt.hit.health,
                tgt.hit.damage,
                hitgroup_name,
                tgt.hitchance * 100.0f,
                bt_delta,
                was_forced ? xs(", forced") : "",
                is_extrap ? xs(", extrap") : ""
            );
            if (config.no_spread.value)
            {
                logging::console::print(
                    xs("[rage:nospread] weapon {} tick {}+{:.4f} inacc {:.6f} spread {:.6f} recoil {:.3f} punch ({:.4f}, {:.4f})"),
                    shared_ctx.item_def_idx, stamp_tick, stamp_frac, shared_ctx.inaccuracy,
                    shared_ctx.spread, shared_ctx.recoil_index, aim_punch.x, aim_punch.y);
            }
        }

        // Diagnostics compare the impact ray with the intended target ray,
        // not the tilted pre-spread barrel direction produced by the solver.
        const auto diagnostic_aim = config.no_spread.value
            ? math::helpers::calculate_angle(shoot_eye, tgt.hit.position) : aim_angle;
        features::misc::g_impacts.on_boom(tgt.hit.pawn, tgt.hit.hitgroup, tgt.hit.damage, tgt.hitchance, shared_ctx.inaccuracy, shared_ctx.spread, diagnostic_aim, shoot_eye, tgt.hit.record->tick, g_shared.lc().get_skeleton(*tgt.hit.record), was_forced);
        features::esp::player::g_chams.os().push(tgt.hit.pawn);

        const auto record_time = cstypes::tick_fraction::from_value(tgt.hit.record->simulation_time / cstypes::tick_interval);

        for (auto i = 0; i < history_size; ++i)
        {
            const auto entry = cmd->csgo_user_cmd.mutable_input_history(i);
            if (!entry)
                continue;

            if (const auto angles = entry->mutable_view_angles())
            {
                angles->set_x(shot_command.x);
                angles->set_y(shot_command.y);
                angles->set_z(shot_command.z);
            }

            entry->set_render_tick_count(record_time.tick + 1);
            entry->set_render_tick_fraction(0.0f);

            if (config.no_spread.value || !tgt.hit.source_eye.is_uninterpolated)
            {
                entry->set_player_tick_count(stamp_tick);
                entry->set_player_tick_fraction(stamp_frac);
            }

            if (entry->has_sv_interp0())
            {
                const auto interp = entry->mutable_sv_interp0();
                interp->set_src_tick(-1);
                interp->set_dst_tick(-1);
                interp->set_frac(0.0f);
            }
            if (entry->has_sv_interp1())
            {
                const auto interp = entry->mutable_sv_interp1();
                interp->set_src_tick(-1);
                interp->set_dst_tick(-1);
                interp->set_frac(0.0f);
            }
            if (entry->has_cl_interp())
            {
                const auto interp = entry->mutable_cl_interp();
                interp->set_frac(0.0f);
            }
        }

        const auto quick_revolver = shared_ctx.item_def_idx == cstypes::item_definition_index::weapon_r8_revolver
            && settings::g_combat.m_autos.revolver_quick.value;
        const auto attack_button = quick_revolver ? cstypes::command_buttons::in_second_attack : cstypes::command_buttons::in_attack;

        cmd->buttons.value |= attack_button;
        cmd->buttons.value_changed |= attack_button;
        cmd->buttons.value_scroll |= attack_button;

        if (history_size > 0)
        {
            if (quick_revolver)
            {
                cmd->csgo_user_cmd.set_attack2_start_history_index(history_size - 1);
                cmd->csgo_user_cmd.set_attack1_start_history_index(-1);
            }
            else
            {
                cmd->csgo_user_cmd.set_attack1_start_history_index(history_size - 1);
            }
        }

        math::vector3 forward{};
        {
            if (const auto angles = base->viewangles())
                math::helpers::angle_vectors_left({ angles->x(), angles->y(), angles->z() }, &forward);
        }

        const auto punched_aim = shot_command;

        const auto facing_away = forward.dot((tgt.hit.record->origin - systems::g_prediction.pre().networked_origin).normalized()) < 0.707107f;
        auto command_aim = punched_aim;

        // Do not replace a seed-validated command with unrelated hide-shot angles.
        if (!config.no_spread.value && facing_away && settings::g_combat.m_antiaim.hide_shots.value)
        {
            command_aim.x = 179.9f;
            command_aim.y = std::remainderf(punched_aim.y + 180.0f, 360.0f);
        }

        if (const auto angles = base->mutable_viewangles())
        {
            angles->set_x(command_aim.x);
            angles->set_y(command_aim.y);
            angles->set_z(command_aim.z);
        }

        if (!config.silent.value)
            systems::g_input.set_view_angles(punched_aim);
    }

    void rage::fire_melee(systems::input::usercmd* cmd, const target& tgt, const systems::local::snapshot& local)
    {
        if (!tgt.hit.record || !tgt.hit.record->valid)
            return;

        this->m_firing_this_tick = true;
        const auto base = cmd->csgo_user_cmd.mutable_base();
        const auto tick_base = memory::read<int>(local.controller + SCHEMA("CBasePlayerController", "m_nTickBase"_hash));
        g_shared.last_shoot_tick() = tick_base;

        const auto record_time = cstypes::tick_fraction::from_value(tgt.hit.record->simulation_time / cstypes::tick_interval);
        const auto history_index = cmd->csgo_user_cmd.input_history_size() - 1;
        const auto entry = history_index >= 0 ? cmd->csgo_user_cmd.mutable_input_history(history_index) : nullptr;

        if (entry)
        {
            if (const auto angles = entry->mutable_view_angles())
            {
                angles->set_x(tgt.hit.aim_angle.x);
                angles->set_y(tgt.hit.aim_angle.y);
            }
            entry->set_render_tick_count(record_time.tick + 1);
            entry->set_render_tick_fraction(0.0f);

            if (!tgt.hit.source_eye.is_uninterpolated)
            {
                auto tick_add = [](int t, float f, int tick_delta, float frac_delta)
                    {
                        f += frac_delta;
                        auto carry = static_cast<int>(std::floor(f));
                        f -= static_cast<float>(carry);
                        return std::pair{ t + tick_delta + carry, f };
                    };
                const auto [stamp_tick, stamp_frac] = tick_add(tgt.hit.source_eye.player_tick, tgt.hit.source_eye.player_frac, tgt.hit.source_eye.lerp_ticks_int, tgt.hit.source_eye.lerp_ticks_frac);
                entry->set_player_tick_count(stamp_tick);
                entry->set_player_tick_fraction(stamp_frac);
            }

            if (entry->has_sv_interp0())
            {
                const auto interp = entry->mutable_sv_interp0();
                interp->set_src_tick(-1);
                interp->set_dst_tick(-1);
                interp->set_frac(0.0f);
            }
            if (entry->has_sv_interp1())
            {
                const auto interp = entry->mutable_sv_interp1();
                interp->set_src_tick(-1);
                interp->set_dst_tick(-1);
                interp->set_frac(0.0f);
            }
            if (entry->has_cl_interp())
            {
                const auto interp = entry->mutable_cl_interp();
                interp->set_frac(0.0f);
            }
        }

        const auto is_secondary = tgt.hit.attack_type == 1;
        const auto attack_button = is_secondary ? cstypes::command_buttons::in_second_attack : cstypes::command_buttons::in_attack;

        cmd->buttons.value |= attack_button;
        cmd->buttons.value_changed |= attack_button;
        cmd->buttons.value_scroll |= attack_button;

        if (history_index >= 0)
        {
            if (is_secondary)
                cmd->csgo_user_cmd.set_attack2_start_history_index(history_index);
            else
                cmd->csgo_user_cmd.set_attack1_start_history_index(history_index);
        }

        if (const auto angles = base->mutable_viewangles())
        {
            angles->set_x(tgt.hit.aim_angle.x);
            angles->set_y(tgt.hit.aim_angle.y);
        }
    }

    std::size_t rage::generate_multipoints(const systems::hitboxes::entry& hitbox, const math::vector3& center, const math::quaternion& bone_rot, float pointscale, const math::vector3& shoot_pos, std::optional<float> cone_tangent, std::array<math::vector3, 3>& out) const
    {
        if (hitbox.index != 0 && (hitbox.index < 2 || hitbox.index > 6))
            return 0;
        auto scale = std::clamp(pointscale / 100.0f, 0.0f, 1.0f);
        if (scale <= 0.01f)
            return 0;

        // Dynamic pointscale based on the caller's unchanged spread cone.
        if (cone_tangent && hitbox.radius > 0.001f)
        {
            const auto cone_radius = *cone_tangent * (center - shoot_pos).length();
            const auto automatic_scale = std::clamp(0.9f - cone_radius / hitbox.radius, 0.0f, 1.0f);
            scale = std::min(scale, automatic_scale);
            if (scale <= 0.01f)
                return 0;
        }

        // Build view-relative frame
        const auto shoot_dir = (center - shoot_pos).normalized();
        const auto ang = math::helpers::vector_to_angle(shoot_dir);
        math::vector3 left{}, up{};
        math::helpers::angle_vectors_left(ang, nullptr, &left, &up);
        const auto right = math::vector3{ -left.x, -left.y, -left.z };
        auto inverse = bone_rot;
        inverse.x = -inverse.x;
        inverse.y = -inverse.y;
        inverse.z = -inverse.z;
        const auto extents = (hitbox.maxs - hitbox.mins) * 0.5f;
        const auto scaled_radius = hitbox.radius * scale;

        const auto scaled_offset = [&](const math::vector3& direction) -> math::vector3
        {
            if (hitbox.radius > 0.001f)
            {
                return center + direction * scaled_radius;
            }
            const auto local_dir = inverse.rotate_vector(direction);
            auto distance = 8192.0f;
            if (std::fabs(local_dir.x) > 1.0e-6f) distance = std::min(distance, std::fabs(extents.x / local_dir.x));
            if (std::fabs(local_dir.y) > 1.0e-6f) distance = std::min(distance, std::fabs(extents.y / local_dir.y));
            if (std::fabs(local_dir.z) > 1.0e-6f) distance = std::min(distance, std::fabs(extents.z / local_dir.z));
            if (distance < 8192.0f)
                return center + direction * (distance * scale);
            return center;
        };

        // Preserve point order and expose only initialized entries. Reusing the
        // array after a head scan must not leak its third point into a torso scan.
        if (hitbox.index == 0)
        {
            out[0] = scaled_offset(up);
            out[1] = scaled_offset(right);
            out[2] = scaled_offset(-right);
            return 3;
        }
        out[0] = scaled_offset(right);
        out[1] = scaled_offset(-right);
        return 2;
    }

    bool rage::should_stop_movement(const aim_context& ctx) const
    {
        const auto& shared_ctx = g_shared.ctx();
        const auto& prestate = systems::g_prediction.pre();
        const auto velocity = prestate.networked_velocity;

        if (shared_ctx.weapon_type == cstypes::weapon_type::sniper && !ctx.is_scoped)
            return false;

        if (ctx.on_ground)
        {
            const auto speed_2d = velocity.length_2d();
            if (speed_2d <= 0.1f)
                return false;

            const auto inaccuracy_move = memory::read<float>(shared_ctx.weapon_vdata + SCHEMA("CCSWeaponBaseVData", "m_flInaccuracyMove"_hash));
            const auto inaccuracy_stand = memory::read<float>(shared_ctx.weapon_vdata + SCHEMA("CCSWeaponBaseVData", "m_flInaccuracyStand"_hash));
            return speed_2d * inaccuracy_move > inaccuracy_stand;
        }

        // Air stop logic for snipers
        if (shared_ctx.weapon_type != cstypes::weapon_type::sniper)
            return false;

        if (velocity.z > 140.0f)
            return false;

        const auto sv_gravity = CONVAR("sv_gravity")->get<float>();
        const auto sv_friction = CONVAR("sv_friction")->get<float>();
        const auto sv_stopspeed = CONVAR("sv_stopspeed")->get<float>();
        const auto inac_jump_initial = memory::read<float>(shared_ctx.weapon_vdata + SCHEMA("CCSWeaponBaseVData", "m_flInaccuracyJumpInitial"_hash));
        const auto inac_jump_apex = memory::read<float>(shared_ctx.weapon_vdata + SCHEMA("CCSWeaponBaseVData", "m_flInaccuracyJumpApex"_hash));

        const auto shootable_threshold = inac_jump_apex + 0.001f;
        const auto early_threshold = inac_jump_initial * 0.55f + inac_jump_apex * 0.45f;
        const auto air_inaccuracy = g_shared.get_air_inaccuracy(velocity.z, inac_jump_initial, inac_jump_apex);

        if (air_inaccuracy <= shootable_threshold || air_inaccuracy <= early_threshold)
            return true;

        auto sim_vz = velocity.z;
        auto ticks_to_shootable{ 0 };
        for (auto i = 1; i <= 32; ++i)
        {
            sim_vz -= sv_gravity * cstypes::tick_interval;
            if (g_shared.get_air_inaccuracy(sim_vz, inac_jump_initial, inac_jump_apex) <= shootable_threshold)
            {
                ticks_to_shootable = i;
                break;
            }
        }

        if (ticks_to_shootable == 0)
            return false;

        const auto speed_2d = velocity.length_2d();
        const auto max_speed = memory::read<float>(shared_ctx.weapon_vdata + SCHEMA("CCSWeaponBaseVData", "m_flMaxSpeed"_hash));
        const auto accurate_threshold = max_speed * 0.34f;

        if (speed_2d <= accurate_threshold)
            return true;

        auto sim_speed = speed_2d;
        auto ticks_to_stop{ 32 };
        for (auto i = 1; i <= 32; ++i)
        {
            const auto drop = std::fmaxf(sim_speed, sv_stopspeed) * sv_friction * cstypes::tick_interval;
            sim_speed -= drop;
            if (sim_speed <= accurate_threshold)
            {
                ticks_to_stop = i;
                break;
            }
        }

        return ticks_to_shootable <= ticks_to_stop + 2;
    }

    float rage::get_min_damage(const settings::combat::ragebot::weapon_group& config, int target_health, bool override_active) const
    {
        if (override_active)
            return static_cast<float>(config.min_damage_override_value);

        const auto base = static_cast<float>(config.min_damage);
        const auto hp = static_cast<float>(target_health);
        if (hp < base)
            return hp + 1.0f;
        return base;
    }

    float rage::get_knife_damage(float raw, int armor, float armor_ratio) const
    {
        if (armor <= 0)
            return raw;

        const auto ratio = armor_ratio * 0.5f;
        auto damage_to_health = raw * ratio;
        const auto damage_to_armor = (raw - damage_to_health) * 0.5f;

        if (damage_to_armor > static_cast<float>(armor))
            damage_to_health = raw - static_cast<float>(armor) * 2.0f;

        return std::max(0.0f, std::floorf(damage_to_health));
    }

    systems::tracing::result rage::trace_taser_hit(const math::vector3& origin, const math::vector3& forward, float range, std::uintptr_t target_pawn, std::uintptr_t local_pawn) const
    {
        const auto end = origin + forward * range;
        const int filter_extras[]{ 0, 15 };

        for (const auto extra : filter_extras)
        {
            const auto filter = extra == 0 ?
                systems::g_tracing.make_filter(local_pawn, 0x001c1003, 4) :
                systems::g_tracing.make_filter(local_pawn, 0x001c1003, 4, 15);

            auto result = systems::g_tracing.trace(origin, end, filter);
            if ((result.fraction < 1.0f || result.all_solid) && result.hit_entity == target_pawn)
                return result;

            for (auto radius = 2.0f; radius <= 4.0f; radius += 2.0f)
            {
                const auto sweep_end = end - forward * radius;
                result = systems::g_tracing.trace_sphere(origin, sweep_end, radius, filter);
                if ((result.fraction < 1.0f || result.all_solid) && result.hit_entity == target_pawn)
                    return result;
            }
        }

        systems::tracing::result miss{};
        miss.fraction = 1.0f;
        miss.hit_entity = 0;
        return miss;
    }

    systems::tracing::result rage::trace_knife_hit(const math::vector3& origin, const math::vector3& forward, float reach, std::uintptr_t target_pawn, std::uintptr_t local_pawn) const
    {
        const auto end = origin + forward * reach;
        const auto knife_filter = systems::g_tracing.make_filter(local_pawn, 0x0c3001, 4);

        auto result = systems::g_tracing.trace(origin, end, knife_filter);
        if ((result.fraction < 1.0f || result.all_solid) && result.hit_entity == target_pawn)
            return result;

        const auto weapon_filter = systems::g_tracing.make_filter(local_pawn, 0x0c3001, 4, 15);
        result = systems::g_tracing.trace(origin, end, weapon_filter);
        if ((result.fraction < 1.0f || result.all_solid) && result.hit_entity == target_pawn)
            return result;

        for (auto radius = 14.0f; radius > 0.0f; radius -= 3.0f)
        {
            const auto sweep_end = end - forward * radius;
            result = systems::g_tracing.trace_sphere(origin, sweep_end, radius, weapon_filter);
            if ((result.fraction < 1.0f || result.all_solid) && result.hit_entity == target_pawn)
                return result;
        }

        result.fraction = 1.0f;
        result.hit_entity = 0;
        return result;
    }

    void rage::update_penetration_crosshair(const systems::local::snapshot& local)
    {
        const auto& cfg = settings::g_combat.m_penetration_crosshair;
        const auto& ctx = g_shared.ctx();

        if (!cfg.enabled.value || !ctx.valid || !local.is_alive || local.team < 2
            || !local.pawn || !ctx.weapon
            || ctx.weapon_type < cstypes::weapon_type::pistol || ctx.weapon_type > cstypes::weapon_type::lmg)
        {
            this->m_penetration_crosshair_state.store(penetration_crosshair_state::unavailable, std::memory_order_relaxed);
            return;
        }

        const auto eye_pos = g_shared.get_eye_position(local.pawn);
        auto view_angles = systems::g_input.get_view_angles();
        const auto aim_punch = g_shared.get_aim_punch(local.pawn);
        view_angles.x += aim_punch.x;
        view_angles.y += aim_punch.y;

        math::vector3 forward{};
        math::helpers::angle_vectors_left(view_angles, &forward);

        auto pen_damage{ 0.0f };
        const auto can_pen = g_shared.pen().can(eye_pos, forward, pen_damage, local);

        this->m_penetration_crosshair_state.store(
            can_pen ? penetration_crosshair_state::penetrable : penetration_crosshair_state::blocked,
            std::memory_order_relaxed);
    }

    void rage::draw_penetration_crosshair(xdraw::draw_list& draw_list) const
    {
        const auto& cfg = settings::g_combat.m_penetration_crosshair;
        if (!cfg.enabled.value)
            return;

        const auto state = this->m_penetration_crosshair_state.load(std::memory_order_relaxed);
        const auto local = systems::g_local.get();

        if (state == penetration_crosshair_state::unavailable || !local.is_alive || systems::g_local.is_in_cinematic())
            return;

        const auto can_pen = state == penetration_crosshair_state::penetrable;
        const auto& fill = can_pen ? cfg.can_penetrate_fill : cfg.blocked_fill;
        const auto& outline = can_pen ? cfg.can_penetrate_outline : cfg.blocked_outline;

        const auto [screen_w, screen_h] = xdraw::viewport_size();
        const auto cx = std::floorf(static_cast<float>(screen_w) * 0.5f);
        const auto cy = std::floorf(static_cast<float>(screen_h) * 0.5f);

        constexpr auto half_size{ 3.0f };
        constexpr auto outline_size{ 1.0f };

        if (cfg.glow)
        {
            auto& glow = xdraw::get_glow();
            const auto glow_a = static_cast<std::uint8_t>(static_cast<float>(outline.value.a) * cfg.glow_strength);
            const auto glow_col = xdraw::color{ outline.value.r, outline.value.g, outline.value.b, glow_a };
            glow.rect_filled(cx - half_size - outline_size, cy - half_size - outline_size,
                (half_size + outline_size) * 2.0f, (half_size + outline_size) * 2.0f, glow_col);
        }

        draw_list.rect_filled(cx - half_size - outline_size, cy - half_size - outline_size,
            (half_size + outline_size) * 2.0f, (half_size + outline_size) * 2.0f, outline);
        draw_list.rect_filled(cx - half_size, cy - half_size, half_size * 2.0f, half_size * 2.0f, fill);
    }

} // namespace features::combat