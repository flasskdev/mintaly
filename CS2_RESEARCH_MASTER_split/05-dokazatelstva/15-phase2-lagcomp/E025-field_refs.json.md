<!-- split-part | CS2_RESEARCH_MASTER.md lines 10056-10837 | body-sha256 6e22cec955b8eeb13c6f28189394e018b156d46b05024b2a8a13fd220dcebde6 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-025"></a>

## E025. `analysis/phase2/lagcomp/field_refs.json`

Bytes: 20114. SHA-256: `cf1e03102f8400fae572d16d04bcd434a0664413f4298fbaa731c5238a4b86f7`.

```json
[
  {
    "function_rva": "0x37a020",
    "rva": "0x37a61a",
    "va": "0x212c367a61a",
    "bytes": "83b9f050000000",
    "mnemonic": "cmp",
    "operands": "dword ptr [rcx + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x37a020",
    "rva": "0x37a4ea",
    "va": "0x212c367a4ea",
    "bytes": "448b89f4500000",
    "mnemonic": "mov",
    "operands": "r9d, dword ptr [rcx + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x37a020",
    "rva": "0x37a4dd",
    "va": "0x212c367a4dd",
    "bytes": "83b9f050000000",
    "mnemonic": "cmp",
    "operands": "dword ptr [rcx + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x4707f0",
    "rva": "0x471003",
    "va": "0x212c3771003",
    "bytes": "48c786f050000000000000",
    "mnemonic": "mov",
    "operands": "qword ptr [rsi + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 2
  },
  {
    "function_rva": "0x471d70",
    "rva": "0x471d7d",
    "va": "0x212c3771d7d",
    "bytes": "48c786f050000000000000",
    "mnemonic": "mov",
    "operands": "qword ptr [rsi + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 2
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x472240",
    "va": "0x212c3772240",
    "bytes": "8b86f0500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rsi + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x472258",
    "va": "0x212c3772258",
    "bytes": "448b86f4500000",
    "mnemonic": "mov",
    "operands": "r8d, dword ptr [rsi + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x472297",
    "va": "0x212c3772297",
    "bytes": "898ef0500000",
    "mnemonic": "mov",
    "operands": "dword ptr [rsi + 0x50f0], ecx",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 2
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x472834",
    "va": "0x212c3772834",
    "bytes": "8b8ef4500000",
    "mnemonic": "mov",
    "operands": "ecx, dword ptr [rsi + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x4728a8",
    "va": "0x212c37728a8",
    "bytes": "898ef4500000",
    "mnemonic": "mov",
    "operands": "dword ptr [rsi + 0x50f4], ecx",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 2
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x4728b5",
    "va": "0x212c37728b5",
    "bytes": "8b8ef4500000",
    "mnemonic": "mov",
    "operands": "ecx, dword ptr [rsi + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x4728d0",
    "va": "0x212c37728d0",
    "bytes": "898ef4500000",
    "mnemonic": "mov",
    "operands": "dword ptr [rsi + 0x50f4], ecx",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 2
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x4728e8",
    "va": "0x212c37728e8",
    "bytes": "8986f0500000",
    "mnemonic": "mov",
    "operands": "dword ptr [rsi + 0x50f0], eax",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 2
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x4727a9",
    "va": "0x212c37727a9",
    "bytes": "8b86f0500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rsi + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x4727af",
    "va": "0x212c37727af",
    "bytes": "8b8ef4500000",
    "mnemonic": "mov",
    "operands": "ecx, dword ptr [rsi + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x47280b",
    "va": "0x212c377280b",
    "bytes": "8986f0500000",
    "mnemonic": "mov",
    "operands": "dword ptr [rsi + 0x50f0], eax",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 2
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x47282c",
    "va": "0x212c377282c",
    "bytes": "898ef4500000",
    "mnemonic": "mov",
    "operands": "dword ptr [rsi + 0x50f4], ecx",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 2
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x4732a1",
    "va": "0x212c37732a1",
    "bytes": "83bef050000002",
    "mnemonic": "cmp",
    "operands": "dword ptr [rsi + 0x50f0], 2",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x4732ae",
    "va": "0x212c37732ae",
    "bytes": "8b8ef4500000",
    "mnemonic": "mov",
    "operands": "ecx, dword ptr [rsi + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x4721d0",
    "rva": "0x473375",
    "va": "0x212c3773375",
    "bytes": "c786f050000001000000",
    "mnemonic": "mov",
    "operands": "dword ptr [rsi + 0x50f0], 1",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 2
  },
  {
    "function_rva": "0x474d20",
    "rva": "0x474d32",
    "va": "0x212c3774d32",
    "bytes": "83b9f050000000",
    "mnemonic": "cmp",
    "operands": "dword ptr [rcx + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x474d20",
    "rva": "0x474d45",
    "va": "0x212c3774d45",
    "bytes": "8b81f4500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rcx + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x48eab0",
    "rva": "0x48ed84",
    "va": "0x212c378ed84",
    "bytes": "8b80f0500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rax + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x48eab0",
    "rva": "0x48ee54",
    "va": "0x212c378ee54",
    "bytes": "8b80f4500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rax + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x4aad60",
    "rva": "0x4aaed8",
    "va": "0x212c37aaed8",
    "bytes": "8b87f4500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rdi + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x4b9600",
    "rva": "0x4b98b3",
    "va": "0x212c37b98b3",
    "bytes": "83b8f050000000",
    "mnemonic": "cmp",
    "operands": "dword ptr [rax + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x4b9600",
    "rva": "0x4b98c0",
    "va": "0x212c37b98c0",
    "bytes": "8b88f4500000",
    "mnemonic": "mov",
    "operands": "ecx, dword ptr [rax + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x4feba0",
    "rva": "0x4ffd3a",
    "va": "0x212c37ffd3a",
    "bytes": "418b8ef0500000",
    "mnemonic": "mov",
    "operands": "ecx, dword ptr [r14 + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x4feba0",
    "rva": "0x4ffd49",
    "va": "0x212c37ffd49",
    "bytes": "418b86f4500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [r14 + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x512280",
    "rva": "0x51265f",
    "va": "0x212c381265f",
    "bytes": "8b81f0500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rcx + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x512280",
    "rva": "0x512669",
    "va": "0x212c3812669",
    "bytes": "8b91f4500000",
    "mnemonic": "mov",
    "operands": "edx, dword ptr [rcx + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x51b480",
    "rva": "0x51b567",
    "va": "0x212c381b567",
    "bytes": "83bbf050000000",
    "mnemonic": "cmp",
    "operands": "dword ptr [rbx + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x51b480",
    "rva": "0x51b57b",
    "va": "0x212c381b57b",
    "bytes": "8b83f4500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rbx + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x51b480",
    "rva": "0x51b611",
    "va": "0x212c381b611",
    "bytes": "83bbf050000002",
    "mnemonic": "cmp",
    "operands": "dword ptr [rbx + 0x50f0], 2",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x51b480",
    "rva": "0x51b61a",
    "va": "0x212c381b61a",
    "bytes": "8b83f4500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rbx + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x51e4f0",
    "rva": "0x51ef14",
    "va": "0x212c381ef14",
    "bytes": "448bb6f0500000",
    "mnemonic": "mov",
    "operands": "r14d, dword ptr [rsi + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x51e4f0",
    "rva": "0x51ef96",
    "va": "0x212c381ef96",
    "bytes": "8b86f4500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rsi + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x52b250",
    "rva": "0x52b31c",
    "va": "0x212c382b31c",
    "bytes": "8baff0500000",
    "mnemonic": "mov",
    "operands": "ebp, dword ptr [rdi + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x52b250",
    "rva": "0x52b38c",
    "va": "0x212c382b38c",
    "bytes": "8b87f4500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rdi + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x52cd20",
    "rva": "0x52cda5",
    "va": "0x212c382cda5",
    "bytes": "458b81f0500000",
    "mnemonic": "mov",
    "operands": "r8d, dword ptr [r9 + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x52cd20",
    "rva": "0x52cdc2",
    "va": "0x212c382cdc2",
    "bytes": "458b91f4500000",
    "mnemonic": "mov",
    "operands": "r10d, dword ptr [r9 + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x52cd20",
    "rva": "0x52cf7c",
    "va": "0x212c382cf7c",
    "bytes": "486381f0500000",
    "mnemonic": "movsxd",
    "operands": "rax, dword ptr [rcx + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x52cd20",
    "rva": "0x52cfa4",
    "va": "0x212c382cfa4",
    "bytes": "8b89f4500000",
    "mnemonic": "mov",
    "operands": "ecx, dword ptr [rcx + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x52cd20",
    "rva": "0x52d6dd",
    "va": "0x212c382d6dd",
    "bytes": "418b9424f0500000",
    "mnemonic": "mov",
    "operands": "edx, dword ptr [r12 + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x52cd20",
    "rva": "0x52d773",
    "va": "0x212c382d773",
    "bytes": "458b8c24f4500000",
    "mnemonic": "mov",
    "operands": "r9d, dword ptr [r12 + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x52cd20",
    "rva": "0x52d8dc",
    "va": "0x212c382d8dc",
    "bytes": "458b9424f4500000",
    "mnemonic": "mov",
    "operands": "r10d, dword ptr [r12 + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x52dd20",
    "rva": "0x52df01",
    "va": "0x212c382df01",
    "bytes": "4183bef050000000",
    "mnemonic": "cmp",
    "operands": "dword ptr [r14 + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x52dd20",
    "rva": "0x52df0b",
    "va": "0x212c382df0b",
    "bytes": "418b86f4500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [r14 + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x52f030",
    "rva": "0x52f0b4",
    "va": "0x212c382f0b4",
    "bytes": "83bbf050000000",
    "mnemonic": "cmp",
    "operands": "dword ptr [rbx + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x665540",
    "rva": "0x66c60b",
    "va": "0x212c396c60b",
    "bytes": "83bef050000000",
    "mnemonic": "cmp",
    "operands": "dword ptr [rsi + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x665540",
    "rva": "0x66c9df",
    "va": "0x212c396c9df",
    "bytes": "83bef050000000",
    "mnemonic": "cmp",
    "operands": "dword ptr [rsi + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x665540",
    "rva": "0x66c836",
    "va": "0x212c396c836",
    "bytes": "8b86f4500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rsi + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x665540",
    "rva": "0x66c8a7",
    "va": "0x212c396c8a7",
    "bytes": "8b8ef0500000",
    "mnemonic": "mov",
    "operands": "ecx, dword ptr [rsi + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x665540",
    "rva": "0x66c912",
    "va": "0x212c396c912",
    "bytes": "448b86f4500000",
    "mnemonic": "mov",
    "operands": "r8d, dword ptr [rsi + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x6788a0",
    "rva": "0x678b9d",
    "va": "0x212c3978b9d",
    "bytes": "418b9c24f0500000",
    "mnemonic": "mov",
    "operands": "ebx, dword ptr [r12 + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x6788a0",
    "rva": "0x678bfa",
    "va": "0x212c3978bfa",
    "bytes": "418b8424f4500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [r12 + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x6ac690",
    "rva": "0x6ae648",
    "va": "0x212c39ae648",
    "bytes": "488b87f0500000",
    "mnemonic": "mov",
    "operands": "rax, qword ptr [rdi + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x6ac690",
    "rva": "0x6ae64f",
    "va": "0x212c39ae64f",
    "bytes": "49898424f0500000",
    "mnemonic": "mov",
    "operands": "qword ptr [r12 + 0x50f0], rax",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 2
  },
  {
    "function_rva": "0x6ac690",
    "rva": "0x6af3d6",
    "va": "0x212c39af3d6",
    "bytes": "83bef050000000",
    "mnemonic": "cmp",
    "operands": "dword ptr [rsi + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x6ac690",
    "rva": "0x6af3e3",
    "va": "0x212c39af3e3",
    "bytes": "8b86f4500000",
    "mnemonic": "mov",
    "operands": "eax, dword ptr [rsi + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x6ac690",
    "rva": "0x6b2d8e",
    "va": "0x212c39b2d8e",
    "bytes": "418b8df0500000",
    "mnemonic": "mov",
    "operands": "ecx, dword ptr [r13 + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x6ac690",
    "rva": "0x6b2d99",
    "va": "0x212c39b2d99",
    "bytes": "418b95f4500000",
    "mnemonic": "mov",
    "operands": "edx, dword ptr [r13 + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x6ac690",
    "rva": "0x6b2aa0",
    "va": "0x212c39b2aa0",
    "bytes": "8b89f0500000",
    "mnemonic": "mov",
    "operands": "ecx, dword ptr [rcx + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0x6ac690",
    "rva": "0x6b2abe",
    "va": "0x212c39b2abe",
    "bytes": "8b92f4500000",
    "mnemonic": "mov",
    "operands": "edx, dword ptr [rdx + 0x50f4]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f4",
    "access": 1
  },
  {
    "function_rva": "0x6ce200",
    "rva": "0x6ce2f9",
    "va": "0x212c39ce2f9",
    "bytes": "48c787f050000000000000",
    "mnemonic": "mov",
    "operands": "qword ptr [rdi + 0x50f0], 0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 2
  },
  {
    "function_rva": "0xa3d3a0",
    "rva": "0xa3dcc4",
    "va": "0x212c3d3dcc4",
    "bytes": "c4c1781181f0500000",
    "mnemonic": "vmovups",
    "operands": "xmmword ptr [r9 + 0x50f0], xmm0",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0xa3d3a0",
    "rva": "0xa3dd5b",
    "va": "0x212c3d3dd5b",
    "bytes": "4c8d87f0500000",
    "mnemonic": "lea",
    "operands": "r8, [rdi + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0xa42e40",
    "rva": "0xa42f64",
    "va": "0x212c3d42f64",
    "bytes": "488bbef0500000",
    "mnemonic": "mov",
    "operands": "rdi, qword ptr [rsi + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0xa42e40",
    "rva": "0xa42f74",
    "va": "0x212c3d42f74",
    "bytes": "488d9ef0500000",
    "mnemonic": "lea",
    "operands": "rbx, [rsi + 0x50f0]",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 1
  },
  {
    "function_rva": "0xa58da0",
    "rva": "0xa61a9a",
    "va": "0x212c3d61a9a",
    "bytes": "66c785f0500000ee8f",
    "mnemonic": "mov",
    "operands": "word ptr [rbp + 0x50f0], 0x8fee",
    "provenance": "entry_recursive_direct_cfg",
    "displacement": "0x50f0",
    "access": 2
  }
]
```
