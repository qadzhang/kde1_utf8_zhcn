/****************************************************************************
**
** Implementation of TQSqlCursor class
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

#include "ntqsqlcursor.h"

#ifndef TQT_NO_SQL

#include "ntqsqldriver.h"
#include "ntqsqlresult.h"
#include "ntqdatetime.h"
#include "ntqsqldatabase.h"
#include "ntqsql.h"

class TQSqlCursorPrivate
{
public:

    TQSqlCursorPrivate( const TQString& name, TQSqlDatabase* sdb )
	: lastAt( TQSql::BeforeFirst ), nm( name ), srt( name ), md( 0 ), db( sdb ), q( 0 )
    {}
    ~TQSqlCursorPrivate()
    {
	delete q;
    }

    TQSqlQuery* query()
    {
	if ( !q )
	    q = new TQSqlQuery( 0, db );
	return q;
    }
    
    int               lastAt;
    TQString           nm;         //name
    TQSqlIndex         srt;        //sort
    TQString           ftr;        //filter
    int               md;         //mode
    TQSqlIndex         priIndx;    //primary index
    TQSqlRecord        editBuffer;
    // the primary index as it was before the user changed the values in editBuffer
    TQString           editIndex;
    TQSqlRecordInfo    infoBuffer;
    TQSqlDatabase*     db;
    TQSqlQuery*        q;
};

TQString qOrderByClause( const TQSqlIndex & i, const TQString& prefix = TQString::null )
{
    TQString str;
    int k = i.count();
    if( k == 0 ) return TQString::null;
    str = " order by " + i.toString( prefix );
    return str;
}

TQString qWhereClause( const TQString& prefix, TQSqlField* field, const TQSqlDriver* driver )
{
    TQString f;
    if ( field && driver ) {
        f = ( prefix.length() > 0 ? prefix + TQString(".") : TQString::null ) + field->name();
        if ( field->isNull() ) {
            f += " IS NULL";
        } else {
            f += " = " + driver->formatValue( field );
        }
    }
    return f;
}

TQString qWhereClause( TQSqlRecord* rec, const TQString& prefix, const TQString& sep, 
		      const TQSqlDriver* driver )
{
    static TQString blank( " " );
    TQString filter;
    bool separator = false;
    for ( uint j = 0; j < rec->count(); ++j ) {
        TQSqlField* f = rec->field( j );
        if ( rec->isGenerated( j ) ) {
            if ( separator )
                filter += sep + blank;
            filter += qWhereClause( prefix, f, driver );
            filter += blank;
            separator = true;
        }
    }
    return filter;
}

/*!
    \class TQSqlCursor ntqsqlcursor.h
    \brief The TQSqlCursor class provides browsing and editing of SQL
    tables and views.

    \ingroup database
    \module sql

    A TQSqlCursor is a database record (see \l TQSqlRecord) that
    corresponds to a table or view within an SQL database (see \l
    TQSqlDatabase). There are two buffers in a cursor, one used for
    browsing and one used for editing records. Each buffer contains a
    list of fields which correspond to the fields in the table or
    view.

    When positioned on a valid record, the browse buffer contains the
    values of the current record's fields from the database. The edit
    buffer is separate, and is used for editing existing records and
    inserting new records.

    For browsing data, a cursor must first select() data from the
    database. After a successful select() the cursor is active
    (isActive() returns true), but is initially not positioned on a
    valid record (isValid() returns false). To position the cursor on
    a valid record, use one of the navigation functions, next(),
    prev(), first(), last(), or seek(). Once positioned on a valid
    record, data can be retrieved from the browse buffer using
    value(). If a navigation function is not successful, it returns
    false, the cursor will no longer be positioned on a valid record
    and the values returned by value() are undefined.

    For example:

    \quotefile sql/overview/retrieve2/main.cpp
    \skipto TQSqlCursor
    \printline TQSqlCursor
    \printuntil }

    In the above example, a cursor is created specifying a table or
    view name in the database. Then, select() is called, which can be
    optionally parameterised to filter and order the records
    retrieved. Each record in the cursor is retrieved using next().
    When next() returns false, there are no more records to process,
    and the loop terminates.

    For editing records (rows of data), a cursor contains a separate
    edit buffer which is independent of the fields used when browsing.
    The functions insert(), update() and del() operate on the edit
    buffer. This allows the cursor to be repositioned to other
    records while simultaneously maintaining a separate buffer for
    edits. You can get a pointer to the edit buffer using
    editBuffer(). The primeInsert(), primeUpdate() and primeDelete()
    functions also return a pointer to the edit buffer and prepare it
    for insert, update and delete respectively. Edit operations only
    affect a single row at a time. Note that update() and del()
    require that the table or view contain a primaryIndex() to ensure
    that edit operations affect a unique record within the database.

    For example:

    \quotefile sql/overview/update/main.cpp
    \skipto prices
    \printline prices
    \printuntil update
    \printline

    To edit an existing database record, first move to the record you
    wish to update. Call primeUpdate() to get the pointer to the
    cursor's edit buffer. Then use this pointer to modify the values
    in the edit buffer. Finally, call update() to save the changes to
    the database. The values in the edit buffer will be used to
    locate the appropriate record when updating the database (see
    primaryIndex()).

    Similarly, when deleting an existing database record, first move
    to the record you wish to delete. Then, call primeDelete() to get
    the pointer to the edit buffer. Finally, call del() to delete the
    record from the database. Again, the values in the edit buffer
    will be used to locate and delete the appropriate record.

    To insert a new record, call primeInsert() to get the pointer to
    the edit buffer. Use this pointer to populate the edit buffer
    with new values and then insert() the record into the database.

    After calling insert(), update() or del(), the cursor is no longer
    positioned on a valid record and can no longer be navigated
    (isValid() return false). The reason for this is that any changes
    made to the database will not be visible until select() is called
    to refresh the cursor. You can change this behavior by passing
    false to insert(), update() or del() which will prevent the cursor
    from becoming invalid. The edits will still not be visible when
    navigating the cursor until select() is called.

    TQSqlCursor contains virtual methods which allow editing behavior
    to be customized by subclasses. This allows custom cursors to be
    created that encapsulate the editing behavior of a database table
    for an entire application. For example, a cursor can be customized
    to always auto-number primary index fields, or provide fields with
    suitable default values, when inserting new records. TQSqlCursor
    generates SQL statements which are sent to the database engine;
    you can control which fields are included in these statements
    using setGenerated().

    Note that TQSqlCursor does not inherit from TQObject. This means
    that you are responsible for destroying instances of this class
    yourself. However if you create a TQSqlCursor and use it in a
    \l TQDataTable, \l TQDataBrowser or a \l TQDataView these classes will
    usually take ownership of the cursor and destroy it when they
    don't need it anymore. The documentation for TQDataTable,
    TQDataBrowser and TQDataView explicitly states which calls take
    ownership of the cursor.
*/

