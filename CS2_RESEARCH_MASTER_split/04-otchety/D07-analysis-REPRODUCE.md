<!-- split-part | CS2_RESEARCH_MASTER.md lines 2325-2414 | body-sha256 2cc1b706f88f8debae894f669571353e40ae744c45a4edb163ec288b6845b7a4 -->
[← все части](../README.md) · [индекс отчётов](00-index.md)

<!-- split-body-start -->

<a id="doc-07"></a>

# Приложение D07. `analysis/REPRODUCE.md`

**Дословный исходный отчёт.** Даты/статусы/неизвестные роли внутри относятся к своему этапу. При расхождении использовать актуальный свод и поздние доказательства. Пути внутри текста — исторические: соответствующие материалы встроены в MD/CPP и находятся через поиск исходного пути.

# Воспроизведение статического анализа

## Важно

Эти материалы не являются рабочим исходным кодом DLL. Не запускайте BIN/DLL, не загружайте его через Wine/LoadLibrary и не используйте загрузчик игры. Все описанные действия читают байты статически. Инструменты декомпиляции также имеют собственную поверхность атаки; анализ неизвестных файлов следует выполнять в изолированной среде.

В архиве анализа исходный BIN и дистрибутивы инструментов не включены. Отдельный Ghidra-проект содержит импортированное представление исходных байтов. Сохранены аналитические имена `FUN_<VA>`, а не выдуманные исходные имена.

## Размещение

Распакуйте основной архив так, чтобы текущая директория содержала `analysis/`. Из исходного RAR извлеките единственный файл в `analysis/input/cs2_212C3300000.bin`. Проверьте SHA-256:

```text
3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27
```

Использованный `libarchive/bsdtar` распаковал архив. Debian 7zip смог перечислить содержимое, но не извлечь RAR method; это ограничение конкретной сборки инструмента, не доказательство повреждения архива.

## Python

```sh
python3 -m venv analysis/venv
analysis/venv/bin/pip install -r analysis/requirements.txt
analysis/venv/bin/python analysis/scripts/triage.py
analysis/venv/bin/python analysis/scripts/index_code.py
python3 analysis/strings/extract_compact.py
analysis/venv/bin/python analysis/scripts/anchor_xrefs.py
```

`index_code.py` пересоздаёт `results/code.sqlite`. Строковые и кодовые индексы — эвристические: их отрицательные результаты не подтверждают отсутствие функции. Предоставленная таблица импортов сохранена в `imports/user_imports_source.txt`/CSV; это не автоматически восстановленная import directory.

## Ghidra

Использована официальная **Ghidra 12.1.4 PUBLIC**, OpenJDK 21.0.12.1. Исходный релиз:

https://github.com/NationalSecurityAgency/ghidra/releases/tag/Ghidra_12.1.4_build

Дистрибутив `ghidra_12.1.4_PUBLIC_20260921.zip`, SHA-256:

```text
ddac49f903da9d5bac833e5cc79395098b9c33cfd3279be5f31bd00387d2d4db
```

Откройте отдельный проект `CS2_static.gpr` вместе с каталогом `CS2_static.rep`, либо импортируйте BIN заново как **Raw Binary**, x86 little-endian 64, compiler spec Windows, base `0x212C3300000`.

Пример воспроизведения из родительской директории `analysis` (замените `/path/to/ghidra` на путь к установленному официальному инструменту):

```sh
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
```

`-noanalysis` намеренно исключает неконтролируемый полный auto-analysis 80 MiB образа; скрипт размечает выбранные диапазоны. Лимит на декомпиляцию одной функции — 45 секунд. Не все функции получены успешно: смотреть `decompiled_*/index.tsv`.

## Проверка результатов

```sh
analysis/venv/bin/python analysis/scripts/verify_results.py
```

Проверяются размер/hash входа, точный перенос таблицы импортов, адреса/байты ASCII anchors, целостность SQLite, совпадение каждой сохранённой инструкции Ghidra с исходными байтами и наличие успешных C-выходов. Это **не** проверка полной семантической эквивалентности, корректности всех типов или полноты rage-функций.

`MANIFEST_SHA256.txt` содержит хеши файлов основного архива. Проверка из его корня: `sha256sum -c MANIFEST_SHA256.txt`.


---
