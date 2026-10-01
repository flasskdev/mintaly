<!-- split-part | CS2_RESEARCH_MASTER.md lines 2641-2828 | body-sha256 0813c087a7435f56e5459388f5364b2c7b2aa44b078e7c62fb45ce0bd6418876 -->
[← все части](../README.md) · [индекс отчётов](00-index.md)

<!-- split-body-start -->

<a id="doc-10"></a>

# Приложение D10. `analysis/phase2/main/PENETRATION_AND_MELEE_RU.md`

**Дословный исходный отчёт.** Даты/статусы/неизвестные роли внутри относятся к своему этапу. При расхождении использовать актуальный свод и поздние доказательства. Пути внутри текста — исторические: соответствующие материалы встроены в MD/CPP и находятся через поиск исходного пути.

# Damage/penetration и melee: восстановленные части

Все адреса — RVA от `0x212C3300000`, если явно не указан VA. Это статическая реконструкция, а не исходники, не готовая DLL и не динамически проверенная эквивалентность. Условные C-листинги получены из отдельной модели успешных environment-check ветвей; оригинальный BIN неизменён.

## 1. Bullet damage: `0x51DAC0`

Реальная цепочка от sample-worker: `0x51D320 → 0x51DAC0 → 0x2F19C0 → 0x2F2110 → 0x2F0DD0`. Это существенно сильнее совпадения слова «penetration» или импорта RNG.

Подтверждённые шаги scalar evaluator:

1. Нормализовать направление `point - source`, включая отдельные обработки нулевой длины и экстремальных значений; растянуть на `context.float_at_0x100`.
2. `0x2F19C0` пересекает луч с подготовленным геометрическим массивом record+0x80. Математика содержит ось, радиус, квадратные корни, ветвь почти параллельного направления, выбор ближайшей доли. Это локальная геометрия hit volumes; не чтение стен мира.
3. Потребовать положительный hit-result, ненулевой геометрический объект и разрешённый category bit из context+0xCC.
4. Вычислить дистанционную оценку, применить коэффициент категории и проверить минимальный score из target+0x18.
5. Подготовить trace workspace и вызвать `0x2F2110`; затем пропустить сегменты через `0x2F0DD0`.
6. Потребовать возвращённые segment/object pointers, флаг object+0x33==1 и ненулевой object+0x10.
7. Для значения weapon-like field context+0x118 == 0x1F использовать segment+8 напрямую; иначе умножить его на target category multiplier. Не присваиваем оружию название только по этому ID.
8. При неотрицательном результате вернуть float score, опционально записать отношение к предварительной оценке и result-record pointer. Неуспех имеет sentinel `-1.0f`, RVA константы `0xE06ABC`.

Сырой Ghidra C иногда ошибочно показывает `ulonglong` return и усечённые вызовы: реальная ABI передаёт float через XMM-регистры, часть stack-аргументов скрыта. Такой листинг нельзя просто скомпилировать как восстановленный оригинал. Проверять соответствующий ASM.

### Range attenuation

`0x5239D0` подготавливает context+0x10C из `M_0xD97150(range_parameter) * 0.002f`. `0x51DAC0` вызывает `M_0xD93590(distance * context+0x10C)`.

По статической математике библиотечных kernels:

- `0xD97150 → 0xD97170`: log2f-подобный kernel; вход 1 возвращает 0, выделяется exponent, коэффициент `1.4426950186867042 ≈ log2(e)` по RVA `0xE8FF78`.
- `0xD93590 → 0xD935B0`: exp2f-подобный kernel; полином имеет `0.6931471806916203 ≈ ln(2)` по `0xE8D408` и таблицу степеней двойки.
- `0xE99580`: float `0.0020000000949949026`.

На обычных конечных положительных входах математический смысл:

```text
attenuation = exp2(distance * log2(range_parameter) / 500)
            = range_parameter ** (distance / 500)
estimate = base_damage * attenuation * category_multiplier
```

Это не обещание bit-exact совпадения Python/C++ pow с библиотечными kernels. Исключительные float-входы, округление SIMD и динамический выбор библиотечной реализации нужно сохранять отдельно.

## 2. Почему полного penetration в этом BIN нет

`0x2F2110` выполняет реальные вызовы за пределами образа:

| Адрес | Наблюдаемая роль в цепочке |
|---|---|
| **VA `0x7FFCEF4A3E50`** | Внешний вызов при построении trace workspace |
| **VA `0x7FFCF0FE45A0`**, virtual `+0x510` | Внешний объект/интерфейс trace |
| **VA `0x7FFCEF48E030`** | Дополнительная обработка изменённого списка |
| **VA `0x7FFCEF4A8DB0`** | Внешняя обработка каждого penetration segment с изменением текущего damage-state |

