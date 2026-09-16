#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>
#include "../air_acceleration.hpp"

namespace features::movement {
    namespace {
        constexpr auto k_max_subticks{ 16 };

        [[nodiscard]] float strafe_yaw(float vx, float vy, float target_yaw, float dt,
            bool side_switch, float wishspeed, float air_accel, float friction, float cap)
        {
            const auto speed = std::hypot(vx, vy);
            if (speed < 0.001f)
                return target_yaw;

            const auto theta = detail::ideal_air_angle(speed, dt, wishspeed, air_accel, friction, cap);
            const auto velocity_yaw = std::atan2(vy, vx) * (180.0f / std::numbers::pi_v<float>);
            const auto delta = math::helpers::normalize_yaw(target_yaw - velocity_yaw);
            const auto positive = std::fabs(delta) > 2.0f ? delta > 0.0f : side_switch;
            return math::helpers::normalize_yaw(velocity_yaw + (positive ? theta : -theta));
        }
    }

    [[nodiscard]] bool test_strafer::is_active() const
    {
        if (!settings::g_movement.airstrafe.value && !settings::g_movement.m_test_strafer.enabled.value)
            return false;
        const auto quantized = CONVAR("sv_quantize_movement_input");
        return quantized ? quantized->get<bool>() : true;
    }

    math::vector2 test_strafer::movement_from_buttons(std::uintptr_t pressed)
    {
        auto forward{ 0.0f };
        auto side{ 0.0f };
        if (pressed & cstypes::command_buttons::in_forward) forward = 1.0f;
        else if (pressed & cstypes::command_buttons::in_back) forward = -1.0f;
        if (pressed & cstypes::command_buttons::in_moveleft) side = -1.0f;
        else if (pressed & cstypes::command_buttons::in_moveright) side = 1.0f;
        return { forward, side };
    }

    void test_strafer::on_create_move(systems::input::usercmd* cmd)
    {
        this->m_handled_this_tick = false;
        if (!cmd || !this->is_active())
        {
            this->m_last_buttons = 0;
            this->m_last_pressed = 0;
            this->m_substep_counter = 0;
            return;
        }

        const auto local = systems::g_local.get();
        const auto& pre = systems::g_prediction.pre();
        if (!local.is_alive || !local.pawn || !pre.movement_valid || pre.pawn != local.pawn)
            return;

        const auto move_type = memory::read<std::uint8_t>(local.pawn + SCHEMA("C_BaseEntity", "m_nActualMoveType"_hash));
        if (move_type == cstypes::move_type::ladder || move_type == cstypes::move_type::noclip ||
            (pre.flags & cstypes::entity_flags::on_ground))
            return;

        // Jumpbug runs after this feature and owns the final command. Its
        // active_this_tick flag here still describes the previous command.
        if (features::combat::g_rage.is_firing_this_tick() ||
            (cmd->buttons.value & cstypes::command_buttons::in_attack))
            return;

        // Do not disable acceleration near an apex or several ticks before
        // landing. Grounded flags, rather than a stationary vertical probe,
        // decide whether this command is airborne.
        this->strafe_path(cmd);
    }

    bool test_strafer::apply_yaw_subtick(proto::base_usercmd_pb* base, float when, float yaw_delta) const
    {
        yaw_delta = math::helpers::normalize_yaw(yaw_delta);
        // Zero yaw needs no event, but it is not an allocation failure and must
        // not terminate the remaining simulation intervals.
        if (std::fabs(yaw_delta) <= 0.00001f)
            return true;

        const auto step = systems::g_input.acquire_subtick_step(base->mutable_subtick_moves());
        if (!step)
            return false;

        step->set_when(when);
        step->set_button(0);
        step->set_pressed(false);
        step->set_analog_forward_delta(0.0f);
        step->set_analog_left_delta(0.0f);
        step->set_yaw_delta(yaw_delta);
        step->set_pitch_delta(0.0f);
        return true;
    }

