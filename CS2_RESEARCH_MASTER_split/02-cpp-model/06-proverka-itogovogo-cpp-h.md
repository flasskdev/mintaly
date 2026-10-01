<!-- split-part | CS2_RESEARCH_MASTER.md lines 974-1481 | body-sha256 47493b7945c1d0a8d8c54941e7fd2dfbd4a766ba0b0b1f5dd52ddf9bf72c93b4 -->
[← все части](../README.md) · [индекс порта](00-index.md)

<!-- split-body-start -->


### Проверка именно итогового большого CPP/H

```json
{
  "status": "passed",
  "scope": "Final combined artifacts: text integrity, independent C++17 model and compiled evidence catalog. Not binary behavioral equivalence.",
  "sample_binary_executed": false,
  "compiler": "g++ (Debian 14.2.0-19) 14.2.0",
  "integrity": {
    "cpp_bodies_verified_against_sources": 218,
    "md_bodies_verified_against_sources": 197,
    "original_reports_preserved": 13,
    "c_listings_preserved": 63,
    "asm_listings_preserved": 125,
    "report_fragments_preserved": 29,
    "source_python_model_preserved": true,
    "original_bin_sha256": "3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27",
    "cpp_sha256": "f570809c9b68f33f5fba58e7d8abc11054874af2bbf3f3ae43531c59c1237996",
    "header_sha256": "2e08dbcff8d552dc9c56c21f8f6e3db1428bea06c2c13a9894306f1f95460f83"
  },
  "compiled_catalog_records_verified": 218,
  "missing_external_dependencies_verified": 5,
  "commands": [
    {
      "label": "Compile final combined CPP with self-test enabled",
      "command": "g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -fno-fast-math -ffp-contract=off -DCS2_RECONSTRUCTION_SELF_TEST analysis/consolidated/CS2_RECONSTRUCTION.cpp -o analysis/consolidated/qa/final_self_test",
      "exit_code": 0,
      "stderr": ""
    },
    {
      "label": "Run only the independent mathematical model",
      "command": "analysis/consolidated/qa/final_self_test",
      "exit_code": 0,
      "stderr": ""
    },
    {
      "label": "Compile final CPP library with independent catalog verifier",
      "command": "g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -fno-fast-math -ffp-contract=off -I analysis/consolidated analysis/consolidated/CS2_RECONSTRUCTION.cpp analysis/consolidated/qa/catalog_check.cpp -o analysis/consolidated/qa/catalog_check",
      "exit_code": 0,
      "stderr": ""
    },
    {
      "label": "Run compiled metadata catalog verifier",
      "command": "analysis/consolidated/qa/catalog_check",
      "exit_code": 0,
      "stderr": ""
    },
    {
      "label": "Check final standalone header including layout assertions and double inclusion",
      "command": "g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -fno-fast-math -ffp-contract=off -I analysis/consolidated -x c++ -fsyntax-only -",
      "exit_code": 0,
      "stderr": "",
      "stdin": "#include \"CS2_RECONSTRUCTION.h\"\n#include \"CS2_RECONSTRUCTION.h\"\nstatic_assert(cs2_reconstruction::model::sample_count == 64);\n"
    }
  ],
  "self_test": {
    "passed": 110,
    "failed": 0,
    "python_check_count": 23,
    "checks": [
      {
        "name": "full includes equality at health threshold",
        "passed": true
      },
      {
        "name": "fast excludes equality at health threshold",
        "passed": true
      },
      {
        "name": "uniform block permits early-fill after 8",
        "passed": true
      },
      {
        "name": "full always evaluates all supplied 64",
        "passed": true
      },
      {
        "name": "uniform means and special fractions",
        "passed": true
      },
      {
        "name": "special mode gate",
        "passed": true
      },
      {
        "name": "full measured fraction differs from extrapolated fast fraction",
        "passed": true
      },
      {
        "name": "unsatisfied A forces all blocks",
        "passed": true
      },
      {
        "name": "zero-scale shortcut",
        "passed": true
      },
      {
        "name": "two deterministic tables have 64 pairs",
        "passed": true
      },
      {
        "name": "ring endpoint radii",
        "passed": true
      },
      {
        "name": "golden endpoint radii",
        "passed": true
      },
      {
        "name": "weight cutoff is strict",
        "passed": true
      },
      {
        "name": "minimum setting clips to health",
        "passed": true
      },
      {
        "name": "over-100 mode uses health",
        "passed": true
      },
      {
        "name": "half-health mode rounds upward",
        "passed": true
      },
      {
        "name": "threshold maximum 130",
        "passed": true
      },
      {
        "name": "attenuation endpoints",
        "passed": true
      },
      {
        "name": "aging keeps newest prefix",
        "passed": true
      },
      {
        "name": "all-stale aging preserves original count in this block",
        "passed": true
      },
      {
        "name": "discontinuity boundary is strict",
        "passed": true
      },
      {
        "name": "discontinuity clamp is linear in squared-distance limit",
        "passed": true
      },
      {
        "name": "collection mismatch resets",
        "passed": true
      },
      {
        "name": "sample defaults preserve valid and category",
        "passed": true
      },
      {
        "name": "minimum equality is included by both aggregators",
        "passed": true
      },
      {
        "name": "fast special mode gate",
        "passed": true
      },
      {
        "name": "fast extrapolates high-health special block",
        "passed": true
      },
      {
        "name": "checked outcomes normalize invalid scores and preserve category",
        "passed": true
      },
      {
        "name": "normalization preserves valid scores and input",
        "passed": true
      },
      {
        "name": "invalid scores contribute neither fractions nor mean",
        "passed": true
      },
      {
        "name": "invalid positive scores are also zeroed",
        "passed": true
      },
      {
        "name": "all-invalid fast input evaluates every block",
        "passed": true
      },
      {
        "name": "fast early-fill does not extrapolate mixed special categories",
        "passed": true
      },
      {
        "name": "fast visits blocks in reverse order and fills after second block",
        "passed": true
      },
      {
        "name": "mixed health blocks prevent early-fill despite all minimum successes",
        "passed": true
      },
      {
        "name": "early-fill extrapolates the current block mean",
        "passed": true
      },
      {
        "name": "special mask accepts category 2",
        "passed": true
      },
      {
        "name": "special mask accepts category 3",
        "passed": true
      },
      {
        "name": "special mask accepts category 258",
        "passed": true
      },
      {
        "name": "special mask accepts category 259",
        "passed": true
      },
      {
        "name": "special mask accepts category -254",
        "passed": true
      },
      {
        "name": "special mask accepts category -253",
        "passed": true
      },
      {
        "name": "special mask excludes category 0",
        "passed": true
      },
      {
        "name": "special mask excludes category 1",
        "passed": true
      },
      {
        "name": "special mask excludes category 4",
        "passed": true
      },
      {
        "name": "special mask excludes category 5",
        "passed": true
      },
      {
        "name": "special mask excludes category 255",
        "passed": true
      },
      {
        "name": "zero-scale preserves central mean and special gate",
        "passed": true
      },
      {
        "name": "zero-scale does not borrow sample positivity validation",
        "passed": true
      },
      {
        "name": "zero-scale does not validate health threshold",
        "passed": true
      },
      {
        "name": "both aggregators reject sample count 0",
        "passed": true
      },
      {
        "name": "both aggregators reject sample count 63",
        "passed": true
      },
      {
        "name": "both aggregators reject sample count 65",
        "passed": true
      },
      {
        "name": "both aggregators reject zero minimum threshold",
        "passed": true
      },
      {
        "name": "both aggregators reject zero health threshold",
        "passed": true
      },
      {
        "name": "both aggregators reject zero valid score before early-fill",
        "passed": true
      },
      {
        "name": "both aggregators reject negative minimum threshold",
        "passed": true
      },
      {
        "name": "both aggregators reject negative health threshold",
        "passed": true
      },
      {
        "name": "both aggregators reject negative valid score before early-fill",
        "passed": true
      },
      {
        "name": "both aggregators reject infinity minimum threshold",
        "passed": true
      },
      {
        "name": "both aggregators reject infinity health threshold",
        "passed": true
      },
      {
        "name": "both aggregators reject infinity valid score before early-fill",
        "passed": true
      },
      {
        "name": "both aggregators reject negative infinity minimum threshold",
        "passed": true
      },
      {
        "name": "both aggregators reject negative infinity health threshold",
        "passed": true
      },
      {
        "name": "both aggregators reject negative infinity valid score before early-fill",
        "passed": true
      },
      {
        "name": "both aggregators reject NaN minimum threshold",
        "passed": true
      },
      {
        "name": "both aggregators reject NaN health threshold",
        "passed": true
      },
      {
        "name": "both aggregators reject NaN valid score before early-fill",
        "passed": true
      },
      {
        "name": "both aggregators reject infinity even when invalid",
        "passed": true
      },
      {
        "name": "both aggregators reject negative infinity even when invalid",
        "passed": true
      },
      {
        "name": "both aggregators reject NaN even when invalid",
        "passed": true
      },
      {
        "name": "validation checks sample count before thresholds",
        "passed": true
      },
      {
        "name": "validation checks thresholds before scores",
        "passed": true
      },
      {
        "name": "public validation helper rejects invalid input",
        "passed": true
      },
      {
        "name": "sampling is deterministic",
        "passed": true
      },
      {
        "name": "zero scale collapses both sampling tables",
        "passed": true
      },
      {
        "name": "ring scale multiplies every coordinate",
        "passed": true
      },
      {
        "name": "negative golden scale reflects every coordinate",
        "passed": true
      },
      {
        "name": "ring order groups eight sectors per radius",
        "passed": true
      },
      {
        "name": "golden order follows square-root radii",
        "passed": true
      },
      {
        "name": "ring angular coordinates match Python reference",
        "passed": true
      },
      {
        "name": "golden angular coordinates match Python reference",
        "passed": true
      },
      {
        "name": "empty damage sequence preserves health",
        "passed": true
      },
      {
        "name": "weights at or below cutoff are ignored",
        "passed": true
      },
      {
        "name": "health is not clamped after damage",
        "passed": true
      },
      {
        "name": "negative damage remains mathematical input",
        "passed": true
      },
      {
        "name": "exactly 100 remains ordinary minimum setting",
        "passed": true
      },
      {
        "name": "half-health flag does not affect ordinary setting",
        "passed": true
      },
      {
        "name": "half-health rounding also handles fractional health",
        "passed": true
      },
      {
        "name": "half-health result is capped at 130",
        "passed": true
      },
      {
        "name": "negative minimum setting is not clamped upward",
        "passed": true
      },
      {
        "name": "zero minimum setting is preserved",
        "passed": true
      },
      {
        "name": "zero health is rejected",
        "passed": true
      },
      {
        "name": "negative health is rejected",
        "passed": true
      },
      {
        "name": "attenuation uses distance divided by 500",
        "passed": true
      },
      {
        "name": "attenuation allows unity and growth parameters",
        "passed": true
      },
      {
        "name": "zero range is rejected",
        "passed": true
      },
      {
        "name": "negative range is rejected",
        "passed": true
      },
      {
        "name": "negative distance is rejected",
        "passed": true
      },
      {
        "name": "empty history retains zero entries",
        "passed": true
      },
      {
        "name": "aging equality is retained",
        "passed": true
      },
      {
        "name": "aging keeps all entries when oldest is fresh",
        "passed": true
      },
      {
        "name": "single stale entry is preserved",
        "passed": true
      },
      {
        "name": "aging does not sort or validate supplied order",
        "passed": true
      },
      {
        "name": "nonpositive delta clamps to one",
        "passed": true
      },
      {
        "name": "intermediate delta scales squared threshold linearly",
        "passed": true
      },
      {
        "name": "int64 delta endpoints clamp without overflow",
        "passed": true
      },
      {
        "name": "equal collection and zero distance retain history",
        "passed": true
      },
      {
        "name": "collection mismatch resets independently of distance",
        "passed": true
      },
      {
        "name": "negative squared distance is not additionally validated",
        "passed": true
      }
    ],
    "scope": "Independent mathematical reconstruction only; not equivalence testing of the binary",
    "sample_binary_executed": false
  }
}
```
