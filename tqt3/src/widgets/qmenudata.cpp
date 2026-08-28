/****************************************************************************
**
** Implementation of TQMenuData class
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

#include "ntqmenudata.h"
#ifndef TQT_NO_MENUDATA
#include "ntqpopupmenu.h"
#include "ntqmenubar.h"
#include "ntqapplication.h"
#include "ntqguardedptr.h"

class TQMenuItemData {
public:
    TQCustomMenuItem    *custom_item;	// custom menu item
};

class TQMenuDataData {
    // attention: also defined in qmenubar.cpp and qpopupmenu.cpp
public:
    TQMenuDataData();
    TQGuardedPtr<TQWidget> aWidget;
    int aInt;
};
TQMenuDataData::TQMenuDataData()
    : aInt(-1)
{}

/*!
    \class TQMenuData ntqmenudata.h
    \brief The TQMenuData class is a base class for TQMenuBar and TQPopupMenu.

    \ingroup misc

    TQMenuData has an internal list of menu items. A menu item can have
    a text(), an \link accel() accelerator\endlink, a pixmap(), an
    iconSet(), a whatsThis() text and a popup menu (unless it is a
    separator). Menu items may optionally be \link setItemChecked()
    checked\endlink (except for separators).

    The menu item sends out an \link TQMenuBar::activated()
    activated()\endlink signal when it is chosen and a \link
    TQMenuBar::highlighted() highlighted()\endlink signal when it
    receives the user input focus.

    \keyword menu identifier

    Menu items are assigned the menu identifier \e id that is passed
    in insertItem() or an automatically generated identifier if \e id
    is < 0 (the default). The generated identifiers (negative
    integers) are guaranteed to be unique within the entire
    application. The identifier is used to access the menu item in
    other functions.

    Menu items can be removed with removeItem() and removeItemAt(), or
    changed with changeItem(). All menu items can be removed with
    clear(). Accelerators can be changed or set with setAccel().
    Checkable items can be checked or unchecked with setItemChecked().
    Items can be enabled or disabled using setItemEnabled() and
    connected and disconnected with connectItem() and disconnectItem()
    respectively. By default, newly created menu items are visible.
    They can be hidden (and shown again) with setItemVisible().

    Menu items are stored in a list. Use findItem() to find an item by
    its list position or by its menu identifier. (See also indexOf()
    and idAt().)

    \sa TQAccel TQPopupMenu TQAction
*/


/*****************************************************************************
  TQMenuItem member functions
 *****************************************************************************/

TQMenuItem::TQMenuItem()
    :ident( -1 ), iconset_data( 0 ), pixmap_data( 0 ), popup_menu( 0 ),
     widget_item( 0 ), signal_data( 0 ), is_separator( false ), is_enabled( true ),
     is_checked( false ), is_dirty( true ), is_visible( true ), d( 0)
{}

TQMenuItem::~TQMenuItem()
{
    delete iconset_data;
    delete pixmap_data;
    delete signal_data;
    delete widget_item;
    if ( d )
	delete d->custom_item;
    delete d;
}


/*****************************************************************************
  TQMenuData member functions
 *****************************************************************************/

TQMenuItemData* TQMenuItem::extra()
{
    if ( !d ) d = new TQMenuItemData;
    return d;
}

TQCustomMenuItem *TQMenuItem::custom() const
{
    if ( !d ) return 0;
    return d->custom_item;
}


static int get_seq_id()
{
    static int seq_no = -2;
    return seq_no--;
}


/*!
    Constructs an empty menu data list.
*/

TQMenuData::TQMenuData()
{
    actItem = -1;				// no active menu item
    mitems = new TQMenuItemList;			// create list of menu items
    TQ_CHECK_PTR( mitems );
    mitems->setAutoDelete( true );
    parentMenu = 0;				// assume top level
    isPopupMenu = false;
    isMenuBar = false;
    mouseBtDn = false;
    badSize = true;
    avoid_circularity = 0;
    actItemDown = false;
    d = new TQMenuDataData;
}

/*!
    Removes all menu items and disconnects any signals that have been
    connected.
*/

TQMenuData::~TQMenuData()
{
    delete mitems;				// delete menu item list
    delete d;
}


/*!
    Virtual function; notifies subclasses about an item with \a id
    that has been changed.
*/

void TQMenuData::updateItem( int /* id */ )	// reimplemented in subclass
{
}

/*!
    Virtual function; notifies subclasses that one or more items have
    been inserted or removed.
*/

void TQMenuData::menuContentsChanged()		// reimplemented in subclass
{
}

/*!
    Virtual function; notifies subclasses that one or more items have
    changed state (enabled/disabled or checked/unchecked).
*/

void TQMenuData::menuStateChanged()		// reimplemented in subclass
{
}

/*!
    Virtual function; notifies subclasses that a popup menu item has
    been inserted.
*/

void TQMenuData::menuInsPopup( TQPopupMenu * )	// reimplemented in subclass
{
}

/*!
    Virtual function; notifies subclasses that a popup menu item has
    been removed.
*/

void TQMenuData::menuDelPopup( TQPopupMenu * )	// reimplemented in subclass
{
}


/*!
    Returns the number of items in the menu.
*/

uint TQMenuData::count() const
{
    return mitems->count();
}



/*!
  \internal

  Internal function that insert a menu item. Called by all insert()
  functions.
*/