    void test_strafer::strafe_path(systems::input::usercmd* cmd)
    {
        const auto& antiaim = features::combat::g_misc.antiaim();
        const auto aa_active = antiaim.has_modified_angles();
        const auto original_buttons = aa_active ? antiaim.get_original_buttons() : cmd->buttons.value;
        if (original_buttons & cstypes::command_buttons::in_sprint)
            return;

        const auto base = cmd->csgo_user_cmd.mutable_base();
        if (!base || systems::g_input.has_analog_subticks(base))
            return;

        this->check_button(original_buttons, cstypes::command_buttons::in_moveleft);
        this->check_button(original_buttons, cstypes::command_buttons::in_moveright);
        this->check_button(original_buttons, cstypes::command_buttons::in_forward);
        this->check_button(original_buttons, cstypes::command_buttons::in_back);
        this->m_last_buttons = original_buttons;
        const auto player_move = movement_from_buttons(this->m_last_pressed);
        // Enabling anti-aim must not invent a forward key when WASD is released.
        if (player_move.x == 0.0f && player_move.y == 0.0f)
            return;

        const auto local = systems::g_local.get();
        const auto movement_services = memory::read<std::uintptr_t>(local.pawn + SCHEMA("C_BasePlayerPawn", "m_pMovementServices"_hash));
        const auto accel_var = CONVAR("sv_airaccelerate");
        const auto speed_var = CONVAR("sv_maxspeed");
        const auto cap_var = CONVAR("sv_air_max_wishspeed");
        if (!movement_services || !accel_var || !speed_var || !cap_var)
            return;

        const auto& pre = systems::g_prediction.pre();
        const auto player_max = memory::read<float>(movement_services + SCHEMA("CPlayer_MovementServices", "m_flMaxspeed"_hash));
        const auto server_max = speed_var->get<float>();
        const auto air_accel = accel_var->get<float>();
        const auto cap = cap_var->get<float>();
        const auto friction = pre.surface_friction;
        const auto command_yaw = systems::g_input.get_view_angles().y;
        const auto base_yaw = base->viewangles() ? base->viewangles()->y() : command_yaw;
        if (!std::isfinite(player_max) || player_max <= 0.0f ||
            !std::isfinite(server_max) || server_max <= 0.0f ||
            !std::isfinite(air_accel) || air_accel <= 0.0f ||
            !std::isfinite(cap) || cap <= 0.0f || !std::isfinite(friction) || friction <= 0.0f ||
            !std::isfinite(pre.networked_velocity.x) || !std::isfinite(pre.networked_velocity.y) ||
            !std::isfinite(command_yaw) || !std::isfinite(base_yaw))
            return;

        const auto wishspeed = std::min(player_max, server_max);
        const auto offset = std::atan2(-player_move.y, player_move.x) * (180.0f / std::numbers::pi_v<float>);
        const auto target_yaw = math::helpers::normalize_yaw(command_yaw + offset);

        // A late landing jump must not delay all steering until the end of the
        // command. Use the available AIR interval starting at zero instead.
        auto end_when = 1.0f;
        for (auto i = 0; i < base->subtick_moves_size(); ++i)
        {
            const auto step = base->mutable_subtick_moves(i);
            if (step && (step->button() & cstypes::command_buttons::in_jump) && step->pressed() &&
                std::isfinite(step->when()) && step->when() > 0.0f)
                end_when = std::min(end_when, step->when());
        }

        const auto moves = base->mutable_subtick_moves();
        if (!moves)
            return;
        const auto original_size = moves->m_current_size;
        const auto interval = end_when / static_cast<float>(k_max_subticks);
        const auto dt = interval * cstypes::tick_interval;
        auto accumulated_yaw = base_yaw;
        auto vx = pre.networked_velocity.x;
        auto vy = pre.networked_velocity.y;

        for (auto i = 0; i < k_max_subticks; ++i)
        {
            const auto side_switch = ((this->m_substep_counter + i) % 2) == 0;
            const auto wish_yaw = strafe_yaw(vx, vy, target_yaw, dt, side_switch,
                wishspeed, air_accel, friction, cap);
            const auto view_yaw = math::helpers::normalize_yaw(wish_yaw - offset);
            if (!this->apply_yaw_subtick(base, i * interval, view_yaw - accumulated_yaw))
            {
                // Do not leave half a strafe sequence with an unmodified base.
                moves->m_current_size = original_size;
                return;
            }
            accumulated_yaw = view_yaw;
            detail::simulate_air_acceleration(vx, vy, wish_yaw, dt, wishspeed, air_accel, friction, cap);
        }

        // Restore the original command heading only at the end, not at 0.995
        // after a different anti-aim-only simulation window. At a known landing
        // boundary restore the movement basis as well before ground movement.
        const auto restore_when = end_when < 1.0f ? end_when : std::nextafter(1.0f, 0.0f);
        if (!this->apply_yaw_subtick(base, restore_when, base_yaw - accumulated_yaw))
        {
            moves->m_current_size = original_size;
            return;
        }

        const auto original_forward = base->forwardmove();
        const auto original_left = base->leftmove();
        if (end_when < 1.0f)
        {
            const auto restore = systems::g_input.acquire_subtick_step(moves);
            const auto initial = restore ? systems::g_input.acquire_subtick_step(moves) : nullptr;
            if (!restore || !initial)
            {
                moves->m_current_size = original_size;
                return;
            }
            // input::apply skips its initial analog event when we supply one.
            initial->set_when(0.0f);
            initial->set_button(0);
            initial->set_pressed(false);
            initial->set_analog_forward_delta(player_move.x - pre.last_movement_impulses.x);
            initial->set_analog_left_delta(-player_move.y - pre.last_movement_impulses.y);
            restore->set_when(restore_when);
            restore->set_button(0);
            restore->set_pressed(false);
            restore->set_analog_forward_delta(original_forward - player_move.x);
            restore->set_analog_left_delta(original_left + player_move.y);
        }

        base->set_forwardmove(player_move.x);
        base->set_leftmove(-player_move.y);
        constexpr auto movement_mask = cstypes::command_buttons::in_forward | cstypes::command_buttons::in_back |
            cstypes::command_buttons::in_moveleft | cstypes::command_buttons::in_moveright;
        const auto before = cmd->buttons.value;
        // Preserve changes made by combat/duckpeek after anti-aim captured WASD.
        cmd->buttons.value = (before & ~movement_mask) | (this->m_last_pressed & movement_mask);
        cmd->buttons.value_changed |= before ^ cmd->buttons.value;
        this->m_handled_this_tick = true;
        this->m_substep_counter ^= 1;
    }

