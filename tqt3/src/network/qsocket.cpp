/****************************************************************************
**
** Implementation of TQSocket class.
**
** Created : 970521
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqsocket.h"
#ifndef TQT_NO_NETWORK
#include "ntqptrlist.h"
#include "ntqtimer.h"
#include "ntqsocketdevice.h"
#include "ntqdns.h"
#include "private/qinternal_p.h"

#include <string.h>
#ifndef NO_ERRNO_H
#include <errno.h>
#endif

//#define TQSOCKET_DEBUG

/*
  Perhaps this private functionality needs to be refactored.

  Comment from Robert D Gatlin (Intel):

    It would be nice to have the functionality inherent in TQSocket available
    as a separate class as a standard part of the TQt library, something along
    the line of:

      class TQByteBuffer : public TQIODevice { ... }

    The same class could/would be used within TQSocket for the Read/Write
    buffers.

    The above class could be used in the following way(s):

	buffer.open( IO_WriteOnly | IO_Append );
	buffer.writeBlock( a ); // a = TQByteArray
	buffer.close();

	TQByteArray b;
	b.resize( buffer.size() );
	buffer.open( IO_ReadOnly );
	buffer.readBlock( b.data(), b.size() );
	buffer.close();

    But would also be useable with TQDataStream (via TQIODevice) with:

	buffer.open( IO_WriteOnly | IO_Append );
	TQDataStream is( &buffer );
	is << 100;
	buffer.close();

	buffer.open( IO_ReadOnly );
	TQDataStream os( &buffer );
	TQ_UINT32 x;
	os >> x;
	buffer.close();

    The real usefulness is with any situations where data (TQByteArray) arrives
    incrementally (as in TQSocket and filter case above).

    I tried using TQBuffer, but TQBuffer does not trim bytes from the front of
    the buffer in cases like:

	TQBuffer buf;
	buf.open( IO_ReadOnly );
	TQDataStream ds( &buf );
	TQ_INT32 x;
	ds >> x;
	buf.close();

    In the above case, buf.size() will be identical before and after the
    operation with TQDataStream. Based on the implementation of TQBuffer, it
    does not appear well suited for this kind of operation.
*/

// Private class for TQSocket

class TQSocketPrivate {
public:
    TQSocketPrivate();
   ~TQSocketPrivate();
    void closeSocket();
    void close();
    void connectionClosed();
    void setSocketDevice( TQSocket *q, TQSocketDevice *device );

    TQSocket::State	state;			// connection state
    TQString		host;			// host name
    TQ_UINT16		port;			// host port
    TQSocketDevice      *socket;			// connection socket
    TQSocketNotifier    *rsn, *wsn;		// socket notifiers
    TQMembuf		rba;			// read buffer
    TQ_ULONG		readBufferSize;		// limit for the read buffer size
    TQPtrList<TQByteArray> wba;			// list of write bufs
    TQHostAddress	addr;			// connection address
    TQValueList<TQHostAddress> addresses;		// alternatives looked up
    TQIODevice::Offset	wsize;			// write total buf size
    TQIODevice::Offset	windex;			// write index
#ifndef TQT_NO_DNS
    TQDns	       *dns4;
    TQDns	       *dns6;
#endif
    static TQPtrList<TQSocket> sn_read_alreadyCalled; // used to avoid unwanted recursion
    TQValueList<TQHostAddress> l4;
    TQValueList<TQHostAddress> l6;
};

TQPtrList<TQSocket> TQSocketPrivate::sn_read_alreadyCalled;

TQSocketPrivate::TQSocketPrivate()
    : state(TQSocket::Idle), host(TQString::fromLatin1("")), port(0),
      socket(0), rsn(0), wsn(0), readBufferSize(0), wsize(0), windex(0)
{
#ifndef TQT_NO_DNS
    dns4 = 0;
    dns6 = 0;
#endif
    wba.setAutoDelete( true );
}

TQSocketPrivate::~TQSocketPrivate()
{
    close();
    delete socket;
#ifndef TQT_NO_DNS
    delete dns4;
    delete dns6;
#endif
}

void TQSocketPrivate::closeSocket()
{
    // Order is important here - the socket notifiers must go away
    // before the socket does, otherwise libc or the kernel will
    // become unhappy.
    delete rsn;
    rsn = 0;
    delete wsn;
    wsn = 0;
    if ( socket )
	socket->close();
}

void TQSocketPrivate::close()
{
    closeSocket();
    wsize = 0;
    rba.clear(); wba.clear();
    windex = 0;
}

void TQSocketPrivate::connectionClosed()
{
    // We keep the open state in case there's unread incoming data
    state = TQSocket::Idle;
    closeSocket();
    wba.clear();
    windex = wsize = 0;
}

void TQSocketPrivate::setSocketDevice( TQSocket *q, TQSocketDevice *device )
{
    delete socket;
    delete rsn;
    delete wsn;

    if ( device ) {
	socket = device;
    } else {
	socket = new TQSocketDevice( TQSocketDevice::Stream,
				    ( addr.isIPv4Address() ?
				      TQSocketDevice::IPv4 :
				      TQSocketDevice::IPv6 ), 0 );
	socket->setBlocking( false );
	socket->setAddressReusable( true );
    }

    rsn = new TQSocketNotifier( socket->socket(),
			       TQSocketNotifier::Read, q, "read" );
    wsn = new TQSocketNotifier( socket->socket(),
			       TQSocketNotifier::Write, q, "write" );

    TQObject::connect( rsn, TQ_SIGNAL(activated(int)), q, TQ_SLOT(sn_read()) );
    rsn->setEnabled( false );
    TQObject::connect( wsn, TQ_SIGNAL(activated(int)), q, TQ_SLOT(sn_write()) );
    wsn->setEnabled( false );
}

