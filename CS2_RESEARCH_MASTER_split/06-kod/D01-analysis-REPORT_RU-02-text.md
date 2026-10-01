<!-- split-kod | 04-otchety/D01-analysis-REPORT_RU.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D01-analysis-REPORT_RU.md` (язык `text`)

[← исходная часть](../04-otchety/D01-analysis-REPORT_RU.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D01-analysis-REPORT_RU.md`; в самой части блок остаётся на месте. Символов: 153.

````text
upper_seconds = min(weapon_related_value, 0.1)
lower_seconds = max(upper_seconds - 0.05, 0)
delta_ticks = RandomFloat(lower_seconds, upper_seconds) * 64
````
