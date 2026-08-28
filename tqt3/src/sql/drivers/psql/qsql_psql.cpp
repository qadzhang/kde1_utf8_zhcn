/****************************************************************************
**
** Implementation of PostgreSQL driver classes
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

#include "qsql_psql.h"
#include <private/qsqlextension_p.h>

#include <math.h>

#include <ntqpointarray.h>
#include <ntqsqlrecord.h>
#include <ntqregexp.h>
#include <ntqdatetime.h>
// PostgreSQL header <utils/elog.h> included by <postgres.h> redefines DEBUG.
#if defined(DEBUG)
# undef DEBUG
#endif
#include <postgres.h>
#include <libpq/libpq-fs.h>
// PostgreSQL header <catalog/pg_type.h> redefines errno erroneously.
#if defined(errno)
# undef errno
#endif
#define errno qt_psql_errno
#include <catalog/pg_type.h>
#undef errno
#ifdef open
# undef open
#endif

TQPtrDict<TQSqlDriverExtension> *tqSqlDriverExtDict();
TQPtrDict<TQSqlOpenExtension> *tqSqlOpenExtDict();

class TQPSQLPrivate
{
public:
  TQPSQLPrivate():connection(0), result(0), isUtf8(false) {}
    PGconn	*connection;
    PGresult	*result;
    bool        isUtf8;
};

class TQPSQLDriverExtension : public TQSqlDriverExtension
{
public:
    TQPSQLDriverExtension( TQPSQLDriver *dri )
	: TQSqlDriverExtension(), driver(dri) { }
    ~TQPSQLDriverExtension() {}

    bool isOpen() const;
private:
    TQPSQLDriver *driver;
};

bool TQPSQLDriverExtension::isOpen() const
{
    return PQstatus( driver->connection() ) == CONNECTION_OK;
}

class TQPSQLOpenExtension : public TQSqlOpenExtension
{
public:
    TQPSQLOpenExtension( TQPSQLDriver *dri )
	: TQSqlOpenExtension(), driver(dri) { }
    ~TQPSQLOpenExtension() {}

    bool open( const TQString& db,
	       const TQString& user,
	       const TQString& password,
	       const TQString& host,
	       int port,
	       const TQString& connOpts );
private:
    TQPSQLDriver *driver;
};

bool TQPSQLOpenExtension::open( const TQString& db,
			       const TQString& user,
			       const TQString& password,
			       const TQString& host,
			       int port,
			       const TQString& connOpts )
{
    return driver->open( db, user, password, host, port, connOpts );
}

static TQSqlError qMakeError( const TQString& err, int type, const TQPSQLPrivate* p )
{
    const char *s = PQerrorMessage(p->connection);
    TQString msg = p->isUtf8 ? TQString::fromUtf8(s) : TQString::fromLocal8Bit(s);
    return TQSqlError("TQPSQL: " + err, msg, type);
}

static TQVariant::Type qDecodePSQLType( int t )
{
    TQVariant::Type type = TQVariant::Invalid;
    switch ( t ) {
    case BOOLOID	:
	type = TQVariant::Bool;
	break;
    case INT8OID	:
	type = TQVariant::LongLong;
	break;
    case INT2OID	:
	//    case INT2VECTOROID  : // 7.x
    case INT4OID        :
	type = TQVariant::Int;
	break;
    case NUMERICOID     :
    case FLOAT4OID      :
    case FLOAT8OID      :
	type = TQVariant::Double;
	break;
#ifdef ABSTIMEOID // PostgreSQL << 12.x
    case ABSTIMEOID     :
    case RELTIMEOID     :
#endif
    case DATEOID	:
	type = TQVariant::Date;
	break;
    case TIMEOID	:
#ifdef TIMETZOID // 7.x
	case TIMETZOID  :
#endif
	type = TQVariant::Time;
	break;
    case TIMESTAMPOID   :
#ifdef DATETIMEOID
    // Postgres 6.x datetime workaround.
    // DATETIMEOID == TIMESTAMPOID (only the names have changed)
    case DATETIMEOID    :
#endif
#ifdef TIMESTAMPTZOID
    // Postgres 7.2 workaround
    // TIMESTAMPTZOID == TIMESTAMPOID == DATETIMEOID
    case TIMESTAMPTZOID :
#endif
	type = TQVariant::DateTime;
	break;
	//    case ZPBITOID	: // 7.x
	//    case VARBITOID	: // 7.x
    case OIDOID         :
    case BYTEAOID       :
	type = TQVariant::ByteArray;
	break;
    case REGPROCOID     :
    case TIDOID         :
    case XIDOID         :
    case CIDOID         :
	//    case OIDVECTOROID   : // 7.x
    case UNKNOWNOID     :
	//    case TINTERVALOID   : // 7.x
	type = TQVariant::Invalid;
	break;
    default:
    case CHAROID	:
    case BPCHAROID	:
	//    case LZTEXTOID	: // 7.x
    case VARCHAROID	:
    case TEXTOID	:
    case NAMEOID	:
    case CASHOID        :
    case INETOID        :
    case CIDROID        :
    case CIRCLEOID      :
	type = TQVariant::String;
	break;
    }
    return type;
}

TQPSQLResult::TQPSQLResult( const TQPSQLDriver* db, const TQPSQLPrivate* p )
: TQSqlResult( db ),
  currentSize( 0 )
{
    d =   new TQPSQLPrivate();
    (*d) = (*p);
}

TQPSQLResult::~TQPSQLResult()
{
    cleanup();
    delete d;
}

PGresult* TQPSQLResult::result()
{
    return d->result;
}

void TQPSQLResult::cleanup()
{
    if ( d->result )
	PQclear( d->result );
    d->result = 0;
    setAt( -1 );
    currentSize = 0;
    setActive( false );
}

bool TQPSQLResult::fetch( int i )
{
    if ( !isActive() )
	return false;
    if ( i < 0 )
	return false;
    if ( i >= currentSize )
	return false;
    if ( at() == i )
	return true;
    setAt( i );
    return true;
}

bool TQPSQLResult::fetchFirst()
{
    return fetch( 0 );
}

bool TQPSQLResult::fetchLast()
{
    return fetch( PQntuples( d->result ) - 1 );
}

// some Postgres conversions
static TQPoint pointFromString( const TQString& s)
{
    // format '(x,y)'
    int pivot = s.find( ',' );
    if ( pivot != -1 ) {
	int x = s.mid( 1, pivot-1 ).toInt();
	int y = s.mid( pivot+1, s.length()-pivot-2 ).toInt();
	return TQPoint( x, y ) ;
    } else
	return TQPoint();
}

TQVariant TQPSQLResult::data( int i )
{
    if ( i >= PQnfields( d->result ) ) {
	tqWarning( "TQPSQLResult::data: column %d out of range", i );
	return TQVariant();
    }
    int ptype = PQftype( d->result, i );
    TQVariant::Type type = qDecodePSQLType( ptype );
    const TQString val = ( d->isUtf8 && ptype != BYTEAOID ) ?
			TQString::fromUtf8( PQgetvalue( d->result, at(), i ) ) :
			TQString::fromLocal8Bit( PQgetvalue( d->result, at(), i ) );
    if ( PQgetisnull( d->result, at(), i ) ) {
	TQVariant v;
	v.cast( type );
	return v;
    }
    switch ( type ) {
    case TQVariant::Bool:
	{
	    TQVariant b ( (bool)(val == "t") );
	    return ( b );
	}
    case TQVariant::String:
	return TQVariant( val );
    case TQVariant::LongLong:
	if ( val[0] == '-' )
	    return TQVariant( val.toLongLong() );
	else
	    return TQVariant( val.toULongLong() );
    case TQVariant::Int:
	return TQVariant( val.toInt() );
    case TQVariant::Double:
	if ( ptype == NUMERICOID )
	    return TQVariant( val );
	return TQVariant( val.toDouble() );
    case TQVariant::Date:
	if ( val.isEmpty() ) {
	    return TQVariant( TQDate() );
	} else {
	    return TQVariant( TQDate::fromString( val, TQt::ISODate ) );
	}
    case TQVariant::Time:
	if ( val.isEmpty() )
	    return TQVariant( TQTime() );
	if ( val.at( val.length() - 3 ) == '+' )
	    // strip the timezone
	    return TQVariant( TQTime::fromString( val.left( val.length() - 3 ), TQt::ISODate ) );
	return TQVariant( TQTime::fromString( val, TQt::ISODate ) );
    case TQVariant::DateTime: {
	if ( val.length() < 10 )
	    return TQVariant( TQDateTime() );
	// remove the timezone
	TQString dtval = val;
	if ( dtval.at( dtval.length() - 3 ) == '+' )
	    dtval.truncate( dtval.length() - 3 );
	// milliseconds are sometimes returned with 2 digits only
	if ( dtval.at( dtval.length() - 3 ).isPunct() )
	    dtval += '0';
	if ( dtval.isEmpty() )
	    return TQVariant( TQDateTime() );
	else
	    return TQVariant( TQDateTime::fromString( dtval, TQt::ISODate ) );
    }
    case TQVariant::Point:
	return TQVariant( pointFromString( val ) );
    case TQVariant::Rect: // format '(x,y),(x',y')'
	{
	    int pivot = val.find( "),(" );
	    if ( pivot != -1 )
		return TQVariant( TQRect( pointFromString( val.mid(pivot+2,val.length()) ), pointFromString( val.mid(0,pivot+1) ) ) );
	    return TQVariant( TQRect() );
	}
    case TQVariant::PointArray: // format '((x,y),(x1,y1),...,(xn,yn))'
	{
	    TQRegExp pointPattern("\\([0-9-]*,[0-9-]*\\)");
	    int points = val.contains( pointPattern );
	    TQPointArray parray( points );
	    int idx = 1;
	    for ( int i = 0; i < points; i++ ){
		int start = val.find( pointPattern, idx );
		int end = -1;
		if ( start != -1 ) {
		    end = val.find( ')', start+1 );
		    if ( end != -1 ) {
			parray.setPoint( i, pointFromString( val.mid(idx, end-idx+1) ) );
		    }
		    else
			parray.setPoint( i, TQPoint() );
		} else {
		    parray.setPoint( i, TQPoint() );
		    break;
		}
		idx = end+2;
	    }
	    return TQVariant( parray );
	}
    case TQVariant::ByteArray: {
	if ( ptype == BYTEAOID ) {
	    uint i = 0;
	    int index = 0;
	    uint len = val.length();
	    static const TQChar backslash( '\\' );
	    TQByteArray ba( (int)len );
	    while ( i < len ) {
		if ( val.at( i ) == backslash ) {
		    if ( val.at( i + 1 ).isDigit() ) {
			ba[ index++ ] = (char)(val.mid( i + 1, 3 ).toInt( 0, 8 ));
			i += 4;
		    } else {
			ba[ index++ ] = val.at( i + 1 );
			i += 2;
		    }
		} else {
		    ba[ index++ ] = val.at( i++ ).unicode();
		}
	    }
	    ba.resize( index );
	    return TQVariant( ba );
	}

	TQByteArray ba;
	((TQSqlDriver*)driver())->beginTransaction();
	Oid oid = val.toInt();
	int fd = lo_open( d->connection, oid, INV_READ );
#ifdef QT_CHECK_RANGE
	if ( fd < 0) {
	    tqWarning( "TQPSQLResult::data: unable to open large object for read" );
	    ((TQSqlDriver*)driver())->commitTransaction();
	    return TQVariant( ba );
	}
#endif
	int size = 0;
	int retval = lo_lseek( d->connection, fd, 0L, SEEK_END );
	if ( retval >= 0 ) {
	    size = lo_tell( d->connection, fd );
	    lo_lseek( d->connection, fd, 0L, SEEK_SET );
	}
	if ( size == 0 ) {
	    lo_close( d->connection, fd );
	    ((TQSqlDriver*)driver())->commitTransaction();
	    return TQVariant( ba );
	}
	char * buf = new char[ size ];

#ifdef Q_OS_WIN32
	// ### For some reason lo_read() fails if we try to read more than
	// ### 32760 bytes
	char * p = buf;
	int nread = 0;

	while( size < nread ){
		retval = lo_read( d->connection, fd, p, 32760 );
		nread += retval;
		p += retval;
	}
#else
	retval = lo_read( d->connection, fd, buf, size );
#endif

	if (retval < 0) {
	    tqWarning( "TQPSQLResult::data: unable to read large object" );
	} else {
	    ba.duplicate( buf, size );
	}
	delete [] buf;
	lo_close( d->connection, fd );
	((TQSqlDriver*)driver())->commitTransaction();
	return TQVariant( ba );
    }
    default:
    case TQVariant::Invalid:
#ifdef QT_CHECK_RANGE
	tqWarning("TQPSQLResult::data: unknown data type");
#endif
	;
    }
    return TQVariant();
}

bool TQPSQLResult::isNull( int field )
{
    PQgetvalue( d->result, at(), field );
    return PQgetisnull( d->result, at(), field );
}

bool TQPSQLResult::reset ( const TQString& query )
{
    cleanup();
    if ( !driver() )
	return false;
    if ( !driver()->isOpen() || driver()->isOpenError() )
	return false;
    setActive( false );
    setAt( TQSql::BeforeFirst );
    if ( d->result )
	PQclear( d->result );
    if ( d->isUtf8 ) {
	d->result = PQexec( d->connection, query.utf8().data() );
    } else {
	d->result = PQexec( d->connection, query.local8Bit().data() );
    }
    int status =  PQresultStatus( d->result );
    if ( status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK ) {
	if ( status == PGRES_TUPLES_OK ) {
	    setSelect( true );
	    currentSize = PQntuples( d->result );
	} else {
	    setSelect( false );
	    currentSize = -1;
	}
	setActive( true );
	return true;
    }
    setLastError( qMakeError( "Unable to create query", TQSqlError::Statement, d ) );
    return false;
}

int TQPSQLResult::size()
{
    return currentSize;
}

int TQPSQLResult::numRowsAffected()
{
    return TQString( PQcmdTuples( d->result ) ).toInt();
}

///////////////////////////////////////////////////////////////////

static bool setEncodingUtf8( PGconn* connection )
{
    PGresult* result = PQexec( connection, "SET CLIENT_ENCODING TO 'UNICODE'" );
    int status = PQresultStatus( result );
    PQclear( result );
    return status == PGRES_COMMAND_OK;
}

static void setDatestyle( PGconn* connection )
{
    PGresult* result = PQexec( connection, "SET DATESTYLE TO 'ISO'" );
#ifdef QT_CHECK_RANGE
    int status =  PQresultStatus( result );
    if ( status != PGRES_COMMAND_OK )
        tqWarning( "%s", PQerrorMessage( connection ) );
#endif
    PQclear( result );
}

static TQPSQLDriver::Protocol getPSQLVersion( PGconn* connection )
{
    PGresult* result = PQexec( connection, "select version()" );
    int status =  PQresultStatus( result );
    if ( status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK ) {
	TQString val( PQgetvalue( result, 0, 0 ) );
	PQclear( result );
	TQRegExp rx( "(\\d+)\\.(\\d+)" );
	rx.setMinimal ( true ); // enforce non-greedy RegExp
        if ( rx.search( val ) != -1 ) {
	    int vMaj = rx.cap( 1 ).toInt();
	    int vMin = rx.cap( 2 ).toInt();
	    if ( vMaj < 6 ) {
#ifdef QT_CHECK_RANGE
		tqWarning( "This version of PostgreSQL is not supported and may not work." );
#endif
		return TQPSQLDriver::Version6;
	    }
	    if ( vMaj == 6 ) {
		return TQPSQLDriver::Version6;
	    } else if ( vMaj == 7 ) {
		if ( vMin < 1 )
		    return TQPSQLDriver::Version7;
		else if ( vMin < 3 )
		    return TQPSQLDriver::Version71;
	    }
	    return TQPSQLDriver::Version73;
	}
    } else {
#ifdef QT_CHECK_RANGE
	tqWarning( "This version of PostgreSQL is not supported and may not work." );
#endif
    }

    return TQPSQLDriver::Version6;
}

TQPSQLDriver::TQPSQLDriver( TQObject * parent, const char * name )
    : TQSqlDriver(parent,name ? name : "TQPSQL"), pro( TQPSQLDriver::Version6 )
{
    init();
}

TQPSQLDriver::TQPSQLDriver( PGconn * conn, TQObject * parent, const char * name )
    : TQSqlDriver(parent,name ? name : "TQPSQL"), pro( TQPSQLDriver::Version6 )
{
    init();
    d->connection = conn;
    if ( conn ) {
	pro = getPSQLVersion( d->connection );
	setOpen( true );
	setOpenError( false );
    }
}

void TQPSQLDriver::init()
{
    tqSqlDriverExtDict()->insert( this, new TQPSQLDriverExtension(this) );
    tqSqlOpenExtDict()->insert( this, new TQPSQLOpenExtension(this) );

    d = new TQPSQLPrivate();
}

TQPSQLDriver::~TQPSQLDriver()
{
    if ( d->connection )
	PQfinish( d->connection );
    delete d;
    if ( !tqSqlDriverExtDict()->isEmpty() ) {
	TQSqlDriverExtension *ext = tqSqlDriverExtDict()->take( this );
	delete ext;
    }
    if ( !tqSqlOpenExtDict()->isEmpty() ) {
	TQSqlOpenExtension *ext = tqSqlOpenExtDict()->take( this );
	delete ext;
    }
}

PGconn* TQPSQLDriver::connection()
{
    return d->connection;
}


bool TQPSQLDriver::hasFeature( DriverFeature f ) const
{
    switch ( f ) {
    case Transactions:
	return true;
    case QuerySize:
	return true;
    case BLOB:
	return pro >= TQPSQLDriver::Version71;
    case Unicode:
	return d->isUtf8;
    default:
	return false;
    }
}

bool TQPSQLDriver::open( const TQString&,
			const TQString&,
			const TQString&,
			const TQString&,
			int )
{
    tqWarning("TQPSQLDriver::open(): This version of open() is no longer supported." );
    return false;
}

bool TQPSQLDriver::open( const TQString & db,
			const TQString & user,
			const TQString & password,
			const TQString & host,
			int port,
			const TQString& connOpts )
{
    if ( isOpen() )
	close();
    TQString connectString;
    if ( host.length() )
	connectString.append( "host=" ).append( host );
    if ( db.length() )
	connectString.append( " dbname=" ).append( db );
    if ( user.length() )
	connectString.append( " user=" ).append( user );
    if ( password.length() )
	connectString.append( " password=" ).append( password );
    if ( port > -1 )
	connectString.append( " port=" ).append( TQString::number( port ) );

    // add any connect options - the server will handle error detection
    if ( !connOpts.isEmpty() )
	connectString += " " + TQStringList::split( ';', connOpts ).join( " " );

    d->connection = PQconnectdb( connectString.local8Bit().data() );
    if ( PQstatus( d->connection ) == CONNECTION_BAD ) {
	setLastError( qMakeError("Unable to connect", TQSqlError::Connection, d ) );
	setOpenError( true );
	return false;
    }

    pro = getPSQLVersion( d->connection );
    d->isUtf8 = setEncodingUtf8( d->connection );
    setDatestyle( d->connection );

    setOpen( true );
    setOpenError( false );
    return true;
}

void TQPSQLDriver::close()
{
    if ( isOpen() ) {
        if (d->connection)
	    PQfinish( d->connection );
	d->connection = 0;
	setOpen( false );
	setOpenError( false );
    }
}

bool TQPSQLDriver::ping()
{
    if ( !isOpen() ) {
        return false;
    }

    PGresult *res = NULL;

    // Send ping
    res = PQexec( d->connection, "" );
    PQclear(res);

    // Check connection status
    if ( PQstatus( d->connection ) != CONNECTION_OK ) {
	PQreset( d->connection );
	if ( PQstatus( d->connection ) != CONNECTION_OK ) {
	    setLastError( qMakeError("Unable to execute ping", TQSqlError::Statement, d ) );
	    return false;
	}
    }
    return true;
}

TQSqlQuery TQPSQLDriver::createQuery() const
{
    return TQSqlQuery( new TQPSQLResult( this, d ) );
}

bool TQPSQLDriver::beginTransaction()
{
    if ( !isOpen() ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQPSQLDriver::beginTransaction: Database not open" );
#endif
	return false;
    }
    PGresult* res = PQexec( d->connection, "BEGIN" );
    if ( !res || PQresultStatus( res ) != PGRES_COMMAND_OK ) {
	PQclear( res );
	setLastError( qMakeError( "Could not begin transaction", TQSqlError::Transaction, d ) );
	return false;
    }
    PQclear( res );
    return true;
}

bool TQPSQLDriver::commitTransaction()
{
    if ( !isOpen() ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQPSQLDriver::commitTransaction: Database not open" );
#endif
	return false;
    }
    PGresult* res = PQexec( d->connection, "COMMIT" );
    if ( !res || PQresultStatus( res ) != PGRES_COMMAND_OK ) {
	PQclear( res );
	setLastError( qMakeError( "Could not commit transaction", TQSqlError::Transaction, d ) );
	return false;
    }
    PQclear( res );
    return true;
}

bool TQPSQLDriver::rollbackTransaction()
{
    if ( !isOpen() ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQPSQLDriver::rollbackTransaction: Database not open" );
#endif
	return false;
    }
    PGresult* res = PQexec( d->connection, "ROLLBACK" );
    if ( !res || PQresultStatus( res ) != PGRES_COMMAND_OK ) {
	setLastError( qMakeError( "Could not rollback transaction", TQSqlError::Transaction, d ) );
	PQclear( res );
	return false;
    }
    PQclear( res );
    return true;
}

TQStringList TQPSQLDriver::tables( const TQString& typeName ) const
{
    TQStringList tl;
    if ( !isOpen() )
	return tl;
    int type = typeName.toInt();
    TQSqlQuery t = createQuery();
    t.setForwardOnly( true );

    if ( typeName.isEmpty() || ((type & (int)TQSql::Tables) == (int)TQSql::Tables) ) {

        TQString query("select relname from pg_class where (relkind = 'r') "
                      "and (relname !~ '^Inv') "
                      "and (relname !~ '^pg_') ");
        if (pro >= TQPSQLDriver::Version73)
            query.append("and (relnamespace not in "
                         "(select oid from pg_namespace where nspname = 'information_schema')) "
                         "and pg_table_is_visible(pg_class.oid) ");
        t.exec(query);
	while ( t.next() )
	    tl.append( t.value(0).toString() );
    }
    if ( (type & (int)TQSql::Views) == (int)TQSql::Views ) {
        TQString query("select relname from pg_class where ( relkind = 'v' ) "
                "and ( relname !~ '^Inv' ) "
                "and ( relname !~ '^pg_' ) ");
        if (pro >= TQPSQLDriver::Version73)
            query.append("and (relnamespace not in "
                         "(select oid from pg_namespace where nspname = 'information_schema')) "
                         "and pg_table_is_visible(pg_class.oid) ");
        t.exec(query);
	while ( t.next() )
	    tl.append( t.value(0).toString() );
    }
    if ( (type & (int)TQSql::SystemTables) == (int)TQSql::SystemTables ) {
        TQString query( "select relname from pg_class where ( relkind = 'r' ) "
		"and ( relname like 'pg_%' ) " );
        if (pro >= TQPSQLDriver::Version73)
		query.append( "and pg_table_is_visible(pg_class.oid) " );
        t.exec(query);
	while ( t.next() )
	    tl.append( t.value(0).toString() );
    }

    return tl;
}

TQSqlIndex TQPSQLDriver::primaryIndex( const TQString& tablename ) const
{
    TQSqlIndex idx( tablename );
    if ( !isOpen() )
	return idx;
    TQSqlQuery i = createQuery();
    TQString stmt;

    switch( pro ) {
    case TQPSQLDriver::Version6:
	stmt = "select pg_att1.attname, int(pg_att1.atttypid), pg_att2.attnum, pg_cl.relname "
		"from pg_attribute pg_att1, pg_attribute pg_att2, pg_class pg_cl, pg_index pg_ind "
		"where lower(pg_cl.relname) = '%1_pkey' ";
	break;
    case TQPSQLDriver::Version7:
    case TQPSQLDriver::Version71:
	stmt = "select pg_att1.attname, pg_att1.atttypid::int, pg_cl.relname "
		"from pg_attribute pg_att1, pg_attribute pg_att2, pg_class pg_cl, pg_index pg_ind "
		"where lower(pg_cl.relname) = '%1_pkey' ";
	break;
    case TQPSQLDriver::Version73:
	stmt = "select pg_att1.attname, pg_att1.atttypid::int, pg_cl.relname "
		"from pg_attribute pg_att1, pg_attribute pg_att2, pg_class pg_cl, pg_index pg_ind "
		"where lower(pg_cl.relname) = '%1_pkey' "
		"and pg_table_is_visible(pg_cl.oid) "
		"and pg_att1.attisdropped = false ";
	break;
    }
    stmt += "and pg_cl.oid = pg_ind.indexrelid "
	    "and pg_att2.attrelid = pg_ind.indexrelid "
	    "and pg_att1.attrelid = pg_ind.indrelid "
	    "and pg_att1.attnum = pg_ind.indkey[pg_att2.attnum-1] "
	    "order by pg_att2.attnum";

    i.exec( stmt.arg( tablename.lower() ) );
    while ( i.isActive() && i.next() ) {
	TQSqlField f( i.value(0).toString(), qDecodePSQLType( i.value(1).toInt() ) );
	idx.append( f );
	idx.setName( i.value(2).toString() );
    }
    return idx;
}

TQSqlRecord TQPSQLDriver::record( const TQString& tablename ) const
{
    TQSqlRecord fil;
    if ( !isOpen() )
	return fil;
    TQString stmt;
    switch( pro ) {
    case TQPSQLDriver::Version6:
	stmt = "select pg_attribute.attname, int(pg_attribute.atttypid) "
			"from pg_class, pg_attribute "
			"where lower(pg_class.relname) = '%1' "
			"and pg_attribute.attnum > 0 "
			"and pg_attribute.attrelid = pg_class.oid ";
	break;
    case TQPSQLDriver::Version7:
    case TQPSQLDriver::Version71:
	stmt = "select pg_attribute.attname, pg_attribute.atttypid::int "
			"from pg_class, pg_attribute "
			"where lower(pg_class.relname) = '%1' "
			"and pg_attribute.attnum > 0 "
			"and pg_attribute.attrelid = pg_class.oid ";
	break;
    case TQPSQLDriver::Version73:
	stmt = "select pg_attribute.attname, pg_attribute.atttypid::int "
			"from pg_class, pg_attribute "
			"where lower(pg_class.relname) = '%1' "
			"and pg_table_is_visible(pg_class.oid) "
			"and pg_attribute.attnum > 0 "
			"and pg_attribute.attisdropped = false "
			"and pg_attribute.attrelid = pg_class.oid ";
	break;
    }

    TQSqlQuery fi = createQuery();
    fi.exec( stmt.arg( tablename.lower() ) );
    while ( fi.next() ) {
	TQSqlField f( fi.value(0).toString(), qDecodePSQLType( fi.value(1).toInt() ) );
	fil.append( f );
    }
    return fil;
}

TQSqlRecord TQPSQLDriver::record( const TQSqlQuery& query ) const
{
    TQSqlRecord fil;
    if ( !isOpen() )
	return fil;
    if ( query.isActive() && query.driver() == this ) {
	TQPSQLResult* result = (TQPSQLResult*)query.result();
	int count = PQnfields( result->d->result );
	for ( int i = 0; i < count; ++i ) {
	    TQString name = PQfname( result->d->result, i );
	    TQVariant::Type type = qDecodePSQLType( PQftype( result->d->result, i ) );
	    TQSqlField rf( name, type );
	    fil.append( rf );
	}
    }
    return fil;
}

TQSqlRecordInfo TQPSQLDriver::recordInfo( const TQString& tablename ) const
{
    TQSqlRecordInfo info;
    if ( !isOpen() )
	return info;

    TQString stmt;
    switch( pro ) {
    case TQPSQLDriver::Version6:
	stmt = "select pg_attribute.attname, int(pg_attribute.atttypid), pg_attribute.attnotnull, "
		"pg_attribute.attlen, pg_attribute.atttypmod, int(pg_attribute.attrelid), pg_attribute.attnum "
		"from pg_class, pg_attribute "
		"where lower(pg_class.relname) = '%1' "
		"and pg_attribute.attnum > 0 "
		"and pg_attribute.attrelid = pg_class.oid ";
	break;
    case TQPSQLDriver::Version7:
	stmt = "select pg_attribute.attname, pg_attribute.atttypid::int, pg_attribute.attnotnull, "
		"pg_attribute.attlen, pg_attribute.atttypmod, pg_attribute.attrelid::int, pg_attribute.attnum "
		"from pg_class, pg_attribute "
		"where lower(pg_class.relname) = '%1' "
		"and pg_attribute.attnum > 0 "
		"and pg_attribute.attrelid = pg_class.oid ";
	break;
    case TQPSQLDriver::Version71:
	stmt = "select pg_attribute.attname, pg_attribute.atttypid::int, pg_attribute.attnotnull, "
		"pg_attribute.attlen, pg_attribute.atttypmod, pg_attrdef.adsrc "
		"from pg_class, pg_attribute "
		"left join pg_attrdef on (pg_attrdef.adrelid = pg_attribute.attrelid and pg_attrdef.adnum = pg_attribute.attnum) "
		"where lower(pg_class.relname) = '%1' "
		"and pg_attribute.attnum > 0 "
		"and pg_attribute.attrelid = pg_class.oid "
		"order by pg_attribute.attnum ";
	break;
    case TQPSQLDriver::Version73:
	stmt = "select pg_attribute.attname, pg_attribute.atttypid::int, pg_attribute.attnotnull, "
		"pg_attribute.attlen, pg_attribute.atttypmod, pg_attrdef.adsrc "
		"from pg_class, pg_attribute "
		"left join pg_attrdef on (pg_attrdef.adrelid = pg_attribute.attrelid and pg_attrdef.adnum = pg_attribute.attnum) "
		"where lower(pg_class.relname) = '%1' "
		"and pg_table_is_visible(pg_class.oid) "
		"and pg_attribute.attnum > 0 "
		"and pg_attribute.attrelid = pg_class.oid "
		"and pg_attribute.attisdropped = false "
		"order by pg_attribute.attnum ";
	break;
    }

    TQSqlQuery query = createQuery();
    query.exec( stmt.arg( tablename.lower() ) );
    if ( pro >= TQPSQLDriver::Version71 ) {
	while ( query.next() ) {
	    int len = query.value( 3 ).toInt();
	    int precision = query.value( 4 ).toInt();
	    // swap length and precision if length == -1
	    if ( len == -1 && precision > -1 ) {
		len = precision - 4;
		precision = -1;
	    }
	    TQString defVal = query.value( 5 ).toString();
	    if ( !defVal.isEmpty() && defVal.startsWith( "'" ) )
		defVal = defVal.mid( 1, defVal.length() - 2 );
	    info.append( TQSqlFieldInfo( query.value( 0 ).toString(),
					qDecodePSQLType( query.value( 1 ).toInt() ),
					query.value( 2 ).toBool(),
					len,
					precision,
					defVal,
					query.value( 1 ).toInt() ) );
	}
    } else {
	// Postgres < 7.1 cannot handle outer joins
	while ( query.next() ) {
	    TQString defVal;
	    TQString stmt2 = "select pg_attrdef.adsrc from pg_attrdef where "
			    "pg_attrdef.adrelid = %1 and pg_attrdef.adnum = %2 ";
	    TQSqlQuery query2 = createQuery();
	    query2.exec( stmt2.arg( query.value( 5 ).toInt() ).arg( query.value( 6 ).toInt() ) );
	    if ( query2.isActive() && query2.next() )
		defVal = query2.value( 0 ).toString();
	    if ( !defVal.isEmpty() && defVal.startsWith( "'" ) )
		defVal = defVal.mid( 1, defVal.length() - 2 );
	    int len = query.value( 3 ).toInt();
	    int precision = query.value( 4 ).toInt();
	    // swap length and precision if length == -1
	    if ( len == -1 && precision > -1 ) {
		len = precision - 4;
		precision = -1;
	    }
	    info.append( TQSqlFieldInfo( query.value( 0 ).toString(),
					qDecodePSQLType( query.value( 1 ).toInt() ),
					query.value( 2 ).toBool(),
					len,
					precision,
					defVal,
					query.value( 1 ).toInt() ) );
	}
    }

    return info;
}

TQSqlRecordInfo TQPSQLDriver::recordInfo( const TQSqlQuery& query ) const
{
    TQSqlRecordInfo info;
    if ( !isOpen() )
	return info;
    if ( query.isActive() && query.driver() == this ) {
	TQPSQLResult* result = (TQPSQLResult*)query.result();
	int count = PQnfields( result->d->result );
	for ( int i = 0; i < count; ++i ) {
	    TQString name = PQfname( result->d->result, i );
	    int len = PQfsize( result->d->result, i );
	    int precision = PQfmod( result->d->result, i );
	    // swap length and precision if length == -1
	    if ( len == -1 && precision > -1 ) {
		len = precision - 4;
		precision = -1;
	    }
	    info.append( TQSqlFieldInfo( name,
					qDecodePSQLType( PQftype( result->d->result, i ) ),
					-1,
					len,
					precision,
					TQVariant(),
					PQftype( result->d->result, i ) ) );
	}
    }
    return info;
}

TQString TQPSQLDriver::formatValue( const TQSqlField* field,
				  bool ) const
{
    TQString r;
    if ( field->isNull() ) {
	r = nullText();
    } else {
	switch ( field->type() ) {
	case TQVariant::DateTime:
	    if ( field->value().toDateTime().isValid() ) {
		TQDate dt = field->value().toDateTime().date();
		TQTime tm = field->value().toDateTime().time();
		// msecs need to be right aligned otherwise psql
		// interpretes them wrong
		r = "'" + TQString::number( dt.year() ) + "-" +
			  TQString::number( dt.month() ) + "-" +
			  TQString::number( dt.day() ) + " " +
			  tm.toString() + "." +
			  TQString::number( tm.msec() ).rightJustify( 3, '0' ) + "'";
	    } else {
		r = nullText();
	    }
	    break;
	case TQVariant::Time:
	    if ( field->value().toTime().isValid() ) {
		r = field->value().toTime().toString( TQt::ISODate );
	    } else {
		r = nullText();
	    }
	case TQVariant::String:
	case TQVariant::CString: {
	    switch ( field->value().type() ) {
		case TQVariant::Rect: {
		    TQRect rec = field->value().toRect();
		    // upper right corner then lower left according to psql docs
		    r = "'(" + TQString::number( rec.right() ) +
			"," + TQString::number( rec.bottom() ) +
			"),(" + TQString::number( rec.left() ) +
			"," + TQString::number( rec.top() ) + ")'";
		    break;
		}
		case TQVariant::Point: {
		    TQPoint p = field->value().toPoint();
		    r = "'(" + TQString::number( p.x() ) +
			"," + TQString::number( p.y() ) + ")'";
		    break;
		}
		case TQVariant::PointArray: {
		    TQPointArray pa = field->value().toPointArray();
		    r = "' ";
		    for ( int i = 0; i < (int)pa.size(); ++i ) {
			r += "(" + TQString::number( pa[i].x() ) +
			     "," + TQString::number( pa[i].y() ) + "),";
		    }
		    r.truncate( r.length() - 1 );
		    r += "'";
		    break;
		}
		default:
		    // Escape '\' characters
		    r = TQSqlDriver::formatValue( field );
		    r.replace( "\\", "\\\\" );
		    break;
	    }
	    break;
	}
	case TQVariant::Bool:
	    if ( field->value().toBool() )
		r = "TRUE";
	    else
		r = "FALSE";
	    break;
	case TQVariant::ByteArray: {
	    TQByteArray ba = field->value().asByteArray();
	    TQString res;
	    r = "'";
	    unsigned char uc;
	    for ( int i = 0; i < (int)ba.size(); ++i ) {
		uc = (unsigned char) ba[ i ];
		if ( uc > 40 && uc < 92 ) {
		    r += uc;
		} else {
		    r += "\\\\";
		    r += TQString::number( (unsigned char) ba[ i ], 8 ).rightJustify( 3, '0', true );
		}
	    }
	    r += "'";
	    break;
	}
	default:
	    r = TQSqlDriver::formatValue( field );
	    break;
	}
    }
    return r;
}
