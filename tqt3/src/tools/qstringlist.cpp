/****************************************************************************
**
** Implementation of TQStringList
**
** Created : 990406
**
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

#include "ntqstringlist.h"

#ifndef TQT_NO_STRINGLIST
#include "ntqregexp.h"
#include "ntqstrlist.h"
#include "ntqdatastream.h"
#include "ntqtl.h"

/*!
    \class TQStringList ntqstringlist.h
    \reentrant
    \brief The TQStringList class provides a list of strings.

    \ingroup tools
    \ingroup shared
    \ingroup text
    \mainclass

    It is used to store and manipulate strings that logically belong
    together. Essentially TQStringList is a TQValueList of TQString
    objects. Unlike TQStrList, which stores pointers to characters,
    TQStringList holds real TQString objects. It is the class of choice
    whenever you work with Unicode strings. TQStringList is part of the
    \link ntqtl.html TQt Template Library\endlink.

    Like TQString itself, TQStringList objects are implicitly shared, so
    passing them around as value-parameters is both fast and safe.

    Strings can be added to a list using append(), operator+=() or
    operator<<(), e.g.
    \code
    TQStringList fonts;
    fonts.append( "Times" );
    fonts += "Courier";
    fonts += "Courier New";
    fonts << "Helvetica [Cronyx]" << "Helvetica [Adobe]";
    \endcode

    String lists have an iterator, TQStringList::Iterator(), e.g.
    \code
    for ( TQStringList::Iterator it = fonts.begin(); it != fonts.end(); ++it ) {
	cout << *it << ":";
    }
    cout << endl;
    // Output:
    //	Times:Courier:Courier New:Helvetica [Cronyx]:Helvetica [Adobe]:
    \endcode

    Many TQt functions return string lists by value; to iterate over
    these you should make a copy and iterate over the copy.

    You can concatenate all the strings in a string list into a single
    string (with an optional separator) using join(), e.g.
    \code
    TQString allFonts = fonts.join( ", " );
    cout << allFonts << endl;
    // Output:
    //	Times, Courier, Courier New, Helvetica [Cronyx], Helvetica [Adobe]
    \endcode

    You can sort the list with sort(), and extract a new list which
    contains only those strings which contain a particular substring
    (or match a particular regular expression) using the grep()
    functions, e.g.
    \code
    fonts.sort();
    cout << fonts.join( ", " ) << endl;
    // Output:
    //	Courier, Courier New, Helvetica [Adobe], Helvetica [Cronyx], Times

    TQStringList helveticas = fonts.grep( "Helvetica" );
    cout << helveticas.join( ", " ) << endl;
    // Output:
    //	Helvetica [Adobe], Helvetica [Cronyx]
    \endcode

    Existing strings can be split into string lists with character,
    string or regular expression separators, e.g.
    \code
    TQString s = "Red\tGreen\tBlue";
    TQStringList colors = TQStringList::split( "\t", s );
    cout << colors.join( ", " ) << endl;
    // Output:
    //	Red, Green, Blue
    \endcode
*/

/*!
    \fn TQStringList::TQStringList()

    Creates an empty string list.
*/

/*!
    \fn TQStringList::TQStringList( const TQStringList& l )

    Creates a copy of the list \a l. This function is very fast
    because TQStringList is implicitly shared. In most situations this
    acts like a deep copy, for example, if this list or the original
    one or some other list referencing the same shared data is
    modified, the modifying list first makes a copy, i.e.
    copy-on-write.
    In a threaded environment you may require a real deep copy
    \omit see \l TQDeepCopy\endomit.
*/

/*!
    \fn TQStringList::TQStringList (const TQString & i)

    Constructs a string list consisting of the single string \a i.
    Longer lists are easily created as follows:

    \code
    TQStringList items;
    items << "Buy" << "Sell" << "Update" << "Value";
    \endcode
*/

/*!
    \fn TQStringList::TQStringList (const char* i)

    Constructs a string list consisting of the single Latin-1 string \a i.
*/

