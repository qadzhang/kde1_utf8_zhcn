/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "nntp.h"
#include <ntqurlinfo.h>
#include <stdlib.h>
#include <ntqurloperator.h>
#include <ntqstringlist.h>
#include <ntqregexp.h>

Nntp::Nntp()
    : TQNetworkProtocol(), connectionReady( false ),
      readGroups( false ), readArticle( false )
{
    // create the command socket and connect to its signals
    commandSocket = new TQSocket( this );
    connect( commandSocket, TQ_SIGNAL( hostFound() ),
	     this, TQ_SLOT( hostFound() ) );
    connect( commandSocket, TQ_SIGNAL( connected() ),
	     this, TQ_SLOT( connected() ) );
    connect( commandSocket, TQ_SIGNAL( connectionClosed() ),
	     this, TQ_SLOT( closed() ) );
    connect( commandSocket, TQ_SIGNAL( readyRead() ),
	     this, TQ_SLOT( readyRead() ) );
    connect( commandSocket, TQ_SIGNAL( error( int ) ),
	     this, TQ_SLOT( error( int ) ) );
}

Nntp::~Nntp()
{
    close();
    delete commandSocket;
}

void Nntp::operationListChildren( TQNetworkOperation * )
{
    // create a command
    TQString path = url()->path(), cmd;
    if ( path.isEmpty() || path == "/" ) {
	// if the path is empty or we are in the root dir,
	// we want to read the list of available newsgroups
	cmd = "list newsgroups\r\n";
    } else if ( url()->isDir() ) {
	// if the path is a directory (in our case a news group)
	// we want to list the articles of this group
	path = path.replace( "/", "" );
	cmd = "listgroup " + path + "\r\n";
    } else
	return;

    // write the command to the socket
    commandSocket->writeBlock( cmd.latin1(), cmd.length() );
    readGroups = true;
}

void Nntp::operationGet( TQNetworkOperation *op )
{
    // get the dirPath of the URL (this is our news group)
    // and the filename (which is the article we want to read)
    TQUrl u( op->arg( 0 ) );
    TQString dirPath = u.dirPath(), file = u.fileName();
    dirPath = dirPath.replace( "/", "" );

    // go to the group in which the article is
    TQString cmd;
    cmd = "group " + dirPath + "\r\n";
    commandSocket->writeBlock( cmd.latin1(), cmd.length() );

    // read the head of the article
    cmd = "article " + file + "\r\n";
    commandSocket->writeBlock( cmd.latin1(), cmd.length() );
    readArticle = true;
}

bool Nntp::checkConnection( TQNetworkOperation * )
{
    // we are connected, return true
    if ( commandSocket->isOpen() && connectionReady )
	return true;

    // seems that there is no chance to connect
    if ( commandSocket->isOpen() )
	return false;

    // don't call connectToHost() if we are already trying to connect
    if ( commandSocket->state() == TQSocket::Connecting )
	return false;

    // start connecting
    connectionReady = false;
    commandSocket->connectToHost( url()->host(),
				  url()->port() != -1 ? url()->port() : 119 );
    return false;
}

void Nntp::close()
{
    // close the command socket
    if ( commandSocket->isOpen() ) {
 	commandSocket->writeBlock( "quit\r\n", strlen( "quit\r\n" ) );
 	commandSocket->close();
    }
}

int Nntp::supportedOperations() const
{
    // we only support listing children and getting data
    return OpListChildren | OpGet;
}

void Nntp::hostFound()
{
    if ( url() )
	emit connectionStateChanged( ConHostFound, tr( "Host %1 found" ).arg( url()->host() ) );
    else
	emit connectionStateChanged( ConHostFound, tr( "Host found" ) );
}

void Nntp::connected()
{
    if ( url() )
	emit connectionStateChanged( ConConnected, tr( "Connected to host %1" ).arg( url()->host() ) );
    else
	emit connectionStateChanged( ConConnected, tr( "Connected to host" ) );
}

void Nntp::closed()
{
    if ( url() )
	emit connectionStateChanged( ConClosed, tr( "Connection to %1 closed" ).arg( url()->host() ) );
    else
	emit connectionStateChanged( ConClosed, tr( "Connection closed" ) );
}

void Nntp::readyRead()
{
    // new data arrived on the command socket

    // of we should read the list of available groups, let's do so
    if ( readGroups ) {
	parseGroups();
	return;
    }

    // of we should read an article, let's do so
    if ( readArticle ) {
	parseArticle();
	return;
    }

    // read the new data from the socket
    TQCString s;
    s.resize( commandSocket->bytesAvailable() + 1 );
    commandSocket->readBlock( s.data(), commandSocket->bytesAvailable() );

    if ( !url() )
	return;

    // of the code of the server response was 200, we know that the
    // server is ready to get commands from us now
    if ( s.left( 3 ) == "200" )
	connectionReady = true;
}

void Nntp::parseGroups()
{
    if ( !commandSocket->canReadLine() )
	return;

    // read one line after the other
    while ( commandSocket->canReadLine() ) {
	TQString s = commandSocket->readLine();

	// if the  line starts with a dot, all groups or articles have been listed,
	// so we finished processing the listChildren() command
	if ( s[ 0 ] == '.' ) {
	    readGroups = false;
	    operationInProgress()->setState( StDone );
	    emit finished( operationInProgress() );
	    return;
	}

	// if the code of the server response is 215 or 211
	// the next line will be the first group or article (depending on what we read).
	// So let others know that we start reading now...
	if ( s.left( 3 ) == "215" || s.left( 3 ) == "211" ) {
	    operationInProgress()->setState( StInProgress );
	    emit start( operationInProgress() );
	    continue;
	}

	// parse the line and create a TQUrlInfo object
	// which describes the child (group or article)
	bool tab = s.find( '\t' ) != -1;
	TQString group = s.mid( 0, s.find( tab ? '\t' : ' ' ) );
	TQUrlInfo inf;
	inf.setName( group );
	TQString path = url()->path();
	inf.setDir( path.isEmpty() || path == "/" );
	inf.setSymLink( false );
	inf.setFile( !inf.isDir() );
	inf.setWritable( false );
	inf.setReadable( true );

	// let others know about our new child
	emit newChild( inf, operationInProgress() );
    }

}

void Nntp::parseArticle()
{
    if ( !commandSocket->canReadLine() )
	return;

    // read an article one line after the other
    while ( commandSocket->canReadLine() ) {
	TQString s = commandSocket->readLine();

	// if the  line starts with a dot, we finished reading something
	if ( s[ 0 ] == '.' ) {
	    readArticle = false;
	    operationInProgress()->setState( StDone );
	    emit finished( operationInProgress() );
	    return;
	}

	if ( s.right( 1 ) == "\n" )
	    s.remove( s.length() - 1, 1 );

	// emit the new data of the article which we read
	emit data( TQCString( s.ascii() ), operationInProgress() );
    }
}

void Nntp::error( int code )
{
    if ( code == TQSocket::ErrHostNotFound ||
	 code == TQSocket::ErrConnectionRefused ) {
	// this signal is called if connecting to the server failed
	if ( operationInProgress() ) {
	    TQString msg = tr( "Host not found or couldn't connect to: \n" + url()->host() );
	    operationInProgress()->setState( StFailed );
	    operationInProgress()->setProtocolDetail( msg );
	    operationInProgress()->setErrorCode( (int)ErrHostNotFound );
	    clearOperationQueue();
	    emit finished( operationInProgress() );
	}
    }
}
