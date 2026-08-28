/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 57 "moc.y"

#define MOC_YACC_CODE
void yyerror( const char *msg );

#include "qplatformdefs.h"
#include "ntqasciidict.h"
#include "ntqdatetime.h"
#include "ntqdict.h"
#include "ntqfile.h"
#include "ntqdir.h"
#include "ntqptrlist.h"
#include "ntqregexp.h"
#include "ntqstrlist.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#if defined CONST
#undef CONST
#endif
#if defined VOID
#undef VOID
#endif

bool isEnumType( const char* type );
int enumIndex( const char* type );
bool isVariantType( const char* type );
int qvariant_nameToType( const char* name );
static void init();				// initialize
static void initClass();			// prepare for new class
static void generateClass();			// generate C++ code for class
static void initExpression();			// prepare for new expression
static void enterNameSpace( const char *name = 0 );
static void leaveNameSpace();
static void selectOutsideClassState();
static void registerClassInNamespace();
static bool suppress_func_warn = false;
static void func_warn( const char *msg );
static void moc_warn( const char *msg );
static void moc_err( const char *s );
static void moc_err( const char *s1, const char *s2 );
static void operatorError();
static void checkPropertyName( const char* ident );

static const char* const utype_map[] =
{
    "bool",
    "int",
    "double",
    "TQString",
    "TQVariant",
    0
};

