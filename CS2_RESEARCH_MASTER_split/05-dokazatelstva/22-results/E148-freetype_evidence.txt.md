<!-- split-part | CS2_RESEARCH_MASTER.md lines 76433-76453 | body-sha256 7c81d25ad012cb0f42adfd3a38b3d5144e4ab4a54828bdd066dfb6edf1666639 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-148"></a>

## E148. `analysis/results/freetype_evidence.txt`

Bytes: 1266. SHA-256: `2e5de075212b01e252bc2520e16d5b6fb5d14625c7e6d328fa58f6edcbc11f1c`.

```text
Статическое сопоставление библиотечного кода

Источник для сравнения: https://raw.githubusercontent.com/freetype/freetype/master/src/sdf/ftsdfrend.c
Проверено чтением public source, 2026-09-30. Код источника не запускался.

RVA 0xc421c0: setter свойства spread в поле +0x78 с диапазоном 2..32; flip_sign в +0x7c; flip_y в +0x7d; overlaps в +0x7e. Неизвестное свойство возвращает 0x0c, недопустимый spread — 6.
RVA 0xc422b0: соответствующий getter тех же четырёх свойств.
Строки в dump: spread at 0xf58ec8, flip_sign at 0xf6a3a0, flip_y at 0xf58008, overlaps at 0xf62418.
Порядок условий, семантика и типы значений соответствуют sdf_property_set / sdf_property_get FreeType.
Следовательно, найденный bare spread относится к SDF-рендерингу шрифтов, не является доказательством расчёта разброса оружия.
Точная версия FreeType в dump не установлена этим сопоставлением.
```
