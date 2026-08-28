/****************************************************************************
**
** Implementation of TQIconView widget class
**
** Created : 990707
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the iconview module of the TQt GUI Toolkit.
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

#include "ntqglobal.h"
#include "ntqiconview.h"

#ifndef TQT_NO_ICONVIEW

#include "ntqfontmetrics.h"
#include "ntqpainter.h"
#include "ntqevent.h"
#include "ntqpalette.h"
#include "ntqmime.h"
#include "ntqimage.h"
#include "ntqpen.h"
#include "ntqbrush.h"
#include "ntqtimer.h"
#include "ntqcursor.h"
#include "ntqapplication.h"
#include "ntqtextedit.h"
#include "ntqmemarray.h"
#include "ntqptrlist.h"
#include "ntqvbox.h"
#include "ntqtooltip.h"
#include "ntqbitmap.h"
#include "ntqpixmapcache.h"
#include "ntqptrdict.h"
#include "ntqstringlist.h"
#include "ntqcleanuphandler.h"
#include "private/qrichtext_p.h"
#include "ntqstyle.h"

#include <limits.h>
#include <stdlib.h>

#define RECT_EXTENSION 300

static const char * const unknown_xpm[] = {
    "32 32 11 1",
    "c c #ffffff",
    "g c #c0c0c0",
    "a c #c0ffc0",
    "h c #a0a0a4",
    "d c #585858",
    "f c #303030",
    "i c #400000",
    "b c #00c000",
    "e c #000000",
    "# c #000000",
    ". c None",
    "...###..........................",
    "...#aa##........................",
    ".###baaa##......................",
    ".#cde#baaa##....................",
    ".#cccdeebaaa##..##f.............",
    ".#cccccdeebaaa##aaa##...........",
    ".#cccccccdeebaaaaaaaa##.........",
    ".#cccccccccdeebaaaaaaa#.........",
    ".#cccccgcgghhebbbbaaaaa#........",
    ".#ccccccgcgggdebbbbbbaa#........",
    ".#cccgcgcgcgghdeebiebbba#.......",
    ".#ccccgcggggggghdeddeeba#.......",
    ".#cgcgcgcggggggggghghdebb#......",
    ".#ccgcggggggggghghghghd#b#......",
    ".#cgcgcggggggggghghghhd#b#......",
    ".#gcggggggggghghghhhhhd#b#......",
    ".#cgcggggggggghghghhhhd#b#......",
    ".#ggggggggghghghhhhhhhdib#......",
    ".#gggggggggghghghhhhhhd#b#......",
    ".#hhggggghghghhhhhhhhhd#b#......",
    ".#ddhhgggghghghhhhhhhhd#b#......",
    "..##ddhhghghhhhhhhhhhhdeb#......",
    "....##ddhhhghhhhhhhhhhd#b#......",
    "......##ddhhhhhhhhhhhhd#b#......",
    "........##ddhhhhhhhhhhd#b#......",
    "..........##ddhhhhhhhhd#b#......",
    "............##ddhhhhhhd#b###....",
    "..............##ddhhhhd#b#####..",
    "................##ddhhd#b######.",
    "..................##dddeb#####..",
    "....................##d#b###....",
    "......................####......"};

static TQPixmap *unknown_icon = 0;
static TQPixmap *qiv_buffer_pixmap = 0;
#if !defined(TQ_WS_X11)
static TQPixmap *qiv_selection = 0;
#endif
static bool optimize_layout = false;

static TQCleanupHandler<TQPixmap> qiv_cleanup_pixmap;

#if !defined(TQ_WS_X11)
static void createSelectionPixmap( const TQColorGroup &cg )
{
    TQBitmap m( 2, 2 );
    m.fill( TQt::color1 );
    TQPainter p( &m );
    p.setPen( TQt::color0 );
    for ( int j = 0; j < 2; ++j ) {
	p.drawPoint( j % 2, j );
    }
    p.end();

    qiv_selection = new TQPixmap( 2, 2 );
    qiv_cleanup_pixmap.add( &qiv_selection );
    qiv_selection->fill( TQt::color0 );
    qiv_selection->setMask( m );
    qiv_selection->fill( cg.highlight() );
}
#endif

static TQPixmap *get_qiv_buffer_pixmap( const TQSize &s )
{
    if ( !qiv_buffer_pixmap ) {
	qiv_buffer_pixmap = new TQPixmap( s );
	qiv_cleanup_pixmap.add( &qiv_buffer_pixmap );
	return qiv_buffer_pixmap;
    }

    qiv_buffer_pixmap->resize( s );
    return qiv_buffer_pixmap;
}

#ifndef TQT_NO_DRAGANDDROP

class TQ_EXPORT TQIconDragData
{
public:
    TQIconDragData();
    TQIconDragData( const TQRect &ir, const TQRect &tr );

    TQRect pixmapRect() const;
    TQRect textRect() const;

    void setPixmapRect( const TQRect &r );
    void setTextRect( const TQRect &r );

    TQRect iconRect_, textRect_;
    TQString key_;

    bool operator==( const TQIconDragData &i ) const;
};

class TQ_EXPORT TQIconDragDataItem
{
public:
    TQIconDragDataItem() {}
    TQIconDragDataItem( const TQIconDragItem &i1, const TQIconDragData &i2 ) : data( i1 ), item( i2 ) {}
    TQIconDragItem data;
    TQIconDragData item;
    bool operator== ( const TQIconDragDataItem& ) const;
};

class TQIconDragPrivate
{
public:
    TQValueList<TQIconDragDataItem> items;
    static bool decode( TQMimeSource* e, TQValueList<TQIconDragDataItem> &lst );
};

#endif

class TQIconViewToolTip;

class TQIconViewPrivate
{
public:
    TQIconViewItem *firstItem, *lastItem;
    uint count;
    TQIconView::SelectionMode selectionMode;
    TQIconViewItem *currentItem, *tmpCurrentItem, *highlightedItem,
	*startDragItem, *pressedItem, *selectAnchor, *renamingItem;
    TQRect *rubber;
    TQPixmap *backBuffer;
    TQTimer *scrollTimer, *adjustTimer, *updateTimer, *inputTimer,
	*fullRedrawTimer;
    int rastX, rastY, spacing;
    int dragItems;
    TQPoint oldDragPos;
    TQIconView::Arrangement arrangement;
    TQIconView::ResizeMode resizeMode;
    TQSize oldSize;
#ifndef TQT_NO_DRAGANDDROP
    TQValueList<TQIconDragDataItem> iconDragData;
#endif
    int numDragItems, cachedW, cachedH;
    int maxItemWidth, maxItemTextLength;
    TQPoint dragStart;
    TQString currInputString;
    TQIconView::ItemTextPos itemTextPos;
#ifndef TQT_NO_CURSOR
    TQCursor oldCursor;
#endif
    int cachedContentsX, cachedContentsY;
    TQBrush itemTextBrush;
    TQRegion clipRegion;
    TQPoint dragStartPos;
    TQFontMetrics *fm;
    int minLeftBearing, minRightBearing;
    int rubberStartX, rubberStartY;

    uint mousePressed		:1;
    uint cleared		:1;
    uint dropped		:1;
    uint clearing		:1;
    uint oldDragAcceptAction	:1;
    uint isIconDrag		:1;
    uint drawDragShapes		:1;
    uint dirty			:1;
    uint rearrangeEnabled	:1;
    uint reorderItemsWhenInsert	:1;
    uint drawAllBack		:1;
    uint resortItemsWhenInsert	:1;
    uint sortDirection		:1;
    uint wordWrapIconText	:1;
    uint containerUpdateLocked	:1;
    uint firstSizeHint : 1;
    uint showTips		:1;
    uint pressedSelected	:1;
    uint canStartRubber		:1;
    uint dragging		:1;
    uint drawActiveSelection	:1;
    uint inMenuMode		:1;

    TQIconViewToolTip *toolTip;
    TQPixmapCache maskCache;
    TQPixmap *backrubber;    
    TQPtrDict<TQIconViewItem> selectedItems;

    struct ItemContainer {
	ItemContainer( ItemContainer *pr, ItemContainer *nx, const TQRect &r )
	    : p( pr ), n( nx ), rect( r ) {
		items.setAutoDelete( false );
		if ( p )
		    p->n = this;
		if ( n )
		    n->p = this;
	}
	ItemContainer *p, *n;
	TQRect rect;
	TQPtrList<TQIconViewItem> items;
    } *firstContainer, *lastContainer;

    struct SortableItem {
	TQIconViewItem *item;
    };
};

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

#ifdef Q_OS_TEMP
static int _cdecl cmpIconViewItems( const void *n1, const void *n2 )
#else
static int cmpIconViewItems( const void *n1, const void *n2 )
#endif
{
    if ( !n1 || !n2 )
	return 0;

    TQIconViewPrivate::SortableItem *i1 = (TQIconViewPrivate::SortableItem *)n1;
    TQIconViewPrivate::SortableItem *i2 = (TQIconViewPrivate::SortableItem *)n2;

    return i1->item->compare( i2->item );
}

#if defined(Q_C_CALLBACKS)
}
#endif


#ifndef TQT_NO_TOOLTIP
class TQIconViewToolTip : public TQToolTip
{
public:
    TQIconViewToolTip( TQWidget *parent, TQIconView *iv );
    virtual ~TQIconViewToolTip();

    void maybeTip( const TQPoint &pos );

private:
    TQIconView *view;
};

TQIconViewToolTip::TQIconViewToolTip( TQWidget *parent, TQIconView *iv )
    : TQToolTip( parent ), view( iv )
{
}

TQIconViewToolTip::~TQIconViewToolTip()
{
}

void TQIconViewToolTip::maybeTip( const TQPoint &pos )
{
    if ( !parentWidget() || !view || view->wordWrapIconText() || !view->showToolTips() )
	return;

    TQIconViewItem *item = view->findItem( view->viewportToContents( pos ) );
    if ( !item || item->tmpText == item->itemText )
	return;

    TQRect r( item->textRect( false ) );
    TQRect r2 = item->pixmapRect( false );
    /* this probably should be | r, but TQToolTip does not handle that
     * well */

    // At this point the rectangle is too small (it is the width of the icon)
    // since we need it to be bigger than that, extend it here.
    r.setWidth( view->d->fm->boundingRect( item->itemText ).width() + 4 );
    r = TQRect( view->contentsToViewport( TQPoint( r.x(), r.y() ) ), TQSize( r.width(), r.height() ) );

    r2 = TQRect( view->contentsToViewport( TQPoint( r2.x(), r2.y() ) ), TQSize( r2.width(), r2.height() ) );
    tip( r2, item->itemText, r );
}
#endif


class TQIconViewItemPrivate
{
public:
    TQIconViewPrivate::ItemContainer *container1, *container2;
};

#ifndef TQT_NO_TEXTEDIT

class TQIconViewItemLineEdit : public TQTextEdit
{
    friend class TQIconViewItem;

public:
    TQIconViewItemLineEdit( const TQString &text, TQWidget *parent, TQIconViewItem *theItem, const char* name=0 );

protected:
    void keyPressEvent( TQKeyEvent *e );
    void focusOutEvent( TQFocusEvent *e );

protected:
    TQIconViewItem *item;
    TQString startText;
private:
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQIconViewItemLineEdit( const TQIconViewItemLineEdit & );
    TQIconViewItemLineEdit &operator=( const TQIconViewItemLineEdit & );
#endif

};

TQIconViewItemLineEdit::TQIconViewItemLineEdit( const TQString &text, TQWidget *parent,
					      TQIconViewItem *theItem, const char *name )
    : TQTextEdit( parent, name ), item( theItem ), startText( text )
{
    setFrameStyle( TQFrame::Plain | TQFrame::Box );
    setLineWidth( 1 );

    setHScrollBarMode( AlwaysOff );
    setVScrollBarMode( AlwaysOff );

    setWordWrap( WidgetWidth );
    setWrapColumnOrWidth( item->iconView()->maxItemWidth() -
			  ( item->iconView()->itemTextPos() == TQIconView::Bottom ?
			    0 : item->pixmapRect().width() ) );
    document()->formatter()->setAllowBreakInWords( true );
    resize( 200, 200 ); // ### some size, there should be a forceReformat()
    setTextFormat( PlainText );
    setText( text );
    setAlignment( TQt::AlignCenter );

    resize( wrapColumnOrWidth() + 2, heightForWidth( wrapColumnOrWidth() ) + 2 );
}

void TQIconViewItemLineEdit::keyPressEvent( TQKeyEvent *e )
{
    if ( e->key()  == Key_Escape ) {
	item->TQIconViewItem::setText( startText );
	item->cancelRenameItem();
    } else if ( e->key() == Key_Enter ||
		e->key() == Key_Return ) {
	item->renameItem();
    } else {
	TQTextEdit::keyPressEvent( e );
	sync();
	resize( width(), document()->height() + 2 );

    }
}

void TQIconViewItemLineEdit::focusOutEvent( TQFocusEvent *e )
{
    if ( e->reason() != TQFocusEvent::Popup )
	item->cancelRenameItem();
}
#endif

#ifndef TQT_NO_DRAGANDDROP


/*!
    \class TQIconDragItem ntqiconview.h
    \ingroup draganddrop

    \brief The TQIconDragItem class encapsulates a drag item.

    \module iconview

    The TQIconDrag class uses a list of TQIconDragItems to support drag
    and drop operations.

    In practice a TQIconDragItem object (or an object of a class derived
    from TQIconDragItem) is created for each icon view item which is
    dragged. Each of these TQIconDragItems is stored in a TQIconDrag
    object.

    See TQIconView::dragObject() for more information.

    See the \l fileiconview/qfileiconview.cpp and
    \l iconview/simple_dd/main.cpp examples.
*/

/*!
    Constructs a TQIconDragItem with no data.
*/

TQIconDragItem::TQIconDragItem()
    : ba( (int)strlen( "no data" ) )
{
    memcpy( ba.data(), "no data", strlen( "no data" ) );
}

/*!
    Destructor.
*/

TQIconDragItem::~TQIconDragItem()
{
}

/*!
    Returns the data contained in the TQIconDragItem.
*/

TQByteArray TQIconDragItem::data() const
{
    return ba;
}

/*!
    Sets the data for the TQIconDragItem to the data stored in the
    TQByteArray \a d.
*/

void TQIconDragItem::setData( const TQByteArray &d )
{
    ba = d;
}

/*!
    \reimp
*/

bool TQIconDragItem::operator==( const TQIconDragItem &i ) const
{
    return ba == i.ba;
}

/*!
    \reimp
*/

bool TQIconDragDataItem::operator==( const TQIconDragDataItem &i ) const
{
    return ( i.item == item &&
	     i.data == data );
}

/*!
    \reimp
*/

bool TQIconDragData::operator==( const TQIconDragData &i ) const
{
    return key_ == i.key_;
}


/*!
    \class TQIconDrag ntqiconview.h

    \brief The TQIconDrag class supports drag and drop operations
    within a TQIconView.

    \ingroup draganddrop
    \module iconview

    A TQIconDrag object is used to maintain information about the
    positions of dragged items and the data associated with the
    dragged items. TQIconViews are able to use this information to
    paint the dragged items in the correct positions. Internally
    TQIconDrag stores the data associated with drag items in
    TQIconDragItem objects.

    If you want to use the extended drag-and-drop functionality of
    TQIconView, create a TQIconDrag object in a reimplementation of
    TQIconView::dragObject(). Then create a TQIconDragItem for each item
    which should be dragged, set the data it represents with
    TQIconDragItem::setData(), and add each TQIconDragItem to the drag
    object using append().

    The data in TQIconDragItems is stored in a TQByteArray and is
    mime-typed (see TQMimeSource and the
    \link http://doc.trolltech.com/dnd.html Drag and Drop\endlink
    overview). If you want to use your own mime-types derive a class
    from TQIconDrag and reimplement format(), encodedData() and
    canDecode().

    The fileiconview example program demonstrates the use of the
    TQIconDrag class including subclassing and reimplementing
    dragObject(), format(), encodedData() and canDecode(). See the files
    \c qt/examples/fileiconview/qfileiconview.h and
    \c qt/examples/fileiconview/qfileiconview.cpp.

    \sa TQMimeSource::format()
*/
// ### consider using \dontinclude and friends there
// ### Not here in the module overview instead...

/*!
    Constructs a drag object called \a name, which is a child of \a
    dragSource.

    Note that the drag object will be deleted when \a dragSource is deleted.
*/

TQIconDrag::TQIconDrag( TQWidget * dragSource, const char* name )
    : TQDragObject( dragSource, name )
{
    d = new TQIconDragPrivate;
}

/*!
    Destructor.
*/

TQIconDrag::~TQIconDrag()
{
    delete d;
}

/*!
    Append the TQIconDragItem, \a i, to the TQIconDrag object's list of
    items. You must also supply the geometry of the pixmap, \a pr, and
    the textual caption, \a tr.

    \sa TQIconDragItem
*/

void TQIconDrag::append( const TQIconDragItem &i, const TQRect &pr, const TQRect &tr )
{
    d->items.append( TQIconDragDataItem( i, TQIconDragData( pr, tr ) ) );
}

/*!
    \reimp
*/

const char* TQIconDrag::format( int i ) const
{
    if ( i == 0 )
	return "application/x-qiconlist";
    return 0;
}

/*!
    Returns the encoded data of the drag object if \a mime is
    application/x-qiconlist.
*/

TQByteArray TQIconDrag::encodedData( const char* mime ) const
{
    if ( d->items.count() <= 0 || TQString( mime ) !=
	 "application/x-qiconlist" )
	return TQByteArray();

    TQValueList<TQIconDragDataItem>::ConstIterator it = d->items.begin();
    TQString s;
    for ( ; it != d->items.end(); ++it ) {
	TQString k( "%1$@@$%2$@@$%3$@@$%4$@@$%5$@@$%6$@@$%7$@@$%8$@@$" );
	k = k.arg( (*it).item.pixmapRect().x() ).arg(
	    (*it).item.pixmapRect().y() ).arg( (*it).item.pixmapRect().width() ).
	    arg( (*it).item.pixmapRect().height() ).arg(
		(*it).item.textRect().x() ).arg( (*it).item.textRect().y() ).
	    arg( (*it).item.textRect().width() ).arg(
		(*it).item.textRect().height() );
	k += TQString( (*it).data.data() ) + "$@@$";
	s += k;
    }

    TQByteArray a( s.length() + 1 );
    memcpy( a.data(), s.latin1(), a.size() );
    return a;
}

/*!
    Returns true if \a e can be decoded by the TQIconDrag, otherwise
    return false.
*/

bool TQIconDrag::canDecode( TQMimeSource* e )
{
    return e->provides( "application/x-qiconlist" );
}

/*!
    Decodes the data which is stored (encoded) in \a e and, if
    successful, fills the \a list of icon drag items with the decoded
    data. Returns true if there was some data, false otherwise.
*/

bool TQIconDragPrivate::decode( TQMimeSource* e, TQValueList<TQIconDragDataItem> &lst )
{
    TQByteArray ba = e->encodedData( "application/x-qiconlist" );
    if ( ba.size() ) {
	lst.clear();
	TQString s = ba.data();
	TQIconDragDataItem item;
	TQRect ir, tr;
	TQStringList l = TQStringList::split( "$@@$", s );

	int i = 0;
	TQStringList::Iterator it = l.begin();
	for ( ; it != l.end(); ++it ) {
	    if ( i == 0 ) {
		ir.setX( ( *it ).toInt() );
	    } else if ( i == 1 ) {
		ir.setY( ( *it ).toInt() );
	    } else if ( i == 2 ) {
		ir.setWidth( ( *it ).toInt() );
	    } else if ( i == 3 ) {
		ir.setHeight( ( *it ).toInt() );
	    } else if ( i == 4 ) {
		tr.setX( ( *it ).toInt() );
	    } else if ( i == 5 ) {
		tr.setY( ( *it ).toInt() );
	    } else if ( i == 6 ) {
		tr.setWidth( ( *it ).toInt() );
	    } else if ( i == 7 ) {
		tr.setHeight( ( *it ).toInt() );
	    } else if ( i == 8 ) {
		TQByteArray d( ( *it ).length() );
		memcpy( d.data(), ( *it ).latin1(), ( *it ).length() );
		item.item.setPixmapRect( ir );
		item.item.setTextRect( tr );
		item.data.setData( d );
		lst.append( item );
	    }
	    ++i;
	    if ( i > 8 )
		i = 0;
	}
	return true;
    }

    return false;
}

TQIconDragData::TQIconDragData()
    : iconRect_(), textRect_()
{
}

TQIconDragData::TQIconDragData( const TQRect &ir, const TQRect &tr )
    : iconRect_( ir ), textRect_( tr )
{
}

TQRect TQIconDragData::textRect() const
{
    return textRect_;
}

TQRect TQIconDragData::pixmapRect() const
{
    return iconRect_;
}

void TQIconDragData::setPixmapRect( const TQRect &r )
{
    iconRect_ = r;
}

void TQIconDragData::setTextRect( const TQRect &r )
{
    textRect_ = r;
}

#endif


/*!
    \class TQIconViewItem ntqiconview.h
    \brief The TQIconViewItem class provides a single item in a TQIconView.

    \ingroup advanced
    \module iconview

    A TQIconViewItem contains an icon, a string and optionally a sort
    key, and can display itself in a TQIconView.
    The class is designed to be very similar to TQListView and TQListBox
    in use, both via instantiation and subclassing.

    The simplest way to create a TQIconViewItem and insert it into a
    TQIconView is to construct the item passing the constructor a
    pointer to the icon view, a string and an icon:

    \code
    (void) new TQIconViewItem(
                    iconView,   // A pointer to a TQIconView
                    "This is the text of the item",
                    aPixmap );
    \endcode

    By default the text of an icon view item may not be edited by the
    user but calling setRenameEnabled(true) will allow the user to
    perform in-place editing of the item's text.

    When the icon view is deleted all items in it are deleted
    automatically.

    The TQIconView::firstItem() and TQIconViewItem::nextItem() functions
    provide a means of iterating over all the items in a TQIconView:

    \code
    TQIconViewItem *item;
    for ( item = iconView->firstItem(); item; item = item->nextItem() )
        do_something_with( item );
    \endcode

    The item's icon view is available from iconView(), and its
    position in the icon view from index().

    The item's selection status is available from isSelected() and is
    set and controlled by setSelected() and isSelectable().

    The text and icon can be set with setText() and setPixmap() and
    retrieved with text() and pixmap(). The item's sort key defaults
    to text() but may be set with setKey() and retrieved with key().
    The comparison function, compare() uses key().

    Items may be repositioned with move() and moveBy(). An item's
    geometry is available from rect(), x(), y(), width(), height(),
    size(), pos(), textRect() and pixmapRect(). You can also test
    against the position of a point with contains() and intersects().

    To remove an item from an icon view, just delete the item. The
    TQIconViewItem destructor removes it cleanly from its icon view.

    Because the icon view is designed to use drag-and-drop, the icon
    view item also has functions for drag-and-drop which may be
    reimplemented.

    \target pixmap-size-limit
    <b>Note:</b> Pixmaps with individual dimensions larger than 300 pixels may
    not be displayed properly, depending on the \link TQIconView::Arrangement
    arrangement in use\endlink. For example, pixmaps wider than 300 pixels
    will not be arranged correctly if the icon view uses a
    \l TQIconView::TopToBottom arrangement, and pixmaps taller than 300 pixels
    will not be arranged correctly if the icon view uses a
    \l TQIconView::LeftToRight arrangement.
*/

/*!
    Constructs a TQIconViewItem and inserts it into icon view \a parent
    with no text and a default icon.
*/

TQIconViewItem::TQIconViewItem( TQIconView *parent )
    : view( parent ), itemText(), itemIcon( unknown_icon )
{
    init();
}

/*!
    Constructs a TQIconViewItem and inserts it into the icon view \a
    parent with no text and a default icon, after the icon view item
    \a after.
*/

