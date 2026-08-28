/****************************************************************************
**
** Implementation of the TQDockArea class
**
** Created : 001010
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqdockarea.h"

#ifndef TQT_NO_MAINWINDOW
#include "ntqsplitter.h"
#include "ntqlayout.h"
#include "ntqptrvector.h"
#include "ntqapplication.h"
#include "ntqpainter.h"
#include "ntqwidgetlist.h"
#include "ntqmap.h"
#include "ntqmainwindow.h"

//#define TQDOCKAREA_DEBUG

struct TQ_EXPORT DockData
{
    DockData() : w( 0 ), rect() {}
    DockData( TQDockWindow *dw, const TQRect &r ) : w( dw ), rect( r ) {}
    TQDockWindow *w;
    TQRect rect;
};

static int fix_x( TQDockWindow* w, int width = -1 ) {
    if ( TQApplication::reverseLayout() ) {
	if ( width < 0 )
	    width = w->width();
	return w->parentWidget()->width() - w->x() - width;
    }
    return w->x();
}
static int fix_x( TQDockWindow* w, int x, int width = -1 ) {
    if ( TQApplication::reverseLayout() ) {
	if ( width < 0 )
	    width = w->width();
	return w->parentWidget()->width() - x - width;
    }
    return x;
}

static TQPoint fix_pos( TQDockWindow* w ) {
    if ( TQApplication::reverseLayout() ) {
	TQPoint p = w->pos();
	p.rx() = w->parentWidget()->width() - p.x() - w->width();
	return p;
    }
    return w->pos();
}


void TQDockAreaLayout::setGeometry( const TQRect &r )
{
    TQLayout::setGeometry( r );
    layoutItems( r );
}

TQLayoutIterator TQDockAreaLayout::iterator()
{
    return 0;
}

TQSize TQDockAreaLayout::sizeHint() const
{
    if ( !dockWindows || !dockWindows->first() )
	return TQSize( 0, 0 );

    if ( dirty ) {
	TQDockAreaLayout *that = (TQDockAreaLayout *) this;
	that->layoutItems( geometry() );
    }

    int w = 0;
    int h = 0;
    TQPtrListIterator<TQDockWindow> it( *dockWindows );
    TQDockWindow *dw = 0;
    it.toFirst();
    int y = -1;
    int x = -1;
    int ph = 0;
    int pw = 0;
    while ( ( dw = it.current() ) != 0 ) {
	int plush = 0, plusw = 0;
	++it;
	if ( dw->isHidden() )
	    continue;
	if ( hasHeightForWidth() ) {
	    if ( y != dw->y() )
		plush = ph;
	    y = dw->y();
	    ph = dw->height();
	} else {
	    if ( x != dw->x() )
		plusw = pw;
	    x = dw->x();
	    pw = dw->width();
	}
	h = TQMAX( h, dw->height() + plush );
	w = TQMAX( w, dw->width() + plusw );
    }

    if ( hasHeightForWidth() )
	return TQSize( 0, h );
    return TQSize( w, 0 );
}

bool TQDockAreaLayout::hasHeightForWidth() const
{
    return orient == Horizontal;
}

void TQDockAreaLayout::init()
{
    dirty = true;
    cached_width = 0;
    cached_height = 0;
    cached_hfw = -1;
    cached_wfh = -1;
}

void TQDockAreaLayout::invalidate()
{
    dirty = true;
    cached_width = 0;
    cached_height = 0;
}

static int start_pos( const TQRect &r, TQt::Orientation o )
{
    if ( o == TQt::Horizontal ) {
	return TQMAX( 0, r.x() );
    } else {
	return TQMAX( 0, r.y() );
    }
}

static void add_size( int s, int &pos, TQt::Orientation o )
{
    if ( o == TQt::Horizontal ) {
	pos += s;
    } else {
	pos += s;
    }
}

static int space_left( const TQRect &r, int pos, TQt::Orientation o )
{
    if ( o == TQt::Horizontal ) {
	return ( r.x() + r.width() ) - pos;
    } else {
	return ( r.y() + r.height() ) - pos;
    }
}

static int dock_extent( TQDockWindow *w, TQt::Orientation o, int maxsize )
{
    if ( o == TQt::Horizontal )
	return TQMIN( maxsize, TQMAX( w->sizeHint().width(), w->fixedExtent().width() ) );
    else
	return TQMIN( maxsize, TQMAX( w->sizeHint().height(), w->fixedExtent().height() ) );
}

static int dock_strut( TQDockWindow *w, TQt::Orientation o )
{
    if ( o != TQt::Horizontal ) {
	int wid;
	if ( ( wid = w->fixedExtent().width() ) != -1 ) {
	    return TQMAX( wid, TQMAX( w->minimumSize().width(), w->minimumSizeHint().width() ) );
	}
	return TQMAX( w->sizeHint().width(), TQMAX( w->minimumSize().width(), w->minimumSizeHint().width() ) );
    } else {
	int hei;
	if ( ( hei = w->fixedExtent().height() ) != -1 ) {
	    return TQMAX( hei, TQMAX( w->minimumSizeHint().height(), w->minimumSize().height() ) );
	}
	return TQMAX( w->sizeHint().height(), TQMAX( w->minimumSizeHint().height(), w->minimumSize().height() ) );
    }
}

static void set_geometry( TQDockWindow *w, int pos, int sectionpos, int extent, int strut, TQt::Orientation o )
{
    if ( o == TQt::Horizontal )
	w->setGeometry( fix_x( w, pos, extent), sectionpos, extent, strut );
    else
	w->setGeometry( sectionpos, pos, strut, extent );
}

static int size_extent( const TQSize &s, TQt::Orientation o, bool swap = false )
{
    return o == TQt::Horizontal ? ( swap ? s.height() : s.width() ) : ( swap ? s.width() :  s.height() );
}

static int point_pos( const TQPoint &p, TQt::Orientation o, bool swap = false )
{
    return o == TQt::Horizontal ? ( swap ? p.y() : p.x() ) : ( swap ? p.x() : p.y() );
}

static void shrink_extend( TQDockWindow *dw, int &dockExtend, int /*spaceLeft*/, TQt::Orientation o )
{
    TQToolBar *tb = ::tqt_cast<TQToolBar*>(dw);
    if ( o == TQt::Horizontal ) {
	int mw = 0;
	if ( !tb )
	    mw = dw->minimumWidth();
	else
	    mw = dw->sizeHint().width();
	dockExtend = mw;
    } else {
	int mh = 0;
	if ( !tb )
	    mh = dw->minimumHeight();
	else
	    mh = dw->sizeHint().height();
	dockExtend = mh;
    }
}

