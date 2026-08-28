/****************************************************************************
**
** Definition of TQLocalFs class
**
** Created : 950429
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#ifndef TQLOCALFS_H
#define TQLOCALFS_H

#ifndef QT_H
#include "ntqnetworkprotocol.h"
#include "ntqdir.h"
#endif // QT_H

#ifndef TQT_NO_NETWORKPROTOCOL

class TQ_EXPORT TQLocalFs : public TQNetworkProtocol
{
    TQ_OBJECT

public:
    TQLocalFs();
    virtual int supportedOperations() const;

protected:
    virtual void operationListChildren( TQNetworkOperation *op );
    virtual void operationMkDir( TQNetworkOperation *op );
    virtual void operationRemove( TQNetworkOperation *op );
    virtual void operationRename( TQNetworkOperation *op );
    virtual void operationGet( TQNetworkOperation *op );
    virtual void operationPut( TQNetworkOperation *op );

private:
    int calcBlockSize( int totalSize ) const;
    TQDir dir;

};

#endif // TQT_NO_NETWORKPROTOCOL

#endif // TQLOCALFS_H
