<!-- split-kod | 04-otchety/D01-analysis-REPORT_RU.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D01-analysis-REPORT_RU.md` (язык `text`)

[← исходная часть](../04-otchety/D01-analysis-REPORT_RU.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D01-analysis-REPORT_RU.md`; в самой части блок остаётся на месте. Символов: 188.

````text
delta = target_point - origin_point
yaw   = atan2(delta.y, delta.x) * 180/pi
pitch = atan2(-delta.z, sqrt(delta.x² + delta.y²)) * 180/pi
wrap(angle) = angle - 360 * floor(angle/360 + 0.5)
````
