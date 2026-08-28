/****************************************************************************
**
** Implementation of TQFtp class.
**
** Created : 970521
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqftp.h"

#ifndef TQT_NO_NETWORKPROTOCOL_FTP

#include "ntqsocket.h"
#include "ntqsocketdevice.h"
#include "ntqurlinfo.h"
#include "ntqurloperator.h"
#include "ntqstringlist.h"
#include "ntqregexp.h"
#include "ntqtimer.h"
#include "ntqfileinfo.h"
#include "ntqptrdict.h" // binary compatibility

#ifndef TQT_NO_TEXTCODEC
#include "ntqtextcodec.h"
#endif

//#define TQFTPPI_DEBUG
//#define TQFTPDTP_DEBUG


class TQFtpPI;

class TQFtpDTP : public TQObject
{
    TQ_OBJECT

public:
    enum ConnectState {
	CsHostFound,
	CsConnected,
	CsClosed,
	CsHostNotFound,
	CsConnectionRefused
    };

    TQFtpDTP( TQFtpPI *p, TQObject *parent=0, const char *name=0 );

    void setData( TQByteArray * );
    void setDevice( TQIODevice * );
    void writeData();

    void setBytesTotal( int bytes )
    {
	bytesTotal = bytes;
	bytesDone = 0;
	emit dataTransferProgress( bytesDone, bytesTotal );
    }

    bool hasError() const;
    TQString errorMessage() const;
    void clearError();

    void connectToHost( const TQString & host, TQ_UINT16 port )
    { socket.connectToHost( host, port ); }

    TQSocket::State socketState() const
    { return socket.state(); }

    TQ_ULONG bytesAvailable() const
    { return socket.bytesAvailable(); }

    TQ_LONG readBlock( char *data, TQ_ULONG maxlen )
    {
	TQ_LONG read = socket.readBlock( data, maxlen );
	bytesDone += read;
	return read;
    }

    TQByteArray readAll()
    {
	TQByteArray tmp = socket.readAll();
	bytesDone += tmp.size();
	return tmp;
    }

    void abortConnection();

    static bool parseDir( const TQString &buffer, const TQString &userName, TQUrlInfo *info );

signals:
    void listInfo( const TQUrlInfo& );
    void readyRead();
    void dataTransferProgress( int, int );

    void connectState( int );

private slots:
    void socketConnected();
    void socketReadyRead();
    void socketError( int );
    void socketConnectionClosed();
    void socketBytesWritten( int );

private:
    void clearData()
    {
	is_ba = false;
	data.dev = 0;
    }

    TQSocket socket;
    TQFtpPI *pi;
    TQString err;
    int bytesDone;
    int bytesTotal;
    bool callWriteData;

    // If is_ba is true, ba is used; ba is never 0.
    // Otherwise dev is used; dev can be 0 or not.
    union {
	TQByteArray *ba;
	TQIODevice *dev;
    } data;
    bool is_ba;
};

class TQFtpPI : public TQObject
{
    TQ_OBJECT

public:
    TQFtpPI( TQObject *parent = 0 );

    void connectToHost( const TQString &host, TQ_UINT16 port );

    bool sendCommands( const TQStringList &cmds );
    bool sendCommand( const TQString &cmd )
    { return sendCommands( TQStringList( cmd ) ); }

    void clearPendingCommands();
    void abort();

    TQString currentCommand() const
    { return currentCmd; }

    bool rawCommand;

    TQFtpDTP dtp; // the PI has a DTP which is not the design of RFC 959, but it
		 // makes the design simpler this way
signals:
    void connectState( int );
    void finished( const TQString& );
    void error( int, const TQString& );
    void rawFtpReply( int, const TQString& );

private slots:
    void hostFound();
    void connected();
    void connectionClosed();
    void delayedCloseFinished();
    void readyRead();
    void error( int );

    void dtpConnectState( int );

private:
    // the states are modelled after the generalized state diagram of RFC 959,
    // page 58
    enum State {
	Begin,
	Idle,
	Waiting,
	Success,
	Failure
    };

    enum AbortState {
	None,
	AbortStarted,
	WaitForAbortToFinish
    };

    bool processReply();
    bool startNextCmd();

    TQSocket commandSocket;
    TQString replyText;
    char replyCode[3];
    State state;
    AbortState abortState;
    TQStringList pendingCommands;
    TQString currentCmd;

    bool waitForDtpToConnect;
    bool waitForDtpToClose;
};

/**********************************************************************
 *
 * TQFtpCommand implemenatation
 *
 *********************************************************************/
class TQFtpCommand
{
public:
    TQFtpCommand( TQFtp::Command cmd, TQStringList raw );
    TQFtpCommand( TQFtp::Command cmd, TQStringList raw, const TQByteArray &ba );
    TQFtpCommand( TQFtp::Command cmd, TQStringList raw, TQIODevice *dev );
    ~TQFtpCommand();

    int id;
    TQFtp::Command command;
    TQStringList rawCmds;

    // If is_ba is true, ba is used; ba is never 0.
    // Otherwise dev is used; dev can be 0 or not.
    union {
	TQByteArray *ba;
	TQIODevice *dev;
    } data;
    bool is_ba;

    static int idCounter;
};

int TQFtpCommand::idCounter = 0;

TQFtpCommand::TQFtpCommand( TQFtp::Command cmd, TQStringList raw )
    : command(cmd), rawCmds(raw), is_ba(false)
{
    id = ++idCounter;
    data.dev = 0;
}

TQFtpCommand::TQFtpCommand( TQFtp::Command cmd, TQStringList raw, const TQByteArray &ba )
    : command(cmd), rawCmds(raw), is_ba(true)
{
    id = ++idCounter;
    data.ba = new TQByteArray( ba );
}

TQFtpCommand::TQFtpCommand( TQFtp::Command cmd, TQStringList raw, TQIODevice *dev )
    : command(cmd), rawCmds(raw), is_ba(false)
{
    id = ++idCounter;
    data.dev = dev;
}

TQFtpCommand::~TQFtpCommand()
{
    if ( is_ba )
	delete data.ba;
}

/**********************************************************************
 *
 * TQFtpDTP implemenatation
 *
 *********************************************************************/
TQFtpDTP::TQFtpDTP( TQFtpPI *p, TQObject *parent, const char *name ) :
    TQObject( parent, name ),
    socket( 0, "TQFtpDTP_socket" ),
    pi( p ),
    callWriteData( false )
{
    clearData();

    connect( &socket, TQ_SIGNAL( connected() ),
	     TQ_SLOT( socketConnected() ) );
    connect( &socket, TQ_SIGNAL( readyRead() ),
	     TQ_SLOT( socketReadyRead() ) );
    connect( &socket, TQ_SIGNAL( error(int) ),
	     TQ_SLOT( socketError(int) ) );
    connect( &socket, TQ_SIGNAL( connectionClosed() ),
	     TQ_SLOT( socketConnectionClosed() ) );
    connect( &socket, TQ_SIGNAL( bytesWritten(int) ),
	     TQ_SLOT( socketBytesWritten(int) ) );
}

void TQFtpDTP::setData( TQByteArray *ba )
{
    is_ba = true;
    data.ba = ba;
}

void TQFtpDTP::setDevice( TQIODevice *dev )
{
    is_ba = false;
    data.dev = dev;
}

void TQFtpDTP::writeData()
{
    if ( is_ba ) {
#if defined(TQFTPDTP_DEBUG)
	tqDebug( "TQFtpDTP::writeData: write %d bytes", data.ba->size() );
#endif
	if ( data.ba->size() == 0 )
	    emit dataTransferProgress( 0, bytesTotal );
	else
	    socket.writeBlock( data.ba->data(), data.ba->size() );
	socket.close();
	clearData();
    } else if ( data.dev ) {
	callWriteData = false;
	const int blockSize = 16*1024;
	char buf[blockSize];
	while ( !data.dev->atEnd() && socket.bytesToWrite()==0 ) {
	    TQ_LONG read = data.dev->readBlock( buf, blockSize );
#if defined(TQFTPDTP_DEBUG)
	    tqDebug( "TQFtpDTP::writeData: writeBlock() of size %d bytes", (int)read );
#endif
	    socket.writeBlock( buf, read );
	    if ( !data.dev )
		return; // this can happen when a command is aborted
	}
	if ( data.dev->atEnd() ) {
	    if ( bytesDone==0 && socket.bytesToWrite()==0 )
		emit dataTransferProgress( 0, bytesTotal );
	    socket.close();
	    clearData();
	} else {
	    callWriteData = true;
	}
    }
}

