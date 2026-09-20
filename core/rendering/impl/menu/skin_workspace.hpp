#pragma once

#include <core/settings.hpp>
#include <core/features/changer/changer.hpp>
#include <core/hooks/hooks.hpp>
#include "../../theme.hpp"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <cctype>
#include <stdexcept>
#include <array>
#include <optional>
#include <memory>

namespace rendering::skin_workspace {
using econ_type = features::changer::econ_item_system;
inline int team = 3;
inline bool dialog_busy = false;
inline int pending_delete_agent = -1;
inline std::array<std::int16_t, 2> focused_weapon{};
struct hover_state {
    std::optional<std::int16_t> weapon, agent;
    std::optional<int> paint, music;
    int custom = -2;
};
inline hover_state hover;


inline int active_browsing_weapon = 0;
inline bool hovered(const xui::rect& r) {
    const auto* win = xui::layout::current_window();
    const auto& c = xui::ctx();
    return !c.overlay_blocking() && c.input.in_rect(r) && (!win || c.input.in_rect(win->bounds));
}

// Wider left panel: ~360px on default 820px body
inline float left_width(float width) { return std::clamp(width * 0.44f, 330.0f, 390.0f); }

inline std::string lower(std::string value) {
    for (auto& c : value) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return value;
}

inline constexpr std::array<xdraw::color, 8> rarity_colors{{
    { 235, 235, 235, 255 },
    { 138, 173, 233, 255 },
    {  77, 116, 196, 255 },
    { 138,  86, 207, 255 },
    { 211,  44, 230, 255 },
    { 235,  75,  75, 255 },
    { 228, 174,  57, 255 },
    { 255, 215,   0, 255 }
}};

inline constexpr std::array<const char*, 8> rarity_names{{
    "Default",
    "Consumer Grade",
    "Industrial Grade",
    "Mil-Spec Grade",
    "Restricted",
    "Classified",
    "Covert",
    "Contraband"
}};

// Index immutable schema data once; filtered lists are rebuilt only when query or parameters change.
class catalog {
    std::array<std::uintptr_t, 10> stamp_{};
    std::array<std::vector<const econ_type::item_def*>, 3> weapons_;
    std::unordered_map<std::int16_t, std::vector<const econ_type::paint_kit*>> paints_;
    std::string paint_query_, music_query_, agent_query_;
    int paint_def_ = -1;
    int agent_team_ = -1;
    bool music_valid_ = false;
    std::vector<const econ_type::paint_kit*> filtered_paints_;
    std::vector<const econ_type::music_kit*> filtered_music_;
    std::vector<const econ_type::item_def*> filtered_agents_;
public:
    void refresh() {
        auto& e = features::changer::g_econ_item_system;
        const bool rebuilt = e.poll_schema();
        const std::array<std::uintptr_t, 10> stamp{
            reinterpret_cast<std::uintptr_t>(e.skins().data()), e.skins().size(),
            reinterpret_cast<std::uintptr_t>(e.item_defs().data()), e.item_defs().size(),
            reinterpret_cast<std::uintptr_t>(e.paint_kits().data()), e.paint_kits().size(),
            reinterpret_cast<std::uintptr_t>(e.agents().data()), e.agents().size(),
            reinterpret_cast<std::uintptr_t>(e.gloves().data()), e.gloves().size()};
        if (!rebuilt && stamp == stamp_) return;
        stamp_ = stamp;
        paints_.clear();
        for (auto& list : weapons_) list.clear();
        std::unordered_map<std::int16_t, std::unordered_set<int>> seen;
        for (const auto& s : e.skins()) {
            if (!seen[s.def_index].insert(s.paint_kit_id).second) continue;
            if (const auto* p = e.find_paint_kit(s.paint_kit_id)) paints_[s.def_index].push_back(p);
        }
        for (auto& [def, list] : paints_) {
            std::sort(list.begin(), list.end(), [](const auto* a, const auto* b) {
                return a->localized_name == b->localized_name ? a->id < b->id : a->localized_name < b->localized_name;
            });
        }
        const std::array<const std::vector<const econ_type::item_def*>*, 3> sources{&e.guns(), &e.knives(), &e.gloves()};
        for (std::size_t i = 0; i < sources.size(); ++i) {
            for (const auto* w : *sources[i]) {
                if (i == 2) {
                    // Gloves: always list all available glove types
                    weapons_[i].push_back(w);
                } else if ((i == 0 || paints_.contains(w->def_index)) && (!w->image_inventory.empty() || paints_.contains(w->def_index))) {
                    weapons_[i].push_back(w);
                }
            }
        }
        paint_def_ = -1; agent_team_ = -1; music_valid_ = false;
        filtered_paints_.clear(); filtered_music_.clear(); filtered_agents_.clear();
    }
    const auto& weapons(int category) const { return weapons_[std::clamp(category, 0, 2)]; }
    const auto& paints(int def, const std::string& query) {
        if (paint_def_ != def || paint_query_ != query) {
            paint_def_ = def; paint_query_ = query; filtered_paints_.clear();
            const auto q = lower(query);
            auto& econ = features::changer::g_econ_item_system;
            const auto* d = econ.find_def(static_cast<std::int16_t>(def));

            if (const auto it = paints_.find(static_cast<std::int16_t>(def)); it != paints_.end() && !it->second.empty()) {
                for (const auto* p : it->second) {
                    if (q.empty() || lower(p->localized_name + " " + p->name).find(q) != std::string::npos)
                        filtered_paints_.push_back(p);
                }
            } else if (d && d->category == econ_type::item_category::glove) {
                // Fallback for gloves if VPK index didn't map specific def to paint kits:
                // Include all paint kits that belong to gloves (finishes)
                for (const auto& pk : econ.paint_kits()) {
                    if (pk.id >= 10000 || pk.name.find("glove") != std::string::npos ||
                        pk.name.find("slick") != std::string::npos || pk.name.find("sporty") != std::string::npos ||
                        pk.name.find("specialist") != std::string::npos || pk.name.find("motorcycle") != std::string::npos ||
                        pk.name.find("handwrap") != std::string::npos || pk.name.find("bloodhound") != std::string::npos ||
                        pk.name.find("hydra") != std::string::npos || pk.name.find("brokenfang") != std::string::npos) {
                        if (q.empty() || lower(pk.localized_name + " " + pk.name).find(q) != std::string::npos)
                            filtered_paints_.push_back(&pk);
                    }
                }
            }

            // Prepend vanilla (default) skin at the top of the list
            static const econ_type::paint_kit s_vanilla_kit{ .id = 0, .name = "vanilla", .localized_name = "Default (Vanilla)" };
            if (q.empty() || lower(s_vanilla_kit.localized_name).find(q) != std::string::npos ||
                std::string("vanilla").find(q) != std::string::npos ||
                std::string("default").find(q) != std::string::npos) {
                filtered_paints_.insert(filtered_paints_.begin(), &s_vanilla_kit);
            }
        }
        return filtered_paints_;
    }
    const auto& music(const std::string& query) {
        if (!music_valid_ || music_query_ != query) {
            music_valid_ = true; music_query_ = query; filtered_music_.clear();
            const auto q = lower(query);
            for (const auto& m : features::changer::g_econ_item_system.music_kits())
                if (q.empty() || lower(m.localized_name + " " + m.localized_desc + " " + m.name).find(q) != std::string::npos)
                    filtered_music_.push_back(&m);
        }
        return filtered_music_;
    }
    const auto& agents(int team_filter, const std::string& query) {
        if (agent_team_ != team_filter || agent_query_ != query) {
            agent_team_ = team_filter; agent_query_ = query; filtered_agents_.clear();
            const auto q = lower(query);
            for (const auto* a : features::changer::g_econ_item_system.agents()) {
                const auto at = a->team();
                if (at != 0 && team_filter != 0 && at != team_filter) continue;
                if (q.empty() || lower(a->localized_name + " " + a->name).find(q) != std::string::npos)
                    filtered_agents_.push_back(a);
            }
        }
        return filtered_agents_;
    }
};
inline catalog items;

struct cosmetics {
    settings::changer::skin_map_field skins;
    settings::changer::agent_selection_field agents;
    settings::changer::custom_agents_field custom;
    settings::changer::music_field music;
    static cosmetics capture() {
        const auto& c = settings::g_changer;
        return {c.skins, c.agents, c.custom_agents, c.music};
    }
    nlohmann::json encode() const {
        return {{"skins", skins.serialize()}, {"agents", agents.serialize()},
                {"custom_agents", custom.serialize()}, {"music", music.serialize()}};
    }
    static cosmetics decode(const nlohmann::json& j) {
        if (!j.is_object() || !j.contains("skins") || !j.at("skins").is_object())
            throw std::runtime_error("Invalid cosmetic profile");
        cosmetics c;
        c.skins.deserialize(j.at("skins"));
        if (j.contains("agents")) c.agents.deserialize(j.at("agents"));
        if (j.contains("custom_agents")) c.custom.deserialize(j.at("custom_agents"));
        if (j.contains("music")) c.music.deserialize(j.at("music"));
        const auto validate_custom = [&](int& index, int side) {
            if (index < 0 || index >= static_cast<int>(c.custom.entries.size()) ||
                (c.custom.entries[index].team != 0 && c.custom.entries[index].team != side)) index = -1;
        };
        validate_custom(c.custom.selected_ct, 3); validate_custom(c.custom.selected_t, 2);
        if (c.music.id < 0 || c.music.id > 65535) c.music.id = 0;
        return c;
    }
    void apply() const {
        auto& c = settings::g_changer;
        c.skins = skins; c.agents = agents; c.custom_agents = custom; c.music = music;
        // Keep captured originals: changers need them to restore items removed by this profile.
        features::changer::g_guns.invalidate();
        features::changer::g_knives.invalidate();
        features::changer::g_skin_sync.trigger_push();
        hooks::cheat::trigger_lobby_music(static_cast<std::uint16_t>(c.music.id));
        hover = {}; focused_weapon = {};
    }
};

struct profile { std::string name; cosmetics values; };

class profile_store {
    bool initialized_ = false, writable_ = false;
    int pending_load_ = -1;
    nlohmann::json pending_values_, baseline_;
    std::filesystem::path path_;

