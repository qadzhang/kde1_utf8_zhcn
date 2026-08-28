/****************************************************************************
**
** Implementation of TQMainWindow class
**
** Created : 980312
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

#include "ntqmainwindow.h"
#ifndef TQT_NO_MAINWINDOW

#include "ntqtimer.h"
#include "ntqlayout.h"
#include "ntqobjectlist.h"
#include "ntqintdict.h"
#include "ntqapplication.h"
#include "ntqptrlist.h"
#include "ntqmap.h"
#include "ntqcursor.h"
#include "ntqpainter.h"
#include "ntqmenubar.h"
#include "ntqpopupmenu.h"
#include "ntqtoolbar.h"
#include "ntqstatusbar.h"
#include "ntqscrollview.h"
#include "ntqtooltip.h"
#include "ntqdatetime.h"
#include "ntqwhatsthis.h"
#include "ntqbitmap.h"
#include "ntqdockarea.h"
#include "ntqstringlist.h"
#include "ntqstyle.h"
#ifdef TQ_WS_MACX
#  include "qt_mac.h"
#endif

class TQHideDock;
class TQMainWindowLayout;

class TQMainWindowPrivate
{
public:
    TQMainWindowPrivate()
	:  mb(0), sb(0), ttg(0), mc(0), tll(0), mwl(0), ubp( false ), utl( false ),
	   justify( false ), movable( true ), opaque( false ), dockMenu( true )
    {
	docks.insert( TQt::DockTop, true );
	docks.insert( TQt::DockBottom, true );
	docks.insert( TQt::DockLeft, true );
	docks.insert( TQt::DockRight, true );
	docks.insert( TQt::DockMinimized, false );
	docks.insert( TQt::DockTornOff, true );
    }

    ~TQMainWindowPrivate()
    {
    }

#ifndef TQT_NO_MENUBAR
    TQMenuBar * mb;
#else
    TQWidget * mb;
#endif
    TQStatusBar * sb;
    TQToolTipGroup * ttg;

    TQWidget * mc;

    TQBoxLayout * tll;
    TQMainWindowLayout * mwl;

    uint ubp :1;
    uint utl :1;
    uint justify :1;
    uint movable :1;
    uint opaque :1;
    uint dockMenu :1;

    TQDockArea *topDock, *bottomDock, *leftDock, *rightDock;

    TQPtrList<TQDockWindow> dockWindows;
    TQMap<TQt::Dock, bool> docks;
    TQStringList disabledDocks;
    TQHideDock *hideDock;

    TQGuardedPtr<TQPopupMenu> rmbMenu, tbMenu, dwMenu;
    TQMap<TQDockWindow*, bool> appropriate;
    TQMap<TQPopupMenu*, TQMainWindow::DockWindows> dockWindowModes;

};


/* TQMainWindowLayout, respects widthForHeight layouts (like the left
  and right docks are)
*/

class TQMainWindowLayout : public TQLayout
{
    TQ_OBJECT

public:
    TQMainWindowLayout( TQMainWindow *mw, TQLayout* parent = 0 );
    ~TQMainWindowLayout() {}

    void addItem( TQLayoutItem * );
    void setLeftDock( TQDockArea *l );
    void setRightDock( TQDockArea *r );
    void setCentralWidget( TQWidget *w );
    bool hasHeightForWidth() const { return false; }
    TQSize sizeHint() const;
    TQSize minimumSize() const;
    TQLayoutIterator iterator();
    TQSizePolicy::ExpandData expanding() const { return TQSizePolicy::BothDirections; }
    void invalidate() {}

protected:
    void setGeometry( const TQRect &r ) {
	TQLayout::setGeometry( r );
	layoutItems( r );
    }

private:
    int layoutItems( const TQRect&, bool testonly = false );
    int extraPixels() const;

    TQDockArea *left, *right;
    TQWidget *central;
    TQMainWindow *mainWindow;

};

TQSize TQMainWindowLayout::sizeHint() const
{
    int w = 0;
    int h = 0;

    if ( left ) {
	w += left->sizeHint().width();
	h = TQMAX( h, left->sizeHint().height() );
    }
    if ( right ) {
	w += right->sizeHint().width();
	h = TQMAX( h, right->sizeHint().height() );
    }
    if ( central ) {
	w += central->sizeHint().width();
	int diff = extraPixels();
	h = TQMAX( h, central->sizeHint().height() + diff );
    }
    return TQSize( w, h );
}

TQSize TQMainWindowLayout::minimumSize() const
{
    int w = 0;
    int h = 0;

    if ( left ) {
	TQSize ms = left->minimumSizeHint().expandedTo( left->minimumSize() );
	w += ms.width();
	h = TQMAX( h, ms.height() );
    }
    if ( right ) {
	TQSize ms = right->minimumSizeHint().expandedTo( right->minimumSize() );
	w += ms.width();
	h = TQMAX( h, ms.height() );
    }
    if ( central ) {
	TQSize min = central->minimumSize().isNull() ?
		    central->minimumSizeHint() : central->minimumSize();
	w += min.width();
	int diff = extraPixels();
	h = TQMAX( h, min.height() + diff );
    }
    return TQSize( w, h );
}

TQMainWindowLayout::TQMainWindowLayout( TQMainWindow *mw, TQLayout* parent )
    : TQLayout( parent ), left( 0 ), right( 0 ), central( 0 )
{
    mainWindow = mw;
}

void TQMainWindowLayout::setLeftDock( TQDockArea *l )
{
    left = l;
}

void TQMainWindowLayout::setRightDock( TQDockArea *r )
{
    right = r;
}

void TQMainWindowLayout::setCentralWidget( TQWidget *w )
{
    central = w;
}

int TQMainWindowLayout::layoutItems( const TQRect &r, bool testonly )
{
    if ( !left && !central && !right )
	return 0;

    int wl = 0, wr = 0;
    if ( left )
	wl = ( (TQDockAreaLayout*)left->TQWidget::layout() )->widthForHeight( r.height() );
    if ( right )
	wr = ( (TQDockAreaLayout*)right->TQWidget::layout() )->widthForHeight( r.height() );
    int w = r.width() - wr - wl;
    if ( w < 0 )
	w = 0;

    int diff = extraPixels();
    if ( !testonly ) {
	TQRect g( geometry() );
	if ( left )
	    left->setGeometry( TQRect( g.x(), g.y() + diff, wl, r.height() - diff ) );
	if ( right )
	    right->setGeometry( TQRect( g.x() + g.width() - wr, g.y() + diff, wr, r.height() - diff ) );
	if ( central )
	    central->setGeometry( g.x() + wl, g.y() + diff, w, r.height() - diff );
    }

    w = wl + wr;
    if ( central )
	w += central->minimumSize().width();
    return w;
}

int TQMainWindowLayout::extraPixels() const
{
    if ( mainWindow->d->topDock->isEmpty() &&
	 !(mainWindow->d->leftDock->isEmpty() &&
	   mainWindow->d->rightDock->isEmpty()) ) {
	return 2;
    } else {
	return 0;
    }
}

void TQMainWindowLayout::addItem( TQLayoutItem * /* item */ )
{
}


TQLayoutIterator TQMainWindowLayout::iterator()
{
    return 0;
}


/*
  TQHideToolTip and TQHideDock - minimized dock
*/

#ifndef TQT_NO_TOOLTIP
class TQHideToolTip : public TQToolTip
{
public:
    TQHideToolTip( TQWidget *parent ) : TQToolTip( parent ) {}
    ~TQHideToolTip() {}

    virtual void maybeTip( const TQPoint &pos );
};
#endif


class TQHideDock : public TQWidget
{
    TQ_OBJECT

public:
    TQHideDock( TQMainWindow *parent ) : TQWidget( parent, "qt_hide_dock" ) {
	hide();
	setFixedHeight( style().pixelMetric( TQStyle::PM_DockWindowHandleExtent,
					     this ) + 3 );
	pressedHandle = -1;
	pressed = false;
	setMouseTracking( true );
	win = parent;
#ifndef TQT_NO_TOOLTIP
	tip = new TQHideToolTip( this );
#endif
    }
    ~TQHideDock()
    {
#ifndef TQT_NO_TOOLTIP
	delete tip;
#endif
    }

protected:
    void paintEvent( TQPaintEvent *e ) {
	if ( !children() || children()->isEmpty() )
	    return;
	TQPainter p( this );
	p.setClipRegion( e->rect() );
	p.fillRect( e->rect(), colorGroup().brush( TQColorGroup::Background ) );
	int x = 0;
	int i = -1;
	TQObjectListIt it( *children() );
	TQObject *o;
	while ( ( o = it.current() ) ) {
	    ++it;
	    ++i;
	    TQDockWindow *dw = ::tqt_cast<TQDockWindow*>(o);
	    if ( !dw || !dw->isVisible() )
		continue;

	    TQStyle::SFlags flags = TQStyle::Style_Default;
	    if ( i == pressedHandle )
		flags |= TQStyle::Style_On;

	    style().drawPrimitive( TQStyle::PE_DockWindowHandle, &p,
				   TQRect( x, 0, 30, 10 ), colorGroup(),
				   flags );
	    x += 30;
	}
    }

    void mousePressEvent( TQMouseEvent *e ) {
	pressed = true;
	if ( !children() || children()->isEmpty() )
	    return;
	mouseMoveEvent( e );
	pressedHandle = -1;

	if ( e->button() == RightButton && win->isDockMenuEnabled() ) {
	    // ### TODO: HideDock menu
	} else {
	    mouseMoveEvent( e );
	}
    }

