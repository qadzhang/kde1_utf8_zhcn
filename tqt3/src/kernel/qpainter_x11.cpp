/****************************************************************************
**
** Implementation of TQPainter class for X11
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

#include "qplatformdefs.h"

#include "ntqfont.h"
#include "ntqpainter.h"
#include "ntqwidget.h"
#include "ntqbitmap.h"
#include "ntqpixmapcache.h"
#include "ntqtextcodec.h"
#include "ntqpaintdevicemetrics.h"

#include "qt_x11_p.h"

#include "qtextlayout_p.h"
#include "qfontdata_p.h"
#include "qfontengine_p.h"
#include "qtextengine_p.h"

#include <math.h>

// paintevent magic to provide Windows semantics on X11
static TQRegion* paintEventClipRegion = 0;
static TQPaintDevice* paintEventDevice = 0;

void tqt_set_paintevent_clipping( TQPaintDevice* dev, const TQRegion& region)
{
    if ( !paintEventClipRegion )
        paintEventClipRegion = new TQRegion( region );
    else
        *paintEventClipRegion = region;
    paintEventDevice = dev;
}

void tqt_clear_paintevent_clipping()
{
    delete paintEventClipRegion;
    paintEventClipRegion = 0;
    paintEventDevice = 0;
}

class TQWFlagWidget : public TQWidget
{
public:
    void setWState( WFlags f )          { TQWidget::setWState(f); }
    void clearWState( WFlags f )        { TQWidget::clearWState(f); }
    void setWFlags( WFlags f )          { TQWidget::setWFlags(f); }
    void clearWFlags( WFlags f )        { TQWidget::clearWFlags(f); }
};

void tqt_erase_region( TQWidget* w, const TQRegion& region)
{
    TQRegion reg = region;

    if ( TQPainter::redirect(w) || (!w->isTopLevel() && w->backgroundPixmap()
                                   && w->backgroundOrigin() != TQWidget::WidgetOrigin) ) {
	TQPoint offset = w->backgroundOffset();
	int ox = offset.x();
	int oy = offset.y();

        bool unclipped = w->testWFlags( TQt::WPaintUnclipped );
        if ( unclipped )
            ((TQWFlagWidget*)w)->clearWFlags( TQt::WPaintUnclipped );
        TQPainter p( w );
        p.setClipRegion( region ); // automatically includes paintEventDevice if required
        if ( w->backgroundPixmap() )
            p.drawTiledPixmap( 0, 0, w->width(), w->height(),
                               *w->backgroundPixmap(), ox, oy );
        else
            p.fillRect( w->rect(), w->eraseColor() );
        if ( unclipped )
            ((TQWFlagWidget*)w)->setWFlags( TQt::WPaintUnclipped );
        return;
    }

    if ( w == paintEventDevice && paintEventClipRegion )
        reg = paintEventClipRegion->intersect( reg );

    TQMemArray<TQRect> r = reg.rects();
    for (uint i=0; i<r.size(); i++) {
        const TQRect& rr = r[(int)i];
        XClearArea( w->x11Display(), w->winId(),
                    rr.x(), rr.y(), rr.width(), rr.height(), False );
    }
}

void tqt_erase_rect( TQWidget* w, const TQRect& r)
{
    if ( TQPainter::redirect(w) || w == paintEventDevice
         || w->backgroundOrigin() != TQWidget::WidgetOrigin )
        tqt_erase_region( w, r );
    else
        XClearArea( w->x11Display(), w->winId(), r.x(), r.y(), r.width(), r.height(), False );

}

#ifdef TQT_NO_XFTFREETYPE
static const TQt::HANDLE rendhd = 0;
#endif

// hack, so we don't have to make TQRegion::clipRectangles() public or include
// X11 headers in ntqregion.h
inline void *qt_getClipRects( const TQRegion &r, int &num )
{
    return r.clipRectangles( num );
}

static inline void x11SetClipRegion(Display *dpy, GC gc, GC gc2, TQt::HANDLE draw, const TQRegion &r)
{
    int num;
    XRectangle *rects = (XRectangle *)qt_getClipRects( r, num );

    if (gc)
        XSetClipRectangles( dpy, gc, 0, 0, rects, num, YXBanded );
    if (gc2)
        XSetClipRectangles( dpy, gc2, 0, 0, rects, num, YXBanded );

#ifndef TQT_NO_XFTFREETYPE
    if (draw)
        XftDrawSetClipRectangles((XftDraw *) draw, 0, 0, rects, num);
#else
    Q_UNUSED(draw);
#endif // TQT_NO_XFTFREETYPE
}

static inline void x11ClearClipRegion(Display *dpy, GC gc, GC gc2, TQt::HANDLE draw)
{
    if (gc)
        XSetClipMask(dpy, gc, None);
    if (gc2)
        XSetClipMask(dpy, gc2, None);

#ifndef TQT_NO_XFTFREETYPE
    if (draw) {
	XftDrawSetClip((XftDraw *) draw, None);
    }
#else
    Q_UNUSED(draw);
#endif // TQT_NO_XFTFREETYPE
}


/*****************************************************************************
  Trigonometric function for TQPainter

  We have implemented simple sine and cosine function that are called from
  TQPainter::drawPie() and TQPainter::drawChord() when drawing the outline of
  pies and chords.
  These functions are slower and less accurate than math.h sin() and cos(),
  but with still around 1/70000th sec. execution time (on a 486DX2-66) and
  8 digits accuracy, it should not be the bottleneck in drawing these shapes.
  The advantage is that you don't have to link in the math library.
 *****************************************************************************/

const double Q_PI   = 3.14159265358979323846;   // pi
const double Q_2PI  = 6.28318530717958647693;   // 2*pi
const double Q_PI2  = 1.57079632679489661923;   // pi/2


#if defined(Q_CC_GNU) && defined(Q_OS_AIX)
// AIX 4.2 gcc 2.7.2.3 gets internal error.
static int tqRoundAIX( double d )
{
    return tqRound(d);
}
#define tqRound tqRoundAIX
#endif


#if defined(Q_CC_GNU) && defined(__i386__)

inline double qcos( double a )
{
    double r;
    __asm__ (
        "fcos"
        : "=t" (r) : "0" (a) );
    return(r);
}

inline double qsin( double a )
{
    double r;
    __asm__ (
        "fsin"
        : "=t" (r) : "0" (a) );
    return(r);
}

double qsincos( double a, bool calcCos=false )
{
    return calcCos ? qcos(a) : qsin(a);
}

#else

double qsincos( double a, bool calcCos=false )
{
    if ( calcCos )                              // calculate cosine
        a -= Q_PI2;
    if ( a >= Q_2PI || a <= -Q_2PI ) {          // fix range: -2*pi < a < 2*pi
        int m = (int)(a/Q_2PI);
        a -= Q_2PI*m;
    }
    if ( a < 0.0 )                              // 0 <= a < 2*pi
        a += Q_2PI;
    int sign = a > Q_PI ? -1 : 1;
    if ( a >= Q_PI )
        a = Q_2PI - a;
    if ( a >= Q_PI2 )
        a = Q_PI - a;
    if ( calcCos )
        sign = -sign;
    double a2  = a*a;                           // here: 0 <= a < pi/4
    double a3  = a2*a;                          // make taylor sin sum
    double a5  = a3*a2;
    double a7  = a5*a2;
    double a9  = a7*a2;
    double a11 = a9*a2;
    return (a-a3/6+a5/120-a7/5040+a9/362880-a11/39916800)*sign;
}

inline double qsin( double a ) { return qsincos(a, false); }
inline double qcos( double a ) { return qsincos(a, true); }

#endif


/*****************************************************************************
  TQPainter internal GC (Graphics Context) allocator.

  The GC allocator offers two functions; alloc_gc() and free_gc() that
  reuse GC objects instead of calling XCreateGC() and XFreeGC(), which
  are a whole lot slower.
 *****************************************************************************/

struct TQGC
{
    GC   gc;
    char in_use;
    bool mono;
    int scrn;
};

const  int  gc_array_size = 256;
static TQGC  gc_array[gc_array_size];            // array of GCs
static bool gc_array_init = false;


static void init_gc_array()
{
    if ( !gc_array_init ) {
        memset( gc_array, 0, gc_array_size*sizeof(TQGC) );
        gc_array_init = true;
    }
}

static void cleanup_gc_array( Display *dpy )
{
    TQGC *p = gc_array;
    int i = gc_array_size;
    if ( gc_array_init ) {
        while ( i-- ) {
            if ( p->gc )                        // destroy GC
                XFreeGC( dpy, p->gc );
            p++;
        }
        gc_array_init = false;
    }
}

// #define DONT_USE_GC_ARRAY

static GC alloc_gc( Display *dpy, int scrn, Drawable hd, bool monochrome=false,
                    bool privateGC = false )
{
#if defined(DONT_USE_GC_ARRAY)
    privateGC = true;                           // will be slower
#endif
    if ( privateGC ) {
        GC gc = XCreateGC( dpy, hd, 0, 0 );
        XSetGraphicsExposures( dpy, gc, False );
        return gc;
    }
    TQGC *p = gc_array;
    int i = gc_array_size;
    if ( !gc_array_init )                       // not initialized
        init_gc_array();
    while ( i-- ) {
        if ( !p->gc ) {                         // create GC (once)
            p->gc = XCreateGC( dpy, hd, 0, 0 );
            p->scrn = scrn;
            XSetGraphicsExposures( dpy, p->gc, False );
            p->in_use = false;
            p->mono   = monochrome;
        }
        if ( !p->in_use && p->mono == monochrome && p->scrn == scrn ) {
            p->in_use = true;                   // available/compatible GC
            return p->gc;
        }
        p++;
    }
#if defined(QT_CHECK_NULL)
    tqWarning( "TQPainter: Internal error; no available GC" );
#endif
    GC gc = XCreateGC( dpy, hd, 0, 0 );
    XSetGraphicsExposures( dpy, gc, False );
    return gc;
}

static void free_gc( Display *dpy, GC gc, bool privateGC = false )
{
#if defined(DONT_USE_GC_ARRAY)
    privateGC = true;                           // will be slower
#endif
    if ( privateGC ) {
        Q_ASSERT( dpy != 0 );
        XFreeGC( dpy, gc );
        return;
    }
    TQGC *p = gc_array;
    int i = gc_array_size;
    if ( gc_array_init ) {
        while ( i-- ) {
            if ( p->gc == gc ) {
                p->in_use = false;              // set available
                XSetClipMask( dpy, gc, None );  // make it reusable
                XSetFunction( dpy, gc, GXcopy );
                XSetFillStyle( dpy, gc, FillSolid );
                XSetTSOrigin( dpy, gc, 0, 0 );
                return;
            }
            p++;
        }
    }

    // not found in gc_array
    XFreeGC(dpy, gc);
}


/*****************************************************************************
  TQPainter internal GC (Graphics Context) cache for solid pens and
  brushes.

  The GC cache makes a significant contribution to speeding up
  drawing.  Setting new pen and brush colors will make the painter
  look for another GC with the same color instead of changing the
  color value of the GC currently in use. The cache structure is
  optimized for fast lookup.  Only solid line pens with line width 0
  and solid brushes are cached.

  In addition, stored GCs may have an implicit clipping region
  set. This prevents any drawing outside paint events. Both
  updatePen() and updateBrush() keep track of the validity of this
  clipping region by storing the clip_serial number in the cache.

*****************************************************************************/

struct TQGCC                                     // cached GC
{
    GC gc;
    uint pix;
    int count;
    int hits;
    uint clip_serial;
    int scrn;
};

const  int   gc_cache_size = 29;                // multiply by 4
static TQGCC *gc_cache_buf;
static TQGCC *gc_cache[4*gc_cache_size];
static bool  gc_cache_init = false;
static uint gc_cache_clip_serial = 0;


static void init_gc_cache()
{
    if ( !gc_cache_init ) {
        gc_cache_init = true;
	gc_cache_clip_serial = 0;
        TQGCC *g = gc_cache_buf = new TQGCC[4*gc_cache_size];
        memset( g, 0, 4*gc_cache_size*sizeof(TQGCC) );
        for ( int i=0; i<4*gc_cache_size; i++ )
            gc_cache[i] = g++;
    }
}


// #define GC_CACHE_STAT
#if defined(GC_CACHE_STAT)
#include "ntqtextstream.h"
#include "ntqbuffer.h"

static int g_numhits    = 0;
static int g_numcreates = 0;
static int g_numfaults  = 0;
#endif


