	lw	0	1	var	Loading x1
	lw	0	2	top
	lw	0	3	one
loop	beq	1	2	done
	add	1	3	1	Adding to x1
	beq	0	0	World
	beq	0	2	-5
done	noop				10
	sw	0	1	1
	sw	0	2	done
	sw	0	1	-1
	halt				Quit the program
	jalr	4	7		3
var	.fill	1
top	.fill	7
one	.fill	1
Globe	.fill	4