TQIconViewItem::TQIconViewItem( TQIconView *parent, TQIconViewItem *after )
    : view( parent ), itemText(), itemIcon( unknown_icon ),
      prev( 0 ), next( 0 )
{
    init( after );
}

/*!
    Constructs an icon view item  and inserts it into the icon view \a
    parent using \a text as the text and a default icon.
*/

TQIconViewItem::TQIconViewItem( TQIconView *parent, const TQString &text )
    : view( parent ), itemText( text ), itemIcon( unknown_icon )
{
    init( 0 );
}

/*!
    Constructs an icon view item and inserts it into the icon view \a
    parent using \a text as the text and a default icon, after the
    icon view item \a after.
*/

TQIconViewItem::TQIconViewItem( TQIconView *parent, TQIconViewItem *after,
			      const TQString &text )
    : view( parent ), itemText( text ), itemIcon( unknown_icon )
{
    init( after );
}

/*!
    Constructs an icon view item and inserts it into the icon view \a
    parent using \a text as the text and \a icon as the icon.
*/

TQIconViewItem::TQIconViewItem( TQIconView *parent, const TQString &text,
			      const TQPixmap &icon )
    : view( parent ),
      itemText( text ), itemIcon( new TQPixmap( icon ) )
{
    init( 0 );
}


/*!
    Constructs an icon view item and inserts it into the icon view \a
    parent using \a text as the text and \a icon as the icon, after
    the icon view item \a after.

    \sa setPixmap()
*/

TQIconViewItem::TQIconViewItem( TQIconView *parent, TQIconViewItem *after,
			      const TQString &text, const TQPixmap &icon )
    : view( parent ), itemText( text ), itemIcon( new TQPixmap( icon ) )
{
    init( after );
}

/*!
    Constructs an icon view item and inserts it into the icon view \a
    parent using \a text as the text and \a picture as the icon.
*/

#ifndef TQT_NO_PICTURE
TQIconViewItem::TQIconViewItem( TQIconView *parent, const TQString &text,
			      const TQPicture &picture )
    : view( parent ), itemText( text ), itemIcon( 0 )
{
    init( 0, new TQPicture( picture ) );
}

/*!
    Constructs an icon view item and inserts it into the icon view \a
    parent using \a text as the text and \a picture as the icon, after
    the icon view item \a after.
*/

TQIconViewItem::TQIconViewItem( TQIconView *parent, TQIconViewItem *after,
			      const TQString &text, const TQPicture &picture )
    : view( parent ), itemText( text ), itemIcon( 0 )
{
    init( after, new TQPicture( picture ) );
}
#endif

/*!
  This private function initializes the icon view item and inserts it
  into the icon view.
*/

void TQIconViewItem::init( TQIconViewItem *after
#ifndef TQT_NO_PICTURE
			  , TQPicture *pic
#endif
			  )
{
    d = new TQIconViewItemPrivate;
    d->container1 = 0;
    d->container2 = 0;
    prev = next = 0;
    allow_rename = false;
    allow_drag = true;
    allow_drop = true;
    visible = true;
    selected = false;
    selectable = true;
#ifndef TQT_NO_TEXTEDIT
    renameBox = 0;
#endif
#ifndef TQT_NO_PICTURE
    itemPic = pic;
#endif
    if ( view ) {
	itemKey = itemText;
	dirty = true;
	wordWrapDirty = true;
	itemRect = TQRect( -1, -1, 0, 0 );
	calcRect();
	view->insertItem( this, after );
    }
}

/*!
    Destroys the icon view item and tells the parent icon view that
    the item has been destroyed.
*/

TQIconViewItem::~TQIconViewItem()
{
#ifndef TQT_NO_TEXTEDIT
    removeRenameBox();
#endif
    if ( view && !view->d->clearing )
	view->takeItem( this );
    view = 0;
    if ( itemIcon && itemIcon->serialNumber() != unknown_icon->serialNumber() )
	delete itemIcon;
#ifndef TQT_NO_PICTURE
    delete itemPic;
#endif
    delete d;
}

int TQIconViewItem::RTTI = 0;

/*!
    Returns 0.

    Make your derived classes return their own values for rtti(), so
    that you can distinguish between icon view item types. You should
    use values greater than 1000, preferably a large random number, to
    allow for extensions to this class.
*/

int TQIconViewItem::rtti() const
{
    return RTTI;
}

/*!
    If \a b is true, the item is made visible; otherwise it is hidden.
*/

void TQIconViewItem::setVisible( bool b )
{
    if ( b == (bool)visible )
	return;
    TQIconView *iv = iconView();
    if ( !iv )
	return;
    visible = b;
    if ( iv )
	iv->updateContents();
}

/*!
    Returns true if the item is visible; otherwise returns false.

    \sa setVisible()
*/

bool TQIconViewItem::isVisible() const
{
    return (bool)visible;
}

/*!
    Sets \a text as the text of the icon view item. This function
    might be a no-op if you reimplement text().

    \sa text()
*/

void TQIconViewItem::setText( const TQString &text )
{
    if ( text == itemText )
	return;

    wordWrapDirty = true;
    itemText = text;
    if ( itemKey.isEmpty() )
	itemKey = itemText;

    TQRect oR = rect();
    calcRect();
    oR = oR.unite( rect() );

    if ( view ) {
	if ( TQRect( view->contentsX(), view->contentsY(),
		    view->visibleWidthSB(), view->visibleHeightSB() ).
	     intersects( oR ) )
	    view->repaintContents( oR.x() - 1, oR.y() - 1,
				   oR.width() + 2, oR.height() + 2, false );
    }
}

/*!
    Sets \a k as the sort key of the icon view item. By default
    text() is used for sorting.

    \sa compare()
*/

void TQIconViewItem::setKey( const TQString &k )
{
    if ( k == itemKey )
	return;

    itemKey = k;
}

/*!
    Sets \a icon as the item's icon in the icon view. This function
    might be a no-op if you reimplement pixmap().

    <b>Note:</b> Pixmaps with individual dimensions larger than 300 pixels may
    not be displayed properly, depending on the \link TQIconView::Arrangement
    arrangement in use\endlink. See the \link #pixmap-size-limit main class
    documentation\endlink for details.

    \sa pixmap()
*/

void TQIconViewItem::setPixmap( const TQPixmap &icon )
{
    if (isVisible() == false)
	return;

    if ( itemIcon && itemIcon == unknown_icon )
	itemIcon = 0;

    if ( itemIcon )
	*itemIcon = icon;
    else
	itemIcon = new TQPixmap( icon );
    TQRect oR = rect();
    calcRect();
    oR = oR.unite( rect() );

    if ( view ) {
	if ( TQRect( view->contentsX(), view->contentsY(),
		    view->visibleWidthSB(), view->visibleHeightSB() ).
	     intersects( oR ) )
	    view->repaintContents( oR.x() - 1, oR.y() - 1,
				   oR.width() + 2, oR.height() + 2, false );
    }
}

/*!
    Sets \a icon as the item's icon in the icon view. This function
    might be a no-op if you reimplement picture().

    \sa picture()
*/

#ifndef TQT_NO_PICTURE
void TQIconViewItem::setPicture( const TQPicture &icon )
{
    // clear assigned pixmap if any
    if ( itemIcon ) {
	if ( itemIcon == unknown_icon ) {
	    itemIcon = 0;
	} else {
	    delete itemIcon;
	    itemIcon = 0;
	}
    }
    if ( itemPic )
	delete itemPic;
    itemPic = new TQPicture( icon );

    TQRect oR = rect();
    calcRect();
    oR = oR.unite( rect() );

    if ( view ) {
	if ( TQRect( view->contentsX(), view->contentsY(),
		    view->visibleWidthSB(), view->visibleHeightSB() ).
	     intersects( oR ) )
	    view->repaintContents( oR.x() - 1, oR.y() - 1,
				   oR.width() + 2, oR.height() + 2, false );
    }
}
#endif

/*!
    \overload

    Sets \a text as the text of the icon view item. If \a recalc is
    true, the icon view's layout is recalculated. If \a redraw is true
    (the default), the icon view is repainted.

    \sa text()
*/

void TQIconViewItem::setText( const TQString &text, bool recalc, bool redraw )
{
    if ( text == itemText )
	return;

    wordWrapDirty = true;
    itemText = text;

    if ( recalc )
	calcRect();
    if ( redraw )
	repaint();
}

/*!
    \overload

    Sets \a icon as the item's icon in the icon view. If \a recalc is
    true, the icon view's layout is recalculated. If \a redraw is true
    (the default), the icon view is repainted.

    <b>Note:</b> Pixmaps with individual dimensions larger than 300 pixels may
    not be displayed properly, depending on the \link TQIconView::Arrangement
    arrangement in use\endlink. See the \link #pixmap-size-limit main class
    documentation\endlink for details.

    \sa pixmap()
*/

void TQIconViewItem::setPixmap( const TQPixmap &icon, bool recalc, bool redraw )
{
    if (isVisible() == false)
	return;

    if ( itemIcon && itemIcon == unknown_icon )
	itemIcon = 0;

    if ( itemIcon )
	*itemIcon = icon;
    else
	itemIcon = new TQPixmap( icon );

    if ( redraw ) {
	if ( recalc ) {
	    TQRect oR = rect();
	    calcRect();
	    oR = oR.unite( rect() );

	    if ( view ) {
		if ( TQRect( view->contentsX(), view->contentsY(),
			    view->visibleWidthSB(), view->visibleHeightSB() ).
		     intersects( oR ) )
		    view->repaintContents( oR.x() - 1, oR.y() - 1,
					   oR.width() + 2, oR.height() + 2, false );
	    }
	} else {
	    repaint();
	}
    } else if ( recalc ) {
	calcRect();
    }
}

/*!
    If \a allow is true, the user can rename the icon view item by
    clicking on the text (or pressing F2) while the item is selected
    (in-place renaming). If \a allow is false, in-place renaming is
    not possible.
*/

void TQIconViewItem::setRenameEnabled( bool allow )
{
    allow_rename = (uint)allow;
}

/*!
    If \a allow is true, the icon view permits the user to drag the
    icon view item either to another position within the icon view or
    to somewhere outside of it. If \a allow is false, the item cannot
    be dragged.
*/

void TQIconViewItem::setDragEnabled( bool allow )
{
    allow_drag = (uint)allow;
}

/*!
    If \a allow is true, the icon view lets the user drop something on
    this icon view item.
*/

void TQIconViewItem::setDropEnabled( bool allow )
{
    allow_drop = (uint)allow;
}

/*!
    Returns the text of the icon view item. Normally you set the text
    of the item with setText(), but sometimes it's inconvenient to
    call setText() for every item; so you can subclass TQIconViewItem,
    reimplement this function, and return the text of the item. If you
    do this, you must call calcRect() manually each time the text
    (and therefore its size) changes.

    \sa setText()
*/

TQString TQIconViewItem::text() const
{
    return itemText;
}

/*!
    Returns the key of the icon view item or text() if no key has been
    explicitly set.

    \sa setKey(), compare()
*/

TQString TQIconViewItem::key() const
{
    return itemKey;
}

/*!
    Returns the icon of the icon view item if it is a pixmap, or 0 if
    it is a picture. In the latter case use picture() instead.
    Normally you set the pixmap of the item with setPixmap(), but
    sometimes it's inconvenient to call setPixmap() for every item. So
    you can subclass TQIconViewItem, reimplement this function and
    return a pointer to the item's pixmap. If you do this, you \e must
    call calcRect() manually each time the size of this pixmap
    changes.

    \sa setPixmap()
*/

TQPixmap *TQIconViewItem::pixmap() const
{
    return itemIcon;
}

/*!
    Returns the icon of the icon view item if it is a picture, or 0 if
    it is a pixmap. In the latter case use pixmap() instead. Normally
    you set the picture of the item with setPicture(), but sometimes
    it's inconvenient to call setPicture() for every item. So you can
    subclass TQIconViewItem, reimplement this function and return a
    pointer to the item's picture. If you do this, you \e must call
    calcRect() manually each time the size of this picture changes.

    \sa setPicture()
*/

#ifndef TQT_NO_PICTURE
TQPicture *TQIconViewItem::picture() const
{
    return itemPic;
}
#endif

/*!
    Returns true if the item can be renamed by the user with in-place
    renaming; otherwise returns false.

    \sa setRenameEnabled()
*/

bool TQIconViewItem::renameEnabled() const
{
    return (bool)allow_rename;
}

/*!
    Returns true if the user is allowed to drag the icon view item;
    otherwise returns false.

    \sa setDragEnabled()
*/

bool TQIconViewItem::dragEnabled() const
{
    return (bool)allow_drag;
}

/*!
    Returns true if the user is allowed to drop something onto the
    item; otherwise returns false.

    \sa setDropEnabled()
*/

bool TQIconViewItem::dropEnabled() const
{
    return (bool)allow_drop;
}

/*!
    Returns a pointer to this item's icon view parent.
*/

TQIconView *TQIconViewItem::iconView() const
{
    return view;
}

/*!
    Returns a pointer to the previous item, or 0 if this is the first
    item in the icon view.

    \sa nextItem() TQIconView::firstItem()
*/

TQIconViewItem *TQIconViewItem::prevItem() const
{
    return prev;
}

/*!
    Returns a pointer to the next item, or 0 if this is the last item
    in the icon view.

    To find the first item use TQIconView::firstItem().

    Example:
    \code
    TQIconViewItem *item;
    for ( item = iconView->firstItem(); item; item = item->nextItem() )
	do_something_with( item );
    \endcode

    \sa prevItem()
*/

TQIconViewItem *TQIconViewItem::nextItem() const
{
    return next;
}

/*!
    Returns the index of this item in the icon view, or -1 if an error
    occurred.
*/

int TQIconViewItem::index() const
{
    if ( view )
	return view->index( this );

    return -1;
}



/*!
    \overload

    This variant is equivalent to calling the other variant with \e cb
    set to false.
*/

void TQIconViewItem::setSelected( bool s )
{
    setSelected( s, false );
}

/*!
    Selects or unselects the item, depending on \a s; it may also
    unselect other items, depending on TQIconView::selectionMode() and
    \a cb.

    If \a s is false, the item is unselected.

    If \a s is true and TQIconView::selectionMode() is \c Single, the
    item is selected and the item previously selected is unselected.

    If \a s is true and TQIconView::selectionMode() is \c Extended, the
    item is selected. If \a cb is true, the selection state of the
    other items is left unchanged. If \a cb is false (the default) all
    other items are unselected.

    If \a s is true and TQIconView::selectionMode() is \c Multi, the
    item is selected.

    Note that \a cb is used only if TQIconView::selectionMode() is \c
    Extended; cb defaults to false.

    All items whose selection status changes repaint themselves.
*/

void TQIconViewItem::setSelected( bool s, bool cb )
{
    if ( !view )
	return;
    if ( view->selectionMode() != TQIconView::NoSelection &&
	 selectable && s != (bool)selected ) {

	if ( view->d->selectionMode == TQIconView::Single && this != view->d->currentItem ) {
	    TQIconViewItem *o = view->d->currentItem;
	    if ( o && o->selected )
		o->selected = false;
	    view->d->currentItem = this;
	    if ( o )
		o->repaint();
	    emit view->currentChanged( this );
	}

	if ( !s ) {
	    selected = false;
	} else {
	    if ( view->d->selectionMode == TQIconView::Single && view->d->currentItem ) {
		view->d->currentItem->selected = false;
	    }
	    if ( ( view->d->selectionMode == TQIconView::Extended && !cb ) ||
		 view->d->selectionMode == TQIconView::Single ) {
		bool b = view->signalsBlocked();
		view->blockSignals( true );
		view->selectAll( false );
		view->blockSignals( b );
	    }
	    selected = s;
	}

 	repaint();
	if ( !view->signalsBlocked() ) {
	    bool emitIt = view->d->selectionMode == TQIconView::Single && s;
	    TQIconView *v = view;
	    emit v->selectionChanged();
	    if ( emitIt )
		emit v->selectionChanged( this );
	}
    }
}

/*!
    Sets this item to be selectable if \a enable is true (the default)
    or unselectable if \a enable is false.

    The user is unable to select a non-selectable item using either
    the keyboard or the mouse. (The application programmer can select
    an item in code regardless of this setting.)

    \sa isSelectable()
*/

void TQIconViewItem::setSelectable( bool enable )
{
    selectable = (uint)enable;
}

/*!
    Returns true if the item is selected; otherwise returns false.

    \sa setSelected()
*/

bool TQIconViewItem::isSelected() const
{
    return (bool)selected;
}

/*!
    Returns true if the item is selectable; otherwise returns false.

    \sa setSelectable()
*/

bool TQIconViewItem::isSelectable() const
{
    return (bool)selectable;
}

/*!
    Repaints the item.
*/

void TQIconViewItem::repaint()
{
    if ( view )
	view->repaintItem( this );
}

/*!
    Moves the item to position (\a x, \a y) in the icon view (these
    are contents coordinates).
*/

bool TQIconViewItem::move( int x, int y )
{
    if ( x == this->x() && y == this->y() )
	return false;
    itemRect.setRect( x, y, itemRect.width(), itemRect.height() );
    checkRect();
    if ( view )
	view->updateItemContainer( this );
    return true;
}

/*!
    Moves the item \a dx pixels in the x-direction and \a dy pixels in
    the y-direction.
*/

void TQIconViewItem::moveBy( int dx, int dy )
{
    itemRect.moveBy( dx, dy );
    checkRect();
    if ( view )
	view->updateItemContainer( this );
}

/*!
    \overload

    Moves the item to the point \a pnt.
*/

bool TQIconViewItem::move( const TQPoint &pnt )
{
    return move( pnt.x(), pnt.y() );
}

/*!
    \overload

    Moves the item by the x, y values in point \a pnt.
*/

void TQIconViewItem::moveBy( const TQPoint &pnt )
{
    moveBy( pnt.x(), pnt.y() );
}

/*!
    Returns the bounding rectangle of the item (in contents
    coordinates).
*/

TQRect TQIconViewItem::rect() const
{
    return itemRect;
}

/*!
    Returns the x-coordinate of the item (in contents coordinates).
*/

int TQIconViewItem::x() const
{
    return itemRect.x();
}

/*!
    Returns the y-coordinate of the item (in contents coordinates).
*/

int TQIconViewItem::y() const
{
    return itemRect.y();
}

/*!
    Returns the width of the item.
*/

int TQIconViewItem::width() const
{
    return TQMAX( itemRect.width(), TQApplication::globalStrut().width() );
}

/*!
    Returns the height of the item.
*/

int TQIconViewItem::height() const
{
    return TQMAX( itemRect.height(), TQApplication::globalStrut().height() );
}

/*!
    Returns the size of the item.
*/

TQSize TQIconViewItem::size() const
{
    return TQSize( itemRect.width(), itemRect.height() );
}

/*!
    Returns the position of the item (in contents coordinates).
*/

TQPoint TQIconViewItem::pos() const
{
    return TQPoint( itemRect.x(), itemRect.y() );
}

/*!
    Returns the bounding rectangle of the item's text.

    If \a relative is true, (the default), the returned rectangle is
    relative to the origin of the item's rectangle. If \a relative is
    false, the returned rectangle is relative to the origin of the
    icon view's contents coordinate system.
*/

TQRect TQIconViewItem::textRect( bool relative ) const
{
    if ( relative )
	return itemTextRect;
    else
	return TQRect( x() + itemTextRect.x(), y() + itemTextRect.y(), itemTextRect.width(), itemTextRect.height() );
}

/*!
    Returns the bounding rectangle of the item's icon.

    If \a relative is true, (the default), the rectangle is relative to
    the origin of the item's rectangle. If \a relative is false, the
    returned rectangle is relative to the origin of the icon view's
    contents coordinate system.
*/

TQRect TQIconViewItem::pixmapRect( bool relative ) const
{
    if ( relative )
	return itemIconRect;
    else
	return TQRect( x() + itemIconRect.x(), y() + itemIconRect.y(), itemIconRect.width(), itemIconRect.height() );
}

/*!
    Returns true if the item contains the point \a pnt (in contents
    coordinates); otherwise returns false.
*/

bool TQIconViewItem::contains( const TQPoint& pnt ) const
{
    TQRect textArea = textRect( false );
    TQRect pixmapArea = pixmapRect( false );
    if ( iconView()->itemTextPos() == TQIconView::Bottom )
	textArea.setTop( pixmapArea.bottom() );
    else
	textArea.setLeft( pixmapArea.right() );
    return textArea.contains( pnt ) || pixmapArea.contains( pnt );
}

/*!
    Returns true if the item intersects the rectangle \a r (in
    contents coordinates); otherwise returns false.
*/

bool TQIconViewItem::intersects( const TQRect& r ) const
{
    return ( textRect( false ).intersects( r ) ||
	     pixmapRect( false ).intersects( r ) );
}

/*!
    \fn bool TQIconViewItem::acceptDrop( const TQMimeSource *mime ) const

    Returns true if you can drop things with a TQMimeSource of \a mime
    onto this item; otherwise returns false.

    The default implementation always returns false. You must subclass
    TQIconViewItem and reimplement acceptDrop() to accept drops.
*/

bool TQIconViewItem::acceptDrop( const TQMimeSource * ) const
{
    return false;
}

#ifndef TQT_NO_TEXTEDIT
/*!
    Starts in-place renaming of an icon, if allowed.

    This function sets up the icon view so that the user can edit the
    item text, and then returns. When the user is done, setText() will
    be called and TQIconView::itemRenamed() will be emitted (unless the
    user canceled, e.g. by pressing the Escape key).

    \sa setRenameEnabled()
*/

void TQIconViewItem::rename()
{
    if ( !view )
	return;
    if ( renameBox )
	removeRenameBox();
    oldRect = rect();
    renameBox = new TQIconViewItemLineEdit( itemText, view->viewport(), this, "qt_renamebox" );
    iconView()->ensureItemVisible( this );
    TQRect tr( textRect( false ) );
    view->addChild( renameBox, tr.x() + ( tr.width() / 2 - renameBox->width() / 2 ), tr.y() - 3 );
    renameBox->selectAll();
    view->viewport()->setFocusProxy( renameBox );
    renameBox->setFocus();
    renameBox->show();
    Q_ASSERT( view->d->renamingItem == 0L );
    view->d->renamingItem = this;
}
#endif

/*!
    Compares this icon view item to \a i. Returns -1 if this item is
    less than \a i, 0 if they are equal, and 1 if this icon view item
    is greater than \a i.

    The default implementation compares the item keys (key()) using
    TQString::localeAwareCompare(). A reimplementation may use
    different values and a different comparison function. Here is a
    reimplementation that uses plain Unicode comparison:

    \code
	int MyIconViewItem::compare( TQIconViewItem *i ) const
	{
	    return key().compare( i->key() );
	}
    \endcode

    \sa key() TQString::localeAwareCompare() TQString::compare()
*/

int TQIconViewItem::compare( TQIconViewItem *i ) const
{
    return key().localeAwareCompare( i->key() );
}

#ifndef TQT_NO_TEXTEDIT
/*!
  This private function is called when the user pressed Return during
  in-place renaming.
*/

void TQIconViewItem::renameItem()
{
    if ( !renameBox || !view )
	return;

    if ( !view->d->wordWrapIconText ) {
	wordWrapDirty = true;
	calcRect();
    }
    TQRect r = itemRect;
    setText( renameBox->text() );
    view->repaintContents( oldRect.x() - 1, oldRect.y() - 1, oldRect.width() + 2, oldRect.height() + 2, false );
    view->repaintContents( r.x() - 1, r.y() - 1, r.width() + 2, r.height() + 2, false );
    removeRenameBox();

    view->emitRenamed( this );
}

/*!
    Cancels in-place renaming.
*/