inline bool TQFtpDTP::hasError() const
{
    return !err.isNull();
}

inline TQString TQFtpDTP::errorMessage() const
{
    return err;
}

inline void TQFtpDTP::clearError()
{
    err = TQString::null;
}

void TQFtpDTP::abortConnection()
{
#if defined(TQFTPDTP_DEBUG)
    tqDebug( "TQFtpDTP::abortConnection" );
#endif
    callWriteData = false;
    clearData();

    socket.clearPendingData();
    socket.close();
}

bool TQFtpDTP::parseDir( const TQString &buffer, const TQString &userName, TQUrlInfo *info )
{
    TQStringList lst = TQStringList::split( " ", buffer );

    if ( lst.count() < 9 )
	return false;

    TQString tmp;

    // permissions
    tmp = lst[ 0 ];

    if ( tmp[ 0 ] == TQChar( 'd' ) ) {
	info->setDir( true );
	info->setFile( false );
	info->setSymLink( false );
    } else if ( tmp[ 0 ] == TQChar( '-' ) ) {
	info->setDir( false );
	info->setFile( true );
	info->setSymLink( false );
    } else if ( tmp[ 0 ] == TQChar( 'l' ) ) {
	info->setDir( true ); // #### todo
	info->setFile( false );
	info->setSymLink( true );
    } else {
	return false;
    }

    static int user = 0;
    static int group = 1;
    static int other = 2;
    static int readable = 0;
    static int writable = 1;
    static int executable = 2;

    bool perms[ 3 ][ 3 ];
    perms[0][0] = (tmp[ 1 ] == 'r');
    perms[0][1] = (tmp[ 2 ] == 'w');
    perms[0][2] = (tmp[ 3 ] == 'x');
    perms[1][0] = (tmp[ 4 ] == 'r');
    perms[1][1] = (tmp[ 5 ] == 'w');
    perms[1][2] = (tmp[ 6 ] == 'x');
    perms[2][0] = (tmp[ 7 ] == 'r');
    perms[2][1] = (tmp[ 8 ] == 'w');
    perms[2][2] = (tmp[ 9 ] == 'x');

    // owner
    tmp = lst[ 2 ];
    info->setOwner( tmp );

    // group
    tmp = lst[ 3 ];
    info->setGroup( tmp );

    // ### not correct
    info->setWritable( ( userName == info->owner() && perms[ user ][ writable ] ) ||
	perms[ other ][ writable ] );
    info->setReadable( ( userName == info->owner() && perms[ user ][ readable ] ) ||
	perms[ other ][ readable ] );

    int p = 0;
    if ( perms[ user ][ readable ] )
	p |= TQUrlInfo::ReadOwner;
    if ( perms[ user ][ writable ] )
	p |= TQUrlInfo::WriteOwner;
    if ( perms[ user ][ executable ] )
	p |= TQUrlInfo::ExeOwner;
    if ( perms[ group ][ readable ] )
	p |= TQUrlInfo::ReadGroup;
    if ( perms[ group ][ writable ] )
	p |= TQUrlInfo::WriteGroup;
    if ( perms[ group ][ executable ] )
	p |= TQUrlInfo::ExeGroup;
    if ( perms[ other ][ readable ] )
	p |= TQUrlInfo::ReadOther;
    if ( perms[ other ][ writable ] )
	p |= TQUrlInfo::WriteOther;
    if ( perms[ other ][ executable ] )
	p |= TQUrlInfo::ExeOther;
    info->setPermissions( p );

    // size
    tmp = lst[ 4 ];
    info->setSize( tmp.toInt() );

    // date and time
    TQTime time;
    TQString dateStr;
    dateStr += "Sun ";
    lst[ 5 ][ 0 ] = lst[ 5 ][ 0 ].upper();
    dateStr += lst[ 5 ];
    dateStr += ' ';
    dateStr += lst[ 6 ];
    dateStr += ' ';

    if ( lst[ 7 ].contains( ":" ) ) {
	time = TQTime( lst[ 7 ].left( 2 ).toInt(), lst[ 7 ].right( 2 ).toInt() );
	dateStr += TQString::number( TQDate::currentDate().year() );
    } else {
	dateStr += lst[ 7 ];
    }

    TQDate date = TQDate::fromString( dateStr );
    info->setLastModified( TQDateTime( date, time ) );

    if ( lst[ 7 ].contains( ":" ) ) {
	const int futureTolerance = 600;
	if( info->lastModified().secsTo( TQDateTime::currentDateTime() ) < -futureTolerance ) {
	    TQDateTime dt = info->lastModified();
	    TQDate d = dt.date();
	    d.setYMD(d.year()-1, d.month(), d.day());
	    dt.setDate(d);
	    info->setLastModified(dt);
	}
    }

    // name
    if ( info->isSymLink() )
	info->setName( lst[ 8 ].stripWhiteSpace() );
    else {
	TQString n;
	for ( uint i = 8; i < lst.count(); ++i )
	    n += lst[ i ] + " ";
	n = n.stripWhiteSpace();
	info->setName( n );
    }
    return true;
}

void TQFtpDTP::socketConnected()
{
#if !defined (TQ_WS_QWS)
    // Use a large send buffer to reduce the number
    // of writeBlocks when download and uploading files.
    // The actual size used here (128k) is default on most
    // Unixes.
    socket.socketDevice()->setSendBufferSize(128 * 1024);
    socket.socketDevice()->setReceiveBufferSize(128 * 1024);
#endif

    bytesDone = 0;
#if defined(TQFTPDTP_DEBUG)
    tqDebug( "TQFtpDTP::connectState( CsConnected )" );
#endif
    emit connectState( TQFtpDTP::CsConnected );
}

void TQFtpDTP::socketReadyRead()
{
    if ( pi->currentCommand().isEmpty() ) {
	socket.close();
#if defined(TQFTPDTP_DEBUG)
	tqDebug( "TQFtpDTP::connectState( CsClosed )" );
#endif
	emit connectState( TQFtpDTP::CsClosed );
	return;
    }

    if ( pi->currentCommand().startsWith("LIST") ) {
	while ( socket.canReadLine() ) {
	    TQUrlInfo i;
	    TQString line = socket.readLine();
#if defined(TQFTPDTP_DEBUG)
	    tqDebug( "TQFtpDTP read (list): '%s'", line.latin1() );
#endif
	    if ( parseDir( line, "", &i ) ) {
		emit listInfo( i );
	    } else {
		// some FTP servers don't return a 550 if the file or directory
		// does not exist, but rather write a text to the data socket
		// -- try to catch these cases
		if ( line.endsWith( "No such file or directory\r\n" ) )
		    err = line;
	    }
	}
    } else {
	if ( !is_ba && data.dev ) {
	    TQByteArray ba( socket.bytesAvailable() );
	    TQ_LONG bytesRead = socket.readBlock( ba.data(), ba.size() );
	    if ( bytesRead < 0 ) {
		// ### error handling
		return;
	    }
	    ba.resize( bytesRead );
	    bytesDone += bytesRead;
#if defined(TQFTPDTP_DEBUG)
	    tqDebug( "TQFtpDTP read: %d bytes (total %d bytes)", (int)bytesRead, bytesDone );
#endif
	    emit dataTransferProgress( bytesDone, bytesTotal );
            if (data.dev)       // make sure it wasn't deleted in the slot
                data.dev->writeBlock( ba );
	} else {
#if defined(TQFTPDTP_DEBUG)
	    tqDebug( "TQFtpDTP readyRead: %d bytes available (total %d bytes read)", (int)bytesAvailable(), bytesDone );
#endif
	    emit dataTransferProgress( bytesDone+socket.bytesAvailable(), bytesTotal );
	    emit readyRead();
	}
    }
}

void TQFtpDTP::socketError( int e )
{
    if ( e == TQSocket::ErrHostNotFound ) {
#if defined(TQFTPDTP_DEBUG)
	tqDebug( "TQFtpDTP::connectState( CsHostNotFound )" );
#endif
	emit connectState( TQFtpDTP::CsHostNotFound );
    } else if ( e == TQSocket::ErrConnectionRefused ) {
#if defined(TQFTPDTP_DEBUG)
	tqDebug( "TQFtpDTP::connectState( CsConnectionRefused )" );
#endif
	emit connectState( TQFtpDTP::CsConnectionRefused );
    }
}

void TQFtpDTP::socketConnectionClosed()
{
    if ( !is_ba && data.dev ) {
	clearData();
    }
#if defined(TQFTPDTP_DEBUG)
    tqDebug( "TQFtpDTP::connectState( CsClosed )" );
#endif
    emit connectState( TQFtpDTP::CsClosed );
}

