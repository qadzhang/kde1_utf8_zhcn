/****************************************************************************
**
** Implementation of SQLite driver classes.
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

#include "qsql_sqlite.h"

#include <ntqdatetime.h>
#include <ntqregexp.h>
#include <ntqfile.h>
#include <ntqptrvector.h>
#if !defined TQ_WS_WIN32
#  include <unistd.h>
#endif
#include <sqlite.h>

typedef struct sqlite_vm sqlite_vm;

#define TQSQLITE_DRIVER_NAME "TQSQLITE"

static TQSqlVariant::Type nameToType(const TQString& typeName)
{
    TQString tName = typeName.upper();
    if (tName.startsWith("INT"))
        return TQSqlVariant::Int;
    if (tName.startsWith("FLOAT") || tName.startsWith("NUMERIC"))
        return TQSqlVariant::Double;
    if (tName.startsWith("BOOL"))
        return TQSqlVariant::Bool;
    // SQLite is typeless - consider everything else as string
    return TQSqlVariant::String;
}

class TQSQLiteDriverPrivate
{
public:
    TQSQLiteDriverPrivate();
    sqlite *access;
    bool utf8;
};

TQSQLiteDriverPrivate::TQSQLiteDriverPrivate() : access(0)
{
    utf8 = (qstrcmp(sqlite_encoding, "UTF-8") == 0);
}

class TQSQLiteResultPrivate
{
public:
    TQSQLiteResultPrivate(TQSQLiteResult *res);
    void cleanup();
    bool fetchNext(TQtSqlCachedResult::RowCache *row);
    bool isSelect();
    // initializes the recordInfo and the cache
    void init(const char **cnames, int numCols, TQtSqlCachedResult::RowCache **row = 0);
    void finalize();

    TQSQLiteResult* q;
    sqlite *access;

    // and we have too keep our own struct for the data (sqlite works via
    // callback.
    const char *currentTail;
    sqlite_vm *currentMachine;

    uint skippedStatus: 1; // the status of the fetchNext() that's skipped
    TQtSqlCachedResult::RowCache *skipRow;

    uint utf8: 1;
    TQSqlRecordInfo rInf;
};

static const uint initial_cache_size = 128;

TQSQLiteResultPrivate::TQSQLiteResultPrivate(TQSQLiteResult* res) : q(res), access(0), currentTail(0),
    currentMachine(0), skippedStatus(false), skipRow(0), utf8(false)
{
}

void TQSQLiteResultPrivate::cleanup()
{
    finalize();
    rInf.clear();
    currentTail = 0;
    currentMachine = 0;
    skippedStatus = false;
    delete skipRow;
    skipRow = 0;
    q->setAt(TQSql::BeforeFirst);
    q->setActive(false);
    q->cleanup();
}

void TQSQLiteResultPrivate::finalize()
{
    if (!currentMachine)
        return;

    char* err = 0;
    int res = sqlite_finalize(currentMachine, &err);
    if (err) {
        q->setLastError(TQSqlError("Unable to fetch results", err, TQSqlError::Statement, res));
        sqlite_freemem(err);
    }
    currentMachine = 0;
}

// called on first fetch
void TQSQLiteResultPrivate::init(const char **cnames, int numCols, TQtSqlCachedResult::RowCache **row)
{
    if (!cnames)
        return;

    rInf.clear();
    if (numCols <= 0)
        return;

    for (int i = 0; i < numCols; ++i) {
        const char* lastDot = strrchr(cnames[i], '.');
        const char* fieldName = lastDot ? lastDot + 1 : cnames[i];
        rInf.append(TQSqlFieldInfo(fieldName, nameToType(cnames[i+numCols])));
    }
    // skip the first fetch
    if (row && !*row) {
	*row = new TQtSqlCachedResult::RowCache(numCols);
	skipRow = *row;
    }
}

bool TQSQLiteResultPrivate::fetchNext(TQtSqlCachedResult::RowCache* row)
{
    // may be caching.
    const char **fvals;
    const char **cnames;
    int colNum;
    int res;
    int i;

    if (skipRow) {
	// already fetched
	if (row)
	    *row = *skipRow;
	delete skipRow;
	skipRow = 0;
	return skippedStatus;
    }

    if (!currentMachine)
	return false;

    // keep trying while busy, wish I could implement this better.
    while ((res = sqlite_step(currentMachine, &colNum, &fvals, &cnames)) == SQLITE_BUSY) {
	// sleep instead requesting result again immidiately.
#if defined TQ_WS_WIN32
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
	    init(cnames, colNum, &row);
	if (!fvals)
	    return false;
	if (!row)
	    return true;
	for (i = 0; i < colNum; ++i)
	    (*row)[i] = utf8 ? TQString::fromUtf8(fvals[i]) : TQString(fvals[i]);
	return true;
    case SQLITE_DONE:
	if (rInf.isEmpty())
	    // must be first call.
	    init(cnames, colNum);
	q->setAt(TQSql::AfterLast);
	return false;
    case SQLITE_ERROR:
    case SQLITE_MISUSE:
    default:
	// something wrong, don't get col info, but still return false
	finalize(); // finalize to get the error message.
	q->setAt(TQSql::AfterLast);
	return false;
    }
    return false;
}

TQSQLiteResult::TQSQLiteResult(const TQSQLiteDriver* db)
: TQtSqlCachedResult(db)
{
    d = new TQSQLiteResultPrivate(this);
    d->access = db->d->access;
    d->utf8 = db->d->utf8;
}

TQSQLiteResult::~TQSQLiteResult()
{
    d->cleanup();
    delete d;
}

/*
   Execute \a query.
*/
bool TQSQLiteResult::reset (const TQString& query)
{
    // this is where we build a query.
    if (!driver())
        return false;
    if (!driver()-> isOpen() || driver()->isOpenError())
        return false;

    d->cleanup();

    // Um, ok.  callback based so.... pass private static function for this.
    setSelect(false);
    char *err = 0;
    int res = sqlite_compile(d->access,
                                d->utf8 ? (const char*)query.utf8().data() : query.ascii(),
                                &(d->currentTail),
                                &(d->currentMachine),
                                &err);
    if (res != SQLITE_OK || err) {
        setLastError(TQSqlError("Unable to execute statement", err, TQSqlError::Statement, res));
        sqlite_freemem(err);
    }
    //if (*d->currentTail != '\000' then there is more sql to eval
    if (!d->currentMachine) {
	setActive(false);
	return false;
    }
    // we have to fetch one row to find out about
    // the structure of the result set
    d->skippedStatus = d->fetchNext(0);
    setSelect(!d->rInf.isEmpty());
    if (isSelect())
	init(d->rInf.count());
    setActive(true);
    return true;
}

