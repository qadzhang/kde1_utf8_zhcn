/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
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


#ifndef TQHBOX_H
#define TQHBOX_H

#ifndef QT_H
#include "ntqwidget.h"
#endif // QT_H

#ifndef TQT_NO_HBOX

#include "ntqframe.h"

class TQBoxLayout;

class TQ_EXPORT TQHBox : public TQFrame
{
    TQ_OBJECT
public:
    TQHBox( TQWidget* parent=0, const char* name=0, WFlags f=0 );

    void setSpacing( int );
    bool setStretchFactor( TQWidget*, int stretch );
    TQSize sizeHint() const;

protected:
    TQHBox( bool horizontal, TQWidget* parent, const char* name, WFlags f = 0 );
    void frameChanged();

private:
    TQBoxLayout *lay;

#if defined(TQ_DISABLE_COPY)
    TQHBox( const TQHBox & );
    TQHBox &operator=( const TQHBox & );
#endif
};

#endif // TQT_NO_HBOX

#endif // TQHBOX_H
