/****************************************************************************
**
** Implementation of TQSqlQuery class
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

#include "ntqsqlquery.h"

#ifndef TQT_NO_SQL

//#define QT_DEBUG_SQL

#include "ntqsqlresult.h"
#include "ntqsqldriver.h"
#include "ntqsqldatabase.h"
#include "ntqsql.h"
#include "ntqregexp.h"
#include "private/qsqlextension_p.h"


/*!
\internal
*/
TQSqlResultShared::TQSqlResultShared( TQSqlResult* result ): sqlResult(result)
{
    if ( result )
	connect( result->driver(), TQ_SIGNAL(destroyed()), this, TQ_SLOT(slotResultDestroyed()) );
}

/*!
\internal
*/
TQSqlResultShared::~TQSqlResultShared()
{
    delete sqlResult;
}

/*!
\internal

In case a plugin gets unloaded the pointer to the sqlResult gets invalid
*/
void TQSqlResultShared::slotResultDestroyed()
{
    delete sqlResult;
    sqlResult = 0;
}

/*!
    \class TQSqlQuery ntqsqlquery.h
    \brief The TQSqlQuery class provides a means of executing and
    manipulating SQL statements.

    \ingroup database
    \mainclass
    \module sql

    TQSqlQuery encapsulates the functionality involved in creating,
    navigating and retrieving data from SQL queries which are executed
    on a \l TQSqlDatabase. It can be used to execute DML (data
    manipulation language) statements, e.g. \c SELECT, \c INSERT, \c
    UPDATE and \c DELETE, and also DDL (data definition language)
    statements, e.g. \c{CREATE TABLE}. It can also be used to
    execute database-specific commands which are not standard SQL
    (e.g. \c{SET DATESTYLE=ISO} for PostgreSQL).

    Successfully executed SQL statements set the query's state to
    active (isActive() returns true); otherwise the query's state is
    set to inactive. In either case, when executing a new SQL
    statement, the query is positioned on an invalid record; an active
    query must be navigated to a valid record (so that isValid()
    returns true) before values can be retrieved.

    Navigating records is performed with the following functions:

    \list
    \i \c next()
    \i \c prev()
    \i \c first()
    \i \c last()
    \i \c \link TQSqlQuery::seek() seek\endlink(int)
    \endlist

    These functions allow the programmer to move forward, backward or
    arbitrarily through the records returned by the query. If you only
    need to move forward through the results, e.g. using next() or
    using seek() with a positive offset, you can use setForwardOnly()
    and save a significant amount of memory overhead. Once an active
    query is positioned on a valid record, data can be retrieved using
    value(). All data is transferred from the SQL backend using
    TQVariants.

    For example:

    \code
    TQSqlQuery query( "SELECT name FROM customer" );
    while ( query.next() ) {
	TQString name = query.value(0).toString();
	doSomething( name );
    }
    \endcode

    To access the data returned by a query, use the value() method.
    Each field in the data returned by a SELECT statement is accessed
    by passing the field's position in the statement, starting from 0.
    Information about the fields can be obtained via TQSqlDatabase::record().
    For the sake of efficiency there are no functions to access a field
    by name. (The \l TQSqlCursor class provides a higher-level interface
    with field access by name and automatic SQL generation.)

    TQSqlQuery supports prepared query execution and the binding of
    parameter values to placeholders. Some databases don't support
    these features, so for them TQt emulates the required
    functionality. For example, the Oracle and ODBC drivers have
    proper prepared query support, and TQt makes use of it; but for
    databases that don't have this support, TQt implements the feature
    itself, e.g. by replacing placeholders with actual values when a
    query is executed. The exception is positional binding using named
    placeholders, which requires that the database supports prepared
    queries.

    Oracle databases identify placeholders by using a colon-name
    syntax, e.g \c{:name}. ODBC simply uses \c ? characters. TQt
    supports both syntaxes (although you can't mix them in the same
    query).

    Below we present the same example using each of the four different
    binding approaches.

    <b>Named binding using named placeholders</b>
    \code
    TQSqlQuery query;
    query.prepare( "INSERT INTO atable (id, forename, surname) "
	           "VALUES (:id, :forename, :surname)" );
    query.bindValue( ":id", 1001 );
    query.bindValue( ":forename", "Bart" );
    query.bindValue( ":surname", "Simpson" );
    query.exec();
    \endcode

    <b>Positional binding using named placeholders</b>
    \code
    TQSqlQuery query;
    query.prepare( "INSERT INTO atable (id, forename, surname) "
		   "VALUES (:id, :forename, :surname)" );
    query.bindValue( 0, 1001 );
    query.bindValue( 1, "Bart" );
    query.bindValue( 2, "Simpson" );
    query.exec();
    \endcode
    <b>Note:</b> Using positional binding with named placeholders will
    only work if the database supports prepared queries. This can be
    checked with TQSqlDriver::hasFeature() using TQSqlDriver::PreparedQueries
    as argument for driver feature.

    <b>Binding values using positional placeholders #1</b>
    \code
    TQSqlQuery query;
    query.prepare( "INSERT INTO atable (id, forename, surname) "
		   "VALUES (?, ?, ?)" );
    query.bindValue( 0, 1001 );
    query.bindValue( 1, "Bart" );
    query.bindValue( 2, "Simpson" );
    query.exec();
    \endcode

    <b>Binding values using positional placeholders #2</b>
    \code
    query.prepare( "INSERT INTO atable (id, forename, surname) "
		   "VALUES (?, ?, ?)" );
    query.addBindValue( 1001 );
    query.addBindValue( "Bart" );
    query.addBindValue( "Simpson" );
    query.exec();
    \endcode

    <b>Binding values to a stored procedure</b>
    This code calls a stored procedure called \c AsciiToInt(), passing
    it a character through its in parameter, and taking its result in
    the out parameter.
    \code
    TQSqlQuery query;
    query.prepare( "call AsciiToInt(?, ?)" );
    query.bindValue( 0, "A" );
    query.bindValue( 1, 0, TQSql::Out );
    query.exec();
    int i = query.boundValue( 1 ).toInt(); // i is 65.
    \endcode

    \sa TQSqlDatabase TQSqlCursor TQVariant
*/

