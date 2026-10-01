<!-- split-part | CS2_RESEARCH_MASTER.md lines 60249-60397 | body-sha256 3241a94560a11a1493791074483196d778d6acc4387bf34f5ac66695f297f7da -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-047"></a>

## E047. `analysis/phase2/main/math_kernels_original.tsv`

Bytes: 4883. SHA-256: `a3b95323482057ff0361d1faa4540377ed637681524b636fd6840cfb97e66bbc`.

```tsv
rva	bytes	instruction
0xd93590	8b053edb9b00	mov eax, dword ptr [rip + 0x9bdb3e]
0xd93596	83e003	and eax, 3
0xd93599	3c03	cmp al, 3
0xd9359b	0f844f610200	je 0xdb96f0
0xd935a1	e90a000000	jmp 0xd935b0
0xd935a6	cc	int3 
0xd935a7	cc	int3 
0xd935a8	cc	int3 
0xd935a9	cc	int3 
0xd935aa	cc	int3 
0xd935ab	cc	int3 
0xd935ac	cc	int3 
0xd935ad	cc	int3 
0xd935ae	cc	int3 
0xd935af	cc	int3 
0xd935b0	0f28c8	movaps xmm1, xmm0
0xd935b3	0f57db	xorps xmm3, xmm3
0xd935b6	f30f114c2408	movss dword ptr [rsp + 8], xmm1
0xd935bc	8b442408	mov eax, dword ptr [rsp + 8]
0xd935c0	c1e814	shr eax, 0x14
0xd935c3	25ff070000	and eax, 0x7ff
0xd935c8	f30f5ad9	cvtss2sd xmm3, xmm1
0xd935cc	3d30040000	cmp eax, 0x430
0xd935d1	7246	jb 0xd93619
0xd935d3	f30f114c2408	movss dword ptr [rsp + 8], xmm1
0xd935d9	817c2408000080ff	cmp dword ptr [rsp + 8], 0xff800000
0xd935e1	7504	jne 0xd935e7
0xd935e3	0f57c0	xorps xmm0, xmm0
0xd935e6	c3	ret 
0xd935e7	3df8070000	cmp eax, 0x7f8
0xd935ec	7208	jb 0xd935f6
0xd935ee	f30f58c9	addss xmm1, xmm1
0xd935f2	0f28c1	movaps xmm0, xmm1
0xd935f5	c3	ret 
0xd935f6	0f57c0	xorps xmm0, xmm0
0xd935f9	0f2fc8	comiss xmm1, xmm0
0xd935fc	7607	jbe 0xd93605
0xd935fe	33c9	xor ecx, ecx
0xd93600	e9cb610200	jmp 0xdb97d0
0xd93605	f30f100513460f00	movss xmm0, dword ptr [rip + 0xf4613]
0xd9360d	0f2fc1	comiss xmm0, xmm1
0xd93610	7207	jb 0xd93619
0xd93612	33c9	xor ecx, ecx
0xd93614	e907620200	jmp 0xdb9820
0xd93619	0f28d3	movaps xmm2, xmm3
0xd9361c	488d15cd9c0f00	lea rdx, [rip + 0xf9ccd]
0xd93623	f20f5815c59d0f00	addsd xmm2, qword ptr [rip + 0xf9dc5]
0xd9362b	0f28c2	movaps xmm0, xmm2
0xd9362e	66480f7ed1	movq rcx, xmm2
0xd93633	f20f5c05b59d0f00	subsd xmm0, qword ptr [rip + 0xf9db5]
0xd9363b	488bc1	mov rax, rcx
0xd9363e	48c1e12f	shl rcx, 0x2f
0xd93642	83e01f	and eax, 0x1f
0xd93645	f20f5cd8	subsd xmm3, xmm0
0xd93649	48030cc2	add rcx, qword ptr [rdx + rax*8]
0xd9364d	0f28cb	movaps xmm1, xmm3
0xd93650	0f28c3	movaps xmm0, xmm3
0xd93653	f20f590d9d9d0f00	mulsd xmm1, qword ptr [rip + 0xf9d9d]
0xd9365b	f20f59c3	mulsd xmm0, xmm3
0xd9365f	f20f591da19d0f00	mulsd xmm3, qword ptr [rip + 0xf9da1]
0xd93667	f20f580d919d0f00	addsd xmm1, qword ptr [rip + 0xf9d91]
0xd9366f	f20f581dc9930500	addsd xmm3, qword ptr [rip + 0x593c9]
0xd93677	f20f59c8	mulsd xmm1, xmm0
0xd9367b	66480f6ec1	movq xmm0, rcx
0xd93680	f20f58cb	addsd xmm1, xmm3
0xd93684	f20f59c8	mulsd xmm1, xmm0
0xd93688	660f5ac1	cvtpd2ps xmm0, xmm1
0xd9368c	c3	ret 
0xd97150	8b057e9f9b00	mov eax, dword ptr [rip + 0x9b9f7e]
0xd97156	83e003	and eax, 3
0xd97159	3c03	cmp al, 3
0xd9715b	0f84bf410200	je 0xdbb320
0xd97161	e90a000000	jmp 0xd97170
0xd97166	cc	int3 
0xd97167	cc	int3 
0xd97168	cc	int3 
0xd97169	cc	int3 
0xd9716a	cc	int3 
0xd9716b	cc	int3 
0xd9716c	cc	int3 
0xd9716d	cc	int3 
0xd9716e	cc	int3 
0xd9716f	cc	int3 
0xd97170	660f7ec2	movd edx, xmm0
0xd97174	81fa0000803f	cmp edx, 0x3f800000
0xd9717a	7504	jne 0xd97180
0xd9717c	0f57c0	xorps xmm0, xmm0
0xd9717f	c3	ret 
0xd97180	8d82000080ff	lea eax, [rdx - 0x800000]
0xd97186	3d0000007f	cmp eax, 0x7f000000
0xd9718b	7240	jb 0xd971cd
0xd9718d	8d0412	lea eax, [rdx + rdx]
0xd97190	85c0	test eax, eax
0xd97192	7508	jne 0xd9719c
0xd97194	8d4801	lea ecx, [rax + 1]
0xd97197	e9a4260200	jmp 0xdb9840
0xd9719c	81fa0000807f	cmp edx, 0x7f800000
0xd971a2	0f84b2000000	je 0xd9725a
0xd971a8	85d2	test edx, edx
0xd971aa	0f88ab000000	js 0xd9725b
0xd971b0	3d000000ff	cmp eax, 0xff000000
0xd971b5	0f83a0000000	jae 0xd9725b
0xd971bb	f30f5905f90f0f00	mulss xmm0, dword ptr [rip + 0xf0ff9]
0xd971c3	660f7ec2	movd edx, xmm0
0xd971c7	81c2000080f4	add edx, 0xf4800000
0xd971cd	8d8a0000cdc0	lea ecx, [rdx - 0x3f330000]
0xd971d3	0f57c9	xorps xmm1, xmm1
0xd971d6	8bc1	mov eax, ecx
0xd971d8	25000080ff	and eax, 0xff800000
0xd971dd	2bd0	sub edx, eax
0xd971df	8bc1	mov eax, ecx
0xd971e1	48c1e813	shr rax, 0x13
0xd971e5	89542408	mov dword ptr [rsp + 8], edx
0xd971e9	83e00f	and eax, 0xf
0xd971ec	f30f105c2408	movss xmm3, dword ptr [rsp + 8]
0xd971f2	488d15678c0f00	lea rdx, [rip + 0xf8c67]
0xd971f9	0f5adb	cvtps2pd xmm3, xmm3
0xd971fc	4803c0	add rax, rax
0xd971ff	c1f917	sar ecx, 0x17
0xd97202	f20f2ac9	cvtsi2sd xmm1, ecx
0xd97206	f20f591cc2	mulsd xmm3, qword ptr [rdx + rax*8]
0xd9720b	f20f584cc208	addsd xmm1, qword ptr [rdx + rax*8 + 8]
0xd97211	f20f5c1d27580500	subsd xmm3, qword ptr [rip + 0x55827]
0xd97219	0f28c3	movaps xmm0, xmm3
0xd9721c	0f28d3	movaps xmm2, xmm3
0xd9721f	f20f5905518d0f00	mulsd xmm0, qword ptr [rip + 0xf8d51]
0xd97227	f20f59d3	mulsd xmm2, xmm3
0xd9722b	f20f591d358d0f00	mulsd xmm3, qword ptr [rip + 0xf8d35]
0xd97233	f20f58c8	addsd xmm1, xmm0
0xd97237	0f28c2	movaps xmm0, xmm2
0xd9723a	f20f59051e8d0f00	mulsd xmm0, qword ptr [rip + 0xf8d1e]
0xd97242	f20f581d268d0f00	addsd xmm3, qword ptr [rip + 0xf8d26]
0xd9724a	f20f58d8	addsd xmm3, xmm0
0xd9724e	f20f59da	mulsd xmm3, xmm2
0xd97252	f20f58cb	addsd xmm1, xmm3
0xd97256	660f5ac1	cvtpd2ps xmm0, xmm1
0xd9725a	c3	ret 
0xd9725b	e940260200	jmp 0xdb98a0
```
