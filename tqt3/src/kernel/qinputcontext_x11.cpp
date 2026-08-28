/****************************************************************************
**
** Implementation of TQInputContext class
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqinputcontext.h"

#ifndef TQT_NO_IM

#include "qplatformdefs.h"

#include "ntqapplication.h"
#include "ntqwidget.h"

#include "qt_x11_p.h"

/*!
    This function may be overridden only if input method is depending
    on X11 and you need raw XEvent. Otherwise, this function must not.

    This function is designed to filter raw key events for XIM, but
    other input methods may use this to implement some special
    features such as distinguishing Shift_L and Shift_R.

    Return true if the \a event has been consumed. Otherwise, the
    unfiltered \a event will be translated into TQEvent and forwarded
    to filterEvent(). Filtering at both x11FilterEvent() and
    filterEvent() in single input method is allowed.

    \a keywidget is a client widget into which a text is inputted. \a
    event is inputted XEvent.

    \sa filterEvent()
*/
bool TQInputContext::x11FilterEvent( TQWidget *keywidget, XEvent *event )
{
    Q_UNUSED(keywidget);
    Q_UNUSED(event);
    return false;
}

#endif //Q_NO_IM
