/****************************************************************************
**
** Implementation of TQSqlDriver class
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

#include "ntqsqldriver.h"

#ifndef TQT_NO_SQL

#include "ntqdatetime.h"
#include "qsqlextension_p.h"

// database states
#define DBState_Open		0x0001
#define DBState_OpenError	0x0002

// ### This needs to go in 4.0!
TQPtrDict<TQSqlDriverExtension> *tqSqlDriverExtDict();
TQPtrDict<TQSqlOpenExtension> *tqSqlOpenExtDict();

/*!
    \class TQSqlDriver ntqsqldriver.h
    \brief The TQSqlDriver class is an abstract base class for accessing
    SQL databases.

    \ingroup database
    \module sql

    This class should not be used directly. Use TQSqlDatabase instead.
*/

/*!
    Default constructor. Creates a new driver with parent \a parent,
    called \a name.

*/

TQSqlDriver::TQSqlDriver( TQObject * parent, const char * name )
: TQObject(parent, name),
  dbState(0),
  error()
{
}

/*!
    Destroys the object and frees any allocated resources.
*/

TQSqlDriver::~TQSqlDriver()
{
}

/*!
    \fn bool TQSqlDriver::open( const TQString& db, const TQString& user,
    const TQString& password, const TQString& host, int port )

    Derived classes must reimplement this abstract virtual function in
    order to open a database connection on database \a db, using user
    name \a user, password \a password, host \a host and port \a port.

    The function \e must return true on success and false on failure.

    \sa setOpen()

*/

/*!
    \fn bool TQSqlDriver::close()

    Derived classes must reimplement this abstract virtual function in
    order to close the database connection. Return true on success,
    false on failure.

    \sa setOpen()

*/

/*!
    \fn TQSqlQuery TQSqlDriver::createQuery() const

    Creates an empty SQL result on the database. Derived classes must
    reimplement this function and return a TQSqlQuery object
    appropriate for their database to the caller.

*/

//void TQSqlDriver::destroyResult( TQSqlResult* r ) const
//{
//    if ( r )
//	delete r;
//}

/*!
    Returns true if the database connection is open; otherwise returns
    false.
*/

bool TQSqlDriver::isOpen() const
{
    if ( !tqSqlDriverExtDict()->isEmpty() ) {
	TQSqlDriverExtension *ext = tqSqlDriverExtDict()->find( (TQSqlDriver *) this );
	if ( ext )
	    return ext->isOpen();
    }

    return ((dbState & DBState_Open) == DBState_Open);
}

/*!
    Returns true if the there was an error opening the database
    connection; otherwise returns false.
*/

bool TQSqlDriver::isOpenError() const
{
    return ((dbState & DBState_OpenError) == DBState_OpenError);
}

/*!
    \enum TQSqlDriver::DriverFeature

    This enum contains a list of features a driver may support. Use
    hasFeature() to query whether a feature is supported or not.

    \value Transactions  whether the driver supports SQL transactions
    \value QuerySize  whether the database is capable of reporting the size
    of a query. Note that some databases do not support returning the size
    (i.e. number of rows returned) of a query, in which case
    TQSqlQuery::size() will return -1
    \value BLOB  whether the driver supports Binary Large Object fields
    \value Unicode  whether the driver supports Unicode strings if the
    database server does
    \value PreparedQueries  whether the driver supports prepared query execution
    \value NamedPlaceholders  whether the driver supports usage of named placeholders
    \value PositionalPlaceholders  whether the driver supports usage of positional placeholders

    More information about supported features can be found in the
    \link sql-driver.html TQt SQL driver\endlink documentation.

    \sa hasFeature()
*/

/*!
    \fn bool TQSqlDriver::hasFeature( DriverFeature f ) const

    Returns true if the driver supports feature \a f; otherwise
    returns false.

    Note that some databases need to be open() before this can be
    determined.

    \sa DriverFeature
*/

/*!
    Protected function which sets the open state of the database to \a
    o. Derived classes can use this function to report the status of
    open().

    \sa open(), setOpenError()
*/

