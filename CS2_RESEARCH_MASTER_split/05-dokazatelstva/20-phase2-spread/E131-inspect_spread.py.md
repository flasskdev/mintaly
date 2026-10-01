<!-- split-part | CS2_RESEARCH_MASTER.md lines 74313-74498 | body-sha256 15208c43f5500c5aa40f909284eeb4f213681b2d72b311ae6e72135205fcd84b -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-131"></a>

## E131. `analysis/phase2/spread/inspect_spread.py`

Bytes: 8364. SHA-256: `c04dd4026822f889a3ad1f29498e23d9925cd6bbb494c312cfdd031947110a8c`.

```python
#!/usr/bin/env python3
import argparse
import bisect
import csv
import json
import mmap
from pathlib import Path
import sqlite3
import struct

import capstone
from capstone import x86

BASE = 0x212C3300000
ROOT = Path(__file__).resolve().parents[3]
OUTPUT = Path(__file__).resolve().parent
DATABASE = ROOT / 'analysis/results/code.sqlite'
IMAGE = ROOT / 'analysis/input/cs2_212C3300000.bin'
EXCLUDED = {0x528B90}
PATCHES = (bytes.fromhex('0f1a2410'), bytes.fromhex('0f1b2410'), bytes.fromhex('0f1c2410'))


def database():
    connection = sqlite3.connect(f'file:{DATABASE}?mode=ro&immutable=1', uri=True)
    connection.execute('PRAGMA query_only=ON')
    return connection


def owner_at(functions, starts, offset):
    position = bisect.bisect_right(starts, offset) - 1
    if position >= 0 and offset < functions[position][1]:
        return functions[position]
    return None


def write_rows(name, headers, rows):
    with (OUTPUT / name).open('w', newline='') as output:
        writer = csv.writer(output, delimiter='\t')
        writer.writerow(headers)
        writer.writerows(rows)


def exact_scan(image, functions, starts):
    values = (16807, 127773, 2836, 2147483647, 214013, 2531011,
              1664525, 1013904223, 0x41C64E6D, 48271, 0x6C078965,
              0x9908B0DF, 0x9D2C5680, 0xEFC60000)
    rows = []
    summaries = []
    for value in values:
        pattern = struct.pack('<I', value)
        cursor = 0
        total = 0
        in_ranges = 0
        while True:
            offset = image.find(pattern, cursor)
            if offset < 0:
                break
            cursor = offset + 1
            total += 1
            owner = owner_at(functions, starts, offset)
            in_ranges += bool(owner)
            rows.append((hex(value), value, hex(offset), hex(owner[0]) if owner else '',
                         hex(owner[1]) if owner else '', hex(owner[2]) if owner else ''))
        summaries.append({'value': value, 'hex': hex(value), 'byte_hits': total, 'runtime_range_hits': in_ranges})
    write_rows('constant_hits.tsv', ['hex_value', 'decimal_value', 'offset', 'owner', 'end', 'decoded_end'], rows)
    (OUTPUT / 'constant_counts.json').write_text(json.dumps(summaries, indent=2) + '\n')
    print(json.dumps(summaries, indent=2))


def raw_calls(image, functions, starts, connection):
    targets = {0xD92B60: 'cosf_like', 0xD9ABE0: 'sinf_like', 0xF92658: 'RandomFloat_IAT',
               0xF92660: 'RandomInt_IAT', 0xF92668: 'RandomSeed_IAT'}
    rows = []
    code_end = 0xDE0000
    for opcode, displacement_offset, length in ((b'\xe8', 1, 5), (b'\xff\x15', 2, 6), (b'\xff\x25', 2, 6)):
        cursor = 0x1000
        while True:
            site = image.find(opcode, cursor, code_end)
            if site < 0:
                break
            cursor = site + 1
            target = site + length + struct.unpack_from('<i', image, site + displacement_offset)[0]
            if target not in targets:
                continue
            owner = owner_at(functions, starts, site)
            if owner and owner[0] in EXCLUDED:
                continue
            indexed = connection.execute('SELECT COUNT(*) FROM refs WHERE source=? AND target=?', (site, target)).fetchone()[0]
            rows.append((hex(site), hex(target), targets[target], hex(owner[0]) if owner else '',
                         hex(owner[2]) if owner else '', opcode.hex(), bool(indexed)))
    rows.sort(key=lambda row: int(row[0], 16))
    write_rows('raw_target_calls.tsv', ['source', 'target', 'label', 'owner', 'decoded_end', 'opcode', 'indexed'], rows)
    print('raw matched call-byte sites:', len(rows), '; unindexed:', sum(not row[-1] for row in rows))
    for row in rows:
        if not row[-1]:
            print(row)


def disassemble(image, functions, starts, connection, begin, size):
    if begin in EXCLUDED:
        raise ValueError('Excluded owner')
    owner = owner_at(functions, starts, begin)
    if owner and owner[0] in EXCLUDED:
        raise ValueError('Excluded range')
    end = begin + size if size else (owner[1] if owner else begin + 0x200)
    if end - begin > 0x18000:
        raise ValueError('Explicit disassembly window exceeds bound')
    original = image[begin:end]
    data = bytearray(original)
    changes = []
    for pattern in PATCHES:
        cursor = 0
        while True:
            offset = original.find(pattern, cursor)
            if offset < 0:
                break
            cursor = offset + 1
            data[offset:offset+4] = b'\x90' * 4
            changes.append({'offset': hex(begin + offset), 'original': pattern.hex(), 'replacement': '90909090'})
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    decoder.detail = True
    instructions = list(decoder.disasm(bytes(data), BASE + begin))
    decoded_end = instructions[-1].address + instructions[-1].size - BASE if instructions else begin
    imports = {}
    with (ROOT / 'analysis/imports/user_imports.csv').open() as source:
        for row in csv.DictReader(source):
            imports[int(row['RVA'], 16)] = row['module'] + '!' + row['name']
    path = OUTPUT / f'{begin:08x}.asm'
    with path.open('w') as output:
        output.write(f'; RVA {begin:#x}..{end:#x}; static copy; original unchanged\n')
        output.write(f'; runtime owner: {owner}; original index end: {owner[2] if owner else None}\n')
        for instruction in instructions:
            notes = []
            for operand in instruction.operands:
                if operand.type == x86.X86_OP_IMM and instruction.group(capstone.CS_GRP_BRANCH_RELATIVE):
                    notes.append(f'target RVA {operand.imm-BASE:#x}')
                if operand.type == x86.X86_OP_MEM and operand.mem.base == x86.X86_REG_RIP:
                    target = instruction.address + instruction.size + operand.mem.disp - BASE
                    if 0 <= target <= len(image) - 8:
                        raw = image[target:target+8]
                        notes.append(f'RVA {target:#x} bytes={raw.hex()} f32={struct.unpack("<f", raw[:4])[0]:.10g}')
                        if target in imports:
                            notes.append(imports[target])
            output.write(f'{instruction.address-BASE:08x} {instruction.bytes.hex():24s} {instruction.mnemonic:12s} {instruction.op_str}' + (' ; ' + ' | '.join(notes) if notes else '') + '\n')
    log = {'begin': hex(begin), 'end': hex(end), 'decoded_end': hex(decoded_end), 'patches': sorted(changes, key=lambda item: int(item['offset'], 16)),
           'stop_bytes': image[decoded_end:min(end, decoded_end+16)].hex(), 'method': 'linear decode; PEB/junk reachability not resolved; no skipdata'}
    (OUTPUT / f'{begin:08x}.decode.json').write_text(json.dumps(log, indent=2) + '\n')
    references = connection.execute('SELECT source,target,owner,mnemonic,operands,kind FROM refs WHERE owner=? OR target=? ORDER BY source', (begin, begin)).fetchall()
    write_rows(f'{begin:08x}.refs.tsv', ['source', 'target', 'owner', 'mnemonic', 'operands', 'kind'],
               [(hex(site), hex(target), hex(caller), mnemonic, operands, kind) for site, target, caller, mnemonic, operands, kind in references])
    calls = [(hex(instruction.address-BASE), instruction.op_str) for instruction in instructions if instruction.mnemonic == 'call']
    print(json.dumps({'file': path.name, **log, 'calls': calls}, indent=2))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('mode', choices=('constants', 'calls', 'disasm'))
    parser.add_argument('addresses', nargs='*', type=lambda value: int(value, 0))
    parser.add_argument('--size', type=lambda value: int(value, 0), default=0)
    args = parser.parse_args()
    connection = database()
    functions = connection.execute('SELECT begin,end,decoded_end FROM functions ORDER BY begin').fetchall()
    starts = [row[0] for row in functions]
    with IMAGE.open('rb') as source:
        image = mmap.mmap(source.fileno(), 0, access=mmap.ACCESS_READ)
        if args.mode == 'constants':
            exact_scan(image, functions, starts)
        elif args.mode == 'calls':
            raw_calls(image, functions, starts, connection)
        else:
            for begin in args.addresses:
                disassemble(image, functions, starts, connection, begin, args.size)


if __name__ == '__main__':
    main()
```
