#include <utilities/indexed_cache.hpp>
#include <utilities/threadpool/partition.hpp>
#include <core/features/combat/ballistics.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

namespace {
    struct entry
    {
        int index{};
        int type{};
        int payload{};
        bool operator==(const entry&) const = default;
    };

    using cache_type = utilities::indexed_cache<entry, 64, 4>;

    void verify_cache(cache_type& cache, const std::vector<entry>& reference)
    {
        assert(cache.entries() == reference);
        assert(cache.empty() == reference.empty());
        for (int id = -1; id <= 65; ++id)
        {
            const auto found = std::find_if(reference.begin(), reference.end(),
                [=](const entry& value) { return value.index == id; });
            const auto actual = cache.find(static_cast<std::size_t>(id));
            assert((actual != nullptr) == (found != reference.end()));
            if (actual) assert(*actual == *found);
        }
        for (int type = 0; type < 4; ++type)
        {
            std::vector<entry> expected;
            for (const auto& value : reference)
                if (value.type == type) expected.push_back(value);
            const auto& snapshot = cache.snapshot(type);
            assert(snapshot == expected);
            assert(cache.snapshot_if_ready(type) == &snapshot);
            assert(&cache.snapshot(type) == &snapshot);
        }
        assert(cache.snapshot(4).empty());
        assert(cache.snapshot(std::numeric_limits<std::size_t>::max()).empty());
    }

    void entity_cache()
    {
        cache_type cache;
        std::vector<entry> reference;
        cache.reserve(64);
        verify_cache(cache, reference);
        assert(!cache.insert({-1, 1, 1}));
        assert(!cache.insert({64, 1, 1}));
        assert(cache.insert({0, 1, 11}));
        reference.push_back({0, 1, 11});
        verify_cache(cache, reference);
        const auto old_snapshot = cache.snapshot(1);
        assert(!cache.insert({0, 2, 22}));
        assert(cache.snapshot_if_ready(1)); // A rejected insertion changes nothing.
        assert(cache.insert({63, 2, 33}));
        reference.push_back({63, 2, 33});
        assert(!cache.snapshot_if_ready(1));
        assert(old_snapshot == std::vector<entry>({{0, 1, 11}}));
        verify_cache(cache, reference);
        assert(cache.erase(0)); // Last entry moves across types into slot zero.
        reference[0] = reference.back();
        reference.pop_back();
        assert(!cache.snapshot_if_ready(2));
        verify_cache(cache, reference);
        assert(cache.insert({0, 3, 44})); // Reuse the removed ID.
        reference.push_back({0, 3, 44});
        verify_cache(cache, reference);

        // Differential test against the original linear lookup/swap removal.
        std::uint32_t state = 0x12345678u;
        for (int step = 0; step < 4000; ++step)
        {
            state = state * 1664525u + 1013904223u;
            const auto id = static_cast<int>((state >> 16) % 70u) - 3;
            auto found = std::find_if(reference.begin(), reference.end(),
                [=](const entry& value) { return value.index == id; });
            if (step % 97 == 0)
            {
                cache.clear();
                reference.clear();
            }
            else if ((state & 1u) != 0)
            {
                const entry value{id, static_cast<int>((state >> 8) % 4u), step};
                const bool inserted = id >= 0 && id < 64 && found == reference.end();
                assert(cache.insert(value) == inserted);
                if (inserted) reference.push_back(value);
            }
            else
            {
                const bool removed = found != reference.end();
                assert(cache.erase(static_cast<std::size_t>(id)) == removed);
                if (removed)
                {
                    *found = reference.back();
                    reference.pop_back();
                }
            }
            verify_cache(cache, reference);
        }
        cache.clear();
        verify_cache(cache, {});
        utilities::indexed_cache<entry, 0, 0> empty;
        assert(!empty.insert({0, 0, 0}) && !empty.find(0) && !empty.erase(0));
        assert(empty.snapshot(0).empty());
    }

    void verify_partition(int begin, int end, int minimum, int requested)
    {
        const auto result = threadpool::detail::partition_range(begin, end, minimum, requested);
        if (begin >= end)
        {
            assert(result.count == 0);
            return;
        }
        assert(result.count >= 1 && result.count <= 4);
        const auto total = static_cast<std::int64_t>(end) - begin;
        const auto count = std::clamp(requested, 1, 4);
        const auto expected_size = std::max((total + count - 1) / count,
            static_cast<std::int64_t>(std::max(minimum, 1)));
        auto cursor = begin;
        std::int64_t covered = 0;
        for (int i = 0; i < result.count; ++i)
        {
            const auto chunk = result.chunks[i];
            assert(chunk.begin == cursor && chunk.end > chunk.begin && chunk.end <= end);
            const auto size = static_cast<std::int64_t>(chunk.end) - chunk.begin;
            assert(size == std::min(expected_size, static_cast<std::int64_t>(end) - cursor));
            covered += size;
            cursor = chunk.end;
        }
        assert(cursor == end && covered == total);
    }

    void partitions()
    {
        for (int begin = -20; begin <= 20; ++begin)
            for (int size = -1; size <= 45; ++size)
                for (int minimum = -2; minimum <= 8; ++minimum)
                    for (int requested = 0; requested <= 6; ++requested)
                        verify_partition(begin, begin + size, minimum, requested);
        const auto low = std::numeric_limits<int>::min();
        const auto high = std::numeric_limits<int>::max();
        for (int minimum : {1, 2, high})
            for (int requested : {1, 2, 3, 4})
            {
                verify_partition(low, high, minimum, requested);
                verify_partition(low, low + 1, minimum, requested);
                verify_partition(high - 1, high, minimum, requested);
            }
    }

    void timing_windows()
    {
        using features::combat::ballistics::lagcomp_cutoff;
        for (float limit : {0.0f, 0.1f, 0.2f, 1.0f})
            for (float time : {0.0f, 1.0f, 1000.0f})
                for (float latency : {-1.0f, 0.0f, 0.05f, 0.2f, 1.0f})
                {
                    const auto budget = limit - std::max(latency, 0.0f);
                    const auto cutoff = lagcomp_cutoff(limit, time, latency);
                    assert(cutoff.has_value() == (budget > 0.0f));
                    if (cutoff)
                    {
                        assert(*cutoff == time - budget);
                        assert(std::nextafter(*cutoff, -std::numeric_limits<float>::infinity()) < *cutoff);
                    }
                }
        const auto nan = std::numeric_limits<float>::quiet_NaN();
        const auto inf = std::numeric_limits<float>::infinity();
        assert(!lagcomp_cutoff(nan, 1, 0));
        assert(!lagcomp_cutoff(1, inf, 0));
        assert(!lagcomp_cutoff(1, 1, nan));
        assert(!lagcomp_cutoff(1, 1, inf));
        assert(!lagcomp_cutoff(-1, 1, 0));
        const auto max = std::numeric_limits<float>::max();
        assert(!lagcomp_cutoff(max, -max, 0));
    }
}

int main()
{
    entity_cache();
    partitions();
    timing_windows();
}
