	lw	0	1	Kavin	load r1 = -50
	lw	0	3	dinner
	lw	0	2	lunch
	lw	0	4	ten
start	add	1	4	1	r1 = r1 + r4
	beq	2	1	end
	beq	0	0	start
	add	3	3	3
end	noop
	lw	0	6	Four
	add	4	6	4
	jalr	4	5		r5 = 12; pc = 10;
	add	1	1	1
	add	1	1	1
	halt
Kavin	.fill	-50
lunch	.fill	Four
dinner	.fill	lunch
ten	.fill	10
zero	.fill	0
four	.fill	4
One	.fill	1