Указанные VA не находятся в `[0x212C3300000, 0x212C8301000)`. Код по ним и соответствующий world/material state в приложенном образе не содержатся. Имена модулей для этих адресов без карты модулей процесса не установлены. Нельзя приписывать их конкретной DLL лишь по старшему префиксу адреса.

Поэтому можно восстановить вызывающую логику, layout и остановки, но нельзя честно получить из этого файла тело внешнего расчёта потерь в материале, коэффициенты surface combinations, trace-to-exit и все правила двигателя. Подстановка типового CS:GO autowall была бы новым алгоритмом, а не реконструкцией этого дампа.

### Точная структура цикла `0x2F0DD0`

Рабочий список: count `workspace+0x1C20`, begin `workspace+0x1C28`, stride **0x18**. Возвращается пара object/segment pointers либо нули.

```text
state.damage = request.float_at_0x34
state.parameters = request.fields_0x38_0x3C, caller_parameters
request.float_at_0x48 = 0

for index in [0, segment_count):
    segment = begin + index * 0x18
    before = state.damage
    stop = EXTERNAL_0x7FFCEF4A8DB0(workspace, state, segment, request.field_0x40)

    if segment.flags_at_0x14 & 1:
        request.float_at_0x48 += before - state.damage

    if stop or state.damage < request.float_at_0x44:
        truncate_segment_count_to(index + 1)
        return null_pair

    if not (segment.flags_at_0x14 & 1):
        object = workspace.object_array + segment.u16_at_0x12 * 0x38
        if object.id_at_0x2C == request.id_at_0x20
           and object.byte_at_0x33 == 1
           and object.pointer_at_0x10 != null
           and object.pointer_at_0x10.u32_at_0x38 < 12:
            segment.float_at_0x08 = state.damage
            segment.field_at_0x0C = state.caller_parameter
            truncate_segment_count_to(index + 1)
            return (object, segment)

return null_pair
```

Названия `damage`, `segment`, `object` — смысловые имена аналитика. `EXTERNAL_*` намеренно остаётся неизвестной зависимостью, не «восстановленной» заглушкой, всегда возвращающей успех. Структуру запроса нельзя считать подтверждённым SDK-типом целиком.

`0x2F2110` дополнительно временно меняет filter+0x37 и bit 1 в filter+8, вызывает внешние trace-функции, при необходимости вставляет две записи в список stride 0x38 и восстанавливает сохранённые flags. В decompiler output есть KUSER-derived аргумент внешнего вызова; его runtime value не доказан.

## 3. Важное уточнение: `0x528B90` — melee/knifebot-подобная ветка

Это **не bullet penetration kernel**. Основания — совокупность двух режимов атаки, фронтального/заднего условия, damage tables, ограниченной дальности и выбора primary/secondary. Термин knifebot — аналитическая идентификация, не восстановленное исходное имя.

Подтверждённые константы оригинала:

| RVA | Значение / использование |
|---|---|
| `0xE9BB30/34` | multipliers **1.0 / 0.825**, выбор по положительности entity field |
| `0xE98144`, `0xE99930`, `0xE9BA20` | **25 / 40 / 40** для primary/front timing logic |
| `0xE980C4` | временная граница **0.4** |
| `0xE98A6C` | **90** для заднего primary случая |
| `0xE9BB40/44` | **65 / 180** для двух secondary случаев |
| `0xE9ADB8` | directional dot threshold **0.475** |
| `0xE9ADB4`, `0xE98820` | trace distance **66 / 50**, соответственно mode 0 / 1 |

Следует использовать именно **0.825, 66 и 50**, а не переносить распространённые из других версий значения 0.85, 48 или 32.

Путь:

- Сравнить время с 0.4 и выбрать базовый primary score 25/40; дополнительные config-ветви +0x6962 и helper `0x313070` влияют на вариант выбора.
- Нормализовать горизонтальное направление от source к record position. Через target pitch/yaw получить forward и проверить dot > 0.475; это переключает front/back damage.
- Применить 1/0.825 по entity field. Название armor вероятно по поведению, но schema offset ещё заглушка; аналогично health-подобный field нельзя считать разрешённым SDK offset.
- Из integer health-like бюджета вычесть float entries в массиве stride 0x1A0 с совпадающим target identifier; точный lifecycle этих entries здесь не восстановлен.
- Config +0x4C1/+0x4C2 и lethal comparisons выбирают mode. Продолжить только если вычисленный mode == входной requested mode.
- Проверить несколько вертикальных точек с шагом 2; число определяется record+0x58 и выражением `28 - int(value*8)`, с отдельным fallback.
- Выполнить ray/hull проверки через `0x1463A0`; внутри — внешний **VA `0x7FFCEF616040`**, а не локальный trace engine.
- Для принятых точек выбрать позицию и добавить candidate через `0x52AD40`.

