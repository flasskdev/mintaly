<!-- split-kod | 04-otchety/D13-analysis-phase2-REPRODUCE.md | lang=sh -->
<!-- split-body-start -->
# Code-блок из `04-otchety/D13-analysis-phase2-REPRODUCE.md` (язык `sh`)

[← исходная часть](../04-otchety/D13-analysis-phase2-REPRODUCE.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `04-otchety/D13-analysis-phase2-REPRODUCE.md`; в самой части блок остаётся на месте. Символов: 535.

````sh
GHIDRA=analysis/tools/ghidra_12.1.4_PUBLIC/support/analyzeHeadless
ROOT="$PWD/analysis"
mkdir -p analysis/phase2/ghidra_conditional
"$GHIDRA" analysis/phase2/ghidra_conditional CONDITIONAL_PATH_MODEL \
  -import "$ROOT/phase2/model/CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin" \
  -loader BinaryLoader -loader-baseAddr 0x212C3300000 \
  -processor x86:LE:64:default -cspec windows -noanalysis \
  -scriptPath "$ROOT/scripts" \
  -postScript RecoverDump.java "$ROOT" phase2/main/trace_targets.tsv phase2/conditional_trace \
  -max-cpu 2
````
