/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_Else = 258,
    T_Or = 259,
    T_And = 260,
    T_Equal = 261,
    T_NotEqual = 262,
    T_GreaterEqual = 263,
    T_LessEqual = 264,
    T_Inc = 265,
    T_Dec = 266,
    T_Void = 267,
    T_Bool = 268,
    T_Int = 269,
    T_Double = 270,
    T_String = 271,
    T_Class = 272,
    T_Dims = 273,
    T_Null = 274,
    T_Extends = 275,
    T_This = 276,
    T_Interface = 277,
    T_Implements = 278,
    T_While = 279,
    T_For = 280,
    T_If = 281,
    T_Return = 282,
    T_Break = 283,
    T_New = 284,
    T_NewArray = 285,
    T_Print = 286,
    T_ReadInteger = 287,
    T_ReadLine = 288,
    T_Identifier = 289,
    T_StringConstant = 290,
    T_IntConstant = 291,
    T_DoubleConstant = 292,
    T_BoolConstant = 293,
    T_NullConstant = 294
  };
#endif
/* Tokens.  */
#define T_Else 258
#define T_Or 259
#define T_And 260
#define T_Equal 261
#define T_NotEqual 262
#define T_GreaterEqual 263
#define T_LessEqual 264
#define T_Inc 265
#define T_Dec 266
#define T_Void 267
#define T_Bool 268
#define T_Int 269
#define T_Double 270
#define T_String 271
#define T_Class 272
#define T_Dims 273
#define T_Null 274
#define T_Extends 275
#define T_This 276
#define T_Interface 277
#define T_Implements 278
#define T_While 279
#define T_For 280
#define T_If 281
#define T_Return 282
#define T_Break 283
#define T_New 284
#define T_NewArray 285
#define T_Print 286
#define T_ReadInteger 287
#define T_ReadLine 288
#define T_Identifier 289
#define T_StringConstant 290
#define T_IntConstant 291
#define T_DoubleConstant 292
#define T_BoolConstant 293
#define T_NullConstant 294

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 57 "parser.y" /* yacc.c:1909  */

    int integerConstant;
    bool boolConstant;
    char *stringConstant;
    double doubleConstant;
    char identifier[MaxIdentLen+1]; // +1 for terminating null
    NullConstant *nullConstant;
    Decl *decl;
    VarDecl *var;
    FnDecl *fDecl;
    ClassDecl *cDecl;
    InterfaceDecl *iDecl;
    Type *type;
    Expr *expr;
    LValue *lValue;
    Call *call;
    Stmt *stmt;
    ConditionalStmt *cStmt;
    LoopStmt *lStmt;
    ReturnStmt *rStmt;
    BreakStmt *bStmt;
    PrintStmt *pStmt;
    List<Expr*> *exprList;
    List<Stmt*> *stmtList;
    List<VarDecl*> *varList;
    List<Decl*> *declList;
    List<NamedType*> *impList;
    NamedType *extend;

#line 162 "y.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
