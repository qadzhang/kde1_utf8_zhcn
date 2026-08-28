/****************************************************************************
**
** Implementation of TQScrollView class
**
** Created : 950524
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

#include "ntqwidget.h"
#ifndef TQT_NO_SCROLLVIEW
#include "ntqscrollbar.h"
#include "ntqobjectlist.h"
#include "ntqpainter.h"
#include "ntqpixmap.h"
#include "ntqcursor.h"
#include "ntqfocusdata.h"
#include "ntqscrollview.h"
#include "ntqptrdict.h"
#include "ntqapplication.h"
#include "ntqtimer.h"
#include "ntqstyle.h"
#include "ntqlistview.h"
#ifdef TQ_WS_MAC
# include "qt_mac.h"
#endif

static const int coord_limit = 4000;
static const int autoscroll_margin = 16;
static const int initialScrollTime = 30;
static const int initialScrollAccel = 5;

struct TQSVChildRec {
    TQSVChildRec(TQWidget* c, int xx, int yy) :
        child(c),
        x(xx), y(yy)
    {
    }

    void hideOrShow(TQScrollView* sv, TQWidget* clipped_viewport);
    void moveTo(TQScrollView* sv, int xx, int yy, TQWidget* clipped_viewport)
    {
        if ( x != xx || y != yy ) {
            x = xx;
            y = yy;
            hideOrShow(sv,clipped_viewport);
        }
    }
    TQWidget* child;
    int x, y;
};

void TQSVChildRec::hideOrShow(TQScrollView* sv, TQWidget* clipped_viewport)
{
    if ( clipped_viewport ) {
	if ( x+child->width() < sv->contentsX()+clipped_viewport->x()
	     || x > sv->contentsX()+clipped_viewport->width()
	     || y+child->height() < sv->contentsY()+clipped_viewport->y()
	     || y > sv->contentsY()+clipped_viewport->height() ) {
	    child->move(clipped_viewport->width(),
			clipped_viewport->height());
	} else {
	    child->move(x-sv->contentsX()-clipped_viewport->x(),
			y-sv->contentsY()-clipped_viewport->y());
	}
    } else {
	child->move(x-sv->contentsX(), y-sv->contentsY());
    }
}

class TQViewportWidget : public TQWidget
{
    TQ_OBJECT

public:
    TQViewportWidget( TQScrollView* parent=0, const char* name=0, WFlags f = 0 )
	: TQWidget( parent, name, f ) {}
};

class TQClipperWidget : public TQWidget
{
    TQ_OBJECT

public:
    TQClipperWidget( TQWidget * parent=0, const char * name=0, WFlags f=0 )
        : TQWidget ( parent,name,f) {}
};

#include "qscrollview.moc"

class TQScrollViewData {
public:
    TQScrollViewData(TQScrollView* parent, int vpwflags) :
        hbar( new TQScrollBar( TQScrollBar::Horizontal, parent, "qt_hbar" ) ),
        vbar( new TQScrollBar( TQScrollBar::Vertical, parent, "qt_vbar" ) ),
        viewport( new TQViewportWidget( parent, "qt_viewport", vpwflags ) ),
        clipped_viewport( 0 ),
        flags( vpwflags ),
        vx( 0 ), vy( 0 ), vwidth( 1 ), vheight( 1 ),
#ifndef TQT_NO_DRAGANDDROP
        autoscroll_timer( parent, "scrollview autoscroll timer" ),
	drag_autoscroll( true ),
#endif
	scrollbar_timer( parent, "scrollview scrollbar timer" ),
        inresize( false ), use_cached_size_hint( true )
    {
	l_marg = r_marg = t_marg = b_marg = 0;
	viewport->polish();
	viewport->setBackgroundMode( TQWidget::PaletteDark );
	viewport->setBackgroundOrigin( TQWidget::WidgetOrigin );
	vMode = TQScrollView::Auto;
	hMode = TQScrollView::Auto;
	corner = 0;
	defaultCorner = new TQWidget( parent, "qt_default_corner" );
	defaultCorner->hide();
	vbar->setSteps( 20, 1/*set later*/ );
	hbar->setSteps( 20, 1/*set later*/ );
	policy = TQScrollView::Default;
	signal_choke = false;
	static_bg = false;
	fake_scroll = false;
	hbarPressed = false;
	vbarPressed = false;
    }
    ~TQScrollViewData();

    TQSVChildRec* rec(TQWidget* w) { return childDict.find(w); }
    TQSVChildRec* ancestorRec(TQWidget* w);
    TQSVChildRec* addChildRec(TQWidget* w, int x, int y )
    {
        TQSVChildRec *r = new TQSVChildRec(w,x,y);
        children.append(r);
        childDict.insert(w, r);
        return r;
    }
    void deleteChildRec(TQSVChildRec* r)
    {
        childDict.remove(r->child);
        children.removeRef(r);
        delete r;
    }

    void hideOrShowAll(TQScrollView* sv, bool isScroll = false );
    void moveAllBy(int dx, int dy);
    bool anyVisibleChildren();
    void autoMove(TQScrollView* sv);
    void autoResize(TQScrollView* sv);
    void autoResizeHint(TQScrollView* sv);
    void viewportResized( int w, int h );

    TQScrollBar*  hbar;
    TQScrollBar*  vbar;
    bool hbarPressed;
    bool vbarPressed;
    TQViewportWidget*	viewport;
    TQClipperWidget*	clipped_viewport;
    int         flags;
    TQPtrList<TQSVChildRec>       children;
    TQPtrDict<TQSVChildRec>       childDict;
    TQWidget*    corner, *defaultCorner;
    int         vx, vy, vwidth, vheight; // for drawContents-style usage
    int         l_marg, r_marg, t_marg, b_marg;
    TQScrollView::ResizePolicy policy;
    TQScrollView::ScrollBarMode  vMode;
    TQScrollView::ScrollBarMode  hMode;
#ifndef TQT_NO_DRAGANDDROP
    TQPoint cpDragStart;
    TQTimer autoscroll_timer;
    int autoscroll_time;
    int autoscroll_accel;
    bool drag_autoscroll;
#endif
    TQTimer scrollbar_timer;

    uint static_bg : 1;
    uint fake_scroll : 1;

    // This variable allows ensureVisible to move the contents then
    // update both the sliders.  Otherwise, updating the sliders would
    // cause two image scrolls, creating ugly flashing.
    //
    uint signal_choke : 1;

    // This variables indicates in updateScrollBars() that we are
    // in a resizeEvent() and thus don't want to flash scrollbars
    uint inresize : 1;
    uint use_cached_size_hint : 1;
    TQSize cachedSizeHint;

    inline int contentsX() const { return -vx; }
    inline int contentsY() const { return -vy; }
    inline int contentsWidth() const { return vwidth; }
};

inline TQScrollViewData::~TQScrollViewData()
{
    children.setAutoDelete( true );
}

TQSVChildRec* TQScrollViewData::ancestorRec(TQWidget* w)
{
    if ( clipped_viewport ) {
	while (w->parentWidget() != clipped_viewport) {
	    w = w->parentWidget();
	    if (!w) return 0;
	}
    } else {
	while (w->parentWidget() != viewport) {
	    w = w->parentWidget();
	    if (!w) return 0;
	}
    }
    return rec(w);
}

void TQScrollViewData::hideOrShowAll(TQScrollView* sv, bool isScroll )
{
    if ( !clipped_viewport )
	return;
    if ( clipped_viewport->x() <= 0
	 && clipped_viewport->y() <= 0
	 && clipped_viewport->width()+clipped_viewport->x() >=
	 viewport->width()
	 && clipped_viewport->height()+clipped_viewport->y() >=
	 viewport->height() ) {
	// clipped_viewport still covers viewport
	if( static_bg )
	    clipped_viewport->repaint( true );
	else if ( ( !isScroll && !clipped_viewport->testWFlags( TQt::WStaticContents) )
		  || static_bg )
	    TQApplication::postEvent( clipped_viewport,
		     new TQPaintEvent( clipped_viewport->clipRegion(),
			      !clipped_viewport->testWFlags(TQt::WResizeNoErase) ) );
    } else {
	// Re-center
	int nx = ( viewport->width() - clipped_viewport->width() ) / 2;
	int ny = ( viewport->height() - clipped_viewport->height() ) / 2;
	clipped_viewport->move(nx,ny);
        clipped_viewport->update();
    }
    for (TQSVChildRec *r = children.first(); r; r=children.next()) {
	r->hideOrShow(sv, clipped_viewport);
    }
}

void TQScrollViewData::moveAllBy(int dx, int dy)
{
    if ( clipped_viewport && !static_bg ) {
	clipped_viewport->move( clipped_viewport->x()+dx,
				clipped_viewport->y()+dy );
    } else {
	for (TQSVChildRec *r = children.first(); r; r=children.next()) {
	    r->child->move(r->child->x()+dx,r->child->y()+dy);
	}
	if ( static_bg )
	    viewport->repaint( true );
    }
}

bool TQScrollViewData::anyVisibleChildren()
{
    for (TQSVChildRec *r = children.first(); r; r=children.next()) {
	if (r->child->isVisible()) return true;
    }
    return false;
}

void TQScrollViewData::autoMove(TQScrollView* sv)
{
    if ( policy == TQScrollView::AutoOne ) {
	TQSVChildRec* r = children.first();
	if (r)
	    sv->setContentsPos(-r->child->x(),-r->child->y());
    }
}

void TQScrollViewData::autoResize(TQScrollView* sv)
{
    if ( policy == TQScrollView::AutoOne ) {
	TQSVChildRec* r = children.first();
	if (r)
	    sv->resizeContents(r->child->width(),r->child->height());
    }
}

void TQScrollViewData::autoResizeHint(TQScrollView* sv)
{
    if ( policy == TQScrollView::AutoOne ) {
	TQSVChildRec* r = children.first();
	if (r) {
	    TQSize s = r->child->sizeHint();
	    if ( s.isValid() )
		r->child->resize(s);
	}
    } else if ( policy == TQScrollView::AutoOneFit ) {
	TQSVChildRec* r = children.first();
	if (r) {
	    TQSize sh = r->child->sizeHint();
	    sh = sh.boundedTo( r->child->maximumSize() );
	    sv->resizeContents( sh.width(), sh.height() );
	}
    }
}