void TQIconViewItem::cancelRenameItem()
{
    if ( !view )
	return;

    TQRect r = itemRect;
    calcRect();
    view->repaintContents( oldRect.x() - 1, oldRect.y() - 1, oldRect.width() + 2, oldRect.height() + 2, false );
    view->repaintContents( r.x() - 1, r.y() - 1, r.width() + 2, r.height() + 2, false );

    if ( !renameBox )
	return;

    removeRenameBox();
}

/*!
    Removes the editbox that is used for in-place renaming.
*/

void TQIconViewItem::removeRenameBox()
{
    if ( !renameBox || !view )
	return;

    bool resetFocus = view->viewport()->focusProxy() == renameBox;
    renameBox->hide();
    renameBox->deleteLater();
    renameBox = 0;
    if ( resetFocus ) {
	view->viewport()->setFocusProxy( view );
	view->setFocus();
    }
    Q_ASSERT( view->d->renamingItem == this );
    view->d->renamingItem = 0L;
}
#endif

/*!
    This virtual function is responsible for calculating the
    rectangles returned by rect(), textRect() and pixmapRect().
    setRect(), setTextRect() and setPixmapRect() are provided mainly
    for reimplementations of this function.

    \a text_ is an internal parameter which defaults to TQString::null.
*/

void TQIconViewItem::calcRect( const TQString &text_ )
{
    if ( !view ) // #####
	return;

    wordWrapDirty = true;
    int pw = 0;
    int ph = 0;

#ifndef TQT_NO_PICTURE
    if ( picture() ) {
	TQRect br = picture()->boundingRect();
	pw = br.width() + 2;
	ph = br.height() + 2;
    } else
#endif
    {
	pw = ( pixmap() ? pixmap() : unknown_icon )->width() + 2;
	ph = ( pixmap() ? pixmap() : unknown_icon )->height() + 2;
    }

    itemIconRect.setWidth( pw );
    itemIconRect.setHeight( ph );

    calcTmpText();

    TQString t = text_;
    if ( t.isEmpty() ) {
	if ( view->d->wordWrapIconText )
	    t = itemText;
	else
	    t = tmpText;
    }

    int tw = 0;
    int th = 0;
    // ##### TODO: fix font bearings!
    TQRect r;
    if ( view->d->wordWrapIconText ) {
	r = TQRect( view->d->fm->boundingRect( 0, 0, iconView()->maxItemWidth() -
					      ( iconView()->itemTextPos() == TQIconView::Bottom ? 0 :
						pixmapRect().width() ),
					      0xFFFFFFFF, AlignHCenter | WordBreak | BreakAnywhere, t ) );
	r.setWidth( r.width() + 4 );
    } else {
	r = TQRect( 0, 0, view->d->fm->width( t ), view->d->fm->height() );
	r.setWidth( r.width() + 4 );
    }

    if ( r.width() > iconView()->maxItemWidth() -
	 ( iconView()->itemTextPos() == TQIconView::Bottom ? 0 :
	   pixmapRect().width() ) )
	r.setWidth( iconView()->maxItemWidth() - ( iconView()->itemTextPos() == TQIconView::Bottom ? 0 :
						   pixmapRect().width() ) );

    tw = r.width();
    th = r.height();
    if ( tw < view->d->fm->width( "X" ) )
	tw = view->d->fm->width( "X" );

    itemTextRect.setWidth( tw );
    itemTextRect.setHeight( th );

    int w = 0;
    int h = 0;
    if ( view->itemTextPos() == TQIconView::Bottom ) {
	w = TQMAX( itemTextRect.width(), itemIconRect.width() );
	h = itemTextRect.height() + itemIconRect.height() + 1;

	itemRect.setWidth( w );
	itemRect.setHeight( h );

	itemTextRect = TQRect( ( width() - itemTextRect.width() ) / 2, height() - itemTextRect.height(),
			      itemTextRect.width(), itemTextRect.height() );
	itemIconRect = TQRect( ( width() - itemIconRect.width() ) / 2, 0,
			      itemIconRect.width(), itemIconRect.height() );
    } else {
	h = TQMAX( itemTextRect.height(), itemIconRect.height() );
	w = itemTextRect.width() + itemIconRect.width() + 1;

	itemRect.setWidth( w );
	itemRect.setHeight( h );

	itemTextRect = TQRect( width() - itemTextRect.width(), ( height() - itemTextRect.height() ) / 2,
			      itemTextRect.width(), itemTextRect.height() );
	itemIconRect = TQRect( 0, ( height() - itemIconRect.height() ) / 2,
			      itemIconRect.width(), itemIconRect.height() );
    }
    if ( view )
	view->updateItemContainer( this );
}

/*!
    Paints the item using the painter \a p and the color group \a cg.
    If you want the item to be drawn with a different font or color,
    reimplement this function, change the values of the color group or
    the painter's font, and then call the TQIconViewItem::paintItem()
    with the changed values.
*/

void TQIconViewItem::paintItem( TQPainter *p, const TQColorGroup &cg )
{
    if ( !view )
	return;

    p->save();

    if ( isSelected() ) {
	p->setPen( cg.highlightedText() );
    } else {
	p->setPen( cg.text() );
    }

    calcTmpText();

#ifndef TQT_NO_PICTURE
    if ( picture() ) {
	TQPicture *pic = picture();
	if ( isSelected() ) {
            p->setBrush( TQBrush( cg.highlight(), TQBrush::Dense4Pattern ) );
            p->setPen( TQPen( cg.highlight(), TQBrush::Dense4Pattern ) );
            p->drawRoundRect( pixmapRect( false ), 
			    1000 / pixmapRect( false ).width(),
			    1000 / pixmapRect( false ).height() );		
	}
	p->drawPicture( x()-pic->boundingRect().x(), y()-pic->boundingRect().y(), *pic );
	if ( isSelected() ) {
            p->setBrush( TQBrush( cg.highlight() ) );
            p->setPen( TQPen( cg.highlight() ) );
            p->drawRoundRect( textRect( false ), 
			    1000 / textRect( false ).width(),
			    1000 / textRect( false ).height() );		
	    p->setPen( TQPen( cg.highlightedText() ) );
	} else if ( view->d->itemTextBrush != NoBrush ) {
            p->setBrush( view->d->itemTextBrush );
            p->setPen( TQPen( view->d->itemTextBrush.color() ) );
            p->drawRoundRect( textRect( false ), 
			    1000 / textRect( false ).width(),
			    1000 / textRect( false ).height() );		
	}

	int align = view->itemTextPos() == TQIconView::Bottom ? AlignHCenter : AlignAuto;
	if ( view->d->wordWrapIconText )
	    align |= WordBreak | BreakAnywhere;
	p->drawText( textRect( false ), align, view->d->wordWrapIconText ? itemText : tmpText );
	p->restore();
	return;
    }
#endif
    bool textOnBottom = ( view->itemTextPos() == TQIconView::Bottom );
    int dim;
    if ( textOnBottom )
	dim = ( pixmap() ? pixmap() : unknown_icon)->width();
    else
	dim = ( pixmap() ? pixmap() : unknown_icon)->height();
    if ( isSelected() ) {
	TQPixmap *pix = pixmap() ? pixmap() : unknown_icon;
	if ( pix && !pix->isNull() ) {
	    TQPixmap *buffer = get_qiv_buffer_pixmap( pix->size() );
	    TQBitmap mask = view->mask( pix );

	    TQPainter p2( buffer );
	    p2.fillRect( pix->rect(), white );
	    p2.drawPixmap( 0, 0, *pix );
	    p2.end();
	    buffer->setMask( mask );
	    p2.begin( buffer );
#if defined(TQ_WS_X11)
	    p2.fillRect( pix->rect(), TQBrush( cg.highlight(), TQBrush::Dense4Pattern) );
#else // in WIN32 Dense4Pattern doesn't work correctly (transparency problem), so work around it
	    if ( iconView()->d->drawActiveSelection ) {
		if ( !qiv_selection )
		    createSelectionPixmap( cg );
		p2.drawTiledPixmap( 0, 0, pix->width(), pix->height(),
				    *qiv_selection );
	    }
#endif
	    p2.end();
	    TQRect cr = pix->rect();
	    if ( textOnBottom )
		p->drawPixmap( x() + ( width() - dim ) / 2, y(), *buffer, 0, 0,
			       cr.width(), cr.height() );
	    else
		p->drawPixmap( x() , y() + ( height() - dim ) / 2, *buffer, 0, 0,
			       cr.width(), cr.height() );
	}
    } else {
	if ( textOnBottom )
	    p->drawPixmap( x() + ( width() - dim ) / 2, y(),
			   *( pixmap() ? pixmap() : unknown_icon ) );
	else
	    p->drawPixmap( x() , y() + ( height() - dim ) / 2,
			   *( pixmap() ? pixmap() : unknown_icon ) );
    }

    p->save();
    if ( isSelected() ) {
        p->setBrush( TQBrush( cg.highlight() ) );
        p->setPen( TQPen( cg.highlight() ) );
        p->drawRoundRect( textRect( false ), 
                          1000 / textRect( false ).width(),
			  1000 / textRect( false ).height() );		
	p->setPen( TQPen( cg.highlightedText() ) );
    } else if ( view->d->itemTextBrush != NoBrush ) {
        p->setBrush( view->d->itemTextBrush );
        p->setPen( TQPen( view->d->itemTextBrush.color() ) );
        p->drawRoundRect( textRect( false ), 
			  1000 / textRect( false ).width(),
			  1000 / textRect( false ).height() );		
    }

    int align = AlignHCenter;
    if ( view->d->wordWrapIconText )
	align |= WordBreak | BreakAnywhere;
    p->drawText( textRect( false ), align,
		 view->d->wordWrapIconText ? itemText : tmpText );

    p->restore();

    p->restore();
}

/*!
    Paints the focus rectangle of the item using the painter \a p and
    the color group \a cg.
    
    The default implementation does nothing; subclasses may
    reimplement this function.
*/

void TQIconViewItem::paintFocus( TQPainter *, const TQColorGroup & )
{
}

/*!
    \fn void TQIconViewItem::dropped( TQDropEvent *e, const TQValueList<TQIconDragItem> &lst )

    This function is called when something is dropped on the item. \a
    e provides all the information about the drop. If the drag object
    of the drop was a TQIconDrag, \a lst contains the list of the
    dropped items. You can get the data by calling
    TQIconDragItem::data() on each item. If the \a lst is empty, i.e.
    the drag was not a TQIconDrag, you must decode the data in \a e and
    work with that.

    The default implementation does nothing; subclasses may
    reimplement this function.
*/

#ifndef TQT_NO_DRAGANDDROP
void TQIconViewItem::dropped( TQDropEvent *, const TQValueList<TQIconDragItem> & )
{
}
#endif

/*!
    This function is called when a drag enters the item's bounding
    rectangle.

    The default implementation does nothing; subclasses may
    reimplement this function.
*/

void TQIconViewItem::dragEntered()
{
}

/*!
    This function is called when a drag leaves the item's bounding
    rectangle.

    The default implementation does nothing; subclasses may
    reimplement this function.
*/

void TQIconViewItem::dragLeft()
{
}

/*!
    Sets the bounding rectangle of the whole item to \a r. This
    function is provided for subclasses which reimplement calcRect(),
    so that they can set the calculated rectangle. \e{Any other use is
    discouraged.}

    \sa calcRect() textRect() setTextRect() pixmapRect() setPixmapRect()
*/

void TQIconViewItem::setItemRect( const TQRect &r )
{
    itemRect = r;
    checkRect();
    if ( view )
	view->updateItemContainer( this );
}

/*!
    Sets the bounding rectangle of the item's text to \a r. This
    function is provided for subclasses which reimplement calcRect(),
    so that they can set the calculated rectangle. \e{Any other use is
    discouraged.}

    \sa calcRect() textRect() setItemRect() setPixmapRect()
*/

void TQIconViewItem::setTextRect( const TQRect &r )
{
    itemTextRect = r;
    if ( view )
	view->updateItemContainer( this );
}

/*!
    Sets the bounding rectangle of the item's icon to \a r. This
    function is provided for subclasses which reimplement calcRect(),
    so that they can set the calculated rectangle. \e{Any other use is
    discouraged.}

    \sa calcRect() pixmapRect() setItemRect() setTextRect()
*/

void TQIconViewItem::setPixmapRect( const TQRect &r )
{
    itemIconRect = r;
    if ( view )
	view->updateItemContainer( this );
}

/*!
    \internal
*/

void TQIconViewItem::calcTmpText()
{
    if ( !view || view->d->wordWrapIconText || !wordWrapDirty )
	return;
    wordWrapDirty = false;

    int w = iconView()->maxItemWidth() - ( iconView()->itemTextPos() == TQIconView::Bottom ? 0 :
					   pixmapRect().width() );
    if ( view->d->fm->width( itemText ) < w ) {
	tmpText = itemText;
	return;
    }

    tmpText = "...";
    int i = 0;
    while ( view->d->fm->width( tmpText + itemText[ i ] ) < w )
	tmpText += itemText[ i++ ];
    tmpText.remove( (uint)0, 3 );
    tmpText += "...";
}

/*! \internal */

TQString TQIconViewItem::tempText() const
{
    return tmpText;
}

void TQIconViewItem::checkRect()
{
    int x = itemRect.x();
    int y = itemRect.y();
    int w = itemRect.width();
    int h = itemRect.height();

    bool changed = false;
    if ( x < 0 ) {
	x = 0;
	changed = true;
    }
    if ( y < 0 ) {
	y = 0;
	changed = true;
    }

    if ( changed )
	itemRect.setRect( x, y, w, h );
}


/*! \file iconview/simple_dd/main.h */
/*! \file iconview/simple_dd/main.cpp */


/*!
    \class TQIconView ntqiconview.h
    \brief The TQIconView class provides an area with movable labelled icons.

    \module iconview
    \ingroup advanced
    \mainclass

    A TQIconView can display and manage a grid or other 2D layout of
    labelled icons. Each labelled icon is a TQIconViewItem. Items
    (TQIconViewItems) can be added or deleted at any time; items can be
    moved within the TQIconView. Single or multiple items can be
    selected. Items can be renamed in-place. TQIconView also supports
    \link #draganddrop drag and drop\endlink.

    Each item contains a label string, a pixmap or picture (the icon
    itself) and optionally a sort key. The sort key is used for
    sorting the items and defaults to the label string. The label
    string can be displayed below or to the right of the icon (see \l
    ItemTextPos).

    The simplest way to create a TQIconView is to create a TQIconView
    object and create some TQIconViewItems with the TQIconView as their
    parent, set the icon view's geometry and show it.
    For example:
    \code
    TQIconView *iv = new TQIconView( this );
    TQDir dir( path, "*.xpm" );
    for ( uint i = 0; i < dir.count(); i++ ) {
	(void) new TQIconViewItem( iv, dir[i], TQPixmap( path + dir[i] ) );
    }
    iv->resize( 600, 400 );
    iv->show();
    \endcode

    The TQIconViewItem call passes a pointer to the TQIconView we wish to
    populate, along with the label text and a TQPixmap.

    When an item is inserted the TQIconView allocates a position for it.
    Existing items are rearranged if autoArrange() is true. The
    default arrangement is \c LeftToRight -- the TQIconView fills
    the \e top-most row from left to right, then moves one row \e down
    and fills that row from left to right and so on. The
    arrangement can be modified with any of the following approaches:
    \list
    \i Call setArrangement(), e.g. with \c TopToBottom which will fill up
    the \e left-most column from top to bottom, then moves one column
    \e right and fills that from top to bottom and so on.
    
    \i Construct each TQIconViewItem using a constructor which allows
    you to specify which item the new one is to follow.
    \i Call setSorting() or sort() to sort the items.
    \endlist

    The spacing between items is set with setSpacing(). Items can be
    laid out using a fixed grid using setGridX() and setGridY(); by
    default the TQIconView calculates a grid dynamically. The position
    of items' label text is set with setItemTextPos(). The text's
    background can be set with setItemTextBackground(). The maximum
    width of an item and of its text are set with setMaxItemWidth()
    and setMaxItemTextLength(). The label text will be word-wrapped if
    it is too long; this is controlled by setWordWrapIconText(). If
    the label text is truncated, the user can still see the entire
    text in a tool tip if they hover the mouse over the item. This is
    controlled with setShowToolTips().

    Items which are \link TQIconViewItem::isSelectable()
    selectable\endlink may be selected depending on the SelectionMode;
    the default is \c Single. Because TQIconView offers multiple
    selection it must display keyboard focus and selection state
    separately. Therefore there are functions to set the selection
    state of an item (setSelected()) and to select which item displays
    keyboard focus (setCurrentItem()). When multiple items may be
    selected the icon view provides a rubberband, too.

    When in-place renaming is enabled (it is disabled by default), the
    user may change the item's label. They do this by selecting the item
    (single clicking it or navigating to it with the arrow keys), then
    single clicking it (or pressing F2), and entering their text. If no
    key has been set with TQIconViewItem::setKey() the new text will also
    serve as the key. (See TQIconViewItem::setRenameEnabled().)

    You can control whether users can move items themselves with
    setItemsMovable().

    Because the internal structure used to store the icon view items is
    linear, no iterator class is needed to iterate over all the items.
    Instead we iterate by getting the first item from the \e{icon view}
    and then each subsequent (\l TQIconViewItem::nextItem()) from each
    \e item in turn:
    \code
	for ( TQIconViewItem *item = iv->firstItem(); item; item = item->nextItem() )
	    do_something( item );
    \endcode
    TQIconView also provides currentItem(). You can search for an item
    using findItem() (searching by position or for label text) and
    with findFirstVisibleItem() and findLastVisibleItem(). The number
    of items is returned by count(). An item can be removed from an
    icon view using takeItem(); to delete an item use \c delete. All
    the items can be deleted with clear().

    The TQIconView emits a wide range of useful signals, including
    selectionChanged(), currentChanged(), clicked(), moved() and
    itemRenamed().

    \target draganddrop
    \section1 Drag and Drop

    TQIconView supports the drag and drop of items within the TQIconView
    itself. It also supports the drag and drop of items out of or into
    the TQIconView and drag and drop onto items themselves. The drag and
    drop of items outside the TQIconView can be achieved in a simple way
    with basic functionality, or in a more sophisticated way which
    provides more power and control.

    The simple approach to dragging items out of the icon view is to
    subclass TQIconView and reimplement TQIconView::dragObject().

    \code
    TQDragObject *MyIconView::dragObject()
    {
	return new TQTextDrag( currentItem()->text(), this );
    }
    \endcode

    In this example we create a TQTextDrag object, (derived from
    TQDragObject), containing the item's label and return it as the drag
    object. We could just as easily have created a TQImageDrag from the
    item's pixmap and returned that instead.

    TQIconViews and their TQIconViewItems can also be the targets of drag
    and drops. To make the TQIconView itself able to accept drops connect
    to the dropped() signal. When a drop occurs this signal will be
    emitted with a TQDragEvent and a TQValueList of TQIconDragItems. To
    make a TQIconViewItem into a drop target subclass TQIconViewItem and
    reimplement TQIconViewItem::acceptDrop() and
    TQIconViewItem::dropped().

    \code
    bool MyIconViewItem::acceptDrop( const TQMimeSource *mime ) const
    {
	return mime->provides( "text/plain" );
    }

    void MyIconViewItem::dropped( TQDropEvent *evt, const TQValueList<TQIconDragItem>& )
    {
	TQString label;
	if ( TQTextDrag::decode( evt, label ) )
	    setText( label );
    }
    \endcode

    See \l iconview/simple_dd/main.h and \l
    iconview/simple_dd/main.cpp for a simple drag and drop example
    which demonstrates drag and drop between a TQIconView and a
    TQListBox.

    If you want to use extended drag-and-drop or have drag shapes drawn
    you must take a more sophisticated approach.

    The first part is starting drags -- you should use a TQIconDrag (or a
    class derived from it) for the drag object. In dragObject() create the
    drag object, populate it with TQIconDragItems and return it. Normally
    such a drag should offer each selected item's data. So in dragObject()
    you should iterate over all the items, and create a TQIconDragItem for
    each selected item, and append these items with TQIconDrag::append() to
    the TQIconDrag object. You can use TQIconDragItem::setData() to set the
    data of each item that should be dragged. If you want to offer the
    data in additional mime-types, it's best to use a class derived from
    TQIconDrag, which implements additional encoding and decoding
    functions.

    When a drag enters the icon view, there is little to do. Simply
    connect to the dropped() signal and reimplement
    TQIconViewItem::acceptDrop() and TQIconViewItem::dropped(). If you've
    used a TQIconDrag (or a subclass of it) the second argument to the
    dropped signal contains a TQValueList of TQIconDragItems -- you can
    access their data by calling TQIconDragItem::data() on each one.

    For an example implementation of complex drag-and-drop look at the
    fileiconview example (qt/examples/fileiconview).

    \sa TQIconViewItem::setDragEnabled(), TQIconViewItem::setDropEnabled(),
	TQIconViewItem::acceptDrop(), TQIconViewItem::dropped().

    <img src=qiconview-m.png> <img src=qiconview-w.png>
*/

/*! \enum TQIconView::ResizeMode

    This enum type is used to tell TQIconView how it should treat the
    positions of its icons when the widget is resized. The modes are:

    \value Fixed  The icons' positions are not changed.
    \value Adjust  The icons' positions are adjusted to be within the
    new geometry, if possible.
*/

/*!
    \enum TQIconView::SelectionMode

    This enumerated type is used by TQIconView to indicate how it
    reacts to selection by the user. It has four values:

    \value Single  When the user selects an item, any already-selected
    item becomes unselected and the user cannot unselect the selected
    item. This means that the user can never clear the selection. (The
    application programmer can, using TQIconView::clearSelection().)

    \value Multi  When the user selects an item, e.g. by navigating
    to it with the keyboard arrow keys or by clicking it, the
    selection status of that item is toggled and the other items are
    left alone. Also, multiple items can be selected by dragging the
    mouse while the left mouse button stays pressed.

    \value Extended  When the user selects an item the selection is
    cleared and the new item selected. However, if the user presses
    the Ctrl key when clicking on an item, the clicked item gets
    toggled and all other items are left untouched. If the user
    presses the Shift key while clicking on an item, all items between
    the current item and the clicked item get selected or unselected,
    depending on the state of the clicked item. Also, multiple items
    can be selected by dragging the mouse while the left mouse button
    stays pressed.

    \value NoSelection  Items cannot be selected.

    To summarise: \c Single is a real single-selection icon view; \c
    Multi a real multi-selection icon view; \c Extended is an icon
    view in which users can select multiple items but usually want to
    select either just one or a range of contiguous items; and \c
    NoSelection mode is for an icon view where the user can look but
    not touch.
*/

/*!
    \enum TQIconView::Arrangement

    This enum type determines in which direction the items flow when
    the view runs out of space.

    \value LeftToRight  Items which don't fit into the view cause the
    viewport to extend vertically (you get a vertical scrollbar).

    \value TopToBottom  Items which don't fit into the view cause the
    viewport to extend horizontally (you get a horizontal scrollbar).
*/

/*!
    \enum TQIconView::ItemTextPos

    This enum type specifies the position of the item text in relation
    to the icon.

    \value Bottom  The text is drawn below the icon.
    \value Right  The text is drawn to the right of the icon.
*/

/*!
    \fn void  TQIconView::dropped ( TQDropEvent * e, const TQValueList<TQIconDragItem> &lst )

    This signal is emitted when a drop event occurs in the viewport
    (but not on any icon) which the icon view itself can't handle.

    \a e provides all the information about the drop. If the drag
    object of the drop was a TQIconDrag, \a lst contains the list of
    the dropped items. You can get the data using
    TQIconDragItem::data() on each item. If the \a lst is empty, i.e.
    the drag was not a TQIconDrag, you have to decode the data in \a e
    and work with that.

    Note TQIconViewItems may be drop targets; if a drop event occurs on
    an item the item handles the drop.
*/