static void cleanup_gc_cache()
{
    if ( !gc_cache_init )
        return;
#if defined(GC_CACHE_STAT)
    tqDebug( "Number of cache hits = %d", g_numhits );
    tqDebug( "Number of cache creates = %d", g_numcreates );
    tqDebug( "Number of cache faults = %d", g_numfaults );
    for ( int i=0; i<gc_cache_size; i++ ) {
        TQCString    str;
        TQBuffer     buf( str );
        buf.open(IO_ReadWrite);
        TQTextStream s(&buf);
        s << i << ": ";
        for ( int j=0; j<4; j++ ) {
            TQGCC *g = gc_cache[i*4+j];
            s << (g->gc ? 'X' : '-') << ',' << g->hits << ','
              << g->count << '\t';
        }
        s << '\0';
        tqDebug( str );
        buf.close();
    }
#endif
    delete [] gc_cache_buf;
    gc_cache_init = false;
}


static bool obtain_gc( void **ref, GC *gc, uint pix, Display *dpy, int scrn,
		       TQt::HANDLE hd, uint painter_clip_serial )
{
    if ( !gc_cache_init )
        init_gc_cache();

    int   k = (pix % gc_cache_size) * 4;
    TQGCC *g = gc_cache[k];
    TQGCC *prev = 0;

#define NOMATCH (g->gc && (g->pix != pix || g->scrn != scrn || \
                 (g->clip_serial > 0 && g->clip_serial != painter_clip_serial)))

    if ( NOMATCH ) {
        prev = g;
        g = gc_cache[++k];
        if ( NOMATCH ) {
            prev = g;
            g = gc_cache[++k];
            if ( NOMATCH ) {
                prev = g;
                g = gc_cache[++k];
                if ( NOMATCH ) {
                    if ( g->count == 0 && g->scrn == scrn) {    // steal this GC
                        g->pix   = pix;
                        g->count = 1;
                        g->hits  = 1;
			g->clip_serial = 0;
                        XSetForeground( dpy, g->gc, pix );
			XSetClipMask(dpy, g->gc, None);
                        gc_cache[k]   = prev;
                        gc_cache[k-1] = g;
                        *ref = (void *)g;
                        *gc = g->gc;
                        return true;
                    } else {                    // all GCs in use
#if defined(GC_CACHE_STAT)
                        g_numfaults++;
#endif
                        *ref = 0;
                        return false;
                    }
                }
            }
        }
    }

#undef NOMATCH

    *ref = (void *)g;

    if ( g->gc ) {                              // reuse existing GC
#if defined(GC_CACHE_STAT)
        g_numhits++;
#endif
        *gc = g->gc;
        g->count++;
        g->hits++;
        if ( prev && g->hits > prev->hits ) {   // maintain LRU order
            gc_cache[k]   = prev;
            gc_cache[k-1] = g;
        }
        return true;
    } else {                                    // create new GC
#if defined(GC_CACHE_STAT)
        g_numcreates++;
#endif
        g->gc = alloc_gc( dpy, scrn, hd, false );
        g->scrn = scrn;
        g->pix = pix;
        g->count = 1;
        g->hits = 1;
        g->clip_serial = 0;
        *gc = g->gc;
        return false;
    }
}

static inline void release_gc( void *ref )
{
    ((TQGCC*)ref)->count--;
}

/*****************************************************************************
  TQPainter member functions
 *****************************************************************************/

/*!
  \internal

  Internal function that initializes the painter.
*/

void TQPainter::initialize()
{
    init_gc_array();
    init_gc_cache();
}

/*!
  \internal

  Internal function that cleans up the painter.
*/

void TQPainter::cleanup()
{
    cleanup_gc_cache();
    cleanup_gc_array( TQPaintDevice::x11AppDisplay() );
    TQPointArray::cleanBuffers();
}

/*!
  \internal

  Internal function that destroys up the painter.
*/

void TQPainter::destroy()
{

}

void TQPainter::init()
{
    d = 0;
    flags = IsStartingUp;
    bg_col = white;                             // default background color
    bg_mode = TransparentMode;                  // default background mode
    rop = CopyROP;                                // default ROP
    tabstops = 0;                               // default tabbing
    tabarray = 0;
    tabarraylen = 0;
    ps_stack = 0;
    wm_stack = 0;
    gc = gc_brush = 0;
    pdev = 0;
    dpy  = 0;
    txop = txinv = 0;
    penRef = brushRef = 0;
    clip_serial = 0;
    pfont = 0;
    block_ext = false;
}


/*!
    \fn const TQFont &TQPainter::font() const

    Returns the currently set painter font.

    \sa setFont(), TQFont
*/

/*!
    Sets the painter's font to \a font.

    This font is used by subsequent drawText() functions. The text
    color is the same as the pen color.

    \sa font(), drawText()
*/

void TQPainter::setFont( const TQFont &font )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
        tqWarning( "TQPainter::setFont: Will be reset by begin()" );
#endif
    if ( cfont.d != font.d ) {
        cfont = font;
	cfont.x11SetScreen( scrn );
        setf(DirtyFont);
    }
}


void TQPainter::updateFont()
{
    if (!isActive())
        return;

    clearf(DirtyFont);
    if ( testf(ExtDev) ) {
        if (pdev->devType() == TQInternal::Printer) {
            if ( pfont ) delete pfont;
            pfont = new TQFont( cfont.d, pdev );
        }
        TQPDevCmdParam param[1];
        param[0].font = &cfont;
        if ( !pdev->cmd( TQPaintDevice::PdcSetFont, this, param ) || !hd )
            return;
    }
    setf(NoCache);
    if ( penRef )
        updatePen();                            // force a non-cached GC
}


void TQPainter::updatePen()
{
    if (!isActive())
        return;

    if ( testf(ExtDev) ) {
        TQPDevCmdParam param[1];
        param[0].pen = &cpen;
        if ( !pdev->cmd( TQPaintDevice::PdcSetPen, this, param ) || !hd )
            return;
    }

    int ps = cpen.style();
    bool cacheIt = !testf(ClipOn|MonoDev|NoCache) &&
                   (ps == NoPen || ps == SolidLine) &&
                   cpen.width() == 0 && rop == CopyROP;

    bool obtained = false;
    bool internclipok = hasClipping();
    if ( cacheIt ) {
        if ( gc ) {
            if ( penRef )
                release_gc( penRef );
            else
                free_gc( dpy, gc );
        }
        obtained = obtain_gc(&penRef, &gc, cpen.color().pixel(scrn), dpy, scrn,
			     hd, clip_serial);
        if ( !obtained && !penRef )
            gc = alloc_gc( dpy, scrn, hd, false );
    } else {
        if ( gc ) {
            if ( penRef ) {
                release_gc( penRef );
                penRef = 0;
                gc = alloc_gc( dpy, scrn, hd, testf(MonoDev) );
            } else {
                internclipok = true;
            }
        } else {
            gc = alloc_gc( dpy, scrn, hd, testf(MonoDev), testf(UsePrivateCx) );
        }
    }

    if ( !internclipok ) {
        if ( pdev == paintEventDevice && paintEventClipRegion ) {
            if ( penRef &&((TQGCC*)penRef)->clip_serial < gc_cache_clip_serial ) {
		x11SetClipRegion( dpy, gc, 0, rendhd, *paintEventClipRegion );
                ((TQGCC*)penRef)->clip_serial = gc_cache_clip_serial;
            } else if ( !penRef ) {
		x11SetClipRegion( dpy, gc, 0, rendhd, *paintEventClipRegion );
            }
        } else if (penRef && ((TQGCC*)penRef)->clip_serial ) {
            x11ClearClipRegion(dpy, gc, 0, rendhd);
            ((TQGCC*)penRef)->clip_serial = 0;
        }
    }

    if ( obtained )
        return;

    char dashes[10];                            // custom pen dashes
    int dash_len = 0;                           // length of dash list
    int s = LineSolid;
    int cp = CapButt;
    int jn = JoinMiter;

    /*
      We are emulating Windows here.  Windows treats cpen.width() == 1
      (or 0) as a very special case.  The fudge variable unifies this
      case with the general case.
    */
    int dot = cpen.width();                     // width of a dot
    int fudge = 1;
    bool allow_zero_lw = true;
    if ( dot <= 1 ) {
        dot = 3;
        fudge = 2;
    }

    switch( ps ) {
    case NoPen:
    case SolidLine:
	s = LineSolid;
	break;
    case DashLine:
	dashes[0] = fudge * 3 * dot;
	dashes[1] = fudge * dot;
	dash_len = 2;
	allow_zero_lw = false;
	break;
    case DotLine:
	dashes[0] = dot;
	dashes[1] = dot;
	dash_len = 2;
	allow_zero_lw = false;
	break;
    case DashDotLine:
	dashes[0] = 3 * dot;
	dashes[1] = fudge * dot;
	dashes[2] = dot;
	dashes[3] = fudge * dot;
	dash_len = 4;
	allow_zero_lw = false;
	break;
    case DashDotDotLine:
	dashes[0] = 3 * dot;
	dashes[1] = dot;
	dashes[2] = dot;
	dashes[3] = dot;
	dashes[4] = dot;
	dashes[5] = dot;
	dash_len = 6;
	allow_zero_lw = false;
	break;
    case FineDotLine:
	dot = 1;
	dashes[0] = dot;
	dashes[1] = dot;
	dash_len = 2;
	allow_zero_lw = false;
    }
    Q_ASSERT( dash_len <= (int) sizeof(dashes) );

    switch ( cpen.capStyle() ) {
    case SquareCap:
        cp = CapProjecting;
        break;
    case RoundCap:
        cp = CapRound;
        break;
    case FlatCap:
    default:
        cp = CapButt;
        break;
    }
    switch ( cpen.joinStyle() ) {
    case BevelJoin:
        jn = JoinBevel;
        break;
    case RoundJoin:
        jn = JoinRound;
        break;
    case MiterJoin:
    default:
        jn = JoinMiter;
        break;
    }

    XSetForeground( dpy, gc, cpen.color().pixel(scrn) );
    XSetBackground( dpy, gc, bg_col.pixel(scrn) );

    if ( dash_len ) {                           // make dash list
        XSetDashes( dpy, gc, 0, dashes, dash_len );
        s = bg_mode == TransparentMode ? LineOnOffDash : LineDoubleDash;
    }
    XSetLineAttributes( dpy, gc,
			(! allow_zero_lw && cpen.width() == 0) ? 1 : cpen.width(),
			s, cp, jn );
}


