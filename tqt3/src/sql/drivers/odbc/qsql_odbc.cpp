/****************************************************************************
**
** Implementation of ODBC driver classes
**
** Created : 001103
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
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

#include "qsql_odbc.h"
#include <ntqsqlrecord.h>

#if defined (Q_OS_WIN32)
#include <qt_windows.h>
#include <ntqapplication.h>
#endif
#include <ntqdatetime.h>
#include <private/qsqlextension_p.h>
#include <private/qinternal_p.h>
#include <stdlib.h>

// undefine this to prevent initial check of the ODBC driver
#define ODBC_CHECK_DRIVER

#if defined(Q_ODBC_VERSION_2)
//crude hack to get non-unicode capable driver managers to work
# undef UNICODE
# define SQLTCHAR SQLCHAR
# define SQL_C_WCHAR SQL_C_CHAR
#endif

// newer platform SDKs use SQLLEN instead of SQLINTEGER
#if defined(SQLLEN) || defined(Q_OS_WIN64) || defined(Q_OS_UNIX)
# define TQSQLLEN SQLLEN
#else
# define TQSQLLEN SQLINTEGER
#endif

#if defined(SQLULEN) || defined(Q_OS_WIN64) || defined(Q_OS_UNIX)
# define TQSQLULEN SQLULEN
#else
# define TQSQLULEN SQLUINTEGER
#endif

static const TQSQLLEN COLNAMESIZE = 256;
//Map TQt parameter types to ODBC types
static const SQLSMALLINT qParamType[ 4 ] = { SQL_PARAM_INPUT, SQL_PARAM_INPUT, SQL_PARAM_OUTPUT, SQL_PARAM_INPUT_OUTPUT };

class TQODBCPrivate
{
public:
    TQODBCPrivate()
    : hEnv(0), hDbc(0), hStmt(0), useSchema(false)
    {
	sql_char_type = sql_varchar_type = sql_longvarchar_type = TQVariant::CString;
	unicode = false;
    }

    SQLHANDLE hEnv;
    SQLHANDLE hDbc;
    SQLHANDLE hStmt;

    bool unicode;
    bool useSchema;
    TQVariant::Type sql_char_type;
    TQVariant::Type sql_varchar_type;
    TQVariant::Type sql_longvarchar_type;

    TQSqlRecordInfo rInf;

    bool checkDriver() const;
    void checkUnicode();
    void checkSchemaUsage();
    bool setConnectionOptions( const TQString& connOpts );
    void splitTableQualifier(const TQString &qualifier, TQString &catalog,
			     TQString &schema, TQString &table);
};

class TQODBCPreparedExtension : public TQSqlExtension
{
public:
    TQODBCPreparedExtension( TQODBCResult * r )
	: result( r ) {}

    bool prepare( const TQString& query )
    {
	return result->prepare( query );
    }

    bool exec()
    {
	return result->exec();
    }

    TQODBCResult * result;
};

TQPtrDict<TQSqlOpenExtension> *tqSqlOpenExtDict();

class TQODBCOpenExtension : public TQSqlOpenExtension
{
public:
    TQODBCOpenExtension( TQODBCDriver *dri )
	: TQSqlOpenExtension(), driver(dri) {}
    ~TQODBCOpenExtension() {}

    bool open( const TQString& db,
	       const TQString& user,
	       const TQString& password,
	       const TQString& host,
	       int port,
	       const TQString& connOpts );
private:
    TQODBCDriver *driver;
};

bool TQODBCOpenExtension::open( const TQString& db,
			       const TQString& user,
			       const TQString& password,
			       const TQString& host,
			       int port,
			       const TQString& connOpts )
{
    return driver->open( db, user, password, host, port, connOpts );
}

static TQString qWarnODBCHandle(int handleType, SQLHANDLE handle)
{
    SQLINTEGER nativeCode_;
    SQLSMALLINT msgLen;
    SQLRETURN r = SQL_ERROR;
    SQLTCHAR state_[SQL_SQLSTATE_SIZE+1];
    SQLTCHAR description_[SQL_MAX_MESSAGE_LENGTH];
    r = SQLGetDiagRec( handleType,
			 handle,
			 1,
			 (SQLTCHAR*)state_,
			 &nativeCode_,
			 (SQLTCHAR*)description_,
			 SQL_MAX_MESSAGE_LENGTH-1, /* in bytes, not in characters */
			 &msgLen);
    if ( r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO )
#ifdef UNICODE
	return TQString( (const TQChar*)description_, (uint)msgLen );
#else
	return TQString::fromLocal8Bit( (const char*)description_ );
#endif
    return TQString::null;
}

static TQString qODBCWarn( const TQODBCPrivate* odbc)
{
    return ( qWarnODBCHandle( SQL_HANDLE_ENV, odbc->hEnv ) + " "
	     + qWarnODBCHandle( SQL_HANDLE_DBC, odbc->hDbc ) + " "
	     + qWarnODBCHandle( SQL_HANDLE_STMT, odbc->hStmt ) );
}

static void qSqlWarning( const TQString& message, const TQODBCPrivate* odbc )
{
#ifdef QT_CHECK_RANGE
    tqWarning( "%s\tError: %s", message.local8Bit().data(), qODBCWarn( odbc ).local8Bit().data() );
#endif
}

static TQSqlError qMakeError( const TQString& err, int type, const TQODBCPrivate* p )
{
    return TQSqlError( "TQODBC3: " + err, qODBCWarn(p), type );
}

static TQVariant::Type qDecodeODBCType( SQLSMALLINT sqltype, const TQODBCPrivate* p )
{
    TQVariant::Type type = TQVariant::Invalid;
    switch ( sqltype ) {
    case SQL_DECIMAL:
    case SQL_NUMERIC:
    case SQL_REAL:
    case SQL_FLOAT:
    case SQL_DOUBLE:
	type = TQVariant::Double;
	break;
    case SQL_SMALLINT:
    case SQL_INTEGER:
    case SQL_BIT:
    case SQL_TINYINT:
	type = TQVariant::Int;
	break;
    case SQL_BIGINT:
	type = TQVariant::LongLong;
	break;
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
	type = TQVariant::ByteArray;
	break;
    case SQL_DATE:
    case SQL_TYPE_DATE:
	type = TQVariant::Date;
	break;
    case SQL_TIME:
    case SQL_TYPE_TIME:
	type = TQVariant::Time;
	break;
    case SQL_TIMESTAMP:
    case SQL_TYPE_TIMESTAMP:
	type = TQVariant::DateTime;
	break;
#ifndef Q_ODBC_VERSION_2
    case SQL_WCHAR:
    case SQL_WVARCHAR:
    case SQL_WLONGVARCHAR:
	type = TQVariant::String;
	break;
#endif
    case SQL_CHAR:
	type = p->sql_char_type;
	break;
    case SQL_VARCHAR:
	type = p->sql_varchar_type;
	break;
    case SQL_LONGVARCHAR:
	type = p->sql_longvarchar_type;
	break;
    default:
	type = TQVariant::CString;
	break;
    }
    return type;
}

static TQString qGetStringData( SQLHANDLE hStmt, int column, int colSize, bool& isNull, bool unicode = false )
{
    TQString     fieldVal;
    SQLRETURN   r = SQL_ERROR;
    TQSQLLEN  lengthIndicator = 0;

    if ( colSize <= 0 ) {
	colSize = 256;
    } else if ( colSize > 65536 ) { // limit buffer size to 64 KB
	colSize = 65536;
    } else {
	colSize++; // make sure there is room for more than the 0 termination
	if ( unicode ) {
	    colSize *= 2; // a tiny bit faster, since it saves a SQLGetData() call
	}
    }
    char* buf = new char[ colSize ];
    while ( true ) {
	r = SQLGetData( hStmt,
			column+1,
			unicode ? SQL_C_WCHAR : SQL_C_CHAR,
			(SQLPOINTER)buf,
			(TQSQLLEN)colSize,
			&lengthIndicator );
	if ( r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO ) {
	    if ( lengthIndicator == SQL_NULL_DATA || lengthIndicator == SQL_NO_TOTAL ) {
		fieldVal = TQString::null;
		isNull = true;
		break;
	    }
	    // if SQL_SUCCESS_WITH_INFO is returned, indicating that
	    // more data can be fetched, the length indicator does NOT
	    // contain the number of bytes returned - it contains the
	    // total number of bytes that CAN be fetched
	    // colSize-1: remove 0 termination when there is more data to fetch
	    int rSize = (r == SQL_SUCCESS_WITH_INFO) ? (unicode ? colSize-2 : colSize-1) : lengthIndicator;
	    if ( unicode ) {
		fieldVal += TQString( (TQChar*) buf, rSize / 2 );
	    } else {
		buf[ rSize ] = 0;
		fieldVal += buf;
	    }
	    if ( lengthIndicator < colSize ) {
		// workaround for Drivermanagers that don't return SQL_NO_DATA
		break;
	    }
	} else if ( r == SQL_NO_DATA ) {
	    break;
	} else {
#ifdef QT_CHECK_RANGE
	    tqWarning( "qGetStringData: Error while fetching data (%d)", r );
#endif
	    fieldVal = TQString::null;
	    break;
	}
    }
    delete[] buf;
    return fieldVal;
}

