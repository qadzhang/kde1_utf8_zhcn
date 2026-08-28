/****************************************************************************
**
** Implementation of TQDns class.
**
** Created : 991122
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the network module of the TQt GUI Toolkit.
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

#include "qplatformdefs.h"

// POSIX Large File Support redefines open -> open64
#if defined(open)
# undef open
#endif

// POSIX Large File Support redefines truncate -> truncate64
#if defined(truncate)
# undef truncate
#endif

// Solaris redefines connect -> __xnet_connect with _XOPEN_SOURCE_EXTENDED.
#if defined(connect)
# undef connect
#endif

// UnixWare 7 redefines socket -> _socket
#if defined(socket)
# undef socket
#endif

#include "ntqdns.h"

#ifndef TQT_NO_DNS

#include "ntqdatetime.h"
#include "ntqdict.h"
#include "ntqptrlist.h"
#include "ntqstring.h"
#include "ntqtimer.h"
#include "ntqapplication.h"
#include "ntqptrvector.h"
#include "ntqstrlist.h"
#include "ntqptrdict.h"
#include "ntqfile.h"
#include "ntqtextstream.h"
#include "ntqsocketdevice.h"
#include "ntqcleanuphandler.h"
#include <limits.h>
#ifdef Q_OS_MAC
#include "../3rdparty/dlcompat/dlfcn.h"
#endif

//#define TQDNS_DEBUG

static TQ_UINT16 id; // ### seeded started by now()


static TQDateTime * originOfTime = 0;

static TQCleanupHandler<TQDateTime> qdns_cleanup_time;

static TQ_UINT32 now()
{
    if ( originOfTime )
	return originOfTime->secsTo( TQDateTime::currentDateTime() );

    originOfTime = new TQDateTime( TQDateTime::currentDateTime() );
    ::id = originOfTime->time().msec() * 60 + originOfTime->time().second();
    qdns_cleanup_time.add( &originOfTime );
    return 0;
}

#if defined(__RES) && (__RES >= 19980901)
#define Q_MODERN_RES_API
#else
#endif

static TQPtrList<TQHostAddress> * ns = 0;
static TQStrList * domains = 0;
static bool ipv6support = false;

#if defined(Q_MODERN_RES_API)
#else
static int qdns_res_init()
{
#ifdef Q_OS_MAC
    typedef int (*PtrRes_init)();
    static PtrRes_init ptrRes_init = 0;
    if (!ptrRes_init)
	ptrRes_init = (PtrRes_init)DL_PREFIX(dlsym)(RTLD_NEXT, "res_init");
    if (ptrRes_init)
	return (*ptrRes_init)();
    else
	return -1;
#elif defined(Q_OS_UNIX)
    return res_init();
#else
    return 0; // not called at all on Windows.
#endif
}
#endif


class TQDnsPrivate {
public:
    TQDnsPrivate() : queryTimer( 0 ), noNames(false)
    {
#if defined(Q_DNS_SYNCHRONOUS)
#if defined(Q_OS_UNIX)
	noEventLoop = tqApp==0 || tqApp->loopLevel()==0;
#else
	noEventLoop = false;
#endif
#endif
    }
    ~TQDnsPrivate()
    {
	delete queryTimer;
    }
private:
    TQTimer * queryTimer;
    bool noNames;
#if defined(Q_DNS_SYNCHRONOUS)
    bool noEventLoop;
#endif

    friend class TQDns;
    friend class TQDnsAnswer;
};


class TQDnsRR;
class TQDnsDomain;



// TQDnsRR is the class used to store a single RR.  TQDnsRR can store
// all of the supported RR types.  a TQDnsRR is always cached.

// TQDnsRR is mostly constructed from the outside.  a but hacky, but
// permissible since the entire class is internal.

class TQDnsRR {
public:
    TQDnsRR( const TQString & label );
    ~TQDnsRR();

public:
    TQDnsDomain * domain;
    TQDns::RecordType t;
    bool nxdomain;
    bool current;
    TQ_UINT32 expireTime;
    TQ_UINT32 deleteTime;
    // somewhat space-wasting per-type data
    // a / aaaa
    TQHostAddress address;
    // cname / mx / srv / ptr
    TQString target;
    // mx / srv
    TQ_UINT16 priority;
    // srv
    TQ_UINT16 weight;
    TQ_UINT16 port;
    // txt
    TQString text; // could be overloaded into target...
private:

};


class TQDnsDomain {
public:
    TQDnsDomain( const TQString & label );
    ~TQDnsDomain();

    static void add( const TQString & label, TQDnsRR * );
    static TQPtrList<TQDnsRR> * cached( const TQDns * );

    void take( TQDnsRR * );

    void sweep( TQ_UINT32 thisSweep );

    bool isEmpty() const { return rrs == 0 || rrs->isEmpty(); }

    TQString name() const { return l; }

public:
    TQString l;
    TQPtrList<TQDnsRR> * rrs;
};


class TQDnsQuery: public TQTimer { // this inheritance is a very evil hack
public:
    TQDnsQuery():
	id( 0 ), t( TQDns::None ), step(0), started(0),
	dns( new TQPtrDict<void>(17) ) {}
    ~TQDnsQuery() { delete dns; }
    TQ_UINT16 id;
    TQDns::RecordType t;
    TQString l;

    uint step;
    TQ_UINT32 started;

    TQPtrDict<void> * dns;
};



class TQDnsAnswer {
public:
    TQDnsAnswer( TQDnsQuery * );
    TQDnsAnswer( const TQByteArray &, TQDnsQuery * );
    ~TQDnsAnswer();

    void parse();
    void notify();

    bool ok;

private:
    TQDnsQuery * query;

    TQ_UINT8 * answer;
    int size;
    int pp;

    TQPtrList<TQDnsRR> * rrs;

    // convenience
    int next;
    int ttl;
    TQString label;
    TQDnsRR * rr;

    TQString readString(bool multipleLabels = true);
    void parseA();
    void parseAaaa();
    void parseMx();
    void parseSrv();
    void parseCname();
    void parsePtr();
    void parseTxt();
    void parseNs();
};


TQDnsRR::TQDnsRR( const TQString & label )
    : domain( 0 ), t( TQDns::None ),
      nxdomain( false ), current( false ),
      expireTime( 0 ), deleteTime( 0 ),
      priority( 0 ), weight( 0 ), port( 0 )
{
    TQDnsDomain::add( label, this );
}


// not supposed to be deleted except by TQDnsDomain
TQDnsRR::~TQDnsRR()
{
    // nothing is necessary
}


// this one just sticks in a NXDomain
TQDnsAnswer::TQDnsAnswer( TQDnsQuery * query_ )
{
    ok = true;

    answer = 0;
    size = 0;
    query = query_;
    pp = 0;
    rrs = new TQPtrList<TQDnsRR>;
    rrs->setAutoDelete( false );
    next = size;
    ttl = 0;
    label = TQString::null;
    rr = 0;

    TQDnsRR * newrr = new TQDnsRR( query->l );
    newrr->t = query->t;
    newrr->deleteTime = query->started + 10;
    newrr->expireTime = query->started + 10;
    newrr->nxdomain = true;
    newrr->current = true;
    rrs->append( newrr );
}


TQDnsAnswer::TQDnsAnswer( const TQByteArray& answer_,
			TQDnsQuery * query_ )
{
    ok = true;

    answer = (TQ_UINT8 *)(answer_.data());
    size = (int)answer_.size();
    query = query_;
    pp = 0;
    rrs = new TQPtrList<TQDnsRR>;
    rrs->setAutoDelete( false );
    next = size;
    ttl = 0;
    label = TQString::null;
    rr = 0;
}


TQDnsAnswer::~TQDnsAnswer()
{
    if ( !ok && rrs ) {
	TQPtrListIterator<TQDnsRR> it( *rrs );
	TQDnsRR * tmprr;
	while( (tmprr=it.current()) != 0 ) {
	    ++it;
	    tmprr->t = TQDns::None; // will be deleted soonish
	}
    }
    delete rrs;
}


TQString TQDnsAnswer::readString(bool multipleLabels)
{
    int p = pp;
    TQString r = TQString::null;
    TQ_UINT8 b;
    for( ;; ) {
	b = 128;
        // Read one character
        if ( p >= 0 && p < size )
	    b = answer[p];

	switch( b >> 6 ) {
	case 0:
            // b is less than 64
	    p++;

            // Detect end of data
	    if ( b == 0 ) {
		if ( p > pp )
		    pp = p;
                return r.isNull() ? TQString( "." ) : r;
	    }

            // Read a label of size 'b' characters
            if ( !r.isNull() )
		r += '.';
            while( b-- > 0 ) {
                r += TQChar( answer[p] );
                p++;
            }

            // Return immediately if we were only supposed to read one
            // label.
            if (!multipleLabels)
                return r;

	    break;
	default:
            // Ignore unrecognized control character, or p was out of
            // range.
	    goto not_ok;
	case 3:
            // Use the next character to determine the relative offset
            // to jump to before continuing the packet parsing.
	    int q = ( (answer[p] & 0x3f) << 8 ) + answer[p+1];

	    if ( q >= pp || q >= p )
		goto not_ok;
	    if ( p >= pp )
		pp = p + 2;
	    p = q;
        }
    }
not_ok:
    ok = false;
    return TQString::null;
}



void TQDnsAnswer::parseA()
{
    if ( next != pp + 4 ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "TQDns: saw %d bytes long IN A for %s",
		next - pp, label.ascii() );
#endif
	return;
    }

    rr = new TQDnsRR( label );
    rr->t = TQDns::A;
    rr->address = TQHostAddress( ( answer[pp+0] << 24 ) +
				( answer[pp+1] << 16 ) +
				( answer[pp+2] <<  8 ) +
				( answer[pp+3] ) );
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns: saw %s IN A %s (ttl %d)", label.ascii(),
	    rr->address.toString().ascii(), ttl );
#endif
}


void TQDnsAnswer::parseAaaa()
{
    if ( next != pp + 16 ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "TQDns: saw %d bytes long IN Aaaa for %s",
		next - pp, label.ascii() );
#endif
	return;
    }

    rr = new TQDnsRR( label );
    rr->t = TQDns::Aaaa;
    rr->address = TQHostAddress( answer+pp );
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns: saw %s IN Aaaa %s (ttl %d)", label.ascii(),
	    rr->address.toString().ascii(), ttl );
#endif
}



void TQDnsAnswer::parseMx()
{
    if ( next < pp + 2 ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "TQDns: saw %d bytes long IN MX for %s",
		next - pp, label.ascii() );
#endif
	return;
    }

    rr = new TQDnsRR( label );
    rr->priority = (answer[pp] << 8) + answer[pp+1];
    pp += 2;
    rr->target = readString().lower();
    if ( !ok ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "TQDns: saw bad string in MX for %s", label.ascii() );
#endif
	return;
    }
    rr->t = TQDns::Mx;
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns: saw %s IN MX %d %s (ttl %d)", label.ascii(),
	    rr->priority, rr->target.ascii(), ttl );
#endif
}


void TQDnsAnswer::parseSrv()
{
    if ( next < pp + 6 ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "TQDns: saw %d bytes long IN SRV for %s",
		next - pp, label.ascii() );
#endif
	return;
    }

    rr = new TQDnsRR( label );
    rr->priority = (answer[pp] << 8) + answer[pp+1];
    rr->weight = (answer[pp+2] << 8) + answer[pp+3];
    rr->port = (answer[pp+4] << 8) + answer[pp+5];
    pp += 6;
    rr->target = readString().lower();
    if ( !ok ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "TQDns: saw bad string in SRV for %s", label.ascii() );
#endif
	return;
    }
    rr->t = TQDns::Srv;
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns: saw %s IN SRV %d %d %d %s (ttl %d)", label.ascii(),
	    rr->priority, rr->weight, rr->port, rr->target.ascii(), ttl );
#endif
}


void TQDnsAnswer::parseCname()
{
    TQString target = readString().lower();
    if ( !ok ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "TQDns: saw bad cname for for %s", label.ascii() );
#endif
	return;
    }

    rr = new TQDnsRR( label );
    rr->t = TQDns::Cname;
    rr->target = target;
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns: saw %s IN CNAME %s (ttl %d)", label.ascii(),
	    rr->target.ascii(), ttl );
#endif
}


void TQDnsAnswer::parseNs()
{
    TQString target = readString().lower();
    if ( !ok ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "TQDns: saw bad cname for for %s", label.ascii() );
#endif
	return;
    }

    // parse, but ignore

#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns: saw %s IN NS %s (ttl %d)", label.ascii(),
	    target.ascii(), ttl );
#endif
}


void TQDnsAnswer::parsePtr()
{
    TQString target = readString().lower();
    if ( !ok ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "TQDns: saw bad PTR for for %s", label.ascii() );
#endif
	return;
    }

    rr = new TQDnsRR( label );
    rr->t = TQDns::Ptr;
    rr->target = target;
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns: saw %s IN PTR %s (ttl %d)", label.ascii(),
	    rr->target.ascii(), ttl );
#endif
}


void TQDnsAnswer::parseTxt()
{
    TQString text = readString(false);
    if ( !ok ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "TQDns: saw bad TXT for for %s", label.ascii() );
#endif
	return;
    }

    rr = new TQDnsRR( label );
    rr->t = TQDns::Txt;
    rr->text = text;
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns: saw %s IN TXT \"%s\" (ttl %d)", label.ascii(),
	    rr->text.ascii(), ttl );
#endif
}


void TQDnsAnswer::parse()
{
    // okay, do the work...
    if ( (answer[2] & 0x78) != 0 ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "DNS Manager: answer to wrong query type (%d)", answer[1] );
#endif
	ok = false;
	return;
    }

    // AA
    bool aa = (answer[2] & 4) != 0;

    // TC
    if ( (answer[2] & 2) != 0 ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "DNS Manager: truncated answer; pressing on" );
#endif
    }

    // RD
    bool rd = (answer[2] & 1) != 0;

    // we don't test RA
    // we don't test the MBZ fields

    if ( (answer[3] & 0x0f) == 3 ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "DNS Manager: saw NXDomain for %s", query->l.ascii() );
#endif
	// NXDomain.  cache that for one minute.
	rr = new TQDnsRR( query->l );
	rr->t = query->t;
	rr->deleteTime = query->started + 60;
	rr->expireTime = query->started + 60;
	rr->nxdomain = true;
	rr->current = true;
	rrs->append( rr );
	return;
    }

    if ( (answer[3] & 0x0f) != 0 ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "DNS Manager: error code %d", answer[3] & 0x0f );
#endif
	ok = false;
	return;
    }

    int qdcount = ( answer[4] << 8 ) + answer[5];
    int ancount = ( answer[6] << 8 ) + answer[7];
    int nscount = ( answer[8] << 8 ) + answer[9];
    int adcount = (answer[10] << 8 ) +answer[11];

    pp = 12;

    // read query
    while( qdcount > 0 && pp < size ) {
	// should I compare the string against query->l?
	(void)readString();
	if ( !ok )
	    return;
	pp += 4;
	qdcount--;
    }

    // answers and stuff
    int rrno = 0;
    // if we parse the answer completely, but there are no answers,
    // ignore the entire thing.
    int answers = 0;
    while( ( rrno < ancount ||
	     ( ok && answers >0 && rrno < ancount + nscount + adcount ) ) &&
	   pp < size ) {
	label = readString().lower();
	if ( !ok )
	    return;
	int rdlength = 0;
	if ( pp + 10 <= size )
	    rdlength = ( answer[pp+8] << 8 ) + answer[pp+9];
	if ( pp + 10 + rdlength > size ) {
#if defined(TQDNS_DEBUG)
	    tqDebug( "DNS Manager: ran out of stuff to parse (%d+%d>%d (%d)",
		    pp, rdlength, size, rrno < ancount );
#endif
	    // if we're still in the AN section, we should go back and
	    // at least down the TTLs.  probably best to invalidate
	    // the results.
	    // the rrs list is good for this
	    ok = ( rrno < ancount );
	    return;
	}
	uint type, clas;
	type = ( answer[pp+0] << 8 ) + answer[pp+1];
	clas = ( answer[pp+2] << 8 ) + answer[pp+3];
	ttl = ( answer[pp+4] << 24 ) + ( answer[pp+5] << 16 ) +
	      ( answer[pp+6] <<  8 ) + answer[pp+7];
	pp = pp + 10;
	if ( clas != 1 ) {
#if defined(TQDNS_DEBUG)
	    tqDebug( "DNS Manager: class %d (not internet) for %s",
		    clas, label.isNull() ? "." : label.ascii() );
#endif
	} else {
            next = pp + rdlength;
	    rr = 0;
	    switch( type ) {
	    case 1:
		parseA();
		break;
	    case 28:
		parseAaaa();
		break;
	    case 15:
		parseMx();
		break;
	    case 33:
		parseSrv();
		break;
	    case 5:
		parseCname();
		break;
	    case 12:
		parsePtr();
		break;
	    case 16:
		parseTxt();
		break;
	    case 2:
		parseNs();
		break;
	    default:
		// something we don't know
#if defined(TQDNS_DEBUG)
		tqDebug( "DNS Manager: type %d for %s", type,
			label.isNull() ? "." : label.ascii() );
#endif
		break;
	    }
	    if ( rr ) {
		rr->deleteTime = 0;
		if ( ttl > 0 )
		    rr->expireTime = query->started + ttl;
		else
		    rr->expireTime = query->started + 20;
		if ( rrno < ancount ) {
		    answers++;
		    rr->deleteTime = rr->expireTime;
		}
		rr->current = true;
		rrs->append( rr );
	    }
        }
	if ( !ok )
	    return;
	pp = next;
	next = size;
	rrno++;
    }
    if ( answers == 0 ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "DNS Manager: answer contained no answers" );
#endif
	ok = ( aa && rd );
    }

    // now go through the list and mark all the As that are referenced
    // by something we care about.  we want to cache such As.
    rrs->first();
    TQDict<void> used( 17 );
    used.setAutoDelete( false );
    while( (rr=rrs->current()) != 0 ) {
	rrs->next();
	if ( rr->target.length() && rr->deleteTime > 0 && rr->current )
	    used.insert( rr->target, (void*)42 );
	if ( ( rr->t == TQDns::A || rr->t == TQDns::Aaaa ) &&
	     used.find( rr->domain->name() ) != 0 )
	    rr->deleteTime = rr->expireTime;
    }

    // next, for each RR, delete any older RRs that are equal to it
    rrs->first();
    while( (rr=rrs->current()) != 0 ) {
	rrs->next();
	if ( rr && rr->domain && rr->domain->rrs ) {
	    TQPtrList<TQDnsRR> * drrs = rr->domain->rrs;
	    drrs->first();
	    TQDnsRR * older;
	    while( (older=drrs->current()) != 0 ) {
		if ( older != rr &&
		     older->t == rr->t &&
		     older->nxdomain == rr->nxdomain &&
		     older->address == rr->address &&
		     older->target == rr->target &&
		     older->priority == rr->priority &&
		     older->weight == rr->weight &&
		     older->port == rr->port &&
		     older->text == rr->text ) {
		    // well, it's equal, but it's not the same. so we kill it,
		    // but use its expiry time.
#if defined(TQDNS_DEBUG)
		    tqDebug( "killing off old %d for %s, expire was %d",
                            older->t, older->domain->name().latin1(),
                            rr->expireTime );
#endif
		    older->t = TQDns::None;
		    rr->expireTime = TQMAX( older->expireTime, rr->expireTime );
		    rr->deleteTime = TQMAX( older->deleteTime, rr->deleteTime );
		    older->deleteTime = 0;
#if defined(TQDNS_DEBUG)
		    tqDebug( "    adjusted expire is %d", rr->expireTime );
#endif
		}
		drrs->next();
	    }
	}
    }

#if defined(TQDNS_DEBUG)
    //tqDebug( "DNS Manager: ()" );
#endif
}


class TQDnsUgleHack: public TQDns {
public:
    void ugle( bool emitAnyway=false );
};


void TQDnsAnswer::notify()
{
    if ( !rrs || !ok || !query || !query->dns )
	return;

    TQPtrDict<void> notified;
    notified.setAutoDelete( false );

    TQPtrDictIterator<void> it( *query->dns );
    TQDns * dns;
    it.toFirst();
    while( (dns=(TQDns*)(it.current())) != 0 ) {
	++it;
	if ( notified.find( (void*)dns ) == 0 ) {
	    notified.insert( (void*)dns, (void*)42 );
	    if ( rrs->count() == 0 ) {
#if defined(TQDNS_DEBUG)
		tqDebug( "DNS Manager: found no answers!" );
#endif
		dns->d->noNames = true;
		((TQDnsUgleHack*)dns)->ugle( true );
	    } else {
		TQStringList n = dns->qualifiedNames();
		if ( query && n.contains(query->l) )
		    ((TQDnsUgleHack*)dns)->ugle();
#if defined(TQDNS_DEBUG)
		else
		    tqDebug( "DNS Manager: DNS thing %s not notified for %s",
			    dns->label().ascii(), query->l.ascii() );
#endif
	    }
	}
    }
}


//
//
// TQDnsManager
//
//


class TQDnsManager: public TQDnsSocket {
private:
public: // just to silence the moronic g++.
    TQDnsManager();
    ~TQDnsManager();
public:
    static TQDnsManager * manager();

    TQDnsDomain * domain( const TQString & );

    void transmitQuery( TQDnsQuery * );
    void transmitQuery( int );

    // reimplementation of the slots
    void cleanCache();
    void retransmit();
    void answer();

public:
    TQPtrVector<TQDnsQuery> queries;
    TQDict<TQDnsDomain> cache;
    TQSocketDevice * ipv4Socket;
#if !defined (TQT_NO_IPV6)
    TQSocketDevice * ipv6Socket;
#endif
};



static TQDnsManager * globalManager = 0;

static void cleanupDns()
{
    delete globalManager;
    globalManager = 0;
}

TQDnsManager * TQDnsManager::manager()
{
    if ( !globalManager ) {
        tqAddPostRoutine(cleanupDns);
	new TQDnsManager();
    }
    return globalManager;
}


void TQDnsUgleHack::ugle( bool emitAnyway)
{
    if ( emitAnyway || !isWorking() ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "DNS Manager: status change for %s (type %d)",
		label().ascii(), recordType() );
#endif
	emit resultsReady();
    }
}


TQDnsManager::TQDnsManager()
    : TQDnsSocket( tqApp, "Internal DNS manager" ),
      queries( TQPtrVector<TQDnsQuery>( 0 ) ),
      cache( TQDict<TQDnsDomain>( 83, false ) ),
      ipv4Socket( new TQSocketDevice( TQSocketDevice::Datagram, TQSocketDevice::IPv4, 0 ) )
#if !defined (TQT_NO_IPV6)
      , ipv6Socket( new TQSocketDevice( TQSocketDevice::Datagram, TQSocketDevice::IPv6, 0 ) )
#endif
{
    cache.setAutoDelete( true );
    globalManager = this;

    TQTimer * sweepTimer = new TQTimer( this );
    sweepTimer->start( 1000 * 60 * 3 );
    connect( sweepTimer, TQ_SIGNAL(timeout()),
	     this, TQ_SLOT(cleanCache()) );

    TQSocketNotifier * rn4 = new TQSocketNotifier( ipv4Socket->socket(),
						 TQSocketNotifier::Read,
						 this, "dns IPv4 socket watcher" );
    ipv4Socket->setAddressReusable( false );
    ipv4Socket->setBlocking( false );
    connect( rn4, TQ_SIGNAL(activated(int)), TQ_SLOT(answer()) );

#if !defined (TQT_NO_IPV6)
    // Don't connect the IPv6 socket notifier if the host does not
    // support IPv6.
    if ( ipv6Socket->socket() != -1 ) {
	TQSocketNotifier * rn6 = new TQSocketNotifier( ipv6Socket->socket(),
						     TQSocketNotifier::Read,
						     this, "dns IPv6 socket watcher" );

	ipv6support = true;
	ipv6Socket->setAddressReusable( false );
	ipv6Socket->setBlocking( false );
	connect( rn6, TQ_SIGNAL(activated(int)), TQ_SLOT(answer()) );
    }
#endif

    if ( !ns )
	TQDns::doResInit();

    // O(n*n) stuff here.  but for 3 and 6, O(n*n) with a low k should
    // be perfect.  the point is to eliminate any duplicates that
    // might be hidden in the lists.
    TQPtrList<TQHostAddress> * ns = new TQPtrList<TQHostAddress>;

    ::ns->first();
    TQHostAddress * h;
    while( (h=::ns->current()) != 0 ) {
	ns->first();
	while( ns->current() != 0 && !(*ns->current() == *h) )
	    ns->next();
	if ( !ns->current() ) {
	    ns->append( new TQHostAddress(*h) );
#if defined(TQDNS_DEBUG)
	    tqDebug( "using name server %s", h->toString().latin1() );
	} else {
	    tqDebug( "skipping address %s", h->toString().latin1() );
#endif
	}
	::ns->next();
    }

    delete ::ns;
    ::ns = ns;
    ::ns->setAutoDelete( true );

    TQStrList * domains = new TQStrList( true );

    ::domains->first();
    const char * s;
    while( (s=::domains->current()) != 0 ) {
	domains->first();
	while( domains->current() != 0 && qstrcmp( domains->current(), s ) )
	    domains->next();
	if ( !domains->current() ) {
	    domains->append( s );
#if defined(TQDNS_DEBUG)
	    tqDebug( "searching domain %s", s );
	} else {
	    tqDebug( "skipping domain %s", s );
#endif
	}
	::domains->next();
    }

    delete ::domains;
    ::domains = domains;
    ::domains->setAutoDelete( true );
}


TQDnsManager::~TQDnsManager()
{
    if ( globalManager )
	globalManager = 0;
    queries.setAutoDelete( true );
    cache.setAutoDelete( true );
    delete ipv4Socket;
#if !defined (TQT_NO_IPV6)
    delete ipv6Socket;
#endif
}

static TQ_UINT32 lastSweep = 0;

void TQDnsManager::cleanCache()
{
    bool again = false;
    TQDictIterator<TQDnsDomain> it( cache );
    TQDnsDomain * d;
    TQ_UINT32 thisSweep = now();
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDnsManager::cleanCache(: Called, time is %u, last was %u",
	   thisSweep, lastSweep );
#endif

    while( (d=it.current()) != 0 ) {
	++it;
	d->sweep( thisSweep ); // after this, d may be empty
	if ( !again )
	    again = !d->isEmpty();
    }
    if ( !again )
	delete this;
    lastSweep = thisSweep;
}


void TQDnsManager::retransmit()
{
    const TQObject * o = sender();
    if ( o == 0 || globalManager == 0 || this != globalManager )
	return;
    uint q = 0;
    while( q < queries.size() && queries[q] != o )
	q++;
    if ( q < queries.size() )
	transmitQuery( q );
}


void TQDnsManager::answer()
{
    TQByteArray a( 16383 ); // large enough for anything, one suspects

    int r;
#if defined (TQT_NO_IPV6)
    r = ipv4Socket->readBlock(a.data(), a.size());
#else
    if (((TQSocketNotifier *)sender())->socket() == ipv4Socket->socket())
        r = ipv4Socket->readBlock(a.data(), a.size());
    else
        r = ipv6Socket->readBlock(a.data(), a.size());
#endif
#if defined(TQDNS_DEBUG)
#if !defined (TQT_NO_IPV6)
    tqDebug("DNS Manager: answer arrived: %d bytes from %s:%d", r,
	   useIpv4Socket ? ipv4Socket->peerAddress().toString().ascii()
	   : ipv6Socket->peerAddress().toString().ascii(),
	   useIpv4Socket ? ipv4Socket->peerPort() : ipv6Socket->peerPort() );
#else
    tqDebug("DNS Manager: answer arrived: %d bytes from %s:%d", r,
           ipv4Socket->peerAddress().toString().ascii(), ipv4Socket->peerPort());;
#endif
#endif
    if ( r < 12 )
	return;
    // maybe we should check that the answer comes from port 53 on one
    // of our name servers...
    a.resize( r );

    TQ_UINT16 aid = (((TQ_UINT8)a[0]) << 8) + ((TQ_UINT8)a[1]);
    uint i = 0;
    while( i < queries.size() &&
	   !( queries[i] && queries[i]->id == aid ) )
	i++;
    if ( i == queries.size() ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "DNS Manager: bad id (0x%04x) %d", aid, i );
#endif
	return;
    }

    // at this point queries[i] is whatever we asked for.

    if ( ( (TQ_UINT8)(a[2]) & 0x80 ) == 0 ) {
#if defined(TQDNS_DEBUG)
	tqDebug( "DNS Manager: received a query" );
#endif
	return;
    }

    TQDnsQuery * q = queries[i];
    TQDnsAnswer answer( a, q );
    answer.parse();
    if ( answer.ok ) {
	queries.take( i );
	answer.notify();
	delete q;
    }
}


void TQDnsManager::transmitQuery( TQDnsQuery * query_ )
{
    if ( !query_ )
	return;

    uint i = 0;
    while( i < queries.size() && queries[i] != 0 )
	i++;
    if ( i == queries.size() )
	queries.resize( i+1 );
    queries.insert( i, query_ );
    transmitQuery( i );
}


void TQDnsManager::transmitQuery( int i )
{
    if ( i < 0 || i >= (int)queries.size() )
	return;
    TQDnsQuery * q = queries[i];

    if ( q && q->step > 8 ) {
	// okay, we've run out of retransmissions. we fake an NXDomain
	// with a very short life time...
	TQDnsAnswer answer( q );
	answer.notify();

        if (globalManager == 0)
            return;

	// and then get rid of the query
	queries.take( i );
#if defined(TQDNS_DEBUG)
	tqDebug( "DNS Manager: giving up on query 0x%04x", q->id );
#endif
	delete q;
	TQTimer::singleShot( 0, TQDnsManager::manager(), TQ_SLOT(cleanCache()) );
	// and don't process anything more
	return;
    }

    if ( ( q && !q->dns ) || q->dns->isEmpty() )
	// noone currently wants the answer, so there's no point in
	// retransmitting the query. we keep it, though. an answer may
	// arrive for an earlier query transmission, and if it does we
	// may benefit from caching the result.
	return;

    TQByteArray p( 12 + q->l.length() + 2 + 4 );
    if ( p.size() > 500 )
	return; // way over the limit, so don't even try

    // header
    // id
    p[0] = (q->id & 0xff00) >> 8;
    p[1] =  q->id & 0x00ff;
    p[2] = 1; // recursion desired, rest is 0
    p[3] = 0; // all is 0
    // one query
    p[4] = 0;
    p[5] = 1;
    // no answers, name servers or additional data
    p[6] = p[7] = p[8] = p[9] = p[10] = p[11] = 0;

    // the name is composed of several components.  each needs to be
    // written by itself... so we write...
    // oh, and we assume that there's no funky characters in there.
    int pp = 12;
    uint lp = 0;
    while( lp < q->l.length() ) {
	int le = q->l.find( '.', lp );
	if ( le < 0 )
	    le = q->l.length();
	TQString component = q->l.mid( lp, le-lp );
	p[pp++] = component.length();
	int cp;
	for( cp=0; cp < (int)component.length(); cp++ )
	    p[pp++] = component[cp].latin1();
	lp = le + 1;
    }
    // final null
    p[pp++] = 0;
    // query type
    p[pp++] = 0;
    switch( q->t ) {
    case TQDns::A:
	p[pp++] = 1;
	break;
    case TQDns::Aaaa:
	p[pp++] = 28;
	break;
    case TQDns::Mx:
	p[pp++] = 15;
	break;
    case TQDns::Srv:
	p[pp++] = 33;
	break;
    case TQDns::Cname:
	p[pp++] = 5;
	break;
    case TQDns::Ptr:
	p[pp++] = 12;
	break;
    case TQDns::Txt:
	p[pp++] = 16;
	break;
    default:
	p[pp++] = (char)255; // any
	break;
    }
    // query class (always internet)
    p[pp++] = 0;
    p[pp++] = 1;

    // if we have no name servers, we should regenerate ns in case
    // name servers have recently been defined (like on windows,
    // plugging/unplugging the network cable will change the name
    // server entries)
    if ( !ns || ns->isEmpty() )
        TQDns::doResInit();

    if ( !ns || ns->isEmpty() ) {
	// we don't find any name servers. We fake an NXDomain
	// with a very short life time...
	TQDnsAnswer answer( q );
	answer.notify();
	// and then get rid of the query
	queries.take( i );
#if defined(TQDNS_DEBUG)
	tqDebug( "DNS Manager: no DNS server found on query 0x%04x", q->id );
#endif
	delete q;
	TQTimer::singleShot( 1000*10, TQDnsManager::manager(), TQ_SLOT(cleanCache()) );
	// and don't process anything more
	return;
    }

    TQHostAddress receiver = *ns->at( q->step % ns->count() );
    if (receiver.isIPv4Address())
	ipv4Socket->writeBlock( p.data(), pp, receiver, 53 );
#if !defined (TQT_NO_IPV6)
    else
	ipv6Socket->writeBlock( p.data(), pp, receiver, 53 );
#endif
#if defined(TQDNS_DEBUG)
    tqDebug( "issuing query 0x%04x (%d) about %s type %d to %s",
	    q->id, q->step, q->l.ascii(), q->t,
	    ns->at( q->step % ns->count() )->toString().ascii() );
#endif
    if ( ns->count() > 1 && q->step == 0 && queries.count() == 1 ) {
	// if it's the first time, and we don't have any other
	// outstanding queries, send nonrecursive queries to the other
	// name servers too.
	p[2] = 0;
	TQHostAddress * server;
	while( (server=ns->next()) != 0 ) {
	    if (server->isIPv4Address())
		ipv4Socket->writeBlock( p.data(), pp, *server, 53 );
#if !defined (TQT_NO_IPV6)
	    else
		ipv6Socket->writeBlock( p.data(), pp, *server, 53 );
#endif
#if defined(TQDNS_DEBUG)
	    tqDebug( "copying query to %s", server->toString().ascii() );
#endif
	}
    }
    q->step++;
    // some testing indicates that normal dns queries take up to 0.6
    // seconds.  the graph becomes steep around that point, and the
    // number of errors rises... so it seems good to retry at that
    // point.
    q->start( q->step < ns->count() ? 800 : 1500, true );
}


TQDnsDomain * TQDnsManager::domain( const TQString & label )
{
    TQDnsDomain * d = cache.find( label );
    if ( !d ) {
	d = new TQDnsDomain( label );
	cache.insert( label, d );
    }
    return d;
}


//
//
// the TQDnsDomain class looks after and coordinates queries for TQDnsRRs for
// each domain, and the cached TQDnsRRs.  (A domain, in DNS terminology, is
// a node in the DNS.  "no", "trolltech.com" and "lupinella.troll.no" are
// all domains.)
//
//


// this is ONLY to be called by TQDnsManager::domain().  noone else.
TQDnsDomain::TQDnsDomain( const TQString & label )
{
    l = label;
    rrs = 0;
}


TQDnsDomain::~TQDnsDomain()
{
    delete rrs;
    rrs = 0;
}


void TQDnsDomain::add( const TQString & label, TQDnsRR * rr )
{
    TQDnsDomain * d = TQDnsManager::manager()->domain( label );
    if ( !d->rrs ) {
	d->rrs = new TQPtrList<TQDnsRR>;
	d->rrs->setAutoDelete( true );
    }
    d->rrs->append( rr );
    rr->domain = d;
}


TQPtrList<TQDnsRR> * TQDnsDomain::cached( const TQDns * r )
{
    TQPtrList<TQDnsRR> * l = new TQPtrList<TQDnsRR>;

    // test at first if you have to start a query at all
    if ( r->recordType() == TQDns::A ) {
	if ( r->label().lower() == "localhost" ) {
	    // undocumented hack. ipv4-specific. also, may be a memory
	    // leak? not sure. would be better to do this in doResInit(),
	    // anyway.
	    TQDnsRR *rrTmp = new TQDnsRR( r->label() );
	    rrTmp->t = TQDns::A;
	    rrTmp->address = TQHostAddress( 0x7f000001 );
	    rrTmp->current = true;
	    l->append( rrTmp );
	    return l;
	}
	TQHostAddress tmp;
	if ( tmp.setAddress( r->label() ) ) {
	    TQDnsRR *rrTmp = new TQDnsRR( r->label() );
	    if ( tmp.isIPv4Address() ) {
		rrTmp->t = TQDns::A;
                rrTmp->address = tmp;
                rrTmp->current = true;
                l->append( rrTmp );
            } else {
                rrTmp->nxdomain = true;
            }
	    return l;
	}
    }
    if ( r->recordType() == TQDns::Aaaa ) {
	TQHostAddress tmp;
	if ( tmp.setAddress(r->label()) ) {
	    TQDnsRR *rrTmp = new TQDnsRR( r->label() );
	    if ( tmp.isIPv6Address() ) {
		rrTmp->t = TQDns::Aaaa;
                rrTmp->address = tmp;
                rrTmp->current = true;
                l->append( rrTmp );
            } else {
                rrTmp->nxdomain = true;
            }
	    return l;
	}
    }

    // if you reach this point, you have to do the query
    TQDnsManager * m = TQDnsManager::manager();
    TQStringList n = r->qualifiedNames();
    TQValueListIterator<TQString> it = n.begin();
    TQValueListIterator<TQString> end = n.end();
    bool nxdomain;
    int cnamecount = 0;
    while( it != end ) {
	TQString s = *it++;
	nxdomain = false;
#if defined(TQDNS_DEBUG)
	tqDebug( "looking at cache for %s (%s %d)",
		s.ascii(), r->label().ascii(), r->recordType() );
#endif
	TQDnsDomain * d = m->domain( s );
#if defined(TQDNS_DEBUG)
	tqDebug( " - found %d RRs", d && d->rrs ? d->rrs->count() : 0 );
#endif
	if ( d->rrs )
	    d->rrs->first();
	TQDnsRR * rr;
	bool answer = false;
	while( d->rrs && (rr=d->rrs->current()) != 0 ) {
	    if ( rr->t == TQDns::Cname && r->recordType() != TQDns::Cname &&
		 !rr->nxdomain && cnamecount < 16 ) {
		// cname.  if the code is ugly, that may just
		// possibly be because the concept is.
#if defined(TQDNS_DEBUG)
		tqDebug( "found cname from %s to %s",
			r->label().ascii(), rr->target.ascii() );
#endif
		s = rr->target;
		d = m->domain( s );
		if ( d->rrs )
		    d->rrs->first();
		it = end;
		// we've elegantly moved over to whatever the cname
		// pointed to.  well, not elegantly.  let's remember
		// that we've done something, anyway, so we can't be
		// fooled into an infinte loop as well.
		cnamecount++;
	    } else {
		if ( rr->t == r->recordType() ) {
		    if ( rr->nxdomain )
			nxdomain = true;
		    else
			answer = true;
		    l->append( rr );
		    if ( rr->deleteTime <= lastSweep ) {
			// we're returning something that'll be
			// deleted soon.  we assume that if the client
			// wanted it twice, it'll want it again, so we
			// ask the name server again right now.
			TQDnsQuery * query = new TQDnsQuery;
			query->started = now();
			query->id = ++::id;
			query->t = rr->t;
			query->l = rr->domain->name();
			// note that here, we don't bother about
			// notification. but we do bother about
			// timeouts: we make sure to use high timeouts
			// and few tramsissions.
			query->step = ns->count();
			TQObject::connect( query, TQ_SIGNAL(timeout()),
					  TQDnsManager::manager(),
					  TQ_SLOT(retransmit()) );
			TQDnsManager::manager()->transmitQuery( query );
		    }
		}
		d->rrs->next();
	    }
	}
	// if we found a positive result, return quickly
	if ( answer && l->count() ) {
#if defined(TQDNS_DEBUG)
	    tqDebug( "found %d records for %s",
		    l->count(), r->label().ascii() );
	    l->first();
	    while( l->current() ) {
		tqDebug( "  type %d target %s address %s",
		       l->current()->t,
		       l->current()->target.latin1(),
		       l->current()->address.toString().latin1() );
		l->next();
	    }
#endif
	    l->first();
	    return l;
	}

#if defined(TQDNS_DEBUG)
	if ( nxdomain )
	    tqDebug( "found NXDomain %s", s.ascii() );
#endif

	if ( !nxdomain ) {
	    // if we didn't, and not a negative result either, perhaps
	    // we need to transmit a query.
	    uint q = 0;
	    while ( q < m->queries.size() &&
		    ( m->queries[q] == 0 ||
		      m->queries[q]->t != r->recordType() ||
		      m->queries[q]->l != s ) )
		q++;
	    // we haven't done it before, so maybe we should.  but
	    // wait - if it's an unqualified name, only ask when all
	    // the other alternatives are exhausted.
	    if ( q == m->queries.size() && ( s.find( '.' ) >= 0 ||
					     l->count() >= n.count()-1 ) ) {
		TQDnsQuery * query = new TQDnsQuery;
		query->started = now();
		query->id = ++::id;
		query->t = r->recordType();
		query->l = s;
		query->dns->replace( (void*)r, (void*)r );
		TQObject::connect( query, TQ_SIGNAL(timeout()),
				  TQDnsManager::manager(), TQ_SLOT(retransmit()) );
		TQDnsManager::manager()->transmitQuery( query );
	    } else if ( q < m->queries.size() ) {
		// if we've found an earlier query for the same
		// domain/type, subscribe to its answer
		m->queries[q]->dns->replace( (void*)r, (void*)r );
	    }
	}
    }
    l->first();
    return l;
}


void TQDnsDomain::sweep( TQ_UINT32 thisSweep )
{
    if ( !rrs )
	return;

    TQDnsRR * rr;
    rrs->first();
    while( (rr=rrs->current()) != 0 ) {
	if ( !rr->deleteTime )
	    rr->deleteTime = thisSweep; // will hit next time around

#if defined(TQDNS_DEBUG)
	tqDebug( "TQDns::sweep: %s type %d expires %u %u - %s / %s",
	       rr->domain->name().latin1(), rr->t,
	       rr->expireTime, rr->deleteTime,
	       rr->target.latin1(), rr->address.toString().latin1());
#endif
	if ( !rr->current ||
	     rr->t == TQDns::None ||
	     rr->deleteTime <= thisSweep ||
	     rr->expireTime <= thisSweep )
	    rrs->remove();
	else
	    rrs->next();
    }

    if ( rrs->isEmpty() ) {
	delete rrs;
	rrs = 0;
    }
}




// the itsy-bitsy little socket class I don't really need except for
// so I can subclass and reimplement the slots.


TQDnsSocket::TQDnsSocket( TQObject * parent, const char * name )
    : TQObject( parent, name )
{
    // nothing
}


TQDnsSocket::~TQDnsSocket()
{
    // nothing
}


void TQDnsSocket::cleanCache()
{
    // nothing
}


void TQDnsSocket::retransmit()
{
    // nothing
}


void TQDnsSocket::answer()
{
    // nothing
}


/*!
    \class TQDns ntqdns.h
    \brief The TQDns class provides asynchronous DNS lookups.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \module network
    \ingroup io

    Both Windows and Unix provide synchronous DNS lookups; Windows
    provides some asynchronous support too. At the time of writing
    neither operating system provides asynchronous support for
    anything other than hostname-to-address mapping.

    TQDns rectifies this shortcoming, by providing asynchronous caching
    lookups for the record types that we expect modern GUI
    applications to need in the near future.

    The class is \e not straightforward to use (although it is much
    simpler than the native APIs); TQSocket provides much easier to use
    TCP connection facilities. The aim of TQDns is to provide a correct
    and small API to the DNS and nothing more. (We use "correctness"
    to mean that the DNS information is correctly cached, and
    correctly timed out.)

    The API comprises a constructor, functions to set the DNS node
    (the domain in DNS terminology) and record type (setLabel() and
    setRecordType()), the corresponding get functions, an isWorking()
    function to determine whether TQDns is working or reading, a
    resultsReady() signal and query functions for the result.

    There is one query function for each RecordType, namely
    addresses(), mailServers(), servers(), hostNames() and texts().
    There are also two generic query functions: canonicalName()
    returns the name you'll presumably end up using (the exact meaning
    of this depends on the record type) and qualifiedNames() returns a
    list of the fully qualified names label() maps to.

    \sa TQSocket
*/