void TQSqlDriver::setOpen( bool o )
{
    if ( o )
	dbState |= DBState_Open;
    else
	dbState &= ~DBState_Open;
}

/*!
    Protected function which sets the open error state of the database
    to \a e. Derived classes can use this function to report the
    status of open(). Note that if \a e is true the open state of the
    database is set to closed (i.e. isOpen() returns false).

    \sa open(), setOpenError()
*/

void TQSqlDriver::setOpenError( bool e )
{
    if ( e ) {
	dbState |= DBState_OpenError;
	dbState &= ~DBState_Open;
    }
    else
	dbState &= ~DBState_OpenError;
}

/*!
    Protected function which derived classes can reimplement to begin
    a transaction. If successful, return true, otherwise return false.
    The default implementation returns false.

    \sa commitTransaction(), rollbackTransaction()
*/

bool TQSqlDriver::beginTransaction()
{
    return false;
}

/*!
    Protected function which derived classes can reimplement to commit
    a transaction. If successful, return true, otherwise return false.
    The default implementation returns false.

    \sa beginTransaction(), rollbackTransaction()
*/

bool TQSqlDriver::commitTransaction()
{
    return false;
}

/*!
    Protected function which derived classes can reimplement to
    rollback a transaction. If successful, return true, otherwise
    return false. The default implementation returns false.

    \sa beginTransaction(), commitTransaction()
*/

bool TQSqlDriver::rollbackTransaction()
{
    return false;
}

/*!
    Protected function which allows derived classes to set the value
    of the last error, \a e, that occurred on the database.

    \sa lastError()
*/

void TQSqlDriver::setLastError( const TQSqlError& e )
{
    error = e;
}

/*!
    Returns a TQSqlError object which contains information about the
    last error that occurred on the database.
*/

TQSqlError TQSqlDriver::lastError() const
{
    return error;
}

/*!
    Returns a list of tables in the database. The default
    implementation returns an empty list.

    The \a tableType argument describes what types of tables
    should be returned. Due to binary compatibility, the string
    contains the value of the enum TQSql::TableTypes as text.
    An empty string should be treated as TQSql::Tables for
    downward compatibility.

    \sa TQSql::TableType
*/

TQStringList TQSqlDriver::tables( const TQString&	) const
{
    return TQStringList();
}

/*!
    Returns the primary index for table \a tableName. Returns an empty
    TQSqlIndex if the table doesn't have a primary index. The default
    implementation returns an empty index.
*/

TQSqlIndex TQSqlDriver::primaryIndex( const TQString& ) const
{
    return TQSqlIndex();
}


/*!
    Returns a TQSqlRecord populated with the names of the fields in
    table \a tableName. If no such table exists, an empty record is
    returned. The default implementation returns an empty record.
*/

TQSqlRecord TQSqlDriver::record( const TQString&  ) const
{
    return TQSqlRecord();
}

/*!
    \overload

    Returns a TQSqlRecord populated with the names of the fields in the
    SQL \a query. The default implementation returns an empty record.
*/

TQSqlRecord TQSqlDriver::record( const TQSqlQuery& ) const
{
   return TQSqlRecord();
}

/*!
    Returns a TQSqlRecordInfo object with meta data about the table \a
    tablename.
*/
TQSqlRecordInfo TQSqlDriver::recordInfo( const TQString& tablename ) const
{
    return TQSqlRecordInfo( record( tablename ) );
}

/*!
    \overload

    Returns a TQSqlRecordInfo object with meta data for the TQSqlQuery
    \a query. Note that this overloaded function may return less
    information than the recordInfo() function which takes the name of
    a table as parameter.
*/
TQSqlRecordInfo TQSqlDriver::recordInfo( const TQSqlQuery& query ) const
{
    return TQSqlRecordInfo( record( query ) );
}


/*!
    Returns a string representation of the NULL value for the
    database. This is used, for example, when constructing INSERT and
    UPDATE statements. The default implementation returns the string
    "NULL".
*/

