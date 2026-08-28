/****************************************************************************
**
** Implementation of the TQString class and related Unicode functions
**
** Created : 920722
**
** Copyright (C) 2015 Timothy Pearson. All rights reserved.
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the tools module of the TQt GUI Toolkit.
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
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

// Don't define it while compiling this module, or USERS of TQt will
// not be able to link.
#ifdef TQT_NO_CAST_ASCII
#undef TQT_NO_CAST_ASCII
#endif

// WARNING
// When MAKE_QSTRING_THREAD_SAFE is defined, overall TQt3 performance suffers badly!
// TQString is thread unsafe in many other areas; perhaps this option is not even useful?
// #define MAKE_QSTRING_THREAD_SAFE 1

#include "ntqstring.h"
#include "ntqregexp.h"
#include "ntqdatastream.h"
#ifndef TQT_NO_TEXTCODEC
#include "ntqtextcodec.h"
#endif
#include "ntqlocale.h"
#include "qlocale_p.h"

#include "qunicodetables_p.h"
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef Q_OS_TEMP
#include <locale.h>
#endif
#if defined(TQ_WS_WIN)
#include "qt_windows.h"
#endif
#if defined(Q_OS_LINUX)
#include <sys/mman.h>
#endif
#if !defined( TQT_NO_COMPONENT ) && !defined( QT_LITE_COMPONENT )
#include "ntqcleanuphandler.h"
#endif

#if defined(Q_OS_LINUX)
#define LINUX_MEMLOCK_LIMIT_BYTES 16384
#define LINUX_MEMLOCK_LIMIT_CHARACTERS LINUX_MEMLOCK_LIMIT_BYTES/sizeof(TQChar)
#endif

#ifndef LLONG_MAX
#define LLONG_MAX TQ_INT64_C(9223372036854775807)
#endif
#ifndef LLONG_MIN
#define LLONG_MIN (-LLONG_MAX - TQ_INT64_C(1))
#endif
#ifndef ULLONG_MAX
#define ULLONG_MAX TQ_UINT64_C(18446744073709551615)
#endif

#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
#include "ntqmutex.h"
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE

extern TQMutex *tqt_sharedStringMutex;

static int ucstrcmp( const TQString &as, const TQString &bs )
{
    const TQChar *a = as.unicode();
    const TQChar *b = bs.unicode();
    if ( a == b )
	return 0;
    if ( a == 0 )
	return 1;
    if ( b == 0 )
	return -1;
    int l=TQMIN(as.length(),bs.length());
    while ( l-- && *a == *b )
	a++,b++;
    if ( l==-1 )
	return ( as.length()-bs.length() );
    return a->unicode() - b->unicode();
}

static int ucstrncmp( const TQChar *a, const TQChar *b, int l )
{
    while ( l-- && *a == *b )
	a++,b++;
    if ( l==-1 )
	return 0;
    return a->unicode() - b->unicode();
}

static int ucstrnicmp( const TQChar *a, const TQChar *b, int l )
{
    while ( l-- && ::lower( *a ) == ::lower( *b ) )
	a++,b++;
    if ( l==-1 )
	return 0;
    return ::lower( *a ).unicode() - ::lower( *b ).unicode();
}

static uint computeNewMax( uint len )
{
    if (len >= 0x80000000)
        return len;

    uint newMax = 4;
    while ( newMax < len )
	newMax *= 2;
    // try to save some memory
    if ( newMax >= 1024 * 1024 && len <= newMax - (newMax >> 2) )
	newMax -= newMax >> 2;
    return newMax;
}

static bool qIsUpper(char c)
{
    return c >= 'A' && c <= 'Z';
}

static bool qIsDigit(char c)
{
    return c >= '0' && c <= '9';
}

static char qToLower(char c)
{
    if (c >= 'A' && c <= 'Z')
    	return c - 'A' + 'a';
    else
    	return c;
}

/*!
    \class TQCharRef ntqstring.h
    \reentrant
    \brief The TQCharRef class is a helper class for TQString.

    \ingroup text

    When you get an object of type TQCharRef, if you can assign to it,
    the assignment will apply to the character in the string from
    which you got the reference. That is its whole purpose in life.
    The TQCharRef becomes invalid once modifications are made to the
    string: if you want to keep the character, copy it into a TQChar.

    Most of the TQChar member functions also exist in TQCharRef.
    However, they are not explicitly documented here.

    \sa TQString::operator[]() TQString::at() TQChar
*/

/*!
    \class TQChar ntqstring.h
    \reentrant
    \brief The TQChar class provides a lightweight Unicode character.

    \ingroup text

    Unicode characters are (so far) 16-bit entities without any markup
    or structure. This class represents such an entity. It is
    lightweight, so it can be used everywhere. Most compilers treat it
    like a "short int".  (In a few years it may be necessary to make
    TQChar 32-bit when more than 65536 Unicode code points have been
    defined and come into use.)

    TQChar provides a full complement of testing/classification
    functions, converting to and from other formats, converting from
    composed to decomposed Unicode, and trying to compare and
    case-convert if you ask it to.

    The classification functions include functions like those in
    ctype.h, but operating on the full range of Unicode characters.
    They all return true if the character is a certain type of
    character; otherwise they return false. These classification
    functions are isNull() (returns true if the character is U+0000),
    isPrint() (true if the character is any sort of printable
    character, including whitespace), isPunct() (any sort of
    punctation), isMark() (Unicode Mark), isLetter (a letter),
    isNumber() (any sort of numeric character), isLetterOrNumber(),
    and isDigit() (decimal digits). All of these are wrappers around
    category() which return the Unicode-defined category of each
    character.

    TQChar further provides direction(), which indicates the "natural"
    writing direction of this character. The joining() function
    indicates how the character joins with its neighbors (needed
    mostly for Arabic) and finally mirrored(), which indicates whether
    the character needs to be mirrored when it is printed in its
    "unnatural" writing direction.

    Composed Unicode characters (like &aring;) can be converted to
    decomposed Unicode ("a" followed by "ring above") by using
    decomposition().

    In Unicode, comparison is not necessarily possible and case
    conversion is very difficult at best. Unicode, covering the
    "entire" world, also includes most of the world's case and sorting
    problems. TQt tries, but not very hard: operator==() and friends
    will do comparison based purely on the numeric Unicode value (code
    point) of the characters, and upper() and lower() will do case
    changes when the character has a well-defined upper/lower-case
    equivalent. There is no provision for locale-dependent case
    folding rules or comparison; these functions are meant to be fast
    so they can be used unambiguously in data structures. (See
    TQString::localeAwareCompare() though.)

    The conversion functions include unicode() (to a scalar), latin1()
    (to scalar, but converts all non-Latin-1 characters to 0), row()
    (gives the Unicode row), cell() (gives the Unicode cell),
    digitValue() (gives the integer value of any of the numerous digit
    characters), and a host of constructors.

    More information can be found in the document \link unicode.html
    About Unicode. \endlink

    \sa TQString TQCharRef
*/

/*!
    \enum TQChar::Category

    This enum maps the Unicode character categories.

    The following characters are normative in Unicode:

    \value Mark_NonSpacing  Unicode class name Mn

    \value Mark_SpacingCombining  Unicode class name Mc

    \value Mark_Enclosing  Unicode class name Me

    \value Number_DecimalDigit  Unicode class name Nd

    \value Number_Letter  Unicode class name Nl

    \value Number_Other  Unicode class name No

    \value Separator_Space  Unicode class name Zs

    \value Separator_Line  Unicode class name Zl

    \value Separator_Paragraph  Unicode class name Zp

    \value Other_Control  Unicode class name Cc

    \value Other_Format  Unicode class name Cf

    \value Other_Surrogate  Unicode class name Cs

    \value Other_PrivateUse  Unicode class name Co

    \value Other_NotAssigned  Unicode class name Cn


    The following categories are informative in Unicode:

    \value Letter_Uppercase  Unicode class name Lu

    \value Letter_Lowercase  Unicode class name Ll

    \value Letter_Titlecase  Unicode class name Lt

    \value Letter_Modifier  Unicode class name Lm

    \value Letter_Other Unicode class name Lo

    \value Punctuation_Connector  Unicode class name Pc

    \value Punctuation_Dash  Unicode class name Pd

    \value Punctuation_Open  Unicode class name Ps

    \value Punctuation_Close  Unicode class name Pe

    \value Punctuation_InitialQuote  Unicode class name Pi

    \value Punctuation_FinalQuote  Unicode class name Pf

    \value Punctuation_Other  Unicode class name Po

    \value Symbol_Math  Unicode class name Sm

    \value Symbol_Currency  Unicode class name Sc

    \value Symbol_Modifier  Unicode class name Sk

    \value Symbol_Other  Unicode class name So


    There are two categories that are specific to TQt:

    \value NoCategory  used when TQt is dazed and confused and cannot
    make sense of anything.

    \value Punctuation_Dask  old typo alias for Punctuation_Dash

*/

/*!
    \enum TQChar::Direction

    This enum type defines the Unicode direction attributes. See \link
    http://www.unicode.org/ the Unicode Standard\endlink for a
    description of the values.

    In order to conform to C/C++ naming conventions "Dir" is prepended
    to the codes used in the Unicode Standard.
*/

/*!
    \enum TQChar::Decomposition

    This enum type defines the Unicode decomposition attributes. See
    \link http://www.unicode.org/ the Unicode Standard\endlink for a
    description of the values.
*/

/*!
    \enum TQChar::Joining

    This enum type defines the Unicode joining attributes. See \link
    http://www.unicode.org/ the Unicode Standard\endlink for a
    description of the values.
*/

/*!
    \enum TQChar::CombiningClass

    This enum type defines names for some of the Unicode combining
    classes. See \link http://www.unicode.org/ the Unicode
    Standard\endlink for a description of the values.
*/

/*!
    \fn void TQChar::setCell( uchar cell )
    \internal
*/

/*!
    \fn void TQChar::setRow( uchar row )
    \internal
*/


/*!
    \fn TQChar::TQChar()

    Constructs a null TQChar (one that isNull()).
*/


/*!
    \fn TQChar::TQChar( char c )

    Constructs a TQChar corresponding to ASCII/Latin-1 character \a c.
*/


/*!
    \fn TQChar::TQChar( uchar c )

    Constructs a TQChar corresponding to ASCII/Latin-1 character \a c.
*/


/*!
    \fn TQChar::TQChar( uchar c, uchar r )

    Constructs a TQChar for Unicode cell \a c in row \a r.
*/


/*!
    \fn TQChar::TQChar( const TQChar& c )

    Constructs a copy of \a c. This is a deep copy, if such a
    lightweight object can be said to have deep copies.
*/


/*!
    \fn TQChar::TQChar( ushort rc )

    Constructs a TQChar for the character with Unicode code point \a rc.
*/


/*!
    \fn TQChar::TQChar( short rc )

    Constructs a TQChar for the character with Unicode code point \a rc.
*/


/*!
    \fn TQChar::TQChar( uint rc )

    Constructs a TQChar for the character with Unicode code point \a rc.
*/


/*!
    \fn TQChar::TQChar( int rc )

    Constructs a TQChar for the character with Unicode code point \a rc.
*/


/*!
    \fn bool  TQChar::networkOrdered ()

    \obsolete

    Returns true if this character is in network byte order (MSB
    first); otherwise returns false. This is platform dependent.
*/


/*!
    \fn bool TQChar::isNull() const

    Returns true if the character is the Unicode character 0x0000
    (ASCII NUL); otherwise returns false.
*/

/*!
    \fn uchar TQChar::cell () const

    Returns the cell (least significant byte) of the Unicode
    character.
*/

/*!
    \fn uchar TQChar::row () const

    Returns the row (most significant byte) of the Unicode character.
*/

/*!
    Returns true if the character is a printable character; otherwise
    returns false. This is any character not of category Cc or Cn.

    Note that this gives no indication of whether the character is
    available in a particular \link TQFont font\endlink.
*/
bool TQChar::isPrint() const
{
    Category c = ::category( *this );
    return !(c == Other_Control || c == Other_NotAssigned);
}

/*!
    Returns true if the character is a separator character
    (Separator_* categories); otherwise returns false.
*/
bool TQChar::isSpace() const
{
    return ::isSpace( *this );
}

/*!
    Returns true if the character is a mark (Mark_* categories);
    otherwise returns false.
*/
bool TQChar::isMark() const
{
    Category c = ::category( *this );
    return c >= Mark_NonSpacing && c <= Mark_Enclosing;
}

/*!
    Returns true if the character is a punctuation mark (Punctuation_*
    categories); otherwise returns false.
*/
bool TQChar::isPunct() const
{
    Category c = ::category( *this );
    return (c >= Punctuation_Connector && c <= Punctuation_Other);
}

/*!
    Returns true if the character is a letter (Letter_* categories);
    otherwise returns false.
*/
bool TQChar::isLetter() const
{
    Category c = ::category( *this );
    return (c >= Letter_Uppercase && c <= Letter_Other);
}

/*!
    Returns true if the character is a number (of any sort - Number_*
    categories); otherwise returns false.

    \sa isDigit()
*/
bool TQChar::isNumber() const
{
    Category c = ::category( *this );
    return c >= Number_DecimalDigit && c <= Number_Other;
}

/*!
    Returns true if the character is a letter or number (Letter_* or
    Number_* categories); otherwise returns false.
*/
bool TQChar::isLetterOrNumber() const
{
    Category c = ::category( *this );
    return (c >= Letter_Uppercase && c <= Letter_Other)
	|| (c >= Number_DecimalDigit && c <= Number_Other);
}


/*!
    Returns true if the character is a decimal digit
    (Number_DecimalDigit); otherwise returns false.
*/
bool TQChar::isDigit() const
{
    return (::category( *this ) == Number_DecimalDigit);
}


/*!
    Returns true if the character is a symbol (Symbol_* categories);
    otherwise returns false.
*/
bool TQChar::isSymbol() const
{
    Category c = ::category( *this );
    return c >= Symbol_Math && c <= Symbol_Other;
}

/*!
    Returns the numeric value of the digit, or -1 if the character is
    not a digit.
*/
int TQChar::digitValue() const
{
#ifndef TQT_NO_UNICODETABLES
    int pos = TQUnicodeTables::decimal_info[row()];
    if( !pos )
	return -1;
    return TQUnicodeTables::decimal_info[(pos<<8) + cell()];
#else
    // ##### just latin1
    if ( ucs < '0' || ucs > '9' )
	return -1;
    else
	return ucs - '0';
#endif
}

/*!
    Returns the character category.

    \sa Category
*/
TQChar::Category TQChar::category() const
{
     return ::category( *this );
}

/*!
    Returns the character's direction.

    \sa Direction
*/
TQChar::Direction TQChar::direction() const
{
     return ::direction( *this );
}

/*!
    \warning This function is not supported (it may change to use
    Unicode character classes).

    Returns information about the joining properties of the character
    (needed for example, for Arabic).
*/
TQChar::Joining TQChar::joining() const
{
    return ::joining( *this );
}


/*!
    Returns true if the character is a mirrored character (one that
    should be reversed if the text direction is reversed); otherwise
    returns false.
*/
bool TQChar::mirrored() const
{
    return ::mirrored( *this );
}

/*!
    Returns the mirrored character if this character is a mirrored
    character, otherwise returns the character itself.
*/
TQChar TQChar::mirroredChar() const
{
    return ::mirroredChar( *this );
}

#ifndef TQT_NO_UNICODETABLES
// ### REMOVE ME 4.0
static TQString shared_decomp;
#endif
/*!
    \nonreentrant

    Decomposes a character into its parts. Returns TQString::null if no
    decomposition exists.
*/
const TQString &TQChar::decomposition() const
{
#ifndef TQT_NO_UNICODETABLES
    int pos = TQUnicodeTables::decomposition_info[row()];
    if(!pos) return TQString::null;

    pos = TQUnicodeTables::decomposition_info[(pos<<8)+cell()];
    if(!pos) return TQString::null;
    pos+=2;

    TQString s;
    TQ_UINT16 c;
    while ( (c = TQUnicodeTables::decomposition_map[pos++]) != 0 )
	s += TQChar( c );
    // ### In 4.0, return s, and not shared_decomp.  shared_decomp
    // prevents this function from being reentrant.
    shared_decomp = s;
    return shared_decomp;
#else
    return TQString::null;
#endif
}

/*!
    Returns the tag defining the composition of the character. Returns
    TQChar::Single if no decomposition exists.
*/
TQChar::Decomposition TQChar::decompositionTag() const
{
#ifndef TQT_NO_UNICODETABLES
    int pos = TQUnicodeTables::decomposition_info[row()];
    if(!pos) return TQChar::Single;

    pos = TQUnicodeTables::decomposition_info[(pos<<8)+cell()];
    if(!pos) return TQChar::Single;

    return (TQChar::Decomposition) TQUnicodeTables::decomposition_map[pos];
#else
    return Single; // ########### FIX eg. just latin1
#endif
}

/*!
    Returns the combining class for the character as defined in the
    Unicode standard. This is mainly useful as a positioning hint for
    marks attached to a base character.

    The TQt text rendering engine uses this information to correctly
    position non spacing marks around a base character.
*/
unsigned char TQChar::combiningClass() const
{
    return ::combiningClass( *this );
}


/*!
    Returns the lowercase equivalent if the character is uppercase;
    otherwise returns the character itself.
*/
TQChar TQChar::lower() const
{
     return ::lower( *this );
}

/*!
    Returns the uppercase equivalent if the character is lowercase;
    otherwise returns the character itself.
*/
TQChar TQChar::upper() const
{
     return ::upper( *this );
}

/*!
    \fn TQChar::operator char() const

    Returns the Latin-1 character equivalent to the TQChar, or 0. This
    is mainly useful for non-internationalized software.

    \sa unicode()
*/

/*!
    \fn ushort TQChar::unicode() const

    Returns the numeric Unicode value equal to the TQChar. Normally,
    you should use TQChar objects as they are equivalent, but for some
    low-level tasks (e.g. indexing into an array of Unicode
    information), this function is useful.
*/

/*!
    \fn ushort & TQChar::unicode()

    \overload

    Returns a reference to the numeric Unicode value equal to the
    TQChar.
*/

/*****************************************************************************
  Documentation of TQChar related functions
 *****************************************************************************/

/*!
    \fn bool operator==( TQChar c1, TQChar c2 )

    \relates TQChar

    Returns true if \a c1 and \a c2 are the same Unicode character;
    otherwise returns false.
*/

/*!
    \fn bool operator==( char ch, TQChar c )

    \overload
    \relates TQChar

    Returns true if \a c is the ASCII/Latin-1 character \a ch;
    otherwise returns false.
*/

/*!
    \fn bool operator==( TQChar c, char ch )

    \overload
    \relates TQChar

    Returns true if \a c is the ASCII/Latin-1 character \a ch;
    otherwise returns false.
*/

/*!
    \fn int operator!=( TQChar c1, TQChar c2 )

    \relates TQChar

    Returns true if \a c1 and \a c2 are not the same Unicode
    character; otherwise returns false.
*/

/*!
    \fn int operator!=( char ch, TQChar c )

    \overload
    \relates TQChar

    Returns true if \a c is not the ASCII/Latin-1 character \a ch;
    otherwise returns false.
*/

/*!
    \fn int operator!=( TQChar c, char ch )

    \overload
    \relates TQChar

    Returns true if \a c is not the ASCII/Latin-1 character \a ch;
    otherwise returns false.
*/

/*!
    \fn int operator<=( TQChar c1, TQChar c2 )

    \relates TQChar

    Returns true if the numeric Unicode value of \a c1 is less than
    that of \a c2, or they are the same Unicode character; otherwise
    returns false.
*/

/*!
    \fn int operator<=( TQChar c, char ch )

    \overload
    \relates TQChar

    Returns true if the numeric Unicode value of \a c is less than or
    equal to that of the ASCII/Latin-1 character \a ch; otherwise
    returns false.
*/

/*!
    \fn int operator<=( char ch, TQChar c )

    \overload
    \relates TQChar

    Returns true if the numeric Unicode value of the ASCII/Latin-1
    character \a ch is less than or equal to that of \a c; otherwise
    returns false.
*/

/*!
    \fn int operator>=( TQChar c1, TQChar c2 )

    \relates TQChar

    Returns true if the numeric Unicode value of \a c1 is greater than
    that of \a c2, or they are the same Unicode character; otherwise
    returns false.
*/

/*!
    \fn int operator>=( TQChar c, char ch )

    \overload
    \relates TQChar

    Returns true if the numeric Unicode value of \a c is greater than
    or equal to that of the ASCII/Latin-1 character \a ch; otherwise
    returns false.
*/

/*!
    \fn int operator>=( char ch, TQChar c )

    \overload
    \relates TQChar

    Returns true if the numeric Unicode value of the ASCII/Latin-1
    character \a ch is greater than or equal to that of \a c;
    otherwise returns false.
*/

/*!
    \fn int operator<( TQChar c1, TQChar c2 )

    \relates TQChar

    Returns true if the numeric Unicode value of \a c1 is less than
    that of \a c2; otherwise returns false.
*/

/*!
    \fn int operator<( TQChar c, char ch )

    \overload
    \relates TQChar

    Returns true if the numeric Unicode value of \a c is less than that
    of the ASCII/Latin-1 character \a ch; otherwise returns false.
*/

/*!
    \fn int operator<( char ch, TQChar c )

    \overload
    \relates TQChar

    Returns true if the numeric Unicode value of the ASCII/Latin-1
    character \a ch is less than that of \a c; otherwise returns
    false.
*/

/*!
    \fn int operator>( TQChar c1, TQChar c2 )

    \relates TQChar

    Returns true if the numeric Unicode value of \a c1 is greater than
    that of \a c2; otherwise returns false.
*/

/*!
    \fn int operator>( TQChar c, char ch )

    \overload
    \relates TQChar

    Returns true if the numeric Unicode value of \a c is greater than
    that of the ASCII/Latin-1 character \a ch; otherwise returns false.
*/

/*!
    \fn int operator>( char ch, TQChar c )

    \overload
    \relates TQChar

    Returns true if the numeric Unicode value of the ASCII/Latin-1
    character \a ch is greater than that of \a c; otherwise returns
    false.
*/

#ifndef TQT_NO_UNICODETABLES

// small class used internally in TQString::Compose()
class TQLigature
{
public:
    TQLigature( TQChar c );

    TQ_UINT16 first() { cur = ligatures; return cur ? *cur : 0; }
    TQ_UINT16 next() { return cur && *cur ? *(cur++) : 0; }
    TQ_UINT16 current() { return cur ? *cur : 0; }

    int match(TQString & str, unsigned int index);
    TQChar head();
    TQChar::Decomposition tag();

private:
    TQ_UINT16 *ligatures;
    TQ_UINT16 *cur;
};

TQLigature::TQLigature( TQChar c )
{
    int pos = TQUnicodeTables::ligature_info[c.row()];
    if( !pos )
	ligatures = 0;
    else
    {
	pos = TQUnicodeTables::ligature_info[(pos<<8)+c.cell()];
	ligatures = (TQ_UINT16 *)&(TQUnicodeTables::ligature_map[pos]);
    }
    cur = ligatures;
}

TQChar TQLigature::head()
{
    if(current())
	return TQChar(TQUnicodeTables::decomposition_map[current()+1]);

    return TQChar::null;
}

TQChar::Decomposition TQLigature::tag()
{
    if(current())
	return (TQChar::Decomposition) TQUnicodeTables::decomposition_map[current()];

    return TQChar::Canonical;
}

int TQLigature::match(TQString & str, unsigned int index)
{
    unsigned int i=index;

    if(!current()) return 0;

    TQ_UINT16 lig = current() + 2;
    TQ_UINT16 ch;

    while ((i < str.length()) && (ch = TQUnicodeTables::decomposition_map[lig])) {
	if (str[(int)i] != TQChar(ch))
	    return 0;
	i++;
	lig++;
    }

    if (!TQUnicodeTables::decomposition_map[lig])
    {
	return i-index;
    }
    return 0;
}


