<!-- split-part | CS2_RESEARCH_MASTER.md lines 1889-2113 | body-sha256 5831b0c0ec015911abae9ee60baa1494c34b553df08a68d0632e3ff27489cf58 -->
[← все части](../README.md) · [индекс отчётов](00-index.md)

<!-- split-body-start -->

<a id="doc-02"></a>

# Приложение D02. `analysis/review_rng/summary.md`

**Дословный исходный отчёт.** Даты/статусы/неизвестные роли внутри относятся к своему этапу. При расхождении использовать актуальный свод и поздние доказательства. Пути внутри текста — исторические: соответствующие материалы встроены в MD/CPP и находятся через поиск исходного пути.

# Static review RNG: feature semantics и evidence

## Вывод

**`0x212c380fb10` (RVA `0x50fb10`) — реальная aim/fire-command логика: направление на точку, коррекция/выбор углов, tick/subtick и кнопки атаки. `0x212c35e9b50` (RVA `0x2e9b50`) — пространственный визуальный эффект с двумя концами, цветовыми параметрами и вариантами ресурса; это не расчёт hitchance.**

В этих восьми функциях не подтверждён самостоятельный алгоритм оценки hitchance, моделирования weapon spread или antiaim. Это вывод о показанном поведении, **не утверждение об отсутствии таких подсистем в бинарнике или в непросмотренных callees**. `384bc60` относится преимущественно к обработке результата/диагностике выстрела, но имеет дополнительные записи состояния — называть её полностью безвредной UI-only функцией преждевременно.

Приоритеты дополнительной декомпиляции: [shortlist.md](shortlist.md), машиночитаемый список прямых callees — [callee_addresses.txt](callee_addresses.txt).

## Подтверждение по новому caller `505f50`

Прочитан только узкий контекст `analysis/decompiled_features/212c3805f50.c:312–454`, без разворачивания всего 189K файла. В строках 420–436 caller выбирает одну запись из массива `this+0x7ac0…0x7ac8` с шагом `0x30`, по flags и integer score `+0xc`; затем строка 448 передаёт выбранную запись в `50fb10` (callsite RVA `0x506787`). Следовательно точнее называть `50fb10` **apply/finalize выбранного aim-решения в fire command**, а не поиском лучшей цели. Семантика score (damage или другое) не восстановлена.

Это **положительное evidence candidate selection**: сравниваются flags записи (`+0x10`, `+0x11`), integer field `+0xc` и flag связанного source record `+0x14`. Выбранный candidate передаётся как point-record, а его `+0x18` — как source record. Это не случайный выбор и не sampling внутри `50fb10`; точные бизнес-имена полей и смысл integer score неизвестны.

`3375a0` дополнительно разобран в [addendum_vector_sampling.md](addendum_vector_sampling.md). **Подтверждённое поведение `511d50`:** копирование `[0,0x5c)`, два vector-triples at `+0x60` (element `0x48`) и `+0x78` (element `0xac`), tail `[0x90,0xba)`, сохранение прежних flags `B6/B7/B8`. Это поддерживает роль copy-helper, не aim solver; полный разбор находится в основном отчёте.

`7447e0` не нужен для подтверждения core роли: прежнее совпадение bytes `ac 7a 00 00` там относится к rel32 call, не к чтению state+0x7aac. Дополнительного target-selection evidence из него не заявляю.

## Метод и ограничения

