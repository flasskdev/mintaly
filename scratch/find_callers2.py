"""Find E8 callers of a target RVA. Usage: py -3 scratch/find_callers2.py 93E0E0"""
import struct
import sys
from pathlib import Path

DLL = Path(r'C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\csgo\bin\win64\client.dll')
TEXT_RVA, TEXT_OFF, TEXT_SIZE = 0x1000, 0x400, 0x1AA34C0


def find_callers(target: int) -> list[int]:
    with open(DLL, 'rb') as f:
        f.seek(TEXT_OFF)
        img = f.read(TEXT_SIZE)
    out = []
    i = 0
    while i < len(img) - 5:
        if img[i] == 0xE8:
            rel = struct.unpack_from('<i', img, i + 1)[0]
            site = TEXT_RVA + i
            if site + 5 + rel == target:
                out.append(site)
                i += 4
        i += 1
    return out


def find_ff15_sites(target_func_va_base_hint=None):
    """Not implemented; placeholder for indirect-call analysis."""
    pass


if __name__ == '__main__':
    t = int(sys.argv[1], 16)
    callers = find_callers(t)
    print(f'callers of 0x{t:X}: {len(callers)}')
    for c in callers:
        print(f'  E8 @ 0x{c:X}')