static void place_line( TQValueList<DockData> &lastLine, TQt::Orientation o, int linestrut, int fullextent, int tbstrut, int maxsize, TQDockAreaLayout * )
{
    TQDockWindow *last = 0;
    TQRect lastRect;
    for ( TQValueList<DockData>::Iterator it = lastLine.begin(); it != lastLine.end(); ++it ) {
	if ( tbstrut != -1 && ::tqt_cast<TQToolBar*>((*it).w) )
	    (*it).rect.setHeight( tbstrut );
	if ( !last ) {
	    last = (*it).w;
	    lastRect = (*it).rect;
	    continue;
	}
	if ( !last->isStretchable() ) {
	    int w = TQMIN( lastRect.width(), maxsize );
	    set_geometry( last, lastRect.x(), lastRect.y(), w, lastRect.height(), o );
	} else {
	    int w = TQMIN( (*it).rect.x() - lastRect.x(), maxsize );
	    set_geometry( last, lastRect.x(), lastRect.y(), w,
			  last->isResizeEnabled() ? linestrut : lastRect.height(), o );
	}
	last = (*it).w;
	lastRect = (*it).rect;
    }
    if ( !last )
	return;
    if ( !last->isStretchable() ) {
	int w = TQMIN( lastRect.width(), maxsize );
	set_geometry( last, lastRect.x(), lastRect.y(), w, lastRect.height(), o );
    } else {
	int w = TQMIN( fullextent - lastRect.x() - ( o == TQt::Vertical ? 1 : 0 ), maxsize );
	set_geometry( last, lastRect.x(), lastRect.y(), w,
		      last->isResizeEnabled() ? linestrut : lastRect.height(), o );
    }
}


TQSize TQDockAreaLayout::minimumSize() const
{
    if ( !dockWindows || !dockWindows->first() )
	return TQSize( 0, 0 );

    if ( dirty ) {
	TQDockAreaLayout *that = (TQDockAreaLayout *) this;
	that->layoutItems( geometry() );
    }

    int s = 0;

    TQPtrListIterator<TQDockWindow> it( *dockWindows );
    TQDockWindow *dw = 0;
    while ( ( dw = it.current() ) != 0 ) {
	++it;
	if ( dw->isHidden() )
	    continue;
	s = TQMAX( s, dock_strut( dw, orientation() ) );
    }

    return orientation() == Horizontal ? TQSize( 0, s ? s+2 : 0 ) :  TQSize( s, 0 );
}



int TQDockAreaLayout::layoutItems( const TQRect &rect, bool testonly )
{
    if ( !dockWindows || !dockWindows->first() )
	return 0;

    dirty = false;

    // some corrections
    TQRect r = rect;
    if ( orientation() == Vertical )
	r.setHeight( r.height() - 3 );

    // init
    lines.clear();
    ls.clear();
    TQPtrListIterator<TQDockWindow> it( *dockWindows );
    TQDockWindow *dw = 0;
    int start = start_pos( r, orientation() );
    int pos = start;
    int sectionpos = 0;
    int linestrut = 0;
    TQValueList<DockData> lastLine;
    int tbstrut = -1;
    int maxsize = size_extent( rect.size(), orientation() );
    int visibleWindows = 0;

    // go through all widgets in the dock
    while ( ( dw = it.current() ) != 0 ) {
	++it;
	if ( dw->isHidden() )
	    continue;
	++visibleWindows;
	// find position for the widget: This is the maximum of the
	// end of the previous widget and the offset of the widget. If
	// the position + the width of the widget dosn't fit into the
	// dock, try moving it a bit back, if possible.
	int op = pos;
	int dockExtend = dock_extent( dw, orientation(), maxsize );
	if ( !dw->isStretchable() ) {
	    pos = TQMAX( pos, dw->offset() );
	    if ( pos + dockExtend > size_extent( r.size(), orientation() ) - 1 )
		pos = TQMAX( op, size_extent( r.size(), orientation() ) - 1 - dockExtend );
	}
	if ( !lastLine.isEmpty() && !dw->newLine() && space_left( rect, pos, orientation() ) < dockExtend )
	    shrink_extend( dw, dockExtend, space_left( rect, pos, orientation() ), orientation() );
	// if the current widget doesn't fit into the line anymore and it is not the first widget of the line
	if ( !lastLine.isEmpty() &&
	     ( space_left( rect, pos, orientation() ) < dockExtend || dw->newLine() ) ) {
	    if ( !testonly ) // place the last line, if not in test mode
		place_line( lastLine, orientation(), linestrut, size_extent( r.size(), orientation() ), tbstrut, maxsize, this );
	    // remember the line coordinats of the last line
	    if ( orientation() == Horizontal )
		lines.append( TQRect( 0, sectionpos, r.width(), linestrut ) );
	    else
		lines.append( TQRect( sectionpos, 0, linestrut, r.height() ) );
	    // do some clearing for the next line
	    lastLine.clear();
	    sectionpos += linestrut;
	    linestrut = 0;
	    pos = start;
	    tbstrut = -1;
	}

	// remeber first widget of a line
	if ( lastLine.isEmpty() ) {
	    ls.append( dw );
	    // try to make the best position
	    int op = pos;
	    if ( !dw->isStretchable() )
		pos = TQMAX( pos, dw->offset() );
	    if ( pos + dockExtend > size_extent( r.size(), orientation() ) - 1 )
		pos = TQMAX( op, size_extent( r.size(), orientation() ) - 1 - dockExtend );
	}
	// do some calculations and add the remember the rect which the docking widget requires for the placing
	TQRect dwRect(pos, sectionpos, dockExtend, dock_strut( dw, orientation()  ) );
	lastLine.append( DockData( dw, dwRect ) );
	if ( ::tqt_cast<TQToolBar*>(dw) )
	    tbstrut = TQMAX( tbstrut, dock_strut( dw, orientation() ) );
	linestrut = TQMAX( dock_strut( dw, orientation() ), linestrut );
	add_size( dockExtend, pos, orientation() );
    }

    // if some stuff was not placed/stored yet, do it now
    if ( !testonly )
	place_line( lastLine, orientation(), linestrut, size_extent( r.size(), orientation() ), tbstrut, maxsize, this );
    if ( orientation() == Horizontal )
	lines.append( TQRect( 0, sectionpos, r.width(), linestrut ) );
    else
	lines.append( TQRect( sectionpos, 0, linestrut, r.height() ) );
    if ( *(--lines.end()) == *(--(--lines.end())) )
	lines.remove( lines.at( lines.count() - 1 ) );

    it.toFirst();
    bool hadResizable = false;
    while ( ( dw = it.current() ) != 0 ) {
	++it;
	if ( !dw->isVisibleTo( parentWidget ) )
	    continue;
	hadResizable = hadResizable || dw->isResizeEnabled();
	dw->updateSplitterVisibility( visibleWindows > 1 ); //!dw->area()->isLastDockWindow( dw ) );
    }
    return sectionpos + linestrut;
}

