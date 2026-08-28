/****************************************************************************
**
** Implementation of TQSplitter class
**
**  Created : 980105
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

#include "ntqsplitter.h"
#ifndef TQT_NO_SPLITTER

#include "ntqlayout.h"
#include "../kernel/qlayoutengine_p.h"
#include "ntqapplication.h"
#include "ntqbitmap.h"
#include "ntqdrawutil.h"
#include "ntqmemarray.h"
#include "ntqobjectlist.h"
#include "ntqpainter.h"
#include "ntqptrlist.h"
#include "ntqstyle.h"

const uint Default = QT_QSPLITTER_DEFAULT;

static int mouseOffset;
static int opaqueOldPos = -1; // this assumes that there's only one mouse

static TQPoint toggle( TQWidget *w, TQPoint pos )
{
    TQSize minS = qSmartMinSize( w );
    return -pos - TQPoint( minS.width(), minS.height() );
}

static bool isCollapsed( TQWidget *w )
{
    return w->x() < 0 || w->y() < 0;
}

static TQPoint topLeft( TQWidget *w )
{
    if ( isCollapsed(w) ) {
	return toggle( w, w->pos() );
    } else {
	return w->pos();
    }
}

static TQPoint bottomRight( TQWidget *w )
{
    if ( isCollapsed(w) ) {
	return toggle( w, w->pos() ) - TQPoint( 1, 1 );
    } else {
	return w->geometry().bottomRight();
    }
}

TQSplitterHandle::TQSplitterHandle( Orientation o, TQSplitter *parent,
				  const char * name )
    : TQWidget( parent, name )
{
    s = parent;
    setOrientation( o );
}

TQSize TQSplitterHandle::sizeHint() const
{
    int hw = s->handleWidth();
    return parentWidget()->style().sizeFromContents( TQStyle::CT_Splitter, s,
						     TQSize(hw, hw) )
				  .expandedTo( TQApplication::globalStrut() );
}

void TQSplitterHandle::setOrientation( Orientation o )
{
    orient = o;
#ifndef TQT_NO_CURSOR
    setCursor( o == TQSplitter::Horizontal ? splitHCursor : splitVCursor );
#endif
}

void TQSplitterHandle::mouseMoveEvent( TQMouseEvent *e )
{
    if ( !(e->state()&LeftButton) )
	return;
    TQCOORD pos = s->pick( parentWidget()->mapFromGlobal(e->globalPos()) )
		 - mouseOffset;
    if ( opaque() ) {
	s->moveSplitter( pos, id() );
    } else {
	s->setRubberband( s->adjustPos(pos, id()) );
    }
}

void TQSplitterHandle::mousePressEvent( TQMouseEvent *e )
{
    if ( e->button() == LeftButton )
	mouseOffset = s->pick( e->pos() );
}

void TQSplitterHandle::mouseReleaseEvent( TQMouseEvent *e )
{
    if ( !opaque() && e->button() == LeftButton ) {
	TQCOORD pos = s->pick( parentWidget()->mapFromGlobal(e->globalPos()) )
		     - mouseOffset;
	s->setRubberband( -1 );
	s->moveSplitter( pos, id() );
    }
}

void TQSplitterHandle::paintEvent( TQPaintEvent * )
{
    TQStyle::SFlags flags = (orientation() == Horizontal ? TQStyle::Style_Horizontal : 0);
    if (hasMouse()) {
	flags |= TQStyle::Style_MouseOver;
    }

    TQPainter p( this );
    parentWidget()->style().drawPrimitive( TQStyle::PE_Splitter, &p, rect(),
					   colorGroup(),
					   flags );
}

TQCOORD TQSplitterLayoutStruct::getSizer( Orientation orient )
{
    if ( sizer == -1 ) {
	TQSize s = wid->sizeHint();
	if ( !s.isValid() || wid->testWState(WState_Resized) )
	    s = wid->size();
	sizer = ( orient == Horizontal ) ? s.width() : s.height();
    }
    return sizer;
}

/*!
    \class TQSplitter
    \brief The TQSplitter class implements a splitter widget.

    \ingroup organizers
    \mainclass

    A splitter lets the user control the size of child widgets by
    dragging the boundary between the children. Any number of widgets
    may be controlled by a single splitter.

    To show a TQListBox, a TQListView and a TQTextEdit side by side:
    \code
	TQSplitter *split = new TQSplitter( parent );
	TQListBox *lb = new TQListBox( split );
	TQListView *lv = new TQListView( split );
	TQTextEdit *ed = new TQTextEdit( split );
    \endcode

    TQSplitter lays out its children horizontally (side by side); you
    can use setOrientation(TQSplitter::Vertical) to lay out the
    children vertically.

    By default, all widgets can be as large or as small as the user
    wishes, between the \l minimumSizeHint() (or \l minimumSize())
    and \l maximumSize() of the widgets. Use setResizeMode() to
    specify that a widget should keep its size when the splitter is
    resized, or set the stretch component of the \l sizePolicy.

    Although TQSplitter normally resizes the children only at the end
    of a resize operation, if you call setOpaqueResize(true) the
    widgets are resized as often as possible.

    The initial distribution of size between the widgets is determined
    by the initial size of each widget. You can also use setSizes() to
    set the sizes of all the widgets. The function sizes() returns the
    sizes set by the user.

    If you hide() a child its space will be distributed among the
    other children. It will be reinstated when you show() it again. It
    is also possible to reorder the widgets within the splitter using
    moveToFirst() and moveToLast().

    <img src=qsplitter-m.png> <img src=qsplitter-w.png>

    \sa TQTabBar
*/