// this function is just used in TQString::compose()
static inline bool format(TQChar::Decomposition tag, TQString & str,
			  int index, int len)
{
    unsigned int l = index + len;
    unsigned int r = index;

    bool left = false, right = false;

    left = ((l < str.length()) &&
	    ((str[(int)l].joining() == TQChar::Dual) ||
	     (str[(int)l].joining() == TQChar::Right)));
    if (r > 0) {
	r--;
	//printf("joining(right) = %d\n", str[(int)r].joining());
	right = (str[(int)r].joining() == TQChar::Dual);
    }


    switch (tag) {
    case TQChar::Medial:
	return (left & right);
    case TQChar::Initial:
	return (left && !right);
    case TQChar::Final:
	return (right);// && !left);
    case TQChar::Isolated:
    default:
	return (!right && !left);
    }
} // format()
#endif

TQStringData::TQStringData() : TQShared(),
	unicode(0),
	ascii(0),
	len(0),
	issimpletext(true),
	maxl(0),
	islatin1(false),
	security_unpaged(false),
	cString(0) {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
	mutex = new TQMutex(false);
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
	ref();
}

TQStringData::TQStringData(TQChar *u, uint l, uint m) : TQShared(),
	unicode(u),
	ascii(0),
	len(l),
	issimpletext(false),
	maxl(m),
	islatin1(false),
	security_unpaged(false),
	cString(0) {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
	mutex = new TQMutex(false);
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
}

TQStringData::~TQStringData() {
	if ( unicode ) {
		delete[] ((char*)unicode);
	}
#if defined(Q_OS_LINUX)
	if ( ascii && security_unpaged ) {
		munlock(ascii, LINUX_MEMLOCK_LIMIT_BYTES);
	}
#endif
	if ( ascii ) {
		delete[] ascii;
	}
	if (cString) {
		delete cString;
	}
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
	if ( mutex ) {
		delete mutex;
		mutex = NULL;
	}
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
}

void TQStringData::setDirty() {
	if ( ascii ) {
		delete [] ascii;
		ascii = 0;
	}
	if (cString) {
		delete cString;
		cString = 0;
	}
	issimpletext = false;
}

/*
  TQString::compose() and visual() were developed by Gordon Tisher
  <tisher@uniserve.ca>, with input from Lars Knoll <knoll@mpi-hd.mpg.de>,
  who developed the unicode data tables.
*/
/*!
    \warning This function is not supported in TQt 3.x. It is provided
    for experimental and illustrative purposes only. It is mainly of
    interest to those experimenting with Arabic and other
    composition-rich texts.

    Applies possible ligatures to a TQString. Useful when
    composition-rich text requires rendering with glyph-poor fonts,
    but it also makes compositions such as TQChar(0x0041) ('A') and
    TQChar(0x0308) (Unicode accent diaresis), giving TQChar(0x00c4)
    (German A Umlaut).
*/
void TQString::compose()
{
#ifndef TQT_NO_UNICODETABLES
    unsigned int index=0, len;
    unsigned int cindex = 0;

    TQChar code, head;

    TQMemArray<TQChar> dia;

    TQString composed = *this;

    while (index < length()) {
	code = at(index);
	//printf("\n\nligature for 0x%x:\n", code.unicode());
	TQLigature ligature(code);
	ligature.first();
	while ( ligature.current() ) {
	    if ((len = ligature.match(*this, index)) != 0) {
		head = ligature.head();
		unsigned short code = head.unicode();
		// we exclude Arabic presentation forms A and a few
		// other ligatures, which are undefined in most fonts
		if(!(code > 0xfb50 && code < 0xfe80) &&
		   !(code > 0xfb00 && code < 0xfb2a)) {
				// joining info is only needed for Arabic
		    if (format(ligature.tag(), *this, index, len)) {
			//printf("using ligature 0x%x, len=%d\n",code,len);
			// replace letter
			composed.replace(cindex, len, TQChar(head));
			index += len-1;
			// we continue searching in case we have a final
			// form because medial ones are preferred.
			if ( len != 1 || ligature.tag() !=TQChar::Final )
			    break;
		    }
		}
	    }
	    ligature.next();
	}
	cindex++;
	index++;
    }
    *this = composed;
#endif
}


// These macros are used for efficient allocation of TQChar strings.
// IMPORTANT! If you change these, make sure you also change the
// "delete unicode" statement in ~TQStringData() in ntqstring.h correspondingly!

#define QT_ALLOC_QCHAR_VEC( N ) (TQChar*) new char[ sizeof(TQChar)*( N ) ]
#define QT_DELETE_QCHAR_VEC( P ) delete[] ((char*)( P ))


/*!
    This utility function converts the 8-bit string \a ba to Unicode,
    returning the result.

    The caller is responsible for deleting the return value with
    delete[].
*/

TQChar* TQString::latin1ToUnicode( const TQByteArray& ba, uint* len )
{
    if ( ba.isNull() ) {
	*len = 0;
	return 0;
    }
    int l = 0;
    while ( l < (int)ba.size() && ba[l] )
	l++;
    char* str = ba.data();
    TQChar *uc = new TQChar[ l ];   // Can't use macro, since function is public
    TQChar *result = uc;
    if ( len )
	*len = l;
    while (l--)
	*uc++ = *str++;
    return result;
}

static TQChar* internalLatin1ToUnicode( const TQByteArray& ba, uint* len )
{
    if ( ba.isNull() ) {
	*len = 0;
	return 0;
    }
    int l = 0;
    while ( l < (int)ba.size() && ba[l] )
	l++;
    char* str = ba.data();
    TQChar *uc = QT_ALLOC_QCHAR_VEC( l );
    TQChar *result = uc;
    if ( len )
	*len = l;
    while (l--)
	*uc++ = *str++;
    return result;
}

/*!
    \overload

    This utility function converts the '\0'-terminated 8-bit string \a
    str to Unicode, returning the result and setting \a *len to the
    length of the Unicode string.

    The caller is responsible for deleting the return value with
    delete[].
*/

TQChar* TQString::latin1ToUnicode( const char *str, uint* len, uint maxlen )
{
    TQChar* result = 0;
    uint l = 0;
    if ( str ) {
	if ( maxlen != (uint)-1 ) {
	    while ( l < maxlen && str[l] )
		l++;
	} else {
	    // Faster?
	    l = int(strlen( str ));
	}
	TQChar *uc = new TQChar[ l ]; // Can't use macro since function is public
	result = uc;
	uint i = l;
	while ( i-- )
	    *uc++ = *str++;
    }
    if ( len )
	*len = l;
    return result;
}

static TQChar* internalLatin1ToUnicode( const char *str, uint* len, uint maxlen = (uint)-1 )
{
    TQChar* result = 0;
    uint l = 0;
    if ( str ) {
	if ( maxlen != (uint)-1 ) {
	    while ( l < maxlen && str[l] ) {
		l++;
	    }
	}
	else {
	    // Faster?
	    l = int(strlen( str ));
	}
	TQChar *uc = QT_ALLOC_QCHAR_VEC( l );
	result = uc;
	uint i = l;
	while ( i-- ) {
	    *uc++ = *str++;
	}
    }
    if ( len ) {
	*len = l;
    }
    return result;
}

/*!
    ABI compatibility
*/

char* TQString::unicodeToLatin1(const TQChar *uc, uint l)
{
    return unicodeToLatin1(uc, l, false);
}

/*!
    This utility function converts \a l 16-bit characters from \a uc
    to ASCII, returning a '\0'-terminated string.

    The caller is responsible for deleting the resultant string with
    delete[].
*/

char* TQString::unicodeToLatin1(const TQChar *uc, uint l, bool unpaged)
{
    if (!uc) {
	return 0;
    }
    char *a = new char[l+1];
    char *result = a;
    if (unpaged) {
#if defined(Q_OS_LINUX)
	mlock(result, LINUX_MEMLOCK_LIMIT_BYTES);
#endif
    }
    while (l--) {
	*a++ = (uc->unicode() > 0xff) ? '?' : (char)uc->unicode();
	uc++;
    }
    *a = '\0';
    return result;
}

/*****************************************************************************
  TQString member functions
 *****************************************************************************/

/*!
    \class TQString ntqstring.h
    \reentrant

    \brief The TQString class provides an abstraction of Unicode text
    and the classic C '\0'-terminated char array.

    \ingroup tools
    \ingroup shared
    \ingroup text
    \mainclass

    TQString uses \link shclass.html implicit sharing\endlink, which
    makes it very efficient and easy to use.

    In all of the TQString methods that take \c {const char *}
    parameters, the \c {const char *} is interpreted as a classic
    C-style '\0'-terminated ASCII string. It is legal for the \c
    {const char *} parameter to be 0. If the \c {const char *} is not
    '\0'-terminated, the results are undefined. Functions that copy
    classic C strings into a TQString will not copy the terminating
    '\0' character. The TQChar array of the TQString (as returned by
    unicode()) is generally not terminated by a '\0'. If you need to
    pass a TQString to a function that requires a C '\0'-terminated
    string use latin1().

    \keyword TQString::null
    A TQString that has not been assigned to anything is \e null, i.e.
    both the length and data pointer is 0. A TQString that references
    the empty string ("", a single '\0' char) is \e empty. Both null
    and empty TQStrings are legal parameters to the methods. Assigning
    \c{(const char *) 0} to TQString gives a null TQString. For
    convenience, \c TQString::null is a null TQString. When sorting,
    empty strings come first, followed by non-empty strings, followed
    by null strings. We recommend using \c{if ( !str.isNull() )} to
    check for a non-null string rather than \c{if ( !str )}; see \l
    operator!() for an explanation.

    Note that if you find that you are mixing usage of \l TQCString,
    TQString, and \l TQByteArray, this causes lots of unnecessary
    copying and might indicate that the true nature of the data you
    are dealing with is uncertain. If the data is '\0'-terminated 8-bit
    data, use \l TQCString; if it is unterminated (i.e. contains '\0's)
    8-bit data, use \l TQByteArray; if it is text, use TQString.

    Lists of strings are handled by the TQStringList class. You can
    split a string into a list of strings using TQStringList::split(),
    and join a list of strings into a single string with an optional
    separator using TQStringList::join(). You can obtain a list of
    strings from a string list that contain a particular substring or
    that match a particular \link ntqregexp.html regex\endlink using
    TQStringList::grep().

    <b>Note for C programmers</b>

    Due to C++'s type system and the fact that TQString is implicitly
    shared, TQStrings can be treated like ints or other simple base
    types. For example:

    \code
    TQString boolToString( bool b )
    {
	TQString result;
	if ( b )
	    result = "True";
	else
	    result = "False";
	return result;
    }
    \endcode

    The variable, result, is an auto variable allocated on the stack.
    When return is called, because we're returning by value, The copy
    constructor is called and a copy of the string is returned. (No
    actual copying takes place thanks to the implicit sharing, see
    below.)

    Throughout TQt's source code you will encounter TQString usages like
    this:
    \code
    TQString func( const TQString& input )
    {
	TQString output = input;
	// process output
	return output;
    }
    \endcode

    The 'copying' of input to output is almost as fast as copying a
    pointer because behind the scenes copying is achieved by
    incrementing a reference count. TQString (like all TQt's implicitly
    shared classes) operates on a copy-on-write basis, only copying if
    an instance is actually changed.

    If you wish to create a deep copy of a TQString without losing any
    Unicode information then you should use TQDeepCopy.

    \sa TQChar TQCString TQByteArray TQConstString
*/

/*! \enum TQt::ComparisonFlags
\internal
*/
/*!
    \enum TQt::StringComparisonMode

    This enum type is used to set the string comparison mode when
    searching for an item. It is used by TQListBox, TQListView and
    TQIconView, for example. We'll refer to the string being searched
    as the 'target' string.

    \value CaseSensitive The strings must match case sensitively.
    \value ExactMatch The target and search strings must match exactly.
    \value BeginsWith The target string begins with the search string.
    \value EndsWith The target string ends with the search string.
    \value Contains The target string contains the search string.

    If you OR these flags together (excluding \c CaseSensitive), the
    search criteria be applied in the following order: \c ExactMatch,
    \c BeginsWith, \c EndsWith, \c Contains.

    Matching is case-insensitive unless \c CaseSensitive is set. \c
    CaseSensitive can be OR-ed with any combination of the other
    flags.

*/
TQ_EXPORT TQStringData *TQString::shared_null = 0;
const TQString TQString::null;
const TQChar TQChar::null;
const TQChar TQChar::replacement((ushort)0xfffd);
const TQChar TQChar::byteOrderMark((ushort)0xfeff);
const TQChar TQChar::byteOrderSwapped((ushort)0xfffe);
const TQChar TQChar::nbsp((ushort)0x00a0);

TQStringData* TQString::makeSharedNull()
{
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
    if (tqt_sharedStringMutex) tqt_sharedStringMutex->lock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE

    if (TQString::shared_null) {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
	if (tqt_sharedStringMutex) tqt_sharedStringMutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
	return TQString::shared_null;
    }

    TQString::shared_null = new TQStringData;
#if defined( Q_OS_MAC ) || defined(Q_OS_SOLARIS) || defined(Q_OS_AIX)
    TQString *that = const_cast<TQString *>(&TQString::null);
    that->d = TQString::shared_null;
#endif

#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
    if (tqt_sharedStringMutex) tqt_sharedStringMutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
    return TQString::shared_null;
}

/*!
    \fn TQString::TQString()

    Constructs a null string, i.e. both the length and data pointer
    are 0.

    \sa isNull()
*/

// FIXME
// Original TQt3 code stated that there is
// "No safe way to pre-init shared_null on ALL compilers/linkers"
// Is this still true?

TQString::TQString() :
    d(0)
{
	d = shared_null ? shared_null : makeSharedNull();
}

/*!
    Constructs a string of length one, containing the character \a ch.
*/
TQString::TQString( TQChar ch )
{
    d = new TQStringData( QT_ALLOC_QCHAR_VEC( 1 ), 1, 1 );
    d->unicode[0] = ch;
}

/*!
    Constructs an implicitly shared copy of \a s. This is very fast
    since it only involves incrementing a reference count.
*/
TQString::TQString( const TQString &s ) :
    d(s.d)
{
    if ( d && (d != shared_null) ) {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
        d->mutex->lock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
        d->ref();
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
        d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
    }
}

/*!
  \internal

  Private function.

  Constructs a string with preallocated space for \a size characters.

  The string is empty.

  \sa isNull()
*/

TQString::TQString( int size, bool /*dummy*/ )
{
    if ( size ) {
	int l = size;
	TQChar* uc = QT_ALLOC_QCHAR_VEC( l );
	d = new TQStringData( uc, 0, l );
    } else {
	d = shared_null ? shared_null : (shared_null=new TQStringData);
    }
}

/*!
    Constructs a string that is a deep copy of \a ba interpreted as a
    classic C string.
*/

TQString::TQString( const TQByteArray& ba )
{
#ifndef TQT_NO_TEXTCODEC
    if ( TQTextCodec::codecForCStrings() ) {
	d = 0;
	*this = fromAscii( ba.data(), ba.size() );
	return;
    }
#endif
    uint l;
    TQChar *uc = internalLatin1ToUnicode(ba,&l);
    d = new TQStringData(uc,l,l);
}

/*!
    Constructs a string that is a deep copy of the first \a length
    characters in the TQChar array.

    If \a unicode and \a length are 0, then a null string is created.

    If only \a unicode is 0, the string is empty but has \a length
    characters of space preallocated: TQString expands automatically
    anyway, but this may speed up some cases a little. We recommend
    using the plain constructor and setLength() for this purpose since
    it will result in more readable code.

    \sa isNull() setLength()
*/

TQString::TQString( const TQChar* unicode, uint length )
{
    if ( !unicode && !length ) {
	d = shared_null ? shared_null : makeSharedNull();
    }
    else {
	TQChar* uc = QT_ALLOC_QCHAR_VEC( length );
	if ( unicode ) {
	    memcpy(uc, unicode, length*sizeof(TQChar));
	}
	d = new TQStringData(uc,unicode ? length : 0,length);
    }
}

/*!
    Constructs a string that is a deep copy of \a str, interpreted as
    a classic C string. The encoding is assumed to be Latin-1, unless
    you change it using TQTextCodec::setCodecForCStrings().

    If \a str is 0, then a null string is created.

    This is a cast constructor, but it is perfectly safe: converting a
    Latin-1 \c{const char *} to TQString preserves all the information. You
    can disable this constructor by defining \c TQT_NO_CAST_ASCII when
    you compile your applications. You can also make TQString objects
    by using setLatin1(), fromLatin1(), fromLocal8Bit(), and
    fromUtf8(). Or whatever encoding is appropriate for the 8-bit data
    you have.

    \sa isNull(), fromAscii()
*/

TQString::TQString( const char *str )
{
#ifndef TQT_NO_TEXTCODEC
    if ( TQTextCodec::codecForCStrings() ) {
	d = 0;
	*this = fromAscii( str );
	return;
    }
#endif
    uint l;
    TQChar *uc = internalLatin1ToUnicode(str,&l);
    d = new TQStringData(uc,l,l);
}

#ifndef TQT_NO_STL
/*!
    Constructs a string that is a deep copy of \a str.

    This is the same as fromAscii(\a str).
*/

TQString::TQString( const std::string &str )
{
#ifndef TQT_NO_TEXTCODEC
    if ( TQTextCodec::codecForCStrings() ) {
	d = 0;
	*this = fromAscii( str.c_str() );
	return;
    }
#endif
    uint l;
    TQChar *uc = internalLatin1ToUnicode(str.c_str(),&l);
    d = new TQStringData(uc,l,l);
}
#endif

TQString::TQString( TQStringData* dd, bool /* dummy */ ) {
	d = dd;
}

/*!
    \fn TQString::~TQString()

    Destroys the string and frees the string's data if this is the
    last reference to the string.
*/

TQString::~TQString()
{
#if defined(QT_CHECK_RANGE)
	if (!d) {
		tqWarning( "TQString::~TQString: Double free or delete detected!" );
		return;
	}
#endif

	if (d == shared_null) {
		return;
	}

#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
	d->mutex->lock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
	if ( d->deref() ) {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
		d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
		d->deleteSelf();
		d = NULL;
	}
	else {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
		d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
	}
}


/*!
    Deallocates any space reserved solely by this TQString.

    If the string does not share its data with another TQString
    instance, nothing happens; otherwise the function creates a new,
    unique copy of this string. This function is called whenever the
    string is modified.
*/

void TQString::real_detach()
{
    setLength( length() );
}

void TQString::deref()
{
	if ( d && (d != shared_null) ) {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
		d->mutex->lock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
		if ( d->deref() ) {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
			d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
			if ( d != shared_null ) {
				delete d;
			}
			d = 0;
		}
		else {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
			d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
		}
	}
}

void TQStringData::deleteSelf()
{
    delete this;
}

/*!
    \fn TQString& TQString::operator=( TQChar c )

    Sets the string to contain just the single character \a c.
*/

/*!
    \fn TQString& TQString::operator=( const std::string& s )

    \overload

    Makes a deep copy of \a s and returns a reference to the deep
    copy.
*/

/*!
    \fn TQString& TQString::operator=( char c )

    \overload

    Sets the string to contain just the single character \a c.
*/

/*!
    \overload

    Assigns a shallow copy of \a s to this string and returns a
    reference to this string. This is very fast because the string
    isn't actually copied.
*/
TQString &TQString::operator=( const TQString &s )
{
    if ( s.d && (s.d != shared_null) ) {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
        s.d->mutex->lock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
        s.d->ref();
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
        s.d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
    }
    deref();
    d = s.d;

    return *this;
}

/*!
    \overload

    Assigns a deep copy of \a cstr, interpreted as a classic C
    string, to this string. Returns a reference to this string.
*/
TQString &TQString::operator=( const TQCString& cstr )
{
    return setAscii( cstr );
}


/*!
    \overload

    Assigns a deep copy of \a str, interpreted as a classic C string
    to this string and returns a reference to this string.

    If \a str is 0, then a null string is created.

    \sa isNull()
*/
TQString &TQString::operator=( const char *str )
{
    return setAscii(str);
}


/*!
    \fn bool TQString::isNull() const

    Returns true if the string is null; otherwise returns false. A
    null string is always empty.

    \code
	TQString a;          // a.unicode() == 0, a.length() == 0
	a.isNull();         // true, because a.unicode() == 0
	a.isEmpty();        // true, because a.length() == 0
    \endcode

    \sa isEmpty(), length()
*/

/*!
    \fn bool TQString::isEmpty() const

    Returns true if the string is empty, i.e. if length() == 0;
    otherwise returns false. Null strings are also empty.

    \code
	TQString a( "" );
	a.isEmpty();        // true
	a.isNull();         // false

	TQString b;
	b.isEmpty();        // true
	b.isNull();         // true
    \endcode

    \sa isNull(), length()
*/

/*!
    \fn uint TQString::length() const

    Returns the length of the string.

    Null strings and empty strings have zero length.

    \sa isNull(), isEmpty()
*/

/*!
    If \a newLen is less than the length of the string, then the
    string is truncated at position \a newLen. Otherwise nothing
    happens.

    \code
	TQString s = "truncate me";
	s.truncate( 5 );            // s == "trunc"
    \endcode

    \sa setLength()
*/

void TQString::truncate( uint newLen )
{
    if ( newLen < d->len )
	setLength( newLen );
}

/*!
    Ensures that at least \a newLen characters are allocated to the
    string, and sets the length of the string to \a newLen. Any new
    space allocated contains arbitrary data.

    \sa reserve(), truncate()
*/
void TQString::setLength( uint newLen )
{
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
    d->mutex->lock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE

    if ( d->count != 1 || newLen > d->maxl ||
	 ( newLen * 4 < d->maxl && d->maxl > 4 ) ) {
	// detach, grow or shrink
	uint newMax = computeNewMax( newLen );
	TQChar* nd = QT_ALLOC_QCHAR_VEC( newMax );
	if ( nd ) {
	    uint len = TQMIN( d->len, newLen );
	    memcpy( nd, d->unicode, sizeof(TQChar) * len );
	    bool unpaged = d->security_unpaged;
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
	    d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
	    deref();
	    d = new TQStringData( nd, newLen, newMax );
	    setSecurityUnPaged(unpaged);
	}
	else {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
	    d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
	}
    }
    else {
	d->len = newLen;
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
	d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
	d->setDirty();
    }
}

/*!
    \fn uint TQString::capacity() const

    Returns the number of characters this string can hold
    in the allocated memory.

    \sa reserve(), squeeze()
*/

/*!
    Ensures that at least \a minCapacity characters are allocated to
    the string.

    This function is useful for code that needs to build up a long
    string and wants to avoid repeated reallocation. In this example,
    we want to add to the string until some condition is true, and
    we're fairly sure that size is big enough:
    \code
	TQString result;
	int len = 0;
	result.reserve(maxLen);
	while (...) {
	    result[len++] = ...         // fill part of the space
	}
	result.squeeze();
    \endcode

    If \e maxLen is an underestimate, the worst that will happen is
    that the loop will slow down.

    If it is not possible to allocate enough memory, the string
    remains unchanged.

    \sa capacity(), squeeze(), setLength()
*/

void TQString::reserve( uint minCapacity )
{
    if ( d->maxl < minCapacity ) {
	TQChar *nd = QT_ALLOC_QCHAR_VEC( minCapacity );
	if ( nd ) {
	    uint len = d->len;
	    if ( len )
		memcpy( nd, d->unicode, sizeof(TQChar) * len );
	    bool unpaged = d->security_unpaged;
	    deref();
	    d = new TQStringData( nd, len, minCapacity );
	    setSecurityUnPaged(unpaged);
	}
    }
}


/*!
    Squeezes the string's capacity to the current content.

    \sa capacity(), reserve()
*/
void TQString::squeeze()
{
    if ( d->maxl > d->len ) {
	TQChar *nd = QT_ALLOC_QCHAR_VEC( d->len );
	if ( nd ) {
	    uint len = d->len;
	    if ( len )
		memcpy( nd, d->unicode, sizeof(TQChar) * len );
	    bool unpaged = d->security_unpaged;
	    deref();
	    d = new TQStringData( nd, len, len );
	    setSecurityUnPaged(unpaged);
	}
    }
}

/*!
    \internal

    Like setLength, but doesn't shrink the allocated memory.
*/
void TQString::grow( uint newLen )
{
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
    d->mutex->lock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE

    if ( d->count != 1 || newLen > d->maxl ) {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
	d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
	setLength( newLen );
    }
    else {
	d->len = newLen;
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
	d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
	d->setDirty();
    }
}

