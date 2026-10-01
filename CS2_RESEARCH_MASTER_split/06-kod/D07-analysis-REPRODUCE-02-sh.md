<!-- split-kod | 04-otchety/D07-analysis-REPRODUCE.md | lang=sh -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D07-analysis-REPRODUCE.md` (язык `sh`)

[← исходная часть](../04-otchety/D07-analysis-REPRODUCE.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D07-analysis-REPRODUCE.md`; в самой части блок остаётся на месте. Символов: 299.

````sh
python3 -m venv analysis/venv
analysis/venv/bin/pip install -r analysis/requirements.txt
analysis/venv/bin/python analysis/scripts/triage.py
analysis/venv/bin/python analysis/scripts/index_code.py
python3 analysis/strings/extract_compact.py
analysis/venv/bin/python analysis/scripts/anchor_xrefs.py
````