/*!
    \fn void TQIconView::moved()

    This signal is emitted after successfully dropping one (or more)
    items of the icon view. If the items should be removed, it's best
    to do so in a slot connected to this signal.
*/

/*!
    \fn void  TQIconView::doubleClicked(TQIconViewItem * item)

    This signal is emitted when the user double-clicks on \a item.
*/

/*!
    \fn void  TQIconView::returnPressed (TQIconViewItem * item)

    This signal is emitted if the user presses the Return or Enter
    key. \a item is the currentItem() at the time of the keypress.
*/

/*!
    \fn void  TQIconView::selectionChanged()

    This signal is emitted when the selection has been changed. It's
    emitted in each selection mode.
*/

/*!
    \overload void TQIconView::selectionChanged( TQIconViewItem *item )

    This signal is emitted when the selection changes. \a item is the
    newly selected item. This signal is emitted only in single
    selection mode.
*/

/*!
    \fn void TQIconView::currentChanged( TQIconViewItem *item )

    This signal is emitted when a new item becomes current. \a item is
    the new current item (or 0 if no item is now current).

    \sa currentItem()
*/

/*!
    \fn void  TQIconView::onItem( TQIconViewItem *item )

    This signal is emitted when the user moves the mouse cursor onto
    an \a item, similar to the TQWidget::enterEvent() function.
*/

// ### bug here - enter/leave event aren't considered. move the mouse
// out of the window and back in, to the same item.

/*!
    \fn void TQIconView::onViewport()

    This signal is emitted when the user moves the mouse cursor from
    an item to an empty part of the icon view.

    \sa onItem()
*/

/*!
    \overload void TQIconView::itemRenamed (TQIconViewItem * item)

    This signal is emitted when \a item has been renamed, usually by
    in-place renaming.

    \sa TQIconViewItem::setRenameEnabled() TQIconViewItem::rename()
*/

/*!
    \fn void TQIconView::itemRenamed (TQIconViewItem * item, const TQString &name)

    This signal is emitted when \a item has been renamed to \a name,
    usually by in-place renaming.

    \sa TQIconViewItem::setRenameEnabled() TQIconViewItem::rename()
*/

/*!
    \fn void TQIconView::rightButtonClicked (TQIconViewItem * item, const TQPoint & pos)

    This signal is emitted when the user clicks the right mouse
    button. If \a item is non-null, the cursor is on \a item. If \a
    item is null, the mouse cursor isn't on any item.

    \a pos is the position of the mouse cursor in the global
    coordinate system (TQMouseEvent::globalPos()). (If the click's
    press and release differ by a pixel or two, \a pos is the
    position at release time.)

    \sa rightButtonPressed() mouseButtonClicked() clicked()
*/

/*!
    \fn void TQIconView::contextMenuRequested( TQIconViewItem *item, const TQPoint & pos )

    This signal is emitted when the user invokes a context menu with
    the right mouse button or with special system keys, with \a item
    being the item under the mouse cursor or the current item,
    respectively.

    \a pos is the position for the context menu in the global
    coordinate system.
*/

/*!
    \fn void TQIconView::mouseButtonPressed (int button, TQIconViewItem * item, const TQPoint & pos)

    This signal is emitted when the user presses mouse button \a
    button. If \a item is non-null, the cursor is on \a item. If \a
    item is null, the mouse cursor isn't on any item.

    \a pos is the position of the mouse cursor in the global
    coordinate system (TQMouseEvent::globalPos()).

    \sa rightButtonClicked() mouseButtonPressed() pressed()
*/

/*!
    \fn void TQIconView::mouseButtonClicked (int button, TQIconViewItem * item, const TQPoint & pos )

    This signal is emitted when the user clicks mouse button \a
    button. If \a item is non-null, the cursor is on \a item. If \a
    item is null, the mouse cursor isn't on any item.

    \a pos is the position of the mouse cursor in the global
    coordinate system (TQMouseEvent::globalPos()). (If the click's
    press and release differ by a pixel or two, \a pos is the
    position at release time.)

    \sa mouseButtonPressed() rightButtonClicked() clicked()
*/

/*!
    \overload void TQIconView::clicked ( TQIconViewItem * item, const TQPoint & pos )

    This signal is emitted when the user clicks any mouse button on an
    icon view item. \a item is a pointer to the item that has been
    clicked.

    \a pos is the position of the mouse cursor in the global coordinate
    system (TQMouseEvent::globalPos()). (If the click's press and release
    differ by a pixel or two, \a pos is the  position at release time.)

    \sa mouseButtonClicked() rightButtonClicked() pressed()
*/

/*!
    \overload void TQIconView::pressed ( TQIconViewItem * item, const TQPoint & pos )

    This signal is emitted when the user presses any mouse button. If
    \a item is non-null, the cursor is on \a item. If \a item is null,
    the mouse cursor isn't on any item.

    \a pos is the position of the mouse cursor in the global
    coordinate system (TQMouseEvent::globalPos()). (If the click's
    press and release differ by a pixel or two, \a pos is the
    position at release time.)

    \sa mouseButtonPressed() rightButtonPressed() clicked()
*/

/*!
    \fn void TQIconView::clicked ( TQIconViewItem * item )

    This signal is emitted when the user clicks any mouse button. If
    \a item is non-null, the cursor is on \a item. If \a item is null,
    the mouse cursor isn't on any item.

    \sa mouseButtonClicked() rightButtonClicked() pressed()
*/

/*!
    \fn void TQIconView::pressed ( TQIconViewItem * item )

    This signal is emitted when the user presses any mouse button. If
    \a item is non-null, the cursor is on \a item. If \a item is null,
    the mouse cursor isn't on any item.

    \sa mouseButtonPressed() rightButtonPressed() clicked()
*/

/*!
    \fn void TQIconView::rightButtonPressed( TQIconViewItem * item, const TQPoint & pos )

    This signal is emitted when the user presses the right mouse
    button. If \a item is non-null, the cursor is on \a item. If \a
    item is null, the mouse cursor isn't on any item.

    \a pos is the position of the mouse cursor in the global
    coordinate system (TQMouseEvent::globalPos()).
*/

/*!
    Constructs an empty icon view called \a name, with parent \a
    parent and using the widget flags \a f.
*/

TQIconView::TQIconView( TQWidget *parent, const char *name, WFlags f )
    : TQScrollView( parent, name, WStaticContents | WNoAutoErase  | f )
{
    if ( !unknown_icon ) {
	unknown_icon = new TQPixmap( (const char **)unknown_xpm );
	qiv_cleanup_pixmap.add( &unknown_icon );
    }

    d = new TQIconViewPrivate;
    d->dragging = false;
    d->firstItem = 0;
    d->lastItem = 0;
    d->count = 0;
    d->mousePressed = false;
    d->selectionMode = Single;
    d->currentItem = 0;
    d->highlightedItem = 0;
    d->rubber = 0;
    d->canStartRubber = false;
    d->backBuffer = 0;
    d->scrollTimer = 0;
    d->startDragItem = 0;
    d->tmpCurrentItem = 0;
    d->rastX = d->rastY = -1;
    d->spacing = 5;
    d->cleared = false;
    d->arrangement = LeftToRight;
    d->resizeMode = Fixed;
    d->dropped = false;
    d->adjustTimer = new TQTimer( this, "iconview adjust timer" );
    d->isIconDrag = false;
    d->inMenuMode = false;
#ifndef TQT_NO_DRAGANDDROP
    d->iconDragData.clear();
#endif
    d->numDragItems = 0;
    d->updateTimer = new TQTimer( this, "iconview update timer" );
    d->cachedW = d->cachedH = 0;
    d->maxItemWidth = 100;
    d->maxItemTextLength = 255;
    d->inputTimer = new TQTimer( this, "iconview input timer" );
    d->currInputString = TQString::null;
    d->dirty = false;
    d->rearrangeEnabled = true;
    d->itemTextPos = Bottom;
    d->reorderItemsWhenInsert = true;
#ifndef TQT_NO_CURSOR
    d->oldCursor = arrowCursor;
#endif
    d->resortItemsWhenInsert = false;
    d->sortDirection = true;
    d->wordWrapIconText = true;
    d->cachedContentsX = d->cachedContentsY = -1;
    d->clearing = false;
    d->fullRedrawTimer = new TQTimer( this, "iconview full redraw timer" );
    d->itemTextBrush = NoBrush;
    d->drawAllBack = true;
    d->fm = new TQFontMetrics( font() );
    d->minLeftBearing = d->fm->minLeftBearing();
    d->minRightBearing = d->fm->minRightBearing();
    d->firstContainer = d->lastContainer = 0;
    d->containerUpdateLocked = false;
    d->firstSizeHint = false;
    d->selectAnchor = 0;
    d->renamingItem = 0;
    d->drawActiveSelection = true;
    d->drawDragShapes = false;
    d->backrubber = 0;    

    connect( d->adjustTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( adjustItems() ) );
    connect( d->updateTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( slotUpdate() ) );
    connect( d->fullRedrawTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( updateContents() ) );
    connect( this, TQ_SIGNAL( contentsMoving(int,int) ),
	     this, TQ_SLOT( movedContents(int,int) ) );

    setAcceptDrops( true );
    viewport()->setAcceptDrops( true );

    setMouseTracking( true );
    viewport()->setMouseTracking( true );

    viewport()->setBackgroundMode( PaletteBase);
    setBackgroundMode( PaletteBackground, PaletteBase );
    viewport()->setFocusProxy( this );
    viewport()->setFocusPolicy( TQWidget::WheelFocus );

#ifndef TQT_NO_TOOLTIP
    d->toolTip = new TQIconViewToolTip( viewport(), this );
#endif
    d->showTips = true;
}

/*!
    \reimp
*/

void TQIconView::styleChange( TQStyle& old )
{
    TQScrollView::styleChange( old );
    *d->fm = TQFontMetrics( font() );
    d->minLeftBearing = d->fm->minLeftBearing();
    d->minRightBearing = d->fm->minRightBearing();

    TQIconViewItem *item = d->firstItem;
    for ( ; item; item = item->next ) {
	item->wordWrapDirty = true;
	item->calcRect();
    }

#if !defined(TQ_WS_X11)
    delete qiv_selection;
    qiv_selection = 0;
#endif
}

/*!
    \reimp
*/

void TQIconView::setFont( const TQFont & f )
{
    TQScrollView::setFont( f );
    *d->fm = TQFontMetrics( font() );
    d->minLeftBearing = d->fm->minLeftBearing();
    d->minRightBearing = d->fm->minRightBearing();

    TQIconViewItem *item = d->firstItem;
    for ( ; item; item = item->next ) {
	item->wordWrapDirty = true;
	item->calcRect();
    }
}

/*!
    \reimp
*/

void TQIconView::setPalette( const TQPalette & p )
{
    TQScrollView::setPalette( p );
    *d->fm = TQFontMetrics( font() );
    d->minLeftBearing = d->fm->minLeftBearing();
    d->minRightBearing = d->fm->minRightBearing();

    TQIconViewItem *item = d->firstItem;
    for ( ; item; item = item->next ) {
	item->wordWrapDirty = true;
	item->calcRect();
    }
}

/*!
    Destroys the icon view and deletes all items.
*/

TQIconView::~TQIconView()
{
    TQIconViewItem *tmp, *item = d->firstItem;
    d->clearing = true;
    TQIconViewPrivate::ItemContainer *c = d->firstContainer, *tmpc;
    while ( c ) {
	tmpc = c->n;
	delete c;
	c = tmpc;
    }
    while ( item ) {
	tmp = item->next;
	delete item;
	item = tmp;
    }
    delete d->backBuffer;
    d->backBuffer = 0;
    delete d->fm;
    d->fm = 0;
#ifndef TQT_NO_TOOLTIP
    delete d->toolTip;
    d->toolTip = 0;
#endif
    delete d;
    d = NULL;
}

/*!
    Inserts the icon view item \a item after \a after. If \a after is
    0, \a item is appended after the last item.

    \e{You should never need to call this function.} Instead create
    TQIconViewItem's and associate them with your icon view like this:

    \code
	(void) new TQIconViewItem( myIconview, "The text of the item", aPixmap );
    \endcode
*/

void TQIconView::insertItem( TQIconViewItem *item, TQIconViewItem *after )
{
    if ( !item )
	return;

    if ( d->firstItem == item || item->prev || item->next)
	return;

    if ( !item->view )
	item->view = this;

    if ( !d->firstItem ) {
	d->firstItem = d->lastItem = item;
	item->prev = 0;
	item->next = 0;
    } else {
	if ( !after || after == d->lastItem ) {
	    d->lastItem->next = item;
	    item->prev = d->lastItem;
	    item->next = 0;
	    d->lastItem = item;
	} else {
	    TQIconViewItem *i = d->firstItem;
	    while ( i != after )
		i = i->next;

	    if ( i ) {
		TQIconViewItem *next = i->next;
		item->next = next;
		item->prev = i;
		i->next = item;
		next->prev = item;
	    }
	}
    }

    if ( isVisible() ) {
	if ( d->reorderItemsWhenInsert ) {
	    if ( d->updateTimer->isActive() )
		d->updateTimer->stop();
	    d->fullRedrawTimer->stop();
	    // #### uncomment this ASA insertInGrid uses cached values and is efficient
	    //insertInGrid( item );

	    d->cachedW = TQMAX( d->cachedW, item->x() + item->width() );
	    d->cachedH = TQMAX( d->cachedH, item->y() + item->height() );

	    d->updateTimer->start( 0, true );
	} else {
	    insertInGrid( item );

	    viewport()->update(item->x() - contentsX(),
			       item->y() - contentsY(),
			       item->width(), item->height());
	}
    } else if ( !autoArrange() ) {
	item->dirty = false;
    }

    d->count++;
    d->dirty = true;
}

/*!
    This slot is used for a slightly-delayed update.

    The icon view is not redrawn immediately after inserting a new item
    but after a very small delay using a TQTimer. This means that when
    many items are inserted in a loop the icon view is probably redrawn
    only once at the end of the loop. This makes the insertions both
    flicker-free and faster.
*/

void TQIconView::slotUpdate()
{
    d->updateTimer->stop();
    d->fullRedrawTimer->stop();

    if ( !d->firstItem || !d->lastItem )
	return;

    // #### remove that ASA insertInGrid uses cached values and is efficient
    if ( d->resortItemsWhenInsert )
	sort( d->sortDirection );
    else {
	int y = d->spacing;
	TQIconViewItem *item = d->firstItem;
	int w = 0, h = 0;
	while ( item ) {
	    bool changed;
	    TQIconViewItem *next = makeRowLayout( item, y, changed );
	    if ( !next || !next->next )
		break;

	    if( !TQApplication::reverseLayout() )
		item = next;
	    w = TQMAX( w, item->x() + item->width() );
	    h = TQMAX( h, item->y() + item->height() );
	    item = next;
	    if ( d->arrangement == LeftToRight )
		h = TQMAX( h, y );

	    item = item->next;
	}

	if ( d->lastItem && d->arrangement == TopToBottom ) {
	    item = d->lastItem;
	    int x = item->x();
	    while ( item && item->x() >= x ) {
		w = TQMAX( w, item->x() + item->width() );
		h = TQMAX( h, item->y() + item->height() );
		item = item->prev;
	    }
	}

	w = TQMAX( TQMAX( d->cachedW, w ), d->lastItem->x() + d->lastItem->width() );
	h = TQMAX( TQMAX( d->cachedH, h ), d->lastItem->y() + d->lastItem->height() );

	if ( d->arrangement == TopToBottom )
	    w += d->spacing;
	else
	    h += d->spacing;
	viewport()->setUpdatesEnabled( false );
	resizeContents( w, h );
	viewport()->setUpdatesEnabled( true );
	viewport()->repaint( false );
    }

    int cx = d->cachedContentsX == -1 ? contentsX() : d->cachedContentsX;
    int cy = d->cachedContentsY == -1 ? contentsY() : d->cachedContentsY;

    if ( cx != contentsX() || cy != contentsY() )
	setContentsPos( cx, cy );

    d->cachedContentsX = d->cachedContentsY = -1;
    d->cachedW = d->cachedH = 0;
}

/*!
    Takes the icon view item \a item out of the icon view and causes
    an update of the screen display. The item is not deleted. You
    should normally not need to call this function because
    TQIconViewItem::~TQIconViewItem() calls it. The normal way to delete
    an item is to delete it.
*/

void TQIconView::takeItem( TQIconViewItem *item )
{
    if ( !item )
	return;

    if ( item->d->container1 )
	item->d->container1->items.removeRef( item );
    if ( item->d->container2 )
	item->d->container2->items.removeRef( item );
    item->d->container2 = 0;
    item->d->container1 = 0;

    bool block = signalsBlocked();
    blockSignals( d->clearing );

    TQRect r = item->rect();

    if ( d->currentItem == item ) {
	if ( item->prev ) {
	    d->currentItem = item->prev;
	    emit currentChanged( d->currentItem );
	    repaintItem( d->currentItem );
	} else if ( item->next ) {
	    d->currentItem = item->next;
	    emit currentChanged( d->currentItem );
	    repaintItem( d->currentItem );
	} else {
	    d->currentItem = 0;
	    emit currentChanged( d->currentItem );
	}
    }
    if ( item->isSelected() ) {
	item->selected = false;
	emit selectionChanged();
    }

    if ( item == d->firstItem ) {
	d->firstItem = d->firstItem->next;
	if ( d->firstItem )
	    d->firstItem->prev = 0;
    } else if ( item == d->lastItem ) {
	d->lastItem = d->lastItem->prev;
	if ( d->lastItem )
	    d->lastItem->next = 0;
    } else {
	TQIconViewItem *i = item;
	if ( i ) {
	    if ( i->prev )
		i->prev->next = i->next;
	    if ( i->next )
		i->next->prev = i->prev;
	}
    }

    if ( d->selectAnchor == item )
	d->selectAnchor = d->currentItem;

    if ( !d->clearing )
	repaintContents( r.x(), r.y(), r.width(), r.height(), true );

    item->view = 0;
    item->prev = 0;
    item->next = 0;
    d->count--;

    blockSignals( block );
}

/*!
    Returns the index of \a item, or -1 if \a item doesn't exist in
    this icon view.
*/

int TQIconView::index( const TQIconViewItem *item ) const
{
    if ( !item )
	return -1;

    if ( item == d->firstItem )
	return 0;
    else if ( item == d->lastItem )
	return d->count - 1;
    else {
	TQIconViewItem *i = d->firstItem;
	int j = 0;
	while ( i && i != item ) {
	    i = i->next;
	    ++j;
	}

	return i ? j : -1;
    }
}

/*!
    Returns a pointer to the first item of the icon view, or 0 if
    there are no items in the icon view.

    \sa lastItem() currentItem()
*/

TQIconViewItem *TQIconView::firstItem() const
{
    if (d) {
        return d->firstItem;
    }
    else {
        return NULL;
    }
}

/*!
    Returns a pointer to the last item of the icon view, or 0 if there
    are no items in the icon view.

    \sa firstItem() currentItem()
*/

TQIconViewItem *TQIconView::lastItem() const
{
    if (d) {
        return d->lastItem;
    }
    else {
        return NULL;
    }
}

/*!
    Returns a pointer to the current item of the icon view, or 0 if no
    item is current.

    \sa setCurrentItem() firstItem() lastItem()
*/

TQIconViewItem *TQIconView::currentItem() const
{
    if (d) {
        return d->currentItem;
    }
    else {
        return NULL;
    }
}

/*!
    Makes \a item the new current item of the icon view.
*/

void TQIconView::setCurrentItem( TQIconViewItem *item )
{
    if ( !item || item == d->currentItem )
	return;

    setMicroFocusHint( item->x(), item->y(), item->width(), item->height(), false );

    TQIconViewItem *old = d->currentItem;
    d->currentItem = item;
    emit currentChanged( d->currentItem );
    if ( d->selectionMode == Single ) {
	bool changed = false;
	if ( old && old->selected ) {
	    old->selected = false;
	    changed = true;
	}
	if ( item && !item->selected && item->isSelectable() && d->selectionMode != NoSelection ) {
	    item->selected = true;
	    changed = true;
	    emit selectionChanged( item );
	}
	if ( changed )
	    emit selectionChanged();
    }

    if ( old )
	repaintItem( old );
    repaintItem( d->currentItem );
}

/*!
    Selects or unselects \a item depending on \a s, and may also
    unselect other items, depending on TQIconView::selectionMode() and
    \a cb.

    If \a s is false, \a item is unselected.

    If \a s is true and TQIconView::selectionMode() is \c Single, \a
    item is selected, and the item which was selected is unselected.

    If \a s is true and TQIconView::selectionMode() is \c Extended, \a
    item is selected. If \a cb is true, the selection state of the
    icon view's other items is left unchanged. If \a cb is false (the
    default) all other items are unselected.

    If \a s is true and TQIconView::selectionMode() is \c Multi \a item
    is selected.

    Note that \a cb is used only if TQIconView::selectionMode() is \c
    Extended. \a cb defaults to false.

    All items whose selection status is changed repaint themselves.
*/

void TQIconView::setSelected( TQIconViewItem *item, bool s, bool cb )
{
    if ( !item )
	return;
    item->setSelected( s, cb );
}

/*!
    \property TQIconView::count
    \brief the number of items in the icon view
*/

uint TQIconView::count() const
{
    return d->count;
}

/*!
    Performs autoscrolling when selecting multiple icons with the
    rubber band.
*/

