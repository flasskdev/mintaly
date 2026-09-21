"""Source-wiring checks and a numerical reference model, NOT a C++ build/game test."""
from pathlib import Path
import math
import random
import unittest

ROOT = Path(__file__).resolve().parents[1]
def source(path): return (ROOT / path).read_text(encoding='utf-8')

def reference_solve(target, inherited, speed):
    if not all(math.isfinite(x) for x in (*target, *inherited, speed)) or speed <= 0:
        return None
    norm = math.sqrt(sum(x*x for x in target))
    if norm < 1e-6: return None
    direction = [x / norm for x in target]
    along = sum(a*b for a,b in zip(direction, inherited))
    perpendicular = [v-d*along for v,d in zip(inherited,direction)]
    discriminant = speed*speed-sum(x*x for x in perpendicular)
    if discriminant < 0: return None
    component = math.sqrt(discriminant)
    forward_speed = along + component
    if forward_speed <= 0: return None
    return [(d*component-p)/speed for d,p in zip(direction,perpendicular)], forward_speed

class Wiring(unittest.TestCase):
    def test_standalone_skin_profiles_are_removed(self):
        s = source('core/rendering/impl/menu/skin_workspace.hpp')
        for removed in ('profile_store', 'profile_popup', 'profiles.json', 'Choose config', 'Create config'):
            self.assertNotIn(removed, s)
        self.assertNotIn('skin_workspace::profiles', source('core/rendering/impl/menu/menu.skins.cpp'))

    def test_cosmetics_still_belong_to_global_cfg(self):
        s = source('core/settings.hpp')
        for name in ('applied skins', 'agents', 'custom agents', 'music'):
            self.assertIn('make_key("changer", "' + name + '")', s)
        self.assertIn('Saved with the main config', source('core/rendering/impl/menu/skin_workspace.hpp'))

    def test_main_cfg_writes_remain_atomic(self):
        s = source('external/config.hpp')
        self.assertIn('MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH', s)
        self.assertIn('g_active_config = saved_name;', s)

    def test_super_toss_only_in_main(self):
        s = source('core/rendering/impl/menu/menu.misc.cpp')
        self.assertEqual(s.count('xui::toggle( "Super Toss"'), 1)
        self.assertLess(s.index('xui::toggle( "Super Toss"'), s.index('else if ( subtab == 1 )'))

    def test_throw_runs_after_movement(self):
        s = source('core/hooks/impl/cheat.cpp')
        toss = s.index('g_projectile_trajectory.on_create_move( current_cmd )')
        for feature in ('g_test_strafer', 'g_jumpbug'):
            self.assertLess(s.index(feature + '.on_create_move( current_cmd'), toss)
        self.assertLess(s.index('g_misc.quickpeek( ).on_create_move( current_cmd )'), toss)

    def test_throw_uses_prediction_history_and_exact_strength(self):
        s = source('core/features/misc/impl/projectile.trajectory.cpp')
        body = s[s.index('void projectile_trajectory::correct_throw_angles'):s.index('void projectile_trajectory::compute_desired_direction')]
        self.assertIn('g_prediction.simulate', body)
        self.assertIn('m_vecStashedVelocity', body)
        self.assertIn('throw_compensation::solve', body)
        self.assertIn('mutable_input_history', body)
        self.assertNotIn('strength = 0.5f', s)

    def test_jumpbug_latches_and_releases_owned_buttons(self):
        s = source('core/features/movement/impl/jumpbug.cpp')
        self.assertIn('m_cycle.fired()', s)
        self.assertIn('!can_attempt', s)
        self.assertIn('release_owned()', s)
        self.assertIn('original_buttons & jump', s)
        self.assertIn('original_buttons & duck', s)
        self.assertIn('pre.ducked &&', s)
        self.assertIn('jumpbug_command::make', s)
        self.assertIn('cmd->buttons.value = plan->final_buttons', s)
        self.assertIn('m_owned_duck = plan->owns_duck', s)
        self.assertIn('m_fired_last_tick = plan->owns_jump', s)

    def test_fps_limit_is_removed_from_runtime_sources(self):
        for directory in ('core', 'protection', 'utilities'):
            for path in (ROOT / directory).rglob('*'):
                if path.suffix not in ('.hpp', '.cpp', '.h'):
                    continue
                content = path.read_text(encoding='utf-8')
                for removed in ('m_fps_limit', 'fps_max_cvar', 'get_convar_value_float', 'FPS Limit'):
                    self.assertNotIn(removed, content, str(path))

    def test_molotov_is_wired_and_scoped(self):
        menu = source('core/rendering/impl/menu/menu.misc.cpp')
        scene = source('core/features/world/impl/scene.cpp')
        hook = source('core/hooks/impl/cheat.cpp')
        self.assertIn('xui::toggle( "Molotov Color"', menu)
        self.assertIn('m_smoke_and_fire_color.molotov_color', menu)
        self.assertIn('is_inferno_primitive(mesh)', scene)
        self.assertIn('== "C_Inferno"_hash', scene)
        self.assertIn('{color.r, color.g, color.b, original->a}', scene)
        self.assertIn('detail::restore_batch_colors', hook)
        self.assertIn('m_smoke_and_fire_color.custom_molotov.value', hook)

class NumericalReference(unittest.TestCase):
    def test_random_throw_solutions(self):
        rng = random.Random(725)
        for _ in range(10000):
            target = [rng.uniform(-1,1) for _ in range(3)]
            inherited = [rng.uniform(-400,400) for _ in range(3)]
            answer = reference_solve(target,inherited,1000)
            self.assertIsNotNone(answer)
            direction, speed = answer
            self.assertAlmostEqual(sum(x*x for x in direction), 1, places=10)
            norm = math.sqrt(sum(x*x for x in target))
            for i in range(3):
                self.assertAlmostEqual(direction[i]*1000+inherited[i], target[i]/norm*speed, places=9)

    def test_strafe_jump_and_impossible_weak_throw(self):
        for lateral in (-312.5,312.5):
            self.assertIsNotNone(reference_solve((1,0,0),(0,lateral,375),675))
            self.assertIsNone(reference_solve((1,0,0),(0,lateral,375),202.5))
        self.assertIsNone(reference_solve((1,0,0),(-700,0,0),675))
        self.assertIsNone(reference_solve((1,0,0),(math.nan,0,0),675))

    def test_inverse_pitch_reference(self):
        for i in range(-890,891):
            pitch = i / 10
            launch = pitch - (90-abs(pitch))/9
            recovered = (launch+10) * (0.9 if launch >= -10 else 9/8)
            self.assertAlmostEqual(recovered,pitch,places=10)

if __name__ == '__main__': unittest.main(verbosity=2)