- Только чтение восьми `analysis/decompiled_rng/*.c`, read-only SQLite (`mode=ro&immutable=1`, `query_only=ON`), чтение dump и статический `objdump`. Исходный бинарник не исполнялся. Ghidra, проект, symbols и исходный псевдокод не изменялись. Все созданные файлы находятся в `analysis/review_rng/`.
- Base: `0x212c3300000`. В этом raw memory dump **file offset = VA − base = RVA**. Размер `0x5001000`; начальный PE header отсутствует. Адреса `functions.begin/end/decoded_end`, `refs.source/target/owner` в SQLite — RVA, не VA.
- По результатам построения индекса полностью декодированы 43,604 из 45,064 unique runtime ranges. Linear Capstone останавливается на первой invalid instruction. **Отсутствие call/xref не используется как отрицательное доказательство.** Register-indirect calls также не равны прямым refs.
- Проверенные примеры усечения: `50fb10`: `decoded_end=0x5105b8`, `end=0x510ffe`; `54bc60`: `0x54db20` против `0x54e052`; caller `505f50`: `0x507f53` против `0x50bd30`; caller `4feba0`: `0x4ff46b` против `0x505404`. Даже `decoded_end=end` не означает правильный CFG или полный межпроцедурный граф.
- Имена функций не восстановлены. Термины «effect», «aim», «компенсация», «лог» — аналитические роли, не исходные символы. Названия IAT_RandomFloat/Int и V_modff берутся из предоставленного псевдокода; их слоты сопоставлены с байтами. Внешние указатели ведут вне данного dump.
- Ghidra теряет FP/stack arguments и путает bitcasts с преобразованиями типов. В частности `RandomFloat()` в `50fb10` **имеет два аргумента**, восстановленные по XMM-регистрам. AVX XOR с `0x80000000` — смена знака float, не работа с RNG seed.
- Confidence ниже — экспертная оценка роли, не статистическая вероятность и не доказательство исходного имени.

## Быстрый triage восьми функций

| VA / RVA | Роль, подтверждённая наблюдениями | Confidence и граница вывода |
|---|---|---|
| `0x212c380fb10` / `0x50fb10` | Aim + формирование команды атаки и её временных/угловых записей | Высокая. Rage-specific контекст — предположение; target selection находится выше в caller, spread и hitchance здесь не показаны |
| `0x212c35e9b50` / `0x2e9b50` | Косметическая пространственная генерация эффекта: endpoints, RGB/alpha, lifetime, варианты | Высокая. Particle/control-point API — обоснованное предположение; конкретное название эффекта неизвестно |
| `0x212c384bc60` / `0x54bc60` | Постобработка результата выстрела: причины/status, форматирование, уведомление/лог, дополнительные state updates | Высокая для логирования; принадлежность RNG-записей к gameplay не установлена. Не считать ни hitchance, ни гарантированно pure UI |
| `0x212c3722970` / `0x422970` | UI выбора одного из пяти интервалов и случайного значения внутри выбранного интервала | Высокая для UI. Cosmetic/wear selector — предположение без названий/инициализации порогов |
| `0x212c3706c10` / `0x406c10` | UI-редактирование целого значения с рандомизацией младших четырёх десятичных разрядов | Высокая для UI. Конкретное поле (seed, pattern, wear и т. п.) не установлено |
| `0x212c3a1ebe0` / `0x71ebe0` | UI transition/state reset; две независимые случайные компоненты ±0.08, reset timers | Высокая, с учётом UI caller. Называть это spread из-за двух random-компонент нельзя |
| `0x212c37fd710` / `0x4fd710` | Hash lookup строк и случайный выбор строкового варианта из массива | Высокая для string-selector; конкретный потребитель текста неизвестен. Предоставленный `.c` существенно неверен |
| `0x212c82f1690` / `0x4ff1690` | Init/DllMain-подобная обёртка: при reason==1 два вызова, затем return 1 | Высокая для wrapper; DllMain — предположение. В теле нет RNG; поведение callees не исследовано |

## 1. `2e9b50`: geometry для эффекта, не для попадания

Источник: [212c35e9b50.c](../decompiled_rng/212c35e9b50.c), основная математика строки 103–233, ресурс/параметры 234–355, lifetime/варианты 374–488, вторичный эффект 635–661. Байты: [212c35e9b50.asm](212c35e9b50.asm).

### Восстановленные ветви

Для конечных обычных float, игнорируя signed zero и служебные AVX lanes:

```text
theta = returned_by_35eab50[1] * pi/180
forward = (cos(theta), sin(theta), 0)
right   = (-sin(theta), cos(theta), 0)
up      = (0, 0, 1)

если param_3 != null:
    B = *param_3                  # три исходных float
    A = B + right*U(-1200,1200)
          + up*(2500*U(0.9,1.1))
          + forward*I(-600,600)

иначе:
    O = singleton_4a6f378.xyz_at_0x10
    C = O + forward*I(3000,4000)
          + right*U(-1200,1200)
          + up*I(350,650)
    w = (I(0,1)==0 ? -1 : +1) * 750*U(0.85,1.15)
    B = C - right*w
    A = C + right*w + up*(2500*U(0.9,1.1)) + forward*I(-600,600)
```