void TQScrollViewData::viewportResized( int w, int h )
{
    if ( policy == TQScrollView::AutoOneFit ) {
	TQSVChildRec* r = children.first();
	if (r) {
	    TQSize sh = r->child->sizeHint();
	    sh = sh.boundedTo( r->child->maximumSize() );
	    r->child->resize( TQMAX(w,sh.width()), TQMAX(h,sh.height()) );
	}

    }
}


/*!
    \class TQScrollView ntqscrollview.h
    \brief The TQScrollView widget provides a scrolling area with on-demand scroll bars.

    \ingroup abstractwidgets
    \mainclass

    The TQScrollView is a large canvas - potentially larger than the
    coordinate system normally supported by the underlying window
    system. This is important because it is quite easy to go beyond
    these limitations (e.g. many web pages are more than 32000 pixels
    high). Additionally, the TQScrollView can have TQWidgets positioned
    on it that scroll around with the drawn content. These sub-widgets
    can also have positions outside the normal coordinate range (but
    they are still limited in size).

    To provide content for the widget, inherit from TQScrollView,
    reimplement drawContents() and use resizeContents() to set the
    size of the viewed area. Use addChild() and moveChild() to
    position widgets on the view.

    To use TQScrollView effectively it is important to understand its
    widget structure in the three styles of use: a single large child
    widget, a large panning area with some widgets and a large panning
    area with many widgets.

    \section1 Using One Big Widget

    \img qscrollview-vp2.png

    The first, simplest usage of TQScrollView (depicted above), is
    appropriate for scrolling areas that are never more than about
    4000 pixels in either dimension (this is about the maximum
    reliable size on X11 servers). In this usage, you just make one
    large child in the TQScrollView. The child should be a child of the
    viewport() of the scrollview and be added with addChild():
    \code
	TQScrollView* sv = new TQScrollView(...);
	TQVBox* big_box = new TQVBox(sv->viewport());
	sv->addChild(big_box);
    \endcode
    You can go on to add arbitrary child widgets to the single child
    in the scrollview as you would with any widget:
    \code
	TQLabel* child1 = new TQLabel("CHILD", big_box);
	TQLabel* child2 = new TQLabel("CHILD", big_box);
	TQLabel* child3 = new TQLabel("CHILD", big_box);
	...
    \endcode

    Here the TQScrollView has four children: the viewport(), the
    verticalScrollBar(), the horizontalScrollBar() and a small
    cornerWidget(). The viewport() has one child: the big TQVBox. The
    TQVBox has the three TQLabel objects as child widgets. When the view
    is scrolled, the TQVBox is moved; its children move with it as
    child widgets normally do.

    \section1 Using a Very Big View with Some Widgets

    \img qscrollview-vp.png

    The second usage of TQScrollView (depicted above) is appropriate
    when few, if any, widgets are on a very large scrolling area that
    is potentially larger than 4000 pixels in either dimension. In
    this usage you call resizeContents() to set the size of the area
    and reimplement drawContents() to paint the contents. You may also
    add some widgets by making them children of the viewport() and
    adding them with addChild() (this is the same as the process for
    the single large widget in the previous example):
    \code
	TQScrollView* sv = new TQScrollView(...);
	TQLabel* child1 = new TQLabel("CHILD", sv->viewport());
	sv->addChild(child1);
	TQLabel* child2 = new TQLabel("CHILD", sv->viewport());
	sv->addChild(child2);
	TQLabel* child3 = new TQLabel("CHILD", sv->viewport());
	sv->addChild(child3);
    \endcode
    Here, the TQScrollView has the same four children: the viewport(),
    the verticalScrollBar(), the horizontalScrollBar() and a small
    cornerWidget(). The viewport() has the three TQLabel objects as
    child widgets. When the view is scrolled, the scrollview moves the
    child widgets individually.

    \section1 Using a Very Big View with Many Widgets

    \target enableclipper
    \img qscrollview-cl.png

    The final usage of TQScrollView (depicted above) is appropriate
    when many widgets are on a very large scrolling area that is
    potentially larger than 4000 pixels in either dimension. In this
    usage you call resizeContents() to set the size of the area and
    reimplement drawContents() to paint the contents. You then call
    enableClipper(true) and add widgets, again by making them children
    of the viewport(), and adding them with addChild():
    \code
	TQScrollView* sv = new TQScrollView(...);
	sv->enableClipper(true);
	TQLabel* child1 = new TQLabel("CHILD", sv->viewport());
	sv->addChild(child1);
	TQLabel* child2 = new TQLabel("CHILD", sv->viewport());
	sv->addChild(child2);
	TQLabel* child3 = new TQLabel("CHILD", sv->viewport());
	sv->addChild(child3);
    \endcode

    Here, the TQScrollView has four children:  the clipper() (not the
    viewport() this time), the verticalScrollBar(), the
    horizontalScrollBar() and a small cornerWidget(). The clipper()
    has one child: the viewport(). The viewport() has the same three
    labels as child widgets. When the view is scrolled the viewport()
    is moved; its children move with it as child widgets normally do.

    \target allviews
    \section1 Details Relevant for All Views

    Normally you will use the first or third method if you want any
    child widgets in the view.

    Note that the widget you see in the scrolled area is the
    viewport() widget, not the TQScrollView itself. So to turn mouse
    tracking on, for example, use viewport()->setMouseTracking(true).

    To enable drag-and-drop, you would setAcceptDrops(true) on the
    TQScrollView (because drag-and-drop events propagate to the
    parent). But to work out the logical position in the view, you
    would need to map the drop co-ordinate from being relative to the
    TQScrollView to being relative to the contents; use the function
    viewportToContents() for this.

    To handle mouse events on the scrolling area, subclass scrollview
    as you would subclass other widgets, but rather than
    reimplementing mousePressEvent(), reimplement
    contentsMousePressEvent() instead. The contents specific event
    handlers provide translated events in the coordinate system of the
    scrollview. If you reimplement mousePressEvent(), you'll get
    called only when part of the TQScrollView is clicked: and the only
    such part is the "corner" (if you don't set a cornerWidget()) and
    the frame; everything else is covered up by the viewport, clipper
    or scroll bars.

    When you construct a TQScrollView, some of the widget flags apply
    to the viewport() instead of being sent to the TQWidget constructor
    for the TQScrollView. This applies to \c WNoAutoErase, \c
    WStaticContents, and \c WPaintClever. See \l TQt::WidgetFlags for
    documentation about these flags. Here are some examples:

    \list

    \i An image-manipulation widget would use \c
    WNoAutoErase|WStaticContents because the widget draws all pixels
    itself, and when its size increases, it only needs a paint event
    for the new part because the old part remains unchanged.

    \i A scrolling game widget in which the background scrolls as the
    characters move might use \c WNoAutoErase (in addition to \c
    WStaticContents) so that the window system background does not
    flash in and out during scrolling.

    \i A word processing widget might use \c WNoAutoErase and repaint
    itself line by line to get a less-flickery resizing. If the widget
    is in a mode in which no text justification can take place, it
    might use \c WStaticContents too, so that it would only get a
    repaint for the newly visible parts.

    \endlist

    Child widgets may be moved using addChild() or moveChild(). Use
    childX() and childY() to get the position of a child widget.

    A widget may be placed in the corner between the vertical and
    horizontal scrollbars with setCornerWidget(). You can get access
    to the scrollbars using horizontalScrollBar() and
    verticalScrollBar(), and to the viewport with viewport(). The
    scroll view can be scrolled using scrollBy(), ensureVisible(),
    setContentsPos() or center().

    The visible area is given by visibleWidth() and visibleHeight(),
    and the contents area by contentsWidth() and contentsHeight(). The
    contents may be repainted using one of the repaintContents() or
    updateContents() functions.

    Coordinate conversion is provided by contentsToViewport() and
    viewportToContents().

    The contentsMoving() signal is emitted just before the contents
    are moved to a new position.

    \warning TQScrollView currently does not erase the background when
    resized, i.e. you must always clear the background manually in
    scrollview subclasses. This will change in a future version of TQt
    and we recommend specifying the WNoAutoErase flag explicitly.

    <img src=qscrollview-m.png> <img src=qscrollview-w.png>
*/


/*!
    \enum TQScrollView::ResizePolicy

    This enum type is used to control a TQScrollView's reaction to
    resize events.

    \value Default  the TQScrollView selects one of the other settings
    automatically when it has to. In this version of TQt, TQScrollView
    changes to \c Manual if you resize the contents with
    resizeContents() and to \c AutoOne if a child is added.

    \value Manual  the contents stays the size set by resizeContents().

    \value AutoOne  if there is only one child widget the contents stays
    the size of that widget. Otherwise the behavior is undefined.

    \value AutoOneFit if there is only one child widget the contents stays
    the size of that widget's sizeHint(). If the scrollview is resized
    larger than the child's sizeHint(), the child will be resized to
    fit. If there is more than one child, the behavior is undefined.

*/
//####  The widget will be resized to its sizeHint() when a LayoutHint event
//#### is received

/*!
    Constructs a TQScrollView called \a name with parent \a parent and
    widget flags \a f.

    The widget flags \c WStaticContents, \c WNoAutoErase and \c
    WPaintClever are propagated to the viewport() widget. The other
    widget flags are propagated to the parent constructor as usual.
*/

TQScrollView::TQScrollView( TQWidget *parent, const char *name, WFlags f ) :
    TQFrame( parent, name, f & (~WStaticContents) & (~WResizeNoErase) )
{
    WFlags flags = WResizeNoErase | (f&WPaintClever) | (f&WRepaintNoErase) | (f&WStaticContents);
    d = new TQScrollViewData( this, flags );

#ifndef TQT_NO_DRAGANDDROP
    connect( &d->autoscroll_timer, TQ_SIGNAL( timeout() ),
             this, TQ_SLOT( doDragAutoScroll() ) );
#endif

    connect( d->hbar, TQ_SIGNAL( valueChanged(int) ),
        this, TQ_SLOT( hslide(int) ) );
    connect( d->vbar, TQ_SIGNAL( valueChanged(int) ),
        this, TQ_SLOT( vslide(int) ) );

    connect( d->hbar, TQ_SIGNAL(sliderPressed()), this, TQ_SLOT(hbarIsPressed()) );
    connect( d->hbar, TQ_SIGNAL(sliderReleased()), this, TQ_SLOT(hbarIsReleased()) );
    connect( d->vbar, TQ_SIGNAL(sliderPressed()), this, TQ_SLOT(vbarIsPressed()) );
    connect( d->vbar, TQ_SIGNAL(sliderReleased()), this, TQ_SLOT(vbarIsReleased()) );


    d->viewport->installEventFilter( this );

    connect( &d->scrollbar_timer, TQ_SIGNAL( timeout() ),
             this, TQ_SLOT( updateScrollBars() ) );

    setFrameStyle( TQFrame::StyledPanel | TQFrame::Sunken );
    setLineWidth( style().pixelMetric(TQStyle::PM_DefaultFrameWidth, this) );
    setSizePolicy( TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Expanding ) );
}


