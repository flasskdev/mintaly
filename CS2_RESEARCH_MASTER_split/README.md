# CS2_RESEARCH_MASTER — разбивка на мелкие части

Источник: [CS2_RESEARCH_MASTER.md](../CS2_RESEARCH_MASTER.md) — 101453 строк, SHA-256 `1345d01e11154aa6e78cd994880496b372aa958b3be7c8bb6f55c72e54625828`.

Скрипт разбивки: [split_master_md.py](../split_master_md.py) · проверка целостности: [_verify.md](_verify.md) · карта строк: [_source-map.tsv](_source-map.tsv).

## Куда смотреть по темам

| Тема | Часть |
|---|---|
| Как читать свод | [01-obzor/00-kak-chitat.md](01-obzor/00-kak-chitat.md) |
| Краткий актуальный результат | [01-obzor/01-kratkiy-rezultat.md](01-obzor/01-kratkiy-rezultat.md) |
| Входные данные и адресация | [01-obzor/02-vhodnye-dannye-i-adresaciya.md](01-obzor/02-vhodnye-dannye-i-adresaciya.md) |
| Метод и уровни достоверности | [01-obzor/03-metod-i-urovni-dostovernosti.md](01-obzor/03-metod-i-urovni-dostovernosti.md) |
| Aim/fire — применение решения | [01-obzor/04-aim-fire-primenenie-resheniya.md](01-obzor/04-aim-fire-primenenie-resheniya.md) |
| Hitchance и генераторы направлений | [01-obzor/05-hitchance-i-generatory-napravleniy.md](01-obzor/05-hitchance-i-generatory-napravleniy.md) |
| Health budget, minimum damage, выбор точки | [01-obzor/06-health-budget-minimum-damage-tochka.md](01-obzor/06-health-budget-minimum-damage-tochka.md) |
| Damage/penetration и отсутствующий внешний код | [01-obzor/07-damage-penetration-i-vneshniy-kod.md](01-obzor/07-damage-penetration-i-vneshniy-kod.md) |
| История и lag-логика | [01-obzor/08-istoriya-i-lag-logika.md](01-obzor/08-istoriya-i-lag-logika.md) |
| Melee/knifebot | [01-obzor/09-melee-knifebot.md](01-obzor/09-melee-knifebot.md) |
| Resolver и ложные совпадения | [01-obzor/10-resolver-i-lozhnye-sovpadeniya.md](01-obzor/10-resolver-i-lozhnye-sovpadeniya.md) |
| Устройство CPP/H и `#if 0` | [01-obzor/11-ustroystvo-cpp-i-h.md](01-obzor/11-ustroystvo-cpp-i-h.md) |
| Контроль полноты и воспроизводимости | [01-obzor/12-kontrol-polnoty-i-vosproizvodimosti.md](01-obzor/12-kontrol-polnoty-i-vosproizvodimosti.md) |
| Что не появилось от объединения | [01-obzor/13-chego-ne-poyavilos-ot-obedineniya.md](01-obzor/13-chego-ne-poyavilos-ot-obedineniya.md) |
| Автопаспорт объединения | [01-obzor/14-avtomaticheskiy-pasport-obedineniya.md](01-obzor/14-avtomaticheskiy-pasport-obedineniya.md) |
| Порт C++17-математики | [02-cpp-model/00-port-i-delivery-and-scope.md](02-cpp-model/00-port-i-delivery-and-scope.md) |
| Навигация по отчётам D01–D13 | [03-katalogi/15-navigaciya-po-otchetam.md](03-katalogi/15-navigaciya-po-otchetam.md) |
| Каталог 63 C-псевдокодов | [03-katalogi/16-katalog-c-psevdokodov.md](03-katalogi/16-katalog-c-psevdokodov.md) |
| Каталог ASM/архивных блоков № 64–218 | [03-katalogi/17-katalog-asm-i-arhivnyh-blokov.md](03-katalogi/17-katalog-asm-i-arhivnyh-blokov.md) |
| Отчёты D01–D13 (дословно) | [04-otchety/00-index.md](04-otchety/00-index.md) |
| Машинные свидетельства E001–E184 | [05-dokazatelstva/00-index.md](05-dokazatelstva/00-index.md) |
| Code-блоки обзора/порта/отчётов | [06-kod/00-index.md](06-kod/00-index.md) |
| Приложение F | [07-prilozhenie-f.md](07-prilozhenie-f.md) |

## Как устроена папка

```text
01-obzor/              вводный свод: как читать + разделы 1–14
02-cpp-model/          документация порта CS2_RECONSTRUCTION.cpp/.h
03-katalogi/           разделы 15–17: навигация, псевдокод, ASM
  pseudocode-c/        карточки 63 C-псевдокодов
  asm/                 карточки ASM/архивных блоков № 64–218 по 13 категориям
04-otchety/            приложения D01–D13 (дословно)
05-dokazatelstva/      приложения E: manifest + E001–E184 по категориям
06-kod/                code-блоки обзора/порта/отчётов (копии)
07-prilozhenie-f.md    приложение F: крупные индексы вне текста
```

## Состав

- Основных частей: **224** (дословные срезы исходника, каждая строка входит ровно в одну).
- Карточек C-псевдокода: **63** — [индекс](03-katalogi/pseudocode-c/00-index.md).
- Карточек ASM/архивных блоков: **155** — [индекс](03-katalogi/asm/00-index.md).
- Вырезанных code-блоков: **37** — [индекс](06-kod/00-index.md).
- Отчётов D01–D13: **13** — [индекс](04-otchety/00-index.md).
- Свидетельств E001–E184: **184** — [manifest](05-dokazatelstva/00-manifest.md), [индекс](05-dokazatelstva/00-index.md).

## Проверка целостности

- Склейка тел всех основных частей совпадает с исходником байт-в-байт: **да** (SHA-256 `1345d01e11154aa6e78cd994880496b372aa958b3be7c8bb6f55c72e54625828`).
- Каждая часть начинается генерируемым заголовком и строкой навигации; неизменный текст начинается после `<!-- split-body-start -->`.
- Карточки и code-блоки — производные копии, они не участвуют в склейке; табличные строки и блоки остаются и в исходных частях.
- Исходный `CS2_RESEARCH_MASTER.md` не изменялся и не удалялся.

## Раздел 16 и 17: псевдокод и ASM

- [Каталог C-псевдокодов](03-katalogi/16-katalog-c-psevdokodov.md) — 63 листинга из архивного `CS2_RECONSTRUCTION.cpp`; на каждый сделана карточка с RVA/VA, ролью, источником и строкой CPP.
- [Каталог ASM и архивных блоков](03-katalogi/17-katalog-asm-i-arhivnyh-blokov.md) — номера 64–218: 125 ASM-листингов, 29 report_fragment-вставок и python-модель; карточки сгруппированы по исходным папкам анализа.

