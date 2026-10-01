<!-- split-part | CS2_RESEARCH_MASTER.md lines 81025-81095 | body-sha256 f1adfed58a8398a1ece123a77a72d646f7302c7079b59c71a3e941e1855ff6e6 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-157"></a>

## E157. `analysis/review_rng/collect_evidence.py`

Bytes: 3510. SHA-256: `496b0927585c11bb6ae31121a8e93474dcf9c13b6aa77cca54fe1fb769815c5e`.

```python
from pathlib import Path
import hashlib
import json
import mmap
import re
import sqlite3
import struct

BASE = 0x212C3300000
ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "analysis/review_rng"
SOURCE = ROOT / "analysis/decompiled_rng"
DUMP = ROOT / "analysis/input/cs2_212C3300000.bin"
DATABASE = ROOT / "analysis/results/code.sqlite"
connection = sqlite3.connect(DATABASE.as_uri() + "?mode=ro&immutable=1", uri=True)
connection.row_factory = sqlite3.Row
connection.execute("PRAGMA query_only=ON")
functions = []
constant_users = {}
source_hashes = {}
for path in sorted(SOURCE.glob("*.c")):
    text = path.read_text()
    source_hashes[str(path.relative_to(ROOT))] = hashlib.sha256(path.read_bytes()).hexdigest()
    address = int(path.stem, 16)
    rva = address - BASE
    metadata = connection.execute("SELECT * FROM functions WHERE begin=?", (rva,)).fetchone()
    outgoing = connection.execute("SELECT * FROM refs WHERE owner=? ORDER BY source LIMIT 2049", (rva,)).fetchall()
    incoming = connection.execute("SELECT * FROM refs WHERE target=? ORDER BY source LIMIT 129", (rva,)).fetchall()
    functions.append({"va": hex(address), "rva": hex(rva), "metadata": dict(metadata) if metadata else None,
                      "refs_truncated": len(outgoing) > 2048, "refs": [dict(row) for row in outgoing[:2048]],
                      "incoming_truncated": len(incoming) > 128, "incoming": [dict(row) for row in incoming[:128]]})
    for line_number, line in enumerate(text.splitlines(), 1):
        for match in re.finditer(r"(?:_?DAT_|[a-zA-Z]+Ram0*)(212c[0-9a-f]+)", line):
            constant_users.setdefault(int(match.group(1), 16), []).append(f"{path.name}:{line_number}")
with DUMP.open("rb") as handle:
    image = mmap.mmap(handle.fileno(), 0, access=mmap.ACCESS_READ)
    constants = []
    for address, users in sorted(constant_users.items()):
        offset = address - BASE
        if 0 <= offset <= len(image) - 16:
            raw = image[offset:offset + 16]
            constants.append({"va": hex(address), "offset": hex(offset), "bytes16": raw.hex(" "),
                              "u32": hex(struct.unpack("<I", raw[:4])[0]),
                              "f32": repr(struct.unpack("<f", raw[:4])[0]),
                              "u64": hex(struct.unpack("<Q", raw[:8])[0]),
                              "f64": repr(struct.unpack("<d", raw[:8])[0]), "uses": users})
    image.close()
manifest = {"base": hex(BASE), "dump_size": DUMP.stat().st_size, "sqlite_addresses": "RVA", "source_sha256": source_hashes,
            "db_mode": "mode=ro&immutable=1; query_only=ON", "functions": functions}
(OUT / "refs.json").write_text(json.dumps(manifest, indent=2) + "\n")
(OUT / "constants.json").write_text(json.dumps(constants, indent=2) + "\n")
lines = ["va\toffset\tbytes16\tf32\tu64\tuses"]
for constant in constants:
    lines.append("\t".join([constant["va"], constant["offset"], constant["bytes16"], constant["f32"], constant["u64"], ",".join(constant["uses"])]))
(OUT / "constants.tsv").write_text("\n".join(lines) + "\n")
for function in functions:
    meta = function["metadata"]
    print(function["va"], function["rva"], "end", hex(meta["end"]) if meta else None, "refs", len(function["refs"]), "incoming", len(function["incoming"]))
    for ref in function["incoming"]:
        print("  incoming", hex(BASE + ref["source"]), "owner", hex(BASE + ref["owner"]) if ref["owner"] is not None else None, ref["kind"], ref["operands"])
print("Constants:", len(constants))
```