void TQPainter::updateBrush()
{
    if (!isActive())
        return;

    static const uchar dense1_pat[] = { 0xff, 0xbb, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff };
    static const uchar dense2_pat[] = { 0x77, 0xff, 0xdd, 0xff, 0x77, 0xff, 0xdd, 0xff };
    static const uchar dense3_pat[] = { 0x55, 0xbb, 0x55, 0xee, 0x55, 0xbb, 0x55, 0xee };
    static const uchar dense4_pat[] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };
    static const uchar dense5_pat[] = { 0xaa, 0x44, 0xaa, 0x11, 0xaa, 0x44, 0xaa, 0x11 };
    static const uchar dense6_pat[] = { 0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00 };
    static const uchar dense7_pat[] = { 0x00, 0x44, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00 };
    static const uchar hor_pat[] = {                      // horizontal pattern
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uchar ver_pat[] = {                      // vertical pattern
	0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
	0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
	0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
	0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
	0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
	0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20 };
    static const uchar cross_pat[] = {                    // cross pattern
	0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0xff, 0xff, 0xff,
	0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
	0x08, 0x82, 0x20, 0xff, 0xff, 0xff, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
	0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0xff, 0xff, 0xff,
	0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
	0x08, 0x82, 0x20, 0xff, 0xff, 0xff, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20 };
    static const uchar bdiag_pat[] = {                    // backward diagonal pattern
	0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x01, 0x01,
	0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04,
	0x02, 0x02, 0x01, 0x01, 0x80, 0x80, 0x40, 0x40 };
    static const uchar fdiag_pat[] = {                    // forward diagonal pattern
	0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x40, 0x40,
	0x80, 0x80, 0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10,
	0x20, 0x20, 0x40, 0x40, 0x80, 0x80, 0x01, 0x01 };
    static const uchar dcross_pat[] = {                   // diagonal cross pattern
	0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x14, 0x14, 0x22, 0x22, 0x41, 0x41,
	0x80, 0x80, 0x41, 0x41, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x14, 0x14,
	0x22, 0x22, 0x41, 0x41, 0x80, 0x80, 0x41, 0x41 };
    static const uchar * const pat_tbl[] = {
	dense1_pat, dense2_pat, dense3_pat, dense4_pat, dense5_pat,
	dense6_pat, dense7_pat,
	hor_pat, ver_pat, cross_pat, bdiag_pat, fdiag_pat, dcross_pat };

    if ( testf(ExtDev) ) {
        TQPDevCmdParam param[1];
        param[0].brush = &cbrush;
        if ( !pdev->cmd( TQPaintDevice::PdcSetBrush, this, param ) || !hd )
            return;
    }

    int  bs      = cbrush.style();
    bool cacheIt = !testf(ClipOn|MonoDev|NoCache) &&
                   (bs == NoBrush || bs == SolidPattern) &&
	           bro.x() == 0 && bro.y() == 0 && rop == CopyROP;

    bool obtained = false;
    bool internclipok = hasClipping();
    if ( cacheIt ) {
        if ( gc_brush ) {
            if ( brushRef )
                release_gc( brushRef );
            else
                free_gc( dpy, gc_brush );
        }
        obtained = obtain_gc(&brushRef, &gc_brush, cbrush.color().pixel(scrn), dpy,
			     scrn, hd, clip_serial);
        if ( !obtained && !brushRef )
            gc_brush = alloc_gc( dpy, scrn, hd, false );
    } else {
        if ( gc_brush ) {
            if ( brushRef ) {
                release_gc( brushRef );
                brushRef = 0;
                gc_brush = alloc_gc( dpy, scrn, hd, testf(MonoDev) );
            } else {
                internclipok = true;
            }
        } else {
            gc_brush = alloc_gc( dpy, scrn, hd, testf(MonoDev), testf(UsePrivateCx));
        }
    }

    if ( !internclipok ) {
        if ( pdev == paintEventDevice && paintEventClipRegion ) {
            if ( brushRef &&((TQGCC*)brushRef)->clip_serial < gc_cache_clip_serial ) {
		x11SetClipRegion( dpy, gc_brush, 0, rendhd, *paintEventClipRegion );
                ((TQGCC*)brushRef)->clip_serial = gc_cache_clip_serial;
            } else if ( !brushRef ){
		x11SetClipRegion( dpy, gc_brush, 0, rendhd, *paintEventClipRegion );
            }
        } else if (brushRef && ((TQGCC*)brushRef)->clip_serial ) {
            x11ClearClipRegion(dpy, gc_brush, 0, rendhd);
            ((TQGCC*)brushRef)->clip_serial = 0;
        }
    }

    if ( obtained )
        return;

    const uchar *pat = 0;                             // pattern
    int d = 0;                                  // defalt pattern size: d*d
    int s  = FillSolid;
    if ( bs >= Dense1Pattern && bs <= DiagCrossPattern ) {
        pat = pat_tbl[ bs-Dense1Pattern ];
        if ( bs <= Dense7Pattern )
            d = 8;
        else if ( bs <= CrossPattern )
            d = 24;
        else
            d = 16;
    }

    XSetLineAttributes( dpy, gc_brush, 0, LineSolid, CapButt, JoinMiter );
    XSetForeground( dpy, gc_brush, cbrush.color().pixel(scrn) );
    XSetBackground( dpy, gc_brush, bg_col.pixel(scrn) );

    if ( bs == CustomPattern || pat ) {
        TQPixmap *pm;
        if ( pat ) {
            TQString key;
            key.sprintf( "$qt-brush$%d", bs );
            pm = TQPixmapCache::find( key );
            bool del = false;
            if ( !pm ) {                        // not already in pm dict
                pm = new TQBitmap( d, d, pat, true );
                TQ_CHECK_PTR( pm );
                del = !TQPixmapCache::insert( key, pm );
            }
            if ( cbrush.data->pixmap )
                delete cbrush.data->pixmap;
            cbrush.data->pixmap = new TQPixmap( *pm );
            if (del) delete pm;
        }
        pm = cbrush.data->pixmap;
        pm->x11SetScreen( scrn );
        if ( pm->depth() == 1 ) {
            XSetStipple( dpy, gc_brush, pm->handle() );
            s = bg_mode == TransparentMode ? FillStippled : FillOpaqueStippled;
        } else {
            XSetTile( dpy, gc_brush, pm->handle() );
            s = FillTiled;
        }
    }
    XSetFillStyle( dpy, gc_brush, s );
}


/*!
    Begins painting the paint device \a pd and returns true if
    successful; otherwise returns false. If \a unclipped is true, the
    painting will not be clipped at the paint device's boundaries,
    (although this is not supported by all platforms).

    The errors that can occur are serious problems, such as these:

    \code
	p->begin( 0 ); // impossible - paint device cannot be 0

	TQPixmap pm( 0, 0 );
	p->begin( pm ); // impossible - pm.isNull();

	p->begin( myWidget );
	p2->begin( myWidget ); // impossible - only one painter at a time
    \endcode

    Note that most of the time, you can use one of the constructors
    instead of begin(), and that end() is automatically done at
    destruction.

    \warning A paint device can only be painted by one painter at a
    time.

    \sa end(), flush()
*/

bool TQPainter::begin( const TQPaintDevice *pd, bool unclipped )
{
    if ( isActive() ) {                         // already active painting
#if defined(QT_CHECK_STATE)
        tqWarning( "TQPainter::begin: Painter is already active."
                  "\n\tYou must end() the painter before a second begin()" );
#endif
        return false;
    }
    if ( pd == 0 ) {
#if defined(QT_CHECK_NULL)
        tqWarning( "TQPainter::begin: Paint device cannot be null" );
#endif
        return false;
    }

    TQPixmap::x11SetDefaultScreen( pd->x11Screen() );

    const TQWidget *copyFrom = 0;
    pdev = redirect( (TQPaintDevice*)pd );
    if ( pdev ) {				    // redirected paint device?
	if ( pd->devType() == TQInternal::Widget )
	    copyFrom = (const TQWidget *)pd;	    // copy widget settings
    } else {
	pdev = (TQPaintDevice*)pd;
    }

    if ( pdev->isExtDev() && pdev->paintingActive() ) {
        // somebody else is already painting
#if defined(QT_CHECK_STATE)
        tqWarning( "TQPainter::begin: Another TQPainter is already painting "
                  "this device;\n\tAn extended paint device can only be "
                  "painted by one TQPainter at a time." );
#endif
        return false;
    }

    bool reinit = flags != IsStartingUp;        // 2nd or 3rd etc. time called
    flags = IsActive | DirtyFont;               // init flags
    int dt = pdev->devType();                   // get the device type

    if ( (pdev->devFlags & TQInternal::ExternalDevice) != 0 )
        setf(ExtDev);
    else if ( dt == TQInternal::Pixmap )         // device is a pixmap
        ((TQPixmap*)pdev)->detach();             // will modify it

    dpy = pdev->x11Display();                   // get display variable
    scrn = pdev->x11Screen();			// get screen variable
    hd  = pdev->handle();                       // get handle to drawable
    rendhd = pdev->rendhd;

    if ( testf(ExtDev) ) {                      // external device
        if ( !pdev->cmd( TQPaintDevice::PdcBegin, this, 0 ) ) {
            // could not begin painting
            if ( reinit )
                clearf( IsActive | DirtyFont );
            else
                flags = IsStartingUp;
            pdev = 0;
            return false;
        }
        if ( tabstops )                         // update tabstops for device
            setTabStops( tabstops );
        if ( tabarray )                         // update tabarray for device
            setTabArray( tabarray );
    }

    if ( pdev->x11Depth() != pdev->x11AppDepth( scrn ) ) { // non-standard depth
        setf(NoCache);
        setf(UsePrivateCx);
    }

    pdev->painters++;                           // also tell paint device
    bro = curPt = TQPoint( 0, 0 );
    if ( reinit ) {
        bg_mode = TransparentMode;              // default background mode
        rop = CopyROP;                          // default ROP
        wxmat.reset();                          // reset world xform matrix
	xmat.reset();
	ixmat.reset();
        txop = txinv = 0;
        if ( dt != TQInternal::Widget ) {
            TQFont  defaultFont;                 // default drawing tools
            TQPen   defaultPen;
            TQBrush defaultBrush;
            cfont  = defaultFont;               // set these drawing tools
            cpen   = defaultPen;
            cbrush = defaultBrush;
            bg_col = white;                     // default background color
        }
    }
    wx = wy = vx = vy = 0;                      // default view origins

    if ( dt == TQInternal::Widget ) {                    // device is a widget
        TQWidget *w = (TQWidget*)pdev;
        cfont = w->font();                      // use widget font
        cpen = TQPen( w->foregroundColor() );    // use widget fg color
        if ( reinit ) {
            TQBrush defaultBrush;
            cbrush = defaultBrush;
        }
        bg_col = w->backgroundColor();          // use widget bg color
        ww = vw = w->width();                   // default view size
        wh = vh = w->height();
        if ( unclipped || w->testWFlags( WPaintUnclipped ) ) {  // paint direct on device
            setf( NoCache );
            setf(UsePrivateCx);
            updatePen();
            updateBrush();
            XSetSubwindowMode( dpy, gc, IncludeInferiors );
            XSetSubwindowMode( dpy, gc_brush, IncludeInferiors );
#ifndef TQT_NO_XFTFREETYPE
	    if (rendhd)
                XftDrawSetSubwindowMode((XftDraw *) rendhd, IncludeInferiors);
#endif
        }
    } else if ( dt == TQInternal::Pixmap ) {             // device is a pixmap
        TQPixmap *pm = (TQPixmap*)pdev;
        if ( pm->isNull() ) {
#if defined(QT_CHECK_NULL)
            tqWarning( "TQPainter::begin: Cannot paint null pixmap" );
#endif
            end();
            return false;
        }
        bool mono = pm->depth() == 1;           // monochrome bitmap
        if ( mono ) {
            setf( MonoDev );
            bg_col = color0;
            cpen.setColor( color1 );
        }
        ww = vw = pm->width();                  // default view size
        wh = vh = pm->height();
    } else if ( testf(ExtDev) ) {               // external device
        ww = vw = pdev->metric( TQPaintDeviceMetrics::PdmWidth );
        wh = vh = pdev->metric( TQPaintDeviceMetrics::PdmHeight );
    }
    if ( ww == 0 )
        ww = wh = vw = vh = 1024;
    if ( copyFrom ) {                           // copy redirected widget
        cfont = copyFrom->font();
        cpen = TQPen( copyFrom->foregroundColor() );
        bg_col = copyFrom->backgroundColor();
    }
    if ( testf(ExtDev) ) {                      // external device
        setBackgroundColor( bg_col );           // default background color
        setBackgroundMode( TransparentMode );   // default background mode
        setRasterOp( CopyROP );                 // default raster operation
    }
    clip_serial = gc_cache_clip_serial++;
    updateBrush();
    updatePen();
    return true;
}

/*!
    Ends painting. Any resources used while painting are released.

    Note that while you mostly don't need to call end(), the
    destructor will do it, there is at least one common case when it
    is needed, namely double buffering.

    \code
	TQPainter p( myPixmap, this )
	// ...
	p.end(); // stops drawing on myPixmap
	p.begin( this );
	p.drawPixmap( 0, 0, myPixmap );
    \endcode

    Since you can't draw a TQPixmap while it is being painted, it is
    necessary to close the active painter.

    \sa begin(), isActive()
*/

bool TQPainter::end()                            // end painting
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
        tqWarning( "TQPainter::end: Missing begin() or begin() failed" );
#endif
        return false;
    }
    killPStack();

    //#### This should not be necessary:
    if ( pdev->devType() == TQInternal::Widget  &&       // #####
         ((TQWidget*)pdev)->testWFlags(WPaintUnclipped) ) {
        if ( gc )
            XSetSubwindowMode( dpy, gc, ClipByChildren );
        if ( gc_brush )
            XSetSubwindowMode( dpy, gc_brush, ClipByChildren );
    }

    if ( gc_brush ) {                           // restore brush gc
        if ( brushRef ) {
            release_gc( brushRef );
            brushRef = 0;
        } else {
            free_gc( dpy, gc_brush, testf(UsePrivateCx) );
        }
        gc_brush = 0;

    }
    if ( gc ) {                                 // restore pen gc
        if ( penRef ) {
            release_gc( penRef );
            penRef = 0;
        } else {
            free_gc( dpy, gc, testf(UsePrivateCx) );
        }
        gc = 0;
    }

    if ( testf(ExtDev) )
        pdev->cmd( TQPaintDevice::PdcEnd, this, 0 );

#ifndef TQT_NO_XFTFREETYPE
    if (rendhd) {
        // reset clipping/subwindow mode on our render picture
        XftDrawSetClip((XftDraw *) rendhd, None);
        XftDrawSetSubwindowMode((XftDraw *) rendhd, ClipByChildren);
    }
#endif // TQT_NO_XFTFREETYPE

    if ( pfont ) {
	delete pfont;
	pfont = 0;
    }

    flags = 0;
    pdev->painters--;
    pdev = 0;
    dpy  = 0;
    return true;
}

/*!
    Flushes any buffered drawing operations inside the region \a
    region using clipping mode \a cm.

    The flush may update the whole device if the platform does not
    support flushing to a specified region.

    \sa flush() CoordinateMode
*/

void TQPainter::flush(const TQRegion &rgn, CoordinateMode m)
{
    if ( testf(ExtDev) ) {
        TQPDevCmdParam param[2];
        param[0].rgn = &rgn;
        param[1].ival = m;
        pdev->cmd( TQPaintDevice::PdcFlushRegion, this, param );
        return;
    }

    flush();
}


