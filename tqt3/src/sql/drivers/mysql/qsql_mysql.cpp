/****************************************************************************
**
** Implementation of MYSQL driver classes
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

#include "qsql_mysql.h"
#include <private/qsqlextension_p.h>

#include <ntqdatetime.h>
#include <ntqvaluevector.h>
#include <ntqsqlrecord.h>

#define TQMYSQL_DRIVER_NAME "TQMYSQL3"

#ifdef Q_OS_WIN32
// comment the next line out if you want to use MySQL/embedded on Win32 systems.
// note that it will crash if you don't statically link to the mysql/e library!
# define Q_NO_MYSQL_EMBEDDED
#endif

TQPtrDict<TQSqlOpenExtension> *tqSqlOpenExtDict();

static int qMySqlConnectionCount = 0;
static bool qMySqlInitHandledByUser = false;

class TQMYSQLOpenExtension : public TQSqlOpenExtension
{
public:
    TQMYSQLOpenExtension( TQMYSQLDriver *dri )
	: TQSqlOpenExtension(), driver(dri) {}
    ~TQMYSQLOpenExtension() {}

    bool open( const TQString& db,
	       const TQString& user,
	       const TQString& password,
	       const TQString& host,
	       int port,
	       const TQString& connOpts );
    
private:
    TQMYSQLDriver *driver;
};

bool TQMYSQLOpenExtension::open( const TQString& db,
				const TQString& user,
				const TQString& password,
				const TQString& host,
				int port,
				const TQString& connOpts )
{
    return driver->open( db, user, password, host, port, connOpts );
}

class TQMYSQLDriverPrivate
{
public:
    TQMYSQLDriverPrivate() : mysql(0) {}
    MYSQL*     mysql;
};

class TQMYSQLResultPrivate : public TQMYSQLDriverPrivate
{
public:
    TQMYSQLResultPrivate() : TQMYSQLDriverPrivate(), result(0) {}
    MYSQL_RES* result;
    MYSQL_ROW  row;
    TQValueVector<TQVariant::Type> fieldTypes;
};

TQSqlError qMakeError( const TQString& err, int type, const TQMYSQLDriverPrivate* p )
{
    return TQSqlError(TQMYSQL_DRIVER_NAME ": " + err, TQString(mysql_error( p->mysql )), type, mysql_errno( p->mysql ));
}

TQVariant::Type qDecodeMYSQLType( int mysqltype, uint flags )
{
    TQVariant::Type type;
    switch ( mysqltype ) {
    case FIELD_TYPE_TINY :
    case FIELD_TYPE_SHORT :
    case FIELD_TYPE_LONG :
    case FIELD_TYPE_INT24 :
	type = (flags & UNSIGNED_FLAG) ? TQVariant::UInt : TQVariant::Int;
	break;
    case FIELD_TYPE_YEAR :
	type = TQVariant::Int;
	break;
    case FIELD_TYPE_LONGLONG :
	type = (flags & UNSIGNED_FLAG) ? TQVariant::ULongLong : TQVariant::LongLong;
	break;
    case FIELD_TYPE_DECIMAL :
    case FIELD_TYPE_FLOAT :
    case FIELD_TYPE_DOUBLE :
	type = TQVariant::Double;
	break;
    case FIELD_TYPE_DATE :
	type = TQVariant::Date;
	break;
    case FIELD_TYPE_TIME :
	type = TQVariant::Time;
	break;
    case FIELD_TYPE_DATETIME :
    case FIELD_TYPE_TIMESTAMP :
	type = TQVariant::DateTime;
	break;
    case FIELD_TYPE_BLOB :
    case FIELD_TYPE_TINY_BLOB :
    case FIELD_TYPE_MEDIUM_BLOB :
    case FIELD_TYPE_LONG_BLOB :
	type = (flags & BINARY_FLAG) ? TQVariant::ByteArray : TQVariant::CString;
	break;
    default:
    case FIELD_TYPE_ENUM :
    case FIELD_TYPE_SET :
    case FIELD_TYPE_STRING :
    case FIELD_TYPE_VAR_STRING :
	type = TQVariant::String;
	break;
    }
    return type;
}

TQMYSQLResult::TQMYSQLResult( const TQMYSQLDriver* db )
: TQSqlResult( db )
{
    d =   new TQMYSQLResultPrivate();
    d->mysql = db->d->mysql;
}

TQMYSQLResult::~TQMYSQLResult()
{
    cleanup();
    delete d;
}

MYSQL_RES* TQMYSQLResult::result()
{
    return d->result;
}

void TQMYSQLResult::cleanup()
{
    if ( d->result ) {
	mysql_free_result( d->result );
    }
    d->result = NULL;
    d->row = NULL;
    setAt( -1 );
    setActive( false );
}

bool TQMYSQLResult::fetch( int i )
{
    if ( isForwardOnly() ) { // fake a forward seek
	if ( at() < i ) {
	    int x = i - at();
	    while ( --x && fetchNext() );
	    return fetchNext();
	} else {
	    return false;
	}
    }    
    if ( at() == i )
	return true;
    mysql_data_seek( d->result, i );
    d->row = mysql_fetch_row( d->result );
    if ( !d->row )
	return false;
    setAt( i );
    return true;
}

bool TQMYSQLResult::fetchNext()
{
    d->row = mysql_fetch_row( d->result );
    if ( !d->row )
	return false;
    setAt( at() + 1 );
    return true;
}

bool TQMYSQLResult::fetchLast()
{
    if ( isForwardOnly() ) { // fake this since MySQL can't seek on forward only queries
	bool success = fetchNext(); // did we move at all?
	while ( fetchNext() );
	return success;
    }
    my_ulonglong numRows = mysql_num_rows( d->result );
    if ( !numRows )
	return false;
    return fetch( numRows - 1 );
}

bool TQMYSQLResult::fetchFirst()
{
    if ( isForwardOnly() ) // again, fake it
	return fetchNext();
    return fetch( 0 );
}

TQVariant TQMYSQLResult::data( int field )
{
    if ( !isSelect() || field >= (int) d->fieldTypes.count() ) {
	tqWarning( "TQMYSQLResult::data: column %d out of range", field );
	return TQVariant();
    }
    
    TQString val( d->row[field] );
    switch ( d->fieldTypes.at( field ) ) {
    case TQVariant::LongLong:
	return TQVariant( val.toLongLong() );
    case TQVariant::ULongLong:
	return TQVariant( val.toULongLong() );
    case TQVariant::Int: 
	return TQVariant( val.toInt() );
    case TQVariant::UInt:
	return TQVariant( val.toUInt() );
    case TQVariant::Double:
	return TQVariant( val.toDouble() );
    case TQVariant::Date:
	if ( val.isEmpty() ) {
	    return TQVariant( TQDate() );
	} else {
	    return TQVariant( TQDate::fromString( val, TQt::ISODate )  );
	}
    case TQVariant::Time:
	if ( val.isEmpty() ) {
	    return TQVariant( TQTime() );
	} else {
	    return TQVariant( TQTime::fromString( val, TQt::ISODate ) );
	}
    case TQVariant::DateTime:
	if ( val.isEmpty() )
	    return TQVariant( TQDateTime() );
	if ( val.length() == 14u )
	    // TIMESTAMPS have the format yyyyMMddhhmmss
	    val.insert(4, "-").insert(7, "-").insert(10, 'T').insert(13, ':').insert(16, ':');
	return TQVariant( TQDateTime::fromString( val, TQt::ISODate ) );
    case TQVariant::ByteArray: {
	unsigned long* fl = mysql_fetch_lengths( d->result );
	TQByteArray ba;
	ba.duplicate( d->row[field], fl[field] );
	return TQVariant( ba );
    }
    default:
    case TQVariant::String:
    case TQVariant::CString:
	return TQVariant( val );
    }
#ifdef QT_CHECK_RANGE
    tqWarning("TQMYSQLResult::data: unknown data type");
#endif
    return TQVariant();
}

bool TQMYSQLResult::isNull( int field )
{
    if ( d->row[field] == NULL )
	return true;
    return false;
}

bool TQMYSQLResult::reset ( const TQString& query )
{
    if ( !driver() )
	return false;
    if ( !driver()-> isOpen() || driver()->isOpenError() )
	return false;
    cleanup();

    const char *encQuery = query.ascii();
    if ( mysql_real_query( d->mysql, encQuery, tqstrlen(encQuery) ) ) {
	setLastError( qMakeError("Unable to execute query", TQSqlError::Statement, d ) );
	return false;
    }
    if ( isForwardOnly() ) {
	if ( isActive() || isValid() ) // have to empty the results from previous query
	    fetchLast();
	d->result = mysql_use_result( d->mysql );
    } else {
	d->result = mysql_store_result( d->mysql );
    }
    if ( !d->result && mysql_field_count( d->mysql ) > 0 ) {
	setLastError( qMakeError( "Unable to store result", TQSqlError::Statement, d ) );
	return false;
    }
    int numFields = mysql_field_count( d->mysql );
    setSelect( !( numFields == 0) );
    d->fieldTypes.resize( numFields );
    if ( isSelect() ) {
	for( int i = 0; i < numFields; i++) {
	    MYSQL_FIELD* field = mysql_fetch_field_direct( d->result, i );
	    if ( field->type == FIELD_TYPE_DECIMAL )
		d->fieldTypes[i] = TQVariant::String;
	    else
		d->fieldTypes[i] = qDecodeMYSQLType( field->type, field->flags );
	}
    }
    setActive( true );
    return true;
}

int TQMYSQLResult::size()
{
    return isSelect() ? (int)mysql_num_rows( d->result ) : -1;
}

int TQMYSQLResult::numRowsAffected()
{
    return (int)mysql_affected_rows( d->mysql );
}

/////////////////////////////////////////////////////////
static void qServerEnd()
{
#ifndef Q_NO_MYSQL_EMBEDDED
#if !defined(MARIADB_BASE_VERSION) && !defined(MARIADB_VERSION_ID)
# if MYSQL_VERSION_ID > 40000
#  if (MYSQL_VERSION_ID >= 40110 && MYSQL_VERSION_ID < 50000) || MYSQL_VERSION_ID >= 50003
    mysql_library_end();
#  else
    mysql_server_end();
#  endif
# endif
#endif
#endif
}

static void qServerInit()
{
#ifndef Q_NO_MYSQL_EMBEDDED
# if MYSQL_VERSION_ID >= 40000
    if (qMySqlInitHandledByUser || qMySqlConnectionCount > 1)
        return;

    if (
# if (MYSQL_VERSION_ID >= 40110 && MYSQL_VERSION_ID < 50000) || MYSQL_VERSION_ID >= 50003
        mysql_library_init(0, 0, 0)
# else
        mysql_server_init(0, 0, 0)
# endif
       ) {
        tqWarning("TQMYSQLDriver::qServerInit: unable to start server.");
    }
# endif // MYSQL_VERSION_ID
#endif // Q_NO_MYSQL_EMBEDDED

#if defined(MARIADB_BASE_VERSION) || defined(MARIADB_VERSION_ID)
    tqAddPostRoutine(mysql_server_end);
#endif
}

TQMYSQLDriver::TQMYSQLDriver( TQObject * parent, const char * name )
    : TQSqlDriver( parent, name ? name : TQMYSQL_DRIVER_NAME )
{
    init();
    qServerInit();
}

/*!
    Create a driver instance with an already open connection handle.
*/