/*!
    Constructs a horizontal splitter with the \a parent and \a name
    arguments being passed on to the TQFrame constructor.
*/

TQSplitter::TQSplitter( TQWidget *parent, const char *name )
    : TQFrame( parent, name, WPaintUnclipped )
{
    orient = Horizontal;
    init();
}


/*!
    Constructs a splitter with orientation \a o with the \a parent and
    \a name arguments being passed on to the TQFrame constructor.
*/

TQSplitter::TQSplitter( Orientation o, TQWidget *parent, const char *name )
    : TQFrame( parent, name, WPaintUnclipped )
{
    orient = o;
    init();
}


/*!
    Destroys the splitter and any children.
*/

TQSplitter::~TQSplitter()
{
    delete d;
}


void TQSplitter::init()
{
    d = new TQSplitterPrivate;
    d->list.setAutoDelete( true );
    TQSizePolicy sp( TQSizePolicy::Expanding, TQSizePolicy::Preferred );
    if ( orient == Vertical )
	sp.transpose();
    setSizePolicy( sp );
    clearWState( WState_OwnSizePolicy );
}

/*!
    \fn void TQSplitter::refresh()

    Updates the splitter's state. You should not need to call this
    function.
*/


/*!
    \property TQSplitter::orientation
    \brief the orientation of the splitter

    By default the orientation is horizontal (the widgets are side by
    side). The possible orientations are \c Horizontal and
    \c Vertical.
*/

void TQSplitter::setOrientation( Orientation o )
{
    if ( orient == o )
	return;

    if ( !testWState( WState_OwnSizePolicy ) ) {
	TQSizePolicy sp = sizePolicy();
	sp.transpose();
	setSizePolicy( sp );
	clearWState( WState_OwnSizePolicy );
    }

    orient = o;

    TQSplitterLayoutStruct *s = d->list.first();
    while ( s ) {
	if ( s->isHandle )
	    ((TQSplitterHandle*)s->wid)->setOrientation( o );
	s = d->list.next();
    }
    recalc( isVisible() );
}

/*!
    \property TQSplitter::childrenCollapsible
    \brief whether child widgets can be resized down to size 0 by the user

    By default, children are collapsible. It is possible to enable
    and disable the collapsing of individual children; see
    setCollapsible().
*/

void TQSplitter::setChildrenCollapsible( bool collapse )
{
    d->childrenCollapsible = collapse;
}

bool TQSplitter::childrenCollapsible() const
{
    return d->childrenCollapsible;
}

/*!
    Sets whether the child widget \a w is collapsible to \a collapse.

    By default, children are collapsible, meaning that the user can
    resize them down to size 0, even if they have a non-zero
    minimumSize() or minimumSizeHint(). This behavior can be changed
    on a per-widget basis by calling this function, or globally for
    all the widgets in the splitter by setting the \l
    childrenCollapsible property.

    \sa childrenCollapsible
*/

void TQSplitter::setCollapsible( TQWidget *w, bool collapse )
{
    findWidget( w )->collapsible = collapse ? 1 : 0;
}

/*!
    \reimp
*/
void TQSplitter::resizeEvent( TQResizeEvent * )
{
    doResize();
}

TQSplitterLayoutStruct *TQSplitter::findWidget( TQWidget *w )
{
    processChildEvents();
    TQSplitterLayoutStruct *s = d->list.first();
    while ( s ) {
	if ( s->wid == w )
	    return s;
	s = d->list.next();
    }
    return addWidget( w );
}

/*
    Inserts the widget \a w at the end (or at the beginning if \a
    prepend is true) of the splitter's list of widgets.

    It is the responsibility of the caller to make sure that \a w is
    not already in the splitter and to call recalcId() if needed. (If
    \a prepend is true, then recalcId() is very probably needed.)
*/

