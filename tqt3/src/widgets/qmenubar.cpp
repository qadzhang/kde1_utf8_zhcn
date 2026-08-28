/****************************************************************************
**
** Implementation of TQMenuBar class
**
** Created : 941209
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

// ntqmainwindow.h before ntqmenubar.h because of GCC-2.7.* compatibility
// ### could be reorganised by discarding INCLUDE_MENUITEM_DEF and put
// the relevant declarations in a private header?
#include "ntqmainwindow.h"
#ifndef TQT_NO_MENUBAR
#include "ntqmenubar.h"
#include "ntqpopupmenu.h"
#include "ntqaccel.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqapplication.h"
#include "ntqguardedptr.h"
#include "ntqlayout.h"
#include "ntqcleanuphandler.h"
#include "../kernel/qinternal_p.h"
#include "ntqstyle.h"
#include "ntqtimer.h"
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif

class TQMenuDataData {
    // attention: also defined in qmenudata.cpp
public:
    TQMenuDataData();
    TQGuardedPtr<TQWidget> aWidget;
    int aInt;
};

#if defined(QT_ACCESSIBILITY_SUPPORT)
static bool inMenu = false;
#endif

#if defined(TQ_WS_X11)
extern int tqt_xfocusout_grab_counter; // defined in qapplication_x11.cpp
#endif

/*!
    \class TQMenuBar ntqmenubar.h
    \brief The TQMenuBar class provides a horizontal menu bar.

    \ingroup application
    \mainclass

    A menu bar consists of a list of pull-down menu items. You add
    menu items with \link TQMenuData::insertItem()
    insertItem()\endlink. For example, asuming that \c menubar is a
    pointer to a TQMenuBar and \c filemenu is a pointer to a
    TQPopupMenu, the following statement inserts the menu into the menu
    bar:
    \code
    menubar->insertItem( "&File", filemenu );
    \endcode
    The ampersand in the menu item's text sets Alt+F as a shortcut for
    this menu. (You can use "\&\&" to get a real ampersand in the menu
    bar.)

    Items are either enabled or disabled. You toggle their state with
    setItemEnabled().

    There is no need to lay out a menu bar. It automatically sets its
    own geometry to the top of the parent widget and changes it
    appropriately whenever the parent is resized.

    \important insertItem removeItem clear insertSeparator setItemEnabled isItemEnabled setItemVisible isItemVisible

    Example of creating a menu bar with menu items (from \l menu/menu.cpp):
    \quotefile menu/menu.cpp
    \skipto file = new TQPopupMenu
    \printline
    \skipto Key_O
    \printline
    \printline
    \skipto new TQMenuBar
    \printline
    \skipto insertItem
    \printline

    In most main window style applications you would use the menuBar()
    provided in TQMainWindow, adding \l{TQPopupMenu}s to the menu bar
    and adding \l{TQAction}s to the popup menus.

    Example (from \l action/application.cpp):
    \quotefile action/application.cpp
    \skipto file = new TQPopupMenu
    \printuntil fileNewAction

    Menu items can have text and pixmaps (or iconsets), see the
    various \link TQMenuData::insertItem() insertItem()\endlink
    overloads, as well as separators, see \link
    TQMenuData::insertSeparator() insertSeparator()\endlink. You can
    also add custom menu items that are derived from
    \l{TQCustomMenuItem}.

    Menu items may be removed with removeItem() and enabled or
    disabled with \link TQMenuData::setItemEnabled()
    setItemEnabled()\endlink.

    <img src=qmenubar-m.png> <img src=qmenubar-w.png>

    \section1 TQMenuBar on TQt/Mac

    TQMenuBar on TQt/Mac is a wrapper for using the system-wide menubar.
    If you have multiple menubars in one dialog the outermost menubar
    (normally inside a widget with widget flag \c WType_TopLevel) will
    be used for the system-wide menubar.

    Note that arbitrary TQt widgets \e cannot be inserted into a
    TQMenuBar on the Mac because TQt uses Mac's native menus which don't
    support this functionality. This limitation does not apply to
    stand-alone TQPopupMenus.

    TQt/Mac also provides a menubar merging feature to make TQMenuBar
    conform more closely to accepted Mac OS X menubar layout. The
    merging functionality is based on string matching the title of a
    TQPopupMenu entry. These strings are translated (using
    TQObject::tr()) in the "TQMenuBar" context. If an entry is moved its
    slots will still fire as if it was in the original place. The
    table below outlines the strings looked for and where the entry is
    placed if matched:

    \table
    \header \i String matches \i Placement \i Notes
    \row \i about.*
	 \i Application Menu | About <application name>
	 \i If this entry is not found no About item will appear in
	    the Application Menu
    \row \i config, options, setup, settings or preferences
	 \i Application Menu | Preferences
	 \i If this entry is not found the Settings item will be disabled
    \row \i quit or exit
	 \i Application Menu | Quit <application name>
	 \i If this entry is not found a default Quit item will be
	    created to call TQApplication::quit()
    \endtable

    \link menu-example.html menu/menu.cpp\endlink is an example of
    TQMenuBar and TQPopupMenu use.

    \sa TQPopupMenu TQAccel TQAction \link http://developer.apple.com/techpubs/macosx/Carbon/HumanInterfaceToolbox/Aqua/aqua.html Aqua Style Guidelines \endlink \link guibooks.html#fowler GUI Design Handbook: Menu Bar \endlink
*/


/*!
    \enum TQMenuBar::Separator

    This enum type is used to decide whether TQMenuBar should draw a
    separator line at its bottom.

    \value Never In many applications there is already a separator,
    and having two looks wrong.

    \value InWindowsStyle In some other applications a separator looks
    good in Windows style, but nowhere else.
*/

/*!
    \fn void TQMenuBar::activated( int id )

    This signal is emitted when a menu item is selected; \a id is the
    id of the selected item.

    Normally you will connect each menu item to a single slot using
    TQMenuData::insertItem(), but sometimes you will want to connect
    several items to a single slot (most often if the user selects
    from an array). This signal is useful in such cases.

    \sa highlighted(), TQMenuData::insertItem()
*/

