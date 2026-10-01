<!-- split-part | CS2_RESEARCH_MASTER.md lines 74953-76097 | body-sha256 0861ba2651366baa6b088503c46e6929a163b2d96e05210e5485f72aeb37f408 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-138"></a>

## E138. `analysis/phase2/spread/shortlist_decode.log`

Bytes: 19839. SHA-256: `9b003a583523f05c313162bc698001c2901f3aeb44e8884985bd7de14eb004d1`.

```text
{
  "file": "00515ca0.asm",
  "begin": "0x515ca0",
  "end": "0x51aa45",
  "decoded_end": "0x51618e",
  "patches": [
    {
      "offset": "0x515eff",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x515fa7",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x5160a4",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x5160b6",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x51619d",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x5161bd",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x516401",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x51640b",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x5164e3",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x516771",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x516d2b",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x5180ab",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x5181aa",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x5181c0",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x5182dc",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x5183d5",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x518c82",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x518e0f",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x518f14",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x51902f",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x519039",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x519102",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x519114",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x519370",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x51937e",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x519771",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x519826",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x519d75",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x519e60",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x519e72",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x519eff",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x519f19",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x51a414",
      "original": "0f1a2410",
      "replacement": "90909090"
    }
  ],
  "stop_bytes": "ff7d48b8725f57da0280ffff480fce0f",
  "method": "linear decode; PEB/junk reachability not resolved; no skipdata",
  "calls": [
    [
      "0x515d40",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x515d74",
      "0x212c3604060"
    ],
    [
      "0x515dd4",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x515f95",
      "0x2130c6b437d"
    ],
    [
      "0x515fe6",
      "0x212c360c8b0"
    ]
  ]
}
{
  "file": "004feba0.asm",
  "begin": "0x4feba0",
  "end": "0x505404",
  "decoded_end": "0x5022ab",
  "patches": [
    {
      "offset": "0x4ff46b",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4ff47a",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4ff52a",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4ff52e",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4ff53f",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x5021da",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x502667",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x5049b2",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x504d01",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x504d05",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x504d17",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x504dcf",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x504eb9",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x50531b",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x505325",
      "original": "0f1a2410",
      "replacement": "90909090"
    }
  ],
  "stop_bytes": "fe7f030c256c02fe7f030c25e802fe7f",
  "method": "linear decode; PEB/junk reachability not resolved; no skipdata",
  "calls": [
    [
      "0x4fec4a",
      "0x212c36ca9e0"
    ],
    [
      "0x4fecfb",
      "0x212c36388d0"
    ],
    [
      "0x4fed0d",
      "0x212c40c5d20"
    ],
    [
      "0x4fed3a",
      "0x212c40c5d20"
    ],
    [
      "0x4fed55",
      "0x212c40c5d20"
    ],
    [
      "0x4fed78",
      "0x212c40c5d20"
    ],
    [
      "0x4fedca",
      "0x212c3638a40"
    ],
    [
      "0x4feddc",
      "0x212c40c5d20"
    ],
    [
      "0x4fee06",
      "0x212c40c5d20"
    ],
    [
      "0x4fee21",
      "0x212c40c5d20"
    ],
    [
      "0x4fee3c",
      "0x212c40c5d20"
    ],
    [
      "0x4feeab",
      "0x212c36ca9e0"
    ],
    [
      "0x4fef6f",
      "0x212c36388d0"
    ],
    [
      "0x4fef81",
      "0x212c40c5d20"
    ],
    [
      "0x4fefae",
      "0x212c40c5d20"
    ],
    [
      "0x4fefc9",
      "0x212c40c5d20"
    ],
    [
      "0x4fefec",
      "0x212c40c5d20"
    ],
    [
      "0x4ff03e",
      "0x212c3638a40"
    ],
    [
      "0x4ff050",
      "0x212c40c5d20"
    ],
    [
      "0x4ff07a",
      "0x212c40c5d20"
    ],
    [
      "0x4ff095",
      "0x212c40c5d20"
    ],
    [
      "0x4ff0b0",
      "0x212c40c5d20"
    ],
    [
      "0x4ff111",
      "0x212c3601fd0"
    ],
    [
      "0x4ff190",
      "0x212c384bc60"
    ],
    [
      "0x4ff19f",
      "0x212c384ecb0"
    ],
    [
      "0x4ff1ea",
      "qword ptr [rax + 0x18]"
    ],
    [
      "0x4ff222",
      "0x212c36027b0"
    ],
    [
      "0x4ff230",
      "qword ptr [rax + 0x18]"
    ],
    [
      "0x4ff24e",
      "0x212c35c0f70"
    ],
    [
      "0x4ff363",
      "0x212c36465d0"
    ],
    [
      "0x4ff3bd",
      "0x212c35c6110"
    ],
    [
      "0x4ff3e8",
      "0x212c37b9350"
    ],
    [
      "0x4ff3fd",
      "qword ptr [rax + 0x18]"
    ],
    [
      "0x4ff572",
      "0x212c3a3e380"
    ],
    [
      "0x4ff5b5",
      "0x212c34d80e0"
    ],
    [
      "0x4ff5ee",
      "0x212c360c8b0"
    ],
    [
      "0x4ff63d",
      "0x212c3609a00"
    ],
    [
      "0x4ff68d",
      "0x212c35b7280"
    ],
    [
      "0x4ff6a6",
      "qword ptr [rax + 0x50]"
    ],
    [
      "0x4ff8a3",
      "0x212c34d7970"
    ],
    [
      "0x4ff93e",
      "rdi"
    ],
    [
      "0x4ff94f",
      "rdi"
    ],
    [
      "0x4ff9ce",
      "0x212c35b7280"
    ],
    [
      "0x4ffa4f",
      "qword ptr [rip + 0xa92c93]"
    ],
    [
      "0x4ffae8",
      "qword ptr [rip + 0xa92bfa]"
    ],
    [
      "0x4ffb92",
      "qword ptr [rip + 0xa92b50]"
    ],
    [
      "0x4ffcc1",
      "0x212c36080f0"
    ],
    [
      "0x50005d",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x5000f7",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x500187",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x500242",
      "0x212c4092b60"
    ],
    [
      "0x50043d",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x50054e",
      "0x212c4092b60"
    ],
    [
      "0x500727",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x5007d4",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x500934",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x500a03",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x500a9a",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x500f6a",
      "0x212c35b7280"
    ],
    [
      "0x501059",
      "qword ptr [rax + 8]"
    ],
    [
      "0x50117d",
      "qword ptr [rax + 8]"
    ],
    [
      "0x50134d",
      "qword ptr [rax + 0x18]"
    ],
    [
      "0x501486",
      "0x212c34a0140"
    ],
    [
      "0x5015cb",
      "0x2131c50ce18"
    ],
    [
      "0x501655",
      "0x212c37711b0"
    ],
    [
      "0x5017ab",
      "0x212c36080f0"
    ],
    [
      "0x5018b0",
      "qword ptr [rax + 8]"
    ],
    [
      "0x5018df",
      "qword ptr [rax + 8]"
    ],
    [
      "0x50190d",
      "0x212c40c5d20"
    ],
    [
      "0x501934",
      "0x212c40c5d20"
    ],
    [
      "0x501986",
      "qword ptr [rax + 0x18]"
    ],
    [
      "0x501a19",
      "0x212c3453ee0"
    ],
    [
      "0x5020b6",
      "qword ptr [rax + 0x38]"
    ],
    [
      "0x5020da",
      "qword ptr [rax + 0x98]"
    ],
    [
      "0x5020ec",
      "0x212c382cd20"
    ]
  ]
}
{
  "file": "004beb50.asm",
  "begin": "0x4beb50",
  "end": "0x4cac30",
  "decoded_end": "0x4c3e87",
  "patches": [
    {
      "offset": "0x4bec45",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bed12",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bf054",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bf418",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bf42c",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bf4c9",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bf4dc",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bf4e6",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bf551",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bf55b",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bf569",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bf9d7",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bfbb3",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bfbb7",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bfcb3",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4bfcc8",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c083e",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c0be1",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c0be9",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c0cb2",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c0cba",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c0cd4",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c1302",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c1306",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c16b1",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c16b5",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c16d4",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c1854",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c185c",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c1c9f",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c1d90",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c21f0",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c29fb",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c2da0",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c2db0",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c2e63",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c2e7b",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c3a37",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c3a54",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c3dd5",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c3e92",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c3ea2",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c3ead",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c6098",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c60aa",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c641d",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c650f",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c6607",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c660f",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c69cc",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c6ab8",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c6abc",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c6c37",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c6fb6",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c70b3",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c7148",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c75ac",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c773a",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c7744",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c774c",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c7b0d",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c7b11",
      "original": "0f1b2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c7b1b",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c8b26",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c8e74",
      "original": "0f1c2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c8e7e",
      "original": "0f1a2410",
      "replacement": "90909090"
    },
    {
      "offset": "0x4c8f5c",
      "original": "0f1c2410",
      "replacement": "90909090"
    }
  ],
  "stop_bytes": "ffff67914887c948c1ca400f1c241048",
  "method": "linear decode; PEB/junk reachability not resolved; no skipdata",
  "calls": [
    [
      "0x4bed60",
      "0x212c3620800"
    ],
    [
      "0x4bed95",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x4befc9",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x4bfa26",
      "0x212c34d7970"
    ],
    [
      "0x4bfd32",
      "0x212c35f6d70"
    ],
    [
      "0x4bfe3c",
      "0x212c34d7970"
    ],
    [
      "0x4bfe9d",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x4bfee4",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x4bff93",
      "rdi"
    ],
    [
      "0x4bffa4",
      "rdi"
    ],
    [
      "0x4bffb5",
      "rdi"
    ],
    [
      "0x4c030d",
      "rdi"
    ],
    [
      "0x4c031b",
      "rdi"
    ],
    [
      "0x4c04db",
      "0x212c37a85c0"
    ],
    [
      "0x4c056d",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x4c074a",
      "0x212c33f57b0"
    ],
    [
      "0x4c0d3a",
      "0x212c34463a0"
    ],
    [
      "0x4c0d61",
      "0x212c35b7280"
    ],
    [
      "0x4c0e03",
      "0x212c35b7280"
    ],
    [
      "0x4c0ea7",
      "0x212c34d7970"
    ],
    [
      "0x4c111d",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x4c11ef",
      "0x212c3638ba0"
    ],
    [
      "0x4c12a3",
      "0x212c36204a0"
    ],
    [
      "0x4c1ce0",
      "0x212c34463a0"
    ],
    [
      "0x4c2220",
      "0x212c34463a0"
    ],
    [
      "0x4c23b7",
      "0x212c408f800"
    ],
    [
      "0x4c2454",
      "0x212c37bb160"
    ],
    [
      "0x4c246f",
      "qword ptr [rax + 8]"
    ],
    [
      "0x4c2493",
      "0x212c37bb280"
    ],
    [
      "0x4c2500",
      "0x212c37bb280"
    ],
    [
      "0x4c256d",
      "0x212c37bb280"
    ],
    [
      "0x4c25da",
      "0x212c37bb280"
    ],
    [
      "0x4c2647",
      "0x212c37bb280"
    ],
    [
      "0x4c26b4",
      "0x212c37bb280"
    ],
    [
      "0x4c2721",
      "0x212c37bb280"
    ],
    [
      "0x4c278e",
      "0x212c37bb280"
    ],
    [
      "0x4c28ec",
      "0x212c3638ba0"
    ],
    [
      "0x4c2998",
      "0x212c36204a0"
    ],
    [
      "0x4c2edd",
      "0x212c34463a0"
    ],
    [
      "0x4c30b3",
      "qword ptr [rax + 0x10]"
    ],
    [
      "0x4c30e8",
      "0x212c40c5d20"
    ],
    [
      "0x4c3154",
      "0x212c40c5d20"
    ],
    [
      "0x4c3590",
      "0x212c37bb160"
    ],
    [
      "0x4c35ab",
      "qword ptr [rax + 8]"
    ],
    [
      "0x4c35b8",
      "0x212c37bb280"
    ],
    [
      "0x4c3620",
      "qword ptr [rax]"
    ],
    [
      "0x4c3630",
      "qword ptr [rax + 8]"
    ],
    [
      "0x4c37d6",
      "0x212c408f800"
    ],
    [
      "0x4c384d",
      "0x212c37cbe60"
    ],
    [
      "0x4c3884",
      "0x212c37cbe60"
    ],
    [
      "0x4c393b",
      "0x212c34d7970"
    ]
  ]
}
```
