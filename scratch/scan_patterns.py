"""Resolve every protection/patterns.cpp signature against the real game DLLs
the same way memory::resolve_pattern does (first match, op post-processing),
then flag any result landing inside the crash RVAs from mintaly_init.log.

Usage: py -3 scratch/scan_patterns.py
"""
import re
import struct
from pathlib import Path

WIN64 = Path(r'C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\csgo\bin\win64')
BIN64 = Path(r'C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\bin\win64')
SEARCH_DIRS = [WIN64, BIN64]
PATTERNS_CPP = Path(__file__).resolve().parents[1] / 'protection' / 'patterns.cpp'

# Crash windows (RVA) observed in logs:
#   FSN/menu      0x3E7E70..0x3E7F90  (fault 0x3E7F46: mov rax,[rcx]; call [rax+170])
#   prediction    0xA753D0..0xA75430  (fault 0xA753EF: mov rdx,[rbx+40], rbx=2)
#   present       0x1A50100..0x1A501A0 (fault 0x1A50153)
WINDOWS = [
    ('FSN',     0x3E7E70,   0x3E7F90),
    ('PRED',    0xA753D0,   0xA75430),
    ('PRESENT', 0x1A50100,  0x1A501A0),
]


def norm(name: str) -> str:
    return ''.join(ch for ch in name.lower() if ch.isalnum())


def find_module(prefix: str):
    for d in SEARCH_DIRS:
        direct = d / prefix
        if direct.exists():
            return direct
    target = norm(prefix)
    for d in SEARCH_DIRS:
        if not d.is_dir():
            continue
        for p in d.glob('*.dll'):
            if norm(p.name) == target:
                return p
    return None


_cache = {}


def load_image(module):
    """Map file sections into a SizeOfImage virtual buffer (like loaded module)."""
    if module in _cache:
        return _cache[module]
    path = find_module(module)
    if path is None:
        _cache[module] = None
        return None
    data = path.read_bytes()
    e_lfanew = struct.unpack_from('<I', data, 0x3C)[0]
    assert data[e_lfanew:e_lfanew + 4] == b'PE\0\0'
    nsec = struct.unpack_from('<H', data, e_lfanew + 6)[0]
    optsz = struct.unpack_from('<H', data, e_lfanew + 20)[0]
    opt = e_lfanew + 24
    magic = struct.unpack_from('<H', data, opt)[0]
    assert magic == 0x20B, hex(magic)
    size_of_image = struct.unpack_from('<I', data, opt + 56)[0]
    img = bytearray(size_of_image)
    sec = opt + optsz
    for i in range(nsec):
        off = sec + i * 40
        vsz, vaddr, rsz, rptr = struct.unpack_from('<IIII', data, off + 8)
        take = min(vsz or rsz, rsz, len(data) - rptr)
        end = min(vaddr + take, size_of_image)
        img[vaddr:end] = data[rptr:rptr + (end - vaddr)]
    _cache[module] = bytes(img)
    return _cache[module]



def parse_pattern(s):
    """Mirror memory::detail::parse_pattern."""
    out = []           # list[int|None]
    op = 'direct'      # direct | rel_call | rip | abs
    op_idx = 0
    post = 0
    deref = False
    i = 0
    while i < len(s):
        c = s[i]
        if c in ' \t':
            i += 1
            continue
        if c == '>':
            op, op_idx = 'rel_call', len(out)
            i += 1
            continue
        if c == '*':
            op, op_idx = 'rip', len(out)
            i += 1
            continue
        if c == '^':
            op, op_idx = 'abs', len(out)
            i += 1
            continue
        if c == '~':
            deref = True
            i += 1
            continue
        if c in '+-':
            neg = c == '-'
            i += 1
            val = 0
            while i < len(s) and s[i] in '0123456789abcdefABCDEF':
                val = val * 16 + int(s[i], 16)
                i += 1
            post = -val if neg else val
            continue
        if c == '?':
            out.append(None)
            i += 1
            if i < len(s) and s[i] == '?':
                i += 1
            continue
        hi = int(s[i], 16)
        i += 1
        lo = None
        if i < len(s) and s[i] in '0123456789abcdefABCDEF':
            lo = int(s[i], 16)
            i += 1
        out.append(hi if lo is None else (hi << 4) | lo)
    return out, op, op_idx, post, deref


