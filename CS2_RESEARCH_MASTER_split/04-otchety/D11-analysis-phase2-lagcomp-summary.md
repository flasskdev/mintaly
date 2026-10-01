<!-- split-part | CS2_RESEARCH_MASTER.md lines 2829-3066 | body-sha256 bd86cfe72f8cf2c64cc510b74262c3fa26b404401f822f6f590eaf936cccd2a1 -->
[← все части](../README.md) · [индекс отчётов](00-index.md)

<!-- split-body-start -->

<a id="doc-11"></a>

# Приложение D11. `analysis/phase2/lagcomp/summary.md`

**Дословный исходный отчёт.** Даты/статусы/неизвестные роли внутри относятся к своему этапу. При расхождении использовать актуальный свод и поздние доказательства. Пути внутри текста — исторические: соответствующие материалы встроены в MD/CPP и находятся через поиск исходного пути.

# Lag-ring: producers, aging, validation — частичная статическая реконструкция

## Результат и граница вывода

**Главный producer — RVA `0x4721D0`.** Он пишет тот же layout, который читает `0x52B250`: ring16, stride `0x500`, slots `object+0xF0`, count `+0x50F0`, head `+0x50F4`. Восстановлены вставка/перезапись, два вида отсечения истории, проверка разрыва и несколько точных callers. **Это не завершённая lag compensation.**

Разделение назначения:

- **Подтверждено байтами:** пространственно-временная история и операции над ней; у producer нет доказанной привязки исключительно к одному оружию.
- **Результат основной ветки статического анализа:** `0x528B90` — knife damage path; поэтому selector `0x52B250` и обслуживающий его ring могут относиться к melee-specific пути. Принадлежность всего контейнера только melee остаётся открытой. Conditional C-модель основной ветки эта работа не импортировала и не верифицировала.
- Не называем `0x52B250` универсальным hitscan/backtrack selector и не выводим knife-only назначение самого контейнера. В частности, позиционная и угловая история сама по себе этого не доказывает.

## Входы, безопасность и метод

- Base `0x212C3300000`; все адреса ниже — RVA, если не написано VA.
- Raw: `analysis/input/cs2_212C3300000.bin`, 83 890 176 байт.
- SHA-256: `3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27`; сверяется до анализа и при сборке evidence.
- Прочитаны `analysis/CORE_ADDENDUM_RU.md` и `analysis/decompiled_core/212c382b250.c`; каталог C находится под `analysis/`, не в корне.
- Sample **не исполнялся, не загружался как executable, не эмулировался и не патчился**. SQLite открыт `mode=ro`; оба Ghidra-проекта не использовались. Externals не исполнялись и не разрешались через live process; обход лицензии не выполнялся. Записи только в `analysis/phase2/lagcomp/`.
- Использован существующий Capstone из `analysis/venv`: metadata wheel **5.0.9**, binding `__version__` **5.0.7**. В SQLite 45 064 ranges, только 43 604 имеют `decoded_end==end`. Это полнота линейного декодирования диапазона, а не доказательство корректного CFG.
- Поиск: raw little-endian displacement patterns `0x50F0/0x50F4`; затем Capstone operand-проверка и обход прямых ветвей от function entry. Дополнительно raw `E8/E9 rel32` к выбранным функциям и указатели VA в данных; callers проверены по границам инструкций.
- Для продвижения декодера только **недекодируемые точные** `0F 1A 24 10`, `0F 1B 24 10`, `0F 1C 24 10` интерпретированы как 4-byte NOP. Это продвижение декодера, не изменение raw. Все 220 встреченных применений в текущем поиске перечислены в `metadata.json`; число включает функции вне shortlist. Обычные `.asm` не печатают эти synthetic NOP-строки: соответствующие gaps раскрыты в metadata.
- Обе стороны условных ветвей сохраняются. PEB/junk branches не объявляются исполненными или невозможными; indirect jumps остаются нерешёнными. Отдельная условная модель основной ветки анализа предполагает `PEBhash==0x5877` и `KUSERsum==0x92FB254D`; **это не доказанные runtime values и не условия, принудительно применённые этим скриптом**.
- Псевдокод ниже описывает полезные прямые блоки при достижении их соответствующих guard-success путей. Декодируемость не равна реальной достижимости. Отсутствие xref не доказывает отсутствия caller/writer, особенно при aliasing, bulk copy и indirect dispatch.