/*!
    \overload

    Flushes any buffered drawing operations.
*/

void TQPainter::flush()
{
    if ( testf(ExtDev) ) {
        pdev->cmd( TQPaintDevice::PdcFlush, this, 0 );
        return;
    }

    if ( isActive() && dpy )
        XFlush( dpy );
}


/*!
    Sets the background color of the painter to \a c.

    The background color is the color that is filled in when drawing
    opaque text, stippled lines and bitmaps. The background color has
    no effect in transparent background mode (which is the default).

    \sa backgroundColor() setBackgroundMode() BackgroundMode
*/

void TQPainter::setBackgroundColor( const TQColor &c )
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
        tqWarning( "TQPainter::setBackgroundColor: Call begin() first" );
#endif
        return;
    }
    bg_col = c;
    if ( testf(ExtDev) ) {
        TQPDevCmdParam param[1];
        param[0].color = &bg_col;
        if ( !pdev->cmd( TQPaintDevice::PdcSetBkColor, this, param ) || !hd )
            return;
    }
    if ( !penRef )
        updatePen();                            // update pen setting
    if ( !brushRef )
        updateBrush();                          // update brush setting
}

/*!
    Sets the background mode of the painter to \a m, which must be
    either \c TransparentMode (the default) or \c OpaqueMode.

    Transparent mode draws stippled lines and text without setting the
    background pixels. Opaque mode fills these space with the current
    background color.

    Note that in order to draw a bitmap or pixmap transparently, you
    must use TQPixmap::setMask().

    \sa backgroundMode(), setBackgroundColor()
*/

void TQPainter::setBackgroundMode( BGMode m )
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
        tqWarning( "TQPainter::setBackgroundMode: Call begin() first" );
#endif
        return;
    }
    if ( m != TransparentMode && m != OpaqueMode ) {
#if defined(QT_CHECK_RANGE)
        tqWarning( "TQPainter::setBackgroundMode: Invalid mode" );
#endif
        return;
    }
    bg_mode = m;
    if ( testf(ExtDev) ) {
        TQPDevCmdParam param[1];
        param[0].ival = m;
        if ( !pdev->cmd( TQPaintDevice::PdcSetBkMode, this, param ) || !hd )
            return;
    }
    if ( !penRef )
        updatePen();                            // update pen setting
    if ( !brushRef )
        updateBrush();                          // update brush setting
}

static const short ropCodes[] = {                     // ROP translation table
    GXcopy, // CopyROP
    GXor, // OrROP
    GXxor, // XorROP
    GXandInverted, // NotAndROP EraseROP
    GXcopyInverted, // NotCopyROP
    GXorInverted, // NotOrROP
    GXequiv, // NotXorROP
    GXand, // AndROP
    GXinvert, // NotROP
    GXclear, // ClearROP
    GXset, // SetROP
    GXnoop, // NopROP
    GXandReverse, // AndNotROP
    GXorReverse, // OrNotROP
    GXnand, // NandROP
    GXnor // NorROP
};


/*!
    Sets the \link TQt::RasterOp raster operation \endlink to \a r.
    The default is \c CopyROP.

    \sa rasterOp() TQt::RasterOp
*/

void TQPainter::setRasterOp( RasterOp r )
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
        tqWarning( "TQPainter::setRasterOp: Call begin() first" );
#endif
        return;
    }
    if ( (uint)r > LastROP ) {
#if defined(QT_CHECK_RANGE)
        tqWarning( "TQPainter::setRasterOp: Invalid ROP code" );
#endif
        return;
    }
    rop = r;
    if ( testf(ExtDev) ) {
        TQPDevCmdParam param[1];
        param[0].ival = r;
        if ( !pdev->cmd( TQPaintDevice::PdcSetROP, this, param ) || !hd )
            return;
    }
    if ( penRef )
        updatePen();                            // get non-cached pen GC
    if ( brushRef )
        updateBrush();                          // get non-cached brush GC
    XSetFunction( dpy, gc, ropCodes[rop] );
    XSetFunction( dpy, gc_brush, ropCodes[rop] );
}

// ### matthias - true?

/*!
    Sets the brush origin to \a (x, y).

    The brush origin specifies the (0, 0) coordinate of the painter's
    brush. This setting only applies to pattern brushes and pixmap
    brushes.

    \sa brushOrigin()
*/

void TQPainter::setBrushOrigin( int x, int y )
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
        tqWarning( "TQPainter::setBrushOrigin: Call begin() first" );
#endif
        return;
    }
    bro = TQPoint(x, y);
    if ( testf(ExtDev) ) {
        TQPDevCmdParam param[1];
        param[0].point = &bro;
        if ( !pdev->cmd( TQPaintDevice::PdcSetBrushOrigin, this, param ) ||
             !hd )
            return;
    }
    if ( brushRef )
        updateBrush();                          // get non-cached brush GC
    XSetTSOrigin( dpy, gc_brush, x, y );
}


/*!
    Enables clipping if \a enable is true, or disables clipping if \a
    enable is false.

    \sa hasClipping(), setClipRect(), setClipRegion()
*/

void TQPainter::setClipping( bool enable )
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
        tqWarning( "TQPainter::setClipping: Will be reset by begin()" );
#endif
        return;
    }

    if ( enable == testf(ClipOn) )
        return;

    setf( ClipOn, enable );
    if ( testf(ExtDev) ) {
	if ( block_ext )
	    return;
        TQPDevCmdParam param[1];
        param[0].ival = enable;
        if ( !pdev->cmd( TQPaintDevice::PdcSetClip, this, param ) || !hd )
            return;
    }
    if ( enable ) {
        TQRegion rgn = crgn;
        if ( pdev == paintEventDevice && paintEventClipRegion )
            rgn = rgn.intersect( *paintEventClipRegion );
        if ( penRef )
            updatePen();
        if ( brushRef )
            updateBrush();
	x11SetClipRegion( dpy, gc, gc_brush, rendhd, rgn );
    } else {
        if ( pdev == paintEventDevice && paintEventClipRegion ) {
	    x11SetClipRegion( dpy, gc, gc_brush , rendhd, *paintEventClipRegion );
        } else {
            x11ClearClipRegion(dpy, gc, gc_brush, rendhd);
        }
    }
}


/*!
    \overload

    Sets the clip region to the rectangle \a r and enables clipping.
    The clip mode is set to \a m.

    \sa  CoordinateMode
*/

void TQPainter::setClipRect( const TQRect &r, CoordinateMode m )
{
    setClipRegion( TQRegion( r ), m );
}

/*!
    Sets the clip region to \a rgn and enables clipping. The clip mode
    is set to \a m.

    Note that the clip region is given in physical device coordinates
    and \e not subject to any \link coordsys.html coordinate
    transformation.\endlink

    \sa setClipRect(), clipRegion(), setClipping() CoordinateMode
*/

void TQPainter::setClipRegion( const TQRegion &rgn, CoordinateMode m )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
        tqWarning( "TQPainter::setClipRegion: Will be reset by begin()" );
#endif
    if ( m == CoordDevice )
	crgn = rgn;
    else
	crgn = xmat * rgn;

    if ( testf(ExtDev) ) {
	if ( block_ext )
	    return;
        TQPDevCmdParam param[2];
        param[0].rgn = &rgn;
        param[1].ival = m;
        if ( !pdev->cmd( TQPaintDevice::PdcSetClipRegion, this, param ) )
            return; // device cannot clip
    }
    clearf( ClipOn );                           // be sure to update clip rgn
    setClipping( true );
}


/*!
    \internal

  Internal function for drawing a polygon.
*/

void TQPainter::drawPolyInternal( const TQPointArray &a, bool close )
{
    if ( a.size() < 2 )
        return;

    int x1, y1, x2, y2;                         // connect last to first point
    a.point( a.size()-1, &x1, &y1 );
    a.point( 0, &x2, &y2 );
    bool do_close = close && !(x1 == x2 && y1 == y2);

    if ( close && cbrush.style() != NoBrush ) { // draw filled polygon
        XFillPolygon( dpy, hd, gc_brush, (XPoint*)a.shortPoints(), a.size(),
                      Nonconvex, CoordModeOrigin );
        if ( cpen.style() == NoPen ) {          // draw fake outline
            XDrawLines( dpy, hd, gc_brush, (XPoint*)a.shortPoints(), a.size(),
                        CoordModeOrigin );
            if ( do_close )
                XDrawLine( dpy, hd, gc_brush, x1, y1, x2, y2 );
        }
    }
    if ( cpen.style() != NoPen ) {              // draw outline
        XDrawLines( dpy, hd, gc, (XPoint*)a.shortPoints(), a.size(),
                    CoordModeOrigin);
        if ( do_close )
            XDrawLine( dpy, hd, gc, x1, y1, x2, y2 );
    }
}


/*!
    Draws/plots a single point at \a (x, y) using the current pen.

    \sa TQPen
*/

void TQPainter::drawPoint( int x, int y )
{
    if ( !isActive() )
        return;
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[1];
            TQPoint p( x, y );
            param[0].point = &p;
            if ( !pdev->cmd( TQPaintDevice::PdcDrawPoint, this, param ) ||
                 !hd )
                return;
        }
        map( x, y, &x, &y );
    }
    if ( cpen.style() != NoPen )
        XDrawPoint( dpy, hd, gc, x, y );
}


/*!
    Draws/plots an array of points, \a a, using the current pen.

    If \a index is non-zero (the default is zero) only points from \a
    index are drawn. If \a npoints is negative (the default) the rest
    of the points from \a index are drawn. If \a npoints is zero or
    greater, \a npoints points are drawn.

    \warning On X11, coordinates that do not fit into 16-bit signed
    values are truncated. This limitation is expected to go away in
    TQt 4.
*/

void TQPainter::drawPoints( const TQPointArray& a, int index, int npoints )
{
    if ( npoints < 0 )
        npoints = a.size() - index;
    if ( index + npoints > (int)a.size() )
        npoints = a.size() - index;
    if ( !isActive() || npoints < 1 || index < 0 )
        return;
    TQPointArray pa = a;
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[1];
            for (int i=0; i<npoints; i++) {
                TQPoint p( pa[index+i].x(), pa[index+i].y() );
                param[0].point = &p;
                if ( !pdev->cmd( TQPaintDevice::PdcDrawPoint, this, param ))
                    return;
            }
            if ( !hd ) return;
        }
        if ( txop != TxNone ) {
            pa = xForm( a, index, npoints );
            if ( pa.size() != a.size() ) {
                index = 0;
                npoints = pa.size();
            }
        }
    }
    if ( cpen.style() != NoPen )
        XDrawPoints( dpy, hd, gc, (XPoint*)(pa.shortPoints( index, npoints )),
                     npoints, CoordModeOrigin );
}


/*! \obsolete
    Sets the current pen position to \a (x, y)

    \sa lineTo(), pos()
*/

void TQPainter::moveTo( int x, int y )
{
    if ( !isActive() )
        return;
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[1];
            TQPoint p( x, y );
            param[0].point = &p;
            if ( !pdev->cmd( TQPaintDevice::PdcMoveTo, this, param ) || !hd )
                return;
        }
    }
    curPt = TQPoint( x, y );
}

/*! \obsolete
  Use drawLine() instead.

    Draws a line from the current pen position to \a (x, y) and sets
    \a (x, y) to be the new current pen position.

    \sa TQPen moveTo(), drawLine(), pos()
*/

void TQPainter::lineTo( int x, int y )
{
    if ( !isActive() )
        return;
    int cx = curPt.x(), cy = curPt.y();
    curPt = TQPoint( x, y );
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[1];
            TQPoint p( x, y );
            param[0].point = &p;
            if ( !pdev->cmd( TQPaintDevice::PdcLineTo, this, param ) || !hd )
                return;
        }
        map( x, y, &x, &y );
	map( cx,  cy,  &cx,  &cy );
    }
    if ( cpen.style() != NoPen )
        XDrawLine( dpy, hd, gc, cx, cy, x, y );
}

/*!
    Draws a line from (\a x1, \a y1) to (\a x2, \a y2) and sets the
    current pen position to (\a x2, \a y2).

    \sa pen()
*/

void TQPainter::drawLine( int x1, int y1, int x2, int y2 )
{
    if ( !isActive() )
        return;
    curPt = TQPoint( x2, y2 );
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[2];
            TQPoint p1(x1, y1), p2(x2, y2);
            param[0].point = &p1;
            param[1].point = &p2;
            if ( !pdev->cmd( TQPaintDevice::PdcDrawLine, this, param ) || !hd )
                return;
        }
        map( x1, y1, &x1, &y1 );
        map( x2, y2, &x2, &y2 );
    }
    if ( cpen.style() != NoPen )
        XDrawLine( dpy, hd, gc, x1, y1, x2, y2 );
}



