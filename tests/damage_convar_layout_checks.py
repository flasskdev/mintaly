"""Source checks only; runtime ConVar ABI and MSVC build remain unverified."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / 'core/features/combat/impl/shared.cpp').read_text(encoding='utf-8')


class DamageConvarLayoutChecks(unittest.TestCase):
    def test_bounded_read_only_capture(self):
        start = SOURCE.index('void report_damage_convar_layout(')
        end = SOURCE.index('[[nodiscard]] bool finite_position', start)
        capture = SOURCE[start:end]
        self.assertIn('std::array<std::atomic<bool>, 4>', capture)
        self.assertIn('slot >= reported.size()', capture)
        self.assertIn('reported[slot].exchange(true, std::memory_order_relaxed)', capture)
        self.assertIn('std::array<std::uint32_t, 32>', capture)
        self.assertIn('std::array<char, 64>', capture)
        self.assertIn('if (!words)', capture)
        self.assertIn('name ? 64 : 10', capture)
        self.assertIn('i += 4', capture)
        self.assertNotIn('memory::write', capture)
        self.assertNotIn('memory::safe_write', capture)
        self.assertNotIn('memory::call', capture)
        self.assertNotIn('get<float>', capture)

    def test_each_coefficient_has_its_own_slot(self):
        for slot, suffix in enumerate(('ct_head', 't_head', 'ct_body', 't_body')):
            self.assertIn(
                f'detail::report_damage_convar_layout({slot}, "mp_damage_scale_{suffix}", '
                f'cv_{suffix}, ctx.scales.{suffix});', SOURCE)

    def test_invalid_damage_protection_remains(self):
        self.assertIn('if (!damage_validation::positive_finite(out.damage))', SOURCE)
        self.assertIn('action=reject', SOURCE)


if __name__ == '__main__':
    unittest.main()
