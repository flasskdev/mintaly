<!-- split-part | CS2_RESEARCH_MASTER.md lines 194-209 | body-sha256 37ec6388e848df1bf73775b5e35e7b1e30ca53852a392f34fd3e9988c87f9f6b -->
[← все части](../README.md) · [индекс обзора](00-index.md)

<!-- split-body-start -->

## 11. Как устроены CPP/H и почему есть `#if 0`

В начале CPP находится **компилируемая независимая модель**: sample generation, full/fast aggregation, zero-scale, health/minimum threshold, attenuation, tail aging и discontinuity. Она не обращается к памяти процесса и не вызывает sample code.

Затем идут все сохранённые raw C listings. Ghidra оставляет `undefined*`, `unaff_*`, SIMD intrinsics, неверные prototypes и абсолютные обращения. Они помещены в явные compiler-disabled блоки, а не выданы за готовый C++. Два варианта `0x528B90` сохранены отдельно. ASM и исходные псевдокодовые фрагменты дополнительно сохранены как неактивные raw-string blocks. Числа, предупреждения и старые имена функций не редактировались.

Для каждого блока есть source path, SHA-256, формат/этап и позиция в итоговом CPP. Каталог в `.h`/`.cpp` доступен без исполнения архивного кода. В `.h` неизвестные поля оставлены `unknown`, адреса представлены uint64, layout проверен `static_assert`. Не следует воспринимать эти наблюдаемые структуры как согласованный с движком SDK.

Пример обычной сборки математической части:

```sh
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -c CS2_RECONSTRUCTION.cpp
```

Это только компиляция нашей независимой модели и метаданных. Верификация поведения исходной DLL, инжекция, лицензирование, полный game client и работа external dependencies не входят в эту команду.
