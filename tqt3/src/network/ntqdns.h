/****************************************************************************
**
** Definition of TQDns class.
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

#ifndef TQDNS_H
#define TQDNS_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqhostaddress.h"
#include "ntqsocketnotifier.h"
#include "ntqstringlist.h"
#endif // QT_H

#if !defined( TQT_MODULE_NETWORK ) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_NETWORK )
#define TQM_EXPORT_DNS
#else
#define TQM_EXPORT_DNS TQ_EXPORT
#endif

#ifndef TQT_NO_DNS

//#define Q_DNS_SYNCHRONOUS

class TQDnsPrivate;

class TQM_EXPORT_DNS TQDns: public TQObject {
    TQ_OBJECT
public:
    enum RecordType {
	None,
	A, Aaaa,
	Mx, Srv,
	Cname,
	Ptr,
	Txt
    };

    TQDns();
    TQDns( const TQString & label, RecordType rr = A );
    TQDns( const TQHostAddress & address, RecordType rr = Ptr );
    virtual ~TQDns();

    // to set/change the query
    virtual void setLabel( const TQString & label );
    virtual void setLabel( const TQHostAddress & address );
    TQString label() const { return l; }

    virtual void setRecordType( RecordType rr = A );
    RecordType recordType() const { return t; }

    // whether something is happening behind the scenes
    bool isWorking() const;

    // to query for replies
    TQValueList<TQHostAddress> addresses() const;

    class TQM_EXPORT_DNS MailServer {
    public:
	MailServer( const TQString & n=TQString::null, TQ_UINT16 p=0 )
	    :name(n), priority(p) {}
	TQString name;
	TQ_UINT16 priority;
    };
    TQValueList<MailServer> mailServers() const;

    class TQM_EXPORT_DNS Server {
    public:
	Server(const TQString & n=TQString::null, TQ_UINT16 p=0, TQ_UINT16 w=0, TQ_UINT16 po=0 )
	    : name(n), priority(p), weight(w), port(po) {}
	TQString name;
	TQ_UINT16 priority;
	TQ_UINT16 weight;
	TQ_UINT16 port;
    };
    TQValueList<Server> servers() const;

    TQStringList hostNames() const;

    TQStringList texts() const;

    TQString canonicalName() const; // ### real-world but uncommon: TQStringList

    TQStringList qualifiedNames() const { return n; }

#if defined(Q_DNS_SYNCHRONOUS)
protected:
    void connectNotify( const char *signal );
#endif

signals:
    void resultsReady();

private slots:
    void startQuery();

private:
    static void doResInit();
    void setStartQueryTimer();
    static TQString toInAddrArpaDomain( const TQHostAddress &address );
#if defined(Q_DNS_SYNCHRONOUS)
    void doSynchronousLookup();
#endif

    TQString l;
    TQStringList n;
    RecordType t;
    TQDnsPrivate * d;

    friend class TQDnsAnswer;
    friend class TQDnsManager;
};


// TQDnsSocket are sockets that are used for DNS lookup

class TQDnsSocket: public TQObject {
    TQ_OBJECT
    // note: Private not public.  This class contains NO public API.
protected:
    TQDnsSocket( TQObject *, const char * );
    virtual ~TQDnsSocket();

private slots:
    virtual void cleanCache();
    virtual void retransmit();
    virtual void answer();
};

#endif // TQT_NO_DNS

#endif // TQDNS_H
