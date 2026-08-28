/****************************************************************************
**
** Implementation of TQPainter, TQPen and TQBrush classes
**
** Created : 940112
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

#include "ntqpainter.h"
#include "qpainter_p.h"
#include "ntqbitmap.h"
#include "ntqptrstack.h"
#include "ntqptrdict.h"
#include "ntqdatastream.h"
#include "ntqwidget.h"
#include "ntqimage.h"
#include "ntqpaintdevicemetrics.h"
#include "ntqapplication.h"
#include "qrichtext_p.h"
#include "ntqregexp.h"
#include "ntqcleanuphandler.h"
#ifdef TQ_WS_QWS
#include "qgfx_qws.h"
#endif
#include <string.h>

#include "qtextlayout_p.h"
#include "qfontengine_p.h"

#ifndef TQT_NO_TRANSFORMATIONS
typedef TQPtrStack<TQWMatrix> TQWMatrixStack;
#endif

// POSIX Large File Support redefines truncate -> truncate64
#if defined(truncate)
# undef truncate
#endif

/*!
    \class TQPainter ntqpainter.h
    \brief The TQPainter class does low-level painting e.g. on widgets.

    \ingroup graphics
    \ingroup images
    \mainclass

    The painter provides highly optimized functions to do most of the
    drawing GUI programs require. TQPainter can draw everything from
    simple lines to complex shapes like pies and chords. It can also
    draw aligned text and pixmaps. Normally, it draws in a "natural"
    coordinate system, but it can also do view and world
    transformation.

    The typical use of a painter is:

    \list
    \i Construct a painter.
    \i Set a pen, a brush etc.
    \i Draw.
    \i Destroy the painter.
    \endlist

    Mostly, all this is done inside a paint event. (In fact, 99% of
    all TQPainter use is in a reimplementation of
    TQWidget::paintEvent(), and the painter is heavily optimized for
    such use.) Here's one very simple example:

    \code
    void SimpleExampleWidget::paintEvent()
    {
	TQPainter paint( this );
	paint.setPen( TQt::blue );
	paint.drawText( rect(), AlignCenter, "The Text" );
    }
    \endcode

    Usage is simple, and there are many settings you can use:

    \list

    \i font() is the currently set font. If you set a font that isn't
    available, TQt finds a close match. In fact font() returns what
    you set using setFont() and fontInfo() returns the font actually
    being used (which may be the same).

    \i brush() is the currently set brush; the color or pattern that's
    used for filling e.g. circles.

    \i pen() is the currently set pen; the color or stipple that's
    used for drawing lines or boundaries.

    \i backgroundMode() is \c Opaque or \c Transparent, i.e. whether
    backgroundColor() is used or not.

    \i backgroundColor() only applies when backgroundMode() is Opaque
    and pen() is a stipple. In that case, it describes the color of
    the background pixels in the stipple.

    \i rasterOp() is how pixels drawn interact with the pixels already
    there.

    \i brushOrigin() is the origin of the tiled brushes, normally the
    origin of the window.

    \i viewport(), window(), worldMatrix() and many more make up the
    painter's coordinate transformation system. See \link
    coordsys.html The Coordinate System \endlink for an explanation of
    this, or see below for a very brief overview of the functions.

    \i hasClipping() is whether the painter clips at all. (The paint
    device clips, too.) If the painter clips, it clips to clipRegion().

    \i pos() is the current position, set by moveTo() and used by
    lineTo().

    \endlist

    Note that some of these settings mirror settings in some paint
    devices, e.g. TQWidget::font(). TQPainter::begin() (or the TQPainter
    constructor) copies these attributes from the paint device.
    Calling, for example, TQWidget::setFont() doesn't take effect until
    the next time a painter begins painting on it.

    save() saves all of these settings on an internal stack, restore()
    pops them back.

    The core functionality of TQPainter is drawing, and there are
    functions to draw most primitives: drawPoint(), drawPoints(),
    drawLine(), drawRect(), drawWinFocusRect(), drawRoundRect(),
    drawEllipse(), drawArc(), drawPie(), drawChord(),
    drawLineSegments(), drawPolyline(), drawPolygon(),
    drawConvexPolygon() and drawCubicBezier(). All of these functions
    take integer coordinates; there are no floating-point versions
    since we want drawing to be as fast as possible.

    There are functions to draw pixmaps/images, namely drawPixmap(),
    drawImage() and drawTiledPixmap(). drawPixmap() and drawImage()
    produce the same result, except that drawPixmap() is faster
    on-screen and drawImage() faster and sometimes better on TQPrinter
    and TQPicture.

    Text drawing is done using drawText(), and when you need
    fine-grained positioning, boundingRect() tells you where a given
    drawText() command would draw.

    There is a drawPicture() function that draws the contents of an
    entire TQPicture using this painter. drawPicture() is the only
    function that disregards all the painter's settings: the TQPicture
    has its own settings.

    Normally, the TQPainter operates on the device's own coordinate
    system (usually pixels), but TQPainter has good support for
    coordinate transformation. See \link coordsys.html The Coordinate
    System \endlink for a more general overview and a simple example.

    The most common functions used are scale(), rotate(), translate()
    and shear(), all of which operate on the worldMatrix().
    setWorldMatrix() can replace or add to the currently set
    worldMatrix().

    setViewport() sets the rectangle on which TQPainter operates. The
    default is the entire device, which is usually fine, except on
    printers. setWindow() sets the coordinate system, that is, the
    rectangle that maps to viewport(). What's drawn inside the
    window() ends up being inside the viewport(). The window's
    default is the same as the viewport, and if you don't use the
    transformations, they are optimized away, gaining another little
    bit of speed.

    After all the coordinate transformation is done, TQPainter can clip
    the drawing to an arbitrary rectangle or region. hasClipping() is
    true if TQPainter clips, and clipRegion() returns the clip region.
    You can set it using either setClipRegion() or setClipRect().
    Note that the clipping can be slow. It's all system-dependent,
    but as a rule of thumb, you can assume that drawing speed is
    inversely proportional to the number of rectangles in the clip
    region.

    After TQPainter's clipping, the paint device may also clip. For
    example, most widgets clip away the pixels used by child widgets,
    and most printers clip away an area near the edges of the paper.
    This additional clipping is not reflected by the return value of
    clipRegion() or hasClipping().

    TQPainter also includes some less-used functions that are very
    useful on those occasions when they're needed.

    isActive() indicates whether the painter is active. begin() (and
    the most usual constructor) makes it active. end() (and the
    destructor) deactivates it. If the painter is active, device()
    returns the paint device on which the painter paints.

    Sometimes it is desirable to make someone else paint on an unusual
    TQPaintDevice. TQPainter supports a static function to do this,
    redirect(). We recommend not using it, but for some hacks it's
    perfect.

    setTabStops() and setTabArray() can change where the tab stops
    are, but these are very seldomly used.

    \warning Note that TQPainter does not attempt to work around
    coordinate limitations in the underlying window system. Some
    platforms may behave incorrectly with coordinates as small as
    +/-4000.

    \headerfile ntqdrawutil.h

    \sa TQPaintDevice TQWidget TQPixmap TQPrinter TQPicture
	\link simple-application.html Application Walkthrough \endlink
	\link coordsys.html Coordinate System Overview \endlink
*/

/*!
    \fn TQGfx * TQPainter::internalGfx()

    \internal
*/

/*!
    \enum TQPainter::CoordinateMode
    \value CoordDevice
    \value CoordPainter

    \sa clipRegion()
*/
/*!
    \enum TQPainter::TextDirection
    \value Auto
    \value RTL right to left
    \value LTR left to right

    \sa drawText()
*/

/*!
    \enum TQt::PaintUnit
    \value PixelUnit
    \value LoMetricUnit \e obsolete
    \value HiMetricUnit \e obsolete
    \value LoEnglishUnit \e obsolete
    \value HiEnglishUnit \e obsolete
    \value TwipsUnit \e obsolete
*/

/*!
    \enum TQt::BrushStyle

    \value NoBrush
    \value SolidPattern
    \value Dense1Pattern
    \value Dense2Pattern
    \value Dense3Pattern
    \value Dense4Pattern
    \value Dense5Pattern
    \value Dense6Pattern
    \value Dense7Pattern
    \value HorPattern
    \value VerPattern
    \value CrossPattern
    \value BDiagPattern
    \value FDiagPattern
    \value DiagCrossPattern
    \value CustomPattern

    \img brush-styles.png Brush Styles

*/

/*!
    \enum TQt::RasterOp

    This enum type is used to describe the way things are written to
    the paint device. Each bit of the \e src (what you write)
    interacts with the corresponding bit of the \e dst pixel.

    \value CopyROP  dst = src
    \value OrROP   dst = src OR dst
    \value XorROP   dst = src XOR dst
    \value NotAndROP  dst = (NOT src) AND dst
    \value EraseROP  an alias for \c NotAndROP
    \value NotCopyROP  dst = NOT src
    \value NotOrROP  dst = (NOT src) OR dst
    \value NotXorROP  dst = (NOT src) XOR dst
    \value AndROP  dst = src AND dst
    \value NotEraseROP  an alias for \c AndROP
    \value NotROP  dst = NOT dst
    \value ClearROP  dst = 0
    \value SetROP  dst = 1
    \value NopROP  dst = dst
    \value AndNotROP  dst = src AND (NOT dst)
    \value OrNotROP  dst = src OR (NOT dst)
    \value NandROP  dst = NOT (src AND dst)
    \value NorROP  dst = NOT (src OR dst)

    By far the most useful ones are \c CopyROP and \c XorROP.

    On TQt/Embedded, only \c CopyROP, \c XorROP, and \c NotROP are supported.
*/

/*!
    \enum TQt::AlignmentFlags

    This enum type is used to describe alignment. It contains
    horizontal and vertical flags.

    The horizontal flags are:

    \value AlignAuto Aligns according to the language. Left for most,
	right for Arabic and Hebrew.
    \value AlignLeft Aligns with the left edge.
    \value AlignRight Aligns with the right edge.
    \value AlignHCenter Centers horizontally in the available space.
    \value AlignJustify Justifies the text in the available space.
	Does not work for everything and may be interpreted as
	AlignAuto in some cases.

    The vertical flags are:

    \value AlignTop Aligns with the top.
    \value AlignBottom Aligns with the bottom.
    \value AlignVCenter Centers vertically in the available space.

    You can use only one of the horizontal flags at a time. There is
    one two-dimensional flag:

    \value AlignCenter Centers in both dimensions.

    You can use at most one horizontal and one vertical flag at a time. \c
    AlignCenter counts as both horizontal and vertical.

    Masks:

    \value AlignHorizontal_Mask
    \value AlignVertical_Mask

    Conflicting combinations of flags have undefined meanings.
*/

/*!
    \enum TQt::TextFlags

    This enum type is used to define some modifier flags. Some of
    these flags only make sense in the context of printing:

    \value SingleLine Treats all whitespace as spaces and prints just
	one line.
    \value DontClip If it's impossible to stay within the given bounds,
	it prints outside.
    \value ExpandTabs Makes the U+0009 (ASCII tab) character move to
	the next tab stop.
    \value ShowPrefix Displays the string "\&P" as <u>P</u>
	(see TQButton for an example). For an ampersand, use "\&\&".
    \value WordBreak Breaks lines at appropriate points, e.g. at word
	boundaries.
    \value BreakAnywhere Breaks lines anywhere, even within words.
    \value NoAccel Same as ShowPrefix but doesn't draw the underlines.

    You can use as many modifier flags as you want, except that \c
    SingleLine and \c WordBreak cannot be combined.

    Flags that are inappropriate for a given use (e.g. ShowPrefix to
    TQGridLayout::addWidget()) are generally ignored.

*/

/*!
    \enum TQt::PenStyle

    This enum type defines the pen styles that can be drawn using
    TQPainter. The styles are

    \value NoPen  no line at all. For example, TQPainter::drawRect()
    fills but does not draw any boundary line.

    \value SolidLine  a simple line.

    \value DashLine  dashes separated by a few pixels.

    \value DotLine  dots separated by a few pixels.

    \value DashDotLine  alternate dots and dashes.

    \value DashDotDotLine  one dash, two dots, one dash, two dots.

    \value FineDotLine  single pixel dots separated by single pixels.

    \value MPenStyle mask of the pen styles.

    \img pen-styles.png Pen Styles
*/

/*!
    \enum TQt::PenCapStyle

    This enum type defines the pen cap styles supported by TQt, i.e.
    the line end caps that can be drawn using TQPainter.

    \value FlatCap  a square line end that does not cover the end
	point of the line.
    \value SquareCap  a square line end that covers the end point and
	extends beyond it with half the line width.
    \value RoundCap  a rounded line end.
    \value MPenCapStyle mask of the pen cap styles.

    \img pen-cap-styles.png Pen Cap Styles
*/

/*!
    \enum TQt::PenJoinStyle

    This enum type defines the pen join styles supported by TQt, i.e.
    which joins between two connected lines can be drawn using
    TQPainter.

    \value MiterJoin  The outer edges of the lines are extended to
	meet at an angle, and this area is filled.
    \value BevelJoin  The triangular notch between the two lines is filled.
    \value RoundJoin  A circular arc between the two lines is filled.
    \value MPenJoinStyle mask of the pen join styles.

    \img pen-join-styles.png Pen Join Styles
*/

/*!
    \enum TQt::BGMode

    Background mode

    \value TransparentMode
    \value OpaqueMode
*/

/*!
    Constructs a painter.

    Notice that all painter settings (setPen, setBrush etc.) are reset
    to default values when begin() is called.

    \sa begin(), end()
*/

TQPainter::TQPainter()
{
    init();
}


