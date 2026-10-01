<!-- split-part | CS2_RESEARCH_MASTER.md lines 76329-76347 | body-sha256 30219e2f13f5d7f03e5fd0de6d6f2ec1d2bc0c581178aab14f64cfbaa1888b42 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-142"></a>

## E142. `analysis/phase2/spread/targets.tsv`

Bytes: 761. SHA-256: `705b0f2d71e35311b2f58afa3e4f0b40725ccee1ebbf597dd5b46c47d86d39d6`.

```tsv
rva	va	end	unwind	original_decoded_end	role
0x525830	0x212c3825830	0x526286	0x1073570	0x526286	64-sample scoring aggregate; primary
0x51cf40	0x212c381cf40	0x51d317	0x1072a50	0x51d317	sample frame / scaled basis setup
0x51d320	0x212c381d320	0x51d735	0x1072a70	0x51d735	job callback; shared with point optimizer
0x512710	0x212c3812710	0x512e03	0x1071ebc	0x512e03	two deterministic disk tables
0x5239d0	0x212c38239d0	0x52473e	0x1073258	0x523ae0	ring-scale cache producer
0x524a30	0x212c3824a30	0x52566c	0x10733ec	0x52566c	candidate aggregates + point optimization
0x51c9c0	0x212c381c9c0	0x51cf34	0x1072a14	0x51cf34	golden-angle coverage/point optimization
0x526290	0x212c3826290	0x528839	0x10735b0	0x528839	fast 8x8 sampling with early-fill extrapolation
```
