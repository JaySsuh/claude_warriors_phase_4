%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<../src/tree.h>
#include<../src/strtab.h>

#define NODE_TYPES_DEFINED

extern int yylineno;
extern int yylex(void);

int yywarning(char *msg);
void yyerror(const char *msg);

symEntry *ST_lookup_current(char *id);
char *scope = "";
int func_index = -1;
int func_params = 0;
int suppress_undeclared = 0;

int count_args(tree *node) {
    if (node == NULL || node->nodeKind != ARGLIST || node->numChildren == 0) {
        return 0;
    }
    int count = 0;
    for (int i = 0; i < node->numChildren; i++) {
        tree *child = getChild(node, i);
        if (child->nodeKind == ARGLIST) {
            count += count_args(child);
        } else {
            count++;
        }
    }
    return count;
}

void gather_args(tree *node, tree **arr, int *n) {
    if (node == NULL || node->nodeKind != ARGLIST) {
        return;
    }

    for (int i = 0; i < node->numChildren; i++) {
        tree *child = getChild(node, i);
        if (child->nodeKind == ARGLIST) {
            gather_args(child, arr, n);
        } else {
            arr[(*n)++] = child;
        }
    }
}

int find_slot(const char *id) {
    table_node *sc = current_scope;
    while (sc) {
        for (int i = 0; i < MAXIDS; i++) {
            if (sc->strTable[i] != NULL &&
                strcmp(sc->strTable[i]->id, id) == 0) {
                    return i;
                }
        }
        sc = sc->parent;
    }
    return -1;
}

tree *unwrap_expr(tree *node) {
    while (node != NULL && node->numChildren == 1 &&
            (node->nodeKind == EXPRESSION ||
             node->nodeKind == ADDEXPR ||
             node->nodeKind == TERM ||
             node->nodeKind == FACTOR)) {
                node = getChild(node, 0);
    }
    return node;
}

int expr_type(tree *node) {
    if (node == NULL) return -1;

    /* unwrap single-child wrapper nodes */
    if (node->numChildren == 1 &&
        (node->nodeKind == EXPRESSION ||
         node->nodeKind == ADDEXPR ||
         node->nodeKind == TERM ||
         node->nodeKind == FACTOR)) {
        return expr_type(getChild(node, 0));
    }

    /* binary expression: both sides must agree */
    if (node->numChildren == 3 && (node->nodeKind == ADDEXPR || node->nodeKind == TERM)) {
      int lt = expr_type(getChild(node, 0));
      int rt = expr_type(getChild(node, 2));
      if (lt == -1 && rt == -1) return -1;
      if (lt == -1) return rt;
      if (rt == -1) return lt;
      if (lt == rt) return lt;
        return -2;
    }

    if (node->nodeKind == INTEGER) return INT_TYPE;
    if (node->nodeKind == CHAR_NODE) return CHAR_TYPE;

    if ((node->nodeKind == VAR || node->nodeKind == FUNCCALLEXPR)
        && node->numChildren > 0) {
        int slot = getChild(node, 0)->val;
        table_node *sc = current_scope;
        while (sc) {
            if (slot >= 0 && sc->strTable[slot] != NULL) {
                return sc->strTable[slot]->data_type;
            }
            sc = sc->parent;
        }
    }

    return -1;
}
    

%}
/* the union describes the fields available in the yylval variable */
%union
{
    int value;
    struct treenode *node;
    char *strval;
}

/* token declarations */
%token <strval> ID
%token <value> INTCONST
%token <value> CHARCONST
%token KWD_INT KWD_CHAR KWD_VOID KWD_IF KWD_ELSE KWD_WHILE KWD_RETURN
%token LPAREN RPAREN LCRLY_BRKT RCRLY_BRKT LSQ_BRKT RSQ_BRKT
%token SEMICLN COMMA
%token OPER_ASGN
%token OPER_ADD OPER_SUB OPER_MUL OPER_DIV
%token OPER_LT OPER_LTE OPER_GT OPER_GTE OPER_EQ OPER_NEQ
%token ERROR ILLEGAL_TOK

/*non-terminal symbols*/
%type <node> program declList decl
%type <node> varDecl typeSpec funDecl
%type <node> formalDeclList formalDecl
%type <node> funBody localDeclList
%type <node> stmtList stmt exprStmt
%type <node> compoundStmt assignStmt condStmt loopStmt returnStmt
%type <node> expr relop addExpr addop term mulop factor
%type <node> funcCallExpr argList var
 /*precendence for dangling else and operators*/
%nonassoc LOWER_THAN_ELSE
%nonassoc KWD_ELSE