/*!
    \enum TQSqlCursor::Mode

    This enum type describes how TQSqlCursor operates on records in the
    database.

    \value ReadOnly  the cursor can only SELECT records from the
    database.

    \value Insert  the cursor can INSERT records into the database.

    \value Update  the cursor can UPDATE records in the database.

    \value Delete  the cursor can DELETE records from the database.

    \value Writable  the cursor can INSERT, UPDATE and DELETE records
    in the database.
*/

/*!
    Constructs a cursor on database \a db using table or view \a name.

    If \a autopopulate is true (the default), the \a name of the
    cursor must correspond to an existing table or view name in the
    database so that field information can be automatically created.
    If the table or view does not exist, the cursor will not be
    functional.

    The cursor is created with an initial mode of TQSqlCursor::Writable
    (meaning that records can be inserted, updated or deleted using
    the cursor). If the cursor does not have a unique primary index,
    update and deletes cannot be performed.

    Note that \a autopopulate refers to populating the cursor with
    meta-data, e.g. the names of the table's fields, not with
    retrieving data. The select() function is used to populate the
    cursor with data.

    \sa setName() setMode()
*/

TQSqlCursor::TQSqlCursor( const TQString & name, bool autopopulate, TQSqlDatabase* db )
    : TQSqlRecord(), TQSqlQuery( TQString::null, db )
{
    d = new TQSqlCursorPrivate( name, db );
    setMode( Writable );
    if ( !d->nm.isNull() )
	setName( d->nm, autopopulate );
}

/*!
    Constructs a copy of \a other.
*/

TQSqlCursor::TQSqlCursor( const TQSqlCursor & other )
    : TQSqlRecord( other ), TQSqlQuery( other )
{
    d = new TQSqlCursorPrivate( other.d->nm, other.d->db );
    d->lastAt = other.d->lastAt;
    d->nm = other.d->nm;
    d->srt = other.d->srt;
    d->ftr = other.d->ftr;
    d->priIndx = other.d->priIndx;
    d->editBuffer = other.d->editBuffer;
    d->infoBuffer = other.d->infoBuffer;
    d->q = 0; // do not share queries
    setMode( other.mode() );
}

/*!
    Destroys the object and frees any allocated resources.
*/

TQSqlCursor::~TQSqlCursor()
{
    delete d;
}

/*!
    Sets the cursor equal to \a other.
*/

TQSqlCursor& TQSqlCursor::operator=( const TQSqlCursor& other )
{
    TQSqlRecord::operator=( other );
    TQSqlQuery::operator=( other );
    delete d;
    d = new TQSqlCursorPrivate( other.d->nm, other.d->db );
    d->lastAt = other.d->lastAt;
    d->nm = other.d->nm;
    d->srt = other.d->srt;
    d->ftr = other.d->ftr;
    d->priIndx = other.d->priIndx;
    d->editBuffer = other.d->editBuffer;
    d->infoBuffer = other.d->infoBuffer;
    d->q = 0; // do not share queries
    setMode( other.mode() );
    return *this;
}

/*!
    Sets the current sort to \a sort. Note that no new records are
    selected. To select new records, use select(). The \a sort will
    apply to any subsequent select() calls that do not explicitly
    specify a sort.
*/

void TQSqlCursor::setSort( const TQSqlIndex& sort )
{
    d->srt = sort;
}

/*!
    Returns the current sort, or an empty index if there is no current
    sort.
*/
TQSqlIndex TQSqlCursor::sort() const
{
    return d->srt;
}

/*!
    Sets the current filter to \a filter. Note that no new records are
    selected. To select new records, use select(). The \a filter will
    apply to any subsequent select() calls that do not explicitly
    specify a filter.

    The filter is a SQL \c WHERE clause without the keyword 'WHERE',
    e.g. \c{name='Dave'} which will be processed by the DBMS.
*/
void TQSqlCursor::setFilter( const TQString& filter )
{
    d->ftr = filter;
}

/*!
    Returns the current filter, or an empty string if there is no
    current filter.
*/
TQString TQSqlCursor::filter() const
{
    return d->ftr;
}

