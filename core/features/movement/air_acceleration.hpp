#pragma once

#include <algorithm>
#include <cmath>
#include <numbers>

namespace features::movement::detail {

    // Maximize the speed gain of the same capped air-acceleration step used
    // below. With enough acceleration the optimum is perpendicular, not
    // cap / (2 * speed) or an arbitrary "aggressive" multiplier.
    [[nodiscard]] inline float ideal_air_angle(float speed, float dt, float wishspeed,
        float air_accel, float friction, float wishspeed_cap)
    {
        if (speed <= 0.0f)
            return 0.0f;

        const auto cap = std::min(wishspeed, wishspeed_cap);
        const auto acceleration = wishspeed * air_accel * friction * dt;
        const auto projection = std::clamp(cap - acceleration, 0.0f, speed);
        return std::acos(projection / speed) * (180.0f / std::numbers::pi_v<float>);
    }

    inline void simulate_air_acceleration(float& vx, float& vy, float yaw, float dt,
        float wishspeed, float air_accel, float friction, float wishspeed_cap)
    {
        const auto radians = yaw * (std::numbers::pi_v<float> / 180.0f);
        const auto dx = std::cos(radians);
        const auto dy = std::sin(radians);
        const auto available = std::min(wishspeed, wishspeed_cap) - (vx * dx + vy * dy);
        if (available <= 0.0f)
            return;

        const auto step = std::min(wishspeed * air_accel * friction * dt, available);
        vx += dx * step;
        vy += dy * step;
    }

} // namespace features::movement::detail