TQSplitterLayoutStruct *TQSplitter::addWidget( TQWidget *w, bool prepend )
{
    TQSplitterLayoutStruct *s;
    TQSplitterHandle *newHandle = 0;
    if ( d->list.count() > 0 ) {
	s = new TQSplitterLayoutStruct;
	s->resizeMode = KeepSize;
	TQString tmp = "qt_splithandle_";
	tmp += w->name();
	newHandle = new TQSplitterHandle( orientation(), this, tmp );
	s->wid = newHandle;
	newHandle->setId( d->list.count() );
	s->isHandle = true;
	s->sizer = pick( newHandle->sizeHint() );
	if ( prepend )
	    d->list.prepend( s );
	else
	    d->list.append( s );
    }
    s = new TQSplitterLayoutStruct;
    s->resizeMode = DefaultResizeMode;
    s->wid = w;
    s->isHandle = false;
    if ( prepend )
	d->list.prepend( s );
    else
	d->list.append( s );
    if ( newHandle && isVisible() )
	newHandle->show(); // will trigger sending of post events
    return s;
}


/*!
    Tells the splitter that the child widget described by \a c has
    been inserted or removed.
*/

void TQSplitter::childEvent( TQChildEvent *c )
{
    if ( c->type() == TQEvent::ChildInserted ) {
	if ( !c->child()->isWidgetType() )
	    return;

	if ( ((TQWidget*)c->child())->testWFlags( WType_TopLevel ) )
	    return;

	TQSplitterLayoutStruct *s = d->list.first();
	while ( s ) {
	    if ( s->wid == c->child() )
		return;
	    s = d->list.next();
	}
	addWidget( (TQWidget*)c->child() );
	recalc( isVisible() );
    } else if ( c->type() == TQEvent::ChildRemoved ) {
	TQSplitterLayoutStruct *prev = 0;
	if ( d->list.count() > 1 )
	    prev = d->list.at( 1 );  // yes, this is correct
	TQSplitterLayoutStruct *curr = d->list.first();
	while ( curr ) {
	    if ( curr->wid == c->child() ) {
		d->list.removeRef( curr );
		if ( prev && prev->isHandle ) {
		    TQWidget *w = prev->wid;
		    d->list.removeRef( prev );
		    delete w; // will call childEvent()
		}
		recalcId();
		doResize();
		return;
	    }
	    prev = curr;
	    curr = d->list.next();
	}
    }
}


/*!
    Displays a rubber band at position \a p. If \a p is negative, the
    rubber band is removed.
*/

void TQSplitter::setRubberband( int p )
{
    TQPainter paint( this );
    paint.setPen( gray );
    paint.setBrush( gray );
    paint.setRasterOp( XorROP );
    TQRect r = contentsRect();
    const int rBord = 3; // customizable?
    int hw = handleWidth();
    if ( orient == Horizontal ) {
	if ( opaqueOldPos >= 0 )
	    paint.drawRect( opaqueOldPos + hw / 2 - rBord, r.y(),
			    2 * rBord, r.height() );
	if ( p >= 0 )
	    paint.drawRect( p + hw / 2 - rBord, r.y(), 2 * rBord, r.height() );
    } else {
	if ( opaqueOldPos >= 0 )
	    paint.drawRect( r.x(), opaqueOldPos + hw / 2 - rBord,
			    r.width(), 2 * rBord );
	if ( p >= 0 )
	    paint.drawRect( r.x(), p + hw / 2 - rBord, r.width(), 2 * rBord );
    }
    opaqueOldPos = p;
}


/*!
    \reimp
*/

bool TQSplitter::event( TQEvent *e )
{
    switch ( e->type() ) {
    case TQEvent::Show:
	if ( !d->firstShow )
	    break;
	d->firstShow = false;
	// fall through
    case TQEvent::LayoutHint:
	recalc( isVisible() );
	break;
    default:
	;
    }
    return TQWidget::event( e );
}


/*!
  \obsolete

  Draws the splitter handle in the rectangle described by \a x, \a y,
  \a w, \a h using painter \a p.
  \sa TQStyle::drawPrimitive()
*/

// ### Remove this in 4.0

void TQSplitter::drawSplitter( TQPainter *p,
			      TQCOORD x, TQCOORD y, TQCOORD w, TQCOORD h )
{
    style().drawPrimitive(TQStyle::PE_Splitter, p, TQRect(x, y, w, h), colorGroup(),
			  (orientation() == Horizontal ?
			   TQStyle::Style_Horizontal : 0));
}


/*!
    Returns the ID of the widget to the right of or below the widget
    \a w, or 0 if there is no such widget (i.e. it is either not in
    this TQSplitter or \a w is at the end).
*/

int TQSplitter::idAfter( TQWidget* w ) const
{
    TQSplitterLayoutStruct *s = d->list.first();
    bool seen_w = false;
    while ( s ) {
	if ( s->isHandle && seen_w )
	    return d->list.at();
	if ( !s->isHandle && s->wid == w )
	    seen_w = true;
	s = d->list.next();
    }
    return 0;
}