/*!
    Constructs a painter that begins painting the paint device \a pd
    immediately. Depending on the underlying graphic system the
    painter will paint over children of the paintdevice if \a
    unclipped is true.

    This constructor is convenient for short-lived painters, e.g. in a
    \link TQWidget::paintEvent() paint event\endlink and should be used
    only once. The constructor calls begin() for you and the TQPainter
    destructor automatically calls end().

    Here's an example using begin() and end():
    \code
	void MyWidget::paintEvent( TQPaintEvent * )
	{
	    TQPainter p;
	    p.begin( this );
	    p.drawLine( ... );	// drawing code
	    p.end();
	}
    \endcode

    The same example using this constructor:
    \code
	void MyWidget::paintEvent( TQPaintEvent * )
	{
	    TQPainter p( this );
	    p.drawLine( ... );	// drawing code
	}
    \endcode

    Since the constructor cannot provide feedback when the initialization
    of the painter failed you should rather use begin() and end() to paint
    on external devices, e.g. printers.

    \sa begin(), end()
*/

TQPainter::TQPainter( const TQPaintDevice *pd, bool unclipped )
{
    init();
    if ( begin( pd, unclipped ) )
	flags |= CtorBegin;
}


/*!
    Constructs a painter that begins painting the paint device \a pd
    immediately, with the default arguments taken from \a
    copyAttributes. The painter will paint over children of the paint
    device if \a unclipped is true (although this is not supported on
    all platforms).

    \sa begin()
*/

TQPainter::TQPainter( const TQPaintDevice *pd,
		    const TQWidget *copyAttributes, bool unclipped )
{
    init();
    if ( begin( pd, copyAttributes, unclipped ) )
	flags |= CtorBegin;
}


/*!
    Destroys the painter.
*/

TQPainter::~TQPainter()
{
    if ( isActive() )
	end();
    else
	killPStack();
    if ( tabarray )				// delete tab array
	delete [] tabarray;
#ifndef TQT_NO_TRANSFORMATIONS
    if ( wm_stack )
	delete (TQWMatrixStack *)wm_stack;
#endif
    destroy();
}


/*!
    \overload bool TQPainter::begin( const TQPaintDevice *pd, const TQWidget *copyAttributes, bool unclipped )

    This version opens the painter on a paint device \a pd and sets
    the initial pen, background color and font from \a copyAttributes,
    painting over the paint device's children when \a unclipped is
    true. This is equivalent to:

    \code
	TQPainter p;
	p.begin( pd );
	p.setPen( copyAttributes->foregroundColor() );
	p.setBackgroundColor( copyAttributes->backgroundColor() );
	p.setFont( copyAttributes->font() );
    \endcode

    This begin function is convenient for double buffering. When you
    draw in a pixmap instead of directly in a widget (to later bitBlt
    the pixmap into the widget) you will need to set the widget's
    font etc. This function does exactly that.

    Example:
    \code
	void MyWidget::paintEvent( TQPaintEvent * )
	{
	    TQPixmap pm(size());
	    TQPainter p;
	    p.begin(&pm, this);
	    // ... potentially flickering paint operation ...
	    p.end();
	    bitBlt(this, 0, 0, &pm);
	}
    \endcode

    \sa end()
*/

bool TQPainter::begin( const TQPaintDevice *pd, const TQWidget *copyAttributes, bool unclipped )
{
    if ( copyAttributes == 0 ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQPainter::begin: The widget to copy attributes from cannot "
		 "be null" );
#endif
	return false;
    }
    if ( begin( pd, unclipped ) ) {
	setPen( copyAttributes->foregroundColor() );
	setBackgroundColor( copyAttributes->backgroundColor() );
	setFont( copyAttributes->font() );
	return true;
    }
    return false;
}


/*!
  \internal
  Sets or clears a pointer flag.
*/

void TQPainter::setf( uint b, bool v )
{
    if ( v )
	setf( b );
    else
	clearf( b );
}


/*!
    \fn bool TQPainter::isActive() const

    Returns true if the painter is active painting, i.e. begin() has
    been called and end() has not yet been called; otherwise returns
    false.

    \sa TQPaintDevice::paintingActive()
*/

/*!
    \fn TQPaintDevice *TQPainter::device() const

    Returns the paint device on which this painter is currently
    painting, or 0 if the painter is not active.

    \sa TQPaintDevice::paintingActive()
*/


struct TQPState {				// painter state
    TQFont	font;
    TQPen	pen;
    TQPoint	curPt;
    TQBrush	brush;
    TQColor	bgc;
    uchar	bgm;
    uchar	rop;
    TQPoint	bro;
    TQRect	wr, vr;
#ifndef TQT_NO_TRANSFORMATIONS
    TQWMatrix	wm;
#else
    int		xlatex;
    int		xlatey;
#endif
    bool	vxf;
    bool	wxf;
    TQRegion	rgn;
    bool	clip;
    int		ts;
    int	       *ta;
    void* wm_stack;
};

//TODO lose the worldmatrix stack

typedef TQPtrStack<TQPState> TQPStateStack;


void TQPainter::killPStack()
{
#if defined(QT_CHECK_STATE)
    if ( ps_stack && !((TQPStateStack *)ps_stack)->isEmpty() )
	tqWarning( "TQPainter::killPStack: non-empty save/restore stack when "
		  "end() was called" );
#endif
    delete (TQPStateStack *)ps_stack;
    ps_stack = 0;
}

/*!
    Saves the current painter state (pushes the state onto a stack). A
    save() must be followed by a corresponding restore(). end()
    unwinds the stack.

    \sa restore()
*/

void TQPainter::save()
{
    if ( testf(ExtDev) ) {
	if ( testf(DirtyFont) )
	    updateFont();
	if ( testf(DirtyPen) )
	    updatePen();
	if ( testf(DirtyBrush) )
	    updateBrush();
	pdev->cmd( TQPaintDevice::PdcSave, this, 0 );
    }
    TQPStateStack *pss = (TQPStateStack *)ps_stack;
    if ( pss == 0 ) {
	pss = new TQPtrStack<TQPState>;
	TQ_CHECK_PTR( pss );
	pss->setAutoDelete( true );
	ps_stack = pss;
    }
    TQPState *ps = new TQPState;
    TQ_CHECK_PTR( ps );
    ps->font  = cfont;
    ps->pen   = cpen;
    ps->curPt = pos();
    ps->brush = cbrush;
    ps->bgc   = bg_col;
    ps->bgm   = bg_mode;
    ps->rop   = rop;
    ps->bro   = bro;
#ifndef TQT_NO_TRANSFORMATIONS
    ps->wr    = TQRect( wx, wy, ww, wh );
    ps->vr    = TQRect( vx, vy, vw, vh );
    ps->wm    = wxmat;
    ps->vxf   = testf(VxF);
    ps->wxf   = testf(WxF);
#else
    ps->xlatex = xlatex;
    ps->xlatey = xlatey;
#endif
    ps->rgn   = crgn;
    ps->clip  = testf(ClipOn);
    ps->ts    = tabstops;
    ps->ta    = tabarray;
    ps->wm_stack = wm_stack;
    wm_stack = 0;
    pss->push( ps );
}

/*!
    Restores the current painter state (pops a saved state off the
    stack).

    \sa save()
*/

void TQPainter::restore()
{
    if ( testf(ExtDev) ) {
	pdev->cmd( TQPaintDevice::PdcRestore, this, 0 );
	if ( pdev->devType() == TQInternal::Picture )
	    block_ext = true;
    }
    TQPStateStack *pss = (TQPStateStack *)ps_stack;
    if ( pss == 0 || pss->isEmpty() ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQPainter::restore: Empty stack error" );
#endif
	return;
    }
    TQPState *ps = pss->pop();
    bool hardRestore = testf(VolatileDC);

    if ( ps->font != cfont || hardRestore )
	setFont( ps->font );
    if ( ps->pen != cpen || hardRestore )
	setPen( ps->pen );
    if ( ps->brush != cbrush || hardRestore )
	setBrush( ps->brush );
    if ( ps->bgc != bg_col || hardRestore )
	setBackgroundColor( ps->bgc );
    if ( ps->bgm != bg_mode || hardRestore )
	setBackgroundMode( (BGMode)ps->bgm );
    if ( ps->rop != rop || hardRestore )
	setRasterOp( (RasterOp)ps->rop );
    if ( ps->bro != bro || hardRestore )
	setBrushOrigin( ps->bro );
#ifndef TQT_NO_TRANSFORMATIONS
    TQRect wr( wx, wy, ww, wh );
    TQRect vr( vx, vy, vw, vh );
    if ( ps->wr != wr || hardRestore )
	setWindow( ps->wr );
    if ( ps->vr != vr || hardRestore )
	setViewport( ps->vr );
    if ( ps->wm != wxmat || hardRestore )
	setWorldMatrix( ps->wm );
    if ( ps->vxf != testf(VxF) || hardRestore )
	setViewXForm( ps->vxf );
    if ( ps->wxf != testf(WxF) || hardRestore )
	setWorldXForm( ps->wxf );
#else
    xlatex = ps->xlatex;
    xlatey = ps->xlatey;
    setf( VxF, xlatex || xlatey );
#endif
    if ( ps->curPt != pos() || hardRestore )
	moveTo( ps->curPt );
    if ( ps->rgn != crgn || hardRestore )
	setClipRegion( ps->rgn );
    if ( ps->clip != testf(ClipOn) || hardRestore )
	setClipping( ps->clip );
    tabstops = ps->ts;
    tabarray = ps->ta;

#ifndef TQT_NO_TRANSFORMATIONS
    if ( wm_stack )
	delete (TQWMatrixStack *)wm_stack;
    wm_stack = ps->wm_stack;
#endif
    delete ps;
    block_ext = false;
}

typedef TQPtrDict<TQPaintDevice> TQPaintDeviceDict;
static TQPaintDeviceDict *pdev_dict = 0;

/*!
    Redirects all paint commands for a paint device, \a pdev, to
    another paint device, \a replacement, unless \a replacement is 0.
    If \a replacement is 0, the redirection for \a pdev is removed.

    In general, you'll probably find calling TQPixmap::grabWidget() or
    TQPixmap::grabWindow() is an easier solution.
*/

void TQPainter::redirect( TQPaintDevice *pdev, TQPaintDevice *replacement )
{
    if ( pdev_dict == 0 ) {
        if ( replacement == 0 )
            return;
        pdev_dict = new TQPaintDeviceDict;
        TQ_CHECK_PTR( pdev_dict );
    }
#if defined(QT_CHECK_NULL)
    if ( pdev == 0 )
        tqWarning( "TQPainter::redirect: The pdev argument cannot be 0" );
#endif
    if ( replacement ) {
        pdev_dict->insert( pdev, replacement );
    } else {
        pdev_dict->remove( pdev );
        if ( pdev_dict->count() == 0 ) {
            delete pdev_dict;
            pdev_dict = 0;
        }
    }
}

/*!
    \internal
    Returns the replacement for \a pdev, or 0 if there is no replacement.
*/
TQPaintDevice *TQPainter::redirect( TQPaintDevice *pdev )
{
    return pdev_dict ? pdev_dict->find( pdev ) : 0;
}

/*!
    Returns the font metrics for the painter, if the painter is
    active. It is not possible to obtain metrics for an inactive
    painter, so the return value is undefined if the painter is not
    active.

    \sa fontInfo(), isActive()
*/

TQFontMetrics TQPainter::fontMetrics() const
{
    if ( pdev && pdev->devType() == TQInternal::Picture )
	return TQFontMetrics( cfont );

    return TQFontMetrics(this);
}

/*!
    Returns the font info for the painter, if the painter is active.
    It is not possible to obtain font information for an inactive
    painter, so the return value is undefined if the painter is not
    active.

    \sa fontMetrics(), isActive()
*/

TQFontInfo TQPainter::fontInfo() const
{
    if ( pdev && pdev->devType() == TQInternal::Picture )
	return TQFontInfo( cfont );

    return TQFontInfo(this);
}


/*!
    \fn const TQPen &TQPainter::pen() const

    Returns the painter's current pen.

    \sa setPen()
*/

/*!
    Sets a new painter pen.

    The \a pen defines how to draw lines and outlines, and it also
    defines the text color.

    \sa pen()
*/

void TQPainter::setPen( const TQPen &pen )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	tqWarning( "TQPainter::setPen: Will be reset by begin()" );
#endif
    if ( cpen == pen )
	return;
    cpen = pen;
    updatePen();
}

/*!
    \overload

    Sets the painter's pen to have style \a style, width 0 and black
    color.

    \sa pen(), TQPen
*/

void TQPainter::setPen( PenStyle style )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	tqWarning( "TQPainter::setPen: Will be reset by begin()" );
#endif
    TQPen::TQPenData *d = cpen.data;	// low level access
    if ( d->style == style && d->linest == style && !d->width && d->color == TQt::black )
	return;
    if ( d->count != 1 ) {
	cpen.detach();
	d = cpen.data;
    }
    d->style = style;
    d->width = 0;
    d->color = TQt::black;
    d->linest = style;
    updatePen();
}

/*!
    \overload

    Sets the painter's pen to have style \c SolidLine, width 0 and the
    specified \a color.

    \sa pen(), TQPen
*/

void TQPainter::setPen( const TQColor &color )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	tqWarning( "TQPainter::setPen: Will be reset by begin()" );
#endif
    TQPen::TQPenData *d = cpen.data;	// low level access
    if ( d->color == color && !d->width && d->style == SolidLine && d->linest == SolidLine )
	return;
    if ( d->count != 1 ) {
	cpen.detach();
	d = cpen.data;
    }
    d->style = SolidLine;
    d->width = 0;
    d->color = color;
    d->linest = SolidLine;
    updatePen();
}

/*!
    \fn const TQBrush &TQPainter::brush() const

    Returns the painter's current brush.

    \sa TQPainter::setBrush()
*/

/*!
    \overload

    Sets the painter's brush to \a brush.

    The \a brush defines how shapes are filled.

    \sa brush()
*/

