 /**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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

#ifndef PREFERENCEINTERFACE_H
#define PREFERENCEINTERFACE_H

//
//  W A R N I N G  --  PRIVATE INTERFACES
//  --------------------------------------
//
// This file and the interfaces declared in the file are not
// public. It exists for internal purpose. This header file and
// interfaces may change from version to version (even binary
// incompatible) without notice, or even be removed.
//
// We mean it.
//
//

#include <private/qcom_p.h>
#include <ntqwidgetlist.h>
#include <ntqcstring.h>

// {5c168ee7-4bee-469f-9995-6afdb04ce5a2}
#ifndef IID_Preference
#define IID_Preference TQUuid( 0x5c168ee7, 0x4bee, 0x469f, 0x99, 0x95, 0x6a, 0xfd, 0xb0, 0x4c, 0xe5, 0xa2 )
#endif

struct PreferenceInterface : public TQUnknownInterface
{
    struct Preference
    {
	TQWidget *tab;
	TQString title;
	TQObject *receiver;
	const char *init_slot;
	const char *accept_slot;
    };

    virtual Preference *preference() = 0;
    virtual void connectTo( TQUnknownInterface *appInterface ) = 0;
    virtual void deletePreferenceObject( Preference * ) = 0;
};

#endif
