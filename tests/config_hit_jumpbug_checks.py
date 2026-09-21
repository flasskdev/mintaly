"""Source-wiring/audit checks only, not C++ execution or a CS2/server test."""
from pathlib import Path
import re
import unittest
ROOT = Path(__file__).resolve().parents[1]
def source(path): return (ROOT / path).read_text(encoding='utf-8')
def body(text, start, end): return text.split(start, 1)[1].split(end, 1)[0]

class ConfigAndBinds(unittest.TestCase):
    def test_every_rage_numeric_and_hitbox_field_is_registered(self):
        s = body(source('core/settings.hpp'), 'struct weapon_group', 'struct individual_weapon')
        declared = set(re.findall(r'config::(?:val<[^>]+>|bools<[^>]+>)\s+(\w+)', s))
        registered = set(re.findall(r'this->(\w+)\.reg\(', s))
        self.assertEqual(declared, registered)
        self.assertEqual(len(declared), 7)
    def test_every_rage_boolean_bind_has_a_per_weapon_category(self):
        s = body(source('core/settings.hpp'), 'struct weapon_group', 'struct individual_weapon')
        declared = set(re.findall(r'xui::setting\s+(\w+)', s))
        categories = set(re.findall(r'this->(\w+)\.category = s;', s))
        self.assertEqual(declared, categories)
        self.assertEqual(len(declared), 10)
    def test_all_rage_group_and_weapon_instances_initialize(self):
        s = body(source('core/settings.hpp'), 'ragebot()', '[[nodiscard]] bool is_weapon_overridden')
        self.assertIn('i < k_group_count', s)
        self.assertIn('i < k_weapon_count', s)
        self.assertIn('this->groups[i].init(', s)
        self.assertIn('this->weapons[i].init(', s)
    def test_both_loaders_reset_before_field_deserialization(self):
        s = source('external/config.hpp')
        for start, end in [('inline bool from_json(', 'inline nlohmann::json to_json_delta'),
                           ('inline bool from_json_delta(', 'namespace registry')]:
            part = body(s, start, end)
            self.assertLess(part.index('xui::slider_binds::reset()'), part.index('serial::json_to_field'))
            self.assertIn('xui::binds::reset_runtime()', part)
    def test_reset_button_discards_active_slider_binds_first(self):
        s = body(source('core/rendering/impl/menu/menu.config.cpp'), 'void reset_defaults', '} // namespace detail')
        self.assertLess(s.index('xui::slider_binds::reset'), s.index('serial::json_to_field'))
        self.assertIn('xui::binds::reset_runtime', s)
    def test_deserialize_replaces_even_empty_or_invalid_bind_list(self):
        s = body(source('external/xdraw/xui/xui.cpp'), 'void deserialize(std::string_view s)', 'void reset( )')
        self.assertLess(s.index('reset();'), s.index('json::parse'))
        self.assertIn('if (!document.is_array()) return;', s)
    def test_slider_persistence_has_stable_setting_key(self):
        s = source('external/xdraw/xui/xui.cpp')
        self.assertIn('item["field"] = entry->config_key', s)
        self.assertIn('field_id(known->second)', s)
        self.assertIn('store.config_keys[ptr] = key', s)
        self.assertNotIn('e->id = id;', s)
    def test_numeric_bind_targets_register_before_tabs_open(self):
        s = body(source('external/config.hpp'), 'inline void initialize()', 'inline nlohmann::json to_json()')
        self.assertIn('xui::slider_binds::register_field(f.ptr, f.key', s)
        self.assertIn('field_type::int_val', s)
        self.assertIn('field_type::float_val', s)
    def test_legacy_bind_import_waits_for_correct_pointer(self):
        s = body(source('external/xdraw/xui/xui.cpp'), 'slider_bind_entry* get_or_create', 'void register_field')
        self.assertIn('!old->second->ptr', s)
        self.assertIn('old->second->count = 0', s)
        self.assertIn('alias->second = nullptr', s)
    def test_save_does_not_persist_temporary_numeric_override(self):
        s = body(source('external/config.hpp'), 'inline nlohmann::json field_to_json', 'inline void json_to_field')
        self.assertEqual(s.count('bind->has_base_value'), 2)
        self.assertIn('std::round(bind->base_value)', s)
    def test_numeric_base_keeps_int_precision(self):
        self.assertIn('double base_value', source('external/xdraw/xui/xui.hpp'))
        s = source('external/xdraw/xui/xui.cpp')
        self.assertNotIn('roundf( entry->base_value )', s)
    def test_invalid_hotkeys_and_modes_are_rejected(self):
        s = source('external/xdraw/xui/xui.cpp')
        self.assertIn('key <= 0 || key >= 256 || mode < 0 || mode > 2', s)
        self.assertIn('b.key <= 0 || b.key >= 256', s)
        self.assertIn('!std::isfinite(value)', s)
    def test_pressed_keys_are_reprimed_after_switch(self):
        s = source('external/xdraw/xui/xui.cpp')
        self.assertIn('reg.prime_keys = true;', s)
        self.assertIn('store.prime_keys = true;', s)
        self.assertIn('std::exchange( reg.prime_keys, false )', s)
        self.assertIn('std::exchange(store.prime_keys, false)', s)
    def test_rendering_does_not_reenable_weapon_override(self):
        for name in ('ragebot', 'legitbot'):
            self.assertNotIn('has_bind', source(f'core/rendering/impl/menu/menu.{name}.cpp'))
    def test_old_custom_field_addresses_are_detached_on_reset(self):
        s = body(source('external/xdraw/xui/xui.cpp'), '\n\t\tvoid reset( )', '} // namespace slider_binds')
        self.assertIn('!store.config_keys.contains(entry->ptr)', s)
        self.assertIn('entry->ptr = nullptr', s)