## Shortlist точных функций

Границы — из SQLite, полуинтервалы `[begin,end)`. Роли аналитические, не восстановленные исходные имена.

| Function RVA | End RVA | Роль | Минимальное байтовое свидетельство |
|---|---|---|---|
| **`0x4721D0`** | `0x47345C` | Основной producer, aging, discontinuity | `0x472951: 89 07` tick; `0x472953: C5 FA 11 47 04` fraction; `0x4728E8: 89 86 F0 50 00 00` count |
| **`0x473490`** | `0x473987` | Источник scalar aging-окна | `0x473957: 48 8B 40 08`; `0x47395B: C5 FA 10 40 58` |
| **`0x6AC690`** | `0x6B3406` | Orchestration, создание/перенос истории, dispatch | `0x6AF95D: E8 6E 28 DC FF` → producer; `0x6AE64F: 49 89 84 24 F0 50 00 00` перенос count/head |
| **`0x661010`** | `0x661BC0` | Дополнительный caller producer | `0x661BA8: E8 23 06 E1 FF` → `0x4721D0` |
| **`0x4707F0`** | `0x4711A1` | Инициализация объекта/слотов | `0x471003: 48 C7 86 F0 50 00 00 00 00 00 00` обнуляет сразу count/head |
| **`0x475830`** | `0x475C59` | Копирование record и owned buffers | `0x475853: C5 FC 10 02`; `0x475875: C5 FC 11 01` — первая 32-byte часть; далее другие части/буферы |
| **`0x471D70`** | `0x471DA0` | Reset истории и части состояния | `0x471D7D: 48 C7 86 F0 50 00 00 00 00 00 00`; перед ним call `0x471B90` |
| **`0x474D20`** | `0x4751A7` | Производная scratch-record, **не вставка в ring** | `0x474F6C: E8 BF 08 00 00` → copy helper; `0x4751A1: C6 40 0C 01` marker |

Для следующей C-очереди также полезен **`0x473F30`**, range `[0x473F30,0x474B27)`: из producer вызывается на `0x47316F` (`E8 BC 0D 00 00`) с `(slot, object)`. Его payload-семантика здесь не реконструирована. Уже известные `0x52DD20/0x52F030/0x52E720` повторно не запрашиваются.

## Структура

| Основа | Offset | Установлено |
|---|---|---|
| object | `+0x10` | entity/source pointer, читаемый producer |
| object | `+0x28` | byte mode; на найденном controller path устанавливается 0/1 |
| object | `+0x4C` | альтернативный источник времени при mode==1 |
| object | `+0xF0+i*0x500` | 16 slots |
| object | `+0x50F0/+0x50F4` | int32 count/head; head указывает **новейшую** запись |
| slot | `+0/+4` | int32 tick / float fraction |
| slot | `+8` | auxiliary integer, обычно -1; отдельный режим заполняет runtime sequence-like значением |
| slot | `+0x0C` | byte: обычный producer очищает вместе с +0x0D, scratch producer пишет 1; имя флага неизвестно |
| slot | `+0x14/+0x18/+0x1C` | XYZ position |
| slot | `+0x44/+0x48/+0x4C` | три float из helper `0x153120`; consumer использует первые два как углы |
| slot | `+0x80/+0x88/+0x90` | begin/end/capacity-like указатели owned buffer |
| slot | `+0xA0` | size-like int массива **элементов по 0x20**, а не entity/model ID |
| slot | `+0xB0/+0xB8` | pointer / capacity-like величина этого массива |
| slot | `+0x4E0/+0x4E8/+0x4F0` | ещё один owned buffer; полная семантика payload не установлена |

Count/head выводятся совместным qword-zero/copy и отдельными dword операциями. Размер объекта `0x53C0` подтверждён initializer `0x470830` и controller stride `0x6AEBB9`.

### Заполнение slot

В `0x4721D0`, при достижении insertion path:

