<!-- split-part | CS2_RESEARCH_MASTER.md lines 2262-2324 | body-sha256 228b5466d9a23e318c6e70e915f1066f7c46806e94d445cfe9c503e247527d0c -->
[← все части](../README.md) · [индекс отчётов](00-index.md)

<!-- split-body-start -->

<a id="doc-06"></a>

# Приложение D06. `analysis/strings/summary.md`

**Дословный исходный отчёт.** Даты/статусы/неизвестные роли внутри относятся к своему этапу. При расхождении использовать актуальный свод и поздние доказательства. Пути внутри текста — исторические: соответствующие материалы встроены в MD/CPP и находятся через поиск исходного пути.

# Компактный каталог строк raw x64 дампа

- SHA-256 проверен: `3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27`. Размер `0x05001000`, base `0x212C3300000`. File offset = RVA; VA = base + RVA. Только статические байты; бинарник не запускался, xrefs/код/Ghidra/SQLite не читались.
- Конкретные ragebot/aimbot/hitchance/resolver/lagcomp/backtrack/multipoint/penetration/antiaim/rapidfire/defensive/tickbase anchors не найдены открытым текстом. Это не доказательство отсутствия функций и не доказательство обфускации.
- UI и реализация: подтверждённых feature UI labels или implementation symbols нет. Поля usercmd/cs_usercmd — protobuf-метаданные, могут вести к сериализации. Три engine_name anchors вне этих дескрипторов приоритетнее для отдельного xref-прохода; это не подтверждённые адреса функций.
- History/interpolation/target/shoot position и subtick/prediction/random_seed присутствуют в схемах. Нельзя по ним утверждать наличие backtrack, resolver, tickbase-shift или алгоритма aim.
- `spread` неоднозначен; `damagefilter` — имя свойства без установленной роли. `spreadMethod`/RTTI SpreadMethod@lunasvg относятся к SVG, не weapon spread.
- `records` при `0x00ED2CE8` соседствует с `TrajectoryConstants` в DXBC shader metadata; при `0x00F07118` — с `TracerConstants`/`TracerRecord`. Эти строки не использовать как доказательство lag records.
- Происхождение: 8 прямых путей зависимостей содержат `neverlose\neverlose-cs2`; `neverlose_cs2.dll` при `0x03A4238D` и particle-ассеты подтверждают согласованное имя проекта/артефакта Neverlose CS2. Подлинность не проверена; CI runner identifier скрыт.
- Точный релиз/build/date продукта не найден. `LuaJIT 2.1.1774946682` и `OpenSSL 1.1.1t  7 Feb 2023` — версии компонентов, не продукта. LuaJIT number не трактуется как product build date.
- Lua/API: LuaJIT, jit.opt, jit.util, LUA_PATH/LUA_CPATH и MessagePack Lua diagnostics найдены. Специфический публичный Lua API rage/antiaim/hitchance не подтверждён.
- RTTI: 220 декорированных MSVC type-name строк, преимущественно protobuf/CryptoPP/std/Boost/lunasvg. Наличие имени типа не доказывает исполнение кода. Полный компактный список — `rtti.csv`.
- PDB/version: точные сигнатуры {'RSDS': 0, 'NB10': 0, 'VS_FIXEDFILEINFO': 0}; результаты поиска .pdb/FileVersion/ProductVersion — `coverage.json`. В проверенных представлениях эти строки не найдены.

## 20 anchors

| Строка | RVA = offset | VA | Роль |
|---|---|---|---|
| `CCSPlayer_MovementServices` | `0x00F4EB78` | `0x212C424EB78` | engine_name |
| `CPlayer_WeaponServices` | `0x00F4EB93` | `0x212C424EB93` | engine_name |
| `CSGOInterpolationInfo` | `0x00F12B80` | `0x212C4212B80` | engine_name |
| `spread` | `0x00F58EC8` | `0x212C4258EC8` | ambiguous_bare_string |
| `CBaseUserCmdPB` | `0x00DED236` | `0x212C40ED236` | protobuf_message |
| `CSubtickMoveStep` | `0x00DED14F` | `0x212C40ED14F` | protobuf_message |
| `prediction_offset_ticks_x256` | `0x00DED27C` | `0x212C40ED27C` | protobuf_field |
| `subtick_moves` | `0x00DED3A5` | `0x212C40ED3A5` | protobuf_field |
| `consumed_server_angle_changes` | `0x00DED3E1` | `0x212C40ED3E1` | protobuf_field |
| `CSGOInputHistoryEntryPB` | `0x00DF1428` | `0x212C40F1428` | protobuf_message |
| `input_history` | `0x00DF16A6` | `0x212C40F16A6` | protobuf_field |
| `attack1_start_history_index` | `0x00DF16D7` | `0x212C40F16D7` | protobuf_field |
| `attack2_start_history_index` | `0x00DF1700` | `0x212C40F1700` | protobuf_field |
| `player_tick_fraction` | `0x00DF14B9` | `0x212C40F14B9` | protobuf_field |
| `cl_interp` | `0x00DF14D7` | `0x212C40F14D7` | protobuf_field |
| `target_ent_index` | `0x00DF15AA` | `0x212C40F15AA` | protobuf_field |
| `shoot_position` | `0x00DF15C8` | `0x212C40F15C8` | protobuf_field |
| `target_head_pos_check` | `0x00DF15ED` | `0x212C40F15ED` | protobuf_field |
| `LuaJIT 2.1.1774946682` | `0x00F83078` | `0x212C4283078` | dependency_version |
| `neverlose_cs2.dll` | `0x03A4238D` | `0x212C6D4238D` | product_artifact_name |

## Файлы

- `evidence.json` / `evidence.csv`: 299 верифицированных byte spans с offset/RVA/VA, ролью и длиной; runner ID редактирован без изменения адреса/исходной длины.
- `anchors.json` / `anchors.csv`: 43 приоритетных строк; `rtti.csv`: RTTI; `coverage.json`: точные positive/negative counts.
- `extract_compact.py` — воспроизводимый завершённый компактный проход: `python3 -B analysis/strings/extract_compact.py`.
- `extract_strings.py` — дополнительный широкий скрипт. Актуальный завершённый результат — компактные JSON/CSV и `summary.md`; дополнительные `schema_fields.csv` и `xor_probe.json` получены широким проходом. Ожидание широкого прогона было прервано пользователем, но файлы успели сохраниться.

## Открытым текстом не найдено

`ragebot`, `rage bot`, `aimbot`, `aim bot`, `hitchance`, `hit chance`, `hit_chance`, `multipoint`, `multi point`, `multi_point`, `resolver`, `backtrack`, `back_track`, `lagcomp`, `lag_comp`, `lag compensation`, `lag_compensation`, `penetration`, `autowall`, `inaccuracy`, `minimum_damage`, `minimum damage`, `antiaim`, `anti aim`, `anti_aim`, `rapidfire`, `rapid fire`, `rapid_fire`, `doubletap`, `double tap`, `double_tap`, `defensive`, `tickbase`, `tick_base`, `autostop`, `auto_stop`, `bunnyhop`, `bhop`, `strafe`, `keybind`, `hotkey`, `ui.find`, `rage.antiaim`, `.pdb`, `FileVersion`, `ProductVersion`

Ограничения: выборка целевая; произвольные Unicode, сложная обфускация/упаковка и функции не восстанавливались. Короткие подстроки/соседство не устанавливают API, UI, алгоритм или функцию-владельца. Полный неотфильтрованный dump строк не сохранялся.

Дополнительная проверка: все 58 protobuf field-name spans из `schema_fields.csv` повторно сопоставлены исходным байтам.


---
