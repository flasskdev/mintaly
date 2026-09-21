#pragma once

#include <array>
#include <optional>
#include <string_view>

namespace features::changer::name_tag {
    using buffer = std::array<char, 161>;
    struct snapshot { buffer name{}, override_name{}; };

    inline std::optional<std::array<int, 2>> offsets() {
        return std::nullopt;
    }

    inline std::optional<snapshot> capture(std::uintptr_t /*item*/) {
        return std::nullopt;
    }

    inline bool restore(std::uintptr_t /*item*/, const snapshot& /*saved*/) {
        return true;
    }

    inline snapshot desired(std::string_view /*text*/) {
        return snapshot{};
    }

    inline bool matches(std::uintptr_t /*item*/, std::string_view /*text*/) {
        return true;
    }

    inline bool apply(std::uintptr_t /*item*/, std::string_view /*text*/) {
        return true;
    }
}

