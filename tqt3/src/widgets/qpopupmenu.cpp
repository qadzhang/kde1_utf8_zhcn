/****************************************************************************
**
** Implementation of TQPopupMenu class
**
** Created : 941128
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

#include "ntqpopupmenu.h"
#ifndef TQT_NO_POPUPMENU
#include "ntqmenubar.h"
#include "ntqaccel.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqapplication.h"
#include "ntqpixmap.h"
#include "ntqpixmapcache.h"
#include "ntqtimer.h"
#include "ntqwhatsthis.h"
#include "ntqobjectlist.h"
#include "ntqguardedptr.h"
#include "qeffects_p.h"
#include "ntqcursor.h"
#include "ntqstyle.h"
#include "ntqtimer.h"
#include "ntqdatetime.h"
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif

//#define ANIMATED_POPUP
//#define BLEND_POPUP

// Motif style parameters

static const int motifArrowHMargin	= 6;	// arrow horizontal margin
static const int motifArrowVMargin	= 2;	// arrow vertical margin

static const int gtkArrowHMargin	= 0;	// arrow horizontal margin
static const int gtkArrowVMargin	= 0;	// arrow vertical margin

/*

+-----------------------------
|      PopupFrame
|   +-------------------------
|   |	   ItemFrame
|   |	+---------------------
|   |	|
|   |	|			   \
|   |	|   ^	T E X T	  ^	    | ItemVMargin
|   |	|   |		  |	   /
|   |	      ItemHMargin
|

*/

#if 0
# define DEBUG_SLOPPY_SUBMENU
#endif

// used for internal communication
static TQPopupMenu * syncMenu = 0;
static int syncMenuId = 0;

// Used to detect motion prior to mouse-release
static int motion;

// used to provide ONE single-shot timer
static TQTimer * singleSingleShot = 0;

static bool supressAboutToShow = false;

static void cleanup()
{
    delete singleSingleShot;
    singleSingleShot = 0;
}

static void popupSubMenuLater( int msec, TQPopupMenu * receiver ) {
    if ( !singleSingleShot ) {
	singleSingleShot = new TQTimer( tqApp, "popup submenu timer" );
	tqAddPostRoutine( cleanup );
    }

    singleSingleShot->disconnect( TQ_SIGNAL(timeout()) );
    TQObject::connect( singleSingleShot, TQ_SIGNAL(timeout()),
		      receiver, TQ_SLOT(subMenuTimer()) );
    singleSingleShot->start( msec, true );
}

static bool preventAnimation = false;

#ifndef TQT_NO_WHATSTHIS
extern void qWhatsThisBDH();
static TQMenuItem* whatsThisItem = 0;
#endif

/*!
    \class TQPopupMenu ntqpopupmenu.h
    \brief The TQPopupMenu class provides a popup menu widget.

    \ingroup application
    \ingroup basic
    \mainclass

    A popup menu widget is a selection menu. It can be either a
    pull-down menu in a menu bar or a standalone context (popup) menu.
    Pull-down menus are shown by the menu bar when the user clicks on
    the respective item or presses the specified shortcut key. Use
    TQMenuBar::insertItem() to insert a popup menu into a menu bar.
    Show a context menu either asynchronously with popup() or
    synchronously with exec().

    Technically, a popup menu consists of a list of menu items. You
    add items with insertItem(). An item is either a string, a pixmap
    or a custom item that provides its own drawing function (see
    TQCustomMenuItem). In addition, items can have an optional icon
    drawn on the very left side and an accelerator key such as
    "Ctrl+X".

    There are three kinds of menu items: separators, menu items that
    perform an action and menu items that show a submenu. Separators
    are inserted with insertSeparator(). For submenus, you pass a
    pointer to a TQPopupMenu in your call to insertItem(). All other
    items are considered action items.

    When inserting action items you usually specify a receiver and a
    slot. The receiver will be notifed whenever the item is selected.
    In addition, TQPopupMenu provides two signals, activated() and
    highlighted(), which signal the identifier of the respective menu
    item. It is sometimes practical to connect several items to one
    slot. To distinguish between them, specify a slot that takes an
    integer argument and use setItemParameter() to associate a unique
    value with each item.

    You clear a popup menu with clear() and remove single items with
    removeItem() or removeItemAt().

    A popup menu can display check marks for certain items when
    enabled with setCheckable(true). You check or uncheck items with
    setItemChecked().

    Items are either enabled or disabled. You toggle their state with
    setItemEnabled(). Just before a popup menu becomes visible, it
    emits the aboutToShow() signal. You can use this signal to set the
    correct enabled/disabled states of all menu items before the user
    sees it. The corresponding aboutToHide() signal is emitted when
    the menu hides again.

    You can provide What's This? help for single menu items with
    setWhatsThis(). See TQWhatsThis for general information about this
    kind of lightweight online help.

    For ultimate flexibility, you can also add entire widgets as items
    into a popup menu (for example, a color selector).

    A TQPopupMenu can also provide a tear-off menu. A tear-off menu is
    a top-level window that contains a copy of the menu. This makes it
    possible for the user to "tear off" frequently used menus and
    position them in a convenient place on the screen. If you want
    that functionality for a certain menu, insert a tear-off handle
    with insertTearOffHandle(). When using tear-off menus, bear in
    mind that the concept isn't typically used on Microsoft Windows so
    users may not be familiar with it. Consider using a TQToolBar
    instead. Tear-off menus cannot contain custom widgets; if the
    original menu contains a custom widget item, this item is omitted.

    \link menu-example.html menu/menu.cpp\endlink is an example of
    TQMenuBar and TQPopupMenu use.

    \important insertItem removeItem removeItemAt clear text pixmap iconSet insertSeparator changeItem whatsThis setWhatsThis accel setAccel setItemEnabled isItemEnabled setItemVisible isItemVisible setItemChecked isItemChecked connectItem disconnectItem setItemParameter itemParameter

    <img src=qpopmenu-m.png> <img src=qpopmenu-w.png>

    \sa TQMenuBar
    \link guibooks.html#fowler GUI Design Handbook: Menu, Drop-Down and
    Pop-Up\endlink
*/


/*!
    \fn void TQPopupMenu::aboutToShow()

    This signal is emitted just before the popup menu is displayed.
    You can connect it to any slot that sets up the menu contents
    (e.g. to ensure that the right items are enabled).

    \sa aboutToHide(), setItemEnabled(), setItemChecked(), insertItem(), removeItem()
*/

/*!
    \fn void TQPopupMenu::aboutToHide()

    This signal is emitted just before the popup menu is hidden after
    it has been displayed.

    \warning Do not open a widget in a slot connected to this signal.

    \sa aboutToShow(), setItemEnabled(), setItemChecked(), insertItem(), removeItem()
*/



/*****************************************************************************
  TQPopupMenu member functions
 *****************************************************************************/

class TQMenuDataData {
    // attention: also defined in qmenudata.cpp
public:
    TQMenuDataData();
    TQGuardedPtr<TQWidget> aWidget;
    int aInt;
};

class TQPopupMenuPrivate {
public:
    struct Scroll {
	enum { ScrollNone=0, ScrollUp=0x01, ScrollDown=0x02 };
	uint scrollable : 2;
	uint direction : 1;
	int topScrollableIndex, scrollableSize;
	TQTime lastScroll;
	TQTimer *scrolltimer;
    } scroll;
    TQSize calcSize;
    TQRegion mouseMoveBuffer;
    uint hasmouse : 1;
    TQPoint ignoremousepos;
};

static TQPopupMenu* active_popup_menu = 0;

/*!
    Constructs a popup menu called \a name with parent \a parent.

    Although a popup menu is always a top-level widget, if a parent is
    passed the popup menu will be deleted when that parent is
    destroyed (as with any other TQObject).
*/

TQPopupMenu::TQPopupMenu( TQWidget *parent, const char *name )
    : TQFrame( parent, name, WType_Popup  | WNoAutoErase )
{
    d = new TQPopupMenuPrivate;
    d->scroll.scrollableSize = d->scroll.topScrollableIndex = 0;
    d->scroll.scrollable = TQPopupMenuPrivate::Scroll::ScrollNone;
    d->scroll.scrolltimer = 0;
    d->hasmouse = 0;
    isPopupMenu	  = true;
#ifndef TQT_NO_ACCEL
    autoaccel	  = 0;
    accelDisabled = false;
#endif
    popupActive	  = -1;
    snapToMouse	  = true;
    tab = 0;
    checkable = 0;
    tornOff = 0;
    pendingDelayedContentsChanges = 0;
    pendingDelayedStateChanges = 0;
    maxPMWidth = 0;

    tab = 0;
    ncols = 1;
    setFrameStyle( TQFrame::PopupPanel | TQFrame::Raised );
    setMouseTracking(style().styleHint(TQStyle::SH_PopupMenu_MouseTracking, this));
    style().polishPopupMenu( this );
    setBackgroundMode( PaletteButton );
    connectModalRecursionSafety = 0;

    setFocusPolicy( StrongFocus );
#ifdef TQ_WS_X11
    x11SetWindowType( X11WindowTypePopup );
#endif
}

/*!
    Destroys the popup menu.
*/

TQPopupMenu::~TQPopupMenu()
{
    if ( syncMenu == this && tqApp ) {
	tqApp->exit_loop();
	syncMenu = 0;
    }

    if(d->scroll.scrolltimer)
	delete d->scroll.scrolltimer;

    if ( isVisible() ) {
        parentMenu = 0;
        hidePopups();
    }

    delete (TQWidget*) TQMenuData::d->aWidget;  // tear-off menu

    preventAnimation = false;
    delete d;
}


/*!
    Updates the item with identity \a id.
*/
void TQPopupMenu::updateItem( int id )		// update popup menu item
{
    updateRow( indexOf(id) );
}


void TQPopupMenu::setCheckable( bool enable )
{
    if ( isCheckable() != enable ) {
	checkable = enable;
	badSize = true;
	if ( TQMenuData::d->aWidget )
	    ( (TQPopupMenu*)(TQWidget*)TQMenuData::d->aWidget)->setCheckable( enable );
    }
}

/*!
    \property TQPopupMenu::checkable
    \brief whether the display of check marks on menu items is enabled

    When true, the display of check marks on menu items is enabled.
    Checking is always enabled when in Windows-style.

    \sa TQMenuData::setItemChecked()
*/

bool TQPopupMenu::isCheckable() const
{
    return checkable;
}

void TQPopupMenu::menuContentsChanged()
{
    // here the part that can't be delayed
    TQMenuData::menuContentsChanged();
    badSize = true;				// might change the size
#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    mac_dirty_popup = 1;
#endif
    if( pendingDelayedContentsChanges )
        return;
    pendingDelayedContentsChanges = 1;
    if( !pendingDelayedStateChanges ) // if the timer hasn't been started yet
	TQTimer::singleShot( 0, this, TQ_SLOT(performDelayedChanges()));
}

void TQPopupMenu::performDelayedContentsChanged()
{
    pendingDelayedContentsChanges = 0;
    // here the part the can be delayed
#ifndef TQT_NO_ACCEL
    // if performDelayedStateChanged() will be called too,
    // it will call updateAccel() too, no need to do it twice
    if( !pendingDelayedStateChanges )
        updateAccel( 0 );
#endif
    if ( isVisible() ) {
	if ( tornOff )
	    return;
	updateSize(true);
	update();
    }
    TQPopupMenu* p = (TQPopupMenu*)(TQWidget*)TQMenuData::d->aWidget;
    if ( p && p->isVisible() ) {
	p->updateSize(true);
	p->update();
    }
#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    mac_dirty_popup = 1;
#endif
}


