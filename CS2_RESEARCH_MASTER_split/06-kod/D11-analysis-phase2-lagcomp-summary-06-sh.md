<!-- split-kod | 04-otchety/D11-analysis-phase2-lagcomp-summary.md | lang=sh -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D11-analysis-phase2-lagcomp-summary.md` (язык `sh`)

[← исходная часть](../04-otchety/D11-analysis-phase2-lagcomp-summary.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D11-analysis-phase2-lagcomp-summary.md`; в самой части блок остаётся на месте. Символов: 259.

````sh
PYTHONDONTWRITEBYTECODE=1 analysis/venv/bin/python analysis/phase2/lagcomp/analyze.py > analysis/phase2/lagcomp/scan.log
PYTHONDONTWRITEBYTECODE=1 analysis/venv/bin/python analysis/phase2/lagcomp/build_evidence.py > analysis/phase2/lagcomp/evidence_check.log
````
