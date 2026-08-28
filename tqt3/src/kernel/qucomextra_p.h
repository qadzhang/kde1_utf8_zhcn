/****************************************************************************
**
** Definition of extra TQUcom classes
**
** Created : 990101
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

#ifndef TQUCOMEXTRA_P_H
#define TQUCOMEXTRA_P_H

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
#include <private/qucom_p.h>
#endif // QT_H

#if __GNUC__ - 0 > 3
#pragma GCC system_header
#endif

class TQVariant;

#ifndef  TQT_NO_VARIANT
// 6dc75d58-a1d9-4417-b591-d45c63a3a4ea
extern const TQUuid TID_QUType_TQVariant;

struct TQ_EXPORT TQUType_TQVariant : public TQUType
{
    const TQUuid *uuid() const;
    const char *desc() const;

    void set( TQUObject *, const TQVariant & );
    TQVariant &get( TQUObject * o );

    bool canConvertFrom( TQUObject *, TQUType * );
    bool canConvertTo( TQUObject *, TQUType * );
    bool convertFrom( TQUObject *, TQUType * );
    bool convertTo( TQUObject *, TQUType * );
    void clear( TQUObject * );
    int serializeTo( TQUObject *, TQUBuffer * );
    int serializeFrom( TQUObject *, TQUBuffer * );
};
extern TQ_EXPORT TQUType_TQVariant static_QUType_TQVariant;
#endif //TQT_NO_VARIANT


// {0x8d48b3a8, 0xbd7f, 0x11d5, 0x8d, 0x74, 0x00, 0xc0, 0xf0, 0x3b, 0xc0, 0xf3 }
extern TQ_EXPORT const TQUuid TID_QUType_varptr;
struct TQ_EXPORT TQUType_varptr : public TQUType
{
    const TQUuid *uuid() const;
    const char *desc() const;

    void set( TQUObject *, const void* );
    void* &get( TQUObject * o ) { return o->payload.ptr; }
    bool canConvertFrom( TQUObject *, TQUType * );
    bool canConvertTo( TQUObject *, TQUType * );
    bool convertFrom( TQUObject *, TQUType * );
    bool convertTo( TQUObject *, TQUType * );
    void clear( TQUObject * ) {}
    int serializeTo( TQUObject *, TQUBuffer * );
    int serializeFrom( TQUObject *, TQUBuffer * );
};
extern TQ_EXPORT TQUType_varptr static_QUType_varptr;


#endif // TQUCOMEXTRA_P_H