/*!
    Draws a rectangle with upper left corner at \a (x, y) and with
    width \a w and height \a h.

    \sa TQPen, drawRoundRect()
*/

void TQPainter::drawRect( int x, int y, int w, int h )
{
    if ( !isActive() )
        return;
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[1];
            TQRect r( x, y, w, h );
            param[0].rect = &r;
            if ( !pdev->cmd( TQPaintDevice::PdcDrawRect, this, param ) || !hd )
                return;
        }
        if ( txop == TxRotShear ) {             // rotate/shear polygon
	    TQPointArray pa = xmat.mapToPolygon( TQRect(x, y, w, h) );
	    pa.resize( 5 );
	    pa.setPoint( 4, pa.point( 0 ) );
            drawPolyInternal( pa );
            return;
        }
        map( x, y, w, h, &x, &y, &w, &h );
    }
    if ( w <= 0 || h <= 0 ) {
        if ( w == 0 || h == 0 )
            return;
        fix_neg_rect( &x, &y, &w, &h );
    }
    if ( cbrush.style() != NoBrush ) {
        if ( cpen.style() == NoPen ) {
            XFillRectangle( dpy, hd, gc_brush, x, y, w, h );
            return;
        }
	int lw = cpen.width();
	int lw2 = (lw+1)/2;
        if ( w > lw && h > lw )
            XFillRectangle( dpy, hd, gc_brush, x+lw2, y+lw2, w-lw-1, h-lw-1 );
    }
    if ( cpen.style() != NoPen )
        XDrawRectangle( dpy, hd, gc, x, y, w-1, h-1 );
}

/*!
    \overload

    Draws a Windows focus rectangle with upper left corner at (\a x,
    \a y) and with width \a w and height \a h.

    This function draws a stippled XOR rectangle that is used to
    indicate keyboard focus (when TQApplication::style() is \c
    WindowStyle).

    \warning This function draws nothing if the coordinate system has
    been \link rotate() rotated\endlink or \link shear()
    sheared\endlink.

    \sa drawRect(), TQApplication::style()
*/

void TQPainter::drawWinFocusRect( int x, int y, int w, int h )
{
    drawWinFocusRect( x, y, w, h, true, color0 );
}

/*!
    Draws a Windows focus rectangle with upper left corner at (\a x,
    \a y) and with width \a w and height \a h using a pen color that
    contrasts with \a bgColor.

    This function draws a stippled rectangle (XOR is not used) that is
    used to indicate keyboard focus (when the TQApplication::style() is
    \c WindowStyle).

    The pen color used to draw the rectangle is either white or black
    depending on the color of \a bgColor (see TQColor::gray()).

    \warning This function draws nothing if the coordinate system has
    been \link rotate() rotated\endlink or \link shear()
    sheared\endlink.

    \sa drawRect(), TQApplication::style()
*/

void TQPainter::drawWinFocusRect( int x, int y, int w, int h,
                                 const TQColor &bgColor )
{
    drawWinFocusRect( x, y, w, h, false, bgColor );
}


/*!
  \internal
*/

void TQPainter::drawWinFocusRect( int x, int y, int w, int h,
                                 bool xorPaint, const TQColor &bgColor )
{
    if ( !isActive() || txop == TxRotShear )
        return;
    static char winfocus_line[] = { 1, 1 };

    TQPen     old_pen = cpen;
    TQBrush   old_brush = cbrush;
    RasterOp  old_rop = (RasterOp)rop;

    if ( xorPaint ) {
        if ( TQColor::numBitPlanes() <= 8 ) {
            setPen( TQPen(color1, 0, TQt::FineDotLine) );
        }
        else if ( TQColor::numBitPlanes() <= 8 ) {
            setPen( TQPen(white, 0, TQt::FineDotLine) );
        }
        else {
            setPen( TQPen(TQColor(tqRgba(255,255,255,0)), 0, TQt::FineDotLine) );
        }
        setRasterOp( XorROP );
    }
    else {
        if ( tqGray( bgColor.rgb() ) < 128 ) {
            setPen( TQPen(white, 0, TQt::FineDotLine) );
        }
        else {
            setPen( TQPen(black, 0, TQt::FineDotLine) );
        }
    }

    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[1];
            TQRect r( x, y, w-1, h-1 );
            TQBrush noBrush;
            setBrush( noBrush );
            param[0].rect = &r;
            if ( !pdev->cmd( TQPaintDevice::PdcDrawRect, this, param ) || !hd) {
                setRasterOp( old_rop );
                setPen( old_pen );
                setBrush( old_brush );
                return;
            }
        }
        map( x, y, w, h, &x, &y, &w, &h );
    }
    if ( w <= 0 || h <= 0 ) {
        if ( w == 0 || h == 0 )
            return;
        fix_neg_rect( &x, &y, &w, &h );
    }
    XSetDashes( dpy, gc, 0, winfocus_line, 2 );
    XSetLineAttributes( dpy, gc, 1, LineOnOffDash, CapButt, JoinMiter );

    XDrawRectangle( dpy, hd, gc, x, y, w-1, h-1 );

    XSetLineAttributes( dpy, gc, 0, LineSolid, CapButt, JoinMiter );
    setRasterOp( old_rop );
    setPen( old_pen );
}


/*!
    Draws a rectangle with rounded corners at \a (x, y), with width \a
    w and height \a h.

    The \a xRnd and \a yRnd arguments specify how rounded the corners
    should be. 0 is angled corners, 99 is maximum roundedness.

    The width and height include all of the drawn lines.

    \sa drawRect(), TQPen
*/

void TQPainter::drawRoundRect( int x, int y, int w, int h, int xRnd, int yRnd )
{
    if ( !isActive() )
        return;
    if ( xRnd <= 0 || yRnd <= 0 ) {
        drawRect( x, y, w, h );                 // draw normal rectangle
        return;
    }
    if ( xRnd >= 100 )                          // fix ranges
        xRnd = 99;
    if ( yRnd >= 100 )
        yRnd = 99;
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[3];
            TQRect r( x, y, w, h );
            param[0].rect = &r;
            param[1].ival = xRnd;
            param[2].ival = yRnd;
            if ( !pdev->cmd( TQPaintDevice::PdcDrawRoundRect, this, param ) ||
                 !hd )
                return;
        }
        if ( txop == TxRotShear ) {             // rotate/shear polygon
            if ( w <= 0 || h <= 0 )
                fix_neg_rect( &x, &y, &w, &h );
            w--;
            h--;
            int rxx = w*xRnd/200;
            int ryy = h*yRnd/200;
            // were there overflows?
            if ( rxx < 0 )
                rxx = w/200*xRnd;
            if ( ryy < 0 )
                ryy = h/200*yRnd;
            int rxx2 = 2*rxx;
            int ryy2 = 2*ryy;
            TQPointArray a[4];
            a[0].makeArc( x, y, rxx2, ryy2, 1*16*90, 16*90, xmat );
            a[1].makeArc( x, y+h-ryy2, rxx2, ryy2, 2*16*90, 16*90, xmat );
            a[2].makeArc( x+w-rxx2, y+h-ryy2, rxx2, ryy2, 3*16*90, 16*90, xmat );
            a[3].makeArc( x+w-rxx2, y, rxx2, ryy2, 0*16*90, 16*90, xmat );
            // ### is there a better way to join TQPointArrays?
            TQPointArray aa;
            aa.resize( a[0].size() + a[1].size() + a[2].size() + a[3].size() );
            uint j = 0;
            for ( int k=0; k<4; k++ ) {
                for ( uint i=0; i<a[k].size(); i++ ) {
                    aa.setPoint( j, a[k].point(i) );
                    j++;
                }
            }
            drawPolyInternal( aa );
            return;
        }
        map( x, y, w, h, &x, &y, &w, &h );
    }
    w--;
    h--;
    if ( w <= 0 || h <= 0 ) {
        if ( w == 0 || h == 0 )
            return;
        fix_neg_rect( &x, &y, &w, &h );
    }
    int rx = (w*xRnd)/200;
    int ry = (h*yRnd)/200;
    int rx2 = 2*rx;
    int ry2 = 2*ry;
    if ( cbrush.style() != NoBrush ) {          // draw filled round rect
        int dp, ds;
        if ( cpen.style() == NoPen ) {
            dp = 0;
            ds = 1;
        }
        else {
            dp = 1;
            ds = 0;
        }
#define SET_ARC(px, py, w, h, a1, a2) \
    a->x=px; a->y=py; a->width=w; a->height=h; a->angle1=a1; a->angle2=a2; a++
        XArc arcs[4];
        XArc *a = arcs;
        SET_ARC( x+w-rx2, y, rx2, ry2, 0, 90*64 );
        SET_ARC( x, y, rx2, ry2, 90*64, 90*64 );
        SET_ARC( x, y+h-ry2, rx2, ry2, 180*64, 90*64 );
        SET_ARC( x+w-rx2, y+h-ry2, rx2, ry2, 270*64, 90*64 );
        XFillArcs( dpy, hd, gc_brush, arcs, 4 );
#undef SET_ARC
#define SET_RCT(px, py, w, h) \
    r->x=px; r->y=py; r->width=w; r->height=h; r++
        XRectangle rects[3];
        XRectangle *r = rects;
        SET_RCT( x+rx, y+dp, w-rx2, ry );
        SET_RCT( x+dp, y+ry, w+ds, h-ry2 );
        SET_RCT( x+rx, y+h-ry, w-rx2, ry+ds );
        XFillRectangles( dpy, hd, gc_brush, rects, 3 );
#undef SET_RCT
    }
    if ( cpen.style() != NoPen ) {              // draw outline
#define SET_ARC(px, py, w, h, a1, a2) \
    a->x=px; a->y=py; a->width=w; a->height=h; a->angle1=a1; a->angle2=a2; a++
        XArc arcs[4];
        XArc *a = arcs;
        SET_ARC( x+w-rx2, y, rx2, ry2, 0, 90*64 );
        SET_ARC( x, y, rx2, ry2, 90*64, 90*64 );
        SET_ARC( x, y+h-ry2, rx2, ry2, 180*64, 90*64 );
        SET_ARC( x+w-rx2, y+h-ry2, rx2, ry2, 270*64, 90*64 );
        XDrawArcs( dpy, hd, gc, arcs, 4 );
#undef SET_ARC
#define SET_SEG(xp1, yp1, xp2, yp2) \
    s->x1=xp1; s->y1=yp1; s->x2=xp2; s->y2=yp2; s++
        XSegment segs[4];
        XSegment *s = segs;
        SET_SEG( x+rx, y, x+w-rx, y );
        SET_SEG( x+rx, y+h, x+w-rx, y+h );
        SET_SEG( x, y+ry, x, y+h-ry );
        SET_SEG( x+w, y+ry, x+w, y+h-ry );
        XDrawSegments( dpy, hd, gc, segs, 4 );
#undef SET_SET
    }
}

/*!
    Draws an ellipse with center at \a (x + w/2, y + h/2) and size \a
    (w, h).
*/

void TQPainter::drawEllipse( int x, int y, int w, int h )
{
    if ( !isActive() )
        return;
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[1];
            TQRect r( x, y, w, h );
            param[0].rect = &r;
            if ( !pdev->cmd( TQPaintDevice::PdcDrawEllipse, this, param ) ||
                 !hd )
                return;
        }
        if ( txop == TxRotShear ) {             // rotate/shear polygon
            TQPointArray a;
            a.makeArc( x, y, w, h, 0, 360*16, xmat );
            drawPolyInternal( a );
            return;
        }
        map( x, y, w, h, &x, &y, &w, &h );
    }
    if ( w <= 0 || h <= 0 ) {
        if ( w == 0 || h == 0 )
            return;
        fix_neg_rect( &x, &y, &w, &h );
    }
    if ( w == 1 && h == 1 ) {
	XDrawPoint( dpy, hd, (cpen.style() == NoPen)?gc_brush:gc, x, y );
	return;
    }
    w--;
    h--;
    if ( cbrush.style() != NoBrush ) {          // draw filled ellipse
        XFillArc( dpy, hd, gc_brush, x, y, w, h, 0, 360*64 );
        if ( cpen.style() == NoPen ) {
            XDrawArc( dpy, hd, gc_brush, x, y, w, h, 0, 360*64 );
            return;
        }
    }
    if ( cpen.style() != NoPen )                // draw outline
        XDrawArc( dpy, hd, gc, x, y, w, h, 0, 360*64 );
}


/*!
    Draws an arc defined by the rectangle \a (x, y, w, h), the start
    angle \a a and the arc length \a alen.

    The angles \a a and \a alen are 1/16th of a degree, i.e. a full
    circle equals 5760 (16*360). Positive values of \a a and \a alen
    mean counter-clockwise while negative values mean the clockwise
    direction. Zero degrees is at the 3 o'clock position.

    Example:
    \code
	TQPainter p( myWidget );
	p.drawArc( 10,10, 70,100, 100*16, 160*16 ); // draws a "(" arc
    \endcode

    \sa drawPie(), drawChord()
*/