/*!
    \fn void TQMenuBar::highlighted( int id )

    This signal is emitted when a menu item is highlighted; \a id is
    the id of the highlighted item.

    Normally, you will connect each menu item to a single slot using
    TQMenuData::insertItem(), but sometimes you will want to connect
    several items to a single slot (most often if the user selects
    from an array). This signal is useful in such cases.

    \sa activated(), TQMenuData::insertItem()
*/


// Motif style parameters

static const int motifBarHMargin	= 2;	// menu bar hor margin to item
static const int motifBarVMargin	= 1;	// menu bar ver margin to item
static const int motifItemFrame		= 2;	// menu item frame width
static const int motifItemHMargin	= 5;	// menu item hor text margin
static const int motifItemVMargin	= 4;	// menu item ver text margin

// The others are 0
static const int gtkItemHMargin = 8;
static const int gtkItemVMargin = 8;

/*

+-----------------------------
|      BarFrame
|   +-------------------------
|   |	   V  BarMargin
|   |	+---------------------
|   | H |      ItemFrame
|   |	|  +-----------------
|   |	|  |			   \
|   |	|  |  ^	 T E X T   ^	    | ItemVMargin
|   |	|  |  |		   |	   /
|   |	|      ItemHMargin
|   |
|

*/


/*****************************************************************************
  TQMenuBar member functions
 *****************************************************************************/


/*!
    Constructs a menu bar called \a name with parent \a parent.
*/
TQMenuBar::TQMenuBar( TQWidget *parent, const char *name )
    : TQFrame( parent, name, WNoAutoErase )
{
#if defined( TQ_WS_MAC ) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    mac_eaten_menubar = false;
    mac_d = 0;
    macCreateNativeMenubar();
#endif
    isMenuBar = true;
#ifndef TQT_NO_ACCEL
    autoaccel = 0;
#endif
    irects    = 0;
    rightSide = 0; // Right of here is rigth-aligned content
    mseparator = 0;
    waitforalt = 0;
    popupvisible = 0;
    hasmouse = 0;
    defaultup = 0;
    toggleclose = 0;
    pendingDelayedContentsChanges = 0;
    pendingDelayedStateChanges = 0;
    if ( parent ) {
	// filter parent events for resizing
	parent->installEventFilter( this );

	// filter top-level-widget events for accelerators
	TQWidget *tlw = topLevelWidget();
	if ( tlw != parent )
	    tlw->installEventFilter( this );
    }
    installEventFilter( this );

    setBackgroundMode( PaletteButton );
    setFrameStyle( TQFrame::MenuBarPanel | TQFrame::Raised );

    TQFontMetrics fm = fontMetrics();

    int h;
    int gs = style().styleHint(TQStyle::SH_GUIStyle);
    if (gs == GtkStyle) {
        h = fm.height() + gtkItemVMargin;
    } else {
        h = 2*motifBarVMargin + fm.height() + motifItemVMargin + 2*frameWidth() + 2*motifItemFrame;
    }

    setGeometry( 0, 0, width(), h );

    setMouseTracking( style().styleHint(TQStyle::SH_MenuBar_MouseTracking) );
}



/*! \reimp */

void TQMenuBar::styleChange( TQStyle& old )
{
    setMouseTracking( style().styleHint(TQStyle::SH_MenuBar_MouseTracking) );
    TQFrame::styleChange( old );
}



/*!
    Destroys the menu bar.
*/

TQMenuBar::~TQMenuBar()
{
#ifndef TQT_NO_ACCEL
    delete autoaccel;
#endif
#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    macRemoveNativeMenubar();
#endif
    if ( irects )		// Avoid purify complaint.
	delete [] irects;
}

/*!
    \internal

    Repaints the menu item with id \a id; does nothing if there is no
    such menu item.
*/
void TQMenuBar::updateItem( int id )
{
    int i = indexOf( id );
    if ( i >= 0 && irects )
	repaint( irects[i], false );
}

#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
static bool fromFrameChange = false;
#endif

/*!
    Recomputes the menu bar's display data according to the new
    contents.

    You should never need to call this; it is called automatically by
    TQMenuData whenever it needs to be called.
*/

void TQMenuBar::menuContentsChanged()
{
    // here the part that can't be delayed
    TQMenuData::menuContentsChanged();
    badSize = true;				// might change the size
    if( pendingDelayedContentsChanges )
        return;
    pendingDelayedContentsChanges = 1;
    if( !pendingDelayedStateChanges )// if the timer hasn't been started yet
        TQTimer::singleShot( 0, this, TQ_SLOT(performDelayedChanges()));
}

void TQMenuBar::performDelayedContentsChanged()
{
    pendingDelayedContentsChanges = 0;
    // here the part the can be delayed
#ifndef TQT_NO_ACCEL
    // if performDelayedStateChanged() will be called too,
    // it will call setupAccelerators() too, no need to do it twice
    if( !pendingDelayedStateChanges )
        setupAccelerators();
#endif
    calculateRects();
    if ( isVisible() ) {
	update();
#ifndef TQT_NO_MAINWINDOW
	TQMainWindow *mw = ::tqt_cast<TQMainWindow*>(parent());
	if ( mw ) {
	    mw->triggerLayout();
	    mw->update();
	}
#endif
#ifndef TQT_NO_LAYOUT
	if ( parentWidget() && parentWidget()->layout() )
	    parentWidget()->layout()->activate();
#endif
    }
}

/*!
    Recomputes the menu bar's display data according to the new state.

    You should never need to call this; it is called automatically by
    TQMenuData whenever it needs to be called.
*/

void TQMenuBar::menuStateChanged()
{
    if( pendingDelayedStateChanges )
        return;
    pendingDelayedStateChanges = 1;
    if( !pendingDelayedContentsChanges ) // if the timer hasn't been started yet
	TQTimer::singleShot( 0, this, TQ_SLOT(performDelayedChanges()));
}

void TQMenuBar::performDelayedStateChanged()
{
    pendingDelayedStateChanges = 0;
    // here the part that can be delayed
#ifndef TQT_NO_ACCEL
    setupAccelerators(); // ### when we have a good solution for the accel vs. focus
			 // widget problem, remove that. That is only a workaround
                         // if you remove this, see performDelayedContentsChanged()
#endif
    update();
}


