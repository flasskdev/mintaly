"""Structural regression checks, NOT a compiler or a CS2 runtime test."""
from pathlib import Path
import unittest
ROOT = Path(__file__).resolve().parents[1]
def text(p): return (ROOT / p).read_text(encoding="utf-8")

class SourceChecks(unittest.TestCase):
    def test_event_key_is_not_zero(self):
        s = text("utilities/cstypes.hpp")
        self.assertIn("hash( event_key::hash( s ) )", s)
        self.assertNotIn(": hash( 0 ), unk( 0xFFFFFFFF ), str( s )", s)
    def test_hurt_uses_controller_identity(self):
        s = text("core/features/misc/impl/impacts.cpp").split("impacts::hit_data impacts::parse_event", 1)[1]
        self.assertIn("event_key::local_attacker( actual_local, local.controller, attacker, victim )", s)
        self.assertNotIn('get_pawn( reinterpret_cast< void* >( event ), "attacker" )', s)
    def test_music_request_does_not_call_engine(self):
        s = text("core/hooks/impl/cheat.cpp").split("void cheat::trigger_lobby_music",1)[1].split("void cheat::process_lobby_music",1)[0]
        self.assertIn("g_lobby_music_requests.submit", s)
        self.assertNotIn("memory::call", s)
    def test_no_cached_audio_context_or_global_stop(self):
        s = text("core/hooks/impl/cheat.cpp")
        self.assertNotIn("s_last_music_thisptr", s)
        self.assertNotIn('"stopsound"', s)
        self.assertNotIn("playvol Music.Background.", s)
    def test_lobby_dispatch_before_missing_controller_return(self):
        s = text("core/hooks/impl/cheat.cpp").split("if ( !local_player_controller || is_level_shutting_down( ) )",1)[1].split("m_was_connected = true;",1)[0]
        self.assertIn("process_lobby_music( );", s)
        self.assertIn("g_inspect_preview.on_frame_stage_notify( );", s)
    def test_knife_after_original_callback(self):
        s = text("core/hooks/impl/cheat.cpp").split("void __fastcall cheat::frame_stage_notify",1)[1].split("// Process impacts after event dispatch",1)[0]
        k = s.index("g_knives.on_frame_stage_notify( );")
        self.assertGreater(k, s.rindex("m_frame_stage_notify.call<void>( thisptr, stage );"))
        self.assertEqual(s.count("g_knives.on_frame_stage_notify( );"), 1)
    def test_sync_waits_for_match(self):
        s = text("core/features/changer/skin_sync.cpp").split("void skin_sync::on_present",1)[1].split("void skin_sync::on_frame_stage_notify",1)[0]
        self.assertIn("systems::g_local.get( ).controller != 0", s)
        self.assertLess(s.index("if ( !active )"), s.index("this->initialize"))
        worker = text("core/features/changer/skin_sync.cpp").split("void skin_sync::worker_loop", 1)[1].split("bool skin_sync::perform_push", 1)[0]
        self.assertLess(worker.index('!this->m_match_active.load()'), worker.index('this->perform_push()'))
    def test_handle_generation_is_validated(self):
        lookup = text("core/systems/impl/entities.cpp").split("entities::lookup", 1)[1].split("entities::get_by_type", 1)[0]
        self.assertIn("current_handle != handle", lookup)
        self.assertIn("entity + 0x10 ) != identity", lookup)
    def test_hud_binding_matches_active_weapon(self):
        helper = text("core/features/changer/hud_weapon.hpp")
        self.assertIn('"m_hWeapon"_hash', helper)
        self.assertIn('value_or(0xffffffffu) == active', helper)
        self.assertIn('i < 128', helper)
        for name in ('guns', 'knives'):
            s = text(f"core/features/changer/impl/{name}.cpp").split(f"{name}::find_hud_model_weapon", 1)[1].split(f"void {name}::clear_hud_icon", 1)[0]
            self.assertIn('return hud_weapon::find( pawn );', s)
    def test_cached_sync_users_are_refreshed(self):
        s = text("core/features/changer/skin_sync.cpp").split("void skin_sync::perform_users_update",1)[1]
        self.assertNotIn("!this->m_cache.contains( sid )", s)
    def test_agents_do_not_force_standing_hull(self):
        s = text("core/features/changer/impl/agents.cpp")
        self.assertNotIn("is_in_cinematic( )", s)
        self.assertNotIn("math::vector3( 16.0f, 16.0f, 72.0f )", s)
        self.assertIn("m_hPlayerPawn", s)
        self.assertIn("agent_model_matches( local_pawn, model_path )", s)
    def test_hud_refresh_does_not_require_weapon_switch(self):
        s = text("core/features/changer/impl/knives.cpp")
        self.assertNotIn("if ( active_handle != this->m_last_active_handle )", s)
        self.assertIn("memory::call<void>( set_model, view_model, target );", s)
        self.assertIn("m_tracked_weapon_handle != handle", s)
        self.assertIn("cosmetic_attributes::matches( iv, *selected_skin )", s)
    def test_jumpbug_uses_swept_window_and_atomic_allocation(self):
        s = text("core/features/movement/impl/jumpbug.cpp")
        self.assertIn("jumpbug_timing::find_contact_time", s)
        self.assertIn("safe_release(*when + jumpbug_timing::event_gap)", s)
        self.assertNotIn("std::round(result.fraction", s)
        self.assertLess(s.index("steps[i] = systems::g_input.acquire_subtick_step"), s.index("step->set_button(step->button() & ~controlled)"))
        self.assertIn("moves->m_current_size = old_size;", s)
    def test_jump_release_survives_disabling_feature(self):
        s = text("core/features/movement/impl/jumpbug.cpp")
        self.assertLess(s.index("if (fired_previous &&"), s.index("const auto& config"))
        self.assertIn("if (!cmd) { this->m_fired_last_tick = fired_previous; return; }", s)

if __name__ == "__main__": unittest.main(verbosity=2)
