#pragma once

#include <array>
#include <algorithm>
#include <optional>
#include <string_view>
#include <core/systems/systems.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/skin_options.hpp>

namespace features::changer::name_tag {
    using buffer = std::array<char, 161>;
    struct snapshot { buffer name{}, override_name{}; };

    inline std::optional<std::array<int, 2>> offsets() {
        const auto name = SCHEMA("C_EconItemView", "m_szCustomName"_hash);
        const auto override_name = SCHEMA("C_EconItemView", "m_szCustomNameOverride"_hash);
        // Refuse unknown layouts rather than writing through a missing schema offset.
        if (!name || override_name <= name || override_name - name != sizeof(buffer)) return std::nullopt;
        return std::array<int, 2>{name, override_name};
    }

    inline std::optional<snapshot> capture(std::uintptr_t item) {
        const auto layout = offsets();
        if (!item || !layout) return std::nullopt;
        const auto name = memory::safe_read<buffer>(item + (*layout)[0]);
        const auto override_name = memory::safe_read<buffer>(item + (*layout)[1]);
        if (!name || !override_name) return std::nullopt;
        return snapshot{*name, *override_name};
    }

    inline bool restore(std::uintptr_t item, const snapshot& saved) {
        const auto layout = offsets();
        return item && layout && memory::safe_write(item + (*layout)[0], saved.name)
            && memory::safe_write(item + (*layout)[1], saved.override_name);
    }

    inline snapshot desired(std::string_view text) {
        snapshot result{};
        const auto normalized = skin_options::normalize_name_tag(text);
        std::copy(normalized.begin(), normalized.end(), result.name.begin());
        result.override_name = result.name;
        return result;
    }

    inline bool matches(std::uintptr_t item, std::string_view text) {
        const auto current = capture(item);
        if (!current) return text.empty();
        const auto target = desired(text);
        return current->name == target.name && current->override_name == target.override_name;
    }

    inline bool apply(std::uintptr_t item, std::string_view text) {
        if (!offsets()) return text.empty();
        return restore(item, desired(text));
    }
}
