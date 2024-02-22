  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro PUSH reg
    addi $sp, $sp, -4
    sw   $\reg, 0($sp)
.endm

.macro POP reg
    lw   $\reg, 0($sp)
    addi $sp, $sp, 4
.endm

  #
  # Please add your own code below this line
  #
	.globl delay
	.globl time2string
    
  	.data
	.align 2
mytime:	.word 0x5957
timstr:	.ascii "text more text lots of text\0"
	.text
main:
	# print timstr
	la	$a0,timstr
	li	$v0,4
	syscall
	nop
	# wait a little
	li	$a0,1000
	jal	delay
	nop
	# call tick
	la	$a0,mytime
	jal	tick
	nop
	# call your function time2string
	la	$a0,timstr
	la	$t0,mytime
	lw	$a1,0($t0)
	jal	time2string
	nop
	# print a newline
	li	$a0,10
	li	$v0,11
	syscall
	nop
	# go back and do it all again
	j	main
	nop
# tick: update time pointed to by $a0
tick:	lw	$t0,0($a0)	# get time
	addiu	$t0,$t0,1	# increase
	andi	$t1,$t0,0xf	# check lowest digit
	sltiu	$t2,$t1,0xa	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x6	# adjust lowest digit
	andi	$t1,$t0,0xf0	# check next digit
	sltiu	$t2,$t1,0x60	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa0	# adjust digit
	andi	$t1,$t0,0xf00	# check minute digit
	sltiu	$t2,$t1,0xa00	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x600	# adjust digit
	andi	$t1,$t0,0xf000	# check last digit
	sltiu	$t2,$t1,0x6000	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa000	# adjust last digit
tiend:	sw	$t0,0($a0)	# save updated result
	jr	$ra		# return
	nop

  # you can write your code for subroutine "hexasc" below this line
  #
  hexasc:
	andi $t0,$a0,0x0F
	
	blt $t0,10,less_than_10
	li $t1,'A'
	addi $t0,$t0,-10
	add $t0,$t0,$t1
	j  hexasc_end
	nop
	
less_than_10:
	addi $t0,$t0,'0'
	
hexasc_end:
	move $v0,$t0
	andi $v0,$v0,0x7F
	
	jr $ra 
	nop
	
delay:
	li $t1,4475
	blez $a0,end_delay
	
outer_loop:
	addi $a0,$a0,-1
	blez $a0,end_delay
	
	li $t0,0

inner_loop:
	addi $t0,$t0,1
	bge $t0,$t1,outer_loop
	
	j inner_loop
	nop

end_delay:
	jr $ra
	nop
	
time2string:
	PUSH ra   # Save return address
    	PUSH a0   # Save $a0, the address where we need to store the result

   	# Extract and convert the first minute digit
   	andi  $t0, $a1, 0xF000  # Extract first digit
    	srl   $t0, $t0, 12      # Shift it to the rightmost position
   	move  $a0, $t0          # Move to $a0 for hexasc
   	jal   hexasc            # Convert to ASCII
   	nop
   	move  $t1, $v0          # Move result to $t1
   	POP a0                # Restore original $a0
   	sb    $t1, 0($a0)       # Store the digit
   	PUSH a0               # Save $a0 again for next operation

   	# Extract and convert the second minute digit
   	andi  $t0, $a1, 0x0F00  # Extract second digit
   	srl   $t0, $t0, 8       # Shift it to the rightmost position
    	move  $a0, $t0          # Move to $a0 for hexasc
   	jal   hexasc            # Convert to ASCII
   	nop
   	move  $t1, $v0          # Move result to $t1
   	POP a0                # Restore original $a0
   	sb    $t1, 1($a0)       # Store the digit
    	PUSH a0               # Save $a0 again for next operation

   	# Store the colon
   	li    $t1, ':'          # Load colon character
   	sb    $t1, 2($a0)       # Store the colon

   	# Extract and convert the first second digit
   	andi  $t0, $a1, 0x00F0  # Extract third digit
   	srl   $t0, $t0, 4       # Shift it to the rightmost position
   	move  $a0, $t0          # Move to $a0 for hexasc
   	jal   hexasc            # Convert to ASCII
   	nop
   	move  $t1, $v0          # Move result to $t1
   	POP a0                # Restore original $a0
   	sb    $t1, 3($a0)       # Store the digit
   	PUSH a0               # Save $a0 again for next operation

   	# Extract and convert the second second digit
  	andi  $t0, $a1, 0x000F  # Extract fourth digit
  	move  $a0, $t0          # Move to $a0 for hexasc
  	jal   hexasc            # Convert to ASCII
  	nop
  	move  $t1, $v0          # Move result to $t1
   	POP a0                # Restore original $a0
   	sb    $t1, 4($a0)       # Store the digit

   	# Store the null byte to terminate the string
   	sb    $zero, 5($a0)     # Store null byte

   	POP ra    # Restore return address
   	jr    $ra   # Return
   	nop         # Delay slot