void TQPopupMenu::menuStateChanged()
{
    // here the part that can't be delayed
    if( pendingDelayedStateChanges )
	return;
    pendingDelayedStateChanges = 1;
    if( !pendingDelayedContentsChanges ) // if the timer hasn't been started yet
	TQTimer::singleShot( 0, this, TQ_SLOT(performDelayedChanges()));
}

void TQPopupMenu::performDelayedStateChanged()
{
    pendingDelayedStateChanges = 0;
    // here the part that can be delayed
#ifndef TQT_NO_ACCEL
    updateAccel( 0 ); // ### when we have a good solution for the accel vs. focus widget problem, remove that. That is only a workaround
    // if you remove this, see performDelayedContentsChanged()
#endif
    update();
    if ( TQMenuData::d->aWidget )
	TQMenuData::d->aWidget->update();
}

void TQPopupMenu::performDelayedChanges()
{
    if( pendingDelayedContentsChanges )
	performDelayedContentsChanged();
    if( pendingDelayedStateChanges )
	performDelayedStateChanged();
}

void TQPopupMenu::menuInsPopup( TQPopupMenu *popup )
{
    connect( popup, TQ_SIGNAL(activatedRedirect(int)),
	     TQ_SLOT(subActivated(int)) );
    connect( popup, TQ_SIGNAL(highlightedRedirect(int)),
	     TQ_SLOT(subHighlighted(int)) );
    connect( popup, TQ_SIGNAL(destroyed(TQObject*)),
	     this, TQ_SLOT(popupDestroyed(TQObject*)) );
}

void TQPopupMenu::menuDelPopup( TQPopupMenu *popup )
{
    popup->disconnect( TQ_SIGNAL(activatedRedirect(int)) );
    popup->disconnect( TQ_SIGNAL(highlightedRedirect(int)) );
    disconnect( popup, TQ_SIGNAL(destroyed(TQObject*)),
		this, TQ_SLOT(popupDestroyed(TQObject*)) );
}


void TQPopupMenu::frameChanged()
{
    menuContentsChanged();
}

TQRect TQPopupMenu::screenRect( const TQPoint& pos )
{
    int screen_num = TQApplication::desktop()->screenNumber( pos );
#ifdef TQ_WS_MAC
    return TQApplication::desktop()->availableGeometry( screen_num );
#else
    return TQApplication::desktop()->screenGeometry( screen_num );
#endif
}
/*!
    Displays the popup menu so that the item number \a indexAtPoint
    will be at the specified \e global position \a pos. To translate a
    widget's local coordinates into global coordinates, use
    TQWidget::mapToGlobal().

    When positioning a popup with exec() or popup(), bear in mind that
    you cannot rely on the popup menu's current size(). For
    performance reasons, the popup adapts its size only when
    necessary, so in many cases, the size before and after the show is
    different. Instead, use sizeHint(). It calculates the proper size
    depending on the menu's current contents.
*/

void TQPopupMenu::popup( const TQPoint &pos, int indexAtPoint )
{
    if ( !isPopup() && isVisible() )
	hide();

    //avoid circularity
    if ( isVisible() || !isEnabled() )
	return;

#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    if( macPopupMenu(pos, indexAtPoint ))
	return;
#endif

#if (TQT_VERSION-0 >= 0x040000)
#error "Fix this now"
    // #### should move to TQWidget - anything might need this functionality,
    // #### since anything can have WType_Popup window flag.
    // #### This includes stuff in TQPushButton and some stuff for setting
    // #### the geometry of TQDialog.
    // TQPopupMenu
    // ::exec()
    // ::popup()
    // TQPushButton (shouldn't require TQMenuPopup)
    // ::popupPressed
    // Some stuff in qwidget.cpp for dialogs... can't remember exactly.
    // Also the code here indicatets the parameter should be a rect, not a
    // point.
#endif

    TQRect screen = screenRect( geometry().center());
    TQRect screen2 = screenRect( TQApplication::reverseLayout()
        ? pos+TQPoint(width(),0) : pos );
    // if the widget is not in the screen given by the position, move it
    // there, so that updateSize() uses the right size of the screen
    if( screen != screen2 ) {
        screen = screen2;
        move( screen.x(), screen.y());
    }
    if(d->scroll.scrollable) {
	d->scroll.scrollable = TQPopupMenuPrivate::Scroll::ScrollNone;
	d->scroll.topScrollableIndex = d->scroll.scrollableSize = 0;
	badSize = true;
    }
    updateSize();

    TQPoint mouse = TQCursor::pos();
    snapToMouse = pos == mouse;

    // have to emit here as a menu might be setup in a slot connected
    // to aboutToShow which will change the size of the menu
    bool s = supressAboutToShow;
    supressAboutToShow = true;
    if ( !s) {
	emit aboutToShow();
	updateSize(true);
    }

    int sw = screen.width();			// screen width
    int sh = screen.height();			// screen height
    int sx = screen.x();			// screen pos
    int sy = screen.y();
    int x  = pos.x();
    int y  = pos.y();
    if ( indexAtPoint >= 0 )			// don't subtract when < 0
	y -= itemGeometry( indexAtPoint ).y();		// (would subtract 2 pixels!)
    int w  = width();
    int h  = height();

    if ( snapToMouse ) {
	if ( tqApp->reverseLayout() )
	    x -= w;
	if ( x+w > sx+sw )
	    x = mouse.x()-w;
	if ( y+h > sy+sh )
	    y = mouse.y()-h;
	if ( x < sx )
	    x = mouse.x();
	if ( y < sy )
	    y = sy;
    }
#ifdef TQ_WS_X11
#ifndef TQT_NO_MENUBAR
    TQMenuData *top = this;		// find top level
    while ( top->parentMenu )
	top = top->parentMenu;
    if( top->isMenuBar )
        x11SetWindowType( X11WindowTypeDropdown );
    if( parentMenu && parentMenu->isMenuBar )
        x11SetWindowTransient( static_cast< TQMenuBar* >( parentMenu )->topLevelWidget());
#endif
    if( parentMenu && !parentMenu->isMenuBar )
        x11SetWindowTransient( static_cast< TQPopupMenu* >( parentMenu ));
    if( !parentMenu ) {
        // hackish ... try to find the main window related to this popup
        TQWidget* parent = parentWidget() ? parentWidget()->topLevelWidget() : NULL;
        if( parent == NULL )
            parent = TQApplication::widgetAt( pos );
        if( parent == NULL )
            parent = tqApp->activeWindow();
        if( parent != NULL )
            x11SetWindowTransient( parent );
    }
#endif

    if ( x+w > sx+sw )				// the complete widget must
	x = sx+sw - w;				//   be visible
    if ( y+h > sy+sh )
	y = sy+sh - h;
    if ( x < sx )
	x = sx;
    if ( y < sy )
	y = sy;

    if(style().styleHint(TQStyle::SH_PopupMenu_Scrollable, this)) {
	int off_top = 0, off_bottom = 0;
	if(y+h > sy+sh)
	    off_bottom = (y+h) - (sy+sh);
	if(y < sy)
	    off_top = sy - y;
	if(off_bottom || off_top) {
	    int ch = updateSize().height(); //store the old height, before setting scrollable --Sam
	    const int vextra = style().pixelMetric(TQStyle::PM_PopupMenuFrameVerticalExtra, this);
	    d->scroll.scrollableSize = h - off_top - off_bottom - 2*vextra;
	    if(off_top) {
		move( x, y = sy );
		d->scroll.scrollable = d->scroll.scrollable | TQPopupMenuPrivate::Scroll::ScrollUp;
	    }
	    if( off_bottom )
		d->scroll.scrollable = d->scroll.scrollable | TQPopupMenuPrivate::Scroll::ScrollDown;
	    if( off_top != off_bottom && indexAtPoint >= 0 ) {
		ch -= (vextra * 2);
		if(ch > sh) //no bigger than the screen!
		    ch = sh;
		if( ch > d->scroll.scrollableSize ) 
		    d->scroll.scrollableSize = ch;
	    }

	    updateSize(true); //now set the size using the scrollable/scrollableSize as above
            w = width();
            h = height();
	    if(indexAtPoint >= 0) { 
		if(off_top) { //scroll to it
		    TQMenuItem *mi = NULL;
		    TQMenuItemListIt it(*mitems);
		    for(int tmp_y = 0; tmp_y < off_top && (mi=it.current()); ) {
			TQSize sz = style().sizeFromContents(TQStyle::CT_PopupMenuItem, this,
							    TQSize(0, itemHeight( mi )),
							    TQStyleOption(mi,maxPMWidth,0));
			tmp_y += sz.height();
			d->scroll.topScrollableIndex++;
		    }
		} 
	    }
	}
    }
    move( x, y );
    motion=0;
    actItem = -1;

#ifndef TQT_NO_EFFECTS
    int hGuess = tqApp->reverseLayout() ? TQEffects::LeftScroll : TQEffects::RightScroll;
    int vGuess = TQEffects::DownScroll;
    if ( tqApp->reverseLayout() ) {
	if ( ( snapToMouse && ( x + w/2 > mouse.x() ) ) ||
	    ( parentMenu && parentMenu->isPopupMenu &&
	    ( x + w/2 > ((TQPopupMenu*)parentMenu)->x() ) ) )
	    hGuess = TQEffects::RightScroll;
    } else {
	if ( ( snapToMouse && ( x + w/2 < mouse.x() ) ) ||
	    ( parentMenu && parentMenu->isPopupMenu &&
	    ( x + w/2 < ((TQPopupMenu*)parentMenu)->x() ) ) )
	    hGuess = TQEffects::LeftScroll;
    }

#ifndef TQT_NO_MENUBAR
    if ( ( snapToMouse && ( y + h/2 < mouse.y() ) ) ||
	( parentMenu && parentMenu->isMenuBar &&
	( y + h/2 < ((TQMenuBar*)parentMenu)->mapToGlobal( ((TQMenuBar*)parentMenu)->pos() ).y() ) ) )
	vGuess = TQEffects::UpScroll;
#endif

    if ( TQApplication::isEffectEnabled( UI_AnimateMenu ) &&
	 preventAnimation == false ) {
	if ( TQApplication::isEffectEnabled( UI_FadeMenu ) )
	    qFadeEffect( this );
	else if ( parentMenu )
	    qScrollEffect( this, parentMenu->isPopupMenu ? hGuess : vGuess );
	else
	    qScrollEffect( this, hGuess | vGuess );
    } else
#endif
    {
	show();
    }
#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::PopupMenuStart );
#endif
}

/*!
    \fn void TQPopupMenu::activated( int id )

    This signal is emitted when a menu item is selected; \a id is the
    id of the selected item.

    Normally, you connect each menu item to a single slot using
    TQMenuData::insertItem(), but sometimes you will want to connect
    several items to a single slot (most often if the user selects
    from an array). This signal is useful in such cases.

    \sa highlighted(), TQMenuData::insertItem()
*/

/*!
    \fn void TQPopupMenu::highlighted( int id )

    This signal is emitted when a menu item is highlighted; \a id is
    the id of the highlighted item.

    \sa activated(), TQMenuData::insertItem()
*/

/*! \fn void TQPopupMenu::highlightedRedirect( int id )
  \internal
  Used internally to connect submenus to their parents.
*/

/*! \fn void TQPopupMenu::activatedRedirect( int id )
  \internal
  Used internally to connect submenus to their parents.
*/