void TQPainter::setBrush( const TQBrush &brush )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	tqWarning( "TQPainter::setBrush: Will be reset by begin()" );
#endif
    if ( cbrush == brush )
	return;
    cbrush = brush;
    updateBrush();
}

/*!
    Sets the painter's brush to black color and the specified \a
    style.

    \sa brush(), TQBrush
*/

void TQPainter::setBrush( BrushStyle style )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	tqWarning( "TQPainter::setBrush: Will be reset by begin()" );
#endif
    TQBrush::TQBrushData *d = cbrush.data; // low level access
    if ( d->style == style && d->color == TQt::black && !d->pixmap )
	return;
    if ( d->count != 1 ) {
	cbrush.detach();
	d = cbrush.data;
    }
    d->style = style;
    d->color = TQt::black;
    if ( d->pixmap ) {
	delete d->pixmap;
	d->pixmap = 0;
    }
    updateBrush();
}

/*!
    \overload

    Sets the painter's brush to have style \c SolidPattern and the
    specified \a color.

    \sa brush(), TQBrush
*/

void TQPainter::setBrush( const TQColor &color )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	tqWarning( "TQPainter::setBrush: Will be reset by begin()" );
#endif
    TQBrush::TQBrushData *d = cbrush.data; // low level access
    if ( d->color == color && d->style == SolidPattern && !d->pixmap )
	return;
    if ( d->count != 1 ) {
	cbrush.detach();
	d = cbrush.data;
    }
    d->style = SolidPattern;
    d->color = color;
    if ( d->pixmap ) {
	delete d->pixmap;
	d->pixmap = 0;
    }
    updateBrush();
}


/*!
    \fn const TQColor &TQPainter::backgroundColor() const

    Returns the current background color.

    \sa setBackgroundColor() TQColor
*/

/*!
    \fn BGMode TQPainter::backgroundMode() const

    Returns the current background mode.

    \sa setBackgroundMode() BGMode
*/

/*!
    \fn RasterOp TQPainter::rasterOp() const

    Returns the current \link TQt::RasterOp raster operation \endlink.

    \sa setRasterOp() RasterOp
*/

/*!
    \fn const TQPoint &TQPainter::brushOrigin() const

    Returns the brush origin currently set.

    \sa setBrushOrigin()
*/


/*!
    \fn int TQPainter::tabStops() const

    Returns the tab stop setting.

    \sa setTabStops()
*/

/*!
    Set the tab stop width to \a ts, i.e. locates tab stops at \a ts,
    2*\a ts, 3*\a ts and so on.

    Tab stops are used when drawing formatted text with \c ExpandTabs
    set. This fixed tab stop value is used only if no tab array is set
    (which is the default case).

    A value of 0 (the default) implies a tabstop setting of 8 times the width of the
    character 'x' in the font currently set on the painter.

    \sa tabStops(), setTabArray(), drawText(), fontMetrics()
*/

void TQPainter::setTabStops( int ts )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	tqWarning( "TQPainter::setTabStops: Will be reset by begin()" );
#endif
    tabstops = ts;
    if ( isActive() && testf(ExtDev) ) {	// tell extended device
	TQPDevCmdParam param[1];
	param[0].ival = ts;
	pdev->cmd( TQPaintDevice::PdcSetTabStops, this, param );
    }
}

/*!
    \fn int *TQPainter::tabArray() const

    Returns the currently set tab stop array.

    \sa setTabArray()
*/

/*!
    Sets the tab stop array to \a ta. This puts tab stops at \a ta[0],
    \a ta[1] and so on. The array is null-terminated.

    If both a tab array and a tab top size is set, the tab array wins.

    \sa tabArray(), setTabStops(), drawText(), fontMetrics()
*/

void TQPainter::setTabArray( int *ta )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	tqWarning( "TQPainter::setTabArray: Will be reset by begin()" );
#endif
    if ( ta != tabarray ) {
	tabarraylen = 0;
	if ( tabarray )				// Avoid purify complaint
	    delete [] tabarray;			// delete old array
	if ( ta ) {				// tabarray = copy of 'ta'
	    while ( ta[tabarraylen] )
		tabarraylen++;
	    tabarraylen++; // and 0 terminator
	    tabarray = new int[tabarraylen];	// duplicate ta
	    memcpy( tabarray, ta, sizeof(int)*tabarraylen );
	} else {
	    tabarray = 0;
	}
    }
    if ( isActive() && testf(ExtDev) ) {	// tell extended device
	TQPDevCmdParam param[2];
	param[0].ival = tabarraylen;
	param[1].ivec = tabarray;
	pdev->cmd( TQPaintDevice::PdcSetTabArray, this, param );
    }
}


/*!
    \fn HANDLE TQPainter::handle() const

    Returns the platform-dependent handle used for drawing. Using this
    function is not portable.
*/


/*****************************************************************************
  TQPainter xform settings
 *****************************************************************************/

#ifndef TQT_NO_TRANSFORMATIONS

/*!
    Enables view transformations if \a enable is true, or disables
    view transformations if \a enable is false.

    \sa hasViewXForm(), setWindow(), setViewport(), setWorldMatrix(),
    setWorldXForm(), xForm()
*/

void TQPainter::setViewXForm( bool enable )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	tqWarning( "TQPainter::setViewXForm: Will be reset by begin()" );
#endif
    if ( !isActive() || enable == testf(VxF) )
	return;
    setf( VxF, enable );
    if ( testf(ExtDev) ) {
	TQPDevCmdParam param[1];
	param[0].ival = enable;
	pdev->cmd( TQPaintDevice::PdcSetVXform, this, param );
    }
    updateXForm();
}

/*!
    \fn bool TQPainter::hasViewXForm() const

    Returns true if view transformation is enabled; otherwise returns
    false.

    \sa setViewXForm(), xForm()
*/

/*!
    Returns the window rectangle.

    \sa setWindow(), setViewXForm()
*/

TQRect TQPainter::window() const
{
    return TQRect( wx, wy, ww, wh );
}

/*!
    Sets the window rectangle view transformation for the painter and
    enables view transformation.

    The window rectangle is part of the view transformation. The
    window specifies the logical coordinate system and is specified by
    the \a x, \a y, \a w width and \a h height parameters. Its sister,
    the viewport(), specifies the device coordinate system.

    The default window rectangle is the same as the device's
    rectangle. See the \link coordsys.html Coordinate System Overview
    \endlink for an overview of coordinate transformation.

    \sa window(), setViewport(), setViewXForm(), setWorldMatrix(),
    setWorldXForm()
*/

void TQPainter::setWindow( int x, int y, int w, int h )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	tqWarning( "TQPainter::setWindow: Will be reset by begin()" );
#endif
    wx = x;
    wy = y;
    ww = w;
    wh = h;
    if ( testf(ExtDev) ) {
	TQRect r( x, y, w, h );
	TQPDevCmdParam param[1];
	param[0].rect = (TQRect*)&r;
	pdev->cmd( TQPaintDevice::PdcSetWindow, this, param );
    }
    if ( testf(VxF) )
	updateXForm();
    else
	setViewXForm( true );
}

/*!
    Returns the viewport rectangle.

    \sa setViewport(), setViewXForm()
*/

TQRect TQPainter::viewport() const		// get viewport
{
    return TQRect( vx, vy, vw, vh );
}

/*!
    Sets the viewport rectangle view transformation for the painter
    and enables view transformation.

    The viewport rectangle is part of the view transformation. The
    viewport specifies the device coordinate system and is specified
    by the \a x, \a y, \a w width and \a h height parameters. Its
    sister, the window(), specifies the logical coordinate system.

    The default viewport rectangle is the same as the device's
    rectangle. See the \link coordsys.html Coordinate System Overview
    \endlink for an overview of coordinate transformation.

    \sa viewport(), setWindow(), setViewXForm(), setWorldMatrix(),
    setWorldXForm(), xForm()
*/

void TQPainter::setViewport( int x, int y, int w, int h )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	tqWarning( "TQPainter::setViewport: Will be reset by begin()" );
#endif
    vx = x;
    vy = y;
    vw = w;
    vh = h;
    if ( testf(ExtDev) ) {
	TQRect r( x, y, w, h );
	TQPDevCmdParam param[1];
	param[0].rect = (TQRect*)&r;
	pdev->cmd( TQPaintDevice::PdcSetViewport, this, param );
    }
    if ( testf(VxF) )
	updateXForm();
    else
	setViewXForm( true );
}


/*!
    Enables world transformations if \a enable is true, or disables
    world transformations if \a enable is false. The world
    transformation matrix is not changed.

    \sa setWorldMatrix(), setWindow(), setViewport(), setViewXForm(),
    xForm()
*/

void TQPainter::setWorldXForm( bool enable )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	tqWarning( "TQPainter::setWorldXForm: Will be reset by begin()" );
#endif
    if ( !isActive() || enable == testf(WxF) )
	return;
    setf( WxF, enable );
    if ( testf(ExtDev)  && !block_ext ) {
	TQPDevCmdParam param[1];
	param[0].ival = enable;
	pdev->cmd( TQPaintDevice::PdcSetWXform, this, param );
    }
    updateXForm();
}

/*!
    \fn bool TQPainter::hasWorldXForm() const

    Returns true if world transformation is enabled; otherwise returns
    false.

    \sa setWorldXForm()
*/

/*!
    Returns the world transformation matrix.

    \sa setWorldMatrix()
*/

const TQWMatrix &TQPainter::worldMatrix() const
{
    return wxmat;
}

/*!
    Sets the world transformation matrix to \a m and enables world
    transformation.

    If \a combine is true, then \a m is combined with the current
    transformation matrix, otherwise \a m replaces the current
    transformation matrix.

    If \a m is the identity matrix and \a combine is false, this
    function calls setWorldXForm(false). (The identity matrix is the
    matrix where TQWMatrix::m11() and TQWMatrix::m22() are 1.0 and the
    rest are 0.0.)

    World transformations are applied after the view transformations
    (i.e. \link setWindow() window\endlink and \link setViewport()
    viewport\endlink).

    The following functions can transform the coordinate system without using
    a TQWMatrix:
    \list
    \i translate()
    \i scale()
    \i shear()
    \i rotate()
    \endlist

    They operate on the painter's worldMatrix() and are implemented like this:

    \code
	void TQPainter::rotate( double a )
	{
	    TQWMatrix m;
	    m.rotate( a );
	    setWorldMatrix( m, true );
	}
    \endcode

    Note that you should always use \a combine when you are drawing
    into a TQPicture. Otherwise it may not be possible to replay the
    picture with additional transformations. Using translate(),
    scale(), etc., is safe.

    For a brief overview of coordinate transformation, see the \link
    coordsys.html Coordinate System Overview. \endlink

    \sa worldMatrix() setWorldXForm() setWindow() setViewport()
    setViewXForm() xForm() TQWMatrix
*/

void TQPainter::setWorldMatrix( const TQWMatrix &m, bool combine )
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQPainter::setWorldMatrix: Will be reset by begin()" );
#endif
	return;
    }
    if ( combine )
	wxmat = m * wxmat;			// combines
    else
	wxmat = m;				// set new matrix
    bool identity = wxmat.m11() == 1.0F && wxmat.m22() == 1.0F &&
		    wxmat.m12() == 0.0F && wxmat.m21() == 0.0F &&
		    wxmat.dx()	== 0.0F && wxmat.dy()  == 0.0F;
    if ( testf(ExtDev) && !block_ext ) {
	TQPDevCmdParam param[2];
	param[0].matrix = &m;
	param[1].ival = combine;
	pdev->cmd( TQPaintDevice::PdcSetWMatrix, this, param );
    }
    if ( identity && pdev->devType() != TQInternal::Picture )
	setWorldXForm( false );
    else if ( !testf(WxF) )
	setWorldXForm( true );
    else
	updateXForm();
}

/*! \obsolete

  We recommend using save() instead.
*/

void TQPainter::saveWorldMatrix()
{
    TQWMatrixStack *stack = (TQWMatrixStack *)wm_stack;
    if ( stack == 0 ) {
	stack  = new TQPtrStack<TQWMatrix>;
	TQ_CHECK_PTR( stack );
	stack->setAutoDelete( true );
	wm_stack = stack;
    }

    stack->push( new TQWMatrix( wxmat ) );

}

/*! \obsolete
  We recommend using restore() instead.
*/

void TQPainter::restoreWorldMatrix()
{
    TQWMatrixStack *stack = (TQWMatrixStack *)wm_stack;
    if ( stack == 0 || stack->isEmpty() ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQPainter::restoreWorldMatrix: Empty stack error" );
#endif
	return;
    }
    TQWMatrix* m = stack->pop();
    setWorldMatrix( *m );
    delete m;
}

#endif // TQT_NO_TRANSFORMATIONS

/*!
    Translates the coordinate system by \a (dx, dy). After this call,
    \a (dx, dy) is added to points.

    For example, the following code draws the same point twice:
    \code
	void MyWidget::paintEvent()
	{
	    TQPainter paint( this );

	    paint.drawPoint( 0, 0 );

	    paint.translate( 100.0, 40.0 );
	    paint.drawPoint( -100, -40 );
	}
    \endcode

    \sa scale(), shear(), rotate(), resetXForm(), setWorldMatrix(), xForm()
*/

void TQPainter::translate( double dx, double dy )
{
#ifndef TQT_NO_TRANSFORMATIONS
    TQWMatrix m;
    m.translate( dx, dy );
    setWorldMatrix( m, true );
#else
    xlatex += (int)dx;
    xlatey += (int)dy;
    setf( VxF, xlatex || xlatey );
#endif
}


#ifndef TQT_NO_TRANSFORMATIONS
/*!
    Scales the coordinate system by \a (sx, sy).

    \sa translate(), shear(), rotate(), resetXForm(), setWorldMatrix(),
    xForm()
*/

void TQPainter::scale( double sx, double sy )
{
    TQWMatrix m;
    m.scale( sx, sy );
    setWorldMatrix( m, true );
}