/*!
    \fn TQStringList::TQStringList( const TQValueList<TQString>& l )

    Constructs a new string list that is a copy of \a l.
*/

/*!
    Sorts the list of strings in ascending case-sensitive order.

    Sorting is very fast. It uses the \link ntqtl.html TQt Template
    Library's\endlink efficient HeapSort implementation that has a
    time complexity of O(n*log n).

    If you want to sort your strings in an arbitrary order consider
    using a TQMap. For example you could use a TQMap\<TQString,TQString\>
    to create a case-insensitive ordering (e.g. mapping the lowercase
    text to the text), or a TQMap\<int,TQString\> to sort the strings by
    some integer index, etc.
*/
void TQStringList::sort()
{
    qHeapSort( *this );
}

/*!
    \overload

    This version of the function uses a TQChar as separator, rather
    than a regular expression.

    \sa join() TQString::section()
*/

TQStringList TQStringList::split( const TQChar &sep, const TQString &str,
				bool allowEmptyEntries )
{
    return split( TQString(sep), str, allowEmptyEntries );
}

/*!
    \overload

    This version of the function uses a TQString as separator, rather
    than a regular expression.

    If \a sep is an empty string, the return value is a list of
    one-character strings: split( TQString( "" ), "four" ) returns the
    four-item list, "f", "o", "u", "r".

    If \a allowEmptyEntries is true, a null string is inserted in
    the list wherever the separator matches twice without intervening
    text.

    \sa join() TQString::section()
*/

TQStringList TQStringList::split( const TQString &sep, const TQString &str,
				bool allowEmptyEntries )
{
    TQStringList lst;

    int j = 0;
    int i = str.find( sep, j );

    while ( i != -1 ) {
	if ( i > j && i <= (int)str.length() )
	    lst << str.mid( j, i - j );
	else if ( allowEmptyEntries )
	    lst << TQString::null;
	j = i + sep.length();
	i = str.find( sep, sep.length() > 0 ? j : j+1 );
    }

    int l = str.length() - 1;
    if ( str.mid( j, l - j + 1 ).length() > 0 )
	lst << str.mid( j, l - j + 1 );
    else if ( allowEmptyEntries )
	lst << TQString::null;

    return lst;
}

#ifndef TQT_NO_REGEXP
/*!
    Splits the string \a str into strings wherever the regular
    expression \a sep occurs, and returns the list of those strings.

    If \a allowEmptyEntries is true, a null string is inserted in
    the list wherever the separator matches twice without intervening
    text.

    For example, if you split the string "a,,b,c" on commas, split()
    returns the three-item list "a", "b", "c" if \a allowEmptyEntries
    is false (the default), and the four-item list "a", "", "b", "c"
    if \a allowEmptyEntries is true.

    If \a sep does not match anywhere in \a str, split() returns a
    single element list with the element containing the single string
    \a str.

    \sa join() TQString::section()
*/

TQStringList TQStringList::split( const TQRegExp &sep, const TQString &str,
				bool allowEmptyEntries )
{
    TQStringList lst;

    TQRegExp tep = sep;

    int j = 0;
    int i = tep.search( str, j );

    while ( i != -1 ) {
	if ( str.mid( j, i - j ).length() > 0 )
	    lst << str.mid( j, i - j );
	else if ( allowEmptyEntries )
	    lst << TQString::null;
	if ( tep.matchedLength() == 0 )
	    j = i + 1;
	else
	    j = i + tep.matchedLength();
	i = tep.search( str, j );
    }

    int l = str.length() - 1;
    if ( str.mid( j, l - j + 1 ).length() > 0 )
	lst << str.mid( j, l - j + 1 );
    else if ( allowEmptyEntries )
	lst << TQString::null;

    return lst;
}
#endif

/*!
    Returns a list of all the strings containing the substring \a str.

    If \a cs is true, the grep is done case-sensitively; otherwise
    case is ignored.

    \code
    TQStringList list;
    list << "Bill Gates" << "John Doe" << "Bill Clinton";
    list = list.grep( "Bill" );
    // list == ["Bill Gates", "Bill Clinton"]
    \endcode

    \sa TQString::find()
*/