/*!
    Sets the name of the cursor to \a name. If \a autopopulate is true
    (the default), the \a name must correspond to a valid table or
    view name in the database. Also, note that all references to the
    cursor edit buffer become invalidated when fields are
    auto-populated. See the TQSqlCursor constructor documentation for
    more information.
*/
void TQSqlCursor::setName( const TQString& name, bool autopopulate )
{
    d->nm = name;
    if ( autopopulate ) {
	if ( driver() ) {
	    d->infoBuffer = driver()->recordInfo( name );
	    *this = d->infoBuffer.toRecord();
	    d->editBuffer = *this;
	    d->priIndx = driver()->primaryIndex( name );
	}
#ifdef QT_CHECK_RANGE
	if ( isEmpty() )
	    tqWarning("TQSqlCursor::setName: unable to build record, does '%s' exist?", name.latin1() );
#endif
    }
}

/*!
    Returns the name of the cursor.
*/

TQString TQSqlCursor::name() const
{
    return d->nm;
}

/*! \reimp
*/

TQString TQSqlCursor::toString( const TQString& prefix, const TQString& sep ) const
{
    TQString pflist;
    TQString pfix =  prefix.isEmpty() ? TQString::null : prefix + ".";
    bool comma = false;

    for ( uint i = 0; i < count(); ++i ) {
	const TQString fname = fieldName( i );
	if ( isGenerated( i ) ) {
	    if( comma )
		pflist += sep + " ";
	    pflist += pfix + fname;
	    comma = true;
	}
    }
    return pflist;
}

/*!
  \internal

  Assigns the record \a list.

*/
TQSqlRecord & TQSqlCursor::operator=( const TQSqlRecord & list )
{
    return TQSqlRecord::operator=( list );
}

/*!
    Append a copy of field \a fieldInfo to the end of the cursor. Note
    that all references to the cursor edit buffer become invalidated.
*/

void TQSqlCursor::append( const TQSqlFieldInfo& fieldInfo )
{
    d->editBuffer.append( fieldInfo.toField() );
    d->editBuffer.setGenerated( d->editBuffer.count() - 1, fieldInfo.isGenerated() );
    d->infoBuffer.append( fieldInfo );
    TQSqlRecord::append( fieldInfo.toField() );
    TQSqlRecord::setGenerated( TQSqlRecord::count() - 1, fieldInfo.isGenerated() );
}

/*!
    Removes all fields from the cursor. Note that all references to
    the cursor edit buffer become invalidated.
*/
void TQSqlCursor::clear()
{
    d->editBuffer.clear();
    d->infoBuffer.clear();
    TQSqlRecord::clear();
}


/*!
    Insert a copy of \a fieldInfo at position \a pos. If a field
    already exists at \a pos, it is removed. Note that all references
    to the cursor edit buffer become invalidated.
*/

void  TQSqlCursor::insert( int pos, const TQSqlFieldInfo& fieldInfo )
{
    d->editBuffer.insert( pos, fieldInfo.toField() );
    d->editBuffer.setGenerated( pos, fieldInfo.isGenerated() );
    d->infoBuffer[ pos ] = fieldInfo;
    TQSqlRecord::insert( pos, fieldInfo.toField() );
    TQSqlRecord::setGenerated( pos, fieldInfo.isGenerated() );
}

/*!
    Removes the field at \a pos. If \a pos does not exist, nothing
    happens. Note that all references to the cursor edit buffer become
    invalidated.
*/

void TQSqlCursor::remove( int pos )
{
    d->editBuffer.remove( pos );
    d->infoBuffer[ pos ] = TQSqlFieldInfo();
    TQSqlRecord::remove( pos );
}

/*!
    Sets the generated flag for the field \a name to \a generated. If
    the field does not exist, nothing happens. Only fields that have
    \a generated set to true are included in the SQL that is
    generated by insert(), update() or del().

    \sa isGenerated()
*/

void TQSqlCursor::setGenerated( const TQString& name, bool generated )
{
    int pos = position( name );
    if ( pos == -1 )
	return;
    TQSqlRecord::setGenerated( name, generated );
    d->editBuffer.setGenerated( name, generated );
    d->infoBuffer[ pos ].setGenerated( generated );
}

/*!
    \overload

    Sets the generated flag for the field \a i to \a generated.

    \sa isGenerated()
*/
void TQSqlCursor::setGenerated( int i, bool generated )
{
    if ( i < 0 || i >= (int)d->infoBuffer.count() )
	return;
    TQSqlRecord::setGenerated( i, generated );
    d->editBuffer.setGenerated( i, generated );
    d->infoBuffer[i].setGenerated( generated );
}

/*!
    Returns the primary index associated with the cursor as defined in
    the database, or an empty index if there is no primary index. If
    \a setFromCursor is true (the default), the index fields are
    populated with the corresponding values in the cursor's current
    record.
*/

TQSqlIndex TQSqlCursor::primaryIndex( bool setFromCursor ) const
{
    if ( setFromCursor ) {
	for ( uint i = 0; i < d->priIndx.count(); ++i ) {
	    const TQString fn = d->priIndx.fieldName( i );
	    if ( contains( fn ) )
		d->priIndx.setValue( i, value( fn ) );
	}
    }
    return d->priIndx;
}

/*!
    Sets the primary index associated with the cursor to the index \a
    idx. Note that this index must contain a field or set of fields
    which identify a unique record within the underlying database
    table or view so that update() and del() will execute as expected.

    \sa update() del()
*/

void TQSqlCursor::setPrimaryIndex( const TQSqlIndex& idx )
{
    d->priIndx = idx;
}