## `01-obzor`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [00-kak-chitat.md](01-obzor/00-kak-chitat.md) | 1–15 | Как читать этот документ |
| [01-kratkiy-rezultat.md](01-obzor/01-kratkiy-rezultat.md) | 16–30 | 1. Краткий актуальный результат |
| [02-vhodnye-dannye-i-adresaciya.md](01-obzor/02-vhodnye-dannye-i-adresaciya.md) | 31–47 | 2. Входные данные и адресация |
| [03-metod-i-urovni-dostovernosti.md](01-obzor/03-metod-i-urovni-dostovernosti.md) | 48–69 | 3. Метод и уровни достоверности |
| [04-aim-fire-primenenie-resheniya.md](01-obzor/04-aim-fire-primenenie-resheniya.md) | 70–101 | 4. Aim/fire — применение решения, не выбор лучшей цели |
| [05-hitchance-i-generatory-napravleniy.md](01-obzor/05-hitchance-i-generatory-napravleniy.md) | 102–126 | 5. Hitchance и генераторы направлений |
| [06-health-budget-minimum-damage-tochka.md](01-obzor/06-health-budget-minimum-damage-tochka.md) | 127–143 | 6. Health budget, minimum damage и выбор точки |
| [07-damage-penetration-i-vneshniy-kod.md](01-obzor/07-damage-penetration-i-vneshniy-kod.md) | 144–159 | 7. Damage/penetration и отсутствующий внешний код |
| [08-istoriya-i-lag-logika.md](01-obzor/08-istoriya-i-lag-logika.md) | 160–171 | 8. История и lag-related логика |
| [09-melee-knifebot.md](01-obzor/09-melee-knifebot.md) | 172–177 | 9. Melee/knifebot |
| [10-resolver-i-lozhnye-sovpadeniya.md](01-obzor/10-resolver-i-lozhnye-sovpadeniya.md) | 178–193 | 10. Resolver и ложные совпадения |
| [11-ustroystvo-cpp-i-h.md](01-obzor/11-ustroystvo-cpp-i-h.md) | 194–209 | 11. Как устроены CPP/H и почему есть `#if 0` |
| [12-kontrol-polnoty-i-vosproizvodimosti.md](01-obzor/12-kontrol-polnoty-i-vosproizvodimosti.md) | 210–217 | 12. Контроль полноты и воспроизводимости |
| [13-chego-ne-poyavilos-ot-obedineniya.md](01-obzor/13-chego-ne-poyavilos-ot-obedineniya.md) | 218–223 | 13. Что не появилось от объединения |
| [14-avtomaticheskiy-pasport-obedineniya.md](01-obzor/14-avtomaticheskiy-pasport-obedineniya.md) | 224–878 | 14. Автоматический паспорт объединения |

## `02-cpp-model`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [00-port-i-delivery-and-scope.md](02-cpp-model/00-port-i-delivery-and-scope.md) | 879–894 | C++17 mathematical reconstruction port |
| [01-api-and-preserved-semantics.md](02-cpp-model/01-api-and-preserved-semantics.md) | 895–911 | API and preserved semantics |
| [02-validation-results.md](02-cpp-model/02-validation-results.md) | 912–925 | Validation results |
| [03-reproduction-commands.md](02-cpp-model/03-reproduction-commands.md) | 926–953 | Reproduction commands |
| [04-main-assembly-contract.md](02-cpp-model/04-main-assembly-contract.md) | 954–966 | Main assembly contract |
| [05-port-limitations.md](02-cpp-model/05-port-limitations.md) | 967–973 | Port limitations |
| [06-proverka-itogovogo-cpp-h.md](02-cpp-model/06-proverka-itogovogo-cpp-h.md) | 974–1481 | Проверка именно итогового большого CPP/H |

## `03-katalogi`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [15-navigaciya-po-otchetam.md](03-katalogi/15-navigaciya-po-otchetam.md) | 1482–1500 | 15. Навигация по исходным отчётам |
| [16-katalog-c-psevdokodov.md](03-katalogi/16-katalog-c-psevdokodov.md) | 1501–1570 | 16. Каталог всех C-псевдокодов |
| [17-katalog-asm-i-arhivnyh-blokov.md](03-katalogi/17-katalog-asm-i-arhivnyh-blokov.md) | 1571–1733 | 17. Каталог ASM и дополнительных архивных блоков |

## `04-otchety`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [D01-analysis-REPORT_RU.md](04-otchety/D01-analysis-REPORT_RU.md) | 1734–1888 | Приложение D01. `analysis/REPORT_RU.md` |
| [D02-analysis-review_rng-summary.md](04-otchety/D02-analysis-review_rng-summary.md) | 1889–2113 | Приложение D02. `analysis/review_rng/summary.md` |
| [D03-analysis-review_rng-addendum_vector_sampling.md](04-otchety/D03-analysis-review_rng-addendum_vector_sampling.md) | 2114–2160 | Приложение D03. `analysis/review_rng/addendum_vector_sampling.md` |
| [D04-analysis-CORE_ADDENDUM_RU.md](04-otchety/D04-analysis-CORE_ADDENDUM_RU.md) | 2161–2236 | Приложение D04. `analysis/CORE_ADDENDUM_RU.md` |
| [D05-analysis-review_rng-shortlist.md](04-otchety/D05-analysis-review_rng-shortlist.md) | 2237–2261 | Приложение D05. `analysis/review_rng/shortlist.md` |
| [D06-analysis-strings-summary.md](04-otchety/D06-analysis-strings-summary.md) | 2262–2324 | Приложение D06. `analysis/strings/summary.md` |
| [D07-analysis-REPRODUCE.md](04-otchety/D07-analysis-REPRODUCE.md) | 2325–2414 | Приложение D07. `analysis/REPRODUCE.md` |
| [D08-analysis-phase2-REPORT_RU.md](04-otchety/D08-analysis-phase2-REPORT_RU.md) | 2415–2543 | Приложение D08. `analysis/phase2/REPORT_RU.md` |
| [D09-analysis-phase2-spread-summary.md](04-otchety/D09-analysis-phase2-spread-summary.md) | 2544–2640 | Приложение D09. `analysis/phase2/spread/summary.md` |
| [D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md](04-otchety/D10-analysis-phase2-main-PENETRATION_AND_MELEE_RU.md) | 2641–2828 | Приложение D10. `analysis/phase2/main/PENETRATION_AND_MELEE_RU.md` |
| [D11-analysis-phase2-lagcomp-summary.md](04-otchety/D11-analysis-phase2-lagcomp-summary.md) | 2829–3066 | Приложение D11. `analysis/phase2/lagcomp/summary.md` |
| [D12-analysis-phase2-reconstructed-README.md](04-otchety/D12-analysis-phase2-reconstructed-README.md) | 3067–3104 | Приложение D12. `analysis/phase2/reconstructed/README.md` |
| [D13-analysis-phase2-REPRODUCE.md](04-otchety/D13-analysis-phase2-REPRODUCE.md) | 3105–3173 | Приложение D13. `analysis/phase2/REPRODUCE.md` |