void TQPopupMenu::subActivated( int id )
{
    emit activatedRedirect( id );
}

void TQPopupMenu::subHighlighted( int id )
{
    emit highlightedRedirect( id );
}

static bool fromAccel = false;

#ifndef TQT_NO_ACCEL
void TQPopupMenu::accelActivated( int id )
{
    TQMenuItem *mi = findItem( id );
    if ( mi && mi->isEnabledAndVisible() ) {
	TQGuardedPtr<TQSignal> signal = mi->signal();
	fromAccel = true;
	actSig( mi->id() );
	fromAccel = false;
	if ( signal )
	    signal->activate();
    }
}

void TQPopupMenu::accelDestroyed()		// accel about to be deleted
{
    autoaccel = 0;				// don't delete it twice!
}
#endif //TQT_NO_ACCEL

void TQPopupMenu::popupDestroyed( TQObject *o )
{
    removePopup( (TQPopupMenu*)o );
}

void TQPopupMenu::actSig( int id, bool inwhatsthis )
{
    if ( !inwhatsthis ) {
	emit activated( id );
#if defined(QT_ACCESSIBILITY_SUPPORT)
	if ( !fromAccel )
	    TQAccessible::updateAccessibility( this, indexOf(id)+1, TQAccessible::MenuCommand );
#endif
    } else {
#ifndef TQT_NO_WHATSTHIS
	TQRect r( itemGeometry( indexOf( id ) ) );
	TQPoint p( r.center().x(), r.bottom() );
	TQString whatsThis = findItem( id )->whatsThis();
	if ( whatsThis.isNull() )
	    whatsThis = TQWhatsThis::textFor( this, p );
	TQWhatsThis::leaveWhatsThisMode( whatsThis, mapToGlobal( p ), this );
#endif
    }

    emit activatedRedirect( id );
}

void TQPopupMenu::hilitSig( int id )
{
    emit highlighted( id );
    emit highlightedRedirect( id );

#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, indexOf(id)+1, TQAccessible::Focus );
    TQAccessible::updateAccessibility( this, indexOf(id)+1, TQAccessible::Selection );
#endif
}

void TQPopupMenu::setFirstItemActive()
{
    TQMenuItemListIt it(*mitems);
    TQMenuItem *mi;
    int ai = 0;
    if(d->scroll.scrollable)
	ai = d->scroll.topScrollableIndex;
    while ( (mi=it.current()) ) {
	++it;
	if ( !mi->isSeparator() && mi->id() != TQMenuData::d->aInt &&
	     ( style().styleHint( TQStyle::SH_PopupMenu_AllowActiveAndDisabled, this ) || mi->isEnabledAndVisible() )) {
	    setActiveItem( ai );
	    return;
	}
	ai++;
    }
    actItem = -1;
}

/*!
  \internal
  Hides all popup menus (in this menu tree) that are currently open.
*/

void TQPopupMenu::hideAllPopups()
{
    TQMenuData *top = this;		// find top level popup
    if ( !preventAnimation )
	TQTimer::singleShot( 10, this, TQ_SLOT(allowAnimation()) );
    preventAnimation = true;

    if ( !isPopup() )
	return; // nothing to do

    while ( top->parentMenu && top->parentMenu->isPopupMenu
	    && ((TQPopupMenu*)top->parentMenu)->isPopup() )
	top = top->parentMenu;
    ((TQPopupMenu*)top)->hide();			// cascade from top level

#ifndef TQT_NO_WHATSTHIS
    if (whatsThisItem) {
	qWhatsThisBDH();
	whatsThisItem = 0;
    }
#endif

}

/*!
  \internal
  Hides all popup sub-menus.
*/

void TQPopupMenu::hidePopups()
{
    if ( !preventAnimation )
	TQTimer::singleShot( 10, this, TQ_SLOT(allowAnimation()) );
    preventAnimation = true;

    TQMenuItemListIt it(*mitems);
    TQMenuItem *mi;
    while ( (mi=it.current()) ) {
	++it;
	if ( mi->popup() && mi->popup()->parentMenu == this ) //avoid circularity
	    mi->popup()->hide();
    }
    popupActive = -1;				// no active sub menu
    if(style().styleHint(TQStyle::SH_PopupMenu_SubMenuPopupDelay, this))
	d->mouseMoveBuffer = TQRegion();

    TQRect mfrect = itemGeometry( actItem );
    setMicroFocusHint( mfrect.x(), mfrect.y(), mfrect.width(), mfrect.height(), false );
}


/*!
  \internal
  Sends the event to the menu bar.
*/

bool TQPopupMenu::tryMenuBar( TQMouseEvent *e )
{
    TQMenuData *top = this;		// find top level
    while ( top->parentMenu )
	top = top->parentMenu;
#ifndef TQT_NO_MENUBAR
    return top->isMenuBar ?
	((TQMenuBar *)top)->tryMouseEvent( this, e ) :
			      ((TQPopupMenu*)top)->tryMouseEvent(this, e );
#else
    return ((TQPopupMenu*)top)->tryMouseEvent(this, e );
#endif
}


/*!
  \internal
*/
bool TQPopupMenu::tryMouseEvent( TQPopupMenu *p, TQMouseEvent * e)
{
    if ( p == this )
	return false;
    TQPoint pos = mapFromGlobal( e->globalPos() );
    if ( !rect().contains( pos ) )		// outside
	return false;
    TQMouseEvent ee( e->type(), pos, e->globalPos(), e->button(), e->state() );
    event( &ee );
    return true;
}

/*!
  \internal
  Tells the menu bar to go back to idle state.
*/

void TQPopupMenu::byeMenuBar()
{
#ifndef TQT_NO_MENUBAR
    TQMenuData *top = this;		// find top level
    while ( top->parentMenu )
	top = top->parentMenu;
#endif
    hideAllPopups();
#ifndef TQT_NO_MENUBAR
    if ( top->isMenuBar )
	((TQMenuBar *)top)->goodbye();
#endif
}


/*!
  \internal
  Return the item at \a pos, or -1 if there is no item there or if
  it is a separator item.
*/

int TQPopupMenu::itemAtPos( const TQPoint &pos, bool ignoreSeparator ) const
{
    if ( !contentsRect().contains(pos) )
	return -1;

    int row = 0;
    int x = contentsRect().x();
    int y = contentsRect().y();
    TQMenuItem *mi;
    TQMenuItemListIt it( *mitems );
    if(d->scroll.scrollable) {
	if(d->scroll.topScrollableIndex) {
	    for( ; (mi = it.current()) && row < d->scroll.topScrollableIndex; row++)
		++it;
	    if(!mi) {
		row = 0;
		it.toFirst();
	    }
	    y += style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this);
	}
    }
    int itemw = contentsRect().width() / ncols;
    TQSize sz;
    while ( (mi=it.current()) ) {
	if(d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollDown &&
	   y >= contentsRect().height() - style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this))
	    return -1;
	++it;
	if ( !mi->isVisible() ) {
	    ++row;
	    continue;
	}
	int itemh = itemHeight( mi );

	sz = style().sizeFromContents(TQStyle::CT_PopupMenuItem, this,
				      TQSize(0, itemh),
				      TQStyleOption(mi,maxPMWidth));
	sz = sz.expandedTo(TQSize(itemw, sz.height()));
	itemw = sz.width();
	itemh = sz.height();

	if ( ncols > 1 && y + itemh > contentsRect().bottom() ) {
	    y = contentsRect().y();
	    x +=itemw;
	}
	if ( TQRect( x, y, itemw, itemh ).contains( pos ) )
	    break;
	y += itemh;
	++row;
    }

    if ( mi && ( !ignoreSeparator || !mi->isSeparator() ) )
	return row;
    return -1;
}

/*!
  \internal
  Returns the geometry of item number \a index.
*/

TQRect TQPopupMenu::itemGeometry( int index )
{
    TQMenuItem *mi;
    TQSize sz;
    int row = 0, scrollh = 0;
    int x = contentsRect().x();
    int y = contentsRect().y();
    TQMenuItemListIt it( *mitems );
    if(d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollUp) {
	scrollh = style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this);
	y += scrollh;
	if(d->scroll.topScrollableIndex) {
	    for( ; (mi = it.current()) && row < d->scroll.topScrollableIndex; row++)
		++it;
	    if(!mi) {
		row = 0;
		it.toFirst();
	    }
	}
    }
    int itemw = contentsRect().width() / ncols;
    while ( (mi=it.current()) ) {
	if(d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollDown &&
	   y >= contentsRect().height() - scrollh)
	    break;
	++it;
	if ( !mi->isVisible() ) {
	    ++row;
	    continue;
	}
	int itemh = itemHeight( mi );

	sz = style().sizeFromContents(TQStyle::CT_PopupMenuItem, this,
				      TQSize(0, itemh),
				      TQStyleOption(mi,maxPMWidth));
	sz = sz.expandedTo(TQSize(itemw, sz.height()));
	itemw = sz.width();
	itemh = sz.height();
	if(d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollDown &&
	   (y + itemh > contentsRect().height() - scrollh))
	    itemh -= (y + itemh) - (contentsRect().height() - scrollh);
	if ( ncols > 1 && y + itemh > contentsRect().bottom() ) {
	    y = contentsRect().y();
	    x +=itemw;
	}
	if ( row == index )
	    return TQRect( x,y,itemw,itemh );
	y += itemh;
	++row;
    }

    return TQRect(0,0,0,0);
}


/*!
  \internal
  Calculates and sets the size of the popup menu, based on the size
  of the items.
*/

