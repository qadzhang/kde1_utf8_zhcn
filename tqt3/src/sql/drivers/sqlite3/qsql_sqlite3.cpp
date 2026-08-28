/****************************************************************************
**
** Implementation of SQLite driver classes.
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

#include "qsql_sqlite3.h"

#include <ntqdatetime.h>
#include <ntqvaluevector.h>
#include <ntqregexp.h>
#include <ntqfile.h>
#include <sqlite3.h>
#include <ntqptrvector.h>
#if !defined TQ_WS_WIN32
#  include <unistd.h>
#endif

typedef struct sqlite3_stmt sqlite3_stmt;

#define TQSQLITE3_DRIVER_NAME "TQSQLITE3"

static TQVariant::Type qSqliteType(int tp)
{
    switch (tp) {
    case SQLITE_INTEGER:
        return TQVariant::Int;
    case SQLITE_FLOAT:
        return TQVariant::Double;
    case SQLITE_BLOB:
        return TQVariant::ByteArray;
    case SQLITE_TEXT:
    default:
        return TQVariant::String;
    }
}

static TQSqlError qMakeError(sqlite3 *access, const TQString &descr, TQSqlError::Type type,
                            int errorCode = -1)
{
    return TQSqlError(descr,
                     TQString::fromUtf8(sqlite3_errmsg(access)),
                     type, errorCode);
}

class TQSQLite3DriverPrivate
{
public:
    TQSQLite3DriverPrivate();
    sqlite3 *access;
    bool utf8;
};

TQSQLite3DriverPrivate::TQSQLite3DriverPrivate() : access(0)
{
    utf8 = true;
}

class TQSQLite3ResultPrivate
{
public:
    TQSQLite3ResultPrivate(TQSQLite3Result *res);
    void cleanup();
    bool fetchNext(TQtSqlCachedResult::RowCache *row);
    bool isSelect();
    // initializes the recordInfo and the cache
    void initColumns(TQtSqlCachedResult::RowCache **row = 0);
    void finalize();

    TQSQLite3Result* q;
    sqlite3 *access;

    sqlite3_stmt *stmt;

    uint skippedStatus: 1; // the status of the fetchNext() that's skipped
    TQtSqlCachedResult::RowCache *skipRow;

    uint utf8: 1;
    TQSqlRecord rInf;
};

static const uint initial_cache_size = 128;

TQSQLite3ResultPrivate::TQSQLite3ResultPrivate(TQSQLite3Result* res) : q(res), access(0),
    stmt(0), skippedStatus(false), skipRow(0), utf8(false)
{
}

void TQSQLite3ResultPrivate::cleanup()
{
    finalize();
    rInf.clear();
    skippedStatus = false;
    delete skipRow;
    skipRow = 0;
    q->setAt(TQSql::BeforeFirst);
    q->setActive(false);
    q->cleanup();
}

void TQSQLite3ResultPrivate::finalize()
{
    if (!stmt)
        return;

    sqlite3_finalize(stmt);
    stmt = 0;
}

// called on first fetch
void TQSQLite3ResultPrivate::initColumns(TQtSqlCachedResult::RowCache** row)
{
    rInf.clear();

    int nCols = sqlite3_column_count(stmt);
    if (nCols <= 0)
        return;

    q->init(nCols);

    for (int i = 0; i < nCols; ++i) {
        TQString colName = TQString::fromUtf8(sqlite3_column_name(stmt, i));

        int dotIdx = colName.findRev('.');
        rInf.append(TQSqlField(colName.mid(dotIdx == -1 ? 0 : dotIdx + 1),
                qSqliteType(sqlite3_column_type(stmt, i))));
    }

    // skip the first fetch
    if (row && !*row) {
        *row = new TQtSqlCachedResult::RowCache(nCols);
        skipRow = *row;
    }
}

bool TQSQLite3ResultPrivate::fetchNext(TQtSqlCachedResult::RowCache* row)
{
    int res;
    unsigned int i;

    if (skipRow) {
	// already fetched
	if (row)
	    *row = *skipRow;
	delete skipRow;
	skipRow = 0;
	return skippedStatus;
    }

    if (!stmt)
        return false;

    // keep trying while busy, wish I could implement this better.
    while ((res = sqlite3_step(stmt)) == SQLITE_BUSY) {
        // sleep instead requesting result again immidiately.
#if defined Q_OS_WIN
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    switch(res) {
    case SQLITE_ROW:
        // check to see if should fill out columns
        if (rInf.isEmpty())
            // must be first call.
            initColumns(&row);
        if (!row)
            return true;
        for (i = 0; i < rInf.count(); ++i)
            // todo - handle other types
            (*row)[i] = TQString::fromUtf8((char *)(sqlite3_column_text(stmt, i)));
 //           values[i + idx] = utf8 ? TQString::fromUtf8(fvals[i]) : TQString::fromAscii(fvals[i]);
        return true;
    case SQLITE_DONE:
        if (rInf.isEmpty())
            // must be first call.
            initColumns(&row);
        q->setAt(TQSql::AfterLast);
        return false;
    case SQLITE_ERROR:
    case SQLITE_MISUSE:
    default:
        // something wrong, don't get col info, but still return false
        q->setLastError(qMakeError(access, "Unable to fetch row", TQSqlError::Connection, res));
        finalize();
        q->setAt(TQSql::AfterLast);
        return false;
    }
    return false;
}

TQSQLite3Result::TQSQLite3Result(const TQSQLite3Driver* db)
    : TQtSqlCachedResult(db)
{
    d = new TQSQLite3ResultPrivate(this);
    d->access = db->d->access;
}

TQSQLite3Result::~TQSQLite3Result()
{
    d->cleanup();
    delete d;
}

/*
   Execute \a query.
*/
bool TQSQLite3Result::reset (const TQString &query)
{
    // this is where we build a query.
    if (!driver() || !driver()->isOpen() || driver()->isOpenError())
        return false;

    d->cleanup();

    setSelect(false);

    int res = sqlite3_prepare(d->access, query.utf8().data(), (query.length() + 1) * sizeof(TQChar),
                                &d->stmt, 0);

    if (res != SQLITE_OK) {
        setLastError(qMakeError(d->access, "Unable to execute statement", TQSqlError::Statement, res));
        d->finalize();
        return false;
    }

    d->skippedStatus = d->fetchNext(0);

    setSelect(!d->rInf.isEmpty());
    setActive(true);
    return true;
}

