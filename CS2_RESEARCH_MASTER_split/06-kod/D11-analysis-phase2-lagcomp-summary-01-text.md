<!-- split-kod | 04-otchety/D11-analysis-phase2-lagcomp-summary.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D11-analysis-phase2-lagcomp-summary.md` (язык `text`)

[← исходная часть](../04-otchety/D11-analysis-phase2-lagcomp-summary.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D11-analysis-phase2-lagcomp-summary.md`; в самой части блок остаётся на месте. Символов: 160.

````text
base_time = mode==1 ? object.float_at_4C : float_at(helper_2B7280()+0xC0)
window = helper_473490()
cutoff_tick = cvttss2si((base_time - window) * 64.0f + 0.5f)
````
