#pragma once

#include <cmath>

namespace features::movement {
    // A submitted command is an attempt, not confirmation of a successful hop.
    // Keep the landing/ascending phase latched, but allow a retry while falling
    // and re-arm for the descent after a successful hop (possibly off a ledge).
    class jumpbug_cycle {
        bool waiting_ = false;
        int ground_ticks_ = 0;
    public:
        void fired() { waiting_ = true; ground_ticks_ = 0; }
        bool available(bool on_ground, float vertical_speed) {
            if (!std::isfinite(vertical_speed)) return false;
            if (!waiting_) return true;
            if (!on_ground && vertical_speed < 0.0f) {
                waiting_ = false;
                ground_ticks_ = 0;
                return true;
            }
            ground_ticks_ = on_ground && vertical_speed <= 0.0f ? ground_ticks_ + 1 : 0;
            if (ground_ticks_ >= 2) { waiting_ = false; ground_ticks_ = 0; }
            return !waiting_;
        }
    };
}