    void test_strafer::check_button(std::uintptr_t current_buttons, std::uintptr_t button)
    {
        constexpr auto moveleft = static_cast<std::uintptr_t>(cstypes::command_buttons::in_moveleft);
        constexpr auto moveright = static_cast<std::uintptr_t>(cstypes::command_buttons::in_moveright);
        constexpr auto forward = static_cast<std::uintptr_t>(cstypes::command_buttons::in_forward);
        constexpr auto back = static_cast<std::uintptr_t>(cstypes::command_buttons::in_back);

        if (current_buttons & button && (!(this->m_last_buttons & button) ||
            (button & moveleft && !(this->m_last_pressed & moveright)) ||
            (button & moveright && !(this->m_last_pressed & moveleft)) ||
            (button & forward && !(this->m_last_pressed & back)) ||
            (button & back && !(this->m_last_pressed & forward))))
        {
            if (button & moveleft) this->m_last_pressed &= ~moveright;
            else if (button & moveright) this->m_last_pressed &= ~moveleft;
            else if (button & forward) this->m_last_pressed &= ~back;
            else if (button & back) this->m_last_pressed &= ~forward;
            this->m_last_pressed |= button;
        }
        else if (!(current_buttons & button))
        {
            this->m_last_pressed &= ~button;
        }
    }

} // namespace features::movement
