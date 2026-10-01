"""Inspect scenesystem vtables for generate_primitives vs animatable variant.

Usage: py -3 scratch/vtables.py
"""
import struct
from pathlib import Path

BIN64 = Path(r'C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\bin\win64')

from capstone import Cs, CS_ARCH_X86, CS_MODE_64  # noqa: E402

BASE = 0x180000000


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


def show(md, img, off, n):
    o, end = off, off + n
    while o < end:
        for ins in md.disasm(img[o:o + 16], o):
            print(f'  {o:#x}: {ins.mnemonic:9s} {ins.op_str}')
            o += ins.size
            break
        else:
            o += 1


def main():
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    scene = load_image(BIN64 / 'scenesystem.dll')

    for label, refs in (('genprims 0x78170', [0x5e4688, 0x5ee4e0]),
                        ('animatable 0x7d7c0', [0x5e5cb0, 0x5e5d90])):
        for r in refs:
            print(f'\n=== {label} vtable @ {r:#x} ===')
            for k in range(-6, 4):
                va = struct.unpack_from('<Q', scene, r + k * 8)[0]
                star = '  <<<' if k == 0 else ''
                print(f'  slot{k:+d} @ {r + k * 8:#x}: VA={va:#x} RVA={va - BASE:#x}{star}')

    # Which function reads m_hOwnerIndex at +0xc0?
    print('\n=== 0x78170 prologue ===')
    show(md, scene, 0x78170, 0x50)
    print('\n=== 0x7d7c0 prologue ===')
    show(md, scene, 0x7d7c0, 0x50)


if __name__ == '__main__':
    main()