    void mouseMoveEvent( TQMouseEvent *e ) {
	if ( !children() || children()->isEmpty() )
	    return;
	if ( !pressed )
	    return;
	int x = 0;
	int i = -1;
	if ( e->y() >= 0 && e->y() <= height() ) {
	    TQObjectListIt it( *children() );
	    TQObject *o;
	    while ( ( o = it.current() ) ) {
		++it;
		++i;
		TQDockWindow *dw = ::tqt_cast<TQDockWindow*>(o);
		if ( !dw || !dw->isVisible() )
		    continue;

		if ( e->x() >= x && e->x() <= x + 30 ) {
		    int old = pressedHandle;
		    pressedHandle = i;
		    if ( pressedHandle != old )
			repaint( true );
		    return;
		}
		x += 30;
	    }
	}
	int old = pressedHandle;
	pressedHandle = -1;
	if ( old != -1 )
	    repaint( true );
    }

    void mouseReleaseEvent( TQMouseEvent *e ) {
	pressed = false;
	if ( pressedHandle == -1 )
	    return;
	if ( !children() || children()->isEmpty() )
	    return;
	if ( e->button() == LeftButton ) {
	    if ( e->y() >= 0 && e->y() <= height() ) {
		TQObject *o = ( (TQObjectList*)children() )->at( pressedHandle );
		TQDockWindow *dw = ::tqt_cast<TQDockWindow*>(o);
		if ( dw ) {
		    dw->show();
		    dw->dock();
		}
	    }
	}
	pressedHandle = -1;
	repaint( false );
    }

    bool eventFilter( TQObject *o, TQEvent *e ) {
	if ( o == this || !o->isWidgetType() )
	    return TQWidget::eventFilter( o, e );
	if ( e->type() == TQEvent::Hide ||
	     e->type() == TQEvent::Show ||
	     e->type() == TQEvent::ShowToParent )
	    updateState();
	return TQWidget::eventFilter( o, e );
    }

    void updateState() {
	bool visible = true;
	if ( !children() || children()->isEmpty() ) {
	    visible = false;
	} else {
	    TQObjectListIt it( *children() );
	    TQObject *o;
	    while ( ( o = it.current() ) ) {
		++it;
		TQDockWindow *dw = ::tqt_cast<TQDockWindow*>(o);
		if ( !dw )
		    continue;
		if ( dw->isHidden() ) {
		    visible = false;
		    continue;
		}
		if ( !dw->isVisible() )
		    continue;
		visible = true;
		break;
	    }
	}

	if ( visible )
	    show();
	else
	    hide();
	win->triggerLayout( false );
	update();
    }

    void childEvent( TQChildEvent *e ) {
	TQWidget::childEvent( e );
	if ( e->type() == TQEvent::ChildInserted )
	    e->child()->installEventFilter( this );
	else
	    e->child()->removeEventFilter( this );
	updateState();
    }

private:
    TQMainWindow *win;
    int pressedHandle;
    bool pressed;
#ifndef TQT_NO_TOOLTIP
    TQHideToolTip *tip;
    friend class TQHideToolTip;
#endif
};

#ifndef TQT_NO_TOOLTIP
void TQHideToolTip::maybeTip( const TQPoint &pos )
{
    if ( !parentWidget() )
	return;
    TQHideDock *dock = (TQHideDock*)parentWidget();

    if ( !dock->children() || dock->children()->isEmpty() )
	return;
    TQObjectListIt it( *dock->children() );
    TQObject *o;
    int x = 0;
    while ( ( o = it.current() ) ) {
	++it;
	TQDockWindow *dw = ::tqt_cast<TQDockWindow*>(o);
	if ( !dw || !dw->isVisible() )
	    continue;

	if ( pos.x() >= x && pos.x() <= x + 30 ) {
	    TQDockWindow *dw = (TQDockWindow*)o;
	    if ( !dw->caption().isEmpty() )
		tip( TQRect( x, 0, 30, dock->height() ), dw->caption() );
	    return;
	}
	x += 30;
    }
}
#endif

/*!
    \class TQMainWindow ntqmainwindow.h
    \brief The TQMainWindow class provides a main application window,
    with a menu bar, dock windows (e.g. for toolbars), and a status
    bar.

    \ingroup application
    \mainclass

    Main windows are most often used to provide menus, toolbars and a
    status bar around a large central widget, such as a text edit,
    drawing canvas or TQWorkspace (for MDI applications). TQMainWindow
    is usually subclassed since this makes it easier to encapsulate
    the central widget, menus and toolbars as well as the window's
    state. Subclassing makes it possible to create the slots that are
    called when the user clicks menu items or toolbar buttons. You can
    also create main windows using \link designer-manual.book TQt
    Designer\endlink. We'll briefly review adding menu items and
    toolbar buttons then describe the facilities of TQMainWindow
    itself.

    \code
    TQMainWindow *mw = new TQMainWindow;
    TQTextEdit *edit = new TQTextEdit( mw, "editor" );
    edit->setFocus();
    mw->setCaption( "Main Window" );
    mw->setCentralWidget( edit );
    mw->show();
    \endcode

    TQMainWindows may be created in their own right as shown above.
    The central widget is set with setCentralWidget(). Popup menus can
    be added to the default menu bar, widgets can be added to the
    status bar, toolbars and dock windows can be added to any of the
    dock areas.

    \quotefile application/main.cpp
    \skipto ApplicationWindow
    \printuntil show

    In the extract above ApplicationWindow is a subclass of
    TQMainWindow that we must write for ourselves; this is the usual
    approach to using TQMainWindow. (The source for the extracts in
    this description are taken from \l application/main.cpp, \l
    application/application.cpp, \l action/main.cpp, and \l
    action/application.cpp )

    When subclassing we add the menu items and toolbars in the
    subclass's constructor. If we've created a TQMainWindow instance
    directly we can add menu items and toolbars just as easily by
    passing the TQMainWindow instance as the parent instead of the \e
    this pointer.

    \quotefile application/application.cpp
    \skipto help = new
    \printuntil about

    Here we've added a new menu with one menu item. The menu has been
    inserted into the menu bar that TQMainWindow provides by default
    and which is accessible through the menuBar() function. The slot
    will be called when the menu item is clicked.

    \quotefile application/application.cpp
    \skipto fileTools
    \printuntil setLabel
    \skipto TQToolButton
    \printuntil open file

    This extract shows the creation of a toolbar with one toolbar
    button. TQMainWindow supplies four dock areas for toolbars. When a
    toolbar is created as a child of a TQMainWindow (or derived class)
    instance it will be placed in a dock area (the \c Top dock area by
    default). The slot will be called when the toolbar button is
    clicked. Any dock window can be added to a dock area either using
    addDockWindow(), or by creating a dock window with the TQMainWindow
    as the parent.

    \quotefile application/application.cpp
    \skipto editor
    \printuntil statusBar

    Having created the menus and toolbar we create an instance of the
    large central widget, give it the focus and set it as the main
    window's central widget. In the example we've also set the status
    bar, accessed via the statusBar() function, to an initial message
    which will be displayed for two seconds. Note that you can add
    additional widgets to the status bar, for example labels, to show
    further status information. See the TQStatusBar documentation for
    details, particularly the addWidget() function.

    Often we want to synchronize a toolbar button with a menu item.
    For example, if the user clicks a 'bold' toolbar button we want
    the 'bold' menu item to be checked. This synchronization can be
    achieved automatically by creating actions and adding the actions
    to the toolbar and menu.

    \quotefile action/application.cpp
    \skipto TQAction * fileOpen
    \printline
    \skipto fileOpenAction
    \printuntil choose

    Here we create an action with an icon which will be used in any
    menu and toolbar that the action is added to. We've also given the
    action a menu name, '\&Open', and a keyboard shortcut. The
    connection that we have made will be used when the user clicks
    either the menu item \e or the toolbar button.

    \quotefile action/application.cpp
    \skipto TQPopupMenu * file
    \printuntil menuBar
    \skipto fileOpen
    \printline

    The extract above shows the creation of a popup menu. We add the
    menu to the TQMainWindow's menu bar and add our action.

    \quotefile action/application.cpp
    \skipto TQToolBar * fileTool
    \printuntil OpenAction

    Here we create a new toolbar as a child of the TQMainWindow and add
    our action to the toolbar.

    We'll now explore the functionality offered by TQMainWindow.

    The main window will take care of the dock areas, and the geometry
    of the central widget, but all other aspects of the central widget
    are left to you. TQMainWindow automatically detects the creation of
    a menu bar or status bar if you specify the TQMainWindow as parent,
    or you can use the provided menuBar() and statusBar() functions.
    The functions menuBar() and statusBar() create a suitable widget
    if one doesn't exist, and update the window's layout to make
    space.

    TQMainWindow provides a TQToolTipGroup connected to the status bar.
    The function toolTipGroup() provides access to the default
    TQToolTipGroup. It isn't possible to set a different tool tip
    group.

    New dock windows and toolbars can be added to a TQMainWindow using
    addDockWindow(). Dock windows can be moved using moveDockWindow()
    and removed with removeDockWindow(). TQMainWindow allows default
    dock window (toolbar) docking in all its dock areas (\c Top, \c
    Left, \c Right, \c Bottom). You can use setDockEnabled() to
    enable and disable docking areas for dock windows. When adding or
    moving dock windows you can specify their 'edge' (dock area). The
    currently available edges are: \c Top, \c Left, \c Right, \c
    Bottom, \c Minimized (effectively a 'hidden' dock area) and \c
    TornOff (floating). See \l TQt::Dock for an explanation of these
    areas. Note that the *ToolBar functions are included for backward
    compatibility; all new code should use the *DockWindow functions.
    TQToolbar is a subclass of TQDockWindow so all functions that work
    with dock windows work on toolbars in the same way.

    \target dwm
    If the user clicks the close button, then the dock window is
    hidden. A dock window can be hidden or unhidden by the user by
    right clicking a dock area and clicking the name of the relevant
    dock window on the pop up dock window menu. This menu lists the
    names of every dock window; visible dock windows have a tick
    beside their names. The dock window menu is created automatically
    as required by createDockWindowMenu(). Since it may not always be
    appropriate for a dock window to appear on this menu the
    setAppropriate() function is used to inform the main window
    whether or not the dock window menu should include a particular
    dock window. Double clicking a dock window handle (usually on the
    left-hand side of the dock window) undocks (floats) the dock
    window. Double clicking a floating dock window's titlebar will
    dock the floating dock window. (See also
    \l{TQMainWindow::DockWindows}.)

    Some functions change the appearance of a TQMainWindow globally:
    \list
    \i TQDockWindow::setHorizontalStretchable() and
    TQDockWindow::setVerticalStretchable() are used to make specific dock
    windows or toolbars stretchable.
    \i setUsesBigPixmaps() is used to set whether tool buttons should
    draw small or large pixmaps (see TQIconSet for more information).
    \i setUsesTextLabel() is used to set whether tool buttons
    should display a textual label in addition to pixmaps
    (see TQToolButton for more information).
    \endlist

    The user can drag dock windows into any enabled docking area. Dock
    windows can also be dragged \e within a docking area, for example
    to rearrange the order of some toolbars. Dock windows can also be
    dragged outside any docking area (undocked or 'floated'). Being
    able to drag dock windows can be enabled (the default) and
    disabled using setDockWindowsMovable().

    The \c Minimized edge is a hidden dock area. If this dock area is
    enabled the user can hide (minimize) a dock window or show (restore)
    a minimized dock window by clicking the dock window handle. If the
    user hovers the mouse cursor over one of the handles, the caption of
    the dock window is displayed in a tool tip (see
    TQDockWindow::caption() or TQToolBar::label()), so if you enable the
    \c Minimized dock area, it is best to specify a meaningful caption
    or label for each dock window. To minimize a dock window
    programmatically use moveDockWindow() with an edge of \c Minimized.

    Dock windows are moved transparently by default, i.e. during the
    drag an outline rectangle is drawn on the screen representing the
    position of the dock window as it moves. If you want the dock
    window to be shown normally whilst it is moved use
    setOpaqueMoving().

    The location of a dock window, i.e. its dock area and position
    within the dock area, can be determined by calling getLocation().
    Movable dock windows can be lined up to minimize wasted space with
    lineUpDockWindows(). Pointers to the dock areas are available from
    topDock(), leftDock(), rightDock() and bottomDock(). A customize
    menu item is added to the pop up dock window menu if
    isCustomizable() returns true; it returns false by default.
    Reimplement isCustomizable() and customize() if you want to offer
    this extra menu item, for example, to allow the user to change
    settings relating to the main window and its toolbars and dock
    windows.

    The main window's menu bar is fixed (at the top) by default. If
    you want a movable menu bar, create a TQMenuBar as a stretchable
    widget inside its own movable dock window and restrict this dock
    window to only live within the \c Top or \c Bottom dock:

    \code
    TQToolBar *tb = new TQToolBar( this );
    addDockWindow( tb, tr( "Menubar" ), Top, false );
    TQMenuBar *mb = new TQMenuBar( tb );
    mb->setFrameStyle( TQFrame::NoFrame );
    tb->setStretchableWidget( mb );
    setDockEnabled( tb, Left, false );
    setDockEnabled( tb, Right, false );
    \endcode

    An application with multiple dock windows can choose to save the
    current dock window layout in order to restore it later, e.g. in
    the next session. You can do this by using the streaming operators
    for TQMainWindow.

    To save the layout and positions of all the dock windows do this:

    \code
    TQFile file( filename );
    if ( file.open( IO_WriteOnly ) ) {
	TQTextStream stream( &file );
	stream << *mainWindow;
	file.close();
    }
    \endcode

    To restore the dock window positions and sizes (normally when the
    application is next started), do following:

    \code
    TQFile file( filename );
    if ( file.open( IO_ReadOnly ) ) {
	TQTextStream stream( &file );
	stream >> *mainWindow;
	file.close();
    }
    \endcode

    The TQSettings class can be used in conjunction with the streaming
    operators to store the application's settings.

    TQMainWindow's management of dock windows and toolbars is done
    transparently behind-the-scenes by TQDockArea.

    For multi-document interfaces (MDI), use a TQWorkspace as the
    central widget.

    Adding dock windows, e.g. toolbars, to TQMainWindow's dock areas is
    straightforward. If the supplied dock areas are not sufficient for
    your application we suggest that you create a TQWidget subclass and
    add your own dock areas (see \l TQDockArea) to the subclass since
    TQMainWindow provides functionality specific to the standard dock
    areas it provides.

    <img src=qmainwindow-m.png> <img src=qmainwindow-w.png>

    \sa TQToolBar TQDockWindow TQStatusBar TQAction TQMenuBar TQPopupMenu TQToolTipGroup TQDialog
*/