void TQFtpDTP::socketBytesWritten( int bytes )
{
    bytesDone += bytes;
#if defined(TQFTPDTP_DEBUG)
    tqDebug( "TQFtpDTP::bytesWritten( %d )", bytesDone );
#endif
    emit dataTransferProgress( bytesDone, bytesTotal );
    if ( callWriteData )
	writeData();
}

/**********************************************************************
 *
 * TQFtpPI implemenatation
 *
 *********************************************************************/
TQFtpPI::TQFtpPI( TQObject *parent ) :
    TQObject( parent ),
    rawCommand(false),
    dtp( this ),
    commandSocket( 0, "TQFtpPI_socket" ),
    state( Begin ), abortState( None ),
    currentCmd( TQString::null ),
    waitForDtpToConnect( false ),
    waitForDtpToClose( false )
{
    connect( &commandSocket, TQ_SIGNAL(hostFound()),
	    TQ_SLOT(hostFound()) );
    connect( &commandSocket, TQ_SIGNAL(connected()),
	    TQ_SLOT(connected()) );
    connect( &commandSocket, TQ_SIGNAL(connectionClosed()),
	    TQ_SLOT(connectionClosed()) );
    connect( &commandSocket, TQ_SIGNAL(delayedCloseFinished()),
	    TQ_SLOT(delayedCloseFinished()) );
    connect( &commandSocket, TQ_SIGNAL(readyRead()),
	    TQ_SLOT(readyRead()) );
    connect( &commandSocket, TQ_SIGNAL(error(int)),
	    TQ_SLOT(error(int)) );

    connect( &dtp, TQ_SIGNAL(connectState(int)),
	     TQ_SLOT(dtpConnectState(int)) );
}

void TQFtpPI::connectToHost( const TQString &host, TQ_UINT16 port )
{
    emit connectState( TQFtp::HostLookup );
    commandSocket.connectToHost( host, port );
}

/*
  Sends the sequence of commands \a cmds to the FTP server. When the commands
  are all done the finished() signal is emitted. When an error occurs, the
  error() signal is emitted.

  If there are pending commands in the queue this functions returns false and
  the \a cmds are not added to the queue; otherwise it returns true.
*/
bool TQFtpPI::sendCommands( const TQStringList &cmds )
{
    if ( !pendingCommands.isEmpty() )
	return false;

    if ( commandSocket.state()!=TQSocket::Connected || state!=Idle ) {
	emit error( TQFtp::NotConnected, TQFtp::tr( "Not connected" ) );
	return true; // there are no pending commands
    }

    pendingCommands = cmds;
    startNextCmd();
    return true;
}

void TQFtpPI::clearPendingCommands()
{
    pendingCommands.clear();
    dtp.abortConnection();
    currentCmd = TQString::null;
    state = Idle;
}

void TQFtpPI::abort()
{
    pendingCommands.clear();

    if ( abortState != None )
	// ABOR already sent
	return;

    abortState = AbortStarted;
#if defined(TQFTPPI_DEBUG)
    tqDebug( "TQFtpPI send: ABOR" );
#endif
    commandSocket.writeBlock( "ABOR\r\n", 6 );

    if ( currentCmd.startsWith("STOR ") )
	dtp.abortConnection();
}

void TQFtpPI::hostFound()
{
    emit connectState( TQFtp::Connecting );
}

void TQFtpPI::connected()
{
    state = Begin;
#if defined(TQFTPPI_DEBUG)
//    tqDebug( "TQFtpPI state: %d [connected()]", state );
#endif
    emit connectState( TQFtp::Connected );
}

void TQFtpPI::connectionClosed()
{
    commandSocket.close();
    emit connectState( TQFtp::Unconnected );
}

void TQFtpPI::delayedCloseFinished()
{
    emit connectState( TQFtp::Unconnected );
}

void TQFtpPI::error( int e )
{
    if ( e == TQSocket::ErrHostNotFound ) {
	emit connectState( TQFtp::Unconnected );
	emit error( TQFtp::HostNotFound,
		    TQFtp::tr( "Host %1 not found" ).arg( commandSocket.peerName() ) );
    } else if ( e == TQSocket::ErrConnectionRefused ) {
	emit connectState( TQFtp::Unconnected );
	emit error( TQFtp::ConnectionRefused,
		    TQFtp::tr( "Connection refused to host %1" ).arg( commandSocket.peerName() ) );
    }
}

void TQFtpPI::readyRead()
{
    if ( waitForDtpToClose )
	return;

    while ( commandSocket.canReadLine() ) {
	// read line with respect to line continuation
	TQString line = commandSocket.readLine();
	if ( replyText.isEmpty() ) {
	    if ( line.length() < 3 ) {
		// ### protocol error
		return;
	    }
	    const int lowerLimit[3] = {1,0,0};
	    const int upperLimit[3] = {5,5,9};
	    for ( int i=0; i<3; i++ ) {
		replyCode[i] = line[i].digitValue();
		if ( replyCode[i]<lowerLimit[i] || replyCode[i]>upperLimit[i] ) {
		    // ### protocol error
		    return;
		}
	    }
	}
	TQString endOfMultiLine;
	endOfMultiLine[0] = '0' + replyCode[0];
	endOfMultiLine[1] = '0' + replyCode[1];
	endOfMultiLine[2] = '0' + replyCode[2];
	endOfMultiLine[3] = ' ';
	TQString lineCont( endOfMultiLine );
	lineCont[3] = '-';
	TQString lineLeft4 = line.left(4);

	while ( lineLeft4 != endOfMultiLine ) {
	    if ( lineLeft4 == lineCont )
		replyText += line.mid( 4 ); // strip 'xyz-'
	    else
		replyText += line;
	    if ( !commandSocket.canReadLine() )
		return;
	    line = commandSocket.readLine();
	    lineLeft4 = line.left(4);
	}
	replyText += line.mid( 4 ); // strip reply code 'xyz '
	if ( replyText.endsWith("\r\n") )
	    replyText.truncate( replyText.length()-2 );

	if ( processReply() )
	    replyText = "";
    }
}

/*
  Process a reply from the FTP server.

  Returns true if the reply was processed or false if the reply has to be
  processed at a later point.
*/
bool TQFtpPI::processReply()
{
#if defined(TQFTPPI_DEBUG)
//    tqDebug( "TQFtpPI state: %d [processReply() begin]", state );
    if ( replyText.length() < 400 )
	tqDebug( "TQFtpPI recv: %d %s", 100*replyCode[0]+10*replyCode[1]+replyCode[2], replyText.latin1() );
    else
	tqDebug( "TQFtpPI recv: %d (text skipped)", 100*replyCode[0]+10*replyCode[1]+replyCode[2] );
#endif

    // process 226 replies ("Closing Data Connection") only when the data
    // connection is really closed to avoid short reads of the DTP
    if ( 100*replyCode[0]+10*replyCode[1]+replyCode[2] == 226 ) {
	if ( dtp.socketState() != TQSocket::Idle ) {
	    waitForDtpToClose = true;
	    return false;
	}
    }

    switch ( abortState ) {
	case AbortStarted:
	    abortState = WaitForAbortToFinish;
	    break;
	case WaitForAbortToFinish:
	    abortState = None;
	    return true;
	default:
	    break;
    }

    // get new state
    static const State table[5] = {
	/* 1yz   2yz      3yz   4yz      5yz */
	Waiting, Success, Idle, Failure, Failure
    };
    switch ( state ) {
	case Begin:
	    if ( replyCode[0] == 1 ) {
		return true;
	    } else if ( replyCode[0] == 2 ) {
		state = Idle;
		emit finished( TQFtp::tr( "Connected to host %1" ).arg( commandSocket.peerName() ) );
		break;
	    }
	    // ### error handling
	    return true;
	case Waiting:
	    if ( replyCode[0]<0 || replyCode[0]>5 )
		state = Failure;
	    else
		state = table[ replyCode[0] - 1 ];
	    break;
	default:
	    // ### spontaneous message
	    return true;
    }
#if defined(TQFTPPI_DEBUG)
//    tqDebug( "TQFtpPI state: %d [processReply() intermediate]", state );
#endif

    // special actions on certain replies
    int replyCodeInt = 100*replyCode[0] + 10*replyCode[1] + replyCode[2];
    emit rawFtpReply( replyCodeInt, replyText );
    if ( rawCommand ) {
	rawCommand = false;
    } else if ( replyCodeInt == 227 ) {
	// 227 Entering Passive Mode (h1,h2,h3,h4,p1,p2)
        // rfc959 does not define this response precisely, and gives
        // both examples where the parenthesis are used, and where
        // they are missing. We need to scan for the address and host
        // info.
	TQRegExp addrPortPattern("(\\d+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+)");
	if (addrPortPattern.search(replyText) == -1) {
#if defined(TQFTPPI_DEBUG)
	    tqDebug( "TQFtp: bad 227 response -- address and port information missing" );
#endif
	    // ### error handling
	} else {
	    TQStringList lst = addrPortPattern.capturedTexts();
	    TQString host = lst[1] + "." + lst[2] + "." + lst[3] + "." + lst[4];
	    TQ_UINT16 port = ( lst[5].toUInt() << 8 ) + lst[6].toUInt();
	    waitForDtpToConnect = true;
	    dtp.connectToHost( host, port );
	}
    } else if ( replyCodeInt == 230 ) {
	if ( currentCmd.startsWith("USER ") && pendingCommands.count()>0 &&
		pendingCommands.first().startsWith("PASS ") ) {
	    // no need to send the PASS -- we are already logged in
	    pendingCommands.pop_front();
	}
	// 230 User logged in, proceed.
	emit connectState( TQFtp::LoggedIn );
    } else if ( replyCodeInt == 213 ) {
	// 213 File status.
	if ( currentCmd.startsWith("SIZE ") )
	    dtp.setBytesTotal( replyText.simplifyWhiteSpace().toInt() );
    } else if ( replyCode[0]==1 && currentCmd.startsWith("STOR ") ) {
	dtp.writeData();
    }

    // react on new state
    switch ( state ) {
	case Begin:
	    // ### should never happen
	    break;
	case Success:
	    // ### success handling
	    state = Idle;
	    // no break!
	case Idle:
	    if ( dtp.hasError() ) {
		emit error( TQFtp::UnknownError, dtp.errorMessage() );
		dtp.clearError();
	    }
	    startNextCmd();
	    break;
	case Waiting:
	    // ### do nothing
	    break;
	case Failure:
	    emit error( TQFtp::UnknownError, replyText );
	    state = Idle;
	    startNextCmd();
	    break;
    }
#if defined(TQFTPPI_DEBUG)
//    tqDebug( "TQFtpPI state: %d [processReply() end]", state );
#endif
    return true;
}

