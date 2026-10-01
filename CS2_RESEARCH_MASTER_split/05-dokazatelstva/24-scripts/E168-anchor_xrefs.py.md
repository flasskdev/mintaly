<!-- split-part | CS2_RESEARCH_MASTER.md lines 94653-94679 | body-sha256 3e5c119c064539081c138dcd0957f34510308a1979f7f1dcf7322a626626914e -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-168"></a>

## E168. `analysis/scripts/anchor_xrefs.py`

Bytes: 1074. SHA-256: `9deb32d08d0eca6be2e53e63c9ba3253c2ae3178f67844460329423885e5945b`.

```python
from pathlib import Path
import csv
import sqlite3

BASE = 0x212C3300000
ROOT = Path('analysis')
connection = sqlite3.connect(ROOT / 'results/code.sqlite')
rows = []
for anchor in csv.DictReader((ROOT / 'strings/anchors.csv').open()):
    target = int(anchor['rva_hex'], 16)
    for source, owner, mnemonic, operands in connection.execute('SELECT source,owner,mnemonic,operands FROM refs WHERE target=? ORDER BY source', (target,)):
        rows.append({'anchor': anchor['text'], 'anchor_rva': hex(target), 'source_rva': hex(source), 'source_va': hex(BASE + source), 'runtime_owner_rva': hex(owner), 'runtime_owner_va': hex(BASE + owner), 'mnemonic': mnemonic, 'operands': operands})
with (ROOT / 'results/anchor_xrefs.csv').open('w') as output:
    writer = csv.DictWriter(output, fieldnames=['anchor','anchor_rva','source_rva','source_va','runtime_owner_rva','runtime_owner_va','mnemonic','operands'])
    writer.writeheader()
    writer.writerows(rows)
print('Anchors:', len(list(csv.DictReader((ROOT / 'strings/anchors.csv').open()))), 'indexed references:', len(rows))
```