static TQByteArray qGetBinaryData( SQLHANDLE hStmt, int column, TQSQLLEN& lengthIndicator, bool& isNull )
{
    TQByteArray fieldVal;
    SQLSMALLINT colNameLen;
    SQLSMALLINT colType;
    TQSQLULEN colSize;
    SQLSMALLINT colScale;
    SQLSMALLINT nullable;
    SQLRETURN r = SQL_ERROR;

    SQLTCHAR colName[COLNAMESIZE];
    r = SQLDescribeCol( hStmt,
			column+1,
			colName,
			COLNAMESIZE,
			&colNameLen,
			&colType,
			&colSize,
			&colScale,
			&nullable );
#ifdef QT_CHECK_RANGE
    if ( r != SQL_SUCCESS )
	tqWarning( "qGetBinaryData: Unable to describe column %d", column );
#endif
    // SQLDescribeCol may return 0 if size cannot be determined
    if (!colSize) {
	colSize = 256;
    }
    if ( colSize > 65536 ) { // read the field in 64 KB chunks
	colSize = 65536;
    }
    char * buf = new char[ colSize ];
    while ( true ) {
	r = SQLGetData( hStmt,
			column+1,
			SQL_C_BINARY,
			(SQLPOINTER) buf,
			(TQSQLLEN)colSize,
			&lengthIndicator );
	if ( r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO ) {
	    if ( lengthIndicator == SQL_NULL_DATA ) {
 		isNull = true;
		break;
	    } else {
		int rSize;
		r == SQL_SUCCESS ? rSize = lengthIndicator : rSize = colSize;
		if ( lengthIndicator == SQL_NO_TOTAL ) { // size cannot be determined
		    rSize = colSize;
		}
		// NB! This is not a memleak - the mem will be deleted by TQByteArray when
		// no longer ref'd
		char * tmp = (char *) malloc( rSize + fieldVal.size() );
		if ( fieldVal.size() ) {
		    memcpy( tmp, fieldVal.data(), fieldVal.size() );
		}
		memcpy( tmp + fieldVal.size(), buf, rSize );
		fieldVal = fieldVal.assign( tmp, fieldVal.size() + rSize );

		if ( r == SQL_SUCCESS ) { // the whole field was read in one chunk
		    break;
		}
	    }
	} else {
	    break;
	}
    }
    delete [] buf;
    return fieldVal;
}

static int qGetIntData( SQLHANDLE hStmt, int column, bool& isNull  )
{
    TQSQLLEN intbuf = 0;
    isNull = false;
    TQSQLLEN lengthIndicator = 0;
    SQLRETURN r = SQLGetData( hStmt,
			      column+1,
			      SQL_C_SLONG,
			      (SQLPOINTER)&intbuf,
			      (TQSQLLEN)0,
			      &lengthIndicator );
    if ( ( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO ) || lengthIndicator == SQL_NULL_DATA ) {
	isNull = true;
	return 0;
    }
    return (int)intbuf;
}

static double qGetDoubleData( SQLHANDLE hStmt, int column, bool& isNull )
{
    SQLDOUBLE dblbuf;
    TQSQLLEN lengthIndicator = 0;
    isNull = false;
    SQLRETURN r = SQLGetData( hStmt,
			      column+1,
			      SQL_C_DOUBLE,
			      (SQLPOINTER)&dblbuf,
			      (TQSQLLEN)0,
			      &lengthIndicator );
    if ( ( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO ) || lengthIndicator == SQL_NULL_DATA ) {
	isNull = true;
	return 0.0;
    }

    return (double) dblbuf;
}

static SQLBIGINT qGetBigIntData( SQLHANDLE hStmt, int column, bool& isNull )
{
    SQLBIGINT lngbuf = TQ_INT64_C( 0 );
    isNull = false;
    TQSQLLEN lengthIndicator = 0;
    SQLRETURN r = SQLGetData( hStmt,
			      column+1,
			      SQL_C_SBIGINT,
			      (SQLPOINTER) &lngbuf,
			      (TQSQLLEN)0,
			      &lengthIndicator );
    if ( ( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO ) || lengthIndicator == SQL_NULL_DATA )
	isNull = true;

    return lngbuf;
}

// creates a TQSqlFieldInfo from a valid hStmt generated
// by SQLColumns. The hStmt has to point to a valid position.
static TQSqlFieldInfo qMakeFieldInfo( const SQLHANDLE hStmt, const TQODBCPrivate* p )
{
    bool isNull;
    TQString fname = qGetStringData( hStmt, 3, -1, isNull, p->unicode );
    int type = qGetIntData( hStmt, 4, isNull ); // column type
    int required = qGetIntData( hStmt, 10, isNull ); // nullable-flag
    // required can be SQL_NO_NULLS, SQL_NULLABLE or SQL_NULLABLE_UNKNOWN
    if ( required == SQL_NO_NULLS ) {
	required = 1;
    } else if ( required == SQL_NULLABLE ) {
	required = 0;
    } else {
	required = -1;
    }
    int size = qGetIntData( hStmt, 6, isNull ); // column size
    int prec = qGetIntData( hStmt, 8, isNull ); // precision
    return TQSqlFieldInfo( fname, qDecodeODBCType( type, p ), required, size, prec, TQVariant(), type );
}

static TQSqlFieldInfo qMakeFieldInfo( const TQODBCPrivate* p, int i  )
{
    SQLSMALLINT colNameLen;
    SQLSMALLINT colType;
    TQSQLULEN colSize;
    SQLSMALLINT colScale;
    SQLSMALLINT nullable;
    SQLRETURN r = SQL_ERROR;
    SQLTCHAR colName[ COLNAMESIZE ];
    r = SQLDescribeCol( p->hStmt,
			i+1,
			colName,
			(TQSQLULEN)COLNAMESIZE,
			&colNameLen,
			&colType,
			&colSize,
			&colScale,
			&nullable);

    if ( r != SQL_SUCCESS ) {
#ifdef QT_CHECK_RANGE
	qSqlWarning( TQString("qMakeField: Unable to describe column %1").arg(i), p );
#endif
	return TQSqlFieldInfo();
    }
#ifdef UNICODE
    TQString qColName( (const TQChar*)colName, (uint)colNameLen );
#else
    TQString qColName = TQString::fromLocal8Bit( (const char*)colName );
#endif
    // nullable can be SQL_NO_NULLS, SQL_NULLABLE or SQL_NULLABLE_UNKNOWN
    int required = -1;
    if ( nullable == SQL_NO_NULLS ) {
	required = 1;
    } else if ( nullable == SQL_NULLABLE ) {
	required = 0;
    }
    TQVariant::Type type = qDecodeODBCType( colType, p );
    return TQSqlFieldInfo( qColName,
    			  type,
    			  required,
    			  (int)colSize == 0 ? -1 : (int)colSize,
    			  (int)colScale == 0 ? -1 : (int)colScale,
    			  TQVariant(),
    			  (int)colType );
}