/*! \enum TQt::ToolBarDock
    \internal
*/

/*!
    \enum TQt::Dock

    Each dock window can be in one of the following positions:

    \value DockTop  above the central widget, below the menu bar.

    \value DockBottom  below the central widget, above the status bar.

    \value DockLeft  to the left of the central widget.

    \value DockRight to the right of the central widget.

    \value DockMinimized the dock window is not shown (this is
    effectively a 'hidden' dock area); the handles of all minimized
    dock windows are drawn in one row below the menu bar.

    \value DockTornOff the dock window floats as its own top level
    window which always stays on top of the main window.

    \value DockUnmanaged not managed by a TQMainWindow.
*/

/*!
    \enum TQMainWindow::DockWindows

    Right-clicking a dock area will pop-up the dock window menu
    (createDockWindowMenu() is called automatically). When called in
    code you can specify what items should appear on the menu with
    this enum.

    \value OnlyToolBars The menu will list all the toolbars, but not
    any other dock windows.

    \value NoToolBars The menu will list dock windows but not
    toolbars.

    \value AllDockWindows The menu will list all toolbars and other
    dock windows. (This is the default.)
*/

/*!
    \obsolete
    \fn void TQMainWindow::addToolBar( TQDockWindow *, Dock = Top, bool newLine = false );
*/

/*!
    \obsolete
    \overload void TQMainWindow::addToolBar( TQDockWindow *, const TQString &label, Dock = Top, bool newLine = false );
*/

/*!
    \obsolete
    \fn void TQMainWindow::moveToolBar( TQDockWindow *, Dock = Top );
*/

/*!
    \obsolete
    \overload void TQMainWindow::moveToolBar( TQDockWindow *, Dock, bool nl, int index, int extraOffset = -1 );
*/

/*!
    \obsolete
    \fn void TQMainWindow::removeToolBar( TQDockWindow * );
*/

/*!
    \obsolete
    \fn void TQMainWindow::lineUpToolBars( bool keepNewLines = false );
*/

/*!
    \obsolete
    \fn void TQMainWindow::toolBarPositionChanged( TQToolBar * );
*/

/*!
    \obsolete
    \fn bool TQMainWindow::toolBarsMovable() const
*/

/*!
    \obsolete
    \fn void TQMainWindow::setToolBarsMovable( bool )
*/

/*!
    Constructs an empty main window. The \a parent, \a name and widget
    flags \a f, are passed on to the TQWidget constructor.

    By default, the widget flags are set to \c WType_TopLevel rather
    than 0 as they are with TQWidget. If you don't want your
    TQMainWindow to be a top level widget then you will need to set \a
    f to 0.
*/

TQMainWindow::TQMainWindow( TQWidget * parent, const char * name, WFlags f )
    : TQWidget( parent, name, f )
{
    d = new TQMainWindowPrivate;
#ifdef TQ_WS_MACX
    d->opaque = true;
#else
    d->opaque = false;
#endif
    installEventFilter( this );
    d->topDock = new TQDockArea( Horizontal, TQDockArea::Normal, this, "qt_top_dock" );
    d->topDock->installEventFilter( this );
    d->bottomDock = new TQDockArea( Horizontal, TQDockArea::Reverse, this, "qt_bottom_dock" );
    d->bottomDock->installEventFilter( this );
    d->leftDock = new TQDockArea( Vertical, TQDockArea::Normal, this, "qt_left_dock" );
    d->leftDock->installEventFilter( this );
    d->rightDock = new TQDockArea( Vertical, TQDockArea::Reverse, this, "qt_right_dock" );
    d->rightDock->installEventFilter( this );
    d->hideDock = new TQHideDock( this );
}


/*!
    Destroys the object and frees any allocated resources.
*/

TQMainWindow::~TQMainWindow()
{
    delete layout();
    delete d;
}

#ifndef TQT_NO_MENUBAR
/*!
    Sets this main window to use the menu bar \a newMenuBar.

    The existing menu bar (if any) is deleted along with its contents.

    \sa menuBar()
*/

void TQMainWindow::setMenuBar( TQMenuBar * newMenuBar )
{
    if ( !newMenuBar )
	return;
    if ( d->mb )
	delete d->mb;
    d->mb = newMenuBar;
    d->mb->installEventFilter( this );
    triggerLayout();
}


/*!
    Returns the menu bar for this window.

    If there isn't one, then menuBar() creates an empty menu bar.

    \sa statusBar()
*/

TQMenuBar * TQMainWindow::menuBar() const
{
    if ( d->mb )
	return d->mb;

    TQObjectList * l
	= ((TQObject*)this)->queryList( "TQMenuBar", 0, false, false );
    TQMenuBar * b;
    if ( l && l->count() ) {
	b = (TQMenuBar *)l->first();
    } else {
	b = new TQMenuBar( (TQMainWindow *)this, "automatic menu bar" );
	b->show();
    }
    delete l;
    d->mb = b;
    d->mb->installEventFilter( this );
    ((TQMainWindow *)this)->triggerLayout();
    return b;
}
#endif // TQT_NO_MENUBAR

