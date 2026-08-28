/****************************************************************************
**
** Definition of TQWhatsThis class
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

#ifndef TQWHATSTHIS_H
#define TQWHATSTHIS_H

#ifndef QT_H
#include "ntqobject.h"
#endif // QT_H

#ifndef TQT_NO_WHATSTHIS

#include "ntqcursor.h"

class TQToolButton;
class TQPopupMenu;
class TQStyleSheet;

class TQ_EXPORT TQWhatsThis: public TQt
{
public:
    TQWhatsThis( TQWidget *);
    virtual ~TQWhatsThis();

    virtual TQString text( const TQPoint & );
    virtual bool clicked( const TQString& href );

    // the common static functions
    static void setFont( const TQFont &font );    
    
    static void add( TQWidget *, const TQString &);
    static void remove( TQWidget * );
    static TQString textFor( TQWidget *, const TQPoint & pos = TQPoint(), bool includeParents = false );

    static TQToolButton * whatsThisButton( TQWidget * parent );

    static void enterWhatsThisMode();
    static bool inWhatsThisMode();
    static void leaveWhatsThisMode( const TQString& = TQString::null, const TQPoint& pos = TQCursor::pos(), TQWidget* w = 0 );

    static void display( const TQString& text, const TQPoint& pos = TQCursor::pos(), TQWidget* w = 0 );
};

#endif // TQT_NO_WHATSTHIS

#endif // TQWHATSTHIS_H
