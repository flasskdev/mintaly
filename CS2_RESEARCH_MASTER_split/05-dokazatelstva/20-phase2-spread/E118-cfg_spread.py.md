<!-- split-part | CS2_RESEARCH_MASTER.md lines 68435-68558 | body-sha256 d84ce35191e2ea11501055004d26061b18f3ffb4035c82be2b90f85dae9aadbe -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-118"></a>

## E118. `analysis/phase2/spread/cfg_spread.py`

Bytes: 6151. SHA-256: `f393974e067f48354f6044ebf865ae2fc75aab327bdc19cf2f91be5db8aca7b0`.

```python
#!/usr/bin/env python3
import argparse
from collections import deque
import csv
import json
import mmap
import struct

import capstone
from capstone import x86
from inspect_spread import BASE, ROOT, OUTPUT, IMAGE, PATCHES, EXCLUDED, database


def recover(begin, extra_seeds):
    if begin in EXCLUDED:
        raise ValueError('Excluded owner')
    connection = database()
    end, decoded_end = connection.execute('SELECT end,decoded_end FROM functions WHERE begin=?', (begin,)).fetchone()
    if end - begin > 0x18000:
        raise ValueError('Range exceeds bounded analysis limit')
    with IMAGE.open('rb') as source:
        image = mmap.mmap(source.fileno(), 0, access=mmap.ACCESS_READ)
        original = image[begin:end]
        data = bytearray(original)
        patches = []
        for pattern in PATCHES:
            cursor = 0
            while True:
                offset = original.find(pattern, cursor)
                if offset < 0:
                    break
                cursor = offset + 1
                data[offset:offset+4] = b'\x90' * 4
                patches.append({'offset': hex(begin+offset), 'original': pattern.hex(), 'replacement': '90909090'})
        decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
        decoder.detail = True
        pending = deque([begin] + extra_seeds)
        instructions = {}
        edges = []
        stops = []
        while pending:
            address = pending.popleft()
            while begin <= address < end and address not in instructions:
                instruction = next(decoder.disasm(bytes(data[address-begin:address-begin+15]), BASE+address, count=1), None)
                if instruction is None:
                    stops.append({'source': hex(address), 'reason': 'decode_failure', 'bytes': original[address-begin:address-begin+15].hex()})
                    break
                instructions[address] = instruction
                following = address + instruction.size
                if instruction.group(capstone.CS_GRP_JUMP):
                    operand = instruction.operands[0]
                    if operand.type == x86.X86_OP_IMM:
                        target = operand.imm - BASE
                        edges.append((address, target, instruction.mnemonic))
                        if begin <= target < end:
                            pending.append(target)
                    else:
                        stops.append({'source': hex(address), 'reason': 'indirect_jump', 'operands': instruction.op_str})
                    if instruction.mnemonic == 'jmp':
                        break
                if instruction.group(capstone.CS_GRP_RET) or instruction.mnemonic in ('int3', 'ud2', 'hlt'):
                    break
                address = following
        imports = {}
        with (ROOT / 'analysis/imports/user_imports.csv').open() as source:
            for row in csv.DictReader(source):
                imports[int(row['RVA'], 16)] = row['module'] + '!' + row['name']
        calls = []
        with (OUTPUT / f'{begin:08x}.cfg.asm').open('w') as output:
            output.write('; Static recursive CFG; conditional edges both followed, guards not assumed true.\n')
            output.write('; Only known four-byte NOP substitutions in memory. Extra seeds do not prove entry reachability.\n')
            previous = begin
            for address, instruction in sorted(instructions.items()):
                if previous != address:
                    output.write(f'; gap or overlap {previous:#x} -> {address:#x}\n')
                previous = address + instruction.size
                notes = []
                for operand in instruction.operands:
                    if operand.type == x86.X86_OP_IMM and (instruction.group(capstone.CS_GRP_JUMP) or instruction.group(capstone.CS_GRP_CALL)):
                        notes.append(f'RVA {operand.imm-BASE:#x}')
                    if operand.type == x86.X86_OP_MEM and operand.mem.base == x86.X86_REG_RIP:
                        target = address + instruction.size + operand.mem.disp
                        if 0 <= target <= len(image)-8:
                            raw = image[target:target+8]
                            notes.append(f'RVA {target:#x} bytes={raw.hex()} f32={struct.unpack("<f", raw[:4])[0]:.10g}')
                            if target in imports:
                                notes.append(imports[target])
                text = f'{address:08x} {instruction.bytes.hex():24s} {instruction.mnemonic:12s} {instruction.op_str}' + (' ; ' + ' | '.join(notes) if notes else '')
                output.write(text + '\n')
                if instruction.mnemonic == 'call':
                    calls.append(text)
        log = {'begin': hex(begin), 'end': hex(end), 'original_decoded_end': hex(decoded_end),
               'extra_seeds': [hex(seed) for seed in extra_seeds], 'instructions': len(instructions), 'stops': stops,
               'patches': sorted(patches, key=lambda item: int(item['offset'],16)),
               'method': 'recursive static decode, no execution/emulation, no guard simplification, calls not traversed'}
        (OUTPUT / f'{begin:08x}.cfg.json').write_text(json.dumps(log, indent=2) + '\n')
        with (OUTPUT / f'{begin:08x}.cfg.edges.tsv').open('w', newline='') as output:
            writer = csv.writer(output, delimiter='\t'); writer.writerow(['source', 'target', 'mnemonic'])
            writer.writerows((hex(site), hex(target), mnemonic) for site,target,mnemonic in edges)
        (OUTPUT / f'{begin:08x}.cfg.calls.txt').write_text('\n'.join(calls) + '\n')
        print(hex(begin), 'instructions', len(instructions), 'patches', len(patches), 'unresolved stops', len(stops), 'extra seeds',extra_seeds)
        print('\n'.join(text for text in calls if 'Random' in text))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('begin', type=lambda value: int(value,0))
    parser.add_argument('--seed', action='append', default=[], type=lambda value: int(value,0))
    arguments = parser.parse_args()
    recover(arguments.begin, arguments.seed)


if __name__ == '__main__':
    main()
```
