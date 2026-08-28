/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Linguist.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include <metatranslator.h>

#include <ntqfile.h>
#include <ntqregexp.h>
#include <ntqstring.h>
#include <ntqtextstream.h>
#include <ntqvaluestack.h>
#include <ntqxml.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

/* qmake ignore TQ_OBJECT */

static const char MagicComment[] = "TRANSLATOR ";

static TQMap<TQCString, int> needs_TQ_OBJECT;
static TQMap<TQCString, int> lacks_TQ_OBJECT;

/*
  The first part of this source file is the C++ tokenizer.  We skip
  most of C++; the only tokens that interest us are defined here.
  Thus, the code fragment

      int main()
      {
	  printf( "Hello, world!\n" );
	  return 0;
      }

  is broken down into the following tokens (Tok_ omitted):

      Ident Ident LeftParen RightParen
      LeftBrace
	  Ident LeftParen String RightParen Semicolon
	  return Semicolon
      RightBrace.

  The 0 doesn't produce any token.
*/

enum { Tok_Eof, Tok_class, Tok_namespace, Tok_return, Tok_tr,
       Tok_trUtf8, Tok_translate, Tok_TQ_OBJECT, Tok_Ident,
       Tok_Comment, Tok_String, Tok_Arrow, Tok_Colon,
       Tok_Gulbrandsen, Tok_LeftBrace, Tok_RightBrace, Tok_LeftParen,
       Tok_RightParen, Tok_Comma, Tok_Semicolon };

/*
  The tokenizer maintains the following global variables. The names
  should be self-explanatory.
*/
static TQCString yyFileName;
static int yyCh;
static char yyIdent[128];
static size_t yyIdentLen;
static char yyComment[65536];
static size_t yyCommentLen;
static char yyString[65536];
static size_t yyStringLen;
static TQValueStack<int> yySavedBraceDepth;
static TQValueStack<int> yySavedParenDepth;
static int yyBraceDepth;
static int yyParenDepth;
static int yyLineNo;
static int yyCurLineNo;
static int yyBraceLineNo;
static int yyParenLineNo;

// the file to read from (if reading from a file)
static FILE *yyInFile;

// the string to read from and current position in the string (otherwise)
static TQString yyInStr;
static int yyInPos;

static int (*getChar)();

static int getCharFromFile()
{
    int c = getc( yyInFile );
    if ( c == '\n' )
	yyCurLineNo++;
    return c;
}

static int getCharFromString()
{
    if ( yyInPos == (int) yyInStr.length() ) {
	return EOF;
    } else {
	return yyInStr[yyInPos++].latin1();
    }
}

static void startTokenizer( const char *fileName, int (*getCharFunc)() )
{
    yyInPos = 0;
    getChar = getCharFunc;

    yyFileName = fileName;
    yyCh = getChar();
    yySavedBraceDepth.clear();
    yySavedParenDepth.clear();
    yyBraceDepth = 0;
    yyParenDepth = 0;
    yyCurLineNo = 1;
    yyBraceLineNo = 1;
    yyParenLineNo = 1;
}