/*!
    \class TQSocket ntqsocket.h
    \brief The TQSocket class provides a buffered TCP connection.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup io
    \module network

    It provides a totally non-blocking TQIODevice, and modifies and
    extends the API of TQIODevice with socket-specific code.

    Note that a TQApplication must have been constructed before this
    class can be used.

    The functions you're likely to call most are connectToHost(),
    bytesAvailable(), canReadLine() and the ones it inherits from
    TQIODevice.

    connectToHost() is the most-used function. As its name implies,
    it opens a connection to a named host.

    Most network protocols are either packet-oriented or
    line-oriented. canReadLine() indicates whether a connection
    contains an entire unread line or not, and bytesAvailable()
    returns the number of bytes available for reading.

    The signals error(), connected(), readyRead() and
    connectionClosed() inform you of the progress of the connection.
    There are also some less commonly used signals. hostFound() is
    emitted when connectToHost() has finished its DNS lookup and is
    starting its TCP connection. delayedCloseFinished() is emitted
    when close() succeeds. bytesWritten() is emitted when TQSocket
    moves data from its "to be written" queue into the TCP
    implementation.

    There are several access functions for the socket: state() returns
    whether the object is idle, is doing a DNS lookup, is connecting,
    has an operational connection, etc. address() and port() return
    the IP address and port used for the connection. The peerAddress()
    and peerPort() functions return the IP address and port used by
    the peer, and peerName() returns the name of the peer (normally
    the name that was passed to connectToHost()). socketDevice()
    returns a pointer to the TQSocketDevice used for this socket.

    TQSocket inherits TQIODevice, and reimplements some functions. In
    general, you can treat it as a TQIODevice for writing, and mostly
    also for reading. The match isn't perfect, since the TQIODevice
    API is designed for devices that are controlled by the same
    machine, and an asynchronous peer-to-peer network connection isn't
    quite like that. For example, there is nothing that matches
    TQIODevice::size() exactly. The documentation for open(), close(),
    flush(), size(), at(), atEnd(), readBlock(), writeBlock(),
    getch(), putch(), ungetch() and readLine() describes the
    differences in detail.

    \warning TQSocket is not suitable for use in threads. If you need
    to uses sockets in threads use the lower-level TQSocketDevice class.

    \warning Because TQt doesn't use the native socketstream
    implementation on Mac OS X, TQSocket has an implicit transfer
    latency of 100ms. You can achieve lower latency on Mac OS X by
    using TQSocketDevice instead.

    \sa TQSocketDevice, TQHostAddress, TQSocketNotifier
*/


/*!
    Creates a TQSocket object in \c TQSocket::Idle state.

    The \a parent and \a name arguments are passed on to the TQObject
    constructor.

    Note that a TQApplication must have been constructed before sockets
    can be used.
*/

TQSocket::TQSocket( TQObject *parent, const char *name )
    : TQObject( parent, name )
{
    d = new TQSocketPrivate;
    setSocketDevice( 0 );
    setFlags( IO_Direct );
    resetStatus();
}


/*!
    Destroys the socket. Closes the connection if necessary.

    \sa close()
*/

TQSocket::~TQSocket()
{
#if defined(TQSOCKET_DEBUG)
    tqDebug( "TQSocket (%s): Destroy", name() );
#endif
    if ( state() != Idle )
	close();
    Q_ASSERT( d != 0 );
    delete d;
}


/*!
    Returns a pointer to the internal socket device.

    There is normally no need to manipulate the socket device directly
    since this class does the necessary setup for most applications.
*/

TQSocketDevice *TQSocket::socketDevice()
{
    return d->socket;
}

/*!
    Sets the internal socket device to \a device. Passing a \a device
    of 0 will cause the internal socket device to be used. Any
    existing connection will be disconnected before using the new \a
    device.

    The new device should not be connected before being associated
    with a TQSocket; after setting the socket call connectToHost() to
    make the connection.

    This function is useful if you need to subclass TQSocketDevice and
    want to use the TQSocket API, for example, to implement Unix domain
    sockets.
*/

void TQSocket::setSocketDevice( TQSocketDevice *device )
{
    if ( state() != Idle )
	close();
    d->setSocketDevice( this, device );
}

/*!
    \enum TQSocket::State

    This enum defines the connection states:

    \value Idle if there is no connection
    \value HostLookup during a DNS lookup
    \value Connecting during TCP connection establishment
    \value Connected when there is an operational connection
    \value Closing if the socket is closing down, but is not yet closed.
*/

/*!
    Returns the current state of the socket connection.

    \sa TQSocket::State
*/

TQSocket::State TQSocket::state() const
{
    return d->state;
}


#ifndef TQT_NO_DNS

/*!
    Attempts to make a connection to \a host on the specified \a port
    and return immediately.

    Any connection or pending connection is closed immediately, and
    TQSocket goes into the \c HostLookup state. When the lookup
    succeeds, it emits hostFound(), starts a TCP connection and goes
    into the \c Connecting state. Finally, when the connection
    succeeds, it emits connected() and goes into the \c Connected
    state. If there is an error at any point, it emits error().

    \a host may be an IP address in string form, or it may be a DNS
    name. TQSocket will do a normal DNS lookup if required. Note that
    \a port is in native byte order, unlike some other libraries.

    \sa state()
*/

