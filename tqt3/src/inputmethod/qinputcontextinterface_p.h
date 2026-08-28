/****************************************************************************
** $Id: qinputcontextinterface_p.h,v 1.2 2004/06/20 18:43:11 daisuke Exp $
**
** ...
**
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.TQPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid TQt Enterprise Edition or TQt Professional Edition
** licenses may use this file in accordance with the TQt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about TQt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for TQPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef TQINPUTCONTEXTINTERFACE_P_H
#define TQINPUTCONTEXTINTERFACE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//
//

#ifndef QT_H
#include <private/qcom_p.h>
#endif // QT_H

#ifndef TQT_NO_IM
#ifndef TQT_NO_COMPONENT

class TQWidget;
class TQInputContext;

// old version interface in qt-x11-immodule-bc-qt3.3.2-20040623.diff:
// {6C2B9EDE-B63C-14c9-A729-3C7643739C4C}
// 
// new version interface:
// {a5f5c63d-e044-11d8-9718-000d6077a78d}
// {b0bf3e59-e526-11d8-80da-000d6077a78d}
// {9ef05c7f-0272-11d9-846c-000d6077a78d}

#ifndef IID_QInputContextFactory
//#define IID_QInputContextFactory TQUuid(0x6c2b9ede, 0xb63c, 0x14c9, 0xa7, 0x29, 0x3c, 0x76, 0x43, 0x73, 0x9c, 0x4c)
//#define IID_QInputContextFactory TQUuid(0xa5f5c63d, 0xe044, 0x11d8, 0x97, 0x18, 0x00, 0x0d, 0x60, 0x77, 0xa7, 0x8d)
//#define IID_QInputContextFactory TQUuid(0xb0bf3e59, 0xe526, 0x11d8, 0x80, 0xda, 0x00, 0x0d, 0x60, 0x77, 0xa7, 0x8d)
#define IID_QInputContextFactory TQUuid(0x9ef05c7f, 0x0272, 0x11d9, 0x84, 0x6c, 0x00, 0x0d, 0x60, 0x77, 0xa7, 0x8d)
#endif

struct TQ_EXPORT TQInputContextFactoryInterface : public TQFeatureListInterface
{
    virtual TQInputContext *create( const TQString &key ) = 0;
    virtual TQStringList languages( const TQString &key ) = 0;
    virtual TQString displayName( const TQString &key ) = 0;
    virtual TQString description( const TQString &key ) = 0;
};

#endif //TQT_NO_COMPONENT
#endif //TQT_NO_IM

#endif //TQINPUTCONTEXTINTERFACE_P_H
