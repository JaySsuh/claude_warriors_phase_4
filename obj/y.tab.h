/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_OBJ_Y_TAB_H_INCLUDED
# define YY_YY_OBJ_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 258,                      /* ID  */
    INTCONST = 259,                /* INTCONST  */
    CHARCONST = 260,               /* CHARCONST  */
    KWD_INT = 261,                 /* KWD_INT  */
    KWD_CHAR = 262,                /* KWD_CHAR  */
    KWD_VOID = 263,                /* KWD_VOID  */
    KWD_IF = 264,                  /* KWD_IF  */
    KWD_ELSE = 265,                /* KWD_ELSE  */
    KWD_WHILE = 266,               /* KWD_WHILE  */
    KWD_RETURN = 267,              /* KWD_RETURN  */
    LPAREN = 268,                  /* LPAREN  */
    RPAREN = 269,                  /* RPAREN  */
    LCRLY_BRKT = 270,              /* LCRLY_BRKT  */
    RCRLY_BRKT = 271,              /* RCRLY_BRKT  */
    LSQ_BRKT = 272,                /* LSQ_BRKT  */
    RSQ_BRKT = 273,                /* RSQ_BRKT  */
    SEMICLN = 274,                 /* SEMICLN  */
    COMMA = 275,                   /* COMMA  */
    OPER_ASGN = 276,               /* OPER_ASGN  */
    OPER_ADD = 277,                /* OPER_ADD  */
    OPER_SUB = 278,                /* OPER_SUB  */
    OPER_MUL = 279,                /* OPER_MUL  */
    OPER_DIV = 280,                /* OPER_DIV  */
    OPER_LT = 281,                 /* OPER_LT  */
    OPER_LTE = 282,                /* OPER_LTE  */
    OPER_GT = 283,                 /* OPER_GT  */
    OPER_GTE = 284,                /* OPER_GTE  */
    OPER_EQ = 285,                 /* OPER_EQ  */
    OPER_NEQ = 286,                /* OPER_NEQ  */
    ERROR = 287,                   /* ERROR  */
    ILLEGAL_TOK = 288,             /* ILLEGAL_TOK  */
    LOWER_THAN_ELSE = 289          /* LOWER_THAN_ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define ID 258
#define INTCONST 259
#define CHARCONST 260
#define KWD_INT 261
#define KWD_CHAR 262
#define KWD_VOID 263
#define KWD_IF 264
#define KWD_ELSE 265
#define KWD_WHILE 266
#define KWD_RETURN 267
#define LPAREN 268
#define RPAREN 269
#define LCRLY_BRKT 270
#define RCRLY_BRKT 271
#define LSQ_BRKT 272
#define RSQ_BRKT 273
#define SEMICLN 274
#define COMMA 275
#define OPER_ASGN 276
#define OPER_ADD 277
#define OPER_SUB 278
#define OPER_MUL 279
#define OPER_DIV 280
#define OPER_LT 281
#define OPER_LTE 282
#define OPER_GT 283
#define OPER_GTE 284
#define OPER_EQ 285
#define OPER_NEQ 286
#define ERROR 287
#define ILLEGAL_TOK 288
#define LOWER_THAN_ELSE 289

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 123 "src/parser.y"

    int value;
    struct treenode *node;
    char *strval;

#line 141 "obj/y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_OBJ_Y_TAB_H_INCLUDED  */
