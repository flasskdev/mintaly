<!-- split-part | CS2_RESEARCH_MASTER.md lines 2415-2543 | body-sha256 ae471ba11d31176d41e9c359b881b81d4182ac1029801fa7ae32fe77f1a3cc5b -->
[← все части](../README.md) · [индекс отчётов](00-index.md)

<!-- split-body-start -->

<a id="doc-08"></a>

# Приложение D08. `analysis/phase2/REPORT_RU.md`

**Дословный исходный отчёт.** Даты/статусы/неизвестные роли внутри относятся к своему этапу. При расхождении использовать актуальный свод и поздние доказательства. Пути внутри текста — исторические: соответствующие материалы встроены в MD/CPP и находятся через поиск исходного пути.

# CS2 dump — этап 2: hitchance, damage/penetration, история и режимы

## Результат

Восстановлены **детерминированное 64-sample ядро hitchance**, его **ускоренный 8×8 вариант**, генераторы sample tables, часть выбора/оптимизации aim points, подготовка minimum-damage/health thresholds, вызывающий контур bullet damage/penetration, producer/aging пространственно-временной истории и отдельная melee/knifebot-подобная ветка.

**Это ещё не «FULL всех функций» и не исходники, готовые к сборке.** Resolver не идентифицирован как завершённый алгоритм. Ключевое тело обработки penetration segment находится по внешнему VA вне этого dump. Динамические offsets и таблицы в pre-entry состоянии ещё не инициализированы. Эти пробелы не заполнены типовыми алгоритмами из других читов.

## Покрытие запроса

| Область | Что подтверждено | Что не закрыто |
|---|---|---|
| Hitchance | 64 deterministic directions, 4 агрегата, callback, thresholds, zero-scale shortcut | Точное engine spread RNG, все weapon-specific поправки, динамическая проверка |
| Fast hitchance | 8 блоков по 8, reverse traversal, ранняя экстраполяция, отличие `>` от `≥` | Полная эквивалентность fast geometry engine trace не заявляется |
| Penetration/autowall | Локальный hit-volume test, range attenuation, trace orchestration, segment loop, damage gates | Тело внешней функции `VA 0x7FFCEF4A8DB0`, world/material state, все engine rules |
| Lag compensation | Ring16, capture producer, tick admission, aging, continuity reset, callers | Полное применение/откат bones/state, network correction, все режимы server rewind |
| Resolver | Разобраны отдельные angle/history consumers; сохранены C/ASM | Завершённая resolver state machine и доказанная привязка этих consumers к resolver |
| Прочие режимы | Melee damage/mode selection, aim-point optimization, candidate comparison, minimum-damage rules | Исчерпывающий список GUI modes и все связи конфигов |

## 1. Hitchance: теперь это реальный kernel, а не совпадение RandomFloat

Цепочка:

```text
0x512710  →  deterministic tables
0x5239D0  →  scaled source cache
0x525830  →  64-job dispatch
0x51CF40  →  basis/frame
0x51D320  →  direction + scalar score evaluator 0x51DAC0
0x524A30  →  final aggregates copied to candidate+0x44…+0x50
```

Таблица 1: 8 радиальных уровней × 8 углов. `radius=ring/8`, `angle=ring·π/8 + sector·π/4`. Таблица 2 для point optimization: `radius=sqrt((index+1)/64)`, `angle=index·2.399963140487671`. Это не подтверждённая реализация native weapon RNG/ran1 и не случайный Monte Carlo.

Для полного агрегатора `0x525830`:

- `+0x44`: доля валидных samples, достигших minimum score.
- `+0x48`: доля scores **≥** health-like budget.
- `+0x4C`: доля специальной категории `(category & 0xFE)==2`, если разрешена.
- `+0x50`: средний score по 64 slots.

Health-like budget берётся из integer entity field и уменьшается на weight×damage для записей того же target, если weight>0.75. Minimum threshold берётся из context+0x88; setting<=100 ограничивается health, setting>100 заменяется health либо ceil(health/2) по отдельному mode; cap=130. Источник offsets пока placeholder, поэтому точные SDK/GUI названия не выдаются за восстановленные символы.

`0x526290` обрабатывает **8–64** samples, но делит всегда на 64. После однородного успешного блока может заполнить оставшиеся counts/mean экстраполяцией. В нём thresholdB сравнивается **строго `>`**, в полном агрегаторе — **`≥`**. Поэтому эти два пути нельзя заменять одним циклом с одинаковым сравнением.

Подробности и первичные адреса: `spread/summary.md`, `spread/evidence.tsv`, `conditional_trace/212c381d320.c`, `conditional_trace/212c3826290.c`, `conditional_bullet/212c3825830.c`.

## 2. Autowall: восстановлен свой контур, установлен отсутствующий dependency

`0x51DAC0` нормализует направление, проверяет prepared hit volumes через `0x2F19C0`, применяет category mask и дистанционную оценку, затем вызывает `0x2F2110 → 0x2F0DD0`.

Математический смысл attenuation на обычных входах:

```text
range_parameter ** (distance / 500)
```

Он реализован через log2f/exp2f-подобные библиотечные kernels, а не автоматически через стандартный pow с обещанием bit-exact результата.

`0x2F0DD0` проходит список stride **0x18**, вызывает внешний обработчик для каждого segment, накапливает потери, останавливается по return flag/minimum damage и возвращает подходящий object/segment. **Сам обработчик `VA 0x7FFCEF4A8DB0` отсутствует в приложенном образе.** Отсутствуют также внешние trace implementations и состояние материалов/мира. Список пользовательских imports не добавляет отсутствующие тела функций.

