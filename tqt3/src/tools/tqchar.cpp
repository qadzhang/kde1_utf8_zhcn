/****************************************************************************
**
** Implementation of the TQChar class and related Unicode functions
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

#include "tqchar.h"
#include "tqunicodetables_p.h"

/*!
    \class TQChar tqchar.h
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


const TQChar TQChar::null;
const TQChar TQChar::replacement((ushort)0xfffd);
const TQChar TQChar::byteOrderMark((ushort)0xfeff);
const TQChar TQChar::byteOrderSwapped((ushort)0xfffe);
const TQChar TQChar::nbsp((ushort)0x00a0);


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
