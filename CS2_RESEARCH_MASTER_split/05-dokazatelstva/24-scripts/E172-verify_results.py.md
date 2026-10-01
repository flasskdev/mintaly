<!-- split-part | CS2_RESEARCH_MASTER.md lines 94864-94946 | body-sha256 3e0f82c16422a71f1a1fe25008b7bad6cc283771fd72b10dbfd949a9005d5602 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-172"></a>

## E172. `analysis/scripts/verify_results.py`

Bytes: 3486. SHA-256: `80afb4028fa9e83b3c2368986555ff866aa7f105049f7a1a26266001fa624f29`.

```python
from pathlib import Path
import csv
import hashlib
import json
import re
import sqlite3

ROOT = Path('analysis')
BASE = 0x212C3300000
raw = (ROOT / 'input/cs2_212C3300000.bin').read_bytes()
checks = {'dump_size': len(raw), 'dump_sha256': hashlib.sha256(raw).hexdigest()}
assert len(raw) == 0x5001000
assert checks['dump_sha256'] == '3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27'
source = (ROOT / 'imports/user_imports_source.txt').read_text()
source_rows = re.findall(r'\{\s*(0x[0-9A-Fa-f]+),\s*"([^"]+)",\s*"([^"]+)"\s*\}', source)
imports = list(csv.DictReader((ROOT / 'imports/user_imports.csv').open()))
assert len(imports) == len(source_rows) == 391
for imported, original in zip(imports, source_rows):
    assert int(imported['VA'], 16) == int(original[0], 16)
    assert int(imported['RVA'], 16) == int(imported['VA'], 16) - BASE
    assert (imported['module'], imported['name']) == original[1:]
checks['exact_import_rows'] = len(imports)
anchors = list(csv.DictReader((ROOT / 'strings/anchors.csv').open()))
verified_anchors = 0
for anchor in anchors:
    location = int(anchor['rva_hex'], 16)
    assert int(anchor['va_hex'], 16) == BASE + location
    if anchor['encoding'] == 'ascii' and anchor['redacted'].lower() == 'false':
        text = anchor['text'].encode('ascii')
        assert raw[location:location + len(text)] == text
        verified_anchors += 1
checks['ascii_anchor_bytes_verified'] = verified_anchors
checks['anchors'] = len(anchors)
connection = sqlite3.connect(f'file:{ROOT / "results/code.sqlite"}?mode=ro', uri=True)
assert connection.execute('PRAGMA integrity_check').fetchone()[0] == 'ok'
checks['unique_runtime_ranges'] = connection.execute('SELECT count(*) FROM functions').fetchone()[0]
checks['linear_references'] = connection.execute('SELECT count(*) FROM refs').fetchone()[0]
assert checks['unique_runtime_ranges'] == 45064
assert checks['linear_references'] == 509768
instruction_count = 0
assembly_files = sorted((ROOT / 'ghidra_assembly').glob('*.asm'))
for assembly_file in assembly_files:
    for row in assembly_file.read_text().splitlines()[1:]:
        fields = row.split('\t')
        if len(fields) < 3:
            continue
        location = int(fields[0], 16) - BASE
        machine_code = bytes.fromhex(fields[1])
        assert raw[location:location + len(machine_code)] == machine_code, (assembly_file.name, fields[0])
        instruction_count += 1
checks['assembly_files'] = len(assembly_files)
checks['ghidra_instruction_bytes_verified'] = instruction_count
successes = []
failures = []
for directory in ['decompiled_rng','decompiled_features','decompiled_core']:
    index_path = ROOT / directory / 'index.tsv'
    if not index_path.exists():
        continue
    for row in index_path.read_text().splitlines():
        fields = row.split('\t')
        if len(fields) < 4:
            continue
        if fields[3] == 'ok':
            output = ROOT / directory / fields[2]
            assert output.exists() and output.stat().st_size > 0
            successes.append(fields[0])
        else:
            failures.append({'rva':fields[0], 'error':fields[3]})
checks['decompilation_successes'] = len(successes)
checks['decompilation_failures'] = failures
checks['interpretation_scope'] = 'Integrity/address/byte consistency checks only; no execution-equivalence or full feature-coverage claim.'
(ROOT / 'results/verification.json').write_text(json.dumps(checks,indent=2))
print(json.dumps(checks,indent=2))
```