TQSize TQPopupMenu::updateSize(bool force_update, bool do_resize)
{
    polish();
    if ( count() == 0 ) {
	TQSize ret = TQSize( 50, 8 );
	if(do_resize)
	    setFixedSize( ret );
	badSize = true;
	return ret;
    }

    int scrheight = 0;
    if(d->scroll.scrollableSize) {
	if(d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollUp)
	    scrheight += style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this);
	if(d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollDown)
	    scrheight += style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this);
    }

    if(badSize || force_update) {
#ifndef TQT_NO_ACCEL
	updateAccel( 0 );
#endif
	int height = 0;
	int max_width = 0, max_height = 0;
	TQFontMetrics fm = fontMetrics();
	TQMenuItem *mi;
	maxPMWidth = 0;
	int maxWidgetWidth = 0;
	tab = 0;

	for ( TQMenuItemListIt it( *mitems ); it.current(); ++it ) {
	    mi = it.current();
	    TQWidget *miw = mi->widget();
	    if (miw) {
		if ( miw->parentWidget() != this )
		    miw->reparent( this, TQPoint(0,0), true );
		// widget items musn't propgate mouse events
		((TQPopupMenu*)miw)->setWFlags(WNoMousePropagation);
	    }
	    if ( mi->custom() )
		mi->custom()->setFont( font() );
	    if ( mi->iconSet() != 0)
		maxPMWidth = TQMAX( maxPMWidth,
				   mi->iconSet()->pixmap( TQIconSet::Small, TQIconSet::Normal ).width() + 4 );
	}

	int dh = screenRect( geometry().center()).height();
	ncols = 1;

	for ( TQMenuItemListIt it2( *mitems ); it2.current(); ++it2 ) {
	    mi = it2.current();
	    if ( !mi->isVisible() )
		continue;
	    int w = 0;
	    int itemHeight = TQPopupMenu::itemHeight( mi );

	    if ( mi->widget() ) {
		TQSize s( mi->widget()->sizeHint() );
		s = s.expandedTo( mi->widget()->minimumSize() );
		mi->widget()->resize( s );
		if ( s.width()  > maxWidgetWidth )
		    maxWidgetWidth = s.width();
		itemHeight = s.height();
	    } else {
		if( ! mi->isSeparator() ) {
		    if ( mi->custom() ) {
			if ( mi->custom()->fullSpan() ) {
			    maxWidgetWidth = TQMAX( maxWidgetWidth,
						   mi->custom()->sizeHint().width() );
			} else {
			    TQSize s ( mi->custom()->sizeHint() );
			    w += s.width();
			}
		    }

		    w += maxPMWidth;

		    if (! mi->text().isNull()) {
			TQString s = mi->text();
			int t;
			if ( (t = s.find('\t')) >= 0 ) { // string contains tab
			    w += fm.width( s, t );
			    w -= s.contains('&') * fm.width('&');
			    w += s.contains("&&") * fm.width('&');
			    int tw = fm.width( s.mid(t + 1) );
			    if ( tw > tab)
				tab = tw;
			} else {
			    w += fm.width( s );
			    w -= s.contains('&') * fm.width('&');
			    w += s.contains("&&") * fm.width('&');
			}
		    } else if (mi->pixmap())
			w += mi->pixmap()->width();
		} else {
		    if ( mi->custom() ) {
			TQSize s ( mi->custom()->sizeHint() );
			w += s.width();
		    } else {
			w = itemHeight = 2;
		    }
		}

		TQSize sz = style().sizeFromContents(TQStyle::CT_PopupMenuItem, this,
						    TQSize(w, itemHeight),
						    TQStyleOption(mi,maxPMWidth));

		w = sz.width();
		itemHeight = sz.height();

#if defined(QT_CHECK_NULL)
		if ( mi->text().isNull() && !mi->pixmap() && !mi->iconSet() &&
		     !mi->isSeparator() && !mi->widget() && !mi->custom() )
		    tqWarning( "TQPopupMenu: (%s) Popup has invalid menu item",
			      name( "unnamed" ) );
#endif
	    }
	    height += itemHeight;
	    if(style().styleHint(TQStyle::SH_PopupMenu_Scrollable, this)) {
		if(scrheight && height >= d->scroll.scrollableSize - scrheight) {
                    height = d->scroll.scrollableSize - scrheight;
		    break;
                }
	    } else if( height + 2*frameWidth() >= dh ) {
		ncols++;
		max_height = TQMAX(max_height, height - itemHeight);
		height = itemHeight;
	    }
	    if ( w > max_width )
		max_width = w;
	}
	if( ncols == 1 && !max_height )
	    max_height = height;

	if(style().styleHint(TQStyle::SH_PopupMenu_Scrollable, this)) {
	    height += scrheight;
	    setMouseTracking(true);
	}

	if ( tab )
	    tab -= fontMetrics().minRightBearing();
	else
	    max_width -= fontMetrics().minRightBearing();

	if ( max_width + tab < maxWidgetWidth )
	    max_width = maxWidgetWidth - tab;

	const int fw = frameWidth();
	int extra_width = (fw+style().pixelMetric(TQStyle::PM_PopupMenuFrameHorizontalExtra, this)) * 2,
	   extra_height = (fw+style().pixelMetric(TQStyle::PM_PopupMenuFrameVerticalExtra,   this)) * 2;
	if ( ncols == 1 )
	    d->calcSize = TQSize( TQMAX( minimumWidth(), max_width + tab + extra_width ),
			      TQMAX( minimumHeight() , height + extra_height ) );
	else
	    d->calcSize = TQSize( TQMAX( minimumWidth(), (ncols*(max_width + tab)) + extra_width ),
			      TQMAX( minimumHeight(), TQMIN( max_height + extra_height + 1, dh ) ) );
	badSize = false;
    }

    {
	// Position the widget items. It could be done in drawContents
	// but this way we get less flicker.
	TQSize sz;
	int x = contentsRect().x();
	int y = contentsRect().y();
	int itemw = contentsRect().width() / ncols;
	for(TQMenuItemListIt it(*mitems); it.current(); ++it) {
	    TQMenuItem *mi = it.current();
	    if ( !mi->isVisible() )
		continue;

	    int itemh = itemHeight( mi );

	    sz = style().sizeFromContents(TQStyle::CT_PopupMenuItem, this,
					  TQSize(0, itemh), TQStyleOption(mi,maxPMWidth));
	    sz = sz.expandedTo(TQSize(itemw, sz.height()));
	    itemw = sz.width();
	    itemh = sz.height();

	    if ( ncols > 1 && y + itemh > contentsRect().bottom() ) {
		y = contentsRect().y();
		x +=itemw;
	    }
	    if ( mi->widget() )
		mi->widget()->setGeometry( x, y, itemw, mi->widget()->height() );
	    y += itemh;
	}
    }

    if( do_resize && size() != d->calcSize ) {
	setMaximumSize( d->calcSize );
        d->calcSize = maximumSize(); //let the max size adjust it (virtual)
	resize( d->calcSize );
    }
    return d->calcSize;
}


#ifndef TQT_NO_ACCEL
/*!
  \internal
  The \a parent is 0 when it is updated when a menu item has
  changed a state, or it is something else if called from the menu bar.
*/

void TQPopupMenu::updateAccel( TQWidget *parent )
{
    TQMenuItemListIt it(*mitems);
    TQMenuItem *mi;

    if ( parent ) {
	delete autoaccel;
	autoaccel = 0;
    } else if ( !autoaccel ) {
	// we have no parent. Rather than ignoring any accelerators we try to find this popup's main window
	if ( tornOff ) {
	    parent = this;
	} else {
	    TQWidget *w = (TQWidget *) this;
	    parent = w->parentWidget();
	    while ( (!w->testWFlags(WType_TopLevel) || !w->testWFlags(WType_Popup)) && parent ) {
		w = parent;
		parent = parent->parentWidget();
	    }
	}
    }

    if ( parent == 0 && autoaccel == 0 )
 	return;

    if ( autoaccel )				// build it from scratch
	autoaccel->clear();
    else {
	// create an autoaccel in any case, even if we might not use
	// it immediately. Maybe the user needs it later.
	autoaccel = new TQAccel( parent, this );
	connect( autoaccel, TQ_SIGNAL(activated(int)),
		 TQ_SLOT(accelActivated(int)) );
	connect( autoaccel, TQ_SIGNAL(activatedAmbiguously(int)),
		 TQ_SLOT(accelActivated(int)) );
	connect( autoaccel, TQ_SIGNAL(destroyed()),
		 TQ_SLOT(accelDestroyed()) );
	if ( accelDisabled )
	    autoaccel->setEnabled( false );
    }
    while ( (mi=it.current()) ) {
	++it;
	TQKeySequence k = mi->key();
	if ( (int)k ) {
	    int id = autoaccel->insertItem( k, mi->id() );
#ifndef TQT_NO_WHATSTHIS
	    autoaccel->setWhatsThis( id, mi->whatsThis() );
#endif
	}
	if ( !mi->text().isNull() || mi->custom() ) {
	    TQString s = mi->text();
	    int i = s.find('\t');

	    // Note: Only looking at the first key in the sequence!
	    if ( (int)k && (int)k != Key_unknown ) {
		TQString t = (TQString)mi->key();
		if ( i >= 0 )
		    s.replace( i+1, s.length()-i, t );
		else {
		    s += '\t';
		    s += t;
		}
	    } else if ( !k ) {
 		if ( i >= 0 )
 		    s.truncate( i );
	    }
	    if ( s != mi->text() ) {
		mi->setText( s );
		badSize = true;
	    }
	}
	if ( mi->popup() && parent ) {		// call recursively
	    // reuse
	    TQPopupMenu* popup = mi->popup();
	    if (!popup->avoid_circularity) {
		popup->avoid_circularity = 1;
		popup->updateAccel( parent );
		popup->avoid_circularity = 0;
	    }
	}
    }
}

/*!
  \internal
  It would be better to check in the slot.
*/

void TQPopupMenu::enableAccel( bool enable )
{
    if ( autoaccel )
	autoaccel->setEnabled( enable );
    accelDisabled = !enable;		// rememeber when updateAccel
    TQMenuItemListIt it(*mitems);
    TQMenuItem *mi;
    while ( (mi=it.current()) ) {		// do the same for sub popups
	++it;
	if ( mi->popup() )			// call recursively
	    mi->popup()->enableAccel( enable );
    }
}
#endif

/*!
    \reimp
*/
void TQPopupMenu::setFont( const TQFont &font )
{
    TQWidget::setFont( font );
    badSize = true;
    if ( isVisible() ) {
	updateSize();
	update();
    }
}

/*!
    \reimp
*/
void TQPopupMenu::show()
{
    if ( !isPopup() && isVisible() )
	hide();

    if ( isVisible() ) {
	supressAboutToShow = false;
	TQWidget::show();
	return;
    }
    if (!supressAboutToShow)
	emit aboutToShow();
    else
	supressAboutToShow = false;
    performDelayedChanges();
    updateSize(true);
    TQWidget::show();
    updateSize();
    popupActive = -1;
    if(style().styleHint(TQStyle::SH_PopupMenu_SubMenuPopupDelay, this))
	d->mouseMoveBuffer = TQRegion();
    d->ignoremousepos = TQCursor::pos();
}

/*!
    \reimp
*/

void TQPopupMenu::hide()
{
    if ( syncMenu == this && tqApp ) {
	tqApp->exit_loop();
	syncMenu = 0;
    }

    if ( !isVisible() ) {
	TQWidget::hide();
  	return;
    }
    emit aboutToHide();

    actItem = popupActive = -1;
    if(style().styleHint(TQStyle::SH_PopupMenu_SubMenuPopupDelay, this))
	d->mouseMoveBuffer = TQRegion();
    mouseBtDn = false;				// mouse button up
#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::PopupMenuEnd );
#endif
#ifndef TQT_NO_MENUBAR
    TQMenuData *top = this;		// find top level
    while ( top->parentMenu )
	top = top->parentMenu;
    if( top->isMenuBar )
        x11SetWindowType( X11WindowTypePopup ); // reset
#endif
    parentMenu = 0;
    hidePopups();
    TQWidget::hide();
}


/*!
    Calculates the height in pixels of the item in row \a row.
*/
int TQPopupMenu::itemHeight( int row ) const
{
    return itemHeight( mitems->at( row ) );
}

/*!
    \overload

    Calculates the height in pixels of the menu item \a mi.
*/
int TQPopupMenu::itemHeight( TQMenuItem *mi ) const
{
    if  ( mi->widget() )
	return mi->widget()->height();
    if ( mi->custom() && mi->custom()->fullSpan() )
	return mi->custom()->sizeHint().height();

    TQFontMetrics fm(fontMetrics());
    int h = 0;
    if ( mi->isSeparator() ) // separator height
        h = 2;
    else if ( mi->pixmap() ) // pixmap height
        h = mi->pixmap()->height();
    else                     // text height
        h = fm.height();

    if ( !mi->isSeparator() && mi->iconSet() != 0 )
        h = TQMAX(h, mi->iconSet()->pixmap( TQIconSet::Small,
					   TQIconSet::Normal ).height());
    if ( mi->custom() )
        h = TQMAX(h, mi->custom()->sizeHint().height());

    return h;
}