/*!
    Sets this main window to use the status bar \a newStatusBar.

    The existing status bar (if any) is deleted along with its
    contents.

    Note that \a newStatusBar \e must be a child of this main window,
    and that it is not automatically displayed. If you call this
    function after show(), you will probably also need to call
    newStatusBar->show().

    \sa setMenuBar() statusBar()
*/

void TQMainWindow::setStatusBar( TQStatusBar * newStatusBar )
{
    if ( !newStatusBar || newStatusBar == d->sb )
	return;
    if ( d->sb )
	delete d->sb;
    d->sb = newStatusBar;
#ifndef TQT_NO_TOOLTIP
    // ### this code can cause unnecessary creation of a tool tip group
    connect( toolTipGroup(), TQ_SIGNAL(showTip(const TQString&)),
	     d->sb, TQ_SLOT(message(const TQString&)) );
    connect( toolTipGroup(), TQ_SIGNAL(removeTip()),
	     d->sb, TQ_SLOT(clear()) );
#endif
    d->sb->installEventFilter( this );
    triggerLayout();
}


/*!
    Returns this main window's status bar. If there isn't one,
    statusBar() creates an empty status bar, and if necessary a tool
    tip group too.

    \sa  menuBar() toolTipGroup()
*/

TQStatusBar * TQMainWindow::statusBar() const
{
    if ( d->sb )
	return d->sb;

    TQObjectList * l
	= ((TQObject*)this)->queryList( "TQStatusBar", 0, false, false );
    TQStatusBar * s;
    if ( l && l->count() ) {
	s = (TQStatusBar *)l->first();
    } else {
	s = new TQStatusBar( (TQMainWindow *)this, "automatic status bar" );
	s->show();
    }
    delete l;
    ((TQMainWindow *)this)->setStatusBar( s );
    ((TQMainWindow *)this)->triggerLayout( true );
    return s;
}


#ifndef TQT_NO_TOOLTIP
/*!
    Sets this main window to use the tool tip group \a
    newToolTipGroup.

    The existing tool tip group (if any) is deleted along with its
    contents. All the tool tips connected to it lose the ability to
    display the group texts.

    \sa menuBar() toolTipGroup()
*/

void TQMainWindow::setToolTipGroup( TQToolTipGroup * newToolTipGroup )
{
    if ( !newToolTipGroup || newToolTipGroup == d->ttg )
	return;
    if ( d->ttg )
	delete d->ttg;
    d->ttg = newToolTipGroup;

    connect( toolTipGroup(), TQ_SIGNAL(showTip(const TQString&)),
	     statusBar(), TQ_SLOT(message(const TQString&)) );
    connect( toolTipGroup(), TQ_SIGNAL(removeTip()),
	     statusBar(), TQ_SLOT(clear()) );
}


/*!
    Returns this main window's tool tip group. If there isn't one,
    toolTipGroup() creates an empty tool tip group.

    \sa menuBar() statusBar()
*/

TQToolTipGroup * TQMainWindow::toolTipGroup() const
{
    if ( d->ttg )
	return d->ttg;

    TQToolTipGroup * t = new TQToolTipGroup( (TQMainWindow*)this,
					   "automatic tool tip group" );
    ((TQMainWindowPrivate*)d)->ttg = t;
    return t;
}
#endif


/*!
    If \a enable is true then users can dock windows in the \a dock
    area. If \a enable is false users cannot dock windows in the \a
    dock dock area.

    Users can dock (drag) dock windows into any enabled dock area.
*/

void TQMainWindow::setDockEnabled( Dock dock, bool enable )
{
    d->docks.replace( dock, enable );
}


/*!
    Returns true if the \a dock dock area is enabled, i.e. it can
    accept user dragged dock windows; otherwise returns false.

    \sa setDockEnabled()
*/

bool TQMainWindow::isDockEnabled( Dock dock ) const
{
    return d->docks[ dock ];
}

/*!
    \overload

    Returns true if dock area \a area is enabled, i.e. it can accept
    user dragged dock windows; otherwise returns false.

    \sa setDockEnabled()
*/

bool TQMainWindow::isDockEnabled( TQDockArea *area ) const
{

    if ( area == d->leftDock )
	return d->docks[ DockLeft ];
    if ( area == d->rightDock )
	return d->docks[ DockRight ];
    if ( area == d->topDock )
	return d->docks[ DockTop ];
    if ( area == d->bottomDock )
	return d->docks[ DockBottom ];
    return false;
}

/*!
    \overload

    If \a enable is true then users can dock the \a dw dock window in
    the \a dock area. If \a enable is false users cannot dock the \a
    dw dock window in the \a dock area.

    In general users can dock (drag) dock windows into any enabled
    dock area. Using this function particular dock areas can be
    enabled (or disabled) as docking points for particular dock
    windows.
*/


void TQMainWindow::setDockEnabled( TQDockWindow *dw, Dock dock, bool enable )
{
    if ( d->dockWindows.find( dw ) == -1 ) {
	d->dockWindows.append( dw );
	connect( dw, TQ_SIGNAL( placeChanged(TQDockWindow::Place) ),
		 this, TQ_SLOT( slotPlaceChanged() ) );
    }
    TQString s;
    s.sprintf( "%p_%d", (void*)dw, (int)dock );
    if ( enable )
	d->disabledDocks.remove( s );
    else if ( d->disabledDocks.find( s ) == d->disabledDocks.end() )
	d->disabledDocks << s;
    switch ( dock ) {
	case DockTop:
	    topDock()->setAcceptDockWindow( dw, enable );
	    break;
	case DockLeft:
	    leftDock()->setAcceptDockWindow( dw, enable );
	    break;
	case DockRight:
	    rightDock()->setAcceptDockWindow( dw, enable );
	    break;
	case DockBottom:
	    bottomDock()->setAcceptDockWindow( dw, enable );
	    break;
	default:
	    break;
    }
}

/*!
    \overload

    Returns true if dock area \a area is enabled for the dock window
    \a dw; otherwise returns false.

    \sa setDockEnabled()
*/

bool TQMainWindow::isDockEnabled( TQDockWindow *dw, TQDockArea *area ) const
{
    if ( !isDockEnabled( area ) )
	return false;
    Dock dock;
    if ( area == d->leftDock )
	dock = DockLeft;
    else if ( area == d->rightDock )
	dock = DockRight;
    else if ( area == d->topDock )
	dock = DockTop;
    else if ( area == d->bottomDock )
	dock = DockBottom;
    else
	return false;
    return isDockEnabled( dw, dock );
}

/*!
    \overload

    Returns true if dock area \a dock is enabled for the dock window
    \a tb; otherwise returns false.

    \sa setDockEnabled()
*/

bool TQMainWindow::isDockEnabled( TQDockWindow *tb, Dock dock ) const
{
    if ( !isDockEnabled( dock ) )
	return false;
    TQString s;
    s.sprintf( "%p_%d", (void*)tb, (int)dock );
    return d->disabledDocks.find( s ) == d->disabledDocks.end();
}



/*!
    Adds \a dockWindow to the \a edge dock area.

    If \a newLine is false (the default) then the \a dockWindow is
    added at the end of the \a edge. For vertical edges the end is at
    the bottom, for horizontal edges (including \c Minimized) the end
    is at the right. If \a newLine is true a new line of dock windows
    is started with \a dockWindow as the first (left-most and
    top-most) dock window.

    If \a dockWindow is managed by another main window, it is first
    removed from that window.
*/

void TQMainWindow::addDockWindow( TQDockWindow *dockWindow,
			      Dock edge, bool newLine )
{
#ifdef TQ_WS_MAC
    if(isTopLevel() && edge == DockTop)
	ChangeWindowAttributes((WindowPtr)handle(), kWindowToolbarButtonAttribute, 0);
#endif
    moveDockWindow( dockWindow, edge );
    dockWindow->setNewLine( newLine );
    if ( d->dockWindows.find( dockWindow ) == -1 ) {
	d->dockWindows.append( dockWindow );
	connect( dockWindow, TQ_SIGNAL( placeChanged(TQDockWindow::Place) ),
		 this, TQ_SLOT( slotPlaceChanged() ) );
	dockWindow->installEventFilter( this );
    }
    dockWindow->setOpaqueMoving( d->opaque );
}


/*!
    \overload

    Adds \a dockWindow to the dock area with label \a label.

    If \a newLine is false (the default) the \a dockWindow is added at
    the end of the \a edge. For vertical edges the end is at the
    bottom, for horizontal edges (including \c Minimized) the end is
    at the right. If \a newLine is true a new line of dock windows is
    started with \a dockWindow as the first (left-most and top-most)
    dock window.

    If \a dockWindow is managed by another main window, it is first
    removed from that window.
*/

void TQMainWindow::addDockWindow( TQDockWindow * dockWindow, const TQString &label,
			      Dock edge, bool newLine )
{
    addDockWindow( dockWindow, edge, newLine );
#ifndef TQT_NO_TOOLBAR
    TQToolBar *tb = ::tqt_cast<TQToolBar*>(dockWindow);
    if ( tb )
	tb->setLabel( label );
#endif
}

/*!
    Moves \a dockWindow to the end of the \a edge.

    For vertical edges the end is at the bottom, for horizontal edges
    (including \c Minimized) the end is at the right.

    If \a dockWindow is managed by another main window, it is first
    removed from that window.
*/

