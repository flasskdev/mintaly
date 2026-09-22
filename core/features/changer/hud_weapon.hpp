#pragma once
#include <cstdint>
#include <core/systems/systems.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/cosmetic_model.hpp>
<<<<<<< HEAD
#include <utilities/hud_weapon_binding.hpp>
#include <array>
=======
#include <utilities/hud_binding.hpp>
#include <chrono>
>>>>>>> fb47dc3818127cef83b2780e079c3550143312a7
#include "entity_guard.hpp"

namespace features::changer::hud_weapon {
    inline std::uint32_t weapon_handle_offset() {
        // schemas::lookup searches declared fields only, not inherited fields.
        const auto direct = SCHEMA("C_CS2HudModelWeapon", "m_hWeapon"_hash);
        return direct ? direct : SCHEMA("C_CS2HudModelBase", "m_hWeapon"_hash);
    }

    inline std::uint32_t hud_handle_offset() {
        // Optional forward links: use only offsets actually exposed by this build.
        const auto direct = SCHEMA("C_CSWeaponBase", "m_hHudModel"_hash);
        if (direct) return direct;
        return SCHEMA("C_BasePlayerWeapon", "m_hHudModel"_hash);
    }

    inline void report_lookup(std::uint32_t active, unsigned candidates, unsigned matches,
        std::uint32_t reverse_offset, std::uint32_t forward_offset) {
        static auto next = std::chrono::steady_clock::time_point{};
        const auto now = std::chrono::steady_clock::now();
        if (now < next) return;
        next = now + std::chrono::seconds(5);
        diag::writef(diag::level::warning,
            "[skin-hud] active=%u candidates=%u matches=%u reverse_offset=%u forward_offset=%u",
            static_cast<unsigned>(active), candidates, matches,
            static_cast<unsigned>(reverse_offset), static_cast<unsigned>(forward_offset));
        static bool dumped = false;
        if (!dumped && addresses::globals::schema_system) {
            dumped = true;
            for (const auto* name : {"C_CS2HudModelWeapon", "C_CS2HudModelBase", "C_CSWeaponBase", "C_BasePlayerWeapon"})
                systems::schemas::dump_fields(name);
        }
    }

    // Multiple HUD children can coexist during a weapon switch. Never select
    // the first child by class alone, or overwrite a holstered weapon's model.
    struct lookup_result {
        std::uintptr_t entity{};
        const char* reason{"unready-player"};
        std::size_t candidates{};
    };

    inline lookup_result locate(std::uintptr_t pawn) {
        const auto player = entity_guard::capture(pawn);
        if (!player) return {};
        const auto services_offset = SCHEMA("C_BasePlayerPawn", "m_pWeaponServices"_hash);
        const auto active_offset = SCHEMA("CPlayer_WeaponServices", "m_hActiveWeapon"_hash);
        const auto arms_offset = SCHEMA("C_CSPlayerPawn", "m_hHudModelArms"_hash);
<<<<<<< HEAD
        const auto weapon_offset = weapon_handle_offset(); // Optional in current CS2 builds.
=======
        const auto weapon_offset = weapon_handle_offset();
        const auto forward_offset = hud_handle_offset();
        const auto entity_owner_offset = SCHEMA("C_BaseEntity", "m_hOwnerEntity"_hash);
>>>>>>> fb47dc3818127cef83b2780e079c3550143312a7
        const auto scene_offset = SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash);
        const auto child_offset = SCHEMA("CGameSceneNode", "m_pChild"_hash);
        const auto sibling_offset = SCHEMA("CGameSceneNode", "m_pNextSibling"_hash);
        const auto parent_offset = SCHEMA("CGameSceneNode", "m_pParent"_hash);
        const auto owner_offset = SCHEMA("CGameSceneNode", "m_pOwner"_hash);
<<<<<<< HEAD
        const auto weapon_owner_offset = SCHEMA("C_BaseEntity", "m_hOwnerEntity"_hash);
        if (!services_offset || !active_offset || !arms_offset || !weapon_owner_offset ||
            !scene_offset || !child_offset || !sibling_offset || !parent_offset || !owner_offset)
            return {0, "scene-schema"};
=======
        if (!services_offset || !active_offset || !arms_offset ||
            !scene_offset || !child_offset || !sibling_offset || !parent_offset || !owner_offset) return 0;
>>>>>>> fb47dc3818127cef83b2780e079c3550143312a7
        const auto services = memory::safe_read<std::uintptr_t>(pawn + services_offset).value_or(0);
        if (!services) return {0, "weapon-services"};
        const auto active = memory::safe_read<std::uint32_t>(services + active_offset).value_or(0);
<<<<<<< HEAD
        const auto weapon = entity_guard::capture(systems::g_entities.lookup(active));
        if (!weapon) return {0, "active-weapon"};
        const auto owned_by_player = [&]() {
            const auto owner = memory::safe_read<std::uint32_t>(weapon->entity + weapon_owner_offset).value_or(0);
            return systems::g_entities.lookup(owner) == pawn;
        };
        if (!owned_by_player()) return {0, "weapon-owner"};
        const auto arms_handle = memory::safe_read<std::uint32_t>(pawn + arms_offset).value_or(0);
        const auto arms = entity_guard::capture(systems::g_entities.lookup(arms_handle));
        if (!arms) return {0, "hud-arms"};
        const auto scene = memory::safe_read<std::uintptr_t>(arms->entity + scene_offset).value_or(0);
        if (!scene) return {0, "arms-scene"};

