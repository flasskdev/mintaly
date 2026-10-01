"""Статические проверки патча; не заменяют сборку MSVC и проверку в игре."""
from pathlib import Path
import argparse
import unittest

parser = argparse.ArgumentParser()
parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
args, rest = parser.parse_known_args()
ROOT = args.root
SHARED = (ROOT / 'core/features/combat/impl/shared.cpp').read_text(encoding='utf-8')
RAGE = (ROOT / 'core/features/combat/impl/rage.cpp').read_text(encoding='utf-8')


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


class NoShotSourceChecks(unittest.TestCase):
    def test_unwritten_native_output_cannot_be_a_valid_zero(self):
        function = body(SHARED, 'math::vector3 shared::get_shoot_position(')
        self.assertIn('auto native = detail::invalid_shoot_position( );', function)
        self.assertIn('memory::safe_call<void>', function)
        self.assertIn('detail::finite_position( native )', function)
        self.assertIn('( !native_zero || eye_zero )', function)
        self.assertIn('const auto result = use_native ? native : *eye;', function)

    def test_schema_fallback_is_evaluated_world_eye(self):
        function = body(SHARED, 'std::optional<math::vector3> read_current_eye(')
        self.assertIn('"m_vecAbsOrigin"_hash', function)
        self.assertIn('"m_vecViewOffset"_hash', function)
        self.assertIn('const auto eye = *origin + *view;', function)
        self.assertIn('memory::safe_read<math::vector3>', function)
        self.assertIn('if ( !scene_offset || !origin_offset || !view_offset )', function)
        self.assertNotIn('"m_vecOrigin"_hash', function)

    def test_all_three_fallbacks_fail_closed(self):
        for signature, ret in [('bool rage::run_gun(', 'return false;'),
                               ('void rage::run_taser(', 'return;'),
                               ('void rage::run_knife(', 'return;')]:
            with self.subTest(function=signature):
                function = body(RAGE, signature)
                begin = function.index('const auto eye = g_shared.get_shoot_position();')
                end = function.index('eye_candidates.entries[0].position = eye;', begin)
                guard = function[begin:end]
                for component in 'xyz':
                    self.assertIn(f'!std::isfinite(eye.{component})', guard)
                self.assertIn(ret, guard)

    def test_history_is_cleared_before_optional_prediction(self):
        function = body(RAGE, 'rage::aim_context rage::build_context(')
        self.assertLess(function.index('g_shared.sh() = {};'),
                        function.index('systems::g_prediction.simulate('))

    def test_run_reads_arrays_after_bullet_mutation(self):
        function = body(SHARED, 'bool shared::penetration::run(')
        call = function.index('memory::call<void> (PATTERN (patterns::trace_bullet),')
        for text in ('const auto num_hits = trace->num_hits;',
                     'const auto hit_array =', 'const auto surface_array ='):
            self.assertGreater(function.index(text), call)
        self.assertIn('num_hits > trace->hit_capacity', function)
        self.assertIn('contact_index >= trace->unknown3', function)

    def test_can_reads_array_after_bullet_mutation(self):
        function = body(SHARED, 'bool shared::penetration::can(')
        call = function.index('memory::call<void> (PATTERN (patterns::trace_bullet),')
        for text in ('const auto num_hits = trace->num_hits;', 'const auto hit_array ='):
            self.assertGreater(function.index(text), call)
        self.assertIn('num_hits > trace->hit_capacity', function)

    def test_invalid_positions_never_reach_bullet_trace(self):
        for signature in ('bool shared::penetration::run(', 'bool shared::penetration::can('):
            function = body(SHARED, signature)
            self.assertLess(function.index('!detail::finite_position( start )'),
                            function.index('systems::g_tracing.setup_trace('))
            self.assertIn('!std::isfinite( damage ) || damage <= 0.0f', function)

    def test_target_matching_and_final_damage_gate_preserved(self):
        function = body(SHARED, 'bool shared::penetration::run(')
        self.assertIn('if ( hit_entity != ctx.target_pawn )', function)
        self.assertIn('systems::g_entities.lookup( owner_handle ) != ctx.target_pawn', function)
        self.assertIn('if ( target_hit_idx < 0 )', function)
        self.assertIn('pen.damage < this->get_min_damage(', RAGE)
        self.assertIn('pen.hitgroup != tgt.hit.hitgroup', RAGE)


if __name__ == '__main__':
    unittest.main(argv=['rage_no_shot_checks.py', *rest])