/*!
    Constructs a DNS query object with invalid settings for both the
    label and the search type.
*/

TQDns::TQDns()
{
    d = new TQDnsPrivate;
    t = None;
}




/*!
    Constructs a DNS query object that will return record type \a rr
    information about \a label.

    The DNS lookup is started the next time the application enters the
    event loop. When the result is found the signal resultsReady() is
    emitted.

    \a rr defaults to \c A, IPv4 addresses.
*/

TQDns::TQDns( const TQString & label, RecordType rr )
{
    d = new TQDnsPrivate;
    t = rr;
    setLabel( label );
    setStartQueryTimer(); // start query the next time we enter event loop
}



/*!
    Constructs a DNS query object that will return record type \a rr
    information about host address \a address. The label is set to the
    IN-ADDR.ARPA domain name. This is useful in combination with the
    \c Ptr record type (e.g. if you want to look up a hostname for a
    given address).

    The DNS lookup is started the next time the application enters the
    event loop. When the result is found the signal resultsReady() is
    emitted.

    \a rr defaults to \c Ptr, that maps addresses to hostnames.
*/

TQDns::TQDns( const TQHostAddress & address, RecordType rr )
{
    d = new TQDnsPrivate;
    t = rr;
    setLabel( address );
    setStartQueryTimer(); // start query the next time we enter event loop
}




