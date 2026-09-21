#pragma once
#include "changer.hpp"
#include "entity_guard.hpp"
#include <unordered_map>

namespace features::changer::preview_item {
    struct state { cosmetic_cache::identity visual; settings::changer::applied_skin skin; };
    inline std::unordered_map<std::uintptr_t, state> applied;
    inline constexpr std::uint64_t item_id = 0xf000000000000010ull;
    inline void reset() { applied.clear(); }
    inline cosmetic_cache::identity identity(std::uintptr_t weapon) {
        cosmetic_cache::identity v{};
        if (!entity_guard::ready(weapon)) return v;
        v.weapon = weapon;
        v.scene = memory::safe_read<std::uintptr_t>(weapon + SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash)).value_or(0);
        v.owner = memory::safe_read<std::uint32_t>(weapon + SCHEMA("C_BaseEntity", "m_hOwnerEntity"_hash)).value_or(0);
        if (v.scene) v.model = memory::safe_read<std::uintptr_t>(v.scene + SCHEMA("CSkeletonInstance", "m_modelState"_hash)
            + SCHEMA("CModelState", "m_hModel"_hash)).value_or(0);
        return v;
    }
    inline bool available() {
        return addresses::globals::schema_system && cosmetic_attributes::available() && PATTERN(patterns::weapon_update_skin)
            && PATTERN(patterns::weapon_update_composite_material) && PATTERN(patterns::weapon_set_mesh_group_mask)
            && SCHEMA("C_EconItemView", "m_bDisallowSOC"_hash)
            && SCHEMA("C_EconItemView", "m_iItemID"_hash)
            && SCHEMA("C_EconItemView", "m_iItemIDHigh"_hash)
            && SCHEMA("C_EconItemView", "m_iItemIDLow"_hash)
            && SCHEMA("C_EconItemView", "m_iAccountID"_hash)
            && SCHEMA("C_EconItemView", "m_bInitialized"_hash)
            && SCHEMA("C_EconItemView", "m_iEntityQuality"_hash)
            && SCHEMA("C_EconEntity", "m_nFallbackPaintKit"_hash)
            && SCHEMA("C_EconEntity", "m_nFallbackSeed"_hash)
            && SCHEMA("C_EconEntity", "m_flFallbackWear"_hash)
            && SCHEMA("C_EconEntity", "m_nFallbackStatTrak"_hash);
    }
    inline bool matches(std::uintptr_t w, std::uintptr_t iv,
        const settings::changer::applied_skin& s, std::uint32_t account, int quality) {
        const auto it = applied.find(w);
        if (it != applied.end()) {
            if (it->second.skin.paint_kit_id == s.paint_kit_id && it->second.skin.seed == s.seed
                && it->second.skin.wear == s.wear && it->second.skin.stattrak == s.stattrak
                && it->second.skin.name_tag == s.name_tag && it->second.skin.stattrak_count != s.stattrak_count) {
                it->second.skin.stattrak_count = s.stattrak_count;
                memory::safe_write<int>(w + SCHEMA("C_EconEntity", "m_nFallbackStatTrak"_hash), s.stattrak ? s.stattrak_count : -1);
            }
        }
        return it != applied.end() && it->second.skin == s && cosmetic_cache::reusable(it->second.visual, identity(w))
            && memory::safe_read<std::uint64_t>(iv + SCHEMA("C_EconItemView", "m_iItemID"_hash)).value_or(0) == item_id
            && memory::safe_read<std::uint32_t>(iv + SCHEMA("C_EconItemView", "m_iItemIDHigh"_hash)).value_or(0) == 0xf0000000u
            && memory::safe_read<std::uint32_t>(iv + SCHEMA("C_EconItemView", "m_iItemIDLow"_hash)).value_or(0) == 0x10u
            && memory::safe_read<std::uint32_t>(iv + SCHEMA("C_EconItemView", "m_iAccountID"_hash)).value_or(0) == account
            && memory::safe_read<int>(iv + SCHEMA("C_EconItemView", "m_iEntityQuality"_hash)).value_or(-1) == quality
            && memory::safe_read<bool>(iv + SCHEMA("C_EconItemView", "m_bInitialized"_hash)).value_or(false)
            && memory::safe_read<bool>(iv + SCHEMA("C_EconItemView", "m_bDisallowSOC"_hash)).value_or(false)
            && memory::safe_read<int>(w + SCHEMA("C_EconEntity", "m_nFallbackPaintKit"_hash)).value_or(-1) == s.paint_kit_id
            && memory::safe_read<int>(w + SCHEMA("C_EconEntity", "m_nFallbackSeed"_hash)).value_or(-1) == s.seed
            && memory::safe_read<float>(w + SCHEMA("C_EconEntity", "m_flFallbackWear"_hash)).value_or(-1.f) == s.wear
            && memory::safe_read<int>(w + SCHEMA("C_EconEntity", "m_nFallbackStatTrak"_hash)).value_or(-2) == (s.stattrak ? s.stattrak_count : -1)
            && cosmetic_attributes::matches(iv, s) && name_tag::matches(iv, s.name_tag);
    }
    inline bool write(std::uintptr_t w, std::uintptr_t iv,
        const settings::changer::applied_skin& s, std::uint32_t account, int quality) {
        applied.erase(w); // A partial write must always be retried.
        if (!available() || !identity(w).ready()) return false;
        const bool written =
            memory::safe_write<std::uint64_t>(iv + SCHEMA("C_EconItemView", "m_iItemID"_hash), item_id)
            && memory::safe_write<std::uint32_t>(iv + SCHEMA("C_EconItemView", "m_iItemIDHigh"_hash), 0xf0000000u)
            && memory::safe_write<std::uint32_t>(iv + SCHEMA("C_EconItemView", "m_iItemIDLow"_hash), 0x10u)
            && memory::safe_write<std::uint32_t>(iv + SCHEMA("C_EconItemView", "m_iAccountID"_hash), account)
            && memory::safe_write<int>(iv + SCHEMA("C_EconItemView", "m_iEntityQuality"_hash), quality)
            && memory::safe_write<bool>(iv + SCHEMA("C_EconItemView", "m_bInitialized"_hash), true)
            && memory::safe_write<bool>(iv + SCHEMA("C_EconItemView", "m_bDisallowSOC"_hash), true)
            && memory::safe_write<int>(w + SCHEMA("C_EconEntity", "m_nFallbackPaintKit"_hash), s.paint_kit_id)
            && memory::safe_write<int>(w + SCHEMA("C_EconEntity", "m_nFallbackSeed"_hash), s.seed)
            && memory::safe_write<float>(w + SCHEMA("C_EconEntity", "m_flFallbackWear"_hash), s.wear)
            && memory::safe_write<int>(w + SCHEMA("C_EconEntity", "m_nFallbackStatTrak"_hash), s.stattrak ? s.stattrak_count : -1);
        return written && cosmetic_attributes::apply(iv, s) && name_tag::apply(iv, s.name_tag);
    }
    inline void remember(std::uintptr_t w, const settings::changer::applied_skin& s) {
        const auto v = identity(w);
        if (v.ready()) applied[w] = {v, s};
    }
}
