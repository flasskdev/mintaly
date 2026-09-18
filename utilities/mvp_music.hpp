#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>
#include <utility>

namespace mvp_music {
    [[nodiscard]] constexpr bool valid_kit(int kit) {
        return kit > 0 && kit < 0xffff;
    }

    // These are the track IDs already used by the game's playback adapter.
    inline constexpr int anthem_track = 11;
    inline constexpr int lobby_track = 1;

    class resolver {
    public:
        using clock = std::chrono::steady_clock;
        using callback = std::function<void(std::uint16_t)>;
        struct playback {
            callback play;
            std::uint16_t kit{};
        };

        void clear() {
            m_pending.reset();
            m_winner.reset();
        }

        // Zero means the event resolved without a usable override: keep engine audio.
        void set_winner(int kit, clock::time_point now) {
            m_winner = valid_kit(kit) ? kit : 0;
            m_winner_time = now;
        }

        [[nodiscard]] int winner_kit(clock::time_point now) const {
            return ready(now) ? *m_winner : 0;
        }

        [[nodiscard]] std::optional<playback> submit(std::uint16_t engine_kit, callback play, clock::time_point now) {
            if (ready(now)) return playback{std::move(play), select(engine_kit)};
            // Bound storage without silently losing a second engine request.
            std::optional<playback> displaced;
            if (m_pending) displaced = playback{std::move(m_pending->play), m_pending->kit};
            m_pending = pending{std::move(play), engine_kit, now};
            return displaced;
        }

        [[nodiscard]] std::optional<playback> poll(clock::time_point now) {
            if (!m_pending) return std::nullopt;
            // Never replay an old request after a long pause / stalled frame loop.
            if (now - m_pending->time > std::chrono::seconds(1)) {
                m_pending.reset();
                return std::nullopt;
            }
            if (!ready(now) && now - m_pending->time < std::chrono::milliseconds(100))
                return std::nullopt;
            playback result{std::move(m_pending->play),
                ready(now) ? select(m_pending->kit) : m_pending->kit};
            m_pending.reset();
            return result;
        }

    private:
        struct pending {
            callback play;
            std::uint16_t kit{};
            clock::time_point time{};
        };
        [[nodiscard]] bool ready(clock::time_point now) const {
            return m_winner.has_value() && now - m_winner_time <= std::chrono::seconds(25);
        }
        [[nodiscard]] std::uint16_t select(std::uint16_t engine_kit) const {
            return m_winner && valid_kit(*m_winner) ? static_cast<std::uint16_t>(*m_winner) : engine_kit;
        }
        std::optional<int> m_winner;
        clock::time_point m_winner_time{};
        std::optional<pending> m_pending;
    };
}