TQMYSQLDriver::TQMYSQLDriver( MYSQL * con, TQObject * parent, const char * name )
    : TQSqlDriver( parent, name ? name : TQMYSQL_DRIVER_NAME )
{
    init();
    if ( con ) {
	d->mysql = (MYSQL *) con;
	setOpen( true );
	setOpenError( false );
        if (qMySqlConnectionCount == 1)
            qMySqlInitHandledByUser = true;
    } else {
        qServerInit();
    }
}

void TQMYSQLDriver::init()
{
    tqSqlOpenExtDict()->insert( this, new TQMYSQLOpenExtension(this) );
    d = new TQMYSQLDriverPrivate();
    d->mysql = 0;
    qMySqlConnectionCount++;
}

TQMYSQLDriver::~TQMYSQLDriver()
{
    qMySqlConnectionCount--;
    if (qMySqlConnectionCount == 0 && !qMySqlInitHandledByUser)
        qServerEnd();

    delete d;
    if ( !tqSqlOpenExtDict()->isEmpty() ) {
	TQSqlOpenExtension *ext = tqSqlOpenExtDict()->take( this );
	delete ext;
    }
}

bool TQMYSQLDriver::hasFeature( DriverFeature f ) const
{
    switch ( f ) {
    case Transactions:
// CLIENT_TRANSACTION should be defined in all recent mysql client libs > 3.23.34
#ifdef CLIENT_TRANSACTIONS
	if ( d->mysql ) {
	    if ( ( d->mysql->server_capabilities & CLIENT_TRANSACTIONS ) == CLIENT_TRANSACTIONS )
		return true;
	}
#endif
	return false;
    case QuerySize:
	return true;
    case BLOB:
	return true;
    case Unicode:
	return false;
    default:
	return false;
    }
}