/*!
    Moves the left/top edge of the splitter handle with ID \a id as
    close as possible to position \a p, which is the distance from the
    left (or top) edge of the widget.

    For Arabic, Hebrew and other right-to-left languages the layout is
    reversed.  \a p is then the distance from the right (or top) edge
    of the widget.

    \sa idAfter()
*/
void TQSplitter::moveSplitter( TQCOORD p, int id )
{
    TQSplitterLayoutStruct *s = d->list.at( id );
    int farMin;
    int min;
    int max;
    int farMax;

    p = adjustPos( p, id, &farMin, &min, &max, &farMax );
    int oldP = pick( s->wid->pos() );

    if ( TQApplication::reverseLayout() && orient == Horizontal ) {
	int q = p + s->wid->width();
	doMove( false, q, id - 1, -1, (q > oldP), (p > max) );
	doMove( true, q, id, -1, (q > oldP), (p < min) );
    } else {
	doMove( false, p, id, +1, (p < oldP), (p > max) );
	doMove( true, p, id - 1, +1, (p < oldP), (p < min) );
    }
    storeSizes();
}


void TQSplitter::setGeo( TQWidget *w, int p, int s, bool splitterMoved )
{
    TQRect r;
    if ( orient == Horizontal ) {
	if ( TQApplication::reverseLayout() && orient == Horizontal
	     && !splitterMoved )
	    p = contentsRect().width() - p - s;
	r.setRect( p, contentsRect().y(), s, contentsRect().height() );
    } else {
	r.setRect( contentsRect().x(), p, contentsRect().width(), s );
    }

    /*
      Hide the child widget, but without calling hide() so that the
      splitter handle is still shown.
    */
    if ( !w->isHidden() && s <= 0 && pick(qSmartMinSize(w)) > 0 )
	r.moveTopLeft( toggle(w, r.topLeft()) );
    w->setGeometry( r );
}


void TQSplitter::doMove( bool backwards, int pos, int id, int delta, bool upLeft,
			bool mayCollapse )
{
    if ( id < 0 || id >= (int) d->list.count() )
	return;

    TQSplitterLayoutStruct *s = d->list.at( id );
    TQWidget *w = s->wid;

    int nextId = backwards ? id - delta : id + delta;

    if ( w->isHidden() ) {
	doMove( backwards, pos, nextId, delta, upLeft, true );
    } else {
	if ( s->isHandle ) {
	    int dd = s->getSizer( orient );
	    int nextPos = backwards ? pos - dd : pos + dd;
	    int left = backwards ? pos - dd : pos;
	    setGeo( w, left, dd, true );
	    doMove( backwards, nextPos, nextId, delta, upLeft, mayCollapse );
	} else {
	    int dd = backwards ? pos - pick( topLeft(w) )
			       : pick( bottomRight(w) ) - pos + 1;
	    if ( dd > 0 || (!isCollapsed(w) && !mayCollapse) ) {
		dd = TQMAX( pick(qSmartMinSize(w)),
			   TQMIN(dd, pick(w->maximumSize())) );
	    } else {
		dd = 0;
	    }
	    setGeo( w, backwards ? pos - dd : pos, dd, true );
	    doMove( backwards, backwards ? pos - dd : pos + dd, nextId, delta,
		    upLeft, true );
	}
    }
}

int TQSplitter::findWidgetJustBeforeOrJustAfter( int id, int delta, int &collapsibleSize )
{
    id += delta;
    do {
	TQWidget *w = d->list.at( id )->wid;
	if ( !w->isHidden() ) {
            if ( collapsible(d->list.at(id)) )
                collapsibleSize = pick( qSmartMinSize(w) );
	    return id;
	}
	id += 2 * delta; // go to previous (or next) widget, skip the handle
    } while ( id >= 0 && id < (int)d->list.count() );

    return -1;
}