int TQMenuData::insertAny( const TQString *text, const TQPixmap *pixmap,
			  TQPopupMenu *popup, const TQIconSet* iconset, int id, int index,
			  TQWidget* widget, TQCustomMenuItem* custom )
{
    if ( index < 0 ) {	// append, but not if the rightmost item is an mdi separator in the menubar
	index = mitems->count();
	if ( isMenuBar && mitems->last() && mitems->last()->widget() && mitems->last()->isSeparator() )
	    index--;
    } else if ( index > (int) mitems->count() ) { // append
	index = mitems->count();
    }
    if ( id < 0 )				// -2, -3 etc.
	id = get_seq_id();

    TQMenuItem *mi = new TQMenuItem;
    TQ_CHECK_PTR( mi );
    mi->ident = id;
    if ( widget != 0 ) {
	mi->widget_item = widget;
	mi->is_separator = !widget->isFocusEnabled();
    } else if ( custom != 0 ) {
	mi->extra()->custom_item = custom;
	mi->is_separator = custom->isSeparator();
	if ( iconset && !iconset->isNull() )
	    mi->iconset_data = new TQIconSet( *iconset );
    } else if ( text == 0 && pixmap == 0 && popup == 0 ) {
	mi->is_separator = true;		// separator
    } else {
#ifndef Q_OS_TEMP
	mi->text_data = text?*text:TQString();
#else
	TQString newText( *text );
	newText.truncate( newText.findRev( '\t' ) );
	mi->text_data = newText.isEmpty()?TQString():newText;
#endif
#ifndef TQT_NO_ACCEL
	mi->accel_key = TQt::Key_unknown;
#endif
	if ( pixmap && !pixmap->isNull() )
	    mi->pixmap_data = new TQPixmap( *pixmap );
	if ( (mi->popup_menu = popup) )
	    menuInsPopup( popup );
	if ( iconset && !iconset->isNull() )
	    mi->iconset_data = new TQIconSet( *iconset );
    }

    mitems->insert( index, mi );
    TQPopupMenu* p = ::tqt_cast<TQPopupMenu*>(TQMenuData::d->aWidget);
    if (p && p->isVisible() && p->mitems) {
	p->mitems->clear();
	for ( TQMenuItemListIt it( *mitems ); it.current(); ++it ) {
	    if ( it.current()->id() != TQMenuData::d->aInt && !it.current()->widget() )
		p->mitems->append( it.current() );
	}
    }
    menuContentsChanged();			// menu data changed
    return mi->ident;
}

/*!
    \internal

  Internal function that finds the menu item where \a popup is located,
  storing its index at \a index if \a index is not NULL.
*/
TQMenuItem *TQMenuData::findPopup( TQPopupMenu *popup, int *index )
{
    int i = 0;
    TQMenuItem *mi = mitems->first();
    while ( mi ) {
	if ( mi->popup_menu == popup )		// found popup
	    break;
	i++;
	mi = mitems->next();
    }
    if ( index && mi )
	*index = i;
    return mi;
}

void TQMenuData::removePopup( TQPopupMenu *popup )
{
    int index = 0;
    TQMenuItem *mi = findPopup( popup, &index );
    if ( mi ) {
	mi->popup_menu = 0;
	removeItemAt( index );
    }
}


/*!
    The family of insertItem() functions inserts menu items into a
    popup menu or a menu bar.

    A menu item is usually either a text string or a pixmap, both with
    an optional icon or keyboard accelerator. For special cases it is
    also possible to insert custom items (see \l{TQCustomMenuItem}) or
    even widgets into popup menus.

    Some insertItem() members take a popup menu as an additional
    argument. Use this to insert submenus into existing menus or
    pulldown menus into a menu bar.

    The number of insert functions may look confusing, but they are
    actually quite simple to use.

    This default version inserts a menu item with the text \a text,
    the accelerator key \a accel, an id and an optional index and
    connects it to the slot \a member in the object \a receiver.

    Example:
    \code
	TQMenuBar   *mainMenu = new TQMenuBar;
	TQPopupMenu *fileMenu = new TQPopupMenu;
	fileMenu->insertItem( "New",  myView, TQ_SLOT(newFile()), CTRL+Key_N );
	fileMenu->insertItem( "Open", myView, TQ_SLOT(open()),    CTRL+Key_O );
	mainMenu->insertItem( "File", fileMenu );
    \endcode

    Not all insert functions take an object/slot parameter or an
    accelerator key. Use connectItem() and setAccel() on those items.

    If you need to translate accelerators, use tr() with the text and
    accelerator. (For translations use a string \link TQKeySequence key
    sequence\endlink.):
    \code
	fileMenu->insertItem( tr("Open"), myView, TQ_SLOT(open()),
			      tr("Ctrl+O") );
    \endcode

    In the example above, pressing Ctrl+O or selecting "Open" from the
    menu activates the myView->open() function.

    Some insert functions take a TQIconSet parameter to specify the
    little menu item icon. Note that you can always pass a TQPixmap
    object instead.

    The \a id specifies the identification number associated with the
    menu item. Note that only positive values are valid, as a negative
    value will make TQt select a unique id for the item.

    The \a index specifies the position in the menu. The menu item is
    appended at the end of the list if \a index is negative.

    Note that keyboard accelerators in TQt are not application-global,
    instead they are bound to a certain top-level window. For example,
    accelerators in TQPopupMenu items only work for menus that are
    associated with a certain window. This is true for popup menus
    that live in a menu bar since their accelerators will then be
    installed in the menu bar itself. This also applies to stand-alone
    popup menus that have a top-level widget in their parentWidget()
    chain. The menu will then install its accelerator object on that
    top-level widget. For all other cases use an independent TQAccel
    object.

    \warning Be careful when passing a literal 0 to insertItem()
    because some C++ compilers choose the wrong overloaded function.
    Cast the 0 to what you mean, e.g. \c{(TQObject*)0}.

    \warning On Mac OS X, items that connect to a slot that are inserted into a
    menubar will not function as we use the native menubar that knows nothing
    about signals or slots. Instead insert the items into a popup menu and
    insert the popup menu into the menubar. This may be fixed in a future TQt
    version.

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem(), changeItem(), setAccel(), connectItem(), TQAccel,
    ntqnamespace.h
*/

