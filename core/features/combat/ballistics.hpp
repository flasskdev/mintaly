#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <optional>

namespace features::combat::ballistics {

    template <typename Vector>
    [[nodiscard]] bool finite(const Vector& value)
    {
        return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
    }

    // Closed finite segment against an axis-aligned box, in box-local space.
    // A zero-length segment hits only when its origin is inside the box.
    template <typename Vector>
    [[nodiscard]] bool segment_box(const Vector& origin, const Vector& delta,
        const Vector& minimum, const Vector& maximum, float& fraction)
    {
        fraction = 1.0f;
        if (!finite(origin) || !finite(delta) || !finite(minimum) || !finite(maximum) ||
            minimum.x > maximum.x || minimum.y > maximum.y || minimum.z > maximum.z)
            return false;

        auto entry = 0.0f;
        auto exit = 1.0f;
        const auto axis = [&](float start, float direction, float low, float high)
        {
            if (direction == 0.0f)
                return start >= low && start <= high;
            auto first = (low - start) / direction;
            auto second = (high - start) / direction;
            if (first > second)
                std::swap(first, second);
            entry = std::max(entry, first);
            exit = std::min(exit, second);
            return entry <= exit;
        };
        if (!axis(origin.x, delta.x, minimum.x, maximum.x) ||
            !axis(origin.y, delta.y, minimum.y, maximum.y) ||
            !axis(origin.z, delta.z, minimum.z, maximum.z))
            return false;
        fraction = entry;
        return true;
    }

    // The percentage API must count every sample. A quiet prefix does not
    // establish an upper bound on hits in the untested suffix.
    template <typename Predicate>
    [[nodiscard]] float sample_ratio(int count, Predicate&& intersects)
    {
        if (count <= 0)
            return 0.0f;
        auto hits = 0;
        for (auto i = 0; i < count; ++i)
            hits += intersects(i) ? 1 : 0;
        return static_cast<float>(hits) / static_cast<float>(count);
    }

    // Monotone in hitchance for positive damage/health. Evaluating at 1.0
    // therefore gives a conservative upper bound for target pruning.
    [[nodiscard]] inline float target_score(float damage, int health, float hitchance,
        float needed, bool no_spread, bool penetrated, bool center, int priority, float fov)
    {
        auto score = no_spread || hitchance >= needed ? 1000000.0f : 0.0f;
        if (damage >= static_cast<float>(health))
            score += 100000.0f + hitchance * 10000.0f;
        else
            score += damage * hitchance * 100.0f + damage * 5.0f;
        score += penetrated ? 0.0f : 250.0f;
        score += center ? 50.0f : 0.0f;
        score += static_cast<float>(priority) * 2.0f;
        score -= fov * 0.1f;
        return score;
    }

    // Seed generation and command serialization must see exactly the same
    // recoil-compensated angles. Keep roll: the inverse-spread solver uses it.
    template <typename Angle>
    [[nodiscard]] Angle command_angles(const Angle& ballistic, const Angle& punch)
    {
        return Angle{ballistic.x - punch.x, ballistic.y - punch.y, ballistic.z};
    }

    // Engine-independent control flow: callbacks supply the real seed hash
    // and inverse-spread calculation. No RNG approximation or heap allocation.
    template <typename Angle, typename Seed, typename Correct>
    [[nodiscard]] std::optional<Angle> solve_spread(const Angle& aim, Seed&& seed_for,
        Correct&& correct)
    {
        if (!finite(aim))
            return std::nullopt;

        constexpr auto iteration_limit = 12;
        constexpr auto grid_samples = 240;
        // At most 252 insertions into 512 slots. Exact keys (including zero
        // and UINT32_MAX), no allocation and no false-positive rejection.
        std::array<std::uint32_t, 512> visited{};
        std::array<bool, 512> occupied{};
        const auto first_visit = [&](std::uint32_t value)
        {
            auto slot = static_cast<std::size_t>((value * 2654435761u) & 511u);
            while (occupied[slot])
            {
                if (visited[slot] == value)
                    return false;
                slot = (slot + 1) & 511u;
            }
            occupied[slot] = true;
            visited[slot] = value;
            return true;
        };
        auto seed = seed_for(aim);
        for (auto i = 0; i < iteration_limit; ++i)
        {
            if (!first_visit(seed))
                break;
            const auto angle = correct(seed);
            if (!angle || !finite(*angle))
                break;
            const auto next = seed_for(*angle);
            if (next == seed)
                return angle;
            seed = next;
        }

        // Keep the existing full-pitch search as a deterministic fallback.
        // A failed solve is not represented by a valid zero-degree angle.
        for (auto i = 0; i < grid_samples; ++i)
        {
            seed = seed_for(Angle{static_cast<float>(i) * 1.5f, aim.y, 0.0f});
            if (!first_visit(seed))
                continue;
            const auto angle = correct(seed);
            if (angle && finite(*angle) && seed_for(*angle) == seed)
                return angle;
        }
        return std::nullopt;
    }
}
