#pragma once

#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <core/systems/systems.hpp>
#include <core/rendering/theme.hpp>
#include <external/xdraw/xui/xui.hpp>
#include <utilities/cstypes.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

namespace nemesis::preview3d {
inline constexpr std::uintptr_t agent_window_id = 0x4e454d3341475052ull;

class agent_preview_window final : public xui::overlay {
    xui::rect window_{};
    systems::model_preview::request request_{};
    std::string agent_name_ = "CS2 Agent";
    float open_anim_{};
    float preview_ready_time_{};
    bool rotating_{}, dragging_window_{};
    float grab_x_{}, grab_y_{};

    struct point2 { float x{}, y{}; };
    struct projected_agent {
        std::array<point2, 16> bones{};
        std::array<bool, 16> bone_valid{};
        xui::rect bounds{};
        bool valid{};
    };

    [[nodiscard]] static projected_agent project_agent(
        const systems::model_preview::agent_pose_snapshot& pose) {
        static constexpr std::array<std::uint32_t, 16> bone_map{{
            cstypes::bone_ids::head, cstypes::bone_ids::neck,
            cstypes::bone_ids::spine_4, cstypes::bone_ids::pelvis,
            cstypes::bone_ids::left_shoulder, cstypes::bone_ids::left_elbow,
            cstypes::bone_ids::left_hand, cstypes::bone_ids::right_shoulder,
            cstypes::bone_ids::right_elbow, cstypes::bone_ids::right_hand,
            cstypes::bone_ids::left_hip, cstypes::bone_ids::left_knee,
            cstypes::bone_ids::left_foot, cstypes::bone_ids::right_hip,
            cstypes::bone_ids::right_knee, cstypes::bone_ids::right_foot
        }};
        projected_agent result{};
        float min_x = pose.right, min_y = pose.bottom;
        float max_x = pose.left, max_y = pose.top;
        std::size_t valid_points{};
        for (std::size_t i = 0; i < bone_map.size(); ++i) {
            const auto bone_index = bone_map[i];
            if (bone_index >= pose.bones.size() || !pose.bones[bone_index].valid)
                continue;
            auto& point = result.bones[i];
            point.x = pose.bones[bone_index].x;
            point.y = pose.bones[bone_index].y;
            result.bone_valid[i] = true;
            min_x = std::min(min_x, point.x);
            min_y = std::min(min_y, point.y);
            max_x = std::max(max_x, point.x);
            max_y = std::max(max_y, point.y);
            ++valid_points;
        }

        if (!pose.valid || valid_points < 10 || !result.bone_valid[0] ||
            !result.bone_valid[3] || !result.bone_valid[12] || !result.bone_valid[15] ||
            pose.right <= pose.left || pose.bottom <= pose.top)
            return result;
        result.bounds = {pose.left, pose.top, pose.right - pose.left, pose.bottom - pose.top};
        result.valid = true;
        return result;
    }

    [[nodiscard]] xui::rect body() const {
        return {window_.x + 10.0f, window_.y + 73.0f, window_.w - 20.0f, window_.h - 88.0f};
    }

    void select_agent(int team) {
        team = team == 2 ? 2 : 3;
        // Team changes create a new native preview actor. Do not carry the
        // previous tab's inspection rotation into the new model.
        // Let the native preview camera choose the actor's initial facing.
        // Applying a fixed quarter turn makes some agents render side-on.
        request_.yaw = 0.0f;
        request_.pitch = 0.0f;
        const auto& changer = settings::g_changer;
        auto def_index = team == 3 ? changer.agents.ct_def : changer.agents.t_def;
        std::string model_path;
        const auto custom_index = team == 3 ? changer.custom_agents.selected_ct : changer.custom_agents.selected_t;
        if (custom_index >= 0 && custom_index < static_cast<int>(changer.custom_agents.entries.size())) {
            const auto& custom = changer.custom_agents.entries[custom_index];
            if ((custom.team == 0 || custom.team == team) && !custom.model_path.empty())
                model_path = custom.model_path;
        }

        auto& items = features::changer::g_econ_item_system;
        const features::changer::econ_item_system::item_def* def =
            def_index ? items.find_def(def_index) : nullptr;
        if (!def) {
            for (const auto* candidate : items.agents()) {
                if (candidate && candidate->team() == team) {
                    def = candidate;
                    def_index = candidate->def_index;
                    break;
                }
            }
        }
        if (model_path.empty() && def) model_path = def->model_player;
        if (model_path.empty()) {
            model_path = team == 3
                ? "agents/models/ctm_sas/ctm_sas.vmdl"
                : "agents/models/tm_phoenix/tm_phoenix.vmdl";
        }

        request_.type = systems::model_preview::kind::agent;
        request_.team = team;
        request_.def_index = def_index;
        request_.paint_kit = request_.music_kit = 0;
        request_.model_path = std::move(model_path);
        agent_name_ = def ? (def->localized_name.empty() ? def->name : def->localized_name)
            : (team == 3 ? "Counter-Terrorist" : "Terrorist");
    }

