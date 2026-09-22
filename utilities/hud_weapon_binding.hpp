#pragma once
#include <cstdint>
#include <span>
#include <utilities/hud_binding.hpp>

namespace hud_binding {
    inline constexpr std::uint32_t invalid_handle = 0xffffffffu;

    struct candidate {
        std::uintptr_t entity{};
        std::uint32_t weapon{invalid_handle};
        bool ready{};
        bool model_matches{};
        std::uint32_t owner{invalid_handle};
    };

    enum class status { missing, invalid_active, ambiguous, wrong_binding, model_mismatch, linked, scene_model };
    struct result {
        std::uintptr_t entity{};
        status state{status::missing};
    };

    // Prefer explicit links, including when several HUD children coexist.
    // A model-only fallback still requires exactly one child in total.
    inline result select(std::span<const candidate> children, std::uint32_t active,
        bool has_weapon_field, std::uintptr_t forward = 0) {
        if (!active || active == invalid_handle) return {0, status::invalid_active};
        if (children.empty()) return {};
        result selected{0, status::wrong_binding};
        bool has_link = has_weapon_field || forward != 0;
        for (const auto& child : children) {
            has_link = has_link || child.owner == active;
            const auto reverse = has_weapon_field ? std::optional<std::uint32_t>{child.weapon} : std::nullopt;
            if (!matches(active, child.entity, reverse, child.owner, forward)) continue;
            // Count even unready linked children: readiness is not identity.
            if (selected.entity) return {0, status::ambiguous};
            selected = {child.entity, child.ready ? status::linked : status::missing};
        }
        if (selected.entity)
            return selected.state == status::linked ? selected : result{};
        if (has_link) return {0, status::wrong_binding};
        if (children.size() != 1) return {0, status::ambiguous};
        const auto& child = children.front();
        if (!child.entity || !child.ready) return {};
        if (!child.model_matches) return {0, status::model_mismatch};
        return {child.entity, status::scene_model};
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