void TQSplitter::getRange( int id, int *farMin, int *min, int *max, int *farMax )
{
    int n = d->list.count();
    if ( id <= 0 || id >= n - 1 )
	return;

    int collapsibleSizeBefore = 0;
    int idJustBefore = findWidgetJustBeforeOrJustAfter( id, -1, collapsibleSizeBefore );

    int collapsibleSizeAfter = 0;
    int idJustAfter = findWidgetJustBeforeOrJustAfter( id, +1, collapsibleSizeAfter );

    int minBefore = 0;
    int minAfter = 0;
    int maxBefore = 0;
    int maxAfter = 0;
    int i;

    for ( i = 0; i < id; i++ )
	addContribution( i, &minBefore, &maxBefore, i == idJustBefore );
    for ( i = id; i < n; i++ )
	addContribution( i, &minAfter, &maxAfter, i == idJustAfter );

    TQRect r = contentsRect();
    int farMinVal;
    int minVal;
    int maxVal;
    int farMaxVal;

    int smartMinBefore = TQMAX( minBefore, pick(r.size()) - maxAfter );
    int smartMaxBefore = TQMIN( maxBefore, pick(r.size()) - minAfter );

    if ( orient == Vertical || !TQApplication::reverseLayout() ) {
	minVal = pick( r.topLeft() ) + smartMinBefore;
	maxVal = pick( r.topLeft() ) + smartMaxBefore;

	farMinVal = minVal;
	if ( minBefore - collapsibleSizeBefore >= pick(r.size()) - maxAfter )
	    farMinVal -= collapsibleSizeBefore;
	farMaxVal = maxVal;
	if ( pick(r.size()) - (minAfter - collapsibleSizeAfter) <= maxBefore )
	    farMaxVal += collapsibleSizeAfter;
    } else {
	int hw = handleWidth();
	minVal = r.width() - smartMaxBefore - hw;
	maxVal = r.width() - smartMinBefore - hw;

	farMinVal = minVal;
	if ( pick(r.size()) - (minAfter - collapsibleSizeAfter) <= maxBefore )
	    farMinVal -= collapsibleSizeAfter;
	farMaxVal = maxVal;
	if ( minBefore - collapsibleSizeBefore >= pick(r.size()) - maxAfter )
	    farMaxVal += collapsibleSizeBefore;
    }

    if ( farMin )
	*farMin = farMinVal;
    if ( min )
	*min = minVal;
    if ( max )
	*max = maxVal;
    if ( farMax )
	*farMax = farMaxVal;
}

/*!
    Returns the valid range of the splitter with ID \a id in \a *min
    and \a *max if \a min and \a max are not 0.

    \sa idAfter()
*/

void TQSplitter::getRange( int id, int *min, int *max )
{
    getRange( id, min, 0, 0, max );
}


/*!
    Returns the closest legal position to \a pos of the widget with ID
    \a id.

    \sa idAfter()
*/

int TQSplitter::adjustPos( int pos, int id )
{
    int x, i, n, u;
    return adjustPos( pos, id, &u, &n, &i, &x );
}

int TQSplitter::adjustPos( int pos, int id, int *farMin, int *min, int *max,
			  int *farMax )
{
    const int Threshold = 40;

    getRange( id, farMin, min, max, farMax );

    if ( pos >= *min ) {
	if ( pos <= *max ) {
	    return pos;
	} else {
	    int delta = pos - *max;
	    int width = *farMax - *max;

	    if ( delta > width / 2 && delta >= TQMIN(Threshold, width) ) {
		return *farMax;
	    } else {
		return *max;
	    }
	}
    } else {
	int delta = *min - pos;
	int width = *min - *farMin;

	if ( delta > width / 2 && delta >= TQMIN(Threshold, width) ) {
	    return *farMin;
	} else {
	    return *min;
	}
    }
}

bool TQSplitter::collapsible( TQSplitterLayoutStruct *s )
{
    if (pick(qSmartMinSize(s->wid)) == 1)
        return false;
    if ( s->collapsible != Default ) {
	return (bool) s->collapsible;
    } else {
	return d->childrenCollapsible;
    }
}

void TQSplitter::doResize()
{
    TQRect r = contentsRect();
    int n = d->list.count();
    TQMemArray<TQLayoutStruct> a( n );

    for ( int pass = 0; pass < 2; pass++ ) {
	int numAutoWithStretch = 0;
	int numAutoWithoutStretch = 0;

	for ( int i = 0; i < n; i++ ) {
	    a[i].init();
	    TQSplitterLayoutStruct *s = d->list.at( i );
	    if ( s->wid->isHidden() || isCollapsed(s->wid) ) {
		a[i].maximumSize = 0;
	    } else if ( s->isHandle ) {
		a[i].sizeHint = a[i].minimumSize = a[i].maximumSize = s->sizer;
		a[i].empty = false;
	    } else {
		int mode = s->resizeMode;
		int stretch = 1;

		if ( mode == DefaultResizeMode ) {
		    TQSizePolicy p = s->wid->sizePolicy();
		    int sizePolicyStretch =
			    pick( TQSize(p.horStretch(), p.verStretch()) );
		    if ( sizePolicyStretch > 0 ) {
			mode = Stretch;
			stretch = sizePolicyStretch;
			numAutoWithStretch++;
		    } else {
			/*
			  Do things differently on the second pass,
			  if there's one. A second pass is necessary
			  if it was found out during the first pass
			  that all DefaultResizeMode items are
			  KeepSize items. In that case, we make them
			  all Stretch items instead, for a more TQt
			  3.0-compatible behavior.
			*/
			mode = ( pass == 0 ) ? KeepSize : Stretch;
			numAutoWithoutStretch++;
		    }
		}

		a[i].minimumSize = pick( qSmartMinSize(s->wid) );
		a[i].maximumSize = pick( s->wid->maximumSize() );
		a[i].empty = false;

		if ( mode == Stretch ) {
		    if ( s->getSizer(orient) > 1 )
			stretch *= s->getSizer( orient );
		    // TQMIN(): ad hoc work-around for layout engine limitation
		    a[i].stretch = TQMIN( stretch, 8192 );
		    a[i].sizeHint = a[i].minimumSize;
		} else if ( mode == KeepSize ) {
		    a[i].sizeHint = s->getSizer( orient );
		} else { // mode == FollowSizeHint
		    a[i].sizeHint = pick( s->wid->sizeHint() );
		}
	    }
	}

	// a second pass would yield the same results
	if ( numAutoWithStretch > 0 || numAutoWithoutStretch == 0 )
	    break;
    }

    qGeomCalc( a, 0, n, pick( r.topLeft() ), pick( r.size() ), 0 );

    for ( int i = 0; i < n; i++ ) {
	TQSplitterLayoutStruct *s = d->list.at(i);
	setGeo( s->wid, a[i].pos, a[i].size, false );
    }
}