inline bool isIdentChar( char x )
{						// Avoid bug in isalnum
    return x == '_' || (x >= '0' && x <= '9') ||
	 (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z');
}

bool validUType( TQCString ctype )
{
     if ( ctype.left(6) == "const " )
	ctype = ctype.mid( 6, ctype.length() - 6 );
    if ( ctype.right(1) == "&" )
	ctype = ctype.left( ctype.length() - 1 );
    else if ( ctype.right(1) == "*" )
	return true;

    int i = -1;
    while ( utype_map[++i] )
	if ( ctype == utype_map[i] )
	    return true;

    return isEnumType( ctype );
}

TQCString castToUType( TQCString ctype )
{
     if ( ctype.right(1) == "&" )
	 ctype = ctype.left( ctype.length() - 1 );
     if( ctype.right(1) == "]") {
	 int lb = ctype.findRev('[');
	 if(lb != -1)
	     ctype = ctype.left(lb) + "*";
     }
     return ctype;
}

TQCString rawUType( TQCString ctype )
{
    ctype = castToUType( ctype );
    if ( ctype.left(6) == "const " )
	ctype = ctype.mid( 6, ctype.length() - 6 );
    return ctype;
}

TQCString uType( TQCString ctype )
{
    if ( !validUType( ctype ) ) {
	if ( isVariantType( rawUType(ctype) ) )
	    return "varptr";
	else
	    return "ptr";
    }
    if ( ctype.left(6) == "const " )
	ctype = ctype.mid( 6, ctype.length() - 6 );
    if ( ctype.right(1) == "&" ) {
	ctype = ctype.left( ctype.length() - 1 );
    } else if ( ctype.right(1) == "*" ) {
	TQCString raw = ctype.left( ctype.length() - 1 );
	ctype = "ptr";
	if ( raw == "char" )
	    ctype = "charstar";
	else if ( raw == "TQUnknownInterface" )
	    ctype = "iface";
	else if ( raw == "TQDispatchInterface" )
	    ctype = "idisp";
	else if ( isVariantType( raw ) )
	    ctype = "varptr";
    }
    if ( isEnumType( ctype ) )
	ctype = "enum";
    return ctype;
}

bool isInOut( TQCString ctype )
{
    if ( ctype.left(6) == "const " )
	return false;
    if ( ctype.right(1) == "&" )
	return true;
    if ( ctype.right(2) == "**" )
	return true;
    return false;
}

TQCString uTypeExtra( TQCString ctype )
{
    TQCString typeExtra = "0";
    if ( !validUType( ctype ) ) {
	if ( isVariantType( rawUType(ctype) ) )
	    typeExtra.sprintf("\"\\x%02x\"", qvariant_nameToType( rawUType(ctype) ) );
	else
	    typeExtra.sprintf( "\"%s\"", rawUType(ctype).data() );
	return typeExtra;
    }
    if ( ctype.left(6) == "const " )
	ctype = ctype.mid( 6, ctype.length() - 6 );
    if ( ctype.right(1) == "&" )
	ctype = ctype.left( ctype.length() - 1 );
    if ( ctype.right(1) == "*" ) {
	TQCString raw = ctype.left( ctype.length() - 1 );
	ctype = "ptr";
	if ( raw == "char" )
	    ;
	else if ( isVariantType( raw ) )
	    typeExtra.sprintf("\"\\x%02x\"", qvariant_nameToType( raw ) );
	else
	    typeExtra.sprintf( "\"%s\"", raw.stripWhiteSpace().data() );

    } else if ( isEnumType( ctype ) ) {
	int idx = enumIndex( ctype );
	if ( idx >= 0 ) {
	    typeExtra.sprintf( "&enum_tbl[%d]", enumIndex( ctype ) );
	} else {
	    typeExtra.sprintf( "parentObject->enumerator(\"%s\", true )", ctype.data() );
	}
	typeExtra =
	    "\n#ifndef TQT_NO_PROPERTIES\n\t  " + typeExtra +
	    "\n#else"
	    "\n\t  0"
	    "\n#endif // TQT_NO_PROPERTIES\n\t  ";
    }
    return typeExtra;
}

/*
  Attention!
  This table is copied from qvariant.cpp. If you change
  one, change both.
*/
static const int ntypes = 35;
static const char* const type_map[ntypes] =
{
    0,
    "TQMap<TQString,TQVariant>",
    "TQValueList<TQVariant>",
    "TQString",
    "TQStringList",
    "TQFont",
    "TQPixmap",
    "TQBrush",
    "TQRect",
    "TQSize",
    "TQColor",
    "TQPalette",
    "TQColorGroup",
    "TQIconSet",
    "TQPoint",
    "TQImage",
    "int",
    "uint",
    "bool",
    "double",
    "TQCString",
    "TQPointArray",
    "TQRegion",
    "TQBitmap",
    "TQCursor",
    "TQSizePolicy",
    "TQDate",
    "TQTime",
    "TQDateTime",
    "TQByteArray",
    "TQBitArray",
    "TQKeySequence",
    "TQPen",
    "TQ_LLONG",
    "TQ_ULLONG"
};

int qvariant_nameToType( const char* name )
{
    for ( int i = 0; i < ntypes; i++ ) {
	if ( !qstrcmp( type_map[i], name ) )
	    return i;
    }
    return 0;
}

/*
  Returns true if the type is a TQVariant types.
*/
bool isVariantType( const char* type )
{
    return qvariant_nameToType( type ) != 0;
}

/*
  Replaces '>>' with '> >' (as in 'TQValueList<TQValueList<double> >').
  This function must be called to produce valid C++ code. However,
  the string representation still uses '>>'.
*/
void fixRightAngles( TQCString *str )
{
    str->replace( TQRegExp(">>"), "> >" );
}

static TQCString rmWS( const char * );

enum Access { Private, Protected, Public };


class Argument					// single arg meta data
{
public:
    Argument( const char *left, const char *right, const char* argName = 0, bool isDefaultArgument = false )
    {
	leftType = rmWS( left );
	rightType = rmWS( right );
	if ( leftType == "void" && rightType.isEmpty() )
	    leftType = "";

	int len = leftType.length();

	/*
	  Convert 'char const *' into 'const char *'. Start at index 1,
	  not 0, because 'const char *' is already OK.
	*/
	for ( int i = 1; i < len; i++ ) {
	    if ( leftType[i] == 'c' &&
		 strncmp(leftType.data() + i + 1, "onst", 4) == 0
                 && (i + 5 >= len || !isIdentChar(leftType[i + 5]))
                 && !isIdentChar(i-1)
                ) {
		leftType.remove( i, 5 );
		if ( leftType[i - 1] == ' ' )
		    leftType.remove( i - 1, 1 );
		leftType.prepend( "const " );
		break;
	    }

	    /*
	      We musn't convert 'char * const *' into 'const char **'
	      and we must beware of 'Bar<const Bla>'.
	    */
	    if ( leftType[i] == '&' || leftType[i] == '*' ||
		 leftType[i] == '<' )
		break;
	}

	name = argName;
	isDefault = isDefaultArgument;
    }

    TQCString leftType;
    TQCString rightType;
    TQCString name;
    bool isDefault;
};

class ArgList : public TQPtrList<Argument> {	// member function arg list
public:
    ArgList() { setAutoDelete( true ); }
    ~ArgList() { clear(); }

    /* the clone has one default argument less, the orignal has all default arguments removed */
    ArgList* magicClone() {
	ArgList* l = new ArgList;
	bool firstDefault = false;
	for ( first(); current(); next() ) {
	    bool isDefault = current()->isDefault;
	    if ( !firstDefault && isDefault ) {
		isDefault = false;
		firstDefault = true;
	    }
	    l->append( new Argument( current()->leftType, current()->rightType, current()->name, isDefault ) );
	}
	for ( first(); current(); ) {
	    if ( current()->isDefault )
		remove();
	    else
		next();
	}
	return l;
    }

    bool hasDefaultArguments() {
	for ( Argument* a = first(); a; a = next() ) {
	    if ( a->isDefault )
		return true;
	}
	return false;
    }

};


struct Function					// member function meta data
{
    Access access;
    TQCString    qualifier;			// const or volatile
    TQCString    name;
    TQCString    type;
    TQCString    signature;
    int	       lineNo;
    ArgList   *args;
    Function() { args=0;}
   ~Function() { delete args; }
    const char* accessAsString() {
	switch ( access ) {
	case Private: return "Private";
	case Protected: return "Protected";
	default: return "Public";
	}
    }
};

class FuncList : public TQPtrList<Function> {	// list of member functions
public:
    FuncList( bool autoDelete = false ) { setAutoDelete( autoDelete ); }

    FuncList find( const char* name )
    {
	FuncList result;
	for ( TQPtrListIterator<Function> it(*this); it.current(); ++it ) {
	    if ( it.current()->name == name )
		result.append( it.current() );
	}
	return result;
    }
};

class Enum : public TQStrList
{
public:
    TQCString name;
    bool set;
};

class EnumList : public TQPtrList<Enum> {		// list of property enums
public:
    EnumList() { setAutoDelete(true); }
};


struct Property
{
    Property( int l, const char* t, const char* n, const char* s, const char* g, const char* r,
	      const TQCString& st, const TQCString& d, const TQCString& sc, bool ov )
	: lineNo(l), type(t), name(n), set(s), get(g), reset(r), setfunc(0), getfunc(0),
	  sspec(Unspecified), gspec(Unspecified), stored( st ),
	  designable( d ), scriptable( sc ), override( ov ), oredEnum( -1 )
    {
	/*
	  The TQ_PROPERTY construct cannot contain any commas, since
	  commas separate macro arguments. We therefore expect users
	  to type "TQMap" instead of "TQMap<TQString, TQVariant>". For
	  coherence, we also expect the same for
	  TQValueList<TQVariant>, the other template class supported by
	  TQVariant.
	*/
	if ( type == "TQMap" ) {
	    type = "TQMap<TQString,TQVariant>";
	} else if ( type == "TQValueList" ) {
	    type = "TQValueList<TQVariant>";
	} else if ( type == "LongLong" ) {
	    type = "TQ_LLONG";
	} else if ( type == "ULongLong" ) {
	    type = "TQ_ULLONG";
	}
    }

    int lineNo;
    TQCString type;
    TQCString name;
    TQCString set;
    TQCString get;
    TQCString reset;
    TQCString stored;
    TQCString designable;
    TQCString scriptable;
    bool override;

    Function* setfunc;
    Function* getfunc;

    int oredEnum; // If the enums item may be ored. That means the data type is int.
		  // Allowed values are 1 (True), 0 (False), and -1 (Unset)
    TQCString enumsettype; // contains the set function type in case of oredEnum
    TQCString enumgettype; // contains the get function type in case of oredEnum

    enum Specification  { Unspecified, Class, Reference, Pointer, ConstCharStar };
    Specification sspec;
    Specification gspec;

    bool stdSet() {
	TQCString s = "set";
	s += toupper( name[0] );
	s += name.mid( 1 );
	return s == set;
    }

    static const char* specToString( Specification s )
    {
	switch ( s ) {
	case Class:
	    return "Class";
	case Reference:
	    return "Reference";
	case Pointer:
	    return "Pointer";
	case ConstCharStar:
	    return "ConstCharStar";
	default:
	    return "Unspecified";
	}
    }
};

class PropList : public TQPtrList<Property> {	// list of properties
public:
    PropList() { setAutoDelete( true ); }
};


struct ClassInfo
{
    ClassInfo( const char* n, const char* v )
	: name(n), value(v)
    {}
    TQCString name;
    TQCString value;
};

class ClassInfoList : public TQPtrList<ClassInfo> {	// list of class infos
public:
    ClassInfoList() { setAutoDelete( true ); }
};

class parser_reg {
 public:
    parser_reg();
    ~parser_reg();

    // some temporary values
    TQCString   tmpExpression;			// Used to store the characters the lexer
						// is currently skipping (see addExpressionChar and friends)
    TQCString  fileName;				// file name
    TQCString  outputFile;				// output file name
    TQCString  pchFile;				// name of PCH file (used on Windows)
    TQStrList  includeFiles;			// name of #include files
    TQCString  includePath;				// #include file path
    TQCString  qtPath;				// #include qt file path
    int           gen_count; //number of classes generated
    bool	  noInclude;		// no #include <filename>
    bool	  generatedCode;		// no code generated
    bool	  mocError;			// moc parsing error occurred
    bool       hasVariantIncluded;	//whether or not ntqvariant.h was included yet
    TQCString  className;				// name of parsed class
    TQCString  superClassName;			// name of first super class
    TQStrList  multipleSuperClasses;			// other superclasses
    FuncList  signals;				// signal interface
    FuncList  slots;				// slots interface
    FuncList  propfuncs;				// all possible property access functions
    FuncList  funcs;			// all parsed functions, including signals
    EnumList  enums;				// enums used in properties
    PropList  props;				// list of all properties
    ClassInfoList	infos;				// list of all class infos

// Used to store the values in the TQ_PROPERTY macro
    TQCString propWrite;				// set function
    TQCString propRead;				// get function
    TQCString propReset;				// reset function
    TQCString propStored;				//
    TQCString propDesignable;				// "true", "false" or function or empty if not specified
    TQCString propScriptable;				// "true", "false" or function or empty if not specified
    bool propOverride;				// Wether OVERRIDE was detected

    TQStrList qtEnums;				// Used to store the contents of TQ_ENUMS
    TQStrList qtSets;				// Used to store the contents of TQ_SETS

};

static parser_reg *g = 0;

ArgList *addArg( Argument * );			// add arg to tmpArgList

enum Member { SignalMember,
	      SlotMember,
	      PropertyCandidateMember
	    };

void	 addMember( Member );			// add tmpFunc to current class
void     addEnum();				// add tmpEnum to current class

char	*stradd( const char *, const char * );	// add two strings
char	*stradd( const char *, const char *,	// add three strings
			       const char * );
char 	*stradd( const char *, const char *,	// adds 4 strings
		 const char *, const char * );

char	*straddSpc( const char *, const char * );
char	*straddSpc( const char *, const char *,
			       const char * );
char	*straddSpc( const char *, const char *,
		    const char *, const char * );

extern int yydebug;
bool	   lexDebug	   = false;
int	   lineNo;			// current line number
bool	   errorControl	   = false;	// controled errors
bool	   displayWarnings = true;
bool	   skipClass;			// don't generate for class
bool	   skipFunc;			// don't generate for func
bool	   templateClass;		// class is a template
bool	   templateClassOld;		// previous class is a template

ArgList	  *tmpArgList;			// current argument list
Function  *tmpFunc;			// current member function
Enum      *tmpEnum;			// current enum
Access tmpAccess;			// current access permission
Access subClassPerm;			// current access permission

bool	   TQ_OBJECTdetected;		// true if current class
					//  contains the TQ_OBJECT macro
bool	   TQ_PROPERTYdetected;		// true if current class
					//  contains at least one TQ_PROPERTY,
					//  TQ_OVERRIDE, TQ_SETS or TQ_ENUMS macro
bool	   tmpPropOverride;		// current property override setting

int	   tmpYYStart;			// Used to store the lexers current mode
int	   tmpYYStart2;			// Used to store the lexers current mode
					//  (if tmpYYStart is already used)

// if the format revision changes, you MUST change it in ntqmetaobject.h too
const int formatRevision = 26;		// moc output format revision

// if the flags change, you HAVE to change it in ntqmetaobject.h too
enum Flags  {
    Invalid		= 0x00000000,
    Readable		= 0x00000001,
    Writable		= 0x00000002,
    EnumOrSet		= 0x00000004,
    UnresolvedEnum	= 0x00000008,
    StdSet		= 0x00000100,
    Override		= 0x00000200,
    NotDesignable	= 0x00001000,
    DesignableOverride  = 0x00002000,
    NotScriptable	= 0x00004000,
    ScriptableOverride  = 0x00008000,
    NotStored 		= 0x00010000,
    StoredOverride 	= 0x00020000
};


#ifdef YYBISON
# if defined(Q_OS_WIN32)
# include <io.h>
# undef isatty
extern "C" int hack_isatty( int )
 {
     return 0;
 }
# define isatty hack_isatty
# else
# include <unistd.h>
# endif

# define YYDEBUG 1
# include "moc_yacc.h"
# include "moc_lex.cpp"
#endif //YYBISON


/* Line 268 of yacc.c  */
#line 704 "moc_yacc"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 293 of yacc.c  */
#line 692 "moc.y"

    char	char_val;
    int		int_val;
    double	double_val;
    char       *string;
    Access	access;
    Function   *function;
    ArgList    *arg_list;
    Argument   *arg;



/* Line 293 of yacc.c  */
#line 865 "moc_yacc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 877 "moc_yacc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   612

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  79
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  140
/* YYNRULES -- Number of rules.  */
#define YYNRULES  329
/* YYNRULES -- Number of states.  */
#define YYNSTATES  492

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   311

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    78,     2,     2,     2,    74,    69,     2,
      66,    67,    68,    71,    63,    72,     2,    73,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    70,    60,
      61,    59,    62,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    64,     2,    65,    75,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    57,    76,    58,    77,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    17,
      19,    21,    22,    23,    31,    32,    33,    40,    42,    48,
      51,    54,    57,    58,    62,    64,    66,    71,    72,    73,
      74,    75,    77,    79,    81,    83,    85,    89,    90,    92,
      94,    97,    99,   101,   103,   105,   107,   109,   111,   113,
     115,   117,   119,   122,   124,   126,   128,   130,   132,   134,
     136,   138,   140,   142,   147,   148,   150,   153,   156,   158,
     161,   165,   167,   170,   173,   176,   179,   183,   184,   186,
     187,   189,   190,   192,   196,   198,   201,   202,   208,   213,
     214,   222,   223,   225,   228,   229,   234,   235,   241,   243,
     247,   249,   252,   253,   259,   263,   265,   274,   276,   279,
     280,   285,   286,   292,   293,   298,   299,   305,   306,   308,
     310,   313,   316,   319,   320,   322,   324,   327,   329,   331,
     333,   335,   339,   340,   344,   345,   351,   353,   357,   361,
     366,   369,   371,   373,   375,   377,   379,   382,   386,   389,
     393,   394,   396,   401,   402,   404,   407,   409,   413,   414,
     420,   421,   423,   424,   426,   429,   431,   433,   435,   436,
     440,   441,   446,   448,   449,   450,   458,   459,   460,   468,
     469,   470,   480,   481,   482,   490,   491,   492,   500,   503,
     507,   508,   512,   514,   515,   517,   520,   522,   524,   525,
     527,   530,   532,   534,   535,   537,   540,   542,   544,   545,
     548,   551,   555,   557,   562,   567,   569,   573,   576,   580,
     583,   585,   589,   592,   596,   599,   601,   603,   605,   609,
     613,   615,   617,   619,   621,   623,   625,   627,   629,   631,
     633,   635,   637,   639,   642,   645,   648,   651,   654,   657,
     660,   663,   666,   669,   672,   675,   678,   681,   684,   688,
     692,   695,   698,   701,   704,   706,   710,   713,   716,   719,
     720,   722,   725,   727,   731,   737,   740,   744,   749,   753,
     756,   763,   768,   774,   778,   783,   790,   795,   800,   806,
     810,   814,   815,   816,   824,   826,   830,   832,   833,   838,
     839,   843,   844,   845,   849,   852,   853,   855,   861,   866,
     867,   869,   870,   872,   876,   878,   879,   884,   885,   890,
     891,   895,   899,   903,   907,   911,   915,   916,   919,   920
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      80,     0,    -1,    -1,    80,    81,    -1,    93,    -1,    82,
      -1,    90,    -1,    92,    -1,    91,    -1,    83,    -1,    86,
      -1,    -1,    -1,    39,     7,    84,    57,    85,    89,    58,
      -1,    -1,    -1,    39,    87,    57,    88,    89,    58,    -1,
      80,    -1,    39,     7,    59,   114,    60,    -1,    40,    39,
      -1,    40,     7,    -1,    40,    36,    -1,    -1,    94,   148,
      60,    -1,     7,    -1,    96,    -1,     7,    61,    97,    62,
      -1,    -1,    -1,    -1,    -1,   105,    -1,   107,    -1,   106,
      -1,     8,    -1,     9,    -1,   103,   108,   103,    -1,    -1,
     104,    -1,   101,    -1,   104,   101,    -1,    10,    -1,    11,
      -1,    12,    -1,    13,    -1,    14,    -1,    15,    -1,    16,
      -1,    17,    -1,   116,    -1,   114,    -1,   109,    -1,   109,
     110,    -1,   110,    -1,    18,    -1,    19,    -1,    20,    -1,
      21,    -1,    22,    -1,    23,    -1,    24,    -1,    25,    -1,
      26,    -1,    38,    61,    97,    62,    -1,    -1,   111,    -1,
     112,    28,    -1,   112,    29,    -1,   115,    -1,    36,   115,
      -1,   115,    36,    95,    -1,    95,    -1,   113,     7,    -1,
      27,     7,    -1,    30,     7,    -1,   118,   120,    -1,   121,
      63,    37,    -1,    -1,   121,    -1,    -1,   122,    -1,    -1,
      37,    -1,   121,    63,   122,    -1,   122,    -1,   102,   125,
      -1,    -1,   102,   125,    59,   123,    99,    -1,   102,   125,
     131,   125,    -1,    -1,   102,   125,   131,   125,    59,   124,
      99,    -1,    -1,   126,    -1,   126,   141,    -1,    -1,    64,
     127,    98,    65,    -1,    -1,   126,    64,   128,    98,    65,
      -1,   141,    -1,    66,   126,    67,    -1,   131,    -1,   129,
     141,    -1,    -1,   129,    64,   130,    98,    65,    -1,    66,
     129,    67,    -1,     7,    -1,    66,   117,    67,   142,   156,
     154,   210,   145,    -1,     7,    -1,     7,   136,    -1,    -1,
       7,    59,   134,    98,    -1,    -1,     7,   136,    59,   135,
      98,    -1,    -1,    64,   137,    98,    65,    -1,    -1,   136,
      64,   138,    98,    65,    -1,    -1,   140,    -1,   141,    -1,
     140,   141,    -1,    68,   142,    -1,    69,   142,    -1,    -1,
     143,    -1,   144,    -1,   143,   144,    -1,    16,    -1,    17,
      -1,    60,    -1,   146,    -1,    59,     4,    60,    -1,    -1,
      57,   147,    58,    -1,    -1,   152,    57,   149,   162,    58,
      -1,   151,    -1,   151,    68,     7,    -1,   151,    69,     7,
      -1,   151,    66,     7,    67,    -1,   111,   150,    -1,     7,
      -1,   110,    -1,   107,    -1,   105,    -1,   106,    -1,   113,
     115,    -1,   113,     7,    95,    -1,   151,   161,    -1,   113,
     115,   161,    -1,    -1,   155,    -1,    42,    66,   119,    67,
      -1,    -1,   157,    -1,    70,   158,    -1,   159,    -1,   159,
      63,   158,    -1,    -1,   114,    66,   160,    98,    67,    -1,
      -1,   190,    -1,    -1,   163,    -1,   163,   165,    -1,   165,
      -1,   194,    -1,    44,    -1,    -1,   164,   166,   178,    -1,
      -1,    43,   167,    70,   183,    -1,    45,    -1,    -1,    -1,
      46,   168,    66,   214,    67,   169,   180,    -1,    -1,    -1,
      47,   170,    66,   214,    67,   171,   180,    -1,    -1,    -1,
      48,   172,    66,     6,    63,     6,    67,   173,   180,    -1,
      -1,    -1,    49,   174,    66,   217,    67,   175,   180,    -1,
      -1,    -1,    50,   176,    66,   218,    67,   177,   180,    -1,
      43,    70,    -1,    44,    70,   186,    -1,    -1,    70,   179,
     180,    -1,     7,    -1,    -1,   181,    -1,   181,   182,    -1,
     182,    -1,   198,    -1,    -1,   184,    -1,   184,   185,    -1,
     185,    -1,   198,    -1,    -1,   187,    -1,   187,   188,    -1,
     188,    -1,   198,    -1,    -1,   189,    60,    -1,    70,   191,
      -1,   191,    63,   193,    -1,   193,    -1,     7,    66,     7,
      67,    -1,     7,    66,   110,    67,    -1,   114,    -1,    15,
     194,   114,    -1,    15,   114,    -1,   194,    15,   114,    -1,
     194,   114,    -1,   192,    -1,    15,   194,   192,    -1,    15,
     192,    -1,   194,    15,   192,    -1,   194,   192,    -1,    32,
      -1,    33,    -1,    34,    -1,   103,     7,   139,    -1,   103,
     110,   139,    -1,    71,    -1,    72,    -1,    68,    -1,    73,
      -1,    74,    -1,    75,    -1,    69,    -1,    76,    -1,    77,
      -1,    78,    -1,    59,    -1,    61,    -1,    62,    -1,    71,
      59,    -1,    72,    59,    -1,    68,    59,    -1,    73,    59,
      -1,    74,    59,    -1,    75,    59,    -1,    69,    59,    -1,
      76,    59,    -1,    77,    59,    -1,    78,    59,    -1,    59,
      59,    -1,    61,    59,    -1,    62,    59,    -1,    61,    61,
      -1,    62,    62,    -1,    61,    61,    59,    -1,    62,    62,
      59,    -1,    69,    69,    -1,    76,    76,    -1,    71,    71,
      -1,    72,    72,    -1,    63,    -1,    72,    62,    68,    -1,
      72,    62,    -1,    66,    67,    -1,    64,    65,    -1,    -1,
      15,    -1,   108,   133,    -1,   133,    -1,   196,    77,   133,
      -1,   104,   108,   103,   139,   133,    -1,   104,   108,    -1,
     108,   140,   133,    -1,   108,   104,   139,   133,    -1,   108,
      35,   195,    -1,    35,   195,    -1,   104,   108,   103,   139,
      35,   195,    -1,   108,   140,    35,   195,    -1,   108,   104,
     139,    35,   195,    -1,   197,   132,   189,    -1,   197,   205,
      60,   189,    -1,   197,   205,    63,   201,    60,   189,    -1,
     207,   210,    60,   189,    -1,    40,   114,    60,   189,    -1,
      40,    39,   114,    60,   189,    -1,    39,     7,    57,    -1,
     153,    60,   189,    -1,    -1,    -1,   153,    57,   199,    58,
     200,    60,   189,    -1,   202,    -1,   201,    63,   202,    -1,
     129,    -1,    -1,     7,    70,   203,    98,    -1,    -1,    70,
     204,    98,    -1,    -1,    -1,    70,   206,    98,    -1,    27,
     209,    -1,    -1,    63,    -1,     7,    57,   211,   208,    58,
      -1,    57,   211,   208,    58,    -1,    -1,     7,    -1,    -1,
     212,    -1,   211,    63,   212,    -1,     7,    -1,    -1,     7,
      59,   213,   100,    -1,    -1,     7,     7,   215,   216,    -1,
      -1,    51,     7,   216,    -1,    52,     7,   216,    -1,    56,
       7,   216,    -1,    53,     7,   216,    -1,    54,     7,   216,
      -1,    55,     7,   216,    -1,    -1,     7,   217,    -1,    -1,
       7,   218,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   807,   807,   808,   811,   813,   814,   815,   816,   819,
     820,   824,   825,   823,   832,   833,   832,   840,   843,   848,
     851,   852,   855,   855,   864,   865,   868,   880,   893,   902,
     905,   911,   912,   913,   914,   915,   918,   921,   922,   925,
     926,   929,   930,   931,   932,   935,   936,   939,   940,   943,
     944,   945,   948,   950,   953,   954,   955,   956,   957,   958,
     959,   960,   961,   964,   970,   971,   977,   978,   981,   982,
     986,   988,   992,   993,   994,   999,  1000,  1006,  1007,  1010,
    1011,  1014,  1015,  1021,  1024,  1027,  1030,  1029,  1033,  1038,
    1036,  1044,  1045,  1048,  1050,  1050,  1055,  1055,  1060,  1061,
    1064,  1065,  1067,  1067,  1072,  1075,  1078,  1090,  1091,  1093,
    1093,  1096,  1096,  1102,  1102,  1104,  1104,  1109,  1110,  1113,
    1114,  1117,  1118,  1125,  1126,  1129,  1130,  1134,  1135,  1138,
    1139,  1140,  1143,  1143,  1151,  1150,  1156,  1158,  1160,  1162,
    1166,  1170,  1171,  1172,  1173,  1174,  1178,  1183,  1191,  1195,
    1200,  1201,  1205,  1208,  1209,  1212,  1215,  1216,  1220,  1220,
    1225,  1226,  1229,  1230,  1233,  1234,  1238,  1239,  1243,  1243,
    1245,  1245,  1247,  1255,  1258,  1255,  1262,  1265,  1262,  1269,
    1271,  1269,  1276,  1277,  1276,  1282,  1283,  1282,  1290,  1292,
    1293,  1293,  1303,  1310,  1311,  1314,  1315,  1318,  1321,  1322,
    1325,  1326,  1330,  1333,  1334,  1337,  1338,  1341,  1344,  1345,
    1348,  1351,  1352,  1355,  1357,  1361,  1362,  1363,  1364,  1365,
    1366,  1367,  1368,  1369,  1370,  1373,  1374,  1375,  1378,  1379,
    1380,  1381,  1382,  1383,  1384,  1385,  1386,  1387,  1388,  1389,
    1390,  1391,  1392,  1393,  1394,  1395,  1396,  1397,  1398,  1399,
    1400,  1401,  1402,  1403,  1404,  1405,  1406,  1407,  1408,  1409,
    1410,  1411,  1412,  1413,  1414,  1415,  1416,  1417,  1418,  1422,
    1423,  1426,  1429,  1436,  1443,  1451,  1453,  1457,  1462,  1464,
    1466,  1469,  1471,  1477,  1478,  1480,  1483,  1485,  1487,  1489,
    1492,  1495,  1498,  1494,  1503,  1504,  1507,  1508,  1508,  1510,
    1510,  1514,  1515,  1515,  1520,  1525,  1526,  1529,  1536,  1540,
    1541,  1544,  1545,  1546,  1549,  1550,  1550,  1555,  1554,  1591,
    1592,  1593,  1594,  1595,  1596,  1597,  1600,  1601,  1604,  1605
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CHAR_VAL", "INT_VAL", "DOUBLE_VAL",
  "STRING", "IDENTIFIER", "FRIEND", "TYPEDEF", "AUTO", "REGISTER",
  "STATIC", "EXTERN", "INLINE", "VIRTUAL", "CONST", "VOLATILE", "CHAR",
  "SHORT", "INT", "LONG", "SIGNED", "UNSIGNED", "FLOAT", "DOUBLE", "VOID",
  "ENUM", "CLASS", "STRUCT", "UNION", "ASM", "PRIVATE", "PROTECTED",
  "PUBLIC", "OPERATOR", "DBL_COLON", "TRIPLE_DOT", "TEMPLATE", "NAMESPACE",
  "USING", "MUTABLE", "THROW", "SIGNALS", "SLOTS", "TQ_OBJECT",
  "TQ_PROPERTY", "TQ_OVERRIDE", "TQ_CLASSINFO", "TQ_ENUMS", "TQ_SETS",
  "READ", "WRITE", "STORED", "DESIGNABLE", "SCRIPTABLE", "RESET", "'{'",
  "'}'", "'='", "';'", "'<'", "'>'", "','", "'['", "']'", "'('", "')'",
  "'*'", "'&'", "':'", "'+'", "'-'", "'/'", "'%'", "'^'", "'|'", "'~'",
  "'!'", "$accept", "declaration_seq", "declaration", "namespace_def",
  "named_namespace_def", "$@1", "$@2", "unnamed_namespace_def", "$@3",
  "$@4", "namespace_body", "namespace_alias_def", "using_directive",
  "using_declaration", "class_def", "$@5", "class_name",
  "template_class_name", "template_args", "const_expression",
  "def_argument", "enumerator_expression", "decl_specifier",
  "decl_specifiers", "decl_specs_opt", "decl_specs",
  "storage_class_specifier", "fct_specifier", "type_specifier",
  "type_name", "simple_type_names", "simple_type_name", "template_spec",
  "opt_template_spec", "class_key", "complete_class_name",
  "qualified_class_name", "elaborated_type_specifier",
  "argument_declaration_list", "arg_declaration_list_opt",
  "opt_exception_argument", "triple_dot_opt", "arg_declaration_list",
  "argument_declaration", "$@6", "$@7", "abstract_decl_opt",
  "abstract_decl", "$@8", "$@9", "declarator", "$@10", "dname", "fct_decl",
  "fct_name", "$@11", "$@12", "array_decls", "$@13", "$@14",
  "ptr_operators_opt", "ptr_operators", "ptr_operator",
  "cv_qualifier_list_opt", "cv_qualifier_list", "cv_qualifier",
  "fct_body_or_semicolon", "fct_body", "$@15", "class_specifier", "$@16",
  "whatever", "class_head", "full_class_head", "nested_class_head",
  "exception_spec_opt", "exception_spec", "ctor_initializer_opt",
  "ctor_initializer", "mem_initializer_list", "mem_initializer", "$@17",
  "opt_base_spec", "opt_obj_member_list", "obj_member_list",
  "qt_access_specifier", "obj_member_area", "$@18", "$@19", "$@20", "$@21",
  "$@22", "$@23", "$@24", "$@25", "$@26", "$@27", "$@28", "$@29",
  "slot_area", "$@30", "opt_property_candidates",
  "property_candidate_declarations", "property_candidate_declaration",
  "opt_signal_declarations", "signal_declarations", "signal_declaration",
  "opt_slot_declarations", "slot_declarations", "slot_declaration",
  "opt_semicolons", "base_spec", "base_list", "qt_macro_name",
  "base_specifier", "access_specifier", "operator_name", "opt_virtual",
  "type_and_name", "signal_or_slot", "$@31", "$@32",
  "member_declarator_list", "member_declarator", "$@33", "$@34",
  "opt_bitfield", "$@35", "enum_specifier", "opt_komma", "enum_tail",
  "opt_identifier", "enum_list", "enumerator", "$@36", "property", "$@37",
  "prop_statements", "qt_enums", "qt_sets", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   123,   125,    61,
      59,    60,    62,    44,    91,    93,    40,    41,    42,    38,
      58,    43,    45,    47,    37,    94,   124,   126,    33
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    79,    80,    80,    81,    81,    81,    81,    81,    82,
      82,    84,    85,    83,    87,    88,    86,    89,    90,    91,
      92,    92,    94,    93,    95,    95,    96,    97,    98,    99,
     100,   101,   101,   101,   101,   101,   102,   103,   103,   104,
     104,   105,   105,   105,   105,   106,   106,   107,   107,   108,
     108,   108,   109,   109,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   111,   112,   112,   113,   113,   114,   114,
     115,   115,   116,   116,   116,   117,   117,   118,   118,   119,
     119,   120,   120,   121,   121,   122,   123,   122,   122,   124,
     122,   125,   125,   126,   127,   126,   128,   126,   126,   126,
     129,   129,   130,   129,   129,   131,   132,   133,   133,   134,
     133,   135,   133,   137,   136,   138,   136,   139,   139,   140,
     140,   141,   141,   142,   142,   143,   143,   144,   144,   145,
     145,   145,   147,   146,   149,   148,   148,   148,   148,   148,
     148,   150,   150,   150,   150,   150,   151,   151,   152,   153,
     154,   154,   155,   156,   156,   157,   158,   158,   160,   159,
     161,   161,   162,   162,   163,   163,   164,   164,   166,   165,
     167,   165,   165,   168,   169,   165,   170,   171,   165,   172,
     173,   165,   174,   175,   165,   176,   177,   165,   178,   178,
     179,   178,   178,   180,   180,   181,   181,   182,   183,   183,
     184,   184,   185,   186,   186,   187,   187,   188,   189,   189,
     190,   191,   191,   192,   192,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   194,   194,   194,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   196,
     196,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   198,   198,   198,   198,   198,   198,   198,
     198,   199,   200,   198,   201,   201,   202,   203,   202,   204,
     202,   205,   206,   205,   207,   208,   208,   209,   209,   210,
     210,   211,   211,   211,   212,   213,   212,   215,   214,   216,
     216,   216,   216,   216,   216,   216,   217,   217,   218,   218
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     0,     0,     7,     0,     0,     6,     1,     5,     2,
       2,     2,     0,     3,     1,     1,     4,     0,     0,     0,
       0,     1,     1,     1,     1,     1,     3,     0,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     0,     1,     2,     2,     1,     2,
       3,     1,     2,     2,     2,     2,     3,     0,     1,     0,
       1,     0,     1,     3,     1,     2,     0,     5,     4,     0,
       7,     0,     1,     2,     0,     4,     0,     5,     1,     3,
       1,     2,     0,     5,     3,     1,     8,     1,     2,     0,
       4,     0,     5,     0,     4,     0,     5,     0,     1,     1,
       2,     2,     2,     0,     1,     1,     2,     1,     1,     1,
       1,     3,     0,     3,     0,     5,     1,     3,     3,     4,
       2,     1,     1,     1,     1,     1,     2,     3,     2,     3,
       0,     1,     4,     0,     1,     2,     1,     3,     0,     5,
       0,     1,     0,     1,     2,     1,     1,     1,     0,     3,
       0,     4,     1,     0,     0,     7,     0,     0,     7,     0,
       0,     9,     0,     0,     7,     0,     0,     7,     2,     3,
       0,     3,     1,     0,     1,     2,     1,     1,     0,     1,
       2,     1,     1,     0,     1,     2,     1,     1,     0,     2,
       2,     3,     1,     4,     4,     1,     3,     2,     3,     2,
       1,     3,     2,     3,     2,     1,     1,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     3,     3,
       2,     2,     2,     2,     1,     3,     2,     2,     2,     0,
       1,     2,     1,     3,     5,     2,     3,     4,     3,     2,
       6,     4,     5,     3,     4,     6,     4,     4,     5,     3,
       3,     0,     0,     7,     1,     3,     1,     0,     4,     0,
       3,     0,     0,     3,     2,     0,     1,     5,     4,     0,
       1,     0,     1,     3,     1,     0,     4,     0,     4,     0,
       3,     3,     3,     3,     3,     3,     0,     2,     0,     2
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,    22,     1,    14,     0,     3,     5,     9,    10,     6,
       8,     7,     4,    64,    11,     0,    20,    21,    19,     0,
      65,     0,     0,     0,   136,     0,     0,     0,    15,    27,
     141,    41,    42,    43,    44,    45,    46,    47,    48,    54,
      55,    56,    57,    58,    59,    60,    61,    62,   144,   145,
     143,   142,   140,    66,    67,    24,    71,    25,   146,    23,
       0,     0,     0,     0,   148,   161,   134,    24,     0,     0,
      68,    12,     2,     0,    27,   147,     0,     0,   137,   138,
      24,     0,   225,   226,   227,   215,   210,   220,   212,     0,
     162,    69,    18,     2,    22,     0,    63,     0,    70,   139,
       0,   217,   222,     0,     0,     0,   219,   224,   170,   167,
     172,   173,   176,   179,   182,   185,     0,   163,   168,   165,
     166,     0,    16,    26,     0,     0,   216,   221,   211,   218,
     223,     0,     0,     0,     0,     0,     0,   135,   164,     0,
      13,   213,   214,   198,     0,     0,     0,   326,   328,   192,
       0,     0,   190,   169,    24,    34,    35,    46,     0,     0,
      37,     0,     0,    39,    64,    31,    33,    32,     0,    51,
      53,    65,     0,    50,    49,   272,     0,   171,   199,   201,
       0,   301,   202,   309,     0,     0,     0,     0,   326,     0,
     328,     0,   188,   203,   193,   109,   113,   108,    73,   311,
     304,    74,   240,   241,   242,   264,     0,     0,   232,   236,
     230,   231,   233,   234,   235,   237,   238,   239,     0,    38,
     279,     0,     0,     0,     0,    40,    37,     0,   107,    37,
     123,   123,   117,   271,     0,   119,    52,    72,   160,   291,
     208,   200,     0,    37,   302,   208,     0,   310,     0,   317,
     174,   177,     0,   327,   183,   329,   186,   189,   204,   206,
     207,   191,   194,   196,   197,    28,    28,   111,   115,   311,
     314,   305,   312,   253,   254,   256,   255,   257,   268,   267,
     245,   249,   260,   243,   262,   244,   266,   263,   246,   247,
     248,   250,   261,   251,   252,   117,   117,   289,     0,   208,
      73,   117,    72,   278,   127,   128,   121,   124,   125,   122,
       0,   118,    37,   276,   120,   149,     0,   290,   273,    91,
      64,     0,    81,    78,    84,    28,   283,   208,     0,   208,
     319,   193,   193,     0,   193,   193,   205,   195,   110,     0,
      28,    28,   305,   315,   306,     0,   258,   259,   265,   228,
     229,   208,   287,     0,   126,    37,   277,   281,   292,   209,
      94,     0,    85,    92,    98,    37,   123,    82,    75,    37,
     303,   284,   105,     0,   299,   296,   100,     0,   294,   286,
       0,     0,     0,     0,     0,     0,   318,   175,   178,   180,
     184,   187,   114,   112,     0,     0,    30,   313,   308,   288,
      37,   274,   282,     0,    28,     0,   105,    86,    91,    96,
      93,    36,   153,    76,    83,   297,     0,    28,   102,   101,
     208,     0,   319,   319,   319,   319,   319,   319,   193,   116,
     307,   316,   280,   208,     0,    99,    29,    88,    28,     0,
     150,   154,    28,   104,   300,    28,   285,   295,   320,   321,
     323,   324,   325,   322,   181,   293,    95,    87,    89,     0,
       0,   155,   156,     0,   309,   151,   298,     0,    29,    97,
     158,     0,    37,     0,   103,    90,    28,   157,     0,    80,
     132,     0,   129,   106,   130,     0,   152,     0,     0,   159,
     133,   131
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    94,     5,     6,     7,    27,    93,     8,    15,    72,
      95,     9,    10,    11,    12,    13,    56,    57,    73,   338,
     457,   431,   163,   319,   218,   164,   165,   166,   167,   168,
     169,   170,   171,    21,   172,   173,    70,   174,   321,   322,
     478,   368,   323,   324,   436,   468,   362,   363,   404,   438,
     375,   445,   376,   245,   175,   265,   340,   197,   266,   341,
     310,   311,   235,   306,   307,   308,   483,   484,   487,    23,
      90,    52,    24,    25,   176,   464,   465,   440,   441,   461,
     462,   476,    64,   116,   117,   118,   119,   139,   131,   132,
     331,   133,   332,   134,   428,   135,   334,   136,   335,   153,
     194,   261,   262,   263,   177,   178,   179,   257,   258,   259,
     317,    65,    86,    87,    88,    89,   220,   180,   181,   264,
     316,   403,   377,   378,   442,   417,   246,   325,   183,   345,
     200,   248,   271,   272,   396,   185,   330,   386,   189,   191
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -356
static const yytype_int16 yypact[] =
{
    -356,    61,  -356,    35,    83,  -356,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,    12,    37,    -9,  -356,  -356,  -356,    21,
     469,   112,   100,    53,   118,    63,    36,    67,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,     9,  -356,  -356,   113,  -356,
     153,   155,   207,    99,  -356,  -356,  -356,   120,   231,   215,
     113,  -356,  -356,   218,  -356,  -356,   231,   209,  -356,  -356,
     -22,   176,  -356,  -356,  -356,  -356,   244,  -356,  -356,    20,
     493,   113,  -356,  -356,    -6,   247,  -356,   253,  -356,  -356,
     489,  -356,  -356,   101,    99,   101,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,  -356,   283,   493,  -356,  -356,
    -356,   284,  -356,  -356,   276,   285,  -356,  -356,  -356,  -356,
    -356,   281,   288,   289,   290,   292,   293,  -356,  -356,    23,
    -356,  -356,  -356,   243,   364,   364,   366,   369,   372,  -356,
     315,   316,  -356,  -356,   314,  -356,  -356,   306,    16,   380,
     156,   381,    87,  -356,   385,  -356,  -356,  -356,   277,   526,
    -356,  -356,   382,  -356,  -356,  -356,   138,  -356,   243,  -356,
     337,    73,  -356,   384,   409,   351,   352,   359,   369,   357,
     372,   358,  -356,   243,   243,  -356,  -356,    46,   370,   419,
    -356,  -356,   379,    97,   164,  -356,   374,   375,   388,    43,
     -13,   -21,   416,   418,   438,   -31,   439,   440,   498,   451,
    -356,   386,    36,   442,   434,  -356,   287,   437,    91,   156,
     174,   174,   313,  -356,    29,  -356,  -356,    68,   -17,  -356,
    -356,  -356,   494,   323,  -356,  -356,   214,  -356,   443,  -356,
    -356,  -356,   500,  -356,  -356,  -356,  -356,  -356,   243,  -356,
    -356,  -356,   243,  -356,  -356,  -356,  -356,  -356,  -356,   419,
     445,   465,  -356,  -356,  -356,   470,  -356,   471,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,  -356,   432,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,   137,   137,  -356,   472,  -356,
    -356,   137,  -356,  -356,  -356,  -356,  -356,   174,  -356,  -356,
      19,   137,   156,  -356,  -356,  -356,   473,   474,  -356,   135,
     410,   466,   516,   491,  -356,  -356,   474,  -356,    22,  -356,
     192,   243,   243,   468,   243,   243,  -356,  -356,  -356,   490,
    -356,  -356,   465,  -356,   419,   499,  -356,  -356,  -356,  -356,
    -356,  -356,   474,    25,  -356,   156,  -356,  -356,  -356,  -356,
    -356,   135,    10,   249,  -356,   451,   174,  -356,  -356,   441,
    -356,   474,   486,     8,  -356,   280,  -356,   256,  -356,   474,
     551,   552,   553,   554,   555,   556,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,   501,   506,  -356,  -356,  -356,   474,
     156,  -356,  -356,   505,  -356,   172,  -356,  -356,   135,  -356,
    -356,  -356,   497,  -356,  -356,  -356,   242,  -356,  -356,  -356,
    -356,    22,   192,   192,   192,   192,   192,   192,   243,  -356,
    -356,  -356,  -356,  -356,   503,  -356,  -356,   510,  -356,    36,
     528,  -356,  -356,  -356,  -356,  -356,   474,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,   474,  -356,  -356,  -356,   507,
     508,  -356,   512,   511,   384,  -356,  -356,   513,  -356,  -356,
    -356,    36,   353,   117,  -356,  -356,  -356,  -356,   504,  -356,
    -356,   569,  -356,  -356,  -356,   509,  -356,   521,   520,  -356,
    -356,  -356
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -356,   581,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
     492,  -356,  -356,  -356,  -356,  -356,    54,  -356,   514,  -260,
     114,  -356,  -133,  -356,  -223,  -158,   563,   564,   566,  -157,
    -356,   -16,   574,  -356,   -12,   -26,   -11,  -356,  -356,  -356,
    -356,  -356,  -356,  -355,  -356,  -356,   181,   229,  -356,  -356,
     219,  -356,   232,  -356,  -159,  -356,  -356,  -356,  -356,  -356,
    -180,   423,  -216,  -218,  -356,   286,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,   124,
    -356,  -356,   360,  -356,  -356,  -356,   479,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
    -356,  -310,  -356,   335,  -356,  -356,   421,  -356,  -356,   342,
    -240,  -356,  -356,   132,   502,   -41,  -221,  -356,  -356,  -131,
    -356,  -356,  -356,   180,  -356,  -356,  -356,  -356,  -356,   260,
    -356,   139,   336,   263,  -356,   459,  -356,    47,   420,   422
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -276
static const yytype_int16 yytable[] =
{
      69,    22,   219,   301,    51,   326,   339,   226,   303,   233,
     232,    58,   182,   309,   414,   406,    67,   406,   314,    76,
     320,   387,   388,   198,   390,   391,   228,    80,   291,   372,
     149,   225,   228,     3,     4,   105,   228,    85,   285,    74,
     103,   286,    14,    67,   100,   292,   283,   182,    28,   120,
      19,   287,   -17,    63,   355,   101,    68,    91,   284,   352,
     400,     2,   260,   106,   312,   370,   150,   151,   219,   407,
      74,   219,    68,   199,   373,   313,   120,   126,    85,   129,
     393,   394,    29,   318,   125,   219,   225,   371,   373,   379,
      16,   357,   374,   152,    67,   314,    26,   230,   231,   225,
       3,     4,   281,   364,   -24,   267,    80,    55,    80,    75,
     268,   399,   282,    59,    81,   349,   350,   479,   454,    17,
      66,   353,    18,    68,    71,   -24,   222,   260,   -24,    74,
      98,    82,    83,    84,   402,    68,   223,    68,   -24,   243,
      53,    54,   411,   244,   434,   364,   320,   410,   412,    76,
     195,   356,   227,   236,   219,   196,   274,   444,   275,   419,
      77,   238,    78,   365,   155,   156,    31,    32,    33,    34,
      35,    36,    37,    38,   480,  -160,   481,   482,   459,   432,
     446,    74,   466,    80,    60,   467,    61,    62,    63,   410,
     304,   305,   364,   455,   401,   239,   298,   219,   240,   360,
     419,   361,   296,   230,   231,   230,   231,   219,    82,    83,
      84,   219,    68,   102,    79,   202,   485,   203,   204,   205,
     206,   107,   207,   276,   208,   209,   277,   210,   211,   212,
     213,   214,   215,   216,   217,   127,   409,   130,    67,   435,
     230,   231,   219,   380,   381,   382,   383,   384,   385,   320,
     154,   155,   156,    31,    32,    33,    34,    35,   157,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
     158,   -64,   -64,   159,   327,    92,    99,   328,   160,    68,
      96,    19,   161,   162,   228,   155,   156,    31,    32,    33,
      34,    35,    36,    37,    38,   155,   156,    31,    32,    33,
      34,    35,    36,    37,    38,   122,   418,   104,   227,   443,
     230,   231,   229,   409,   219,   123,   420,   230,   231,   421,
    -269,   155,   156,    31,    32,    33,    34,    35,    36,    37,
      38,   155,   156,    31,    32,    33,    34,    35,    36,    37,
      38,   137,   140,   141,   418,   230,   231,  -275,   230,   231,
    -275,   143,   142,  -275,   144,   145,   146,  -275,   147,   148,
     -77,   155,   156,    31,    32,    33,    34,    35,    36,    37,
      38,   184,   187,   195,  -107,    74,   188,  -107,   196,   190,
    -107,   230,   231,  -270,  -107,   192,   193,   201,   221,   237,
     -77,   247,    67,   155,   156,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,   224,   460,   242,   159,   249,    67,   250,   251,
     -79,    68,   252,    19,   254,   256,   270,   269,    39,    40,
      41,    42,    43,    44,    45,    46,    47,   224,   273,   278,
     159,   300,   279,   297,   302,   460,    68,   280,    19,   155,
     156,    31,    32,    33,    34,    35,    36,    37,    38,   155,
     156,    31,    32,    33,    34,    35,    36,    37,    38,   448,
     449,   450,   451,   452,   453,   288,    30,   289,   413,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,   124,   290,   293,   294,
     348,   228,   299,   329,   343,   295,   333,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    82,    83,    84,   344,   346,
     347,   358,   351,   366,   359,   389,   108,   109,   110,   111,
     112,   113,   114,   115,    39,    40,    41,    42,    43,    44,
      45,    46,    47,   367,   369,   392,   415,   398,   422,   423,
     424,   425,   426,   427,   430,   433,   429,   439,   456,   458,
     463,   486,   469,   488,   470,   471,   489,   472,   474,   490,
     491,     1,   475,    48,    49,   121,    50,    20,    97,   437,
     405,   234,   416,   354,   408,   477,   138,   337,   315,   241,
     336,   447,   395,   473,   186,   342,   128,   397,   253,     0,
       0,     0,   255
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-356))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      26,    13,   160,   226,    20,   245,   266,   164,   229,   168,
     168,    22,   143,   231,   369,     7,     7,     7,   234,    36,
     243,   331,   332,     7,   334,   335,     7,     7,    59,     7,
       7,   164,     7,    39,    40,    15,     7,    63,    59,    61,
      81,    62,     7,     7,    66,    76,    59,   178,    57,    90,
      38,    72,    58,    70,    35,    81,    36,    68,    71,   299,
      35,     0,   193,    89,    35,   325,    43,    44,   226,    59,
      61,   229,    36,    57,    66,   234,   117,   103,   104,   105,
     340,   341,    61,   242,   100,   243,   219,   327,    66,   329,
       7,   312,    70,    70,     7,   311,    59,    68,    69,   232,
      39,    40,    59,   319,    36,    59,     7,     7,     7,    55,
      64,   351,    69,    60,    15,   295,   296,   472,   428,    36,
      57,   301,    39,    36,    57,    57,    39,   258,    60,    61,
      76,    32,    33,    34,   355,    36,   162,    36,    70,    66,
      28,    29,   365,    70,   404,   361,   369,   363,   366,    36,
      59,   310,   164,   169,   312,    64,    59,   417,    61,   375,
       7,   172,     7,   320,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    57,    57,    59,    60,   438,   400,
     420,    61,   442,     7,    66,   445,    68,    69,    70,   405,
      16,    17,   408,   433,   353,    57,   222,   355,    60,    64,
     416,    66,   218,    68,    69,    68,    69,   365,    32,    33,
      34,   369,    36,    81,     7,    59,   476,    61,    62,    63,
      64,    89,    66,    59,    68,    69,    62,    71,    72,    73,
      74,    75,    76,    77,    78,   103,    64,   105,     7,    67,
      68,    69,   400,    51,    52,    53,    54,    55,    56,   472,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    60,    60,    67,    63,    35,    36,
      62,    38,    39,    40,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    58,    64,    63,   320,    67,
      68,    69,    35,    64,   472,    62,    60,    68,    69,    63,
      77,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    58,    58,    67,    64,    68,    69,    60,    68,    69,
      63,    70,    67,    66,    66,    66,    66,    70,    66,    66,
      37,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,     7,     6,    59,    60,    61,     7,    63,    64,     7,
      66,    68,    69,    77,    70,    70,    70,     7,     7,     7,
      67,     7,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,   439,    77,    30,     7,     7,    67,    67,
      67,    36,    63,    38,    67,    67,     7,    57,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    59,    65,
      30,     7,    67,    57,     7,   471,    36,    59,    38,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,   422,
     423,   424,   425,   426,   427,    59,     7,    59,    37,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,     7,    59,    59,    59,
      68,     7,    60,    60,    59,     7,     6,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    32,    33,    34,    63,    59,
      59,    58,    60,    67,    60,    67,    43,    44,    45,    46,
      47,    48,    49,    50,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    37,    63,    65,    70,    58,     7,     7,
       7,     7,     7,     7,    58,    60,    65,    70,    65,    59,
      42,    67,    65,     4,    66,    63,    67,    66,    65,    58,
      60,     0,   468,    20,    20,    93,    20,    13,    74,   408,
     361,   168,   373,   307,   362,   471,   117,   262,   238,   178,
     258,   421,   342,   464,   145,   269,   104,   344,   188,    -1,
      -1,    -1,   190
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    80,     0,    39,    40,    81,    82,    83,    86,    90,
      91,    92,    93,    94,     7,    87,     7,    36,    39,    38,
     111,   112,   113,   148,   151,   152,    59,    84,    57,    61,
       7,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,   105,   106,
     107,   110,   150,    28,    29,     7,    95,    96,   115,    60,
      66,    68,    69,    70,   161,   190,    57,     7,    36,   114,
     115,    57,    88,    97,    61,    95,    36,     7,     7,     7,
       7,    15,    32,    33,    34,   114,   191,   192,   193,   194,
     149,   115,    60,    85,    80,    89,    62,    97,    95,    67,
      66,   114,   192,   194,    63,    15,   114,   192,    43,    44,
      45,    46,    47,    48,    49,    50,   162,   163,   164,   165,
     194,    89,    58,    62,     7,   110,   114,   192,   193,   114,
     192,   167,   168,   170,   172,   174,   176,    58,   165,   166,
      58,    67,    67,    70,    66,    66,    66,    66,    66,     7,
      43,    44,    70,   178,     7,     8,     9,    15,    27,    30,
      35,    39,    40,   101,   104,   105,   106,   107,   108,   109,
     110,   111,   113,   114,   116,   133,   153,   183,   184,   185,
     196,   197,   198,   207,     7,   214,   214,     6,     7,   217,
       7,   218,    70,    70,   179,    59,    64,   136,     7,    57,
     209,     7,    59,    61,    62,    63,    64,    66,    68,    69,
      71,    72,    73,    74,    75,    76,    77,    78,   103,   104,
     195,     7,    39,   114,    27,   101,   108,   113,     7,    35,
      68,    69,   104,   133,   140,   141,   110,     7,   115,    57,
      60,   185,    77,    66,    70,   132,   205,     7,   210,     7,
      67,    67,    63,   217,    67,   218,    67,   186,   187,   188,
     198,   180,   181,   182,   198,   134,   137,    59,    64,    57,
       7,   211,   212,    59,    59,    61,    59,    62,    65,    67,
      59,    59,    69,    59,    71,    59,    62,    72,    59,    59,
      59,    59,    76,    59,    59,     7,   110,    57,   114,    60,
       7,   103,     7,   195,    16,    17,   142,   143,   144,   142,
     139,   140,    35,   133,   141,   161,   199,   189,   133,   102,
     103,   117,   118,   121,   122,   206,   189,    60,    63,    60,
     215,   169,   171,     6,   175,   177,   188,   182,    98,    98,
     135,   138,   211,    59,    63,   208,    59,    59,    68,   139,
     139,    60,   189,   139,   144,    35,   133,   195,    58,    60,
      64,    66,   125,   126,   141,   108,    67,    37,   120,    63,
      98,   189,     7,    66,    70,   129,   131,   201,   202,   189,
      51,    52,    53,    54,    55,    56,   216,   180,   180,    67,
     180,   180,    65,    98,    98,   208,   213,   212,    58,   189,
      35,   133,   195,   200,   127,   126,     7,    59,   131,    64,
     141,   103,   142,    37,   122,    70,   129,   204,    64,   141,
      60,    63,     7,     7,     7,     7,     7,     7,   173,    65,
      58,   100,   195,    60,    98,    67,   123,   125,   128,    70,
     156,   157,   203,    67,    98,   130,   189,   202,   216,   216,
     216,   216,   216,   216,   180,   189,    65,    99,    59,    98,
     114,   158,   159,    42,   154,   155,    98,    98,   124,    65,
      66,    63,    66,   210,    65,    99,   160,   158,   119,   122,
      57,    59,    60,   145,   146,    98,    67,   147,     4,    67,
      58,    60
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _DEFAULT_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 11:

/* Line 1806 of yacc.c  */
#line 824 "moc.y"
    { enterNameSpace((yyvsp[(2) - (2)].string)); }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 825 "moc.y"
    { BEGIN IN_NAMESPACE; }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 827 "moc.y"
    { leaveNameSpace();
					       selectOutsideClassState();
					     }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 832 "moc.y"
    { enterNameSpace(); }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 833 "moc.y"
    { BEGIN IN_NAMESPACE; }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 835 "moc.y"
    { leaveNameSpace();
					       selectOutsideClassState();
					     }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 844 "moc.y"
    { selectOutsideClassState(); }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 848 "moc.y"
    { selectOutsideClassState(); }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 851 "moc.y"
    { selectOutsideClassState(); }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 852 "moc.y"
    { selectOutsideClassState(); }
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 855 "moc.y"
    { initClass(); }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 856 "moc.y"
    { generateClass();
						registerClassInNamespace();
						selectOutsideClassState(); }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 864 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 865 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 869 "moc.y"
    { g->tmpExpression = rmWS( g->tmpExpression );
				     (yyval.string) = stradd( (yyvsp[(1) - (4)].string), "<",
						  g->tmpExpression, ">" ); }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 880 "moc.y"
    { initExpression();
						    templLevel = 1;
						    BEGIN IN_TEMPL_ARGS; }
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 893 "moc.y"
    { initExpression();
						    BEGIN IN_EXPR; }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 902 "moc.y"
    { BEGIN IN_DEF_ARG; }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 905 "moc.y"
    { initExpression();
						    BEGIN IN_ENUM; }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 911 "moc.y"
    { (yyval.string) = ""; }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 912 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 913 "moc.y"
    { (yyval.string) = ""; }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 914 "moc.y"
    { skipFunc = true; (yyval.string) = ""; }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 915 "moc.y"
    { skipFunc = true; (yyval.string) = ""; }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 919 "moc.y"
    { (yyval.string) = straddSpc((yyvsp[(1) - (3)].string),(yyvsp[(2) - (3)].string),(yyvsp[(3) - (3)].string)); }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 921 "moc.y"
    { (yyval.string) = ""; }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 922 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 925 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 926 "moc.y"
    { (yyval.string) = straddSpc((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string)); }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 931 "moc.y"
    { skipFunc = true; }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 935 "moc.y"
    { }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 936 "moc.y"
    { }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 939 "moc.y"
    { (yyval.string) = "const"; }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 940 "moc.y"
    { (yyval.string) = "volatile"; }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 943 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 944 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 945 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 949 "moc.y"
    { (yyval.string) = straddSpc((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string)); }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 950 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 953 "moc.y"
    { (yyval.string) = "char"; }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 954 "moc.y"
    { (yyval.string) = "short"; }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 955 "moc.y"
    { (yyval.string) = "int"; }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 956 "moc.y"
    { (yyval.string) = "long"; }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 957 "moc.y"
    { (yyval.string) = "signed"; }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 958 "moc.y"
    { (yyval.string) = "unsigned"; }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 959 "moc.y"
    { (yyval.string) = "float"; }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 960 "moc.y"
    { (yyval.string) = "double"; }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 961 "moc.y"
    { (yyval.string) = "void"; }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 965 "moc.y"
    { g->tmpExpression = rmWS( g->tmpExpression );
				     (yyval.string) = stradd( "template<",
						  g->tmpExpression, ">" ); }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 971 "moc.y"
    { templateClassOld = templateClass;
						  templateClass = true;
						}
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 977 "moc.y"
    { (yyval.string) = "class"; }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 978 "moc.y"
    { (yyval.string) = "struct"; }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 981 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 983 "moc.y"
    { (yyval.string) = stradd( "::", (yyvsp[(2) - (2)].string) ); }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 987 "moc.y"
    { (yyval.string) = stradd( (yyvsp[(1) - (3)].string), "::", (yyvsp[(3) - (3)].string) );}
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 988 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 992 "moc.y"
    { (yyval.string) = straddSpc((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string)); }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 993 "moc.y"
    { (yyval.string) = stradd("enum ",(yyvsp[(2) - (2)].string)); }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 994 "moc.y"
    { (yyval.string) = stradd("union ",(yyvsp[(2) - (2)].string)); }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 999 "moc.y"
    { (yyval.arg_list) = (yyvsp[(1) - (2)].arg_list);}
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 1000 "moc.y"
    { (yyval.arg_list) = (yyvsp[(1) - (3)].arg_list);
				       func_warn("Ellipsis not supported"
						 " in signals and slots.\n"
						 "Ellipsis argument ignored."); }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 1006 "moc.y"
    { (yyval.arg_list) = tmpArgList; }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 1007 "moc.y"
    { (yyval.arg_list) = (yyvsp[(1) - (1)].arg_list); }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 1010 "moc.y"
    { (yyval.arg) = 0; }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 1015 "moc.y"
    { func_warn("Ellipsis not supported"
						 " in signals and slots.\n"
						 "Ellipsis argument ignored."); }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 1023 "moc.y"
    { (yyval.arg_list) = addArg((yyvsp[(3) - (3)].arg)); }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 1024 "moc.y"
    { (yyval.arg_list) = addArg((yyvsp[(1) - (1)].arg)); }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 1028 "moc.y"
    { (yyval.arg) = new Argument(straddSpc((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string)),""); }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 1030 "moc.y"
    { expLevel = 1; }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 1032 "moc.y"
    { (yyval.arg) = new Argument(straddSpc((yyvsp[(1) - (5)].string),(yyvsp[(2) - (5)].string)),"", 0, true ); }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 1035 "moc.y"
    { (yyval.arg) = new Argument(straddSpc((yyvsp[(1) - (4)].string),(yyvsp[(2) - (4)].string)),(yyvsp[(4) - (4)].string), (yyvsp[(3) - (4)].string)); }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 1038 "moc.y"
    { expLevel = 1; }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 1040 "moc.y"
    { (yyval.arg) = new Argument(straddSpc((yyvsp[(1) - (7)].string),(yyvsp[(2) - (7)].string)),(yyvsp[(4) - (7)].string), (yyvsp[(3) - (7)].string), true); }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 1044 "moc.y"
    { (yyval.string) = ""; }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 1045 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 1049 "moc.y"
    { (yyval.string) = straddSpc((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string)); }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 1050 "moc.y"
    { expLevel = 1; }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 1052 "moc.y"
    { (yyval.string) = stradd( "[",
				     g->tmpExpression =
				     g->tmpExpression.stripWhiteSpace(), "]" ); }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 1055 "moc.y"
    { expLevel = 1; }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 1057 "moc.y"
    { (yyval.string) = stradd( (yyvsp[(1) - (5)].string),"[",
				     g->tmpExpression =
				     g->tmpExpression.stripWhiteSpace(),"]" ); }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 1060 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 1061 "moc.y"
    { (yyval.string) = (yyvsp[(2) - (3)].string); }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 1064 "moc.y"
    { (yyval.string) = ""; }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 1066 "moc.y"
    { (yyval.string) = straddSpc((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string));}
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 1067 "moc.y"
    { expLevel = 1; }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 1069 "moc.y"
    { (yyval.string) = stradd( (yyvsp[(1) - (5)].string),"[",
				     g->tmpExpression =
				     g->tmpExpression.stripWhiteSpace(),"]" ); }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 1072 "moc.y"
    { (yyval.string) = (yyvsp[(2) - (3)].string); }
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 1086 "moc.y"
    { tmpFunc->args	     = (yyvsp[(2) - (8)].arg_list);
						  tmpFunc->qualifier = (yyvsp[(4) - (8)].string); }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 1092 "moc.y"
    { func_warn("Variable as signal or slot."); }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 1093 "moc.y"
    { expLevel=0; }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 1095 "moc.y"
    { skipFunc = true; }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 1096 "moc.y"
    { expLevel=0; }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 1098 "moc.y"
    { skipFunc = true; }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 1102 "moc.y"
    { expLevel = 1; }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 1104 "moc.y"
    { expLevel = 1; }
    break;

  case 117:

/* Line 1806 of yacc.c  */
#line 1109 "moc.y"
    { (yyval.string) = ""; }
    break;

  case 118:

/* Line 1806 of yacc.c  */
#line 1110 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 119:

/* Line 1806 of yacc.c  */
#line 1113 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 1114 "moc.y"
    { (yyval.string) = straddSpc((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string));}
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 1117 "moc.y"
    { (yyval.string) = straddSpc("*",(yyvsp[(2) - (2)].string));}
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 1118 "moc.y"
    { (yyval.string) = stradd("&",(yyvsp[(2) - (2)].string));}
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 1125 "moc.y"
    { (yyval.string) = ""; }
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 1126 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 1129 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 1131 "moc.y"
    { (yyval.string) = straddSpc((yyvsp[(1) - (2)].string),(yyvsp[(2) - (2)].string)); }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 1134 "moc.y"
    { (yyval.string) = "const"; }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 1135 "moc.y"
    { (yyval.string) = "volatile"; }
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 1143 "moc.y"
    { BEGIN IN_FCT; fctLevel = 1;}
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 1144 "moc.y"
    { BEGIN QT_DEF; }
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 1151 "moc.y"
    { BEGIN IN_CLASS;
						  classPLevel = 1;
						}
    break;

  case 135:

/* Line 1806 of yacc.c  */
#line 1155 "moc.y"
    { BEGIN QT_DEF; }
    break;

  case 136:

/* Line 1806 of yacc.c  */
#line 1156 "moc.y"
    { BEGIN QT_DEF;	  /* -- " -- */
						  skipClass = true; }
    break;

  case 137:

/* Line 1806 of yacc.c  */
#line 1158 "moc.y"
    { BEGIN QT_DEF;	  /* -- " -- */
 						  skipClass = true; }
    break;

  case 138:

/* Line 1806 of yacc.c  */
#line 1160 "moc.y"
    { BEGIN QT_DEF;	  /* -- " -- */
 						  skipClass = true; }
    break;

  case 139:

/* Line 1806 of yacc.c  */
#line 1164 "moc.y"
    { BEGIN QT_DEF; /* catch ';' */
						  skipClass = true; }
    break;

  case 140:

/* Line 1806 of yacc.c  */
#line 1166 "moc.y"
    { skipClass = true;
						  BEGIN GIMME_SEMICOLON; }
    break;

  case 144:

/* Line 1806 of yacc.c  */
#line 1173 "moc.y"
    { (yyval.string) = ""; }
    break;

  case 146:

/* Line 1806 of yacc.c  */
#line 1179 "moc.y"
    { g->className = (yyvsp[(2) - (2)].string);
						  if ( g->className == "TQObject" )
						     TQ_OBJECTdetected = true;
						}
    break;

  case 147:

/* Line 1806 of yacc.c  */
#line 1185 "moc.y"
    { g->className = (yyvsp[(3) - (3)].string);
						  if ( g->className == "TQObject" )
						     TQ_OBJECTdetected = true;
						}
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 1192 "moc.y"
    { g->superClassName = (yyvsp[(2) - (2)].string); }
    break;

  case 149:

/* Line 1806 of yacc.c  */
#line 1197 "moc.y"
    { templateClass = templateClassOld; }
    break;

  case 158:

/* Line 1806 of yacc.c  */
#line 1220 "moc.y"
    { expLevel = 1; }
    break;

  case 160:

/* Line 1806 of yacc.c  */
#line 1225 "moc.y"
    { (yyval.string) = 0; }
    break;

  case 161:

/* Line 1806 of yacc.c  */
#line 1226 "moc.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); }
    break;

  case 166:

/* Line 1806 of yacc.c  */
#line 1238 "moc.y"
    { tmpAccess = (yyvsp[(1) - (1)].access); }
    break;

  case 167:

/* Line 1806 of yacc.c  */
#line 1239 "moc.y"
    { moc_err( "Missing access specifier"
						   " before \"slots:\"." ); }
    break;

  case 168:

/* Line 1806 of yacc.c  */
#line 1243 "moc.y"
    { BEGIN QT_DEF; }
    break;

  case 170:

/* Line 1806 of yacc.c  */
#line 1245 "moc.y"
    { BEGIN QT_DEF; }
    break;

  case 172:

/* Line 1806 of yacc.c  */
#line 1247 "moc.y"
    {
			      if ( tmpAccess )
				  moc_warn("TQ_OBJECT is not in the private"
					   " section of the class.\n"
					   "TQ_OBJECT is a macro that resets"
					   " access permission to \"private\".");
			      TQ_OBJECTdetected = true;
			  }
    break;

  case 173:

/* Line 1806 of yacc.c  */
#line 1255 "moc.y"
    { tmpYYStart = YY_START;
				       tmpPropOverride = false;
				       BEGIN IN_PROPERTY; }
    break;

  case 174:

/* Line 1806 of yacc.c  */
#line 1258 "moc.y"
    {
						BEGIN tmpYYStart;
					   }
    break;

  case 176:

/* Line 1806 of yacc.c  */
#line 1262 "moc.y"
    { tmpYYStart = YY_START;
				       tmpPropOverride = true;
				       BEGIN IN_PROPERTY; }
    break;

  case 177:

/* Line 1806 of yacc.c  */
#line 1265 "moc.y"
    {
						BEGIN tmpYYStart;
					   }
    break;

  case 179:

/* Line 1806 of yacc.c  */
#line 1269 "moc.y"
    { tmpYYStart = YY_START; BEGIN IN_CLASSINFO; }
    break;

  case 180:

/* Line 1806 of yacc.c  */
#line 1271 "moc.y"
    {
				      g->infos.append( new ClassInfo( (yyvsp[(4) - (7)].string), (yyvsp[(6) - (7)].string) ) );
				      BEGIN tmpYYStart;
				  }
    break;

  case 182:

/* Line 1806 of yacc.c  */
#line 1276 "moc.y"
    { tmpYYStart = YY_START; BEGIN IN_PROPERTY; }
    break;

  case 183:

/* Line 1806 of yacc.c  */
#line 1277 "moc.y"
    {
						TQ_PROPERTYdetected = true;
						BEGIN tmpYYStart;
					   }
    break;

  case 185:

/* Line 1806 of yacc.c  */
#line 1282 "moc.y"
    { tmpYYStart = YY_START; BEGIN IN_PROPERTY; }
    break;

  case 186:

/* Line 1806 of yacc.c  */
#line 1283 "moc.y"
    {
						TQ_PROPERTYdetected = true;
						BEGIN tmpYYStart;
					   }
    break;

  case 188:

/* Line 1806 of yacc.c  */
#line 1290 "moc.y"
    { moc_err( "Signals cannot "
						 "have access specifiers" ); }
    break;

  case 190:

/* Line 1806 of yacc.c  */
#line 1293 "moc.y"
    { if ( tmpAccess == Public && TQ_PROPERTYdetected )
						  BEGIN QT_DEF;
					      else
						  BEGIN IN_CLASS;
					  suppress_func_warn = true;
					}
    break;

  case 191:

/* Line 1806 of yacc.c  */
#line 1300 "moc.y"
    {
					  suppress_func_warn = false;
					}
    break;

  case 192:

/* Line 1806 of yacc.c  */
#line 1303 "moc.y"
    { BEGIN IN_CLASS;
					   if ( classPLevel != 1 )
					       moc_warn( "unexpected access"
							 "specifier" );
					}
    break;

  case 197:

/* Line 1806 of yacc.c  */
#line 1318 "moc.y"
    { addMember( PropertyCandidateMember ); }
    break;

  case 202:

/* Line 1806 of yacc.c  */
#line 1330 "moc.y"
    { addMember( SignalMember ); }
    break;

  case 207:

/* Line 1806 of yacc.c  */
#line 1341 "moc.y"
    { addMember( SlotMember ); }
    break;

  case 210:

/* Line 1806 of yacc.c  */
#line 1348 "moc.y"
    { (yyval.string)=(yyvsp[(2) - (2)].string); }
    break;

  case 211:

/* Line 1806 of yacc.c  */
#line 1351 "moc.y"
    { g->multipleSuperClasses.append( (yyvsp[(3) - (3)].string) ); }
    break;

  case 213:

/* Line 1806 of yacc.c  */
#line 1356 "moc.y"
    { (yyval.string) = stradd( (yyvsp[(1) - (4)].string), "(", (yyvsp[(3) - (4)].string), ")" ); }
    break;

  case 214:

/* Line 1806 of yacc.c  */
#line 1358 "moc.y"
    { (yyval.string) = stradd( (yyvsp[(1) - (4)].string), "(", (yyvsp[(3) - (4)].string), ")" ); }
    break;

  case 215:

/* Line 1806 of yacc.c  */
#line 1361 "moc.y"
    {(yyval.string)=(yyvsp[(1) - (1)].string);}
    break;

  case 216:

/* Line 1806 of yacc.c  */
#line 1362 "moc.y"
    {(yyval.string)=(yyvsp[(3) - (3)].string);}
    break;

  case 217:

/* Line 1806 of yacc.c  */
#line 1363 "moc.y"
    {(yyval.string)=(yyvsp[(2) - (2)].string);}
    break;

  case 218:

/* Line 1806 of yacc.c  */
#line 1364 "moc.y"
    {(yyval.string)=(yyvsp[(3) - (3)].string);}
    break;

  case 219:

/* Line 1806 of yacc.c  */
#line 1365 "moc.y"
    {(yyval.string)=(yyvsp[(2) - (2)].string);}
    break;

  case 220:

/* Line 1806 of yacc.c  */
#line 1366 "moc.y"
    {(yyval.string)=(yyvsp[(1) - (1)].string);}
    break;

  case 221:

/* Line 1806 of yacc.c  */
#line 1367 "moc.y"
    {(yyval.string)=(yyvsp[(3) - (3)].string);}
    break;

  case 222:

/* Line 1806 of yacc.c  */
#line 1368 "moc.y"
    {(yyval.string)=(yyvsp[(2) - (2)].string);}
    break;

  case 223:

/* Line 1806 of yacc.c  */
#line 1369 "moc.y"
    {(yyval.string)=(yyvsp[(3) - (3)].string);}
    break;

  case 224:

/* Line 1806 of yacc.c  */
#line 1370 "moc.y"
    {(yyval.string)=(yyvsp[(2) - (2)].string);}
    break;

  case 225:

/* Line 1806 of yacc.c  */
#line 1373 "moc.y"
    { (yyval.access)=Private; }
    break;

  case 226:

/* Line 1806 of yacc.c  */
#line 1374 "moc.y"
    { (yyval.access)=Protected; }
    break;

  case 227:

/* Line 1806 of yacc.c  */
#line 1375 "moc.y"
    { (yyval.access)=Public; }
    break;

  case 228:

/* Line 1806 of yacc.c  */
#line 1378 "moc.y"
    { }
    break;

  case 229:

/* Line 1806 of yacc.c  */
#line 1379 "moc.y"
    { }
    break;

  case 271:

/* Line 1806 of yacc.c  */
#line 1427 "moc.y"
    { tmpFunc->type = (yyvsp[(1) - (2)].string);
						  tmpFunc->name = (yyvsp[(2) - (2)].string); }
    break;

  case 272:

/* Line 1806 of yacc.c  */
#line 1430 "moc.y"
    { tmpFunc->type = "int";
						  tmpFunc->name = (yyvsp[(1) - (1)].string);
				  if ( tmpFunc->name == g->className )
				      func_warn( "Constructors cannot be"
						 " signals or slots.");
						}
    break;

  case 273:

/* Line 1806 of yacc.c  */
#line 1437 "moc.y"
    { tmpFunc->type = "void";
						  tmpFunc->name = "~";
						  tmpFunc->name += (yyvsp[(3) - (3)].string);
				       func_warn( "Destructors cannot be"
						  " signals or slots.");
						}
    break;

  case 274:

/* Line 1806 of yacc.c  */
#line 1445 "moc.y"
    {
						    char *tmp =
							straddSpc((yyvsp[(1) - (5)].string),(yyvsp[(2) - (5)].string),(yyvsp[(3) - (5)].string),(yyvsp[(4) - (5)].string));
						    tmpFunc->type = rmWS(tmp);
						    delete [] tmp;
						    tmpFunc->name = (yyvsp[(5) - (5)].string); }
    break;

  case 275:

/* Line 1806 of yacc.c  */
#line 1452 "moc.y"
    { skipFunc = true; }
    break;

  case 276:

/* Line 1806 of yacc.c  */
#line 1454 "moc.y"
    { tmpFunc->type =
						      straddSpc((yyvsp[(1) - (3)].string),(yyvsp[(2) - (3)].string));
						  tmpFunc->name = (yyvsp[(3) - (3)].string); }
    break;

  case 277:

/* Line 1806 of yacc.c  */
#line 1459 "moc.y"
    { tmpFunc->type =
						      straddSpc((yyvsp[(1) - (4)].string),(yyvsp[(2) - (4)].string),(yyvsp[(3) - (4)].string));
						  tmpFunc->name = (yyvsp[(4) - (4)].string); }
    break;

  case 278:

/* Line 1806 of yacc.c  */
#line 1463 "moc.y"
    { operatorError();    }
    break;

  case 279:

/* Line 1806 of yacc.c  */
#line 1465 "moc.y"
    { operatorError();    }
    break;

  case 280:

/* Line 1806 of yacc.c  */
#line 1468 "moc.y"
    { operatorError();    }
    break;

  case 281:

/* Line 1806 of yacc.c  */
#line 1470 "moc.y"
    { operatorError();    }
    break;

  case 282:

/* Line 1806 of yacc.c  */
#line 1473 "moc.y"
    { operatorError();    }
    break;

  case 284:

/* Line 1806 of yacc.c  */
#line 1479 "moc.y"
    { func_warn("Unexpected variable declaration."); }
    break;

  case 285:

/* Line 1806 of yacc.c  */
#line 1482 "moc.y"
    { func_warn("Unexpected variable declaration."); }
    break;

  case 286:

/* Line 1806 of yacc.c  */
#line 1484 "moc.y"
    { func_warn("Unexpected enum declaration."); }
    break;

  case 287:

/* Line 1806 of yacc.c  */
#line 1486 "moc.y"
    { func_warn("Unexpected using declaration."); }
    break;

  case 288:

/* Line 1806 of yacc.c  */
#line 1488 "moc.y"
    { func_warn("Unexpected using declaration."); }
    break;

  case 289:

/* Line 1806 of yacc.c  */
#line 1490 "moc.y"
    { classPLevel++;
				  moc_err("Unexpected namespace declaration."); }
    break;

  case 290:

/* Line 1806 of yacc.c  */
#line 1493 "moc.y"
    { func_warn("Unexpected class declaration.");}
    break;

  case 291:

/* Line 1806 of yacc.c  */
#line 1495 "moc.y"
    { func_warn("Unexpected class declaration.");
				  BEGIN IN_FCT; fctLevel=1;
				}
    break;

  case 292:

/* Line 1806 of yacc.c  */
#line 1498 "moc.y"
    { BEGIN QT_DEF; }
    break;

  case 296:

/* Line 1806 of yacc.c  */
#line 1507 "moc.y"
    { }
    break;

  case 297:

/* Line 1806 of yacc.c  */
#line 1508 "moc.y"
    { expLevel = 0; }
    break;

  case 299:

/* Line 1806 of yacc.c  */
#line 1510 "moc.y"
    { expLevel = 0; }
    break;

  case 302:

/* Line 1806 of yacc.c  */
#line 1515 "moc.y"
    { expLevel = 0; }
    break;

  case 307:

/* Line 1806 of yacc.c  */
#line 1530 "moc.y"
    { BEGIN QT_DEF;
				  if ( tmpAccess == Public) {
				      tmpEnum->name = (yyvsp[(1) - (5)].string);
				      addEnum();
				  }
				}
    break;

  case 308:

/* Line 1806 of yacc.c  */
#line 1537 "moc.y"
    { tmpEnum->clear();}
    break;

  case 310:

/* Line 1806 of yacc.c  */
#line 1541 "moc.y"
    { }
    break;

  case 314:

/* Line 1806 of yacc.c  */
#line 1549 "moc.y"
    { if ( tmpAccess == Public) tmpEnum->append( (yyvsp[(1) - (1)].string) ); }
    break;

  case 315:

/* Line 1806 of yacc.c  */
#line 1550 "moc.y"
    { enumLevel=0; }
    break;

  case 316:

/* Line 1806 of yacc.c  */
#line 1551 "moc.y"
    {  if ( tmpAccess == Public) tmpEnum->append( (yyvsp[(1) - (4)].string) );  }
    break;

  case 317:

/* Line 1806 of yacc.c  */
#line 1555 "moc.y"
    {
				     g->propWrite = "";
				     g->propRead = "";
				     g->propOverride = tmpPropOverride;
				     g->propReset = "";
				     if ( g->propOverride ) {
					 g->propStored = "";
					 g->propDesignable = "";
					 g->propScriptable = "";
				     } else {
					 g->propStored = "true";
					 g->propDesignable = "true";
					 g->propScriptable = "true";
				     }
				}
    break;

  case 318:

/* Line 1806 of yacc.c  */
#line 1571 "moc.y"
    {
				    if ( g->propRead.isEmpty() && !g->propOverride )
					moc_err( "A property must at least feature a read method." );
				    checkPropertyName( (yyvsp[(2) - (4)].string) );
				    TQ_PROPERTYdetected = true;
				    // Avoid duplicates
				    for( TQPtrListIterator<Property> lit( g->props ); lit.current(); ++lit ) {
					if ( lit.current()->name == (yyvsp[(2) - (4)].string) ) {
					    if ( displayWarnings )
						moc_err( "Property '%s' defined twice.",
							 (const char*)lit.current()->name );
					}
				    }
				    g->props.append( new Property( lineNo, (yyvsp[(1) - (4)].string), (yyvsp[(2) - (4)].string),
								g->propWrite, g->propRead, g->propReset,
								   g->propStored, g->propDesignable,
								   g->propScriptable, g->propOverride ) );
				}
    break;

  case 320:

/* Line 1806 of yacc.c  */
#line 1592 "moc.y"
    { g->propRead = (yyvsp[(2) - (3)].string); }
    break;

  case 321:

/* Line 1806 of yacc.c  */
#line 1593 "moc.y"
    { g->propWrite = (yyvsp[(2) - (3)].string); }
    break;

  case 322:

/* Line 1806 of yacc.c  */
#line 1594 "moc.y"
    { g->propReset = (yyvsp[(2) - (3)].string); }
    break;

  case 323:

/* Line 1806 of yacc.c  */
#line 1595 "moc.y"
    { g->propStored = (yyvsp[(2) - (3)].string); }
    break;

  case 324:

/* Line 1806 of yacc.c  */
#line 1596 "moc.y"
    { g->propDesignable = (yyvsp[(2) - (3)].string); }
    break;

  case 325:

/* Line 1806 of yacc.c  */
#line 1597 "moc.y"
    { g->propScriptable = (yyvsp[(2) - (3)].string); }
    break;

  case 326:

/* Line 1806 of yacc.c  */
#line 1600 "moc.y"
    { }
    break;

  case 327:

/* Line 1806 of yacc.c  */
#line 1601 "moc.y"
    { g->qtEnums.append( (yyvsp[(1) - (2)].string) ); }
    break;

  case 328:

/* Line 1806 of yacc.c  */
#line 1604 "moc.y"
    { }
    break;

  case 329:

/* Line 1806 of yacc.c  */
#line 1605 "moc.y"
    { g->qtSets.append( (yyvsp[(1) - (2)].string) ); }
    break;



/* Line 1806 of yacc.c  */
#line 4252 "moc_yacc"
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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 1608 "moc.y"


#ifndef YYBISON
# if defined(Q_OS_WIN32)
# include <io.h>
# undef isatty
extern "C" int hack_isatty( int )
{
    return 0;
}
# define isatty hack_isatty
# else
# include <unistd.h>
# endif
# include "moc_lex.cpp"
#endif //YYBISON

void      cleanup();
TQCString  combinePath( const char *, const char * );

FILE  *out;					// output file

parser_reg::parser_reg() : funcs(true)
{
    gen_count = 0;
    noInclude     = false;		// no #include <filename>
    generatedCode = false;		// no code generated
    mocError = false;			// moc parsing error occurred
    hasVariantIncluded = false;
}


parser_reg::~parser_reg()
{
    slots.clear();
    signals.clear();
    propfuncs.clear();
    funcs.clear();
    infos.clear();
    props.clear();
    infos.clear();
}

int yyparse();

void replace( char *s, char c1, char c2 );

void setDefaultIncludeFile()
{
    if ( g->includeFiles.isEmpty() ) {
	if ( g->includePath.isEmpty() ) {
	    if ( !g->fileName.isEmpty() && !g->outputFile.isEmpty() ) {
		g->includeFiles.append( combinePath(g->fileName, g->outputFile) );
	    } else {
		g->includeFiles.append( g->fileName );
	    }
	} else {
	    g->includeFiles.append( combinePath(g->fileName, g->fileName) );
	}
    }
}

#ifdef Q_CC_MSVC
#define ErrorFormatString "%s(%d):"
#else
#define ErrorFormatString "%s:%d:"
#endif

int main( int argc, char **argv )
{
    init();

    bool autoInclude = true;
    const char *error	     = 0;
    g->qtPath = "";
    for ( int n=1; n<argc && error==0; n++ ) {
	TQCString arg = argv[n];
	if ( arg[0] == '-' ) {			// option
	    TQCString opt = &arg[1];
	    if ( opt[0] == 'o' ) {		// output redirection
		if ( opt[1] == '\0' ) {
		    if ( !(n < argc-1) ) {
			error = "Missing output file name";
			break;
		    }
		    g->outputFile = argv[++n];
		} else
		    g->outputFile = &opt[1];
	    } else if ( opt == "i" ) {		// no #include statement
		g->noInclude   = true;
		autoInclude = false;
	    } else if ( opt[0] == 'f' ) {	// produce #include statement
		g->noInclude   = false;
		autoInclude = false;
		if ( opt[1] )			// -fsomething.h
		    g->includeFiles.append( &opt[1] );
	    } else if ( opt == "pch" ) {	// produce #include statement for PCH
		if ( !(n < argc-1) ) {
		    error = "Missing name of PCH file";
		    break;
		}
		g->pchFile = argv[++n];
	    } else if ( opt[0] == 'p' ) {	// include file path
		if ( opt[1] == '\0' ) {
		    if ( !(n < argc-1) ) {
			error = "Missing path name for the -p option.";
			break;
		    }
		    g->includePath = argv[++n];
		} else {
		    g->includePath = &opt[1];
		}
	    } else if ( opt[0] == 'q' ) {	// qt include file path
		if ( opt[1] == '\0' ) {
		    if ( !(n < argc-1) ) {
			error = "Missing path name for the -q option.";
			break;
		    }
		    g->qtPath = argv[++n];
		} else {
		    g->qtPath = &opt[1];
		}
		replace(g->qtPath.data(),'\\','/');
		if ( g->qtPath.right(1) != "/" )
		    g->qtPath += '/';
	    } else if ( opt == "v" ) {		// version number
		fprintf( stderr, "TQt Meta Object Compiler version %d"
				 " (TQt %s)\n", formatRevision,
				 TQT_VERSION_STR );
		cleanup();
		return 1;
	    } else if ( opt == "k" ) {		// stop on errors
		errorControl = true;
	    } else if ( opt == "nw" ) {		// don't display warnings
		displayWarnings = false;
	    } else if ( opt == "ldbg" ) {	// lex debug output
		lexDebug = true;
	    } else if ( opt == "ydbg" ) {	// yacc debug output
		yydebug = true;
	    } else {
		error = "Invalid argument";
	    }
	} else {
	    if ( !g->fileName.isNull() )		// can handle only one file
		error = "Too many input files specified";
	    else
		g->fileName = arg.copy();
	}
    }

    if ( autoInclude ) {
	int ppos = g->fileName.findRev('.');
	if ( ppos != -1 && tolower( g->fileName[ppos + 1] ) == 'h' )
	    g->noInclude = false;
	else
	    g->noInclude = true;
    }
    setDefaultIncludeFile();

    if ( g->fileName.isNull() && !error ) {
	g->fileName = "standard input";
	yyin	 = stdin;
    } else if ( argc < 2 || error ) {		// incomplete/wrong args
	fprintf( stderr, "TQt meta object compiler\n" );
	if ( error )
	    fprintf( stderr, "moc: %s\n", error );
	fprintf( stderr, "Usage:  moc [options] <header-file>\n"
		 "\t-o file    Write output to file rather than stdout\n"
		 "\t-f[file]   Force #include, optional file name\n"
		 "\t-p path    Path prefix for included file\n"
		 "\t-i         Do not generate an #include statement\n"
		 "\t-k         Do not stop on errors\n"
		 "\t-nw        Do not display warnings\n"
		 "\t-v         Display version of moc\n" );
	cleanup();
	return 1;
    } else {
	yyin = fopen( (const char *)g->fileName, "r" );
	if ( !yyin ) {
	    fprintf( stderr, "moc: %s: No such file\n", (const char*)g->fileName);
	    cleanup();
	    return 1;
	}
    }
    if ( !g->outputFile.isEmpty() ) {		// output file specified
	out = fopen( (const char *)g->outputFile, "w" );	// create output file
	if ( !out ) {
	    fprintf( stderr, "moc: Cannot create %s\n",
		     (const char*)g->outputFile );
	    cleanup();
	    return 1;
	}
    } else {					// use stdout
	out = stdout;
    }
    yyparse();
    fclose( yyin );
    if ( !g->outputFile.isNull() )
	fclose( out );

    if ( !g->generatedCode && displayWarnings && !g->mocError ) {
	fprintf( stderr, ErrorFormatString" Warning: %s\n", g->fileName.data(), 0,
		 "No relevant classes found. No output generated." );
    }

    int ret = g->mocError ? 1 : 0;
    cleanup();
    return ret;
}

void replace( char *s, char c1, char c2 )
{
    if ( !s )
	return;
    while ( *s ) {
	if ( *s == c1 )
	    *s = c2;
	s++;
    }
}

/*
    This function looks at two file names and returns the name of the
    infile with a path relative to outfile.

    Examples:

	/tmp/abc, /tmp/bcd -> abc
	xyz/a/bc, xyz/b/ac -> ../a/bc
	/tmp/abc, xyz/klm -> /tmp/abc
 */

TQCString combinePath( const char *infile, const char *outfile )
{
    TQFileInfo inFileInfo( TQDir::current(), TQFile::decodeName(infile) );
    TQFileInfo outFileInfo( TQDir::current(), TQFile::decodeName(outfile) );
    int numCommonComponents = 0;

    TQStringList inSplitted =
	TQStringList::split( '/', inFileInfo.dir().canonicalPath(), true );
    TQStringList outSplitted =
	TQStringList::split( '/', outFileInfo.dir().canonicalPath(), true );

    while ( !inSplitted.isEmpty() && !outSplitted.isEmpty() &&
	    inSplitted.first() == outSplitted.first() ) {
	inSplitted.remove( inSplitted.begin() );
	outSplitted.remove( outSplitted.begin() );
	numCommonComponents++;
    }

    if ( numCommonComponents < 2 ) {
	/*
	  The paths don't have the same drive, or they don't have the
	  same root directory. Use an absolute path.
	*/
	return TQFile::encodeName( inFileInfo.absFilePath() );
    } else {
	/*
	  The paths have something in common. Use a path relative to
	  the output file.
	*/
	while ( !outSplitted.isEmpty() ) {
	    outSplitted.remove( outSplitted.begin() );
	    inSplitted.prepend( ".." );
	}
	inSplitted.append( inFileInfo.fileName() );
	return TQFile::encodeName( inSplitted.join("/") );
    }
}


#define getenv hack_getenv			// workaround for byacc
char *getenv()		     { return 0; }
char *getenv( const char * ) { return 0; }

void init()					// initialize
{
    BEGIN OUTSIDE;
    if(g)
	delete g;
    g = new parser_reg;
    lineNo	 = 1;
    skipClass	 = false;
    skipFunc	 = false;
    tmpArgList	 = new ArgList;
    tmpFunc	 = new Function;
    tmpEnum	 = new Enum;
}

void cleanup()
{
    delete g;
    g = NULL;
}

void initClass()				 // prepare for new class
{
    tmpAccess      = Private;
    subClassPerm       = Private;
    TQ_OBJECTdetected   = false;
    TQ_PROPERTYdetected = false;
    skipClass	       = false;
    templateClass      = false;
    g->slots.clear();
    g->signals.clear();
    g->propfuncs.clear();
    g->enums.clear();
    g->funcs.clear();
    g->props.clear();
    g->infos.clear();
    g->qtSets.clear();
    g->qtEnums.clear();
    g->multipleSuperClasses.clear();
}

struct NamespaceInfo
{
    TQCString name;
    int pLevelOnEntering; // Parenthesis level on entering the namespace
    TQDict<char> definedClasses; // Classes defined in the namespace
};

TQPtrList<NamespaceInfo> namespaces;

void enterNameSpace( const char *name )	 // prepare for new class
{
    static bool first = true;
    if ( first ) {
	namespaces.setAutoDelete( true );
	first = false;
    }

    NamespaceInfo *tmp = new NamespaceInfo;
    if ( name )
	tmp->name = name;
    tmp->pLevelOnEntering = namespacePLevel;
    namespaces.append( tmp );
}

void leaveNameSpace()				 // prepare for new class
{
    NamespaceInfo *tmp = namespaces.last();
    namespacePLevel = tmp->pLevelOnEntering;
    namespaces.remove();
}

TQCString nameQualifier()
{
    TQPtrListIterator<NamespaceInfo> iter( namespaces );
    NamespaceInfo *tmp;
    TQCString qualifier = "";
    for( ; (tmp = iter.current()) ; ++iter ) {
	if ( !tmp->name.isNull() ) {  // If not unnamed namespace
	    qualifier += tmp->name;
	    qualifier += "::";
	}
    }
    return qualifier;
}

int openNameSpaceForMetaObject( FILE *out )
{
    int levels = 0;
    TQPtrListIterator<NamespaceInfo> iter( namespaces );
    NamespaceInfo *tmp;
    TQCString indent = "";
    for( ; (tmp = iter.current()) ; ++iter ) {
	if ( !tmp->name.isNull() ) {  // If not unnamed namespace
	    fprintf( out, "%snamespace %s {\n", (const char *)indent,
		     (const char *) tmp->name );
	    indent += "    ";
	    levels++;
	}
    }
    TQCString nm = g->className;
    int pos;
    while( (pos = nm.find( "::" )) != -1 ) {
	TQCString spaceName = nm.left( pos );
	nm = nm.right( nm.length() - pos - 2 );
	if ( !spaceName.isEmpty() ) {
	    fprintf( out, "%snamespace %s {\n", (const char *)indent,
		     (const char *) spaceName );
	    indent += "    ";
	    levels++;
	}
    }
    return levels;
}

void closeNameSpaceForMetaObject( FILE *out, int levels )
{
    int i;
    for( i = 0 ; i < levels ; i++ )
	    fprintf( out, "}" );
    if ( levels )
	fprintf( out, "\n" );

}

void selectOutsideClassState()
{
    if ( namespaces.count() == 0 )
	BEGIN OUTSIDE;
    else
	BEGIN IN_NAMESPACE;
}

void registerClassInNamespace()
{
    if ( namespaces.count() == 0 )
	return;
    namespaces.last()->definedClasses.insert((const char *)g->className,(char*)1);
}

//
// Remove white space from TQ_SIGNAL and TQ_SLOT names.
// This function has been copied from qobject.cpp.
//

inline bool isSpace( char x )
{
    return isspace( (uchar) x );
}

static TQCString rmWS( const char *src )
{
    TQCString result( tqstrlen(src)+1 );
    char *d = result.data();
    char *s = (char *)src;
    char last = 0;
    while( *s && isSpace(*s) )			// skip leading space
	s++;
    while ( *s ) {
	while ( *s && !isSpace(*s) )
	    last = *d++ = *s++;
	while ( *s && isSpace(*s) )
	    s++;
	if ( *s && isIdentChar(*s) && isIdentChar(last) )
	    last = *d++ = ' ';
    }
    result.truncate( (int)(d - result.data()) );
    return result;
}


void initExpression()
{
    g->tmpExpression = "";
}

void addExpressionString( const char *s )
{
    g->tmpExpression += s;
}

void addExpressionChar( const char c )
{
    g->tmpExpression += c;
}

void yyerror( const char *msg )			// print yacc error message
{
    g->mocError = true;
    fprintf( stderr, ErrorFormatString" Error: %s\n", g->fileName.data(), lineNo, msg );
    if ( errorControl ) {
	if ( !g->outputFile.isEmpty() && yyin && fclose(yyin) == 0 )
	    remove( g->outputFile );
	exit( -1 );
    }
}

void moc_err( const char *s )
{
    yyerror( s );
}

void moc_err( const char *s1, const char *s2 )
{
    static char tmp[1024];
    sprintf( tmp, s1, s2 );
    yyerror( tmp );
}

void moc_warn( const char *msg )
{
    if ( displayWarnings )
	fprintf( stderr, ErrorFormatString" Warning: %s\n", g->fileName.data(), lineNo, msg);
}

void moc_warn( char *s1, char *s2 )
{
    static char tmp[1024];
    sprintf( tmp, s1, s2 );
    if ( displayWarnings )
	fprintf( stderr, ErrorFormatString" Warning: %s\n", g->fileName.data(), lineNo, tmp);
}

void func_warn( const char *msg )
{
    if ( !suppress_func_warn )
	moc_warn( msg );
    skipFunc = true;
}

void operatorError()
{
    if ( !suppress_func_warn )
	moc_warn("Operator functions cannot be signals or slots.");
    skipFunc = true;
}

#ifndef yywrap
int yywrap()					// more files?
{
    return 1;					// end of file
}
#endif

char *stradd( const char *s1, const char *s2 )	// adds two strings
{
    char *n = new char[tqstrlen(s1)+tqstrlen(s2)+1];
    qstrcpy( n, s1 );
    strcat( n, s2 );
    return n;
}

char *stradd( const char *s1, const char *s2, const char *s3 )// adds 3 strings
{
    char *n = new char[tqstrlen(s1)+tqstrlen(s2)+tqstrlen(s3)+1];
    qstrcpy( n, s1 );
    strcat( n, s2 );
    strcat( n, s3 );
    return n;
}

char *stradd( const char *s1, const char *s2,
	      const char *s3, const char *s4 )// adds 4 strings
{
    char *n = new char[tqstrlen(s1)+tqstrlen(s2)+tqstrlen(s3)+tqstrlen(s4)+1];
    qstrcpy( n, s1 );
    strcat( n, s2 );
    strcat( n, s3 );
    strcat( n, s4 );
    return n;
}


char *straddSpc( const char *s1, const char *s2 )
{
    char *n = new char[tqstrlen(s1)+tqstrlen(s2)+2];
    qstrcpy( n, s1 );
    strcat( n, " " );
    strcat( n, s2 );
    return n;
}

char *straddSpc( const char *s1, const char *s2, const char *s3 )
{
    char *n = new char[tqstrlen(s1)+tqstrlen(s2)+tqstrlen(s3)+3];
    qstrcpy( n, s1 );
    strcat( n, " " );
    strcat( n, s2 );
    strcat( n, " " );
    strcat( n, s3 );
    return n;
}

char *straddSpc( const char *s1, const char *s2,
	      const char *s3, const char *s4 )
{
    char *n = new char[tqstrlen(s1)+tqstrlen(s2)+tqstrlen(s3)+tqstrlen(s4)+4];
    qstrcpy( n, s1 );
    strcat( n, " " );
    strcat( n, s2 );
    strcat( n, " " );
    strcat( n, s3 );
    strcat( n, " " );
    strcat( n, s4 );
    return n;
}

// Generate C++ code for building member function table


/*
  We call B::tqt_invoke() rather than A::B::tqt_invoke() to
  work around a bug in MSVC 6. The bug occurs if the
  super-class is in a namespace and the sub-class isn't.

  Exception: If the superclass has the same name as the subclass, we
  want non-MSVC users to have a working generated files.
*/
TQCString purestSuperClassName()
{
    TQCString sc = g->superClassName;
    TQCString c = g->className;
    /*
      Make sure qualified template arguments (e.g., foo<bar::baz>)
      don't interfere.
    */
    int pos = sc.findRev( "::", sc.find( '<' ) );
    if ( pos != -1 ) {
	sc = sc.right( sc.length() - pos - 2 );
	pos = c.findRev( "::" );
	if ( pos != -1 )
	    c = c.right( c.length() - pos - 2 );
	if ( sc == c )
	    sc = g->superClassName;
    }
    return sc;
}

TQCString qualifiedClassName()
{
    return nameQualifier() + g->className;
}

const int Slot_Num = 1;
const int Signal_Num = 2;
const int Prop_Num = 3;

void generateFuncs( FuncList *list, const char *functype, int num )
{
    Function *f;
    for ( f=list->first(); f; f=list->next() ) {
	bool hasReturnValue = f->type != "void" && (validUType( f->type ) || isVariantType( f->type) );

	if ( hasReturnValue || !f->args->isEmpty() ) {
	    fprintf( out, "    static const TQUParameter param_%s_%d[] = {\n", functype, list->at() );
	    if ( hasReturnValue ) {
		if ( validUType( f->type ) )
		    fprintf( out, "\t{ 0, &static_QUType_%s, %s, TQUParameter::Out }", uType(f->type).data(), uTypeExtra(f->type).data() );
		else
		    fprintf( out, "\t{ 0, &static_QUType_TQVariant, %s, TQUParameter::Out }", uTypeExtra(f->type).data() );
		if ( !f->args->isEmpty() )
		    fprintf( out, ",\n" );
	    }
	    Argument* a = f->args->first();
	    while ( a ) {
		TQCString type = a->leftType + ' ' + a->rightType;
		type = type.simplifyWhiteSpace();
		if( a->name.isEmpty() )
		    fprintf( out, "\t{ 0, &static_QUType_%s, %s, TQUParameter::%s }",
			     uType( type ).data(), uTypeExtra( type ).data(),
			     isInOut( type ) ? "InOut" : "In" );
		else
		    fprintf( out, "\t{ \"%s\", &static_QUType_%s, %s, TQUParameter::%s }",
			     a->name.data(), uType( type ).data(), uTypeExtra( type ).data(),
			     isInOut( type ) ? "InOut" : "In" );
		a = f->args->next();
		if ( a )
		    fprintf( out, ",\n" );
	    }
	    fprintf( out, "\n    };\n");
	}

	fprintf( out, "    static const TQUMethod %s_%d = {", functype, list->at() );
	int n = f->args->count();
	if ( hasReturnValue )
	    n++;
	fprintf( out, "\"%s\", %d,", f->name.data(), n );
	if ( n )
	    fprintf( out, " param_%s_%d };\n", functype, list->at() );
	else
	    fprintf( out, " 0 };\n" );

	TQCString typstr = "";
	int count = 0;
	Argument *a = f->args->first();
	while ( a ) {
	    if ( !a->leftType.isEmpty() || ! a->rightType.isEmpty() ) {
		if ( count++ )
		    typstr += ",";
		typstr += a->leftType;
		typstr += a->rightType;
	    }
	    a = f->args->next();
	}
	f->signature = f->name;
	f->signature += "(";
	f->signature += typstr;
	f->signature += ")";
    }
    if ( list->count() ) {
	fprintf(out,"    static const TQMetaData %s_tbl[] = {\n", functype );
	f = list->first();
	while ( f ) {
	    fprintf( out, "\t{ \"%s\",", f->signature.data() );
	    fprintf( out, " &%s_%d,", functype, list->at() );
	    fprintf( out, " TQMetaData::%s }", f->accessAsString() );
	    f = list->next();
	    if ( f )
		fprintf( out, ",\n");
	}
	fprintf( out, "\n    };\n" );
    }
}


int enumIndex( const char* type )
{
    int index = 0;
    for( TQPtrListIterator<Enum> lit( g->enums ); lit.current(); ++lit ) {
	if ( lit.current()->name == type )
	    return index;
	index++;
    }
    return -1;
}

bool isEnumType( const char* type )
{
    return enumIndex( type ) >= 0 ||  ( g->qtEnums.contains( type ) || g->qtSets.contains( type ) );
}

bool isPropertyType( const char* type )
{
    if ( isVariantType( type ) )
	return true;

    return isEnumType( type );
}

int generateEnums()
{
    if ( g->enums.count() == 0 )
	return 0;

    fprintf( out, "#ifndef TQT_NO_PROPERTIES\n" );
    int i = 0;
    for ( TQPtrListIterator<Enum> it( g->enums ); it.current(); ++it, ++i ) {
	fprintf( out, "    static const TQMetaEnum::Item enum_%i[] = {\n", i );
	int k = 0;
	for( TQStrListIterator eit( *it.current() ); eit.current(); ++eit, ++k ) {
	    if ( k )
		fprintf( out, ",\n" );
	    fprintf( out, "\t{ \"%s\",  (int) %s::%s }", eit.current(), (const char*) g->className, eit.current() );
	}
	fprintf( out, "\n    };\n" );
    }
    fprintf( out, "    static const TQMetaEnum enum_tbl[] = {\n" );
    i = 0;
    for ( TQPtrListIterator<Enum> it2( g->enums ); it2.current(); ++it2, ++i ) {
	if ( i )
	    fprintf( out, ",\n" );
	fprintf( out, "\t{ \"%s\", %u, enum_%i, %s }",
		 (const char*)it2.current()->name,
		 it2.current()->count(),
		 i,
		 it2.current()->set ? "true" : "false" );
    }
    fprintf( out, "\n    };\n" );
    fprintf( out, "#endif // TQT_NO_PROPERTIES\n" );

    return g->enums.count();
}

int generateProps()
{
    //
    // Resolve and verify property access functions
    //
    for( TQPtrListIterator<Property> it( g->props ); it.current(); ) {
	Property* p = it.current();
	++it;

	// verify get function
	if ( !p->get.isEmpty() ) {
	    FuncList candidates = g->propfuncs.find( p->get );
	    for ( Function* f = candidates.first(); f; f = candidates.next() ) {
		if ( f->qualifier != "const" ) // get functions must be const
		    continue;
		if ( f->args && !f->args->isEmpty() ) // and must not take any arguments
		    continue;
		TQCString tmp = f->type;
		Property::Specification spec = Property::Unspecified;
		if ( p->type == "TQCString" && (tmp == "const char*" || tmp == "const char *" ) ) {
		    tmp = "TQCString";
		    spec = Property::ConstCharStar;
		} else if ( tmp.right(1) == "&" ) {
		    tmp = tmp.left( tmp.length() - 1 );
		    spec = Property::Reference;
		} else if ( tmp.right(1) == "*" ) {
		    tmp = tmp.left( tmp.length() - 1 );
		    spec = Property::Pointer;
		} else {
		    spec = Property::Class;
		}
		if ( tmp.left(6) == "const " )
		    tmp = tmp.mid( 6, tmp.length() - 6 );
		tmp = tmp.simplifyWhiteSpace();
		if ( p->type == tmp ) {
		    // If it is an enum then it may not be a set
		    bool ok = true;
		    for( TQPtrListIterator<Enum> lit( g->enums ); lit.current(); ++lit )
			if ( lit.current()->name == p->type && lit.current()->set )
			    ok = false;
		    if ( !ok ) continue;
		    p->gspec = spec;
		    p->getfunc = f;
		    p->oredEnum = 0;
		    break;
		}
		else if ( !isVariantType( p->type ) ) {
		    if ( tmp == "int" || tmp == "uint" || tmp == "unsigned int" ) {
			// Test whether the enum is really a set (unfortunately we don't know enums of super classes)
			bool ok = true;
			for( TQPtrListIterator<Enum> lit( g->enums ); lit.current(); ++lit )
			    if ( lit.current()->name == p->type && !lit.current()->set )
				ok = false;
			if ( !ok ) continue;
			p->gspec = spec;
			p->getfunc = f;
			p->oredEnum = 1;
			p->enumgettype = tmp;
		    }
		}
	    }
	    if ( p->getfunc == 0 ) {
		if ( displayWarnings ) {

		    // Is the type a set, that means, mentioned in TQ_SETS?
		    bool set = false;
		    for( TQPtrListIterator<Enum> lit( g->enums ); lit.current(); ++lit )
			if ( lit.current()->name == p->type && lit.current()->set )
			    set = true;

		    fprintf( stderr, ErrorFormatString" Warning: Property '%s' not available.\n",
			     g->fileName.data(), p->lineNo, (const char*) p->name );
		    fprintf( stderr, "   Have been looking for public get functions \n");
		    if ( !set ) {
			fprintf( stderr,
			     "      %s %s() const\n"
			     "      %s& %s() const\n"
			     "      const %s& %s() const\n"
			     "      %s* %s() const\n",
			     (const char*) p->type, (const char*) p->get,
			     (const char*) p->type, (const char*) p->get,
			     (const char*) p->type, (const char*) p->get,
			     (const char*) p->type, (const char*) p->get );
		    }
		    if ( set || !isPropertyType( p->type ) ) {
			fprintf( stderr,
			     "      int %s() const\n"
			     "      uint %s() const\n"
			     "      unsigned int %s() const\n",
			     (const char*) p->get,
			     (const char*) p->get,
			     (const char*) p->get );
		    }
		    if ( p->type == "TQCString" )
			fprintf( stderr, "      const char* %s() const\n",
				 (const char*)p->get );

		    if ( candidates.isEmpty() ) {
			fprintf( stderr, "   but found nothing.\n");
		    } else {
			fprintf( stderr, "   but only found the mismatching candidate(s)\n");
			for ( Function* f = candidates.first(); f; f = candidates.next() ) {
			    TQCString typstr = "";
			    Argument *a = f->args->first();
			    int count = 0;
			    while ( a ) {
				if ( !a->leftType.isEmpty() || ! a->rightType.isEmpty() ) {
				    if ( count++ )
					typstr += ",";
				    typstr += a->leftType;
				    typstr += a->rightType;
				}
				a = f->args->next();
			    }
			    fprintf( stderr, "      %s:%d: %s %s(%s) %s\n", g->fileName.data(), f->lineNo,
				     (const char*) f->type,(const char*) f->name, (const char*) typstr,
				     f->qualifier.isNull()?"":(const char*) f->qualifier );
			}
		    }
		}
	    }
	}

	// verify set function
	if ( !p->set.isEmpty() ) {
	    FuncList candidates = g->propfuncs.find( p->set );
	    for ( Function* f = candidates.first(); f; f = candidates.next() ) {
		if ( !f->args || f->args->isEmpty() )
		    continue;
		TQCString tmp = f->args->first()->leftType;
		tmp = tmp.simplifyWhiteSpace();
		Property::Specification spec = Property::Unspecified;
		if ( tmp.right(1) == "&" ) {
		    tmp = tmp.left( tmp.length() - 1 );
		    spec = Property::Reference;
		}
		else {
		    spec = Property::Class;
		}
		if ( p->type == "TQCString" && (tmp == "const char*" || tmp == "const char *" ) ) {
		    tmp = "TQCString";
		    spec = Property::ConstCharStar;
		}
		if ( tmp.left(6) == "const " )
		    tmp = tmp.mid( 6, tmp.length() - 6 );
		tmp = tmp.simplifyWhiteSpace();

		if ( p->type == tmp && f->args->count() == 1 ) {
		    // If it is an enum then it may not be a set
		    if ( p->oredEnum == 1 )
			continue;
		    bool ok = true;
		    for( TQPtrListIterator<Enum> lit( g->enums ); lit.current(); ++lit )
			if ( lit.current()->name == p->type && lit.current()->set )
			    ok = false;
		    if ( !ok ) continue;
		    p->sspec = spec;
		    p->setfunc = f;
		    p->oredEnum = 0;
		    break;
		} else if ( !isVariantType( p->type ) && f->args->count() == 1 ) {
		    if ( tmp == "int" || tmp == "uint" || tmp == "unsigned int" ) {
			if ( p->oredEnum == 0 )
			    continue;
			// Test wether the enum is really a set (unfortunately we don't know enums of super classes)
			bool ok = true;
			for( TQPtrListIterator<Enum> lit( g->enums ); lit.current(); ++lit )
			    if ( lit.current()->name == p->type && !lit.current()->set )
				ok = false;
			if ( !ok ) continue;
			p->sspec = spec;
			p->setfunc = f;
			p->oredEnum = 1;
			p->enumsettype = tmp;
		    }
		}
	    }
	    if ( p->setfunc == 0 ) {
		if ( displayWarnings ) {

		    // Is the type a set, that means, mentioned in TQ_SETS ?
		    bool set = false;
		    for( TQPtrListIterator<Enum> lit( g->enums ); lit.current(); ++lit )
			if ( lit.current()->name == p->type && lit.current()->set )
			    set = true;

		    fprintf( stderr, ErrorFormatString" Warning: Property '%s' not writable.\n",
			     g->fileName.data(), p->lineNo, (const char*) p->name );
		    fprintf( stderr, "   Have been looking for public set functions \n");
		    if ( !set && p->oredEnum != 1 ) {
			fprintf( stderr,
			     "      void %s( %s )\n"
			     "      void %s( %s& )\n"
			     "      void %s( const %s& )\n",
			     (const char*) p->set, (const char*) p->type,
			     (const char*) p->set, (const char*) p->type,
			     (const char*) p->set, (const char*) p->type );
		    }
		    if ( set || ( !isPropertyType( p->type ) && p->oredEnum != 0 ) ) {
			fprintf( stderr,
			     "      void %s( int )\n"
			     "      void %s( uint )\n"
			     "      void %s( unsigned int )\n",
			     (const char*) p->set,
			     (const char*) p->set,
			     (const char*) p->set );
		    }

		    if ( p->type == "TQCString" )
			fprintf( stderr, "      void %s( const char* ) const\n",
				 (const char*) p->set );

		    if ( !candidates.isEmpty() ) {
			fprintf( stderr, "   but only found the mismatching candidate(s)\n");
			for ( Function* f = candidates.first(); f; f = candidates.next() ) {
			    TQCString typstr = "";
			    Argument *a = f->args->first();
			    int count = 0;
			    while ( a ) {
				if ( !a->leftType.isEmpty() || ! a->rightType.isEmpty() ) {
				    if ( count++ )
					typstr += ",";
				    typstr += a->leftType;
				    typstr += a->rightType;
				}
				a = f->args->next();
			    }
			    fprintf( stderr, "      %s:%d: %s %s(%s)\n", g->fileName.data(), f->lineNo,
				     (const char*) f->type,(const char*) f->name, (const char*) typstr );
			}
		    }
		}
	    }
	}
    }

    //
    // Create meta data
    //
    if ( g->props.count() )   {
	if ( displayWarnings && !TQ_OBJECTdetected )
		moc_err("The declaration of the class \"%s\" contains properties"
		" but no TQ_OBJECT macro.", g->className.data());

	fprintf( out, "#ifndef TQT_NO_PROPERTIES\n" );

	fprintf( out, "    static const TQMetaProperty props_tbl[%d] = {\n ", g->props.count() );
	for( TQPtrListIterator<Property> it( g->props ); it.current(); ++it ) {

	    fprintf( out, "\t{ \"%s\",\"%s\", ", it.current()->type.data(), it.current()->name.data() );
	    int flags = Invalid;
	    if ( !isVariantType( it.current()->type ) ) {
		flags |= EnumOrSet;
		if ( !isEnumType( it.current()->type ) )
		    flags |= UnresolvedEnum;
	    } else {
		flags |= qvariant_nameToType( it.current()->type ) << 24;
	    }
	    if ( it.current()->getfunc )
		flags |= Readable;
	    if ( it.current()->setfunc ) {
		flags |= Writable;
		if ( it.current()->stdSet() )
		    flags |= StdSet;
	    }
	    if ( it.current()->override )
		flags |= Override;

	    if ( it.current()->designable.isEmpty() )
		flags |= DesignableOverride;
	    else if ( it.current()->designable == "false" )
		flags |= NotDesignable;

	    if ( it.current()->scriptable.isEmpty() )
		flags |= ScriptableOverride;
	    else if ( it.current()->scriptable == "false" )
		flags |= NotScriptable;

	    if ( it.current()->stored.isEmpty() )
		flags |= StoredOverride;
	    else if ( it.current()->stored == "false" )
		flags |= NotStored;


	    fprintf( out, "0x%.4x, ", flags );
	    fprintf( out, "&%s::metaObj, ", (const char*) qualifiedClassName() );
	    if ( !isVariantType( it.current()->type ) ) {
		int enumpos = -1;
		int k = 0;
		for( TQPtrListIterator<Enum> eit( g->enums ); eit.current(); ++eit, ++k ) {
		    if ( eit.current()->name == it.current()->type )
			enumpos = k;
		}

		// Is it an enum of this class ?
		if ( enumpos != -1 )
		    fprintf( out, "&enum_tbl[%i], ", enumpos );
		else
		    fprintf( out, "0, ");
	    } else {
		fprintf( out, "0, ");
	    }
	    fprintf( out, "-1 }" );
	    if ( !it.atLast() )
		fprintf( out, ",\n" );
	    else
		fprintf( out, "\n" );
	}
	fprintf( out, "    };\n" );
	fprintf( out, "#endif // TQT_NO_PROPERTIES\n" );
    }

    return g->props.count();
}



int generateClassInfos()
{
    if ( g->infos.isEmpty() )
	return 0;

    if ( displayWarnings && !TQ_OBJECTdetected )
	moc_err("The declaration of the class \"%s\" contains class infos"
		" but no TQ_OBJECT macro.", g->className.data());

    fprintf( out, "    static const TQClassInfo classinfo_tbl[] = {\n" );
    int i = 0;
    for( TQPtrListIterator<ClassInfo> it( g->infos ); it.current(); ++it, ++i ) {
	if ( i )
	    fprintf( out, ",\n" );
	fprintf( out, "\t{ \"%s\", \"%s\" }", it.current()->name.data(),it.current()->value.data() );
    }
    fprintf( out, "\n    };\n" );
    return i;
}


void generateClass()		      // generate C++ source code for a class
{
    const char *hdr1 = "/****************************************************************************\n"
		 "** %s meta object code from reading C++ file '%s'\n**\n";
    const char *hdr2 = "** Created by: The TQt Meta Object Compiler version %d (TQt %s)\n";
    const char *hdr3 = "** WARNING! All changes made in this file will be lost!\n";
    const char *hdr4 = "*****************************************************************************/\n\n";
    int   i;

    if ( skipClass )				// don't generate for class
	return;

    if ( !TQ_OBJECTdetected ) {
	if ( g->signals.count() == 0 && g->slots.count() == 0 && g->props.count() == 0 && g->infos.count() == 0 )
	    return;
	if ( displayWarnings && (g->signals.count() + g->slots.count()) != 0 )
	    moc_err("The declaration of the class \"%s\" contains signals "
		    "or slots\n\t but no TQ_OBJECT macro.", g->className.data());
    } else {
	if ( g->superClassName.isEmpty() )
	    moc_err("The declaration of the class \"%s\" contains the\n"
		    "\tTQ_OBJECT macro but does not inherit from any class!\n"
		    "\tInherit from TQObject or one of its descendants"
		    " or remove TQ_OBJECT.", g->className.data() );
    }
    if ( templateClass ) {			// don't generate for class
	moc_err( "Sorry, TQt does not support templates that contain\n"
		 "\tsignals, slots or TQ_OBJECT." );
	return;
    }
    g->generatedCode = true;
    g->gen_count++;

    if ( g->gen_count == 1 ) {			// first class to be generated
	TQCString fn = g->fileName;
	i = g->fileName.length()-1;
	while ( i>0 && g->fileName[i-1] != '/' && g->fileName[i-1] != '\\' )
	    i--;				// skip path
	if ( i >= 0 )
	    fn = &g->fileName[i];
	fprintf( out, hdr1, (const char*)qualifiedClassName(),(const char*)fn);
	fprintf( out, hdr2, formatRevision, TQT_VERSION_STR );
	fprintf( out, "%s", hdr3 );
	fprintf( out, "%s", hdr4 );

	if ( !g->noInclude ) {
	    /*
	      The header file might be a TQt header file with
	      TQT_NO_COMPAT macros around signals, slots or
	      properties. Without the #undef, we cannot compile the
	      TQt library with TQT_NO_COMPAT defined.

	      Header files of libraries build around TQt can also use
	      TQT_NO_COMPAT, so this #undef might be beneficial to
	      users of TQt, and not only to developers of TQt.
	    */
	    fprintf( out, "#undef TQT_NO_COMPAT\n" );

	    if ( !g->pchFile.isEmpty() )
	    	fprintf( out, "#include \"%s\" // PCH include\n", (const char*)g->pchFile );
	    if ( !g->includePath.isEmpty() && g->includePath.right(1) != "/" )
		g->includePath += "/";

	    g->includeFiles.first();
	    while ( g->includeFiles.current() ) {
		TQCString inc = g->includeFiles.current();
		if ( inc[0] != '<' && inc[0] != '"' ) {
		    if ( !g->includePath.isEmpty() && g->includePath != "./" )
			inc.prepend( g->includePath );
		    inc = "\"" + inc + "\"";
		}
		fprintf( out, "#include %s\n", (const char *)inc );
		g->includeFiles.next();
	    }
	}
	fprintf( out, "#include <%sntqmetaobject.h>\n", (const char*)g->qtPath );
	fprintf( out, "#include <%sntqapplication.h>\n\n", (const char*)g->qtPath );
	fprintf( out, "#include <%sprivate/qucomextra_p.h>\n", (const char*)g->qtPath );
	fprintf( out, "#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != %d)\n", formatRevision );
	fprintf( out, "#error \"This file was generated using the moc from %s."
		 " It\"\n#error \"cannot be used with the include files from"
		 " this version of TQt.\"\n#error \"(The moc has changed too"
		 " much.)\"\n", TQT_VERSION_STR );
	fprintf( out, "#endif\n\n" );
    } else {
	fprintf( out, "\n\n" );
    }

    if ( !g->hasVariantIncluded ) {
	bool needToIncludeVariant = !g->props.isEmpty();
	for ( Function* f =g->slots.first(); f && !needToIncludeVariant; f=g->slots.next() )
	    needToIncludeVariant = ( f->type != "void" && !validUType( f->type ) && isVariantType( f->type) );

	if ( needToIncludeVariant ) {
	    fprintf( out, "#include <%sntqvariant.h>\n", (const char*)g->qtPath );
	    g->hasVariantIncluded = true;
	}
    }

    bool isTQObject =  g->className == "TQObject" ;


//
// Generate virtual function className()
//
    fprintf( out, "const char *%s::className() const\n{\n    ",
	     (const char*)qualifiedClassName() );
    fprintf( out, "return \"%s\";\n}\n\n", (const char*)qualifiedClassName() );

//
// Generate static metaObj variable
//
    fprintf( out, "TQMetaObject *%s::metaObj = 0;\n", (const char*)qualifiedClassName());

//
// Generate static cleanup object variable
//
    TQCString cleanup = qualifiedClassName().copy();
    for ( int cnpos = 0; cnpos < cleanup.length(); cnpos++ ) {
	if ( cleanup[cnpos] == ':' )
	    cleanup[cnpos] = '_';
    }

    fprintf( out, "static TQMetaObjectCleanUp cleanUp_%s( \"%s\", &%s::staticMetaObject );\n\n", (const char*)cleanup, (const char*)qualifiedClassName(), (const char*)qualifiedClassName() );

//
// Generate tr and trUtf8 member functions
//
    fprintf( out, "#ifndef TQT_NO_TRANSLATION\n" );
    fprintf( out, "TQString %s::tr( const char *s, const char *c )\n{\n",
	     (const char*)qualifiedClassName() );
    fprintf( out, "    if ( tqApp )\n" );
    fprintf( out, "\treturn tqApp->translate( \"%s\", s, c,"
		  " TQApplication::DefaultCodec );\n",
	     (const char*)qualifiedClassName() );
    fprintf( out, "    else\n" );
    fprintf( out, "\treturn TQString::fromLatin1( s );\n");
    fprintf( out, "}\n" );
    fprintf( out, "#ifndef TQT_NO_TRANSLATION_UTF8\n" );
    fprintf( out, "TQString %s::trUtf8( const char *s, const char *c )\n{\n",
	     (const char*)qualifiedClassName() );
    fprintf( out, "    if ( tqApp )\n" );
    fprintf( out, "\treturn tqApp->translate( \"%s\", s, c,"
		  " TQApplication::UnicodeUTF8 );\n",
	     (const char*)qualifiedClassName() );
    fprintf( out, "    else\n" );
    fprintf( out, "\treturn TQString::fromUtf8( s );\n" );
    fprintf( out, "}\n" );
    fprintf( out, "#endif // TQT_NO_TRANSLATION_UTF8\n\n" );
    fprintf( out, "#endif // TQT_NO_TRANSLATION\n\n" );

//
// Generate staticMetaObject member function
//
    fprintf( out, "TQMetaObject* %s::staticMetaObject()\n{\n", (const char*)qualifiedClassName() );
    fprintf( out, "    if ( metaObj ) {\n\treturn metaObj;\n}\n" );
    fprintf( out, "#ifdef TQT_THREAD_SUPPORT\n    if (tqt_sharedMetaObjectMutex) tqt_sharedMetaObjectMutex->lock();\n" );
    fprintf( out, "    if ( metaObj ) {\n\tif (tqt_sharedMetaObjectMutex) tqt_sharedMetaObjectMutex->unlock();\n\treturn metaObj;\n    }\n#endif // TQT_THREAD_SUPPORT\n" );
    if ( isTQObject )
	fprintf( out, "    TQMetaObject* parentObject = staticTQtMetaObject();\n" );
    else if ( !g->superClassName.isEmpty() )
	fprintf( out, "    TQMetaObject* parentObject = %s::staticMetaObject();\n", (const char*)g->superClassName );
    else
	fprintf( out, "    TQMetaObject* parentObject = 0;\n" );

//
// Build the classinfo array
//
   int n_infos = generateClassInfos();

// Build the enums array
// Enums HAVE to be generated BEFORE the properties and slots
//
    int n_enums = generateEnums();

//
// Build slots array in staticMetaObject()
//
    generateFuncs( &g->slots, "slot", Slot_Num );

//
// Build signals array in staticMetaObject()
//
    generateFuncs( &g->signals, "signal", Signal_Num );

//
// Build property array in staticMetaObject()
//
   int n_props = generateProps();

//
// Finally code to create and return meta object
//
    fprintf( out, "    metaObj = TQMetaObject::new_metaobject(\n"
		  "\t\"%s\", parentObject,\n", (const char*)qualifiedClassName() );

    if ( g->slots.count() )
	fprintf( out, "\tslot_tbl, %d,\n", g->slots.count() );
    else
	fprintf( out, "\t0, 0,\n" );

    if ( g->signals.count() )
	fprintf( out, "\tsignal_tbl, %d,\n", g->signals.count() );
    else
	fprintf( out, "\t0, 0,\n" );

    fprintf( out, "#ifndef TQT_NO_PROPERTIES\n" );
    if ( n_props )
	fprintf( out, "\tprops_tbl, %d,\n", n_props );
    else
	fprintf( out, "\t0, 0,\n" );
    if ( n_enums )
	fprintf( out, "\tenum_tbl, %d,\n", n_enums );
    else
	fprintf( out, "\t0, 0,\n" );
    fprintf( out, "#endif // TQT_NO_PROPERTIES\n" );

    if ( n_infos )
	fprintf( out, "\tclassinfo_tbl, %d );\n", n_infos );
    else
	fprintf( out, "\t0, 0 );\n" );


//
// Setup cleanup handler and return meta object
//
    fprintf( out, "    cleanUp_%s.setMetaObject( metaObj );\n", cleanup.data() );
    fprintf( out, "#ifdef TQT_THREAD_SUPPORT\n    if (tqt_sharedMetaObjectMutex) tqt_sharedMetaObjectMutex->unlock();\n#endif // TQT_THREAD_SUPPORT\n" );
    fprintf( out, "    return metaObj;\n}\n" );

//
// End of function staticMetaObject()
//

//
// Generate smart cast function
//
    fprintf( out, "\nvoid* %s::tqt_cast( const char* clname )\n{\n",
	     (const char*)qualifiedClassName() );
    fprintf( out, "    if ( !qstrcmp( clname, \"%s\" ) )\n"
		  "\treturn this;\n",
	     (const char*)qualifiedClassName() );
    for ( const char* cname = g->multipleSuperClasses.first(); cname; cname = g->multipleSuperClasses.next() ) {
	fprintf( out, "    if ( !qstrcmp( clname, \"%s\" ) )\n", cname);
        TQCString fixed(cname);
        while (fixed.find(">>") != -1)
            fixed = fixed.replace(">>", "> >");
        fprintf( out, "\treturn (%s*)this;\n", fixed.data());
    }
    if ( !g->superClassName.isEmpty() && !isTQObject )
	fprintf( out, "    return %s::tqt_cast( clname );\n",
		 (const char*)purestSuperClassName() );
    else
	fprintf( out, "    return 0;\n" );
    fprintf( out, "}\n" );

//
// Generate internal signal functions
//
    Function *f;
    f = g->signals.first();			// make internal signal methods
    static bool included_list_headers = false;
    int sigindex = 0;
    while ( f ) {
	TQCString argstr;
	char buf[12];
	Argument *a = f->args->first();
	int offset = 0;
	const char *predef_call_func = 0;
	bool hasReturnValue = f->type != "void" && (validUType( f->type ) || isVariantType( f->type) );
	if ( hasReturnValue ) {
	    ; // no predefined function available
	} else if ( !a ) {
	    predef_call_func = "activate_signal";
	} else if ( f->args->count() == 1 ) {
	    TQCString ctype = (a->leftType + ' ' + a->rightType).simplifyWhiteSpace();
	    if ( !isInOut( ctype ) ) {
		TQCString utype = uType( ctype );
		if ( utype == "bool" )
		    predef_call_func = "activate_signal_bool";
		else if ( utype == "TQString" || utype == "int" || utype == "double"  )
		    predef_call_func = "activate_signal";
	    }
	}

	if ( !predef_call_func && !included_list_headers ) {
	    // yes we need it, because otherwise TQT_VERSION may not be defined
	    fprintf( out, "\n#include <%sntqobjectdefs.h>\n", (const char*)g->qtPath );
	    fprintf( out, "#include <%sntqsignalslotimp.h>\n", (const char*)g->qtPath );
	    included_list_headers = true;
	}

	while ( a ) { // argument list
	    if ( !a->leftType.isEmpty() || !a->rightType.isEmpty() ) {
		argstr += a->leftType;
		argstr += " ";
		sprintf( buf, "t%d", offset++ );
		argstr += buf;
		argstr += a->rightType;
		a = f->args->next();
		if ( a )
		    argstr += ", ";
	    } else {
		a = f->args->next();
	    }
	}

	fixRightAngles( &argstr );

	fprintf( out, "\n// TQ_SIGNAL %s\n", (const char*)f->name );
	fprintf( out, "%s %s::%s(", (const char*) f->type,
		 (const char*)qualifiedClassName(),
		 (const char*)f->name );

	if ( argstr.isEmpty() )
	    fprintf( out, ")\n{\n" );
	else
	    fprintf( out, " %s )\n{\n", (const char*)argstr );

	if ( predef_call_func ) {
	    fprintf( out, "    %s( staticMetaObject()->signalOffset() + %d", predef_call_func, sigindex );
	    if ( !argstr.isEmpty() )
		fprintf( out, ", t0" );
	    fprintf( out, " );\n}\n" );
	} else {
	    if ( hasReturnValue )
		fprintf( out, "    %s something;\n", f->type.data() );
	    int nargs = f->args->count();
	    fprintf( out, "    if ( signalsBlocked() )\n\treturn%s;\n", hasReturnValue ? " something" : "" );
	    fprintf( out, "    TQConnectionList *clist = receivers( staticMetaObject()->signalOffset() + %d );\n",
		     sigindex );
	    fprintf( out, "    if ( !clist )\n\treturn%s;\n", hasReturnValue ? " something" : "" );
	    fprintf( out, "    TQUObject o[%d];\n", f->args->count() + 1 );

	    // initialize return value to something
	    if ( hasReturnValue ) {
		if ( validUType( f->type ) ) {
		    TQCString utype = uType( f->type );
		    fprintf( out, "    static_QUType_%s.set(o,something);\n", utype.data() );
		} else if ( uType( f->type ) == "varptr" ) {
		    fprintf( out, "    static_QUType_varptr.set(o,&something);\n" );
		} else {
		    fprintf( out, "    static_QUType_ptr.set(o,&something);\n" );
		}
	    }

	    // initialize arguments
	    if ( !f->args->isEmpty() ) {
		offset = 0;
		Argument* a = f->args->first();
		while ( a ) {
		    TQCString type = a->leftType + ' ' + a->rightType;
		    type = type.simplifyWhiteSpace();
		    if ( validUType( type ) ) {
			TQCString utype = uType( type );
			fprintf( out, "    static_QUType_%s.set(o+%d,t%d);\n", utype.data(), offset+1, offset );
		    } else if ( uType( type ) == "varptr" ) {
			fprintf( out, "    static_QUType_varptr.set(o+%d,&t%d);\n", offset+1, offset );
		    } else {
			fprintf( out, "    static_QUType_ptr.set(o+%d,&t%d);\n", offset+1, offset );
		    }
		    a = f->args->next();
		    offset++;
		}
	    }
	    fprintf( out, "    o[%d].isLastObject = true;\n", f->args->count() + 0 );
	    fprintf( out, "    activate_signal( clist, o );\n" );

	    // get return values from inOut parameters
	    if ( !f->args->isEmpty() ) {
		offset = 0;
		Argument* a = f->args->first();
		while ( a ) {
		    TQCString type = a->leftType + ' ' + a->rightType;
		    type = type.simplifyWhiteSpace();
		    if ( validUType( type ) && isInOut( type ) ) {
			TQCString utype = uType( type );
			if ( utype == "enum" )
			    fprintf( out, "    t%d = (%s)static_QUType_%s.get(o+%d);\n", offset, type.data(), utype.data(), offset+1 );
			else if ( utype == "ptr" && type.right(2) == "**" )
			    fprintf( out, "    if (t%d) *t%d = *(%s)static_QUType_ptr.get(o+%d);\n", offset, offset, type.data(), offset+1 );
			else
			    fprintf( out, "    t%d = static_QUType_%s.get(o+%d);\n", offset, utype.data(), offset+1 );
		    }
		    a = f->args->next();
		    offset++;
		}
	    }

	    // get and return return value
	    if ( hasReturnValue ) {
		TQCString utype = uType( f->type );
		if ( utype == "enum" || utype == "ptr" || utype == "varptr" ) // need cast
		    fprintf( out, "    return (%s)static_QUType_%s.get(o);\n", f->type.data(), utype.data() );
		else
		    fprintf( out, "    return static_QUType_%s.get(o);\n", utype.data() );
	    }

	    fprintf( out, "}\n" );
	}

	f = g->signals.next();
	sigindex++;
    }


//
// Generate internal tqt_invoke()  function
//
    fprintf( out, "\nbool %s::tqt_invoke( int _id, TQUObject* _o )\n{\n", qualifiedClassName().data() );

    if( !g->slots.isEmpty() ) {
	fprintf( out, "    switch ( _id - staticMetaObject()->slotOffset() ) {\n" );
	int slotindex = -1;
	for ( f = g->slots.first(); f; f = g->slots.next() ) {
	    slotindex ++;
	    if ( f->type == "void" && f->args->isEmpty() ) {
		fprintf( out, "    case %d: %s(); break;\n", slotindex, f->name.data() );
		continue;
	    }

	    fprintf( out, "    case %d: ", slotindex );
	    bool hasReturnValue = false;
	    bool hasVariantReturn = false;
	    if ( f->type != "void" )  {
		if (  validUType( f->type )) {
		    hasReturnValue = true;
		    fprintf( out, "static_QUType_%s.set(_o,", uType(f->type).data() );
		} else if ( isVariantType( f->type ) ) {
		    hasReturnValue = hasVariantReturn = true;
		    // do not need special handling for bool since this is handled as utype
		    fprintf( out, "static_QUType_TQVariant.set(_o,TQVariant(" );
		}
	    }
	    int offset = 0;
	    fprintf( out, "%s(", f->name.data() );
	    Argument* a = f->args->first();
	    while ( a ) {
		TQCString type = a->leftType + ' ' + a->rightType;
		type = type.simplifyWhiteSpace();
		fixRightAngles( &type );
		if ( validUType( type ) ) {
		    TQCString utype = uType( type );
		    if ( utype == "ptr" || utype == "varptr" || utype == "enum" )
			fprintf( out, "(%s)static_QUType_%s.get(_o+%d)", type.data(), utype.data(), offset+1 );
		    else
			fprintf( out, "(%s)static_QUType_%s.get(_o+%d)", type.data(), utype.data(), offset+1 );
		} else {
		    TQCString castType = castToUType( type );
		    if(castType == type)
			fprintf( out, "(%s)(*((%s*)static_QUType_ptr.get(_o+%d)))", type.data(),
				 castType.data(), offset+1 );
		    else
			fprintf( out, "(%s)*((%s*)static_QUType_ptr.get(_o+%d))", type.data(),
				 castType.data(), offset+1 );
		}
		a = f->args->next();
		if ( a )
		    fprintf( out, "," );
		offset++;
	    }
	    fprintf( out, ")" );
	    if ( hasReturnValue )
		fprintf( out, ")" );
	    if ( hasVariantReturn )
		fprintf( out, ")" );
	    fprintf( out, "; break;\n" );
	}
	fprintf( out, "    default:\n" );

	if ( !g->superClassName.isEmpty() && !isTQObject ) {
	    fprintf( out, "\treturn %s::tqt_invoke( _id, _o );\n",
		     (const char *) purestSuperClassName() );
	} else {
	    fprintf( out, "\treturn false;\n" );
	}
	fprintf( out, "    }\n" );
	fprintf( out, "    return true;\n}\n" );
    } else {
	if ( !g->superClassName.isEmpty()  && !isTQObject )
	    fprintf( out, "    return %s::tqt_invoke(_id,_o);\n}\n",
		     (const char *) purestSuperClassName() );
	else
	    fprintf( out, "    return false;\n}\n" );
    }


//
// Generate internal tqt_emit()  function
//
    fprintf( out, "\nbool %s::tqt_emit( int _id, TQUObject* _o )\n{\n", qualifiedClassName().data() );

    if ( !g->signals.isEmpty() ) {
	fprintf( out, "    switch ( _id - staticMetaObject()->signalOffset() ) {\n" );
	int signalindex = -1;
	for ( f = g->signals.first(); f; f = g->signals.next() ) {
	    signalindex++;
	    if ( f->type == "void" && f->args->isEmpty() ) {
		fprintf( out, "    case %d: %s(); break;\n", signalindex, f->name.data() );
		continue;
	    }

	    fprintf( out, "    case %d: ", signalindex );
	    bool hasReturnValue = false;
	    if ( f->type != "void" && validUType( f->type )) {
		hasReturnValue = true;
		fprintf( out, "static_QUType_%s.set(_o,", uType(f->type).data() );
	    }
	    int offset = 0;
	    fprintf( out, "%s(", f->name.data() );
	    Argument* a = f->args->first();
	    while ( a ) {
		TQCString type = a->leftType + ' ' + a->rightType;
		type = type.simplifyWhiteSpace();
		fixRightAngles( &type );
		if ( validUType( type ) ) {
		    TQCString utype = uType( type );
		    if ( utype == "ptr" || utype == "varptr" || utype == "enum" )
			fprintf( out, "(%s)static_QUType_%s.get(_o+%d)", type.data(), utype.data(), offset+1 );
		    else
			fprintf( out, "(%s)static_QUType_%s.get(_o+%d)", type.data(), utype.data(), offset+1 );
		} else {
		    TQCString castType = castToUType( type );
		    if(castType == type)
			fprintf( out, "(%s)(*((%s*)static_QUType_ptr.get(_o+%d)))", type.data(),
				 castType.data(), offset+1 );
		    else
			fprintf( out, "(%s)*((%s*)static_QUType_ptr.get(_o+%d))", type.data(),
				 castType.data(), offset+1 );
		}
		a = f->args->next();
		if ( a )
		    fprintf( out, "," );
		offset++;
	    }
	    fprintf( out, ")" );
	    if ( hasReturnValue )
		fprintf( out, ")" );
	    fprintf( out, "; break;\n" );
	}
	fprintf( out, "    default:\n" );
	if ( !g->superClassName.isEmpty()  && !isTQObject )
	    fprintf( out, "\treturn %s::tqt_emit(_id,_o);\n",
		     (const char *) purestSuperClassName() );
	else
	    fprintf( out, "\treturn false;\n" );
	fprintf( out, "    }\n" );
	fprintf( out, "    return true;\n}\n" );
    } else {
	if ( !g->superClassName.isEmpty()  && !isTQObject )
	    fprintf( out, "    return %s::tqt_emit(_id,_o);\n}\n",
		     (const char *) purestSuperClassName() );
	else
	    fprintf( out, "    return false;\n}\n" );
    }


    fprintf( out, "#ifndef TQT_NO_PROPERTIES\n" );
//
// Generate internal tqt_property()  functions
//

    fprintf( out, "\nbool %s::tqt_property( int id, int f, TQVariant* v)\n{\n", qualifiedClassName().data() );

    if ( !g->props.isEmpty() ) {
	fprintf( out, "    switch ( id - staticMetaObject()->propertyOffset() ) {\n" );
	int propindex = -1;
	bool need_resolve = false;

	for( TQPtrListIterator<Property> it( g->props ); it.current(); ++it ){
	    propindex ++;
	    fprintf( out, "    case %d: ", propindex );
	    fprintf( out, "switch( f ) {\n" );

	    uint flag_break = 0;
	    uint flag_propagate = 0;

	    if ( it.current()->setfunc ) {
		fprintf( out, "\tcase 0: %s(", it.current()->setfunc->name.data() );
		TQCString type = it.current()->type.copy(); // detach on purpose
		if ( it.current()->oredEnum )
		    type = it.current()->enumsettype;
		if ( type == "uint" )
		    fprintf( out, "v->asUInt()" );
		else if ( type == "unsigned int" )
		    fprintf( out, "(uint)v->asUInt()" );
		else if ( type == "TQMap<TQString,TQVariant>" )
		    fprintf( out, "v->asMap()" );
		else if ( type == "TQValueList<TQVariant>" )
		    fprintf( out, "v->asList()" );
		else if ( type == "TQ_LLONG" )
		    fprintf( out, "v->asLongLong()" );
		else if ( type == "TQ_ULLONG" )
		    fprintf( out, "v->asULongLong()" );
		else if ( isVariantType( type ) ) {
		    if (( type[0] == 'T' ) && ( type[1] == 'Q' ))
			type = type.mid(2);
		    else
			type[0] = toupper( type[0] );
		    fprintf( out, "v->as%s()", type.data() );
		} else {
		    fprintf( out, "(%s&)v->asInt()", type.data() );
		}
		fprintf( out, "); break;\n" );

	    } else if ( it.current()->override ) {
		flag_propagate |= 1 << (0+1);
	    }
	    if ( it.current()->getfunc ) {
		if ( it.current()->gspec == Property::Pointer )
		    fprintf( out, "\tcase 1: if ( this->%s() ) *v = TQVariant( %s*%s() ); break;\n",
			     it.current()->getfunc->name.data(),
			     !isVariantType( it.current()->type ) ? "(int)" : "",
			     it.current()->getfunc->name.data());
		else
		    fprintf( out, "\tcase 1: *v = TQVariant( %sthis->%s() ); break;\n",
			     !isVariantType( it.current()->type ) ? "(int)" : "",
			     it.current()->getfunc->name.data());
	    } else if ( it.current()->override ) {
		flag_propagate |= 1<< (1+1);
	    }

	    if ( !it.current()->reset.isEmpty() )
		fprintf( out, "\tcase 2: this->%s(); break;\n", it.current()->reset.data() );

	    if ( it.current()->designable.isEmpty() )
		flag_propagate |= 1 << (3+1);
	    else if ( it.current()->designable == "true" )
		flag_break |= 1 << (3+1);
	    else if ( it.current()->designable != "false" )
		fprintf( out, "\tcase 3: return this->%s();\n", it.current()->designable.data() );

	    if ( it.current()->scriptable.isEmpty() )
		flag_propagate |= 1 << (4+1);
	    else if ( it.current()->scriptable == "true" )
		flag_break |= 1 << (4+1);
	    else if ( it.current()->scriptable != "false" )
		fprintf( out, "\tcase 4: return this->%s();\n", it.current()->scriptable.data() );

	    if ( it.current()->stored.isEmpty() )
		flag_propagate |= 1 << (5+1);
	    else if ( it.current()->stored == "true" )
		flag_break |= 1 << (5+1);
	    else if ( it.current()->stored != "false" )
		fprintf( out, "\tcase 5: return this->%s();\n", it.current()->stored.data() );

	    int i = 0;
	    if ( flag_propagate != 0 ) {
		fprintf( out, "\t" );
		for ( i = 0; i <= 5; i++ ) {
		    if ( flag_propagate & (1 << (i+1) ) )
			fprintf( out, "case %d: ", i );
		}
		if (!g->superClassName.isEmpty() &&  !isTQObject ) {
		    fprintf( out, "goto resolve;\n" );
		    need_resolve = true;
		} else {
		    fprintf( out, "    return false;\n" );
		}
	    }
	    if ( flag_break != 0 ) {
		fprintf( out, "\t" );
		for ( i = 0; i <= 5; i++ ) {
		    if ( flag_break & (1 << (i+1) ) )
			fprintf( out, "case %d: ", i );
		}
		fprintf( out, "break;\n");
	    }

	    fprintf( out, "\tdefault: return false;\n    } break;\n" );
	}
	fprintf( out, "    default:\n" );
	if ( !g->superClassName.isEmpty()  && !isTQObject )
	    fprintf( out, "\treturn %s::tqt_property( id, f, v );\n",
		     (const char *) purestSuperClassName() );
	else
	    fprintf( out, "\treturn false;\n" );
	fprintf( out, "    }\n" );
	fprintf( out, "    return true;\n" );

	if ( need_resolve )
	    fprintf( out, "resolve:\n    return %s::tqt_property( staticMetaObject()->resolveProperty(id), f, v );\n",
		     (const char *) purestSuperClassName() );
	fprintf( out, "}\n" );
    } else {
	if ( !g->superClassName.isEmpty() &&  !isTQObject )
	    fprintf( out, "    return %s::tqt_property( id, f, v);\n}\n",
		     (const char *) purestSuperClassName() );
	else
	    fprintf( out, "    return false;\n}\n" );
    }

    fprintf( out, "\nbool %s::tqt_static_property( TQObject* , int , int , TQVariant* ){ return false; }\n", qualifiedClassName().data() );
    fprintf( out, "#endif // TQT_NO_PROPERTIES\n" );
}


ArgList *addArg( Argument *a )			// add argument to list
{
    if ( (!a->leftType.isEmpty() || !a->rightType.isEmpty() ) )  //filter out truely void arguments
	tmpArgList->append( a );
    return tmpArgList;
}

void addEnum()
{
    // Avoid duplicates
    for( TQPtrListIterator<Enum> lit( g->enums ); lit.current(); ++lit ) {
	if ( lit.current()->name == tmpEnum->name )
	{
	    if ( displayWarnings )
		moc_err( "Enum %s defined twice.", (const char*)tmpEnum->name );
	}
    }

    // Only look at types mentioned  in TQ_ENUMS and TQ_SETS
    if ( g->qtEnums.contains( tmpEnum->name ) || g->qtSets.contains( tmpEnum->name ) )
    {
	g->enums.append( tmpEnum );
	if ( g->qtSets.contains( tmpEnum->name ) )
	    tmpEnum->set = true;
	else
	    tmpEnum->set = false;
    }
    else
	delete tmpEnum;
    tmpEnum = new Enum;
}

void addMember( Member m )
{
    if ( skipFunc ) {
	tmpFunc->args = tmpArgList; // just to be sure
	delete tmpFunc;
	tmpArgList  = new ArgList;   // ugly but works
	tmpFunc	    = new Function;
	skipFunc    = false;
	return;
    }

    tmpFunc->type = tmpFunc->type.simplifyWhiteSpace();
    tmpFunc->access = tmpAccess;
    tmpFunc->args = tmpArgList;
    tmpFunc->lineNo = lineNo;

    for ( ;; ) {
	g->funcs.append( tmpFunc );

	if ( m == SignalMember ) {
	    g->signals.append( tmpFunc );
	    break;
	} else {
	    if ( m == SlotMember )
		g->slots.append( tmpFunc );
	    // PropertyCandidateMember or SlotMember
	    if ( !tmpFunc->name.isEmpty() && tmpFunc->access == Public )
		g->propfuncs.append( tmpFunc );
	    if ( !tmpFunc->args || !tmpFunc->args->hasDefaultArguments() )
		break;
	    tmpFunc = new Function( *tmpFunc );
	    tmpFunc->args = tmpFunc->args->magicClone();
	}
    }

    skipFunc = false;
    tmpFunc = new Function;
    tmpArgList = new ArgList;
}

void checkPropertyName( const char* ident )
{
    if ( ident[0] == '_' ) {
	moc_err( "Invalid property name '%s'.", ident );
	return;
    }
}

