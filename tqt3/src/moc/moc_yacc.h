/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CHAR_VAL = 258,
     INT_VAL = 259,
     DOUBLE_VAL = 260,
     STRING = 261,
     IDENTIFIER = 262,
     FRIEND = 263,
     TYPEDEF = 264,
     AUTO = 265,
     REGISTER = 266,
     STATIC = 267,
     EXTERN = 268,
     INLINE = 269,
     VIRTUAL = 270,
     CONST = 271,
     VOLATILE = 272,
     CHAR = 273,
     SHORT = 274,
     INT = 275,
     LONG = 276,
     SIGNED = 277,
     UNSIGNED = 278,
     FLOAT = 279,
     DOUBLE = 280,
     VOID = 281,
     ENUM = 282,
     CLASS = 283,
     STRUCT = 284,
     UNION = 285,
     ASM = 286,
     PRIVATE = 287,
     PROTECTED = 288,
     PUBLIC = 289,
     OPERATOR = 290,
     DBL_COLON = 291,
     TRIPLE_DOT = 292,
     TEMPLATE = 293,
     NAMESPACE = 294,
     USING = 295,
     MUTABLE = 296,
     THROW = 297,
     SIGNALS = 298,
     SLOTS = 299,
     TQ_OBJECT = 300,
     TQ_PROPERTY = 301,
     TQ_OVERRIDE = 302,
     TQ_CLASSINFO = 303,
     TQ_ENUMS = 304,
     TQ_SETS = 305,
     READ = 306,
     WRITE = 307,
     STORED = 308,
     DESIGNABLE = 309,
     SCRIPTABLE = 310,
     RESET = 311
   };
#endif
/* Tokens.  */
#define CHAR_VAL 258
#define INT_VAL 259
#define DOUBLE_VAL 260
#define STRING 261
#define IDENTIFIER 262
#define FRIEND 263
#define TYPEDEF 264
#define AUTO 265
#define REGISTER 266
#define STATIC 267
#define EXTERN 268
#define INLINE 269
#define VIRTUAL 270
#define CONST 271
#define VOLATILE 272
#define CHAR 273
#define SHORT 274
#define INT 275
#define LONG 276
#define SIGNED 277
#define UNSIGNED 278
#define FLOAT 279
#define DOUBLE 280
#define VOID 281
#define ENUM 282
#define CLASS 283
#define STRUCT 284
#define UNION 285
#define ASM 286
#define PRIVATE 287
#define PROTECTED 288
#define PUBLIC 289
#define OPERATOR 290
#define DBL_COLON 291
#define TRIPLE_DOT 292
#define TEMPLATE 293
#define NAMESPACE 294
#define USING 295
#define MUTABLE 296
#define THROW 297
#define SIGNALS 298
#define SLOTS 299
#define TQ_OBJECT 300
#define TQ_PROPERTY 301
#define TQ_OVERRIDE 302
#define TQ_CLASSINFO 303
#define TQ_ENUMS 304
#define TQ_SETS 305
#define READ 306
#define WRITE 307
#define STORED 308
#define DESIGNABLE 309
#define SCRIPTABLE 310
#define RESET 311




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 692 "moc.y"

    char	char_val;
    int		int_val;
    double	double_val;
    char       *string;
    Access	access;
    Function   *function;
    ArgList    *arg_list;
    Argument   *arg;



/* Line 2068 of yacc.c  */
#line 175 "moc_yacc.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


