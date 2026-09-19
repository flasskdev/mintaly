#pragma once

namespace features::movement {
    // A synthetic jump starts another airborne arc. Do not re-arm on that arc,
    // or on a single transient grounded sample at the original landing.
    class jumpbug_cycle {
        bool waiting_ = false;
        int ground_ticks_ = 0;
    public:
        void fired() { waiting_ = true; ground_ticks_ = 0; }
        bool available(bool on_ground, float vertical_speed) {
            if (!waiting_) return true;
            ground_ticks_ = on_ground && vertical_speed <= 0.0f ? ground_ticks_ + 1 : 0;
            if (ground_ticks_ >= 2) { waiting_ = false; ground_ticks_ = 0; }
            return !waiting_;
        }
    };
}