int TQMenuData::insertItem( const TQString &text,
			   const TQObject *receiver, const char* member,
			   const TQKeySequence& accel, int id, int index )
{
    int actualID = insertAny( &text, 0, 0, 0, id, index );
    connectItem( actualID, receiver, member );
#ifndef TQT_NO_ACCEL
    if ( accel )
	setAccel( accel, actualID );
#endif
    return actualID;
}

/*!
    \overload

    Inserts a menu item with icon \a icon, text \a text, accelerator
    \a accel, optional id \a id, and optional \a index position. The
    menu item is connected it to the \a receiver's \a member slot. The
    icon will be displayed to the left of the text in the item.

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem(), changeItem(), setAccel(), connectItem(), TQAccel,
    ntqnamespace.h
*/

int TQMenuData::insertItem( const TQIconSet& icon,
			   const TQString &text,
			   const TQObject *receiver, const char* member,
			   const TQKeySequence& accel, int id, int index )
{
    int actualID = insertAny( &text, 0, 0, &icon, id, index );
    connectItem( actualID, receiver, member );
#ifndef TQT_NO_ACCEL
    if ( accel )
	setAccel( accel, actualID );
#endif
    return actualID;
}

/*!
    \overload

    Inserts a menu item with pixmap \a pixmap, accelerator \a accel,
    optional id \a id, and optional \a index position. The menu item
    is connected it to the \a receiver's \a member slot. The icon will
    be displayed to the left of the text in the item.

    To look best when being highlighted as a menu item, the pixmap
    should provide a mask (see TQPixmap::mask()).

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int TQMenuData::insertItem( const TQPixmap &pixmap,
			   const TQObject *receiver, const char* member,
			   const TQKeySequence& accel, int id, int index )
{
    int actualID = insertAny( 0, &pixmap, 0, 0, id, index );
    connectItem( actualID, receiver, member );
#ifndef TQT_NO_ACCEL
    if ( accel )
	setAccel( accel, actualID );
#endif
    return actualID;
}


/*!
    \overload

    Inserts a menu item with icon \a icon, pixmap \a pixmap,
    accelerator \a accel, optional id \a id, and optional \a index
    position. The icon will be displayed to the left of the pixmap in
    the item. The item is connected to the \a member slot in the \a
    receiver object.

    To look best when being highlighted as a menu item, the pixmap
    should provide a mask (see TQPixmap::mask()).

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem(), changeItem(), setAccel(), connectItem(), TQAccel,
    ntqnamespace.h
*/

int TQMenuData::insertItem( const TQIconSet& icon,
			   const TQPixmap &pixmap,
			   const TQObject *receiver, const char* member,
			   const TQKeySequence& accel, int id, int index )
{
    int actualID = insertAny( 0, &pixmap, 0, &icon, id, index );
    connectItem( actualID, receiver, member );
#ifndef TQT_NO_ACCEL
    if ( accel )
	setAccel( accel, actualID );
#endif
    return actualID;
}