struct ArgEscapeData
{
    uint min_escape;	    // lowest escape sequence number
    uint occurrences;	    // number of occurences of the lowest escape
    	    	    	    // sequence number
    uint locale_occurrences; // number of occurences of the lowest escape
    	    	    	    // sequence number which contain 'L'
    uint escape_len;	    // total length of escape sequences which will
    	    	    	    // be replaced
};

static ArgEscapeData findArgEscapes(const TQString &s)
{
    const TQChar *uc_begin = s.unicode();
    const TQChar *uc_end = uc_begin + s.length();

    ArgEscapeData d;

    d.min_escape = 10;
    d.occurrences = 0;
    d.escape_len = 0;
    d.locale_occurrences = 0;

    const TQChar *c = uc_begin;
    while (c != uc_end) {
    	while (c != uc_end && c->unicode() != '%')
	    ++c;

	if (c == uc_end || ++c == uc_end)
	    break;

	bool locale_arg = false;
    	if (c->unicode() == 'L') {
	    locale_arg = true;
	    if (++c == uc_end)
		break;
	}

    	if (c->unicode() < '0' || c->unicode() > '9')
	    continue;

	uint escape = c->unicode() - '0';
    	++c;

    	if (escape > d.min_escape)
	    continue;

    	if (escape < d.min_escape) {
	    d.min_escape = escape;
	    d.occurrences = 0;
	    d.escape_len = 0;
	    d.locale_occurrences = 0;
	}

#if TQT_VERSION < 0x040000
    	// ### remove preprocessor in TQt 4.0
	/* Since in TQt < 4.0 only the first instance is replaced,
	   escape_len should hold the length of only the first escape
	   sequence */
    	if (d.occurrences == 0)
#endif
    	{
	    ++d.occurrences;
    	    if (locale_arg) {
    	    	++d.locale_occurrences;
		d.escape_len += 3;
	    }
	    else
		d.escape_len += 2;
	}
    }

    return d;
}

static TQString replaceArgEscapes(const TQString &s, const ArgEscapeData &d, int field_width,
    	    	    	    	    const TQString &arg, const TQString &larg)
{
    const TQChar *uc_begin = s.unicode();
    const TQChar *uc_end = uc_begin + s.length();

    uint abs_field_width = TQABS(field_width);
    uint result_len = s.length()
    	    	    	- d.escape_len
			+ (d.occurrences - d.locale_occurrences)
			    *TQMAX(abs_field_width, arg.length())
			+ d.locale_occurrences
			    *TQMAX(abs_field_width, larg.length());

    TQString result;
    result.setLength(result_len);
    TQChar *result_buff = (TQChar*) result.unicode();

    TQChar *rc = result_buff;
    const TQChar *c = uc_begin;
    uint repl_cnt = 0;
    while (c != uc_end) {
    	/* We don't have to check if we run off the end of the string with c,
	   because as long as d.occurrences > 0 we KNOW there are valid escape
	   sequences. */

    	const TQChar *text_start = c;

    	while (c->unicode() != '%')
	    ++c;

	const TQChar *escape_start = c++;

	bool locale_arg = false;
    	if (c->unicode() == 'L') {
	    locale_arg = true;
	    ++c;
	}

    	if (c->unicode() != '0' + d.min_escape) {
	    memcpy(rc, text_start, (c - text_start)*sizeof(TQChar));
	    rc += c - text_start;
	}
	else {
    	    ++c;

	    memcpy(rc, text_start, (escape_start - text_start)*sizeof(TQChar));
	    rc += escape_start - text_start;

    	    uint pad_chars;
    	    if (locale_arg)
	    	pad_chars = TQMAX(abs_field_width, larg.length()) - larg.length();
	    else
	    	pad_chars = TQMAX(abs_field_width, arg.length()) - arg.length();

    	    if (field_width > 0) { // left padded
		for (uint i = 0; i < pad_chars; ++i)
	    	    (rc++)->unicode() = ' ';
	    }

    	    if (locale_arg) {
	    	memcpy(rc, larg.unicode(), larg.length()*sizeof(TQChar));
		rc += larg.length();
	    }
	    else {
	    	memcpy(rc, arg.unicode(), arg.length()*sizeof(TQChar));
	    	rc += arg.length();
	    }

    	    if (field_width < 0) { // right padded
		for (uint i = 0; i < pad_chars; ++i)
	    	    (rc++)->unicode() = ' ';
	    }

	    if (++repl_cnt == d.occurrences) {
		memcpy(rc, c, (uc_end - c)*sizeof(TQChar));
		rc += uc_end - c;
		Q_ASSERT(rc - result_buff == (int)result_len);
		c = uc_end;
	    }
	}
    }

    return result;
}

/*!
    This function will return a string that replaces the lowest
    numbered occurrence of \c %1, \c %2, ..., \c %9 with \a a.

    The \a fieldWidth value specifies the minimum amount of space that
    \a a is padded to. A positive value will produce right-aligned
    text, whereas a negative value will produce left-aligned text.

    The following example shows how we could create a 'status' string
    when processing a list of files:
    \code
    TQString status = TQString( "Processing file %1 of %2: %3" )
			.arg( i )         // current file's number
			.arg( total )     // number of files to process
			.arg( fileName ); // current file's name
    \endcode

    It is generally fine to use filenames and numbers as we have done
    in the example above. But note that using arg() to construct
    natural language sentences does not usually translate well into
    other languages because sentence structure and word order often
    differ between languages.

    If there is no place marker (\c %1, \c %2, etc.), a warning
    message (tqWarning()) is output and the result is undefined.

    \warning If any placeholder occurs more than once, the result is undefined.

*/
TQString TQString::arg( const TQString& a, int fieldWidth ) const
{
    ArgEscapeData d = findArgEscapes(*this);

    if (d.occurrences == 0) {
        tqWarning( "TQString::arg(): Argument missing: %s, %s", latin1(),
                  a.latin1() );
        return *this;
    }

    return replaceArgEscapes(*this, d, fieldWidth, a, a);
}

/*!
    \fn TQString TQString::arg( const TQString& a1, const TQString& a2 ) const

    \overload

    This is the same as str.arg(\a a1).arg(\a a2), except that
    the strings are replaced in one pass. This can make a difference
    if \a a1 contains e.g. \c{%1}:

    \code
    TQString str( "%1 %2" );
    str.arg( "Hello", "world" );        // returns "Hello world"
    str.arg( "Hello" ).arg( "world" );  // returns "Hello world"

    str.arg( "(%1)", "Hello" );           // returns "(%1) Hello"
    str.arg( "(%1)" ).arg( "Hello" );     // returns "(Hello) %2"
    \endcode
*/

/*!
    \fn TQString TQString::arg( const TQString& a1, const TQString& a2,
			      const TQString& a3 ) const
    \overload

    This is the same as calling str.arg(\a a1).arg(\a a2).arg(\a a3),
    except that the strings are replaced in one pass.
*/

/*!
    \fn TQString TQString::arg( const TQString& a1, const TQString& a2,
			      const TQString& a3, const TQString& a4 ) const
    \overload

    This is the same as calling
    str.arg(\a a1).arg(\a a2).arg(\a a3).arg(\a a4),
    except that the strings are replaced in one pass.
*/

/*!
    \overload

    The \a fieldWidth value specifies the minimum amount of space that
    \a a is padded to. A positive value will produce a right-aligned
    number, whereas a negative value will produce a left-aligned
    number.

    \a a is expressed in base \a base, which is 10 by default and must
    be between 2 and 36.

    The '%' can be followed by an 'L', in which case the sequence is
    replaced with a localized representation of \a a. The conversion
    uses the default locale. The default locale is determined from the
    system's locale settings at application startup. It can be changed
    using TQLocale::setDefault(). The 'L' flag is ignored if \a base is
    not 10.

    \code
	TQString str;
	str = TQString( "Decimal 63 is %1 in hexadecimal" )
		.arg( 63, 0, 16 );
	// str == "Decimal 63 is 3f in hexadecimal"

	TQLocale::setDefault(TQLocale::English, TQLocale::UnitedStates);
	str = TQString( "%1 %L2 %L3" )
		.arg( 12345 )
		.arg( 12345 )
		.arg( 12345, 0, 16 );
	// str == "12345 12,345 3039"
    \endcode
*/
TQString TQString::arg( long a, int fieldWidth, int base ) const
{
    return arg((TQ_LLONG)a, fieldWidth, base);
}

/*!
    \overload

    \a a is expressed in base \a base, which is 10 by default and must
    be between 2 and 36. If \a base is 10, the '%L' syntax can be used
    to produce localized strings.
*/
TQString TQString::arg( ulong a, int fieldWidth, int base ) const
{
    return arg((TQ_ULLONG)a, fieldWidth, base);
}

/*!
    \overload

    \a a is expressed in base \a base, which is 10 by default and must
    be between 2 and 36. If \a base is 10, the '%L' syntax can be used
    to produce localized strings.
*/
TQString TQString::arg( TQ_LLONG a, int fieldWidth, int base ) const
{
    ArgEscapeData d = findArgEscapes(*this);

    if (d.occurrences == 0) {
        tqWarning( "TQString::arg(): Argument missing: %s, %lld", latin1(),
                  a );
        return *this;
    }

    TQString arg;
    if (d.occurrences > d.locale_occurrences)
    	arg = number(a, base);

    TQString locale_arg;
    if (d.locale_occurrences > 0) {
	TQLocale locale;
	locale_arg = locale.d->longLongToString(a, -1, base, -1, TQLocalePrivate::ThousandsGroup);
    }

    return replaceArgEscapes(*this, d, fieldWidth, arg, locale_arg);
}

/*!
    \overload

    \a a is expressed in base \a base, which is 10 by default and must
    be between 2 and 36. If \a base is 10, the '%L' syntax can be used
    to produce localized strings.
*/
TQString TQString::arg( TQ_ULLONG a, int fieldWidth, int base ) const
{
    ArgEscapeData d = findArgEscapes(*this);

    if (d.occurrences == 0) {
        tqWarning( "TQString::arg(): Argument missing: %s, %llu", latin1(),
                  a );
        return *this;
    }

    TQString arg;
    if (d.occurrences > d.locale_occurrences)
    	arg = number(a, base);

    TQString locale_arg;
    if (d.locale_occurrences > 0) {
	TQLocale locale;
	locale_arg = locale.d->unsLongLongToString(a, -1, base, -1, TQLocalePrivate::ThousandsGroup);
    }

    return replaceArgEscapes(*this, d, fieldWidth, arg, locale_arg);
}

/*!
    \fn TQString TQString::arg( int a, int fieldWidth, int base ) const

    \overload

    \a a is expressed in base \a base, which is 10 by default and must
    be between 2 and 36. If \a base is 10, the '%L' syntax can be used
    to produce localized strings.
*/

/*!
    \fn TQString TQString::arg( uint a, int fieldWidth, int base ) const

    \overload

    \a a is expressed in base \a base, which is 10 by default and must
    be between 2 and 36. If \a base is 10, the '%L' syntax can be used
    to produce localized strings.
*/

/*!
    \fn TQString TQString::arg( short a, int fieldWidth, int base ) const

    \overload

    \a a is expressed in base \a base, which is 10 by default and must
    be between 2 and 36. If \a base is 10, the '%L' syntax can be used
    to produce localized strings.
*/

/*!
    \fn TQString TQString::arg( ushort a, int fieldWidth, int base ) const

    \overload

    \a a is expressed in base \a base, which is 10 by default and must
    be between 2 and 36. If \a base is 10, the '%L' syntax can be used
    to produce localized strings.
*/


/*!
    \overload

    \a a is assumed to be in the Latin-1 character set.
*/
TQString TQString::arg( char a, int fieldWidth ) const
{
    TQString c;
    c += a;
    return arg( c, fieldWidth );
}

/*!
    \overload
*/
TQString TQString::arg( TQChar a, int fieldWidth ) const
{
    TQString c;
    c += a;
    return arg( c, fieldWidth );
}

/*!
    \overload

    \target arg-formats

    Argument \a a is formatted according to the \a fmt format specified,
    which is 'g' by default and can be any of the following:

    \table
    \header \i Format \i Meaning
    \row \i \c e \i format as [-]9.9e[+|-]999
    \row \i \c E \i format as [-]9.9E[+|-]999
    \row \i \c f \i format as [-]9.9
    \row \i \c g \i use \c e or \c f format, whichever is the most concise
    \row \i \c G \i use \c E or \c f format, whichever is the most concise
    \endtable

    With 'e', 'E', and 'f', \a prec is the number of digits after the
    decimal point. With 'g' and 'G', \a prec is the maximum number of
    significant digits (trailing zeroes are omitted).

    \code
        double d = 12.34;
        TQString ds = TQString( "'E' format, precision 3, gives %1" )
                        .arg( d, 0, 'E', 3 );
        // ds == "'E' format, precision 3, gives 1.234E+01"
    \endcode

    The '%L' syntax can be used to produce localized strings.
*/
TQString TQString::arg( double a, int fieldWidth, char fmt, int prec ) const
{
    ArgEscapeData d = findArgEscapes(*this);

    if (d.occurrences == 0) {
        tqWarning( "TQString::arg(): Argument missing: %s, %g", latin1(),
                  a );
        return *this;
    }

    TQString arg;
    if (d.occurrences > d.locale_occurrences)
    	arg = number(a, fmt, prec);

    TQString locale_arg;
    if (d.locale_occurrences > 0) {
	TQLocale locale;

	TQLocalePrivate::DoubleForm form = TQLocalePrivate::DFDecimal;
	uint flags = 0;

	if (qIsUpper(fmt))
    	    flags = TQLocalePrivate::CapitalEorX;
	fmt = qToLower(fmt);

	switch (fmt) {
	    case 'f':
		form = TQLocalePrivate::DFDecimal;
		break;
	    case 'e':
		form = TQLocalePrivate::DFExponent;
		break;
	    case 'g':
		form = TQLocalePrivate::DFSignificantDigits;
		break;
	    default:
#if defined(QT_CHECK_RANGE)
		tqWarning( "TQString::setNum: Invalid format char '%c'", fmt );
#endif
		break;
	}

    	flags |= TQLocalePrivate::ThousandsGroup;

	locale_arg = locale.d->doubleToString(a, prec, form, -1, flags);
    }

    return replaceArgEscapes(*this, d, fieldWidth, arg, locale_arg);
}

TQString TQString::multiArg( int numArgs, const TQString& a1, const TQString& a2,
			   const TQString& a3, const TQString& a4 ) const
{
    TQString result;
    union {
	int digitUsed[10];
	int argForDigit[10];
    };
    const TQChar *uc = d->unicode;
    const TQString *args[4];
    const int len = (int) length();
    const int end = len - 1;
    int lastDigit = -1;
    int i;

    memset( digitUsed, 0, sizeof(digitUsed) );
    args[0] = &a1;
    args[1] = &a2;
    args[2] = &a3;
    args[3] = &a4;

    for ( i = 0; i < end; i++ ) {
	if ( uc[i] == '%' ) {
	    int digit = uc[i + 1].unicode() - '0';
	    if ( digit >= 0 && digit <= 9 )
		digitUsed[digit]++;
	}
    }

    for ( i = 0; i < numArgs; i++ ) {
	do {
	    ++lastDigit;
	} while ( lastDigit < 10 && digitUsed[lastDigit] == 0 );

	if ( lastDigit == 10 ) {
	    tqWarning( "TQString::arg(): Argument missing: %s, %s",
		      latin1(), args[i]->latin1() );
	    numArgs = i;
	    lastDigit = 9;
	    break;
	}
	argForDigit[lastDigit] = i;
    }

    i = 0;
    while ( i < len ) {
	if ( uc[i] == '%' && i != end ) {
	    int digit = uc[i + 1].unicode() - '0';
	    if ( digit >= 0 && digit <= lastDigit ) {
		result += *args[argForDigit[digit]];
		i += 2;
		continue;
	    }
	}
	result += uc[i++];
    }
    return result;
}


/*!
    Safely builds a formatted string from the format string \a cformat
    and an arbitrary list of arguments. The format string supports all
    the escape sequences of printf() in the standard C library.

    The %s escape sequence expects a utf8() encoded string. The format
    string \e cformat is expected to be in latin1. If you need a
    Unicode format string, use arg() instead. For typesafe string
    building, with full Unicode support, you can use TQTextOStream like
    this:

    \code
	TQString str;
	TQString s = ...;
	int x = ...;
	TQTextOStream( &str ) << s << " : " << x;
    \endcode

    For \link TQObject::tr() translations,\endlink especially if the
    strings contains more than one escape sequence, you should
    consider using the arg() function instead. This allows the order
    of the replacements to be controlled by the translator, and has
    Unicode support.

    The %lc escape sequence expects a unicode character of type ushort
    (as returned by TQChar::unicode()).
    The %ls escape sequence expects a pointer to a zero-terminated
    array of unicode characters of type ushort (as returned by
    TQString::ucs2()).

    \sa arg()
*/

#ifndef TQT_NO_SPRINTF
TQString &TQString::sprintf(const char *cformat, ...)
{
    va_list ap;
    va_start(ap, cformat);

    if ( !cformat || !*cformat ) {
	// TQt 1.x compat
	*this = fromLatin1( "" );
    } else {
	vsprintf(cformat, ap);
    }

    va_end(ap);
    return *this;
}

TQString &TQString::vsprintf( const char* cformat, va_list ap )
{
    TQLocale locale(TQLocale::C);

    // Parse cformat

    TQString result;
    const char *c = cformat;
    for (;;) {
    	// Copy non-escape chars to result
    	while (*c != '\0' && *c != '%')
	    result.append(*c++);

	if (*c == '\0')
	    break;

	// Found '%'
	const char *escape_start = c;
	++c;

	if (*c == '\0') {
	    result.append('%'); // a % at the end of the string - treat as non-escape text
	    break;
	}
    	if (*c == '%') {
	    result.append('%'); // %%
	    ++c;
	    continue;
	}

	// Parse flag characters
	unsigned flags = 0;
	bool no_more_flags = false;
	do {
	    switch (*c) {
		case '#': flags |= TQLocalePrivate::Alternate; break;
		case '0': flags |= TQLocalePrivate::ZeroPadded; break;
		case '-': flags |= TQLocalePrivate::LeftAdjusted; break;
		case ' ': flags |= TQLocalePrivate::BlankBeforePositive; break;
		case '+': flags |= TQLocalePrivate::AlwaysShowSign; break;
		case '\'': flags |= TQLocalePrivate::ThousandsGroup; break;
		default: no_more_flags = true; break;
    	    }

	    if (!no_more_flags)
	    	++c;
	} while (!no_more_flags);

	if (*c == '\0') {
	    result.append(escape_start); // incomplete escape, treat as non-escape text
	    break;
	}

	// Parse field width
	int width = -1; // -1 means unspecified
	if (qIsDigit(*c)) {
	    TQString width_str;
	    while (*c != '\0' && qIsDigit(*c))
	    	width_str.append(*c++);

	    // can't be negative - started with a digit
    	    // contains at least one digit
	    width = width_str.toInt();
	}
	else if (*c == '*') {
	    width = va_arg(ap, int);
	    if (width < 0)
	    	width = -1; // treat all negative numbers as unspecified
	    ++c;
	}

	if (*c == '\0') {
	    result.append(escape_start); // incomplete escape, treat as non-escape text
	    break;
	}

	// Parse precision
	int precision = -1; // -1 means unspecified
	if (*c == '.') {
	    ++c;
	    if (qIsDigit(*c)) {
		TQString precision_str;
		while (*c != '\0' && qIsDigit(*c))
	    	    precision_str.append(*c++);

		// can't be negative - started with a digit
    	    	// contains at least one digit
		precision = precision_str.toInt();
	    }
	    else if (*c == '*') {
		precision = va_arg(ap, int);
		if (precision < 0)
	    	    precision = -1; // treat all negative numbers as unspecified
		++c;
	    }
	}

	if (*c == '\0') {
	    result.append(escape_start); // incomplete escape, treat as non-escape text
	    break;
	}

    	// Parse the length modifier
    	enum LengthMod { lm_none, lm_hh, lm_h, lm_l, lm_ll, lm_L, lm_j, lm_z, lm_t };
	LengthMod length_mod = lm_none;
	switch (*c) {
	    case 'h':
	    	++c;
		if (*c == 'h') {
		    length_mod = lm_hh;
		    ++c;
		}
		else
		    length_mod = lm_h;
		break;

	    case 'l':
	    	++c;
		if (*c == 'l') {
		    length_mod = lm_ll;
		    ++c;
		}
		else
		    length_mod = lm_l;
		break;

	    case 'L':
	    	++c;
		length_mod = lm_L;
		break;

	    case 'j':
	    	++c;
		length_mod = lm_j;
		break;

	    case 'z':
	    case 'Z':
	    	++c;
		length_mod = lm_z;
		break;

	    case 't':
	    	++c;
		length_mod = lm_t;
		break;

	    default: break;
	}

	if (*c == '\0') {
	    result.append(escape_start); // incomplete escape, treat as non-escape text
	    break;
	}

	// Parse the conversion specifier and do the conversion
	TQString subst;
	switch (*c) {
	    case 'd':
	    case 'i': {
	    	TQ_LLONG i;
		switch (length_mod) {
		    case lm_none: i = va_arg(ap, int); break;
		    case lm_hh: i = va_arg(ap, int); break;
		    case lm_h: i = va_arg(ap, int); break;
		    case lm_l: i = va_arg(ap, long int); break;
		    case lm_ll: i = va_arg(ap, TQ_LLONG); break;
		    case lm_j: i = va_arg(ap, long int); break;
		    case lm_z: i = va_arg(ap, size_t); break;
		    case lm_t: i = va_arg(ap, int); break;
                    default: i = 0; break;
		}
		subst = locale.d->longLongToString(i, precision, 10, width, flags);
		++c;
		break;
    	    }
	    case 'o':
	    case 'u':
	    case 'x':
	    case 'X': {
	    	TQ_ULLONG u;
		switch (length_mod) {
		    case lm_none: u = va_arg(ap, unsigned int); break;
		    case lm_hh: u = va_arg(ap, unsigned int); break;
		    case lm_h: u = va_arg(ap, unsigned int); break;
		    case lm_l: u = va_arg(ap, unsigned long int); break;
		    case lm_ll: u = va_arg(ap, TQ_ULLONG); break;
                    default: u = 0; break;
		}

		if (qIsUpper(*c))
		    flags |= TQLocalePrivate::CapitalEorX;

    	    	int base = 10;
		switch (qToLower(*c)) {
		    case 'o':
	    		base = 8; break;
		    case 'u':
			base = 10; break;
		    case 'x':
			base = 16; break;
		    default: break;
		}
		subst = locale.d->unsLongLongToString(u, precision, base, width, flags);
		++c;
		break;
    	    }
	    case 'E':
	    case 'e':
    	    case 'F':
	    case 'f':
	    case 'G':
	    case 'g':
	    case 'A':
	    case 'a': {
	    	double d;
		if (length_mod == lm_L)
		    d = va_arg(ap, long double); // not supported - converted to a double
		else
		    d = va_arg(ap, double);

		if (qIsUpper(*c))
		    flags |= TQLocalePrivate::CapitalEorX;

    	    	TQLocalePrivate::DoubleForm form = TQLocalePrivate::DFDecimal;
		switch (qToLower(*c)) {
		    case 'e': form = TQLocalePrivate::DFExponent; break;
		    case 'a': 	    	    	// not supported - decimal form used instead
		    case 'f': form = TQLocalePrivate::DFDecimal; break;
		    case 'g': form = TQLocalePrivate::DFSignificantDigits; break;
		    default: break;
		}
		subst = locale.d->doubleToString(d, precision, form, width, flags);
		++c;
		break;
	    }
	    case 'c': {
	    	if (length_mod == lm_l)
		    subst = TQChar((ushort) va_arg(ap, int));
    	    	else
		    subst = (uchar) va_arg(ap, int);
		++c;
		break;
	    }
	    case 's': {
	    	if (length_mod == lm_l) {
		    const ushort *buff = va_arg(ap, const ushort*);
		    const ushort *ch = buff;
		    while (*ch != 0)
		    	++ch;
		    subst.setUnicodeCodes(buff, ch - buff);
		} else
	    	    subst = TQString::fromUtf8(va_arg(ap, const char*));
		if (precision != -1)
		    subst.truncate(precision);
		++c;
		break;
	    }
	    case 'p': {
		TQ_ULLONG i;
#ifdef Q_OS_WIN64
	    	i = (TQ_ULLONG) va_arg(ap, void*);
#else
		i = (TQ_ULONG) va_arg(ap, void*);
#endif

#ifdef Q_OS_WIN32
		flags |= TQLocalePrivate::CapitalEorX; // Windows does 1234ABCD
#else
		flags |= TQLocalePrivate::Alternate; // Unix and Mac do 0x1234abcd
#endif

		subst = locale.d->unsLongLongToString(i, precision, 16, width, flags);
		++c;
		break;
	    }
	    case 'n':
	    	switch (length_mod) {
		    case lm_hh: {
		    	signed char *n = va_arg(ap, signed char*);
			*n = result.length();
			break;
		    }
		    case lm_h: {
		    	short int *n = va_arg(ap, short int*);
			*n = result.length();
    	    	    	break;
		    }
		    case lm_l: {
		    	long int *n = va_arg(ap, long int*);
			*n = result.length();
			break;
		    }
		    case lm_ll: {
		    	TQ_LLONG *n = va_arg(ap, TQ_LLONG*);
			volatile uint tmp = result.length(); // egcs-2.91.66 gets internal
			*n = tmp;			     // compiler error without volatile
			break;
		    }
		    default: {
		    	int *n = va_arg(ap, int*);
			*n = result.length();
			break;
		    }
		}
		++c;
		break;

	    default: // bad escape, treat as non-escape text
	    	for (const char *cc = escape_start; cc != c; ++cc)
		    result.append(*cc);
		continue;
	}

	if (flags & TQLocalePrivate::LeftAdjusted)
	    result.append(subst.leftJustify(width));
	else
	    result.append(subst.rightJustify(width));
    }

    *this = result;

    return *this;
}
#endif