/*!
    Draws menu item \a mi in the area \a x, \a y, \a w, \a h, using
    painter \a p. The item is drawn active if \a act is true or drawn
    inactive if \a act is false. The rightmost \a tab_ pixels are used
    for accelerator text.

    \sa TQStyle::drawControl()
*/
void TQPopupMenu::drawItem( TQPainter* p, int tab_, TQMenuItem* mi,
			   bool act, int x, int y, int w, int h)
{
    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled() && mi->isEnabledAndVisible() && (!mi->popup() || mi->popup()->isEnabled()) )
	flags |= TQStyle::Style_Enabled;
    if (act)
	flags |= TQStyle::Style_Active;
    if (mouseBtDn)
	flags |= TQStyle::Style_Down;

    const TQColorGroup &cg = ((flags&TQStyle::Style_Enabled) ? colorGroup() : palette().disabled() );

    if ( mi->custom() && mi->custom()->fullSpan() ) {
	TQMenuItem dummy;
	style().drawControl(TQStyle::CE_PopupMenuItem, p, this, TQRect(x, y, w, h), cg,
			    flags, TQStyleOption(&dummy,maxPMWidth,tab_));
	mi->custom()->paint( p, cg, act, flags&TQStyle::Style_Enabled, x, y, w, h );
    } else
	style().drawControl(TQStyle::CE_PopupMenuItem, p, this, TQRect(x, y, w, h), cg,
			    flags, TQStyleOption(mi,maxPMWidth,tab_));
}


/*!
    Draws all menu items using painter \a p.
*/
void TQPopupMenu::drawContents( TQPainter* p )
{
    TQMenuItemListIt it(*mitems);
    TQMenuItem *mi = 0;
    int row = 0;
    int x = contentsRect().x();
    int y = contentsRect().y();
    if(d->scroll.scrollable) {
	if(d->scroll.topScrollableIndex) {
	    for( ; (mi = it.current()) && row < d->scroll.topScrollableIndex; row++)
		++it;
	    if(!mi)
		it.toFirst();
	}
	if(d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollUp) {
	    TQRect rect(x, y, contentsRect().width(),
		       style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this));
	    if(!p->hasClipping() || p->clipRegion().contains(rect)) {
		TQStyle::SFlags flags = TQStyle::Style_Up;
		if (isEnabled())
		    flags |= TQStyle::Style_Enabled;
		style().drawControl(TQStyle::CE_PopupMenuScroller, p, this, rect,
				    colorGroup(), flags, TQStyleOption(maxPMWidth));
	    }
	    y += rect.height();
	}
    }

    int itemw = contentsRect().width() / ncols;
    TQSize sz;
    TQStyle::SFlags flags;
    while ( (mi=it.current()) ) {
	if(d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollDown &&
	   y >= contentsRect().height() - style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this))
	    break;
	++it;
	if ( !mi->isVisible() ) {
	    ++row;
	    continue;
	}
	int itemh = itemHeight( mi );
	sz = style().sizeFromContents(TQStyle::CT_PopupMenuItem, this,
				      TQSize(0, itemh),
				      TQStyleOption(mi,maxPMWidth,0)
				);
	sz = sz.expandedTo(TQSize(itemw, sz.height()));
	itemw = sz.width();
	itemh = sz.height();

	if ( ncols > 1 && y + itemh > contentsRect().bottom() ) {
	    if ( y < contentsRect().bottom() ) {
		TQRect rect(x, y, itemw, contentsRect().bottom() - y);
		if(!p->hasClipping() || p->clipRegion().contains(rect)) {
		    flags = TQStyle::Style_Default;
		    if (isEnabled() && mi->isEnabledAndVisible())
			flags |= TQStyle::Style_Enabled;
		    style().drawControl(TQStyle::CE_PopupMenuItem, p, this, rect,
					colorGroup(), flags, TQStyleOption((TQMenuItem*)0,maxPMWidth));
		}
	    }
	    y = contentsRect().y();
	    x +=itemw;
	}
	if (!mi->widget() && (!p->hasClipping() || p->clipRegion().contains(TQRect(x, y, itemw, itemh))))
	    drawItem( p, tab, mi, row == actItem, x, y, itemw, itemh );
	y += itemh;
	++row;
    }
    if ( y < contentsRect().bottom() ) {
	TQRect rect(x, y, itemw, contentsRect().bottom() - y);
	if(!p->hasClipping() || p->clipRegion().contains(rect)) {
	    flags = TQStyle::Style_Default;
	    if ( isEnabled() )
		flags |= TQStyle::Style_Enabled;
	    style().drawControl(TQStyle::CE_PopupMenuItem, p, this, rect,
				colorGroup(), flags, TQStyleOption((TQMenuItem*)0,maxPMWidth));
	}
    }
    if( d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollDown ) {
	int sh = style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this);
	TQRect rect(x, contentsRect().height() - sh, contentsRect().width(), sh);
	if(!p->hasClipping() || p->clipRegion().contains(rect)) {
	    TQStyle::SFlags flags = TQStyle::Style_Down;
	    if (isEnabled())
		flags |= TQStyle::Style_Enabled;
	    style().drawControl(TQStyle::CE_PopupMenuScroller, p, this, rect,
				colorGroup(), flags, TQStyleOption(maxPMWidth));
	}
    }
#if defined( DEBUG_SLOPPY_SUBMENU )
    if ( style().styleHint(TQStyle::SH_PopupMenu_SloppySubMenus, this )) {
	p->setClipRegion( d->mouseMoveBuffer );
	p->fillRect( d->mouseMoveBuffer.boundingRect(), colorGroup().brush( TQColorGroup::Highlight ) );
    }
#endif
}


/*****************************************************************************
  Event handlers
 *****************************************************************************/

/*!
    \reimp
*/

void TQPopupMenu::paintEvent( TQPaintEvent *e )
{
    TQFrame::paintEvent( e );
}

/*!
    \reimp
*/

void TQPopupMenu::closeEvent( TQCloseEvent * e) {
    e->accept();
    byeMenuBar();
}


/*!
    \reimp
*/

void TQPopupMenu::mousePressEvent( TQMouseEvent *e )
{
    int sh = style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this);
    if (rect().contains(e->pos()) &&
	((d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollUp && e->pos().y() <= sh) || //up
	 (d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollDown &&
	     e->pos().y() >= contentsRect().height() - sh))) //down
	return;

    mouseBtDn = true;				// mouse button down
    int item = itemAtPos( e->pos() );
    if ( item == -1 ) {
	if ( !rect().contains(e->pos()) && !tryMenuBar(e) ) {
	    byeMenuBar();
	}
	return;
    }
    TQMenuItem *mi = mitems->at(item);
    if ( item != actItem )			// new item activated
	setActiveItem( item );

    TQPopupMenu *popup = mi->popup();
    if ( popup ) {
	if ( popup->isVisible() ) {		// sub menu already open
	    int pactItem = popup->actItem;
	    popup->actItem = -1;
	    popup->hidePopups();
	    popup->updateRow( pactItem );
	} else {				// open sub menu
	    hidePopups();
	    popupSubMenuLater( 20, this );
	}
    } else {
	hidePopups();
    }
}

/*!
    \reimp
*/

void TQPopupMenu::mouseReleaseEvent( TQMouseEvent *e )
{
    // do not hide a standalone context menu on press-release, unless
    // the user moved the mouse significantly
    if ( !parentMenu && !mouseBtDn && actItem < 0 && motion < 6 )
	return;

    mouseBtDn = false;

    // if the user released the mouse outside the menu, pass control
    // to the menubar or our parent menu
    int sh = style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this);
    if ( !rect().contains( e->pos() ) && tryMenuBar(e) )
	return;
    else if((d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollUp && e->pos().y() <= sh) || //up
	    (d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollDown &&
	     e->pos().y() >= contentsRect().height() - sh)) //down
	return;

    if ( actItem < 0 ) { // we do not have an active item
	// if the release is inside without motion (happens with
	// oversized popup menus on small screens), ignore it
	if ( rect().contains( e->pos() ) && motion < 6 )
	    return;
	else
	    byeMenuBar();
    } else {	// selected menu item!
	TQMenuItem *mi = mitems->at(actItem);
	if ( mi ->widget() ) {
	    TQWidget* widgetAt = TQApplication::widgetAt( e->globalPos(), true );
	    if ( widgetAt && widgetAt != this ) {
		TQMouseEvent me( e->type(), widgetAt->mapFromGlobal( e->globalPos() ),
				e->globalPos(), e->button(), e->state() );
		TQApplication::sendEvent( widgetAt, &me );
	    }
	}
	TQPopupMenu *popup = mi->popup();
#ifndef TQT_NO_WHATSTHIS
	    bool b = TQWhatsThis::inWhatsThisMode();
#else
	    const bool b = false;
#endif
	if ( !mi->isEnabledAndVisible() ) {
#ifndef TQT_NO_WHATSTHIS
	    if ( b ) {
		actItem = -1;
		updateItem( mi->id() );
		byeMenuBar();
		actSig( mi->id(), b);
	    }
#endif
	} else 	if ( popup ) {
	    popup->setFirstItemActive();
	} else {				// normal menu item
	    byeMenuBar();			// deactivate menu bar
	    if ( mi->isEnabledAndVisible() ) {
		actItem = -1;
		updateItem( mi->id() );
		active_popup_menu = this;
		TQGuardedPtr<TQSignal> signal = mi->signal();
		actSig( mi->id(), b );
		if ( signal && !b )
		    signal->activate();
		active_popup_menu = 0;
	    }
	}
    }
}

/*!
    \reimp
*/

void TQPopupMenu::mouseMoveEvent( TQMouseEvent *e )
{
    if( e->globalPos() == d->ignoremousepos ) {
        return;
    }
    d->ignoremousepos = TQPoint();

    motion++;

    if ( parentMenu && parentMenu->isPopupMenu ) {
	TQPopupMenu* p = (TQPopupMenu*)parentMenu;
	int myIndex;

	p->findPopup( this, &myIndex );
	TQPoint pPos = p->mapFromParent( e->globalPos() );
	if ( p->actItem != myIndex && !p->rect().contains( pPos ) )
	    p->setActiveItem( myIndex );

	if ( style().styleHint(TQStyle::SH_PopupMenu_SloppySubMenus, this )) {
	    p->d->mouseMoveBuffer = TQRegion();
#ifdef DEBUG_SLOPPY_SUBMENU
	    p->repaint();
#endif
	}
    }

    if ( (e->state() & TQt::MouseButtonMask) == 0 &&
	 !hasMouseTracking() )
	return;

    if(d->scroll.scrollable && e->pos().x() >= rect().x() && e->pos().x() <= rect().width()) {
        int sh = style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this);
        if((d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollUp && e->pos().y() <= sh) || 
           (d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollDown && e->pos().y() >= height()-sh)) {
            if(!d->scroll.scrolltimer) {
                d->scroll.scrolltimer = new TQTimer(this, "popup scroll timer");
                TQObject::connect( d->scroll.scrolltimer, TQ_SIGNAL(timeout()),
                                  this, TQ_SLOT(subScrollTimer()) );
            }
            if(!d->scroll.scrolltimer->isActive())
                d->scroll.scrolltimer->start(40);
            return;
        }
    }

    int	 item = itemAtPos( e->pos() );
    if ( item == -1 ) {				// no valid item
        if( !d->hasmouse ) {
            tryMenuBar( e );
            return;
        }
        d->hasmouse = 0;
	int lastActItem = actItem;
	actItem = -1;
	if ( lastActItem >= 0 )
	    updateRow( lastActItem );
        if ( lastActItem > 0 ||
		    ( !rect().contains( e->pos() ) && !tryMenuBar( e ) ) ) {
	    popupSubMenuLater(style().styleHint(TQStyle::SH_PopupMenu_SubMenuPopupDelay,
						this), this);
	}
    } else {					// mouse on valid item
	// but did not register mouse press
        d->hasmouse = 1;
	if ( (e->state() & TQt::MouseButtonMask) && !mouseBtDn )
	    mouseBtDn = true; // so mouseReleaseEvent will pop down

	TQMenuItem *mi = mitems->at( item );

	if ( mi->widget() ) {
	    TQWidget* widgetAt = TQApplication::widgetAt( e->globalPos(), true );
	    if ( widgetAt && widgetAt != this ) {
		TQMouseEvent me( e->type(), widgetAt->mapFromGlobal( e->globalPos() ),
				e->globalPos(), e->button(), e->state() );
		TQApplication::sendEvent( widgetAt, &me );
	    }
	}

	if ( actItem == item )
	    return;

	if ( style().styleHint(TQStyle::SH_PopupMenu_SloppySubMenus, this) &&
	     d->mouseMoveBuffer.contains( e->pos() ) ) {
	    actItem = item;
	    popupSubMenuLater( style().styleHint(TQStyle::SH_PopupMenu_SubMenuPopupDelay, this) * 6,
			       this );
	    return;
	}

	if ( mi->popup() || ( popupActive >= 0 && popupActive != item ))
	    popupSubMenuLater( style().styleHint(TQStyle::SH_PopupMenu_SubMenuPopupDelay, this),
			       this );
	else if ( singleSingleShot )
	    singleSingleShot->stop();

	if ( item != actItem )
	    setActiveItem( item );
    }
}


