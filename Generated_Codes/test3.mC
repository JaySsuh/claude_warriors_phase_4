.text
.globl main
main:
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  li $t0, 15
  li $t2, 5
     sll $t2, $t2, 2
    addi $t3, $fp, -428
     add $t3, $t3, $t2
     sw $t0, 0($t3)
  li $t0, 10
  li $t2, 0
     sll $t2, $t2, 2
    addi $t3, $fp, -340
     add $t3, $t3, $t2
     sw $t0, 0($t3)
  li $t2, 5
  sll $t2, $t2, 2
    addi $t3, $fp, -428
     add $t3, $t3, $t2
     lw $t0, 0($t3)
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  li $t2, 0
  sll $t2, $t2, 2
    addi $t3, $fp, -340
     add $t3, $t3, $t2
     lw $t1, 0($t3)
  lw $t0, 0($sp)
  addi $sp, $sp, 4
  add $t0, $t0, $t1
    sw $t0, -372($fp)
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra

