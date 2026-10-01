<!-- split-part | CS2_RESEARCH_MASTER.md lines 912-925 | body-sha256 e7b67a355ab8eb595ec0a9c8072a5bde5f4cbf3247ded2e4e549ad74fc861ed0 -->
[← все части](../README.md) · [индекс порта](00-index.md)

<!-- split-body-start -->

## Validation results

Actual compiler: `/usr/bin/g++`, `g++ (Debian 14.2.0-19) 14.2.0`.

- **110 distinct C++ checks passed; 0 failed.** The first 23 match every original Python check by name and behavior; 87 additional checks cover validation order, invalid payload normalization, threshold equality, reverse blocks, second-block early-fill, mixed categories, category masking, zero-scale behavior, sampling coordinates/scaling, minimum damage, attenuation, aging, and reset boundaries.
- The original Python runner passed all 23 checks under Python 3.14.2. Its `Path.write_text` call was intercepted in memory and bytecode writing disabled, so its existing results file and source directory remained unchanged. The exact invocation is recorded in JSON.
- Optimized `-O2` C++17 build: 110/110 passed.
- AddressSanitizer, UndefinedBehaviorSanitizer, and leak detection build: 110/110 passed, with no diagnostics.
- Standalone header syntax check passed, including double inclusion to verify `#pragma once`.
- Streamed combined translation unit (`model_impl.cpp`, followed by `model_checks.cpp` inside `#ifdef CS2_RECONSTRUCTION_SELF_TEST`) compiled both with and without the macro; its enabled runner passed 110/110 when executed from an isolated temporary directory, without runtime files.
- Builds used `-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -fno-fast-math -ffp-contract=off`; no warnings were emitted. Sanitizers additionally used `-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined`.

The repeated runs exercise the same 110 checks, not 330 distinct tests. Exact executed commands, temporary paths, stdin descriptions/hashes, return codes, and sanitizer environment settings are in `check_results.json`.
