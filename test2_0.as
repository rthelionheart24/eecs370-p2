	lw	0	1	start
	lw	0	2	four
start	jalr	4	7
	beq	0	1	done
done	halt
four	.fill	4
	.fill	done