/*!
    Creates a TQSqlQuery object which uses the TQSqlResult \a r to
    communicate with a database.
*/

TQSqlQuery::TQSqlQuery( TQSqlResult * r )
{
    d = new TQSqlResultShared( r );
}

/*!
    Destroys the object and frees any allocated resources.
*/

TQSqlQuery::~TQSqlQuery()
{
    if (d->deref()) {
	delete d;
    }
}

/*!
    Constructs a copy of \a other.
*/

TQSqlQuery::TQSqlQuery( const TQSqlQuery& other )
    : d(other.d)
{
    d->ref();
}

/*!
    Creates a TQSqlQuery object using the SQL \a query and the database
    \a db. If \a db is 0, (the default), the application's default
    database is used. If \a query is not a null string, it will be
    executed.

    \sa TQSqlDatabase
*/
TQSqlQuery::TQSqlQuery( const TQString& query, TQSqlDatabase* db )
{
    init( query, db );
}

/*!
    Creates a TQSqlQuery object using the database \a db. If \a db is
    0, the application's default database is used.

    \sa TQSqlDatabase
*/

TQSqlQuery::TQSqlQuery( TQSqlDatabase* db )
{
    init( TQString::null, db );
}

/*! \internal
*/

void TQSqlQuery::init( const TQString& query, TQSqlDatabase* db )
{
    d = new TQSqlResultShared( 0 );
    TQSqlDatabase* database = db;
    if ( !database )
	database = TQSqlDatabase::database( TQSqlDatabase::defaultConnection, false );
    if ( database )
	*this = database->driver()->createQuery();
    if ( !query.isNull() )
	exec( query );
}

/*!
    Assigns \a other to the query.
*/

TQSqlQuery& TQSqlQuery::operator=( const TQSqlQuery& other )
{
    other.d->ref();
    deref();
    d = other.d;
    return *this;
}

/*!
    Returns true if the query is active and positioned on a valid
    record and the \a field is NULL; otherwise returns false. Note
    that for some drivers isNull() will not return accurate
    information until after an attempt is made to retrieve data.

    \sa isActive() isValid() value()
*/

bool TQSqlQuery::isNull( int field ) const
{
    if ( !d->sqlResult )
	return false;
    if ( d->sqlResult->isActive() && d->sqlResult->isValid() )
	return d->sqlResult->isNull( field );
    return false;
}

