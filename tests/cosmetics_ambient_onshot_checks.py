"""Source-wiring regressions; not a C++ build, FPS benchmark or game test."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

def source(path):
    return (ROOT / path).read_text(encoding='utf-8')

class CosmeticAmbientHitChecks(unittest.TestCase):
    def test_one_match_cosmetic_pass(self):
        hook = source('core/hooks/impl/cheat.cpp')
        block = hook.split('// One cosmetic pass', 1)[1].split('// Process impacts', 1)[0]
        self.assertIn('stage == 12 &&', block)
        self.assertIn('addresses::globals::game_rules', block)
        for feature in ('agents', 'knives', 'guns', 'gloves'):
            self.assertEqual(block.count(f'g_{feature}.on_frame_stage_notify( );'), 1)
        self.assertIn('g_guns.on_render_start( );', block)
        self.assertNotIn('preview_scene::refresh(', hook)

    def test_unchanged_hud_does_not_rebind_materials(self):
        guns = source('core/features/changer/impl/guns.cpp')
        render = guns.split('void guns::on_render_start', 1)[1].split('std::uintptr_t guns::find_hud_model_weapon', 1)[0]
        fast = render.split('if (!refresh)', 1)[1].split('if (!this->update_view_model', 1)[0]
        self.assertIn('entity_guard::set_mesh', fast)
        self.assertIn('return;', fast)
        self.assertNotIn('update_view_model', fast)
        guard = source('core/features/changer/entity_guard.hpp')
        self.assertIn('if (applied && *applied == mask) return true;', guard)
        self.assertIn('m_remote_frame_counter == 0 && this->m_remote_player_index == 0', guns)

    def test_music_is_requeued_after_reset(self):
        hook = source('core/hooks/impl/cheat.cpp')
        process = hook.split('void cheat::process_lobby_music', 1)[1].split('void __fastcall cheat::play_music', 1)[0]
        self.assertLess(process.index('settings::g_changer.music.id'), process.index('g_lobby_music_requests.next'))
        self.assertIn('g_lobby_music_requests.submit', process)
        self.assertIn('source_for( GetCurrentThreadId( ) )', process)
        self.assertNotIn('volume =', process)

    def test_same_map_still_resolves_current_settings(self):
        settings = source('core/settings.hpp')
        update = settings.split('void update_active(', 1)[1].split('copy_weather(this->m_weather, source->m_weather);', 1)[0]
        self.assertNotIn('return;', update)
        self.assertIn('override_map.value', update)
        self.assertIn('copy_scene(this->m_scene, source->m_scene);', update)

    def test_onshot_uses_actual_victim_and_duration(self):
        chams = source('core/features/esp/player/player.chams.cpp')
        hit = chams.split('void chams::onshot::push', 1)[1].split('void chams::apply_layer', 1)[0]
        self.assertIn('m_entries[pawn] = {handle, clock::now(), seconds};', hit)
        self.assertIn('std::clamp(duration, 0.05f, 5.0f)', hit)
        self.assertIn('lookup(hit.pawn_handle) == pawn', hit)
        self.assertIn('count() < hit.duration', hit)
        self.assertNotIn('setup_bones', hit)
        self.assertNotIn('e.create', hit)
        self.assertIn('apply_config( onshot, scene_object );', chams)
        self.assertNotIn('is_hit_ghost', chams)
        impacts = source('core/features/misc/impl/impacts.cpp')
        self.assertIn('if (data.victim_pawn && data.damage > 0)', impacts)
        self.assertIn('g_chams.os().push(data.victim_pawn)', impacts)

if __name__ == '__main__':
    unittest.main(verbosity=2)