/*!
    \overload

    Inserts a menu item with text \a text, optional id \a id, and
    optional \a index position.

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int TQMenuData::insertItem( const TQString &text, int id, int index )
{
    return insertAny( &text, 0, 0, 0, id, index );
}

/*!
    \overload

    Inserts a menu item with icon \a icon, text \a text, optional id
    \a id, and optional \a index position. The icon will be displayed
    to the left of the text in the item.

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int TQMenuData::insertItem( const TQIconSet& icon,
			   const TQString &text, int id, int index )
{
    return insertAny( &text, 0, 0, &icon, id, index );
}

/*!
    \overload

    Inserts a menu item with text \a text, submenu \a popup, optional
    id \a id, and optional \a index position.

    The \a popup must be deleted by the programmer or by its parent
    widget. It is not deleted when this menu item is removed or when
    the menu is deleted.

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int TQMenuData::insertItem( const TQString &text, TQPopupMenu *popup,
			   int id, int index )
{
    return insertAny( &text, 0, popup, 0, id, index );
}

/*!
    \overload

    Inserts a menu item with icon \a icon, text \a text, submenu \a
    popup, optional id \a id, and optional \a index position. The icon
    will be displayed to the left of the text in the item.

    The \a popup must be deleted by the programmer or by its parent
    widget. It is not deleted when this menu item is removed or when
    the menu is deleted.

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int TQMenuData::insertItem( const TQIconSet& icon,
			   const TQString &text, TQPopupMenu *popup,
			   int id, int index )
{
    return insertAny( &text, 0, popup, &icon, id, index );
}

/*!
    \overload

    Inserts a menu item with pixmap \a pixmap, optional id \a id, and
    optional \a index position.

    To look best when being highlighted as a menu item, the pixmap
    should provide a mask (see TQPixmap::mask()).

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int TQMenuData::insertItem( const TQPixmap &pixmap, int id, int index )
{
    return insertAny( 0, &pixmap, 0, 0, id, index );
}

/*!
    \overload

    Inserts a menu item with icon \a icon, pixmap \a pixmap, optional
    id \a id, and optional \a index position. The icon will be
    displayed to the left of the pixmap in the item.

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int TQMenuData::insertItem( const TQIconSet& icon,
			   const TQPixmap &pixmap, int id, int index )
{
    return insertAny( 0, &pixmap, 0, &icon, id, index );
}


/*!
    \overload

    Inserts a menu item with pixmap \a pixmap, submenu \a popup,
    optional id \a id, and optional \a index position.

    The \a popup must be deleted by the programmer or by its parent
    widget. It is not deleted when this menu item is removed or when
    the menu is deleted.

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int TQMenuData::insertItem( const TQPixmap &pixmap, TQPopupMenu *popup,
			   int id, int index )
{
    return insertAny( 0, &pixmap, popup, 0, id, index );
}


/*!
    \overload

    Inserts a menu item with icon \a icon, pixmap \a pixmap submenu \a
    popup, optional id \a id, and optional \a index position. The icon
    will be displayed to the left of the pixmap in the item.

    The \a popup must be deleted by the programmer or by its parent
    widget. It is not deleted when this menu item is removed or when
    the menu is deleted.

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem(), changeItem(), setAccel(), connectItem()
*/

int TQMenuData::insertItem( const TQIconSet& icon,
			   const TQPixmap &pixmap, TQPopupMenu *popup,
			   int id, int index )
{
    return insertAny( 0, &pixmap, popup, &icon, id, index );
}



/*!
    \overload

    Inserts a menu item that consists of the widget \a widget with
    optional id \a id, and optional \a index position.

    Ownership of \a widget is transferred to the popup menu or to the
    menu bar.

    Theoretically, any widget can be inserted into a popup menu. In
    practice, this only makes sense with certain widgets.

    If a widget is not focus-enabled (see
    \l{TQWidget::isFocusEnabled()}), the menu treats it as a separator;
    this means that the item is not selectable and will never get
    focus. In this way you can, for example, simply insert a TQLabel if
    you need a popup menu with a title.

    If the widget is focus-enabled it will get focus when the user
    traverses the popup menu with the arrow keys. If the widget does
    not accept \c ArrowUp and \c ArrowDown in its key event handler,
    the focus will move back to the menu when the respective arrow key
    is hit one more time. This works with a TQLineEdit, for example. If
    the widget accepts the arrow key itself, it must also provide the
    possibility to put the focus back on the menu again by calling
    TQWidget::focusNextPrevChild(). Futhermore, if the embedded widget
    closes the menu when the user made a selection, this can be done
    safely by calling:
    \code
	if ( isVisible() &&
	     parentWidget() &&
	     parentWidget()->inherits("TQPopupMenu") )
	    parentWidget()->close();
    \endcode

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa removeItem()
*/
int TQMenuData::insertItem( TQWidget* widget, int id, int index )
{
    return insertAny( 0, 0, 0, 0, id, index, widget );
}


/*!
    \overload

    Inserts a custom menu item \a custom with optional id \a id, and
    optional \a index position.

    This only works with popup menus. It is not supported for menu
    bars. Ownership of \a custom is transferred to the popup menu.

    If you want to connect a custom item to a slot, use connectItem().

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa connectItem(), removeItem(), TQCustomMenuItem
*/
int TQMenuData::insertItem( TQCustomMenuItem* custom, int id, int index )
{
    return insertAny( 0, 0, 0, 0, id, index, 0, custom );
}

/*!
    \overload

    Inserts a custom menu item \a custom with an \a icon and with
    optional id \a id, and optional \a index position.

    This only works with popup menus. It is not supported for menu
    bars. Ownership of \a custom is transferred to the popup menu.

    If you want to connect a custom item to a slot, use connectItem().

    Returns the allocated menu identifier number (\a id if \a id >= 0).

    \sa connectItem(), removeItem(), TQCustomMenuItem
*/
int TQMenuData::insertItem( const TQIconSet& icon, TQCustomMenuItem* custom, int id, int index )
{
    return insertAny( 0, 0, 0, &icon, id, index, 0, custom );
}


/*!
    Inserts a separator at position \a index, and returns the menu identifier
    number allocated to it. The separator becomes the last menu item if
    \a index is negative.

    In a popup menu a separator is rendered as a horizontal line. In a
    Motif menu bar a separator is spacing, so the rest of the items
    (normally just "Help") are drawn right-justified. In a Windows
    menu bar separators are ignored (to comply with the Windows style
    guidelines).
*/
int TQMenuData::insertSeparator( int index )
{
    return insertAny( 0, 0, 0, 0, -1, index );
}

/*!
    \fn void TQMenuData::removeItem( int id )

    Removes the menu item that has the identifier \a id.

    \sa removeItemAt(), clear()
*/

void TQMenuData::removeItem( int id )
{
    TQMenuData *parent;
    if ( findItem( id, &parent ) )
	parent->removeItemAt(parent->indexOf(id));
}

