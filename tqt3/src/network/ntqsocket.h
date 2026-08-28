/****************************************************************************
**
** Definition of TQSocket class.
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

#ifndef TQSOCKET_H
#define TQSOCKET_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqiodevice.h"
#include "ntqhostaddress.h" // int->TQHostAddress conversion
#endif // QT_H

#if !defined( TQT_MODULE_NETWORK ) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_NETWORK )
#define TQM_EXPORT_NETWORK
#else
#define TQM_EXPORT_NETWORK TQ_EXPORT
#endif

#ifndef TQT_NO_NETWORK
class TQSocketPrivate;
class TQSocketDevice;


class TQM_EXPORT_NETWORK TQSocket : public TQObject, public TQIODevice
{
    TQ_OBJECT
public:
    enum Error {
	ErrConnectionRefused,
	ErrHostNotFound,
	ErrSocketRead
    };

    TQSocket( TQObject *parent=0, const char *name=0 );
    virtual ~TQSocket();

    enum State { Idle, HostLookup, Connecting,
		 Connected, Closing,
		 Connection=Connected };
    State	 state() const;

    int		 socket() const;
    virtual void setSocket( int );

    TQSocketDevice *socketDevice();
    virtual void setSocketDevice( TQSocketDevice * );

#ifndef TQT_NO_DNS
    virtual void connectToHost( const TQString &host, TQ_UINT16 port );
#endif
    TQString	 peerName() const;

    // Implementation of TQIODevice abstract virtual functions
    bool	 open( int mode );
    void	 close();
    void	 flush();
    Offset	 size() const;
    Offset	 at() const;
    bool	 at( Offset );
    bool	 atEnd() const;

    TQ_ULONG	 bytesAvailable() const; // ### TQIODevice::Offset instead?
    TQ_ULONG	 waitForMore( int msecs, bool *timeout  ) const;
    TQ_ULONG	 waitForMore( int msecs ) const; // ### TQt 4.0: merge the two overloads
    TQ_ULONG	 bytesToWrite() const;
    void	 clearPendingData();

    TQ_LONG	 readBlock( char *data, TQ_ULONG maxlen );
    TQ_LONG	 writeBlock( const char *data, TQ_ULONG len );
    TQ_LONG	 readLine( char *data, TQ_ULONG maxlen );

    int		 getch();
    int		 putch( int );
    int		 ungetch(int);

    bool	 canReadLine() const;
    virtual	 TQString readLine();

    TQ_UINT16	 port() const;
    TQ_UINT16	 peerPort() const;
    TQHostAddress address() const;
    TQHostAddress peerAddress() const;

    void	 setReadBufferSize( TQ_ULONG );
    TQ_ULONG	 readBufferSize() const;

signals:
    void	 hostFound();
    void	 connected();
    void	 connectionClosed();
    void	 delayedCloseFinished();
    void	 readyRead();
    void	 bytesWritten( int nbytes );
    void	 error( int );

protected slots:
    virtual void sn_read( bool force=false );
    virtual void sn_write();

private slots:
    void	tryConnecting();
    void	emitErrorConnectionRefused();

private:
    TQSocketPrivate *d;

    bool	 consumeWriteBuf( TQ_ULONG nbytes );
    void	 tryConnection();
    void         setSocketIntern( int socket );

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQSocket( const TQSocket & );
    TQSocket &operator=( const TQSocket & );
#endif
};

#endif //TQT_NO_NETWORK
#endif // TQSOCKET_H