## `05-dokazatelstva`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [00-manifest.md](05-dokazatelstva/00-manifest.md) | 3174–3364 | Приложения E. Машинные свидетельства и воспроизводимые скрипты |

## `05-dokazatelstva/01-consolidated`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E001-assemble_bundle.py.md](05-dokazatelstva/01-consolidated/E001-assemble_bundle.py.md) | 3365–3824 | E001. `/home/daytona/albigg/analysis/consolidated/assemble_bundle.py` |

## `05-dokazatelstva/02-consolidated-staging-math`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E002-CS2_RECONSTRUCTION.h.md](05-dokazatelstva/02-consolidated-staging-math/E002-CS2_RECONSTRUCTION.h.md) | 3825–3891 | E002. `analysis/consolidated/staging/math/CS2_RECONSTRUCTION.h` |
| [E003-model_checks.cpp.md](05-dokazatelstva/02-consolidated-staging-math/E003-model_checks.cpp.md) | 3892–4319 | E003. `analysis/consolidated/staging/math/model_checks.cpp` |
| [E004-model_impl.cpp.md](05-dokazatelstva/02-consolidated-staging-math/E004-model_impl.cpp.md) | 4320–4525 | E004. `analysis/consolidated/staging/math/model_impl.cpp` |

## `05-dokazatelstva/03-decompiled-core`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E005-index.tsv.md](05-dokazatelstva/03-decompiled-core/E005-index.tsv.md) | 4526–4539 | E005. `analysis/decompiled_core/index.tsv` |

## `05-dokazatelstva/04-decompiled-features`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E006-index.tsv.md](05-dokazatelstva/04-decompiled-features/E006-index.tsv.md) | 4540–4560 | E006. `analysis/decompiled_features/index.tsv` |

## `05-dokazatelstva/05-decompiled-rng`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E007-index.tsv.md](05-dokazatelstva/05-decompiled-rng/E007-index.tsv.md) | 4561–4578 | E007. `analysis/decompiled_rng/index.tsv` |

## `05-dokazatelstva/06-ghidra-assembly`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E008-functions.tsv.md](05-dokazatelstva/06-ghidra-assembly/E008-functions.tsv.md) | 4579–4611 | E008. `analysis/ghidra_assembly/functions.tsv` |

## `05-dokazatelstva/07-imports`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E009-user_imports.csv.md](05-dokazatelstva/07-imports/E009-user_imports.csv.md) | 4612–5013 | E009. `analysis/imports/user_imports.csv` |
| [E010-user_imports_source.txt.md](05-dokazatelstva/07-imports/E010-user_imports_source.txt.md) | 5014–5416 | E010. `analysis/imports/user_imports_source.txt` |
| [E011-user_imports_summary.json.md](05-dokazatelstva/07-imports/E011-user_imports_summary.json.md) | 5417–6700 | E011. `analysis/imports/user_imports_summary.json` |

## `05-dokazatelstva/08-phase2-conditional-assembly`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E012-functions.tsv.md](05-dokazatelstva/08-phase2-conditional-assembly/E012-functions.tsv.md) | 6701–6754 | E012. `analysis/phase2/conditional_assembly/functions.tsv` |

## `05-dokazatelstva/09-phase2-conditional-bullet`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E013-index.tsv.md](05-dokazatelstva/09-phase2-conditional-bullet/E013-index.tsv.md) | 6755–6772 | E013. `analysis/phase2/conditional_bullet/index.tsv` |

## `05-dokazatelstva/10-phase2-conditional-core`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E014-index.tsv.md](05-dokazatelstva/10-phase2-conditional-core/E014-index.tsv.md) | 6773–6800 | E014. `analysis/phase2/conditional_core/index.tsv` |

## `05-dokazatelstva/11-phase2-conditional-damage`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E015-index.tsv.md](05-dokazatelstva/11-phase2-conditional-damage/E015-index.tsv.md) | 6801–6811 | E015. `analysis/phase2/conditional_damage/index.tsv` |

## `05-dokazatelstva/12-phase2-conditional-lag`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E016-index.tsv.md](05-dokazatelstva/12-phase2-conditional-lag/E016-index.tsv.md) | 6812–6827 | E016. `analysis/phase2/conditional_lag/index.tsv` |

## `05-dokazatelstva/13-phase2-conditional-targets`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E017-index.tsv.md](05-dokazatelstva/13-phase2-conditional-targets/E017-index.tsv.md) | 6828–6839 | E017. `analysis/phase2/conditional_targets/index.tsv` |

## `05-dokazatelstva/14-phase2-conditional-trace`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E018-index.tsv.md](05-dokazatelstva/14-phase2-conditional-trace/E018-index.tsv.md) | 6840–6858 | E018. `analysis/phase2/conditional_trace/index.tsv` |

