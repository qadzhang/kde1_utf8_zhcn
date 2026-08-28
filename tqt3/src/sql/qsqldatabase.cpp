/****************************************************************************
**
** Implementation of TQSqlDatabase class
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

#include "ntqsqldatabase.h"

#ifndef TQT_NO_SQL

#ifdef Q_OS_WIN32
// Conflicting declarations of LPCBYTE in sqlfront.h and winscard.h
#define _WINSCARD_H_
#endif

#ifdef QT_SQL_POSTGRES
#include "drivers/psql/qsql_psql.h"
#endif
#ifdef QT_SQL_MYSQL
#include "drivers/mysql/qsql_mysql.h"
#endif
#ifdef QT_SQL_ODBC
#include "drivers/odbc/qsql_odbc.h"
#endif
#ifdef QT_SQL_OCI
#include "drivers/oci/qsql_oci.h"
#endif
#ifdef QT_SQL_TDS
#include "drivers/tds/qsql_tds.h"
#endif
#ifdef QT_SQL_DB2
#include "drivers/db2/qsql_db2.h"
#endif
#ifdef QT_SQL_SQLITE
#include "drivers/sqlite/qsql_sqlite.h"
#endif
#ifdef QT_SQL_SQLITE3
#include "drivers/sqlite3/qsql_sqlite3.h"
#endif
#ifdef QT_SQL_IBASE
#include "drivers/ibase/qsql_ibase.h"
#endif

#include "ntqapplication.h"
#include "ntqsqlresult.h"
#include "ntqsqldriver.h"
#include "qsqldriverinterface_p.h"
#include <private/qpluginmanager_p.h>
#include <private/qsqlextension_p.h>
#include "ntqobject.h"
#include "ntqguardedptr.h"
#include "ntqcleanuphandler.h"
#include "ntqdict.h"
#include <stdlib.h>

const char * const TQSqlDatabase::defaultConnection = "qt_sql_default_connection";

TQPtrDict<TQSqlDriverExtension> *qt_driver_extension_dict = 0;
TQPtrDict<TQSqlOpenExtension> *qt_open_extension_dict = 0;

static TQSingleCleanupHandler< TQPtrDict<TQSqlDriverExtension> > qt_driver_ext_cleanup;
static TQSingleCleanupHandler< TQPtrDict<TQSqlOpenExtension> > qt_open_ext_cleanup;

TQ_EXPORT TQPtrDict<TQSqlDriverExtension> *tqSqlDriverExtDict()
{
    if ( !qt_driver_extension_dict ) {
	qt_driver_extension_dict = new TQPtrDict<TQSqlDriverExtension>;
	qt_driver_ext_cleanup.set( &qt_driver_extension_dict );
    }
    return qt_driver_extension_dict;
}

TQ_EXPORT TQPtrDict<TQSqlOpenExtension> *tqSqlOpenExtDict()
{
    if ( !qt_open_extension_dict ) {
	qt_open_extension_dict = new TQPtrDict<TQSqlOpenExtension>;
	qt_open_ext_cleanup.set( &qt_open_extension_dict );
    }
    return qt_open_extension_dict;
}

class TQNullResult : public TQSqlResult
{
public:
    TQNullResult(const TQSqlDriver* d): TQSqlResult(d){}
    ~TQNullResult(){}
protected:
    TQVariant    data( int ) { return TQVariant(); }
    bool        reset ( const TQString& sqlquery ) { TQString s(sqlquery); return false; }
    bool        fetch( int i ) { i = i; return false; }
    bool        fetchFirst() { return false; }
    bool        fetchLast() { return false; }
    bool        isNull( int ) {return false; }
    TQSqlRecord 	record() {return TQSqlRecord();}
    int         size()  {return 0;}
    int         numRowsAffected() {return 0;}
};

class TQNullDriver : public TQSqlDriver
{
public:
    TQNullDriver(): TQSqlDriver(){}
    ~TQNullDriver(){}
    bool    hasFeature( DriverFeature /* f */ ) const { return false; } ;
    bool    open( const TQString & ,
		  const TQString & ,
		  const TQString & ,
		  const TQString &,
		  int ) {
	return false;
    }
    void    close() {}
    bool    ping() { return true; }
    TQSqlQuery createQuery() const { return TQSqlQuery( new TQNullResult(this) ); }
};

TQSqlDriverCreatorBase::TQSqlDriverCreatorBase() {
}

TQSqlDriverCreatorBase::~TQSqlDriverCreatorBase() {
}

typedef TQDict<TQSqlDriverCreatorBase> TQDriverDict;

