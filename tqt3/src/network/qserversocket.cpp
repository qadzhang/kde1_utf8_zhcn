/****************************************************************************
**
** Implementation of TQServerSocket class.
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

#include "ntqserversocket.h"

#ifndef TQT_NO_NETWORK

#include "ntqsocketnotifier.h"

class TQServerSocketPrivate {
public:
    TQServerSocketPrivate(): s(0), n(0) {}
    ~TQServerSocketPrivate() { delete n; delete s; }
    TQSocketDevice *s;
    TQSocketNotifier *n;
};


/*!
    \class TQServerSocket ntqserversocket.h
    \brief The TQServerSocket class provides a TCP-based server.
\if defined(commercial)
    It is part of the <a href="commercialeditions.html">TQt Enterprise Edition</a>.
\endif

    \ingroup io
    \module network

    This class is a convenience class for accepting incoming TCP
    connections. You can specify the port or have TQServerSocket pick
    one, and listen on just one address or on all the machine's
    addresses.

    Using the API is very simple: subclass TQServerSocket, call the
    constructor of your choice, and implement newConnection() to
    handle new incoming connections. There is nothing more to do.

    (Note that due to lack of support in the underlying APIs,
    TQServerSocket cannot accept or reject connections conditionally.)

    \sa TQSocket, TQSocketDevice, TQHostAddress, TQSocketNotifier
*/


/*!
    Creates a server socket object, that will serve the given \a port
    on all the addresses of this host. If \a port is 0, TQServerSocket
    will pick a suitable port in a system-dependent manner. Use \a
    backlog to specify how many pending connections the server can
    have.

    The \a parent and \a name arguments are passed on to the TQObject
    constructor.

    \warning On Tru64 Unix systems a value of 0 for \a backlog means
    that you don't accept any connections at all; you should specify a
    value larger than 0.
*/

TQServerSocket::TQServerSocket( TQ_UINT16 port, int backlog,
			      TQObject *parent, const char *name )
    : TQObject( parent, name )
{
    d = new TQServerSocketPrivate;
    init( TQHostAddress(), port, backlog );
}


/*!
    Creates a server socket object, that will serve the given \a port
    only on the given \a address. Use \a backlog to specify how many
    pending connections the server can have.

    The \a parent and \a name arguments are passed on to the TQObject
    constructor.

    \warning On Tru64 Unix systems a value of 0 for \a backlog means
    that you don't accept any connections at all; you should specify a
    value larger than 0.
*/

TQServerSocket::TQServerSocket( const TQHostAddress & address, TQ_UINT16 port,
			      int backlog,
			      TQObject *parent, const char *name )
    : TQObject( parent, name )
{
    d = new TQServerSocketPrivate;
    init( address, port, backlog );
}


/*!
    Construct an empty server socket.

    This constructor, in combination with setSocket(), allows us to
    use the TQServerSocket class as a wrapper for other socket types
    (e.g. Unix Domain Sockets under Unix).

    The \a parent and \a name arguments are passed on to the TQObject
    constructor.

    \sa setSocket()
*/

TQServerSocket::TQServerSocket( TQObject *parent, const char *name )
    : TQObject( parent, name )
{
    d = new TQServerSocketPrivate;
}


/*!
    Returns true if the construction succeeded; otherwise returns false.
*/
bool TQServerSocket::ok() const
{
    return !!d->s;
}

/*
  The common bit of the constructors.
 */
void TQServerSocket::init( const TQHostAddress & address, TQ_UINT16 port, int backlog )
{
    d->s = new TQSocketDevice( TQSocketDevice::Stream, address.isIPv4Address()
			      ? TQSocketDevice::IPv4 : TQSocketDevice::IPv6, 0 );
#if !defined(Q_OS_WIN32)
    // Under Unix, we want to be able to use the port, even if a socket on the
    // same address-port is in TIME_WAIT. Under Windows this is possible anyway
    // -- furthermore, the meaning of reusable is different: it means that you
    // can use the same address-port for multiple listening sockets.
    d->s->setAddressReusable( true );
#endif
    if ( d->s->bind( address, port )
      && d->s->listen( backlog ) )
    {
	d->n = new TQSocketNotifier( d->s->socket(), TQSocketNotifier::Read,
				    this, "accepting new connections" );
	connect( d->n, TQ_SIGNAL(activated(int)),
		 this, TQ_SLOT(incomingConnection(int)) );
    } else {
	tqWarning( "TQServerSocket: failed to bind or listen to the socket" );
	delete d->s;
	d->s = 0;
    }
}


/*!
    Destroys the socket.

    This causes any backlogged connections (connections that have
    reached the host, but not yet been completely set up by calling
    TQSocketDevice::accept()) to be severed.

    Existing connections continue to exist; this only affects the
    acceptance of new connections.
*/
TQServerSocket::~TQServerSocket()
{
    delete d;
}


/*!
    \fn void TQServerSocket::newConnection( int socket )

    This pure virtual function is responsible for setting up a new
    incoming connection. \a socket is the fd (file descriptor) for the
    newly accepted connection.
*/


void TQServerSocket::incomingConnection( int )
{
    int fd = d->s->accept();
    if ( fd >= 0 )
	newConnection( fd );
}


/*!
    Returns the port number on which this server socket listens. This
    is always non-zero; if you specify 0 in the constructor,
    TQServerSocket will pick a non-zero port itself. ok() must be true
    before calling this function.

    \sa address() TQSocketDevice::port()
*/
TQ_UINT16 TQServerSocket::port() const
{
    if ( !d || !d->s )
	return 0;
    return d->s->port();
}


/*!
    Returns the operating system socket.
*/
int TQServerSocket::socket() const
{
    if ( !d || !d->s )
	return -1;

    return d->s->socket();
}

/*!
    Returns the address on which this object listens, or 0.0.0.0 if
    this object listens on more than one address. ok() must be true
    before calling this function.

    \sa port() TQSocketDevice::address()
*/
TQHostAddress TQServerSocket::address() const
{
    if ( !d || !d->s )
	return TQHostAddress();

    return d->s->address();
}


/*!
    Returns a pointer to the internal socket device. The returned
    pointer is 0 if there is no connection or pending connection.

    There is normally no need to manipulate the socket device directly
    since this class does all the necessary setup for most client or
    server socket applications.
*/
TQSocketDevice *TQServerSocket::socketDevice()
{
    if ( !d )
	return 0;

    return d->s;
}


/*!
    Sets the socket to use \a socket. bind() and listen() should
    already have been called for \a socket.

    This allows us to use the TQServerSocket class as a wrapper for
    other socket types (e.g. Unix Domain Sockets).
*/
void TQServerSocket::setSocket( int socket )
{
    delete d;
    d = new TQServerSocketPrivate;
    d->s = new TQSocketDevice( socket, TQSocketDevice::Stream );
    d->n = new TQSocketNotifier( d->s->socket(), TQSocketNotifier::Read,
	       this, "accepting new connections" );
    connect( d->n, TQ_SIGNAL(activated(int)),
	     this, TQ_SLOT(incomingConnection(int)) );
}

#endif //TQT_NO_NETWORK
