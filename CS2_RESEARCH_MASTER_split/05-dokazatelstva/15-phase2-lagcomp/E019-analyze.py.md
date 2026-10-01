<!-- split-part | CS2_RESEARCH_MASTER.md lines 6859-7045 | body-sha256 9d92662884ed3cfc1608a894ae611cdadad9fb3b4261aed4918f37b323e4e2eb -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-019"></a>

## E019. `analysis/phase2/lagcomp/analyze.py`

Bytes: 10363. SHA-256: `aeecf5ccbe1a57c7d5d0e04e689b68d824ab5f0774ea44fe072592e8f61bf82d`.

```python
#!/usr/bin/env python3
import argparse
import bisect
import csv
import hashlib
import json
import mmap
import sqlite3
from collections import deque
from pathlib import Path

import capstone

BASE = 0x212C3300000
EXPECTED_SHA256 = "3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27"
DEFAULT_FUNCTIONS = [0x378C80, 0x4707F0, 0x471B90, 0x471D70, 0x4721D0, 0x473490, 0x474D20, 0x475830, 0x475EF0, 0x4764D0, 0x52B250, 0x661010, 0x6AC690, 0x6CE200]


def main():
    parser = argparse.ArgumentParser(description="Static-only Capstone evidence extraction; never loads or executes the sample")
    parser.add_argument("--root", type=Path, default=Path.cwd())
    parser.add_argument("--functions", nargs="*", type=lambda value: int(value, 0), default=DEFAULT_FUNCTIONS)
    args = parser.parse_args()
    root = args.root.resolve()
    output = root / "analysis/phase2/lagcomp"
    output.mkdir(parents=True, exist_ok=True)
    raw_file = root / "analysis/input/cs2_212C3300000.bin"
    with raw_file.open("rb") as handle:
        data = mmap.mmap(handle.fileno(), 0, access=mmap.ACCESS_READ)
    digest = hashlib.sha256(data).hexdigest()
    if digest != EXPECTED_SHA256:
        raise SystemExit("Raw SHA-256 mismatch")
    database = sqlite3.connect(f"file:{root / 'analysis/results/code.sqlite'}?mode=ro", uri=True)
    ranges = database.execute("SELECT begin,end,decoded_end,instruction_count FROM functions ORDER BY begin").fetchall()
    starts = [record[0] for record in ranges]
    by_start = {record[0]: record for record in ranges}
    engine = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    engine.detail = True

    def owner(address):
        index = bisect.bisect_right(starts, address) - 1
        return ranges[index][0] if index >= 0 and address < ranges[index][1] else None

    decoded = {}
    failures = {}
    assumed_nops = {}

    def decode_function(begin):
        if begin in decoded:
            return decoded[begin]
        end = by_start[begin][1]
        instructions = {}
        undecodable = set()
        pending = deque([begin])
        while pending:
            address = pending.popleft()
            while begin <= address < end and address not in instructions:
                instruction = next(engine.disasm(data[address:min(address + 15, end)], BASE + address, count=1), None)
                if instruction is None:
                    if data[address:address + 4] in (b"\x0f\x1a\x24\x10", b"\x0f\x1b\x24\x10", b"\x0f\x1c\x24\x10"):
                        assumed_nops[address] = {"rva": hex(address), "function_rva": hex(begin), "bytes": data[address:address + 4].hex(), "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"}
                        address += 4
                        continue
                    undecodable.add(address)
                    break
                instructions[address] = instruction
                following = address + instruction.size
                if instruction.group(capstone.CS_GRP_JUMP):
                    if instruction.operands and instruction.operands[0].type == capstone.CS_OP_IMM:
                        target = instruction.operands[0].imm - BASE
                        if begin <= target < end:
                            pending.append(target)
                    if instruction.mnemonic in ("jmp", "ljmp"):
                        break
                if instruction.group(capstone.CS_GRP_RET) or instruction.mnemonic in ("ud2", "int3", "hlt"):
                    break
                address = following
        decoded[begin] = instructions
        failures[begin] = sorted(undecodable)
        return instructions

    def evidence(instruction, begin, provenance):
        address = instruction.address - BASE
        return {"function_rva": hex(begin) if begin is not None else None,
                "rva": hex(address), "va": hex(instruction.address),
                "bytes": instruction.bytes.hex(), "mnemonic": instruction.mnemonic,
                "operands": instruction.op_str, "provenance": provenance}

    patterns = []
    field_evidence = []
    field_owners = set()
    for displacement in (0x50F0, 0x50F4):
        address = 0
        while True:
            address = data.find(displacement.to_bytes(4, "little"), address)
            if address < 0:
                break
            begin = owner(address)
            patterns.append({"displacement": hex(displacement), "pattern_rva": hex(address), "owner_rva": hex(begin) if begin is not None else None})
            if begin is not None:
                field_owners.add(begin)
            address += 1
    for begin in sorted(field_owners):
        for instruction in decode_function(begin).values():
            for operand in instruction.operands:
                if operand.type == capstone.CS_OP_MEM and operand.mem.disp in (0x50F0, 0x50F4) and operand.mem.base != capstone.x86.X86_REG_RIP:
                    record = evidence(instruction, begin, "entry_recursive_cfg_with_explicit_nop_assumptions")
                    record.update(displacement=hex(operand.mem.disp), access=operand.access)
                    field_evidence.append(record)
    branch_refs = []
    pointer_refs = []
    branch_targets = {0x4707F0, 0x471B90, 0x471D70, 0x4721D0, 0x473490, 0x475EF0, 0x474D20}
    for opcode in (b"\xe8", b"\xe9"):
        address = 0
        while True:
            address = data.find(opcode, address)
            if address < 0 or address + 5 > len(data):
                break
            target = address + 5 + int.from_bytes(data[address + 1:address + 5], "little", signed=True)
            if target in branch_targets:
                begin = owner(address)
                instruction = decode_function(begin).get(address) if begin is not None else None
                record = {"source_rva": hex(address), "target_rva": hex(target), "owner_rva": hex(begin) if begin is not None else None,
                          "bytes": data[address:address + 5].hex(), "opcode": opcode.hex(),
                          "entry_cfg_boundary": bool(instruction and instruction.bytes == data[address:address + 5]),
                          "present_in_sqlite_refs": bool(database.execute("SELECT 1 FROM refs WHERE source=? AND target=? LIMIT 1", (address, target)).fetchone()),
                          "beyond_sqlite_linear_end": address >= by_start[begin][2] if begin is not None else None}
                branch_refs.append(record)
            address += 1
    for target in sorted(branch_targets):
        address = 0
        while True:
            address = data.find((BASE + target).to_bytes(8, "little"), address)
            if address < 0:
                break
            pointer_refs.append({"storage_rva": hex(address), "target_rva": hex(target), "bytes": data[address:address + 8].hex(), "meaning": "raw pointer, not proof of executed call"})
            address += 1
    requested = sorted(set(args.functions))
    function_summaries = []
    all_instructions = []
    callers = []
    for begin in requested:
        if begin not in by_start:
            raise SystemExit(f"Function boundary missing: {begin:#x}")
        instructions = decode_function(begin)
        begin, end, decoded_end, instruction_count = by_start[begin]
        function_summaries.append({"begin": hex(begin), "end": hex(end), "sqlite_decoded_end": hex(decoded_end), "sqlite_instruction_count": instruction_count,
                                   "cfg_instruction_count": len(instructions), "undecodable_rvas": [hex(value) for value in failures[begin]]})
        assembly = []
        for address, instruction in sorted(instructions.items()):
            assembly.append(f"{address:08x} {instruction.bytes.hex():30s} {instruction.mnemonic:10s} {instruction.op_str}")
            all_instructions.append(evidence(instruction, begin, "entry_recursive_cfg_with_explicit_nop_assumptions"))
        (output / f"{begin:08x}.asm").write_text("\n".join(assembly) + "\n")
        for source, caller, mnemonic, operands, kind in database.execute("SELECT source,owner,mnemonic,operands,kind FROM refs WHERE target=?", (begin,)):
            instruction = next(engine.disasm(data[source:source + 15], BASE + source, count=1), None)
            record = evidence(instruction, caller, "sqlite_ref_locally_redecoded")
            record.update(target_rva=hex(begin), kind=kind)
            callers.append(record)
    metadata = {"base": hex(BASE), "raw": str(raw_file.relative_to(root)), "raw_size": len(data), "sha256": digest,
                "capstone_version": capstone.__version__, "sqlite_function_ranges": len(ranges),
                "sqlite_fully_linear_decoded_ranges": sum(end == decoded_end for begin, end, decoded_end, count in ranges),
                "method": "read-only raw + SQLite; bounded recursive direct-branch decoding; only undecodable exact 0F 1A/1B/1C 24 10 sequences treated as 4-byte NOP under explicit user/Ghidra assumption; no execution/emulation/guard forcing; all other invalid instructions, traps and indirect jumps unresolved; reachability is syntactic, not environment-proven",
                "assumed_nops": list(assumed_nops.values()), "functions": function_summaries}
    for name, payload in [("metadata.json", metadata), ("field_refs.json", field_evidence), ("raw_patterns.json", patterns), ("callers.json", callers), ("raw_branch_refs.json", branch_refs), ("raw_function_pointers.json", pointer_refs), ("instructions.json", all_instructions)]:
        (output / name).write_text(json.dumps(payload, indent=2) + "\n")
    with (output / "field_refs.tsv").open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=["function_rva", "rva", "va", "bytes", "mnemonic", "operands", "provenance", "displacement", "access"], delimiter="\t")
        writer.writeheader()
        writer.writerows(field_evidence)
    print(json.dumps({"sha256": digest, "ranges": len(ranges), "full_linear": metadata["sqlite_fully_linear_decoded_ranges"], "field_refs": len(field_evidence), "functions_written": len(function_summaries)}, indent=2))
    for record in field_evidence:
        if int(record["function_rva"], 16) < 0x700000:
            print(record["function_rva"], record["rva"], record["bytes"], record["mnemonic"], record["operands"])


if __name__ == "__main__":
    main()
```