### Melee candidate stride 0x30

| Offset | Наблюдаемое поле |
|---|---|
| +0x00/+0x04/+0x08 | XYZ |
| +0x0C | integer damage-like score |
| +0x10 | lethal: оставшийся health-like бюджет <= score |
| +0x11 | attack mode |
| +0x18/+0x20/+0x28 | source/target/record pointers |

Этот массив нельзя смешивать с bullet candidates stride **0x58**, у которых +0x44…+0x50 — четыре sample aggregates.

## 4. Связка hitchance с health/minimum damage

Начало `0x51E4F0` связывает ранее символические thresholdA/B с источниками:

```text
health_budget = float(entity.int_at_dynamic_offset_17586C4)
for matching_entry in outcome_array_stride_0x1A0:
    if matching_entry.float_at_0x4C > 0.75:
        health_budget -= matching_entry.float_at_0x4C * matching_entry.float_at_0x50
if health_budget <= 0:
    return

setting = context.float_at_0x88
if setting <= 100:
    minimum_score = min(setting, health_budget)
else:
    minimum_score = health_budget
    if context.byte_at_0x50 == 1:
        minimum_score = ceil(health_budget * 0.5)
minimum_score = min(minimum_score, 130)
target.float_at_0x14 = health_budget
target.float_at_0x18 = minimum_score
```

Это доказывает смысл «health-like budget / minimum score» через data-flow. Поле entity ещё имеет placeholder-offset; исходные SDK/GUI-имена не восстановлены. Нельзя заменять ветвь setting>100 популярной формулой «health+(setting−100)»: в этом фрагменте такой формулы нет. Mode+0x50 нельзя автоматически назвать double-tap, хотя half-health поведение совместимо с двухвыстрельной оценкой.

`0x7BDB90` возвращает category multiplier с внешними runtime scalar settings. В switch по `group−1`: group 1 использует входной float parameter × отдельный runtime scalar; group 3 — body scalar ×1.25; groups 6/7 — body scalar ×0.75; groups 8/9 и выход за диапазон — 1.0. Остальные обработанные cases используют body scalar. Runtime scalar выбирается в зависимости от entity byte field ==3; внешние корни включают VA `0x7FFCF0F68D98`, `0x7FFCF0F68DB8`, `0x7FFCF0F68DA8`, `0x7FFCF0F68DC8`. Названия convar и точные значения отсутствуют в этом dump.

`0x51E4F0` дополнительно формирует 12 per-category multipliers, учитывая positive armor-like field, bit mask 0x13D, особый group 1 flag и weapon scalar. Сама branch-арифметика присутствует в `conditional_targets/212c381e4f0.c`; её нельзя свести к одному множителю 0.5 для всех попаданий.

## 5. Прединициализированные поля

RVA `0x1758658`, `0x17586C4`, `0x1758290`, `0x1758680`, `0x176F7C8`, `0x17702D0`, `0x176FE34` содержат **0x13371337**, не рабочие offsets. Два sampling-table массива также ещё нулевые; их формулы восстановлены из producer, не считаны как готовые runtime values.

Это согласуется с предоставленной характеристикой pre-entry dump. Для независимой проверки названий полей, полного trace/penetration и runtime поведения требуются код соответствующих внешних модулей, карта модулей/версий и post-init состояние, полученные в разрешённой среде. Исходники, символы или SDK соответствующей сборки существенно уменьшают неопределённость.

## Файлы доказательств

- `../conditional_trace/212c381dac0.c`, `212c35f19c0.c`, `212c35f2110.c`, `212c35f0dd0.c`.
- `../conditional_core/212c3828b90.c`, `212c382ad40.c`, `212c34463a0.c`.
- Соответствующие `../conditional_assembly/*.asm` — первичная проверка ABI/инструкций в условной модели.
- `verification.json` сверяет оригинал, модель, branch targets, константы и границы внешних адресов. Эта проверка не заменяет execution equivalence.


---
