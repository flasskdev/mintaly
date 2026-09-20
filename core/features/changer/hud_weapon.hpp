#pragma once
#include <cstdint>
#include <core/systems/systems.hpp>
#include <utilities/memory/memory.hpp>

namespace features::changer::hud_weapon {
    // Multiple HUD children can coexist during a weapon switch. Never select
    // the first child by class alone, or overwrite a holstered weapon's model.
    inline std::uintptr_t find(std::uintptr_t pawn) {
        if (!pawn) return 0;
        const auto services_offset = SCHEMA("C_BasePlayerPawn", "m_pWeaponServices"_hash);
        const auto active_offset = SCHEMA("CPlayer_WeaponServices", "m_hActiveWeapon"_hash);
        const auto arms_offset = SCHEMA("C_CSPlayerPawn", "m_hHudModelArms"_hash);
        const auto weapon_offset = SCHEMA("C_CS2HudModelWeapon", "m_hWeapon"_hash);
        const auto scene_offset = SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash);
        const auto child_offset = SCHEMA("CGameSceneNode", "m_pChild"_hash);
        const auto sibling_offset = SCHEMA("CGameSceneNode", "m_pNextSibling"_hash);
        const auto parent_offset = SCHEMA("CGameSceneNode", "m_pParent"_hash);
        const auto owner_offset = SCHEMA("CGameSceneNode", "m_pOwner"_hash);
        if (!services_offset || !active_offset || !arms_offset || !weapon_offset ||
            !scene_offset || !child_offset || !sibling_offset || !parent_offset || !owner_offset) return 0;
        const auto services = memory::safe_read<std::uintptr_t>(pawn + services_offset).value_or(0);
        if (!services) return 0;
        const auto active = memory::safe_read<std::uint32_t>(services + active_offset).value_or(0);
        if (!systems::g_entities.lookup(active)) return 0;
        const auto arms = systems::g_entities.lookup(memory::safe_read<std::uint32_t>(pawn + arms_offset).value_or(0));
        if (!arms) return 0;
        const auto scene = memory::safe_read<std::uintptr_t>(arms + scene_offset).value_or(0);
        if (!scene) return 0;
        auto child = memory::safe_read<std::uintptr_t>(scene + child_offset).value_or(0);
        for (int i = 0; child && i < 128; ++i) {
            if (memory::safe_read<std::uintptr_t>(child + parent_offset).value_or(0) != scene) return 0;
            const auto owner = memory::safe_read<std::uintptr_t>(child + owner_offset).value_or(0);
            const auto name = owner ? systems::g_entities.get_schema_name(owner) : nullptr;
            if (name && fnv1a::runtime_hash(name) == "C_CS2HudModelWeapon"_hash &&
                memory::safe_read<std::uintptr_t>(owner + scene_offset).value_or(0) == child &&
                memory::safe_read<std::uint32_t>(owner + weapon_offset).value_or(0xffffffffu) == active)
                return owner;
            child = memory::safe_read<std::uintptr_t>(child + sibling_offset).value_or(0);
        }
        return 0; // Missing binding: retry next frame, never guess.
    }
}