/*!
    Destroys the TQScrollView. Any children added with addChild() will
    be deleted.
*/
TQScrollView::~TQScrollView()
{
    // Be careful not to get all those useless events...
    if ( d->clipped_viewport )
        d->clipped_viewport->removeEventFilter( this );
    else
        d->viewport->removeEventFilter( this );

    // order is important
    // ~TQWidget may cause a WM_ERASEBKGND on Windows
    delete d->vbar;
    d->vbar = 0;
    delete d->hbar;
    d->hbar = 0;
    delete d->viewport;
    d->viewport = 0;
    delete d;
    d = 0;
}

/*!
    \fn void TQScrollView::horizontalSliderPressed()

    This signal is emitted whenever the user presses the horizontal slider.
*/
/*!
    \fn void TQScrollView::horizontalSliderReleased()

    This signal is emitted whenever the user releases the horizontal slider.
*/
/*!
    \fn void TQScrollView::verticalSliderPressed()

    This signal is emitted whenever the user presses the vertical slider.
*/
/*!
    \fn void TQScrollView::verticalSliderReleased()

    This signal is emitted whenever the user releases the vertical slider.
*/
void TQScrollView::hbarIsPressed()
{
    d->hbarPressed = true;
    emit( horizontalSliderPressed() );
}

void TQScrollView::hbarIsReleased()
{
    d->hbarPressed = false;
    emit( horizontalSliderReleased() );
}

/*!
    Returns true if horizontal slider is pressed by user; otherwise returns false.
*/
bool TQScrollView::isHorizontalSliderPressed()
{
    return d->hbarPressed;
}

void TQScrollView::vbarIsPressed()
{
    d->vbarPressed = true;
    emit( verticalSliderPressed() );
}

void TQScrollView::vbarIsReleased()
{
    d->vbarPressed = false;
    emit( verticalSliderReleased() );
}

/*!
    Returns true if vertical slider is pressed by user; otherwise returns false.
*/
bool TQScrollView::isVerticalSliderPressed()
{
    return d->vbarPressed;
}

/*!
    \reimp
*/
void TQScrollView::styleChange( TQStyle& old )
{
    TQWidget::styleChange( old );
    updateScrollBars();
    d->cachedSizeHint = TQSize();
}

/*!
    \reimp
*/
void TQScrollView::fontChange( const TQFont &old )
{
    TQWidget::fontChange( old );
    updateScrollBars();
    d->cachedSizeHint = TQSize();
}

void TQScrollView::hslide( int pos )
{
    if ( !d->signal_choke ) {
        moveContents( -pos, -d->contentsY() );
        TQApplication::syncX();
    }
}

void TQScrollView::vslide( int pos )
{
    if ( !d->signal_choke ) {
        moveContents( -d->contentsX(), -pos );
        TQApplication::syncX();
    }
}

/*!
    Called when the horizontal scroll bar geometry changes. This is
    provided as a protected function so that subclasses can do
    interesting things such as providing extra buttons in some of the
    space normally used by the scroll bars.

    The default implementation simply gives all the space to \a hbar.
    The new geometry is given by \a x, \a y, \a w and \a h.

    \sa setVBarGeometry()
*/
void TQScrollView::setHBarGeometry(TQScrollBar& hbar,
    int x, int y, int w, int h)
{
    hbar.setGeometry( x, y, w, h );
}

/*!
    Called when the vertical scroll bar geometry changes. This is
    provided as a protected function so that subclasses can do
    interesting things such as providing extra buttons in some of the
    space normally used by the scroll bars.

    The default implementation simply gives all the space to \a vbar.
    The new geometry is given by \a x, \a y, \a w and \a h.

    \sa setHBarGeometry()
*/
void TQScrollView::setVBarGeometry( TQScrollBar& vbar,
    int x, int y, int w, int h)
{
    vbar.setGeometry( x, y, w, h );
}


/*!
    Returns the viewport size for size (\a x, \a y).

    The viewport size depends on \a (x, y) (the size of the contents),
    the size of this widget and the modes of the horizontal and
    vertical scroll bars.

    This function permits widgets that can trade vertical and
    horizontal space for each other to control scroll bar appearance
    better. For example, a word processor or web browser can control
    the width of the right margin accurately, whether or not there
    needs to be a vertical scroll bar.
*/

TQSize TQScrollView::viewportSize( int x, int y ) const
{
    int fw = frameWidth();
    int lmarg = fw+d->l_marg;
    int rmarg = fw+d->r_marg;
    int tmarg = fw+d->t_marg;
    int bmarg = fw+d->b_marg;

    int w = width();
    int h = height();

    bool needh, needv;
    bool showh, showv;
    int hsbExt = horizontalScrollBar()->sizeHint().height();
    int vsbExt = verticalScrollBar()->sizeHint().width();

    if ( d->policy != AutoOne || d->anyVisibleChildren() ) {
        // Do we definitely need the scrollbar?
        needh = w-lmarg-rmarg < x;
        needv = h-tmarg-bmarg < y;

        // Do we intend to show the scrollbar?
        if (d->hMode == AlwaysOn)
            showh = true;
        else if (d->hMode == AlwaysOff)
            showh = false;
        else
            showh = needh;

        if (d->vMode == AlwaysOn)
            showv = true;
        else if (d->vMode == AlwaysOff)
            showv = false;
        else
            showv = needv;

        // Given other scrollbar will be shown, NOW do we need one?
        if ( showh && h-vsbExt-tmarg-bmarg < y ) {
            if (d->vMode == Auto)
                showv=true;
        }
        if ( showv && w-hsbExt-lmarg-rmarg < x ) {
            if (d->hMode == Auto)
                showh=true;
        }
    } else {
        // Scrollbars not needed, only show scrollbar that are always on.
        showh = d->hMode == AlwaysOn;
        showv = d->vMode == AlwaysOn;
    }

    return TQSize( w-lmarg-rmarg - (showv ? vsbExt : 0),
                  h-tmarg-bmarg - (showh ? hsbExt : 0) );
}