static int getToken()
{
    const char tab[] = "abfnrtv";
    const char backTab[] = "\a\b\f\n\r\t\v";
    uint n;

    yyIdentLen = 0;
    yyCommentLen = 0;
    yyStringLen = 0;

    while ( yyCh != EOF ) {
	yyLineNo = yyCurLineNo;

	if ( isalpha(yyCh) || yyCh == '_' ) {
	    do {
		if ( yyIdentLen < sizeof(yyIdent) - 1 )
		    yyIdent[yyIdentLen++] = (char) yyCh;
		yyCh = getChar();
	    } while ( isalnum(yyCh) || yyCh == '_' );
	    yyIdent[yyIdentLen] = '\0';

	    switch ( yyIdent[0] ) {
	    case 'Q':
		if ( strcmp(yyIdent + 1, "_OBJECT") == 0 ) {
		    return Tok_TQ_OBJECT;
		} else if ( strcmp(yyIdent + 1, "T_TR_NOOP") == 0 ) {
		    return Tok_tr;
		} else if ( strcmp(yyIdent + 1, "T_TRANSLATE_NOOP") == 0 ) {
		    return Tok_translate;
		}
		break;
	    case 'T':
		// TR() for when all else fails
		if ( tqstricmp(yyIdent + 1, "R") == 0 )
		    return Tok_tr;
		break;
	    case 'c':
		if ( strcmp(yyIdent + 1, "lass") == 0 )
		    return Tok_class;
		break;
	    case 'f':
		/*
		  TQTranslator::findMessage() has the same parameters as
		  TQApplication::translate().
		*/
		if ( strcmp(yyIdent + 1, "indMessage") == 0 )
		    return Tok_translate;
		break;
	    case 'n':
		if ( strcmp(yyIdent + 1, "amespace") == 0 )
		    return Tok_namespace;
		break;
	    case 'r':
		if ( strcmp(yyIdent + 1, "eturn") == 0 )
		    return Tok_return;
		break;
	    case 's':
		if ( strcmp(yyIdent + 1, "truct") == 0 )
		    return Tok_class;
		break;
	    case 't':
		if ( strcmp(yyIdent + 1, "r") == 0 ) {
		    return Tok_tr;
		} else if ( qstrcmp(yyIdent + 1, "rUtf8") == 0 ) {
		    return Tok_trUtf8;
		} else if ( qstrcmp(yyIdent + 1, "ranslate") == 0 ) {
		    return Tok_translate;
		}
	    }
	    return Tok_Ident;
	} else {
	    switch ( yyCh ) {
	    case '#':
		/*
		  Early versions of lupdate complained about
		  unbalanced braces in the following code:

		      #ifdef ALPHA
			  while ( beta ) {
		      #else
			  while ( gamma ) {
		      #endif
			      delta;
			  }

		  The code contains, indeed, two opening braces for
		  one closing brace; yet there's no reason to panic.

		  The solution is to remember yyBraceDepth as it was
		  when #if, #ifdef or #ifndef was met, and to set
		  yyBraceDepth to that value when meeting #elif or
		  #else.
		*/
		do {
		    yyCh = getChar();
		} while ( isspace(yyCh) && yyCh != '\n' );

		switch ( yyCh ) {
		case 'i':
		    yyCh = getChar();
		    if ( yyCh == 'f' ) {
			// if, ifdef, ifndef
			yySavedBraceDepth.push( yyBraceDepth );
                        yySavedParenDepth.push( yyParenDepth );
		    }
		    break;
		case 'e':
		    yyCh = getChar();
		    if ( yyCh == 'l' ) {
			// elif, else
			if ( !yySavedBraceDepth.isEmpty() ) {
			    yyBraceDepth = yySavedBraceDepth.top();
                            yyParenDepth = yySavedParenDepth.top();
			}
		    } else if ( yyCh == 'n' ) {
			// endif
			if ( !yySavedBraceDepth.isEmpty() ) {
			    yySavedBraceDepth.pop();
                            yySavedParenDepth.pop();
			}
		    }
		}
		while ( isalnum(yyCh) || yyCh == '_' )
		    yyCh = getChar();
		break;
	    case '/':
		yyCh = getChar();
		if ( yyCh == '/' ) {
		    do {
			yyCh = getChar();
		    } while ( yyCh != EOF && yyCh != '\n' );
		} else if ( yyCh == '*' ) {
		    bool metAster = false;
		    bool metAsterSlash = false;

		    while ( !metAsterSlash ) {
			yyCh = getChar();
			if ( yyCh == EOF ) {
			    fprintf( stderr,
				     "%s: Unterminated C++ comment starting at"
				     " line %d\n",
				     (const char *) yyFileName, yyLineNo );
			    yyComment[yyCommentLen] = '\0';
			    return Tok_Comment;
			}
			if ( yyCommentLen < sizeof(yyComment) - 1 )
			    yyComment[yyCommentLen++] = (char) yyCh;

			if ( yyCh == '*' )
			    metAster = true;
			else if ( metAster && yyCh == '/' )
			    metAsterSlash = true;
			else
			    metAster = false;
		    }
		    yyCh = getChar();
		    yyCommentLen -= 2;
		    yyComment[yyCommentLen] = '\0';
		    return Tok_Comment;
		}
		break;
	    case '"':
		yyCh = getChar();

		while ( yyCh != EOF && yyCh != '\n' && yyCh != '"' ) {
		    if ( yyCh == '\\' ) {
			yyCh = getChar();

			if ( yyCh == '\n' ) {
			    yyCh = getChar();
			} else if ( yyCh == 'x' ) {
			    TQCString hex = "0";

			    yyCh = getChar();
			    while ( isxdigit(yyCh) ) {
				hex += (char) yyCh;
				yyCh = getChar();
			    }
			    sscanf( hex, "%x", &n );
			    if ( yyStringLen < sizeof(yyString) - 1 )
				yyString[yyStringLen++] = (char) n;
			} else if ( yyCh >= '0' && yyCh < '8' ) {
			    TQCString oct = "";

			    do {
				oct += (char) yyCh;
				yyCh = getChar();
			    } while ( yyCh >= '0' && yyCh < '8' );
			    sscanf( oct, "%o", &n );
			    if ( yyStringLen < sizeof(yyString) - 1 )
				yyString[yyStringLen++] = (char) n;
			} else {
			    const char *p = strchr( tab, yyCh );
			    if ( yyStringLen < sizeof(yyString) - 1 )
				yyString[yyStringLen++] = ( p == 0 ) ?
					(char) yyCh : backTab[p - tab];
			    yyCh = getChar();
			}
		    } else {
			if ( yyStringLen < sizeof(yyString) - 1 )
			    yyString[yyStringLen++] = (char) yyCh;
			yyCh = getChar();
		    }
		}
		yyString[yyStringLen] = '\0';

		if ( yyCh != '"' )
		    tqWarning( "%s:%d: Unterminated C++ string",
			      (const char *) yyFileName, yyLineNo );

		if ( yyCh == EOF ) {
		    return Tok_Eof;
		} else {
		    yyCh = getChar();
		    return Tok_String;
		}
		break;
	    case '-':
		yyCh = getChar();
		if ( yyCh == '>' ) {
		    yyCh = getChar();
		    return Tok_Arrow;
		}
		break;
	    case ':':
		yyCh = getChar();
		if ( yyCh == ':' ) {
		    yyCh = getChar();
		    return Tok_Gulbrandsen;
		}
		return Tok_Colon;
	    case '\'':
		yyCh = getChar();
		if ( yyCh == '\\' )
		    yyCh = getChar();

		do {
		    yyCh = getChar();
		} while ( yyCh != EOF && yyCh != '\'' );
		yyCh = getChar();
		break;
	    case '{':
                if (yyBraceDepth == 0)
		    yyBraceLineNo = yyCurLineNo;
		yyBraceDepth++;
		yyCh = getChar();
		return Tok_LeftBrace;
	    case '}':
                if (yyBraceDepth == 0)
		    yyBraceLineNo = yyCurLineNo;
		yyBraceDepth--;
		yyCh = getChar();
		return Tok_RightBrace;
	    case '(':
                if (yyParenDepth == 0)
		    yyParenLineNo = yyCurLineNo;
		yyParenDepth++;
		yyCh = getChar();
		return Tok_LeftParen;
	    case ')':
		if (yyParenDepth == 0)
		    yyParenLineNo = yyCurLineNo;
		yyParenDepth--;
		yyCh = getChar();
		return Tok_RightParen;
	    case ',':
		yyCh = getChar();
		return Tok_Comma;
	    case ';':
		yyCh = getChar();
		return Tok_Semicolon;
	    default:
		yyCh = getChar();
	    }
	}
    }
    return Tok_Eof;
}

