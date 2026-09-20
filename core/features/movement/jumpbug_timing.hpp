#pragma once

#include <cmath>
#include <optional>

namespace features::movement::jumpbug_timing {
    // Keep release and jump in distinct, ordered subticks, both strictly < 1.
    inline constexpr float event_gap = 1.0f / 1024.0f;
    inline constexpr float latest_release = 1.0f - 2.0f * event_gap;

    // Airborne unduck preserves the hull center: half the height difference
    // extends below the crouched feet, the other half above the crouched head.
    [[nodiscard]] inline std::optional<float> airborne_unduck_expansion(float crouched_height, float standing_height) {
        if (!std::isfinite(crouched_height) || !std::isfinite(standing_height) ||
            crouched_height <= 0.0f || standing_height < crouched_height) return std::nullopt;
        return (standing_height - crouched_height) * 0.5f;
    }

    // Match the half-gravity step used by the movement landing predictor:
    // gravity changes velocity before the tick's movement sweep, not along a
    // separate parabolic chord for each binary-search candidate.
    [[nodiscard]] inline std::optional<float> movement_velocity_z(float velocity, float gravity, float interval) {
        if (!std::isfinite(velocity) || !std::isfinite(gravity) || gravity < 0.0f ||
            !std::isfinite(interval) || interval <= 0.0f) return std::nullopt;
        const float result = velocity - 0.5f * gravity * interval;
        return std::isfinite(result) ? std::optional<float>{result} : std::nullopt;
    }

    // Find first contact with a swept probe, not a uniform sampling of a narrow
    // landing window. reached(t) must test the entire path [0, t], so contact
    // remains true after passing a thin ledge. Testing only support at t breaks
    // the monotonic predicate required by binary search.
    // The callback returns nullopt for unusable traces; callers must validate
    // actual hull clearance/ground support at the returned time.
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

    // A stationary expanded probe can start inside the floor even though the
    // actual standing hull is clear and supported. Check that valid immediate
    // release before using the swept probe, which must reject solid traces.
    template <typename Probe, typename SafeRelease>
    [[nodiscard]] std::optional<float> find_release_time(Probe&& reached, SafeRelease&& safe_release) {
        if (safe_release(0.0f)) return 0.0f;
        const auto when = find_contact_time(reached);
        if (!when || !safe_release(*when)) return std::nullopt;
        return when;
    }
}
