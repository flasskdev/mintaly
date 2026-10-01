<!-- split-part | CS2_RESEARCH_MASTER.md lines 70-101 | body-sha256 c9afb4864d32cb695e283dea4ac19a1d490e0c6f01487aec6b70b7d269fd2d26 -->
[← все части](../README.md) · [индекс обзора](00-index.md)

<!-- split-body-start -->

## 4. Aim/fire — применение решения, не выбор лучшей цели

RVA `0x50FB10` получает уже выбранный candidate из caller `0x505F50`; call site `0x506787`. В caller имеется массив stride `0x30`, сравнение score/flags и передача выбранной записи. После второго этапа этот размер записи нельзя смешивать с bullet candidate stride `0x58`.

В `0x50FB10`:

- `0x511D50` копирует snapshot command, включая массивы `0x48` и `0xAC`.
- Нормализуются tick/fraction, выбирается допустимая временная пара; в одном пути окно относительно T−3…T+1.
- При отсутствии готовых углов вычисляются `yaw=atan2(dy,dx)`, `pitch=atan2(-dz,sqrt(dx²+dy²))`, затем перевод в degrees и wrap через 360.
- Два вызова `0x3375A0` дают временно согласованные угловые поправки; их сумма используется в отдельных mode branches.
- Формируется history entry stride `0xAC`: углы, временные пары, presence flags.
- Опционально `0x309500` заменяет временную пару на основании отдельного ring32.
- Кнопка primary/secondary выбирается масками `1`/`0x800`; есть работа с press-records stride `0x48` и лимитом 32.

RandomFloat здесь применяется **к времени**:

```text
upper = min(weapon_related_value, 0.1)
lower = max(upper - 0.05, 0)
delta_ticks = RandomFloat(lower, upper) * 64
```

Число 64 в этом месте — ticks per second, не число spread samples. Это важный отвергнутый ложный путь поиска hitchance.

### Copy-helper `0x511D50`

Копируются `[0,0x5C)`, vector-triple at +0x60 с элементами `0x48`, vector-triple at +0x78 с элементами `0xAC`, хвост `[0x90,0xBA)`. Destination flags +0xB6/+0xB7/+0xB8 сохраняются и восстанавливаются. Неизвестные поля не названы произвольно SDK-структурой.

### Угловой прогноз `0x3375A0`

Это cached temporal angular-state decay, а не простой Vec3 lerp. Частота samples 128 Hz, до 129 точек с нулевой; коэффициент `0.939413070679`, linear decay `18/128`, trapezoidal velocity contribution `1/256`, squared small threshold `1/1024`. Между samples есть Euler→quaternion, sign alignment, полиномиально скорректированная интерполяция, normalization и обратный перевод. Точное название recoil/punch и producers двух состояний не доказаны. Полный исходный C-листинг сохранён в CPP-архиве; эта функция не заменена упрощённым новым lerp.