bool TQODBCPrivate::setConnectionOptions( const TQString& connOpts )
{
    // Set any connection attributes
    TQStringList raw = TQStringList::split( ';', connOpts );
    TQStringList opts;
    SQLRETURN r = SQL_SUCCESS;
    TQMap<TQString, TQString> connMap;
    for ( TQStringList::ConstIterator it = raw.begin(); it != raw.end(); ++it ) {
	TQString tmp( *it );
	int idx;
	if ( (idx = tmp.find( '=' )) != -1 )
	    connMap[ tmp.left( idx ) ] = tmp.mid( idx + 1 ).simplifyWhiteSpace();
	else
	    tqWarning( "TQODBCDriver::open: Illegal connect option value '%s'", tmp.latin1() );
    }
    if ( connMap.count() ) {
	TQMap<TQString, TQString>::ConstIterator it;
	TQString opt, val;
	SQLUINTEGER v = 0;
	for ( it = connMap.begin(); it != connMap.end(); ++it ) {
	    opt = it.key().upper();
	    val = it.data().upper();
	    r = SQL_SUCCESS;
	    if ( opt == "SQL_ATTR_ACCESS_MODE" ) {
		if ( val == "SQL_MODE_READ_ONLY" ) {
		    v = SQL_MODE_READ_ONLY;
		} else if ( val == "SQL_MODE_READ_WRITE" ) {
		    v = SQL_MODE_READ_WRITE;
		} else {
		    tqWarning( "TQODBCDriver::open: Unknown option value '%s'", (*it).latin1() );
		    break;
		}
		r = SQLSetConnectAttr( hDbc, SQL_ATTR_ACCESS_MODE, (SQLPOINTER) v, 0 );
	    } else if ( opt == "SQL_ATTR_CONNECTION_TIMEOUT" ) {
		v = val.toUInt();
		r = SQLSetConnectAttr( hDbc, SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER) v, 0 );
	    } else if ( opt == "SQL_ATTR_LOGIN_TIMEOUT" ) {
		v = val.toUInt();
		r = SQLSetConnectAttr( hDbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER) v, 0 );
	    } else if ( opt == "SQL_ATTR_CURRENT_CATALOG" ) {
		val.ucs2(); // 0 terminate
		r = SQLSetConnectAttr( hDbc, SQL_ATTR_CURRENT_CATALOG,
#ifdef UNICODE
				       (SQLWCHAR*) val.unicode(),
#else
				       (SQLCHAR*) val.latin1(),
#endif
				       SQL_NTS );
	    } else if ( opt == "SQL_ATTR_METADATA_ID" ) {
		if ( val == "SQL_TRUE" ) {
		    v = SQL_TRUE;
		} else if ( val == "SQL_FALSE" ) {
		    v = SQL_FALSE;
		} else {
		    tqWarning( "TQODBCDriver::open: Unknown option value '%s'", (*it).latin1() );
		    break;
		}
		r = SQLSetConnectAttr( hDbc, SQL_ATTR_METADATA_ID, (SQLPOINTER) v, 0 );
	    } else if ( opt == "SQL_ATTR_PACKET_SIZE" ) {
		v = val.toUInt();
		r = SQLSetConnectAttr( hDbc, SQL_ATTR_PACKET_SIZE, (SQLPOINTER) v, 0 );
	    } else if ( opt == "SQL_ATTR_TRACEFILE" ) {
		val.ucs2(); // 0 terminate
		r = SQLSetConnectAttr( hDbc, SQL_ATTR_TRACEFILE,
#ifdef UNICODE
				       (SQLWCHAR*) val.unicode(),
#else
				       (SQLCHAR*) val.latin1(),
#endif
				       SQL_NTS );
	    } else if ( opt == "SQL_ATTR_TRACE" ) {
		if ( val == "SQL_OPT_TRACE_OFF" ) {
		    v = SQL_OPT_TRACE_OFF;
		} else if ( val == "SQL_OPT_TRACE_ON" ) {
		    v = SQL_OPT_TRACE_ON;
		} else {
		    tqWarning( "TQODBCDriver::open: Unknown option value '%s'", (*it).latin1() );
		    break;
		}
		r = SQLSetConnectAttr( hDbc, SQL_ATTR_TRACE, (SQLPOINTER) v, 0 );
	    }
#ifdef QT_CHECK_RANGE
              else {
		  tqWarning( "TQODBCDriver::open: Unknown connection attribute '%s'", opt.latin1() );
	    }
#endif
	    if ( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO ) {
#ifdef QT_CHECK_RANGE
		qSqlWarning( TQString("TQODBCDriver::open: Unable to set connection attribute '%1'").arg( opt ), this );
#endif
		return false;
	    }
	}
    }
    return true;
}

void TQODBCPrivate::splitTableQualifier(const TQString & qualifier, TQString &catalog,
				       TQString &schema, TQString &table)
{
    if (!useSchema) {
	table = qualifier;
	return;
    }
    TQStringList l = TQStringList::split( ".", qualifier, true );
    if ( l.count() > 3 )
	return; // can't possibly be a valid table qualifier
    int i = 0, n = l.count();
    if ( n == 1 ) {
	table = qualifier;
    } else {
	for ( TQStringList::Iterator it = l.begin(); it != l.end(); ++it ) {
	    if ( n == 3 ) {
		if ( i == 0 ) {
		    catalog = *it;
		} else if ( i == 1 ) {
		    schema = *it;
		} else if ( i == 2 ) {
		    table = *it;
		}
	    } else if ( n == 2 ) {
		if ( i == 0 ) {
		    schema = *it;
		} else if ( i == 1 ) {
		    table = *it;
		}
	    }
	    i++;
	}
    }
}

////////////////////////////////////////////////////////////////////////////

TQODBCResult::TQODBCResult( const TQODBCDriver * db, TQODBCPrivate* p )
: TQSqlResult(db)
{
    d = new TQODBCPrivate();
    (*d) = (*p);
    setExtension( new TQODBCPreparedExtension( this ) );
}

TQODBCResult::~TQODBCResult()
{
    if ( d->hStmt && driver()->isOpen() ) {
	SQLRETURN r = SQLFreeHandle( SQL_HANDLE_STMT, d->hStmt );
#ifdef QT_CHECK_RANGE
	if ( r != SQL_SUCCESS )
	    qSqlWarning( "TQODBCDriver: Unable to free statement handle " + TQString::number(r), d );
#endif
    }

    delete d;
}

bool TQODBCResult::reset ( const TQString& query )
{
    setActive( false );
    setAt( TQSql::BeforeFirst );
    SQLRETURN r;

    d->rInf.clear();
    // Always reallocate the statement handle - the statement attributes
    // are not reset if SQLFreeStmt() is called which causes some problems.
    if ( d->hStmt ) {
	r = SQLFreeHandle( SQL_HANDLE_STMT, d->hStmt );
	if ( r != SQL_SUCCESS ) {
#ifdef QT_CHECK_RANGE
	    qSqlWarning( "TQODBCResult::reset: Unable to free statement handle", d );
#endif
	    return false;
	}
    }
    r  = SQLAllocHandle( SQL_HANDLE_STMT,
			 d->hDbc,
			 &d->hStmt );
    if ( r != SQL_SUCCESS ) {
#ifdef QT_CHECK_RANGE
	qSqlWarning( "TQODBCResult::reset: Unable to allocate statement handle", d );
#endif
	return false;
    }

    if ( isForwardOnly() ) {
	r = SQLSetStmtAttr( d->hStmt,
			    SQL_ATTR_CURSOR_TYPE,
			    (SQLPOINTER)SQL_CURSOR_FORWARD_ONLY,
			    SQL_IS_UINTEGER );
    } else {
	r = SQLSetStmtAttr( d->hStmt,
			    SQL_ATTR_CURSOR_TYPE,
			    (SQLPOINTER)SQL_CURSOR_STATIC,
			    SQL_IS_UINTEGER );
    }
    if ( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO ) {
#ifdef QT_CHECK_RANGE
	qSqlWarning( "TQODBCResult::reset: Unable to set 'SQL_CURSOR_STATIC' as statement attribute. Please check your ODBC driver configuration", d );
#endif
	return false;
    }

#ifdef UNICODE
    r = SQLExecDirect( d->hStmt,
		       (SQLWCHAR*) query.unicode(),
		       (SQLINTEGER) query.length() );
#else
    TQCString query8 = query.local8Bit();
    r = SQLExecDirect( d->hStmt,
                       (SQLCHAR*) query8.data(),
                       (SQLINTEGER) query8.length() );
#endif
    if ( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO ) {
	setLastError( qMakeError( "Unable to execute statement", TQSqlError::Statement, d ) );
	return false;
    }
    SQLSMALLINT count;
    r = SQLNumResultCols( d->hStmt, &count );
    if ( count ) {
	setSelect( true );
	for ( int i = 0; i < count; ++i ) {
	    d->rInf.append( qMakeFieldInfo( d, i ) );
	}
    } else {
	setSelect( false );
    }
    setActive( true );
    return true;
}

bool TQODBCResult::fetch(int i)
{
    if ( isForwardOnly() && i < at() )
	return false;
    if ( i == at() )
	return true;
    fieldCache.clear();
    nullCache.clear();
    int actualIdx = i + 1;
    if ( actualIdx <= 0 ) {
	setAt( TQSql::BeforeFirst );
	return false;
    }
    SQLRETURN r;
    if ( isForwardOnly() ) {
	bool ok = true;
	while ( ok && i > at() )
	    ok = fetchNext();
	return ok;
    } else {
	r = SQLFetchScroll( d->hStmt,
			    SQL_FETCH_ABSOLUTE,
			    actualIdx );
    }
    if ( r != SQL_SUCCESS ){
 	return false;
    }
    setAt( i );
    return true;
}

bool TQODBCResult::fetchNext()
{
    SQLRETURN r;
    fieldCache.clear();
    nullCache.clear();
    r = SQLFetchScroll( d->hStmt,
		       SQL_FETCH_NEXT,
		       0 );
    if ( r != SQL_SUCCESS )
	return false;
    setAt( at() + 1 );
    return true;
}

