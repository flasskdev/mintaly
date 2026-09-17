import re

with open(r"c:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project\core\settings.hpp", "r", encoding="utf-8") as f:
    content = f.read()

def fnv1a(s):
    h = 0x811c9dc5
    for b in s.encode('latin1'):
        h ^= b
        h = (h * 0x01000193) & 0xffffffff
    return h

def make_key(cat, name):
    cat_part = cat[:120]
    name_part = name[:120]
    buf = f"{cat_part}.{name_part}"
    return fnv1a(buf), buf

# Let's find all xui::setting in settings.hpp
# Pattern: xui::setting var_name { value, bind, "name", "category" }
p = re.compile(r'xui::setting\s+([a-zA-Z0-9_]+)\s*\{([^}]+)\}')
matches = p.findall(content)

keys = {}
for var_name, init in matches:
    parts = [s.strip() for s in init.split(',')]
    # find string literals in init
    strs = re.findall(r'"([^"]*)"', init)
    if len(strs) >= 2:
        name = strs[0]
        cat = strs[1]
        k, buf = make_key(cat, name)
        if k in keys:
            print(f"COLLISION / DUPLICATE: {k:08x} -> '{buf}' (var {var_name}) already exists as '{keys[k]}'")
        else:
            keys[k] = (var_name, buf)
    else:
        print(f"Less than 2 strings in init: var {var_name} -> strs: {strs}, init: {init}")

print(f"Total settings with name & cat found: {len(keys)}")
