<!-- split-part | CS2_RESEARCH_MASTER.md lines 954-966 | body-sha256 69d903f93217b344e2b3fd083888575a1c6615e4f8dbbe382a3b1715f07174c2 -->
[← все части](../README.md) · [индекс порта](00-index.md)

<!-- split-body-start -->

## Main assembly contract

Keep the final `CS2_RECONSTRUCTION.h` beside the final `CS2_RECONSTRUCTION.cpp`. Main can append metadata/layout declarations to the header after its closed model namespace and append this test runner inside `#ifdef CS2_RECONSTRUCTION_SELF_TEST` in the final cpp. The implementation and runner require no path rewriting and no additional translation unit once concatenated. Do not also link `model_impl.cpp` separately when its definitions are embedded in the final cpp.

The requested final command is:

```sh
g++ -std=c++17 -DCS2_RECONSTRUCTION_SELF_TEST CS2_RECONSTRUCTION.cpp
./a.out
```

The equivalent single-translation-unit model/test composition was compiled and executed here. Final metadata/layout additions and the compiler-disabled archive of 63 raw C and 125 ASM listings belong to main; they were not assembled or independently verified by this math-only task. The delivered runner itself remains unguarded so it also builds as a standalone runner; main supplies the self-test guard during concatenation.
