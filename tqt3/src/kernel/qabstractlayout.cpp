/****************************************************************************
**
** Implementation of the abstract layout base class
**
** Created : 960416
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

#include "ntqlayout.h"

#ifndef TQT_NO_LAYOUT
#include "ntqapplication.h"
#include "qlayoutengine_p.h"
#include "ntqmenubar.h"
#include "ntqtoolbar.h"

static int menuBarHeightForWidth( TQMenuBar *menubar, int w )
{
#ifndef TQT_NO_MENUBAR
    if ( menubar && !menubar->isHidden() && !menubar->isTopLevel() )
	return menubar->heightForWidth( TQMAX(w, menubar->minimumWidth()) );
    else
#endif
	return 0;
}

/*!
    \class TQLayoutItem
    \ingroup appearance
    \ingroup geomanagement
    \brief The TQLayoutItem class provides an abstract item that a
    TQLayout manipulates.

    This is used by custom layouts.

    Pure virtual functions are provided to return information about
    the layout, including, sizeHint(), minimumSize(), maximumSize()
    and expanding().

    The layout's geometry can be set and retrieved with setGeometry()
    and geometry(), and its alignment with setAlignment() and
    alignment().

    isEmpty() returns whether the layout is empty. iterator() returns
    an iterator for the layout's children. If the concrete item is a
    TQWidget, it can be retrieved using widget(). Similarly for
    layout() and spacerItem().

    \sa TQLayout
*/

/*!
    \class TQSpacerItem
    \ingroup appearance
    \ingroup geomanagement
    \brief The TQSpacerItem class provides blank space in a layout.

    This class is used by custom layouts.

    \sa TQLayout TQLayout::spacerItem()
*/

/*!
    \class TQWidgetItem
    \ingroup appearance
    \ingroup geomanagement
    \brief The TQWidgetItem class is a layout item that represents a widget.

    This is used by custom layouts.

    \sa TQLayout TQLayout::widget()
*/

/*!
    \fn TQLayoutItem::TQLayoutItem( int alignment )

    Constructs a layout item with an \a alignment that is a bitwise OR
    of the \l{TQt::AlignmentFlags}. Not all subclasses support
    alignment.
*/

/*!
    \fn int TQLayoutItem::alignment() const

    Returns the alignment of this item.
*/

/*!
    Sets the alignment of this item to \a a, which is a bitwise OR of
    the \l{TQt::AlignmentFlags}. Not all subclasses support alignment.
*/
void TQLayoutItem::setAlignment( int a )
{
    align = a;
}

/*!
    \fn TQSize TQLayoutItem::maximumSize() const

    Implemented in subclasses to return the maximum size of this item.
*/

/*!
    \fn TQSize TQLayoutItem::minimumSize() const

    Implemented in subclasses to return the minimum size of this item.
*/

/*!
    \fn TQSize TQLayoutItem::sizeHint() const

    Implemented in subclasses to return the preferred size of this item.
*/

/*!
    \fn TQSizePolicy::ExpandData TQLayoutItem::expanding() const

    Implemented in subclasses to return the direction(s) this item
    "wants" to expand in (if any).
*/

/*!
    \fn void TQLayoutItem::setGeometry( const TQRect &r )

    Implemented in subclasses to set this item's geometry to \a r.
*/

/*!
    \fn TQRect TQLayoutItem::geometry() const

    Returns the rectangle covered by this layout item.
*/

/*!
    \fn virtual bool TQLayoutItem::isEmpty() const

    Implemented in subclasses to return whether this item is empty,
    i.e. whether it contains any widgets.
*/

/*!
    \fn TQSpacerItem::TQSpacerItem( int w, int h, TQSizePolicy::SizeType hData, TQSizePolicy::SizeType vData )

    Constructs a spacer item with preferred width \a w, preferred
    height \a h, horizontal size policy \a hData and vertical size
    policy \a vData.

    The default values provide a gap that is able to stretch if
    nothing else wants the space.
*/

/*!
    Changes this spacer item to have preferred width \a w, preferred
    height \a h, horizontal size policy \a hData and vertical size
    policy \a vData.

    The default values provide a gap that is able to stretch if
    nothing else wants the space.
*/
void TQSpacerItem::changeSize( int w, int h, TQSizePolicy::SizeType hData,
			      TQSizePolicy::SizeType vData )
{
    width = w;
    height = h;
    sizeP = TQSizePolicy( hData, vData );
}

/*!
    \fn TQWidgetItem::TQWidgetItem (TQWidget * w)

    Creates an item containing widget \a w.
*/

/*!
    Destroys the TQLayoutItem.
*/
TQLayoutItem::~TQLayoutItem()
{
}

/*!
    Invalidates any cached information in this layout item.
*/
void TQLayoutItem::invalidate()
{
}

/*!
    If this item is a TQLayout, it is returned as a TQLayout; otherwise
    0 is returned. This function provides type-safe casting.
*/
TQLayout * TQLayoutItem::layout()
{
    return 0;
}

/*!
    If this item is a TQSpacerItem, it is returned as a TQSpacerItem;
    otherwise 0 is returned. This function provides type-safe casting.
*/
TQSpacerItem * TQLayoutItem::spacerItem()
{
    return 0;
}

/*!
    \reimp
*/
TQLayout * TQLayout::layout()
{
    return this;
}

/*!
    \reimp
*/
TQSpacerItem * TQSpacerItem::spacerItem()
{
    return this;
}

/*!
    If this item is a TQWidget, it is returned as a TQWidget; otherwise
    0 is returned. This function provides type-safe casting.
*/
TQWidget * TQLayoutItem::widget()
{
    return 0;
}

/*!
    Returns the widget managed by this item.
*/
TQWidget * TQWidgetItem::widget()
{
    return wid;
}

/*!
    Returns true if this layout's preferred height depends on its
    width; otherwise returns false. The default implementation returns
    false.

    Reimplement this function in layout managers that support height
    for width.

    \sa heightForWidth(), TQWidget::heightForWidth()
*/
bool TQLayoutItem::hasHeightForWidth() const
{
    return false;
}

/*!
    Returns an iterator over this item's TQLayoutItem children. The
    default implementation returns an empty iterator.

    Reimplement this function in subclasses that can have children.
*/
TQLayoutIterator TQLayoutItem::iterator()
{
    return TQLayoutIterator( 0 );
}

