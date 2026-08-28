/****************************************************************************
**
** Definition of TQSocketDevice class.
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

#ifndef TQSOCKETDEVICE_H
#define TQSOCKETDEVICE_H

#ifndef QT_H
#include "ntqiodevice.h"
#include "ntqhostaddress.h" // int->TQHostAddress conversion
#endif // QT_H

#if !defined( TQT_MODULE_NETWORK ) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_NETWORK )
#define TQM_EXPORT_NETWORK
#else
#define TQM_EXPORT_NETWORK TQ_EXPORT
#endif

#ifndef TQT_NO_NETWORK
class TQSocketDevicePrivate;


class  TQM_EXPORT_NETWORK TQSocketDevice: public TQIODevice
{
public:
    enum Type { Stream, Datagram };
    enum Protocol { IPv4, IPv6, Unknown };

    TQSocketDevice( Type type = Stream );
    TQSocketDevice( Type type, Protocol protocol, int dummy );
    TQSocketDevice( int socket, Type type );
    virtual ~TQSocketDevice();

    bool	 isValid() const;
    Type	 type() const;
    Protocol	 protocol() const;

    int		 socket() const;
    virtual void setSocket( int socket, Type type );

    bool	 open( int mode );
    void	 close();
    void	 flush();

    // Implementation of TQIODevice abstract virtual functions
    Offset	 size() const;
    Offset	 at() const;
    bool	 at( Offset );
    bool	 atEnd() const;

    bool	 blocking() const;
    virtual void setBlocking( bool );

    bool	 addressReusable() const;
    virtual void setAddressReusable( bool );

    int		 receiveBufferSize() const;
    virtual void setReceiveBufferSize( uint );
    int		 sendBufferSize() const;
    virtual void setSendBufferSize( uint );

    virtual bool connect( const TQHostAddress &, TQ_UINT16 );

    virtual bool bind( const TQHostAddress &, TQ_UINT16 );
    virtual bool listen( int backlog );
    virtual int	 accept();

    TQ_LONG	 bytesAvailable() const;
    TQ_LONG	 waitForMore( int msecs, bool *timeout=0 ) const;
    TQ_LONG	 readBlock( char *data, TQ_ULONG maxlen );
    TQ_LONG	 writeBlock( const char *data, TQ_ULONG len );
    virtual TQ_LONG  writeBlock( const char *data, TQ_ULONG len,
			    const TQHostAddress & host, TQ_UINT16 port );

    int		 getch();
    int		 putch( int );
    int		 ungetch(int);

    TQ_UINT16	 port() const;
    TQ_UINT16	 peerPort() const;
    TQHostAddress address() const;
    TQHostAddress peerAddress() const;

    enum Error {
	NoError,
	AlreadyBound,
	Inaccessible,
	NoResources,
	InternalError,
	Bug = InternalError, // ### remove in 4.0?
	Impossible,
	NoFiles,
	ConnectionRefused,
	NetworkFailure,
	UnknownError
    };
    Error	 error() const;

protected:
    void setError( Error err );

private:
    int fd;
    Type t;
    TQ_UINT16 p;
    TQHostAddress a;
    TQ_UINT16 pp;
    TQHostAddress pa;
    TQSocketDevice::Error e;
    TQSocketDevicePrivate * d;

    enum Option { Broadcast, ReceiveBuffer, ReuseAddress, SendBuffer };

    int		 option( Option ) const;
    virtual void setOption( Option, int );

    void	 fetchConnectionParameters();
#if defined(Q_OS_WIN32)
    void	 fetchPeerConnectionParameters();
#endif

    static void  init();
    int		 createNewSocket();
    Protocol	 getProtocol() const;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQSocketDevice( const TQSocketDevice & );
    TQSocketDevice &operator=( const TQSocketDevice & );
#endif
};

#endif // TQT_NO_NETWORK
#endif // TQSOCKETDEVICE_H