/*
  The second part of this source file is the parser. It accomplishes
  a very easy task: It finds all strings inside a tr() or translate()
  call, and possibly finds out the context of the call. It supports
  three cases: (1) the context is specified, as in
  FunnyDialog::tr("Hello") or translate("FunnyDialog", "Hello");
  (2) the call appears within an inlined function; (3) the call
  appears within a function defined outside the class definition.
*/

static int yyTok;

static bool match( int t )
{
    bool matches = ( yyTok == t );
    if ( matches )
	yyTok = getToken();
    return matches;
}

static bool matchString( TQCString *s )
{
    bool matches = ( yyTok == Tok_String );
    *s = "";
    while ( yyTok == Tok_String ) {
	*s += yyString;
	yyTok = getToken();
    }
    return matches;
}

static bool matchEncoding( bool *utf8 )
{
    if ( yyTok == Tok_Ident ) {
	if ( strcmp(yyIdent, "TQApplication") == 0 ) {
	    yyTok = getToken();
	    if ( yyTok == Tok_Gulbrandsen )
		yyTok = getToken();
	}
	*utf8 = TQString( yyIdent ).endsWith( TQString("UTF8") );
	yyTok = getToken();
	return true;
    } else {
	return false;
    }
}

static void parse( MetaTranslator *tor, const char *initialContext,
		   const char *defaultContext )
{
    TQMap<TQCString, TQCString> qualifiedContexts;
    TQStringList namespaces;
    TQCString context;
    TQCString text;
    TQCString com;
    TQCString functionContext = initialContext;
    TQCString prefix;
    bool utf8 = false;
    bool missing_TQ_OBJECT = false;

    yyTok = getToken();
    while ( yyTok != Tok_Eof ) {
	switch ( yyTok ) {
	case Tok_class:
	    /*
	      Partial support for inlined functions.
	    */
	    yyTok = getToken();
	    if ( yyBraceDepth == (int) namespaces.count() &&
		 yyParenDepth == 0 ) {
		do {
		    /*
		      This code should execute only once, but we play
		      safe with impure definitions such as
		      'class TQ_EXPORT TQMessageBox', in which case
		      'TQMessageBox' is the class name, not 'TQ_EXPORT'.
		    */
		    functionContext = yyIdent;
		    yyTok = getToken();
		} while ( yyTok == Tok_Ident );

		while ( yyTok == Tok_Gulbrandsen ) {
		    yyTok = getToken();
		    functionContext += "::";
		    functionContext += yyIdent;
		    yyTok = getToken();
		}

		if ( yyTok == Tok_Colon ) {
		    missing_TQ_OBJECT = true;
		} else {
		    functionContext = defaultContext;
		}
	    }
	    break;
	case Tok_namespace:
	    yyTok = getToken();
	    if ( yyTok == Tok_Ident ) {
		TQCString ns = yyIdent;
		yyTok = getToken();
		if ( yyTok == Tok_LeftBrace &&
		     yyBraceDepth == (int) namespaces.count() + 1 )
		    namespaces.append( TQString(ns) );
	    }
	    break;
	case Tok_tr:
	case Tok_trUtf8:
	    utf8 = ( yyTok == Tok_trUtf8 );
	    yyTok = getToken();
	    if ( match(Tok_LeftParen) && matchString(&text) ) {
		com = "";
		if ( match(Tok_RightParen) || (match(Tok_Comma) &&
			matchString(&com) && match(Tok_RightParen)) ) {
		    if ( prefix.isNull() ) {
			context = functionContext;
			if ( !namespaces.isEmpty() )
			    context.prepend( (namespaces.join(TQString("::")) +
					      TQString("::")).latin1() );
		    } else {
			context = prefix;
		    }
		    prefix = (const char *) 0;

		    if ( qualifiedContexts.contains(context) )
			context = qualifiedContexts[context];
		    tor->insert( MetaTranslatorMessage(context, text, com,
						       TQString::null, utf8) );

		    if ( lacks_TQ_OBJECT.contains(context) ) {
			tqWarning( "%s:%d: Class '%s' lacks TQ_OBJECT macro",
				  (const char *) yyFileName, yyLineNo,
				  (const char *) context );
			lacks_TQ_OBJECT.remove( context );
		    } else {
			needs_TQ_OBJECT.insert( context, 0 );
		    }
		}
	    }
	    break;
	case Tok_translate:
	    utf8 = false;
	    yyTok = getToken();
	    if ( match(Tok_LeftParen) &&
		 matchString(&context) &&
		 match(Tok_Comma) &&
		 matchString(&text) ) {
		com = "";
		if ( match(Tok_RightParen) ||
		     (match(Tok_Comma) &&
		      matchString(&com) &&
		      (match(Tok_RightParen) ||
		       (match(Tok_Comma) &&
		        matchEncoding(&utf8) &&
		        match(Tok_RightParen)))) )
		    tor->insert( MetaTranslatorMessage(context, text, com,
						       TQString::null, utf8) );
	    }
	    break;
	case Tok_TQ_OBJECT:
	    missing_TQ_OBJECT = false;
	    yyTok = getToken();
	    break;
	case Tok_Ident:
	    if ( !prefix.isNull() )
		prefix += "::";
	    prefix += yyIdent;
	    yyTok = getToken();
	    if ( yyTok != Tok_Gulbrandsen )
		prefix = (const char *) 0;
	    break;
	case Tok_Comment:
	    com = yyComment;
	    com = com.simplifyWhiteSpace();
	    if ( com.left(sizeof(MagicComment) - 1) == MagicComment ) {
		com.remove( 0, sizeof(MagicComment) - 1 );
		int k = com.find( ' ' );
		if ( k == -1 ) {
		    context = com;
		} else {
		    context = com.left( k );
		    com.remove( 0, k + 1 );
		    tor->insert( MetaTranslatorMessage(context, "", com,
						       TQString::null, false) );
		}

		/*
		  Provide a backdoor for people using "using
		  namespace". See the manual for details.
		*/
		k = 0;
		while ( (k = context.find("::", k)) != -1 ) {
		    qualifiedContexts.insert( context.mid(k + 2), context );
		    k++;
		}
	    }
	    yyTok = getToken();
	    break;
	case Tok_Arrow:
	    yyTok = getToken();
	    if ( yyTok == Tok_tr || yyTok == Tok_trUtf8 )
		tqWarning( "%s:%d: Cannot invoke tr() like this",
			  (const char *) yyFileName, yyLineNo );
	    break;
	case Tok_Gulbrandsen:
	    // at top level?
	    if ( yyBraceDepth == (int) namespaces.count() && yyParenDepth == 0 )
		functionContext = prefix;
	    yyTok = getToken();
	    break;
	case Tok_RightBrace:
	case Tok_Semicolon:
	    if ( yyBraceDepth >= 0 &&
		 yyBraceDepth + 1 == (int) namespaces.count() )
		namespaces.remove( namespaces.fromLast() );
	    if ( yyBraceDepth == (int) namespaces.count() ) {
		if ( missing_TQ_OBJECT ) {
		    if ( needs_TQ_OBJECT.contains(functionContext) ) {
			tqWarning( "%s:%d: Class '%s' lacks TQ_OBJECT macro",
				  (const char *) yyFileName, yyLineNo,
				  (const char *) functionContext );
		    } else {
			lacks_TQ_OBJECT.insert( functionContext, 0 );
		    }
		}
		functionContext = defaultContext;
		missing_TQ_OBJECT = false;
	    }
	    yyTok = getToken();
	    break;
	default:
	    yyTok = getToken();
	}
    }

    if ( yyBraceDepth != 0 )
	fprintf( stderr,
		 "%s:%d: Unbalanced braces in C++ code (or abuse of the C++"
		  " preprocessor)\n",
		  (const char *)yyFileName, yyBraceLineNo );
    else if ( yyParenDepth != 0 )
	fprintf( stderr,
		 "%s:%d: Unbalanced parentheses in C++ code (or abuse of the C++"
		 " preprocessor)\n",
		 (const char *)yyFileName, yyParenLineNo );
}

