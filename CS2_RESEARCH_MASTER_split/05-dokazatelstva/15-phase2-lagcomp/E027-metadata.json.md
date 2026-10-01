<!-- split-part | CS2_RESEARCH_MASTER.md lines 10919-12374 | body-sha256 686b81c969ddb96e5f81caf079c55a66bd52bbecb163e78ae22a5eba5ee81f3a -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-027"></a>

## E027. `analysis/phase2/lagcomp/metadata.json`

Bytes: 47827. SHA-256: `9e3b7b0f55dd770c163b6b76f8d29b6534a95866cba4341f985dc6cd6fe0b547`.

```json
{
  "base": "0x212c3300000",
  "raw": "analysis/input/cs2_212C3300000.bin",
  "raw_size": 83890176,
  "sha256": "3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27",
  "capstone_version": "5.0.7",
  "sqlite_function_ranges": 45064,
  "sqlite_fully_linear_decoded_ranges": 43604,
  "method": "read-only raw + SQLite; bounded recursive direct-branch decoding; only undecodable exact 0F 1A/1B/1C 24 10 sequences treated as 4-byte NOP under explicit user/Ghidra assumption; no execution/emulation/guard forcing; all other invalid instructions, traps and indirect jumps unresolved; reachability is syntactic, not environment-proven",
  "assumed_nops": [
    {
      "rva": "0x37a101",
      "function_rva": "0x37a020",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x37a109",
      "function_rva": "0x37a020",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x37a110",
      "function_rva": "0x37a020",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x37a1d0",
      "function_rva": "0x37a020",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x37a1d7",
      "function_rva": "0x37a020",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x37a2e1",
      "function_rva": "0x37a020",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x37a2fe",
      "function_rva": "0x37a020",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x37a3c4",
      "function_rva": "0x37a020",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x37a3cb",
      "function_rva": "0x37a020",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x47230f",
      "function_rva": "0x4721d0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x4726bd",
      "function_rva": "0x4721d0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x47277e",
      "function_rva": "0x4721d0",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x472793",
      "function_rva": "0x4721d0",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x4727a0",
      "function_rva": "0x4721d0",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x472f4c",
      "function_rva": "0x4721d0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x4b9703",
      "function_rva": "0x4b9600",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x4ff46b",
      "function_rva": "0x4feba0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x4ff47a",
      "function_rva": "0x4feba0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x4ff52a",
      "function_rva": "0x4feba0",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x4ff52e",
      "function_rva": "0x4feba0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x4ff53f",
      "function_rva": "0x4feba0",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x5021da",
      "function_rva": "0x4feba0",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x502667",
      "function_rva": "0x4feba0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x5049b2",
      "function_rva": "0x4feba0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x504d01",
      "function_rva": "0x4feba0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x504d05",
      "function_rva": "0x4feba0",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x504d17",
      "function_rva": "0x4feba0",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x504dcf",
      "function_rva": "0x4feba0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x504eb9",
      "function_rva": "0x4feba0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x50531b",
      "function_rva": "0x4feba0",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x505325",
      "function_rva": "0x4feba0",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x51235f",
      "function_rva": "0x512280",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x512372",
      "function_rva": "0x512280",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x512412",
      "function_rva": "0x512280",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x512420",
      "function_rva": "0x512280",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x5124ff",
      "function_rva": "0x512280",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x51250b",
      "function_rva": "0x512280",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x512517",
      "function_rva": "0x512280",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x5125a7",
      "function_rva": "0x512280",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x5125bc",
      "function_rva": "0x512280",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x51e858",
      "function_rva": "0x51e4f0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x51e85f",
      "function_rva": "0x51e4f0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x51ec18",
      "function_rva": "0x51e4f0",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x51eceb",
      "function_rva": "0x51e4f0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x51ed0e",
      "function_rva": "0x51e4f0",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x51f3a8",
      "function_rva": "0x51e4f0",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x51f7d3",
      "function_rva": "0x51e4f0",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x51f7da",
      "function_rva": "0x51e4f0",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66563a",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66572c",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x665784",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6657a6",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x665887",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x665b06",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x665b17",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x665b1b",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x665bd1",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x666afa",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6663fe",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x667eee",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x667f0e",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x667adb",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x667ae2",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6672d1",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6672db",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6672e3",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x666be3",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x666be7",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6694b7",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66876b",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66876f",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x668c8c",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6698b8",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x669c4a",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x669c5d",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x669c65",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x669c6d",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6693ec",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6693f7",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6687fd",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x669997",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66a07c",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66aab1",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66aab9",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66abb1",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66ac18",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66ac1c",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66ac24",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66bcc2",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66bcc6",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66bcca",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66bce5",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66afbb",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66bdbd",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66bdd3",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66be36",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66b053",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66b05f",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66b0c9",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66b0e0",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66b19c",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66b1a4",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66d985",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66daab",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66dc15",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66dc2f",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66c3e3",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66c401",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66dd1a",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66dd33",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66c4be",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66fd5d",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66de97",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66c68a",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66f63f",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66e965",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66e969",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66e318",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66e3c9",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66e3e8",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x670ec0",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x670ed6",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x67019b",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6701b0",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6701b7",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66e7ad",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66ed06",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66ed24",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x670389",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66e85f",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66e863",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66e867",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66e882",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x671353",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x67135d",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66edc7",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x67167a",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x67167e",
      "function_rva": "0x665540",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x67168c",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66f28a",
      "function_rva": "0x665540",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66cf71",
      "function_rva": "0x665540",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x678d7d",
      "function_rva": "0x6788a0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x678d98",
      "function_rva": "0x6788a0",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x678e64",
      "function_rva": "0x6788a0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x678e70",
      "function_rva": "0x6788a0",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x678e7b",
      "function_rva": "0x6788a0",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ac777",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ac77e",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ac850",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6aca30",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6aca4c",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ad09f",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6acdb2",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ad1e5",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ad1ef",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ad5c6",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ad68e",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ad69c",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ad6a4",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ad748",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ad755",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6adbc3",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b0324",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b032e",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6add12",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b03ce",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b03e6",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b0480",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b048a",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b0a87",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b0523",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ae0d5",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ae19d",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b16df",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b16e3",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b17ef",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b0c03",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b0cd9",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b0ce3",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b0e3b",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b0e46",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b0e4d",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6ae97e",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b0f4d",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6aea7d",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6aea88",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b10fb",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6aeeb0",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6aeeb7",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6af270",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6af292",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6af36d",
      "function_rva": "0x6ac690",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b207f",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b2229",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b291e",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b2926",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b27d5",
      "function_rva": "0x6ac690",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6b29d9",
      "function_rva": "0x6ac690",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x476542",
      "function_rva": "0x4764d0",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x47655a",
      "function_rva": "0x4764d0",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x476565",
      "function_rva": "0x4764d0",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x47690b",
      "function_rva": "0x4764d0",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x476926",
      "function_rva": "0x4764d0",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x47692a",
      "function_rva": "0x4764d0",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x476a03",
      "function_rva": "0x4764d0",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x661485",
      "function_rva": "0x661010",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6614a4",
      "function_rva": "0x661010",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66153b",
      "function_rva": "0x661010",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x661557",
      "function_rva": "0x661010",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66155e",
      "function_rva": "0x661010",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6615ea",
      "function_rva": "0x661010",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x66170f",
      "function_rva": "0x661010",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x661a05",
      "function_rva": "0x661010",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x6617fc",
      "function_rva": "0x661010",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x661810",
      "function_rva": "0x661010",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x661acf",
      "function_rva": "0x661010",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x378cf5",
      "function_rva": "0x378c80",
      "bytes": "0f1b2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x473895",
      "function_rva": "0x473490",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x473941",
      "function_rva": "0x473490",
      "bytes": "0f1a2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    },
    {
      "rva": "0x473948",
      "function_rva": "0x473490",
      "bytes": "0f1c2410",
      "interpretation": "4-byte NOP per user/Ghidra observation; only decoder advance, raw unchanged"
    }
  ],
  "functions": [
    {
      "begin": "0x378c80",
      "end": "0x379248",
      "sqlite_decoded_end": "0x378cf5",
      "sqlite_instruction_count": 21,
      "cfg_instruction_count": 309,
      "undecodable_rvas": []
    },
    {
      "begin": "0x4707f0",
      "end": "0x4711a1",
      "sqlite_decoded_end": "0x4711a1",
      "sqlite_instruction_count": 357,
      "cfg_instruction_count": 357,
      "undecodable_rvas": []
    },
    {
      "begin": "0x471b90",
      "end": "0x471d6d",
      "sqlite_decoded_end": "0x471d6d",
      "sqlite_instruction_count": 72,
      "cfg_instruction_count": 72,
      "undecodable_rvas": []
    },
    {
      "begin": "0x471d70",
      "end": "0x471da0",
      "sqlite_decoded_end": "0x471da0",
      "sqlite_instruction_count": 11,
      "cfg_instruction_count": 11,
      "undecodable_rvas": []
    },
    {
      "begin": "0x4721d0",
      "end": "0x47345c",
      "sqlite_decoded_end": "0x47230f",
      "sqlite_instruction_count": 74,
      "cfg_instruction_count": 1107,
      "undecodable_rvas": []
    },
    {
      "begin": "0x473490",
      "end": "0x473987",
      "sqlite_decoded_end": "0x473895",
      "sqlite_instruction_count": 219,
      "cfg_instruction_count": 282,
      "undecodable_rvas": []
    },
    {
      "begin": "0x474d20",
      "end": "0x4751a7",
      "sqlite_decoded_end": "0x4751a7",
      "sqlite_instruction_count": 260,
      "cfg_instruction_count": 260,
      "undecodable_rvas": []
    },
    {
      "begin": "0x475830",
      "end": "0x475c59",
      "sqlite_decoded_end": "0x475c59",
      "sqlite_instruction_count": 258,
      "cfg_instruction_count": 258,
      "undecodable_rvas": []
    },
    {
      "begin": "0x475ef0",
      "end": "0x475f3b",
      "sqlite_decoded_end": "0x475f3b",
      "sqlite_instruction_count": 24,
      "cfg_instruction_count": 24,
      "undecodable_rvas": []
    },
    {
      "begin": "0x4764d0",
      "end": "0x476c3a",
      "sqlite_decoded_end": "0x476542",
      "sqlite_instruction_count": 25,
      "cfg_instruction_count": 430,
      "undecodable_rvas": []
    },
    {
      "begin": "0x52b250",
      "end": "0x52b852",
      "sqlite_decoded_end": "0x52b852",
      "sqlite_instruction_count": 343,
      "cfg_instruction_count": 343,
      "undecodable_rvas": []
    },
    {
      "begin": "0x661010",
      "end": "0x661bc0",
      "sqlite_decoded_end": "0x661485",
      "sqlite_instruction_count": 243,
      "cfg_instruction_count": 697,
      "undecodable_rvas": []
    },
    {
      "begin": "0x6ac690",
      "end": "0x6b3406",
      "sqlite_decoded_end": "0x6ac777",
      "sqlite_instruction_count": 43,
      "cfg_instruction_count": 6146,
      "undecodable_rvas": []
    },
    {
      "begin": "0x6ce200",
      "end": "0x6ce71a",
      "sqlite_decoded_end": "0x6ce71a",
      "sqlite_instruction_count": 277,
      "cfg_instruction_count": 277,
      "undecodable_rvas": []
    }
  ]
}
```
