<!-- split-kod | 04-otchety/D07-analysis-REPRODUCE.md | lang=sh -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D07-analysis-REPRODUCE.md` (язык `sh`)

[← исходная часть](../04-otchety/D07-analysis-REPRODUCE.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D07-analysis-REPRODUCE.md`; в самой части блок остаётся на месте. Символов: 992.

````sh
mkdir -p analysis/ghidra_project
/path/to/ghidra/support/analyzeHeadless analysis/ghidra_project CS2_static \
  -import analysis/input/cs2_212C3300000.bin \
  -loader BinaryLoader -loader-baseAddr 0x212C3300000 \
  -processor x86:LE:64:default -cspec windows -noanalysis \
  -scriptPath analysis/scripts \
  -postScript RecoverDump.java "$PWD/analysis" results/rng_targets.tsv decompiled_rng \
  -max-cpu 2

/path/to/ghidra/support/analyzeHeadless analysis/ghidra_project CS2_static \
  -process cs2_212C3300000.bin -noanalysis -scriptPath analysis/scripts \
  -postScript RecoverDump.java "$PWD/analysis" results/feature_targets.tsv decompiled_features \
  -max-cpu 2

/path/to/ghidra/support/analyzeHeadless analysis/ghidra_project CS2_static \
  -process cs2_212C3300000.bin -noanalysis -scriptPath analysis/scripts \
  -postScript RecoverDump.java "$PWD/analysis" results/core_targets.tsv decompiled_core \
  -postScript ExportEvidence.java "$PWD/analysis/ghidra_assembly" \
  -max-cpu 2
````