/*!
    Destroys the DNS query object and frees its allocated resources.
*/

TQDns::~TQDns()
{
    if ( globalManager ) {
	uint q = 0;
	TQDnsManager * m = globalManager;
	while( q < m->queries.size() ) {
	    TQDnsQuery * query=m->queries[q];
	    if ( query && query->dns )
		    (void)query->dns->take( (void*) this );
		q++;
	}

    }

    delete d;
    d = 0;
}




/*!
    Sets this DNS query object to query for information about \a
    label.

    This does not change the recordType(), but its isWorking() status
    will probably change as a result.

    The DNS lookup is started the next time the application enters the
    event loop. When the result is found the signal resultsReady() is
    emitted.
*/

void TQDns::setLabel( const TQString & label )
{
    l = label;
    d->noNames = false;

    // construct a list of qualified names
    n.clear();
    if ( l.length() > 1 && l[(int)l.length()-1] == '.' ) {
	n.append( l.left( l.length()-1 ).lower() );
    } else {
	int i = l.length();
	int dots = 0;
	const int maxDots = 2;
	while( i && dots < maxDots ) {
	    if ( l[--i] == '.' )
		dots++;
	}
	if ( dots < maxDots ) {
	    (void)TQDnsManager::manager(); // create a TQDnsManager, if it is not already there
	    TQStrListIterator it( *domains );
	    const char * dom;
	    while( (dom=it.current()) != 0 ) {
		++it;
		n.append( l.lower() + "." + dom );
	    }
	}
	n.append( l.lower() );
    }

#if defined(Q_DNS_SYNCHRONOUS)
    if ( d->noEventLoop ) {
	doSynchronousLookup();
    } else {
	setStartQueryTimer(); // start query the next time we enter event loop
    }
#else
    setStartQueryTimer(); // start query the next time we enter event loop
#endif
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns::setLabel: %d address(es) for %s", n.count(), l.ascii() );
    int i = 0;
    for( i = 0; i < (int)n.count(); i++ )
	tqDebug( "TQDns::setLabel: %d: %s", i, n[i].ascii() );
#endif
}


