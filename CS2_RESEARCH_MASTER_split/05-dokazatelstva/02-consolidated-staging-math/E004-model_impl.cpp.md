<!-- split-part | CS2_RESEARCH_MASTER.md lines 4320-4525 | body-sha256 bbaf93d15917ab1f06855dfd1b4762ed2311bdaa3af100cfd846f859636a9c07 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-004"></a>

## E004. `analysis/consolidated/staging/math/model_impl.cpp`

Bytes: 8030. SHA-256: `6a3e13925131fb381c72543af15e73724d42f74e68fc598f6fa380b0464774fb`.

```cpp
#include "CS2_RECONSTRUCTION.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace cs2_reconstruction::model {

namespace {

constexpr double pi = 3.141592653589793238462643383279502884;
constexpr std::size_t block_size = 8;
constexpr std::size_t block_count = sample_count / block_size;

bool has_special_category(std::int64_t category) {
    return (static_cast<std::uint64_t>(category) & 0xfeu) == 2u;
}

}

SampleTable ring_samples(double scale) {
    SampleTable samples{};
    std::size_t sample_index = 0;
    for (std::size_t ring_index = 1; ring_index <= 8; ++ring_index) {
        for (std::size_t sector_index = 0; sector_index < 8; ++sector_index) {
            const double radius = scale * static_cast<double>(ring_index) / 8.0;
            const double angle = static_cast<double>(ring_index) * pi / 8.0
                               + static_cast<double>(sector_index) * pi / 4.0;
            samples[sample_index++] = {radius * std::cos(angle),
                                       radius * std::sin(angle)};
        }
    }
    return samples;
}

SampleTable golden_samples(double scale) {
    constexpr double angle_step = 2.399963140487671;
    SampleTable samples{};
    for (std::size_t sample_index = 0; sample_index < sample_count; ++sample_index) {
        const double radius = scale * std::sqrt(
            static_cast<double>(sample_index + 1) / 64.0);
        const double angle = static_cast<double>(sample_index) * angle_step;
        samples[sample_index] = {radius * std::cos(angle), radius * std::sin(angle)};
    }
    return samples;
}

SampleOutcomes checked_outcomes(const SampleOutcomes& outcomes,
                               double minimum_score, double health_score) {
    if (outcomes.size() != sample_count) {
        throw std::invalid_argument(
            "Exactly 64 already-evaluated sample outcomes are required");
    }
    if (!std::isfinite(minimum_score) || minimum_score <= 0.0
        || !std::isfinite(health_score) || health_score <= 0.0) {
        throw std::invalid_argument(
            "This mathematical model covers positive finite thresholds only");
    }
    for (const auto& outcome : outcomes) {
        if (!std::isfinite(outcome.score) || (outcome.valid && outcome.score <= 0.0)) {
            throw std::invalid_argument("Valid sample scores must be positive and finite");
        }
    }
    SampleOutcomes samples;
    samples.reserve(sample_count);
    for (const auto& outcome : outcomes) {
        samples.push_back(outcome.valid
            ? outcome : SampleOutcome{0.0, false, outcome.category});
    }
    return samples;
}

Aggregates aggregate_full(const SampleOutcomes& outcomes,
                          double minimum_score, double health_score,
                          bool special_enabled) {
    const auto samples = checked_outcomes(outcomes, minimum_score, health_score);
    std::size_t count_minimum = 0;
    std::size_t count_health = 0;
    std::size_t count_special = 0;
    double sum_score = 0.0;
    for (const auto& sample : samples) {
        count_minimum += sample.valid && sample.score >= minimum_score;
        count_health += sample.score >= health_score;
        count_special += sample.valid && has_special_category(sample.category);
        sum_score += sample.score;
    }
    return {static_cast<double>(count_minimum) / 64.0,
            static_cast<double>(count_health) / 64.0,
            special_enabled ? static_cast<double>(count_special) / 64.0 : 0.0,
            sum_score / 64.0, sample_count};
}

Aggregates aggregate_fast(const SampleOutcomes& outcomes,
                          double minimum_score, double health_score,
                          bool special_enabled) {
    const auto samples = checked_outcomes(outcomes, minimum_score, health_score);
    std::size_t count_minimum = 0;
    std::size_t count_health = 0;
    std::size_t count_special = 0;
    double sum_score = 0.0;
    std::size_t evaluated = 0;
    for (std::size_t remaining_blocks = block_count; remaining_blocks > 0; --remaining_blocks) {
        const std::size_t block_index = remaining_blocks - 1;
        const std::size_t block_begin = block_index * block_size;
        std::size_t block_minimum = 0;
        std::size_t block_health = 0;
        std::size_t block_special = 0;
        double block_sum = 0.0;
        for (std::size_t sample_index = block_begin;
             sample_index < block_begin + block_size; ++sample_index) {
            const auto& sample = samples[sample_index];
            block_minimum += sample.valid && sample.score >= minimum_score;
            block_health += sample.score > health_score;
            block_special += sample.valid && has_special_category(sample.category);
            block_sum += sample.score;
        }
        count_minimum += block_minimum;
        count_health += block_health;
        count_special += block_special;
        sum_score += block_sum;
        evaluated += block_size;
        if (block_minimum == block_size
            && (block_health == 0 || block_health == block_size)) {
            const std::size_t remaining = block_index * block_size;
            count_minimum += remaining;
            count_health += block_health == block_size ? remaining : 0;
            count_special += block_special == block_size ? remaining : 0;
            sum_score += static_cast<double>(remaining) * block_sum / 8.0;
            break;
        }
    }
    return {static_cast<double>(count_minimum) / 64.0,
            static_cast<double>(count_health) / 64.0,
            special_enabled ? static_cast<double>(count_special) / 64.0 : 0.0,
            sum_score / 64.0, evaluated};
}

Aggregates zero_scale_aggregate(double central_score, double health_score,
                                std::int64_t category, bool special_enabled) {
    return {1.0, static_cast<double>(central_score >= health_score),
            static_cast<double>(special_enabled && has_special_category(category)),
            central_score, 0};
}

double estimated_health(double base_health,
                        const WeightDamagePairs& matching_weight_damage) {
    double weighted_damage = 0.0;
    for (const auto& [weight, damage] : matching_weight_damage) {
        if (weight > 0.75) {
            weighted_damage += weight * damage;
        }
    }
    return base_health - weighted_damage;
}

double minimum_damage_threshold(double setting, double health,
                                bool half_health_mode) {
    if (health <= 0.0) {
        throw std::invalid_argument(
            "The original caller returns before candidate generation for nonpositive health");
    }
    const double selected = setting > 100.0
        ? (half_health_mode ? std::ceil(health * 0.5) : health)
        : std::min(setting, health);
    return std::min(selected, 130.0);
}

double distance_attenuation(double range_parameter, double distance) {
    if (range_parameter <= 0.0 || distance < 0.0) {
        throw std::invalid_argument(
            "This model covers positive range parameters and nonnegative distances only");
    }
    return std::pow(range_parameter, distance / 500.0);
}

std::size_t retained_count_after_tail_aging(
    const std::vector<std::int64_t>& newest_first_ticks, std::int64_t cutoff_tick) {
    const std::size_t original_count = newest_first_ticks.size();
    for (std::size_t kept_count = original_count; kept_count > 0; --kept_count) {
        if (newest_first_ticks[kept_count - 1] >= cutoff_tick) {
            return kept_count;
        }
    }
    return original_count;
}

bool resets_history(std::int64_t delta_tick, double squared_distance,
                    std::int64_t new_collection_size,
                    std::int64_t previous_collection_size) {
    const std::int64_t factor = std::min(std::max(delta_tick, std::int64_t{1}),
                                      std::int64_t{5});
    return squared_distance > static_cast<double>(4096 * factor)
        || new_collection_size != previous_collection_size;
}

}
```