`I`/`U` обозначают вызовы предоставленных RandomInt/RandomFloat, а не восстановление конкретного PRNG или гарантию endpoint inclusivity.

После генерации A/B выполняются проверки доступности менеджера/ресурса, вызов `393a320` со строковым именем и получение handle (`-1` означает отказ). Затем `3936050` вызывается с индексами:

- `1 → B`, `0 → A`;
- `2 → первые RGB × 255`, `3 → вторые RGB × 255`;
- `4 → первая alpha × 255`, `5 → вторая alpha × 255`.

Далее сохраняется запись размером `0x20`: handle/связанные данные, время окончания (`time + configurable lifetime`) и две интенсивности. Это намного сильнее свидетельствует о render-effect/control-points, чем сам факт sin/cos и RNG — об aim.

В дополнительной включаемой ветви выбирается `k=I(0,2); k += (previous<=k)`: один из четырёх индексов без повторения предыдущего, если previous в 0…3. По строковому варианту вызывается `3945b20`, с геометрией на основе `angle=(k+1)*2.094394922`, радиуса 128 и значений 30/128. Точный тип вторичного эффекта/звука из одной этой обёртки не установлен.

### Константы из dump

В столбце bytes — little-endian float32; VA всегда `base + offset`.

| Offset | VA | Bytes | Значение / роль |
|---|---|---|---|
| `0xe08658` | `0x212c4108658` | `35 fa 8e 3c` | `0.0174532923847`, pi/180 |
| `0xe98ba8`, `0xe98bac` | `0x212c4198ba8`, `0x212c4198bac` | `00 00 96 c4`, `00 00 96 44` | −1200, +1200, lateral displacement |
| `0xe42de8`, `0xe98ba0` | `0x212c4142de8`, `0x212c4198ba0` | `66 66 66 3f`, `cd cc 8c 3f` | ≈0.9, ≈1.1 |
| `0xe98ba4` | `0x212c4198ba4` | `00 40 1c 45` | 2500, vertical displacement scale |
| `0xe98bb0`, `0xe98bb4` | `0x212c4198bb0`, `0x212c4198bb4` | `9a 99 59 3f`, `33 33 93 3f` | ≈0.85, ≈1.15 |
| `0xe98bb8` | `0x212c4198bb8` | `00 80 3b 44` | 750, endpoint separation scale |
| `0xe06ab4`, `0xe95360` | `0x212c4106ab4`, `0x212c4195360` | `00 00 7f 43` | 255; vector at e95360 = (255,255,0,0) |
| `0xe98bbc` | `0x212c4198bbc` | `91 0a 06 40` | 2.094394922 ≈ 2pi/3 |
| `0xe98bc0` | `0x212c4198bc0` | `00 00 00 43` | vector (128,128,0,0) |
| `0xdecdf0`, `0xe87100` | `0x212c40ecdf0`, `0x212c4187100` | `00 00 00 80` | sign bit, −0.0 interpreted as float |

Integer bounds are immediates in code, not separate `.rdata` constants: RVA `0x2e9cf7/0x2e9cfc` → 3000/4000; `0x2e9d62/0x2e9d67` → 350/650; `0x2e9c83/0x2e9c88` and `0x2e9dec/0x2e9df1` → −600/600. `30.0` immediate at instruction `0x2ea59c` (`0x41f00000`), `128.0` at `0x2ea5cc` (`0x43000000`).

**Evidence sites:** `0x2e9d31–0x2e9d41` restores both RandomFloat bounds omitted by Ghidra; creation call `0x2e9f6e → 0x63a320`; parameter calls `0x2ea028/0x2ea077/0x2ea0c6/0x2ea110/0x2ea15a/0x2ea190 → 0x636050`; variant RNG `0x2ea44f`; secondary submission `0x2ea5ff → 0x645b20`.

Trig roles have independent static support: `4092b60` returns 1 for tiny input and uses `1−x²/2` (constants at e87378/e87270/e87280), so cosf-like; `409abe0` preserves tiny input and uses `x−x³/6` (e88d90), so sinf-like. No code from those routines was executed.

## 2. `50fb10`: aim angles + attack command, RNG здесь временной

