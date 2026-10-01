#pragma once

#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include <optional>

// Mathematical port of CS2_RESEARCH_MASTER_split, overview 05 and report D09.
// Not an engine ABI, weapon RNG replacement, or bit-exact AVX reconstruction.
// The live combat path is intentionally unchanged pending evaluator/SDK mapping.
namespace features::combat::research_hitchance {
    constexpr std::size_t sample_count = 64;
    struct sample { float x{}, y{}; };
    using sample_table = std::array<sample, sample_count>;
    struct outcome {
        float score{};
        bool valid{}; // Caller must supply a genuine positive evaluator result.
        std::uint8_t category{};
    };
    using outcomes = std::array<outcome, sample_count>;
    struct thresholds { float minimum{}, health{}; bool special_enabled{}; };
    struct aggregates {
        float minimum_fraction{}, health_fraction{}, special_fraction{}, mean_score{};
        std::size_t evaluated{};
    };

    [[nodiscard]] inline bool valid_thresholds(thresholds t)
    {
        return std::isfinite(t.minimum) && t.minimum > 0.0f &&
            std::isfinite(t.health) && t.health > 0.0f;
    }
    [[nodiscard]] inline bool valid_outcome(outcome o)
    {
        return std::isfinite(o.score) && (!o.valid || o.score > 0.0f);
    }
    [[nodiscard]] inline bool special(outcome o, thresholds t)
    {
        return o.valid && t.special_enabled && (o.category & 0xfeu) == 2u;
    }
    [[nodiscard]] inline std::optional<sample_table> ring_samples(float scale)
    {
        if (!std::isfinite(scale) || scale < 0.0f) return std::nullopt;
        sample_table result{};
        for (std::size_t ring = 1; ring <= 8; ++ring)
            for (std::size_t sector = 0; sector < 8; ++sector)
            {
                const float radius = static_cast<float>(ring) / 8.0f * scale;
                const float angle = static_cast<float>(ring) * (std::numbers::pi_v<float> / 8.0f) +
                    static_cast<float>(sector) * (std::numbers::pi_v<float> / 4.0f);
                result[(ring - 1) * 8 + sector] = {radius * std::cos(angle), radius * std::sin(angle)};
            }
        return result;
    }

    [[nodiscard]] inline std::optional<aggregates> aggregate_full(const outcomes& values, thresholds t)
    {
        if (!valid_thresholds(t)) return std::nullopt;
        aggregates result{};
        for (const auto o : values)
        {
            if (!valid_outcome(o)) return std::nullopt;
            const float score = o.valid ? o.score : 0.0f;
            result.minimum_fraction += o.valid && score >= t.minimum ? 1.0f : 0.0f;
            result.health_fraction += score >= t.health ? 1.0f : 0.0f;
            result.special_fraction += special(o, t) ? 1.0f : 0.0f;
            result.mean_score += score;
        }
        result.minimum_fraction *= 1.0f / 64.0f;
        result.health_fraction *= 1.0f / 64.0f;
        result.special_fraction *= 1.0f / 64.0f;
        result.mean_score *= 1.0f / 64.0f;
        result.evaluated = sample_count;
        return result;
    }

    // Evaluator takes a sample index. Direction/trace adapters remain caller-owned.
    // This is an extrapolating estimate, NOT an exact probability or pruning bound.
    template <typename Evaluate>
    [[nodiscard]] std::optional<aggregates> evaluate_fast(thresholds t, Evaluate&& evaluate)
    {
        if (!valid_thresholds(t)) return std::nullopt;
        aggregates result{};
        for (int block = 7; block >= 0; --block)
        {
            unsigned minimum = 0, health = 0, category = 0;
            float sum = 0.0f;
            for (std::size_t offset = 0; offset < 8; ++offset)
            {
                const outcome o = evaluate(static_cast<std::size_t>(block) * 8 + offset);
                if (!valid_outcome(o)) return std::nullopt;
                const float score = o.valid ? o.score : 0.0f;
                minimum += o.valid && score >= t.minimum;
                health += o.valid && score > t.health; // Deliberately strict in fast path.
                category += special(o, t);
                sum += score;
            }
            result.evaluated += 8;
            result.minimum_fraction += static_cast<float>(minimum);
            result.health_fraction += static_cast<float>(health);
            result.special_fraction += static_cast<float>(category);
            result.mean_score += sum;
            if (minimum == 8 && (health == 0 || health == 8))
            {
                const float remaining = static_cast<float>(block * 8);
                result.minimum_fraction += remaining;
                if (health == 8) result.health_fraction += remaining;
                if (category == 8) result.special_fraction += remaining;
                result.mean_score += remaining * (sum * 0.125f);
                break;
            }
        }
        result.minimum_fraction *= 1.0f / 64.0f;
        result.health_fraction *= 1.0f / 64.0f;
        result.special_fraction *= 1.0f / 64.0f;
        result.mean_score *= 1.0f / 64.0f;
        return result;
    }