class TQSqlDatabaseManager : public TQObject
{
public:
    TQSqlDatabaseManager( TQObject * parent = 0, const char * name = 0 );
    ~TQSqlDatabaseManager();
    static TQSqlDatabase* database( const TQString& name, bool open );
    static TQSqlDatabase* addDatabase( TQSqlDatabase* db, const TQString & name );
    static void          removeDatabase( const TQString& name );
    static void          removeDatabase( TQSqlDatabase* db );
    static bool          contains( const TQString& name );
    static TQDriverDict*  driverDict();

protected:
    static TQSqlDatabaseManager* instance();
    TQDict< TQSqlDatabase > dbDict;
    TQDriverDict* drDict;
};

/*!
    Constructs an SQL database manager.
*/

TQSqlDatabaseManager::TQSqlDatabaseManager( TQObject * parent, const char * name )
    : TQObject( parent, name ), dbDict( 1 ), drDict( 0 )
{
}

/*!
    Destroys the object and frees any allocated resources. All open
    database connections are closed. All database connections are
    deleted.
*/

TQSqlDatabaseManager::~TQSqlDatabaseManager()
{
    TQDictIterator< TQSqlDatabase > it( dbDict );
    while ( it.current() ) {
	it.current()->close();
	delete it.current();
	++it;
    }
    delete drDict;
}

/*!
  \internal
*/
TQDriverDict* TQSqlDatabaseManager::driverDict()
{
    TQSqlDatabaseManager* sqlConnection = instance();
    if ( !sqlConnection->drDict ) {
	sqlConnection->drDict = new TQDriverDict();
	sqlConnection->drDict->setAutoDelete( true );
    }
    return sqlConnection->drDict;
}


/*!
  \internal
*/
TQSqlDatabaseManager* TQSqlDatabaseManager::instance()
{
    static TQGuardedPtr<TQSqlDatabaseManager> sqlConnection = 0;
    if ( !sqlConnection ) {
	if( tqApp == 0 ){
	    tqFatal( "TQSqlDatabaseManager: A TQApplication object has to be "
		    "instantiated in order to use the SQL module." );
	    return 0;
	}
	sqlConnection = new TQSqlDatabaseManager( tqApp, "database manager" );
    }
    return (TQSqlDatabaseManager*)sqlConnection;
}

/*!
    Returns the database connection called \a name. If \a open is
    true, the database connection is opened. If \a name does not exist
    in the list of managed databases, 0 is returned.
*/

TQSqlDatabase* TQSqlDatabaseManager::database( const TQString& name, bool open )
{
    if ( !contains( name ) )
	return 0;

    TQSqlDatabaseManager* sqlConnection = instance();
    TQSqlDatabase* db = sqlConnection->dbDict.find( name );
    if ( db && !db->isOpen() && open ) {
	db->open();
#ifdef QT_CHECK_RANGE
	if ( !db->isOpen() )
	    tqWarning("TQSqlDatabaseManager::database: unable to open database: %s: %s",
                    db->lastError().databaseText().latin1(), db->lastError().driverText().latin1() );
#endif
    }
    return db;
}

/*!
    Returns true if the list of database connections contains \a name;
    otherwise returns false.
*/

bool TQSqlDatabaseManager::contains( const TQString& name )
{
   TQSqlDatabaseManager* sqlConnection = instance();
   TQSqlDatabase* db = sqlConnection->dbDict.find( name );
   if ( db )
       return true;
   return false;
}


/*!
    Adds a database to the SQL connection manager. The database
    connection is referred to by \a name. The newly added database
    connection is returned. This function will only return 0 if it is
    called \e before a TQApplication object has been instantiated. Use
    the output of drivers() to determine whether a particular driver
    is available or not.

    The returned TQSqlDatabase object is owned by the framework and
    must not be deleted. If you want to explicitly remove the connection,
    use removeDatabase().

    \sa TQSqlDatabase database()
*/

TQSqlDatabase* TQSqlDatabaseManager::addDatabase( TQSqlDatabase* db, const TQString & name )
{
    TQSqlDatabaseManager* sqlConnection = instance();
    if( sqlConnection == 0 )
	return 0;
    if ( contains( name ) )
	sqlConnection->removeDatabase( name );
    sqlConnection->dbDict.insert( name, db );
    return db;
}

/*!
    Removes the database connection \a name from the SQL connection
    manager.

    \warning There should be no open queries on the database
    connection when this function is called, otherwise a resource leak
    will occur.
*/

void TQSqlDatabaseManager::removeDatabase( const TQString& name )
{
    TQSqlDatabaseManager* sqlConnection = instance();
    sqlConnection->dbDict.setAutoDelete( true );
    sqlConnection->dbDict.remove( name );
    sqlConnection->dbDict.setAutoDelete( false );
}