void TQMenuBar::performDelayedChanges()
{
#if defined(TQ_WS_MAC) && !defined(TQMAC_MENUBAR_NO_NATIVE)
    // I must do this here as the values change in the function below.
    bool needMacUpdate = (pendingDelayedContentsChanges || pendingDelayedStateChanges);
#endif
    if( pendingDelayedContentsChanges )
        performDelayedContentsChanged();
    if( pendingDelayedStateChanges )
        performDelayedStateChanged();
#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    if(mac_eaten_menubar && needMacUpdate) {
	macDirtyNativeMenubar();

	bool all_hidden = true;
	if(irects) {
	    for(int i = 0; all_hidden && i < (int)mitems->count(); i++)
		all_hidden = irects[i].isEmpty();
	}
	if( all_hidden ) {
	    if( !isHidden())
		hide();
	} else {
	    if( !isShown() && !fromFrameChange )
		show();
	}
    }
#endif
}


void TQMenuBar::menuInsPopup( TQPopupMenu *popup )
{
    connect( popup, TQ_SIGNAL(activatedRedirect(int)),
	     TQ_SLOT(subActivated(int)) );
    connect( popup, TQ_SIGNAL(highlightedRedirect(int)),
	     TQ_SLOT(subHighlighted(int)) );
    connect( popup, TQ_SIGNAL(destroyed(TQObject*)),
	     this, TQ_SLOT(popupDestroyed(TQObject*)) );
}

void TQMenuBar::menuDelPopup( TQPopupMenu *popup )
{
    popup->disconnect( TQ_SIGNAL(activatedRedirect(int)) );
    popup->disconnect( TQ_SIGNAL(highlightedRedirect(int)) );
    disconnect( popup, TQ_SIGNAL(destroyed(TQObject*)),
		this, TQ_SLOT(popupDestroyed(TQObject*)) );
}

void TQMenuBar::frameChanged()
{
#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    fromFrameChange = true;
#endif
    menuContentsChanged();
#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    fromFrameChange = false;
#endif
}

void TQMenuBar::languageChange()
{
    menuContentsChanged();
}

/*!
    \internal

    This function is used to adjust the menu bar's geometry to the
    parent widget's geometry. Note that this is \e not part of the
    public interface - the function is \c public only because
    TQObject::eventFilter() is.

    Resizes the menu bar to fit in the parent widget when the parent
    receives a resize event.
*/

bool TQMenuBar::eventFilter( TQObject *object, TQEvent *event )
{
    if ( object == parent() && object
#ifndef TQT_NO_TOOLBAR
	 && !::tqt_cast<TQToolBar*>(object)
#endif
	 && event->type() == TQEvent::Resize ) {
	TQResizeEvent *e = (TQResizeEvent *)event;
	int w = e->size().width();
	setGeometry( 0, y(), w, heightForWidth(w) );
	return false;
    }

    if ( !isVisible() || !object->isWidgetType() )
	return false;

    if ( object == this && event->type() == TQEvent::LanguageChange ) {
	badSize = true;
	calculateRects();
	return false;
    } else if ( event->type() == TQEvent::MouseButtonPress ||
		event->type() == TQEvent::MouseButtonRelease ) {
	waitforalt = 0;
	return false;
    } else if ( waitforalt && event->type() == TQEvent::FocusOut ) {
	// some window systems/managers use alt/meta as accelerator keys
	// for switching between windows/desktops/etc.  If the focus
	// widget gets unfocused, then we need to stop waiting for alt
	// NOTE: this event came from the real focus widget, so we don't
	// need to touch the event filters
	waitforalt = 0;
	// although the comment above said not to remove the event filter, it is
	// incorrect. We need to remove our self fom the focused widget as normally
	// this happens in the key release but it does not happen in this case
	TQWidget * f = ((TQWidget *)object)->focusWidget();
	if (f)
	    f->removeEventFilter( this );
	return false;
    } else if ( !( event->type() == TQEvent::Accel ||
		event->type() == TQEvent::AccelOverride ||
		event->type() == TQEvent::KeyPress ||
		event->type() == TQEvent::KeyRelease ) ||
		!style().styleHint(TQStyle::SH_MenuBar_AltKeyNavigation, this) ) {
	return false;
    }

    TQKeyEvent * ke = (TQKeyEvent *) event;
#ifndef TQT_NO_ACCEL
    // look for Alt press and Alt-anything press
    if ( event->type() == TQEvent::Accel ) {
	TQWidget * f = ((TQWidget *)object)->focusWidget();
	// ### this thinks alt and meta are the same
	if ( ke->key() == Key_Alt || ke->key() == Key_Meta ) {
	    // A new Alt press and we wait for release, eat
	    // this key and don't wait for Alt on this widget
	    if ( waitforalt ) {
		waitforalt = 0;
		if ( object->parent() )
		    object->removeEventFilter( this );
		ke->accept();
		return true;
	    // Menu has focus, send focus back
	    } else if ( hasFocus() ) {
		setAltMode( false );
		ke->accept();
		return true;
	    // Start waiting for Alt release on focus widget
	    } else if ( ke->stateAfter() == AltButton ) {
		waitforalt = 1;
#if defined(TQ_WS_X11)
		TQMenuData::d->aInt = tqt_xfocusout_grab_counter;
#endif
		if ( f && f != object )
		    f->installEventFilter( this );
	    }
	// Other modifiers kills focus on menubar
	} else if ( ke->key() == Key_Control || ke->key() == Key_Shift) {
	    setAltMode( false );
	// Got other key, no need to wait for Alt release
	} else {
	    waitforalt = 0;
	}
	// ### ! block all accelerator events when the menu bar is active
	if ( tqApp && tqApp->focusWidget() == this ) {
	    return true;
	}

	return false;
    }
#endif
    // look for Alt release
    if ( ((TQWidget*)object)->focusWidget() == object ||
	 (object->parent() == 0 && ((TQWidget*)object)->focusWidget() == 0) ) {
	if ( waitforalt && event->type() == TQEvent::KeyRelease &&
	    ( ke->key() == Key_Alt || ke->key() == Key_Meta )
#if defined(TQ_WS_X11)
		&& TQMenuData::d->aInt == tqt_xfocusout_grab_counter
#endif
	    ) {
	    setAltMode( true );
	    if ( object->parent() )
		object->removeEventFilter( this );
	    TQWidget * tlw = ((TQWidget *)object)->topLevelWidget();
	    if ( tlw ) {
		// ### !
		// make sure to be the first event filter, so we can kill
		// accelerator events before the accelerators get to them.
		tlw->removeEventFilter( this );
		tlw->installEventFilter( this );
	    }
	    return true;
	// Cancel if next keypress is NOT Alt/Meta,
	} else if ( !hasFocus() && (event->type() == TQEvent::AccelOverride ) &&
		    !(((TQKeyEvent *)event)->key() == Key_Alt ||
		      ((TQKeyEvent *)event)->key() == Key_Meta) ) {
	    if ( object->parent() )
		object->removeEventFilter( this );
	    setAltMode( false );
	}
    }

    return false;				// don't stop event
}



