#pragma once

#include <array>
#include <cmath>
#include <cstdint>

namespace utilities::rage_shot_diagnostics {
    enum class acceptance { none, contact, geometry };
    struct segment {
        bool present{};
        float enter{}, exit{}, damage{};
        int team_raw{};
        std::uint16_t enter_index{}, exit_index{};
        std::uint8_t flags_raw{}; // Observed byte only; not a penetration predicate.
    };
    // Owned values only: no pointers into a trace workspace or live entity state.
    struct snapshot {
        acceptance method{};
        int selected{-1}, count{};
        float target_fraction{}, range{}, damage_scaled{};
        float first_boundary{};
        bool visibility_blocked{};
        std::array<float, 3> start{}, requested_end{};
        segment previous{}, chosen{}, next{};
    };
    [[nodiscard]] inline const char* method_name(acceptance value)
    {
        switch (value) {
        case acceptance::contact: return "contact";
        case acceptance::geometry: return "geometry";
        default: return "unavailable";
        }
    }
    // Diagnostic relation only. Do not use it to accept/reject a shot or infer
    // material solidity: enter/exit are the current trace-record interpretation.
    [[nodiscard]] inline const char* relation(float target, const segment& value)
    {
        if (!value.present || !std::isfinite(target) || !std::isfinite(value.enter) ||
            !std::isfinite(value.exit) || value.enter > value.exit)
            return "unknown";
        if (target < value.enter) return "before";
        if (target > value.exit) return "after";
        return "inside";
    }
    // Owner-thread only, called under rage's command mutex after attack_set.
    // observed counts attack-setting calls, NOT server-confirmed weapon_fire.
    struct report_budget {
        std::uint64_t observed{}, emitted{}, suppressed{}, last_ms{};
        [[nodiscard]] bool take(std::uint64_t now_ms)
        {
            ++observed;
            if (emitted >= 32 || (emitted && (now_ms < last_ms || now_ms - last_ms < 1000))) {
                ++suppressed;
                return false;
            }
            last_ms = now_ms;
            ++emitted;
            return true;
        }
    };
}
