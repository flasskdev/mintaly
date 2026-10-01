<!-- split-part | CS2_RESEARCH_MASTER.md lines 94802-94863 | body-sha256 1d5e863e247db0467bb974ef5aec40fcd637e554d7c3baf115ecfc1d0bdd6016 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-171"></a>

## E171. `analysis/scripts/triage.py`

Bytes: 2333. SHA-256: `2cfc40e345c979d6fcb5085f856c69ccec6ab09b7090de150e6a4ddc9a26be01`.

```python
from pathlib import Path
import collections
import hashlib
import json
import math
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_64

BASE = 0x212C3300000
DATA = Path('analysis/input/cs2_212C3300000.bin').read_bytes()
OUTPUT = Path('analysis/results')
OUTPUT.mkdir(exist_ok=True)

def entropy(block):
    counts = collections.Counter(block)
    return round(-sum((count / len(block)) * math.log2(count / len(block)) for count in counts.values()), 4)

regions = []
for offset in range(0, len(DATA), 0x100000):
    block = DATA[offset:offset + 0x100000]
    regions.append({'rva': hex(offset), 'entropy': entropy(block), 'zero_fraction': round(block.count(0) / len(block), 3), 'cc_fraction': round(block.count(0xcc) / len(block), 3)})

runs = []
for alignment in (0, 4, 8):
    start = None
    previous = 0
    count = 0
    for offset in range(alignment, len(DATA) - 12, 12):
        begin, end, unwind = struct.unpack_from('<III', DATA, offset)
        valid = 0x1000 <= begin < end < len(DATA) and end - begin < 0x20000 and 0x1000 <= unwind < len(DATA) - 4 and unwind % 4 == 0 and DATA[unwind] & 7 in (1, 2) and begin > previous
        if valid:
            if start is None:
                start = offset
            count += 1
            previous = begin
        else:
            if count >= 10:
                runs.append({'table_rva': hex(start), 'count': count, 'first_begin': hex(struct.unpack_from('<I', DATA, start)[0]), 'last_begin': hex(previous)})
            start = None
            count = 0
            previous = 0
    if count >= 10:
        runs.append({'table_rva': hex(start), 'count': count})

info = {'size': hex(len(DATA)), 'sha256': hashlib.sha256(DATA).hexdigest(), 'base': hex(BASE), 'entry_rva_user': hex(0x212C82F1690 - BASE), 'regions_1mb': regions, 'runtime_function_candidates': sorted(runs, key=lambda row: row['count'], reverse=True)}
(OUTPUT / 'triage.json').write_text(json.dumps(info, indent=2))
print(json.dumps(info, indent=2))
disassembler = Cs(CS_ARCH_X86, CS_MODE_64)
for entry in [0x4ff1690, 0x1000, 0xf91d40, 0x1827000]:
    print('\nDISASSEMBLY / BYTES', hex(BASE + entry), DATA[entry:entry + 64].hex())
    for instruction in disassembler.disasm(DATA[entry:entry + 192], BASE + entry):
        print(hex(instruction.address), instruction.mnemonic, instruction.op_str)
```
