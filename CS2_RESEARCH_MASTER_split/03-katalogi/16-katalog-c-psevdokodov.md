<!-- split-part | CS2_RESEARCH_MASTER.md lines 1501-1570 | body-sha256 a447b53fdeee59b6736a822b16b5f3e01a275641e505e7f615e3b0703e7cd4eb -->
[← все части](../README.md) · [карточки C-псевдокода](pseudocode-c/00-index.md)

<!-- split-body-start -->

## 16. Каталог всех C-псевдокодов

Указанный номер строки относится к окончательному `CS2_RECONSTRUCTION.cpp`; все адреса VA/RVA сохранены. Листинг conditional не подменяет исходный original.

| № | RVA | VA | Роль | Источник | Строка CPP | Bytes |
|---:|---|---|---|---|---:|---:|
| 1 | `0x309500` | `0x212c3609500` | Tick/fraction selection from separate ring32 | `analysis/decompiled_core/212c3609500.c` | 624 | 3565 |
| 2 | `0x50dd90` | `0x212c380dd90` | Not separately attributed; preserve source evidence | `analysis/decompiled_core/212c380dd90.c` | 770 | 34272 |
| 3 | `0x528b90` | `0x212c3828b90` | Melee/knifebot-like damage and mode evaluator | `analysis/decompiled_core/212c3828b90.c` | 1806 | 47092 |
| 4 | `0x52b250` | `0x212c382b250` | Select up to two records from ring16 | `analysis/decompiled_core/212c382b250.c` | 2927 | 8276 |
| 5 | `0x1bec10` | `0x212c34bec10` | Not separately attributed; preserve source evidence | `analysis/decompiled_features/212c34bec10.c` | 3188 | 2241 |
| 6 | `0x1bee90` | `0x212c34bee90` | Not separately attributed; preserve source evidence | `analysis/decompiled_features/212c34bee90.c` | 3268 | 2417 |
| 7 | `0x1bf110` | `0x212c34bf110` | Not separately attributed; preserve source evidence | `analysis/decompiled_features/212c34bf110.c` | 3346 | 2417 |
| 8 | `0x3375a0` | `0x212c36375a0` | Cached temporal angular correction predictor | `analysis/decompiled_features/212c36375a0.c` | 3424 | 27862 |
| 9 | `0x505f50` | `0x212c3805f50` | Caller with 0x30 candidate selection before finalization | `analysis/decompiled_features/212c3805f50.c` | 4121 | 193260 |
| 10 | `0x511d50` | `0x212c3811d50` | Snapshot deep-copy helper; preserve B6/B7/B8 flags | `analysis/decompiled_features/212c3811d50.c` | 8064 | 4003 |
| 11 | `0x7447e0` | `0x212c3a447e0` | Not separately attributed; preserve source evidence | `analysis/decompiled_features/212c3a447e0.c` | 8195 | 64601 |
| 12 | `0xc421c0` | `0x212c3f421c0` | FreeType SDF property setter, not weapon spread | `analysis/decompiled_features/212c3f421c0.c` | 9602 | 1016 |
| 13 | `0xc422b0` | `0x212c3f422b0` | FreeType SDF property getter, not weapon spread | `analysis/decompiled_features/212c3f422b0.c` | 9650 | 970 |
| 14 | `0x2e9b50` | `0x212c35e9b50` | Spatial visual effect endpoints/color/lifetime | `analysis/decompiled_rng/212c35e9b50.c` | 9695 | 24917 |
| 15 | `0x406c10` | `0x212c3706c10` | UI value randomization of decimal suffix | `analysis/decompiled_rng/212c3706c10.c` | 10365 | 1051 |
| 16 | `0x422970` | `0x212c3722970` | UI interval selection and random value | `analysis/decompiled_rng/212c3722970.c` | 10410 | 9373 |
| 17 | `0x4fd710` | `0x212c37fd710` | String selector; significant known decompiler inaccuracies | `analysis/decompiled_rng/212c37fd710.c` | 10689 | 30533 |
| 18 | `0x50fb10` | `0x212c380fb10` | Apply/finalize selected aim/fire command | `analysis/decompiled_rng/212c380fb10.c` | 11518 | 31119 |
| 19 | `0x54bc60` | `0x212c384bc60` | Shot-result diagnostics and additional state writes | `analysis/decompiled_rng/212c384bc60.c` | 12322 | 55888 |
| 20 | `0x71ebe0` | `0x212c3a1ebe0` | UI transition/reset with small random offsets | `analysis/decompiled_rng/212c3a1ebe0.c` | 13723 | 2501 |
| 21 | `0x4ff1690` | `0x212c82f1690` | Init/DllMain-like wrapper; reason==1 calls two helpers | `analysis/decompiled_rng/212c82f1690.c` | 13813 | 165 |
| 22 | `0x512710` | `0x212c3812710` | Initialize two deterministic Vec2[64] tables | `analysis/phase2/conditional_bullet/212c3812710.c` | 13830 | 9354 |
| 23 | `0x515ca0` | `0x212c3815ca0` | Angular processing including random correction; broad role unresolved | `analysis/phase2/conditional_bullet/212c3815ca0.c` | 14098 | 81885 |
| 24 | `0x51c5f0` | `0x212c381c5f0` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_bullet/212c381c5f0.c` | 16108 | 3628 |
| 25 | `0x51d910` | `0x212c381d910` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_bullet/212c381d910.c` | 16213 | 2041 |
| 26 | `0x5248e0` | `0x212c38248e0` | Candidate score and fast-aggregate preparation | `analysis/phase2/conditional_bullet/212c38248e0.c` | 16275 | 2172 |
| 27 | `0x525830` | `0x212c3825830` | Full 64-direction four-component aggregation | `analysis/phase2/conditional_bullet/212c3825830.c` | 16353 | 18943 |
| 28 | `0xf57b0` | `0x212c33f57b0` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_core/212c33f57b0.c` | 16844 | 1976 |
| 29 | `0x1463a0` | `0x212c34463a0` | External ray/hull trace adapter | `analysis/phase2/conditional_core/212c34463a0.c` | 16905 | 763 |
| 30 | `0x30d580` | `0x212c360d580` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_core/212c360d580.c` | 16939 | 129324 |
| 31 | `0x50ce90` | `0x212c380ce90` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_core/212c380ce90.c` | 19606 | 4278 |
| 32 | `0x511160` | `0x212c3811160` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_core/212c3811160.c` | 19733 | 5826 |
| 33 | `0x5114d0` | `0x212c38114d0` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_core/212c38114d0.c` | 19934 | 20964 |
| 34 | `0x51bea0` | `0x212c381bea0` | Candidate comparator | `analysis/phase2/conditional_core/212c381bea0.c` | 20590 | 16047 |
| 35 | `0x51df10` | `0x212c381df10` | Direction/hit-score gate; incomplete semantic typing | `analysis/phase2/conditional_core/212c381df10.c` | 20977 | 9728 |
| 36 | `0x5239d0` | `0x212c38239d0` | Prepare bullet evaluation and scaled sample caches | `analysis/phase2/conditional_core/212c38239d0.c` | 21191 | 26608 |
| 37 | `0x524a30` | `0x212c3824a30` | Candidate selection and point optimization | `analysis/phase2/conditional_core/212c3824a30.c` | 21873 | 31980 |
| 38 | `0x528b90` | `0x212c3828b90` | Melee/knifebot-like damage and mode evaluator | `analysis/phase2/conditional_core/212c3828b90.c` | 22699 | 24986 |
| 39 | `0x52ad40` | `0x212c382ad40` | Append melee candidate, stride 0x30 | `analysis/phase2/conditional_core/212c382ad40.c` | 23387 | 2690 |
| 40 | `0x52bb10` | `0x212c382bb10` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_core/212c382bb10.c` | 23477 | 34728 |
| 41 | `0x52dd20` | `0x212c382dd20` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_core/212c382dd20.c` | 24309 | 14626 |
| 42 | `0x52e720` | `0x212c382e720` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_core/212c382e720.c` | 24695 | 17780 |
| 43 | `0x52f030` | `0x212c382f030` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_core/212c382f030.c` | 25191 | 5296 |
| 44 | `0x5302c0` | `0x212c38302c0` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_core/212c38302c0.c` | 25346 | 4511 |
| 45 | `0x530cd0` | `0x212c3830cd0` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_core/212c3830cd0.c` | 25512 | 30751 |
| 46 | `0x7bdb90` | `0x212c3abdb90` | Category multiplier with external scalar settings | `analysis/phase2/conditional_damage/212c3abdb90.c` | 26224 | 12690 |
| 47 | `0x4721d0` | `0x212c37721d0` | Ring16 capture producer, aging and continuity checks | `analysis/phase2/conditional_lag/212c37721d0.c` | 26559 | 13018 |
| 48 | `0x473490` | `0x212c3773490` | Runtime history-window scalar source | `analysis/phase2/conditional_lag/212c3773490.c` | 26924 | 3082 |
| 49 | `0x473f30` | `0x212c3773f30` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_lag/212c3773f30.c` | 27005 | 24895 |
| 50 | `0x475830` | `0x212c3775830` | Record-prefix and owned-buffer copy | `analysis/phase2/conditional_lag/212c3775830.c` | 27608 | 7265 |
| 51 | `0x661010` | `0x212c3961010` | Additional history producer caller | `analysis/phase2/conditional_lag/212c3961010.c` | 27861 | 5275 |
| 52 | `0x6ac690` | `0x212c39ac690` | History orchestration, move and worker dispatch | `analysis/phase2/conditional_lag/212c39ac690.c` | 28008 | 144729 |
| 53 | `0x51b480` | `0x212c381b480` | History-based angular correction; not proven resolver | `analysis/phase2/conditional_targets/212c381b480.c` | 31027 | 3481 |
| 54 | `0x51e4f0` | `0x212c381e4f0` | Health/minimum-score context and candidate generation | `analysis/phase2/conditional_targets/212c381e4f0.c` | 31141 | 137784 |
| 55 | `0x2f0dd0` | `0x212c35f0dd0` | Penetration segment traversal with missing external core | `analysis/phase2/conditional_trace/212c35f0dd0.c` | 33926 | 3274 |
| 56 | `0x2f19c0` | `0x212c35f19c0` | Local prepared hit-volume geometric test | `analysis/phase2/conditional_trace/212c35f19c0.c` | 34037 | 11499 |
| 57 | `0x2f2110` | `0x212c35f2110` | External trace workspace orchestration | `analysis/phase2/conditional_trace/212c35f2110.c` | 34354 | 5671 |
| 58 | `0x475e90` | `0x212c3775e90` | Not separately attributed; preserve source evidence | `analysis/phase2/conditional_trace/212c3775e90.c` | 34534 | 381 |
| 59 | `0x51c9c0` | `0x212c381c9c0` | Sampling-based point coverage/optimization | `analysis/phase2/conditional_trace/212c381c9c0.c` | 34553 | 11195 |
| 60 | `0x51cf40` | `0x212c381cf40` | Prepare candidate direction/basis/sample frame | `analysis/phase2/conditional_trace/212c381cf40.c` | 34846 | 8205 |
| 61 | `0x51d320` | `0x212c381d320` | Sample worker and scalar-score dispatch | `analysis/phase2/conditional_trace/212c381d320.c` | 35071 | 8597 |
| 62 | `0x51dac0` | `0x212c381dac0` | Scalar bullet score evaluation and trace orchestration | `analysis/phase2/conditional_trace/212c381dac0.c` | 35292 | 6651 |
| 63 | `0x526290` | `0x212c3826290` | Fast 8x8 aggregate with extrapolating early-fill | `analysis/phase2/conditional_trace/212c3826290.c` | 35478 | 93570 |