class HitEffectsAndMovement(unittest.TestCase):
    def test_onshot_has_exactly_one_confirmed_hit_trigger(self):
        files = list((ROOT / 'core').rglob('*.cpp'))
        calls = [(p, m.start()) for p in files for m in re.finditer(r'g_chams\.os\s*\(\s*\)\.push\s*\(', p.read_text())]
        self.assertEqual(len(calls), 1)
        self.assertEqual(calls[0][0].name, 'impacts.cpp')
        s = body(source('core/features/misc/impl/impacts.cpp'), 'void impacts::on_player_hurt', 'const auto& cfg = settings::g_misc.m_impacts;')
        self.assertIn('data.victim_pawn && data.damage > 0', s)
    def test_hit_effect_does_not_require_rage_records(self):
        s = body(source('core/features/esp/player/player.chams.cpp'), 'void chams::onshot::push', 'void chams::onshot::update')
        self.assertNotIn('get_valid_records', s)
        self.assertIn('memory::safe_read<systems::bones::data>', s)
        self.assertIn('lookup(handle) != pawn', s)
    def test_effect_queue_stores_time_and_duration_at_hit(self):
        s = source('core/features/esp/player/player.chams.cpp')
        self.assertIn('pending.hit_time = clock::now()', s)
        self.assertIn('std::clamp(duration, 0.05f, 5.0f)', s)
        self.assertIn('e.hit_time = pending.hit_time', s)
        self.assertIn('lookup(pending.pawn_handle) != pawn', s)
    def test_effect_expiry_runs_every_render_frame(self):
        s = source('core/hooks/impl/cheat.cpp')
        self.assertIn('Expire hit ghosts every render frame', s)
        stage = s[s.index('if ( stage == 12 )\n\t\t{\n            // Expire'):]
        self.assertLess(stage.index('g_chams.os().update()'), stage.index('g_view.update_matrix'))
    def test_ghost_does_not_need_live_model_draw_and_has_no_unfaded_base(self):
        hook = source('core/hooks/impl/cheat.cpp')
        self.assertIn('g_chams.os().get_pawn(scene_object)', hook)
        s = source('core/features/esp/player/player.chams.cpp')
        self.assertIn('if (is_hit_ghost) return true;', s)
        self.assertIn('needs_original_base && !suppress_fill && !is_hit_ghost', s)
    def test_duration_ui_keeps_fractions_and_existing_config_key(self):
        self.assertIn('onshot_fade_time{ 0.25f, "chams onshot", "fade time" }', source('core/settings.hpp'))
        self.assertIn('0.05f, 5.0f, "%.2f s"', source('core/rendering/impl/menu/menu.player.cpp'))
    def test_jumpbug_cannot_lose_jump_to_hidden_cfg_flag(self):
        s = source('core/features/movement/impl/jumpbug.cpp')
        self.assertIn('const bool include_jump = true;', s)
        self.assertNotIn('jumpbug_include_jump_steps.value', s)
        self.assertIn('jumpbug_command::make(cmd->buttons.value, jump, duck, release, include_jump)', s)
    def test_jumpbug_prepares_on_descent_without_damage_speed_gate(self):
        s = source('core/features/movement/impl/jumpbug.cpp')
        self.assertNotIn('pre.networked_velocity.z > -200.0f', s)
        self.assertIn('pre.networked_velocity.z >= 0.0f', s)
        self.assertIn('support.fraction >= 0.0f', s)
    def test_jumpbug_does_not_fake_health_or_fall_velocity(self):
        s = source('core/features/movement/impl/jumpbug.cpp')
        self.assertNotIn('m_iHealth', s)
        self.assertNotIn('m_flFallVelocity', s)

if __name__ == '__main__': unittest.main(verbosity=2)