## `05-dokazatelstva/15-phase2-lagcomp`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E019-analyze.py.md](05-dokazatelstva/15-phase2-lagcomp/E019-analyze.py.md) | 6859–7045 | E019. `analysis/phase2/lagcomp/analyze.py` |
| [E020-build_evidence.py.md](05-dokazatelstva/15-phase2-lagcomp/E020-build_evidence.py.md) | 7046–7168 | E020. `analysis/phase2/lagcomp/build_evidence.py` |
| [E021-callers.json.md](05-dokazatelstva/15-phase2-lagcomp/E021-callers.json.md) | 7169–7334 | E021. `analysis/phase2/lagcomp/callers.json` |
| [E022-evidence.json.md](05-dokazatelstva/15-phase2-lagcomp/E022-evidence.json.md) | 7335–9747 | E022. `analysis/phase2/lagcomp/evidence.json` |
| [E023-evidence.tsv.md](05-dokazatelstva/15-phase2-lagcomp/E023-evidence.tsv.md) | 9748–10039 | E023. `analysis/phase2/lagcomp/evidence.tsv` |
| [E024-evidence_check.log.md](05-dokazatelstva/15-phase2-lagcomp/E024-evidence_check.log.md) | 10040–10055 | E024. `analysis/phase2/lagcomp/evidence_check.log` |
| [E025-field_refs.json.md](05-dokazatelstva/15-phase2-lagcomp/E025-field_refs.json.md) | 10056–10837 | E025. `analysis/phase2/lagcomp/field_refs.json` |
| [E026-field_refs.tsv.md](05-dokazatelstva/15-phase2-lagcomp/E026-field_refs.tsv.md) | 10838–10918 | E026. `analysis/phase2/lagcomp/field_refs.tsv` |
| [E027-metadata.json.md](05-dokazatelstva/15-phase2-lagcomp/E027-metadata.json.md) | 10919–12374 | E027. `analysis/phase2/lagcomp/metadata.json` |
| [E028-metadata_strict_capstone.json.md](05-dokazatelstva/15-phase2-lagcomp/E028-metadata_strict_capstone.json.md) | 12375–12625 | E028. `analysis/phase2/lagcomp/metadata_strict_capstone.json` |
| [E029-raw_branch_refs.json.md](05-dokazatelstva/15-phase2-lagcomp/E029-raw_branch_refs.json.md) | 12626–12777 | E029. `analysis/phase2/lagcomp/raw_branch_refs.json` |
| [E030-raw_function_pointers.json.md](05-dokazatelstva/15-phase2-lagcomp/E030-raw_function_pointers.json.md) | 12778–12807 | E030. `analysis/phase2/lagcomp/raw_function_pointers.json` |
| [E031-raw_patterns.json.md](05-dokazatelstva/15-phase2-lagcomp/E031-raw_patterns.json.md) | 12808–13189 | E031. `analysis/phase2/lagcomp/raw_patterns.json` |
| [E032-scan.log.md](05-dokazatelstva/15-phase2-lagcomp/E032-scan.log.md) | 13190–13271 | E032. `analysis/phase2/lagcomp/scan.log` |

## `05-dokazatelstva/16-phase2-logs`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E033-phase2-bullet.log.md](05-dokazatelstva/16-phase2-logs/E033-phase2-bullet.log.md) | 13272–13335 | E033. `analysis/phase2/logs/phase2-bullet.log` |
| [E034-phase2-core.log.md](05-dokazatelstva/16-phase2-logs/E034-phase2-core.log.md) | 13336–13419 | E034. `analysis/phase2/logs/phase2-core.log` |
| [E035-phase2-damage.log.md](05-dokazatelstva/16-phase2-logs/E035-phase2-damage.log.md) | 13420–13477 | E035. `analysis/phase2/logs/phase2-damage.log` |
| [E036-phase2-lag.log.md](05-dokazatelstva/16-phase2-logs/E036-phase2-lag.log.md) | 13478–13540 | E036. `analysis/phase2/logs/phase2-lag.log` |
| [E037-phase2-targets.log.md](05-dokazatelstva/16-phase2-logs/E037-phase2-targets.log.md) | 13541–13599 | E037. `analysis/phase2/logs/phase2-targets.log` |
| [E038-phase2-trace.log.md](05-dokazatelstva/16-phase2-logs/E038-phase2-trace.log.md) | 13600–13697 | E038. `analysis/phase2/logs/phase2-trace.log` |

## `05-dokazatelstva/17-phase2-main`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E039-bullet_targets.tsv.md](05-dokazatelstva/17-phase2-main/E039-bullet_targets.tsv.md) | 13698–13714 | E039. `analysis/phase2/main/bullet_targets.tsv` |
| [E040-conditional_guards.tsv.md](05-dokazatelstva/17-phase2-main/E040-conditional_guards.tsv.md) | 13715–16813 | E040. `analysis/phase2/main/conditional_guards.tsv` |
| [E041-conditional_model_manifest.json.md](05-dokazatelstva/17-phase2-main/E041-conditional_model_manifest.json.md) | 16814–60077 | E041. `analysis/phase2/main/conditional_model_manifest.json` |
| [E042-core_targets.tsv.md](05-dokazatelstva/17-phase2-main/E042-core_targets.tsv.md) | 60078–60105 | E042. `analysis/phase2/main/core_targets.tsv` |
| [E043-damage_multiplier.tsv.md](05-dokazatelstva/17-phase2-main/E043-damage_multiplier.tsv.md) | 60106–60116 | E043. `analysis/phase2/main/damage_multiplier.tsv` |
| [E044-guard_scan.txt.md](05-dokazatelstva/17-phase2-main/E044-guard_scan.txt.md) | 60117–60214 | E044. `analysis/phase2/main/guard_scan.txt` |
| [E045-lag_targets.tsv.md](05-dokazatelstva/17-phase2-main/E045-lag_targets.tsv.md) | 60215–60230 | E045. `analysis/phase2/main/lag_targets.tsv` |
| [E046-math_constants.json.md](05-dokazatelstva/17-phase2-main/E046-math_constants.json.md) | 60231–60248 | E046. `analysis/phase2/main/math_constants.json` |
| [E047-math_kernels_original.tsv.md](05-dokazatelstva/17-phase2-main/E047-math_kernels_original.tsv.md) | 60249–60397 | E047. `analysis/phase2/main/math_kernels_original.tsv` |
| [E048-target_contexts.tsv.md](05-dokazatelstva/17-phase2-main/E048-target_contexts.tsv.md) | 60398–60409 | E048. `analysis/phase2/main/target_contexts.tsv` |
| [E049-trace_targets.tsv.md](05-dokazatelstva/17-phase2-main/E049-trace_targets.tsv.md) | 60410–60428 | E049. `analysis/phase2/main/trace_targets.tsv` |
| [E050-verification.json.md](05-dokazatelstva/17-phase2-main/E050-verification.json.md) | 60429–60720 | E050. `analysis/phase2/main/verification.json` |
| [E051-verification_run.txt.md](05-dokazatelstva/17-phase2-main/E051-verification_run.txt.md) | 60721–60751 | E051. `analysis/phase2/main/verification_run.txt` |

## `05-dokazatelstva/18-phase2-reconstructed`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E052-check_math.py.md](05-dokazatelstva/18-phase2-reconstructed/E052-check_math.py.md) | 60752–60818 | E052. `analysis/phase2/reconstructed/check_math.py` |
| [E053-math_check_results.json.md](05-dokazatelstva/18-phase2-reconstructed/E053-math_check_results.json.md) | 60819–60858 | E053. `analysis/phase2/reconstructed/math_check_results.json` |
| [E054-recovered_math.py.md](05-dokazatelstva/18-phase2-reconstructed/E054-recovered_math.py.md) | 60859–61001 | E054. `analysis/phase2/reconstructed/recovered_math.py` |