void fetchtr_cpp( const char *fileName, MetaTranslator *tor,
		  const char *defaultContext, bool mustExist )
{
    yyInFile = fopen( fileName, "r" );
    if ( yyInFile == 0 ) {
	if ( mustExist )
	    fprintf( stderr,
		     "lupdate error: Cannot open C++ source file '%s': %s\n",
		     fileName, strerror(errno) );
	return;
    }

    startTokenizer( fileName, getCharFromFile );
    parse( tor, 0, defaultContext );
    fclose( yyInFile );
}

/*
  In addition to C++, we support TQt Designer UI files.
*/

/*
  Fetches tr() calls in C++ code in UI files (inside "<function>"
  tag). This mechanism is obsolete.
*/
void fetchtr_inlined_cpp( const char *fileName, const TQString& in,
			  MetaTranslator *tor, const char *context )
{
    yyInStr = in;
    startTokenizer( fileName, getCharFromString );
    parse( tor, context, 0 );
    yyInStr = TQString::null;
}

class UiHandler : public TQXmlDefaultHandler
{
public:
    UiHandler( MetaTranslator *translator, const char *fileName )
	: tor( translator ), fname( fileName ), comment( "" ) { }

    virtual bool startElement( const TQString& namespaceURI,
			       const TQString& localName, const TQString& qName,
			       const TQXmlAttributes& atts );
    virtual bool endElement( const TQString& namespaceURI,
			     const TQString& localName, const TQString& qName );
    virtual bool characters( const TQString& ch );
    virtual bool fatalError( const TQXmlParseException& exception );

private:
    void flush();

