<!-- split-part | CS2_RESEARCH_MASTER.md lines 7046-7168 | body-sha256 adaba16439258a706ba08c890e0bdb3dcb690c4682981a69e5f0d1b9a222aec9 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-020"></a>

## E020. `analysis/phase2/lagcomp/build_evidence.py`

Bytes: 11247. SHA-256: `5941aa1c776b8f093cc471a70b5343e58842d3dc3c71d8ac95f009d0c7ac89dc`.

```python
#!/usr/bin/env python3
import csv
import hashlib
import json
import mmap
import struct
from pathlib import Path

import capstone

from analyze import BASE, EXPECTED_SHA256

CLAIMS = [
    ("aging_cutoff", 0x4721D0, "Cutoff = cvttss2si((time_source - helper_473490()) * 64.0 + 0.5); source selected by object+0x28.",
     [0x4721F5, 0x4721FB, 0x472201, 0x472206, 0x472214, 0x472218, 0x47221C, 0x472225, 0x47222D, 0x472235]),
    ("aging_tail", 0x4721D0, "Scan slot[(head + candidate_count - 1) % 16].tick, reject old tails by signed JL; commit reduced count only on first surviving tail.",
     [0x472240, 0x472250, 0x472253, 0x472258, 0x47225F, 0x472263, 0x472275, 0x472279, 0x472283, 0x472287, 0x47228B, 0x472290, 0x472292, 0x472294, 0x472297]),
    ("aging_all_stale_caveat", 0x4721D0, "If candidate_count reaches zero, JLE skips the count store. This block does NOT prove count=0 for an all-stale ring.",
     [0x472246, 0x472250, 0x472253, 0x472275, 0x472290, 0x472297, 0x47229F]),
    ("mode_dispatch", 0x4721D0, "mode byte==0 enters timestamp guard; nonzero with count==0 enters insertion; other path has environment guards and slot+8 pruning.",
     [0x47229F, 0x4722A3, 0x4722A9, 0x4722AB, 0x4722B1]),
    ("mode_assignment", 0x6AC690, "Controller sets object+0x28=1 when object entity pointer equals comparison entity pointer; stores source time at +0x4C. Entity role is not identified.",
     [0x6AEB80, 0x6AEB84, 0x6AEB8B, 0x6AEB90, 0x6AEBCF, 0x6AEBD3, 0x6AEBD7, 0x6AEBDA, 0x6AEBF0]),
    ("prune_slot8_front", 0x4721D0, "Guarded mode path removes newest slots while slot+8 >= auxiliary source integer: decrement count and increment head modulo 16.",
     [0x4727A9, 0x4727AF, 0x4727BD, 0x4727C1, 0x4727F7, 0x4727FF, 0x472803, 0x472809, 0x47280B, 0x472811, 0x472820, 0x47282C, 0x472832]),
    ("tick_admission", 0x4721D0, "If nonempty and newest.tick >= cvttss2si(source_time*64+0.5), return without inserting. No fraction comparison here.",
     [0x472834, 0x47283A, 0x47283C, 0x47285E, 0x472862, 0x472869, 0x47286E, 0x472876, 0x47287E, 0x472882, 0x472887]),
    ("ring_insert", 0x4721D0, "New head=(head+15)%16; count incremented only when <=15, fresh empty path sets count=1. Slot=object+0xF0+head*0x500.",
     [0x472239, 0x4728A8, 0x4728AE, 0x4728B1, 0x4728B3, 0x4728BB, 0x4728C6, 0x4728D0, 0x4728E4, 0x4728E6, 0x4728E8, 0x4728EE, 0x4728F1, 0x4728F5, 0x4728F9]),
    ("slot_time_producer", 0x4721D0, "V_modff(source_time*64) produces integer/fraction, with negative-fraction borrow normalization; writes slot+0 and +4.",
     [0x4728FC, 0x472900, 0x472911, 0x47291F, 0x472929, 0x472936, 0x472938, 0x472949, 0x47294D, 0x472951, 0x472953]),
    ("slot_position_producer", 0x4721D0, "Read dynamic-offset entity component; write position XYZ at slot+0x14/+0x18/+0x1C.",
     [0x472958, 0x47295F, 0x472963, 0x47296A, 0x47296E, 0x472971, 0x472975]),
    ("slot_angles_producer", 0x4721D0, "Call 0x153120(entity); copy three floats to slot+0x44/+0x48/+0x4C. Consumer uses first two.",
     [0x4729C5, 0x4729C8, 0x4729CD, 0x4729D0, 0x4729D4, 0x4729D7]),
    ("slot_aux_integer", 0x4721D0, "slot+8 defaults to -1; mode==1 guarded path assigns [[opaque_root]+0x38]+8. Do not name it command number without further evidence.",
     [0x472B20, 0x472B27, 0x472B2B, 0x473013, 0x473017, 0x47301B, 0x47301E]),
    ("discontinuity", 0x4721D0, "When count>=2 compare newest/previous XYZ squared distance to float(4096*clamp(signed tick difference,1,5)); strict greater or slot+A0 mismatch => count=1.",
     [0x4732A1, 0x4732A8, 0x4732AE, 0x4732F2, 0x4732F9, 0x473300, 0x473307, 0x47330E, 0x473315, 0x473328, 0x47332D, 0x473332, 0x473335, 0x47333B, 0x47333F, 0x473343, 0x473348, 0x47334C, 0x47334F, 0x47335B, 0x47335F, 0x473367, 0x47336D, 0x473373, 0x473375]),
    ("slot_A0_collection_size", 0x475830, "slot+A0 is collection-size-like: source count controls copying count*0x20 bytes from pointer +B0, destination capacity +B8, then size is restored. Not an entity/model ID.",
     [0x475A95, 0x475A9B, 0x475AA2, 0x475AAC, 0x475AB3, 0x475AB8, 0x475ABF, 0x475AD0, 0x475AD7, 0x475B30, 0x475B37, 0x475B52, 0x475B5C, 0x475B6B]),
    ("payload_capture", 0x4721D0, "slot+A0 cleared; helper 0x378C80 receives object+0x5100 and slot+0xA0, then helper 0x473F30 receives slot and object. Capture semantics beyond this remain open.",
     [0x472A76, 0x472B19, 0x47307A, 0x473129, 0x473134, 0x47313C, 0x47313F, 0x473142, 0x473166, 0x473169, 0x47316F]),
    ("constructor", 0x4707F0, "Initialization object stride 0x53C0, zero ring storage and explicitly zero combined count/head qword.",
     [0x470830, 0x47083E, 0x47085E, 0x47088A, 0x470895, 0x47089A, 0x471003]),
    ("reset", 0x471D70, "After call 0x471B90, count/head qword is zeroed; additional state reset. Vtable pointer evidence is separate.",
     [0x471D78, 0x471D7D, 0x471D88, 0x471D8F, 0x471D96]),
    ("reset_inlined", 0x6CE200, "Same cleanup + count/head zero sequence appears inline in another function.",
     [0x6CE2F1, 0x6CE2F4, 0x6CE2F9, 0x6CE304]),
    ("ring_move", 0x6AC690, "16 iterations copy the slot prefixes and transfer dynamic buffers; then copy combined count/head. This is state movement, not a new time sample.",
     [0x6AE516, 0x6AE520, 0x6AE52A, 0x6AE534, 0x6AE53E, 0x6AE548, 0x6AE552, 0x6AE55C, 0x6AE566, 0x6AE570, 0x6AE578, 0x6AE634, 0x6AE63B, 0x6AE642, 0x6AE648, 0x6AE64F]),
    ("worker_dispatch", 0x475EF0, "Atomic work-item index at +0x40; each item calls 0x4721D0. Worker function pointer stored at RVA 0xEDFA70.",
     [0x475EF6, 0x475EFE, 0x475F03, 0x475F05, 0x475F10, 0x475F14, 0x475F18, 0x475F1D, 0x475F21, 0x475F2B]),
    ("additional_caller", 0x661010, "Refresh object fields, clear scratch counters +C0/+E0, then call the producer; call absent from original linear index.",
     [0x661B57, 0x661B5C, 0x661B91, 0x661B9B, 0x661BA5, 0x661BA8]),
    ("controller_dispatch", 0x6AC690, "One item calls producer directly; multiple items construct worker object with vtable at EDFA68 and submit through an indirect scheduler call.",
     [0x6AF8D9, 0x6AF8DC, 0x6AF8E6, 0x6AF8ED, 0x6AF916, 0x6AF934, 0x6AF953, 0x6AF957, 0x6AF95D, 0x6AF992]),
    ("copy_record", 0x475830, "Copies scalar prefix including tick/fraction/position/angles, then independently handles owned buffers. Not a raw memcpy of the whole slot.",
     [0x47584D, 0x475850, 0x475853, 0x475857, 0x47585C, 0x475861, 0x475866, 0x47586B, 0x475870, 0x475875, 0x475882]),
    ("derived_record_not_ring_insert", 0x474D20, "Requires nonempty ring, target time later than newest, scratch counter<=31. Copies newest to object+A8 scratch vector, writes target pos/time and byte+C=1. Not a ring producer.",
     [0x474D32, 0x474D39, 0x474D45, 0x474D66, 0x474D6D, 0x474D73, 0x474E25, 0x474E27, 0x474E2D, 0x474E33, 0x474E36, 0x474F48, 0x474F56, 0x474F6C, 0x475105, 0x47510B, 0x47519A, 0x47519C, 0x4751A1]),
    ("consumer_admission", 0x52B250, "Known consumer: distance2 <=62500, signed trunc(abs(time_delta_sec)*1000)<=200, inclusive time bounds, then direction split. Time argument at +C is int32, not float.",
     [0x52B31C, 0x52B38C, 0x52B4B1, 0x52B4DE, 0x52B4E5, 0x52B4E8, 0x52B533, 0x52B537, 0x52B53F, 0x52B547, 0x52B54F, 0x52B553, 0x52B559, 0x52B567, 0x52B618, 0x52B652, 0x52B664, 0x52B677]),
    ("aging_window_source", 0x473490, "Guarded helper returns scalar at [[opaque_root]+8]+0x58; parameter/cvar name and runtime value not resolved.",
     [0x4734A3, 0x47350A, 0x47350F, 0x4738A0, 0x4738A5, 0x473952, 0x473957, 0x47395B, 0x473978]),
]


def main():
    root = Path.cwd()
    output = root / "analysis/phase2/lagcomp"
    with (root / "analysis/input/cs2_212C3300000.bin").open("rb") as handle:
        raw = mmap.mmap(handle.fileno(), 0, access=mmap.ACCESS_READ)
    assert hashlib.sha256(raw).hexdigest() == EXPECTED_SHA256
    instructions = {int(row["rva"], 16): row for row in json.loads((output / "instructions.json").read_text())}
    claims = []
    flattened = []
    for claim_id, function, statement, addresses in CLAIMS:
        sites = []
        for address in addresses:
            assert address in instructions, f"Instruction boundary absent: {address:#x} ({claim_id})"
            row = instructions[address]
            assert int(row["function_rva"], 16) == function
            expected = bytes.fromhex(row["bytes"])
            assert raw[address:address + len(expected)] == expected
            site = {key: row[key] for key in ("rva", "va", "bytes", "mnemonic", "operands")}
            sites.append(site)
            flattened.append({"claim": claim_id, "function_rva": hex(function), "statement": statement, **site})
        claims.append({"id": claim_id, "function_rva": hex(function), "statement": statement, "sites": sites})
    constants = []
    for address, interpretation in [(0xE58838, "64.0 ticks/second"), (0xDEC7D8, "0.5 rounding offset"), (0xDEC898, "1.0"), (0xE06ABC, "-1.0 borrow"), (0xE9BB50, "62500.0 distance squared"), (0xE98AC0, "1/64 seconds/tick"), (0xE9AC44, "1000.0 milliseconds/second"), (0xE9ADB8, "direction split 0.475"), (0x17701F0, "unresolved dynamic source-time offset sentinel"), (0x177035C, "unresolved entity-component offset sentinel"), (0x176F8C8, "unresolved position offset sentinel")]:
        constants.append({"rva": hex(address), "bytes": raw[address:address + 4].hex(), "uint32": hex(int.from_bytes(raw[address:address + 4], "little")), "float32": struct.unpack_from("<f", raw, address)[0], "interpretation": interpretation})
    engine = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    for record in json.loads((output / "raw_branch_refs.json").read_text()):
        address = int(record["source_rva"], 16)
        assert raw[address:address + 5].hex() == record["bytes"]
        displacement = int.from_bytes(raw[address + 1:address + 5], "little", signed=True)
        assert address + 5 + displacement == int(record["target_rva"], 16)
    result = {"base": hex(BASE), "sha256": EXPECTED_SHA256,
              "assumptions": ["All facts are static; no sample execution or emulation", "Exact undecodable 0F 1A/1B/1C 24 10 may be advanced as 4-byte NOP per user/Ghidra observation", "Both conditional branch arms retained; no PEB/KUSER runtime values claimed", "Separate user conditional model assumes PEBhash==0x5877 and KUSERsum==0x92FB254D; not applied as runtime facts here", "Simplified numerical guards assume finite normalized times and valid count/head invariants"],
              "claims": claims, "constants": constants,
              "raw_branch_refs": json.loads((output / "raw_branch_refs.json").read_text()),
              "raw_function_pointers": json.loads((output / "raw_function_pointers.json").read_text())}
    (output / "evidence.json").write_text(json.dumps(result, indent=2) + "\n")
    with (output / "evidence.tsv").open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=["claim", "function_rva", "rva", "va", "bytes", "mnemonic", "operands", "statement"], delimiter="\t")
        writer.writeheader()
        writer.writerows(flattened)
    print(json.dumps({"claims": len(claims), "verified_instruction_rows": len(flattened), "constants": len(constants), "raw_unchanged_sha256": hashlib.sha256(raw).hexdigest()}, indent=2))


if __name__ == "__main__":
    main()
```