void TQSocket::connectToHost( const TQString &host, TQ_UINT16 port )
{
#if defined(TQSOCKET_DEBUG)
    tqDebug( "TQSocket (%s)::connectToHost: host %s, port %d",
	    name(), host.ascii(), port );
#endif
    setSocketIntern( -1 );
    d->state = HostLookup;
    d->host = host;
    d->port = port;
    d->dns4 = new TQDns( host, TQDns::A );
    d->dns6 = new TQDns( host, TQDns::Aaaa );

    // try if the address is already available (for faster connecting...)
    tryConnecting();
    if ( d->state == HostLookup ) {
	connect( d->dns4, TQ_SIGNAL(resultsReady()),
		 this, TQ_SLOT(tryConnecting()) );
	connect( d->dns6, TQ_SIGNAL(resultsReady()),
		 this, TQ_SLOT(tryConnecting()) );
    }
}

#endif


/*!
    This private slots continues the connection process where
    connectToHost() leaves off.
*/

void TQSocket::tryConnecting()
{
#if defined(TQSOCKET_DEBUG)
    tqDebug( "TQSocket (%s)::tryConnecting()", name() );
#endif
    // ### this ifdef isn't correct - addresses() also does /etc/hosts and
    // numeric-address-as-string handling.
#ifndef TQT_NO_DNS

    if ( d->dns4 ) {
	d->l4 = d->dns4->addresses();
	if ( !d->l4.isEmpty() || !d->dns4->isWorking() ) {
#if defined(TQSOCKET_DEBUG)
	    tqDebug( "TQSocket (%s)::tryConnecting: host %s, port %d: "
		    "%d IPv4 addresses",
		    name(), d->host.ascii(), d->port, d->l4.count() );
#endif
	    delete d->dns4;
	    d->dns4 = 0;
	}
    }

    if ( d->dns6 ) {
	d->l6 = d->dns6->addresses();
	if ( !d->l6.isEmpty() || !d->dns6->isWorking() ) {
#if defined(TQSOCKET_DEBUG)
	    tqDebug( "TQSocket (%s)::tryConnecting: host %s, port %d: "
		    "%d IPv6 addresses",
		    name(), d->host.ascii(), d->port, d->l6.count() );
#endif
	    delete d->dns6;
	    d->dns6 = 0;
	}
    }

    if ( d->state == HostLookup ) {
	if ( d->l4.isEmpty() && d->l6.isEmpty() &&
	     !d->dns4 && !d->dns6 ) {
	    // no results and we're not still looking: give up
	    d->state = Idle;
	    emit error( ErrHostNotFound );
	    return;
	}
	if ( d->l4.isEmpty() && d->l6.isEmpty() ) {
	    // no results (yet): try again later
	    return;
	}

	// we've found something. press on with that. if we later find
	// more, fine.
	emit hostFound();
	d->state = Connecting;
    }

    if ( d->state == Connecting ) {
	d->addresses += d->l4;
	d->addresses += d->l6;
	d->l4.clear();
	d->l6.clear();

	// try one address at a time, falling back to the next one if
	// there is a connection failure. (should also support a timeout,
	// or do multiple TCP-level connects at a time, with staggered
	// starts to avoid bandwidth waste and cause fewer
	// "connect-and-abort" errors. but that later.)
	bool stuck = true;
	while( stuck ) {
	    stuck = false;
	    if ( d->socket &&
		 !d->socket->connect( d->addr, d->port ) ) {
		if ( d->socket->error() == TQSocketDevice::NoError ) {
		    if ( d->wsn )
			d->wsn->setEnabled( true );
		    return; // not serious, try again later
		}

#if defined(TQSOCKET_DEBUG)
		tqDebug( "TQSocket (%s)::tryConnecting: "
			"Gave up on IP address %s",
			name(), d->socket->peerAddress().toString().ascii() );
#endif
		delete d->wsn;
		d->wsn = 0;
		delete d->rsn;
		d->rsn = 0;
		delete d->socket;
		d->socket = 0;

                if(d->addresses.isEmpty()) {
                    emit error( ErrConnectionRefused );
                    return;
                }
            }
	    // if the host has more addresses, try another some.
	    if ( d->socket == 0 && !d->addresses.isEmpty() ) {
		d->addr = *d->addresses.begin();
		d->addresses.remove( d->addresses.begin() );
		d->setSocketDevice( this, 0 );
		stuck = true;
#if defined(TQSOCKET_DEBUG)
		tqDebug( "TQSocket (%s)::tryConnecting: Trying IP address %s",
			name(), d->addr.toString().ascii() );
#endif
	    }
	};

	// The socket write notifier will fire when the connection succeeds
	if ( d->wsn )
	    d->wsn->setEnabled( true );
    }
#endif
}

/*!
    \enum TQSocket::Error

    This enum specifies the possible errors:
    \value ErrConnectionRefused if the connection was refused
    \value ErrHostNotFound if the host was not found
    \value ErrSocketRead if a read from the socket failed
*/

/*!
    \fn void TQSocket::error( int )

    This signal is emitted after an error occurred. The parameter is
    the \l Error value.
*/

/*!
    \fn void TQSocket::hostFound()

    This signal is emitted after connectToHost() has been called and
    the host lookup has succeeded.

    \sa connected()
*/


