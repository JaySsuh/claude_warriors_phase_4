#include "tree.h"
#include "strtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int labelCount = 0;

static int newLabel() {
    return labelCount++;
}

static void genProgram(tree *node, FILE *out);
static void genDeclList(tree *node, FILE *out);
static void genDecl(tree *node, FILE *out);
static void genFunDecl(tree *node, FILE *out);
static void genFunBody(tree *node, FILE *out);
static void genStmtList(tree *node, FILE *out);
static void genStmt(tree *node, FILE *out);
static void genExpr(tree *node, FILE *out, const char *targetReg);



void generateCode(tree *ast, const char *inputFile) {
    char outName[256];
    const char *dot = strrchr(inputFile, '.');
    if(dot) {
        size_t len = dot - inputFile;
        if (len >= sizeof(outName)) len = sizeof(outName) - 1;
        strncpy(outName, inputFile, len);
        outName[len] = '\0';
        strcat(outName, ".s");
    } else {
        snprintf(outName, sizeof(outName), "%s.s", inputFile);
    }
    FILE *out = fopen(outName, "w");
    if(!out) return;

    fprintf(out, ".text\n");
    fprintf(out, ".globl main\n");

    genProgram(ast, out);

    fclose(out);
}

static void genProgram(tree *node, FILE *out) {
    if (!node) return;

    if (node->nodeKind == PROGRAM && node->numChildren > 0) {
        genDeclList(getChild(node, 0), out);
    }
}

static void genDeclList(tree *node, FILE *out) {
    if (!node) return;

    if (node->nodeKind == DECLLIST) {
        for (int i = 0; i < node->numChildren; i++) {
            genDecl(getChild(node, i), out);
        }
    } else {
        genDecl(node, out);
    }
}

static void genDecl(tree *node, FILE *out) {
    if (!node) return;

    if (node->nodeKind == DECL && node->numChildren > 0) {
        tree *child = getChild(node, 0);

        if (child->nodeKind == FUNDECL) {
            genFunDecl(child, out);
        } else if (child->nodeKind == VARDECL && child->numChildren >= 2) {
            int slot = getChild(child, 1)->val;
            if(slot >= 0 ) {
                fprintf(out, ".data\n");
                fprintf(out, "_gvar_%d: .word 0\n", slot);
                fprintf(out, ".text\n");
            }
        }
    }
}

static void genFunDecl(tree *node, FILE *out) {
    if (!node) return;

    tree *funTypeName = NULL;
    tree *funBody = NULL;

    for (int i = 0; i < node->numChildren; i++) {
        tree *child = getChild(node, i);
        if (child->nodeKind == FUNCTYPENAME) {
            funTypeName = child;
        } else if (child->nodeKind == FUNBODY) {
            funBody = child;
        }
    }
    const char *funcName ="unknown";
    for (int i = 0; i < node->numChildren; i++) {
        tree *child = getChild(node, i);
        if (child->nodeKind == FUNCTYPENAME) {
            for (int j = 0; j < child->numChildren; j++) {
                tree *c2 =  getChild(child, j);
                if (c2->nodeKind == IDENTIFIER && c2->val >= 0) funcName = get_symbol_id(c2->val);
            }
        }
    }
    fprintf(out, "%s:\n", funcName);
    fprintf(out, "  addi $sp, $sp, -8\n");
    fprintf(out, "  sw $ra, 4($sp)\n");
    fprintf(out, "  sw $fp, 0($sp)\n");
    fprintf(out, "  move $fp, $sp\n");

    if(funBody) {
        genFunBody(funBody, out);
    }

    fprintf(out, "  move $sp, $fp\n");
    fprintf(out, "  lw $fp, 0($sp)\n");
    fprintf(out, "  lw $ra, 4($sp)\n");
    fprintf(out, "  addi $sp, $sp, 8\n");
    fprintf(out, "  jr $ra\n\n");
}

