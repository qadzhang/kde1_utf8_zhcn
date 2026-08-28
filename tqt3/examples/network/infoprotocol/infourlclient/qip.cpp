/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqsocket.h>
#include <ntqurlinfo.h>
#include <ntqurloperator.h>
#include <ntqtextstream.h>

#include "qip.h"


Qip::Qip()
{
    state = Start;
    socket = new TQSocket( this );
    connect( socket, TQ_SIGNAL(connected()), TQ_SLOT(socketConnected()) );
    connect( socket, TQ_SIGNAL(connectionClosed()), TQ_SLOT(socketConnectionClosed()) );
    connect( socket, TQ_SIGNAL(readyRead()), TQ_SLOT(socketReadyRead()) );
    connect( socket, TQ_SIGNAL(error(int)), TQ_SLOT(socketError(int)) );
}

int Qip::supportedOperations() const
{
    return OpListChildren | OpGet;
}

bool Qip::checkConnection( TQNetworkOperation * )
{
    if ( socket->isOpen() )
	return true;

    // don't call connectToHost() if we are already trying to connect
    if ( socket->state() == TQSocket::Connecting )
	return false;

    socket->connectToHost( url()->host(), url()->port() != -1 ? url()->port() : infoPort );
    return false;
}

void Qip::operationListChildren( TQNetworkOperation * )
{
    TQTextStream os(socket);
    os << "LIST " + url()->path() + "\r\n";
    operationInProgress()->setState( StInProgress );
}

void Qip::operationGet( TQNetworkOperation * )
{
    TQTextStream os(socket);
    os << "GET " + url()->path() + "\r\n";
    operationInProgress()->setState( StInProgress );
}

void Qip::socketConnected()
{
    if ( url() )
	emit connectionStateChanged( ConConnected, tr( "Connected to host %1" ).arg( url()->host() ) );
    else
	emit connectionStateChanged( ConConnected, tr ("Connected to host" ) );
}

void Qip::socketConnectionClosed()
{
    if ( url() )
	emit connectionStateChanged( ConClosed, tr( "Connection to %1 closed" ).arg( url()->host() ) );
    else
	emit connectionStateChanged( ConClosed, tr ("Connection closed" ) );
}

void Qip::socketError( int code )
{
    if ( code == TQSocket::ErrHostNotFound ||
	 code == TQSocket::ErrConnectionRefused ) {
	error( ErrHostNotFound, tr( "Host not found or couldn't connect to: %1\n" ).arg( url()->host() ) );
    } else
	error( ErrUnsupported, tr( "Error" ) );
}

void Qip::socketReadyRead()
{
    // read from the server
    TQTextStream stream( socket );
    TQString line;
    while ( socket->canReadLine() ) {
	line = stream.readLine();
	if ( line.startsWith( "500" ) ) {
	    error( ErrValid, line.mid( 4 ) ); 
	} else if ( line.startsWith( "550" ) ) {
	    error( ErrFileNotExisting, line.mid( 4 ) ); 
	} else if ( line.startsWith( "212+" ) ) {
	    if ( state != List ) {
		state = List;
	        emit start( operationInProgress() );
	    }
	    TQUrlInfo inf;
	    inf.setName( line.mid( 6 ) + TQString( ( line[ 4 ] == 'D' ) ? "/" : "" ) );
	    inf.setDir( line[ 4 ] == 'D' );
	    inf.setSymLink( false );
	    inf.setFile( line[ 4 ] == 'F' );
	    inf.setWritable( false );
	    inf.setReadable( true );
	    emit newChild( inf, operationInProgress() );
	} else if ( line.startsWith( "213+" ) ) {
	    state = Data;
	    emit data( line.mid( 4 ).utf8(), operationInProgress() );
	}
	if( line[3] == ' ' && state != Start) {
	    state = Start;
	    operationInProgress()->setState( StDone );
	    emit finished( operationInProgress() );
	}
    }
}

void Qip::error( int code, const TQString& msg )
{
    if ( operationInProgress() ) {
	operationInProgress()->setState( StFailed );
	operationInProgress()->setErrorCode( code );
	operationInProgress()->setProtocolDetail( msg );
	clearOperationQueue();
	emit finished( operationInProgress() );
    }
    state = Start;
}