## `05-dokazatelstva/19-phase2-scripts`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E055-build_conditional_model.py.md](05-dokazatelstva/19-phase2-scripts/E055-build_conditional_model.py.md) | 61002–61042 | E055. `analysis/phase2/scripts/build_conditional_model.py` |
| [E056-find_guards.py.md](05-dokazatelstva/19-phase2-scripts/E056-find_guards.py.md) | 61043–61159 | E056. `analysis/phase2/scripts/find_guards.py` |
| [E057-package_phase2.py.md](05-dokazatelstva/19-phase2-scripts/E057-package_phase2.py.md) | 61160–61222 | E057. `analysis/phase2/scripts/package_phase2.py` |
| [E058-verify_phase2.py.md](05-dokazatelstva/19-phase2-scripts/E058-verify_phase2.py.md) | 61223–61320 | E058. `analysis/phase2/scripts/verify_phase2.py` |

## `05-dokazatelstva/20-phase2-spread`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E059-0030ac00.decode.json.md](05-dokazatelstva/20-phase2-spread/E059-0030ac00.decode.json.md) | 61321–61364 | E059. `analysis/phase2/spread/0030ac00.decode.json` |
| [E060-0030ac00.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E060-0030ac00.refs.tsv.md) | 61365–61513 | E060. `analysis/phase2/spread/0030ac00.refs.tsv` |
| [E061-0030d580.decode.json.md](05-dokazatelstva/20-phase2-spread/E061-0030d580.decode.json.md) | 61514–61531 | E061. `analysis/phase2/spread/0030d580.decode.json` |
| [E062-0030d580.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E062-0030d580.refs.tsv.md) | 61532–62460 | E062. `analysis/phase2/spread/0030d580.refs.tsv` |
| [E063-004beb50.cfg.calls.txt.md](05-dokazatelstva/20-phase2-spread/E063-004beb50.cfg.calls.txt.md) | 62461–62715 | E063. `analysis/phase2/spread/004beb50.cfg.calls.txt` |
| [E064-004beb50.cfg.edges.tsv.md](05-dokazatelstva/20-phase2-spread/E064-004beb50.cfg.edges.tsv.md) | 62716–63714 | E064. `analysis/phase2/spread/004beb50.cfg.edges.tsv` |
| [E065-004beb50.cfg.json.md](05-dokazatelstva/20-phase2-spread/E065-004beb50.cfg.json.md) | 63715–64221 | E065. `analysis/phase2/spread/004beb50.cfg.json` |
| [E066-004beb50.decode.json.md](05-dokazatelstva/20-phase2-spread/E066-004beb50.decode.json.md) | 64222–64575 | E066. `analysis/phase2/spread/004beb50.decode.json` |
| [E067-004beb50.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E067-004beb50.refs.tsv.md) | 64576–64587 | E067. `analysis/phase2/spread/004beb50.refs.tsv` |
| [E068-004feba0.cfg.calls.txt.md](05-dokazatelstva/20-phase2-spread/E068-004feba0.cfg.calls.txt.md) | 64588–64749 | E068. `analysis/phase2/spread/004feba0.cfg.calls.txt` |
| [E069-004feba0.cfg.edges.tsv.md](05-dokazatelstva/20-phase2-spread/E069-004feba0.cfg.edges.tsv.md) | 64750–65481 | E069. `analysis/phase2/spread/004feba0.cfg.edges.tsv` |
| [E070-004feba0.cfg.json.md](05-dokazatelstva/20-phase2-spread/E070-004feba0.cfg.json.md) | 65482–65618 | E070. `analysis/phase2/spread/004feba0.cfg.json` |
| [E071-004feba0.decode.json.md](05-dokazatelstva/20-phase2-spread/E071-004feba0.decode.json.md) | 65619–65712 | E071. `analysis/phase2/spread/004feba0.decode.json` |
| [E072-004feba0.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E072-004feba0.refs.tsv.md) | 65713–65824 | E072. `analysis/phase2/spread/004feba0.refs.tsv` |
| [E073-00512710.decode.json.md](05-dokazatelstva/20-phase2-spread/E073-00512710.decode.json.md) | 65825–65842 | E073. `analysis/phase2/spread/00512710.decode.json` |
| [E074-00512710.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E074-00512710.refs.tsv.md) | 65843–65943 | E074. `analysis/phase2/spread/00512710.refs.tsv` |
| [E075-00515ca0.cfg.calls.txt.md](05-dokazatelstva/20-phase2-spread/E075-00515ca0.cfg.calls.txt.md) | 65944–66030 | E075. `analysis/phase2/spread/00515ca0.cfg.calls.txt` |
| [E076-00515ca0.cfg.edges.tsv.md](05-dokazatelstva/20-phase2-spread/E076-00515ca0.cfg.edges.tsv.md) | 66031–66457 | E076. `analysis/phase2/spread/00515ca0.cfg.edges.tsv` |
| [E077-00515ca0.cfg.json.md](05-dokazatelstva/20-phase2-spread/E077-00515ca0.cfg.json.md) | 66458–66750 | E077. `analysis/phase2/spread/00515ca0.cfg.json` |
| [E078-00515ca0.decode.json.md](05-dokazatelstva/20-phase2-spread/E078-00515ca0.decode.json.md) | 66751–66934 | E078. `analysis/phase2/spread/00515ca0.decode.json` |
| [E079-00515ca0.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E079-00515ca0.refs.tsv.md) | 66935–66986 | E079. `analysis/phase2/spread/00515ca0.refs.tsv` |
| [E080-00516fe0.decode.json.md](05-dokazatelstva/20-phase2-spread/E080-00516fe0.decode.json.md) | 66987–67004 | E080. `analysis/phase2/spread/00516fe0.decode.json` |
| [E081-00516fe0.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E081-00516fe0.refs.tsv.md) | 67005–67015 | E081. `analysis/phase2/spread/00516fe0.refs.tsv` |
| [E082-0051c9c0.decode.json.md](05-dokazatelstva/20-phase2-spread/E082-0051c9c0.decode.json.md) | 67016–67033 | E082. `analysis/phase2/spread/0051c9c0.decode.json` |
| [E083-0051c9c0.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E083-0051c9c0.refs.tsv.md) | 67034–67075 | E083. `analysis/phase2/spread/0051c9c0.refs.tsv` |
| [E084-0051cf40.decode.json.md](05-dokazatelstva/20-phase2-spread/E084-0051cf40.decode.json.md) | 67076–67093 | E084. `analysis/phase2/spread/0051cf40.decode.json` |
| [E085-0051cf40.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E085-0051cf40.refs.tsv.md) | 67094–67143 | E085. `analysis/phase2/spread/0051cf40.refs.tsv` |
| [E086-0051d320.decode.json.md](05-dokazatelstva/20-phase2-spread/E086-0051d320.decode.json.md) | 67144–67161 | E086. `analysis/phase2/spread/0051d320.decode.json` |
| [E087-0051d320.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E087-0051d320.refs.tsv.md) | 67162–67208 | E087. `analysis/phase2/spread/0051d320.refs.tsv` |
| [E088-0051d910.decode.json.md](05-dokazatelstva/20-phase2-spread/E088-0051d910.decode.json.md) | 67209–67226 | E088. `analysis/phase2/spread/0051d910.decode.json` |
| [E089-0051d910.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E089-0051d910.refs.tsv.md) | 67227–67248 | E089. `analysis/phase2/spread/0051d910.refs.tsv` |
| [E090-0051dac0.decode.json.md](05-dokazatelstva/20-phase2-spread/E090-0051dac0.decode.json.md) | 67249–67266 | E090. `analysis/phase2/spread/0051dac0.decode.json` |
| [E091-0051dac0.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E091-0051dac0.refs.tsv.md) | 67267–67312 | E091. `analysis/phase2/spread/0051dac0.refs.tsv` |
| [E092-005239d0.cfg.calls.txt.md](05-dokazatelstva/20-phase2-spread/E092-005239d0.cfg.calls.txt.md) | 67313–67359 | E092. `analysis/phase2/spread/005239d0.cfg.calls.txt` |
| [E093-005239d0.cfg.edges.tsv.md](05-dokazatelstva/20-phase2-spread/E093-005239d0.cfg.edges.tsv.md) | 67360–67432 | E093. `analysis/phase2/spread/005239d0.cfg.edges.tsv` |
| [E094-005239d0.cfg.json.md](05-dokazatelstva/20-phase2-spread/E094-005239d0.cfg.json.md) | 67433–67474 | E094. `analysis/phase2/spread/005239d0.cfg.json` |
| [E095-00524a30.decode.json.md](05-dokazatelstva/20-phase2-spread/E095-00524a30.decode.json.md) | 67475–67492 | E095. `analysis/phase2/spread/00524a30.decode.json` |
| [E096-00524a30.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E096-00524a30.refs.tsv.md) | 67493–67677 | E096. `analysis/phase2/spread/00524a30.refs.tsv` |
| [E097-00525830.decode.json.md](05-dokazatelstva/20-phase2-spread/E097-00525830.decode.json.md) | 67678–67695 | E097. `analysis/phase2/spread/00525830.decode.json` |
| [E098-00525830.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E098-00525830.refs.tsv.md) | 67696–67724 | E098. `analysis/phase2/spread/00525830.refs.tsv` |
| [E099-00526290.decode.json.md](05-dokazatelstva/20-phase2-spread/E099-00526290.decode.json.md) | 67725–67742 | E099. `analysis/phase2/spread/00526290.decode.json` |
| [E100-00526290.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E100-00526290.refs.tsv.md) | 67743–68041 | E100. `analysis/phase2/spread/00526290.refs.tsv` |
| [E101-005a7480.decode.json.md](05-dokazatelstva/20-phase2-spread/E101-005a7480.decode.json.md) | 68042–68059 | E101. `analysis/phase2/spread/005a7480.decode.json` |
| [E102-005a7480.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E102-005a7480.refs.tsv.md) | 68060–68086 | E102. `analysis/phase2/spread/005a7480.refs.tsv` |
| [E103-00635a00.decode.json.md](05-dokazatelstva/20-phase2-spread/E103-00635a00.decode.json.md) | 68087–68104 | E103. `analysis/phase2/spread/00635a00.decode.json` |
| [E104-00635a00.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E104-00635a00.refs.tsv.md) | 68105–68149 | E104. `analysis/phase2/spread/00635a00.refs.tsv` |
| [E105-00790fe0.decode.json.md](05-dokazatelstva/20-phase2-spread/E105-00790fe0.decode.json.md) | 68150–68167 | E105. `analysis/phase2/spread/00790fe0.decode.json` |
| [E106-00790fe0.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E106-00790fe0.refs.tsv.md) | 68168–68178 | E106. `analysis/phase2/spread/00790fe0.refs.tsv` |
| [E107-00c6ff30.decode.json.md](05-dokazatelstva/20-phase2-spread/E107-00c6ff30.decode.json.md) | 68179–68196 | E107. `analysis/phase2/spread/00c6ff30.decode.json` |
| [E108-00c6ff30.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E108-00c6ff30.refs.tsv.md) | 68197–68227 | E108. `analysis/phase2/spread/00c6ff30.refs.tsv` |
| [E109-00c70088.decode.json.md](05-dokazatelstva/20-phase2-spread/E109-00c70088.decode.json.md) | 68228–68245 | E109. `analysis/phase2/spread/00c70088.decode.json` |
| [E110-00c70088.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E110-00c70088.refs.tsv.md) | 68246–68275 | E110. `analysis/phase2/spread/00c70088.refs.tsv` |
| [E111-00d8fc20.decode.json.md](05-dokazatelstva/20-phase2-spread/E111-00d8fc20.decode.json.md) | 68276–68293 | E111. `analysis/phase2/spread/00d8fc20.decode.json` |
| [E112-00d8fc20.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E112-00d8fc20.refs.tsv.md) | 68294–68311 | E112. `analysis/phase2/spread/00d8fc20.refs.tsv` |
| [E113-00d99b24.decode.json.md](05-dokazatelstva/20-phase2-spread/E113-00d99b24.decode.json.md) | 68312–68329 | E113. `analysis/phase2/spread/00d99b24.decode.json` |
| [E114-00d99b24.refs.tsv.md](05-dokazatelstva/20-phase2-spread/E114-00d99b24.refs.tsv.md) | 68330–68342 | E114. `analysis/phase2/spread/00d99b24.refs.tsv` |
| [E115-angle_switch_decode.log.md](05-dokazatelstva/20-phase2-spread/E115-angle_switch_decode.log.md) | 68343–68355 | E115. `analysis/phase2/spread/angle_switch_decode.log` |
| [E116-atan_check.log.md](05-dokazatelstva/20-phase2-spread/E116-atan_check.log.md) | 68356–68375 | E116. `analysis/phase2/spread/atan_check.log` |
| [E117-callback_decode.log.md](05-dokazatelstva/20-phase2-spread/E117-callback_decode.log.md) | 68376–68434 | E117. `analysis/phase2/spread/callback_decode.log` |
| [E118-cfg_spread.py.md](05-dokazatelstva/20-phase2-spread/E118-cfg_spread.py.md) | 68435–68558 | E118. `analysis/phase2/spread/cfg_spread.py` |
| [E119-constant_counts.json.md](05-dokazatelstva/20-phase2-spread/E119-constant_counts.json.md) | 68559–68654 | E119. `analysis/phase2/spread/constant_counts.json` |
| [E120-constant_hits.tsv.md](05-dokazatelstva/20-phase2-spread/E120-constant_hits.tsv.md) | 68655–72423 | E120. `analysis/phase2/spread/constant_hits.tsv` |
| [E121-constants.log.md](05-dokazatelstva/20-phase2-spread/E121-constants.log.md) | 72424–72519 | E121. `analysis/phase2/spread/constants.log` |
| [E122-consumer_decode.log.md](05-dokazatelstva/20-phase2-spread/E122-consumer_decode.log.md) | 72520–72628 | E122. `analysis/phase2/spread/consumer_decode.log` |
| [E123-evidence.tsv.md](05-dokazatelstva/20-phase2-spread/E123-evidence.tsv.md) | 72629–72689 | E123. `analysis/phase2/spread/evidence.tsv` |
| [E124-evidence_audit.json.md](05-dokazatelstva/20-phase2-spread/E124-evidence_audit.json.md) | 72690–72713 | E124. `analysis/phase2/spread/evidence_audit.json` |
| [E125-export_evidence.py.md](05-dokazatelstva/20-phase2-spread/E125-export_evidence.py.md) | 72714–72844 | E125. `analysis/phase2/spread/export_evidence.py` |
| [E126-fast_decode.log.md](05-dokazatelstva/20-phase2-spread/E126-fast_decode.log.md) | 72845–72933 | E126. `analysis/phase2/spread/fast_decode.log` |
| [E127-final_audit.log.md](05-dokazatelstva/20-phase2-spread/E127-final_audit.log.md) | 72934–72957 | E127. `analysis/phase2/spread/final_audit.log` |
| [E128-first_math.log.md](05-dokazatelstva/20-phase2-spread/E128-first_math.log.md) | 72958–74144 | E128. `analysis/phase2/spread/first_math.log` |
| [E129-formula_samples.tsv.md](05-dokazatelstva/20-phase2-spread/E129-formula_samples.tsv.md) | 74145–74283 | E129. `analysis/phase2/spread/formula_samples.tsv` |
| [E130-hc_kernel_decode.log.md](05-dokazatelstva/20-phase2-spread/E130-hc_kernel_decode.log.md) | 74284–74312 | E130. `analysis/phase2/spread/hc_kernel_decode.log` |
| [E131-inspect_spread.py.md](05-dokazatelstva/20-phase2-spread/E131-inspect_spread.py.md) | 74313–74498 | E131. `analysis/phase2/spread/inspect_spread.py` |
| [E132-math_constants.tsv.md](05-dokazatelstva/20-phase2-spread/E132-math_constants.tsv.md) | 74499–74520 | E132. `analysis/phase2/spread/math_constants.tsv` |
| [E133-native_rng_checks.log.md](05-dokazatelstva/20-phase2-spread/E133-native_rng_checks.log.md) | 74521–74591 | E133. `analysis/phase2/spread/native_rng_checks.log` |
| [E134-raw_target_calls.tsv.md](05-dokazatelstva/20-phase2-spread/E134-raw_target_calls.tsv.md) | 74592–74845 | E134. `analysis/phase2/spread/raw_target_calls.tsv` |
| [E135-ring_consumer.log.md](05-dokazatelstva/20-phase2-spread/E135-ring_consumer.log.md) | 74846–74857 | E135. `analysis/phase2/spread/ring_consumer.log` |
| [E136-sampling_helpers.log.md](05-dokazatelstva/20-phase2-spread/E136-sampling_helpers.log.md) | 74858–74932 | E136. `analysis/phase2/spread/sampling_helpers.log` |
| [E137-seed_window.log.md](05-dokazatelstva/20-phase2-spread/E137-seed_window.log.md) | 74933–74952 | E137. `analysis/phase2/spread/seed_window.log` |
| [E138-shortlist_decode.log.md](05-dokazatelstva/20-phase2-spread/E138-shortlist_decode.log.md) | 74953–76097 | E138. `analysis/phase2/spread/shortlist_decode.log` |
| [E139-switch_edges.tsv.md](05-dokazatelstva/20-phase2-spread/E139-switch_edges.tsv.md) | 76098–76118 | E139. `analysis/phase2/spread/switch_edges.tsv` |
| [E140-table_refs.py.md](05-dokazatelstva/20-phase2-spread/E140-table_refs.py.md) | 76119–76190 | E140. `analysis/phase2/spread/table_refs.py` |
| [E141-table_refs.tsv.md](05-dokazatelstva/20-phase2-spread/E141-table_refs.tsv.md) | 76191–76328 | E141. `analysis/phase2/spread/table_refs.tsv` |
| [E142-targets.tsv.md](05-dokazatelstva/20-phase2-spread/E142-targets.tsv.md) | 76329–76347 | E142. `analysis/phase2/spread/targets.tsv` |
| [E143-thunks.log.md](05-dokazatelstva/20-phase2-spread/E143-thunks.log.md) | 76348–76367 | E143. `analysis/phase2/spread/thunks.log` |