/*!
    Returns the preferred height for this layout item, given the width
    \a w.

    The default implementation returns -1, indicating that the
    preferred height is independent of the width of the item. Using
    the function hasHeightForWidth() will typically be much faster
    than calling this function and testing for -1.

    Reimplement this function in layout managers that support height
    for width. A typical implementation will look like this:
    \code
	int MyLayout::heightForWidth( int w ) const
	{
	    if ( cache_dirty || cached_width != w ) {
		// not all C++ compilers support "mutable"
		MyLayout *that = (MyLayout*)this;
		int h = calculateHeightForWidth( w );
		that->cached_hfw = h;
		return h;
	    }
	    return cached_hfw;
	}
    \endcode

    Caching is strongly recommended; without it layout will take
    exponential time.

    \sa hasHeightForWidth()
*/
int TQLayoutItem::heightForWidth( int /* w */ ) const
{
    return -1;
}

/*!
    Stores the spacer item's rect \a r so that it can be returned by
    geometry().
*/
void TQSpacerItem::setGeometry( const TQRect &r )
{
    rect = r;
}

/*!
    Sets the geometry of this item's widget to be contained within
    rect \a r, taking alignment and maximum size into account.
*/
void TQWidgetItem::setGeometry( const TQRect &r )
{
    TQSize s = r.size().boundedTo( qSmartMaxSize( this ) );
    int x = r.x();
    int y = r.y();
    if ( align & (TQt::AlignHorizontal_Mask | TQt::AlignVertical_Mask) ) {
	TQSize pref = wid->sizeHint().expandedTo( wid->minimumSize() ); //###
	if ( align & TQt::AlignHorizontal_Mask )
	    s.setWidth( TQMIN( s.width(), pref.width() ) );
	if ( align & TQt::AlignVertical_Mask ) {
	    if ( hasHeightForWidth() )
		s.setHeight( TQMIN( s.height(), heightForWidth(s.width()) ) );
	    else
		s.setHeight( TQMIN( s.height(), pref.height() ) );
	}
    }
    int alignHoriz = TQApplication::horizontalAlignment( align );
    if ( alignHoriz & TQt::AlignRight )
	x = x + ( r.width() - s.width() );
    else if ( !(alignHoriz & TQt::AlignLeft) )
	x = x + ( r.width() - s.width() ) / 2;

    if ( align & TQt::AlignBottom )
	y = y + ( r.height() - s.height() );
    else if ( !(align & TQt::AlignTop) )
	y = y + ( r.height() - s.height() ) / 2;

    if ( !isEmpty() )
	wid->setGeometry( x, y, s.width(), s.height() );
}

/*!
    \reimp
*/
TQRect TQSpacerItem::geometry() const
{
    return rect;
}

/*!
    \reimp
*/
TQRect TQWidgetItem::geometry() const
{
    return wid->geometry();
}

/*!
    \reimp
*/
TQRect TQLayout::geometry() const
{
    return rect;
}

/*!
    \reimp
*/
bool TQWidgetItem::hasHeightForWidth() const
{
    if ( isEmpty() )
	return false;
    if ( wid->layout() )
	return wid->layout()->hasHeightForWidth();
    return wid->sizePolicy().hasHeightForWidth();
}

/*!
    \reimp
*/
int TQWidgetItem::heightForWidth( int w ) const
{
    if ( isEmpty() )
	return -1;
    int hfw;
    if ( wid->layout() )
	hfw = wid->layout()->totalHeightForWidth( w );
    else
	hfw = wid->heightForWidth( w );

    if ( hfw > wid->maximumHeight() )
	hfw = wid->maximumHeight();
    if ( hfw < wid->minimumHeight() )
	hfw = wid->minimumHeight();
    if ( hfw < 1 )
	hfw = 1;
    return hfw;
}

/*!
    Returns the direction in which this spacer item will expand.

    \sa TQSizePolicy::ExpandData
*/
TQSizePolicy::ExpandData TQSpacerItem::expanding() const
{
    return sizeP.expanding();
}

/*!
    Returns whether this item's widget can make use of more space than
    sizeHint(). A value of \c Vertical or \c Horizontal means that it wants
    to grow in only one dimension, whereas \c BothDirections means that
    it wants to grow in both dimensions and \c NoDirection means that
    it doesn't want to grow at all.
*/
TQSizePolicy::ExpandData TQWidgetItem::expanding() const
{
    if ( isEmpty() )
	return TQSizePolicy::NoDirection;

    int e = wid->sizePolicy().expanding();
    /*
      If the layout is expanding, we make the widget expanding, even if
      its own size policy isn't expanding. This behavior should be
      reconsidered in TQt 4.0. (###)
    */
    if ( wid->layout() ) {
	if ( wid->sizePolicy().mayGrowHorizontally()
		&& (wid->layout()->expanding() & TQSizePolicy::Horizontally) )
	    e |= TQSizePolicy::Horizontally;
	if ( wid->sizePolicy().mayGrowVertically()
		&& (wid->layout()->expanding() & TQSizePolicy::Vertically) )
	    e |= TQSizePolicy::Vertically;
    }

    if ( align & TQt::AlignHorizontal_Mask )
	e &= ~TQSizePolicy::Horizontally;
    if ( align & TQt::AlignVertical_Mask)
	e &= ~TQSizePolicy::Vertically;
    return (TQSizePolicy::ExpandData)e;
}

/*!
    Returns the minimum size of this spacer item.
*/
TQSize TQSpacerItem::minimumSize() const
{
    return TQSize( sizeP.mayShrinkHorizontally() ? 0 : width,
		  sizeP.mayShrinkVertically() ? 0 : height );
}

/*!
    Returns the minimum size of this item.
*/
TQSize TQWidgetItem::minimumSize() const
{
    if ( isEmpty() )
	return TQSize( 0, 0 );
    return qSmartMinSize( this );
}

/*!
    Returns the maximum size of this spacer item.
*/
TQSize TQSpacerItem::maximumSize() const
{
    return TQSize( sizeP.mayGrowHorizontally() ? TQLAYOUTSIZE_MAX : width,
		  sizeP.mayGrowVertically() ? TQLAYOUTSIZE_MAX : height );
}

/*!
    Returns the maximum size of this item.
*/
TQSize TQWidgetItem::maximumSize() const
{
    if ( isEmpty() ) {
	return TQSize( 0, 0 );
    } else {
	return qSmartMaxSize( this, align );
    }
}

/*!
    Returns the preferred size of this spacer item.
*/
TQSize TQSpacerItem::sizeHint() const
{
    return TQSize( width, height );
}

