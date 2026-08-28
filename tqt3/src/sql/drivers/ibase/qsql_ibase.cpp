/****************************************************************************
**
** Implementation of Interbase driver classes.
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

#include "qsql_ibase.h"

#include <ntqdatetime.h>
#include <private/qsqlextension_p.h>

#include <ibase.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#define TQIBASE_DRIVER_NAME "TQIBASE"

class TQIBasePreparedExtension : public TQSqlExtension
{
public:
    TQIBasePreparedExtension(TQIBaseResult *r)
	: result(r) {}

    bool prepare(const TQString &query)
    {
	return result->prepare(query);
    }
    
    bool exec()
    {
	return result->exec();
    }

    TQIBaseResult *result;
};

static bool getIBaseError(TQString& msg, ISC_STATUS* status, long &sqlcode)
{
    if (status[0] != 1 || status[1] <= 0)
	return false;
    
    sqlcode = isc_sqlcode(status);
    char buf[512];
    isc_sql_interprete(sqlcode, buf, 512);
    msg = TQString::fromUtf8(buf);
    return true;
}

static void createDA(XSQLDA *&sqlda)
{
    sqlda = (XSQLDA *) malloc(XSQLDA_LENGTH(1));
    sqlda->sqln = 1;
    sqlda->sqld = 0;
    sqlda->version = SQLDA_VERSION1;
    sqlda->sqlvar[0].sqlind = 0;
    sqlda->sqlvar[0].sqldata = 0;
}

static void enlargeDA(XSQLDA *&sqlda, int n)
{
    free(sqlda);
    sqlda = (XSQLDA *) malloc(XSQLDA_LENGTH(n));
    sqlda->sqln = n;
    sqlda->version = SQLDA_VERSION1;    
}

static void initDA(XSQLDA *sqlda)
{
    for (int i = 0; i < sqlda->sqld; ++i) {
	switch (sqlda->sqlvar[i].sqltype & ~1) {
	case SQL_INT64:
	case SQL_LONG:
	case SQL_SHORT:
	case SQL_FLOAT:
	case SQL_DOUBLE:
	case SQL_TIMESTAMP:
	case SQL_TYPE_TIME:
        case SQL_TYPE_DATE:
	case SQL_TEXT:
	case SQL_BLOB:
	    sqlda->sqlvar[i].sqldata = (char*)malloc(sqlda->sqlvar[i].sqllen);
	    break;
	case SQL_VARYING:
	    sqlda->sqlvar[i].sqldata = (char*)malloc(sqlda->sqlvar[i].sqllen + sizeof(short));
	    break;
	default:
	    // not supported - do not bind.
	    sqlda->sqlvar[i].sqldata = 0;
	    break;
	}
	if (sqlda->sqlvar[i].sqltype & 1) {
	    sqlda->sqlvar[i].sqlind = (short*)malloc(sizeof(short));
	    *(sqlda->sqlvar[i].sqlind) = 0;
	} else {
	    sqlda->sqlvar[i].sqlind = 0;
	}
    }
}

static void delDA(XSQLDA *&sqlda)
{
    if (!sqlda)
	return;
    for (int i = 0; i < sqlda->sqld; ++i) {
	free(sqlda->sqlvar[i].sqlind);
	free(sqlda->sqlvar[i].sqldata);
    }
    free(sqlda);
    sqlda = 0;    
}

static TQVariant::Type qIBaseTypeName(int iType)
{
    switch (iType) {
    case blr_varying:
    case blr_varying2:
    case blr_text:
    case blr_cstring:
    case blr_cstring2:
	return TQVariant::String;
    case blr_sql_time:
	return TQVariant::Time;
    case blr_sql_date:
        return TQVariant::Date;
    case blr_timestamp:
	return TQVariant::DateTime;
    case blr_blob:
	return TQVariant::ByteArray;
    case blr_quad:
    case blr_short:
    case blr_long:
	return TQVariant::Int;
    case blr_int64:
	return TQVariant::LongLong;
    case blr_float:
    case blr_d_float:
    case blr_double:
	return TQVariant::Double;
    }
    return TQVariant::Invalid;
}

static TQVariant::Type qIBaseTypeName2(int iType)
{
    switch(iType & ~1) {
    case SQL_VARYING:
    case SQL_TEXT:
	return TQVariant::String;
    case SQL_LONG:
    case SQL_SHORT:
	return TQVariant::Int;
    case SQL_INT64:
	return TQVariant::LongLong;
    case SQL_FLOAT:
    case SQL_DOUBLE:
	return TQVariant::Double;
    case SQL_TIMESTAMP:
	return TQVariant::DateTime;
    case SQL_TYPE_DATE:
        return TQVariant::Date;
    case SQL_TYPE_TIME:
	return TQVariant::Time;
    default:
	return TQVariant::Invalid;
    }
}

static ISC_TIME toTime(const TQTime &t)
{
    static const TQTime midnight(0, 0, 0, 0);
    return (ISC_TIME)midnight.msecsTo(t) * 10;
}

static ISC_DATE toDate(const TQDate &d)
{
    static const TQDate basedate(1858, 11, 17);
    return (ISC_DATE)basedate.daysTo(d);
}

static ISC_TIMESTAMP toTimeStamp(const TQDateTime &dt)
{
    ISC_TIMESTAMP ts;
    ts.timestamp_time = toTime(dt.time());
    ts.timestamp_date = toDate(dt.date());
    return ts;
}

static TQTime toTQTime(ISC_TIME time)
{
    // have to demangle the structure ourselves because isc_decode_time
    // strips the msecs
    static const TQTime t;
    return t.addMSecs(time / 10);
}

static TQDate toTQDate(ISC_DATE d)
{
    static const TQDate bd(1858, 11, 17);
    return bd.addDays(d);
}

static TQDateTime toTQDateTime(ISC_TIMESTAMP *ts)
{
    return TQDateTime(toTQDate(ts->timestamp_date), toTQTime(ts->timestamp_time));
}

class TQIBaseDriverPrivate
{
public:
    TQIBaseDriverPrivate(TQIBaseDriver *d): q(d)
    {
	ibase = 0;
	trans = 0;
    }
    
    bool isError(const TQString &msg = TQString::null, TQSqlError::Type typ = TQSqlError::Unknown) 
    {
	TQString imsg;
	long sqlcode;
	if (!getIBaseError(imsg, status, sqlcode))
	    return false;
	
	q->setLastError(TQSqlError(msg, imsg, typ, (int)sqlcode));
	return true;
    }

public:    
    TQIBaseDriver* q;
    isc_db_handle ibase;
    isc_tr_handle trans;
    ISC_STATUS status[20];    
};

class TQIBaseResultPrivate
{
public:
    TQIBaseResultPrivate(TQIBaseResult *d, const TQIBaseDriver *ddb);
    ~TQIBaseResultPrivate() { cleanup(); }
    
    void cleanup();
    bool isError(const TQString &msg = TQString::null, TQSqlError::Type typ = TQSqlError::Unknown) 
    {
	TQString imsg;
	long sqlcode;
	if (!getIBaseError(imsg, status, sqlcode))
	    return false;
	
	q->setLastError(TQSqlError(msg, imsg, typ, (int)sqlcode));
	return true;
    }    
    
    bool transaction();
    bool commit();

    bool isSelect();
    TQVariant fetchBlob(ISC_QUAD *bId);
    void writeBlob(int i, const TQByteArray &ba);

public:    
    TQIBaseResult *q; 
    const TQIBaseDriver *db;
    ISC_STATUS status[20];
    isc_tr_handle trans;
    //indicator whether we have a local transaction or a transaction on driver level    
    bool localTransaction;
    isc_stmt_handle stmt;
    isc_db_handle ibase;
    XSQLDA *sqlda; // output sqlda
    XSQLDA *inda; // input parameters
    int queryType;
};

TQIBaseResultPrivate::TQIBaseResultPrivate(TQIBaseResult *d, const TQIBaseDriver *ddb):
    q(d), db(ddb), trans(0), stmt(0), ibase(ddb->d->ibase), sqlda(0), inda(0), queryType(-1)
{
    localTransaction = (ddb->d->ibase == 0);
}

void TQIBaseResultPrivate::cleanup()
{
    if (stmt) {
	isc_dsql_free_statement(status, &stmt, DSQL_drop);
	stmt = 0;
    }

    commit();
    if (!localTransaction)
	trans = 0;

    delDA(sqlda);
    delDA(inda);

    queryType = -1;
    q->cleanup();
}

void TQIBaseResultPrivate::writeBlob(int i, const TQByteArray &ba)
{
    isc_blob_handle handle = 0;
    ISC_QUAD *bId = (ISC_QUAD*)inda->sqlvar[i].sqldata;
    isc_create_blob2(status, &ibase, &trans, &handle, bId, 0, 0);
    if (!isError("Unable to create BLOB", TQSqlError::Statement)) {
	uint i = 0;
	while (i < ba.size()) {
	    isc_put_segment(status, &handle, TQMIN(ba.size() - i, SHRT_MAX), ba.data());
	    if (isError("Unable to write BLOB"))
		break;
	    i += SHRT_MAX;
	}
    }
    isc_close_blob(status, &handle);
}

TQVariant TQIBaseResultPrivate::fetchBlob(ISC_QUAD *bId)
{
    isc_blob_handle handle = 0;

    isc_open_blob2(status, &ibase, &trans, &handle, bId, 0, 0);
    if (isError("Unable to open BLOB", TQSqlError::Statement))
	return TQVariant();

    unsigned short len = 0;
    TQByteArray ba(255);
    ISC_STATUS stat = isc_get_segment(status, &handle, &len, ba.size(), ba.data());
    while (status[1] == isc_segment) {
	uint osize = ba.size();
	// double the amount of data fetched on each iteration
	ba.resize(TQMIN(ba.size() * 2, SHRT_MAX));
	stat = isc_get_segment(status, &handle, &len, osize, ba.data() + osize);
    }
    bool isErr = isError("Unable to read BLOB", TQSqlError::Statement);
    isc_close_blob(status, &handle);
    if (isErr)
	return TQVariant();

    if (ba.size() > 255)
	ba.resize(ba.size() / 2 + len);
    else
	ba.resize(len);

    return ba;
}

bool TQIBaseResultPrivate::isSelect()
{
    char acBuffer[9];
    char qType = isc_info_sql_stmt_type;
    isc_dsql_sql_info(status, &stmt, 1, &qType, sizeof(acBuffer), acBuffer);
    if (isError("Could not get query info", TQSqlError::Statement))
	return false;    
    int iLength = isc_vax_integer(&acBuffer[1], 2);
    queryType = isc_vax_integer(&acBuffer[3], iLength);
    return (queryType == isc_info_sql_stmt_select);
}

bool TQIBaseResultPrivate::transaction()
{
    if (trans)
	return true;
    if (db->d->trans) {
	localTransaction = false;
	trans = db->d->trans;
	return true;
    }
    localTransaction = true;
    
    isc_start_transaction(status, &trans, 1, &ibase, 0, NULL);
    if (isError("Could not start transaction", TQSqlError::Statement))
	return false;
    
    return true;
}

// does nothing if the transaction is on the
// driver level
bool TQIBaseResultPrivate::commit()
{
    if (!trans)
	return false;
    // don't commit driver's transaction, the driver will do it for us
    if (!localTransaction)
	return true;
    
    isc_commit_transaction(status, &trans);
    trans = 0;
    return !isError("Unable to commit transaction", TQSqlError::Statement);
}

//////////

TQIBaseResult::TQIBaseResult(const TQIBaseDriver* db):
    TQtSqlCachedResult(db)
{
    d = new TQIBaseResultPrivate(this, db);
    setExtension(new TQIBasePreparedExtension(this));
}

TQIBaseResult::~TQIBaseResult()
{
    delete d;
}

bool TQIBaseResult::prepare(const TQString& query)
{
    //tqDebug("prepare: %s", query.ascii());
    if (!driver() || !driver()->isOpen() || driver()->isOpenError())
	return false;
    d->cleanup();
    setActive(false);
    setAt(TQSql::BeforeFirst);
    
    createDA(d->sqlda);
    createDA(d->inda);
    
    if (!d->transaction())
	return false;
    
    isc_dsql_allocate_statement(d->status, &d->ibase, &d->stmt);
    if (d->isError("Could not allocate statement", TQSqlError::Statement))
	return false;
    isc_dsql_prepare(d->status, &d->trans, &d->stmt, 0, query.utf8().data(), 3, d->sqlda);
    if (d->isError("Could not prepare statement", TQSqlError::Statement))
	return false;
    
    isc_dsql_describe_bind(d->status, &d->stmt, 1, d->inda);
    if (d->isError("Could not describe input statement", TQSqlError::Statement))
	return false;
    if (d->inda->sqld > d->inda->sqln) {
	enlargeDA(d->inda, d->inda->sqld);

	isc_dsql_describe_bind(d->status, &d->stmt, 1, d->inda);
	if (d->isError("Could not describe input statement", TQSqlError::Statement))
	    return false;
    }
    initDA(d->inda);    
    if (d->sqlda->sqld > d->sqlda->sqln) {
	// need more field descriptors
	enlargeDA(d->sqlda, d->sqlda->sqld);

	isc_dsql_describe(d->status, &d->stmt, 1, d->sqlda);
	if (d->isError("Could not describe statement", TQSqlError::Statement))
	    return false;
    }
    initDA(d->sqlda);
 
    setSelect(d->isSelect());
    if (!isSelect()) {
	free(d->sqlda);
	d->sqlda = 0;
    }

    return true;
}

bool TQIBaseResult::exec()
{    
    if (!driver() || !driver()->isOpen() || driver()->isOpenError())
	return false;
    setActive(false);
    setAt(TQSql::BeforeFirst);

    if (d->inda && extension()->index.count() > 0) {
	TQMap<int, TQString>::ConstIterator it;
	if ((int)extension()->index.count() > d->inda->sqld) {
	    tqWarning("TQIBaseResult::exec: Parameter mismatch, expected %d, got %d parameters", d->inda->sqld, extension()->index.count());
	    return false;
	}
	int para = 0;
	for (it = extension()->index.constBegin(); it != extension()->index.constEnd(); ++it, ++para) {
	    if (para >= d->inda->sqld)
		break;
	    if (!d->inda->sqlvar[para].sqldata)
		continue;
	    const TQVariant val(extension()->values[it.data()].value);
	    if (d->inda->sqlvar[para].sqltype & 1) {
		if (val.isNull()) {
		    // set null indicator
		    *(d->inda->sqlvar[para].sqlind) = 1;
		    // and set the value to 0, otherwise it would count as empty string.
		    *((short*)d->inda->sqlvar[para].sqldata) = 0;
		    continue;
		}
		// a value of 0 means non-null.
		*(d->inda->sqlvar[para].sqlind) = 0;
	    }
	    switch(d->inda->sqlvar[para].sqltype & ~1) {
	    case SQL_INT64:
		if (d->inda->sqlvar[para].sqlscale < 0)
		    *((TQ_LLONG*)d->inda->sqlvar[para].sqldata) = TQ_LLONG(val.toDouble() * 
									 pow(10.0, d->inda->sqlvar[para].sqlscale * -1));
		else
		    *((TQ_LLONG*)d->inda->sqlvar[para].sqldata) = val.toLongLong();
		break;
	    case SQL_LONG:
		*((long*)d->inda->sqlvar[para].sqldata) = (long)val.toLongLong();
		break;
	    case SQL_SHORT:
		*((short*)d->inda->sqlvar[para].sqldata) = (short)val.toInt();
		break;
	    case SQL_FLOAT:
		*((float*)d->inda->sqlvar[para].sqldata) = (float)val.toDouble();
		break;
	    case SQL_DOUBLE:
		*((double*)d->inda->sqlvar[para].sqldata) = val.toDouble();
		break;
	    case SQL_TIMESTAMP:
		*((ISC_TIMESTAMP*)d->inda->sqlvar[para].sqldata) = toTimeStamp(val.toDateTime());
		break;
	    case SQL_TYPE_TIME:
		*((ISC_TIME*)d->inda->sqlvar[para].sqldata) = toTime(val.toTime());
		break;
            case SQL_TYPE_DATE:
		*((ISC_DATE*)d->inda->sqlvar[para].sqldata) = toDate(val.toDate());
		break;
	    case SQL_VARYING: {
		TQCString str(val.toString().utf8()); // keep a copy of the string alive in this scope
		short buflen = d->inda->sqlvar[para].sqllen;
		if (str.length() < (uint)buflen)
		    buflen = str.length();
		*(short*)d->inda->sqlvar[para].sqldata = buflen; // first two bytes is the length
		memcpy(d->inda->sqlvar[para].sqldata + sizeof(short), str.data(), buflen);
		break; }
	    case SQL_TEXT: {
		TQCString str(val.toString().utf8().leftJustify(d->inda->sqlvar[para].sqllen, ' ', true));
		memcpy(d->inda->sqlvar[para].sqldata, str.data(), d->inda->sqlvar[para].sqllen);
		break; }
	case SQL_BLOB:
		d->writeBlob(para, val.toByteArray());
		break;
	    default:
		break;
	    }
	}
    }

    if (colCount()) {
	isc_dsql_free_statement(d->status, &d->stmt, DSQL_close);
	if (d->isError("Unable to close statement"))
	    return false;
	cleanup();
    }
    if (d->sqlda)
	init(d->sqlda->sqld);
    isc_dsql_execute2(d->status, &d->trans, &d->stmt, 1, d->inda, 0);
    if (d->isError("Unable to execute query"))
	return false;

    setActive(true);
    return true;
}

bool TQIBaseResult::reset (const TQString& query)
{
//    tqDebug("reset: %s", query.ascii());
    if (!driver() || !driver()->isOpen() || driver()->isOpenError())
	return false;
    d->cleanup();
    setActive(false);
    setAt(TQSql::BeforeFirst);
    
    createDA(d->sqlda);

    if (!d->transaction())
	return false;
    
    isc_dsql_allocate_statement(d->status, &d->ibase, &d->stmt);
    if (d->isError("Could not allocate statement", TQSqlError::Statement))
	return false;
    isc_dsql_prepare(d->status, &d->trans, &d->stmt, 0, query.utf8().data(), 3, d->sqlda);
    if (d->isError("Could not prepare statement", TQSqlError::Statement))
	return false;
    
    if (d->sqlda->sqld > d->sqlda->sqln) {
	// need more field descriptors
	int n = d->sqlda->sqld;
	free(d->sqlda);
	d->sqlda = (XSQLDA *) malloc(XSQLDA_LENGTH(n));
	d->sqlda->sqln = n;
	d->sqlda->version = SQLDA_VERSION1;

	isc_dsql_describe(d->status, &d->stmt, 1, d->sqlda);
	if (d->isError("Could not describe statement", TQSqlError::Statement))
	    return false;
    }
    
    initDA(d->sqlda);

    setSelect(d->isSelect());
    if (isSelect()) {
	init(d->sqlda->sqld);
    } else {
	free(d->sqlda);
	d->sqlda = 0;
    }
    
    isc_dsql_execute(d->status, &d->trans, &d->stmt, 1, 0);
    if (d->isError("Unable to execute query"))
	return false;

    // commit non-select queries (if they are local)
    if (!isSelect() && !d->commit())
	return false;
    
    setActive(true);
    return true;
}

bool TQIBaseResult::gotoNext(TQtSqlCachedResult::RowCache* row)
{
    ISC_STATUS stat = isc_dsql_fetch(d->status, &d->stmt, 1, d->sqlda);
    
    if (stat == 100) {
	// no more rows
	setAt(TQSql::AfterLast);
	return false;
    }
    if (d->isError("Could not fetch next item", TQSqlError::Statement))
	return false;
    if (!row) // not interested in actual values
	return true;

    Q_ASSERT(row);
    Q_ASSERT((int)row->size() == d->sqlda->sqld);
    for (int i = 0; i < d->sqlda->sqld; ++i) {
	char *buf = d->sqlda->sqlvar[i].sqldata;
	int size = d->sqlda->sqlvar[i].sqllen;
	Q_ASSERT(buf);
	
	if ((d->sqlda->sqlvar[i].sqltype & 1) && *d->sqlda->sqlvar[i].sqlind) {
	    // null value
	    TQVariant v;
	    v.cast(qIBaseTypeName2(d->sqlda->sqlvar[i].sqltype));
	    (*row)[i] = v;
	    continue;
	}
    
	switch(d->sqlda->sqlvar[i].sqltype & ~1) {
	case SQL_VARYING:
	    // pascal strings - a short with a length information followed by the data
	    (*row)[i] = TQString::fromUtf8(buf + sizeof(short), *(short*)buf);
	    break;
	case SQL_INT64:
	    if (d->sqlda->sqlvar[i].sqlscale < 0)
		(*row)[i] = *(TQ_LLONG*)buf * pow(10.0, d->sqlda->sqlvar[i].sqlscale);
	    else
		(*row)[i] = TQVariant(*(TQ_LLONG*)buf);
	    break;
	case SQL_LONG:
	    if (sizeof(int) == sizeof(long)) //dear compiler: please optimize me out.
		(*row)[i] = TQVariant((int)(*(long*)buf));
	    else
		(*row)[i] = TQVariant((TQ_LLONG)(*(long*)buf));
	    break;
	case SQL_SHORT:
	    (*row)[i] = TQVariant((int)(*(short*)buf));
	    break;
	case SQL_FLOAT:
	    (*row)[i] = TQVariant((double)(*(float*)buf));	    
	    break;
	case SQL_DOUBLE:
	    (*row)[i] = TQVariant(*(double*)buf);
	    break;
	case SQL_TIMESTAMP:
	    (*row)[i] = toTQDateTime((ISC_TIMESTAMP*)buf);
            break;
	case SQL_TYPE_TIME:
	    (*row)[i] = toTQTime(*(ISC_TIME*)buf);
	    break;
        case SQL_TYPE_DATE:
	    (*row)[i] = toTQDate(*(ISC_DATE*)buf);
            break;
	case SQL_TEXT:
	    (*row)[i] = TQString::fromUtf8(buf, size);
	    break;
	case SQL_BLOB:
	    (*row)[i] = d->fetchBlob((ISC_QUAD*)buf);
	    break;
	default:
	    // unknown type - don't even try to fetch
	    (*row)[i] = TQVariant();
	    break;
	}	
    }

    return true;    
}

int TQIBaseResult::size()
{
    static char sizeInfo[] = {isc_info_sql_records};
    char buf[33];
    
    if (!isActive() || !isSelect())
	return -1;
    
    isc_dsql_sql_info(d->status, &d->stmt, sizeof(sizeInfo), sizeInfo, sizeof(buf), buf);
    for (char* c = buf + 3; *c != isc_info_end; /*nothing*/) {
	char ct = *c++;
	short len = isc_vax_integer(c, 2);
	c += 2;
	int val = isc_vax_integer(c, len);
	c += len;
	if (ct == isc_info_req_select_count)
	    return val;
    }
    return -1;
}