void TQMainWindow::moveDockWindow( TQDockWindow * dockWindow, Dock edge )
{
    Orientation oo = dockWindow->orientation();
    switch ( edge ) {
    case DockTop:
	if ( dockWindow->area() != d->topDock )
	    dockWindow->removeFromDock( false );
	d->topDock->moveDockWindow( dockWindow );
	emit dockWindowPositionChanged( dockWindow );
	break;
    case DockBottom:
	if ( dockWindow->area() != d->bottomDock )
	    dockWindow->removeFromDock( false );
	d->bottomDock->moveDockWindow( dockWindow );
	emit dockWindowPositionChanged( dockWindow );
	break;
    case DockRight:
	if ( dockWindow->area() != d->rightDock )
	    dockWindow->removeFromDock( false );
	d->rightDock->moveDockWindow( dockWindow );
	emit dockWindowPositionChanged( dockWindow );
	break;
    case DockLeft:
	if ( dockWindow->area() != d->leftDock )
	    dockWindow->removeFromDock( false );
	d->leftDock->moveDockWindow( dockWindow );
	emit dockWindowPositionChanged( dockWindow );
	break;
    case DockTornOff:
	dockWindow->undock();
	break;
    case DockMinimized:
	dockWindow->undock( d->hideDock );
	break;
    case DockUnmanaged:
	break;
    }

    if ( oo != dockWindow->orientation() )
	dockWindow->setOrientation( dockWindow->orientation() );
}

/*!
    \overload

    Moves \a dockWindow to position \a index within the \a edge dock
    area.

    Any dock windows with positions \a index or higher have their
    position number incremented and any of these on the same line are
    moved right (down for vertical dock areas) to make room.

    If \a nl is true, a new dock window line is created below the line
    in which the moved dock window appears and the moved dock window,
    with any others with higher positions on the same line, is moved
    to this new line.

    The \a extraOffset is the space to put between the left side of
    the dock area (top side for vertical dock areas) and the dock
    window. (This is mostly used for restoring dock windows to the
    positions the user has dragged them to.)

    If \a dockWindow is managed by another main window, it is first
    removed from that window.
*/

void TQMainWindow::moveDockWindow( TQDockWindow * dockWindow, Dock edge, bool nl, int index, int extraOffset )
{
    Orientation oo = dockWindow->orientation();

    dockWindow->setNewLine( nl );
    dockWindow->setOffset( extraOffset );
    switch ( edge ) {
    case DockTop:
	if ( dockWindow->area() != d->topDock )
	    dockWindow->removeFromDock( false );
	d->topDock->moveDockWindow( dockWindow, index );
	break;
    case DockBottom:
	if ( dockWindow->area() != d->bottomDock )
	    dockWindow->removeFromDock( false );
	d->bottomDock->moveDockWindow( dockWindow, index );
	break;
    case DockRight:
	if ( dockWindow->area() != d->rightDock )
	    dockWindow->removeFromDock( false );
	d->rightDock->moveDockWindow( dockWindow, index );
	break;
    case DockLeft:
	if ( dockWindow->area() != d->leftDock )
	    dockWindow->removeFromDock( false );
	d->leftDock->moveDockWindow( dockWindow, index );
	break;
    case DockTornOff:
	dockWindow->undock();
	break;
    case DockMinimized:
	dockWindow->undock( d->hideDock );
	break;
    case DockUnmanaged:
	break;
    }

    if ( oo != dockWindow->orientation() )
	dockWindow->setOrientation( dockWindow->orientation() );
}

/*!
    Removes \a dockWindow from the main window's docking area,
    provided \a dockWindow is non-null and managed by this main
    window.
*/

void TQMainWindow::removeDockWindow( TQDockWindow * dockWindow )
{
#ifdef TQ_WS_MAC
    if(isTopLevel() && dockWindow->area() == topDock() && !dockWindows( DockTop ).count())
	ChangeWindowAttributes((WindowPtr)handle(), 0, kWindowToolbarButtonAttribute);
#endif

    dockWindow->hide();
    d->dockWindows.removeRef( dockWindow );
    disconnect( dockWindow, TQ_SIGNAL( placeChanged(TQDockWindow::Place) ),
		this, TQ_SLOT( slotPlaceChanged() ) );
    dockWindow->removeEventFilter( this );
}

/*!
    Sets up the geometry management of the window. It is called
    automatically when needed, so you shouldn't need to call it.
*/

void TQMainWindow::setUpLayout()
{
#ifndef TQT_NO_MENUBAR
    if ( !d->mb ) {
	// slightly evil hack here.  reconsider this
	TQObjectList * l
	    = ((TQObject*)this)->queryList( "TQMenuBar", 0, false, false );
	if ( l && l->count() )
	    d->mb = menuBar();
	delete l;
    }
#endif
    if ( !d->sb ) {
	// as above.
	TQObjectList * l
	    = ((TQObject*)this)->queryList( "TQStatusBar", 0, false, false );
	if ( l && l->count() )
	    d->sb = statusBar();
	delete l;
    }

    if (!d->tll) {
        d->tll = new TQBoxLayout( this, TQBoxLayout::Down );
        d->tll->setResizeMode( minimumSize().isNull() ? TQLayout::Minimum : TQLayout::FreeResize );
    } else {
        d->tll->setMenuBar( 0 );
        TQLayoutIterator it = d->tll->iterator();
        TQLayoutItem *item;
        while ( (item = it.takeCurrent()) )
	    delete item;
    }

#ifndef TQT_NO_MENUBAR
    if ( d->mb && d->mb->isVisibleTo( this ) ) {
	d->tll->setMenuBar( d->mb );
	if (style().styleHint(TQStyle::SH_MainWindow_SpaceBelowMenuBar, this))
	    d->tll->addSpacing( d->movable ? 1 : 2 );
    }
#endif

    d->tll->addWidget( d->hideDock );
    if(d->topDock->parentWidget() == this)
	d->tll->addWidget( d->topDock );

    TQMainWindowLayout *mwl = new TQMainWindowLayout( this, d->tll );
    d->tll->setStretchFactor( mwl, 1 );

    if(d->leftDock->parentWidget() == this)
	mwl->setLeftDock( d->leftDock );
    if ( centralWidget() )
	mwl->setCentralWidget( centralWidget() );
    if(d->rightDock->parentWidget() == this)
	mwl->setRightDock( d->rightDock );
    d->mwl = mwl;

    if(d->bottomDock->parentWidget() == this)
	d->tll->addWidget( d->bottomDock );

    if ( d->sb && d->sb->parentWidget() == this) {
	d->tll->addWidget( d->sb, 0 );
	// make the sb stay on top of tool bars if there isn't enough space
	d->sb->raise();
    }
}

/*!  \reimp */
void TQMainWindow::show()
{
    if ( !d->tll )
	setUpLayout();

    // show all floating dock windows not explicitly hidden
    if (!isVisible()) {
	TQPtrListIterator<TQDockWindow> it(d->dockWindows);
	while ( it.current() ) {
	    TQDockWindow *dw = it.current();
	    ++it;
	    if ( dw->isTopLevel() && !dw->isVisible() && !dw->testWState(WState_ForceHide) )
		dw->show();
	}
    }

    // show us last so we get focus
    TQWidget::show();
}


/*! \reimp
*/
void TQMainWindow::hide()
{
    if ( isVisible() ) {
	TQPtrListIterator<TQDockWindow> it(d->dockWindows);
	while ( it.current() ) {
	    TQDockWindow *dw = it.current();
	    ++it;
	    if ( dw->isTopLevel() && dw->isVisible() ) {
		dw->hide(); // implicit hide, so clear forcehide
		((TQMainWindow*)dw)->clearWState(WState_ForceHide);
	    }
	}
    }

    TQWidget::hide();
}


/*!  \reimp */
TQSize TQMainWindow::sizeHint() const
{
    TQMainWindow* that = (TQMainWindow*) this;
    // Workaround: because d->tll get's deleted in
    // totalSizeHint->polish->sendPostedEvents->childEvent->triggerLayout
    // [eg. canvas example on TQt/Embedded]
    TQApplication::sendPostedEvents( that, TQEvent::ChildInserted );
    if ( !that->d->tll )
	that->setUpLayout();
    return that->d->tll->totalSizeHint();
}

/*!  \reimp */
TQSize TQMainWindow::minimumSizeHint() const
{
    if ( !d->tll ) {
	TQMainWindow* that = (TQMainWindow*) this;
	that->setUpLayout();
    }
    return d->tll->totalMinimumSize();
}

/*!
    Sets the central widget for this main window to \a w.

    The central widget is surrounded by the left, top, right and
    bottom dock areas. The menu bar is above the top dock area.

    \sa centralWidget()
*/

void TQMainWindow::setCentralWidget( TQWidget * w )
{
    if ( d->mc )
	d->mc->removeEventFilter( this );
    d->mc = w;
    if ( d->mc )
	d->mc->installEventFilter( this );
    triggerLayout();
}


/*!
    Returns a pointer to the main window's central widget.

    The central widget is surrounded by the left, top, right and
    bottom dock areas. The menu bar is above the top dock area.

    \sa setCentralWidget()
*/

TQWidget * TQMainWindow::centralWidget() const
{
    return d->mc;
}


/*! \reimp */

void TQMainWindow::paintEvent( TQPaintEvent * )
{
    if (d->mb &&
	style().styleHint(TQStyle::SH_MainWindow_SpaceBelowMenuBar, this)) {
	TQPainter p( this );
	int y = d->mb->height() + 1;
	style().drawPrimitive(TQStyle::PE_Separator, &p, TQRect(0, y, width(), 1),
			      colorGroup(), TQStyle::Style_Sunken);
    }
}


bool TQMainWindow::dockMainWindow( TQObject *dock )
{
    while ( dock ) {
	if ( dock->parent() && dock->parent() == this )
	    return true;
	if ( ::tqt_cast<TQMainWindow*>(dock->parent()) )
	    return false;
	dock = dock->parent();
    }
    return false;
}

/*!
    \reimp
*/

bool TQMainWindow::eventFilter( TQObject* o, TQEvent *e )
{
    if ( e->type() == TQEvent::Show && o == this ) {
	if ( !d->tll )
	    setUpLayout();
	d->tll->activate();
    } else if ( e->type() == TQEvent::ContextMenu && d->dockMenu &&
	( ( ::tqt_cast<TQDockArea*>(o) && dockMainWindow( o ) ) || o == d->hideDock || o == d->mb ) ) {
	if ( showDockMenu( ( (TQMouseEvent*)e )->globalPos() ) ) {
	    ( (TQContextMenuEvent*)e )->accept();
	    return true;
	}
    }

    return TQWidget::eventFilter( o, e );
}