/*!
    Removes the database connection \a db from the SQL connection
    manager. The TQSqlDatabase object is destroyed when it is removed
    from the manager.

    \warning The \a db pointer is not valid after this function has
    been called.
*/

void TQSqlDatabaseManager::removeDatabase( TQSqlDatabase* db )
{
    TQSqlDatabaseManager* sqlConnection = instance();
    if ( !sqlConnection )
	return;
    TQDictIterator< TQSqlDatabase > it( sqlConnection->dbDict );
    while ( it.current() ) {
	if ( it.current() == db ) {
	    sqlConnection->dbDict.remove( it.currentKey() );
	    db->close();
	    delete db;
	    break;
	}
	++it;
    }
}

class TQSqlDatabasePrivate
{
public:
    TQSqlDatabasePrivate():
	driver(0),
#ifndef TQT_NO_COMPONENT
	plugIns(0),
#endif
	port(-1) {}
    ~TQSqlDatabasePrivate()
    {
    }
    TQSqlDriver* driver;
#ifndef TQT_NO_COMPONENT
    TQPluginManager<TQSqlDriverFactoryInterface> *plugIns;
#endif
    TQString dbname;
    TQString uname;
    TQString pword;
    TQString hname;
    TQString drvName;
    int port;
    TQString connOptions;
};

/*!
    \class TQSqlDatabase ntqsqldatabase.h
    \brief The TQSqlDatabase class is used to create SQL database
    connections and to provide transaction handling.

    \ingroup database
    \mainclass
    \module sql

    Note that transaction handling is not supported by every SQL
    database. You can find out whether transactions are supported
    using TQSqlDriver::hasFeature().

    The TQSqlDatabase class provides an abstract interface for
    accessing many types of database backends. Database-specific
    drivers are used internally to actually access and manipulate
    data, (see TQSqlDriver). Result set objects provide the interface
    for executing and manipulating SQL queries (see TQSqlQuery).
*/

/*!
    Adds a database to the list of database connections using the
    driver \a type and the connection name \a connectionName.

    The database connection is referred to by \a connectionName. The
    newly added database connection is returned. This pointer is owned
    by TQSqlDatabase and will be deleted on program exit or when
    removeDatabase() is called.

    If \a connectionName is not specified, the newly added database
    connection becomes the default database connection for the
    application, and subsequent calls to database() (without a
    database name parameter) will return a pointer to it. If \a
    connectionName is given, use \link TQSqlDatabase::database()
    database(connectionName)\endlink to retrieve a pointer to the
    database connection.

    \warning If you add a database with the same name as an
    existing database, the new database will replace the old one.
    This will happen automatically if you call this function more
    than once without specifying \a connectionName.

    \sa database() removeDatabase()
*/
TQSqlDatabase* TQSqlDatabase::addDatabase( const TQString& type, const TQString& connectionName )
{
    return TQSqlDatabaseManager::addDatabase( new TQSqlDatabase( type, connectionName ), connectionName );
}

/*!
    Returns the database connection called \a connectionName. The
    database connection must have been previously added with
    addDatabase(). If \a open is true (the default) and the database
    connection is not already open it is opened now. If no \a
    connectionName is specified the default connection is used. If \a
    connectionName does not exist in the list of databases, 0 is
    returned. The pointer returned is owned by TQSqlDatabase and should
    \e not be deleted.

    \warning There are restrictions on the use of database connections
    in threaded applications. Please see the \link threads.html#threads-sql
    Thread Support in TQt\endlink document for more information about
    threading and SQL databases.
*/

TQSqlDatabase* TQSqlDatabase::database( const TQString& connectionName, bool open )
{
    return TQSqlDatabaseManager::database( connectionName, open );
}

/*!
    Removes the database connection \a connectionName from the list of
    database connections.

    \warning There should be no open queries on the database
    connection when this function is called, otherwise a resource leak
    will occur.
*/

void TQSqlDatabase::removeDatabase( const TQString& connectionName )
{
    TQSqlDatabaseManager::removeDatabase( connectionName );
}

/*!
    \overload

    Removes the database connection \a db from the list of database
    connections. The TQSqlDatabase object is destroyed when it is removed
    from the list.

    \warning The \a db pointer is not valid after this function has
    been called. There should be no open queries on the database
    connection when this function is called, otherwise a resource leak
    will occur.
*/

void TQSqlDatabase::removeDatabase( TQSqlDatabase* db )
{
    TQSqlDatabaseManager::removeDatabase( db );
}

/*!
    Returns a list of all the available database drivers.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = TQSqlDatabase::drivers();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode
*/

