#pragma once
#include <utilities/cstypes.hpp>

namespace chams_weapons {
inline constexpr auto entries = [] {
    constexpr cstypes::weapons::weapon_entry extra[] = {
        {31, "Zeus x27", "taser", 5}, {42, "CT Knife", "knife_ct", 5}, {59, "T Knife", "knife_t", 5},
        {500, "Bayonet", "bayonet", 5}, {503, "Classic Knife", "classic", 5},
        {505, "Flip Knife", "flip", 5}, {506, "Gut Knife", "gut", 5}, {507, "Karambit", "karambit", 5},
        {508, "M9 Bayonet", "m9", 5}, {509, "Huntsman Knife", "huntsman", 5},
        {512, "Falchion Knife", "falchion", 5}, {514, "Bowie Knife", "bowie", 5},
        {515, "Butterfly Knife", "butterfly", 5}, {516, "Shadow Daggers", "daggers", 5},
        {517, "Paracord Knife", "paracord", 5}, {518, "Survival Knife", "survival", 5},
        {519, "Ursus Knife", "ursus", 5}, {520, "Navaja Knife", "navaja", 5},
        {521, "Nomad Knife", "nomad", 5}, {522, "Stiletto Knife", "stiletto", 5},
        {523, "Talon Knife", "talon", 5}, {525, "Skeleton Knife", "skeleton", 5},
        {526, "Kukri Knife", "kukri", 5}
    };
    std::array<cstypes::weapons::weapon_entry, cstypes::weapons::k_total_weapons + sizeof(extra) / sizeof(extra[0])> result{};
    std::size_t index = 0;
    for (const auto& weapon : cstypes::weapons::k_weapons) result[index++] = weapon;
    for (const auto& weapon : extra) result[index++] = weapon;
    return result;
}();
inline constexpr int index(std::uint16_t id) {
    for (std::size_t i = 0; i < entries.size(); ++i)
        if (entries[i].id == id) return static_cast<int>(i);
    return -1;
}
inline constexpr auto names = [] {
    std::array<const char*, entries.size() + 1> result{};
    result[0] = "Default";
    for (std::size_t i = 0; i < entries.size(); ++i) result[i + 1] = entries[i].name;
    return result;
}();
}
