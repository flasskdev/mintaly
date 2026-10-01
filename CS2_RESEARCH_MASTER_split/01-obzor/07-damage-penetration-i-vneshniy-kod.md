<!-- split-part | CS2_RESEARCH_MASTER.md lines 144-159 | body-sha256 c3c784a3156a63c8600ef9878a52d6774b1ae2a3782b7ba16b8f20bc0a0a274d -->
[← все части](../README.md) · [индекс обзора](00-index.md)

<!-- split-body-start -->

## 7. Damage/penetration и отсутствующий внешний код

`0x51DAC0` нормализует направление, растягивает на range, проверяет локальные prepared hit volumes через `0x2F19C0`, проверяет category mask, оценивает attenuation и вызывает `0x2F2110 → 0x2F0DD0`.

Для обычных положительных конечных параметров математический смысл attenuation — `range_parameter ** (distance/500)`, через log2f/exp2f-like kernels. Ветка item-like field ==0x1F обрабатывает возвращённый score иначе; номер не превращается автоматически в имя оружия.

`0x2F0DD0` перебирает segment list stride0x18, обновляет damage-state внешним вызовом, накапливает потери по flags, останавливается по return/minimum damage, сопоставляет object/target и возвращает pointer pair. Ключевые внешние VA:

- `0x7FFCEF4A8DB0`: обработка penetration segment.
- `0x7FFCEF4A3E50`: построение trace workspace.
- `0x7FFCEF48E030`: дополнительная обработка изменённого списка.
- `0x7FFCF0FE45A0`: внешний trace object/interface.
- `0x7FFCEF616040`: вызываемый через `0x1463A0` ray/hull adapter.

Все они вне приложенного image. Их тела, world/material state и module mapping не появляются от объединения файлов. C++ не содержит фальшивых успешных реализаций этих функций. `0x7BDB90` и начало `0x51E4F0` показывают category multipliers, armor-like conditions и внешние scalar settings; исходные значения settings отсутствуют.