%left OPER_LT OPER_LTE OPER_GT OPER_GTE OPER_EQ OPER_NEQ
%left OPER_ADD OPER_SUB 
%left OPER_MUL OPER_DIV

%start program

%%

program         : {new_scope();} declList
                  {
                    tree *n = maketree(PROGRAM);
                    addChild(n, $2);
                    ast = n;
                  }
                ;

declList        : decl
                  {
                    tree *n = maketree(DECLLIST);
                    addChild(n, $1);
                    $$ = n;
                  }
                | declList decl
                  {
                    tree *n = maketree(DECLLIST);
                    addChild(n, $1);
                    addChild(n, $2);
                    $$ = n;
                  }
                ;

/* declarations */
decl            : varDecl
                  {
                    tree *n = maketree(DECL);
                    addChild(n, $1);
                    $$ = n;
                  }
                | funDecl
                  {
                    tree *n = maketree(DECL);
                    addChild(n, $1);
                    $$ = n;
                  }
                ;

varDecl         : typeSpec ID SEMICLN
                  {
                    tree *n = maketree(VARDECL);
                    tree *id = maketree(IDENTIFIER);
                    id->val = -1;

                    if (ST_lookup_current($2) != NULL) {
                        yywarning("Symbol declared multiple times.");
                    } else {
                        id->val = ST_insert($2, $1->val, SCALAR, (int *)scope);
                    }

                    addChild(n, $1);
                    addChild(n, id);
                    $$ = n;
                  }
                | typeSpec ID LSQ_BRKT INTCONST RSQ_BRKT SEMICLN
                  {
                    tree *n = maketree(VARDECL);
                    tree *id = maketree(IDENTIFIER);
                    tree *sz = maketreeWithVal(INTEGER, $4);
                    id->val = -1;

                    if ($4 == 0) {
                      yywarning("Array variable declared with size of zero.");
                    }

                    if (ST_lookup_current($2) != NULL) {
                        yywarning("Symbol declared multiple times.");
                    } else {
                        id->val = ST_insert($2, $1->val, ARRAY, (int *)scope);
                        if (id->val >= 0) {
                            symEntry *e = ST_lookup($2);
                            if (e) {
                                e->size = $4;
                            }
                        }
                    }

                    addChild(n, $1);  // type
                    addChild(n, id);  // name
                    addChild(n, sz);  // size
                    $$ = n;
                  }
                ;

typeSpec        : KWD_INT
                  {
                    tree *t = maketree(TYPESPEC);
                    t->val = INT_TYPE;
                    $$ = t;
                  }
                | KWD_CHAR
                  {
                    tree *t = maketree(TYPESPEC);
                    t->val = CHAR_TYPE;
                    $$ = t;
                  }
                | KWD_VOID
                  {
                    tree *t = maketree(TYPESPEC);
                    t->val = VOID_TYPE;
                    $$ = t;
                  }
                ;

// functions, parameters, body
funDecl         : typeSpec ID
                  {
                    if (ST_lookup_current($2) != NULL) {
                        yywarning("Symbol declared multiple times.");
                        func_index = -1;
                    } else {
                        func_index = ST_insert($2, $1->val, FUNCTION, (int *)"");
                    }
                    func_params = 0;
                    scope = strdup($2);
                    new_scope();
                  }
                  LPAREN formalDeclList RPAREN funBody
                  {
                    if (func_index >= 0) {
                        connect_params(func_index, func_params);
                    }
                    /* build AST */
                    tree *n = maketree(FUNDECL);
                    tree *ft = maketree(FUNCTYPENAME);
                    addChild(ft, $1);
                    tree *id = maketree(IDENTIFIER);
                    id->val = func_index;
                    addChild(ft, id);
                    addChild(n, ft);
                    if ($5->numChildren > 0) {
                        addChild(n, $5);
                    }
                    addChild(n, $7);
                    $$ = n;

                    up_scope();
                    /* after finishing this function, reset scope to global */
                    scope = "";
                  }
                ;

formalDeclList  : // empty
                  {
                    $$ = maketree(FORMALDECLLIST);
                  }
                | formalDecl
                  {
                    tree *l = maketree(FORMALDECLLIST);
                    addChild(l, $1);
                    $$ = l;
                  }
                | formalDecl COMMA formalDeclList
                  {
                    tree *l = maketree(FORMALDECLLIST);
                    addChild(l, $1);
                    if ($3->numChildren > 0) {
                        addChild(l, $3);
                    }
                    $$ = l;
                  }
                ;

