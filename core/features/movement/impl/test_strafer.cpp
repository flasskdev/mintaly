#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>
#include "../movement.hpp"
#include "../air_strafe_math.hpp"

namespace features::movement {
    bool test_strafer::is_active( ) const {
        return settings::g_movement.airstrafe.value || settings::g_movement.m_test_strafer.enabled.value;
    }

    bool test_strafer::emit_step(proto::base_usercmd_pb* base, float when,
                                float forward_delta, float left_delta) const {
        if (!base || !std::isfinite(when) || !std::isfinite(forward_delta) || !std::isfinite(left_delta)) return false;
        auto* step = systems::g_input.acquire_subtick_step(base->mutable_subtick_moves());
        if (!step || !valid_runtime_address(reinterpret_cast<std::uintptr_t>(step))) return false;
        step->set_button(0); step->set_pressed(false);
        step->set_when(std::clamp(when, 0.0f, 0.999f));
        step->set_analog_forward_delta(forward_delta);
        step->set_analog_left_delta(left_delta);
        step->set_pitch_delta(0.0f); step->set_yaw_delta(0.0f);
        return true;
    }

    void test_strafer::check_button(std::uintptr_t buttons, std::uintptr_t button) {
        constexpr auto forward = cstypes::command_buttons::in_forward;
        constexpr auto back = cstypes::command_buttons::in_back;
        constexpr auto left = cstypes::command_buttons::in_moveleft;
        constexpr auto right = cstypes::command_buttons::in_moveright;
        if (!(buttons & button)) { m_last_pressed &= ~button; return; }
        if (!(m_last_buttons & button)) {
            const auto opposite = button == forward ? back : button == back ? forward : button == left ? right : left;
            m_last_pressed = (m_last_pressed & ~opposite) | button;
        }
    }

