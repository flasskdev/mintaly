<!-- split-part | CS2_RESEARCH_MASTER.md lines 967-973 | body-sha256 29ce8464dfff3a6deba7945da4f8de7e91cbe379d6685897ab627c101eba97f3 -->
[← все части](../README.md) · [индекс порта](00-index.md)

<!-- split-body-start -->

## Port limitations

- Scope remains normal finite arithmetic with representable intermediate/results, not bit-exact SIMD, float32, original CRT, or binary-runtime equivalence. C++ uses `double` and standard-library `sin`, `cos`, `sqrt`, `ceil`, and `pow`. Ordinary C++ accumulation can round differently from newer CPython compensated `sum`; tests use relative tolerance `1e-9` for approximate floating results and a small absolute tolerance for near-zero scaled coordinates.
- Python arbitrary-size integers and dynamic sequence types become explicitly typed C++ containers and signed 64-bit input integers. Values outside that integer range are outside the C++ API; immutable record assignment is intentionally unavailable.
- Nonfinite/overflow behavior beyond the source's explicit validation is not promised identical to Python. No additional global finite-input guards or invented domain restrictions were added.
- The sampling tables are computed model outputs, not recovered initialized runtime state. Fast aggregation remains arithmetic over supplied outcomes, not an implementation of a native AVX evaluator or its geometry.