bool TQSQLiteResult::gotoNext(TQtSqlCachedResult::RowCache* row)
{
    return d->fetchNext(row);
}

int TQSQLiteResult::size()
{
    return -1;
}

int TQSQLiteResult::numRowsAffected()
{
    return sqlite_changes(d->access);
}

/////////////////////////////////////////////////////////

TQSQLiteDriver::TQSQLiteDriver(TQObject * parent, const char * name)
    : TQSqlDriver(parent, name ? name : TQSQLITE_DRIVER_NAME)
{
    d = new TQSQLiteDriverPrivate();
}

TQSQLiteDriver::TQSQLiteDriver(sqlite *connection, TQObject *parent, const char *name)
    : TQSqlDriver(parent, name ? name : TQSQLITE_DRIVER_NAME)
{
    d = new TQSQLiteDriverPrivate();
    d->access = connection;
    setOpen(true);
    setOpenError(false);
}


TQSQLiteDriver::~TQSQLiteDriver()
{
    delete d;
}

bool TQSQLiteDriver::hasFeature(DriverFeature f) const
{
    switch (f) {
    case Transactions:
        return true;
#if (TQT_VERSION-0 >= 0x030000)
    case Unicode:
        return d->utf8;
#endif
//   case BLOB:
    default:
        return false;
    }
}

