import re
import os

menu_dir = r"c:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project\core\rendering\impl\menu"

toggles = []
for file in os.listdir(menu_dir):
    if file.endswith(".cpp") or file.endswith(".hpp"):
        with open(os.path.join(menu_dir, file), "r", encoding="utf-8", errors="ignore") as f:
            for line_no, line in enumerate(f, 1):
                m = re.search(r'xui::(toggle|checkbox)\s*\(([^;]+)\)', line)
                if m:
                    toggles.append((file, line_no, m.group(1), m.group(2).strip()))

print(f"Total toggle/checkbox calls: {len(toggles)}")
for f, l, fn, args in toggles:
    print(f"{f}:{l} -> {fn}({args})")