bool TQSQLite3Result::gotoNext(TQtSqlCachedResult::RowCache* row)
{
    return d->fetchNext(row);
}

int TQSQLite3Result::size()
{
    return -1;
}

int TQSQLite3Result::numRowsAffected()
{
    return sqlite3_changes(d->access);
}

/////////////////////////////////////////////////////////

TQSQLite3Driver::TQSQLite3Driver(TQObject * parent, const char *name)
    : TQSqlDriver(parent, name)
{
    d = new TQSQLite3DriverPrivate();
}

TQSQLite3Driver::TQSQLite3Driver(sqlite3 *connection, TQObject *parent, const char *name)
    : TQSqlDriver(parent, name)
{
    d = new TQSQLite3DriverPrivate();
    d->access = connection;
    setOpen(true);
    setOpenError(false);
}


TQSQLite3Driver::~TQSQLite3Driver()
{
    delete d;
}

bool TQSQLite3Driver::hasFeature(DriverFeature f) const
{
    switch (f) {
    case Transactions:
    case Unicode:
    case BLOB:
        return true;
    default:
        break;
    }
    return false;
}

/*
   SQLite dbs have no user name, passwords, hosts or ports.
   just file names.
*/
bool TQSQLite3Driver::open(const TQString & db, const TQString &, const TQString &, const TQString &, int, const TQString &)
{
    if (isOpen())
        close();

    if (db.isEmpty())
        return false;

    if (sqlite3_open(TQFile::encodeName(db), &d->access) == SQLITE_OK) {
        setOpen(true);
        setOpenError(false);
        return true;
    } else {
        setLastError(qMakeError(d->access, "Error opening database",
                     TQSqlError::Connection));
        setOpenError(true);
        return false;
    }
}

void TQSQLite3Driver::close()
{
    if (isOpen()) {
        if (sqlite3_close(d->access) != SQLITE_OK)
            setLastError(qMakeError(d->access, "Error closing database",
                                    TQSqlError::Connection));
        d->access = 0;
        setOpen(false);
        setOpenError(false);
    }
}

bool TQSQLite3Driver::ping()
{
    if ( !isOpen() ) {
        return false;
    }

    // FIXME
    // Implement ping if available
    return true;
}

TQSqlQuery TQSQLite3Driver::createQuery() const
{
    return TQSqlQuery(new TQSQLite3Result(this));
}