bool TQMYSQLDriver::open( const TQString&,
			 const TQString&,
			 const TQString&,
			 const TQString&,
			 int )
{
    tqWarning("TQMYSQLDriver::open(): This version of open() is no longer supported." );
    return false;
}

bool TQMYSQLDriver::open( const TQString& db,
			 const TQString& user,
			 const TQString& password,
			 const TQString& host,
			 int port,
			 const TQString& connOpts )
{
    if ( isOpen() )
	close();
 
    unsigned int clientOptionFlags = 0;
    
    TQStringList raw = TQStringList::split( ';', connOpts );
    TQStringList opts;
    TQStringList::ConstIterator it;
    TQString	ssl_key, ssl_cert, ssl_ca, ssl_capath, ssl_cipher;
    
    // extract the real options from the string
    for ( it = raw.begin(); it != raw.end(); ++it ) {
	TQString tmp( *it ), name;
	int idx;
	if ( (idx = tmp.find( '=' )) != -1 ) {
	    TQString val( tmp.mid( idx + 1 ) );
	    val.simplifyWhiteSpace();
	    name = tmp.left( idx );
	    if (name == "MYSQL_SSL_KEY")
		ssl_key = val;
	    else if (name == "MYSQL_SSL_CERT")
		ssl_cert = val;
	    else if (name == "MYSQL_SSL_CA")
		ssl_ca = val;
	    else if (name == "MYSQL_SSL_CAPATH")
		ssl_capath = val;
	    else if (name == "MYSQL_SSL_CIPHER")
		ssl_capath = val;
	    else if ( val == "TRUE" || val == "1" )
		opts << tmp.left( idx );
	    else
		tqWarning( "TQMYSQLDriver::open: Illegal connect option value '%s'", tmp.latin1() );
	} else {
	    opts << tmp;
	}
    }

    if (!(d->mysql = mysql_init((MYSQL*) 0))) {
	    setLastError( qMakeError( "Unable to connect", TQSqlError::Connection, d ) );
	    mysql_close( d->mysql );
	    setOpenError( true );
	    return false;
    }

    bool reconnect = 0;
    for ( it = opts.begin(); it != opts.end(); ++it ) {
	TQString opt( (*it).upper() );
	if ( opt == "CLIENT_COMPRESS" )
	    clientOptionFlags |= CLIENT_COMPRESS;
	else if ( opt == "CLIENT_FOUND_ROWS" )
	    clientOptionFlags |= CLIENT_FOUND_ROWS;
	else if ( opt == "CLIENT_IGNORE_SPACE" )
	    clientOptionFlags |= CLIENT_IGNORE_SPACE;
	else if ( opt == "CLIENT_INTERACTIVE" )
	    clientOptionFlags |= CLIENT_INTERACTIVE;
	else if ( opt == "CLIENT_NO_SCHEMA" )
	    clientOptionFlags |= CLIENT_NO_SCHEMA;
	else if ( opt == "CLIENT_ODBC" )
	    clientOptionFlags |= CLIENT_ODBC;
	else if ( opt == "CLIENT_SSL" )
	    clientOptionFlags |= CLIENT_SSL;
	else if ( opt == "MYSQL_OPT_RECONNECT" )
	    reconnect = 1;
	else
	    tqWarning( "TQMYSQLDriver::open: Unknown connect option '%s'", (*it).latin1() );
    }

    if (clientOptionFlags & CLIENT_SSL) {
	mysql_ssl_set(d->mysql,
	    ssl_key.isEmpty()? static_cast<const char *>(0):ssl_key.local8Bit().data(),
	    ssl_cert.isEmpty()? static_cast<const char *>(0):ssl_cert.local8Bit().data(),
	    ssl_ca.isEmpty()? static_cast<const char *>(0):ssl_ca.local8Bit().data(),
	    ssl_capath.isEmpty()? static_cast<const char *>(0):ssl_capath.local8Bit().data(),
	    ssl_cipher.isEmpty()? static_cast<const char *>(0):ssl_cipher.local8Bit().data()
	);
    }
    mysql_options(d->mysql, MYSQL_OPT_RECONNECT, &reconnect);

    if ( mysql_real_connect( d->mysql,
				host,
				user,
				password,
				db.isNull() ? TQString("") : db,
				(port > -1) ? port : 0,
				NULL,
				clientOptionFlags ) )
    {
	if ( !db.isEmpty() && mysql_select_db( d->mysql, db )) {
	    setLastError( qMakeError("Unable open database '" + db + "'", TQSqlError::Connection, d ) );
	    mysql_close( d->mysql );
	    setOpenError( true );
	    return false;
	}
    } else {
	    setLastError( qMakeError( "Unable to connect", TQSqlError::Connection, d ) );
	    mysql_close( d->mysql );
	    setOpenError( true );
	    return false;
    }
    setOpen( true );
    setOpenError( false );
    return true;
}