/*!
    Shears the coordinate system by \a (sh, sv).

    \sa translate(), scale(), rotate(), resetXForm(), setWorldMatrix(),
    xForm()
*/

void TQPainter::shear( double sh, double sv )
{
    TQWMatrix m;
    m.shear( sv, sh );
    setWorldMatrix( m, true );
}

/*!
    Rotates the coordinate system \a a degrees counterclockwise.

    \sa translate(), scale(), shear(), resetXForm(), setWorldMatrix(),
    xForm()
*/

void TQPainter::rotate( double a )
{
    TQWMatrix m;
    m.rotate( a );
    setWorldMatrix( m, true );
}


/*!
    Resets any transformations that were made using translate(), scale(),
    shear(), rotate(), setWorldMatrix(), setViewport() and
    setWindow().

    \sa worldMatrix(), viewport(), window()
*/

void TQPainter::resetXForm()
{
    if ( !isActive() )
	return;
    wx = wy = vx = vy = 0;			// default view origins
    ww = vw = pdev->metric( TQPaintDeviceMetrics::PdmWidth );
    wh = vh = pdev->metric( TQPaintDeviceMetrics::PdmHeight );
    wxmat = TQWMatrix();
    setWorldXForm( false );
    setViewXForm( false );
}

/*!
  \internal
  Updates an internal integer transformation matrix.
*/

void TQPainter::updateXForm()
{
    TQWMatrix m;
    if ( testf(VxF) ) {
	double scaleW = (double)vw/(double)ww;
	double scaleH = (double)vh/(double)wh;
	m.setMatrix( scaleW, 0,  0,  scaleH, vx - wx*scaleW, vy - wy*scaleH );
    }
    if ( testf(WxF) ) {
	if ( testf(VxF) )
	    m = wxmat * m;
	else
	    m = wxmat;
    }
    xmat = m;

    txinv = false;				// no inverted matrix
    txop  = TxNone;
    if ( m12()==0.0 && m21()==0.0 && m11() >= 0.0 && m22() >= 0.0 ) {
	if ( m11()==1.0 && m22()==1.0 ) {
	    if ( dx()!=0.0 || dy()!=0.0 )
		txop = TxTranslate;
	} else {
	    txop = TxScale;
#if defined(TQ_WS_WIN)
	    setf(DirtyFont);
#endif
	}
    } else {
	txop = TxRotShear;
#if defined(TQ_WS_WIN)
	setf(DirtyFont);
#endif
    }
}


/*!
  \internal
  Updates an internal integer inverse transformation matrix.
*/

void TQPainter::updateInvXForm()
{
#if defined(QT_CHECK_STATE)
    Q_ASSERT( txinv == false );
#endif
    txinv = true;				// creating inverted matrix
    bool invertible;
    TQWMatrix m;
    if ( testf(VxF) ) {
	m.translate( vx, vy );
	m.scale( 1.0*vw/ww, 1.0*vh/wh );
	m.translate( -wx, -wy );
    }
    if ( testf(WxF) ) {
	if ( testf(VxF) )
	    m = wxmat * m;
	else
	    m = wxmat;
    }
    ixmat = m.invert( &invertible );		// invert matrix
}

#else
void TQPainter::resetXForm()
{
    xlatex = 0;
    xlatey = 0;
    clearf( VxF );
}
#endif // TQT_NO_TRANSFORMATIONS


extern bool tqt_old_transformations;

/*!
  \internal
  Maps a point from logical coordinates to device coordinates.
*/

void TQPainter::map( int x, int y, int *rx, int *ry ) const
{
#ifndef TQT_NO_TRANSFORMATIONS
    if ( tqt_old_transformations ) {
	switch ( txop ) {
	    case TxNone:
		*rx = x;  *ry = y;
		break;
	    case TxTranslate:
		// #### "Why no rounding here?", Warwick asked of Haavard.
		*rx = int(x + dx());
		*ry = int(y + dy());
		break;
	    case TxScale: {
		double tx = m11()*x + dx();
		double ty = m22()*y + dy();
		*rx = tx >= 0 ? int(tx + 0.5) : int(tx - 0.5);
		*ry = ty >= 0 ? int(ty + 0.5) : int(ty - 0.5);
	    } break;
	    default: {
		double tx = m11()*x + m21()*y+dx();
		double ty = m12()*x + m22()*y+dy();
		*rx = tx >= 0 ? int(tx + 0.5) : int(tx - 0.5);
		*ry = ty >= 0 ? int(ty + 0.5) : int(ty - 0.5);
	    } break;
	}
    } else {
	switch ( txop ) {
	    case TxNone:
		*rx = x;
		*ry = y;
		break;
	    case TxTranslate:
		*rx = tqRound( x + dx() );
		*ry = tqRound( y + dy() );
		break;
	    case TxScale:
		*rx = tqRound( m11()*x + dx() );
		*ry = tqRound( m22()*y + dy() );
		break;
	    default:
		*rx = tqRound( m11()*x + m21()*y+dx() );
		*ry = tqRound( m12()*x + m22()*y+dy() );
		break;
	}
    }
#else
    *rx = x + xlatex;
    *ry = y + xlatey;
#endif
}

/*!
  \internal
  Maps a rectangle from logical coordinates to device coordinates.
  This internal function does not handle rotation and/or shear.
*/

void TQPainter::map( int x, int y, int w, int h,
		    int *rx, int *ry, int *rw, int *rh ) const
{
#ifndef TQT_NO_TRANSFORMATIONS
    if ( tqt_old_transformations ) {
	switch ( txop ) {
	    case TxNone:
		*rx = x;  *ry = y;
		*rw = w;  *rh = h;
		break;
	    case TxTranslate:
		// #### "Why no rounding here?", Warwick asked of Haavard.
		*rx = int(x + dx());
		*ry = int(y + dy());
		*rw = w;  *rh = h;
		break;
	    case TxScale: {
		double tx1 = m11()*x + dx();
		double ty1 = m22()*y + dy();
		double tx2 = m11()*(x + w - 1) + dx();
		double ty2 = m22()*(y + h - 1) + dy();
		*rx = tqRound( tx1 );
		*ry = tqRound( ty1 );
		*rw = tqRound( tx2 ) - *rx + 1;
		*rh = tqRound( ty2 ) - *ry + 1;
	    } break;
	    default:
#if defined(QT_CHECK_STATE)
		tqWarning( "TQPainter::map: Internal error" );
#endif
		break;
	}
    } else {
	switch ( txop ) {
	    case TxNone:
		*rx = x;  *ry = y;
		*rw = w;  *rh = h;
		break;
	    case TxTranslate:
		*rx = tqRound(x + dx() );
		*ry = tqRound(y + dy() );
		*rw = w;  *rh = h;
		break;
	    case TxScale:
		*rx = tqRound( m11()*x + dx() );
		*ry = tqRound( m22()*y + dy() );
		*rw = tqRound( m11()*w );
		*rh = tqRound( m22()*h );
		break;
	    default:
#if defined(QT_CHECK_STATE)
		tqWarning( "TQPainter::map: Internal error" );
#endif
		break;
	}
    }
#else
    *rx = x + xlatex;
    *ry = y + xlatey;
    *rw = w;  *rh = h;
#endif
}

/*!
  \internal
  Maps a point from device coordinates to logical coordinates.
*/

void TQPainter::mapInv( int x, int y, int *rx, int *ry ) const
{
#ifndef TQT_NO_TRANSFORMATIONS
#if defined(QT_CHECK_STATE)
    if ( !txinv )
	tqWarning( "TQPainter::mapInv: Internal error" );
#endif
    if ( tqt_old_transformations ) {
	double tx = im11()*x + im21()*y+idx();
	double ty = im12()*x + im22()*y+idy();
	*rx = tx >= 0 ? int(tx + 0.5) : int(tx - 0.5);
	*ry = ty >= 0 ? int(ty + 0.5) : int(ty - 0.5);
    } else {
	*rx = tqRound( im11()*x + im21()*y + idx() );
	*ry = tqRound( im12()*x + im22()*y + idy() );
    }
#else
    *rx = x - xlatex;
    *ry = y - xlatey;
#endif
}

/*!
  \internal
  Maps a rectangle from device coordinates to logical coordinates.
  Cannot handle rotation and/or shear.
*/

void TQPainter::mapInv( int x, int y, int w, int h,
		       int *rx, int *ry, int *rw, int *rh ) const
{
#ifndef TQT_NO_TRANSFORMATIONS
#if defined(QT_CHECK_STATE)
    if ( !txinv || txop == TxRotShear )
	tqWarning( "TQPainter::mapInv: Internal error" );
#endif
    if ( tqt_old_transformations ) {
	double tx = im11()*x + idx();
	double ty = im22()*y + idy();
	double tw = im11()*w;
	double th = im22()*h;
	*rx = tx >= 0 ? int(tx + 0.5) : int(tx - 0.5);
	*ry = ty >= 0 ? int(ty + 0.5) : int(ty - 0.5);
	*rw = tw >= 0 ? int(tw + 0.5) : int(tw - 0.5);
	*rh = th >= 0 ? int(th + 0.5) : int(th - 0.5);
    } else {
	*rx = tqRound( im11()*x + idx() );
	*ry = tqRound( im22()*y + idy() );
	*rw = tqRound( im11()*w );
	*rh = tqRound( im22()*h );
    }
#else
    *rx = x - xlatex;
    *ry = y - xlatey;
    *rw = w;
    *rh = h;
#endif
}


/*!
    Returns the point \a pv transformed from model coordinates to
    device coordinates.

    \sa xFormDev(), TQWMatrix::map()
*/

TQPoint TQPainter::xForm( const TQPoint &pv ) const
{
#ifndef TQT_NO_TRANSFORMATIONS
    if ( txop == TxNone )
	return pv;
    int x=pv.x(), y=pv.y();
    map( x, y, &x, &y );
    return TQPoint( x, y );
#else
    return TQPoint( pv.x()+xlatex, pv.y()+xlatey );
#endif
}

/*!
    \overload

    Returns the rectangle \a rv transformed from model coordinates to
    device coordinates.

    If world transformation is enabled and rotation or shearing has
    been specified, then the bounding rectangle is returned.

    \sa xFormDev(), TQWMatrix::map()
*/

TQRect TQPainter::xForm( const TQRect &rv ) const
{
#ifndef TQT_NO_TRANSFORMATIONS
    if ( txop == TxNone )
	return rv;
    if ( txop == TxRotShear ) {			// rotation/shear
	return xmat.mapRect( rv );
    }
    // Just translation/scale
    int x, y, w, h;
    rv.rect( &x, &y, &w, &h );
    map( x, y, w, h, &x, &y, &w, &h );
    return TQRect( x, y, w, h );
#else
    return TQRect( rv.x()+xlatex, rv.y()+xlatey, rv.width(), rv.height() );
#endif
}

/*!
    \overload

    Returns the point array \a av transformed from model coordinates
    to device coordinates.

    \sa xFormDev(), TQWMatrix::map()
*/

TQPointArray TQPainter::xForm( const TQPointArray &av ) const
{
    TQPointArray a = av;
#ifndef TQT_NO_TRANSFORMATIONS
    if ( txop != TxNone )
    {
	return xmat * av;
    }
#else
    a.translate( xlatex, xlatey );
#endif
    return a;
}

/*!
    \overload

    Returns the point array \a av transformed from model coordinates
    to device coordinates. The \a index is the first point in the
    array and \a npoints denotes the number of points to be
    transformed. If \a npoints is negative, all points from \a
    av[index] until the last point in the array are transformed.

    The returned point array consists of the number of points that
    were transformed.

    Example:
    \code
	TQPointArray a(10);
	TQPointArray b;
	b = painter.xForm(a, 2, 4);  // b.size() == 4
	b = painter.xForm(a, 2, -1); // b.size() == 8
    \endcode

    \sa xFormDev(), TQWMatrix::map()
*/

TQPointArray TQPainter::xForm( const TQPointArray &av, int index,
			     int npoints ) const
{
    int lastPoint = npoints < 0 ? av.size() : index+npoints;
    TQPointArray a( lastPoint-index );
    memcpy( a.data(), av.data()+index, (lastPoint-index)*sizeof( TQPoint ) );
#ifndef TQT_NO_TRANSFORMATIONS
    return xmat*a;
#else
    a.translate( xlatex, xlatey );
    return a;
#endif
}

/*!
    \overload

    Returns the point \a pd transformed from device coordinates to
    model coordinates.

    \sa xForm(), TQWMatrix::map()
*/

TQPoint TQPainter::xFormDev( const TQPoint &pd ) const
{
#ifndef TQT_NO_TRANSFORMATIONS
    if ( txop == TxNone )
	return pd;
    if ( !txinv ) {
	TQPainter *that = (TQPainter*)this;	// mutable
	that->updateInvXForm();
    }
#endif
    int x=pd.x(), y=pd.y();
    mapInv( x, y, &x, &y );
    return TQPoint( x, y );
}

/*!
    Returns the rectangle \a rd transformed from device coordinates to
    model coordinates.

    If world transformation is enabled and rotation or shearing is
    used, then the bounding rectangle is returned.

    \sa xForm(), TQWMatrix::map()
*/

TQRect TQPainter::xFormDev( const TQRect &rd ) const
{
#ifndef TQT_NO_TRANSFORMATIONS
    if ( txop == TxNone )
	return rd;
    if ( !txinv ) {
	TQPainter *that = (TQPainter*)this;	// mutable
	that->updateInvXForm();
    }
    if ( txop == TxRotShear ) {			// rotation/shear
	return ixmat.mapRect( rd );
    }
#endif
    // Just translation/scale
    int x, y, w, h;
    rd.rect( &x, &y, &w, &h );
    mapInv( x, y, w, h, &x, &y, &w, &h );
    return TQRect( x, y, w, h );
}

