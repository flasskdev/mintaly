"""Source-wiring regression checks only. Not a C++ compiler or a CS2 runtime test."""
from pathlib import Path
import re
import unittest
ROOT = Path(__file__).resolve().parents[1]
def source(p): return (ROOT / p).read_text(encoding='utf-8')
class LobbyCosmeticsChecks(unittest.TestCase):
    def test_preview_identity_uses_schema(self):
        s = source('core/features/changer/preview_scene.hpp')
        for field in ('m_hController', 'm_hDefaultController', 'm_hOriginalController', 'm_iAccountID'):
            self.assertIn(field, s)
        self.assertIn('if (!conflict) p.steam_id = candidate', s)
    def test_no_guessed_preview_offsets(self):
        for path in ('impl/agents.cpp','impl/guns.cpp','impl/knives.cpp','skin_sync.cpp'):
            s = source('core/features/changer/' + path)
            for old in ('0x34d0','0x3480','0x3498'):
                self.assertNotIn(old, s)
    def test_all_supported_entity_slots(self):
        s = source('core/features/changer/preview_scene.hpp')
        self.assertIn('i < 0x4000', s)
        self.assertIn('milliseconds(100)', s)
    def test_weapons_are_not_guessed_from_arbitrary_entities(self):
        s = source('core/features/changer/preview_scene.hpp')
        self.assertIn('m_pWeaponServices', s)
        self.assertIn('m_hMyWeapons', s)
        for path in ('guns','knives'):
            s = source(f'core/features/changer/impl/{path}.cpp').split(f'void {path}::on_lobby',1)[1]
            self.assertIn('preview.weapons', s)
            self.assertNotIn('get_by_index', s)
    def test_unknown_party_members_are_not_all_local(self):
        s = source('core/features/changer/preview_scene.hpp')
        self.assertIn('players.size() == 1', s)
        self.assertIn('return sid >= steam_base && p.steam_id == sid;', s)
        self.assertIn('local_player_controller', s.split('players.size() == 1',1)[1])
    def test_preview_runs_in_menu_and_match(self):
        s = source('core/hooks/impl/cheat.cpp').split('void __fastcall cheat::frame_stage_notify',1)[1].split('// Process impacts',1)[0]
        self.assertEqual(s.count('detail::reconcile_preview_scene( );'), 2)
        self.assertNotIn('s_map_name.empty', s)
        self.assertIn('stage == 6 || stage == 7 || stage == 12', s)
    def test_preview_sync_uses_shared_identities(self):
        s = source('core/features/changer/skin_sync.cpp')
        self.assertIn('for ( const auto& preview : preview_scene::players )', s)
        self.assertIn('ids_to_query.push_back( preview.steam_id )', s)
    def test_music_does_not_reject_background_map_name(self):
        s = source('core/hooks/impl/cheat.cpp').split('void cheat::process_lobby_music',1)[1].split('void __fastcall cheat::play_music',1)[0]
        self.assertNotIn('s_map_name', s)
        self.assertIn('local_player_controller', s)
        self.assertIn('g_lobby_music_requests.acknowledge', s)
    def test_team_number_reads_are_byte_sized(self):
        for name in ('agents','guns','knives','gloves'):
            s=source(f'core/features/changer/impl/{name}.cpp')
            self.assertIsNone(re.search(r'memory::(?:safe_read|read)<int>\([^\n]*m_iTeamNum',s))
    def test_preview_identity_is_written_and_verified(self):
        s=source('core/features/changer/preview_item.hpp')
        for field in ('m_bDisallowSOC','m_bInitialized','m_iItemIDHigh','m_iAccountID','m_nFallbackStatTrak'):
            self.assertGreaterEqual(s.count(field), 3)
        self.assertIn('cosmetic_cache::reusable',s)
        self.assertIn('cosmetic_attributes::matches',s)
        self.assertIn('name_tag::matches',s)
    def test_knife_model_changes_precede_identity_attributes(self):
        s=source('core/features/changer/impl/knives.cpp').split('void knives::apply(',1)[1].split('void knives::restore(',1)[0]
        self.assertLess(s.index('this->update_model('),s.index('memory::write<std::uint64_t>'))
        self.assertLess(s.index('this->update_model('),s.index('cosmetic_attributes::apply'))
        self.assertIn('preview_item::remember',s)
    def test_knife_original_is_not_recaptured_after_invalidation(self):
        s=source('core/features/changer/impl/knives.cpp')
        self.assertIn('if ( !this->m_original.captured )',s)
        self.assertNotIn('is_in_cinematic( )',s)
    def test_gloves_preview_is_implemented(self):
        s=source('core/features/changer/impl/gloves.cpp').split('void gloves::on_lobby',1)[1]
        self.assertIn('m_EconGloves',s)
        self.assertIn('this->apply( preview.pawn',s)
    def test_controller_survives_missing_pawn(self):
        s=source('core/systems/impl/local.cpp')
        self.assertEqual(s.count('publish_controller_only( );'),2)
        self.assertIn('s.controller = local_player_controller',s)
    def test_stattrak_uses_effective_team_and_no_detached_save(self):
        s=source('core/features/misc/impl/other.cpp').split('void other::on_player_death',1)[1].split('m_kill_say.enabled',1)[0]
        self.assertIn('skins.for_team(kill_team)',s)
        self.assertNotIn('skins.data',s)
        self.assertNotIn('.detach()',s)
        self.assertIn('std::numeric_limits<int>::max',s)
        self.assertIn('config::registry::save_active()',s)
        self.assertIn('g_skin_sync.trigger_push()',s)
    def test_atomic_config_save_publishes_name_after_success(self):
        s=source('external/config.hpp').split('inline bool save(std::wstring_view name)',1)[1].split('inline bool load',1)[0]
        self.assertIn('MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH',s)
        self.assertIn('std::ofstream file(temporary',s)
        self.assertLess(s.index('MoveFileExW'),s.index('g_active_config = saved_name'))
    def test_preview_state_is_cleared_on_shutdown(self):
        s=source('core/hooks/impl/cheat.cpp').split('void cheat::do_level_shutdown',1)[1]
        self.assertIn('preview_scene::reset( );',s)
        self.assertIn('preview_item::reset( );',s)
if __name__ == '__main__': unittest.main(verbosity=2)