/*!
    \fn void TQSocket::connected()

    This signal is emitted after connectToHost() has been called and a
    connection has been successfully established.

    \sa connectToHost(), connectionClosed()
*/


/*!
    \fn void TQSocket::connectionClosed()

    This signal is emitted when the other end has closed the
    connection. The read buffers may contain buffered input data which
    you can read after the connection was closed.

    \sa connectToHost(), close()
*/


/*!
    \fn void TQSocket::delayedCloseFinished()

    This signal is emitted when a delayed close is finished.

    If you call close() and there is buffered output data to be
    written, TQSocket goes into the \c TQSocket::Closing state and
    returns immediately. It will then keep writing to the socket until
    all the data has been written. Then, the delayedCloseFinished()
    signal is emitted.

    \sa close()
*/


/*!
    \fn void TQSocket::readyRead()

    This signal is emitted every time there is new incoming data.

    Bear in mind that new incoming data is only reported once; if you do not
    read all the data, this class buffers the data and you can read it later,
    but no signal is emitted unless new data arrives. A good practice is to
    read all data in the slot connected to this signal unless you are sure that
    you need to receive more data to be able to process it.

    \sa readBlock(), readLine(), bytesAvailable()
*/


/*!
    \fn void TQSocket::bytesWritten( int nbytes )

    This signal is emitted when data has been written to the network.
    The \a nbytes parameter specifies how many bytes were written.

    The bytesToWrite() function is often used in the same context; it
    indicates how many buffered bytes there are left to write.

    \sa writeBlock(), bytesToWrite()
*/


/*!
    Opens the socket using the specified TQIODevice file mode \a m.
    This function is called automatically when needed and you should
    not call it yourself.

    \sa close()
*/

bool TQSocket::open( int m )
{
    if ( isOpen() ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSocket::open: Already open" );
#endif
	return false;
    }
    TQIODevice::setMode( m & IO_ReadWrite );
    setState( IO_Open );
    return true;
}


/*!
    Closes the socket.

    The read buffer is cleared.

    If the output buffer is empty, the state is set to \c
    TQSocket::Idle and the connection is terminated immediately. If the
    output buffer still contains data to be written, TQSocket goes into
    the \c TQSocket::Closing state and the rest of the data will be
    written. When all of the outgoing data have been written, the
    state is set to \c TQSocket::Idle and the connection is terminated.
    At this point, the delayedCloseFinished() signal is emitted.

    If you don't want that the data of the output buffer is written, call
    clearPendingData() before you call close().

    \sa state(), bytesToWrite() clearPendingData()
*/

void TQSocket::close()
{
    if ( !isOpen() || d->state == Idle )	// already closed
	return;
    if ( d->state == Closing )
	return;
    if ( !d->rsn || !d->wsn )
	return;
#if defined(TQSOCKET_DEBUG)
    tqDebug( "TQSocket (%s): close socket", name() );
#endif
    if ( d->socket && d->wsize ) {		// there's data to be written
	d->state = Closing;
	if ( d->rsn )
	    d->rsn->setEnabled( false );
	if ( d->wsn )
	    d->wsn->setEnabled( true );
	d->rba.clear();				// clear incoming data
	return;
    }
    setFlags( IO_Sequential );
    resetStatus();
    setState( 0 );
    d->close();
    d->state = Idle;
}


/*!
    This function consumes \a nbytes bytes of data from the write
    buffer.
*/

bool TQSocket::consumeWriteBuf( TQ_ULONG nbytes )
{
    if ( nbytes <= 0 || nbytes > d->wsize )
	return false;
#if defined(TQSOCKET_DEBUG)
    tqDebug( "TQSocket (%s): skipWriteBuf %d bytes", name(), (int)nbytes );
#endif
    d->wsize -= nbytes;
    for ( ;; ) {
	TQByteArray *a = d->wba.first();
	if ( d->windex + nbytes >= a->size() ) {
	    nbytes -= a->size() - d->windex;
	    d->wba.remove();
	    d->windex = 0;
	    if ( nbytes == 0 )
		break;
	} else {
	    d->windex += nbytes;
	    break;
	}
    }
    return true;
}



/*!
    Implementation of the abstract virtual TQIODevice::flush() function.
*/

