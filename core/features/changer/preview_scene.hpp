#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <string_view>
#include <string>
#include <utility>
#include <vector>
#include <core/systems/systems.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/steam/steam.hpp>

namespace features::changer::preview_scene {
    inline constexpr std::uint64_t steam_base = 76561197960265728ull;
    struct player {
        std::uintptr_t pawn{};
        std::uint64_t steam_id{};
        int team{};
        std::vector<std::uintptr_t> weapons;
    };
    inline std::vector<int> slots;
    inline std::vector<player> players;
    inline std::chrono::steady_clock::time_point next_scan{};

    inline bool is_player(const char* name) {
        if (!name) return false;
        const std::string_view s{name};
        return s.find("PreviewPlayer") != s.npos || s.find("TeamPreviewModel") != s.npos
            || s.find("player_preview") != s.npos || s.find("preview_player") != s.npos
            || s == "csgo_previewplayer";
    }
    inline void reset() { players.clear(); slots.clear(); next_scan = {}; }
    inline int team(std::uintptr_t pawn) {
        const auto offset = SCHEMA("C_BaseEntity", "m_iTeamNum"_hash);
        const int value = offset ? memory::safe_read<std::uint8_t>(pawn + offset).value_or(0) : 0;
        if (value == 2 || value == 3) return value;
        const auto node_offset = SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash);
        const auto node = node_offset ? memory::safe_read<std::uintptr_t>(pawn + node_offset).value_or(0) : 0;
        const auto state = SCHEMA("CSkeletonInstance", "m_modelState"_hash);
        const auto model = SCHEMA("CModelState", "m_ModelName"_hash);
        const auto name = node && state && model
            ? memory::safe_read<std::uintptr_t>(node + state + model).value_or(0) : 0;
        const auto path = name ? memory::read_string(name) : std::string{};
        if (path.find("ctm_") != path.npos || path.find("counter") != path.npos) return 3;
        if (path.find("tm_") != path.npos || path.find("terrorist") != path.npos) return 2;
        return 0; // Never silently use CT settings for an unknown team.
    }
    inline std::uint64_t controller_id(std::uintptr_t pawn) {
        const auto sid_offset = SCHEMA("CBasePlayerController", "m_steamID"_hash);
        if (!sid_offset) return 0;
        for (const auto offset : {
            SCHEMA("C_BasePlayerPawn", "m_hController"_hash),
            SCHEMA("C_BasePlayerPawn", "m_hDefaultController"_hash),
            SCHEMA("C_CSPlayerPawnBase", "m_hOriginalController"_hash)}) {
            if (!offset) continue;
            const auto h = memory::safe_read<std::uint32_t>(pawn + offset).value_or(0);
            const auto ctrl = systems::g_entities.lookup(h);
            const auto name = ctrl ? systems::g_entities.get_schema_name(ctrl) : nullptr;
            if (!name || std::string_view{name}.find("PlayerController") == std::string_view::npos) continue;
            const auto sid = memory::safe_read<std::uint64_t>(ctrl + sid_offset).value_or(0);
            if (sid >= steam_base) return sid;
        }
        return 0;
    }
    inline std::uint64_t item_owner(std::uintptr_t item) {
        const auto offset = SCHEMA("C_EconItemView", "m_iAccountID"_hash);
        const auto id = item && offset ? memory::safe_read<std::uint32_t>(item + offset).value_or(0) : 0;
        return id ? steam_base + id : 0;
    }
    inline bool is_local(const player& p) {
        const auto sid = steam::user::get_steam_id();
        return sid >= steam_base && p.steam_id == sid;
    }
    inline bool is_weapon(const char* name) {
        if (!name) return false;
        const std::string_view s{name};
        return s.starts_with("C_Weapon") || s == "C_AK47" || s == "C_DEagle"
            || s == "C_Knife" || s == "C_CSWeaponBase" || s == "C_CSWeaponBaseGun"
            || s == "C_CSGO_PreviewWeapon";
    }
    inline void add_weapon(player& p, std::uintptr_t weapon) {
        if (weapon && is_weapon(systems::g_entities.get_schema_name(weapon)) &&
            std::find(p.weapons.begin(), p.weapons.end(), weapon) == p.weapons.end())
            p.weapons.push_back(weapon);
    }
    inline void collect_attached_weapons(player& p) {
        const auto scene_offset = SCHEMA("C_BaseEntity", "m_pGameSceneNode"_hash);
        const auto child_offset = SCHEMA("CGameSceneNode", "m_pChild"_hash);
        const auto sibling_offset = SCHEMA("CGameSceneNode", "m_pNextSibling"_hash);
        const auto parent_offset = SCHEMA("CGameSceneNode", "m_pParent"_hash);
        const auto owner_offset = SCHEMA("CGameSceneNode", "m_pOwner"_hash);
        if (!scene_offset || !child_offset || !sibling_offset || !parent_offset || !owner_offset) return;
        const auto root = memory::safe_read<std::uintptr_t>(p.pawn + scene_offset).value_or(0);
        if (!root) return;

        // Preview weapons need not be in WeaponServices. Follow only this pawn's
        // attachment tree, never unrelated inventory/inspect or party entities.
        std::vector<std::pair<std::uintptr_t, std::uintptr_t>> pending;
        std::vector<std::uintptr_t> visited{root};
        const auto first = memory::safe_read<std::uintptr_t>(root + child_offset).value_or(0);
        if (first) pending.emplace_back(first, root);
        for (std::size_t i = 0; i < pending.size() && i < 128; ++i) {
            const auto [node, parent] = pending[i];
            if (!node || std::find(visited.begin(), visited.end(), node) != visited.end()) continue;
            visited.push_back(node);
            if (memory::safe_read<std::uintptr_t>(node + parent_offset).value_or(0) != parent) continue;
            const auto sibling = memory::safe_read<std::uintptr_t>(node + sibling_offset).value_or(0);
            if (sibling) pending.emplace_back(sibling, parent);
            const auto owner = memory::safe_read<std::uintptr_t>(node + owner_offset).value_or(0);
            if (owner && owner != p.pawn && is_player(systems::g_entities.get_schema_name(owner))) continue;
            if (owner && memory::safe_read<std::uintptr_t>(owner + scene_offset).value_or(0) == node)
                add_weapon(p, owner);
            const auto child = memory::safe_read<std::uintptr_t>(node + child_offset).value_or(0);
            if (child) pending.emplace_back(child, node);
        }
    }
    // Called only after the original game-thread frame callback. The entity range
    // is the range supported by entities::get_by_index (32 chunks * 512 slots).
    inline void refresh() {
        players.clear();
        if (!addresses::globals::entity_list || !addresses::globals::schema_system) return;
        const auto now = std::chrono::steady_clock::now();
        if (now >= next_scan) {
            slots.clear();
            for (int i = 0; i < 0x4000; ++i) {
                const auto entity = systems::g_entities.get_by_index(i);
                if (entity && is_player(systems::g_entities.get_schema_name(entity))) slots.push_back(i);
            }
            next_scan = now + std::chrono::milliseconds(100);
        }
        const auto service_offset = SCHEMA("C_BasePlayerPawn", "m_pWeaponServices"_hash);
        const auto weapons_offset = SCHEMA("CPlayer_WeaponServices", "m_hMyWeapons"_hash);
        const auto active_offset = SCHEMA("CPlayer_WeaponServices", "m_hActiveWeapon"_hash);
        const auto manager_offset = SCHEMA("C_EconEntity", "m_AttributeManager"_hash);
        const auto item_offset = SCHEMA("C_AttributeContainer", "m_Item"_hash);
        for (const int index : slots) {
            const auto entity = systems::g_entities.get_by_index(index);
            if (!entity || !is_player(systems::g_entities.get_schema_name(entity))) continue;
            player p{};
            p.pawn = entity; p.team = team(entity); p.steam_id = controller_id(entity);
            const auto services = service_offset
                ? memory::safe_read<std::uintptr_t>(entity + service_offset).value_or(0) : 0;
            const auto add_handle = [&](std::uint32_t h) {
                add_weapon(p, systems::g_entities.lookup(h));
            };
            if (services && weapons_offset) {
                const auto base = services + weapons_offset;
                const auto count = memory::safe_read<int>(base).value_or(0);
                const auto data = memory::safe_read<std::uintptr_t>(base + 8).value_or(0);
                if (data && count > 0 && count <= 64)
                    for (int i = 0; i < count; ++i)
                        add_handle(memory::safe_read<std::uint32_t>(data + i * sizeof(std::uint32_t)).value_or(0));
            }
            if (services && active_offset)
                add_handle(memory::safe_read<std::uint32_t>(services + active_offset).value_or(0));
            // Discover attachments before resolving identity: their economy items
            // also identify controller-less lobby agents in a party.
            collect_attached_weapons(p);
            if (!p.steam_id && manager_offset && item_offset) {
                // All available economy items must agree. Conflicting identities
                // are not enough evidence to apply another player's loadout.
                std::uint64_t candidate{};
                bool conflict{};
                for (const auto w : p.weapons) {
                    const auto owner = item_owner(w + manager_offset + item_offset);
                    if (owner && candidate && candidate != owner) conflict = true;
                    if (owner) candidate = owner;
                }
                const auto gloves = SCHEMA("C_CSPlayerPawn", "m_EconGloves"_hash);
                const auto owner = gloves ? item_owner(entity + gloves) : 0;
                if (owner && candidate && candidate != owner) conflict = true;
                if (owner) candidate = owner;
                if (!conflict) p.steam_id = candidate;
            }
            players.push_back(std::move(p));
        }
        // No guess for anonymous party members or in-match team previews.
        if (players.size() == 1 && !players.front().steam_id &&
            !memory::safe_read<std::uintptr_t>(addresses::globals::local_player_controller).value_or(0))
            players.front().steam_id = steam::user::get_steam_id();
    }
}