formalDecl      : typeSpec ID
                  {
                    tree *n = maketree(FORMALDECL);
                    tree *id = maketree(IDENTIFIER);
                    id->val = -1;

                    if (ST_lookup_current($2) != NULL) {
                        yywarning("Symbol declared multiple times.");
                    } else {
                        id->val = ST_insert($2, $1->val, SCALAR, (int *)scope);
                        add_param($1->val, SCALAR);
                        func_params++;
                    }

                    addChild(n, $1);
                    addChild(n, id);
                    $$ = n;
                  }
                | typeSpec ID LSQ_BRKT RSQ_BRKT
                  {
                    tree *n = maketree(FORMALDECL);
                    tree *id = maketree(IDENTIFIER);
                    tree *arr = maketree(ARRAYDECL);
                    id->val = -1;

                    if (ST_lookup_current($2) != NULL) {
                        yywarning("Symbol declared multiple times.");
                    } else {
                        id->val = ST_insert($2, $1->val, ARRAY, (int *)scope);
                        add_param($1->val, ARRAY);
                        func_params++;
                    }

                    addChild(n, $1);
                    addChild(n, id);
                    addChild(n, arr);
                    $$ = n;
                  }
                ;

funBody         : LCRLY_BRKT localDeclList stmtList RCRLY_BRKT
                  {
                    tree *n = maketree(FUNBODY);
                    if($2->numChildren > 0) {
                        addChild(n, $2);
                    }
                    if($3->numChildren > 0) {
                        addChild(n, $3);
                    }
                    $$ = n;
                  }
                ;

localDeclList   : //empty
                  {
                    $$ = maketree(LOCALDECLLIST);
                  }
                | varDecl localDeclList
                  {
                    tree *l = maketree(LOCALDECLLIST);
                    addChild(l, $1);
                    if ($2->numChildren > 0) {
                        addChild(l, $2);
                    }
                    $$ = l;
                  }
                ;

// Statements
stmtList        : // empty
                  {
                    $$ = maketree(STATEMENTLIST);
                  }
                | stmt stmtList
                  {
                    tree *sl = maketree(STATEMENTLIST);
                    tree *s = maketree(STATEMENT);
                    addChild(s, $1);
                    addChild(sl, s);
                    if ($2->numChildren > 0) {
                        addChild(sl, $2);
                    }
                    $$ = sl;
                  }
                ;

stmt                : compoundStmt
                      {
                        $$ = $1;
                      }
                    | assignStmt
                      {
                        $$ = $1;
                      }
                    | condStmt
                      {
                        $$ = $1;
                      }
                    | loopStmt
                      {
                        $$ = $1;
                      }
                    | returnStmt
                      {
                        $$ = $1;
                      }
                    | exprStmt
                      {
                        $$ = $1;
                      }
                    ;

compoundStmt        : LCRLY_BRKT localDeclList stmtList RCRLY_BRKT
                      {
                        tree *n = maketree(COMPOUNDSTMT);
                        if ($2->numChildren > 0) {
                            addChild(n, $2);
                        }
                        addChild(n, $3);
                        $$ = n;
                      }
                    ;

assignStmt          : var OPER_ASGN expr SEMICLN
                      {
                        tree *n = maketree(ASSIGNSTMT);
                        addChild(n, $1); // lhs
                        addChild(n, $3); // rhs
                        
                        /* type mismatch check for simple RHS expressions */
                        int lhs_type = -1;
                        if ($1->numChildren > 0) {
                            int slot = getChild($1, 0)->val;
                            table_node *sc = current_scope;
                            while (sc) {
                                if (slot >= 0 && sc->strTable[slot] != NULL) {
                                    lhs_type = sc->strTable[slot]->data_type;
                                    break;
                                }
                                sc = sc->parent;
                            }
                        }

                        int rhs_type = expr_type($3);
                        if (lhs_type != -1 && rhs_type != -1 && lhs_type != rhs_type) {
                            yywarning("Type mismatch in assignment.");
                        }
                        $$ = n;
                      }
                    ;

condStmt            : KWD_IF LPAREN expr RPAREN stmt %prec LOWER_THAN_ELSE
                      {
                        tree *n = maketree(CONDSTMT);
                        tree *s = maketree(STATEMENT);
                        addChild(s, $5);
                        addChild(n, $3); // condition
                        addChild(n, s); // then branch
                        $$ = n;
                      }
                    | KWD_IF LPAREN expr RPAREN stmt KWD_ELSE stmt
                      {
                        tree *n = maketree(CONDSTMT);
                        tree *s1 = maketree(STATEMENT);
                        tree *s2 = maketree(STATEMENT);
                        addChild(s1, $5);
                        addChild(s2, $7);
                        addChild(n, $3); // condition
                        addChild(n, s1); // then branch
                        addChild(n, s2); // else branch
                        $$ = n;
                      }
                    ;