void TQSocket::flush()
{
    if ( !d->socket )
        return;
    bool osBufferFull = false;
    int consumed = 0;
    while ( !osBufferFull && d->state >= Connecting && d->wsize > 0 ) {
#if defined(TQSOCKET_DEBUG)
	tqDebug( "TQSocket (%s): flush: Write data to the socket", name() );
#endif
	TQByteArray *a = d->wba.first();
	int nwritten;
	int i = 0;
	if ( (int)a->size() - d->windex < 1460 ) {
	    // Concatenate many smaller blocks.  the first may be
	    // partial, but each subsequent block is copied entirely
	    // or not at all.  the sizes here are picked so that we
	    // generally won't trigger nagle's algorithm in the tcp
	    // implementation: we concatenate if we'd otherwise send
	    // less than PMTU bytes (we assume PMTU is 1460 bytes),
	    // and concatenate up to the largest payload TCP/IP can
	    // carry.  with these precautions, nagle's algorithm
	    // should apply only when really appropriate.
	    TQByteArray out( 65536 );
	    int j = d->windex;
	    int s = a->size() - j;
	    while ( a && i+s < (int)out.size() ) {
		memcpy( out.data()+i, a->data()+j, s );
		j = 0;
		i += s;
		a = d->wba.next();
		s = a ? a->size() : 0;
	    }
	    nwritten = d->socket->writeBlock( out.data(), i );
	    if ( d->wsn )
		d->wsn->setEnabled( false ); // the TQSocketNotifier documentation says so
	} else {
	    // Big block, write it immediately
	    i = a->size() - d->windex;
	    nwritten = d->socket->writeBlock( a->data() + d->windex, i );
	    if ( d->wsn )
		d->wsn->setEnabled( false ); // the TQSocketNotifier documentation says so
	}
	if ( nwritten > 0 ) {
	    if ( consumeWriteBuf( nwritten ) )
		consumed += nwritten;
	}
	if ( nwritten < i )
	    osBufferFull = true;
    }
    if ( consumed > 0 ) {
#if defined(TQSOCKET_DEBUG)
	tqDebug( "TQSocket (%s): flush: wrote %d bytes, %d left",
		name(), consumed, (int)d->wsize );
#endif
	emit bytesWritten( consumed );
    }
    if ( d->state == Closing && d->wsize == 0 ) {
#if defined(TQSOCKET_DEBUG)
	tqDebug( "TQSocket (%s): flush: Delayed close done. Terminating.",
		name() );
#endif
	setFlags( IO_Sequential );
	resetStatus();
	setState( 0 );
	d->close();
	d->state = Idle;
	emit delayedCloseFinished();
	return;
    }
    if ( !d->socket->isOpen() ) {
	d->connectionClosed();
	emit connectionClosed();
	return;
    }
    if ( d->wsn )
	d->wsn->setEnabled( d->wsize > 0 ); // write if there's data
}


/*!
    Returns the number of incoming bytes that can be read right now
    (like bytesAvailable()).
*/

TQIODevice::Offset TQSocket::size() const
{
    return (Offset)bytesAvailable();
}


/*!
    Returns the current read index. Since TQSocket is a sequential
    device, the current read index is always zero.
*/

TQIODevice::Offset TQSocket::at() const
{
    return 0;
}


/*!
    \overload

    Moves the read index forward to \a index and returns true if the
    operation was successful; otherwise returns false. Moving the
    index forward means skipping incoming data.
*/

bool TQSocket::at( Offset index )
{
    if ( index > d->rba.size() )
	return false;
    d->rba.consumeBytes( (TQ_ULONG)index, 0 );			// throw away data 0..index-1
    // After we read data from our internal buffer, if we use the
    // setReadBufferSize() to limit our buffer, we might now be able to
    // read more data in our buffer. So enable the read socket notifier,
    // but do this only if we are not in a slot connected to the
    // readyRead() signal since this might cause a bad recursive behavior.
    // We can test for this condition by looking at the
    // sn_read_alreadyCalled flag.
    if ( d->rsn && TQSocketPrivate::sn_read_alreadyCalled.findRef(this) == -1 )
	d->rsn->setEnabled( true );
    return true;
}


/*!
    Returns true if there is no more data to read; otherwise returns false.
*/

bool TQSocket::atEnd() const
{
    if ( d->socket == 0 )
	return true;
    TQSocket * that = (TQSocket *)this;
    if ( that->d->socket->bytesAvailable() )	// a little slow, perhaps...
	that->sn_read();
    return that->d->rba.size() == 0;
}


/*!
    Returns the number of incoming bytes that can be read, i.e. the
    size of the input buffer. Equivalent to size().

    This function can trigger the readyRead() signal, if more data has
    arrived on the socket.

    \sa bytesToWrite()
*/

TQ_ULONG TQSocket::bytesAvailable() const
{
    if ( d->socket == 0 )
	return 0;
    TQSocket * that = (TQSocket *)this;
    if ( that->d->socket->bytesAvailable() ) // a little slow, perhaps...
	(void)that->sn_read();
    return that->d->rba.size();
}


/*!
    Wait up to \a msecs milliseconds for more data to be available.

    If \a msecs is -1 the call will block indefinitely.

    Returns the number of bytes available.

    If \a timeout is non-null and no error occurred (i.e. it does not
    return -1): this function sets \a *timeout to true, if the reason
    for returning was that the timeout was reached; otherwise it sets
    \a *timeout to false. This is useful to find out if the peer
    closed the connection.

    \warning This is a blocking call and should be avoided in event
    driven applications.

    \sa bytesAvailable()
*/

TQ_ULONG TQSocket::waitForMore( int msecs, bool *timeout ) const
{
    if ( d->socket == 0 )
	return 0;
    TQSocket * that = (TQSocket *)this;
    if ( that->d->socket->waitForMore( msecs, timeout ) > 0 )
	(void)that->sn_read( true );
    return that->d->rba.size();
}

/*! \overload
*/

TQ_ULONG TQSocket::waitForMore( int msecs ) const
{
    return waitForMore( msecs, 0 );
}

/*!
    Returns the number of bytes that are waiting to be written, i.e.
    the size of the output buffer.

    \sa bytesAvailable() clearPendingData()
*/

TQ_ULONG TQSocket::bytesToWrite() const
{
    return d->wsize;
}

/*!
    Deletes the data that is waiting to be written. This is useful if you want
    to close the socket without waiting for all the data to be written.

    \sa bytesToWrite() close() delayedCloseFinished()
*/

void TQSocket::clearPendingData()
{
    d->wba.clear();
    d->windex = d->wsize = 0;
}

