# Ragebot: staged pipeline (перевод на схему из skeet)

Дата: 29.09.2026. Основание: `docs/RAGEBOT.ru.md` (спека стадий и бюджетов) и
`docs/skeet_rage_reference/` (декомпил payload-модуля skeet, 980 функций).

Задача была: **не копировать декомпил, а переделать наш ragebot на ту же схему** —
стадии, лимиты, бюджеты и порядок проверок. Сделано на нативном C++, без Lua VM.

## Что было до

`rage::on_create_move` → `run_gun` → `gather_candidates` (без лимита числа целей)
→ `scan_players` (все записи всех целей, мультипоинты для всех)
→ `select_best` (hitchance считался для всех кандидатов) → `fire_gun`.
Лимитов на цели/записи/точки/трассы/hitchance-запросы не было.

## Что стало

Пайплайн разделён на четыре поименованные стадии, как в спеке
(`create_move → targets → scan → select → fire`):

| Стадия | Метод | Что делает |
|---|---|---|
| targets | `rage::build_targets` | сбор целей с фильтрами (жив, враг, HP, immunity, записи), порядок «в FOV → ближе → меньше HP», обрезка до 13 |
| scan | `rage::run_scan_pass` | один таргет + одна запись + точки: FOV-гейт, `trace_budget`, `centers_only`, пробой, сбор хитов |
| select | `rage::run_select_pass` → `select_best` | дешёвый скор → top-K по записи → hitchance-волны → скор и выбор |
| fire | `rage::run_fire_pass` | перепроверка `required_hitchance`/`allow_force` на выбранном хите и выстрел |

`scan_players` остался параллельным драйвером батча: он готовит точки
(`prepare_scan`) и раздаёт по хитбоксам в threadpool. Логика одной точки вынесена
в общий `rage::execute_scan_point`, поэтому последовательный проход
(`run_scan_pass`) и батч используют **один и тот же** код проверки и сборки хита.

Дополнительно: команда с одной целью (дуэль — частый случай) идёт по
последовательному стадийному проходу без барьера threadpool, с тем же порядком
точек и тем же бюджетом.

## Лимиты (совпадают со спекой и декомпилом)

| Константа | Значение | Источник |
|---|---|---|
| `k_max_targets` | 13 | спека: «до 13 целей» |
| `k_detailed_targets` | 2 | спека: «2 подробных цели» |
| `k_max_scan_records` | 4 | спека: «4 записей на цель» |
| `k_penetration_budget` | 160 | спека: «общий бюджет penetration 160» |
| `k_max_scan_points` | 128 | спека: `rage_scan` — максимум 128 точек |
| `k_max_hitchance_queries` | 32 | спека/декомпил: «at most 32 hitchance queries per command» |

Реализация ограничений:

- **Цели**: `build_targets` сортирует (в FOV → дистанция → HP) и обрезает до 13;
  из них первые `detailed_targets` (настройка, 1..2) сканируются подробно
  (мультипоинты + все записи), остальные — только центры и только новейшая запись.
- **Записи**: `lagcomp::get_scan_records` отдаёт до 4 записей на цель
  (новейшая, до двух равномерных средних, старейшая) вместо прежних двух.
- **Точки**: `prepare_scan` не публикует больше 128 точек на таргет/запись.
- **Трассы**: счётчик `m_penetration_used` общий на команду, захват слота через
  CAS-цикл — параллельные воркеры не могут перебрать бюджет. Плюс отдельный
  `trace_budget` на проход (`rage_scan.trace_budget`).
- **Hitchance**: `m_hitchance_used` общий на команду; волны обсчёта обрезаются по
  32 запросам. Кандидаты идут в порядке дешёвого скора, поэтому порог тратится на
  лучших, а не на случайных.

## Новые настройки

`settings::combat::ragebot::weapon_group` (на группу и на оружие):

- `trace budget` (16..160, по умолчанию 160) — `trace_budget`;
- `detailed targets` (1..2, по умолчанию 2) — `detailed_targets`;
- `centers only scan` (выкл.) — `centers_only`;
- `prefer visible targets` (вкл.) — `prefer_visible`.

Все поля зарегистрированы (`reg`), имеют категорию для биндов, участвуют в
`copy_values_from` и выведены в меню Ragebot.

## Что перенесено из декомпила «идейно», без копипаста

`rage_decompiled` — это `FUN_xxx` без типов и структур. Поэтому взяты только
проверяемые вещи:

- контекст `rage_scan` (один таргет + одна запись + eye, `prediction`,
  `centers_only`, `trace_budget`, `hitboxes`, `points`) — из стека
  `FUN_180044dc0`;
- контекст `rage_select` (`required_hitchance`, `allow_force`, `no_spread`) — из
  спек-описания `rage_select` и `FUN_180041210`;
- контекст `rage_fire` (один хит + `hitchance`, `required_hitchance`, `forced`) —
  `{cancel=...}` в спеке, `FUN_1800393e0/1800392f0` в декомпиле;
- лимит 32 query (`1800D1710`, `1800D5530`: строки `at most 32 hitchance queries per command`);
- бюджеты 13/2/4/160 (README спеки, `180044dc0`).

Не копировались: Lua-диспетчер стадий, `claim_command`, эвристики сэмплера
`0.9/0.35/0.65` (их проверяем тестом перед включением), сырой псевдокод Ghidra.

## Проверка

```powershell
# Синтаксис затронутых TU (без линковки):
cmd /c scratch\syntax_rage.bat

# Тесты, включая новые проверки стадий и бюджетов:
py -3 project\tests\config_hit_jumpbug_checks.py
```

`rage.cpp`, `shared.cpp`, `menu.ragebot.cpp`, `entry.cpp` собираются без ошибок.
Тесты `test_staged_pipeline_budgets_match_spec_limits`,
`test_every_rage_numeric_and_hitbox_field_is_registered`,
`test_every_rage_boolean_bind_has_a_per_weapon_category` проходят.

## Остаточные риски

- Полный билд (`msbuild mintaly-cs2.vcxproj`) в рамках сессии не прогонялся:
  сборка идёт дольше лимита командной сессии. Проверен `cl /Zs` по всем
  затронутым TU.
- Поведение в игре не измерялось: лимиты меняют число обрабатываемых целей и
  количество hitchance-запросов, поэтому скейлы и выдачу нужно перепроверить
  на реальном сервере.
- `centers_only` и `detailed_targets` по умолчанию повторяют прежнее поведение
  для 1v1 и слегка экономят работу в массовых ситуациях; при агрессивных
  настройках (2/128/160) поведение близко к прежнему.
