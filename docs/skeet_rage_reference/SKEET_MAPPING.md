# Skeet rage reference — карта соответствия и статус переноса

Источник: `C:\Users\stass\Downloads\AyuGram Desktop\skeet_29_09_26\skeet 29.09.26\rage_decompiled`
(980 функций Ghidra, payload `skeet_payload.dll` извлечён из `.rsrc` `skeet.exe`).

Назначение в проекте: `project/docs/skeet_rage_reference/` — референс для чтения
(декомпил Ghidra, имена `FUN_xxx`, типы не восстановлены). Не компилируется, в билд не входит.
Бинарник `skeet_payload.dll` (4.7 МБ) в проект НЕ переносим — только `.txt`/`.tsv`/`.java`/`.md`.

**Статус: схема перенесена в код** — см. `docs/ragebot-staged-pipeline-ru.md`
(стадии targets/scan/select/fire, бюджеты 13/2/4/160/128/32, новые настройки).

## Что перенесено в папку

- `autowall_damage/` — 35 файлов (ядро скана `180044DC0`, сэмплер `1800487A0`, возврат урона в Lua `180049950`)
- `hitbox_backtrack/` — 18 файлов (multipoints `1800D2FD0`, `max backtrack ticks` `180001180`)
- `hitchance/` — 135 файлов (ядро `180041210`, лимит `at most 32 hitchance queries per command`)
- `nospread/` — 228 файлов (`get_spread`/`no_spread`/`weapon_calculate_spread`, `prediction_seed`)
- `pipeline/` — 357 файлов (`create_move` `180087F70`, драйвер стадий `180038870`, ядро отбора `180039560`)
- `trace_geometry/` — 207 файлов (`trace_ray/hull/entity`, `calc_angle/aim_punch/can_shoot`)
- `INDEX.tsv` (ea,name,file,category,depth,via,size), `SUMMARY.txt`, `README.txt`, `RageDump.java`
- Итого: 984 файла, ~3.8 МБ (без DLL). Проверка: `Get-ChildItem -Recurse -File | Measure-Object`.

## Карта: skeet -> mintaly-cs2 (project/core/features/combat)

| Skeet (декомпил) | Mintaly аналог | Статус |
|---|---|---|
| `pipeline/180087F70` create_move `rage begin/end` | `impl/rage.cpp: rage::on_create_move` | совпадает: логирование стадии, контекст, запуск стадий |
| `pipeline/180039560` ядро: targets/records/fov/min_damage/allow_fire | `build_targets` + `run_scan_pass` + `run_select_pass` | ПЕРЕДЕЛАНО: разделено на стадии, 13 целей / 2 подробных / 4 записи |
| `pipeline/180038870` драйвер стадий | `run_gun` (фазы gather/eye_scan/selection/movement) | ПЕРЕДЕЛАНО: стадии вызываются явно, бюджеты общие |
| `hitchance/180041210` (no_spread, required_hitchance, allow_force, симуляция) | `run_select_pass`/`select_best` + `aim_context.required_hitchance/no_spread/allow_force` | ПЕРЕДЕЛАНО: порог и force приходят из стадийного контекста, лимит 32 запроса |
| `hitchance/1800D1710`, `1800D5530` («at most 32 hitchance queries per command») | `m_hitchance_used` + `k_max_hitchance_queries` | ПЕРЕДЕЛАНО: волны обрезаются по 32 |
| `autowall_damage/180044DC0` (points<=128, trace_budget, prediction, centers_only, hitboxes) | `rage_scan` + `run_scan_pass` + `execute_scan_point` | ПЕРЕДЕЛАНО: 128 точек, `trace_budget`, `centers_only`, общий бюджет 160 |
| `autowall_damage/1800487A0` сэмплирование (clamp hitchance/100, эвристики 0.9/0.35/0.65) | `shared::penetration::run` + `calculate_hitchance` | НЕ ПЕРЕНОСИЛОСЬ: эвристику включать только после теста |
| `autowall_damage/180049950` возврат `{damage,hitbox,hitgroup,penetrated}` | `scan_hit` (`position/angles/damage/fov/hitbox/requested_hitbox/hitgroup/center/penetrated/eye/record`) | ПЕРЕДЕЛАНО: `requested_hitbox` добавлен, набор полей совпадает со спекой |
| `nospread/*` (`weapon_calculate_spread`, `prediction_seed=180163290`) | `spread_cache`, `get_spread/get_inaccuracy`, `ballistics.hpp: solve_spread` | совпадает по смыслу, свой решатель сохранён |
| `trace_geometry/*` (`trace_ray/hull`, `calc_angle`, `can_shoot`) | `systems/impl/tracing.cpp`, `math::helpers`, `shared::can_shoot` | прямое соответствие, геометрия не дублировалась |
| `hitbox_backtrack/1800D2FD0` multipoints (scale 0..100, max 128) | `generate_multipoints` + `pointscale`/`centers_only` | ПЕРЕДЕЛАНО: мультипоинты только для подробных целей и при `centers_only=off` |
| `hitbox_backtrack/180001180` `max backtrack ticks` | `lagcomp::get_scan_records` (4 записи) + `max_backtrack_ticks` | ПЕРЕДЕЛАНО: новейшая + 2 средних + старейшая |

## Что НЕ переносим кодом (только идеи)

1. Сырой псевдокод Ghidra (`undefined8`, `DAT_xxx`, `FUN_xxx`) — типы/структуры не восстановлены, копипаст сломает билд.
2. Lua-диспетчер стадий (`rage_targets/scan/select/fire`, `claim_command`, бюджет 16ms) — у нас нет Lua VM в rage-пайплайне.
3. `skeet_payload.dll` / `skeet.exe` — чужой бинарник, в репозиторий не кладем.
4. No-spread коррекцию выстрела (`solve_spread` уже есть свой) — skeet-вариант только как референс математики.

## Открытые кандидаты (следующие шаги)

1. Эвристика сэмплера `0.9/0.35/0.65` из `1800487A0` — только после юнит-теста.
2. `rage_match_*/requested_hitbox`-проверки: убедиться, что «фактически поражённый хитбокс»
   совпадает с запрошенным в мультипоинтах (сейчас есть гейт только для головы).
3. Оценка Lua-стадий как отдельного модуля расширений (вне rage-пайплайна).

## Проверка переноса

```powershell
Get-ChildItem -Path 'project\docs\skeet_rage_reference' | Format-Table Name
Get-ChildItem -Path 'project\docs\skeet_rage_reference' -Recurse -File | Measure-Object -Property Length -Sum
cmd /c scratch\syntax_rage.bat
py -3 project\tests\config_hit_jumpbug_checks.py
```