TQStringList TQSqlDatabase::drivers()
{
    TQStringList l;

#ifndef TQT_NO_COMPONENT
    TQPluginManager<TQSqlDriverFactoryInterface> *plugIns;
    plugIns = new TQPluginManager<TQSqlDriverFactoryInterface>( IID_QSqlDriverFactory, TQApplication::libraryPaths(), "/sqldrivers" );

    l = plugIns->featureList();
    delete plugIns;
#endif

    TQDictIterator<TQSqlDriverCreatorBase> itd( *TQSqlDatabaseManager::driverDict() );
    while ( itd.current() ) {
	if ( !l.contains( itd.currentKey() ) )
	    l << itd.currentKey();
	++itd;
    }

#ifdef QT_SQL_POSTGRES
    if ( !l.contains( "TQPSQL7" ) )
	l << "TQPSQL7";
#endif
#ifdef QT_SQL_MYSQL
    if ( !l.contains( "TQMYSQL3" ) )
	l << "TQMYSQL3";
#endif
#ifdef QT_SQL_ODBC
    if ( !l.contains( "TQODBC3" ) )
	l << "TQODBC3";
#endif
#ifdef QT_SQL_OCI
    if ( !l.contains( "TQOCI8" ) )
	l << "TQOCI8";
#endif
#ifdef QT_SQL_TDS
    if ( !l.contains( "TQTDS7" ) )
	l << "TQTDS7";
#endif
#ifdef QT_SQL_DB2
    if ( !l.contains( "TQDB2" ) )
	l << "TQDB2";
#endif
#ifdef QT_SQL_SQLITE
    if ( !l.contains( "TQSQLITE" ) )
	l << "TQSQLITE";
#endif
#ifdef QT_SQL_SQLITE3
    if ( !l.contains( "TQSQLITE3" ) )
	l << "TQSQLITE3";
#endif
#ifdef QT_SQL_IBASE
    if ( !l.contains( "TQIBASE" ) )
	l << "TQIBASE";
#endif

    return l;
}

/*!
    This function registers a new SQL driver called \a name, within
    the SQL framework. This is useful if you have a custom SQL driver
    and don't want to compile it as a plugin.

    Example usage:

    \code
    TQSqlDatabase::registerSqlDriver( "MYDRIVER", new TQSqlDriverCreator<MyDatabaseDriver> );
    TQSqlDatabase* db = TQSqlDatabase::addDatabase( "MYDRIVER" );
    ...
    \endcode

    \warning The framework takes ownership of the \a creator pointer,
    so it should not be deleted.
*/
void TQSqlDatabase::registerSqlDriver( const TQString& name, const TQSqlDriverCreatorBase* creator )
{
    TQSqlDatabaseManager::driverDict()->remove( name );
    if ( creator )
	TQSqlDatabaseManager::driverDict()->insert( name, creator );
}

/*!
    Returns true if the list of database connections contains \a
    connectionName; otherwise returns false.
*/

bool TQSqlDatabase::contains( const TQString& connectionName )
{
    return TQSqlDatabaseManager::contains( connectionName );
}


/*!
    Creates a TQSqlDatabase connection called \a name that uses the
    driver referred to by \a type, with the parent \a parent and the
    object name \a objname. If the \a type is not recognized, the
    database connection will have no functionality.

    The currently available drivers are:

    \table
    \header \i Driver Type \i Description
    \row \i TQODBC3 \i ODBC Driver (includes Microsoft SQL Server)
    \row \i TQOCI8 \i Oracle Call Interface Driver
    \row \i TQPSQL7 \i PostgreSQL v6.x and v7.x Driver
    \row \i TQTDS7 \i Sybase Adaptive Server
    \row \i TQMYSQL3 \i MySQL Driver
    \row \i TQDB2 \i IBM DB2, v7.1 and higher
    \row \i TQSQLITE \i SQLite Driver
    \row \i TQIBASE \i Borland Interbase Driver
    \endtable

    Additional third party drivers, including your own custom drivers,
    can be loaded dynamically.

    \sa registerSqlDriver()
*/

TQSqlDatabase::TQSqlDatabase( const TQString& type, const TQString& name, TQObject * parent, const char * objname )
    : TQObject( parent, objname )
{
    init( type, name );
}


/*!
    \overload

     Creates a database connection using the driver \a driver, with
     the parent \a parent and the object name \a objname.

     \warning The framework takes ownership of the \a driver pointer,
     so it should not be deleted.
*/

TQSqlDatabase::TQSqlDatabase( TQSqlDriver* driver, TQObject * parent, const char * objname )
    : TQObject( parent, objname )
{
    d = new TQSqlDatabasePrivate();
    d->driver = driver;
}

