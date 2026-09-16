#pragma once

#include <algorithm>
#include <array>
#include <cstdint>

namespace threadpool::detail {

    inline constexpr int max_partitions = 4;

    struct work_chunk
    {
        int begin{};
        int end{};
    };

    struct work_partition
    {
        std::array<work_chunk, max_partitions> chunks{};
        int count{};
    };

    // Preserve the ceil-division/minimum-chunk policy, including a short tail.
    // Wide arithmetic handles the full signed-int interval without overflow.
    [[nodiscard]] inline work_partition partition_range(int begin, int end,
        int minimum_chunk, int requested_chunks)
    {
        work_partition out{};
        if (begin >= end)
            return out;
        const auto total = static_cast<std::int64_t>(end) - begin;
        const auto count = std::clamp(requested_chunks, 1, max_partitions);
        const auto size = std::max((total + count - 1) / count,
            static_cast<std::int64_t>(std::max(minimum_chunk, 1)));
        for (auto cursor = static_cast<std::int64_t>(begin); cursor < end; cursor += size)
        {
            const auto next = std::min(cursor + size, static_cast<std::int64_t>(end));
            out.chunks[out.count++] = {static_cast<int>(cursor), static_cast<int>(next)};
        }
        return out;
    }
}