#ifndef TQT_NO_TEXTCODEC
TQM_EXPORT_FTP TQTextCodec *tqt_ftp_filename_codec = 0;
#endif

/*
  Starts next pending command. Returns false if there are no pending commands,
  otherwise it returns true.
*/
bool TQFtpPI::startNextCmd()
{
    if ( waitForDtpToConnect )
	// don't process any new commands until we are connected
	return true;

#if defined(TQFTPPI_DEBUG)
    if ( state != Idle )
	tqDebug( "TQFtpPI startNextCmd: Internal error! TQFtpPI called in non-Idle state %d", state );
#endif
    if ( pendingCommands.isEmpty() ) {
	currentCmd = TQString::null;
	emit finished( replyText );
	return false;
    }
    currentCmd = pendingCommands.first();
    pendingCommands.pop_front();
#if defined(TQFTPPI_DEBUG)
    tqDebug( "TQFtpPI send: %s", currentCmd.left( currentCmd.length()-2 ).latin1() );
#endif
    state = Waiting;
#ifndef TQT_NO_TEXTCODEC
    if ( tqt_ftp_filename_codec ) {
	int len = 0;
	TQCString enc = tqt_ftp_filename_codec->fromUnicode(currentCmd,len);
	commandSocket.writeBlock( enc.data(), len );
    } else
#endif
    {
	commandSocket.writeBlock( currentCmd.latin1(), currentCmd.length() );
    }
    return true;
}

void TQFtpPI::dtpConnectState( int s )
{
    switch ( s ) {
	case TQFtpDTP::CsClosed:
	    if ( waitForDtpToClose ) {
		// there is an unprocessed reply
		if ( processReply() )
		    replyText = "";
		else
		    return;
	    }
	    waitForDtpToClose = false;
	    readyRead();
	    return;
	case TQFtpDTP::CsConnected:
	    waitForDtpToConnect = false;
	    startNextCmd();
	    return;
	case TQFtpDTP::CsHostNotFound:
	case TQFtpDTP::CsConnectionRefused:
	    emit error( TQFtp::ConnectionRefused,
			TQFtp::tr( "Connection refused for data connection" ) );
	    startNextCmd();
	    return;
	default:
	    return;
    }
}

/**********************************************************************
 *
 * TQFtpPrivate
 *
 *********************************************************************/
class TQFtpPrivate
{
public:
    TQFtpPrivate() :
	close_waitForStateChange(false),
	state( TQFtp::Unconnected ),
	error( TQFtp::NoError ),
	npWaitForLoginDone( false )
    { pending.setAutoDelete( true ); }

    TQFtpPI pi;
    TQPtrList<TQFtpCommand> pending;
    bool close_waitForStateChange;
    TQFtp::State state;
    TQFtp::Error error;
    TQString errorString;

    bool npWaitForLoginDone;
};

static TQPtrDict<TQFtpPrivate> *d_ptr = 0;
static void cleanup_d_ptr()
{
    delete d_ptr;
    d_ptr = 0;
}
static TQFtpPrivate* d( const TQFtp* foo )
{
    if ( !d_ptr ) {
	d_ptr = new TQPtrDict<TQFtpPrivate>;
	d_ptr->setAutoDelete( true );
	tqAddPostRoutine( cleanup_d_ptr );
    }
    TQFtpPrivate* ret = d_ptr->find( (void*)foo );
    if ( ! ret ) {
	ret = new TQFtpPrivate;
	d_ptr->replace( (void*) foo, ret );
    }
    return ret;
}

static void delete_d( const TQFtp* foo )
{
    if ( d_ptr )
	d_ptr->remove( (void*) foo );
}

/**********************************************************************
 *
 * TQFtp implementation
 *
 *********************************************************************/
/*!
    \class TQFtp ntqftp.h
    \brief The TQFtp class provides an implementation of the FTP protocol.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup io
    \module network

    This class provides two different interfaces: one is the
    TQNetworkProtocol interface that allows you to use FTP through the
    TQUrlOperator abstraction. The other is a direct interface to FTP
    that gives you lower-level access to the FTP protocol for finer
    control. Using the direct interface you can also execute arbitrary
    FTP commands.

    Don't mix the two interfaces, since the behavior is not
    well-defined.

    If you want to use TQFtp with the TQNetworkProtocol interface, you
    do not use it directly, but rather through a TQUrlOperator, for
    example:

    \code
    TQUrlOperator op( "ftp://ftp.trolltech.com" );
    op.listChildren(); // Asks the server to provide a directory listing
    \endcode

    This code will only work if the TQFtp class is registered; to
    register the class, you must call tqInitNetworkProtocols() before
    using a TQUrlOperator with TQFtp.

    The rest of this descrption describes the direct interface to FTP.

    The class works asynchronously, so there are no blocking
    functions. If an operation cannot be executed immediately, the
    function will still return straight away and the operation will be
    scheduled for later execution. The results of scheduled operations
    are reported via signals. This approach depends on the event loop
    being in operation.

    The operations that can be scheduled (they are called "commands"
    in the rest of the documentation) are the following:
    connectToHost(), login(), close(), list(), cd(), get(), put(),
    remove(), mkdir(), rmdir(), rename() and rawCommand().

    All of these commands return a unique identifier that allows you
    to keep track of the command that is currently being executed.
    When the execution of a command starts, the commandStarted()
    signal with the command's identifier is emitted. When the command
    is finished, the commandFinished() signal is emitted with the
    command's identifier and a bool that indicates whether the command
    finished with an error.

    In some cases, you might want to execute a sequence of commands,
    e.g. if you want to connect and login to a FTP server. This is
    simply achieved:

    \code
    TQFtp *ftp = new TQFtp( this ); // this is an optional TQObject parent
    ftp->connectToHost( "ftp.trolltech.com" );
    ftp->login();
    \endcode

    In this case two FTP commands have been scheduled. When the last
    scheduled command has finished, a done() signal is emitted with
    a bool argument that tells you whether the sequence finished with
    an error.

    If an error occurs during the execution of one of the commands in
    a sequence of commands, all the pending commands (i.e. scheduled,
    but not yet executed commands) are cleared and no signals are
    emitted for them.

    Some commands, e.g. list(), emit additional signals to report
    their results.

    Example: If you want to download the INSTALL file from Trolltech's
    FTP server, you would write this:

    \code
    ftp->connectToHost( "ftp.trolltech.com" );  // id == 1
    ftp->login();                               // id == 2
    ftp->cd( "qt" );                            // id == 3
    ftp->get( "INSTALL" );                      // id == 4
    ftp->close();                               // id == 5
    \endcode

    For this example the following sequence of signals is emitted
    (with small variations, depending on network traffic, etc.):

    \code
    start( 1 )
    stateChanged( HostLookup )
    stateChanged( Connecting )
    stateChanged( Connected )
    finished( 1, false )

    start( 2 )
    stateChanged( LoggedIn )
    finished( 2, false )

    start( 3 )
    finished( 3, false )

    start( 4 )
    dataTransferProgress( 0, 3798 )
    dataTransferProgress( 2896, 3798 )
    readyRead()
    dataTransferProgress( 3798, 3798 )
    readyRead()
    finished( 4, false )

    start( 5 )
    stateChanged( Closing )
    stateChanged( Unconnected )
    finished( 5, false )

    done( false )
    \endcode

    The dataTransferProgress() signal in the above example is useful
    if you want to show a \link TQProgressBar progressbar \endlink to
    inform the user about the progress of the download. The
    readyRead() signal tells you that there is data ready to be read.
    The amount of data can be queried then with the bytesAvailable()
    function and it can be read with the readBlock() or readAll()
    function.

    If the login fails for the above example, the signals would look
    like this:

    \code
    start( 1 )
    stateChanged( HostLookup )
    stateChanged( Connecting )
    stateChanged( Connected )
    finished( 1, false )

    start( 2 )
    finished( 2, true )

    done( true )
    \endcode

    You can then get details about the error with the error() and
    errorString() functions.

    The functions currentId() and currentCommand() provide more
    information about the currently executing command.

    The functions hasPendingCommands() and clearPendingCommands()
    allow you to query and clear the list of pending commands.

    The safest and easiest way to use the FTP protocol is to use
    TQUrlOperator() or the FTP commands described above. If you are an
    experienced network programmer and want to have complete control
    you can use rawCommand() to execute arbitrary FTP commands.

    \sa \link network.html TQt Network Documentation \endlink TQNetworkProtocol, TQUrlOperator TQHttp
*/

