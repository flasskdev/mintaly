p = r'C:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project\core\hooks\impl\cheat.cpp'
d = open(p, 'rb').read()

# Step 1: Insert helper function in detail namespace after dispatch_lobby_music_guarded
old1 = b'\t\t}\r\n\r\n\t\tstruct viewmodel_anim_state'

new1 = b'\t\t}\r\n\r\n\t\t// SEH-safe swap chain validation: returns true if the swap chain is valid.\r\n\t\t// Must be in a separate function without C++ destructors (MSVC C2712).\r\n\t\tbool is_swap_chain_valid( IDXGISwapChain* swap_chain )\r\n\t\t{\r\n\t\t\tif ( !swap_chain )\r\n\t\t\t\treturn false;\r\n\r\n\t\t\t// Check vtable pointer\r\n\t\t\t__try\r\n\t\t\t{\r\n\t\t\t\tif ( !*reinterpret_cast<std::uintptr_t*>( swap_chain ) )\r\n\t\t\t\t\treturn false;\r\n\t\t\t}\r\n\t\t\t__except ( EXCEPTION_EXECUTE_HANDLER )\r\n\t\t\t{\r\n\t\t\t\treturn false;\r\n\t\t\t}\r\n\r\n\t\t\t// Check device pointer - a destroyed swap chain may have valid vtable but NULL device\r\n\t\t\tID3D11Device* device = nullptr;\r\n\t\t\t__try\r\n\t\t\t{\r\n\t\t\t\tif ( FAILED( swap_chain->GetDevice( __uuidof( ID3D11Device ), reinterpret_cast< void** >( &device ) ) ) || !device )\r\n\t\t\t\t\treturn false;\r\n\t\t\t\tdevice->Release( );\r\n\t\t\t}\r\n\t\t\t__except ( EXCEPTION_EXECUTE_HANDLER )\r\n\t\t\t{\r\n\t\t\t\treturn false;\r\n\t\t\t}\r\n\r\n\t\t\treturn true;\r\n\t\t}\r\n\r\n\t\t// SEH-safe original Present call: wraps the trampoline call in SEH to catch\r\n\t\t// any access violations inside the original Present function.\r\n\t\t// Must be in a separate function without C++ destructors (MSVC C2712).\r\n\t\tHRESULT call_present_safe( hooking::jmp& hook, IDXGISwapChain* thisptr, UINT sync_interval, UINT flags )\r\n\t\t{\r\n\t\t\t__try\r\n\t\t\t{\r\n\t\t\t\treturn hook.call<HRESULT>( thisptr, sync_interval, flags );\r\n\t\t\t}\r\n\t\t\t__except ( EXCEPTION_EXECUTE_HANDLER )\r\n\t\t\t{\r\n\t\t\t\treturn E_FAIL;\r\n\t\t\t}\r\n\t\t}\r\n\r\n\t\tstruct viewmodel_anim_state'

n1 = d.count(old1)
print('step1 occurrences:', n1)
assert n1 == 1
d = d.replace(old1, new1)

# Step 2: Add validation call in present function
old2 = b'\t\tstatic auto last_sub_check = std::chrono::steady_clock::now( );'

new2 = b'\t\t// Validate swap chain vtable pointer\r\n\t\tif ( !memory::safe_read<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( thisptr ) ).value_or( 0 ) )\r\n\t\t\treturn E_FAIL;\r\n\r\n\t\t// Additional validation: check if the swap chain\'s device pointer is valid\r\n\t\tif ( !detail::is_swap_chain_valid( thisptr ) )\r\n\t\t\treturn E_FAIL;\r\n\r\n\t\tstatic auto last_sub_check = std::chrono::steady_clock::now( );'

n2 = d.count(old2)
print('step2 occurrences:', n2)
assert n2 == 1
d = d.replace(old2, new2)

# Step 3: Change the final call to use the SEH-safe wrapper
old3 = b'\t\treturn m_present.call<HRESULT>( thisptr, sync_interval, flags );\r\n\t}\r\n\r\n\tHRESULT __fastcall cheat::resize_buffers'

new3 = b'\t\t// The original Present may fault on a half-released swap chain during\r\n\t\t// level transitions. ESP/box have already rendered above; never let the\r\n\t\t// engine\'s own Present take the process down with it.\r\n\t\treturn detail::call_present_safe( m_present, thisptr, sync_interval, flags );\r\n\t}\r\n\r\n\tHRESULT __fastcall cheat::resize_buffers'

n3 = d.count(old3)
print('step3 occurrences:', n3)
assert n3 == 1
d = d.replace(old3, new3)

open(p, 'wb').write(d)
print('written', len(d))