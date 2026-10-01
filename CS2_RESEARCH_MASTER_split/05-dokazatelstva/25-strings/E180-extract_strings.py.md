<!-- split-part | CS2_RESEARCH_MASTER.md lines 100488-101029 | body-sha256 9aaf9c8d239b59a34148d234433c5efe110d7782ac54e12f536a73ee8e09467e -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-180"></a>

## E180. `analysis/strings/extract_strings.py`

Bytes: 42748. SHA-256: `d37a56da8537efefde4190127f284012e6d25ee1a892972ed40f0184fb4ca6dc`.

```python
#!/usr/bin/env python3
"""Read-only static byte/string catalog; all generated files stay beside this script."""

import bisect
import collections
import csv
import hashlib
import json
import math
import pathlib
import re
import struct
import sys

sys.dont_write_bytecode = True
ROOT = pathlib.Path(__file__).resolve().parent
SOURCE = ROOT.parent / "input" / "cs2_212C3300000.bin"
EXPECTED_SHA256 = "3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27"
BASE = 0x212C3300000
EXPECTED_SIZE = 0x5001000
ENCODINGS = ("ascii", "utf-16le", "utf-16be", "utf-32le", "utf-32be")
TOPIC_TERMS = {
    "rage_aim": ["ragebot", "rage bot", "rage", "aimbot", "aim bot", "aim", "aim assistance", "silent aim", "silent_aim", "aim_target", "get_target", "target_ent_index", "shoot_position", "viewangles", "view_angles", "target_head_pos_check", "target_abs_pos_check", "target_abs_ang_check"],
    "hitchance_spread": ["hitchance", "hit chance", "hit_chance", "hit-chance", "spread", "inaccuracy", "accuracy", "nospread", "no_spread", "random_seed", "recoil", "weapon_accuracy_nospread"],
    "damage_penetration": ["damage", "min damage", "minimum damage", "minimum_damage", "min_damage", "mindamage", "penetration", "penetrate", "penetrating", "autowall", "auto wall", "wallbang", "armor", "armour", "hitgroup", "hit_group", "is_predicting_body_shot_fx", "is_predicting_head_shot_fx"],
    "lag_history_records": ["lag compensation", "lag_compensation", "lagcompensation", "lagcomp", "lag_comp", "backtrack", "back track", "back_track", "backtracking", "history", "input_history", "records", "record", "lag_record", "lagrecord", "simulation_time", "simulationtime", "simtime", "rewind", "interpolation", "interp", "cl_interp", "sv_unlag", "sv_maxunlag", "render_tick_count", "player_tick_count"],
    "resolver_multipoint": ["resolver", "resolve", "multipoint", "multi point", "multi_point", "multi-point", "pointscale", "point_scale", "point scale", "hitbox", "hitboxes", "safe point", "safe_point", "safepoint", "body aim", "body_aim", "baim", "prefer body"],
    "antiaim": ["antiaim", "anti aim", "anti_aim", "anti-aim", "desync", "freestanding", "freestand", "fake yaw", "fake_yaw", "pitch", "yaw", "jitter", "lowerbody", "lower_body", "lby", "bodyyaw", "body_yaw", "fakelag", "fake lag", "fake_lag", "choke", "choked", "sendpacket", "send_packet"],
    "rapidfire_defensive_tickbase": ["rapidfire", "rapid fire", "rapid_fire", "rapid-fire", "doubletap", "double tap", "double_tap", "double-tap", "defensive", "tickbase", "tick base", "tick_base", "tickshift", "tick_shift", "shift_ticks", "hide shots", "hide_shots", "hideshots", "exploit", "recharge", "prediction_offset_ticks_x256", "client_tick", "tickcount", "tick_count"],
    "movement": ["movement", "autostop", "auto stop", "auto_stop", "quickstop", "quick_stop", "slowwalk", "slow walk", "slow_walk", "autostrafe", "auto strafe", "auto_strafe", "airstrafe", "air_strafe", "strafe", "bunnyhop", "bunny hop", "bunny_hop", "bhop", "duck", "jump", "fakeduck", "fake duck", "edgejump", "edge_jump", "velocity", "onground", "on_ground", "forwardmove", "leftmove", "sidemove", "upmove", "subtick", "analog_forward_delta", "analog_left_delta", "buttonstate", "CSubtickMoveStep", "CCSPlayer_MovementServices"],
    "settings_ui": ["keybind", "hotkey", "checkbox", "combobox", "slider", "menu", "override", "overrides", "override_damage", "override_hitchance", "set_value", "get_value", "ui.find", "ui.get", "ui.set", "ui.reference", "weapon_config", "configuration", "settings"],
    "lua_api": ["LuaJIT", "luaJIT_BC_", "lua_", "luaL_", "luaopen_", "luabridge", "sol::", "jit.opt", "jit.util", "LUA_PATH", "LUA_CPATH", "LUA_NOENV", "lua_debug", "set_callback", "register_callback", "createmove", "create_move", "get_target", "get_hitbox", "trace_ray", "trace_bullet", "get_inaccuracy", "get_spread", "get_simulation_time", "get_tickbase", "entity_list", "globals.tickcount", "rage.antiaim"],
    "provenance": ["neverlose", "neverlose_cs2.dll", ".pdb", "RSDS", "NB10", "FileVersion", "ProductVersion", "FileDescription", "ProductName", "OriginalFilename", "BuildVersion", "build_date", "build_time", "git_commit", "git_revision", "compiled on", "__DATE__", "__TIME__"],
}
FEATURE_TOPICS = tuple(TOPIC_TERMS)[:8]
RTTI_PATTERN = re.compile(r"^\.\?A[UVT].*@@$")
SOURCE_PATTERN = re.compile(r"[A-Za-z]:[\\/].*\.(?:cpp|hpp|cxx|cc|c|pdb)(?:$|\s)", re.I)
LIBRARY_PATTERN = re.compile(r"LuaJIT|^OpenSSL 1\.|(?:libcurl|curl|Crypto\+\+|zlib|zstd|FreeType)[ /-]+[0-9]|Microsoft \(R\) HLSL Shader Compiler", re.I)
LUA_PATTERN = re.compile(r"\blua(?:jit|_|L_)|\bjit\.(?:opt|util)|mp_(?:encode|decode)_.*lua|\\lua\\|\.lua(?:;|$)|\b(?:ffi|cdef|metatype|gcinfo|getfenv|setfenv|pcall|xpcall|rawget|rawset|rawequal|collectgarbage|newproxy)\b", re.I)
ENGINE_PATTERN = re.compile(r"^(?:C(?:CS|_CS|Player_|_Player|SGO|_Base|_World)|m_(?:fl|vec|ang|nTick|weapon|body))|^(?:client|engine2|schemasystem|panoramauiclient|tier0|vstdlib|inputsystem)\.dll$")
TOPIC_PATTERNS = {topic: re.compile("|".join(re.escape(term) for term in sorted(terms, key=len, reverse=True)), re.I) for topic, terms in TOPIC_TERMS.items()}
ANCHOR_NAMES = [
    "CCSPlayer_MovementServices", "CSGOInterpolationInfo", "CPlayer_WeaponServices", "spread",
    "CBaseUserCmdPB", "CSubtickMoveStep", "prediction_offset_ticks_x256", "subtick_moves",
    "client_tick", "consumed_server_angle_changes", "random_seed", "analog_forward_delta", "analog_left_delta", "pitch_delta", "yaw_delta",
    "CSGOInputHistoryEntryPB", "CSGOUserCmdPB", "input_history", "attack1_start_history_index", "attack2_start_history_index",
    "render_tick_count", "render_tick_fraction", "player_tick_count", "player_tick_fraction", "cl_interp", "sv_interp0", "sv_interp1", "player_interp",
    "target_ent_index", "shoot_position", "target_head_pos_check", "target_abs_pos_check", "target_abs_ang_check",
    "is_predicting_body_shot_fx", "is_predicting_head_shot_fx", "LuaJIT 2.1.1774946682", "neverlose_cs2.dll",
]
SUMMARY_NAMES = ["CCSPlayer_MovementServices", "CSGOInterpolationInfo", "spread", "CSubtickMoveStep", "CBaseUserCmdPB", "prediction_offset_ticks_x256", "subtick_moves", "consumed_server_angle_changes", "random_seed", "CSGOInputHistoryEntryPB", "input_history", "attack1_start_history_index", "attack2_start_history_index", "shoot_position", "target_ent_index", "target_head_pos_check", "cl_interp", "player_tick_fraction", "LuaJIT 2.1.1774946682", "neverlose_cs2.dll"]


def hx(value):
    return f"0x{value:08X}"


def address_fields(offset, length):
    return {"file_offset": offset, "file_offset_hex": hx(offset), "rva": offset, "rva_hex": hx(offset), "va": BASE + offset, "va_hex": hx(BASE + offset), "byte_length": length, "end_offset_exclusive_hex": hx(offset + length)}


def redact(text):
    reasons = []
    replacements = [
        (r"(?i)(\\builds\\)[^\\]+", r"\1[RUNNER-ID REDACTED]", "ci_runner_identifier"),
        (r"(?i)\bBearer\s+(?!%)[A-Za-z0-9._~+/-]{12,}=*", "Bearer [REDACTED]", "bearer_value"),
        (r"\beyJ[A-Za-z0-9_-]{8,}\.[A-Za-z0-9_-]{8,}\.[A-Za-z0-9_-]{8,}\b", "[REDACTED JWT]", "jwt"),
        (r"\b(?:gh[pousr]_|github_pat_|sk_live_|sk_test_|xox[baprs]-|AKIA)[A-Za-z0-9_-]{12,}\b", "[REDACTED CREDENTIAL]", "credential_pattern"),
        (r"(?i)((?:api[_-]?key|access[_-]?token|refresh[_-]?token|password|client[_-]?secret|authorization|secret|token)\s*[:=]\s*[\"']?)(?!%|\[|<)[A-Za-z0-9._~+/=-]{12,}", r"\1[REDACTED]", "assigned_secret_candidate"),
        (r"(?i)(https?://)[^\s/@:]+:[^\s/@]+@", r"\1[REDACTED USERINFO]@", "url_userinfo"),
        (r"(?i)([?&](?:token|key|secret|password|auth|signature|sig)=)[^&\s\"']+", r"\1[REDACTED]", "url_secret_parameter"),
    ]
    for pattern, replacement, reason in replacements:
        text, count = re.subn(pattern, replacement, text)
        if count:
            reasons.append(reason)
    if re.fullmatch(r"[A-Za-z0-9_+/=-]{40,}", text):
        frequencies = collections.Counter(text)
        entropy = -sum((count / len(text)) * math.log2(count / len(text)) for count in frequencies.values())
        if entropy > 4.6 and any(character.isdigit() for character in text):
            text = "[REDACTED OPAQUE HIGH-ENTROPY STRING]"
            reasons.append("opaque_value")
    if "PRIVATE KEY-----" in text:
        text = "[REDACTED PRIVATE KEY MARKER]"
        reasons.append("private_key")
    return text, reasons


def save_json(name, value):
    (ROOT / name).write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def save_csv(name, rows, columns):
    with (ROOT / name).open("w", encoding="utf-8", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=columns, extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            output = {}
            for column in columns:
                value = row.get(column, "")
                if isinstance(value, (dict, list)):
                    value = json.dumps(value, ensure_ascii=False)
                if isinstance(value, str) and value.startswith(("=", "+", "-", "@")):
                    value = "'" + value
                output[column] = value
            writer.writerow(output)


def extract_runs(blob):
    records = []
    counts = {}
    patterns = [("ascii", rb"[\x20-\x7e]{4,}"), ("utf-16le", rb"(?:[\x20-\x7e]\x00){4,}"), ("utf-16be", rb"(?:\x00[\x20-\x7e]){4,}"), ("utf-32le", rb"(?:[\x20-\x7e]\x00\x00\x00){4,}"), ("utf-32be", rb"(?:\x00\x00\x00[\x20-\x7e]){4,}")]
    for encoding, pattern in patterns:
        count = 0
        for match in re.finditer(pattern, blob):
            text = match.group().decode(encoding)
            records.append({"offset": match.start(), "length": match.end() - match.start(), "text": text, "encoding": encoding})
            count += 1
        counts[encoding] = count
    records.sort(key=lambda record: (record["offset"], record["encoding"]))
    return records, counts


def varint(blob, offset, end):
    value = 0
    shift = 0
    while offset < end and shift < 70:
        byte = blob[offset]
        offset += 1
        value |= (byte & 127) << shift
        if byte < 128:
            return value, offset
        shift += 7
    raise ValueError("invalid varint")


def wire_field(blob, offset, end):
    start = offset
    tag, offset = varint(blob, offset, end)
    number, wire = tag >> 3, tag & 7
    if number == 0 or wire not in (0, 1, 2, 5):
        raise ValueError("invalid wire field")
    if wire == 0:
        value_start = offset
        value, offset = varint(blob, offset, end)
    else:
        if wire == 2:
            length, offset = varint(blob, offset, end)
        else:
            length = 8 if wire == 1 else 4
        value_start = offset
        offset += length
        if offset > end:
            raise ValueError("truncated field")
        value = blob[value_start:offset]
    return {"number": number, "wire": wire, "start": start, "value_start": value_start, "end": offset, "value": value}, offset


def wire_fields(blob, start, end):
    fields = []
    while start < end:
        field, start = wire_field(blob, start, end)
        fields.append(field)
    return fields


def parse_command_schemas(blob):
    regions = []
    names = {}
    schema_fields = []
    protobuf_types = {1: "double", 2: "float", 3: "int64", 4: "uint64", 5: "int32", 6: "fixed64", 7: "fixed32", 8: "bool", 9: "string", 10: "group", 11: "message", 12: "bytes", 13: "uint32", 14: "enum", 15: "sfixed32", 16: "sfixed64", 17: "sint32", 18: "sint64"}

    def text_field(field):
        return field["value"].decode("ascii")

    def visit_message(field, filename, parent=""):
        children = wire_fields(blob, field["value_start"], field["end"])
        message_field = next(child for child in children if child["number"] == 1 and child["wire"] == 2)
        message_name = text_field(message_field)
        owner = parent + "." + message_name if parent else message_name
        names[message_field["value_start"]] = {"role": "message", "owner": owner, "proto_file": filename, "text": message_name, "length": len(message_field["value"])}
        for child in children:
            if child["number"] == 3 and child["wire"] == 2:
                visit_message(child, filename, owner)
            if child["number"] != 2 or child["wire"] != 2:
                continue
            members = wire_fields(blob, child["value_start"], child["end"])
            indexed = {member["number"]: member for member in members}
            name_field = indexed[1]
            field_name = text_field(name_field)
            field_number = indexed[3]["value"]
            label = {1: "optional", 2: "required", 3: "repeated"}.get(indexed.get(4, {}).get("value"), "unspecified")
            field_type = protobuf_types.get(indexed.get(5, {}).get("value"), "unknown")
            type_name = text_field(indexed[6]) if 6 in indexed else ""
            record = {"proto_file": filename, "owner": owner, "field": field_name, "field_number": field_number, "label": label, "protobuf_type": field_type, "type_name": type_name, **address_fields(name_field["value_start"], len(name_field["value"]))}
            schema_fields.append(record)
            names[name_field["value_start"]] = {"role": "field", "owner": owner, "proto_file": filename, "text": field_name, "length": len(name_field["value"]), "field_number": field_number, "protobuf_type": field_type, "label": label}

    for filename in ("usercmd.proto", "cs_usercmd.proto"):
        encoded = filename.encode("ascii")
        marker = b"\x0a" + bytes([len(encoded)]) + encoded
        offset = blob.find(marker)
        if offset < 0:
            continue
        cursor = offset
        top = []
        limit = min(offset + 65536, len(blob))
        while cursor < limit:
            try:
                field, next_cursor = wire_field(blob, cursor, limit)
            except ValueError:
                break
            if field["number"] not in range(1, 15):
                break
            top.append(field)
            cursor = next_cursor
        if not top or top[0]["value"] != encoded:
            raise ValueError("descriptor name mismatch")
        for field in top:
            if field["number"] == 4 and field["wire"] == 2:
                visit_message(field, filename)
        regions.append({"proto_file": filename, "start": offset, "end": cursor, "start_hex": hx(offset), "end_exclusive_hex": hx(cursor), "top_level_fields": len(top)})
    return regions, names, schema_fields


def shader_regions(blob):
    regions = []
    for match in re.finditer(b"DXBC", blob):
        start = match.start()
        if start + 32 > len(blob):
            continue
        size, count = struct.unpack_from("<II", blob, start + 24)
        if not (32 <= size <= 0x1000000 and 0 < count <= 128 and start + size <= len(blob) and 32 + 4 * count <= size):
            continue
        chunks = []
        valid = True
        for index in range(count):
            relative = struct.unpack_from("<I", blob, start + 32 + index * 4)[0]
            chunk_start = start + relative
            if relative < 32 + count * 4 or chunk_start + 8 > start + size:
                valid = False
                break
            fourcc = blob[chunk_start:chunk_start + 4]
            chunk_size = struct.unpack_from("<I", blob, chunk_start + 4)[0]
            if chunk_start + 8 + chunk_size > start + size or not re.fullmatch(rb"[A-Z0-9 ]{4}", fourcc):
                valid = False
                break
            chunks.append({"fourcc": fourcc.decode("ascii"), "start": chunk_start, "end": chunk_start + 8 + chunk_size})
        if valid:
            regions.append({"start": start, "end": start + size, "size": size, "chunks": chunks})
    return regions


def enclosing(offset, regions, starts):
    position = bisect.bisect_right(starts, offset) - 1
    if position >= 0 and regions[position]["start"] <= offset < regions[position]["end"]:
        return regions[position]
    return None


def topic_matches(text, offset, encoding):
    result = []
    for topic, pattern in TOPIC_PATTERNS.items():
        for match in pattern.finditer(text):
            prefix_length = len(text[:match.start()].encode(encoding))
            result.append({"topic": topic, "term": match.group(), **address_fields(offset + prefix_length, len(match.group().encode(encoding)))})
    return result


def classify(text, offset, schema_regions, schema_names, shaders, shader_starts):
    if offset in schema_names:
        info = schema_names[offset]
        return "protobuf_schema", "high", f"{info['proto_file']} / {info['owner']}: {info['role']}; сериализация/структура команды, не доказательство feature-реализации."
    if any(region["start"] <= offset < region["end"] for region in schema_regions):
        return "protobuf_schema_context", "high", "Строка внутри побайтово разобранного FileDescriptorProto команды."
    shader = enclosing(offset, shaders, shader_starts)
    if shader:
        return "shader_reflection", "high", f"Внутри валидированного DXBC {hx(shader['start'])}; records/prediction/spread здесь нельзя приписывать lag compensation."
    if RTTI_PATTERN.fullmatch(text):
        return "rtti_type_name", "high", "Декорированное MSVC-имя типа; наличие типа не подтверждает выполнение соответствующего кода."
    if SOURCE_PATTERN.search(text):
        return "build_source_path", "high", "Прямой путь исходника/сборки; зависимости не определяют версию конечного продукта."
    if LIBRARY_PATTERN.search(text):
        return "third_party_version", "high", "Версия стороннего компонента/компилятора, не версия продукта."
    if "neverlose" in text.lower():
        return "product_or_asset_identity", "high", "Прямое имя DLL/ассета/проекта; само по себе не доказывает подлинность или номер релиза."
    if LUA_PATTERN.search(text):
        return "lua_runtime_or_library", "high", "Lua/LuaJIT/MessagePack runtime evidence; пользовательский API rage/antiaim этим не подтверждается."
    if ENGINE_PATTERN.search(text):
        return "engine_name_candidate", "medium", "Имя игрового типа/поля/модуля; связь с функцией требует отдельного xref, здесь не выполнялась."
    if 0xDED000 <= offset < 0xE00000:
        return "protocol_or_library_metadata", "medium", "Область protobuf/протокольных метаданных; не считать UI либо игровой feature-реализацией."
    if re.search(r"ssl|tls|dane|x509|dtls|TS_|TS_|record (?:overflow|boundary|length|type|mac)|record_padding|recordpadding|id-smime|DNS|\baRecord\b|\bmXRecord\b|\bnSRecord\b|\bcNAMERecord\b|\bsOARecord\b|No data record|maxrecord|BIO_|RAND_|prediction resistance|bignum|OCSP", text, re.I):
        return "third_party_noise", "high", "Криптография/TLS/DNS/библиотечная терминология; совпадение не является game-feature evidence."
    if text in ("spread", "head", "pitch", "yaw", "records", "record", "aim", "damage", "settings", "menu"):
        return "ambiguous_bare_string", "low", "Короткое имя без контекста реализации; UI/API/алгоритм по одной строке не различимы."
    if "spreadMethod" in text or "SVG" in text or "<svg" in text or "spreadshee" in text:
        return "graphics_or_font_noise", "high", "SVG/графическая/шрифтовая терминология, не доказательство weapon spread."
    if 0x12B4000 <= offset < 0x12C0000:
        return "particle_asset_metadata", "medium", "Область встроенных particle-ассетов; имена hitboxes/velocity не доказывают aim/movement implementation."
    if len(text) <= 12 and not re.fullmatch(r"[A-Za-z][A-Za-z0-9_ .-]*", text):
        return "incidental_binary_fragment", "low", "Короткий печатный фрагмент бинарных данных; не считать осмысленным именем."
    return "unclassified_text", "low", "Точное текстовое совпадение без установленной роли; UI и реализация не подтверждены."


def keyword_coverage(blob):
    lowered = blob.lower()
    output = []
    for topic, terms in TOPIC_TERMS.items():
        for term in terms:
            per_encoding = {}
            for encoding in ENCODINGS:
                needle = term.lower().encode(encoding)
                count = lowered.count(needle)
                positions = []
                cursor = 0
                while count and len(positions) < 500:
                    offset = lowered.find(needle, cursor)
                    if offset < 0:
                        break
                    positions.append(hx(offset))
                    cursor = offset + len(needle)
                per_encoding[encoding] = {"count": count, "rvas": positions, "positions_truncated": count > len(positions)}
            output.append({"topic": topic, "term": term, "encodings": per_encoding})
    return output


def xor_diagnostic(blob):
    delta = bytes(left ^ right for left, right in zip(blob, blob[1:]))
    words = ["ragebot", "aimbot", "hitchance", "multipoint", "antiaim", "rapidfire", "defensive", "tickbase", "backtrack", "resolver", "penetration", "lagcompensation", "minimum_damage", "doubletap", "neverlose"]
    candidates = {}
    for word in words:
        for form in dict.fromkeys((word, word.title(), word.upper())):
            needle = form.encode("ascii")
            signature = bytes(left ^ right for left, right in zip(needle, needle[1:]))
            cursor = 0
            while True:
                offset = delta.find(signature, cursor)
                if offset < 0:
                    break
                cursor = offset + 1
                key = blob[offset] ^ needle[0]
                original = blob[offset:offset + len(needle)]
                if not key or original.lower() == needle.lower():
                    continue
                identifier = (offset, key)
                candidates[identifier] = {"decoded_text": form, "xor_key_hex": f"0x{key:02X}", "encoding": "ascii_single_byte_xor_candidate", "interpretation": "Только точное совпадение после XOR; не доказательство общего шифрования строк либо роли в программе.", **address_fields(offset, len(needle))}
    return {"tested_words": words, "forms": ["lowercase", "Titlecase", "UPPERCASE"], "method": "Adjacent-byte XOR invariant, with original-byte verification; ignore ordinary case-only XOR 0x20 aliases.", "candidates": sorted(candidates.values(), key=lambda candidate: candidate["file_offset"])}


def markdown_escape(text):
    return text.replace("|", "\\|").replace("`", "\\`").replace("\n", " ")


def table(rows, include_class=True):
    header = "| Строка | RVA = file offset | VA |" + (" Роль |" if include_class else "")
    separator = "|---|---|---|" + ("---|" if include_class else "")
    lines = [header, separator]
    for record in rows:
        line = f"| `{markdown_escape(record['text'])}` | `{record['rva_hex']}` | `{record['va_hex']}` |"
        if include_class:
            line += f" {record['classification']} |"
        lines.append(line)
    return "\n".join(lines)


def main():
    if ROOT.name != "strings" or ROOT.parent.name != "analysis":
        raise SystemExit("Run only from the authorized analysis/strings/ directory")
    blob = SOURCE.read_bytes()
    digest = hashlib.sha256(blob).hexdigest()
    if digest != EXPECTED_SHA256 or len(blob) != EXPECTED_SIZE:
        raise SystemExit("Input size/hash mismatch; no outputs written")
    runs, run_counts = extract_runs(blob)
    schema_regions, schema_names, schema_fields = parse_command_schemas(blob)
    shaders = shader_regions(blob)
    shader_starts = [region["start"] for region in shaders]
    run_offsets = [record["offset"] for record in runs]
    selected = {}
    seeds = []
    for index, record in enumerate(runs):
        text = record["text"]
        offset = record["offset"]
        tags = [topic for topic, pattern in TOPIC_PATTERNS.items() if pattern.search(text)]
        if RTTI_PATTERN.fullmatch(text):
            tags.append("rtti")
        if SOURCE_PATTERN.search(text):
            tags.append("build_paths")
        if LIBRARY_PATTERN.search(text):
            tags.append("dependency_versions")
        if LUA_PATTERN.search(text):
            tags.append("lua_runtime")
        if ENGINE_PATTERN.search(text):
            tags.append("engine_names")
        if any(region["start"] <= offset < region["end"] for region in schema_regions):
            tags.append("command_schema")
        if not tags:
            continue
        item = dict(record)
        item["categories"] = sorted(set(tags))
        item["context_for_rvas"] = []
        selected[(offset, record["encoding"])] = item
        if any(topic in tags for topic in FEATURE_TOPICS) or "build_paths" in tags or "lua_runtime" in tags or "engine_names" in tags:
            seeds.append(index)
    for index in seeds:
        seed = runs[index]
        if len(seed["text"]) > 4000:
            continue
        for nearby_index in range(max(0, index - 2), min(len(runs), index + 3)):
            nearby = runs[nearby_index]
            if nearby["encoding"] != seed["encoding"] or abs(nearby["offset"] - seed["offset"]) > 160:
                continue
            key = (nearby["offset"], nearby["encoding"])
            if key not in selected:
                selected[key] = {**nearby, "categories": ["local_text_context"], "context_for_rvas": []}
            if nearby_index != index:
                selected[key]["context_for_rvas"].append(hx(seed["offset"]))
    for offset, info in schema_names.items():
        key = (offset, "ascii")
        if key not in selected:
            selected[key] = {"offset": offset, "length": info["length"], "text": info["text"], "encoding": "ascii", "categories": ["command_schema"], "context_for_rvas": []}
    evidence = []
    verification_errors = []
    for item in sorted(selected.values(), key=lambda record: (record["offset"], record["encoding"])):
        offset, original, encoding = item["offset"], item["text"], item["encoding"]
        exact_bytes = original.encode(encoding)
        if blob[offset:offset + item["length"]] != exact_bytes:
            verification_errors.append(hx(offset))
        classification, confidence, note = classify(original, offset, schema_regions, schema_names, shaders, shader_starts)
        text, reasons = redact(original)
        matches = topic_matches(original, offset, encoding)
        matches = [{**match, "term": redact(match["term"])[0]} for match in matches]
        record = {"id": f"E{len(evidence) + 1:05d}", **address_fields(offset, item["length"]), "encoding": encoding, "text": text, "character_length_original": len(original), "categories": item["categories"], "classification": classification, "confidence_in_classification": confidence, "note": note, "redacted": bool(reasons), "redaction_reasons": reasons, "keyword_matches": matches, "context_for_rvas": sorted(set(item["context_for_rvas"]))}
        if offset in schema_names:
            record["schema"] = {key: value for key, value in schema_names[offset].items() if key not in ("text", "length")}
        shader = enclosing(offset, shaders, shader_starts)
        if shader:
            record["dxbc_container_rva"] = hx(shader["start"])
        evidence.append(record)
    if verification_errors:
        raise SystemExit("Byte span verification failed: " + str(verification_errors))
    coverage = keyword_coverage(blob)
    xor = xor_diagnostic(blob)
    markers = {}
    for name, marker in [("RSDS", b"RSDS"), ("NB10", b"NB10"), ("VS_FIXEDFILEINFO_signature", b"\xbd\x04\xef\xfe")]:
        offsets = [match.start() for match in re.finditer(re.escape(marker), blob)]
        markers[name] = {"count": len(offsets), "rvas": [hx(offset) for offset in offsets]}
    metadata = {
        "source": "analysis/input/cs2_212C3300000.bin", "sha256": digest, "size_bytes": len(blob), "size_hex": hx(len(blob)), "base_va_hex": hx(BASE),
        "address_model": "User-supplied linear memory image: file offset = RVA, VA = 0x212C3300000 + file offset. This is not a PE disk-file RVA conversion. End offsets are exclusive; addresses point at the first character byte, not NUL terminators or RTTI headers.",
        "scope": "Static strings/data only. Target never loaded or executed. No disassembly, xrefs, function ownership, network lookups, code.sqlite or Ghidra access. Writes only beside this script.",
        "extraction": "Maximal runs of ASCII-range printable characters (0x20..0x7e), minimum 4 characters, ASCII/UTF-16LE/BE/UTF-32LE/BE. Unaligned byte starts allowed. Latin ASCII UTF-8 keywords are covered by ASCII scan; full non-ASCII UTF-8/UTF-16 text is not exhaustively decoded. No Unicode normalization or decrypted/packed-string recovery beyond the documented limited XOR diagnostic.",
        "coverage_semantics": "Case-insensitive byte substring counts, not semantic matches. Very short words can match random bytes or longer unrelated words. Counts for LE/BE can be overlapping shifted interpretations, not independent strings. Keyword positions are capped at 500 per term/encoding with an explicit truncation flag; evidence itself is not capped.",
        "selection": "Topic terms, all MSVC RTTI-looking names, source/build paths, explicit dependency versions, Lua runtime names, engine names, both command protobuf schemas, and up to two neighboring same-encoding runs within 160 bytes of feature/build/Lua/engine seeds.",
        "redaction": "Runner identifiers, common credential/JWT/bearer/assigned-secret patterns and opaque high-entropy token candidates are redacted before output; original addresses/byte lengths retained. No unfiltered strings dump is saved. Redaction is heuristic, not a claim that every value can be recognized.",
        "csv": "UTF-8, columns with arrays/dicts use JSON; leading =,+,-,@ cells are apostrophe-escaped for spreadsheet safety. JSON is the canonical exact sanitized representation.",
        "raw_printable_run_counts": run_counts, "evidence_count": len(evidence), "classification_counts": dict(collections.Counter(record["classification"] for record in evidence)), "category_counts": dict(collections.Counter(topic for record in evidence for topic in record["categories"])),
        "schema_regions": schema_regions, "schema_field_count": len(schema_fields), "validated_dxbc_count": len(shaders), "codeview_and_version_markers": markers,
        "redacted_record_count": sum(record["redacted"] for record in evidence), "byte_span_verification": {"checked_records": len(evidence), "failed_records": len(verification_errors)},
    }
    columns = ["id", "file_offset_hex", "rva_hex", "va_hex", "file_offset", "rva", "va", "byte_length", "end_offset_exclusive_hex", "encoding", "text", "classification", "confidence_in_classification", "categories", "note", "redacted", "redaction_reasons", "schema", "dxbc_container_rva", "context_for_rvas", "keyword_matches"]
    anchors = []
    for name in ANCHOR_NAMES:
        for record in evidence:
            if record["text"] == name and record["encoding"] == "ascii":
                anchors.append(record)
    rtti = [record for record in evidence if record["classification"] == "rtti_type_name"]
    save_json("evidence.json", {"metadata": metadata, "evidence": evidence})
    save_csv("evidence.csv", evidence, columns)
    save_json("coverage.json", {"semantics": metadata["coverage_semantics"], "queries": coverage})
    save_csv("coverage.csv", [{"topic": item["topic"], "term": item["term"], "encoding": encoding, **details} for item in coverage for encoding, details in item["encodings"].items()], ["topic", "term", "encoding", "count", "rvas", "positions_truncated"])
    save_json("anchors.json", {"warning": "Text-only candidates: schema names often lead to generated protobuf metadata/serialization, not feature implementation. No function ownership or xrefs were computed.", "anchors": anchors})
    save_csv("anchors.csv", anchors, columns)
    save_csv("rtti.csv", rtti, columns)
    save_csv("schema_fields.csv", schema_fields, ["proto_file", "owner", "field", "field_number", "label", "protobuf_type", "type_name", "file_offset_hex", "rva_hex", "va_hex", "byte_length"])
    save_json("xor_probe.json", xor)
    save_json("metadata.json", metadata)
    (ROOT / "anchors.md").write_text("# Anchors для отдельного xref/decompile прохода\n\nПервые строки вне protobuf-дескрипторов; ниже — точные поля и типы команд. Это не адреса функций. Наличие UI/API/реализации из совпадения не следует.\n\n" + table(anchors) + "\n", encoding="utf-8")
    catalog = ["# Каталог текстовых свидетельств", "", "Адреса каждого совпадения — в evidence.json/csv. Категории могут пересекаться. local_text_context означает только близость в файле, не code reference. Поле keyword_matches хранит адрес подстроки отдельно от начала полного printable-run.", ""]
    for topic in [*TOPIC_TERMS, "command_schema", "rtti", "build_paths", "dependency_versions", "lua_runtime", "engine_names"]:
        subset = [record for record in evidence if topic in record["categories"]]
        catalog.extend([f"## {topic} ({len(subset)})", "", table(subset) if subset else "Открытых строк в выбранных представлениях не найдено.", ""])
    (ROOT / "catalog.md").write_text("\n".join(catalog), encoding="utf-8")
    summary_anchors = []
    for name in SUMMARY_NAMES:
        record = next((record for record in anchors if record["text"] == name), None)
        if record:
            summary_anchors.append(record)
    direct_absent = ["ragebot", "aimbot", "hitchance", "multipoint", "antiaim", "rapidfire", "defensive", "tickbase", "backtrack", "resolver", "penetration", "autowall", "inaccuracy", "doubletap"]
    absent_verified = [term for term in direct_absent if all(not any(details["count"] for details in item["encodings"].values()) for item in coverage if item["term"] == term)]
    summary = f"""# Статический каталог строк raw x64 дампа

## Короткие наблюдения

- Проверен SHA-256 `{digest}`, размер `{hx(len(blob))}`, base `{hx(BASE)}`. **File offset = RVA; VA = base + RVA** по модели предоставленного линейного memory dump. Бинарник не запускался; код/xrefs/SQLite/Ghidra не анализировались.
- Происхождение: восемь путей исходников зависимостей содержат `neverlose\\neverlose-cs2`, имеется `neverlose_cs2.dll` при `0x03A4238D`, плюс particle-ассеты `particles/neverlose/...`. Это несколько согласованных прямых свидетельств названия проекта/артефакта Neverlose CS2; не независимая проверка подлинности. CI runner identifier скрыт.
- **Точный релиз/номер сборки/дата продукта не установлены.** `LuaJIT 2.1.1774946682` (`0x00F83078`), `OpenSSL 1.1.1t  7 Feb 2023` (`0x00E03720`), `Microsoft (R) HLSL Shader Compiler 10.1` — версии компонентов, не продукта. Число в LuaJIT не переинтерпретируется как дата сборки продукта.
- Открытым текстом в пяти ASCII-совместимых представлениях не найдены: {', '.join('`' + term + '`' for term in absent_verified)}. Другие варианты написания и полные counts — `coverage.json`. Отсутствие строк не доказывает отсутствие функций; обфускация/шифрование не установлены.
- **Подтверждённых UI-названий или имён реализации перечисленных cheat-features нет.** Есть точные схемы `usercmd.proto` и `cs_usercmd.proto`: input history, interpolation, target/shoot position, subtick movement, prediction offset, random seed. Поля схемы не являются доказательством backtrack/resolver/rapidfire. Их точные protobuf field numbers/types сохранены в `schema_fields.csv`.
- Наиболее специфичные непроtobuf anchors: `CCSPlayer_MovementServices` (`0x00F4EB78`), `CPlayer_WeaponServices` (`0x00F4EB93`), `CSGOInterpolationInfo` (`0x00F12B80`). Это имена типов/метаданных; роли вызывающих функций неизвестны.
- `spread` при `0x00F58EC8` — неоднозначное короткое имя. `spreadMethod` и RTTI `SpreadMethod@lunasvg` относятся к SVG. `damagefilter` при `0x00F4EFB5` — bare engine/property candidate, не min-damage/penetration implementation.
- **Ложные lag-record anchors отсеяны:** `records` при `0x00ED2CE8` находится в DXBC с `TrajectoryConstants`; `records` при `0x00F07118` — DXBC `TracerConstants`/`TracerRecord`. Криптографические TLS `record`/`accuracy` тоже не игровые алгоритмы. Проверено {len(shaders)} структурно валидных DXBC-контейнеров только по байтам/заголовкам.
- Lua подтверждается строками LuaJIT, `jit.opt`, `jit.util`, `LUA_PATH`, `LUA_CPATH`, MessagePack Lua diagnostics. Конкретного публичного Lua API `rage/antiaim/hitchance` не найдено. Библиотечные callbacks/FFI не считать feature API.
- Найдено {len(rtti)} RTTI-looking MSVC имён; полный список — `rtti.csv`. Среди отобранных RTTI нет конкретных имён перечисленных cheat-features; преобладают protobuf, CryptoPP, std/Boost и lunasvg. `.pdb`, точные CodeView `RSDS`/`NB10`, строки FileVersion/ProductVersion и сигнатура VS_FIXEDFILEINFO не найдены в проверенных представлениях.
- Ограниченный single-byte XOR probe не дал anchors основных feature-слов; два расположения имени Neverlose имеют точное XOR-совпадение, но без контекста это лишь кандидаты (`xor_probe.json`), не установленная схема дешифрования.

## 20 ключевых anchors

{table(summary_anchors)}

## Результаты и воспроизводимость

- `extract_strings.py` — stdlib-only скрипт; запуск из корня sandbox: `python3 -B analysis/strings/extract_strings.py`.
- `evidence.json`, `evidence.csv` — {len(evidence)} записей, точные byte spans/offset/RVA/VA, категории, роль, redaction, локальный контекст и адреса keyword-подстрок. JSON является основным машиночитаемым форматом.
- `anchors.md`, `anchors.json`, `anchors.csv` — компактная подборка для основного агента, без вычисления xrefs.
- `schema_fields.csv` — {len(schema_fields)} полей двух protobuf-схем с типами/номерами/владельцами, извлечённых статическим wire-format разбором.
- `catalog.md` — сгруппированный текстовый каталог; `rtti.csv` — RTTI; `coverage.json/csv` — позитивные и негативные поиски по всем вариантам; `xor_probe.json` — ограниченный XOR probe; `metadata.json` — методика и статистика.
- Проверены байты всех {len(evidence)} spans; ошибок {len(verification_errors)}. {metadata['redacted_record_count']} записей редактированы для защиты потенциально чувствительных значений; offsets/исходные длины сохранены. Полный неотфильтрованный strings dump не записывался.

## Ограничения

Отбираются ASCII-range printable runs длиной от 4, в том числе в wide encodings, а не все Unicode тексты. Короткие слова `aim`, `lag`, `record`, `resolve` могут встречаться случайно/в библиотеках. Подстроки и соседство не устанавливают достижимость кода, владельца функции, интерфейс Lua/UI или фактическую работу feature. Никаких алгоритмов ragebot/lag compensation по строкам не восстановлено. Нулевой заголовок raw dump не позволяет подменять supplied base автоматическим PE address mapping.
"""
    (ROOT / "summary.md").write_text(summary, encoding="utf-8")
    after = hashlib.sha256(SOURCE.read_bytes()).hexdigest()
    if after != digest:
        raise SystemExit("Input changed during analysis")
    print(json.dumps({"status": "ok", "sha256": digest, "evidence_records": len(evidence), "anchors": len(anchors), "rtti_names": len(rtti), "schema_fields": len(schema_fields), "validated_dxbc": len(shaders), "xor_candidates": len(xor["candidates"]), "redacted_records": metadata["redacted_record_count"], "classification_counts": metadata["classification_counts"]}, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
```
