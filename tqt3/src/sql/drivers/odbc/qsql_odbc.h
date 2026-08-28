/****************************************************************************
**
** Definition of ODBC driver classes
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

#ifndef TQSQL_ODBC_H
#define TQSQL_ODBC_H

#include <ntqmap.h>
#include <ntqstring.h>
#include <ntqsqldriver.h>
#include <ntqsqlfield.h>
#include <ntqsqlresult.h>
#include <ntqsqlindex.h>

#if defined (Q_OS_WIN32)
#include <qt_windows.h>
#endif

#if defined (Q_OS_MAC)
// assume we use iodbc on MAC
// comment next line out if you use a 
// unicode compatible manager
# define Q_ODBC_VERSION_2
#endif

#ifdef QT_PLUGIN
#define TQ_EXPORT_SQLDRIVER_ODBC
#else
#define TQ_EXPORT_SQLDRIVER_ODBC TQ_EXPORT
#endif

#ifdef Q_OS_UNIX
#define HAVE_LONG_LONG 1 // force UnixODBC NOT to fall back to a struct for BIGINTs
#endif

#include <sql.h>
#include <sqlext.h>
#include "debian_qsql_odbc.h"

class TQODBCPrivate;
class TQODBCDriver;
class TQSqlRecordInfo;

class TQODBCResult : public TQSqlResult
{
    friend class TQODBCDriver;
public:
    TQODBCResult( const TQODBCDriver * db, TQODBCPrivate* p );
    ~TQODBCResult();

    SQLHANDLE   statement();
    bool	prepare( const TQString& query );
    bool	exec();

protected:
    bool	fetchNext();
    bool	fetchFirst();
    bool	fetchLast();
    bool	fetchPrior();
    bool	fetch(int i);
    bool	reset ( const TQString& query );
    TQVariant	data( int field );
    bool	isNull( int field );
    int         size();
    int         numRowsAffected();
private:
    TQODBCPrivate*	d;
    typedef TQMap<int,TQVariant> FieldCache;
    FieldCache fieldCache;
    typedef TQMap<int,bool> NullCache;
    NullCache nullCache;
};

class TQ_EXPORT_SQLDRIVER_ODBC TQODBCDriver : public TQSqlDriver
{
public:
    TQODBCDriver( TQObject * parent=0, const char * name=0 );
    TQODBCDriver( SQLHANDLE env, SQLHANDLE con, TQObject * parent=0, const char * name=0 );
    ~TQODBCDriver();
    bool		hasFeature( DriverFeature f ) const;
    bool		open( const TQString & db,
			      const TQString & user = TQString::null,
			      const TQString & password = TQString::null,
			      const TQString & host = TQString::null,
			      int port = -1 );
    void		close();
    bool		ping();
    TQSqlQuery		createQuery() const;
    TQStringList		tables( const TQString& user ) const;
    TQSqlRecord		record( const TQString& tablename ) const;
    TQSqlRecord		record( const TQSqlQuery& query ) const;
    TQSqlRecordInfo	recordInfo( const TQString& tablename ) const;
    TQSqlRecordInfo	recordInfo( const TQSqlQuery& query ) const;
    TQSqlIndex		primaryIndex( const TQString& tablename ) const;
    SQLHANDLE		environment();
    SQLHANDLE		connection();

    TQString		formatValue( const TQSqlField* field,
				     bool trimStrings ) const;
    // ### remove me for 4.0
    bool open( const TQString& db,
	       const TQString& user,
	       const TQString& password,
	       const TQString& host,
	       int port,
	       const TQString& connOpts );
    
protected:
    bool		beginTransaction();
    bool		commitTransaction();
    bool		rollbackTransaction();
private:
    void init();
    bool endTrans();
    void cleanup();
    TQODBCPrivate* d;
};

#endif
