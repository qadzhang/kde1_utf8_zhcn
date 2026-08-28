/****************************************************************************
**
** Implementation of tqInitNetworkProtocols function.
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

#include "ntqnetwork.h"

#ifndef TQT_NO_NETWORK

#include "ntqnetworkprotocol.h"

// protocols
#include "ntqftp.h"
#include "ntqhttp.h"

/*! \file ntqnetwork.h */
/*!
    \relates TQUrlOperator

    This function registers the network protocols for FTP and HTTP.
    You must call this function before you use TQUrlOperator for
    these protocols.

    This function is declared in \l ntqnetwork.h.
*/
void tqInitNetworkProtocols()
{
#ifndef TQT_NO_NETWORKPROTOCOL_FTP
    TQNetworkProtocol::registerNetworkProtocol( "ftp", new TQNetworkProtocolFactory< TQFtp > );
#endif
#ifndef TQT_NO_NETWORKPROTOCOL_HTTP
    TQNetworkProtocol::registerNetworkProtocol( "http", new TQNetworkProtocolFactory< TQHttp > );
#endif
}

#endif // TQT_NO_NETWORK