/*!
  \internal
  Receives signals from menu items.
*/

void TQMenuBar::subActivated( int id )
{
    emit activated( id );
}

/*!
  \internal
  Receives signals from menu items.
*/

void TQMenuBar::subHighlighted( int id )
{
    emit highlighted( id );
}

/*!
  \internal
  Receives signals from menu accelerator.
*/
#ifndef TQT_NO_ACCEL
void TQMenuBar::accelActivated( int id )
{
#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    if(mac_eaten_menubar)
	return;
#endif
    if ( !isEnabled() )				// the menu bar is disabled
	return;
    setAltMode( true );
    setActiveItem( indexOf( id ) );
}
#endif

/*!
  \internal
  This slot receives signals from menu accelerator when it is about to be
  destroyed.
*/
#ifndef TQT_NO_ACCEL
void TQMenuBar::accelDestroyed()
{
    autoaccel = 0;				// don't delete it twice!
}
#endif

void TQMenuBar::popupDestroyed( TQObject *o )
{
    removePopup( (TQPopupMenu*)o );
}

bool TQMenuBar::tryMouseEvent( TQPopupMenu *, TQMouseEvent *e )
{
    TQPoint pos = mapFromGlobal( e->globalPos() );
    if ( !rect().contains( pos ) )		// outside
	return false;
    int item = itemAtPos( pos );
    if ( item == -1 && (e->type() == TQEvent::MouseButtonPress ||
			e->type() == TQEvent::MouseButtonRelease) ) {
	hidePopups();
	goodbye();
	return false;
    }
    TQMouseEvent ee( e->type(), pos, e->globalPos(), e->button(), e->state() );
    event( &ee );
    return true;
}


void TQMenuBar::tryKeyEvent( TQPopupMenu *, TQKeyEvent *e )
{
    event( e );
}


void TQMenuBar::goodbye( bool cancelled )
{
    mouseBtDn = false;
    popupvisible = 0;
    setAltMode( cancelled && style().styleHint(TQStyle::SH_MenuBar_AltKeyNavigation, this) );
}


void TQMenuBar::openActPopup()
{
#if defined(QT_ACCESSIBILITY_SUPPORT)
    if ( !inMenu ) {
	TQAccessible::updateAccessibility( this, 0, TQAccessible::MenuStart );
	inMenu = true;
    }
#endif

    if ( actItem < 0 )
	return;
    TQPopupMenu *popup = mitems->at(actItem)->popup();
    if ( !popup || !popup->isEnabled() )
	return;

    TQRect  r = itemRect( actItem );
    bool reverse = TQApplication::reverseLayout();
    const int yoffset = 1; //(style().styleHint( TQStyle::SH_GUIStyle ) == TQStyle::WindowsStyle) ? 4 : 1; ### this breaks designer mainwindow editing
    TQPoint pos = r.bottomLeft() + TQPoint(0,yoffset);
    if( reverse ) {
	pos = r.bottomRight() + TQPoint(0,yoffset);
	pos.rx() -= popup->sizeHint().width();
    }

    int ph = popup->sizeHint().height();
    pos = mapToGlobal(pos);
    int sh = TQApplication::desktop()->height();
    if ( defaultup || (pos.y() + ph > sh) ) {
	TQPoint t = mapToGlobal( r.topLeft() );
	if( reverse ) {
	    t = mapToGlobal( r.topRight() );
	    t.rx() -= popup->sizeHint().width();
	}
	t.ry() -= (TQCOORD)ph;
	if ( !defaultup || t.y() >= 0 )
	    pos = t;
    }

    //avoid circularity
    if ( popup->isVisible() )
	return;

    Q_ASSERT( popup->parentMenu == 0 );
    popup->parentMenu = this;			// set parent menu

    popup->snapToMouse = false;
    popup->popup( pos );
    popup->snapToMouse = true;
}

/*!
  \internal
  Hides all popup menu items.
*/

void TQMenuBar::hidePopups()
{
#if defined(QT_ACCESSIBILITY_SUPPORT)
    bool anyVisible = false;
#endif
    TQMenuItemListIt it(*mitems);
    TQMenuItem *mi;
    while ( (mi=it.current()) ) {
	++it;
	if ( mi->popup() && mi->popup()->isVisible() ) {
#if defined(QT_ACCESSIBILITY_SUPPORT)
	    anyVisible = true;
#endif
	    mi->popup()->hide();
	}
    }
#if defined(QT_ACCESSIBILITY_SUPPORT)
    if ( !popupvisible && anyVisible && inMenu ) {
	TQAccessible::updateAccessibility( this, 0, TQAccessible::MenuEnd );
	inMenu = false;
    }
#endif
}


/*!
    Reimplements TQWidget::show() in order to set up the correct
    keyboard accelerators and to raise itself to the top of the widget
    stack.
*/

void TQMenuBar::show()
{
#ifndef TQT_NO_ACCEL
    setupAccelerators();
#endif

    if ( parentWidget() )
	resize( parentWidget()->width(), height() );

    TQApplication::sendPostedEvents( this, TQEvent::Resize );
    performDelayedChanges();
    calculateRects();

#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    if(mac_eaten_menubar) {
	//If all elements are invisible no reason for me to be visible either
	bool all_hidden = true;
	if(irects) {
	    for(int i = 0; all_hidden && i < (int)mitems->count(); i++)
		all_hidden = irects[i].isEmpty();
	}
	if(all_hidden)
	    TQWidget::hide();
	else
	    TQWidget::show();
    } else {
	TQWidget::show();
    }