/*!
    Updates scroll bars: all possibilities are considered. You should
    never need to call this in your code.
*/
void TQScrollView::updateScrollBars()
{
    if(!horizontalScrollBar() && !verticalScrollBar())
	return;

    // I support this should use viewportSize()... but it needs
    // so many of the temporary variables from viewportSize.  hm.
    int fw = frameWidth();
    int lmarg = fw+d->l_marg;
    int rmarg = fw+d->r_marg;
    int tmarg = fw+d->t_marg;
    int bmarg = fw+d->b_marg;

    int w = width();
    int h = height();

    int portw, porth;

    bool needh;
    bool needv;
    bool showh;
    bool showv;
    bool showc = false;

    int hsbExt = horizontalScrollBar()->sizeHint().height();
    int vsbExt = verticalScrollBar()->sizeHint().width();

    TQSize oldVisibleSize( visibleWidth(), visibleHeight() );

    if ( d->policy != AutoOne || d->anyVisibleChildren() ) {
        // Do we definitely need the scrollbar?
        needh = w-lmarg-rmarg < d->contentsWidth();
        if ( d->inresize )
            needh  = !horizontalScrollBar()->isHidden();
        needv = h-tmarg-bmarg < contentsHeight();

        // Do we intend to show the scrollbar?
        if (d->hMode == AlwaysOn)
            showh = true;
        else if (d->hMode == AlwaysOff)
            showh = false;
        else
            showh = needh;

        if (d->vMode == AlwaysOn)
            showv = true;
        else if (d->vMode == AlwaysOff)
            showv = false;
        else
            showv = needv;

#ifdef TQ_WS_MAC
	bool mac_need_scroll = false;
	if(!parentWidget()) {
	    mac_need_scroll = true;
	} else {
	    TQWidget *tlw = topLevelWidget();
	    TQPoint tlw_br = TQPoint(tlw->width(), tlw->height()),
		    my_br = posInWindow(this) + TQPoint(w, h);
	    if(my_br.x() >= tlw_br.x() - 3 && my_br.y() >= tlw_br.y() - 3)
		mac_need_scroll = true;
	}
	if(mac_need_scroll) {
	    WindowAttributes attr;
	    GetWindowAttributes((WindowPtr)handle(), &attr);
	    mac_need_scroll = (attr & kWindowResizableAttribute);
	}
	if(mac_need_scroll) {
	    showc = true;
	    if(d->vMode == Auto)
		showv = true;
	    if(d->hMode == Auto)
		showh = true;
	}
#endif

        // Given other scrollbar will be shown, NOW do we need one?
        if ( showh && h-vsbExt-tmarg-bmarg < contentsHeight() ) {
            needv=true;
            if (d->vMode == Auto)
                showv=true;
        }
        if ( showv && !d->inresize && w-hsbExt-lmarg-rmarg < d->contentsWidth() ) {
            needh=true;
            if (d->hMode == Auto)
                showh=true;
        }
    } else {
        // Scrollbars not needed, only show scrollbar that are always on.
        needh = needv = false;
        showh = d->hMode == AlwaysOn;
        showv = d->vMode == AlwaysOn;
    }

    bool sc = d->signal_choke;
    d->signal_choke=true;

    // Hide unneeded scrollbar, calculate viewport size
    if ( showh ) {
        porth=h-hsbExt-tmarg-bmarg;
    } else {
        if (!needh)
            d->hbar->setValue(0);
        d->hbar->hide();
        porth=h-tmarg-bmarg;
    }
    if ( showv ) {
        portw=w-vsbExt-lmarg-rmarg;
    } else {
        if (!needv)
            d->vbar->setValue(0);
        d->vbar->hide();
        portw=w-lmarg-rmarg;
    }

    // Configure scrollbars that we will show
    if ( needv ) {
	d->vbar->setRange( 0, contentsHeight()-porth );
	d->vbar->setSteps( TQScrollView::d->vbar->lineStep(), porth );
    } else {
	d->vbar->setRange( 0, 0 );
    }
    if ( needh ) {
	d->hbar->setRange( 0, TQMAX(0, d->contentsWidth()-portw) );
	d->hbar->setSteps( TQScrollView::d->hbar->lineStep(), portw );
    } else {
	d->hbar->setRange( 0, 0 );
    }

    // Position the scrollbars, viewport and corner widget.
    int bottom;
    bool reverse = TQApplication::reverseLayout();
    int xoffset = ( reverse && (showv || cornerWidget() )) ? vsbExt : 0;
    int xpos = reverse ? 0 : w - vsbExt;
    bool frameContentsOnly =
	style().styleHint(TQStyle::SH_ScrollView_FrameOnlyAroundContents);

    if( ! frameContentsOnly ) {
	if ( reverse )
            xpos += fw;
        else
            xpos -= fw;
    }
    if ( showh ) {
        int right = ( showc || showv || cornerWidget() ) ? w-vsbExt : w;
        if ( ! frameContentsOnly )
            setHBarGeometry( *d->hbar, fw + xoffset, h-hsbExt-fw,
			     right-fw-fw, hsbExt );
        else
            setHBarGeometry( *d->hbar, 0 + xoffset, h-hsbExt, right,
			     hsbExt );
        bottom=h-hsbExt;
    } else {
        bottom=h;
    }
    if ( showv ) {
	clipper()->setGeometry( lmarg + xoffset, tmarg,
				w-vsbExt-lmarg-rmarg,
				bottom-tmarg-bmarg );
	d->viewportResized( w-vsbExt-lmarg-rmarg, bottom-tmarg-bmarg );
	if ( ! frameContentsOnly )
	    changeFrameRect(TQRect(0, 0, w, h) );
	else
	    changeFrameRect(TQRect(xoffset, 0, w-vsbExt, bottom));
	if (showc || cornerWidget()) {
	    if ( ! frameContentsOnly )
		setVBarGeometry( *d->vbar, xpos,
				 fw, vsbExt,
				 h-hsbExt-fw-fw );
	    else
		setVBarGeometry( *d->vbar, xpos, 0,
				 vsbExt,
				 h-hsbExt );
	}
	else {
	    if ( ! frameContentsOnly )
		setVBarGeometry( *d->vbar, xpos,
				 fw, vsbExt,
				 bottom-fw-fw );
	    else
		setVBarGeometry( *d->vbar, xpos, 0,
				 vsbExt, bottom );
	}
    } else {
        if ( ! frameContentsOnly )
            changeFrameRect(TQRect(0, 0, w, h));
        else
            changeFrameRect(TQRect(0, 0, w, bottom));
        clipper()->setGeometry( lmarg, tmarg,
				w-lmarg-rmarg, bottom-tmarg-bmarg );
        d->viewportResized( w-lmarg-rmarg, bottom-tmarg-bmarg );
    }

    TQWidget *corner = d->corner;
    if ( !d->corner )
	corner = d->defaultCorner;
    if ( ! frameContentsOnly )
	corner->setGeometry( xpos,
			     h-hsbExt-fw,
			     vsbExt,
			     hsbExt );
    else
	corner->setGeometry( xpos,
			     h-hsbExt,
			     vsbExt,
			     hsbExt );

    d->signal_choke=sc;

    if ( d->contentsX()+visibleWidth() > d->contentsWidth() ) {
        int x;
#if 0
        if ( reverse )
            x =TQMIN(0,d->contentsWidth()-visibleWidth());
        else
#endif
            x =TQMAX(0,d->contentsWidth()-visibleWidth());
        d->hbar->setValue(x);
        // Do it even if it is recursive
        moveContents( -x, -d->contentsY() );
    }
    if ( d->contentsY()+visibleHeight() > contentsHeight() ) {
        int y=TQMAX(0,contentsHeight()-visibleHeight());
        d->vbar->setValue(y);
        // Do it even if it is recursive
        moveContents( -d->contentsX(), -y );
    }

    // Finally, show the scroll bars
    if ( showh && ( d->hbar->isHidden() || !d->hbar->isVisible() ) )
        d->hbar->show();
    if ( showv && ( d->vbar->isHidden() || !d->vbar->isVisible() ) )
        d->vbar->show();

    d->signal_choke=true;
    d->vbar->setValue( d->contentsY() );
    d->hbar->setValue( d->contentsX() );
    d->signal_choke=false;

    TQSize newVisibleSize( visibleWidth(), visibleHeight() );
    if ( d->clipped_viewport && oldVisibleSize != newVisibleSize ) {
	TQResizeEvent e( newVisibleSize, oldVisibleSize );
	viewportResizeEvent( &e );
    }
}


/*!
    \reimp
*/
void TQScrollView::show()
{
    if ( isVisible() )
	return;
    TQWidget::show();
    updateScrollBars();
    d->hideOrShowAll(this);
}

/*!
    \reimp
 */
void TQScrollView::resize( int w, int h )
{
    TQWidget::resize( w, h );
}

/*!
    \reimp
*/
void TQScrollView::resize( const TQSize& s )
{
    resize( s.width(), s.height() );
}

/*!
    \reimp
*/
void TQScrollView::resizeEvent( TQResizeEvent* event )
{
    TQFrame::resizeEvent( event );

#if 0
    if ( TQApplication::reverseLayout() ) {
        d->fake_scroll = true;
        scrollBy( -event->size().width() + event->oldSize().width(), 0 );
        d->fake_scroll = false;
    }
#endif

    bool inresize = d->inresize;
    d->inresize = true;
    updateScrollBars();
    d->inresize = inresize;
    d->scrollbar_timer.start( 0, true );

    d->hideOrShowAll(this);
}



/*!
    \reimp
*/
void  TQScrollView::mousePressEvent( TQMouseEvent * e) //#### remove for 4.0
{
    e->ignore();
}

/*!
    \reimp
*/
void  TQScrollView::mouseReleaseEvent( TQMouseEvent *e ) //#### remove for 4.0
{
    e->ignore();
}


/*!
    \reimp
*/
void  TQScrollView::mouseDoubleClickEvent( TQMouseEvent *e ) //#### remove for 4.0
{
    e->ignore();
}

/*!
    \reimp
*/
void  TQScrollView::mouseMoveEvent( TQMouseEvent *e ) //#### remove for 4.0
{
    e->ignore();
}

/*!
    \reimp
*/
#ifndef TQT_NO_WHEELEVENT
void TQScrollView::wheelEvent( TQWheelEvent *e )
{
    TQWheelEvent ce( viewport()->mapFromGlobal( e->globalPos() ),
                    e->globalPos(), e->delta(), e->state(), e->orientation());
    viewportWheelEvent(&ce);
    if ( !ce.isAccepted() ) {
	if ( e->orientation() == Horizontal && horizontalScrollBar() && horizontalScrollBar()->isEnabled() )
	    TQApplication::sendEvent( horizontalScrollBar(), e);
	else  if (e->orientation() == Vertical && verticalScrollBar() && verticalScrollBar()->isEnabled() )
	    TQApplication::sendEvent( verticalScrollBar(), e);
    } else {
	e->accept();
    }
}
#endif

/*!
    \reimp
*/
void TQScrollView::contextMenuEvent( TQContextMenuEvent *e )
{
    if ( e->reason() != TQContextMenuEvent::Keyboard ) {
	e->ignore();
        return;
    }

    TQContextMenuEvent ce( e->reason(), viewport()->mapFromGlobal( e->globalPos() ),
                          e->globalPos(), e->state() );
    viewportContextMenuEvent( &ce );
    if ( ce.isAccepted() )
        e->accept();
    else
        e->ignore();
}

TQScrollView::ScrollBarMode TQScrollView::vScrollBarMode() const
{
    return d->vMode;
}


/*!
    \enum TQScrollView::ScrollBarMode

    This enum type describes the various modes of TQScrollView's scroll
    bars.

    \value Auto  TQScrollView shows a scroll bar when the content is
    too large to fit and not otherwise. This is the default.

    \value AlwaysOff  TQScrollView never shows a scroll bar.

    \value AlwaysOn  TQScrollView always shows a scroll bar.

    (The modes for the horizontal and vertical scroll bars are
    independent.)
*/


/*!
    \property TQScrollView::vScrollBarMode
    \brief the mode for the vertical scroll bar

    The default mode is \c TQScrollView::Auto.

    \sa hScrollBarMode
*/
void  TQScrollView::setVScrollBarMode( ScrollBarMode mode )
{
    if (d->vMode != mode) {
        d->vMode = mode;
        updateScrollBars();
    }
}


/*!
    \property TQScrollView::hScrollBarMode
    \brief the mode for the horizontal scroll bar

    The default mode is \c TQScrollView::Auto.

    \sa vScrollBarMode
*/
TQScrollView::ScrollBarMode TQScrollView::hScrollBarMode() const
{
    return d->hMode;
}

void TQScrollView::setHScrollBarMode( ScrollBarMode mode )
{
    if (d->hMode != mode) {
        d->hMode = mode;
        updateScrollBars();
    }
}


/*!
    Returns the widget in the corner between the two scroll bars.

    By default, no corner widget is present.
*/
TQWidget* TQScrollView::cornerWidget() const
{
    return d->corner;
}

/*!
    Sets the widget in the \a corner between the two scroll bars.

    You will probably also want to set at least one of the scroll bar
    modes to \c AlwaysOn.

    Passing 0 shows no widget in the corner.

    Any previous \a corner widget is hidden.

    You may call setCornerWidget() with the same widget at different
    times.

    All widgets set here will be deleted by the TQScrollView when it is
    destroyed unless you separately reparent the widget after setting
    some other corner widget (or 0).

    Any \e newly set widget should have no current parent.

    By default, no corner widget is present.

    \sa setVScrollBarMode(), setHScrollBarMode()
*/
void TQScrollView::setCornerWidget(TQWidget* corner)
{
    TQWidget* oldcorner = d->corner;
    if (oldcorner != corner) {
        if (oldcorner) oldcorner->hide();
        d->corner = corner;

        if ( corner && corner->parentWidget() != this ) {
            // #### No clean way to get current WFlags
            corner->reparent( this, (((TQScrollView*)corner))->getWFlags(),
                              TQPoint(0,0), false );
        }

        updateScrollBars();
        if ( corner ) corner->show();
    }
}


void TQScrollView::setResizePolicy( ResizePolicy r )
{
    d->policy = r;
}

/*!
    \property TQScrollView::resizePolicy
    \brief the resize policy

    The default is \c Default.

    \sa ResizePolicy
*/
TQScrollView::ResizePolicy TQScrollView::resizePolicy() const
{
    return d->policy;
}

