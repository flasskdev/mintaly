#pragma once

#include <cmath>
#include <optional>

namespace features::movement::jumpbug_timing {
    // Keep release and jump in distinct, ordered subticks, both strictly < 1.
    inline constexpr float event_gap = 1.0f / 1024.0f;
    inline constexpr float latest_release = 1.0f - 2.0f * event_gap;

    // Find first contact with a swept probe, not a uniform sampling of a narrow
    // landing window. The callback returns nullopt for unusable traces; callers
    // must validate actual hull clearance/ground support at the returned time.
    template <typename Probe>
    [[nodiscard]] std::optional<float> find_contact_time(Probe&& reached)
    {
        const auto initial = reached(0.0f);
        if (!initial) return std::nullopt;
        if (*initial) return 0.0f;
        const auto final = reached(latest_release);
        if (!final || !*final) return std::nullopt;
        float low = 0.0f;
        float high = latest_release;
        for (int i = 0; i < 16; ++i)
        {
            const auto middle = low + (high - low) * 0.5f;
            const auto contact = reached(middle);
            if (!contact) return std::nullopt;
            if (*contact) high = middle;
            else low = middle;
        }
        return high;
    }
}