/*!
  \internal

  Create the actual driver instance \a type.
*/

void TQSqlDatabase::init( const TQString& type, const TQString& )
{
    d = new TQSqlDatabasePrivate();
    d->drvName = type;

    if ( !d->driver ) {

#ifdef QT_SQL_POSTGRES
	if ( type == "TQPSQL7" )
	    d->driver = new TQPSQLDriver();
#endif

#ifdef QT_SQL_MYSQL
	if ( type == "TQMYSQL3" )
	    d->driver = new TQMYSQLDriver();
#endif

#ifdef QT_SQL_ODBC
	if ( type == "TQODBC3" )
	    d->driver = new TQODBCDriver();
#endif

#ifdef QT_SQL_OCI
	if ( type == "TQOCI8" )
	    d->driver = new TQOCIDriver();
#endif

#ifdef QT_SQL_TDS
	if ( type == "TQTDS7" )
	    d->driver = new TQTDSDriver();
#endif

#ifdef QT_SQL_DB2
	if ( type == "TQDB2" )
	    d->driver = new TQDB2Driver();
#endif

#ifdef QT_SQL_SQLITE
	if ( type == "TQSQLITE" )
	    d->driver = new TQSQLiteDriver();
#endif

#ifdef QT_SQL_SQLITE3
	if ( type == "TQSQLITE3" )
	    d->driver = new TQSQLite3Driver();
#endif

#ifdef QT_SQL_IBASE
	if ( type == "TQIBASE" )
	    d->driver = new TQIBaseDriver();
#endif

    }

    if ( !d->driver ) {
	TQDictIterator<TQSqlDriverCreatorBase> it( *TQSqlDatabaseManager::driverDict() );
	while ( it.current() && !d->driver ) {
	    if ( type == it.currentKey() ) {
		d->driver = it.current()->createObject();
	    }
	    ++it;
	}
    }

#ifndef TQT_NO_COMPONENT
    if ( !d->driver ) {
	d->plugIns =
	    new TQPluginManager<TQSqlDriverFactoryInterface>( IID_QSqlDriverFactory, TQApplication::libraryPaths(), "/sqldrivers" );

	TQInterfacePtr<TQSqlDriverFactoryInterface> iface = 0;
	d->plugIns->queryInterface( type, &iface );
	if( iface )
	    d->driver = iface->create( type );
    }
#endif

    if ( !d->driver ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQSqlDatabase: %s driver not loaded", type.latin1() );
	tqWarning( "TQSqlDatabase: available drivers: %s", drivers().join(" ").latin1() );
#endif
	d->driver = new TQNullDriver();
	d->driver->setLastError( TQSqlError( "Driver not loaded", "Driver not loaded" ) );
    }
}

/*!
    Destroys the object and frees any allocated resources.
*/

TQSqlDatabase::~TQSqlDatabase()
{
    delete d->driver;
#ifndef TQT_NO_COMPONENT
    delete d->plugIns;
#endif
    delete d;
}

/*!
    Executes a SQL statement (e.g. an \c INSERT, \c UPDATE or \c
    DELETE statement) on the database, and returns a TQSqlQuery object.
    Use lastError() to retrieve error information. If \a query is
    TQString::null, an empty, invalid query is returned and lastError()
    is not affected.

    \sa TQSqlQuery lastError()
*/

TQSqlQuery TQSqlDatabase::exec( const TQString & query ) const
{
    TQSqlQuery r = d->driver->createQuery();
    if ( !query.isNull() ) {
	r.exec( query );
	d->driver->setLastError( r.lastError() );
    }
    return r;
}

/*!
    Opens the database connection using the current connection values.
    Returns true on success; otherwise returns false. Error
    information can be retrieved using the lastError() function.

    \sa lastError()
*/

bool TQSqlDatabase::open()
{
    return d->driver->open( d->dbname, d->uname, d->pword, d->hname,
			    d->port, d->connOptions );
}

/*!
    \overload

    Opens the database connection using the given \a user name and \a
    password. Returns true on success; otherwise returns false. Error
    information can be retrieved using the lastError() function.

    This function does not store the password it is given. Instead,
    the password is passed directly to the driver for opening a
    connection and is then discarded.

    \sa lastError()
*/

bool TQSqlDatabase::open( const TQString& user, const TQString& password )
{
    setUserName( user );
    return d->driver->open( d->dbname, user, password, d->hname,
			    d->port, d->connOptions );
}

/*!
    Closes the database connection, freeing any resources acquired.

    \sa removeDatabase()
*/

void TQSqlDatabase::close()
{
    d->driver->close();
}