/*!
    Returns the preferred size of this item.
*/
TQSize TQWidgetItem::sizeHint() const
{
    TQSize s;
    if ( isEmpty() ) {
	s = TQSize( 0, 0 );
    } else {
	s = wid->sizeHint();
	if ( wid->sizePolicy().horData() == TQSizePolicy::Ignored )
	    s.setWidth( 1 );
	if ( wid->sizePolicy().verData() == TQSizePolicy::Ignored )
	    s.setHeight( 1 );
	s = s.boundedTo( wid->maximumSize() )
	    .expandedTo( wid->minimumSize() ).expandedTo( TQSize(1, 1) );
    }
    return s;
}

/*!
    Returns true because a spacer item never contains widgets.
*/
bool TQSpacerItem::isEmpty() const
{
    return true;
}

/*!
    Returns true if the widget has been hidden; otherwise returns
    false.
*/
bool TQWidgetItem::isEmpty() const
{
    return wid->isHidden() || wid->isTopLevel();
}

/*!
    \class TQLayout
    \brief The TQLayout class is the base class of geometry managers.

    \ingroup appearance
    \ingroup geomanagement

    This is an abstract base class inherited by the concrete classes,
    TQBoxLayout and TQGridLayout.

    For users of TQLayout subclasses or of TQMainWindow there is seldom
    any need to use the basic functions provided by TQLayout, such as
    \l setResizeMode() or setMenuBar(). See the \link layout.html layout
    overview page \endlink for more information.

    To make your own layout manager, subclass TQGLayoutIterator and
    implement the functions addItem(), sizeHint(), setGeometry(), and
    iterator(). You should also implement minimumSize() to ensure your
    layout isn't resized to zero size if there is too little space. To
    support children whose heights depend on their widths, implement
    hasHeightForWidth() and heightForWidth(). See the \link
    customlayout.html custom layout page \endlink for an in-depth
    description.

    Geometry management stops when the layout manager is deleted.
*/

/*!
    Constructs a new top-level TQLayout called \a name, with main
    widget \a parent. \a parent may not be 0.

    The \a margin is the number of pixels between the edge of the
    widget and the managed children. The \a spacing sets the value of
    spacing(), which gives the spacing between the managed widgets. If
    \a spacing is -1 (the default), spacing is set to the value of \a
    margin.

    There can be only one top-level layout for a widget. It is
    returned by TQWidget::layout()
*/
TQLayout::TQLayout( TQWidget *parent, int margin, int spacing, const char *name )
    : TQObject( parent, name )
{
    init();
    if ( parent ) {
	if ( parent->layout() ) {
	    tqWarning( "TQLayout \"%s\" added to %s \"%s\", which already has a"
		      " layout", TQObject::name(), parent->className(),
		      parent->name() );
	    parent->removeChild( this );
	} else {
	    topLevel = true;
	    parent->installEventFilter( this );
	    setWidgetLayout( parent, this );
	}
    }
    outsideBorder = margin;
    if ( spacing < 0 )
	insideSpacing = margin;
    else
	insideSpacing = spacing;
}

void TQLayout::init()
{
    insideSpacing = 0;
    outsideBorder = 0;
    topLevel = false;
    enabled = true;
    autoNewChild = false;
    frozen = false;
    activated = false;
    marginImpl = false;
    autoMinimum = false;
    autoResizeMode = true;
    extraData = 0;
#ifndef TQT_NO_MENUBAR
    menubar = 0;
#endif
}

/*!
    Constructs a new child TQLayout called \a name, and places it
    inside \a parentLayout by using the default placement defined by
    addItem().

    If \a spacing is -1, this TQLayout inherits \a parentLayout's
    spacing(), otherwise the value of \a spacing is used.
*/
TQLayout::TQLayout( TQLayout *parentLayout, int spacing, const char *name )
    : TQObject( parentLayout, name )

{
    init();
    insideSpacing = spacing < 0 ? parentLayout->insideSpacing : spacing;
    parentLayout->addItem( this );
}

/*!
    Constructs a new child TQLayout called \a name. If \a spacing is
    -1, this TQLayout inherits its parent's spacing(); otherwise the
    value of \a spacing is used.

    This layout has to be inserted into another layout before geometry
    management will work.
*/
TQLayout::TQLayout( int spacing, const char *name )
    : TQObject( 0, name )
{
    init();
    insideSpacing = spacing;
}

/*!
    \fn void TQLayout::addItem( TQLayoutItem *item )

    Implemented in subclasses to add an \a item. How it is added is
    specific to each subclass.

    The ownership of \a item is transferred to the layout, and it's
    the layout's responsibility to delete it.
*/

/*!
    \fn TQLayoutIterator TQLayout::iterator()

    Implemented in subclasses to return an iterator that iterates over
    this layout's children.

    A typical implementation will be:
    \code
	TQLayoutIterator MyLayout::iterator()
	{
	    TQGLayoutIterator *i = new MyLayoutIterator( internal_data );
	    return TQLayoutIterator( i );
	}
    \endcode
    where MyLayoutIterator is a subclass of TQGLayoutIterator.
*/

/*!
    \fn void TQLayout::add( TQWidget *w )

    Adds widget \a w to this layout in a manner specific to the
    layout. This function uses addItem().
*/

/*!
    \fn TQMenuBar* TQLayout::menuBar () const

    Returns the menu bar set for this layout, or 0 if no menu bar is
    set.
*/

/*!
    \fn bool TQLayout::isTopLevel () const

    Returns true if this layout is a top-level layout, i.e. not a
    child of another layout; otherwise returns false.
*/

/*!
    \property TQLayout::margin
    \brief the width of the outside border of the layout

    For some layout classes this property has an effect only on
    top-level layouts; TQBoxLayout and TQGridLayout support margins for
    child layouts. The default value is 0.

    \sa spacing
*/

/*!
    \property TQLayout::spacing
    \brief the spacing between widgets inside the layout

    The default value is -1, which signifies that the layout's spacing
    should not override the widget's spacing.

    \sa margin
*/
void TQLayout::setMargin( int margin )
{
    outsideBorder = margin;
    invalidate();
    if ( mainWidget() ) {
	TQEvent *lh = new TQEvent( TQEvent::LayoutHint );
	TQApplication::postEvent( mainWidget(), lh );
    }
}

void TQLayout::setSpacing( int spacing )
{
    insideSpacing = spacing;
    if ( spacing >= 0 )
	propagateSpacing( this );
    invalidate();
    if ( mainWidget() ) {
	TQEvent *lh = new TQEvent( TQEvent::LayoutHint );
	TQApplication::postEvent( mainWidget(), lh );
    }
}

