#pragma once
#include <cstdint>
#include <optional>

namespace hud_binding {
    // Only explicit entity links count as evidence. A model name, child order,
    // or a shared pawn owner cannot identify a particular physical weapon.
    constexpr bool matches(std::uint32_t active, std::uintptr_t candidate,
        std::optional<std::uint32_t> reverse, std::uint32_t owner,
        std::uintptr_t forward) {
        if (!active || active == 0xffffffffu || !candidate) return false;
        if (reverse && *reverse != active) return false;
        if (forward && forward != candidate) return false;
        return (reverse && *reverse == active) || owner == active || forward == candidate;
    }
}