Источник: [212c380fb10.c](../decompiled_rng/212c380fb10.c), математика 84–299, history/time 300–443, view write 475–633, attack/subtick entries 634–797. Байты: [212c380fb10.asm](212c380fb10.asm).

### Ветви и data flow

1. `3811d50(command, this+0x2b8)` копирует базовое состояние с двумя массивами (элементы `0x48`, `0xac`), а не проверяет попадание. `command=*(this+0x1f0)`.
2. Нормализуется `(this+0x6c4, this+0x6d0)` как integer tick + fraction, с `modff`, переносом/заёмом через 1.0. Выбираемая временная пара ограничивается относительно `T−3 … T+1`; upper fraction `0.9999999404` — float непосредственно ниже 1. Есть готовая пара в `param_2+0x30`, когда выставлен flag `param_2+0x38`.
3. Если `param_6+0xc` установлен, берутся готовые углы `param_6.xyz`. Иначе из `D=param_3.xyz−param_2.xyz` рассчитывается:

```text
yaw   = atan2(D.y, D.x) * 180/pi
pitch = atan2(-D.z, sqrt(D.x²+D.y²)) * 180/pi
normalize(a) = a - 360*floor(a/360 + 0.5)
```

   Для вертикального D отдельная ветвь 90/270 градусов и yaw=0. `atan2`-роль внешнего слота `0x212c42926c8` следует из аргументов и формулы; сам внешний код в dump не доступен. Знак D.z меняется sign-mask.
4. Два вызова `36375a0` на состояниях `this+0x5a0` и `this+0x5e8` дают векторные поправки. При `(this+0x1eb)!=0 || (this+0x6930)!=0` сумма XY вычитается из aim XY. Для roll используется либо 0, либо минус сумма Z. **Назвать конкретную поправку recoil/punch можно лишь вероятностно:** требуется декомпиляция `36375a0`; его начальные bytes показывают temporal/vector sampling и сравнение cached values, но не раскрывают происхождение векторов.
5. В другой ветви используются два binary-search/lower-bound прохода по float-таблице `4a52680…4a52688`. Сравнение производится через sin/cos и `408f800` (atan2f-like), а не через серию случайных выстрелов. Выбранные углы могут заменяться элементом таблицы. Таблица в данном dump не инициализирована; её реальные значения/назначение не восстановлены. Это **не evidence Monte Carlo или hitchance**.
6. При flag `param_2+0x14` может сработать `3609500`, заменяя пару tick/subtick. Затем сохраняется history entry `0xac`: углы в `+0/+4/+8`, время в `+0xc/+0x10`, другая временная пара в `+0x18`, presence flags. Это адреса внутри записи, не глобальные RVA.
7. Ветка `(this+0x6930)==1 && (param_2+0x14)==0` действительно вызывает RandomFloat, но результат идёт в **время**, не в направление или hit test:

```text
upper_seconds = min(weapon_related_value, 0.1)
lower_seconds = max(upper_seconds - 0.05, 0)
delta_ticks = U(lower_seconds, upper_seconds) * 64
(this+0x4a4, this+0x4a8) = normalized_tick_pair + delta_ticks
```

   Для ожидаемого неотрицательного input это максимум 6.4 tick, ширина окна максимум 3.2 tick. **64 здесь — коэффициент seconds→ticks, не число spread samples.** Ghidra показывает `RandomFloat()` без аргументов, но `0x5102bd` кладёт lower в xmm0, `0x5102c1` upper в xmm1, `0x5102c8` вызывает IAT; `0x5102ce` умножает результат на 64.
8. При одном config branch есть отдельно пересчитанные/ограниченные pitch/yaw и запись через внешний указатель; pitch clamp `[-89,+89]`, yaw `[-180,+180]`, roll=0. Это совместимо с visible/silent aim разделением, но исходное имя config неизвестно; путь содержит opaque guards и требует осторожности.
9. `param_5==0` устанавливает mask `1`, иначе `0x800` и очищает несовместимые биты. Это согласуется с IN_ATTACK/IN_ATTACK2. Есть привязка к history index, фильтрация/добавление subtick press records `0x48`, лимит 32 и special case item code `0x40`. Это сильное положительное evidence **aim + fire command**, не UI и не antiaim.

### Константы из dump

