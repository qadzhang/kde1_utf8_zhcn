/****************************************************************************
**
** Implementation of validator classes
**
** Created : 970610
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
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

#include "ntqvalidator.h"
#ifndef TQT_NO_VALIDATOR

#include <limits.h>
#include <math.h>

/*!
    \class TQValidator
    \brief The TQValidator class provides validation of input text.

    \ingroup misc
    \mainclass

    The class itself is abstract. Two subclasses, \l TQIntValidator and
    \l TQDoubleValidator, provide basic numeric-range checking, and \l
    TQRegExpValidator provides general checking using a custom regular
    expression.

    If the built-in validators aren't sufficient, you can subclass
    TQValidator. The class has two virtual functions: validate() and
    fixup().

    \l validate() must be implemented by every subclass. It returns
    \c Invalid, \c Intermediate or \c Acceptable depending on whether
    its argument is valid (for the subclass's definition of valid).

    These three states require some explanation. An \c Invalid string
    is \e clearly invalid. \c Intermediate is less obvious: the
    concept of validity is slippery when the string is incomplete
    (still being edited). TQValidator defines \c Intermediate as the
    property of a string that is neither clearly invalid nor
    acceptable as a final result. \c Acceptable means that the string
    is acceptable as a final result. One might say that any string
    that is a plausible intermediate state during entry of an \c
    Acceptable string is \c Intermediate.

    Here are some examples:

    \list

    \i For a line edit that accepts integers from 0 to 999 inclusive,
    42 and 123 are \c Acceptable, the empty string and 1114 are \c
    Intermediate and asdf is \c Invalid.

    \i For an editable combobox that accepts URLs, any well-formed URL
    is \c Acceptable, "http://www.trolltech.com/," is \c Intermediate
    (it might be a cut and paste operation that accidentally took in a
    comma at the end), the empty string is \c Intermediate (the user
    might select and delete all of the text in preparation for entering
    a new URL), and "http:///./" is \c Invalid.

    \i For a spin box that accepts lengths, "11cm" and "1in" are \c
    Acceptable, "11" and the empty string are \c Intermediate and
    "http://www.trolltech.com" and "hour" are \c Invalid.

    \endlist

    \l fixup() is provided for validators that can repair some user
    errors. The default implementation does nothing. TQLineEdit, for
    example, will call fixup() if the user presses Enter (or Return)
    and the content is not currently valid. This allows the fixup()
    function the opportunity of performing some magic to make an \c
    Invalid string \c Acceptable.

    TQValidator is typically used with TQLineEdit, TQSpinBox and
    TQComboBox.
*/


/*!
    \enum TQValidator::State

    This enum type defines the states in which a validated string can
    exist.

    \value Invalid  the string is \e clearly invalid.

    \value Intermediate  the string is a plausible intermediate value
    during editing.

    \value Acceptable  the string is acceptable as a final result,
    i.e. it is valid.
*/


/*!
    Sets up the validator. The \a parent and \a name parameters are
    passed on to the TQObject constructor.
*/

TQValidator::TQValidator( TQObject * parent, const char *name )
    : TQObject( parent, name )
{
}


/*!
    Destroys the validator, freeing any storage and other resources
    used.
*/

TQValidator::~TQValidator()
{
}


/*!
    \fn TQValidator::State TQValidator::validate( TQString& input, int& pos ) const

    This pure virtual function returns \c Invalid if \a input is
    invalid according to this validator's rules, \c Intermediate if it
    is likely that a little more editing will make the input
    acceptable (e.g. the user types '4' into a widget which accepts
    integers between 10 and 99) and \c Acceptable if the input is
    valid.

    The function can change \a input and \a pos (the cursor position)
    if it wants to.
*/


/*!
    \fn void TQValidator::fixup( TQString & input ) const

    This function attempts to change \a input to be valid according to
    this validator's rules. It need not result in a valid string:
    callers of this function must re-test afterwards; the default does
    nothing.

    Reimplementations of this function can change \a input even if
    they do not produce a valid string. For example, an ISBN validator
    might want to delete every character except digits and "-", even
    if the result is still not a valid ISBN; a surname validator might
    want to remove whitespace from the start and end of the string,
    even if the resulting string is not in the list of accepted
    surnames.
*/

void TQValidator::fixup( TQString & ) const
{
}


