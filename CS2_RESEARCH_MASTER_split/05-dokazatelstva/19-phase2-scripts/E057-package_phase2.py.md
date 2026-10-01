<!-- split-part | CS2_RESEARCH_MASTER.md lines 61160-61222 | body-sha256 d055481ff100a56eebc7f35b18669e5217e882f6f1bd155327309d69f8096471 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-057"></a>

## E057. `analysis/phase2/scripts/package_phase2.py`

Bytes: 3766. SHA-256: `4eba999378521db341861a2ee6a34ec170f0224033147e87896d8883afb5dc0c`.

```python
from pathlib import Path
import hashlib
import json
import zipfile

ROOT = Path('analysis')
PHASE = ROOT / 'phase2'
DESTINATION = Path('deliverables')
DESTINATION.mkdir(exist_ok=True)
files = [path for path in PHASE.rglob('*')
         if path.is_file() and not set(path.relative_to(PHASE).parts) & {'model', 'ghidra_conditional', '__pycache__'}]
for name in ['RecoverDump.java', 'ApplyConditionalGuards.java', 'ExportEvidence.java', 'index_code.py', 'triage.py', 'anchor_xrefs.py']:
    files.append(ROOT / 'scripts' / name)
for name in ['requirements.txt', 'REPRODUCE.md']:
    files.append(ROOT / name)
files.extend(path for path in (ROOT / 'imports').glob('*') if path.is_file())
files.extend(path for path in [ROOT / 'strings/anchors.csv', ROOT / 'strings/extract_compact.py'] if path.exists())
files = sorted(set(files))
manifest = '\n'.join(hashlib.sha256(path.read_bytes()).hexdigest() + '  ' + str(path) for path in files) + '\n'
analysis_zip = DESTINATION / 'cs2_phase2_analysis.zip'
with zipfile.ZipFile(analysis_zip, 'w', zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
    for path in files:
        archive.write(path, str(path))
    archive.writestr('MANIFEST_SHA256.txt', manifest)
    archive.writestr('START_HERE.txt', 'Read analysis/phase2/REPORT_RU.md and REPRODUCE.md. Partial static reconstruction, not a runnable DLL or recovered original source. Binary/model/tool installations are deliberately omitted.\n')
    archive.writestr('LIMITS.txt', 'Original raw sample was not executed. Conditional C depends on explicitly assumed environment predicates. External penetration/trace code is outside the supplied image. Mathematical checks are not binary behavioral-equivalence tests.\n')
project_root = PHASE / 'ghidra_conditional'
project_zip = DESTINATION / 'cs2_phase2_ghidra_conditional.zip'
project_files = sorted(path for path in project_root.rglob('*') if path.is_file() and not path.name.endswith(('.lock', '.ulock')))
project_manifest = []
with zipfile.ZipFile(project_zip, 'w', zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
    for path in project_files:
        name = str(path.relative_to(project_root))
        archive.write(path, name)
        project_manifest.append(hashlib.sha256(path.read_bytes()).hexdigest() + '  ' + name)
    for path in [PHASE / 'REPORT_RU.md', PHASE / 'main/conditional_model_manifest.json', PHASE / 'main/verification.json']:
        archive.write(path, path.name)
        project_manifest.append(hashlib.sha256(path.read_bytes()).hexdigest() + '  ' + path.name)
    archive.writestr('MANIFEST_SHA256.txt', '\n'.join(project_manifest) + '\n')
    archive.writestr('README_FIRST.txt', 'Open CONDITIONAL_PATH_MODEL.gpr in Ghidra 12.1.4. This is an ANALYSIS-ONLY assumed-success path model, NOT the unchanged original image and NOT a runnable DLL. 3089 recorded equality branches were changed. Runtime predicate assumptions have NOT been validated. See REPORT_RU.md and conditional_model_manifest.json. Original project from phase 1 remains separate and unchanged.\n')
results = []
for path in [analysis_zip, project_zip]:
    with zipfile.ZipFile(path) as archive:
        assert archive.testzip() is None
        names = set(archive.namelist())
        for line in archive.read('MANIFEST_SHA256.txt').decode().splitlines():
            expected, name = line.split('  ', 1)
            assert name in names
            assert hashlib.sha256(archive.read(name)).hexdigest() == expected
    results.append({'file': str(path), 'bytes': path.stat().st_size,
                    'sha256': hashlib.sha256(path.read_bytes()).hexdigest(), 'zip_and_manifest_verified': True})
(DESTINATION / 'phase2_delivery_manifest.json').write_text(json.dumps(results, indent=2) + '\n')
print(json.dumps(results, indent=2))
```
