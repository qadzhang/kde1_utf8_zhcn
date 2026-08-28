/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "smtp.h"

#include <ntqtextstream.h>
#include <ntqsocket.h>
#include <ntqdns.h>
#include <ntqtimer.h>
#include <ntqapplication.h>
#include <ntqmessagebox.h>
#include <ntqregexp.h>


Smtp::Smtp( const TQString &from, const TQString &to,
	    const TQString &subject,
	    const TQString &body )
{
    socket = new TQSocket( this );
    connect ( socket, TQ_SIGNAL( readyRead() ),
	      this, TQ_SLOT( readyRead() ) );
    connect ( socket, TQ_SIGNAL( connected() ),
	      this, TQ_SLOT( connected() ) );

    mxLookup = new TQDns( to.mid( to.find( '@' )+1 ), TQDns::Mx );
    connect( mxLookup, TQ_SIGNAL(resultsReady()),
	     this, TQ_SLOT(dnsLookupHelper()) );

    message = TQString::fromLatin1( "From: " ) + from +
	      TQString::fromLatin1( "\nTo: " ) + to +
	      TQString::fromLatin1( "\nSubject: " ) + subject +
	      TQString::fromLatin1( "\n\n" ) + body + "\n";
    message.replace( TQString::fromLatin1( "\n" ),
		     TQString::fromLatin1( "\r\n" ) );
    message.replace( TQString::fromLatin1( "\r\n.\r\n" ),
		     TQString::fromLatin1( "\r\n..\r\n" ) );

    this->from = from;
    rcpt = to;

    state = Init;
}


Smtp::~Smtp()
{
    delete t;
    delete socket;
}


void Smtp::dnsLookupHelper()
{
    TQValueList<TQDns::MailServer> s = mxLookup->mailServers();
    if ( s.isEmpty() ) {
	if ( !mxLookup->isWorking() )
	    emit status( tr( "Error in MX record lookup" ) );
	return;
    }

    emit status( tr( "Connecting to %1" ).arg( s.first().name ) );

    socket->connectToHost( s.first().name, 25 );
    t = new TQTextStream( socket );
}


void Smtp::connected()
{
    emit status( tr( "Connected to %1" ).arg( socket->peerName() ) );
}

void Smtp::readyRead()
{
    // SMTP is line-oriented
    if ( !socket->canReadLine() )
	return;

    TQString responseLine;
    do {
	responseLine = socket->readLine();
	response += responseLine;
    } while( socket->canReadLine() && responseLine[3] != ' ' );
    responseLine.truncate( 3 );

    if ( state == Init && responseLine[0] == '2' ) {
	// banner was okay, let's go on
	*t << "HELO there\r\n";
	state = Mail;
    } else if ( state == Mail && responseLine[0] == '2' ) {
	// HELO response was okay (well, it has to be)
	*t << "MAIL FROM: <" << from << ">\r\n";
	state = Rcpt;
    } else if ( state == Rcpt && responseLine[0] == '2' ) {
	*t << "RCPT TO: <" << rcpt << ">\r\n";
	state = Data;
    } else if ( state == Data && responseLine[0] == '2' ) {
	*t << "DATA\r\n";
	state = Body;
    } else if ( state == Body && responseLine[0] == '3' ) {
	*t << message << ".\r\n";
	state = Quit;
    } else if ( state == Quit && responseLine[0] == '2' ) {
	*t << "QUIT\r\n";
	// here, we just close.
	state = Close;
	emit status( tr( "Message sent" ) );
    } else if ( state == Close ) {
	deleteLater();
	return;
    } else {
	// something broke.
	TQMessageBox::warning( tqApp->activeWindow(),
			      tr( "TQt Mail Example" ),
			      tr( "Unexpected reply from SMTP server:\n\n" ) +
			      response );
	state = Close;
    }

    response = "";
}