/*!
    Constructs a TQFtp object.
*/
TQFtp::TQFtp() : TQNetworkProtocol()
{
    init();
}

/*!
    Constructs a TQFtp object. The \a parent and \a name parameters
    are passed to the TQObject constructor.
*/
TQFtp::TQFtp( TQObject *parent, const char *name ) : TQNetworkProtocol()
{
    if ( parent )
	parent->insertChild( this );
    setName( name );
    init();
}

void TQFtp::init()
{
    TQFtpPrivate *d = ::d( this );
    d->errorString = tr( "Unknown error" );

    connect( &d->pi, TQ_SIGNAL(connectState(int)),
	    TQ_SLOT(piConnectState(int)) );
    connect( &d->pi, TQ_SIGNAL(finished(const TQString&)),
	    TQ_SLOT(piFinished(const TQString&)) );
    connect( &d->pi, TQ_SIGNAL(error(int,const TQString&)),
	    TQ_SLOT(piError(int,const TQString&)) );
    connect( &d->pi, TQ_SIGNAL(rawFtpReply(int,const TQString&)),
	    TQ_SLOT(piFtpReply(int,const TQString&)) );

    connect( &d->pi.dtp, TQ_SIGNAL(readyRead()),
	    TQ_SIGNAL(readyRead()) );
    connect( &d->pi.dtp, TQ_SIGNAL(dataTransferProgress(int,int)),
	    TQ_SIGNAL(dataTransferProgress(int,int)) );
    connect( &d->pi.dtp, TQ_SIGNAL(listInfo(const TQUrlInfo&)),
	    TQ_SIGNAL(listInfo(const TQUrlInfo&)) );
}

/*!
    \enum TQFtp::State

    This enum defines the connection state:

    \value Unconnected There is no connection to the host.
    \value HostLookup A host name lookup is in progress.
    \value Connecting An attempt to connect to the host is in progress.
    \value Connected Connection to the host has been achieved.
    \value LoggedIn Connection and user login have been achieved.
    \value Closing The connection is closing down, but it is not yet
    closed. (The state will be \c Unconnected when the connection is
    closed.)

    \sa stateChanged() state()
*/
/*!
    \enum TQFtp::Error

    This enum identifies the error that occurred.

    \value NoError No error occurred.
    \value HostNotFound The host name lookup failed.
    \value ConnectionRefused The server refused the connection.
    \value NotConnected Tried to send a command, but there is no connection to
    a server.
    \value UnknownError An error other than those specified above
    occurred.

    \sa error()
*/

/*!
    \enum TQFtp::Command

    This enum is used as the return value for the currentCommand() function.
    This allows you to perform specific actions for particular
    commands, e.g. in a FTP client, you might want to clear the
    directory view when a list() command is started; in this case you
    can simply check in the slot connected to the start() signal if
    the currentCommand() is \c List.

    \value None No command is being executed.
    \value ConnectToHost connectToHost() is being executed.
    \value Login login() is being executed.
    \value Close close() is being executed.
    \value List list() is being executed.
    \value Cd cd() is being executed.
    \value Get get() is being executed.
    \value Put put() is being executed.
    \value Remove remove() is being executed.
    \value Mkdir mkdir() is being executed.
    \value Rmdir rmdir() is being executed.
    \value Rename rename() is being executed.
    \value RawCommand rawCommand() is being executed.

    \sa currentCommand()
*/

/*!
    \fn void TQFtp::stateChanged( int state )

    This signal is emitted when the state of the connection changes.
    The argument \a state is the new state of the connection; it is
    one of the \l State values.

    It is usually emitted in response to a connectToHost() or close()
    command, but it can also be emitted "spontaneously", e.g. when the
    server closes the connection unexpectedly.

    \sa connectToHost() close() state() State
*/

/*!
    \fn void TQFtp::listInfo( const TQUrlInfo &i );

    This signal is emitted for each directory entry the list() command
    finds. The details of the entry are stored in \a i.

    \sa list()
*/

/*!
    \fn void TQFtp::commandStarted( int id )

    This signal is emitted when processing the command identified by
    \a id starts.

    \sa commandFinished() done()
*/

/*!
    \fn void TQFtp::commandFinished( int id, bool error )

    This signal is emitted when processing the command identified by
    \a id has finished. \a error is true if an error occurred during
    the processing; otherwise \a error is false.

    \sa commandStarted() done() error() errorString()
*/

/*!
    \fn void TQFtp::done( bool error )

    This signal is emitted when the last pending command has finished;
    (it is emitted after the last command's commandFinished() signal).
    \a error is true if an error occurred during the processing;
    otherwise \a error is false.

    \sa commandFinished() error() errorString()
*/

/*!
    \fn void TQFtp::readyRead()

    This signal is emitted in response to a get() command when there
    is new data to read.

    If you specify a device as the second argument in the get()
    command, this signal is \e not emitted; instead the data is
    written directly to the device.

    You can read the data with the readAll() or readBlock() functions.

    This signal is useful if you want to process the data in chunks as
    soon as it becomes available. If you are only interested in the
    complete data, just connect to the commandFinished() signal and
    read the data then instead.

    \sa get() readBlock() readAll() bytesAvailable()
*/

/*!
    \fn void TQFtp::dataTransferProgress( int done, int total )

    This signal is emitted in response to a get() or put() request to
    indicate the current progress of the download or upload.

    \a done is the amount of data that has already been transferred
    and \a total is the total amount of data to be read or written. It
    is possible that the TQFtp class is not able to determine the total
    amount of data that should be transferred, in which case \a total
    is 0. (If you connect this signal to a TQProgressBar, the progress
    bar shows a busy indicator if the total is 0).

    \warning \a done and \a total are not necessarily the size in
    bytes, since for large files these values might need to be
    "scaled" to avoid overflow.

    \sa get() put() TQProgressBar::setProgress()
*/

/*!
    \fn void TQFtp::rawCommandReply( int replyCode, const TQString &detail );

    This signal is emitted in response to the rawCommand() function.
    \a replyCode is the 3 digit reply code and \a detail is the text
    that follows the reply code.

    \sa rawCommand()
*/

/*!
    Connects to the FTP server \a host using port \a port.

    The stateChanged() signal is emitted when the state of the
    connecting process changes, e.g. to \c HostLookup, then \c
    Connecting, then \c Connected.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa stateChanged() commandStarted() commandFinished()
*/
int TQFtp::connectToHost( const TQString &host, TQ_UINT16 port )
{
    TQStringList cmds;
    cmds << host;
    cmds << TQString::number( (uint)port );
    return addCommand( new TQFtpCommand( ConnectToHost, cmds ) );
}

