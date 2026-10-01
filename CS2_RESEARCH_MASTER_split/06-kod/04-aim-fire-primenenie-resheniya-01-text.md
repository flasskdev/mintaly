<!-- split-kod | 01-obzor/04-aim-fire-primenenie-resheniya.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `01-obzor/04-aim-fire-primenenie-resheniya.md` (язык `text`)

[← исходная часть](../01-obzor/04-aim-fire-primenenie-resheniya.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `01-obzor/04-aim-fire-primenenie-resheniya.md`; в самой части блок остаётся на месте. Символов: 113.

````text
upper = min(weapon_related_value, 0.1)
lower = max(upper - 0.05, 0)
delta_ticks = RandomFloat(lower, upper) * 64
````
