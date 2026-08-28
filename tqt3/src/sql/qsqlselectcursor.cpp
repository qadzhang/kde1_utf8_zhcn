/****************************************************************************
**
** Definition of TQSqlSelectCursor class
**
** Created : 2002-11-13
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqsqlselectcursor.h"
#include "ntqsqldriver.h"

#ifndef TQT_NO_SQL

class TQSqlSelectCursorPrivate
{
public:
    TQSqlSelectCursorPrivate() : populated( false ) {}
    TQString query;
    bool populated : 1;
};

/*!
    \class TQSqlSelectCursor ntqsqlselectcursor.h
    \brief The TQSqlSelectCursor class provides browsing of general SQL
    SELECT statements.

    \ingroup database
    \module sql

    TQSqlSelectCursor is a convenience class that makes it possible to
    display result sets from general SQL \c SELECT statements in
    data-aware TQt widgets. TQSqlSelectCursor is read-only and does not
    support \c INSERT, \c UPDATE or \c DELETE operations.

    Pass the query in at construction time, or use the
    TQSqlSelectCursor::exec() function.

    Example:
    \code
    ...
    TQSqlSelectCursor* cur = new TQSqlSelectCursor( "SELECT id, firstname, lastname FROM author" );
    TQDataTable* table = new TQDataTable( this );
    table->setSqlCursor( cur, true, true );
    table->refresh();
    ...
    cur->exec( "SELECT * FROM books" );
    table->refresh();
    ...
    \endcode
*/

/*!
    Constructs a read only cursor on database \a db using the query \a query.
 */
TQSqlSelectCursor::TQSqlSelectCursor( const TQString& query, TQSqlDatabase* db )
    : TQSqlCursor( TQString::null, false, db )
{
    d = new TQSqlSelectCursorPrivate;
    d->query = query;
    TQSqlCursor::setMode( ReadOnly );
    if ( !query.isNull() )
	exec( query );
}

/*! Constructs a copy of \a other */
TQSqlSelectCursor::TQSqlSelectCursor( const TQSqlSelectCursor& other )
    : TQSqlCursor( other )
{
    d = new TQSqlSelectCursorPrivate;
    d->query = other.d->query;
    d->populated = other.d->populated;
}

/*! Destroys the object and frees any allocated resources */
TQSqlSelectCursor::~TQSqlSelectCursor()
{
    delete d;
}

/*! \reimp */
bool TQSqlSelectCursor::exec( const TQString& query )
{
    d->query = query;
    bool ret = TQSqlCursor::exec( query );
    if ( ret ) {
	TQSqlCursor::clear();
	populateCursor();
    }
    return ret;
}

/*! \fn bool TQSqlSelectCursor::select()
    \reimp
*/

/*! \reimp */
bool TQSqlSelectCursor::select( const TQString&, const TQSqlIndex& )
{
    bool ret = TQSqlCursor::exec( d->query );
    if ( ret && !d->populated )
	populateCursor();
    return ret;
}

/*! \internal */
void TQSqlSelectCursor::populateCursor()
{
    TQSqlRecordInfo inf = driver()->recordInfo( *(TQSqlQuery*)this );
    for ( TQSqlRecordInfo::const_iterator it = inf.begin(); it != inf.end(); ++it )
	TQSqlCursor::append( *it );
    d->populated = true;
}

/*! \fn TQSqlIndex TQSqlSelectCursor::primaryIndex( bool ) const
    \reimp
*/

/*! \fn TQSqlIndex TQSqlSelectCursor::index( const TQStringList& ) const
    \reimp
*/

/*! \fn TQSqlIndex TQSqlSelectCursor::index( const TQString& ) const
    \reimp
*/

/*! \fn TQSqlIndex TQSqlSelectCursor::index( const char* ) const
    \reimp
*/

/*! \fn void TQSqlSelectCursor::setPrimaryIndex( const TQSqlIndex& )
    \reimp
*/

/*! \fn void TQSqlSelectCursor::append( const TQSqlFieldInfo& )
    \reimp
*/

/*! \fn void TQSqlSelectCursor::insert( int, const TQSqlFieldInfo& )
    \reimp
*/

/*! \fn void TQSqlSelectCursor::remove( int )
    \reimp
*/

/*! \fn void TQSqlSelectCursor::clear()
    \reimp
*/

/*! \fn void TQSqlSelectCursor::setGenerated( const TQString&, bool )
    \reimp
*/

/*! \fn void TQSqlSelectCursor::setGenerated( int, bool )
    \reimp
*/

/*! \fn TQSqlRecord* TQSqlSelectCursor::editBuffer( bool )
    \reimp
*/

/*! \fn TQSqlRecord* TQSqlSelectCursor::primeInsert()
    \reimp
*/

/*! \fn TQSqlRecord* TQSqlSelectCursor::primeUpdate()
    \reimp
*/

/*! \fn TQSqlRecord* TQSqlSelectCursor::primeDelete()
    \reimp
*/

/*! \fn int TQSqlSelectCursor::insert( bool )
    \reimp
*/

/*! \fn int TQSqlSelectCursor::update( bool )
    \reimp
*/

/*! \fn int TQSqlSelectCursor::del( bool )
    \reimp
*/

/*! \fn void TQSqlSelectCursor::setMode( int )
    \reimp
*/

/*! \fn void TQSqlSelectCursor::setSort( const TQSqlIndex& )
    \reimp
*/

/*! \fn TQSqlIndex TQSqlSelectCursor::sort() const
    \reimp
*/

/*! \fn void TQSqlSelectCursor::setFilter( const TQString& )
    \reimp
*/

/*! \fn TQString TQSqlSelectCursor::filter() const
    \reimp
*/

/*! \fn void TQSqlSelectCursor::setName( const TQString&, bool )
    \reimp
*/

/*! \fn TQString TQSqlSelectCursor::name() const
    \reimp
*/

/*! \fn TQString TQSqlSelectCursor::toString( const TQString&, const TQString& ) const
    \reimp
*/
#endif // TQT_NO_SQL