        std::string target;
        const auto state = SCHEMA("CSkeletonInstance", "m_modelState"_hash);
        const auto name_offset = SCHEMA("CModelState", "m_ModelName"_hash);
        if (!weapon_offset) {
            const auto manager = SCHEMA("C_EconEntity", "m_AttributeManager"_hash);
            const auto item = SCHEMA("C_AttributeContainer", "m_Item"_hash);
            const auto get_model = PATTERN(patterns::weapon_get_model_path);
            if (!manager || !item || !state || !name_offset || !get_model)
                return {0, "model-dependencies"};
            const auto path = memory::call<const char*>(get_model, weapon->entity + manager + item);
            if (path) target = memory::read_string(reinterpret_cast<std::uintptr_t>(path));
            if (target.empty()) return {0, "active-model-path"};
        }

        std::array<hud_binding::candidate, 128> candidates{};
        std::size_t count = 0;
        const auto first = memory::safe_read<std::uintptr_t>(scene + child_offset);
        if (!first) return {0, "unreadable-children"};
        auto child = *first;
=======
        const auto weapon = systems::g_entities.lookup(active);
        if (!weapon) return 0;
        const auto forward = forward_offset ? systems::g_entities.lookup(
            memory::safe_read<std::uint32_t>(weapon + forward_offset).value_or(0xffffffffu)) : 0;
        const auto arms = systems::g_entities.lookup(memory::safe_read<std::uint32_t>(pawn + arms_offset).value_or(0));
        if (!arms) return 0;
        const auto scene = memory::safe_read<std::uintptr_t>(arms + scene_offset).value_or(0);
        if (!scene) return 0;
        std::uintptr_t found{};
        unsigned candidates{}, matches{};
        auto child = memory::safe_read<std::uintptr_t>(scene + child_offset).value_or(0);
>>>>>>> fb47dc3818127cef83b2780e079c3550143312a7
        for (int i = 0; child && i < 128; ++i) {
            if (memory::safe_read<std::uintptr_t>(child + parent_offset).value_or(0) != scene)
                return {0, "changed-parent", count};
            const auto owner = memory::safe_read<std::uintptr_t>(child + owner_offset).value_or(0);
            const auto name = owner ? systems::g_entities.get_schema_name(owner) : nullptr;
<<<<<<< HEAD
            if (!name) return {0, "unreadable-child-class", count};
            if (fnv1a::runtime_hash(name) == "C_CS2HudModelWeapon"_hash) {
                auto& candidate = candidates[count++];
                candidate.entity = owner;
                candidate.ready = entity_guard::ready(owner) &&
                    memory::safe_read<std::uintptr_t>(owner + scene_offset).value_or(0) == child;
                if (weapon_offset) {
                    // An unreadable/mismatched existing field must NOT use the fallback.
                    candidate.weapon = memory::safe_read<std::uint32_t>(owner + weapon_offset).value_or(0xffffffffu);
                } else {
                    const auto model_name = memory::safe_read<std::uintptr_t>(child + state + name_offset).value_or(0);
                    candidate.model_matches = model_name && cosmetic_model::matches(memory::read_string(model_name), target);
                }
            }
            const auto next = memory::safe_read<std::uintptr_t>(child + sibling_offset);
            if (!next) return {0, "unreadable-sibling", count};
            child = *next;
        }
        if (child) return {0, "child-limit-or-cycle", count};
        if (!entity_guard::current(*player) || !entity_guard::current(*weapon) || !entity_guard::current(*arms) ||
            memory::safe_read<std::uintptr_t>(pawn + services_offset).value_or(0) != services ||
            memory::safe_read<std::uint32_t>(services + active_offset).value_or(0) != active ||
            memory::safe_read<std::uint32_t>(pawn + arms_offset).value_or(0) != arms_handle || !owned_by_player())
            return {0, "binding-changed", count};
        const auto selected = hud_binding::select(
            std::span<const hud_binding::candidate>{candidates.data(), count}, active, weapon_offset != 0);
        return {selected.entity, hud_binding::describe(selected.state), count};
    }

    inline std::uintptr_t find(std::uintptr_t pawn) {
        return locate(pawn).entity;
=======
            if (name && fnv1a::runtime_hash(name) == "C_CS2HudModelWeapon"_hash &&
                memory::safe_read<std::uintptr_t>(owner + scene_offset).value_or(0) == child) {
                ++candidates;
                const auto reverse = weapon_offset ? memory::safe_read<std::uint32_t>(owner + weapon_offset)
                    : std::optional<std::uint32_t>{};
                // A resolved field that cannot be read is not an absent field.
                if (weapon_offset && !reverse) return 0;
                const auto entity_owner = entity_owner_offset ? memory::safe_read<std::uint32_t>(
                    owner + entity_owner_offset).value_or(0xffffffffu) : 0xffffffffu;
                if (hud_binding::matches(active, owner, reverse, entity_owner, forward)) {
                    found = owner;
                    ++matches;
                }
            }
            child = memory::safe_read<std::uintptr_t>(child + sibling_offset).value_or(0);
        }
        // Reject truncated/cyclic lists and ambiguous bindings, even with a matching model name.
        if (child || matches != 1) {
            report_lookup(active, candidates, matches, weapon_offset, forward_offset);
            return 0;
        }
        return found;
>>>>>>> fb47dc3818127cef83b2780e079c3550143312a7
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
        // A paint refresh does not require rebuilding an already matching model binding.
        const bool should_bind = model_mismatch && is_firstperson;
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
        const bool changed_model = !name || !cosmetic_model::matches(memory::read_string(name), target);
        if (changed_model && !entity_guard::set_model(*hud, target.c_str())) return false;
        if (!entity_guard::current(*player) || !entity_guard::current(*weapon) ||
            active_handle() != handle || find(pawn) != hud->entity) return false;
        if (!entity_guard::set_mesh(*hud, mesh)) return false;
        if (bind || model_mismatch || changed_model) {
            // Refresh through the real econ weapon AFTER HUD binding/SetModel.
            // C_CS2HudModelWeapon is not an econ entity; never write an item view into it.
            const auto skin = PATTERN(patterns::weapon_update_skin);
            if (!skin) return false;
            memory::call<void>(skin, weapon->entity, true);
        }
        if (!entity_guard::current(*player) || !entity_guard::current(*weapon) ||
            active_handle() != handle || find(pawn) != hud->entity) return false;
        return entity_guard::set_mesh(*hud, mesh);
    }
}
