#pragma once

#include <cmath>

namespace features::combat::damage_validation {
    [[nodiscard]] inline bool positive_finite(float damage) noexcept
    {
        return std::isfinite(damage) && damage > 0.0f;
    }

    // A negative comparison alone (damage < minimum) lets NaN pass.
    // Zero minimum is supported, but zero/negative/invalid damage never fires.
    [[nodiscard]] inline bool meets_minimum(float damage, float minimum) noexcept
    {
        return positive_finite(damage) && std::isfinite(minimum) &&
            minimum >= 0.0f && damage >= minimum;
    }
}
