"""Proper masked pattern scan + locate update_skin via composite_material callers.

Usage: py -3 scratch/scan2.py
"""
import struct
from pathlib import Path

CSGO64 = Path(r'C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\csgo\bin\win64')
BIN64 = Path(r'C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\bin\win64')

from capstone import Cs, CS_ARCH_X86, CS_MODE_64  # noqa: E402


def load_image(p: Path):
    data = p.read_bytes()
    e = struct.unpack_from('<I', data, 0x3C)[0]
    optsz = struct.unpack_from('<H', data, e + 20)[0]
    nsec = struct.unpack_from('<H', data, e + 6)[0]
    opt = e + 24
    soi = struct.unpack_from('<I', data, opt + 56)[0]
    img = bytearray(soi)
    sec = opt + optsz
    for i in range(nsec):
        off = sec + i * 40
        vsz, vaddr, rsz, rptr = struct.unpack_from('<IIII', data, off + 8)
        take = min(vsz or rsz, rsz, len(data) - rptr)
        end = min(vaddr + take, soi)
        img[vaddr:end] = data[rptr:rptr + (end - vaddr)]
    return bytes(img)


def to_pat(hx):
    """hex string with ?? wildcards and optional spaces -> list[int|None]"""
    hxs = hx.replace(' ', '')
    return [None if hxs[i:i + 2] == '??' else int(hxs[i:i + 2], 16) for i in range(0, len(hxs), 2)]


def longest_run(pat):
    best_s = best_l = 0
    cs, cl = None, 0
    for i, b in enumerate(pat):
        if b is None:
            if cl > best_l:
                best_s, best_l = cs, cl
            cs, cl = None, 0
        else:
            if cs is None:
                cs, cl = i, 0
            cl += 1
    if cl > best_l:
        best_s, best_l = cs, cl
    return best_s, best_l


def scan(img, pat):
    """Return list of match offsets."""
    run_s, run_l = longest_run(pat)
    needle = bytes(pat[run_s:run_s + run_l])
    out, start = [], 0
    n = len(pat)
    while True:
        k = img.find(needle, start)
        if k < 0:
            return out
        start = k + 1
        st = k - run_s
        if st < 0 or st + n > len(img):
            continue
        ok = True
        for j, b in enumerate(pat):
            if b is not None and img[st + j] != b:
                ok = False
                break
        if ok:
            out.append(st)


def show(md, img, off, n, indent='  '):
    o, end = off, off + n
    while o < end:
        for ins in md.disasm(img[o:o + 16], o):
            print(f'{indent}{o:#x}: {ins.mnemonic:9s} {ins.op_str}')
            o += ins.size
            break
        else:
            o += 1


def callers(img, target):
    out = []
    start = 0
    while True:
        s = img.find(b'\xE8', start)
        if s < 0:
            return out
        start = s + 1
        if s + 5 > len(img):
            continue
        rel = struct.unpack_from('<i', img, s + 1)[0]
        if s + 5 + rel == target:
            out.append(s)
    return out


def main():
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    client = load_image(CSGO64 / 'client.dll')

    # ---- weapon_update_composite_material
    pat = to_pat('48895C241048896C2418488974242057415641574883EC20440FB6F2488BF9')
    hits = scan(client, pat)
    print(f'update_composite_material hits={[hex(h) for h in hits]}')
    for h in hits[:3]:
        show(md, client, h, 0x60)
        print()

    # ---- all 440FB6FA sites with context
    print('=== movzx r15d,dl sites (22) ===')
    s = 0
    while True:
        s = client.find(bytes.fromhex('440FB6FA'), s)
        if s < 0:
            break
        print(f'  --- {s:#x}')
        show(md, client, max(0, s - 24), 56)
        s += 1

    # ---- old update_skin tail: 440FB6FA 488BD9 near 40555341
    print('\n=== 440FB6F2 (movzx r14d,dl) / 440FB6FA with mov rbx,rcx or mov rbp,rcx ===')
    for a in (bytes.fromhex('440FB6FA488BD9'), bytes.fromhex('440FB6FA488BE9'),
              bytes.fromhex('440FB6F72', )):
        print(f'  {a.hex().upper()}: {[hex(x) for x in _findall(client, a)]}')

    # ---- generate_primitives target check in scenesystem
    scene = load_image(BIN64 / 'scenesystem.dll')
    pat2 = to_pat('488D05????????488907488B7C2448')
    print(f'\nscenesystem generate_primitives pattern hits={[hex(x) for x in scan(scene, pat2)]}')


def _findall(img, needle):
    out, start = [], 0
    while True:
        k = img.find(needle, start)
        if k < 0:
            return out
        out.append(k)
        start = k + 1


if __name__ == '__main__':
    main()
