/****************************************************************************
**
** Definition of SQLite driver classes.
**
** Copyright (C) 1992-2003 Trolltech AS. All rights reserved.
**
** This file is part of the sql module of the TQt GUI Toolkit.
** EDITIONS: FREE, ENTERPRISE
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef TQSQL_SQLITE3_H
#define TQSQL_SQLITE3_H

#include <ntqsqldriver.h>
#include <ntqsqlresult.h>
#include <ntqsqlrecord.h>
#include <ntqsqlindex.h>
#include "qsqlcachedresult.h"

#if (TQT_VERSION-0 >= 0x030200)
typedef TQVariant TQSqlVariant;
#endif

#if defined (Q_OS_WIN32)
# include <qt_windows.h>
#endif

class TQSQLite3DriverPrivate;
class TQSQLite3ResultPrivate;
class TQSQLite3Driver;
struct sqlite3;

class TQSQLite3Result : public TQtSqlCachedResult
{
    friend class TQSQLite3Driver;
    friend class TQSQLite3ResultPrivate;
public:
    TQSQLite3Result(const TQSQLite3Driver* db);
    ~TQSQLite3Result();

protected:
    bool gotoNext(TQtSqlCachedResult::RowCache* row);
    bool reset (const TQString& query);
    int size();
    int numRowsAffected();

private:
    TQSQLite3ResultPrivate* d;
};

class TQSQLite3Driver : public TQSqlDriver
{
    friend class TQSQLite3Result;
public:
    TQSQLite3Driver(TQObject *parent = 0, const char *name = 0);
    TQSQLite3Driver(sqlite3 *connection, TQObject *parent = 0, const char *name = 0);
    ~TQSQLite3Driver();
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
    TQStringList tables(const TQString &user) const;

    TQSqlRecord record(const TQString& tablename) const;
    TQSqlRecordInfo recordInfo(const TQString& tablename) const;
    TQSqlIndex primaryIndex(const TQString &table) const;
    TQSqlRecord record(const TQSqlQuery& query) const;
    TQSqlRecordInfo recordInfo(const TQSqlQuery& query) const;

private:
    TQSQLite3DriverPrivate* d;
};
#endif