## `05-dokazatelstva/21-analysis-root`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E144-requirements.txt.md](05-dokazatelstva/21-analysis-root/E144-requirements.txt.md) | 76368–76380 | E144. `analysis/requirements.txt` |

## `05-dokazatelstva/22-results`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E145-anchor_xrefs.csv.md](05-dokazatelstva/22-results/E145-anchor_xrefs.csv.md) | 76381–76398 | E145. `analysis/results/anchor_xrefs.csv` |
| [E146-core_targets.tsv.md](05-dokazatelstva/22-results/E146-core_targets.tsv.md) | 76399–76412 | E146. `analysis/results/core_targets.tsv` |
| [E147-feature_targets.tsv.md](05-dokazatelstva/22-results/E147-feature_targets.tsv.md) | 76413–76432 | E147. `analysis/results/feature_targets.tsv` |
| [E148-freetype_evidence.txt.md](05-dokazatelstva/22-results/E148-freetype_evidence.txt.md) | 76433–76453 | E148. `analysis/results/freetype_evidence.txt` |
| [E149-import_validation.json.md](05-dokazatelstva/22-results/E149-import_validation.json.md) | 76454–79593 | E149. `analysis/results/import_validation.json` |
| [E150-index_summary.json.md](05-dokazatelstva/22-results/E150-index_summary.json.md) | 79594–79613 | E150. `analysis/results/index_summary.json` |
| [E151-rng_targets.tsv.md](05-dokazatelstva/22-results/E151-rng_targets.tsv.md) | 79614–79631 | E151. `analysis/results/rng_targets.tsv` |
| [E152-triage.json.md](05-dokazatelstva/22-results/E152-triage.json.md) | 79632–80167 | E152. `analysis/results/triage.json` |
| [E153-triage.txt.md](05-dokazatelstva/22-results/E153-triage.txt.md) | 80168–80957 | E153. `analysis/results/triage.txt` |
| [E154-verification.json.md](05-dokazatelstva/22-results/E154-verification.json.md) | 80958–80986 | E154. `analysis/results/verification.json` |

