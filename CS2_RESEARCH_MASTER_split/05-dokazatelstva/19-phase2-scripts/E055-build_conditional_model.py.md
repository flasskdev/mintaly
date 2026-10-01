<!-- split-part | CS2_RESEARCH_MASTER.md lines 61002-61042 | body-sha256 dadace70658a8d217b3fda6e1613b797adec7ae592b74f8105ae161d9c233cba -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-055"></a>

## E055. `analysis/phase2/scripts/build_conditional_model.py`

Bytes: 1797. SHA-256: `a7d914215df7d6f08c406f06f72e26389e96b676cb522c94ed6cb59e283c81c4`.

```python
from pathlib import Path
import struct
import hashlib
import json

ROOT = Path('analysis')
source = ROOT / 'input/cs2_212C3300000.bin'
original = source.read_bytes()
model = bytearray(original)
assert hashlib.sha256(original).hexdigest() == '3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27'
guards = json.loads((ROOT/'phase2/main/environment_guards.json').read_text())
changes=[]
for guard in guards:
    location = int(guard['branch_rva'],16)
    destination = int(guard['target_rva'],16)
    size = guard['size']
    assert original[location:location+size].hex() == guard['branch_bytes']
    if size == 2:
        replacement = b'\xeb' + struct.pack('<b',destination-location-2)
    elif size == 6:
        replacement = b'\xe9' + struct.pack('<i',destination-location-5) + b'\x90'
    else:
        raise ValueError(guard)
    model[location:location+size] = replacement
    changes.append({**guard, 'analysis_only_replacement':replacement.hex(), 'assumption':'environment equality holds; failure path is not modeled'})
output=ROOT/'phase2/model'
output.mkdir(parents=True,exist_ok=True)
(output/'CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin').write_bytes(model)
manifest={'warning':'ANALYSIS-ONLY PATH MODEL. NOT an equivalent original or a runnable DLL. Original sample remains unchanged. These equality predicates are assumptions; their runtime inputs are not in the dump.', 'original_sha256':hashlib.sha256(original).hexdigest(),'model_sha256':hashlib.sha256(model).hexdigest(),'original_path':str(source),'guard_changes':changes}
(ROOT/'phase2/main/conditional_model_manifest.json').write_text(json.dumps(manifest,indent=2))
print('analysis-only branch choices',len(changes),'original unchanged',hashlib.sha256(source.read_bytes()).hexdigest()==manifest['original_sha256'])
```
