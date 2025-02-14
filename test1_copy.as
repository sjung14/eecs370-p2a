	lw	0	1	Kavin	load r1 = 50
	noop
	lw	0	2	incre	load r2 = 1
	lw	0	3	Four	load r3 = 4
	add	2	3	4	r4 = r2 + r3 = 5
	add	4	4	4	r4 = r4 * 2 = 10
	sw	0	4	One	store r4 in line 2
	lw	0	5	1	load r5 = 10
start	add	4	5	5
	beq	1	5	end
	beq	0	0	start
end	halt
kavin	.fill	Kavin
incre	.fill	1
Four	.fill	4
