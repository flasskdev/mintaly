# Ragebot: почему не стрелял и что исправлено

Дата: 01.10.2026. База: локальное дерево `mintaly-cs2` с уже применённым
`mintaly-rage-no-shot-v3.patch`. Основание: реальные логи
`%TEMP%\nemesis-<PID>\mintaly_init.log` сессий 30.09–01.10 и данные
`rage-diag:*`.

## Симптом

Рейдж не стреляет (или стреляет единично). В логе это видно прямо:

```
[rage-diag:pipeline] entry=938 gun_calls=434 targets=434 scan_empty=14 scan_nonempty=420
                     scan_hits=8170 selection=420 selected=420 fire_calls=420 attack_set=6
[rage-diag:penetration] pen_calls=16993 contacts=38416 damage_exhausted=8226
                        contact_mismatch=18403 no_target_hit=16487 pen_success=506
```

`fire_calls` — сотни за 5 секунд, `attack_set` — единицы. В другой сессии
(`no_spread=1`) скан вообще не давал ни одного попадания:
`contact_mismatch == no_target_hit == pen_calls`.

## Причины, найденные по логам и коду

### 1. `penetration::run` отбраковывал почти каждый пробив

`[rage-contact:v2]` показал, что для записи, попадающей в цель, движок отдаёт
индекс контакта, который резолвится в worldspawn:

```
record=0 damage=83.0921 enter=0.000000 exit=0.644980 team=4 enter_ix=8000 exit_ix=0000 bits=a4
record=1 damage=61.7616 enter=0.644980 exit=1.000000 team=3 enter_ix=8000 exit_ix=0002 bits=a5
```

`enter_ix & 0x7fff == 0` — это контакт мира, а не игрока, поэтому проверка
`hit_entity != ctx.target_pawn` уходила в `contact_mismatch` и точка отбрасывалась.
Кроме того, старая ветка `if ( ( hit->can_penetrate & 1 ) != 0 )` проверяла бит,
который по всем восьми разобранным записям равен чётности индекса записи
(`0x52/0x53`, `0x54/0x55`, `0xa4/0xa5`, `0x42/0x43/0x42`), то есть пропускала
именно целевую запись и обрывала цикл без попадания.

### 2. `fire_gun` прерывал выстрел без input history

```cpp
const auto history_size = cmd->csgo_user_cmd.input_history_size();
if (!base || !base->mutable_viewangles() || history_size <= 0 || ...) return;
```

`history_size <= 0` возвращал управление **до** установки кнопки атаки. При
`fire_calls=420` до `attack_set` доходило 6 — ровно доля команд, у которых
клиент заполнил input history. При этом `fire_melee` (нож/тезер) такого гейта
не имеет и обрабатывает пустую историю корректно, а `push_input_history` в
`systems::input` вообще нигде не вызывался.

### 3. Ассерт на `out.penetrated`

Флаг побития выставлялся по тому самому биту чётности, поэтому `legit`
(`!autowall && penetrated`) и скоринг рейджа получали случайное значение.

## Что изменено

| Файл | Изменение |
|---|---|
| `core/features/combat/impl/shared.cpp` | `penetration::run`: ветка `can_penetrate` удалена; контакт остался быстрым подтверждением, добавлен фолбэк по геометрии записи (сегмент пути, накрывающий точку входа луча в тело цели, и только при `damage > 0`); `out.penetrated` считается по `g_tracing.is_visible`, а не по дележу пути на записи |
| `core/features/combat/impl/shared.cpp` | `penetration::can` (прицел пробития) больше не зависит от бита чётности: возвращает, остался ли у пули урон в конце отрезка |
| `core/features/combat/impl/rage.cpp` | `fire_gun` больше не прерывается при пустой input history: запись создаётся через `push_input_history`, иначе атака якорится индексом `-1`; прерывание осталось только при отсутствии `base`/`viewangles` |
| `core/features/combat/impl/rage.cpp` | `on_create_move` считает каждую причину раннего выхода (`gate_*`) |
| `core/systems/impl/input.cpp` | `push_input_history` защищён как `acquire_subtick_step`: проверка паттернов, `safe_call`, `valid_runtime_pointer`, подтверждение роста размера вектора |
| `utilities/rage_scan_diagnostics.hpp` | новые счётчики телеметрии (см. ниже) |

## Как проверить результат

Телеметрия остаётся включённой. После игры в логе
`%TEMP%\nemesis-<PID>\mintaly_init.log` смотрите группы
`[rage-diag:pipeline|scan|penetration]`:

- `gate_inactive` — невалидный контекст или оружие вне pistol..lmg;
- `gate_no_enemies` — живых врагов нет;
- `gate_disabled` — рейдбот выключен в меню;
- `gate_cannot_shoot` — `can_shoot` запретил (перезарядка, задержка и т.д.);
- `fire_calls` → `attack_set`: раньше разрыв был 100:1, теперь должен быть близок к 1:1;
- `pen_contact_accept` + `pen_geometry_accept` против `contact_mismatch` / `no_target_hit`:
  суммарное число принятых точек должно вырасти на порядки;
- `fire_history_pushed` / `fire_history_missing` — как часто приходилось создавать
  запись истории заново;
- `fire_abort_viewangles` — сколько выстрелов некуда было записать.

Проверка на стенде (без игры):

```powershell
cmd /c scratch\compile_rage_fix.bat      # объекты shared/rage/input без ошибок
cmd /c scratch\compile_combat_fix.bat    # legit/misc/extrapolation/menu
py -3 project\tests\rage_contact_geometry_checks.py
py -3 -m unittest discover -s project\tests -p '*_checks.py'
```

## Остаточные риски

- Игра не проверялась из сессии: числа `damage` берутся из той же
  `trace_bullet`, а принятие точки теперь опирается на геометрию записи. Порог
  `min_damage` и скейлы не менялись.
- Если `trace_bullet` решает продолжить проекцию сквозь корпус блокирующего
  игрока, точка в цели всё ещё может быть принята: движок сам не даст урона по
  такой цели. Отдельного «блокирует другой пешка» теста нет — контакты движка
  для этого оказались ненадёжны.
- `out.penetrated` теперь стоит один `is_visible`-трейс на принятую точку.
  Вызовы рейдж-скана идут из воркеров, как и прежние обращения к
  `g_entities`/`g_tracing` в этом коде.
- `push_input_history` впервые вызывается из игры: если паттерн
  `history_field_alloc` не сойдётся на текущем билде клиента, функция вернёт
  `nullptr`, счётчик `fire_history_missing` вырастет, но выстрел всё равно
  уйдёт с индексом `-1`.