/*!
    Executes the SQL in \a query. Returns true and sets the query
    state to active if the query was successful; otherwise returns
    false and sets the query state to inactive. The \a query string
    must use syntax appropriate for the SQL database being queried,
    for example, standard SQL.

    After the query is executed, the query is positioned on an \e
    invalid record, and must be navigated to a valid record before
    data values can be retrieved, e.g. using next().

    Note that the last error for this query is reset when exec() is
    called.

    \sa isActive() isValid() next() prev() first() last() seek()
*/

bool TQSqlQuery::exec ( const TQString& query )
{
    if ( !d->sqlResult )
	return false;
    if ( d->sqlResult->extension() && driver()->hasFeature( TQSqlDriver::PreparedQueries ) )
	d->sqlResult->extension()->clear();
    d->sqlResult->setActive( false );
    d->sqlResult->setLastError( TQSqlError() );
    d->sqlResult->setAt( TQSql::BeforeFirst );
    if ( !driver() ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQSqlQuery::exec: no driver" );
#endif
	return false;
    }
    if ( d->count > 1 )
	*this = driver()->createQuery();
    d->sqlResult->setQuery( query.stripWhiteSpace() );
    d->executedQuery = d->sqlResult->lastQuery();
    if ( !driver()->isOpen() || driver()->isOpenError() ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQSqlQuery::exec: database not open" );
#endif
	return false;
    }
    if ( query.isNull() || query.length() == 0 ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQSqlQuery::exec: empty query" );
#endif
	return false;
    }
#ifdef QT_DEBUG_SQL
    tqDebug( "\n TQSqlQuery: " + query );
#endif
    return d->sqlResult->reset( query );
}

/*!
    Returns the value of the \a{i}-th field in the query (zero based).

    The fields are numbered from left to right using the text of the
    \c SELECT statement, e.g. in \c{SELECT forename, surname FROM people},
    field 0 is \c forename and field 1 is \c surname. Using \c{SELECT *}
    is not recommended because the order of the fields in the query is
    undefined.

    An invalid TQVariant is returned if field \a i does not exist, if
    the query is inactive, or if the query is positioned on an invalid
    record.

    \sa prev() next() first() last() seek() isActive() isValid()
*/

TQVariant TQSqlQuery::value( int i ) const
{
    if ( !d->sqlResult )
	return TQVariant();
    if ( isActive() && isValid() && ( i > TQSql::BeforeFirst ) ) {
	return d->sqlResult->data( i );
    } else {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQSqlQuery::value: not positioned on a valid record" );
#endif
    }
    return TQVariant();
}

/*!
    Returns the current internal position of the query. The first
    record is at position zero. If the position is invalid, a
    TQSql::Location will be returned indicating the invalid position.

    \sa prev() next() first() last() seek() isActive() isValid()
*/

int TQSqlQuery::at() const
{
    if ( !d->sqlResult )
	return TQSql::BeforeFirst;
    return d->sqlResult->at();
}

/*!
    Returns the text of the current query being used, or TQString::null
    if there is no current query text.

    \sa executedQuery()
*/

TQString TQSqlQuery::lastQuery() const
{
    if ( !d->sqlResult )
	return TQString::null;
    return d->sqlResult->lastQuery();
}

/*!
    Returns the database driver associated with the query.
*/

const TQSqlDriver* TQSqlQuery::driver() const
{
    if ( !d->sqlResult )
	return 0;
    return d->sqlResult->driver();
}

/*!
    Returns the result associated with the query.
*/

const TQSqlResult* TQSqlQuery::result() const
{
    return d->sqlResult;
}

