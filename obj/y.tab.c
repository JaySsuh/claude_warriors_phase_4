/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/parser.y"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<../src/tree.h>
#include<../src/strtab.h>

extern int yylineno;
extern int yylex(void);

int yywarning(char *msg);
void yyerror(const char *msg);

enum nodeTypes {PROGRAM, DECLLIST, DECL, VARDECL, TYPESPEC, FUNDECL,
                FORMALDECLLIST, FORMALDECL, FUNBODY, LOCALDECLLIST,
                STATEMENTLIST, STATEMENT, COMPOUNDSTMT, ASSIGNSTMT,
                CONDSTMT, LOOPSTMT, RETURNSTMT, EXPRESSION, RELOP,
                ADDEXPR, ADDOP, TERM, MULOP, FACTOR, FUNCCALLEXPR,
                ARGLIST, INTEGER, IDENTIFIER, VAR, ARRAYDECL, CHAR_NODE,
                FUNCTYPENAME};

enum opType {OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_LT, OP_LTE, OP_EQ, OP_GTE, OP_GT, OP_NEQ};

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
    


#line 200 "obj/y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "y.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_INTCONST = 4,                   /* INTCONST  */
  YYSYMBOL_CHARCONST = 5,                  /* CHARCONST  */
  YYSYMBOL_KWD_INT = 6,                    /* KWD_INT  */
  YYSYMBOL_KWD_CHAR = 7,                   /* KWD_CHAR  */
  YYSYMBOL_KWD_VOID = 8,                   /* KWD_VOID  */
  YYSYMBOL_KWD_IF = 9,                     /* KWD_IF  */
  YYSYMBOL_KWD_ELSE = 10,                  /* KWD_ELSE  */
  YYSYMBOL_KWD_WHILE = 11,                 /* KWD_WHILE  */
  YYSYMBOL_KWD_RETURN = 12,                /* KWD_RETURN  */
  YYSYMBOL_LPAREN = 13,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 14,                    /* RPAREN  */
  YYSYMBOL_LCRLY_BRKT = 15,                /* LCRLY_BRKT  */
  YYSYMBOL_RCRLY_BRKT = 16,                /* RCRLY_BRKT  */
  YYSYMBOL_LSQ_BRKT = 17,                  /* LSQ_BRKT  */
  YYSYMBOL_RSQ_BRKT = 18,                  /* RSQ_BRKT  */
  YYSYMBOL_SEMICLN = 19,                   /* SEMICLN  */
  YYSYMBOL_COMMA = 20,                     /* COMMA  */
  YYSYMBOL_OPER_ASGN = 21,                 /* OPER_ASGN  */
  YYSYMBOL_OPER_ADD = 22,                  /* OPER_ADD  */
  YYSYMBOL_OPER_SUB = 23,                  /* OPER_SUB  */
  YYSYMBOL_OPER_MUL = 24,                  /* OPER_MUL  */
  YYSYMBOL_OPER_DIV = 25,                  /* OPER_DIV  */
  YYSYMBOL_OPER_LT = 26,                   /* OPER_LT  */
  YYSYMBOL_OPER_LTE = 27,                  /* OPER_LTE  */
  YYSYMBOL_OPER_GT = 28,                   /* OPER_GT  */
  YYSYMBOL_OPER_GTE = 29,                  /* OPER_GTE  */
  YYSYMBOL_OPER_EQ = 30,                   /* OPER_EQ  */
  YYSYMBOL_OPER_NEQ = 31,                  /* OPER_NEQ  */
  YYSYMBOL_ERROR = 32,                     /* ERROR  */
  YYSYMBOL_ILLEGAL_TOK = 33,               /* ILLEGAL_TOK  */
  YYSYMBOL_LOWER_THAN_ELSE = 34,           /* LOWER_THAN_ELSE  */
  YYSYMBOL_YYACCEPT = 35,                  /* $accept  */
  YYSYMBOL_program = 36,                   /* program  */
  YYSYMBOL_37_1 = 37,                      /* $@1  */
  YYSYMBOL_declList = 38,                  /* declList  */
  YYSYMBOL_decl = 39,                      /* decl  */
  YYSYMBOL_varDecl = 40,                   /* varDecl  */
  YYSYMBOL_typeSpec = 41,                  /* typeSpec  */
  YYSYMBOL_funDecl = 42,                   /* funDecl  */
  YYSYMBOL_43_2 = 43,                      /* $@2  */
  YYSYMBOL_formalDeclList = 44,            /* formalDeclList  */
  YYSYMBOL_formalDecl = 45,                /* formalDecl  */
  YYSYMBOL_funBody = 46,                   /* funBody  */
  YYSYMBOL_localDeclList = 47,             /* localDeclList  */
  YYSYMBOL_stmtList = 48,                  /* stmtList  */
  YYSYMBOL_stmt = 49,                      /* stmt  */
  YYSYMBOL_compoundStmt = 50,              /* compoundStmt  */
  YYSYMBOL_assignStmt = 51,                /* assignStmt  */
  YYSYMBOL_condStmt = 52,                  /* condStmt  */
  YYSYMBOL_loopStmt = 53,                  /* loopStmt  */
  YYSYMBOL_returnStmt = 54,                /* returnStmt  */
  YYSYMBOL_exprStmt = 55,                  /* exprStmt  */
  YYSYMBOL_var = 56,                       /* var  */
  YYSYMBOL_funcCallExpr = 57,              /* funcCallExpr  */
  YYSYMBOL_58_3 = 58,                      /* $@3  */
  YYSYMBOL_59_4 = 59,                      /* $@4  */
  YYSYMBOL_argList = 60,                   /* argList  */
  YYSYMBOL_expr = 61,                      /* expr  */
  YYSYMBOL_relop = 62,                     /* relop  */
  YYSYMBOL_addExpr = 63,                   /* addExpr  */
  YYSYMBOL_addop = 64,                     /* addop  */
  YYSYMBOL_term = 65,                      /* term  */
  YYSYMBOL_mulop = 66,                     /* mulop  */
  YYSYMBOL_factor = 67                     /* factor  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   105

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  35
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  33
/* YYNRULES -- Number of rules.  */
#define YYNRULES  67
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  111

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   289


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   170,   170,   170,   178,   184,   194,   200,   208,   224,
     254,   260,   266,   276,   275,   313,   316,   322,   333,   351,
     373,   387,   390,   403,   406,   419,   423,   427,   431,   435,
     439,   445,   456,   484,   493,   507,   518,   522,   530,   539,
     557,   623,   623,   623,   671,   674,   680,   690,   696,   708,
     712,   716,   720,   724,   728,   734,   740,   751,   755,   761,
     767,   777,   781,   787,   793,   799,   805,   812
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "INTCONST",
  "CHARCONST", "KWD_INT", "KWD_CHAR", "KWD_VOID", "KWD_IF", "KWD_ELSE",
  "KWD_WHILE", "KWD_RETURN", "LPAREN", "RPAREN", "LCRLY_BRKT",
  "RCRLY_BRKT", "LSQ_BRKT", "RSQ_BRKT", "SEMICLN", "COMMA", "OPER_ASGN",
  "OPER_ADD", "OPER_SUB", "OPER_MUL", "OPER_DIV", "OPER_LT", "OPER_LTE",
  "OPER_GT", "OPER_GTE", "OPER_EQ", "OPER_NEQ", "ERROR", "ILLEGAL_TOK",
  "LOWER_THAN_ELSE", "$accept", "program", "$@1", "declList", "decl",
  "varDecl", "typeSpec", "funDecl", "$@2", "formalDeclList", "formalDecl",
  "funBody", "localDeclList", "stmtList", "stmt", "compoundStmt",
  "assignStmt", "condStmt", "loopStmt", "returnStmt", "exprStmt", "var",
  "funcCallExpr", "$@3", "$@4", "argList", "expr", "relop", "addExpr",
  "addop", "term", "mulop", "factor", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-78)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -78,    18,    61,   -78,   -78,   -78,   -78,    61,   -78,   -78,
      10,   -78,   -78,    44,    24,   -78,    21,    36,    61,    45,
      69,    59,    54,   -78,    60,    63,    61,    58,    61,   -78,
     -78,   -78,    61,    76,    34,   -78,    44,    -6,   -78,   -78,
      67,    68,    11,    37,    61,    66,    34,   -78,   -78,   -78,
     -78,   -78,   -78,    62,   -78,    65,    29,   -15,   -78,   -78,
      37,    37,    37,   -78,   -78,    70,    71,    34,   -78,   -78,
      37,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
      37,    37,   -78,   -78,    37,    37,    13,    72,    73,   -78,
     -78,    74,    75,    48,   -15,   -78,    77,   -78,   -78,    34,
      34,   -78,   -78,    37,    78,    81,   -78,   -78,   -78,    34,
     -78
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     0,     1,    10,    11,    12,     3,     4,     6,
       0,     7,     5,    13,     0,     8,     0,     0,    15,     0,
       0,     0,    16,     9,    18,     0,    15,     0,    21,    14,
      17,    19,    21,     0,    23,    22,     0,    39,    66,    67,
       0,     0,     0,     0,    21,     0,    23,    25,    26,    27,
      28,    29,    30,    64,    65,     0,    47,    55,    59,    41,
       0,     0,     0,    36,    64,     0,     0,    23,    20,    24,
       0,    38,    57,    58,    49,    50,    51,    52,    53,    54,
       0,     0,    61,    62,     0,    44,     0,     0,     0,    37,
      63,     0,     0,    48,    56,    60,    42,    45,    40,     0,
       0,    31,    32,     0,     0,    33,    35,    46,    43,     0,
      34
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -78,   -78,   -78,   -78,    86,    46,     1,   -78,   -78,    79,
     -78,   -78,   -27,   -42,   -77,   -78,   -78,   -78,   -78,   -78,
     -78,   -34,   -78,   -78,   -78,   -78,   -41,   -78,   -54,   -78,
       7,   -78,    12
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     2,     7,     8,    32,    33,    11,    16,    21,
      22,    29,    34,    45,    46,    47,    48,    49,    50,    51,
      52,    64,    54,    85,   104,    96,    55,    80,    56,    81,
      57,    84,    58
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      53,    65,    66,    10,    69,    35,    86,    59,    10,    82,
      83,    60,    53,    13,    37,    38,    39,    67,     3,    20,
      87,    88,   105,   106,    43,    91,    93,    20,    17,    92,
      63,    98,   110,    53,    18,    72,    73,    37,    38,    39,
      37,    38,    39,    40,    97,    41,    42,    43,     9,    44,
      43,    72,    73,     9,    19,    74,    75,    76,    77,    78,
      79,    14,   107,    15,    23,    53,    53,     4,     5,     6,
      72,    73,    24,    25,    26,    53,    31,    27,    28,    36,
      61,    62,    68,    70,    71,    90,    99,   100,    94,    89,
     101,   109,   108,    12,   102,     0,    95,   103,     0,     0,
       0,     0,     0,     0,     0,    30
};