/*!
    Removes the menu item at position \a index.

    \sa removeItem(), clear()
*/

void TQMenuData::removeItemAt( int index )
{
    if ( index < 0 || index >= (int)mitems->count() ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQMenuData::removeItem: Index %d out of range", index );
#endif
	return;
    }
    TQMenuItem *mi = mitems->at( index );
    if ( mi->popup_menu )
	menuDelPopup( mi->popup_menu );
    mitems->remove();
    TQPopupMenu* p = ::tqt_cast<TQPopupMenu*>(TQMenuData::d->aWidget);
    if (p && p->isVisible() && p->mitems) {
	p->mitems->clear();
	for ( TQMenuItemListIt it( *mitems ); it.current(); ++it ) {
	    if ( it.current()->id() != TQMenuData::d->aInt && !it.current()->widget() )
		p->mitems->append( it.current() );
	}
    }
    if ( !TQApplication::closingDown() )		// avoid trouble
	menuContentsChanged();
}


/*!
    Removes all menu items.

    \sa removeItem(), removeItemAt()
*/

void TQMenuData::clear()
{
    TQMenuItem *mi = mitems->first();
    while ( mi ) {
	if ( mi->popup_menu )
	    menuDelPopup( mi->popup_menu );
	mitems->remove();
	mi = mitems->current();
    }
    TQPopupMenu* p = ::tqt_cast<TQPopupMenu*>(TQMenuData::d->aWidget);
    if (p && p->isVisible() && p->mitems) {
	p->mitems->clear();
    }
    if ( !TQApplication::closingDown() )		// avoid trouble
	menuContentsChanged();
}

#ifndef TQT_NO_ACCEL

/*!
    Returns the accelerator key that has been defined for the menu
    item \a id, or 0 if it has no accelerator key or if there is no
    such menu item.

    \sa setAccel(), TQAccel, ntqnamespace.h
*/

TQKeySequence TQMenuData::accel( int id ) const
{
    TQMenuItem *mi = findItem( id );
    if ( mi )
	return mi->key();
    return TQKeySequence();
}

/*!
    Sets the accelerator key for the menu item \a id to \a key.

    An accelerator key consists of a key code and a combination of the
    modifiers \c SHIFT, \c CTRL, \c ALT or \c UNICODE_ACCEL (OR'ed or
    added). The header file \c ntqnamespace.h contains a list of key
    codes.

    Defining an accelerator key produces a text that is added to the
    menu item; for instance, \c CTRL + \c Key_O produces "Ctrl+O". The
    text is formatted differently for different platforms.

    Note that keyboard accelerators in TQt are not application-global,
    instead they are bound to a certain top-level window. For example,
    accelerators in TQPopupMenu items only work for menus that are
    associated with a certain window. This is true for popup menus
    that live in a menu bar since their accelerators will then be
    installed in the menu bar itself. This also applies to stand-alone
    popup menus that have a top-level widget in their parentWidget()
    chain. The menu will then install its accelerator object on that
    top-level widget. For all other cases use an independent TQAccel
    object.

    Example:
    \code
	TQMenuBar *mainMenu = new TQMenuBar;
	TQPopupMenu *fileMenu = new TQPopupMenu;       // file sub menu
	fileMenu->insertItem( "Open Document", 67 ); // add "Open" item
	fileMenu->setAccel( CTRL + Key_O, 67 );      // Ctrl+O to open
	fileMenu->insertItem( "Quit", 69 );          // add "Quit" item
	fileMenu->setAccel( CTRL + ALT + Key_Delete, 69 ); // add Alt+Del to quit
	mainMenu->insertItem( "File", fileMenu );    // add the file menu
    \endcode

    If you need to translate accelerators, use tr() with a string:
    \code
	fileMenu->setAccel( tr("Ctrl+O"), 67 );
    \endcode

    You can also specify the accelerator in the insertItem() function.
    You may prefer to use TQAction to associate accelerators with menu
    items.

    \sa accel() insertItem() TQAccel TQAction
*/

void TQMenuData::setAccel( const TQKeySequence& key, int id )
{
    TQMenuData *parent;
    TQMenuItem *mi = findItem( id, &parent );
    if ( mi ) {
	mi->accel_key = key;
	parent->menuContentsChanged();
    }
}

#endif // TQT_NO_ACCEL

/*!
    Returns the icon set that has been set for menu item \a id, or 0
    if no icon set has been set.

    \sa changeItem(), text(), pixmap()
*/

TQIconSet* TQMenuData::iconSet( int id ) const
{
    TQMenuItem *mi = findItem( id );
    return mi ? mi->iconSet() : 0;
}

/*!
    Returns the text that has been set for menu item \a id, or
    TQString::null if no text has been set.

    \sa changeItem(), pixmap(), iconSet()
*/

TQString TQMenuData::text( int id ) const
{
    TQMenuItem *mi = findItem( id );
    return mi ? mi->text() : TQString::null;
}

/*!
    Returns the pixmap that has been set for menu item \a id, or 0 if
    no pixmap has been set.

    \sa changeItem(), text(), iconSet()
*/

TQPixmap *TQMenuData::pixmap( int id ) const
{
    TQMenuItem *mi = findItem( id );
    return mi ? mi->pixmap() : 0;
}