/*!
    Retrieves the record at position (offset) \a i, if available, and
    positions the query on the retrieved record. The first record is
    at position 0. Note that the query must be in an active state and
    isSelect() must return true before calling this function.

    If \a relative is false (the default), the following rules apply:

    \list
    \i If \a i is negative, the result is positioned before the
    first record and false is returned.
    \i Otherwise, an attempt is made to move to the record at position
    \a i. If the record at position \a i could not be retrieved, the
    result is positioned after the last record and false is returned. If
    the record is successfully retrieved, true is returned.
    \endlist

    If \a relative is true, the following rules apply:

    \list
    \i If the result is currently positioned before the first
    record or on the first record, and \a i is negative, there is no
    change, and false is returned.
    \i If the result is currently located after the last record, and
    \a i is positive, there is no change, and false is returned.
    \i If the result is currently located somewhere in the middle,
    and the relative offset \a i moves the result below zero, the
    result is positioned before the first record and false is
    returned.
    \i Otherwise, an attempt is made to move to the record \a i
    records ahead of the current record (or \a i records behind the
    current record if \a i is negative). If the record at offset \a i
    could not be retrieved, the result is positioned after the last
    record if \a i >= 0, (or before the first record if \a i is
    negative), and false is returned. If the record is successfully
    retrieved, true is returned.
    \endlist

    \sa next() prev() first() last() at() isActive() isValid()
*/
bool TQSqlQuery::seek( int i, bool relative )
{
    if ( !isSelect() || !isActive() )
	return false;
    beforeSeek();
    checkDetach();
    int actualIdx;
    if ( !relative ) { // arbitrary seek
	if ( i < 0 ) {
	    d->sqlResult->setAt( TQSql::BeforeFirst );
	    afterSeek();
	    return false;
	}
	actualIdx = i;
    } else {
	switch ( at() ) { // relative seek
	case TQSql::BeforeFirst:
	    if ( i > 0 )
		actualIdx = i;
	    else {
		afterSeek();
		return false;
	    }
	    break;
	case TQSql::AfterLast:
	    if ( i < 0 ) {
		d->sqlResult->fetchLast();
		actualIdx = at() + i;
	    } else {
		afterSeek();
		return false;
	    }
	    break;
	default:
	    if ( ( at() + i ) < 0  ) {
		d->sqlResult->setAt( TQSql::BeforeFirst );
		afterSeek();
		return false;
	    }
	    actualIdx = at() + i;
	    break;
	}
    }
    // let drivers optimize
    if ( isForwardOnly() && actualIdx < at() ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQSqlQuery::seek: cannot seek backwards in a forward only query" );
#endif
	afterSeek();
	return false;
    }
    if ( actualIdx == ( at() + 1 ) && at() != TQSql::BeforeFirst ) {
	if ( !d->sqlResult->fetchNext() ) {
	    d->sqlResult->setAt( TQSql::AfterLast );
	    afterSeek();
	    return false;
	}
	afterSeek();
	return true;
    }
    if ( actualIdx == ( at() - 1 ) ) {
	if ( !d->sqlResult->fetchPrev() ) {
	    d->sqlResult->setAt( TQSql::BeforeFirst );
	    afterSeek();
	    return false;
	}
	afterSeek();
	return true;
    }
    if ( !d->sqlResult->fetch( actualIdx ) ) {
	d->sqlResult->setAt( TQSql::AfterLast );
	afterSeek();
	return false;
    }
    afterSeek();
    return true;
}

/*!
    Retrieves the next record in the result, if available, and
    positions the query on the retrieved record. Note that the result
    must be in an active state and isSelect() must return true before
    calling this function or it will do nothing and return false.

    The following rules apply:

    \list
    \i If the result is currently located before the first
    record, e.g. immediately after a query is executed, an attempt is
    made to retrieve the first record.

    \i If the result is currently located after the last record,
    there is no change and false is returned.

    \i If the result is located somewhere in the middle, an attempt
    is made to retrieve the next record.
    \endlist

    If the record could not be retrieved, the result is positioned after
    the last record and false is returned. If the record is successfully
    retrieved, true is returned.

    \sa prev() first() last() seek() at() isActive() isValid()
*/

bool TQSqlQuery::next()
{
    if ( !isSelect() || !isActive() )
	return false;
    beforeSeek();
    checkDetach();
    bool b = false;
    switch ( at() ) {
    case TQSql::BeforeFirst:
	b = d->sqlResult->fetchFirst();
	afterSeek();
	return b;
    case TQSql::AfterLast:
	afterSeek();
	return false;
    default:
	if ( !d->sqlResult->fetchNext() ) {
	    d->sqlResult->setAt( TQSql::AfterLast );
	    afterSeek();
	    return false;
	}
	afterSeek();
	return true;
    }
}

