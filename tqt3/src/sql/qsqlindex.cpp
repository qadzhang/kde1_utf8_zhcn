/****************************************************************************
**
** Implementation of TQSqlIndex class
**
** Created : 2000-11-03
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the sql module of the TQt GUI Toolkit.
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

#include "ntqsqlindex.h"

#ifndef TQT_NO_SQL

#include "ntqsqlcursor.h"

/*!
    \class TQSqlIndex ntqsqlindex.h
    \brief The TQSqlIndex class provides functions to manipulate and
    describe TQSqlCursor and TQSqlDatabase indexes.

    \ingroup database
    \module sql

    This class is used to describe and manipulate TQSqlCursor and
    TQSqlDatabase indexes. An index refers to a single table or view
    in a database. Information about the fields that comprise the
    index can be used to generate SQL statements, or to affect the
    behavior of a \l TQSqlCursor object.

    Normally, TQSqlIndex objects are created by \l TQSqlDatabase or
    TQSqlCursor.
*/

/*!
    Constructs an empty index using the cursor name \a cursorname and
    index name \a name.
*/

TQSqlIndex::TQSqlIndex( const TQString& cursorname, const TQString& name )
    : TQSqlRecord(), cursor(cursorname), nm(name)
{

}

/*!
    Constructs a copy of \a other.
*/

TQSqlIndex::TQSqlIndex( const TQSqlIndex& other )
    : TQSqlRecord(other), cursor(other.cursor), nm(other.nm), sorts(other.sorts)
{
}

/*!
    Sets the index equal to \a other.
*/

TQSqlIndex& TQSqlIndex::operator=( const TQSqlIndex& other )
{
    cursor = other.cursor;
    nm = other.nm;
    sorts = other.sorts;
    TQSqlRecord::operator=( other );
    return *this;
}

/*!
    Destroys the object and frees any allocated resources.
*/

TQSqlIndex::~TQSqlIndex()
{

}

/*!
    Sets the name of the index to \a name.
*/

void TQSqlIndex::setName( const TQString& name )
{
    nm = name;
}

/*!
    \fn TQString TQSqlIndex::name() const

    Returns the name of the index.
*/

/*!
    Appends the field \a field to the list of indexed fields. The
    field is appended with an ascending sort order.
*/

void TQSqlIndex::append( const TQSqlField& field )
{
    append( field, false );
}

/*!
    \overload

    Appends the field \a field to the list of indexed fields. The
    field is appended with an ascending sort order, unless \a desc is
    true.
*/

void TQSqlIndex::append( const TQSqlField& field, bool desc )
{
    sorts.append( desc );
    TQSqlRecord::append( field );
}


/*!
    Returns true if field \a i in the index is sorted in descending
    order; otherwise returns false.
*/

bool TQSqlIndex::isDescending( int i ) const
{
    if ( sorts.at( i ) != sorts.end() )
	return sorts[i];
    return false;
}

/*!
    If \a desc is true, field \a i is sorted in descending order.
    Otherwise, field \a i is sorted in ascending order (the default).
    If the field does not exist, nothing happens.
*/

void TQSqlIndex::setDescending( int i, bool desc )
{
    if ( sorts.at( i ) != sorts.end() )
	sorts[i] = desc;
}

/*!
    \reimp

    Returns a comma-separated list of all the index's field names as a
    string. This string is suitable, for example, for generating a
    SQL SELECT statement. Only generated fields are included in the
    list (see \l{isGenerated()}). If a \a prefix is specified, e.g. a
    table name, it is prepended before all field names in the form:

    "\a{prefix}.<fieldname>"

    If \a sep is specified, each field is separated by \a sep. If \a
    verbose is true (the default), each field contains a suffix
    indicating an ASCending or DESCending sort order.
*/

TQString TQSqlIndex::toString( const TQString& prefix, const TQString& sep, bool verbose ) const
{
    TQString s;
    bool comma = false;
    for ( uint i = 0; i < count(); ++i ) {
	if( comma )
	    s += sep + " ";
	s += createField( i, prefix, verbose );
	comma = true;
    }
    return s;
}

/*!
    \reimp

    Returns a list of all the index's field names. Only generated
    fields are included in the list (see \l{isGenerated()}). If a \a
    prefix is specified, e.g. a table name, all fields are prefixed in
    the form:

    "\a{prefix}.<fieldname>"

    If \a verbose is true (the default), each field contains a suffix
    indicating an ASCending or DESCending sort order.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myIndex.toStringList();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

*/
TQStringList TQSqlIndex::toStringList( const TQString& prefix, bool verbose ) const
{
    TQStringList s;
    for ( uint i = 0; i < count(); ++i )
	s += createField( i, prefix, verbose );
    return s;
}

/*! \internal

  Creates a string representing the field number \a i using prefix \a
  prefix. If \a verbose is true, ASC or DESC is included in the field
  description if the field is sorted in ASCending or DESCending order.
*/

TQString TQSqlIndex::createField( int i, const TQString& prefix, bool verbose ) const
{
    TQString f;
    if ( !prefix.isEmpty() )
	f += prefix + ".";
    f += field( i )->name();
    if ( verbose )
	f += " " + TQString( ( isDescending( i ) ? "DESC" : "ASC" ) );
    return f;
}

/*!
    Returns an index based on the field descriptions in \a l and the
    cursor \a cursor. The field descriptions should be in the same
    format that toStringList() produces, for example, a surname field
    in the people table might be in one of these forms: "surname",
    "surname DESC" or "people.surname ASC".

    \sa toStringList()
*/

TQSqlIndex TQSqlIndex::fromStringList( const TQStringList& l, const TQSqlCursor* cursor )
{
    TQSqlIndex newSort;
    for ( uint i = 0; i < l.count(); ++i ) {
	TQString f = l[ i ];
	bool desc = false;
	if ( f.mid( f.length()-3 ) == "ASC" )
	    f = f.mid( 0, f.length()-3 );
	if ( f.mid( f.length()-4 ) == "DESC" ) {
	    desc = true;
	    f = f.mid( 0, f.length()-4 );
	}
	int dot = f.findRev( '.' );
	if ( dot != -1 )
	    f = f.mid( dot+1 );
	const TQSqlField* field = cursor->field( f.simplifyWhiteSpace() );
	if ( field )
	    newSort.append( *field, desc );
	else
	    tqWarning( "TQSqlIndex::fromStringList: unknown field: '%s'", f.latin1());
    }
    return newSort;
}

/*!
    \fn TQString TQSqlIndex::cursorName() const

    Returns the name of the cursor which the index is associated with.
*/


/*!
    Sets the name of the cursor that the index is associated with to
    \a cursorName.
*/
void TQSqlIndex::setCursorName( const TQString& cursorName )
{
    cursor = cursorName;
}

#endif
