<!-- split-kod | 04-otchety/D11-analysis-phase2-lagcomp-summary.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D11-analysis-phase2-lagcomp-summary.md` (язык `text`)

[← исходная часть](../04-otchety/D11-analysis-phase2-lagcomp-summary.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D11-analysis-phase2-lagcomp-summary.md`; в самой части блок остаётся на месте. Символов: 91.

````text
head = (head + 15) % 16
count = min(count + 1, 16)
new_record = object + 0xF0 + head*0x500
````
