	lw	0	1	mcand
	lw	0	2	mplier	use r2 to modify
	lw	0	3	zero	r3 = 0; store result in r3
	lw	0	4	one	r4 = 1; r4 will mask mplier
	lw	0	5	i	r5 = 1000000000000000
	nor	2	2	2	r2 = ~r2
s1	nor	4	4	4	r4 = ~r4
	nor	2	4	7	r7 = r2 & r4 (line 7-9)
	beq	7	0	noadd	if r7 == 0, don't add; skip a line
	add	1	3	3	r3 = r3 + r1
noadd	noop
	add	1	1	1	r1 = r1 << 1;
	nor	4	4	4	r4 = r4
	add	4	4	4	r4 = r4 << 1;
	beq	4	5	e1	if r4 == r5, end loop!
	beq	0	0	s1
e1	noop
end	halt
mcand	.fill	32767
mplier	.fill	32767
one	.fill	1
i	.fill	32768
zero	.fill	0