def matches(img, pos, pat):
    for j, b in enumerate(pat):
        if b is None:
            continue
        if img[pos + j] != b:
            return False
    return True


def find_first(img, pat):
    n = len(pat)
    first_idx = next(i for i, b in enumerate(pat) if b is not None)
    fb = pat[first_idx]
    limit = len(img) - 32          # C++ scan_end = size - 32
    start = 0
    fb_at = bytes([fb])
    while True:
        k = img.find(fb_at, start, limit)
        if k < 0:
            return None
        cand = k - first_idx
        if cand >= 0 and cand + n <= len(img) and matches(img, cand, pat):
            return cand
        start = k + 1


def resolve(img, pattern):
    pat, op, op_idx, post, deref = parse_pattern(pattern)
    m = find_first(img, pat)
    if m is None:
        return None, 0
    result = m
    if op == 'rel_call':
        operand = m + op_idx + 1
        if operand + 4 > len(img):
            return None, 1
        rel = struct.unpack_from('<i', img, operand)[0]
        result = operand + 4 + rel
    elif op == 'rip':
        operand = m + op_idx
        if operand + 4 > len(img):
            return None, 1
        rel = struct.unpack_from('<i', img, operand)[0]
        result = operand + 4 + rel
    elif op == 'abs':
        operand = m + op_idx
        if operand + 8 > len(img):
            return None, 1
        result = struct.unpack_from('<Q', img, operand)[0]
    result += post
    if deref:
        if not (0 <= result <= len(img) - 8):
            return None, 1
        result = struct.unpack_from('<Q', img, result)[0]
    return result, 1


def load_pattern_table():
    text = PATTERNS_CPP.read_text(encoding='utf-8', errors='replace')
    rx = re.compile(
        r'address_t&\s*(\w+)\s*=\s*ADDRESS_IMPL\(\s*'
        r'::protection::addresses::hash\("([^"]+)"\)',
        re.S)
    return rx.findall(text)


def main():
    entries = load_pattern_table()
    print(f'parsed {len(entries)} pattern entries from patterns.cpp')

    results = []  # (name, module, status, final)
    for name, spec in entries:
        if ':' not in spec:
            results.append((name, spec, 'BAD-SPEC', None))
            continue
        module, pat = spec.split(':', 1)
        img = load_image(module)
        if img is None:
            results.append((name, module, 'MODULE-MISSING', None))
            continue
        final, ok = resolve(img, pat)
        if not ok or final is None:
            results.append((name, module, 'NOT-FOUND', None))
        else:
            results.append((name, module, 'ok', final))

    # 1) anything resolving INTO a crash window
    print('\n=== resolutions inside crash windows ===')
    hit_any = False
    for name, module, status, final in results:
        if status != 'ok':
            continue
        for label, lo, hi in WINDOWS:
            if module == 'client.dll' and lo <= final < hi:
                hit_any = True
                print(f'  [{label}] {name:34s} -> 0x{final:X}')
    if not hit_any:
        print('  (none)')

    # 2) critical hooks status
    critical = [
        'frame_stage_notify', 'create_move', 'handle_view_angles',
        'parse_report_hit', 'cmd_interpreter',
        'prediction_set_state', 'prediction_set_pawn', 'prediction_setup_move',
        'prediction_process_movement', 'prediction_finish_move', 'prediction_reset_pawn',
        'engine_client_cmd', 'local_player_controller', 'entity_list',
        'global_vars', 'view_matrix', 'game_rules', 'game_event_manager',
        'level_initialization', 'level_shutdown', 'play_music',
        'get_usercmd', 'subtick_move_alloc', 'button_state_alloc',
    ]
    print('\n=== critical patterns ===')
    by_name = {r[0]: r for r in results}
    for c in critical:
        r = by_name.get(c)
        if r is None:
            print(f'  {c:34s} (not defined in patterns.cpp)')
            continue
        name, module, status, final = r
        addr = f'0x{final:X}' if final is not None else '-'
        print(f'  {c:34s} {module:22s} {status:14s} {addr}')

    # 3) summary
    missing = [r for r in results if r[2] != 'ok']
    print(f'\ntotal={len(results)} ok={len(results) - len(missing)} missing={len(missing)}')
    for r in missing:
        print(f'  MISSING {r[0]:34s} {r[1]:22s} {r[2]}')


if __name__ == '__main__':
    main()
