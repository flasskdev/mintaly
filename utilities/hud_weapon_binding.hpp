#pragma once
#include <cstdint>
#include <span>

namespace hud_binding {
    inline constexpr std::uint32_t invalid_handle = 0xffffffffu;

    struct candidate {
        std::uintptr_t entity{};
        std::uint32_t weapon{invalid_handle};
        bool ready{};
        bool model_matches{};
    };

    enum class status { missing, invalid_active, ambiguous, wrong_binding, model_mismatch, linked, scene_model };
    struct result {
        std::uintptr_t entity{};
        status state{status::missing};
    };

    // With no network link, require ONE HUD child in total, not merely one
    // matching model. Old/new children can overlap during weapon switches.
    inline result select(std::span<const candidate> children, std::uint32_t active, bool has_weapon_field) {
        if (!active || active == invalid_handle) return {0, status::invalid_active};
        if (children.empty()) return {};
        if (!has_weapon_field) {
            if (children.size() != 1) return {0, status::ambiguous};
            const auto& child = children.front();
            if (!child.entity || !child.ready) return {};
            if (!child.model_matches) return {0, status::model_mismatch};
            return {child.entity, status::scene_model};
        }
        result selected{0, status::wrong_binding};
        for (const auto& child : children) {
            if (!child.entity || !child.ready || child.weapon != active) continue;
            if (selected.entity) return {0, status::ambiguous};
            selected = {child.entity, status::linked};
        }
        return selected;
    }

    inline const char* describe(status state) {
        switch (state) {
        case status::missing: return "missing-or-unready-child";
        case status::invalid_active: return "invalid-active-handle";
        case status::ambiguous: return "ambiguous-children";
        case status::wrong_binding: return "wrong-weapon-binding";
        case status::model_mismatch: return "model-mismatch";
        case status::linked: return "weapon-handle";
        case status::scene_model: return "scene-model";
        }
        return "unknown";
    }
}
