"""Проверки структуры исходников. Это не компиляция C++ и не тест в CS2."""
from pathlib import Path
import unittest
ROOT = Path(__file__).resolve().parents[1]
def source(p): return (ROOT / p).read_text(encoding='utf-8')
def between(s,a,b): return s.split(a,1)[1].split(b,1)[0]
class SkinSyncChecks(unittest.TestCase):
    def test_preview_knife_glove_entry_points_do_not_write_entities(self):
        for kind in ('knives','gloves'):
            s=source(f'core/features/changer/impl/{kind}.cpp')
            lobby=between(s,f'void {kind}::on_lobby',f'void {kind}::reset')
            self.assertNotIn('memory::',lobby)
            self.assertNotIn('this->apply',lobby)
            self.assertIn(f'void {kind}::on_frame_stage_notify',s)
    def test_hud_identity_and_item_flags_are_in_both_gun_fast_paths(self):
        s=source('core/features/changer/impl/guns.cpp')
        self.assertEqual(s.count('&& applied_it->second.hud == hud_visual'),2)
        self.assertEqual(s.count('m_bDisallowSOC"_hash ) ).value_or( false )'),2)
        self.assertEqual(s.count('m_bInitialized"_hash ) ).value_or( false )'),2)
    def test_pickup_provenance_is_independent_of_applied_cache(self):
        s=source('core/features/changer/impl/guns.cpp')
        selection=between(s,'guns::select_skin','void guns::on_frame_stage_notify')
        self.assertIn('original.cosmetic->account_id != holder_account',selection)
        self.assertIn('return original.cosmetic;',selection)
        self.assertIn('systems::g_entities.lookup( handle ) == weapon',selection)
        self.assertIn('*item == original.item_id',selection)
        self.assertIn('original.def_index == definition',selection)
        self.assertLess(selection.index('return original.cosmetic;'),selection.index('skins.find( definition )'))
        self.assertEqual(s.count('this->select_skin('),3)
    def test_item_snapshot_is_published_after_apply(self):
        s=source('core/features/changer/impl/guns.cpp')
        apply=between(s,'bool guns::apply','bool guns::capture_original')
        self.assertLess(apply.index('this->rebuild_paint'),apply.index('original->second.cosmetic ='))
    def test_inventory_lengths_are_bounded(self):
        for kind in ('guns','knives'):
            s=source(f'core/features/changer/impl/{kind}.cpp')
            self.assertIn('weapons_size <= 64',s)
            self.assertIn('remote_weapons_size > 64',s)
    def test_death_does_not_save_or_invalidate_all_cosmetics(self):
        s=source('core/features/misc/impl/other.cpp')
        death=between(s,'void other::on_player_death','void other::on_frame_stage_notify')
        self.assertIn('request_save_active()',death)
        self.assertNotIn('save_active()',death.replace('request_save_active()',''))
        self.assertNotIn('g_guns.invalidate',death)
        self.assertNotIn('g_knives.invalidate',death)
    def test_deferred_save_is_flushed_before_profile_replacement(self):
        s=source('external/config.hpp')
        for a,b in [('inline bool from_json(', 'inline nlohmann::json to_json_delta'),
                    ('inline bool from_json_delta(', 'namespace registry {'),
                    ('inline bool load(', 'inline bool remove(')]:
            self.assertIn('flush_pending_save()',between(s,a,b))
        self.assertIn('flush_pending_save()',source('entry.cpp'))
        self.assertIn('flush_pending_save()',source('core/hooks/impl/cheat.cpp'))
        self.assertIn('flush_pending_save()',between(source('core/features/misc/impl/other.cpp'),'void other::on_round_start','void other::on_player_death'))
    def test_sync_worker_is_joined_before_free_library(self):
        s=source('core/features/changer/skin_sync.cpp')
        init=between(s,'void skin_sync::initialize','bool skin_sync::shutdown')
        self.assertNotIn('CloseHandle(worker)',init)
        shutdown=between(s,'bool skin_sync::shutdown','void skin_sync::trigger_push')
        self.assertLess(shutdown.index('WaitForSingleObject'),shutdown.index('CloseHandle'))
        e=between(source('entry.cpp'),'void unload_and_exit','void request_unload')
        self.assertIn('if ( !shutdown_all_cheat_systems() )',e)
        self.assertLess(e.index('return;'),e.index('FreeLibraryAndExitThread'))
    def test_profile_decode_is_outside_cache_lock(self):
        s=between(source('core/features/changer/skin_sync.cpp'),'void skin_sync::perform_pull','void skin_sync::perform_users_update')
        self.assertLess(s.index('decode_map'),s.index('std::unique_lock lock( this->m_mutex )'))
        self.assertIn('updates.emplace',s)
    def test_event_timing_diagnostic_exists(self):
        self.assertIn('slow game event: %s elapsed_ms=%.3f',source('core/systems/impl/events.cpp'))
if __name__ == '__main__': unittest.main(verbosity=2)
