Installation:
			add $sp, $zero, $imm, 4095    		# 0: set $sp = 4095
			add $t0, $zero, $zero, 0			# i defines sector num i = $t0 = 0
			add $sp, $sp, $imm, -3 				# make space on stack
			sw  $s0, $sp, $imm, 2				# store $s0
			sw  $s1, $sp, $imm, 1				# store $s1
			sw  $s2, $sp, $imm, 0				# store $s2
FirstLoop:
			add $t1, $zero, $imm, 3 			# $t1 = 3 
			bgt $imm, $t0, $t1, Exit1Loop		# if i > 3 exit the loop
			add $t1, $zero, $imm, 17			# diskstatus register (17)
			in $t2, $zero, $t1, 0				# $t2 = IO register[17]
			bne $imm, $t2, $zero, FirstLoop			# check if the disk is free
			add $t1, $zero, $imm, 15			# $t1 = 15 for sector place
			out $t0, $zero, $t1, 0				# updating the sector for reading
			add $t1, $zero, $imm, 16			# $t1 = 16 for buffer address
			bne $imm, $t0, $zero, IF1			# if(i == 0)
			out $zero, $zero, $t1, 0			# buffer address of sector 0 is 0 in the memory
			beq $imm, $zero, $zero, Command		# skip the other conditions
IF1:	
			add $t2, $zero, $imm, 1				# $t2 = 1, condition for if 1
			bne $imm, $t0, $t2, IF2				# if(i == 1)
			out $imm, $zero, $t1, 128			# buffer address for sector 1 is 128 
			beq $imm, $zero, $zero, Command		# skip the other conditions
IF2:	
			add $t2, $zero, $imm, 2				# $t2 = 2, condition for if 2
			bne $imm, $t0, $t2, IF3				# if(i == 2)
			out $imm, $zero, $t1, 256			# buffer address for sector 2 is 256 in the memory
			beq $imm, $zero, $zero, Command		# skip the other conditions
IF3:	
			add $t2, $zero, $imm, 3				# $t2 = 3, condition for if 3
			bne $imm, $t0, $t2, Command			# if(i == 3)
			out $imm, $zero, $t1, 384			# buffer address for sector 3 is 384 in the memory
Command:
			add $t1, $zero, $imm, 14			# $t1 = 14 for command status
			out $imm, $zero, $t1, 1				# IO registers[14] = 1 read from disk to memory
			add $t0, $t0, $imm, 1				# i ++
			beq $imm, $zero, $zero, FirstLoop		# start loop again
Exit1Loop:
			add $t1, $zero, $imm, 17			# 17 is the diskstatus register in IO registers
			in $t0, $zero, $t1, 0				# $t0 = IO_regs[17](diskstatus)	
			bne $imm, $zero, $t0, Exit1Loop		# wait to sector 3 finish to read
			add $s0, $zero, $zero, 0			# i is place in sector i = $s0 = 0
			add $s1, $zero, $zero, 0			# temp is const which help us calculate needed xors. temp = 0
			add $s2, $zero, $imm, 512			# buffer place for wb the xor data
XorLoop:
			add $t0, $zero, $imm, 127			# $t0 = 127
			bgt $imm, $s0, $t0, EndXorLoop		# if i > 127 exit the loop
			lw $t0, $zero, $s0, 0				# $t0 = memory[i] , sector 0
			add $t1, $s0, $imm, 128				# $t1 = i + 128
			lw $t1, $t1, $zero, 0				# $t1 = memory[i+128] , sector 1
			xor $s1, $t0, $t1, 0				# temp = memory[i] xor memory[i+128] (xor between sectors 0,1)
			add $t1, $s0, $imm, 256				# $t1 = i + 256
			lw $t1, $t1, $zero, 0				# $t1 = memory[i+256] , sector 2
			xor $s1, $s1, $t1, 0				# temp = memory[i] xor memory[i+128] xor memory[i+256] (xor between sectors 0,1,2)
			add $t1, $s0, $imm, 384				# $t1 = i + 384
			lw $t1, $t1, $zero, 0				# $t1 = memory[i+384] , sector 3
			xor $s1, $s1, $t1, 0				# temp = memory[i] xor memory[i+128] xor memory[i+256] xor memory[i+384](xor between sectors 0,1,2,3)
			sw $s1, $s0, $s2, 0					# memory[i+ 512] = temp
			add $s0, $s0, $imm, 1				# i ++
			beq $imm, $zero, $zero, XorLoop		# go back
EndXorLoop:
			add $s0, $zero, $zero, 0			# variable named a = 0 for loop
UpdateLoop:
			bne $imm, $s0, $zero, Finish		# if a != 0 we end the loop
			add $t0, $zero, $imm, 17			# 17 is the diskstatus register in IO registers
			in $t1, $zero, $t0, 0				# $t1 = IO registers[17] (diskstatus)	
			bne $imm, $t1, $zero, UpdateLoop			# if IO registers[17] == 0 load to disk
			add $t0, $zero, $imm, 15			# $t0 = 15 , sector place
			out $imm, $zero, $t0, 4				# IO registers[15] = 4
			add $t0, $zero, $imm, 16			# $t0 = 16 , buffer
			out $s2, $zero, $t0, 0				# IO registers[16] = 512
			add $t0, $zero, $imm, 14			# $t0 = 14 , command
			out $imm, $zero, $t0, 2				# IO registers[14] = 2 , writing data to disk
			add $s0, $s0, $imm, 1				# make a=1 to exit loop
Finish:	
			add $t0, $zero, $imm, 17			# $t0 = 17
			in $t1, $zero, $t0, 0				# $t1 = IO registers[17] (diskstatus)	
			bne $imm, $t1, $zero, Finish		# if IO registers[17] == 0 loading to disk ended halt
			lw $s2, $sp, $imm, 0				# restore $s2
			lw $s1, $sp, $imm, 1				# restore $s1
			lw $s0, $sp, $imm, 2				# restore $s0
			add $sp, $sp, $imm, 3				# free stack
			halt $zero, $zero, $zero, 0 		# exit