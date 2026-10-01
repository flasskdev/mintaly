<!-- split-part | CS2_RESEARCH_MASTER.md lines 101369-101433 | body-sha256 90b19a5efb5d51f5cefcc50cf2cb2dcdd13543e8c20cb28fc13870c1e20adf43 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-184"></a>

## E184. `analysis/strings/xor_probe.json`

Bytes: 1688. SHA-256: `3cf0891ef4dd59c1e942c5f73feabf648e931bdf8a7fd8d485656e564c349983`.

```json
{
  "tested_words": [
    "ragebot",
    "aimbot",
    "hitchance",
    "multipoint",
    "antiaim",
    "rapidfire",
    "defensive",
    "tickbase",
    "backtrack",
    "resolver",
    "penetration",
    "lagcompensation",
    "minimum_damage",
    "doubletap",
    "neverlose"
  ],
  "forms": [
    "lowercase",
    "Titlecase",
    "UPPERCASE"
  ],
  "method": "Adjacent-byte XOR invariant, with original-byte verification; ignore ordinary case-only XOR 0x20 aliases.",
  "candidates": [
    {
      "decoded_text": "Neverlose",
      "xor_key_hex": "0x87",
      "encoding": "ascii_single_byte_xor_candidate",
      "interpretation": "Только точное совпадение после XOR; не доказательство общего шифрования строк либо роли в программе.",
      "file_offset": 19403670,
      "file_offset_hex": "0x01281396",
      "rva": 19403670,
      "rva_hex": "0x01281396",
      "va": 2279626773398,
      "va_hex": "0x212C4581396",
      "byte_length": 9,
      "end_offset_exclusive_hex": "0x0128139F"
    },
    {
      "decoded_text": "Neverlose",
      "xor_key_hex": "0x28",
      "encoding": "ascii_single_byte_xor_candidate",
      "interpretation": "Только точное совпадение после XOR; не доказательство общего шифрования строк либо роли в программе.",
      "file_offset": 19417960,
      "file_offset_hex": "0x01284B68",
      "rva": 19417960,
      "rva_hex": "0x01284B68",
      "va": 2279626787688,
      "va_hex": "0x212C4584B68",
      "byte_length": 9,
      "end_offset_exclusive_hex": "0x01284B71"
    }
  ]
}
```