/*!
    Reads \a maxlen bytes from the socket into \a data and returns the
    number of bytes read. Returns -1 if an error occurred.
*/

TQ_LONG TQSocket::readBlock( char *data, TQ_ULONG maxlen )
{
    if ( data == 0 && maxlen != 0 ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQSocket::readBlock: Null pointer error" );
#endif
	return -1;
    }
    if ( !isOpen() ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSocket::readBlock: Socket is not open" );
#endif
	return -1;
    }
    if ( maxlen >= d->rba.size() )
	maxlen = d->rba.size();
#if defined(TQSOCKET_DEBUG)
    tqDebug( "TQSocket (%s): readBlock %d bytes", name(), (int)maxlen );
#endif
    d->rba.consumeBytes( maxlen, data );
    // After we read data from our internal buffer, if we use the
    // setReadBufferSize() to limit our buffer, we might now be able to
    // read more data in our buffer. So enable the read socket notifier,
    // but do this only if we are not in a slot connected to the
    // readyRead() signal since this might cause a bad recursive behavior.
    // We can test for this condition by looking at the
    // sn_read_alreadyCalled flag.
    if ( d->rsn && TQSocketPrivate::sn_read_alreadyCalled.findRef(this) == -1 )
	d->rsn->setEnabled( true );
    return maxlen;
}


/*!
    Writes \a len bytes to the socket from \a data and returns the
    number of bytes written. Returns -1 if an error occurred.
*/

TQ_LONG TQSocket::writeBlock( const char *data, TQ_ULONG len )
{
#if defined(QT_CHECK_NULL)
    if ( data == 0 && len != 0 ) {
	tqWarning( "TQSocket::writeBlock: Null pointer error" );
    }
#endif
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {
	tqWarning( "TQSocket::writeBlock: Socket is not open" );
	return -1;
    }
#endif
#if defined(QT_CHECK_STATE)
    if ( d->state == Closing ) {
	tqWarning( "TQSocket::writeBlock: Cannot write, socket is closing" );
    }
#endif
    if ( len == 0 || d->state == Closing || d->state == Idle )
	return 0;
    TQByteArray *a = d->wba.last();

    // next bit is sensitive.  if we're writing really small chunks,
    // try to buffer up since system calls are expensive, and nagle's
    // algorithm is even more expensive.  but if anything even
    // remotely large is being written, try to issue a write at once.

    bool writeNow = ( d->wsize + len >= 1400 || len > 512 );

    if ( a && a->size() + len < 128 ) {
	// small buffer, resize
	int i = a->size();
	a->resize( i+len );
	memcpy( a->data()+i, data, len );
    } else {
	// append new buffer
	a = new TQByteArray( len );
	memcpy( a->data(), data, len );
	d->wba.append( a );
    }
    d->wsize += len;
    if ( writeNow )
	flush();
    else if ( d->wsn )
	d->wsn->setEnabled( true );
#if defined(TQSOCKET_DEBUG)
    tqDebug( "TQSocket (%s): writeBlock %d bytes", name(), (int)len );
#endif
    return len;
}


/*!
    Reads a single byte/character from the internal read buffer.
    Returns the byte/character read, or -1 if there is nothing to be
    read.

    \sa bytesAvailable(), putch()
*/

int TQSocket::getch()
{
    if ( isOpen() && d->rba.size() > 0 ) {
	uchar c;
	d->rba.consumeBytes( 1, (char*)&c );
	// After we read data from our internal buffer, if we use the
	// setReadBufferSize() to limit our buffer, we might now be able to
	// read more data in our buffer. So enable the read socket notifier,
	// but do this only if we are not in a slot connected to the
	// readyRead() signal since this might cause a bad recursive behavior.
	// We can test for this condition by looking at the
	// sn_read_alreadyCalled flag.
	if ( d->rsn && TQSocketPrivate::sn_read_alreadyCalled.findRef(this) == -1 )
	    d->rsn->setEnabled( true );
	return c;
    }
    return -1;
}


/*!
    Writes the character \a ch to the output buffer.

    Returns \a ch, or -1 if an error occurred.

    \sa getch()
*/

int TQSocket::putch( int ch )
{
    char buf[2];
    buf[0] = ch;
    return writeBlock(buf, 1) == 1 ? ch : -1;
}


/*!
    This implementation of the virtual function TQIODevice::ungetch()
    prepends the character \a ch to the read buffer so that the next
    read returns this character as the first character of the output.
*/

int TQSocket::ungetch( int ch )
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {
	tqWarning( "TQSocket::ungetch: Socket not open" );
	return -1;
    }
#endif
    return d->rba.ungetch( ch );
}


/*!
    Returns true if it's possible to read an entire line of text from
    this socket at this time; otherwise returns false.

    Note that if the peer closes the connection unexpectedly, this
    function returns false. This means that loops such as this won't
    work:

    \code
	while( !socket->canReadLine() ) // WRONG
	    ;
    \endcode

    \sa readLine()
*/

bool TQSocket::canReadLine() const
{
    if ( ((TQSocket*)this)->d->rba.scanNewline( 0 ) )
	return true;
    return ( bytesAvailable() > 0 &&
	     ((TQSocket*)this)->d->rba.scanNewline( 0 ) );
}

/*!
  \reimp
  \internal
    So that it's not hidden by our other readLine().
*/
TQ_LONG TQSocket::readLine( char *data, TQ_ULONG maxlen )
{
    return TQIODevice::readLine(data,maxlen);
}

