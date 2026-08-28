/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the TQAssistantClient library.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "ntqassistantclient.h"

#include <ntqsocket.h>
#include <ntqtextstream.h>
#include <ntqprocess.h>
#include <ntqtimer.h>
#include <ntqfileinfo.h>

class TQAssistantClientPrivate
{
    friend class TQAssistantClient;
    TQStringList arguments;
};

static TQMap<const TQAssistantClient*,TQAssistantClientPrivate*> *dpointers = 0;

static TQAssistantClientPrivate *data( const TQAssistantClient *client, bool create=false )
{
    if( !dpointers )
	dpointers = new TQMap<const TQAssistantClient*,TQAssistantClientPrivate*>;
    TQAssistantClientPrivate *d = (*dpointers)[client];
    if( !d && create ) {
	d = new TQAssistantClientPrivate;
	dpointers->insert( client, d );
    }
    return d;
}

/*!
    \class TQAssistantClient
    \brief The TQAssistantClient class provides a means of using TQt
    Assistant as an application's help tool.

    Using TQt Assistant is simple: Create a TQAssistantClient instance,
    then call showPage() as often as necessary to show your help
    pages. When you call showPage(), TQt Assistant will be launched if
    it isn't already running.

    The TQAssistantClient instance can open (openAssistant()) or close
    (closeAssistant()) TQt Assistant whenever required. If TQt Assistant
    is open, isOpen() returns true.

    One TQAssistantClient instance interacts with one TQt Assistant
    instance, so every time you call openAssistant(), showPage() or
    closeAssistant() they are applied to the particular TQt Assistant
    instance associated with the TQAssistantClient.

    When you call openAssistant() the assistantOpened() signal is
    emitted. Similarly when closeAssistant() is called,
    assistantClosed() is emitted. In either case, if an error occurs,
    error() is emitted.

    This class is not included in the TQt library itself. To use it you
    must link against \c libtqassistantclient.a (Unix) or \c
    qassistantclient.lib (Windows), which is built into \c INSTALL/lib
    if you built the TQt tools (\c INSTALL is the directory where TQt is
    installed). If you use qmake, then you can simply add the following
    line to your pro file:

    \code
	LIBS += -ltqassistantclient
    \endcode

    See also "Adding Documentation to TQt Assistant" in the \link
    assistant.book TQt Assistant manual\endlink.
*/

/*!
    \fn void TQAssistantClient::assistantOpened()

    This signal is emitted when TQt Assistant is open and the
    client-server communication is set up.
*/

/*!
    \fn void TQAssistantClient::assistantClosed()

    This signal is emitted when the connection to TQt Assistant is
    closed. This happens when the user exits TQt Assistant, or when an
    error in the server or client occurs, or if closeAssistant() is
    called.
*/

/*!
    \fn void TQAssistantClient::error( const TQString &msg )

    This signal is emitted if TQt Assistant cannot be started or if an
    error occurs during the initialization of the connection between
    TQt Assistant and the calling application. The \a msg provides an
    explanation of the error.
*/

/*!
    Constructs an assistant client object. The \a path specifies the
    path to the TQt Assistant executable. If \a path is an empty
    string the system path (\c{%PATH%} or \c $PATH) is used.

    The assistant client object is a child of \a parent and is called
    \a name.
*/
TQAssistantClient::TQAssistantClient( const TQString &path, TQObject *parent, const char *name )
    : TQObject( parent, name ), host ( "localhost" )
{
    if ( path.isEmpty() )
	assistantCommand = "assistant";
    else {
	TQFileInfo fi( path );
	if ( fi.isDir() )
	    assistantCommand = path + "/assistant";
	else
	    assistantCommand = path;
    }

#if defined(Q_OS_MACX)
    assistantCommand += ".app/Contents/MacOS/assistant";
#elif defined(TQ_WS_WIN)
    if (!assistantCommand.endsWith(".exe"))
        assistantCommand += ".exe";
#endif
    socket = new TQSocket( this );
    connect( socket, TQ_SIGNAL( connected() ),
	    TQ_SLOT( socketConnected() ) );
    connect( socket, TQ_SIGNAL( connectionClosed() ),
	    TQ_SLOT( socketConnectionClosed() ) );
    connect( socket, TQ_SIGNAL( error( int ) ),
	    TQ_SLOT( socketError( int ) ) );
    opened = false;
    proc = new TQProcess( this );
    port = 0;
    pageBuffer = "";
    connect( proc, TQ_SIGNAL( readyReadStderr() ),
	     this, TQ_SLOT( readStdError() ) );
}

