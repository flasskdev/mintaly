#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <optional>
#include <string_view>
#include <core/systems/systems.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/skin_options.hpp>

namespace features::changer::name_tag {
    using buffer = std::array<char, 161>;
    struct snapshot { buffer name{}, override_name{}; };

    inline std::optional<std::array<int, 2>> offsets() {
        const auto custom_name = SCHEMA("C_EconItemView", "m_szCustomName"_hash);
        const auto override_name = SCHEMA("C_EconItemView", "m_szCustomNameOverride"_hash);
        if (!custom_name) return std::nullopt;
        return std::array<int, 2>{ custom_name, override_name ? override_name : custom_name };
    }

    inline std::optional<snapshot> capture(std::uintptr_t item) {
        const auto off = offsets();
        if (!off || !item) return std::nullopt;
        const auto name = memory::safe_read<buffer>(item + (*off)[0]);
        if (!name) return std::nullopt;
        snapshot s{};
        s.name = *name;
        if ((*off)[1] && (*off)[1] != (*off)[0]) {
            const auto override_name = memory::safe_read<buffer>(item + (*off)[1]);
            if (override_name) s.override_name = *override_name;
        } else {
            s.override_name = *name;
        }
        return s;
    }

    inline bool restore(std::uintptr_t item, const snapshot& saved) {
        const auto off = offsets();
        if (!off || !item) return false;
        bool ok = memory::safe_write(item + (*off)[0], saved.name);
        if ((*off)[1] && (*off)[1] != (*off)[0]) {
            ok = memory::safe_write(item + (*off)[1], saved.override_name) && ok;
        }
        return ok;
    }

    inline snapshot desired(std::string_view text) {
        snapshot s{};
        const auto normalized = skin_options::normalize_name_tag(text);
        if (!normalized.empty()) {
            const auto len = std::min(normalized.size(), s.name.size() - 1);
            std::memcpy(s.name.data(), normalized.data(), len);
            s.name[len] = '\0';
            std::memcpy(s.override_name.data(), normalized.data(), len);
            s.override_name[len] = '\0';
        }
        return s;
    }

    inline bool matches(std::uintptr_t item, std::string_view text) {
        const auto current = capture(item);
        if (!current) return text.empty();
        const auto target = desired(text);
        return std::strncmp(current->name.data(), target.name.data(), 160) == 0;
    }

    inline bool apply(std::uintptr_t item, std::string_view text) {
        const auto off = offsets();
        if (!off) return text.empty();
        const auto target = desired(text);
        return restore(item, target);
    }
}

