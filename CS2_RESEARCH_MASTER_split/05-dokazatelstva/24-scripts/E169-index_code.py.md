<!-- split-part | CS2_RESEARCH_MASTER.md lines 94680-94753 | body-sha256 82da6007b258b5d6fd067bf7d414a0f80e83ca2e2599c079a5ce66d478de8d14 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-169"></a>

## E169. `analysis/scripts/index_code.py`

Bytes: 3514. SHA-256: `727973d004f31970bc3250aa24ff26222ae53e0a57a6f03ef268688f39f39312`.

```python
from pathlib import Path
import struct
import sqlite3
import json
import re
from capstone import Cs, CS_ARCH_X86, CS_MODE_64

BASE = 0x212C3300000
DATA = Path('analysis/input/cs2_212C3300000.bin').read_bytes()
OUTPUT = Path('analysis/results')
TABLE_START = 0x3f454f0
functions = []
offset = TABLE_START
previous = 0
while offset + 12 <= len(DATA):
    begin, end, unwind = struct.unpack_from('<III', DATA, offset)
    if not (0x1000 <= begin < end <= len(DATA) and 0x1000 <= unwind < len(DATA) - 4 and begin >= previous):
        break
    functions.append((begin, end, unwind, offset))
    previous = begin
    offset += 12
print('table', hex(TABLE_START), hex(offset), 'entries', len(functions), flush=True)
print('code bytes', sum(end-begin for begin,end,_,_ in functions), flush=True)
(OUTPUT / 'runtime_functions.json').write_text(json.dumps([{'begin':hex(begin),'end':hex(end),'unwind':hex(unwind),'table_rva':hex(table)} for begin,end,unwind,table in functions], indent=2))
database = sqlite3.connect(OUTPUT / 'code.sqlite')
database.executescript('DROP TABLE IF EXISTS functions; DROP TABLE IF EXISTS refs; CREATE TABLE functions (begin INTEGER PRIMARY KEY, end INTEGER, unwind INTEGER, table_rva INTEGER, decoded_end INTEGER, instruction_count INTEGER); CREATE TABLE refs (source INTEGER, target INTEGER, owner INTEGER, mnemonic TEXT, operands TEXT, kind TEXT);')
engine = Cs(CS_ARCH_X86, CS_MODE_64)
refs = []
rows = []
seen_begins = set()
for index, (begin, end, unwind, table) in enumerate(functions):
    if begin in seen_begins:
        continue
    seen_begins.add(begin)
    decoded_end = begin
    instructions = 0
    for address, size, mnemonic, operands in engine.disasm_lite(DATA[begin:end], BASE + begin):
        instruction_rva = address - BASE
        decoded_end = instruction_rva + size
        instructions += 1
        if 'rip' in operands:
            match = re.search(r'\[rip(?: ([+-]) (0x[0-9a-f]+))?\]', operands)
            if match:
                displacement = int(match[2],16) if match[2] else 0
                if match[1] == '-':
                    displacement = -displacement
                target = instruction_rva + size + displacement
                if 0 <= target < len(DATA):
                    refs.append((instruction_rva, target, begin, mnemonic, operands, 'rip'))
        if mnemonic == 'call' or mnemonic.startswith('j'):
            if operands.startswith('0x'):
                target = int(operands,16) - BASE
                if 0 <= target < len(DATA):
                    refs.append((instruction_rva, target, begin, mnemonic, operands, 'branch'))
    rows.append((begin,end,unwind,table,decoded_end,instructions))
    if index % 5000 == 0:
        print('indexed', index, flush=True)
database.executemany('INSERT INTO functions VALUES (?,?,?,?,?,?)',rows)
database.executemany('INSERT INTO refs VALUES (?,?,?,?,?,?)',refs)
database.executescript('CREATE INDEX refs_target ON refs(target); CREATE INDEX refs_owner ON refs(owner); CREATE INDEX refs_source ON refs(source);')
database.commit()
summary = {'runtime_table_start':hex(TABLE_START),'runtime_table_end':hex(offset),'runtime_entries':len(functions),'decoded_instructions':sum(row[5] for row in rows),'fully_decoded_ranges':sum(row[1]==row[4] for row in rows),'references':len(refs),'min_begin':hex(min(row[0] for row in rows)),'max_end':hex(max(row[1] for row in rows))}
(OUTPUT / 'index_summary.json').write_text(json.dumps(summary,indent=2))
print(json.dumps(summary,indent=2),flush=True)
```