/*!
    \reimp
*/

void TQPopupMenu::keyPressEvent( TQKeyEvent *e )
{
    /*
      I get nothing but complaints about this.  -Brad

      - if (mouseBtDn && actItem >= 0) {
      -	if (e->key() == Key_Shift ||
      -	    e->key() == Key_Control ||
      -	    e->key() == Key_Alt)
      -	    return;
      -
      -	TQMenuItem *mi = mitems->at(actItem);
      -	int modifier = (((e->state() & ShiftButton) ? SHIFT : 0) |
      -			((e->state() & ControlButton) ? CTRL : 0) |
      -			((e->state() & AltButton) ? ALT : 0));
      -
      - #ifndef TQT_NO_ACCEL
      -	if (mi)
      -	    setAccel(modifier + e->key(), mi->id());
      - #endif
      - return;
      - }
    */

    TQMenuItem  *mi = 0;
    TQPopupMenu *popup;
    int dy = 0;
    bool ok_key = true;

    int key = e->key();
    if ( TQApplication::reverseLayout() ) {
	// in reverse mode opening and closing keys for submenues are reversed
	if ( key == Key_Left )
	    key = Key_Right;
	else if ( key == Key_Right )
	    key = Key_Left;
    }

    switch ( key ) {
    case Key_Tab:
	// ignore tab, otherwise it will be passed to the menubar
	break;

    case Key_Up:
	dy = -1;
	break;

    case Key_Down:
	dy = 1;
	break;

    case Key_Alt:
	if ( style().styleHint(TQStyle::SH_MenuBar_AltKeyNavigation, this) )
	    byeMenuBar();
	break;

    case Key_Escape:
	if ( tornOff ) {
	    close();
	    return;
	}
	// just hide one
	{
	    TQMenuData* p = parentMenu;
	    hide();
#ifndef TQT_NO_MENUBAR
	    if ( p && p->isMenuBar )
		((TQMenuBar*) p)->goodbye( true );
#endif
	}
	break;

    case Key_Left:
	if ( ncols > 1 && actItem >= 0 ) {
	    TQRect r( itemGeometry( actItem ) );
	    int newActItem = itemAtPos( TQPoint( r.left() - 1, r.center().y() ) );
	    if ( newActItem >= 0 ) {
		setActiveItem( newActItem );
		break;
	    }
	}
	if ( parentMenu && parentMenu->isPopupMenu ) {
	    ((TQPopupMenu *)parentMenu)->hidePopups();
	    if ( singleSingleShot )
		singleSingleShot->stop();
	    break;
	}

	ok_key = false;
    	break;

    case Key_Right:
	if ( actItem >= 0 && ( mi=mitems->at(actItem) )->isEnabledAndVisible() && (popup=mi->popup()) ) {
	    hidePopups();
	    if ( singleSingleShot )
		singleSingleShot->stop();
	    // ### The next two lines were switched to fix the problem with the first item of the
	    // submenu not being highlighted...any reason why they should have been the other way??
	    subMenuTimer();
	    popup->setFirstItemActive();
	    break;
	} else if ( actItem == -1 && ( parentMenu && !parentMenu->isMenuBar )) {
	    dy = 1;
	    break;
	}
	if ( ncols > 1 && actItem >= 0 ) {
	    TQRect r( itemGeometry( actItem ) );
	    int newActItem = itemAtPos( TQPoint( r.right() + 1, r.center().y() ) );
	    if ( newActItem >= 0 ) {
		setActiveItem( newActItem );
		break;
	    }
	}
	ok_key = false;
	break;

    case Key_Space:
	if (! style().styleHint(TQStyle::SH_PopupMenu_SpaceActivatesItem, this))
	    break;
	// for motif, fall through

    case Key_Return:
    case Key_Enter:
	{
	    if ( actItem < 0 )
		break;
#ifndef TQT_NO_WHATSTHIS
	    bool b = TQWhatsThis::inWhatsThisMode();
#else
	    const bool b = false;
#endif
	    mi = mitems->at( actItem );
	    if ( !mi->isEnabled() && !b )
		break;
	    popup = mi->popup();
	    if ( popup ) {
		hidePopups();
		popupSubMenuLater( 20, this );
		popup->setFirstItemActive();
	    } else {
		actItem = -1;
		updateItem( mi->id() );
		byeMenuBar();
		if ( mi->isEnabledAndVisible() || b ) {
		    active_popup_menu = this;
		    TQGuardedPtr<TQSignal> signal = mi->signal();
		    actSig( mi->id(), b );
		    if ( signal && !b )
			signal->activate();
		    active_popup_menu = 0;
		}
	    }
	}
	break;
#ifndef TQT_NO_WHATSTHIS
    case Key_F1:
	if ( actItem < 0 || e->state() != ShiftButton)
	    break;
	mi = mitems->at( actItem );
	if ( !mi->whatsThis().isNull() ){
	    if ( !TQWhatsThis::inWhatsThisMode() )
		TQWhatsThis::enterWhatsThisMode();
	    TQRect r( itemGeometry( actItem) );
	    TQWhatsThis::leaveWhatsThisMode( mi->whatsThis(), mapToGlobal( r.bottomLeft()) );
	}
	//fall-through!
#endif
    default:
	ok_key = false;

    }
    if ( !ok_key &&
	 ( !e->state() || e->state() == AltButton || e->state() == ShiftButton ) &&
	 e->text().length()==1 ) {
	TQChar c = e->text()[0].upper();

	TQMenuItemListIt it(*mitems);
	TQMenuItem* first = 0;
	TQMenuItem* currentSelected = 0;
	TQMenuItem* firstAfterCurrent = 0;

	TQMenuItem *m;
	mi = 0;
	int indx = 0;
	int clashCount = 0;
	while ( (m=it.current()) ) {
	    ++it;
	    TQString s = m->text();
	    if ( !s.isEmpty() ) {
		int i = s.find( '&' );
		while ( i >= 0 && i < (int)s.length() - 1 ) {
		    if ( s[i+1].upper() == c ) {
			ok_key = true;
			clashCount++;
			if ( !first )
			    first = m;
			if ( indx == actItem )
			    currentSelected = m;
			else if ( !firstAfterCurrent && currentSelected )
			    firstAfterCurrent = m;
			break;
		    } else if ( s[i+1] == '&' ) {
			i = s.find( '&', i+2 );
		    } else {
			break;
		    }
		}
	    }
	    if ( mi )
		break;
	    indx++;
	}

	if ( 1 == clashCount ) { // No clashes, continue with selection
	    mi = first;
	    popup = mi->popup();
	    if ( popup ) {
		setActiveItem( indexOf(mi->id()) );
		hidePopups();
		popupSubMenuLater( 20, this );
		popup->setFirstItemActive();
	    } else {
		byeMenuBar();
#ifndef TQT_NO_WHATSTHIS
		bool b = TQWhatsThis::inWhatsThisMode();
#else
		const bool b = false;
#endif
		if ( mi->isEnabledAndVisible() || b ) {
		    active_popup_menu = this;
		    TQGuardedPtr<TQSignal> signal = mi->signal();
		    actSig( mi->id(), b );
		    if ( signal && !b  )
			signal->activate();
		    active_popup_menu = 0;
		}
	    }
	} else if ( clashCount > 1 ) { // Clashes, highlight next...
	    // If there's clashes and no one is selected, use first one
	    // or if there is no clashes _after_ current, use first one
	    if ( !currentSelected || (currentSelected && !firstAfterCurrent))
		dy = indexOf( first->id() ) - actItem;
	    else
		dy = indexOf( firstAfterCurrent->id() ) - actItem;
	}
    }
#ifndef TQT_NO_MENUBAR
    if ( !ok_key ) {				// send to menu bar
	TQMenuData *top = this;		// find top level
	while ( top->parentMenu )
	    top = top->parentMenu;
	if ( top->isMenuBar ) {
	    int beforeId = top->actItem;
	    ((TQMenuBar*)top)->tryKeyEvent( this, e );
	    if ( beforeId != top->actItem )
		ok_key = true;
	}
    }
#endif
    if ( actItem < 0 ) {
	if ( dy > 0 ) {
	    setFirstItemActive();
	} else if ( dy < 0 ) {
	    TQMenuItemListIt it(*mitems);
	    it.toLast();
	    TQMenuItem *mi;
	    int ai = count() - 1;
	    while ( (mi=it.current()) ) {
		--it;
		if ( !mi->isSeparator() && mi->id() != TQMenuData::d->aInt ) {
		    setActiveItem( ai );
		    return;
		}
		ai--;
	    }
	    actItem = -1;
	}
	return;
    }

    if ( dy ) {				// highlight next/prev
	int i = actItem;
	int c = mitems->count();
	for(int n = c; n; n--) {
	    i = i + dy;
	    if(d->scroll.scrollable) {
		if(d->scroll.scrolltimer)
		    d->scroll.scrolltimer->stop();
		if(i < 0)
		    i = 0;
		else if(i >= c)
		    i  = c - 1;
	    } else {
		if ( i == c )
		    i = 0;
		else if ( i < 0 )
		    i = c - 1;
	    }
	    mi = mitems->at( i );
	    if ( !mi || !mi->isVisible() )
		continue;

	    if ( !mi->isSeparator() &&
		 ( style().styleHint(TQStyle::SH_PopupMenu_AllowActiveAndDisabled, this)
		   || mi->isEnabledAndVisible() ) )
		break;
	}
	if ( i != actItem )
	    setActiveItem( i );
	if(d->scroll.scrollable) { //need to scroll to make it visible?
	    TQRect r = itemGeometry(actItem);
	    if(r.isNull() || r.height() < itemHeight(mitems->at(actItem))) {
		bool refresh = false;
		if(d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollUp && dy == -1) { //up
		    if(d->scroll.topScrollableIndex >= 0) {
			d->scroll.topScrollableIndex--;
			refresh = true;
		    }
		} else if(d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollDown) { //down
		    TQMenuItemListIt it(*mitems);
		    int sh = style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this);
		    for(int i = 0, y = ((d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollUp) ? sh : 0); it.current(); i++, ++it) {
			if(i >= d->scroll.topScrollableIndex) {
			    int itemh = itemHeight(it.current());
			    TQSize sz = style().sizeFromContents(TQStyle::CT_PopupMenuItem, this,
								TQSize(0, itemh),
								TQStyleOption(it.current(),maxPMWidth,0));
			    y += sz.height();
			    if(y > (contentsRect().height()-sh)) {
				if(sz.height() > sh || !it.atLast())
				    d->scroll.topScrollableIndex++;
				refresh = true;
				break;
			    }
			}
		    }
		}
		if(refresh) {
		    updateScrollerState();
		    update();
		}
	    }
	}
    }

