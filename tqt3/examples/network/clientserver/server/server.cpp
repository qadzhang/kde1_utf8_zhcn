/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqsocket.h>
#include <ntqserversocket.h>
#include <ntqapplication.h>
#include <ntqvbox.h>
#include <ntqtextview.h>
#include <ntqlabel.h>
#include <ntqpushbutton.h>
#include <ntqtextstream.h>

#include <stdlib.h>


/*
  The ClientSocket class provides a socket that is connected with a client.
  For every client that connects to the server, the server creates a new
  instance of this class.
*/
class ClientSocket : public TQSocket
{
    TQ_OBJECT
public:
    ClientSocket( int sock, TQObject *parent=0, const char *name=0 ) :
	TQSocket( parent, name )
    {
	line = 0;
	connect( this, TQ_SIGNAL(readyRead()),
		TQ_SLOT(readClient()) );
	connect( this, TQ_SIGNAL(connectionClosed()),
		TQ_SLOT(deleteLater()) );
	setSocket( sock );
    }

    ~ClientSocket()
    {
    }

signals:
    void logText( const TQString& );

private slots:
    void readClient()
    {
	TQTextStream ts( this );
	while ( canReadLine() ) {
	    TQString str = ts.readLine();
	    emit logText( tr("Read: '%1'\n").arg(str) );

	    ts << line << ": " << str << endl;
	    emit logText( tr("Wrote: '%1: %2'\n").arg(line).arg(str) );

	    line++;
	}
    }

private:
    int line;
};


/*
  The SimpleServer class handles new connections to the server. For every
  client that connects, it creates a new ClientSocket -- that instance is now
  responsible for the communication with that client.
*/
class SimpleServer : public TQServerSocket
{
    TQ_OBJECT
public:
    SimpleServer( TQObject* parent=0 ) :
	TQServerSocket( 4242, 1, parent )
    {
	if ( !ok() ) {
	    tqWarning("Failed to bind to port 4242");
	    exit(1);
	}
    }

    ~SimpleServer()
    {
    }

    void newConnection( int socket )
    {
	ClientSocket *s = new ClientSocket( socket, this );
	emit newConnect( s );
    }

signals:
    void newConnect( ClientSocket* );
};


/*
  The ServerInfo class provides a small GUI for the server. It also creates the
  SimpleServer and as a result the server.
*/
class ServerInfo : public TQVBox
{
    TQ_OBJECT
public:
    ServerInfo()
    {
	SimpleServer *server = new SimpleServer( this );

	TQString itext = tr(
		"This is a small server example.\n"
		"Connect with the client now."
		);
	TQLabel *lb = new TQLabel( itext, this );
	lb->setAlignment( AlignHCenter );
	infoText = new TQTextView( this );
	TQPushButton *quit = new TQPushButton( tr("Quit") , this );

	connect( server, TQ_SIGNAL(newConnect(ClientSocket*)),
		TQ_SLOT(newConnect(ClientSocket*)) );
	connect( quit, TQ_SIGNAL(clicked()), tqApp,
		TQ_SLOT(quit()) );
    }

    ~ServerInfo()
    {
    }

private slots:
    void newConnect( ClientSocket *s )
    {
	infoText->append( tr("New connection\n") );
	connect( s, TQ_SIGNAL(logText(const TQString&)),
		infoText, TQ_SLOT(append(const TQString&)) );
	connect( s, TQ_SIGNAL(connectionClosed()),
		TQ_SLOT(connectionClosed()) );
    }

    void connectionClosed()
    {
	infoText->append( tr("Client closed connection\n") );
    }

private:
    TQTextView *infoText;
};


int main( int argc, char** argv )
{
    TQApplication app( argc, argv );
    ServerInfo info;
    app.setMainWidget( &info );
    info.show();
    return app.exec();
}

#include "server.moc"
