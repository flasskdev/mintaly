#pragma once
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <core/systems/systems.hpp>

namespace features::esp::detail {
inline std::uint16_t weapon_definition(std::uintptr_t weapon) {
    if (weapon < 0x10000) return 0;
    const auto item = weapon + SCHEMA("C_EconEntity", "m_AttributeManager"_hash) + SCHEMA("C_AttributeContainer", "m_Item"_hash);
    return memory::safe_read<std::uint16_t>(item + SCHEMA("C_EconItemView", "m_iItemDefinitionIndex"_hash)).value_or(0);
}
inline std::uint16_t active_weapon_definition() {
    const auto pawn = systems::g_local.get().view_pawn();
    if (pawn < 0x10000) return 0;
    const auto services = memory::safe_read<std::uintptr_t>(pawn + SCHEMA("C_BasePlayerPawn", "m_pWeaponServices"_hash)).value_or(0);
    if (services < 0x10000) return 0;
    const auto handle = memory::safe_read<std::uint32_t>(services + SCHEMA("CPlayer_WeaponServices", "m_hActiveWeapon"_hash)).value_or(0xffffffff);
    return weapon_definition(systems::g_entities.lookup(handle));
}
}