int TQIBaseResult::numRowsAffected()
{
    static char acCountInfo[] = {isc_info_sql_records};
    char cCountType;

    switch (d->queryType) {
    case isc_info_sql_stmt_select:
	cCountType = isc_info_req_select_count;
	break;
    case isc_info_sql_stmt_update:
	cCountType = isc_info_req_update_count;
	break;
    case isc_info_sql_stmt_delete:
	cCountType = isc_info_req_delete_count;
	break;
    case isc_info_sql_stmt_insert:
	cCountType = isc_info_req_insert_count;
	break;
    }

    char acBuffer[33];
    int iResult = -1;    
    isc_dsql_sql_info(d->status, &d->stmt, sizeof(acCountInfo), acCountInfo, sizeof(acBuffer), acBuffer);
    if (d->isError("Could not get statement info", TQSqlError::Statement))
	return -1;
    for (char *pcBuf = acBuffer + 3; *pcBuf != isc_info_end; /*nothing*/) {
	char cType = *pcBuf++;
	short sLength = isc_vax_integer (pcBuf, 2);
	pcBuf += 2;
	int iValue = isc_vax_integer (pcBuf, sLength);
	pcBuf += sLength;
		
	if (cType == cCountType) {
	    iResult = iValue;
	    break;
	}
    }
    return iResult;    
}

