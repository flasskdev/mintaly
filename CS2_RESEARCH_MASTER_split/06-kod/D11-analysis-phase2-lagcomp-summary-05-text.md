<!-- split-kod | 04-otchety/D11-analysis-phase2-lagcomp-summary.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D11-analysis-phase2-lagcomp-summary.md` (язык `text`)

[← исходная часть](../04-otchety/D11-analysis-phase2-lagcomp-summary.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D11-analysis-phase2-lagcomp-summary.md`; в самой части блок остаётся на месте. Символов: 291.

````text
delta_tick = newest.tick - previous.tick
factor = clamp(delta_tick, 1, 5)
limit_squared = float(factor << 12)       # 4096 * factor
if squared_distance_XYZ(newest, previous) > limit_squared:
    count = 1
else if newest.collection_size_at_A0 != previous.collection_size_at_A0:
    count = 1
````
