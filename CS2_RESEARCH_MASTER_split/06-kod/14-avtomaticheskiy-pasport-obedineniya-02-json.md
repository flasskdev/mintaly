<!-- split-kod | 01-obzor/14-avtomaticheskiy-pasport-obedineniya.md | lang=json -->
<!-- split-body-start -->
# Code-блок из `01-obzor/14-avtomaticheskiy-pasport-obedineniya.md` (язык `json`)

[← исходная часть](../01-obzor/14-avtomaticheskiy-pasport-obedineniya.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `01-obzor/14-avtomaticheskiy-pasport-obedineniya.md`; в самой части блок остаётся на месте. Символов: 21163.

````json
{
  "status": "passed",
  "scope": "Faithful C++17 port of the independent mathematical reconstruction; not original-binary equivalence testing",
  "sample_binary_executed": false,
  "working_directory": "/home/daytona/albigg",
  "compiler": {
    "path": "/usr/bin/g++",
    "version": "g++ (Debian 14.2.0-19) 14.2.0",
    "standard": "C++17"
  },
  "python": {
    "path": "/usr/local/bin/python3",
    "version": "3.14.2 (main, Jan 13 2026, 05:57:24) [GCC 14.2.0]"
  },
  "source_sha256": {
    "analysis/phase2/reconstructed/recovered_math.py": "75e4f25e551afbd9a7eb5dc0fc9e821eee02e605493374eddc6e81ab8ba83fe6",
    "analysis/phase2/reconstructed/README.md": "fc791e56c6f00620c0c18436b52d9e98c025c07568aadffa5bebc8be288b3a9e",
    "analysis/phase2/reconstructed/check_math.py": "415087abfb3099c60fd35c84d5c96c64cc9a8a10178d9d2d4e8e74b1bf7abfc6",
    "analysis/phase2/reconstructed/math_check_results.json": "fa8516a408566ca9919ba678f07d8caf90981a35f36b047f3eb78b9f0271c79d"
  },
  "commands": [
    {
      "label": "Python original checks, report write intercepted in memory",
      "command": "/usr/local/bin/python3 -B -c 'import runpy\nimport sys\nfrom pathlib import Path\nfrom unittest.mock import patch\nsys.path.insert(0, '\"'\"'analysis/phase2/reconstructed'\"'\"')\nexpected = Path('\"'\"'analysis/phase2/reconstructed/math_check_results.json'\"'\"').resolve()\ndef capture_write(path, data, *args, **kwargs):\n    if path.resolve() != expected:\n        raise AssertionError('\"'\"'Unexpected attempted write: '\"'\"' + str(path))\n    return len(data)\nwith patch.object(Path, '\"'\"'write_text'\"'\"', capture_write):\n    runpy.run_path('\"'\"'analysis/phase2/reconstructed/check_math.py'\"'\"', run_name='\"'\"'__main__'\"'\"')\n'",
      "exit_code": 0,
      "stderr": ""
    },
    {
      "label": "Standalone header, double inclusion",
      "command": "/usr/bin/g++ -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -fno-fast-math -ffp-contract=off -I /home/daytona/albigg/analysis/consolidated/staging/math -x c++ -fsyntax-only -",
      "exit_code": 0,
      "stderr": "",
      "stdin": "#include \"CS2_RECONSTRUCTION.h\"\n#include \"CS2_RECONSTRUCTION.h\"\nstatic_assert(cs2_reconstruction::model::sample_count == 64);\n"
    },
    {
      "label": "Compile optimized standalone C++17 runner",
      "command": "/usr/bin/g++ -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -fno-fast-math -ffp-contract=off -O2 /home/daytona/albigg/analysis/consolidated/staging/math/model_impl.cpp /home/daytona/albigg/analysis/consolidated/staging/math/model_checks.cpp -o /home/daytona/albigg/analysis/consolidated/staging/math/.validation-risodd0v/model_checks",
      "exit_code": 0,
      "stderr": ""
    },
    {
      "label": "Run optimized standalone C++17 checks",
      "command": "/home/daytona/albigg/analysis/consolidated/staging/math/.validation-risodd0v/model_checks",
      "exit_code": 0,
      "stderr": ""
    },
    {
      "label": "Compile ASan and UBSan C++17 runner",
      "command": "/usr/bin/g++ -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -fno-fast-math -ffp-contract=off -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined /home/daytona/albigg/analysis/consolidated/staging/math/model_impl.cpp /home/daytona/albigg/analysis/consolidated/staging/math/model_checks.cpp -o /home/daytona/albigg/analysis/consolidated/staging/math/.validation-risodd0v/model_checks_sanitized",
      "exit_code": 0,
      "stderr": ""
    },
    {
      "label": "Run ASan and UBSan checks",
      "command": "/home/daytona/albigg/analysis/consolidated/staging/math/.validation-risodd0v/model_checks_sanitized",
      "exit_code": 0,
      "stderr": "",
      "environment": {
        "ASAN_OPTIONS": "detect_leaks=1:halt_on_error=1",
        "UBSAN_OPTIONS": "halt_on_error=1:print_stacktrace=1"
      }
    },
    {
      "label": "Combined translation unit without self-test enabled",
      "command": "/usr/bin/g++ -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -fno-fast-math -ffp-contract=off -I /home/daytona/albigg/analysis/consolidated/staging/math -x c++ -fsyntax-only -",
      "working_directory": "/home/daytona/albigg/analysis/consolidated/staging/math/.assembly-check-hs28725g",
      "environment": {
        "TMPDIR": "/home/daytona/albigg/analysis/consolidated/staging/math/.assembly-check-hs28725g",
        "LC_ALL": "C"
      },
      "exit_code": 0,
      "stderr": "",
      "stdin_description": "model_impl.cpp + newline + #ifdef CS2_RECONSTRUCTION_SELF_TEST + newline + model_checks.cpp + newline + #endif + newline",
      "stdin_sha256": "8ec5ab57fa6169c5d1bd2faf20ebcbf031716a4130857f5b6303bd58af79f590"
    },
    {
      "label": "Combined translation unit with CS2_RECONSTRUCTION_SELF_TEST",
      "command": "/usr/bin/g++ -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -fno-fast-math -ffp-contract=off -DCS2_RECONSTRUCTION_SELF_TEST -O2 -I /home/daytona/albigg/analysis/consolidated/staging/math -x c++ - -o /home/daytona/albigg/analysis/consolidated/staging/math/.assembly-check-hs28725g/combined_checks",
      "working_directory": "/home/daytona/albigg/analysis/consolidated/staging/math/.assembly-check-hs28725g",
      "environment": {
        "TMPDIR": "/home/daytona/albigg/analysis/consolidated/staging/math/.assembly-check-hs28725g",
        "LC_ALL": "C"
      },
      "exit_code": 0,
      "stderr": "",
      "stdin_description": "model_impl.cpp + newline + #ifdef CS2_RECONSTRUCTION_SELF_TEST + newline + model_checks.cpp + newline + #endif + newline",
      "stdin_sha256": "8ec5ab57fa6169c5d1bd2faf20ebcbf031716a4130857f5b6303bd58af79f590"
    },
    {
      "label": "Run combined self-test from isolated directory",
      "command": "/home/daytona/albigg/analysis/consolidated/staging/math/.assembly-check-hs28725g/combined_checks",
      "working_directory": "/home/daytona/albigg/analysis/consolidated/staging/math/.assembly-check-hs28725g",
      "environment": {
        "TMPDIR": "/home/daytona/albigg/analysis/consolidated/staging/math/.assembly-check-hs28725g",
        "LC_ALL": "C"
      },
      "exit_code": 0,
      "stderr": ""
    }
  ],
  "command_environment": {
    "TMPDIR": "/home/daytona/albigg/analysis/consolidated/staging/math/.validation-risodd0v",
    "LC_ALL": "C",
    "PYTHONDONTWRITEBYTECODE": "1"
  },
  "python_reference": {
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
    "report_write_intercepted": true,
    "source_files_modified": false
  },
  "standalone_header": {
    "passed": true,
    "double_inclusion": true
  },
  "tests": {
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
  },
  "baseline_checks_matched": 23,
  "sanitizers": {
    "passed": 110,
    "failed": 0,
    "address_sanitizer": true,
    "undefined_behavior_sanitizer": true,
    "leak_detection": true
  },
  "protected_source_files_unchanged": true,
  "temporary_build_files_removed": true,
  "port_source_sha256": {
    "CS2_RECONSTRUCTION.h": "5ffef91ad7cd92af57663e9f2d4cbd784a71186ace31950ce264831fdb0b2fe8",
    "model_impl.cpp": "6a3e13925131fb381c72543af15e73724d42f74e68fc598f6fa380b0464774fb",
    "model_checks.cpp": "62a3798688faefeab168447cfb058de720a856c7b2334c662b379d470f9b7f64"
  },
  "combined_translation_unit": {
    "without_self_test_syntax_passed": true,
    "with_self_test_build_passed": true,
    "passed": 110,
    "failed": 0,
    "runtime_input_files_required": false,
    "test_execution_directory": "/home/daytona/albigg/analysis/consolidated/staging/math/.assembly-check-hs28725g",
    "final_assembly_command": "g++ -std=c++17 -DCS2_RECONSTRUCTION_SELF_TEST CS2_RECONSTRUCTION.cpp",
    "final_assembly_command_note": "Expected main-assembly command; the equivalent streamed single translation unit was tested here. Final mega source, metadata and raw archives are outside this task."
  },
  "function_mapping": {
    "ring_samples": "cs2_reconstruction::model::ring_samples",
    "golden_samples": "cs2_reconstruction::model::golden_samples",
    "_checked_outcomes": "cs2_reconstruction::model::checked_outcomes",
    "aggregate_full": "cs2_reconstruction::model::aggregate_full",
    "aggregate_fast": "cs2_reconstruction::model::aggregate_fast",
    "zero_scale_aggregate": "cs2_reconstruction::model::zero_scale_aggregate",
    "estimated_health": "cs2_reconstruction::model::estimated_health",
    "minimum_damage_threshold": "cs2_reconstruction::model::minimum_damage_threshold",
    "distance_attenuation": "cs2_reconstruction::model::distance_attenuation",
    "retained_count_after_tail_aging": "cs2_reconstruction::model::retained_count_after_tail_aging",
    "resets_history": "cs2_reconstruction::model::resets_history"
  },
  "passed": 110,
  "failed": 0,
  "additional_cpp_checks": 87,
  "code_comments_present": false
}
````