bool TQODBCResult::fetchFirst()
{
    if ( isForwardOnly() && at() != TQSql::BeforeFirst )
	return false;
    SQLRETURN r;
    fieldCache.clear();
    nullCache.clear();
    if ( isForwardOnly() ) {
	return fetchNext();
    }
    r = SQLFetchScroll( d->hStmt,
		       SQL_FETCH_FIRST,
		       0 );
    if ( r != SQL_SUCCESS )
	return false;
    setAt( 0 );
    return true;
}

bool TQODBCResult::fetchPrior()
{
    if ( isForwardOnly() )
	return false;
    SQLRETURN r;
    fieldCache.clear();
    nullCache.clear();
    r = SQLFetchScroll( d->hStmt,
		       SQL_FETCH_PRIOR,
		       0 );
    if ( r != SQL_SUCCESS )
	return false;
    setAt( at() - 1 );
    return true;
}

bool TQODBCResult::fetchLast()
{
    SQLRETURN r;
    fieldCache.clear();
    nullCache.clear();

    if ( isForwardOnly() ) {
	// cannot seek to last row in forwardOnly mode, so we have to use brute force
	int i = at();
	if ( i == TQSql::AfterLast )
	    return false;
	if ( i == TQSql::BeforeFirst )
	    i = 0;
	while ( fetchNext() )
	    ++i;
	setAt( i );
	return true;
    }

    r = SQLFetchScroll( d->hStmt,
		       SQL_FETCH_LAST,
		       0 );
    if ( r != SQL_SUCCESS ) {
	return false;
    }
    SQLINTEGER currRow;
    r = SQLGetStmtAttr( d->hStmt,
			SQL_ROW_NUMBER,
			&currRow,
			SQL_IS_INTEGER,
			0 );
    if ( r != SQL_SUCCESS )
	return false;
    setAt( currRow-1 );
    return true;
}

TQVariant TQODBCResult::data( int field )
{
    if ( field >= (int) d->rInf.count() ) {
	tqWarning( "TQODBCResult::data: column %d out of range", field );
	return TQVariant();
    }
    if ( fieldCache.contains( field ) )
	return fieldCache[ field ];
    SQLRETURN r(0);
    TQSQLLEN lengthIndicator = 0;
    bool isNull = false;
    int current = fieldCache.count();
    for ( ; current < (field + 1); ++current ) {
	const TQSqlFieldInfo info = d->rInf[ current ];
	switch ( info.type() ) {
	case TQVariant::LongLong:
	    fieldCache[ current ] = TQVariant( (TQ_LLONG) qGetBigIntData( d->hStmt, current, isNull ) );
	    nullCache[ current ] = isNull;
	    break;
	case TQVariant::Int:
	    fieldCache[ current ] = TQVariant( qGetIntData( d->hStmt, current, isNull ) );
	    nullCache[ current ] = isNull;
	    break;
	case TQVariant::Date:
	    DATE_STRUCT dbuf;
	    r = SQLGetData( d->hStmt,
			    current+1,
			    SQL_C_DATE,
			    (SQLPOINTER)&dbuf,
			    (TQSQLLEN)0,
			    &lengthIndicator );
	    if ( ( r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO ) && ( lengthIndicator != SQL_NULL_DATA ) ) {
		fieldCache[ current ] = TQVariant( TQDate( dbuf.year, dbuf.month, dbuf.day ) );
		nullCache[ current ] = false;
	    } else {
		fieldCache[ current ] = TQVariant( TQDate() );
		nullCache[ current ] = true;
	    }
	    break;
	case TQVariant::Time:
	    TIME_STRUCT tbuf;
	    r = SQLGetData( d->hStmt,
			    current+1,
			    SQL_C_TIME,
			    (SQLPOINTER)&tbuf,
			    (TQSQLLEN)0,
			    &lengthIndicator );
	    if ( ( r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO ) && ( lengthIndicator != SQL_NULL_DATA ) ) {
		fieldCache[ current ] = TQVariant( TQTime( tbuf.hour, tbuf.minute, tbuf.second ) );
		nullCache[ current ] = false;
	    } else {
		fieldCache[ current ] = TQVariant( TQTime() );
		nullCache[ current ] = true;
	    }
	    break;
	case TQVariant::DateTime:
	    TIMESTAMP_STRUCT dtbuf;
	    r = SQLGetData( d->hStmt,
			    current+1,
			    SQL_C_TIMESTAMP,
			    (SQLPOINTER)&dtbuf,
			    (TQSQLLEN)0,
			    &lengthIndicator );
	    if ( ( r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO ) && ( lengthIndicator != SQL_NULL_DATA ) ) {
		fieldCache[ current ] = TQVariant( TQDateTime( TQDate( dtbuf.year, dtbuf.month, dtbuf.day ), TQTime( dtbuf.hour, dtbuf.minute, dtbuf.second, dtbuf.fraction / 1000000 ) ) );
		nullCache[ current ] = false;
	    } else {
		fieldCache[ current ] = TQVariant( TQDateTime() );
		nullCache[ current ] = true;
	    }
	    break;
        case TQVariant::ByteArray: {
	    isNull = false;
	    TQByteArray val = qGetBinaryData( d->hStmt, current, lengthIndicator, isNull );
	    fieldCache[ current ] = TQVariant( val );
	    nullCache[ current ] = isNull;
	    break; }
	case TQVariant::String:
	    isNull = false;
	    fieldCache[ current ] = TQVariant( qGetStringData( d->hStmt, current,
					      info.length(), isNull, true ) );
	    nullCache[ current ] = isNull;
	    break;
	case TQVariant::Double:
	    if ( info.typeID() == SQL_DECIMAL || info.typeID() == SQL_NUMERIC )
		// bind Double values as string to prevent loss of precision
		fieldCache[ current ] = TQVariant( qGetStringData( d->hStmt, current,
						  info.length() + 1, isNull, false ) ); // length + 1 for the comma
	    else
		fieldCache[ current ] = TQVariant( qGetDoubleData( d->hStmt, current, isNull ) );
	    nullCache[ current ] = isNull;
	    break;
	case TQVariant::CString:
	default:
	    isNull = false;
	    fieldCache[ current ] = TQVariant( qGetStringData( d->hStmt, current,
					      info.length(), isNull, false ) );
	    nullCache[ current ] = isNull;
	    break;
	}
    }
    return fieldCache[ --current ];
}

bool TQODBCResult::isNull( int field )
{
    if ( !fieldCache.contains( field ) ) {
	// since there is no good way to find out whether the value is NULL
	// without fetching the field we'll fetch it here.
	// (data() also sets the NULL flag)
	data( field );
    }
    return nullCache[ field ];
}

int TQODBCResult::size()
{
    return -1;
}

int TQODBCResult::numRowsAffected()
{
    TQSQLLEN affectedRowCount(0);
    SQLRETURN r = SQLRowCount( d->hStmt, &affectedRowCount );
    if ( r == SQL_SUCCESS )
	return affectedRowCount;
#ifdef QT_CHECK_RANGE
    else
	qSqlWarning( "TQODBCResult::numRowsAffected: Unable to count affected rows", d );
#endif
    return -1;
}

bool TQODBCResult::prepare( const TQString& query )
{
    setActive( false );
    setAt( TQSql::BeforeFirst );
    SQLRETURN r;

    d->rInf.clear();
    if ( d->hStmt ) {
	r = SQLFreeHandle( SQL_HANDLE_STMT, d->hStmt );
	if ( r != SQL_SUCCESS ) {
#ifdef QT_CHECK_RANGE
	    qSqlWarning( "TQODBCResult::prepare: Unable to close statement", d );
#endif
	    return false;
	}
    }
    r  = SQLAllocHandle( SQL_HANDLE_STMT,
	    		 d->hDbc,
			 &d->hStmt );
    if ( r != SQL_SUCCESS ) {
#ifdef QT_CHECK_RANGE
	qSqlWarning( "TQODBCResult::prepare: Unable to allocate statement handle", d );
#endif
	return false;
    }

    if ( isForwardOnly() ) {
	r = SQLSetStmtAttr( d->hStmt,
	    		    SQL_ATTR_CURSOR_TYPE,
			    (SQLPOINTER)SQL_CURSOR_FORWARD_ONLY,
			    SQL_IS_UINTEGER );
    } else {
	r = SQLSetStmtAttr( d->hStmt,
			    SQL_ATTR_CURSOR_TYPE,
			    (SQLPOINTER)SQL_CURSOR_STATIC,
			    SQL_IS_UINTEGER );
    }
    if ( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO ) {
#ifdef QT_CHECK_RANGE
	qSqlWarning( "TQODBCResult::prepare: Unable to set 'SQL_CURSOR_STATIC' as statement attribute. Please check your ODBC driver configuration", d );
#endif
	return false;
    }

#ifdef UNICODE
    r = SQLPrepare( d->hStmt,
		    (SQLWCHAR*) query.unicode(),
		    (SQLINTEGER) query.length() );
#else
    TQCString query8 = query.local8Bit();
    r = SQLPrepare( d->hStmt,
		    (SQLCHAR*) query8.data(),
		    (SQLINTEGER) query8.length() );
#endif

    if ( r != SQL_SUCCESS ) {
#ifdef QT_CHECK_RANGE
	qSqlWarning( "TQODBCResult::prepare: Unable to prepare statement", d );
#endif
	return false;
    }
    return true;
}

