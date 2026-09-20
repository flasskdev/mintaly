#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include "jumpbug_timing.hpp"

namespace features::movement::jumpbug_command {
    struct event {
        std::uint64_t button{};
        bool pressed{};
        float when{};
    };
    struct plan {
        std::array<event, 4> events{};
        int count{};
        std::uint64_t final_buttons{};
        bool owns_duck{};
        bool owns_jump{};
    };

    // The protobuf button summary must agree with the final state of the
    // subtick stream, not with the crouch preparation at the start of the tick.
    inline std::optional<plan> make(std::uint64_t buttons, std::uint64_t jump,
        std::uint64_t duck, std::optional<float> release, bool include_jump) {
        if (release && (!std::isfinite(*release) || *release < 0.0f ||
            *release > jumpbug_timing::latest_release)) return std::nullopt;
        plan result{};
        result.final_buttons = buttons & ~(jump | duck);
        result.events[result.count++] = {jump, false, 0.0f};
        // Already in the window: do not press and release duck at the same time.
        if (!release || *release > 0.0f)
            result.events[result.count++] = {duck, true, 0.0f};
        if (release) {
            result.events[result.count++] = {duck, false, *release};
            if (include_jump) {
                result.events[result.count++] = {jump, true, *release + jumpbug_timing::event_gap};
                result.final_buttons |= jump;
                result.owns_jump = true;
            }
        } else {
            result.final_buttons |= duck;
            result.owns_duck = true;
        }
        return result;
    }
}