/*!
    Monitors events, recieved in \a e, to ensure the layout is updated.
*/
void TQMainWindow::childEvent( TQChildEvent* e)
{
    if ( e->type() == TQEvent::ChildRemoved ) {
	if ( e->child() == 0 ||
	     !e->child()->isWidgetType() ||
	     ((TQWidget*)e->child())->testWFlags( WType_TopLevel ) ) {
	    // nothing
	} else if ( e->child() == d->sb ) {
	    d->sb = 0;
	    triggerLayout();
	} else if ( e->child() == d->mb ) {
	    d->mb = 0;
	    triggerLayout();
	} else if ( e->child() == d->mc ) {
	    d->mc = 0;
	    d->mwl->setCentralWidget( 0 );
	    triggerLayout();
	} else if ( ::tqt_cast<TQDockWindow*>(e->child()) ) {
	    removeDockWindow( (TQDockWindow *)(e->child()) );
	    d->appropriate.remove( (TQDockWindow*)e->child() );
	    triggerLayout();
	}
    } else if ( e->type() == TQEvent::ChildInserted && !d->sb ) {
	d->sb = ::tqt_cast<TQStatusBar*>(e->child());
	if ( d->sb ) {
	    if ( d->tll ) {
		if ( !d->tll->findWidget( d->sb ) )
		    d->tll->addWidget( d->sb );
	    } else {
		triggerLayout();
	    }
	}
    }
}

/*!
    \reimp
*/

bool TQMainWindow::event( TQEvent * e )
{
    if ( e->type() == TQEvent::ChildRemoved && ( (TQChildEvent*)e )->child() == d->mc ) {
	d->mc->removeEventFilter( this );
	d->mc = 0;
	d->mwl->setCentralWidget( 0 );
    }

    return TQWidget::event( e );
}


/*!
    \property TQMainWindow::usesBigPixmaps
    \brief whether big pixmaps are enabled

    If false (the default), the tool buttons will use small pixmaps;
    otherwise big pixmaps will be used.

    Tool buttons and other widgets that wish to respond to this
    setting are responsible for reading the correct state on startup,
    and for connecting to the main window's widget's
    pixmapSizeChanged() signal.
*/

bool TQMainWindow::usesBigPixmaps() const
{
    return d->ubp;
}

void TQMainWindow::setUsesBigPixmaps( bool enable )
{
    if ( enable == (bool)d->ubp )
	return;

    d->ubp = enable;
    emit pixmapSizeChanged( enable );

    TQObjectList *l = queryList( "TQLayout" );
    if ( !l || !l->first() ) {
	delete l;
	return;
    }
    for ( TQLayout *lay = (TQLayout*)l->first(); lay; lay = (TQLayout*)l->next() )
	    lay->activate();
    delete l;
}

/*!
    \property TQMainWindow::usesTextLabel
    \brief whether text labels for toolbar buttons are enabled

    If disabled (the default), the tool buttons will not use text
    labels. If enabled, text labels will be used.

    Tool buttons and other widgets that wish to respond to this
    setting are responsible for reading the correct state on startup,
    and for connecting to the main window's widget's
    usesTextLabelChanged() signal.

    \sa TQToolButton::setUsesTextLabel()
*/

bool TQMainWindow::usesTextLabel() const
{
    return d->utl;
}


void TQMainWindow::setUsesTextLabel( bool enable )
{
    if ( enable == (bool)d->utl )
	return;

    d->utl = enable;
    emit usesTextLabelChanged( enable );

    TQObjectList *l = queryList( "TQLayout" );
    if ( !l || !l->first() ) {
	delete l;
	return;
    }
    for ( TQLayout *lay = (TQLayout*)l->first(); lay; lay = (TQLayout*)l->next() )
	    lay->activate();
    delete l;
}


/*!
    \fn void TQMainWindow::pixmapSizeChanged( bool )

    This signal is emitted whenever the setUsesBigPixmaps() is called
    with a value different to the current setting. All widgets that
    should respond to such changes, e.g. toolbar buttons, must connect
    to this signal.
*/

/*!
    \fn void TQMainWindow::usesTextLabelChanged( bool )

    This signal is emitted whenever the setUsesTextLabel() is called
    with a value different to the current setting. All widgets that
    should respond to such changes, e.g. toolbar buttons, must connect
    to this signal.
*/

/*!
    \fn void TQMainWindow::dockWindowPositionChanged( TQDockWindow *dockWindow )

    This signal is emitted when the \a dockWindow has changed its
    position. A change in position occurs when a dock window is moved
    within its dock area or moved to another dock area (including the
    \c Minimized and \c TearOff dock areas).

    \sa getLocation()
*/

void TQMainWindow::setRightJustification( bool enable )
{
    if ( enable == (bool)d->justify )
	return;
    d->justify = enable;
    triggerLayout( true );
}


/*!
    \obsolete
    \property TQMainWindow::rightJustification
    \brief whether the main window right-justifies its dock windows

    If disabled (the default), stretchable dock windows are expanded,
    and non-stretchable dock windows are given the minimum space they
    need. Since most dock windows are not stretchable, this usually
    results in an unjustified right edge (or unjustified bottom edge
    for a vertical dock area). If enabled, the main window will
    right-justify its dock windows.

    \sa TQDockWindow::setVerticalStretchable(), TQDockWindow::setHorizontalStretchable()
*/

bool TQMainWindow::rightJustification() const
{
    return d->justify;
}

/*! \internal
 */

void TQMainWindow::triggerLayout( bool deleteLayout )
{
    if ( deleteLayout || !d->tll )
	setUpLayout();
    TQApplication::postEvent( this, new TQEvent( TQEvent::LayoutHint ) );
}

/*!
    Enters 'What's This?' mode and returns immediately.

    This is the same as TQWhatsThis::enterWhatsThisMode(), but
    implemented as a main window object's slot. This way it can easily
    be used for popup menus, for example:

    \code
    TQPopupMenu * help = new TQPopupMenu( this );
    help->insertItem( "What's &This", this , TQ_SLOT(whatsThis()), SHIFT+Key_F1);
    \endcode

    \sa TQWhatsThis::enterWhatsThisMode()
*/
void TQMainWindow::whatsThis()
{
#ifndef TQT_NO_WHATSTHIS
    TQWhatsThis::enterWhatsThisMode();
#endif
}


/*!
    \reimp
*/

void TQMainWindow::styleChange( TQStyle& old )
{
    TQWidget::styleChange( old );
}

/*!
    Finds the location of the dock window \a dw.

    If the \a dw dock window is found in the main window the function
    returns true and populates the \a dock variable with the dw's dock
    area and the \a index with the dw's position within the dock area.
    It also sets \a nl to true if the \a dw begins a new line
    (otherwise false), and \a extraOffset with the dock window's offset.

    If the \a dw dock window is not found then the function returns
    false and the state of \a dock, \a index, \a nl and \a extraOffset
    is undefined.

    If you want to save and restore dock window positions then use
    operator>>() and operator<<().

    \sa operator>>() operator<<()
*/

bool TQMainWindow::getLocation( TQDockWindow *dw, Dock &dock, int &index, bool &nl, int &extraOffset ) const
{
    dock = DockTornOff;
    if ( d->topDock->hasDockWindow( dw, &index ) )
	dock = DockTop;
    else if ( d->bottomDock->hasDockWindow( dw, &index ) )
	dock = DockBottom;
    else if ( d->leftDock->hasDockWindow( dw, &index ) )
	dock = DockLeft;
    else if ( d->rightDock->hasDockWindow( dw, &index ) )
	dock = DockRight;
    else if ( dw->parentWidget() == d->hideDock ) {
	index = 0;
	dock = DockMinimized;
    } else {
	index = 0;
    }
    nl = dw->newLine();
    extraOffset = dw->offset();
    return true;
}

#ifndef TQT_NO_TOOLBAR
/*!
    Returns a list of all the toolbars which are in the \a dock dock
    area, regardless of their state.

    For example, the \c TornOff dock area may contain closed toolbars
    but these are returned along with the visible toolbars.

    \sa dockWindows()
*/

TQPtrList<TQToolBar> TQMainWindow::toolBars( Dock dock ) const
{
    TQPtrList<TQDockWindow> lst = dockWindows( dock );
    TQPtrList<TQToolBar> tbl;
    for ( TQDockWindow *w = lst.first(); w; w = lst.next() ) {
	TQToolBar *tb = ::tqt_cast<TQToolBar*>(w);
	if ( tb )
	    tbl.append( tb );
    }
    return tbl;
}
#endif

/*!
    Returns a list of all the dock windows which are in the \a dock
    dock area, regardless of their state.

    For example, the \c DockTornOff dock area may contain closed dock
    windows but these are returned along with the visible dock
    windows.
*/

TQPtrList<TQDockWindow> TQMainWindow::dockWindows( Dock dock ) const
{
    TQPtrList<TQDockWindow> lst;
    switch ( dock ) {
    case DockTop:
	return d->topDock->dockWindowList();
    case DockBottom:
	return d->bottomDock->dockWindowList();
    case DockLeft:
	return d->leftDock->dockWindowList();
    case DockRight:
	return d->rightDock->dockWindowList();
    case DockTornOff: {
	for ( TQDockWindow *w = d->dockWindows.first(); w; w = d->dockWindows.next() ) {
	    if ( !w->area() && w->place() == TQDockWindow::OutsideDock )
		lst.append( w );
	}
    }
    return lst;
    case DockMinimized: {
	if ( d->hideDock->children() ) {
	    TQObjectListIt it( *d->hideDock->children() );
	    TQObject *o;
	    while ( ( o = it.current() ) ) {
		++it;
		TQDockWindow *dw = ::tqt_cast<TQDockWindow*>(o);
		if ( !dw )
		    continue;
		lst.append( dw );
	    }
	}
    }
    return lst;
    default:
	break;
    }
    return lst;
}

