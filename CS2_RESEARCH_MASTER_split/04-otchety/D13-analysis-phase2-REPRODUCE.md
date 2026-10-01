<!-- split-part | CS2_RESEARCH_MASTER.md lines 3105-3173 | body-sha256 6124caeac466102a2e0df8b07e03859d16350dc3b210979b980ec67d95626dd7 -->
[← все части](../README.md) · [индекс отчётов](00-index.md)

<!-- split-body-start -->

<a id="doc-13"></a>

# Приложение D13. `analysis/phase2/REPRODUCE.md`

**Дословный исходный отчёт.** Даты/статусы/неизвестные роли внутри относятся к своему этапу. При расхождении использовать актуальный свод и поздние доказательства. Пути внутри текста — исторические: соответствующие материалы встроены в MD/CPP и находятся через поиск исходного пути.

# Воспроизведение статического анализа этапа 2

## Вход

Исходный raw dump `analysis/input/cs2_212C3300000.bin` из предоставленного RAR; SHA-256 `3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27`. BIN не поставляется заново в компактном архиве. Это raw memory image без рабочего PE header; не запускать его как программу.

Предпосылки: Python 3, wheel Capstone 5.0.9 (его `capstone.__version__` сообщает 5.0.7), Ghidra 12.1.4, JDK21. Скрипты запускаются из каталога, содержащего `analysis/`. SQLite индекс `analysis/results/code.sqlite` и исходные input/imports принадлежат предыдущему этапу; `index_code.py`, `triage.py` и ранний REPRODUCE включены для повторного построения. Существующий индекс имеет неполные линейные ranges; отсутствие ссылки в нём не является доказательством отсутствия кода.

## Условная модель

```sh
analysis/venv/bin/python analysis/phase2/scripts/find_guards.py
analysis/venv/bin/python analysis/phase2/scripts/build_conditional_model.py
```

Это создаёт `analysis/phase2/model/CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin`. Скрипты не перезаписывают исходный sample. Модель принудительно выбирает success targets распознанных environment checks; она **не доказана эквивалентной исходному образу**. См. `main/conditional_model_manifest.json` с точными before/after и assumptions. Замена недекодируемых NOP-hints используется только во временной памяти scanner, не в итоговом BIN-модели.

Для нового Ghidra-проекта:

```sh
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
```

Для остальных target lists применить тот же script в уже созданном проекте через `-process CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin` вместо `-import … -loader … -processor … -cspec …`:

| Targets | Output |
|---|---|
| `phase2/main/core_targets.tsv` | `phase2/conditional_core` |
| `phase2/main/bullet_targets.tsv` | `phase2/conditional_bullet` |
| `phase2/main/lag_targets.tsv` | `phase2/conditional_lag` |
| `phase2/main/target_contexts.tsv` | `phase2/conditional_targets` |
| `phase2/main/damage_multiplier.tsv` | `phase2/conditional_damage` |

Финальный post-script: `ExportEvidence.java "$ROOT/phase2/conditional_assembly"`. Не запускать два headless writers одновременно на одном проекте. `RecoverDump.java` даёт 45 секунд на функцию; `0x4BEB50` не уложился и не выдан за успешный C-output.

`ApplyConditionalGuards.java` использован только при обновлении уже созданной ранней модели v1 (1593 guards) до итоговой (3089 guards). Для нового импорта итоговой BIN-модели не требуется. Он отказывается работать с program name, отличным от `CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin`.

## Проверки

```sh
PYTHONDONTWRITEBYTECODE=1 analysis/venv/bin/python analysis/phase2/scripts/verify_phase2.py
PYTHONDONTWRITEBYTECODE=1 analysis/venv/bin/python analysis/phase2/reconstructed/check_math.py
```

Первая проверка — происхождение и соответствие байтов, branch targets, original/model SHA; вторая — 23 математических случая собственной реконструкции. Ни одна не исполняет sample. Анализ внешних адресов заканчивается на границе доступного dump: код неизвестных модулей не был скачан или подменён.

## Два архива

- `cs2_phase2_analysis.zip`: отчёты, 42 C-листинга, ASM, evidence, независимая математическая модель, скрипты и manifests. Без raw input, модели BIN, инструментов, venv и Ghidra database.
- `cs2_phase2_ghidra_conditional.zip`: законченный отдельный Ghidra-проект с **условным** memory image. Открыть `CONDITIONAL_PATH_MODEL.gpr` в Ghidra 12.1.4. Исходный проект предыдущего этапа не изменялся. Наличие project image не означает runnable DLL.


---
