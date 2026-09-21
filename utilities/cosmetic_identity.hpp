#pragma once
#include <cstdint>

namespace cosmetic_cache {
struct identity {
    std::uintptr_t weapon{};
    std::uintptr_t scene{};
    std::uintptr_t model{};
    std::uint32_t owner{};
    bool operator==(const identity&) const = default;
    constexpr bool ready() const { return weapon != 0 && scene != 0 && model != 0; }
};
inline constexpr bool reusable(const identity& saved, const identity& current) {
    return saved.ready() && current.ready() && saved == current;
}
inline constexpr bool hud_reusable(const identity& saved, const identity& current, bool required) {
    return required ? reusable(saved, current) : saved == current;
}
}
