<!-- split-part | CS2_RESEARCH_MASTER.md lines 172-177 | body-sha256 0da57ad36ba1cb52b342f9f5ab1518d75d3c93932e4cb1e5bc44f67f9e1ba52c -->
[← все части](../README.md) · [индекс обзора](00-index.md)

<!-- split-body-start -->

## 9. Melee/knifebot

`0x528B90` после уточнения CFG идентифицирован как melee/knifebot-подобный evaluator, а не bullet autowall. Константы: primary25/40/90, secondary65/180, scale1/0.825, front/back dot0.475, traces66/50, timing0.4. Важны именно 0.825 и 66/50: типовые значения другого кода не подставляются.

После mode/lethality/config checks идут вертикальные probes, ray/hull calls и append через `0x52AD40`. Melee candidate stride0x30: XYZ, integer score+0x0C, lethal byte+0x10, mode+0x11, pointers+0x18/+0x20/+0x28. Bullet candidate stride0x58 имеет другие pointer/aggregate fields. Наблюдаемые layouts в `.h` описывают bytes/addresses, не реальные SDK pointer types.
