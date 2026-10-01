<!-- split-part | CS2_RESEARCH_MASTER.md lines 12375-12625 | body-sha256 cae999acc7acc70495564497718a67ab98a5f0cb8aa151006152b2c2d0c5b063 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-028"></a>

## E028. `analysis/phase2/lagcomp/metadata_strict_capstone.json`

Bytes: 6189. SHA-256: `3f058f7e791dfe73f2efd45e31c3d372a87e54f65518b3e82abdbf5f49a9305a`.

```json
{
  "base": "0x212c3300000",
  "raw": "analysis/input/cs2_212C3300000.bin",
  "raw_size": 83890176,
  "sha256": "3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27",
  "capstone_version": "5.0.7",
  "sqlite_function_ranges": 45064,
  "sqlite_fully_linear_decoded_ranges": 43604,
  "method": "read-only raw + SQLite; bounded recursive direct-branch decoding, no execution/emulation; indirect jumps, traps and invalid instructions are unresolved",
  "functions": [
    {
      "begin": "0x37a020",
      "end": "0x37a62c",
      "sqlite_decoded_end": "0x37a101",
      "sqlite_instruction_count": 57,
      "cfg_instruction_count": 124,
      "undecodable_rvas": [
        "0x37a101"
      ]
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
      "cfg_instruction_count": 727,
      "undecodable_rvas": [
        "0x47230f",
        "0x472f4c"
      ]
    },
    {
      "begin": "0x473490",
      "end": "0x473987",
      "sqlite_decoded_end": "0x473895",
      "sqlite_instruction_count": 219,
      "cfg_instruction_count": 219,
      "undecodable_rvas": [
        "0x473895"
      ]
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
      "begin": "0x48eab0",
      "end": "0x48f83a",
      "sqlite_decoded_end": "0x48f83a",
      "sqlite_instruction_count": 864,
      "cfg_instruction_count": 851,
      "undecodable_rvas": []
    },
    {
      "begin": "0x4aad60",
      "end": "0x4ab004",
      "sqlite_decoded_end": "0x4ab004",
      "sqlite_instruction_count": 157,
      "cfg_instruction_count": 157,
      "undecodable_rvas": []
    },
    {
      "begin": "0x4b9600",
      "end": "0x4ba0ae",
      "sqlite_decoded_end": "0x4b9703",
      "sqlite_instruction_count": 56,
      "cfg_instruction_count": 572,
      "undecodable_rvas": [
        "0x4b9703"
      ]
    },
    {
      "begin": "0x4feba0",
      "end": "0x505404",
      "sqlite_decoded_end": "0x4ff46b",
      "sqlite_instruction_count": 498,
      "cfg_instruction_count": 496,
      "undecodable_rvas": [
        "0x4ff46b"
      ]
    },
    {
      "begin": "0x512280",
      "end": "0x51270d",
      "sqlite_decoded_end": "0x51235f",
      "sqlite_instruction_count": 44,
      "cfg_instruction_count": 61,
      "undecodable_rvas": [
        "0x51235f"
      ]
    },
    {
      "begin": "0x51b480",
      "end": "0x51b72a",
      "sqlite_decoded_end": "0x51b72a",
      "sqlite_instruction_count": 150,
      "cfg_instruction_count": 150,
      "undecodable_rvas": []
    },
    {
      "begin": "0x51e4f0",
      "end": "0x5231d8",
      "sqlite_decoded_end": "0x51e858",
      "sqlite_instruction_count": 177,
      "cfg_instruction_count": 195,
      "undecodable_rvas": [
        "0x51e858"
      ]
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
      "begin": "0x52cd20",
      "end": "0x52dbb2",
      "sqlite_decoded_end": "0x52dbb2",
      "sqlite_instruction_count": 790,
      "cfg_instruction_count": 783,
      "undecodable_rvas": []
    },
    {
      "begin": "0x52dd20",
      "end": "0x52e709",
      "sqlite_decoded_end": "0x52e709",
      "sqlite_instruction_count": 519,
      "cfg_instruction_count": 519,
      "undecodable_rvas": []
    },
    {
      "begin": "0x52f030",
      "end": "0x52f447",
      "sqlite_decoded_end": "0x52f447",
      "sqlite_instruction_count": 218,
      "cfg_instruction_count": 215,
      "undecodable_rvas": []
    },
    {
      "begin": "0x665540",
      "end": "0x671dbc",
      "sqlite_decoded_end": "0x66563a",
      "sqlite_instruction_count": 45,
      "cfg_instruction_count": 81,
      "undecodable_rvas": [
        "0x66563a"
      ]
    },
    {
      "begin": "0x6788a0",
      "end": "0x679148",
      "sqlite_decoded_end": "0x678d7d",
      "sqlite_instruction_count": 263,
      "cfg_instruction_count": 304,
      "undecodable_rvas": [
        "0x678d7d"
      ]
    },
    {
      "begin": "0x6ac690",
      "end": "0x6b3406",
      "sqlite_decoded_end": "0x6ac777",
      "sqlite_instruction_count": 43,
      "cfg_instruction_count": 43,
      "undecodable_rvas": [
        "0x6ac777"
      ]
    },
    {
      "begin": "0x6ce200",
      "end": "0x6ce71a",
      "sqlite_decoded_end": "0x6ce71a",
      "sqlite_instruction_count": 277,
      "cfg_instruction_count": 277,
      "undecodable_rvas": []
    },
    {
      "begin": "0xa3d3a0",
      "end": "0xa3fc69",
      "sqlite_decoded_end": "0xa3fc69",
      "sqlite_instruction_count": 2048,
      "cfg_instruction_count": 2048,
      "undecodable_rvas": []
    },
    {
      "begin": "0xa42e40",
      "end": "0xa435b4",
      "sqlite_decoded_end": "0xa435b4",
      "sqlite_instruction_count": 447,
      "cfg_instruction_count": 445,
      "undecodable_rvas": []
    },
    {
      "begin": "0xa58da0",
      "end": "0xa919bc",
      "sqlite_decoded_end": "0xa919bc",
      "sqlite_instruction_count": 25458,
      "cfg_instruction_count": 25450,
      "undecodable_rvas": []
    }
  ]
}
```