## `05-dokazatelstva/23-review-rng`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E155-callee_addresses.txt.md](05-dokazatelstva/23-review-rng/E155-callee_addresses.txt.md) | 80987–81004 | E155. `analysis/review_rng/callee_addresses.txt` |
| [E156-checks.json.md](05-dokazatelstva/23-review-rng/E156-checks.json.md) | 81005–81024 | E156. `analysis/review_rng/checks.json` |
| [E157-collect_evidence.py.md](05-dokazatelstva/23-review-rng/E157-collect_evidence.py.md) | 81025–81095 | E157. `analysis/review_rng/collect_evidence.py` |
| [E158-constants.json.md](05-dokazatelstva/23-review-rng/E158-constants.json.md) | 81096–83580 | E158. `analysis/review_rng/constants.json` |
| [E159-constants.tsv.md](05-dokazatelstva/23-review-rng/E159-constants.tsv.md) | 83581–83769 | E159. `analysis/review_rng/constants.tsv` |
| [E160-followup_refs.json.md](05-dokazatelstva/23-review-rng/E160-followup_refs.json.md) | 83770–85361 | E160. `analysis/review_rng/followup_refs.json` |
| [E161-helper_constants.json.md](05-dokazatelstva/23-review-rng/E161-helper_constants.json.md) | 85362–85429 | E161. `analysis/review_rng/helper_constants.json` |
| [E162-helper_ranges.json.md](05-dokazatelstva/23-review-rng/E162-helper_ranges.json.md) | 85430–85566 | E162. `analysis/review_rng/helper_ranges.json` |
| [E163-refs.json.md](05-dokazatelstva/23-review-rng/E163-refs.json.md) | 85567–94214 | E163. `analysis/review_rng/refs.json` |
| [E164-targeted_evidence.json.md](05-dokazatelstva/23-review-rng/E164-targeted_evidence.json.md) | 94215–94458 | E164. `analysis/review_rng/targeted_evidence.json` |