bool TQODBCResult::exec()
{
    SQLRETURN r;
    TQPtrList<TQVirtualDestructor> tmpStorage; // holds temporary ptrs. which will be deleted on fu exit
    tmpStorage.setAutoDelete( true );

    setActive( false );
    setAt( TQSql::BeforeFirst );
    d->rInf.clear();

    if ( !d->hStmt ) {
#ifdef QT_CHECK_RANGE
	qSqlWarning( "TQODBCResult::exec: No statement handle available", d );
#endif
	return false;
    } else {
	r = SQLFreeStmt( d->hStmt, SQL_CLOSE );
	if ( r != SQL_SUCCESS ) {
	    qSqlWarning( "TQODBCResult::exec: Unable to close statement handle", d );
	    return false;
	}
    }

    // bind parameters - only positional binding allowed
    if ( extension()->index.count() > 0 ) {
	TQMap<int, TQString>::Iterator it;
	int para = 1;
	TQVariant val;
	for ( it = extension()->index.begin(); it != extension()->index.end(); ++it ) {
	    val = extension()->values[ it.data() ].value;
	    TQSQLLEN *ind = new TQSQLLEN( SQL_NTS );
	    tmpStorage.append( qAutoDeleter(ind) );
	    if ( val.isNull() ) {
		*ind = SQL_NULL_DATA;
	    }
	    switch ( val.type() ) {
		case TQVariant::Date: {
		    DATE_STRUCT * dt = new DATE_STRUCT;
		    tmpStorage.append( qAutoDeleter(dt) );
		    TQDate qdt = val.toDate();
		    dt->year = qdt.year();
		    dt->month = qdt.month();
		    dt->day = qdt.day();
		    r = SQLBindParameter( d->hStmt,
					  para,
					  qParamType[ (int)extension()->values[ it.data() ].typ ],
					  SQL_C_DATE,
					  SQL_DATE,
					  0,
					  0,
					  (void *) dt,
					  (TQSQLLEN)0,
					  *ind == SQL_NULL_DATA ? ind : NULL );
		    break; }
		case TQVariant::Time: {
		    TIME_STRUCT * dt = new TIME_STRUCT;
		    tmpStorage.append( qAutoDeleter(dt) );
		    TQTime qdt = val.toTime();
		    dt->hour = qdt.hour();
		    dt->minute = qdt.minute();
		    dt->second = qdt.second();
		    r = SQLBindParameter( d->hStmt,
					  para,
					  qParamType[ (int)extension()->values[ it.data() ].typ ],
					  SQL_C_TIME,
					  SQL_TIME,
					  0,
					  0,
					  (void *) dt,
					  (TQSQLLEN)0,
	        			  *ind == SQL_NULL_DATA ? ind : NULL );
		    break; }
		case TQVariant::DateTime: {
		    TIMESTAMP_STRUCT * dt = new TIMESTAMP_STRUCT;
		    tmpStorage.append( qAutoDeleter(dt) );
		    TQDateTime qdt = val.toDateTime();
		    dt->year = qdt.date().year();
		    dt->month = qdt.date().month();
		    dt->day = qdt.date().day();
		    dt->hour = qdt.time().hour();
		    dt->minute = qdt.time().minute();
		    dt->second = qdt.time().second();
		    dt->fraction = 0;
		    r = SQLBindParameter( d->hStmt,
					  para,
					  qParamType[ (int)extension()->values[ it.data() ].typ ],
					  SQL_C_TIMESTAMP,
					  SQL_TIMESTAMP,
					  0,
					  0,
					  (void *) dt,
					  (TQSQLLEN)0,
					  *ind == SQL_NULL_DATA ? ind : NULL );
		    break; }
	        case TQVariant::Int: {
		    int * v = new int( val.toInt() );
		    tmpStorage.append( qAutoDeleter(v) );
		    r = SQLBindParameter( d->hStmt,
					  para,
					  qParamType[ (int)extension()->values[ it.data() ].typ ],
					  SQL_C_SLONG,
					  SQL_INTEGER,
					  0,
					  0,
					  (void *) v,
					  (TQSQLLEN)0,
					  *ind == SQL_NULL_DATA ? ind : NULL );
		    break; }
	        case TQVariant::Double: {
		    double * v = new double( val.toDouble() );
		    tmpStorage.append( qAutoDeleter(v) );
		    r = SQLBindParameter( d->hStmt,
					  para,
					  qParamType[ (int)extension()->values[ it.data() ].typ ],
					  SQL_C_DOUBLE,
					  SQL_DOUBLE,
					  0,
					  0,
					  (void *) v,
					  (TQSQLLEN)0,
					  *ind == SQL_NULL_DATA ? ind : NULL );
		    break; }
	        case TQVariant::ByteArray: {
		    if ( *ind != SQL_NULL_DATA ) {
			*ind = val.asByteArray().size();
		    }
		    r = SQLBindParameter( d->hStmt,
					  para,
					  qParamType[ (int)extension()->values[ it.data() ].typ ],
					  SQL_C_BINARY,
					  SQL_LONGVARBINARY,
					  val.asByteArray().size(),
					  0,
					  (void *) val.asByteArray().data(),
					  (TQSQLLEN)val.asByteArray().size(),
					  ind );
		    break; }
#ifndef Q_ODBC_VERSION_2
	        case TQVariant::String:
		    if ( d->unicode ) {
			TQString * str = new TQString( val.asString() );
			str->ucs2();
			int len = str->length()*2;
			tmpStorage.append( qAutoDeleter(str) );
			r = SQLBindParameter( d->hStmt,
					      para,
					      qParamType[ (int)extension()->values[ it.data() ].typ ],
					      SQL_C_WCHAR,
					      len > 8000 ? SQL_WLONGVARCHAR : SQL_WVARCHAR,
					      len > 8000 ? len : 0,
					      0,
					      (void *) str->unicode(),
					      (TQSQLLEN) len,
					      ind );
			break;
		    }
#endif
		    // fall through
	        default: {
		    TQCString * str = new TQCString( val.asString().local8Bit() );
		    tmpStorage.append( qAutoDeleter(str) );
		    r = SQLBindParameter( d->hStmt,
					  para,
					  qParamType[ (int)extension()->values[ it.data() ].typ ],
					  SQL_C_CHAR,
					  str->length() > 4000 ? SQL_LONGVARCHAR : SQL_VARCHAR,
					  str->length() + 1,
					  0,
					  (void *) str->data(),
					  (TQSQLLEN)(str->length() + 1),
					  ind );
		    break; }
	    }
	    para++;
	    if ( r != SQL_SUCCESS ) {
#ifdef QT_CHECK_RANGE
		tqWarning( "TQODBCResult::exec: unable to bind variable: %s", qODBCWarn( d ).local8Bit().data() );
#endif
		setLastError( qMakeError( "Unable to bind variable", TQSqlError::Statement, d ) );
		return false;
	    }
	}
    }
    r = SQLExecute( d->hStmt );
    if ( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQODBCResult::exec: Unable to execute statement: %s", qODBCWarn( d ).local8Bit().data() );
#endif
	setLastError( qMakeError( "Unable to execute statement", TQSqlError::Statement, d ) );
	return false;
    }
    SQLSMALLINT count;
    r = SQLNumResultCols( d->hStmt, &count );
    if ( count ) {
	setSelect( true );
	for ( int i = 0; i < count; ++i ) {
	    d->rInf.append( qMakeFieldInfo( d, i ) );
	}
    } else {
	setSelect( false );
    }
    setActive( true );

    //get out parameters
    if ( extension()->index.count() > 0 ) {
	TQMap<int, TQString>::Iterator it;
	for ( it = extension()->index.begin(); it != extension()->index.end(); ++it ) {

	    SQLINTEGER* indPtr = qAutoDeleterData( (TQAutoDeleter<SQLINTEGER>*)tmpStorage.getFirst() );
	    if ( !indPtr )
		return false;
	    bool isNull = (*indPtr == SQL_NULL_DATA);
	    tmpStorage.removeFirst();

	    TQVariant::Type type = extension()->values[ it.data() ].value.type();
	    if ( isNull ) {
		TQVariant v;
		v.cast(type);
		extension()->values[ it.data() ].value = v;
		if (type != TQVariant::ByteArray)
		    tmpStorage.removeFirst();
		continue;
	    }

	    switch (type) {
		case TQVariant::Date: {
		    DATE_STRUCT * ds = qAutoDeleterData( (TQAutoDeleter<DATE_STRUCT>*)tmpStorage.getFirst() );
		    extension()->values[ it.data() ].value = TQVariant( TQDate( ds->year, ds->month, ds->day ) );
		    break; }
		case TQVariant::Time: {
		    TIME_STRUCT * dt = qAutoDeleterData( (TQAutoDeleter<TIME_STRUCT>*)tmpStorage.getFirst() );
		    extension()->values[ it.data() ].value = TQVariant( TQTime( dt->hour, dt->minute, dt->second ) );
		    break; }
		case TQVariant::DateTime: {
		    TIMESTAMP_STRUCT * dt = qAutoDeleterData( (TQAutoDeleter<TIMESTAMP_STRUCT>*)tmpStorage.getFirst() );
		    extension()->values[ it.data() ].value = TQVariant( TQDateTime( TQDate( dt->year, dt->month, dt->day ),
										  TQTime( dt->hour, dt->minute, dt->second ) ) );
		    break; }
	        case TQVariant::Int: {
		    int * v = qAutoDeleterData( (TQAutoDeleter<int>*)tmpStorage.getFirst() );
		    extension()->values[ it.data() ].value = TQVariant( *v );
		    break; }
	        case TQVariant::Double: {
		    double * v = qAutoDeleterData( (TQAutoDeleter<double>*)tmpStorage.getFirst() );
		    extension()->values[ it.data() ].value = TQVariant( *v );
		    break; }
	        case TQVariant::ByteArray:
		    break;
	        case TQVariant::String:
		    if ( d->unicode ) {
			TQString * str = qAutoDeleterData( (TQAutoDeleter<TQString>*)tmpStorage.getFirst() );
			extension()->values[ it.data() ].value = TQVariant( *str );
			break;
		    }
		    // fall through
	        default: {
		    TQCString * str = qAutoDeleterData( (TQAutoDeleter<TQCString>*)tmpStorage.getFirst() );
		    extension()->values[ it.data() ].value = TQVariant( *str );
		    break; }
	    }
	    if (type != TQVariant::ByteArray)
		tmpStorage.removeFirst();
	}
    }

    return true;
}

