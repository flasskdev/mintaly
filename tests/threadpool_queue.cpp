#include <utilities/threadpool/work_queue.hpp>
#include <utilities/performance.hpp>
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <limits>
#include <thread>
#include <vector>

namespace {
    using queue = threadpool::detail::work_queue;

    void sequential_ranges(int begin, int end, int grain)
    {
        queue work(begin, end, grain);
        queue::chunk chunk{};
        std::int64_t cursor = begin;
        std::int64_t count{};
        while (work.claim(chunk))
        {
            assert(chunk.begin == cursor);
            assert(chunk.end > chunk.begin && chunk.end <= end);
            const auto length = static_cast<std::int64_t>(chunk.end) - chunk.begin;
            assert(length == std::min<std::int64_t>(std::max(grain, 1), static_cast<std::int64_t>(end) - cursor));
            count += length;
            cursor = chunk.end;
        }
        assert(count == std::max<std::int64_t>(static_cast<std::int64_t>(end) - begin, 0));
        assert(!work.claim(chunk));
        if (begin < end) assert(cursor == end);
    }

    void concurrent_claims(int size, int grain, int participants)
    {
        constexpr int offset = -31;
        queue work(offset, offset + size, grain);
        std::vector<std::atomic<int>> visits(static_cast<std::size_t>(size));
        std::vector<std::uint64_t> results(static_cast<std::size_t>(size));
        for (auto& value : visits) value.store(0);
        std::atomic<bool> start{false};
        std::vector<std::thread> workers;
        for (int worker = 0; worker < participants; ++worker)
        {
            workers.emplace_back([&]
            {
                start.wait(false);
                queue::chunk chunk{};
                while (work.claim(chunk))
                {
                    for (int index = chunk.begin; index < chunk.end; ++index)
                    {
                        const auto slot = static_cast<std::size_t>(index - offset);
                        assert(visits[slot].fetch_add(1) == 0);
                        // Uneven task durations must not drop or duplicate work.
                        if (slot % 7 == 0) std::this_thread::yield();
                        std::uint64_t value = slot;
                        for (std::size_t round = 0; round < slot % 13; ++round)
                            value = value * 1664525u + 1013904223u;
                        results[slot] = value;
                    }
                }
            });
        }
        start.store(true);
        start.notify_all();
        for (auto& worker : workers) worker.join();
        for (std::size_t slot = 0; slot < results.size(); ++slot)
        {
            assert(visits[slot].load() == 1);
            std::uint64_t expected = slot;
            for (std::size_t round = 0; round < slot % 13; ++round)
                expected = expected * 1664525u + 1013904223u;
            assert(results[slot] == expected);
        }
    }

    void profiler()
    {
        namespace perf = utilities::performance;
        (void)perf::take_samples();
        std::vector<std::thread> workers;
        for (int i = 0; i < 8; ++i)
            workers.emplace_back([]
            {
                for (int j = 0; j < 1000; ++j)
                    perf::record(perf::stage::scan_batch, static_cast<std::uint64_t>(j));
            });
        for (auto& worker : workers) worker.join();
        const auto samples = perf::take_samples();
        const auto& scan = samples[static_cast<std::size_t>(perf::stage::scan_batch)];
        assert(scan.calls == 8000 && scan.total_ns == 8u * 999u * 1000u / 2u && scan.maximum_ns == 999);
        for (const auto& value : perf::take_samples()) assert(value.calls == 0);
        {
            perf::scope timer{perf::stage::gather};
            timer.finish();
            timer.finish();
        }
        assert(perf::take_samples()[static_cast<std::size_t>(perf::stage::gather)].calls == 1);
    }
}

int main()
{
    for (int begin : {-31, 0, 17})
        for (int size : {-1, 0, 1, 3, 19, 127})
            for (int grain : {-1, 0, 1, 2, 7, 128})
                sequential_ranges(begin, begin + size, grain);
    const auto low = std::numeric_limits<int>::min();
    const auto high = std::numeric_limits<int>::max();
    sequential_ranges(low, high, high);
    sequential_ranges(low, low + 1, 0);
    sequential_ranges(high - 1, high, 1);
    for (int participants : {1, 2, 8, 32})
        for (int size : {0, 1, 3, 19, 4097})
            for (int grain : {0, 1, 7, 1024})
                concurrent_claims(size, grain, participants);
    // Independent batches must not share their cursors or result storage.
    std::thread first([] { concurrent_claims(8193, 1, 8); });
    std::thread second([] { concurrent_claims(4099, 3, 8); });
    first.join();
    second.join();
    profiler();
}