TQStringList TQStringList::grep( const TQString &str, bool cs ) const
{
    TQStringList res;
    for ( TQStringList::ConstIterator it = begin(); it != end(); ++it )
	if ( (*it).contains(str, cs) )
	    res << *it;

    return res;
}

#ifndef TQT_NO_REGEXP
/*!
    \overload

    Returns a list of all the strings that match the regular
    expression \a rx.

    \sa TQString::find()
*/

TQStringList TQStringList::grep( const TQRegExp &rx ) const
{
    TQStringList res;
    for ( TQStringList::ConstIterator it = begin(); it != end(); ++it )
	if ( (*it).find(rx) != -1 )
	    res << *it;

    return res;
}
#endif

/*!
    Replaces every occurrence of the string \a before in the strings
    that constitute the string list with the string \a after. Returns
    a reference to the string list.

    If \a cs is true, the search is case sensitive; otherwise the
    search is case insensitive.

    Example:
    \code
    TQStringList list;
    list << "alpha" << "beta" << "gamma" << "epsilon";
    list.gres( "a", "o" );
    // list == ["olpho", "beto", "gommo", "epsilon"]
    \endcode

    \sa TQString::replace()
*/
TQStringList& TQStringList::gres( const TQString &before, const TQString &after,
				bool cs )
{
    TQStringList::Iterator it = begin();
    while ( it != end() ) {
	(*it).replace( before, after, cs );
	++it;
    }
    return *this;
}

#ifndef TQT_NO_REGEXP_CAPTURE
/*!
    \overload

    Replaces every occurrence of the regexp \a rx in the string
    with \a after. Returns a reference to the string list.

    Example:
    \code
    TQStringList list;
    list << "alpha" << "beta" << "gamma" << "epsilon";
    list.gres( TQRegExp("^a"), "o" );
    // list == ["olpha", "beta", "gamma", "epsilon"]
    \endcode

    For regexps containing \link ntqregexp.html#capturing-text
    capturing parentheses \endlink, occurrences of <b>\\1</b>,
    <b>\\2</b>, ..., in \a after are replaced with \a{rx}.cap(1),
    cap(2), ...

    Example:
    \code
    TQStringList list;
    list << "Bill Clinton" << "Gates, Bill";
    list.gres( TQRegExp("^(.*), (.*)$"), "\\2 \\1" );
    // list == ["Bill Clinton", "Bill Gates"]
    \endcode

    \sa TQString::replace()
*/
TQStringList& TQStringList::gres( const TQRegExp &rx, const TQString &after )
{
    TQStringList::Iterator it = begin();
    while ( it != end() ) {
	(*it).replace( rx, after );
	++it;
    }
    return *this;
}

#endif

/*!
    Joins the string list into a single string with each element
    separated by the string \a sep (which can be empty).

    \sa split()
*/
TQString TQStringList::join( const TQString &sep ) const
{
    TQString res;
    bool alredy = false;
    for ( TQStringList::ConstIterator it = begin(); it != end(); ++it ) {
	if ( alredy )
	    res += sep;
	alredy = true;
	res += *it;
    }

    return res;
}

#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator>>( TQDataStream & s, TQStringList& l )
{
    return s >> (TQValueList<TQString>&)l;
}

TQ_EXPORT TQDataStream &operator<<( TQDataStream & s, const TQStringList& l )
{
    return s << (const TQValueList<TQString>&)l;
}
#endif

/*!
    Converts from an ASCII-TQStrList \a ascii to a TQStringList (Unicode).
*/
TQStringList TQStringList::fromStrList(const TQStrList& ascii)
{
    TQStringList res;
    const char * s;
    for ( TQStrListIterator it(ascii); (s=it.current()); ++it )
	res << s;
    return res;
}

/*! \fn void TQStringList::detach()
    \reimp
*/


#endif //TQT_NO_STRINGLIST
