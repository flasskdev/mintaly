#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace cosmetic_paint {
    struct attribute_state {
        float value{};
        bool present{};
    };
    using snapshot = std::array<attribute_state, 3>;
    using values = std::array<float, 3>;

    template <typename Snapshot>
    [[nodiscard]] bool matches(const Snapshot& actual, const values& expected) {
        for (std::size_t slot = 0; slot < expected.size(); ++slot) {
            if (!actual[slot].present ||
                std::bit_cast<std::uint32_t>(actual[slot].value) !=
                std::bit_cast<std::uint32_t>(expected[slot])) return false;
        }
        return true;
    }

    // The setter owns allocation and notifications. Never retain an engine vector
    // pointer across calls: inserting an absent attribute can reallocate it.
    template <typename Snapshot = snapshot, typename Read, typename Set>
    [[nodiscard]] bool ensure(const values& expected, Read read, Set set) {
        Snapshot before{};
        if (!read(before)) return false;
        if (matches(before, expected)) return true;
        for (std::size_t slot = 0; slot < expected.size(); ++slot) {
            if (!before[slot].present ||
                std::bit_cast<std::uint32_t>(before[slot].value) !=
                std::bit_cast<std::uint32_t>(expected[slot])) {
                set(slot, expected[slot]);
            }
        }
        Snapshot after{};
        return read(after) && matches(after, expected);
    }
}