1. Entity `object+0x10`, исходный float времени по динамическому offset из RVA `0x17701F0`.
2. Умножение на **64.0**: константа RVA `0xE58838`, bytes `00 00 80 42`.
3. `V_modff`, call `0x472911` через IAT RVA `0xF926E8`; имя сверено с `analysis/imports/user_imports_source.txt`. Отрицательная fraction нормализуется добавлением 1 и заёмом integer; есть float-rounding edge branch, обнуляющий fraction.
4. Tick write `0x472951`; fraction write `0x472953`.
5. XYZ пишет `0x47296E` / `0x472975`; исходный component pointer и position offset берутся из RVA `0x177035C` / `0x176F8C8`.
6. Angles: call `0x4729C8` → `0x153120`, copy на `0x4729D0` / `0x4729D7`.
7. `0x472A37: 66 C7 47 0C 00 00` очищает status bytes; `0x472B20` задаёт slot+8=-1. При mode==1 guarded block `0x473013–0x47301E` заменяет slot+8 значением `[[opaque_root]+0x38]+8`.

**Ограничение schema:** все три упомянутые dynamic-offset globals в raw содержат `37 13 37 13` (`0x13371337`). Поэтому названия вроде `m_flSimulationTime`, `m_vecAbsOrigin` и точные engine offsets не считаются восстановленными. Положение полей назначения подтверждено независимо от этих заглушек.

## Aging, admission и продвижение кольца

Следующие формулы предполагают обычные инварианты `0<=count<=16`, `0<=head<16`, finite float и boolean mode. В ASM остаток реализован как **signed remainder**, поэтому для повреждённого отрицательного head нельзя безоговорочно заменять его unsigned mask.

### 1. Отсечение старого хвоста

`0x4721F5–0x472235`:

```text
base_time = mode==1 ? object.float_at_4C : float_at(helper_2B7280()+0xC0)
window = helper_473490()
cutoff_tick = cvttss2si((base_time - window) * 64.0f + 0.5f)
```

`helper_2B7280()+0xC0` — сокращение полезного ненулевого пути; ASM содержит `test/cmove`, а затем добавляет `0x30`. Это не восстановленный безопасный NULL guard. `0.5f` находится по RVA `0xDEC7D8`, bytes `00 00 00 3F`. `helper_473490` на полезном guarded пути возвращает float `[[opaque_root]+8]+0x58`; имя настройки и её runtime value неизвестны.

Точное управление счётчиком на `0x472240–0x47229F`:

```text
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
```

**Существенный edge case:** когда все просмотренные записи старые, `JLE` на `0x472253` уходит на `0x47229F`, минуя store `0x472297`. Этот блок **не пишет count=0**. Не подменять его «нормальным prune-all» по ожиданию от типовой lagcomp. Фактический дальнейший результат зависит от следующих ветвей producer.

### 2. Отдельное отсечение по slot+8

Mode!=0 при непустой истории проходит environment-guarded блок к `0x4727A9`. Пока новейший slot+8 **>=** runtime auxiliary integer (`CMP 0x4727FF`, `JL 0x472803` — условие выхода):

```text
count -= 1
head = (head + 1) % 16
```

Stores: `0x47280B`, `0x47282C`. Здесь, в отличие от хвостового aging, count действительно может стать 0. Название auxiliary integer — command number, tick counter и т.п. — не доказано. Controller на `0x6AEBCF–0x6AEBF0` устанавливает mode в зависимости от равенства entity pointer с отдельным comparison pointer; автоматически называть его local-player flag нельзя.

### 3. Admission нового времени

Для непустой истории `0x472862–0x472887` считает `candidate_tick=cvttss2si(source_time*64+0.5)` и возвращает без вставки, если `newest.tick >= candidate_tick` (**signed JGE**, bytes `0F 8D F2 0A 00 00`). Fraction здесь не сравнивается. Это не строгая дедупликация полной tick/fraction пары: admission округляет с +0.5, а хранимая пара получается через modff.

### 4. Вставка и порядок

`0x472890–0x4728F9`:

```text
head = (head + 15) % 16
count = min(count + 1, 16)
new_record = object + 0xF0 + head*0x500
```