static void genFunBody(tree *node, FILE *out) {
    if (!node) return;

    for (int i = 0; i < node->numChildren; i++) {
        tree *child = getChild(node, i);
        if(child->nodeKind == STATEMENTLIST) {
            genStmtList(child, out);
        }
    }
}

static void genStmtList(tree *node, FILE *out) {
    if (!node) return;

    if (node->nodeKind == STATEMENTLIST) {
        for (int i = 0; i < node->numChildren; i++) {
            genStmt(getChild(node, i), out);
        }
    } else {
        genStmt(node, out);
    }
}

static void genStmt(tree *node, FILE *out) {
    if (!node) return;

    if (node->nodeKind == STATEMENT && node->numChildren > 0) {
        node = getChild(node, 0);
    }

    switch (node->nodeKind) {
        case ASSIGNSTMT: {
            tree *lhs = getChild(node, 0);
            tree *rhs = getChild(node, 1);

            genExpr(rhs, out, "$t0");

            tree *idNode = getChild(lhs, 0);
            int slot = idNode->val;
            if (lhs->numChildren >= 2) {
                genExpr(getChild(lhs, 1), out, "$t2");
                fprintf(out, "     sll $t2, $t2, 2\n");
                if (lhs->scope == SCOPE_GLOBAL) fprintf(out, "     la $t3, _gvar_%d\n", slot);
                else fprintf(out, "    addi $t3, $fp, %d\n", -(slot * 4 + 8));

                fprintf(out, "     add $t3, $t3, $t2\n");
                fprintf(out, "     sw $t0, 0($t3)\n");
            } else {
                if (lhs->scope == SCOPE_GLOBAL) fprintf(out, "     sw $t0, _gvar_%d\n", slot);
                else fprintf(out, "    sw $t0, %d($fp)\n", -(slot * 4 + 8));
            }
            break;
        }

        case CONDSTMT: {
            int lblElse = newLabel();
            int lblEnd = newLabel();

            tree *cond = getChild(node, 0);
            tree *thenStmt = getChild(node, 1);
            tree *elseStmt = (node->numChildren > 2) ? getChild(node, 2) : NULL;

            genExpr(cond, out, "$t0");
            fprintf(out, "  beq $t0, $zero, L%d\n", elseStmt ? lblElse : lblEnd);

            genStmt(thenStmt, out);

            if (elseStmt) {
                fprintf(out, "  j L%d\n", lblEnd);
                fprintf(out, "L%d:\n", lblElse);
                genStmt(elseStmt, out);
            }

            fprintf(out, "L%d:\n", lblEnd);
            break;
        }
        
        case LOOPSTMT: {
            int lblStart = newLabel();
            int lblEnd = newLabel();

            tree *cond = getChild(node, 0);
            tree *body = getChild(node, 1);

            fprintf(out, "L%d:\n", lblStart);
            genExpr(cond, out, "$t0");
            fprintf(out, "  beq $t0, $zero, L%d\n", lblEnd);
            genStmt(body, out);
            fprintf(out, "  j L%d\n", lblStart);
            fprintf(out, "L%d:\n", lblEnd);
            break;
        }

        case COMPOUNDSTMT:
            for (int i = 0; i < node->numChildren; i++) {
                tree *child = getChild(node, i);
                if ( child->nodeKind == STATEMENTLIST) {
                    genStmtList(child, out);
                }
            }
            break;

        case RETURNSTMT:
            if (node->numChildren > 0) {
                genExpr(getChild(node, 0), out, "$v0");
            }
            fprintf(out, "  move $sp, $fp\n");
            fprintf(out, "  lw $fp, 0($sp)\n");
            fprintf(out, "  lw $ra, 4($sp)\n");
            fprintf(out, "  addi $sp, $sp, 8\n");
            fprintf(out, "  jr $ra\n");
            break;

        default:
            fprintf(out, "  # Unhandled statement node kind: %d\n", node->nodeKind);
            break;
    }
}

