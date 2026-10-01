<!-- split-part | CS2_RESEARCH_MASTER.md lines 100313-100487 | body-sha256 52025b0e9ed0e73c12aa5b219934930ded7ccc5a26a76d53ad3d8dfc212f1187 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-179"></a>

## E179. `analysis/strings/extract_compact.py`

Bytes: 13899. SHA-256: `4861bfc9d9eb8a670655b454498e218b487d2d241ce3a50acc6de0b887100d1c`.

```python
#!/usr/bin/env python3
"""Static, read-only compact string anchors; writes only analysis/strings/."""
import collections
import csv
import hashlib
import json
import pathlib
import re

ROOT = pathlib.Path(__file__).resolve().parent
SOURCE = ROOT.parent / 'input' / 'cs2_212C3300000.bin'
BASE = 0x212C3300000
EXPECTED = '3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27'
ANCHORS = [
 (0xF4EB78,'CCSPlayer_MovementServices','engine_name'),
 (0xF4EB93,'CPlayer_WeaponServices','engine_name'),
 (0xF12B80,'CSGOInterpolationInfo','engine_name'),
 (0xF58EC8,'spread','ambiguous_bare_string'),
 (0xDED0F2,'CInButtonStatePB','protobuf_message'),
 (0xDED14F,'CSubtickMoveStep','protobuf_message'),
 (0xDED192,'analog_forward_delta','protobuf_field'),
 (0xDED1B0,'analog_left_delta','protobuf_field'),
 (0xDED1CB,'pitch_delta','protobuf_field'),
 (0xDED1E0,'yaw_delta','protobuf_field'),
 (0xDED236,'CBaseUserCmdPB','protobuf_message'),
 (0xDED248,'legacy_command_number','protobuf_field'),
 (0xDED267,'client_tick','protobuf_field'),
 (0xDED27C,'prediction_offset_ticks_x256','protobuf_field'),
 (0xDED2C9,'viewangles','protobuf_field'),
 (0xDED2EA,'forwardmove','protobuf_field'),
 (0xDED2FF,'leftmove','protobuf_field'),
 (0xDED311,'upmove','protobuf_field'),
 (0xDED348,'random_seed','protobuf_field'),
 (0xDED3A5,'subtick_moves','protobuf_field'),
 (0xDED3E1,'consumed_server_angle_changes','protobuf_field'),
 (0xDF1394,'CSGOInterpolationInfoPB','protobuf_message'),
 (0xDF1428,'CSGOInputHistoryEntryPB','protobuf_message'),
 (0xDF1443,'view_angles','protobuf_field'),
 (0xDF1465,'render_tick_count','protobuf_field'),
 (0xDF1480,'render_tick_fraction','protobuf_field'),
 (0xDF149E,'player_tick_count','protobuf_field'),
 (0xDF14B9,'player_tick_fraction','protobuf_field'),
 (0xDF14D7,'cl_interp','protobuf_field'),
 (0xDF1507,'sv_interp0','protobuf_field'),
 (0xDF1535,'sv_interp1','protobuf_field'),
 (0xDF1563,'player_interp','protobuf_field'),
 (0xDF15AA,'target_ent_index','protobuf_field'),
 (0xDF15C8,'shoot_position','protobuf_field'),
 (0xDF15ED,'target_head_pos_check','protobuf_field'),
 (0xDF1619,'target_abs_pos_check','protobuf_field'),
 (0xDF1644,'target_abs_ang_check','protobuf_field'),
 (0xDF1676,'CSGOUserCmdPB','protobuf_message'),
 (0xDF16A6,'input_history','protobuf_field'),
 (0xDF16D7,'attack1_start_history_index','protobuf_field'),
 (0xDF1700,'attack2_start_history_index','protobuf_field'),
 (0xDF174B,'is_predicting_body_shot_fx','protobuf_field'),
 (0xDF1776,'is_predicting_head_shot_fx','protobuf_field'),
]
SUPPORT = [
 (0xDED0B2,'usercmd.proto','protobuf_source'),(0xDF1342,'cs_usercmd.proto','protobuf_source'),
 (0xF4EFB5,'damagefilter','ambiguous_engine_property'),
 (0xED2CE8,'records','shader_reflection_not_lag_records'),
 (0xED2CF0,'TrajectoryConstants','shader_reflection_context'),
 (0xED2F89,'prediction','shader_reflection_not_tickbase'),
 (0xED301D,'Record','shader_reflection_context'),
 (0xF07118,'records','shader_reflection_not_lag_records'),
 (0xF07120,'TracerConstants','shader_reflection_context'),
 (0xF07281,'TracerRecord','shader_reflection_context'),
 (0xF755D8,'spreadMethod','svg_not_weapon_spread'),
 (0xE03720,'OpenSSL 1.1.1t  7 Feb 2023','dependency_version'),
 (0xF83078,'LuaJIT 2.1.1774946682','dependency_version'),
 (0xF644E0,'luaJIT_BC_%s','lua_runtime'),
 (0xF5EB40,'jit.opt','lua_runtime'),(0xF6C180,'jit.util','lua_runtime'),
 (0xF7B398,'LUA_NOENV','lua_runtime'),(0xF7D098,'LUA_PATH','lua_runtime'),
 (0xF7D0A8,'LUA_CPATH','lua_runtime'),(0xF86790,'lua_debug> ','lua_runtime'),
 (0xF4D990,'in function mp_encode_lua_table_as_array','lua_messagepack'),
 (0xF4D9CD,'in function mp_decode_to_lua_array','lua_messagepack'),
 (0xF4F347,'in function mp_encode_lua_table_as_map','lua_messagepack'),
 (0x3A4238D,'neverlose_cs2.dll','product_artifact_name'),
]
TERMS = ['ragebot','rage bot','aimbot','aim bot','hitchance','hit chance','hit_chance','multipoint','multi point','multi_point','resolver','backtrack','back_track','lagcomp','lag_comp','lag compensation','lag_compensation','penetration','autowall','inaccuracy','minimum_damage','minimum damage','antiaim','anti aim','anti_aim','rapidfire','rapid fire','rapid_fire','doubletap','double tap','double_tap','defensive','tickbase','tick_base','autostop','auto_stop','bunnyhop','bhop','strafe','keybind','hotkey','ui.find','rage.antiaim','.pdb','FileVersion','ProductVersion']

def address(offset):
 return f'0x{offset:08X}'

def save_csv(name,rows):
 columns=['file_offset_hex','rva_hex','va_hex','file_offset','rva','va','byte_length','encoding','text','classification','redacted']
 with (ROOT/name).open('w',encoding='utf-8',newline='') as stream:
  writer=csv.DictWriter(stream,fieldnames=columns,extrasaction='ignore')
  writer.writeheader()
  writer.writerows(rows)

def main():
 if ROOT.name!='strings' or ROOT.parent.name!='analysis':
  raise SystemExit('Output scope must be analysis/strings/')
 blob=SOURCE.read_bytes()
 assert len(blob)==0x5001000 and hashlib.sha256(blob).hexdigest()==EXPECTED
 rows=[]
 def add(offset,text,classification):
  raw=text.encode('ascii')
  assert blob[offset:offset+len(raw)]==raw,(address(offset),text)
  safe=re.sub(r'(\\builds\\)[^\\]+',r'\1[RUNNER-ID REDACTED]',text,flags=re.I)
  row={'file_offset_hex':address(offset),'rva_hex':address(offset),'va_hex':address(BASE+offset),'file_offset':offset,'rva':offset,'va':BASE+offset,'byte_length':len(raw),'encoding':'ascii','text':safe,'classification':classification,'redacted':safe!=text}
  rows.append(row)
  return row
 anchors=[add(*item) for item in ANCHORS]
 for item in SUPPORT:
  add(*item)
 rtti=[]
 paths=[]
 assets=[]
 for match in re.finditer(rb'[\x20-\x7e]{4,}',blob):
  raw=match.group()
  if raw.startswith((b'.?AV',b'.?AU',b'.?AT')) and raw.endswith(b'@@'):
   rtti.append(add(match.start(),raw.decode('ascii'),'rtti_type_name'))
  elif raw.startswith(b'C:\\') and b'neverlose\\neverlose-cs2\\' in raw:
   paths.append(add(match.start(),raw.decode('ascii'),'build_source_path'))
  elif b'particles/neverlose' in raw:
   assets.append(add(match.start(),raw.decode('ascii'),'particle_asset_identity'))
 lowered=blob.lower()
 coverage=[]
 for term in TERMS:
  counts={encoding:lowered.count(term.lower().encode(encoding)) for encoding in ['ascii','utf-16le','utf-16be','utf-32le','utf-32be']}
  coverage.append({'term':term,'counts':counts})
 markers={name:blob.count(marker) for name,marker in [('RSDS',b'RSDS'),('NB10',b'NB10'),('VS_FIXEDFILEINFO',b'\xbd\x04\xef\xfe')]}
 metadata={'scope':'Static bytes only; target not executed; no code/xrefs/database reads.','source':str(SOURCE),'sha256':EXPECTED,'size':len(blob),'base':address(BASE),'address_model':'file offset = RVA; VA = supplied base + RVA; offsets are first text bytes, not enclosing metadata headers.','selection':'Verified curated literal spans + ASCII MSVC RTTI + build paths + product particle names. Compact catalog is deliberately selective, not all strings.','negative_search':'Case-insensitive byte substring searches in ASCII and ASCII-range UTF-16LE/BE/UTF-32LE/BE; no exhaustive Unicode/decryption/packing search.','ui_implementation_warning':'No confirmed target-feature UI names or implementation symbols; protobuf names describe schemas, engine_name labels do not prove function ownership.','markers':markers,'counts':dict(collections.Counter(row['classification'] for row in rows)),'verified_spans':len(rows)}
 for filename,payload in [('evidence.json',{'metadata':metadata,'evidence':rows}),('anchors.json',{'warning':metadata['ui_implementation_warning'],'anchors':anchors}),('coverage.json',{'method':metadata['negative_search'],'queries':coverage,'exact_markers':markers})]:
  (ROOT/filename).write_text(json.dumps(payload,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
 save_csv('evidence.csv',rows)
 save_csv('anchors.csv',anchors)
 save_csv('rtti.csv',rtti)
 negative=[entry['term'] for entry in coverage if not any(entry['counts'].values())]
 selected_names=['CCSPlayer_MovementServices','CPlayer_WeaponServices','CSGOInterpolationInfo','spread','CBaseUserCmdPB','CSubtickMoveStep','prediction_offset_ticks_x256','subtick_moves','consumed_server_angle_changes','CSGOInputHistoryEntryPB','input_history','attack1_start_history_index','attack2_start_history_index','player_tick_fraction','cl_interp','target_ent_index','shoot_position','target_head_pos_check','LuaJIT 2.1.1774946682','neverlose_cs2.dll']
 table=['| Строка | RVA = offset | VA | Роль |','|---|---|---|---|']
 for name in selected_names:
  row=next(row for row in rows if row['text']==name)
  table.append(f"| `{name}` | `{row['rva_hex']}` | `{row['va_hex']}` | {row['classification']} |")
 summary='\n'.join([
 '# Компактный каталог строк raw x64 дампа','',
 f'- SHA-256 проверен: `{EXPECTED}`. Размер `0x05001000`, base `0x212C3300000`. File offset = RVA; VA = base + RVA. Только статические байты; бинарник не запускался, xrefs/код/Ghidra/SQLite не читались.',
 '- Конкретные ragebot/aimbot/hitchance/resolver/lagcomp/backtrack/multipoint/penetration/antiaim/rapidfire/defensive/tickbase anchors не найдены открытым текстом. Это не доказательство отсутствия функций и не доказательство обфускации.',
 '- UI и реализация: подтверждённых feature UI labels или implementation symbols нет. Поля usercmd/cs_usercmd — protobuf-метаданные, могут вести к сериализации. Три engine_name anchors вне этих дескрипторов приоритетнее для отдельного xref-прохода; это не подтверждённые адреса функций.',
 '- History/interpolation/target/shoot position и subtick/prediction/random_seed присутствуют в схемах. Нельзя по ним утверждать наличие backtrack, resolver, tickbase-shift или алгоритма aim.',
 '- `spread` неоднозначен; `damagefilter` — имя свойства без установленной роли. `spreadMethod`/RTTI SpreadMethod@lunasvg относятся к SVG, не weapon spread.',
 '- `records` при `0x00ED2CE8` соседствует с `TrajectoryConstants` в DXBC shader metadata; при `0x00F07118` — с `TracerConstants`/`TracerRecord`. Эти строки не использовать как доказательство lag records.',
 f'- Происхождение: {len(paths)} прямых путей зависимостей содержат `neverlose\\neverlose-cs2`; `neverlose_cs2.dll` при `0x03A4238D` и particle-ассеты подтверждают согласованное имя проекта/артефакта Neverlose CS2. Подлинность не проверена; CI runner identifier скрыт.',
 '- Точный релиз/build/date продукта не найден. `LuaJIT 2.1.1774946682` и `OpenSSL 1.1.1t  7 Feb 2023` — версии компонентов, не продукта. LuaJIT number не трактуется как product build date.',
 '- Lua/API: LuaJIT, jit.opt, jit.util, LUA_PATH/LUA_CPATH и MessagePack Lua diagnostics найдены. Специфический публичный Lua API rage/antiaim/hitchance не подтверждён.',
 f'- RTTI: {len(rtti)} декорированных MSVC type-name строк, преимущественно protobuf/CryptoPP/std/Boost/lunasvg. Наличие имени типа не доказывает исполнение кода. Полный компактный список — `rtti.csv`.',
 f'- PDB/version: точные сигнатуры {markers}; результаты поиска .pdb/FileVersion/ProductVersion — `coverage.json`. В проверенных представлениях эти строки не найдены.',
 '', '## 20 anchors','',*table,'',
 '## Файлы','',
 f'- `evidence.json` / `evidence.csv`: {len(rows)} верифицированных byte spans с offset/RVA/VA, ролью и длиной; runner ID редактирован без изменения адреса/исходной длины.',
 f'- `anchors.json` / `anchors.csv`: {len(anchors)} приоритетных строк; `rtti.csv`: RTTI; `coverage.json`: точные positive/negative counts.',
 '- `extract_compact.py` — воспроизводимый завершённый компактный проход: `python3 -B analysis/strings/extract_compact.py`.',
 '- `extract_strings.py` — сохранённый широкий скрипт; его прогон был прерван по запросу. Для итогов использовать компактные файлы, перечисленные выше.',
 '', '## Открытым текстом не найдено','',', '.join('`'+term+'`' for term in negative), '',
 'Ограничения: выборка целевая; произвольные Unicode, сложная обфускация/упаковка и функции не восстанавливались. Короткие подстроки/соседство не устанавливают API, UI, алгоритм или функцию-владельца. Полный неотфильтрованный dump строк не сохранялся.',
 ])
 (ROOT/'summary.md').write_text(summary+'\n',encoding='utf-8')
 assert hashlib.sha256(SOURCE.read_bytes()).hexdigest()==EXPECTED
 print(json.dumps({'status':'ok','directory':str(ROOT),'evidence':len(rows),'anchors':len(anchors),'rtti':len(rtti),'paths':len(paths),'exact_markers':markers,'nonzero_feature_queries':[item for item in coverage if any(item['counts'].values())]},ensure_ascii=False))

if __name__=='__main__':
 main()
```
