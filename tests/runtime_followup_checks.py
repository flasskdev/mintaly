"""Source-wiring checks only. Not compilation, a benchmark, or a CS2 test."""
from pathlib import Path
import unittest
ROOT = Path(__file__).resolve().parents[1]
def source(path): return (ROOT / path).read_text(encoding='utf-8')
class RuntimeFollowupChecks(unittest.TestCase):
    def test_music_publication_is_independent_of_ui_catalog(self):
        s = source('core/hooks/impl/cheat.cpp').split('void cheat::trigger_lobby_music', 1)[1].split('void cheat::process_lobby_music', 1)[0]
        self.assertIn('submit( { kit_id, {} } )', s)
        self.assertNotIn('find_music_kit', s)
        self.assertNotIn('memory::call', s)
    def test_preview_has_event_invalidation_and_poll_fallback(self):
        s = source('core/features/changer/preview_scene.hpp')
        self.assertIn('std::atomic<std::uint64_t> scene_revision', s)
        self.assertIn('revision != scanned_revision || now >= fallback_scan', s)
        self.assertIn('scanned_revision = revision;', s)
        self.assertIn('? 1000 : 100', s)
        self.assertIn('i < systems::entities::entity_slot_count', s)
        change = s.split('inline void on_entity_changed', 1)[1].split('// Called only after', 1)[0]
        self.assertIn('is_scene_candidate', change)
        self.assertIn('fetch_add', change)
    def test_entity_invalidation_uses_live_objects(self):
        s = source('core/hooks/impl/cheat.cpp')
        add = s.split('void __fastcall cheat::add_entity', 1)[1].split('void __fastcall cheat::remove_entity', 1)[0]
        self.assertGreater(add.index('on_entity_changed'), add.rindex('m_add_entity.call'))
        remove = s.split('void __fastcall cheat::remove_entity', 1)[1].split('void __fastcall cheat::render_view', 1)[0]
        self.assertLess(remove.index('on_entity_changed'), remove.rindex('m_remove_entity.call'))
        self.assertIn('m_add_entity.is_enabled() && m_remove_entity.is_enabled()', s)
    def test_preview_does_not_duplicate_sync_work(self):
        s = source('core/hooks/impl/cheat.cpp')
        reconcile = s.split('void reconcile_preview_scene', 1)[1].split('// Isolated SEH frame', 1)[0]
        self.assertNotIn('g_skin_sync.on_frame_stage_notify', reconcile)
        match = s.split('// Resolve team/intro previews once', 1)[1].split('// Process impacts', 1)[0]
        self.assertIn('if ( stage == 12 )\n\t\t\tdetail::reconcile_preview_scene( );', match)
    def test_sync_rate_limit_precedes_query_allocations(self):
        s = source('core/features/changer/skin_sync.cpp').split('void skin_sync::on_frame_stage_notify', 1)[1].split('std::optional<remote_player_skin>', 1)[0]
        self.assertLess(s.index('query_now < this->m_next_query_time'), s.index('ids_to_query{}'))
        self.assertIn('this->m_next_query_time = {};', s)
        self.assertIn('std::chrono::milliseconds(250)', s)
    def test_empty_cosmetics_do_not_bypass_restoration(self):
        s = source('core/features/changer/impl/guns.cpp')
        self.assertIn('!active_skins.empty() || !this->m_original_weapons.empty()', s)
        self.assertIn('remote_skins.empty() && this->m_original_weapons.empty()', s)
        self.assertIn('this->restore( weapon, iv, handle, remote_active_handle, pawn );', s)
    def test_jumpbug_diagnostics_are_not_damage_suppression(self):
        s = source('core/features/movement/impl/jumpbug.cpp')
        self.assertIn('MINTALY_JUMPBUG_DIAGNOSTICS', s)
        self.assertIn('std::chrono::milliseconds(250)', s)
        self.assertIn('missing_trace_dependency', s)
        self.assertIn('prepare_no_window', s)
        self.assertIn('cmd->buttons.value = plan->final_buttons;', s)
        self.assertNotIn('m_iHealth', s)
        self.assertNotIn('m_flFallVelocity', s)
if __name__ == '__main__': unittest.main(verbosity=2)