    void clear_pending() { pending_load_ = -1; pending_values_ = {}; }
    bool persist(const std::vector<profile>& next, int active) {
        if (!writable_) { status = "Storage unavailable: press Retry"; return false; }
        auto temp = path_; temp += L".tmp";
        try {
            auto list = nlohmann::json::array();
            for (const auto& p : next) list.push_back({{"name", p.name}, {"values", p.values.encode()}});
            // Additional metadata is backwards compatible with the version-1 reader.
            const auto bytes = nlohmann::json{{"version", 1}, {"selected", active}, {"profiles", list}}.dump(2);
            if (bytes.size() > 8 * 1024 * 1024) throw std::runtime_error("Profiles exceed 8 MiB");
            std::filesystem::create_directories(path_.parent_path());
            {
                std::ofstream file;
                file.exceptions(std::ios::failbit | std::ios::badbit);
                file.open(temp, std::ios::binary | std::ios::trunc);
                file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
                file.flush(); file.close();
            }
            if (std::filesystem::exists(path_)) {
                auto backup = path_; backup += L".bak";
                if (!CopyFileW(path_.c_str(), backup.c_str(), FALSE))
                    throw std::runtime_error("Cannot back up cosmetic profiles (error " + std::to_string(GetLastError()) + ")");
            }
            if (!MoveFileExW(temp.c_str(), path_.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
                throw std::runtime_error("Cannot replace cosmetic profiles (error " + std::to_string(GetLastError()) + ")");
            status = "Saved locally";
            return true;
        } catch (const std::exception& e) {
            std::error_code ignored; std::filesystem::remove(temp, ignored);
            status = e.what(); return false;
        }
    }
public:
    std::vector<profile> entries;
    int selected = -1;
    std::string status;
    bool ready() const { return writable_; }
    void load(bool retry = false) {
        if (initialized_ && !retry) return;
        initialized_ = true;
        try {
            wchar_t base[32768]{};
            const auto size = GetEnvironmentVariableW(L"LOCALAPPDATA", base, 32768);
            if (!size || size >= 32768) throw std::runtime_error("LOCALAPPDATA unavailable");
            path_ = std::filesystem::path(base) / L"nemesis" / L"skins" / L"profiles.json";
            std::vector<profile> next;
            int active = -1;
            if (std::filesystem::exists(path_)) {
                if (std::filesystem::file_size(path_) > 8 * 1024 * 1024) throw std::runtime_error("Profiles exceed 8 MiB");
                std::ifstream file(path_, std::ios::binary);
                if (!file) throw std::runtime_error("Cannot read cosmetic profiles");
                const auto j = nlohmann::json::parse(file);
                if (j.at("version") != 1 || !j.at("profiles").is_array() || j.at("profiles").size() > 5)
                    throw std::runtime_error("Unsupported cosmetic profiles file");
                for (const auto& p : j.at("profiles")) {
                    auto name = p.at("name").get<std::string>();
                    if (name.empty() || name.size() > 64) throw std::runtime_error("Invalid profile name");
                    next.push_back({std::move(name), cosmetics::decode(p.at("values"))});
                }
                active = j.value("selected", -1);
            }
            const auto current = cosmetics::capture().encode();
            entries = std::move(next);
            selected = active >= 0 && active < static_cast<int>(entries.size()) ? active : -1;
            // Opening the menu must not silently overwrite the current loadout.
            baseline_ = selected >= 0 ? entries[selected].values.encode() : current;
            writable_ = true; clear_pending();
            status = selected >= 0 ? "Ready: Save updates selected config" : "Save creates a new config";
        } catch (const std::exception& e) {
            writable_ = false; status = e.what();
        }
    }
    bool create() {
        if (!writable_) { status = "Storage unavailable: press Retry"; return false; }
        if (entries.size() >= 5) { status = "5 configs: select one to overwrite or delete one"; return false; }
        int suffix = 1;
        std::string name;
        do { name = "Config " + std::to_string(suffix++); }
        while (std::any_of(entries.begin(), entries.end(), [&](const profile& p) { return lower(p.name) == lower(name); }));
        auto next = entries;
        next.push_back({name, cosmetics::capture()});
        const int active = static_cast<int>(next.size()) - 1;
        if (!persist(next, active)) return false;
        entries = std::move(next); selected = active;
        baseline_ = entries[selected].values.encode(); clear_pending();
        return true;
    }
    void save() {
        // A fresh install (or deleting the active profile) is a valid Save As,
        // not a permanently disabled button and not an implicit overwrite.
        if (selected < 0 || selected >= static_cast<int>(entries.size())) { create(); return; }
        auto next = entries; next[selected].values = cosmetics::capture();
        if (persist(next, selected)) {
            entries = std::move(next); baseline_ = entries[selected].values.encode(); clear_pending();
        }
    }
    bool select(int index) {
        if (dialog_busy || index < 0 || index >= static_cast<int>(entries.size())) return false;
        const auto current = cosmetics::capture().encode();
        const bool dirty = !baseline_.is_null() && current != baseline_;
        if (dirty && (pending_load_ != index || pending_values_ != current)) {
            pending_load_ = index; pending_values_ = current;
            status = "Unsaved: select again to discard";
            return false;
        }
        // Write selection metadata before applying; on an I/O error neither
        // the current loadout nor the selected profile changes.
        if (!persist(entries, index)) return false;
        entries[index].values.apply(); selected = index; status = "Loaded";
        baseline_ = entries[index].values.encode(); clear_pending();
        return true;
    }
    bool rename(std::string name) {
        if (selected < 0 || selected >= static_cast<int>(entries.size()) || !writable_) return false;
        const auto first = name.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) { status = "Name cannot be empty"; return false; }
        name = name.substr(first, name.find_last_not_of(" \t\r\n") - first + 1);
        if (name.size() > 64 || std::any_of(name.begin(), name.end(), [](unsigned char c) { return c < 32; })) {
            status = "Invalid profile name"; return false;
        }
        for (int i = 0; i < static_cast<int>(entries.size()); ++i)
            if (i != selected && lower(entries[i].name) == lower(name)) { status = "Name already exists"; return false; }
        auto next = entries; next[selected].name = std::move(name);
        if (!persist(next, selected)) return false;
        entries = std::move(next); clear_pending(); return true;
    }
    void remove(int index) {
        if (index < 0 || index >= static_cast<int>(entries.size())) return;
        auto next = entries; next.erase(next.begin() + index);
        const int active = selected == index ? -1 : selected - (selected > index ? 1 : 0);
        if (!persist(next, active)) return;
        entries = std::move(next); selected = active; clear_pending();
        if (selected < 0) { baseline_ = cosmetics::capture().encode(); status = "Deleted: Save creates a new config"; }
    }
};
inline profile_store profiles;

inline constexpr std::uintptr_t profile_popup_id = 0x534b494e43464735ull;

class profile_popup final : public xui::overlay {
    int delete_row_ = -1;
    xui::rect bounds() const {
        const auto [sw, sh] = xdraw::viewport_size();
        const float w = std::min(280.0f, static_cast<float>(sw));
        const float h = 50.0f + 36.0f * static_cast<float>(profiles.entries.size() + 1);
        return {std::clamp(m_anchor.x, 0.0f, std::max(0.0f, sw - w)),
                std::clamp(m_anchor.bottom() + 6.0f, 0.0f, std::max(0.0f, sh - h)), w, h};
    }
public:
    explicit profile_popup(const xui::rect& anchor) : overlay(profile_popup_id, anchor) {}
    bool hit_test(float x, float y) const override { return bounds().contains(x, y); }
    bool process_input(const xui::input_state& input) override {
        if (m_closing || m_closed) { m_closed = true; return false; }
        const auto r = bounds();
        for (const auto key : input.key_presses()) if (key == VK_ESCAPE) { m_closed = true; return true; }
        if ((input.mouse_clicked || input.rmb_clicked) && !input.in_rect(r)) { m_closed = true; return true; }
        if (!input.in_rect(r)) return false;
        for (int i = 0; i < static_cast<int>(profiles.entries.size()); ++i) {
            const xui::rect row{r.x + 6.0f, r.y + 6.0f + i * 36.0f, r.w - 12.0f, 32.0f};
            if (!input.in_rect(row)) continue;
            if (input.rmb_clicked) delete_row_ = delete_row_ == i ? -1 : i;
            if (input.mouse_clicked) {
                if (delete_row_ == i && input.mouse_x >= row.right() - 72.0f) {
                    profiles.remove(i); delete_row_ = -1;
                } else if (!dialog_busy) { m_closed = profiles.select(i); }
                return true;
            }
        }
        const xui::rect create{r.x + 6.0f, r.y + 6.0f + profiles.entries.size() * 36.0f, r.w - 12.0f, 32.0f};
        if (input.mouse_clicked && input.in_rect(create) && profiles.entries.size() < 5) {
            m_closed = profiles.create();
        }
        return true;
    }
    void render(const xui::style& style, const xui::input_state& input) override {
        if (m_closing || m_closed) { m_closed = true; return; }
        const auto r = bounds();
        auto& dl = xdraw::get(xdraw::layer::top);
        dl.rect_filled(r.x, r.y, r.w, r.h, style.popup_bg, xdraw::corner_radius{8.0f});
        dl.rect(r.x, r.y, r.w, r.h, style.popup_border, xdraw::corner_radius{8.0f});

        for (int i = 0; i < static_cast<int>(profiles.entries.size()); ++i) {
            const xui::rect row{r.x + 6.0f, r.y + 6.0f + i * 36.0f, r.w - 12.0f, 32.0f};
            const bool is_sel = (i == profiles.selected);
            const bool row_hover = input.in_rect(row);
            dl.rect_filled(row.x, row.y, row.w, row.h,
                is_sel ? tokens::col_accent.alpha(45) : (row_hover ? tokens::col_elevated : tokens::col_card),
                xdraw::corner_radius{5.0f});
            if (is_sel) {
                dl.rect(row.x, row.y, row.w, row.h, tokens::col_accent.alpha(110), xdraw::corner_radius{5.0f});
            }
            dl.text(row.x + 10.0f, row.y + 8.0f, theme::fit_text(profiles.entries[i].name, row.w - (delete_row_ == i ? 88.0f : 20.0f)), style.text);

            if (delete_row_ == i) {
                const xui::rect del{row.right() - 70.0f, row.y + 2.0f, 68.0f, row.h - 4.0f};
                const bool hot = input.in_rect(del);
                dl.rect_filled(del.x, del.y, del.w, del.h,
                    hot ? xdraw::color{235, 60, 75} : xdraw::color{180, 45, 55, 160},
                    xdraw::corner_radius{4.0f});
                const auto [dw, dh] = xdraw::measure_text("Delete");
                dl.text(del.x + (del.w - dw) * 0.5f, del.y + (del.h - dh) * 0.5f, "Delete",
                    hot ? xdraw::color{255, 255, 255} : xdraw::color{250, 210, 215});
            }
        }

        const float y = r.y + 6.0f + profiles.entries.size() * 36.0f;
        const xui::rect create{r.x + 6.0f, y, r.w - 12.0f, 32.0f};
        const bool can_create = profiles.entries.size() < 5;
        const bool create_hover = input.in_rect(create) && can_create;
        if (create_hover) {
            dl.rect_filled(create.x, create.y, create.w, create.h, tokens::col_elevated, xdraw::corner_radius{5.0f});
        }
        dl.text(create.x + 10.0f, create.y + 8.0f, can_create ? "+ Create config" : "5 / 5 configs",
            can_create ? tokens::col_accent : tokens::col_text_dim);
    }
};

inline void panel(const xui::rect& r) {
    auto& dl = xui::draw::current();
    dl.rect_filled(r.x, r.y, r.w, r.h, tokens::col_card, xdraw::corner_radius{12.0f});
    dl.rect(r.x, r.y, r.w, r.h, tokens::col_border.alpha(140), xdraw::corner_radius{12.0f});
}

inline bool button(const xui::rect& r, const char* label, bool selected = false, bool enabled = true) {
    auto& dl = xui::draw::current();
    const bool hot = enabled && hovered(r);
    const auto fade = xui::anim::lerp(xui::fnv1a(label), hot || selected ? 1.0f : 0.0f, 14.0f);
    dl.rect_filled(r.x, r.y, r.w, r.h, xui::lerp(tokens::col_elevated, tokens::col_accent.alpha(65), fade), xdraw::corner_radius{7.0f});
    if (selected) {
        dl.rect(r.x, r.y, r.w, r.h, tokens::col_accent, xdraw::corner_radius{7.0f}, 1.0f);
    }
    const auto [w, h] = xdraw::measure_text(label);
    dl.text(r.x + (r.w - w) * 0.5f, r.y + (r.h - h) * 0.5f, label, enabled ? (selected ? tokens::col_accent : tokens::col_text) : tokens::col_text_dim.alpha(90));
    return hot && xui::ctx().input.mouse_clicked;
}

inline bool image(const xui::rect& r, const econ_type::skin_image* img) {
    if (!img || !img->srv || img->width <= 0 || img->height <= 0 || r.w <= 0 || r.h <= 0) return false;
    const float scale = std::min(r.w / img->width, r.h / img->height);
    const float w = img->width * scale, h = img->height * scale;
    xui::draw::current().image(r.x + (r.w - w) * 0.5f, r.y + (r.h - h) * 0.5f, w, h, img->srv.Get(), {255, 255, 255});
    return true;
}

inline void team_icon(float x, float y, int side) {
    auto& dl = xui::draw::current();
    if (side == 3) {
        const auto col = xdraw::color{100, 175, 255};
        const std::array<float, 10> shield{
            x - 7.0f, y - 8.0f,
            x + 7.0f, y - 8.0f,
            x + 6.0f, y + 3.0f,
            x,        y + 9.0f,
            x - 6.0f, y + 3.0f
        };
        dl.polyline(shield, col, true, 1.8f);
        dl.circle_filled(x, y - 1.0f, 2.0f, col);
    } else {
        const auto col = xdraw::color{240, 190, 75};
        dl.line(x - 6.0f, y - 6.0f, x + 6.0f, y + 6.0f, col, 2.0f);
        dl.line(x + 6.0f, y - 6.0f, x - 6.0f, y + 6.0f, col, 2.0f);
        dl.circle(x, y, 7.5f, col.alpha(160), 1.2f);
    }
}

// Live operative + weapon buy-menu preview and cosmetic sidebar controls
inline bool sidebar(const xui::rect& r, int category) {
    auto& dl = xui::draw::current();
    auto& econ = features::changer::g_econ_item_system;

    const xui::rect main{r.x, r.y, r.w, r.h - 88.0f};
    panel(main);

    // 1. Top Section: Cosmetic Config dropdown & save button
    dl.text(r.x + 14.0f, r.y + 13.0f, theme::fit_text("CONFIG: " + profiles.status, r.w - 28.0f), tokens::col_text_dim);
    const xui::rect selector{r.x + 14.0f, r.y + 32.0f, r.w - 86.0f, 32.0f};
    const auto label = (profiles.selected >= 0 && profiles.selected < static_cast<int>(profiles.entries.size())) ? profiles.entries[profiles.selected].name : "Choose config";
    if (button(selector, theme::fit_text(label, selector.w - 18.0f).c_str(), false, profiles.ready() && !dialog_busy)) {
        if (xui::overlays::is_open(profile_popup_id)) {
            xui::overlays::close(profile_popup_id);
        } else {
            xui::overlays::add(std::make_unique<profile_popup>(selector));
        }
    }
    if (xui::overlays::is_open(profile_popup_id)) {
        xui::overlays::touch(profile_popup_id);
    }

    if (button({selector.right() + 6.0f, selector.y, 52.0f, 32.0f}, profiles.ready() ? "Save" : "Retry", false,
        !dialog_busy)) {
        if (profiles.ready()) profiles.save(); else profiles.load(true);
    }

    static int editing_profile = -1;
    static std::string profile_name;
    if (editing_profile != profiles.selected) {
        editing_profile = profiles.selected;
        profile_name = editing_profile >= 0 ? profiles.entries[editing_profile].name : "";
    }
    if (profiles.selected >= 0 && !dialog_busy) {
        const auto* parent = xui::layout::current_window();
        if (parent) {
            xui::layout::set_cursor(r.x + 14.0f - parent->bounds.x, selector.bottom() + 6.0f - parent->bounds.y);
            if (xui::begin_child("##cosmetic_profile_name", r.w - 108.0f, 34.0f, false, false)) {
                xui::text_input("##profile_name", profile_name, 64, "Profile name");
                xui::end_child();
            }
        }
        if (button({r.right() - 86.0f, selector.bottom() + 6.0f, 72.0f, 30.0f}, "Rename"))
            profiles.rename(profile_name);
    }

    // 2. Bottom Section of main panel: CT/T Loadouts in a single row with animated width & transitions
    constexpr float btn_h = 32.0f;
    const float loadout_top = main.bottom() - 12.0f - btn_h;
    const float row_w = r.w - 28.0f;
    constexpr float gap = 8.0f;
    constexpr float compact_w = 42.0f;
    const float expanded_w = (row_w - gap) - compact_w;

    struct loadout_anim_t {
        float ct_weight = 1.0f;
    };
    static loadout_anim_t s_anim{};

    const float target_weight = (team == 3 ? 1.0f : 0.0f);
    const float dt = xdraw::delta_time();
    s_anim.ct_weight += (target_weight - s_anim.ct_weight) * (1.0f - std::exp(-dt * 18.0f));
    s_anim.ct_weight = std::clamp(s_anim.ct_weight, 0.0f, 1.0f);

    const float ct_factor = s_anim.ct_weight;
    const float t_factor = 1.0f - ct_factor;

    const float ct_w = std::lerp(compact_w, expanded_w, ct_factor);
    const float t_w = (row_w - gap) - ct_w;

    const xui::rect ct_btn{r.x + 14.0f, loadout_top, ct_w, btn_h};
    const xui::rect t_btn{ct_btn.right() + gap, loadout_top, t_w, btn_h};

    // CT Button (Icon + text when active, compact icon-only when inactive)
    const bool ct_hover = hovered(ct_btn);
    if (ct_hover && xui::ctx().input.mouse_clicked) {
        team = 3;
    }
    const auto ct_accent = xdraw::color{75, 155, 255};
    const auto ct_bg = (team == 3) ? tokens::col_elevated : (ct_hover ? tokens::col_card : tokens::col_dark);
    const auto ct_border = (team == 3) ? ct_accent.alpha(static_cast<uint8_t>(180 * ct_factor)) : (ct_hover ? tokens::col_border.alpha(160) : tokens::col_border);
    dl.rect_filled(ct_btn.x, ct_btn.y, ct_btn.w, ct_btn.h, ct_bg, xdraw::corner_radius{6.0f});
    dl.rect(ct_btn.x, ct_btn.y, ct_btn.w, ct_btn.h, ct_border, xdraw::corner_radius{6.0f});

    const float ct_icon_x = std::lerp(ct_btn.center_x(), ct_btn.x + 18.0f, ct_factor);
    team_icon(ct_icon_x, ct_btn.center_y(), 3);

    if (ct_factor > 0.15f) {
        dl.push_clip(ct_btn.x, ct_btn.y, ct_btn.w, ct_btn.h);
        const uint8_t alpha = static_cast<uint8_t>(std::clamp((ct_factor - 0.15f) / 0.85f, 0.0f, 1.0f) * 255.0f);
        dl.text(ct_btn.x + 36.0f, ct_btn.center_y() - 7.0f, "CT Loadout", (team == 3 ? tokens::col_text : tokens::col_text_dim).alpha(alpha));
        dl.pop_clip();
    }

    // T Button (Icon + text when active, compact icon-only when inactive)
    const bool t_hover = hovered(t_btn);
    if (t_hover && xui::ctx().input.mouse_clicked) {
        team = 2;
    }
    const auto t_accent = xdraw::color{240, 180, 65};
    const auto t_bg = (team == 2) ? tokens::col_elevated : (t_hover ? tokens::col_card : tokens::col_dark);
    const auto t_border = (team == 2) ? t_accent.alpha(static_cast<uint8_t>(180 * t_factor)) : (t_hover ? tokens::col_border.alpha(160) : tokens::col_border);
    dl.rect_filled(t_btn.x, t_btn.y, t_btn.w, t_btn.h, t_bg, xdraw::corner_radius{6.0f});
    dl.rect(t_btn.x, t_btn.y, t_btn.w, t_btn.h, t_border, xdraw::corner_radius{6.0f});

    const float t_icon_x = std::lerp(t_btn.center_x(), t_btn.x + 18.0f, t_factor);
    team_icon(t_icon_x, t_btn.center_y(), 2);

    if (t_factor > 0.15f) {
        dl.push_clip(t_btn.x, t_btn.y, t_btn.w, t_btn.h);
        const uint8_t alpha = static_cast<uint8_t>(std::clamp((t_factor - 0.15f) / 0.85f, 0.0f, 1.0f) * 255.0f);
        dl.text(t_btn.x + 36.0f, t_btn.center_y() - 7.0f, "T Loadout", (team == 2 ? tokens::col_text : tokens::col_text_dim).alpha(alpha));
        dl.pop_clip();
    }

    // 2D Skin / Weapon / Item Preview
    const float prev_y = selector.bottom() + (profiles.selected >= 0 ? 48.0f : 10.0f);
    const float prev_h = std::max(80.0f, loadout_top - 10.0f - prev_y);
    const xui::rect preview{r.x + 14.0f, prev_y, r.w - 28.0f, prev_h};
    const xui::rect canvas{preview.x + 8.0f, preview.y + 28.0f,
        std::max(1.0f, preview.w - 16.0f), std::max(1.0f, preview.h - 76.0f)};

    std::string title;
    const econ_type::skin_image* artwork = nullptr;
    const econ_type::item_def* weapon = nullptr;
    int current_paint_kit = 0;
    settings::changer::applied_skin selected_skin{};
    const auto& skins = settings::g_changer.skins.for_team(team);

    int active_category = category;
    if (hover.music) active_category = 4;
    else if (hover.agent) active_category = 3;
    else if (hover.weapon) {
        if (const auto* d = econ.find_def(*hover.weapon))
            active_category = d->category == econ_type::item_category::glove ? 2 :
                d->category == econ_type::item_category::knife ? 1 : 0;
    }
    if (active_category == 4) {
        const int music_id = hover.music.value_or(settings::g_changer.music.id);
        const auto* kit = econ.find_music_kit(music_id);
        title = kit ? kit->localized_name : "Default Music Kit";
        if (kit) artwork = econ.get_skin_image(kit->image_inventory);
    } else if (active_category == 3) {
        const auto& ca = settings::g_changer.custom_agents;
        const int custom = hover.custom != -2 ? hover.custom : (team == 3 ? ca.selected_ct : ca.selected_t);
        const int agent_def = hover.agent.value_or(team == 3 ? settings::g_changer.agents.ct_def : settings::g_changer.agents.t_def);
        if (custom >= 0 && custom < static_cast<int>(ca.entries.size())) {
            title = ca.entries[custom].name;
        } else if (const auto* agent = econ.find_def(static_cast<std::int16_t>(agent_def))) {
            title = agent->localized_name;
            artwork = econ.get_skin_image(agent->image_inventory);
        } else {
            title = team == 3 ? "Default CT Agent" : "Default T Agent";
        }
    } else {
        const auto desired_category = active_category == 2 ? econ_type::item_category::glove :
            active_category == 1 ? econ_type::item_category::knife : econ_type::item_category::gun;
        const int focused = focused_weapon[team == 3 ? 0 : 1];
        int def = hover.weapon.value_or(static_cast<std::int16_t>(active_browsing_weapon ? active_browsing_weapon : focused));
        weapon = econ.find_def(static_cast<std::int16_t>(def));
        if (!weapon || weapon->category != desired_category) {
            weapon = nullptr;
            for (const auto& [id, options] : skins) {
                const auto* d = econ.find_def(id);
                if (d && d->category == desired_category && (d->team() == 0 || d->team() == team) &&
                    (!weapon || d->def_index < weapon->def_index)) weapon = d;
            }
            if (!weapon) {
                for (const auto* d : items.weapons(std::clamp(active_category, 0, 2))) {
                    if (d && (d->team() == 0 || d->team() == team)) { weapon = d; break; }
                }
            }
        }
        if (weapon) {
            if (const auto it = skins.find(weapon->def_index); it != skins.end()) selected_skin = it->second;
            selected_skin.paint_kit_id = hover.paint.value_or(selected_skin.paint_kit_id);
            current_paint_kit = std::max(0, selected_skin.paint_kit_id);
            const auto* paint = econ.find_paint_kit(current_paint_kit);
            title = weapon->localized_name + (paint ? " | " + paint->localized_name : "");
            artwork = econ.get_skin_image(weapon->def_index, current_paint_kit);
            if (!artwork) artwork = econ.get_skin_image(weapon->image_inventory);
        } else title = "No item selected";
    }

    dl.push_clip(preview.x, preview.y, preview.w, preview.h);
    dl.rect_filled(preview.x, preview.y, preview.w, preview.h,
        tokens::col_dark.alpha(175), xdraw::corner_radius{8.0f});
    dl.rect(preview.x, preview.y, preview.w, preview.h,
        tokens::col_border.alpha(100), xdraw::corner_radius{8.0f});

    dl.text(preview.x + 10.0f, preview.y + 8.0f, "ITEM PREVIEW", tokens::col_text_dim);

    dl.push_clip(canvas.x, canvas.y, canvas.w, canvas.h);
    if (!image(canvas, artwork)) {
        const auto [tw, th] = xdraw::measure_text("Preview unavailable");
        dl.text(canvas.center_x() - tw * 0.5f, canvas.center_y() - th * 0.5f, "Preview unavailable", tokens::col_text_dim);
    }
    dl.pop_clip();

    dl.text(preview.x + 10.0f, preview.bottom() - 40.0f,
        theme::fit_text(title, preview.w - 20.0f), tokens::col_text);

    std::string subtitle;
    xdraw::color sub_color = tokens::col_text_dim;
    if (weapon) {
        const auto rarity = std::clamp(econ.combined_rarity(weapon->def_index, current_paint_kit), 0, 7);
        sub_color = rarity_colors[rarity];
        subtitle = rarity_names[rarity];
        if (selected_skin.stattrak && active_category != 2) {
            subtitle = "StatTrak™ " + subtitle;
            sub_color = xdraw::color{207, 106, 50};
        }
    } else if (active_category == 3) {
        subtitle = "Agent";
    } else if (active_category == 4) {
        subtitle = "Music Kit";
    }
    if (!subtitle.empty()) {
        dl.text(preview.x + 10.0f, preview.bottom() - 22.0f,
            theme::fit_text(subtitle, preview.w - 20.0f), sub_color);
    }
    dl.pop_clip();

    // 4. Mini Music Kit Panel (separated by 12px gap below main panel)
    const xui::rect music{r.x, main.bottom() + 12.0f, r.w, 76.0f};
    panel(music);

    const auto* kit = econ.find_music_kit(hover.music.value_or(settings::g_changer.music.id));
    const xui::rect cover{music.x + 12.0f, music.y + 12.0f, 52.0f, 52.0f};
    dl.rect_filled(cover.x, cover.y, cover.w, cover.h, tokens::col_elevated, xdraw::corner_radius{6.0f});

    if (!kit || !image(cover, econ.get_skin_image(kit->image_inventory))) {
        dl.circle(cover.center_x(), cover.center_y(), 14.0f, tokens::col_accent, 1.5f);
        dl.circle_filled(cover.center_x(), cover.center_y(), 3.0f, tokens::col_accent);
    }

    dl.text(music.x + 76.0f, music.y + 15.0f, "MUSIC KIT", tokens::col_text_dim);
    const auto music_name = kit ? kit->localized_name : "Standard";
    dl.text(music.x + 76.0f, music.y + 34.0f, theme::fit_text(music_name, music.w - 88.0f), tokens::col_text);
    if (kit && !kit->localized_desc.empty()) {
        dl.text(music.x + 76.0f, music.y + 51.0f, theme::fit_text(kit->localized_desc, music.w - 88.0f), tokens::col_text_dim);
    }

    return hovered(music) && xui::ctx().input.mouse_clicked;
}

} // namespace rendering::skin_workspace
