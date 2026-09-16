#pragma once

#include <core/settings.hpp>
#include <core/features/changer/changer.hpp>
#include <core/hooks/hooks.hpp>
#include "../../theme.hpp"
#include <filesystem>
#include <fstream>
#include <cctype>
#include <stdexcept>

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
inline bool hovered(const xui::rect& r) {
    const auto* win = xui::layout::current_window();
    const auto& c = xui::ctx();
    return !c.overlay_blocking() && c.input.in_rect(r) && (!win || c.input.in_rect(win->bounds));
}
inline float left_width(float width) { return std::clamp(width * 0.34f, 200.0f, 280.0f); }
inline std::string lower(std::string value) {
    for (auto& c : value) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return value;
}

// Index immutable schema data once; filtered lists are rebuilt only for a new query.
class catalog {
    std::array<std::uintptr_t, 8> stamp_{};
    std::array<std::vector<const econ_type::item_def*>, 3> weapons_;
    std::unordered_map<std::int16_t, std::vector<const econ_type::paint_kit*>> paints_;
    std::unordered_map<int, std::string> paint_names_, music_names_;
    std::string paint_query_, music_query_;
    int paint_def_ = -1;
    bool music_valid_ = false;
    std::vector<const econ_type::paint_kit*> filtered_paints_;
    std::vector<const econ_type::music_kit*> filtered_music_;
public:
    void refresh() {
        const auto& e = features::changer::g_econ_item_system;
        const std::array<std::uintptr_t, 8> stamp{
            reinterpret_cast<std::uintptr_t>(e.skins().data()), e.skins().size(),
            reinterpret_cast<std::uintptr_t>(e.item_defs().data()), e.item_defs().size(),
            reinterpret_cast<std::uintptr_t>(e.paint_kits().data()), e.paint_kits().size(),
            reinterpret_cast<std::uintptr_t>(e.music_kits().data()), e.music_kits().size()};
        if (stamp == stamp_) return;
        stamp_ = stamp;
        paints_.clear(); paint_names_.clear(); music_names_.clear();
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
        for (const auto& p : e.paint_kits()) paint_names_[p.id] = lower(p.localized_name);
        for (const auto& m : e.music_kits()) music_names_[m.id] = lower(m.localized_name + " " + m.localized_desc + " " + m.name);
        const std::array<const std::vector<const econ_type::item_def*>*, 3> sources{&e.guns(), &e.knives(), &e.gloves()};
        for (std::size_t i = 0; i < sources.size(); ++i)
            for (const auto* w : *sources[i])
                if ((i == 0 || paints_.contains(w->def_index)) && (!w->image_inventory.empty() || paints_.contains(w->def_index)))
                    weapons_[i].push_back(w);
        paint_def_ = -1; music_valid_ = false;
        filtered_paints_.clear(); filtered_music_.clear();
    }
    const auto& weapons(int category) const { return weapons_[std::clamp(category, 0, 2)]; }
    const auto& paints(int def, const std::string& query) {
        if (paint_def_ != def || paint_query_ != query) {
            paint_def_ = def; paint_query_ = query; filtered_paints_.clear();
            const auto q = lower(query);
            if (const auto it = paints_.find(static_cast<std::int16_t>(def)); it != paints_.end())
                for (const auto* p : it->second)
                    if (q.empty() || paint_names_.at(p->id).find(q) != std::string::npos) filtered_paints_.push_back(p);
        }
        return filtered_paints_;
    }
    const auto& music(const std::string& query) {
        if (!music_valid_ || music_query_ != query) {
            music_valid_ = true; music_query_ = query; filtered_music_.clear();
            const auto q = lower(query);
            for (const auto& m : features::changer::g_econ_item_system.music_kits())
                if (q.empty() || music_names_.at(m.id).find(q) != std::string::npos) filtered_music_.push_back(&m);
        }
        return filtered_music_;
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
        for (const auto* key : {"skins", "agents", "custom_agents", "music"})
            if (!j.contains(key) || !j.at(key).is_object()) throw std::runtime_error("Invalid cosmetic profile");
        cosmetics c;
        c.skins.deserialize(j.at("skins")); c.agents.deserialize(j.at("agents"));
        c.custom.deserialize(j.at("custom_agents")); c.music.deserialize(j.at("music"));
        const auto validate_custom = [&](int& index, int side) {
            if (index < 0 || index >= static_cast<int>(c.custom.entries.size()) ||
                (c.custom.entries[index].team != 0 && c.custom.entries[index].team != side)) index = -1;
        };
        validate_custom(c.custom.selected_ct, 3); validate_custom(c.custom.selected_t, 2);
        if (c.music.id < 0 || c.music.id > 65535) c.music.id = 0;
        return c;
    }
    void apply() const {
        // Only cosmetic values are touched; never construct/register a second changer.
        auto& c = settings::g_changer;
        c.skins = skins; c.agents = agents; c.custom_agents = custom; c.music = music;
        features::changer::g_guns.reset(); features::changer::g_knives.reset();
        features::changer::g_gloves.reset(); features::changer::g_agents.reset();
        features::changer::g_music.reset(); features::changer::g_skin_sync.trigger_push();
        hooks::cheat::trigger_lobby_music(static_cast<std::uint16_t>(c.music.id));
        hover = {}; focused_weapon = {};
    }
};
struct profile { std::string name; cosmetics values; };
class profile_store {
    bool initialized_ = false, writable_ = false;
    std::filesystem::path path_;
    bool persist(const std::vector<profile>& next) {
        if (!writable_) return false;
        auto temp = path_; temp += L".tmp";
        try {
            auto list = nlohmann::json::array();
            for (const auto& p : next) list.push_back({{"name", p.name}, {"values", p.values.encode()}});
            const auto bytes = nlohmann::json{{"version", 1}, {"profiles", list}}.dump(2);
            if (bytes.size() > 8 * 1024 * 1024) throw std::runtime_error("Profiles exceed 8 MiB");
            std::filesystem::create_directories(path_.parent_path());
            {
                std::ofstream file;
                file.exceptions(std::ios::failbit | std::ios::badbit);
                file.open(temp, std::ios::binary | std::ios::trunc);
                file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
                file.flush(); file.close();
            }
            if (!MoveFileExW(temp.c_str(), path_.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
                throw std::runtime_error("Cannot replace cosmetic profiles file");
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
            }
            entries = std::move(next); selected = -1; writable_ = true;
            status = "Save stores both loadouts";
        } catch (const std::exception& e) {
            writable_ = false; status = e.what(); // Never overwrite unreadable/corrupt data.
        }
    }
    void create() {
        if (entries.size() >= 5 || !writable_) return;
        int suffix = 1;
        std::string name;
        do { name = "Loadout " + std::to_string(suffix++); }
        while (std::any_of(entries.begin(), entries.end(), [&](const profile& p) { return p.name == name; }));
        auto next = entries;
        next.push_back({name, cosmetics::capture()});
        if (persist(next)) { entries = std::move(next); selected = static_cast<int>(entries.size()) - 1; }
    }
    void save() {
        if (selected < 0 || selected >= static_cast<int>(entries.size())) return;
        auto next = entries; next[selected].values = cosmetics::capture();
        if (persist(next)) entries = std::move(next);
    }
    void select(int index) {
        if (dialog_busy || index < 0 || index >= static_cast<int>(entries.size())) return;
        entries[index].values.apply(); selected = index; status = "Loaded cosmetic profile";
    }
    void remove(int index) {
        if (index < 0 || index >= static_cast<int>(entries.size())) return;
        auto next = entries; next.erase(next.begin() + index);
        if (!persist(next)) return;
        entries = std::move(next);
        if (selected == index) selected = -1;
        else if (selected > index) --selected;
    }
};
inline profile_store profiles;
inline constexpr std::uintptr_t profile_popup_id = 0x534b494e43464735ull;
class profile_popup final : public xui::overlay {
    int delete_row_ = -1;
    xui::rect bounds() const {
        const auto [sw, sh] = xdraw::viewport_size();
        const float w = std::min(280.0f, static_cast<float>(sw));
        const float h = 74.0f + 34.0f * static_cast<float>(profiles.entries.size());
        return {std::clamp(m_anchor.x, 0.0f, std::max(0.0f, sw - w)),
                std::clamp(m_anchor.bottom() + 6, 0.0f, std::max(0.0f, sh - h)), w, h};
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
            const xui::rect row{r.x + 6, r.y + 6 + i * 34.0f, r.w - 12, 32};
            if (!input.in_rect(row)) continue;
            if (input.rmb_clicked) delete_row_ = delete_row_ == i ? -1 : i;
            if (input.mouse_clicked) {
                if (delete_row_ == i && input.mouse_x >= row.right() - 70) {
                    profiles.remove(i); delete_row_ = -1;
                } else if (!dialog_busy) { profiles.select(i); m_closed = true; }
                return true;
            }
        }
        const xui::rect create{r.x + 6, r.y + 6 + profiles.entries.size() * 34.0f, r.w - 12, 32};
        if (input.mouse_clicked && input.in_rect(create) && profiles.entries.size() < 5) { profiles.create(); m_closed = true; }
        return true;
    }
    void render(const xui::style& style, const xui::input_state& input) override {
        if (m_closing || m_closed) { m_closed = true; return; }
        const auto r = bounds();
        auto& dl = xdraw::get(xdraw::layer::top);
        dl.rect_filled(r.x, r.y, r.w, r.h, style.popup_bg, xdraw::corner_radius{8});
        dl.rect(r.x, r.y, r.w, r.h, style.popup_border, xdraw::corner_radius{8});
        for (int i = 0; i < static_cast<int>(profiles.entries.size()); ++i) {
            const xui::rect row{r.x + 6, r.y + 6 + i * 34.0f, r.w - 12, 32};
            dl.rect_filled(row.x, row.y, row.w, row.h, i == profiles.selected ? tokens::col_accent.alpha(35) : (input.in_rect(row) ? tokens::col_elevated : tokens::col_card), xdraw::corner_radius{5});
            dl.text(row.x + 10, row.y + 8, theme::fit_text(profiles.entries[i].name, row.w - (delete_row_ == i ? 88 : 20)), style.text);
            if (delete_row_ == i) {
                const xui::rect del{row.right() - 70, row.y, 70, row.h};
                const bool hot = input.in_rect(del);
                dl.rect_filled(del.x, del.y, del.w, del.h, hot ? xdraw::color{210, 55, 70} : tokens::col_elevated, xdraw::corner_radius{5});
                dl.text(del.x + 12, del.y + 8, "Delete", hot ? xdraw::color{255, 255, 255} : style.text_dim);
            }
        }
        const float y = r.y + 6 + profiles.entries.size() * 34.0f;
        dl.text(r.x + 16, y + 8, profiles.entries.size() < 5 ? "+ Create config" : "5 / 5 configs", tokens::col_accent);
        dl.text(r.x + 16, r.bottom() - 24, "Save edits before switching", style.text_dim);
    }
};
inline void panel(const xui::rect& r) {
    auto& dl = xui::draw::current();
    dl.rect_filled(r.x, r.y, r.w, r.h, tokens::col_card, xdraw::corner_radius{12});
    dl.rect(r.x, r.y, r.w, r.h, tokens::col_border.alpha(140), xdraw::corner_radius{12});
}
inline bool button(const xui::rect& r, const char* label, bool selected = false, bool enabled = true) {
    auto& dl = xui::draw::current();
    const bool hot = enabled && hovered(r);
    const auto fade = xui::anim::lerp(xui::fnv1a(label), hot || selected ? 1.0f : 0.0f, 14.0f);
    dl.rect_filled(r.x, r.y, r.w, r.h, xui::lerp(tokens::col_elevated, tokens::col_accent.alpha(65), fade), xdraw::corner_radius{7});
    const auto [w, h] = xdraw::measure_text(label);
    dl.text(r.x + (r.w - w) * 0.5f, r.y + (r.h - h) * 0.5f, label, enabled ? tokens::col_text : tokens::col_text_dim.alpha(90));
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
    const auto color = side == 3 ? xdraw::color{100, 175, 255} : xdraw::color{235, 190, 90};
    if (side == 3) {
        const std::array<float, 10> shield{x - 7, y - 8, x + 7, y - 8, x + 6, y + 3, x, y + 9, x - 6, y + 3};
        dl.polyline(shield, color, true, 1.6f);
    } else {
        dl.circle(x, y, 9, color, 1.3f);
        dl.line(x - 4, y - 5, x + 4, y + 5, color, 1.8f);
        dl.line(x + 4, y - 5, x - 4, y + 5, color, 1.8f);
    }
}
// Inventory-art fallback, not a native 3D agent preview.
inline bool sidebar(const xui::rect& r) {
    auto& dl = xui::draw::current();
    auto& econ = features::changer::g_econ_item_system;
    const xui::rect main{r.x, r.y, r.w, r.h - 88};
    panel(main);
    dl.text(r.x + 14, r.y + 14, "COSMETIC CONFIG", tokens::col_text_dim);
    const xui::rect selector{r.x + 14, r.y + 36, r.w - 96, 32};
    const auto label = profiles.selected >= 0 ? profiles.entries[profiles.selected].name : "Choose config";
    if (button(selector, theme::fit_text(label, selector.w - 18).c_str(), false, profiles.ready() && !dialog_busy))
        xui::overlays::add(std::make_unique<profile_popup>(selector));
    xui::overlays::touch(profile_popup_id);
    if (button({selector.right() + 6, selector.y, 62, 32}, profiles.ready() ? "Save" : "Retry", false,
        !dialog_busy && (!profiles.ready() || profiles.selected >= 0))) {
        if (profiles.ready()) profiles.save(); else profiles.load(true);
    }
    dl.text(r.x + 14, r.y + 77, theme::fit_text(profiles.status, r.w - 28), tokens::col_text_dim);
    const xui::rect preview{r.x + 14, r.y + 102, r.w - 28, std::max(40.0f, main.h - 212)};
    dl.push_clip(preview.x, preview.y, preview.w, preview.h);
    dl.rect_filled(preview.x, preview.y, preview.w, preview.h, tokens::col_dark, xdraw::corner_radius{9});
    dl.text(preview.x + 10, preview.y + 10, "Inventory preview (2D)", tokens::col_text_dim);
    const auto& ca = settings::g_changer.custom_agents;
    const int custom = hover.custom != -2 ? hover.custom : (team == 3 ? ca.selected_ct : ca.selected_t);
    const auto agent_id = hover.agent.value_or(team == 3 ? settings::g_changer.agents.ct_def : settings::g_changer.agents.t_def);
    const auto* agent = econ.find_def(agent_id);
    const float agent_h = std::max(20.0f, (preview.h - 42) * 0.58f);
    std::string agent_name;
    if (custom >= 0 && custom < static_cast<int>(ca.entries.size())) {
        agent_name = ca.entries[custom].name;
        dl.text(preview.x + 10, preview.y + 46, "Custom model selected", tokens::col_accent);
        dl.text(preview.x + 10, preview.y + 65, "3D preview unavailable", tokens::col_text_dim);
    } else {
        if (agent) agent_name = agent->localized_name;
        else agent_name = team == 3 ? "Default CT" : "Default T";
        if (!agent || !image({preview.x + 8, preview.y + 30, preview.w - 16, agent_h}, econ.get_skin_image(agent->image_inventory)))
            dl.text(preview.x + 10, preview.y + 50, "Agent artwork unavailable", tokens::col_text_dim);
    }
    dl.text(preview.x + 10, preview.y + 32 + agent_h, theme::fit_text(agent_name, preview.w - 20), tokens::col_text);
    const auto weapon_id = hover.weapon.value_or(focused_weapon[team == 3 ? 0 : 1]);
    const auto* weapon = econ.find_def(weapon_id);
    const auto& skins = settings::g_changer.skins.for_team(team);
    if (weapon) {
        const auto it = skins.find(weapon_id);
        const int paint = hover.paint.value_or(it != skins.end() ? it->second.paint_kit_id : 0);
        const auto* art = paint ? econ.get_skin_image(weapon_id, paint) : econ.get_skin_image(weapon->image_inventory);
        image({preview.x + 8, preview.y + 55 + agent_h, preview.w - 16, std::max(12.0f, preview.h - agent_h - 85)}, art);
        const auto* kit = econ.find_paint_kit(paint);
        dl.text(preview.x + 10, preview.bottom() - 22, theme::fit_text(kit ? kit->localized_name : weapon->localized_name, preview.w - 20), tokens::col_text);
    } else dl.text(preview.x + 10, preview.bottom() - 24, "Hover an item to preview", tokens::col_text_dim);
    dl.pop_clip();
    for (const int side : {3, 2}) {
        const xui::rect row{r.x + 14, main.bottom() - (side == 3 ? 94.0f : 48.0f), r.w - 28, 36};
        if (button(row, side == 3 ? "CT Loadout" : "T Loadout", team == side)) team = side;
        team_icon(row.x + 18, row.center_y(), side);
    }
    const xui::rect music{r.x, main.bottom() + 12, r.w, 76};
    panel(music);
    const auto* kit = econ.find_music_kit(hover.music.value_or(settings::g_changer.music.id));
    const xui::rect cover{music.x + 12, music.y + 12, 52, 52};
    dl.rect_filled(cover.x, cover.y, cover.w, cover.h, tokens::col_elevated, xdraw::corner_radius{6});
    if (!kit || !image(cover, econ.get_skin_image(kit->image_inventory))) {
        dl.circle(cover.center_x(), cover.center_y(), 14, tokens::col_accent, 1.5f);
        dl.circle_filled(cover.center_x(), cover.center_y(), 3, tokens::col_accent);
    }
    dl.text(music.x + 76, music.y + 16, "MUSIC KIT", tokens::col_text_dim);
    dl.text(music.x + 76, music.y + 37, theme::fit_text(kit ? kit->localized_name : "Default", music.w - 88), tokens::col_text);
    return hovered(music) && xui::ctx().input.mouse_clicked;
}
} // namespace rendering::skin_workspace
