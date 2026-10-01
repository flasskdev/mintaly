<!-- split-kod | 04-otchety/D11-analysis-phase2-lagcomp-summary.md | lang=text -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D11-analysis-phase2-lagcomp-summary.md` (язык `text`)

[← исходная часть](../04-otchety/D11-analysis-phase2-lagcomp-summary.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D11-analysis-phase2-lagcomp-summary.md`; в самой части блок остаётся на месте. Символов: 305.

````text
original_count = object.count
candidate_count = original_count
while candidate_count > 0:
    kept_count = candidate_count
    candidate_count -= 1
    tail = slots[(head + kept_count - 1) % 16]
    if tail.tick < cutoff_tick:
        continue
    object.count = min(original_count, kept_count)
    break
````