/*!
    Logs in to the FTP server with the username \a user and the
    password \a password.

    The stateChanged() signal is emitted when the state of the
    connecting process changes, e.g. to \c LoggedIn.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int TQFtp::login( const TQString &user, const TQString &password )
{
    TQStringList cmds;
    cmds << ( TQString("USER ") + ( user.isNull() ? TQString("anonymous") : user ) + "\r\n" );
    cmds << ( TQString("PASS ") + ( password.isNull() ? TQString("anonymous@") : password ) + "\r\n" );
    return addCommand( new TQFtpCommand( Login, cmds ) );
}

/*!
    Closes the connection to the FTP server.

    The stateChanged() signal is emitted when the state of the
    connecting process changes, e.g. to \c Closing, then \c
    Unconnected.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa stateChanged() commandStarted() commandFinished()
*/
int TQFtp::close()
{
    return addCommand( new TQFtpCommand( Close, TQStringList("QUIT\r\n") ) );
}

/*!
    Lists the contents of directory \a dir on the FTP server. If \a
    dir is empty, it lists the contents of the current directory.

    The listInfo() signal is emitted for each directory entry found.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa listInfo() commandStarted() commandFinished()
*/
int TQFtp::list( const TQString &dir )
{
    TQStringList cmds;
    cmds << "TYPE A\r\n";
    cmds << "PASV\r\n";
    if ( dir.isEmpty() )
	cmds << "LIST\r\n";
    else
	cmds << ( "LIST " + dir + "\r\n" );
    return addCommand( new TQFtpCommand( List, cmds ) );
}

/*!
    Changes the working directory of the server to \a dir.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int TQFtp::cd( const TQString &dir )
{
    return addCommand( new TQFtpCommand( Cd, TQStringList("CWD "+dir+"\r\n") ) );
}

/*!
    Downloads the file \a file from the server.

    If \a dev is 0, then the readyRead() signal is emitted when there
    is data available to read. You can then read the data with the
    readBlock() or readAll() functions.

    If \a dev is not 0, the data is written directly to the device \a
    dev. Make sure that the \a dev pointer is valid for the duration
    of the operation (it is safe to delete it when the
    commandFinished() signal is emitted). In this case the readyRead()
    signal is \e not emitted and you cannot read data with the
    readBlock() or readAll() functions.

    If you don't read the data immediately it becomes available, i.e.
    when the readyRead() signal is emitted, it is still available
    until the next command is started.

    For example, if you want to present the data to the user as soon
    as there is something available, connect to the readyRead() signal
    and read the data immediately. On the other hand, if you only want
    to work with the complete data, you can connect to the
    commandFinished() signal and read the data when the get() command
    is finished.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa readyRead() dataTransferProgress() commandStarted()
    commandFinished()
*/
int TQFtp::get( const TQString &file, TQIODevice *dev )
{
    TQStringList cmds;
    cmds << ( "SIZE " + file + "\r\n" );
    cmds << "TYPE I\r\n";
    cmds << "PASV\r\n";
    cmds << ( "RETR " + file + "\r\n" );
    if ( dev )
	return addCommand( new TQFtpCommand( Get, cmds, dev ) );
    return addCommand( new TQFtpCommand( Get, cmds ) );
}

/*!
    \overload

    Writes the data \a data to the file called \a file on the server.
    The progress of the upload is reported by the
    dataTransferProgress() signal.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa dataTransferProgress() commandStarted() commandFinished()
*/
int TQFtp::put( const TQByteArray &data, const TQString &file )
{
    TQStringList cmds;
    cmds << "TYPE I\r\n";
    cmds << "PASV\r\n";
    cmds << ( "ALLO " + TQString::number(data.size()) + "\r\n" );
    cmds << ( "STOR " + file + "\r\n" );
    return addCommand( new TQFtpCommand( Put, cmds, data ) );
}

/*!
    Reads the data from the IO device \a dev, and writes it to the
    file called \a file on the server. The data is read in chunks from
    the IO device, so this overload allows you to transmit large
    amounts of data without the need to read all the data into memory
    at once.

    Make sure that the \a dev pointer is valid for the duration of the
    operation (it is safe to delete it when the commandFinished() is
    emitted).
*/
int TQFtp::put( TQIODevice *dev, const TQString &file )
{
    TQStringList cmds;
    cmds << "TYPE I\r\n";
    cmds << "PASV\r\n";
    if ( !dev->isSequentialAccess() )
	cmds << ( "ALLO " + TQString::number(dev->size()) + "\r\n" );
    cmds << ( "STOR " + file + "\r\n" );
    return addCommand( new TQFtpCommand( Put, cmds, dev ) );
}

/*!
    Deletes the file called \a file from the server.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int TQFtp::remove( const TQString &file )
{
    return addCommand( new TQFtpCommand( Remove, TQStringList("DELE "+file+"\r\n") ) );
}

/*!
    Creates a directory called \a dir on the server.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int TQFtp::mkdir( const TQString &dir )
{
    return addCommand( new TQFtpCommand( Mkdir, TQStringList("MKD "+dir+"\r\n") ) );
}

/*!
    Removes the directory called \a dir from the server.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int TQFtp::rmdir( const TQString &dir )
{
    return addCommand( new TQFtpCommand( Rmdir, TQStringList("RMD "+dir+"\r\n") ) );
}

/*!
    Renames the file called \a oldname to \a newname on the server.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa commandStarted() commandFinished()
*/
int TQFtp::rename( const TQString &oldname, const TQString &newname )
{
    TQStringList cmds;
    cmds << ( "RNFR " + oldname + "\r\n" );
    cmds << ( "RNTO " + newname + "\r\n" );
    return addCommand( new TQFtpCommand( Rename, cmds ) );
}

/*!
    Sends the raw FTP command \a command to the FTP server. This is
    useful for low-level FTP access. If the operation you wish to
    perform has an equivalent TQFtp function, we recommend using the
    function instead of raw FTP commands since the functions are
    easier and safer.

    The function does not block and returns immediately. The command
    is scheduled, and its execution is performed asynchronously. The
    function returns a unique identifier which is passed by
    commandStarted() and commandFinished().

    When the command is started the commandStarted() signal is
    emitted. When it is finished the commandFinished() signal is
    emitted.

    \sa rawCommandReply() commandStarted() commandFinished()
*/
int TQFtp::rawCommand( const TQString &command )
{
    TQString cmd = command.stripWhiteSpace() + "\r\n";
    return addCommand( new TQFtpCommand( RawCommand, TQStringList(cmd) ) );
}

/*!
    Returns the number of bytes that can be read from the data socket
    at the moment.

    \sa get() readyRead() readBlock() readAll()
*/
TQ_ULONG TQFtp::bytesAvailable() const
{
    TQFtpPrivate *d = ::d( this );
    return d->pi.dtp.bytesAvailable();
}

/*!
    Reads \a maxlen bytes from the data socket into \a data and
    returns the number of bytes read. Returns -1 if an error occurred.

    \sa get() readyRead() bytesAvailable() readAll()
*/
TQ_LONG TQFtp::readBlock( char *data, TQ_ULONG maxlen )
{
    TQFtpPrivate *d = ::d( this );
    return d->pi.dtp.readBlock( data, maxlen );
}

/*!
    Reads all the bytes available from the data socket and returns
    them.

    \sa get() readyRead() bytesAvailable() readBlock()
*/
TQByteArray TQFtp::readAll()
{
    TQFtpPrivate *d = ::d( this );
    return d->pi.dtp.readAll();
}

/*!
    Aborts the current command and deletes all scheduled commands.

    If there is an unfinished command (i.e. a command for which the
    commandStarted() signal has been emitted, but for which the
    commandFinished() signal has not been emitted), this function
    sends an \c ABORT command to the server. When the server replies
    that the command is aborted, the commandFinished() signal with the
    \c error argument set to \c true is emitted for the command. Due
    to timing issues, it is possible that the command had already
    finished before the abort request reached the server, in which
    case, the commandFinished() signal is emitted with the \c error
    argument set to \c false.

    For all other commands that are affected by the abort(), no
    signals are emitted.

    If you don't start further FTP commands directly after the
    abort(), there won't be any scheduled commands and the done()
    signal is emitted.

    \warning Some FTP servers, for example the BSD FTP daemon (version
    0.3), wrongly return a positive reply even when an abort has
    occurred. For these servers the commandFinished() signal has its
    error flag set to \c false, even though the command did not
    complete successfully.

    \sa clearPendingCommands()
*/
void TQFtp::abort()
{
    TQFtpPrivate *d = ::d( this );
    if ( d->pending.isEmpty() )
	return;

    clearPendingCommands();
    d->pi.abort();
}

