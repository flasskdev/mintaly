"""Minimal minidump parser: exception context + module list + fault-thread stack.

Usage: py -3 scratch/read_dump.py <path-to.dmp>
"""
import struct
import sys
from pathlib import Path

MINIDUMP_MODULE_LIST = 4
MINIDUMP_THREAD_LIST = 3
MINIDUMP_EXCEPTION_STREAM = 6


def read(path):
    data = Path(path).read_bytes()
    # MINIDUMP_HEADER: Signature u32@0, Version u32@4, NumberOfStreams u32@8, StreamDirectoryRVA u32@12
    sig, ver, nstreams, dir_rva = struct.unpack_from('<IIII', data, 0)
    assert sig == 0x504D444D, hex(sig)
    streams = []
    for i in range(nstreams):
        # MINIDUMP_DIRECTORY: StreamType u32, Location{DataSize u32, Rva u32}
        stype, dsize, rva = struct.unpack_from('<III', data, dir_rva + i * 12)
        streams.append((stype, rva, dsize))
    out = {'modules': [], 'exception': None, 'threads': []}

    for stype, sloc, ssize in streams:
        if stype == MINIDUMP_MODULE_LIST:
            count, = struct.unpack_from('<I', data, sloc)
            for i in range(count):
                base, = struct.unpack_from('<Q', data, sloc + 4 + i * 108)
                size, = struct.unpack_from('<I', data, sloc + 4 + i * 108 + 8)
                # MINIDUMP_MODULE: BaseOfImage(8) SizeOfImage(4) CheckSum(4) TimeDateStamp(4) ModuleNameRva(4)
                name_rva, = struct.unpack_from('<I', data, sloc + 4 + i * 108 + 20)
                nlen, = struct.unpack_from('<I', data, name_rva)
                name = data[name_rva + 4:name_rva + 4 + nlen].decode('utf-16-le', 'replace')
                out['modules'].append((name, base, size))
        elif stype == MINIDUMP_EXCEPTION_STREAM:
            tid, = struct.unpack_from('<I', data, sloc)
            print(f'[debug] exception stream @0x{sloc:X} size=0x{ssize:X}', file=sys.stderr)
            hdr = data[sloc:sloc + 176]
            print('[debug] ' + ' '.join(f'{b:02X}' for b in hdr[:96]), file=sys.stderr)
            print('[debug] ' + ' '.join(f'{b:02X}' for b in hdr[96:176]), file=sys.stderr)
            # MINIDUMP_EXCEPTION_STREAM: ThreadId u32, __alignment u32,
            # then MINIDUMP_EXCEPTION: Code u32, Flags u32, Record u64, Addr u64,
            # NumParams u32, unused u32, Info[15] u64 = 4+4+8+8+4+4+120 = 152
            # total to ThreadContext = 8 + 152 = 160
            code, flags, exc_addr = struct.unpack_from('<IIQ', data, sloc + 8)
            ctx_loc = sloc + 160
            ctx_size, ctx_rva = struct.unpack_from('<II', data, ctx_loc)
            print(f'[debug] ctx_size={ctx_size} ctx_rva=0x{ctx_rva:X}', file=sys.stderr)
            if ctx_rva + min(ctx_size, 0x4B0) > len(data) or ctx_size == 0:
                # fallback: context may be directly embedded; try scanning for plausible RIP later
                ctx = b''
            else:
                ctx = data[ctx_rva:ctx_rva + ctx_size]
            rip = rsp = rbp = 0
            if len(ctx) >= 0x100:
                rip, = struct.unpack_from('<Q', ctx, 0xF8)
                rsp, = struct.unpack_from('<Q', ctx, 0x98)
                rbp, = struct.unpack_from('<Q', ctx, 0xA8)
            out['exception'] = {'tid': tid, 'code': code, 'addr': exc_addr,
                                'rip': rip, 'rsp': rsp, 'rbp': rbp}
        elif stype == MINIDUMP_THREAD_LIST:
            count, = struct.unpack_from('<I', data, sloc)
            for i in range(count):
                base = sloc + 4 + i * 48
                tid, susp, prio, teb = struct.unpack_from('<IIIQ', data, base)
                # STACK descriptor at base+24: StartOfMemoryRange(8) DataSize(4) Rva(4)
                smr, dsize, drva = struct.unpack_from('<QII', data, base + 24)
                out['threads'].append({'tid': tid, 'teb': teb, 'stack_base': smr,
                                       'stack': data[drva:drva + dsize]})
    return out


def main():
    d = read(sys.argv[1])
    exc = d['exception']
    print(f"exception: code=0x{exc['code']:08X} addr=0x{exc['addr']:X} tid={exc['tid']}")
    print(f"rip=0x{exc['rip']:X} rsp=0x{exc['rsp']:X} rbp=0x{exc['rbp']:X}")
    print('\nmodules:')
    for name, base, size in d['modules']:
        if any(k in name.lower() for k in ('client', 'engine', 'mintaly', 'server', 'panorama')):
            print(f'  {name} base=0x{base:X} size=0x{size:X}')

    def to_mod(addr):
        for name, base, size in d['modules']:
            if base <= addr < base + size:
                return name, addr - base
        return None, None

    for th in d['threads']:
        if th['tid'] != exc['tid']:
            continue
        print(f"\nfault thread {th['tid']} stack @0x{th['stack_base']:X} ({len(th['stack'])} bytes):")
        seen = 0
        for off in range(0, len(th['stack']) - 7, 8):
            val, = struct.unpack_from('<Q', th['stack'], off)
            name, rva = to_mod(val)
            if name is None:
                continue
            sp = th['stack_base'] + off
            if sp < exc['rsp']:
                continue
            print(f"  [rsp=0x{sp:X}] {name}+0x{rva:X}")
            seen += 1
            if seen >= 40:
                break
        break


if __name__ == '__main__':
    main()
