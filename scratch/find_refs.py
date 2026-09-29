"""Brute-force capstone scan: find all instructions referencing a given RVA.

Usage: py -3 scratch/find_refs.py 0x2234170
Prints instruction addresses + mnemonics referencing the target via RIP-relative.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from find_dead import load_text  # noqa: E402

from capstone import Cs, CS_ARCH_X86, CS_MODE_64  # noqa: E402
from capstone.x86 import X86_OP_MEM, X86_REG_RIP  # noqa: E402

DLL = Path(r'C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\csgo\bin\win64\client.dll')


def main():
    target = int(sys.argv[1], 16)
    hay, va = load_text(DLL)
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    md.detail = True
    hits = []
    for ins in md.disasm(hay, va):
        for op in ins.operands:
            if op.type == X86_OP_MEM and op.mem.base == X86_REG_RIP:
                if ins.address + ins.size + op.mem.disp == target:
                    hits.append(ins.address)
                    print(f'{ins.address:#x}: {ins.mnemonic} {ins.op_str}')
    print(f'total refs: {len(hits)}')


if __name__ == '__main__':
    main()
