.word 0x100 2
.word 0x101 0
.word 0x102 0
.word 0x103 0
.word 0x104 0
.word 0x105 2
.word 0x106 0
.word 0x107 0
.word 0x108 0
.word 0x109 0
.word 0x10A 2
.word 0x10B 0
.word 0x10C 0
.word 0x10D 0
.word 0x10E 0
.word 0x10F 2
.word 0x110 1
.word 0x111 1
.word 0x112 1
.word 0x113 1
.word 0x114 1
.word 0x115 1
.word 0x116 1
.word 0x117 1
.word 0x118 1
.word 0x119 1
.word 0x11A 1
.word 0x11B 1
.word 0x11C 1
.word 0x11D 1
.word 0x11E 1
.word 0x11F 1

add $s0,$zero,$imm,0			# $s0=0 (i=0)
add $s1,$zero,$imm,0			# $s1=0 (j=0)
add $s2,$zero,$imm,0			# $s2=0 (k=0)
add $t3,$zero,$imm,3			# $t3=3 (size=3)

forI:
add $s2,$zero,$zero,0			# k=0
add $s1,$zero,$zero,0			# j=0

forJ:
add $s2,$zero,$zero,0			# k=0
mul $t0,$s0,$imm,4				# $t0=4i
add $t0,$t0,$s1,0				# $t0=4i+j
sw $zero,$t0,$imm,0x120			# C[i][j]=0 (Reset)

forK:
mul $t0,$s0,$imm,4				# $t0=4i
add $t0,$t0,$s2,0				# $t0=4i+k
lw $a0,$imm,$t0,0x100			# A[i][k]
mul $t0,$s2,$imm,4				# $t0=4k
add $t0,$t0,$s1,0				# $t0=4k+j
lw $a1,$imm,$t0,0x110			# B[k][j]
mul $a0,$a0,$a1,0				# $a0 = A[i][k] * B[k][j]
mul $t0,$s0,$imm,4				# $t0=4i
add $t0,$t0,$s1,0				# $t0=4i+j
lw $a1,$imm,$t0,0x120			# C[i][j]
add $a1,$a1,$a0,0				# C[i][j]+=A[i][k] * B[k][j] addition
sw $a1,$imm,$t0,0x120			# C[i][j]+=A[i][k] * B[k][j] store
add $s2,$s2,$imm,1				# k++
ble $imm,$s2,$t3, forK			# if k<=3 jump to forK

add $s1,$s1,$imm,1				# j++
ble $imm,$s1,$t3, forJ			# if j<=3 jump to forJ
add $s0,$s0,$imm,1				# i++
ble $imm,$s0,$t3, forI			# if i<=3 jump to forI

halt $zero,$zero,$zero,0		# halt