/*********************************/

TQIBaseDriver::TQIBaseDriver(TQObject * parent, const char * name)
    : TQSqlDriver(parent, name ? name : TQIBASE_DRIVER_NAME)
{
    d = new TQIBaseDriverPrivate(this);
}

TQIBaseDriver::TQIBaseDriver(void *connection, TQObject *parent, const char *name)
    : TQSqlDriver(parent, name ? name : TQIBASE_DRIVER_NAME)
{
    d = new TQIBaseDriverPrivate(this);
    d->ibase = (isc_db_handle)(long int)connection;
    setOpen(true);
    setOpenError(false);
}

TQIBaseDriver::~TQIBaseDriver()
{
    delete d;
}

bool TQIBaseDriver::hasFeature(DriverFeature f) const
{
    switch (f) {
    case Transactions:
//    case QuerySize:
    case PreparedQueries:
    case PositionalPlaceholders:
    case Unicode:
    case BLOB:
        return true;
    default:
        return false;
    }
}

bool TQIBaseDriver::open(const TQString & db,
	  const TQString & user,
	  const TQString & password,
	  const TQString & host,
	  int /*port*/,
	  const TQString & /* connOpts */)
{
    if (isOpen())
        close();
    
    static const char enc[8] = "UTF_FSS";
    TQCString usr = user.local8Bit();
    TQCString pass = password.local8Bit();
    usr.truncate(255);
    pass.truncate(255);

    TQByteArray ba(usr.length() + pass.length() + sizeof(enc) + 6);
    int i = -1;
    ba[++i] = isc_dpb_version1;
    ba[++i] = isc_dpb_user_name;
    ba[++i] = usr.length();
    memcpy(&ba[++i], usr.data(), usr.length());
    i += usr.length();
    ba[i] = isc_dpb_password;
    ba[++i] = pass.length();
    memcpy(&ba[++i], pass.data(), pass.length());
    i += pass.length();
    ba[i] = isc_dpb_lc_ctype;
    ba[++i] = sizeof(enc) - 1;
    memcpy(&ba[++i], enc, sizeof(enc) - 1);
    i += sizeof(enc) - 1;

    TQString ldb;
    if (!host.isEmpty())
	ldb += host + ":";
    ldb += db;
    isc_attach_database(d->status, 0, (char*)ldb.latin1(), &d->ibase, i, ba.data());
    if (d->isError("Error opening database", TQSqlError::Connection)) {
	setOpenError(true);
	return false;
    }
    
    setOpen(true);
    return true;
}

