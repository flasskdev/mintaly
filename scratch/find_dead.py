"""Search candidate sites for the 4 dead signatures in the current build.

Usage: py -3 scratch/find_dead.py
"""
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))

BIN64 = Path(r'C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\bin\win64')
CSGO64 = Path(r'C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\csgo\bin\win64')

try:
    from capstone import Cs, CS_ARCH_X86, CS_MODE_64
    HAVE_CS = True
except ImportError:
    HAVE_CS = False


def load_text(dll_path: Path):
    data = dll_path.read_bytes()
    e_lfanew = struct.unpack_from('<I', data, 0x3C)[0]
    optsz = struct.unpack_from('<H', data, e_lfanew + 20)[0]
    nsec = struct.unpack_from('<H', data, e_lfanew + 6)[0]
    opt = e_lfanew + 24
    sec = opt + optsz
    for i in range(nsec):
        off = sec + i * 40
        name = data[off:off + 8].split(b'\x00')[0]
        vsz, vaddr, rsz, rptr = struct.unpack_from('<IIII', data, off + 8)
        if name == b'.text':
            return data[rptr:rptr + rsz], vaddr
    raise RuntimeError('no .text')


def find_all(hay: bytes, needle: bytes):
    out, start = [], 0
    while True:
        k = hay.find(needle, start)
        if k < 0:
            return out
        out.append(k)
        start = k + 1


def disasm(md, hay, text_va, off, n=24):
    for ins in md.disasm(hay[off:off + n], text_va + off):
        print(f'    {text_va + off:#x}: {ins.mnemonic} {ins.op_str}')
        off += ins.size


def main():
    md = Cs(CS_ARCH_X86, CS_MODE_64) if HAVE_CS else None

    # --- 1. parse_report_hit: prefix without the trailing lea displacement ---
    print('=== parse_report_hit (client.dll) ===')
    hay, va = load_text(CSGO64 / 'client.dll')
    prefix = bytes.fromhex('48895C24184889742420574883EC20488D05')  # ...lea rax,[rip+X]
    sites = find_all(hay, prefix)
    print(f'  prefix hits: {len(sites)}')
    for s in sites[:6]:
        disp = struct.unpack_from('<i', hay, s + len(prefix))[0]
        target = va + s + len(prefix) + 4 + disp
        print(f'  site={va + s:#x} lea-> {target:#x}')
        if md:
            disasm(md, hay, va, s, 40)

    # --- 2. weapon_update_mesh: call followed by lea rcx,[r12+0x608] ---
    print('\n=== weapon_update_mesh (client.dll) ===')
    tail = bytes.fromhex('498D8C2408060000')
    sites = find_all(hay, tail)
    print(f'  tail hits: {len(sites)}')
    for s in sites[:8]:
        # expect E8 rel32 immediately before (5 bytes)
        if s >= 5 and hay[s - 5] == 0xE8:
            rel = struct.unpack_from('<i', hay, s - 4)[0]
            call_site = va + s - 5
            target = call_site + 5 + rel
            print(f'  E8 @ {call_site:#x} -> {target:#x}')
            if md:
                disasm(md, hay, va, call_site - 8, 32)

    # --- 3. viewmodel_update_mesh: call; lea rcx,[r13+disp32]; ...; lea rdx,[rsp+..] ---
    print('\n=== viewmodel_update_mesh (client.dll) ===')
    # lea rcx,[r13+disp32] = 49 8D 8D xx xx xx xx
    lea = bytes.fromhex('498D8D')
    sites = find_all(hay, lea)
    print(f'  lea rcx,[r13+d32] hits: {len(sites)}')
    shown = 0
    for s in sites:
        if s < 5 or hay[s - 5] != 0xE8:
            continue
        # window: within next ~16 bytes must appear 48 8D 54 24 (lea rdx,[rsp+..])
        win = hay[s:s + 24]
        if bytes.fromhex('488D5424') not in win:
            continue
        rel = struct.unpack_from('<i', hay, s - 4)[0]
        call_site = va + s - 5
        target = call_site + 5 + rel
        print(f'  E8 @ {call_site:#x} -> {target:#x}')
        if md:
            disasm(md, hay, va, call_site - 6, 36)
        shown += 1
        if shown >= 8:
            break

    # --- 4. filesystem_close: call rbx; mov r12,[rsp+0x98] ---
    print('\n=== filesystem_close (filesystem_stdio.dll) ===')
    fs = BIN64 / 'filesystem_stdio.dll'
    if not fs.exists():
        # try other known dirs
        for d in (BIN64, CSGO64):
            if (d / 'filesystem_stdio.dll').exists():
                fs = d / 'filesystem_stdio.dll'
    print(f'  dll: {fs} exists={fs.exists()}')
    if fs.exists():
        hay2, va2 = load_text(fs)
        tail2 = bytes.fromhex('FFD34C8BA42498000000')  # call rbx; mov r12,[rsp+98h]
        sites = find_all(hay2, tail2)
        print(f'  tail hits: {len(sites)}')
        for s in sites[:8]:
            # look back up to 24 bytes for E8 rel32
            found = False
            for back in range(5, 30):
                if s - back < 0 or hay2[s - back] != 0xE8:
                    continue
                rel = struct.unpack_from('<i', hay2, s - back + 1)[0]
                call_site = va2 + s - back
                target = call_site + 5 + rel
                print(f'  E8 @ {call_site:#x} -> {target:#x} (gap={back})')
                if md:
                    disasm(md, hay2, va2, call_site - 10, 40)
                found = True
                break
            if not found and md:
                print(f'  site {va2 + s:#x}: no preceding E8, context:')
                disasm(md, hay2, va2, max(0, s - 16), 40)


if __name__ == '__main__':
    main()
