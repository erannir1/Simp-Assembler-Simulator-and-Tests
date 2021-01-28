.word 0x100 4
.word 0x101 2

add $sp, $zero, $imm, 1			# set $sp = 1
sll $sp, $sp, $imm, 11			# set $sp = 1 << 11 = 2048
lw $a0,$imm,$zero,0x100			# load n value
lw $a1,$zero,$imm,0x101			# load k value
jal $imm,$zero,$zero, Binom		# calc $v0 = Binom(n,k)
sw $v0, $zero, $imm,0x102		# store Binom(n,k) in 0x102
halt $zero,$zero,$zero,0		# halt

Binom:
add $sp,$sp,$imm,-4				# adjust stack for 3 items
sw $s0,$sp,$imm,3				# save $s0
sw $ra,$sp,$imm,2				# save return address
sw $a1,$sp,$imm,1				# save k
sw $a0,$sp,$imm,0				# save n
bne $imm,$a1,$zero,L1			# if k!=0 jump to L1
bne $imm,$a0,$a1,L1				# if n!=k jump to L1
add $v0,$zero,$imm,1			# if n==k or k==0 binom=1
beq $imm,$zero,$zero,L2			# jump to L2

L1:
sub $a0,$a0,imm,1				# calculate n-1
jal $imm,$zero,zero,Binom		# calc $v0=Binom(n-1,k)
add $s0,$v0,$zero,0				# $s0 = Binom(n-1,k)
sub $a1,$a1,$imm,1				# calculate k-1
lw $a0, $sp, $imm, 0			# restore $a0 = n
sub $a0,$a0,imm,1				# calculate n-1
jal $imm,$zero,$zero,Binom		# calculate Binom(n-1,k-1)
add $v0,$v0,$s0,0				# $v0=Binom(n-1,k-1)+Binom(n-1,k)
lw $a0,$sp,$imm,0				# restore $a0
lw $a1,$sp,$imm,1				# restore $a1
lw $ra,$sp,$imm,2				# restore $ra
lw $s0,$sp,$imm,3				# restore $s0

L2:
add $sp,$sp,$imm,4				# pop 4 items from stack
beq $ra,$zero,$zero,0			# and return

