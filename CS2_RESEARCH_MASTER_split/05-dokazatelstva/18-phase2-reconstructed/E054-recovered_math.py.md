<!-- split-part | CS2_RESEARCH_MASTER.md lines 60859-61001 | body-sha256 03991e97c1ae3f240fbd3d42bd22d4f96bf48c0cfd2a5d5639b11a2af74ca177 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-054"></a>

## E054. `analysis/phase2/reconstructed/recovered_math.py`

Bytes: 5739. SHA-256: `75e4f25e551afbd9a7eb5dc0fc9e821eee02e605493374eddc6e81ab8ba83fe6`.

```python
from dataclasses import dataclass
from math import ceil, cos, isfinite, pi, sin, sqrt
from typing import Sequence


@dataclass(frozen=True)
class SampleOutcome:
    score: float
    valid: bool = True
    category: int = 0


@dataclass(frozen=True)
class Aggregates:
    minimum_score_fraction: float
    health_score_fraction: float
    special_fraction: float
    mean_score: float
    evaluated_samples: int


def ring_samples(scale: float = 1.0) -> list[tuple[float, float]]:
    return [
        (scale * ring / 8.0 * cos(ring * pi / 8.0 + sector * pi / 4.0),
         scale * ring / 8.0 * sin(ring * pi / 8.0 + sector * pi / 4.0))
        for ring in range(1, 9)
        for sector in range(8)
    ]


def golden_samples(scale: float = 1.0) -> list[tuple[float, float]]:
    angle_step = 2.399963140487671
    return [
        (scale * sqrt((index + 1) / 64.0) * cos(index * angle_step),
         scale * sqrt((index + 1) / 64.0) * sin(index * angle_step))
        for index in range(64)
    ]


def _checked_outcomes(outcomes: Sequence[SampleOutcome], minimum_score: float,
                      health_score: float) -> list[SampleOutcome]:
    if len(outcomes) != 64:
        raise ValueError('Exactly 64 already-evaluated sample outcomes are required')
    if not all(isfinite(value) and value > 0 for value in (minimum_score, health_score)):
        raise ValueError('This mathematical model covers positive finite thresholds only')
    if any(not isfinite(outcome.score) or (outcome.valid and outcome.score <= 0)
           for outcome in outcomes):
        raise ValueError('Valid sample scores must be positive and finite')
    return [outcome if outcome.valid else SampleOutcome(0.0, False, outcome.category)
            for outcome in outcomes]


def aggregate_full(outcomes: Sequence[SampleOutcome], minimum_score: float,
                   health_score: float, special_enabled: bool) -> Aggregates:
    samples = _checked_outcomes(outcomes, minimum_score, health_score)
    count_minimum = sum(sample.valid and sample.score >= minimum_score for sample in samples)
    count_health = sum(sample.score >= health_score for sample in samples)
    count_special = sum(sample.valid and sample.category & 0xfe == 2 for sample in samples)
    return Aggregates(count_minimum / 64.0, count_health / 64.0,
                      count_special / 64.0 if special_enabled else 0.0,
                      sum(sample.score for sample in samples) / 64.0, 64)


def aggregate_fast(outcomes: Sequence[SampleOutcome], minimum_score: float,
                   health_score: float, special_enabled: bool) -> Aggregates:
    samples = _checked_outcomes(outcomes, minimum_score, health_score)
    count_minimum = 0
    count_health = 0
    count_special = 0
    sum_score = 0.0
    evaluated = 0
    for block_index in range(7, -1, -1):
        block = samples[block_index * 8:(block_index + 1) * 8]
        block_minimum = sum(sample.valid and sample.score >= minimum_score for sample in block)
        block_health = sum(sample.score > health_score for sample in block)
        block_special = sum(sample.valid and sample.category & 0xfe == 2 for sample in block)
        block_sum = sum(sample.score for sample in block)
        count_minimum += block_minimum
        count_health += block_health
        count_special += block_special
        sum_score += block_sum
        evaluated += 8
        if block_minimum == 8 and block_health in (0, 8):
            remaining = block_index * 8
            count_minimum += remaining
            count_health += remaining if block_health == 8 else 0
            count_special += remaining if block_special == 8 else 0
            sum_score += remaining * block_sum / 8.0
            break
    return Aggregates(count_minimum / 64.0, count_health / 64.0,
                      count_special / 64.0 if special_enabled else 0.0,
                      sum_score / 64.0, evaluated)


def zero_scale_aggregate(central_score: float, health_score: float,
                         category: int, special_enabled: bool) -> Aggregates:
    return Aggregates(1.0, float(central_score >= health_score),
                      float(special_enabled and category & 0xfe == 2), central_score, 0)


def estimated_health(base_health: float, matching_weight_damage: Sequence[tuple[float, float]]) -> float:
    return base_health - sum(weight * damage for weight, damage in matching_weight_damage
                             if weight > 0.75)


def minimum_damage_threshold(setting: float, health: float, half_health_mode: bool) -> float:
    if health <= 0:
        raise ValueError('The original caller returns before candidate generation for nonpositive health')
    if setting > 100:
        selected = float(ceil(health * 0.5)) if half_health_mode else health
    else:
        selected = min(setting, health)
    return min(selected, 130.0)


def distance_attenuation(range_parameter: float, distance: float) -> float:
    if range_parameter <= 0 or distance < 0:
        raise ValueError('This model covers positive range parameters and nonnegative distances only')
    return range_parameter ** (distance / 500.0)


def retained_count_after_tail_aging(newest_first_ticks: Sequence[int], cutoff_tick: int) -> int:
    original_count = len(newest_first_ticks)
    for kept_count in range(original_count, 0, -1):
        if newest_first_ticks[kept_count - 1] >= cutoff_tick:
            return kept_count
    return original_count


def resets_history(delta_tick: int, squared_distance: float,
                   new_collection_size: int, previous_collection_size: int) -> bool:
    factor = min(max(delta_tick, 1), 5)
    return squared_distance > 4096 * factor or new_collection_size != previous_collection_size
```
