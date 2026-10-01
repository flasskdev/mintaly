<!-- split-part | CS2_RESEARCH_MASTER.md lines 3892-4319 | body-sha256 d2d8efe98bd870adcd435926bb53d0674685c1184acd0a98004f124ad0b7459b -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-003"></a>

## E003. `analysis/consolidated/staging/math/model_checks.cpp`

Bytes: 24151. SHA-256: `62a3798688faefeab168447cfb058de720a856c7b2334c662b379d470f9b7f64`.

```cpp
#include "CS2_RECONSTRUCTION.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace model = cs2_reconstruction::model;

namespace {

struct CheckResult {
    std::string name;
    bool passed;
};

class CheckRunner {
public:
    void check(const std::string& name, bool condition) {
        results_.push_back({name, condition});
    }

    int report() const {
        const auto passed = std::count_if(results_.begin(), results_.end(),
            [](const auto& result) { return result.passed; });
        const auto failed = results_.size() - static_cast<std::size_t>(passed);
        std::cout << "{\n  \"passed\": " << passed
                  << ",\n  \"failed\": " << failed
                  << ",\n  \"python_check_count\": 23,\n  \"checks\": [\n";
        for (std::size_t check_index = 0; check_index < results_.size(); ++check_index) {
            const auto& result = results_[check_index];
            std::cout << "    {\"name\": " << std::quoted(result.name)
                      << ", \"passed\": " << (result.passed ? "true" : "false") << "}"
                      << (check_index + 1 == results_.size() ? "\n" : ",\n");
        }
        std::cout << "  ],\n  \"scope\": \"Independent mathematical reconstruction only; "
                     "not equivalence testing of the binary\",\n"
                     "  \"sample_binary_executed\": false\n}\n";
        return failed == 0 ? 0 : 1;
    }

private:
    std::vector<CheckResult> results_;
};

bool is_close(double actual, double expected, double absolute_tolerance = 0.0) {
    return std::abs(actual - expected)
        <= std::max(absolute_tolerance,
                    1e-9 * std::max(std::abs(actual), std::abs(expected)));
}

template <typename Factory>
model::SampleOutcomes make_outcomes(Factory factory) {
    model::SampleOutcomes outcomes;
    outcomes.reserve(model::sample_count);
    for (std::size_t sample_index = 0; sample_index < model::sample_count; ++sample_index) {
        outcomes.push_back(factory(sample_index));
    }
    return outcomes;
}

template <typename Operation>
bool rejects_with(Operation operation, const std::string& expected_message) {
    try {
        operation();
    } catch (const std::invalid_argument& error) {
        return error.what() == expected_message;
    } catch (...) {
        return false;
    }
    return false;
}

bool aggregators_reject(const model::SampleOutcomes& outcomes,
                        double minimum_score, double health_score,
                        const std::string& expected_message) {
    return rejects_with([&] {
        model::aggregate_full(outcomes, minimum_score, health_score, false);
    }, expected_message) && rejects_with([&] {
        model::aggregate_fast(outcomes, minimum_score, health_score, false);
    }, expected_message);
}

int run_checks() {
    static_assert(!std::is_assignable_v<model::SampleOutcome&, model::SampleOutcome>);
    static_assert(!std::is_assignable_v<model::Aggregates&, model::Aggregates>);
    CheckRunner checks;
    const model::SampleOutcomes uniform(64, model::SampleOutcome{50.0, true, 2});
    const auto full = model::aggregate_full(uniform, 20.0, 50.0, true);
    const auto fast = model::aggregate_fast(uniform, 20.0, 50.0, true);
    checks.check("full includes equality at health threshold", full.health_score_fraction == 1.0);
    checks.check("fast excludes equality at health threshold", fast.health_score_fraction == 0.0);
    checks.check("uniform block permits early-fill after 8", fast.evaluated_samples == 8);
    checks.check("full always evaluates all supplied 64", full.evaluated_samples == 64);
    checks.check("uniform means and special fractions",
        full.mean_score == 50.0 && fast.mean_score == 50.0
        && full.special_fraction == 1.0 && fast.special_fraction == 1.0);
    checks.check("special mode gate",
        model::aggregate_full(uniform, 20.0, 50.0, false).special_fraction == 0.0);
    const auto nonuniform = make_outcomes([](std::size_t sample_index) {
        return sample_index < 56 ? model::SampleOutcome{0.0, false}
                                 : model::SampleOutcome{60.0, true, 3};
    });
    checks.check("full measured fraction differs from extrapolated fast fraction",
        model::aggregate_full(nonuniform, 20.0, 50.0, true).minimum_score_fraction == 0.125
        && model::aggregate_fast(nonuniform, 20.0, 50.0, true).minimum_score_fraction == 1.0);
    checks.check("unsatisfied A forces all blocks",
        model::aggregate_fast(uniform, 80.0, 100.0, false).evaluated_samples == 64);
    const auto shortcut = model::zero_scale_aggregate(50.0, 50.0, 2, true);
    checks.check("zero-scale shortcut",
        shortcut.minimum_score_fraction == 1.0 && shortcut.health_score_fraction == 1.0
        && shortcut.evaluated_samples == 0);
    const auto ring = model::ring_samples();
    const auto golden = model::golden_samples();
    checks.check("two deterministic tables have 64 pairs", ring.size() == 64 && golden.size() == 64);
    checks.check("ring endpoint radii",
        is_close(std::hypot(ring.front().first, ring.front().second), 0.125)
        && is_close(std::hypot(ring.back().first, ring.back().second), 1.0));
    checks.check("golden endpoint radii",
        is_close(std::hypot(golden.front().first, golden.front().second), 0.125)
        && is_close(std::hypot(golden.back().first, golden.back().second), 1.0));
    checks.check("weight cutoff is strict",
        model::estimated_health(100.0, {{0.75, 20.0}, {0.8, 20.0}}) == 84.0);
    checks.check("minimum setting clips to health",
        model::minimum_damage_threshold(90.0, 40.0, false) == 40.0);
    checks.check("over-100 mode uses health",
        model::minimum_damage_threshold(101.0, 41.0, false) == 41.0);
    checks.check("half-health mode rounds upward",
        model::minimum_damage_threshold(101.0, 41.0, true) == 21.0);
    checks.check("threshold maximum 130",
        model::minimum_damage_threshold(200.0, 500.0, false) == 130.0);
    checks.check("attenuation endpoints",
        model::distance_attenuation(0.98, 0.0) == 1.0
        && is_close(model::distance_attenuation(0.98, 500.0), 0.98));
    checks.check("aging keeps newest prefix",
        model::retained_count_after_tail_aging({100, 95, 90}, 96) == 1);
    checks.check("all-stale aging preserves original count in this block",
        model::retained_count_after_tail_aging({90, 89}, 100) == 2);
    checks.check("discontinuity boundary is strict",
        !model::resets_history(1, 4096.0, 8, 8) && model::resets_history(1, 4097.0, 8, 8));
    checks.check("discontinuity clamp is linear in squared-distance limit",
        !model::resets_history(50, 20480.0, 8, 8) && model::resets_history(50, 20481.0, 8, 8));
    checks.check("collection mismatch resets", model::resets_history(1, 0.0, 7, 8));

    const model::SampleOutcome defaults{1.0};
    checks.check("sample defaults preserve valid and category", defaults.valid && defaults.category == 0);
    checks.check("minimum equality is included by both aggregators",
        model::aggregate_full(uniform, 50.0, 100.0, false).minimum_score_fraction == 1.0
        && model::aggregate_fast(uniform, 50.0, 100.0, false).minimum_score_fraction == 1.0);
    checks.check("fast special mode gate",
        model::aggregate_fast(uniform, 20.0, 50.0, false).special_fraction == 0.0);
    const auto extrapolated = model::aggregate_fast(nonuniform, 20.0, 50.0, true);
    checks.check("fast extrapolates high-health special block",
        extrapolated.health_score_fraction == 1.0 && extrapolated.special_fraction == 1.0
        && extrapolated.mean_score == 60.0 && extrapolated.evaluated_samples == 8);
    const auto invalid_payload = make_outcomes([](std::size_t sample_index) {
        return sample_index == 0 ? model::SampleOutcome{50.0, true, 3}
                                  : model::SampleOutcome{-999.0, false, 2};
    });
    const auto normalized = model::checked_outcomes(invalid_payload, 20.0, 50.0);
    checks.check("checked outcomes normalize invalid scores and preserve category",
        normalized.size() == 64 && normalized[1].score == 0.0
        && !normalized[1].valid && normalized[1].category == 2);
    checks.check("normalization preserves valid scores and input",
        normalized[0].score == 50.0 && normalized[0].valid && normalized[0].category == 3
        && invalid_payload[1].score == -999.0);
    const auto sparse = model::aggregate_full(invalid_payload, 20.0, 50.0, true);
    checks.check("invalid scores contribute neither fractions nor mean",
        sparse.minimum_score_fraction == 1.0 / 64.0
        && sparse.health_score_fraction == 1.0 / 64.0
        && sparse.special_fraction == 1.0 / 64.0 && sparse.mean_score == 50.0 / 64.0);
    const model::SampleOutcomes invalid_positive(64, model::SampleOutcome{500.0, false, 3});
    const auto empty_full = model::aggregate_full(invalid_positive, 20.0, 50.0, true);
    const auto empty_fast = model::aggregate_fast(invalid_positive, 20.0, 50.0, true);
    checks.check("invalid positive scores are also zeroed",
        empty_full.mean_score == 0.0 && empty_full.minimum_score_fraction == 0.0
        && empty_full.health_score_fraction == 0.0 && empty_full.special_fraction == 0.0);
    checks.check("all-invalid fast input evaluates every block",
        empty_fast.mean_score == 0.0 && empty_fast.minimum_score_fraction == 0.0
        && empty_fast.health_score_fraction == 0.0 && empty_fast.special_fraction == 0.0
        && empty_fast.evaluated_samples == 64);
    const auto mixed_special = make_outcomes([](std::size_t sample_index) {
        return sample_index < 56 ? model::SampleOutcome{0.0, false}
            : model::SampleOutcome{60.0, true, sample_index % 2 == 0 ? 2 : 4};
    });
    const auto mixed_special_fast = model::aggregate_fast(mixed_special, 20.0, 50.0, true);
    checks.check("fast early-fill does not extrapolate mixed special categories",
        mixed_special_fast.special_fraction == 4.0 / 64.0
        && mixed_special_fast.minimum_score_fraction == 1.0
        && mixed_special_fast.health_score_fraction == 1.0
        && mixed_special_fast.mean_score == 60.0 && mixed_special_fast.evaluated_samples == 8);
    const auto two_blocks = make_outcomes([](std::size_t sample_index) {
        if (sample_index < 48) {
            return model::SampleOutcome{0.0, false};
        }
        if (sample_index < 56) {
            return model::SampleOutcome{80.0, true, 2};
        }
        return model::SampleOutcome{sample_index % 2 == 0 ? 40.0 : 60.0, true, 0};
    });
    const auto second_block_fill = model::aggregate_fast(two_blocks, 20.0, 50.0, true);
    checks.check("fast visits blocks in reverse order and fills after second block",
        second_block_fill.evaluated_samples == 16
        && second_block_fill.minimum_score_fraction == 1.0
        && second_block_fill.health_score_fraction == 60.0 / 64.0
        && second_block_fill.special_fraction == 56.0 / 64.0
        && second_block_fill.mean_score == 76.25);
    const auto mixed_health = make_outcomes([](std::size_t sample_index) {
        return model::SampleOutcome{sample_index % 2 == 0 ? 40.0 : 60.0, true,
                                    sample_index % 2 == 0 ? 2 : 4};
    });
    const auto no_fill = model::aggregate_fast(mixed_health, 20.0, 50.0, true);
    checks.check("mixed health blocks prevent early-fill despite all minimum successes",
        no_fill.evaluated_samples == 64 && no_fill.minimum_score_fraction == 1.0
        && no_fill.health_score_fraction == 0.5 && no_fill.special_fraction == 0.5
        && no_fill.mean_score == 50.0);
    const auto varying_fill = make_outcomes([](std::size_t sample_index) {
        return sample_index < 56 ? model::SampleOutcome{0.0, false}
            : model::SampleOutcome{60.0 + static_cast<double>(sample_index - 56), true, 2};
    });
    checks.check("early-fill extrapolates the current block mean",
        model::aggregate_fast(varying_fill, 20.0, 50.0, true).mean_score == 63.5);
    for (const std::int64_t category : {2, 3, 258, 259, -254, -253}) {
        const model::SampleOutcomes outcomes(64, model::SampleOutcome{60.0, true, category});
        checks.check("special mask accepts category " + std::to_string(category),
            model::aggregate_full(outcomes, 20.0, 50.0, true).special_fraction == 1.0
            && model::aggregate_fast(outcomes, 20.0, 50.0, true).special_fraction == 1.0
            && model::zero_scale_aggregate(60.0, 50.0, category, true).special_fraction == 1.0);
    }
    for (const std::int64_t category : {0, 1, 4, 5, 255}) {
        const model::SampleOutcomes outcomes(64, model::SampleOutcome{60.0, true, category});
        checks.check("special mask excludes category " + std::to_string(category),
            model::aggregate_full(outcomes, 20.0, 50.0, true).special_fraction == 0.0
            && model::aggregate_fast(outcomes, 20.0, 50.0, true).special_fraction == 0.0
            && model::zero_scale_aggregate(60.0, 50.0, category, true).special_fraction == 0.0);
    }
    checks.check("zero-scale preserves central mean and special gate",
        shortcut.mean_score == 50.0 && shortcut.special_fraction == 1.0
        && model::zero_scale_aggregate(50.0, 50.0, 2, false).special_fraction == 0.0);
    const auto nonpositive_shortcut = model::zero_scale_aggregate(-2.0, 50.0, 3, true);
    checks.check("zero-scale does not borrow sample positivity validation",
        nonpositive_shortcut.minimum_score_fraction == 1.0
        && nonpositive_shortcut.health_score_fraction == 0.0
        && nonpositive_shortcut.special_fraction == 1.0
        && nonpositive_shortcut.mean_score == -2.0 && nonpositive_shortcut.evaluated_samples == 0);
    checks.check("zero-scale does not validate health threshold",
        model::zero_scale_aggregate(0.0, 0.0, 0, false).health_score_fraction == 1.0);

    const std::string count_error = "Exactly 64 already-evaluated sample outcomes are required";
    const std::string threshold_error = "This mathematical model covers positive finite thresholds only";
    const std::string score_error = "Valid sample scores must be positive and finite";
    for (const std::size_t count : {std::size_t{0}, std::size_t{63}, std::size_t{65}}) {
        checks.check("both aggregators reject sample count " + std::to_string(count),
            aggregators_reject(model::SampleOutcomes(count, model::SampleOutcome{50.0}),
                               20.0, 50.0, count_error));
    }
    const double infinity = std::numeric_limits<double>::infinity();
    const double not_a_number = std::numeric_limits<double>::quiet_NaN();
    const std::vector<std::pair<std::string, double>> invalid_thresholds{
        {"zero", 0.0}, {"negative", -1.0}, {"infinity", infinity},
        {"negative infinity", -infinity}, {"NaN", not_a_number}
    };
    for (const auto& [label, threshold] : invalid_thresholds) {
        checks.check("both aggregators reject " + label + " minimum threshold",
            aggregators_reject(uniform, threshold, 50.0, threshold_error));
        checks.check("both aggregators reject " + label + " health threshold",
            aggregators_reject(uniform, 20.0, threshold, threshold_error));
        const auto invalid_valid_score = make_outcomes([&](std::size_t sample_index) {
            return model::SampleOutcome{sample_index == 0 ? threshold : 50.0, true, 2};
        });
        checks.check("both aggregators reject " + label + " valid score before early-fill",
            aggregators_reject(invalid_valid_score, 20.0, 50.0, score_error));
    }
    for (const auto& [label, score] : std::vector<std::pair<std::string, double>>{
             {"infinity", infinity}, {"negative infinity", -infinity}, {"NaN", not_a_number}}) {
        const model::SampleOutcomes invalid_scores(64, model::SampleOutcome{score, false});
        checks.check("both aggregators reject " + label + " even when invalid",
            aggregators_reject(invalid_scores, 20.0, 50.0, score_error));
    }
    checks.check("validation checks sample count before thresholds",
        aggregators_reject({}, 0.0, 0.0, count_error));
    checks.check("validation checks thresholds before scores",
        aggregators_reject(model::SampleOutcomes(64, model::SampleOutcome{0.0}),
                           0.0, 0.0, threshold_error));
    checks.check("public validation helper rejects invalid input",
        rejects_with([&] { model::checked_outcomes({}, 20.0, 50.0); }, count_error));

    checks.check("sampling is deterministic", ring == model::ring_samples() && golden == model::golden_samples());
    const auto zero_ring = model::ring_samples(0.0);
    const auto zero_golden = model::golden_samples(0.0);
    const auto pair_is_zero = [](const auto& sample) {
        return sample.first == 0.0 && sample.second == 0.0;
    };
    checks.check("zero scale collapses both sampling tables",
        std::all_of(zero_ring.begin(), zero_ring.end(), pair_is_zero)
        && std::all_of(zero_golden.begin(), zero_golden.end(), pair_is_zero));
    const auto scaled_ring = model::ring_samples(2.5);
    const auto negative_golden = model::golden_samples(-2.0);
    bool ring_scaling_matches = true;
    bool golden_scaling_matches = true;
    bool ring_radii_match = true;
    bool golden_radii_match = true;
    for (std::size_t sample_index = 0; sample_index < 64; ++sample_index) {
        ring_scaling_matches = ring_scaling_matches
            && is_close(scaled_ring[sample_index].first, ring[sample_index].first * 2.5, 1e-14)
            && is_close(scaled_ring[sample_index].second, ring[sample_index].second * 2.5, 1e-14);
        golden_scaling_matches = golden_scaling_matches
            && is_close(negative_golden[sample_index].first, golden[sample_index].first * -2.0, 1e-14)
            && is_close(negative_golden[sample_index].second, golden[sample_index].second * -2.0, 1e-14);
        ring_radii_match = ring_radii_match
            && is_close(std::hypot(ring[sample_index].first, ring[sample_index].second),
                        static_cast<double>(sample_index / 8 + 1) / 8.0);
        golden_radii_match = golden_radii_match
            && is_close(std::hypot(golden[sample_index].first, golden[sample_index].second),
                        std::sqrt(static_cast<double>(sample_index + 1) / 64.0));
    }
    checks.check("ring scale multiplies every coordinate", ring_scaling_matches);
    checks.check("negative golden scale reflects every coordinate", golden_scaling_matches);
    checks.check("ring order groups eight sectors per radius", ring_radii_match);
    checks.check("golden order follows square-root radii", golden_radii_match);
    checks.check("ring angular coordinates match Python reference",
        is_close(ring[0].first, 0.11548494156391084)
        && is_close(ring[0].second, 0.04783542904563622)
        && is_close(ring[7].first, 0.11548494156391081)
        && is_close(ring[7].second, -0.0478354290456363)
        && is_close(ring[8].first, 0.1767766952966369)
        && is_close(ring[63].first, -0.7071067811865467)
        && is_close(ring[63].second, 0.7071067811865483));
    checks.check("golden angular coordinates match Python reference",
        golden[0].first == 0.125 && golden[0].second == 0.0
        && is_close(golden[1].first, -0.13034962282492385)
        && is_close(golden[1].second, 0.11941095355703384)
        && is_close(golden[8].first, 0.3522455779607675)
        && is_close(golden[63].first, 0.9205811259812875)
        && is_close(golden[63].second, 0.39055139288834306));

    checks.check("empty damage sequence preserves health", model::estimated_health(100.0, {}) == 100.0);
    checks.check("weights at or below cutoff are ignored",
        model::estimated_health(100.0, {{-1.0, 100.0}, {0.0, 100.0}, {0.75, 100.0}}) == 100.0);
    checks.check("health is not clamped after damage", model::estimated_health(10.0, {{1.0, 20.0}}) == -10.0);
    checks.check("negative damage remains mathematical input",
        model::estimated_health(100.0, {{1.0, -20.0}}) == 120.0);
    checks.check("exactly 100 remains ordinary minimum setting",
        model::minimum_damage_threshold(100.0, 500.0, true) == 100.0);
    checks.check("half-health flag does not affect ordinary setting",
        model::minimum_damage_threshold(90.0, 40.0, true) == 40.0);
    checks.check("half-health rounding also handles fractional health",
        model::minimum_damage_threshold(101.0, 40.5, true) == 21.0);
    checks.check("half-health result is capped at 130",
        model::minimum_damage_threshold(101.0, 501.0, true) == 130.0);
    checks.check("negative minimum setting is not clamped upward",
        model::minimum_damage_threshold(-5.0, 40.0, false) == -5.0);
    checks.check("zero minimum setting is preserved",
        model::minimum_damage_threshold(0.0, 40.0, false) == 0.0);
    const std::string health_error =
        "The original caller returns before candidate generation for nonpositive health";
    checks.check("zero health is rejected",
        rejects_with([] { model::minimum_damage_threshold(20.0, 0.0, false); }, health_error));
    checks.check("negative health is rejected",
        rejects_with([] { model::minimum_damage_threshold(101.0, -1.0, true); }, health_error));
    checks.check("attenuation uses distance divided by 500",
        is_close(model::distance_attenuation(0.81, 250.0), 0.9)
        && is_close(model::distance_attenuation(0.98, 1000.0), 0.98 * 0.98));
    checks.check("attenuation allows unity and growth parameters",
        model::distance_attenuation(1.0, 1000.0) == 1.0
        && model::distance_attenuation(4.0, 250.0) == 2.0);
    const std::string range_error =
        "This model covers positive range parameters and nonnegative distances only";
    checks.check("zero range is rejected",
        rejects_with([] { model::distance_attenuation(0.0, 0.0); }, range_error));
    checks.check("negative range is rejected",
        rejects_with([] { model::distance_attenuation(-1.0, 500.0); }, range_error));
    checks.check("negative distance is rejected",
        rejects_with([] { model::distance_attenuation(0.98, -1.0); }, range_error));

    checks.check("empty history retains zero entries",
        model::retained_count_after_tail_aging({}, 100) == 0);
    checks.check("aging equality is retained",
        model::retained_count_after_tail_aging({100, 95, 90}, 95) == 2);
    checks.check("aging keeps all entries when oldest is fresh",
        model::retained_count_after_tail_aging({100, 95, 90}, 90) == 3);
    checks.check("single stale entry is preserved",
        model::retained_count_after_tail_aging({1}, 2) == 1);
    checks.check("aging does not sort or validate supplied order",
        model::retained_count_after_tail_aging({10, 100, 20}, 50) == 2);
    checks.check("nonpositive delta clamps to one",
        !model::resets_history(0, 4096.0, 8, 8)
        && model::resets_history(-100, 4097.0, 8, 8));
    checks.check("intermediate delta scales squared threshold linearly",
        !model::resets_history(3, 12288.0, 8, 8)
        && model::resets_history(3, 12289.0, 8, 8));
    checks.check("int64 delta endpoints clamp without overflow",
        !model::resets_history(std::numeric_limits<std::int64_t>::min(), 4096.0, 8, 8)
        && !model::resets_history(std::numeric_limits<std::int64_t>::max(), 20480.0, 8, 8));
    checks.check("equal collection and zero distance retain history", !model::resets_history(1, 0.0, 8, 8));
    checks.check("collection mismatch resets independently of distance",
        model::resets_history(5, -10.0, 7, 8));
    checks.check("negative squared distance is not additionally validated",
        !model::resets_history(1, -10.0, 8, 8));
    return checks.report();
}

}

int main() {
    try {
        return run_checks();
    } catch (const std::exception& error) {
        std::cerr << "Unexpected exception: " << error.what() << '\n';
        return 2;
    }
}
```
