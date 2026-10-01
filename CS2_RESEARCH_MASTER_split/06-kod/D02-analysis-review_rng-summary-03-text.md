<!-- split-kod | 04-otchety/D02-analysis-review_rng-summary.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D02-analysis-review_rng-summary.md` (язык `text`)

[← исходная часть](../04-otchety/D02-analysis-review_rng-summary.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D02-analysis-review_rng-summary.md`; в самой части блок остаётся на месте. Символов: 205.

````text
upper_seconds = min(weapon_related_value, 0.1)
lower_seconds = max(upper_seconds - 0.05, 0)
delta_ticks = U(lower_seconds, upper_seconds) * 64
(this+0x4a4, this+0x4a8) = normalized_tick_pair + delta_ticks
````
