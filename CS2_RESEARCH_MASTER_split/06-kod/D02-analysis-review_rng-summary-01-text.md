<!-- split-kod | 04-otchety/D02-analysis-review_rng-summary.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D02-analysis-review_rng-summary.md` (язык `text`)

[← исходная часть](../04-otchety/D02-analysis-review_rng-summary.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D02-analysis-review_rng-summary.md`; в самой части блок остаётся на месте. Символов: 583.

````text
theta = returned_by_35eab50[1] * pi/180
forward = (cos(theta), sin(theta), 0)
right   = (-sin(theta), cos(theta), 0)
up      = (0, 0, 1)

если param_3 != null:
    B = *param_3                  # три исходных float
    A = B + right*U(-1200,1200)
          + up*(2500*U(0.9,1.1))
          + forward*I(-600,600)

иначе:
    O = singleton_4a6f378.xyz_at_0x10
    C = O + forward*I(3000,4000)
          + right*U(-1200,1200)
          + up*I(350,650)
    w = (I(0,1)==0 ? -1 : +1) * 750*U(0.85,1.15)
    B = C - right*w
    A = C + right*w + up*(2500*U(0.9,1.1)) + forward*I(-600,600)
````
