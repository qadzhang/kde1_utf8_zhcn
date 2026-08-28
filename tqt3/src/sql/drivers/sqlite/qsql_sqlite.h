/****************************************************************************
**
** Definition of SQLite driver classes.
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the sql module of the TQt GUI Toolkit.
** EDITIONS: FREE, ENTERPRISE
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef TQSQL_SQLITE_H
#define TQSQL_SQLITE_H

#include <ntqsqldriver.h>
#include <ntqsqlresult.h>
#include <ntqsqlrecord.h>
#include <ntqsqlindex.h>
#include "../cache/qsqlcachedresult.h"

#if (TQT_VERSION-0 >= 0x030000)
typedef TQVariant TQSqlVariant;
#endif

#if defined (Q_OS_WIN32)
# include <qt_windows.h>
#endif

class TQSQLiteDriverPrivate;
class TQSQLiteResultPrivate;
class TQSQLiteDriver;
struct sqlite;

class TQSQLiteResult : public TQtSqlCachedResult
{
    friend class TQSQLiteDriver;
    friend class TQSQLiteResultPrivate;
public:
    TQSQLiteResult(const TQSQLiteDriver* db);
    ~TQSQLiteResult();

protected:
    bool gotoNext(TQtSqlCachedResult::RowCache* row);    
    bool reset (const TQString& query);
    int size();
    int numRowsAffected();

private:
    TQSQLiteResultPrivate* d;
};

class TQSQLiteDriver : public TQSqlDriver
{
    friend class TQSQLiteResult;
public:
    TQSQLiteDriver(TQObject *parent = 0, const char *name = 0);
    TQSQLiteDriver(sqlite *connection, TQObject *parent = 0, const char *name = 0);
    ~TQSQLiteDriver();
    bool hasFeature(DriverFeature f) const;
    bool open(const TQString & db,
                   const TQString & user,
                   const TQString & password,
                   const TQString & host,
                   int port,
                   const TQString & connOpts);
    bool open( const TQString & db,
	    const TQString & user,
	    const TQString & password,
	    const TQString & host,
	    int port ) { return open (db, user, password, host, port, TQString()); }
    void close();
    bool ping();
    TQSqlQuery createQuery() const;
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    TQStringList tables(const TQString& user) const;

    TQSqlRecord record(const TQString& tablename) const;
    TQSqlRecordInfo recordInfo(const TQString& tablename) const;
    TQSqlIndex primaryIndex(const TQString &table) const;
    TQSqlRecord record(const TQSqlQuery& query) const;
    TQSqlRecordInfo recordInfo(const TQSqlQuery& query) const;

private:
    TQSQLiteDriverPrivate* d;
};
#endif
