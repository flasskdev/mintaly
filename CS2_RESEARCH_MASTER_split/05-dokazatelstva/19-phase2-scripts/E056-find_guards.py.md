<!-- split-part | CS2_RESEARCH_MASTER.md lines 61043-61159 | body-sha256 4f5472892f25da654c80d01be52bf41975eff8fef86482aa945e65ec3d0a821a -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-056"></a>

## E056. `analysis/phase2/scripts/find_guards.py`

Bytes: 5714. SHA-256: `4739412e887b12481ed37cb4ed77061c9f1b7a40796bb2ddf323b819388fd5d1`.

```python
from pathlib import Path
import json
import struct
import sqlite3
from capstone import Cs, CS_ARCH_X86, CS_MODE_64

ROOT = Path('analysis')
BASE = 0x212C3300000
raw = (ROOT / 'input/cs2_212C3300000.bin').read_bytes()
normalized = bytearray(raw)
normalization = []
for sequence in [bytes.fromhex('0f1a2410'), bytes.fromhex('0f1b2410'), bytes.fromhex('0f1c2410')]:
    offset = 0
    while True:
        offset = raw.find(sequence, offset)
        if offset < 0:
            break
        normalized[offset:offset + 4] = b'\x90' * 4
        normalization.append(offset)
        offset += 4
connection = sqlite3.connect(ROOT / 'results/code.sqlite')
engine = Cs(CS_ARCH_X86, CS_MODE_64)
mask = (1 << 64) - 1

def constant_rax(instructions):
    value = None
    for address, size, mnemonic, operands in instructions:
        parts = operands.split(', ')
        if mnemonic in ('movabs', 'mov') and len(parts) == 2 and parts[0] == 'rax' and parts[1].startswith('0x'):
            value = int(parts[1], 16)
        elif value is not None and parts[0] == 'rax':
            if mnemonic in ('add','sub','xor','and','or') and len(parts) == 2:
                try:
                    operand = int(parts[1], 0)
                except ValueError:
                    return None
                if mnemonic == 'add': value += operand
                if mnemonic == 'sub': value -= operand
                if mnemonic == 'xor': value ^= operand
                if mnemonic == 'and': value &= operand
                if mnemonic == 'or': value |= operand
                value &= mask
            elif mnemonic in ('rol','ror') and len(parts) == 2:
                amount = int(parts[1],0) & 63
                if amount:
                    if mnemonic == 'rol': value = ((value << amount) | (value >> (64-amount))) & mask
                    else: value = ((value >> amount) | (value << (64-amount))) & mask
            elif mnemonic == 'not': value ^= mask
            elif mnemonic == 'neg': value = (-value) & mask
            elif mnemonic == 'bswap': value = int.from_bytes(value.to_bytes(8,'little'),'big')
            elif mnemonic in ('cmp','test','nop'): pass
            elif mnemonic == 'mov' and len(parts) == 2 and parts[1].startswith('qword ptr [rsp'): pass
            else: return None
    return value

found = []
offset = 0
needle = bytes.fromhex('65488b042560000000')
while True:
    offset = raw.find(needle, offset)
    if offset < 0: break
    owner = connection.execute('SELECT begin,end FROM functions WHERE begin<=? AND end>? ORDER BY begin DESC LIMIT 1',(offset,offset)).fetchone()
    if owner and owner[0] < 0xde0000:
        instructions = list(engine.disasm_lite(bytes(normalized[offset:offset + 260]),offset))
        for index, instruction in enumerate(instructions):
            address, size, mnemonic, operands = instruction
            if mnemonic == 'je' and index > 0:
                previous = instructions[index-1]
                if previous[2] == 'cmp' and any(register in previous[3] for register in ('[rsp', '[rbx', '[rbp')):
                    destination = int(operands,16)
                    gap = destination - (address + size)
                    value = constant_rax(instructions[:index])
                    if 20 <= gap <= 400 and value is not None and destination < owner[1]:
                        found.append({'guard_start_rva':hex(offset),'branch_rva':hex(address),'target_rva':hex(destination),'owner_rva':hex(owner[0]),'owner_end_rva':hex(owner[1]),'size':size,'condition':previous[3],'expected_environment_hash':hex(value),'branch_bytes':raw[address:address+size].hex(),'excluded_fallthrough_bytes':gap})
                break
            if mnemonic.startswith('j') and mnemonic != 'jmp':
                break
    offset += 9
known_sites = {row['branch_rva'] for row in found}
offset = 0
while True:
    offset = raw.find(b'\x48\xb8', offset)
    if offset < 0: break
    owner = connection.execute('SELECT begin,end FROM functions WHERE begin<=? AND end>? ORDER BY begin DESC LIMIT 1',(offset,offset)).fetchone()
    if owner and owner[0] < 0xde0000:
        instructions = list(engine.disasm_lite(bytes(normalized[offset:offset + 180]), offset))
        for index, instruction in enumerate(instructions):
            address, size, mnemonic, operands = instruction
            if mnemonic == 'je' and index > 0:
                previous = instructions[index-1]
                value = constant_rax(instructions[:index])
                if previous[2] == 'cmp' and any(register in previous[3] for register in ('[rsp', '[rbx', '[rbp')) and value in (0x5877, 0x92fb254d):
                    destination = int(operands,16)
                    gap = destination - address - size
                    if 20 <= gap <= 400 and destination < owner[1] and hex(address) not in known_sites:
                        found.append({'guard_start_rva':hex(offset),'branch_rva':hex(address),'target_rva':hex(destination),'owner_rva':hex(owner[0]),'owner_end_rva':hex(owner[1]),'size':size,'condition':previous[3],'expected_environment_hash':hex(value),'branch_bytes':raw[address:address+size].hex(),'excluded_fallthrough_bytes':gap})
                        known_sites.add(hex(address))
                break
            if mnemonic.startswith('j') and mnemonic != 'jmp': break
    offset += 2
output=ROOT/'phase2/main'
output.mkdir(parents=True,exist_ok=True)
(output/'environment_guards.json').write_text(json.dumps(found,indent=2))
print('PEB guard patterns with decoded constant:',len(found),'NOP hint occurrences:',len(normalization))
for item in found:
    owner=int(item['owner_rva'],16)
    if 0x500000<=owner<0x540000 or owner in [0xf57b0,0x1463a0]: print(item)
```
