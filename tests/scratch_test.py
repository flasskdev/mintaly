import struct, lz4.block, json, re

with open('C:/mintaly/configs/legit sc.cfg', 'rb') as f:
    data = f.read()

orig_size = struct.unpack('<I', data[:4])[0]
decompressed = lz4.block.decompress(data[4:], uncompressed_size=orig_size)
j = json.loads(decompressed.decode('utf-8'))
fields = j.get('fields', {})

def fnv1a(cat, name):
    full = cat + '.' + name
    val = 14695981039346656037
    for ch in full.encode('utf-8'):
        val = (val ^ ch) * 1099511628211
        val &= 0xFFFFFFFFFFFFFFFF
    return f'{val & 0xFFFFFFFF:08x}'

# Check all possible legit categories
cats = [
    'legitbot',
    'legitbot - pistol', 'legitbot - smg', 'legitbot - rifle', 'legitbot - shotgun', 'legitbot - sniper', 'legitbot - lmg',
    'legitbot - deagle', 'legitbot - duals', 'legitbot - fiveseven', 'legitbot - glock', 'legitbot - tec9',
    'legitbot - p2000', 'legitbot - p250', 'legitbot - usps', 'legitbot - cz75', 'legitbot - revolver',
    'legitbot - mac10', 'legitbot - mp9', 'legitbot - mp7', 'legitbot - mp5sd', 'legitbot - ump45', 'legitbot - p90', 'legitbot - bizon',
    'legitbot - ak47', 'legitbot - m4a4', 'legitbot - m4a1s', 'legitbot - galil', 'legitbot - famas', 'legitbot - aug', 'legitbot - sg553',
    'legitbot - nova', 'legitbot - xm1014', 'legitbot - mag7', 'legitbot - sawedoff',
    'legitbot - awp', 'legitbot - ssg08', 'legitbot - scar20', 'legitbot - g3sg1',
    'legitbot - m249', 'legitbot - negev',
    'autos'
]

names = [
    'enabled', 'override',
    'aimbot', 'fov', 'smooth', 'hitboxes', 'visualize fov', 'fov color',
    'rcs', 'rcs min', 'rcs max',
    'standalone rcs', 'standalone rcs strength', 'standalone rcs min', 'standalone rcs max',
    'triggerbot', 'trigger delay', 'trigger hitchance', 'trigger head only', 'trigger seed mode',
    'autowall', 'min damage',
    'smoke check', 'smoke radius', 'smoke lifetime',
    'scope check', 'auto scope', 'flash check', 'ground check',
    'auto revolver', 'revolver quick shot'
]

for c in cats:
    for n in names:
        k = fnv1a(c, n)
        if k in fields:
            val = fields[k]
            # print if not false
            if isinstance(val, dict):
                if val.get('v') or val.get('b'):
                    print(f'{c}.{n} = {val}')
            elif val != False and val != 0 and val != 0.0:
                print(f'{c}.{n} = {val}')