/*!
    Returns the identifier of the FTP command that is being executed
    or 0 if there is no command being executed.

    \sa currentCommand()
*/
int TQFtp::currentId() const
{
    TQFtpPrivate *d = ::d( this );
    TQFtpCommand *c = d->pending.getFirst();
    if ( c == 0 )
	return 0;
    return c->id;
}

/*!
    Returns the command type of the FTP command being executed or \c
    None if there is no command being executed.

    \sa currentId()
*/
TQFtp::Command TQFtp::currentCommand() const
{
    TQFtpPrivate *d = ::d( this );
    TQFtpCommand *c = d->pending.getFirst();
    if ( c == 0 )
	return None;
    return c->command;
}

/*!
    Returns the TQIODevice pointer that is used by the FTP command to read data
    from or store data to. If there is no current FTP command being executed or
    if the command does not use an IO device, this function returns 0.

    This function can be used to delete the TQIODevice in the slot connected to
    the commandFinished() signal.

    \sa get() put()
*/
TQIODevice* TQFtp::currentDevice() const
{
    TQFtpPrivate *d = ::d( this );
    TQFtpCommand *c = d->pending.getFirst();
    if ( !c )
	return 0;
    if ( c->is_ba )
	return 0;
    return c->data.dev;
}

/*!
    Returns true if there are any commands scheduled that have not yet
    been executed; otherwise returns false.

    The command that is being executed is \e not considered as a
    scheduled command.

    \sa clearPendingCommands() currentId() currentCommand()
*/
bool TQFtp::hasPendingCommands() const
{
    TQFtpPrivate *d = ::d( this );
    return d->pending.count() > 1;
}

/*!
    Deletes all pending commands from the list of scheduled commands.
    This does not affect the command that is being executed. If you
    want to stop this this as well, use abort().

    \sa hasPendingCommands() abort()
*/
void TQFtp::clearPendingCommands()
{
    TQFtpPrivate *d = ::d( this );
    TQFtpCommand *c = 0;
    if ( d->pending.count() > 0 )
	c = d->pending.take( 0 );
    d->pending.clear();
    if ( c )
	d->pending.append( c );
}

/*!
    Returns the current state of the object. When the state changes,
    the stateChanged() signal is emitted.

    \sa State stateChanged()
*/
TQFtp::State TQFtp::state() const
{
    TQFtpPrivate *d = ::d( this );
    return d->state;
}

/*!
    Returns the last error that occurred. This is useful to find out
    what when wrong when receiving a commandFinished() or a done()
    signal with the \c error argument set to \c true.

    If you start a new command, the error status is reset to \c NoError.
*/
TQFtp::Error TQFtp::error() const
{
    TQFtpPrivate *d = ::d( this );
    return d->error;
}

/*!
    Returns a human-readable description of the last error that
    occurred. This is useful for presenting a error message to the
    user when receiving a commandFinished() or a done() signal with
    the \c error argument set to \c true.

    The error string is often (but not always) the reply from the
    server, so it is not always possible to translate the string. If
    the message comes from TQt, the string has already passed through
    tr().
*/
TQString TQFtp::errorString() const
{
    TQFtpPrivate *d = ::d( this );
    return d->errorString;
}

int TQFtp::addCommand( TQFtpCommand *cmd )
{
    TQFtpPrivate *d = ::d( this );
    d->pending.append( cmd );

    if ( d->pending.count() == 1 )
	// don't emit the commandStarted() signal before the id is returned
	TQTimer::singleShot( 0, this, TQ_SLOT(startNextCommand()) );

    return cmd->id;
}

void TQFtp::startNextCommand()
{
    TQFtpPrivate *d = ::d( this );

    TQFtpCommand *c = d->pending.getFirst();
    if ( c == 0 )
	return;

    d->error = NoError;
    d->errorString = tr( "Unknown error" );

    if ( bytesAvailable() )
	readAll(); // clear the data
    emit commandStarted( c->id );

    if ( c->command == ConnectToHost ) {
	d->pi.connectToHost( c->rawCmds[0], c->rawCmds[1].toUInt() );
    } else {
	if ( c->command == Put ) {
	    if ( c->is_ba ) {
		d->pi.dtp.setData( c->data.ba );
		d->pi.dtp.setBytesTotal( c->data.ba->size() );
	    } else if ( c->data.dev && (c->data.dev->isOpen() || c->data.dev->open(IO_ReadOnly) ) ) {
		d->pi.dtp.setDevice( c->data.dev );
		if ( c->data.dev->isSequentialAccess() )
		    d->pi.dtp.setBytesTotal( 0 );
		else
		    d->pi.dtp.setBytesTotal( c->data.dev->size() );
	    }
	} else if ( c->command == Get ) {
	    if ( !c->is_ba && c->data.dev ) {
		d->pi.dtp.setDevice( c->data.dev );
	    }
	} else if ( c->command == Close ) {
	    d->state = TQFtp::Closing;
	    emit stateChanged( d->state );
	}
	if ( !d->pi.sendCommands( c->rawCmds ) ) {
	    // ### error handling (this case should not happen)
	}
    }
}

void TQFtp::piFinished( const TQString& )
{
    TQFtpPrivate *d = ::d( this );
    TQFtpCommand *c = d->pending.getFirst();
    if ( c == 0 )
	return;

    if ( c->command == Close ) {
	// The order of in which the slots are called is arbitrary, so
	// disconnect the TQ_SIGNAL-TQ_SIGNAL temporary to make sure that we
	// don't get the commandFinished() signal before the stateChanged()
	// signal.
	if ( d->state != TQFtp::Unconnected ) {
	    d->close_waitForStateChange = true;
	    return;
	}
    }
    emit commandFinished( c->id, false );

    d->pending.removeFirst();
    if ( d->pending.isEmpty() ) {
	emit done( false );
    } else {
	startNextCommand();
    }
}

void TQFtp::piError( int errorCode, const TQString &text )
{
    TQFtpPrivate *d = ::d( this );
    TQFtpCommand *c = d->pending.getFirst();

    // non-fatal errors
    if ( c->command==Get && d->pi.currentCommand().startsWith("SIZE ") ) {
	d->pi.dtp.setBytesTotal( -1 );
	return;
    } else if ( c->command==Put && d->pi.currentCommand().startsWith("ALLO ") ) {
	return;
    }

    d->error = (Error)errorCode;
    switch ( currentCommand() ) {
	case ConnectToHost:
	    d->errorString = tr( "Connecting to host failed:\n%1" ).arg( text );
	    break;
	case Login:
	    d->errorString = tr( "Login failed:\n%1" ).arg( text );
	    break;
	case List:
	    d->errorString = tr( "Listing directory failed:\n%1" ).arg( text );
	    break;
	case Cd:
	    d->errorString = tr( "Changing directory failed:\n%1" ).arg( text );
	    break;
	case Get:
	    d->errorString = tr( "Downloading file failed:\n%1" ).arg( text );
	    break;
	case Put:
	    d->errorString = tr( "Uploading file failed:\n%1" ).arg( text );
	    break;
	case Remove:
	    d->errorString = tr( "Removing file failed:\n%1" ).arg( text );
	    break;
	case Mkdir:
	    d->errorString = tr( "Creating directory failed:\n%1" ).arg( text );
	    break;
	case Rmdir:
	    d->errorString = tr( "Removing directory failed:\n%1" ).arg( text );
	    break;
	default:
	    d->errorString = text;
	    break;
    }

    d->pi.clearPendingCommands();
    clearPendingCommands();
    emit commandFinished( c->id, true );

    d->pending.removeFirst();
    if ( d->pending.isEmpty() )
	emit done( true );
    else
	startNextCommand();
}

void TQFtp::piConnectState( int state )
{
    TQFtpPrivate *d = ::d( this );
    d->state = (State)state;
    emit stateChanged( d->state );
    if ( d->close_waitForStateChange ) {
	d->close_waitForStateChange = false;
	piFinished( tr( "Connection closed" ) );
    }
}

void TQFtp::piFtpReply( int code, const TQString &text )
{
    if ( currentCommand() == RawCommand ) {
	TQFtpPrivate *d = ::d( this );
	d->pi.rawCommand = true;
	emit rawCommandReply( code, text );
    }
}

/*!
    Destructor.
*/
TQFtp::~TQFtp()
{
    abort();
    close();
    delete_d( this );
}

