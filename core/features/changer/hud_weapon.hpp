#pragma once
#include <cstdint>
#include <core/systems/systems.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/cosmetic_model.hpp>
#include <utilities/hud_weapon_binding.hpp>
#include <utilities/diag.hpp>
#include <array>
#include <chrono>
#include "entity_guard.hpp"

namespace features::changer::hud_weapon {
    inline std::uint32_t weapon_handle_offset() {
        // schemas::lookup searches declared fields only, not inherited fields.
        const auto direct = SCHEMA("C_CS2HudModelWeapon", "m_hWeapon"_hash);
        return direct ? direct : SCHEMA("C_CS2HudModelBase", "m_hWeapon"_hash);
    }

    inline std::uint32_t hud_handle_offset() {
        const auto direct = SCHEMA("C_CSWeaponBase", "m_hHudModel"_hash);
        return direct ? direct : SCHEMA("C_BasePlayerWeapon", "m_hHudModel"_hash);
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
        const auto weapon_offset = weapon_handle_offset();
        const auto forward_offset = hud_handle_offset();
        const auto scene_offset = SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash);
        const auto child_offset = SCHEMA("CGameSceneNode", "m_pChild"_hash);
        const auto sibling_offset = SCHEMA("CGameSceneNode", "m_pNextSibling"_hash);
        const auto parent_offset = SCHEMA("CGameSceneNode", "m_pParent"_hash);
        const auto owner_offset = SCHEMA("CGameSceneNode", "m_pOwner"_hash);
        const auto weapon_owner_offset = SCHEMA("C_BaseEntity", "m_hOwnerEntity"_hash);
        if (!services_offset || !active_offset || !arms_offset || !weapon_owner_offset ||
            !scene_offset || !child_offset || !sibling_offset || !parent_offset || !owner_offset)
            return {0, "scene-schema"};
        const auto services = memory::safe_read<std::uintptr_t>(pawn + services_offset).value_or(0);
        if (!services) return {0, "weapon-services"};
        const auto active = memory::safe_read<std::uint32_t>(services + active_offset).value_or(0);
        const auto weapon = entity_guard::capture(systems::g_entities.lookup(active));
        if (!weapon) return {0, "active-weapon"};
        const auto owned_by_player = [&]() {
            const auto owner = memory::safe_read<std::uint32_t>(weapon->entity + weapon_owner_offset).value_or(0);
            return systems::g_entities.lookup(owner) == pawn;
        };
        if (!owned_by_player()) return {0, "weapon-owner"};
        std::uint32_t forward_handle = hud_binding::invalid_handle;
        if (forward_offset) {
            const auto value = memory::safe_read<std::uint32_t>(weapon->entity + forward_offset);
            if (!value) return {0, "unreadable-forward-link"};
            forward_handle = *value;
        }
        const bool has_forward = forward_handle && forward_handle != hud_binding::invalid_handle;
        const auto forward = has_forward ? systems::g_entities.lookup(forward_handle) : 0;
        if (has_forward && !forward) return {0, "unready-forward-link"};
        const auto arms_handle = memory::safe_read<std::uint32_t>(pawn + arms_offset).value_or(0);
        const auto arms = entity_guard::capture(systems::g_entities.lookup(arms_handle));
        if (!arms) return {0, "hud-arms"};
        const auto scene = memory::safe_read<std::uintptr_t>(arms->entity + scene_offset).value_or(0);
        if (!scene) return {0, "arms-scene"};