/*!
    \reimp
*/
void TQScrollView::setEnabled( bool enable )
{
    TQFrame::setEnabled( enable );
}

/*!
    Removes the \a child widget from the scrolled area. Note that this
    happens automatically if the \a child is deleted.
*/
void TQScrollView::removeChild(TQWidget* child)
{
    if ( !d || !child ) // First check in case we are destructing
        return;

    TQSVChildRec *r = d->rec(child);
    if ( r ) d->deleteChildRec( r );
}

/*!
    \reimp
*/
void TQScrollView::removeChild(TQObject* child)
{
    TQFrame::removeChild(child);
}

/*!
    Inserts the widget, \a child, into the scrolled area positioned at
    (\a x, \a y). The position defaults to (0, 0). If the child is
    already in the view, it is just moved.

    You may want to call enableClipper(true) if you add a large number
    of widgets.
*/
void TQScrollView::addChild(TQWidget* child, int x, int y)
{
    if ( !child ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQScrollView::addChild(): Cannot add null child" );
#endif
	return;
    }
    child->polish();
    child->setBackgroundOrigin(WidgetOrigin);

    if ( child->parentWidget() == viewport() ) {
        // May already be there
        TQSVChildRec *r = d->rec(child);
        if (r) {
            r->moveTo(this,x,y,d->clipped_viewport);
            if ( d->policy > Manual ) {
                d->autoResizeHint(this);
                d->autoResize(this); // #### better to just deal with this one widget!
            }
            return;
        }
    }

    if ( d->children.isEmpty() && d->policy != Manual ) {
        if ( d->policy == Default )
            setResizePolicy( AutoOne );
        child->installEventFilter( this );
    } else if ( d->policy == AutoOne ) {
        child->removeEventFilter( this ); //#### ?????
        setResizePolicy( Manual );
    }
    if ( child->parentWidget() != viewport() ) {
            child->reparent( viewport(), 0, TQPoint(0,0), false );
    }
    d->addChildRec(child,x,y)->hideOrShow(this, d->clipped_viewport);

    if ( d->policy > Manual ) {
        d->autoResizeHint(this);
        d->autoResize(this); // #### better to just deal with this one widget!
    }
}

/*!
    Repositions the \a child widget to (\a x, \a y). This function is
    the same as addChild().
*/
void TQScrollView::moveChild(TQWidget* child, int x, int y)
{
    addChild(child,x,y);
}

/*!
    Returns the X position of the given \a child widget. Use this
    rather than TQWidget::x() for widgets added to the view.

    This function returns 0 if \a child has not been added to the view.
*/
int TQScrollView::childX(TQWidget* child)
{
    TQSVChildRec *r = d->rec(child);
    return r ? r->x : 0;
}

/*!
    Returns the Y position of the given \a child widget. Use this
    rather than TQWidget::y() for widgets added to the view.

    This function returns 0 if \a child has not been added to the view.
*/
int TQScrollView::childY(TQWidget* child)
{
    TQSVChildRec *r = d->rec(child);
    return r ? r->y : 0;
}

/*! \fn bool TQScrollView::childIsVisible(TQWidget*)
  \obsolete

  Returns true if \a child is visible. This is equivalent
  to child->isVisible().
*/

/*! \fn void TQScrollView::showChild(TQWidget* child, bool y)
  \obsolete

  Sets the visibility of \a child. Equivalent to
  TQWidget::show() or TQWidget::hide().
*/

/*!
    This event filter ensures the scroll bars are updated when a
    single contents widget is resized, shown, hidden or destroyed; it
    passes mouse events to the TQScrollView. The event is in \a e and
    the object is in \a obj.
*/

bool TQScrollView::eventFilter( TQObject *obj, TQEvent *e )
{
    if ( !d )
	return false; // we are destructing
    if ( obj == d->viewport || obj == d->clipped_viewport ) {
        switch ( e->type() ) {
            /* Forward many events to viewport...() functions */
        case TQEvent::Paint:
            viewportPaintEvent( (TQPaintEvent*)e );
            break;
        case TQEvent::Resize:
	    if ( !d->clipped_viewport )
		viewportResizeEvent( (TQResizeEvent *)e );
            break;
        case TQEvent::MouseButtonPress:
            viewportMousePressEvent( (TQMouseEvent*)e );
	    if ( ((TQMouseEvent*)e)->isAccepted() )
		return true;
            break;
        case TQEvent::MouseButtonRelease:
            viewportMouseReleaseEvent( (TQMouseEvent*)e );
	    if ( ((TQMouseEvent*)e)->isAccepted() )
		return true;
            break;
        case TQEvent::MouseButtonDblClick:
            viewportMouseDoubleClickEvent( (TQMouseEvent*)e );
	    if ( ((TQMouseEvent*)e)->isAccepted() )
		return true;
            break;
        case TQEvent::MouseMove:
            viewportMouseMoveEvent( (TQMouseEvent*)e );
	    if ( ((TQMouseEvent*)e)->isAccepted() )
		return true;
            break;
#ifndef TQT_NO_DRAGANDDROP
        case TQEvent::DragEnter:
            viewportDragEnterEvent( (TQDragEnterEvent*)e );
            break;
        case TQEvent::DragMove: {
            if ( d->drag_autoscroll ) {
                TQPoint vp = ((TQDragMoveEvent*) e)->pos();
                TQRect inside_margin( autoscroll_margin, autoscroll_margin,
                                     visibleWidth() - autoscroll_margin * 2,
                                     visibleHeight() - autoscroll_margin * 2 );
                if ( !inside_margin.contains( vp ) ) {
                    startDragAutoScroll();
                    // Keep sending move events
                    ( (TQDragMoveEvent*)e )->accept( TQRect(0,0,0,0) );
                }
            }
            viewportDragMoveEvent( (TQDragMoveEvent*)e );
        } break;
        case TQEvent::DragLeave:
            stopDragAutoScroll();
            viewportDragLeaveEvent( (TQDragLeaveEvent*)e );
            break;
        case TQEvent::Drop:
            stopDragAutoScroll();
            viewportDropEvent( (TQDropEvent*)e );
            break;
#endif // TQT_NO_DRAGANDDROP
        case TQEvent::ContextMenu:
            viewportContextMenuEvent( (TQContextMenuEvent*)e );
	    if ( ((TQContextMenuEvent*)e)->isAccepted() )
		return true;
            break;
        case TQEvent::ChildRemoved:
            removeChild((TQWidget*)((TQChildEvent*)e)->child());
            break;
        case TQEvent::LayoutHint:
            d->autoResizeHint(this);
            break;
        case TQEvent::WindowActivate:
        case TQEvent::WindowDeactivate:
            return true;
        default:
            break;
        }
    } else if ( d && d->rec((TQWidget*)obj) ) {  // must be a child
        if ( e->type() == TQEvent::Resize )
            d->autoResize(this);
        else if ( e->type() == TQEvent::Move )
            d->autoMove(this);
    }
    return TQFrame::eventFilter( obj, e );  // always continue with standard event processing
}

/*!
    This event handler is called whenever the TQScrollView receives a
    mousePressEvent(): the press position in \a e is translated to be a point
    on the contents.
*/
void TQScrollView::contentsMousePressEvent( TQMouseEvent* e )
{
    e->ignore();
}

/*!
    This event handler is called whenever the TQScrollView receives a
    mouseReleaseEvent(): the release position in \a e is translated to be a
    point on the contents.
*/
void TQScrollView::contentsMouseReleaseEvent( TQMouseEvent* e )
{
    e->ignore();
}

/*!
    This event handler is called whenever the TQScrollView receives a
    mouseDoubleClickEvent(): the click position in \a e is translated to be a
    point on the contents.

    The default implementation generates a normal mouse press event.
*/
void TQScrollView::contentsMouseDoubleClickEvent( TQMouseEvent* e )
{
    contentsMousePressEvent(e);		    // try mouse press event
}

/*!
    This event handler is called whenever the TQScrollView receives a
    mouseMoveEvent(): the mouse position in \a e is translated to be a point
    on the contents.
*/
void TQScrollView::contentsMouseMoveEvent( TQMouseEvent* e )
{
    e->ignore();
}

#ifndef TQT_NO_DRAGANDDROP

/*!
    This event handler is called whenever the TQScrollView receives a
    dragEnterEvent(): the drag position is translated to be a point
    on the contents.
*/
void TQScrollView::contentsDragEnterEvent( TQDragEnterEvent * )
{
}

/*!
    This event handler is called whenever the TQScrollView receives a
    dragMoveEvent(): the drag position is translated to be a point on
    the contents.
*/
void TQScrollView::contentsDragMoveEvent( TQDragMoveEvent * )
{
}

/*!
    This event handler is called whenever the TQScrollView receives a
    dragLeaveEvent(): the drag position is translated to be a point
    on the contents.
*/
void TQScrollView::contentsDragLeaveEvent( TQDragLeaveEvent * )
{
}

/*!
    This event handler is called whenever the TQScrollView receives a
    dropEvent(): the drop position is translated to be a point on the
    contents.
*/
void TQScrollView::contentsDropEvent( TQDropEvent * )
{
}

#endif // TQT_NO_DRAGANDDROP

/*!
    This event handler is called whenever the TQScrollView receives a
    wheelEvent() in \a{e}: the mouse position is translated to be a
    point on the contents.
*/
#ifndef TQT_NO_WHEELEVENT
void TQScrollView::contentsWheelEvent( TQWheelEvent * e )
{
    e->ignore();
}
#endif
/*!
    This event handler is called whenever the TQScrollView receives a
    contextMenuEvent() in \a{e}: the mouse position is translated to
    be a point on the contents.
*/
void TQScrollView::contentsContextMenuEvent( TQContextMenuEvent *e )
{
    e->ignore();
}

