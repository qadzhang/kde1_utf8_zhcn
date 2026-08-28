/****************************************************************************
**
** Definition of TQToolBar class
**
** Created : 980306
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

#ifndef TQTOOLBAR_H
#define TQTOOLBAR_H

#ifndef QT_H
#include "ntqdockwindow.h"
#endif // QT_H

#ifndef TQT_NO_TOOLBAR

class TQMainWindow;
class TQButton;
class TQBoxLayout;
class TQToolBarPrivate;

class TQ_EXPORT TQToolBar: public TQDockWindow
{
    TQ_OBJECT
    TQ_PROPERTY( TQString label READ label WRITE setLabel )

public:
    TQToolBar( const TQString &label,
	      TQMainWindow *, ToolBarDock = DockTop,
	      bool newLine = false, const char* name=0 );
    TQToolBar( const TQString &label, TQMainWindow *, TQWidget *,
	      bool newLine = false, const char* name=0, WFlags f = 0 );
    TQToolBar( TQMainWindow* parent=0, const char* name=0 );
    ~TQToolBar();

    void addSeparator();

    void show();
    void hide();

    TQMainWindow * mainWindow() const;

    virtual void setStretchableWidget( TQWidget * );

    bool event( TQEvent * e );

    virtual void setLabel( const TQString & );
    TQString label() const;

    virtual void clear();

    TQSize minimumSize() const;
    TQSize minimumSizeHint() const;

    void setOrientation( Orientation o );
    void setMinimumSize( int minw, int minh );

protected:
    void resizeEvent( TQResizeEvent *e );
    void styleChange( TQStyle & );

private slots:
    void createPopup();

private:
    void init();
    void checkForExtension( const TQSize &sz );
    TQToolBarPrivate * d;
    TQMainWindow * mw;
    TQWidget * sw;
    TQString l;

    friend class TQMainWindow;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQToolBar( const TQToolBar & );
    TQToolBar& operator=( const TQToolBar & );
#endif
};

#endif // TQT_NO_TOOLBAR

#endif // TQTOOLBAR_H