TQString TQSqlDriver::nullText() const
{
    return "NULL";
}

/*!
    Returns a string representation of the \a field value for the
    database. This is used, for example, when constructing INSERT and
    UPDATE statements.

    The default implementation returns the value formatted as a string
    according to the following rules:

    \list

    \i If \a field is NULL, nullText() is returned.

    \i If \a field is character data, the value is returned enclosed
    in single quotation marks, which is appropriate for many SQL
    databases. Any embedded single-quote characters are escaped
    (replaced with two single-quote characters). If \a trimStrings is
    true (the default is false), all trailing whitespace is trimmed
    from the field.

    \i If \a field is date/time data, the value is formatted in ISO
    format and enclosed in single quotation marks. If the date/time
    data is invalid, nullText() is returned.

    \i If \a field is bytearray data, and the driver can edit binary
    fields, the value is formatted as a hexadecimal string.

    \i For any other field type toString() will be called on its value
    and the result returned.

    \endlist

    \sa TQVariant::toString().

*/
TQString TQSqlDriver::formatValue( const TQSqlField* field, bool trimStrings ) const
{
    TQString r;
    if ( field->isNull() )
	r = nullText();
    else {
	switch ( field->type() ) {
	case TQVariant::Int:
	case TQVariant::UInt:
	    if ( field->value().type() == TQVariant::Bool )
		r = field->value().toBool() ? "1" : "0";
	    else
		r = field->value().toString();
	    break;
	case TQVariant::Date:
	    if ( field->value().toDate().isValid() )
		r = "'" + field->value().toDate().toString( TQt::ISODate ) + "'";
	    else
		r = nullText();
	    break;
	case TQVariant::Time:
	    if ( field->value().toTime().isValid() )
		r = "'" + field->value().toTime().toString( TQt::ISODate ) + "'";
	    else
		r = nullText();
	    break;
	case TQVariant::DateTime:
	    if ( field->value().toDateTime().isValid() )
		r = "'" +
		    field->value().toDateTime().toString( TQt::ISODate ) + "'";
	    else
		r = nullText();
	    break;
	case TQVariant::String:
	case TQVariant::CString: {
	    TQString result = field->value().toString();
	    if ( trimStrings ) {
		int end = result.length() - 1;
		while ( end && result[end].isSpace() ) /* skip white space from end */
		    end--;
		result.truncate( end );
	    }
	    /* escape the "'" character */
	    result.replace( TQChar( '\'' ), "''" );
	    r = "'" + result + "'";
	    break;
	}
	case TQVariant::Bool:
	    if ( field->value().toBool() )
		r = "1";
	    else
		r = "0";
	    break;
	case TQVariant::ByteArray : {
	    if ( hasFeature( BLOB ) ) {
		TQByteArray ba = field->value().toByteArray();
		TQString res;
		static const char hexchars[] = "0123456789abcdef";
		for ( uint i = 0; i < ba.size(); ++i ) {
		    uchar s = (uchar) ba[(int)i];
		    res += hexchars[s >> 4];
		    res += hexchars[s & 0x0f];
		}
		r = "'" + res + "'";
		break;
	    }
	}
	default:
	    r = field->value().toString();
	    break;
	}
    }
    return r;
}

/*!
    \overload
    
    Open a database connection on database \a db, using user name \a
    user, password \a password, host \a host, port \a port and
    connection options \a connOpts.

    Returns true on success and false on failure.

    \sa setOpen()
*/
bool TQSqlDriver::open( const TQString& db,
		       const TQString& user,
		       const TQString& password,
		       const TQString& host,
		       int port,
		       const TQString& connOpts )
{
    if ( !tqSqlOpenExtDict()->isEmpty() ) {
	TQSqlOpenExtension *ext = tqSqlOpenExtDict()->find( (TQSqlDriver *) this );
	if ( ext )
	    return ext->open( db, user, password, host, port, connOpts );
    }
    return open( db, user, password, host, port );
}

#endif // TQT_NO_SQL
