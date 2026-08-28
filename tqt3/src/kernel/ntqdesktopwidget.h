/****************************************************************************
**
** Definition of TQDesktopWidget class.
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

#ifndef TQDESKTOPWIDGET_H
#define TQDESKTOPWIDGET_H

#ifndef QT_H
#include "ntqwidget.h"
#endif // QT_H

class TQApplication;
class TQDesktopWidgetPrivate; /* Don't touch! */

class TQ_EXPORT TQDesktopWidget : public TQWidget
{
    TQ_OBJECT
public:
    TQDesktopWidget();
    ~TQDesktopWidget();

    bool isVirtualDesktop() const;

    int numScreens() const;
    int primaryScreen() const;

    int screenNumber( TQWidget *widget = 0 ) const; // ### 4.0: const TQWidget*
    int screenNumber( const TQPoint & ) const;

    TQWidget *screen( int screen = -1 );

    const TQRect& screenGeometry( int screen = -1 ) const;
    const TQRect& screenGeometry( TQWidget *widget ) const
    { return screenGeometry( screenNumber( widget ) ); }
    const TQRect& screenGeometry( const TQPoint &point ) const
    { return screenGeometry( screenNumber( point ) ); }

    const TQRect& availableGeometry( int screen = -1 ) const;
    const TQRect& availableGeometry( TQWidget *widget ) const
    { return availableGeometry( screenNumber( widget ) ); }
    const TQRect& availableGeometry( const TQPoint &point ) const
    { return availableGeometry( screenNumber( point ) ); }

    void insertChild( TQObject * );

    inline void emitResizedSignal(int value) { emit resized(value); }

signals:
    void resized( int );
    void workAreaResized( int );
    void screenCountChanged( int );

protected:
    void resizeEvent( TQResizeEvent *e );

private:
    TQDesktopWidgetPrivate *d;

#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQDesktopWidget( const TQDesktopWidget & );
    TQDesktopWidget &operator=( const TQDesktopWidget & );
#endif

    friend class TQApplication;
#ifdef TQ_WS_QWS
    friend class TQWSDisplay;
#endif
};

#endif //TQDESKTOPWIDGET_H