#else
    TQWidget::show();
#endif

#ifndef TQT_NO_MAINWINDOW
    TQMainWindow *mw = ::tqt_cast<TQMainWindow*>(parent());
    if ( mw ) //### ugly workaround
	mw->triggerLayout();
#endif
    raise();
}

/*!
    Reimplements TQWidget::hide() in order to deselect any selected
    item, and calls setUpLayout() for the main window.
*/

void TQMenuBar::hide()
{
    TQWidget::hide();
    setAltMode( false );
    hidePopups();
#ifndef TQT_NO_MAINWINDOW
    TQMainWindow *mw = ::tqt_cast<TQMainWindow*>(parent());
    if ( mw ) //### ugly workaround
	mw->triggerLayout();
#endif
}

/*!
  \internal
  Needs to change the size of the menu bar when a new font is set.
*/

void TQMenuBar::fontChange( const TQFont & f )
{
    badSize = true;
    updateGeometry();
    if ( isVisible() )
	calculateRects();
    TQWidget::fontChange( f );
}


/*****************************************************************************
  Item geometry functions
 *****************************************************************************/

/*
    This function serves two different purposes. If the parameter is
    negative, it updates the irects member for the current width and
    resizes. Otherwise, it does the same calculations for the GIVEN
    width and returns the height to which it WOULD have resized. A bit
    tricky, but both operations require almost identical steps.
*/
int TQMenuBar::calculateRects( int max_width )
{
    polish();
    bool update = ( max_width < 0 );

    if ( update ) {
	rightSide = 0;
	if ( !badSize )				// size was not changed
	    return 0;
	delete [] irects;
	int i = mitems->count();
	if ( i == 0 ) {
	    irects = 0;
	} else {
	    irects = new TQRect[ i ];
	    TQ_CHECK_PTR( irects );
	}
	max_width = width();
    }
    TQFontMetrics fm = fontMetrics();
    int max_height = 0;
    int max_item_height = 0;
    int nlitems = 0;				// number on items on cur line
    int gs = style().styleHint(TQStyle::SH_GUIStyle);
    bool reverse = TQApplication::reverseLayout();
    int x = frameWidth();
    int y = frameWidth();
    if ( gs == MotifStyle ) {
	x += motifBarHMargin;
	y += motifBarVMargin;
    } else if ( style().inherits("TQWindowsXPStyle") && style().styleHint(TQStyle::SH_TitleBar_NoBorder) ) {
        ;
    } else if ( gs == WindowsStyle ) {
	x += 2;
	y += 2;
    }
    if ( reverse )
	x = max_width - x;

    int i = 0;
    int separator = -1;
    const int itemSpacing = style().pixelMetric(TQStyle::PM_MenuBarItemSpacing);
    const int lastItem = reverse ? 0 : mitems->count() - 1;

    while ( i < (int)mitems->count() ) {	// for each menu item...
	TQMenuItem *mi = mitems->at(i);

	int w=0, h=0;
	if ( !mi->isVisible()
#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
				  ||  (mac_eaten_menubar && !mi->custom() && !mi->widget() )
#endif
	     ) {
	    ; // empty rectangle
	} else if ( mi->widget() ) {
	    if ( mi->widget()->parentWidget() != this ) {
		mi->widget()->reparent( this, TQPoint(0,0) );
	    }
	    w = mi->widget()->sizeHint().expandedTo( TQApplication::globalStrut() ).width()+2;
	    h = mi->widget()->sizeHint().expandedTo( TQApplication::globalStrut() ).height()+2;
	    if ( i && separator < 0 )
		separator = i;
	} else if ( mi->pixmap() ) {			// pixmap item
	    w = TQMAX( mi->pixmap()->width() + 4, TQApplication::globalStrut().width() );
	    h = TQMAX( mi->pixmap()->height() + 4, TQApplication::globalStrut().height() );
	} else if ( !mi->text().isNull() ) {	// text item
	    TQString s = mi->text();
        if ( gs == GtkStyle ) {
            w = fm.boundingRect( s ).width() + 2*gtkItemHMargin;
        } else {
	        w = fm.boundingRect( s ).width() + 2*motifItemHMargin;
        }
	    w -= s.contains('&')*fm.width('&');
	    w += s.contains("&&")*fm.width('&');
	    w = TQMAX( w, TQApplication::globalStrut().width() );
        if (gs == GtkStyle ) {
            h = TQMAX( fm.height() + gtkItemVMargin, TQApplication::globalStrut().height() );
        } else {
	        h = TQMAX( fm.height() + motifItemVMargin, TQApplication::globalStrut().height() );
        }
	} else if ( mi->isSeparator() ) {	// separator item
	    if ( style().styleHint(TQStyle::SH_GUIStyle) == MotifStyle )
		separator = i; //### only motif?
	}
	if ( !mi->isSeparator() || mi->widget() ) {
#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
	    if ( !mac_eaten_menubar ) {
#endif
		if ( gs == MotifStyle && mi->isVisible() ) {
		    w += 2*motifItemFrame;
		    h += 2*motifItemFrame;
		}
#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
	    }
#endif

	    if ( ( ( !reverse && x + w + frameWidth() - max_width > 0 ) ||
		 ( reverse && x - w - itemSpacing - frameWidth() < 0 ) )
		 && nlitems > 0 ) {
		nlitems = 0;
		x = frameWidth();
		y += h;
		if ( gs == MotifStyle ) {
		    x += motifBarHMargin;
		    y += motifBarVMargin;
		}
		if ( reverse )
		    x = max_width - x + itemSpacing;
		if ( style().styleHint(TQStyle::SH_GUIStyle) == MotifStyle )
		    separator = -1;
	    }
	    if ( y + h + 2*frameWidth() > max_height )
		max_height = y + h + 2*frameWidth();
	    if ( h > max_item_height )
		max_item_height = h;
	}

	const bool isLast = (i == lastItem);

	if( reverse ) {
	    x -= w;
	    if (!isLast && !mi->isSeparator())
		x -= itemSpacing;
	}
	if ( update ) {
	    irects[i].setRect( x, y, w, h );
	}
	if ( !reverse ) {
	    x += w;
	    if (!isLast && !mi->isSeparator())
		x += itemSpacing;
	}
	nlitems++;
	i++;
    }
    if ( gs == WindowsStyle ) {
	max_height += 2;
	max_width += 2;
    }

    if ( update ) {
	if ( separator >= 0 ) {
	    int moveBy = reverse ? - x - frameWidth() : max_width - x - frameWidth();
	    rightSide = x;
	    while( --i >= separator ) {
		irects[i].moveBy( moveBy, 0 );
	    }
	} else {
	    rightSide = width()-frameWidth();
	}
	if ( max_height != height() )
	    resize( width(), max_height );
	for ( i = 0; i < (int)mitems->count(); i++ ) {
	    irects[i].setHeight( max_item_height  );
	    TQMenuItem *mi = mitems->at(i);
	    if ( mi->widget() ) {
		TQRect r ( TQPoint(0,0), mi->widget()->sizeHint() );
		r.moveCenter( irects[i].center() );
		mi->widget()->setGeometry( r );
		if( mi->widget()->isHidden() )
		    mi->widget()->show();
	    }
	}
	badSize = false;
    }

    return max_height;
}

