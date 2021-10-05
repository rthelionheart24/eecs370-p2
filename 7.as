Dup	lw	0	1	ct
	lw	0	2	limit
	lw	0	3	one
loop	beq	1	2	done
	add	1	3	1
	beq	0	0	loop
done	noop
	sw	0	1	1000
	halt
	jalr	4	7
ct	.fill	0
limit	.fill	7
one	.fill	1
Dup	.fill	3
