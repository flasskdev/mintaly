<!-- split-part | CS2_RESEARCH_MASTER.md lines 160-171 | body-sha256 2320373a5ce9d7b810aaa44a7ea2373ea86a9587280c183d4e0f0a13e02c1fef -->
[← все части](../README.md) · [индекс обзора](00-index.md)

<!-- split-body-start -->

## 8. История и lag-related логика

Основной producer **`0x4721D0`**: ring capacity16, slots=object+0xF0, stride0x500, count+0x50F0, head+0x50F4. Новый head=(head+15)%16 при нормальных неотрицательных индексе/count. Signed remainder в оригинале не заменяется общей unsigned-маской для повреждённых входов.

Admission сравнивает новый `int(source_time*64+0.5)` с newest tick; fraction здесь не сравнивается. Aging считает cutoff с runtime window и проходит oldest tail. **All-stale ветвь конкретного блока не записывает count=0**. Позднейшие ветви могут изменить count отдельно. Эта особенность сохранена в математической модели.

Discontinuity reset count=1, если distanceSquared > 4096·clamp(deltaTick,1,5) либо меняется размер payload collection. Это не `(64*deltaTick)²`. Равенство порогу само по себе не сбрасывает историю.

`0x474D20` создаёт отдельную derived/scratch запись на основе newest, не обновляя ring head/count. `0x475830` копирует owned buffers отдельно — record нельзя слепо считать POD. Orchestration `0x6AC690`, дополнительный caller `0x661010`, worker `0x475EF0` и найденные reset/move paths раскрыты в специализированном отчёте.

Reader `0x52B250` выбирает до двух близких records: squared-distance bound62500, integer-truncated absolute milliseconds<=200, дополнительные runtime-window gates, directional split0.475. Это policy конкретного selector, не общая спецификация всех hitscan/backtrack режимов. Отдельный ring32 в `0x309500` обслуживает временные пары и не сливается с ring16.