/*!
    Returns the main widget (parent widget) of this layout, or 0 if
    this layout is a sub-layout that is not yet inserted.
*/
TQWidget *TQLayout::mainWidget()
{
    if ( !topLevel ) {
	if ( parent() ) {
	    TQLayout *parentLayout = ::tqt_cast<TQLayout*>(parent());
	    Q_ASSERT(parentLayout);
	    return parentLayout->mainWidget();
	} else {
	    return 0;
	}
    } else {
	Q_ASSERT(parent() && parent()->isWidgetType());
	return (TQWidget*)parent();
    }
}

/*!
    Returns true if this layout is empty. The default implementation
    returns false.
*/
bool TQLayout::isEmpty() const
{
    return false; //### should check
}

/*!
    Sets widget \a w's layout to layout \a l.
*/
void TQLayout::setWidgetLayout( TQWidget *w, TQLayout *l )
{
    w->setLayout( l );
}

/*!
    This function is reimplemented in subclasses to perform layout.

    The default implementation maintains the geometry() information
    given by rect \a r. Reimplementors must call this function.
*/
void TQLayout::setGeometry( const TQRect &r )
{
    rect = r;
}

/*!
    Invalidates cached information. Reimplementations must call this.
*/
void TQLayout::invalidate()
{
    rect = TQRect();
}

static bool removeWidgetRecursively( TQLayoutItem *lay, TQWidget *w )
{
    bool didSomething = false;
    TQLayoutIterator it = lay->iterator();
    TQLayoutItem *child;
    while ( (child = it.current()) != 0 ) {
	if ( child->widget() == w ) {
	    it.deleteCurrent();
	    lay->invalidate(); // maybe redundant
	    didSomething = true;
	} else if ( removeWidgetRecursively(child, w) ) {
	    lay->invalidate(); // maybe redundant
	    didSomething = true;
	} else {
	    ++it;
	}
    }
    return didSomething;
}

/*!
    \reimp
    Performs child widget layout when the parent widget is resized.
    Also handles removal of widgets and child layouts. \a e is the
    event the occurred on object \a o.
*/
bool TQLayout::eventFilter( TQObject *o, TQEvent *e )
{
    if ( !enabled )
	return false;

    if ( !o->isWidgetType() )
	return false;

    switch ( e->type() ) {
    case TQEvent::Resize:
	if ( activated ) {
	    TQResizeEvent *r = (TQResizeEvent *)e;
	    int mbh = 0;
#ifndef TQT_NO_MENUBAR
	    mbh = menuBarHeightForWidth( menubar, r->size().width() );
#endif
	    int b = marginImpl ? 0 : outsideBorder;
	    setGeometry( TQRect( b, mbh + b, r->size().width() - 2 * b,
				r->size().height() - mbh - 2 * b ) );
	} else {
	    activate();
	}
	break;
    case TQEvent::ChildRemoved:
	{
	    TQChildEvent *c = (TQChildEvent *)e;
	    if ( c->child()->isWidgetType() ) {
		TQWidget *w = (TQWidget *)c->child();
#ifndef TQT_NO_MENUBAR
		if ( w == menubar )
		    menubar = 0;
#endif
		if ( removeWidgetRecursively( this, w ) ) {
		    TQEvent *lh = new TQEvent( TQEvent::LayoutHint );
		    TQApplication::postEvent( o, lh );
		}
	    }
	}
	break;
    case TQEvent::ChildInserted:
	if ( topLevel && autoNewChild ) {
	    TQChildEvent *c = (TQChildEvent *)e;
	    if ( c->child()->isWidgetType() ) {
		TQWidget *w = (TQWidget *)c->child();
		if ( !w->isTopLevel() ) {
#if !defined(TQT_NO_MENUBAR) && !defined(TQT_NO_TOOLBAR)
		    if ( ::tqt_cast<TQMenuBar*>(w) && !::tqt_cast<TQToolBar*>(w->parentWidget()) )
			menubar = (TQMenuBar *)w;
		    else
#endif
			addItem( new TQWidgetItem( w ) );
		    TQEvent *lh = new TQEvent( TQEvent::LayoutHint );
		    TQApplication::postEvent( o, lh );
		}
	    }
	}
	break;
    case TQEvent::LayoutHint:
	activate();
	break;
    default:
	break;
    }
    return TQObject::eventFilter( o, e );
}

/*!
    \reimp
*/
void TQLayout::childEvent( TQChildEvent *e )
{
    if ( !enabled )
	return;

    if ( e->type() == TQEvent::ChildRemoved ) {
	TQChildEvent *c = (TQChildEvent*)e;
	TQLayoutIterator it = iterator();
	TQLayoutItem *item;
	while ( (item = it.current() ) ) {
	    if ( item == (TQLayout*)c->child() ) {
		it.takeCurrent();
		invalidate();
		break;
	    } else {
		++it;
	    }
	}
    }
}

/*!
  \internal
  Also takes margin() and menu bar into account.
*/
int TQLayout::totalHeightForWidth( int w ) const
{
    if ( topLevel ) {
	TQWidget *mw = (TQWidget*)parent();
	if ( mw && !mw->testWState(WState_Polished) ) {
	    mw->polish();
	}
    }
    int b = ( topLevel && !marginImpl ) ? 2 * outsideBorder : 0;
    int h = heightForWidth( w - b ) + b;
#ifndef TQT_NO_MENUBAR
    h += menuBarHeightForWidth( menubar, w );
#endif
    return h;
}

/*!
  \internal
  Also takes margin() and menu bar into account.
*/
TQSize TQLayout::totalMinimumSize() const
{
    if ( topLevel ) {
	TQWidget *mw = (TQWidget*)parent();
	if ( mw && !mw->testWState(WState_Polished) )
	    mw->polish();
    }
    int b = ( topLevel && !marginImpl ) ? 2 * outsideBorder : 0;

    TQSize s = minimumSize();
    int h = b;
#ifndef TQT_NO_MENUBAR
    h += menuBarHeightForWidth( menubar, s.width() );
#endif
    return s + TQSize( b, h );
}

/*!
  \internal
  Also takes margin() and menu bar into account.
*/
TQSize TQLayout::totalSizeHint() const
{
    if ( topLevel ) {
	TQWidget *mw = (TQWidget*)parent();
	if ( mw && !mw->testWState(WState_Polished) )
	    mw->polish();
    }
    int b = ( topLevel && !marginImpl ) ? 2 * outsideBorder : 0;

    TQSize s = sizeHint();
    if ( hasHeightForWidth() )
	s.setHeight( heightForWidth(s.width()) );
    int h = b;
#ifndef TQT_NO_MENUBAR
    h += menuBarHeightForWidth( menubar, s.width() );
#endif
    return s + TQSize( b, h );
}

