#include <core/features/combat/research_hitchance.hpp>
#include <cassert>
#include <limits>
#include <thread>
#include <vector>

namespace hc = features::combat::research_hitchance;
namespace {
    bool close(float a, float b) { return std::fabs(a - b) < 1e-5f; }
    struct vec { float x{}, y{}, z{}; };
}

int main()
{
    const hc::thresholds t{10, 20, true};
    hc::outcomes values{};
    values.fill({20, true, 3});
    const auto full = hc::aggregate_full(values, t);
    const auto fast = hc::aggregate_fast(values, t);
    assert(full && fast);
    assert(full->minimum_fraction == 1 && full->health_fraction == 1);
    assert(full->special_fraction == 1 && full->mean_score == 20 && full->evaluated == 64);
    assert(fast->minimum_fraction == 1 && fast->health_fraction == 0);
    assert(fast->special_fraction == 1 && fast->mean_score == 20 && fast->evaluated == 8);

    values.fill({7, false, 2});
    const auto misses = hc::aggregate_full(values, t);
    assert(misses && misses->minimum_fraction == 0 && misses->health_fraction == 0);
    assert(misses->special_fraction == 0 && misses->mean_score == 0);
    assert(hc::aggregate_fast(values, t)->evaluated == 64);

    // Reverse traversal: a mixed first block must be preserved when block 6 fills.
    values.fill({25, true, 2});
    for (std::size_t i = 56; i < 64; ++i) values[i] = {0, false, 0};
    values[56] = {20, true, 2};
    const auto partial = hc::aggregate_fast(values, t);
    assert(partial && partial->evaluated == 16);
    assert(partial->minimum_fraction == 57.0f / 64.0f);
    assert(partial->health_fraction == 56.0f / 64.0f);
    assert(partial->special_fraction == 57.0f / 64.0f);
    assert(partial->mean_score == 1420.0f / 64.0f);

    // Mixed categories in a triggering block do NOT extrapolate special counts.
    values.fill({30, true, 0});
    values[56].category = 3;
    const auto mixed = hc::aggregate_fast(values, t);
    assert(mixed && mixed->evaluated == 8 && mixed->special_fraction == 1.0f / 64.0f);
    assert(hc::aggregate_fast(values, {10, 20, false})->special_fraction == 0);

    // Every block has mixed health comparisons: no shortcut, equality differs.
    for (std::size_t i = 0; i < 64; ++i) values[i] = {i % 2 ? 21.0f : 20.0f, true, 2};
    assert(hc::aggregate_full(values, t)->health_fraction == 1);
    const auto all = hc::aggregate_fast(values, t);
    assert(all && all->evaluated == 64 && all->health_fraction == 0.5f);
    assert(all->mean_score == 20.5f);

    // Reject malformed unvisited slots before array-based early-fill.
    values.fill({30, true, 2});
    values[0].score = std::numeric_limits<float>::quiet_NaN();
    assert(!hc::aggregate_fast(values, t) && !hc::aggregate_full(values, t));
    values[0] = {0, true, 2};
    assert(!hc::aggregate_fast(values, t));
    assert(!hc::aggregate_fast(values, {0, 20, true}));
    std::size_t calls = 0;
    const auto lazy = hc::evaluate_fast(t, [&](std::size_t index) {
        assert(index == 56 + calls++);
        return hc::outcome{30, true, 2};
    });
    assert(lazy && calls == 8 && lazy->evaluated == 8);

    const auto zero = hc::zero_scale_aggregate(20, 20, 3, true);
    assert(zero.minimum_fraction == 1 && zero.health_fraction == 1);
    assert(zero.special_fraction == 1 && zero.mean_score == 20 && zero.evaluated == 0);

    const auto table = hc::ring_samples(2);
    assert(table);
    for (std::size_t ring = 1; ring <= 8; ++ring)
        for (std::size_t sector = 0; sector < 8; ++sector)
        {
            const auto s = (*table)[(ring - 1) * 8 + sector];
            assert(close(std::hypot(s.x, s.y), static_cast<float>(ring) / 4.0f));
        }
    assert(close((*table)[0].x, 0.25f * std::cos(std::numbers::pi_v<float> / 8)));
    assert(!hc::ring_samples(-1));
    const auto zero_table = hc::ring_samples(0);
    assert(zero_table);
    for (const auto s : *zero_table) assert(s.x == 0 && s.y == 0);

    const hc::frame<vec> frame{{1, 2, 3}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 100};
    const auto end = hc::endpoint(frame, {0, 0});
    assert(end && end->x == 101 && end->y == 2 && end->z == 3);
    const auto diagonal = hc::endpoint(frame, {1, 1});
    assert(diagonal && close(diagonal->x, 1 + 100 / std::sqrt(3.0f)));
    const hc::frame<vec> degenerate{{}, {}, {}, {}, 100};
    assert(!hc::endpoint(degenerate, {0, 0}));

    hc::worker_job job;
    std::array<std::atomic<unsigned>, 64> seen{};
    std::vector<std::thread> workers;
    for (int i = 0; i < 8; ++i)
        workers.emplace_back([&] {
            job.worker([&](std::size_t index) {
                seen[index].fetch_add(1, std::memory_order_relaxed);
                return hc::outcome{static_cast<float>(index + 1), true, 2};
            });
        });
    for (auto& worker : workers) worker.join();
    for (std::size_t i = 0; i < 64; ++i) {
        assert(seen[i].load() == 1);
        assert(job.values[i].score == static_cast<float>(i + 1));
    }
    job.worker([](std::size_t) -> hc::outcome { assert(false); return {}; });
    assert(hc::aggregate_full(job.values, t)->evaluated == 64);

    assert(*hc::distance_attenuation(0.5f, 500) == 0.5f);
    assert(*hc::distance_attenuation(2, 500) == 2);
    assert(*hc::distance_attenuation(0.5f, 0) == 1);
    assert(!hc::distance_attenuation(0, 500));
    assert(!hc::distance_attenuation(0.5f, -1));
}