/*!
    Fills the string with \a len characters of value \a c, and returns
    a reference to the string.

    If \a len is negative (the default), the current string length is
    used.

    \code
	TQString str;
	str.fill( 'g', 5 );      // string == "ggggg"
    \endcode
*/

TQString& TQString::fill( TQChar c, int len )
{
    if ( len < 0 )
	len = length();
    if ( len == 0 ) {
	*this = "";
    } else {
	bool unpaged = d->security_unpaged;
	deref();
	TQChar * nd = QT_ALLOC_QCHAR_VEC( len );
	d = new TQStringData(nd,len,len);
	setSecurityUnPaged(unpaged);
	while (len--) *nd++ = c;
    }
    return *this;
}


/*!
  \fn TQString TQString::copy() const

  \obsolete

  In TQt 2.0 and later, all calls to this function are needless. Just
  remove them.
*/

/*!
    \overload

    Finds the first occurrence of the character \a c, starting at
    position \a index. If \a index is -1, the search starts at the
    last character; if -2, at the next to last character and so on.
    (See findRev() for searching backwards.)

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.

    Returns the position of \a c or -1 if \a c could not be found.
*/

int TQString::find( TQChar c, int index, bool cs ) const
{
    const uint l = length();
    if ( index < 0 )
	index += l;
    if ( (uint)index >= l )
	return -1;
    const TQChar *uc = unicode()+index;
    const TQChar *end = unicode() + l;
    if ( cs ) {
	while ( uc < end && *uc != c )
	    uc++;
    } else {
	c = ::lower( c );
	while ( uc < end && ::lower( *uc ) != c )
	    uc++;
    }
    if ( uint(uc - unicode()) >= l )
	return -1;
    return (int)(uc - unicode());
}

/* an implementation of the Boyer-Moore search algorithm
*/

/* initializes the skiptable to know haw far ahead we can skip on a wrong match
*/
static void bm_init_skiptable( const TQString &pattern, uint *skiptable, bool cs )
{
    int i = 0;
    uint *st = skiptable;
    int l = pattern.length();
    while ( i++ < 0x100/8 ) {
	*(st++) = l;
	*(st++) = l;
	*(st++) = l;
	*(st++) = l;
	*(st++) = l;
	*(st++) = l;
	*(st++) = l;
	*(st++) = l;
    }
    const TQChar *uc = pattern.unicode();
    if ( cs ) {
	while ( l-- ) {
	    skiptable[ uc->cell() ] = l;
	    uc++;
	}
    } else {
	while ( l-- ) {
	    skiptable[ ::lower( *uc ).cell() ] = l;
	    uc++;
	}
    }
}

static int bm_find( const TQString &str, int index, const TQString &pattern, uint *skiptable, bool cs )
{
    const uint l = str.length();
    if ( pattern.isEmpty() )
	return index > (int)l ? -1 : index;

    const TQChar *uc = str.unicode();
    const TQChar *puc = pattern.unicode();
    const uint pl = pattern.length();
    const uint pl_minus_one = pl - 1;

    const TQChar *current = uc + index + pl_minus_one;
    const TQChar *end = uc + l;
    if ( cs ) {
	while ( current < end ) {
	    uint skip = skiptable[ current->cell() ];
	    if ( !skip ) {
		// possible match
		while ( skip < pl ) {
		    if ( *(current - skip ) != puc[pl_minus_one-skip] )
			break;
		    skip++;
		}
		if ( skip > pl_minus_one ) { // we have a match
		    return (current - uc) - skip + 1;
		}
		// in case we don't have a match we are a bit inefficient as we only skip by one
		// when we have the non matching char in the string.
		if ( skiptable[ (current-skip)->cell() ] == pl )
		    skip = pl - skip;
		else
		    skip = 1;
	    }
	    current += skip;
	}
    } else {
	while ( current < end ) {
	    uint skip = skiptable[ ::lower( *current ).cell() ];
	    if ( !skip ) {
		// possible match
		while ( skip < pl ) {
		    if ( ::lower( *(current - skip) ) != ::lower( puc[pl_minus_one-skip] ) )
			break;
		    skip++;
		}
		if ( skip > pl_minus_one ) // we have a match
		    return (current - uc) - skip + 1;
		// in case we don't have a match we are a bit inefficient as we only skip by one
		// when we have the non matching char in the string.
		if ( skiptable[ ::lower(*(current - skip)).cell() ] == pl )
		    skip = pl - skip;
		else
		    skip = 1;
	    }
	    current += skip;
	}
    }
    // not found
    return -1;
}


#define REHASH( a ) \
    if ( sl_minus_1 < sizeof(uint) * CHAR_BIT ) \
	hashHaystack -= (a) << sl_minus_1; \
    hashHaystack <<= 1

/*!
    \overload

    Finds the first occurrence of the string \a str, starting at
    position \a index. If \a index is -1, the search starts at the
    last character, if it is -2, at the next to last character and so
    on. (See findRev() for searching backwards.)

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.

    Returns the position of \a str or -1 if \a str could not be found.
*/

int TQString::find( const TQString& str, int index, bool cs ) const
{
    const uint l = length();
    const uint sl = str.length();
    if ( index < 0 )
	index += l;
    if ( sl + index > l )
	return -1;
    if ( !sl )
	return index;
    if (!l)
	return -1;

#if defined(Q_OS_MACX) && defined(QT_MACOSX_VERSION) && QT_MACOSX_VERSION >= 0x1020
    if ( sl == 1 )
	return find( *str.unicode(), index, cs );
#endif

    // we use the Boyer-Moore algorithm in cases where the overhead
    // for the hash table should pay off, otherwise we use a simple
    // hash function
    if ( l > 500 && sl > 5 ) {
	uint skiptable[0x100];
	bm_init_skiptable( str, skiptable, cs );
	return bm_find( *this, index, str, skiptable, cs );
    }

    /*
      We use some hashing for efficiency's sake. Instead of
      comparing strings, we compare the hash value of str with that of
      a part of this TQString. Only if that matches, we call ucstrncmp
      or ucstrnicmp.
    */
    const TQChar* needle = str.unicode();
    const TQChar* haystack = unicode() + index;
    const TQChar* end = unicode() + (l-sl);
    const uint sl_minus_1 = sl-1;
    uint hashNeedle = 0, hashHaystack = 0, i;

    if ( cs ) {
	for ( i = 0; i < sl; ++i ) {
	    hashNeedle = ((hashNeedle<<1) + needle[i].unicode() );
	    hashHaystack = ((hashHaystack<<1) + haystack[i].unicode() );
	}
	hashHaystack -= (haystack+sl_minus_1)->unicode();

	while ( haystack <= end ) {
	    hashHaystack += (haystack+sl_minus_1)->unicode();
 	    if ( hashHaystack == hashNeedle
		 && ucstrncmp( needle, haystack, sl ) == 0 )
		return haystack-unicode();

	    REHASH( haystack->unicode() );
	    ++haystack;
	}
    } else {
	for ( i = 0; i < sl; ++i ) {
	    hashNeedle = ((hashNeedle<<1) +
			  ::lower( needle[i].unicode() ).unicode() );
	    hashHaystack = ((hashHaystack<<1) +
			    ::lower( haystack[i].unicode() ).unicode() );
	}

	hashHaystack -= ::lower(*(haystack+sl_minus_1)).unicode();
	while ( haystack <= end ) {
	    hashHaystack += ::lower(*(haystack+sl_minus_1)).unicode();
	    if ( hashHaystack == hashNeedle
		 && ucstrnicmp( needle, haystack, sl ) == 0 )
		return haystack-unicode();

	    REHASH( ::lower(*haystack).unicode() );
	    ++haystack;
	}
    }
    return -1;
}

/*!
    \fn int TQString::findRev( const char* str, int index ) const

    Equivalent to findRev(TQString(\a str), \a index).
*/

/*!
    \fn int TQString::find( const char* str, int index ) const

    \overload

    Equivalent to find(TQString(\a str), \a index).
*/

/*!
    \overload

    Finds the first occurrence of the character \a c, starting at
    position \a index and searching backwards. If the index is -1, the
    search starts at the last character, if it is -2, at the next to
    last character and so on.

    Returns the position of \a c or -1 if \a c could not be found.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.

    \code
	TQString string( "bananas" );
	int i = string.findRev( 'a' );      // i == 5
    \endcode
*/

int TQString::findRev( TQChar c, int index, bool cs ) const
{
#if defined(Q_OS_MACX) && defined(QT_MACOSX_VERSION) && QT_MACOSX_VERSION < 0x1020
    return findRev( TQString( c ), index, cs );
#else
    const uint l = length();
    if ( index < 0 )
	index += l;
    if ( (uint)index >= l )
	return -1;
    const TQChar *end = unicode();
    const TQChar *uc = end + index;
    if ( cs ) {
	while ( uc >= end && *uc != c )
	    uc--;
    } else {
	c = ::lower( c );
	while ( uc >= end && ::lower( *uc ) != c )
	    uc--;
    }
    return uc - end;
#endif
}

/*!
    \overload

    Finds the first occurrence of the string \a str, starting at
    position \a index and searching backwards. If the index is -1, the
    search starts at the last character, if it is -2, at the next to
    last character and so on.

    Returns the position of \a str or -1 if \a str could not be found.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.

    \code
    TQString string("bananas");
    int i = string.findRev( "ana" );      // i == 3
    \endcode
*/

int TQString::findRev( const TQString& str, int index, bool cs ) const
{
    /*
      See TQString::find() for explanations.
    */
    const uint l = length();
    if ( index < 0 )
	index += l;
    const uint sl = str.length();
    int delta = l-sl;
    if ( index < 0 || index > (int)l || delta < 0 )
	return -1;
    if ( index > delta )
	index = delta;

#if defined(Q_OS_MACX) && defined(QT_MACOSX_VERSION) && QT_MACOSX_VERSION >= 0x1020
    if ( sl == 1 )
	return findRev( *str.unicode(), index, cs );
#endif

    const TQChar* needle = str.unicode();
    const TQChar* haystack = unicode() + index;
    const TQChar* end = unicode();
    const uint sl_minus_1 = sl-1;
    const TQChar* n = needle+sl_minus_1;
    const TQChar* h = haystack+sl_minus_1;
    uint hashNeedle = 0, hashHaystack = 0, i;

    if ( cs ) {
	for ( i = 0; i < sl; ++i ) {
	    hashNeedle = ((hashNeedle<<1) + (n-i)->unicode() );
	    hashHaystack = ((hashHaystack<<1) + (h-i)->unicode() );
	}
	hashHaystack -= haystack->unicode();

	while ( haystack >= end ) {
	    hashHaystack += haystack->unicode();
 	    if ( hashHaystack == hashNeedle
		 && ucstrncmp( needle, haystack, sl ) == 0 )
		return haystack-unicode();
	    --haystack;
	    REHASH( (haystack+sl)->unicode() );
	}
    } else {
	for ( i = 0; i < sl; ++i ) {
	    hashNeedle = ((hashNeedle<<1)
			  + ::lower( (n-i)->unicode() ).unicode() );
	    hashHaystack = ((hashHaystack<<1)
			    + ::lower( (h-i)->unicode() ).unicode() );
	}
	hashHaystack -= ::lower(*haystack).unicode();

	while ( haystack >= end ) {
	    hashHaystack += ::lower(*haystack).unicode();
	    if ( hashHaystack == hashNeedle
		 && ucstrnicmp( needle, haystack, sl ) == 0 )
		return haystack-unicode();
	    --haystack;
	    REHASH( ::lower(*(haystack+sl)).unicode() );
	}
    }
    return -1;
}

#undef REHASH

/*!
    \enum TQString::SectionFlags

    \value SectionDefault Empty fields are counted, leading and
    trailing separators are not included, and the separator is
    compared case sensitively.

    \value SectionSkipEmpty Treat empty fields as if they don't exist,
    i.e. they are not considered as far as \e start and \e end are
    concerned.

    \value SectionIncludeLeadingSep Include the leading separator (if
    any) in the result string.

    \value SectionIncludeTrailingSep Include the trailing separator
    (if any) in the result string.

    \value SectionCaseInsensitiveSeps Compare the separator
    case-insensitively.

    Any of the last four values can be OR-ed together to form a flag.

    \sa section()
*/

/*!
    \fn TQString TQString::section( TQChar sep, int start, int end = 0xffffffff, int flags = SectionDefault ) const

    This function returns a section of the string.

    This string is treated as a sequence of fields separated by the
    character, \a sep. The returned string consists of the fields from
    position \a start to position \a end inclusive. If \a end is not
    specified, all fields from position \a start to the end of the
    string are included. Fields are numbered 0, 1, 2, etc., counting
    from the left, and -1, -2, etc., counting from right to left.

    The \a flags argument can be used to affect some aspects of the
    function's behaviour, e.g. whether to be case sensitive, whether
    to skip empty fields and how to deal with leading and trailing
    separators; see \l{SectionFlags}.

    \code
    TQString csv( "forename,middlename,surname,phone" );
    TQString s = csv.section( ',', 2, 2 );   // s == "surname"

    TQString path( "/usr/local/bin/myapp" ); // First field is empty
    TQString s = path.section( '/', 3, 4 );  // s == "bin/myapp"
    TQString s = path.section( '/', 3, 3, SectionSkipEmpty ); // s == "myapp"
    \endcode

    If \a start or \a end is negative, we count fields from the right
    of the string, the right-most field being -1, the one from
    right-most field being -2, and so on.

    \code
    TQString csv( "forename,middlename,surname,phone" );
    TQString s = csv.section( ',', -3, -2 );  // s == "middlename,surname"

    TQString path( "/usr/local/bin/myapp" ); // First field is empty
    TQString s = path.section( '/', -1 ); // s == "myapp"
    \endcode

    \sa TQStringList::split()
*/

/*!
    \overload

    This function returns a section of the string.

    This string is treated as a sequence of fields separated by the
    string, \a sep. The returned string consists of the fields from
    position \a start to position \a end inclusive. If \a end is not
    specified, all fields from position \a start to the end of the
    string are included. Fields are numbered 0, 1, 2, etc., counting
    from the left, and -1, -2, etc., counting from right to left.

    The \a flags argument can be used to affect some aspects of the
    function's behaviour, e.g. whether to be case sensitive, whether
    to skip empty fields and how to deal with leading and trailing
    separators; see \l{SectionFlags}.

    \code
    TQString data( "forename**middlename**surname**phone" );
    TQString s = data.section( "**", 2, 2 ); // s == "surname"
    \endcode

    If \a start or \a end is negative, we count fields from the right
    of the string, the right-most field being -1, the one from
    right-most field being -2, and so on.

    \code
    TQString data( "forename**middlename**surname**phone" );
    TQString s = data.section( "**", -3, -2 ); // s == "middlename**surname"
    \endcode

    \sa TQStringList::split()
*/

TQString TQString::section( const TQString &sep, int start, int end, int flags ) const
{
    TQStringList sections = TQStringList::split(sep, *this, true);
    if(sections.isEmpty())
	return TQString();
    if(!(flags & SectionSkipEmpty)) {
	if(start < 0)
	    start += int(sections.count());
	if(end < 0)
	    end += int(sections.count());
    } else {
	int skip = 0;
	for(TQStringList::Iterator it = sections.begin(); it != sections.end(); ++it) {
	    if((*it).isEmpty())
		skip++;
	}
	if(start < 0)
	    start += int(sections.count()) - skip;
	if(end < 0)
	    end += int(sections.count()) - skip;
    }
    int x = 0, run = 0;
    TQString ret;
    for(TQStringList::Iterator it = sections.begin(); x <= end && it != sections.end(); ++it) {
	if(x >= start) {
	    if((*it).isEmpty()) {
		run++;
	    } else {
		if(!ret.isEmpty() || !(flags & SectionSkipEmpty)) {
		    int i_end = run;
		    if(!ret.isEmpty() && !(flags & SectionIncludeTrailingSep))
			i_end++;
		    if((flags & SectionIncludeLeadingSep) && it != sections.begin() && x == start)
			i_end++;
		    for(int i = 0; i < i_end; i++)
			ret += sep;
		} else if((flags & SectionIncludeLeadingSep) && it != sections.begin()) {
		    ret += sep;
		}
		run = 0;
		ret += (*it);
		if((flags & SectionIncludeTrailingSep) && it != sections.end())
		    ret += sep;
	    }
	}
	if(!(*it).isEmpty() || !(flags & SectionSkipEmpty))
	    x++;
    }
    return ret;
}

#ifndef TQT_NO_REGEXP
class section_chunk {
public:
    section_chunk(int l, TQString s) { length = l; string = s; }
    int length;
    TQString string;
};
/*!
    \overload

    This function returns a section of the string.

    This string is treated as a sequence of fields separated by the
    regular expression, \a reg. The returned string consists of the
    fields from position \a start to position \a end inclusive. If \a
    end is not specified, all fields from position \a start to the end
    of the string are included. Fields are numbered 0, 1, 2, etc., counting
    from the left, and -1, -2, etc., counting from right to left.

    The \a flags argument can be used to affect some aspects of the
    function's behaviour, e.g. whether to be case sensitive, whether
    to skip empty fields and how to deal with leading and trailing
    separators; see \l{SectionFlags}.

    \code
    TQString line( "forename\tmiddlename  surname \t \t phone" );
    TQRegExp sep( "\s+" );
    TQString s = line.section( sep, 2, 2 ); // s == "surname"
    \endcode

    If \a start or \a end is negative, we count fields from the right
    of the string, the right-most field being -1, the one from
    right-most field being -2, and so on.

    \code
    TQString line( "forename\tmiddlename  surname \t \t phone" );
    TQRegExp sep( "\\s+" );
    TQString s = line.section( sep, -3, -2 ); // s == "middlename  surname"
    \endcode

    \warning Using this TQRegExp version is much more expensive than
    the overloaded string and character versions.

    \sa TQStringList::split() simplifyWhiteSpace()
*/

TQString TQString::section( const TQRegExp &reg, int start, int end, int flags ) const
{
    const TQChar *uc = unicode();
    if(!uc)
	return TQString();

    TQRegExp sep(reg);
    sep.setCaseSensitive(!(flags & SectionCaseInsensitiveSeps));

    TQPtrList<section_chunk> l;
    l.setAutoDelete(true);
    int n = length(), m = 0, last_m = 0, last = 0, last_len = 0;

    while ( ( m = sep.search( *this, m ) ) != -1 ) {
	l.append(new section_chunk(last_len, TQString(uc + last_m, m - last_m)));
	last_m = m;
        last_len = sep.matchedLength();
	if((m += TQMAX(sep.matchedLength(), 1)) >= n) {
	    last = 1;
	    break;
	}
    }
    if(!last)
	l.append(new section_chunk(last_len, TQString(uc + last_m, n - last_m)));

    if(start < 0)
	start = l.count() + start;
    if(end == -1)
	end = l.count();
    else if(end < 0)
	end = l.count() + end;

    int i = 0;
    TQString ret;
    for ( section_chunk *chk=l.first(); chk; chk=l.next(), i++ ) {
	if((flags & SectionSkipEmpty) && chk->length == (int)chk->string.length()) {
	    if(i <= start)
		start++;
	    end++;
	}
	if(i == start) {
	    ret = (flags & SectionIncludeLeadingSep) ? chk->string : chk->string.mid(chk->length);
	} else if(i > start) {
	    ret += chk->string;
	}
	if(i == end) {
	    if((chk=l.next()) && flags & SectionIncludeTrailingSep)
		ret += chk->string.left(chk->length);
	    break;
	}
    }
    return ret;
}
#endif

/*!
    \fn TQString TQString::section( char sep, int start, int end = 0xffffffff, int flags = SectionDefault ) const

    \overload
*/

/*!
    \fn TQString TQString::section( const char *sep, int start, int end = 0xffffffff, int flags = SectionDefault ) const

    \overload
*/


/*!
    Returns the number of times the character \a c occurs in the
    string.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.

    \code
    TQString string( "Trolltech and TQt" );
    int n = string.contains( 't', false );
    // n == 3
    \endcode
*/

int TQString::contains( TQChar c, bool cs ) const
{
    int count = 0;
    const TQChar *uc = unicode();
    if ( !uc )
	return 0;
    int n = length();
    if ( cs ) {
        while ( n-- ) {
			if ( *uc == c )
			count++;
			uc++;
		}
    } else {
	    c = ::lower( c );
	    while ( n-- ) {
	        if ( ::lower( *uc ) == c )
		    count++;
	        uc++;
	    }
    }
    return count;
}

/*!
    \overload

    Returns the number of times the string \a str occurs in the string.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.
*/
int TQString::contains( const char* str, bool cs ) const
{
    return contains( TQString(str), cs );
}

/*!
    \fn int TQString::contains( char c, bool cs ) const

    \overload
*/

/*!
    \fn int TQString::find( char c, int index, bool cs ) const

    \overload

    Find character \a c starting from position \a index.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.
*/

/*!
    \fn int TQString::findRev( char c, int index, bool cs ) const

    \overload

    Find character \a c starting from position \a index and working
    backwards.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.
*/

/*!
    \overload

    Returns the number of times \a str occurs in the string.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.

    This function counts overlapping strings, so in the example below,
    there are two instances of "ana" in "bananas".

    \code
    TQString str( "bananas" );
    int i = str.contains( "ana" );  // i == 2
    \endcode

    \sa findRev()
*/

int TQString::contains( const TQString &str, bool cs ) const
{
    if ( isNull() )
	return 0;
    int count = 0;
    uint skiptable[0x100];
    bm_init_skiptable( str, skiptable, cs );
    int i = -1;
    // use boyer-moore for the ultimate speed experience
    while ( ( i = bm_find( *this, i + 1, str, skiptable, cs ) ) != -1 )
	count++;
    return count;
}

/*!
    Returns a substring that contains the \a len leftmost characters
    of the string.

    The whole string is returned if \a len exceeds the length of the
    string.

    \code
	TQString s = "Pineapple";
	TQString t = s.left( 4 );    // t == "Pine"
    \endcode

    \sa right(), mid(), isEmpty()
*/

TQString TQString::left( uint len ) const
{
    if ( isEmpty() ) {
	return TQString();
    } else if ( len == 0 ) {                    // ## just for 1.x compat:
	return fromLatin1( "" );
    } else if ( len >= length() ) {
	return *this;
    } else {
	TQString s( len, true );
	memcpy( s.d->unicode, d->unicode, len * sizeof(TQChar) );
	s.d->len = len;
	return s;
    }
}