/*!
    Destroys the assistant client object and frees up all allocated
    resources.
*/
TQAssistantClient::~TQAssistantClient()
{
    if ( proc && proc->isRunning() ) {
	proc->tryTerminate();
	proc->kill();
    }

    if( dpointers ) {
	TQAssistantClientPrivate *d = (*dpointers)[ this ];
	if( d ) {
	    dpointers->remove( this );
	    delete d;
	    if( dpointers->isEmpty() ) {
		delete dpointers;
		dpointers = 0;
	    }
	}
    }
}

/*!
    This function opens TQt Assistant and sets up the client-server
    communiction between the application and TQt Assistant. If it is
    already open, this function does nothing. If an error occurs,
    error() is emitted.

    \sa assistantOpened()
*/
void TQAssistantClient::openAssistant()
{
    if ( proc->isRunning() )
	return;
    proc->clearArguments();
    proc->addArgument( assistantCommand );
    proc->addArgument( "-server" );
    if( !pageBuffer.isEmpty() ) {
        proc->addArgument( "-file" );
        proc->addArgument( pageBuffer );
    }

    TQAssistantClientPrivate *d = data( this );
    if( d ) {
	TQStringList::ConstIterator it = d->arguments.begin();
	while( it!=d->arguments.end() ) {
	    proc->addArgument( *it );
	    ++it;
	}
    }

    if ( !proc->launch( TQString::null ) ) {
	emit error( tr( "Cannot start TQt Assistant '%1'" )
		    .arg( proc->arguments().join( " " ) ) );
	return;
    }
    connect( proc, TQ_SIGNAL( readyReadStdout() ),
	     this, TQ_SLOT( readPort() ) );
}

void TQAssistantClient::readPort()
{
    TQString p = proc->readLineStdout();
    TQ_UINT16 port = p.toUShort();
    if ( port == 0 ) {
	emit error( tr( "Cannot connect to TQt Assistant." ) );
	return;
    }
    socket->connectToHost( host, port );
    disconnect( proc, TQ_SIGNAL( readyReadStdout() ),
		this, TQ_SLOT( readPort() ) );
}

/*!
    Use this function to close TQt Assistant.

    \sa assistantClosed()
*/
void TQAssistantClient::closeAssistant()
{
    if ( !opened )
	return;
    proc->tryTerminate();
    proc->kill();
}

/*!
    Call this function to make TQt Assistant show a particular \a page.
    The \a page is a filename (e.g. \c myhelpfile.html). See "Adding
    Documentation to TQt Assistant" in the \link assistant.book TQt
    Assistant manual\endlink for further information.

    If TQt Assistant hasn't been \link openAssistant() opened\endlink
    yet, this function will do nothing. You can use isOpen() to
    determine whether TQt Assistant is up and running, or you can
    connect to the asssistantOpened() signal.

    \sa isOpen(), assistantOpened()
*/
void TQAssistantClient::showPage( const TQString &page )
{
    if ( !opened ) {
	pageBuffer = page;
	openAssistant();
	pageBuffer = TQString::null;	
	return;
    }
    TQTextStream os( socket );
    os << page << "\n";
}

/*!
    \property TQAssistantClient::open
    \brief Whether TQt Assistant is open.

*/
bool TQAssistantClient::isOpen() const
{
    return opened;
}

void TQAssistantClient::socketConnected()
{
    opened = true;
    if ( !pageBuffer.isEmpty() )
	showPage( pageBuffer );
    emit assistantOpened();
}

void TQAssistantClient::socketConnectionClosed()
{
    opened = false;
    emit assistantClosed();
}

void TQAssistantClient::socketError( int i )
{
    if ( i == TQSocket::ErrConnectionRefused )
	emit error( tr( "Could not connect to Assistant: Connection refused" ) );
    else if ( i == TQSocket::ErrHostNotFound )
	emit error( tr( "Could not connect to Assistant: Host not found" ) );
    else
	emit error( tr( "Communication error" ) );
}

void TQAssistantClient::readStdError()
{
    TQString errmsg;
    while ( proc->canReadLineStderr() ) {
	errmsg += proc->readLineStderr();
	errmsg += "\n";
    }
    if (!errmsg.isEmpty())
	emit error( tr( errmsg.simplifyWhiteSpace() ) );
}

/*!
    Sets the command line arguments used when TQt Assistant is
    started to \a args.
*/
void TQAssistantClient::setArguments( const TQStringList &args )
{
    TQAssistantClientPrivate *d = data( this, true );
    d->arguments = args;
}
