"""Source-wiring checks; these do not replace an MSVC build or CS2 runtime tests."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
def source(path):
    return (ROOT / path).read_text(encoding="utf-8")
def block(text, first, last):
    return text.split(first, 1)[1].split(last, 1)[0]

class SyncLifecycleChecks(unittest.TestCase):
    def test_identity_checks_fail_closed(self):
        text = source("core/features/changer/entity_guard.hpp")
        self.assertIn('SCHEMA("CEntityIdentity", "m_flags"_hash)', text)
        self.assertIn('if (!flags || *flags != 0)', text)
        self.assertIn('systems::g_entities.lookup(handle) != entity', text)
        self.assertIn('return actual && *actual == expected;', text)
        self.assertNotIn('identity + 0x48', text)
        self.assertNotIn('safe_write', text)

    def test_hud_is_reacquired_after_binding(self):
        text = source("core/features/changer/hud_weapon.hpp")
        self.assertLess(text.index('memory::call<void>(binding'), text.index('const auto hud = entity_guard::capture(find(pawn))'))
        self.assertIn('active_handle() != handle', text)
        self.assertIn('entity_guard::set_model(*hud', text)
        self.assertIn('entity_guard::set_mesh(*hud', text)

    def test_failed_rebuild_is_not_success(self):
        for kind in ('guns', 'knives'):
            text = source(f"core/features/changer/impl/{kind}.cpp")
            rebuild = block(text, f'bool {kind}::rebuild_paint', f'bool {kind}::update_view_model')
            self.assertIn('needs_hud && !this->update_view_model', rebuild)
            self.assertIn('entity_guard::rebuild_materials', rebuild)
            apply = block(text, f'{kind}::apply(', f'{kind}::' + ('capture_original' if kind == 'guns' else 'restore'))
            self.assertIn('!this->rebuild_paint', apply)
        knives = source('core/features/changer/impl/knives.cpp')
        apply = block(knives, 'void knives::apply', 'void knives::restore')
        self.assertIn('!this->update_model', apply)
        self.assertLess(apply.index('!this->rebuild_paint'), apply.index('preview_item::remember'))

    def test_remote_selection_has_no_local_fallback(self):
        text = source('core/features/changer/impl/guns.cpp')
        select = block(text, 'guns::select_skin', 'void guns::on_frame_stage_notify')
        self.assertNotIn('settings::g_changer.skins.data', select)

    def test_worker_control_and_reply_generation(self):
        text = source('core/features/changer/skin_sync.cpp')
        toggle = block(text, 'void skin_sync::on_sync_toggled', 'void skin_sync::set_local_steam_id')
        self.assertNotIn('m_last_push_time', toggle)
        self.assertNotIn('resolve_local_steam_id', toggle)
        self.assertIn('++m_epoch', toggle)
        worker = block(text, 'void skin_sync::worker_loop', 'bool skin_sync::perform_push')
        self.assertNotIn('settings::', worker)
        self.assertIn('m_schedule_reset.exchange(false)', worker)
        for first, last in [('bool skin_sync::perform_push', 'void skin_sync::perform_pull'),
                            ('void skin_sync::perform_pull', 'void skin_sync::perform_users_update')]:
            self.assertIn('epoch != m_epoch', block(text, first, last))
        self.assertIn('epoch != m_epoch', text.split('void skin_sync::perform_users_update', 1)[1])
        read = block(text, 'skin_sync::get_remote_skin', 'bool skin_sync::is_cheat_user')
        self.assertIn('!fresh(found->second)', read)
        pull = block(text, 'void skin_sync::perform_pull', 'void skin_sync::perform_users_update')
        self.assertLess(pull.index('decode_map'), pull.index('std::unique_lock lock(m_mutex)'))
        self.assertIn('if (!users.contains(std::to_string(id)))', pull)

if __name__ == '__main__':
    unittest.main(verbosity=2)