int TQDockAreaLayout::heightForWidth( int w ) const
{
    if ( dockWindows->isEmpty() && parentWidget )
	return parentWidget->minimumHeight();

    if ( cached_width != w ) {
	TQDockAreaLayout * mthis = (TQDockAreaLayout*)this;
	mthis->cached_width = w;
	int h = mthis->layoutItems( TQRect( 0, 0, w, 0 ), true );
	mthis->cached_hfw = h;
	return h;
    }

    return cached_hfw;
}

int TQDockAreaLayout::widthForHeight( int h ) const
{
    if ( cached_height != h ) {
	TQDockAreaLayout * mthis = (TQDockAreaLayout*)this;
	mthis->cached_height = h;
	int w = mthis->layoutItems( TQRect( 0, 0, 0, h ), true );
	mthis->cached_wfh = w;
	return w;
    }
    return cached_wfh;
}




/*!
    \class TQDockArea ntqdockarea.h
    \brief The TQDockArea class manages and lays out TQDockWindows.

    \ingroup application

    A TQDockArea is a container which manages a list of
    \l{TQDockWindow}s which it lays out within its area. In cooperation
    with the \l{TQDockWindow}s it is responsible for the docking and
    undocking of \l{TQDockWindow}s and moving them inside the dock
    area. TQDockAreas also handle the wrapping of \l{TQDockWindow}s to
    fill the available space as compactly as possible. TQDockAreas can
    contain TQToolBars since TQToolBar is a TQDockWindow subclass.

    TQMainWindow contains four TQDockAreas which you can use for your
    TQToolBars and TQDockWindows, so in most situations you do not need
    to use the TQDockArea class directly. Although TQMainWindow contains
    support for its own dock areas it isn't convenient for adding new
    TQDockAreas. If you need to create your own dock areas we suggest
    that you create a subclass of TQWidget and add your TQDockAreas to
    your subclass.

    \img qmainwindow-qdockareas.png TQMainWindow's TQDockAreas

    \target lines
    \e Lines. TQDockArea uses the concept of lines. A line is a
    horizontal region which may contain dock windows side-by-side. A
    dock area may have room for more than one line. When dock windows
    are docked into a dock area they are usually added at the right
    hand side of the top-most line that has room (unless manually
    placed by the user). When users move dock windows they may leave
    empty lines or gaps in non-empty lines. Dock windows can be lined
    up to minimize wasted space using the lineUp() function.

    The TQDockArea class maintains a position list of all its child
    dock windows. Dock windows are added to a dock area from position
    0 onwards. Dock windows are laid out sequentially in position
    order from left to right, and in the case of multiple lines of
    dock windows, from top to bottom. If a dock window is floated it
    still retains its position since this is where the window will
    return if the user double clicks its caption. A dock window's
    position can be determined with hasDockWindow(). The position can
    be changed with moveDockWindow().

    To dock or undock a dock window use TQDockWindow::dock() and
    TQDockWindow::undock() respectively. If you want to control which
    dock windows can dock in a dock area use setAcceptDockWindow(). To
    see if a dock area contains a particular dock window use
    \l{hasDockWindow()}; to see how many dock windows a dock area
    contains use count().

    The streaming operators can write the positions of the dock
    windows in the dock area to a TQTextStream. The positions can be
    read back later to restore the saved positions.

    Save the positions to a TQTextStream:
    \code
    ts << *myDockArea;
    \endcode

    Restore the positions from a TQTextStream:
    \code
    ts >> *myDockArea;
    \endcode
*/

