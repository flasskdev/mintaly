p = r'C:\Users\stass\OneDrive\Documents\OneTap\gaycity\mintaly-cs2\project\core\hooks\impl\cheat.cpp'
d = open(p, 'rb').read()

# Remove the create_move entry trace
old1 = b'\tvoid __fastcall cheat::create_move( std::uintptr_t thisptr, int slot, bool active )\n\t{\n\t\tdiag::step( "create_move: entry" );\n\t\tsettings::enforce_safe_mode();'
new1 = b'\tvoid __fastcall cheat::create_move( std::uintptr_t thisptr, int slot, bool active )\n\t{\n\t\tsettings::enforce_safe_mode();'
n1 = d.count(old1)
print('create_move entry occurrences', n1)
assert n1 == 1
d = d.replace(old1, new1)

# Remove the fsn lobby path trace
old2 = b'\t\t\tm_frame_stage_notify.call<void>( thisptr, stage );\n\t\t\tif ( stage == 12 ) diag::step( "fsn: stage 12 lobby path" );\n\t\t\t// Lobby work does not require a map-owned controller or camera.'
new2 = b'\t\t\tm_frame_stage_notify.call<void>( thisptr, stage );\n\t\t\t// Lobby work does not require a map-owned controller or camera.'
n2 = d.count(old2)
print('fsn lobby path occurrences', n2)
assert n2 == 1
d = d.replace(old2, new2)

open(p, 'wb').write(d)
print('written', len(d))