/*
   SQLite dbs have no user name, passwords, hosts or ports.
   just file names.
*/
bool TQSQLiteDriver::open(const TQString & db, const TQString &, const TQString &, const TQString &, int, const TQString &)
{
    if (isOpen())
        close();

    if (db.isEmpty())
	return false;

    char* err = 0;
    d->access = sqlite_open(TQFile::encodeName(db), 0, &err);
    if (err) {
        setLastError(TQSqlError("Error to open database", err, TQSqlError::Connection));
        sqlite_freemem(err);
        err = 0;
    }

    if (d->access) {
        setOpen(true);
	setOpenError(false);
        return true;
    }
    setOpenError(true);
    return false;
}

void TQSQLiteDriver::close()
{
    if (isOpen()) {
        sqlite_close(d->access);
        d->access = 0;
        setOpen(false);
        setOpenError(false);
    }
}

bool TQSQLiteDriver::ping()
{
    if ( !isOpen() ) {
        return false;
    }

    // FIXME
    // Implement ping if available
    return true;
}

TQSqlQuery TQSQLiteDriver::createQuery() const
{
    return TQSqlQuery(new TQSQLiteResult(this));
}

bool TQSQLiteDriver::beginTransaction()
{
    if (!isOpen() || isOpenError())
        return false;

    char* err;
    int res = sqlite_exec(d->access, "BEGIN", 0, this, &err);

    if (res == SQLITE_OK)
        return true;

    setLastError(TQSqlError("Unable to begin transaction", err, TQSqlError::Transaction, res));
    sqlite_freemem(err);
    return false;
}

bool TQSQLiteDriver::commitTransaction()
{
    if (!isOpen() || isOpenError())
        return false;

    char* err;
    int res = sqlite_exec(d->access, "COMMIT", 0, this, &err);

    if (res == SQLITE_OK)
        return true;

    setLastError(TQSqlError("Unable to commit transaction", err, TQSqlError::Transaction, res));
    sqlite_freemem(err);
    return false;
}

bool TQSQLiteDriver::rollbackTransaction()
{
    if (!isOpen() || isOpenError())
        return false;

    char* err;
    int res = sqlite_exec(d->access, "ROLLBACK", 0, this, &err);

    if (res == SQLITE_OK)
        return true;

    setLastError(TQSqlError("Unable to rollback Transaction", err, TQSqlError::Transaction, res));
    sqlite_freemem(err);
    return false;
}

TQStringList TQSQLiteDriver::tables(const TQString &typeName) const
{
    TQStringList res;
    if (!isOpen())
        return res;
    int type = typeName.toInt();

    TQSqlQuery q = createQuery();
    q.setForwardOnly(true);
#if (TQT_VERSION-0 >= 0x030000)
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

#if (TQT_VERSION-0 >= 0x030000)
    if (type & (int)TQSql::SystemTables) {
        // there are no internal tables beside this one:
        res.append("sqlite_master");
    }
#endif

    return res;
}

TQSqlIndex TQSQLiteDriver::primaryIndex(const TQString &tblname) const
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

    TQSqlIndex index(tblname, indexname);
    while(q.next()) {
	TQString name = q.value(2).toString();
	TQSqlVariant::Type type = TQSqlVariant::Invalid;
	if (rec.contains(name))
	    type = rec.find(name).type();
	index.append(TQSqlField(name, type));
    }
    return index;
}

TQSqlRecordInfo TQSQLiteDriver::recordInfo(const TQString &tbl) const
{
    if (!isOpen())
        return TQSqlRecordInfo();

    TQSqlQuery q = createQuery();
    q.setForwardOnly(true);
    q.exec("SELECT * FROM " + tbl + " LIMIT 1");
    return recordInfo(q);
}

TQSqlRecord TQSQLiteDriver::record(const TQString &tblname) const
{
    if (!isOpen())
        return TQSqlRecord();

    return recordInfo(tblname).toRecord();
}

TQSqlRecord TQSQLiteDriver::record(const TQSqlQuery& query) const
{
    if (query.isActive() && query.driver() == this) {
        TQSQLiteResult* result = (TQSQLiteResult*)query.result();
        return result->d->rInf.toRecord();
    }
    return TQSqlRecord();
}

TQSqlRecordInfo TQSQLiteDriver::recordInfo(const TQSqlQuery& query) const
{
    if (query.isActive() && query.driver() == this) {
        TQSQLiteResult* result = (TQSQLiteResult*)query.result();
        return result->d->rInf;
    }
    return TQSqlRecordInfo();
}