void TQIBaseDriver::close()
{
    if (isOpen()) {
	isc_detach_database(d->status, &d->ibase);
	d->ibase = 0;
	setOpen(false);
	setOpenError(false);
    }
}

bool TQIBaseDriver::ping()
{
    if ( !isOpen() ) {
        return false;
    }

    // FIXME
    // Implement ping if available
    return true;
}

TQSqlQuery TQIBaseDriver::createQuery() const
{
    return TQSqlQuery(new TQIBaseResult(this));
}

bool TQIBaseDriver::beginTransaction()
{
    if (!isOpen() || isOpenError())
	return false;
    if (d->trans)
	return false;
    
    isc_start_transaction(d->status, &d->trans, 1, &d->ibase, 0, NULL);    
    return !d->isError("Could not start transaction", TQSqlError::Transaction);
}

bool TQIBaseDriver::commitTransaction()
{
    if (!isOpen() || isOpenError())
	return false;
    if (!d->trans)
	return false;

    isc_commit_transaction(d->status, &d->trans);
    d->trans = 0;
    return !d->isError("Unable to commit transaction", TQSqlError::Transaction);
}

bool TQIBaseDriver::rollbackTransaction()
{
    if (!isOpen() || isOpenError())
	return false;
    if (!d->trans)
	return false;
    
    isc_rollback_transaction(d->status, &d->trans);
    d->trans = 0;
    return !d->isError("Unable to rollback transaction", TQSqlError::Transaction);
}

