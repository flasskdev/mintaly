#pragma once
#include <utilities/cstypes.hpp>

namespace chams_weapons {
// Keep gameplay weapon groups unchanged; cosmetic knives and utility have their own IDs.
inline constexpr auto entries = [] {
    constexpr cstypes::weapons::weapon_entry extra[] = {
        {31, "Zeus x27", "taser", 5}, {42, "CT Knife", "knife_ct", 5}, {59, "T Knife", "knife_t", 5},
        {500, "Bayonet", "bayonet", 5}, {503, "Classic Knife", "classic", 