    [[nodiscard]] inline std::optional<aggregates> aggregate_fast(const outcomes& values, thresholds t)
    {
        if (!valid_thresholds(t)) return std::nullopt;
        // Validate the ENTIRE supplied collection before extrapolating its prefix.
        for (const auto o : values) if (!valid_outcome(o)) return std::nullopt;
        return evaluate_fast(t, [&](std::size_t index) { return values[index]; });
    }

    // Only for an already-selected central candidate. Never apply this shortcut
    // to an unsuccessful trace. Native zero-scale comparison remains inclusive.
    [[nodiscard]] inline aggregates zero_scale_aggregate(float central_score,
        float health_threshold, std::uint8_t category, bool special_enabled)
    {
        return {1.0f, central_score >= health_threshold ? 1.0f : 0.0f,
            special_enabled && (category & 0xfeu) == 2u ? 1.0f : 0.0f, central_score, 0};
    }

    // Worker geometry uses a supplied, prepared frame: the native vertical and
    // degenerate basis-construction branches have NOT been guessed here.
    template <typename Vector>
    struct frame { Vector origin, forward, basis1, basis2; float range{}; };

    template <typename Vector>
    [[nodiscard]] std::optional<Vector> endpoint(const frame<Vector>& f, sample s)
    {
        const auto finite = [](const Vector& v) {
            return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
        };
        if (!finite(f.origin) || !finite(f.forward) || !finite(f.basis1) || !finite(f.basis2) ||
            !std::isfinite(s.x) || !std::isfinite(s.y) || !std::isfinite(f.range) || f.range <= 0.0f)
            return std::nullopt;
        const Vector direction{f.forward.x + s.x * f.basis1.x + s.y * f.basis2.x,
            f.forward.y + s.x * f.basis1.y + s.y * f.basis2.y,
            f.forward.z + s.x * f.basis1.z + s.y * f.basis2.z};
        const float length = std::hypot(direction.x, direction.y, direction.z);
        if (!std::isfinite(length) || length <= 0.0f) return std::nullopt;
        const Vector result{f.origin.x + f.range * (direction.x / length),
            f.origin.y + f.range * (direction.y / length),
            f.origin.z + f.range * (direction.z / length)};
        return finite(result) ? std::optional<Vector>{result} : std::nullopt;
    }

    // One job per command. Run worker() concurrently only with thread-safe
    // evaluators, immutable frame/table, and separate callback state as needed.
    // Join ALL workers before reading values or destroying the job. No implicit
    // engine thread-safety promise and no detached threads or global counters.
    class worker_job {
    public:
        outcomes values{};
        template <typename Evaluate>
        void worker(Evaluate&& evaluate)
        {
            std::size_t index = m_next.load(std::memory_order_relaxed);
            while (index < sample_count)
            {
                if (!m_next.compare_exchange_weak(index, index + 1, std::memory_order_relaxed))
                    continue;
                values[index] = evaluate(index);
                index = m_next.load(std::memory_order_relaxed);
            }
        }
    private:
        std::atomic<std::size_t> m_next{};
    };

    [[nodiscard]] inline std::optional<float> distance_attenuation(float range_parameter, float distance)
    {
        if (!std::isfinite(range_parameter) || range_parameter <= 0.0f ||
            !std::isfinite(distance) || distance < 0.0f) return std::nullopt;
        return std::pow(range_parameter, distance / 500.0f);
    }
}