////////////////////////////////////////


TQODBCDriver::TQODBCDriver( TQObject * parent, const char * name )
    : TQSqlDriver(parent,name ? name : "TQODBC")
{
    init();
}

TQODBCDriver::TQODBCDriver( SQLHANDLE env, SQLHANDLE con, TQObject * parent, const char * name )
    : TQSqlDriver(parent,name ? name : "TQODBC")
{
    init();
    d->hEnv = env;
    d->hDbc = con;
    if ( env && con ) {
	setOpen( true );
	setOpenError( false );
    }
}

void TQODBCDriver::init()
{
    tqSqlOpenExtDict()->insert( this, new TQODBCOpenExtension(this) );
    d = new TQODBCPrivate();
}

TQODBCDriver::~TQODBCDriver()
{
    cleanup();
    delete d;
    if ( !tqSqlOpenExtDict()->isEmpty() ) {
	TQSqlOpenExtension *ext = tqSqlOpenExtDict()->take( this );
	delete ext;
    }
}

bool TQODBCDriver::hasFeature( DriverFeature f ) const
{
    switch ( f ) {
    case Transactions: {
	if ( !d->hDbc )
	    return false;
	SQLUSMALLINT txn;
	SQLSMALLINT t;
	int r = SQLGetInfo( d->hDbc,
			(SQLUSMALLINT)SQL_TXN_CAPABLE,
			&txn,
			sizeof(txn),
			&t);
	if ( r != SQL_SUCCESS || txn == SQL_TC_NONE )
	    return false;
	else
	    return true;
    }
    case QuerySize:
	return false;
    case BLOB:
	return true;
    case Unicode:
	return d->unicode;
    case PreparedQueries:
	return true;
    case PositionalPlaceholders:
	return true;
    default:
	return false;
    }
}

bool TQODBCDriver::open( const TQString&,
			const TQString&,
			const TQString&,
			const TQString&,
			int )
{
    tqWarning("TQODBCDriver::open(): This version of open() is no longer supported." );
    return false;
}

bool TQODBCDriver::open( const TQString & db,
			const TQString & user,
			const TQString & password,
			const TQString &,
			int,
			const TQString& connOpts )
{
    if ( isOpen() )
      close();
    SQLRETURN r;
    r = SQLAllocHandle( SQL_HANDLE_ENV,
			SQL_NULL_HANDLE,
			&d->hEnv);
    if ( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO ) {
#ifdef QT_CHECK_RANGE
	qSqlWarning( "TQODBCDriver::open: Unable to allocate environment", d );
#endif
	setOpenError( true );
	return false;
    }
    r = SQLSetEnvAttr( d->hEnv,
		       SQL_ATTR_ODBC_VERSION,
		       (SQLPOINTER)SQL_OV_ODBC2,
		       SQL_IS_UINTEGER );
    r = SQLAllocHandle( SQL_HANDLE_DBC,
		        d->hEnv,
		        &d->hDbc);
    if ( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO ) {
#ifdef QT_CHECK_RANGE
	qSqlWarning( "TQODBCDriver::open: Unable to allocate connection", d );
#endif
	setOpenError( true );
	return false;
    }

    if ( !d->setConnectionOptions( connOpts ) )
	return false;

    // Create the connection string
    TQString connTQStr;
    // support the "DRIVER={SQL SERVER};SERVER=blah" syntax
    if ( db.contains(".dsn") )
	connTQStr = "FILEDSN=" + db;
    else if ( db.contains( "DRIVER" ) || db.contains( "SERVER" ) )
	connTQStr = db;
    else
	connTQStr = "DSN=" + db;
    connTQStr += ";UID=" + user + ";PWD=" + password;
    SQLSMALLINT cb;
    SQLTCHAR connOut[1024];
    r = SQLDriverConnect( d->hDbc,
			  NULL,
#ifdef UNICODE
			  (SQLWCHAR*)connTQStr.unicode(),
#else
			  (SQLCHAR*)connTQStr.latin1(),
#endif
			  (SQLSMALLINT)connTQStr.length(),
			  connOut,
			  1024,
			  &cb,
			  SQL_DRIVER_NOPROMPT );
    if ( r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO ) {
	setLastError( qMakeError( "Unable to connect", TQSqlError::Connection, d ) );
	setOpenError( true );
	return false;
    }

    if ( !d->checkDriver() ) {
	setLastError( qMakeError( "Unable to connect - Driver doesn't support all needed functionality", TQSqlError::Connection, d ) );
	setOpenError( true );
	return false;
    }

    d->checkUnicode();
    d->checkSchemaUsage();

    setOpen( true );
    setOpenError( false );
    return true;
}

void TQODBCDriver::close()
{
    cleanup();
    setOpen( false );
    setOpenError( false );
}

bool TQODBCDriver::ping()
{
    // FIXME
    // Implement ping if supported
    return true;
}

void TQODBCDriver::cleanup()
{
    SQLRETURN r;
    if ( !d )
	return;

    if( d->hDbc ) {
	// Open statements/descriptors handles are automatically cleaned up by SQLDisconnect
	if ( isOpen() ) {
	    r = SQLDisconnect( d->hDbc );
#ifdef QT_CHECK_RANGE
	    if ( r != SQL_SUCCESS )
		qSqlWarning( "TQODBCDriver::disconnect: Unable to disconnect datasource", d );
#endif
	}

	r = SQLFreeHandle( SQL_HANDLE_DBC, d->hDbc );
#ifdef QT_CHECK_RANGE
	if ( r != SQL_SUCCESS )
	    qSqlWarning( "TQODBCDriver::cleanup: Unable to free connection handle", d );
#endif
	d->hDbc = 0;
    }

    if ( d->hEnv ) {
	r = SQLFreeHandle( SQL_HANDLE_ENV, d->hEnv );
#ifdef QT_CHECK_RANGE
	if ( r != SQL_SUCCESS )
	    qSqlWarning( "TQODBCDriver::cleanup: Unable to free environment handle", d );
#endif
	d->hEnv = 0;
    }
}

