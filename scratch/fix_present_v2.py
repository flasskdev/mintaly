p = r'C:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project\core\hooks\impl\cheat.cpp'
d = open(p, 'rb').read()

old1 = b'\t\tstatic auto last_sub_check = std::chrono::steady_clock::now( );'

new1 = b'\t\t// Additional validation: check if the swap chain\'s device pointer is valid.\r\n\t\t// A destroyed swap chain during level transition may have a valid vtable\r\n\t\t// but a NULL device pointer, causing the engine\'s Present to crash.\r\n\t\tIDXGISwapChain* swap_chain = thisptr;\r\n\t\tID3D11Device* device = nullptr;\r\n\t\t__try\r\n\t\t{\r\n\t\t\tif ( FAILED( swap_chain->GetDevice( __uuidof( ID3D11Device ), reinterpret_cast< void** >( &device ) ) ) || !device )\r\n\t\t\t\treturn E_FAIL;\r\n\t\t\tdevice->Release( );\r\n\t\t}\r\n\t\t__except ( EXCEPTION_EXECUTE_HANDLER )\r\n\t\t{\r\n\t\t\treturn E_FAIL;\r\n\t\t}\r\n\r\n\t\tstatic auto last_sub_check = std::chrono::steady_clock::now( );'

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