/*!
    Returns a string that contains the \a len rightmost characters of
    the string.

    If \a len is greater than the length of the string then the whole
    string is returned.

    \code
	TQString string( "Pineapple" );
	TQString t = string.right( 5 );   // t == "apple"
    \endcode

    \sa left(), mid(), isEmpty()
*/

TQString TQString::right( uint len ) const
{
    if ( isEmpty() ) {
	return TQString();
    } else if ( len == 0 ) {                    // ## just for 1.x compat:
	return fromLatin1( "" );
    } else {
	uint l = length();
	if ( len >= l )
	    return *this;
	TQString s( len, true );
	memcpy( s.d->unicode, d->unicode+(l-len), len*sizeof(TQChar) );
	s.d->len = len;
	return s;
    }
}

/*!
    Returns a string that contains the \a len characters of this
    string, starting at position \a index.

    Returns a null string if the string is empty or \a index is out of
    range. Returns the whole string from \a index if \a index + \a len
    exceeds the length of the string.

    \code
	TQString s( "Five pineapples" );
	TQString t = s.mid( 5, 4 );                  // t == "pine"
    \endcode

    \sa left(), right()
*/

TQString TQString::mid( uint index, uint len ) const
{
    uint slen = length();
    if ( isEmpty() || index >= slen ) {
	return TQString();
    } else if ( len == 0 ) {                    // ## just for 1.x compat:
	return fromLatin1( "" );
    } else {
	if ( len > slen-index )
	    len = slen - index;
	if ( index == 0 && len == slen )
	    return *this;
	const TQChar *p = unicode()+index;
	TQString s( len, true );
	memcpy( s.d->unicode, p, len * sizeof(TQChar) );
	s.d->len = len;
	return s;
    }
}

/*!
    Returns a string of length \a width that contains this string
    padded by the \a fill character.

    If \a truncate is false and the length of the string is more than
    \a width, then the returned string is a copy of the string.

    If \a truncate is true and the length of the string is more than
    \a width, then any characters in a copy of the string after length
    \a width are removed, and the copy is returned.

    \code
	TQString s( "apple" );
	TQString t = s.leftJustify( 8, '.' );        // t == "apple..."
    \endcode

    \sa rightJustify()
*/

TQString TQString::leftJustify( uint width, TQChar fill, bool truncate ) const
{
    TQString result;
    int len = length();
    int padlen = width - len;
    if ( padlen > 0 ) {
	result.setLength(len+padlen);
	if ( len )
	    memcpy( result.d->unicode, unicode(), sizeof(TQChar)*len );
	TQChar* uc = result.d->unicode + len;
	while (padlen--)
	    *uc++ = fill;
    } else {
	if ( truncate )
	    result = left( width );
	else
	    result = *this;
    }
    return result;
}

/*!
    Returns a string of length \a width that contains the \a fill
    character followed by the string.

    If \a truncate is false and the length of the string is more than
    \a width, then the returned string is a copy of the string.

    If \a truncate is true and the length of the string is more than
    \a width, then the resulting string is truncated at position \a
    width.

    \code
	TQString string( "apple" );
	TQString t = string.rightJustify( 8, '.' );  // t == "...apple"
    \endcode

    \sa leftJustify()
*/

TQString TQString::rightJustify( uint width, TQChar fill, bool truncate ) const
{
    TQString result;
    int len = length();
    int padlen = width - len;
    if ( padlen > 0 ) {
	result.setLength( len+padlen );
	TQChar* uc = result.d->unicode;
	while (padlen--)
	    *uc++ = fill;
	if ( len )
	    memcpy( uc, unicode(), sizeof(TQChar)*len );
    } else {
	if ( truncate )
	    result = left( width );
	else
	    result = *this;
    }
    return result;
}

/*!
    Returns a lowercase copy of the string.

    \code
	TQString string( "TROlltECH" );
	str = string.lower();   // str == "trolltech"
    \endcode

    \sa upper()
*/

TQString TQString::lower() const
{
    int l = length();
    TQChar *p = d->unicode;
    while ( l ) {
	if ( *p != ::lower(*p) ) {
	    TQString s( *this );
	    s.real_detach();
	    p = s.d->unicode + ( p - d->unicode );
	    while ( l ) {
		*p = ::lower( *p );
		l--;
		p++;
	    }
	    return s;
	}
	l--;
	p++;
    }
    return *this;
}

/*!
    Returns an uppercase copy of the string.

    \code
	TQString string( "TeXt" );
	str = string.upper();     // t == "TEXT"
    \endcode

    \sa lower()
*/

TQString TQString::upper() const
{
    int l = length();
    TQChar *p = d->unicode;
    while ( l ) {
	if ( *p != ::upper(*p) ) {
	    TQString s( *this );
	    s.real_detach();
	    p = s.d->unicode + ( p - d->unicode );
	    while ( l ) {
		*p = ::upper( *p );
		l--;
		p++;
	    }
	    return s;
	}
	l--;
	p++;
    }
    return *this;
}


/*!
    Returns a string that has whitespace removed from the start and
    the end.

    Whitespace means any character for which TQChar::isSpace() returns
    true. This includes Unicode characters with decimal values 9
    (TAB), 10 (LF), 11 (VT), 12 (FF), 13 (CR) and 32 (Space), and may
    also include other Unicode characters.

    \code
	TQString string = "   white space   ";
	TQString s = string.stripWhiteSpace();       // s == "white space"
    \endcode

    \sa simplifyWhiteSpace()
*/

TQString TQString::stripWhiteSpace() const
{
    if ( isEmpty() )                            // nothing to do
	return *this;
    const TQChar *s = unicode();
    if ( !s->isSpace() && !s[length()-1].isSpace() )
	return *this;

    int start = 0;
    int end = length() - 1;
    while ( start<=end && s[start].isSpace() )  // skip white space from start
	start++;
    if ( start <= end ) {                          // only white space
	while ( end && s[end].isSpace() )           // skip white space from end
	    end--;
    }
    int l = end - start + 1;
    if ( l <= 0 )
    	return TQString::fromLatin1("");

    TQString result( l, true );
    memcpy( result.d->unicode, &s[start], sizeof(TQChar)*l );
    result.d->len = l;
    return result;
}


/*!
    Returns a string that has whitespace removed from the start and
    the end, and which has each sequence of internal whitespace
    replaced with a single space.

    Whitespace means any character for which TQChar::isSpace() returns
    true. This includes Unicode characters with decimal values 9
    (TAB), 10 (LF), 11 (VT), 12 (FF), 13 (CR), and 32 (Space).

    \code
	TQString string = "  lots\t of\nwhite    space ";
	TQString t = string.simplifyWhiteSpace();
	// t == "lots of white space"
    \endcode

    \sa stripWhiteSpace()
*/

TQString TQString::simplifyWhiteSpace() const
{
    if ( isEmpty() )
	return *this;
    TQString result;
    result.setLength( length() );
    const TQChar *from = unicode();
    const TQChar *fromend = from+length();
    int outc=0;
    TQChar *to   = result.d->unicode;
    for (;;) {
	while ( from!=fromend && from->isSpace() )
	    from++;
	while ( from!=fromend && !from->isSpace() )
	    to[outc++] = *from++;
	if ( from!=fromend )
	    to[outc++] = ' ';
	else
	    break;
    }
    if ( outc > 0 && to[outc-1] == ' ' )
	outc--;
    result.truncate( outc );
    return result;
}


/*!
    Inserts \a s into the string at position \a index.

    If \a index is beyond the end of the string, the string is
    extended with spaces to length \a index and \a s is then appended
    and returns a reference to the string.

    \code
	TQString string( "I like fish" );
	str = string.insert( 2, "don't " );
	// str == "I don't like fish"
    \endcode

    \sa remove(), replace()
*/

TQString &TQString::insert( uint index, const TQString &s )
{
    // the sub function takes care of &s == this case.
    return insert( index, s.unicode(), s.length() );
}

/*! \fn TQString &TQString::insert( uint index, const TQByteArray &s )
    \overload

    Inserts \a s into the string at position \a index and returns
    a reference to the string.
*/

/*! \fn TQString &TQString::insert( uint index, const char *s )
    \overload

    Inserts \a s into the string at position \a index and returns
    a reference to the string.
*/

#ifndef TQT_NO_CAST_ASCII
TQString &TQString::insertHelper( uint index, const char *s, uint len )
{
    if ( s ) {
#ifndef TQT_NO_TEXTCODEC
	if ( TQTextCodec::codecForCStrings() )
	    return insert( index, fromAscii( s, len ) );
#endif
	if ( len == UINT_MAX )
	    len = int(strlen( s ));
	if ( len == 0 )
	    return *this;

	uint olen = length();
	int nlen = olen + len;

	if ( index >= olen ) {                      // insert after end of string
	    grow( len + index );
	    int n = index - olen;
	    TQChar* uc = d->unicode + olen;
	    while ( n-- )
		*uc++ = ' ';

	    uc = d->unicode + index;
	    while ( len-- )
		*uc++ = *s++;
	} else {                                    // normal insert
	    grow( nlen );
	    memmove( d->unicode + index + len, unicode() + index,
		    sizeof(TQChar) * (olen - index) );

	    TQChar* uc = d->unicode + index;
	    while ( len-- )
		*uc++ = *s++;
	}
    }
    return *this;
}
#endif

/*!
    \overload

    Inserts the first \a len characters in \a s into the string at
    position \a index and returns a reference to the string.
*/

TQString &TQString::insert( uint index, const TQChar* s, uint len )
{
    if ( len == 0 )
	return *this;
    uint olen = length();
    int nlen = olen + len;

    if ( s >= d->unicode && (uint)(s - d->unicode) < d->maxl ) {
	// Part of me - take a copy.
	TQChar *tmp = QT_ALLOC_QCHAR_VEC( len );
	memcpy(tmp,s,len*sizeof(TQChar));
	insert(index,tmp,len);
	QT_DELETE_QCHAR_VEC( tmp );
	return *this;
    }

    if ( index >= olen ) {                      // insert after end of string
	grow( len + index );
	int n = index - olen;
	TQChar* uc = d->unicode+olen;
	while (n--)
	    *uc++ = ' ';
	memcpy( d->unicode+index, s, sizeof(TQChar)*len );
    } else {                                    // normal insert
	grow( nlen );
	memmove( d->unicode + index + len, unicode() + index,
		 sizeof(TQChar) * (olen - index) );
	memcpy( d->unicode + index, s, sizeof(TQChar) * len );
    }
    return *this;
}

/*!
    \overload

    Insert \a c into the string at position \a index and returns a
    reference to the string.

    If \a index is beyond the end of the string, the string is
    extended with spaces (ASCII 32) to length \a index and \a c is
    then appended.
*/

TQString &TQString::insert( uint index, TQChar c ) // insert char
{
    TQString s( c );
    return insert( index, s );
}

/*!
    \fn TQString& TQString::insert( uint index, char c )

    \overload

    Insert character \a c at position \a index.
*/

/*!
    \fn TQString &TQString::prepend( const TQString &s )

    Inserts \a s at the beginning of the string and returns a
    reference to the string.

    Equivalent to insert(0, \a s).

    \code
	TQString string = "42";
	string.prepend( "The answer is " );
	// string == "The answer is 42"
    \endcode

    \sa insert()
*/

/*!
    \fn TQString& TQString::prepend( char ch )

    \overload

    Inserts \a ch at the beginning of the string and returns a
    reference to the string.

    Equivalent to insert(0, \a ch).

    \sa insert()
*/

/*!
    \fn TQString& TQString::prepend( TQChar ch )

    \overload

    Inserts \a ch at the beginning of the string and returns a
    reference to the string.

    Equivalent to insert(0, \a ch).

    \sa insert()
*/

/*! \fn TQString& TQString::prepend( const TQByteArray &s )
  \overload

  Inserts \a s at the beginning of the string and returns a reference to the string.

  Equivalent to insert(0, \a s).

  \sa insert()
 */

/*! \fn TQString& TQString::prepend( const std::string &s )
  \overload

  Inserts \a s at the beginning of the string and returns a reference to the string.

  Equivalent to insert(0, \a s).

  \sa insert()
*/

/*!
  \overload

  Inserts \a s at the beginning of the string and returns a reference to the string.

  Equivalent to insert(0, \a s).

  \sa insert()
 */
TQString &TQString::prepend( const char *s )
{
    return insert( 0, TQString(s) );
}

/*!
    Removes \a len characters from the string starting at position \a
    index, and returns a reference to the string.

    If \a index is beyond the length of the string, nothing happens.
    If \a index is within the string, but \a index + \a len is beyond
    the end of the string, the string is truncated at position \a
    index.

    \code
	TQString string( "Montreal" );
	string.remove( 1, 4 );      // string == "Meal"
    \endcode

    \sa insert(), replace()
*/

TQString &TQString::remove( uint index, uint len )
{
    uint olen = length();
    if ( index >= olen  ) {
	// range problems
    } else if ( index + len >= olen ) {  // index ok
	setLength( index );
    } else if ( len != 0 ) {
	real_detach();
	memmove( d->unicode+index, d->unicode+index+len,
		 sizeof(TQChar)*(olen-index-len) );
	setLength( olen-len );
    }
    return *this;
}

/*! \overload

    Removes every occurrence of the character \a c in the string.
    Returns a reference to the string.

    This is the same as replace(\a c, "").
*/
TQString &TQString::remove( TQChar c )
{
    int i = 0;
    while ( i < (int) length() ) {
	if ( constref(i) == c ) {
	    remove( i, 1 );
	} else {
	    i++;
	}
    }
    return *this;
}

/*! \overload

    \fn TQString &TQString::remove( char c )

    Removes every occurrence of the character \a c in the string.
    Returns a reference to the string.

    This is the same as replace(\a c, "").
*/

/*! \overload

    Removes every occurrence of \a str in the string. Returns a
    reference to the string.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.

    This is the same as replace(\a str, "", \a cs).
*/
TQString &TQString::remove( const TQString & str, bool cs )
{
    if ( str.isEmpty() ) {
	if ( isNull() )
	    real_detach();
    } else {
	int index = 0;
	while ( (index = find(str, index, cs)) != -1 )
	    remove( index, str.length() );
    }
    return *this;
}

TQString &TQString::remove( const TQString & str )
{
    return remove( str, true );
}

/*! \overload

    Replaces every occurrence of \a c1 with the char \a c2. Returns a
    reference to the string.
*/
TQString &TQString::replace( TQChar c1, TQChar c2 )
{
    if ( isEmpty() )
	return *this;

    real_detach();
    uint i = 0;
    while ( i < d->len ) {
	if ( d->unicode[i] == c1 )
	    d->unicode[i] = c2;
	i++;
    }
    return *this;
}

#ifndef TQT_NO_REGEXP_CAPTURE

/*! \overload

    Removes every occurrence of the regular expression \a rx in the
    string. Returns a reference to the string.

    This is the same as replace(\a rx, "").
*/

TQString &TQString::remove( const TQRegExp & rx )
{
    return replace( rx, TQString::null );
}

#endif

/*!
    \overload

    Removes every occurrence of \a str in the string. Returns a
    reference to the string.
*/
TQString &TQString::remove( const char *str )
{
    return remove( TQString::fromAscii(str), true );
}

/*!
    Replaces \a len characters from the string with \a s, starting at
    position \a index, and returns a reference to the string.

    If \a index is beyond the length of the string, nothing is deleted
    and \a s is appended at the end of the string. If \a index is
    valid, but \a index + \a len is beyond the end of the string,
    the string is truncated at position \a index, then \a s is
    appended at the end.

    \code
	TQString string( "Say yes!" );
	string = string.replace( 4, 3, "NO" );
	// string == "Say NO!"
    \endcode

    \warning TQt 3.3.3 and earlier had different semantics for the
    case \a index >= length(), which contradicted the documentation.
    To avoid portability problems between TQt 3 versions and with TQt
    4, we recommend that you never call the function with \a index >=
    length().

    \sa insert(), remove()
*/

TQString &TQString::replace( uint index, uint len, const TQString &s )
{
    return replace( index, len, s.unicode(), s.length() );
}

/*! \overload

    This is the same as replace(\a index, \a len, TQString(\a c)).
*/
TQString &TQString::replace( uint index, uint len, TQChar c )
{
    return replace( index, len, &c, 1 );
}

/*! \overload
    \fn TQString &TQString::replace( uint index, uint len, char c )

    This is the same as replace(\a index, \a len, TQChar(\a c)).
*/

/*!
    \overload

    Replaces \a len characters with \a slen characters of TQChar data
    from \a s, starting at position \a index, and returns a reference
    to the string.

    \sa insert(), remove()
*/

TQString &TQString::replace( uint index, uint len, const TQChar* s, uint slen )
{
    if (index > length())
        index = length();
    real_detach();
    if ( len == slen && index + len <= length() ) {
	// Optimized common case: replace without size change
	memcpy( d->unicode+index, s, len * sizeof(TQChar) );
    } else if ( s >= d->unicode && (uint)(s - d->unicode) < d->maxl ) {
	// Part of me - take a copy.
	TQChar *tmp = QT_ALLOC_QCHAR_VEC( slen );
	memcpy( tmp, s, slen * sizeof(TQChar) );
	replace( index, len, tmp, slen );
	QT_DELETE_QCHAR_VEC( tmp );
    } else {
	remove( index, len );
	insert( index, s, slen );
    }
    return *this;
}

/*! \overload

    Replaces every occurrence of the character \a c in the string
    with \a after. Returns a reference to the string.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.

    Example:
    \code
    TQString s = "a,b,c";
    s.replace( TQChar(','), " or " );
    // s == "a or b or c"
    \endcode
*/
TQString &TQString::replace( TQChar c, const TQString & after, bool cs )
{
    return replace( TQString( c ), after, cs );
}

TQString &TQString::replace( TQChar c, const TQString & after )
{
    return replace( TQString( c ), after, true );
}

/*! \overload
    \fn TQString &TQString::replace( char c, const TQString & after, bool cs )

    Replaces every occurrence of the character \a c in the string
    with \a after. Returns a reference to the string.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.
*/

/*! \overload

    Replaces every occurrence of the string \a before in the string
    with the string \a after. Returns a reference to the string.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.

    Example:
    \code
    TQString s = "Greek is Greek";
    s.replace( "Greek", "English" );
    // s == "English is English"
    \endcode
*/
TQString &TQString::replace( const TQString & before, const TQString & after,
			   bool cs )
{
    if ( isEmpty() ) {
	if ( !before.isEmpty() )
	    return *this;
    } else {
	if ( cs && before == after )
	    return *this;
    }

    real_detach();

    int index = 0;
    uint skiptable[256];
    bm_init_skiptable( before, skiptable, cs );
    const int bl = before.length();
    const int al = after.length();

    if ( bl == al ) {
	if ( bl ) {
	    const TQChar *auc = after.unicode();
	    while ( (index = bm_find(*this, index, before, skiptable, cs) ) != -1 ) {
		memcpy( d->unicode + index, auc, al * sizeof(TQChar) );
		index += bl;
	    }
	}
    } else if ( al < bl ) {
	const TQChar *auc = after.unicode();
	uint to = 0;
	uint movestart = 0;
	uint num = 0;
	while ( (index = bm_find(*this, index, before, skiptable, cs)) != -1 ) {
	    if ( num ) {
		int msize = index - movestart;
		if ( msize > 0 ) {
		    memmove( d->unicode + to, d->unicode + movestart, msize*sizeof(TQChar) );
		    to += msize;
		}
	    } else {
		to = index;
	    }
	    if ( al ) {
		memcpy( d->unicode+to, auc, al*sizeof(TQChar) );
		to += al;
	    }
	    index += bl;
	    movestart = index;
	    num++;
	}
	if ( num ) {
	    int msize = d->len - movestart;
	    if ( msize > 0 )
		memmove( d->unicode + to, d->unicode + movestart, msize*sizeof(TQChar) );
	    setLength( d->len - num*(bl-al) );
	}
    } else {
	// the most complex case. We don't want to loose performance by doing repeated
	// copies and reallocs of the string.
	while ( index != -1 ) {
	    uint indices[4096];
	    uint pos = 0;
	    while ( pos < 4095 ) {
		index = bm_find( *this, index, before, skiptable, cs );
		if ( index == -1 )
		    break;
		indices[pos++] = index;
		index += bl;
		// avoid infinite loop
		if ( !bl )
		    index++;
	    }
	    if ( !pos )
		break;

	    // we have a table of replacement positions, use them for fast replacing
	    int adjust = pos*(al-bl);
	    // index has to be adjusted in case we get back into the loop above.
	    if ( index != -1 )
		index += adjust;
	    uint newlen = d->len + adjust;
	    int moveend = d->len;
	    if ( newlen > d->len )
		setLength( newlen );

	    while ( pos ) {
		pos--;
		int movestart = indices[pos] + bl;
		int insertstart = indices[pos] + pos*(al-bl);
		int moveto = insertstart + al;
		memmove( d->unicode + moveto, d->unicode + movestart, (moveend - movestart)*sizeof(TQChar) );
		memcpy( d->unicode + insertstart, after.unicode(), al*sizeof(TQChar) );
		moveend = movestart-bl;
	    }
	}
    }
    return *this;
}

TQString &TQString::replace( const TQString & before, const TQString & after )
{
    return replace( before, after, true );
}

#ifndef TQT_NO_REGEXP_CAPTURE
/*! \overload

  Replaces every occurrence of the regexp \a rx in the string with
  \a after. Returns a reference to the string. For example:
  \code
    TQString s = "banana";
    s.replace( TQRegExp("an"), "" );
    // s == "ba"
  \endcode

  For regexps containing \link ntqregexp.html#capturing-text capturing
  parentheses \endlink, occurrences of <b>\\1</b>, <b>\\2</b>, ...,
  in \a after are replaced with \a{rx}.cap(1), cap(2), ...

  \code
    TQString t = "A <i>bon mot</i>.";
    t.replace( TQRegExp("<i>([^<]*)</i>"), "\\emph{\\1}" );
    // t == "A \\emph{bon mot}."
  \endcode

  \sa find(), findRev(), TQRegExp::cap()
*/

TQString &TQString::replace( const TQRegExp &rx, const TQString &after )
{
    TQRegExp rx2 = rx;

    if ( isEmpty() && rx2.search(*this) == -1 )
	return *this;

    real_detach();

    int index = 0;
    int numCaptures = rx2.numCaptures();
    int al = after.length();
    TQRegExp::CaretMode caretMode = TQRegExp::CaretAtZero;

    if ( numCaptures > 0 ) {
	if ( numCaptures > 9 )
	    numCaptures = 9;

	const TQChar *uc = after.unicode();
	int numBackRefs = 0;

	for ( int i = 0; i < al - 1; i++ ) {
	    if ( uc[i] == '\\' ) {
		int no = uc[i + 1].digitValue();
		if ( no > 0 && no <= numCaptures )
		    numBackRefs++;
	    }
	}

	/*
	  This is the harder case where we have back-references.
	  We don't try to optimize it.
	*/
	if ( numBackRefs > 0 ) {
	    int *capturePositions = new int[numBackRefs];
	    int *captureNumbers = new int[numBackRefs];
	    int j = 0;

	    for ( int i = 0; i < al - 1; i++ ) {
		if ( uc[i] == '\\' ) {
		    int no = uc[i + 1].digitValue();
		    if ( no > 0 && no <= numCaptures ) {
			capturePositions[j] = i;
			captureNumbers[j] = no;
			j++;
		    }
		}
	    }

	    while ( index <= (int)length() ) {
		index = rx2.search( *this, index, caretMode );
		if ( index == -1 )
		    break;

		TQString after2 = after;
		for ( j = numBackRefs - 1; j >= 0; j-- )
		    after2.replace( capturePositions[j], 2,
				    rx2.cap(captureNumbers[j]) );

		replace( index, rx2.matchedLength(), after2 );
		index += after2.length();

		if ( rx2.matchedLength() == 0 ) {
		    // avoid infinite loop on 0-length matches (e.g., [a-z]*)
		    index++;
		}
		caretMode = TQRegExp::CaretWontMatch;
	    }
	    delete[] capturePositions;
	    delete[] captureNumbers;
	    return *this;
	}
    }

    /*
      This is the simple and optimized case where we don't have
      back-references.
    */
    while ( index != -1 ) {
	struct {
	    int pos;
	    int length;
	} replacements[2048];

	uint pos = 0;
	int adjust = 0;
	while ( pos < 2047 ) {
	    index = rx2.search( *this, index, caretMode );
	    if ( index == -1 )
		break;
	    int ml = rx2.matchedLength();
	    replacements[pos].pos = index;
	    replacements[pos++].length = ml;
	    index += ml;
	    adjust += al - ml;
	    // avoid infinite loop
	    if ( !ml )
		index++;
	}
	if ( !pos )
	    break;
	replacements[pos].pos = d->len;
	uint newlen = d->len + adjust;

	// to continue searching at the right position after we did
	// the first round of replacements
	if ( index != -1 )
	    index += adjust;
	TQChar *newuc = QT_ALLOC_QCHAR_VEC( newlen + 1 );
	TQChar *uc = newuc;
	int copystart = 0;
	uint i = 0;
	while ( i < pos ) {
	    int copyend = replacements[i].pos;
	    int size = copyend - copystart;
	    memcpy( uc, d->unicode + copystart, size * sizeof(TQChar) );
	    uc += size;
	    memcpy( uc, after.unicode(), al * sizeof(TQChar) );
	    uc += al;
	    copystart = copyend + replacements[i].length;
	    i++;
	}
	memcpy( uc, d->unicode + copystart,
		(d->len - copystart) * sizeof(TQChar) );
	QT_DELETE_QCHAR_VEC( d->unicode );
	d->unicode = newuc;
	d->len = newlen;
	d->maxl = newlen + 1;
	d->setDirty();
	caretMode = TQRegExp::CaretWontMatch;
    }
    return *this;
}
#endif