// checks whether the server can return char, varchar and longvarchar
// as two byte unicode characters
void TQODBCPrivate::checkUnicode()
{
#if defined(TQ_WS_WIN)
    if ( !qt_winunicode ) {
	unicode = false;
	return;
    }
#endif
    SQLRETURN   r;
    SQLUINTEGER fFunc;

    unicode = false;
    r = SQLGetInfo( hDbc,
		    SQL_CONVERT_CHAR,
		    (SQLPOINTER)&fFunc,
		    sizeof(fFunc),
		    NULL );
    if ( ( r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO ) && ( fFunc & SQL_CVT_WCHAR ) ) {
	sql_char_type = TQVariant::String;
	unicode = true;
    }

    r = SQLGetInfo( hDbc,
		    SQL_CONVERT_VARCHAR,
		    (SQLPOINTER)&fFunc,
		    sizeof(fFunc),
		    NULL );
    if ( ( r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO ) && ( fFunc & SQL_CVT_WVARCHAR ) ) {
	sql_varchar_type = TQVariant::String;
	unicode = true;
    }

    r = SQLGetInfo( hDbc,
		    SQL_CONVERT_LONGVARCHAR,
		    (SQLPOINTER)&fFunc,
		    sizeof(fFunc),
		    NULL );
    if ( ( r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO ) && ( fFunc & SQL_CVT_WLONGVARCHAR ) ) {
	sql_longvarchar_type = TQVariant::String;
	unicode = true;
    }
}

bool TQODBCPrivate::checkDriver() const
{
#ifdef ODBC_CHECK_DRIVER
    // do not query for SQL_API_SQLFETCHSCROLL because it can't be used at this time
    static const SQLUSMALLINT reqFunc[] = {
		SQL_API_SQLDESCRIBECOL, SQL_API_SQLGETDATA, SQL_API_SQLCOLUMNS,
		SQL_API_SQLGETSTMTATTR, SQL_API_SQLGETDIAGREC, SQL_API_SQLEXECDIRECT,
		SQL_API_SQLGETINFO, SQL_API_SQLTABLES, 0
    };

    // these functions are optional
    static const SQLUSMALLINT optFunc[] = {
	SQL_API_SQLNUMRESULTCOLS, SQL_API_SQLROWCOUNT, 0
    };

    SQLRETURN r;
    SQLUSMALLINT sup;


    int i;
    // check the required functions
    for ( i = 0; reqFunc[ i ] != 0; ++i ) {

	r = SQLGetFunctions( hDbc, reqFunc[ i ], &sup );

#ifdef QT_CHECK_RANGE
        if ( r != SQL_SUCCESS ) {
	    qSqlWarning( "TQODBCDriver::checkDriver: Cannot get list of supported functions", this );
	    return false;
	}
#endif
	if ( sup == SQL_FALSE ) {
#ifdef QT_CHECK_RANGE
	    tqWarning ( "TQODBCDriver::open: Warning - Driver doesn't support all needed functionality (%d). "
		       "Please look at the TQt SQL Module Driver documentation for more information.", reqFunc[ i ] );
#endif
	    return false;
	}
    }

    // these functions are optional and just generate a warning
    for ( i = 0; optFunc[ i ] != 0; ++i ) {

	r = SQLGetFunctions( hDbc, optFunc[ i ], &sup );

#ifdef QT_CHECK_RANGE
        if ( r != SQL_SUCCESS ) {
	    qSqlWarning( "TQODBCDriver::checkDriver: Cannot get list of supported functions", this );
	    return false;
	}
#endif
	if ( sup == SQL_FALSE ) {
#ifdef QT_CHECK_RANGE
	    tqWarning( "TQODBCDriver::checkDriver: Warning - Driver doesn't support some non-critical functions (%d)", optFunc[ i ] );
#endif
	    return true;
	}
    }
#endif //ODBC_CHECK_DRIVER

    return true;
}

void TQODBCPrivate::checkSchemaUsage()
{
    SQLRETURN   r;
    SQLUINTEGER val;

    r = SQLGetInfo(hDbc,
		   SQL_SCHEMA_USAGE,
		   (SQLPOINTER) &val,
		   sizeof(val),
		   NULL);
    if (r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO)
	useSchema = (val != 0);
}

TQSqlQuery TQODBCDriver::createQuery() const
{
    return TQSqlQuery( new TQODBCResult( this, d ) );
}

bool TQODBCDriver::beginTransaction()
{
    if ( !isOpen() ) {
#ifdef QT_CHECK_RANGE
	tqWarning(" TQODBCDriver::beginTransaction: Database not open" );
#endif
	return false;
    }
    SQLUINTEGER ac(SQL_AUTOCOMMIT_OFF);
    SQLRETURN r  = SQLSetConnectAttr( d->hDbc,
				      SQL_ATTR_AUTOCOMMIT,
				      (SQLPOINTER)ac,
				      sizeof(ac) );
    if ( r != SQL_SUCCESS ) {
	setLastError( qMakeError( "Unable to disable autocommit", TQSqlError::Transaction, d ) );
	return false;
    }
    return true;
}

bool TQODBCDriver::commitTransaction()
{
    if ( !isOpen() ) {
#ifdef QT_CHECK_RANGE
	tqWarning(" TQODBCDriver::commitTransaction: Database not open" );
#endif
	return false;
    }
    SQLRETURN r = SQLEndTran( SQL_HANDLE_DBC,
			      d->hDbc,
			      SQL_COMMIT );
    if ( r != SQL_SUCCESS ) {
	setLastError( qMakeError("Unable to commit transaction", TQSqlError::Transaction, d ) );
	return false;
    }
    return endTrans();
}

bool TQODBCDriver::rollbackTransaction()
{
    if ( !isOpen() ) {
#ifdef QT_CHECK_RANGE
	tqWarning(" TQODBCDriver::rollbackTransaction: Database not open" );
#endif
	return false;
    }
    SQLRETURN r = SQLEndTran( SQL_HANDLE_DBC,
			      d->hDbc,
			      SQL_ROLLBACK );
    if ( r != SQL_SUCCESS ) {
	setLastError( qMakeError( "Unable to rollback transaction", TQSqlError::Transaction, d ) );
	return false;
    }
    return endTrans();
}

bool TQODBCDriver::endTrans()
{
    SQLUINTEGER ac(SQL_AUTOCOMMIT_ON);
    SQLRETURN r  = SQLSetConnectAttr( d->hDbc,
				      SQL_ATTR_AUTOCOMMIT,
				      (SQLPOINTER)ac,
				      sizeof(ac));
    if ( r != SQL_SUCCESS ) {
	setLastError( qMakeError( "Unable to enable autocommit", TQSqlError::Transaction, d ) );
	return false;
    }
    return true;
}

TQStringList TQODBCDriver::tables( const TQString& typeName ) const
{
    TQStringList tl;
    if ( !isOpen() )
	return tl;
    int type = typeName.toInt();
    SQLHANDLE hStmt;

    SQLRETURN r = SQLAllocHandle( SQL_HANDLE_STMT,
				  d->hDbc,
				  &hStmt );
    if ( r != SQL_SUCCESS ) {
#ifdef QT_CHECK_RANGE
	qSqlWarning( "TQODBCDriver::tables: Unable to allocate handle", d );
#endif
	return tl;
    }
    r = SQLSetStmtAttr( hStmt,
			SQL_ATTR_CURSOR_TYPE,
			(SQLPOINTER)SQL_CURSOR_FORWARD_ONLY,
			SQL_IS_UINTEGER );
    TQString tableType;
    if ( typeName.isEmpty() || ((type & (int)TQSql::Tables) == (int)TQSql::Tables) )
	tableType += "TABLE,";
    if ( (type & (int)TQSql::Views) == (int)TQSql::Views )
	tableType += "VIEW,";
    if ( (type & (int)TQSql::SystemTables) == (int)TQSql::SystemTables )
	tableType += "SYSTEM TABLE,";
    if ( tableType.isEmpty() )
	return tl;
    tableType.truncate( tableType.length() - 1 );

    r = SQLTables( hStmt,
		   NULL,
		   0,
		   NULL,
		   0,
		   NULL,
		   0,
#ifdef UNICODE
		   (SQLWCHAR*)tableType.unicode(),
#else
		   (SQLCHAR*)tableType.latin1(),
#endif
		   tableType.length() /* characters, not bytes */ );

#ifdef QT_CHECK_RANGE
    if ( r != SQL_SUCCESS )
	qSqlWarning( "TQODBCDriver::tables Unable to execute table list", d );
#endif
    r = SQLFetchScroll( hStmt,
			SQL_FETCH_NEXT,
			0);
    while ( r == SQL_SUCCESS ) {
	bool isNull;
	TQString fieldVal = qGetStringData( hStmt, 2, -1, isNull, d->unicode );
	tl.append( fieldVal );
	r = SQLFetchScroll( hStmt,
			    SQL_FETCH_NEXT,
			    0);
    }

    r = SQLFreeHandle( SQL_HANDLE_STMT, hStmt );
    if ( r!= SQL_SUCCESS )
	qSqlWarning( "TQODBCDriver: Unable to free statement handle" + TQString::number(r), d );
    return tl;
}