/*!
    Retrieves the previous record in the result, if available, and
    positions the query on the retrieved record. Note that the result
    must be in an active state and isSelect() must return true before
    calling this function or it will do nothing and return false.

    The following rules apply:

    \list
    \i If the result is currently located before the first record,
    there is no change and false is returned.

    \i If the result is currently located after the last record, an
    attempt is made to retrieve the last record.

    \i If the result is somewhere in the middle, an attempt is made
    to retrieve the previous record.
    \endlist

    If the record could not be retrieved, the result is positioned
    before the first record and false is returned. If the record is
    successfully retrieved, true is returned.

    \sa next() first() last() seek() at() isActive() isValid()
*/

bool TQSqlQuery::prev()
{
    if ( !isSelect() || !isActive() )
	return false;
    if ( isForwardOnly() ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQSqlQuery::seek: cannot seek backwards in a forward only query" );
#endif
	return false;
    }

    beforeSeek();
    checkDetach();
    bool b = false;
    switch ( at() ) {
    case TQSql::BeforeFirst:
	afterSeek();
	return false;
    case TQSql::AfterLast:
	b = d->sqlResult->fetchLast();
	afterSeek();
	return b;
    default:
	if ( !d->sqlResult->fetchPrev() ) {
	    d->sqlResult->setAt( TQSql::BeforeFirst );
	    afterSeek();
	    return false;
	}
	afterSeek();
	return true;
    }
}

/*!
    Retrieves the first record in the result, if available, and
    positions the query on the retrieved record. Note that the result
    must be in an active state and isSelect() must return true before
    calling this function or it will do nothing and return false.
    Returns true if successful. If unsuccessful the query position is
    set to an invalid position and false is returned.

    \sa next() prev() last() seek() at() isActive() isValid()
*/

bool TQSqlQuery::first()
{
    if ( !isSelect() || !isActive() )
	return false;
    if ( isForwardOnly() && at() > TQSql::BeforeFirst ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQSqlQuery::seek: cannot seek backwards in a forward only query" );
#endif
	return false;
    }
    beforeSeek();
    checkDetach();
    bool b = false;
    b = d->sqlResult->fetchFirst();
    afterSeek();
    return b;
}

/*!
    Retrieves the last record in the result, if available, and
    positions the query on the retrieved record. Note that the result
    must be in an active state and isSelect() must return true before
    calling this function or it will do nothing and return false.
    Returns true if successful. If unsuccessful the query position is
    set to an invalid position and false is returned.

    \sa next() prev() first() seek() at() isActive() isValid()
*/

bool TQSqlQuery::last()
{
    if ( !isSelect() || !isActive() )
	return false;
    beforeSeek();
    checkDetach();
    bool b = false;
    b = d->sqlResult->fetchLast();
    afterSeek();
    return b;
}

/*!
    Returns the size of the result, (number of rows returned), or -1
    if the size cannot be determined or if the database does not
    support reporting information about query sizes. Note that for
    non-\c SELECT statements (isSelect() returns false), size() will
    return -1. If the query is not active (isActive() returns false),
    -1 is returned.

    To determine the number of rows affected by a non-SELECT
    statement, use numRowsAffected().

    \sa isActive() numRowsAffected() TQSqlDriver::hasFeature()
*/
int TQSqlQuery::size() const
{
    if ( !d->sqlResult )
	return -1;
    if ( isActive() && d->sqlResult->driver()->hasFeature( TQSqlDriver::QuerySize ) )
	return d->sqlResult->size();
    return -1;
}

/*!
    Returns the number of rows affected by the result's SQL statement,
    or -1 if it cannot be determined. Note that for \c SELECT
    statements, the value is undefined; see size() instead. If the
    query is not active (isActive() returns false), -1 is returned.

    \sa size() TQSqlDriver::hasFeature()
*/

int TQSqlQuery::numRowsAffected() const
{
    if ( !d->sqlResult )
	return -1;
    if ( isActive() )
	return d->sqlResult->numRowsAffected();
    return -1;
}

/*!
    Returns error information about the last error (if any) that
    occurred.

    \sa TQSqlError
*/

TQSqlError TQSqlQuery::lastError() const
{
    if ( !d->sqlResult )
	return TQSqlError();
    return d->sqlResult->lastError();
}

