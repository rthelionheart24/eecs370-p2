	lw	0	1	Stack
	lw	0	2	Eight
	beq	0	0	next
A	add	2	3	3
	nor	1	1	2
next	sw	0	2	OffV
	halt
Seven	.fill	7
Ten	.fill	Seven