/*!
    Sends a ping to the database server.
*/

bool TQSqlDatabase::ping()
{
    return d->driver->ping();
}

/*!
    Returns true if the database connection is currently open;
    otherwise returns false.
*/

bool TQSqlDatabase::isOpen() const
{
    return d->driver->isOpen();
}

/*!
    Returns true if there was an error opening the database
    connection; otherwise returns false. Error information can be
    retrieved using the lastError() function.
*/

bool TQSqlDatabase::isOpenError() const
{
    return d->driver->isOpenError();
}

/*!
    Begins a transaction on the database if the driver supports
    transactions. Returns true if the operation succeeded; otherwise
    returns false.

    \sa TQSqlDriver::hasFeature() commit() rollback()
*/

bool TQSqlDatabase::transaction()
{
    if ( !d->driver->hasFeature( TQSqlDriver::Transactions ) )
	return false;
    return d->driver->beginTransaction();
}

/*!
    Commits a transaction to the database if the driver supports
    transactions. Returns true if the operation succeeded; otherwise
    returns false.

    \sa TQSqlDriver::hasFeature() rollback()
*/

bool TQSqlDatabase::commit()
{
    if ( !d->driver->hasFeature( TQSqlDriver::Transactions ) )
	return false;
    return d->driver->commitTransaction();
}

/*!
    Rolls a transaction back on the database if the driver supports
    transactions. Returns true if the operation succeeded; otherwise
    returns false.

    \sa TQSqlDriver::hasFeature() commit() transaction()
*/

bool TQSqlDatabase::rollback()
{
    if ( !d->driver->hasFeature( TQSqlDriver::Transactions ) )
	return false;
    return d->driver->rollbackTransaction();
}

/*!
    \property TQSqlDatabase::databaseName
    \brief the name of the database

    Note that the database name is the TNS Service Name for the TQOCI8
    (Oracle) driver.

    For the TQODBC3 driver it can either be a DSN, a DSN filename (the
    file must have a \c .dsn extension), or a connection string. MS
    Access users can for example use the following connection string
    to open a \c .mdb file directly, instead of having to create a DSN
    entry in the ODBC manager:

    \code
    ...
    db = TQSqlDatabase::addDatabase( "TQODBC3" );
    db->setDatabaseName( "DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=myaccessfile.mdb" );
    if ( db->open() ) {
        // success!
    }
    ...
    \endcode
    ("FIL" is the required spelling in Microsoft's API.)

    There is no default value.
*/

void TQSqlDatabase::setDatabaseName( const TQString& name )
{
    d->dbname = name;
}

/*!
    \property TQSqlDatabase::userName
    \brief the user name connected to the database

    There is no default value.
*/

void TQSqlDatabase::setUserName( const TQString& name )
{
    d->uname = name;
}

/*!
    \property TQSqlDatabase::password
    \brief the password used to connect to the database

    There is no default value.

    \warning This function stores the password in plain text within
    TQt. Use the open() call that takes a password as parameter to
    avoid this behaviour.

    \sa open()
*/

void TQSqlDatabase::setPassword( const TQString& password )
{
    d->pword = password;
}

/*!
    \property TQSqlDatabase::hostName
    \brief the host name where the database resides

    There is no default value.
*/

void TQSqlDatabase::setHostName( const TQString& host )
{
    d->hname = host;
}

/*!
    \property TQSqlDatabase::port
    \brief the port used to connect to the database

    There is no default value.
*/

void TQSqlDatabase::setPort( int p )
{
    d->port = p;
}

TQString TQSqlDatabase::databaseName() const
{
    return d->dbname;
}

TQString TQSqlDatabase::userName() const
{
    return d->uname;
}

TQString TQSqlDatabase::password() const
{
    return d->pword;
}

TQString TQSqlDatabase::hostName() const
{
    return d->hname;
}

/*!
    Returns the name of the driver used by the database connection.
*/
TQString TQSqlDatabase::driverName() const
{
    return d->drvName;
}

int TQSqlDatabase::port() const
{
    return d->port;
}

/*!
    Returns the database driver used to access the database
    connection.
*/

TQSqlDriver* TQSqlDatabase::driver() const
{
    return d->driver;
}

/*!
    Returns information about the last error that occurred on the
    database. See TQSqlError for more information.
*/

TQSqlError TQSqlDatabase::lastError() const
{
    return d->driver->lastError();
}


/*!
    \overload

    Returns a list of the database's tables that are visible to the
    user. To include views or system tables, use the version of this
    function that takes a table \c type parameter.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myDatabase.tables();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode
*/

TQStringList TQSqlDatabase::tables() const
{
    return tables( TQSql::Tables );
}