TQStringList TQIBaseDriver::tables(const TQString& typeName) const
{
    TQStringList res;
    if (!isOpen())
	return res;

    int type = typeName.isEmpty() ? (int)TQSql::Tables | (int)TQSql::Views : typeName.toInt();
    TQString typeFilter;

    if (type == (int)TQSql::SystemTables) {
	typeFilter += "RDB$SYSTEM_FLAG != 0";
    } else if (type == ((int)TQSql::SystemTables | (int)TQSql::Views)) {
	typeFilter += "RDB$SYSTEM_FLAG != 0 OR RDB$VIEW_BLR NOT NULL";
    } else {
	if (!(type & (int)TQSql::SystemTables))
	    typeFilter += "RDB$SYSTEM_FLAG = 0 AND ";
	if (!(type & (int)TQSql::Views))
	    typeFilter += "RDB$VIEW_BLR IS NULL AND ";
	if (!(type & (int)TQSql::Tables))
	    typeFilter += "RDB$VIEW_BLR IS NOT NULL AND ";
	if (!typeFilter.isEmpty())
	    typeFilter.truncate(typeFilter.length() - 5);
    }
    if (!typeFilter.isEmpty())
	typeFilter.prepend("where ");
    
    TQSqlQuery q = createQuery();
    q.setForwardOnly(true);
    if (!q.exec("select rdb$relation_name from rdb$relations " + typeFilter))
	return res;
    while(q.next())
	    res << q.value(0).toString().stripWhiteSpace();

    return res;
}

