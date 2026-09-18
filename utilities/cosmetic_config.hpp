#pragma once

#include <external/nlohmann/json.hpp>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace cosmetic_config {
// Reject invalid IDs rather than narrowing or clamping them to a different item.
inline int integer(const nlohmann::json& value, int fallback, int maximum) {
    if (value.is_number_unsigned()) {
        const auto number = value.get<std::uint64_t>();
        return number <= static_cast<std::uint64_t>(maximum) ? static_cast<int>(number) : fallback;
    }
    if (value.is_number_integer()) {
        const auto number = value.get<std::int64_t>();
        return number >= 0 && number <= maximum ? static_cast<int>(number) : fallback;
    }
    return fallback;
}

inline int field(const nlohmann::json& object, const char* key, int fallback, int maximum) {
    if (!object.is_object()) return fallback;
    const auto it = object.find(key);
    return it == object.end() ? fallback : integer(*it, fallback, maximum);
}

inline constexpr int retain_team(int previous, int current) {
    return current == 2 || current == 3 ? current : previous;
}

struct custom_agent_entry {
    std::string name{};
    std::string model_path{};
    int team{3};
    bool operator==(const custom_agent_entry&) const = default;
};

struct custom_agents {
    std::vector<custom_agent_entry> entries;
    int selected_ct{-1};
    int selected_t{-1};
};

inline custom_agents decode_custom_agents(const nlohmann::json& object) {
    custom_agents result;
    if (!object.is_object()) return result;
    const auto list = object.find("entries");
    if (list == object.end() || !list->is_array()) return result;
    const int ct = field(object, "selected_ct", -1, std::numeric_limits<int>::max());
    const int t = field(object, "selected_t", -1, std::numeric_limits<int>::max());
    for (std::size_t i = 0; i < list->size(); ++i) {
        const auto& item = (*list)[i];
        if (!item.is_object()) continue;
        const auto path = item.contains("model_path") ? item.find("model_path") : item.find("model");
        if (path == item.end() || !path->is_string()) continue;
        custom_agent_entry entry;
        entry.model_path = path->get<std::string>();
        if (entry.model_path.empty()) continue;
        entry.team = field(item, "team", 3, 3);
        // A malformed explicit team must not silently become a CT agent.
        if (item.contains("team")) entry.team = field(item, "team", -1, 3);
        if (entry.team != 0 && entry.team != 2 && entry.team != 3) continue;
        const auto name = item.find("name");
        entry.name = name != item.end() && name->is_string() ? name->get<std::string>() : "Custom Agent";
        if (result.entries.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max())) break;
        const int index = static_cast<int>(result.entries.size());
        // Selection indices refer to the original array, before invalid entries are removed.
        if (ct >= 0 && i == static_cast<std::size_t>(ct) && (entry.team == 0 || entry.team == 3)) result.selected_ct = index;
        if (t >= 0 && i == static_cast<std::size_t>(t) && (entry.team == 0 || entry.team == 2)) result.selected_t = index;
        result.entries.push_back(std::move(entry));
    }
    return result;
}
} // namespace cosmetic_config