/*!
    Returns true if the query is currently positioned on a valid
    record; otherwise returns false.
*/

bool TQSqlQuery::isValid() const
{
    if ( !d->sqlResult )
	return false;
    return d->sqlResult->isValid();
}

/*!
    Returns true if the query is currently active; otherwise returns
    false.
*/

bool TQSqlQuery::isActive() const
{
    if ( !d->sqlResult )
	return false;
    return d->sqlResult->isActive();
}

/*!
    Returns true if the current query is a \c SELECT statement;
    otherwise returns false.
*/

bool TQSqlQuery::isSelect() const
{
    if ( !d->sqlResult )
	return false;
    return d->sqlResult->isSelect();
}

/*!
    Returns true if you can only scroll \e forward through a result
    set; otherwise returns false.

    \sa setForwardOnly()
*/
bool TQSqlQuery::isForwardOnly() const
{
    if ( !d->sqlResult )
	return false;
    return d->sqlResult->isForwardOnly();
}

/*!
    Sets forward only mode to \a forward. If forward is true only
    next(), and seek() with positive values, are allowed for
    navigating the results. Forward only mode needs far less memory
    since results do not need to be cached.

    Forward only mode is off by default.

    Forward only mode cannot be used with data aware widgets like
    TQDataTable, since they must to be able to scroll backward as well
    as forward.

    \sa isForwardOnly(), next(), seek()
*/
void TQSqlQuery::setForwardOnly( bool forward )
{
    if ( d->sqlResult )
	d->sqlResult->setForwardOnly( forward );
}

/*!
  \internal
*/

void TQSqlQuery::deref()
{
    if ( d->deref() ) {
	delete d;
	d = 0;
    }
}

/*!
  \internal
*/

bool TQSqlQuery::checkDetach()
{
    if ( d->count > 1 && d->sqlResult ) {
	TQString sql = d->sqlResult->lastQuery();
	*this = driver()->createQuery();
	exec( sql );
	return true;
    }
    return false;
}


/*!
    Protected virtual function called before the internal record
    pointer is moved to a new record. The default implementation does
    nothing.
*/

void TQSqlQuery::beforeSeek()
{

}


/*!
    Protected virtual function called after the internal record
    pointer is moved to a new record. The default implementation does
    nothing.
*/

void TQSqlQuery::afterSeek()
{

}

// XXX: Hack to keep BCI - remove in 4.0. TQSqlExtension should be
// removed, and the prepare(), exec() etc. fu's should be
// made virtual members of TQSqlQuery/TQSqlResult

/*!
    Prepares the SQL query \a query for execution. The query may
    contain placeholders for binding values. Both Oracle style
    colon-name (e.g. \c{:surname}), and ODBC style (e.g. \c{?})
    placeholders are supported; but they cannot be mixed in the same
    query. See the \link #details Description\endlink for examples.

    \sa exec(), bindValue(), addBindValue()
*/
bool TQSqlQuery::prepare( const TQString& query )
{
    if ( !d->sqlResult || !d->sqlResult->extension() )
	return false;
    d->sqlResult->setActive( false );
    d->sqlResult->setLastError( TQSqlError() );
    d->sqlResult->setAt( TQSql::BeforeFirst );
    d->sqlResult->extension()->clear();
    if ( !driver() ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQSqlQuery::prepare: no driver" );
#endif
	return false;
    }
    if ( d->count > 1 )
	*this = driver()->createQuery();
    d->sqlResult->setQuery( query.stripWhiteSpace() );
    if ( !driver()->isOpen() || driver()->isOpenError() ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQSqlQuery::prepare: database not open" );
#endif
	return false;
    }
    if ( query.isNull() || query.length() == 0 ) {
#ifdef QT_CHECK_RANGE
	tqWarning("TQSqlQuery::prepare: empty query" );
#endif
	return false;
    }
#ifdef QT_DEBUG_SQL
    tqDebug( "\n TQSqlQuery: " + query );
#endif
    TQString q = query;
    TQRegExp rx(TQString::fromLatin1("'[^']*'|:([a-zA-Z0-9_]+)"));
    if ( driver()->hasFeature( TQSqlDriver::PreparedQueries ) ) {
	// below we substitute Oracle placeholders with ODBC ones and
	// vice versa to make this db independent
	int i = 0, cnt = 0;
	if ( driver()->hasFeature( TQSqlDriver::NamedPlaceholders ) ) {
	    TQRegExp rx(TQString::fromLatin1("'[^']*'|\\?"));
	    while ( (i = rx.search( q, i )) != -1 ) {
		if ( rx.cap(0) == "?" ) {
		    q = q.replace( i, 1, ":f" + TQString::number(cnt) );
		    cnt++;
		}
		i += rx.matchedLength();
	    }
	} else if ( driver()->hasFeature( TQSqlDriver::PositionalPlaceholders ) ) {
	    while ( (i = rx.search( q, i )) != -1 ) {
		if ( rx.cap(1).isEmpty() ) {
		    i += rx.matchedLength();
		} else {
		    // record the index of the placeholder - needed
		    // for emulating named bindings with ODBC
		    d->sqlResult->extension()->index[ cnt ]= rx.cap(0);
		    q = q.replace( i, rx.matchedLength(), "?" );
		    i++;
		    cnt++;
		}
	    }
	}
	d->executedQuery = q;
	return d->sqlResult->extension()->prepare( q );
    } else {
	int i = 0;
	while ( (i = rx.search( q, i )) != -1 ) {
	    if ( !rx.cap(1).isEmpty() )
		d->sqlResult->extension()->holders.append( Holder( rx.cap(0), i ) );
	    i += rx.matchedLength();
	}
	return true; // fake prepares should always succeed
    }
}

