	lw	0	1	five
	lw	0	4	SubAdr
start	jalr	4	7
	beq	0	1	done
SubAdr	add	0	1	2
	beq	0	0	start
done	halt
five	.fill	5
