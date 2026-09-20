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
        self.assertIn('i < systems::entities::entity_slot_count', s)
        self.assertIn('milliseconds(100)', s)
        self.assertIn('entity_slot_count = 0x8000', source('core/systems/systems.hpp'))
        lookup = source('core/systems/impl/entities.cpp').split('entities::get_by_index', 1)[1].split('entities::lookup', 1)[0]
        self.assertIn('index < 0 || index >= entity_slot_count', lookup)
        self.assertNotIn('m_cached_list_entries.size', lookup)
    def test_detached_cinematic_pawns_share_preview_processing(self):
        s = source('core/features/changer/preview_scene.hpp')
        candidate = s.split('inline bool is_scene_candidate', 1)[1].split('inline void refresh()', 1)[0]
        self.assertIn('"C_CSPlayerPawn"', candidate)
        self.assertIn('if (ctrl && player_pawn(ctrl) == entity) continue;', s)
        self.assertEqual(s.count('is_scene_candidate(systems::g_entities.get_schema_name(entity))'), 1)
        self.assertIn('if (!is_scene_candidate(name)) continue;', s)
    def test_lobby_music_replays_on_observed_thread(self):
        s = source('core/hooks/impl/cheat.cpp')
        dispatch = s.split('void cheat::process_lobby_music', 1)[1].split('void __fastcall cheat::play_music', 1)[0]
        self.assertIn('source_for( GetCurrentThreadId( ) )', dispatch)
        self.assertIn('pending->value.kit : source->original_kit', dispatch)
        self.assertIn('1, kit, source->volume', dispatch)
        playback = s.split('void __fastcall cheat::play_music', 1)[1].split('float __fastcall cheat::get_convar_value_float', 1)[0]
        self.assertLess(playback.index('g_lobby_music_requests.observe'), playback.index('const auto custom_kit'))
        self.assertNotIn('volume = 0.7f', playback)
    def test_lobby_bootstrap_waits_for_playback(self):
        s = source('core/hooks/impl/cheat.cpp')
        dispatch = s.split('void cheat::process_lobby_music', 1)[1].split('void __fastcall cheat::play_music', 1)[0]
        bootstrap = dispatch.split('const bool refreshed', 1)[1]
        self.assertNotIn('.acknowledge(', bootstrap)
        self.assertIn('dispatch_lobby_music_guarded( stop, update, nullptr )', dispatch)
        self.assertNotIn('pending->value.name.c_str()', dispatch)
        self.assertNotIn('if ( bootstrap_attempts >= 3 ) return;', dispatch)
        self.assertIn('bootstrap_attempts >= 3 ? 10 : 2', dispatch)
        self.assertIn('std::chrono::seconds( 2 )', dispatch)
        self.assertIn('[lobby-music] awaiting playback', bootstrap)
        playback = s.split('void __fastcall cheat::play_music', 1)[1].split('float __fastcall cheat::get_convar_value_float', 1)[0]
        self.assertIn('lobby_track && thisptr', playback)
        self.assertIn('pending_lobby ? pending_lobby->value.kit', playback)
        original = playback.rindex('m_play_music.call<void>( thisptr, track_type, music_kit_id, volume );')
        self.assertGreater(playback.index('acknowledge( *pending_lobby )'), original)
    def test_weapons_are_not_guessed_from_arbitrary_entities(self):
        s = source('core/features/changer/preview_scene.hpp')
        self.assertIn('m_pWeaponServices', s)
        self.assertIn('m_hMyWeapons', s)
        for path in ('guns','knives'):
            s = source(f'core/features/changer/impl/{path}.cpp').split(f'void {path}::on_lobby',1)[1]
            self.assertIn('preview.weapons', s)
            self.assertNotIn('get_by_index', s)
    def test_attached_weapons_do_not_require_weapon_services(self):
        s = source('core/features/changer/preview_scene.hpp')
        refresh = s.split('inline void refresh()', 1)[1]
        self.assertIn('collect_attached_weapons(p);', refresh)
        self.assertLess(refresh.index('collect_attached_weapons(p);'),
                        refresh.index('if (!p.steam_id && manager_offset && item_offset)'))
        self.assertNotIn('if (!services)', refresh)
        self.assertIn('add_weapon(p, systems::g_entities.lookup(h))', refresh)
    def test_attachment_walk_is_scoped_and_bounded(self):
        s = source('core/features/changer/preview_scene.hpp')
        walk = s.split('inline void collect_attached_weapons', 1)[1].split('inline void refresh()', 1)[0]
        for field in ('m_pGameSceneNode', 'm_pChild', 'm_pNextSibling', 'm_pParent', 'm_pOwner'):
            self.assertIn(field, walk)
        self.assertIn('i < 128', walk)
        self.assertIn('std::find(visited.begin(), visited.end(), node)', walk)
        self.assertIn('node + parent_offset).value_or(0) != parent', walk)
        self.assertIn('owner + scene_offset).value_or(0) == node', walk)
        self.assertIn('is_player(systems::g_entities.get_schema_name(owner))) continue;', walk)
        self.assertNotIn('get_by_index', walk)
    def test_weapon_candidates_are_validated_and_deduplicated(self):
        s = source('core/features/changer/preview_scene.hpp')
        add = s.split('inline void add_weapon', 1)[1].split('inline void collect_attached_weapons', 1)[0]
        self.assertIn('is_weapon(systems::g_entities.get_schema_name(weapon))', add)
        self.assertIn('std::find(p.weapons.begin(), p.weapons.end(), weapon)', add)
        self.assertIn('s == "C_CSGO_PreviewWeapon"', s)
    def test_unknown_party_members_are_not_all_local(self):
        s = source('core/features/changer/preview_scene.hpp')
        self.assertIn('players.size() == 1', s)
        self.assertIn('return sid >= steam_base && p.steam_id == sid;', s)
        self.assertIn('local_player_controller', s.split('players.size() == 1',1)[1])
    def test_preview_is_guarded_to_match_only(self):
        reconcile = source('core/hooks/impl/cheat.cpp').split('void reconcile_preview_scene', 1)[1].split('// Isolated SEH frame', 1)[0]
        self.assertLess(reconcile.index('local_player_controller'), reconcile.index('preview_scene::refresh'))
        self.assertIn('value_or( 0 ) ) return;', reconcile)
        s = source('core/hooks/impl/cheat.cpp').split('void __fastcall cheat::frame_stage_notify',1)[1].split('// Process impacts',1)[0]
        self.assertEqual(s.count('detail::reconcile_preview_scene( );'), 2)
        self.assertNotIn('s_map_name.empty', s)
        self.assertIn('stage == 6 || stage == 7 || stage == 12', s)
    def test_lobby_has_client_frame_dispatch_without_net_updates(self):
        s = source('core/hooks/impl/cheat.cpp')
        frame = s.split('void __fastcall cheat::read_frame_input', 1)[1].split('void __fastcall cheat::process_input_event', 1)[0]
        self.assertLess(frame.index('m_read_frame_input.call<void>'), frame.index('process_lobby_music( );'))
        self.assertIn('detail::reconcile_preview_scene( );', frame)
        self.assertIn('local_player_controller', frame)
        self.assertIn('lifecycle::is_unloading', frame)
        present = s.split('HRESULT __fastcall cheat::present', 1)[1].split('HRESULT __fastcall cheat::resize_buffers', 1)[0]
        self.assertNotIn('reconcile_preview_scene', present)
        self.assertNotIn('process_lobby_music', present)
    def test_cosmetics_use_player_pawn_instead_of_observer(self):
        for name in ('guns', 'knives', 'gloves'):
            s = source(f'core/features/changer/impl/{name}.cpp').split(f'void {name}::on_lobby', 1)[0]
            self.assertIn('preview_scene::player_pawn( local_ctrl )', s)
            self.assertIn('preview_scene::player_pawn( ctrl )', s)
            self.assertIn('preview_scene::player_ready( local_pawn )', s)
            self.assertNotIn('local.is_alive && local_pawn', s)
            self.assertNotIn('is_in_cinematic( )', s)
        helper = source('core/features/changer/preview_scene.hpp').split('inline std::uintptr_t player_pawn', 1)[1].split('inline bool player_ready', 1)[0]
        self.assertLess(helper.index('m_hPlayerPawn'), helper.index('m_hPawn"_hash'))
        self.assertIn('std::string_view{name} == "C_CSPlayerPawn"', helper)
    def test_original_controller_declaring_class_and_owner_links(self):
        s = source('core/features/changer/preview_scene.hpp')
        self.assertIn('SCHEMA("C_CSPlayerPawn", "m_hOriginalController"_hash)', s)
        self.assertIn('m_hOwnerEntity', s)
        self.assertIn('if (systems::g_entities.lookup(handle) == pawn) return entry.ptr;', s)
    def test_visible_sync_queries_precede_discovery_backlog(self):
        s = source('core/features/changer/skin_sync.cpp')
        self.assertLess(s.index('for (const auto id : ids_to_query) append(id);'),
                        s.index('for (const auto id : this->m_pending_query_ids) append(id);'))
        self.assertNotIn('// In lobby, query all known cheat users', s)
    def test_music_selection_notifies_config_and_sync(self):
        s = source('core/rendering/impl/menu/menu.skins.cpp').split('static inline void draw_music_tile', 1)[1].split('static void integer_input', 1)[0]
        self.assertIn('notify_skin_changed( );', s)
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
