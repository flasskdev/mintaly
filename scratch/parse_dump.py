import struct
import sys

def parse_dump(dump_path):
    with open(dump_path, 'rb') as f:
        sig, ver, n_streams, dir_rva = struct.unpack('<IIII', f.read(16))
        print(f"Sig: {hex(sig)}, Streams: {n_streams}")
        f.seek(dir_rva)
        streams = []
        for _ in range(n_streams):
            stype, size, rva = struct.unpack('<III', f.read(12))
            streams.append((stype, size, rva))

        modules = []
        for stype, size, rva in streams:
            if stype == 4: # ModuleList
                f.seek(rva)
                n_mods = struct.unpack('<I', f.read(4))[0]
                for _ in range(n_mods):
                    entry = f.read(108)
                    base, msize, chk, stamp, name_rva = struct.unpack('<QIIII', entry[:24])
                    cur = f.tell()
                    f.seek(name_rva)
                    nbytes = struct.unpack('<I', f.read(4))[0]
                    name = f.read(nbytes).decode('utf-16le', errors='ignore').rstrip('\0')
                    f.seek(cur)
                    modules.append((base, msize, name))

        for b, s, name in modules:
            if any(x in name.lower() for x in ['rendersystem', 'filesystem', 'cs2', 'mintaly', 'tier0']):
                print(f"  {name}: {b:#x} .. {b+s:#x}")

        for stype, size, rva in streams:
            if stype == 6: # ExceptionStream
                f.seek(rva)
                # MINIDUMP_EXCEPTION_STREAM:
                # ULONG32 ThreadId
                # ULONG32 __alignment
                # MINIDUMP_EXCEPTION ExceptionRecord:
                #   ULONG32 ExceptionCode
                #   ULONG32 ExceptionFlags
                #   ULONG64 ExceptionRecord
                #   ULONG64 ExceptionAddress
                #   ULONG32 NumberParameters
                #   ULONG32 __unusedAlignment
                #   ULONG64 ExceptionInformation[15]
                # MINIDUMP_LOCATION_DESCRIPTOR ThreadContext
                tid, _, code, flags, rec, addr, nparams, _ = struct.unpack('<IIIIQQII', f.read(40))
                print(f"\nException: TID={tid} Code={code:#x} Addr={addr:#x} Flags={flags:#x}")
                params = [struct.unpack('<Q', f.read(8))[0] for _ in range(nparams)]
                print(f"  Params: {[hex(p) for p in params]}")
                found_mod = False
                for b, s, name in modules:
                    if b <= addr < b + s:
                        print(f"  In module: {name} (RVA: {addr-b:#x})")
                        found_mod = True
                        break
                if not found_mod:
                    print(f"  Address {addr:#x} NOT in any loaded module!")

                # Thread context location
                ctx_size, ctx_rva = struct.unpack('<II', f.read(8))
                f.seek(ctx_rva)
                ctx = f.read(ctx_size)
                # In x64 CONTEXT:
                # 0x78: Rip (offset 248 = 0xf8 in x64 CONTEXT)
                # Let's unpack standard registers:
                # ContextFlags is at 0x30
                # Rax=0x78, Rcx=0x80, Rdx=0x88, Rbx=0x90, Rsp=0x98, Rbp=0xa0, Rsi=0xa8, Rdi=0xb0, R8..R15=0xb8..0xf0, Rip=0xf8, EFlags=0x100
                if len(ctx) >= 0x108:
                    rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi = struct.unpack_from('<8Q', ctx, 0x78)
                    r8, r9, r10, r11, r12, r13, r14, r15 = struct.unpack_from('<8Q', ctx, 0xb8)
                    rip, eflags = struct.unpack_from('<QI', ctx, 0xf8)
                    print(f"\nRegisters at crash:")
                    print(f"  RIP={rip:#x} RSP={rsp:#x} RBP={rbp:#x}")
                    print(f"  RAX={rax:#x} RBX={rbx:#x} RCX={rcx:#x} RDX={rdx:#x}")
                    print(f"  RSI={rsi:#x} RDI={rdi:#x} R8={r8:#x} R9={r9:#x}")
                    print(f"  R10={r10:#x} R11={r11:#x} R12={r12:#x} R13={r13:#x} R14={r14:#x} R15={r15:#x}")

if __name__ == '__main__':
    parse_dump(sys.argv[1] if len(sys.argv) > 1 else r'C:\CrashDumps\cs2.exe_260925_144853.dmp')
