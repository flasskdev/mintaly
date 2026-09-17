import re
import os

# Let's inspect all files in project/core and project/external and project/
# to find all xui::setting instances, including in rendering, menu, etc.

results = []
for root, dirs, files in os.walk(r"c:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project"):
    for file in files:
        if file.endswith((".hpp", ".h", ".cpp")):
            filepath = os.path.join(root, file)
            with open(filepath, "r", encoding="utf-8", errors="ignore") as f:
                content = f.read()
            # find xui::setting declarations
            for m in re.finditer(r'xui::setting\s+([a-zA-Z0-9_]+)\s*(\{[^;]*\})?;', content):
                results.append((filepath, m.group(1), m.group(2) if m.group(2) else ""))

print(f"Total xui::setting found across project: {len(results)}")
for path, name, init in results:
    if not init or init.strip() == "{}":
        rel = os.path.relpath(path, r"c:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project")
        print(f"Empty init: {rel} -> {name}")