/*!
    This is a low-level painting routine that draws the viewport
    contents. Reimplement this if drawContents() is too high-level
    (for example, if you don't want to open a TQPainter on the
    viewport). The paint event is passed in \a pe.
*/
void TQScrollView::viewportPaintEvent( TQPaintEvent* pe )
{
    TQWidget* vp = viewport();

    TQPainter p(vp);
    TQRect r = pe->rect();

    if ( d->clipped_viewport ) {
	TQRect rr(
	    -d->clipped_viewport->x(), -d->clipped_viewport->y(),
	    d->viewport->width(), d->viewport->height()
	    );
	r &= rr;
	if ( r.isValid() ) {
	    int ex = r.x() + d->clipped_viewport->x() + d->contentsX();
	    int ey = r.y() + d->clipped_viewport->y() + d->contentsY();
	    int ew = r.width();
	    int eh = r.height();
	    drawContentsOffset(&p,
		d->contentsX()+d->clipped_viewport->x(),
		d->contentsY()+d->clipped_viewport->y(),
		ex, ey, ew, eh);
	}
    } else {
	r &= d->viewport->rect();
	int ex = r.x() + d->contentsX();
	int ey = r.y() + d->contentsY();
	int ew = r.width();
	int eh = r.height();
	drawContentsOffset(&p, d->contentsX(), d->contentsY(), ex, ey, ew, eh);
    }
}


/*!
    To provide simple processing of events on the contents, this
    function receives all resize events sent to the viewport.

    \sa TQWidget::resizeEvent()
*/
void TQScrollView::viewportResizeEvent( TQResizeEvent* )
{
}

/*! \internal

  To provide simple processing of events on the contents, this
  function receives all mouse press events sent to the viewport,
  translates the event and calls contentsMousePressEvent().

  \sa contentsMousePressEvent(), TQWidget::mousePressEvent()
*/
void TQScrollView::viewportMousePressEvent( TQMouseEvent* e )
{
    TQMouseEvent ce(e->type(), viewportToContents(e->pos()),
        e->globalPos(), e->button(), e->state());
    contentsMousePressEvent(&ce);
    if ( !ce.isAccepted() )
	e->ignore();
}

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all mouse release events sent to the viewport, translates
  the event and calls contentsMouseReleaseEvent().

  \sa TQWidget::mouseReleaseEvent()
*/
void TQScrollView::viewportMouseReleaseEvent( TQMouseEvent* e )
{
    TQMouseEvent ce(e->type(), viewportToContents(e->pos()),
        e->globalPos(), e->button(), e->state());
    contentsMouseReleaseEvent(&ce);
    if ( !ce.isAccepted() )
	e->ignore();
}

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all mouse double click events sent to the viewport,
  translates the event and calls contentsMouseDoubleClickEvent().

  \sa TQWidget::mouseDoubleClickEvent()
*/
void TQScrollView::viewportMouseDoubleClickEvent( TQMouseEvent* e )
{
    TQMouseEvent ce(e->type(), viewportToContents(e->pos()),
        e->globalPos(), e->button(), e->state());
    contentsMouseDoubleClickEvent(&ce);
    if ( !ce.isAccepted() )
	e->ignore();
}

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all mouse move events sent to the viewport, translates the
  event and calls contentsMouseMoveEvent().

  \sa TQWidget::mouseMoveEvent()
*/
void TQScrollView::viewportMouseMoveEvent( TQMouseEvent* e )
{
    TQMouseEvent ce(e->type(), viewportToContents(e->pos()),
        e->globalPos(), e->button(), e->state());
    contentsMouseMoveEvent(&ce);
    if ( !ce.isAccepted() )
	e->ignore();
}

#ifndef TQT_NO_DRAGANDDROP

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all drag enter events sent to the viewport, translates the
  event and calls contentsDragEnterEvent().

  \sa TQWidget::dragEnterEvent()
*/
void TQScrollView::viewportDragEnterEvent( TQDragEnterEvent* e )
{
    e->setPoint(viewportToContents(e->pos()));
    contentsDragEnterEvent(e);
    e->setPoint(contentsToViewport(e->pos()));
}

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all drag move events sent to the viewport, translates the
  event and calls contentsDragMoveEvent().

  \sa TQWidget::dragMoveEvent()
*/
void TQScrollView::viewportDragMoveEvent( TQDragMoveEvent* e )
{
    e->setPoint(viewportToContents(e->pos()));
    contentsDragMoveEvent(e);
    e->setPoint(contentsToViewport(e->pos()));
}

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all drag leave events sent to the viewport and calls
  contentsDragLeaveEvent().

  \sa TQWidget::dragLeaveEvent()
*/
void TQScrollView::viewportDragLeaveEvent( TQDragLeaveEvent* e )
{
    contentsDragLeaveEvent(e);
}

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all drop events sent to the viewport, translates the event
  and calls contentsDropEvent().

  \sa TQWidget::dropEvent()
*/
void TQScrollView::viewportDropEvent( TQDropEvent* e )
{
    e->setPoint(viewportToContents(e->pos()));
    contentsDropEvent(e);
    e->setPoint(contentsToViewport(e->pos()));
}

#endif // TQT_NO_DRAGANDDROP

/*!\internal

  To provide simple processing of events on the contents, this function
  receives all wheel events sent to the viewport, translates the
  event and calls contentsWheelEvent().

  \sa TQWidget::wheelEvent()
*/
#ifndef TQT_NO_WHEELEVENT
void TQScrollView::viewportWheelEvent( TQWheelEvent* e )
{
    /*
       Different than standard mouse events, because wheel events might
       be sent to the focus widget if the widget-under-mouse doesn't want
       the event itself.
    */
    TQWheelEvent ce( viewportToContents(e->pos()),
        e->globalPos(), e->delta(), e->state(), e->orientation());
    contentsWheelEvent(&ce);
    if ( ce.isAccepted() )
        e->accept();
    else
        e->ignore();
}
#endif

/*! \internal

  To provide simple processing of events on the contents, this function
  receives all context menu events sent to the viewport, translates the
  event and calls contentsContextMenuEvent().
*/
void TQScrollView::viewportContextMenuEvent( TQContextMenuEvent *e )
{
    TQContextMenuEvent ce(e->reason(), viewportToContents(e->pos()), e->globalPos(), e->state() );
    contentsContextMenuEvent( &ce );
    if ( ce.isAccepted() )
        e->accept();
    else
        e->ignore();
}

/*!
    Returns the component horizontal scroll bar. It is made available
    to allow accelerators, autoscrolling, etc.

    It should not be used for other purposes.

    This function never returns 0.
*/
TQScrollBar* TQScrollView::horizontalScrollBar() const
{
    return d->hbar;
}

/*!
    Returns the component vertical scroll bar. It is made available to
    allow accelerators, autoscrolling, etc.

    It should not be used for other purposes.

    This function never returns 0.
*/
TQScrollBar* TQScrollView::verticalScrollBar() const {
    return d->vbar;
}


/*!
    Scrolls the content so that the point \a (x, y) is visible with at
    least 50-pixel margins (if possible, otherwise centered).
*/
void TQScrollView::ensureVisible( int x, int y )
{
    ensureVisible(x, y, 50, 50);
}

/*!
    \overload

    Scrolls the content so that the point \a (x, y) is visible with at
    least the \a xmargin and \a ymargin margins (if possible,
    otherwise centered).
*/
void TQScrollView::ensureVisible( int x, int y, int xmargin, int ymargin )
{
    int pw=visibleWidth();
    int ph=visibleHeight();

    int cx=-d->contentsX();
    int cy=-d->contentsY();
    int cw=d->contentsWidth();
    int ch=contentsHeight();

    if ( pw < xmargin*2 )
        xmargin=pw/2;
    if ( ph < ymargin*2 )
        ymargin=ph/2;

    if ( cw <= pw ) {
        xmargin=0;
        cx=0;
    }
    if ( ch <= ph ) {
        ymargin=0;
        cy=0;
    }

    if ( x < -cx+xmargin )
        cx = -x+xmargin;
    else if ( x >= -cx+pw-xmargin )
        cx = -x+pw-xmargin;

    if ( y < -cy+ymargin )
        cy = -y+ymargin;
    else if ( y >= -cy+ph-ymargin )
        cy = -y+ph-ymargin;

    if ( cx > 0 )
        cx=0;
    else if ( cx < pw-cw && cw>pw )
        cx=pw-cw;

    if ( cy > 0 )
        cy=0;
    else if ( cy < ph-ch && ch>ph )
        cy=ph-ch;

    setContentsPos( -cx, -cy );
}

/*!
    Scrolls the content so that the point \a (x, y) is in the top-left
    corner.
*/
void TQScrollView::setContentsPos( int x, int y )
{
#if 0
    // bounds checking...
    if ( TQApplication::reverseLayout() )
        if ( x > d->contentsWidth() - visibleWidth() ) x = d->contentsWidth() - visibleWidth();
    else
#endif
        if ( x < 0 ) x = 0;
    if ( y < 0 ) y = 0;
    // Choke signal handling while we update BOTH sliders.
    d->signal_choke=true;
    moveContents( -x, -y );
    d->vbar->setValue( y );
    d->hbar->setValue( x );
    d->signal_choke=false;
}

/*!
    Scrolls the content by \a dx to the left and \a dy upwards.
*/
void TQScrollView::scrollBy( int dx, int dy )
{
    setContentsPos( TQMAX( d->contentsX()+dx, 0 ), TQMAX( d->contentsY()+dy, 0 ) );
}

/*!
    Scrolls the content so that the point \a (x, y) is in the center
    of visible area.
*/
void TQScrollView::center( int x, int y )
{
    ensureVisible( x, y, 32000, 32000 );
}

/*!
    \overload

    Scrolls the content so that the point \a (x, y) is visible with
    the \a xmargin and \a ymargin margins (as fractions of visible
    the area).

    For example:
    \list
    \i Margin 0.0 allows (x, y) to be on the edge of the visible area.
    \i Margin 0.5 ensures that (x, y) is in middle 50% of the visible area.
    \i Margin 1.0 ensures that (x, y) is in the center of the the visible area.
    \endlist
*/
void TQScrollView::center( int x, int y, float xmargin, float ymargin )
{
    int pw=visibleWidth();
    int ph=visibleHeight();
    ensureVisible( x, y, int( xmargin/2.0*pw+0.5 ), int( ymargin/2.0*ph+0.5 ) );
}


/*!
    \fn void TQScrollView::contentsMoving(int x, int y)

    This signal is emitted just before the contents are moved to
    position \a (x, y).

    \sa contentsX(), contentsY()
*/

