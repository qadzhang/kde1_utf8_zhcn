/****************************************************************************
**
** ...
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

#ifndef TQIMAGEFORMATINTERFACE_P_H
#define TQIMAGEFORMATINTERFACE_P_H

#ifndef QT_H
#include <private/qcom_p.h>
#endif // QT_H

#if __GNUC__ - 0 > 3
#pragma GCC system_header
#endif

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of internal files.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//
//

#ifndef TQT_NO_COMPONENT

// {04903F05-54B1-4726-A849-FB5CB097CA87} 
#ifndef IID_QImageFormat
#define IID_QImageFormat TQUuid( 0x04903f05, 0x54b1, 0x4726, 0xa8, 0x49, 0xfb, 0x5c, 0xb0, 0x97, 0xca, 0x87 )
#endif

class TQImage;

struct TQ_EXPORT TQImageFormatInterface : public TQFeatureListInterface
{
    virtual TQRESULT loadImage( const TQString &format, const TQString &filename, TQImage * ) = 0;
    virtual TQRESULT saveImage( const TQString &format, const TQString &filename, const TQImage & ) = 0;

    virtual TQRESULT installIOHandler( const TQString & ) = 0;
};

#endif // TQT_NO_COMPONENT

#endif // TQIMAGEFORMATINTERFACE_P_H