/*!
    Returns an index composed of \a fieldNames, all in ASCending
    order. Note that all field names must exist in the cursor,
    otherwise an empty index is returned.

    \sa TQSqlIndex
*/

TQSqlIndex TQSqlCursor::index( const TQStringList& fieldNames ) const
{
    TQSqlIndex idx;
    for ( TQStringList::ConstIterator it = fieldNames.begin(); it != fieldNames.end(); ++it ) {
	const TQSqlField* f = field( (*it) );
	if ( !f ) { /* all fields must exist */
	    idx.clear();
	    break;
	}
	idx.append( *f );
    }
    return idx;
}

/*!
    \overload

    Returns an index based on \a fieldName.
*/

TQSqlIndex TQSqlCursor::index( const TQString& fieldName ) const
{
    TQStringList fl( fieldName );
    return index( fl );
}

/*!
    \overload

    Returns an index based on \a fieldName.
*/

TQSqlIndex TQSqlCursor::index( const char* fieldName ) const
{
    return index( TQStringList( TQString( fieldName ) ) );
}

/*!
    Selects all fields in the cursor from the database matching the
    filter criteria \a filter. The data is returned in the order
    specified by the index \a sort. Returns true if the data was
    successfully selected; otherwise returns false.

    The \a filter is a string containing a SQL \c WHERE clause but
    without the 'WHERE' keyword. The cursor is initially positioned at
    an invalid row after this function is called. To move to a valid
    row, use seek(), first(), last(), prev() or next().

    Example:
    \code
    TQSqlCursor cur( "Employee" ); // Use the Employee table or view
    cur.select( "deptno=10" ); // select all records in department 10
    while( cur.next() ) {
	... // process data
    }
    ...
    // select records in other departments, ordered by department number
    cur.select( "deptno>10", cur.index( "deptno" ) );
    ...
    \endcode

    The filter will apply to any subsequent select() calls that do not
    explicitly specify another filter. Similarly the sort will apply
    to any subsequent select() calls that do not explicitly specify
    another sort.

    \code
    TQSqlCursor cur( "Employee" );
    cur.select( "deptno=10" ); // select all records in department 10
    while( cur.next() ) {
	... // process data
    }
    ...
    cur.select(); // re-selects all records in department 10
    ...
    \endcode

*/

bool TQSqlCursor::select( const TQString & filter, const TQSqlIndex & sort )
{
    TQString fieldList = toString( d->nm );
    if ( fieldList.isEmpty() )
	return false;
    TQString str= "select " + fieldList;
    str += " from " + d->nm;
    if ( !filter.isEmpty() ) {
	d->ftr = filter;
	str += " where " + filter;
    } else
	d->ftr = TQString::null;
    if ( sort.count() > 0 )
	str += " order by " + sort.toString( d->nm );
    d->srt = sort;
    return exec( str );
}

/*!
    \overload

    Selects all fields in the cursor from the database. The rows are
    returned in the order specified by the last call to setSort() or
    the last call to select() that specified a sort, whichever is the
    most recent. If there is no current sort, the order in which the
    rows are returned is undefined. The records are filtered according
    to the filter specified by the last call to setFilter() or the
    last call to select() that specified a filter, whichever is the
    most recent. If there is no current filter, all records are
    returned. The cursor is initially positioned at an invalid row. To
    move to a valid row, use seek(), first(), last(), prev() or
    next().

    \sa setSort() setFilter()
*/

bool TQSqlCursor::select()
{
    return select( filter(), sort() );
}

/*!
    \overload

    Selects all fields in the cursor from the database. The data is
    returned in the order specified by the index \a sort. The records
    are filtered according to the filter specified by the last call to
    setFilter() or the last call to select() that specified a filter,
    whichever is the most recent. The cursor is initially positioned
    at an invalid row. To move to a valid row, use seek(), first(),
    last(), prev() or next().
*/

bool TQSqlCursor::select( const TQSqlIndex& sort )
{
    return select( filter(), sort );
}

/*!
    \overload

    Selects all fields in the cursor matching the filter index \a
    filter. The data is returned in the order specified by the index
    \a sort. The \a filter index works by constructing a WHERE clause
    using the names of the fields from the \a filter and their values
    from the current cursor record. The cursor is initially positioned
    at an invalid row. To move to a valid row, use seek(), first(),
    last(), prev() or next(). This function is useful, for example,
    for retrieving data based upon a table's primary index:

    \code
    TQSqlCursor cur( "Employee" );
    TQSqlIndex pk = cur.primaryIndex();
    cur.setValue( "id", 10 );
    cur.select( pk, pk ); // generates "SELECT ... FROM Employee WHERE id=10 ORDER BY id"
    ...
    \endcode

    In this example the TQSqlIndex, pk, is used for two different
    purposes. When used as the filter (first) argument, the field
    names it contains are used to construct the WHERE clause, each set
    to the current cursor value, \c{WHERE id=10}, in this case. When
    used as the sort (second) argument the field names it contains are
    used for the ORDER BY clause, \c{ORDER BY id} in this example.
*/

bool TQSqlCursor::select( const TQSqlIndex & filter, const TQSqlIndex & sort )
{
    return select( toString( filter, this, d->nm, "=", "and" ), sort );
}

/*!
    Sets the cursor mode to \a mode. This value can be an OR'ed
    combination of \l TQSqlCursor::Mode values. The default mode for a
    cursor is \c TQSqlCursor::Writable.

    \code
    TQSqlCursor cur( "Employee" );
    cur.setMode( TQSqlCursor::Writable ); // allow insert/update/delete
    ...
    cur.setMode( TQSqlCursor::Insert | TQSqlCursor::Update ); // allow inserts and updates only
    ...
    cur.setMode( TQSqlCursor::ReadOnly ); // no inserts/updates/deletes allowed

    \endcode
*/