/*!
    \overload

    Returns the list of dock windows which belong to this main window,
    regardless of which dock area they are in or what their state is,
    (e.g. irrespective of whether they are visible or not).
*/

TQPtrList<TQDockWindow> TQMainWindow::dockWindows() const
{
    return d->dockWindows;
}

void TQMainWindow::setDockWindowsMovable( bool enable )
{
    d->movable = enable;
    TQObjectList *l = queryList( "TQDockWindow" );
    if ( l ) {
	for ( TQObject *o = l->first(); o; o = l->next() )
	    ( (TQDockWindow*)o )->setMovingEnabled( enable );
    }
    delete l;
}

/*!
    \property TQMainWindow::dockWindowsMovable
    \brief whether the dock windows are movable

    If true (the default), the user will be able to move movable dock
    windows from one TQMainWindow dock area to another, including the
    \c TearOff area (i.e. where the dock window floats freely as a
    window in its own right), and the \c Minimized area (where only
    the dock window's handle is shown below the menu bar). Moveable
    dock windows can also be moved within TQMainWindow dock areas, i.e.
    to rearrange them within a dock area.

    If false the user will not be able to move any dock windows.

    By default dock windows are moved transparently (i.e. only an
    outline rectangle is shown during the drag), but this setting can
    be changed with setOpaqueMoving().

    \sa setDockEnabled(), setOpaqueMoving()
*/

bool TQMainWindow::dockWindowsMovable() const
{
    return d->movable;
}

void TQMainWindow::setOpaqueMoving( bool b )
{
    d->opaque = b;
    TQObjectList *l = queryList( "TQDockWindow" );
    if ( l ) {
	for ( TQObject *o = l->first(); o; o = l->next() )
	    ( (TQDockWindow*)o )->setOpaqueMoving( b );
    }
    delete l;
}

/*!
    \property TQMainWindow::opaqueMoving
    \brief whether dock windows are moved opaquely

    If true the dock windows of the main window are shown opaquely
    (i.e. it shows the toolbar as it looks when docked) whilst it is
    being moved. If false (the default) they are shown transparently,
    (i.e. as an outline rectangle).

    \warning Opaque moving of toolbars and dockwindows is known to
    have several problems. We recommend avoiding the use of this
    feature for the time being. We intend fixing the problems in a
    future release.
*/

bool TQMainWindow::opaqueMoving() const
{
    return d->opaque;
}

/*!
    This function will line up dock windows within the visible dock
    areas (\c Top, \c Left, \c Right and \c Bottom) as compactly as
    possible.

    If \a keepNewLines is true, all dock windows stay on their
    original lines. If \a keepNewLines is false then newlines may be
    removed to achieve the most compact layout possible.

    The method only works if dockWindowsMovable() returns true.
*/

void TQMainWindow::lineUpDockWindows( bool keepNewLines )
{
    if ( !dockWindowsMovable() )
	return;
    d->topDock->lineUp( keepNewLines );
    d->leftDock->lineUp( keepNewLines );
    d->rightDock->lineUp( keepNewLines );
    d->bottomDock->lineUp( keepNewLines );
}

/*!
    Returns true, if the dock window menu is enabled; otherwise
    returns false.

    The menu lists the (appropriate()) dock windows (which may be
    shown or hidden), and has a "Line Up Dock Windows" menu item. It
    will also have a "Customize" menu item if isCustomizable() returns
    true.

    \sa setDockEnabled(), lineUpDockWindows() appropriate()
    setAppropriate()
*/

bool TQMainWindow::isDockMenuEnabled() const
{
    return d->dockMenu;
}

/*!
    If \a b is true, then right clicking on a dock window or dock area
    will pop up the dock window menu. If \a b is false, right clicking
    a dock window or dock area will not pop up the menu.

    The menu lists the (appropriate()) dock windows (which may be
    shown or hidden), and has a "Line Up Dock Windows" item. It will
    also have a "Customize" menu item if isCustomizable() returns
    true.

    \sa lineUpDockWindows(), isDockMenuEnabled()
*/

void TQMainWindow::setDockMenuEnabled( bool b )
{
    d->dockMenu = b;
}

/*!
    Creates the dock window menu which contains all toolbars (if \a
    dockWindows is \c OnlyToolBars ), all dock windows (if \a
    dockWindows is \c NoToolBars) or all toolbars and dock windows (if
    \a dockWindows is \c AllDockWindows - the default).

    This function is called internally when necessary, e.g. when the
    user right clicks a dock area (providing isDockMenuEnabled()
    returns true).
\omit
### TQt 4.0
    You can reimplement this function if you wish to customize the
    behaviour.
\endomit

    The menu items representing the toolbars and dock windows are
    checkable. The visible dock windows are checked and the hidden
    dock windows are unchecked. The user can click a menu item to
    change its state (show or hide the dock window).

    The list and the state are always kept up-to-date.

    Toolbars and dock windows which are not appropriate in the current
    context (see setAppropriate()) are not listed in the menu.

    The menu also has a menu item for lining up the dock windows.

    If isCustomizable() returns true, a Customize menu item is added
    to the menu, which if clicked will call customize(). The
    isCustomizable() function we provide returns false and customize()
    does nothing, so they must be reimplemented in a subclass to be
    useful.
*/

TQPopupMenu *TQMainWindow::createDockWindowMenu( DockWindows dockWindows ) const
{
    TQObjectList *l = queryList( "TQDockWindow" );

    if ( !l || l->isEmpty() )
	return 0;

    delete l;

    TQPopupMenu *menu = new TQPopupMenu( (TQMainWindow*)this, "qt_customize_menu" );
    menu->setCheckable( true );
    d->dockWindowModes.replace( menu, dockWindows );
    connect( menu, TQ_SIGNAL( aboutToShow() ), this, TQ_SLOT( menuAboutToShow() ) );
    return menu;
}

/*!
    This slot is called from the aboutToShow() signal of the default
    dock menu of the mainwindow. The default implementation
    initializes the menu with all dock windows and toolbars in this
    slot.
\omit
### TQt 4.0
    If you want to do small adjustments to the menu, you can do it in
    this slot; or you can reimplement createDockWindowMenu().
\endomit
*/

void TQMainWindow::menuAboutToShow()
{
    TQPopupMenu *menu = (TQPopupMenu*)sender();
    TQMap<TQPopupMenu*, DockWindows>::Iterator it = d->dockWindowModes.find( menu );
    if ( it == d->dockWindowModes.end() )
	return;
    menu->clear();

    DockWindows dockWindows = *it;

    TQObjectList *l = queryList( "TQDockWindow" );

    bool empty = true;
    if ( l && !l->isEmpty() ) {

	TQObject *o = 0;
	if ( dockWindows == AllDockWindows || dockWindows == NoToolBars ) {
	    for ( o = l->first(); o; o = l->next() ) {
		TQDockWindow *dw = (TQDockWindow*)o;
		if ( !appropriate( dw ) || ::tqt_cast<TQToolBar*>(dw) || !dockMainWindow( dw ) )
		    continue;
		TQString label = dw->caption();
		if ( !label.isEmpty() ) {
		    int id = menu->insertItem( label, dw, TQ_SLOT( toggleVisible() ) );
		    menu->setItemChecked( id, dw->isVisible() );
		    empty = false;
		}
	    }
	    if ( !empty )
		menu->insertSeparator();
	}

	empty = true;

#ifndef TQT_NO_TOOLBAR
	if ( dockWindows == AllDockWindows || dockWindows == OnlyToolBars ) {
	    for ( o = l->first(); o; o = l->next() ) {
		TQToolBar *tb = ::tqt_cast<TQToolBar*>(o);
		if ( !tb || !appropriate(tb) || !dockMainWindow(tb) )
		    continue;
		TQString label = tb->label();
		if ( !label.isEmpty() ) {
		    int id = menu->insertItem( label, tb, TQ_SLOT( toggleVisible() ) );
		    menu->setItemChecked( id, tb->isVisible() );
		    empty = false;
		}
	    }
	}
#endif

    }

    delete l;

    if ( !empty )
	menu->insertSeparator();

    if ( dockWindowsMovable() )
	menu->insertItem( tr( "Line up" ), this, TQ_SLOT( doLineUp() ) );
    if ( isCustomizable() )
	menu->insertItem( tr( "Customize..." ), this, TQ_SLOT( customize() ) );
}

/*!
    Shows the dock menu at the position \a globalPos. The menu lists
    the dock windows so that they can be shown (or hidden), lined up,
    and possibly customized. Returns true if the menu is shown;
    otherwise returns false.

    If you want a custom menu, reimplement this function. You can
    create the menu from scratch or call createDockWindowMenu() and
    modify the result.
\omit
### TQt 4.0
    The default implementation uses the dock window menu which gets
    created by createDockWindowMenu(). You can reimplement
    createDockWindowMenu() if you want to use your own specialized
    popup menu.
\endomit
*/

bool TQMainWindow::showDockMenu( const TQPoint &globalPos )
{
    if ( !d->dockMenu )
	return false;
    if ( !d->rmbMenu )
	d->rmbMenu = createDockWindowMenu();
    if ( !d->rmbMenu )
	return false;

    d->rmbMenu->exec( globalPos );
    return true;
}

