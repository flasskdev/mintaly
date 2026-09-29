import sys
sys.path.insert(0, r'mintaly-cs2\project\scratch')
import scan_patterns
import re

with open(r'mintaly-cs2\project\protection\patterns.cpp', 'r', encoding='utf-8', errors='ignore') as f:
    text = f.read()

entries = re.findall(r'(\w+)\s*=\s*ADDRESS_IMPL\s*\([^;]+?\"([^\"]+)\"\s*\);', text)

for name, sig in entries:
    mod = sig.split(':', 1)[0]
    pat_str = sig.split(':', 1)[1]
    img = scan_patterns.load_image(mod)
    if img:
        res, cnt = scan_patterns.resolve(img, pat_str)
        if res:
            if mod in ['engine2.dll', 'tier0.dll']:
                print(f'{mod:20s} {name:30s} {hex(res):12s} {pat_str}')
            if res == 0x6edf98 or abs(res - 0x6edf98) < 0x1000:
                print(f'*** MATCH NEAR 0x6edf98: {mod} {name} -> {hex(res)} ***')