/*!
    \overload

    Returns the point array \a ad transformed from device coordinates
    to model coordinates.

    \sa xForm(), TQWMatrix::map()
*/

TQPointArray TQPainter::xFormDev( const TQPointArray &ad ) const
{
#ifndef TQT_NO_TRANSFORMATIONS
    if ( txop == TxNone )
	return ad;
    if ( !txinv ) {
	TQPainter *that = (TQPainter*)this;	// mutable
	that->updateInvXForm();
    }
    return ixmat * ad;
#else
    // ###
    return ad;
#endif
}

/*!
    \overload

    Returns the point array \a ad transformed from device coordinates
    to model coordinates. The \a index is the first point in the array
    and \a npoints denotes the number of points to be transformed. If
    \a npoints is negative, all points from \a ad[index] until the
    last point in the array are transformed.

    The returned point array consists of the number of points that
    were transformed.

    Example:
    \code
	TQPointArray a(10);
	TQPointArray b;
	b = painter.xFormDev(a, 1, 3);  // b.size() == 3
	b = painter.xFormDev(a, 1, -1); // b.size() == 9
    \endcode

    \sa xForm(), TQWMatrix::map()
*/

TQPointArray TQPainter::xFormDev( const TQPointArray &ad, int index,
				int npoints ) const
{
    int lastPoint = npoints < 0 ? ad.size() : index+npoints;
    TQPointArray a( lastPoint-index );
    memcpy( a.data(), ad.data()+index, (lastPoint-index)*sizeof( TQPoint ) );
#ifndef TQT_NO_TRANSFORMATIONS
    if ( txop == TxNone )
	return a;
    if ( !txinv ) {
	TQPainter *that = (TQPainter*)this;	// mutable
	that->updateInvXForm();
    }
    return ixmat * a;
#else
    // ###
    return a;
#endif
}


/*!
    Fills the rectangle \a (x, y, w, h) with the \a brush.

    You can specify a TQColor as \a brush, since there is a TQBrush
    constructor that takes a TQColor argument and creates a solid
    pattern brush.

    \sa drawRect()
*/

void TQPainter::fillRect( int x, int y, int w, int h, const TQBrush &brush )
{
    TQPen   oldPen   = pen();			// save pen
    TQBrush oldBrush = this->brush();		// save brush
    setPen( NoPen );
    setBrush( brush );
    drawRect( x, y, w, h );			// draw filled rect
    setBrush( oldBrush );			// restore brush
    setPen( oldPen );				// restore pen
}


/*!
    \overload void TQPainter::setBrushOrigin( const TQPoint &p )

    Sets the brush origin to point \a p.
*/

/*!
    \overload void TQPainter::setWindow( const TQRect &r )

    Sets the painter's window to rectangle \a r.
*/


/*!
    \overload void TQPainter::setViewport( const TQRect &r )

    Sets the painter's viewport to rectangle \a r.
*/


/*!
    \fn bool TQPainter::hasClipping() const

    Returns true if clipping has been set; otherwise returns false.

    \sa setClipping()
*/

/*!
    Returns the currently set clip region. Note that the clip region
    is given in physical device coordinates and \e not subject to any
    \link coordsys.html coordinate transformation \endlink if \a m is
    equal to \c CoordDevice (the default). If \a m equals \c
    CoordPainter the returned region is in model coordinates.

    \sa setClipRegion(), setClipRect(), setClipping() TQPainter::CoordinateMode
*/
TQRegion TQPainter::clipRegion( CoordinateMode m ) const
{
    // ### FIXME in 4.0:
    // If the transformation mode is CoordPainter, we should transform the
    // clip region with painter transformations.

#ifndef TQT_NO_TRANSFORMATIONS
    TQRegion r;
    if ( m == CoordDevice ) {
	r = crgn;
    } else {
	if ( !txinv ) {
	    TQPainter *that = (TQPainter*)this;	// mutable
	    that->updateInvXForm();
	}

	r = ixmat * crgn;
    }
    return r;
#else
    return crgn;
#endif
}

/*!
    \fn void TQPainter::setClipRect( int x, int y, int w, int h, CoordinateMode m)

    Sets the clip region to the rectangle \a x, \a y, \a w, \a h and
    enables clipping. The clip mode is set to \a m.

    If \a m is \c CoordDevice (the default), the coordinates given for
    the clip region are taken to be physical device coordinates and
    are \e not subject to any \link coordsys.html coordinate
    transformations\endlink. If \a m is \c CoordPainter, the
    coordinates given for the clip region are taken to be model
    coordinates.

    \sa setClipRegion(), clipRegion(), setClipping() TQPainter::CoordinateMode
*/

/*!
    \overload void TQPainter::drawPoint( const TQPoint &p )

    Draws the point \a p.
*/


/*!
    \overload void TQPainter::moveTo( const TQPoint &p )

    Moves to the point \a p.
*/

/*!
    \overload void TQPainter::lineTo( const TQPoint &p )

    Draws a line to the point \a p.
*/

/*!
    \overload void TQPainter::drawLine( const TQPoint &p1, const TQPoint &p2 )

    Draws a line from point \a p1 to point \a p2.
*/

/*!
    \overload void TQPainter::drawRect( const TQRect &r )

    Draws the rectangle \a r.
*/

/*!
    \overload void TQPainter::drawWinFocusRect( const TQRect &r )

    Draws rectangle \a r as a window focus rectangle.
*/

/*!
    \overload void TQPainter::drawWinFocusRect( const TQRect &r, const TQColor &bgColor )

    Draws rectangle \a r as a window focus rectangle using background
    color \a bgColor.
*/


#if !defined(TQ_WS_X11) && !defined(TQ_WS_QWS) && !defined(TQ_WS_MAC)
// The doc and X implementation of this functions is in qpainter_x11.cpp
void TQPainter::drawWinFocusRect( int, int, int, int,
				 bool, const TQColor & )
{
    // do nothing, only called from X11 specific functions
}
#endif


/*!
    \overload void TQPainter::drawRoundRect( const TQRect &r, int xRnd, int yRnd )

    Draws a rounded rectangle \a r, rounding to the x position \a xRnd
    and the y position \a yRnd on each corner.
*/

/*!
    \overload void TQPainter::drawEllipse( const TQRect &r )

    Draws the ellipse that fits inside rectangle \a r.
*/

/*!
    \overload void TQPainter::drawArc( const TQRect &r, int a, int alen )

    Draws the arc that fits inside the rectangle \a r with start angle
    \a a and arc length \a alen.
*/

/*!
    \overload void TQPainter::drawPie( const TQRect &r, int a, int alen )

    Draws a pie segment that fits inside the rectangle \a r with start
    angle \a a and arc length \a alen.
*/

/*!
    \overload void TQPainter::drawChord( const TQRect &r, int a, int alen )

    Draws a chord that fits inside the rectangle \a r with start angle
    \a a and arc length \a alen.
*/

/*!
    \overload void TQPainter::drawPixmap( const TQPoint &p, const TQPixmap &pm, const TQRect &sr )

    Draws the rectangle \a sr of pixmap \a pm with its origin at point
    \a p.
*/

/*!
    \overload void TQPainter::drawPixmap( const TQPoint &p, const TQPixmap &pm )

    Draws the pixmap \a pm with its origin at point \a p.
*/

void TQPainter::drawPixmap( const TQPoint &p, const TQPixmap &pm )
{
    drawPixmap( p.x(), p.y(), pm, 0, 0, pm.width(), pm.height() );
}

#if !defined(TQT_NO_IMAGE_SMOOTHSCALE) || !defined(TQT_NO_PIXMAP_TRANSFORMATION)

/*!
    \overload

    Draws the pixmap \a pm into the rectangle \a r. The pixmap is
    scaled to fit the rectangle, if image and rectangle size disagree.
*/
void TQPainter::drawPixmap( const TQRect &r, const TQPixmap &pm )
{
    int rw = r.width();
    int rh = r.height();
    int iw= pm.width();
    int ih = pm.height();
    if ( rw <= 0 || rh <= 0 || iw <= 0 || ih <= 0 )
	return;
    bool scale = ( rw != iw || rh != ih );
    float scaleX = (float)rw/(float)iw;
    float scaleY = (float)rh/(float)ih;
    bool smooth = ( scaleX < 1.5 || scaleY < 1.5 );

    if ( testf(ExtDev) ) {
	TQPDevCmdParam param[2];
	param[0].rect = &r;
	param[1].pixmap = &pm;
#if defined(TQ_WS_WIN)
	if ( !pdev->cmd( TQPaintDevice::PdcDrawPixmap, this, param ) || !hdc )
	    return;
#elif defined(TQ_WS_QWS)
	pdev->cmd( TQPaintDevice::PdcDrawPixmap, this, param );
	return;
#elif defined(TQ_WS_MAC)
	if ( !pdev->cmd( TQPaintDevice::PdcDrawPixmap, this, param ) || !pdev->handle())
	    return;
#else
	if ( !pdev->cmd( TQPaintDevice::PdcDrawPixmap, this, param ) || !hd )
	    return;
#endif
    }

    TQPixmap pixmap = pm;

    if ( scale ) {
#ifndef TQT_NO_IMAGE_SMOOTHSCALE
# ifndef TQT_NO_PIXMAP_TRANSFORMATION
	if ( smooth )
# endif
	{
	    TQImage i = pm.convertToImage();
	    pixmap = TQPixmap( i.smoothScale( rw, rh ) );
	}
# ifndef TQT_NO_PIXMAP_TRANSFORMATION
	else
# endif
#endif
#ifndef TQT_NO_PIXMAP_TRANSFORMATION
	{
	    pixmap = pm.xForm( TQWMatrix( scaleX, 0, 0, scaleY, 0, 0 ) );
	}
#endif
    }
    drawPixmap( r.x(), r.y(), pixmap );
}

#endif

/*!
    \overload void TQPainter::drawImage( const TQPoint &, const TQImage &, const TQRect &sr, int conversionFlags = 0 );

    Draws the rectangle \a sr from the image at the given point.
*/

/*
    Draws at point \a p the \sr rect from image \a pm, using \a
    conversionFlags if the image needs to be converted to a pixmap.
    The default value for \a conversionFlags is 0; see
    convertFromImage() for information about what other values do.

  This function may convert \a image to a pixmap and then draw it, if
  device() is a TQPixmap or a TQWidget, or else draw it directly, if
  device() is a TQPrinter or TQPicture.
*/

/*!
    Draws at (\a x, \a y) the \a sw by \a sh area of pixels from (\a
    sx, \a sy) in \a image, using \a conversionFlags if the image
    needs to be converted to a pixmap. The default value for \a
    conversionFlags is 0; see convertFromImage() for information about
    what other values do.

    This function may convert \a image to a pixmap and then draw it,
    if device() is a TQPixmap or a TQWidget, or else draw it directly,
    if device() is a TQPrinter or TQPicture.

    Currently alpha masks of the image are ignored when painting on a TQPrinter.

    \sa drawPixmap() TQPixmap::convertFromImage()
*/
void TQPainter::drawImage( int x, int y, const TQImage & image,
			  int sx, int sy, int sw, int sh,
			  int conversionFlags )
{
#ifdef TQ_WS_QWS
    //### Hackish
# ifndef TQT_NO_TRANSFORMATIONS
    if ( !image.isNull() && gfx &&
	(txop==TxNone||txop==TxTranslate) && !testf(ExtDev) )
# else
    if ( !image.isNull() && gfx && !testf(ExtDev) )
# endif
    {
	if(sw<0)
	    sw=image.width();
	if(sh<0)
	    sh=image.height();

	TQImage image2 = qt_screen->mapToDevice( image );

	// This is a bit dubious
	if(image2.depth()==1) {
	    image2.setNumColors( 2 );
	    image2.setColor( 0, tqRgb(255,255,255) );
	    image2.setColor( 1, tqRgb(0,0,0) );
	}
	if ( image2.hasAlphaBuffer() )
	    gfx->setAlphaType(TQGfx::InlineAlpha);
	else
	    gfx->setAlphaType(TQGfx::IgnoreAlpha);
	gfx->setSource(&image2);
	if ( testf(VxF|WxF) ) {
	    map( x, y, &x, &y );
	}
	gfx->blt(x,y,sw,sh,sx,sy);
	return;
    }
#endif

    if ( !isActive() || image.isNull() )
	return;

    // right/bottom
    if ( sw < 0 )
	sw = image.width()  - sx;
    if ( sh < 0 )
	sh = image.height() - sy;

    // Sanity-check clipping
    if ( sx < 0 ) {
	x -= sx;
	sw += sx;
	sx = 0;
    }
    if ( sw + sx > image.width() )
	sw = image.width() - sx;
    if ( sy < 0 ) {
	y -= sy;
	sh += sy;
	sy = 0;
    }
    if ( sh + sy > image.height() )
	sh = image.height() - sy;

    if ( sw <= 0 || sh <= 0 )
	return;

    bool all = image.rect().intersect(TQRect(sx,sy,sw,sh)) == image.rect();
    TQImage subimage = all ? image : image.copy(sx,sy,sw,sh);

    if ( testf(ExtDev) ) {
	TQPDevCmdParam param[2];
	TQRect r( x, y, subimage.width(), subimage.height() );
	param[0].rect = &r;
	param[1].image = &subimage;
#if defined(TQ_WS_WIN)
	if ( !pdev->cmd( TQPaintDevice::PdcDrawImage, this, param ) || !hdc )
	    return;
#elif defined (TQ_WS_QWS)
	pdev->cmd( TQPaintDevice::PdcDrawImage, this, param );
	return;
#elif defined(TQ_WS_MAC)
	if(!pdev->cmd( TQPaintDevice::PdcDrawImage, this, param ) || !pdev->handle() )
	    return;
#else
	if ( !pdev->cmd( TQPaintDevice::PdcDrawImage, this, param ) || !hd )
	    return;
#endif
    }

    TQPixmap pm;
    pm.convertFromImage( subimage, conversionFlags );
    drawPixmap( x, y, pm );
}

