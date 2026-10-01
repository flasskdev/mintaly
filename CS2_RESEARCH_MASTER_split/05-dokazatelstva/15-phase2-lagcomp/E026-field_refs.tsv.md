<!-- split-part | CS2_RESEARCH_MASTER.md lines 10838-10918 | body-sha256 0f1c097bb7562aa6ca86ab990e8c12dd1720afebd3d11aaff05a35cef9d09e31 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-026"></a>

## E026. `analysis/phase2/lagcomp/field_refs.tsv`

Bytes: 8287. SHA-256: `73d466188aaedad7849fbe32ba86141a0b9c42b8dc07a5f261834b1d5a908750`.

```tsv
function_rva	rva	va	bytes	mnemonic	operands	provenance	displacement	access
0x37a020	0x37a61a	0x212c367a61a	83b9f050000000	cmp	dword ptr [rcx + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	1
0x37a020	0x37a4ea	0x212c367a4ea	448b89f4500000	mov	r9d, dword ptr [rcx + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x37a020	0x37a4dd	0x212c367a4dd	83b9f050000000	cmp	dword ptr [rcx + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	1
0x4707f0	0x471003	0x212c3771003	48c786f050000000000000	mov	qword ptr [rsi + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	2
0x471d70	0x471d7d	0x212c3771d7d	48c786f050000000000000	mov	qword ptr [rsi + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	2
0x4721d0	0x472240	0x212c3772240	8b86f0500000	mov	eax, dword ptr [rsi + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x4721d0	0x472258	0x212c3772258	448b86f4500000	mov	r8d, dword ptr [rsi + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x4721d0	0x472297	0x212c3772297	898ef0500000	mov	dword ptr [rsi + 0x50f0], ecx	entry_recursive_direct_cfg	0x50f0	2
0x4721d0	0x472834	0x212c3772834	8b8ef4500000	mov	ecx, dword ptr [rsi + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x4721d0	0x4728a8	0x212c37728a8	898ef4500000	mov	dword ptr [rsi + 0x50f4], ecx	entry_recursive_direct_cfg	0x50f4	2
0x4721d0	0x4728b5	0x212c37728b5	8b8ef4500000	mov	ecx, dword ptr [rsi + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x4721d0	0x4728d0	0x212c37728d0	898ef4500000	mov	dword ptr [rsi + 0x50f4], ecx	entry_recursive_direct_cfg	0x50f4	2
0x4721d0	0x4728e8	0x212c37728e8	8986f0500000	mov	dword ptr [rsi + 0x50f0], eax	entry_recursive_direct_cfg	0x50f0	2
0x4721d0	0x4727a9	0x212c37727a9	8b86f0500000	mov	eax, dword ptr [rsi + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x4721d0	0x4727af	0x212c37727af	8b8ef4500000	mov	ecx, dword ptr [rsi + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x4721d0	0x47280b	0x212c377280b	8986f0500000	mov	dword ptr [rsi + 0x50f0], eax	entry_recursive_direct_cfg	0x50f0	2
0x4721d0	0x47282c	0x212c377282c	898ef4500000	mov	dword ptr [rsi + 0x50f4], ecx	entry_recursive_direct_cfg	0x50f4	2
0x4721d0	0x4732a1	0x212c37732a1	83bef050000002	cmp	dword ptr [rsi + 0x50f0], 2	entry_recursive_direct_cfg	0x50f0	1
0x4721d0	0x4732ae	0x212c37732ae	8b8ef4500000	mov	ecx, dword ptr [rsi + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x4721d0	0x473375	0x212c3773375	c786f050000001000000	mov	dword ptr [rsi + 0x50f0], 1	entry_recursive_direct_cfg	0x50f0	2
0x474d20	0x474d32	0x212c3774d32	83b9f050000000	cmp	dword ptr [rcx + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	1
0x474d20	0x474d45	0x212c3774d45	8b81f4500000	mov	eax, dword ptr [rcx + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x48eab0	0x48ed84	0x212c378ed84	8b80f0500000	mov	eax, dword ptr [rax + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x48eab0	0x48ee54	0x212c378ee54	8b80f4500000	mov	eax, dword ptr [rax + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x4aad60	0x4aaed8	0x212c37aaed8	8b87f4500000	mov	eax, dword ptr [rdi + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x4b9600	0x4b98b3	0x212c37b98b3	83b8f050000000	cmp	dword ptr [rax + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	1
0x4b9600	0x4b98c0	0x212c37b98c0	8b88f4500000	mov	ecx, dword ptr [rax + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x4feba0	0x4ffd3a	0x212c37ffd3a	418b8ef0500000	mov	ecx, dword ptr [r14 + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x4feba0	0x4ffd49	0x212c37ffd49	418b86f4500000	mov	eax, dword ptr [r14 + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x512280	0x51265f	0x212c381265f	8b81f0500000	mov	eax, dword ptr [rcx + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x512280	0x512669	0x212c3812669	8b91f4500000	mov	edx, dword ptr [rcx + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x51b480	0x51b567	0x212c381b567	83bbf050000000	cmp	dword ptr [rbx + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	1
0x51b480	0x51b57b	0x212c381b57b	8b83f4500000	mov	eax, dword ptr [rbx + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x51b480	0x51b611	0x212c381b611	83bbf050000002	cmp	dword ptr [rbx + 0x50f0], 2	entry_recursive_direct_cfg	0x50f0	1
0x51b480	0x51b61a	0x212c381b61a	8b83f4500000	mov	eax, dword ptr [rbx + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x51e4f0	0x51ef14	0x212c381ef14	448bb6f0500000	mov	r14d, dword ptr [rsi + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x51e4f0	0x51ef96	0x212c381ef96	8b86f4500000	mov	eax, dword ptr [rsi + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x52b250	0x52b31c	0x212c382b31c	8baff0500000	mov	ebp, dword ptr [rdi + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x52b250	0x52b38c	0x212c382b38c	8b87f4500000	mov	eax, dword ptr [rdi + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x52cd20	0x52cda5	0x212c382cda5	458b81f0500000	mov	r8d, dword ptr [r9 + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x52cd20	0x52cdc2	0x212c382cdc2	458b91f4500000	mov	r10d, dword ptr [r9 + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x52cd20	0x52cf7c	0x212c382cf7c	486381f0500000	movsxd	rax, dword ptr [rcx + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x52cd20	0x52cfa4	0x212c382cfa4	8b89f4500000	mov	ecx, dword ptr [rcx + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x52cd20	0x52d6dd	0x212c382d6dd	418b9424f0500000	mov	edx, dword ptr [r12 + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x52cd20	0x52d773	0x212c382d773	458b8c24f4500000	mov	r9d, dword ptr [r12 + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x52cd20	0x52d8dc	0x212c382d8dc	458b9424f4500000	mov	r10d, dword ptr [r12 + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x52dd20	0x52df01	0x212c382df01	4183bef050000000	cmp	dword ptr [r14 + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	1
0x52dd20	0x52df0b	0x212c382df0b	418b86f4500000	mov	eax, dword ptr [r14 + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x52f030	0x52f0b4	0x212c382f0b4	83bbf050000000	cmp	dword ptr [rbx + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	1
0x665540	0x66c60b	0x212c396c60b	83bef050000000	cmp	dword ptr [rsi + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	1
0x665540	0x66c9df	0x212c396c9df	83bef050000000	cmp	dword ptr [rsi + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	1
0x665540	0x66c836	0x212c396c836	8b86f4500000	mov	eax, dword ptr [rsi + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x665540	0x66c8a7	0x212c396c8a7	8b8ef0500000	mov	ecx, dword ptr [rsi + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x665540	0x66c912	0x212c396c912	448b86f4500000	mov	r8d, dword ptr [rsi + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x6788a0	0x678b9d	0x212c3978b9d	418b9c24f0500000	mov	ebx, dword ptr [r12 + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x6788a0	0x678bfa	0x212c3978bfa	418b8424f4500000	mov	eax, dword ptr [r12 + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x6ac690	0x6ae648	0x212c39ae648	488b87f0500000	mov	rax, qword ptr [rdi + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x6ac690	0x6ae64f	0x212c39ae64f	49898424f0500000	mov	qword ptr [r12 + 0x50f0], rax	entry_recursive_direct_cfg	0x50f0	2
0x6ac690	0x6af3d6	0x212c39af3d6	83bef050000000	cmp	dword ptr [rsi + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	1
0x6ac690	0x6af3e3	0x212c39af3e3	8b86f4500000	mov	eax, dword ptr [rsi + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x6ac690	0x6b2d8e	0x212c39b2d8e	418b8df0500000	mov	ecx, dword ptr [r13 + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x6ac690	0x6b2d99	0x212c39b2d99	418b95f4500000	mov	edx, dword ptr [r13 + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x6ac690	0x6b2aa0	0x212c39b2aa0	8b89f0500000	mov	ecx, dword ptr [rcx + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0x6ac690	0x6b2abe	0x212c39b2abe	8b92f4500000	mov	edx, dword ptr [rdx + 0x50f4]	entry_recursive_direct_cfg	0x50f4	1
0x6ce200	0x6ce2f9	0x212c39ce2f9	48c787f050000000000000	mov	qword ptr [rdi + 0x50f0], 0	entry_recursive_direct_cfg	0x50f0	2
0xa3d3a0	0xa3dcc4	0x212c3d3dcc4	c4c1781181f0500000	vmovups	xmmword ptr [r9 + 0x50f0], xmm0	entry_recursive_direct_cfg	0x50f0	1
0xa3d3a0	0xa3dd5b	0x212c3d3dd5b	4c8d87f0500000	lea	r8, [rdi + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0xa42e40	0xa42f64	0x212c3d42f64	488bbef0500000	mov	rdi, qword ptr [rsi + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0xa42e40	0xa42f74	0x212c3d42f74	488d9ef0500000	lea	rbx, [rsi + 0x50f0]	entry_recursive_direct_cfg	0x50f0	1
0xa58da0	0xa61a9a	0x212c3d61a9a	66c785f0500000ee8f	mov	word ptr [rbp + 0x50f0], 0x8fee	entry_recursive_direct_cfg	0x50f0	2
```
