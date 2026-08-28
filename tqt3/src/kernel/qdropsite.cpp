/****************************************************************************
**
** Implementation of Drag and Drop support
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

#include "ntqdropsite.h"

#ifndef TQT_NO_DRAGANDDROP

#include "ntqwidget.h"


// NOT REVISED
/*!
  \class TQDropSite ntqdropsite.h
  \brief The TQDropSite class provides nothing and does nothing.

  \obsolete

  If your code uses it, you can safely delete it.

  It was used in TQt 1.x to do some drag and drop; that has since been
  folded into TQWidget.

  For detailed information about drag-and-drop, see the TQDragObject class.

  \sa TQDragObject, TQTextDrag, TQImageDrag
*/

/*!
  Constructs a TQDropSite to handle events for the widget \a self.

  Pass \c this as the \a self parameter.
  This enables dropping by calling TQWidget::setAcceptDrops(true).
*/
TQDropSite::TQDropSite( TQWidget* self )
{
    self->setAcceptDrops( true );
}

/*!
  Destroys the drop site.
*/
TQDropSite::~TQDropSite()
{
}

#endif // TQT_NO_DRAGANDDROP