static void genExpr(tree *node, FILE *out, const char *targetReg) {
    if (!node) return;

    if ((node->nodeKind == EXPRESSION ||
        node->nodeKind == ADDEXPR ||
        node->nodeKind == TERM ||
        node->nodeKind == FACTOR) &&
        node->numChildren == 1) {
            genExpr(getChild(node, 0), out, targetReg);
            return;
        }

    switch (node->nodeKind) {
        case INTEGER:
            fprintf(out, "  li %s, %d\n", targetReg, node->val);
            break;
        
        case CHARNODE:
            fprintf(out, "  li %s, %d\n", targetReg, node->val);
            break;
        
        case VAR:
            tree *idNode = getChild(node, 0);
            int slot = idNode->val;
            if (node->numChildren >= 2) {
                genExpr(getChild(node, 1), out, "$t2");
                fprintf(out, "  sll $t2, $t2, 2\n");
                if (node->scope == SCOPE_GLOBAL) fprintf(out, "    la $t3, _gvar_%d\n", slot);
                else fprintf(out, "    addi $t3, $fp, %d\n", -(slot * 4 + 8));
        
                fprintf(out, "     add $t3, $t3, $t2\n");
                fprintf(out, "     lw %s, 0($t3)\n", targetReg);
            } else {
                if (node->scope == SCOPE_GLOBAL) fprintf(out, "    lw %s, _gvar_%d\n", targetReg, slot);
                else fprintf(out, "    lw %s, %d($fp)\n", targetReg, -(slot * 4 + 8));
            }
            break;

        case ADDEXPR:
        case TERM:
            if(node->numChildren == 3) {
                tree *left = getChild(node, 0);
                tree *op = getChild(node, 1);
                tree *right = getChild(node, 2);

                genExpr(left, out, targetReg);
                genExpr(right, out, "$t1");

                switch (op->val) {
                    case 0: // OPADD or OP_ADD
                        fprintf(out, "  add %s, %s, $t1\n", targetReg, targetReg);
                        break;
                    
                    case 1: // OPSUB or OP_SUB
                        fprintf(out, "  sub %s, %s, $t1\n", targetReg, targetReg);
                        break;
                    
                    case 2: // OPMUL or OP_MUL
                        fprintf(out, "  mul %s, %s, $t1\n", targetReg, targetReg);
                        break;
                    
                    case 3: // OPDIV or OP_DIV
                        fprintf(out, "  div %s, $t1\n", targetReg);
                        fprintf(out, "  mflo %s\n", targetReg);
                        break;
                    
                    default:
                        fprintf(out, "  # unsuported arithmetic operation %d\n", op->val);
                        break;
                }
            }
            break;

        case EXPRESSION:
            if (node->numChildren == 3) {
                tree *left = getChild(node, 0);
                tree *op = getChild(node, 1);
                tree *right = getChild(node, 2);

                genExpr(left, out, targetReg);
                genExpr(right, out, "$t1");

                switch (op->val) {
                    case 4: // OPLT or OP_LT
                        fprintf(out, "  slt %s, %s, $t1\n", targetReg, targetReg);
                        break;
                    
                    case 5: // OPLTE or OP_LTE
                        fprintf(out, "  sle %s, %s, $t1\n", targetReg, targetReg);
                        break;

                    case 6: // OPEQ or OP_EQ
                        fprintf(out, "  seq %s, %s, $t1\n", targetReg, targetReg);
                        break;

                    case 7: // OPGTE or OP_GTE
                        fprintf(out, "  sge %s, %s, $t1\n", targetReg, targetReg);
                        break;
                    
                    case 8: // OPGT or OP_GT
                        fprintf(out, "  sgt %s, %s, $t1\n", targetReg, targetReg);
                        break;

                    case 9: // OPNEQ or OP_NEQ
                        fprintf(out, "  sne %s, %s, $t1\n", targetReg, targetReg);
                        break;

                    default:
                        fprintf(out, "  # unsupported relational operation %d\n", op->val);
                        break;
                }
            }
            break;

            default:
                fprintf(out, "  # Unhandled expression node kind: %d\n", node->nodeKind);
                break;
    }
}