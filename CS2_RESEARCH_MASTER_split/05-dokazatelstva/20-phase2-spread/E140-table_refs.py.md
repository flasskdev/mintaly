<!-- split-part | CS2_RESEARCH_MASTER.md lines 76119-76190 | body-sha256 0390801e9a227e2cba10a0e93062e92e00e658425fc4931c0e1458f93fa30bba -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-140"></a>

## E140. `analysis/phase2/spread/table_refs.py`

Bytes: 3060. SHA-256: `89a7e320856c4c6cfa41ce166c11e6ebc49f7cf058487cd3c63c645afa07db6e`.

```python
#!/usr/bin/env python3
import bisect
import csv
import mmap
import struct

import capstone
from capstone import x86
from inspect_spread import BASE, OUTPUT, IMAGE, EXCLUDED, database

connection = database()
functions = connection.execute('SELECT begin,end,decoded_end FROM functions ORDER BY begin').fetchall()
starts = [row[0] for row in functions]
decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
decoder.detail = True
rows = []
seen = set()
with IMAGE.open('rb') as source:
    image = mmap.mmap(source.fileno(), 0, access=mmap.ACCESS_READ)
    code_begin = 0x1000
    code_end = 0xDE0000
    for alignment in range(4):
        start = code_begin + alignment
        stop = code_end - ((code_end-start) % 4)
        view = memoryview(image)[start:stop]
        for index, (displacement,) in enumerate(struct.iter_unpack('<i', view)):
            displacement_offset = start + index*4
            instruction_end = displacement_offset + 4
            target = instruction_end + displacement
            if not 0x1754680 <= target < 0x1754AC0:
                continue
            position = bisect.bisect_right(starts, displacement_offset)-1
            owner = functions[position] if position >= 0 and displacement_offset < functions[position][1] else None
            if owner and owner[0] in EXCLUDED:
                rows.append((hex(displacement_offset),hex(target),hex(owner[0]),'excluded owner; displacement candidate only','',''))
                continue
            candidates = []
            for address in range(max(code_begin,displacement_offset-6),displacement_offset):
                instruction = next(decoder.disasm(image[address:instruction_end], BASE+address, count=1),None)
                if instruction is None or instruction.address+instruction.size != BASE+instruction_end:
                    continue
                if any(operand.type==x86.X86_OP_MEM and operand.mem.base==x86.X86_REG_RIP and instruction_end+operand.mem.disp==target for operand in instruction.operands):
                    candidates.append(instruction)
            if not candidates:
                continue
            instruction = max(candidates,key=lambda item:item.size)
            address = instruction.address-BASE
            if (address,target) in seen:
                continue
            seen.add((address,target))
            indexed = connection.execute('SELECT COUNT(*) FROM refs WHERE source=? AND target=?',(address,target)).fetchone()[0]
            rows.append((hex(address),hex(target),hex(owner[0]) if owner else '',instruction.mnemonic,instruction.op_str,str(bool(indexed))))
        view.release()
rows.sort(key=lambda row:int(row[0],16))
with (OUTPUT/'table_refs.tsv').open('w',newline='') as output:
    writer=csv.writer(output,delimiter='\t')
    writer.writerow(['source','target','owner','mnemonic','operands','indexed'])
    writer.writerows(rows)
print('Matched RIP-reference byte sites:',len(rows),'; no reachability implied for unindexed sites')
for row in rows:
    if row[-1]!='True':
        print(row)
```