| Offset | Bytes / lanes | Значение / роль |
|---|---|---|
| `0xe98eb0` | `00 00 34 43` ×2 | 180, rad→degree numerator |
| `0xe98ec0` | `db 0f 49 40` ×2 | 3.141592741, denominator pi |
| `0xe98910` | `61 0b 36 3b` ×2 | 1/360 ≈ 0.002777777845 |
| `0xe95370`, `0xdec7d8` | `00 00 00 3f` | 0.5 |
| `0xe98920`, `0xe98934` | `00 00 b4 c3` | −360 |
| `0xe06ab8` | `00 00 b4 43` | +360 |
| `0xe98a6c`, `0xe98ed0` | `00 00 b4 42`, `00 00 87 43` | 90, 270 |
| `0xe9b940` | `00 00 b2 c2 00 00 34 c3` | pitch/yaw minima (−89,−180) |
| `0xe9b950` | `00 00 b2 42 00 00 34 43` | maxima (+89,+180) |
| `0xdec898`, `0xe06abc` | `00 00 80 3f`, `00 00 80 bf` | +1,−1 tick-fraction carry |
| `0xe98f44` | `ff ff 7f 3f` | 0.9999999403953552 |
| `0xe98754`, `0xe9b934` | `cd cc cc 3d`, `cd cc 4c bd` | ≈+0.1,≈−0.05 seconds |
| `0xe58838` | `00 00 80 42` | 64 seconds→ticks |

**Evidence sites (RVA):** angle rad→degree `0x50fd58/0x50fd60`; wrap `0x50fd9f…0x50fdbd` (`vroundps ... 0x9` means floor with exception suppression); vector helpers `0x50fe03/0x50fe46`; lower-bound math calls `0x50fe90…0x50ffcb`; time override `0x510034`; history pair stores `0x510165/0x510175/0x510179`; random time `0x510230…0x510345`; angle bounds `0x510555/0x51055d`. The late attack writes are also visible in `.c:634` onward; the SQLite decoder stopped before them.

The shared IAT slots are `0xf92658` (RandomFloat), `0xf92660` (RandomInt), `0xf926e8` (V_modff). Their presence alone does not classify the feature.

## 3. Остальные: кратко, с оговорками

### `54bc60`: shot-result/log, не доказанная spread-математика

- `param_1+0x10` или status `param_1+0xd4 == 1` → early return. По status 3…10 выбираются строки через `3b769a0`; затем formatting через `3b78aa0`, formatting-details `384f480`, и display object через `3851c60`. Есть сравнение с предыдущим текстом, увеличение duplicate count, обновление времени жизни. Это положительное evidence логирования (`.c:209`, `653`, `949`, `970`, `1057`, `1091`).
- Перед этим RNG обновляет singleton `*4a57008`:

```text
state[0x7aac] += U(-1.2,-1.0)
if state[0x7aac] < -10: state[0x7aac] = U(4,5)
state[0x7ab0] = U(-6,6)
state[0x7ab4] = U(-10,10)
```

  Bytes/offsets: `e9bfb0=9a9999bf` (−1.2), `e06abc=000080bf` (−1), `e98a64=000020c1` (−10), `e06a98=00008040` (4), `e97c4c=0000a040` (5), `e98fdc=0000c0c0` (−6), `e42df4=0000c040` (6), `e06aa4=00002041` (10). Stores: RVA `54bce5`, `54bd11`, `54bd32`, `54bd4c`.
- Значения `+0x7aac/+0x7ab0/+0x7ab4` не прослежены до потребителя. Ограниченный поиск exact disp32 на `[0,0xde0000)` нашёл писателя и два посторонних совпадения bytes (rel32 call и stack displacement); это **не доказательство отсутствия readers**, особенно при SIMD/aliases/сдвинутой базе. Не называю эти поля ни antiaim, ни cosmetic noise как установленный факт.
- Процент `round(100*field_4c)` здесь упаковывается для сообщения (`.c:653–662`), а не рассчитывается sampling loop. Также имеется отдельный status==10 path с проверкой entity/tick difference и изменением другого состояния (`.c:816–916`). Следовательно, функция преимущественно диагностическая, но не чистый formatter.

### `422970` и `406c10`: UI RNG