/*!
  \fn void TQMenuData::changeItem( const TQString &, int )
  \obsolete

  Changes the text of the menu item \a id. If the item has an icon,
  the icon remains unchanged.

  \sa text()
*/
/*!
  \fn void TQMenuData::changeItem( const TQPixmap &, int )
  \obsolete

  Changes the pixmap of the menu item \a id. If the item has an icon,
  the icon remains unchanged.

  \sa pixmap()
*/

/*!
  \fn void TQMenuData::changeItem( const TQIconSet &, const TQString &, int )
  \obsolete

  Changes the icon and text of the menu item \a id.

  \sa pixmap()
*/

/*!
    Changes the text of the menu item \a id to \a text. If the item
    has an icon, the icon remains unchanged.

    \sa text()
*/

void TQMenuData::changeItem( int id, const TQString &text )
{
    TQMenuData *parent;
    TQMenuItem *mi = findItem( id, &parent );
    if ( mi ) {					// item found
	if ( mi->text_data == text )		// same string
	    return;
	if ( mi->pixmap_data ) {		// delete pixmap
	    delete mi->pixmap_data;
	    mi->pixmap_data = 0;
	}
	mi->text_data = text;
#ifndef TQT_NO_ACCEL
	if ( !mi->accel_key && text.find( '\t' ) != -1 )
	    mi->accel_key = TQt::Key_unknown;
#endif
	parent->menuContentsChanged();
    }
}

/*!
    \overload

    Changes the pixmap of the menu item \a id to the pixmap \a pixmap.
    If the item has an icon, the icon is unchanged.

    \sa pixmap()
*/

void TQMenuData::changeItem( int id, const TQPixmap &pixmap )
{
    TQMenuData *parent;
    TQMenuItem *mi = findItem( id, &parent );
    if ( mi ) {					// item found
	TQPixmap *i = mi->pixmap_data;
	bool fast_refresh = i != 0 &&
	    i->width() == pixmap.width() &&
	    i->height() == pixmap.height() &&
	    !mi->text();
	if ( !mi->text_data.isNull() )		// delete text
	    mi->text_data = TQString::null;
	if ( !pixmap.isNull() )
	    mi->pixmap_data = new TQPixmap( pixmap );
	else
	    mi->pixmap_data = 0;
	delete i; // old mi->pixmap_data, could be &pixmap
	if ( fast_refresh )
	    parent->updateItem( id );
	else
	    parent->menuContentsChanged();
    }
}

/*!
    \overload

    Changes the iconset and text of the menu item \a id to the \a icon
    and \a text respectively.

    \sa pixmap()
*/

void TQMenuData::changeItem( int id, const TQIconSet &icon, const TQString &text )
{
    changeItem( id, text );
    changeItemIconSet( id, icon );
}

/*!
    \overload

    Changes the iconset and pixmap of the menu item \a id to \a icon
    and \a pixmap respectively.

    \sa pixmap()
*/

void TQMenuData::changeItem( int id, const TQIconSet &icon, const TQPixmap &pixmap )
{
    changeItem( id, pixmap );
    changeItemIconSet( id, icon );
}



/*!
    Changes the icon of the menu item \a id to \a icon.

    \sa pixmap()
*/

void TQMenuData::changeItemIconSet( int id, const TQIconSet &icon )
{
    TQMenuData *parent;
    TQMenuItem *mi = findItem( id, &parent );
    if ( mi ) {					// item found
	TQIconSet *i = mi->iconset_data;
	bool fast_refresh = i != 0;
	if ( !icon.isNull() )
	    mi->iconset_data = new TQIconSet( icon );
	else
	    mi->iconset_data = 0;
	delete i; // old mi->iconset_data, could be &icon
	if ( fast_refresh )
	    parent->updateItem( id );
	else
	    parent->menuContentsChanged();
    }
}


/*!
    Returns true if the item with identifier \a id is enabled;
    otherwise returns false

    \sa setItemEnabled(), isItemVisible()
*/

bool TQMenuData::isItemEnabled( int id ) const
{
    TQMenuItem *mi = findItem( id );
    return mi ? mi->isEnabled() : false;
}

/*!
    If \a enable is true, enables the menu item with identifier \a id;
    otherwise disables the menu item with identifier \a id.

    \sa isItemEnabled()
*/

void TQMenuData::setItemEnabled( int id, bool enable )
{
    TQMenuData *parent;
    TQMenuItem *mi = findItem( id, &parent );
    if ( mi && (bool)mi->is_enabled != enable ) {
	mi->is_enabled = enable;
#if !defined(TQT_NO_ACCEL) && !defined(TQT_NO_POPUPMENU)
	if ( mi->popup() )
	    mi->popup()->enableAccel( enable );
#endif
	parent->menuStateChanged();
    }
}


/*!
    Returns true if the menu item with the id \a id is currently
    active; otherwise returns false.
*/
bool TQMenuData::isItemActive( int id ) const
{
    if ( actItem == -1 )
	return false;
    return indexOf( id ) == actItem;
}

/*!
    Returns true if the menu item with the id \a id has been checked;
    otherwise returns false.

    \sa setItemChecked()
*/

bool TQMenuData::isItemChecked( int id ) const
{
    TQMenuItem *mi = findItem( id );
    return mi ? mi->isChecked() : false;
}

/*!
    If \a check is true, checks the menu item with id \a id; otherwise
    unchecks the menu item with id \a id. Calls
    TQPopupMenu::setCheckable( true ) if necessary.

    \sa isItemChecked()
*/