Полная схема, псевдокод цикла, внешние адреса, damage/category helper: `main/PENETRATION_AND_MELEE_RU.md`.

## 3. История: найден producer, а не только reader

**`0x4721D0`** пишет историю, которую читает `0x52B250`:

- 16 slots, stride `0x500`, начало object+0xF0.
- count object+0x50F0, head object+0x50F4.
- admission сравнивает newest tick с `int(source_time*64+0.5)`.
- новый head — `(head+15)%16`, count растёт до 16; новый slot становится первым.
- reset count=1 при `distanceSquared > 4096*clamp(deltaTick,1,5)` либо изменении размера payload collection.
- В хвостовом aging при **all-stale** конкретный блок **не записывает count=0**. Это сохранено в реконструкции, а не исправлено «по здравому смыслу».

Контейнер используется несколькими callers, включая orchestration `0x6AC690` и дополнительный producer caller `0x661010`. Нельзя объявлять весь контейнер knife-only только по назначению reader `0x52B250`; так же нельзя называть один reader универсальным bullet-backtrack selector.

Отчёт, точные stores, layout и оговорки: `lagcomp/summary.md`, `lagcomp/evidence.tsv`; свежие C-листинги в `conditional_lag/`.

## 4. Melee и другие режимы

`0x528B90` идентифицирован как melee/knifebot-подобная ветка, **не bullet autowall**. Подтверждённые значения: primary scores 25/40/90, secondary 65/180, multipliers **1/0.825**, dot threshold **0.475**, trace distances **66/50**. Есть mode selection по ожидаемой летальности и config, вертикальные probes, ray/hull calls и append candidate stride0x30 через `0x52AD40`.

Bullet candidates имеют stride0x58 и другой layout. Их comparator `0x51BEA0`, selector/optimizer `0x524A30`, sampling-based point optimizer `0x51C9C0` разобраны отдельно.

`0x51B480` использует углы из истории для вычисления нормализованной разницы и вычитает её из this+0x1D0. `0x515CA0` содержит, среди прочего, RandomFloat-поправку угла. **Ни один такой факт сам по себе не доказывает полноценный resolver.** Их C сохранён, но отсутствующее назначение не придумано.

## 5. Что именно приложено

- `reconstructed/recovered_math.py`: читаемая независимая модель sample tables, full/fast aggregates, threshold rules, attenuation и отдельных lag-history условий.
- `reconstructed/check_math.py`, `math_check_results.json`: **23 успешные проверки** обычных и граничных математических случаев. Это тестирование написанной модели, не исполнение дампа.
- `conditional_*/*.c`: **42 успешно полученных C-листинга** выбранных функций. `0x4BEB50` остался decompiler timeout; ASM присутствует. Успешная декомпиляция не означает восстановленные типы и полную достоверность каждой строки.
- `conditional_assembly/*.asm`: ASM выбранных функций; byte verification против модели.
- `spread/`, `lagcomp/`: отдельные доказательства, original-byte checks, scripts, формулы.
- `main/conditional_model_manifest.json`: полный журнал условных branch-преобразований.
- Отдельный Ghidra-проект `CONDITIONAL_PATH_MODEL`: воспроизводимый навигационный анализ, не исполняемый продукт.

## 6. Метод и проверка

Original BIN: 83 890 176 bytes, base `0x212C3300000`, SHA-256:

`3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27`

Исходный файл **не изменён и не запускался**. Ghidra 12.1.4 + Capstone (wheel 5.0.9, binding `__version__` 5.0.7) использованы только для статического анализа.

Из-за environment predicates и ложных failure branches часть декомпиляции была загрязнена. В отдельной модели **3089 equality branches** заменены на переход к их success target. Допущения: вычисленные environment predicates равны `0x5877` или `0x92FB254D`. Runtime inputs отсутствуют, поэтому это **условная модель**, а не доказанное устранение невозможных ветвей.

SHA-256 модели:

`bd4b358c86a23027dc72de102790018803d1dbe9aabfcbe789196f826d6f1f39`

Проверка `scripts/verify_phase2.py` удостоверяет original hash, точные before/after bytes, совпадение branch targets и то, что модель отличается от оригинала только перечисленными преобразованиями. ASM сверяется с **моделью**, а не ложно объявляется полностью оригинальным. Подробные счётчики — `main/verification.json`.

Original-byte sidecar evidence независимо показывает, что таблицы по RVA `0x1754680` и `0x17548C0` имеют по **512 нулевых bytes**. Несколько динамических offsets содержат **0x13371337**. Это pre-entry состояние, не готовые runtime tables/SDK offsets.

## 7. Что требуется для действительно полного восстановления

Нужны соответствующие этой сборке внешние модули с кодом вызываемых trace/penetration функций, карта модулей с base/version и post-init state из разрешённой среды. Для resolver и полного перечня режимов дополнительно нужна дальнейшая привязка config callbacks, state writers, hit-volume transformations и dispatch paths. PDB/символы/исходники, если доступны, помогут; их наличие не предполагалось.

Из этого одного pre-entry BIN нельзя честно предъявить отсутствующие внешние функции, рабочие runtime offsets и проверенную реализацию всех режимов. Текущий результат — существенно расширенная, проверяемая реконструкция с точными границами, а не обещание «фулл» поверх непроверенных заглушек.


---
