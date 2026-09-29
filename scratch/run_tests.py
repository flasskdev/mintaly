import subprocess, sys, io, os

base = r'C:\Users\stass\AppData\Local\Python\pythoncore-3.14-64\python.exe'
mods = ['tests.fix_source_checks', 'tests.runtime_followup_checks']
r = subprocess.run([base, '-m', 'unittest'] + mods, capture_output=True, text=True, errors='replace')
open(r'C:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project\scratch\ut.log', 'w', encoding='utf-8').write(r.stdout + '\n---STDERR---\n' + r.stderr)
print(r.stdout[-5000:] + r.stderr[-3000:])