void TQPainter::drawArc( int x, int y, int w, int h, int a, int alen )
{
    if ( !isActive() )
        return;
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[3];
            TQRect r( x, y, w, h );
            param[0].rect = &r;
            param[1].ival = a;
            param[2].ival = alen;
            if ( !pdev->cmd( TQPaintDevice::PdcDrawArc, this, param ) ||
                 !hd )
                return;
        }
        if ( txop == TxRotShear ) {             // rotate/shear
            TQPointArray pa;
            pa.makeArc( x, y, w, h, a, alen, xmat ); // arc polyline
            drawPolyInternal( pa, false );
            return;
        }
        map( x, y, w, h, &x, &y, &w, &h );
    }
    w--;
    h--;
    if ( w <= 0 || h <= 0 ) {
        if ( w == 0 || h == 0 )
            return;
        fix_neg_rect( &x, &y, &w, &h );
    }
    if ( cpen.style() != NoPen )
        XDrawArc( dpy, hd, gc, x, y, w, h, a*4, alen*4 );
}


/*!
    Draws a pie defined by the rectangle \a (x, y, w, h), the start
    angle \a a and the arc length \a alen.

    The pie is filled with the current brush().

    The angles \a a and \a alen are 1/16th of a degree, i.e. a full
    circle equals 5760 (16*360). Positive values of \a a and \a alen
    mean counter-clockwise while negative values mean the clockwise
    direction. Zero degrees is at the 3 o'clock position.

    \sa drawArc(), drawChord()
*/

void TQPainter::drawPie( int x, int y, int w, int h, int a, int alen )
{
    // Make sure "a" is 0..360*16, as otherwise a*4 may overflow 16 bits.
    if ( a > (360*16) ) {
        a = a % (360*16);
    } else if ( a < 0 ) {
        a = a % (360*16);
        if ( a < 0 ) a += (360*16);
    }

    if ( !isActive() )
        return;
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[3];
            TQRect r( x, y, w, h );
            param[0].rect = &r;
            param[1].ival = a;
            param[2].ival = alen;
            if ( !pdev->cmd( TQPaintDevice::PdcDrawPie, this, param ) || !hd )
                return;
        }
        if ( txop == TxRotShear ) {             // rotate/shear
            TQPointArray pa;
            pa.makeArc( x, y, w, h, a, alen, xmat ); // arc polyline
            int n = pa.size();
            int cx, cy;
            xmat.map(x+w/2, y+h/2, &cx, &cy);
            pa.resize( n+2 );
            pa.setPoint( n, cx, cy );   // add legs
            pa.setPoint( n+1, pa.at(0) );
            drawPolyInternal( pa );
            return;
        }
        map( x, y, w, h, &x, &y, &w, &h );
    }
    XSetArcMode( dpy, gc_brush, ArcPieSlice );
    w--;
    h--;
    if ( w <= 0 || h <= 0 ) {
        if ( w == 0 || h == 0 )
            return;
        fix_neg_rect( &x, &y, &w, &h );
    }

    GC g = gc;
    bool nopen = cpen.style() == NoPen;

    if ( cbrush.style() != NoBrush ) {          // draw filled pie
        XFillArc( dpy, hd, gc_brush, x, y, w, h, a*4, alen*4 );
        if ( nopen ) {
            g = gc_brush;
            nopen = false;
        }
    }
    if ( !nopen ) {                             // draw pie outline
        double w2 = 0.5*w;                      // with, height in ellipsis
        double h2 = 0.5*h;
        double xc = (double)x+w2;
        double yc = (double)y+h2;
        double ra1 = Q_PI/2880.0*a;             // convert a, alen to radians
        double ra2 = ra1 + Q_PI/2880.0*alen;
        int xic = tqRound(xc);
        int yic = tqRound(yc);
        XDrawLine( dpy, hd, g, xic, yic,
                   tqRound(xc + qcos(ra1)*w2), tqRound(yc - qsin(ra1)*h2));
        XDrawLine( dpy, hd, g, xic, yic,
                   tqRound(xc + qcos(ra2)*w2), tqRound(yc - qsin(ra2)*h2));
        XDrawArc( dpy, hd, g, x, y, w, h, a*4, alen*4 );
    }
}


/*!
    Draws a chord defined by the rectangle \a (x, y, w, h), the start
    angle \a a and the arc length \a alen.

    The chord is filled with the current brush().

    The angles \a a and \a alen are 1/16th of a degree, i.e. a full
    circle equals 5760 (16*360). Positive values of \a a and \a alen
    mean counter-clockwise while negative values mean the clockwise
    direction. Zero degrees is at the 3 o'clock position.

    \sa drawArc(), drawPie()
*/

void TQPainter::drawChord( int x, int y, int w, int h, int a, int alen )
{
    if ( !isActive() )
        return;
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[3];
            TQRect r( x, y, w, h );
            param[0].rect = &r;
            param[1].ival = a;
            param[2].ival = alen;
            if ( !pdev->cmd(TQPaintDevice::PdcDrawChord, this, param) || !hd )
                return;
        }
        if ( txop == TxRotShear ) {             // rotate/shear
            TQPointArray pa;
            pa.makeArc( x, y, w-1, h-1, a, alen, xmat ); // arc polygon
            int n = pa.size();
            pa.resize( n+1 );
            pa.setPoint( n, pa.at(0) );         // connect endpoints
            drawPolyInternal( pa );
            return;
        }
        map( x, y, w, h, &x, &y, &w, &h );
    }
    XSetArcMode( dpy, gc_brush, ArcChord );
    w--;
    h--;
    if ( w <= 0 || h <= 0 ) {
        if ( w == 0 || h == 0 )
            return;
        fix_neg_rect( &x, &y, &w, &h );
    }

    GC g = gc;
    bool nopen = cpen.style() == NoPen;

    if ( cbrush.style() != NoBrush ) {          // draw filled chord
        XFillArc( dpy, hd, gc_brush, x, y, w, h, a*4, alen*4 );
        if ( nopen ) {
            g = gc_brush;
            nopen = false;
        }
    }
    if ( !nopen ) {                             // draw chord outline
        double w2 = 0.5*w;                      // with, height in ellipsis
        double h2 = 0.5*h;
        double xc = (double)x+w2;
        double yc = (double)y+h2;
        double ra1 = Q_PI/2880.0*a;             // convert a, alen to radians
        double ra2 = ra1 + Q_PI/2880.0*alen;
        XDrawLine( dpy, hd, g,
                   tqRound(xc + qcos(ra1)*w2), tqRound(yc - qsin(ra1)*h2),
                   tqRound(xc + qcos(ra2)*w2), tqRound(yc - qsin(ra2)*h2));
        XDrawArc( dpy, hd, g, x, y, w, h, a*4, alen*4 );
    }
    XSetArcMode( dpy, gc_brush, ArcPieSlice );
}


/*!
    Draws \a nlines separate lines from points defined in \a a,
    starting at \a a[index] (\a index defaults to 0). If \a nlines is
    -1 (the default) all points until the end of the array are used
    (i.e. (a.size()-index)/2 lines are drawn).

    Draws the 1st line from \a a[index] to \a a[index+1]. Draws the
    2nd line from \a a[index+2] to \a a[index+3] etc.

    \warning On X11, coordinates that do not fit into 16-bit signed
    values are truncated. This limitation is expected to go away in
    TQt 4.

    \sa drawPolyline(), drawPolygon(), TQPen
*/

void TQPainter::drawLineSegments( const TQPointArray &a, int index, int nlines )
{
    if ( nlines < 0 )
        nlines = a.size()/2 - index/2;
    if ( index + nlines*2 > (int)a.size() )
        nlines = (a.size() - index)/2;
    if ( !isActive() || nlines < 1 || index < 0 )
        return;
    TQPointArray pa = a;
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            if ( 2*nlines != (int)pa.size() ) {
                pa = TQPointArray( nlines*2 );
                for ( int i=0; i<nlines*2; i++ )
                    pa.setPoint( i, a.point(index+i) );
                index = 0;
            }
            TQPDevCmdParam param[1];
            param[0].ptarr = (TQPointArray*)&pa;
            if ( !pdev->cmd(TQPaintDevice::PdcDrawLineSegments, this, param) ||
                 !hd )
                return;
        }
        if ( txop != TxNone ) {
            pa = xForm( a, index, nlines*2 );
            if ( pa.size() != a.size() ) {
                index  = 0;
                nlines = pa.size()/2;
            }
        }
    }
    if ( cpen.style() != NoPen )
        XDrawSegments( dpy, hd, gc,
                       (XSegment*)(pa.shortPoints( index, nlines*2 )), nlines );
}


/*!
    Draws the polyline defined by the \a npoints points in \a a
    starting at \a a[index]. (\a index defaults to 0.)

    If \a npoints is -1 (the default) all points until the end of the
    array are used (i.e. a.size()-index-1 line segments are drawn).

    \warning On X11, coordinates that do not fit into 16-bit signed
    values are truncated. This limitation is expected to go away in
    TQt 4.

    \sa drawLineSegments(), drawPolygon(), TQPen
*/

void TQPainter::drawPolyline( const TQPointArray &a, int index, int npoints )
{
    if ( npoints < 0 )
        npoints = a.size() - index;
    if ( index + npoints > (int)a.size() )
        npoints = a.size() - index;
    if ( !isActive() || npoints < 2 || index < 0 )
        return;
    TQPointArray pa = a;
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            if ( npoints != (int)pa.size() ) {
                pa = TQPointArray( npoints );
                for ( int i=0; i<npoints; i++ )
                    pa.setPoint( i, a.point(index+i) );
                index = 0;
            }
            TQPDevCmdParam param[1];
            param[0].ptarr = (TQPointArray*)&pa;
            if ( !pdev->cmd(TQPaintDevice::PdcDrawPolyline, this, param) || !hd )
                return;
        }
        if ( txop != TxNone ) {
            pa = xForm( pa, index, npoints );
            if ( pa.size() != a.size() ) {
                index   = 0;
                npoints = pa.size();
            }
        }
    }
    if ( cpen.style() != NoPen ) {
        while(npoints>65535) {
            XDrawLines( dpy, hd, gc, (XPoint*)(pa.shortPoints( index, 65535 )),
                        65535, CoordModeOrigin );
            npoints-=65535;
            index+=65535;
        }
        XDrawLines( dpy, hd, gc, (XPoint*)(pa.shortPoints( index, npoints )),
                    npoints, CoordModeOrigin );
    }
}

static int global_polygon_shape = Complex;

/*!
    Draws the polygon defined by the \a npoints points in \a a
    starting at \a a[index]. (\a index defaults to 0.)

    If \a npoints is -1 (the default) all points until the end of the
    array are used (i.e. a.size()-index line segments define the
    polygon).

    The first point is always connected to the last point.

    The polygon is filled with the current brush(). If \a winding is
    true, the polygon is filled using the winding fill algorithm. If
    \a winding is false, the polygon is filled using the even-odd
    (alternative) fill algorithm.

    \warning On X11, coordinates that do not fit into 16-bit signed
    values are truncated. This limitation is expected to go away in
    TQt 4.

    \sa drawLineSegments(), drawPolyline(), TQPen
*/

void TQPainter::drawPolygon( const TQPointArray &a, bool winding,
                            int index, int npoints )
{
    if ( npoints < 0 )
        npoints = a.size() - index;
    if ( index + npoints > (int)a.size() )
        npoints = a.size() - index;
    if ( !isActive() || npoints < 2 || index < 0 )
        return;
    TQPointArray pa = a;
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            if ( npoints != (int)a.size() ) {
                pa = TQPointArray( npoints );
                for ( int i=0; i<npoints; i++ )
                    pa.setPoint( i, a.point(index+i) );
                index = 0;
            }
            TQPDevCmdParam param[2];
            param[0].ptarr = (TQPointArray*)&pa;
            param[1].ival = winding;
            if ( !pdev->cmd(TQPaintDevice::PdcDrawPolygon, this, param) || !hd )
                return;
        }
        if ( txop != TxNone ) {
            pa = xForm( a, index, npoints );
            if ( pa.size() != a.size() ) {
                index   = 0;
                npoints = pa.size();
            }
        }
    }
    if ( winding )                              // set to winding fill rule
        XSetFillRule( dpy, gc_brush, WindingRule );

    if ( pa[index] != pa[index+npoints-1] ){   // close open pointarray
        pa.detach();
        pa.resize( index+npoints+1 );
        pa.setPoint( index+npoints, pa[index] );
        npoints++;
    }

    if ( cbrush.style() != NoBrush ) {          // draw filled polygon
        XFillPolygon( dpy, hd, gc_brush,
                      (XPoint*)(pa.shortPoints( index, npoints )),
                      npoints, global_polygon_shape, CoordModeOrigin );
    }
    if ( cpen.style() != NoPen ) {              // draw outline
        XDrawLines( dpy, hd, gc, (XPoint*)(pa.shortPoints( index, npoints )),
                    npoints, CoordModeOrigin );
    }
    if ( winding )                              // set to normal fill rule
        XSetFillRule( dpy, gc_brush, EvenOddRule );
}