/*!
    \class TQIntValidator
    \brief The TQIntValidator class provides a validator which ensures
    that a string contains a valid integer within a specified range.

    \ingroup misc

    Example of use:

    \code
    TQValidator* validator = new TQIntValidator( 100, 999, this );
    TQLineEdit* edit = new TQLineEdit( this );

    // the edit lineedit will only accept integers between 100 and 999
    edit->setValidator( validator );
    \endcode

    Below we present some examples of validators. In practice they would
    normally be associated with a widget as in the example above.

    \code
    TQString str;
    int pos = 0;
    TQIntValidator v( 100, 999, this );

    str = "1";
    v.validate( str, pos );     // returns Intermediate
    str = "12";
    v.validate( str, pos );     // returns Intermediate

    str = "123";
    v.validate( str, pos );     // returns Acceptable
    str = "678";
    v.validate( str, pos );     // returns Acceptable

    str = "1234";
    v.validate( str, pos );     // returns Invalid
    str = "-123";
    v.validate( str, pos );     // returns Invalid
    str = "abc";
    v.validate( str, pos );     // returns Invalid
    str = "12cm";
    v.validate( str, pos );     // returns Invalid
    \endcode

    The minimum and maximum values are set in one call with setRange()
    or individually with setBottom() and setTop().

    \sa TQDoubleValidator TQRegExpValidator
*/


/*!
    Constructs a validator called \a name with parent \a parent, that
    accepts all integers.
*/

TQIntValidator::TQIntValidator( TQObject * parent, const char *name )
    : TQValidator( parent, name )
{
    b = INT_MIN;
    t = INT_MAX;
}


/*!
    Constructs a validator called \a name with parent \a parent, that
    accepts integers from \a minimum to \a maximum inclusive.
*/

TQIntValidator::TQIntValidator( int minimum, int maximum,
			      TQObject * parent, const char* name )
    : TQValidator( parent, name )
{
    b = minimum;
    t = maximum;
}


/*!
    Destroys the validator, freeing any resources allocated.
*/

TQIntValidator::~TQIntValidator()
{
    // nothing
}


/*!
    Returns \c Acceptable if the \a input is an integer within the
    valid range, \c Intermediate if the \a input is an integer outside
    the valid range and \c Invalid if the \a input is not an integer.

    Note: If the valid range consists of just positive integers (e.g. 32 - 100)
    and \a input is a negative integer then Invalid is returned.

    \code
    int pos = 0;
    s = "abc";
    v.validate( s, pos ); // returns Invalid

    s = "5";
    v.validate( s, pos ); // returns Intermediate

    s = "50";
    v.validate( s, pos ); // returns Valid
    \endcode
*/

TQValidator::State TQIntValidator::validate( TQString & input, int & ) const
{
    TQString stripped = input.stripWhiteSpace();
    if ( stripped.isEmpty() || (b < 0 && stripped == "-") )
	return Intermediate;
    bool ok;
    long entered = input.toLong( &ok );
    if ( !ok || (entered < 0 && b >= 0) ) {
	return Invalid;
    } else if ( entered >= b && entered <= t ) {
	return Acceptable;
    } else {
	if ( entered >= 0 )
	    return ( entered > t ) ? Invalid : Intermediate;
	else
	    return ( entered < b ) ? Invalid : Intermediate;
    }
}


/*!
    Sets the range of the validator to only accept integers between \a
    bottom and \a top inclusive.
*/

void TQIntValidator::setRange( int bottom, int top )
{
    b = bottom;
    t = top;
}


/*!
    \property TQIntValidator::bottom
    \brief the validator's lowest acceptable value

    \sa setRange()
*/
void TQIntValidator::setBottom( int bottom )
{
    setRange( bottom, top() );
}

/*!
    \property TQIntValidator::top
    \brief the validator's highest acceptable value

    \sa setRange()
*/
void TQIntValidator::setTop( int top )
{
    setRange( bottom(), top );
}


#ifndef TQT_NO_REGEXP

/*!
    \class TQDoubleValidator

    \brief The TQDoubleValidator class provides range checking of
    floating-point numbers.

    \ingroup misc

    TQDoubleValidator provides an upper bound, a lower bound and a
    limit on the number of digits after the decimal point. It does not
    provide a fixup() function.

    You can set the acceptable range in one call with setRange(), or
    with setBottom() and setTop(). Set the number of decimal places
    with setDecimals(). The validate() function returns the validation
    state.

    \sa TQIntValidator TQRegExpValidator
*/

