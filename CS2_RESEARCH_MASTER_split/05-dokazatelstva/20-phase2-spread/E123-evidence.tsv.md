<!-- split-part | CS2_RESEARCH_MASTER.md lines 72629-72689 | body-sha256 945c3f2ce1fc5560b71d5e26ae8df88470ef12d92e152b4054115671dabd6684 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-123"></a>

## E123. `analysis/phase2/spread/evidence.tsv`

Bytes: 3519. SHA-256: `b92252d8b95b6cb8da983323630d504a665380d9ad295380d05088a6165745c4`.

```tsv
owner	site	original_bytes_12	meaning
0x525830	0x52589e	498b4018c5fa10401cc44138	source=candidate+0x18; scale=source+0x1c
0x525830	0x525936	e80576ffff498b4710488984	call 0x51cf40(frame,candidate,1)
0x525830	0x525911	4c8b70204c898424d8010000	samples=source+0x20 ring-cache pointer
0x525830	0x525993	488d05cebf9b004889842480	vtable 0xee1968
0x525830	0x5259a2	c78424880200004000000048	job.count=64
0x525830	0x5259fb	ff9098000000f30fb88424b8	virtual dispatch +0x98
0x525830	0x525a01	f30fb88424b8000000038424	popcount mask A low
0x525830	0x525a11	f30fb88c24bc00000001c189	popcount mask A high
0x525830	0x525a2f	c4e279184814c5f810bc24c8	threshold=candidate->+0x10->+0x14
0x525830	0x525b6b	c5aa59dbc5f85bc9c5aa59c9	mask A count divided by 64
0x525830	0x525b73	c5aa59c9488b400883785000	score threshold count divided by 64
0x525830	0x525ffc	c57a5905bc2a9700c5f857c0	mask B count divided by 64
0x525830	0x52613c	c5fa111ec5fa114e04c5fa58	out[0]
0x525830	0x526140	c5fa114e04c5fa5884246c01	out[1]
0x525830	0x526214	c57a114608c5fa59059f2897	out[2]
0x525830	0x526219	c5fa59059f289700c5fa1146	sum(score)/64
0x525830	0x526221	c5fa11460cc5f828b5a00200	out[3]
0x524a30	0x524d66	e8c50a00004c8d7f44c5f810	call aggregate 0x525830
0x524a30	0x524d77	c5f81147444889f14889fae8	copy float4 to candidate+0x44
0x51d320	0x51d400	c4a279180ce0c5f259411cc4	load sample.x
0x51d320	0x51d40b	c4a2791854e004c5ea595928	load sample.y
0x51d320	0x51d558	e863050000c5782ec00f8377	call shared evaluator 0x51dac0
0x51d320	0x51d585	c4a17a1144a040498b451848	store score[index]
0x51d320	0x51d593	c5f82e40180f8232feffff49	mask A threshold at target+0x18
0x51d320	0x51d5b5	f04409448830498b4518488b	atomic mask A
0x51d320	0x51d5ce	0fb6403924fe3c020f8502fe	mask B record byte+0x39
0x51d320	0x51d5f3	f04409448838e9e0fdffffc5	atomic mask B
0x512710	0x512b84	c57a590574709800c5fa5905	ring radius /8
0x512710	0x512b8c	c5fa5905c48d9800c57a5ac8	ring angle *pi/8
0x512710	0x512d4c	c57a51c8c5d22ac6c5ba59f0	golden radius sqrt((sample+1)/64)
0x512710	0x512d54	c5ba59f0c5f828c6e8fffd87	golden angle *sample
0x5239d0	0x5241bc	c4c17a107f1c4885ff74b94c	ring scale source record+0x1c
0x5239d0	0x5242b7	c5f8590dc1032301c5f85915	ring table vector scaling
0x524a30	0x524ffd	c5f8590dbbf82201c5f85915	golden table vector scaling
0x51c9c0	0x51cc3b	c5fa59057dbe9700b301c5f8	coverage count /64
0x515ca0	0x51794f	ff1503ada700c5fa59057b4e	RandomFloat angular offset, not sample loop
0x526290	0x526720	b807000000c7442434000000	fast block index starts at 7
0x526290	0x526760	488b8424e80000004883e801	decrement blocks, exit after 0
0x526290	0x52678f	c5fc280401c5fc284c0120c5	load first 32 bytes of eight Vec2 batch
0x526290	0x526794	c5fc284c0120c5fcc6d122c5	load second 32 bytes of eight Vec2 batch
0x526290	0x528187	0f97c3015c243424fe4531f6	fast thresholdB uses strict seta (score > B)
0x526290	0x5286ef	f6c3f70f94c24420e120d180	early fill requires block countB 0 or 8
0x526290	0x528703	c5fa5905f5149700c5ba58c0	block mean: score times 1/8
0x526290	0x528717	01f8488944243831c083fb08	fill remaining countA
0x526290	0x52872b	014c24344183fe080f44c701	conditional fill remaining countB
0x526290	0x528736	01c6c5d22a8c24e8000000c5	conditional fill remaining category count
0x526290	0x52874d	c5fa58ff488b442450488b40	extrapolate sumScore with block mean
0x526290	0x528769	c5fa100d4f039700c5fa59c1	fixed divisor 64
0x526290	0x528795	c5f8590583339700c5f81340	out1/out2 vector normalization by 64
0x526290	0x5287a2	c5fa11480c488b8424a80700	out3 mean score
```
