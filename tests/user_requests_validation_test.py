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

if __name__ == "__main__":
    unittest.main(verbosity=2)