#ifndef TQT_NO_REGEXP
/*!
    Finds the first match of the regular expression \a rx, starting
    from position \a index. If \a index is -1, the search starts at
    the last character; if -2, at the next to last character and so
    on. (See findRev() for searching backwards.)

    Returns the position of the first match of \a rx or -1 if no match
    was found.

    \code
	TQString string( "bananas" );
	int i = string.find( TQRegExp("an"), 0 );    // i == 1
    \endcode

    \sa findRev() replace() contains()
*/

int TQString::find( const TQRegExp &rx, int index ) const
{
    return rx.search( *this, index );
}

/*!
    \overload

    Finds the first match of the regexp \a rx, starting at position \a
    index and searching backwards. If the index is -1, the search
    starts at the last character, if it is -2, at the next to last
    character and so on. (See findRev() for searching backwards.)

    Returns the position of the match or -1 if no match was found.

    \code
	TQString string( "bananas" );
	int i = string.findRev( TQRegExp("an") );      // i == 3
    \endcode

    \sa find()
*/

int TQString::findRev( const TQRegExp &rx, int index ) const
{
    return rx.searchRev( *this, index );
}

/*!
    \overload

    Returns the number of times the regexp, \a rx, matches in the
    string.

    This function counts overlapping matches, so in the example below,
    there are four instances of "ana" or "ama".

    \code
	TQString str = "banana and panama";
	TQRegExp rxp = TQRegExp( "a[nm]a", true, false );
	int i = str.contains( rxp );    // i == 4
    \endcode

    \sa find() findRev()
*/

int TQString::contains( const TQRegExp &rx ) const
{
    int count = 0;
    int index = -1;
    int len = length();
    while ( index < len - 1 ) {                 // count overlapping matches
	index = rx.search( *this, index + 1 );
	if ( index == -1 )
	    break;
	count++;
    }
    return count;
}

#endif //TQT_NO_REGEXP

/*!
    Returns the string converted to a \c long using base \a
    base, which is 10 by default and must be between 2 and 36 or 0. If
    \a base is 0, the base is determined automatically using the
    following rules:
    <ul>
    <li>If the string begins with "0x", it is assumed to
    be hexadecimal;
    <li>If it begins with "0", it is assumed to be octal;
    <li>Otherwise it is assumed to be decimal.
    </ul>

    Returns 0 if the conversion fails.

    If \a ok is not 0: if a conversion error occurs, \a *ok is set to
    false; otherwise \a *ok is set to true.

    Leading and trailing whitespace is ignored by this function.

    For information on how string-to-number functions in TQString handle
    localized input, see toDouble().

    \sa number()
*/

long TQString::toLong( bool *ok, int base ) const
{
    TQ_LLONG v = toLongLong( ok, base );
    if ( v < LONG_MIN || v > LONG_MAX ) {
	if ( ok )
	    *ok = false;
	v = 0;
    }
    return long(v);
}

/*!
    Returns the string converted to a \c {long long} using base \a
    base, which is 10 by default and must be between 2 and 36 or 0. If
    \a base is 0, the base is determined automatically using the
    following rules:
    <ul>
    <li>If the string begins with "0x", it is assumed to
    be hexadecimal;
    <li>If it begins with "0", it is assumed to be octal;
    <li>Otherwise it is assumed to be decimal.
    </ul>

    Returns 0 if the conversion fails.

    If \a ok is not 0: if a conversion error occurs, \a *ok is set to
    false; otherwise \a *ok is set to true.

    Leading and trailing whitespace is ignored by this function.

    For information on how string-to-number functions in TQString handle
    localized input, see toDouble().

    \sa number()
*/

TQ_LLONG TQString::toLongLong( bool *ok, int base ) const
{
#if defined(QT_CHECK_RANGE)
    if ( base != 0 && (base < 2 || base > 36) ) {
	tqWarning( "TQString::toLongLong: Invalid base (%d)", base );
	base = 10;
    }
#endif

    bool my_ok;
    TQLocale def_locale;
    TQ_LLONG result = def_locale.d->stringToLongLong(*this, base, &my_ok, TQLocalePrivate::FailOnGroupSeparators);
    if (my_ok) {
    	if (ok != 0)
	    *ok = true;
	return result;
    }

    // If the default was not "C", try the "C" locale
    if (def_locale.language() == TQLocale::C) {
    	if (ok != 0)
	    *ok = false;
	return 0;
    }

    TQLocale c_locale(TQLocale::C);
    return c_locale.d->stringToLongLong(*this, base, ok, TQLocalePrivate::FailOnGroupSeparators);
}

/*!
    Returns the string converted to an \c {unsigned long} using base \a
    base, which is 10 by default and must be between 2 and 36 or 0. If
    \a base is 0, the base is determined automatically using the
    following rules:
    <ul>
    <li>If the string begins with "0x", it is assumed to
    be hexadecimal;
    <li>If it begins with "0", it is assumed to be octal;
    <li>Otherwise it is assumed to be decimal.
    </ul>

    Returns 0 if the conversion fails.

    If \a ok is not 0: if a conversion error occurs, \a *ok is set to
    false; otherwise \a *ok is set to true.

    Leading and trailing whitespace is ignored by this function.

    For information on how string-to-number functions in TQString handle
    localized input, see toDouble().

    \sa number()
*/

ulong TQString::toULong( bool *ok, int base ) const
{
    TQ_ULLONG v = toULongLong( ok, base );
    if ( v > ULONG_MAX ) {
	if ( ok )
	    *ok = false;
	v = 0;
    }
    return ulong(v);
}

/*!
    Returns the string converted to an \c {unsigned long long} using base \a
    base, which is 10 by default and must be between 2 and 36 or 0. If
    \a base is 0, the base is determined automatically using the
    following rules:
    <ul>
    <li>If the string begins with "0x", it is assumed to
    be hexadecimal;
    <li>If it begins with "0", it is assumed to be octal;
    <li>Otherwise it is assumed to be decimal.
    </ul>

    Returns 0 if the conversion fails.

    If \a ok is not 0: if a conversion error occurs, \a *ok is set to
    false; otherwise \a *ok is set to true.

    Leading and trailing whitespace is ignored by this function.

    For information on how string-to-number functions in TQString handle
    localized input, see toDouble().

    \sa number()
*/

TQ_ULLONG TQString::toULongLong( bool *ok, int base ) const
{
#if defined(QT_CHECK_RANGE)
    if ( base != 0 && (base < 2 || base > 36) ) {
	tqWarning( "TQString::toULongLong: Invalid base %d", base );
	base = 10;
    }
#endif

    bool my_ok;
    TQLocale def_locale;
    TQ_ULLONG result = def_locale.d->stringToUnsLongLong(*this, base, &my_ok, TQLocalePrivate::FailOnGroupSeparators);
    if (my_ok) {
    	if (ok != 0)
	    *ok = true;
	return result;
    }

    // If the default was not "C", try the "C" locale
    if (def_locale.language() == TQLocale::C) {
    	if (ok != 0)
	    *ok = false;
	return 0;
    }

    TQLocale c_locale(TQLocale::C);
    return c_locale.d->stringToUnsLongLong(*this, base, ok, TQLocalePrivate::FailOnGroupSeparators);
}

/*!
    Returns the string converted to a \c short using base \a
    base, which is 10 by default and must be between 2 and 36 or 0. If
    \a base is 0, the base is determined automatically using the
    following rules:
    <ul>
    <li>If the string begins with "0x", it is assumed to
    be hexadecimal;
    <li>If it begins with "0", it is assumed to be octal;
    <li>Otherwise it is assumed to be decimal.
    </ul>


    Returns 0 if the conversion fails.

    If \a ok is not 0: if a conversion error occurs, \a *ok is set to
    false; otherwise \a *ok is set to true.

    Leading and trailing whitespace is ignored by this function.

    For information on how string-to-number functions in TQString handle
    localized input, see toDouble().

    \sa number()
*/


short TQString::toShort( bool *ok, int base ) const
{
    TQ_LLONG v = toLongLong( ok, base );
    if ( v < SHRT_MIN || v > SHRT_MAX ) {
	if ( ok )
	    *ok = false;
	v = 0;
    }
    return (short)v;
}

/*!
    Returns the string converted to an \c {unsigned short} using base \a
    base, which is 10 by default and must be between 2 and 36 or 0. If
    \a base is 0, the base is determined automatically using the
    following rules:
    <ul>
    <li>If the string begins with "0x", it is assumed to
    be hexadecimal;
    <li>If it begins with "0", it is assumed to be octal;
    <li>Otherwise it is assumed to be decimal.
    </ul>


    Returns 0 if the conversion fails.

    If \a ok is not 0: if a conversion error occurs, \a *ok is set to
    false; otherwise \a *ok is set to true.

    Leading and trailing whitespace is ignored by this function.

    For information on how string-to-number functions in TQString handle
    localized input, see toDouble().

    \sa number()
*/

ushort TQString::toUShort( bool *ok, int base ) const
{
    TQ_ULLONG v = toULongLong( ok, base );
    if ( v > USHRT_MAX ) {
	if ( ok )
	    *ok = false;
	v = 0;
    }
    return (ushort)v;
}


/*!
    Returns the string converted to an \c int using base \a
    base, which is 10 by default and must be between 2 and 36 or 0. If
    \a base is 0, the base is determined automatically using the
    following rules:
    <ul>
    <li>If the string begins with "0x", it is assumed to
    be hexadecimal;
    <li>If it begins with "0", it is assumed to be octal;
    <li>Otherwise it is assumed to be decimal.
    </ul>


    Returns 0 if the conversion fails.

    If \a ok is not 0: if a conversion error occurs, \a *ok is set to
    false; otherwise \a *ok is set to true.

    \code
	TQString str( "FF" );
	bool ok;
	int hex = str.toInt( &ok, 16 );     // hex == 255, ok == true
	int dec = str.toInt( &ok, 10 );     // dec == 0, ok == false
    \endcode

    Leading and trailing whitespace is ignored by this function.

    For information on how string-to-number functions in TQString handle
    localized input, see toDouble().

    \sa number()
*/

int TQString::toInt( bool *ok, int base ) const
{
    TQ_LLONG v = toLongLong( ok, base );
    if ( v < INT_MIN || v > INT_MAX ) {
	if ( ok )
	    *ok = false;
	v = 0;
    }
    return (int)v;
}

/*!
    Returns the string converted to an \c {unsigned int} using base \a
    base, which is 10 by default and must be between 2 and 36 or 0. If
    \a base is 0, the base is determined automatically using the
    following rules:
    <ul>
    <li>If the string begins with "0x", it is assumed to
    be hexadecimal;
    <li>If it begins with "0", it is assumed to be octal;
    <li>Otherwise it is assumed to be decimal.
    </ul>

    Returns 0 if the conversion fails.

    If \a ok is not 0: if a conversion error occurs, \a *ok is set to
    false; otherwise \a *ok is set to true.

    Leading and trailing whitespace is ignored by this function.

    For information on how string-to-number functions in TQString handle
    localized input, see toDouble().

    \sa number()
*/

uint TQString::toUInt( bool *ok, int base ) const
{
    TQ_ULLONG v = toULongLong( ok, base );
    if ( v > UINT_MAX ) {
	if ( ok )
	    *ok = false;
	v = 0;
    }
    return (uint)v;
}

/*!
    Returns the string converted to a \c double value.

    If \a ok is not 0: if a conversion error occurs, \a *ok is set to
    false; otherwise \a *ok is set to true.

    \code
	TQString string( "1234.56" );
	double a = string.toDouble();   // a == 1234.56
    \endcode

    The string-to-number functions:
    \list
    \i toShort()
    \i toUShort()
    \i toInt()
    \i toUInt()
    \i toLong()
    \i toULong()
    \i toLongLong()
    \i toULongLong()
    \i toFloat()
    \i toDouble()
    \endlist
    can handle numbers
    represented in various locales. These representations may use different
    characters for the decimal point, thousands group sepearator
    and even individual digits. TQString's functions try to interpret
    the string according to the current locale. The current locale is
    determined from the system at application startup and can be changed
    by calling TQLocale::setDefault(). If the string cannot be interpreted
    according to the current locale, this function falls back
    on the "C" locale.

    \code
	bool ok;
	double d;

        TQLocale::setDefault(TQLocale::C);
	d = TQString( "1234,56" ).toDouble(&ok); // ok == false
	d = TQString( "1234.56" ).toDouble(&ok); // ok == true, d == 1234.56

	TQLocale::setDefault(TQLocale::German);
	d = TQString( "1234,56" ).toDouble(&ok); // ok == true, d == 1234.56
	d = TQString( "1234.56" ).toDouble(&ok); // ok == true, d == 1234.56
    \endcode

    Due to the ambiguity between the decimal point and thousands group
    separator in various locales, these functions do not handle
    thousands group separators. If you need to convert such numbers,
    use the corresponding function in TQLocale.

    \code
	bool ok;
        TQLocale::setDefault(TQLocale::C);
	double d = TQString( "1,234,567.89" ).toDouble(&ok); // ok == false
    \endcode

    \warning If the string contains trailing whitespace this function
    will fail, and set \a *ok to false if \a ok is not 0. Leading
    whitespace is ignored.

    \sa number() TQLocale::setDefault() TQLocale::toDouble() stripWhiteSpace()
*/

double TQString::toDouble( bool *ok ) const
{
    // If there is trailing whitespace, set ok to false but return the correct
    // result anyway to preserve behavour of pervious versions of TQt
    if (length() > 0 && unicode()[length() - 1].isSpace()) {
        TQString tmp = stripWhiteSpace();
    	if (ok != 0)
	    *ok = false;
    	return tmp.toDouble();
    }

    // Try the default locale
    bool my_ok;
    TQLocale def_locale;
    double result = def_locale.d->stringToDouble(*this, &my_ok, TQLocalePrivate::FailOnGroupSeparators);
    if (my_ok) {
    	if (ok != 0)
	    *ok = true;
	return result;
    }

    // If the default was not "C", try the "C" locale
    if (def_locale.language() == TQLocale::C) {
    	if (ok != 0)
	    *ok = false;
	return 0.0;
    }

    TQLocale c_locale(TQLocale::C);
    return c_locale.d->stringToDouble(*this, ok, TQLocalePrivate::FailOnGroupSeparators);
}

/*!
    Returns the string converted to a \c float value.

    Returns 0.0 if the conversion fails.

    If \a ok is not 0: if a conversion error occurs, \a *ok is set to
    false; otherwise \a *ok is set to true.

    For information on how string-to-number functions in TQString handle
    localized input, see toDouble().

    \warning If the string contains trailing whitespace this function
    will fail, settings \a *ok to false if \a ok is not 0.
    Leading whitespace is ignored.

    \sa number()
*/

#define QT_MAX_FLOAT 3.4028234663852886e+38

float TQString::toFloat( bool *ok ) const
{
    bool myOk;
    double d = toDouble(&myOk);
    if (!myOk || d > QT_MAX_FLOAT || d < -QT_MAX_FLOAT) {
        if (ok != 0)
            *ok = false;
        return 0.0;
    }
    if (ok != 0)
        *ok = true;
    return (float) d;
}

/*!
    Sets the string to the printed value of \a n in base \a base and
    returns a reference to the string. The returned string is in "C" locale.

    The base is 10 by default and must be between 2 and 36.

    \code
	TQString string;
	string = string.setNum( 1234 );     // string == "1234"
    \endcode
*/

TQString &TQString::setNum( TQ_LLONG n, int base )
{
#if defined(QT_CHECK_RANGE)
    if ( base < 2 || base > 36 ) {
	tqWarning( "TQString::setNum: Invalid base %d", base );
	base = 10;
    }
#endif
    TQLocale locale(TQLocale::C);
    *this = locale.d->longLongToString(n, -1, base);
    return *this;
}

/*!
    \overload

    Sets the string to the printed value of \a n in base \a base and
    returns a reference to the string.

    The base is 10 by default and must be between 2 and 36.
*/

TQString &TQString::setNum( TQ_ULLONG n, int base )
{
#if defined(QT_CHECK_RANGE)
    if ( base < 2 || base > 36 ) {
	tqWarning( "TQString::setNum: Invalid base %d", base );
	base = 10;
    }
#endif
    TQLocale locale(TQLocale::C);
    *this = locale.d->unsLongLongToString(n, -1, base);
    return *this;
}

/*!
    \fn TQString &TQString::setNum( long n, int base )

    \overload
*/
// ### 4.0: inline
TQString &TQString::setNum( long n, int base )
{
    return setNum( (TQ_LLONG)n, base );
}

/*!
    \fn TQString &TQString::setNum( ulong n, int base )

    \overload
*/
// ### 4.0: inline
TQString &TQString::setNum( ulong n, int base )
{
    return setNum( (TQ_ULLONG)n, base );
}

/*!
    \fn TQString &TQString::setNum( int n, int base )

    \overload

    Sets the string to the printed value of \a n in base \a base and
    returns a reference to the string.

    The base is 10 by default and must be between 2 and 36.
*/

/*!
    \fn TQString &TQString::setNum( uint n, int base )

    \overload

    Sets the string to the printed value of \a n in base \a base and
    returns a reference to the string.

    The base is 10 by default and must be between 2 and 36.
*/

/*!
    \fn TQString &TQString::setNum( short n, int base )

    \overload

    Sets the string to the printed value of \a n in base \a base and
    returns a reference to the string.

    The base is 10 by default and must be between 2 and 36.
*/

/*!
    \fn TQString &TQString::setNum( ushort n, int base )

    \overload

    Sets the string to the printed value of \a n in base \a base and
    returns a reference to the string.

    The base is 10 by default and must be between 2 and 36.
*/

/*!
    \overload

    Sets the string to the printed value of \a n, formatted in format
    \a f with precision \a prec, and returns a reference to the
    string.

    The format \a f can be 'f', 'F', 'e', 'E', 'g' or 'G'. See \link
    #arg-formats arg \endlink() for an explanation of the formats.
*/

TQString &TQString::setNum( double n, char f, int prec )
{
    TQLocalePrivate::DoubleForm form = TQLocalePrivate::DFDecimal;
    uint flags = 0;

    if (qIsUpper(f))
    	flags = TQLocalePrivate::CapitalEorX;
    f = qToLower(f);

    switch (f) {
	case 'f':
	    form = TQLocalePrivate::DFDecimal;
	    break;
	case 'e':
	    form = TQLocalePrivate::DFExponent;
	    break;
	case 'g':
	    form = TQLocalePrivate::DFSignificantDigits;
	    break;
	default:
#if defined(QT_CHECK_RANGE)
	    tqWarning( "TQString::setNum: Invalid format char '%c'", f );
#endif
	    break;
    }

    TQLocale locale(TQLocale::C);
    *this = locale.d->doubleToString(n, prec, form, -1, flags);
    return *this;
}

/*!
    \fn TQString &TQString::setNum( float n, char f, int prec )

    \overload

    Sets the string to the printed value of \a n, formatted in format
    \a f with precision \a prec, and returns a reference to the
    string.

    The format \a f can be 'f', 'F', 'e', 'E', 'g' or 'G'. See \link
    #arg-formats arg \endlink() for an explanation of the formats.
*/


/*!
    A convenience function that returns a string equivalent of the
    number \a n to base \a base, which is 10 by default and must be
    between 2 and 36. The returned string is in "C" locale.

    \code
	long a = 63;
	TQString str = TQString::number( a, 16 );             // str == "3f"
	TQString str = TQString::number( a, 16 ).upper();     // str == "3F"
    \endcode

    \sa setNum()
*/
TQString TQString::number( long n, int base )
{
    TQString s;
    s.setNum( n, base );
    return s;
}

/*!
    \overload

    \sa setNum()
*/
TQString TQString::number( ulong n, int base )
{
    TQString s;
    s.setNum( n, base );
    return s;
}

/*!
    \overload

    \sa setNum()
*/
TQString TQString::number( TQ_LLONG n, int base )
{
    TQString s;
    s.setNum( n, base );
    return s;
}

/*!
    \overload

    \sa setNum()
*/
TQString TQString::number( TQ_ULLONG n, int base )
{
    TQString s;
    s.setNum( n, base );
    return s;
}

/*!
    \overload

    \sa setNum()
*/
TQString TQString::number( int n, int base )
{
    TQString s;
    s.setNum( n, base );
    return s;
}

/*!
    \overload

    A convenience factory function that returns a string
    representation of the number \a n to the base \a base, which is 10
    by default and must be between 2 and 36.

    \sa setNum()
*/
TQString TQString::number( uint n, int base )
{
    TQString s;
    s.setNum( n, base );
    return s;
}

/*!
    \overload

    Argument \a n is formatted according to the \a f format specified,
    which is \c g by default, and can be any of the following:

    \table
    \header \i Format \i Meaning
    \row \i \c e \i format as [-]9.9e[+|-]999
    \row \i \c E \i format as [-]9.9E[+|-]999
    \row \i \c f \i format as [-]9.9
    \row \i \c g \i use \c e or \c f format, whichever is the most concise
    \row \i \c G \i use \c E or \c f format, whichever is the most concise
    \endtable

    With 'e', 'E', and 'f', \a prec is the number of digits after the
    decimal point. With 'g' and 'G', \a prec is the maximum number of
    significant digits (trailing zeroes are omitted).

    \code
    double d = 12.34;
    TQString ds = TQString( "'E' format, precision 3, gives %1" )
		    .arg( d, 0, 'E', 3 );
    // ds == "1.234E+001"
    \endcode

    \sa setNum()
    */
TQString TQString::number( double n, char f, int prec )
{
    TQString s;
    s.setNum( n, f, prec );
    return s;
}


/*! \obsolete

  Sets the character at position \a index to \a c and expands the
  string if necessary, filling with spaces.

  This method is redundant in TQt 3.x, because operator[] will expand
  the string as necessary.
*/

void TQString::setExpand( uint index, TQChar c )
{
    int spaces = index - d->len;
    at(index) = c;
    while (spaces-->0)
	d->unicode[--index]=' ';
}


/*!
  \fn const char* TQString::data() const

  \obsolete

  Returns a pointer to a '\0'-terminated classic C string.

  In TQt 1.x, this returned a char* allowing direct manipulation of the
  string as a sequence of bytes. In TQt 2.x where TQString is a Unicode
  string, char* conversion constructs a temporary string, and hence
  direct character operations are meaningless.
*/

