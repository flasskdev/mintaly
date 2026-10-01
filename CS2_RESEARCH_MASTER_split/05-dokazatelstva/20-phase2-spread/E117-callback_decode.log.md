<!-- split-part | CS2_RESEARCH_MASTER.md lines 68376-68434 | body-sha256 f7d0eb67851ccd994a95c83f431dda7800a3d92d7715b14bdaa777bcb2cfff86 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-117"></a>

## E117. `analysis/phase2/spread/callback_decode.log`

Bytes: 873. SHA-256: `b58cd01d787cc62d5b0d6e3fcd3ae59dd5b9bb2b114f307a4b76085ceb82470d`.

```text
{
  "file": "0051d320.asm",
  "begin": "0x51d320",
  "end": "0x51d735",
  "decoded_end": "0x51d735",
  "patches": [],
  "stop_bytes": "",
  "method": "linear decode; PEB/junk reachability not resolved; no skipdata",
  "calls": [
    [
      "0x51d331",
      "0x212c3dee460"
    ],
    [
      "0x51d388",
      "0x212c381d910"
    ],
    [
      "0x51d558",
      "0x212c381dac0"
    ],
    [
      "0x51d64f",
      "qword ptr [rax + 0x18]"
    ],
    [
      "0x51d68f",
      "qword ptr [rax + 0x18]"
    ],
    [
      "0x51d6dd",
      "qword ptr [rax + 0x18]"
    ],
    [
      "0x51d6f6",
      "qword ptr [rax + 0x18]"
    ]
  ]
}
{
  "file": "0051cf40.asm",
  "begin": "0x51cf40",
  "end": "0x51d317",
  "decoded_end": "0x51d317",
  "patches": [],
  "stop_bytes": "",
  "method": "linear decode; PEB/junk reachability not resolved; no skipdata",
  "calls": []
}
```
