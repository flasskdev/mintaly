p = r'C:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project\core\hooks\impl\cheat.cpp'
d = open(p, 'rb').read()

old = b'\t{\r\n\t\tdiag::exception_scope scope{ "present" };\r\n\t\tif ( lifecycle::is_unloading( ) )\r\n\t\t{\r\n\t\t\treturn m_present.call<HRESULT>( thisptr, sync_interval, flags );\r\n\t\t}\r\n\r\n\t\tstatic auto last_sub_check = std::chrono::steady_clock::now( );'

new = b'\t{\r\n\t\tdiag::exception_scope scope{ "present" };\r\n\t\tif ( lifecycle::is_unloading( ) )\r\n\t\t{\r\n\t\t\treturn m_present.call<HRESULT>( thisptr, sync_interval, flags );\r\n\t\t}\r\n\r\n\t\t// A NULL or corrupt swap-chain pointer dereferences [thisptr+0x10] inside\r\n\t\t// the original Present and faults. Fail closed instead of crashing the game.\r\n\t\tif ( !thisptr )\r\n\t\t\treturn E_FAIL;\r\n\t\t// safe_read runs its own SEH frame, so no __try here (MSVC C2712).\r\n\t\t// Reads the vtable pointer stored at the swap-chain address.\r\n\t\tif ( !memory::safe_read<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( thisptr ) ).value_or( 0 ) )\r\n\t\t\treturn E_FAIL;\r\n\r\n\t\tstatic auto last_sub_check = std::chrono::steady_clock::now( );'

n = d.count(old)
print('occurrences', n)
assert n == 1
d = d.replace(old, new)
open(p, 'wb').write(d)
print('written', len(d))