void TQMenuData::setItemChecked( int id, bool check )
{
    TQMenuData *parent;
    TQMenuItem *mi = findItem( id, &parent );
    if ( mi && (bool)mi->is_checked != check ) {
	mi->is_checked = check;
#ifndef TQT_NO_POPUPMENU
	if ( parent->isPopupMenu && !((TQPopupMenu *)parent)->isCheckable() )
	    ((TQPopupMenu *)parent)->setCheckable( true );
#endif
	parent->menuStateChanged();
    }
}

/*!
  Returns true if the menu item with the id \a id is  visible;
  otherwise returns false.

  \sa setItemVisible()
*/

bool TQMenuData::isItemVisible( int id ) const
{
    TQMenuItem *mi = findItem( id );
    return mi ? mi->isVisible() : false;
}

/*!
  If \a visible is true, shows the menu item with id \a id; otherwise
  hides the menu item with id \a id.

  \sa isItemVisible(), isItemEnabled()
*/

void TQMenuData::setItemVisible( int id, bool visible )
{
    TQMenuData *parent;
    TQMenuItem *mi = findItem( id, &parent );
    if ( mi && (bool)mi->is_visible != visible ) {
	mi->is_visible = visible;
	parent->menuContentsChanged();
    }
}


/*!
    Returns the menu item with identifier \a id, or 0 if there is no
    item with this identifier.

    Note that TQMenuItem is an internal class, and that you should not
    need to call this function. Use the higher level functions like
    text(), pixmap() and changeItem() to get and modify menu item
    attributes instead.

    \sa indexOf()
*/

TQMenuItem *TQMenuData::findItem( int id ) const
{
    return findItem( id, 0 );
}


/*!
    \overload

    Returns the menu item with identifier \a id, or 0 if there is no
    item with this identifier. Changes \a *parent to point to the
    parent of the return value.

    Note that TQMenuItem is an internal class, and that you should not
    need to call this function. Use the higher level functions like
    text(), pixmap() and changeItem() to get and modify menu item
    attributes instead.

    \sa indexOf()
*/

TQMenuItem * TQMenuData::findItem( int id, TQMenuData ** parent ) const
{
    if ( parent )
	*parent = (TQMenuData *)this;		// ###

    if ( id == -1 )				// bad identifier
	return 0;
    TQMenuItemListIt it( *mitems );
    TQMenuItem *mi;
    while ( (mi=it.current()) ) {		// search this menu
	++it;
	if ( mi->ident == id )			// found item
	    return mi;
    }
    it.toFirst();
    while ( (mi=it.current()) ) {		// search submenus
	++it;
#ifndef TQT_NO_POPUPMENU
	if ( mi->popup_menu ) {
	    TQPopupMenu *p = mi->popup_menu;
	    if (!p->avoid_circularity) {
		p->avoid_circularity = 1;
		mi = mi->popup_menu->findItem( id, parent );
		p->avoid_circularity = 0;
		if ( mi )				// found item
		    return mi;
	    }
	}
#endif
    }
    return 0;					// not found
}

/*!
    Returns the index of the menu item with identifier \a id, or -1 if
    there is no item with this identifier.

    \sa idAt(), findItem()
*/

int TQMenuData::indexOf( int id ) const
{
    if ( id == -1 )				// bad identifier
	return -1;
    TQMenuItemListIt it( *mitems );
    TQMenuItem *mi;
    int index = 0;
    while ( (mi=it.current()) ) {
	if ( mi->ident == id )			// this one?
	    return index;
	++index;
	++it;
    }
    return -1;					// not found
}

/*!
    Returns the identifier of the menu item at position \a index in
    the internal list, or -1 if \a index is out of range.

    \sa setId(), indexOf()
*/

int TQMenuData::idAt( int index ) const
{
    return index < (int)mitems->count() && index >= 0 ?
	   mitems->at(index)->id() : -1;
}

/*!
    Sets the menu identifier of the item at \a index to \a id.

    If \a index is out of range, the operation is ignored.

    \sa idAt()
*/

void TQMenuData::setId( int index, int id )
{
    if ( index < (int)mitems->count() )
	mitems->at(index)->ident = id;
}


/*!
    Sets the parameter of the activation signal of item \a id to \a
    param.

    If any receiver takes an integer parameter, this value is passed.

    \sa connectItem(), disconnectItem(), itemParameter()
*/
bool TQMenuData::setItemParameter( int id, int param ) {
    TQMenuItem *mi = findItem( id );
    if ( !mi )					// no such identifier
	return false;
    if ( !mi->signal_data ) {			// create new signal
	mi->signal_data = new TQSignal;
	TQ_CHECK_PTR( mi->signal_data );
    }
    mi->signal_data->setValue( param );
    return true;
}


/*!
    Returns the parameter of the activation signal of item \a id.

    If no parameter has been specified for this item with
    setItemParameter(), the value defaults to \a id.

    \sa connectItem(), disconnectItem(), setItemParameter()
*/
int TQMenuData::itemParameter( int id ) const
{
    TQMenuItem *mi = findItem( id );
    if ( !mi || !mi->signal_data )
	return id;
    return mi->signal_data->value().toInt();
}


/*!
    Connects the menu item with identifier \a id to \a{receiver}'s \a
    member slot or signal.

    The receiver's slot (or signal) is activated when the menu item is
    activated.

    \sa disconnectItem(), setItemParameter()
*/

