#include <pch/pch.hpp>
#include <core/settings.hpp>
#include <core/systems/systems.hpp>
#include <core/rendering/preview3d/agent_window.hpp>

#include "../../rendering.hpp"
#include "menu.chams_common.hpp"

namespace rendering {

void menu::draw_player(float group_w, int subtab) const
{
	auto& esp = settings::g_esp;
	auto& p = esp.m_player;
	const auto col_w = (this->m_body_w - tokens::gap) * 0.5f;
	constexpr float k_header_h = menu::k_panel_header_h;
	subtab = std::clamp(subtab, 0, 2);
	const auto has_overlay = (subtab <= 1);
	auto draw_panel_title = [&](float x, const char* title) {
		this->draw_column_header(x, title);
	};

	draw_panel_title(this->m_body_x, has_overlay ? "PLAYER CHAMS" : "LOCAL CHAMS");
	draw_panel_title(this->m_body_x + col_w + tokens::gap, has_overlay ? "PREVIEW" : "VIEWMODEL");

	const auto column_h = this->m_body_h - k_header_h;

	// ==================== LEFT COLUMN ====================
	xui::layout::set_cursor(this->m_body_x - this->m_x, this->m_body_y + k_header_h - this->m_y);
	if (has_overlay)
	{
		auto& chams = (subtab == 0) ? p.m_chams.enemy : p.m_chams.team;
		auto& chams_ragdoll = (subtab == 0) ? p.m_chams.enemy_ragdoll : p.m_chams.team_ragdoll;
		auto& glow = (subtab == 0) ? p.m_glow.enemy : p.m_glow.team;
		auto& glow_ragdoll = (subtab == 0) ? p.m_glow.enemy_ragdoll : p.m_glow.team_ragdoll;

		if (xui::begin_child("##player_chams_left", col_w, column_h, true))
		{
			xui::toggle("player chams", chams.enabled);
			chams.primary.enabled.value = chams.enabled.value;
			if (xui::begin_popup("##player_chams_popup", 235.0f))
			{
				const bool is_vis_glow = ( chams.primary.material.value == settings::esp::cham_ids::outline_glow || chams.primary.material.value == settings::esp::cham_ids::outline_glow_ignorez );
				const auto prev_vis_mat = chams.primary.material.value;
				xui::color_picker("color##visible", chams.primary.color);
				if ( detail::draw_chams_material_combo("material##visible", chams.primary.material, false) )
				{
					detail::reset_filled_for_new_outline( chams.primary, prev_vis_mat );
				}
				if (is_vis_glow)
				{
					xui::layout::spacing( 5.0f );
					xui::layout::separator( );
					detail::draw_outline_glow_sliders( "p_vis_glow", chams.primary.glow );
				}

				xui::layout::spacing(5.0f);

				// Through Wall checkbox (secondary layer, ignorez materials without (iz) in name)
				xui::checkbox("through wall", chams.secondary.enabled);
				if (chams.secondary.enabled)
				{
					const bool is_wall_glow = ( chams.secondary.material.value == settings::esp::cham_ids::outline_glow || chams.secondary.material.value == settings::esp::cham_ids::outline_glow_ignorez );
					const auto prev_wall_mat = chams.secondary.material.value;
					xui::color_picker("color##wall", chams.secondary.color);
					if ( detail::draw_chams_material_combo("material##wall", chams.secondary.material, true) )
					{
						detail::reset_filled_for_new_outline( chams.secondary, prev_wall_mat );
					}
					if (is_wall_glow)
					{
						xui::layout::spacing( 5.0f );
						xui::layout::separator( );
						detail::draw_outline_glow_sliders( "p_wall_glow", chams.secondary.glow );
					}
				}

				xui::layout::spacing(5.0f);

				// Overlay checkbox (overlay layer, non-iz materials)
				xui::checkbox("overlay", chams.overlay.enabled);
				if (chams.overlay.enabled)
				{
					const bool is_ov_glow = ( chams.overlay.material.value == settings::esp::cham_ids::outline_glow || chams.overlay.material.value == settings::esp::cham_ids::outline_glow_ignorez );
					const auto prev_ov_mat = chams.overlay.material.value;
					xui::color_picker("color##overlay", chams.overlay.color);
					if ( detail::draw_chams_material_combo("material##overlay", chams.overlay.material, false) )
					{
						detail::reset_filled_for_new_outline( chams.overlay, prev_ov_mat );
					}
					if (is_ov_glow)
					{
						xui::layout::spacing( 5.0f );
						xui::layout::separator( );
						detail::draw_outline_glow_sliders( "p_ov_glow", chams.overlay.glow );
					}
				}

				xui::end_popup();
			}

			xui::layout::separator();
			detail::draw_chams_config("ragdoll chams", "ragdoll", chams_ragdoll, false);
			if (subtab == 0)
			{
				xui::layout::separator();
				detail::draw_chams_config("onshot chams", "os", p.m_chams.onshot, false, true, &p.m_chams.onshot_fade_time.value);
			}

			xui::layout::separator();
			xui::toggle("glow", glow.enabled);
			if (xui::begin_popup("##glow_popup", 220.0f)) {
				xui::color_picker("color##glow", glow.color);
				xui::end_popup();
			}
			xui::toggle("ragdoll glow", glow_ragdoll.enabled);
			if (xui::begin_popup("##glow_rag_popup", 220.0f)) {
				xui::color_picker("color##glow_rag", glow_ragdoll.color);
				xui::end_popup();
			}

			xui::end_child();
		}
	}
	else
	{
		if (xui::begin_child("##local_chams_glow", col_w, column_h, true))
		{
			detail::draw_chams_config("chams", "local_main", p.m_chams.local);
			xui::layout::separator();
			xui::toggle("lower opacity", esp.m_local_alpha.enabled);
			if (xui::begin_popup("##local_alpha_popup", 220.0f))
			{
				xui::slider_float("opacity", esp.m_local_alpha.opacity, 0.0f, 1.0f, "%.2f");
				xui::checkbox("only when scoped", esp.m_local_alpha.only_scoped);
				xui::end_popup();
			}
			xui::layout::separator();
			detail::draw_chams_config("ragdoll chams", "local_ragdoll", p.m_chams.local_ragdoll, false);
			xui::layout::separator();
			xui::toggle("glow", p.m_glow.local.enabled);
			if (xui::begin_popup("##local_glow_popup", 220.0f))
			{
				xui::color_picker("color##local_glow", p.m_glow.local.color);
				xui::end_popup();
			}
			xui::toggle("ragdoll glow", p.m_glow.local_ragdoll.enabled);
			if (xui::begin_popup("##local_glow_rag_popup", 220.0f))
			{
				xui::color_picker("color##local_glow_rag", p.m_glow.local_ragdoll.color);
				xui::end_popup();
			}
			xui::end_child();
		}
	}

	// ==================== RIGHT COLUMN ====================
	const auto col_right_x = this->m_body_x + col_w + tokens::gap;
	const auto col_right_y = this->m_body_y + k_header_h;

	if (has_overlay)
	{
		constexpr float k_esp_bar_h = 38.0f;
		constexpr float k_spacing = 6.0f;
		const float preview_h = column_h - k_esp_bar_h - k_spacing;
		const xui::rect preview_rect{ col_right_x, col_right_y, col_w, preview_h };

		// Preview model selection
		const auto local_team = systems::g_local.get().team;
		static int s_preview_team_choice = 0; // 0 = auto
		int target_team = 2;
		if (s_preview_team_choice == 2 || s_preview_team_choice == 3)
		{
			target_team = s_preview_team_choice;
		}
		else
		{
			target_team = (subtab == 0) ? (local_team == 3 ? 2 : 3) : (local_team == 2 ? 2 : 3);
		}

		static float s_agent_yaw = 0.0f;
		static float s_agent_pitch = 0.0f;
		static bool s_agent_rotating = false;
		static std::string s_last_agent_model;

		systems::model_preview::request req{};
		std::string agent_name = "CS2 Agent";
		nemesis::preview3d::setup_agent_request(req, agent_name, target_team);

		if (s_last_agent_model != req.model_path)
		{
			s_last_agent_model = req.model_path;
			s_agent_yaw = 0.0f;
			s_agent_pitch = 0.0f;
			s_agent_rotating = false;
		}

		const auto& input = xui::ctx().input;
		const xui::rect team_btn_t{ preview_rect.x + 8.0f, preview_rect.y + 8.0f, 68.0f, 22.0f };
		const xui::rect team_btn_ct{ preview_rect.x + 80.0f, preview_rect.y + 8.0f, 68.0f, 22.0f };

		const bool over_team_buttons = team_btn_t.contains(input.mouse_x, input.mouse_y) ||
			team_btn_ct.contains(input.mouse_x, input.mouse_y);

		if (!input.rmb_down) s_agent_rotating = false;
		if (input.rmb_clicked && preview_rect.contains(input.mouse_x, input.mouse_y) && !over_team_buttons && !xui::ctx().overlay_blocking())
		{
			s_agent_rotating = true;
		}
		if (s_agent_rotating && input.rmb_down)
		{
			s_agent_yaw = std::remainder(s_agent_yaw + input.mouse_delta_x() * 0.5f, 360.0f);
			s_agent_pitch = std::clamp(s_agent_pitch + input.mouse_delta_y() * 0.35f, -35.0f, 35.0f);
		}

		req.yaw = s_agent_yaw;
		req.pitch = s_agent_pitch;
		req.x = static_cast<int>(std::round(preview_rect.x));
		req.y = static_cast<int>(std::round(preview_rect.y));
		req.width = static_cast<int>(std::round(preview_rect.w));
		req.height = static_cast<int>(std::round(preview_rect.h));
		const auto [vw, vh] = xdraw::viewport_size();
		req.screen_width = static_cast<int>(vw);
		req.screen_height = static_cast<int>(vh);
		req.background_rgb = (static_cast<std::uint32_t>(tokens::col_card.r) << 16) |
			(static_cast<std::uint32_t>(tokens::col_card.g) << 8) | tokens::col_card.b;
		req.visible = true;
		systems::g_model_preview.submit(std::move(req));

		auto& dl = xdraw::get(xdraw::layer::middle);
		dl.rect(preview_rect.x, preview_rect.y, preview_rect.w, preview_rect.h,
			tokens::col_border.alpha(160), xdraw::corner_radius{7.0f}, 1.0f);

		// CT/T switcher buttons
		auto draw_team_btn = [&](const xui::rect& r, const char* label, int team_val) {
			const bool active = (target_team == team_val);
			const bool hovered = input.in_rect(r) && !xui::ctx().overlay_blocking();
			if (hovered && input.mouse_clicked)
			{
				s_preview_team_choice = team_val;
			}
			const auto bg = active ? tokens::col_accent.alpha(55) : (hovered ? tokens::col_elevated.alpha(200) : tokens::col_card.alpha(170));
			dl.rect_filled(r.x, r.y, r.w, r.h, bg, xdraw::corner_radius{4.0f});
			dl.rect(r.x, r.y, r.w, r.h, active ? tokens::col_accent.alpha(180) : tokens::col_border.alpha(120), xdraw::corner_radius{4.0f}, 1.0f);
			const auto [lw, lh] = xdraw::measure_text(label);
			dl.text(r.x + (r.w - lw) * 0.5f, r.y + (r.h - lh) * 0.5f, label,
				active ? tokens::col_accent : (hovered ? tokens::col_text : tokens::col_text_dim));
		};

		draw_team_btn(team_btn_t, "Terrorist", 2);
		draw_team_btn(team_btn_ct, "Counter-T", 3);

		// Live projected ESP overlay
		dl.push_clip(preview_rect.x, preview_rect.y, preview_rect.w, preview_rect.h);
		const auto pose = systems::g_model_preview.get_agent_pose();
		const auto projected = nemesis::preview3d::project_agent(pose);
		nemesis::preview3d::draw_agent_esp_overlay(dl, projected, p.m_overlay[subtab], target_team, agent_name);
		dl.pop_clip();

		// ESP bottom bar with chevron > opening popup
		const float bar_y = col_right_y + preview_h + k_spacing;
		xui::layout::set_cursor(col_right_x - this->m_x, bar_y - this->m_y);

		auto& ov = p.m_overlay[subtab];
		xui::nav_row("esp", col_w, k_esp_bar_h);
		char popup_id[32]{};
		std::snprintf(popup_id, sizeof(popup_id), "##esp_options_popup_%d", subtab);

		enum class esp_subpage : int
		{
			main = 0,
			box,
			skeleton,
			health,
			ammo,
			name,
			weapon,
			flags,
			oof
		};
		static esp_subpage s_esp_page = esp_subpage::main;

		const auto popup_hash = xui::make_id(popup_id);
		if (!xui::overlays::is_open(popup_hash))
		{
			s_esp_page = esp_subpage::main;
		}

		auto draw_gear_btn = []( const char* id_str ) -> bool
		{
			auto win = xui::layout::current_window( );
			if ( !win )
				return false;

			constexpr auto dot_area_w = 16.0f;
			const auto dot_area_h = win->last_item.h;
			float dot_x{ 0.0f };
			if ( win->last_item_is_toggle )
			{
				dot_x = win->last_toggle_x - dot_area_w - 6.0f;
			}
			else
			{
				dot_x = win->bounds.x + win->last_item.x + win->last_item.w - 12.0f - dot_area_w;
			}
			const auto dot_local_y = win->last_item.y;
			const auto dot_abs = xui::rect{ std::floorf( dot_x ), std::floorf( win->bounds.y + dot_local_y ), dot_area_w, dot_area_h };

			auto& c = xui::ctx( );
			const auto id = xui::make_id( id_str );
			const auto hovered = !c.overlay_blocking( ) && c.input.in_rect( dot_abs );
			if ( hovered )
			{
				xui::set_hovered_tooltip( "Settings", "Configure advanced options for this feature" );
			}
			const auto hover_anim = xui::anim::lerp( id + 1, hovered ? 1.0f : 0.0f, 12.0f );
			const auto dot_col = xui::lerp( c.style.text_dim, c.style.accent, hover_anim );

			auto& dl = xui::draw::current( );
			const auto cx = dot_abs.x + dot_area_w * 0.5f;
			const auto cy = dot_abs.y + dot_area_h * 0.5f;
			xui::draw_gear( dl, cx, cy, dot_col, 11.0f );

			if ( hovered && c.input.mouse_clicked )
			{
				c.input.mouse_clicked = false;
				return true;
			}
			return false;
		};

		if (xui::begin_popup(popup_id, 270.0f, nullptr, true))
		{
			if ( s_esp_page != esp_subpage::main )
			{
				xui::push_style_var( xui::style_var::item_spacing_y, 4.0f );
				if ( xui::button( "<  Back", 70.0f ) )
				{
					s_esp_page = esp_subpage::main;
				}
				xui::layout::same_line( );
				const char* title = "";
				switch ( s_esp_page )
				{
				case esp_subpage::box: title = "box options"; break;
				case esp_subpage::skeleton: title = "skeleton options"; break;
				case esp_subpage::health: title = "health bar options"; break;
				case esp_subpage::ammo: title = "ammo bar options"; break;
				case esp_subpage::name: title = "name options"; break;
				case esp_subpage::weapon: title = "weapon options"; break;
				case esp_subpage::flags: title = "flags options"; break;
				case esp_subpage::oof: title = "oof arrows options"; break;
				default: break;
				}
				xui::text( title, tokens::col_text );
				xui::layout::separator( );

				switch ( s_esp_page )
				{
				case esp_subpage::box:
				{
					constexpr const char* box_styles[]{ "full", "cornered" };
					xui::combo( "style##box", ov.m_box.style.value, box_styles, 2 );
					xui::checkbox( "fill", ov.m_box.fill );
					xui::checkbox( "outline", ov.m_box.outline );
					xui::slider_float( "corner length", ov.m_box.corner_length, 2.0f, 20.0f, "%.0f" );
					xui::color_picker( "visible color##box", ov.m_box.visible_color );
					xui::color_picker( "occluded color##box", ov.m_box.occluded_color );
					break;
				}
				case esp_subpage::skeleton:
				{
					constexpr const char* skel_modes[]{ "normal", "backtrack" };
					xui::combo( "mode##skel", ov.m_skeleton.type.value, skel_modes, 2 );
					xui::slider_float( "thickness##skel", ov.m_skeleton.thickness, 0.5f, 4.0f, "%.1f" );
					xui::color_picker( "visible color##skel", ov.m_skeleton.visible_color );
					xui::color_picker( "occluded color##skel", ov.m_skeleton.occluded_color );
					break;
				}
				case esp_subpage::health:
				{
					constexpr const char* bar_positions[]{ "left", "top", "bottom" };
					xui::combo( "position##hp", ov.m_health_bar.position.value, bar_positions, 3 );
					xui::checkbox( "outline##hp", ov.m_health_bar.outline_setting );
					xui::checkbox( "gradient##hp", ov.m_health_bar.gradient );
					xui::checkbox( "show value##hp", ov.m_health_bar.show_value);
					xui::checkbox( "glow##hp", ov.m_health_bar.glow );
					xui::color_picker( "full color##hp", ov.m_health_bar.full_color );
					xui::color_picker( "low color##hp", ov.m_health_bar.low_color );
					xui::color_picker( "background##hp", ov.m_health_bar.background_color );
					xui::color_picker( "outline color##hp", ov.m_health_bar.outline_color );
					xui::color_picker( "text color##hp", ov.m_health_bar.text_color );
					xui::color_picker( "glow color##hp", ov.m_health_bar.glow_color );
					xui::slider_float( "glow strength##hp", ov.m_health_bar.glow_strength, 0.1f, 1.0f, "%.2f" );
					break;
				}
				case esp_subpage::ammo:
				{
					constexpr const char* bar_positions[]{ "left", "top", "bottom" };
					xui::combo( "position##ammo", ov.m_ammo_bar.position.value, bar_positions, 3 );
					xui::checkbox( "outline##ammo", ov.m_ammo_bar.outline_setting );
					xui::checkbox( "gradient##ammo", ov.m_ammo_bar.gradient );
					xui::checkbox( "show value##ammo", ov.m_ammo_bar.show_value );
					xui::checkbox( "glow##ammo", ov.m_ammo_bar.glow );
					xui::color_picker( "full color##ammo", ov.m_ammo_bar.full_color );
					xui::color_picker( "low color##ammo", ov.m_ammo_bar.low_color );
					xui::color_picker( "background##ammo", ov.m_ammo_bar.background_color );
					xui::color_picker( "outline color##ammo", ov.m_ammo_bar.outline_color );
					xui::color_picker( "text color##ammo", ov.m_ammo_bar.text_color );
					xui::color_picker( "glow color##ammo", ov.m_ammo_bar.glow_color );
					xui::slider_float( "glow strength##ammo", ov.m_ammo_bar.glow_strength, 0.1f, 1.0f, "%.2f" );
					break;
				}
				case esp_subpage::name:
				{
					xui::color_picker( "color##name", ov.m_name.color );
					break;
				}
				case esp_subpage::weapon:
				{
					constexpr const char* display_types[]{ "text", "icon", "text + icon" };
					xui::combo( "display##wep", ov.m_weapon.display.value, display_types, 3 );
					xui::color_picker( "text color##wep", ov.m_weapon.text_color );
					xui::color_picker( "icon color##wep", ov.m_weapon.icon_color );
					break;
				}
				case esp_subpage::flags:
				{
					constexpr const char* flag_names[]{ "money", "armor", "kit", "scoped", "defusing", "flashed", "ping", "distance", "c4" };
					xui::multicombo( "flags##mc", ov.m_info_flags.flags, flag_names, settings::esp::player::overlay::info_flags::count );
					xui::color_picker( "money##flags", ov.m_info_flags.money_color );
					xui::color_picker( "armor##flags", ov.m_info_flags.armor_color );
					xui::color_picker( "kit##flags", ov.m_info_flags.kit_color );
					xui::color_picker( "scoped##flags", ov.m_info_flags.scoped_color );
					xui::color_picker( "defusing##flags", ov.m_info_flags.defusing_color );
					xui::color_picker( "flashed##flags", ov.m_info_flags.flashed_color );
					xui::color_picker( "distance##flags", ov.m_info_flags.distance_color );
					break;
				}
				case esp_subpage::oof:
				{
					xui::checkbox( "glow##oof", ov.m_oof_arrow.glow );
					xui::slider_float( "width##oof", ov.m_oof_arrow.width, 4.0f, 40.0f, "%.0f" );
					xui::slider_float( "height##oof", ov.m_oof_arrow.height, 4.0f, 40.0f, "%.0f" );
					xui::slider_float( "radius x##oof", ov.m_oof_arrow.radius_x, 50.0f, 600.0f, "%.0f" );
					xui::slider_float( "radius y##oof", ov.m_oof_arrow.radius_y, 50.0f, 600.0f, "%.0f" );
					xui::slider_float( "glow strength##oof", ov.m_oof_arrow.glow_strength, 0.1f, 1.0f, "%.2f" );
					xui::color_picker( "visible color##oof", ov.m_oof_arrow.visible_color );
					xui::color_picker( "occluded color##oof", ov.m_oof_arrow.occluded_color );
					break;
				}
				default: break;
				}

				xui::pop_style_var( );
			}
			else
			{
				xui::push_style_var(xui::style_var::item_spacing_y, 4.0f);
				xui::toggle("enable", ov.enabled);
				xui::layout::separator();

				xui::checkbox("only visible", ov.only_visible);
				if (ov.only_visible.value)
				{
					xui::checkbox("include sounds", ov.sound_reveal);
					if (ov.sound_reveal.value)
					{
						xui::slider_float("sound duration", ov.sound_duration, 0.1f, 5.0f, "%.1f s");
						xui::slider_float("sound distance", ov.sound_distance, 1.0f, 100.0f, "%.0f m");
					}
				}

				xui::layout::separator();

				xui::toggle("box", ov.m_box.enabled);
				if (draw_gear_btn("##gear_box")) s_esp_page = esp_subpage::box;

				xui::toggle("skeleton", ov.m_skeleton.enabled);
				if (draw_gear_btn("##gear_skeleton")) s_esp_page = esp_subpage::skeleton;

				xui::toggle("health bar", ov.m_health_bar.enabled);
				if (draw_gear_btn("##gear_health")) s_esp_page = esp_subpage::health;

				xui::toggle("ammo bar", ov.m_ammo_bar.enabled);
				if (draw_gear_btn("##gear_ammo")) s_esp_page = esp_subpage::ammo;

				xui::toggle("name", ov.m_name.enabled);
				if (draw_gear_btn("##gear_name")) s_esp_page = esp_subpage::name;

				xui::toggle("weapon", ov.m_weapon.enabled);
				if (draw_gear_btn("##gear_weapon")) s_esp_page = esp_subpage::weapon;

				xui::toggle("flags", ov.m_info_flags.enabled);
				if (draw_gear_btn("##gear_flags")) s_esp_page = esp_subpage::flags;

				xui::toggle("oof arrows", ov.m_oof_arrow.enabled);
				if (draw_gear_btn("##gear_oof")) s_esp_page = esp_subpage::oof;

				xui::pop_style_var();
			}
			xui::end_popup();
		}
		}
	else
	{
		xui::layout::set_cursor(col_right_x - this->m_x, col_right_y - this->m_y);
		if (xui::begin_child("##viewmodel", col_w, column_h, true))
		{
			detail::draw_chams_config("weapon chams", "vm_weapon", esp.m_viewmodel.weapon);
			xui::layout::separator();
			detail::draw_chams_config("arms chams", "vm_arms", esp.m_viewmodel.arms);
			xui::end_child();
		}
	}
}

} // namespace rendering