void TQIconView::doAutoScroll()
{
    TQRect oldRubber = *d->rubber;

    TQPoint vp = viewport()->mapFromGlobal( TQCursor::pos() );
    TQPoint pos = viewportToContents( vp );

    if ( pos == d->rubber->bottomRight() )
	return;

    d->rubber->setRight( pos.x() );
    d->rubber->setBottom( pos.y() );

    int minx = contentsWidth(), miny = contentsHeight();
    int maxx = 0, maxy = 0;
    bool changed = false;
    bool block = signalsBlocked();

    TQRect rr;

    blockSignals( true );
    viewport()->setUpdatesEnabled( false );
    bool alreadyIntersected = false;
    TQRect nr = d->rubber->normalize();
    TQRect rubberUnion = nr.unite( oldRubber.normalize() );
    TQIconViewPrivate::ItemContainer *c = d->firstContainer;
    for ( ; c; c = c->n ) {
	if ( c->rect.intersects( rubberUnion ) ) {
	    alreadyIntersected = true;
	    TQIconViewItem *item = c->items.first();
	    for ( ; item; item = c->items.next() ) {
		if ( d->selectedItems.find( item ) )
		    continue;
		if ( !item->intersects( nr ) ) {
		    if ( item->isSelected() ) {
			item->setSelected( false );
			changed = true;
			rr = rr.unite( item->rect() );
		    }
		} else if ( item->intersects( nr ) ) {
		    if ( !item->isSelected() && item->isSelectable() ) {
			item->setSelected( true, true );
			changed = true;
			rr = rr.unite( item->rect() );
		    }

		    minx = TQMIN( minx, item->x() - 1 );
		    miny = TQMIN( miny, item->y() - 1 );
		    maxx = TQMAX( maxx, item->x() + item->width() + 1 );
		    maxy = TQMAX( maxy, item->y() + item->height() + 1 );
		}
	    }
	} else {
	    if ( alreadyIntersected )
		break;
	}
    }
    viewport()->setUpdatesEnabled( true );
    blockSignals( block );

    // static bool drawAll;
    if ( d->backrubber == 0 ) {
        d->backrubber = new TQPixmap( viewport()->rect().size() );
        d->backrubber->fill( viewport(), viewport()->rect().topLeft() );
        // drawAll = true;
    }

    // int oldX = 0, oldY = 0;
    // if ( !drawAll && d->scrollTimer ) {
    //     oldX = contentsX();
    //     oldY = contentsY();
    // }
    ensureVisible( pos.x(), pos.y() );
    // if ( !drawAll && d->scrollTimer && ( oldX != contentsX() || oldY != contentsY() ) )
    //     drawAll = true;

    TQRect allRect = oldRubber.normalize();
    if ( changed ) 
        allRect |= rr.normalize();
    allRect |= d->rubber->normalize();
    TQPoint point = contentsToViewport( allRect.topLeft() );
    allRect = TQRect( point.x(), point.y(), allRect.width(), allRect.height() );
    allRect &= viewport()->rect();

    d->dragging = false;

    TQPainter p( d->backrubber );
    p.translate( -contentsX(), -contentsY() );
#if 0
    if ( !drawAll ) {
        oldRubber = oldRubber.normalize();
        point = contentsToViewport( oldRubber.topLeft() );
        oldRubber = TQRect( point.x(), point.y(), oldRubber.width(), oldRubber.height() );
        oldRubber &= viewport()->rect();

        point = contentsToViewport( nr.topLeft() );
        nr = TQRect( point.x(), point.y(), nr.width(), nr.height() );
        nr &= viewport()->rect();

	TQRegion region;
	if ( allRect != nr )
            region = TQRegion(allRect).subtract( TQRegion( nr ) );
	if ( allRect != oldRubber )
            region += TQRegion(allRect).subtract( TQRegion( oldRubber ) );

	TQMemArray< TQRect > ar = region.rects();
	for ( uint i = 0; i < ar.size(); ++i ) {
             ar[i].addCoords( -2, -2, 4, 4 );
             ar[i] = ar[i].normalize();
	
             p.setClipRect( ar[i] );
             drawContents( &p, contentsX() + ar[i].left(), contentsY() + ar[i].top(), ar[i].width(), ar[i].height() );
	}
    }
    else
#endif
    {	    
        drawContents( &p, 
                      contentsX() + allRect.left(), contentsY() + allRect.top(), 
                      allRect.width(), allRect.height() ); 
    }

    p.end();
    // drawAll = false;
    d->dragging = true;
    bitBlt( viewport(), allRect.topLeft(), d->backrubber, allRect );

    if ( changed ) {
	emit selectionChanged();
        if ( d->selectionMode == Single )
            emit selectionChanged( d->currentItem );
    }

    if ( !TQRect( 50, 50, viewport()->width()-100, viewport()->height()-100 ).contains( vp ) &&
	 !d->scrollTimer ) {
	d->scrollTimer = new TQTimer( this );

	connect( d->scrollTimer, TQ_SIGNAL( timeout() ),
		 this, TQ_SLOT( doAutoScroll() ) );
	d->scrollTimer->start( 100, false );
    } else if ( TQRect( 50, 50, viewport()->width()-100, viewport()->height()-100 ).contains( vp ) &&
		d->scrollTimer ) {
	disconnect( d->scrollTimer, TQ_SIGNAL( timeout() ),
		    this, TQ_SLOT( doAutoScroll() ) );
	d->scrollTimer->stop();
	delete d->scrollTimer;
	d->scrollTimer = 0;
    }

}

/*!
    \reimp
*/

void TQIconView::drawContents( TQPainter *p, int cx, int cy, int cw, int ch )
{
    p->save();
    TQRect r = TQRect( cx, cy, cw, ch );

    TQIconViewPrivate::ItemContainer *c = d->firstContainer;
    TQRegion remaining( TQRect( cx, cy, cw, ch ) );
    bool alreadyIntersected = false;
    while ( c ) {
	if ( c->rect.intersects( r ) ) {
	    p->save();
	    p->resetXForm();
	    TQRect r2 = c->rect;
	    r2 = r2.intersect( r );
	    TQRect r3( contentsToViewport( TQPoint( r2.x(), r2.y() ) ), TQSize( r2.width(), r2.height() ) );
	    if ( d->drawAllBack ) {
		p->setClipRect( r3 );
	    } else {
		TQRegion reg = d->clipRegion.intersect( r3 );
		p->setClipRegion( reg );
	    }
	    drawBackground( p, r3 );
	    remaining = remaining.subtract( r3 );
	    p->restore();

	    TQColorGroup cg;
	    d->drawActiveSelection = hasFocus() || d->inMenuMode
		|| !style().styleHint( TQStyle::SH_ItemView_ChangeHighlightOnFocus, this );

	    if ( !d->drawActiveSelection )
		cg = palette().inactive();
	    else
		cg = colorGroup();

	    TQIconViewItem *item = c->items.first();
	    // clip items to the container rect by default... this
	    // prevents icons with alpha channels from being painted
	    // twice when they are in 2 containers
	    //
	    // NOTE: the item could override this cliprect in it's
	    // paintItem() implementation, which makes this useless
	    p->setClipRect( TQRect( contentsToViewport( r2.topLeft() ), r2.size() ) );
	    for ( ; item; item = c->items.next() ) {
		if ( item->rect().intersects( r ) && !item->dirty ) {
		    p->save();
		    p->setFont( font() );
		    item->paintItem( p, cg );
		    p->restore();
		}
	    }
	    alreadyIntersected = true;
	} else {
	    if ( alreadyIntersected )
		break;
	}
	c = c->n;
    }

    if ( !remaining.isNull() && !remaining.isEmpty() ) {
	p->save();
	p->resetXForm();
	if ( d->drawAllBack ) {
	    p->setClipRegion( remaining );
	} else {
	    remaining = d->clipRegion.intersect( remaining );
	    p->setClipRegion( remaining );
	}
	drawBackground( p, remaining.boundingRect() );
	p->restore();
    }

    if ( ( hasFocus() || viewport()->hasFocus() ) && d->currentItem &&
	 d->currentItem->rect().intersects( r ) ) {
	d->currentItem->paintFocus( p, colorGroup() );
    }

   p->restore();
   if ( d->rubber ) {
       p->save();              
       p->translate( contentsX(), contentsY() );
       p->setRasterOp( NotROP );
       p->setPen( TQPen( color0, 1 ) );
       p->setBrush( NoBrush );        
       drawRubber( p );
       p->restore();
    }
}

/*!
    \overload

    Arranges all the items in the grid given by gridX() and gridY().

    Even if sorting() is enabled, the items are not sorted by this
    function. If you want to sort or rearrange the items, use
    iconview->sort(iconview->sortDirection()).

    If \a update is true (the default), the viewport is repainted as
    well.

    \sa TQIconView::setGridX(), TQIconView::setGridY(), TQIconView::sort()
*/

void TQIconView::arrangeItemsInGrid( bool update )
{
    if ( !d->firstItem || !d->lastItem )
	return;

    d->containerUpdateLocked = true;

    int w = 0, h = 0, y = d->spacing;

    TQIconViewItem *item = d->firstItem;
    while (item && (item->isVisible() == false)) {
	item = item->next;
    }
    bool changedLayout = false;
    while ( item ) {
	bool changed;
	TQIconViewItem *next = makeRowLayout( item, y, changed );
	changedLayout = changed || changedLayout;
	if( !TQApplication::reverseLayout() )
	    item = next;
	w = TQMAX( w, item->x() + item->width() );
	h = TQMAX( h, item->y() + item->height() );
	item = next;
	if ( d->arrangement == LeftToRight )
	    h = TQMAX( h, y );

	if ( !item || !item->next )
	    break;

	item = item->next;
	while (item && (item->isVisible() == false)) {
	    item = item->next;
	}
    }

    if ( d->lastItem && d->arrangement == TopToBottom ) {
	item = d->lastItem;
	while (item && (item->isVisible() == false)) {
	    item = item->prev;
	}
	int x = 0;
	if (item)
	    x = item->x();
	while ( item && item->x() >= x ) {
	    w = TQMAX( w, item->x() + item->width() );
	    h = TQMAX( h, item->y() + item->height() );
	    item = item->prev;
	    while (item && (item->isVisible() == false)) {
		item = item->prev;
	    }
	}
    }
    d->containerUpdateLocked = false;

    TQIconViewItem *lastVisibleItem = d->lastItem;
    while (lastVisibleItem->prev && (lastVisibleItem->isVisible() == false)) {
	lastVisibleItem = lastVisibleItem->prev;
    }

    w = TQMAX( TQMAX( d->cachedW, w ), lastVisibleItem->x() + lastVisibleItem->width() );
    h = TQMAX( TQMAX( d->cachedH, h ), lastVisibleItem->y() + lastVisibleItem->height() );

    if ( d->arrangement == TopToBottom )
	w += d->spacing;
    else
	h += d->spacing;

    bool ue = isUpdatesEnabled();
    viewport()->setUpdatesEnabled( false );
    int vw = visibleWidth();
    int vh = visibleHeight();
    resizeContents( w, h );
    bool doAgain = false;
    if ( d->arrangement == LeftToRight )
	doAgain = visibleWidth() != vw;
    if ( d->arrangement == TopToBottom )
	doAgain = visibleHeight() != vh;
    if ( doAgain ) // in the case that the visibleExtend changed because of the resizeContents (scrollbar show/hide), redo layout again
	arrangeItemsInGrid( false );
    viewport()->setUpdatesEnabled( ue );
    d->dirty = !isVisible();
    rebuildContainers();
    if ( update && ( !optimize_layout || changedLayout ) )
	repaintContents( contentsX(), contentsY(), viewport()->width(), viewport()->height(), false );
}

/*!
    This variant uses \a grid instead of (gridX(), gridY()). If \a
    grid is invalid (see TQSize::isValid()), arrangeItemsInGrid()
    calculates a valid grid itself and uses that.

    If \a update is true (the default) the viewport is repainted.
*/

void TQIconView::arrangeItemsInGrid( const TQSize &grid, bool update )
{
    d->containerUpdateLocked = true;
    TQSize grid_( grid );
    if ( !grid_.isValid() ) {
	int w = 0, h = 0;
	TQIconViewItem *item = d->firstItem;
	for ( ; item; item = item->next ) {
	    w = TQMAX( w, item->width() );
	    h = TQMAX( h, item->height() );
	}

	grid_ = TQSize( TQMAX( d->rastX + d->spacing, w ),
		       TQMAX( d->rastY + d->spacing, h ) );
    }

    int w = 0, h = 0;
    TQIconViewItem *item = d->firstItem;
    for ( ; item; item = item->next ) {
	int nx = item->x() / grid_.width();
	int ny = item->y() / grid_.height();
	item->move( nx * grid_.width(),
		    ny * grid_.height() );
	w = TQMAX( w, item->x() + item->width() );
	h = TQMAX( h, item->y() + item->height() );
	item->dirty = false;
    }
    d->containerUpdateLocked = false;

    resizeContents( w, h );
    rebuildContainers();
    if ( update )
	repaintContents( contentsX(), contentsY(), viewport()->width(), viewport()->height(), false );
}

/*!
    \reimp
*/

void TQIconView::setContentsPos( int x, int y )
{
    if ( d->updateTimer->isActive() ) {
	d->cachedContentsX = x;
	d->cachedContentsY = y;
    } else {
	d->cachedContentsY = d->cachedContentsX = -1;
	TQScrollView::setContentsPos( x, y );
    }
}

/*!
    \reimp
*/

void TQIconView::showEvent( TQShowEvent * )
{
    if ( d->dirty ) {
	resizeContents( TQMAX( contentsWidth(), viewport()->width() ),
			TQMAX( contentsHeight(), viewport()->height() ) );
	if ( d->resortItemsWhenInsert )
	    sort( d->sortDirection );
	if ( autoArrange() )
	    arrangeItemsInGrid( false );
    }
    TQScrollView::show();
}

/*!
    \property TQIconView::selectionMode
    \brief the selection mode of the icon view

    This can be \c Single (the default), \c Extended, \c Multi or \c
    NoSelection.
*/

void TQIconView::setSelectionMode( SelectionMode m )
{
    d->selectionMode = m;
}

TQIconView::SelectionMode TQIconView::selectionMode() const
{
    return d->selectionMode;
}

/*!
    Returns a pointer to the item that contains point \a pos, which is
    given in contents coordinates, or 0 if no item contains point \a
    pos.
*/

TQIconViewItem *TQIconView::findItem( const TQPoint &pos ) const
{
    if ( !d->firstItem )
	return 0;

    TQIconViewPrivate::ItemContainer *c = d->lastContainer;
    for ( ; c; c = c->p ) {
	if ( c->rect.contains( pos ) ) {
	    TQIconViewItem *item = c->items.last();
	    for ( ; item; item = c->items.prev() )
		if ( item->contains( pos ) )
		    return item;
	}
    }

    return 0;
}

/*!
    \overload

    Returns a pointer to the first item whose text begins with \a
    text, or 0 if no such item could be found. Use the \a compare flag
    to control the comparison behaviour. (See \l
    {TQt::StringComparisonMode}.)
*/

TQIconViewItem *TQIconView::findItem( const TQString &text, ComparisonFlags compare ) const
{
    if ( !d->firstItem )
	return 0;

    if ( compare == CaseSensitive || compare == 0 )
	compare |= ExactMatch;

    TQString itmtxt;
    TQString comtxt = text;
    if ( ! (compare & CaseSensitive) )
	comtxt = text.lower();

    TQIconViewItem *item;
    if ( d->currentItem )
	item = d->currentItem;
    else
	item = d->firstItem;

    TQIconViewItem *beginsWithItem = 0;
    TQIconViewItem *endsWithItem = 0;
    TQIconViewItem *containsItem = 0;

    if ( item ) {
	for ( ; item; item = item->next ) {
	    if ( ! (compare & CaseSensitive) )
		itmtxt = item->text().lower();
	    else
		itmtxt = item->text();

	    if ( compare & ExactMatch && itmtxt == comtxt )
		return item;
	    if ( compare & BeginsWith && !beginsWithItem && itmtxt.startsWith( comtxt ) )
		beginsWithItem = containsItem = item;
	    if ( compare & EndsWith && !endsWithItem && itmtxt.endsWith( comtxt ) )
		endsWithItem = containsItem = item;
	    if ( compare & Contains && !containsItem && itmtxt.contains( comtxt ) )
		containsItem = item;
	}

	if ( d->currentItem && d->firstItem ) {
	    item = d->firstItem;
	    for ( ; item && item != d->currentItem; item = item->next ) {
		if ( ! (compare & CaseSensitive) )
		    itmtxt = item->text().lower();
		else
		    itmtxt = item->text();

		if ( compare & ExactMatch && itmtxt == comtxt )
		    return item;
		if ( compare & BeginsWith && !beginsWithItem && itmtxt.startsWith( comtxt ) )
		    beginsWithItem = containsItem = item;
		if ( compare & EndsWith && !endsWithItem && itmtxt.endsWith( comtxt ) )
		    endsWithItem = containsItem = item;
		if ( compare & Contains && !containsItem && itmtxt.contains( comtxt ) )
		    containsItem = item;
	    }
	}
    }

    // Obey the priorities
    if ( beginsWithItem )
	return beginsWithItem;
    else if ( endsWithItem )
	return endsWithItem;
    else if ( containsItem )
	return containsItem;
    return 0;
}

/*!
    Unselects all the items.
*/

void TQIconView::clearSelection()
{
    selectAll( false );
}

/*!
    In Multi and Extended modes, this function sets all items to be
    selected if \a select is true, and to be unselected if \a select
    is false.

    In Single and NoSelection modes, this function only changes the
    selection status of currentItem().
*/

void TQIconView::selectAll( bool select )
{
    if ( d->selectionMode == NoSelection )
	return;

    if ( d->selectionMode == Single ) {
	if ( d->currentItem )
	    d->currentItem->setSelected( select );
	return;
    }

    bool b = signalsBlocked();
    blockSignals( true );
    TQIconViewItem *item = d->firstItem;
    TQIconViewItem *i = d->currentItem;
    bool changed = false;
    bool ue = viewport()->isUpdatesEnabled();
    viewport()->setUpdatesEnabled( false );
    TQRect rr;
    for ( ; item; item = item->next ) {
        if (item->isVisible()) {
	    if ( select != item->isSelected() ) {
	        item->setSelected( select, true );
	        rr = rr.unite( item->rect() );
	        changed = true;
	    }
	}
	else {
	    if ( false != item->isSelected() ) {
	        item->setSelected( false, true );
	        changed = true;
	    }
	}
    }
    viewport()->setUpdatesEnabled( ue );
    // we call updateContents not repaintContents because of possible previous updateContents
    TQScrollView::updateContents( rr );
    TQApplication::sendPostedEvents( viewport(), TQEvent::Paint );
    if ( i )
	setCurrentItem( i );
    blockSignals( b );
    if ( changed ) {
	emit selectionChanged();
    }
}

/*!
    Inverts the selection. Works only in Multi and Extended selection
    mode.
*/

void TQIconView::invertSelection()
{
    if ( d->selectionMode == Single ||
	 d->selectionMode == NoSelection )
	return;

    bool b = signalsBlocked();
    blockSignals( true );
    TQIconViewItem *item = d->firstItem;
    for ( ; item; item = item->next ) {
        if (item->isVisible()) {
            item->setSelected( !item->isSelected(), true );
        }
        else {
            if ( false != item->isSelected() ) {
                item->setSelected( false, true );
            }
        }
    }
    blockSignals( b );
    emit selectionChanged();
}

/*!
    Repaints the \a item.
*/

void TQIconView::repaintItem( TQIconViewItem *item )
{
    if ( !item || item->dirty )
	return;

    if ( TQRect( contentsX(), contentsY(), visibleWidth(), visibleHeight() ).
	 intersects( TQRect( item->x() - 1, item->y() - 1, item->width() + 2, item->height() + 2 ) ) )
	repaintContents( item->x() - 1, item->y() - 1, item->width() + 2, item->height() + 2, false );
}

/*!
    Repaints the selected items.
*/
void TQIconView::repaintSelectedItems()
{
    if ( selectionMode() == NoSelection )
	return;

    if ( selectionMode() == Single ) {
	if ( !currentItem() || !currentItem()->isSelected() )
	    return;
	TQRect itemRect = currentItem()->rect(); //rect in contents coordinates
	itemRect.moveBy( -contentsX(), -contentsY() );
	viewport()->update( itemRect );
    } else {
	// check if any selected items are visible
	TQIconViewItem *item = firstItem();
	const TQRect vr = TQRect( contentsX(), contentsY(), visibleWidth(), visibleHeight() );

	while ( item ) {
	    if ( item->isSelected() && item->rect().intersects( vr ) )
		repaintItem( item );
	    item = item->nextItem();
	}
    }
}

/*!
    Makes sure that \a item is entirely visible. If necessary,
    ensureItemVisible() scrolls the icon view.

    \sa ensureVisible()
*/

void TQIconView::ensureItemVisible( TQIconViewItem *item )
{
    if ( !item )
	return;

    if ( (d->updateTimer && d->updateTimer->isActive()) ||
	 (d->fullRedrawTimer && d->fullRedrawTimer->isActive()) )
	slotUpdate();

    int w = item->width();
    int h = item->height();
    ensureVisible( item->x() + w / 2, item->y() + h / 2,
		   w / 2 + 1, h / 2 + 1 );
}

/*!
    Finds the first item whose bounding rectangle overlaps \a r and
    returns a pointer to that item. \a r is given in content
    coordinates. Returns 0 if no item overlaps \a r.

    If you want to find all items that touch \a r, you will need to
    use this function and nextItem() in a loop ending at
    findLastVisibleItem() and test TQIconViewItem::rect() for each of
    these items.

    \sa findLastVisibleItem() TQIconViewItem::rect()
*/

TQIconViewItem* TQIconView::findFirstVisibleItem( const TQRect &r ) const
{
    TQIconViewPrivate::ItemContainer *c = d->firstContainer;
    TQIconViewItem *i = 0;
    bool alreadyIntersected = false;
    for ( ; c; c = c->n ) {
	if ( c->rect.intersects( r ) ) {
	    alreadyIntersected = true;
	    TQIconViewItem *item = c->items.first();
	    for ( ; item; item = c->items.next() ) {
		if ( r.intersects( item->rect() ) ) {
		    if ( !i ) {
			i = item;
		    } else {
			TQRect r2 = item->rect();
			TQRect r3 = i->rect();
			if ( r2.y() < r3.y() )
			    i = item;
			else if ( r2.y() == r3.y() &&
				  r2.x() < r3.x() )
			    i = item;
		    }
		}
	    }
	} else {
	    if ( alreadyIntersected )
		break;
	}
    }

    return i;
}

/*!
    Finds the last item whose bounding rectangle overlaps \a r and
    returns a pointer to that item. \a r is given in content
    coordinates. Returns 0 if no item overlaps \a r.

    \sa findFirstVisibleItem()
*/

TQIconViewItem* TQIconView::findLastVisibleItem( const TQRect &r ) const
{
    TQIconViewPrivate::ItemContainer *c = d->firstContainer;
    TQIconViewItem *i = 0;
    bool alreadyIntersected = false;
    for ( ; c; c = c->n ) {
	if ( c->rect.intersects( r ) ) {
	    alreadyIntersected = true;
	    TQIconViewItem *item = c->items.first();
	    for ( ; item; item = c->items.next() ) {
		if ( r.intersects( item->rect() ) ) {
		    if ( !i ) {
			i = item;
		    } else {
			TQRect r2 = item->rect();
			TQRect r3 = i->rect();
			if ( r2.y() > r3.y() )
			    i = item;
			else if ( r2.y() == r3.y() &&
				  r2.x() > r3.x() )
			    i = item;
		    }
		}
	    }
	} else {
	    if ( alreadyIntersected )
		break;
	}
    }

    return i;
}

/*!
    Clears the icon view. All items are deleted.
*/

void TQIconView::clear()
{
    setContentsPos( 0, 0 );
    d->clearing = true;
    bool block = signalsBlocked();
    blockSignals( true );
    clearSelection();
    blockSignals( block );
    setContentsPos( 0, 0 );
    d->currentItem = 0;

    if ( !d->firstItem ) {
	d->clearing = false;
	return;
    }

    TQIconViewItem *item = d->firstItem, *tmp;
    d->firstItem = 0;
    while ( item ) {
	tmp = item->next;
	delete item;
	item = tmp;
    }
    TQIconViewPrivate::ItemContainer *c = d->firstContainer, *tmpc;
    while ( c ) {
	tmpc = c->n;
	delete c;
	c = tmpc;
    }
    d->firstContainer = d->lastContainer = 0;

    d->count = 0;
    d->lastItem = 0;
    setCurrentItem( 0 );
    d->highlightedItem = 0;
    d->tmpCurrentItem = 0;
    d->drawDragShapes = false;

    resizeContents( 0, 0 );
    // maybe we don�t need this update, so delay it
    d->fullRedrawTimer->start( 0, true );

    d->cleared = true;
    d->clearing = false;
}

/*!
    \property TQIconView::gridX
    \brief the horizontal grid  of the icon view

    If the value is -1, (the default), TQIconView computes suitable
    column widths based on the icon view's contents.

    Note that setting a grid width overrides setMaxItemWidth().
*/

void TQIconView::setGridX( int rx )
{
    d->rastX = rx >= 0 ? rx : -1;
}

/*!
    \property TQIconView::gridY
    \brief the vertical grid  of the icon view

    If the value is -1, (the default), TQIconView computes suitable
    column heights based on the icon view's contents.
*/

void TQIconView::setGridY( int ry )
{
    d->rastY = ry >= 0 ? ry : -1;
}

int TQIconView::gridX() const
{
    return d->rastX;
}

int TQIconView::gridY() const
{
    return d->rastY;
}

/*!
    \property TQIconView::spacing
    \brief the space in pixels between icon view items

    The default is 5 pixels.

    Negative values for spacing are illegal.
*/

void TQIconView::setSpacing( int sp )
{
    d->spacing = sp;
}

int TQIconView::spacing() const
{
    return d->spacing;
}

/*!
    \property TQIconView::itemTextPos
    \brief the position where the text of each item is drawn.

    Valid values are \c Bottom or \c Right. The default is \c Bottom.
*/