/*!
    Returns a line of text including a terminating newline character
    (\n). Returns "" if canReadLine() returns false.

    \sa canReadLine()
*/

TQString TQSocket::readLine()
{
    TQByteArray a(256);
    bool nl = d->rba.scanNewline( &a );
    TQString s;
    if ( nl ) {
	at( a.size() );				// skips the data read
	s = TQString( a );
    }
    return s;
}

/*!
  \internal
    Internal slot for handling socket read notifications.

    This function has can usually only be entered once (i.e. no
    recursive calls). If the argument \a force is true, the function
    is executed, but no readyRead() signals are emitted. This
    behaviour is useful for the waitForMore() function, so that it is
    possible to call waitForMore() in a slot connected to the
    readyRead() signal.
*/

void TQSocket::sn_read( bool force )
{
    TQ_LONG maxToRead = 0;
    if ( d->readBufferSize > 0 ) {
	maxToRead = d->readBufferSize - d->rba.size();
	if ( maxToRead <= 0 ) {
	    if ( d->rsn )
		d->rsn->setEnabled( false );
	    return;
	}
    }

    // Use TQSocketPrivate::sn_read_alreadyCalled to avoid recursive calls of
    // sn_read() (and as a result avoid emitting the readyRead() signal in a
    // slot for readyRead(), if you use bytesAvailable()).
    if ( !force && TQSocketPrivate::sn_read_alreadyCalled.findRef(this) != -1 )
	return;
    TQSocketPrivate::sn_read_alreadyCalled.append( this );

    char buf[4096];
    TQ_LONG nbytes = d->socket->bytesAvailable();
    TQ_LONG nread;
    TQByteArray *a = 0;

    if ( state() == Connecting ) {
	if ( nbytes > 0 ) {
	    tryConnection();
	} else {
	    // nothing to do, nothing to care about
	    TQSocketPrivate::sn_read_alreadyCalled.removeRef( this );
	    return;
	}
    }
    if ( state() == Idle ) {
	TQSocketPrivate::sn_read_alreadyCalled.removeRef( this );
	return;
    }

    if ( nbytes <= 0 ) {			// connection closed?
	// On Windows this may happen when the connection is still open.
	// This happens when the system is heavily loaded and we have
	// read all the data on the socket before a new WSAAsyncSelect
	// event is processed. A new read operation would then block.
	// This code is also useful when TQSocket is used without an
	// event loop.
	nread = d->socket->readBlock( buf, maxToRead ? TQMIN((TQ_LONG)sizeof(buf),maxToRead) : sizeof(buf) );
	if ( nread == 0 ) {			// really closed
	    if ( !d->socket->isOpen() ) {
#if defined(TQSOCKET_DEBUG)
		tqDebug( "TQSocket (%s): sn_read: Connection closed", name() );
#endif
		d->connectionClosed();
		emit connectionClosed();
	    }
	    TQSocketPrivate::sn_read_alreadyCalled.removeRef( this );
	    return;
	} else {
	    if ( nread < 0 ) {
		if ( d->socket->error() == TQSocketDevice::NoError ) {
		    // all is fine
		    TQSocketPrivate::sn_read_alreadyCalled.removeRef( this );
		    return;
		}
#if defined(TQSOCKET_DEBUG)
		tqWarning( "TQSocket::sn_read (%s): Close error", name() );
#endif
		if ( d->rsn )
		    d->rsn->setEnabled( false );
		emit error( ErrSocketRead );
		TQSocketPrivate::sn_read_alreadyCalled.removeRef( this );
		return;
	    }
	    a = new TQByteArray( nread );
	    memcpy( a->data(), buf, nread );
	}

    } else {					// data to be read
#if defined(TQSOCKET_DEBUG)
	tqDebug( "TQSocket (%s): sn_read: %ld incoming bytes", name(), nbytes );
#endif
	if ( nbytes > (int)sizeof(buf) ) {
	    // big
	    a = new TQByteArray( nbytes );
	    nread = d->socket->readBlock( a->data(), maxToRead ? TQMIN(nbytes,maxToRead) : nbytes );
	} else {
	    a = 0;
	    nread = d->socket->readBlock( buf, maxToRead ? TQMIN((TQ_LONG)sizeof(buf),maxToRead) : sizeof(buf) );
	    if ( nread > 0 ) {
		// ##### could setRawData
		a = new TQByteArray( nread );
		memcpy( a->data(), buf, nread );
	    }
	}
	if ( nread == 0 ) {
#if defined(TQSOCKET_DEBUG)
	    tqDebug( "TQSocket (%s): sn_read: Connection closed", name() );
#endif
	    // ### we should rather ask the socket device if it is closed
	    d->connectionClosed();
	    emit connectionClosed();
	    TQSocketPrivate::sn_read_alreadyCalled.removeRef( this );
            delete a;
	    return;
	} else if ( nread < 0 ) {
            delete a;

	    if ( d->socket->error() == TQSocketDevice::NoError ) {
		// all is fine
		TQSocketPrivate::sn_read_alreadyCalled.removeRef( this );
		return;
	    }
#if defined(QT_CHECK_RANGE)
	    tqWarning( "TQSocket::sn_read: Read error" );
#endif
	    if ( d->rsn )
		d->rsn->setEnabled( false );
	    emit error( ErrSocketRead );
	    TQSocketPrivate::sn_read_alreadyCalled.removeRef( this );
	    return;
	}
	if ( nread != (int)a->size() ) {		// unexpected
#if defined(CHECK_RANGE) && !defined(Q_OS_WIN32)
	    tqWarning( "TQSocket::sn_read: Unexpected short read" );
#endif
	    a->resize( nread );
	}
    }
    d->rba.append( a );
    if ( !force ) {
	if ( d->rsn )
	    d->rsn->setEnabled( false );
	emit readyRead();
	if ( d->rsn )
	    d->rsn->setEnabled( true );
    }

    TQSocketPrivate::sn_read_alreadyCalled.removeRef( this );
}


