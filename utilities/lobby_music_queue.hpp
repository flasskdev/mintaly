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
    struct playback_source {
        std::uintptr_t context{};
        std::uint64_t thread{};
        std::uint16_t original_kit{};
        float volume{};
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
        // Capture only an actual lobby-track call, before replacing its kit.
        // Never infer an audio context from another manager or another track.
        void observe(playback_source source) {
            std::lock_guard lock(m_mutex);
            if (!source.context || !source.thread) return;
            m_source = source;
        }
        [[nodiscard]] std::optional<playback_source> source_for(std::uint64_t thread) const {
            std::lock_guard lock(m_mutex);
            if (!m_source || !thread || m_source->thread != thread) return std::nullopt;
            return m_source;
        }
        void reset() {
            std::lock_guard lock(m_mutex);
            ++m_generation;
            m_pending.reset();
            m_applied.reset();
            m_source.reset();
        }
    private:
        mutable std::mutex m_mutex;
        std::optional<request> m_pending;
        std::optional<selection> m_applied;
        std::optional<playback_source> m_source;
        std::uint64_t m_generation{};
    };
}
