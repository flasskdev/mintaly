import struct

DLL = r'C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\csgo\bin\win64\client.dll'
SECS = [
    (b'.text', 0x1AA34C0, 0x1000, 0x400),
    (b'.rdata', 0x710FE6, 0x1AA5000, 0x1AA3A00),
    (b'.data', 0x5E308C, 0x21B6000, 0x21B4A00),
]

def rva_to_off(rva):
    for name, vsz, vaddr, rptr in SECS:
        if vaddr <= rva < vaddr + vsz:
            return rptr + (rva - vaddr)
    return None

def read_at(off, n):
    with open(DLL, 'rb') as f:
        f.seek(off)
        return f.read(n)

def hexdump(rva, n=96):
    off = rva_to_off(rva)
    b = read_at(off, n)
    for i in range(0, n, 16):
        chunk = b[i:i+16]
        # NOTE: left column is file offset, RVA in parens (Raw != RVA: .text RPtr=0x400, VAddr=0x1000).
        print('%08X (RVA %08X): %s' % (off+i, rva+i, ' '.join('%02X' % x for x in chunk)))

def parse_pat(s):
    toks = s.split(' ')
    out = []
    for t in toks:
        if t in ('?', '??'):
            out.append(None)
        else:
            out.append(int(t, 16))
    return out

TEXT_RVA = 0x1000
TEXT_OFF = 0x400
TEXT_SIZE = 0x1AA34C0

def off_to_rva(off):
    return TEXT_RVA + (off - TEXT_OFF)

def scan(pat_s, cap=25):
    pat = parse_pat(pat_s)
    n = len(pat)
    first = next(i for i, x in enumerate(pat) if x is not None)
    fb = pat[first]
    hits = []
    with open(DLL, 'rb') as f:
        f.seek(TEXT_OFF)
        buf = f.read(TEXT_SIZE)
    # naive but chunked .text search
    for i in range(0, len(buf) - n):
        if buf[i+first] != fb:
            continue
        ok = True
        for j in range(n):
            if pat[j] is None:
                continue
            if buf[i+j] != pat[j]:
                ok = False
                break
        if ok:
            # base+i is file off; convert back: rva = vaddr + (off - rptr)
            off = TEXT_OFF + i
            rva = 0x1000 + (off - 0x400)
            hits.append(rva)
            if len(hits) >= cap:
                break
    return hits

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == 'dump':
        hexdump(int(sys.argv[2], 16), int(sys.argv[3]) if len(sys.argv) > 2 else 96)
    elif len(sys.argv) > 1 and sys.argv[1] == 'scan':
        for h in scan(sys.argv[2]):
            print('HIT %08X' % h)
