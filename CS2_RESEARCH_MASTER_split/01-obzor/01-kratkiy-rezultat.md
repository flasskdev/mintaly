<!-- split-part | CS2_RESEARCH_MASTER.md lines 16-30 | body-sha256 c07be4b29575ce1b68f4d1b12be3335199cbb5024fb0b8101fa4c6661b757bb6 -->
[← все части](../README.md) · [индекс обзора](00-index.md)

<!-- split-body-start -->

## 1. Краткий актуальный результат

| Подсистема | Что реально восстановлено | Граница результата |
|---|---|---|
| Aim/fire | Финализация выбранного aim-решения, углы, временные пары, history/subtick records, атака | Не вся политика выбора цели и не все режимы GUI |
| Hitchance | Детерминированные 64 направления, basis, worker, 4 агрегата, zero-scale ветвь | Не точная реализация native weapon PRNG и не полный runtime spread |
| Fast hitchance | 8×8 directions, reverse blocks, early-fill, собственные сравнения | Не гарантированно эквивалентен полному kernel |
| Penetration | Собственный внешний контур, hit-volume test, attenuation, обработка списка сегментов | Внешнее тело расчёта потерь не находится в дампе |
| History/lag | Ring16 producer, admission, aging, discontinuity, копирование и отдельные callers | Полный rewind/apply/restore и engine integration не восстановлены |
| Melee | Damage/mode selection, front/back проверка, probes, candidate append | External trace и полный runtime state отсутствуют |
| Resolver | Отдельные angle/history consumers исследованы | Завершённый resolver не доказан и не реализован |
| Прочее | Point optimization, category multipliers, minimum-damage/health budget, UI/effect RNG triage | Не полный каталог функций продукта |

Скомпилировать приложенный C++ можно как **независимую математическую библиотеку**. Это не означает, что весь Ghidra-псевдокод стал корректным C++, что восстановлена игровая DLL или что появились отсутствующие trace/resolver реализации.