void TQSqlCursor::setMode( int mode )
{
    d->md = mode;
}

/*!
    Returns the current cursor mode.

    \sa setMode()
*/

int TQSqlCursor::mode() const
{
    return d->md;
}

/*!
    Sets field \a name to \a calculated. If the field \a name does not
    exist, nothing happens. The value of a calculated field is set by
    the calculateField() virtual function which you must reimplement
    (or the field value will be an invalid TQVariant). Calculated
    fields do not appear in generated SQL statements sent to the
    database.

    \sa calculateField() TQSqlRecord::setGenerated()
*/

void TQSqlCursor::setCalculated( const TQString& name, bool calculated )
{
    int pos = position( name );
    if ( pos < 0 )
	return;
    d->infoBuffer[ pos ].setCalculated( calculated );
    if ( calculated )
	setGenerated( pos, false );
}

/*!
    Returns true if the field \a name exists and is calculated;
    otherwise returns false.

    \sa setCalculated()
*/

bool TQSqlCursor::isCalculated( const TQString& name ) const
{
    int pos = position( name );
    if ( pos < 0 )
	return false;
    return d->infoBuffer[ pos ].isCalculated();
}

/*!
    Sets field \a{name}'s trimmed status to \a trim. If the field \a
    name does not exist, nothing happens.

    When a trimmed field of type string or cstring is read from the
    database any trailing (right-most) spaces are removed.

    \sa isTrimmed() TQVariant
*/

void TQSqlCursor::setTrimmed( const TQString& name, bool trim )
{
    int pos = position( name );
    if ( pos < 0 )
	return;
    d->infoBuffer[ pos ].setTrim( trim );
}

/*!
    Returns true if the field \a name exists and is trimmed; otherwise
    returns false.

    When a trimmed field of type string or cstring is read from the
    database any trailing (right-most) spaces are removed.

    \sa setTrimmed()
*/

bool TQSqlCursor::isTrimmed( const TQString& name ) const
{
    int pos = position( name );
    if ( pos < 0 )
	return false;
    return d->infoBuffer[ pos ].isTrim();
}

/*!
    Returns true if the cursor is read-only; otherwise returns false.
    The default is false. Read-only cursors cannot be edited using
    insert(), update() or del().

    \sa setMode()
*/

bool TQSqlCursor::isReadOnly() const
{
    return d->md == 0;
}

/*!
    Returns true if the cursor will perform inserts; otherwise returns
    false.

    \sa setMode()
*/

bool TQSqlCursor::canInsert() const
{
    return ( ( d->md & Insert ) == Insert ) ;
}


/*!
    Returns true if the cursor will perform updates; otherwise returns
    false.

    \sa setMode()
*/

bool TQSqlCursor::canUpdate() const
{
    return ( ( d->md & Update ) == Update ) ;
}

/*!
    Returns true if the cursor will perform deletes; otherwise returns
    false.

    \sa setMode()
*/

bool TQSqlCursor::canDelete() const
{
    return ( ( d->md & Delete ) == Delete ) ;
}

/*!
    \overload

    Returns a formatted string composed of the \a prefix (e.g. table
    or view name), ".", the \a field name, the \a fieldSep and the
    field value. If the \a prefix is empty then the string will begin
    with the \a field name. This function is useful for generating SQL
    statements.
*/

TQString TQSqlCursor::toString( const TQString& prefix, TQSqlField* field, const TQString& fieldSep ) const
{
    TQString f;
    if ( field && driver() ) {
	f = ( prefix.length() > 0 ? prefix + TQString(".") : TQString::null ) + field->name();
	f += " " + fieldSep + " ";
	if ( field->isNull() ) {
	    f += "NULL";
	} else {
	    f += driver()->formatValue( field );
	}
    }
    return f;
}

/*!
    Returns a formatted string composed of all the fields in \a rec.
    Each field is composed of the \a prefix (e.g. table or view name),
    ".", the field name, the \a fieldSep and the field value. If the
    \a prefix is empty then each field will begin with the field name.
    The fields are then joined together separated by \a sep. Fields
    where isGenerated() returns false are not included. This function
    is useful for generating SQL statements.
*/

TQString TQSqlCursor::toString( TQSqlRecord* rec, const TQString& prefix, const TQString& fieldSep,
			      const TQString& sep ) const
{
    static TQString blank( " " );
    TQString filter;
    bool separator = false;
    for ( uint j = 0; j < count(); ++j ) {
	TQSqlField* f = rec->field( j );
	if ( rec->isGenerated( j ) ) {
	    if ( separator )
		filter += sep + blank;
	    filter += toString( prefix, f, fieldSep );
	    filter += blank;
	    separator = true;
	}
    }
    return filter;
}

/*!
    \overload

    Returns a formatted string composed of all the fields in the index
    \a i. Each field is composed of the \a prefix (e.g. table or view
    name), ".", the field name, the \a fieldSep and the field value.
    If the \a prefix is empty then each field will begin with the field
    name. The field values are taken from \a rec. The fields are then
    joined together separated by \a sep. Fields where isGenerated()
    returns false are ignored. This function is useful for generating
    SQL statements.
*/