/*!
    \overload void TQPainter::drawImage( const TQPoint &p, const TQImage &i, int conversion_flags )

    Draws the image \a i at point \a p.

    If the image needs to be modified to fit in a lower-resolution
    result (e.g. converting from 32-bit to 8-bit), use the \a
    conversion_flags to specify how you'd prefer this to happen.

    \sa TQt::ImageConversionFlags
*/
void TQPainter::drawImage( const TQPoint & p, const TQImage & i,
			  int conversion_flags )
{
    drawImage(p, i, i.rect(), conversion_flags);
}

#if !defined(TQT_NO_IMAGE_TRANSFORMATION) || !defined(TQT_NO_IMAGE_SMOOTHSCALE)

/*!
    \overload

    Draws the image \a i into the rectangle \a r. The image will be
    scaled to fit the rectangle if image and rectangle dimensions
    differ.
*/
void TQPainter::drawImage( const TQRect &r, const TQImage &i )
{
    int rw = r.width();
    int rh = r.height();
    int iw= i.width();
    int ih = i.height();
    if ( rw <= 0 || rh <= 0 || iw <= 0 || ih <= 0 )
	return;

    if ( testf(ExtDev) ) {
	TQPDevCmdParam param[2];
	param[0].rect = &r;
	param[1].image = &i;
#if defined(TQ_WS_WIN)
	if ( !pdev->cmd( TQPaintDevice::PdcDrawImage, this, param ) || !hdc )
	    return;
#elif defined(TQ_WS_QWS)
	pdev->cmd( TQPaintDevice::PdcDrawImage, this, param );
	return;
#elif defined(TQ_WS_MAC)
	if ( !pdev->cmd( TQPaintDevice::PdcDrawImage, this, param ) || !pdev->handle() )
	    return;
#else
	if ( !pdev->cmd( TQPaintDevice::PdcDrawImage, this, param ) || !hd )
	    return;
#endif
    }


    bool scale = ( rw != iw || rh != ih );
    float scaleX = (float)rw/(float)iw;
    float scaleY = (float)rh/(float)ih;
    bool smooth = ( scaleX < 1.5 || scaleY < 1.5 );

    TQImage img = scale
	? (
#if defined(TQT_NO_IMAGE_TRANSFORMATION)
		i.smoothScale( rw, rh )
#elif defined(TQT_NO_IMAGE_SMOOTHSCALE)
		i.scale( rw, rh )
#else
		smooth ? i.smoothScale( rw, rh ) : i.scale( rw, rh )
#endif
	  )
	: i;

    drawImage( r.x(), r.y(), img );
}

#endif


void bitBlt( TQPaintDevice *dst, int dx, int dy,
	     const TQImage *src, int sx, int sy, int sw, int sh,
	     int conversion_flags )
{
    TQPixmap tmp;
    if ( sx == 0 && sy == 0
	&& (sw<0 || sw==src->width()) && (sh<0 || sh==src->height()) )
    {
	tmp.convertFromImage( *src, conversion_flags );
    } else {
	tmp.convertFromImage( src->copy( sx, sy, sw, sh, conversion_flags),
			      conversion_flags );
    }
    bitBlt( dst, dx, dy, &tmp );
}


/*!
    \overload void TQPainter::drawTiledPixmap( const TQRect &r, const TQPixmap &pm, const TQPoint &sp )

    Draws a tiled pixmap, \a pm, inside rectangle \a r with its origin
    at point \a sp.
*/

/*!
    \overload void TQPainter::drawTiledPixmap( const TQRect &r, const TQPixmap &pm )

    Draws a tiled pixmap, \a pm, inside rectangle \a r.
*/

/*!
    \overload void TQPainter::fillRect( const TQRect &r, const TQBrush &brush )

    Fills the rectangle \a r using brush \a brush.
*/

/*!
    \fn void TQPainter::eraseRect( int x, int y, int w, int h )

    Erases the area inside \a x, \a y, \a w, \a h. Equivalent to
    \c{fillRect( x, y, w, h, backgroundColor() )}.
*/

/*!
    \overload void TQPainter::eraseRect( const TQRect &r )

    Erases the area inside the rectangle \a r.
*/

/*!
    \fn TQPainter::drawText( int x, int y, const TQString &, int len = -1, TextDirection dir = Auto )

    \overload

    Draws the given text at position \a x, \a y. If \a len is -1 (the
    default) all the text is drawn, otherwise the first \a len
    characters are drawn. The text's direction is given by \a dir.

    \sa TQPainter::TextDirection
*/

/*!
    \fn void TQPainter::drawText( int x, int y, int w, int h, int flags,
			         const TQString&, int len = -1, TQRect *br=0,
			         TQTextParag **internal=0 )

    \overload

    Draws the given text within the rectangle starting at \a x, \a y,
    with width \a w and height \a h. If \a len is -1 (the default) all
    the text is drawn, otherwise the first \a len characters are
    drawn. The text's flags that are given in the \a flags parameter
    are \l{TQt::AlignmentFlags} and \l{TQt::TextFlags} OR'd together. \a
    br (if not null) is set to the actual bounding rectangle of the
    output. The \a internal parameter is for internal use only.
*/

/*!
    \fn void TQPainter::drawText( const TQPoint &, const TQString &, int len = -1, TextDirection dir = Auto );

    \overload

    Draws the text at the given point.

    \sa TQPainter::TextDirection
*/

/*
    Draws the text in \a s at point \a p. If \a len is -1 the entire
    string is drawn, otherwise just the first \a len characters. The
    text's direction is specified by \a dir.
*/


/*!
    \fn void TQPainter::drawText( int x, int y, const TQString &, int pos, int len, TextDirection dir = Auto );

    \overload

    Draws the text from position \a pos, at point \a (x, y). If \a len is
    -1 the entire string is drawn, otherwise just the first \a len
    characters. The text's direction is specified by \a dir.
*/

/*!
    \fn void TQPainter::drawText( const TQPoint &p, const TQString &, int pos, int len, TextDirection dir = Auto );

    Draws the text from position \a pos, at point \a p. If \a len is
    -1 the entire string is drawn, otherwise just the first \a len
    characters. The text's direction is specified by \a dir.

    Note that the meaning of \e y is not the same for the two
    drawText() varieties. For overloads that take a simple \e x, \e y
    pair (or a point), the \e y value is the text's baseline; for
    overloads that take a rectangle, \e rect.y() is the top of the
    rectangle and the text is aligned within that rectangle in
    accordance with the alignment flags.

    \sa TQPainter::TextDirection
*/

/*!
    \fn void TQPainter::drawTextItem(const TQPoint &, const TQTextItem &, int)
    \internal
*/

static inline void fix_neg_rect( int *x, int *y, int *w, int *h )
{
    if ( *w < 0 ) {
        *w = -*w + 2;
        *x -= *w - 1;
    }
    if ( *h < 0 ) {
	*h = -*h + 2;
	*y -= *h - 1;
    }
}
void TQPainter::fix_neg_rect( int *x, int *y, int *w, int *h )
{
    ::fix_neg_rect(x,y,w,h);
}

//
// The drawText function takes two special parameters; 'internal' and 'brect'.
//
// The 'internal' parameter contains a pointer to an array of encoded
// information that keeps internal geometry data.
// If the drawText function is called repeatedly to display the same text,
// it makes sense to calculate text width and linebreaks the first time,
// and use these parameters later to print the text because we save a lot of
// CPU time.
// The 'internal' parameter will not be used if it is a null pointer.
// The 'internal' parameter will be generated if it is not null, but points
// to a null pointer, i.e. internal != 0 && *internal == 0.
// The 'internal' parameter will be used if it contains a non-null pointer.
//
// If the 'brect parameter is a non-null pointer, then the bounding rectangle
// of the text will be returned in 'brect'.
//

/*!
    \overload

    Draws at most \a len characters from \a str in the rectangle \a r.

    This function draws formatted text. The \a tf text format is
    really of type \l TQt::AlignmentFlags and \l TQt::TextFlags OR'd
    together.

    Horizontal alignment defaults to AlignAuto and vertical alignment
    defaults to AlignTop.

    \a brect (if not null) is set to the actual bounding rectangle of
    the output. \a internal is, yes, internal.

    \sa boundingRect()
*/

void TQPainter::drawText( const TQRect &r, int tf,
			 const TQString& str, int len, TQRect *brect,
			 TQTextParag **internal )
{
    if ( !isActive() )
	return;
    if ( len < 0 )
	len = str.length();
    if ( len == 0 )				// empty string
	return;

    if ( testf(DirtyFont|ExtDev) ) {
	if ( testf(DirtyFont) )
	    updateFont();
	if ( testf(ExtDev) && (tf & DontPrint) == 0 ) {
	    TQPDevCmdParam param[3];
	    TQString newstr = str;
	    newstr.truncate( len );
	    param[0].rect = &r;
	    param[1].ival = tf;
	    param[2].str = &newstr;
	    if ( pdev->devType() != TQInternal::Printer ) {
#if defined(TQ_WS_WIN)
		if ( !pdev->cmd( TQPaintDevice::PdcDrawText2Formatted,
				 this, param) ||
		     !hdc )
		    return;			// TQPrinter wants PdcDrawText2
#elif defined(TQ_WS_QWS)
		pdev->cmd( TQPaintDevice::PdcDrawText2Formatted, this, param);
		return;
#elif defined(TQ_WS_MAC)
		if ( !pdev->cmd( TQPaintDevice::PdcDrawText2Formatted, this, param) ||
		    !pdev->handle())
		    return;			// TQPrinter wants PdcDrawText2
#else
		if ( !pdev->cmd( TQPaintDevice::PdcDrawText2Formatted,
				 this, param) ||
		     !hd )
		    return;			// TQPrinter wants PdcDrawText2
#endif
	    }
	}
    }

    qt_format_text(font(), r, tf, str, len, brect,
		   tabstops, tabarray, tabarraylen, internal, this);
}

//#define QT_FORMAT_TEXT_DEBUG

#define TQChar_linesep TQChar(0x2028U)

