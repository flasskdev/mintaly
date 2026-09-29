p = r'C:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project\core\hooks\impl\cheat.cpp'
d = open(p, 'rb').read()

# Find a good place to insert the helper function - right before the present function
old1 = b'HRESULT __fastcall cheat::present( IDXGISwapChain* thisptr, UINT sync_interval, UINT flags )'

new1 = b'// SEH-safe swap chain validation: returns true if the swap chain is valid.\r\n// Must be in a separate function without C++ destructors (MSVC C2712).\r\nbool is_swap_chain_valid( IDXGISwapChain* swap_chain )\r\n{\r\n\tif ( !swap_chain )\r\n\t\treturn false;\r\n\r\n\t// Check vtable pointer\r\n\t__try\r\n\t{\r\n\t\tif ( !*reinterpret_cast<std::uintptr_t*>( swap_chain ) )\r\n\t\t\treturn false;\r\n\t}\r\n\t__except ( EXCEPTION_EXECUTE_HANDLER )\r\n\t{\r\n\t\treturn false;\r\n\t}\r\n\r\n\t// Check device pointer - a destroyed swap chain may have valid vtable but NULL device\r\n\tID3D11Device* device = nullptr;\r\n\t__try\r\n\t{\r\n\t\tif ( FAILED( swap_chain->GetDevice( __uuidof( ID3D11Device ), reinterpret_cast< void** >( &device ) ) ) || !device )\r\n\t\t\treturn false;\r\n\t\tdevice->Release( );\r\n\t}\r\n\t__except ( EXCEPTION_EXECUTE_HANDLER )\r\n\t{\r\n\t\treturn false;\r\n\t}\r\n\r\n\treturn true;\r\n}\r\n\r\nHRESULT __fastcall cheat::present( IDXGISwapChain* thisptr, UINT sync_interval, UINT flags )'

n1 = d.count(old1)
print('step1 occurrences:', n1)
assert n1 == 1
d = d.replace(old1, new1)

# Step 2: Add validation call in present function
old2 = b'\t\tif ( !memory::safe_read<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( thisptr ) ).value_or( 0 ) )\r\n\t\t\treturn E_FAIL;\r\n\r\n\t\tstatic auto last_sub_check = std::chrono::steady_clock::now( );'

new2 = b'\t\tif ( !memory::safe_read<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( thisptr ) ).value_or( 0 ) )\r\n\t\t\treturn E_FAIL;\r\n\r\n\t\tif ( !is_swap_chain_valid( thisptr ) )\r\n\t\t\treturn E_FAIL;\r\n\r\n\t\tstatic auto last_sub_check = std::chrono::steady_clock::now( );'

n1 = d.count(old1)
print('step1 occurrences:', n1)
assert n1 == 1
d = d.replace(old1, new1)

# Step 2: Change the final call to use safe_call
old2 = b'\t\treturn m_present.call<HRESULT>( thisptr, sync_interval, flags );\r\n\t}\r\n\r\n\tHRESULT __fastcall cheat::resize_buffers'

new2 = b'\t\t// The original Present may fault on a half-released swap chain during\r\n\t\t// level transitions. ESP/box have already rendered above; never let the\r\n\t\t// engine\'s own Present take the process down with it.\r\n\t\treturn m_present.safe_call<HRESULT>( thisptr, sync_interval, flags );\r\n\t}\r\n\r\n\tHRESULT __fastcall cheat::resize_buffers'

n2 = d.count(old2)
print('step2 occurrences:', n2)
assert n2 == 1
d = d.replace(old2, new2)

open(p, 'wb').write(d)
print('written', len(d))