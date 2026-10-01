<!-- split-part | CS2_RESEARCH_MASTER.md lines 2544-2640 | body-sha256 9c02566812b335e7b2bebb637c7fbe6700d5f2f55b2b7b8780214c9abcc7c93c -->
[← все части](../README.md) · [индекс отчётов](00-index.md)

<!-- split-body-start -->

<a id="doc-09"></a>

# Приложение D09. `analysis/phase2/spread/summary.md`

**Дословный исходный отчёт.** Даты/статусы/неизвестные роли внутри относятся к своему этапу. При расхождении использовать актуальный свод и поздние доказательства. Пути внутри текста — исторические: соответствующие материалы встроены в MD/CPP и находятся через поиск исходного пути.

# Подтверждённый HC/spread-sampling kernel CS2

## Итог

**Приоритетный узел — `0x525830`: детерминированная оценка 64 направлений с четырьмя агрегатами.** Общий worker — **`0x51d320`**, подготовка базиса — **`0x51cf40`**, producer sampling tables — **`0x512710`**. Fast counterpart **`0x526290` тоже рассчитан на 64 направления**, обрабатывает их пакетами по 8 и может экстраполировать остаток после раннего выхода. Это не UI RandomFloat и не вывод только по константе 64.

Native `CUniformRandomStream/ran1` и точное воспроизведение engine weapon RNG **не подтверждены**. Найденный алгоритм детерминированный; называть его Monte Carlo нельзя. Поля thresholds/masks ниже названы символически: привязку к health/min-damage/UI HC должен завершить main.

Все адреса — **RVA = file offset**, VA = `0x212c3300000 + RVA`; dump size `0x5001000`. Только статическое чтение, без исполнения sample. Запись исключительно в `analysis/phase2/spread/`. SQLite read-only/immutable; original и Ghidra project не изменялись. `0x528b90` не разбирался; penetration/damage `0x51dac0` и нижние helpers оставлены main.

## 1. `0x525830`: точные выходные формулы

Контракт: `aggregate(context, out_float4, candidate)`; `source = *(candidate+0x18)`, `target = *(candidate+0x10)`. Используется scale `source+0x1c`, samples pointer `*(source+0x20)` (`0x52589e–0x5258a2`, `0x525911`). Это cache кольцевой таблицы, подготовленный `0x5239d0`.

Ненулевой scale: `0x525936 → 0x51cf40(frame,candidate,1.0)`; `0x525993` ставит vtable **`0xee1968`**, `0x5259a2` — **job.count=0x40**, `0x5259fb` — dispatch через virtual `+0x98`. Точная связь callback: **qword `[0xee1970] = 0x212c381d320`**, то есть vtable slot `+8`. Не `0xe1968`.

Для конечных обычных scores, `thresholdA=*(float*)(target+0x18)`, `thresholdB=*(float*)(target+0x14)`, `specialEnabled=(*(int*)(*(target+8)+0x50)==0)`:

| Float output | Подтверждённая формула при ненулевом scale | ASM evidence |
|---|---|---|
| `out[0]` | `popcount(maskA)/64`; maskA устанавливается для валидного результата со score ≥ thresholdA | `0x525a01/0x525a11`, `0x525b6b`, store `0x52613c` |
| `out[1]` | `count(score[index] ≥ thresholdB)/64`, по всем 64 slots, включая нулевые | SIMD comparisons `0x525a2f–0x525b5a`, ×1/64 `0x525b73`, store `0x526140` |
| `out[2]` | `specialEnabled ? popcount(maskB)/64 : 0` | `0x525b77–0x525b7b`, reduction `0x525ffc`, store `0x526214` |
| `out[3]` | `sum(score[0…63])/64` | сумма `0x526004–0x52620b`, ×1/64 `0x526219`, store `0x526221` |

**Перенос в candidate доказан:** caller `0x524a30`, call `0x524d66`, copy float4 `0x524d77` → **candidate `+0x44`, `+0x48`, `+0x4c`, `+0x50`** соответственно. Это три нормированные доли и средний score, не четыре взаимозаменяемых «HC».

При `source.scale==0` job не запускается: `out=(1, candidate.score≥thresholdB, special predicate, candidate.score)`, где `candidate.score=*(float*)(candidate+0xc)`. Evidence `0x5258b5–0x525909`. Special predicate дополнительно проверяет `(byte(*(candidate+0x20)+0x39)&0xfe)==2`.

