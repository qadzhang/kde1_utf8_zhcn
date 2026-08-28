/****************************************************************************
**
** Definition of Interbase driver classes
**
** Created : 030911
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

#ifndef TQSQL_IBASE_H
#define TQSQL_IBASE_H

#include "ntqsqlresult.h"
#include "ntqsqldriver.h"
#include "../cache/qsqlcachedresult.h"


class TQIBaseDriverPrivate;
class TQIBaseResultPrivate;
class TQIBaseDriver;

class TQIBaseResult : public TQtSqlCachedResult
{
    friend class TQIBaseDriver;
    friend class TQIBaseResultPrivate;

public:
    TQIBaseResult(const TQIBaseDriver* db);
    virtual ~TQIBaseResult();

    bool prepare(const TQString& query);
    bool exec();    

protected:
    bool gotoNext(TQtSqlCachedResult::RowCache* row);
    bool reset (const TQString& query);
    int size();
    int numRowsAffected();

private:
    TQIBaseResultPrivate* d;
};

class TQIBaseDriver : public TQSqlDriver
{
    friend class TQIBaseDriverPrivate;
    friend class TQIBaseResultPrivate;
    friend class TQIBaseResult;
public:
    TQIBaseDriver(TQObject *parent = 0, const char *name = 0);
    TQIBaseDriver(void *connection, TQObject *parent = 0, const char *name = 0);
    virtual ~TQIBaseDriver();
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
    TQStringList tables(const TQString& typeName) const;

    TQSqlRecord record(const TQString& tablename) const;
    TQSqlRecordInfo recordInfo(const TQString& tablename) const;
    TQSqlIndex primaryIndex(const TQString &table) const;
    TQSqlRecord record(const TQSqlQuery& query) const;
    TQSqlRecordInfo recordInfo(const TQSqlQuery& query) const;

    TQString formatValue(const TQSqlField* field, bool trimStrings) const;
    
private:
    TQIBaseDriverPrivate* d;
};


#endif