bool TQMenuData::connectItem( int id, const TQObject *receiver,
			     const char* member )
{
    TQMenuItem *mi = findItem( id );
    if ( !mi )					// no such identifier
	return false;
    if ( !mi->signal_data ) {			// create new signal
	mi->signal_data = new TQSignal;
	TQ_CHECK_PTR( mi->signal_data );
	mi->signal_data->setValue( id );
    }
    return mi->signal_data->connect( receiver, member );
}


/*!
    Disconnects the \a{receiver}'s \a member from the menu item with
    identifier \a id.

    All connections are removed when the menu data object is
    destroyed.

    \sa connectItem(), setItemParameter()
*/

bool TQMenuData::disconnectItem( int id, const TQObject *receiver,
				const char* member )
{
    TQMenuItem *mi = findItem( id );
    if ( !mi || !mi->signal_data )		// no identifier or no signal
	return false;
    return mi->signal_data->disconnect( receiver, member );
}

/*!
    Sets \a text as What's This help for the menu item with identifier
    \a id.

    \sa whatsThis()
*/
void TQMenuData::setWhatsThis( int id, const TQString& text )
{

    TQMenuData *parent;
    TQMenuItem *mi = findItem( id, &parent );
    if ( mi ) {
	mi->setWhatsThis( text );
	parent->menuContentsChanged();
    }
}

/*!
    Returns the What's This help text for the item with identifier \a
    id or TQString::null if no text has yet been defined.

    \sa setWhatsThis()
*/
TQString TQMenuData::whatsThis( int id ) const
{

    TQMenuItem *mi = findItem( id );
    return mi? mi->whatsThis() : TQString::null;
}



/*!
    \class TQCustomMenuItem ntqmenudata.h
    \brief The TQCustomMenuItem class is an abstract base class for custom menu items in popup menus.

    \ingroup misc

    A custom menu item is a menu item that is defined by two pure
    virtual functions, paint() and sizeHint(). The size hint tells the
    menu how much space it needs to reserve for this item, and paint
    is called whenever the item needs painting.

    This simple mechanism allows you to create all kinds of
    application specific menu items. Examples are items showing
    different fonts in a word processor or menus that allow the
    selection of drawing utilities in a vector drawing program.

    A custom item is inserted into a popup menu with
    TQPopupMenu::insertItem().

    By default, a custom item can also have an icon and a keyboard
    accelerator. You can reimplement fullSpan() to return true if you
    want the item to span the entire popup menu width. This is
    particularly useful for labels.

    If you want the custom item to be treated just as a separator,
    reimplement isSeparator() to return true.

    Note that you can insert pixmaps or bitmaps as items into a popup
    menu without needing to create a TQCustomMenuItem. However, custom
    menu items offer more flexibility, and -- especially important
    with Windows style -- provide the possibility of drawing the item
    with a different color when it is highlighted.

    \link menu-example.html menu/menu.cpp\endlink shows a simple
    example how custom menu items can be used.

    Note: the current implementation of TQCustomMenuItem will not
    recognize shortcut keys that are from text with ampersands. Normal
    accelerators work though.

    <img src=qpopmenu-fancy.png>

    \sa TQMenuData, TQPopupMenu
*/



/*!
    Constructs a TQCustomMenuItem
*/
TQCustomMenuItem::TQCustomMenuItem()
{
}

/*!
    Destroys a TQCustomMenuItem
*/
TQCustomMenuItem::~TQCustomMenuItem()
{
}


/*!
    Sets the font of the custom menu item to \a font.

    This function is called whenever the font in the popup menu
    changes. For menu items that show their own individual font entry,
    you want to ignore this.
*/
void TQCustomMenuItem::setFont( const TQFont& /* font */ )
{
}



/*!
    Returns true if this item wants to span the entire popup menu
    width; otherwise returns false. The default is false, meaning that
    the menu may show an icon and an accelerator key for this item as
    well.
*/
bool TQCustomMenuItem::fullSpan() const
{
    return false;
}

/*!
    Returns true if this item is just a separator; otherwise returns
    false.
*/
bool TQCustomMenuItem::isSeparator() const
{
    return false;
}


/*!
    \fn void TQCustomMenuItem::paint( TQPainter* p, const TQColorGroup& cg, bool act,  bool enabled, int x, int y, int w, int h );

    Paints this item. When this function is invoked, the painter \a p
    is set to a font and foreground color suitable for a menu item
    text using color group \a cg. The item is active if \a act is true
    and enabled if \a enabled is true. The geometry values \a x, \a y,
    \a w and \a h specify where to draw the item.

    Do not draw any background, this has already been done by the
    popup menu according to the current GUI style.
*/


/*!
    \fn TQSize TQCustomMenuItem::sizeHint();

    Returns the item's size hint.
*/



/*!
    Activates the menu item at position \a index.

    If the index is invalid (for example, -1), the object itself is
    deactivated.
*/
void TQMenuData::activateItemAt( int index )
{
#ifndef TQT_NO_MENUBAR
    if ( isMenuBar )
	( (TQMenuBar*)this )->activateItemAt( index );
    else
#endif
    {
#ifndef TQT_NO_POPUPMENU
    if ( isPopupMenu )
	( (TQPopupMenu*)this )->activateItemAt( index );
#endif
    }
}

#endif