/*!
    Returns the height that the menu would resize itself to if its
    parent (and hence itself) resized to the given \a max_width. This
    can be useful for simple layout tasks in which the height of the
    menu bar is needed after items have been inserted. See \l
    showimg/showimg.cpp for an example of the usage.
*/
int TQMenuBar::heightForWidth(int max_width) const
{
    // Okay to cast away const, as we are not updating.
    if ( max_width < 0 ) max_width = 0;
    return ((TQMenuBar*)this)->calculateRects( max_width );
}

/*!
  \internal
  Return the bounding rectangle for the menu item at position \a index.
*/

TQRect TQMenuBar::itemRect( int index )
{
    calculateRects();
    return irects ? irects[index] : TQRect(0,0,0,0);
}

/*!
  \internal
  Return the item at \a pos, or -1 if there is no item there or if
  it is a separator item.
*/

int TQMenuBar::itemAtPos( const TQPoint &pos_ )
{
    calculateRects();
    if ( !irects )
	return -1;
    int i = 0;
    TQPoint pos = pos_;
    // Fitts' Law for edges - compensate for the extra margin
    // added in calculateRects()
    const int margin = 2;
    pos.setX( TQMAX( margin, TQMIN( width() - margin, pos.x())));
    pos.setY( TQMAX( margin, TQMIN( height() - margin, pos.y())));
    while ( i < (int)mitems->count() ) {
	if ( !irects[i].isEmpty() && irects[i].contains( pos ) ) {
	    TQMenuItem *mi = mitems->at(i);
	    return mi->isSeparator() ? -1 : i;
	}
	++i;
    }
    return -1;					// no match
}


/*!
  \property TQMenuBar::separator
  \brief in which cases a menubar sparator is drawn

  \obsolete
*/
void TQMenuBar::setSeparator( Separator when )
{
    mseparator = when;
}

TQMenuBar::Separator TQMenuBar::separator() const
{
    return mseparator ? InWindowsStyle : Never;
}

/*****************************************************************************
  Event handlers
 *****************************************************************************/

/*!
    Called from TQFrame::paintEvent(). Draws the menu bar contents
    using painter \a p.
*/

void TQMenuBar::drawContents( TQPainter *p )
{
    performDelayedChanges();
    TQRegion reg( contentsRect() );
    TQColorGroup g = colorGroup();
    bool e;

    // this shouldn't happen
    if ( !irects )
	return;

    for ( int i=0; i<(int)mitems->count(); i++ ) {
	TQMenuItem *mi = mitems->at( i );
	if ( !mi->text().isNull() || mi->pixmap() ) {
	    TQRect r = irects[i];
	    if(r.isEmpty() || !mi->isVisible())
		continue;
	    e = mi->isEnabledAndVisible();
	    if ( e )
		g = isEnabled() ? ( isActiveWindow() ? palette().active() :
				    palette().inactive() ) : palette().disabled();
	    else
		g = palette().disabled();
	    reg = reg.subtract( r );
	    TQSharedDoubleBuffer buffer( p, r );
	    buffer.painter()->setFont( p->font() );
	    buffer.painter()->setPen( p->pen() );
	    buffer.painter()->setBrush( p->brush() );

	    TQStyle::SFlags flags = TQStyle::Style_Default;
	    if (isEnabled() && e)
		flags |= TQStyle::Style_Enabled;
	    if ( i == actItem )
		flags |= TQStyle::Style_Active;
	    if ( actItemDown )
		flags |= TQStyle::Style_Down;
	    if (hasFocus() || hasmouse || popupvisible)
		flags |= TQStyle::Style_HasFocus;
	    style().drawControl(TQStyle::CE_MenuBarItem, buffer.painter(), this,
				r, g, flags, TQStyleOption(mi));
	}
    }

    p->save();
    p->setClipRegion(reg);
    style().drawControl(TQStyle::CE_MenuBarEmptyArea, p, this, contentsRect(), g);
    p->restore();

#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    if ( !mac_eaten_menubar )
#endif
    {
	TQt::GUIStyle gs = (TQt::GUIStyle) style().styleHint(TQStyle::SH_GUIStyle);
	if ( mseparator == InWindowsStyle && gs == WindowsStyle ) {
	    p->setPen( g.light() );
	    p->drawLine( 0, height()-1, width()-1, height()-1 );
	    p->setPen( g.dark() );
	    p->drawLine( 0, height()-2, width()-1, height()-2 );
	}
    }
}


/*!
    \reimp
*/
void TQMenuBar::mousePressEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    mouseBtDn = true;				// mouse button down
    int item = itemAtPos( e->pos() );
    if ( item == actItem && popupvisible )
	toggleclose = 1;
    if ( item >= 0 ) {
	TQFocusEvent::Reason oldReason = TQFocusEvent::reason();
	TQMenuItem *mi = findItem( idAt( item ) );
	// we know that a popup will open, so set the reason to avoid
	// itemviews to redraw their selections
	if ( mi && mi->popup() )
	    TQFocusEvent::setReason( TQFocusEvent::Popup );
	setAltMode( true );
	TQFocusEvent::setReason( oldReason );
    }
    setActiveItem( item, true, false );
}


