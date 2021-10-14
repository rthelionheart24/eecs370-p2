	lw	0	1	Off
	lw	0	2	Six
	lw	0	2	Temp
	lw	0	3	Seven
	lw	0	4	A
	lw	0	5	Ten
Off	beq	0	0	end
end	halt
Six	.fill	6
OffV	.fill	Off
Temp	.fill	Six