    void test_strafer::on_create_move(systems::input::usercmd* cmd, std::uint64_t original_buttons) {
        m_handled_this_tick = false;
        if (!is_active() || !cmd) {
            m_last_buttons = 0; m_last_pressed = 0; m_side_toggle = false;
            return;
        }
        const auto local = systems::g_local.get();
        const auto& pre = systems::g_prediction.pre();
        if (!local.is_alive || !valid_runtime_address(local.pawn) || !pre.movement_valid || pre.pawn != local.pawn) return;
        if (const auto offset = SCHEMA("C_BaseEntity", "m_nActualMoveType"_hash)) {
            const auto type = memory::safe_read<std::uint8_t>(local.pawn + offset);
            if (type && (*type == cstypes::move_type::ladder || *type == cstypes::move_type::noclip)) return;
        }
        constexpr auto forward = cstypes::command_buttons::in_forward;
        constexpr auto back = cstypes::command_buttons::in_back;
        constexpr auto left = cstypes::command_buttons::in_moveleft;
        constexpr auto right = cstypes::command_buttons::in_moveright;
        for (const auto button : {forward, back, left, right}) check_button(original_buttons, button);
        m_last_buttons = original_buttons;
        if (pre.flags & cstypes::entity_flags::on_ground) return;
        const auto& jumpbug = settings::g_movement.jumpbug;
        if (pre.networked_velocity.z < 0.0f && (jumpbug.value || (jumpbug.bind.key != 0 && jumpbug.bind.active))) return;
        auto* base = cmd->csgo_user_cmd.has_base() ? cmd->csgo_user_cmd.mutable_base() : nullptr;
        const auto* angles = base ? base->viewangles() : nullptr;
        if (!angles) return;
        float intent_forward = 0.0f, intent_left = 0.0f;
        const auto axis = [&](std::uint64_t positive, std::uint64_t negative) {
            const bool p = (original_buttons & positive) != 0, n = (original_buttons & negative) != 0;
            if (p != n) return p ? 1.0f : -1.0f;
            if (p && n) return (m_last_pressed & positive) ? 1.0f : -1.0f;
            return 0.0f;
        };
        intent_forward = axis(forward, back); intent_left = axis(left, right);
        // Space-only bunnyhopping still accelerates along the camera direction.
        if (intent_forward == 0.0f && intent_left == 0.0f) intent_forward = 1.0f;
        constexpr float degrees = 180.0f / std::numbers::pi_v<float>;
        const float camera_yaw = systems::g_input.get_view_angles().y;
        const float command_yaw = angles->y();
        const float intent = camera_yaw + std::atan2(intent_left, intent_forward) * degrees;
        if (!std::isfinite(intent) || !std::isfinite(command_yaw)) return;
        const auto cvar_float = [](c_convar* var, float fallback) { return var ? var->get<float>() : fallback; };
        const float air_accel = cvar_float(CONVAR("sv_airaccelerate"), 12.0f);
        const float air_cap = cvar_float(CONVAR("sv_air_max_wishspeed"), 30.0f);
        const float server_max = cvar_float(CONVAR("sv_maxspeed"), 320.0f);
        float player_max = server_max;
        const auto services_offset = SCHEMA("C_BasePlayerPawn", "m_pMovementServices"_hash);
        const auto services = services_offset ? memory::safe_read<std::uintptr_t>(local.pawn + services_offset).value_or(0) : 0;
        if (valid_runtime_address(services)) {
            if (const auto offset = SCHEMA("CPlayer_MovementServices", "m_flMaxspeed"_hash)) {
                const auto value = memory::safe_read<float>(services + offset);
                if (value && std::isfinite(*value) && *value > 0.0f) player_max = *value;
            }
        }
        float friction = pre.surface_friction;
        if (!std::isfinite(friction) || friction <= 0.0f) friction = 1.0f;
        const float wishspeed = std::fmin(server_max, player_max);
        const float cap = std::fmin(wishspeed, air_cap);
        if (!std::isfinite(air_accel) || !std::isfinite(air_cap) || !std::isfinite(server_max) ||
            air_accel <= 0.0f || air_cap <= 0.0f || server_max <= 0.0f || friction > 4.0f) return;
        auto* moves = base->mutable_subtick_moves();
        if (!moves) return;
        const int old_size = moves->m_current_size;
        // Eight segments suffice; leave four slots for jumpbug/other button edges.
        const int count = std::clamp(32 - 4 - old_size, 0, 8);
        if (count == 0) return;
        const float dt = cstypes::tick_interval / static_cast<float>(count);
        const float accel = air_accel * wishspeed * friction * dt;
        float vx = pre.networked_velocity.x, vy = pre.networked_velocity.y;
        float previous_forward = pre.last_movement_impulses.x, previous_left = pre.last_movement_impulses.y;
        if (!std::isfinite(vx) || !std::isfinite(vy) || !std::isfinite(previous_forward) ||
            !std::isfinite(previous_left) || !std::isfinite(accel) || accel <= 0.0f) return;
        const bool old_toggle = m_side_toggle;
        for (int i = 0; i < count; ++i) {
            const float yaw = air_strafe_math::wish_yaw(vx, vy, intent, cap, accel, m_side_toggle);
            const float relative = (yaw - command_yaw) / degrees;
            const float f = std::cos(relative), l = std::sin(relative);
            if (!emit_step(base, static_cast<float>(i) / static_cast<float>(count), f - previous_forward, l - previous_left)) {
                moves->m_current_size = old_size; m_side_toggle = old_toggle; return;
            }
            previous_forward = f; previous_left = l;
            const float radians = yaw / degrees;
            const float x = std::cos(radians), y = std::sin(radians);
            const float gain = std::clamp(cap - (vx * x + vy * y), 0.0f, accel);
            vx += x * gain; vy += y * gain;
            m_side_toggle = !m_side_toggle;
        }
        // Replace earlier analog deltas only after the entire plan succeeds.
        // Button events (including the predicted landing jump) remain intact.
        for (int i = 0; i < old_size; ++i) {
            if (auto* step = base->mutable_subtick_moves(i)) {
                step->set_analog_forward_delta(0.0f); step->set_analog_left_delta(0.0f);
            }
        }
        base->set_forwardmove(previous_forward); base->set_leftmove(previous_left);
        m_handled_this_tick = true;
    }
}
