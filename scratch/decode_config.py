"""Decode a mintaly .cfg (4-byte size + LZ4 block of JSON) and report the
values of the radar/chams toggles.  Mirrors config::make_key (low 32 bits of
the 64-bit FNV-1a of "<category>.<name>").
"""

import json
import sys

import lz4.block

CONFIG_DIR = r"C:\mintaly\configs"


def fnv1a(s: str) -> int:
    h = 14695981039346656037
    for b in s.encode("utf-8"):
        h ^= b
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h


def make_key(cat: str, name: str) -> int:
    return fnv1a(f"{cat[:120]}.{name[:120]}") & 0xFFFFFFFF


KEYS = {
    make_key("misc", "reveal radar"): "reveal radar",
    make_key("chams enemy", "chams"): "chams enemy",
    make_key("chams team", "chams"): "chams team",
    make_key("chams local", "chams"): "chams local",
    make_key("chams enemy ragdoll", "ragdoll chams"): "chams enemy ragdoll",
    make_key("chams team ragdoll", "ragdoll chams"): "chams team ragdoll",
    make_key("chams local ragdoll", "ragdoll chams"): "chams local ragdoll",
    make_key("chams enemy", "primary layer"): "enemy primary layer",
    make_key("chams enemy", "secondary layer"): "enemy secondary layer",
    make_key("chams enemy", "overlay layer"): "enemy overlay layer",
    make_key("misc", "watermark"): "watermark (control)",
    make_key("interface", "color palette"): "color palette (control)",
}


def load(path: str) -> dict:
    with open(path, "rb") as f:
        blob = f.read()
    size = int.from_bytes(blob[:4], "little")
    text = lz4.block.decompress(blob[4:], uncompressed_size=size)
    return json.loads(text)


def main() -> None:
    targets = sys.argv[1:] or [rf"{CONFIG_DIR}\default.cfg"]
    for path in targets:
        print(f"\n=== {path}")
        try:
            root = load(path)
        except Exception as exc:  # noqa: BLE001
            print(f"  failed: {exc}")
            continue
        fields = root.get("fields", {})
        print(f"  version={root.get('version')} fields={len(fields)}")
        for raw_key, label in KEYS.items():
            entry = fields.get(f"{raw_key:08x}")
            if entry is None:
                print(f"  {raw_key:08x}  {label:26} MISSING")
            elif isinstance(entry, dict):
                print(f"  {raw_key:08x}  {label:26} v={entry.get('v')} bind={entry.get('b')}")
            else:
                print(f"  {raw_key:08x}  {label:26} {entry}")
        enabled = sorted(
            k for k, v in fields.items() if isinstance(v, dict) and v.get("v")
        )
        print(f"  enabled setting keys: {len(enabled)}")


if __name__ == "__main__":
    main()