/*!
    \fn bool TQString::operator!() const

    Returns true if this is a null string; otherwise returns false.

    \code
	TQString name = getName();
	if ( !name )
	    name = "Rodney";
    \endcode

    Note that if you say

    \code
	TQString name = getName();
	if ( name )
	    doSomethingWith(name);
    \endcode

    It will call "operator const char*()", which is inefficent; you
    may wish to define the macro \c TQT_NO_ASCII_CAST when writing code
    which you wish to remain Unicode-clean.

    When you want the above semantics, use:

    \code
	TQString name = getName();
	if ( !name.isNull() )
	    doSomethingWith(name);
    \endcode

    \sa isEmpty()
*/


/*!
    \fn TQString& TQString::append( const TQString& str )

    Appends \a str to the string and returns a reference to the
    result.

    \code
	string = "Test";
	string.append( "ing" );        // string == "Testing"
    \endcode

    Equivalent to operator+=().
*/

/*!
    \fn TQString& TQString::append( char ch )

    \overload

    Appends character \a ch to the string and returns a reference to
    the result.

    Equivalent to operator+=().
*/

/*!
    \fn TQString& TQString::append( TQChar ch )

    \overload

    Appends character \a ch to the string and returns a reference to
    the result.

    Equivalent to operator+=().
*/

/*! \fn TQString& TQString::append( const TQByteArray &str )
  \overload

  Appends \a str to the string and returns a reference to the result.

  Equivalent to operator+=().
*/

/*! \fn TQString& TQString::append( const std::string &str )
  \overload

  Appends \a str to the string and returns a reference to the result.

  Equivalent to operator+=().
*/

/*! \fn TQString& TQString::append( const char *str )
  \overload

  Appends \a str to the string and returns a reference to the result.

  Equivalent to operator+=().
*/

/*!
    Appends \a str to the string and returns a reference to the string.
*/
TQString& TQString::operator+=( const TQString &str )
{
    uint len1 = length();
    uint len2 = str.length();
    if ( len2 ) {
	if ( isEmpty() ) {
	    operator=( str );
	} else {
	    grow( len1+len2 );
	    memcpy( d->unicode+len1, str.unicode(), sizeof(TQChar)*len2 );
	}
    } else if ( isNull() && !str.isNull() ) {   // ## just for 1.x compat:
	*this = fromLatin1( "" );
    }
    return *this;
}

#ifndef TQT_NO_CAST_ASCII
TQString &TQString::operatorPlusEqHelper( const char *s, uint len2 )
{
    if ( s ) {
#ifndef TQT_NO_TEXTCODEC
	if ( TQTextCodec::codecForCStrings() )
	    return operator+=( fromAscii( s, len2 ) );
#endif

	uint len1 = length();
	if ( len2 == UINT_MAX )
	    len2 = int(strlen( s ));
	if ( len2 ) {
	    grow( len1 + len2 );
	    TQChar* uc = d->unicode + len1;
	    while ( len2-- )
		*uc++ = *s++;
	} else if ( isNull() ) {   // ## just for 1.x compat:
	    *this = fromLatin1( "" );
	}
    }
    return *this;
}
#endif

/*!
    \overload

  Appends \a str to the string and returns a reference to the string.
*/
#ifndef TQT_NO_CAST_ASCII
TQString& TQString::operator+=( const char *str )
{
    // ### TQt 4: make this function inline
    return operatorPlusEqHelper( str );
}
#endif

/*! \overload

  Appends \a c to the string and returns a reference to the string.
*/

TQString &TQString::operator+=( TQChar c )
{
    grow( length()+1 );
    d->unicode[length()-1] = c;
    return *this;
}

/*!
    \overload

    Appends \a c to the string and returns a reference to the string.
*/

TQString &TQString::operator+=( char c )
{
#ifndef TQT_NO_TEXTCODEC
    if ( TQTextCodec::codecForCStrings() )
	return operator+=( fromAscii( &c, 1 ) );
#endif
    grow( length()+1 );
    d->unicode[length()-1] = c;
    return *this;
}

/*!
  \fn TQString &TQString::operator+=( const TQByteArray &str )
  \overload

  Appends \a str to the string and returns a reference to the string.
*/

/*!
  \fn TQString &TQString::operator+=( const std::string &str )
  \overload

  Appends \a str to the string and returns a reference to the string.
*/

/*!
    \fn char TQChar::latin1() const

    Returns the Latin-1 value of this character, or 0 if it
    cannot be represented in Latin-1.
*/


/*!
    Returns a Latin-1 representation of the string. The
    returned value is undefined if the string contains non-Latin-1
    characters. If you want to convert strings into formats other than
    Unicode, see the TQTextCodec classes.

    This function is mainly useful for boot-strapping legacy code to
    use Unicode.

    The result remains valid so long as one unmodified copy of the
    source string exists.

    \sa fromLatin1(), ascii(), utf8(), local8Bit()
*/
const char* TQString::latin1() const
{
    if ( !d->ascii  || !d->islatin1 ) {
	if (d->security_unpaged) {
#if defined(Q_OS_LINUX)
	    if (d->ascii) {
		munlock(d->ascii, LINUX_MEMLOCK_LIMIT_BYTES);
	    }
#endif
	}
	delete [] d->ascii;
	d->ascii = unicodeToLatin1( d->unicode, d->len, d->security_unpaged );
	d->islatin1 = true;
    }
    return d->ascii;
}

/*!
    Returns an 8-bit ASCII representation of the string.

    If a codec has been set using TQTextCodec::codecForCStrings(),
    it is used to convert Unicode to 8-bit char. Otherwise, this function
    does the same as latin1().

    \sa fromAscii(), latin1(), utf8(), local8Bit()
*/
const char* TQString::ascii() const
{
#ifndef TQT_NO_TEXTCODEC
    if ( TQTextCodec::codecForCStrings() ) {
	if ( !d->ascii || d->islatin1 ) {
	    if (d->security_unpaged) {
#if defined(Q_OS_LINUX)
		if (d->ascii) {
		    munlock(d->ascii, LINUX_MEMLOCK_LIMIT_BYTES);
		}
#endif
	    }
	    delete [] d->ascii;
	    if (d->unicode) {
		TQCString s = TQTextCodec::codecForCStrings()->fromUnicode( *this );
                d->ascii = new char[s.length() + 1];
		if (d->security_unpaged) {
#if defined(Q_OS_LINUX)
		    mlock(d->ascii, LINUX_MEMLOCK_LIMIT_BYTES);
#endif
		}
                memcpy(d->ascii, s.data(), s.length() + 1);
	    } else {
		d->ascii = 0;
	    }
	    d->islatin1 = false;
	}
	return d->ascii;
    }
#endif // TQT_NO_TEXTCODEC
    return latin1();
}

void TQString::setSecurityUnPaged(bool lock) {
    if (lock != d->security_unpaged) {
	if (d->security_unpaged) {
#if defined(Q_OS_LINUX)
	    if (d->ascii) {
		munlock(d->ascii, LINUX_MEMLOCK_LIMIT_BYTES);
	    }
#endif
	    d->security_unpaged = false;
	}
	else {
#if defined(Q_OS_LINUX)
	    if (d->ascii) {
		mlock(d->ascii, LINUX_MEMLOCK_LIMIT_BYTES);
	    }
#endif
	    d->security_unpaged = true;
	}
    }
}

/*!
    Returns the string encoded in UTF-8 format.

    See TQTextCodec for more diverse coding/decoding of Unicode strings.

    \sa fromUtf8(), ascii(), latin1(), local8Bit()
*/
TQCString TQString::utf8() const
{
    if (!d->cString) {
        d->cString = new TQCString("");
    }
    if(d == shared_null)
    {
        return *d->cString;
    }

    int l = length();
    int rlen = l*3+1;
    TQCString rstr(rlen);
    uchar* cursor = (uchar*)rstr.data();
    const TQChar *ch = d->unicode;
    for (int i=0; i < l; i++) {
	uint u = ch->unicode();
 	if ( u < 0x80 ) {
 	    *cursor++ = (uchar)u;
 	} else {
 	    if ( u < 0x0800 ) {
		*cursor++ = 0xc0 | ((uchar) (u >> 6));
 	    } else {
		if (ch[0].isHighSurrogate() && i < (l - 1) && ch[1].isLowSurrogate()) {
		    u = TQChar::surrogateToUcs4(ch[0], ch[1]);
		    ++ch;
		    ++i;
		}
		if (u > 0xffff) {
		    // if people are working in utf8, but strings are encoded in eg. latin1, the resulting
		    // name might be invalid utf8. This and the corresponding code in fromUtf8 takes care
		    // we can handle this without loosing information. This can happen with latin filenames
		    // and a utf8 locale under Unix.
		    if (u > 0x10fe00 && u < 0x10ff00) {
			*cursor++ = (u - 0x10fe00);
			++ch;
			continue;
		    } else {
			*cursor++ = 0xf0 | ((uchar) (u >> 18));
			*cursor++ = 0x80 | ( ((uchar) (u >> 12)) & 0x3f);
		    }
		} else {
		    *cursor++ = 0xe0 | ((uchar) (u >> 12));
		}
 		*cursor++ = 0x80 | ( ((uchar) (u >> 6)) & 0x3f);
 	    }
 	    *cursor++ = 0x80 | ((uchar) (u&0x3f));
 	}
 	++ch;
    }
    rstr.truncate( cursor - (uchar*)rstr.data() );
    *d->cString = rstr;
    return *d->cString;
}

static TQChar *addOne(TQChar *qch, TQString &str)
{
    long sidx = qch - str.unicode();
    str.setLength(str.length()+1);
    return (TQChar *)str.unicode() + sidx;
}

/*!
    Returns the Unicode string decoded from the first \a len
    bytes of \a utf8, ignoring the rest of \a utf8. If \a len is
    -1 then the length of \a utf8 is used. If \a len is bigger than
    the length of \a utf8 then it will use the length of \a utf8.

    \code
	TQString str = TQString::fromUtf8( "123456789", 5 );
	// str == "12345"
    \endcode

    See TQTextCodec for more diverse coding/decoding of Unicode strings.
*/
TQString TQString::fromUtf8( const char* utf8, int len )
{
    if ( !utf8 ) {
	return TQString::null;
    }

    int slen = 0;
    if (len >= 0) {
	while (slen < len && utf8[slen]) {
		slen++;
	}
    } else {
        slen = int(strlen(utf8));
    }
    len = len < 0 ? slen : TQMIN(slen, len);
    TQString result;
    result.setLength( len ); // worst case
    TQChar *qch = (TQChar *)result.unicode();
    uint uc = 0;
    uint min_uc = 0;
    int need = 0;
    int error = -1;
    uchar ch;
    for (int i=0; i<len; i++) {
	ch = utf8[i];
	if (need) {
	    if ( (ch&0xc0) == 0x80 ) {
		uc = (uc << 6) | (ch & 0x3f);
		need--;
		if ( !need ) {
		    if (TQChar::requiresSurrogates(uc)) {
			// surrogate pair
			*qch++ = TQChar(TQChar::highSurrogate(uc));
			*qch++ = TQChar(TQChar::lowSurrogate(uc));
		    } else if (uc < min_uc || (uc >= 0xd800 && uc <= 0xdfff) || (uc >= 0xfffe)) {
			// overlong sequence, UTF16 surrogate or BOM
                        i = error;
                        qch = addOne(qch, result);
                        *qch++ = TQChar(0xdbff);
                        *qch++ = TQChar(0xde00+((uchar)utf8[i]));
                    } else {
			*qch++ = uc;
		    }
		}
	    } else {
		// See TQString::utf8() for explanation.
		//
		// The surrogate below corresponds to a Unicode value of (0x10fe00+ch) which
		// is in one of the private use areas of Unicode.
		i = error;
                qch = addOne(qch, result);
		*qch++ = TQChar(0xdbff);
		*qch++ = TQChar(0xde00+((uchar)utf8[i]));
		need = 0;
	    }
	} else {
	    if ( ch < 128 ) {
		*qch++ = ch;
	    } else if ((ch & 0xe0) == 0xc0) {
		uc = ch & 0x1f;
		need = 1;
		error = i;
		min_uc = 0x80;
	    } else if ((ch & 0xf0) == 0xe0) {
		uc = ch & 0x0f;
		need = 2;
		error = i;
		min_uc = 0x800;
	    } else if ((ch&0xf8) == 0xf0) {
		uc = ch & 0x07;
		need = 3;
		error = i;
		min_uc = 0x10000;
	    } else {
	        // Error
                qch = addOne(qch, result);
	        *qch++ = TQChar(0xdbff);
		*qch++ = TQChar(0xde00+((uchar)utf8[i]));
	    }
	}
    }
    if (need) {
	// we have some invalid characters remaining we need to add to the string
	for (int i = error; i < len; ++i) {
            qch = addOne(qch, result);
	    *qch++ = TQChar(0xdbff);
	    *qch++ = TQChar(0xde00+((uchar)utf8[i]));
	}
    }

    result.truncate( qch - result.unicode() );
    return result;
}

/*!
    Returns the Unicode string decoded from the first \a len
    bytes of \a ascii, ignoring the rest of \a ascii. If \a len
    is -1 then the length of \a ascii is used. If \a len is bigger
    than the length of \a ascii then it will use the length of \a
    ascii.

    If a codec has been set using TQTextCodec::codecForCStrings(),
    it is used to convert the string from 8-bit characters to Unicode.
    Otherwise, this function does the same as fromLatin1().

    This is the same as the TQString(const char*) constructor, but you
    can make that constructor invisible if you compile with the define
    \c TQT_NO_CAST_ASCII, in which case you can explicitly create a
    TQString from 8-bit ASCII text using this function.

    \code
        TQString str = TQString::fromAscii( "123456789", 5 );
        // str == "12345"
    \endcode
 */
TQString TQString::fromAscii( const char* ascii, int len )
{
#ifndef TQT_NO_TEXTCODEC
    if ( TQTextCodec::codecForCStrings() ) {
	if ( !ascii )
	    return TQString::null;
	if ( len < 0 )
	    len = (int)strlen( ascii );
	if ( len == 0 || *ascii == '\0' )
	    return TQString::fromLatin1( "" );
	return TQTextCodec::codecForCStrings()->toUnicode( ascii, len );
    }
#endif
    return fromLatin1( ascii, len );
}


/*!
    Returns the Unicode string decoded from the first \a len
    bytes of \a chars, ignoring the rest of \a chars. If \a len
    is -1 then the length of \a chars is used. If \a len is bigger
    than the length of \a chars then it will use the length of \a
    chars.

    \sa fromAscii()
*/
TQString TQString::fromLatin1( const char* chars, int len )
{
    uint l;
    TQChar *uc;
    if ( len < 0 ) {
	 len = -1;
    }
    uc = internalLatin1ToUnicode( chars, &l, len );
    TQString ret( new TQStringData(uc, l, l), true );

    return ret;
}

/*!
    \fn const TQChar* TQString::unicode() const

    Returns the Unicode representation of the string. The result
    remains valid until the string is modified.
*/

/*!
    Returns the string encoded in a locale-specific format. On X11,
    this is the TQTextCodec::codecForLocale(). On Windows, it is a
    system-defined encoding. On Mac OS X, this always uses UTF-8 as
    the encoding.

    See TQTextCodec for more diverse coding/decoding of Unicode
    strings.

    \sa fromLocal8Bit(), ascii(), latin1(), utf8()
*/

TQCString TQString::local8Bit() const
{
    if (!d->cString) {
        d->cString = new TQCString("");
    }
    if(d == shared_null)
    {
        return *d->cString;
    }
#ifdef TQT_NO_TEXTCODEC
    *d->cString = TQCString(latin1());
    return *d->cString;
#else
#ifdef TQ_WS_X11
    TQTextCodec* codec = TQTextCodec::codecForLocale();
    *d->cString = codec ? codec->fromUnicode(*this) : TQCString(latin1());
    return *d->cString;
#endif
#if defined( TQ_WS_MACX )
    return utf8();
#endif
#if defined( TQ_WS_MAC9 )
    *d->cString = TQCString(latin1()); //I'm evil..
    return *d->cString;
#endif
#ifdef TQ_WS_WIN
    *d->cString = isNull() ? TQCString("") : qt_winTQString2MB( *this );
    return *d->cString;
#endif
#ifdef TQ_WS_QWS
    return utf8(); // ### if there is any 8 bit format supported?
#endif
#endif
}

/*!
    Returns the Unicode string decoded from the first \a len
    bytes of \a local8Bit, ignoring the rest of \a local8Bit. If
    \a len is -1 then the length of \a local8Bit is used. If \a len is
    bigger than the length of \a local8Bit then it will use the length
    of \a local8Bit.

    \code
	TQString str = TQString::fromLocal8Bit( "123456789", 5 );
	// str == "12345"
    \endcode

    \a local8Bit is assumed to be encoded in a locale-specific format.

    See TQTextCodec for more diverse coding/decoding of Unicode strings.
*/
TQString TQString::fromLocal8Bit( const char* local8Bit, int len )
{
#ifdef TQT_NO_TEXTCODEC
    return fromLatin1( local8Bit, len );
#else

    if ( !local8Bit )
	return TQString::null;
#ifdef TQ_WS_X11
    TQTextCodec* codec = TQTextCodec::codecForLocale();
    if ( len < 0 )
	len = strlen( local8Bit );
    return codec
	    ? codec->toUnicode( local8Bit, len )
	    : fromLatin1( local8Bit, len );
#endif
#if defined( TQ_WS_MAC )
    return fromUtf8(local8Bit,len);
#endif
// Should this be OS_WIN32?
#ifdef TQ_WS_WIN
    if ( len >= 0 ) {
	TQCString s(local8Bit,len+1);
	return qt_winMB2TQString(s);
    }
    return qt_winMB2TQString( local8Bit );
#endif
#ifdef TQ_WS_QWS
    return fromUtf8(local8Bit,len);
#endif
#endif // TQT_NO_TEXTCODEC
}

/*!
    \fn TQString::operator const char *() const

    Returns ascii(). Be sure to see the warnings documented in the
    ascii() function. Note that for new code which you wish to be
    strictly Unicode-clean, you can define the macro \c
    TQT_NO_ASCII_CAST when compiling your code to hide this function so
    that automatic casts are not done. This has the added advantage
    that you catch the programming error described in operator!().
*/

/*!
    \fn TQString::operator std::string() const

    Returns ascii() as a std::string.

    \warning The function may cause an application to crash if a static C run-time is in use.
    This can happen in Microsoft Visual C++ if TQt is configured as single-threaded. A safe
    alternative is to call ascii() directly and construct a std::string manually.
*/

/*!
    Returns the TQString as a zero terminated array of unsigned shorts
    if the string is not null; otherwise returns zero.

    The result remains valid so long as one unmodified
    copy of the source string exists.
*/
const unsigned short *TQString::ucs2() const
{
    if ( ! d->unicode )
	return 0;
    unsigned int len = d->len;
    if ( d->maxl < len + 1 ) {
	// detach, grow or shrink
	uint newMax = computeNewMax( len + 1 );
	TQChar* nd = QT_ALLOC_QCHAR_VEC( newMax );
	if ( nd ) {
	    if ( d->unicode )
		memcpy( nd, d->unicode, sizeof(TQChar)*len );
	    ((TQString *)this)->deref();
	    ((TQString *)this)->d = new TQStringData( nd, len, newMax );
	}
    }
    d->unicode[len] = 0;
    return (unsigned short *) d->unicode;
}

/*!
  Constructs a string that is a deep copy of \a str, interpreted as a
  UCS2 encoded, zero terminated, Unicode string.

  If \a str is 0, then a null string is created.

  \sa isNull()
*/
TQString TQString::fromUcs2( const unsigned short *str )
{
    if ( !str ) {
	return TQString::null;
    } else {
	int length = 0;
	while ( str[length] != 0 )
	    length++;
	TQChar* uc = QT_ALLOC_QCHAR_VEC( length );
	memcpy( uc, str, length*sizeof(TQChar) );
	TQString ret( new TQStringData( uc, length, length ), true );
	return ret;
    }
}

/*!
  \fn TQChar TQString::at( uint ) const

    Returns the character at index \a i, or 0 if \a i is beyond the
    length of the string.

    \code
	const TQString string( "abcdefgh" );
	TQChar ch = string.at( 4 );
	// ch == 'e'
    \endcode

    If the TQString is not const (i.e. const TQString) or const& (i.e.
    const TQString &), then the non-const overload of at() will be used
    instead.
*/

/*!
    \fn TQChar TQString::constref(uint i) const

    Returns the TQChar at index \a i by value.

    Equivalent to at(\a i).

    \sa ref()
*/

/*!
    \fn TQChar& TQString::ref(uint i)

    Returns the TQChar at index \a i by reference, expanding the string
    with TQChar::null if necessary. The resulting reference can be
    assigned to, or otherwise used immediately, but becomes invalid
    once furher modifications are made to the string.

    \code
	TQString string("ABCDEF");
	TQChar ch = string.ref( 3 );         // ch == 'D'
    \endcode

    \sa constref()
*/

TQChar& TQString::ref(uint i) {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
	d->mutex->lock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
	if ( (d->count != 1) || (i >= d->len) ) {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
		d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
		subat( i );
	}
	else {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
		d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
	}
	d->setDirty();
	return d->unicode[i];
}

/*!
    \fn TQChar TQString::operator[]( int ) const

    Returns the character at index \a i, or TQChar::null if \a i is
    beyond the length of the string.

    If the TQString is not const (i.e., const TQString) or const\&
    (i.e., const TQString\&), then the non-const overload of operator[]
    will be used instead.
*/

/*!
    \fn TQCharRef TQString::operator[]( int )

    \overload

    The function returns a reference to the character at index \a i.
    The resulting reference can then be assigned to, or used
    immediately, but it will become invalid once further modifications
    are made to the original string.

    If \a i is beyond the length of the string then the string is
    expanded with TQChar::nulls, so that the TQCharRef references a
    valid (null) character in the string.

    The TQCharRef internal class can be used much like a constant
    TQChar, but if you assign to it, you change the original string
    (which will detach itself because of TQString's copy-on-write
    semantics). You will get compilation errors if you try to use the
    result as anything but a TQChar.
*/

/*!
    \fn TQCharRef TQString::at( uint i )

    \overload

    The function returns a reference to the character at index \a i.
    The resulting reference can then be assigned to, or used
    immediately, but it will become invalid once further modifications
    are made to the original string.

    If \a i is beyond the length of the string then the string is
    expanded with TQChar::null.
*/

/*
  Internal chunk of code to handle the
  uncommon cases of at() above.
*/
void TQString::subat( uint i )
{
    uint olen = d->len;
    if ( i >= olen ) {
	setLength( i+1 );               // i is index; i+1 is needed length
	for ( uint j=olen; j<=i; j++ )
	    d->unicode[j] = TQChar::null;
    } else {
	// Just be sure to detach
	real_detach();
    }
}


/*!
    Resizes the string to \a len characters and copies \a unicode into
    the string. If \a unicode is 0, nothing is copied, but the
    string is still resized to \a len. If \a len is zero, then the
    string becomes a \link isNull() null\endlink string.

    \sa setLatin1(), isNull()
*/

TQString& TQString::setUnicode( const TQChar *unicode, uint len )
{
	if ( len == 0 ) {                       // set to null string
		if ( d != shared_null ) {       // beware of nullstring being set to nullstring
			deref();
			d = shared_null ? shared_null : makeSharedNull();
		}
	}
	else {
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
		d->mutex->lock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
		if ( d->count != 1 || len > d->maxl || ( len * 4 < d->maxl && d->maxl > 4 ) ) {
			// detach, grown or shrink
			uint newMax = computeNewMax( len );
			TQChar* nd = QT_ALLOC_QCHAR_VEC( newMax );
			if ( unicode ) {
				memcpy( nd, unicode, sizeof(TQChar)*len );
			}
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
			d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
			deref();
			d = new TQStringData( nd, len, newMax );
		}
		else {
			d->len = len;
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
			d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
			d->setDirty();
			if ( unicode ) {
				memcpy( d->unicode, unicode, sizeof(TQChar)*len );
			}
		}
	}

    return *this;
}

/*!
    Resizes the string to \a len characters and copies \a
    unicode_as_ushorts into the string (on some X11 client platforms
    this will involve a byte-swapping pass).

    If \a unicode_as_ushorts is 0, nothing is copied, but the string
    is still resized to \a len. If \a len is zero, the string becomes
    a \link isNull() null\endlink string.

    \sa setLatin1(), isNull()
*/
TQString& TQString::setUnicodeCodes( const ushort* unicode_as_ushorts, uint len )
{
     return setUnicode((const TQChar*)unicode_as_ushorts, len);
}


