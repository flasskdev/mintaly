#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>

namespace threadpool::detail {
    // Each claimed range has exactly one owner. Wide arithmetic also supports
    // INT_MIN..INT_MAX and invalid/non-positive grain sizes without overflow.
    class work_queue
    {
    public:
        struct chunk { int begin; int end; };

        work_queue(int begin, int end, int grain) noexcept
            : m_next(begin), m_end(end), m_grain(std::max(grain, 1)) {}

        [[nodiscard]] bool claim(chunk& out) noexcept
        {
            auto begin = m_next.load(std::memory_order_relaxed);
            while (begin < m_end)
            {
                const auto end = std::min(begin + m_grain, m_end);
                if (m_next.compare_exchange_weak(begin, end, std::memory_order_relaxed))
                {
                    out = {static_cast<int>(begin), static_cast<int>(end)};
                    return true;
                }
            }
            return false;
        }

    private:
        std::atomic<std::int64_t> m_next;
        const std::int64_t m_end;
        const std::int64_t m_grain;
    };
}
