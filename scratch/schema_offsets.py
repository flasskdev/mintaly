"""Statically resolve schema field offsets from the installed client.dll.

Mirrors the runtime layout used by systems::schemas::lookup:
ClassInfo + 0x24 = uint16 field count, + 0x30 = field array, each field 0x20
bytes with the name pointer at +0x00 and the offset at +0x10.
"""

import struct
import sys

CLIENT = (
    r"C:\Program Files (x86)\Steam\steamapps\common"
    r"\Counter-Strike Global Offensive\game\csgo\bin\win64\client.dll"
)

WANTED = {
    "C_BaseEntity": ["m_iTeamNum", "m_iHealth", "m_lifeState", "m_pGameSceneNode"],
    "C_BasePlayerPawn": ["m_iTeamNum", "m_iHealth", "m_pWeaponServices"],
    "C_CSPlayerPawnBase": ["m_iTeamNum", "m_iHealth"],
    "C_CSPlayerPawn": ["m_entitySpottedState", "m_iTeamNum", "m_iHealth"],
    "CBasePlayerController": ["m_hPawn"],
    "CCSPlayerController": ["m_hPlayerPawn", "m_bPawnIsAlive", "m_iTeamNum"],
    "EntitySpottedState_t": ["m_bSpotted", "m_bSpottedByMask"],
    "C_PlayerWeaponServices": ["m_hActiveWeapon"],
    "CPlayer_WeaponServices": ["m_hActiveWeapon"],
}


def main() -> None:
    with open(CLIENT, "rb") as f:
        data = f.read()

    e_lfanew = struct.unpack_from("<I", data, 0x3C)[0]
    num_sections = struct.unpack_from("<H", data, e_lfanew + 6)[0]
    opt_hdr_size = struct.unpack_from("<H", data, e_lfanew + 20)[0]
    opt_hdr_offset = e_lfanew + 24
    image_base = struct.unpack_from("<Q", data, opt_hdr_offset + 24)[0]
    sec_offset = opt_hdr_offset + opt_hdr_size

    sections = []
    for i in range(num_sections):
        sec = data[sec_offset + i * 40: sec_offset + (i + 1) * 40]
        vsize, vaddr, raw_size, raw_offset = struct.unpack_from("<IIII", sec, 8)
        sections.append({
            "vaddr": vaddr,
            "vsize": vsize,
            "raw_offset": raw_offset,
            "raw_size": raw_size,
        })

    def va_to_offset(va: int):
        if va < image_base:
            return None
        rva = va - image_base
        for s in sections:
            if s["vaddr"] <= rva < s["vaddr"] + max(s["vsize"], s["raw_size"]):
                return s["raw_offset"] + (rva - s["vaddr"])
        return None

    def read_str(off: int, limit: int = 96):
        end = data.find(b"\x00", off, off + limit)
        if end == -1:
            return None
        s = data[off:end].decode("utf-8", errors="ignore")
        return s if s and s.isascii() else None

    def find_class(name: str):
        target = name.encode() + b"\x00"
        pos = 0
        while True:
            idx = data.find(target, pos)
            if idx == -1:
                return None
            pos = idx + 1
            str_rva = None
            for s in sections:
                if s["raw_offset"] <= idx < s["raw_offset"] + s["raw_size"]:
                    str_rva = s["vaddr"] + (idx - s["raw_offset"])
                    break
            if str_rva is None:
                continue
            ptr = struct.pack("<Q", image_base + str_rva)
            p_pos = 0
            while True:
                p_idx = data.find(ptr, p_pos)
                if p_idx == -1:
                    break
                p_pos = p_idx + 1
                class_off = p_idx - 8  # name pointer is not at +0: check both
                for candidate in (p_idx, p_idx - 8, p_idx - 0x10):
                    if candidate < 0:
                        continue
                    count = struct.unpack_from("<H", data, candidate + 0x24)[0]
                    fields_va = struct.unpack_from("<Q", data, candidate + 0x30)[0]
                    fields_off = va_to_offset(fields_va)
                    if not count or count > 512 or fields_off is None:
                        continue
                    first_name_ptr = struct.unpack_from("<Q", data, fields_off)[0]
                    first_name_off = va_to_offset(first_name_ptr)
                    if first_name_off is None:
                        continue
                    first_name = read_str(first_name_off)
                    if first_name and first_name.startswith("m_"):
                        return candidate, count, fields_off
                _ = class_off

    for class_name, wanted in WANTED.items():
        found = find_class(class_name)
        if not found:
            print(f"{class_name}: NOT FOUND")
            continue
        candidate, count, fields_off = found
        size = struct.unpack_from("<I", data, candidate + 0x10)[0]
        print(f"{class_name}: fields={count} size={size}")
        for i in range(count):
            field = data[fields_off + i * 0x20: fields_off + (i + 1) * 0x20]
            name_ptr = struct.unpack_from("<Q", field, 0)[0]
            name_off = va_to_offset(name_ptr)
            if name_off is None:
                continue
            name = read_str(name_off)
            offset = struct.unpack_from("<I", field, 0x10)[0]
            if name in wanted:
                print(f"    {name:24} 0x{offset:x} ({offset})")


if __name__ == "__main__":
    sys.exit(main())