/*!
    Draws the convex polygon defined by the \a npoints points in \a pa
    starting at \a pa[index] (\a index defaults to 0).

    If the supplied polygon is not convex, the results are undefined.

    On some platforms (e.g. X Window), this is faster than
    drawPolygon().

    \warning On X11, coordinates that do not fit into 16-bit signed
    values are truncated. This limitation is expected to go away in
    TQt 4.
*/
void TQPainter::drawConvexPolygon( const TQPointArray &pa,
                                  int index, int npoints )
{
    global_polygon_shape = Convex;
    drawPolygon(pa, false, index, npoints);
    global_polygon_shape = Complex;
}



/*!
    Draws a cubic Bezier curve defined by the control points in \a a,
    starting at \a a[index] (\a index defaults to 0).

    Control points after \a a[index + 3] are ignored. Nothing happens
    if there aren't enough control points.

    \warning On X11, coordinates that do not fit into 16-bit signed
    values are truncated. This limitation is expected to go away in
    TQt 4.
*/

void TQPainter::drawCubicBezier( const TQPointArray &a, int index )
{
    if ( !isActive() )
        return;
    if ( a.size() - index < 4 ) {
#if defined(QT_CHECK_RANGE)
        tqWarning( "TQPainter::drawCubicBezier: Cubic Bezier needs 4 control "
                  "points" );
#endif
        return;
    }
    TQPointArray pa( a );
    if ( index != 0 || a.size() > 4 ) {
        pa = TQPointArray( 4 );
        for ( int i=0; i<4; i++ )
            pa.setPoint( i, a.point(index+i) );
    }
    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            TQPDevCmdParam param[1];
            param[0].ptarr = (TQPointArray*)&pa;
            if ( !pdev->cmd(TQPaintDevice::PdcDrawCubicBezier, this, param) ||
                 !hd )
                return;
        }
        if ( txop != TxNone )
            pa = xForm( pa );
    }
    if ( cpen.style() != NoPen ) {
        pa = pa.cubicBezier();
        XDrawLines( dpy, hd, gc, (XPoint*)pa.shortPoints(), pa.size(),
                    CoordModeOrigin );
    }
}


/*!
    Draws a pixmap at \a (x, y) by copying a part of \a pixmap into
    the paint device.

    \a (x, y) specifies the top-left point in the paint device that is
    to be drawn onto. \a (sx, sy) specifies the top-left point in \a
    pixmap that is to be drawn. The default is (0, 0).

    \a (sw, sh) specifies the size of the pixmap that is to be drawn.
    The default, (-1, -1), means all the way to the bottom right of
    the pixmap.

    Currently the mask of the pixmap or it's alpha channel are ignored
    when painting on a TQPrinter.

    \sa bitBlt(), TQPixmap::setMask()
*/

void TQPainter::drawPixmap( int x, int y, const TQPixmap &pixmap,
                           int sx, int sy, int sw, int sh )
{
    if ( !isActive() || pixmap.isNull() )
        return;

    // right/bottom
    if ( sw < 0 )
        sw = pixmap.width()  - sx;
    if ( sh < 0 )
        sh = pixmap.height() - sy;

    // Sanity-check clipping
    if ( sx < 0 ) {
        x -= sx;
        sw += sx;
        sx = 0;
    }
    if ( sw + sx > pixmap.width() )
        sw = pixmap.width() - sx;
    if ( sy < 0 ) {
        y -= sy;
        sh += sy;
        sy = 0;
    }
    if ( sh + sy > pixmap.height() )
        sh = pixmap.height() - sy;

    if ( sw <= 0 || sh <= 0 )
        return;

    if ( pdev->x11Screen() != pixmap.x11Screen() ) {
        TQPixmap* p = (TQPixmap*) &pixmap;
        p->x11SetScreen( pdev->x11Screen() );
    }

    TQPixmap::x11SetDefaultScreen( pixmap.x11Screen() );

    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) || txop == TxScale || txop == TxRotShear ) {
            if ( sx != 0 || sy != 0 ||
                 sw != pixmap.width() || sh != pixmap.height() ) {
                TQPixmap tmp( sw, sh, pixmap.depth() );
                bitBlt( &tmp, 0, 0, &pixmap, sx, sy, sw, sh, CopyROP, true );
                if ( pixmap.mask() ) {
                    TQBitmap mask( sw, sh );
                    bitBlt( &mask, 0, 0, pixmap.mask(), sx, sy, sw, sh,
                            CopyROP, true );
                    tmp.setMask( mask );
                }
                drawPixmap( x, y, tmp );
                return;
            }
            if ( testf(ExtDev) ) {
                TQPDevCmdParam param[2];
                TQRect r(x, y, pixmap.width(), pixmap.height());
                param[0].rect  = &r;
                param[1].pixmap = &pixmap;
                if ( !pdev->cmd(TQPaintDevice::PdcDrawPixmap, this, param) || !hd )
                    return;
            }
            if ( txop == TxScale || txop == TxRotShear ) {
                TQWMatrix mat( m11(), m12(),
                              m21(), m22(),
                              dx(),  dy() );
                mat = TQPixmap::trueMatrix( mat, sw, sh );
                TQPixmap pm = pixmap.xForm( mat );
                if ( !pm.mask() && txop == TxRotShear ) {
                    TQBitmap bm_clip( sw, sh, 1 );
                    bm_clip.fill( color1 );
                    pm.setMask( bm_clip.xForm(mat) );
                }
                map( x, y, &x, &y );            // compute position of pixmap
                int dx, dy;
                mat.map( 0, 0, &dx, &dy );
                uint save_flags = flags;
                flags = IsActive | (save_flags & ClipOn);
                drawPixmap( x-dx, y-dy, pm );
                flags = save_flags;
                return;
            }
        }
        map( x, y, &x, &y );
    }

    TQBitmap *mask = (TQBitmap *)pixmap.mask();
    bool mono = pixmap.depth() == 1;

    if ( mask && !hasClipping() && pdev != paintEventDevice ) {
        if ( mono ) {                           // needs GCs pen color
            bool selfmask = pixmap.data->selfmask;
            if ( selfmask ) {
                XSetFillStyle( dpy, gc, FillStippled );
                XSetStipple( dpy, gc, pixmap.handle() );
            } else {
                XSetFillStyle( dpy, gc, FillOpaqueStippled );
                XSetStipple( dpy, gc, pixmap.handle() );
                XSetClipMask( dpy, gc, mask->handle() );
                XSetClipOrigin( dpy, gc, x-sx, y-sy );
            }
            XSetTSOrigin( dpy, gc, x-sx, y-sy );
            XFillRectangle( dpy, hd, gc, x, y, sw, sh );
            XSetTSOrigin( dpy, gc, 0, 0 );
            XSetFillStyle( dpy, gc, FillSolid );
            if ( !selfmask ) {
                if ( pdev == paintEventDevice && paintEventClipRegion ) {
                    x11SetClipRegion( dpy, gc, 0, rendhd, *paintEventClipRegion );
		} else {
                    x11ClearClipRegion(dpy, gc, 0, rendhd);
		}
            }
        } else {
            bitBlt( pdev, x, y, &pixmap, sx, sy, sw, sh, (RasterOp)rop );
        }
        return;
    }

    TQRegion rgn = crgn;

    if ( mask ) {                               // pixmap has clip mask
        // Implies that clipping is on, either explicit or implicit
        // Create a new mask that combines the mask with the clip region

        if ( pdev == paintEventDevice && paintEventClipRegion ) {
            if ( hasClipping() )
                rgn = rgn.intersect( *paintEventClipRegion );
            else
                rgn = *paintEventClipRegion;
        }

        TQBitmap *comb = new TQBitmap( sw, sh );
        comb->detach();
        GC cgc = tqt_xget_temp_gc( pixmap.x11Screen(), true );   // get temporary mono GC
        XSetForeground( dpy, cgc, 0 );
        XFillRectangle( dpy, comb->handle(), cgc, 0, 0, sw, sh );
        XSetBackground( dpy, cgc, 0 );
        XSetForeground( dpy, cgc, 1 );
	int num;
	XRectangle *rects = (XRectangle *)qt_getClipRects( rgn, num );
        XSetClipRectangles( dpy, cgc, -x, -y, rects, num, YXBanded );
        XSetFillStyle( dpy, cgc, FillOpaqueStippled );
        XSetStipple( dpy, cgc, mask->handle() );
        XSetTSOrigin( dpy, cgc, -sx, -sy );
        XFillRectangle( dpy, comb->handle(), cgc, 0, 0, sw, sh );
        XSetTSOrigin( dpy, cgc, 0, 0 );         // restore cgc
        XSetFillStyle( dpy, cgc, FillSolid );
        XSetClipMask( dpy, cgc, None );
        mask = comb;                            // it's deleted below

        XSetClipMask( dpy, gc, mask->handle() );
        XSetClipOrigin( dpy, gc, x, y );
    }

    if ( mono ) {
        XSetBackground( dpy, gc, bg_col.pixel(scrn) );
        XSetFillStyle( dpy, gc, FillOpaqueStippled );
        XSetStipple( dpy, gc, pixmap.handle() );
        XSetTSOrigin( dpy, gc, x-sx, y-sy );
        XFillRectangle( dpy, hd, gc, x, y, sw, sh );
        XSetTSOrigin( dpy, gc, 0, 0 );
        XSetFillStyle( dpy, gc, FillSolid );
    } else {
#if !defined(TQT_NO_XFTFREETYPE) && !defined(TQT_NO_XRENDER)
        Picture pict = rendhd ? XftDrawPicture((XftDraw *) rendhd) : None;
	TQPixmap *alpha = pixmap.data->alphapm;

	if ( pict && pixmap.x11RenderHandle() &&
	     alpha && alpha->x11RenderHandle()) {
	    XRenderComposite(dpy, PictOpOver, pixmap.x11RenderHandle(),
			     alpha->x11RenderHandle(), pict,
			     sx, sy, sx, sy, x, y, sw, sh);
	} else
#endif // !TQT_NO_XFTFREETYPE && !TQT_NO_XRENDER
        {
            XCopyArea( dpy, pixmap.handle(), hd, gc, sx, sy, sw, sh, x, y );
        }
    }

    if ( mask ) {                               // restore clipping
        XSetClipOrigin( dpy, gc, 0, 0 );
        XSetRegion( dpy, gc, rgn.handle() );
        delete mask;                            // delete comb, created above
    }
}


/* Internal, used by drawTiledPixmap */

static void drawTile( TQPainter *p, int x, int y, int w, int h,
                      const TQPixmap &pixmap, int xOffset, int yOffset )
{
    int yPos, xPos, drawH, drawW, yOff, xOff;
    yPos = y;
    yOff = yOffset;
    while( yPos < y + h ) {
        drawH = pixmap.height() - yOff;    // Cropping first row
        if ( yPos + drawH > y + h )        // Cropping last row
            drawH = y + h - yPos;
        xPos = x;
        xOff = xOffset;
        while( xPos < x + w ) {
            drawW = pixmap.width() - xOff; // Cropping first column
            if ( xPos + drawW > x + w )    // Cropping last column
                drawW = x + w - xPos;
            p->drawPixmap( xPos, yPos, pixmap, xOff, yOff, drawW, drawH );
            xPos += drawW;
            xOff = 0;
        }
        yPos += drawH;
        yOff = 0;
    }
}

#if 0 // see comment in drawTiledPixmap
/* Internal, used by drawTiledPixmap */

static void fillTile(  TQPixmap *tile, const TQPixmap &pixmap )
{
    bitBlt( tile, 0, 0, &pixmap, 0, 0, -1, -1, TQt::CopyROP, true );
    int x = pixmap.width();
    while ( x < tile->width() ) {
        bitBlt( tile, x,0, tile, 0,0, x,pixmap.height(), TQt::CopyROP, true );
        x *= 2;
    }
    int y = pixmap.height();
    while ( y < tile->height() ) {
        bitBlt( tile, 0,y, tile, 0,0, tile->width(),y, TQt::CopyROP, true );
        y *= 2;
    }
}
#endif

/*!
    Draws a tiled \a pixmap in the specified rectangle.

    \a (x, y) specifies the top-left point in the paint device that is
    to be drawn onto; with the width and height given by \a w and \a
    h. \a (sx, sy) specifies the top-left point in \a pixmap that is
    to be drawn. The default is (0, 0).

    Calling drawTiledPixmap() is similar to calling drawPixmap()
    several times to fill (tile) an area with a pixmap, but is
    potentially much more efficient depending on the underlying window
    system.

    \sa drawPixmap()
*/