static const yytype_int8 yycheck[] =
{
      34,    42,    43,     2,    46,    32,    60,    13,     7,    24,
      25,    17,    46,     3,     3,     4,     5,    44,     0,    18,
      61,    62,    99,   100,    13,    67,    80,    26,     4,    70,
      19,    18,   109,    67,    13,    22,    23,     3,     4,     5,
       3,     4,     5,     9,    85,    11,    12,    13,     2,    15,
      13,    22,    23,     7,    18,    26,    27,    28,    29,    30,
      31,    17,   103,    19,    19,    99,   100,     6,     7,     8,
      22,    23,     3,    14,    20,   109,    18,    17,    15,     3,
      13,    13,    16,    21,    19,    14,    14,    14,    81,    19,
      16,    10,    14,     7,    19,    -1,    84,    20,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    26
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    36,    37,     0,     6,     7,     8,    38,    39,    40,
      41,    42,    39,     3,    17,    19,    43,     4,    13,    18,
      41,    44,    45,    19,     3,    14,    20,    17,    15,    46,
      44,    18,    40,    41,    47,    47,     3,     3,     4,     5,
       9,    11,    12,    13,    15,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    61,    63,    65,    67,    13,
      17,    13,    13,    19,    56,    61,    61,    47,    16,    48,
      21,    19,    22,    23,    26,    27,    28,    29,    30,    31,
      62,    64,    24,    25,    66,    58,    63,    61,    61,    19,
      14,    48,    61,    63,    65,    67,    60,    61,    18,    14,
      14,    16,    19,    20,    59,    49,    49,    61,    14,    10,
      49
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    35,    37,    36,    38,    38,    39,    39,    40,    40,
      41,    41,    41,    43,    42,    44,    44,    44,    45,    45,
      46,    47,    47,    48,    48,    49,    49,    49,    49,    49,
      49,    50,    51,    52,    52,    53,    54,    54,    55,    56,
      56,    58,    59,    57,    60,    60,    60,    61,    61,    62,
      62,    62,    62,    62,    62,    63,    63,    64,    64,    65,
      65,    66,    66,    67,    67,    67,    67,    67
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     2,     1,     1,     3,     6,
       1,     1,     1,     0,     7,     0,     1,     3,     2,     4,
       4,     0,     2,     0,     2,     1,     1,     1,     1,     1,
       1,     4,     4,     5,     7,     5,     2,     3,     2,     1,
       4,     0,     0,     6,     0,     1,     3,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     1,     1,
       3,     1,     1,     3,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* $@1: %empty  */
#line 170 "src/parser.y"
                  {new_scope();}
#line 1330 "obj/y.tab.c"
    break;

  case 3: /* program: $@1 declList  */
#line 171 "src/parser.y"
                  {
                    tree *n = maketree(PROGRAM);
                    addChild(n, (yyvsp[0].node));
                    ast = n;
                  }
#line 1340 "obj/y.tab.c"
    break;

  case 4: /* declList: decl  */
#line 179 "src/parser.y"
                  {
                    tree *n = maketree(DECLLIST);
                    addChild(n, (yyvsp[0].node));
                    (yyval.node) = n;
                  }
#line 1350 "obj/y.tab.c"
    break;

  case 5: /* declList: declList decl  */
#line 185 "src/parser.y"
                  {
                    tree *n = maketree(DECLLIST);
                    addChild(n, (yyvsp[-1].node));
                    addChild(n, (yyvsp[0].node));
                    (yyval.node) = n;
                  }
#line 1361 "obj/y.tab.c"
    break;

  case 6: /* decl: varDecl  */
#line 195 "src/parser.y"
                  {
                    tree *n = maketree(DECL);
                    addChild(n, (yyvsp[0].node));
                    (yyval.node) = n;
                  }
#line 1371 "obj/y.tab.c"
    break;

  case 7: /* decl: funDecl  */
#line 201 "src/parser.y"
                  {
                    tree *n = maketree(DECL);
                    addChild(n, (yyvsp[0].node));
                    (yyval.node) = n;
                  }
#line 1381 "obj/y.tab.c"
    break;

  case 8: /* varDecl: typeSpec ID SEMICLN  */
#line 209 "src/parser.y"
                  {
                    tree *n = maketree(VARDECL);
                    tree *id = maketree(IDENTIFIER);
                    id->val = -1;

                    if (ST_lookup_current((yyvsp[-1].strval)) != NULL) {
                        yywarning("Symbol declared multiple times.");
                    } else {
                        id->val = ST_insert((yyvsp[-1].strval), (yyvsp[-2].node)->val, SCALAR, (int *)scope);
                    }

                    addChild(n, (yyvsp[-2].node));
                    addChild(n, id);
                    (yyval.node) = n;
                  }
#line 1401 "obj/y.tab.c"
    break;

  case 9: /* varDecl: typeSpec ID LSQ_BRKT INTCONST RSQ_BRKT SEMICLN  */
#line 225 "src/parser.y"
                  {
                    tree *n = maketree(VARDECL);
                    tree *id = maketree(IDENTIFIER);
                    tree *sz = maketreeWithVal(INTEGER, (yyvsp[-2].value));
                    id->val = -1;

                    if ((yyvsp[-2].value) == 0) {
                      yywarning("Array variable declared with size of zero.");
                    }

                    if (ST_lookup_current((yyvsp[-4].strval)) != NULL) {
                        yywarning("Symbol declared multiple times.");
                    } else {
                        id->val = ST_insert((yyvsp[-4].strval), (yyvsp[-5].node)->val, ARRAY, (int *)scope);
                        if (id->val >= 0) {
                            symEntry *e = ST_lookup((yyvsp[-4].strval));
                            if (e) {
                                e->size = (yyvsp[-2].value);
                            }
                        }
                    }

                    addChild(n, (yyvsp[-5].node));  // type
                    addChild(n, id);  // name
                    addChild(n, sz);  // size
                    (yyval.node) = n;
                  }
#line 1433 "obj/y.tab.c"
    break;

  case 10: /* typeSpec: KWD_INT  */
#line 255 "src/parser.y"
                  {
                    tree *t = maketree(TYPESPEC);
                    t->val = INT_TYPE;
                    (yyval.node) = t;
                  }
#line 1443 "obj/y.tab.c"
    break;

  case 11: /* typeSpec: KWD_CHAR  */
#line 261 "src/parser.y"
                  {
                    tree *t = maketree(TYPESPEC);
                    t->val = CHAR_TYPE;
                    (yyval.node) = t;
                  }
#line 1453 "obj/y.tab.c"
    break;

  case 12: /* typeSpec: KWD_VOID  */
#line 267 "src/parser.y"
                  {
                    tree *t = maketree(TYPESPEC);
                    t->val = VOID_TYPE;
                    (yyval.node) = t;
                  }
#line 1463 "obj/y.tab.c"
    break;

  case 13: /* $@2: %empty  */
#line 276 "src/parser.y"
                  {
                    if (ST_lookup_current((yyvsp[0].strval)) != NULL) {
                        yywarning("Symbol declared multiple times.");
                        func_index = -1;
                    } else {
                        func_index = ST_insert((yyvsp[0].strval), (yyvsp[-1].node)->val, FUNCTION, (int *)"");
                    }
                    func_params = 0;
                    scope = strdup((yyvsp[0].strval));
                    new_scope();
                  }
#line 1479 "obj/y.tab.c"
    break;

  case 14: /* funDecl: typeSpec ID $@2 LPAREN formalDeclList RPAREN funBody  */
#line 288 "src/parser.y"
                  {
                    if (func_index >= 0) {
                        connect_params(func_index, func_params);
                    }
                    /* build AST */
                    tree *n = maketree(FUNDECL);
                    tree *ft = maketree(FUNCTYPENAME);
                    addChild(ft, (yyvsp[-6].node));
                    tree *id = maketree(IDENTIFIER);
                    id->val = func_index;
                    addChild(ft, id);
                    addChild(n, ft);
                    if ((yyvsp[-2].node)->numChildren > 0) {
                        addChild(n, (yyvsp[-2].node));
                    }
                    addChild(n, (yyvsp[0].node));
                    (yyval.node) = n;

                    up_scope();
                    /* after finishing this function, reset scope to global */
                    scope = "";
                  }
#line 1506 "obj/y.tab.c"
    break;

  case 15: /* formalDeclList: %empty  */
#line 313 "src/parser.y"
                  {
                    (yyval.node) = maketree(FORMALDECLLIST);
                  }
#line 1514 "obj/y.tab.c"
    break;

  case 16: /* formalDeclList: formalDecl  */
#line 317 "src/parser.y"
                  {
                    tree *l = maketree(FORMALDECLLIST);
                    addChild(l, (yyvsp[0].node));
                    (yyval.node) = l;
                  }
#line 1524 "obj/y.tab.c"
    break;

  case 17: /* formalDeclList: formalDecl COMMA formalDeclList  */
#line 323 "src/parser.y"
                  {
                    tree *l = maketree(FORMALDECLLIST);
                    addChild(l, (yyvsp[-2].node));
                    if ((yyvsp[0].node)->numChildren > 0) {
                        addChild(l, (yyvsp[0].node));
                    }
                    (yyval.node) = l;
                  }
#line 1537 "obj/y.tab.c"
    break;

  case 18: /* formalDecl: typeSpec ID  */
#line 334 "src/parser.y"
                  {
                    tree *n = maketree(FORMALDECL);
                    tree *id = maketree(IDENTIFIER);
                    id->val = -1;

                    if (ST_lookup_current((yyvsp[0].strval)) != NULL) {
                        yywarning("Symbol declared multiple times.");
                    } else {
                        id->val = ST_insert((yyvsp[0].strval), (yyvsp[-1].node)->val, SCALAR, (int *)scope);
                        add_param((yyvsp[-1].node)->val, SCALAR);
                        func_params++;
                    }

                    addChild(n, (yyvsp[-1].node));
                    addChild(n, id);
                    (yyval.node) = n;
                  }
#line 1559 "obj/y.tab.c"
    break;

  case 19: /* formalDecl: typeSpec ID LSQ_BRKT RSQ_BRKT  */
#line 352 "src/parser.y"
                  {
                    tree *n = maketree(FORMALDECL);
                    tree *id = maketree(IDENTIFIER);
                    tree *arr = maketree(ARRAYDECL);
                    id->val = -1;

                    if (ST_lookup_current((yyvsp[-2].strval)) != NULL) {
                        yywarning("Symbol declared multiple times.");
                    } else {
                        id->val = ST_insert((yyvsp[-2].strval), (yyvsp[-3].node)->val, ARRAY, (int *)scope);
                        add_param((yyvsp[-3].node)->val, ARRAY);
                        func_params++;
                    }

                    addChild(n, (yyvsp[-3].node));
                    addChild(n, id);
                    addChild(n, arr);
                    (yyval.node) = n;
                  }
#line 1583 "obj/y.tab.c"
    break;

  case 20: /* funBody: LCRLY_BRKT localDeclList stmtList RCRLY_BRKT  */
#line 374 "src/parser.y"
                  {
                    tree *n = maketree(FUNBODY);
                    if((yyvsp[-2].node)->numChildren > 0) {
                        addChild(n, (yyvsp[-2].node));
                    }
                    if((yyvsp[-1].node)->numChildren > 0) {
                        addChild(n, (yyvsp[-1].node));
                    }
                    (yyval.node) = n;
                  }
#line 1598 "obj/y.tab.c"
    break;

  case 21: /* localDeclList: %empty  */
#line 387 "src/parser.y"
                  {
                    (yyval.node) = maketree(LOCALDECLLIST);
                  }
#line 1606 "obj/y.tab.c"
    break;

  case 22: /* localDeclList: varDecl localDeclList  */
#line 391 "src/parser.y"
                  {
                    tree *l = maketree(LOCALDECLLIST);
                    addChild(l, (yyvsp[-1].node));
                    if ((yyvsp[0].node)->numChildren > 0) {
                        addChild(l, (yyvsp[0].node));
                    }
                    (yyval.node) = l;
                  }
#line 1619 "obj/y.tab.c"
    break;

  case 23: /* stmtList: %empty  */
#line 403 "src/parser.y"
                  {
                    (yyval.node) = maketree(STATEMENTLIST);
                  }
#line 1627 "obj/y.tab.c"
    break;

  case 24: /* stmtList: stmt stmtList  */
#line 407 "src/parser.y"
                  {
                    tree *sl = maketree(STATEMENTLIST);
                    tree *s = maketree(STATEMENT);
                    addChild(s, (yyvsp[-1].node));
                    addChild(sl, s);
                    if ((yyvsp[0].node)->numChildren > 0) {
                        addChild(sl, (yyvsp[0].node));
                    }
                    (yyval.node) = sl;
                  }
#line 1642 "obj/y.tab.c"
    break;

  case 25: /* stmt: compoundStmt  */
#line 420 "src/parser.y"
                      {
                        (yyval.node) = (yyvsp[0].node);
                      }
#line 1650 "obj/y.tab.c"
    break;

  case 26: /* stmt: assignStmt  */
#line 424 "src/parser.y"
                      {
                        (yyval.node) = (yyvsp[0].node);
                      }
#line 1658 "obj/y.tab.c"
    break;

  case 27: /* stmt: condStmt  */
#line 428 "src/parser.y"
                      {
                        (yyval.node) = (yyvsp[0].node);
                      }
#line 1666 "obj/y.tab.c"
    break;

  case 28: /* stmt: loopStmt  */
#line 432 "src/parser.y"
                      {
                        (yyval.node) = (yyvsp[0].node);
                      }
#line 1674 "obj/y.tab.c"
    break;

  case 29: /* stmt: returnStmt  */
#line 436 "src/parser.y"
                      {
                        (yyval.node) = (yyvsp[0].node);
                      }
#line 1682 "obj/y.tab.c"
    break;

  case 30: /* stmt: exprStmt  */
#line 440 "src/parser.y"
                      {
                        (yyval.node) = (yyvsp[0].node);
                      }
#line 1690 "obj/y.tab.c"
    break;

  case 31: /* compoundStmt: LCRLY_BRKT localDeclList stmtList RCRLY_BRKT  */
#line 446 "src/parser.y"
                      {
                        tree *n = maketree(COMPOUNDSTMT);
                        if ((yyvsp[-2].node)->numChildren > 0) {
                            addChild(n, (yyvsp[-2].node));
                        }
                        addChild(n, (yyvsp[-1].node));
                        (yyval.node) = n;
                      }
#line 1703 "obj/y.tab.c"
    break;

  case 32: /* assignStmt: var OPER_ASGN expr SEMICLN  */
#line 457 "src/parser.y"
                      {
                        tree *n = maketree(ASSIGNSTMT);
                        addChild(n, (yyvsp[-3].node)); // lhs
                        addChild(n, (yyvsp[-1].node)); // rhs
                        
                        /* type mismatch check for simple RHS expressions */
                        int lhs_type = -1;
                        if ((yyvsp[-3].node)->numChildren > 0) {
                            int slot = getChild((yyvsp[-3].node), 0)->val;
                            table_node *sc = current_scope;
                            while (sc) {
                                if (slot >= 0 && sc->strTable[slot] != NULL) {
                                    lhs_type = sc->strTable[slot]->data_type;
                                    break;
                                }
                                sc = sc->parent;
                            }
                        }

                        int rhs_type = expr_type((yyvsp[-1].node));
                        if (lhs_type != -1 && rhs_type != -1 && lhs_type != rhs_type) {
                            yywarning("Type mismatch in assignment.");
                        }
                        (yyval.node) = n;
                      }
#line 1733 "obj/y.tab.c"
    break;

  case 33: /* condStmt: KWD_IF LPAREN expr RPAREN stmt  */
#line 485 "src/parser.y"
                      {
                        tree *n = maketree(CONDSTMT);
                        tree *s = maketree(STATEMENT);
                        addChild(s, (yyvsp[0].node));
                        addChild(n, (yyvsp[-2].node)); // condition
                        addChild(n, s); // then branch
                        (yyval.node) = n;
                      }
#line 1746 "obj/y.tab.c"
    break;

  case 34: /* condStmt: KWD_IF LPAREN expr RPAREN stmt KWD_ELSE stmt  */
#line 494 "src/parser.y"
                      {
                        tree *n = maketree(CONDSTMT);
                        tree *s1 = maketree(STATEMENT);
                        tree *s2 = maketree(STATEMENT);
                        addChild(s1, (yyvsp[-2].node));
                        addChild(s2, (yyvsp[0].node));
                        addChild(n, (yyvsp[-4].node)); // condition
                        addChild(n, s1); // then branch
                        addChild(n, s2); // else branch
                        (yyval.node) = n;
                      }
#line 1762 "obj/y.tab.c"
    break;

  case 35: /* loopStmt: KWD_WHILE LPAREN expr RPAREN stmt  */
#line 508 "src/parser.y"
                      {
                          tree *n = maketree(LOOPSTMT);
                          tree *s = maketree(STATEMENT);
                          addChild(s, (yyvsp[0].node));
                          addChild(n, (yyvsp[-2].node)); // condition
                          addChild(n, s); // body
                          (yyval.node) = n;
                      }
#line 1775 "obj/y.tab.c"
    break;

  case 36: /* returnStmt: KWD_RETURN SEMICLN  */
#line 519 "src/parser.y"
                      {
                          (yyval.node) = maketree(RETURNSTMT);
                      }
#line 1783 "obj/y.tab.c"
    break;

  case 37: /* returnStmt: KWD_RETURN expr SEMICLN  */
#line 523 "src/parser.y"
                      {
                        tree *n = maketree(RETURNSTMT);
                        addChild(n, (yyvsp[-1].node));
                        (yyval.node) = n;
                      }
#line 1793 "obj/y.tab.c"
    break;

  case 38: /* exprStmt: expr SEMICLN  */
#line 531 "src/parser.y"
                      {
                        tree *n = maketree(ASSIGNSTMT);
                        addChild(n, (yyvsp[-1].node));
                        (yyval.node) = n;
                      }
#line 1803 "obj/y.tab.c"
    break;

  case 39: /* var: ID  */
#line 540 "src/parser.y"
                      {
                        tree *v = maketree(VAR);
                        tree *id = maketree(IDENTIFIER);
                        id->val = -1;

                        symEntry *e = ST_lookup((yyvsp[0].strval));
                        if (e == NULL) {
                          if (!suppress_undeclared) {
                            yywarning("Undeclared variable");
                          }
                        } else {
                            id->val = find_slot((yyvsp[0].strval));
                        }

                        addChild(v, id);
                        (yyval.node) = v;
                      }
#line 1825 "obj/y.tab.c"
    break;

  case 40: /* var: ID LSQ_BRKT addExpr RSQ_BRKT  */
#line 558 "src/parser.y"
                      {
                        int saved_line = yylineno;
                        tree *v = maketree(VAR);
                        tree *id = maketree(IDENTIFIER);
                        id->val = -1;

                        symEntry *e = ST_lookup((yyvsp[-3].strval));
                        if (e == NULL) {
                            printf("error: line %d: %s\n", saved_line, "Undeclared identifier");
                        } else {
                            id->val = find_slot((yyvsp[-3].strval));

                            if (e->symbol_type != ARRAY) {
                                printf("error: line %d: %s\n", saved_line, "Non-array identifier used as an array.");
                            } else {
                                /* unwrap index expression to innermost node */
                                tree *ndx = (yyvsp[-1].node);
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
                        addChild(v, (yyvsp[-1].node));
                        (yyval.node) = v;
                      }
#line 1893 "obj/y.tab.c"
    break;

  case 41: /* $@3: %empty  */
#line 623 "src/parser.y"
                                {suppress_undeclared = 1; }
#line 1899 "obj/y.tab.c"
    break;

  case 42: /* $@4: %empty  */
#line 623 "src/parser.y"
                                                                    { suppress_undeclared = 0; }
#line 1905 "obj/y.tab.c"
    break;

  case 43: /* funcCallExpr: ID LPAREN $@3 argList $@4 RPAREN  */
#line 624 "src/parser.y"
                      {
                        tree *f  = maketree(FUNCCALLEXPR);
                        tree *id = maketree(IDENTIFIER);
                        id->val  = -1;

                        symEntry *e = ST_lookup((yyvsp[-5].strval));
                        if (e == NULL) {
                            yywarning("Undefined function");
                        } else {
                            id->val = find_slot((yyvsp[-5].strval));

                            if (e->symbol_type != FUNCTION) {
                                yywarning("called identifier is not a function");
                            } else {
                                int call_argc = count_args((yyvsp[-2].node));
                                if (call_argc != e->size) {
                                  if(call_argc < e->size) {
                                    yywarning("Too few arguments provided in function call.");
                                } else {
                                  yywarning("Too many arguments provided in function call.");
                                }
                                } else if (e->params != NULL) {
                                    tree *args[MAXCHILDREN];
                                    int nargs = 0;
                                    gather_args((yyvsp[-2].node), args, &nargs);

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
                        if ((yyvsp[-2].node)->numChildren > 0) {
                            addChild(f, (yyvsp[-2].node));
                        }
                        (yyval.node) = f;
                      }
#line 1954 "obj/y.tab.c"
    break;

  case 44: /* argList: %empty  */
#line 671 "src/parser.y"
                      {
                        (yyval.node) = maketree(ARGLIST);
                      }
#line 1962 "obj/y.tab.c"
    break;

  case 45: /* argList: expr  */
#line 675 "src/parser.y"
                      {
                          tree *a = maketree(ARGLIST);
                          addChild(a, (yyvsp[0].node));
                          (yyval.node) = a;
                      }
#line 1972 "obj/y.tab.c"
    break;

  case 46: /* argList: argList COMMA expr  */
#line 681 "src/parser.y"
                      {
                          tree *a = maketree(ARGLIST);
                          addChild(a, (yyvsp[-2].node));
                          addChild(a, (yyvsp[0].node));
                          (yyval.node) = a;
                      }
#line 1983 "obj/y.tab.c"
    break;

  case 47: /* expr: addExpr  */
#line 691 "src/parser.y"
                      {
                          tree *e = maketree(EXPRESSION);
                          addChild(e, (yyvsp[0].node));
                          (yyval.node) = e;
                      }
#line 1993 "obj/y.tab.c"
    break;

  case 48: /* expr: addExpr relop addExpr  */
#line 697 "src/parser.y"
                      {
                          tree *e = maketree(EXPRESSION);
                          tree *left = maketree(EXPRESSION);
                          addChild(left, (yyvsp[-2].node));
                          addChild(e, left);
                          addChild(e, (yyvsp[-1].node));
                          addChild(e, (yyvsp[0].node));
                          (yyval.node) = e;
                      }
#line 2007 "obj/y.tab.c"
    break;

  case 49: /* relop: OPER_LT  */
#line 709 "src/parser.y"
                      {
                        (yyval.node) = maketreeWithVal(RELOP, OP_LT);
                      }
#line 2015 "obj/y.tab.c"
    break;

  case 50: /* relop: OPER_LTE  */
#line 713 "src/parser.y"
                      {
                        (yyval.node) = maketreeWithVal(RELOP, OP_LTE);
                      }
#line 2023 "obj/y.tab.c"
    break;

  case 51: /* relop: OPER_GT  */
#line 717 "src/parser.y"
                      {
                        (yyval.node) = maketreeWithVal(RELOP, OP_GT);
                      }
#line 2031 "obj/y.tab.c"
    break;

  case 52: /* relop: OPER_GTE  */
#line 721 "src/parser.y"
                      {
                        (yyval.node) = maketreeWithVal(RELOP, OP_GTE);
                      }
#line 2039 "obj/y.tab.c"
    break;

  case 53: /* relop: OPER_EQ  */
#line 725 "src/parser.y"
                      {
                        (yyval.node) = maketreeWithVal(RELOP, OP_EQ);
                      }
#line 2047 "obj/y.tab.c"
    break;

  case 54: /* relop: OPER_NEQ  */
#line 729 "src/parser.y"
                      {
                        (yyval.node) = maketreeWithVal(RELOP, OP_NEQ);
                      }
#line 2055 "obj/y.tab.c"
    break;

  case 55: /* addExpr: term  */
#line 735 "src/parser.y"
                      {
                        tree *e = maketree(ADDEXPR);
                        addChild(e, (yyvsp[0].node));
                        (yyval.node) = e;
                      }
#line 2065 "obj/y.tab.c"
    break;

  case 56: /* addExpr: addExpr addop term  */
#line 741 "src/parser.y"
                      {
                        tree *e = maketree(ADDEXPR);
                        addChild(e, (yyvsp[-2].node));
                        addChild(e, (yyvsp[-1].node));
                        addChild(e, (yyvsp[0].node));
                        (yyval.node) = e;
                      }
#line 2077 "obj/y.tab.c"
    break;

  case 57: /* addop: OPER_ADD  */
#line 752 "src/parser.y"
                      {
                        (yyval.node) = maketreeWithVal(ADDOP, OP_ADD);
                      }
#line 2085 "obj/y.tab.c"
    break;

  case 58: /* addop: OPER_SUB  */
#line 756 "src/parser.y"
                      {
                        (yyval.node) = maketreeWithVal(ADDOP, OP_SUB);
                      }
#line 2093 "obj/y.tab.c"
    break;

  case 59: /* term: factor  */
#line 762 "src/parser.y"
                      {
                        tree *t = maketree(TERM);
                        addChild(t, (yyvsp[0].node));
                        (yyval.node) = t;
                      }
#line 2103 "obj/y.tab.c"
    break;

  case 60: /* term: term mulop factor  */
#line 768 "src/parser.y"
                      {
                        tree *t = maketree(TERM);
                        addChild(t, (yyvsp[-2].node));
                        addChild(t, (yyvsp[-1].node));
                        addChild(t, (yyvsp[0].node));
                        (yyval.node) = t;
                      }
#line 2115 "obj/y.tab.c"
    break;

  case 61: /* mulop: OPER_MUL  */
#line 778 "src/parser.y"
                      {
                        (yyval.node) = maketreeWithVal(MULOP, OP_MUL);
                      }
#line 2123 "obj/y.tab.c"
    break;

  case 62: /* mulop: OPER_DIV  */
#line 782 "src/parser.y"
                      {
                        (yyval.node) = maketreeWithVal(MULOP, OP_DIV);
                      }
#line 2131 "obj/y.tab.c"
    break;

  case 63: /* factor: LPAREN expr RPAREN  */
#line 788 "src/parser.y"
                      {
                        tree *f = maketree(FACTOR);
                        addChild(f, (yyvsp[-1].node));
                        (yyval.node) = f;
                      }
#line 2141 "obj/y.tab.c"
    break;

  case 64: /* factor: var  */
#line 794 "src/parser.y"
                      {
                        tree *f = maketree(FACTOR);
                        addChild(f, (yyvsp[0].node));
                        (yyval.node) = f;
                      }
#line 2151 "obj/y.tab.c"
    break;

  case 65: /* factor: funcCallExpr  */
#line 800 "src/parser.y"
                      {
                        tree *f = maketree(FACTOR);
                        addChild(f, (yyvsp[0].node));
                        (yyval.node) = f;
                      }
#line 2161 "obj/y.tab.c"
    break;

  case 66: /* factor: INTCONST  */
#line 806 "src/parser.y"
                      {
                        tree *f = maketree(FACTOR);
                        tree *c = maketreeWithVal(INTEGER, (yyvsp[0].value));
                        addChild(f, c);
                        (yyval.node) = f;
                      }
#line 2172 "obj/y.tab.c"
    break;

  case 67: /* factor: CHARCONST  */
#line 813 "src/parser.y"
                      {
                        tree *f = maketree(FACTOR);
                        tree *c = maketreeWithVal(CHAR_NODE, (yyvsp[0].value));
                        addChild(f, c);
                        (yyval.node) = f;
                      }
#line 2183 "obj/y.tab.c"
    break;


#line 2187 "obj/y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 821 "src/parser.y"


int yywarning(char *msg) {
    printf("error: line %d: %s\n", yylineno, msg);
    return 0;
}

void yyerror(const char *msg) {
    printf("error: line %d: %s\n", yylineno, msg);
}
