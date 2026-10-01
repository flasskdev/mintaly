<!-- split-kod | 02-cpp-model/03-reproduction-commands.md | lang=sh -->
<!-- split-body-start -->
# Code-блок из `02-cpp-model/03-reproduction-commands.md` (язык `sh`)

[← исходная часть](../02-cpp-model/03-reproduction-commands.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `02-cpp-model/03-reproduction-commands.md`; в самой части блок остаётся на месте. Символов: 1060.

````sh
math_dir=analysis/consolidated/staging/math
build_dir="$(mktemp -d "$PWD/$math_dir/.checks.XXXXXX")"
trap 'rm -rf -- "$build_dir"' EXIT
TMPDIR="$build_dir" /usr/bin/g++ -std=c++17 -O2 \
  -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror \
  -fno-fast-math -ffp-contract=off \
  "$math_dir/model_impl.cpp" "$math_dir/model_checks.cpp" \
  -o "$build_dir/model_checks"
"$build_dir/model_checks"
TMPDIR="$build_dir" /usr/bin/g++ -std=c++17 -O1 -g \
  -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror \
  -fno-fast-math -ffp-contract=off -fno-omit-frame-pointer \
  -fsanitize=address,undefined \
  "$math_dir/model_impl.cpp" "$math_dir/model_checks.cpp" \
  -o "$build_dir/model_checks_sanitized"
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
  "$build_dir/model_checks_sanitized"
printf '#include "CS2_RECONSTRUCTION.h"\n#include "CS2_RECONSTRUCTION.h"\n' | \
  TMPDIR="$build_dir" /usr/bin/g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror \
  -I "$math_dir" -x c++ -fsyntax-only -
````