/*!
    Constructs a validator object with parent \a parent, called \a
    name, which accepts any double.
*/

TQDoubleValidator::TQDoubleValidator( TQObject * parent, const char *name )
    : TQValidator( parent, name )
{
    b = -HUGE_VAL;
    t = HUGE_VAL;
    d = 1000;
}


/*!
    Constructs a validator object with parent \a parent, called \a
    name. This validator will accept doubles from \a bottom to \a top
    inclusive, with up to \a decimals digits after the decimal point.
*/

TQDoubleValidator::TQDoubleValidator( double bottom, double top, int decimals,
				    TQObject * parent, const char* name )
    : TQValidator( parent, name )
{
    b = bottom;
    t = top;
    d = decimals;
}


/*!
    Destroys the validator, freeing any resources used.
*/

TQDoubleValidator::~TQDoubleValidator()
{
}


/*!
    Returns \c Acceptable if the string \a input contains a double
    that is within the valid range and is in the correct format.

    Returns \c Intermediate if \a input contains a double that is
    outside the range or is in the wrong format, e.g. with too many
    digits after the decimal point or is empty.

    Returns \c Invalid if the \a input is not a double.

    Note: If the valid range consists of just positive doubles (e.g. 0.0 - 100.0)
    and \a input is a negative double then Invalid is returned.
*/

TQValidator::State TQDoubleValidator::validate( TQString & input, int & ) const
{
    TQRegExp empty( TQString::fromLatin1(" *-?\\.? *") );
    if ( b >= 0 &&
	 input.stripWhiteSpace().startsWith(TQString::fromLatin1("-")) )
	return Invalid;
    if ( empty.exactMatch(input) )
	return Intermediate;
    bool ok = true;
    double entered = input.toDouble( &ok );
    int nume = input.contains( 'e', false );
    if ( !ok ) {
	// explicit exponent regexp
	TQRegExp expexpexp( TQString::fromLatin1("[Ee][+-]?\\d*$") );
	int eeePos = expexpexp.search( input );
	if ( eeePos > 0 && nume == 1 ) {
	    TQString mantissa = input.left( eeePos );
	    entered = mantissa.toDouble( &ok );
	    if ( !ok )
		return Invalid;
	} else if ( eeePos == 0 ) {
	    return Intermediate;
	} else {
	    return Invalid;
	}
    }

    int i = input.find( '.' );
    if ( i >= 0 && nume == 0 ) {
	// has decimal point (but no E), now count digits after that
	i++;
	int j = i;
	while( input[j].isDigit() )
	    j++;
	if ( j - i > d )
	    return Intermediate;
    }

    if ( entered < b || entered > t )
	return Intermediate;
    else
	return Acceptable;
}


/*!
    Sets the validator to accept doubles from \a minimum to \a maximum
    inclusive, with at most \a decimals digits after the decimal
    point.
*/

void TQDoubleValidator::setRange( double minimum, double maximum, int decimals )
{
    b = minimum;
    t = maximum;
    d = decimals;
}

/*!
    \property TQDoubleValidator::bottom
    \brief the validator's minimum acceptable value

    \sa setRange()
*/

void TQDoubleValidator::setBottom( double bottom )
{
    setRange( bottom, top(), decimals() );
}


/*!
    \property TQDoubleValidator::top
    \brief the validator's maximum acceptable value

    \sa setRange()
*/

void TQDoubleValidator::setTop( double top )
{
    setRange( bottom(), top, decimals() );
}

/*!
    \property TQDoubleValidator::decimals
    \brief the validator's maximum number of digits after the decimal point

    \sa setRange()
*/

void TQDoubleValidator::setDecimals( int decimals )
{
    setRange( bottom(), top(), decimals );
}


