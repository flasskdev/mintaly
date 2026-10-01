<!-- split-part | CS2_RESEARCH_MASTER.md lines 60429-60720 | body-sha256 3081d8954a2095ebcc6da1b3aa749afdd1e04a22486c6b8d515a17a12ecd277f -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-050"></a>

## E050. `analysis/phase2/main/verification.json`

Bytes: 5928. SHA-256: `88e9a31a56590a9583fa51264b49ee10c5fc02978d414aa135df150e65e83acd`.

```json
{
  "original_sha256": "3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27",
  "original_unchanged": true,
  "model_sha256": "bd4b358c86a23027dc72de102790018803d1dbe9aabfcbe789196f826d6f1f39",
  "branch_transforms_verified": 3089,
  "model_equals_original_plus_manifest_only": true,
  "runtime_environment_assumptions_verified": false,
  "conditional_assembly_rows_verified_against_model": 34842,
  "conditional_assembly_bytes_verified_against_model": 167577,
  "rows_different_from_original": 124,
  "decompilation_results": [
    {
      "rva": "0x51d910",
      "status": "ok",
      "directory": "conditional_bullet"
    },
    {
      "rva": "0x5248e0",
      "status": "ok",
      "directory": "conditional_bullet"
    },
    {
      "rva": "0x525830",
      "status": "ok",
      "directory": "conditional_bullet"
    },
    {
      "rva": "0x51c5f0",
      "status": "ok",
      "directory": "conditional_bullet"
    },
    {
      "rva": "0x515ca0",
      "status": "ok",
      "directory": "conditional_bullet"
    },
    {
      "rva": "0x512710",
      "status": "ok",
      "directory": "conditional_bullet"
    },
    {
      "rva": "0x4beb50",
      "status": "Exception while decompiling 212c37beb50: process: timeout",
      "directory": "conditional_bullet"
    },
    {
      "rva": "0xf57b0",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x1463a0",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x52ad40",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x528b90",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x5239d0",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x524a30",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x51bea0",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x52bb10",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x52dd20",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x51df10",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x511160",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x5114d0",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x530cd0",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x5302c0",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x50ce90",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x30d580",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x52f030",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x52e720",
      "status": "ok",
      "directory": "conditional_core"
    },
    {
      "rva": "0x7bdb90",
      "status": "ok",
      "directory": "conditional_damage"
    },
    {
      "rva": "0x4721d0",
      "status": "ok",
      "directory": "conditional_lag"
    },
    {
      "rva": "0x473490",
      "status": "ok",
      "directory": "conditional_lag"
    },
    {
      "rva": "0x6ac690",
      "status": "ok",
      "directory": "conditional_lag"
    },
    {
      "rva": "0x661010",
      "status": "ok",
      "directory": "conditional_lag"
    },
    {
      "rva": "0x475830",
      "status": "ok",
      "directory": "conditional_lag"
    },
    {
      "rva": "0x473f30",
      "status": "ok",
      "directory": "conditional_lag"
    },
    {
      "rva": "0x51e4f0",
      "status": "ok",
      "directory": "conditional_targets"
    },
    {
      "rva": "0x51b480",
      "status": "ok",
      "directory": "conditional_targets"
    },
    {
      "rva": "0x51dac0",
      "status": "ok",
      "directory": "conditional_trace"
    },
    {
      "rva": "0x2f19c0",
      "status": "ok",
      "directory": "conditional_trace"
    },
    {
      "rva": "0x2f2110",
      "status": "ok",
      "directory": "conditional_trace"
    },
    {
      "rva": "0x2f0dd0",
      "status": "ok",
      "directory": "conditional_trace"
    },
    {
      "rva": "0x51cf40",
      "status": "ok",
      "directory": "conditional_trace"
    },
    {
      "rva": "0x51c9c0",
      "status": "ok",
      "directory": "conditional_trace"
    },
    {
      "rva": "0x51d320",
      "status": "ok",
      "directory": "conditional_trace"
    },
    {
      "rva": "0x526290",
      "status": "ok",
      "directory": "conditional_trace"
    },
    {
      "rva": "0x475e90",
      "status": "ok",
      "directory": "conditional_trace"
    }
  ],
  "successful_c_outputs": 42,
  "external_addresses_outside_dump": [
    "0x7ffcef4a8db0",
    "0x7ffcef4a3e50",
    "0x7ffcef48e030",
    "0x7ffcef616040",
    "0x7ffcf0fe45a0"
  ],
  "placeholder_fields": [
    {
      "rva": "0x1758658",
      "u32": "0x13371337"
    },
    {
      "rva": "0x17586c4",
      "u32": "0x13371337"
    },
    {
      "rva": "0x1758290",
      "u32": "0x13371337"
    },
    {
      "rva": "0x1758680",
      "u32": "0x13371337"
    },
    {
      "rva": "0x176f7c8",
      "u32": "0x13371337"
    },
    {
      "rva": "0x17702d0",
      "u32": "0x13371337"
    },
    {
      "rva": "0x176fe34",
      "u32": "0x13371337"
    }
  ],
  "float_constants": {
    "0xe99580": 0.0020000000949949026,
    "0xe98820": 50.0,
    "0xe9adb4": 66.0,
    "0xe9bb30": 1.0,
    "0xe9bb34": 0.824999988079071,
    "0xe9bb40": 65.0,
    "0xe9bb44": 180.0,
    "0xe98144": 25.0,
    "0xe99930": 40.0,
    "0xe98a6c": 90.0,
    "0xe980c4": 0.4000000059604645,
    "0xe9adb8": 0.4749999940395355
  },
  "sample_executed": false,
  "behavioral_equivalence_claimed": false
}
```