/*!
  \internal
  Also takes margin() and menu bar into account.
*/
TQSize TQLayout::totalMaximumSize() const
{
    if ( topLevel ) {
	TQWidget *mw = (TQWidget*)parent();
	if ( mw && !mw->testWState(WState_Polished) ) {
	    mw->polish();
	}
    }
    int b = ( topLevel && !marginImpl ) ? 2 * outsideBorder : 0;

    TQSize s = maximumSize();
    int h = b;
#ifndef TQT_NO_MENUBAR
    h += menuBarHeightForWidth( menubar, s.width() );
#endif

    if ( isTopLevel() )
	s = TQSize( TQMIN( s.width() + b, TQLAYOUTSIZE_MAX ),
		   TQMIN( s.height() + h, TQLAYOUTSIZE_MAX ) );
    return s;
}

/*!
  \internal
  Destroys the layout, deleting all child layouts.
  Geometry management stops when a top-level layout is deleted.

  The layout classes will probably be fatally confused if you delete
  a sublayout.
*/
TQLayout::~TQLayout()
{
    /*
      This function may be called during the TQObject destructor,
      when the parent no longer is a TQWidget.
    */
    if ( isTopLevel() && parent() && parent()->isWidgetType() &&
	 ((TQWidget*)parent())->layout() == this )
	setWidgetLayout( (TQWidget*)parent(), 0 );
}

/*!
    Removes and deletes all items in this layout.
*/
void TQLayout::deleteAllItems()
{
    TQLayoutIterator it = iterator();
    TQLayoutItem *l;
    while ( (l = it.takeCurrent()) )
	delete l;
}

/*!
    This function is called from addLayout() functions in subclasses
    to add layout \a l as a sub-layout.
*/
void TQLayout::addChildLayout( TQLayout *l )
{
    if ( l->parent() ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQLayout::addChildLayout: layout already has a parent" );
#endif
	return;
    }
    insertChild( l );
    if ( l->insideSpacing < 0 ) {
	l->insideSpacing = insideSpacing;
	propagateSpacing( l );
    }
}

/*! \fn int TQLayout::defaultBorder() const

  \internal
*/

/*! \fn void TQLayout::freeze()

  \internal
*/

/*!
  \internal
  Fixes the size of the main widget and distributes the available
  space to the child widgets. For widgets which should not be
  resizable, but where a TQLayout subclass is used to set up the initial
  geometry.

  As a special case, freeze(0, 0) is equivalent to setResizeMode(Fixed).
*/
void TQLayout::freeze( int w, int h )
{
    if ( w <= 0 || h <= 0 ) {
	setResizeMode( Fixed );
    } else {
	setResizeMode( FreeResize ); // layout will not change min/max size
	mainWidget()->setFixedSize( w, h );
    }
}

#ifndef TQT_NO_MENUBAR

/*!
    Makes the geometry manager take account of the menu bar \a w. All
    child widgets are placed below the bottom edge of the menu bar.

    A menu bar does its own geometry management: never do addWidget()
    on a TQMenuBar.
*/
void TQLayout::setMenuBar( TQMenuBar *w )
{
    menubar = w;
}

#endif

/*!
    Returns the minimum size of this layout. This is the smallest size
    that the layout can have while still respecting the
    specifications. Does not include what's needed by margin() or
    menuBar().

    The default implementation allows unlimited resizing.
*/
TQSize TQLayout::minimumSize() const
{
    return TQSize( 0, 0 );
}

/*!
    Returns the maximum size of this layout. This is the largest size
    that the layout can have while still respecting the
    specifications. Does not include what's needed by margin() or
    menuBar().

    The default implementation allows unlimited resizing.
*/
TQSize TQLayout::maximumSize() const
{
    return TQSize( TQLAYOUTSIZE_MAX, TQLAYOUTSIZE_MAX );
}

/*!
    Returns whether this layout can make use of more space than
    sizeHint(). A value of \c Vertical or \c Horizontal means that it wants
    to grow in only one dimension, whereas \c BothDirections means that
    it wants to grow in both dimensions.

    The default implementation returns \c BothDirections.
*/
TQSizePolicy::ExpandData TQLayout::expanding() const
{
    return TQSizePolicy::BothDirections;
}

static void invalidateRecursive( TQLayoutItem *lay )
{
    lay->invalidate();
    TQLayoutIterator it = lay->iterator();
    TQLayoutItem *child;
    while ( (child = it.current()) != 0 ) {
	invalidateRecursive( child );
	++it;
    }
}

/*!
    Redoes the layout for mainWidget(). You should generally not need
    to call this because it is automatically called at the most
    appropriate times.

    However, if you set up a TQLayout for a visible widget without
    resizing that widget, you will need to call this function in order
    to lay it out.

    \sa TQWidget::updateGeometry()
*/
bool TQLayout::activate()
{
    invalidateRecursive( this );
    if ( !topLevel )
	return false;

    TQWidget *mw = mainWidget();
    if (!mw) {
#if defined( QT_CHECK_NULL )
	tqWarning( "TQLayout::activate: %s \"%s\" does not have a main widget",
		  TQObject::className(), TQObject::name() );
#endif
	return false;
    }
    activated = true;
    TQSize s = mw->size();
    TQSize ms;
    int mbh = 0;
#ifndef TQT_NO_MENUBAR
    mbh = menuBarHeightForWidth( menubar, s.width() );
#endif
    int b = marginImpl ? 0 : outsideBorder;
    setGeometry(TQRect(b, mbh + b, s.width() - 2 * b, s.height() - mbh - 2 * b));
    if ( frozen ) {
	// will trigger resize
	mw->setFixedSize( totalSizeHint() );
    } else if ( autoMinimum ) {
	ms = totalMinimumSize();
    } else if ( autoResizeMode && topLevel && mw->isTopLevel() ) {
	ms = totalMinimumSize();
	if ( hasHeightForWidth() ) {
	    int h;
	    int mmbh = menuBarHeightForWidth( menubar, ms.width() );
	    // ### 4.0: remove this 'if' when minimumHeightForWidth() is virtual
	    if ( inherits("TQBoxLayout") ) {
		h = ((TQBoxLayout *) this)->minimumHeightForWidth( ms.width() );
	    } else if ( inherits("TQGridLayout") ) {
		h = ((TQGridLayout *) this)->minimumHeightForWidth( ms.width() );
	    } else {
		h = heightForWidth( ms.width() );
	    }
	    if ( h + mmbh > ms.height() )
#if 1
                //old behaviour:
 		ms = TQSize( 0, 0 );
#else
            //better, but too big a change for a patch release in a stable branch:
		ms.setHeight( 0 );
#endif
	}
    }

    if (ms.isValid())
	mw->setMinimumSize( ms );

    // ideally only if sizeHint() or sizePolicy() has changed
    mw->updateGeometry();
    return true;
}

