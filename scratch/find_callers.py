"""Identify crash function 0x3E7E70: its lea-target string + all E8 callers."""
import struct
import sys

sys.path.insert(0, str(__import__('pathlib').Path(__file__).parent))
from dump_sections import rva_to_off, read_at, DLL

FUNC = 0x3E7E70
# lea r14,[rip+0x16D461F] at 0x3E7E89 (len 7)
LEA_RIP = 0x3E7E90
LEA_DISP = struct.unpack('<i', bytes.fromhex('1F466D01'))[0]
str_rva = LEA_RIP + LEA_DISP
print(f'lea target string RVA = 0x{str_rva:X}')
off = rva_to_off(str_rva)
b = read_at(off, 128)
s = b.split(b'\x00')[0]
print('string:', s)

# scan .text for E8 rel32 calls to FUNC
TEXT_RVA, TEXT_OFF, TEXT_SIZE = 0x1000, 0x400, 0x1AA34C0
with open(DLL, 'rb') as f:
    f.seek(TEXT_OFF)
    img = f.read(TEXT_SIZE)

print('\ncallers (E8 rel32 -> 0x%X):' % FUNC)
found = 0
i = 0
while i < len(img) - 5:
    if img[i] == 0xE8:
        rel = struct.unpack_from('<i', img, i + 1)[0]
        site_rva = TEXT_RVA + i
        target = site_rva + 5 + rel
        if target == FUNC:
            caller_rva = None
            print(f'  from RVA 0x{site_rva:X}')
            found += 1
    i += 1
if not found:
    print('  (no direct E8 callers)')

# also FF 15 (call [rip+disp]) is indirect; skip.