loopStmt            : KWD_WHILE LPAREN expr RPAREN stmt
                      {
                          tree *n = maketree(LOOPSTMT);
                          tree *s = maketree(STATEMENT);
                          addChild(s, $5);
                          addChild(n, $3); // condition
                          addChild(n, s); // body
                          $$ = n;
                      }
                    ;

returnStmt          : KWD_RETURN SEMICLN
                      {
                          $$ = maketree(RETURNSTMT);
                      }
                    | KWD_RETURN expr SEMICLN
                      {
                        tree *n = maketree(RETURNSTMT);
                        addChild(n, $2);
                        $$ = n;
                      }
                    ;

exprStmt            : expr SEMICLN
                      {
                        tree *n = maketree(ASSIGNSTMT);
                        addChild(n, $1);
                        $$ = n;
                      }
                    ;

// variables, function calls, arguments
var                 : ID
                      {
                        tree *v = maketree(VAR);
                        tree *id = maketree(IDENTIFIER);
                        id->val = -1;

                        symEntry *e = ST_lookup($1);
                        if (e == NULL) {
                          if (!suppress_undeclared) {
                            yywarning("Undeclared variable");
                          }
                        } else {
                            id->val = find_slot($1);
                        }

                        addChild(v, id);
                        $$ = v;
                      }
                    | ID LSQ_BRKT addExpr RSQ_BRKT
                      {
                        int saved_line = yylineno;
                        tree *v = maketree(VAR);
                        tree *id = maketree(IDENTIFIER);
                        id->val = -1;

                        symEntry *e = ST_lookup($1);
                        if (e == NULL) {
                            printf("error: line %d: %s\n", saved_line, "Undeclared identifier");
                        } else {
                            id->val = find_slot($1);

                            if (e->symbol_type != ARRAY) {
                                printf("error: line %d: %s\n", saved_line, "Non-array identifier used as an array.");
                            } else {
                                /* unwrap index expression to innermost node */
                                tree *ndx = $3;
                                while (ndx->numChildren == 1 &&
                                        (ndx->nodeKind == ADDEXPR ||
                                         ndx->nodeKind == TERM ||
                                         ndx->nodeKind == FACTOR)) {
                                    ndx = getChild(ndx, 0);
                                }

                                int ndx_is_int = 0;
                                int ndx_val = -1;

                                if (ndx->nodeKind == INTEGER) {
                                    ndx_is_int = 1;
                                    ndx_val = ndx->val;
                                } else if (ndx->nodeKind == CHAR_NODE) {
                                    printf("error: line %d: %s\n", saved_line, "Array indexed using non-integer expression.");
                                } else if (ndx->nodeKind == VAR && ndx->numChildren > 0) {
                                    int slot = getChild(ndx, 0)->val;
                                    table_node *sc = current_scope;
                                    
                                    while (sc) {
                                        if (slot >= 0 && sc->strTable[slot] != NULL) {
                                            if (sc->strTable[slot]->data_type == INT_TYPE) {
                                                ndx_is_int = 1;
                                            } else {
                                                printf("error: line %d: %s\n", saved_line, "Array indexed using non-integer expression.");
                                            }
                                            break;
                                        }
                                        sc = sc->parent;
                                    }
                                }

                                if (ndx_is_int) {
                                    if (ndx_val < 0) {
                                        printf("error: line %d: %s\n", saved_line - 1, "Statically sized array indexed with constant, out-of-bounds expression.");
                                    } else if (e->size > 0 && ndx_val >= e->size) {
                                        printf("error: line %d: %s\n", saved_line - 1, "Statically sized array indexed with constant, out-of-bounds expression.");
                                    }
                                }
                            }
                        }

                        addChild(v, id);
                        addChild(v, $3);
                        $$ = v;
                      }
                    ;

