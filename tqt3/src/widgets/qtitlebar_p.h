/****************************************************************************
**
** Definition of some TQt private functions.
**
** Created : 000101
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQTITLEBAR_P_H
#define TQTITLEBAR_P_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of qworkspace.cpp and qdockwindow.cpp.  This header file may change
// from version to version without notice, or even be removed.
//
// We mean it.
//
//


#ifndef QT_H
#include "ntqbutton.h"
#include "ntqlabel.h"
#endif // QT_H

#if !defined(TQT_NO_TITLEBAR)

class TQToolTip;
class TQTitleBarPrivate;
class TQPixmap;

class TQ_EXPORT TQTitleBar : public TQWidget
{
    TQ_OBJECT
    TQ_PROPERTY( bool autoRaise READ autoRaise WRITE setAutoRaise )
    TQ_PROPERTY( bool movable READ isMovable WRITE setMovable )

public:
    TQTitleBar (TQWidget* w, TQWidget* parent, const char* name=0);
    ~TQTitleBar();

    bool isActive() const;
    bool usesActiveColor() const;
    virtual TQString visibleText() const;

    bool isMovable() const;
    void setMovable(bool);

    bool autoRaise() const;
    void setAutoRaise(bool);

    TQWidget *window() const;

    TQSize sizeHint() const;

#ifdef TQT_NO_WIDGET_TOPEXTRA
    // We provide one, since titlebar is useless otherwise.
    TQString caption() const;
#endif

public slots:
    void setActive( bool );
    void setCaption( const TQString& title );
    void setIcon( const TQPixmap& icon );

signals:
    void doActivate();
    void doNormal();
    void doClose();
    void doMaximize();
    void doMinimize();
    void doShade();
    void showOperationMenu();
    void popupOperationMenu( const TQPoint& );
    void doubleClicked();

protected:
    bool event( TQEvent *);
    void resizeEvent( TQResizeEvent *);
    void contextMenuEvent( TQContextMenuEvent * );
    void mousePressEvent( TQMouseEvent * );
    void mouseDoubleClickEvent( TQMouseEvent * );
    void mouseReleaseEvent( TQMouseEvent * );
    void mouseMoveEvent( TQMouseEvent * );
    void enterEvent( TQEvent *e );
    void leaveEvent( TQEvent *e );
    void paintEvent( TQPaintEvent *p );

    virtual void cutText();

private:
    void readColors();

    TQTitleBarPrivate *d;
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQTitleBar( const TQTitleBar & );
    TQTitleBar &operator=( const TQTitleBar & );
#endif
};

#endif
#endif //TQTITLEBAR_P_H