/*!
    Returns a list of the database's tables, system tables and views,
    as specified by the parameter \a type.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myDatabase.tables( TQSql::Tables | TQSql::Views );
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
        myProcessing( *it );
        ++it;
    }
    \endcode
*/

TQStringList TQSqlDatabase::tables( TQSql::TableType type ) const
{
    return d->driver->tables( TQString::number( (int)type ) );
}

/*!
    Returns the primary index for table \a tablename. If no primary
    index exists an empty TQSqlIndex will be returned.
*/

TQSqlIndex TQSqlDatabase::primaryIndex( const TQString& tablename ) const
{
    return d->driver->primaryIndex( tablename );
}


/*!
    Returns a TQSqlRecord populated with the names of all the fields in
    the table (or view) called \a tablename. The order in which the
    fields appear in the record is undefined. If no such table (or
    view) exists, an empty record is returned.

    \sa recordInfo()
*/

TQSqlRecord TQSqlDatabase::record( const TQString& tablename ) const
{
    return d->driver->record( tablename );
}


/*!
    \overload

    Returns a TQSqlRecord populated with the names of all the fields
    used in the SQL \a query. If the query is a "SELECT *" the order
    in which fields appear in the record is undefined.

    \sa recordInfo()
*/

TQSqlRecord TQSqlDatabase::record( const TQSqlQuery& query ) const
{
    return d->driver->record( query );
}

/*!
    Returns a TQSqlRecordInfo populated with meta data about the table
    or view \a tablename. If no such table (or view) exists, an empty
    record is returned.

    \sa TQSqlRecordInfo, TQSqlFieldInfo, record()
*/
TQSqlRecordInfo TQSqlDatabase::recordInfo( const TQString& tablename ) const
{
    return d->driver->recordInfo( tablename );
}

/*!
    \overload

    Returns a TQSqlRecordInfo object with meta data for the TQSqlQuery
    \a query. Note that this overloaded function may return less
    information than the recordInfo() function which takes the name of
    a table as parameter.

    \sa TQSqlRecordInfo, TQSqlFieldInfo, record()
*/
TQSqlRecordInfo TQSqlDatabase::recordInfo( const TQSqlQuery& query ) const
{
    return d->driver->recordInfo( query );
}

/*!
    \property TQSqlDatabase::connectOptions
    \brief the database connect options

    The format of the options string is a semi-colon separated list of
    option names or option = value pairs. The options depend on the
    database client used:

    \table
    \header \i ODBC \i MySQL \i PostgreSQL
    \row

    \i
    \list
    \i SQL_ATTR_ACCESS_MODE
    \i SQL_ATTR_LOGIN_TIMEOUT
    \i SQL_ATTR_CONNECTION_TIMEOUT
    \i SQL_ATTR_CURRENT_CATALOG
    \i SQL_ATTR_METADATA_ID
    \i SQL_ATTR_PACKET_SIZE
    \i SQL_ATTR_TRACEFILE
    \i SQL_ATTR_TRACE
    \endlist

    \i
    \list
    \i CLIENT_COMPRESS
    \i CLIENT_FOUND_ROWS
    \i CLIENT_IGNORE_SPACE
    \i CLIENT_SSL
    \i CLIENT_ODBC
    \i CLIENT_NO_SCHEMA
    \i CLIENT_INTERACTIVE
    \endlist

    \i
    \list
    \i connect_timeout
    \i options
    \i tty
    \i requiressl
    \i service
    \endlist

    \header \i DB2 \i OCI \i TDS
    \row

    \i
    \list
    \i SQL_ATTR_ACCESS_MODE
    \i SQL_ATTR_LOGIN_TIMEOUT
    \endlist

    \i
    \e none

    \i
    \e none

    \endtable

    Example of usage:
    \code
    ...
    // MySQL connection
    db->setConnectOptions( "CLIENT_SSL;CLIENT_IGNORE_SPACE" ); // use an SSL connection to the server
    if ( !db->open() ) {
        db->setConnectOptions(); // clears the connect option string
	...
    }
    ...
    // PostgreSQL connection
    db->setConnectOptions( "requiressl=1" ); // enable PostgreSQL SSL connections
    if ( !db->open() ) {
        db->setConnectOptions(); // clear options
	...
    }
    ...
    // ODBC connection
    db->setConnectOptions( "SQL_ATTR_ACCESS_MODE=SQL_MODE_READ_ONLY;SQL_ATTR_TRACE=SQL_OPT_TRACE_ON" ); // set ODBC options
    if ( !db->open() ) {
        db->setConnectOptions(); // don't try to set this option
	...
    }
    \endcode

    Please refer to the client library documentation for more
    information about the different options. The options will be set
    prior to opening the database connection. Setting new options
    without re-opening the connection does nothing.

    \sa connectOptions()
*/

