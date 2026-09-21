"""Validation tests for all 5 user requirements:
1. Legitbot autoscope config persistence & sync
5. Scoreboard TAB 'M' indicator team color
6. Ragebot autoscope visibility restricted to snipers
8. Skinchanger name tag system removed
12. Custom hitsounds/killsounds (mintaly/sounds) removed
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

def read_source(rel_path: str) -> str:
    return (ROOT / rel_path).read_text(encoding="utf-8")

class UserRequestsValidation(unittest.TestCase):
    # 1. Legitbot autoscope config saving
    def test_item_1_legit_autoscope_persistence(self):
        widgets = read_source("core/rendering/impl/widgets.cpp")
        self.assertIn("s == &wg.auto_scope", widgets, "auto_scope must be recognized in legit group widgets")

        settings = read_source("core/settings.hpp")
        self.assertIn("this->auto_scope.value = other.auto_scope.value;", settings)
        self.assertIn("this->auto_scope.bind = other.auto_scope.bind;", settings)
        self.assertIn("g_combat.m_legitbot.weapons[i].cfg.copy_values_from(g_combat.m_legitbot.groups[grp_idx]);", settings)

        legit_menu = read_source("core/rendering/impl/menu/menu.legitbot.cpp")
        self.assertIn("ow.cfg.auto_scope.value != grp_cfg.auto_scope.value", legit_menu)

        config = read_source("external/config.hpp")
        self.assertIn("k_legit_scope_alt1", config)
        self.assertIn('detail::make_key("legitbot", "auto scope")', config)
        self.assertIn('detail::make_key("legitbot - snipers", "auto scope")', config)

    # 5. TAB scoreboard sync 'M' letter team color
    def test_item_5_scoreboard_tab_team_color(self):
        tab = read_source("core/features/misc/impl/scoreboard_weapons.cpp")
        self.assertIn('{"team", team}', tab, "send_player_indicator must transmit controller team")
        self.assertTrue('teamColor = "#DE9B35"; // Terrorists' in tab or 'teamColor = "#E9D18A"; // Terrorists' in tab)
        self.assertTrue('teamColor = "#5D79AE"; // Counter-Terrorists' in tab or 'teamColor = "#B6D4EE"; // Counter-Terrorists' in tab)
        self.assertIn('updateIndicator(xuid, account_id, name, show, team)', tab)
        self.assertIn('label.style.color = teamColor;', tab)

    # 6. Ragebot autoscope sniper visibility
    def test_item_6_rage_autoscope_sniper_only(self):
        rage_menu = read_source("core/rendering/impl/menu/menu.ragebot.cpp")
        self.assertIn("const bool is_sniper =", rage_menu)
        self.assertIn("group_idx == 4", rage_menu)
        self.assertIn('draw_animated_item( "rb_auto_scope_anim", is_sniper', rage_menu)
        self.assertIn('xui::toggle( "Auto Scope", autos.scope );', rage_menu)

    # 8. Skinchanger name tag system removal
    def test_item_8_name_tag_removal(self):
        skin_menu = read_source("core/rendering/impl/menu/menu.skins.cpp")
        self.assertNotIn("Name Tag", skin_menu)
        self.assertNotIn("skin.name_tag", skin_menu)

        name_tag_h = read_source("core/features/changer/name_tag.hpp")
        self.assertIn("return std::nullopt;", name_tag_h)
        self.assertNotIn("m_szCustomName", name_tag_h)
        self.assertNotIn("m_szCustomNameOverride", name_tag_h)

        cosmetics = read_source("core/features/changer/cosmetic_attributes.hpp")
        self.assertIn("skin.name_tag.clear();", cosmetics)

        skin_sync = read_source("core/features/changer/skin_sync.cpp")
        self.assertIn('{"n", ""}', skin_sync)

    # 12. Custom hitsounds and killsounds (mintaly/sounds) removal
    def test_item_12_custom_sounds_removal(self):
        menu_misc = read_source("core/rendering/impl/menu/menu.misc.cpp")
        self.assertNotIn('"custom"', menu_misc)
        self.assertNotIn('draw_custom_sound_picker', menu_misc)

        settings = read_source("core/settings.hpp")
        self.assertNotIn('custom };', settings)
        self.assertNotIn('custom_hit_sound', settings)
        self.assertNotIn('custom_death_sound', settings)

        misc_h = read_source("core/features/misc/misc.hpp")
        self.assertNotIn('list_custom_sounds', misc_h)
        self.assertNotIn('custom_sounds_directory_narrow', misc_h)
        self.assertNotIn('play_custom_sound', misc_h)

        impacts = read_source("core/features/misc/impl/impacts.cpp")
        self.assertNotIn('C:\\\\mintaly\\\\sounds', impacts)
        self.assertNotIn('mintaly/sounds', impacts)
        self.assertNotIn('load_wav', impacts)
        self.assertNotIn('resolve_sound_path', impacts)
        self.assertNotIn('list_custom_sounds', impacts)
        self.assertNotIn('custom_sounds_directory_narrow', impacts)
        self.assertNotIn('play_custom_sound', impacts)

    # 14. Skin sync toggle state stored in config
    def test_item_14_sync_state_stored_in_config(self):
        settings = read_source("core/settings.hpp")
        self.assertIn('config::val<bool> sync_enabled{ true, "changer", "sync enabled" };', settings)

        menu_core = read_source("core/rendering/impl/menu/menu.core.cpp")
        self.assertIn('settings::g_changer.sync_enabled.value = !settings::g_changer.sync_enabled.value;', menu_core)
        self.assertIn('features::changer::g_skin_sync.on_sync_toggled();', menu_core)
        self.assertIn('config::registry::request_save_active();', menu_core)

        menu_cfg = read_source("core/rendering/impl/menu/menu.config.cpp")
        self.assertIn('features::changer::g_skin_sync.on_sync_toggled( );', menu_cfg)

        cfg_h = read_source("external/config.hpp")
        self.assertIn('detail::make_key("changer", "sync enabled")', cfg_h)

    # 15. Safe Mode toggle in user dropdown (bottom) and removed from topbar
    def test_item_15_safe_mode_in_dropdown_and_removed_from_topbar(self):
        menu_core = read_source("core/rendering/impl/menu/menu.core.cpp")
        self.assertNotIn('mode_btn_x = util_x - 10.0f - mode_btn_w;', menu_core)
        self.assertIn('const auto mode_left = util_x;', menu_core)
        self.assertIn('const float main_h = 261.0f;', menu_core)
        self.assertIn('usr_safe_sw', menu_core)
        self.assertIn('settings::set_safe_mode(!safe_mode::active());', menu_core)
        self.assertIn('"Safe mode"', menu_core)

    # 22. Theme preset 0 is Dark theme
    def test_item_22_theme_preset_0_restoration(self):
        theme_h = read_source("core/rendering/theme.hpp")
        self.assertIn('{ "Dark",    {168, 178, 194}', theme_h)
        xdraw_h = read_source("external/xdraw/xdraw.hpp")
        self.assertIn('inline xdraw::color col_accent{ 168, 178, 194, 255 };', xdraw_h)

    # 23. Movable UNSAFE MODE HUD window styled identically to spectator list
    def test_item_23_unsafe_mode_draggable_hud(self):
        settings = read_source("core/settings.hpp")
        self.assertIn('config::val<float> unsafe_mode_x{ -1.0f, "widgets", "unsafe mode x" };', settings)
        self.assertIn('config::val<float> unsafe_mode_y{ -1.0f, "widgets", "unsafe mode y" };', settings)

        rendering_h = read_source("core/rendering/rendering.hpp")
        self.assertIn('void unsafe_mode_hud(xdraw::draw_list& draw_list);', rendering_h)
        self.assertIn('bool m_unsafe_mode_hud_hovered{ false };', rendering_h)
        self.assertIn('bool m_unsafe_mode_hud_dragging{ false };', rendering_h)

        widgets = read_source("core/rendering/impl/widgets.cpp")
        self.assertIn('this->unsafe_mode_hud( dl );', widgets)
        self.assertIn('void widgets::unsafe_mode_hud( xdraw::draw_list& draw_list )', widgets)
        self.assertIn('"UNSAFE MODE"', widgets)
        self.assertIn('widgets_cfg.unsafe_mode_x = current_x;', widgets)
        self.assertIn('widgets_cfg.unsafe_mode_y = current_y;', widgets)
        self.assertIn('if ( !safe_mode::active( ) )', widgets)
        self.assertIn('!this->m_unsafe_mode_hud_dragging', widgets)

    # 16. Gear settings icon hidden on locked SAFE controls
    def test_item_16_gear_hidden_on_safe_controls(self):
        xui = read_source("external/xdraw/xui/xui.cpp")
        self.assertIn('if (win->last_item_locked) {', xui)
        self.assertIn('if (auto* popup = overlays::find(make_id(label))) popup->force_close();', xui)
        self.assertIn('win->last_item_locked = true;', xui)

    # 17. Onshot chams duration slider moved to gear popup
    def test_item_17_onshot_chams_duration_in_gear(self):
        player = read_source("core/rendering/impl/menu/menu.player.cpp")
        self.assertIn('detail::draw_chams_config("onshot chams", "os", p.m_chams.onshot, false, true, &p.m_chams.onshot_fade_time.value);', player)
        common = read_source("core/rendering/impl/menu/menu.chams_common.hpp")
        self.assertIn('if (duration) {', common)
        self.assertIn('xui::slider_float("duration##ft", *duration, 0.05f, 5.0f, "%.2f s");', common)

    # 18. Weapon skin viewmodel application fix
    def test_item_18_weapon_skin_viewmodel_application(self):
        guns = read_source("core/features/changer/impl/guns.cpp")
        self.assertIn('bool update_view_model( std::uintptr_t pawn, const econ_item_system::paint_kit* pk, bool force = false );', read_source("core/features/changer/changer.hpp"))
        self.assertIn('this->update_view_model(pawn, pk, true)', guns)
        self.assertIn('if (force || !current_name || !cosmetic_model::matches(memory::read_string(current_name), target))', guns)

    # 19. Smooth animated hiding of Ragebot sidebar tab
    def test_item_19_smooth_ragebot_sidebar_collapse(self):
        menu_core = read_source("core/rendering/impl/menu/menu.core.cpp")
        self.assertIn('rage_sidebar_reveal', menu_core)
        self.assertIn('safe_mode::active() && this->m_tab == 0 && rage_reveal < 0.15f', menu_core)
        self.assertIn('dl.push_clip(sb_x, curr_y, sb_w, 42.0f * reveal);', menu_core)
        self.assertIn('curr_y += 42.0f * reveal;', menu_core)

    # 20. Safe mode automatically disables all unsafe features
    def test_item_20_safe_mode_auto_disables_features(self):
        settings = read_source("core/settings.hpp")
        self.assertIn('inline void enforce_safe_mode()', settings)
        self.assertIn('g_combat.m_ragebot.enabled.value = false;', settings)
        self.assertIn('aa.enabled.value = false;', settings)
        self.assertIn('g_combat.m_quickpeek.enabled.value = false;', settings)
        self.assertIn('g_movement.airstrafe.value = false;', settings)

    # 21. Vector icons added to profile Theme and all Watermark options
    def test_item_21_icons_in_theme_and_watermark(self):
        menu_core = read_source("core/rendering/impl/menu/menu.core.cpp")
        self.assertIn('draw_toggle_row("Enabled", m.m_watermark.enabled', menu_core)
        self.assertIn('draw_toggle_row("Steam Username", m.m_watermark.show_user', menu_core)
        self.assertIn('draw_toggle_row("FPS", m.m_watermark.show_fps', menu_core)
        self.assertIn('draw_toggle_row("Ping", m.m_watermark.show_ping', menu_core)
        self.assertIn('draw_toggle_row("Loss", m.m_watermark.show_loss', menu_core)
        self.assertIn('draw_toggle_row("Clock", m.m_watermark.show_time', menu_core)
        self.assertIn('draw_toggle_row("Map", m.m_watermark.show_map', menu_core)
        self.assertIn('draw_toggle_row("Tick Rate", m.m_watermark.show_tick', menu_core)
        self.assertIn('draw_toggle_row("Velocity", m.m_watermark.show_velocity', menu_core)

    # 22. Spectator custom knife animations properly bound instead of standard knife
    def test_item_22_spectator_knife_animations_binding(self):
        hud_h = read_source("core/features/changer/hud_weapon.hpp")
        self.assertIn("model_mismatch", hud_h)
        self.assertIn("(bind || model_mismatch) && is_firstperson", hud_h)
        self.assertIn("PATTERN(patterns::weapon_get_viewmodel)", hud_h)

        knives_cpp = read_source("core/features/changer/impl/knives.cpp")
        self.assertIn("bool knives::update_view_model( std::uintptr_t pawn, const econ_item_system::paint_kit* pk, bool force )", knives_cpp)
        self.assertIn("return hud_weapon::update( pawn, pk && pk->legacy_model ? std::uint64_t{2} : std::uint64_t{1}, force );", knives_cpp)

        changer_h = read_source("core/features/changer/changer.hpp")
        self.assertIn("bool update_view_model( std::uintptr_t pawn, const econ_item_system::paint_kit* pk, bool force = false );", changer_h)

if __name__ == "__main__":
    unittest.main(verbosity=2)

