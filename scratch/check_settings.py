import re
import os

with open(r"c:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project\core\settings.hpp", "r", encoding="utf-8") as f:
    content = f.read()

# Find all xui::setting declarations
matches = re.findall(r'xui::setting\s+([a-zA-Z0-9_]+)\s*(\{[^}]*\})?', content)
print(f"Total xui::setting found: {len(matches)}")
for name, init in matches:
    if not init or init == '{}':
        print(f"Uninitialized or default: {name}")
    elif '""' in init:
        print(f"Empty string in init: {name}: {init}")
