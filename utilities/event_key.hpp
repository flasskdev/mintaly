#pragma once
#include <cstddef>
#include <cstdint>

namespace event_key {
    // Source 2 event keys use MurmurHash2 with the string-token seed.
    [[nodiscard]] constexpr std::uint32_t hash(const char* text) noexcept {
        if (!text) return 0;
        std::size_t length = 0;
        while (text[length]) ++length;
        constexpr std::uint32_t m = 0x5bd1e995u;
        std::uint32_t h = 0x31415926u ^ static_cast<std::uint32_t>(length);
        std::size_t i = 0;
        const auto byte = [&](std::size_t at) { return static_cast<std::uint32_t>(static_cast<unsigned char>(text[at])); };
        while (length >= 4) {
            std::uint32_t k = byte(i) | (byte(i + 1) << 8) | (byte(i + 2) << 16) | (byte(i + 3) << 24);
            k *= m;
            k ^= k >> 24;
            k *= m;
            h = (h * m) ^ k;
            i += 4;
            length -= 4;
        }
        if (length == 3) h ^= byte(i + 2) << 16;
        if (length >= 2) h ^= byte(i + 1) << 8;
        if (length >= 1) { h ^= byte(i); h *= m; }
        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;
        return h;
    }

    // Controller identity only. A watched pawn must never become the attacker.
    [[nodiscard]] constexpr bool local_attacker(std::uintptr_t actual_local,
        std::uintptr_t snapshot_local, std::uintptr_t attacker, std::uintptr_t victim) noexcept {
        return actual_local != 0 && snapshot_local == actual_local &&
            attacker == actual_local && victim != 0 && victim != actual_local;
    }
}
