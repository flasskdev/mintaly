#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>

namespace features::misc::throw_compensation {
    using vector = std::array<float, 3>;
    struct solution { vector direction; float forward_speed; };

    // Solve speed * direction + inherited = forward_speed * desired.
    // A weak throw may have no forward solution; never generate NaN angles.
    [[nodiscard]] inline std::optional<solution> solve(vector desired, vector inherited, float speed) {
        if (!std::isfinite(speed) || speed <= 0.0f) return std::nullopt;
        double norm2 = 0.0;
        for (int i = 0; i < 3; ++i) {
            if (!std::isfinite(desired[i]) || !std::isfinite(inherited[i])) return std::nullopt;
            norm2 += static_cast<double>(desired[i]) * desired[i];
        }
        if (norm2 < 1.0e-12) return std::nullopt;
        const double norm = std::sqrt(norm2);
        std::array<double, 3> direction{}, perpendicular{};
        double along = 0.0, perpendicular2 = 0.0;
        for (int i = 0; i < 3; ++i) {
            direction[i] = desired[i] / norm;
            along += direction[i] * inherited[i];
        }
        for (int i = 0; i < 3; ++i) {
            perpendicular[i] = inherited[i] - direction[i] * along;
            perpendicular2 += perpendicular[i] * perpendicular[i];
        }
        const double discriminant = static_cast<double>(speed) * speed - perpendicular2;
        if (discriminant < 0.0) return std::nullopt;
        const double forward_component = std::sqrt(discriminant);
        const double forward_speed = along + forward_component;
        if (!std::isfinite(forward_speed) || forward_speed <= 0.0) return std::nullopt;
        solution result{};
        for (int i = 0; i < 3; ++i)
            result.direction[i] = static_cast<float>((direction[i] * forward_component - perpendicular[i]) / speed);
        result.forward_speed = static_cast<float>(forward_speed);
        if (!std::isfinite(result.forward_speed)) return std::nullopt;
        return result;
    }

    [[nodiscard]] inline std::optional<float> input_pitch(float launch_pitch) {
        if (!std::isfinite(launch_pitch)) return std::nullopt;
        const float pitch = (launch_pitch + 10.0f) * (launch_pitch >= -10.0f ? 0.9f : 9.0f / 8.0f);
        // Clamping an unreachable pitch would silently break the solved direction.
        if (pitch < -89.0001f || pitch > 89.0001f) return std::nullopt;
        return std::clamp(pitch, -89.0f, 89.0f);
    }
}
