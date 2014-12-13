	# ACall _tmp7
	  addiu $fp, $sp, 8	# set up new fp
	  addiu $fp, $sp, 8	# set up new fp
	  addiu $fp, $sp, 8	# set up new fp
	  addiu $fp, $sp, 8	# set up new fp
	  addiu $fp, $sp, 8	# set up new fp
	  add $sp, $sp, 4	# pop params off stack
	  add $sp, $sp, 4	# pop params off stack
	  add $sp, $sp, 8	# pop params off stack
	  .align 2
	  .align 2
	  .align 2
	# BeginFunc 0
	# BeginFunc 0
	# BeginFunc 0
	# BeginFunc 0
	# BeginFunc 36
	# (below handles reaching end of fn body with no explicit return)
	# (below handles reaching end of fn body with no explicit return)
	# (below handles reaching end of fn body with no explicit return)
	# (below handles reaching end of fn body with no explicit return)
	# (below handles reaching end of fn body with no explicit return)
	  .data
	  .data
	# EndFunc
	# EndFunc
	# EndFunc
	# EndFunc
	# EndFunc
	  .globl main
	  jalr $t0            	# jump to function
	  jalr $t0            	# jump to function
	  jalr $t0            	# jump to function
	  jr $ra		# return from function
	  jr $ra		# return from function
	  jr $ra		# return from function
	  jr $ra		# return from function
	  jr $ra		# return from function
	  lw $fp, 0($fp)	# restore saved fp
	  lw $fp, 0($fp)	# restore saved fp
	  lw $fp, 0($fp)	# restore saved fp
	  lw $fp, 0($fp)	# restore saved fp
	  lw $fp, 0($fp)	# restore saved fp
	  lw $ra, -4($fp)	# restore saved ra
	  lw $ra, -4($fp)	# restore saved ra
	  lw $ra, -4($fp)	# restore saved ra
	  lw $ra, -4($fp)	# restore saved ra
	  lw $ra, -4($fp)	# restore saved ra
	  lw $t0, -12($fp)	# fill _tmp0 to $t0 from $fp-12
	  lw $t0, -16($fp)	# fill _tmp1 to $t0 from $fp-16
	  lw $t0, -20($fp)	# fill _tmp2 to $t0 from $fp-20
	  lw $t0, -24($fp)	# fill _tmp3 to $t0 from $fp-24
	  lw $t0, -28($fp)	# fill _tmp4 to $t0 from $fp-28
	  lw $t0, -32($fp)	# fill _tmp5 to $t0 from $fp-32
	  lw $t0, -32($fp)	# fill _tmp5 to $t0 from $fp-32
	  lw $t0, -36($fp)	# fill _tmp6 to $t0 from $fp-36
	  lw $t0, -40($fp)	# fill _tmp7 to $t0 from $fp-40
	  lw $t0, -8($fp)	# fill temp to $t0 from $fp-8
	  lw $t0, -8($fp)	# fill temp to $t0 from $fp-8
	  lw $t0, -8($fp)	# fill temp to $t0 from $fp-8
	  lw $t0, -8($fp)	# fill temp to $t0 from $fp-8
	  lw $t2, 0($t0) 	# load with offset
	  lw $t2, 0($t0) 	# load with offset
	  lw $t2, 0($t0) 	# load with offset
	  lw $t2, 0($t0) 	# load with offset
	  lw $t2, 4($t0) 	# load with offset
	  lw $t2, 8($t0) 	# load with offset
  main:
	  move $sp, $fp		# pop callee frame off stack
	  move $sp, $fp		# pop callee frame off stack
	  move $sp, $fp		# pop callee frame off stack
	  move $sp, $fp		# pop callee frame off stack
	  move $sp, $fp		# pop callee frame off stack
	  move $t2, $v0		# copy function return value from $v0
	  move $t2, $v0		# copy function return value from $v0
	# PopParams 4
	# PopParams 4
	# PopParams 8
	# PushParam temp
	# PushParam temp
	# PushParam _tmp2
	# PushParam _tmp5
  _Queue.DeQueue:
  _QueueItem.GetNext:
  _QueueItem.GetPrev:
	  QueueItem:		# label for class QueueItem vtable
  _QueueItem.SetPrev:
	  Queue:		# label for class Queue vtable
	# standard Decaf preamble 
	  subu $sp, $sp, 36	# decrement sp to make space for locals/temps
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  subu $sp, $sp, 8	# decrement sp to make space to save ra, fp
	  subu $sp, $sp, 8	# decrement sp to make space to save ra, fp
	  subu $sp, $sp, 8	# decrement sp to make space to save ra, fp
	  subu $sp, $sp, 8	# decrement sp to make space to save ra, fp
	  subu $sp, $sp, 8	# decrement sp to make space to save ra, fp
	  sw $fp, 8($sp)	# save fp
	  sw $fp, 8($sp)	# save fp
	  sw $fp, 8($sp)	# save fp
	  sw $fp, 8($sp)	# save fp
	  sw $fp, 8($sp)	# save fp
	  sw $ra, 4($sp)	# save ra
	  sw $ra, 4($sp)	# save ra
	  sw $ra, 4($sp)	# save ra
	  sw $ra, 4($sp)	# save ra
	  sw $ra, 4($sp)	# save ra
	  sw $t0, 4($sp)	# copy param value to stack
	  sw $t0, 4($sp)	# copy param value to stack
	  sw $t0, 4($sp)	# copy param value to stack
	  sw $t0, 4($sp)	# copy param value to stack
	  sw $t2, -12($fp)	# spill _tmp0 from $t2 to $fp-12
	  sw $t2, -16($fp)	# spill _tmp1 from $t2 to $fp-16
	  sw $t2, -20($fp)	# spill _tmp2 from $t2 to $fp-20
	  sw $t2, -24($fp)	# spill _tmp3 from $t2 to $fp-24
	  sw $t2, -28($fp)	# spill _tmp4 from $t2 to $fp-28
	  sw $t2, -32($fp)	# spill _tmp5 from $t2 to $fp-32
	  sw $t2, -36($fp)	# spill _tmp6 from $t2 to $fp-36
	  sw $t2, -40($fp)	# spill _tmp7 from $t2 to $fp-40
	  .text
	  .text
	  .text
	# _tmp0 = *(temp)
	# _tmp1 = *(_tmp0 + 4)
	# _tmp2 = ACall _tmp1
	# _tmp3 = *(temp)
	# _tmp4 = *(_tmp3)
	# _tmp5 = ACall _tmp4
	# _tmp6 = *(_tmp5)
	# _tmp7 = *(_tmp6 + 8)
	# VTable for class Queue
	# VTable for class QueueItem
	  .word _Queue.DeQueue
	  .word _QueueItem.GetNext
	  .word _QueueItem.GetPrev
	  .word _QueueItem.SetPrev