/*!
    \property TQDockArea::handlePosition
    \brief where the dock window splitter handle is placed in the dock
    area

    The default position is \c Normal.
*/

/*!
    \property TQDockArea::orientation
    \brief the dock area's orientation

    There is no default value; the orientation is specified in the
    constructor.
*/

/*!
    \enum TQDockArea::HandlePosition

    A dock window has two kinds of handles, the dock window handle
    used for dragging the dock window, and the splitter handle used to
    resize the dock window in relation to other dock windows using a
    splitter. (The splitter handle is only visible for docked
    windows.)

    This enum specifies where the dock window splitter handle is
    placed in the dock area.

    \value Normal The splitter handles of dock windows are placed at
    the right or bottom.

    \value Reverse The splitter handles of dock windows are placed at
    the left or top.
*/

/*!
    Constructs a TQDockArea with orientation \a o, HandlePosition \a h,
    parent \a parent and called \a name.
*/

TQDockArea::TQDockArea( Orientation o, HandlePosition h, TQWidget *parent, const char *name )
    : TQWidget( parent, name ), orient( o ), layout( 0 ), hPos( h )
{
    dockWindows = new TQPtrList<TQDockWindow>;
    layout = new TQDockAreaLayout( this, o, dockWindows, 0, 0, "toollayout" );
    installEventFilter( this );
}

/*!
    Destroys the dock area and all the dock windows docked in the dock
    area.

    Does not affect any floating dock windows or dock windows in other
    dock areas, even if they first appeared in this dock area.
    Floating dock windows are effectively top level windows and are
    not child windows of the dock area. When a floating dock window is
    docked (dragged into a dock area) its parent becomes the dock
    area.
*/

TQDockArea::~TQDockArea()
{
    dockWindows->setAutoDelete( true );
    delete dockWindows;
    dockWindows = 0;
}

/*!
    Moves the TQDockWindow \a w within the dock area. If \a w is not
    already docked in this area, \a w is docked first. If \a index is
    -1 or larger than the number of docked widgets, \a w is appended
    at the end, otherwise it is inserted at the position \a index.
*/

void TQDockArea::moveDockWindow( TQDockWindow *w, int index )
{
    invalidateFixedSizes();
    TQDockWindow *dockWindow = 0;
    int dockWindowIndex = findDockWindow( w );
    if ( dockWindowIndex == -1 ) {
	dockWindow = w;
	dockWindow->reparent( this, TQPoint( 0, 0 ), true );
	w->installEventFilter( this );
	updateLayout();
	setSizePolicy( TQSizePolicy( orientation() == Horizontal ? TQSizePolicy::Expanding : TQSizePolicy::Minimum,
				    orientation() == Vertical ? TQSizePolicy::Expanding : TQSizePolicy::Minimum ) );
	dockWindows->append( w );
    } else {
        if ( w->parent() != this )
	    w->reparent( this, TQPoint( 0, 0 ), true );
        if ( index == - 1 ) {
	    dockWindows->removeRef( w );
	    dockWindows->append( w );
        }
    }

    w->dockArea = this;
    w->curPlace = TQDockWindow::InDock;
    w->updateGui();

    if ( index != -1 && index < (int)dockWindows->count() ) {
	dockWindows->removeRef( w );
	dockWindows->insert( index, w );
    }
}

/*!
    Returns true if the dock area contains the dock window \a w;
    otherwise returns false. If \a index is not 0 it will be set as
    follows: if the dock area contains the dock window \a *index is
    set to \a w's index position; otherwise \a *index is set to -1.
*/

bool TQDockArea::hasDockWindow( TQDockWindow *w, int *index )
{
    int i = dockWindows->findRef( w );
    if ( index )
	*index = i;
    return i != -1;
}

int TQDockArea::lineOf( int index )
{
    TQPtrList<TQDockWindow> lineStarts = layout->lineStarts();
    int i = 0;
    for ( TQDockWindow *w = lineStarts.first(); w; w = lineStarts.next(), ++i ) {
	if ( dockWindows->find( w ) >= index )
	    return i;
    }
    return i;
}

/*!
    \overload

    Moves the dock window \a w inside the dock area where \a p is the
    new position (in global screen coordinates), \a r is the suggested
    rectangle of the dock window and \a swap specifies whether or not
    the orientation of the docked widget needs to be changed.

    This function is used internally by TQDockWindow. You shouldn't
    need to call it yourself.
*/