void TQIconView::setItemTextPos( ItemTextPos pos )
{
    if ( pos == d->itemTextPos || ( pos != Bottom && pos != Right ) )
	return;

    d->itemTextPos = pos;

    TQIconViewItem *item = d->firstItem;
    for ( ; item; item = item->next ) {
	item->wordWrapDirty = true;
	item->calcRect();
    }

    arrangeItemsInGrid( true );
}

TQIconView::ItemTextPos TQIconView::itemTextPos() const
{
    return d->itemTextPos;
}

/*!
    \property TQIconView::itemTextBackground
    \brief the brush to use when drawing the background of an item's text.

    By default this brush is set to NoBrush, meaning that only the
    normal icon view background is used.
*/

void TQIconView::setItemTextBackground( const TQBrush &brush )
{
    d->itemTextBrush = brush;
}

TQBrush TQIconView::itemTextBackground() const
{
    return d->itemTextBrush;
}

/*!
    \property TQIconView::arrangement
    \brief the arrangement mode of the icon view

    This can be \c LeftToRight or \c TopToBottom. The default is \c
    LeftToRight.
*/

void TQIconView::setArrangement( Arrangement am )
{
    if ( d->arrangement == am )
	return;

    d->arrangement = am;

    viewport()->setUpdatesEnabled( false );
    resizeContents( viewport()->width(), viewport()->height() );
    viewport()->setUpdatesEnabled( true );
    arrangeItemsInGrid( true );
}

TQIconView::Arrangement TQIconView::arrangement() const
{
    return d->arrangement;
}

/*!
    \property TQIconView::resizeMode
    \brief the resize mode of the icon view

    This can be \c Fixed or \c Adjust. The default is \c Fixed.
    See \l ResizeMode.
*/

void TQIconView::setResizeMode( ResizeMode rm )
{
    if ( d->resizeMode == rm )
	return;

    d->resizeMode = rm;
}

TQIconView::ResizeMode TQIconView::resizeMode() const
{
    return d->resizeMode;
}

/*!
    \property TQIconView::maxItemWidth
    \brief the maximum width that an item may have.

    The default is 100 pixels.

    Note that if the gridX() value is set TQIconView will ignore
    this property.
*/

void TQIconView::setMaxItemWidth( int w )
{
    d->maxItemWidth = w;
}

/*!
    \property TQIconView::maxItemTextLength
    \brief the maximum length (in characters) that an item's text may have.

    The default is 255 characters.
*/

void TQIconView::setMaxItemTextLength( int w )
{
    d->maxItemTextLength = w;
}

int TQIconView::maxItemWidth() const
{
    if ( d->rastX != -1 )
	return d->rastX - 2;
    else
	return d->maxItemWidth;
}

int TQIconView::maxItemTextLength() const
{
    return d->maxItemTextLength;
}

/*!
    \property TQIconView::itemsMovable
    \brief whether the user is allowed to move items around in the icon view

    The default is true.
*/

void TQIconView::setItemsMovable( bool b )
{
    d->rearrangeEnabled = b;
}

bool TQIconView::itemsMovable() const
{
    return d->rearrangeEnabled;
}

/*!
    \property TQIconView::autoArrange
    \brief whether the icon view rearranges its items when a new item is inserted.

    The default is true.

    Note that if the icon view is not visible at the time of
    insertion, TQIconView defers all position-related work until it is
    shown and then calls arrangeItemsInGrid().
*/

void TQIconView::setAutoArrange( bool b )
{
    d->reorderItemsWhenInsert = b;
}

bool TQIconView::autoArrange() const
{
    return d->reorderItemsWhenInsert;
}

/*!
    If \a sort is true, this function sets the icon view to sort items
    when a new item is inserted. If \a sort is false, the icon view
    will not be sorted.

    Note that autoArrange() must be true for sorting to take place.

    If \a ascending is true (the default), items are sorted in
    ascending order. If \a ascending is false, items are sorted in
    descending order.

    TQIconViewItem::compare() is used to compare pairs of items. The
    sorting is based on the items' keys; these default to the items'
    text unless specifically set to something else.

    \sa TQIconView::setAutoArrange(), TQIconView::autoArrange(),
    sortDirection(), sort(), TQIconViewItem::setKey()
*/

void TQIconView::setSorting( bool sort, bool ascending )
{
    d->resortItemsWhenInsert = sort;
    d->sortDirection = ascending;
}

/*!
    \property TQIconView::sorting
    \brief whether the icon view sorts on insertion

    The default is false, i.e. no sorting on insertion.

    To set the sorting, use setSorting().
*/

bool TQIconView::sorting() const
{
    return d->resortItemsWhenInsert;
}

/*!
    \property TQIconView::sortDirection
    \brief whether the sort direction for inserting new items is ascending;

    The default is true (i.e. ascending). This sort direction is only
    meaningful if both sorting() and autoArrange() are true.

    To set the sort direction, use setSorting()
*/

bool TQIconView::sortDirection() const
{
    return d->sortDirection;
}

/*!
    \property TQIconView::wordWrapIconText
    \brief whether the item text will be word-wrapped if it is too long

    The default is true.

    If this property is false, icon text that is too long is
    truncated, and an ellipsis (...) appended to indicate that
    truncation has occurred. The full text can still be seen by the
    user if they hover the mouse because the full text is shown in a
    tooltip; see setShowToolTips().
*/

void TQIconView::setWordWrapIconText( bool b )
{
    if ( d->wordWrapIconText == (uint)b )
	return;

    d->wordWrapIconText = b;
    for ( TQIconViewItem *item = d->firstItem; item; item = item->next ) {
	item->wordWrapDirty = true;
	item->calcRect();
    }
    arrangeItemsInGrid( true );
}

bool TQIconView::wordWrapIconText() const
{
    return d->wordWrapIconText;
}

/*!
    \property TQIconView::showToolTips
    \brief whether the icon view will display a tool tip with the complete text for any truncated item text

    The default is true. Note that this has no effect if
    setWordWrapIconText() is true, as it is by default.
*/

void TQIconView::setShowToolTips( bool b )
{
    d->showTips = b;
}

bool TQIconView::showToolTips() const
{
    return d->showTips;
}

/*!
    \reimp
*/
void TQIconView::contentsMousePressEvent( TQMouseEvent *e )
{
    contentsMousePressEventEx( e );
}

void TQIconView::contentsMousePressEventEx( TQMouseEvent *e )
{
    if ( d->rubber ) {
        TQRect r( d->rubber->normalize() );
        delete d->rubber;
        d->rubber = 0;  
   
        repaintContents( r, false );
        d->dragging = false;

	delete d->backrubber;
	d->backrubber = 0;

	if ( d->scrollTimer ) {
	    disconnect( d->scrollTimer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( doAutoScroll() ) );
	    d->scrollTimer->stop();
	    delete d->scrollTimer;
	    d->scrollTimer = 0;
	}
    }

    d->dragStartPos = e->pos();
    TQIconViewItem *item = findItem( e->pos() );
    d->pressedItem = item;

    if ( item )
	d->selectAnchor = item;

#ifndef TQT_NO_TEXTEDIT
    if ( d->renamingItem )
	d->renamingItem->renameItem();
#endif

    if ( !d->currentItem && !item && d->firstItem ) {
	d->currentItem = d->firstItem;
	repaintItem( d->firstItem );
    }

    if (item && item->dragEnabled())
	d->startDragItem = item;
    else
	d->startDragItem = 0;

    if ( e->button() == LeftButton && !( e->state() & ShiftButton ) &&
	 !( e->state() & ControlButton ) && item && item->isSelected() &&
	 item->textRect( false ).contains( e->pos() ) ) {

	if ( !item->renameEnabled() ) {
	    d->mousePressed = true;
#ifndef TQT_NO_TEXTEDIT
	} else {
	    ensureItemVisible( item );
	    setCurrentItem( item );
	    item->rename();
	    goto emit_signals;
#endif
	}
    }

    d->pressedSelected = item && item->isSelected();

    if ( item && item->isSelectable() ) {
	if ( d->selectionMode == Single )
	    item->setSelected( true, e->state() & ControlButton );
	else if ( d->selectionMode == Multi )
	    item->setSelected( !item->isSelected(), e->state() & ControlButton );
	else if ( d->selectionMode == Extended ) {
	    if ( e->state() & ShiftButton ) {
		d->pressedSelected = false;
		bool block = signalsBlocked();
		blockSignals( true );
		viewport()->setUpdatesEnabled( false );
		TQRect r;
		bool select = true;
		if ( d->currentItem )
		    r = TQRect( TQMIN( d->currentItem->x(), item->x() ),
			       TQMIN( d->currentItem->y(), item->y() ),
			       0, 0 );
		else
		    r = TQRect( 0, 0, 0, 0 );
		if ( d->currentItem ) {
		    if ( d->currentItem->x() < item->x() )
			r.setWidth( item->x() - d->currentItem->x() + item->width() );
		    else
			r.setWidth( d->currentItem->x() - item->x() + d->currentItem->width() );
		    if ( d->currentItem->y() < item->y() )
			r.setHeight( item->y() - d->currentItem->y() + item->height() );
		    else
			r.setHeight( d->currentItem->y() - item->y() + d->currentItem->height() );
		    r = r.normalize();
		    TQIconViewPrivate::ItemContainer *c = d->firstContainer;
		    bool alreadyIntersected = false;
		    TQRect redraw;
		    for ( ; c; c = c->n ) {
			if ( c->rect.intersects( r ) ) {
			    alreadyIntersected = true;
			    TQIconViewItem *i = c->items.first();
			    for ( ; i; i = c->items.next() ) {
				if ( r.intersects( i->rect() ) ) {
				    redraw = redraw.unite( i->rect() );
				    i->setSelected( select, true );
				}
			    }
			} else {
			    if ( alreadyIntersected )
				break;
			}
		    }
		    redraw = redraw.unite( item->rect() );
		    viewport()->setUpdatesEnabled( true );
		    repaintContents( redraw, false );
		}
		blockSignals( block );
		viewport()->setUpdatesEnabled( true );
		item->setSelected( select, true );
		emit selectionChanged();
	    } else if ( e->state() & ControlButton ) {
		d->pressedSelected = false;
		item->setSelected( !item->isSelected(), e->state() & ControlButton );
	    } else {
		item->setSelected( true, e->state() & ControlButton );
	    }
	}
    } else if ( ( d->selectionMode != Single || e->button() == RightButton )
		&& !( e->state() & ControlButton ) )
	selectAll( false );

    setCurrentItem( item );

    d->canStartRubber = false;
    if ( e->button() == LeftButton ) {
	if ( !item && ( d->selectionMode == Multi || d->selectionMode == Extended ) ) {
	    d->canStartRubber = true;
	    d->rubberStartX = e->x();
	    d->rubberStartY = e->y();
	}

	d->mousePressed = true;
    }

 emit_signals:
    if ( !d->canStartRubber ) {
	emit mouseButtonPressed( e->button(), item, e->globalPos() );
	emit pressed( item );
	emit pressed( item, e->globalPos() );

	if ( e->button() == RightButton )
	    emit rightButtonPressed( item, e->globalPos() );
    }
}

/*!
    \reimp
*/

void TQIconView::contentsContextMenuEvent( TQContextMenuEvent *e )
{
    if ( !receivers( TQ_SIGNAL(contextMenuRequested(TQIconViewItem*,const TQPoint&)) ) ) {
	e->ignore();
	return;
    }
    if ( e->reason() == TQContextMenuEvent::Keyboard ) {
	TQIconViewItem *item = currentItem();
	TQRect r = item ? item->rect() : TQRect( 0, 0, visibleWidth(), visibleHeight() );
	emit contextMenuRequested( item, viewport()->mapToGlobal( contentsToViewport( r.center() ) ) );
    } else {
	d->mousePressed = false;
	TQIconViewItem *item = findItem( e->pos() );
	emit contextMenuRequested( item, e->globalPos() );
    }
}

/*!
    \reimp
*/

void TQIconView::contentsMouseReleaseEvent( TQMouseEvent *e )
{
    TQIconViewItem *item = findItem( e->pos() );
    d->selectedItems.clear();

    bool emitClicked = true;
    d->mousePressed = false;
    d->startDragItem = 0;

    d->canStartRubber = false;
    if ( d->rubber ) {
        TQRect r(d->rubber->normalize());
        
	if ( ( d->rubber->topLeft() - d->rubber->bottomRight() ).manhattanLength() >
	     TQApplication::startDragDistance() )
	    emitClicked = false;
	delete d->rubber;
    	d->rubber = 0;
	repaintContents(r, false);
	d->dragging = false;
	delete d->backrubber;
	d->backrubber = 0;      
	d->currentItem = d->tmpCurrentItem;
	d->tmpCurrentItem = 0;
	if ( d->currentItem )
	    repaintItem( d->currentItem );
    }

    if ( d->scrollTimer ) {
	disconnect( d->scrollTimer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( doAutoScroll() ) );
	d->scrollTimer->stop();
	delete d->scrollTimer;
	d->scrollTimer = 0;
    }

    if ( d->selectionMode == Extended &&
	 d->currentItem == d->pressedItem &&
	 d->pressedSelected && d->currentItem ) {
	bool block = signalsBlocked();
	blockSignals( true );
	clearSelection();
	blockSignals( block );
	if ( d->currentItem->isSelectable() ) {
	    d->currentItem->selected = true;
	    repaintItem( d->currentItem );
	}
	emit selectionChanged();
    }
    d->pressedItem = 0;

    if ( emitClicked ) {
	emit mouseButtonClicked( e->button(), item, e->globalPos() );
	emit clicked( item );
	emit clicked( item, e->globalPos() );
	if ( e->button() == RightButton )
	    emit rightButtonClicked( item, e->globalPos() );
    }
}

/*!
    \reimp
*/

void TQIconView::contentsMouseMoveEvent( TQMouseEvent *e )
{
    TQIconViewItem *item = findItem( e->pos() );
    if ( d->highlightedItem != item ) {
	if ( item )
	    emit onItem( item );
	else
	    emit onViewport();
	d->highlightedItem = item;
    }

    if ( d->mousePressed && e->state() == NoButton )
	d->mousePressed = false;

    if ( d->startDragItem )
	item = d->startDragItem;

    if ( d->mousePressed && item && item == d->currentItem &&
	 ( item->isSelected() || d->selectionMode == NoSelection ) && item->dragEnabled() ) {
	if ( !d->startDragItem ) {
	    d->currentItem->setSelected( true, true );
	    d->startDragItem = item;
	}
	if ( ( d->dragStartPos - e->pos() ).manhattanLength() > TQApplication::startDragDistance() ) {
	    d->mousePressed = false;
	    d->cleared = false;
#ifndef TQT_NO_DRAGANDDROP
	    startDrag();
#endif
	    if ( d->tmpCurrentItem )
		repaintItem( d->tmpCurrentItem );
	}
    } else if ( d->mousePressed && ((!d->currentItem && d->rubber) || d->canStartRubber) ) {
	if ( d->canStartRubber ) {
	    d->canStartRubber = false;
	    d->tmpCurrentItem = d->currentItem;
	    d->currentItem = 0;
	    repaintItem( d->tmpCurrentItem );
	    delete d->rubber;
	    d->rubber = new TQRect( d->rubberStartX, d->rubberStartY, 0, 0 );
	    d->selectedItems.clear();
	    if ( ( e->state() & ControlButton ) == ControlButton ||
	         ( e->state() & ShiftButton ) == ShiftButton ) {
		for ( TQIconViewItem *i = firstItem(); i; i = i->nextItem() )
		    if ( i->isSelected() )
			d->selectedItems.insert( i, i );
	    }
	}
	doAutoScroll();
    }
}

/*!
    \reimp
*/

void TQIconView::contentsMouseDoubleClickEvent( TQMouseEvent *e )
{
    TQIconViewItem *item = findItem( e->pos() );
    if ( item ) {
	selectAll( false );
	item->setSelected( true, true );
	emit doubleClicked( item );
    }
}

/*!
    \reimp
*/

#ifndef TQT_NO_DRAGANDDROP
void TQIconView::contentsDragEnterEvent( TQDragEnterEvent *e )
{
    d->dragging = true;
    d->drawDragShapes = true;
    d->tmpCurrentItem = 0;
    initDragEnter( e );
    d->oldDragPos = e->pos();
    d->oldDragAcceptAction = false;
    drawDragShapes( e->pos() );
    d->dropped = false;
}

/*!
    \reimp
*/

void TQIconView::contentsDragMoveEvent( TQDragMoveEvent *e )
{
    if ( e->pos() == d->oldDragPos ) {
	if (d->oldDragAcceptAction)
	    e->acceptAction();
	else
	    e->ignore();
	return;
    }

    drawDragShapes( d->oldDragPos );
    d->dragging = false;

    TQIconViewItem *old = d->tmpCurrentItem;
    d->tmpCurrentItem = 0;

    TQIconViewItem *item = findItem( e->pos() );

    if ( item ) {
	if ( old &&
	     old->rect().contains(d->oldDragPos) &&
	     !old->rect().contains(e->pos()) ) {
	    old->dragLeft();
	    repaintItem( old );
	}
	if ( !item->rect().contains(d->oldDragPos) )
	    item->dragEntered();
        if (item->acceptDrop(e) || (item->isSelected() && e->source() == viewport())) {
	    d->oldDragAcceptAction = true;
	    e->acceptAction();
	} else {
	    d->oldDragAcceptAction = false;
	    e->ignore();
	}

	d->tmpCurrentItem = item;
	TQPainter p;
	p.begin( viewport() );
	p.translate( -contentsX(), -contentsY() );
	item->paintFocus( &p, colorGroup() );
	p.end();
    } else {
	e->acceptAction();
	d->oldDragAcceptAction = true;
	if ( old ) {
	    old->dragLeft();
	    repaintItem( old );
	}
    }

    d->oldDragPos = e->pos();
    drawDragShapes( e->pos() );
    d->dragging = true;
}

/*!
    \reimp
*/

void TQIconView::contentsDragLeaveEvent( TQDragLeaveEvent * )
{
    if ( !d->dropped )
	drawDragShapes( d->oldDragPos );
    d->dragging = false;

    if ( d->tmpCurrentItem ) {
	repaintItem( d->tmpCurrentItem );
	d->tmpCurrentItem->dragLeft();
    }

    d->tmpCurrentItem = 0;
    d->isIconDrag = false;
    d->iconDragData.clear();
}

/*!
    \reimp
*/

void TQIconView::contentsDropEvent( TQDropEvent *e )
{
    d->dropped = true;
    d->dragging = false;
    drawDragShapes( d->oldDragPos );

    if ( d->tmpCurrentItem )
	repaintItem( d->tmpCurrentItem );

    TQIconViewItem *i = findItem( e->pos() );

    if ((!i || i->isSelected()) && e->source() == viewport() && d->currentItem && !d->cleared) {
	if ( !d->rearrangeEnabled )
	    return;
	TQRect r = d->currentItem->rect();

	d->currentItem->move( e->pos() - d->dragStart );

	int w = d->currentItem->x() + d->currentItem->width() + 1;
	int h = d->currentItem->y() + d->currentItem->height() + 1;

	repaintItem( d->currentItem );
	repaintContents( r.x(), r.y(), r.width(), r.height(), false );

	int dx = d->currentItem->x() - r.x();
	int dy = d->currentItem->y() - r.y();

	TQIconViewItem *item = d->firstItem;
	TQRect rr;
	for ( ; item; item = item->next ) {
	    if ( item->isSelected() && item != d->currentItem ) {
		rr = rr.unite( item->rect() );
		item->moveBy( dx, dy );
		rr = rr.unite( item->rect() );
	    }
	    w = TQMAX( w, item->x() + item->width() + 1 );
	    h = TQMAX( h, item->y() + item->height() + 1 );
	}
	repaintContents( rr, false );
	bool fullRepaint = false;
	if ( w > contentsWidth() ||
	     h > contentsHeight() )
	    fullRepaint = true;

	int oldw = contentsWidth();
	int oldh = contentsHeight();

	resizeContents( w, h );


	if ( fullRepaint ) {
	    repaintContents( oldw, 0, contentsWidth() - oldw, contentsHeight(), false );
	    repaintContents( 0, oldh, contentsWidth(), contentsHeight() - oldh, false );
	}
	e->acceptAction();
    } else if ( !i && ( e->source() != viewport() || d->cleared ) ) {
	TQValueList<TQIconDragItem> lst;
	if ( TQIconDrag::canDecode( e ) ) {
	    TQValueList<TQIconDragDataItem> l;
	    TQIconDragPrivate::decode( e, l );
	    TQValueList<TQIconDragDataItem>::Iterator it = l.begin();
	    for ( ; it != l.end(); ++it )
		lst << ( *it ).data;
	}
	emit dropped( e, lst );
    } else if ( i ) {
	TQValueList<TQIconDragItem> lst;
	if ( TQIconDrag::canDecode( e ) ) {
	    TQValueList<TQIconDragDataItem> l;
	    TQIconDragPrivate::decode( e, l );
	    TQValueList<TQIconDragDataItem>::Iterator it = l.begin();
	    for ( ; it != l.end(); ++it )
		lst << ( *it ).data;
	}
	i->dropped( e, lst );
    }
    d->isIconDrag = false;
}
#endif

/*!
    This function grabs all paintevents that otherwise would have been
    processed by the TQScrollView::viewportPaintEvent(). Here we use a
    doublebuffer to reduce 'on-paint' flickering on TQIconView
    (and of course its children).
    
    \sa TQScrollView::viewportPaintEvent(), TQIconView::drawContents()
*/

void TQIconView::bufferedPaintEvent( TQPaintEvent* pe )
{
    TQWidget* vp = viewport();
    TQRect r = pe->rect() & vp->rect();
    int ex = r.x() + contentsX();
    int ey = r.y() + contentsY();
    int ew = r.width();
    int eh = r.height();

    if ( !d->backBuffer )
	d->backBuffer = new TQPixmap(vp->size());
    if ( d->backBuffer->size() != vp->size() ) {
	// Resize function (with hysteresis). Uses a good compromise between memory
	// consumption and speed (number) of resizes.
	float newWidth = (float)vp->width();
	float newHeight = (float)vp->height();
	if ( newWidth > d->backBuffer->width() || newHeight > d->backBuffer->height() )
	{
	    newWidth *= 1.1892;
	    newHeight *= 1.1892;
	    d->backBuffer->resize( (int)newWidth, (int)newHeight );
	} else if ( 1.5*newWidth < d->backBuffer->width() || 1.5*newHeight < d->backBuffer->height() )
	    d->backBuffer->resize( (int)newWidth, (int)newHeight );
    }

    TQPainter p;
    p.begin(d->backBuffer, vp);
    drawContentsOffset(&p, contentsX(), contentsY(), ex, ey, ew, eh);
    p.end();
    bitBlt(vp, r.x(), r.y(), d->backBuffer, r.x(), r.y(), ew, eh);
}

/*!
    \reimp
*/

void TQIconView::resizeEvent( TQResizeEvent* e )
{
    TQScrollView::resizeEvent( e );
    if ( d->resizeMode == Adjust ) {
	optimize_layout = true;
	adjustItems();
	optimize_layout = false;
#if 0 // no need for timer delay anymore
	d->oldSize = e->oldSize();
	if ( d->adjustTimer->isActive() )
	    d->adjustTimer->stop();
	d->adjustTimer->start( 0, true );
#endif
    }
}

/*!
    Adjusts the positions of the items to the geometry of the icon
    view.
*/

void TQIconView::adjustItems()
{
    d->adjustTimer->stop();
    if ( d->resizeMode == Adjust )
	    arrangeItemsInGrid( true );
}

/*!
    \reimp
*/

