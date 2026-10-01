<!-- split-part | CS2_RESEARCH_MASTER.md lines 895-911 | body-sha256 63cdb95400550d82254c892574b4c310bc60070b1c7f115afbf3396f6b60f9cd -->
[← все части](../README.md) · [индекс порта](00-index.md)

<!-- split-body-start -->

## API and preserved semantics

`SampleOutcome` and `Aggregates` retain the Python field names and immutable fields through `const` C++ members. `SampleOutcome` retains `valid = true` and `category = 0`. Scalars use `double`; categories, tick values, and collection-size arguments use `std::int64_t`; counts use `std::size_t`. Sample tables are `std::array<std::pair<double, double>, 64>`. Input sequences use vectors. Python `_checked_outcomes` is exposed as `checked_outcomes`; the remaining function names are unchanged. The complete 11-function mapping is recorded in JSON.

- `ring_samples`: same ring-major order, eight rings and eight sectors, scale multiplication, and angle formula.
- `golden_samples`: same 64 indices, square-root radius, and exact written step constant `2.399963140487671`.
- `checked_outcomes`: requires exactly 64 outcomes, then positive finite thresholds, then finite scores with strictly positive valid scores. Invalid finite scores normalize to zero while keeping their category; invalid nonfinite scores still fail. The complete sequence is validated before any fast early-fill. Python `ValueError` becomes `std::invalid_argument`, preserving messages and validation order.
- `aggregate_full`: minimum and health thresholds both include equality; invalid outcomes contribute zero; all 64 outcomes contribute to the denominator and evaluated count.
- `aggregate_fast`: minimum includes equality but health uses strict `>`; blocks run from 7 down to 0, preserving ascending sample order within each block. Early-fill needs all eight minimum successes and either zero or eight health successes. Only an all-special triggering block fills remaining special counts; the mean extrapolates the triggering block mean. Already processed blocks retain their measured contributions.
- Special categories use `(category & 0xfe) == 2`, not just equality with category 2. Casting to unsigned before the mask preserves the low-bit behavior for representable negative categories as well.
- `zero_scale_aggregate`: unconditional minimum fraction 1, inclusive health comparison, gated special fraction, unchanged central mean, and zero evaluations. It deliberately does not inherit outcome/threshold positivity checks.
- `estimated_health`: subtracts weight times damage only for strict `weight > 0.75`, with no final health clamp.
- `minimum_damage_threshold`: rejects nonpositive health; only strict `setting > 100` enters the health/rounded-up half-health branch; otherwise selects `min(setting, health)`. The result is capped at 130, with no invented lower clamp.
- `distance_attenuation`: `pow(range_parameter, distance / 500.0)`, preserving the positive-range/nonnegative-distance validation and allowing range parameters above 1.
- `retained_count_after_tail_aging`: scans the supplied sequence backward without sorting; equality is fresh; empty input returns zero; an all-stale sequence intentionally retains its original count.
- `resets_history`: clamps delta to 1 through 5, uses strict `squared_distance > 4096 * factor`, or resets on collection-size mismatch.
