<!-- split-part | CS2_RESEARCH_MASTER.md lines 60819-60858 | body-sha256 0c65973565da166c3acad20e4098f290a50272bfdae7b8116f2b6b762f3856aa -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-053"></a>

## E053. `analysis/phase2/reconstructed/math_check_results.json`

Bytes: 1100. SHA-256: `fa8516a408566ca9919ba678f07d8caf90981a35f36b047f3eb78b9f0271c79d`.

```json
{
  "passed": 23,
  "checks": [
    "full includes equality at health threshold",
    "fast excludes equality at health threshold",
    "uniform block permits early-fill after 8",
    "full always evaluates all supplied 64",
    "uniform means and special fractions",
    "special mode gate",
    "full measured fraction differs from extrapolated fast fraction",
    "unsatisfied A forces all blocks",
    "zero-scale shortcut",
    "two deterministic tables have 64 pairs",
    "ring endpoint radii",
    "golden endpoint radii",
    "weight cutoff is strict",
    "minimum setting clips to health",
    "over-100 mode uses health",
    "half-health mode rounds upward",
    "threshold maximum 130",
    "attenuation endpoints",
    "aging keeps newest prefix",
    "all-stale aging preserves original count in this block",
    "discontinuity boundary is strict",
    "discontinuity clamp is linear in squared-distance limit",
    "collection mismatch resets"
  ],
  "scope": "Independent mathematical reconstruction only; not equivalence testing of the binary",
  "sample_binary_executed": false
}
```
