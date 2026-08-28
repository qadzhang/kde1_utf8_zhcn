/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef SMTP_H
#define SMTP_H

#include <ntqobject.h>
#include <ntqstring.h>

class TQSocket;
class TQTextStream;
class TQDns;

class Smtp : public TQObject
{
    TQ_OBJECT

public:
    Smtp( const TQString &from, const TQString &to,
	  const TQString &subject, const TQString &body );
    ~Smtp();

signals:
    void status( const TQString & );

private slots:
    void dnsLookupHelper();
    void readyRead();
    void connected();

private:
    enum State {
	Init,
	Mail,
	Rcpt,
	Data,
	Body,
	Quit,
	Close
    };

    TQString message;
    TQString from;
    TQString rcpt;
    TQSocket *socket;
    TQTextStream * t;
    int state;
    TQString response;
    TQDns * mxLookup;
};

#endif
