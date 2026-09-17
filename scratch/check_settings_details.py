with open(r"c:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project\core\settings.hpp", "r", encoding="utf-8") as f:
    lines = f.readlines()

for idx, line in enumerate(lines[:1300]):
    if "xui::setting" in line and not line.strip().startswith("//"):
        if "{" not in line or "{}" in line.replace(" ", "") or '""' in line:
            print(f"L{idx+1}: {line.strip()}")
