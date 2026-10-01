<!-- split-kod | 01-obzor/06-health-budget-minimum-damage-tochka.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `01-obzor/06-health-budget-minimum-damage-tochka.md` (язык `text`)

[← исходная часть](../01-obzor/06-health-budget-minimum-damage-tochka.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `01-obzor/06-health-budget-minimum-damage-tochka.md`; в самой части блок остаётся на месте. Символов: 175.

````text
setting <= 100: minimum_score = min(setting, health_budget)
setting > 100:  minimum_score = health_budget
               при mode+0x50==1: ceil(health_budget/2)
затем cap 130
````
