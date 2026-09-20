#pragma once
#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <utility>

namespace lobby_music {
    struct selection {
        std::uint16_t kit{};
        std::string name{};
        bool operator==(const selection&) const = default;
    };
    struct request {
        selection value;
        std::uint64_t generation{};
    };

    // Settings are copied on the menu thread. Engine dispatch is game-thread only.
    class queue {
    public:
        void submit(selection value) {
            std::lock_guard lock(m_mutex);
            if (m_pending && m_pending->value == value) return;
            if (!m_pending && m_applied && *m_applied == value) return;
            m_pending = request{std::move(value), ++m_generation};
        }
        [[nodiscard]] std::optional<request> next() const {
            std::lock_guard lock(m_mutex);
            return m_pending;
        }
        void acknowledge(const request& done) {
            std::lock_guard lock(m_mutex);
            if (!m_pending || m_pending->generation != done.generation) return;
            m_applied = done.value;
            m_pending.reset();
        }
        void reset() {
            std::lock_guard lock(m_mutex);
            ++m_generation;
            m_pending.reset();
            m_applied.reset();
        }
    private:
        mutable std::mutex m_mutex;
        std::optional<request> m_pending;
        std::optional<selection> m_applied;
        std::uint64_t m_generation{};
    };
}