void qt_format_text( const TQFont& font, const TQRect &_r,
		     int tf, const TQString& str, int len, TQRect *brect,
		     int tabstops, int* tabarray, int tabarraylen,
		     TQTextParag **, TQPainter* painter )
{
    // we need to copy r here to protect against the case (&r == brect).
    TQRect r( _r );

    bool dontclip  = (tf & TQt::DontClip)  == TQt::DontClip;
    bool wordbreak  = (tf & TQt::WordBreak)  == TQt::WordBreak;
    bool singleline = (tf & TQt::SingleLine) == TQt::SingleLine;
    bool showprefix = (tf & TQt::ShowPrefix) == TQt::ShowPrefix;
    bool noaccel = ( tf & TQt::NoAccel ) == TQt::NoAccel;

    bool isRightToLeft = str.isRightToLeft();
    if ( ( tf & TQt::AlignHorizontal_Mask ) == TQt::AlignAuto )
	tf |= isRightToLeft ? TQt::AlignRight : TQt::AlignLeft;

    bool expandtabs = ( (tf & TQt::ExpandTabs) &&
			( ( (tf & TQt::AlignLeft) && !isRightToLeft ) ||
			  ( (tf & TQt::AlignRight) && isRightToLeft ) ) );

    if ( !painter )
	tf |= TQt::DontPrint;

    int maxUnderlines = 0;
    int numUnderlines = 0;
    int underlinePositionStack[32];
    int *underlinePositions = underlinePositionStack;

    TQFont fnt(painter ? (painter->pfont ? *painter->pfont : painter->cfont) : font);
    TQFontMetrics fm( fnt );

    TQString text = str;
    // str.setLength() always does a deep copy, so the replacement
    // code below is safe.
    text.setLength( len );
    // compatible behaviour to the old implementation. Replace
    // tabs by spaces
    TQChar *chr = (TQChar*)text.unicode();
    const TQChar *end = chr + len;
    bool haveLineSep = false;
    while ( chr != end ) {
	if ( *chr == '\r' || ( singleline && *chr == '\n' ) ) {
	    *chr = ' ';
	} else if ( *chr == '\n' ) {
	    *chr = TQChar_linesep;
	    haveLineSep = true;
	} else if ( *chr == '&' ) {
	    ++maxUnderlines;
	}
	++chr;
    }
    if ( !expandtabs ) {
	chr = (TQChar*)text.unicode();
	while ( chr != end ) {
	    if ( *chr == '\t' )
		*chr = ' ';
	    ++chr;
	}
    } else if (!tabarraylen && !tabstops) {
	tabstops = fm.width('x')*8;
    }

    if ( noaccel || showprefix ) {
	if ( maxUnderlines > 32 )
	    underlinePositions = new int[maxUnderlines];
	TQChar *cout = (TQChar*)text.unicode();
	TQChar *cin = cout;
	int l = len;
	while ( l ) {
	    if ( *cin == '&' ) {
		++cin;
		--l;
		if ( !l )
		    break;
		if ( *cin != '&' )
		    underlinePositions[numUnderlines++] = cout - text.unicode();
	    }
	    *cout = *cin;
	    ++cout;
	    ++cin;
	    --l;
	}
	uint newlen = cout - text.unicode();
	if ( newlen != text.length())
	    text.setLength( newlen );
    }

    // no need to do extra work for underlines if we don't paint
    if ( tf & TQt::DontPrint )
	numUnderlines = 0;

    int height = 0;
    int left = r.width();
    int right = 0;

    TQTextLayout textLayout( text, fnt );
    int rb = TQMAX( 0, -fm.minRightBearing() );
    int lb = TQMAX( 0, -fm.minLeftBearing() );

    if ( text.isEmpty() ) {
	height = fm.height();
	left = right = 0;
	tf |= TQPainter::DontPrint;
    } else {
	textLayout.beginLayout((haveLineSep || expandtabs || wordbreak) ?
			       TQTextLayout::MultiLine :
			       (tf & TQt::DontPrint) ? TQTextLayout::NoBidi : TQTextLayout::SingleLine );

	// break underline chars into items of their own
	for( int i = 0; i < numUnderlines; i++ ) {
	    textLayout.setBoundary( underlinePositions[i] );
	    textLayout.setBoundary( underlinePositions[i]+1 );
	}

	int lineWidth = wordbreak ? TQMAX(0, r.width()-rb-lb) : INT_MAX;
	if(!wordbreak)
	    tf |= TQt::IncludeTrailingSpaces;

	int leading = fm.leading();
	int asc = fm.ascent();
	int desc = fm.descent();
	height = -leading;

	//tqDebug("beginLayout: lw = %d, rectwidth=%d", lineWidth , r.width());
	while ( !textLayout.atEnd() ) {
	    height += leading;
	    textLayout.beginLine( lineWidth );
	    //tqDebug("-----beginLine( %d )-----",  lineWidth );
	    bool linesep = false;
	    while ( 1 ) {
		TQTextItem ti = textLayout.currentItem();
 		//tqDebug("item: from=%d, ch=%x", ti.from(), text.unicode()[ti.from()].unicode() );
		if ( expandtabs && ti.isTab() ) {
		    int tw = 0;
		    int x = textLayout.widthUsed();
		    if ( tabarraylen ) {
// 			tqDebug("tabarraylen=%d", tabarraylen );
			int tab = 0;
			while ( tab < tabarraylen ) {
			    if ( tabarray[tab] > x ) {
				tw = tabarray[tab] - x;
				break;
			    }
			    ++tab;
			}
		    } else {
			tw = tabstops - (x % tabstops);
		    }
 		    //tqDebug("tw = %d",  tw );
		    if ( tw )
			ti.setWidth( tw );
		}
		if ( ti.isObject() && text.unicode()[ti.from()] == TQChar_linesep )
		    linesep = true;

		if ( linesep || textLayout.addCurrentItem() != TQTextLayout::Ok || textLayout.atEnd() )
		    break;
	    }

	    int ascent = asc, descent = desc, lineLeft, lineRight;
	    textLayout.setLineWidth( r.width()-rb-lb );
	    textLayout.endLine( 0, height, tf, &ascent, &descent,
				&lineLeft, &lineRight );
	    //tqDebug("finalizing line: lw=%d ascent = %d, descent=%d lineleft=%d lineright=%d", lineWidth, ascent, descent,lineLeft, lineRight  );
	    left = TQMIN( left, lineLeft );
	    right = TQMAX( right, lineRight );
	    height += ascent + descent + 1;
	    if ( linesep )
		textLayout.nextItem();
	}
    }

    int yoff = 0;
    if ( tf & TQt::AlignBottom )
	yoff = r.height() - height;
    else if ( tf & TQt::AlignVCenter )
	yoff = (r.height() - height)/2;

    if ( brect ) {
	*brect = TQRect( r.x() + left, r.y() + yoff, right-left + lb+rb, height );
	//tqDebug("br = %d %d %d/%d, left=%d, right=%d", brect->x(), brect->y(), brect->width(), brect->height(), left, right);
    }

    if (!(tf & TQPainter::DontPrint)) {
	bool restoreClipping = false;
	bool painterHasClip = false;
	TQRegion painterClipRegion;
	if ( !dontclip ) {
#ifndef TQT_NO_TRANSFORMATIONS
	    TQRegion reg = painter->xmat * r;
#else
	    TQRegion reg = r;
	    reg.translate( painter->xlatex, painter->xlatey );
#endif
	    if ( painter->hasClipping() )
		reg &= painter->clipRegion();

	    painterHasClip = painter->hasClipping();
	    painterClipRegion = painter->clipRegion();
	    restoreClipping = true;
	    painter->setClipRegion( reg );
	} else {
	    if ( painter->hasClipping() ){
		painterHasClip = painter->hasClipping();
		painterClipRegion = painter->clipRegion();
		restoreClipping = true;
		painter->setClipping( false );
	    }
	}

	int cUlChar = 0;
	int _tf = 0;
	if (fnt.underline()) _tf |= TQt::Underline;
	if (fnt.overline()) _tf |= TQt::Overline;
	if (fnt.strikeOut()) _tf |= TQt::StrikeOut;

	//tqDebug("have %d items",textLayout.numItems());
	for ( int i = 0; i < textLayout.numItems(); i++ ) {
	    TQTextItem ti = textLayout.itemAt( i );
 	    //tqDebug("Item %d: from=%d,  length=%d,  space=%d x=%d", i, ti.from(),  ti.length(), ti.isSpace(), ti.x() );
	    if ( ti.isTab() || ti.isObject() )
		continue;
	    int textFlags = _tf;
	    if ( !noaccel && numUnderlines > cUlChar && ti.from() == underlinePositions[cUlChar] ) {
		textFlags |= TQt::Underline;
		cUlChar++;
	    }
#if defined(TQ_WS_X11) || defined(TQ_WS_QWS)
	    if ( painter->bg_mode == TQt::OpaqueMode ) {
                int h = ti.ascent() + ti.descent() + 1;
                if (ti.y() + h < height)
                    // don't add leading to last line
                    h += fm.leading();
		qt_draw_background( painter, r.x()+lb + ti.x(), r.y() + yoff + ti.y() - ti.ascent(),
				    ti.width(), h);
            }
#endif
	    painter->drawTextItem( r.x()+lb, r.y() + yoff, ti, textFlags );
	}

	if ( restoreClipping ) {
	    painter->setClipRegion( painterClipRegion );
	    painter->setClipping( painterHasClip );
	}
    }

    if ( underlinePositions != underlinePositionStack )
	delete [] underlinePositions;
}

/*!
    \overload

    Returns the bounding rectangle of the aligned text that would be
    printed with the corresponding drawText() function using the first
    \a len characters from \a str if \a len is > -1, or the whole of
    \a str if \a len is -1. The drawing, and hence the bounding
    rectangle, is constrained to the rectangle \a r, or to the
    rectangle required to draw the text, whichever is the larger.

    The \a internal parameter should not be used.

    \sa drawText(), fontMetrics(), TQFontMetrics::boundingRect(), TQt::TextFlags
*/

TQRect TQPainter::boundingRect( const TQRect &r, int flags,
			      const TQString& str, int len, TQTextParag **internal )
{
    TQRect brect;
    if ( str.isEmpty() )
	brect.setRect( r.x(),r.y(), 0,0 );
    else
	drawText( r, flags | DontPrint, str, len, &brect, internal );
    return brect;
}

/*!
    \fn TQRect TQPainter::boundingRect( int x, int y, int w, int h, int flags, const TQString&, int len = -1, TQTextParag **intern=0 );

    Returns the bounding rectangle of the aligned text that would be
    printed with the corresponding drawText() function using the first
    \a len characters of the string if \a len is > -1, or the whole of
    the string if \a len is -1. The drawing, and hence the bounding
    rectangle, is constrained to the rectangle that begins at point \a
    (x, y) with width \a w and hight \a h, or to the
    rectangle required to draw the text, whichever is the larger.

    The \a flags argument is
    the bitwise OR of the following flags:
    \table
    \header \i Flag \i Meaning
    \row \i \c AlignAuto \i aligns according to the language, usually left.
    \row \i \c AlignLeft \i aligns to the left border.
    \row \i \c AlignRight \i aligns to the right border.
    \row \i \c AlignHCenter \i aligns horizontally centered.
    \row \i \c AlignTop \i aligns to the top border.
    \row \i \c AlignBottom \i aligns to the bottom border.
    \row \i \c AlignVCenter \i aligns vertically centered.
    \row \i \c AlignCenter \i (== \c AlignHCenter | \c AlignVCenter).
    \row \i \c SingleLine \i ignores newline characters in the text.
    \row \i \c ExpandTabs \i expands tabs.
    \row \i \c ShowPrefix \i interprets "&x" as "<u>x</u>".
    \row \i \c WordBreak \i breaks the text to fit the rectangle.
    \endtable

    Horizontal alignment defaults to \c AlignLeft and vertical
    alignment defaults to \c AlignTop.

    If several of the horizontal or several of the vertical alignment flags
    are set, the resulting alignment is undefined.

    The \a intern parameter should not be used.

    \sa TQt::TextFlags
*/



/*****************************************************************************
  TQPen member functions
 *****************************************************************************/

/*!
    \class TQPen ntqpen.h
    \brief The TQPen class defines how a TQPainter should draw lines and outlines
    of shapes.

    \ingroup graphics
    \ingroup images
    \ingroup shared
    \mainclass

    A pen has a style, width, color, cap style and join style.

    The pen style defines the line type. The default pen style is \c
    TQt::SolidLine. Setting the style to \c NoPen tells the painter to
    not draw lines or outlines.

    When drawing 1 pixel wide diagonal lines you can either use a very
    fast algorithm (specified by a line width of 0, which is the
    default), or a slower but more accurate algorithm (specified by a
    line width of 1). For horizontal and vertical lines a line width
    of 0 is the same as a line width of 1. The cap and join style have
    no effect on 0-width lines.

    The pen color defines the color of lines and text. The default
    line color is black. The TQColor documentation lists predefined
    colors.

    The cap style defines how the end points of lines are drawn. The
    join style defines how the joins between two lines are drawn when
    multiple connected lines are drawn (TQPainter::drawPolyline()
    etc.). The cap and join styles only apply to wide lines, i.e. when
    the width is 1 or greater.

    Use the TQBrush class to specify fill styles.

    Example:
    \code
    TQPainter painter;
    TQPen     pen( red, 2 );             // red solid line, 2 pixels wide
    painter.begin( &anyPaintDevice );   // paint something
    painter.setPen( pen );              // set the red, wide pen
    painter.drawRect( 40,30, 200,100 ); // draw a rectangle
    painter.setPen( blue );             // set blue pen, 0 pixel width
    painter.drawLine( 40,30, 240,130 ); // draw a diagonal in rectangle
    painter.end();                      // painting done
    \endcode

    See the \l TQt::PenStyle enum type for a complete list of pen
    styles.

    With reference to the end points of lines, for wide (non-0-width)
    pens it depends on the cap style whether the end point is drawn or
    not. TQPainter will try to make sure that the end point is drawn
    for 0-width pens, but this cannot be absolutely guaranteed because
    the underlying drawing engine is free to use any (typically
    accelerated) algorithm for drawing 0-width lines. On all tested
    systems, however, the end point of at least all non-diagonal lines
    are drawn.

    A pen's color(), width(), style(), capStyle() and joinStyle() can
    be set in the constructor or later with setColor(), setWidth(),
    setStyle(), setCapStyle() and setJoinStyle(). Pens may also be
    compared and streamed.

    \img pen-styles.png Pen styles

    \sa TQPainter, TQPainter::setPen()
*/


/*!
  \internal
  Initializes the pen.
*/

void TQPen::init( const TQColor &color, uint width, uint linestyle )
{
    data = new TQPenData;
    TQ_CHECK_PTR( data );
    data->style = (PenStyle)(linestyle & MPenStyle);
    data->width = width;
    data->color = color;
    data->linest = linestyle;
}

/*!
    Constructs a default black solid line pen with 0 width, which
    renders lines 1 pixel wide (fast diagonals).
*/

TQPen::TQPen()
{
    init( TQt::black, 0, SolidLine );		// default pen
}

/*!
    Constructs a black pen with 0 width (fast diagonals) and style \a
    style.

    \sa setStyle()
*/

TQPen::TQPen( PenStyle style )
{
    init( TQt::black, 0, style );
}

/*!
    Constructs a pen with the specified \a color, \a width and \a
    style.

    \sa setWidth(), setStyle(), setColor()
*/

TQPen::TQPen( const TQColor &color, uint width, PenStyle style )
{
    init( color, width, style );
}

/*!
    Constructs a pen with the specified color \a cl and width \a w.
    The pen style is set to \a s, the pen cap style to \a c and the
    pen join style to \a j.

    A line width of 0 will produce a 1 pixel wide line using a fast
    algorithm for diagonals. A line width of 1 will also produce a 1
    pixel wide line, but uses a slower more accurate algorithm for
    diagonals. For horizontal and vertical lines a line width of 0 is
    the same as a line width of 1. The cap and join style have no
    effect on 0-width lines.

    \sa setWidth(), setStyle(), setColor()
*/

TQPen::TQPen( const TQColor &cl, uint w, PenStyle s, PenCapStyle c,
	    PenJoinStyle j )
{
    init( cl, w, s | c | j );
}

/*!
    Constructs a pen that is a copy of \a p.
*/

TQPen::TQPen( const TQPen &p )
{
    data = p.data;
    data->ref();
}

/*!
    Destroys the pen.
*/

TQPen::~TQPen()
{
    if ( data->deref() )
	delete data;
}


/*!
    Detaches from shared pen data to make sure that this pen is the
    only one referring the data.

    If multiple pens share common data, this pen dereferences the data
    and gets a copy of the data. Nothing is done if there is just a
    single reference.
*/

void TQPen::detach()
{
    if ( data->count != 1 )
	*this = copy();
}


/*!
    Assigns \a p to this pen and returns a reference to this pen.
*/

TQPen &TQPen::operator=( const TQPen &p )
{
    p.data->ref();
    if ( data->deref() )
	delete data;
    data = p.data;
    return *this;
}


/*!
    Returns a \link shclass.html deep copy\endlink of the pen.
*/

TQPen TQPen::copy() const
{
    TQPen p( data->color, data->width, data->style, capStyle(), joinStyle() );
    return p;
}


/*!
    \fn PenStyle TQPen::style() const

    Returns the pen style.

    \sa setStyle()
*/