/*!
    \overload

    Executes a previously prepared SQL query. Returns true if the
    query executed successfully; otherwise returns false.

    \sa prepare(), bindValue(), addBindValue()
*/
bool TQSqlQuery::exec()
{
    bool ret;
    if ( !d->sqlResult || !d->sqlResult->extension() )
	return false;
    if ( driver()->hasFeature( TQSqlDriver::PreparedQueries ) ) {
	ret = d->sqlResult->extension()->exec();
    } else {
	// fake preparation - just replace the placeholders..
	TQString query = d->sqlResult->lastQuery();
	if ( d->sqlResult->extension()->bindMethod() == TQSqlExtension::BindByName ) {
	    int i;
	    TQVariant val;
	    TQString holder;
	    for ( i = (int)d->sqlResult->extension()->holders.count() - 1; i >= 0; --i ) {
		holder = d->sqlResult->extension()->holders[ (uint)i ].holderName;
		val = d->sqlResult->extension()->values[ holder ].value;
		TQSqlField f( "", val.type() );
		if ( val.isNull() )
		    f.setNull();
		else
		    f.setValue( val );
		query = query.replace( (uint)d->sqlResult->extension()->holders[ (uint)i ].holderPos,
			holder.length(), driver()->formatValue( &f ) );
	    }
	} else {
	    TQMap<int, TQString>::ConstIterator it;
	    TQString val;
	    int i = 0;
	    for ( it = d->sqlResult->extension()->index.begin();
		  it != d->sqlResult->extension()->index.end(); ++it ) {
		i = query.find( '?', i );
		if ( i > -1 ) {
		    TQSqlField f( "", d->sqlResult->extension()->values[ it.data() ].value.type() );
		    if ( d->sqlResult->extension()->values[ it.data() ].value.isNull() )
			f.setNull();
		    else
			f.setValue( d->sqlResult->extension()->values[ it.data() ].value );
		    val = driver()->formatValue( &f );
		    query = query.replace( i, 1, driver()->formatValue( &f ) );
		    i += val.length();
		}
	    }
	}
	// have to retain the original query w/placeholders..
	TQString orig = d->sqlResult->lastQuery();
	ret = exec( query );
	d->executedQuery = query;
	d->sqlResult->setQuery( orig );
    }
    d->sqlResult->extension()->resetBindCount();
    return ret;
}

/*!
    Set the placeholder \a placeholder to be bound to value \a val in
    the prepared statement. Note that the placeholder mark (e.g \c{:})
    must be included when specifying the placeholder name. If \a type
    is \c TQSql::Out or \c TQSql::InOut, the placeholder will be
    overwritten with data from the database after the exec() call.

    \sa addBindValue(), prepare(), exec()
*/
void TQSqlQuery::bindValue( const TQString& placeholder, const TQVariant& val, TQSql::ParameterType type )
{
    if ( !d->sqlResult || !d->sqlResult->extension() )
	return;
    d->sqlResult->extension()->bindValue( placeholder, val, type );
}