/*!
    \class TQSizePolicy
    \brief The TQSizePolicy class is a layout attribute describing horizontal
    and vertical resizing policy.

    \ingroup appearance
    \ingroup geomanagement

    The size policy of a widget is an expression of its willingness to
    be resized in various ways.

    Widgets that reimplement TQWidget::sizePolicy() return a TQSizePolicy
    that describes the horizontal and vertical resizing policy they
    prefer when being laid out. Only \link #interesting one of the
    constructors\endlink is of interest in most applications.

    TQSizePolicy contains two independent SizeType objects; one describes
    the widgets's horizontal size policy, and the other describes its
    vertical size policy. It also contains a flag to indicate whether the
    height and width of its preferred size are related.

    The horizontal and vertical \l{SizeType}s are set in the usual constructor
    and can be queried using a variety of functions.

    The hasHeightForWidth() flag indicates whether the widget's sizeHint()
    is width-dependent (such as a word-wrapping label) or not.

    \sa TQSizePolicy::SizeType
*/

/*!
    \enum TQSizePolicy::SizeType

    The per-dimension sizing types used when constructing a
    TQSizePolicy are:

    \value Fixed  The TQWidget::sizeHint() is the only acceptable
    alternative, so the widget can never grow or shrink (e.g. the
    vertical direction of a push button).

    \value Minimum  The sizeHint() is minimal, and sufficient. The
    widget can be expanded, but there is no advantage to it being
    larger (e.g. the horizontal direction of a push button).
    It cannot be smaller than the size provided by sizeHint().

    \value Maximum  The sizeHint() is a maximum. The widget can be
    shrunk any amount without detriment if other widgets need the
    space (e.g. a separator line).
    It cannot be larger than the size provided by sizeHint().

    \value Preferred  The sizeHint() is best, but the widget can be
    shrunk and still be useful. The widget can be expanded, but there
    is no advantage to it being larger than sizeHint() (the default
    TQWidget policy).

    \value Expanding  The sizeHint() is a sensible size, but the
    widget can be shrunk and still be useful. The widget can make use
    of extra space, so it should get as much space as possible (e.g.
    the horizontal direction of a slider).

    \value MinimumExpanding  The sizeHint() is minimal, and sufficient.
    The widget can make use of extra space, so it should get as much
    space as possible (e.g. the horizontal direction of a slider).

    \value Ignored the sizeHint() is ignored. The widget will get as
    much space as possible.
*/

/*!
    \enum TQSizePolicy::ExpandData

    This enum type describes in which directions a widget can make use
    of extra space. There are four possible values:

    \value NoDirection  the widget cannot make use of extra space in
    any direction.

    \value Horizontally  the widget can usefully be wider than the
    sizeHint().

    \value Vertically  the widget can usefully be taller than the
    sizeHint().

    \value BothDirections  the widget can usefully be both wider and
    taller than the sizeHint().
*/

/*!
    \fn TQSizePolicy::TQSizePolicy()

    Constructs a minimally initialized TQSizePolicy.
*/

/*!
    \fn TQSizePolicy::TQSizePolicy( SizeType hor, SizeType ver, bool hfw )

    \target interesting
    This is the constructor normally used to return a value in the
    overridden \l TQWidget::sizePolicy() function of a TQWidget
    subclass.

    It constructs a TQSizePolicy with independent horizontal and
    vertical sizing types, \a hor and \a ver respectively. These \link
    TQSizePolicy::SizeType sizing types\endlink affect how the widget
    is treated by the \link TQLayout layout engine\endlink.

    If \a hfw is true, the preferred height of the widget is dependent
    on the width of the widget (for example, a TQLabel with line
    wrapping).

    \sa horData() verData() hasHeightForWidth()
*/

/*!
    \fn TQSizePolicy::TQSizePolicy( SizeType hor, SizeType ver, uchar horStretch, uchar verStretch, bool hfw )

    Constructs a TQSizePolicy with independent horizontal and vertical
    sizing types \a hor and \a ver, and stretch factors \a horStretch
    and \a verStretch.

    If \a hfw is true, the preferred height of the widget is dependent on the
    width of the widget.

    \sa horStretch() verStretch()
*/

/*!
    \fn TQSizePolicy::SizeType TQSizePolicy::horData() const

    Returns the horizontal component of the size policy.

    \sa setHorData() verData() horStretch()
*/

/*!
    \fn TQSizePolicy::SizeType TQSizePolicy::verData() const

    Returns the vertical component of the size policy.

    \sa setVerData() horData() verStretch()
*/

/*!
    \fn bool TQSizePolicy::mayShrinkHorizontally() const

    Returns true if the widget can sensibly be narrower than its
    sizeHint(); otherwise returns false.

    \sa mayShrinkVertically() mayGrowHorizontally()
*/

/*!
    \fn bool TQSizePolicy::mayShrinkVertically() const

    Returns true if the widget can sensibly be shorter than its
    sizeHint(); otherwise returns false.

    \sa mayShrinkHorizontally() mayGrowVertically()
*/

/*!
    \fn bool TQSizePolicy::mayGrowHorizontally() const

    Returns true if the widget can sensibly be wider than its
    sizeHint(); otherwise returns false.

    \sa mayGrowVertically() mayShrinkHorizontally()
*/

/*!
    \fn bool TQSizePolicy::mayGrowVertically() const

    Returns true if the widget can sensibly be taller than its
    sizeHint(); otherwise returns false.

    \sa mayGrowHorizontally() mayShrinkVertically()
*/

/*!
    \fn TQSizePolicy::ExpandData TQSizePolicy::expanding() const

    Returns whether this layout can make use of more space than
    sizeHint(). A value of \c Vertical or \c Horizontal means that it wants
    to grow in only one dimension, whereas \c BothDirections means that
    it wants to grow in both dimensions.

    \sa mayShrinkHorizontally() mayGrowHorizontally()
	mayShrinkVertically() mayGrowVertically()
*/

/*!
    \fn void TQSizePolicy::setHorData( SizeType d )

    Sets the horizontal component of the size policy to size type \a
    d.

    \sa horData() setVerData()
*/

/*!
    \fn void TQSizePolicy::setVerData( SizeType d )

    Sets the vertical component of the size policy to size type \a d.

    \sa verData() setHorData()
*/

/*!
    \fn bool TQSizePolicy::hasHeightForWidth() const

    Returns true if the widget's preferred height depends on its
    width; otherwise returns false.

    \sa setHeightForWidth()
*/