/*!
    Sets the pen style to \a s.

    See the \l TQt::PenStyle documentation for a list of all the
    styles.

    \warning On Mac OS X the style setting (other than \c NoPen and \c
    SolidLine) have no effect as they are not implemented by the
    underlying system.

    \warning On Windows 95/98, the style setting (other than \c NoPen
    and \c SolidLine) has no effect for lines with width greater than
    1.

    \sa style()
*/

void TQPen::setStyle( PenStyle s )
{
    if ( data->style == s )
	return;
    detach();
    data->style = s;
    data->linest = (data->linest & ~MPenStyle) | s;
}


/*!
    \fn uint TQPen::width() const

    Returns the pen width.

    \sa setWidth()
*/

/*!
    Sets the pen width to \a w.

    A line width of 0 will produce a 1 pixel wide line using a fast
    algorithm for diagonals. A line width of 1 will also produce a 1
    pixel wide line, but uses a slower more accurate algorithm for
    diagonals. For horizontal and vertical lines a line width of 0 is
    the same as a line width of 1. The cap and join style have no
    effect on 0-width lines.

    \sa width()
*/

void TQPen::setWidth( uint w )
{
    if ( data->width == w )
	return;
    detach();
    data->width = w;
}


/*!
    Returns the pen's cap style.

    \sa setCapStyle()
*/
TQt::PenCapStyle TQPen::capStyle() const
{
    return (PenCapStyle)(data->linest & MPenCapStyle);
}

/*!
    Sets the pen's cap style to \a c.

    The default value is \c FlatCap. The cap style has no effect on
    0-width pens.

    \img pen-cap-styles.png Pen Cap Styles

    \warning On Windows 95/98 and Macintosh, the cap style setting has
    no effect. Wide lines are rendered as if the cap style was \c
    SquareCap.

    \sa capStyle()
*/

void TQPen::setCapStyle( PenCapStyle c )
{
    if ( (data->linest & MPenCapStyle) == c )
	return;
    detach();
    data->linest = (data->linest & ~MPenCapStyle) | c;
}

/*!
    Returns the pen's join style.

    \sa setJoinStyle()
*/
TQt::PenJoinStyle TQPen::joinStyle() const
{
    return (PenJoinStyle)(data->linest & MPenJoinStyle);
}

/*!
    Sets the pen's join style to \a j.

    The default value is \c MiterJoin. The join style has no effect on
    0-width pens.

    \img pen-join-styles.png Pen Join Styles

    \warning On Windows 95/98 and Macintosh, the join style setting
    has no effect. Wide lines are rendered as if the join style was \c
    BevelJoin.

    \sa joinStyle()
*/

void TQPen::setJoinStyle( PenJoinStyle j )
{
    if ( (data->linest & MPenJoinStyle) == j )
	return;
    detach();
    data->linest = (data->linest & ~MPenJoinStyle) | j;
}

/*!
    \fn const TQColor &TQPen::color() const

    Returns the pen color.

    \sa setColor()
*/

/*!
    Sets the pen color to \a c.

    \sa color()
*/

void TQPen::setColor( const TQColor &c )
{
    detach();
    data->color = c;
}


/*!
    \fn bool TQPen::operator!=( const TQPen &p ) const

    Returns true if the pen is different from \a p; otherwise returns
    false.

    Two pens are different if they have different styles, widths or
    colors.

    \sa operator==()
*/

/*!
    Returns true if the pen is equal to \a p; otherwise returns false.

    Two pens are equal if they have equal styles, widths and colors.

    \sa operator!=()
*/

bool TQPen::operator==( const TQPen &p ) const
{
    return (p.data == data) || (p.data->linest == data->linest &&
	    p.data->width == data->width && p.data->color == data->color);
}


/*****************************************************************************
  TQPen stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
/*!
    \relates TQPen

    Writes the pen \a p to the stream \a s and returns a reference to
    the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQPen &p )
{
    // ### width() should not be restricted to 8-bit values
    if ( s.version() < 3 )
	return s << (TQ_UINT8)p.style() << (TQ_UINT8)p.width() << p.color();
    else
	return s << (TQ_UINT8)( p.style() | p.capStyle() | p.joinStyle() )
		 << (TQ_UINT8)p.width() << p.color();
}

/*!
    \relates TQPen

    Reads a pen from the stream \a s into \a p and returns a reference
    to the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQPen &p )
{
    TQ_UINT8 style, width;
    TQColor color;
    s >> style;
    s >> width;
    s >> color;
    p = TQPen( color, (uint)width, (TQt::PenStyle)style );	// owl
    return s;
}
#endif //TQT_NO_DATASTREAM

/*****************************************************************************
  TQBrush member functions
 *****************************************************************************/

/*!
    \class TQBrush ntqbrush.h

    \brief The TQBrush class defines the fill pattern of shapes drawn by a TQPainter.

    \ingroup graphics
    \ingroup images
    \ingroup shared

    A brush has a style and a color. One of the brush styles is a
    custom pattern, which is defined by a TQPixmap.

    The brush style defines the fill pattern. The default brush style
    is \c NoBrush (depending on how you construct a brush). This style
    tells the painter to not fill shapes. The standard style for
    filling is \c SolidPattern.

    The brush color defines the color of the fill pattern. The TQColor
    documentation lists the predefined colors.

    Use the TQPen class for specifying line/outline styles.

    Example:
    \code
        TQPainter painter;
        TQBrush   brush( yellow );           // yellow solid pattern
        painter.begin( &anyPaintDevice );   // paint something
        painter.setBrush( brush );          // set the yellow brush
        painter.setPen( NoPen );            // do not draw outline
        painter.drawRect( 40,30, 200,100 ); // draw filled rectangle
        painter.setBrush( NoBrush );        // do not fill
        painter.setPen( black );            // set black pen, 0 pixel width
        painter.drawRect( 10,10, 30,20 );   // draw rectangle outline
        painter.end();                      // painting done
    \endcode

    See the setStyle() function for a complete list of brush styles.

    \img brush-styles.png Brush Styles

    \sa TQPainter, TQPainter::setBrush(), TQPainter::setBrushOrigin()
*/


/*!
  \internal
  Initializes the brush.
*/

void TQBrush::init( const TQColor &color, BrushStyle style )
{
    data = new TQBrushData;
    TQ_CHECK_PTR( data );
    data->style	 = style;
    data->color	 = color;
    data->pixmap = 0;
}

/*!
    Constructs a default black brush with the style \c NoBrush (will
    not fill shapes).
*/

TQBrush::TQBrush()
{
    static TQBrushData* defBrushData = 0;
    if ( !defBrushData ) {
	static TQSharedCleanupHandler<TQBrushData> defBrushCleanup;
	defBrushData = new TQBrushData;
	defBrushData->style = NoBrush;
	defBrushData->color = TQt::black;
	defBrushData->pixmap = 0;
	defBrushCleanup.set( &defBrushData );
    }
    data = defBrushData;
    data->ref();
}

/*!
    Constructs a black brush with the style \a style.

    \sa setStyle()
*/

TQBrush::TQBrush( BrushStyle style )
{
    init( TQt::black, style );
}

/*!
    Constructs a brush with the color \a color and the style \a style.

    \sa setColor(), setStyle()
*/

TQBrush::TQBrush( const TQColor &color, BrushStyle style )
{
    init( color, style );
}

/*!
    Constructs a brush with the color \a color and a custom pattern
    stored in \a pixmap.

    The color will only have an effect for monochrome pixmaps, i.e.
    for TQPixmap::depth() == 1.

    Pixmap brushes are currently not supported when printing on X11.

    \sa setColor(), setPixmap()
*/

TQBrush::TQBrush( const TQColor &color, const TQPixmap &pixmap )
{
    init( color, CustomPattern );
    setPixmap( pixmap );
}

/*!
    Constructs a brush that is a \link shclass.html shallow
    copy\endlink of \a b.
*/

TQBrush::TQBrush( const TQBrush &b )
{
    data = b.data;
    data->ref();
}

/*!
    Destroys the brush.
*/

TQBrush::~TQBrush()
{
    if ( data->deref() ) {
	delete data->pixmap;
	delete data;
    }
}


/*!
    Detaches from shared brush data to make sure that this brush is
    the only one referring the data.

    If multiple brushes share common data, this brush dereferences the
    data and gets a copy of the data. Nothing is done if there is just
    a single reference.
*/

void TQBrush::detach()
{
    if ( data->count != 1 )
	*this = copy();
}


/*!
    Assigns \a b to this brush and returns a reference to this brush.
*/

TQBrush &TQBrush::operator=( const TQBrush &b )
{
    b.data->ref();				// beware of b = b
    if ( data->deref() ) {
	delete data->pixmap;
	delete data;
    }
    data = b.data;
    return *this;
}


/*!
    Returns a \link shclass.html deep copy\endlink of the brush.
*/

TQBrush TQBrush::copy() const
{
    if ( data->style == CustomPattern ) {     // brush has pixmap
	TQBrush b( data->color, *data->pixmap );
	return b;
    } else {				      // brush has std pattern
	TQBrush b( data->color, data->style );
	return b;
    }
}


/*!
    \fn BrushStyle TQBrush::style() const

    Returns the brush style.

    \sa setStyle()
*/

/*!
    Sets the brush style to \a s.

    The brush styles are:
    \table
    \header \i Pattern \i Meaning
    \row \i NoBrush \i will not fill shapes (default).
    \row \i SolidPattern  \i solid (100%) fill pattern.
    \row \i Dense1Pattern \i11 94% fill pattern.
    \row \i Dense2Pattern \i11 88% fill pattern.
    \row \i Dense3Pattern \i11 63% fill pattern.
    \row \i Dense4Pattern \i11 50% fill pattern.
    \row \i Dense5Pattern \i11 37% fill pattern.
    \row \i Dense6Pattern \i11 12% fill pattern.
    \row \i Dense7Pattern \i11 6% fill pattern.
    \row \i HorPattern \i horizontal lines pattern.
    \row \i VerPattern \i vertical lines pattern.
    \row \i CrossPattern \i crossing lines pattern.
    \row \i BDiagPattern \i diagonal lines (directed /) pattern.
    \row \i FDiagPattern \i diagonal lines (directed \) pattern.
    \row \i DiagCrossPattern \i diagonal crossing lines pattern.
    \row \i CustomPattern \i set when a pixmap pattern is being used.
    \endtable

    On Windows, dense and custom patterns cannot be transparent.

    See the \link #details Detailed Description\endlink for a picture
    of all the styles.

    \sa style()
*/

void TQBrush::setStyle( BrushStyle s )		// set brush style
{
    if ( data->style == s )
	return;
#if defined(QT_CHECK_RANGE)
    if ( s == CustomPattern )
	tqWarning( "TQBrush::setStyle: CustomPattern is for internal use" );
#endif
    detach();
    data->style = s;
}


/*!
    \fn const TQColor &TQBrush::color() const

    Returns the brush color.

    \sa setColor()
*/

/*!
    Sets the brush color to \a c.

    \sa color(), setStyle()
*/

void TQBrush::setColor( const TQColor &c )
{
    detach();
    data->color = c;
}


/*!
    \fn TQPixmap *TQBrush::pixmap() const

    Returns a pointer to the custom brush pattern, or 0 if no custom
    brush pattern has been set.

    \sa setPixmap()
*/

/*!
    Sets the brush pixmap to \a pixmap. The style is set to \c
    CustomPattern.

    The current brush color will only have an effect for monochrome
    pixmaps, i.e. for TQPixmap::depth() == 1.

    Pixmap brushes are currently not supported when printing on X11.

    \sa pixmap(), color()
*/

void TQBrush::setPixmap( const TQPixmap &pixmap )
{
    detach();
    if ( data->pixmap )
	delete data->pixmap;
    if ( pixmap.isNull() ) {
	data->style  = NoBrush;
	data->pixmap = 0;
    } else {
	data->style = CustomPattern;
	data->pixmap = new TQPixmap( pixmap );
	if ( data->pixmap->optimization() == TQPixmap::MemoryOptim )
	    data->pixmap->setOptimization( TQPixmap::NormalOptim );
    }
}


/*!
    \fn bool TQBrush::operator!=( const TQBrush &b ) const

    Returns true if the brush is different from \a b; otherwise
    returns false.

    Two brushes are different if they have different styles, colors or
    pixmaps.

    \sa operator==()
*/

/*!
    Returns true if the brush is equal to \a b; otherwise returns
    false.

    Two brushes are equal if they have equal styles, colors and
    pixmaps.

    \sa operator!=()
*/

bool TQBrush::operator==( const TQBrush &b ) const
{
    return (b.data == data) || (b.data->style == data->style &&
	    b.data->color  == data->color &&
	    b.data->pixmap == data->pixmap);
}


/*!
    \fn inline double TQPainter::translationX() const
    \internal
*/

/*!
    \fn inline double TQPainter::translationY() const
    \internal
*/


/*****************************************************************************
  TQBrush stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
/*!
    \relates TQBrush

    Writes the brush \a b to the stream \a s and returns a reference
    to the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQBrush &b )
{
    s << (TQ_UINT8)b.style() << b.color();
    if ( b.style() == TQt::CustomPattern )
#ifndef TQT_NO_IMAGEIO
	s << *b.pixmap();
#else
	tqWarning("No Image Brush I/O");
#endif
    return s;
}

/*!
    \relates TQBrush

    Reads the brush \a b from the stream \a s and returns a reference
    to the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQBrush &b )
{
    TQ_UINT8 style;
    TQColor color;
    s >> style;
    s >> color;
    if ( style == TQt::CustomPattern ) {
#ifndef TQT_NO_IMAGEIO
	TQPixmap pm;
	s >> pm;
	b = TQBrush( color, pm );
#else
	tqWarning("No Image Brush I/O");
#endif
    }
    else
	b = TQBrush( color, (TQt::BrushStyle)style );
    return s;
}
#endif // TQT_NO_DATASTREAM
