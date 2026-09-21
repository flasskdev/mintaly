#pragma once
#include <cstdint>
#include <core/systems/systems.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/cosmetic_model.hpp>
#include "entity_guard.hpp"

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

    inline bool update(std::uintptr_t pawn, std::uint64_t mesh, bool bind) {
        const auto player = entity_guard::capture(pawn);
        const auto services_offset = SCHEMA("C_BasePlayerPawn", "m_pWeaponServices"_hash);
        const auto active_offset = SCHEMA("CPlayer_WeaponServices", "m_hActiveWeapon"_hash);
        const auto manager = SCHEMA("C_EconEntity", "m_AttributeManager"_hash);
        const auto item = SCHEMA("C_AttributeContainer", "m_Item"_hash);
        const auto state = SCHEMA("CSkeletonInstance", "m_modelState"_hash);
        const auto name_offset = SCHEMA("CModelState", "m_ModelName"_hash);
        const auto get_model = PATTERN(patterns::weapon_get_model_path);
        if (!player || !services_offset || !active_offset || !manager || !item ||
            !state || !name_offset || !get_model) return false;
        const auto active_handle = [&]() {
            const auto services = memory::safe_read<std::uintptr_t>(pawn + services_offset).value_or(0);
            return services ? memory::safe_read<std::uint32_t>(services + active_offset).value_or(0) : 0;
        };
        const auto handle = active_handle();
        const auto weapon = entity_guard::capture(systems::g_entities.lookup(handle));
        if (!weapon) return false;
        const auto local = systems::g_local.get();
        const bool is_firstperson = (pawn == local.pawn || (local.observer_pawn && pawn == local.observer_pawn));
        const auto path = memory::call<const char*>(get_model, weapon->entity + manager + item);
        const auto target = path ? memory::read_string(reinterpret_cast<std::uintptr_t>(path)) : std::string{};
        if (target.empty()) return false;

        const auto existing_hud = find(pawn);
        bool model_mismatch = true;
        if (existing_hud) {
            const auto scene = memory::safe_read<std::uintptr_t>(existing_hud +
                SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash)).value_or(0);
            if (scene) {
                const auto name = memory::safe_read<std::uintptr_t>(scene + state + name_offset).value_or(0);
                if (name && cosmetic_model::matches(memory::read_string(name), target)) {
                    model_mismatch = false;
                }
            }
        }

        // A remote world weapon must not force a local viewmodel binding unless spectated in first-person.
        // When spectating a synchronized player or viewing a custom knife whose HUD model/anim graph
        // is still the default knife, bind the viewmodel so custom animations (inspect, deploy, attacks) play.
        const bool should_bind = (bind || model_mismatch) && is_firstperson;
        if (should_bind) {
            static std::uintptr_t s_last_bound_weapon{ 0 };
            static std::chrono::steady_clock::time_point s_last_bind_time{};
            const auto now = std::chrono::steady_clock::now();
            if (weapon->entity != s_last_bound_weapon || (now - s_last_bind_time) > std::chrono::milliseconds(250)) {
                const auto binding = PATTERN(patterns::weapon_get_viewmodel);
                if (!binding) return false;
                memory::call<void>(binding, weapon->entity);
                s_last_bound_weapon = weapon->entity;
                s_last_bind_time = now;
            }
        }
        if (!entity_guard::current(*player) || !entity_guard::current(*weapon) || active_handle() != handle)
            return false;
        const auto hud = entity_guard::capture(find(pawn)); // Binding may recreate the HUD.
        if (!hud) return false;
        if (!entity_guard::current(*player) || !entity_guard::current(*weapon) ||
            !entity_guard::current(*hud) || active_handle() != handle || find(pawn) != hud->entity) return false;
        const auto scene = memory::safe_read<std::uintptr_t>(hud->entity +
            SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash)).value_or(0);
        if (!scene) return false;
        const auto name = memory::safe_read<std::uintptr_t>(scene + state + name_offset).value_or(0);
        if (!name || !cosmetic_model::matches(memory::read_string(name), target)) {
            if (!entity_guard::set_model(*hud, target.c_str())) return false;
        }
        if (!entity_guard::current(*player) || !entity_guard::current(*weapon) ||
            active_handle() != handle || find(pawn) != hud->entity) return false;
        return entity_guard::set_mesh(*hud, mesh);
    }
}