void TQSplitter::recalc( bool update )
{
    int fi = 2 * frameWidth();
    int maxl = fi;
    int minl = fi;
    int maxt = TQWIDGETSIZE_MAX;
    int mint = fi;
    int n = d->list.count();
    bool first = true;

    /*
      Splitter handles before the first visible widget or right
      before a hidden widget must be hidden.
    */
    for ( int i = 0; i < n; i++ ) {
	TQSplitterLayoutStruct *s = d->list.at( i );
	if ( !s->isHandle ) {
	    TQSplitterLayoutStruct *p = 0;
	    if ( i > 0 )
		p = d->list.at( i - 1 );

	    // may trigger new recalc
	    if ( p && p->isHandle )
		p->wid->setHidden( first || s->wid->isHidden() );

	    if ( !s->wid->isHidden() )
		first = false;
	}
    }

    bool empty = true;
    for ( int j = 0; j < n; j++ ) {
	TQSplitterLayoutStruct *s = d->list.at( j );
	if ( !s->wid->isHidden() ) {
	    empty = false;
	    if ( s->isHandle ) {
		minl += s->getSizer( orient );
		maxl += s->getSizer( orient );
	    } else {
		TQSize minS = qSmartMinSize( s->wid );
		minl += pick( minS );
		maxl += pick( s->wid->maximumSize() );
		mint = TQMAX( mint, trans(minS) );
		int tm = trans( s->wid->maximumSize() );
		if ( tm > 0 )
		    maxt = TQMIN( maxt, tm );
	    }
	}
    }
    if ( empty ) {
	if ( ::tqt_cast<TQSplitter*>(parentWidget()) ) {
	    // nested splitters; be nice
	    maxl = maxt = 0;
	} else {
	    // TQSplitter with no children yet
	    maxl = TQWIDGETSIZE_MAX;
	}
    } else {
	maxl = TQMIN( maxl, TQWIDGETSIZE_MAX );
    }
    if ( maxt < mint )
	maxt = mint;

    if ( orient == Horizontal ) {
	setMaximumSize( maxl, maxt );
	setMinimumSize( minl, mint );
    } else {
	setMaximumSize( maxt, maxl );
	setMinimumSize( mint, minl );
    }
    if ( update )
	doResize();
    else
	d->firstShow = true;
}

/*!
    \enum TQSplitter::ResizeMode

    This enum type describes how TQSplitter will resize each of its
    child widgets.

    \value Auto  The widget will be resized according to the stretch
    factors set in its sizePolicy().

    \value Stretch  The widget will be resized when the splitter
    itself is resized.

    \value KeepSize  TQSplitter will try to keep the widget's size
    unchanged.

    \value FollowSizeHint  TQSplitter will resize the widget when the
    widget's size hint changes.
*/

/*!
    Sets resize mode of widget \a w to \a mode. (The default is \c
    Auto.)
*/

void TQSplitter::setResizeMode( TQWidget *w, ResizeMode mode )
{
    findWidget( w )->resizeMode = mode;
}


/*!
    \property TQSplitter::opaqueResize
    \brief whether resizing is opaque

    Opaque resizing is off by default.
*/

bool TQSplitter::opaqueResize() const
{
    return d->opaque;
}


void TQSplitter::setOpaqueResize( bool on )
{
    d->opaque = on;
}


/*!
    Moves widget \a w to the leftmost/top position.
*/