TQString TQSqlCursor::toString( const TQSqlIndex& i, TQSqlRecord* rec, const TQString& prefix,
				const TQString& fieldSep, const TQString& sep ) const
{
    TQString filter;
    bool separator = false;
    for( uint j = 0; j < i.count(); ++j ){
	if ( rec->isGenerated( j ) ) {
	    if( separator ) {
		filter += " " + sep + " " ;
	    }
	    TQString fn = i.fieldName( j );
	    TQSqlField* f = rec->field( fn );
	    filter += toString( prefix, f, fieldSep );
	    separator = true;
	}
    }
    return filter;
}

/*!
    \overload

    Inserts the current contents of the cursor's edit record buffer
    into the database, if the cursor allows inserts. Returns the
    number of rows affected by the insert. For error information, use
    lastError().

    If \a invalidate is true (the default), the cursor will no longer
    be positioned on a valid record and can no longer be navigated. A
    new select() call must be made before navigating to a valid
    record.

    \quotefile sql/overview/insert2/main.cpp
    \skipto prices
    \printline prices
    \printuntil insert

    In the above example, a cursor is created on the 'prices' table
    and a pointer to the insert buffer is acquired using primeInsert().
    Each field's value is set to the desired value and then insert()
    is called to insert the data into the database. Remember: all edit
    operations (insert(), update() and delete()) operate on the
    contents of the cursor edit buffer and not on the contents of the
    cursor itself.

    \sa setMode() lastError()
*/

int TQSqlCursor::insert( bool invalidate )
{
    if ( ( d->md & Insert ) != Insert || !driver() )
	return false;
    int k = d->editBuffer.count();
    if ( k == 0 )
	return 0;
    
    TQString fList;
    TQString vList;
    bool comma = false;
    // use a prepared query if the driver supports it
    if ( driver()->hasFeature( TQSqlDriver::PreparedQueries ) ) {
	int cnt = 0;
	bool oraStyle = driver()->hasFeature( TQSqlDriver::NamedPlaceholders );
	for( int j = 0; j < k; ++j ) {
	    TQSqlField* f = d->editBuffer.field( j );
	    if ( d->editBuffer.isGenerated( j ) ) {
		if ( comma ) {
		    fList += ",";
		    vList += ",";
		}
		fList += f->name();
		vList += oraStyle ? ":f" + TQString::number(cnt) : TQString("?");
		cnt++;
		comma = true;
	    }
	}
	if ( !comma ) {
	    return 0;
	}
	TQString str;
	str.append( "insert into " ).append( name() ).append( "(" ).append( fList ).append( ") values (" ).append( vList ). append ( ")" );
	return applyPrepared( str, invalidate );
    } else {
	for( int j = 0; j < k; ++j ) {
	    TQSqlField* f = d->editBuffer.field( j );
	    if ( d->editBuffer.isGenerated( j ) ) {
		if ( comma ) {
		    fList += ",";
		    vList += ",";
		}
		fList += f->name();
		vList += driver()->formatValue( f );
		comma = true;
	    }
	}

	if ( !comma ) {
	    // no valid fields found
	    return 0;
	}
	TQString str;
	str.append( "insert into " ).append( name() ).append( "(" ).append( fList ).append( ") values (" ).append( vList ). append ( ")" );
	return apply( str, invalidate );
    }
}

/*!
    Returns the current internal edit buffer. If \a copy is true (the
    default is false), the current cursor field values are first
    copied into the edit buffer. The edit buffer is valid as long as
    the cursor remains valid. The cursor retains ownership of the
    returned pointer, so it must not be deleted or modified.

    \sa primeInsert(), primeUpdate() primeDelete()
*/

TQSqlRecord* TQSqlCursor::editBuffer( bool copy )
{
    if ( copy ) {
	for(uint i = 0; i < d->editBuffer.count(); i++) {
	    if ( TQSqlRecord::isNull( i ) ) {
		d->editBuffer.setNull( i );
	    } else {
		d->editBuffer.setValue( i, value( i ) );
	    }
	}
    }
    return &d->editBuffer;
}

/*!
    This function primes the edit buffer's field values for update and
    returns the edit buffer. The default implementation copies the
    field values from the current cursor record into the edit buffer
    (therefore, this function is equivalent to calling editBuffer(
    true ) ). The cursor retains ownership of the returned pointer, so
    it must not be deleted or modified.

    \sa editBuffer() update()
*/

TQSqlRecord* TQSqlCursor::primeUpdate()
{
    // memorize the primary keys as they were before the user changed the values in editBuffer
    TQSqlRecord* buf = editBuffer( true );
    TQSqlIndex idx = primaryIndex( false );
    if ( !idx.isEmpty() )
	d->editIndex = toString( idx, buf, d->nm, "=", "and" );
    else
	d->editIndex = qWhereClause( buf, d->nm, "and", driver() );
    return buf;
}

/*!
    This function primes the edit buffer's field values for delete and
    returns the edit buffer. The default implementation copies the
    field values from the current cursor record into the edit buffer
    (therefore, this function is equivalent to calling editBuffer(
    true ) ). The cursor retains ownership of the returned pointer, so
    it must not be deleted or modified.

    \sa editBuffer() del()
*/

TQSqlRecord* TQSqlCursor::primeDelete()
{
    return editBuffer( true );
}

/*!
    This function primes the edit buffer's field values for insert and
    returns the edit buffer. The default implementation clears all
    field values in the edit buffer. The cursor retains ownership of
    the returned pointer, so it must not be deleted or modified.

    \sa editBuffer() insert()
*/

TQSqlRecord* TQSqlCursor::primeInsert()
{
    d->editBuffer.clearValues();
    return &d->editBuffer;
}


