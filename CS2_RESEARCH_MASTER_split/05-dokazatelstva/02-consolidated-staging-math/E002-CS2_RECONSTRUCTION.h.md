<!-- split-part | CS2_RESEARCH_MASTER.md lines 3825-3891 | body-sha256 5354d051a1807b1fd4daf23369539faa0fb52b04336b1748ec329cf3fa57d8a4 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-002"></a>

## E002. `analysis/consolidated/staging/math/CS2_RECONSTRUCTION.h`

Bytes: 2077. SHA-256: `5ffef91ad7cd92af57663e9f2d4cbd784a71186ace31950ce264831fdb0b2fe8`.

```cpp
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace cs2_reconstruction::model {

inline constexpr std::size_t sample_count = 64;

struct SampleOutcome {
    const double score;
    const bool valid = true;
    const std::int64_t category = 0;
};

struct Aggregates {
    const double minimum_score_fraction;
    const double health_score_fraction;
    const double special_fraction;
    const double mean_score;
    const std::size_t evaluated_samples;
};

using SampleTable = std::array<std::pair<double, double>, sample_count>;
using SampleOutcomes = std::vector<SampleOutcome>;
using WeightDamagePairs = std::vector<std::pair<double, double>>;

SampleTable ring_samples(double scale = 1.0);
SampleTable golden_samples(double scale = 1.0);

SampleOutcomes checked_outcomes(const SampleOutcomes& outcomes,
                               double minimum_score, double health_score);
Aggregates aggregate_full(const SampleOutcomes& outcomes,
                          double minimum_score, double health_score,
                          bool special_enabled);
Aggregates aggregate_fast(const SampleOutcomes& outcomes,
                          double minimum_score, double health_score,
                          bool special_enabled);
Aggregates zero_scale_aggregate(double central_score, double health_score,
                                std::int64_t category, bool special_enabled);

double estimated_health(double base_health,
                        const WeightDamagePairs& matching_weight_damage);
double minimum_damage_threshold(double setting, double health,
                                bool half_health_mode);
double distance_attenuation(double range_parameter, double distance);

std::size_t retained_count_after_tail_aging(
    const std::vector<std::int64_t>& newest_first_ticks, std::int64_t cutoff_tick);
bool resets_history(std::int64_t delta_tick, double squared_distance,
                    std::int64_t new_collection_size,
                    std::int64_t previous_collection_size);

}
```
