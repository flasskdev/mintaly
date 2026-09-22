#pragma once
#include <cstdint>
#include <optional>
#include <core/systems/systems.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/lifecycle.hpp>
#include <protection/game_addresses.hpp>

namespace features::changer::entity_guard {
    struct stamp {
        std::uintptr_t entity{};
        std::uintptr_t identity{};
        std::uint32_t handle{};
        bool operator==(const stamp&) const = default;
    };

    // Game-thread only. Reject unknown layouts and all nonzero identity flags:
    // EF_IN_STAGING_LIST's bit is not verified for this build. Never guess it.
    inline std::optional<stamp> capture(std::uintptr_t entity) {
        if (lifecycle::is_unloading() || !addresses::globals::schema_system || entity < 0x10000)
            return std::nullopt;
        const auto flags_offset = SCHEMA("CEntityIdentity", "m_flags"_hash);
        const auto scene_offset = SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash);
        const auto owner_offset = SCHEMA("CGameSceneNode", "m_pOwner"_hash);
        const auto dormant_offset = SCHEMA("CGameSceneNode", "m_bDormant"_hash);
        if (!flags_offset || !scene_offset || !owner_offset || !dormant_offset) return std::nullopt;
        // Identity/handle links use the same layout as entities::lookup().
        const auto identity = memory::safe_read<std::uintptr_t>(entity + 0x10).value_or(0);
        if (!identity) return std::nullopt;
        const auto flags = memory::safe_read<std::uint32_t>(identity + flags_offset);
        // if (!flags || *flags != 0)
        if (!flags || (*flags & 0x4) != 0) return std::nullopt;
        const auto handle = memory::safe_read<std::uint32_t>(identity + 0x10).value_or(0xffffffffu);
        if (systems::g_entities.lookup(handle) != entity) return std::nullopt;
        const auto scene = memory::safe_read<std::uintptr_t>(entity + scene_offset).value_or(0);
        if (!scene || memory::safe_read<std::uintptr_t>(scene + owner_offset).value_or(0) != entity ||
            memory::safe_read<bool>(scene + dormant_offset).value_or(false)) return std::nullopt;
        return stamp{entity, identity, handle};
    }

    inline bool ready(std::uintptr_t entity) { return capture(entity).has_value(); }
    inline bool current(const stamp& expected) {
        const auto actual = capture(expected.entity);
        return actual && *actual == expected;
    }
    inline bool set_model(const stamp& expected, const char* path) {
        const auto set = PATTERN(patterns::set_player_model);
        if (!set || !path || !*path || !current(expected)) return false;
        memory::call<void>(set, expected.entity, path);
        return current(expected);
    }
    inline bool set_mesh(const stamp& expected, std::uint64_t mask) {
        const auto set = PATTERN(patterns::weapon_set_mesh_group_mask);
        if (!set || !current(expected)) return false;
        const auto scene = memory::safe_read<std::uintptr_t>(expected.entity +
            SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash)).value_or(0);
        if (!scene) return false;
        const auto state = SCHEMA("CSkeletonInstance", "m_modelState"_hash);
        const auto mesh = SCHEMA("CModelState", "m_MeshGroupMask"_hash);
        if (state && mesh) {
            const auto applied = memory::safe_read<std::uint64_t>(scene + state + mesh);
            if (applied && *applied == mask) return true;
        }
        memory::call<void>(set, scene, mask);
        return current(expected);
    }
    inline bool rebuild_materials(const stamp& expected, std::uint64_t mask) {
        const auto composite = PATTERN(patterns::weapon_update_composite_material);
        const auto skin = PATTERN(patterns::weapon_update_skin);
        if (!composite || !skin || !set_mesh(expected, mask)) return false;
        // Existing build-dependent composite-material subobject layout.
        memory::call<void>(composite, expected.entity + 0x608, true);
        if (!current(expected)) return false;
        memory::call_vfunc<void>(expected.entity, 10, 1);
        if (!current(expected)) return false;
        memory::call<void>(skin, expected.entity, true);
        return current(expected);
    }
}
