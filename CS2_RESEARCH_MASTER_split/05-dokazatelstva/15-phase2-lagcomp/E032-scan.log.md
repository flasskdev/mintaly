<!-- split-part | CS2_RESEARCH_MASTER.md lines 13190-13271 | body-sha256 4300b66b101fb0b0b85b87c74b3295f0d1de36a9e57855f9f95b90e47ecf9f97 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-032"></a>

## E032. `analysis/phase2/lagcomp/scan.log`

Bytes: 4484. SHA-256: `08a3179f09459e4abee4c30e9b1f9ef5252e2643ac81a8827f60bf002a7a6f29`.

```text
{
  "sha256": "3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27",
  "ranges": 45064,
  "full_linear": 43604,
  "field_refs": 70,
  "functions_written": 14
}
0x37a020 0x37a61a 83b9f050000000 cmp dword ptr [rcx + 0x50f0], 0
0x37a020 0x37a4ea 448b89f4500000 mov r9d, dword ptr [rcx + 0x50f4]
0x37a020 0x37a4dd 83b9f050000000 cmp dword ptr [rcx + 0x50f0], 0
0x4707f0 0x471003 48c786f050000000000000 mov qword ptr [rsi + 0x50f0], 0
0x471d70 0x471d7d 48c786f050000000000000 mov qword ptr [rsi + 0x50f0], 0
0x4721d0 0x472240 8b86f0500000 mov eax, dword ptr [rsi + 0x50f0]
0x4721d0 0x472258 448b86f4500000 mov r8d, dword ptr [rsi + 0x50f4]
0x4721d0 0x472297 898ef0500000 mov dword ptr [rsi + 0x50f0], ecx
0x4721d0 0x472834 8b8ef4500000 mov ecx, dword ptr [rsi + 0x50f4]
0x4721d0 0x4728a8 898ef4500000 mov dword ptr [rsi + 0x50f4], ecx
0x4721d0 0x4728b5 8b8ef4500000 mov ecx, dword ptr [rsi + 0x50f4]
0x4721d0 0x4728d0 898ef4500000 mov dword ptr [rsi + 0x50f4], ecx
0x4721d0 0x4728e8 8986f0500000 mov dword ptr [rsi + 0x50f0], eax
0x4721d0 0x4727a9 8b86f0500000 mov eax, dword ptr [rsi + 0x50f0]
0x4721d0 0x4727af 8b8ef4500000 mov ecx, dword ptr [rsi + 0x50f4]
0x4721d0 0x47280b 8986f0500000 mov dword ptr [rsi + 0x50f0], eax
0x4721d0 0x47282c 898ef4500000 mov dword ptr [rsi + 0x50f4], ecx
0x4721d0 0x4732a1 83bef050000002 cmp dword ptr [rsi + 0x50f0], 2
0x4721d0 0x4732ae 8b8ef4500000 mov ecx, dword ptr [rsi + 0x50f4]
0x4721d0 0x473375 c786f050000001000000 mov dword ptr [rsi + 0x50f0], 1
0x474d20 0x474d32 83b9f050000000 cmp dword ptr [rcx + 0x50f0], 0
0x474d20 0x474d45 8b81f4500000 mov eax, dword ptr [rcx + 0x50f4]
0x48eab0 0x48ed84 8b80f0500000 mov eax, dword ptr [rax + 0x50f0]
0x48eab0 0x48ee54 8b80f4500000 mov eax, dword ptr [rax + 0x50f4]
0x4aad60 0x4aaed8 8b87f4500000 mov eax, dword ptr [rdi + 0x50f4]
0x4b9600 0x4b98b3 83b8f050000000 cmp dword ptr [rax + 0x50f0], 0
0x4b9600 0x4b98c0 8b88f4500000 mov ecx, dword ptr [rax + 0x50f4]
0x4feba0 0x4ffd3a 418b8ef0500000 mov ecx, dword ptr [r14 + 0x50f0]
0x4feba0 0x4ffd49 418b86f4500000 mov eax, dword ptr [r14 + 0x50f4]
0x512280 0x51265f 8b81f0500000 mov eax, dword ptr [rcx + 0x50f0]
0x512280 0x512669 8b91f4500000 mov edx, dword ptr [rcx + 0x50f4]
0x51b480 0x51b567 83bbf050000000 cmp dword ptr [rbx + 0x50f0], 0
0x51b480 0x51b57b 8b83f4500000 mov eax, dword ptr [rbx + 0x50f4]
0x51b480 0x51b611 83bbf050000002 cmp dword ptr [rbx + 0x50f0], 2
0x51b480 0x51b61a 8b83f4500000 mov eax, dword ptr [rbx + 0x50f4]
0x51e4f0 0x51ef14 448bb6f0500000 mov r14d, dword ptr [rsi + 0x50f0]
0x51e4f0 0x51ef96 8b86f4500000 mov eax, dword ptr [rsi + 0x50f4]
0x52b250 0x52b31c 8baff0500000 mov ebp, dword ptr [rdi + 0x50f0]
0x52b250 0x52b38c 8b87f4500000 mov eax, dword ptr [rdi + 0x50f4]
0x52cd20 0x52cda5 458b81f0500000 mov r8d, dword ptr [r9 + 0x50f0]
0x52cd20 0x52cdc2 458b91f4500000 mov r10d, dword ptr [r9 + 0x50f4]
0x52cd20 0x52cf7c 486381f0500000 movsxd rax, dword ptr [rcx + 0x50f0]
0x52cd20 0x52cfa4 8b89f4500000 mov ecx, dword ptr [rcx + 0x50f4]
0x52cd20 0x52d6dd 418b9424f0500000 mov edx, dword ptr [r12 + 0x50f0]
0x52cd20 0x52d773 458b8c24f4500000 mov r9d, dword ptr [r12 + 0x50f4]
0x52cd20 0x52d8dc 458b9424f4500000 mov r10d, dword ptr [r12 + 0x50f4]
0x52dd20 0x52df01 4183bef050000000 cmp dword ptr [r14 + 0x50f0], 0
0x52dd20 0x52df0b 418b86f4500000 mov eax, dword ptr [r14 + 0x50f4]
0x52f030 0x52f0b4 83bbf050000000 cmp dword ptr [rbx + 0x50f0], 0
0x665540 0x66c60b 83bef050000000 cmp dword ptr [rsi + 0x50f0], 0
0x665540 0x66c9df 83bef050000000 cmp dword ptr [rsi + 0x50f0], 0
0x665540 0x66c836 8b86f4500000 mov eax, dword ptr [rsi + 0x50f4]
0x665540 0x66c8a7 8b8ef0500000 mov ecx, dword ptr [rsi + 0x50f0]
0x665540 0x66c912 448b86f4500000 mov r8d, dword ptr [rsi + 0x50f4]
0x6788a0 0x678b9d 418b9c24f0500000 mov ebx, dword ptr [r12 + 0x50f0]
0x6788a0 0x678bfa 418b8424f4500000 mov eax, dword ptr [r12 + 0x50f4]
0x6ac690 0x6ae648 488b87f0500000 mov rax, qword ptr [rdi + 0x50f0]
0x6ac690 0x6ae64f 49898424f0500000 mov qword ptr [r12 + 0x50f0], rax
0x6ac690 0x6af3d6 83bef050000000 cmp dword ptr [rsi + 0x50f0], 0
0x6ac690 0x6af3e3 8b86f4500000 mov eax, dword ptr [rsi + 0x50f4]
0x6ac690 0x6b2d8e 418b8df0500000 mov ecx, dword ptr [r13 + 0x50f0]
0x6ac690 0x6b2d99 418b95f4500000 mov edx, dword ptr [r13 + 0x50f4]
0x6ac690 0x6b2aa0 8b89f0500000 mov ecx, dword ptr [rcx + 0x50f0]
0x6ac690 0x6b2abe 8b92f4500000 mov edx, dword ptr [rdx + 0x50f4]
0x6ce200 0x6ce2f9 48c787f050000000000000 mov qword ptr [rdi + 0x50f0], 0
```
