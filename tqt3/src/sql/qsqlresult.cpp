/****************************************************************************
**
** Implementation of TQSqlResult class
**
** Created : 2000-11-03
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

#include "ntqsqlresult.h"
#include "private/qsqlextension_p.h"

#ifndef TQT_NO_SQL

class TQSqlResultPrivate
{
public:
    const TQSqlDriver*   sqldriver;
    int             idx;
    TQString         sql;
    bool            active;
    bool            isSel;
    TQSqlError	    error;
    TQSqlExtension * ext;
};

/*!
    \class TQSqlResult
    \brief The TQSqlResult class provides an abstract interface for
    accessing data from SQL databases.

    \ingroup database
    \module sql

    Normally you would use TQSqlQuery instead of TQSqlResult since TQSqlQuery
    provides a generic wrapper for database-specific implementations of
    TQSqlResult.

    \sa TQSql
*/


/*!
    Protected constructor which creates a TQSqlResult using database \a
    db. The object is initialized to an inactive state.
*/

TQSqlResult::TQSqlResult( const TQSqlDriver * db ): forwardOnly( false )
{
    d = new TQSqlResultPrivate();
    d->sqldriver = db;
    d->idx = TQSql::BeforeFirst;
    d->isSel = false;
    d->active = false;
    d->ext = new TQSqlExtension();
}

/*!
    Destroys the object and frees any allocated resources.
*/

TQSqlResult::~TQSqlResult()
{
    if ( d->ext )
	delete d->ext;
    delete d;
}

/*!
    Sets the current query for the result to \a query. The result must
    be reset() in order to execute the query on the database.
*/

void TQSqlResult::setQuery( const TQString& query )
{
    d->sql = query;
}

/*!
    Returns the current SQL query text, or TQString::null if there is none.
*/

TQString TQSqlResult::lastQuery() const
{
    return d->sql;
}

/*!
    Returns the current (zero-based) position of the result.
*/

int TQSqlResult::at() const
{
    return d->idx;
}


/*!
    Returns true if the result is positioned on a valid record (that
    is, the result is not positioned before the first or after the
    last record); otherwise returns false.
*/

bool TQSqlResult::isValid() const
{
    return (d->idx != TQSql::BeforeFirst && d->idx != TQSql::AfterLast);
}

/*!
    \fn bool TQSqlResult::isNull( int i )

    Returns true if the field at position \a i is NULL; otherwise
    returns false.
*/


/*!
    Returns true if the result has records to be retrieved; otherwise
    returns false.
*/

bool TQSqlResult::isActive() const
{
    return d->active;
}

/*!
    Protected function provided for derived classes to set the
    internal (zero-based) result index to \a at.

    \sa at()
*/

void TQSqlResult::setAt( int at )
{
    d->idx = at;
}


/*!
    Protected function provided for derived classes to indicate
    whether or not the current statement is a SQL SELECT statement.
    The \a s parameter should be true if the statement is a SELECT
    statement, or false otherwise.
*/

void TQSqlResult::setSelect( bool s )
{
    d->isSel = s;
}

/*!
    Returns true if the current result is from a SELECT statement;
    otherwise returns false.
*/

bool TQSqlResult::isSelect() const
{
    return d->isSel;
}

/*!
    Returns the driver associated with the result.
*/

const TQSqlDriver* TQSqlResult::driver() const
{
    return d->sqldriver;
}


/*!
    Protected function provided for derived classes to set the
    internal active state to the value of \a a.

    \sa isActive()
*/

void TQSqlResult::setActive( bool a )
{
    d->active = a;
}

/*!
    Protected function provided for derived classes to set the last
    error to the value of \a e.

    \sa lastError()
*/

void TQSqlResult::setLastError( const TQSqlError& e )
{
    d->error = e;
}


/*!
    Returns the last error associated with the result.
*/

TQSqlError TQSqlResult::lastError() const
{
    return d->error;
}

/*!
    \fn int TQSqlResult::size()

    Returns the size of the result or -1 if it cannot be determined.
*/

/*!
    \fn int TQSqlResult::numRowsAffected()

    Returns the number of rows affected by the last query executed.
*/

/*!
    \fn TQVariant TQSqlResult::data( int i )

    Returns the data for field \a i (zero-based) as a TQVariant. This
    function is only called if the result is in an active state and is
    positioned on a valid record and \a i is non-negative.
    Derived classes must reimplement this function and return the value
    of field \a i, or TQVariant() if it cannot be determined.
*/

/*!
    \fn  bool TQSqlResult::reset( const TQString& query )

    Sets the result to use the SQL statement \a query for subsequent
    data retrieval. Derived classes must reimplement this function and
    apply the \a query to the database. This function is called only
    after the result is set to an inactive state and is positioned
    before the first record of the new result. Derived classes should
    return true if the query was successful and ready to be used,
    or false otherwise.
*/

/*!
    \fn bool TQSqlResult::fetch( int i )

    Positions the result to an arbitrary (zero-based) index \a i. This
    function is only called if the result is in an active state. Derived
    classes must reimplement this function and position the result to the
    index \a i, and call setAt() with an appropriate value. Return true
    to indicate success, or false to signify failure.
*/

/*!
    \fn bool TQSqlResult::fetchFirst()

    Positions the result to the first record in the result. This
    function is only called if the result is in an active state.
    Derived classes must reimplement this function and position the result
    to the first record, and call setAt() with an appropriate value.
    Return true to indicate success, or false to signify failure.
*/

/*!
    \fn bool TQSqlResult::fetchLast()

    Positions the result to the last record in the result. This
    function is only called if the result is in an active state.
    Derived classes must reimplement this function and position the result
    to the last record, and call setAt() with an appropriate value.
    Return true to indicate success, or false to signify failure.
*/

/*!
    Positions the result to the next available record in the result.
    This function is only called if the result is in an active state.
    The default implementation calls fetch() with the next index.
    Derived classes can reimplement this function and position the result
    to the next record in some other way, and call setAt() with an
    appropriate value. Return true to indicate success, or false to
    signify failure.
*/

bool TQSqlResult::fetchNext()
{
    return fetch( at() + 1 );
}

/*!
    Positions the result to the previous available record in the
    result. This function is only called if the result is in an active
    state. The default implementation calls fetch() with the previous
    index. Derived classes can reimplement this function and position the
    result to the next record in some other way, and call setAt() with
    an appropriate value. Return true to indicate success, or false to
    signify failure.
*/

bool TQSqlResult::fetchPrev()
{
    return fetch( at() - 1 );
}

/*!
    Returns true if you can only scroll forward through a result set;
    otherwise returns false.
*/
bool TQSqlResult::isForwardOnly() const
{
    return forwardOnly;
}

/*!
    Sets forward only mode to \a forward. If forward is true only
    fetchNext() is allowed for navigating the results. Forward only
    mode needs far less memory since results do not have to be cached.
    forward only mode is off by default.

    \sa fetchNext()
*/
void TQSqlResult::setForwardOnly( bool forward )
{
    forwardOnly = forward;
}

// XXX BCI HACK - remove in 4.0
/*! \internal */
void TQSqlResult::setExtension( TQSqlExtension * ext )
{
    if ( d->ext )
	delete d->ext;
    d->ext = ext;
}

/*! \internal */
TQSqlExtension * TQSqlResult::extension()
{
    return d->ext;
}
#endif // TQT_NO_SQL
