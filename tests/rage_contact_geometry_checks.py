"""Статические проверки правок contact-geometry и fire-history; не заменяют
сборку MSVC и проверку в игре."""
from pathlib import Path
import argparse
import unittest

parser = argparse.ArgumentParser()
parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
args, rest = parser.parse_known_args()
ROOT = args.root
SHARED = (ROOT / 'core/features/combat/impl/shared.cpp').read_text(encoding='utf-8')
RAGE = (ROOT / 'core/features/combat/impl/rage.cpp').read_text(encoding='utf-8')
INPUT = (ROOT / 'core/systems/impl/input.cpp').read_text(encoding='utf-8')
DIAG = (ROOT / 'utilities/rage_scan_diagnostics.hpp').read_text(encoding='utf-8')


def body(source, signature):
    start = source.index(signature)
    begin = source.index('{', start)
    depth = 0
    for i in range(begin, len(source)):
        if source[i] == '{':
            depth += 1
        elif source[i] == '}':
            depth -= 1
            if depth == 0:
                return source[begin:i + 1]
    raise AssertionError('Незакрытая функция: ' + signature)


class ContactGeometryChecks(unittest.TestCase):
    def test_penetration_no_longer_branches_on_can_penetrate(self):
        # Бит 0 байта на 0x14 совпадает с чётностью индекса записи, а не с
        # признаком пробития: ветка пропускала цель и роняла весь скан.
        function = body(SHARED, 'bool shared::penetration::run(')
        self.assertNotIn('can_penetrate', function)
        self.assertNotIn('can_penetrate', body(SHARED, 'bool shared::penetration::can('))

    def test_geometry_fallback_runs_after_the_contact_pass(self):
        function = body(SHARED, 'bool shared::penetration::run(')
        contact = function.index('rd::mark(rd::event::pen_contact_accept);')
        geometry = function.index('rd::mark(rd::event::pen_geometry_accept);')
        output = function.index('out.hitbox = actual_hitbox;')
        self.assertLess(contact, geometry)
        self.assertLess(geometry, output)
        # Отказ по геометрии остаётся последним рубежом.
        self.assertEqual(function.count('if ( target_hit_idx < 0 )'), 2)
        no_target = function.rindex('if ( target_hit_idx < 0 )')
        self.assertIn('rd::mark(rd::event::no_target_hit);', function[no_target:])

    def test_fallback_takes_only_a_live_segment_that_spans_the_target(self):
        function = body(SHARED, 'bool shared::penetration::run(')
        accept = function.index('rd::mark(rd::event::pen_geometry_accept);')
        fallback = function[function.rindex('if ( target_hit_idx < 0 )', 0, accept):accept]
        self.assertIn('std::clamp( closest_hitbox_fraction, 0.0f, 1.0f )', fallback)
        self.assertIn('!std::isfinite( damage ) || damage <= 0.0f', fallback)
        self.assertIn('target_fraction + fraction_epsilon >= hit->enter_fraction', fallback)
        self.assertIn('target_fraction <= hit->exit_fraction + fraction_epsilon', fallback)
        # Сегмент, в котором пуля уже мертва, не может быть попаданием.
        self.assertIn('rd::mark(rd::event::damage_exhausted);', fallback)
        self.assertIn('break;', fallback)

    def test_penetrated_comes_from_a_visibility_trace(self):
        function = body(SHARED, 'bool shared::penetration::run(')
        self.assertIn(
            'out.penetrated = !systems::g_tracing.is_visible( start, end, ctx.target_pawn, local_pawn );',
            function)
        self.assertNotIn('out.penetrated = penetrated;', function)

    def test_can_reports_whether_damage_is_left_on_the_path(self):
        function = body(SHARED, 'bool shared::penetration::can(')
        self.assertIn('out_damage = 0.0f;', function)
        self.assertIn('out_damage = damage;', function)
        self.assertIn('return out_damage > 0.0f;', function)


class FireHistoryChecks(unittest.TestCase):
    def setUp(self):
        self.fire = body(RAGE, 'void rage::fire_gun(')

    def test_empty_history_never_aborts_the_shot(self):
        # Именно эта проверка держала attack_set в единицах при сотнях fire_calls.
        self.assertNotIn('const auto history_size = cmd->csgo_user_cmd.input_history_size();', self.fire)
        self.assertIn('auto history_size = cmd->csgo_user_cmd.input_history_size();', self.fire)
        fired = self.fire.index('rd::mark(rd::event::fire_calls);')
        first_return = self.fire.index('return;', fired)
        self.assertNotIn('input_history_size()', self.fire[fired:first_return])

    def test_missing_history_is_filled_before_the_stamping_loop(self):
        fired = self.fire.index('rd::mark(rd::event::fire_calls);')
        pushed = self.fire.index('push_input_history', fired)
        loop = self.fire.index('for (auto i = 0; i < history_size; ++i)')
        stamped = self.fire.index('set_attack1_start_history_index(history_size - 1)')
        self.assertLess(fired, pushed)
        self.assertLess(pushed, loop)
        self.assertLess(loop, stamped)
        self.assertIn('rd::mark(rd::event::fire_history_pushed);', self.fire)
        self.assertIn('rd::mark(rd::event::fire_history_missing);', self.fire)

    def test_attack_is_anchored_when_no_entry_exists(self):
        self.assertIn('// No entry survived', self.fire)
        self.assertIn('cmd->csgo_user_cmd.set_attack1_start_history_index(-1);', self.fire)

    def test_only_a_missing_base_message_can_abort_a_fire(self):
        self.assertIn('if (!base || !base->mutable_viewangles())', self.fire)
        self.assertIn('rd::mark(rd::event::fire_abort_viewangles);', self.fire)

    def test_push_input_history_is_guarded_like_subtick_push(self):
        function = body(INPUT, 'proto::input_history_entry* input::push_input_history(')
        self.assertIn('const auto allocator = PATTERN (patterns::history_field_alloc);', function)
        self.assertIn('const auto push = PATTERN (patterns::utl_vector_push);', function)
        self.assertIn('if ( !allocator || !push )', function)
        self.assertIn('valid_runtime_pointer', function)
        self.assertIn('history_field->m_current_size <= before_size', function)
        self.assertNotIn('memory::call<void*>(PATTERN (patterns::history_field_alloc)', function)


class DiagnosticsChecks(unittest.TestCase):
    def test_new_counters_are_declared_named_and_reported(self):
        for counter in ('fire_abort_viewangles', 'fire_history_pushed',
                        'fire_history_missing', 'pen_contact_accept', 'pen_geometry_accept',
                        'gate_inactive', 'gate_no_enemies', 'gate_disabled', 'gate_cannot_shoot'):
            self.assertIn(counter + ',', DIAG)
            self.assertIn('"%s"' % counter, DIAG)
            self.assertIn('event::' + counter, DIAG)
        self.assertIn('static_assert(names.size() == static_cast<std::size_t>(event::count));', DIAG)

    def test_every_create_move_early_exit_is_counted(self):
        function = body(RAGE, 'void rage::on_create_move(')
        for counter in ('gate_inactive', 'gate_no_enemies', 'gate_disabled', 'gate_cannot_shoot'):
            self.assertIn('rd::mark(rd::event::%s);' % counter, function)


if __name__ == '__main__':
    unittest.main(argv=['rage_contact_geometry_checks.py', *rest])