    MetaTranslator *tor;
    TQCString fname;
    TQString context;
    TQString source;
    TQString comment;

    TQString accum;
};

bool UiHandler::startElement( const TQString& /* namespaceURI */,
			      const TQString& /* localName */,
			      const TQString& qName,
			      const TQXmlAttributes& atts )
{
    if ( qName == TQString("item") ) {
	flush();
	if ( !atts.value(TQString("text")).isEmpty() )
	    source = atts.value( TQString("text") );
    } else if ( qName == TQString("string") ) {
	flush();
    }
    accum.truncate( 0 );
    return true;
}

bool UiHandler::endElement( const TQString& /* namespaceURI */,
			    const TQString& /* localName */,
			    const TQString& qName )
{
    accum.replace( TQRegExp(TQString("\r\n")), "\n" );

    if ( qName == TQString("class") ) {
	if ( context.isEmpty() )
	    context = accum;
    } else if ( qName == TQString("string") ) {
	source = accum;
    } else if ( qName == TQString("comment") ) {
	comment = accum;
	flush();
    } else if ( qName == TQString("function") ) {
	fetchtr_inlined_cpp( (const char *) fname, accum, tor,
			     context.latin1() );
    } else {
	flush();
    }
    return true;
}

bool UiHandler::characters( const TQString& ch )
{
    accum += ch;
    return true;
}