void TQSqlDatabase::setConnectOptions( const TQString& options )
{
    d->connOptions = options;
}

TQString TQSqlDatabase::connectOptions() const
{
    return d->connOptions;
}

/*!
    Returns true if a driver called \a name is available; otherwise
    returns false.

    \sa drivers()
*/

bool TQSqlDatabase::isDriverAvailable( const TQString& name )
{
    TQStringList l = drivers();
    TQStringList::ConstIterator it = l.begin();
    for ( ;it != l.end(); ++it ) {
	if ( *it == name )
	    return true;
    }
    return false;
}

/*! \overload

    This function is useful if you need to set up the database
    connection and instantiate the driver yourself. If you do this, it
    is recommended that you include the driver code in your own
    application. For example, setting up a custom PostgreSQL
    connection and instantiating the TQPSQL7 driver can be done the
    following way:

    \code
    #include "qtdir/src/sql/drivers/psql/qsql_psql.cpp"
    \endcode
    (We assume that \c qtdir is the directory where TQt is installed.)
    This will pull in the code that is needed to use the PostgreSQL
    client library and to instantiate a TQPSQLDriver object, assuming
    that you have the PostgreSQL headers somewhere in your include
    search path.

    \code
    PGconn* con = PQconnectdb( "host=server user=bart password=simpson dbname=springfield" );
    TQPSQLDriver* drv =  new TQPSQLDriver( con );
    TQSqlDatabase* db = TQSqlDatabase::addDatabase( drv ); // becomes the new default connection
    TQSqlQuery q;
    q.exec( "SELECT * FROM people" );
    ...
    \endcode

    The above code sets up a PostgreSQL connection and instantiates a
    TQPSQLDriver object. Next, addDatabase() is called to add the
    connection to the known connections so that it can be used by the
    TQt SQL classes. When a driver is instantiated with a connection
    handle (or set of handles), TQt assumes that you have already
    opened the database connection.

    Remember that you must link your application against the database
    client library as well. The simplest way to do this is to add
    lines like those below to your \c .pro file:

    \code
    unix:LIBS += -lpq
    win32:LIBS += libpqdll.lib
    \endcode

    You will need to have the client library in your linker's search
    path.

    The method described above will work for all the drivers, the only
    difference is the arguments the driver constructors take. Below is
    an overview of the drivers and their constructor arguments.

    \table
    \header \i Driver \i Class name \i Constructor arguments \i File to include
    \row
    \i TQPSQL7
    \i TQPSQLDriver
    \i PGconn* connection
    \i \c qsql_psql.cpp
    \row
    \i TQMYSQL3
    \i TQMYSQLDriver
    \i MYSQL* connection
    \i \c qsql_mysql.cpp
    \row
    \i TQOCI8
    \i TQOCIDriver
    \i OCIEnv* environment, OCIError* error, OCISvcCtx* serviceContext
    \i \c qsql_oci.cpp
    \row
    \i TQODBC3
    \i TQODBCDriver
    \i SQLHANDLE environment, SQLHANDLE connection
    \i \c qsql_odbc.cpp
    \row
    \i TQDB2
    \i TQDB2
    \i SQLHANDLE environment, SQLHANDLE connection
    \i \c qsql_db2.cpp
    \row
    \i TQTDS7
    \i TQTDSDriver
    \i LOGINREC* loginRecord, DBPROCESS* dbProcess, const TQString& hostName
    \i \c qsql_tds.cpp
    \row
    \i TQSQLITE
    \i TQSQLiteDriver
    \i sqlite* connection
    \i \c qsql_sqlite.cpp
    \row
    \i TQIBASE
    \i TQIBaseDriver
    \i isc_db_handle connection
    \i \c qsql_ibase.cpp
    \endtable

    Note: The host name (or service name) is needed when constructing
    the TQTDSDriver for creating new connections for internal
    queries. This is to prevent the simultaneous usage of several
    TQSqlQuery/\l{TQSqlCursor} objects from blocking each other.

    \warning The SQL framework takes ownership of the \a driver pointer,
    and it should not be deleted. The returned TQSqlDatabase object is
    owned by the framework and must not be deleted. If you want to
    explicitly remove the connection, use removeDatabase()

    \sa drivers()
*/

TQSqlDatabase* TQSqlDatabase::addDatabase( TQSqlDriver* driver, const TQString& connectionName )
{
    return TQSqlDatabaseManager::addDatabase( new TQSqlDatabase( driver ), connectionName );
}
#endif // TQT_NO_SQL
