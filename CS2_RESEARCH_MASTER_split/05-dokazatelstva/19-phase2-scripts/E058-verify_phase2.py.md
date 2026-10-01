<!-- split-part | CS2_RESEARCH_MASTER.md lines 61223-61320 | body-sha256 f64a5944acd49efc0164a9c502570de3e2a3075255db343f95124879e8433264 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-058"></a>

## E058. `analysis/phase2/scripts/verify_phase2.py`

Bytes: 4361. SHA-256: `ecc83ab34a5f2893a770037ab289befd390954066a7a473b15bb454b7b1376ff`.

```python
from pathlib import Path
import hashlib
import json
import struct

ROOT = Path('analysis')
BASE = 0x212C3300000
EXPECTED = '3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27'
raw = (ROOT / 'input/cs2_212C3300000.bin').read_bytes()
model = (ROOT / 'phase2/model/CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin').read_bytes()
manifest = json.loads((ROOT / 'phase2/main/conditional_model_manifest.json').read_text())
assert hashlib.sha256(raw).hexdigest() == EXPECTED
assert hashlib.sha256(model).hexdigest() == manifest['model_sha256']
expected_model = bytearray(raw)
branch_sites = set()
for change in manifest['guard_changes']:
    address = int(change['branch_rva'], 16)
    destination = int(change['target_rva'], 16)
    before = bytes.fromhex(change['branch_bytes'])
    after = bytes.fromhex(change['analysis_only_replacement'])
    assert address not in branch_sites
    branch_sites.add(address)
    assert raw[address:address + len(before)] == before
    assert len(before) == len(after)
    if before[0] == 0x74:
        assert after[0] == 0xeb
        original_target = address + 2 + struct.unpack('<b', before[1:])[0]
        model_target = address + 2 + struct.unpack('<b', after[1:])[0]
    else:
        assert before[:2] == b'\x0f\x84' and after[0] == 0xe9 and after[-1] == 0x90
        original_target = address + 6 + struct.unpack('<i', before[2:])[0]
        model_target = address + 5 + struct.unpack('<i', after[1:5])[0]
    assert original_target == model_target == destination
    expected_model[address:address + len(after)] = after
assert bytes(expected_model) == model
instruction_count = 0
byte_count = 0
conditional_instruction_rows = 0
for path in sorted((ROOT / 'phase2/conditional_assembly').glob('*.asm')):
    for line in path.read_text().splitlines():
        fields = line.split('\t', 2)
        if len(fields) < 3:
            continue
        try:
            address = int(fields[0], 16) - BASE
            encoded = bytes.fromhex(fields[1])
        except ValueError:
            continue
        assert encoded == model[address:address + len(encoded)], (path.name, hex(address))
        conditional_instruction_rows += encoded != raw[address:address + len(encoded)]
        instruction_count += 1
        byte_count += len(encoded)
outputs = []
for index in sorted((ROOT / 'phase2').glob('conditional_*/index.tsv')):
    for line in index.read_text().splitlines():
        fields = line.split('\t')
        if len(fields) >= 4:
            outputs.append({'rva': fields[0], 'status': fields[3], 'directory': index.parent.name})
placeholder_fields = []
for address in [0x1758658, 0x17586c4, 0x1758290, 0x1758680, 0x176f7c8, 0x17702d0, 0x176fe34]:
    value = struct.unpack_from('<I', raw, address)[0]
    assert value == 0x13371337
    placeholder_fields.append({'rva': hex(address), 'u32': hex(value)})
external_addresses = [0x7ffcef4a8db0, 0x7ffcef4a3e50, 0x7ffcef48e030, 0x7ffcef616040, 0x7ffcf0fe45a0]
assert all(not BASE <= address < BASE + len(raw) for address in external_addresses)
constants = {}
for address in [0xe99580, 0xe98820, 0xe9adb4, 0xe9bb30, 0xe9bb34, 0xe9bb40, 0xe9bb44, 0xe98144, 0xe99930, 0xe98a6c, 0xe980c4, 0xe9adb8]:
    constants[hex(address)] = struct.unpack_from('<f', raw, address)[0]
result = {
    'original_sha256': EXPECTED,
    'original_unchanged': True,
    'model_sha256': manifest['model_sha256'],
    'branch_transforms_verified': len(branch_sites),
    'model_equals_original_plus_manifest_only': True,
    'runtime_environment_assumptions_verified': False,
    'conditional_assembly_rows_verified_against_model': instruction_count,
    'conditional_assembly_bytes_verified_against_model': byte_count,
    'rows_different_from_original': conditional_instruction_rows,
    'decompilation_results': outputs,
    'successful_c_outputs': sum(item['status'] == 'ok' for item in outputs),
    'external_addresses_outside_dump': [hex(address) for address in external_addresses],
    'placeholder_fields': placeholder_fields,
    'float_constants': constants,
    'sample_executed': False,
    'behavioral_equivalence_claimed': False
}
(ROOT / 'phase2/main/verification.json').write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps({key: value for key, value in result.items() if key not in ('decompilation_results', 'float_constants', 'placeholder_fields')}, indent=2))
```