#ifdef Q_OS_WIN32
    if ( !ok_key &&
	!( e->key() == Key_Control || e->key() == Key_Shift || e->key() == Key_Meta ) )
	tqApp->beep();
#endif // Q_OS_WIN32
}


/*!
    \reimp
*/

void TQPopupMenu::timerEvent( TQTimerEvent *e )
{
    TQFrame::timerEvent( e );
}

/*!
    \reimp
*/
void TQPopupMenu::leaveEvent( TQEvent * )
{
    d->hasmouse = 0;
    if ( testWFlags( WStyle_Tool ) && style().styleHint(TQStyle::SH_PopupMenu_MouseTracking, this) ) {
	int lastActItem = actItem;
	actItem = -1;
	if ( lastActItem >= 0 )
	    updateRow( lastActItem );
    }
}

/*!
    \reimp
*/
void TQPopupMenu::styleChange( TQStyle& old )
{
    TQFrame::styleChange( old );
    setMouseTracking(style().styleHint(TQStyle::SH_PopupMenu_MouseTracking, this));
    style().polishPopupMenu( this );
    updateSize(true);
}

/*!\reimp
 */
void TQPopupMenu::enabledChange( bool )
{
    if ( TQMenuData::d->aWidget ) // torn-off menu
	TQMenuData::d->aWidget->setEnabled( isEnabled() );
}


/*!
    If a popup menu does not fit on the screen it lays itself out so
    that it does fit. It is style dependent what layout means (for
    example, on Windows it will use multiple columns).

    This functions returns the number of columns necessary.

    \sa sizeHint()
*/
int TQPopupMenu::columns() const
{
    return ncols;
}

/* This private slot handles the scrolling popupmenu */
void TQPopupMenu::subScrollTimer() {
    TQPoint pos = TQCursor::pos();
    if(!d->scroll.scrollable || !isVisible()) {
	if(d->scroll.scrolltimer)
	    d->scroll.scrolltimer->stop();
	return;
    } else if(pos.x() > x() + width() || pos.x() < x()) {
	return;
    }
    int sh = style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this);
    if(!d->scroll.lastScroll.isValid()) {
	d->scroll.lastScroll = TQTime::currentTime();
    } else {
	int factor=0;
	if(pos.y() < y())
	    factor = y() - pos.y();
	else if(pos.y() > y() + height())
	    factor = pos.y() - (y() + height());
	int msecs = 250 - ((factor / 10) * 40);
	if(d->scroll.lastScroll.msecsTo(TQTime::currentTime()) < TQMAX(0, msecs))
	    return;
	d->scroll.lastScroll = TQTime::currentTime();
    }
    if(d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollUp && pos.y() <= y() + sh) { //up
	if(d->scroll.topScrollableIndex > 0) {
	    d->scroll.topScrollableIndex--;
	    updateScrollerState();
	    update(contentsRect());
	}
    } else if(d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollDown &&
	      pos.y() >= (y() + contentsRect().height()) - sh) { //down
	TQMenuItemListIt it(*mitems);
	for(int i = 0, y = contentsRect().y() + sh; it.current(); i++, ++it) {
	    if(i >= d->scroll.topScrollableIndex) {
		int itemh = itemHeight(it.current());
		TQSize sz = style().sizeFromContents(TQStyle::CT_PopupMenuItem, this, TQSize(0, itemh),
						    TQStyleOption(it.current(),maxPMWidth,0));
		y += sz.height();
		if(y > contentsRect().height() - sh) {
		    d->scroll.topScrollableIndex++;
		    updateScrollerState();
		    update(contentsRect());
		    break;
		}
	    }
	}
    }
}

/* This private slot handles the delayed submenu effects */

void TQPopupMenu::subMenuTimer() {

    if ( !isVisible() || (actItem < 0 && popupActive < 0) || actItem == popupActive )
	return;

    if ( popupActive >= 0 ) {
	hidePopups();
	popupActive = -1;
    }

    // hidePopups() may change actItem etc.
    if ( !isVisible() || actItem < 0 || actItem == popupActive )
	return;

    TQMenuItem *mi = mitems->at(actItem);
    if ( !mi || !mi->isEnabledAndVisible() )
	return;

    TQPopupMenu *popup = mi->popup();
    if ( !popup || !popup->isEnabled() )
	return;

    //avoid circularity
    if ( popup->isVisible() )
	return;

    Q_ASSERT( popup->parentMenu == 0 );
    popup->parentMenu = this;			// set parent menu

    emit popup->aboutToShow();
    supressAboutToShow = true;


    TQRect r( itemGeometry( actItem ) );
    TQPoint p;
    TQSize ps = popup->sizeHint();
    // GUI Style
    int gs = style().styleHint(TQStyle::SH_GUIStyle);
    int arrowHMargin, arrowVMargin;
    if (gs == GtkStyle) {
        arrowHMargin = gtkArrowHMargin;
        arrowVMargin = gtkArrowVMargin;
    } else {
        arrowHMargin = motifArrowHMargin;
        arrowVMargin = motifArrowVMargin;
    }
    if( TQApplication::reverseLayout() ) {
	p = TQPoint( r.left() + arrowHMargin - ps.width(), r.top() + arrowVMargin );
	p = mapToGlobal( p );

	bool right = false;
	if ( ( parentMenu && parentMenu->isPopupMenu &&
	       ((TQPopupMenu*)parentMenu)->geometry().x() < geometry().x() ) ||
	     p.x() < screenRect( p ).left())
	    right = true;
	if ( right && (ps.width() > screenRect( p ).right() - mapToGlobal( r.topRight() ).x() ) )
	    right = false;
	if ( right )
	    p.setX( mapToGlobal( r.topRight() ).x() );
    } else {
	p = TQPoint( r.right() - arrowHMargin, r.top() + arrowVMargin );
	p = mapToGlobal( p );

	bool left = false;
	if ( ( parentMenu && parentMenu->isPopupMenu &&
	       ((TQPopupMenu*)parentMenu)->geometry().x() > geometry().x() ) ||
	     p.x() + ps.width() > screenRect( p ).right() )
	    left = true;
	if ( left && (ps.width() > mapToGlobal( r.topLeft() ).x() ) )
	    left = false;
	if ( left )
	    p.setX( mapToGlobal( r.topLeft() ).x() - ps.width() );
    }
    TQRect pr = popup->itemGeometry(popup->count() - 1);
    if (p.y() + ps.height() > screenRect( p ).bottom() &&
	p.y() - ps.height() + (TQCOORD) pr.height() >= screenRect( p ).top())
	p.setY( p.y() - ps.height() + (TQCOORD) pr.height());

    if ( style().styleHint(TQStyle::SH_PopupMenu_SloppySubMenus, this )) {
	 TQPoint cur = TQCursor::pos();
	 if ( r.contains( mapFromGlobal( cur ) ) ) {
	     TQPoint pts[4];
	     pts[0] = TQPoint( cur.x(), cur.y() - 2 );
	     pts[3] = TQPoint( cur.x(), cur.y() + 2 );
	     if ( p.x() >= cur.x() )	{
		 pts[1] = TQPoint( geometry().right(), p.y() );
		 pts[2] = TQPoint( geometry().right(), p.y() + ps.height() );
	     } else {
		 pts[1] = TQPoint( p.x() + ps.width(), p.y() );
		 pts[2] = TQPoint( p.x() + ps.width(), p.y() + ps.height() );
	     }
	     TQPointArray points( 4 );
	     for( int i = 0; i < 4; i++ )
		 points.setPoint( i, mapFromGlobal( pts[i] ) );
	     d->mouseMoveBuffer = TQRegion( points );
	     repaint();
	 }
    }

    popupActive = actItem;
    popup->popup( p );
}

void TQPopupMenu::allowAnimation()
{
    preventAnimation = false;
}

void TQPopupMenu::updateRow( int row )
{
    if ( !isVisible() )
	return;

    if ( badSize ) {
	updateSize();
	update();
	return;
    }
    updateSize();
    TQRect r = itemGeometry( row );
    if ( !r.isNull() ) // can happen via the scroller
	repaint( r );
}


/*!
    \overload

    Executes this popup synchronously.

    Opens the popup menu so that the item number \a indexAtPoint will
    be at the specified \e global position \a pos. To translate a
    widget's local coordinates into global coordinates, use
    TQWidget::mapToGlobal().

    The return code is the id of the selected item in either the popup
    menu or one of its submenus, or -1 if no item is selected
    (normally because the user pressed Esc).

    Note that all signals are emitted as usual. If you connect a menu
    item to a slot and call the menu's exec(), you get the result both
    via the signal-slot connection and in the return value of exec().

    Common usage is to position the popup at the current mouse
    position:
    \code
	exec( TQCursor::pos() );
    \endcode
    or aligned to a widget:
    \code
	exec( somewidget.mapToGlobal(TQPoint(0, 0)) );
    \endcode

    When positioning a popup with exec() or popup(), bear in mind that
    you cannot rely on the popup menu's current size(). For
    performance reasons, the popup adapts its size only when
    necessary. So in many cases, the size before and after the show is
    different. Instead, use sizeHint(). It calculates the proper size
    depending on the menu's current contents.

    \sa popup(), sizeHint()
*/

int TQPopupMenu::exec( const TQPoint & pos, int indexAtPoint )
{
    snapToMouse = true;
    if ( !tqApp )
	return -1;

    TQPopupMenu* priorSyncMenu = syncMenu;

    syncMenu = this;
    syncMenuId = -1;

    TQGuardedPtr<TQPopupMenu> that = this;
    connectModal( that, true );
    popup( pos, indexAtPoint );
    tqApp->enter_loop();
    connectModal( that, false );

    syncMenu = priorSyncMenu;
    return syncMenuId;
}



/*
  Connect the popup and all its submenus to modalActivation() if
  \a doConnect is true, otherwise disconnect.
 */
void TQPopupMenu::connectModal( TQPopupMenu* receiver, bool doConnect )
{
    if ( !receiver )
	return;

    connectModalRecursionSafety = doConnect;

    if ( doConnect )
	connect( this, TQ_SIGNAL(activated(int)),
		 receiver, TQ_SLOT(modalActivation(int)) );
    else
	disconnect( this, TQ_SIGNAL(activated(int)),
		    receiver, TQ_SLOT(modalActivation(int)) );

    TQMenuItemListIt it(*mitems);
    TQMenuItem *mi;
    while ( (mi=it.current()) ) {
	++it;
	if ( mi->popup() && mi->popup() != receiver
	     && (bool)(mi->popup()->connectModalRecursionSafety) != doConnect )
	    mi->popup()->connectModal( receiver, doConnect ); //avoid circular
    }
}