TQSqlRecord TQIBaseDriver::record(const TQString& tablename) const
{
    TQSqlRecord rec;
    if (!isOpen())
	return rec;
	
    TQSqlQuery q = createQuery();
    q.setForwardOnly(true);
    
    q.exec("SELECT a.RDB$FIELD_NAME, b.RDB$FIELD_TYPE "
	   "FROM RDB$RELATION_FIELDS a, RDB$FIELDS b "
	   "WHERE b.RDB$FIELD_NAME = a.RDB$FIELD_SOURCE "
	   "AND a.RDB$RELATION_NAME = '" + tablename.upper()+ "' "
	   "ORDER BY RDB$FIELD_POSITION");
    while (q.next()) {
	TQSqlField field(q.value(0).toString().stripWhiteSpace(), qIBaseTypeName(q.value(1).toInt()));
	rec.append(field);
   }

    return rec;
}

TQSqlRecordInfo TQIBaseDriver::recordInfo(const TQString& tablename) const
{
    TQSqlRecordInfo rec;
    if (!isOpen())
	return rec;
	
    TQSqlQuery q = createQuery();
    q.setForwardOnly(true);

    q.exec("SELECT a.RDB$FIELD_NAME, b.RDB$FIELD_TYPE, b.RDB$FIELD_LENGTH, b.RDB$FIELD_SCALE, "
	   "b.RDB$FIELD_PRECISION, a.RDB$NULL_FLAG "
	   "FROM RDB$RELATION_FIELDS a, RDB$FIELDS b "
	   "WHERE b.RDB$FIELD_NAME = a.RDB$FIELD_SOURCE "
	   "AND a.RDB$RELATION_NAME = '" + tablename.upper() + "' "
	   "ORDER BY a.RDB$FIELD_POSITION");

    while (q.next()) {
	TQVariant::Type type = qIBaseTypeName(q.value(1).toInt());
	TQSqlFieldInfo field(q.value(0).toString().stripWhiteSpace(), type, q.value(5).toInt(),
			     q.value(2).toInt(), q.value(4).toInt(), TQVariant());
		
	rec.append(field);
    }

    return rec;
}