/*!
    Sets this string to \a str, interpreted as a classic 8-bit ASCII C
    string. If \a len is -1 (the default), then it is set to
    strlen(str).

    If \a str is 0 a null string is created. If \a str is "", an empty
    string is created.

    \sa isNull(), isEmpty()
*/

TQString &TQString::setAscii( const char *str, int len )
{
#ifndef TQT_NO_TEXTCODEC
    if ( TQTextCodec::codecForCStrings() ) {
	*this = TQString::fromAscii( str, len );
	return *this;
    }
#endif // TQT_NO_TEXTCODEC
    return setLatin1( str, len );
}

/*!
    Sets this string to \a str, interpreted as a classic Latin-1 C
    string. If \a len is -1 (the default), then it is set to
    strlen(str).

    If \a str is 0 a null string is created. If \a str is "", an empty
    string is created.

    \sa isNull(), isEmpty()
*/

TQString &TQString::setLatin1( const char *str, int len )
{
    if ( str == 0 )
	return setUnicode(0,0);
    if ( len < 0 )
	len = int(strlen( str ));
    if ( len == 0 ) {                           // won't make a null string
	*this = TQString::fromLatin1( "" );
    } else {
	setUnicode( 0, len );                   // resize but not copy
	TQChar *p = d->unicode;
	while ( len-- )
	    *p++ = *str++;
    }
    return *this;
}

/*! \internal
 */
void TQString::checkSimpleText() const
{
    TQChar *p = d->unicode;
    TQChar *end = p + d->len;
    while ( p < end ) {
	ushort uc = p->unicode();
	// sort out regions of complex text formatting
	if ( uc > 0x058f && ( uc < 0x1100 || uc > 0xfb0f ) ) {
	    d->issimpletext = false;
	    return;
	}
	p++;
    }
    d->issimpletext = true;
}

/*! \fn bool TQString::simpleText() const
  \internal
*/

/*! \internal
 */
bool TQString::isRightToLeft() const
{
    int len = length();
    TQChar *p = d->unicode;
    while ( len-- ) {
	switch( ::direction( *p ) )
	{
	case TQChar::DirL:
	case TQChar::DirLRO:
	case TQChar::DirLRE:
	    return false;
	case TQChar::DirR:
	case TQChar::DirAL:
	case TQChar::DirRLO:
	case TQChar::DirRLE:
	    return true;
	default:
	    break;
	}
	++p;
    }
    return false;
}


/*!
    \fn int TQString::compare( const TQString & s1, const TQString & s2 )

    Lexically compares \a s1 with \a s2 and returns an integer less
    than, equal to, or greater than zero if \a s1 is less than, equal
    to, or greater than \a s2.

    The comparison is based exclusively on the numeric Unicode values
    of the characters and is very fast, but is not what a human would
    expect. Consider sorting user-interface strings with
    TQString::localeAwareCompare().

    \code
	int a = TQString::compare( "def", "abc" );   // a > 0
	int b = TQString::compare( "abc", "def" );   // b < 0
	int c = TQString::compare( "abc", "abc" );   // c == 0
    \endcode
*/

/*!
    \overload

   Lexically compares this string with \a s and returns an integer
   less than, equal to, or greater than zero if it is less than, equal
   to, or greater than \a s.
*/
int TQString::compare( const TQString& s ) const
{
    return ucstrcmp( *this, s );
}

/*!
    \fn int TQString::localeAwareCompare( const TQString & s1, const TQString & s2 )

    Compares \a s1 with \a s2 and returns an integer less than, equal
    to, or greater than zero if \a s1 is less than, equal to, or
    greater than \a s2.

    The comparison is performed in a locale- and also
    platform-dependent manner. Use this function to present sorted
    lists of strings to the user.

    \sa TQString::compare() TQTextCodec::locale()
*/

/*!
    \overload

    Compares this string with \a s.
*/

#if !defined(CSTR_LESS_THAN)
#define CSTR_LESS_THAN    1
#define CSTR_EQUAL        2
#define CSTR_GREATER_THAN 3
#endif

int TQString::localeAwareCompare( const TQString& s ) const
{
    // do the right thing for null and empty
    if ( isEmpty() || s.isEmpty() )
	return compare( s );

#if defined(TQ_WS_WIN)
    int res;
    QT_WA( {
	const TCHAR* s1 = (TCHAR*)ucs2();
	const TCHAR* s2 = (TCHAR*)s.ucs2();
	res = CompareStringW( LOCALE_USER_DEFAULT, 0, s1, length(), s2, s.length() );
    } , {
	TQCString s1 = local8Bit();
	TQCString s2 = s.local8Bit();
	res = CompareStringA( LOCALE_USER_DEFAULT, 0, s1.data(), s1.length(), s2.data(), s2.length() );
    } );

    switch ( res ) {
    case CSTR_LESS_THAN:
	return -1;
    case CSTR_GREATER_THAN:
	return 1;
    default:
	return 0;
    }
#elif defined(TQ_WS_MACX)
    int delta = 0;
#if !defined(TQT_NO_TEXTCODEC)
    TQTextCodec *codec = TQTextCodec::codecForLocale();
    if (codec)
        delta = strcoll(codec->fromUnicode(*this), codec->fromUnicode(s));
    if (delta == 0)
#endif
	delta = ucstrcmp(*this, s);
    return delta;
#elif defined(TQ_WS_X11)
    // declared in <string.h>
    int delta = strcoll( local8Bit(), s.local8Bit() );
    if ( delta == 0 )
	delta = ucstrcmp( *this, s );
    return delta;
#else
    return ucstrcmp( *this, s );
#endif
}

bool operator==( const TQString &s1, const TQString &s2 )
{
    if ( s1.unicode() == s2.unicode() )
	return true;
    return (s1.length() == s2.length()) && s1.isNull() == s2.isNull() &&
	 (memcmp((char*)s1.unicode(),(char*)s2.unicode(),
		 s1.length()*sizeof(TQChar)) == 0 );
}

bool operator!=( const TQString &s1, const TQString &s2 )
{ return !(s1==s2); }

bool operator<( const TQString &s1, const TQString &s2 )
{ return ucstrcmp(s1,s2) < 0; }

bool operator<=( const TQString &s1, const TQString &s2 )
{ return ucstrcmp(s1,s2) <= 0; }

bool operator>( const TQString &s1, const TQString &s2 )
{ return ucstrcmp(s1,s2) > 0; }

bool operator>=( const TQString &s1, const TQString &s2 )
{ return ucstrcmp(s1,s2) >= 0; }


bool operator==( const TQString &s1, const char *s2 )
{
    if ( !s2 )
	return s1.isNull();

    int len = s1.length();
    const TQChar *uc = s1.unicode();
    while ( len ) {
	if ( !(*s2) || uc->unicode() != (uchar) *s2 )
	    return false;
	++uc;
	++s2;
	--len;
    }
    return !*s2;
}

bool operator==( const char *s1, const TQString &s2 )
{ return (s2 == s1); }

bool operator!=( const TQString &s1, const char *s2 )
{ return !(s1==s2); }

bool operator!=( const char *s1, const TQString &s2 )
{ return !(s1==s2); }

bool operator<( const TQString &s1, const char *s2 )
{ return ucstrcmp(s1,s2) < 0; }

bool operator<( const char *s1, const TQString &s2 )
{ return ucstrcmp(s1,s2) < 0; }

bool operator<=( const TQString &s1, const char *s2 )
{ return ucstrcmp(s1,s2) <= 0; }

bool operator<=( const char *s1, const TQString &s2 )
{ return ucstrcmp(s1,s2) <= 0; }

bool operator>( const TQString &s1, const char *s2 )
{ return ucstrcmp(s1,s2) > 0; }

bool operator>( const char *s1, const TQString &s2 )
{ return ucstrcmp(s1,s2) > 0; }

bool operator>=( const TQString &s1, const char *s2 )
{ return ucstrcmp(s1,s2) >= 0; }

bool operator>=( const char *s1, const TQString &s2 )
{ return ucstrcmp(s1,s2) >= 0; }


/*****************************************************************************
  Documentation for TQString related functions
 *****************************************************************************/

/*!
    \fn bool operator==( const TQString &s1, const TQString &s2 )

    \relates TQString

    Returns true if \a s1 is equal to \a s2; otherwise returns false.
    Note that a null string is not equal to a not-null empty string.

    Equivalent to compare(\a s1, \a s2) == 0.

    \sa isNull(), isEmpty()
*/

/*!
    \fn bool operator==( const TQString &s1, const char *s2 )

    \overload
    \relates TQString

    Returns true if \a s1 is equal to \a s2; otherwise returns false.
    Note that a null string is not equal to a not-null empty string.

    Equivalent to compare(\a s1, \a s2) == 0.

    \sa isNull(), isEmpty()
*/

/*!
    \fn bool operator==( const char *s1, const TQString &s2 )

    \overload
    \relates TQString

    Returns true if \a s1 is equal to \a s2; otherwise returns false.
    Note that a null string is not equal to a not-null empty string.

    Equivalent to compare(\a s1, \a s2) == 0.

    \sa isNull(), isEmpty()
*/

/*!
    \fn bool operator!=( const TQString &s1, const TQString &s2 )

    \relates TQString

    Returns true if \a s1 is not equal to \a s2; otherwise returns false.
    Note that a null string is not equal to a not-null empty string.

    Equivalent to compare(\a s1, \a s2) != 0.

    \sa isNull(), isEmpty()
*/

/*!
    \fn bool operator!=( const TQString &s1, const char *s2 )

    \overload
    \relates TQString

    Returns true if \a s1 is not equal to \a s2; otherwise returns false.
    Note that a null string is not equal to a not-null empty string.

    Equivalent to compare(\a s1, \a s2) != 0.

    \sa isNull(), isEmpty()
*/

/*!
    \fn bool operator!=( const char *s1, const TQString &s2 )

    \overload
    \relates TQString

    Returns true if \a s1 is not equal to \a s2; otherwise returns false.
    Note that a null string is not equal to a not-null empty string.

    Equivalent to compare(\a s1, \a s2) != 0.

    \sa isNull(), isEmpty()
*/

/*!
    \fn bool operator<( const TQString &s1, const char *s2 )

    \relates TQString

    Returns true if \a s1 is lexically less than \a s2; otherwise returns false.
    The comparison is case sensitive.

    Equivalent to compare(\a s1, \a s2) \< 0.
*/

/*!
    \fn bool operator<( const char *s1, const TQString &s2 )

    \overload
    \relates TQString

    Returns true if \a s1 is lexically less than \a s2; otherwise returns false.
    The comparison is case sensitive.

    Equivalent to compare(\a s1, \a s2) \< 0.
*/

/*!
    \fn bool operator<=( const TQString &s1, const char *s2 )

    \relates TQString

    Returns true if \a s1 is lexically less than or equal to \a s2;
    otherwise returns false.
    The comparison is case sensitive.
    Note that a null string is not equal to a not-null empty string.

    Equivalent to compare(\a s1,\a s2) \<= 0.

    \sa isNull(), isEmpty()
*/

/*!
    \fn bool operator<=( const char *s1, const TQString &s2 )

    \overload
    \relates TQString

    Returns true if \a s1 is lexically less than or equal to \a s2;
    otherwise returns false.
    The comparison is case sensitive.
    Note that a null string is not equal to a not-null empty string.

    Equivalent to compare(\a s1, \a s2) \<= 0.

    \sa isNull(), isEmpty()
*/

/*!
    \fn bool operator>( const TQString &s1, const char *s2 )

    \relates TQString

    Returns true if \a s1 is lexically greater than \a s2; otherwise
    returns false.
    The comparison is case sensitive.

    Equivalent to compare(\a s1, \a s2) \> 0.
*/

/*!
    \fn bool operator>( const char *s1, const TQString &s2 )

    \overload
    \relates TQString

    Returns true if \a s1 is lexically greater than \a s2; otherwise
    returns false.
    The comparison is case sensitive.

    Equivalent to compare(\a s1, \a s2) \> 0.
*/

/*!
    \fn bool operator>=( const TQString &s1, const char *s2 )

    \relates TQString

    Returns true if \a s1 is lexically greater than or equal to \a s2;
    otherwise returns false.
    The comparison is case sensitive.
    Note that a null string is not equal to a not-null empty string.

    Equivalent to compare(\a s1, \a s2) \>= 0.

    \sa isNull(), isEmpty()
*/

/*!
    \fn bool operator>=( const char *s1, const TQString &s2 )

    \overload
    \relates TQString

    Returns true if \a s1 is lexically greater than or equal to \a s2;
    otherwise returns false.
    The comparison is case sensitive.
    Note that a null string is not equal to a not-null empty string.

    Equivalent to compare(\a s1, \a s2) \>= 0.

    \sa isNull(), isEmpty()
*/

/*!
    \fn const TQString operator+( const TQString &s1, const TQString &s2 )

    \relates TQString

    Returns a string which is the result of concatenating the string
    \a s1 and the string \a s2.

    Equivalent to \a {s1}.append(\a s2).
*/

/*!
    \fn const TQString operator+( const TQString &s1, const char *s2 )

    \overload
    \relates TQString

    Returns a string which is the result of concatenating the string
    \a s1 and character \a s2.

    Equivalent to \a {s1}.append(\a s2).
*/

/*!
    \fn const TQString operator+( const char *s1, const TQString &s2 )

    \overload
    \relates TQString

    Returns a string which is the result of concatenating the
    character \a s1 and string \a s2.
*/

/*!
    \fn const TQString operator+( const TQString &s, char c )

    \overload
    \relates TQString

    Returns a string which is the result of concatenating the string
    \a s and character \a c.

    Equivalent to \a {s}.append(\a c).
*/

/*!
    \fn const TQString operator+( char c, const TQString &s )

    \overload
    \relates TQString

    Returns a string which is the result of concatenating the
    character \a c and string \a s.

    Equivalent to \a {s}.prepend(\a c).
*/


/*****************************************************************************
  TQString stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
/*!
    \relates TQString

    Writes the string \a str to the stream \a s.

    See also \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQString &str )
{
    if ( s.version() == 1 ) {
	TQCString l( str.latin1() );
	s << l;
    }
    else {
	int byteOrder = s.byteOrder();
	const TQChar* ub = str.unicode();
	if ( ub || s.version() < 3 ) {
	    static const uint auto_size = 1024;
	    char t[auto_size];
	    char *b;
	    if ( str.length()*sizeof(TQChar) > auto_size ) {
		b = new char[str.length()*sizeof(TQChar)];
	    } else {
		b = t;
	    }
	    int l = str.length();
	    char *c=b;
	    while ( l-- ) {
		if ( byteOrder == TQDataStream::BigEndian ) {
		    *c++ = (char)ub->row();
		    *c++ = (char)ub->cell();
		} else {
		    *c++ = (char)ub->cell();
		    *c++ = (char)ub->row();
		}
		ub++;
	    }
	    s.writeBytes( b, sizeof(TQChar)*str.length() );
	    if ( str.length()*sizeof(TQChar) > auto_size )
		delete [] b;
	} else {
	    // write null marker
	    s << (TQ_UINT32)0xffffffff;
	}
    }
    return s;
}

/*!
    \relates TQString

    Reads a string from the stream \a s into string \a str.

    See also \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQString &str )
{
#ifdef QT_QSTRING_UCS_4
#if defined(Q_CC_GNU)
#warning "operator>> not working properly"
#endif
#endif
    if ( s.version() == 1 ) {
	TQCString l;
	s >> l;
	str = TQString( l );
    }
    else {
	TQ_UINT32 bytes = 0;
	s >> bytes;                                     // read size of string
	if ( bytes == 0xffffffff ) {                    // null string
	    str = TQString::null;
	} else if ( bytes > 0 ) {                       // not empty
	    int byteOrder = s.byteOrder();
	    str.setLength( bytes/2 );
	    TQChar* ch = str.d->unicode;
	    static const uint auto_size = 1024;
	    char t[auto_size];
	    char *b;
	    if ( bytes > auto_size ) {
		b = new char[bytes];
	    } else {
		b = t;
	    }
	    s.readRawBytes( b, bytes );
	    int bt = bytes/2;
	    char *oldb = b;
	    while ( bt-- ) {
		if ( byteOrder == TQDataStream::BigEndian )
		    *ch++ = (ushort) (((ushort)b[0])<<8) | (uchar)b[1];
		else
		    *ch++ = (ushort) (((ushort)b[1])<<8) | (uchar)b[0];
		b += 2;
	    }
	    if ( bytes > auto_size )
		delete [] oldb;
	} else {
	    str = "";
	}
    }
    return s;
}
#endif // TQT_NO_DATASTREAM

/*****************************************************************************
  TQConstString member functions
 *****************************************************************************/

/*!
  \class TQConstString ntqstring.h
  \reentrant
  \ingroup text
  \brief The TQConstString class provides string objects using constant Unicode data.

    In order to minimize copying, highly optimized applications can
    use TQConstString to provide a TQString-compatible object from
    existing Unicode data. It is then the programmer's responsibility
    to ensure that the Unicode data exists for the entire lifetime of
    the TQConstString object.

    A TQConstString is created with the TQConstString constructor. The
    string held by the object can be obtained by calling string().
*/

/*!
    Constructs a TQConstString that uses the first \a length Unicode
    characters in the array \a unicode. Any attempt to modify copies
    of the string will cause it to create a copy of the data, thus it
    remains forever unmodified.

    The data in \a unicode is not copied. The caller must be able to
    guarantee that \a unicode will not be deleted or modified.
*/
TQConstString::TQConstString( const TQChar* unicode, uint length ) :
    TQString( new TQStringData( (TQChar*)unicode, length, length ), true )
{
}

/*!
    Destroys the TQConstString, creating a copy of the data if other
    strings are still using it.
*/
TQConstString::~TQConstString()
{
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
    d->mutex->lock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE

    if ( d->count > 1 ) {
	TQChar* cp = QT_ALLOC_QCHAR_VEC( d->len );
	memcpy( cp, d->unicode, d->len*sizeof(TQChar) );
	d->unicode = cp;
    }
    else {
	d->unicode = 0;
    }

    // The original d->unicode is now unlinked.
#if defined(TQT_THREAD_SUPPORT) && defined(MAKE_QSTRING_THREAD_SAFE)
    d->mutex->unlock();
#endif // TQT_THREAD_SUPPORT && MAKE_QSTRING_THREAD_SAFE
}

/*!
    \fn const TQString& TQConstString::string() const

    Returns a constant string referencing the data passed during
    construction.
*/

/*!
    Returns true if the string starts with \a s; otherwise returns
    false.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.

    \code
	TQString str( "Bananas" );
	str.startsWith( "Ban" );     // returns true
	str.startsWith( "Car" );     // returns false
    \endcode

    \sa endsWith()
*/
bool TQString::startsWith( const TQString& s, bool cs ) const
{
    if ( isNull() )
	return s.isNull();
    if ( s.length() > length() )
	return false;
    if ( cs ) {
        return memcmp((char*)d->unicode, (char*)s.d->unicode, s.length()*sizeof(TQChar)) == 0;
    } else {
	for ( int i = 0; i < (int) s.length(); i++ ) {
	    if ( ::lower(d->unicode[i]) != ::lower(s.d->unicode[i]) )
		return false;
	}
    }
    return true;
}

bool TQString::startsWith( const TQString& s ) const
{
    return startsWith( s, true );
}

/*!
    Returns true if the string ends with \a s; otherwise returns
    false.

    If \a cs is true (the default), the search is case sensitive;
    otherwise the search is case insensitive.

    \code
	TQString str( "Bananas" );
	str.endsWith( "anas" );         // returns true
	str.endsWith( "pple" );         // returns false
    \endcode

    \sa startsWith()
*/
bool TQString::endsWith( const TQString& s, bool cs ) const
{
    if ( isNull() )
	return s.isNull();
    int pos = length() - s.length();
    if ( pos < 0 )
	return false;
    if ( cs ) {
        return memcmp((char*)&d->unicode[pos], (char*)s.d->unicode, s.length()*sizeof(TQChar)) == 0;
    } else {
	for ( int i = 0; i < (int) s.length(); i++ ) {
	    if ( ::lower(d->unicode[pos + i]) != ::lower(s.d->unicode[i]) )
		return false;
	}
    }
    return true;
}

bool TQString::endsWith( const TQString& s ) const
{
    return endsWith( s, true );
}

/*! \fn void TQString::detach()
  If the string does not share its data with another TQString instance,
  nothing happens; otherwise the function creates a new, unique copy of
  this string. This function is called whenever the string is modified. The
  implicit sharing mechanism is implemented this way.
*/

#if defined(Q_OS_WIN32)

#include <windows.h>

/*!
  \obsolete

  Returns a static Windows TCHAR* from a TQString, adding NUL if \a
  addnul is true.

  The lifetime of the return value is until the next call to this function,
  or until the last copy of str is deleted, whatever comes first.

  Use ucs2() instead.
*/
const void* qt_winTchar(const TQString& str, bool)
{
    // So that the return value lives long enough.
    static TQString str_cache;
    str_cache = str;
#ifdef UNICODE
    return str_cache.ucs2();
#else
    return str_cache.latin1();
#endif
}

/*!
    Makes a new '\0'-terminated Windows TCHAR* from a TQString.
*/
void* qt_winTchar_new(const TQString& str)
{
    if ( str.isNull() )
	return 0;
    int l = str.length()+1;
    TCHAR *tc = new TCHAR[ l ];
#ifdef UNICODE
    memcpy( tc, str.ucs2(), sizeof(TCHAR)*l );
#else
    memcpy( tc, str.latin1(), sizeof(TCHAR)*l );
#endif
    return tc;
}

/*!
    Makes a TQString from a Windows TCHAR*.
*/
TQString qt_winTQString(void* tc)
{
#ifdef UNICODE
    return TQString::fromUcs2( (ushort*)tc );
#else
    return TQString::fromLatin1( (TCHAR *)tc );
#endif
}

TQCString qt_winTQString2MB( const TQString& s, int uclen )
{
    if ( uclen < 0 )
	uclen = s.length();
    if ( s.isNull() )
	return TQCString();
    if ( uclen == 0 )
	return TQCString("");
    BOOL used_def;
    TQCString mb(4096);
    int len;
    while ( !(len=WideCharToMultiByte(CP_ACP, 0, (const WCHAR*)s.unicode(), uclen,
		mb.data(), mb.size()-1, 0, &used_def)) )
    {
	int r = GetLastError();
	if ( r == ERROR_INSUFFICIENT_BUFFER ) {
	    mb.resize(1+WideCharToMultiByte( CP_ACP, 0,
				(const WCHAR*)s.unicode(), uclen,
				0, 0, 0, &used_def));
		// and try again...
	} else {
#ifndef TQT_NO_DEBUG
	    // Fail.
	    tqWarning("WideCharToMultiByte cannot convert multibyte text (error %d): %s (UTF8)",
		r, s.utf8().data());
#endif
	    break;
	}
    }
    mb[len]='\0';
    return mb;
}

// WATCH OUT: mblen must include the NUL (or just use -1)
TQString qt_winMB2TQString( const char* mb, int mblen )
{
    if ( !mb || !mblen )
	return TQString::null;
    const int wclen_auto = 4096;
    WCHAR wc_auto[wclen_auto];
    int wclen = wclen_auto;
    WCHAR *wc = wc_auto;
    int len;
    while ( !(len=MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED,
		mb, mblen, wc, wclen )) )
    {
	int r = GetLastError();
	if ( r == ERROR_INSUFFICIENT_BUFFER ) {
	    if ( wc != wc_auto ) {
		tqWarning("Size changed in MultiByteToWideChar");
		break;
	    } else {
		wclen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED,
				    mb, mblen, 0, 0 );
		wc = new WCHAR[wclen];
		// and try again...
	    }
	} else {
	    // Fail.
	    tqWarning("MultiByteToWideChar cannot convert multibyte text");
	    break;
	}
    }
    if ( len <= 0 )
	return TQString::null;
    TQString s( (TQChar*)wc, len - 1 ); // len - 1: we don't want terminator
    if ( wc != wc_auto )
	delete [] wc;
    return s;
}

#endif // Q_OS_WIN32
