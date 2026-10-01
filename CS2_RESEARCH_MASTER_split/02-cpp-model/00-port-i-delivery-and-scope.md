<!-- split-part | CS2_RESEARCH_MASTER.md lines 879-894 | body-sha256 f48b5ddcb047b5f6c81e58a3799ef6edb0044469a4bc609f0a8645b48b8b1921 -->
[← все части](../README.md) · [индекс порта](00-index.md)

<!-- split-body-start -->

# C++17 mathematical reconstruction port

## Delivery and scope

All deliverables are under `analysis/consolidated/staging/math/`:

- `CS2_RECONSTRUCTION.h`: standalone, `#pragma once`, standard-library-only declarations in `cs2_reconstruction::model`.
- `model_impl.cpp`: implementation of all 11 functions, including the validation helper, beginning with `#include "CS2_RECONSTRUCTION.h"`.
- `model_checks.cpp`: self-contained executable test runner; includes the header, defines `main`, prints JSON status to stdout, and returns 0 on success, 1 on failed checks, or 2 on an unexpected exception. It requires no runtime input files or staging paths.
- `check_results.json`: actual compiler/version, executed commands, input/output hashes, individual test names/results, Python reference verification, sanitizer results, and single-translation-unit assembly checks.
- `notes.md`: this scope, integration, validation, and limitations record.

Read-only inputs were `analysis/phase2/reconstructed/recovered_math.py`, `README.md`, and `check_math.py`. Their hashes, plus the existing `math_check_results.json` hash, were verified unchanged after validation. No existing source report, original artifact, raw C/ASM listing, or final main-assembly file was edited. No binaries from the input dump were executed. Only freshly compiled mathematical test executables were run; temporary build files stayed inside this staging directory and were removed.

There is no engine trace, injection, resolver, runtime offset lookup, penetration implementation, target-ID matching, or unknown-function success stub. Scores and ID-filtered weight/damage pairs are already supplied by the caller. This is a port of the written mathematical model, not proof of equivalence to the original binary.
