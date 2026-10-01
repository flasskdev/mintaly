<!-- split-kod | 04-otchety/D09-analysis-phase2-spread-summary.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D09-analysis-phase2-spread-summary.md` (язык `text`)

[← исходная часть](../04-otchety/D09-analysis-phase2-spread-summary.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D09-analysis-phase2-spread-summary.md`; в самой части блок остаётся на месте. Символов: 127.

````text
direction = normalize(forward + sample.x*basis1 + sample.y*basis2)
endpoint  = source.xyz + context.float_at_0x100 * direction
````