/*!
    Updates the database with the current contents of the edit buffer.
    Returns the number of records which were updated.
    For error information, use lastError().

    Only records which meet the filter criteria specified by the
    cursor's primary index are updated. If the cursor does not contain
    a primary index, no update is performed and 0 is returned.

    If \a invalidate is true (the default), the current cursor can no
    longer be navigated. A new select() call must be made before you
    can move to a valid record. For example:

    \quotefile sql/overview/update/main.cpp
    \skipto prices
    \printline prices
    \printuntil update
    \printline

    In the above example, a cursor is created on the 'prices' table
    and is positioned on the record to be updated. Then a pointer to
    the cursor's edit buffer is acquired using primeUpdate(). A new
    value is calculated and placed into the edit buffer with the
    setValue() call. Finally, an update() call is made on the cursor
    which uses the tables's primary index to update the record in the
    database with the contents of the cursor's edit buffer. Remember:
    all edit operations (insert(), update() and delete()) operate on
    the contents of the cursor edit buffer and not on the contents of
    the cursor itself.

    Note that if the primary index does not uniquely distinguish
    records the database may be changed into an inconsistent state.

    \sa setMode() lastError()
*/

int TQSqlCursor::update( bool invalidate )
{
    if ( d->editIndex.isEmpty() )
	return 0;
    return update( d->editIndex, invalidate );
}

/*!
    \overload

    Updates the database with the current contents of the cursor edit
    buffer using the specified \a filter. Returns the number of
    records which were updated.
    For error information, use lastError().

    Only records which meet the filter criteria are updated, otherwise
    all records in the table are updated.

    If \a invalidate is true (the default), the cursor can no longer
    be navigated. A new select() call must be made before you can move
    to a valid record.

    \sa primeUpdate() setMode() lastError()
*/

int TQSqlCursor::update( const TQString & filter, bool invalidate )
{    
    if ( ( d->md & Update ) != Update ) {
	return false;
    }
    int k = count();
    if ( k == 0 ) {
	return 0;
    }
    
    // use a prepared query if the driver supports it
    if ( driver()->hasFeature( TQSqlDriver::PreparedQueries ) ) {
	TQString fList;
	bool comma = false;
	int cnt = 0;
	bool oraStyle = driver()->hasFeature( TQSqlDriver::NamedPlaceholders );
	for( int j = 0; j < k; ++j ) {
	    TQSqlField* f = d->editBuffer.field( j );
	    if ( d->editBuffer.isGenerated( j ) ) {
		if ( comma ) {
		    fList += ",";
		}
		fList += f->name() + " = " + (oraStyle ? ":f" + TQString::number(cnt) : TQString("?"));
		cnt++;
		comma = true;
	    }
	}
	if ( !comma ) {
	    return 0;
	}
	TQString str = "update " + name() + " set " + fList;
	if ( filter.length() ) {
	    str+= " where " + filter;
	}
	return applyPrepared( str, invalidate );
    } else {
	TQString str = "update " + name();
	str += " set " + toString( &d->editBuffer, TQString::null, "=", "," );
	if ( filter.length() ) {
	    str+= " where " + filter;
	}
	return apply( str, invalidate );
    }
}

/*!
    Deletes a record from the database using the cursor's primary
    index and the contents of the cursor edit buffer. Returns the
    number of records which were deleted.
    For error information, use lastError().

    Only records which meet the filter criteria specified by the
    cursor's primary index are deleted. If the cursor does not contain
    a primary index, no delete is performed and 0 is returned. If \a
    invalidate is true (the default), the current cursor can no longer
    be navigated. A new select() call must be made before you can move
    to a valid record. For example:

    \quotefile sql/overview/delete/main.cpp
    \skipto prices
    \printline prices
    \printuntil }

    In the above example, a cursor is created on the 'prices' table
    and positioned to the record to be deleted. First primeDelete() is
    called to populate the edit buffer with the current cursor values,
    e.g. with an id of 999, and then del() is called to actually
    delete the record from the database. Remember: all edit operations
    (insert(), update() and delete()) operate on the contents of the
    cursor edit buffer and not on the contents of the cursor itself.

    \sa primeDelete() setMode() lastError()
*/

int TQSqlCursor::del( bool invalidate )
{
    TQSqlIndex idx = primaryIndex( false );
    if ( idx.isEmpty() )
        return del( qWhereClause( &d->editBuffer, d->nm, "and", driver() ), invalidate );
    else
	return del( toString( primaryIndex(), &d->editBuffer, d->nm,
			  "=", "and" ), invalidate );
}

/*!
    \overload

    Deletes the current cursor record from the database using the
    filter \a filter. Only records which meet the filter criteria are
    deleted. Returns the number of records which were deleted. If \a
    invalidate is true (the default), the current cursor can no longer
    be navigated. A new select() call must be made before you can move
    to a valid record. For error information, use lastError().

    The \a filter is an SQL \c WHERE clause, e.g. \c{id=500}.

    \sa setMode() lastError()
*/

int TQSqlCursor::del( const TQString & filter, bool invalidate )
{
    if ( ( d->md & Delete ) != Delete )
	return 0;
    int k = count();
    if( k == 0 ) return 0;
    TQString str = "delete from " + name();
    if ( filter.length() )
	str+= " where " + filter;
    return apply( str, invalidate );
}

/*
  \internal
*/