/*!
    \fn void TQSizePolicy::setHeightForWidth( bool b )

    Sets the hasHeightForWidth() flag to \a b.

    \sa hasHeightForWidth()
*/

/*!
    \fn uint TQSizePolicy::horStretch() const

    Returns the horizontal stretch factor of the size policy.

    \sa setHorStretch() verStretch()
*/

/*!
    \fn uint TQSizePolicy::verStretch() const

    Returns the vertical stretch factor of the size policy.

    \sa setVerStretch() horStretch()
*/

/*!
    \fn void TQSizePolicy::setHorStretch( uchar sf )

    Sets the horizontal stretch factor of the size policy to \a sf.

    \sa horStretch() setVerStretch()
*/

/*!
    \fn void TQSizePolicy::setVerStretch( uchar sf )

    Sets the vertical stretch factor of the size policy to \a sf.

    \sa verStretch() setHorStretch()
*/

/*!
    \fn void TQSizePolicy::transpose()

    Swaps the horizontal and vertical policies and stretches.
*/


/*!
    \fn bool TQSizePolicy::operator==( const TQSizePolicy &s ) const

    Returns true if this policy is equal to \a s; otherwise returns
    false.

    \sa operator!=()
*/

/*!
    \fn bool TQSizePolicy::operator!=( const TQSizePolicy &s ) const

    Returns true if this policy is different from \a s; otherwise
    returns false.

    \sa operator==()
*/

/*!
    \class TQGLayoutIterator
    \brief The TQGLayoutIterator class is an abstract base class of internal layout iterators.

    \ingroup appearance
    \ingroup geomanagement

    (This class is \e not OpenGL related, it just happens to start with
    the letters TQGL...)

    Subclass this class to create a custom layout. The functions that
    must be implemented are next(), current(), and takeCurrent().

    The TQGLayoutIterator implements the functionality of
    TQLayoutIterator. Each subclass of TQLayout needs a
    TQGLayoutIterator subclass.
*/

/*!
    \fn TQLayoutItem *TQGLayoutIterator::next()

    Implemented in subclasses to move the iterator to the next item
    and return that item, or 0 if there is no next item.
*/

/*!
    \fn TQLayoutItem *TQGLayoutIterator::current()

    Implemented in subclasses to return the current item, or 0 if
    there is no current item.
*/

/*!
    \fn TQLayoutItem *TQGLayoutIterator::takeCurrent()

    Implemented in subclasses. The function must remove the current
    item from the layout without deleting it, move the iterator to the
    next item and return the removed item, or 0 if no item was
    removed.
*/

/*!
    Destroys the iterator
*/
TQGLayoutIterator::~TQGLayoutIterator()
{
}

/*!
    \class TQLayoutIterator
    \brief The TQLayoutIterator class provides iterators over TQLayoutItem.

    \ingroup appearance
    \ingroup geomanagement

    Use TQLayoutItem::iterator() to create an iterator over a layout.

    TQLayoutIterator uses \e explicit sharing with a reference count.
    If an iterator is copied and one of the copies is modified, both
    iterators will be modified.

    A TQLayoutIterator is not protected against changes in its layout. If
    the layout is modified or deleted the iterator will become invalid.
    It is not possible to test for validity. It is safe to delete an
    invalid layout; any other access may lead to an illegal memory
    reference and the abnormal termination of the program.

    Calling takeCurrent() or deleteCurrent() leaves the iterator in a
    valid state, but may invalidate any other iterators that access the
    same layout.

    The following code will draw a rectangle for each layout item in
    the layout structure of the widget.
    \code
    static void paintLayout( TQPainter *p, TQLayoutItem *lay )
    {
	TQLayoutIterator it = lay->iterator();
	TQLayoutItem *child;
	while ( (child = it.current()) != 0 ) {
	    paintLayout( p, child );
	    ++it;
	}
	p->drawRect( lay->geometry() );
    }
    void ExampleWidget::paintEvent( TQPaintEvent * )
    {
	TQPainter p( this );
	if ( layout() )
	    paintLayout( &p, layout() );
    }
    \endcode

    All the functionality of TQLayoutIterator is implemented by
    subclasses of \l TQGLayoutIterator. TQLayoutIterator itself is not
    designed to be subclassed.
*/

/*!
    \fn TQLayoutIterator::TQLayoutIterator( TQGLayoutIterator *gi )

    Constructs an iterator based on \a gi. The constructed iterator
    takes ownership of \a gi and will delete it.

    This constructor is provided for layout implementors. Application
    programmers should use TQLayoutItem::iterator() to create an
    iterator over a layout.
*/

/*!
    \fn TQLayoutIterator::TQLayoutIterator( const TQLayoutIterator &i )

    Creates a shallow copy of \a i, i.e. if the copy is modified, then
    the original will also be modified.
*/

/*!
    \fn TQLayoutIterator::~TQLayoutIterator()

    Destroys the iterator.
*/

/*!
    \fn TQLayoutIterator &TQLayoutIterator::operator=( const TQLayoutIterator &i )

    Assigns \a i to this iterator and returns a reference to this
    iterator.
*/

/*!
    \fn TQLayoutItem *TQLayoutIterator::operator++()

    Moves the iterator to the next child item and returns that item,
    or 0 if there is no such item.
*/

/*!
    \fn TQLayoutItem *TQLayoutIterator::current()

    Returns the current item, or 0 if there is no current item.
*/

/*!
    \fn TQLayoutItem *TQLayoutIterator::takeCurrent()

    Removes the current child item from the layout without deleting
    it, and moves the iterator to the next item. Returns the removed
    item, or 0 if there was no item to be removed. This iterator will
    still be valid, but any other iterator over the same layout may
    become invalid.
*/

/*!
    \fn void TQLayoutIterator::deleteCurrent()

    Removes and deletes the current child item from the layout and
    moves the iterator to the next item. This iterator will still be
    valid, but any other iterator over the same layout may become
    invalid.
*/

/*!
    \enum TQLayout::ResizeMode

    The possible values are:

    \value Auto  If the main widget is a top-level widget with no
		 height-for-width (hasHeightForWidth()), this is
		 the same as \c Minimium; otherwise, this is the
		 same as \c FreeResize.
    \value Fixed  The main widget's size is set to sizeHint(); it
		  cannot be resized at all.
    \value Minimum  The main widget's minimum size is set to
		    minimumSize(); it cannot be smaller.
    \value FreeResize  The widget is not constrained.
*/

/*!
    \property TQLayout::resizeMode
    \brief the resize mode of the layout

    The default mode is \c Auto.

    \sa TQLayout::ResizeMode
*/

