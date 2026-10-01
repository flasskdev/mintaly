<!-- split-kod | 04-otchety/D02-analysis-review_rng-summary.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D02-analysis-review_rng-summary.md` (язык `text`)

[← исходная часть](../04-otchety/D02-analysis-review_rng-summary.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D02-analysis-review_rng-summary.md`; в самой части блок остаётся на месте. Символов: 121.

````text
yaw   = atan2(D.y, D.x) * 180/pi
pitch = atan2(-D.z, sqrt(D.x²+D.y²)) * 180/pi
normalize(a) = a - 360*floor(a/360 + 0.5)
````