void TQDockArea::moveDockWindow( TQDockWindow *w, const TQPoint &p, const TQRect &r, bool swap )
{
    invalidateFixedSizes();
    int mse = -10;
    bool hasResizable = false;
    for ( TQDockWindow *dw = dockWindows->first(); dw; dw = dockWindows->next() ) {
	if ( dw->isHidden() )
	    continue;
	if ( dw->isResizeEnabled() )
	    hasResizable = true;
	if ( orientation() != TQt::Horizontal )
	    mse = TQMAX( TQMAX( dw->fixedExtent().width(), dw->width() ), mse );
	else
	    mse = TQMAX( TQMAX( dw->fixedExtent().height(), dw->height() ), mse );
    }
    if ( !hasResizable && w->isResizeEnabled() ) {
	if ( orientation() != TQt::Horizontal )
	    mse = TQMAX( w->fixedExtent().width(), mse );
	else
	    mse = TQMAX( w->fixedExtent().height(), mse );
    }

    TQDockWindow *dockWindow = 0;
    int dockWindowIndex = findDockWindow( w );
    TQPtrList<TQDockWindow> lineStarts = layout->lineStarts();
    TQValueList<TQRect> lines = layout->lineList();
    bool wasAloneInLine = false;
    TQPoint pos = mapFromGlobal( p );
    TQRect lr = *lines.at( lineOf( dockWindowIndex ) );
    if ( dockWindowIndex != -1 ) {
	if ( lineStarts.find( w ) != -1 &&
	     ( ( dockWindowIndex < (int)dockWindows->count() - 1 && lineStarts.find( dockWindows->at( dockWindowIndex + 1 ) ) != -1 ) ||
	       dockWindowIndex == (int)dockWindows->count() - 1 ) )
	    wasAloneInLine = true;
	dockWindow = dockWindows->take( dockWindowIndex );
	if ( !wasAloneInLine ) { // only do the pre-layout if the widget isn't the only one in its line
	    if ( lineStarts.findRef( dockWindow ) != -1 && dockWindowIndex < (int)dockWindows->count() )
		dockWindows->at( dockWindowIndex )->setNewLine( true );
	    layout->layoutItems( TQRect( 0, 0, width(), height() ), true );
	}
    } else {
	dockWindow = w;
	dockWindow->reparent( this, TQPoint( 0, 0 ), true );
	if ( swap )
	    dockWindow->resize( dockWindow->height(), dockWindow->width() );
	w->installEventFilter( this );
    }

    lineStarts = layout->lineStarts();
    lines = layout->lineList();

    TQRect rect = TQRect( mapFromGlobal( r.topLeft() ), r.size() );
    if ( orientation() == Horizontal && TQApplication::reverseLayout() ) {
	rect = TQRect( width() - rect.x() - rect.width(), rect.y(), rect.width(), rect.height() );
	pos.rx() = width() - pos.x();
    }
    dockWindow->setOffset( point_pos( rect.topLeft(), orientation() ) );
    if ( orientation() == Horizontal ) {
	int offs = dockWindow->offset();
	if ( width() - offs < dockWindow->minimumWidth() )
	    dockWindow->setOffset( width() - dockWindow->minimumWidth() );
    } else {
	int offs = dockWindow->offset();
	if ( height() - offs < dockWindow->minimumHeight() )
	    dockWindow->setOffset( height() - dockWindow->minimumHeight() );
    }

    if ( dockWindows->isEmpty() ) {
	dockWindows->append( dockWindow );
    } else {
	int dockLine = -1;
	bool insertLine = false;
	int i = 0;
	TQRect lineRect;
	// find the line which we touched with the mouse
	for ( TQValueList<TQRect>::Iterator it = lines.begin(); it != lines.end(); ++it, ++i ) {
	    if ( point_pos( pos, orientation(), true ) >= point_pos( (*it).topLeft(), orientation(), true ) &&
		 point_pos( pos, orientation(), true ) <= point_pos( (*it).topLeft(), orientation(), true ) +
		 size_extent( (*it).size(), orientation(), true ) ) {
		dockLine = i;
		lineRect = *it;
		break;
	    }
	}
	if ( dockLine == -1 ) { // outside the dock...
	    insertLine = true;
	    if ( point_pos( pos, orientation(), true ) < 0 ) // insert as first line
		dockLine = 0;
	    else
		dockLine = (int)lines.count(); // insert after the last line ### size_t/int cast
	} else { // inside the dock (we have found a dockLine)
	    if ( point_pos( pos, orientation(), true ) <
		 point_pos( lineRect.topLeft(), orientation(), true ) + 4 ) {	// mouse was at the very beginning of the line
		insertLine = true;					// insert a new line before that with the docking widget
	    } else if ( point_pos( pos, orientation(), true ) >
			point_pos( lineRect.topLeft(), orientation(), true ) +
			size_extent( lineRect.size(), orientation(), true ) - 4 ) {	// mouse was at the very and of the line
		insertLine = true;						// insert a line after that with the docking widget
		dockLine++;
	    }
	}

	if ( !insertLine && wasAloneInLine && lr.contains( pos ) ) // if we are alone in a line and just moved in there, re-insert it
	    insertLine = true;

#if defined(TQDOCKAREA_DEBUG)
	tqDebug( "insert in line %d, and insert that line: %d", dockLine, insertLine );
	tqDebug( "     (btw, we have %d lines)", lines.count() );
#endif
	TQDockWindow *dw = 0;
	if ( dockLine >= (int)lines.count() ) { // insert after last line
	    dockWindows->append( dockWindow );
	    dockWindow->setNewLine( true );
#if defined(TQDOCKAREA_DEBUG)
	    tqDebug( "insert at the end" );
#endif
	} else if ( dockLine == 0 && insertLine ) { // insert before first line
	    dockWindows->insert( 0, dockWindow );
	    dockWindows->at( 1 )->setNewLine( true );
#if defined(TQDOCKAREA_DEBUG)
	    tqDebug( "insert at the begin" );
#endif
	} else { // insert somewhere in between
	    // make sure each line start has a new line
	    for ( dw = lineStarts.first(); dw; dw = lineStarts.next() )
		dw->setNewLine( true );

	    // find the index of the first widget in the search line
	    int searchLine = dockLine;
#if defined(TQDOCKAREA_DEBUG)
	    tqDebug( "search line start of %d", searchLine );
#endif
	    TQDockWindow *lsw = lineStarts.at( searchLine );
	    int index = dockWindows->find( lsw );
	    if ( index == -1 ) { // the linestart widget hasn't been found, try to find it harder
		if ( lsw == w && dockWindowIndex <= (int)dockWindows->count())
		    index = dockWindowIndex;
		else
		    index = 0;
		if ( index < (int)dockWindows->count() )
		    (void)dockWindows->at( index ); // move current to index
	    }
#if defined(TQDOCKAREA_DEBUG)
	    tqDebug( "     which starts at %d", index );
#endif
	    if ( !insertLine ) { // if we insert the docking widget in the existing line
		// find the index for the widget
		bool inc = true;
		bool firstTime = true;
		for ( dw = dockWindows->current(); dw; dw = dockWindows->next() ) {
		    if ( orientation() == Horizontal )
			dw->setFixedExtentWidth( -1 );
		    else
			dw->setFixedExtentHeight( -1 );
		    if ( !firstTime && lineStarts.find( dw ) != -1 ) // we are in the next line, so break
			break;
		    if ( point_pos( pos, orientation() ) <
			 point_pos( fix_pos( dw ), orientation() ) + size_extent( dw->size(), orientation() ) / 2 ) {
			inc = false;
		    }
		    if ( inc )
			index++;
		    firstTime = false;
		}
#if defined(TQDOCKAREA_DEBUG)
		tqDebug( "insert at index: %d", index );
#endif
		// if we insert it just before a widget which has a new line, transfer the newline to the docking widget
		// but not if we didn't only mave a widget in its line which was alone in the line before
		if ( !( wasAloneInLine && lr.contains( pos ) )
		     && index >= 0 && index < (int)dockWindows->count() &&
		     dockWindows->at( index )->newLine() && lineOf( index ) == dockLine ) {
#if defined(TQDOCKAREA_DEBUG)
		    tqDebug( "get rid of the old newline and get me one" );
#endif
		    dockWindows->at( index )->setNewLine( false );
		    dockWindow->setNewLine( true );
		} else if ( wasAloneInLine && lr.contains( pos ) ) {
		    dockWindow->setNewLine( true );
		} else { // if we are somewhere in a line, get rid of the newline
		    dockWindow->setNewLine( false );
		}
	    } else { // insert in a new line, so make sure the dock widget and the widget which will be after it have a newline
#if defined(TQDOCKAREA_DEBUG)
		tqDebug( "insert a new line" );
#endif
		if ( index < (int)dockWindows->count() ) {
#if defined(TQDOCKAREA_DEBUG)
		    tqDebug( "give the widget at %d a newline", index );
#endif
		    TQDockWindow* nldw = dockWindows->at( index );
		    if ( nldw )
			nldw->setNewLine( true );
		}
#if defined(TQDOCKAREA_DEBUG)
		tqDebug( "give me a newline" );
#endif
		dockWindow->setNewLine( true );
	    }
	    // finally insert the widget
	    dockWindows->insert( index, dockWindow );
	}
    }

    if ( mse != -10 && w->isResizeEnabled() ) {
	if ( orientation() != TQt::Horizontal )
	    w->setFixedExtentWidth( TQMIN( TQMAX( w->minimumWidth(), mse ), w->sizeHint().width() ) );
	else
	    w->setFixedExtentHeight( TQMIN( TQMAX( w->minimumHeight(), mse ), w->sizeHint().height() ) );
    }

    updateLayout();
    setSizePolicy( TQSizePolicy( orientation() == Horizontal ? TQSizePolicy::Expanding : TQSizePolicy::Minimum,
				orientation() == Vertical ? TQSizePolicy::Expanding : TQSizePolicy::Minimum ) );
}