Сверка: `../conditional_bullet/212c3825830.c:127–151, 272–282, 448–483`; первичный ASM — `00525830.asm`.

## 2. Общий frame и callback

**`0x51cf40(frame,candidate,factor)`**: строит `forward=normalize(candidate.xyz−source.xyz)`; две поперечные basis-вектора, умноженные на factor. Поля frame: `+0x00` candidate pointer, `+0x08` forward Vec3, `+0x14` basis1 Vec3, `+0x20` basis2 Vec3; `+0x2c` counter, `+0x30/+0x34` maskA, `+0x38/+0x3c` maskB, `+0x40…+0x13f` score[64]. Counter/masks/scores обнуляются `0x51d071–0x51d0b3`. Есть fallback нормализации и почти вертикального направления; не заменять его одним безусловным cross-product.

**`0x51d320`** получает sample index через `lock xadd` (`0x51d396`, `0x51d3e3`), ограничивает его job.count и вычисляет:

```text
direction = normalize(forward + sample.x*basis1 + sample.y*basis2)
endpoint  = source.xyz + context.float_at_0x100 * direction
```

Пары читаются `0x51d400/0x51d40b`; построение/нормализация `0x51d406–0x51d4d0`; endpoint `0x51d4d4–0x51d4ff`; scalar evaluator **`0x51dac0` по `0x51d558`** — граница main. Только при положительном результате и ненулевом result-record записывается score; при `context.int_at_0xfc>1` он дополнительно умножается на это целое (`0x51d575–0x51d606`). Остальные slots сохраняют 0.

MaskA: score≥target+0x18, atomic OR `0x51d5b5`. MaskB: specialEnabled и `(result_record.byte_at_0x39 & 0xfe)==2`, atomic OR `0x51d5f3`. Подробности damage не дублировались. Сверка: `../conditional_trace/212c381d320.c:148–175`, `212c381cf40.c`; ASM `0051d320.asm`, `0051cf40.asm`.

## 3. Fast counterpart `0x526290`: 8×8, не просто 8 samples

Берёт тот же `source+0x20` cache (`0x5263df`). Начальный block index **7** (`0x526720`), убывает до 0 (`0x526760–0x52677b`); offset=`block<<6`; две 32-byte AVX loads (`0x52678f/0x526794`) читают **8 Vec2**. Итого без раннего выхода **8 blocks × 8 = 64 samples**. Source C: `../conditional_trace/212c3826290.c:574–609, 2173–2186`.

Fast строит и оценивает направления локально, не dispatch-ит callback `0x51d320`; низкоуровневую геометрию/score здесь не переименовываю. Для valid fast-results накапливает `countA`, `countB`, category count и сумму scores; результат — `(countA, countB, specialEnabled ? categoryCount : 0, sumScore)/64`, с поправками early-fill ниже. При полном проходе есть важное отличие: **fast `out[1]` считает score > thresholdB, тогда как полный `0x525830` использует ≥**. Для finite values это подтверждено `seta` после сравнения (`0x528179–0x52818a`, аналогично `0x5286b8–0x5286c3`). `out[0]` использует ≥thresholdA. Special-category count гейтится specialEnabled. Выходные divisors всегда **64**, не число реально просмотренных samples: `0x528769–0x5287a2`; vector constant `0xe9bb20` начинается `(1/64,1/64)`.

**Early-fill:** после полного блока возможен выход, если все 8 samples этого блока приняты по thresholdA и count(score>thresholdB) в блоке равен **0 или 8** (`0x5286ef–0x5286fd`). Для оставшихся `remaining=8·block_index` направлений:

```text
countA += remaining
countB += remaining, только если block.countB == 8
countSpecial += remaining, только если block.countSpecial == 8
sumScore += remaining * meanScoreOfCurrent8
```

Evidence `0x528703–0x52874d`; block mean строится через ×0.125. Затем деление на 64. Поэтому реально обработано **8,16,…,64** направлений; итог после early-fill — **экстраполяционная оценка**, не эмпирический подсчёт всех 64. Zero-scale branch совпадает с shortcut полного агрегатора. Математика damage/penetration намеренно за scope.