/*!
    Moves the contents by \a (x, y).
*/
void TQScrollView::moveContents(int x, int y)
{
    if ( -x+visibleWidth() > d->contentsWidth() )
#if 0
        if( TQApplication::reverseLayout() )
            x=TQMAX(0,-d->contentsWidth()+visibleWidth());
        else
#endif
            x=TQMIN(0,-d->contentsWidth()+visibleWidth());
    if ( -y+visibleHeight() > contentsHeight() )
        y=TQMIN(0,-contentsHeight()+visibleHeight());

    int dx = x - d->vx;
    int dy = y - d->vy;

    if (!dx && !dy)
        return; // Nothing to do

    emit contentsMoving( -x, -y );

    d->vx = x;
    d->vy = y;

    if ( d->clipped_viewport || d->static_bg ) {
        // Cheap move (usually)
        d->moveAllBy(dx,dy);
    } else if ( /*dx && dy ||*/
         ( TQABS(dy) * 5 > visibleHeight() * 4 ) ||
         ( TQABS(dx) * 5 > visibleWidth() * 4 )
        )
    {
        // Big move
        if ( viewport()->isUpdatesEnabled() )
            viewport()->update();
        d->moveAllBy(dx,dy);
    } else if ( !d->fake_scroll || d->contentsWidth() > visibleWidth() ) {
        // Small move
        clipper()->scroll(dx,dy);
    }
    d->hideOrShowAll(this, true );
}

#if (TQT_VERSION-0 >= 0x040000)
#if defined(Q_CC_GNU)
#warning "Should rename contents{X,Y,Width,Height} to viewport{...}"
#endif
// Because it's the viewport rectangle that is "moving", not the contents.
#endif

/*!
    \property TQScrollView::contentsX
    \brief the X coordinate of the contents that are at the left edge of
    the viewport.
*/
int TQScrollView::contentsX() const
{
    return d->contentsX();
}

/*!
    \property TQScrollView::contentsY
    \brief the Y coordinate of the contents that are at the top edge of
    the viewport.
*/
int TQScrollView::contentsY() const
{
    return d->contentsY();
}

/*!
    \property TQScrollView::contentsWidth
    \brief the width of the contents area
*/
int TQScrollView::contentsWidth() const
{
    return d->contentsWidth();
}

/*!
    \property TQScrollView::contentsHeight
    \brief the height of the contents area
*/
int TQScrollView::contentsHeight() const
{
    return d->vheight;
}

/*!
    Sets the size of the contents area to \a w pixels wide and \a h
    pixels high and updates the viewport accordingly.
*/
void TQScrollView::resizeContents( int w, int h )
{
    int ow = d->vwidth;
    int oh = d->vheight;
    d->vwidth = w;
    d->vheight = h;

    d->scrollbar_timer.start( 0, true );

    if ( d->children.isEmpty() && d->policy == Default )
        setResizePolicy( Manual );

    if ( ow > w ) {
        // Swap
        int t=w;
        w=ow;
        ow=t;
    }
    // Refresh area ow..w
    if ( ow < visibleWidth() && w >= 0 ) {
        if ( ow < 0 )
            ow = 0;
        if ( w > visibleWidth() )
            w = visibleWidth();
        clipper()->update( d->contentsX()+ow, 0, w-ow, visibleHeight() );
    }

    if ( oh > h ) {
        // Swap
        int t=h;
        h=oh;
        oh=t;
    }
    // Refresh area oh..h
    if ( oh < visibleHeight() && h >= 0 ) {
        if ( oh < 0 )
            oh = 0;
        if ( h > visibleHeight() )
            h = visibleHeight();
        clipper()->update( 0, d->contentsY()+oh, visibleWidth(), h-oh);
    }
}

/*!
    Calls update() on a rectangle defined by \a x, \a y, \a w, \a h,
    translated appropriately. If the rectangle is not visible, nothing
    is repainted.

    \sa repaintContents()
*/
void TQScrollView::updateContents( int x, int y, int w, int h )
{
    if ( testWState(WState_Visible|WState_BlockUpdates) != WState_Visible )
	return;

    TQWidget* vp = viewport();

    // Translate
    x -= d->contentsX();
    y -= d->contentsY();

    // Clip to TQCOORD space
    if ( x < 0 ) {
        w += x;
        x = 0;
    }
    if ( y < 0 ) {
        h += y;
        y = 0;
    }

    if ( w < 0 || h < 0 )
        return;
    if ( x > visibleWidth() || y > visibleHeight() )
	return;

    if ( w > visibleWidth() )
        w = visibleWidth();
    if ( h > visibleHeight() )
        h = visibleHeight();

    if ( d->clipped_viewport ) {
        // Translate clipper() to viewport()
        x -= d->clipped_viewport->x();
        y -= d->clipped_viewport->y();
    }

    vp->update( x, y, w, h );
}

/*!
    \overload

    Updates the contents in rectangle \a r
*/
void TQScrollView::updateContents( const TQRect& r )
{
    updateContents(r.x(), r.y(), r.width(), r.height());
}

/*!
    \overload
*/
void TQScrollView::updateContents()
{
    updateContents( d->contentsX(), d->contentsY(), visibleWidth(), visibleHeight() );
}

/*!
    \overload

    Repaints the contents of rectangle \a r. If \a erase is true the
    background is cleared using the background color.
*/
void TQScrollView::repaintContents( const TQRect& r, bool erase )
{
    repaintContents(r.x(), r.y(), r.width(), r.height(), erase);
}


/*!
    \overload

    Repaints the contents. If \a erase is true the background is
    cleared using the background color.
*/
void TQScrollView::repaintContents( bool erase )
{
    repaintContents( d->contentsX(), d->contentsY(), visibleWidth(), visibleHeight(), erase );
}


/*!
    Calls repaint() on a rectangle defined by \a x, \a y, \a w, \a h,
    translated appropriately. If the rectangle is not visible, nothing
    is repainted. If \a erase is true the background is cleared using
    the background color.

    \sa updateContents()
*/
void TQScrollView::repaintContents( int x, int y, int w, int h, bool erase )
{
    if ( testWState(WState_Visible|WState_BlockUpdates) != WState_Visible )
	return;

    TQWidget* vp = viewport();

    // Translate logical to clipper()
    x -= d->contentsX();
    y -= d->contentsY();

    // Clip to TQCOORD space
    if ( x < 0 ) {
        w += x;
        x = 0;
    }
    if ( y < 0 ) {
        h += y;
        y = 0;
    }

    if ( w < 0 || h < 0 )
        return;
    if ( w > visibleWidth() )
        w = visibleWidth();
    if ( h > visibleHeight() )
        h = visibleHeight();

    if ( d->clipped_viewport ) {
        // Translate clipper() to viewport()
        x -= d->clipped_viewport->x();
        y -= d->clipped_viewport->y();
    }

    vp->repaint( x, y, w, h, erase );
}


/*!
    For backward-compatibility only. It is easier to use
    drawContents(TQPainter*,int,int,int,int).

    The default implementation translates the painter appropriately
    and calls drawContents(TQPainter*,int,int,int,int). See
    drawContents() for an explanation of the parameters \a p, \a
    offsetx, \a offsety, \a clipx, \a clipy, \a clipw and \a cliph.
*/
void TQScrollView::drawContentsOffset(TQPainter* p, int offsetx, int offsety, int clipx, int clipy, int clipw, int cliph)
{
    p->translate(-offsetx,-offsety);
    drawContents(p, clipx, clipy, clipw, cliph);
}

/*!
    \fn void TQScrollView::drawContents(TQPainter* p, int clipx, int clipy, int clipw, int cliph)

    Reimplement this function if you are viewing a drawing area rather
    than a widget.

    The function should draw the rectangle (\a clipx, \a clipy, \a
    clipw, \a cliph) of the contents using painter \a p. The clip
    rectangle is in the scrollview's coordinates.

    For example:
    \code
    {
	// Fill a 40000 by 50000 rectangle at (100000,150000)

	// Calculate the coordinates...
	int x1 = 100000, y1 = 150000;
	int x2 = x1+40000-1, y2 = y1+50000-1;

	// Clip the coordinates so X/Windows will not have problems...
	if (x1 < clipx) x1=clipx;
	if (y1 < clipy) y1=clipy;
	if (x2 > clipx+clipw-1) x2=clipx+clipw-1;
	if (y2 > clipy+cliph-1) y2=clipy+cliph-1;

	// Paint using the small coordinates...
	if ( x2 >= x1 && y2 >= y1 )
	    p->fillRect(x1, y1, x2-x1+1, y2-y1+1, red);
    }
    \endcode

    The clip rectangle and translation of the painter \a p is already
    set appropriately.
*/
void TQScrollView::drawContents(TQPainter*, int, int, int, int)
{
}


/*!
    \reimp
*/
void TQScrollView::frameChanged()
{
    // slight ugle-hack - the listview header needs readjusting when
    // changing the frame
    if (TQListView *lv = ::tqt_cast<TQListView *>(this))
        lv->triggerUpdate();
    TQFrame::frameChanged();
    updateScrollBars();
}


/*!
    Returns the viewport widget of the scrollview. This is the widget
    containing the contents widget or which is the drawing area.
*/
TQWidget* TQScrollView::viewport() const
{
    if ( d->clipped_viewport )
	return  d->clipped_viewport;
    return d->viewport;
}

/*!
    Returns the clipper widget. Contents in the scrollview are
    ultimately clipped to be inside the clipper widget.

    You should not need to use this function.

    \sa visibleWidth(), visibleHeight()
*/
TQWidget* TQScrollView::clipper() const
{
    return d->viewport;
}

/*!
    \property TQScrollView::visibleWidth
    \brief the horizontal amount of the content that is visible
*/
int TQScrollView::visibleWidth() const
{
    return clipper()->width();
}

/*!
    \property TQScrollView::visibleHeight
    \brief the vertical amount of the content that is visible
*/
int TQScrollView::visibleHeight() const
{
    return clipper()->height();
}


void TQScrollView::changeFrameRect(const TQRect& r)
{
    TQRect oldr = frameRect();
    if (oldr != r) {
        TQRect cr = contentsRect();
        TQRegion fr( frameRect() );
        fr = fr.subtract( contentsRect() );
        setFrameRect( r );
        if ( isVisible() ) {
            cr = cr.intersect( contentsRect() );
            fr = fr.unite( frameRect() );
            fr = fr.subtract( cr );
            if ( !fr.isEmpty() )
                TQApplication::postEvent( this, new TQPaintEvent( fr, false ) );
        }
    }
}