/*!
    \overload

    Sets this DNS query object to query for information about the host
    address \a address. The label is set to the IN-ADDR.ARPA domain
    name. This is useful in combination with the \c Ptr record type
    (e.g. if you want to look up a hostname for a given address).
*/

void TQDns::setLabel( const TQHostAddress & address )
{
    setLabel( toInAddrArpaDomain( address ) );
}


/*!
    \fn TQStringList TQDns::qualifiedNames() const

    Returns a list of the fully qualified names label() maps to.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myDns.qualifiedNames();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

*/


/*!
    \fn TQString TQDns::label() const

    Returns the domain name for which this object returns information.

    \sa setLabel()
*/

/*!
    \enum TQDns::RecordType

    This enum type defines the record types TQDns can handle. The DNS
    provides many more; these are the ones we've judged to be in
    current use, useful for GUI programs and important enough to
    support right away:

    \value None  No information. This exists only so that TQDns can
    have a default.

    \value A  IPv4 addresses. By far the most common type.

    \value Aaaa  IPv6 addresses. So far mostly unused.

    \value Mx  Mail eXchanger names. Used for mail delivery.

    \value Srv  SeRVer names. Generic record type for finding
    servers. So far mostly unused.

    \value Cname  Canonical names. Maps from nicknames to the true
    name (the canonical name) for a host.

    \value Ptr  name PoinTeRs. Maps from IPv4 or IPv6 addresses to hostnames.

    \value Txt  arbitrary TeXT for domains.

    We expect that some support for the
    \link http://www.dns.net/dnsrd/rfc/rfc2535.html RFC-2535 \endlink
    extensions will be added in future versions.
*/

