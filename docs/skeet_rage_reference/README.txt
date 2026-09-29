RAGE-ДЕКОМПИЛЯЦИЯ skeet.exe — КОД ПО ФУНКЦИЯМ (отдельные .txt)
================================================================================

ИСТОЧНИК
--------------------------------------------------------------------------------
skeet.exe (x64 PE, 7 004 160 байт) — это лоадер. В секции .rsrc лежит
встроенный PE-DLL (4 736 000 байт, image base 0x180000000,
PDB-путь: C:\skeet\skeet\..\bin\Release\payload\skeet.pdb) — это и есть
весь чит, включая рейджбот. Дамп делался с этой DLL (skeet_payload.dll),
извлечённой из skeet.exe. Внешний .exe/.rdata рейдж-кода не содержит
(все якорные строки находятся только внутри payload).

ИНСТРУМЕНТ
--------------------------------------------------------------------------------
Ghidra 12.1.4 PUBLIC, headless (analyzeHeadless), встроенный Hex-Rays
декомпилятор (DecompInterface), скрипт RageDump.java.
0 ошибок декомпиляции из 980 функций.

СТРУКТУРА
--------------------------------------------------------------------------------
pipeline/         (357 файлов) — пайплайн create_move, стадии rage-бота,
                   Lua-диспетчер rage_* стадий, сборка цели/таргетов
hitchance/        (135 файлов) — расчёт hitchance, required_hitchance,
                   симуляция, force-shot проверки
nospread/         (228 файлов) — spread/seed/no_spread, расчёт разброса,
                   weapon_calculate_spread, prediction_seed
autowall_damage/  ( 35 файлов) — autowall/penetration, просчёт урона,
                   min_damage, hitgroup/armor/damage scaling
trace_geometry/   (207 файлов) — trace_ray/trace_hull/trace_ray_entity,
                   calc_angle / aim_punch / can_shoot (геометрия)
hitbox_backtrack/ ( 18 файлов) — multipoints, hitbox-трансформы,
                   backtrack (max backtrack ticks), player_hurt/weapon_fire

INDEX.tsv   — таблица: ea, имя, файл, категория, глубина (depth), via, размер.
SUMMARY.txt — сводка по категориям.

Каждый .txt = РОВНО ОДНА функция:
  // function: FUN_xxx
  // ea: адрес
  // category / depth / via (как найдена: якорная строка или вызов)
  // size: размер в байтах
  // anchors: какие якорные строки привели к функции
  далее — псевдокод C от Hex-Rays.

КАРТА КЛЮЧЕВЫХ ФУНКЦИЙ (проверено по строкам и графу вызовов)
--------------------------------------------------------------------------------
ПАЙПЛАЙН РЕЙДЖА
  pipeline\180087F70_FUN_180087f70.txt
      Главный create_move: логирует "create_move: rage begin/end",
      вызывает pre_rage -> rage-стадию -> post_rage (Lua события),
      legit-стадию, quickpeek, input apply.
  pipeline\180038870_FUN_180038870.txt
      Rage-драйвер стадии (вызывается из create_move): вызывает
      FUN_180039560 (сбор цели) и FUN_1800393e0/1800392f0 (выстрел).
  pipeline\180039560_FUN_180039560.txt   [15 790 байт]
      Ядро рейджа: поля targets/records/fov/in_fov/health/armor/
      min_damage/allow_fire/weapon_type + вызов hitchance (FUN_180041210)
      и Lua-стадий. Это "ragebot: сбор цели и условий выстрела".
  pipeline\1800A3380_FUN_1800a3380.txt
      Lua-диспетчер стадий: строки "rage_targets expects...",
      "rage_scan expects...", "rage_select expects...",
      "rage_fire expects..." — приём/валидация ответов Lua-скриптов.
  pipeline\180108100_FUN_180108100.txt
      Диагностика "RAGEBOT PROBLEM DETECTED:".

HITCHANCE
  hitchance\180041210_FUN_180041210.txt   [7 536 байт]
      РАСЧЁТ HITCHANCE: no_spread, required_hitchance, allow_force,
      inaccuracy, spread; строит точки, симулирует попадания,
      возвращает процент. Главная hitchance-функция.
  hitchance\180043550_FUN_180043550.txt
      Хелпер проверки попадания: читает настройки "hitchance",
      "required_hitchance"; вызывает FUN_180049950 (оценка урона).
  hitchance\1800D5530_FUN_1800d5530.txt
      Lua API "ragebot.hitchance" (только для rage_select).

