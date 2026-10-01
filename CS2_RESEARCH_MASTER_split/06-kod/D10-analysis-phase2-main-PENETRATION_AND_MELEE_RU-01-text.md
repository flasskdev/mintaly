<!-- split-kod | 04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md` (язык `text`)

[← исходная часть](../04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md`; в самой части блок остаётся на месте. Символов: 168.

````text
attenuation = exp2(distance * log2(range_parameter) / 500)
            = range_parameter ** (distance / 500)
estimate = base_damage * attenuation * category_multiplier
````
