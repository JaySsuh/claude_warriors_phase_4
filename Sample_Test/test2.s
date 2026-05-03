.text
.globl main
main:
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  li $t0, 2
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  li $t1, 3
  addi $sp, $sp, -4
  sw $t1, 0($sp)
  li $t1, 5
  lw $t1, 0($sp)
  addi $sp, $sp, 4
  mul $t1, $t1, $t1
  addi $sp, $sp, -4
  sw $t1, 0($sp)
  li $t1, 7
  lw $t1, 0($sp)
  addi $sp, $sp, 4
  div $t1, $t1
  mflo $t1
  lw $t0, 0($sp)
  addi $sp, $sp, 4
  add $t0, $t0, $t1
    sw $t0, -372($fp)
    lw $t0, -372($fp)
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  li $t1, 1
  lw $t0, 0($sp)
  addi $sp, $sp, 4
  add $t0, $t0, $t1
    sw $t0, -372($fp)
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra

