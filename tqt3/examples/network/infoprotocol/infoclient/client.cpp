/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqsocket.h>
#include <ntqapplication.h>
#include <ntqtextedit.h>
#include <ntqlineedit.h>
#include <ntqlabel.h>
#include <ntqpushbutton.h>
#include <ntqtextstream.h>
#include <ntqlistbox.h>

#include "client.h"


ClientInfo::ClientInfo( TQWidget *parent, const char *name ) :
    ClientInfoBase( parent, name ), socket( 0 )
{
    edHost->setText( "localhost" );
    edPort->setText( TQString::number( (uint)infoPort ) );

    connect( infoList, TQ_SIGNAL(selected(const TQString&)), TQ_SLOT(selectItem(const TQString&)) );
    connect( btnConnect, TQ_SIGNAL(clicked()), TQ_SLOT(connectToServer()) );
    connect( btnBack, TQ_SIGNAL(clicked()), TQ_SLOT(stepBack()) );
    connect( btnQuit, TQ_SIGNAL(clicked()), tqApp, TQ_SLOT(quit()) );
}


void ClientInfo::connectToServer()
{
    delete socket;
    socket = new TQSocket( this );
    connect( socket, TQ_SIGNAL(connected()), TQ_SLOT(socketConnected()) );
    connect( socket, TQ_SIGNAL(connectionClosed()), TQ_SLOT(socketConnectionClosed()) );
    connect( socket, TQ_SIGNAL(readyRead()), TQ_SLOT(socketReadyRead()) );
    connect( socket, TQ_SIGNAL(error(int)), TQ_SLOT(socketError(int)) );

    socket->connectToHost( edHost->text(), edPort->text().toInt() );
}

void ClientInfo::selectItem( const TQString& item )
{
    // item in listBox selected, use LIST or GET depending of the node type.
    if ( item.endsWith( "/" ) ) {
	sendToServer( List, infoPath->text() + item );
	infoPath->setText( infoPath->text() + item );
    } else 
	sendToServer( Get, infoPath->text() + item );
}

void ClientInfo::stepBack()
{
    // go back (up) in path hierarchy
    int i = infoPath->text().findRev( '/', -2 );
    if ( i > 0 ) 
	infoPath->setText( infoPath->text().left( i + 1 ) );
    else
	infoPath->setText( "/" );
    infoList->clear();
    sendToServer( List, infoPath->text() );
}


void ClientInfo::socketConnected()
{
    sendToServer( List, "/" );
}

void ClientInfo::sendToServer( Operation op, const TQString& location )
{
    TQString line;
    switch (op) {
	case List:
	    infoList->clear();
	    line = "LIST " + location;
	    break;
	case Get:
	    line = "GET " + location;
	    break;
    }
    infoText->clear();
    TQTextStream os(socket);
    os << line << "\r\n";
}

void ClientInfo::socketReadyRead()
{
    TQTextStream stream( socket );
    TQString line;
    while ( socket->canReadLine() ) {
	line = stream.readLine();
	if ( line.startsWith( "500" ) || line.startsWith( "550" ) ) {
	    infoText->append( tr( "error: " ) + line.mid( 4 ) );
	} else if ( line.startsWith( "212+" ) ) {
	    infoList->insertItem( line.mid( 6 ) + TQString( ( line[ 4 ] == 'D' ) ? "/" : "" ) );
	} else if ( line.startsWith( "213+" ) ) {
	    infoText->append( line.mid( 4 ) );
	}
    }
}


void ClientInfo::socketConnectionClosed()
{
    infoText->clear();
    infoText->append( tr( "error: Connection closed by the server\n" ) );
}

void ClientInfo::socketError( int code )
{
    infoText->clear();
    infoText->append( tr( "error: Error number %1 occurred\n" ).arg( code ) );
}