/**********************************************************************
 *
 * TQFtp implementation of the TQNetworkProtocol interface
 *
 *********************************************************************/
/*!  \reimp
*/
void TQFtp::operationListChildren( TQNetworkOperation *op )
{
    op->setState( StInProgress );

    cd( ( url()->path().isEmpty() ? TQString( "/" ) : url()->path() ) );
    list();
    emit start( op );
}

/*!  \reimp
*/
void TQFtp::operationMkDir( TQNetworkOperation *op )
{
    op->setState( StInProgress );

    mkdir( op->arg( 0 ) );
}

/*!  \reimp
*/
void TQFtp::operationRemove( TQNetworkOperation *op )
{
    op->setState( StInProgress );

    cd( ( url()->path().isEmpty() ? TQString( "/" ) : url()->path() ) );
    remove( TQUrl( op->arg( 0 ) ).path() );
}

/*!  \reimp
*/
void TQFtp::operationRename( TQNetworkOperation *op )
{
    op->setState( StInProgress );

    cd( ( url()->path().isEmpty() ? TQString( "/" ) : url()->path() ) );
    rename( op->arg( 0 ), op->arg( 1 ));
}

/*!  \reimp
*/
void TQFtp::operationGet( TQNetworkOperation *op )
{
    op->setState( StInProgress );

    TQUrl u( op->arg( 0 ) );
    get( u.path() );
}

/*!  \reimp
*/
void TQFtp::operationPut( TQNetworkOperation *op )
{
    op->setState( StInProgress );

    TQUrl u( op->arg( 0 ) );
    put( op->rawArg(1), u.path() );
}

/*!  \reimp
*/
bool TQFtp::checkConnection( TQNetworkOperation *op )
{
    TQFtpPrivate *d = ::d( this );
    if ( state() == Unconnected && !d->npWaitForLoginDone ) {
	connect( this, TQ_SIGNAL(listInfo(const TQUrlInfo&)),
		this, TQ_SLOT(npListInfo(const TQUrlInfo&)) );
	connect( this, TQ_SIGNAL(done(bool)),
		this, TQ_SLOT(npDone(bool)) );
	connect( this, TQ_SIGNAL(stateChanged(int)),
		this, TQ_SLOT(npStateChanged(int)) );
	connect( this, TQ_SIGNAL(dataTransferProgress(int,int)),
		this, TQ_SLOT(npDataTransferProgress(int,int)) );
	connect( this, TQ_SIGNAL(readyRead()),
		this, TQ_SLOT(npReadyRead()) );

	d->npWaitForLoginDone = true;
	switch ( op->operation() ) {
	    case OpGet:
	    case OpPut:
		{
		    TQUrl u( op->arg( 0 ) );
		    connectToHost( u.host(), u.port() != -1 ? u.port() : 21 );
		}
		break;
	    default:
		connectToHost( url()->host(), url()->port() != -1 ? url()->port() : 21 );
		break;
	}
	TQString user = url()->user().isEmpty() ? TQString( "anonymous" ) : url()->user();
	TQString pass = url()->password().isEmpty() ? TQString( "anonymous@" ) : url()->password();
	login( user, pass );
    }

    if ( state() == LoggedIn )
	return true;
    return false;
}

/*!  \reimp
*/
int TQFtp::supportedOperations() const
{
    return OpListChildren | OpMkDir | OpRemove | OpRename | OpGet | OpPut;
}

/*! \internal
    Parses the string, \a buffer, which is one line of a directory
    listing which came from the FTP server, and sets the values which
    have been parsed to the url info object, \a info.
*/
void TQFtp::parseDir( const TQString &buffer, TQUrlInfo &info )
{
    TQFtpDTP::parseDir( buffer, url()->user(), &info );
}

void TQFtp::npListInfo( const TQUrlInfo & i )
{
    if ( url() ) {
	TQRegExp filt( url()->nameFilter(), false, true );
	if ( i.isDir() || filt.search( i.name() ) != -1 ) {
	    emit newChild( i, operationInProgress() );
	}
    } else {
	emit newChild( i, operationInProgress() );
    }
}

void TQFtp::npDone( bool err )
{
    TQFtpPrivate *d = ::d( this );

    bool emitFinishedSignal = false;
    TQNetworkOperation *op = operationInProgress();
    if ( op ) {
	if ( err ) {
	    op->setProtocolDetail( errorString() );
	    op->setState( StFailed );
	    if ( error() == HostNotFound ) {
		op->setErrorCode( (int)ErrHostNotFound );
	    } else {
		switch ( op->operation() ) {
		    case OpListChildren:
			op->setErrorCode( (int)ErrListChildren );
			break;
		    case OpMkDir:
			op->setErrorCode( (int)ErrMkDir );
			break;
		    case OpRemove:
			op->setErrorCode( (int)ErrRemove );
			break;
		    case OpRename:
			op->setErrorCode( (int)ErrRename );
			break;
		    case OpGet:
			op->setErrorCode( (int)ErrGet );
			break;
		    case OpPut:
			op->setErrorCode( (int)ErrPut );
			break;
		}
	    }
	    emitFinishedSignal = true;
	} else if ( !d->npWaitForLoginDone ) {
	    switch ( op->operation() ) {
		case OpRemove:
		    emit removed( op );
		    break;
		case OpMkDir:
		    {
			TQUrlInfo inf( op->arg( 0 ), 0, "", "", 0, TQDateTime(),
				TQDateTime(), true, false, false, true, true, true );
			emit newChild( inf, op );
			emit createdDirectory( inf, op );
		    }
		    break;
		case OpRename:
		    emit itemChanged( operationInProgress() );
		    break;
		default:
		    break;
	    }
	    op->setState( StDone );
	    emitFinishedSignal = true;
	}
    }
    d->npWaitForLoginDone = false;

    if ( state() == Unconnected ) {
	disconnect( this, TQ_SIGNAL(listInfo(const TQUrlInfo&)),
		    this, TQ_SLOT(npListInfo(const TQUrlInfo&)) );
	disconnect( this, TQ_SIGNAL(done(bool)),
		    this, TQ_SLOT(npDone(bool)) );
	disconnect( this, TQ_SIGNAL(stateChanged(int)),
		    this, TQ_SLOT(npStateChanged(int)) );
	disconnect( this, TQ_SIGNAL(dataTransferProgress(int,int)),
		    this, TQ_SLOT(npDataTransferProgress(int,int)) );
	disconnect( this, TQ_SIGNAL(readyRead()),
		    this, TQ_SLOT(npReadyRead()) );
    }

    // emit the finished() signal at the very end to avoid reentrance problems
    if ( emitFinishedSignal )
	emit finished( op );
}

void TQFtp::npStateChanged( int state )
{
    if ( url() ) {
	if ( state == Connecting )
	    emit connectionStateChanged( ConHostFound, tr( "Host %1 found" ).arg( url()->host() ) );
	else if ( state == Connected )
	    emit connectionStateChanged( ConConnected, tr( "Connected to host %1" ).arg( url()->host() ) );
	else if ( state == Unconnected )
	    emit connectionStateChanged( ConClosed, tr( "Connection to %1 closed" ).arg( url()->host() ) );
    } else {
	if ( state == Connecting )
	    emit connectionStateChanged( ConHostFound, tr( "Host found" ) );
	else if ( state == Connected )
	    emit connectionStateChanged( ConConnected, tr( "Connected to host" ) );
	else if ( state == Unconnected )
	    emit connectionStateChanged( ConClosed, tr( "Connection closed" ) );
    }
}

void TQFtp::npDataTransferProgress( int bDone, int bTotal )
{
    emit TQNetworkProtocol::dataTransferProgress( bDone, bTotal, operationInProgress() );
}

void TQFtp::npReadyRead()
{
    emit data( readAll(), operationInProgress() );
}

// ### unused -- delete in TQt 4.0
/*!  \internal
*/
void TQFtp::hostFound()
{
}
/*!  \internal
*/
void TQFtp::connected()
{
}
/*!  \internal
*/
void TQFtp::closed()
{
}
/*!  \internal
*/
void TQFtp::dataHostFound()
{
}
/*!  \internal
*/
void TQFtp::dataConnected()
{
}
/*!  \internal
*/
void TQFtp::dataClosed()
{
}
/*!  \internal
*/
void TQFtp::dataReadyRead()
{
}
/*!  \internal
*/
void TQFtp::dataBytesWritten( int )
{
}
/*!  \internal
*/
void TQFtp::error( int )
{
}

#include "qftp.moc"

#endif // TQT_NO_NETWORKPROTOCOL_FTP