/*!
    \overload

    Set the placeholder in position \a pos to be bound to value \a val
    in the prepared statement. Field numbering starts at 0. If \a type
    is \c TQSql::Out or \c TQSql::InOut, the placeholder will be
    overwritten with data from the database after the exec() call.

    \sa addBindValue(), prepare(), exec()
*/
void TQSqlQuery::bindValue( int pos, const TQVariant& val, TQSql::ParameterType type )
{
    if ( !d->sqlResult || !d->sqlResult->extension() )
	return;
    d->sqlResult->extension()->bindValue( pos, val, type );
}

/*!
    Adds the value \a val to the list of values when using positional
    value binding. The order of the addBindValue() calls determines
    which placeholder a value will be bound to in the prepared query.
    If \a type is \c TQSql::Out or \c TQSql::InOut, the placeholder will
    be overwritten with data from the database after the exec() call.

    \sa bindValue(), prepare(), exec()
*/
void TQSqlQuery::addBindValue( const TQVariant& val, TQSql::ParameterType type )
{
    if ( !d->sqlResult || !d->sqlResult->extension() )
	return;
    d->sqlResult->extension()->addBindValue( val, type );
}


/*!
    \overload

    Binds the placeholder with type \c TQSql::In.
*/
void TQSqlQuery::bindValue( const TQString& placeholder, const TQVariant& val )
{
    bindValue( placeholder, val, TQSql::In );
}

/*!
    \overload

    Binds the placeholder at position \a pos with type \c TQSql::In.
*/
void TQSqlQuery::bindValue( int pos, const TQVariant& val )
{
    bindValue( pos, val, TQSql::In );
}

/*!
    \overload

    Binds the placeholder with type \c TQSql::In.
*/
void TQSqlQuery::addBindValue( const TQVariant& val )
{
    addBindValue( val, TQSql::In );
}

/*!
    Returns the value for the \a placeholder.
*/
TQVariant TQSqlQuery::boundValue( const TQString& placeholder ) const
{
    if ( !d->sqlResult || !d->sqlResult->extension() )
	return TQVariant();
    return d->sqlResult->extension()->boundValue( placeholder );
}

/*!
    \overload

    Returns the value for the placeholder at position \a pos.
*/
TQVariant TQSqlQuery::boundValue( int pos ) const
{
    if ( !d->sqlResult || !d->sqlResult->extension() )
	return TQVariant();
    return d->sqlResult->extension()->boundValue( pos );
}

/*!
    Returns a map of the bound values.

    The bound values can be examined in the following way:
    \code
    TQSqlQuery query;
    ...
    // Examine the bound values - bound using named binding
    TQStringVariantMap::ConstIterator it;
    TQStringVariantMap vals = query.boundValues();
    for ( it = vals.begin(); it != vals.end(); ++it )
        tqWarning( "Placeholder: " + it.key() + ", Value: " + (*it).toString() );
    ...

    // Examine the bound values - bound using positional binding
    TQValueList<TQVariant>::ConstIterator it;
    TQValueList<TQVariant> list = query.boundValues().values();
    int i = 0;
    for ( it = list.begin(); it != list.end(); ++it )
        tqWarning( "Placeholder pos: %d, Value: " + (*it).toString(), i++ );
    ...

    \endcode
*/
TQStringVariantMap TQSqlQuery::boundValues() const
{
    if ( !d->sqlResult || !d->sqlResult->extension() )
	return TQStringVariantMap();
    return d->sqlResult->extension()->boundValues();
}

/*!
    Returns the last query that was executed.

    In most cases this function returns the same as lastQuery(). If a
    prepared query with placeholders is executed on a DBMS that does
    not support it, the preparation of this query is emulated. The
    placeholders in the original query are replaced with their bound
    values to form a new query. This function returns the modified
    query. Useful for debugging purposes.

    \sa lastQuery()
*/
TQString TQSqlQuery::executedQuery() const
{
    return d->executedQuery;
}
#endif // TQT_NO_SQL