/*!
    Removes the dock window \a w from the dock area. If \a
    makeFloating is true, \a w gets floated, and if \a swap is true,
    the orientation of \a w gets swapped. If \a fixNewLines is true
    (the default) newlines in the area will be fixed.

    You should never need to call this function yourself. Use
    TQDockWindow::dock() and TQDockWindow::undock() instead.
*/

void TQDockArea::removeDockWindow( TQDockWindow *w, bool makeFloating, bool swap, bool fixNewLines )
{
    w->removeEventFilter( this );
    TQDockWindow *dockWindow = 0;
    int i = findDockWindow( w );
    if ( i == -1 )
	return;
    dockWindow = dockWindows->at( i );
    dockWindows->remove( i );
    TQPtrList<TQDockWindow> lineStarts = layout->lineStarts();
    if ( fixNewLines && lineStarts.findRef( dockWindow ) != -1 && i < (int)dockWindows->count() )
	dockWindows->at( i )->setNewLine( true );
    if ( makeFloating ) {
	TQWidget *p = parentWidget() ? parentWidget() : topLevelWidget();
	dockWindow->reparent( p, WType_Dialog | WStyle_Customize | WStyle_NoBorder | WStyle_Tool, TQPoint( 0, 0 ), false );
    }
    if ( swap )
	dockWindow->resize( dockWindow->height(), dockWindow->width() );
    updateLayout();
    if ( dockWindows->isEmpty() )
	setSizePolicy( TQSizePolicy( TQSizePolicy::Preferred, TQSizePolicy::Preferred ) );
}

int TQDockArea::findDockWindow( TQDockWindow *w )
{
    return dockWindows ? dockWindows->findRef( w ) : -1;
}

void TQDockArea::updateLayout()
{
    layout->invalidate();
    layout->activate();
}

/*! \reimp
 */

bool TQDockArea::eventFilter( TQObject *o, TQEvent *e )
{
    if ( e->type() == TQEvent::Close ) {
	if ( ::tqt_cast<TQDockWindow*>(o) ) {
	    o->removeEventFilter( this );
	    TQApplication::sendEvent( o, e );
	    if ( ( (TQCloseEvent*)e )->isAccepted() )
		removeDockWindow( (TQDockWindow*)o, false, false );
	    return true;
	}
    }
    return false;
}

/*! \internal

    Invalidates the offset of the next dock window in the dock area.
 */

void TQDockArea::invalidNextOffset( TQDockWindow *dw )
{
    int i = dockWindows->find( dw );
    if ( i == -1 || i >= (int)dockWindows->count() - 1 )
	return;
    if ( ( dw = dockWindows->at( ++i ) ) )
	dw->setOffset( 0 );
}

/*!
    \property TQDockArea::count
    \brief the number of dock windows in the dock area
*/
int TQDockArea::count() const
{
    return dockWindows->count();
}