void TQIconView::keyPressEvent( TQKeyEvent *e )
{
    if ( !d->firstItem )
	return;

    if ( !d->currentItem ) {
	setCurrentItem( d->firstItem );
	if ( d->selectionMode == Single )
	    d->currentItem->setSelected( true, true );
	return;
    }

    bool selectCurrent = true;

    switch ( e->key() ) {
    case Key_Escape:
	e->ignore();
	break;
#ifndef TQT_NO_TEXTEDIT
    case Key_F2: {
	if ( d->currentItem->renameEnabled() ) {
	    d->currentItem->renameItem();
	    d->currentItem->rename();
	    return;
	}
    } break;
#endif

    case Key_Home: {
	d->currInputString = TQString::null;
	if ( !d->firstItem )
	    break;

	selectCurrent = false;

	TQIconViewItem *item = 0;
	TQIconViewPrivate::ItemContainer *c = d->firstContainer;
	while ( !item && c ) {
	    TQPtrList<TQIconViewItem> &list = c->items;
	    TQIconViewItem *i = list.first();
	    while ( i ) {
		if ( !item ) {
		    item = i;
		} else {
		    if ( d->arrangement == LeftToRight ) {
			// we use pixmap so the items textlength are ignored
			// find topmost, leftmost item
			if ( i->pixmapRect( false ).y() < item->pixmapRect( false ).y() ||
			     ( i->pixmapRect( false ).y() == item->pixmapRect( false ).y() &&
			       i->pixmapRect( false ).x() < item->pixmapRect( false ).x() ) )
			    item = i;
		    } else {
			// find leftmost, topmost item
			if ( i->pixmapRect( false ).x() < item->pixmapRect( false ).x() ||
			     ( i->pixmapRect( false ).x() == item->pixmapRect( false ).x() &&
			       i->pixmapRect( false ).y() < item->pixmapRect( false ).y() ) )
			    item = i;
		    }
		}
		i = list.next();
	    }
	    c = c->n;
	}

	if ( item ) {
	    TQIconViewItem *old = d->currentItem;
	    setCurrentItem( item );
	    ensureItemVisible( item );
	    handleItemChange( old, e->state() & ShiftButton,
			      e->state() & ControlButton, true );
	}
    } break;
    
    case Key_End: {
	d->currInputString = TQString::null;
	if ( !d->lastItem )
	    break;

	selectCurrent = false;

	TQIconViewItem *item = 0;
	TQIconViewPrivate::ItemContainer *c = d->lastContainer;
	while ( !item && c ) {
	    TQPtrList<TQIconViewItem> &list = c->items;
	    TQIconViewItem *i = list.first();
	    while ( i ) {
		if ( !item ) {
		    item = i;
		} else {
		    if ( d->arrangement == LeftToRight ) {
			// find bottommost, rightmost item
			if ( i->pixmapRect( false ).bottom() > item->pixmapRect( false ).bottom() ||
			     ( i->pixmapRect( false ).bottom() == item->pixmapRect( false ).bottom() &&
			       i->pixmapRect( false ).right() > item->pixmapRect( false ).right() ) )
			    item = i;
		    } else {
			// find rightmost, bottommost item
			if ( i->pixmapRect( false ).right() > item->pixmapRect( false ).right() ||
			     ( i->pixmapRect( false ).right() == item->pixmapRect( false ).right() &&
			       i->pixmapRect( false ).bottom() > item->pixmapRect( false ).bottom() ) )
			    item = i;
		    }
		}
		i = list.next();
	    }
	    c = c->p;
	}

	if ( item ) {
	    TQIconViewItem *old = d->currentItem;
	    setCurrentItem( item );
	    ensureItemVisible( item );
 	    handleItemChange( old, e->state() & ShiftButton,
 			      e->state() & ControlButton, true );
	}
    } break;
    
    case Key_Space: {
	d->currInputString = TQString::null;
	if ( d->selectionMode == Single)
	    break;

	d->currentItem->setSelected( !d->currentItem->isSelected(), true );
    } break;
    
    case Key_Enter: 
    case Key_Return:
	d->currInputString = TQString::null;
	emit returnPressed( d->currentItem );
	break;
	
    case Key_Right: {
	d->currInputString = TQString::null;
	selectCurrent = false;
	Direction dir = DirRight;
	TQIconViewItem *item = findItem(dir, d->currentItem);
	if (item) {
	    TQIconViewItem *old=d->currentItem;
	    setCurrentItem(item);
	    ensureItemVisible(item);
	    handleItemChange(old, e->state() & ShiftButton, e->state() & ControlButton );
        }
    } break;
    
    case Key_Left: {
	d->currInputString = TQString::null;
	selectCurrent = false;
	Direction dir = DirLeft;
	TQIconViewItem *item = findItem(dir, d->currentItem);
	if (item) {
	    TQIconViewItem *old=d->currentItem;
	    setCurrentItem(item);
	    ensureItemVisible(item);
	    handleItemChange(old, e->state() & ShiftButton, e->state() & ControlButton );
        }
    } break;
    
    case Key_Down: {
	d->currInputString = TQString::null;
	selectCurrent = false;
	Direction dir = DirDown;
	TQIconViewItem *item = findItem(dir, d->currentItem);
	if (item) {
	    TQIconViewItem *old=d->currentItem;
	    setCurrentItem(item);
	    ensureItemVisible(item);
	    handleItemChange(old, e->state() & ShiftButton, e->state() & ControlButton );
        }
    } break;
    
    case Key_Up: {
	d->currInputString = TQString::null;
	selectCurrent = false;
	Direction dir = DirUp;
	TQIconViewItem *item = findItem(dir, d->currentItem);
	if (item) {
	    TQIconViewItem *old=d->currentItem;
	    setCurrentItem(item);
	    ensureItemVisible(item);
	    handleItemChange(old, e->state() & ShiftButton, e->state() & ControlButton );
        }
    } break;
    
    case Key_Next: {
	d->currInputString = TQString::null;
	selectCurrent = false;
	TQRect r;
	if ( d->arrangement == LeftToRight )
	    r = TQRect( 0, d->currentItem->y() + visibleHeight(), contentsWidth(), visibleHeight() );
	else
	    r = TQRect( d->currentItem->x() + visibleWidth(), 0, visibleWidth(), contentsHeight() );
	TQIconViewItem *item = d->currentItem;
	TQIconViewItem *ni = findFirstVisibleItem( r  );
	if ( !ni ) {
	    if ( d->arrangement == LeftToRight )
		r = TQRect( 0, d->currentItem->y() + d->currentItem->height(), contentsWidth(), contentsHeight() );
	    else
		r = TQRect( d->currentItem->x() + d->currentItem->width(), 0, contentsWidth(), contentsHeight() );
	    ni = findLastVisibleItem( r  );
	}
	if ( ni ) {
	    setCurrentItem( ni );
	    handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
	}
    } break;
    
    case Key_Prior: {
	d->currInputString = TQString::null;
	selectCurrent = false;
	TQRect r;
	if ( d->arrangement == LeftToRight )
	    r = TQRect( 0, d->currentItem->y() - visibleHeight(), contentsWidth(), visibleHeight() );
	else
	    r = TQRect( d->currentItem->x() - visibleWidth(), 0, visibleWidth(), contentsHeight() );
	TQIconViewItem *item = d->currentItem;
	TQIconViewItem *ni = findFirstVisibleItem( r  );
	if ( !ni ) {
	    if ( d->arrangement == LeftToRight )
		r = TQRect( 0, 0, contentsWidth(), d->currentItem->y() );
	    else
		r = TQRect( 0, 0, d->currentItem->x(), contentsHeight() );
	    ni = findFirstVisibleItem( r  );
	}
	if ( ni ) {
	    setCurrentItem( ni );
	    handleItemChange( item, e->state() & ShiftButton, e->state() & ControlButton );
	}
    } break;
    
    default:
	if ( !e->text().isEmpty() && e->text()[ 0 ].isPrint() ) {
	    selectCurrent = false;
	    TQIconViewItem *i = d->currentItem;
	    if ( !i )
		i = d->firstItem;
	    if ( !d->inputTimer->isActive() ) {
		d->currInputString = e->text();
		i = i->next;
		if ( !i )
		    i = d->firstItem;
		i = findItemByName( i );
	    } else {
		d->inputTimer->stop();
		d->currInputString += e->text();
		i = findItemByName( i );
		if ( !i ) {
		    d->currInputString = e->text();
		    if (d->currentItem && d->currentItem->next)
			i = d->currentItem->next;
		    else
			i = d->firstItem;
		    i = findItemByName(i);
		}
	    }
	    if ( i ) {
		setCurrentItem( i );
		if ( d->selectionMode == Extended ) {
		    bool changed = false;
		    bool block = signalsBlocked();
		    blockSignals( true );
		    selectAll( false );
		    blockSignals( block );
		    if ( !i->selected && i->isSelectable() ) {
			changed = true;
			i->selected = true;
			repaintItem( i );
		    }
		    if ( changed )
			emit selectionChanged();
		}
	    }
	    d->inputTimer->start( 400, true );
	} else {
	    selectCurrent = false;
	    d->currInputString = TQString::null;
	    if ( e->state() & ControlButton ) {
		switch ( e->key() ) {
		case Key_A:
		    selectAll( true );
		    break;
		}
	    }
	    e->ignore();
	    return;
	}
    }

    if ( !( e->state() & ShiftButton ) || !d->selectAnchor )
	d->selectAnchor = d->currentItem;

    if ( d->currentItem && !d->currentItem->isSelected() &&
	 d->selectionMode == Single && selectCurrent ) {
	d->currentItem->setSelected( true );
    }

    ensureItemVisible( d->currentItem );
}

/*
  Finds the closest item in the direction \a dir starting from the specified \a fromItem.
  If the arrangement is LeftToRight (icon view mode): use (center, top) as item reference
  If the arrangement is TopToBottom (multicolumn view mode): use (left, top) as item reference
  This is to allow for smooth scrolling when using the keyboard arrow keys.
*/
TQIconViewItem* TQIconView::findItem(Direction dir, const TQIconViewItem *fromItem) const
{
    TQIconViewItem *closestItem=NULL;
    int distPri=0, distSec=0;
    int itemDistancePri=0, itemDistanceSec=0;

    TQPoint pos=fromItem->rect().topLeft();
    if (d->arrangement == LeftToRight) {
        pos.setX(fromItem->rect().center().x());
    }

    TQRect searchRect;
    switch (dir) {
        case DirDown:
            searchRect.setCoords(pos.x(), 0, contentsWidth(), contentsHeight());
            break;
    
        case DirUp:
            searchRect.setCoords(0, 0, pos.x(), contentsHeight());
            break;
    
        case DirRight:
            searchRect.setCoords(0, pos.y(), contentsWidth(), contentsHeight());
            break;
    
        case DirLeft:
            searchRect.setCoords(0, 0, contentsWidth(), pos.y());
            break;
    }
    
    for (TQIconViewPrivate::ItemContainer *c=d->firstContainer; c; c=c->n) {
        if (c->rect.intersects(searchRect)) {
            TQPtrList<TQIconViewItem> &list = c->items;
            for (TQIconViewItem *item=list.first(); item; item=list.next()) {
                if (item != fromItem) {
                    bool itemOK = true;
                    const TQRect &ir = item->rect();
                    // DirDown/DirUp   : primary distance X, secondary distance Y
                    // DirLeft/DirRight: primary distance Y, secondary distance X
                    if (d->arrangement == LeftToRight) {
                        // Left to right arrangement (icon view mode): use (center, top) as item reference
                        switch (dir) {
                            case DirDown:
                                if (ir.center().x() > pos.x()) {
                                    distPri = ir.center().x()-pos.x();
                                    distSec = ir.top();
                                    }
                                else if (ir.center().x() == pos.x() && ir.top() > pos.y()) {
                                    distPri = 0;
                                    distSec = ir.top()-pos.y();
                                    }
                                else {
                                    itemOK = false;
                                    }
                                break;
                            
                            case DirUp:
                                if (ir.center().x() < pos.x()) {
                                    distPri = pos.x()-ir.center().x();
                                    distSec = contentsHeight()-ir.top();
                                    }
                                else if (ir.center().x() == pos.x() && ir.top() < pos.y()) {
                                    distPri = 0;
                                    distSec = pos.y()-ir.top();
                                    }
                                else {
                                    itemOK = false;
                                    }
                                break;
                            
                            case DirRight:
                                if (ir.top() > pos.y()) {
                                    distPri = ir.top()-pos.y();
                                    distSec = ir.center().x();
                                    }
                                else if (ir.top() == pos.y() && ir.center().x() > pos.x()) {
                                    distPri = 0;
                                    distSec = ir.center().x()-pos.x();
                                    }
                                else {
                                    itemOK = false;
                                    }
                                break;
                            
                            case DirLeft:
                                if (ir.top() < pos.y()) {
                                    distPri = pos.y()-ir.top();
                                    distSec = contentsWidth()-ir.center().x();
                                    }
                                else if (ir.top() == pos.y() && ir.center().x() < pos.x()) {
                                    distPri = 0;
                                    distSec = pos.x()-ir.center().x();
                                    }
                                else {
                                    itemOK = false;
                                    }
                                break;
                                
                            default:
                                itemOK = false;
                                break;
                        }	
                    }
                    else {
                        // Top to bottom arrangement (multicolumn view mode): use (left, top) as item reference
                        switch (dir) {
                            case DirDown:
                                if (ir.left() > pos.x()) {
                                    distPri = ir.left()-pos.x();
                                    distSec = ir.top();
                                    }
                                else if (ir.left() == pos.x() && ir.top() > pos.y()) {
                                    distPri = 0;
                                    distSec = ir.top()-pos.y();
                                    }
                                else {
                                    itemOK = false;
                                    }
                                break;
                            
                            case DirUp:
                                if (ir.left() < pos.x()) {
                                    distPri = pos.x()-ir.left();
                                    distSec = contentsHeight()-ir.top();
                                    }
                                else if (ir.left() == pos.x() && ir.top() < pos.y()) {
                                    distPri = 0;
                                    distSec = pos.y()-ir.top();
                                    }
                                else {
                                    itemOK = false;
                                    }
                                break;
                            
                            case DirRight:
                                if (ir.top() > pos.y()) {
                                    distPri = ir.top()-pos.y();
                                    distSec = ir.left();
                                    }
                                else if (ir.top() == pos.y() && ir.left() > pos.x()) {
                                    distPri = 0;
                                    distSec = ir.left()-pos.x();
                                    }
                                else {
                                    itemOK = false;
                                    }
                                break;
                            
                            case DirLeft:
                                if (ir.top() < pos.y()) {
                                    distPri = pos.y()-ir.top();
                                    distSec = contentsWidth()-ir.left();
                                    }
                                else if (ir.top() == pos.y() && ir.left() < pos.x()) {
                                    distPri = 0;
                                    distSec = pos.x()-ir.left();
                                    }
                                else {
                                    itemOK = false;
                                    }
                                break;
                                
                            default:
                                itemOK = false;
                                break;
                        }	
                    }
                        
                    if (itemOK) {
                        if (!closestItem ||
                            ((distPri < itemDistancePri) ||
                            (distPri == itemDistancePri && distSec < itemDistanceSec))) {
                            closestItem = item;
                            itemDistancePri = distPri;
                            itemDistanceSec = distSec;
                        } 
                    }
                }
            }
        }
    }
    return closestItem;
}

/*!
    \reimp
*/

void TQIconView::focusInEvent( TQFocusEvent* )
{
    d->mousePressed = false;
    d->inMenuMode = false;
    if ( d->currentItem ) {
	repaintItem( d->currentItem );
    } else if ( d->firstItem && TQFocusEvent::reason() != TQFocusEvent::Mouse ) {
	d->currentItem = d->firstItem;
	emit currentChanged( d->currentItem );
	repaintItem( d->currentItem );
    }

    if ( style().styleHint( TQStyle::SH_ItemView_ChangeHighlightOnFocus, this ) )
	repaintSelectedItems();

    if ( d->currentItem )
	setMicroFocusHint( d->currentItem->x(), d->currentItem->y(), d->currentItem->width(), d->currentItem->height(), false );
}

/*!
    \reimp
*/

void TQIconView::focusOutEvent( TQFocusEvent* )
{
    if (style().styleHint( TQStyle::SH_ItemView_ChangeHighlightOnFocus, this )) {
	d->inMenuMode =
	    TQFocusEvent::reason() == TQFocusEvent::Popup ||
 	    (tqApp->focusWidget() && tqApp->focusWidget()->inherits("TQMenuBar"));
 	if ( !d->inMenuMode )
	    repaintSelectedItems();
    }
    if ( d->currentItem )
	repaintItem( d->currentItem );
}

/*!
    Draws the rubber band using the painter \a p.
*/

void TQIconView::drawRubber( TQPainter *p )
{
    if ( !p || !d->rubber )
	return;

    TQPoint pnt( d->rubber->x(), d->rubber->y() );
    pnt = contentsToViewport( pnt );

    style().drawPrimitive( TQStyle::PE_RubberBand, p,
			   TQRect( pnt.x(), pnt.y(), d->rubber->width(), d->rubber->height() ).normalize(),
			   colorGroup(), TQStyle::Style_Default, TQStyleOption(colorGroup().base()) );
}

/*!
    Returns the TQDragObject that should be used for drag-and-drop.
    This function is called by the icon view when starting a drag to
    get the dragobject that should be used for the drag. Subclasses
    may reimplement this.

    \sa TQIconDrag
*/

#ifndef TQT_NO_DRAGANDDROP
TQDragObject *TQIconView::dragObject()
{
    if ( !d->currentItem )
	return 0;

    TQPoint orig = d->dragStartPos;

    TQIconDrag *drag = new TQIconDrag( viewport() );
    drag->setPixmap( ( d->currentItem->pixmap() ?
		     *d->currentItem->pixmap() : TQPixmap() ), // ### TQPicture
		     TQPoint( d->currentItem->pixmapRect().width() / 2,
			     d->currentItem->pixmapRect().height() / 2 ) );

    if ( d->selectionMode == NoSelection ) {
	TQIconViewItem *item = d->currentItem;
	drag->append( TQIconDragItem(),
		      TQRect( item->pixmapRect( false ).x() - orig.x(),
			     item->pixmapRect( false ).y() - orig.y(),
			     item->pixmapRect().width(), item->pixmapRect().height() ),
		      TQRect( item->textRect( false ).x() - orig.x(),
			     item->textRect( false ).y() - orig.y(),
			     item->textRect().width(), item->textRect().height() ) );
    } else {
	for ( TQIconViewItem *item = d->firstItem; item; item = item->next ) {
	    if ( item->isSelected() ) {
		drag->append( TQIconDragItem(),
			      TQRect( item->pixmapRect( false ).x() - orig.x(),
				     item->pixmapRect( false ).y() - orig.y(),
				     item->pixmapRect().width(), item->pixmapRect().height() ),
			      TQRect( item->textRect( false ).x() - orig.x(),
				     item->textRect( false ).y() - orig.y(),
				     item->textRect().width(), item->textRect().height() ) );
	    }
	}
    }

    return drag;
}

/*!
    Starts a drag.
*/

void TQIconView::startDrag()
{
    if ( !d->startDragItem )
	return;

    TQPoint orig = d->dragStartPos;
    d->dragStart = TQPoint( orig.x() - d->startDragItem->x(),
			   orig.y() - d->startDragItem->y() );
    d->startDragItem = 0;
    d->mousePressed = false;
    d->pressedItem = 0;
    d->pressedSelected = 0;

    TQDragObject *drag = dragObject();
    if ( !drag )
	return;

    if ( drag->drag() )
	if ( drag->target() != viewport() )
	    emit moved();
}

#endif

/*!
    Inserts the TQIconViewItem \a item in the icon view's grid. \e{You
    should never need to call this function.} Instead, insert
    TQIconViewItems by creating them with a pointer to the TQIconView
    that they are to be inserted into.
*/

void TQIconView::insertInGrid( TQIconViewItem *item )
{
    if ( !item )
	return;

    if ( d->reorderItemsWhenInsert ) {
	// #### make this efficient - but it's not too dramatic
	int y = d->spacing;

	item->dirty = false;
	if ( item == d->firstItem ) {
	    bool dummy;
	    makeRowLayout( item, y, dummy );
	    return;
	}

	TQIconViewItem *begin = rowBegin( item );
	y = begin->y();
	while ( begin ) {
	    bool dummy;
	    begin = makeRowLayout( begin, y, dummy );

	    if ( !begin || !begin->next )
		break;

	    begin = begin->next;
	}
	item->dirty = false;
    } else {
	TQRegion r( TQRect( 0, 0, TQMAX( contentsWidth(), visibleWidthSB() ),
			  TQMAX( contentsHeight(), visibleHeightSB() ) ) );

	TQIconViewItem *i = d->firstItem;
	int y = -1;
	for ( ; i; i = i->next ) {
	    r = r.subtract( i->rect() );
	    y = TQMAX( y, i->y() + i->height() );
	}

	TQMemArray<TQRect> rects = r.rects();
	TQMemArray<TQRect>::Iterator it = rects.begin();
	bool foundPlace = false;
	for ( ; it != rects.end(); ++it ) {
	    TQRect rect = *it;
	    if ( rect.width() >= item->width() &&
		 rect.height() >= item->height() ) {
		int sx = 0, sy = 0;
		if ( rect.width() >= item->width() + d->spacing )
		    sx = d->spacing;
		if ( rect.height() >= item->height() + d->spacing )
		    sy = d->spacing;
		item->move( rect.x() + sx, rect.y() + sy );
		foundPlace = true;
		break;
	    }
	}

	if ( !foundPlace )
	    item->move( d->spacing, y + d->spacing );

	resizeContents( TQMAX( contentsWidth(), item->x() + item->width() ),
			TQMAX( contentsHeight(), item->y() + item->height() ) );
	item->dirty = false;
    }
}

/*!
    Emits a signal to indicate selection changes. \a i is the
    TQIconViewItem that was selected or de-selected.

    \e{You should never need to call this function.}
*/

void TQIconView::emitSelectionChanged( TQIconViewItem *i )
{
    emit selectionChanged();
    if ( d->selectionMode == Single )
	emit selectionChanged( i ? i : d->currentItem );
}

/*!
    \internal
*/

void TQIconView::emitRenamed( TQIconViewItem *item )
{
    if ( !item )
	return;

    emit itemRenamed( item, item->text() );
    emit itemRenamed( item );
}

/*!
    If a drag enters the icon view the shapes of the objects which the
    drag contains are drawn, usnig \a pos as origin.
*/

void TQIconView::drawDragShapes( const TQPoint &pos )
{
#ifndef TQT_NO_DRAGANDDROP
    if ( pos == TQPoint( -1, -1 ) )
	return;

    if ( !d->drawDragShapes ) {
	d->drawDragShapes = true;
	return;
    }

    TQStyleOption opt(colorGroup().base());

    TQPainter p;
    p.begin( viewport() );
    p.translate( -contentsX(), -contentsY() );
    p.setRasterOp( NotROP );
    p.setPen( TQPen( color0 ) );

    if ( d->isIconDrag ) {
	TQValueList<TQIconDragDataItem>::Iterator it = d->iconDragData.begin();
	for ( ; it != d->iconDragData.end(); ++it ) {
	    TQRect ir = (*it).item.pixmapRect();
	    TQRect tr = (*it).item.textRect();
	    tr.moveBy( pos.x(), pos.y() );
	    ir.moveBy( pos.x(), pos.y() );
	    if ( !ir.intersects( TQRect( contentsX(), contentsY(), visibleWidth(), visibleHeight() ) ) )
		continue;

	    style().drawPrimitive(TQStyle::PE_FocusRect, &p, ir, colorGroup(),
				  TQStyle::Style_Default, opt);
	    style().drawPrimitive(TQStyle::PE_FocusRect, &p, tr, colorGroup(),
				  TQStyle::Style_Default, opt);
	}
    } else if ( d->numDragItems > 0 ) {
	for ( int i = 0; i < d->numDragItems; ++i ) {
	    TQRect r( pos.x() + i * 40, pos.y(), 35, 35 );
	    style().drawPrimitive(TQStyle::PE_FocusRect, &p, r, colorGroup(),
				  TQStyle::Style_Default, opt);
	}

    }
    p.end();
#endif
}