        std::array<hud_binding::candidate, 128> candidates{};
        std::size_t count = 0;
        unsigned matches = 0;
        const auto first = memory::safe_read<std::uintptr_t>(scene + child_offset);
        if (!first) return {0, "unreadable-children"};
        auto child = *first;
        for (int i = 0; child && i < 128; ++i) {
            if (memory::safe_read<std::uintptr_t>(child + parent_offset).value_or(0) != scene)
                return {0, "changed-parent", count};
            const auto owner = memory::safe_read<std::uintptr_t>(child + owner_offset).value_or(0);
            const auto name = owner ? systems::g_entities.get_schema_name(owner) : nullptr;
            if (!name) return {0, "unreadable-child-class", count};
            if (fnv1a::runtime_hash(name) == "C_CS2HudModelWeapon"_hash) {
                auto& candidate = candidates[count++];
                candidate.entity = owner;
                candidate.ready = entity_guard::ready(owner) &&
                    memory::safe_read<std::uintptr_t>(owner + scene_offset).value_or(0) == child;
                const auto entity_owner = memory::safe_read<std::uint32_t>(owner + weapon_owner_offset);
                if (!entity_owner) return {0, "unreadable-owner-link", count};
                candidate.owner = *entity_owner;
                std::optional<std::uint32_t> reverse;
                if (weapon_offset) {
                    reverse = memory::safe_read<std::uint32_t>(owner + weapon_offset);
                    if (!reverse) return {0, "unreadable-reverse-link", count};
                    candidate.weapon = *reverse;
                }
                if (hud_binding::matches(active, owner, reverse, candidate.owner, forward)) ++matches;
            }
            const auto next = memory::safe_read<std::uintptr_t>(child + sibling_offset);
            if (!next) return {0, "unreadable-sibling", count};
            child = *next;
        }
        if (child) return {0, "child-limit-or-cycle", count};
        const auto children = std::span<const hud_binding::candidate>{candidates.data(), count};
        auto selected = hud_binding::select(children, active, weapon_offset != 0, forward);
        // A missing model-path signature must not block a proven entity link.
        if (!selected.entity && selected.state == hud_binding::status::model_mismatch &&
            !weapon_offset && !forward && count == 1) {
            const auto manager = SCHEMA("C_EconEntity", "m_AttributeManager"_hash);
            const auto item = SCHEMA("C_AttributeContainer", "m_Item"_hash);
            const auto state = SCHEMA("CSkeletonInstance", "m_modelState"_hash);
            const auto name_offset = SCHEMA("CModelState", "m_ModelName"_hash);
            const auto get_model = PATTERN(patterns::weapon_get_model_path);
            if (!manager || !item || !state || !name_offset || !get_model)
                return {0, "model-dependencies", count};
            const auto path = memory::call<const char*>(get_model, weapon->entity + manager + item);
            const auto target = path ? memory::read_string(reinterpret_cast<std::uintptr_t>(path)) : std::string{};
            if (target.empty()) return {0, "active-model-path", count};
            const auto candidate_scene = memory::safe_read<std::uintptr_t>(candidates[0].entity + scene_offset).value_or(0);
            const auto model_name = candidate_scene ? memory::safe_read<std::uintptr_t>(candidate_scene + state + name_offset).value_or(0) : 0;
            candidates[0].model_matches = model_name && cosmetic_model::matches(memory::read_string(model_name), target);
            selected = hud_binding::select(children, active, false);
        }
        if (!entity_guard::current(*player) || !entity_guard::current(*weapon) || !entity_guard::current(*arms) ||
            memory::safe_read<std::uintptr_t>(pawn + services_offset).value_or(0) != services ||
            memory::safe_read<std::uint32_t>(services + active_offset).value_or(0) != active ||
            memory::safe_read<std::uint32_t>(pawn + arms_offset).value_or(0) != arms_handle || !owned_by_player() ||
            (forward_offset && memory::safe_read<std::uint32_t>(weapon->entity + forward_offset).value_or(0) != forward_handle))
            return {0, "binding-changed", count};
        if (!selected.entity)
            report_lookup(active, static_cast<unsigned>(count), matches, weapon_offset, forward_offset);
        return {selected.entity, hud_binding::describe(selected.state), count};
    }

    inline std::uintptr_t find(std::uintptr_t pawn) {
        return locate(pawn).entity;
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

        const auto lookup = locate(pawn);
        const auto existing_hud = lookup.entity;
        // Unresolved children are not evidence of a missing model. Rebinding
        // on every ambiguous lookup can repeatedly recreate overlapping HUDs.
        if (!existing_hud && lookup.candidates) return false;
        bool model_mismatch = true;
        if (existing_hud) {
            const auto scene = memory::safe_read<std::uintptr_t>(existing_hud +
                SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash)).value_or(0);
            if (scene) {
                const auto name = memory::safe_read<std::uintptr_t>(scene + state + name_offset).value_or(0);
                if (name && cosmetic_model::matches(memory::read_string(name), target))
                    model_mismatch = false;
            }
        }

        // Only bind local/spectated first-person models, never remote world weapons.
        const bool should_bind = model_mismatch && is_firstperson;
        if (should_bind) {
            static std::uintptr_t s_last_bound_weapon{0};
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
            // Refresh via the real econ weapon; a HUD model has no item view.
            const auto skin = PATTERN(patterns::weapon_update_skin);
            if (!skin) return false;
            memory::call<void>(skin, weapon->entity, true);
        }
        if (!entity_guard::current(*player) || !entity_guard::current(*weapon) ||
            active_handle() != handle || find(pawn) != hud->entity) return false;
        return entity_guard::set_mesh(*hud, mesh);
    }
}