/*!
    \class TQRegExpValidator
    \brief The TQRegExpValidator class is used to check a string
    against a regular expression.

    \ingroup misc

    TQRegExpValidator contains a regular expression, "regexp", used to
    determine whether an input string is \c Acceptable, \c
    Intermediate or \c Invalid.

    The regexp is treated as if it begins with the start of string
    assertion, <b>^</b>, and ends with the end of string assertion
    <b>$</b> so the match is against the entire input string, or from
    the given position if a start position greater than zero is given.

    For a brief introduction to TQt's regexp engine see \l TQRegExp.

    Example of use:
    \code
    // regexp: optional '-' followed by between 1 and 3 digits
    TQRegExp rx( "-?\\d{1,3}" );
    TQValidator* validator = new TQRegExpValidator( rx, this );

    TQLineEdit* edit = new TQLineEdit( this );
    edit->setValidator( validator );
    \endcode

    Below we present some examples of validators. In practice they would
    normally be associated with a widget as in the example above.

    \code
    // integers 1 to 9999
    TQRegExp rx( "[1-9]\\d{0,3}" );
    // the validator treats the regexp as "^[1-9]\\d{0,3}$"
    TQRegExpValidator v( rx, 0 );
    TQString s;
    int pos = 0;

    s = "0";     v.validate( s, pos );    // returns Invalid
    s = "12345"; v.validate( s, pos );    // returns Invalid
    s = "1";     v.validate( s, pos );    // returns Acceptable

    rx.setPattern( "\\S+" );            // one or more non-whitespace characters
    v.setRegExp( rx );
    s = "myfile.txt";  v.validate( s, pos ); // Returns Acceptable
    s = "my file.txt"; v.validate( s, pos ); // Returns Invalid

    // A, B or C followed by exactly five digits followed by W, X, Y or Z
    rx.setPattern( "[A-C]\\d{5}[W-Z]" );
    v.setRegExp( rx );
    s = "a12345Z"; v.validate( s, pos );	// Returns Invalid
    s = "A12345Z"; v.validate( s, pos );	// Returns Acceptable
    s = "B12";     v.validate( s, pos );	// Returns Intermediate

    // match most 'readme' files
    rx.setPattern( "read\\S?me(\.(txt|asc|1st))?" );
    rx.setCaseSensitive( false );
    v.setRegExp( rx );
    s = "readme";      v.validate( s, pos ); // Returns Acceptable
    s = "README.1ST";  v.validate( s, pos ); // Returns Acceptable
    s = "read me.txt"; v.validate( s, pos ); // Returns Invalid
    s = "readm";       v.validate( s, pos ); // Returns Intermediate
    \endcode

    \sa TQRegExp TQIntValidator TQDoubleValidator
*/

/*!
    Constructs a validator that accepts any string (including an empty
    one) as valid. The object's parent is \a parent and its name is \a
    name.
*/

TQRegExpValidator::TQRegExpValidator( TQObject *parent, const char *name )
    : TQValidator( parent, name ), r( TQString::fromLatin1(".*") )
{
}

/*!
    Constructs a validator which accepts all strings that match the
    regular expression \a rx. The object's parent is \a parent and its
    name is \a name.

    The match is made against the entire string, e.g. if the regexp is
    <b>[A-Fa-f0-9]+</b> it will be treated as <b>^[A-Fa-f0-9]+$</b>.
*/

TQRegExpValidator::TQRegExpValidator( const TQRegExp& rx, TQObject *parent,
				    const char *name )
    : TQValidator( parent, name ), r( rx )
{
}

/*!
    Destroys the validator, freeing any resources allocated.
*/

TQRegExpValidator::~TQRegExpValidator()
{
}

/*!
    Returns \c Acceptable if \a input is matched by the regular
    expression for this validator, \c Intermediate if it has matched
    partially (i.e. could be a valid match if additional valid
    characters are added), and \c Invalid if \a input is not matched.

    The \a pos parameter is set to the length of the \a input parameter.

    For example, if the regular expression is <b>\\w\\d\\d</b> (that
    is, word-character, digit, digit) then "A57" is \c Acceptable,
    "E5" is \c Intermediate and "+9" is \c Invalid.

    \sa TQRegExp::match() TQRegExp::search()
*/

TQValidator::State TQRegExpValidator::validate( TQString& input, int& pos ) const
{
    if ( r.exactMatch(input) ) {
	return Acceptable;
    } else {
	if ( ((TQRegExp&) r).matchedLength() == (int) input.length() ) {
	    return Intermediate;
	} else {
	    pos = input.length();
	    return Invalid;
	}
    }
}

/*!
    Sets the regular expression used for validation to \a rx.

    \sa regExp()
*/

void TQRegExpValidator::setRegExp( const TQRegExp& rx )
{
    r = rx;
}

/*!
    \fn const TQRegExp& TQRegExpValidator::regExp() const

    Returns the regular expression used for validation.

    \sa setRegExp()
*/

#endif

#endif