void TQLayout::setResizeMode( ResizeMode mode )
{
    if ( mode == resizeMode() )
	return;

    switch ( mode ) {
    case Auto:
	frozen = false;
	autoMinimum = false;
	autoResizeMode = true;
	break;
    case Fixed:
	frozen = true;
	autoMinimum = false;
	autoResizeMode = false;
	break;
    case FreeResize:
	frozen = false;
	autoMinimum = false;
	autoResizeMode = false;
	break;
    case Minimum:
	frozen = false;
	autoMinimum = true;
	autoResizeMode = false;
    }
    if ( mainWidget() && mainWidget()->isVisible() )
	activate();
}

TQLayout::ResizeMode TQLayout::resizeMode() const
{
    return ( autoResizeMode ? Auto :
	     (frozen ? Fixed : (autoMinimum ? Minimum : FreeResize)) );
}

/*!
    \fn bool TQLayout::autoAdd() const

    Returns true if this layout automatically grabs all new
    mainWidget()'s new children and adds them as defined by addItem();
    otherwise returns false. This has effect only for top-level
    layouts, i.e. layouts that are direct children of their
    mainWidget().

    autoAdd() is disabled by default.

    Note that a top-level layout is not necessarily associated with
    the top-level widget.

    \sa setAutoAdd()
*/

/*!
    If \a b is true, auto-add is enabled; otherwise auto-add is
    disabled.

    \warning If auto-add is enabled, you cannot set stretch factors
    on the child widgets until the widgets are actually inserted in
    the layout (after control returned to the event loop). We
    therefore recommend that you avoid the auto-add feature in new
    programs.

    \sa autoAdd()
*/
void TQLayout::setAutoAdd( bool b )
{
    autoNewChild = b;
}

/*!
    \fn  bool TQLayout::supportsMargin() const

    Returns true if this layout supports \l TQLayout::margin on
    non-top-level layouts; otherwise returns false.

    \sa margin
*/

/*!
    Sets the value returned by supportsMargin(). If \a b is true,
    margin() handling is implemented by the subclass. If \a b is
    false (the default), TQLayout will add margin() around top-level
    layouts.

    If \a b is true, margin handling needs to be implemented in
    setGeometry(), maximumSize(), minimumSize(), sizeHint() and
    heightForWidth().

    \sa supportsMargin()
*/
void TQLayout::setSupportsMargin( bool b )
{
    marginImpl = b;
}

/*!
    Returns the rectangle that should be covered when the geometry of
    this layout is set to \a r, provided that this layout supports
    setAlignment().

    The result is derived from sizeHint() and expanding(). It is never
    larger than \a r.
*/
TQRect TQLayout::alignmentRect( const TQRect &r ) const
{
    TQSize s = sizeHint();
    int a = alignment();

    /*
      This is a hack to obtain the real maximum size, not
      TQSize(TQLAYOUTSIZE_MAX, TQLAYOUTSIZE_MAX), the value consistently
      returned by TQLayoutItems that have an alignment.
    */
    TQLayout *that = (TQLayout *) this;
    that->setAlignment( 0 );
    TQSize ms = maximumSize();
    that->setAlignment( a );

    if ( (expanding() & TQSizePolicy::Horizontally) ||
	 !(a & TQt::AlignHorizontal_Mask ) ) {
	s.setWidth( TQMIN(r.width(), ms.width()) );
    }
    if ( (expanding() & TQSizePolicy::Vertically) ||
	 !(a & TQt::AlignVertical_Mask) ) {
	s.setHeight( TQMIN(r.height(), ms.height()) );
    } else if ( hasHeightForWidth() ) {
	int hfw = heightForWidth( s.width() );
	if ( hfw < s.height() )
	    s.setHeight( TQMIN(hfw, ms.height()) );
    }

    int x = r.x();
    int y = r.y();

    if ( a & TQt::AlignBottom )
	y += ( r.height() - s.height() );
    else if ( !(a & TQt::AlignTop) )
	y += ( r.height() - s.height() ) / 2;

    a = TQApplication::horizontalAlignment( a );
    if ( a & TQt::AlignRight )
	x += ( r.width() - s.width() );
    else if ( !(a & TQt::AlignLeft) )
	x += ( r.width() - s.width() ) / 2;

    return TQRect( x, y, s.width(), s.height() );
}

/*!
    Removes the widget \a widget from the layout. After this call, it
    is the caller's responsibility to give the widget a reasonable
    geometry or to put the widget back into a layout.

    \sa removeItem(), TQWidget::setGeometry(), add()
*/
void TQLayout::remove( TQWidget *widget )
{
    TQLayoutIterator it = iterator();
    TQLayoutItem *child;
    while ( (child = it.current()) != 0 ) {
	if ( child->widget() == widget ) {
	    it.deleteCurrent();
	    invalidate(); // maybe redundant
	    TQApplication::postEvent( mainWidget(),
				     new TQEvent(TQEvent::LayoutHint) );
	} else {
	    ++it;
	}
    }
}

/*!
    Removes the layout item \a item from the layout. It is the
    caller's responsibility to delete the item.

    Notice that \a item can be a layout (since TQLayout inherits
    TQLayoutItem).

    \sa remove(), addItem()
*/
void TQLayout::removeItem( TQLayoutItem *item )
{
    TQLayoutIterator it = iterator();
    TQLayoutItem *child;
    while ( (child = it.current()) != 0 ) {
	if ( child == item ) {
	    it.takeCurrent();
	    invalidate(); // maybe redundant
	    TQApplication::postEvent( mainWidget(),
				     new TQEvent(TQEvent::LayoutHint) );
	} else {
	    ++it;
	}
    }
}

/*!
    Enables this layout if \a enable is true, otherwise disables it.

    An enabled layout adjusts dynamically to changes; a disabled
    layout acts as if it did not exist.

    By default all layouts are enabled.

    \sa isEnabled()
*/
void TQLayout::setEnabled( bool enable )
{
    enabled = enable;
}

/*!
    Returns true if the layout is enabled; otherwise returns false.

    \sa setEnabled()
*/
bool TQLayout::isEnabled() const
{
    return enabled;
}

void TQLayout::propagateSpacing( TQLayout *parent )
{
    TQLayoutIterator it = parent->iterator();
    TQLayoutItem *child;
    while ( (child = it.current()) ) {
	TQLayout *childLayout = child->layout();
	if ( childLayout && childLayout->insideSpacing < 0 ) {
	    childLayout->insideSpacing = parent->insideSpacing;
	    propagateSpacing( childLayout );
	}
	++it;
    }
}

#endif // TQT_NO_LAYOUT
