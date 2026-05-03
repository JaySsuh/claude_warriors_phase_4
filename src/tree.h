#ifndef TREE_H
#define TREE_H
#define MAXCHILDREN 100

enum nodeTypes {
    PROGRAM, DECLLIST, DECL, VARDECL, TYPESPEC, FUNDECL,
    FORMALDECLLIST, FORMALDECL, FUNBODY, LOCALDECLLIST,
    STATEMENTLIST, STATEMENT, COMPOUNDSTMT, ASSIGNSTMT,
    CONDSTMT, LOOPSTMT, RETURNSTMT, EXPRESSION, RELOP,
    ADDEXPR, ADDOP, TERM, MULOP, FACTOR, FUNCCALLEXPR,
    ARGLIST, INTEGER, IDENTIFIER, VAR, ARRAYDECL, CHAR_NODE,
    FUNCTYPENAME, FORSTMT
};

#define CHARNODE CHAR_NODE

enum opType {
      OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_LT, OP_LTE, OP_EQ, OP_GTE, OP_GT, OP_NEQ
};

#define SCOPE_GLOBAL 0
#define SCOPE_LOCAL 1

typedef struct treenode tree;

/* tree node - you may want to add more fields */
struct treenode {
      int nodeKind;
      int numChildren;
      int val;
      int scope; // Used for var/id. Index of the scope. This works b/c only global and local.
      int type;
      int sym_type; // Only used by var to distinguish SCALAR vs ARRAY
      tree *parent;
      tree *children[MAXCHILDREN];
};

extern tree *ast; /* pointer to AST root */

/* builds sub tree with zeor children  */
tree *maketree(int kind);

/* builds sub tree with leaf node */
tree *maketreeWithVal(int kind, int val);

void addChild(tree *parent, tree *child);

void printAst(tree *root, int nestLevel);

/* Adds all children of sublist to list */
void flattenList(tree *list, tree *subList);

/* tree manipulation macros */
/* if you are writing your compiler in C, you would want to have a large collection of these */

#define nextAvailChild(node) node->children[node->numChildren]
#define getChild(node, index) node->children[index]
int yyparse(void);
#endif