/*!
    When a drag enters the icon view, this function is called to
    initialize it. Initializing in this context means getting
    information about the drag, for example so that the icon view
    knows enough about the drag to be able to draw drag shapes for the
    drag data (e.g. shapes of icons which are dragged), etc.
*/

#ifndef TQT_NO_DRAGANDDROP
void TQIconView::initDragEnter( TQDropEvent *e )
{
    if ( TQIconDrag::canDecode( e ) ) {
	TQIconDragPrivate::decode( e, d->iconDragData );
	d->isIconDrag = true;
    } else if ( TQUriDrag::canDecode( e ) ) {
	TQStrList lst;
	TQUriDrag::decode( e, lst );
	d->numDragItems = lst.count();
    } else {
	d->numDragItems = 0;
    }

}
#endif

/*!
    This function is called to draw the rectangle \a r of the
    background using the painter \a p.

    The default implementation fills \a r with the viewport's
    backgroundBrush(). Subclasses may reimplement this to draw custom
    backgrounds.

    \sa contentsX() contentsY() drawContents()
*/

void TQIconView::drawBackground( TQPainter *p, const TQRect &r )
{
    p->fillRect( r, viewport()->backgroundBrush() );
}

/*!
    \reimp
*/

bool TQIconView::eventFilter( TQObject * o, TQEvent * e )
{
    if ( o == viewport() ) {
	switch( e->type() ) {
	case TQEvent::FocusIn:
	    focusInEvent( (TQFocusEvent*)e );
	    return true;
	case TQEvent::FocusOut:
	    focusOutEvent( (TQFocusEvent*)e );
	    return true;
	case TQEvent::Enter:
	    enterEvent( e );
	    return true;
	case TQEvent::Paint:
	    if ( o == viewport() ) {
		if ( d->dragging ) {
		    if ( !d->rubber )
			drawDragShapes( d->oldDragPos );
		    viewportPaintEvent( (TQPaintEvent*)e );
		    if ( !d->rubber )
			drawDragShapes( d->oldDragPos );
		} else {
		    bufferedPaintEvent( (TQPaintEvent*)e );
		}
	    }
	    return true;
	default:
	    // nothing
	    break;
	}
    }

    return TQScrollView::eventFilter( o, e );
}


/*!
    \reimp
*/

TQSize TQIconView::minimumSizeHint() const
{
    return TQScrollView::minimumSizeHint();
}

/*!
  \internal
  Finds the next item after the start item beginning
  with \a text.
*/

TQIconViewItem* TQIconView::findItemByName( TQIconViewItem *start )
{
    if ( !start )
	return 0;
    TQString match = d->currInputString.lower();
    if ( match.length() < 1 )
	return start;
    TQString curText;
    TQIconViewItem *i = start;
    do {
	curText = i->text().lower();
	if ( curText.startsWith( match ) )
	    return i;
	i = i->next;
	if ( !i )
	    i = d->firstItem;
    } while ( i != start );
    return 0;
}

/*!
    Lays out a row of icons (if Arrangement == \c TopToBottom this is
    a column). Starts laying out with the item \a begin. \a y is the
    starting coordinate. Returns the last item of the row (column) and
    sets the new starting coordinate to \a y. The \a changed parameter
    is used internally.

    \warning This function may be made private in a future version of
    TQt. We do not recommend calling it.
*/

TQIconViewItem *TQIconView::makeRowLayout( TQIconViewItem *begin, int &y, bool &changed )
{
    TQIconViewItem *end = 0;

    // Find the first visible item
    while (begin->next && (begin->isVisible() == false)) {
	begin = begin->next;
    }

    bool reverse = TQApplication::reverseLayout();
    changed = false;

    if ( d->arrangement == LeftToRight ) {

	if ( d->rastX == -1 ) {
	    // first calculate the row height
	    int h = 0;
	    int x = 0;
	    int ih = 0;
	    TQIconViewItem *item = begin;
	    for (;;) {
		x += d->spacing + item->width();
		if ( x > visibleWidthSB() && item != begin ) {
		    item = item->prev;
		    while (item && (item->isVisible() == false)) {
			item = item->prev;
		    }
		    break;
		}
		h = TQMAX( h, item->height() );
		ih = TQMAX( ih, item->pixmapRect().height() );
		TQIconViewItem *old = item;
		item = item->next;
		while (item && (item->isVisible() == false)) {
		    item = item->next;
		}
		if ( !item ) {
		    item = old;
		    break;
		}
	    }
	    end = item;

	    if ( d->rastY != -1 )
		h = TQMAX( h, d->rastY );

	    // now move the items
	    item = begin;
	    for (;;) {
		item->dirty = false;
		int x;
		if ( item == begin ) {
		    if ( reverse )
			x = visibleWidthSB() - d->spacing - item->width();
		    else
			x = d->spacing;
		} else {
		    if ( reverse )
			x = item->prev->x() - item->width() - d->spacing;
		    else
			x = item->prev->x() + item->prev->width() + d->spacing;
		}
		changed = item->move( x, y + ih - item->pixmapRect().height() ) || changed;
		if ( y + h < item->y() + item->height() )
		    h = TQMAX( h, ih + item->textRect().height() );
		if ( item == end )
		    break;
		item = item->next;
	    }
	    y += h + d->spacing;
	} else {
	    // first calculate the row height
	    int h = begin->height();
	    int x = d->spacing;
	    int ih = begin->pixmapRect().height();
	    TQIconViewItem *item = begin;
	    int i = 0;
	    int sp = 0;
	    for (;;) {
		int r = calcGridNum( item->width(), d->rastX );
		if ( item == begin ) {
		    i += r;
		    sp += r;
		    x = d->spacing + d->rastX * r;
		} else {
		    sp += r;
		    i += r;
		    x = i * d->rastX + sp * d->spacing;
		}
		if ( x > visibleWidthSB() && item != begin ) {
		    item = item->prev;
		    while (item && (item->isVisible() == false)) {
			item = item->prev;
		    }
		    break;
		}
		h = TQMAX( h, item->height() );
		ih = TQMAX( ih, item->pixmapRect().height() );
		TQIconViewItem *old = item;
		item = item->next;
		while (item && (item->isVisible() == false)) {
		    item = item->next;
		}
		if ( !item ) {
		    item = old;
		    break;
		}
	    }
	    end = item;

	    if ( d->rastY != -1 )
		h = TQMAX( h, d->rastY );

	    // now move the items
	    item = begin;
	    i = 0;
	    sp = 0;
	    for (;;) {
		item->dirty = false;
		int r = calcGridNum( item->width(), d->rastX );
		if ( item == begin ) {
		    if ( d->itemTextPos == Bottom )
			changed = item->move( d->spacing + ( r * d->rastX - item->width() ) / 2,
					      y + ih - item->pixmapRect().height() ) || changed;
		    else
			changed = item->move( d->spacing, y + ih - item->pixmapRect().height() ) || changed;
		    i += r;
		    sp += r;
		} else {
		    sp += r;
		    int x = i * d->rastX + sp * d->spacing;
		    if ( d->itemTextPos == Bottom )
			changed = item->move( x + ( r * d->rastX - item->width() ) / 2,
					      y + ih - item->pixmapRect().height() ) || changed;
		    else
			changed = item->move( x, y + ih - item->pixmapRect().height() ) || changed;
		    i += r;
		}
		if ( y + h < item->y() + item->height() )
		    h = TQMAX( h, ih + item->textRect().height() );
		if ( item == end )
		    break;
		item = item->next;
		while (item && (item->isVisible() == false)) {
		    item = item->next;
		}
	    }
	    y += h + d->spacing;
	}


    } else { // -------------------------------- TopToBottom ------------------------------

	int x = y;

	{
	    int w = 0;
	    int y = 0;
	    TQIconViewItem *item = begin;
	    for (;;) {
		y += d->spacing + item->height();
		if ( y > visibleHeightSB() && item != begin ) {
		    item = item->prev;
		    while (item && (item->isVisible() == false)) {
			item = item->prev;
		    }
		    break;
		}
		w = TQMAX( w, item->width() );
		TQIconViewItem *old = item;
		item = item->next;
		while (item && (item->isVisible() == false)) {
		    item = item->next;
		}
		if ( !item ) {
		    item = old;
		    break;
		}
	    }
	    end = item;

	    if ( d->rastX != -1 )
		w = TQMAX( w, d->rastX );

	    // now move the items
	    item = begin;
	    TQIconViewItem *prevVisibleItem = NULL;
	    for (;;) {
		item->dirty = false;
		if ( d->itemTextPos == Bottom ) {
		    if ( item == begin )
			changed = item->move( x + ( w - item->width() ) / 2, d->spacing )  || changed;
		    else
			changed = item->move( x + ( w - item->width() ) / 2,
					      prevVisibleItem->y() + prevVisibleItem->height() + d->spacing ) || changed;
		} else {
		    if ( item == begin )
			changed = item->move( x, d->spacing ) || changed;
		    else
			changed = item->move( x, prevVisibleItem->y() + prevVisibleItem->height() + d->spacing ) || changed;
		}
		if ( item == end )
		    break;
		prevVisibleItem = item;
		item = item->next;
		while (item && (item->isVisible() == false)) {
		    item = item->next;
		}
	    }
	    x += w + d->spacing;
	}

	y = x;
    }

    return end;
}

/*!
  \internal
  Calculates how many cells an item of width \a w needs in a grid with of
  \a x and returns the result.
*/

int TQIconView::calcGridNum( int w, int x ) const
{
    float r = (float)w / (float)x;
    if ( ( w / x ) * x != w )
	r += 1.0;
    return (int)r;
}

/*!
  \internal
  Returns the first item of the row which contains \a item.
*/

TQIconViewItem *TQIconView::rowBegin( TQIconViewItem * ) const
{
    // #### todo
    return d->firstItem;
}

/*!
    Sorts and rearranges all the items in the icon view. If \a
    ascending is true, the items are sorted in increasing order,
    otherwise they are sorted in decreasing order.

    TQIconViewItem::compare() is used to compare pairs of items. The
    sorting is based on the items' keys; these default to the items'
    text unless specifically set to something else.

    Note that this function sets the sort order to \a ascending.

    \sa TQIconViewItem::key(), TQIconViewItem::setKey(),
    TQIconViewItem::compare(), TQIconView::setSorting(),
    TQIconView::sortDirection()
*/

void TQIconView::sort( bool ascending )
{
    if ( count() == 0 )
	return;

    d->sortDirection = ascending;
    TQIconViewPrivate::SortableItem *items = new TQIconViewPrivate::SortableItem[ count() ];

    TQIconViewItem *item = d->firstItem;
    int i = 0;
    for ( ; item; item = item->next )
	items[ i++ ].item = item;

    qsort( items, count(), sizeof( TQIconViewPrivate::SortableItem ), cmpIconViewItems );

    TQIconViewItem *prev = 0;
    item = 0;
    if ( ascending ) {
	for ( i = 0; i < (int)count(); ++i ) {
	    item = items[ i ].item;
	    if ( item ) {
		item->prev = prev;
		if ( item->prev )
		    item->prev->next = item;
		item->next = 0;
	    }
	    if ( i == 0 )
		d->firstItem = item;
	    if ( i == (int)count() - 1 )
		d->lastItem = item;
	    prev = item;
	}
    } else {
	for ( i = (int)count() - 1; i >= 0 ; --i ) {
	    item = items[ i ].item;
	    if ( item ) {
		item->prev = prev;
		if ( item->prev )
		    item->prev->next = item;
		item->next = 0;
	    }
	    if ( i == (int)count() - 1 )
		d->firstItem = item;
	    if ( i == 0 )
		d->lastItem = item;
	    prev = item;
	}
    }

    delete [] items;

    arrangeItemsInGrid( true );
}

/*!
    \reimp
*/

TQSize TQIconView::sizeHint() const
{
    constPolish();

    if ( !d->firstItem )
	return TQScrollView::sizeHint();

    if ( d->dirty && d->firstSizeHint ) {
	( (TQIconView*)this )->resizeContents( TQMAX( 400, contentsWidth() ),
					      TQMAX( 400, contentsHeight() ) );
	if ( autoArrange() )
	    ( (TQIconView*)this )->arrangeItemsInGrid( false );
	d->firstSizeHint = false;
    }

    d->dirty = true; // ######## warwick: I'm sure this is wrong. Fixed in 2.3.
    int extra = style().pixelMetric(TQStyle::PM_ScrollBarExtent,
				    verticalScrollBar()) + 2*frameWidth();
    TQSize s( TQMIN(400, contentsWidth() + extra),
	     TQMIN(400, contentsHeight() + extra) );
    return s;
}

/*!
  \internal
*/

void TQIconView::updateContents()
{
    viewport()->update();
}

/*!
    \reimp
*/

void TQIconView::enterEvent( TQEvent *e )
{
    TQScrollView::enterEvent( e );
    emit onViewport();
}

/*!
  \internal
  This function is always called when the geometry of an item changes.
  This function moves the item into the correct area in the internal
  data structure.
*/

void TQIconView::updateItemContainer( TQIconViewItem *item )
{
    if ( !item || d->containerUpdateLocked || (!isVisible() && autoArrange()) )
	return;

    if ( item->d->container1 && d->firstContainer ) {
	//Special-case to check if we can use removeLast otherwise use removeRef (slower)
	if (item->d->container1->items.last() == item)
	    item->d->container1->items.removeLast();
	else
	    item->d->container1->items.removeRef( item );
    }
    item->d->container1 = 0;
    if ( item->d->container2 && d->firstContainer ) {
	//Special-case to check if we can use removeLast otherwise use removeRef (slower)
	if (item->d->container2->items.last() == item)
	    item->d->container2->items.removeLast();
	else
	    item->d->container2->items.removeRef( item );
    }
    item->d->container2 = 0;

    TQIconViewPrivate::ItemContainer *c = d->firstContainer;
    if ( !c ) {
	appendItemContainer();
	c = d->firstContainer;
    }

    const TQRect irect = item->rect();
    bool contains = false;
    for (;;) {
	if ( c->rect.intersects( irect ) ) {
	    contains = c->rect.contains( irect );
	    break;
	}

	c = c->n;
	if ( !c ) {
	    appendItemContainer();
	    c = d->lastContainer;
	}
    }

    if ( !c ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQIconViewItem::updateItemContainer(): No fitting container found!" );
#endif
	return;
    }

    if (item->isVisible()) {
        c->items.append( item );
        item->d->container1 = c;

        if ( !contains ) {
	    c = c->n;
	    if ( !c ) {
	        appendItemContainer();
	        c = d->lastContainer;
	    }
	    c->items.append( item );
	    item->d->container2 = c;
        }
    }
    if ( contentsWidth() < irect.right() || contentsHeight() < irect.bottom() )
	resizeContents( TQMAX( contentsWidth(), irect.right() ), TQMAX( contentsHeight(), irect.bottom() ) );
}

/*!
  \internal
  Appends a new rect area to the internal data structure of the items.
*/

void TQIconView::appendItemContainer()
{
    TQSize s;
    // #### We have to find out which value is best here
    if ( d->arrangement == LeftToRight )
	s = TQSize( INT_MAX - 1, RECT_EXTENSION );
    else
	s = TQSize( RECT_EXTENSION, INT_MAX - 1 );

    if ( !d->firstContainer ) {
	d->firstContainer = new TQIconViewPrivate::ItemContainer( 0, 0, TQRect( TQPoint( 0, 0 ), s ) );
	d->lastContainer = d->firstContainer;
    } else {
	if ( d->arrangement == LeftToRight )
	    d->lastContainer = new TQIconViewPrivate::ItemContainer(
		d->lastContainer, 0, TQRect( d->lastContainer->rect.bottomLeft(), s ) );
	else
	    d->lastContainer = new TQIconViewPrivate::ItemContainer(
		d->lastContainer, 0, TQRect( d->lastContainer->rect.topRight(), s ) );
    }
}

/*!  \internal

  Rebuilds the whole internal data structure. This is done when it's
  likely that most/all items change their geometry (e.g. in
  arrangeItemsInGrid()), because calling this is then more efficient
  than calling updateItemContainer() for each item.
*/

void TQIconView::rebuildContainers()
{
    TQIconViewPrivate::ItemContainer *c = d->firstContainer, *tmpc;
    while ( c ) {
	tmpc = c->n;
	delete c;
	c = tmpc;
    }
    d->firstContainer = d->lastContainer = 0;

    TQIconViewItem *item = d->firstItem;
    appendItemContainer();
    c = d->lastContainer;
    while ( item ) {
        if (item->isVisible()) {
	    if ( c->rect.contains( item->rect() ) ) {
	        item->d->container1 = c;
	        item->d->container2 = 0;
	        c->items.append( item );
	        item = item->next;
	    } else if ( c->rect.intersects( item->rect() ) ) {
	        item->d->container1 = c;
	        c->items.append( item );
	        c = c->n;
	        if ( !c ) {
		    appendItemContainer();
		    c = d->lastContainer;
	        }
	        c->items.append( item );
	        item->d->container2 = c;
	        item = item->next;
	        c = c->p;
	    } else {
	        if ( d->arrangement == LeftToRight ) {
		    if ( item->y() < c->rect.y() && c->p ) {
		        c = c->p;
		        continue;
		    }
	        } else {
		    if ( item->x() < c->rect.x() && c->p ) {
		        c = c->p;
		        continue;
		    }
	        }

	        c = c->n;
	        if ( !c ) {
		    appendItemContainer();
		    c = d->lastContainer;
		}
	    }
	}
	else {
	    // Skip this hidden item
	    item = item->next;
	}
    }
}

/*!
  \internal
*/

void TQIconView::movedContents( int, int )
{
    if ( d->drawDragShapes ) {
	drawDragShapes( d->oldDragPos );
	d->oldDragPos = TQPoint( -1, -1 );
    }
}

void TQIconView::handleItemChange( TQIconViewItem *old, bool shift,
				  bool control, bool homeend )
{
    if ( d->selectionMode == Single ) {
	bool block = signalsBlocked();
	blockSignals( true );
	if ( old )
	    old->setSelected( false );
	blockSignals( block );
	d->currentItem->setSelected( true, true );
    } else if ( d->selectionMode == Extended ) {
	 if ( shift ) {
	    if ( !d->selectAnchor ) {
		if ( old && !old->selected && old->isSelectable() ) {
		    old->selected = true;
		    repaintItem( old );
		}
		d->currentItem->setSelected( true, true );
	    } else {
		TQIconViewItem *from = d->selectAnchor, *to = d->currentItem;
		if ( !from || !to )
		    return;

		// checking if it's downwards and if we span rows
		bool downwards = false;
		bool spanning = false;
		if ( d->arrangement == LeftToRight) {
		    if ( from->rect().center().y() < to->rect().center().y() )
			downwards = true;
		} else {
		    if ( from->rect().center().x() < to->rect().center().x() )
			downwards = true;
		}

 		TQRect fr = from->rect();
		TQRect tr = to->rect();
 		if ( d->arrangement == LeftToRight ) {
		    fr.moveTopLeft( TQPoint( tr.x(), fr.y() ) );
 		    if ( !tr.intersects( fr ) )
 			spanning = true;
 		} else {
		    fr.moveTopLeft( TQPoint( fr.x(), tr.y() ) );
 		    if ( !tr.intersects( fr ) )
 			spanning = true;
 		}


		// finding the rectangles
		TQRect topRect, bottomRect, midRect;
		if ( !spanning ) {
		    midRect = from->rect().unite( to->rect() );
		} else {
		    if ( downwards ) {
			topRect = from->rect();
			bottomRect = to->rect();
		    } else {
			topRect = to->rect();
			bottomRect = from->rect();
		    }
		    if ( d->arrangement == LeftToRight ) {
			topRect.setRight( contentsWidth() );
			bottomRect.setLeft( 0 );
			midRect.setRect( 0, topRect.bottom(),
					 contentsWidth(),
					 bottomRect.top() - topRect.bottom() );
		    } else {
			topRect.setBottom( contentsHeight() );
			bottomRect.setTop( 0 );
			midRect.setRect( topRect.right(),
					 0,
					 bottomRect.left() - topRect.right(),
					 contentsHeight() );
		    }
		}

		// finding contained items and selecting them
		TQIconViewItem *item = 0;
		bool changed = false;
		bool midValid = midRect.isValid();
		bool topValid = topRect.isValid();
		bool bottomValid = bottomRect.isValid();
		TQRect selectedRect, unselectedRect;
		for ( item = d->firstItem; item; item = item->next ) {
		    bool contained = false;
		    TQPoint itemCenter = item->rect().center();
		    if ( midValid && midRect.contains( itemCenter ) )
			contained = true;
		    if ( !contained && topValid && topRect.contains( itemCenter ) )
			contained = true;
		    if ( !contained && bottomValid && bottomRect.contains( itemCenter ) )
			contained = true;

		    if ( contained ) {
			if ( !item->selected && item->isSelectable() ) {
			    changed = true;
			    item->selected = true;
			    selectedRect = selectedRect.unite( item->rect() );
			}
		    } else if ( item->selected && !control ) {
			item->selected = false;
			unselectedRect = unselectedRect.unite( item->rect() );
			changed = true;
		    }
		}

		TQRect viewRect( contentsX(), contentsY(),
				visibleWidth(), visibleHeight() );

   		if ( viewRect.intersects( selectedRect ) ) {
		    if ( homeend )
			TQScrollView::updateContents( viewRect.intersect( selectedRect ) );
		    else
			repaintContents( viewRect.intersect( selectedRect ) );
		}
		if ( viewRect.intersects( unselectedRect ) ) {
		    if ( homeend )
			TQScrollView::updateContents( viewRect.intersect( unselectedRect ) );
		    else
			repaintContents( viewRect.intersect( unselectedRect ) );
		}

		if ( changed )
		    emit selectionChanged();
	    }
	} else if ( !control ) {
	    blockSignals( true );
	    selectAll( false );
	    blockSignals( false );
	    d->currentItem->setSelected( true, true );
	}
    } else {
	if ( shift )
	    d->currentItem->setSelected( !d->currentItem->isSelected(), true );
    }
}

TQBitmap TQIconView::mask( TQPixmap *pix ) const
{
    TQBitmap m;
    if ( d->maskCache.find( TQString::number( pix->serialNumber() ), m ) )
	return m;
    m = pix->createHeuristicMask();
    d->maskCache.insert( TQString::number( pix->serialNumber() ), m );
    return m;
}

/*!
    \reimp
    \internal

    (Implemented to get rid of a compiler warning.)
*/
void TQIconView::drawContents( TQPainter * )
{
}

/*!
    \reimp
*/
void TQIconView::windowActivationChange( bool oldActive )
{
    if ( oldActive && d->scrollTimer )
	d->scrollTimer->stop();

    if ( !isVisible() )
	return;

    if ( palette().active() == palette().inactive() )
	return;

    repaintSelectedItems();
}

/*!
    Returns true if an iconview item is being renamed; otherwise
    returns false.
*/

bool TQIconView::isRenaming() const
{
#ifndef TQT_NO_TEXTEDIT
    return d->renamingItem && d->renamingItem->renameBox;
#else
    return false;
#endif
}

int TQIconView::visibleWidthSB() const
{
    if ( vScrollBarMode() != Auto )
        return visibleWidth();

    int offset = verticalScrollBar()->isVisible() ? 0
        : style().pixelMetric( TQStyle::PM_ScrollBarExtent, verticalScrollBar() );
    return TQMAX( 0, visibleWidth() - offset );
}

int TQIconView::visibleHeightSB() const
{
    if ( hScrollBarMode() != Auto )
        return visibleHeight();

    int offset = horizontalScrollBar()->isVisible() ? 0
        : style().pixelMetric( TQStyle::PM_ScrollBarExtent, horizontalScrollBar() );
    return TQMAX( 0, visibleHeight() - offset );
}

#endif // TQT_NO_ICONVIEW
