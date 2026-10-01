<!-- split-part | CS2_RESEARCH_MASTER.md lines 72714-72844 | body-sha256 2c40208726c93d57c120b42379645164ec3067a5f082808f2bde28334de6eca6 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-125"></a>

## E125. `analysis/phase2/spread/export_evidence.py`

Bytes: 7332. SHA-256: `f45410058e7056fa4de7224d62367b0de676da9ca9b2cf5091ebb040e317cc5b`.

```python
#!/usr/bin/env python3
import ast
import csv
import hashlib
import json
import math
import struct

from inspect_spread import BASE, OUTPUT, IMAGE, database


def write_tsv(name, headers, rows):
    with (OUTPUT/name).open('w',newline='') as output:
        writer=csv.writer(output,delimiter='\t')
        writer.writerow(headers)
        writer.writerows(rows)


def main():
    image=IMAGE.read_bytes()
    assert len(image)==0x5001000
    connection=database()
    targets=[(0x525830,'64-sample scoring aggregate; primary'),(0x51cf40,'sample frame / scaled basis setup'),
             (0x51d320,'job callback; shared with point optimizer'),(0x512710,'two deterministic disk tables'),
             (0x5239d0,'ring-scale cache producer'),(0x524a30,'candidate aggregates + point optimization'),
             (0x51c9c0,'golden-angle coverage/point optimization'),(0x526290,'fast 8x8 sampling with early-fill extrapolation')]
    records=[]
    for begin,role in targets:
        end,unwind,decoded=connection.execute('SELECT end,unwind,decoded_end FROM functions WHERE begin=?',(begin,)).fetchone()
        records.append((hex(begin),hex(BASE+begin),hex(end),hex(unwind),hex(decoded),role))
    write_tsv('targets.tsv',['rva','va','end','unwind','original_decoded_end','role'],records)
    evidence=[
        (0x525830,0x52589e,'source=candidate+0x18; scale=source+0x1c'),
        (0x525830,0x525936,'call 0x51cf40(frame,candidate,1)'),
        (0x525830,0x525911,'samples=source+0x20 ring-cache pointer'),
        (0x525830,0x525993,'vtable 0xee1968'),
        (0x525830,0x5259a2,'job.count=64'),
        (0x525830,0x5259fb,'virtual dispatch +0x98'),
        (0x525830,0x525a01,'popcount mask A low'),
        (0x525830,0x525a11,'popcount mask A high'),
        (0x525830,0x525a2f,'threshold=candidate->+0x10->+0x14'),
        (0x525830,0x525b6b,'mask A count divided by 64'),
        (0x525830,0x525b73,'score threshold count divided by 64'),
        (0x525830,0x525ffc,'mask B count divided by 64'),
        (0x525830,0x52613c,'out[0]'),(0x525830,0x526140,'out[1]'),
        (0x525830,0x526214,'out[2]'),(0x525830,0x526219,'sum(score)/64'),(0x525830,0x526221,'out[3]'),
        (0x524a30,0x524d66,'call aggregate 0x525830'),(0x524a30,0x524d77,'copy float4 to candidate+0x44'),
        (0x51d320,0x51d400,'load sample.x'),(0x51d320,0x51d40b,'load sample.y'),
        (0x51d320,0x51d558,'call shared evaluator 0x51dac0'),(0x51d320,0x51d585,'store score[index]'),
        (0x51d320,0x51d593,'mask A threshold at target+0x18'),
        (0x51d320,0x51d5b5,'atomic mask A'),(0x51d320,0x51d5ce,'mask B record byte+0x39'),
        (0x51d320,0x51d5f3,'atomic mask B'),
        (0x512710,0x512b84,'ring radius /8'),(0x512710,0x512b8c,'ring angle *pi/8'),
        (0x512710,0x512d4c,'golden radius sqrt((sample+1)/64)'),(0x512710,0x512d54,'golden angle *sample'),
        (0x5239d0,0x5241bc,'ring scale source record+0x1c'),(0x5239d0,0x5242b7,'ring table vector scaling'),
        (0x524a30,0x524ffd,'golden table vector scaling'),(0x51c9c0,0x51cc3b,'coverage count /64'),
        (0x515ca0,0x51794f,'RandomFloat angular offset, not sample loop'),
        (0x526290,0x526720,'fast block index starts at 7'),
        (0x526290,0x526760,'decrement blocks, exit after 0'),
        (0x526290,0x52678f,'load first 32 bytes of eight Vec2 batch'),
        (0x526290,0x526794,'load second 32 bytes of eight Vec2 batch'),
        (0x526290,0x528187,'fast thresholdB uses strict seta (score > B)'),
        (0x526290,0x5286ef,'early fill requires block countB 0 or 8'),
        (0x526290,0x528703,'block mean: score times 1/8'),
        (0x526290,0x528717,'fill remaining countA'),
        (0x526290,0x52872b,'conditional fill remaining countB'),
        (0x526290,0x528736,'conditional fill remaining category count'),
        (0x526290,0x52874d,'extrapolate sumScore with block mean'),
        (0x526290,0x528769,'fixed divisor 64'),
        (0x526290,0x528795,'out1/out2 vector normalization by 64'),
        (0x526290,0x5287a2,'out3 mean score'),
    ]
    write_tsv('evidence.tsv',['owner','site','original_bytes_12','meaning'],
              [(hex(owner),hex(site),image[site:site+12].hex(),meaning) for owner,site,meaning in evidence])
    assert struct.unpack_from('<Q',image,0xee1970)[0]==BASE+0x51d320
    call_assertions={0x525936:0x51cf40,0x51d558:0x51dac0,0x524d66:0x525830,0x51ca46:0x51cf40,
                     0x524fee:0x51c9c0,0x525247:0x51c9c0}
    for site,target in call_assertions.items():
        assert image[site]==0xe8
        assert site+5+struct.unpack_from('<i',image,site+1)[0]==target
    switch_rows=[]
    for table,count,jump in ((0xf45c1c,6,0x5170fd),(0xf45c34,4,0x517929)):
        for index in range(count):
            target=table+struct.unpack_from('<i',image,table+index*4)[0]
            switch_rows.append((hex(jump),hex(table),index,hex(target),'read-only relative jump table; no runtime guard claim'))
    write_tsv('switch_edges.tsv',['jump_site','table','index','target','qualification'],switch_rows)
    constants=[(0xe99c00,'f32 ring radius factor','f'),(0xe9b958,'f32 ring base angle','f'),
               (0xe98ac0,'f32 denominator factor','f'),(0xe9b978,'f32 golden angle','f')]
    constants += [(offset,'f64 ring angular offset','d') for offset in (0xdeca20,0xdeca90,0xe86928,0xdecaf0,0xe9b960,0xe9b968,0xe9b970)]
    write_tsv('math_constants.tsv',['offset','meaning','bytes','value'],
              [(hex(offset),meaning,image[offset:offset+struct.calcsize(fmt)].hex(),repr(struct.unpack_from('<'+fmt,image,offset)[0])) for offset,meaning,fmt in constants])
    sample_rows=[]
    golden=struct.unpack_from('<f',image,0xe9b978)[0]
    for sample in range(64):
        ring=sample//8+1
        sector=sample%8
        for table,radius,angle in (('ring_formula',ring/8,ring*math.pi/8+sector*math.pi/4),
                                   ('golden_formula',math.sqrt((sample+1)/64),sample*golden)):
            sample_rows.append((table,sample,radius,angle,radius*math.cos(angle),radius*math.sin(angle)))
    write_tsv('formula_samples.tsv',['model_not_runtime_dump','index','radius','angle','x','y'],sample_rows)
    for path in OUTPUT.glob('*.py'):
        ast.parse(path.read_text(),filename=str(path))
    patch_count=0
    for path in list(OUTPUT.glob('*.decode.json'))+list(OUTPUT.glob('*.cfg.json')):
        log=json.loads(path.read_text())
        for change in log['patches']:
            offset=int(change['offset'],16)
            assert image[offset:offset+4].hex()==change['original']
            assert change['replacement']=='90909090'
            patch_count+=1
    audit={'source_sha256':hashlib.sha256(image).hexdigest(),'size':hex(len(image)),
           'table_zero_bytes':{hex(offset):sum(value==0 for value in image[offset:offset+512]) for offset in (0x1754680,0x17548c0)},
           'direct_calls_verified':len(call_assertions),'callback_pointer_verified':True,'patch_log_entries_verified_in_original':patch_count,
           'scripts_syntax_parsed':True,'sample_code_executed':False,
           'formula_samples':'independent mathematical illustrations, not actual dump table contents or bit-exact binary execution'}
    (OUTPUT/'evidence_audit.json').write_text(json.dumps(audit,indent=2)+'\n')
    print(json.dumps(audit,indent=2))


if __name__=='__main__':
    main()
```
