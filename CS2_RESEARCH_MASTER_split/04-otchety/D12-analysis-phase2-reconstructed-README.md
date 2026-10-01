<!-- split-part | CS2_RESEARCH_MASTER.md lines 3067-3104 | body-sha256 4f42111d7b96ec538d87fcd6b55dd0d030f5680b6409feda105be0f0ba8c301f -->
[← все части](../README.md) · [индекс отчётов](00-index.md)

<!-- split-body-start -->

<a id="doc-12"></a>

# Приложение D12. `analysis/phase2/reconstructed/README.md`

**Дословный исходный отчёт.** Даты/статусы/неизвестные роли внутри относятся к своему этапу. При расхождении использовать актуальный свод и поздние доказательства. Пути внутри текста — исторические: соответствующие материалы встроены в MD/CPP и находятся через поиск исходного пути.

# Читаемая математическая реконструкция — НЕ полный ragebot

`recovered_math.py` — написанная заново независимая модель подтверждённых формул. Входные sample scores, valid flags и category values передаются снаружи: здесь нет engine trace, injection, runtime offsets, resolver или тела внешнего penetration. Все функции используют обычную Python arithmetic и покрывают нормальные конечные входы, не bit-exact SIMD/CRT/float32 edge cases оригинала.

| Функция модели | Основание RVA |
|---|---|
| `ring_samples`, `golden_samples` | `0x512710` |
| `aggregate_full`, `zero_scale_aggregate` | `0x525830`, worker `0x51D320` |
| `aggregate_fast` | `0x526290`, reverse blocks 7…0, early-fill |
| `estimated_health`, `minimum_damage_threshold` | начало `0x51E4F0` |
| `distance_attenuation` | `0x5239D0`, `0x51DAC0`, log2f/exp2f-like kernels |
| `retained_count_after_tail_aging`, `resets_history` | `0x4721D0` |

`aggregate_full` принимает уже вычисленные scores, после возможного multiplier context+0xFC. Невалидные outcomes превращаются в 0 как оставленные worker-ом пустые slots. ThresholdA/B обязаны быть положительными и конечными. На исходные неинициализированные указатели модель не опирается. Fast-версия воспроизводит именно арифметику агрегации предоставленных результатов, не подменяет отдельную геометрию её native AVX evaluator.

Обе sampling tables в исходном pre-entry BIN нулевые. Модель вычисляет иллюстрацию producer-формул, а не экспорт готового runtime состояния.

ThresholdB — health-like бюджет: integer entity field за placeholder-offset преобразуется в float, затем для совпадающих target IDs вычитается weight*damage только при weight>0.75. `estimated_health` получает уже отфильтрованные по ID пары. ThresholdA берётся из context+0x88; при setting<=100 ограничивается health, иначе заменяется health или ceil(health/2) по context+0x50; результат ограничен сверху 130. Названия health/minimum damage следуют data-flow и сравнению с damage, но конкретный SDK field name/UI label не восстановлены.

`retained_count_after_tail_aging` намеренно сохраняет count, если все записи stale: это подтверждённая особенность конкретного блока оригинала, а не рекомендуемая реализация кольца. Последующие producer-ветви могут изменить count снова.

Проверка собственной модели:

```sh
analysis/venv/bin/python analysis/phase2/reconstructed/check_math.py
```

`math_check_results.json` — проверки математических граничных случаев. Они **не означают**, что исходный sample был исполнен, что восстановлены все режимы или что доказана поведенческая эквивалентность с оригиналом.


---
