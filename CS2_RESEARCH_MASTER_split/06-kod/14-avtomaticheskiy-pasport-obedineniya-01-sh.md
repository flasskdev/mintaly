<!-- split-kod | 01-obzor/14-avtomaticheskiy-pasport-obedineniya.md | lang=sh -->
<!-- split-body-start -->
# Code-блок из `01-obzor/14-avtomaticheskiy-pasport-obedineniya.md` (язык `sh`)

[← исходная часть](../01-obzor/14-avtomaticheskiy-pasport-obedineniya.md) · [индекс кода](00-index.md) · [все части](../README.md)

Копия fenced-блока из части `01-obzor/14-avtomaticheskiy-pasport-obedineniya.md`; в самой части блок остаётся на месте. Символов: 217.

````sh
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -c CS2_RECONSTRUCTION.cpp
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -DCS2_RECONSTRUCTION_SELF_TEST CS2_RECONSTRUCTION.cpp -o reconstruction_checks
./reconstruction_checks
````