void TQSplitter::moveToFirst( TQWidget *w )
{
    processChildEvents();
    bool found = false;
    TQSplitterLayoutStruct *s = d->list.first();
    while ( s ) {
	if ( s->wid == w ) {
	    found = true;
	    TQSplitterLayoutStruct *p = d->list.prev();
	    if ( p ) { // not already at first place
		d->list.take(); // take p
		d->list.take(); // take s
		d->list.prepend( p );
		d->list.prepend( s );
	    }
	    break;
	}
	s = d->list.next();
    }
    if ( !found )
	addWidget( w, true );
    recalcId();
}


/*!
    Moves widget \a w to the rightmost/bottom position.
*/

void TQSplitter::moveToLast( TQWidget *w )
{
    processChildEvents();
    bool found = false;
    TQSplitterLayoutStruct *s = d->list.first();
    while ( s ) {
	if ( s->wid == w ) {
	    found = true;
	    d->list.take(); // take s
	    TQSplitterLayoutStruct *p = d->list.current();
	    if ( p ) { // the splitter handle after s
		d->list.take(); // take p
		d->list.append( p );
	    }
	    d->list.append( s );
	    break;
	}
	s = d->list.next();
    }
    if ( !found )
	addWidget( w );
    recalcId();
}


void TQSplitter::recalcId()
{
    int n = d->list.count();
    for ( int i = 0; i < n; i++ ) {
	TQSplitterLayoutStruct *s = d->list.at( i );
	if ( s->isHandle )
	    ((TQSplitterHandle*)s->wid)->setId( i );
    }
}


/*!
    \reimp
*/
TQSize TQSplitter::sizeHint() const
{
    constPolish();
    int l = 0;
    int t = 0;
    if ( children() ) {
	const TQObjectList * c = children();
	TQObjectListIt it( *c );
	TQObject * o;

	while( (o = it.current()) != 0 ) {
	    ++it;
	    if ( o->isWidgetType() && !((TQWidget*)o)->isHidden() ) {
		TQSize s = ((TQWidget*)o)->sizeHint();
		if ( s.isValid() ) {
		    l += pick( s );
		    t = TQMAX( t, trans( s ) );
		}
	    }
	}
    }
    return orientation() == Horizontal ? TQSize( l, t ) : TQSize( t, l );
}


/*!
    \reimp
*/

TQSize TQSplitter::minimumSizeHint() const
{
    constPolish();
    int l = 0;
    int t = 0;
    if ( children() ) {
	const TQObjectList * c = children();
	TQObjectListIt it( *c );
	TQObject * o;

	while ( (o = it.current()) != 0 ) {
	    ++it;
	    if ( o->isWidgetType() && !((TQWidget*)o)->isHidden() ) {
		TQSize s = qSmartMinSize( (TQWidget*)o );
		if ( s.isValid() ) {
		    l += pick( s );
		    t = TQMAX( t, trans( s ) );
		}
	    }
	}
    }
    return orientation() == Horizontal ? TQSize( l, t ) : TQSize( t, l );
}


void TQSplitter::storeSizes()
{
    TQSplitterLayoutStruct *s = d->list.first();
    while ( s ) {
	if ( !s->isHandle )
	    s->sizer = pick( s->wid->size() );
	s = d->list.next();
    }
}


void TQSplitter::addContribution( int id, int *min, int *max,
				 bool mayCollapse )
{
    TQSplitterLayoutStruct *s = d->list.at( id );
    if ( !s->wid->isHidden() ) {
	if ( s->isHandle ) {
	    *min += s->getSizer( orient );
	    *max += s->getSizer( orient );
	} else {
	    if ( mayCollapse || !isCollapsed(s->wid) )
		*min += pick( qSmartMinSize(s->wid) );
	    *max += pick( s->wid->maximumSize() );
	}
    }
}


/*!
    Returns a list of the size parameters of all the widgets in this
    splitter.

    If the splitter's orientation is horizontal, the list is a list of
    widget widths; if the orientation is vertical, the list is a list
    of widget heights.

    Giving the values to another splitter's setSizes() function will
    produce a splitter with the same layout as this one.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQValueList<int> list = mySplitter.sizes();
    TQValueList<int>::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa setSizes()
*/

TQValueList<int> TQSplitter::sizes() const
{
    if ( !testWState(WState_Polished) )
	constPolish();

    TQValueList<int> list;
    TQSplitterLayoutStruct *s = d->list.first();
    while ( s ) {
	if ( !s->isHandle )
	    list.append( isCollapsed(s->wid) ? 0 : pick(s->wid->size()));
	s = d->list.next();
    }
    return list;
}

/*!
    Sets the size parameters to the values given in the \a list. If
    the splitter is horizontal, the values set the widths of each
    widget going from left to right. If the splitter is vertical, the
    values set the heights of each widget going from top to bottom.
    Extra values in the \a list are ignored.

    If \a list contains too few values, the result is undefined but
    the program will still be well-behaved.

    Note that the values in \a list should be the height/width that
    the widgets should be resized to.

    \sa sizes()
*/

