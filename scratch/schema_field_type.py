"""Print the declared type of selected schema fields (type name sits behind the
pointer stored at field + 0x08; the type name string is validated the same way
as the field name)."""

import struct

CLIENT = (
    r"C:\Program Files (x86)\Steam\steamapps\common"
    r"\Counter-Strike Global Offensive\game\csgo\bin\win64\client.dll"
)

WANTED = {
    "C_BaseEntity": ["m_iTeamNum", "m_iHealth", "m_lifeState"],
    "C_CSPlayerPawn": ["m_entitySpottedState"],
    "EntitySpottedState_t": ["m_bSpotted", "m_bSpottedByMask"],
    "CCSPlayerController": ["m_bPawnIsAlive"],
}

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
    sections.append((vaddr, vsize, raw_offset, raw_size))


def va_to_offset(va):
    if va < image_base:
        return None
    rva = va - image_base
    for vaddr, vsize, raw_offset, raw_size in sections:
        if vaddr <= rva < vaddr + max(vsize, raw_size):
            return raw_offset + (rva - vaddr)
    return None


def read_str(off, limit=64):
    if off is None or off < 0 or off >= len(data):
        return None
    end = data.find(b"\x00", off, off + limit)
    if end == -1:
        return None
    s = data[off:end].decode("utf-8", errors="ignore")
    return s if s and s.isascii() else None


def find_class(name):
    target = name.encode() + b"\x00"
    pos = 0
    while True:
        idx = data.find(target, pos)
        if idx == -1:
            return None
        pos = idx + 1
        for vaddr, vsize, raw_offset, raw_size in sections:
            if raw_offset <= idx < raw_offset + raw_size:
                str_rva = vaddr + (idx - raw_offset)
                break
        else:
            continue
        ptr = struct.pack("<Q", image_base + str_rva)
        p_pos = 0
        while True:
            p_idx = data.find(ptr, p_pos)
            if p_idx == -1:
                break
            p_pos = p_idx + 1
            for candidate in (p_idx, p_idx - 8, p_idx - 0x10):
                if candidate < 0:
                    continue
                count = struct.unpack_from("<H", data, candidate + 0x24)[0]
                fields_va = struct.unpack_from("<Q", data, candidate + 0x30)[0]
                fields_off = va_to_offset(fields_va)
                if not count or count > 512 or fields_off is None:
                    continue
                first = read_str(va_to_offset(struct.unpack_from("<Q", data, fields_off)[0]))
                if first and first.startswith("m_"):
                    return count, fields_off
    return None


for class_name, wanted in WANTED.items():
    found = find_class(class_name)
    if not found:
        print(f"{class_name}: NOT FOUND")
        continue
    count, fields_off = found
    for i in range(count):
        field = fields_off + i * 0x20
        name = read_str(va_to_offset(struct.unpack_from("<Q", data, field)[0]))
        if name not in wanted:
            continue
        type_ptr = struct.unpack_from("<Q", data, field + 0x08)[0]
        type_off = va_to_offset(type_ptr)
        type_name = read_str(va_to_offset(struct.unpack_from("<Q", data, type_off + 0x08)[0])) if type_off else None
        offset = struct.unpack_from("<I", data, field + 0x10)[0]
        print(f"{class_name}.{name}: offset=0x{offset:x} type={type_name}")