bool TQSQLite3Driver::beginTransaction()
{
    if (!isOpen() || isOpenError())
        return false;

    TQSqlQuery q(createQuery());
    if (!q.exec("BEGIN")) {
        setLastError(TQSqlError("Unable to begin transaction",
                               q.lastError().databaseText(), TQSqlError::Transaction));
        return false;
    }

    return true;
}

bool TQSQLite3Driver::commitTransaction()
{
    if (!isOpen() || isOpenError())
        return false;

    TQSqlQuery q(createQuery());
    if (!q.exec("COMMIT")) {
        setLastError(TQSqlError("Unable to begin transaction",
                               q.lastError().databaseText(), TQSqlError::Transaction));
        return false;
    }

    return true;
}

bool TQSQLite3Driver::rollbackTransaction()
{
    if (!isOpen() || isOpenError())
        return false;

    TQSqlQuery q(createQuery());
    if (!q.exec("ROLLBACK")) {
        setLastError(TQSqlError("Unable to begin transaction",
                               q.lastError().databaseText(), TQSqlError::Transaction));
        return false;
    }

    return true;
}

TQStringList TQSQLite3Driver::tables(const TQString &typeName) const
{
    TQStringList res;
    if (!isOpen())
        return res;
    int type = typeName.toInt();

    TQSqlQuery q = createQuery();
    q.setForwardOnly(true);
#if (TQT_VERSION-0 >= 0x030200)
    if ((type & (int)TQSql::Tables) && (type & (int)TQSql::Views))
        q.exec("SELECT name FROM sqlite_master WHERE type='table' OR type='view'");
    else if (typeName.isEmpty() || (type & (int)TQSql::Tables))
        q.exec("SELECT name FROM sqlite_master WHERE type='table'");
    else if (type & (int)TQSql::Views)
        q.exec("SELECT name FROM sqlite_master WHERE type='view'");
#else
        q.exec("SELECT name FROM sqlite_master WHERE type='table' OR type='view'");
#endif


    if (q.isActive()) {
        while(q.next())
            res.append(q.value(0).toString());
    }

#if (TQT_VERSION-0 >= 0x030200)
    if (type & (int)TQSql::SystemTables) {
        // there are no internal tables beside this one:
        res.append("sqlite_master");
    }
#endif

    return res;
}

TQSqlIndex TQSQLite3Driver::primaryIndex(const TQString &tblname) const
{
    TQSqlRecordInfo rec(recordInfo(tblname)); // expensive :(

    if (!isOpen())
        return TQSqlIndex();

    TQSqlQuery q = createQuery();
    q.setForwardOnly(true);
    // finrst find a UNIQUE INDEX
    q.exec("PRAGMA index_list('" + tblname + "');");
    TQString indexname;
    while(q.next()) {
	if (q.value(2).toInt()==1) {
	    indexname = q.value(1).toString();
	    break;
	}
    }
    if (indexname.isEmpty())
	return TQSqlIndex();

    q.exec("PRAGMA index_info('" + indexname + "');");

    TQSqlIndex index(indexname);
    while(q.next()) {
	TQString name = q.value(2).toString();
	TQSqlVariant::Type type = TQSqlVariant::Invalid;
	if (rec.contains(name))
	    type = rec.find(name).type();
	index.append(TQSqlField(name, type));
    }
    return index;
}

TQSqlRecordInfo TQSQLite3Driver::recordInfo(const TQString &tbl) const
{
    if (!isOpen())
        return TQSqlRecordInfo();

    TQSqlQuery q = createQuery();
    q.setForwardOnly(true);
    q.exec("SELECT * FROM " + tbl + " LIMIT 1");
    return recordInfo(q);
}

TQSqlRecord TQSQLite3Driver::record(const TQString &tblname) const
{
    if (!isOpen())
        return TQSqlRecord();

    return recordInfo(tblname).toRecord();
}

TQSqlRecord TQSQLite3Driver::record(const TQSqlQuery& query) const
{
    if (query.isActive() && query.driver() == this) {
        TQSQLite3Result* result = (TQSQLite3Result*)query.result();
        return result->d->rInf;
    }
    return TQSqlRecord();
}

TQSqlRecordInfo TQSQLite3Driver::recordInfo(const TQSqlQuery& query) const
{
    if (query.isActive() && query.driver() == this) {
        TQSQLite3Result* result = (TQSQLite3Result*)query.result();
        return TQSqlRecordInfo(result->d->rInf);
    }
    return TQSqlRecordInfo();
}
