#include "tree.h"
#include "strtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int labelCount = 0;

status int newLabel() {
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
    const char *dot = strrchr(inputFile, ',');
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
    if(!out) return

    fprintf(out, ".text\n");
    fprinf(out, ".globl main\n");

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
        getDecl(node, out);
    }
}

static void genDecl(tree *node, FILE *out) {
    if (!node) return;

    if (node->nodeKind == DECL && node->numChildren > 0) {
        tree *child = getChild(node, 0);

        if (child->nodeKind == FUNDECL) {
            getFunDecl(child, out);
        }
    }

static void getFunDecl(tree *node, FILE *out) {
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
    fprintf(out, "main:\n");

    fprintf(out, "  addi $sp, $sp, -8\n");
    fprintf(out, "  sw $ra, 4($sp)\n");
    fprintf(out, "  sw $fp, 0($sp)\n");
    fprintf(out, "  move $fp, $sp\n");

    if(funBody) {
        getFunBody(funbody, out);
    }

    fprintf(out, "  move $sp, $fp\n");
    fprintf(out, "  lw $fp, 0($sp)\n");
    fprintf(out, "  lw $ra, 4($sp)\n");
    fprintf(out, "  addi $sp, $sp, 8"\n);
    fprintf(out, "  jr $ra\n\n");
}

static void genFunBody(tree *node, FILE *out) {
    if (!node) return;

    for (int i = 0; i < node->numChildren; i++) {
        tree *child = getChildNode(node, i);
        if(child->nodeKind == STATEMENTLIST) {
            genStmtList(child, out);
        }
    }
}

static void genStmtList(tree *node, FILE *out) {
    if (!node) return;

    if (node->nodeKind == STATEMENTLIST) {
        for (int i = 0; i < node->numChildren; i++) {
            getStmt(getChild(node, i), out);
        }
    } else {
        getStmt(node, out);
    }
}

static void genStmt(tree *node, FILE *out) {
    if (!node) return;

    if (node->nodeKind == STATMENT && node->numChildren > 0) {
        node = getChild(node, 0);
    }

    switch (node->nodeKind) {
        case ASSIGNSTMT: {
            tree *lhr = getChild(node, 0);
            tree *rhs = getChild(node, 1);

            genExpr(rhs, out, "$t0");

            fprintf(out, "  TODO: store assignemnt result into variable\n");
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
            fprintf(out, "  bew $t0, $zero, L%d\n", lblEnd);
            genStmt(body, out);
            fprintf(out, "  j L%d\n", lblStart);
            fprintf(out, "L%d\n", lblEnd);
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
    
}
}