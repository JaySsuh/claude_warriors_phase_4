.text
.globl main
main:
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  li $t0, 10
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  li $t1, 15
  lw $t0, 0($sp)
  addi $sp, $sp, 4
  slt $t0, $t0, $t1
  beq $t0, $zero, L0
  li $t0, 0
    sw $t0, -372($fp)
  j L1
L0:
  li $t0, 5
    sw $t0, -372($fp)
L1:
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra

