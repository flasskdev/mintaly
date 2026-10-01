#pragma once
#include <algorithm>
#include <cmath>
#include <numbers>

namespace features::movement::air_strafe_math {
    inline float normalize(float angle) noexcept {
        return std::remainder(angle, 360.0f);
    }
    // Maximize speed gain under Source's capped projection constraint.
    inline float wish_yaw(float vx, float vy, float intent, float cap,
                          float acceleration, bool positive_side) noexcept {
        const float speed = std::hypot(vx, vy);
        const float projection = std::max(0.0f, cap - acceleration);
        if (speed <= projection || speed < 0.0001f) return normalize(intent);
        constexpr float degrees = 180.0f / std::numbers::pi_v<float>;
        const float velocity_yaw = std::atan2(vy, vx) * degrees;
        const float delta = normalize(intent - velocity_yaw);
        const float theta = std::acos(std::clamp(projection / speed, 0.0f, 1.0f)) * degrees;
        const bool ambiguous = std::fabs(delta) < 0.01f || std::fabs(std::fabs(delta) - 180.0f) < 0.01f;
        const float side = ambiguous ? (positive_side ? 1.0f : -1.0f) : (delta > 0.0f ? 1.0f : -1.0f);
        return normalize(velocity_yaw + side * theta);
    }
}