Таково сокращение для нормального count; ASM при count>15 просто не инкрементирует его, а не исправляет повреждённое значение. Пустой путь устанавливает 1. Новый слот — head; предыдущий по времени — `(head+1)%16`; самый старый — `(head+count-1)%16`. При полном кольце физически переиспользуется бывший старейший slot, owned buffers могут сохранять allocation.

## Проверка разрыва

После capture, `0x4732A1–0x473375`, при count>=2 сравниваются новый и предыдущий slots:

```text
delta_tick = newest.tick - previous.tick
factor = clamp(delta_tick, 1, 5)
limit_squared = float(factor << 12)       # 4096 * factor
if squared_distance_XYZ(newest, previous) > limit_squared:
    count = 1
else if newest.collection_size_at_A0 != previous.collection_size_at_A0:
    count = 1
```

Байты: `0x473332: 83 FA 02`, `0x47333F: 41 83 F8 05`, `0x47334C: C1 E2 0C`, `0x47335F: 77 14`, `0x473375: C7 86 F0 50 00 00 01 00 00 00`.

Это **линейный по clamped tick delta порог squared distance**, а не `(64*delta_tick)^2`. Для factor=1 радиус 64; для factor=5 — `sqrt(20480)`, не 320. Equal distance не сбрасывает историю; float NaN/exceptional paths не заменяются математической формулой без оговорки. Store count=1 оставляет текущий head и новейшую запись; старые bytes не затираются.

Slot+A0 интерпретирован как размер массива по `0x475A95–0x475B6B`: source count сравнивается с destination capacity, масштабируется `<<5`, управляет циклом копирования элементов по `0x20` и записывается обратно. Называть этот check сменой entity ID/model ID/bone count без дальнейшего payload-анализа нельзя.

## Callers, reset и перенос

| Caller function | Call site → target | Bytes | В исходном SQLite refs? |
|---|---|---|---|
| `0x475EF0` | `0x475F21` → `0x4721D0` | `E8 AA C2 FF FF` | Да |
| `0x661010` | `0x661BA8` → `0x4721D0` | `E8 23 06 E1 FF` | **Нет** |
| `0x6AC690` | `0x6AF95D` → `0x4721D0` | `E8 6E 28 DC FF` | **Нет** |
| `0x6AC690` | `0x6AE325` → `0x4707F0` | `E8 C6 24 DC FF` | **Нет** |
| `0x6AC690` | `0x6AE795` → `0x4707F0` | `E8 56 20 DC FF` | **Нет** |

Все пять rel32 совпадают с raw и найденными instruction boundaries. Это прямой пример неполноты индекса, не предположение об отсутствии других callers.

- Worker `0x475EF0` использует atomic `lock xadd [work+0x40]` и вызывает producer для каждого выбранного pointer. В данных RVA `0xEDFA70` содержит VA `0x212C3775EF0`; controller создаёт worker с table-base `0xEDFA68` (`LEA 0x6AF8E6`). При одном элементе — direct call; при нескольких — indirect scheduler dispatch. Полная синхронизация не восстановлена.
- Reset `0x471D70`: после `0x471B90` qword count/head=0. Function pointer имеется по RVA `0xEDFB18`; это свидетельство table entry, **не идентифицированный virtual caller**. Похожий inline reset — `0x6CE2F4/0x6CE2F9` внутри `0x6CE200`.
- `0x471B90` сам по себе очищает scratch/caches; его вызов **нельзя** автоматически считать сбросом ring. В найденном reset обнуление ring — следующая отдельная инструкция.
- Controller `0x6AE520–0x6AE64F` переносит 16 record-prefix и owned buffers, зануляя source buffer pointers, затем переносит qword count/head. Это move существующей истории, а не producer нового timestamp.
- `0x475830` копирует record-prefix и отдельно handles dynamic arrays; не считать slot простой POD-структурой для blind memcpy.

## `0x474D20`: отдельная производная запись

Эта функция нужна для разграничения producers:

1. Требует непустой ring (`0x474D32/0x474D39`).
2. Берёт newest slot; нормализует target pair из второго аргумента `+0x1F4/+0x200`; для обычных finite значений продолжает только если target time **строго позже** newest.
3. Требует scratch counter `object+0xC0 <= 31` (`0x474E33/0x474E36`); использует vector с begin `object+0xA8`, stride `0x500`.
4. Копирует newest через `0x475830`, сдвигает пространственный payload, пишет target position `0x475105/0x47510B`, target tick/fraction `0x47519A/0x47519C`, marker +0xC=1 `0x4751A1`.

