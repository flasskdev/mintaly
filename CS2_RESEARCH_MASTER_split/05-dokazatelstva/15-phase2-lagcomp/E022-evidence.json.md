<!-- split-part | CS2_RESEARCH_MASTER.md lines 7335-9747 | body-sha256 814a3e91a7dd842811345c51cee0bc6693f5094d982d96da262035e63cfeb24f -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-022"></a>

## E022. `analysis/phase2/lagcomp/evidence.json`

Bytes: 67403. SHA-256: `333fd31548c0407cf89505653beddb685740a3cc9753642dcf934a2948387184`.

```json
{
  "base": "0x212c3300000",
  "sha256": "3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27",
  "assumptions": [
    "All facts are static; no sample execution or emulation",
    "Exact undecodable 0F 1A/1B/1C 24 10 may be advanced as 4-byte NOP per user/Ghidra observation",
    "Both conditional branch arms retained; no PEB/KUSER runtime values claimed",
    "Separate user conditional model assumes PEBhash==0x5877 and KUSERsum==0x92FB254D; not applied as runtime facts here",
    "Simplified numerical guards assume finite normalized times and valid count/head invariants"
  ],
  "claims": [
    {
      "id": "aging_cutoff",
      "function_rva": "0x4721d0",
      "statement": "Cutoff = cvttss2si((time_source - helper_473490()) * 64.0 + 0.5); source selected by object+0x28.",
      "sites": [
        {
          "rva": "0x4721f5",
          "va": "0x212c37721f5",
          "bytes": "80792801",
          "mnemonic": "cmp",
          "operands": "byte ptr [rcx + 0x28], 1"
        },
        {
          "rva": "0x4721fb",
          "va": "0x212c37721fb",
          "bytes": "488d4e4c",
          "mnemonic": "lea",
          "operands": "rcx, [rsi + 0x4c]"
        },
        {
          "rva": "0x472201",
          "va": "0x212c3772201",
          "bytes": "e87a50e4ff",
          "mnemonic": "call",
          "operands": "0x212c35b7280"
        },
        {
          "rva": "0x472206",
          "va": "0x212c3772206",
          "bytes": "488d8890000000",
          "mnemonic": "lea",
          "operands": "rcx, [rax + 0x90]"
        },
        {
          "rva": "0x472214",
          "va": "0x212c3772214",
          "bytes": "4883c130",
          "mnemonic": "add",
          "operands": "rcx, 0x30"
        },
        {
          "rva": "0x472218",
          "va": "0x212c3772218",
          "bytes": "c5fa1031",
          "mnemonic": "vmovss",
          "operands": "xmm6, dword ptr [rcx]"
        },
        {
          "rva": "0x47221c",
          "va": "0x212c377221c",
          "bytes": "e86f120000",
          "mnemonic": "call",
          "operands": "0x212c3773490"
        },
        {
          "rva": "0x472225",
          "va": "0x212c3772225",
          "bytes": "c5fa59050b669e00",
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x9e660b]"
        },
        {
          "rva": "0x47222d",
          "va": "0x212c377222d",
          "bytes": "c5fa5805a3a59700",
          "mnemonic": "vaddss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x97a5a3]"
        },
        {
          "rva": "0x472235",
          "va": "0x212c3772235",
          "bytes": "c5fa2cd0",
          "mnemonic": "vcvttss2si",
          "operands": "edx, xmm0"
        }
      ]
    },
    {
      "id": "aging_tail",
      "function_rva": "0x4721d0",
      "statement": "Scan slot[(head + candidate_count - 1) % 16].tick, reject old tails by signed JL; commit reduced count only on first surviving tail.",
      "sites": [
        {
          "rva": "0x472240",
          "va": "0x212c3772240",
          "bytes": "8b86f0500000",
          "mnemonic": "mov",
          "operands": "eax, dword ptr [rsi + 0x50f0]"
        },
        {
          "rva": "0x472250",
          "va": "0x212c3772250",
          "bytes": "4585c0",
          "mnemonic": "test",
          "operands": "r8d, r8d"
        },
        {
          "rva": "0x472253",
          "va": "0x212c3772253",
          "bytes": "7e4a",
          "mnemonic": "jle",
          "operands": "0x212c377229f"
        },
        {
          "rva": "0x472258",
          "va": "0x212c3772258",
          "bytes": "448b86f4500000",
          "mnemonic": "mov",
          "operands": "r8d, dword ptr [rsi + 0x50f4]"
        },
        {
          "rva": "0x47225f",
          "va": "0x212c377225f",
          "bytes": "468d0c01",
          "mnemonic": "lea",
          "operands": "r9d, [rcx + r8]"
        },
        {
          "rva": "0x472263",
          "va": "0x212c3772263",
          "bytes": "41ffc9",
          "mnemonic": "dec",
          "operands": "r9d"
        },
        {
          "rva": "0x472275",
          "va": "0x212c3772275",
          "bytes": "448d41ff",
          "mnemonic": "lea",
          "operands": "r8d, [rcx - 1]"
        },
        {
          "rva": "0x472279",
          "va": "0x212c3772279",
          "bytes": "4183e2f0",
          "mnemonic": "and",
          "operands": "r10d, 0xfffffff0"
        },
        {
          "rva": "0x472283",
          "va": "0x212c3772283",
          "bytes": "4f8d0c89",
          "mnemonic": "lea",
          "operands": "r9, [r9 + r9*4]"
        },
        {
          "rva": "0x472287",
          "va": "0x212c3772287",
          "bytes": "49c1e108",
          "mnemonic": "shl",
          "operands": "r9, 8"
        },
        {
          "rva": "0x47228b",
          "va": "0x212c377228b",
          "bytes": "4339540d00",
          "mnemonic": "cmp",
          "operands": "dword ptr [r13 + r9], edx"
        },
        {
          "rva": "0x472290",
          "va": "0x212c3772290",
          "bytes": "7cbe",
          "mnemonic": "jl",
          "operands": "0x212c3772250"
        },
        {
          "rva": "0x472292",
          "va": "0x212c3772292",
          "bytes": "39c8",
          "mnemonic": "cmp",
          "operands": "eax, ecx"
        },
        {
          "rva": "0x472294",
          "va": "0x212c3772294",
          "bytes": "0f4cc8",
          "mnemonic": "cmovl",
          "operands": "ecx, eax"
        },
        {
          "rva": "0x472297",
          "va": "0x212c3772297",
          "bytes": "898ef0500000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0x50f0], ecx"
        }
      ]
    },
    {
      "id": "aging_all_stale_caveat",
      "function_rva": "0x4721d0",
      "statement": "If candidate_count reaches zero, JLE skips the count store. This block does NOT prove count=0 for an all-stale ring.",
      "sites": [
        {
          "rva": "0x472246",
          "va": "0x212c3772246",
          "bytes": "4189c0",
          "mnemonic": "mov",
          "operands": "r8d, eax"
        },
        {
          "rva": "0x472250",
          "va": "0x212c3772250",
          "bytes": "4585c0",
          "mnemonic": "test",
          "operands": "r8d, r8d"
        },
        {
          "rva": "0x472253",
          "va": "0x212c3772253",
          "bytes": "7e4a",
          "mnemonic": "jle",
          "operands": "0x212c377229f"
        },
        {
          "rva": "0x472275",
          "va": "0x212c3772275",
          "bytes": "448d41ff",
          "mnemonic": "lea",
          "operands": "r8d, [rcx - 1]"
        },
        {
          "rva": "0x472290",
          "va": "0x212c3772290",
          "bytes": "7cbe",
          "mnemonic": "jl",
          "operands": "0x212c3772250"
        },
        {
          "rva": "0x472297",
          "va": "0x212c3772297",
          "bytes": "898ef0500000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0x50f0], ecx"
        },
        {
          "rva": "0x47229f",
          "va": "0x212c377229f",
          "bytes": "807e2800",
          "mnemonic": "cmp",
          "operands": "byte ptr [rsi + 0x28], 0"
        }
      ]
    },
    {
      "id": "mode_dispatch",
      "function_rva": "0x4721d0",
      "statement": "mode byte==0 enters timestamp guard; nonzero with count==0 enters insertion; other path has environment guards and slot+8 pruning.",
      "sites": [
        {
          "rva": "0x47229f",
          "va": "0x212c377229f",
          "bytes": "807e2800",
          "mnemonic": "cmp",
          "operands": "byte ptr [rsi + 0x28], 0"
        },
        {
          "rva": "0x4722a3",
          "va": "0x212c37722a3",
          "bytes": "0f848b050000",
          "mnemonic": "je",
          "operands": "0x212c3772834"
        },
        {
          "rva": "0x4722a9",
          "va": "0x212c37722a9",
          "bytes": "85c0",
          "mnemonic": "test",
          "operands": "eax, eax"
        },
        {
          "rva": "0x4722ab",
          "va": "0x212c37722ab",
          "bytes": "0f8404060000",
          "mnemonic": "je",
          "operands": "0x212c37728b5"
        },
        {
          "rva": "0x4722b1",
          "va": "0x212c37722b1",
          "bytes": "65488b042560000000",
          "mnemonic": "mov",
          "operands": "rax, qword ptr gs:[0x60]"
        }
      ]
    },
    {
      "id": "mode_assignment",
      "function_rva": "0x6ac690",
      "statement": "Controller sets object+0x28=1 when object entity pointer equals comparison entity pointer; stores source time at +0x4C. Entity role is not identified.",
      "sites": [
        {
          "rva": "0x6aeb80",
          "va": "0x212c39aeb80",
          "bytes": "c6462801",
          "mnemonic": "mov",
          "operands": "byte ptr [rsi + 0x28], 1"
        },
        {
          "rva": "0x6aeb84",
          "va": "0x212c39aeb84",
          "bytes": "48630565160c01",
          "mnemonic": "movsxd",
          "operands": "rax, dword ptr [rip + 0x10c1665]"
        },
        {
          "rva": "0x6aeb8b",
          "va": "0x212c39aeb8b",
          "bytes": "c5fa100410",
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rax + rdx]"
        },
        {
          "rva": "0x6aeb90",
          "va": "0x212c39aeb90",
          "bytes": "c5fa11464c",
          "mnemonic": "vmovss",
          "operands": "dword ptr [rsi + 0x4c], xmm0"
        },
        {
          "rva": "0x6aebcf",
          "va": "0x212c39aebcf",
          "bytes": "488b4e10",
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rsi + 0x10]"
        },
        {
          "rva": "0x6aebd3",
          "va": "0x212c39aebd3",
          "bytes": "488b5340",
          "mnemonic": "mov",
          "operands": "rdx, qword ptr [rbx + 0x40]"
        },
        {
          "rva": "0x6aebd7",
          "va": "0x212c39aebd7",
          "bytes": "4839d1",
          "mnemonic": "cmp",
          "operands": "rcx, rdx"
        },
        {
          "rva": "0x6aebda",
          "va": "0x212c39aebda",
          "bytes": "74a4",
          "mnemonic": "je",
          "operands": "0x212c39aeb80"
        },
        {
          "rva": "0x6aebf0",
          "va": "0x212c39aebf0",
          "bytes": "c6462800",
          "mnemonic": "mov",
          "operands": "byte ptr [rsi + 0x28], 0"
        }
      ]
    },
    {
      "id": "prune_slot8_front",
      "function_rva": "0x4721d0",
      "statement": "Guarded mode path removes newest slots while slot+8 >= auxiliary source integer: decrement count and increment head modulo 16.",
      "sites": [
        {
          "rva": "0x4727a9",
          "va": "0x212c37727a9",
          "bytes": "8b86f0500000",
          "mnemonic": "mov",
          "operands": "eax, dword ptr [rsi + 0x50f0]"
        },
        {
          "rva": "0x4727af",
          "va": "0x212c37727af",
          "bytes": "8b8ef4500000",
          "mnemonic": "mov",
          "operands": "ecx, dword ptr [rsi + 0x50f4]"
        },
        {
          "rva": "0x4727bd",
          "va": "0x212c37727bd",
          "bytes": "488b55e0",
          "mnemonic": "mov",
          "operands": "rdx, qword ptr [rbp - 0x20]"
        },
        {
          "rva": "0x4727c1",
          "va": "0x212c37727c1",
          "bytes": "488b5238",
          "mnemonic": "mov",
          "operands": "rdx, qword ptr [rdx + 0x38]"
        },
        {
          "rva": "0x4727f7",
          "va": "0x212c37727f7",
          "bytes": "468b8406f8000000",
          "mnemonic": "mov",
          "operands": "r8d, dword ptr [rsi + r8 + 0xf8]"
        },
        {
          "rva": "0x4727ff",
          "va": "0x212c37727ff",
          "bytes": "443b4208",
          "mnemonic": "cmp",
          "operands": "r8d, dword ptr [rdx + 8]"
        },
        {
          "rva": "0x472803",
          "va": "0x212c3772803",
          "bytes": "7c35",
          "mnemonic": "jl",
          "operands": "0x212c377283a"
        },
        {
          "rva": "0x472809",
          "va": "0x212c3772809",
          "bytes": "ffc8",
          "mnemonic": "dec",
          "operands": "eax"
        },
        {
          "rva": "0x47280b",
          "va": "0x212c377280b",
          "bytes": "8986f0500000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0x50f0], eax"
        },
        {
          "rva": "0x472811",
          "va": "0x212c3772811",
          "bytes": "448d4101",
          "mnemonic": "lea",
          "operands": "r8d, [rcx + 1]"
        },
        {
          "rva": "0x472820",
          "va": "0x212c3772820",
          "bytes": "4183e1f0",
          "mnemonic": "and",
          "operands": "r9d, 0xfffffff0"
        },
        {
          "rva": "0x47282c",
          "va": "0x212c377282c",
          "bytes": "898ef4500000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0x50f4], ecx"
        },
        {
          "rva": "0x472832",
          "va": "0x212c3772832",
          "bytes": "eb9c",
          "mnemonic": "jmp",
          "operands": "0x212c37727d0"
        }
      ]
    },
    {
      "id": "tick_admission",
      "function_rva": "0x4721d0",
      "statement": "If nonempty and newest.tick >= cvttss2si(source_time*64+0.5), return without inserting. No fraction comparison here.",
      "sites": [
        {
          "rva": "0x472834",
          "va": "0x212c3772834",
          "bytes": "8b8ef4500000",
          "mnemonic": "mov",
          "operands": "ecx, dword ptr [rsi + 0x50f4]"
        },
        {
          "rva": "0x47283a",
          "va": "0x212c377283a",
          "bytes": "85c0",
          "mnemonic": "test",
          "operands": "eax, eax"
        },
        {
          "rva": "0x47283c",
          "va": "0x212c377283c",
          "bytes": "747d",
          "mnemonic": "je",
          "operands": "0x212c37728bb"
        },
        {
          "rva": "0x47285e",
          "va": "0x212c377285e",
          "bytes": "488b5e10",
          "mnemonic": "mov",
          "operands": "rbx, qword ptr [rsi + 0x10]"
        },
        {
          "rva": "0x472862",
          "va": "0x212c3772862",
          "bytes": "48631587d92f01",
          "mnemonic": "movsxd",
          "operands": "rdx, dword ptr [rip + 0x12fd987]"
        },
        {
          "rva": "0x472869",
          "va": "0x212c3772869",
          "bytes": "c5fa10041a",
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rdx + rbx]"
        },
        {
          "rva": "0x47286e",
          "va": "0x212c377286e",
          "bytes": "c5fa5905c25f9e00",
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x9e5fc2]"
        },
        {
          "rva": "0x472876",
          "va": "0x212c3772876",
          "bytes": "c5fa58055a9f9700",
          "mnemonic": "vaddss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x979f5a]"
        },
        {
          "rva": "0x47287e",
          "va": "0x212c377287e",
          "bytes": "c57a2cd0",
          "mnemonic": "vcvttss2si",
          "operands": "r10d, xmm0"
        },
        {
          "rva": "0x472882",
          "va": "0x212c3772882",
          "bytes": "4739540d00",
          "mnemonic": "cmp",
          "operands": "dword ptr [r13 + r9], r10d"
        },
        {
          "rva": "0x472887",
          "va": "0x212c3772887",
          "bytes": "0f8df20a0000",
          "mnemonic": "jge",
          "operands": "0x212c377337f"
        }
      ]
    },
    {
      "id": "ring_insert",
      "function_rva": "0x4721d0",
      "statement": "New head=(head+15)%16; count incremented only when <=15, fresh empty path sets count=1. Slot=object+0xF0+head*0x500.",
      "sites": [
        {
          "rva": "0x472239",
          "va": "0x212c3772239",
          "bytes": "4c8daef0000000",
          "mnemonic": "lea",
          "operands": "r13, [rsi + 0xf0]"
        },
        {
          "rva": "0x4728a8",
          "va": "0x212c37728a8",
          "bytes": "898ef4500000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0x50f4], ecx"
        },
        {
          "rva": "0x4728ae",
          "va": "0x212c37728ae",
          "bytes": "83f80f",
          "mnemonic": "cmp",
          "operands": "eax, 0xf"
        },
        {
          "rva": "0x4728b1",
          "va": "0x212c37728b1",
          "bytes": "7e33",
          "mnemonic": "jle",
          "operands": "0x212c37728e6"
        },
        {
          "rva": "0x4728b3",
          "va": "0x212c37728b3",
          "bytes": "eb39",
          "mnemonic": "jmp",
          "operands": "0x212c37728ee"
        },
        {
          "rva": "0x4728bb",
          "va": "0x212c37728bb",
          "bytes": "8d410f",
          "mnemonic": "lea",
          "operands": "eax, [rcx + 0xf]"
        },
        {
          "rva": "0x4728c6",
          "va": "0x212c37728c6",
          "bytes": "83e2f0",
          "mnemonic": "and",
          "operands": "edx, 0xfffffff0"
        },
        {
          "rva": "0x4728d0",
          "va": "0x212c37728d0",
          "bytes": "898ef4500000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0x50f4], ecx"
        },
        {
          "rva": "0x4728e4",
          "va": "0x212c37728e4",
          "bytes": "31c0",
          "mnemonic": "xor",
          "operands": "eax, eax"
        },
        {
          "rva": "0x4728e6",
          "va": "0x212c37728e6",
          "bytes": "ffc0",
          "mnemonic": "inc",
          "operands": "eax"
        },
        {
          "rva": "0x4728e8",
          "va": "0x212c37728e8",
          "bytes": "8986f0500000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0x50f0], eax"
        },
        {
          "rva": "0x4728ee",
          "va": "0x212c37728ee",
          "bytes": "4863c1",
          "mnemonic": "movsxd",
          "operands": "rax, ecx"
        },
        {
          "rva": "0x4728f1",
          "va": "0x212c37728f1",
          "bytes": "488d3c80",
          "mnemonic": "lea",
          "operands": "rdi, [rax + rax*4]"
        },
        {
          "rva": "0x4728f5",
          "va": "0x212c37728f5",
          "bytes": "48c1e708",
          "mnemonic": "shl",
          "operands": "rdi, 8"
        },
        {
          "rva": "0x4728f9",
          "va": "0x212c37728f9",
          "bytes": "4c01ef",
          "mnemonic": "add",
          "operands": "rdi, r13"
        }
      ]
    },
    {
      "id": "slot_time_producer",
      "function_rva": "0x4721d0",
      "statement": "V_modff(source_time*64) produces integer/fraction, with negative-fraction borrow normalization; writes slot+0 and +4.",
      "sites": [
        {
          "rva": "0x4728fc",
          "va": "0x212c37728fc",
          "bytes": "c5fa1002",
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rdx]"
        },
        {
          "rva": "0x472900",
          "va": "0x212c3772900",
          "bytes": "c5fa5905305f9e00",
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x9e5f30]"
        },
        {
          "rva": "0x472911",
          "va": "0x212c3772911",
          "bytes": "ff15d1fdb100",
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xb1fdd1]"
        },
        {
          "rva": "0x47291f",
          "va": "0x212c377291f",
          "bytes": "7621",
          "mnemonic": "jbe",
          "operands": "0x212c3772942"
        },
        {
          "rva": "0x472929",
          "va": "0x212c3772929",
          "bytes": "c5fa58c1",
          "mnemonic": "vaddss",
          "operands": "xmm0, xmm0, xmm1"
        },
        {
          "rva": "0x472936",
          "va": "0x212c3772936",
          "bytes": "7311",
          "mnemonic": "jae",
          "operands": "0x212c3772949"
        },
        {
          "rva": "0x472938",
          "va": "0x212c3772938",
          "bytes": "c5f2580d7c419900",
          "mnemonic": "vaddss",
          "operands": "xmm1, xmm1, dword ptr [rip + 0x99417c]"
        },
        {
          "rva": "0x472949",
          "va": "0x212c3772949",
          "bytes": "c5f857c0",
          "mnemonic": "vxorps",
          "operands": "xmm0, xmm0, xmm0"
        },
        {
          "rva": "0x47294d",
          "va": "0x212c377294d",
          "bytes": "c5fa2cc1",
          "mnemonic": "vcvttss2si",
          "operands": "eax, xmm1"
        },
        {
          "rva": "0x472951",
          "va": "0x212c3772951",
          "bytes": "8907",
          "mnemonic": "mov",
          "operands": "dword ptr [rdi], eax"
        },
        {
          "rva": "0x472953",
          "va": "0x212c3772953",
          "bytes": "c5fa114704",
          "mnemonic": "vmovss",
          "operands": "dword ptr [rdi + 4], xmm0"
        }
      ]
    },
    {
      "id": "slot_position_producer",
      "function_rva": "0x4721d0",
      "statement": "Read dynamic-offset entity component; write position XYZ at slot+0x14/+0x18/+0x1C.",
      "sites": [
        {
          "rva": "0x472958",
          "va": "0x212c3772958",
          "bytes": "486305fdd92f01",
          "mnemonic": "movsxd",
          "operands": "rax, dword ptr [rip + 0x12fd9fd]"
        },
        {
          "rva": "0x47295f",
          "va": "0x212c377295f",
          "bytes": "488b0403",
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rbx + rax]"
        },
        {
          "rva": "0x472963",
          "va": "0x212c3772963",
          "bytes": "48630d5ecf2f01",
          "mnemonic": "movsxd",
          "operands": "rcx, dword ptr [rip + 0x12fcf5e]"
        },
        {
          "rva": "0x47296a",
          "va": "0x212c377296a",
          "bytes": "8b540108",
          "mnemonic": "mov",
          "operands": "edx, dword ptr [rcx + rax + 8]"
        },
        {
          "rva": "0x47296e",
          "va": "0x212c377296e",
          "bytes": "89571c",
          "mnemonic": "mov",
          "operands": "dword ptr [rdi + 0x1c], edx"
        },
        {
          "rva": "0x472971",
          "va": "0x212c3772971",
          "bytes": "488b0401",
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rcx + rax]"
        },
        {
          "rva": "0x472975",
          "va": "0x212c3772975",
          "bytes": "48894714",
          "mnemonic": "mov",
          "operands": "qword ptr [rdi + 0x14], rax"
        }
      ]
    },
    {
      "id": "slot_angles_producer",
      "function_rva": "0x4721d0",
      "statement": "Call 0x153120(entity); copy three floats to slot+0x44/+0x48/+0x4C. Consumer uses first two.",
      "sites": [
        {
          "rva": "0x4729c5",
          "va": "0x212c37729c5",
          "bytes": "4889d9",
          "mnemonic": "mov",
          "operands": "rcx, rbx"
        },
        {
          "rva": "0x4729c8",
          "va": "0x212c37729c8",
          "bytes": "e85307ceff",
          "mnemonic": "call",
          "operands": "0x212c3453120"
        },
        {
          "rva": "0x4729cd",
          "va": "0x212c37729cd",
          "bytes": "488b08",
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rax]"
        },
        {
          "rva": "0x4729d0",
          "va": "0x212c37729d0",
          "bytes": "48894f44",
          "mnemonic": "mov",
          "operands": "qword ptr [rdi + 0x44], rcx"
        },
        {
          "rva": "0x4729d4",
          "va": "0x212c37729d4",
          "bytes": "8b4008",
          "mnemonic": "mov",
          "operands": "eax, dword ptr [rax + 8]"
        },
        {
          "rva": "0x4729d7",
          "va": "0x212c37729d7",
          "bytes": "89474c",
          "mnemonic": "mov",
          "operands": "dword ptr [rdi + 0x4c], eax"
        }
      ]
    },
    {
      "id": "slot_aux_integer",
      "function_rva": "0x4721d0",
      "statement": "slot+8 defaults to -1; mode==1 guarded path assigns [[opaque_root]+0x38]+8. Do not name it command number without further evidence.",
      "sites": [
        {
          "rva": "0x472b20",
          "va": "0x212c3772b20",
          "bytes": "c74708ffffffff",
          "mnemonic": "mov",
          "operands": "dword ptr [rdi + 8], 0xffffffff"
        },
        {
          "rva": "0x472b27",
          "va": "0x212c3772b27",
          "bytes": "807e2801",
          "mnemonic": "cmp",
          "operands": "byte ptr [rsi + 0x28], 1"
        },
        {
          "rva": "0x472b2b",
          "va": "0x212c3772b2b",
          "bytes": "0f85f0040000",
          "mnemonic": "jne",
          "operands": "0x212c3773021"
        },
        {
          "rva": "0x473013",
          "va": "0x212c3773013",
          "bytes": "488b45e0",
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rbp - 0x20]"
        },
        {
          "rva": "0x473017",
          "va": "0x212c3773017",
          "bytes": "488b4038",
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rax + 0x38]"
        },
        {
          "rva": "0x47301b",
          "va": "0x212c377301b",
          "bytes": "8b4008",
          "mnemonic": "mov",
          "operands": "eax, dword ptr [rax + 8]"
        },
        {
          "rva": "0x47301e",
          "va": "0x212c377301e",
          "bytes": "894708",
          "mnemonic": "mov",
          "operands": "dword ptr [rdi + 8], eax"
        }
      ]
    },
    {
      "id": "discontinuity",
      "function_rva": "0x4721d0",
      "statement": "When count>=2 compare newest/previous XYZ squared distance to float(4096*clamp(signed tick difference,1,5)); strict greater or slot+A0 mismatch => count=1.",
      "sites": [
        {
          "rva": "0x4732a1",
          "va": "0x212c37732a1",
          "bytes": "83bef050000002",
          "mnemonic": "cmp",
          "operands": "dword ptr [rsi + 0x50f0], 2"
        },
        {
          "rva": "0x4732a8",
          "va": "0x212c37732a8",
          "bytes": "0f8cd1000000",
          "mnemonic": "jl",
          "operands": "0x212c377337f"
        },
        {
          "rva": "0x4732ae",
          "va": "0x212c37732ae",
          "bytes": "8b8ef4500000",
          "mnemonic": "mov",
          "operands": "ecx, dword ptr [rsi + 0x50f4]"
        },
        {
          "rva": "0x4732f2",
          "va": "0x212c37732f2",
          "bytes": "c4c17a10440514",
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [r13 + rax + 0x14]"
        },
        {
          "rva": "0x4732f9",
          "va": "0x212c37732f9",
          "bytes": "c4c17a5c440d14",
          "mnemonic": "vsubss",
          "operands": "xmm0, xmm0, dword ptr [r13 + rcx + 0x14]"
        },
        {
          "rva": "0x473300",
          "va": "0x212c3773300",
          "bytes": "c4c17a104c0518",
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [r13 + rax + 0x18]"
        },
        {
          "rva": "0x473307",
          "va": "0x212c3773307",
          "bytes": "c4c1725c4c0d18",
          "mnemonic": "vsubss",
          "operands": "xmm1, xmm1, dword ptr [r13 + rcx + 0x18]"
        },
        {
          "rva": "0x47330e",
          "va": "0x212c377330e",
          "bytes": "c4c17a1054051c",
          "mnemonic": "vmovss",
          "operands": "xmm2, dword ptr [r13 + rax + 0x1c]"
        },
        {
          "rva": "0x473315",
          "va": "0x212c3773315",
          "bytes": "c4c16a5c540d1c",
          "mnemonic": "vsubss",
          "operands": "xmm2, xmm2, dword ptr [r13 + rcx + 0x1c]"
        },
        {
          "rva": "0x473328",
          "va": "0x212c3773328",
          "bytes": "418b540500",
          "mnemonic": "mov",
          "operands": "edx, dword ptr [r13 + rax]"
        },
        {
          "rva": "0x47332d",
          "va": "0x212c377332d",
          "bytes": "412b540d00",
          "mnemonic": "sub",
          "operands": "edx, dword ptr [r13 + rcx]"
        },
        {
          "rva": "0x473332",
          "va": "0x212c3773332",
          "bytes": "83fa02",
          "mnemonic": "cmp",
          "operands": "edx, 2"
        },
        {
          "rva": "0x473335",
          "va": "0x212c3773335",
          "bytes": "41b801000000",
          "mnemonic": "mov",
          "operands": "r8d, 1"
        },
        {
          "rva": "0x47333b",
          "va": "0x212c377333b",
          "bytes": "440f4dc2",
          "mnemonic": "cmovge",
          "operands": "r8d, edx"
        },
        {
          "rva": "0x47333f",
          "va": "0x212c377333f",
          "bytes": "4183f805",
          "mnemonic": "cmp",
          "operands": "r8d, 5"
        },
        {
          "rva": "0x473343",
          "va": "0x212c3773343",
          "bytes": "ba05000000",
          "mnemonic": "mov",
          "operands": "edx, 5"
        },
        {
          "rva": "0x473348",
          "va": "0x212c3773348",
          "bytes": "410f4cd0",
          "mnemonic": "cmovl",
          "operands": "edx, r8d"
        },
        {
          "rva": "0x47334c",
          "va": "0x212c377334c",
          "bytes": "c1e20c",
          "mnemonic": "shl",
          "operands": "edx, 0xc"
        },
        {
          "rva": "0x47334f",
          "va": "0x212c377334f",
          "bytes": "c5d22aca",
          "mnemonic": "vcvtsi2ss",
          "operands": "xmm1, xmm5, edx"
        },
        {
          "rva": "0x47335b",
          "va": "0x212c377335b",
          "bytes": "c5f82ec1",
          "mnemonic": "vucomiss",
          "operands": "xmm0, xmm1"
        },
        {
          "rva": "0x47335f",
          "va": "0x212c377335f",
          "bytes": "7714",
          "mnemonic": "ja",
          "operands": "0x212c3773375"
        },
        {
          "rva": "0x473367",
          "va": "0x212c3773367",
          "bytes": "8b80a0000000",
          "mnemonic": "mov",
          "operands": "eax, dword ptr [rax + 0xa0]"
        },
        {
          "rva": "0x47336d",
          "va": "0x212c377336d",
          "bytes": "3b81a0000000",
          "mnemonic": "cmp",
          "operands": "eax, dword ptr [rcx + 0xa0]"
        },
        {
          "rva": "0x473373",
          "va": "0x212c3773373",
          "bytes": "740a",
          "mnemonic": "je",
          "operands": "0x212c377337f"
        },
        {
          "rva": "0x473375",
          "va": "0x212c3773375",
          "bytes": "c786f050000001000000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0x50f0], 1"
        }
      ]
    },
    {
      "id": "slot_A0_collection_size",
      "function_rva": "0x475830",
      "statement": "slot+A0 is collection-size-like: source count controls copying count*0x20 bytes from pointer +B0, destination capacity +B8, then size is restored. Not an entity/model ID.",
      "sites": [
        {
          "rva": "0x475a95",
          "va": "0x212c3775a95",
          "bytes": "8b9fa0000000",
          "mnemonic": "mov",
          "operands": "ebx, dword ptr [rdi + 0xa0]"
        },
        {
          "rva": "0x475a9b",
          "va": "0x212c3775a9b",
          "bytes": "4c8bb7b0000000",
          "mnemonic": "mov",
          "operands": "r14, qword ptr [rdi + 0xb0]"
        },
        {
          "rva": "0x475aa2",
          "va": "0x212c3775aa2",
          "bytes": "c786a000000000000000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0xa0], 0"
        },
        {
          "rva": "0x475aac",
          "va": "0x212c3775aac",
          "bytes": "4c638eb8000000",
          "mnemonic": "movsxd",
          "operands": "r9, dword ptr [rsi + 0xb8]"
        },
        {
          "rva": "0x475ab3",
          "va": "0x212c3775ab3",
          "bytes": "4139d9",
          "mnemonic": "cmp",
          "operands": "r9d, ebx"
        },
        {
          "rva": "0x475ab8",
          "va": "0x212c3775ab8",
          "bytes": "4c63c3",
          "mnemonic": "movsxd",
          "operands": "r8, ebx"
        },
        {
          "rva": "0x475abf",
          "va": "0x212c3775abf",
          "bytes": "49c1e005",
          "mnemonic": "shl",
          "operands": "r8, 5"
        },
        {
          "rva": "0x475ad0",
          "va": "0x212c3775ad0",
          "bytes": "488b8eb0000000",
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rsi + 0xb0]"
        },
        {
          "rva": "0x475ad7",
          "va": "0x212c3775ad7",
          "bytes": "ff15a3cbb100",
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xb1cba3]"
        },
        {
          "rva": "0x475b30",
          "va": "0x212c3775b30",
          "bytes": "c4817c104406e0",
          "mnemonic": "vmovups",
          "operands": "ymm0, ymmword ptr [r14 + r8 - 0x20]"
        },
        {
          "rva": "0x475b37",
          "va": "0x212c3775b37",
          "bytes": "c4a17c114400e0",
          "mnemonic": "vmovups",
          "operands": "ymmword ptr [rax + r8 - 0x20], ymm0"
        },
        {
          "rva": "0x475b52",
          "va": "0x212c3775b52",
          "bytes": "4839ca",
          "mnemonic": "cmp",
          "operands": "rdx, rcx"
        },
        {
          "rva": "0x475b5c",
          "va": "0x212c3775b5c",
          "bytes": "48c1e105",
          "mnemonic": "shl",
          "operands": "rcx, 5"
        },
        {
          "rva": "0x475b6b",
          "va": "0x212c3775b6b",
          "bytes": "899ea0000000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0xa0], ebx"
        }
      ]
    },
    {
      "id": "payload_capture",
      "function_rva": "0x4721d0",
      "statement": "slot+A0 cleared; helper 0x378C80 receives object+0x5100 and slot+0xA0, then helper 0x473F30 receives slot and object. Capture semantics beyond this remain open.",
      "sites": [
        {
          "rva": "0x472a76",
          "va": "0x212c3772a76",
          "bytes": "4c8db7a0000000",
          "mnemonic": "lea",
          "operands": "r14, [rdi + 0xa0]"
        },
        {
          "rva": "0x472b19",
          "va": "0x212c3772b19",
          "bytes": "41c70600000000",
          "mnemonic": "mov",
          "operands": "dword ptr [r14], 0"
        },
        {
          "rva": "0x47307a",
          "va": "0x212c377307a",
          "bytes": "4c8da600510000",
          "mnemonic": "lea",
          "operands": "r12, [rsi + 0x5100]"
        },
        {
          "rva": "0x473129",
          "va": "0x212c3773129",
          "bytes": "41c7042400000000",
          "mnemonic": "mov",
          "operands": "dword ptr [r12], 0"
        },
        {
          "rva": "0x473134",
          "va": "0x212c3773134",
          "bytes": "e857080000",
          "mnemonic": "call",
          "operands": "0x212c3773990"
        },
        {
          "rva": "0x47313c",
          "va": "0x212c377313c",
          "bytes": "4c89e2",
          "mnemonic": "mov",
          "operands": "rdx, r12"
        },
        {
          "rva": "0x47313f",
          "va": "0x212c377313f",
          "bytes": "4d89f0",
          "mnemonic": "mov",
          "operands": "r8, r14"
        },
        {
          "rva": "0x473142",
          "va": "0x212c3773142",
          "bytes": "e8395bf0ff",
          "mnemonic": "call",
          "operands": "0x212c3678c80"
        },
        {
          "rva": "0x473166",
          "va": "0x212c3773166",
          "bytes": "4889f9",
          "mnemonic": "mov",
          "operands": "rcx, rdi"
        },
        {
          "rva": "0x473169",
          "va": "0x212c3773169",
          "bytes": "4889f2",
          "mnemonic": "mov",
          "operands": "rdx, rsi"
        },
        {
          "rva": "0x47316f",
          "va": "0x212c377316f",
          "bytes": "e8bc0d0000",
          "mnemonic": "call",
          "operands": "0x212c3773f30"
        }
      ]
    },
    {
      "id": "constructor",
      "function_rva": "0x4707f0",
      "statement": "Initialization object stride 0x53C0, zero ring storage and explicitly zero combined count/head qword.",
      "sites": [
        {
          "rva": "0x470830",
          "va": "0x212c3770830",
          "bytes": "41b8c0530000",
          "mnemonic": "mov",
          "operands": "r8d, 0x53c0"
        },
        {
          "rva": "0x47083e",
          "va": "0x212c377083e",
          "bytes": "e88d5b9500",
          "mnemonic": "call",
          "operands": "0x212c40c63d0"
        },
        {
          "rva": "0x47085e",
          "va": "0x212c377085e",
          "bytes": "488d8ef0000000",
          "mnemonic": "lea",
          "operands": "rcx, [rsi + 0xf0]"
        },
        {
          "rva": "0x47088a",
          "va": "0x212c377088a",
          "bytes": "41b810500000",
          "mnemonic": "mov",
          "operands": "r8d, 0x5010"
        },
        {
          "rva": "0x470895",
          "va": "0x212c3770895",
          "bytes": "e8365b9500",
          "mnemonic": "call",
          "operands": "0x212c40c63d0"
        },
        {
          "rva": "0x47089a",
          "va": "0x212c377089a",
          "bytes": "c786f8000000ffffffff",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0xf8], 0xffffffff"
        },
        {
          "rva": "0x471003",
          "va": "0x212c3771003",
          "bytes": "48c786f050000000000000",
          "mnemonic": "mov",
          "operands": "qword ptr [rsi + 0x50f0], 0"
        }
      ]
    },
    {
      "id": "reset",
      "function_rva": "0x471d70",
      "statement": "After call 0x471B90, count/head qword is zeroed; additional state reset. Vtable pointer evidence is separate.",
      "sites": [
        {
          "rva": "0x471d78",
          "va": "0x212c3771d78",
          "bytes": "e813feffff",
          "mnemonic": "call",
          "operands": "0x212c3771b90"
        },
        {
          "rva": "0x471d7d",
          "va": "0x212c3771d7d",
          "bytes": "48c786f050000000000000",
          "mnemonic": "mov",
          "operands": "qword ptr [rsi + 0x50f0], 0"
        },
        {
          "rva": "0x471d88",
          "va": "0x212c3771d88",
          "bytes": "c746580d000000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0x58], 0xd"
        },
        {
          "rva": "0x471d8f",
          "va": "0x212c3771d8f",
          "bytes": "c7465000000000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0x50], 0"
        },
        {
          "rva": "0x471d96",
          "va": "0x212c3771d96",
          "bytes": "c6465400",
          "mnemonic": "mov",
          "operands": "byte ptr [rsi + 0x54], 0"
        }
      ]
    },
    {
      "id": "reset_inlined",
      "function_rva": "0x6ce200",
      "statement": "Same cleanup + count/head zero sequence appears inline in another function.",
      "sites": [
        {
          "rva": "0x6ce2f1",
          "va": "0x212c39ce2f1",
          "bytes": "4889f9",
          "mnemonic": "mov",
          "operands": "rcx, rdi"
        },
        {
          "rva": "0x6ce2f4",
          "va": "0x212c39ce2f4",
          "bytes": "e89738daff",
          "mnemonic": "call",
          "operands": "0x212c3771b90"
        },
        {
          "rva": "0x6ce2f9",
          "va": "0x212c39ce2f9",
          "bytes": "48c787f050000000000000",
          "mnemonic": "mov",
          "operands": "qword ptr [rdi + 0x50f0], 0"
        },
        {
          "rva": "0x6ce304",
          "va": "0x212c39ce304",
          "bytes": "c747580d000000",
          "mnemonic": "mov",
          "operands": "dword ptr [rdi + 0x58], 0xd"
        }
      ]
    },
    {
      "id": "ring_move",
      "function_rva": "0x6ac690",
      "statement": "16 iterations copy the slot prefixes and transfer dynamic buffers; then copy combined count/head. This is state movement, not a new time sample.",
      "sites": [
        {
          "rva": "0x6ae516",
          "va": "0x212c39ae516",
          "bytes": "4531ed",
          "mnemonic": "xor",
          "operands": "r13d, r13d"
        },
        {
          "rva": "0x6ae520",
          "va": "0x212c39ae520",
          "bytes": "c4a17c10842ff0000000",
          "mnemonic": "vmovups",
          "operands": "ymm0, ymmword ptr [rdi + r13 + 0xf0]"
        },
        {
          "rva": "0x6ae52a",
          "va": "0x212c39ae52a",
          "bytes": "c4a17c108c2f10010000",
          "mnemonic": "vmovups",
          "operands": "ymm1, ymmword ptr [rdi + r13 + 0x110]"
        },
        {
          "rva": "0x6ae534",
          "va": "0x212c39ae534",
          "bytes": "c4a17c10942f30010000",
          "mnemonic": "vmovups",
          "operands": "ymm2, ymmword ptr [rdi + r13 + 0x130]"
        },
        {
          "rva": "0x6ae53e",
          "va": "0x212c39ae53e",
          "bytes": "c4a17c109c2f50010000",
          "mnemonic": "vmovups",
          "operands": "ymm3, ymmword ptr [rdi + r13 + 0x150]"
        },
        {
          "rva": "0x6ae548",
          "va": "0x212c39ae548",
          "bytes": "c4817c119c2c50010000",
          "mnemonic": "vmovups",
          "operands": "ymmword ptr [r12 + r13 + 0x150], ymm3"
        },
        {
          "rva": "0x6ae552",
          "va": "0x212c39ae552",
          "bytes": "c4817c11942c30010000",
          "mnemonic": "vmovups",
          "operands": "ymmword ptr [r12 + r13 + 0x130], ymm2"
        },
        {
          "rva": "0x6ae55c",
          "va": "0x212c39ae55c",
          "bytes": "c4817c118c2c10010000",
          "mnemonic": "vmovups",
          "operands": "ymmword ptr [r12 + r13 + 0x110], ymm1"
        },
        {
          "rva": "0x6ae566",
          "va": "0x212c39ae566",
          "bytes": "c4817c11842cf0000000",
          "mnemonic": "vmovups",
          "operands": "ymmword ptr [r12 + r13 + 0xf0], ymm0"
        },
        {
          "rva": "0x6ae570",
          "va": "0x212c39ae570",
          "bytes": "4a8b842f80010000",
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rdi + r13 + 0x180]"
        },
        {
          "rva": "0x6ae578",
          "va": "0x212c39ae578",
          "bytes": "4ac7842f8001000000000000",
          "mnemonic": "mov",
          "operands": "qword ptr [rdi + r13 + 0x180], 0"
        },
        {
          "rva": "0x6ae634",
          "va": "0x212c39ae634",
          "bytes": "4981c500050000",
          "mnemonic": "add",
          "operands": "r13, 0x500"
        },
        {
          "rva": "0x6ae63b",
          "va": "0x212c39ae63b",
          "bytes": "4981fd00500000",
          "mnemonic": "cmp",
          "operands": "r13, 0x5000"
        },
        {
          "rva": "0x6ae642",
          "va": "0x212c39ae642",
          "bytes": "0f85d8feffff",
          "mnemonic": "jne",
          "operands": "0x212c39ae520"
        },
        {
          "rva": "0x6ae648",
          "va": "0x212c39ae648",
          "bytes": "488b87f0500000",
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rdi + 0x50f0]"
        },
        {
          "rva": "0x6ae64f",
          "va": "0x212c39ae64f",
          "bytes": "49898424f0500000",
          "mnemonic": "mov",
          "operands": "qword ptr [r12 + 0x50f0], rax"
        }
      ]
    },
    {
      "id": "worker_dispatch",
      "function_rva": "0x475ef0",
      "statement": "Atomic work-item index at +0x40; each item calls 0x4721D0. Worker function pointer stored at RVA 0xEDFA70.",
      "sites": [
        {
          "rva": "0x475ef6",
          "va": "0x212c3775ef6",
          "bytes": "8b7908",
          "mnemonic": "mov",
          "operands": "edi, dword ptr [rcx + 8]"
        },
        {
          "rva": "0x475efe",
          "va": "0x212c3775efe",
          "bytes": "f00fc14140",
          "mnemonic": "lock xadd",
          "operands": "dword ptr [rcx + 0x40], eax"
        },
        {
          "rva": "0x475f03",
          "va": "0x212c3775f03",
          "bytes": "39f8",
          "mnemonic": "cmp",
          "operands": "eax, edi"
        },
        {
          "rva": "0x475f05",
          "va": "0x212c3775f05",
          "bytes": "732d",
          "mnemonic": "jae",
          "operands": "0x212c3775f34"
        },
        {
          "rva": "0x475f10",
          "va": "0x212c3775f10",
          "bytes": "488b4e10",
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rsi + 0x10]"
        },
        {
          "rva": "0x475f14",
          "va": "0x212c3775f14",
          "bytes": "488b4908",
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rcx + 8]"
        },
        {
          "rva": "0x475f18",
          "va": "0x212c3775f18",
          "bytes": "488b09",
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rcx]"
        },
        {
          "rva": "0x475f1d",
          "va": "0x212c3775f1d",
          "bytes": "488b0cc1",
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rcx + rax*8]"
        },
        {
          "rva": "0x475f21",
          "va": "0x212c3775f21",
          "bytes": "e8aac2ffff",
          "mnemonic": "call",
          "operands": "0x212c37721d0"
        },
        {
          "rva": "0x475f2b",
          "va": "0x212c3775f2b",
          "bytes": "f00fc14640",
          "mnemonic": "lock xadd",
          "operands": "dword ptr [rsi + 0x40], eax"
        }
      ]
    },
    {
      "id": "additional_caller",
      "function_rva": "0x661010",
      "statement": "Refresh object fields, clear scratch counters +C0/+E0, then call the producer; call absent from original linear index.",
      "sites": [
        {
          "rva": "0x661b57",
          "va": "0x212c3961b57",
          "bytes": "c5fa100401",
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rcx + rax]"
        },
        {
          "rva": "0x661b5c",
          "va": "0x212c3961b5c",
          "bytes": "c5fa114648",
          "mnemonic": "vmovss",
          "operands": "dword ptr [rsi + 0x48], xmm0"
        },
        {
          "rva": "0x661b91",
          "va": "0x212c3961b91",
          "bytes": "c786c000000000000000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0xc0], 0"
        },
        {
          "rva": "0x661b9b",
          "va": "0x212c3961b9b",
          "bytes": "c786e000000000000000",
          "mnemonic": "mov",
          "operands": "dword ptr [rsi + 0xe0], 0"
        },
        {
          "rva": "0x661ba5",
          "va": "0x212c3961ba5",
          "bytes": "4889f1",
          "mnemonic": "mov",
          "operands": "rcx, rsi"
        },
        {
          "rva": "0x661ba8",
          "va": "0x212c3961ba8",
          "bytes": "e82306e1ff",
          "mnemonic": "call",
          "operands": "0x212c37721d0"
        }
      ]
    },
    {
      "id": "controller_dispatch",
      "function_rva": "0x6ac690",
      "statement": "One item calls producer directly; multiple items construct worker object with vtable at EDFA68 and submit through an indirect scheduler call.",
      "sites": [
        {
          "rva": "0x6af8d9",
          "va": "0x212c39af8d9",
          "bytes": "83fe01",
          "mnemonic": "cmp",
          "operands": "esi, 1"
        },
        {
          "rva": "0x6af8dc",
          "va": "0x212c39af8dc",
          "bytes": "7479",
          "mnemonic": "je",
          "operands": "0x212c39af957"
        },
        {
          "rva": "0x6af8e6",
          "va": "0x212c39af8e6",
          "bytes": "488d057b018300",
          "mnemonic": "lea",
          "operands": "rax, [rip + 0x83017b]"
        },
        {
          "rva": "0x6af8ed",
          "va": "0x212c39af8ed",
          "bytes": "48898380010000",
          "mnemonic": "mov",
          "operands": "qword ptr [rbx + 0x180], rax"
        },
        {
          "rva": "0x6af916",
          "va": "0x212c39af916",
          "bytes": "c783c001000000000000",
          "mnemonic": "mov",
          "operands": "dword ptr [rbx + 0x1c0], 0"
        },
        {
          "rva": "0x6af934",
          "va": "0x212c39af934",
          "bytes": "488b8098000000",
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rax + 0x98]"
        },
        {
          "rva": "0x6af953",
          "va": "0x212c39af953",
          "bytes": "ffd0",
          "mnemonic": "call",
          "operands": "rax"
        },
        {
          "rva": "0x6af957",
          "va": "0x212c39af957",
          "bytes": "488b08",
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rax]"
        },
        {
          "rva": "0x6af95d",
          "va": "0x212c39af95d",
          "bytes": "e86e28dcff",
          "mnemonic": "call",
          "operands": "0x212c37721d0"
        },
        {
          "rva": "0x6af992",
          "va": "0x212c39af992",
          "bytes": "ffd0",
          "mnemonic": "call",
          "operands": "rax"
        }
      ]
    },
    {
      "id": "copy_record",
      "function_rva": "0x475830",
      "statement": "Copies scalar prefix including tick/fraction/position/angles, then independently handles owned buffers. Not a raw memcpy of the whole slot.",
      "sites": [
        {
          "rva": "0x47584d",
          "va": "0x212c377584d",
          "bytes": "4889d7",
          "mnemonic": "mov",
          "operands": "rdi, rdx"
        },
        {
          "rva": "0x475850",
          "va": "0x212c3775850",
          "bytes": "4889ce",
          "mnemonic": "mov",
          "operands": "rsi, rcx"
        },
        {
          "rva": "0x475853",
          "va": "0x212c3775853",
          "bytes": "c5fc1002",
          "mnemonic": "vmovups",
          "operands": "ymm0, ymmword ptr [rdx]"
        },
        {
          "rva": "0x475857",
          "va": "0x212c3775857",
          "bytes": "c5fc104a20",
          "mnemonic": "vmovups",
          "operands": "ymm1, ymmword ptr [rdx + 0x20]"
        },
        {
          "rva": "0x47585c",
          "va": "0x212c377585c",
          "bytes": "c5fc105240",
          "mnemonic": "vmovups",
          "operands": "ymm2, ymmword ptr [rdx + 0x40]"
        },
        {
          "rva": "0x475861",
          "va": "0x212c3775861",
          "bytes": "c5fc105a60",
          "mnemonic": "vmovups",
          "operands": "ymm3, ymmword ptr [rdx + 0x60]"
        },
        {
          "rva": "0x475866",
          "va": "0x212c3775866",
          "bytes": "c5fc115960",
          "mnemonic": "vmovups",
          "operands": "ymmword ptr [rcx + 0x60], ymm3"
        },
        {
          "rva": "0x47586b",
          "va": "0x212c377586b",
          "bytes": "c5fc115140",
          "mnemonic": "vmovups",
          "operands": "ymmword ptr [rcx + 0x40], ymm2"
        },
        {
          "rva": "0x475870",
          "va": "0x212c3775870",
          "bytes": "c5fc114920",
          "mnemonic": "vmovups",
          "operands": "ymmword ptr [rcx + 0x20], ymm1"
        },
        {
          "rva": "0x475875",
          "va": "0x212c3775875",
          "bytes": "c5fc1101",
          "mnemonic": "vmovups",
          "operands": "ymmword ptr [rcx], ymm0"
        },
        {
          "rva": "0x475882",
          "va": "0x212c3775882",
          "bytes": "4c8bb780000000",
          "mnemonic": "mov",
          "operands": "r14, qword ptr [rdi + 0x80]"
        }
      ]
    },
    {
      "id": "derived_record_not_ring_insert",
      "function_rva": "0x474d20",
      "statement": "Requires nonempty ring, target time later than newest, scratch counter<=31. Copies newest to object+A8 scratch vector, writes target pos/time and byte+C=1. Not a ring producer.",
      "sites": [
        {
          "rva": "0x474d32",
          "va": "0x212c3774d32",
          "bytes": "83b9f050000000",
          "mnemonic": "cmp",
          "operands": "dword ptr [rcx + 0x50f0], 0"
        },
        {
          "rva": "0x474d39",
          "va": "0x212c3774d39",
          "bytes": "0f842e040000",
          "mnemonic": "je",
          "operands": "0x212c377516d"
        },
        {
          "rva": "0x474d45",
          "va": "0x212c3774d45",
          "bytes": "8b81f4500000",
          "mnemonic": "mov",
          "operands": "eax, dword ptr [rcx + 0x50f4]"
        },
        {
          "rva": "0x474d66",
          "va": "0x212c3774d66",
          "bytes": "4881c3f0000000",
          "mnemonic": "add",
          "operands": "rbx, 0xf0"
        },
        {
          "rva": "0x474d6d",
          "va": "0x212c3774d6d",
          "bytes": "8baaf4010000",
          "mnemonic": "mov",
          "operands": "ebp, dword ptr [rdx + 0x1f4]"
        },
        {
          "rva": "0x474d73",
          "va": "0x212c3774d73",
          "bytes": "c5fa108200020000",
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rdx + 0x200]"
        },
        {
          "rva": "0x474e25",
          "va": "0x212c3774e25",
          "bytes": "84c0",
          "mnemonic": "test",
          "operands": "al, al"
        },
        {
          "rva": "0x474e27",
          "va": "0x212c3774e27",
          "bytes": "0f8940030000",
          "mnemonic": "jns",
          "operands": "0x212c377516d"
        },
        {
          "rva": "0x474e2d",
          "va": "0x212c3774e2d",
          "bytes": "8b87c0000000",
          "mnemonic": "mov",
          "operands": "eax, dword ptr [rdi + 0xc0]"
        },
        {
          "rva": "0x474e33",
          "va": "0x212c3774e33",
          "bytes": "83f81f",
          "mnemonic": "cmp",
          "operands": "eax, 0x1f"
        },
        {
          "rva": "0x474e36",
          "va": "0x212c3774e36",
          "bytes": "0f8f31030000",
          "mnemonic": "jg",
          "operands": "0x212c377516d"
        },
        {
          "rva": "0x474f48",
          "va": "0x212c3774f48",
          "bytes": "898fc0000000",
          "mnemonic": "mov",
          "operands": "dword ptr [rdi + 0xc0], ecx"
        },
        {
          "rva": "0x474f56",
          "va": "0x212c3774f56",
          "bytes": "48038fa8000000",
          "mnemonic": "add",
          "operands": "rcx, qword ptr [rdi + 0xa8]"
        },
        {
          "rva": "0x474f6c",
          "va": "0x212c3774f6c",
          "bytes": "e8bf080000",
          "mnemonic": "call",
          "operands": "0x212c3775830"
        },
        {
          "rva": "0x475105",
          "va": "0x212c3775105",
          "bytes": "89481c",
          "mnemonic": "mov",
          "operands": "dword ptr [rax + 0x1c], ecx"
        },
        {
          "rva": "0x47510b",
          "va": "0x212c377510b",
          "bytes": "48894814",
          "mnemonic": "mov",
          "operands": "qword ptr [rax + 0x14], rcx"
        },
        {
          "rva": "0x47519a",
          "va": "0x212c377519a",
          "bytes": "8918",
          "mnemonic": "mov",
          "operands": "dword ptr [rax], ebx"
        },
        {
          "rva": "0x47519c",
          "va": "0x212c377519c",
          "bytes": "c5fa114004",
          "mnemonic": "vmovss",
          "operands": "dword ptr [rax + 4], xmm0"
        },
        {
          "rva": "0x4751a1",
          "va": "0x212c37751a1",
          "bytes": "c6400c01",
          "mnemonic": "mov",
          "operands": "byte ptr [rax + 0xc], 1"
        }
      ]
    },
    {
      "id": "consumer_admission",
      "function_rva": "0x52b250",
      "statement": "Known consumer: distance2 <=62500, signed trunc(abs(time_delta_sec)*1000)<=200, inclusive time bounds, then direction split. Time argument at +C is int32, not float.",
      "sites": [
        {
          "rva": "0x52b31c",
          "va": "0x212c382b31c",
          "bytes": "8baff0500000",
          "mnemonic": "mov",
          "operands": "ebp, dword ptr [rdi + 0x50f0]"
        },
        {
          "rva": "0x52b38c",
          "va": "0x212c382b38c",
          "bytes": "8b87f4500000",
          "mnemonic": "mov",
          "operands": "eax, dword ptr [rdi + 0x50f4]"
        },
        {
          "rva": "0x52b4b1",
          "va": "0x212c382b4b1",
          "bytes": "c5782e3d97069700",
          "mnemonic": "vucomiss",
          "operands": "xmm15, dword ptr [rip + 0x970697]"
        },
        {
          "rva": "0x52b4de",
          "va": "0x212c382b4de",
          "bytes": "448b6e0c",
          "mnemonic": "mov",
          "operands": "r13d, dword ptr [rsi + 0xc]"
        },
        {
          "rva": "0x52b4e5",
          "va": "0x212c382b4e5",
          "bytes": "2b4810",
          "mnemonic": "sub",
          "operands": "ecx, dword ptr [rax + 0x10]"
        },
        {
          "rva": "0x52b4e8",
          "va": "0x212c382b4e8",
          "bytes": "c59a5c4014",
          "mnemonic": "vsubss",
          "operands": "xmm0, xmm12, dword ptr [rax + 0x14]"
        },
        {
          "rva": "0x52b533",
          "va": "0x212c382b533",
          "bytes": "c5fa59c2",
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, xmm2"
        },
        {
          "rva": "0x52b537",
          "va": "0x212c382b537",
          "bytes": "c5f259ca",
          "mnemonic": "vmulss",
          "operands": "xmm1, xmm1, xmm2"
        },
        {
          "rva": "0x52b53f",
          "va": "0x212c382b53f",
          "bytes": "c5f8540579188c00",
          "mnemonic": "vandps",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x8c1879]"
        },
        {
          "rva": "0x52b547",
          "va": "0x212c382b547",
          "bytes": "c5fa5905f5f69600",
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x96f6f5]"
        },
        {
          "rva": "0x52b54f",
          "va": "0x212c382b54f",
          "bytes": "c5fa2cc8",
          "mnemonic": "vcvttss2si",
          "operands": "ecx, xmm0"
        },
        {
          "rva": "0x52b553",
          "va": "0x212c382b553",
          "bytes": "81f9c8000000",
          "mnemonic": "cmp",
          "operands": "ecx, 0xc8"
        },
        {
          "rva": "0x52b559",
          "va": "0x212c382b559",
          "bytes": "0f8f21feffff",
          "mnemonic": "jg",
          "operands": "0x212c382b380"
        },
        {
          "rva": "0x52b567",
          "va": "0x212c382b567",
          "bytes": "c5fa594004",
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rax + 4]"
        },
        {
          "rva": "0x52b618",
          "va": "0x212c382b618",
          "bytes": "0f8962fdffff",
          "mnemonic": "jns",
          "operands": "0x212c382b380"
        },
        {
          "rva": "0x52b652",
          "va": "0x212c382b652",
          "bytes": "0f8928fdffff",
          "mnemonic": "jns",
          "operands": "0x212c382b380"
        },
        {
          "rva": "0x52b664",
          "va": "0x212c382b664",
          "bytes": "0f8916fdffff",
          "mnemonic": "jns",
          "operands": "0x212c382b380"
        },
        {
          "rva": "0x52b677",
          "va": "0x212c382b677",
          "bytes": "c5f82e0539f79600",
          "mnemonic": "vucomiss",
          "operands": "xmm0, dword ptr [rip + 0x96f739]"
        }
      ]
    },
    {
      "id": "aging_window_source",
      "function_rva": "0x473490",
      "statement": "Guarded helper returns scalar at [[opaque_root]+8]+0x58; parameter/cvar name and runtime value not resolved.",
      "sites": [
        {
          "rva": "0x4734a3",
          "va": "0x212c37734a3",
          "bytes": "65488b042560000000",
          "mnemonic": "mov",
          "operands": "rax, qword ptr gs:[0x60]"
        },
        {
          "rva": "0x47350a",
          "va": "0x212c377350a",
          "bytes": "4839542428",
          "mnemonic": "cmp",
          "operands": "qword ptr [rsp + 0x28], rdx"
        },
        {
          "rva": "0x47350f",
          "va": "0x212c377350f",
          "bytes": "0f8491000000",
          "mnemonic": "je",
          "operands": "0x212c37735a6"
        },
        {
          "rva": "0x4738a0",
          "va": "0x212c37738a0",
          "bytes": "48394c2428",
          "mnemonic": "cmp",
          "operands": "qword ptr [rsp + 0x28], rcx"
        },
        {
          "rva": "0x4738a5",
          "va": "0x212c37738a5",
          "bytes": "7475",
          "mnemonic": "je",
          "operands": "0x212c377391c"
        },
        {
          "rva": "0x473952",
          "va": "0x212c3773952",
          "bytes": "488b442428",
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rsp + 0x28]"
        },
        {
          "rva": "0x473957",
          "va": "0x212c3773957",
          "bytes": "488b4008",
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rax + 8]"
        },
        {
          "rva": "0x47395b",
          "va": "0x212c377395b",
          "bytes": "c5fa104058",
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rax + 0x58]"
        },
        {
          "rva": "0x473978",
          "va": "0x212c3773978",
          "bytes": "c3",
          "mnemonic": "ret",
          "operands": ""
        }
      ]
    }
  ],
  "constants": [
    {
      "rva": "0xe58838",
      "bytes": "00008042",
      "uint32": "0x42800000",
      "float32": 64.0,
      "interpretation": "64.0 ticks/second"
    },
    {
      "rva": "0xdec7d8",
      "bytes": "0000003f",
      "uint32": "0x3f000000",
      "float32": 0.5,
      "interpretation": "0.5 rounding offset"
    },
    {
      "rva": "0xdec898",
      "bytes": "0000803f",
      "uint32": "0x3f800000",
      "float32": 1.0,
      "interpretation": "1.0"
    },
    {
      "rva": "0xe06abc",
      "bytes": "000080bf",
      "uint32": "0xbf800000",
      "float32": -1.0,
      "interpretation": "-1.0 borrow"
    },
    {
      "rva": "0xe9bb50",
      "bytes": "00247447",
      "uint32": "0x47742400",
      "float32": 62500.0,
      "interpretation": "62500.0 distance squared"
    },
    {
      "rva": "0xe98ac0",
      "bytes": "0000803c",
      "uint32": "0x3c800000",
      "float32": 0.015625,
      "interpretation": "1/64 seconds/tick"
    },
    {
      "rva": "0xe9ac44",
      "bytes": "00007a44",
      "uint32": "0x447a0000",
      "float32": 1000.0,
      "interpretation": "1000.0 milliseconds/second"
    },
    {
      "rva": "0xe9adb8",
      "bytes": "3333f33e",
      "uint32": "0x3ef33333",
      "float32": 0.4749999940395355,
      "interpretation": "direction split 0.475"
    },
    {
      "rva": "0x17701f0",
      "bytes": "37133713",
      "uint32": "0x13371337",
      "float32": 2.3107320954267666e-27,
      "interpretation": "unresolved dynamic source-time offset sentinel"
    },
    {
      "rva": "0x177035c",
      "bytes": "37133713",
      "uint32": "0x13371337",
      "float32": 2.3107320954267666e-27,
      "interpretation": "unresolved entity-component offset sentinel"
    },
    {
      "rva": "0x176f8c8",
      "bytes": "37133713",
      "uint32": "0x13371337",
      "float32": 2.3107320954267666e-27,
      "interpretation": "unresolved position offset sentinel"
    }
  ],
  "raw_branch_refs": [
    {
      "source_rva": "0x471d78",
      "target_rva": "0x471b90",
      "owner_rva": "0x471d70",
      "bytes": "e813feffff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": true,
      "beyond_sqlite_linear_end": false
    },
    {
      "source_rva": "0x47221c",
      "target_rva": "0x473490",
      "owner_rva": "0x4721d0",
      "bytes": "e86f120000",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": true,
      "beyond_sqlite_linear_end": false
    },
    {
      "source_rva": "0x475f21",
      "target_rva": "0x4721d0",
      "owner_rva": "0x475ef0",
      "bytes": "e8aac2ffff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": true,
      "beyond_sqlite_linear_end": false
    },
    {
      "source_rva": "0x476a76",
      "target_rva": "0x473490",
      "owner_rva": "0x4764d0",
      "bytes": "e815caffff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": false,
      "beyond_sqlite_linear_end": true
    },
    {
      "source_rva": "0x52dfb5",
      "target_rva": "0x474d20",
      "owner_rva": "0x52dd20",
      "bytes": "e8666df4ff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": true,
      "beyond_sqlite_linear_end": false
    },
    {
      "source_rva": "0x52f3ca",
      "target_rva": "0x474d20",
      "owner_rva": "0x52f030",
      "bytes": "e85159f4ff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": true,
      "beyond_sqlite_linear_end": false
    },
    {
      "source_rva": "0x661ba8",
      "target_rva": "0x4721d0",
      "owner_rva": "0x661010",
      "bytes": "e82306e1ff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": false,
      "beyond_sqlite_linear_end": true
    },
    {
      "source_rva": "0x6adc02",
      "target_rva": "0x473490",
      "owner_rva": "0x6ac690",
      "bytes": "e88958dcff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": false,
      "beyond_sqlite_linear_end": true
    },
    {
      "source_rva": "0x6ae203",
      "target_rva": "0x471b90",
      "owner_rva": "0x6ac690",
      "bytes": "e88839dcff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": false,
      "beyond_sqlite_linear_end": true
    },
    {
      "source_rva": "0x6ae325",
      "target_rva": "0x4707f0",
      "owner_rva": "0x6ac690",
      "bytes": "e8c624dcff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": false,
      "beyond_sqlite_linear_end": true
    },
    {
      "source_rva": "0x6ae795",
      "target_rva": "0x4707f0",
      "owner_rva": "0x6ac690",
      "bytes": "e85620dcff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": false,
      "beyond_sqlite_linear_end": true
    },
    {
      "source_rva": "0x6af95d",
      "target_rva": "0x4721d0",
      "owner_rva": "0x6ac690",
      "bytes": "e86e28dcff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": false,
      "beyond_sqlite_linear_end": true
    },
    {
      "source_rva": "0x6b5013",
      "target_rva": "0x471b90",
      "owner_rva": "0x6b4f40",
      "bytes": "e878cbdbff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": true,
      "beyond_sqlite_linear_end": false
    },
    {
      "source_rva": "0x6ce2f4",
      "target_rva": "0x471b90",
      "owner_rva": "0x6ce200",
      "bytes": "e89738daff",
      "opcode": "e8",
      "entry_cfg_boundary": true,
      "present_in_sqlite_refs": true,
      "beyond_sqlite_linear_end": false
    }
  ],
  "raw_function_pointers": [
    {
      "storage_rva": "0xedfb10",
      "target_rva": "0x471b90",
      "bytes": "901b77c312020000",
      "meaning": "raw pointer, not proof of executed call"
    },
    {
      "storage_rva": "0xedfb18",
      "target_rva": "0x471d70",
      "bytes": "701d77c312020000",
      "meaning": "raw pointer, not proof of executed call"
    },
    {
      "storage_rva": "0xedfa70",
      "target_rva": "0x475ef0",
      "bytes": "f05e77c312020000",
      "meaning": "raw pointer, not proof of executed call"
    }
  ]
}
```