/*!
    \reimp
*/
void TQMenuBar::mouseReleaseEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;
    if ( !mouseBtDn )
	return;
    mouseBtDn = false;				// mouse button up
    int item = itemAtPos( e->pos() );
    if ( ( item >= 0 && !mitems->at(item)->isEnabledAndVisible() ) ||
	 ( actItem >= 0 && !mitems->at(actItem)->isEnabledAndVisible() ) ) {
	hidePopups();
	setActiveItem( -1 );
	return;
    }
    bool showMenu = true;
    if ( toggleclose &&
	 // pressing an item twice closes in windows, but not in motif :/
	 style().styleHint(TQStyle::SH_GUIStyle) == WindowsStyle &&
	 actItem == item ) {
	showMenu = false;
	setAltMode( false );
    }
    setActiveItem( item, showMenu, !hasMouseTracking() );
    toggleclose = 0;
}


/*!
    \reimp
*/
void TQMenuBar::mouseMoveEvent( TQMouseEvent *e )
{
    int item = itemAtPos( e->pos() );
    if ( !mouseBtDn && !popupvisible) {
	if ( item >= 0 ) {
	    if ( !hasmouse ) {
		hasmouse = 1;
		if ( actItem == item )
		    actItem = -1; // trigger update
	    }
	}
	setActiveItem( item, false, false );
	return;
    }
    if ( item != actItem && item >= 0  && ( popupvisible || mouseBtDn ) )
	setActiveItem( item, true, false );
}


/*!
    \reimp
*/
void TQMenuBar::leaveEvent( TQEvent * e )
{
    hasmouse = 0;
    int actId = idAt( actItem );
    if ( !hasFocus() && !popupvisible )
	actItem = -1;
    updateItem( actId );
    TQFrame::leaveEvent( e );
}


/*!
    \reimp
*/
void TQMenuBar::keyPressEvent( TQKeyEvent *e )
{
    if ( actItem < 0 )
	return;

    TQMenuItem  *mi = 0;
    int dx = 0;

    if ( e->state() & TQt::ControlButton &&
	 ( e->key() == TQt::Key_Tab || e->key() == TQt::Key_Backtab ) )
    {
	e->ignore();
	return;
    }

    switch ( e->key() ) {
     case Key_Left:
	dx = TQApplication::reverseLayout() ? 1 : -1;
	break;

    case Key_Right:
    case Key_Tab:
	dx = TQApplication::reverseLayout() ? -1 : 1;
	break;

    case Key_Up:
    case Key_Down:
    case Key_Enter:
    case Key_Return:
	if ( style().styleHint(TQStyle::SH_MenuBar_AltKeyNavigation) )
	    setActiveItem( actItem );
	break;

    case Key_Escape:
	setAltMode( false );
	break;
    }

    if ( dx ) {					// highlight next/prev
	int i = actItem;
	int c = mitems->count();
	int n = c;
	while ( n-- ) {
	    i = i + dx;
	    if ( i == c )
		i = 0;
	    else if ( i < 0 )
		i = c - 1;
	    mi = mitems->at( i );
	    // ### fix windows-style traversal - currently broken due to
	    // TQMenuBar's reliance on TQPopupMenu
	    if ( /* (style() == WindowsStyle || */ mi->isEnabledAndVisible() /* ) */
		 && !mi->isSeparator() )
		break;
	}
	setActiveItem( i, popupvisible	 );
    } else if ( ( !e->state() || (e->state()&(MetaButton|AltButton)) ) && e->text().length()==1 && !popupvisible ) {
	TQChar c = e->text()[0].upper();

	TQMenuItemListIt it(*mitems);
	TQMenuItem* first = 0;
	TQMenuItem* currentSelected = 0;
	TQMenuItem* firstAfterCurrent = 0;

	TQMenuItem *m;
	int indx = 0;
	int clashCount = 0;
	while ( (m=it.current()) ) {
	    ++it;
	    TQString s = m->text();
	    if ( !s.isEmpty() ) {
		int i = s.find( '&' );
		if ( i >= 0 )
		{
		    if ( s[i+1].upper() == c ) {
			clashCount++;
			if ( !first )
			    first = m;
			if ( indx == actItem )
			    currentSelected = m;
			else if ( !firstAfterCurrent && currentSelected )
			    firstAfterCurrent = m;
		    }
		}
	    }
	    indx++;
	}
	if ( 0 == clashCount ) {
	    return;
	} else if ( 1 == clashCount ) {
 	    indx = indexOf( first->id() );
	} else {
	    // If there's clashes and no one is selected, use first one
	    // or if there is no clashes _after_ current, use first one
	    if ( !currentSelected || (currentSelected && !firstAfterCurrent))
		indx = indexOf( first->id() );
	    else
		indx = indexOf( firstAfterCurrent->id() );
	}

	setActiveItem( indx );
    }
}


/*!
    \reimp
*/
void TQMenuBar::resizeEvent( TQResizeEvent *e )
{
    TQFrame::resizeEvent( e );
    if ( badSize )
	return;
    badSize = true;
    calculateRects();
}

/*
    Sets actItem to \a i and calls repaint for the changed things.

    Takes care to optimize the repainting. Assumes that
    calculateRects() has been called as appropriate.
*/

void TQMenuBar::setActiveItem( int i, bool show, bool activate_first_item )
{
    if ( i == actItem && (uint)show == popupvisible )
	return;

    TQMenuItem* mi = 0;
    if ( i >= 0 )
	mi = mitems->at( i );
    if ( mi && !mi->isEnabledAndVisible() )
	return;

    popupvisible = i >= 0 ? (show) : 0;
    actItemDown = popupvisible;

    if ( i < 0 || actItem < 0 ) {
	// just one item needs repainting
	int n = TQMAX( actItem, i );
	actItem = i;
	if ( irects && n >= 0 )
	    repaint( irects[n], false );
    } else if ( TQABS(i-actItem) == 1 ) {
	// two neighbouring items need repainting
	int o = actItem;
	actItem = i;
	if ( irects )
	    repaint( irects[i].unite( irects[o] ), false );
    } else {
	// two non-neighbouring items need repainting
	int o = actItem;
	actItem = i;
	if ( irects ) {
	    repaint( irects[o], false );
	    repaint( irects[i], false );
	}
    }

    hidePopups();

    if ( !popupvisible && actItem >= 0 && irects ) {
	TQRect mfrect = irects[actItem];
	setMicroFocusHint( mfrect.x(), mfrect.y(), mfrect.width(), mfrect.height(), false );
    }

#if defined(QT_ACCESSIBILITY_SUPPORT)
    if ( mi )
	TQAccessible::updateAccessibility( this, indexOf( mi->id() )+1, TQAccessible::Focus );
#endif

    if ( actItem < 0 || !popupvisible || !mi  )
	return;

    TQPopupMenu *popup = mi->popup();
    if ( popup ) {
	emit highlighted( mi->id() );
	openActPopup();
	if ( activate_first_item )
	    popup->setFirstItemActive();
    } else {				// not a popup
	goodbye( false );
	if ( mi->signal() )			// activate signal
	    mi->signal()->activate();
	emit activated( mi->id() );
    }
}


