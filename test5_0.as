First	lw	0	1	five
	lw	0	2	nine
	lw	0	3	Dude
	lw	0	4	1
	lw	0	5	First
begin	add	5	2	5
	nor	6	1	6
Calc	add	6	6	6
store	sw	2	3	Nine
	sw	3	1	1
	noop
	halt
	.fill	1
	.fill	Nine
	.fill	five
five	.fill	5
nine	.fill	store
ten	.fill	Seven
Nine	.fill	9
Eight	.fill	five
Seven	.fill	Nine