int TQSqlCursor::apply( const TQString& q, bool invalidate )
{
    int ar = 0;
    if ( invalidate ) {
	if ( exec( q ) )
	    ar = numRowsAffected();
    } else if ( driver() ) {
	TQSqlQuery* sql = d->query();
	if ( sql && sql->exec( q ) )
	    ar = sql->numRowsAffected();
    }
    return ar;
}

/*
  \internal
*/

int TQSqlCursor::applyPrepared( const TQString& q, bool invalidate )
{
    int ar = 0;
    TQSqlQuery* sql = 0;
    
    if ( invalidate ) {
	sql = (TQSqlQuery*)this;
	d->lastAt = TQSql::BeforeFirst;
    } else {
	sql = d->query();
    }
    if ( !sql )
	return 0;
    
    if ( invalidate || sql->lastQuery() != q ) {
	if ( !sql->prepare( q ) )
	    return 0;
    }
    
    int cnt = 0;
    int fieldCount = (int)count();
    for ( int j = 0; j < fieldCount; ++j ) {
	const TQSqlField* f = d->editBuffer.field( j );
	if ( d->editBuffer.isGenerated( j ) ) {
	    sql->bindValue( cnt, f->value() );
	    cnt++;
	}
    }
    if ( sql->exec() ) {
	ar = sql->numRowsAffected();
    }
    return ar;
}

/*!  \reimp

  Executes the SQL query \a sql. Returns true of the cursor is
  active, otherwise returns false.

*/
bool TQSqlCursor::exec( const TQString & sql )
{
    d->lastAt = TQSql::BeforeFirst;
    TQSqlQuery::exec( sql );
    return isActive();
}

/*!
    Protected virtual function which is called whenever a field needs
    to be calculated. If calculated fields are being used, derived
    classes must reimplement this function and return the appropriate
    value for field \a name. The default implementation returns an
    invalid TQVariant.

    \sa setCalculated()
*/

TQVariant TQSqlCursor::calculateField( const TQString& )
{
    return TQVariant();
}

/*! \internal
   Ensure fieldlist is synced with query.

*/

static TQString qTrim( const TQString& s )
{
    TQString result = s;
    int end = result.length() - 1;
    while ( end >= 0 && result[end].isSpace() ) // skip white space from end
	end--;
    result.truncate( end + 1 );
    return result;
}

/*! \internal
 */

void TQSqlCursor::sync()
{
    if ( isActive() && isValid() && d->lastAt != at() ) {
	d->lastAt = at();
	uint i = 0;
	uint j = 0;
	bool haveCalculatedFields = false;
	for ( ; i < count(); ++i ) {
	    if ( !haveCalculatedFields && d->infoBuffer[i].isCalculated() ) {
		haveCalculatedFields = true;
	    }
	    if ( TQSqlRecord::isGenerated( i ) ) {
		TQVariant v = TQSqlQuery::value( j );
		if ( ( v.type() == TQVariant::String || v.type() == TQVariant::CString ) &&
			d->infoBuffer[ i ].isTrim() ) {
		    v = qTrim( v.toString() );
		}
		TQSqlRecord::setValue( i, v );
		if ( TQSqlQuery::isNull( j ) )
		    TQSqlRecord::field( i )->setNull();
		j++;
	    }
	}
	if ( haveCalculatedFields ) {
	    for ( i = 0; i < count(); ++i ) {
		if ( d->infoBuffer[i].isCalculated() )
		    TQSqlRecord::setValue( i, calculateField( fieldName( i ) ) );
	    }
	}
    }
}

/*! \reimp

*/

void TQSqlCursor::afterSeek()
{
    sync();
}

/*!
    \reimp

    Returns the value of field number \a i.
*/

TQVariant TQSqlCursor::value( int i ) const
{
    return TQSqlRecord::value( i );
}

/*!
    \reimp

    Returns the value of the field called \a name.
*/

TQVariant TQSqlCursor::value( const TQString& name ) const
{
    return TQSqlRecord::value( name );
}

/*! \internal
  cursors should be filled with TQSqlFieldInfos...
*/
void TQSqlCursor::append( const TQSqlField& field )
{
    append( TQSqlFieldInfo( field ) );
}
/*! \internal
  cursors should be filled with TQSqlFieldInfos...
*/
void TQSqlCursor::insert( int pos, const TQSqlField& field )
{
    insert( pos, TQSqlFieldInfo( field ) );
}

/*!
    Returns true if the field \a i is NULL or if there is no field at
    position \a i; otherwise returns false.

    This is the same as calling TQSqlRecord::isNull( \a i )
*/
bool TQSqlCursor::isNull( int i ) const
{
    return TQSqlRecord::isNull( i );
}
/*!
    \overload

    Returns true if the field called \a name is NULL or if there is no
    field called \a name; otherwise returns false.

    This is the same as calling TQSqlRecord::isNull( \a name )
*/
bool TQSqlCursor::isNull( const TQString& name ) const
{
    return TQSqlRecord::isNull( name );
}

/*! \reimp */
void TQSqlCursor::setValue( int i, const TQVariant& val )
{
#ifdef QT_DEBUG
    tqDebug("TQSqlCursor::setValue(): This will not affect actual database values. Use primeInsert(), primeUpdate() or primeDelete().");
#endif
    TQSqlRecord::setValue( i, val );
}

/*! \reimp */
void TQSqlCursor::setValue( const TQString& name, const TQVariant& val )
{
#ifdef QT_DEBUG
    tqDebug("TQSqlCursor::setValue(): This will not affect actual database values. Use primeInsert(), primeUpdate() or primeDelete().");
#endif
    TQSqlRecord::setValue( name, val );
}
#endif
