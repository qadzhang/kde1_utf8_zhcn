/****************************************************************************
**
** Definition of TQMainWindow class
**
** Created : 980316
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

#ifndef TQMAINWINDOW_H
#define TQMAINWINDOW_H

#ifndef QT_H
#include "ntqwidget.h"
#include "ntqtoolbar.h"
#include "ntqptrlist.h"
#include "ntqtextstream.h"
#endif // QT_H

#ifndef TQT_NO_MAINWINDOW

class TQMenuBar;
class TQStatusBar;
class TQToolTipGroup;
class TQMainWindowPrivate;
class TQMainWindowLayout;
class TQPopupMenu;

class TQ_EXPORT TQMainWindow: public TQWidget
{
    TQ_OBJECT
    TQ_PROPERTY( bool rightJustification READ rightJustification WRITE setRightJustification DESIGNABLE false )
    TQ_PROPERTY( bool usesBigPixmaps READ usesBigPixmaps WRITE setUsesBigPixmaps )
    TQ_PROPERTY( bool usesTextLabel READ usesTextLabel WRITE setUsesTextLabel )
    TQ_PROPERTY( bool dockWindowsMovable READ dockWindowsMovable WRITE setDockWindowsMovable )
    TQ_PROPERTY( bool opaqueMoving READ opaqueMoving WRITE setOpaqueMoving )

public:
    TQMainWindow( TQWidget* parent=0, const char* name=0, WFlags f = WType_TopLevel );
    ~TQMainWindow();

#ifndef TQT_NO_MENUBAR
    TQMenuBar * menuBar() const;
#endif
    TQStatusBar * statusBar() const;
#ifndef TQT_NO_TOOLTIP
    TQToolTipGroup * toolTipGroup() const;
#endif

    virtual void setCentralWidget( TQWidget * );
    TQWidget * centralWidget() const;

    virtual void setDockEnabled( Dock dock, bool enable );
    bool isDockEnabled( Dock dock ) const;
    bool isDockEnabled( TQDockArea *area ) const;
    virtual void setDockEnabled( TQDockWindow *tb, Dock dock, bool enable );
    bool isDockEnabled( TQDockWindow *tb, Dock dock ) const;
    bool isDockEnabled( TQDockWindow *tb, TQDockArea *area ) const;

    virtual void addDockWindow( TQDockWindow *, Dock = DockTop, bool newLine = false );
    virtual void addDockWindow( TQDockWindow *, const TQString &label,
				Dock = DockTop, bool newLine = false );
    virtual void moveDockWindow( TQDockWindow *, Dock = DockTop );
    virtual void moveDockWindow( TQDockWindow *, Dock, bool nl, int index, int extraOffset = -1 );
    virtual void removeDockWindow( TQDockWindow * );

    void show();
    void hide();
    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

    bool rightJustification() const;
    bool usesBigPixmaps() const;
    bool usesTextLabel() const;
    bool dockWindowsMovable() const;
    bool opaqueMoving() const;

    bool eventFilter( TQObject*, TQEvent* );

    bool getLocation( TQDockWindow *tb, Dock &dock, int &index, bool &nl, int &extraOffset ) const;

    TQPtrList<TQDockWindow> dockWindows( Dock dock ) const;
    TQPtrList<TQDockWindow> dockWindows() const;
    void lineUpDockWindows( bool keepNewLines = false );

    bool isDockMenuEnabled() const;

    // compatibility stuff
    bool hasDockWindow( TQDockWindow *dw );
#ifndef TQT_NO_TOOLBAR
    void addToolBar( TQDockWindow *, Dock = DockTop, bool newLine = false );
    void addToolBar( TQDockWindow *, const TQString &label,
		     Dock = DockTop, bool newLine = false );
    void moveToolBar( TQDockWindow *, Dock = DockTop );
    void moveToolBar( TQDockWindow *, Dock, bool nl, int index, int extraOffset = -1 );
    void removeToolBar( TQDockWindow * );

    bool toolBarsMovable() const;
    TQPtrList<TQToolBar> toolBars( Dock dock ) const;
    void lineUpToolBars( bool keepNewLines = false );
#endif

    virtual TQDockArea *dockingArea( const TQPoint &p );
    TQDockArea *leftDock() const;
    TQDockArea *rightDock() const;
    TQDockArea *topDock() const;
    TQDockArea *bottomDock() const;

    virtual bool isCustomizable() const;

    bool appropriate( TQDockWindow *dw ) const;

    enum DockWindows { OnlyToolBars, NoToolBars, AllDockWindows };
    TQPopupMenu *createDockWindowMenu( DockWindows dockWindows = AllDockWindows ) const;

public slots:
    virtual void setRightJustification( bool );
    virtual void setUsesBigPixmaps( bool );
    virtual void setUsesTextLabel( bool );
    virtual void setDockWindowsMovable( bool );
    virtual void setOpaqueMoving( bool );
    virtual void setDockMenuEnabled( bool );
    virtual void whatsThis();
    virtual void setAppropriate( TQDockWindow *dw, bool a );
    virtual void customize();

    // compatibility stuff
    void setToolBarsMovable( bool );

signals:
    void pixmapSizeChanged( bool );
    void usesTextLabelChanged( bool );
    void dockWindowPositionChanged( TQDockWindow * );

#ifndef TQT_NO_TOOLBAR
    // compatibility stuff
    void toolBarPositionChanged( TQToolBar * );
#endif

protected slots:
    virtual void setUpLayout();
    virtual bool showDockMenu( const TQPoint &globalPos );
    void menuAboutToShow();

protected:
    void paintEvent( TQPaintEvent * );
    void childEvent( TQChildEvent * );
    bool event( TQEvent * );
    void styleChange( TQStyle& );

private slots:
    void slotPlaceChanged();
    void doLineUp() { lineUpDockWindows( true ); }

private:
    TQMainWindowPrivate * d;
    void triggerLayout( bool deleteLayout = true);
    bool dockMainWindow( TQObject *dock );

#ifndef TQT_NO_MENUBAR
    virtual void setMenuBar( TQMenuBar * );
#endif
    virtual void setStatusBar( TQStatusBar * );
#ifndef TQT_NO_TOOLTIP
    virtual void setToolTipGroup( TQToolTipGroup * );
#endif

    friend class TQDockWindow;
    friend class TQMenuBar;
    friend class TQHideDock;
    friend class TQToolBar;
    friend class TQMainWindowLayout;
private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQMainWindow( const TQMainWindow & );
    TQMainWindow& operator=( const TQMainWindow & );
#endif
};

#ifndef TQT_NO_TOOLBAR
inline void TQMainWindow::addToolBar( TQDockWindow *w, ToolBarDock dock, bool newLine )
{
    addDockWindow( w, dock, newLine );
}

inline void TQMainWindow::addToolBar( TQDockWindow *w, const TQString &label,
			      ToolBarDock dock, bool newLine )
{
    addDockWindow( w, label, dock, newLine );
}

inline void TQMainWindow::moveToolBar( TQDockWindow *w, ToolBarDock dock )
{
    moveDockWindow( w, dock );
}

inline void TQMainWindow::moveToolBar( TQDockWindow *w, ToolBarDock dock, bool nl, int index, int extraOffset )
{
    moveDockWindow( w, dock, nl, index, extraOffset );
}

inline void TQMainWindow::removeToolBar( TQDockWindow *w )
{
    removeDockWindow( w );
}

inline bool TQMainWindow::toolBarsMovable() const
{
    return dockWindowsMovable();
}

inline void TQMainWindow::lineUpToolBars( bool keepNewLines )
{
    lineUpDockWindows( keepNewLines );
}

inline void TQMainWindow::setToolBarsMovable( bool b )
{
    setDockWindowsMovable( b );
}
#endif

#ifndef TQT_NO_TEXTSTREAM
TQ_EXPORT TQTextStream &operator<<( TQTextStream &, const TQMainWindow & );
TQ_EXPORT TQTextStream &operator>>( TQTextStream &, TQMainWindow & );
#endif

#endif // TQT_NO_MAINWINDOW

#endif // TQMAINWINDOW_H