TQSqlIndex TQODBCDriver::primaryIndex( const TQString& tablename ) const
{
    TQSqlIndex index( tablename );
    if ( !isOpen() )
	return index;
    bool usingSpecialColumns = false;
    TQSqlRecord rec = record( tablename );

    SQLHANDLE hStmt;
    SQLRETURN r = SQLAllocHandle( SQL_HANDLE_STMT,
				  d->hDbc,
				  &hStmt );
    if ( r != SQL_SUCCESS ) {
#ifdef QT_CHECK_RANGE
	qSqlWarning( "TQODBCDriver::primaryIndex: Unable to list primary key", d );
#endif
	return index;
    }
    TQString catalog, schema, table;
    d->splitTableQualifier( tablename, catalog, schema, table );
    r = SQLSetStmtAttr( hStmt,
			SQL_ATTR_CURSOR_TYPE,
			(SQLPOINTER)SQL_CURSOR_FORWARD_ONLY,
			SQL_IS_UINTEGER );
    r = SQLPrimaryKeys( hStmt,
#ifdef UNICODE
			catalog.length() == 0 ? NULL : (SQLWCHAR*)catalog.unicode(),
#else
			catalog.length() == 0 ? NULL : (SQLCHAR*)catalog.latin1(),
#endif
			catalog.length(),
#ifdef UNICODE
			schema.length() == 0 ? NULL : (SQLWCHAR*)schema.unicode(),
#else
			schema.length() == 0 ? NULL : (SQLCHAR*)schema.latin1(),
#endif
			schema.length(),
#ifdef UNICODE
			(SQLWCHAR*)table.unicode(),
#else
			(SQLCHAR*)table.latin1(),
#endif
			table.length() /* in characters, not in bytes */);

    // if the SQLPrimaryKeys() call does not succeed (e.g the driver
    // does not support it) - try an alternative method to get hold of
    // the primary index (e.g MS Access and FoxPro)
    if ( r != SQL_SUCCESS ) {
	    r = SQLSpecialColumns( hStmt,
				   SQL_BEST_ROWID,
#ifdef UNICODE
				   catalog.length() == 0 ? NULL : (SQLWCHAR*)catalog.unicode(),
#else
				   catalog.length() == 0 ? NULL : (SQLCHAR*)catalog.latin1(),
#endif
				   catalog.length(),
#ifdef UNICODE
				   schema.length() == 0 ? NULL : (SQLWCHAR*)schema.unicode(),
#else
				   schema.length() == 0 ? NULL : (SQLCHAR*)schema.latin1(),
#endif
				   schema.length(),
#ifdef UNICODE
				   (SQLWCHAR*)table.unicode(),
#else
				   (SQLCHAR*)table.latin1(),
#endif

				   table.length(),
				   SQL_SCOPE_CURROW,
				   SQL_NULLABLE );

	    if ( r != SQL_SUCCESS ) {
#ifdef QT_CHECK_RANGE
		qSqlWarning( "TQODBCDriver::primaryIndex: Unable to execute primary key list", d );
#endif
	    } else {
		usingSpecialColumns = true;
	    }
    }
    r = SQLFetchScroll( hStmt,
			SQL_FETCH_NEXT,
			0 );
    bool isNull;
    int fakeId = 0;
    TQString cName, idxName;
    // Store all fields in a StringList because some drivers can't detail fields in this FETCH loop
    while ( r == SQL_SUCCESS ) {
	if ( usingSpecialColumns ) {
	    cName = qGetStringData( hStmt, 1, -1, isNull, d->unicode ); // column name
	    idxName = TQString::number( fakeId++ ); // invent a fake index name
	} else {
	    cName = qGetStringData( hStmt, 3, -1, isNull, d->unicode ); // column name
	    idxName = qGetStringData( hStmt, 5, -1, isNull, d->unicode ); // pk index name
	}
        TQSqlField *fld = rec.field(cName);
        if (fld)
	    index.append(*fld);
	index.setName( idxName );
	r = SQLFetchScroll( hStmt,
			    SQL_FETCH_NEXT,
			    0 );
    }
    r = SQLFreeHandle( SQL_HANDLE_STMT, hStmt );
    if ( r!= SQL_SUCCESS )
	qSqlWarning( "TQODBCDriver: Unable to free statement handle" + TQString::number(r), d );
    return index;
}

TQSqlRecord TQODBCDriver::record( const TQString& tablename ) const
{
    return recordInfo( tablename ).toRecord();
}

TQSqlRecord TQODBCDriver::record( const TQSqlQuery& query ) const
{
    return recordInfo( query ).toRecord();
}

TQSqlRecordInfo TQODBCDriver::recordInfo( const TQString& tablename ) const
{
    TQSqlRecordInfo fil;
    if ( !isOpen() )
	return fil;

    SQLHANDLE hStmt;
    TQString catalog, schema, table;
    d->splitTableQualifier( tablename, catalog, schema, table );
    SQLRETURN r = SQLAllocHandle( SQL_HANDLE_STMT,
				  d->hDbc,
				  &hStmt );
    if ( r != SQL_SUCCESS ) {
#ifdef QT_CHECK_RANGE
	qSqlWarning( "TQODBCDriver::record: Unable to allocate handle", d );
#endif
	return fil;
    }
    r = SQLSetStmtAttr( hStmt,
			SQL_ATTR_CURSOR_TYPE,
			(SQLPOINTER)SQL_CURSOR_FORWARD_ONLY,
			SQL_IS_UINTEGER );
    r =  SQLColumns( hStmt,
#ifdef UNICODE
		     catalog.length() == 0 ? NULL : (SQLWCHAR*)catalog.unicode(),
#else
		     catalog.length() == 0 ? NULL : (SQLCHAR*)catalog.latin1(),
#endif
		     catalog.length(),
#ifdef UNICODE
		     schema.length() == 0 ? NULL : (SQLWCHAR*)schema.unicode(),
#else
		     schema.length() == 0 ? NULL : (SQLCHAR*)schema.latin1(),
#endif
		     schema.length(),
#ifdef UNICODE
		     (SQLWCHAR*)table.unicode(),
#else
		     (SQLCHAR*)table.latin1(),
#endif
		     table.length(),
		     NULL,
		     0 );
#ifdef QT_CHECK_RANGE
    if ( r != SQL_SUCCESS )
	qSqlWarning( "TQODBCDriver::record: Unable to execute column list", d );
#endif
    r = SQLFetchScroll( hStmt,
			SQL_FETCH_NEXT,
			0);
    // Store all fields in a StringList because some drivers can't detail fields in this FETCH loop
    while ( r == SQL_SUCCESS ) {

	fil.append( qMakeFieldInfo( hStmt, d ) );

	r = SQLFetchScroll( hStmt,
			    SQL_FETCH_NEXT,
			    0);
    }

    r = SQLFreeHandle( SQL_HANDLE_STMT, hStmt );
    if ( r!= SQL_SUCCESS )
	qSqlWarning( "TQODBCDriver: Unable to free statement handle " + TQString::number(r), d );

    return fil;
}

TQSqlRecordInfo TQODBCDriver::recordInfo( const TQSqlQuery& query ) const
{
    TQSqlRecordInfo fil;
    if ( !isOpen() )
	return fil;
    if ( query.isActive() && query.driver() == this ) {
	TQODBCResult* result = (TQODBCResult*)query.result();
	fil = result->d->rInf;
    }
    return fil;
}

SQLHANDLE TQODBCDriver::environment()
{
    return d->hEnv;
}

SQLHANDLE TQODBCDriver::connection()
{
    return d->hDbc;
}

TQString TQODBCDriver::formatValue( const TQSqlField* field,
				  bool trimStrings ) const
{
    TQString r;
    if ( field->isNull() ) {
	r = nullText();
    } else if ( field->type() == TQVariant::DateTime ) {
	// Use an escape sequence for the datetime fields
	if ( field->value().toDateTime().isValid() ){
	    TQDate dt = field->value().toDateTime().date();
	    TQTime tm = field->value().toDateTime().time();
	    // Dateformat has to be "yyyy-MM-dd hh:mm:ss", with leading zeroes if month or day < 10
	    r = "{ ts '" +
		TQString::number(dt.year()) + "-" +
		TQString::number(dt.month()).rightJustify( 2, '0', true ) + "-" +
		TQString::number(dt.day()).rightJustify( 2, '0', true ) + " " +
		tm.toString() +
		"' }";
	} else
	    r = nullText();
    } else if ( field->type() == TQVariant::ByteArray ) {
	TQByteArray ba = field->value().toByteArray();
	TQString res;
	static const char hexchars[] = "0123456789abcdef";
	for ( uint i = 0; i < ba.size(); ++i ) {
	    uchar s = (uchar) ba[(int)i];
	    res += hexchars[s >> 4];
	    res += hexchars[s & 0x0f];
	}
	r = "0x" + res;
    } else {
	r = TQSqlDriver::formatValue( field, trimStrings );
    }
    return r;
}