    static bool button(xdraw::draw_list& dl, const xui::input_state& input,
        const xui::rect& rect, std::string_view text, const xui::style& style, bool active = false) {
        const bool hovered = input.in_rect(rect);
        auto bg = active ? style.button_active : hovered ? style.button_hovered : style.button_bg;
        dl.rect_filled(rect.x, rect.y, rect.w, rect.h, bg, xdraw::corner_radius{6.0f});
        dl.rect(rect.x, rect.y, rect.w, rect.h,
            active ? tokens::col_accent.alpha(180) : style.child_border, xdraw::corner_radius{6.0f}, 1.0f);
        const auto label = xui::truncate(text, rect.w - 8.0f);
        const auto [tw, th] = xdraw::measure_text(label);
        dl.text(rect.x + (rect.w - tw) * 0.5f, rect.y + (rect.h - th) * 0.5f, label,
            active ? tokens::col_accent : style.text);
        return hovered && input.mouse_clicked;
    }

public:
    agent_preview_window(const xui::rect& menu)
        : overlay(agent_window_id, menu) {
        const auto [sw, sh] = xdraw::viewport_size();
        constexpr float w = 378.0f, h = 568.0f;
        // Reserve room for the side trigger; the preview always docks to the right.
        const float x = menu.x + menu.w + 82.0f;
        window_ = {
            x,
            std::clamp(menu.y + 24.0f, 8.0f, std::max(8.0f, sh - h - 8.0f)),
            std::min(w, std::max(220.0f, sw - x - 8.0f)), h
        };
        auto team = systems::g_local.get().team;
        if (team != 2 && team != 3) team = 3;
        select_agent(team == 3 ? 2 : 3);
    }

    [[nodiscard]] bool hit_test(float x, float y) const override {
        return !m_closed && (dragging_window_ || window_.contains(x, y));
    }

    [[nodiscard]] bool blocks_background() const noexcept override { return false; }

    bool process_input(const xui::input_state& input) override {
        if (m_closing) {
            systems::g_model_preview.hide();
            force_close();
            return true;
        }
        for (const auto key : input.key_presses()) {
            if (key == VK_ESCAPE) {
                systems::g_model_preview.hide();
                force_close();
                return true;
            }
        }
        if (!input.mouse_down)
            dragging_window_ = false;
        const xui::rect title_drag_area{
            window_.x, window_.y, std::max(0.0f, window_.w - 50.0f), 38.0f
        };
        if (input.mouse_clicked && title_drag_area.contains(input.mouse_x, input.mouse_y)) {
            dragging_window_ = true;
            grab_x_ = input.mouse_x - window_.x;
            grab_y_ = input.mouse_y - window_.y;
        }
        if (dragging_window_ && input.mouse_down) {
            const auto [screen_w, screen_h] = xdraw::viewport_size();
            window_.x = std::clamp(input.mouse_x - grab_x_, 0.0f,
                std::max(0.0f, screen_w - window_.w));
            window_.y = std::clamp(input.mouse_y - grab_y_, 0.0f,
                std::max(0.0f, screen_h - window_.h));
        }
        const auto area = body();
        if (!input.rmb_down) rotating_ = false;
        if (input.rmb_clicked && area.contains(input.mouse_x, input.mouse_y)) rotating_ = true;
        if (rotating_ && input.rmb_down) {
            request_.yaw = std::remainder(request_.yaw + input.mouse_delta_x() * 0.5f, 360.0f);
            request_.pitch = std::clamp(request_.pitch + input.mouse_delta_y() * 0.35f, -35.0f, 35.0f);
        }
        return window_.contains(input.mouse_x, input.mouse_y);
    }

