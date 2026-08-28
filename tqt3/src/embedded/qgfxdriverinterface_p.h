/****************************************************************************
**
** Definition of TQt/Embedded Graphics Driver Interface
**
** Created : 20020211
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef TQGFXDRIVERINTERFACE_P_H
#define TQGFXDRIVERINTERFACE_P_H

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

#ifndef TQT_NO_COMPONENT

// {449EC6C6-DF3E-43E3-9E57-354A3D05AB34}
#ifndef IID_QGfxDriver
#define IID_QGfxDriver TQUuid( 0x449ec6c6, 0xdf3e, 0x43e3, 0x9e, 0x57, 0x35, 0x4a, 0x3d, 0x05, 0xab, 0x34)
#endif

class TQScreen;

struct TQ_EXPORT TQGfxDriverInterface : public TQFeatureListInterface
{
    virtual TQScreen* create( const TQString& driver, int displayId ) = 0;
};

#endif // TQT_NO_COMPONENT

#endif // TQGFXDRIVERINTERFACE_P_H