void TQMYSQLDriver::close()
{
    if ( isOpen() ) {
	mysql_close( d->mysql );
	setOpen( false );
	setOpenError( false );
    }
}

bool TQMYSQLDriver::ping()
{
    if ( !isOpen() ) {
        return false;
    }

    if (mysql_ping( d->mysql )) {
	return true;
    }
    else {
	setLastError( qMakeError("Unable to execute ping", TQSqlError::Statement, d ) );
	return false;
    }
}

TQSqlQuery TQMYSQLDriver::createQuery() const
{
    return TQSqlQuery( new TQMYSQLResult( this ) );
}

TQStringList TQMYSQLDriver::tables( const TQString& typeName ) const
{
    TQStringList tl;
    if ( !isOpen() )
	return tl;
    if ( !typeName.isEmpty() && !(typeName.toInt() & (int)TQSql::Tables) )
	return tl;

    MYSQL_RES* tableRes = mysql_list_tables( d->mysql, NULL );
    MYSQL_ROW	row;
    int i = 0;
    while ( tableRes ) {
	mysql_data_seek( tableRes, i );
	row = mysql_fetch_row( tableRes );
	if ( !row )
	    break;
	tl.append( TQString(row[0]) );
	i++;
    }
    mysql_free_result( tableRes );
    return tl;
}