## `05-dokazatelstva/24-scripts`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E165-ApplyConditionalGuards.java.md](05-dokazatelstva/24-scripts/E165-ApplyConditionalGuards.java.md) | 94459–94501 | E165. `analysis/scripts/ApplyConditionalGuards.java` |
| [E166-ExportEvidence.java.md](05-dokazatelstva/24-scripts/E166-ExportEvidence.java.md) | 94502–94546 | E166. `analysis/scripts/ExportEvidence.java` |
| [E167-RecoverDump.java.md](05-dokazatelstva/24-scripts/E167-RecoverDump.java.md) | 94547–94652 | E167. `analysis/scripts/RecoverDump.java` |
| [E168-anchor_xrefs.py.md](05-dokazatelstva/24-scripts/E168-anchor_xrefs.py.md) | 94653–94679 | E168. `analysis/scripts/anchor_xrefs.py` |
| [E169-index_code.py.md](05-dokazatelstva/24-scripts/E169-index_code.py.md) | 94680–94753 | E169. `analysis/scripts/index_code.py` |
| [E170-package_results.py.md](05-dokazatelstva/24-scripts/E170-package_results.py.md) | 94754–94801 | E170. `analysis/scripts/package_results.py` |
| [E171-triage.py.md](05-dokazatelstva/24-scripts/E171-triage.py.md) | 94802–94863 | E171. `analysis/scripts/triage.py` |
| [E172-verify_results.py.md](05-dokazatelstva/24-scripts/E172-verify_results.py.md) | 94864–94946 | E172. `analysis/scripts/verify_results.py` |

## `05-dokazatelstva/25-strings`

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [E173-anchors.csv.md](05-dokazatelstva/25-strings/E173-anchors.csv.md) | 94947–95000 | E173. `analysis/strings/anchors.csv` |
| [E174-anchors.json.md](05-dokazatelstva/25-strings/E174-anchors.json.md) | 95001–95574 | E174. `analysis/strings/anchors.json` |
| [E175-compact_run.log.md](05-dokazatelstva/25-strings/E175-compact_run.log.md) | 95575–95585 | E175. `analysis/strings/compact_run.log` |
| [E176-coverage.json.md](05-dokazatelstva/25-strings/E176-coverage.json.md) | 95586–96065 | E176. `analysis/strings/coverage.json` |
| [E177-evidence.csv.md](05-dokazatelstva/25-strings/E177-evidence.csv.md) | 96066–96375 | E177. `analysis/strings/evidence.csv` |
| [E178-evidence.json.md](05-dokazatelstva/25-strings/E178-evidence.json.md) | 96376–100312 | E178. `analysis/strings/evidence.json` |
| [E179-extract_compact.py.md](05-dokazatelstva/25-strings/E179-extract_compact.py.md) | 100313–100487 | E179. `analysis/strings/extract_compact.py` |
| [E180-extract_strings.py.md](05-dokazatelstva/25-strings/E180-extract_strings.py.md) | 100488–101029 | E180. `analysis/strings/extract_strings.py` |
| [E181-rtti.csv.md](05-dokazatelstva/25-strings/E181-rtti.csv.md) | 101030–101260 | E181. `analysis/strings/rtti.csv` |
| [E182-run.log.md](05-dokazatelstva/25-strings/E182-run.log.md) | 101261–101299 | E182. `analysis/strings/run.log` |
| [E183-schema_fields.csv.md](05-dokazatelstva/25-strings/E183-schema_fields.csv.md) | 101300–101368 | E183. `analysis/strings/schema_fields.csv` |
| [E184-xor_probe.json.md](05-dokazatelstva/25-strings/E184-xor_probe.json.md) | 101369–101433 | E184. `analysis/strings/xor_probe.json` |

## корень разбивки

| Файл | Строки исходника | Заголовок |
|---|---:|---|
| [07-prilozhenie-f.md](07-prilozhenie-f.md) | 101434–101453 | Приложение F. Крупные индексы и бинарные материалы вне текстового объединения |