**Ring count/head здесь не обновляются.** Это scratch/derived record, а не новый historical sample. Callers из SQLite: `0x52DD20` на `0x52DFB5`, `0x52F030` на `0x52F3CA`; их weapon-path назначение зависит от отдельной conditional batch пользователя.

## Consumer validation — сохранить отдельно от producers

ASM `0x52B250` подтверждает уже описанные в addendum guards, но не имя gameplay subsystem:

- `squared_distance <= 62500`, compare `0x52B4B1`.
- Третий аргумент по `+0xC` содержит **int32 tick**, см. `0x52B4DE: 44 8B 6E 0C`; не переносить misleading float-casts из C.
- Для нормализованных конечных пар: record time корректируется парой singleton `+8/+0xC`; центр проверки — `(input_tick,0) - singleton(+0x10,+0x14)`.
- `cvttss2si(abs(delta_seconds)*1000) <= 200`: `0x52B54F–0x52B559`, а не точное `abs(delta)<=0.2`.
- Inclusive bounds для скорректированного record: нижняя граница `(input_tick,0)-normalize(64*singleton.float_at_4)` и верхняя `(input_tick,0)`; branches `0x52B618`, `0x52B652/0x52B664`.
- Split по `0.475` и выбор до двух пространственно ближайших records — **политика selector**, не общая валидность контейнера. При knife-гипотезе эти thresholds нельзя автоматически переносить на hitscan/другие режимы.

## Evidence и воспроизведение

Основные deliverables:

- `summary.md` — этот отчёт.
- `evidence.json`, `evidence.tsv` — **26 claims, 281 instruction rows**, raw bytes/RVA/VA/function, 11 literal entries, raw caller и function-pointer evidence. Некоторые sites повторены между claims намеренно.
- `field_refs.json/.tsv` — 70 совпадений memory operands `+0x50F0/+0x50F4`; **не все принадлежат этому классу**. Совпадение displacement без dataflow не означает lag-ring.
- `raw_patterns.json`, `raw_branch_refs.json`, `raw_function_pointers.json`, `callers.json` — raw/index provenance без заявлений об исчерпывающем покрытии.
- `metadata.json`, `metadata_strict_capstone.json` — текущие границы/assumptions и ранний baseline без NOP-advance; отсутствие remaining invalid-instruction sites не означает полного CFG.
- `instructions.json`, 14 выбранных `.asm` — supporting disassembly.
- `analyze.py`, `build_evidence.py` — два небольших static-only скрипта; никакого запуска образа или Ghidra.

Из `/home/daytona/albigg`:

```sh
PYTHONDONTWRITEBYTECODE=1 analysis/venv/bin/python analysis/phase2/lagcomp/analyze.py > analysis/phase2/lagcomp/scan.log
PYTHONDONTWRITEBYTECODE=1 analysis/venv/bin/python analysis/phase2/lagcomp/build_evidence.py > analysis/phase2/lagcomp/evidence_check.log
```

Второй скрипт проверяет membership sites в instruction boundaries, owner function, каждый сохранённый byte span по исходному raw и каждый direct-call rel32. Проверка байтов **не является** runtime test или доказательством branch feasibility. Raw hash после проверки тот же.

## Что остаётся открытым

- Условия фактической достижимости environment-guarded paths, indirect calls/jumps и связность всего protected CFG.
- Имена engine schema fields/cvars; dynamic offsets в dump — заглушки.
- Смысл slot+8, payload массивов и helper `0x473F30`; bones/model/hitboxes не названы без доказательства.
- Полный lifecycle при смерти/dormancy/смене entity; найденные reset/copy не исчерпывают lifecycle.
- Явный rewind/restore сущности, engine lagcomp integration, корректность всей hit-registration/knife policy.
- Knife-only либо общая применимость истории, и подтверждение новых conditional C-выводов отдельными байтами.

**Итог: восстановлен конкретный producer/aging/validation-срез; вся lagcomp готовой не объявляется.**


---