    void render(const xui::style& style, const xui::input_state& input) override {
        if (m_closing || m_closed) {
            systems::g_model_preview.hide();
            force_close();
            return;
        }
        const auto dt = xdraw::delta_time();
        open_anim_ = std::min(open_anim_ + dt * 2.8f, 1.0f);
        const bool preview_connected = systems::g_model_preview.connected();
        preview_ready_time_ = preview_connected
            ? std::min(preview_ready_time_ + dt, 1.0f) : 0.0f;
        const bool preview_live = preview_connected && preview_ready_time_ >= 0.4f;
        auto viewport = body();
        // Expanding the real Panorama render surface sells the agent's approach
        // animation while CS2 plays the native team-intro animation graph.
        const float inset = (1.0f - xui::ease::out_cubic(open_anim_)) * 30.0f;
        const float panel_w = std::max(64.0f, viewport.w - inset * 2.0f);
        const float panel_h = std::max(64.0f, viewport.h - inset * 2.0f);
        // Keep Panorama's render surface pixel-aligned with the visible body.
        // Oversizing this surface clips the native actor and makes the ESP fit
        // a different rectangle than the one the user sees.
        constexpr float agent_zoom = 1.0f;
        request_.width = static_cast<int>(panel_w * agent_zoom);
        request_.height = static_cast<int>(panel_h * agent_zoom);
        request_.x = viewport.x + inset - (request_.width - panel_w) * 0.5f;
        request_.y = viewport.y + inset - (request_.height - panel_h) * 0.5f;
        const auto [screen_w, screen_h] = xdraw::viewport_size();
        request_.screen_width = static_cast<int>(screen_w);
        request_.screen_height = static_cast<int>(screen_h);
        auto frame_bg = style.window_bg;
        frame_bg.a = 255;
        request_.background_rgb = (static_cast<std::uint32_t>(frame_bg.r) << 16) |
            (static_cast<std::uint32_t>(frame_bg.g) << 8) | frame_bg.b;
        request_.visible = true;
        systems::g_model_preview.submit(request_);

        auto& dl = xdraw::get(xdraw::layer::top);
        // Keep an aperture over the native 3D panel. Panorama supplies its
        // opaque menu-colored backplate behind the agent.
        dl.rect_filled(window_.x, window_.y, window_.w, viewport.y - window_.y,
            frame_bg, xdraw::corner_radius::top(12.0f));
        dl.rect_filled(window_.x, viewport.bottom(), window_.w, window_.bottom() - viewport.bottom(),
            frame_bg, xdraw::corner_radius::bottom(12.0f));
        dl.rect_filled(window_.x, viewport.y, viewport.x - window_.x, viewport.h, frame_bg);
        dl.rect_filled(viewport.right(), viewport.y, window_.right() - viewport.right(), viewport.h, frame_bg);
        dl.rect(window_.x, window_.y, window_.w, window_.h,
            xui::lighten(style.popup_border, 1.15f), xdraw::corner_radius{12.0f}, 1.0f);
        dl.rect_filled(window_.x + 1.0f, window_.y + 1.0f, window_.w - 2.0f, 38.0f,
            frame_bg, xdraw::corner_radius{11.0f, 11.0f, 0.0f, 0.0f});
        dl.text(window_.x + 14.0f, window_.y + 12.0f, "AGENT PREVIEW", style.text);
        const auto badge = request_.team == 3 ? "CT" : "T";
        dl.rect_filled(window_.right() - 92.0f, window_.y + 8.0f, 34.0f, 22.0f,
            tokens::col_accent.alpha(42), xdraw::corner_radius{5.0f});
        dl.text(window_.right() - 82.0f, window_.y + 12.0f, badge, tokens::col_accent);
        const xui::rect close{window_.right() - 42.0f, window_.y + 7.0f, 30.0f, 25.0f};
        if (button(dl, input, close, "x", style)) {
            systems::g_model_preview.hide();
            force_close();
            return;
        }

        const float tabs_y = window_.y + 41.0f;
        const float tab_w = (window_.w - 28.0f) * 0.5f;
        if (button(dl, input, {window_.x + 10.0f, tabs_y, tab_w, 27.0f}, "Terrorist", style, request_.team == 2))
            select_agent(2);
        if (button(dl, input, {window_.x + 18.0f + tab_w, tabs_y, tab_w, 27.0f}, "Counter-Terrorist", style, request_.team == 3))
            select_agent(3);

        if (!preview_live)
            dl.rect_filled(viewport.x, viewport.y, viewport.w, viewport.h,
                frame_bg, xdraw::corner_radius{7.0f});
        dl.rect(viewport.x, viewport.y, viewport.w, viewport.h,
            tokens::col_border.alpha(160), xdraw::corner_radius{7.0f}, 1.0f);

        const auto local_team = systems::g_local.get().team;
        const auto target_index = systems::g_model_preview.is_enemy_preview(local_team) ? 0u : 1u;
        const auto& esp = settings::g_esp.m_player.m_overlay[target_index];
        const auto projected = project_agent(systems::g_model_preview.get_agent_pose());
        const auto box = projected.bounds;
        dl.push_clip(viewport.x, viewport.y, viewport.w, viewport.h);
        // Use the pose captured from the exact PreviewPlayer draw. When its
        // fresh bone projection is unavailable, suppress ESP instead of drawing
        // a guessed skeleton that drifts away from the animated model.
        if (preview_live && projected.valid && esp.enabled.value) {
            if (esp.m_box.enabled.value) {
                const auto color = esp.m_box.visible_color.value;
                if (esp.m_box.fill.value)
                    dl.rect_filled(box.x, box.y, box.w, box.h, color.alpha(18));
                if (esp.m_box.outline.value)
                    dl.rect(box.x - 1.0f, box.y - 1.0f, box.w + 2.0f, box.h + 2.0f,
                        xdraw::color{0, 0, 0, 210}, xdraw::corner_radius{0.0f}, 3.0f);
                if (esp.m_box.style.value == settings::esp::player::overlay::box::style_type::full) {
                    dl.rect(box.x, box.y, box.w, box.h, color, xdraw::corner_radius{0.0f}, 1.4f);
                } else {
                    const float max_corner = std::max(4.0f, box.w * 0.35f);
                    const float c = std::clamp(esp.m_box.corner_length.value, 4.0f, max_corner);
                    dl.line(box.x, box.y, box.x + c, box.y, color, 1.4f);
                    dl.line(box.x, box.y, box.x, box.y + c, color, 1.4f);
                    dl.line(box.right() - c, box.y, box.right(), box.y, color, 1.4f);
                    dl.line(box.right(), box.y, box.right(), box.y + c, color, 1.4f);
                    dl.line(box.x, box.bottom() - c, box.x, box.bottom(), color, 1.4f);
                    dl.line(box.x, box.bottom(), box.x + c, box.bottom(), color, 1.4f);
                    dl.line(box.right() - c, box.bottom(), box.right(), box.bottom(), color, 1.4f);
                    dl.line(box.right(), box.bottom() - c, box.right(), box.bottom(), color, 1.4f);
                }
            }
            if (esp.m_skeleton.enabled.value) {
                const auto color = esp.m_skeleton.visible_color.value;
                const float thickness = std::clamp(esp.m_skeleton.thickness.value, 0.5f, 5.0f);
                static constexpr std::array<std::pair<std::size_t, std::size_t>, 14> edges{{
                    {0, 1}, {1, 2}, {2, 3}, {1, 4}, {4, 5}, {5, 6}, {1, 7},
                    {7, 8}, {8, 9}, {3, 10}, {10, 11}, {11, 12}, {3, 13}, {13, 14}
                }};
                for (const auto& [from, to] : edges) {
                    if (!projected.bone_valid[from] || !projected.bone_valid[to])
                        continue;
                    const auto& a = projected.bones[from];
                    const auto& b = projected.bones[to];
                    dl.line(a.x, a.y, b.x, b.y, color, thickness);
                }
                if (projected.bone_valid[14] && projected.bone_valid[15]) {
                    const auto& knee = projected.bones[14];
                    const auto& foot = projected.bones[15];
                    dl.line(knee.x, knee.y, foot.x, foot.y, color, thickness);
                }
                if (projected.bone_valid[0]) {
                    const auto& head = projected.bones[0];
                    dl.circle(head.x, head.y, std::max(3.0f, viewport.w * 0.018f), color, thickness);
                }
            }
            if (esp.m_health_bar.enabled.value) {
                const auto& health = esp.m_health_bar;
                const auto position = health.position.value;
                const bool vertical = position == settings::esp::player::overlay::health_bar::position_type::left;
                const float x = vertical ? box.x - 7.0f : box.x;
                const float y = position == settings::esp::player::overlay::health_bar::position_type::top
                    ? box.y - 7.0f : position == settings::esp::player::overlay::health_bar::position_type::bottom
                        ? box.bottom() + 7.0f : box.y;
                const float w = vertical ? 4.0f : box.w;
                const float h = vertical ? box.h : 4.0f;
                if (health.glow.value)
                    dl.rect_filled(x - 2.0f, y - 2.0f, w + 4.0f, h + 4.0f,
                        health.glow_color.value.alpha(static_cast<std::uint8_t>(
                            std::clamp(health.glow_strength.value, 0.0f, 1.0f) * 96.0f)));
                if (health.outline_setting.value)
                    dl.rect_filled(x - 1.0f, y - 1.0f, w + 2.0f, h + 2.0f, health.outline_color.value);
                dl.rect_filled(x, y, w, h, health.background_color.value);
                constexpr float health_fraction = 0.76f;
                const float filled = (vertical ? h : w) * health_fraction;
                if (vertical) {
                    if (health.gradient.value)
                        dl.rect_filled_gradient(x, y + h - filled, w, filled,
                            health.full_color.value, health.full_color.value,
                            health.low_color.value, health.low_color.value);
                    else
                        dl.rect_filled(x, y + h - filled, w, filled, health.full_color.value);
                } else if (health.gradient.value) {
                    dl.rect_filled_gradient(x, y, filled, h,
                        health.low_color.value, health.full_color.value,
                        health.full_color.value, health.low_color.value);
                } else {
                    dl.rect_filled(x, y, filled, h, health.full_color.value);
                }
                if (health.show_value.value)
                    dl.text(vertical ? x - 18.0f : x, y - 14.0f, "76",
                        health.text_color.value, xdraw::text_style::outlined);
            }
            if (esp.m_ammo_bar.enabled.value) {
                const auto& ammo = esp.m_ammo_bar;
                const auto position = ammo.position.value;
                const bool vertical = position == settings::esp::player::overlay::ammo_bar::position_type::left;
                const float x = vertical ? box.x - 13.0f : box.x;
                const float y = position == settings::esp::player::overlay::ammo_bar::position_type::top
                    ? box.y - 13.0f : position == settings::esp::player::overlay::ammo_bar::position_type::bottom
                        ? box.bottom() + 13.0f : box.y;
                const float w = vertical ? 3.5f : box.w;
                const float h = vertical ? box.h : 3.5f;
                constexpr float ammo_fraction = 0.58f;
                const float filled = (vertical ? h : w) * ammo_fraction;
                if (ammo.glow.value)
                    dl.rect_filled(x - 2.0f, y - 2.0f, w + 4.0f, h + 4.0f,
                        ammo.glow_color.value.alpha(static_cast<std::uint8_t>(
                            std::clamp(ammo.glow_strength.value, 0.0f, 1.0f) * 96.0f)));
                if (ammo.outline_setting.value)
                    dl.rect_filled(x - 1.0f, y - 1.0f, w + 2.0f, h + 2.0f, ammo.outline_color.value);
                dl.rect_filled(x, y, w, h, ammo.background_color.value);
                if (vertical) {
                    if (ammo.gradient.value)
                        dl.rect_filled_gradient(x, y + h - filled, w, filled,
                            ammo.full_color.value, ammo.full_color.value,
                            ammo.low_color.value, ammo.low_color.value);
                    else
                        dl.rect_filled(x, y + h - filled, w, filled, ammo.full_color.value);
                } else if (ammo.gradient.value) {
                    dl.rect_filled_gradient(x, y, filled, h,
                        ammo.low_color.value, ammo.full_color.value,
                        ammo.full_color.value, ammo.low_color.value);
                } else {
                    dl.rect_filled(x, y, filled, h, ammo.full_color.value);
                }
                if (ammo.show_value.value)
                    dl.text(vertical ? x - 19.0f : x, y + h + 2.0f, "7/12",
                        ammo.text_color.value, xdraw::text_style::outlined);
            }
            if (esp.m_name.enabled.value) {
                const auto [tw, th] = xdraw::measure_text(agent_name_);
                dl.text(box.x + (box.w - tw) * 0.5f, box.y - th - 4.0f,
                    agent_name_, esp.m_name.color.value, xdraw::text_style::outlined);
            }
            if (esp.m_info_flags.enabled.value) {
                const auto& flags = esp.m_info_flags;
                struct flag_preview {
                    settings::esp::player::overlay::info_flags::flag flag;
                    std::string_view label;
                    xdraw::color color;
                };
                const std::array<flag_preview, 9> flag_previews{{
                    {settings::esp::player::overlay::info_flags::money, "$16,000", flags.money_color.value},
                    {settings::esp::player::overlay::info_flags::armor, "ARMOR", flags.armor_color.value},
                    {settings::esp::player::overlay::info_flags::kit, "KIT", flags.kit_color.value},
                    {settings::esp::player::overlay::info_flags::scoped, "SCOPED", flags.scoped_color.value},
                    {settings::esp::player::overlay::info_flags::defusing, "DEFUSING", flags.defusing_color.value},
                    {settings::esp::player::overlay::info_flags::flashed, "FLASHED", flags.flashed_color.value},
                    {settings::esp::player::overlay::info_flags::ping, "38ms", flags.distance_color.value},
                    {settings::esp::player::overlay::info_flags::distance, "12m", flags.distance_color.value},
                    {settings::esp::player::overlay::info_flags::c4, "C4", flags.c4_color.value}
                }};
                float flags_y = box.y + 4.0f;
                for (const auto& flag : flag_previews) {
                    if (!flags.has(flag.flag)) continue;
                    dl.text(box.right() + 6.0f, flags_y, flag.label,
                        flag.color, xdraw::text_style::outlined);
                    flags_y += 13.0f;
                }
            }
            if (esp.m_weapon.enabled.value) {
                const bool is_ct = request_.team == 3;
                const auto weapon_name = is_ct ? std::string_view{"usp_silencer"} : std::string_view{"glock"};
                const auto weapon_label = is_ct ? std::string_view{"USP-S"} : std::string_view{"Glock-18"};
                const auto display = esp.m_weapon.display.value;
                const bool show_icon = display == settings::esp::player::overlay::weapon::display_type::icon ||
                    display == settings::esp::player::overlay::weapon::display_type::text_and_icon;
                const bool show_text = display == settings::esp::player::overlay::weapon::display_type::text ||
                    display == settings::esp::player::overlay::weapon::display_type::text_and_icon;
                float y = box.bottom() + 5.0f;
                bool icon_drawn = false;
                if (show_icon) {
                    const auto* icon = systems::g_icons.get(std::string{weapon_name}, 0.35f);
                    if (icon && icon->texture) {
                        const float x = box.x + (box.w - static_cast<float>(icon->width)) * 0.5f;
                        dl.image(x, y, static_cast<float>(icon->width), static_cast<float>(icon->height),
                            icon->texture.Get(), esp.m_weapon.icon_color.value);
                        y += static_cast<float>(icon->height) + 2.0f;
                        icon_drawn = true;
                    }
                }
                if (show_text || (show_icon && !icon_drawn)) {
                    const auto [tw, th] = xdraw::measure_text(weapon_label);
                    dl.text(box.x + (box.w - tw) * 0.5f, y, weapon_label,
                        esp.m_weapon.text_color.value, xdraw::text_style::outlined);
                }
            }
        }
        dl.pop_clip();
    }
};

inline void open_agent_window(const xui::rect& menu) {
    if (const auto* existing = xui::overlays::find(agent_window_id);
        existing && !existing->is_closed() && !existing->is_closing()) return;
    xui::overlays::add(std::make_unique<agent_preview_window>(menu));
}
}