TQSqlIndex TQIBaseDriver::primaryIndex(const TQString &table) const
{
    TQSqlIndex index(table);
    if (!isOpen())
	return index;

    TQSqlQuery q = createQuery();
    q.setForwardOnly(true);
    q.exec("SELECT a.RDB$INDEX_NAME, b.RDB$FIELD_NAME, d.RDB$FIELD_TYPE "
	   "FROM RDB$RELATION_CONSTRAINTS a, RDB$INDEX_SEGMENTS b, RDB$RELATION_FIELDS c, RDB$FIELDS d "
	   "WHERE a.RDB$CONSTRAINT_TYPE = 'PRIMARY KEY' "
	   "AND a.RDB$RELATION_NAME = '" + table.upper() + "' "
	   "AND a.RDB$INDEX_NAME = b.RDB$INDEX_NAME "
	   "AND c.RDB$RELATION_NAME = a.RDB$RELATION_NAME "
	   "AND c.RDB$FIELD_NAME = b.RDB$FIELD_NAME "
	   "AND d.RDB$FIELD_NAME = c.RDB$FIELD_SOURCE "
	   "ORDER BY b.RDB$FIELD_POSITION");

    while (q.next()) {
	TQSqlField field(q.value(1).toString().stripWhiteSpace(), qIBaseTypeName(q.value(2).toInt()));
	index.append(field); //TODO: asc? desc?
	index.setName(q.value(0).toString());
    }

    return index;
}

