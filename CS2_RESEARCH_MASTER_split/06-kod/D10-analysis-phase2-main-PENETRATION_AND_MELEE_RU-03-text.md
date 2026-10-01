<!-- split-kod | 04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md` (язык `text`)

[← исходная часть](../04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md`; в самой части блок остаётся на месте. Символов: 611.

````text
health_budget = float(entity.int_at_dynamic_offset_17586C4)
for matching_entry in outcome_array_stride_0x1A0:
    if matching_entry.float_at_0x4C > 0.75:
        health_budget -= matching_entry.float_at_0x4C * matching_entry.float_at_0x50
if health_budget <= 0:
    return

setting = context.float_at_0x88
if setting <= 100:
    minimum_score = min(setting, health_budget)
else:
    minimum_score = health_budget
    if context.byte_at_0x50 == 1:
        minimum_score = ceil(health_budget * 0.5)
minimum_score = min(minimum_score, 130)
target.float_at_0x14 = health_budget
target.float_at_0x18 = minimum_score
````