bool UiHandler::fatalError( const TQXmlParseException& exception )
{
    TQString msg;
    msg.sprintf( "Parse error at line %d, column %d (%s).",
		 exception.lineNumber(), exception.columnNumber(),
		 exception.message().latin1() );
    fprintf( stderr, "XML error: %s\n", msg.latin1() );
    return false;
}

void UiHandler::flush()
{
    if ( !context.isEmpty() && !source.isEmpty() )
	tor->insert( MetaTranslatorMessage(context.utf8(), source.utf8(),
					   comment.utf8(), TQString::null,
					   true) );
    source.truncate( 0 );
    comment.truncate( 0 );
}

void fetchtr_ui( const char *fileName, MetaTranslator *tor,
		 const char * /* defaultContext */, bool mustExist )
{
    TQFile f( fileName );
    if ( !f.open(IO_ReadOnly) ) {
	if ( mustExist )
	    fprintf( stderr, "lupdate error: cannot open UI file '%s': %s\n",
		     fileName, strerror(errno) );
	return;
    }

    TQTextStream t( &f );
    TQXmlInputSource in( t );
    TQXmlSimpleReader reader;
    reader.setFeature( "http://xml.org/sax/features/namespaces", false );
    reader.setFeature( "http://xml.org/sax/features/namespace-prefixes", true );
    reader.setFeature( "http://trolltech.com/xml/features/report-whitespace"
		       "-only-CharData", false );
    TQXmlDefaultHandler *hand = new UiHandler( tor, fileName );
    reader.setContentHandler( hand );
    reader.setErrorHandler( hand );

    if ( !reader.parse(in) )
	fprintf( stderr, "%s: Parse error in UI file\n", fileName );
    reader.setContentHandler( 0 );
    reader.setErrorHandler( 0 );
    delete hand;
    f.close();
}
