/****************************************************************************
**
** Definition of TQServerSocketClass.
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

#ifndef TQSERVERSOCKET_H
#define TQSERVERSOCKET_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqhostaddress.h"
#include "ntqsocketdevice.h" // ### remove or keep for users' convenience?
#endif // QT_H
#ifndef TQT_NO_NETWORK

#if !defined( TQT_MODULE_NETWORK ) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_NETWORK )
#define TQM_EXPORT_NETWORK
#else
#define TQM_EXPORT_NETWORK TQ_EXPORT
#endif

class TQServerSocketPrivate;


class TQM_EXPORT_NETWORK TQServerSocket : public TQObject
{
    TQ_OBJECT
public:
    TQServerSocket( TQ_UINT16 port, int backlog = 1,
		   TQObject *parent=0, const char *name=0 );
    TQServerSocket( const TQHostAddress & address, TQ_UINT16 port, int backlog = 1,
		   TQObject *parent=0, const char *name=0 );
    TQServerSocket( TQObject *parent=0, const char *name=0 );
    virtual ~TQServerSocket();

    bool ok() const;

    TQ_UINT16 port() const ;

    int socket() const ;
    virtual void setSocket( int socket );

    TQHostAddress address() const ;

    virtual void newConnection( int socket ) = 0;

protected:
    TQSocketDevice *socketDevice();

private slots:
    void incomingConnection( int socket );

private:
    TQServerSocketPrivate *d;
    void init( const TQHostAddress & address, TQ_UINT16 port, int backlog );
};

#endif // TQT_NO_NETWORK
#endif // TQSERVERSOCKET_H