/*!
    Sets this object to query for record type \a rr records.

    The DNS lookup is started the next time the application enters the
    event loop. When the result is found the signal resultsReady() is
    emitted.

    \sa RecordType
*/

void TQDns::setRecordType( RecordType rr )
{
    t = rr;
    d->noNames = false;
    setStartQueryTimer(); // start query the next time we enter event loop
}

/*!
  \internal

  Private slot for starting the query.
*/
void TQDns::startQuery()
{
    // isWorking() starts the query (if necessary)
    if ( !isWorking() )
	emit resultsReady();
}

/*!
    The three functions TQDns::TQDns(TQString, RecordType),
    TQDns::setLabel() and TQDns::setRecordType() may start a DNS lookup.
    This function handles setting up the single shot timer.
*/
void TQDns::setStartQueryTimer()
{
#if defined(Q_DNS_SYNCHRONOUS)
    if ( !d->queryTimer && !d->noEventLoop )
#else
    if ( !d->queryTimer )
#endif
    {
	// start the query the next time we enter event loop
	d->queryTimer = new TQTimer( this );
	connect( d->queryTimer, TQ_SIGNAL(timeout()),
		 this, TQ_SLOT(startQuery()) );
	d->queryTimer->start( 0, true );
    }
}

/*
    Transforms the host address \a address to the IN-ADDR.ARPA domain
    name. Returns something indeterminate if you're sloppy or
    naughty. This function has an IPv4-specific name, but works for
    IPv6 too.
*/
TQString TQDns::toInAddrArpaDomain( const TQHostAddress &address )
{
    TQString s;
    if ( address.isNull() ) {
	// if the address isn't valid, neither of the other two make
	// cases make sense. better to just return.
    } else if ( address.isIp4Addr() ) {
	TQ_UINT32 i = address.ip4Addr();
	s.sprintf( "%d.%d.%d.%d.IN-ADDR.ARPA",
		   i & 0xff, (i >> 8) & 0xff, (i>>16) & 0xff, (i>>24) & 0xff );
    } else {
	// RFC 3152. (1886 is deprecated, and clients no longer need to
	// support it, in practice).
	Q_IPV6ADDR i = address.toIPv6Address();
	s = "ip6.arpa";
	uint b = 0;
	while( b < 16 ) {
	    s = TQString::number( i.c[b]%16, 16 ) + "." +
		TQString::number( i.c[b]/16, 16 ) + "." + s;
	    b++;
	}
    }
    return s;
}