- `422970`: `value/100`, выбор одного из пяти подписанных интервалов; после успешного UI-select (`3ae06f0`) `newValue=int(U(lo,hi)*100)`, запись через setting object и vfunc `+0x160`. Begin/end-подобные UI calls `3aef790`/`3b311a0`, font-relative width 140. RNG call sites `422d3f/422dd1/422e66/422efb/422f90`. Константа 100 — `e06ab0=0000c842`.
- Пороги хранятся в singleton fields `+2c8/+300/+338/+370/+3a8`; pointer в dump нулевой. Wrapper `35aef90` вызывает `35af0c0`, но конструктор обнуляет эти поля, не задаёт wear boundaries. Поэтому пяти интервалов недостаточно, чтобы объявить их Factory New…Battle Scarred установленными именами.
- `406c10`: перед UI call берётся quotient `value/10000`; при успехе записывается `quotient*10000 + I(0,9999)`. RNG site `406cb4`. Это редактирование setting, а не подсчёт попаданий. Ограниченные refs `3c4c230` ведут к тем же UI helpers.

### `71ebe0`: transition/reset

`param_4+0x28 == -1`: при mode 4 сохраняется mode 3 и независимо выбираются два знака ±0.08 в `+0x20/+0x24`; иначе offsets обнуляются. Затем освобождается предыдущий объект, сбрасываются два timer-like состояния и записываются timestamps `/10000`. Constants `e9ad90=0ad7a33d` и `e9dd90=0ad7a3bd`. RNG indirect calls `71ec32`, `71ec5d`. Caller `71da10` содержит UI/локализационные вызовы. Это не основание для spread attribution.

### `4fd710`: важная ошибка Ghidra

Предоставленный `.c` после `call 40c5d20` превращён в memcpy-подобное тело и помечает дальнейшие блоки unreachable. **Байты показывают нормальный CALL/return continuation**, а не доказанно недостижимые блоки:

- `4fd774: e8 a7 85 8c 00` → call `40c5d20`, затем строка завершается NUL и выполняется lookup;
- FNV-1a-like string hashing с offset basis `0xcbf29ce484222325` (`4fd831` и др.), string compare `4fd9ea/4fdd1a`;
- `4fddb1…4fddbc`: `count=(end-begin)/0x20; RandomInt(0,count−1)`;
- после `4fddc2` выбранный `0x20` SSO-string копируется в output; при отсутствии записи output — empty string.

Следовательно RNG выбирает **строку**, не random direction. Конкретная подсистема текста не установлена. Декомпиляцию надо повторять с проверкой prototype/control-flow `40c5d20`; проект пользователя в этом review не исправлялся.

### `4ff1690`: init wrapper

Bytes `4ff1699` проверяют reason==1; calls `4ff169e → 4ff10b0`, `4ff16a6 → 4ff1450`; `4ff16ab` возвращает 1. Runtime range отсутствует в данной таблице functions, но тело подтверждено bytes. Ни aim, ни UI, ни RNG из этого wrapper не следуют; loader/init — отдельная категория.

## Артефакты и воспроизводимость

- `constants.tsv` / `constants.json`: 178 **сырых наблюдений** DAT/Ram с 16 bytes, float/u64 interpretations и строками использования. Не каждый DAT является float-константой: указатели, byte masks и зашифрованные строки нельзя семантически трактовать по столбцу f32.
- `refs.json`: ограниченные refs владельцев и incoming refs, metadata, SHA-256 исходных восьми `.c`; incoming=0 означает только «не найдено этим индексом».
- `targeted_evidence.json`, `followup_refs.json`, `helper_ranges.json`, `helper_constants.json`: дополнительные ограниченные проверки. `targeted_evidence.json` хранит raw byte candidates, не подтверждённые xrefs по полям.
- `*.asm`: восемь ограниченных исходными range листингов и выбранные helper prefixes/ranges. Это linear disassembly, не гарантия достижимости; mixed code/data и opaque paths могут искажать локальное декодирование.
- `collect_evidence.py`: read-only collector для refs/constants, writes только рядом с собой; исполнялся этот аналитический скрипт, не исходный бинарник.

**Обновление после дополнительного прохода:** `36375a0` разобран в `addendum_vector_sampling.md`, а `3609500` и отбор исторических записей — в `../CORE_ADDENDUM_RU.md`. Полные producers, hit testing и scoring остаются неразрешёнными. Для уверенного имени эффекта отдельно потребуются `393a320` + `3936050`.


---