## 4. Tables и preinit: проверено по original bytes

Producer **`0x512710`** строит две независимые deterministic Vec2[64]:

| Original RVA range | Математическая форма | Instruction anchors |
|---|---|---|
| `[0x1754680,0x1754880)` | `radius=ring/8`, `angle=ring·π/8+sector·π/4`, ring=1…8, sector=0…7; `radius*(cos,sin)` | `0x512b42`, `0x512b84/0x512b8c`, `0x512ba4/0x512bb1`, `0x512d10–0x512d1d` |
| `[0x17548c0,0x1754ac0)` | `radius=sqrt((sample+1)/64)`, `angle=sample·2.399963140487671`, sample=0…63; `radius*(cos,sin)` | `0x512d40–0x512d54`, `0x512d5c/0x512d69`, `0x512d72–0x512d84` |

Первую таблицу `0x5239d0` масштабирует source-record float `+0x1c` в cache stride `0x220`: 64 Vec2 в `[+0,+0x200)`, scale key `+0x200`; cache pointer записывает в source `+0x20` (`0x5241bc`, `0x52419c`, `0x5242b7–0x5244b5`). Это producer для `0x525830`/`0x526290`.

Вторую таблицу `0x524a30` масштабирует в context `+0x140…+0x33f`; **`0x51c9c0` использует её для coverage/point optimization**, с тем же callback, но другим набором samples и возможным factor. Он может вернуть true при 64/64 scores≥thresholdB; иначе меняет candidate.xyz по score-взвешенному направлению (`0x51cc3b–0x51cc4d`, `0x51ce50–0x51cecd`). Не смешивать этот optimizer с четырьмя агрегатами `0x525830`.

**Preinit check:** в исходном dump обе области целиком **512/512 zero bytes**. Значит producer и зависимости доказаны статически, но факт выполненной инициализации не доказан. `formula_samples.tsv` — независимая иллюстрация формул, **не извлечённые runtime samples** и не bit-exact исполнение. Числа 0.125, 1/64, golden angle подтверждены bytes по `0xe99c00`, `0xe98ac0`, `0xe9b978`.

## 5. RNG, достоверность, артефакты

- ran1 literal scan: `127773` отсутствует; LE32 `16807` только по `0x12e6d3f`, `0x2006335`, вне индексированных function ranges. Связка recurrence+32-entry shuffle table не подтверждена. Есть CRT-like LCG `0xd99b24` и byte-generators `0xc6ff30/0xc70088`, без установленной связи с sampling. Внешний tier0 RNG этим не исключён.
- `0x515ca0` не приоритетный spread-kernel: изученный `RandomFloat` (`0x51794f`) даёт половину симметричной угловой поправки; результат нормализуется через 360 и пишется в `this+0x1d0` (`0x5179a1–0x5179c7`).
- Conditional C — дополнительное evidence с допущениями PEBhash=0x5877/KUSERsum=0x92fb254d; итоговая модель содержит 3089 условных branch-преобразований. SHA-256 модели: `bd4b358c86a23027dc72de102790018803d1dbe9aabfcbe789196f826d6f1f39`. Это **не runtime verification**. Dynamic-offset globals с `0x13371337` и отсутствие schema names не позволяют присвоить точные engine field names.
- Собственные NOP-подмены только в памяти, только `0f1a2410/0f1b2410/0f1c2410→90909090`; offsets и unresolved CFG edges сохранены. Sampling core `525830/51cf40/51d320/526290/512710` полностью декодируется в original index. Нет исполнения sample/эмуляции или изменений Ghidra.
- `evidence.tsv`, `targets.tsv`, `math_constants.tsv`, `table_refs.tsv`, `evidence_audit.json`; ASM по RVA рядом. Скрипты `inspect_spread.py`, `cfg_spread.py`, `table_refs.py`, `export_evidence.py`. Запуск: `PYTHONDONTWRITEBYTECODE=1 analysis/venv/bin/python analysis/phase2/spread/export_evidence.py`. Source hashes повторно проверены: original dump и code.sqlite **OK**.

**Дальше нужны не новые RNG-кандидаты, а main-привязка thresholdA/B, категории maskB и финальных comparisons candidate+0x44…+0x50. Дополнительных RVA для завершения этого sidecar не требуется.**


---