void TQSplitter::setSizes( TQValueList<int> list )
{
    processChildEvents();
    TQValueList<int>::Iterator it = list.begin();
    TQSplitterLayoutStruct *s = d->list.first();
    while ( s && it != list.end() ) {
	if ( !s->isHandle ) {
	    s->sizer = TQMAX( *it, 0 );
	    int smartMinSize = pick( qSmartMinSize(s->wid) );
	    // Make sure that we reset the collapsed state.
	    if ( s->sizer == 0 ) {
		if ( collapsible(s) && smartMinSize > 0 ) {
		    s->wid->move( -1, -1 );
		} else {
		    s->sizer = smartMinSize;
		    s->wid->move( 0, 0 );
		}
	    } else {
		if ( s->sizer < smartMinSize )
		    s->sizer = smartMinSize;
		s->wid->move( 0, 0 );
	    }
	    ++it;
	}
	s = d->list.next();
    }
    doResize();
}

/*!
    \property TQSplitter::handleWidth
    \brief the width of the splitter handle
*/

int TQSplitter::handleWidth() const
{
    if ( d->handleWidth > 0 ) {
	return d->handleWidth;
    } else {
	return style().pixelMetric( TQStyle::PM_SplitterWidth, this );
    }
}

void TQSplitter::setHandleWidth( int width )
{
    d->handleWidth = width;
    updateHandles();
}

/*!
    Processes all posted child events, ensuring that the internal state of
    the splitter is kept consistent.
*/

void TQSplitter::processChildEvents()
{
    TQApplication::sendPostedEvents( this, TQEvent::ChildInserted );
}

/*!
    \reimp
*/

void TQSplitter::styleChange( TQStyle& old )
{
    updateHandles();
    TQFrame::styleChange( old );
}

void TQSplitter::updateHandles()
{
    int hw = handleWidth();
    TQSplitterLayoutStruct *s = d->list.first();
    while ( s ) {
	if ( s->isHandle )
	    s->sizer = hw;
	s = d->list.next();
    }
    recalc( isVisible() );
}

#ifndef TQT_NO_TEXTSTREAM
/*!
    \relates TQSplitter

    Writes the sizes and the hidden state of the widgets in the
    splitter \a splitter to the text stream \a ts.

    \sa operator>>(), sizes(), TQWidget::isHidden()
*/

TQTextStream& operator<<( TQTextStream& ts, const TQSplitter& splitter )
{
    TQSplitterLayoutStruct *s = splitter.d->list.first();
    bool first = true;
    ts << "[";

    while ( s != 0 ) {
	if ( !s->isHandle ) {
	    if ( !first )
		ts << ",";

	    if ( s->wid->isHidden() ) {
		ts << "H";
	    } else if ( isCollapsed(s->wid) ) {
		ts << 0;
	    } else {
		ts << s->getSizer( splitter.orientation() );
	    }
	    first = false;
	}
	s = splitter.d->list.next();
    }
    ts << "]" << endl;
    return ts;
}

/*!
    \relates TQSplitter

    Reads the sizes and the hidden state of the widgets in the
    splitter \a splitter from the text stream \a ts. The sizes must
    have been previously written by the operator<<() function.

    \sa operator<<(), setSizes(), TQWidget::hide()
*/

TQTextStream& operator>>( TQTextStream& ts, TQSplitter& splitter )
{
#undef SKIP_SPACES
#define SKIP_SPACES() \
    while ( line[i].isSpace() ) \
	i++

    splitter.processChildEvents();
    TQSplitterLayoutStruct *s = splitter.d->list.first();
    TQString line = ts.readLine();
    int i = 0;

    SKIP_SPACES();
    if ( line[i] == '[' ) {
	i++;
	SKIP_SPACES();
	while ( line[i] != ']' ) {
	    while ( s != 0 && s->isHandle )
		s = splitter.d->list.next();
	    if ( s == 0 )
		break;

	    if ( line[i].upper() == 'H' ) {
		s->wid->hide();
		i++;
	    } else {
		s->wid->show();
		int dim = 0;
		while ( line[i].digitValue() >= 0 ) {
		    dim *= 10;
		    dim += line[i].digitValue();
		    i++;
		}
		s->sizer = dim;
		if ( dim == 0 )
		    splitter.setGeo( s->wid, 0, 0, false );
	    }
	    SKIP_SPACES();
	    if ( line[i] == ',' ) {
		i++;
	    } else {
		break;
	    }
	    SKIP_SPACES();
	    s = splitter.d->list.next();
	}
    }
    splitter.doResize();
    return ts;
}
#endif

#endif