TQSqlIndex TQMYSQLDriver::primaryIndex( const TQString& tablename ) const
{
    TQSqlIndex idx;
    if ( !isOpen() )
	return idx;
    TQSqlQuery i = createQuery();
    TQString stmt( "show index from %1;" );
    TQSqlRecord fil = record( tablename );
    i.exec( stmt.arg( tablename ) );
    while ( i.isActive() && i.next() ) {
	if ( i.value(2).toString() == "PRIMARY" ) {
	    idx.append( *fil.field( i.value(4).toString() ) );
	    idx.setCursorName( i.value(0).toString() );
	    idx.setName( i.value(2).toString() );
	}
    }
    return idx;
}

TQSqlRecord TQMYSQLDriver::record( const TQString& tablename ) const
{
    TQSqlRecord fil;
    if ( !isOpen() )
	return fil;
    MYSQL_RES* r = mysql_list_fields( d->mysql, tablename.local8Bit().data(), 0);
    if ( !r ) {
	return fil;
    }
    MYSQL_FIELD* field;
    while ( (field = mysql_fetch_field( r ))) {
	TQSqlField f ( TQString( field->name ) , qDecodeMYSQLType( (int)field->type, field->flags ) );
	fil.append ( f );
    }
    mysql_free_result( r );
    return fil;
}

TQSqlRecord TQMYSQLDriver::record( const TQSqlQuery& query ) const
{
    TQSqlRecord fil;
    if ( !isOpen() )
	return fil;
    if ( query.isActive() && query.isSelect() && query.driver() == this ) {
	TQMYSQLResult* result =  (TQMYSQLResult*)query.result();
	TQMYSQLResultPrivate* p = result->d;
	if ( !mysql_errno( p->mysql ) ) {
	    for ( ;; ) {
		MYSQL_FIELD* f = mysql_fetch_field( p->result );
		if ( f ) {
		    TQSqlField fi( TQString((const char*)f->name), qDecodeMYSQLType( f->type, f->flags ) );
		    fil.append( fi  );
		} else
		    break;
	    }
	}
	mysql_field_seek( p->result, 0 );
    }
    return fil;
}

