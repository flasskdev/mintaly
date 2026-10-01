<!-- split-part | CS2_RESEARCH_MASTER.md lines 60752-60818 | body-sha256 be637890849e8b3f6fc141aa9d704d801708afb7cfdfd474caea82a6a3568494 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-052"></a>

## E052. `analysis/phase2/reconstructed/check_math.py`

Bytes: 3495. SHA-256: `415087abfb3099c60fd35c84d5c96c64cc9a8a10178d9d2d4e8e74b1bf7abfc6`.

```python
import json
from math import hypot, isclose
from pathlib import Path
from recovered_math import (
    SampleOutcome, aggregate_full, aggregate_fast, zero_scale_aggregate,
    ring_samples, golden_samples, estimated_health, minimum_damage_threshold,
    distance_attenuation, retained_count_after_tail_aging, resets_history,
)

checks = []


def check(name, condition):
    if not condition:
        raise AssertionError(name)
    checks.append(name)


uniform = [SampleOutcome(50.0, True, 2)] * 64
full = aggregate_full(uniform, 20.0, 50.0, True)
fast = aggregate_fast(uniform, 20.0, 50.0, True)
check('full includes equality at health threshold', full.health_score_fraction == 1.0)
check('fast excludes equality at health threshold', fast.health_score_fraction == 0.0)
check('uniform block permits early-fill after 8', fast.evaluated_samples == 8)
check('full always evaluates all supplied 64', full.evaluated_samples == 64)
check('uniform means and special fractions', full.mean_score == fast.mean_score == 50 and full.special_fraction == fast.special_fraction == 1)
check('special mode gate', aggregate_full(uniform, 20, 50, False).special_fraction == 0)
nonuniform = [SampleOutcome(0, False)] * 56 + [SampleOutcome(60, True, 3)] * 8
check('full measured fraction differs from extrapolated fast fraction',
      aggregate_full(nonuniform, 20, 50, True).minimum_score_fraction == 0.125
      and aggregate_fast(nonuniform, 20, 50, True).minimum_score_fraction == 1.0)
check('unsatisfied A forces all blocks', aggregate_fast(uniform, 80, 100, False).evaluated_samples == 64)
shortcut = zero_scale_aggregate(50, 50, 2, True)
check('zero-scale shortcut', shortcut.minimum_score_fraction == 1 and shortcut.health_score_fraction == 1 and shortcut.evaluated_samples == 0)
ring = ring_samples()
golden = golden_samples()
check('two deterministic tables have 64 pairs', len(ring) == len(golden) == 64)
check('ring endpoint radii', isclose(hypot(*ring[0]), 0.125) and isclose(hypot(*ring[-1]), 1.0))
check('golden endpoint radii', isclose(hypot(*golden[0]), 0.125) and isclose(hypot(*golden[-1]), 1.0))
check('weight cutoff is strict', estimated_health(100, [(0.75, 20), (0.8, 20)]) == 84)
check('minimum setting clips to health', minimum_damage_threshold(90, 40, False) == 40)
check('over-100 mode uses health', minimum_damage_threshold(101, 41, False) == 41)
check('half-health mode rounds upward', minimum_damage_threshold(101, 41, True) == 21)
check('threshold maximum 130', minimum_damage_threshold(200, 500, False) == 130)
check('attenuation endpoints', distance_attenuation(0.98, 0) == 1.0 and isclose(distance_attenuation(0.98, 500), 0.98))
check('aging keeps newest prefix', retained_count_after_tail_aging([100, 95, 90], 96) == 1)
check('all-stale aging preserves original count in this block', retained_count_after_tail_aging([90, 89], 100) == 2)
check('discontinuity boundary is strict', not resets_history(1, 4096, 8, 8) and resets_history(1, 4097, 8, 8))
check('discontinuity clamp is linear in squared-distance limit', not resets_history(50, 20480, 8, 8) and resets_history(50, 20481, 8, 8))
check('collection mismatch resets', resets_history(1, 0, 7, 8))
result = {
    'passed': len(checks), 'checks': checks,
    'scope': 'Independent mathematical reconstruction only; not equivalence testing of the binary',
    'sample_binary_executed': False,
}
Path(__file__).with_name('math_check_results.json').write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps(result, indent=2))
```
