#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <mutex>

namespace utilities::performance {
    enum class stage : std::size_t
    {
        rage_total, gather, prepare_scan, scan_batch, selection, hitchance_batch,
        pool_dispatch_wait, pool_join_wait, count
    };

    inline constexpr std::array names{
        "rage_total", "gather", "prepare_scan", "scan_batch", "selection",
        "hitchance_batch", "pool_dispatch_wait", "pool_join_wait"
    };
    static_assert(names.size() == static_cast<std::size_t>(stage::count));

    struct sample
    {
        std::uint64_t calls{};
        std::uint64_t total_ns{};
        std::uint64_t maximum_ns{};
    };

    inline std::mutex mutex;
    inline std::array<sample, names.size()> counters{};

    inline void record(stage id, std::uint64_t ns)
    {
        std::lock_guard lock(mutex);
        auto& value = counters[static_cast<std::size_t>(id)];
        ++value.calls;
        value.total_ns += ns;
        value.maximum_ns = std::max(value.maximum_ns, ns);
    }

    [[nodiscard]] inline auto take_samples()
    {
        std::lock_guard lock(mutex);
        const auto out = counters;
        counters = {};
        return out;
    }

    class scope
    {
    public:
        explicit scope(stage id) : m_id(id), m_start(std::chrono::steady_clock::now()) {}
        scope(const scope&) = delete;
        scope& operator=(const scope&) = delete;
        ~scope() { finish(); }

        void finish()
        {
            if (m_finished) return;
            m_finished = true;
            const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now() - m_start).count();
            record(m_id, static_cast<std::uint64_t>(std::max<std::int64_t>(ns, 0)));
        }

    private:
        stage m_id;
        std::chrono::steady_clock::time_point m_start;
        bool m_finished{};
    };
}