/*!
    \fn TQDns::RecordType TQDns::recordType() const

    Returns the record type of this DNS query object.

    \sa setRecordType() RecordType
*/

/*!
    \fn void TQDns::resultsReady()

    This signal is emitted when results are available for one of the
    qualifiedNames().
*/

/*!
    Returns true if TQDns is doing a lookup for this object (i.e. if it
    does not already have the necessary information); otherwise
    returns false.

    TQDns emits the resultsReady() signal when the status changes to false.
*/

bool TQDns::isWorking() const
{
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns::isWorking (%s, %d)", l.ascii(), t );
#endif
    if ( t == None )
	return false;

#if defined(Q_DNS_SYNCHRONOUS)
    if ( d->noEventLoop )
	return true;
#endif

    TQPtrList<TQDnsRR> * ll = TQDnsDomain::cached( this );
    TQ_LONG queries = n.count();
    while( ll->current() != 0 ) {
	if ( ll->current()->nxdomain ) {
	    queries--;
	} else {
	    delete ll;
	    return false;
	}
	ll->next();
    }
    delete ll;

    if ( queries <= 0 )
	return false;
    if ( d->noNames )
	return false;
    return true;
}


/*!
    Returns a list of the addresses for this name if this TQDns object
    has a recordType() of \c TQDns::A or \c TQDns::Aaaa and the answer
    is available; otherwise returns an empty list.

    As a special case, if label() is a valid numeric IP address, this
    function returns that address.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQValueList<TQHostAddress> list = myDns.addresses();
    TQValueList<TQHostAddress>::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

*/

TQValueList<TQHostAddress> TQDns::addresses() const
{
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns::addresses (%s)", l.ascii() );
#endif
    TQValueList<TQHostAddress> result;
    if ( t != A && t != Aaaa )
	return result;

    TQPtrList<TQDnsRR> * cached = TQDnsDomain::cached( this );

    TQDnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain )
	    result.append( rr->address );
	cached->next();
    }
    delete cached;
    return result;
}


/*!
    \class TQDns::MailServer
    \brief The TQDns::MailServer class is  described in TQDns::mailServers().
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup io

    \internal
*/

