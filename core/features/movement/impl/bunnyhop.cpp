#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include "../movement.hpp"
#include <protection/game_addresses.hpp>

namespace features::movement {
    namespace {
        [[nodiscard]] bool finite_vector(const math::vector3& value)
        {
            return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
        }

        [[nodiscard]] std::optional<float> predict_landing_fraction(std::uintptr_t pawn,
            std::uintptr_t movement_services, const systems::prediction::state& pre)
        {
            if (!finite_vector(pre.networked_origin) || !finite_vector(pre.networked_velocity) ||
                !finite_vector(pre.collision_mins) || !finite_vector(pre.collision_maxs) ||
                pre.collision_maxs.x <= pre.collision_mins.x ||
                pre.collision_maxs.y <= pre.collision_mins.y ||
                pre.collision_maxs.z <= pre.collision_mins.z ||
                !PATTERN(patterns::trace_hull) || !PATTERN(patterns::trace_filter_set_collision))
                return std::nullopt;

            const auto pawn_ptr = memory::read<std::uintptr_t>(movement_services + 56);
            if (!pawn_ptr)
                return std::nullopt;
            auto mask = memory::read<std::uintptr_t>(pawn_ptr + 0xd48);
            if (memory::read<std::uint32_t>(pawn_ptr + 0x3f8) & 0x10)
                mask |= 0x20;

            const auto gravity_var = CONVAR("sv_gravity");
            const auto normal_var = CONVAR("sv_standable_normal");
            if (!gravity_var || !normal_var)
                return std::nullopt;
            const auto gravity = gravity_var->get<float>() * pre.gravity_scale;
            const auto standable = normal_var->get<float>();
            if (!std::isfinite(gravity) || gravity < 0.0f ||
                !std::isfinite(standable) || standable <= 0.0f || standable > 1.0f)
                return std::nullopt;

            const auto filter = systems::g_tracing.make_player_movement_filter(pawn, mask, 11);
            // Use the actual captured hull. Expanding a crouched player to a
            // standing hull falsely predicts contact with stair risers/ceilings.
            const systems::tracing::bbox_collision hull{ pre.collision_mins, pre.collision_maxs };
            auto origin = pre.networked_origin;
            auto velocity = pre.networked_velocity;
            velocity.z -= gravity * cstypes::tick_interval * 0.5f;
            auto remaining = 1.0f;
            auto elapsed = 0.0f;

            for (auto bump = 0; bump < 4 && remaining > 0.0f; ++bump)
            {
                // Do not extend the trajectory downward by a ground-snap probe:
                // its hit fraction is not a time fraction and fires jump early.
                const auto end = origin + velocity * (remaining * cstypes::tick_interval);
                const auto trace = systems::g_tracing.trace_player_bbox(origin, end, hull, filter, movement_services);
                if (trace.all_solid || !std::isfinite(trace.fraction) || trace.fraction < 0.0f ||
                    trace.fraction > 1.0f || !finite_vector(trace.normal) || !finite_vector(trace.end_pos))
                    return std::nullopt;
                if (trace.fraction == 1.0f)
                    return std::nullopt;
                if (trace.normal.length_sqr() < 0.5f)
                    return std::nullopt;

                elapsed += remaining * trace.fraction;
                remaining *= 1.0f - trace.fraction;
                if (velocity.z <= 0.0f && trace.normal.z >= standable)
                {
                    // Contact at fraction zero is valid on slopes and stair
                    // edges. Press just AFTER contact, never round it backward.
                    const auto when = std::max(1.0f / 64.0f, std::nextafter(elapsed, 1.0f));
                    return when < 1.0f ? std::optional<float>{ when } : std::nullopt;
                }

                // A riser/wall is not ground. Continue the remaining downward
                // slide to find a subsequent floor contact within this tick.
                const auto normal = trace.normal.normalized();
                const auto into_plane = velocity.dot(normal);
                if (into_plane >= -0.001f)
                    return std::nullopt;
                origin = trace.end_pos;
                velocity -= normal * into_plane;
            }
            return std::nullopt;
        }

        bool apply_landing_jump(proto::base_usercmd_pb* base, float when)
        {
            const auto moves = base->mutable_subtick_moves();
            if (!moves)
                return false;
            const auto old_size = moves->m_current_size;
            const auto release = systems::g_input.acquire_subtick_step(moves);
            const auto press = release ? systems::g_input.acquire_subtick_step(moves) : nullptr;
            if (!release || !press)
            {
                moves->m_current_size = old_size;
                return false;
            }

            release->set_when(0.0f);
            release->set_button(cstypes::command_buttons::in_jump);
            release->set_pressed(false);
            release->set_analog_forward_delta(0.0f);
            release->set_analog_left_delta(0.0f);
            press->set_when(when);
            press->set_button(cstypes::command_buttons::in_jump);
            press->set_pressed(true);
            press->set_analog_forward_delta(0.0f);
            press->set_analog_left_delta(0.0f);
            return true;
        }
    }

    void bhop::on_create_move(systems::input::usercmd* cmd) const
    {
        if (!cmd || !settings::g_movement.bhop.value)
            return;
        const auto autobhop = CONVAR("sv_autobunnyhopping");
        if (autobhop && autobhop->get<bool>())
            return;
        if (!((cmd->buttons.value | cmd->buttons.value_scroll) & cstypes::command_buttons::in_jump))
            return;

        const auto local = systems::g_local.get();
        const auto& pre = systems::g_prediction.pre();
        if (!local.is_alive || !local.pawn || !pre.movement_valid || pre.pawn != local.pawn)
            return;
        const auto move_type = memory::read<std::uint8_t>(local.pawn + SCHEMA("C_BaseEntity", "m_nActualMoveType"_hash));
        if (move_type == cstypes::move_type::ladder || move_type == cstypes::move_type::noclip)
            return;
        if (pre.flags & cstypes::entity_flags::on_ground)
            return;

        const auto base = cmd->csgo_user_cmd.mutable_base();
        if (!base)
            return;
        cmd->buttons.value &= ~cstypes::command_buttons::in_jump;
        cmd->buttons.value_changed |= cstypes::command_buttons::in_jump;
        cmd->buttons.value_scroll &= ~cstypes::command_buttons::in_jump;
        for (auto i = 0; i < base->subtick_moves_size(); ++i)
        {
            if (const auto step = base->mutable_subtick_moves(i))
                step->set_button(step->button() & ~cstypes::command_buttons::in_jump);
        }

        const auto movement_services = memory::read<std::uintptr_t>(local.pawn + SCHEMA("C_BasePlayerPawn", "m_pMovementServices"_hash));
        if (!movement_services)
            return;
        if (const auto landing = predict_landing_fraction(local.pawn, movement_services, pre))
            apply_landing_jump(base, *landing);
    }

} // namespace features::movement