/*!
    \property TQDockArea::empty
    \brief whether the dock area is empty
*/

bool TQDockArea::isEmpty() const
{
    return dockWindows->isEmpty();
}


/*!
    Returns a list of the dock windows in the dock area.
*/

TQPtrList<TQDockWindow> TQDockArea::dockWindowList() const
{
    return *dockWindows;
}

/*!
    Lines up the dock windows in this dock area to minimize wasted
    space. If \a keepNewLines is true, only space within lines is
    cleaned up. If \a keepNewLines is false the number of lines might
    be changed.
*/

void TQDockArea::lineUp( bool keepNewLines )
{
    for ( TQDockWindow *dw = dockWindows->first(); dw; dw = dockWindows->next() ) {
	dw->setOffset( 0 );
	if ( !keepNewLines )
	    dw->setNewLine( false );
    }
    layout->activate();
}

TQDockArea::DockWindowData *TQDockArea::dockWindowData( TQDockWindow *w )
{
    DockWindowData *data = new DockWindowData;
    data->index = findDockWindow( w );
    if ( data->index == -1 ) {
	delete data;
	return 0;
    }
    TQPtrList<TQDockWindow> lineStarts = layout->lineStarts();
    int i = -1;
    for ( TQDockWindow *dw = dockWindows->first(); dw; dw = dockWindows->next() ) {
	if ( lineStarts.findRef( dw ) != -1 )
	    ++i;
	if ( dw == w )
	    break;
    }
    data->line = i;
    data->offset = point_pos( TQPoint( fix_x(w), w->y() ), orientation() );
    data->area = this;
    data->fixedExtent = w->fixedExtent();
    return data;
}

void TQDockArea::dockWindow( TQDockWindow *dockWindow, DockWindowData *data )
{
    if ( !data )
	return;

    dockWindow->reparent( this, TQPoint( 0, 0 ), false );
    dockWindow->installEventFilter( this );
    dockWindow->dockArea = this;
    dockWindow->updateGui();

    if ( dockWindows->isEmpty() ) {
	dockWindows->append( dockWindow );
    } else {
	TQPtrList<TQDockWindow> lineStarts = layout->lineStarts();
	int index = 0;
	if ( (int)lineStarts.count() > data->line )
	    index = dockWindows->find( lineStarts.at( data->line ) );
	if ( index == -1 ) {
	    index = 0;
	    (void)dockWindows->at( index );
	}
	bool firstTime = true;
	int offset = data->offset;
	for ( TQDockWindow *dw = dockWindows->current(); dw; dw = dockWindows->next() ) {
	    if ( !firstTime && lineStarts.find( dw ) != -1 )
		break;
	    if ( offset <
		 point_pos( fix_pos( dw ), orientation() ) + size_extent( dw->size(), orientation() ) / 2 )
		break;
	    index++;
	    firstTime = false;
	}
	if ( index >= 0 && index < (int)dockWindows->count() &&
	     dockWindows->at( index )->newLine() && lineOf( index ) == data->line ) {
	    dockWindows->at( index )->setNewLine( false );
	    dockWindow->setNewLine( true );
	} else {
	    dockWindow->setNewLine( false );
	}

	dockWindows->insert( index, dockWindow );
    }
    dockWindow->show();

    dockWindow->setFixedExtentWidth( data->fixedExtent.width() );
    dockWindow->setFixedExtentHeight( data->fixedExtent.height() );

    updateLayout();
    setSizePolicy( TQSizePolicy( orientation() == Horizontal ? TQSizePolicy::Expanding : TQSizePolicy::Minimum,
				orientation() == Vertical ? TQSizePolicy::Expanding : TQSizePolicy::Minimum ) );

}

/*!
    Returns true if dock window \a dw could be docked into the dock
    area; otherwise returns false.

    \sa setAcceptDockWindow()
*/

bool TQDockArea::isDockWindowAccepted( TQDockWindow *dw )
{
    if ( !dw )
	return false;
    if ( forbiddenWidgets.findRef( dw ) != -1 )
	return false;

    TQMainWindow *mw = ::tqt_cast<TQMainWindow*>(parentWidget());
    if ( !mw )
	return true;
    if ( !mw->hasDockWindow( dw ) )
	return false;
    if ( !mw->isDockEnabled( this ) )
	return false;
    if ( !mw->isDockEnabled( dw, this ) )
	return false;
    return true;
}

/*!
    If \a accept is true, dock window \a dw can be docked in the dock
    area. If \a accept is false, dock window \a dw cannot be docked in
    the dock area.

    \sa isDockWindowAccepted()
*/

void TQDockArea::setAcceptDockWindow( TQDockWindow *dw, bool accept )
{
    if ( accept )
	forbiddenWidgets.removeRef( dw );
    else if ( forbiddenWidgets.findRef( dw ) == -1 )
	forbiddenWidgets.append( dw );
}

void TQDockArea::invalidateFixedSizes()
{
    for ( TQDockWindow *dw = dockWindows->first(); dw; dw = dockWindows->next() ) {
	if ( orientation() == TQt::Horizontal )
	    dw->setFixedExtentWidth( -1 );
	else
	    dw->setFixedExtentHeight( -1 );
    }
}