/*!
    Returns a list of mail servers if the record type is \c Mx. The
    class \c TQDns::MailServer contains the following public variables:
    \list
    \i TQString TQDns::MailServer::name
    \i TQ_UINT16 TQDns::MailServer::priority
    \endlist

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQValueList<TQDns::MailServer> list = myDns.mailServers();
    TQValueList<TQDns::MailServer>::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

*/
TQValueList<TQDns::MailServer> TQDns::mailServers() const
{
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns::mailServers (%s)", l.ascii() );
#endif
    TQValueList<TQDns::MailServer> result;
    if ( t != Mx )
	return result;

    TQPtrList<TQDnsRR> * cached = TQDnsDomain::cached( this );

    TQDnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain ) {
	    MailServer ms( rr->target, rr->priority );
	    result.append( ms );
	}
	cached->next();
    }
    delete cached;
    return result;
}


/*!
    \class TQDns::Server
    \brief The TQDns::Server class is described in TQDns::servers().
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup io

    \internal
*/

/*!
    Returns a list of servers if the record type is \c Srv. The class
    \c TQDns::Server contains the following public variables:
    \list
    \i TQString TQDns::Server::name
    \i TQ_UINT16 TQDns::Server::priority
    \i TQ_UINT16 TQDns::Server::weight
    \i TQ_UINT16 TQDns::Server::port
    \endlist

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQValueList<TQDns::Server> list = myDns.servers();
    TQValueList<TQDns::Server>::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode
*/
TQValueList<TQDns::Server> TQDns::servers() const
{
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns::servers (%s)", l.ascii() );
#endif
    TQValueList<TQDns::Server> result;
    if ( t != Srv )
	return result;

    TQPtrList<TQDnsRR> * cached = TQDnsDomain::cached( this );

    TQDnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain ) {
	    Server s( rr->target, rr->priority, rr->weight, rr->port );
	    result.append( s );
	}
	cached->next();
    }
    delete cached;
    return result;
}


/*!
    Returns a list of host names if the record type is \c Ptr.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myDns.hostNames();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

*/
TQStringList TQDns::hostNames() const
{
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns::hostNames (%s)", l.ascii() );
#endif
    TQStringList result;
    if ( t != Ptr )
	return result;

    TQPtrList<TQDnsRR> * cached = TQDnsDomain::cached( this );

    TQDnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain ) {
	    TQString str( rr->target );
	    result.append( str );
	}
	cached->next();
    }
    delete cached;
    return result;
}


/*!
    Returns a list of texts if the record type is \c Txt.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myDns.texts();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode
*/
TQStringList TQDns::texts() const
{
#if defined(TQDNS_DEBUG)
    tqDebug( "TQDns::texts (%s)", l.ascii() );
#endif
    TQStringList result;
    if ( t != Txt )
	return result;

    TQPtrList<TQDnsRR> * cached = TQDnsDomain::cached( this );

    TQDnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain ) {
	    TQString str( rr->text );
	    result.append( str );
	}
	cached->next();
    }
    delete cached;
    return result;
}


/*!
    Returns the canonical name for this DNS node. (This works
    regardless of what recordType() is set to.)

    If the canonical name isn't known, this function returns a null
    string.

    The canonical name of a DNS node is its full name, or the full
    name of the target of its CNAME. For example, if l.trolltech.com
    is a CNAME to lillian.troll.no, and the search path for TQDns is
    "trolltech.com", then the canonical name for all of "lillian",
    "l", "lillian.troll.no." and "l.trolltech.com" is
    "lillian.troll.no.".
*/

TQString TQDns::canonicalName() const
{
    // the cname should work regardless of the recordType(), so set the record
    // type temporarily to cname when you look at the cache
    TQDns *that = (TQDns*) this; // mutable function
    RecordType oldType = t;
    that->t = Cname;
    TQPtrList<TQDnsRR> * cached = TQDnsDomain::cached( that );
    that->t = oldType;

    TQDnsRR * rr;
    while( (rr=cached->current()) != 0 ) {
	if ( rr->current && !rr->nxdomain && rr->domain ) {
	    delete cached;
	    return rr->target;
	}
	cached->next();
    }
    delete cached;
    return TQString::null;
}

#if defined(Q_DNS_SYNCHRONOUS)
/*! \reimp
*/
void TQDns::connectNotify( const char *signal )
{
    if ( d->noEventLoop && qstrcmp(signal,TQ_SIGNAL(resultsReady()) )==0 ) {
	doSynchronousLookup();
    }
}
#endif

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)

#if defined(Q_DNS_SYNCHRONOUS)
void TQDns::doSynchronousLookup()
{
    // ### not implemented yet
}
#endif

// the following typedefs are needed for GetNetworkParams() API call
#ifndef IP_TYPES_INCLUDED
#define MAX_HOSTNAME_LEN    128
#define MAX_DOMAIN_NAME_LEN 128
#define MAX_SCOPE_ID_LEN    256
typedef struct {
    char String[4 * 4];
} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;
typedef struct _IP_ADDR_STRING {
    struct _IP_ADDR_STRING* Next;
    IP_ADDRESS_STRING IpAddress;
    IP_MASK_STRING IpMask;
    DWORD Context;
} IP_ADDR_STRING, *PIP_ADDR_STRING;
typedef struct {
    char HostName[MAX_HOSTNAME_LEN + 4] ;
    char DomainName[MAX_DOMAIN_NAME_LEN + 4];
    PIP_ADDR_STRING CurrentDnsServer;
    IP_ADDR_STRING DnsServerList;
    UINT NodeType;
    char ScopeId[MAX_SCOPE_ID_LEN + 4];
    UINT EnableRouting;
    UINT EnableProxy;
    UINT EnableDns;
} FIXED_INFO, *PFIXED_INFO;
#endif
typedef DWORD (WINAPI *GNP)( PFIXED_INFO, PULONG );

// ### FIXME: this code is duplicated in qfiledialog.cpp
static TQString getWindowsRegString( HKEY key, const TQString &subKey )
{
    TQString s;
    QT_WA( {
	char buf[1024];
	DWORD bsz = sizeof(buf);
	int r = RegQueryValueEx( key, (TCHAR*)subKey.ucs2(), 0, 0, (LPBYTE)buf, &bsz );
	if ( r == ERROR_SUCCESS ) {
	    s = TQString::fromUcs2( (unsigned short *)buf );
	} else if ( r == ERROR_MORE_DATA ) {
	    char *ptr = new char[bsz+1];
	    r = RegQueryValueEx( key, (TCHAR*)subKey.ucs2(), 0, 0, (LPBYTE)ptr, &bsz );
	    if ( r == ERROR_SUCCESS )
		s = ptr;
	    delete [] ptr;
	}
    } , {
	char buf[512];
	DWORD bsz = sizeof(buf);
	int r = RegQueryValueExA( key, subKey.local8Bit(), 0, 0, (LPBYTE)buf, &bsz );
	if ( r == ERROR_SUCCESS ) {
	    s = buf;
	} else if ( r == ERROR_MORE_DATA ) {
	    char *ptr = new char[bsz+1];
	    r = RegQueryValueExA( key, subKey.local8Bit(), 0, 0, (LPBYTE)ptr, &bsz );
	    if ( r == ERROR_SUCCESS )
		s = ptr;
	    delete [] ptr;
	}
    } );
    return s;
}

static bool getDnsParamsFromRegistry( const TQString &path,
	TQString *domainName, TQString *nameServer, TQString *searchList )
{
    HKEY k;
    int r;
    QT_WA( {
	r = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
			  (TCHAR*)path.ucs2(),
			  0, KEY_READ, &k );
    } , {
	r = RegOpenKeyExA( HKEY_LOCAL_MACHINE,
			   path,
			   0, KEY_READ, &k );
    } );

    if ( r == ERROR_SUCCESS ) {
	*domainName = getWindowsRegString( k, "DhcpDomain" );
	if ( domainName->isEmpty() )
	    *domainName = getWindowsRegString( k, "Domain" );

	*nameServer = getWindowsRegString( k, "DhcpNameServer" );
	if ( nameServer->isEmpty() )
	    *nameServer = getWindowsRegString( k, "NameServer" );

	*searchList = getWindowsRegString( k, "SearchList" );
    }
    RegCloseKey( k );
    return r == ERROR_SUCCESS;
}