TQSqlRecordInfo TQMYSQLDriver::recordInfo( const TQString& tablename ) const
{
    TQSqlRecordInfo info;
    if ( !isOpen() )
	return info;
    MYSQL_RES* r = mysql_list_fields( d->mysql, tablename.local8Bit().data(), 0);
    if ( !r ) {
	return info;
    }
    MYSQL_FIELD* field;
    while ( (field = mysql_fetch_field( r ))) {
	info.append ( TQSqlFieldInfo( TQString( field->name ),
				qDecodeMYSQLType( (int)field->type, field->flags ),
				IS_NOT_NULL( field->flags ),
				(int)field->length,
				(int)field->decimals,
				TQString( field->def ),
				(int)field->type ) );
    }
    mysql_free_result( r );
    return info;
}

TQSqlRecordInfo TQMYSQLDriver::recordInfo( const TQSqlQuery& query ) const
{
    TQSqlRecordInfo info;
    if ( !isOpen() )
	return info;
    if ( query.isActive() && query.isSelect() && query.driver() == this ) {
	TQMYSQLResult* result =  (TQMYSQLResult*)query.result();
	TQMYSQLResultPrivate* p = result->d;
	if ( !mysql_errno( p->mysql ) ) {
	    for ( ;; ) {
		MYSQL_FIELD* field = mysql_fetch_field( p->result );
		if ( field ) {
		    info.append ( TQSqlFieldInfo( TQString( field->name ),
				qDecodeMYSQLType( (int)field->type, field->flags ),
				IS_NOT_NULL( field->flags ),
				(int)field->length,
				(int)field->decimals,
				TQVariant(),
				(int)field->type ) );
		
		} else
		    break;
	    }
	}
	mysql_field_seek( p->result, 0 );
    }
    return info;
}

MYSQL* TQMYSQLDriver::mysql()
{
     return d->mysql;
}

bool TQMYSQLDriver::beginTransaction()
{
#ifndef CLIENT_TRANSACTIONS
    return false;
#endif
    if ( !isOpen() ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQMYSQLDriver::beginTransaction: Database not open" );
#endif
	return false;
    }
    if ( mysql_query( d->mysql, "BEGIN WORK" ) ) {
	setLastError( qMakeError("Unable to begin transaction", TQSqlError::Statement, d ) );
	return false;
    }
    return true;
}

bool TQMYSQLDriver::commitTransaction()
{
#ifndef CLIENT_TRANSACTIONS
    return false;
#endif
    if ( !isOpen() ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQMYSQLDriver::commitTransaction: Database not open" );
#endif
	return false;
    }
    if ( mysql_query( d->mysql, "COMMIT" ) ) {
	setLastError( qMakeError("Unable to commit transaction", TQSqlError::Statement, d ) );
	return false;
    }
    return true;
}

bool TQMYSQLDriver::rollbackTransaction()
{
#ifndef CLIENT_TRANSACTIONS
    return false;
#endif
    if ( !isOpen() ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQMYSQLDriver::rollbackTransaction: Database not open" );
#endif
	return false;
    }
    if ( mysql_query( d->mysql, "ROLLBACK" ) ) {
	setLastError( qMakeError("Unable to rollback transaction", TQSqlError::Statement, d ) );
	return false;
    }
    return true;
}

TQString TQMYSQLDriver::formatValue( const TQSqlField* field, bool trimStrings ) const
{
    TQString r;
    if ( field->isNull() ) {
	r = nullText();
    } else {
	switch( field->type() ) {
	case TQVariant::ByteArray: {
	
	    const TQByteArray ba = field->value().toByteArray();
	    // buffer has to be at least length*2+1 bytes
	    char* buffer = new char[ ba.size() * 2 + 1 ];
	    /*uint escapedSize =*/ mysql_escape_string( buffer, ba.data(), ba.size() );
	    r.append("'").append(buffer).append("'");
	    delete[] buffer;
	}
	break;
	case TQVariant::String:
	case TQVariant::CString: {
	    // Escape '\' characters
	    r = TQSqlDriver::formatValue( field );
	    r.replace( "\\", "\\\\" );
	    break;
	}
	default:
	    r = TQSqlDriver::formatValue( field, trimStrings );
	}
    }
    return r;
}