void TQMenuBar::setAltMode( bool enable )
{
#if defined(QT_ACCESSIBILITY_SUPPORT)
    if ( inMenu && !enable ) {
	TQAccessible::updateAccessibility( this, 0, TQAccessible::MenuEnd );
	inMenu = false;
    } else if ( !inMenu && enable ) {
	TQAccessible::updateAccessibility( this, 0, TQAccessible::MenuStart );
	inMenu = true;
    }
#endif

    waitforalt = 0;
    actItemDown = false;
    if ( enable ) {
	if ( !TQMenuData::d->aWidget )
	    TQMenuData::d->aWidget = tqApp->focusWidget();
	setFocus();
	updateItem( idAt( actItem ) );
    } else {
	// set the focus back to the previous widget if
	// we still have the focus.
	if ( tqApp->focusWidget() == this ) {
	    if ( TQMenuData::d->aWidget )
		TQMenuData::d->aWidget->setFocus();
	    else
		clearFocus();
	}
	int actId = idAt( actItem );
	actItem = -1;
	updateItem( actId );
	TQMenuData::d->aWidget = 0;
    }
}

/*!
    Sets up keyboard accelerators for the menu bar.
*/
#ifndef TQT_NO_ACCEL

void TQMenuBar::setupAccelerators()
{
    delete autoaccel;
    autoaccel = 0;

    TQMenuItemListIt it(*mitems);
    TQMenuItem *mi;
    while ( (mi=it.current()) ) {
	++it;
	if ( !mi->isEnabledAndVisible() ) // ### when we have a good solution for the accel vs. focus widget problem, remove that. That is only a workaround
	    continue;
	TQString s = mi->text();
	if ( !s.isEmpty() ) {
	    int i = TQAccel::shortcutKey( s );
	    if ( i ) {
		if ( !autoaccel ) {
		    autoaccel = new TQAccel( this );
		    TQ_CHECK_PTR( autoaccel );
		    autoaccel->setIgnoreWhatsThis( true );
		    connect( autoaccel, TQ_SIGNAL(activated(int)),
			     TQ_SLOT(accelActivated(int)) );
		    connect( autoaccel, TQ_SIGNAL(activatedAmbiguously(int)),
			     TQ_SLOT(accelActivated(int)) );
		    connect( autoaccel, TQ_SIGNAL(destroyed()),
			     TQ_SLOT(accelDestroyed()) );
		}
		autoaccel->insertItem( i, mi->id() );
	    }
	}
	if ( mi->popup() ) {
	    TQPopupMenu* popup = mi->popup();
	    popup->updateAccel( this );
	    if ( !popup->isEnabled() )
		popup->enableAccel( false );
	}
    }
}
#endif

/*!
    \reimp
 */
bool TQMenuBar::customWhatsThis() const
{
    return true;
}



/*!
    \reimp
 */
void TQMenuBar::focusInEvent( TQFocusEvent * )
{
    if ( actItem < 0 ) {
	int i = -1;
	while ( actItem < 0 && ++i < (int) mitems->count() ) {
	    TQMenuItem* mi = mitems->at( i );
	    if ( mi && mi->isEnabledAndVisible() && !mi->isSeparator() )
		setActiveItem( i, false );
	}
    } else if ( !popupvisible ) {
	updateItem( idAt( actItem ) );
    }
}

/*!
    \reimp
 */
void TQMenuBar::focusOutEvent( TQFocusEvent * )
{
    updateItem( idAt( actItem ) );
    if ( !popupvisible )
	setAltMode( false );
}

/*!
    \reimp
*/

TQSize TQMenuBar::sizeHint() const
{
    int h = height();
    if ( badSize )
	h = ( (TQMenuBar*)this )->calculateRects();
    TQSize s( 2*frameWidth(),0);
    if ( irects ) {
	for ( int i = 0; i < (int)mitems->count(); ++i )
	    s.setWidth( s.width() + irects[ i ].width() + 2 );
    }
    s.setHeight( h );
    return (style().sizeFromContents(TQStyle::CT_MenuBar, this, s.
				     expandedTo(TQApplication::globalStrut())));
}

/*!
    \reimp
*/

TQSize TQMenuBar::minimumSize() const
{
#ifndef TQT_NO_TOOLBAR
    TQToolBar *tb = ::tqt_cast<TQToolBar*>(parent());
    if ( tb )
	return sizeHint();
#endif
    return TQFrame::minimumSize();
}

/*!
    \reimp
*/

TQSize TQMenuBar::minimumSizeHint() const
{
    return minimumSize();
}

/*!
    \property TQMenuBar::defaultUp
    \brief the popup orientation

    The default popup orientation. By default, menus pop "down" the
    screen. By setting the property to true, the menu will pop "up".
    You might call this for menus that are \e below the document to
    which they refer.

    If the menu would not fit on the screen, the other direction is
    used automatically.
*/
void TQMenuBar::setDefaultUp( bool on )
{
    defaultup = on;
}

bool TQMenuBar::isDefaultUp() const
{
    return defaultup;
}


/*!
    \reimp
 */
void TQMenuBar::activateItemAt( int index )
{
    if ( index >= 0 && index < (int) mitems->count() )
	setActiveItem( index );
    else
	goodbye( false );
}

#endif // TQT_NO_MENUBAR