void TQDns::doResInit()
{
    char separator = 0;

    if ( ns )
        delete ns;
    ns = new TQPtrList<TQHostAddress>;
    ns->setAutoDelete( true );
    domains = new TQStrList( true );
    domains->setAutoDelete( true );

    TQString domainName, nameServer, searchList;

    bool gotNetworkParams = false;
    // try the API call GetNetworkParams() first and use registry lookup only
    // as a fallback
#ifdef Q_OS_TEMP
    HINSTANCE hinstLib = LoadLibraryW( L"iphlpapi" );
#else
    HINSTANCE hinstLib = LoadLibraryA( "iphlpapi" );
#endif
    if ( hinstLib != 0 ) {
#ifdef Q_OS_TEMP
	GNP getNetworkParams = (GNP) GetProcAddressW( hinstLib, L"GetNetworkParams" );
#else
	GNP getNetworkParams = (GNP) GetProcAddress( hinstLib, "GetNetworkParams" );
#endif
	if ( getNetworkParams != 0 ) {
	    ULONG l = 0;
	    DWORD res;
	    res = getNetworkParams( 0, &l );
	    if ( res == ERROR_BUFFER_OVERFLOW ) {
		FIXED_INFO *finfo = (FIXED_INFO*)new char[l];
		res = getNetworkParams( finfo, &l );
		if ( res == ERROR_SUCCESS ) {
		    domainName = finfo->DomainName;
		    nameServer = "";
		    IP_ADDR_STRING *dnsServer = &finfo->DnsServerList;
		    while ( dnsServer != 0 ) {
			nameServer += dnsServer->IpAddress.String;
			dnsServer = dnsServer->Next;
			if ( dnsServer != 0 )
			    nameServer += " ";
		    }
		    searchList = "";
		    separator = ' ';
		    gotNetworkParams = true;
		}
		delete[] finfo;
	    }
	}
	FreeLibrary( hinstLib );
    }
    if ( !gotNetworkParams ) {
	if ( getDnsParamsFromRegistry(
	    TQString( "System\\CurrentControlSet\\Services\\Tcpip\\Parameters" ),
		    &domainName, &nameServer, &searchList )) {
	    // for NT
	    separator = ' ';
	} else if ( getDnsParamsFromRegistry(
	    TQString( "System\\CurrentControlSet\\Services\\VxD\\MSTCP" ),
		    &domainName, &nameServer, &searchList )) {
	    // for 95/98
	    separator = ',';
	} else {
	    // Could not access the TCP/IP parameters
	    domainName = "";
	    nameServer = "127.0.0.1";
	    searchList = "";
	    separator = ' ';
	}
    }

    nameServer = nameServer.simplifyWhiteSpace();
    int first, last;
    if ( !nameServer.isEmpty() ) {
	first = 0;
	do {
	    last = nameServer.find( separator, first );
	    if ( last < 0 )
		last = nameServer.length();
	    TQDns tmp( nameServer.mid( first, last-first ), TQDns::A );
	    TQValueList<TQHostAddress> address = tmp.addresses();
	    TQ_LONG i = address.count();
	    while( i )
		ns->append( new TQHostAddress(address[--i]) );
	    first = last+1;
	} while( first < (int)nameServer.length() );
    }

    searchList = searchList + " " + domainName;
    searchList = searchList.simplifyWhiteSpace().lower();
    first = 0;
    do {
	last = searchList.find( separator, first );
	if ( last < 0 )
	    last = searchList.length();
	domains->append( tqstrdup( searchList.mid( first, last-first ) ) );
	first = last+1;
    } while( first < (int)searchList.length() );
}

#elif defined(Q_OS_UNIX)

#if defined(Q_DNS_SYNCHRONOUS)
void TQDns::doSynchronousLookup()
{
    if ( t!=None && !l.isEmpty() ) {
	TQValueListIterator<TQString> it = n.begin();
	TQValueListIterator<TQString> end = n.end();
	int type;
	switch( t ) {
	    case TQDns::A:
		type = 1;
		break;
	    case TQDns::Aaaa:
		type = 28;
		break;
	    case TQDns::Mx:
		type = 15;
		break;
	    case TQDns::Srv:
		type = 33;
		break;
	    case TQDns::Cname:
		type = 5;
		break;
	    case TQDns::Ptr:
		type = 12;
		break;
	    case TQDns::Txt:
		type = 16;
		break;
	    default:
		type = (char)255; // any
		break;
	}
	while( it != end ) {
	    TQString s = *it;
	    it++;
	    TQByteArray ba( 512 );
	    int len = res_search( s.latin1(), 1, type, (uchar*)ba.data(), ba.size() );
	    if ( len > 0 ) {
		ba.resize( len );

		TQDnsQuery * query = new TQDnsQuery;
		query->started = now();
		query->id = ++::id;
		query->t = t;
		query->l = s;
		TQDnsAnswer a( ba, query );
		a.parse();
	    } else if ( len == -1 ) {
		// res_search error
	    }
	}
	emit resultsReady();
    }
}
#endif

void TQDns::doResInit()
{
    if ( ns )
	return;
    ns = new TQPtrList<TQHostAddress>;
    ns->setAutoDelete( true );
    domains = new TQStrList( true );
    domains->setAutoDelete( true );

    // read resolv.conf manually.
    TQFile resolvConf("/etc/resolv.conf");
    if (resolvConf.open(IO_ReadOnly)) {
        TQTextStream stream( &resolvConf );
	TQString line;

	while ( !stream.atEnd() ) {
	    line = stream.readLine();
            TQStringList list = TQStringList::split( " ", line );
	    if ( line.startsWith( "#" ) || list.count() < 2 )
                continue;
            const TQString type = list[0].lower();

	    if ( type == "nameserver" ) {
		TQHostAddress *address = new TQHostAddress();
		if ( address->setAddress( TQString(list[1]) ) ) {
		    // only add ipv6 addresses from resolv.conf if
		    // this host supports ipv6.
		    if ( address->isIPv4Address() || ipv6support )
			ns->append( address );
                    else
                        delete address;
		} else {
		    delete address;
		}
	    } else if ( type == "search" ) {
		TQStringList srch = TQStringList::split( " ", list[1] );
		for ( TQStringList::Iterator i = srch.begin(); i != srch.end(); ++i )
		    domains->append( (*i).lower() );

	    } else if ( type == "domain" ) {
		domains->append( list[1].lower() );
	    }
	}
    }

    if (ns->isEmpty()) {
#if defined(Q_MODERN_RES_API)
	struct __res_state res;
	/* the storage must be clean */
	memset(&res, 0, sizeof(res));
	res_ninit( &res );
	int i;
	// find the name servers to use
	for( i=0; i < MAXNS && i < res.nscount; i++ )
	    ns->append( new TQHostAddress( ntohl( res.nsaddr_list[i].sin_addr.s_addr ) ) );
#  if defined(MAXDFLSRCH)
	for( i=0; i < MAXDFLSRCH; i++ ) {
	    if ( res.dnsrch[i] && *(res.dnsrch[i]) )
		domains->append( TQString::fromLatin1( res.dnsrch[i] ).lower() );
	    else
		break;
	}
#  endif
	if ( *res.defdname )
	    domains->append( TQString::fromLatin1( res.defdname ).lower() );
#else
	qdns_res_init();
	int i;
	// find the name servers to use
	for( i=0; i < MAXNS && i < _res.nscount; i++ )
	    ns->append( new TQHostAddress( ntohl( _res.nsaddr_list[i].sin_addr.s_addr ) ) );
#  if defined(MAXDFLSRCH)
	for( i=0; i < MAXDFLSRCH; i++ ) {
	    if ( _res.dnsrch[i] && *(_res.dnsrch[i]) )
		domains->append( TQString::fromLatin1( _res.dnsrch[i] ).lower() );
	    else
		break;
	}
#  endif
	if ( *_res.defdname )
	    domains->append( TQString::fromLatin1( _res.defdname ).lower() );
#endif

	// the code above adds "0.0.0.0" as a name server at the slightest
	// hint of trouble. so remove those again.
	ns->first();
	while( ns->current() ) {
	    if ( ns->current()->isNull() )
		delete ns->take();
	    else
		ns->next();
	}
    }

    TQFile hosts( TQString::fromLatin1( "/etc/hosts" ) );
    if ( hosts.open( IO_ReadOnly ) ) {
	// read the /etc/hosts file, creating long-life A and PTR RRs
	// for the things we find.
	TQTextStream i( &hosts );
	TQString line;
	while( !i.atEnd() ) {
	    line = i.readLine().simplifyWhiteSpace().lower();
	    uint n = 0;
	    while( n < line.length() && line[(int)n] != '#' )
		n++;
	    line.truncate( n );
	    n = 0;
	    while( n < line.length() && !line[(int)n].isSpace() )
		n++;
	    TQString ip = line.left( n );
	    TQHostAddress a;
	    a.setAddress( ip );
	    if ( ( a.isIPv4Address() || a.isIPv6Address() ) && !a.isNull() ) {
		bool first = true;
		line = line.mid( n+1 );
		n = 0;
		while( n < line.length() && !line[(int)n].isSpace() )
		    n++;
		TQString hostname = line.left( n );
		// ### in case of bad syntax, hostname is invalid. do we care?
		if ( n ) {
		    TQDnsRR * rr = new TQDnsRR( hostname );
		    if ( a.isIPv4Address() )
			rr->t = TQDns::A;
		    else
			rr->t = TQDns::Aaaa;
		    rr->address = a;
		    rr->deleteTime = UINT_MAX;
		    rr->expireTime = UINT_MAX;
		    rr->current = true;
		    if ( first ) {
			first = false;
			TQDnsRR * ptr = new TQDnsRR( TQDns::toInAddrArpaDomain( a ) );
			ptr->t = TQDns::Ptr;
			ptr->target = hostname;
			ptr->deleteTime = UINT_MAX;
			ptr->expireTime = UINT_MAX;
			ptr->current = true;
		    }
		}
	    }
	}
    }
}

#endif

#endif // TQT_NO_DNS
