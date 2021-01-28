		.word 0x100 50					# set inside memory radios in pixels
		lw $s0, $zero, $imm, 256		# radius = mem[256=0x100]
		add $s1, $zero, $imm, 175		# x center = 175
		add $s2, $zero, $imm, 143		# y center = 143
		sub $t0, $s1, $s0, 0			# x = x center - radius
FirstLoop:
		add $t3, $s1, $s0, 0 			# x max = x center + radius
		bgt $imm, $t0, $t3, Exit 		# if x greater than x max dont enter FirstLoop
		sub $t1, $s2, $s0, 0			# y = y_center - r
SecondLoop:
		add $t3, $s2, $s0, 0 			# y max = y center + r
		bgt $imm, $t1, $t3, IncrementX 		# if y greater than y max dont enter SecondLoop
		sub $t2, $s1, $t0, 0			# $t2 = x center-x
		sub $t3, $s2, $t1, 0			# $t3 = y center-y
		mul $t2, $t2, $t2, 0			# $t2 = (x center-x) * (x center-x)
		mul $t3, $t3, $t3, 0			# $t3 = (y center-y)*(y center-y)
		mul $a0, $s0, $s0, 0			# $a0 = r^2
		add $t2, $t2, $t3, 0 			# $t2 = (x center-x)*(x center-x) + (y center-y)*(y center-y)
		bgt $imm, $t2, $a0, Incy		# if((x center-x)^2+(y center-y)^2 == r^2)
		out $t0, $zero, $imm, 19		# set IOreg[19] to the current x
		out $t1, $zero, $imm, 20		# set IOreg[20] to the current y
		add $t3, $zero, $imm, 21		# set $t3 to IOreg[21] data place
		out $imm, $zero, $t3, 255		# set IOreg[21] to white color
		add $t3, $zero, $imm, 18		# set $t3 to IOreg[18] monitor 
		out $imm, $zero, $t3, 1			# enable IOreg[18] to write app
Incy:
		add $t1, $t1 ,$imm, 1 			# y++
		beq $imm, $zero, $zero, SecondLoop	# jump to inner SecondLoop
IncrementX:
		add $t0, $t0 ,$imm, 1 			# x++
		beq $imm, $zero, $zero, FirstLoop	# jump to outer FirstLoop
Exit:
		halt $zero, $zero, $zero, 0 	# stop the program after finish the circle