void TQMainWindow::slotPlaceChanged()
{
    TQObject* obj = (TQObject*)sender();
    TQDockWindow *dw = ::tqt_cast<TQDockWindow*>(obj);
    if ( dw )
	emit dockWindowPositionChanged( dw );
#ifndef TQT_NO_TOOLBAR
    TQToolBar *tb = ::tqt_cast<TQToolBar*>(obj);
    if ( tb )
	emit toolBarPositionChanged( tb );
#endif
}

/*!
    \internal
    For internal use of TQDockWindow only.
 */

TQDockArea *TQMainWindow::dockingArea( const TQPoint &p )
{
    int mh = d->mb ? d->mb->height() : 0;
    int sh = d->sb ? d->sb->height() : 0;
    if ( p.x() >= -5 && p.x() <= 100 && p.y() > mh && p.y() - height() - sh )
	return d->leftDock;
    if ( p.x() >= width() - 100 && p.x() <= width() + 5 && p.y() > mh && p.y() - height() - sh )
	return d->rightDock;
    if ( p.y() >= -5 && p.y() < mh + 100 && p.x() >= 0 && p.x() <= width() )
	return d->topDock;
    if ( p.y() >= height() - sh - 100 && p.y() <= height() + 5 && p.x() >= 0 && p.x() <= width() )
	return d->bottomDock;
    return 0;
}

/*!
    Returns true if \a dw is a dock window known to the main window;
    otherwise returns false.
*/

bool TQMainWindow::hasDockWindow( TQDockWindow *dw )
{
    return d->dockWindows.findRef( dw ) != -1;
}

/*!
    Returns the \c Left dock area

    \sa rightDock() topDock() bottomDock()
*/

TQDockArea *TQMainWindow::leftDock() const
{
    return d->leftDock;
}

/*!
    Returns the \c Right dock area

    \sa leftDock() topDock() bottomDock()
*/

TQDockArea *TQMainWindow::rightDock() const
{
    return d->rightDock;
}

/*!
    Returns the \c Top dock area

    \sa bottomDock() leftDock() rightDock()
*/

TQDockArea *TQMainWindow::topDock() const
{
    return d->topDock;
}

/*!
    Returns a pointer the \c Bottom dock area

    \sa topDock() leftDock() rightDock()
*/

TQDockArea *TQMainWindow::bottomDock() const
{
    return d->bottomDock;
}

/*!
    This function is called when the user clicks the Customize menu
    item on the dock window menu.

    The customize menu item will only appear if isCustomizable()
    returns true (it returns false by default).

    The function is intended, for example, to provide the user with a
    means of telling the application that they wish to customize the
    main window, dock windows or dock areas.

    The default implementation does nothing and the Customize menu
    item is not shown on the right-click menu by default. If you want
    the item to appear then reimplement isCustomizable() to return
    true, and reimplement this function to do whatever you want.

    \sa isCustomizable()
*/

void TQMainWindow::customize()
{
}

/*!
    Returns true if the dock area dock window menu includes the
    Customize menu item (which calls customize() when clicked).
    Returns false by default, i.e. the popup menu will not contain a
    Customize menu item. You will need to reimplement this function
    and set it to return true if you wish the user to be able to see
    the dock window menu.

    \sa customize()
*/

bool TQMainWindow::isCustomizable() const
{
    return false;
}

/*!
    Returns true if it is appropriate to include a menu item for the
    \a dw dock window in the dock window menu; otherwise returns
    false.

    The user is able to change the state (show or hide) a dock window
    that has a menu item by clicking the item.

    Call setAppropriate() to indicate whether or not a particular dock
    window should appear on the popup menu.

    \sa setAppropriate()
*/

bool TQMainWindow::appropriate( TQDockWindow *dw ) const
{
    TQMap<TQDockWindow*, bool>::ConstIterator it = d->appropriate.find( dw );
    if ( it == d->appropriate.end() )
	return true;
    return *it;
}

/*!
    Use this function to control whether or not the \a dw dock
    window's caption should appear as a menu item on the dock window
    menu that lists the dock windows.

    If \a a is true then the \a dw will appear as a menu item on the
    dock window menu. The user is able to change the state (show or
    hide) a dock window that has a menu item by clicking the item;
    depending on the state of your application, this may or may not be
    appropriate. If \a a is false the \a dw will not appear on the
    popup menu.

    \sa showDockMenu() isCustomizable() customize()
*/

void TQMainWindow::setAppropriate( TQDockWindow *dw, bool a )
{
    d->appropriate.replace( dw, a );
}

#ifndef TQT_NO_TEXTSTREAM
static void saveDockArea( TQTextStream &ts, TQDockArea *a )
{
    TQPtrList<TQDockWindow> l = a->dockWindowList();
    for ( TQDockWindow *dw = l.first(); dw; dw = l.next() ) {
	ts << TQString( dw->caption() );
	ts << ",";
    }
    ts << endl;
    ts << *a;
}

/*!
    \relates TQMainWindow

    Writes the layout (sizes and positions) of the dock windows in the
    dock areas of the TQMainWindow \a mainWindow, including \c
    Minimized and \c TornOff dock windows, to the text stream \a ts.

    This can be used, for example, in conjunction with TQSettings to
    save the user's layout when the \mainWindow receives a closeEvent.

    \sa operator>>() closeEvent()
*/

TQTextStream &operator<<( TQTextStream &ts, const TQMainWindow &mainWindow )
{
    TQPtrList<TQDockWindow> l = mainWindow.dockWindows( TQt::DockMinimized );
    TQDockWindow *dw = 0;
    for ( dw = l.first(); dw; dw = l.next() ) {
	ts << dw->caption();
	ts << ",";
    }
    ts << endl;

    l = mainWindow.dockWindows( TQt::DockTornOff );
    for ( dw = l.first(); dw; dw = l.next() ) {
	ts << dw->caption();
	ts << ",";
    }
    ts << endl;
    for ( dw = l.first(); dw; dw = l.next() ) {
	ts << "[" << dw->caption() << ","
	   << (int)dw->geometry().x() << ","
	   << (int)dw->geometry().y() << ","
	   << (int)dw->geometry().width() << ","
	   << (int)dw->geometry().height() << ","
	   << (int)dw->isVisible() << "]";
    }
    ts << endl;

    saveDockArea( ts, mainWindow.topDock() );
    saveDockArea( ts, mainWindow.bottomDock() );
    saveDockArea( ts, mainWindow.rightDock() );
    saveDockArea( ts, mainWindow.leftDock() );
    return ts;
}

static void loadDockArea( const TQStringList &names, TQDockArea *a, TQt::Dock d, TQPtrList<TQDockWindow> &l, TQMainWindow *mw, TQTextStream &ts )
{
    for ( TQStringList::ConstIterator it = names.begin(); it != names.end(); ++it ) {
	for ( TQDockWindow *dw = l.first(); dw; dw = l.next() ) {
	    if ( dw->caption() == *it ) {
		mw->addDockWindow( dw, d );
		break;
	    }
	}
    }
    if ( a ) {
	ts >> *a;
    } else if ( d == TQt::DockTornOff ) {
	TQString s = ts.readLine();
	enum State { Pre, Name, X, Y, Width, Height, Visible, Post };
	int state = Pre;
	TQString name, x, y, w, h, visible;
	TQChar c;
	for ( int i = 0; i < (int)s.length(); ++i ) {
	    c = s[ i ];
	    if ( state == Pre && c == '[' ) {
		state++;
		continue;
	    }
	    if ( c == ',' &&
		 ( state == Name || state == X || state == Y || state == Width || state == Height ) ) {
		state++;
		continue;
	    }
	    if ( state == Visible && c == ']' ) {
		for ( TQDockWindow *dw = l.first(); dw; dw = l.next() ) {
		    if ( TQString( dw->caption() ) == name ) {
			if ( !::tqt_cast<TQToolBar*>(dw) )
			    dw->setGeometry( x.toInt(), y.toInt(), w.toInt(), h.toInt() );
			else
			    dw->setGeometry( x.toInt(), y.toInt(), dw->width(), dw->height() );
			if ( !(bool)visible.toInt() )
			    dw->hide();
			else
			    dw->show();
			break;
		    }
		}

		name = x = y = w = h = visible = "";

		state = Pre;
		continue;
	    }
	    if ( state == Name )
		name += c;
	    else if ( state == X )
		x += c;
	    else if ( state == Y )
		y += c;
	    else if ( state == Width )
		w += c;
	    else if ( state == Height )
		h += c;
	    else if ( state == Visible )
		visible += c;
	}
    }
}

/*!
    \relates TQMainWindow

    Reads the layout (sizes and positions) of the dock windows in the
    dock areas of the TQMainWindow \a mainWindow from the text stream,
    \a ts, including \c Minimized and \c TornOff dock windows.
    Restores the dock windows and dock areas to these sizes and
    positions. The layout information must be in the format produced
    by operator<<().

    This can be used, for example, in conjunction with TQSettings to
    restore the user's layout.

    \sa operator<<()
*/

TQTextStream &operator>>( TQTextStream &ts, TQMainWindow &mainWindow )
{
    TQPtrList<TQDockWindow> l = mainWindow.dockWindows();

    TQString s = ts.readLine();
    TQStringList names = TQStringList::split( ',', s );
    loadDockArea( names, 0, TQt::DockMinimized, l, &mainWindow, ts );

    s = ts.readLine();
    names = TQStringList::split( ',', s );
    loadDockArea( names, 0, TQt::DockTornOff, l, &mainWindow, ts );

    int i = 0;
    TQDockArea *areas[] = { mainWindow.topDock(), mainWindow.bottomDock(), mainWindow.rightDock(), mainWindow.leftDock() };
    for ( int d = (int)TQt::DockTop; d != (int)TQt::DockMinimized; ++d, ++i ) {
	s = ts.readLine();
	names = TQStringList::split( ',', s );
	loadDockArea( names, areas[ i ], (TQt::Dock)d, l, &mainWindow, ts );
    }
    return ts;
}
#endif

#include "qmainwindow.moc"

#endif