/*!
    Sets the margins around the scrolling area to \a left, \a top, \a
    right and \a bottom. This is useful for applications such as
    spreadsheets with "locked" rows and columns. The marginal space is
    \e inside the frameRect() and is left blank; reimplement
    drawFrame() or put widgets in the unused area.

    By default all margins are zero.

    \sa frameChanged()
*/
void TQScrollView::setMargins(int left, int top, int right, int bottom)
{
    if ( left == d->l_marg &&
         top == d->t_marg &&
         right == d->r_marg &&
         bottom == d->b_marg )
        return;

    d->l_marg = left;
    d->t_marg = top;
    d->r_marg = right;
    d->b_marg = bottom;
    updateScrollBars();
}


/*!
    Returns the left margin.

    \sa setMargins()
*/
int TQScrollView::leftMargin() const
{
    return d->l_marg;
}


/*!
    Returns the top margin.

    \sa setMargins()
*/
int TQScrollView::topMargin() const
{
    return d->t_marg;
}


/*!
    Returns the right margin.

    \sa setMargins()
*/
int TQScrollView::rightMargin() const
{
    return d->r_marg;
}


/*!
    Returns the bottom margin.

    \sa setMargins()
*/
int TQScrollView::bottomMargin() const
{
    return d->b_marg;
}

/*!
    \reimp
*/
bool TQScrollView::focusNextPrevChild( bool next )
{
    //  Makes sure that the new focus widget is on-screen, if
    //  necessary by scrolling the scroll view.

    // first set things up for the scan
    TQFocusData *f = focusData();
    TQWidget *startingPoint = f->home();
    TQWidget *candidate = 0;
    TQWidget *w = next ? f->next() : f->prev();
    TQSVChildRec *r;
    extern bool tqt_tab_all_widgets;
    uint focus_flag = tqt_tab_all_widgets ? TabFocus : StrongFocus;

    // then scan for a possible focus widget candidate
    while( !candidate && w != startingPoint ) {
        if ( w != startingPoint &&
             (w->focusPolicy() & focus_flag) == focus_flag
             && w->isEnabled() &&!w->focusProxy() && w->isVisible() )
            candidate = w;
        w = next ? f->next() : f->prev();
    }

    // if we could not find one, maybe super or parentWidget() can?
    if ( !candidate )
        return TQFrame::focusNextPrevChild( next );

    // we've found one.
    r = d->ancestorRec( candidate );
    if ( r && ( r->child == candidate ||
                candidate->isVisibleTo( r->child ) ) ) {
        TQPoint cp = r->child->mapToGlobal(TQPoint(0,0));
        TQPoint cr = candidate->mapToGlobal(TQPoint(0,0)) - cp;
        ensureVisible( r->x+cr.x()+candidate->width()/2,
                       r->y+cr.y()+candidate->height()/2,
                       candidate->width()/2,
                       candidate->height()/2 );
    }

    candidate->setFocus();
    return true;
}



/*!
    When a large numbers of child widgets are in a scrollview,
    especially if they are close together, the scrolling performance
    can suffer greatly. If \a y is true the scrollview will use an
    extra widget to group child widgets.

    Note that you may only call enableClipper() prior to adding
    widgets.

    For a full discussion, see this class's \link #enableclipper
    detailed description\endlink.
*/
void TQScrollView::enableClipper(bool y)
{
    if ( !d->clipped_viewport == !y )
	return;
    if ( d->children.count() )
	tqFatal("May only call TQScrollView::enableClipper() before adding widgets");
    if ( y ) {
	d->clipped_viewport = new TQClipperWidget(clipper(), "qt_clipped_viewport", d->flags);
	d->clipped_viewport->setGeometry(-coord_limit/2,-coord_limit/2,
					 coord_limit,coord_limit);
	d->clipped_viewport->setBackgroundMode( d->viewport->backgroundMode() );
	d->viewport->setBackgroundMode(NoBackground); // no exposures for this
	d->viewport->removeEventFilter( this );
	d->clipped_viewport->installEventFilter( this );
	d->clipped_viewport->show();
    } else {
	delete d->clipped_viewport;
	d->clipped_viewport = 0;
    }
}

/*!
    Sets the scrollview to have a static background if \a y is true,
    or a scrolling background if \a y is false. By default, the
    background is scrolling.

    Be aware that this mode is quite slow, as a full repaint of the
    visible area has to be triggered on every contents move.

    \sa hasStaticBackground()
*/
void  TQScrollView::setStaticBackground(bool y)
{
    d->static_bg = y;
}

/*!
    Returns true if TQScrollView uses a static background; otherwise
    returns false.

    \sa setStaticBackground()
*/
bool TQScrollView::hasStaticBackground() const
{
    return d->static_bg;
}

/*!
    \overload

    Returns the point \a p translated to a point on the viewport()
    widget.
*/
TQPoint TQScrollView::contentsToViewport( const TQPoint& p ) const
{
    if ( d->clipped_viewport ) {
        return TQPoint( p.x() - d->contentsX() - d->clipped_viewport->x(),
                       p.y() - d->contentsY() - d->clipped_viewport->y() );
    } else {
        return TQPoint( p.x() - d->contentsX(),
                       p.y() - d->contentsY() );
    }
}

/*!
    \overload

    Returns the point on the viewport \a vp translated to a point in
    the contents.
*/
TQPoint TQScrollView::viewportToContents( const TQPoint& vp ) const
{
    if ( d->clipped_viewport ) {
        return TQPoint( vp.x() + d->contentsX() + d->clipped_viewport->x(),
                       vp.y() + d->contentsY() + d->clipped_viewport->y() );
    } else {
        return TQPoint( vp.x() + d->contentsX(),
                       vp.y() + d->contentsY() );
    }
}


/*!
    Translates a point (\a x, \a y) in the contents to a point (\a vx,
    \a vy) on the viewport() widget.
*/
void TQScrollView::contentsToViewport( int x, int y, int& vx, int& vy ) const
{
    const TQPoint v = contentsToViewport(TQPoint(x,y));
    vx = v.x();
    vy = v.y();
}

/*!
    Translates a point (\a vx, \a vy) on the viewport() widget to a
    point (\a x, \a y) in the contents.
*/
void TQScrollView::viewportToContents( int vx, int vy, int& x, int& y ) const
{
    const TQPoint c = viewportToContents(TQPoint(vx,vy));
    x = c.x();
    y = c.y();
}

/*!
    \reimp
*/
TQSize TQScrollView::sizeHint() const
{
    if ( d->use_cached_size_hint && d->cachedSizeHint.isValid() )
	return d->cachedSizeHint;

    constPolish();
    int f = 2 * frameWidth();
    int h = fontMetrics().height();
    TQSize sz( f, f );
    if ( d->policy > Manual ) {
	TQSVChildRec *r = d->children.first();
	if ( r ) {
	    TQSize cs = r->child->sizeHint();
	    if ( cs.isValid() )
		sz += cs.boundedTo( r->child->maximumSize() );
	    else
		sz += r->child->size();
        }
    } else {
	sz += TQSize( d->contentsWidth(), contentsHeight() );
    }
    if (d->vMode == AlwaysOn)
	sz.setWidth(sz.width() + d->vbar->sizeHint().width());
    if (d->hMode == AlwaysOn)
	sz.setHeight(sz.height() + d->hbar->sizeHint().height());
    return sz.expandedTo( TQSize(12 * h, 8 * h) )
	     .boundedTo( TQSize(36 * h, 24 * h) );
}


/*!
    \reimp
*/
TQSize TQScrollView::minimumSizeHint() const
{
    int h = fontMetrics().height();
    if ( h < 10 )
	h = 10;
    int f = 2 * frameWidth();
    return TQSize( (6 * h) + f, (4 * h) + f );
}


/*!
    \reimp

    (Implemented to get rid of a compiler warning.)
*/
void TQScrollView::drawContents( TQPainter * )
{
}

#ifndef TQT_NO_DRAGANDDROP

/*!
  \internal
*/
void TQScrollView::startDragAutoScroll()
{
    if ( !d->autoscroll_timer.isActive() ) {
        d->autoscroll_time = initialScrollTime;
        d->autoscroll_accel = initialScrollAccel;
        d->autoscroll_timer.start( d->autoscroll_time );
    }
}


/*!
  \internal
*/
void TQScrollView::stopDragAutoScroll()
{
    d->autoscroll_timer.stop();
}


/*!
  \internal
*/
void TQScrollView::doDragAutoScroll()
{
    TQPoint p = d->viewport->mapFromGlobal( TQCursor::pos() );

    if ( d->autoscroll_accel-- <= 0 && d->autoscroll_time ) {
        d->autoscroll_accel = initialScrollAccel;
        d->autoscroll_time--;
        d->autoscroll_timer.start( d->autoscroll_time );
    }
    int l = TQMAX( 1, ( initialScrollTime- d->autoscroll_time ) );

    int dx = 0, dy = 0;
    if ( p.y() < autoscroll_margin ) {
        dy = -l;
    } else if ( p.y() > visibleHeight() - autoscroll_margin ) {
        dy = +l;
    }
    if ( p.x() < autoscroll_margin ) {
        dx = -l;
    } else if ( p.x() > visibleWidth() - autoscroll_margin ) {
        dx = +l;
    }
    if ( dx || dy ) {
        scrollBy(dx,dy);
    } else {
        stopDragAutoScroll();
    }
}


/*!
    \property TQScrollView::dragAutoScroll
    \brief whether autoscrolling in drag move events is enabled

    If this property is set to true (the default), the TQScrollView
    automatically scrolls the contents in drag move events if the user
    moves the cursor close to a border of the view. Of course this
    works only if the viewport accepts drops. Specifying false
    disables this autoscroll feature.

    \warning Enabling this property might not be enough to
    effectively turn on autoscrolling. If you put a custom widget in
    the TQScrollView, you might need to call TQDragEvent::ignore() on
    the event in the dragEnterEvent() and dragMoveEvent()
    reimplementations.
*/

void TQScrollView::setDragAutoScroll( bool b )
{
    d->drag_autoscroll = b;
}

bool TQScrollView::dragAutoScroll() const
{
    return d->drag_autoscroll;
}

#endif // TQT_NO_DRAGANDDROP

/*!\internal
 */
void TQScrollView::setCachedSizeHint( const TQSize &sh ) const
{
    if ( isVisible() && !d->cachedSizeHint.isValid() )
	d->cachedSizeHint = sh;
}

/*!\internal
 */
void TQScrollView::disableSizeHintCaching()
{
    d->use_cached_size_hint = false;
}

/*!\internal
 */
TQSize TQScrollView::cachedSizeHint() const
{
    return d->use_cached_size_hint ? d->cachedSizeHint : TQSize();
}

#endif // TQT_NO_SCROLLVIEW