TQSqlRecord TQIBaseDriver::record(const TQSqlQuery& query) const
{
    TQSqlRecord rec;
    if (query.isActive() && query.driver() == this) {
	TQIBaseResult* result = (TQIBaseResult*)query.result();
	if (!result->d->sqlda)
	    return rec;
	XSQLVAR v;
	for (int i = 0; i < result->d->sqlda->sqld; ++i) {
	    v = result->d->sqlda->sqlvar[i];
	    TQSqlField f(TQString::fromLatin1(v.sqlname, v.sqlname_length).stripWhiteSpace(),
			qIBaseTypeName2(result->d->sqlda->sqlvar[i].sqltype));
	    rec.append(f);
	}
    }
    return rec;
}

TQSqlRecordInfo TQIBaseDriver::recordInfo(const TQSqlQuery& query) const
{
    TQSqlRecordInfo rec;
    if (query.isActive() && query.driver() == this) {
	TQIBaseResult* result = (TQIBaseResult*)query.result();
	if (!result->d->sqlda)
	    return rec;
	XSQLVAR v;
	for (int i = 0; i < result->d->sqlda->sqld; ++i) {
	    v = result->d->sqlda->sqlvar[i];
	    TQSqlFieldInfo f(TQString::fromLatin1(v.sqlname, v.sqlname_length).stripWhiteSpace(), 
			    qIBaseTypeName2(result->d->sqlda->sqlvar[i].sqltype),
			    -1, v.sqllen, TQABS(v.sqlscale), TQVariant(), v.sqltype);
	    rec.append(f);
	}
    }
    return rec;
}

TQString TQIBaseDriver::formatValue(const TQSqlField* field, bool trimStrings) const
{
    switch (field->type()) {
    case TQVariant::DateTime: {
	TQDateTime datetime = field->value().toDateTime();
	if (datetime.isValid())
	    return "'" + TQString::number(datetime.date().year()) + "-" +
		TQString::number(datetime.date().month()) + "-" +
		TQString::number(datetime.date().day()) + " " +
		TQString::number(datetime.time().hour()) + ":" +
		TQString::number(datetime.time().minute()) + ":" +
		TQString::number(datetime.time().second()) + "." +
		TQString::number(datetime.time().msec()).rightJustify(3, '0', true) + "'";
	else
	    return "NULL";
    }
    case TQVariant::Time: {
	TQTime time = field->value().toTime();
	if (time.isValid())
	    return "'" + TQString::number(time.hour()) + ":" +
		TQString::number(time.minute()) + ":" +
		TQString::number(time.second()) + "." +
		TQString::number(time.msec()).rightJustify(3, '0', true) + "'";
	else
	    return "NULL";
    }
    case TQVariant::Date: {
	TQDate date = field->value().toDate();
	if (date.isValid())
	    return "'" + TQString::number(date.year()) + "-" +
		TQString::number(date.month()) + "-" +
		TQString::number(date.day()) + "'";
	    else
		return "NULL";
    }
    default:
        return TQSqlDriver::formatValue(field, trimStrings);
    }	
}