/*!
    Executes this popup synchronously.

    This is equivalent to \c{exec(mapToGlobal(TQPoint(0,0)))}. In most
    situations you'll want to specify the position yourself, for
    example at the current mouse position:
    \code
	exec(TQCursor::pos());
    \endcode
    or aligned to a widget:
    \code
	exec(somewidget.mapToGlobal(TQPoint(0,0)));
    \endcode
*/

int TQPopupMenu::exec()
{
    return exec(mapToGlobal(TQPoint(0,0)));
}


/*  Internal slot used for exec(). */

void TQPopupMenu::modalActivation( int id )
{
    syncMenuId = id;
}


/*!
    Sets the currently active item to index \a i and repaints as necessary.
*/

void TQPopupMenu::setActiveItem( int i )
{
    int lastActItem = actItem;
    actItem = i;
    if ( lastActItem >= 0 )
	updateRow( lastActItem );
    if ( i >= 0 && i != lastActItem )
	updateRow( i );
    TQMenuItem *mi = mitems->at( actItem );
    if ( !mi )
	return;

    if ( mi->widget() && mi->widget()->isFocusEnabled() ) {
	mi->widget()->setFocus();
    } else {
	setFocus();
	TQRect mfrect = itemGeometry( actItem );
	setMicroFocusHint( mfrect.x(), mfrect.y(), mfrect.width(), mfrect.height(), false );
    }
    if ( mi->id() != -1 )
	hilitSig( mi->id() );
#ifndef TQT_NO_WHATSTHIS
    if (whatsThisItem && whatsThisItem != mi) {
	qWhatsThisBDH();
    }
    whatsThisItem = mi;
#endif
}


/*!
    \reimp
*/
TQSize TQPopupMenu::sizeHint() const
{
    constPolish();
    TQPopupMenu* that = (TQPopupMenu*) this;
    //We do not need a resize here, just the sizeHint..
    return that->updateSize(false).expandedTo( TQApplication::globalStrut() );
}


/*!
    \overload

    Returns the id of the item at \a pos, or -1 if there is no item
    there or if it is a separator.
*/
int TQPopupMenu::idAt( const TQPoint& pos ) const
{
    return idAt( itemAtPos( pos ) );
}


/*!
    \fn int TQPopupMenu::idAt( int index ) const

    Returns the identifier of the menu item at position \a index in
    the internal list, or -1 if \a index is out of range.

    \sa TQMenuData::setId(), TQMenuData::indexOf()
*/


/*!
    \reimp
 */
bool TQPopupMenu::customWhatsThis() const
{
    return true;
}


/*!
    \reimp
 */
bool TQPopupMenu::focusNextPrevChild( bool next )
{
    TQMenuItem *mi;
    int dy = next? 1 : -1;
    if ( dy && actItem < 0 ) {
	setFirstItemActive();
    } else if ( dy ) {				// highlight next/prev
	int i = actItem;
	int c = mitems->count();
	int n = c;
	while ( n-- ) {
	    i = i + dy;
	    if ( i == c )
		i = 0;
	    else if ( i < 0 )
		i = c - 1;
	    mi = mitems->at( i );
	    if ( mi && !mi->isSeparator() &&
		 ( ( style().styleHint(TQStyle::SH_PopupMenu_AllowActiveAndDisabled, this)
		     && mi->isVisible() )
		   || mi->isEnabledAndVisible() ) )
		break;
	}
	if ( i != actItem )
	    setActiveItem( i );
    }
    return true;
}


/*!
    \reimp
 */
void TQPopupMenu::focusInEvent( TQFocusEvent * )
{
}

/*!
    \reimp
 */
void TQPopupMenu::focusOutEvent( TQFocusEvent * )
{
}


class TQTearOffMenuItem : public TQCustomMenuItem
{
public:
    TQTearOffMenuItem()
    {
    }
    ~TQTearOffMenuItem()
    {
    }
    void paint( TQPainter* p, const TQColorGroup& cg, bool /* act*/,
		bool /*enabled*/, int x, int y, int w, int h )
    {
	p->setPen( TQPen( cg.dark(), 1, DashLine ) );
	p->drawLine( x+2, y+h/2-1, x+w-4, y+h/2-1 );
	p->setPen( TQPen( cg.light(), 1, DashLine ) );
	p->drawLine( x+2, y+h/2, x+w-4, y+h/2 );
    }
    bool fullSpan() const
    {
	return true;
    }

    TQSize sizeHint()
    {
	return TQSize( 20, 6 );
    }
};



/*!
    Inserts a tear-off handle into the menu. A tear-off handle is a
    special menu item that creates a copy of the menu when the menu is
    selected. This "torn-off" copy lives in a separate window. It
    contains the same menu items as the original menu, with the
    exception of the tear-off handle.

    The handle item is assigned the identifier \a id or an
    automatically generated identifier if \a id is < 0. The generated
    identifiers (negative integers) are guaranteed to be unique within
    the entire application.

    The \a index specifies the position in the menu. The tear-off
    handle is appended at the end of the list if \a index is negative.
*/
int TQPopupMenu::insertTearOffHandle( int id, int index )
{
    int myid = insertItem( new TQTearOffMenuItem, id, index );
    connectItem( myid, this, TQ_SLOT( toggleTearOff() ) );
    TQMenuData::d->aInt = myid;
    return myid;
}


/*!\internal

  implements tear-off menus
 */
void TQPopupMenu::toggleTearOff()
{
    if ( active_popup_menu && active_popup_menu->tornOff ) {
	active_popup_menu->close();
    } else  if (TQMenuData::d->aWidget ) {
	delete (TQWidget*) TQMenuData::d->aWidget; // delete the old one
    } else {
	// create a tear off menu
	TQPopupMenu* p = new TQPopupMenu( parentWidget(), "tear off menu" );
	connect( p, TQ_SIGNAL( activated(int) ), this, TQ_SIGNAL( activated(int) ) );
        connect( p, TQ_SIGNAL( highlighted(int) ), this, TQ_SIGNAL( highlighted(int) ) );
#ifndef TQT_NO_WIDGET_TOPEXTRA
	p->setCaption( caption() );
#endif
	p->setCheckable( isCheckable() );
	p->reparent( parentWidget(), WType_TopLevel | WStyle_Tool |
		     WNoAutoErase | WDestructiveClose,
		     geometry().topLeft(), false );
	p->mitems->setAutoDelete( false );
	p->tornOff = true;
#ifdef TQ_WS_X11
        p->x11SetWindowType( X11WindowTypeMenu );
#endif
	for ( TQMenuItemListIt it( *mitems ); it.current(); ++it ) {
	    if ( it.current()->id() != TQMenuData::d->aInt && !it.current()->widget() )
		p->mitems->append( it.current() );
	}
	p->show();
	TQMenuData::d->aWidget = p;
    }
}

/*!
    \reimp
 */
void TQPopupMenu::activateItemAt( int index )
{
    if ( index >= 0 && index < (int) mitems->count() ) {
	TQMenuItem *mi = mitems->at( index );
	if ( index != actItem )			// new item activated
	    setActiveItem( index );
	TQPopupMenu *popup = mi->popup();
	if ( popup ) {
	    if ( popup->isVisible() ) {		// sub menu already open
		int pactItem = popup->actItem;
		popup->actItem = -1;
		popup->hidePopups();
		popup->updateRow( pactItem );
	    } else {				// open sub menu
		hidePopups();
		actItem = index;
		subMenuTimer();
		popup->setFirstItemActive();
	    }
	} else {
	    byeMenuBar();			// deactivate menu bar

#ifndef TQT_NO_WHATSTHIS
	    bool b = TQWhatsThis::inWhatsThisMode();
#else
	    const bool b = false;
#endif
	    if ( !mi->isEnabledAndVisible() ) {
#ifndef TQT_NO_WHATSTHIS
		if ( b ) {
		    actItem = -1;
		    updateItem( mi->id() );
		    byeMenuBar();
		    actSig( mi->id(), b);
		}
#endif
	    } else {
		byeMenuBar();			// deactivate menu bar
		if ( mi->isEnabledAndVisible() ) {
		    actItem = -1;
		    updateItem( mi->id() );
		    active_popup_menu = this;
		    TQGuardedPtr<TQSignal> signal = mi->signal();
		    actSig( mi->id(), b );
		    if ( signal && !b )
			signal->activate();
		    active_popup_menu = 0;
		}
	    }
	}
    } else {
	if ( tornOff ) {
	    close();
	} else {
	    TQMenuData* p = parentMenu;
	    hide();
#ifndef TQT_NO_MENUBAR
	    if ( p && p->isMenuBar )
		((TQMenuBar*) p)->goodbye( true );
#endif
	}
    }

}

/*! \internal
  This private function is to update the scroll states in styles that support scrolling. */
void
TQPopupMenu::updateScrollerState()
{
    uint old_scrollable = d->scroll.scrollable;
    d->scroll.scrollable = TQPopupMenuPrivate::Scroll::ScrollNone;
    if(!style().styleHint(TQStyle::SH_PopupMenu_Scrollable, this))
	return;

    TQMenuItem *mi;
    TQMenuItemListIt it( *mitems );
    if(d->scroll.topScrollableIndex) {
	for(int row = 0; (mi = it.current()) && row < d->scroll.topScrollableIndex; row++)
	    ++it;
	if(!mi)
	    it.toFirst();
    }
    int y = 0, sh = style().pixelMetric(TQStyle::PM_PopupMenuScrollerHeight, this);
    if(!it.atFirst()) {
	// can't use |= because of a bug/feature in IBM xlC 5.0.2
	d->scroll.scrollable = d->scroll.scrollable | TQPopupMenuPrivate::Scroll::ScrollUp;
	y += sh;
    }
    while ( (mi=it.current()) ) {
	++it;
	int myheight = contentsRect().height();
	TQSize sz = style().sizeFromContents(TQStyle::CT_PopupMenuItem, this,
					    TQSize(0, itemHeight( mi )),
					    TQStyleOption(mi,maxPMWidth));
	if(y + sz.height() >= myheight) {
	    d->scroll.scrollable = d->scroll.scrollable | TQPopupMenuPrivate::Scroll::ScrollDown;
	    break;
	}
	y += sz.height();
    }
    if((d->scroll.scrollable & TQPopupMenuPrivate::Scroll::ScrollUp) &&
       !(old_scrollable & TQPopupMenuPrivate::Scroll::ScrollUp))
	d->scroll.topScrollableIndex++;
}

/*!
    Calculates the height in pixels of the menu item \a mi.
*/
int TQPopupMenu::menuItemHeight( TQMenuItem *mi, TQFontMetrics fm )
{
    if  ( mi->widget() )
	return mi->widget()->height();
    if ( mi->custom() && mi->custom()->fullSpan() )
	return mi->custom()->sizeHint().height();

    int h = 0;
    if ( mi->isSeparator() ) // separator height
        h = 2;
    else if ( mi->pixmap() ) // pixmap height
        h = mi->pixmap()->height();
    else                     // text height
        h = fm.height();

    if ( !mi->isSeparator() && mi->iconSet() != 0 )
        h = TQMAX(h, mi->iconSet()->pixmap( TQIconSet::Small,
					   TQIconSet::Normal ).height());
    if ( mi->custom() )
        h = TQMAX(h, mi->custom()->sizeHint().height());

    return h;
}

#endif // TQT_NO_POPUPMENU