int TQDockArea::maxSpace( int hint, TQDockWindow *dw )
{
    int index = findDockWindow( dw );
    if ( index == -1 || index + 1 >= (int)dockWindows->count() ) {
	if ( orientation() == Horizontal )
	    return dw->width();
	return dw->height();
    }

    TQDockWindow *w = 0;
    int i = 0;
    do {
	w = dockWindows->at( index + (++i) );
    } while ( i + 1 < (int)dockWindows->count() && ( !w || w->isHidden() ) );
    if ( !w || !w->isResizeEnabled() || i >= (int)dockWindows->count() ) {
	if ( orientation() == Horizontal )
	    return dw->width();
	return dw->height();
    }
    int min = 0;
    TQToolBar *tb = ::tqt_cast<TQToolBar*>(w);
    if ( orientation() == Horizontal ) {
	w->setFixedExtentWidth( -1 );
	if ( !tb )
	    min = TQMAX( w->minimumSize().width(), w->minimumSizeHint().width() );
	else
	    min = w->sizeHint().width();
    } else {
	w->setFixedExtentHeight( -1 );
	if ( !tb )
	    min = TQMAX( w->minimumSize().height(), w->minimumSizeHint().height() );
	else
	    min = w->sizeHint().height();
    }

    int diff = hint - ( orientation() == Horizontal ? dw->width() : dw->height() );

    if ( ( orientation() == Horizontal ? w->width() : w->height() ) - diff < min )
	hint = ( orientation() == Horizontal ? dw->width() : dw->height() ) + ( orientation() == Horizontal ? w->width() : w->height() ) - min;

    diff = hint - ( orientation() == Horizontal ? dw->width() : dw->height() );
    if ( orientation() == Horizontal )
	w->setFixedExtentWidth( w->width() - diff );
    else
	w->setFixedExtentHeight( w->height() - diff );
    return hint;
}

void TQDockArea::setFixedExtent( int d, TQDockWindow *dw )
{
    TQPtrList<TQDockWindow> lst;
    TQDockWindow *w;
    for ( w = dockWindows->first(); w; w = dockWindows->next() ) {
	if ( w->isHidden() )
	    continue;
	if ( orientation() == Horizontal ) {
	    if ( dw->y() != w->y() )
		continue;
	} else {
	    if ( dw->x() != w->x() )
		continue;
	}
	if ( orientation() == Horizontal )
	    d = TQMAX( d, w->minimumHeight() );
	else
	    d = TQMAX( d, w->minimumWidth() );
	if ( w->isResizeEnabled() )
	    lst.append( w );
    }
    for ( w = lst.first(); w; w = lst.next() ) {
	if ( orientation() == Horizontal )
	    w->setFixedExtentHeight( d );
	else
	    w->setFixedExtentWidth( d );
    }
}

bool TQDockArea::isLastDockWindow( TQDockWindow *dw )
{
    int i = dockWindows->find( dw );
    if ( i == -1 || i >= (int)dockWindows->count() - 1 )
	return true;
    TQDockWindow *w = 0;
    if ( ( w = dockWindows->at( ++i ) ) ) {
	if ( orientation() == Horizontal && dw->y() < w->y() )
	    return true;
	if ( orientation() == Vertical && dw->x() < w->x() )
	    return true;
    } else {
	return true;
    }
    return false;
}

#ifndef TQT_NO_TEXTSTREAM

/*!
    \relates TQDockArea

    Writes the layout of the dock windows in dock area \a dockArea to
    the text stream \a ts.

    \sa operator>>()
*/

TQTextStream &operator<<( TQTextStream &ts, const TQDockArea &dockArea )
{
    TQString str;
    TQPtrList<TQDockWindow> l = dockArea.dockWindowList();

    for ( TQDockWindow *dw = l.first(); dw; dw = l.next() )
	str += "[" + TQString( dw->caption() ) + "," + TQString::number( (int)dw->offset() ) +
	       "," + TQString::number( (int)dw->newLine() ) + "," + TQString::number( dw->fixedExtent().width() ) +
	       "," + TQString::number( dw->fixedExtent().height() ) + "," + TQString::number( (int)!dw->isHidden() ) + "]";
    ts << str << endl;

    return ts;
}

/*!
    \relates TQDockArea

    Reads the layout description of the dock windows in dock area \a
    dockArea from the text stream \a ts and restores it. The layout
    description must have been previously written by the operator<<()
    function.

    \sa operator<<()
*/

TQTextStream &operator>>( TQTextStream &ts, TQDockArea &dockArea )
{
    TQString s = ts.readLine();

    TQString name, offset, newLine, width, height, visible;

    enum State { Pre, Name, Offset, NewLine, Width, Height, Visible, Post };
    int state = Pre;
    TQChar c;
    TQPtrList<TQDockWindow> l = dockArea.dockWindowList();

    for ( int i = 0; i < (int)s.length(); ++i ) {
	c = s[ i ];
	if ( state == Pre && c == '[' ) {
	    state++;
	    continue;
	}
	if ( c == ',' &&
	     ( state == Name || state == Offset || state == NewLine || state == Width || state == Height ) ) {
	    state++;
	    continue;
	}
	if ( state == Visible && c == ']' ) {
	    for ( TQDockWindow *dw = l.first(); dw; dw = l.next() ) {
		if ( TQString( dw->caption() ) == name ) {
		    dw->setNewLine( (bool)newLine.toInt() );
		    dw->setOffset( offset.toInt() );
		    dw->setFixedExtentWidth( width.toInt() );
		    dw->setFixedExtentHeight( height.toInt() );
		    if ( !(bool)visible.toInt() )
			dw->hide();
		    else
			dw->show();
		    break;
		}
	    }

	    name = offset = newLine = width = height = visible = "";

	    state = Pre;
	    continue;
	}
	if ( state == Name )
	    name += c;
	else if ( state == Offset )
	    offset += c;
	else if ( state == NewLine )
	    newLine += c;
	else if ( state == Width )
	    width += c;
	else if ( state == Height )
	    height += c;
	else if ( state == Visible )
	    visible += c;
    }

    dockArea.TQWidget::layout()->invalidate();
    dockArea.TQWidget::layout()->activate();
    return ts;
}
#endif

#endif //TQT_NO_MAINWINDOW
