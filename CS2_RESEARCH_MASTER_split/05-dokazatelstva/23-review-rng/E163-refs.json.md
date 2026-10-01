<!-- split-part | CS2_RESEARCH_MASTER.md lines 85567-94214 | body-sha256 775cb1a60b6c1882f441102a0bd319a5deca1965b59b72a4a69ff28621286d0c -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-163"></a>

## E163. `analysis/review_rng/refs.json`

Bytes: 224365. SHA-256: `5e8fe285cebdff12d40a1cdf188d4eb66130de57c7618e367d1b98c0d99c97d9`.

```json
{
  "base": "0x212c3300000",
  "dump_size": 83890176,
  "sqlite_addresses": "RVA",
  "source_sha256": {
    "analysis/decompiled_rng/212c35e9b50.c": "708754564f466fe4251a835a18b67d5032040b2e1be79d3c5ad7ce0ca2f8ed32",
    "analysis/decompiled_rng/212c3706c10.c": "9f441e701bdb395e9120591414fb80dda129a6e7d19a004e445207abbab0a94c",
    "analysis/decompiled_rng/212c3722970.c": "8700860f82031b986b1dfb66a810b54519d7d91976a44d452a92bed38ce276c6",
    "analysis/decompiled_rng/212c37fd710.c": "409d6df45223f568805c292e890a43790b808de0e5d17e52891d8c78f17475f0",
    "analysis/decompiled_rng/212c380fb10.c": "d5ca769b7ee749849119e2797e1db97bdf747cef0d3700e614cd53f88e5f6b04",
    "analysis/decompiled_rng/212c384bc60.c": "138af0532d0a527e0a416ff55280a842123f9b2251060f772ac72cc4f43be01f",
    "analysis/decompiled_rng/212c3a1ebe0.c": "0d355c4672afe94f675afa4646e83eb11c9dd29c25f2caefa7c197d980b95341",
    "analysis/decompiled_rng/212c82f1690.c": "7af29772d6c35ec30267744d3186b350b383dbabef69bf930c4d9f2dcba7b30e"
  },
  "db_mode": "mode=ro&immutable=1; query_only=ON",
  "functions": [
    {
      "va": "0x212c35e9b50",
      "rva": "0x2e9b50",
      "metadata": {
        "begin": 3054416,
        "end": 3058014,
        "unwind": 17009120,
        "table_rva": 66495988,
        "decoded_end": 3058014,
        "instruction_count": 749
      },
      "refs_truncated": false,
      "refs": [
        {
          "source": 3054535,
          "target": 3058512,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35eab50",
          "kind": "branch"
        },
        {
          "source": 3054545,
          "target": 14714456,
          "owner": 3054416,
          "mnemonic": "vmulss",
          "operands": "xmm8, xmm0, dword ptr [rip + 0xb1ea7f]",
          "kind": "rip"
        },
        {
          "source": 3054557,
          "target": 14265312,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c409abe0",
          "kind": "branch"
        },
        {
          "source": 3054570,
          "target": 14232416,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c4092b60",
          "kind": "branch"
        },
        {
          "source": 3054583,
          "target": 14265312,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c409abe0",
          "kind": "branch"
        },
        {
          "source": 3054595,
          "target": 3054783,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35e9cbf",
          "kind": "branch"
        },
        {
          "source": 3054618,
          "target": 16328280,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0xca8a37]",
          "kind": "rip"
        },
        {
          "source": 3054625,
          "target": 14953960,
          "owner": 3054416,
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rip + 0xb591bf]",
          "kind": "rip"
        },
        {
          "source": 3054633,
          "target": 15305632,
          "owner": 3054416,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0xbaef6f]",
          "kind": "rip"
        },
        {
          "source": 3054643,
          "target": 15305636,
          "owner": 3054416,
          "mnemonic": "vmulss",
          "operands": "xmm9, xmm0, dword ptr [rip + 0xbaef69]",
          "kind": "rip"
        },
        {
          "source": 3054660,
          "target": 15305640,
          "owner": 3054416,
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rip + 0xbaef5c]",
          "kind": "rip"
        },
        {
          "source": 3054668,
          "target": 15305644,
          "owner": 3054416,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0xbaef58]",
          "kind": "rip"
        },
        {
          "source": 3054733,
          "target": 16328288,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xca89cd]",
          "kind": "rip"
        },
        {
          "source": 3054751,
          "target": 14601712,
          "owner": 3054416,
          "mnemonic": "vandps",
          "operands": "xmm2, xmm0, xmmword ptr [rip + 0xb03149]",
          "kind": "rip"
        },
        {
          "source": 3054778,
          "target": 3055159,
          "owner": 3054416,
          "mnemonic": "jmp",
          "operands": "0x212c35e9e37",
          "kind": "branch"
        },
        {
          "source": 3054783,
          "target": 14601712,
          "owner": 3054416,
          "mnemonic": "vxorps",
          "operands": "xmm9, xmm8, xmmword ptr [rip + 0xb03129]",
          "kind": "rip"
        },
        {
          "source": 3054791,
          "target": 24572792,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x14856aa]",
          "kind": "rip"
        },
        {
          "source": 3054801,
          "target": 3054822,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35e9ce6",
          "kind": "branch"
        },
        {
          "source": 3054803,
          "target": 24572784,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1485696]",
          "kind": "rip"
        },
        {
          "source": 3054810,
          "target": 2899168,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35c3ce0",
          "kind": "branch"
        },
        {
          "source": 3054815,
          "target": 24572792,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1485692]",
          "kind": "rip"
        },
        {
          "source": 3054832,
          "target": 16328288,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0xca8969]",
          "kind": "rip"
        },
        {
          "source": 3054867,
          "target": 15233280,
          "owner": 3054416,
          "mnemonic": "vbroadcastss",
          "operands": "xmm10, dword ptr [rip + 0xb9d3e4]",
          "kind": "rip"
        },
        {
          "source": 3054890,
          "target": 16328280,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "r15, qword ptr [rip + 0xca8927]",
          "kind": "rip"
        },
        {
          "source": 3054897,
          "target": 15305640,
          "owner": 3054416,
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rip + 0xbaee6f]",
          "kind": "rip"
        },
        {
          "source": 3054905,
          "target": 15305644,
          "owner": 3054416,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0xbaee6b]",
          "kind": "rip"
        },
        {
          "source": 3054980,
          "target": 15305648,
          "owner": 3054416,
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rip + 0xbaee24]",
          "kind": "rip"
        },
        {
          "source": 3054988,
          "target": 15305652,
          "owner": 3054416,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0xbaee20]",
          "kind": "rip"
        },
        {
          "source": 3054999,
          "target": 15305656,
          "owner": 3054416,
          "mnemonic": "vmulss",
          "operands": "xmm13, xmm0, dword ptr [rip + 0xbaee19]",
          "kind": "rip"
        },
        {
          "source": 3055018,
          "target": 3055025,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35e9db1",
          "kind": "branch"
        },
        {
          "source": 3055043,
          "target": 14953960,
          "owner": 3054416,
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rip + 0xb5901d]",
          "kind": "rip"
        },
        {
          "source": 3055051,
          "target": 15305632,
          "owner": 3054416,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0xbaedcd]",
          "kind": "rip"
        },
        {
          "source": 3055062,
          "target": 15305636,
          "owner": 3054416,
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0xbaedc6]",
          "kind": "rip"
        },
        {
          "source": 3055164,
          "target": 24472624,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0x146cded]",
          "kind": "rip"
        },
        {
          "source": 3055174,
          "target": 3055191,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35e9e57",
          "kind": "branch"
        },
        {
          "source": 3055184,
          "target": 3055224,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35e9e78",
          "kind": "branch"
        },
        {
          "source": 3055186,
          "target": 3057156,
          "owner": 3054416,
          "mnemonic": "jmp",
          "operands": "0x212c35ea604",
          "kind": "branch"
        },
        {
          "source": 3055191,
          "target": 24472616,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x146cdca]",
          "kind": "rip"
        },
        {
          "source": 3055198,
          "target": 3035088,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35e4fd0",
          "kind": "branch"
        },
        {
          "source": 3055203,
          "target": 24472624,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0x146cdc6]",
          "kind": "rip"
        },
        {
          "source": 3055218,
          "target": 3057156,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea604",
          "kind": "branch"
        },
        {
          "source": 3055234,
          "target": 3057156,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea604",
          "kind": "branch"
        },
        {
          "source": 3055244,
          "target": 3057156,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea604",
          "kind": "branch"
        },
        {
          "source": 3055260,
          "target": 6589872,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3948db0",
          "kind": "branch"
        },
        {
          "source": 3055267,
          "target": 3057156,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea604",
          "kind": "branch"
        },
        {
          "source": 3055279,
          "target": 3055291,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35e9ebb",
          "kind": "branch"
        },
        {
          "source": 3055285,
          "target": 3057156,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea604",
          "kind": "branch"
        },
        {
          "source": 3055291,
          "target": 24545512,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x147ea26]",
          "kind": "rip"
        },
        {
          "source": 3055328,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "r14, qword ptr [rip + 0x146dd71]",
          "kind": "rip"
        },
        {
          "source": 3055338,
          "target": 3055359,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35e9eff",
          "kind": "branch"
        },
        {
          "source": 3055340,
          "target": 24476752,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x146dd5d]",
          "kind": "rip"
        },
        {
          "source": 3055347,
          "target": 3053040,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35e95f0",
          "kind": "branch"
        },
        {
          "source": 3055352,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "r14, qword ptr [rip + 0x146dd59]",
          "kind": "rip"
        },
        {
          "source": 3055405,
          "target": 24472624,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x146ccfc]",
          "kind": "rip"
        },
        {
          "source": 3055415,
          "target": 3055436,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35e9f4c",
          "kind": "branch"
        },
        {
          "source": 3055417,
          "target": 24472616,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x146cce8]",
          "kind": "rip"
        },
        {
          "source": 3055424,
          "target": 3035088,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35e4fd0",
          "kind": "branch"
        },
        {
          "source": 3055429,
          "target": 24472624,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x146cce4]",
          "kind": "rip"
        },
        {
          "source": 3055451,
          "target": 3055456,
          "owner": 3054416,
          "mnemonic": "jb",
          "operands": "0x212c35e9f60",
          "kind": "branch"
        },
        {
          "source": 3055470,
          "target": 6529824,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c393a320",
          "kind": "branch"
        },
        {
          "source": 3055483,
          "target": 3057156,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea604",
          "kind": "branch"
        },
        {
          "source": 3055504,
          "target": 3055514,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35e9f9a",
          "kind": "branch"
        },
        {
          "source": 3055512,
          "target": 3055563,
          "owner": 3054416,
          "mnemonic": "jmp",
          "operands": "0x212c35e9fcb",
          "kind": "branch"
        },
        {
          "source": 3055600,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146dc61]",
          "kind": "rip"
        },
        {
          "source": 3055610,
          "target": 3055631,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea00f",
          "kind": "branch"
        },
        {
          "source": 3055612,
          "target": 24476752,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x146dc4d]",
          "kind": "rip"
        },
        {
          "source": 3055619,
          "target": 3053040,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35e95f0",
          "kind": "branch"
        },
        {
          "source": 3055624,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146dc49]",
          "kind": "rip"
        },
        {
          "source": 3055656,
          "target": 6512720,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3936050",
          "kind": "branch"
        },
        {
          "source": 3055661,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146dc24]",
          "kind": "rip"
        },
        {
          "source": 3055671,
          "target": 3055692,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea04c",
          "kind": "branch"
        },
        {
          "source": 3055673,
          "target": 24476752,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x146dc10]",
          "kind": "rip"
        },
        {
          "source": 3055680,
          "target": 3053040,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35e95f0",
          "kind": "branch"
        },
        {
          "source": 3055685,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146dc0c]",
          "kind": "rip"
        },
        {
          "source": 3055692,
          "target": 15291232,
          "owner": 3054416,
          "mnemonic": "vmulps",
          "operands": "xmm0, xmm10, xmmword ptr [rip + 0xbab30c]",
          "kind": "rip"
        },
        {
          "source": 3055700,
          "target": 14707380,
          "owner": 3054416,
          "mnemonic": "vmulss",
          "operands": "xmm1, xmm11, dword ptr [rip + 0xb1ca58]",
          "kind": "rip"
        },
        {
          "source": 3055735,
          "target": 6512720,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3936050",
          "kind": "branch"
        },
        {
          "source": 3055740,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146dbd5]",
          "kind": "rip"
        },
        {
          "source": 3055750,
          "target": 3055771,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea09b",
          "kind": "branch"
        },
        {
          "source": 3055752,
          "target": 24476752,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x146dbc1]",
          "kind": "rip"
        },
        {
          "source": 3055759,
          "target": 3053040,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35e95f0",
          "kind": "branch"
        },
        {
          "source": 3055764,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146dbbd]",
          "kind": "rip"
        },
        {
          "source": 3055771,
          "target": 15291232,
          "owner": 3054416,
          "mnemonic": "vmulps",
          "operands": "xmm0, xmm8, xmmword ptr [rip + 0xbab2bd]",
          "kind": "rip"
        },
        {
          "source": 3055779,
          "target": 14707380,
          "owner": 3054416,
          "mnemonic": "vmulss",
          "operands": "xmm1, xmm9, dword ptr [rip + 0xb1ca09]",
          "kind": "rip"
        },
        {
          "source": 3055814,
          "target": 6512720,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3936050",
          "kind": "branch"
        },
        {
          "source": 3055819,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146db86]",
          "kind": "rip"
        },
        {
          "source": 3055829,
          "target": 3055850,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea0ea",
          "kind": "branch"
        },
        {
          "source": 3055831,
          "target": 24476752,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x146db72]",
          "kind": "rip"
        },
        {
          "source": 3055838,
          "target": 3053040,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35e95f0",
          "kind": "branch"
        },
        {
          "source": 3055843,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146db6e]",
          "kind": "rip"
        },
        {
          "source": 3055850,
          "target": 14707380,
          "owner": 3054416,
          "mnemonic": "vmulss",
          "operands": "xmm6, xmm6, dword ptr [rip + 0xb1c9c2]",
          "kind": "rip"
        },
        {
          "source": 3055888,
          "target": 6512720,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3936050",
          "kind": "branch"
        },
        {
          "source": 3055893,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146db3c]",
          "kind": "rip"
        },
        {
          "source": 3055903,
          "target": 3055924,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea134",
          "kind": "branch"
        },
        {
          "source": 3055905,
          "target": 24476752,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x146db28]",
          "kind": "rip"
        },
        {
          "source": 3055912,
          "target": 3053040,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35e95f0",
          "kind": "branch"
        },
        {
          "source": 3055917,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146db24]",
          "kind": "rip"
        },
        {
          "source": 3055924,
          "target": 14707380,
          "owner": 3054416,
          "mnemonic": "vmulss",
          "operands": "xmm7, xmm7, dword ptr [rip + 0xb1c978]",
          "kind": "rip"
        },
        {
          "source": 3055962,
          "target": 6512720,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3936050",
          "kind": "branch"
        },
        {
          "source": 3055967,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146daf2]",
          "kind": "rip"
        },
        {
          "source": 3055977,
          "target": 3055998,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea17e",
          "kind": "branch"
        },
        {
          "source": 3055979,
          "target": 24476752,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x146dade]",
          "kind": "rip"
        },
        {
          "source": 3055986,
          "target": 3053040,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35e95f0",
          "kind": "branch"
        },
        {
          "source": 3055991,
          "target": 24476760,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146dada]",
          "kind": "rip"
        },
        {
          "source": 3056016,
          "target": 6512720,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3936050",
          "kind": "branch"
        },
        {
          "source": 3056021,
          "target": 2847360,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35b7280",
          "kind": "branch"
        },
        {
          "source": 3056054,
          "target": 3056065,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea1c1",
          "kind": "branch"
        },
        {
          "source": 3056063,
          "target": 3056118,
          "owner": 3054416,
          "mnemonic": "jmp",
          "operands": "0x212c35ea1f6",
          "kind": "branch"
        },
        {
          "source": 3056132,
          "target": 3056174,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea22e",
          "kind": "branch"
        },
        {
          "source": 3056169,
          "target": 3056712,
          "owner": 3054416,
          "mnemonic": "jmp",
          "operands": "0x212c35ea448",
          "kind": "branch"
        },
        {
          "source": 3056238,
          "target": 3057248,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea660",
          "kind": "branch"
        },
        {
          "source": 3056262,
          "target": 3056327,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea2c7",
          "kind": "branch"
        },
        {
          "source": 3056270,
          "target": 3056332,
          "owner": 3054416,
          "mnemonic": "jb",
          "operands": "0x212c35ea2cc",
          "kind": "branch"
        },
        {
          "source": 3056275,
          "target": 3057248,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea660",
          "kind": "branch"
        },
        {
          "source": 3056285,
          "target": 16328472,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xca8474]",
          "kind": "rip"
        },
        {
          "source": 3056304,
          "target": 3058012,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea95c",
          "kind": "branch"
        },
        {
          "source": 3056325,
          "target": 3056351,
          "owner": 3054416,
          "mnemonic": "jmp",
          "operands": "0x212c35ea2df",
          "kind": "branch"
        },
        {
          "source": 3056330,
          "target": 3056351,
          "owner": 3054416,
          "mnemonic": "jmp",
          "operands": "0x212c35ea2df",
          "kind": "branch"
        },
        {
          "source": 3056332,
          "target": 16328472,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xca8445]",
          "kind": "rip"
        },
        {
          "source": 3056393,
          "target": 3056553,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea3a9",
          "kind": "branch"
        },
        {
          "source": 3056402,
          "target": 3056474,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea35a",
          "kind": "branch"
        },
        {
          "source": 3056468,
          "target": 3056416,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea320",
          "kind": "branch"
        },
        {
          "source": 3056477,
          "target": 3056614,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea3e6",
          "kind": "branch"
        },
        {
          "source": 3056549,
          "target": 3056496,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea370",
          "kind": "branch"
        },
        {
          "source": 3056551,
          "target": 3056614,
          "owner": 3054416,
          "mnemonic": "jmp",
          "operands": "0x212c35ea3e6",
          "kind": "branch"
        },
        {
          "source": 3056556,
          "target": 3056618,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea3ea",
          "kind": "branch"
        },
        {
          "source": 3056612,
          "target": 3056560,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea3b0",
          "kind": "branch"
        },
        {
          "source": 3056621,
          "target": 3056678,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea426",
          "kind": "branch"
        },
        {
          "source": 3056636,
          "target": 3056662,
          "owner": 3054416,
          "mnemonic": "jb",
          "operands": "0x212c35ea416",
          "kind": "branch"
        },
        {
          "source": 3056653,
          "target": 3058012,
          "owner": 3054416,
          "mnemonic": "jae",
          "operands": "0x212c35ea95c",
          "kind": "branch"
        },
        {
          "source": 3056662,
          "target": 16328472,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xca82fb]",
          "kind": "rip"
        },
        {
          "source": 3056719,
          "target": 16328288,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xca820b]",
          "kind": "rip"
        },
        {
          "source": 3056742,
          "target": 24545512,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x147e47b]",
          "kind": "rip"
        },
        {
          "source": 3056779,
          "target": 24591020,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rbx, [rip + 0x148961a]",
          "kind": "rip"
        },
        {
          "source": 3056793,
          "target": 3056909,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea50d",
          "kind": "branch"
        },
        {
          "source": 3056805,
          "target": 3056838,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea4c6",
          "kind": "branch"
        },
        {
          "source": 3056823,
          "target": 3056832,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea4c0",
          "kind": "branch"
        },
        {
          "source": 3056828,
          "target": 3056909,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea50d",
          "kind": "branch"
        },
        {
          "source": 3056830,
          "target": 3056838,
          "owner": 3054416,
          "mnemonic": "jmp",
          "operands": "0x212c35ea4c6",
          "kind": "branch"
        },
        {
          "source": 3056836,
          "target": 3056909,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea50d",
          "kind": "branch"
        },
        {
          "source": 3056845,
          "target": 3056856,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea4d8",
          "kind": "branch"
        },
        {
          "source": 3056854,
          "target": 3056906,
          "owner": 3054416,
          "mnemonic": "jmp",
          "operands": "0x212c35ea50a",
          "kind": "branch"
        },
        {
          "source": 3056912,
          "target": 3057156,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea604",
          "kind": "branch"
        },
        {
          "source": 3056918,
          "target": 24476744,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146d72b]",
          "kind": "rip"
        },
        {
          "source": 3056928,
          "target": 3056949,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea535",
          "kind": "branch"
        },
        {
          "source": 3056930,
          "target": 24476736,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x146d717]",
          "kind": "rip"
        },
        {
          "source": 3056937,
          "target": 3053504,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35e97c0",
          "kind": "branch"
        },
        {
          "source": 3056942,
          "target": 24476744,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x146d713]",
          "kind": "rip"
        },
        {
          "source": 3056949,
          "target": 6547712,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c393e900",
          "kind": "branch"
        },
        {
          "source": 3056956,
          "target": 3057156,
          "owner": 3054416,
          "mnemonic": "je",
          "operands": "0x212c35ea604",
          "kind": "branch"
        },
        {
          "source": 3056962,
          "target": 24451112,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "eax, dword ptr [rip + 0x14672e0]",
          "kind": "rip"
        },
        {
          "source": 3056968,
          "target": 24425176,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "ecx, dword ptr [rip + 0x1460d8a]",
          "kind": "rip"
        },
        {
          "source": 3056993,
          "target": 3057253,
          "owner": 3054416,
          "mnemonic": "jg",
          "operands": "0x212c35ea665",
          "kind": "branch"
        },
        {
          "source": 3057007,
          "target": 15305660,
          "owner": 3054416,
          "mnemonic": "vmulss",
          "operands": "xmm6, xmm0, dword ptr [rip + 0xbae645]",
          "kind": "rip"
        },
        {
          "source": 3057015,
          "target": 24476744,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rbx, qword ptr [rip + 0x146d6ca]",
          "kind": "rip"
        },
        {
          "source": 3057025,
          "target": 3057046,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea596",
          "kind": "branch"
        },
        {
          "source": 3057027,
          "target": 24476736,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x146d6b6]",
          "kind": "rip"
        },
        {
          "source": 3057034,
          "target": 3053504,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c35e97c0",
          "kind": "branch"
        },
        {
          "source": 3057039,
          "target": 24476744,
          "owner": 3054416,
          "mnemonic": "mov",
          "operands": "rbx, qword ptr [rip + 0x146d6b2]",
          "kind": "rip"
        },
        {
          "source": 3057063,
          "target": 14265312,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c409abe0",
          "kind": "branch"
        },
        {
          "source": 3057076,
          "target": 14232416,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c4092b60",
          "kind": "branch"
        },
        {
          "source": 3057087,
          "target": 15305664,
          "owner": 3054416,
          "mnemonic": "vmulps",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0xbae5f9]",
          "kind": "rip"
        },
        {
          "source": 3057114,
          "target": 24451120,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x146724f]",
          "kind": "rip"
        },
        {
          "source": 3057131,
          "target": 3057136,
          "owner": 3054416,
          "mnemonic": "jb",
          "operands": "0x212c35ea5f0",
          "kind": "branch"
        },
        {
          "source": 3057151,
          "target": 6576928,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3945b20",
          "kind": "branch"
        },
        {
          "source": 3057248,
          "target": 916704,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c33dfce0",
          "kind": "branch"
        },
        {
          "source": 3057253,
          "target": 24451112,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x14671bc]",
          "kind": "rip"
        },
        {
          "source": 3057260,
          "target": 11213448,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3db1a88",
          "kind": "branch"
        },
        {
          "source": 3057265,
          "target": 24451112,
          "owner": 3054416,
          "mnemonic": "cmp",
          "operands": "dword ptr [rip + 0x14671b0], -1",
          "kind": "rip"
        },
        {
          "source": 3057272,
          "target": 3056999,
          "owner": 3054416,
          "mnemonic": "jne",
          "operands": "0x212c35ea567",
          "kind": "branch"
        },
        {
          "source": 3057336,
          "target": 15305680,
          "owner": 3054416,
          "mnemonic": "vbroadcastss",
          "operands": "xmm0, dword ptr [rip + 0xbae50f]",
          "kind": "rip"
        },
        {
          "source": 3057351,
          "target": 15305684,
          "owner": 3054416,
          "mnemonic": "vbroadcastss",
          "operands": "xmm1, dword ptr [rip + 0xbae504]",
          "kind": "rip"
        },
        {
          "source": 3057380,
          "target": 19001984,
          "owner": 3054416,
          "mnemonic": "vpsubb",
          "operands": "xmm1, xmm1, xmmword ptr [rip + 0xf34b94]",
          "kind": "rip"
        },
        {
          "source": 3057393,
          "target": 19002000,
          "owner": 3054416,
          "mnemonic": "vmovq",
          "operands": "xmm1, qword ptr [rip + 0xf34b97]",
          "kind": "rip"
        },
        {
          "source": 3057417,
          "target": 24451120,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1467120]",
          "kind": "rip"
        },
        {
          "source": 3057432,
          "target": 1119520,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3411520",
          "kind": "branch"
        },
        {
          "source": 3057521,
          "target": 15305688,
          "owner": 3054416,
          "mnemonic": "vbroadcastss",
          "operands": "xmm0, dword ptr [rip + 0xbae45e]",
          "kind": "rip"
        },
        {
          "source": 3057539,
          "target": 15305692,
          "owner": 3054416,
          "mnemonic": "vbroadcastss",
          "operands": "xmm1, dword ptr [rip + 0xbae450]",
          "kind": "rip"
        },
        {
          "source": 3057568,
          "target": 19002487,
          "owner": 3054416,
          "mnemonic": "vpsubb",
          "operands": "xmm1, xmm1, xmmword ptr [rip + 0xf34ccf]",
          "kind": "rip"
        },
        {
          "source": 3057581,
          "target": 19002503,
          "owner": 3054416,
          "mnemonic": "vmovq",
          "operands": "xmm1, qword ptr [rip + 0xf34cd2]",
          "kind": "rip"
        },
        {
          "source": 3057605,
          "target": 24451152,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1467084]",
          "kind": "rip"
        },
        {
          "source": 3057620,
          "target": 1119520,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3411520",
          "kind": "branch"
        },
        {
          "source": 3057698,
          "target": 19001744,
          "owner": 3054416,
          "mnemonic": "xor",
          "operands": "rcx, qword ptr [rip + 0xf34967]",
          "kind": "rip"
        },
        {
          "source": 3057720,
          "target": 15305696,
          "owner": 3054416,
          "mnemonic": "vbroadcastss",
          "operands": "xmm0, dword ptr [rip + 0xbae39f]",
          "kind": "rip"
        },
        {
          "source": 3057737,
          "target": 15305700,
          "owner": 3054416,
          "mnemonic": "vbroadcastss",
          "operands": "xmm1, dword ptr [rip + 0xbae392]",
          "kind": "rip"
        },
        {
          "source": 3057766,
          "target": 19002296,
          "owner": 3054416,
          "mnemonic": "vpxor",
          "operands": "xmm1, xmm1, xmmword ptr [rip + 0xf34b4a]",
          "kind": "rip"
        },
        {
          "source": 3057779,
          "target": 19002312,
          "owner": 3054416,
          "mnemonic": "vmovq",
          "operands": "xmm1, qword ptr [rip + 0xf34b4d]",
          "kind": "rip"
        },
        {
          "source": 3057803,
          "target": 24451184,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1466fde]",
          "kind": "rip"
        },
        {
          "source": 3057818,
          "target": 1119520,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3411520",
          "kind": "branch"
        },
        {
          "source": 3057882,
          "target": 15295420,
          "owner": 3054416,
          "mnemonic": "vbroadcastss",
          "operands": "xmm0, dword ptr [rip + 0xbabad9]",
          "kind": "rip"
        },
        {
          "source": 3057897,
          "target": 15295480,
          "owner": 3054416,
          "mnemonic": "vbroadcastss",
          "operands": "xmm1, dword ptr [rip + 0xbabb06]",
          "kind": "rip"
        },
        {
          "source": 3057926,
          "target": 19002272,
          "owner": 3054416,
          "mnemonic": "vpxor",
          "operands": "xmm1, xmm1, xmmword ptr [rip + 0xf34a92]",
          "kind": "rip"
        },
        {
          "source": 3057939,
          "target": 19002288,
          "owner": 3054416,
          "mnemonic": "vmovq",
          "operands": "xmm1, qword ptr [rip + 0xf34a95]",
          "kind": "rip"
        },
        {
          "source": 3057963,
          "target": 24451216,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1466f5e]",
          "kind": "rip"
        },
        {
          "source": 3057978,
          "target": 1119520,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3411520",
          "kind": "branch"
        },
        {
          "source": 3057983,
          "target": 3059888,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x76a]",
          "kind": "rip"
        },
        {
          "source": 3057990,
          "target": 11214716,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3db1f7c",
          "kind": "branch"
        },
        {
          "source": 3057995,
          "target": 24451112,
          "owner": 3054416,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1466ed6]",
          "kind": "rip"
        },
        {
          "source": 3058002,
          "target": 11213632,
          "owner": 3054416,
          "mnemonic": "call",
          "operands": "0x212c3db1b40",
          "kind": "branch"
        },
        {
          "source": 3058007,
          "target": 3056999,
          "owner": 3054416,
          "mnemonic": "jmp",
          "operands": "0x212c35ea567",
          "kind": "branch"
        }
      ],
      "incoming_truncated": false,
      "incoming": []
    },
    {
      "va": "0x212c3706c10",
      "rva": "0x406c10",
      "metadata": {
        "begin": 4221968,
        "end": 4222169,
        "unwind": 17111012,
        "table_rva": 66523048,
        "decoded_end": 4222169,
        "instruction_count": 52
      },
      "refs_truncated": false,
      "refs": [
        {
          "source": 4222006,
          "target": 4222016,
          "owner": 4221968,
          "mnemonic": "je",
          "operands": "0x212c3706c40",
          "kind": "branch"
        },
        {
          "source": 4222014,
          "target": 4222065,
          "owner": 4221968,
          "mnemonic": "jmp",
          "operands": "0x212c3706c71",
          "kind": "branch"
        },
        {
          "source": 4222097,
          "target": 9749040,
          "owner": 4221968,
          "mnemonic": "call",
          "operands": "0x212c3c4c230",
          "kind": "branch"
        },
        {
          "source": 4222104,
          "target": 4222160,
          "owner": 4221968,
          "mnemonic": "je",
          "operands": "0x212c3706cd0",
          "kind": "branch"
        },
        {
          "source": 4222132,
          "target": 16328288,
          "owner": 4221968,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xb8b9a6]",
          "kind": "rip"
        }
      ],
      "incoming_truncated": false,
      "incoming": []
    },
    {
      "va": "0x212c3722970",
      "rva": "0x422970",
      "metadata": {
        "begin": 4335984,
        "end": 4337673,
        "unwind": 17134044,
        "table_rva": 66528664,
        "decoded_end": 4337673,
        "instruction_count": 363
      },
      "refs_truncated": false,
      "refs": [
        {
          "source": 4336039,
          "target": 4336049,
          "owner": 4335984,
          "mnemonic": "je",
          "operands": "0x212c37229b1",
          "kind": "branch"
        },
        {
          "source": 4336047,
          "target": 4336098,
          "owner": 4335984,
          "mnemonic": "jmp",
          "operands": "0x212c37229e2",
          "kind": "branch"
        },
        {
          "source": 4336102,
          "target": 14707376,
          "owner": 4335984,
          "mnemonic": "vdivss",
          "operands": "xmm6, xmm0, dword ptr [rip + 0x9e40c2]",
          "kind": "rip"
        },
        {
          "source": 4336110,
          "target": 24578856,
          "owner": 4335984,
          "mnemonic": "mov",
          "operands": "r14, qword ptr [rip + 0x134e133]",
          "kind": "rip"
        },
        {
          "source": 4336120,
          "target": 4336141,
          "owner": 4335984,
          "mnemonic": "jne",
          "operands": "0x212c3722a0d",
          "kind": "branch"
        },
        {
          "source": 4336122,
          "target": 24578848,
          "owner": 4335984,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x134e11f]",
          "kind": "rip"
        },
        {
          "source": 4336129,
          "target": 2813840,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "0x212c35aef90",
          "kind": "branch"
        },
        {
          "source": 4336134,
          "target": 24578856,
          "owner": 4335984,
          "mnemonic": "mov",
          "operands": "r14, qword ptr [rip + 0x134e11b]",
          "kind": "rip"
        },
        {
          "source": 4336154,
          "target": 4336221,
          "owner": 4335984,
          "mnemonic": "jae",
          "operands": "0x212c3722a5d",
          "kind": "branch"
        },
        {
          "source": 4336169,
          "target": 4336230,
          "owner": 4335984,
          "mnemonic": "jae",
          "operands": "0x212c3722a66",
          "kind": "branch"
        },
        {
          "source": 4336184,
          "target": 4336239,
          "owner": 4335984,
          "mnemonic": "jae",
          "operands": "0x212c3722a6f",
          "kind": "branch"
        },
        {
          "source": 4336199,
          "target": 4336248,
          "owner": 4335984,
          "mnemonic": "jae",
          "operands": "0x212c3722a78",
          "kind": "branch"
        },
        {
          "source": 4336214,
          "target": 4336257,
          "owner": 4335984,
          "mnemonic": "jae",
          "operands": "0x212c3722a81",
          "kind": "branch"
        },
        {
          "source": 4336219,
          "target": 4336264,
          "owner": 4335984,
          "mnemonic": "jmp",
          "operands": "0x212c3722a88",
          "kind": "branch"
        },
        {
          "source": 4336228,
          "target": 4336264,
          "owner": 4335984,
          "mnemonic": "jmp",
          "operands": "0x212c3722a88",
          "kind": "branch"
        },
        {
          "source": 4336237,
          "target": 4336264,
          "owner": 4335984,
          "mnemonic": "jmp",
          "operands": "0x212c3722a88",
          "kind": "branch"
        },
        {
          "source": 4336246,
          "target": 4336264,
          "owner": 4335984,
          "mnemonic": "jmp",
          "operands": "0x212c3722a88",
          "kind": "branch"
        },
        {
          "source": 4336255,
          "target": 4336264,
          "owner": 4335984,
          "mnemonic": "jmp",
          "operands": "0x212c3722a88",
          "kind": "branch"
        },
        {
          "source": 4336264,
          "target": 24583336,
          "owner": 4335984,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x134f219]",
          "kind": "rip"
        },
        {
          "source": 4336279,
          "target": 15306932,
          "owner": 4335984,
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0xa76615]",
          "kind": "rip"
        },
        {
          "source": 4336349,
          "target": 4336354,
          "owner": 4335984,
          "mnemonic": "jb",
          "operands": "0x212c3722ae2",
          "kind": "branch"
        },
        {
          "source": 4336358,
          "target": 4336409,
          "owner": 4335984,
          "mnemonic": "jne",
          "operands": "0x212c3722b19",
          "kind": "branch"
        },
        {
          "source": 4336404,
          "target": 4336603,
          "owner": 4335984,
          "mnemonic": "jmp",
          "operands": "0x212c3722bdb",
          "kind": "branch"
        },
        {
          "source": 4336433,
          "target": 4336444,
          "owner": 4335984,
          "mnemonic": "jb",
          "operands": "0x212c3722b3c",
          "kind": "branch"
        },
        {
          "source": 4336442,
          "target": 4336451,
          "owner": 4335984,
          "mnemonic": "jmp",
          "operands": "0x212c3722b43",
          "kind": "branch"
        },
        {
          "source": 4336454,
          "target": 4337663,
          "owner": 4335984,
          "mnemonic": "js",
          "operands": "0x212c3722fff",
          "kind": "branch"
        },
        {
          "source": 4336464,
          "target": 4336491,
          "owner": 4335984,
          "mnemonic": "ja",
          "operands": "0x212c3722b6b",
          "kind": "branch"
        },
        {
          "source": 4336489,
          "target": 4336603,
          "owner": 4335984,
          "mnemonic": "jmp",
          "operands": "0x212c3722bdb",
          "kind": "branch"
        },
        {
          "source": 4336512,
          "target": 16328472,
          "owner": 4335984,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xb6fb91]",
          "kind": "rip"
        },
        {
          "source": 4336536,
          "target": 4336562,
          "owner": 4335984,
          "mnemonic": "jb",
          "operands": "0x212c3722bb2",
          "kind": "branch"
        },
        {
          "source": 4336560,
          "target": 4336575,
          "owner": 4335984,
          "mnemonic": "jmp",
          "operands": "0x212c3722bbf",
          "kind": "branch"
        },
        {
          "source": 4336596,
          "target": 14441760,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 4336626,
          "target": 9026784,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "0x212c3b9bce0",
          "kind": "branch"
        },
        {
          "source": 4336645,
          "target": 4336651,
          "owner": 4335984,
          "mnemonic": "jb",
          "operands": "0x212c3722c0b",
          "kind": "branch"
        },
        {
          "source": 4336673,
          "target": 8320912,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "0x212c3aef790",
          "kind": "branch"
        },
        {
          "source": 4336688,
          "target": 4336748,
          "owner": 4335984,
          "mnemonic": "jb",
          "operands": "0x212c3722c6c",
          "kind": "branch"
        },
        {
          "source": 4336706,
          "target": 4336732,
          "owner": 4335984,
          "mnemonic": "ja",
          "operands": "0x212c3722c5c",
          "kind": "branch"
        },
        {
          "source": 4336723,
          "target": 4337671,
          "owner": 4335984,
          "mnemonic": "jae",
          "operands": "0x212c3723007",
          "kind": "branch"
        },
        {
          "source": 4336732,
          "target": 16328472,
          "owner": 4335984,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xb6fab5]",
          "kind": "rip"
        },
        {
          "source": 4336756,
          "target": 4336816,
          "owner": 4335984,
          "mnemonic": "jb",
          "operands": "0x212c3722cb0",
          "kind": "branch"
        },
        {
          "source": 4336774,
          "target": 4336800,
          "owner": 4335984,
          "mnemonic": "ja",
          "operands": "0x212c3722ca0",
          "kind": "branch"
        },
        {
          "source": 4336791,
          "target": 4337671,
          "owner": 4335984,
          "mnemonic": "jae",
          "operands": "0x212c3723007",
          "kind": "branch"
        },
        {
          "source": 4336800,
          "target": 16328472,
          "owner": 4335984,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xb6fa71]",
          "kind": "rip"
        },
        {
          "source": 4336818,
          "target": 4337636,
          "owner": 4335984,
          "mnemonic": "je",
          "operands": "0x212c3722fe4",
          "kind": "branch"
        },
        {
          "source": 4336824,
          "target": 24578856,
          "owner": 4335984,
          "mnemonic": "mov",
          "operands": "rbx, qword ptr [rip + 0x134de69]",
          "kind": "rip"
        },
        {
          "source": 4336834,
          "target": 4336855,
          "owner": 4335984,
          "mnemonic": "jne",
          "operands": "0x212c3722cd7",
          "kind": "branch"
        },
        {
          "source": 4336836,
          "target": 24578848,
          "owner": 4335984,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x134de55]",
          "kind": "rip"
        },
        {
          "source": 4336843,
          "target": 2813840,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "0x212c35aef90",
          "kind": "branch"
        },
        {
          "source": 4336848,
          "target": 24578856,
          "owner": 4335984,
          "mnemonic": "mov",
          "operands": "rbx, qword ptr [rip + 0x134de51]",
          "kind": "rip"
        },
        {
          "source": 4336881,
          "target": 4336886,
          "owner": 4335984,
          "mnemonic": "jbe",
          "operands": "0x212c3722cf6",
          "kind": "branch"
        },
        {
          "source": 4336936,
          "target": 8259312,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "0x212c3ae06f0",
          "kind": "branch"
        },
        {
          "source": 4336945,
          "target": 4336999,
          "owner": 4335984,
          "mnemonic": "je",
          "operands": "0x212c3722d67",
          "kind": "branch"
        },
        {
          "source": 4336959,
          "target": 16328280,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xb6f913]",
          "kind": "rip"
        },
        {
          "source": 4336972,
          "target": 14707376,
          "owner": 4335984,
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x9e3d5c]",
          "kind": "rip"
        },
        {
          "source": 4337025,
          "target": 4337030,
          "owner": 4335984,
          "mnemonic": "jb",
          "operands": "0x212c3722d86",
          "kind": "branch"
        },
        {
          "source": 4337080,
          "target": 8259312,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "0x212c3ae06f0",
          "kind": "branch"
        },
        {
          "source": 4337087,
          "target": 4337148,
          "owner": 4335984,
          "mnemonic": "je",
          "operands": "0x212c3722dfc",
          "kind": "branch"
        },
        {
          "source": 4337105,
          "target": 16328280,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xb6f881]",
          "kind": "rip"
        },
        {
          "source": 4337111,
          "target": 14707376,
          "owner": 4335984,
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x9e3cd1]",
          "kind": "rip"
        },
        {
          "source": 4337174,
          "target": 4337179,
          "owner": 4335984,
          "mnemonic": "jb",
          "operands": "0x212c3722e1b",
          "kind": "branch"
        },
        {
          "source": 4337229,
          "target": 8259312,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "0x212c3ae06f0",
          "kind": "branch"
        },
        {
          "source": 4337236,
          "target": 4337297,
          "owner": 4335984,
          "mnemonic": "je",
          "operands": "0x212c3722e91",
          "kind": "branch"
        },
        {
          "source": 4337254,
          "target": 16328280,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xb6f7ec]",
          "kind": "rip"
        },
        {
          "source": 4337260,
          "target": 14707376,
          "owner": 4335984,
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x9e3c3c]",
          "kind": "rip"
        },
        {
          "source": 4337323,
          "target": 4337328,
          "owner": 4335984,
          "mnemonic": "jb",
          "operands": "0x212c3722eb0",
          "kind": "branch"
        },
        {
          "source": 4337378,
          "target": 8259312,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "0x212c3ae06f0",
          "kind": "branch"
        },
        {
          "source": 4337385,
          "target": 4337446,
          "owner": 4335984,
          "mnemonic": "je",
          "operands": "0x212c3722f26",
          "kind": "branch"
        },
        {
          "source": 4337403,
          "target": 16328280,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xb6f757]",
          "kind": "rip"
        },
        {
          "source": 4337409,
          "target": 14707376,
          "owner": 4335984,
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x9e3ba7]",
          "kind": "rip"
        },
        {
          "source": 4337472,
          "target": 4337477,
          "owner": 4335984,
          "mnemonic": "jb",
          "operands": "0x212c3722f45",
          "kind": "branch"
        },
        {
          "source": 4337527,
          "target": 8259312,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "0x212c3ae06f0",
          "kind": "branch"
        },
        {
          "source": 4337534,
          "target": 4337595,
          "owner": 4335984,
          "mnemonic": "je",
          "operands": "0x212c3722fbb",
          "kind": "branch"
        },
        {
          "source": 4337552,
          "target": 16328280,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xb6f6c2]",
          "kind": "rip"
        },
        {
          "source": 4337558,
          "target": 14707376,
          "owner": 4335984,
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x9e3b12]",
          "kind": "rip"
        },
        {
          "source": 4337595,
          "target": 24583336,
          "owner": 4335984,
          "mnemonic": "mov",
          "operands": "rsi, qword ptr [rip + 0x134ece6]",
          "kind": "rip"
        },
        {
          "source": 4337613,
          "target": 4337622,
          "owner": 4335984,
          "mnemonic": "je",
          "operands": "0x212c3722fd6",
          "kind": "branch"
        },
        {
          "source": 4337622,
          "target": 8589728,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "0x212c3b311a0",
          "kind": "branch"
        },
        {
          "source": 4337634,
          "target": 4337638,
          "owner": 4335984,
          "mnemonic": "jmp",
          "operands": "0x212c3722fe6",
          "kind": "branch"
        },
        {
          "source": 4337666,
          "target": 925984,
          "owner": 4335984,
          "mnemonic": "call",
          "operands": "0x212c33e2120",
          "kind": "branch"
        }
      ],
      "incoming_truncated": false,
      "incoming": []
    },
    {
      "va": "0x212c37fd710",
      "rva": "0x4fd710",
      "metadata": {
        "begin": 5232400,
        "end": 5234385,
        "unwind": 17238796,
        "table_rva": 66552880,
        "decoded_end": 5234385,
        "instruction_count": 516
      },
      "refs_truncated": false,
      "refs": [
        {
          "source": 5232453,
          "target": 5234375,
          "owner": 5232400,
          "mnemonic": "js",
          "operands": "0x212c37fdec7",
          "kind": "branch"
        },
        {
          "source": 5232473,
          "target": 5232525,
          "owner": 5232400,
          "mnemonic": "ja",
          "operands": "0x212c37fd78d",
          "kind": "branch"
        },
        {
          "source": 5232500,
          "target": 14441760,
          "owner": 5232400,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5232523,
          "target": 5232649,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fd809",
          "kind": "branch"
        },
        {
          "source": 5232545,
          "target": 16328472,
          "owner": 5232400,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa94f70]",
          "kind": "rip"
        },
        {
          "source": 5232569,
          "target": 5232606,
          "owner": 5232400,
          "mnemonic": "jb",
          "operands": "0x212c37fd7de",
          "kind": "branch"
        },
        {
          "source": 5232583,
          "target": 5234383,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fdecf",
          "kind": "branch"
        },
        {
          "source": 5232604,
          "target": 5232618,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fd7ea",
          "kind": "branch"
        },
        {
          "source": 5232639,
          "target": 14441760,
          "owner": 5232400,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5232675,
          "target": 5232706,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fd842",
          "kind": "branch"
        },
        {
          "source": 5232687,
          "target": 5232721,
          "owner": 5232400,
          "mnemonic": "jae",
          "operands": "0x212c37fd851",
          "kind": "branch"
        },
        {
          "source": 5232701,
          "target": 5232990,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fd95e",
          "kind": "branch"
        },
        {
          "source": 5232716,
          "target": 5233047,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fd997",
          "kind": "branch"
        },
        {
          "source": 5232741,
          "target": 5232760,
          "owner": 5232400,
          "mnemonic": "jae",
          "operands": "0x212c37fd878",
          "kind": "branch"
        },
        {
          "source": 5232755,
          "target": 5232912,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fd910",
          "kind": "branch"
        },
        {
          "source": 5232910,
          "target": 5232800,
          "owner": 5232400,
          "mnemonic": "jne",
          "operands": "0x212c37fd8a0",
          "kind": "branch"
        },
        {
          "source": 5232919,
          "target": 5232990,
          "owner": 5232400,
          "mnemonic": "jne",
          "operands": "0x212c37fd95e",
          "kind": "branch"
        },
        {
          "source": 5232993,
          "target": 5233047,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fd997",
          "kind": "branch"
        },
        {
          "source": 5233045,
          "target": 5233024,
          "owner": 5232400,
          "mnemonic": "jne",
          "operands": "0x212c37fd980",
          "kind": "branch"
        },
        {
          "source": 5233075,
          "target": 5233189,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fda25",
          "kind": "branch"
        },
        {
          "source": 5233084,
          "target": 5233177,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fda19",
          "kind": "branch"
        },
        {
          "source": 5233092,
          "target": 5233139,
          "owner": 5232400,
          "mnemonic": "jne",
          "operands": "0x212c37fd9f3",
          "kind": "branch"
        },
        {
          "source": 5233099,
          "target": 5233120,
          "owner": 5232400,
          "mnemonic": "jb",
          "operands": "0x212c37fd9e0",
          "kind": "branch"
        },
        {
          "source": 5233105,
          "target": 5233124,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fd9e4",
          "kind": "branch"
        },
        {
          "source": 5233130,
          "target": 14441504,
          "owner": 5232400,
          "mnemonic": "call",
          "operands": "0x212c40c5c20",
          "kind": "branch"
        },
        {
          "source": 5233137,
          "target": 5233184,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fda20",
          "kind": "branch"
        },
        {
          "source": 5233142,
          "target": 5233189,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fda25",
          "kind": "branch"
        },
        {
          "source": 5233152,
          "target": 5233094,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fd9c6",
          "kind": "branch"
        },
        {
          "source": 5233154,
          "target": 5233139,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fd9f3",
          "kind": "branch"
        },
        {
          "source": 5233171,
          "target": 5233189,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fda25",
          "kind": "branch"
        },
        {
          "source": 5233182,
          "target": 5233168,
          "owner": 5232400,
          "mnemonic": "jne",
          "operands": "0x212c37fda10",
          "kind": "branch"
        },
        {
          "source": 5233187,
          "target": 5233192,
          "owner": 5232400,
          "mnemonic": "jne",
          "operands": "0x212c37fda28",
          "kind": "branch"
        },
        {
          "source": 5233204,
          "target": 5233275,
          "owner": 5232400,
          "mnemonic": "jb",
          "operands": "0x212c37fda7b",
          "kind": "branch"
        },
        {
          "source": 5233218,
          "target": 5233244,
          "owner": 5232400,
          "mnemonic": "ja",
          "operands": "0x212c37fda5c",
          "kind": "branch"
        },
        {
          "source": 5233235,
          "target": 5234383,
          "owner": 5232400,
          "mnemonic": "jae",
          "operands": "0x212c37fdecf",
          "kind": "branch"
        },
        {
          "source": 5233244,
          "target": 16328472,
          "owner": 5232400,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa94cb5]",
          "kind": "rip"
        },
        {
          "source": 5233278,
          "target": 5234156,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fddec",
          "kind": "branch"
        },
        {
          "source": 5233296,
          "target": 5234375,
          "owner": 5232400,
          "mnemonic": "js",
          "operands": "0x212c37fdec7",
          "kind": "branch"
        },
        {
          "source": 5233306,
          "target": 5233361,
          "owner": 5232400,
          "mnemonic": "ja",
          "operands": "0x212c37fdad1",
          "kind": "branch"
        },
        {
          "source": 5233334,
          "target": 14441760,
          "owner": 5232400,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5233356,
          "target": 5233495,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fdb57",
          "kind": "branch"
        },
        {
          "source": 5233381,
          "target": 16328472,
          "owner": 5232400,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa94c2c]",
          "kind": "rip"
        },
        {
          "source": 5233405,
          "target": 5233442,
          "owner": 5232400,
          "mnemonic": "jb",
          "operands": "0x212c37fdb22",
          "kind": "branch"
        },
        {
          "source": 5233419,
          "target": 5234383,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fdecf",
          "kind": "branch"
        },
        {
          "source": 5233440,
          "target": 5233454,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fdb2e",
          "kind": "branch"
        },
        {
          "source": 5233481,
          "target": 14441760,
          "owner": 5232400,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5233516,
          "target": 5233546,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fdb8a",
          "kind": "branch"
        },
        {
          "source": 5233527,
          "target": 5233561,
          "owner": 5232400,
          "mnemonic": "jae",
          "operands": "0x212c37fdb99",
          "kind": "branch"
        },
        {
          "source": 5233541,
          "target": 5233817,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fdc99",
          "kind": "branch"
        },
        {
          "source": 5233556,
          "target": 5233863,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fdcc7",
          "kind": "branch"
        },
        {
          "source": 5233577,
          "target": 5233596,
          "owner": 5232400,
          "mnemonic": "jae",
          "operands": "0x212c37fdbbc",
          "kind": "branch"
        },
        {
          "source": 5233591,
          "target": 5233741,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fdc4d",
          "kind": "branch"
        },
        {
          "source": 5233739,
          "target": 5233632,
          "owner": 5232400,
          "mnemonic": "jne",
          "operands": "0x212c37fdbe0",
          "kind": "branch"
        },
        {
          "source": 5233748,
          "target": 5233817,
          "owner": 5232400,
          "mnemonic": "jne",
          "operands": "0x212c37fdc99",
          "kind": "branch"
        },
        {
          "source": 5233820,
          "target": 5233863,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fdcc7",
          "kind": "branch"
        },
        {
          "source": 5233861,
          "target": 5233840,
          "owner": 5232400,
          "mnemonic": "jne",
          "operands": "0x212c37fdcb0",
          "kind": "branch"
        },
        {
          "source": 5233887,
          "target": 5234005,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fdd55",
          "kind": "branch"
        },
        {
          "source": 5233899,
          "target": 5233993,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fdd49",
          "kind": "branch"
        },
        {
          "source": 5233908,
          "target": 5233958,
          "owner": 5232400,
          "mnemonic": "jne",
          "operands": "0x212c37fdd26",
          "kind": "branch"
        },
        {
          "source": 5233915,
          "target": 5233936,
          "owner": 5232400,
          "mnemonic": "jb",
          "operands": "0x212c37fdd10",
          "kind": "branch"
        },
        {
          "source": 5233921,
          "target": 5233940,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fdd14",
          "kind": "branch"
        },
        {
          "source": 5233946,
          "target": 14441504,
          "owner": 5232400,
          "mnemonic": "call",
          "operands": "0x212c40c5c20",
          "kind": "branch"
        },
        {
          "source": 5233956,
          "target": 5234000,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fdd50",
          "kind": "branch"
        },
        {
          "source": 5233961,
          "target": 5234005,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fdd55",
          "kind": "branch"
        },
        {
          "source": 5233971,
          "target": 5233910,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fdcf6",
          "kind": "branch"
        },
        {
          "source": 5233973,
          "target": 5233958,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fdd26",
          "kind": "branch"
        },
        {
          "source": 5233987,
          "target": 5234005,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fdd55",
          "kind": "branch"
        },
        {
          "source": 5233998,
          "target": 5233984,
          "owner": 5232400,
          "mnemonic": "jne",
          "operands": "0x212c37fdd40",
          "kind": "branch"
        },
        {
          "source": 5234003,
          "target": 5234008,
          "owner": 5232400,
          "mnemonic": "jne",
          "operands": "0x212c37fdd58",
          "kind": "branch"
        },
        {
          "source": 5234016,
          "target": 5234079,
          "owner": 5232400,
          "mnemonic": "jb",
          "operands": "0x212c37fdd9f",
          "kind": "branch"
        },
        {
          "source": 5234030,
          "target": 5234056,
          "owner": 5232400,
          "mnemonic": "ja",
          "operands": "0x212c37fdd88",
          "kind": "branch"
        },
        {
          "source": 5234047,
          "target": 5234383,
          "owner": 5232400,
          "mnemonic": "jae",
          "operands": "0x212c37fdecf",
          "kind": "branch"
        },
        {
          "source": 5234056,
          "target": 16328472,
          "owner": 5232400,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa94989]",
          "kind": "rip"
        },
        {
          "source": 5234082,
          "target": 5234156,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fddec",
          "kind": "branch"
        },
        {
          "source": 5234095,
          "target": 5234156,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fddec",
          "kind": "branch"
        },
        {
          "source": 5234108,
          "target": 16328288,
          "owner": 5232400,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xa9489e]",
          "kind": "rip"
        },
        {
          "source": 5234148,
          "target": 5234204,
          "owner": 5232400,
          "mnemonic": "jb",
          "operands": "0x212c37fde1c",
          "kind": "branch"
        },
        {
          "source": 5234154,
          "target": 5234207,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fde1f",
          "kind": "branch"
        },
        {
          "source": 5234210,
          "target": 5234375,
          "owner": 5232400,
          "mnemonic": "js",
          "operands": "0x212c37fdec7",
          "kind": "branch"
        },
        {
          "source": 5234220,
          "target": 5234245,
          "owner": 5232400,
          "mnemonic": "ja",
          "operands": "0x212c37fde45",
          "kind": "branch"
        },
        {
          "source": 5234243,
          "target": 5234184,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fde08",
          "kind": "branch"
        },
        {
          "source": 5234265,
          "target": 16328472,
          "owner": 5232400,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa948b8]",
          "kind": "rip"
        },
        {
          "source": 5234289,
          "target": 5234322,
          "owner": 5232400,
          "mnemonic": "jb",
          "operands": "0x212c37fde92",
          "kind": "branch"
        },
        {
          "source": 5234303,
          "target": 5234383,
          "owner": 5232400,
          "mnemonic": "je",
          "operands": "0x212c37fdecf",
          "kind": "branch"
        },
        {
          "source": 5234320,
          "target": 5234334,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c37fde9e",
          "kind": "branch"
        },
        {
          "source": 5234370,
          "target": 14441760,
          "owner": 5232400,
          "mnemonic": "jmp",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5234378,
          "target": 925984,
          "owner": 5232400,
          "mnemonic": "call",
          "operands": "0x212c33e2120",
          "kind": "branch"
        }
      ],
      "incoming_truncated": false,
      "incoming": [
        {
          "source": 5232071,
          "target": 5232400,
          "owner": 5230960,
          "mnemonic": "call",
          "operands": "0x212c37fd710",
          "kind": "branch"
        }
      ]
    },
    {
      "va": "0x212c380fb10",
      "rva": "0x50fb10",
      "metadata": {
        "begin": 5307152,
        "end": 5312510,
        "unwind": 17242856,
        "table_rva": 66553540,
        "decoded_end": 5309880,
        "instruction_count": 587
      },
      "refs_truncated": false,
      "refs": [
        {
          "source": 5307253,
          "target": 5315920,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c3811d50",
          "kind": "branch"
        },
        {
          "source": 5307288,
          "target": 5307300,
          "owner": 5307152,
          "mnemonic": "ja",
          "operands": "0x212c380fba4",
          "kind": "branch"
        },
        {
          "source": 5307290,
          "target": 14600344,
          "owner": 5307152,
          "mnemonic": "vucomiss",
          "operands": "xmm0, dword ptr [rip + 0x8dccf6]",
          "kind": "rip"
        },
        {
          "source": 5307298,
          "target": 5307372,
          "owner": 5307152,
          "mnemonic": "jb",
          "operands": "0x212c380fbec",
          "kind": "branch"
        },
        {
          "source": 5307309,
          "target": 16328424,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xa82b35]",
          "kind": "rip"
        },
        {
          "source": 5307319,
          "target": 5307354,
          "owner": 5307152,
          "mnemonic": "jbe",
          "operands": "0x212c380fbda",
          "kind": "branch"
        },
        {
          "source": 5307321,
          "target": 14600344,
          "owner": 5307152,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0x8dccd7]",
          "kind": "rip"
        },
        {
          "source": 5307342,
          "target": 5307361,
          "owner": 5307152,
          "mnemonic": "jae",
          "operands": "0x212c380fbe1",
          "kind": "branch"
        },
        {
          "source": 5307344,
          "target": 14707388,
          "owner": 5307152,
          "mnemonic": "vaddss",
          "operands": "xmm1, xmm1, dword ptr [rip + 0x8f6ee4]",
          "kind": "rip"
        },
        {
          "source": 5307352,
          "target": 5307365,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c380fbe5",
          "kind": "branch"
        },
        {
          "source": 5307359,
          "target": 5307365,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c380fbe5",
          "kind": "branch"
        },
        {
          "source": 5307384,
          "target": 5307415,
          "owner": 5307152,
          "mnemonic": "jbe",
          "operands": "0x212c380fc17",
          "kind": "branch"
        },
        {
          "source": 5307386,
          "target": 14600344,
          "owner": 5307152,
          "mnemonic": "vmovss",
          "operands": "xmm2, dword ptr [rip + 0x8dcc96]",
          "kind": "rip"
        },
        {
          "source": 5307402,
          "target": 5307436,
          "owner": 5307152,
          "mnemonic": "jae",
          "operands": "0x212c380fc2c",
          "kind": "branch"
        },
        {
          "source": 5307411,
          "target": 5307424,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c380fc20",
          "kind": "branch"
        },
        {
          "source": 5307413,
          "target": 5307445,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c380fc35",
          "kind": "branch"
        },
        {
          "source": 5307422,
          "target": 5307445,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c380fc35",
          "kind": "branch"
        },
        {
          "source": 5307424,
          "target": 15306564,
          "owner": 5307152,
          "mnemonic": "vucomiss",
          "operands": "xmm1, dword ptr [rip + 0x98931c]",
          "kind": "rip"
        },
        {
          "source": 5307432,
          "target": 5307450,
          "owner": 5307152,
          "mnemonic": "jbe",
          "operands": "0x212c380fc3a",
          "kind": "branch"
        },
        {
          "source": 5307434,
          "target": 5307498,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c380fc6a",
          "kind": "branch"
        },
        {
          "source": 5307443,
          "target": 5307450,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c380fc3a",
          "kind": "branch"
        },
        {
          "source": 5307448,
          "target": 5307501,
          "owner": 5307152,
          "mnemonic": "jl",
          "operands": "0x212c380fc6d",
          "kind": "branch"
        },
        {
          "source": 5307456,
          "target": 5307481,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c380fc59",
          "kind": "branch"
        },
        {
          "source": 5307458,
          "target": 15306564,
          "owner": 5307152,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0x9892fa]",
          "kind": "rip"
        },
        {
          "source": 5307477,
          "target": 5307490,
          "owner": 5307152,
          "mnemonic": "jbe",
          "operands": "0x212c380fc62",
          "kind": "branch"
        },
        {
          "source": 5307479,
          "target": 5307501,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c380fc6d",
          "kind": "branch"
        },
        {
          "source": 5307488,
          "target": 5307501,
          "owner": 5307152,
          "mnemonic": "jg",
          "operands": "0x212c380fc6d",
          "kind": "branch"
        },
        {
          "source": 5307490,
          "target": 15306564,
          "owner": 5307152,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0x9892da]",
          "kind": "rip"
        },
        {
          "source": 5307514,
          "target": 5307641,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c380fcf9",
          "kind": "branch"
        },
        {
          "source": 5307525,
          "target": 5307665,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c380fd11",
          "kind": "branch"
        },
        {
          "source": 5307596,
          "target": 5307679,
          "owner": 5307152,
          "mnemonic": "jae",
          "operands": "0x212c380fd1f",
          "kind": "branch"
        },
        {
          "source": 5307608,
          "target": 15306448,
          "owner": 5307152,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0x9891f0]",
          "kind": "rip"
        },
        {
          "source": 5307616,
          "target": 15305324,
          "owner": 5307152,
          "mnemonic": "vmovss",
          "operands": "xmm2, dword ptr [rip + 0x988d84]",
          "kind": "rip"
        },
        {
          "source": 5307636,
          "target": 5307807,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c380fd9f",
          "kind": "branch"
        },
        {
          "source": 5307659,
          "target": 5307531,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c380fc8b",
          "kind": "branch"
        },
        {
          "source": 5307674,
          "target": 5308438,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c3810016",
          "kind": "branch"
        },
        {
          "source": 5307683,
          "target": 16328392,
          "owner": 5307152,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0xa8299e]",
          "kind": "rip"
        },
        {
          "source": 5307720,
          "target": 14601712,
          "owner": 5307152,
          "mnemonic": "vxorps",
          "operands": "xmm0, xmm11, xmmword ptr [rip + 0x8dd0a0]",
          "kind": "rip"
        },
        {
          "source": 5307736,
          "target": 15306416,
          "owner": 5307152,
          "mnemonic": "vmulps",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x989150]",
          "kind": "rip"
        },
        {
          "source": 5307744,
          "target": 15306432,
          "owner": 5307152,
          "mnemonic": "vdivps",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x989158]",
          "kind": "rip"
        },
        {
          "source": 5307773,
          "target": 5307783,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c380fd87",
          "kind": "branch"
        },
        {
          "source": 5307775,
          "target": 14707384,
          "owner": 5307152,
          "mnemonic": "vaddss",
          "operands": "xmm1, xmm1, dword ptr [rip + 0x8f6d31]",
          "kind": "rip"
        },
        {
          "source": 5307791,
          "target": 5307807,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c380fd9f",
          "kind": "branch"
        },
        {
          "source": 5307793,
          "target": 14707384,
          "owner": 5307152,
          "mnemonic": "vaddss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x8f6d1f]",
          "kind": "rip"
        },
        {
          "source": 5307807,
          "target": 15304976,
          "owner": 5307152,
          "mnemonic": "vmulps",
          "operands": "xmm0, xmm1, xmmword ptr [rip + 0x988b69]",
          "kind": "rip"
        },
        {
          "source": 5307815,
          "target": 15291248,
          "owner": 5307152,
          "mnemonic": "vaddps",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x9855c1]",
          "kind": "rip"
        },
        {
          "source": 5307829,
          "target": 15304992,
          "owner": 5307152,
          "mnemonic": "vmulps",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x988b63]",
          "kind": "rip"
        },
        {
          "source": 5307907,
          "target": 3372448,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c36375a0",
          "kind": "branch"
        },
        {
          "source": 5307944,
          "target": 15772680,
          "owner": 5307152,
          "mnemonic": "lea",
          "operands": "rdx, [rip + 0x9fadd9]",
          "kind": "rip"
        },
        {
          "source": 5307974,
          "target": 3372448,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c36375a0",
          "kind": "branch"
        },
        {
          "source": 5308008,
          "target": 5308019,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c380fe73",
          "kind": "branch"
        },
        {
          "source": 5308017,
          "target": 5308032,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c380fe80",
          "kind": "branch"
        },
        {
          "source": 5308027,
          "target": 5308424,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c3810008",
          "kind": "branch"
        },
        {
          "source": 5308036,
          "target": 14714456,
          "owner": 5307152,
          "mnemonic": "vmulss",
          "operands": "xmm6, xmm9, dword ptr [rip + 0x8f87cc]",
          "kind": "rip"
        },
        {
          "source": 5308048,
          "target": 14232416,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c4092b60",
          "kind": "branch"
        },
        {
          "source": 5308061,
          "target": 14265312,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c409abe0",
          "kind": "branch"
        },
        {
          "source": 5308070,
          "target": 14219264,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c408f800",
          "kind": "branch"
        },
        {
          "source": 5308075,
          "target": 24454792,
          "owner": 5307152,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12427d6]",
          "kind": "rip"
        },
        {
          "source": 5308082,
          "target": 24454784,
          "owner": 5307152,
          "mnemonic": "mov",
          "operands": "r14, qword ptr [rip + 0x12427c7]",
          "kind": "rip"
        },
        {
          "source": 5308102,
          "target": 5308205,
          "owner": 5307152,
          "mnemonic": "jle",
          "operands": "0x212c380ff2d",
          "kind": "branch"
        },
        {
          "source": 5308108,
          "target": 14714456,
          "owner": 5307152,
          "mnemonic": "vmovss",
          "operands": "xmm10, dword ptr [rip + 0x8f8784]",
          "kind": "rip"
        },
        {
          "source": 5308119,
          "target": 5308136,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c380fee8",
          "kind": "branch"
        },
        {
          "source": 5308134,
          "target": 5308198,
          "owner": 5307152,
          "mnemonic": "jle",
          "operands": "0x212c380ff26",
          "kind": "branch"
        },
        {
          "source": 5308149,
          "target": 14232416,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c4092b60",
          "kind": "branch"
        },
        {
          "source": 5308162,
          "target": 14265312,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c409abe0",
          "kind": "branch"
        },
        {
          "source": 5308171,
          "target": 14219264,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c408f800",
          "kind": "branch"
        },
        {
          "source": 5308180,
          "target": 5308128,
          "owner": 5307152,
          "mnemonic": "jbe",
          "operands": "0x212c380fee0",
          "kind": "branch"
        },
        {
          "source": 5308196,
          "target": 5308128,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c380fee0",
          "kind": "branch"
        },
        {
          "source": 5308198,
          "target": 24454792,
          "owner": 5307152,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x124275b]",
          "kind": "rip"
        },
        {
          "source": 5308208,
          "target": 5308217,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c380ff39",
          "kind": "branch"
        },
        {
          "source": 5308215,
          "target": 5308222,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c380ff3e",
          "kind": "branch"
        },
        {
          "source": 5308227,
          "target": 14714456,
          "owner": 5307152,
          "mnemonic": "vmulss",
          "operands": "xmm6, xmm9, dword ptr [rip + 0x8f870d]",
          "kind": "rip"
        },
        {
          "source": 5308239,
          "target": 14232416,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c4092b60",
          "kind": "branch"
        },
        {
          "source": 5308252,
          "target": 14265312,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c409abe0",
          "kind": "branch"
        },
        {
          "source": 5308261,
          "target": 14219264,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c408f800",
          "kind": "branch"
        },
        {
          "source": 5308266,
          "target": 24454792,
          "owner": 5307152,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1242717]",
          "kind": "rip"
        },
        {
          "source": 5308273,
          "target": 24454784,
          "owner": 5307152,
          "mnemonic": "mov",
          "operands": "r14, qword ptr [rip + 0x1242708]",
          "kind": "rip"
        },
        {
          "source": 5308293,
          "target": 5308397,
          "owner": 5307152,
          "mnemonic": "jle",
          "operands": "0x212c380ffed",
          "kind": "branch"
        },
        {
          "source": 5308299,
          "target": 14714456,
          "owner": 5307152,
          "mnemonic": "vmovss",
          "operands": "xmm12, dword ptr [rip + 0x8f86c5]",
          "kind": "rip"
        },
        {
          "source": 5308310,
          "target": 5308328,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c380ffa8",
          "kind": "branch"
        },
        {
          "source": 5308326,
          "target": 5308390,
          "owner": 5307152,
          "mnemonic": "jle",
          "operands": "0x212c380ffe6",
          "kind": "branch"
        },
        {
          "source": 5308341,
          "target": 14232416,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c4092b60",
          "kind": "branch"
        },
        {
          "source": 5308354,
          "target": 14265312,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c409abe0",
          "kind": "branch"
        },
        {
          "source": 5308363,
          "target": 14219264,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c408f800",
          "kind": "branch"
        },
        {
          "source": 5308372,
          "target": 5308320,
          "owner": 5307152,
          "mnemonic": "jbe",
          "operands": "0x212c380ffa0",
          "kind": "branch"
        },
        {
          "source": 5308388,
          "target": 5308320,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c380ffa0",
          "kind": "branch"
        },
        {
          "source": 5308390,
          "target": 24454792,
          "owner": 5307152,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x124269b]",
          "kind": "rip"
        },
        {
          "source": 5308404,
          "target": 5308411,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c380fffb",
          "kind": "branch"
        },
        {
          "source": 5308431,
          "target": 5308438,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c3810016",
          "kind": "branch"
        },
        {
          "source": 5308452,
          "target": 5308483,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c3810043",
          "kind": "branch"
        },
        {
          "source": 5308468,
          "target": 3183872,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c3609500",
          "kind": "branch"
        },
        {
          "source": 5308477,
          "target": 5308483,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c3810043",
          "kind": "branch"
        },
        {
          "source": 5308504,
          "target": 5308575,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c381009f",
          "kind": "branch"
        },
        {
          "source": 5308513,
          "target": 5308567,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c3810097",
          "kind": "branch"
        },
        {
          "source": 5308565,
          "target": 5308575,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c381009f",
          "kind": "branch"
        },
        {
          "source": 5308570,
          "target": 3199888,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c360d390",
          "kind": "branch"
        },
        {
          "source": 5308584,
          "target": 5308618,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c38100ca",
          "kind": "branch"
        },
        {
          "source": 5308616,
          "target": 5308647,
          "owner": 5307152,
          "mnemonic": "jl",
          "operands": "0x212c38100e7",
          "kind": "branch"
        },
        {
          "source": 5308660,
          "target": 24473296,
          "owner": 5307152,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x1246dd5]",
          "kind": "rip"
        },
        {
          "source": 5308670,
          "target": 5308694,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c3810116",
          "kind": "branch"
        },
        {
          "source": 5308672,
          "target": 24473288,
          "owner": 5307152,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1246dc1]",
          "kind": "rip"
        },
        {
          "source": 5308682,
          "target": 3178736,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "0x212c36080f0",
          "kind": "branch"
        },
        {
          "source": 5308687,
          "target": 24473296,
          "owner": 5307152,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x1246dba]",
          "kind": "rip"
        },
        {
          "source": 5308713,
          "target": 14600344,
          "owner": 5307152,
          "mnemonic": "vmovss",
          "operands": "xmm11, dword ptr [rip + 0x8dc767]",
          "kind": "rip"
        },
        {
          "source": 5308737,
          "target": 5308747,
          "owner": 5307152,
          "mnemonic": "jb",
          "operands": "0x212c381014b",
          "kind": "branch"
        },
        {
          "source": 5308739,
          "target": 14707388,
          "owner": 5307152,
          "mnemonic": "vaddss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x8f6971]",
          "kind": "rip"
        },
        {
          "source": 5308766,
          "target": 5308773,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c3810165",
          "kind": "branch"
        },
        {
          "source": 5308782,
          "target": 5308789,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c3810175",
          "kind": "branch"
        },
        {
          "source": 5308862,
          "target": 5308916,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c38101f4",
          "kind": "branch"
        },
        {
          "source": 5308923,
          "target": 5309091,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c38102a3",
          "kind": "branch"
        },
        {
          "source": 5308940,
          "target": 5309254,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c3810346",
          "kind": "branch"
        },
        {
          "source": 5308953,
          "target": 24478356,
          "owner": 5307152,
          "mnemonic": "movsxd",
          "operands": "rcx, dword ptr [rip + 0x1248074]",
          "kind": "rip"
        },
        {
          "source": 5308976,
          "target": 15304532,
          "owner": 5307152,
          "mnemonic": "vminss",
          "operands": "xmm7, xmm0, dword ptr [rip + 0x98851c]",
          "kind": "rip"
        },
        {
          "source": 5308984,
          "target": 15317300,
          "owner": 5307152,
          "mnemonic": "vaddss",
          "operands": "xmm0, xmm7, dword ptr [rip + 0x98b6f4]",
          "kind": "rip"
        },
        {
          "source": 5309021,
          "target": 5309033,
          "owner": 5307152,
          "mnemonic": "ja",
          "operands": "0x212c3810269",
          "kind": "branch"
        },
        {
          "source": 5309023,
          "target": 14600344,
          "owner": 5307152,
          "mnemonic": "vucomiss",
          "operands": "xmm6, dword ptr [rip + 0x8dc631]",
          "kind": "rip"
        },
        {
          "source": 5309031,
          "target": 5309117,
          "owner": 5307152,
          "mnemonic": "jb",
          "operands": "0x212c38102bd",
          "kind": "branch"
        },
        {
          "source": 5309049,
          "target": 16328424,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xa82469]",
          "kind": "rip"
        },
        {
          "source": 5309063,
          "target": 5309099,
          "owner": 5307152,
          "mnemonic": "jbe",
          "operands": "0x212c38102ab",
          "kind": "branch"
        },
        {
          "source": 5309079,
          "target": 5309106,
          "owner": 5307152,
          "mnemonic": "jae",
          "operands": "0x212c38102b2",
          "kind": "branch"
        },
        {
          "source": 5309081,
          "target": 14707388,
          "owner": 5307152,
          "mnemonic": "vaddss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x8f681b]",
          "kind": "rip"
        },
        {
          "source": 5309089,
          "target": 5309110,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c38102b6",
          "kind": "branch"
        },
        {
          "source": 5309094,
          "target": 5309332,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c3810394",
          "kind": "branch"
        },
        {
          "source": 5309104,
          "target": 5309110,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c38102b6",
          "kind": "branch"
        },
        {
          "source": 5309128,
          "target": 16328280,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xa8238a]",
          "kind": "rip"
        },
        {
          "source": 5309134,
          "target": 15042616,
          "owner": 5307152,
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x948562]",
          "kind": "rip"
        },
        {
          "source": 5309151,
          "target": 16328424,
          "owner": 5307152,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xa82403]",
          "kind": "rip"
        },
        {
          "source": 5309165,
          "target": 5309193,
          "owner": 5307152,
          "mnemonic": "jbe",
          "operands": "0x212c3810309",
          "kind": "branch"
        },
        {
          "source": 5309181,
          "target": 5309200,
          "owner": 5307152,
          "mnemonic": "jae",
          "operands": "0x212c3810310",
          "kind": "branch"
        },
        {
          "source": 5309183,
          "target": 14707388,
          "owner": 5307152,
          "mnemonic": "vaddss",
          "operands": "xmm1, xmm1, dword ptr [rip + 0x8f67b5]",
          "kind": "rip"
        },
        {
          "source": 5309191,
          "target": 5309204,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c3810314",
          "kind": "branch"
        },
        {
          "source": 5309198,
          "target": 5309204,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c3810314",
          "kind": "branch"
        },
        {
          "source": 5309229,
          "target": 5309239,
          "owner": 5307152,
          "mnemonic": "jb",
          "operands": "0x212c3810337",
          "kind": "branch"
        },
        {
          "source": 5309231,
          "target": 14707388,
          "owner": 5307152,
          "mnemonic": "vaddss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x8f6785]",
          "kind": "rip"
        },
        {
          "source": 5309254,
          "target": 15304976,
          "owner": 5307152,
          "mnemonic": "vmulps",
          "operands": "xmm0, xmm9, xmmword ptr [rip + 0x9885c2]",
          "kind": "rip"
        },
        {
          "source": 5309262,
          "target": 15291248,
          "owner": 5307152,
          "mnemonic": "vaddps",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x98501a]",
          "kind": "rip"
        },
        {
          "source": 5309276,
          "target": 15304992,
          "owner": 5307152,
          "mnemonic": "vmulps",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x9885bc]",
          "kind": "rip"
        },
        {
          "source": 5309288,
          "target": 15305008,
          "owner": 5307152,
          "mnemonic": "vmulss",
          "operands": "xmm1, xmm10, dword ptr [rip + 0x9885c0]",
          "kind": "rip"
        },
        {
          "source": 5309301,
          "target": 14600152,
          "owner": 5307152,
          "mnemonic": "vaddss",
          "operands": "xmm0, xmm1, dword ptr [rip + 0x8dc45b]",
          "kind": "rip"
        },
        {
          "source": 5309315,
          "target": 15305012,
          "owner": 5307152,
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x9885a9]",
          "kind": "rip"
        },
        {
          "source": 5309332,
          "target": 24591020,
          "owner": 5307152,
          "mnemonic": "lea",
          "operands": "r14, [rip + 0x1263711]",
          "kind": "rip"
        },
        {
          "source": 5309339,
          "target": 24482732,
          "owner": 5307152,
          "mnemonic": "cmp",
          "operands": "byte ptr [rip + 0x124900a], 0",
          "kind": "rip"
        },
        {
          "source": 5309346,
          "target": 5309414,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c38103e6",
          "kind": "branch"
        },
        {
          "source": 5309348,
          "target": 24482816,
          "owner": 5307152,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x1249055]",
          "kind": "rip"
        },
        {
          "source": 5309358,
          "target": 5309394,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c38103d2",
          "kind": "branch"
        },
        {
          "source": 5309379,
          "target": 5309388,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c38103cc",
          "kind": "branch"
        },
        {
          "source": 5309384,
          "target": 5309414,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c38103e6",
          "kind": "branch"
        },
        {
          "source": 5309386,
          "target": 5309394,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c38103d2",
          "kind": "branch"
        },
        {
          "source": 5309392,
          "target": 5309414,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c38103e6",
          "kind": "branch"
        },
        {
          "source": 5309394,
          "target": 24483138,
          "owner": 5307152,
          "mnemonic": "cmp",
          "operands": "byte ptr [rip + 0x1249169], 0",
          "kind": "rip"
        },
        {
          "source": 5309401,
          "target": 5309545,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c3810469",
          "kind": "branch"
        },
        {
          "source": 5309407,
          "target": 24483137,
          "owner": 5307152,
          "mnemonic": "lea",
          "operands": "r14, [rip + 0x124915b]",
          "kind": "rip"
        },
        {
          "source": 5309418,
          "target": 5311410,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c3810bb2",
          "kind": "branch"
        },
        {
          "source": 5309431,
          "target": 5309812,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c3810574",
          "kind": "branch"
        },
        {
          "source": 5309444,
          "target": 5309812,
          "owner": 5307152,
          "mnemonic": "jne",
          "operands": "0x212c3810574",
          "kind": "branch"
        },
        {
          "source": 5309500,
          "target": 5309610,
          "owner": 5307152,
          "mnemonic": "jae",
          "operands": "0x212c38104aa",
          "kind": "branch"
        },
        {
          "source": 5309512,
          "target": 15306448,
          "owner": 5307152,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0x988a80]",
          "kind": "rip"
        },
        {
          "source": 5309520,
          "target": 15305324,
          "owner": 5307152,
          "mnemonic": "vmovss",
          "operands": "xmm2, dword ptr [rip + 0x988614]",
          "kind": "rip"
        },
        {
          "source": 5309540,
          "target": 5309747,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c3810533",
          "kind": "branch"
        },
        {
          "source": 5309545,
          "target": 24483024,
          "owner": 5307152,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1249060]",
          "kind": "rip"
        },
        {
          "source": 5309556,
          "target": 24483016,
          "owner": 5307152,
          "mnemonic": "cmp",
          "operands": "qword ptr [rip + 0x124904d], rax",
          "kind": "rip"
        },
        {
          "source": 5309563,
          "target": 24483008,
          "owner": 5307152,
          "mnemonic": "lea",
          "operands": "rax, [rip + 0x124903e]",
          "kind": "rip"
        },
        {
          "source": 5309584,
          "target": 24483136,
          "owner": 5307152,
          "mnemonic": "lea",
          "operands": "r14, [rip + 0x12490a9]",
          "kind": "rip"
        },
        {
          "source": 5309599,
          "target": 5309424,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c38103f0",
          "kind": "branch"
        },
        {
          "source": 5309605,
          "target": 5311410,
          "owner": 5307152,
          "mnemonic": "jmp",
          "operands": "0x212c3810bb2",
          "kind": "branch"
        },
        {
          "source": 5309617,
          "target": 16328392,
          "owner": 5307152,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0xa82210]",
          "kind": "rip"
        },
        {
          "source": 5309657,
          "target": 14601712,
          "owner": 5307152,
          "mnemonic": "vxorps",
          "operands": "xmm0, xmm10, xmmword ptr [rip + 0x8dc90f]",
          "kind": "rip"
        },
        {
          "source": 5309673,
          "target": 15306416,
          "owner": 5307152,
          "mnemonic": "vmulps",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x9889bf]",
          "kind": "rip"
        },
        {
          "source": 5309681,
          "target": 15306432,
          "owner": 5307152,
          "mnemonic": "vdivps",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x9889c7]",
          "kind": "rip"
        },
        {
          "source": 5309710,
          "target": 5309720,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c3810518",
          "kind": "branch"
        },
        {
          "source": 5309712,
          "target": 14707384,
          "owner": 5307152,
          "mnemonic": "vaddss",
          "operands": "xmm1, xmm1, dword ptr [rip + 0x8f65a0]",
          "kind": "rip"
        },
        {
          "source": 5309731,
          "target": 5309747,
          "owner": 5307152,
          "mnemonic": "je",
          "operands": "0x212c3810533",
          "kind": "branch"
        },
        {
          "source": 5309733,
          "target": 14707384,
          "owner": 5307152,
          "mnemonic": "vaddss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x8f658b]",
          "kind": "rip"
        },
        {
          "source": 5309747,
          "target": 15304976,
          "owner": 5307152,
          "mnemonic": "vmulps",
          "operands": "xmm0, xmm1, xmmword ptr [rip + 0x9883d5]",
          "kind": "rip"
        },
        {
          "source": 5309755,
          "target": 15291248,
          "owner": 5307152,
          "mnemonic": "vaddps",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x984e2d]",
          "kind": "rip"
        },
        {
          "source": 5309769,
          "target": 15304992,
          "owner": 5307152,
          "mnemonic": "vmulps",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x9883cf]",
          "kind": "rip"
        },
        {
          "source": 5309781,
          "target": 15317312,
          "owner": 5307152,
          "mnemonic": "vmovsd",
          "operands": "xmm1, qword ptr [rip + 0x98b3e3]",
          "kind": "rip"
        },
        {
          "source": 5309789,
          "target": 15317328,
          "owner": 5307152,
          "mnemonic": "vmovddup",
          "operands": "xmm2, qword ptr [rip + 0x98b3eb]",
          "kind": "rip"
        }
      ],
      "incoming_truncated": false,
      "incoming": [
        {
          "source": 5269383,
          "target": 5307152,
          "owner": 5267280,
          "mnemonic": "call",
          "operands": "0x212c380fb10",
          "kind": "branch"
        }
      ]
    },
    {
      "va": "0x212c384bc60",
      "rva": "0x54bc60",
      "metadata": {
        "begin": 5553248,
        "end": 5562450,
        "unwind": 17267712,
        "table_rva": 66559456,
        "decoded_end": 5561120,
        "instruction_count": 1663
      },
      "refs_truncated": false,
      "refs": [
        {
          "source": 5553298,
          "target": 5562387,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384e013",
          "kind": "branch"
        },
        {
          "source": 5553314,
          "target": 5562387,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384e013",
          "kind": "branch"
        },
        {
          "source": 5553320,
          "target": 24473608,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0x120b359]",
          "kind": "rip"
        },
        {
          "source": 5553330,
          "target": 5553351,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384bcc7",
          "kind": "branch"
        },
        {
          "source": 5553332,
          "target": 24473600,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x120b345]",
          "kind": "rip"
        },
        {
          "source": 5553339,
          "target": 3162208,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3604060",
          "kind": "branch"
        },
        {
          "source": 5553344,
          "target": 24473608,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0x120b341]",
          "kind": "rip"
        },
        {
          "source": 5553351,
          "target": 15318960,
          "owner": 5553248,
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rip + 0x9502e1]",
          "kind": "rip"
        },
        {
          "source": 5553359,
          "target": 14707388,
          "owner": 5553248,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0x8bade5]",
          "kind": "rip"
        },
        {
          "source": 5553367,
          "target": 16328280,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xa4697b]",
          "kind": "rip"
        },
        {
          "source": 5553389,
          "target": 15305316,
          "owner": 5553248,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0x94cd6f]",
          "kind": "rip"
        },
        {
          "source": 5553401,
          "target": 5553433,
          "owner": 5553248,
          "mnemonic": "jbe",
          "operands": "0x212c384bd19",
          "kind": "branch"
        },
        {
          "source": 5553403,
          "target": 14707352,
          "owner": 5553248,
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rip + 0x8bad95]",
          "kind": "rip"
        },
        {
          "source": 5553411,
          "target": 15301708,
          "owner": 5553248,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0x94bf41]",
          "kind": "rip"
        },
        {
          "source": 5553419,
          "target": 16328280,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "qword ptr [rip + 0xa46947]",
          "kind": "rip"
        },
        {
          "source": 5553433,
          "target": 16328280,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rbx, qword ptr [rip + 0xa46938]",
          "kind": "rip"
        },
        {
          "source": 5553440,
          "target": 15306716,
          "owner": 5553248,
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rip + 0x94d2b4]",
          "kind": "rip"
        },
        {
          "source": 5553448,
          "target": 14953972,
          "owner": 5553248,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0x8f70c4]",
          "kind": "rip"
        },
        {
          "source": 5553466,
          "target": 15305316,
          "owner": 5553248,
          "mnemonic": "vmovss",
          "operands": "xmm0, dword ptr [rip + 0x94cd22]",
          "kind": "rip"
        },
        {
          "source": 5553474,
          "target": 14707364,
          "owner": 5553248,
          "mnemonic": "vmovss",
          "operands": "xmm1, dword ptr [rip + 0x8bad5a]",
          "kind": "rip"
        },
        {
          "source": 5553492,
          "target": 24546408,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0x121cf0d]",
          "kind": "rip"
        },
        {
          "source": 5553499,
          "target": 24591020,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rbx, [rip + 0x1227d4a]",
          "kind": "rip"
        },
        {
          "source": 5553513,
          "target": 5553638,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384bde6",
          "kind": "branch"
        },
        {
          "source": 5553525,
          "target": 5553567,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384bd9f",
          "kind": "branch"
        },
        {
          "source": 5553549,
          "target": 5553558,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384bd96",
          "kind": "branch"
        },
        {
          "source": 5553554,
          "target": 5553638,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384bde6",
          "kind": "branch"
        },
        {
          "source": 5553556,
          "target": 5553567,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384bd9f",
          "kind": "branch"
        },
        {
          "source": 5553565,
          "target": 5553638,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384bde6",
          "kind": "branch"
        },
        {
          "source": 5553574,
          "target": 5553585,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384bdb1",
          "kind": "branch"
        },
        {
          "source": 5553583,
          "target": 5553635,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384bde3",
          "kind": "branch"
        },
        {
          "source": 5553641,
          "target": 5553728,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384be40",
          "kind": "branch"
        },
        {
          "source": 5553651,
          "target": 5553728,
          "owner": 5553248,
          "mnemonic": "jle",
          "operands": "0x212c384be40",
          "kind": "branch"
        },
        {
          "source": 5553653,
          "target": 24573400,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x12237dc]",
          "kind": "rip"
        },
        {
          "source": 5553663,
          "target": 5553690,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384be1a",
          "kind": "branch"
        },
        {
          "source": 5553665,
          "target": 24573392,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x12237c8]",
          "kind": "rip"
        },
        {
          "source": 5553672,
          "target": 2890096,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c35c1970",
          "kind": "branch"
        },
        {
          "source": 5553677,
          "target": 24573400,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x12237c4]",
          "kind": "rip"
        },
        {
          "source": 5553716,
          "target": 24479112,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rdx, qword ptr [rip + 0x120c74d]",
          "kind": "rip"
        },
        {
          "source": 5553723,
          "target": 5886736,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c389d310",
          "kind": "branch"
        },
        {
          "source": 5553778,
          "target": 19397441,
          "owner": 5553248,
          "mnemonic": "movzx",
          "operands": "ecx, byte ptr [rip + 0xd33cc8]",
          "kind": "rip"
        },
        {
          "source": 5553790,
          "target": 19397442,
          "owner": 5553248,
          "mnemonic": "movzx",
          "operands": "ecx, byte ptr [rip + 0xd33cbd]",
          "kind": "rip"
        },
        {
          "source": 5553824,
          "target": 14444896,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c6960",
          "kind": "branch"
        },
        {
          "source": 5553832,
          "target": 5562439,
          "owner": 5553248,
          "mnemonic": "js",
          "operands": "0x212c384e047",
          "kind": "branch"
        },
        {
          "source": 5553845,
          "target": 5553897,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384bee9",
          "kind": "branch"
        },
        {
          "source": 5553879,
          "target": 14441760,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5553892,
          "target": 5554025,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384bf69",
          "kind": "branch"
        },
        {
          "source": 5553917,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa46814]",
          "kind": "rip"
        },
        {
          "source": 5553941,
          "target": 5553975,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384bf37",
          "kind": "branch"
        },
        {
          "source": 5553952,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5553973,
          "target": 5553984,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384bf40",
          "kind": "branch"
        },
        {
          "source": 5554015,
          "target": 14441760,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5554037,
          "target": 5562317,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384dfcd",
          "kind": "branch"
        },
        {
          "source": 5554043,
          "target": 16015808,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x9fa23e]",
          "kind": "rip"
        },
        {
          "source": 5554066,
          "target": 5555409,
          "owner": 5553248,
          "mnemonic": "jle",
          "operands": "0x212c384c4d1",
          "kind": "branch"
        },
        {
          "source": 5554072,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1227ae9]",
          "kind": "rip"
        },
        {
          "source": 5554082,
          "target": 5554103,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384bfb7",
          "kind": "branch"
        },
        {
          "source": 5554084,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1227ad5]",
          "kind": "rip"
        },
        {
          "source": 5554091,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5554096,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1227ad1]",
          "kind": "rip"
        },
        {
          "source": 5554143,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5554156,
          "target": 5554165,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384bff5",
          "kind": "branch"
        },
        {
          "source": 5554175,
          "target": 1713120,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34a23e0",
          "kind": "branch"
        },
        {
          "source": 5554180,
          "target": 5556733,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c9fd",
          "kind": "branch"
        },
        {
          "source": 5554185,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1227a78]",
          "kind": "rip"
        },
        {
          "source": 5554195,
          "target": 5554216,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c028",
          "kind": "branch"
        },
        {
          "source": 5554197,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1227a64]",
          "kind": "rip"
        },
        {
          "source": 5554204,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5554209,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1227a60]",
          "kind": "rip"
        },
        {
          "source": 5554256,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5554269,
          "target": 5554278,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c066",
          "kind": "branch"
        },
        {
          "source": 5554281,
          "target": 14444896,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c6960",
          "kind": "branch"
        },
        {
          "source": 5554296,
          "target": 5555277,
          "owner": 5553248,
          "mnemonic": "jbe",
          "operands": "0x212c384c44d",
          "kind": "branch"
        },
        {
          "source": 5554315,
          "target": 1172496,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c341e410",
          "kind": "branch"
        },
        {
          "source": 5554320,
          "target": 5555697,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c5f1",
          "kind": "branch"
        },
        {
          "source": 5554325,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12279ec]",
          "kind": "rip"
        },
        {
          "source": 5554335,
          "target": 5554356,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c0b4",
          "kind": "branch"
        },
        {
          "source": 5554337,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x12279d8]",
          "kind": "rip"
        },
        {
          "source": 5554344,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5554349,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12279d4]",
          "kind": "rip"
        },
        {
          "source": 5554396,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5554409,
          "target": 5554418,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c0f2",
          "kind": "branch"
        },
        {
          "source": 5554421,
          "target": 14444896,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c6960",
          "kind": "branch"
        },
        {
          "source": 5554436,
          "target": 5555299,
          "owner": 5553248,
          "mnemonic": "jbe",
          "operands": "0x212c384c463",
          "kind": "branch"
        },
        {
          "source": 5554455,
          "target": 1172496,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c341e410",
          "kind": "branch"
        },
        {
          "source": 5554460,
          "target": 5555816,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c668",
          "kind": "branch"
        },
        {
          "source": 5554465,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1227960]",
          "kind": "rip"
        },
        {
          "source": 5554475,
          "target": 5554496,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c140",
          "kind": "branch"
        },
        {
          "source": 5554477,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x122794c]",
          "kind": "rip"
        },
        {
          "source": 5554484,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5554489,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1227948]",
          "kind": "rip"
        },
        {
          "source": 5554536,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5554549,
          "target": 5554558,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c17e",
          "kind": "branch"
        },
        {
          "source": 5554561,
          "target": 14444896,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c6960",
          "kind": "branch"
        },
        {
          "source": 5554576,
          "target": 5555321,
          "owner": 5553248,
          "mnemonic": "jbe",
          "operands": "0x212c384c479",
          "kind": "branch"
        },
        {
          "source": 5554595,
          "target": 1172496,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c341e410",
          "kind": "branch"
        },
        {
          "source": 5554600,
          "target": 5555935,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c6df",
          "kind": "branch"
        },
        {
          "source": 5554605,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12278d4]",
          "kind": "rip"
        },
        {
          "source": 5554615,
          "target": 5554636,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c1cc",
          "kind": "branch"
        },
        {
          "source": 5554617,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x12278c0]",
          "kind": "rip"
        },
        {
          "source": 5554624,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5554629,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12278bc]",
          "kind": "rip"
        },
        {
          "source": 5554676,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5554689,
          "target": 5554698,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c20a",
          "kind": "branch"
        },
        {
          "source": 5554701,
          "target": 14444896,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c6960",
          "kind": "branch"
        },
        {
          "source": 5554716,
          "target": 5555343,
          "owner": 5553248,
          "mnemonic": "jbe",
          "operands": "0x212c384c48f",
          "kind": "branch"
        },
        {
          "source": 5554735,
          "target": 1172496,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c341e410",
          "kind": "branch"
        },
        {
          "source": 5554740,
          "target": 5556054,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c756",
          "kind": "branch"
        },
        {
          "source": 5554752,
          "target": 5555151,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c3cf",
          "kind": "branch"
        },
        {
          "source": 5554758,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x122783b]",
          "kind": "rip"
        },
        {
          "source": 5554768,
          "target": 5554789,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c265",
          "kind": "branch"
        },
        {
          "source": 5554770,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1227827]",
          "kind": "rip"
        },
        {
          "source": 5554777,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5554782,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1227823]",
          "kind": "rip"
        },
        {
          "source": 5554829,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5554842,
          "target": 5554851,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c2a3",
          "kind": "branch"
        },
        {
          "source": 5554861,
          "target": 1713120,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34a23e0",
          "kind": "branch"
        },
        {
          "source": 5554866,
          "target": 5556733,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c9fd",
          "kind": "branch"
        },
        {
          "source": 5554871,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12277ca]",
          "kind": "rip"
        },
        {
          "source": 5554881,
          "target": 5554902,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c2d6",
          "kind": "branch"
        },
        {
          "source": 5554883,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x12277b6]",
          "kind": "rip"
        },
        {
          "source": 5554890,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5554895,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12277b2]",
          "kind": "rip"
        },
        {
          "source": 5554942,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5554955,
          "target": 5554964,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c314",
          "kind": "branch"
        },
        {
          "source": 5554967,
          "target": 14444896,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c6960",
          "kind": "branch"
        },
        {
          "source": 5554982,
          "target": 5555365,
          "owner": 5553248,
          "mnemonic": "jbe",
          "operands": "0x212c384c4a5",
          "kind": "branch"
        },
        {
          "source": 5555001,
          "target": 1172496,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c341e410",
          "kind": "branch"
        },
        {
          "source": 5555006,
          "target": 5556173,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c7cd",
          "kind": "branch"
        },
        {
          "source": 5555011,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x122773e]",
          "kind": "rip"
        },
        {
          "source": 5555021,
          "target": 5555042,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c362",
          "kind": "branch"
        },
        {
          "source": 5555023,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x122772a]",
          "kind": "rip"
        },
        {
          "source": 5555030,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5555035,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1227726]",
          "kind": "rip"
        },
        {
          "source": 5555082,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5555095,
          "target": 5555104,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c3a0",
          "kind": "branch"
        },
        {
          "source": 5555107,
          "target": 14444896,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c6960",
          "kind": "branch"
        },
        {
          "source": 5555122,
          "target": 5555387,
          "owner": 5553248,
          "mnemonic": "jbe",
          "operands": "0x212c384c4bb",
          "kind": "branch"
        },
        {
          "source": 5555141,
          "target": 1172496,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c341e410",
          "kind": "branch"
        },
        {
          "source": 5555146,
          "target": 5556292,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c844",
          "kind": "branch"
        },
        {
          "source": 5555158,
          "target": 5555535,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c54f",
          "kind": "branch"
        },
        {
          "source": 5555164,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12276a5]",
          "kind": "rip"
        },
        {
          "source": 5555174,
          "target": 5555195,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c3fb",
          "kind": "branch"
        },
        {
          "source": 5555176,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1227691]",
          "kind": "rip"
        },
        {
          "source": 5555183,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5555188,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x122768d]",
          "kind": "rip"
        },
        {
          "source": 5555235,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5555248,
          "target": 5555257,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c439",
          "kind": "branch"
        },
        {
          "source": 5555267,
          "target": 1713120,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34a23e0",
          "kind": "branch"
        },
        {
          "source": 5555272,
          "target": 5556733,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c9fd",
          "kind": "branch"
        },
        {
          "source": 5555281,
          "target": 5555661,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c5cd",
          "kind": "branch"
        },
        {
          "source": 5555294,
          "target": 5555668,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c5d4",
          "kind": "branch"
        },
        {
          "source": 5555303,
          "target": 5555780,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c644",
          "kind": "branch"
        },
        {
          "source": 5555316,
          "target": 5555787,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c64b",
          "kind": "branch"
        },
        {
          "source": 5555325,
          "target": 5555899,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c6bb",
          "kind": "branch"
        },
        {
          "source": 5555338,
          "target": 5555906,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c6c2",
          "kind": "branch"
        },
        {
          "source": 5555347,
          "target": 5556018,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c732",
          "kind": "branch"
        },
        {
          "source": 5555360,
          "target": 5556025,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c739",
          "kind": "branch"
        },
        {
          "source": 5555369,
          "target": 5556137,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c7a9",
          "kind": "branch"
        },
        {
          "source": 5555382,
          "target": 5556144,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c7b0",
          "kind": "branch"
        },
        {
          "source": 5555391,
          "target": 5556256,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c820",
          "kind": "branch"
        },
        {
          "source": 5555404,
          "target": 5556263,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c827",
          "kind": "branch"
        },
        {
          "source": 5555416,
          "target": 5556375,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c897",
          "kind": "branch"
        },
        {
          "source": 5555422,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12275a3]",
          "kind": "rip"
        },
        {
          "source": 5555432,
          "target": 5555453,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c4fd",
          "kind": "branch"
        },
        {
          "source": 5555434,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x122758f]",
          "kind": "rip"
        },
        {
          "source": 5555441,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5555446,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x122758b]",
          "kind": "rip"
        },
        {
          "source": 5555493,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5555506,
          "target": 5555515,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c53b",
          "kind": "branch"
        },
        {
          "source": 5555525,
          "target": 1713120,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34a23e0",
          "kind": "branch"
        },
        {
          "source": 5555530,
          "target": 5556733,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c9fd",
          "kind": "branch"
        },
        {
          "source": 5555542,
          "target": 5556515,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c923",
          "kind": "branch"
        },
        {
          "source": 5555548,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1227525]",
          "kind": "rip"
        },
        {
          "source": 5555558,
          "target": 5555579,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c57b",
          "kind": "branch"
        },
        {
          "source": 5555560,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1227511]",
          "kind": "rip"
        },
        {
          "source": 5555567,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5555572,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x122750d]",
          "kind": "rip"
        },
        {
          "source": 5555619,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5555632,
          "target": 5555641,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c5b9",
          "kind": "branch"
        },
        {
          "source": 5555651,
          "target": 1713120,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34a23e0",
          "kind": "branch"
        },
        {
          "source": 5555656,
          "target": 5556733,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c9fd",
          "kind": "branch"
        },
        {
          "source": 5555687,
          "target": 14441760,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5555708,
          "target": 5556745,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384ca09",
          "kind": "branch"
        },
        {
          "source": 5555733,
          "target": 5555759,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384c62f",
          "kind": "branch"
        },
        {
          "source": 5555750,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5555759,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa460e2]",
          "kind": "rip"
        },
        {
          "source": 5555775,
          "target": 5556745,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ca09",
          "kind": "branch"
        },
        {
          "source": 5555806,
          "target": 14441760,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5555827,
          "target": 5556745,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384ca09",
          "kind": "branch"
        },
        {
          "source": 5555852,
          "target": 5555878,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384c6a6",
          "kind": "branch"
        },
        {
          "source": 5555869,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5555878,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa4606b]",
          "kind": "rip"
        },
        {
          "source": 5555894,
          "target": 5556745,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ca09",
          "kind": "branch"
        },
        {
          "source": 5555925,
          "target": 14441760,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5555946,
          "target": 5556745,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384ca09",
          "kind": "branch"
        },
        {
          "source": 5555971,
          "target": 5555997,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384c71d",
          "kind": "branch"
        },
        {
          "source": 5555988,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5555997,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa45ff4]",
          "kind": "rip"
        },
        {
          "source": 5556013,
          "target": 5556745,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ca09",
          "kind": "branch"
        },
        {
          "source": 5556044,
          "target": 14441760,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5556065,
          "target": 5556745,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384ca09",
          "kind": "branch"
        },
        {
          "source": 5556090,
          "target": 5556116,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384c794",
          "kind": "branch"
        },
        {
          "source": 5556107,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5556116,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa45f7d]",
          "kind": "rip"
        },
        {
          "source": 5556132,
          "target": 5556745,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ca09",
          "kind": "branch"
        },
        {
          "source": 5556163,
          "target": 14441760,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5556184,
          "target": 5556745,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384ca09",
          "kind": "branch"
        },
        {
          "source": 5556209,
          "target": 5556235,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384c80b",
          "kind": "branch"
        },
        {
          "source": 5556226,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5556235,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa45f06]",
          "kind": "rip"
        },
        {
          "source": 5556251,
          "target": 5556745,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ca09",
          "kind": "branch"
        },
        {
          "source": 5556282,
          "target": 14441760,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5556303,
          "target": 5556745,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384ca09",
          "kind": "branch"
        },
        {
          "source": 5556328,
          "target": 5556354,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384c882",
          "kind": "branch"
        },
        {
          "source": 5556345,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5556354,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa45e8f]",
          "kind": "rip"
        },
        {
          "source": 5556370,
          "target": 5556745,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ca09",
          "kind": "branch"
        },
        {
          "source": 5556380,
          "target": 14600344,
          "owner": 5553248,
          "mnemonic": "vucomiss",
          "operands": "xmm0, dword ptr [rip + 0x89fff4]",
          "kind": "rip"
        },
        {
          "source": 5556388,
          "target": 5556402,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c8b2",
          "kind": "branch"
        },
        {
          "source": 5556390,
          "target": 5556402,
          "owner": 5553248,
          "mnemonic": "jp",
          "operands": "0x212c384c8b2",
          "kind": "branch"
        },
        {
          "source": 5556396,
          "target": 5556625,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384c991",
          "kind": "branch"
        },
        {
          "source": 5556402,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12271cf]",
          "kind": "rip"
        },
        {
          "source": 5556412,
          "target": 5556433,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c8d1",
          "kind": "branch"
        },
        {
          "source": 5556414,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x12271bb]",
          "kind": "rip"
        },
        {
          "source": 5556421,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5556426,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12271b7]",
          "kind": "rip"
        },
        {
          "source": 5556473,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5556486,
          "target": 5556495,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c90f",
          "kind": "branch"
        },
        {
          "source": 5556505,
          "target": 1713120,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34a23e0",
          "kind": "branch"
        },
        {
          "source": 5556510,
          "target": 5556733,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c9fd",
          "kind": "branch"
        },
        {
          "source": 5556515,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x122715e]",
          "kind": "rip"
        },
        {
          "source": 5556525,
          "target": 5556546,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c942",
          "kind": "branch"
        },
        {
          "source": 5556527,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x122714a]",
          "kind": "rip"
        },
        {
          "source": 5556534,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5556539,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1227146]",
          "kind": "rip"
        },
        {
          "source": 5556586,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5556599,
          "target": 5556608,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c980",
          "kind": "branch"
        },
        {
          "source": 5556618,
          "target": 1713120,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34a23e0",
          "kind": "branch"
        },
        {
          "source": 5556623,
          "target": 5556733,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384c9fd",
          "kind": "branch"
        },
        {
          "source": 5556625,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12270f0]",
          "kind": "rip"
        },
        {
          "source": 5556635,
          "target": 5556656,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384c9b0",
          "kind": "branch"
        },
        {
          "source": 5556637,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x12270dc]",
          "kind": "rip"
        },
        {
          "source": 5556644,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5556649,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12270d8]",
          "kind": "rip"
        },
        {
          "source": 5556696,
          "target": 8874400,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b769a0",
          "kind": "branch"
        },
        {
          "source": 5556709,
          "target": 5556718,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384c9ee",
          "kind": "branch"
        },
        {
          "source": 5556728,
          "target": 1713120,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34a23e0",
          "kind": "branch"
        },
        {
          "source": 5556740,
          "target": 920896,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c33e0d40",
          "kind": "branch"
        },
        {
          "source": 5556753,
          "target": 5556779,
          "owner": 5553248,
          "mnemonic": "jle",
          "operands": "0x212c384ca2b",
          "kind": "branch"
        },
        {
          "source": 5556764,
          "target": 5556779,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384ca2b",
          "kind": "branch"
        },
        {
          "source": 5556777,
          "target": 5556782,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ca2e",
          "kind": "branch"
        },
        {
          "source": 5556824,
          "target": 5562444,
          "owner": 5553248,
          "mnemonic": "js",
          "operands": "0x212c384e04c",
          "kind": "branch"
        },
        {
          "source": 5556834,
          "target": 5556865,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384ca81",
          "kind": "branch"
        },
        {
          "source": 5556863,
          "target": 5556969,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384cae9",
          "kind": "branch"
        },
        {
          "source": 5556885,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa45c7c]",
          "kind": "rip"
        },
        {
          "source": 5556909,
          "target": 5556943,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384cacf",
          "kind": "branch"
        },
        {
          "source": 5556920,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5556941,
          "target": 5556952,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384cad8",
          "kind": "branch"
        },
        {
          "source": 5556962,
          "target": 14441760,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c5d20",
          "kind": "branch"
        },
        {
          "source": 5557035,
          "target": 5557088,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384cb60",
          "kind": "branch"
        },
        {
          "source": 5557046,
          "target": 5557068,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384cb4c",
          "kind": "branch"
        },
        {
          "source": 5557066,
          "target": 5557088,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384cb60",
          "kind": "branch"
        },
        {
          "source": 5557097,
          "target": 5557240,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384cbf8",
          "kind": "branch"
        },
        {
          "source": 5557108,
          "target": 14600344,
          "owner": 5553248,
          "mnemonic": "vucomiss",
          "operands": "xmm0, dword ptr [rip + 0x89fd1c]",
          "kind": "rip"
        },
        {
          "source": 5557116,
          "target": 5557124,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384cb84",
          "kind": "branch"
        },
        {
          "source": 5557118,
          "target": 5557400,
          "owner": 5553248,
          "mnemonic": "jnp",
          "operands": "0x212c384cc98",
          "kind": "branch"
        },
        {
          "source": 5557124,
          "target": 14707376,
          "owner": 5553248,
          "mnemonic": "vmulss",
          "operands": "xmm0, xmm0, dword ptr [rip + 0x8b9f24]",
          "kind": "rip"
        },
        {
          "source": 5557132,
          "target": 14601712,
          "owner": 5553248,
          "mnemonic": "vandps",
          "operands": "xmm1, xmm0, xmmword ptr [rip + 0x8a025c]",
          "kind": "rip"
        },
        {
          "source": 5557140,
          "target": 15307056,
          "owner": 5553248,
          "mnemonic": "vbroadcastss",
          "operands": "xmm2, dword ptr [rip + 0x94c593]",
          "kind": "rip"
        },
        {
          "source": 5557193,
          "target": 5557202,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384cbd2",
          "kind": "branch"
        },
        {
          "source": 5557209,
          "target": 5557229,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384cbed",
          "kind": "branch"
        },
        {
          "source": 5557225,
          "target": 5557273,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384cc19",
          "kind": "branch"
        },
        {
          "source": 5557227,
          "target": 5557251,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384cc03",
          "kind": "branch"
        },
        {
          "source": 5557236,
          "target": 5557251,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384cc03",
          "kind": "branch"
        },
        {
          "source": 5557238,
          "target": 5557273,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384cc19",
          "kind": "branch"
        },
        {
          "source": 5557249,
          "target": 5557273,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384cc19",
          "kind": "branch"
        },
        {
          "source": 5557254,
          "target": 5557266,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384cc12",
          "kind": "branch"
        },
        {
          "source": 5557264,
          "target": 5557273,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384cc19",
          "kind": "branch"
        },
        {
          "source": 5557286,
          "target": 5557914,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384ce9a",
          "kind": "branch"
        },
        {
          "source": 5557295,
          "target": 5557914,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384ce9a",
          "kind": "branch"
        },
        {
          "source": 5557309,
          "target": 5557914,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384ce9a",
          "kind": "branch"
        },
        {
          "source": 5557315,
          "target": 24575336,
          "owner": 5553248,
          "mnemonic": "cmp",
          "operands": "qword ptr [rip + 0x122311d], 0",
          "kind": "rip"
        },
        {
          "source": 5557323,
          "target": 5557337,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384cc59",
          "kind": "branch"
        },
        {
          "source": 5557325,
          "target": 24575328,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x122310c]",
          "kind": "rip"
        },
        {
          "source": 5557332,
          "target": 2891760,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c35c1ff0",
          "kind": "branch"
        },
        {
          "source": 5557337,
          "target": 24591020,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rdi, [rip + 0x1226e4c]",
          "kind": "rip"
        },
        {
          "source": 5557344,
          "target": 24568196,
          "owner": 5553248,
          "mnemonic": "cmp",
          "operands": "byte ptr [rip + 0x122151d], 0",
          "kind": "rip"
        },
        {
          "source": 5557351,
          "target": 5557494,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384ccf6",
          "kind": "branch"
        },
        {
          "source": 5557357,
          "target": 24568280,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x1221564]",
          "kind": "rip"
        },
        {
          "source": 5557367,
          "target": 5557426,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384ccb2",
          "kind": "branch"
        },
        {
          "source": 5557391,
          "target": 5557417,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384cca9",
          "kind": "branch"
        },
        {
          "source": 5557396,
          "target": 5557494,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384ccf6",
          "kind": "branch"
        },
        {
          "source": 5557398,
          "target": 5557426,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ccb2",
          "kind": "branch"
        },
        {
          "source": 5557406,
          "target": 5557195,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384cbcb",
          "kind": "branch"
        },
        {
          "source": 5557412,
          "target": 5557202,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384cbd2",
          "kind": "branch"
        },
        {
          "source": 5557424,
          "target": 5557494,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384ccf6",
          "kind": "branch"
        },
        {
          "source": 5557426,
          "target": 24568602,
          "owner": 5553248,
          "mnemonic": "cmp",
          "operands": "byte ptr [rip + 0x1221661], 0",
          "kind": "rip"
        },
        {
          "source": 5557433,
          "target": 5557444,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384ccc4",
          "kind": "branch"
        },
        {
          "source": 5557435,
          "target": 24568601,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rdi, [rip + 0x1221657]",
          "kind": "rip"
        },
        {
          "source": 5557442,
          "target": 5557494,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ccf6",
          "kind": "branch"
        },
        {
          "source": 5557444,
          "target": 24568488,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12215dd]",
          "kind": "rip"
        },
        {
          "source": 5557455,
          "target": 24568480,
          "owner": 5553248,
          "mnemonic": "cmp",
          "operands": "qword ptr [rip + 0x12215ca], rax",
          "kind": "rip"
        },
        {
          "source": 5557462,
          "target": 24568472,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rax, [rip + 0x12215bb]",
          "kind": "rip"
        },
        {
          "source": 5557483,
          "target": 24568600,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rdi, [rip + 0x1221626]",
          "kind": "rip"
        },
        {
          "source": 5557497,
          "target": 5557914,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384ce9a",
          "kind": "branch"
        },
        {
          "source": 5557503,
          "target": 24568552,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x12215e2]",
          "kind": "rip"
        },
        {
          "source": 5557517,
          "target": 5557537,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384cd21",
          "kind": "branch"
        },
        {
          "source": 5557530,
          "target": 5557634,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384cd82",
          "kind": "branch"
        },
        {
          "source": 5557532,
          "target": 5557914,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ce9a",
          "kind": "branch"
        },
        {
          "source": 5557592,
          "target": 5557914,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384ce9a",
          "kind": "branch"
        },
        {
          "source": 5557641,
          "target": 5557671,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384cda7",
          "kind": "branch"
        },
        {
          "source": 5557657,
          "target": 6486368,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c392f960",
          "kind": "branch"
        },
        {
          "source": 5557662,
          "target": 24568552,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rcx, qword ptr [rip + 0x1221543]",
          "kind": "rip"
        },
        {
          "source": 5557669,
          "target": 5557705,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384cdc9",
          "kind": "branch"
        },
        {
          "source": 5557712,
          "target": 5557723,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384cddb",
          "kind": "branch"
        },
        {
          "source": 5557721,
          "target": 5557773,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ce0d",
          "kind": "branch"
        },
        {
          "source": 5557783,
          "target": 5557840,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384ce50",
          "kind": "branch"
        },
        {
          "source": 5557789,
          "target": 14707376,
          "owner": 5553248,
          "mnemonic": "vdivss",
          "operands": "xmm1, xmm0, dword ptr [rip + 0x8b9c8b]",
          "kind": "rip"
        },
        {
          "source": 5557805,
          "target": 5557816,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384ce38",
          "kind": "branch"
        },
        {
          "source": 5557814,
          "target": 5557823,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ce3f",
          "kind": "branch"
        },
        {
          "source": 5557835,
          "target": 5862480,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3897450",
          "kind": "branch"
        },
        {
          "source": 5557851,
          "target": 5557914,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384ce9a",
          "kind": "branch"
        },
        {
          "source": 5557872,
          "target": 5557898,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384ce8a",
          "kind": "branch"
        },
        {
          "source": 5557889,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5557898,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa45887]",
          "kind": "rip"
        },
        {
          "source": 5557914,
          "target": 24572552,
          "owner": 5553248,
          "mnemonic": "cmp",
          "operands": "byte ptr [rip + 0x12223e7], 0",
          "kind": "rip"
        },
        {
          "source": 5557921,
          "target": 5557940,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384ceb4",
          "kind": "branch"
        },
        {
          "source": 5557923,
          "target": 24572548,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rax, [rip + 0x12223da]",
          "kind": "rip"
        },
        {
          "source": 5557933,
          "target": 5557999,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384ceef",
          "kind": "branch"
        },
        {
          "source": 5557935,
          "target": 5560814,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d9ee",
          "kind": "branch"
        },
        {
          "source": 5557940,
          "target": 24572432,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1222355]",
          "kind": "rip"
        },
        {
          "source": 5557951,
          "target": 24572424,
          "owner": 5553248,
          "mnemonic": "cmp",
          "operands": "qword ptr [rip + 0x1222342], rax",
          "kind": "rip"
        },
        {
          "source": 5557958,
          "target": 24572416,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rax, [rip + 0x1222333]",
          "kind": "rip"
        },
        {
          "source": 5557979,
          "target": 24572544,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rax, [rip + 0x122239e]",
          "kind": "rip"
        },
        {
          "source": 5557993,
          "target": 5560814,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d9ee",
          "kind": "branch"
        },
        {
          "source": 5557999,
          "target": 24572096,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x12221ca]",
          "kind": "rip"
        },
        {
          "source": 5558006,
          "target": 24572096,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x12221c3]",
          "kind": "rip"
        },
        {
          "source": 5558061,
          "target": 15318976,
          "owner": 5553248,
          "mnemonic": "vpshufb",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x94f08a]",
          "kind": "rip"
        },
        {
          "source": 5558078,
          "target": 14657168,
          "owner": 5553248,
          "mnemonic": "vpand",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x8ad74a]",
          "kind": "rip"
        },
        {
          "source": 5558099,
          "target": 5558135,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384cf77",
          "kind": "branch"
        },
        {
          "source": 5558104,
          "target": 5558785,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d201",
          "kind": "branch"
        },
        {
          "source": 5558110,
          "target": 24572552,
          "owner": 5553248,
          "mnemonic": "cmp",
          "operands": "byte ptr [rip + 0x1222323], 0",
          "kind": "rip"
        },
        {
          "source": 5558117,
          "target": 5558961,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d2b1",
          "kind": "branch"
        },
        {
          "source": 5558123,
          "target": 24572548,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rax, [rip + 0x1222312]",
          "kind": "rip"
        },
        {
          "source": 5558130,
          "target": 5559011,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d2e3",
          "kind": "branch"
        },
        {
          "source": 5558135,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1226b0a]",
          "kind": "rip"
        },
        {
          "source": 5558145,
          "target": 5558166,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384cf96",
          "kind": "branch"
        },
        {
          "source": 5558147,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1226af6]",
          "kind": "rip"
        },
        {
          "source": 5558154,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5558159,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1226af2]",
          "kind": "rip"
        },
        {
          "source": 5558188,
          "target": 5558197,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384cfb5",
          "kind": "branch"
        },
        {
          "source": 5558240,
          "target": 8882848,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b78aa0",
          "kind": "branch"
        },
        {
          "source": 5558253,
          "target": 5558313,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d029",
          "kind": "branch"
        },
        {
          "source": 5558271,
          "target": 5558297,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384d019",
          "kind": "branch"
        },
        {
          "source": 5558288,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5558297,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa456f8]",
          "kind": "rip"
        },
        {
          "source": 5558326,
          "target": 16045324,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rax, [rip + 0xa004cf]",
          "kind": "rip"
        },
        {
          "source": 5558333,
          "target": 16045548,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0xa005a8]",
          "kind": "rip"
        },
        {
          "source": 5558347,
          "target": 24475704,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rbx, qword ptr [rip + 0x120a7e6]",
          "kind": "rip"
        },
        {
          "source": 5558400,
          "target": 5577824,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3851c60",
          "kind": "branch"
        },
        {
          "source": 5558413,
          "target": 5560652,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d94c",
          "kind": "branch"
        },
        {
          "source": 5558468,
          "target": 5558594,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d142",
          "kind": "branch"
        },
        {
          "source": 5558481,
          "target": 5558511,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d0ef",
          "kind": "branch"
        },
        {
          "source": 5558509,
          "target": 5558594,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d142",
          "kind": "branch"
        },
        {
          "source": 5558543,
          "target": 5558576,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384d130",
          "kind": "branch"
        },
        {
          "source": 5558563,
          "target": 5558496,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d0e0",
          "kind": "branch"
        },
        {
          "source": 5558572,
          "target": 5558496,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d0e0",
          "kind": "branch"
        },
        {
          "source": 5558587,
          "target": 925520,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c33e1f50",
          "kind": "branch"
        },
        {
          "source": 5558592,
          "target": 5558502,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d0e6",
          "kind": "branch"
        },
        {
          "source": 5558634,
          "target": 5558859,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d24b",
          "kind": "branch"
        },
        {
          "source": 5558644,
          "target": 5558671,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d18f",
          "kind": "branch"
        },
        {
          "source": 5558669,
          "target": 5558754,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d1e2",
          "kind": "branch"
        },
        {
          "source": 5558697,
          "target": 5558736,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384d1d0",
          "kind": "branch"
        },
        {
          "source": 5558714,
          "target": 5558656,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d180",
          "kind": "branch"
        },
        {
          "source": 5558720,
          "target": 5558656,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d180",
          "kind": "branch"
        },
        {
          "source": 5558747,
          "target": 925520,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c33e1f50",
          "kind": "branch"
        },
        {
          "source": 5558752,
          "target": 5558662,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d186",
          "kind": "branch"
        },
        {
          "source": 5558774,
          "target": 5558880,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d260",
          "kind": "branch"
        },
        {
          "source": 5558783,
          "target": 5558887,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d267",
          "kind": "branch"
        },
        {
          "source": 5558785,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0x1226880]",
          "kind": "rip"
        },
        {
          "source": 5558795,
          "target": 5558816,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384d220",
          "kind": "branch"
        },
        {
          "source": 5558797,
          "target": 24590976,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x122686c]",
          "kind": "rip"
        },
        {
          "source": 5558804,
          "target": 1061600,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c34032e0",
          "kind": "branch"
        },
        {
          "source": 5558809,
          "target": 24590984,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0x1226868]",
          "kind": "rip"
        },
        {
          "source": 5558841,
          "target": 5560019,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d6d3",
          "kind": "branch"
        },
        {
          "source": 5558854,
          "target": 5560026,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d6da",
          "kind": "branch"
        },
        {
          "source": 5558878,
          "target": 5558776,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384d1f8",
          "kind": "branch"
        },
        {
          "source": 5558906,
          "target": 5558943,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384d29f",
          "kind": "branch"
        },
        {
          "source": 5558911,
          "target": 5560323,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d803",
          "kind": "branch"
        },
        {
          "source": 5558917,
          "target": 14441504,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c5c20",
          "kind": "branch"
        },
        {
          "source": 5558932,
          "target": 5560332,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384d80c",
          "kind": "branch"
        },
        {
          "source": 5558938,
          "target": 5560391,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d847",
          "kind": "branch"
        },
        {
          "source": 5558950,
          "target": 5560332,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384d80c",
          "kind": "branch"
        },
        {
          "source": 5558956,
          "target": 5560391,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d847",
          "kind": "branch"
        },
        {
          "source": 5558961,
          "target": 24572432,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1221f58]",
          "kind": "rip"
        },
        {
          "source": 5558972,
          "target": 24572424,
          "owner": 5553248,
          "mnemonic": "cmp",
          "operands": "qword ptr [rip + 0x1221f45], rax",
          "kind": "rip"
        },
        {
          "source": 5558979,
          "target": 24572416,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rax, [rip + 0x1221f36]",
          "kind": "rip"
        },
        {
          "source": 5559000,
          "target": 24572544,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rax, [rip + 0x1221fa1]",
          "kind": "rip"
        },
        {
          "source": 5559014,
          "target": 5559111,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384d347",
          "kind": "branch"
        },
        {
          "source": 5559027,
          "target": 5567248,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c384f310",
          "kind": "branch"
        },
        {
          "source": 5559041,
          "target": 5559047,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d307",
          "kind": "branch"
        },
        {
          "source": 5559093,
          "target": 5561778,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384ddb2",
          "kind": "branch"
        },
        {
          "source": 5559106,
          "target": 5561785,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384ddb9",
          "kind": "branch"
        },
        {
          "source": 5559111,
          "target": 24475664,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x120a4c2]",
          "kind": "rip"
        },
        {
          "source": 5559121,
          "target": 5559142,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384d366",
          "kind": "branch"
        },
        {
          "source": 5559123,
          "target": 24475656,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x120a4ae]",
          "kind": "rip"
        },
        {
          "source": 5559130,
          "target": 5551536,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c384b5b0",
          "kind": "branch"
        },
        {
          "source": 5559135,
          "target": 24475664,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x120a4aa]",
          "kind": "rip"
        },
        {
          "source": 5559154,
          "target": 5560005,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d6c5",
          "kind": "branch"
        },
        {
          "source": 5559172,
          "target": 19799232,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "r8, [rip + 0xd94935]",
          "kind": "rip"
        },
        {
          "source": 5559179,
          "target": 19543072,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "r9, [rip + 0xd5608e]",
          "kind": "rip"
        },
        {
          "source": 5559188,
          "target": 14034584,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c4062698",
          "kind": "branch"
        },
        {
          "source": 5559199,
          "target": 5560005,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d6c5",
          "kind": "branch"
        },
        {
          "source": 5559328,
          "target": 5559522,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d4e2",
          "kind": "branch"
        },
        {
          "source": 5559606,
          "target": 5572656,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3850830",
          "kind": "branch"
        },
        {
          "source": 5559613,
          "target": 5560005,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d6c5",
          "kind": "branch"
        },
        {
          "source": 5559626,
          "target": 5560005,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384d6c5",
          "kind": "branch"
        },
        {
          "source": 5559649,
          "target": 5572784,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c38508b0",
          "kind": "branch"
        },
        {
          "source": 5559659,
          "target": 5572912,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3850930",
          "kind": "branch"
        },
        {
          "source": 5559664,
          "target": 24475664,
          "owner": 5553248,
          "mnemonic": "cmp",
          "operands": "qword ptr [rip + 0x120a298], 0",
          "kind": "rip"
        },
        {
          "source": 5559672,
          "target": 5559686,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384d586",
          "kind": "branch"
        },
        {
          "source": 5559674,
          "target": 24475656,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x120a287]",
          "kind": "rip"
        },
        {
          "source": 5559681,
          "target": 5551536,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c384b5b0",
          "kind": "branch"
        },
        {
          "source": 5559702,
          "target": 6352048,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c390ecb0",
          "kind": "branch"
        },
        {
          "source": 5559715,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa4516e]",
          "kind": "rip"
        },
        {
          "source": 5559756,
          "target": 15291392,
          "owner": 5553248,
          "mnemonic": "vbroadcastss",
          "operands": "xmm0, dword ptr [rip + 0x947e2b]",
          "kind": "rip"
        },
        {
          "source": 5559844,
          "target": 19401799,
          "owner": 5553248,
          "mnemonic": "vmovdqu",
          "operands": "xmm0, xmmword ptr [rip + 0xd3361b]",
          "kind": "rip"
        },
        {
          "source": 5559885,
          "target": 15318992,
          "owner": 5553248,
          "mnemonic": "vpaddb",
          "operands": "xmm3, xmm1, xmmword ptr [rip + 0x94e97b]",
          "kind": "rip"
        },
        {
          "source": 5559902,
          "target": 15319008,
          "owner": 5553248,
          "mnemonic": "vpaddb",
          "operands": "xmm3, xmm3, xmmword ptr [rip + 0x94e97a]",
          "kind": "rip"
        },
        {
          "source": 5559925,
          "target": 19401815,
          "owner": 5553248,
          "mnemonic": "vmovq",
          "operands": "xmm0, qword ptr [rip + 0xd335da]",
          "kind": "rip"
        },
        {
          "source": 5559937,
          "target": 15319024,
          "owner": 5553248,
          "mnemonic": "vpsubb",
          "operands": "xmm0, xmm0, xmmword ptr [rip + 0x94e967]",
          "kind": "rip"
        },
        {
          "source": 5559957,
          "target": 14444896,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c40c6960",
          "kind": "branch"
        },
        {
          "source": 5559979,
          "target": 5573408,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3850b20",
          "kind": "branch"
        },
        {
          "source": 5559988,
          "target": 920896,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c33e0d40",
          "kind": "branch"
        },
        {
          "source": 5560000,
          "target": 920896,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c33e0d40",
          "kind": "branch"
        },
        {
          "source": 5560009,
          "target": 920896,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c33e0d40",
          "kind": "branch"
        },
        {
          "source": 5560014,
          "target": 5562243,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384df83",
          "kind": "branch"
        },
        {
          "source": 5560054,
          "target": 5567616,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c384f480",
          "kind": "branch"
        },
        {
          "source": 5560078,
          "target": 5560084,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d714",
          "kind": "branch"
        },
        {
          "source": 5560140,
          "target": 8882848,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3b78aa0",
          "kind": "branch"
        },
        {
          "source": 5560153,
          "target": 5560213,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d795",
          "kind": "branch"
        },
        {
          "source": 5560171,
          "target": 5560197,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384d785",
          "kind": "branch"
        },
        {
          "source": 5560188,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5560197,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa44f8c]",
          "kind": "rip"
        },
        {
          "source": 5560251,
          "target": 5558326,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d036",
          "kind": "branch"
        },
        {
          "source": 5560273,
          "target": 5560299,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384d7eb",
          "kind": "branch"
        },
        {
          "source": 5560290,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5560299,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa44f26]",
          "kind": "rip"
        },
        {
          "source": 5560318,
          "target": 5558326,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d036",
          "kind": "branch"
        },
        {
          "source": 5560330,
          "target": 5560391,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d847",
          "kind": "branch"
        },
        {
          "source": 5560346,
          "target": 5560372,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384d834",
          "kind": "branch"
        },
        {
          "source": 5560363,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5560372,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa44edd]",
          "kind": "rip"
        },
        {
          "source": 5560394,
          "target": 5560574,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d8fe",
          "kind": "branch"
        },
        {
          "source": 5560404,
          "target": 11222484,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3db3dd4",
          "kind": "branch"
        },
        {
          "source": 5560481,
          "target": 5560491,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d8ab",
          "kind": "branch"
        },
        {
          "source": 5560486,
          "target": 10059456,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3c97ec0",
          "kind": "branch"
        },
        {
          "source": 5560502,
          "target": 5560726,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d996",
          "kind": "branch"
        },
        {
          "source": 5560527,
          "target": 5560553,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384d8e9",
          "kind": "branch"
        },
        {
          "source": 5560544,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5560553,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa44e28]",
          "kind": "rip"
        },
        {
          "source": 5560569,
          "target": 5560726,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d996",
          "kind": "branch"
        },
        {
          "source": 5560585,
          "target": 5560648,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d948",
          "kind": "branch"
        },
        {
          "source": 5560606,
          "target": 5560632,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384d938",
          "kind": "branch"
        },
        {
          "source": 5560623,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5560632,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa44dd9]",
          "kind": "rip"
        },
        {
          "source": 5560656,
          "target": 5560687,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d96f",
          "kind": "branch"
        },
        {
          "source": 5560685,
          "target": 5560702,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384d97e",
          "kind": "branch"
        },
        {
          "source": 5560694,
          "target": 5578240,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3851e00",
          "kind": "branch"
        },
        {
          "source": 5560716,
          "target": 5560726,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d996",
          "kind": "branch"
        },
        {
          "source": 5560721,
          "target": 10059456,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3c97ec0",
          "kind": "branch"
        },
        {
          "source": 5560733,
          "target": 5560746,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384d9aa",
          "kind": "branch"
        },
        {
          "source": 5560754,
          "target": 5560814,
          "owner": 5553248,
          "mnemonic": "jb",
          "operands": "0x212c384d9ee",
          "kind": "branch"
        },
        {
          "source": 5560772,
          "target": 5560798,
          "owner": 5553248,
          "mnemonic": "ja",
          "operands": "0x212c384d9de",
          "kind": "branch"
        },
        {
          "source": 5560789,
          "target": 5562437,
          "owner": 5553248,
          "mnemonic": "jae",
          "operands": "0x212c384e045",
          "kind": "branch"
        },
        {
          "source": 5560798,
          "target": 16328472,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0xa44d33]",
          "kind": "rip"
        },
        {
          "source": 5560814,
          "target": 24474168,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0x1209843]",
          "kind": "rip"
        },
        {
          "source": 5560824,
          "target": 5560840,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384da08",
          "kind": "branch"
        },
        {
          "source": 5560833,
          "target": 5560872,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384da28",
          "kind": "branch"
        },
        {
          "source": 5560835,
          "target": 5562243,
          "owner": 5553248,
          "mnemonic": "jmp",
          "operands": "0x212c384df83",
          "kind": "branch"
        },
        {
          "source": 5560840,
          "target": 24474160,
          "owner": 5553248,
          "mnemonic": "lea",
          "operands": "rcx, [rip + 0x1209821]",
          "kind": "rip"
        },
        {
          "source": 5560847,
          "target": 3153072,
          "owner": 5553248,
          "mnemonic": "call",
          "operands": "0x212c3601cb0",
          "kind": "branch"
        },
        {
          "source": 5560852,
          "target": 24474168,
          "owner": 5553248,
          "mnemonic": "mov",
          "operands": "rdi, qword ptr [rip + 0x120981d]",
          "kind": "rip"
        },
        {
          "source": 5560866,
          "target": 5562243,
          "owner": 5553248,
          "mnemonic": "jne",
          "operands": "0x212c384df83",
          "kind": "branch"
        },
        {
          "source": 5560876,
          "target": 5562243,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384df83",
          "kind": "branch"
        },
        {
          "source": 5560994,
          "target": 5561107,
          "owner": 5553248,
          "mnemonic": "je",
          "operands": "0x212c384db13",
          "kind": "branch"
        },
        {
          "source": 5561100,
          "target": 5561222,
          "owner": 5553248,
          "mnemonic": "jo",
          "operands": "0x212c384db86",
          "kind": "branch"
        },
        {
          "source": 5561102,
          "target": 5561218,
          "owner": 5553248,
          "mnemonic": "jns",
          "operands": "0x212c384db82",
          "kind": "branch"
        }
      ],
      "incoming_truncated": false,
      "incoming": [
        {
          "source": 5239184,
          "target": 5553248,
          "owner": 5237664,
          "mnemonic": "call",
          "operands": "0x212c384bc60",
          "kind": "branch"
        }
      ]
    },
    {
      "va": "0x212c3a1ebe0",
      "rva": "0x71ebe0",
      "metadata": {
        "begin": 7465952,
        "end": 7466418,
        "unwind": 17449996,
        "table_rva": 66601156,
        "decoded_end": 7466418,
        "instruction_count": 104
      },
      "refs_truncated": false,
      "refs": [
        {
          "source": 7466006,
          "target": 7466109,
          "owner": 7465952,
          "mnemonic": "jne",
          "operands": "0x212c3a1ec7d",
          "kind": "branch"
        },
        {
          "source": 7466011,
          "target": 7466090,
          "owner": 7465952,
          "mnemonic": "jne",
          "operands": "0x212c3a1ec6a",
          "kind": "branch"
        },
        {
          "source": 7466020,
          "target": 16328288,
          "owner": 7465952,
          "mnemonic": "mov",
          "operands": "r14, qword ptr [rip + 0x873a35]",
          "kind": "rip"
        },
        {
          "source": 7466037,
          "target": 15314320,
          "owner": 7465952,
          "mnemonic": "vmovss",
          "operands": "xmm6, dword ptr [rip + 0x77c153]",
          "kind": "rip"
        },
        {
          "source": 7466045,
          "target": 15326608,
          "owner": 7465952,
          "mnemonic": "vmovss",
          "operands": "xmm7, dword ptr [rip + 0x77f14b]",
          "kind": "rip"
        },
        {
          "source": 7466059,
          "target": 7466065,
          "owner": 7465952,
          "mnemonic": "je",
          "operands": "0x212c3a1ec51",
          "kind": "branch"
        },
        {
          "source": 7466082,
          "target": 7466104,
          "owner": 7465952,
          "mnemonic": "je",
          "operands": "0x212c3a1ec78",
          "kind": "branch"
        },
        {
          "source": 7466088,
          "target": 7466104,
          "owner": 7465952,
          "mnemonic": "jmp",
          "operands": "0x212c3a1ec78",
          "kind": "branch"
        },
        {
          "source": 7466116,
          "target": 7466124,
          "owner": 7465952,
          "mnemonic": "je",
          "operands": "0x212c3a1ec8c",
          "kind": "branch"
        },
        {
          "source": 7466135,
          "target": 24475864,
          "owner": 7465952,
          "mnemonic": "mov",
          "operands": "rax, qword ptr [rip + 0x1038c3a]",
          "kind": "rip"
        },
        {
          "source": 7466145,
          "target": 7466244,
          "owner": 7465952,
          "mnemonic": "je",
          "operands": "0x212c3a1ed04",
          "kind": "branch"
        },
        {
          "source": 7466147,
          "target": 19811548,
          "owner": 7465952,
          "mnemonic": "mov",
          "operands": "ecx, dword ptr [rip + 0xbc6033]",
          "kind": "rip"
        },
        {
          "source": 7466153,
          "target": 19810564,
          "owner": 7465952,
          "mnemonic": "mov",
          "operands": "r8d, dword ptr [rip + 0xbc5c54]",
          "kind": "rip"
        },
        {
          "source": 7466171,
          "target": 7466203,
          "owner": 7465952,
          "mnemonic": "jne",
          "operands": "0x212c3a1ecdb",
          "kind": "branch"
        },
        {
          "source": 7466180,
          "target": 7466203,
          "owner": 7465952,
          "mnemonic": "jne",
          "operands": "0x212c3a1ecdb",
          "kind": "branch"
        },
        {
          "source": 7466188,
          "target": 7466203,
          "owner": 7465952,
          "mnemonic": "jne",
          "operands": "0x212c3a1ecdb",
          "kind": "branch"
        },
        {
          "source": 7466244,
          "target": 19808640,
          "owner": 7465952,
          "mnemonic": "mov",
          "operands": "dword ptr [rip + 0xbc5476], ebx",
          "kind": "rip"
        },
        {
          "source": 7466257,
          "target": 11222484,
          "owner": 7465952,
          "mnemonic": "call",
          "operands": "0x212c3db3dd4",
          "kind": "branch"
        },
        {
          "source": 7466337,
          "target": 11222484,
          "owner": 7465952,
          "mnemonic": "call",
          "operands": "0x212c3db3dd4",
          "kind": "branch"
        }
      ],
      "incoming_truncated": false,
      "incoming": [
        {
          "source": 7463832,
          "target": 7465952,
          "owner": 7461392,
          "mnemonic": "call",
          "operands": "0x212c3a1ebe0",
          "kind": "branch"
        },
        {
          "source": 7464478,
          "target": 7465952,
          "owner": 7461392,
          "mnemonic": "call",
          "operands": "0x212c3a1ebe0",
          "kind": "branch"
        }
      ]
    },
    {
      "va": "0x212c82f1690",
      "rva": "0x4ff1690",
      "metadata": null,
      "refs_truncated": false,
      "refs": [],
      "incoming_truncated": false,
      "incoming": []
    }
  ]
}
```
