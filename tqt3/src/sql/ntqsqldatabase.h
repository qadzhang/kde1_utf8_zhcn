/****************************************************************************
**
** Definition of TQSqlDatabase class
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

#ifndef TQSQLDATABASE_H
#define TQSQLDATABASE_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqstring.h"
#include "ntqsqlquery.h"
#include "ntqstringlist.h"
#endif // QT_H

#if !defined( TQT_MODULE_SQL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_SQL
#else
#define TQM_EXPORT_SQL TQ_EXPORT
#endif

#ifndef TQT_NO_SQL

class TQSqlError;
class TQSqlDriver;
class TQSqlIndex;
class TQSqlRecord;
class TQSqlRecordInfo;
class TQSqlDatabasePrivate;

class TQM_EXPORT_SQL TQSqlDriverCreatorBase
{
public:
    TQSqlDriverCreatorBase();
    virtual ~TQSqlDriverCreatorBase();
public:
    virtual TQSqlDriver* createObject() = 0;
};

template <class type>
class TQM_EXPORT_SQL TQSqlDriverCreator: public TQSqlDriverCreatorBase
{
public:
    TQSqlDriver* createObject() { return new type; }
};

class TQM_EXPORT_SQL TQSqlDatabase : public TQObject
{
    TQ_OBJECT
    TQ_PROPERTY( TQString databaseName  READ databaseName WRITE setDatabaseName )
    TQ_PROPERTY( TQString userName  READ userName WRITE setUserName )
    TQ_PROPERTY( TQString password  READ password WRITE setPassword )
    TQ_PROPERTY( TQString hostName  READ hostName WRITE setHostName )
    TQ_PROPERTY( int port READ port WRITE setPort )
    TQ_PROPERTY( TQString connectOptions READ connectOptions WRITE setConnectOptions )

public:
    ~TQSqlDatabase();

    bool		open();
    bool		open( const TQString& user, const TQString& password );
    void		close();
    bool		ping();
    bool		isOpen() const;
    bool		isOpenError() const;
    TQStringList		tables() const;
    TQStringList		tables( TQSql::TableType type ) const;
    TQSqlIndex		primaryIndex( const TQString& tablename ) const;
    TQSqlRecord		record( const TQString& tablename ) const;
    TQSqlRecord		record( const TQSqlQuery& query ) const;
    TQSqlRecordInfo	recordInfo( const TQString& tablename ) const;
    TQSqlRecordInfo	recordInfo( const TQSqlQuery& query ) const;
    TQSqlQuery		exec( const TQString& query = TQString::null ) const;
    TQSqlError		lastError() const;

    bool		transaction();
    bool		commit();
    bool		rollback();

    virtual void	setDatabaseName( const TQString& name );
    virtual void	setUserName( const TQString& name );
    virtual void	setPassword( const TQString& password );
    virtual void	setHostName( const TQString& host );
    virtual void	setPort( int p );
    void 		setConnectOptions( const TQString& options = TQString::null );
    TQString		databaseName() const;
    TQString		userName() const;
    TQString		password() const;
    TQString		hostName() const;
    TQString		driverName() const;
    int         	port() const;
    TQString 		connectOptions() const;

    TQSqlDriver*		driver() const;
    
    // MOC_SKIP_BEGIN
    static const char * const defaultConnection;
    // MOC_SKIP_END

    static TQSqlDatabase* addDatabase( const TQString& type, const TQString& connectionName = defaultConnection );
    static TQSqlDatabase* addDatabase( TQSqlDriver* driver, const TQString& connectionName = defaultConnection );
    static TQSqlDatabase* database( const TQString& connectionName = defaultConnection, bool open = true );
    static void          removeDatabase( const TQString& connectionName );
    static void          removeDatabase( TQSqlDatabase* db );
    static bool          contains( const TQString& connectionName = defaultConnection );
    static TQStringList   drivers();
    static void          registerSqlDriver( const TQString& name, const TQSqlDriverCreatorBase* creator ); // ### 4.0: creator should not be const
    static bool 	 isDriverAvailable( const TQString& name );
    
protected:
    TQSqlDatabase( const TQString& type, const TQString& name, TQObject * parent=0, const char * objname=0 );
    TQSqlDatabase( TQSqlDriver* driver, TQObject * parent=0, const char * objname=0 );
private:
    void 	init( const TQString& type, const TQString& name );
    TQSqlDatabasePrivate* d;
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQSqlDatabase( const TQSqlDatabase & );
    TQSqlDatabase &operator=( const TQSqlDatabase & );
#endif

};

#endif // TQT_NO_SQL
#endif