void TQPainter::drawTiledPixmap( int x, int y, int w, int h,
                                const TQPixmap &pixmap, int sx, int sy )
{
    int sw = pixmap.width();
    int sh = pixmap.height();
    if (!sw || !sh )
        return;
    if ( sx < 0 )
        sx = sw - -sx % sw;
    else
        sx = sx % sw;
    if ( sy < 0 )
        sy = sh - -sy % sh;
    else
        sy = sy % sh;
    /*
      Requirements for optimizing tiled pixmaps:
      - not an external device
      - not scale or rotshear
      - not mono pixmap
      - no mask
    */
    TQBitmap *mask = (TQBitmap *)pixmap.mask();
    if ( !testf(ExtDev) && txop <= TxTranslate && pixmap.depth() > 1 &&
         mask == 0 ) {
        if ( txop == TxTranslate )
            map( x, y, &x, &y );

#if !defined(TQT_NO_XFTFREETYPE) && !defined(TQT_NO_XRENDER)
        Picture pict = rendhd ? XftDrawPicture((XftDraw *) rendhd) : None;
	TQPixmap *alpha = pixmap.data->alphapm;

        if (pict && pixmap.x11RenderHandle() && alpha && alpha->x11RenderHandle()) {
	    // this is essentially drawTile() from above, inlined for
	    // the XRenderComposite call
	    int yPos, xPos, drawH, drawW, yOff, xOff;
	    yPos = y;
	    yOff = sy;
	    while( yPos < y + h ) {
		drawH = pixmap.height() - yOff;    // Cropping first row
		if ( yPos + drawH > y + h )        // Cropping last row
		    drawH = y + h - yPos;
		xPos = x;
		xOff = sx;
		while( xPos < x + w ) {
		    drawW = pixmap.width() - xOff; // Cropping first column
		    if ( xPos + drawW > x + w )    // Cropping last column
			drawW = x + w - xPos;
		    XRenderComposite(dpy, PictOpOver, pixmap.x11RenderHandle(),
				     alpha->x11RenderHandle(), pict,
				     xOff, yOff, xOff, yOff, xPos, yPos, drawW, drawH);
		    xPos += drawW;
		    xOff = 0;
		}
		yPos += drawH;
		yOff = 0;
	    }
	    return;
	}
#endif // !TQT_NO_XFTFREETYPE && !TQT_NO_XRENDER

	XSetTile( dpy, gc, pixmap.handle() );
	XSetFillStyle( dpy, gc, FillTiled );
	XSetTSOrigin( dpy, gc, x-sx, y-sy );
	XFillRectangle( dpy, hd, gc, x, y, w, h );
	XSetTSOrigin( dpy, gc, 0, 0 );
	XSetFillStyle( dpy, gc, FillSolid );
	return;
    }

#if 0
    // maybe there'll be point in this again, but for the time all it
    // does is make trouble for the postscript code.
    if ( sw*sh < 8192 && sw*sh < 16*w*h ) {
        int tw = sw;
        int th = sh;
        while( th * tw < 4096 && ( th < h || tw < w ) ) {
            if ( h/th > w/tw )
                th *= 2;
            else
                tw *= 2;
        }
        TQPixmap tile( tw, th, pixmap.depth(), TQPixmap::NormalOptim );
        fillTile( &tile, pixmap );
        if ( mask ) {
            TQBitmap tilemask( tw, th, TQPixmap::NormalOptim );
            fillTile( &tilemask, *mask );
            tile.setMask( tilemask );
        }
        drawTile( this, x, y, w, h, tile, sx, sy );
    } else {
        drawTile( this, x, y, w, h, pixmap, sx, sy );
    }
#else
    // for now we'll just output the original and let the postscript
    // code make what it can of it.  qpicture will be unhappy.
    drawTile( this, x, y, w, h, pixmap, sx, sy );
#endif
}

#if 0
//
// Generate a string that describes a transformed bitmap. This string is used
// to insert and find bitmaps in the global pixmap cache.
//

static TQString gen_text_bitmap_key( const TQWMatrix &m, const TQFont &font,
                                    const TQString &str, int pos, int len )
{
    TQString fk = font.key();
    int sz = 4*2 + len*2 + fk.length()*2 + sizeof(double)*6;
    TQByteArray buf(sz);
    uchar *p = (uchar *)buf.data();
    *((double*)p)=m.m11();  p+=sizeof(double);
    *((double*)p)=m.m12();  p+=sizeof(double);
    *((double*)p)=m.m21();  p+=sizeof(double);
    *((double*)p)=m.m22();  p+=sizeof(double);
    *((double*)p)=m.dx();   p+=sizeof(double);
    *((double*)p)=m.dy();   p+=sizeof(double);
    TQChar h1( '$' );
    TQChar h2( 'q' );
    TQChar h3( 't' );
    TQChar h4( '$' );
    *((TQChar*)p)=h1;  p+=2;
    *((TQChar*)p)=h2;  p+=2;
    *((TQChar*)p)=h3;  p+=2;
    *((TQChar*)p)=h4;  p+=2;
    memcpy( (char*)p, (char*)(str.unicode()+pos), len*2 );  p += len*2;
    memcpy( (char*)p, (char*)fk.unicode(), fk.length()*2 ); p += fk.length()*2;
    return TQString( (TQChar*)buf.data(), buf.size()/2 );
}

static TQBitmap *get_text_bitmap( const TQString &key )
{
    return (TQBitmap*)TQPixmapCache::find( key );
}

static void ins_text_bitmap( const TQString &key, TQBitmap *bm )
{
    if ( !TQPixmapCache::insert(key, bm) )       // cannot insert pixmap
        delete bm;
}
#endif

void qt_draw_transformed_rect( TQPainter *p,  int x, int y, int w,  int h, bool fill )
{
    XPoint points[5];
    int xp = x,  yp = y;
    p->map( xp, yp, &xp, &yp );
    points[0].x = xp;
    points[0].y = yp;
    xp = x + w; yp = y;
    p->map( xp, yp, &xp, &yp );
    points[1].x = xp;
    points[1].y = yp;
    xp = x + w; yp = y + h;
    p->map( xp, yp, &xp, &yp );
    points[2].x = xp;
    points[2].y = yp;
    xp = x; yp = y + h;
    p->map( xp, yp, &xp, &yp );
    points[3].x = xp;
    points[3].y = yp;
    points[4] = points[0];

    if ( fill )
	XFillPolygon( p->dpy, p->hd, p->gc, points, 4, Convex, CoordModeOrigin );
    else
	XDrawLines( p->dpy, p->hd, p->gc, points, 5, CoordModeOrigin );
}

void qt_draw_background( TQPainter *p, int x, int y, int w,  int h )
{
    if (p->testf(TQPainter::ExtDev)) {
	if (p->pdev->devType() == TQInternal::Printer)
	    p->fillRect(x, y, w, h, p->bg_col);
	return;
    }
    XSetForeground( p->dpy, p->gc, p->bg_col.pixel(p->scrn) );
    qt_draw_transformed_rect( p, x, y, w, h, true);
    XSetForeground( p->dpy, p->gc, p->cpen.color().pixel(p->scrn) );
}

/*!
    Draws at most \a len characters of the string \a str at position
    \a (x, y).

    \a (x, y) is the base line position. Note that the meaning of \a y
    is not the same for the two drawText() varieties.
*/
void TQPainter::drawText( int x, int y, const TQString &str, int len, TQPainter::TextDirection dir )
{
    drawText( x, y, str, 0, len, dir );
}

/*!
  Draws at most \a len characters starting at position \a pos from the
  string \a str to position \a (x, y).

  \a (x, y) is the base line position. Note that the meaning of \a y
  is not the same for the two drawText() varieties.
*/
void TQPainter::drawText( int x, int y, const TQString &str, int pos, int len, TQPainter::TextDirection dir )
{
    if ( !isActive() )
        return;
    if (len < 0)
        len = str.length() - pos;
    if ( len <= 0 || pos >= (int)str.length() ) // empty string
        return;
    if ( pos + len > (int)str.length() )
        len = str.length() - pos;

    if ( testf(DirtyFont) ) {
	updateFont();
    }

    if ( testf(ExtDev) && pdev->devType() != TQInternal::Printer ) {
	TQPDevCmdParam param[3];
	TQPoint p(x, y);
	TQString string = str.mid( pos,  len );
	param[0].point = &p;
	param[1].str = &string;
	param[2].ival = TQFont::Latin;
	if ( !pdev->cmd(TQPaintDevice::PdcDrawText2, this, param) || !hd )
	    return;
    }

    bool simple = (dir == TQPainter::Auto) && str.simpleText();
    // we can't take the complete string here as we would otherwise
    // get quadratic behaviour when drawing long strings in parts.
    // we do however need some chars around the part we paint to get arabic shaping correct.
    // ### maybe possible to remove after cursor restrictions work in TQRT
    int start;
    int end;
    if ( simple ) {
	start = pos;
	end = pos+len;
    } else {
	start = TQMAX( 0,  pos - 8 );
	end = TQMIN( (int)str.length(), pos + len + 8 );
    }
    TQConstString cstr( str.unicode() + start, end - start );
    pos -= start;

    TQTextEngine engine( cstr.string(), pfont ? pfont->d : cfont.d );
    TQTextLayout layout( &engine );

    // this is actually what beginLayout does. Inlined here, so we can
    // avoid the bidi algorithm if we don't need it.
    engine.itemize( simple ? TQTextEngine::NoBidi|TQTextEngine::SingleLine : TQTextEngine::Full|TQTextEngine::SingleLine );
    engine.currentItem = 0;
    engine.firstItemInLine = -1;

    if ( dir != Auto ) {
	int level = dir == RTL ? 1 : 0;
	for ( int i = engine.items.size(); i >= 0; i-- )
	    engine.items[i].analysis.bidiLevel = level;
    }

    if ( !simple ) {
	layout.setBoundary( pos );
	layout.setBoundary( pos + len );
    }

    // small hack to force skipping of unneeded items
    start = 0;
    while ( engine.items[start].position < pos )
	++start;
    engine.currentItem = start;
    layout.beginLine( 0xfffffff );
    end = start;
    while ( !layout.atEnd() && layout.currentItem().from() < pos + len ) {
	layout.addCurrentItem();
	end++;
    }
    TQFontMetrics fm(fontMetrics());
    int ascent = fm.ascent(), descent = fm.descent();
    int left, right;
    layout.endLine( 0, 0, TQt::SingleLine|TQt::AlignLeft, &ascent, &descent, &left, &right );

    // do _not_ call endLayout() here, as it would clean up the shaped items and we would do shaping another time
    // for painting.

    int textFlags = 0;
    if ( cfont.d->underline ) textFlags |= TQt::Underline;
    if ( cfont.d->overline ) textFlags |= TQt::Overline;
    if ( cfont.d->strikeOut ) textFlags |= TQt::StrikeOut;

    if ( bg_mode == OpaqueMode )
	qt_draw_background( this, x, y-ascent, right-left, ascent+descent+1);

    for ( int i = start; i < end; i++ ) {
	TQTextItem ti;
	ti.item = i;
	ti.engine = &engine;

	drawTextItem( x, y - ascent, ti, textFlags );
    }
    layout.d = 0;
}


/*! \internal
    Draws the text item \a ti at position \a (x, y ).

    This method ignores the painters background mode and
    color. drawText and qt_format_text have to do it themselves, as
    only they know the extents of the complete string.

    It ignores the font set on the painter as the text item has one of its own.

    The underline and strikeout parameters of the text items font are
    ignored aswell. You'll need to pass in the correct flags to get
    underlining and strikeout.
*/
void TQPainter::drawTextItem( int x,  int y, const TQTextItem &ti, int textFlags )
{
    if ( testf(ExtDev) ) {
	TQPDevCmdParam param[2];
	TQPoint p(x, y);
	param[0].point = &p;
	param[1].textItem = &ti;
	bool retval = pdev->cmd(TQPaintDevice::PdcDrawTextItem, this, param);
	if ( !retval || !hd )
	    return;
    }

    TQTextEngine *engine = ti.engine;
    TQScriptItem *si = &engine->items[ti.item];

    engine->shape( ti.item );
    TQFontEngine *fe = si->fontEngine;
    assert( fe != 0 );

    x += si->x;
    y += si->y;

    fe->draw( this, x,  y, engine, si, textFlags );
}

#if TQT_VERSION >= 0x040000
#error "remove current position and associated methods"
#endif
/*!
  \obsolete
    Returns the current position of the  pen.

    \sa moveTo()
 */
TQPoint TQPainter::pos() const
{
    return curPt;
}
