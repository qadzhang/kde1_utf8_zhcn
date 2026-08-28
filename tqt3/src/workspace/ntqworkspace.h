/****************************************************************************
**
** Definition of the TQWorkspace class
**
** Created : 990210
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the workspace module of the TQt GUI Toolkit.
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

#ifndef TQWORKSPACE_H
#define TQWORKSPACE_H

#ifndef QT_H
#include "ntqwidget.h"
#include "ntqwidgetlist.h"
#endif // QT_H

#ifndef TQT_NO_WORKSPACE

#if !defined( TQT_MODULE_WORKSPACE ) || defined( QT_INTERNAL_WORKSPACE )
#define TQM_EXPORT_WORKSPACE
#else
#define TQM_EXPORT_WORKSPACE TQ_EXPORT
#endif

class TQWorkspaceChild;
class TQShowEvent;
class TQWorkspacePrivate;
class TQPopupMenu;
class TQDockWindow;

class TQM_EXPORT_WORKSPACE TQWorkspace : public TQWidget
{
    TQ_OBJECT
    TQ_PROPERTY( bool scrollBarsEnabled READ scrollBarsEnabled WRITE setScrollBarsEnabled )

#ifdef QT_WORKSPACE_WINDOWMODE
public:
#endif
    enum WindowMode { TopLevel, MDI, AutoDetect };
    WindowMode windowMode() const;
#ifdef QT_WORKSPACE_WINDOWMODE
private:
#endif

public:
#ifdef QT_WORKSPACE_WINDOWMODE
    TQWorkspace( WindowMode mode, TQWidget* parent=0, const char* name=0 );
#endif
    TQWorkspace( TQWidget* parent=0, const char* name=0 );

    ~TQWorkspace();

    enum WindowOrder { CreationOrder, StackingOrder };

    TQWidget* activeWindow() const;
    TQWidgetList windowList() const; // ### merge with below in 4.0
    TQWidgetList windowList( WindowOrder order ) const;

    TQSize sizeHint() const;

    bool scrollBarsEnabled() const;
    void setScrollBarsEnabled( bool enable );

    void setPaletteBackgroundColor( const TQColor & );
    void setPaletteBackgroundPixmap( const TQPixmap & );

signals:
    void windowActivated( TQWidget* w);

public slots:
    void cascade();
    void tile();
    void closeActiveWindow();
    void closeAllWindows();
    void activateNextWindow();
    void activatePrevWindow();

protected:
#ifndef TQT_NO_STYLE
    void styleChange( TQStyle& );
#endif
    void childEvent( TQChildEvent * );
    void resizeEvent( TQResizeEvent * );
    bool eventFilter( TQObject *, TQEvent * );
    void showEvent( TQShowEvent *e );
    void hideEvent( TQHideEvent *e );
#ifndef TQT_NO_WHEELEVENT
    void wheelEvent( TQWheelEvent *e );
#endif

private slots:
    void normalizeActiveWindow();
    void minimizeActiveWindow();
    void showOperationMenu();
    void popupOperationMenu( const TQPoint& );
    void operationMenuActivated( int );
    void operationMenuAboutToShow();
    void toolMenuAboutToShow();
    void activatePreviousWindow(); // ### remove in TQt 4.0
    void dockWindowsShow();
    void scrollBarChanged();

private:
    void init();
    void handleUndock( TQDockWindow* w);
    void insertIcon( TQWidget* w);
    void removeIcon( TQWidget* w);
    void place( TQWidget* );

    TQWorkspaceChild* findChild( TQWidget* w);
    void showMaximizeControls();
    void hideMaximizeControls();
    void activateWindow( TQWidget* w, bool change_focus = true );
    void showWindow( TQWidget* w);
    void maximizeWindow( TQWidget* w);
    void minimizeWindow( TQWidget* w);
    void normalizeWindow( TQWidget* w);

    TQRect updateWorkspace();

    TQPopupMenu* popup;
    TQWorkspacePrivate* d;

    friend class TQWorkspacePrivate;
    friend class TQWorkspaceChild;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQWorkspace( const TQWorkspace & );
    TQWorkspace& operator=( const TQWorkspace & );
#endif
};


#endif // TQT_NO_WORKSPACE

#endif // TQWORKSPACE_H
