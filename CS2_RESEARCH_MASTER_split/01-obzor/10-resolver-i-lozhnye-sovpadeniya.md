<!-- split-part | CS2_RESEARCH_MASTER.md lines 178-193 | body-sha256 e470cc10ab8104ea00a6cd94f9f1427228de378472dca3bab10240a599d16ece -->
[← все части](../README.md) · [индекс обзора](00-index.md)

<!-- split-body-start -->

## 10. Resolver и ложные совпадения

`0x51B480` использует историю углов и вычитает нормализованную поправку из this+0x1D0. `0x515CA0` содержит RandomFloat-поправку угла. Ни один факт отдельно не доказывает полноценную resolver state machine. Её реализации в компилируемом API нет.

Отброшенные или переатрибутированные направления:

- `spread` и функции `0xC421C0/0xC422B0` — FreeType SDF properties, не weapon spread.
- `0x2E9B50` — пространственный эффект с endpoints, RGB/alpha×255, lifetime/resources.
- `0x422970`, `0x406C10`, `0x71EBE0` — UI selection/randomization/transitions.
- `0x54BC60` — shot-result diagnostics с дополнительными state writes; не объявлен pure UI.
- `0x4FD710` — строковый selector; исходная C-декомпиляция существенно неточна около memcpy-like helper, ASM сохранён.
- `records` внутри DXBC metadata — shader buffers; protobuf `input_history/random_seed` не доказывают алгоритм.
- Wrapper `0x4FF1690` вызывает два init helper при reason==1 и возвращает1; логика их тел этим не восстанавливается.

Полный каталог строк, RTTI, protobuf evidence и предоставленных 391 imports включён ниже. Имена imports предоставлены вместе с dump, а не независимо верифицированы против отсутствующих exports внешних DLL.
