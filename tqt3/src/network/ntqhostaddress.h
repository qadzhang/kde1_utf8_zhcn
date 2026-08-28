/****************************************************************************
**
** Definition of TQHostAddress class.
**
** Created : 979899
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

#ifndef TQHOSTADDRESS_H
#define TQHOSTADDRESS_H

#ifndef QT_H
#include "ntqstring.h"
#endif // QT_H

#if !defined( TQT_MODULE_NETWORK ) || defined( QT_LICENSE_PROFESSIONAL ) || defined( QT_INTERNAL_NETWORK )
#define TQM_EXPORT_NETWORK
#else
#define TQM_EXPORT_NETWORK TQ_EXPORT
#endif

#ifndef TQT_NO_NETWORK
class TQHostAddressPrivate;

typedef struct {
    TQ_UINT8 c[16];
} Q_IPV6ADDR;

class TQM_EXPORT_NETWORK TQHostAddress
{
public:
    TQHostAddress();
    TQHostAddress( TQ_UINT32 ip4Addr );
    TQHostAddress( TQ_UINT8 *ip6Addr );
    TQHostAddress(const Q_IPV6ADDR &ip6Addr);
#ifndef TQT_NO_STRINGLIST
    TQHostAddress(const TQString &address);
#endif
    TQHostAddress( const TQHostAddress & );
    virtual ~TQHostAddress();

    TQHostAddress & operator=( const TQHostAddress & );

    void setAddress( TQ_UINT32 ip4Addr );
    void setAddress( TQ_UINT8 *ip6Addr );
#ifndef TQT_NO_STRINGLIST
    bool setAddress( const TQString& address );
#endif
    bool	 isIp4Addr()	 const; // obsolete
    TQ_UINT32	 ip4Addr()	 const; // obsolete

    bool	 isIPv4Address() const;
    TQ_UINT32	 toIPv4Address() const;
    bool	 isIPv6Address() const;
    Q_IPV6ADDR	 toIPv6Address() const;

#ifndef TQT_NO_SPRINTF
    TQString	 toString() const;
#endif

    bool	 operator==( const TQHostAddress & ) const;
    bool	 isNull() const;

private:
    TQHostAddressPrivate* d;
};

#endif //TQT_NO_NETWORK
#endif