/*!
  \internal
    Internal slot for handling socket write notifications.
*/

void TQSocket::sn_write()
{
    if ( d->state == Connecting )		// connection established?
	tryConnection();
    flush();
}

void TQSocket::emitErrorConnectionRefused()
{
    emit error( ErrConnectionRefused );
}

void TQSocket::tryConnection()
{
    if ( d->socket->connect( d->addr, d->port ) ) {
	d->state = Connected;
#if defined(TQSOCKET_DEBUG)
	tqDebug( "TQSocket (%s): sn_write: Got connection to %s",
		name(), peerName().ascii() );
#endif
	if ( d->rsn )
	    d->rsn->setEnabled( true );
	emit connected();
    } else {
	d->state = Idle;
	TQTimer::singleShot( 0, this, TQ_SLOT(emitErrorConnectionRefused()) );
	return;
    }
}


/*!
    Returns the socket number, or -1 if there is no socket at the moment.
*/

int TQSocket::socket() const
{
    if ( d->socket == 0 )
	return -1;
    return d->socket->socket();
}

/*!
    Sets the socket to use \a socket and the state() to \c Connected.
    The socket must already be connected.

    This allows us to use the TQSocket class as a wrapper for other
    socket types (e.g. Unix Domain Sockets).
*/

void TQSocket::setSocket( int socket )
{
    setSocketIntern( socket );
    d->state = Connection;
    d->rsn->setEnabled( true );
}


/*!
    Sets the socket to \a socket. This is used by both setSocket() and
    connectToHost() and can also be used on unconnected sockets.
*/

void TQSocket::setSocketIntern( int socket )
{
    if ( state() != Idle ) {
	clearPendingData();
        close();
    }
    TQ_ULONG oldBufferSize = d ? d->readBufferSize : 0;
    delete d;

    d = new TQSocketPrivate;
    if (oldBufferSize)
        d->readBufferSize = oldBufferSize;
    if ( socket >= 0 ) {
	TQSocketDevice *sd = new TQSocketDevice( socket, TQSocketDevice::Stream );
	sd->setBlocking( false );
	sd->setAddressReusable( true );
	d->setSocketDevice( this, sd );
    }
    d->state = Idle;

    // Initialize the IO device flags
    setFlags( IO_Direct );
    resetStatus();
    open( IO_ReadWrite );

    // hm... this is not very nice.
    d->host = TQString::null;
    d->port = 0;
#ifndef TQT_NO_DNS
    delete d->dns4;
    d->dns4 = 0;
    delete d->dns6;
    d->dns6 = 0;
#endif
}


/*!
    Returns the host port number of this socket, in native byte order.
*/

TQ_UINT16 TQSocket::port() const
{
    if ( d->socket == 0 )
	return 0;
    return d->socket->port();
}


/*!
    Returns the peer's host port number, normally as specified to the
    connectToHost() function. If none has been set, this function
    returns 0.

    Note that TQt always uses native byte order, i.e. 67 is 67 in TQt;
    there is no need to call htons().
*/

TQ_UINT16 TQSocket::peerPort() const
{
    if ( d->socket == 0 )
	return 0;
    return d->socket->peerPort();
}


/*!
    Returns the host address of this socket. (This is normally the
    main IP address of the host, but can be e.g. 127.0.0.1 for
    connections to localhost.)
*/

TQHostAddress TQSocket::address() const
{
    if ( d->socket == 0 ) {
	TQHostAddress tmp;
	return tmp;
    }
    return d->socket->address();
}


/*!
    Returns the address of the connected peer if the socket is in
    Connected state; otherwise an empty TQHostAddress is returned.
*/

TQHostAddress TQSocket::peerAddress() const
{
    if ( d->socket == 0 ) {
	TQHostAddress tmp;
	return tmp;
    }
    return d->socket->peerAddress();
}


/*!
    Returns the host name as specified to the connectToHost()
    function. An empty string is returned if none has been set.
*/

TQString TQSocket::peerName() const
{
    return d->host;
}

/*!
    Sets the size of the TQSocket's internal read buffer to \a bufSize.

    Usually TQSocket reads all data that is available from the operating
    system's socket. If the buffer size is limited to a certain size, this
    means that the TQSocket class doesn't buffer more than this size of data.

    If the size of the read buffer is 0, the read buffer is unlimited and all
    incoming data is buffered. This is the default.

    If you read the data in the readyRead() signal, you shouldn't use this
    option since it might slow down your program unnecessary. This option is
    useful if you only need to read the data at certain points in time, like in
    a realtime streaming application.

    \sa readBufferSize()
*/

void TQSocket::setReadBufferSize( TQ_ULONG bufSize )
{
    d->readBufferSize = bufSize;
}

/*!
    Returns the size of the read buffer.

    \sa setReadBufferSize()
*/

TQ_ULONG TQSocket::readBufferSize() const
{
    return d->readBufferSize;
}

#endif //TQT_NO_NETWORK
