<!-- split-part | CS2_RESEARCH_MASTER.md lines 102-126 | body-sha256 8b56c8cd11b8efdae03ad88645e0e68b786217a8afd4d6b2d4b283f24166d7f2 -->
[← все части](../README.md) · [индекс обзора](00-index.md)

<!-- split-body-start -->

## 5. Hitchance и генераторы направлений

Цепочка: `0x512710 → 0x5239D0 → 0x525830 → 0x51CF40 → 0x51D320 → 0x51DAC0`.

Первый deterministic Vec2[64]: 8 rings × 8 sectors, radius=ring/8, angle=ring·π/8+sector·π/4. Второй Vec2[64] для point optimization: radius=sqrt((index+1)/64), angle=index·2.399963140487671. Оба исходных массива в pre-entry dump ещё целиком нулевые по 512 bytes. Формулы восстановлены из producer, готовые runtime samples не выдаются за извлечённые данные.

Worker `0x51D320` через atomic index распределяет samples, строит нормализованное `forward + sample.x*basis1 + sample.y*basis2`, вызывает scalar score evaluator, записывает valid scores и masks. Не все случайные функции в образе относятся к этому worker; native ran1/weapon PRNG не подтверждён.

Полный kernel `0x525830` возвращает четыре float:

| Поле в bullet candidate | Значение |
|---|---|
| +0x44 | Доля валидных samples, достигших minimum score |
| +0x48 | Доля scores **≥** health-like budget |
| +0x4C | Доля разрешённой special category `(category & 0xFE)==2` |
| +0x50 | Средний score по 64 slots |

При нулевом scale sampling не запускается; output=(1, centralScore≥thresholdB, specialPredicate, centralScore). Значение 1 в первой компоненте этого shortcut нельзя переносить на произвольный failed trace.

### Fast kernel `0x526290`

Обход 8 блоков по 8 samples, block index 7…0. При однородном успешном блоке оставшиеся counts и sum экстраполируются из текущих восьми. Фактически могут быть оценены 8,16,…,64 направления, но divisor всегда 64. Fast thresholdB использует **`>`**, полный kernel — **`≥`**. Эти пути не объединяются в одну функцию с одинаковым сравнением.

C++-модель воспроизводит эту арифметику на переданных outcomes. Она не создаёт world trace, не подделывает valid flags и не реализует неизвестные weapon-specific RNG branches. Python double/C++ double не считаются bit-exact заменой native float32/AVX/CRT.
