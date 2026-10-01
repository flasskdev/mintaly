<!-- split-part | CS2_RESEARCH_MASTER.md lines 94754-94801 | body-sha256 351aaba890eda8f3e4e66afd3bc1f9aa43652916ab45fb537b768183c8f8746b -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-170"></a>

## E170. `analysis/scripts/package_results.py`

Bytes: 2736. SHA-256: `3e513abde37ac6654fc4499741435f449f211ec8857cc60423ca76c63795336a`.

```python
from pathlib import Path
import hashlib
import json
import zipfile

ROOT = Path('analysis')
DESTINATION = Path('deliverables')
DESTINATION.mkdir(exist_ok=True)
files = [ROOT / name for name in ['REPORT_RU.md','CORE_ADDENDUM_RU.md','REPRODUCE.md','requirements.txt']]
for directory in ['scripts','imports','results','decompiled_rng','decompiled_features','decompiled_core','ghidra_assembly','review_rng']:
    files.extend(path for path in (ROOT / directory).rglob('*') if path.is_file() and '__pycache__' not in path.parts)
for filename in ['summary.md','anchors.csv','anchors.json','coverage.json','evidence.csv','evidence.json','extract_compact.py','rtti.csv','schema_fields.csv']:
    files.append(ROOT / 'strings' / filename)
files = sorted(set(files))
manifest = '\n'.join(f'{hashlib.sha256(path.read_bytes()).hexdigest()}  {path.as_posix()}' for path in files) + '\n'
main_archive = DESTINATION / 'Neverlose_CS2_static_analysis.zip'
with zipfile.ZipFile(main_archive,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=6) as archive:
    for path in files:
        archive.write(path,path.as_posix())
    archive.writestr('MANIFEST_SHA256.txt',manifest)
project_archive = DESTINATION / 'Neverlose_CS2_Ghidra_project.zip'
with zipfile.ZipFile(project_archive,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=6) as archive:
    for path in sorted((ROOT / 'ghidra_project').rglob('*')):
        if path.is_file() and '.lock' not in path.name:
            archive.write(path,path.relative_to(ROOT).as_posix())
    archive.writestr('OPEN_PROJECT.txt','Ghidra 12.1.4 PUBLIC. Extract all files, then open ghidra_project/CS2_static.gpr with CS2_static.rep beside it. This is a static-analysis project, not a runnable DLL. Base 0x212C3300000. See the separate analysis archive for findings and limitations.\n')
report = DESTINATION / 'Neverlose_CS2_findings_RU.md'
report.write_text((ROOT / 'REPORT_RU.md').read_text() + '\n\n---\n\n' + (ROOT / 'CORE_ADDENDUM_RU.md').read_text() + '\n\n---\n\n' + (ROOT / 'review_rng/addendum_vector_sampling.md').read_text())
for archive_path in [main_archive,project_archive]:
    with zipfile.ZipFile(archive_path) as archive:
        assert archive.testzip() is None
with zipfile.ZipFile(main_archive) as archive:
    for row in archive.read('MANIFEST_SHA256.txt').decode().splitlines():
        expected, filename = row.split('  ',1)
        assert hashlib.sha256(archive.read(filename)).hexdigest() == expected
summary = [{'name':path.name,'size_bytes':path.stat().st_size,'sha256':hashlib.sha256(path.read_bytes()).hexdigest()} for path in [main_archive,project_archive,report]]
(DESTINATION / 'delivery_manifest.json').write_text(json.dumps(summary,indent=2))
print(json.dumps(summary,indent=2))
```