ПРОСЧЁТ УРОНА / AUTOWALL
  autowall_damage\180044DC0_FUN_180044dc0.txt  [1 746 строк]
      ЯДРО rage-scan: точки/хитбоксы/множители, min_damage,
      trace_budget, prediction, inaccuracy, spread, centers_only;
      вызывает автовол-семплер FUN_1800487a0 и оценку FUN_1800485e0.
  autowall_damage\1800487A0_FUN_1800487a0.txt  [4 458 байт]
      Семплирование разброса/проникновения: доля param/100 (0..1),
      32 сэмпла (0x20) вокруг направления, эвристики 0.9 / 0.35 / 0.65.
  autowall_damage\180049950_FUN_180049950.txt
      ВОЗВРАТ РЕЗУЛЬТАТА УРОНА Lua: "damage", "hitbox", "requested_hitbox",
      "hitgroup", "center", "penetrated", "record", "health", "angles".
  autowall_damage\1800CBA90_FUN_1800cba90.txt
      Оценка пробития: "damage", "hitbox", "hitgroup", "penetrated".
  pipeline\180044370 / pipeline\180044C60
      Обёртки/итераторы, ведущие в ядро scan (FUN_180044dc0).

NOSPREAD / SPREAD
  nospread\1800675F0, 180068C30, 180069F60, 18006AA30, 18006B980,
  18006BFF0, 18006B620, 180069A40 ...
      Функции расчёта разброса/сидов (найдены через таблицы API
      get_spread / no_spread / weapon_calculate_spread).
  nospread\180163290_FUN_180163290.txt
      Резолв игровых интерфейсов: game_trace_manager, prediction_seed,
      prediction_state, csgo_input, global_vars и т.д.
  nospread\180003BD0_FUN_180003bd0.txt
      Настройки режима No Spread (страница меню).
  nospread\180084040_FUN_180084040.txt
      Огромный диспетчер Lua-биндов (cmd_interpreter, override_view,
      handle_view_angles, get_transforms_for_hitbox_list, ...).

ГЕОМЕТРИЯ / ТРЕЙСЫ
  trace_geometry\1800C9590_FUN_1800c9590.txt
      Обёртка трейса: "all_solid", "end_pos", "fraction", "normal".
  trace_geometry\180114EE0 / 180115130 / 180115500 / 180114DD0 ...
      trace_ray / trace_hull / trace_ray_entity / trace_filter
      (дескрипторы DAT_1804668b8 и др. из .data-таблицы API).
  trace_geometry\1800AAE50_FUN_1800aae50.txt  [7 246 байт]
      Lua API combat/math: calc_angle, angle_vectors, calc_fov,
      aim_punch, can_shoot, damage, attack, claim_command, ...

HITBOX / BACKTRACK
  hitbox_backtrack\180001180_FUN_180001180.txt
      Ragebot-информация: "ragebot", "max backtrack ticks".
  hitbox_backtrack\1800D2FD0_FUN_1800d2fd0.txt
      Multipoints-скан ("multipoints", "point must contain hitbox").



КАК ЭТО РАБОТАЛО (алгоритм поиска RageDump.java)
--------------------------------------------------------------------------------
1. Поиск ASCII-строк-якорей по секциям (pipeline/hitchance/nospread/
   autowall_damage/trace_geometry/hitbox_backtrack).
2. Сиды = функции, ссылающиеся на строку (xrefs) + сырой скан 8-байтных
   указателей на строку (таблицы {name_ptr, ...}) + преследование
   дескрипторов в .data до функций, на них ссылающихся.
3. BFS по графу вызовов/данных на глубину <=3; исключены:
   - ядро Lua VM (по строкам "stack traceback:", "attempt to index"...)
   - хабы с >30 вызывающими (общие утилиты).
   Потолок 1500 функций; фактически отобрано 980.
4. Каждая функция декомпилирована и записана в отдельный .txt
   в папку своей категории (+ INDEX.tsv, SUMMARY.txt).

ПРИМЕЧАНИЯ
--------------------------------------------------------------------------------
- Имена функций не сохранены (PDB недоступен) — это FUN_<ea>.
- Категория = по каким якорям функция найдена; вложенные вызовы
  унаследовали категорию родителя (depth/via в шапке файла).
- Обфусцированные/шифрованные таблицы данных не всегда удалось связать
  с кодом, но ключевые подсистемы рейджа (пайплайн, hitchance,
  spread/nospread, autowall/урон, геометрия/трейсы, hitbox/backtrack)
  покрыты.