funcCallExpr        : ID LPAREN {suppress_undeclared = 1; } argList { suppress_undeclared = 0; } RPAREN
                      {
                        tree *f  = maketree(FUNCCALLEXPR);
                        tree *id = maketree(IDENTIFIER);
                        id->val  = -1;

                        symEntry *e = ST_lookup($1);
                        if (e == NULL) {
                            yywarning("Undefined function");
                        } else {
                            id->val = find_slot($1);

                            if (e->symbol_type != FUNCTION) {
                                yywarning("called identifier is not a function");
                            } else {
                                int call_argc = count_args($4);
                                if (call_argc != e->size) {
                                  if(call_argc < e->size) {
                                    yywarning("Too few arguments provided in function call.");
                                } else {
                                  yywarning("Too many arguments provided in function call.");
                                }
                                } else if (e->params != NULL) {
                                    tree *args[MAXCHILDREN];
                                    int nargs = 0;
                                    gather_args($4, args, &nargs);

                                    param *p = e->params;
                                    for (int i = 0; i < nargs && p != NULL; i++, p = p->next) {
                                        int atype = expr_type(args[i]);
                                        if (atype != -1 && atype != p->data_type) {
                                            yywarning("Argument type mismatch in function call.");
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        addChild(f, id);
                        if ($4->numChildren > 0) {
                            addChild(f, $4);
                        }
                        $$ = f;
                      }
                    ;

argList             : // empty
                      {
                        $$ = maketree(ARGLIST);
                      }
                    | expr
                      {
                          tree *a = maketree(ARGLIST);
                          addChild(a, $1);
                          $$ = a;
                      }
                    | argList COMMA expr
                      {
                          tree *a = maketree(ARGLIST);
                          addChild(a, $1);
                          addChild(a, $3);
                          $$ = a;
                      }
                    ;

// expressions
expr                : addExpr
                      {
                          tree *e = maketree(EXPRESSION);
                          addChild(e, $1);
                          $$ = e;
                      }
                    | addExpr relop addExpr
                      {
                          tree *e = maketree(EXPRESSION);
                          tree *left = maketree(EXPRESSION);
                          addChild(left, $1);
                          addChild(e, left);
                          addChild(e, $2);
                          addChild(e, $3);
                          $$ = e;
                      }
                    ;

relop               : OPER_LT
                      {
                        $$ = maketreeWithVal(RELOP, OP_LT);
                      }
                    | OPER_LTE
                      {
                        $$ = maketreeWithVal(RELOP, OP_LTE);
                      }
                    | OPER_GT
                      {
                        $$ = maketreeWithVal(RELOP, OP_GT);
                      }
                    | OPER_GTE
                      {
                        $$ = maketreeWithVal(RELOP, OP_GTE);
                      }
                    | OPER_EQ
                      {
                        $$ = maketreeWithVal(RELOP, OP_EQ);
                      }
                    | OPER_NEQ
                      {
                        $$ = maketreeWithVal(RELOP, OP_NEQ);
                      }
                    ;

addExpr             : term
                      {
                        tree *e = maketree(ADDEXPR);
                        addChild(e, $1);
                        $$ = e;
                      }
                    | addExpr addop term
                      {
                        tree *e = maketree(ADDEXPR);
                        addChild(e, $1);
                        addChild(e, $2);
                        addChild(e, $3);
                        $$ = e;
                      }
                    ;


addop               : OPER_ADD
                      {
                        $$ = maketreeWithVal(ADDOP, OP_ADD);
                      }
                    | OPER_SUB
                      {
                        $$ = maketreeWithVal(ADDOP, OP_SUB);
                      }
                    ;

term                : factor
                      {
                        tree *t = maketree(TERM);
                        addChild(t, $1);
                        $$ = t;
                      }
                    | term mulop factor
                      {
                        tree *t = maketree(TERM);
                        addChild(t, $1);
                        addChild(t, $2);
                        addChild(t, $3);
                        $$ = t;
                      }
                    ;

mulop               : OPER_MUL
                      {
                        $$ = maketreeWithVal(MULOP, OP_MUL);
                      }
                    | OPER_DIV
                      {
                        $$ = maketreeWithVal(MULOP, OP_DIV);
                      }
                    ;

factor              : LPAREN expr RPAREN
                      {
                        tree *f = maketree(FACTOR);
                        addChild(f, $2);
                        $$ = f;
                      }
                    | var
                      {
                        tree *f = maketree(FACTOR);
                        addChild(f, $1);
                        $$ = f;
                      }
                    | funcCallExpr
                      {
                        tree *f = maketree(FACTOR);
                        addChild(f, $1);
                        $$ = f;
                      }
                    | INTCONST
                      {
                        tree *f = maketree(FACTOR);
                        tree *c = maketreeWithVal(INTEGER, $1);
                        addChild(f, c);
                        $$ = f;
                      }
                    | CHARCONST
                      {
                        tree *f = maketree(FACTOR);
                        tree *c = maketreeWithVal(CHAR_NODE, $1);
                        addChild(f, c);
                        $$ = f;
                      }
                    ;

%%

int yywarning(char *msg) {
    printf("error: line %d: %s\n", yylineno, msg);
    return 0;
}

void yyerror(const char *msg) {
    printf("error: line %d: %s\n", yylineno, msg);
}