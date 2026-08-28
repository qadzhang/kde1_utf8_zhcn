/****************************************************************************
**
** Implementation of TQPixmap class for X11
**
** Created : 940501
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

// NOT REVISED

#include "qplatformdefs.h"

#if defined(Q_OS_WIN32) && defined(QT_MITSHM)
#undef QT_MITSHM
#endif

#ifdef QT_MITSHM

// Use the MIT Shared Memory extension for pixmap<->image conversions
#define QT_MITSHM_CONVERSIONS

// Uncomment the next line to enable the MIT Shared Memory extension
// for TQPixmap::xForm()
//
// WARNING:  This has some problems:
//
//    1. Consumes a 800x600 pixmap
//    2. TQt does not handle the ShmCompletion message, so you will
//        get strange effects if you xForm() repeatedly.
//
// #define QT_MITSHM_XFORM

#else
#undef QT_MITSHM_CONVERSIONS
#undef QT_MITSHM_XFORM
#endif

#include "ntqbitmap.h"
#include "ntqpaintdevicemetrics.h"
#include "ntqimage.h"
#include "ntqwmatrix.h"
#include "ntqapplication.h"
#include "qt_x11_p.h"

#include <stdlib.h>

#if defined(Q_CC_MIPS)
#  define for if(0){}else for
#endif


/*!
  \class TQPixmap::TQPixmapData
  \brief The TQPixmap::TQPixmapData class is an internal class.
  \internal
*/


// For thread-safety:
//   image->data does not belong to X11, so we must free it ourselves.

inline static void qSafeXDestroyImage( XImage *x )
{
    if ( x->data ) {
	free( x->data );
	x->data = 0;
    }
    XDestroyImage( x );
}


/*****************************************************************************
  MIT Shared Memory Extension support: makes xForm noticeably (~20%) faster.
 *****************************************************************************/

#if defined(QT_MITSHM_XFORM)

static bool	       xshminit = false;
static XShmSegmentInfo xshminfo;
static XImage	      *xshmimg = 0;
static Pixmap	       xshmpm  = 0;

static void tqt_cleanup_mitshm()
{
    if ( xshmimg == 0 )
	return;
    Display *dpy = TQPaintDevice::x11AppDisplay();
    if ( xshmpm ) {
	XFreePixmap( dpy, xshmpm );
	xshmpm = 0;
    }
    XShmDetach( dpy, &xshminfo ); xshmimg->data = 0;
    qSafeXDestroyImage( xshmimg ); xshmimg = 0;
    shmdt( xshminfo.shmaddr );
    shmctl( xshminfo.shmid, IPC_RMID, 0 );
}


static bool tqt_create_mitshm_buffer( const TQPaintDevice* dev, int w, int h )
{
    static int major, minor;
    static Bool pixmaps_ok;
    Display *dpy = dev->x11Display();
    int dd	 = dev->x11Depth();
    Visual *vis	 = (Visual*)dev->x11Visual();

    if ( xshminit ) {
	tqt_cleanup_mitshm();
    } else {
	if ( !XShmQueryVersion(dpy, &major, &minor, &pixmaps_ok) )
	    return false;			// MIT Shm not supported
	tqAddPostRoutine( tqt_cleanup_mitshm );
	xshminit = true;
    }

    xshmimg = XShmCreateImage( dpy, vis, dd, ZPixmap, 0, &xshminfo, w, h );
    if ( !xshmimg )
	return false;

    bool ok;
    xshminfo.shmid = shmget( IPC_PRIVATE,
			     xshmimg->bytes_per_line * xshmimg->height,
			     IPC_CREAT | 0777 );
    ok = xshminfo.shmid != -1;
    if ( ok ) {
	xshmimg->data = (char*)shmat( xshminfo.shmid, 0, 0 );
	xshminfo.shmaddr = xshmimg->data;
	ok = ( xshminfo.shmaddr != (char*)-1 );
    }
    xshminfo.readOnly = false;
    if ( ok )
	ok = XShmAttach( dpy, &xshminfo );
    if ( !ok ) {
	qSafeXDestroyImage( xshmimg );
	xshmimg = 0;
	if ( xshminfo.shmaddr )
	    shmdt( xshminfo.shmaddr );
	if ( xshminfo.shmid != -1 )
	    shmctl( xshminfo.shmid, IPC_RMID, 0 );
	return false;
    }
    if ( pixmaps_ok )
	xshmpm = XShmCreatePixmap( dpy, DefaultRootWindow(dpy), xshmimg->data,
				   &xshminfo, w, h, dd );

    return true;
}

#else

// If extern, need a dummy.
//
// static bool tqt_create_mitshm_buffer( TQPaintDevice*, int, int )
// {
//     return false;
// }

#endif // QT_MITSHM_XFORM

#ifdef QT_MITSHM_CONVERSIONS

static bool tqt_mitshm_error = false;
static int tqt_mitshm_errorhandler( Display*, XErrorEvent* )
{
    tqt_mitshm_error = true;
    return 0;
}

static XImage* tqt_XShmCreateImage( Display* dpy, Visual* visual, unsigned int depth,
    int format, int /*offset*/, char* /*data*/, unsigned int width, unsigned int height,
    int /*bitmap_pad*/, int /*bytes_per_line*/, XShmSegmentInfo* shminfo )
{
    if( width * height * depth < 100*100*32 )
        return NULL;
    static int shm_inited = -1;
    if( shm_inited == -1 ) {
        if( XShmQueryExtension( dpy ))
            shm_inited = 1;
        else
            shm_inited = 0;
    }
    if( shm_inited == 0 )
        return NULL;
    XImage* xi = XShmCreateImage( dpy, visual, depth, format, NULL, shminfo, width,
        height );
    if( xi == NULL )
        return NULL;
    shminfo->shmid = shmget( IPC_PRIVATE, xi->bytes_per_line * xi->height,
        IPC_CREAT|0600);
    if( shminfo->shmid < 0 ) {
        XDestroyImage( xi );
        return NULL;
    }
    shminfo->readOnly = False;
    shminfo->shmaddr = (char*)shmat( shminfo->shmid, 0, 0 );
    if( shminfo->shmaddr == (char*)-1 ) {
        XDestroyImage( xi );
        shmctl( shminfo->shmid, IPC_RMID, 0 );
        return NULL;
    }
    xi->data = shminfo->shmaddr;
#ifndef QT_MITSHM_RMID_IGNORES_REFCOUNT
    // mark as deleted to automatically free the memory in case
    // of a crash (but this doesn't work e.g. on Solaris)
    shmctl( shminfo->shmid, IPC_RMID, 0 );
#endif
    if( shm_inited == 1 ) { // first time
        XErrorHandler old_h = XSetErrorHandler( tqt_mitshm_errorhandler );
        XShmAttach( dpy, shminfo );
        shm_inited = 2;
        XSync( dpy, False );
        XSetErrorHandler( old_h );
        if( tqt_mitshm_error ) { // oops ... perhaps we are remote?
            shm_inited = 0;
            XDestroyImage( xi );
            shmdt( shminfo->shmaddr );
#ifdef QT_MITSHM_RMID_IGNORES_REFCOUNT
            shmctl( shminfo->shmid, IPC_RMID, 0 );
#endif    
            return NULL;
        }
    } else
        XShmAttach( dpy, shminfo );
    return xi;
}

static void tqt_XShmDestroyImage( XImage* xi, XShmSegmentInfo* shminfo )
{
    XShmDetach( TQPaintDevice::x11AppDisplay(), shminfo );
    XDestroyImage( xi );
    shmdt( shminfo->shmaddr );
#ifdef QT_MITSHM_RMID_IGNORES_REFCOUNT
    shmctl( shminfo->shmid, IPC_RMID, 0 );
#endif    
}

static XImage* tqt_XShmGetImage( const TQPixmap* pix, int format,
    XShmSegmentInfo* shminfo )
{
    XImage* xi = tqt_XShmCreateImage( pix->x11Display(), (Visual*)pix->x11Visual(),
        pix->depth(), format, 0, 0, pix->width(), pix->height(), 32, 0, shminfo );
    if( xi == NULL )
        return NULL;
    if( XShmGetImage( pix->x11Display(), pix->handle(), xi, 0, 0, AllPlanes ) == False ) {
        tqt_XShmDestroyImage( xi, shminfo );
        return NULL;
    }
    return xi;
}

#endif // QT_MITSHM_CONVERSIONS

/*****************************************************************************
  Internal functions
 *****************************************************************************/

extern const uchar *tqt_get_bitflip_array();		// defined in qimage.cpp

static uchar *flip_bits( const uchar *bits, int len )
{
    const uchar *p = bits;
    const uchar *end = p + len;
    uchar *newdata = new uchar[len];
    uchar *b = newdata;
    const uchar *f = tqt_get_bitflip_array();
    while ( p < end )
	*b++ = f[*p++];
    return newdata;
}

// Returns position of highest bit set or -1 if none
static int highest_bit( uint v )
{
    int i;
    uint b = (uint)1 << 31;
    for ( i=31; ((b & v) == 0) && i>=0;	 i-- )
	b >>= 1;
    return i;
}

// Returns position of lowest set bit in 'v' as an integer (0-31), or -1
static int lowest_bit( uint v )
{
    int i;
    ulong lb;
    lb = 1;
    for (i=0; ((v & lb) == 0) && i<32;  i++, lb<<=1);
    return i==32 ? -1 : i;
}

// Counts the number of bits set in 'v'
static uint n_bits( uint v )
{
    int i = 0;
    while ( v ) {
	v = v & (v - 1);
	i++;
    }
    return i;
}

static uint *red_scale_table   = 0;
static uint *green_scale_table = 0;
static uint *blue_scale_table  = 0;

static void cleanup_scale_tables()
{
    delete[] red_scale_table;
    delete[] green_scale_table;
    delete[] blue_scale_table;
}

/*
  Could do smart bitshifting, but the "obvious" algorithm only works for
  nBits >= 4. This is more robust.
*/
static void build_scale_table( uint **table, uint nBits )
{
    if ( nBits > 7 ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "build_scale_table: internal error, nBits = %i", nBits );
#endif
	return;
    }
    if (!*table) {
	static bool firstTable = true;
	if ( firstTable ) {
	    tqAddPostRoutine( cleanup_scale_tables );
	    firstTable = false;
	}
	*table = new uint[256];
    }
    int   maxVal   = (1 << nBits) - 1;
    int   valShift = 8 - nBits;
    int i;
    for( i = 0 ; i < maxVal + 1 ; i++ )
	(*table)[i << valShift] = i*255/maxVal;
}

static int defaultScreen = -1;

extern bool tqt_use_xrender; // defined in qapplication_x11.cpp
extern bool tqt_has_xft;     // defined in qfont_x11.cpp

/*****************************************************************************
  TQPixmap member functions
 *****************************************************************************/

/*!
  \internal
  Initializes the pixmap data.
*/

void TQPixmap::init( int w, int h, int d, bool bitmap, Optimization optim )
{
#if defined(QT_CHECK_STATE)
    if ( tqApp->type() == TQApplication::Tty ) {
	tqWarning( "TQPixmap: Cannot create a TQPixmap when no GUI "
		  "is being used" );
    }
#endif

    static int serial = 0;

    if ( defaultScreen >= 0 && defaultScreen != x11Screen() ) {
	TQPaintDeviceX11Data* xd = getX11Data( true );
	xd->x_screen = defaultScreen;
	xd->x_depth = TQPaintDevice::x11AppDepth( xd->x_screen );
	xd->x_cells = TQPaintDevice::x11AppCells( xd->x_screen );
	xd->x_colormap = TQPaintDevice::x11AppColormap( xd->x_screen );
	xd->x_defcolormap = TQPaintDevice::x11AppDefaultColormap( xd->x_screen );
	xd->x_visual = TQPaintDevice::x11AppVisual( xd->x_screen );
	xd->x_defvisual = TQPaintDevice::x11AppDefaultVisual( xd->x_screen );
	setX11Data( xd );
    }

    int dd = x11Depth();

    if ( d != -1 )
        dd = d;

    if ( optim == DefaultOptim )		// use default optimization
	optim = defOptim;

    data = new TQPixmapData;
    TQ_CHECK_PTR( data );

    memset( data, 0, sizeof(TQPixmapData) );
    data->count  = 1;
    data->uninit = true;
    data->bitmap = bitmap;
    data->ser_no = ++serial;
    data->optim	 = optim;

    bool make_null = w == 0 || h == 0;		// create null pixmap
    if ( d == 1 )				// monocrome pixmap
	data->d = 1;
    else if ( d < 0 || d == dd )		// def depth pixmap
	data->d = dd;
    if ( make_null || w < 0 || h < 0 || data->d == 0 ) {
	hd = 0;
	rendhd = 0;
#if defined(QT_CHECK_RANGE)
	if ( !make_null )
	    tqWarning( "TQPixmap: Invalid pixmap parameters" );
#endif
	return;
    }
    data->w = w;
    data->h = h;
    hd = (HANDLE)XCreatePixmap( x11Display(), RootWindow(x11Display(), x11Screen() ),
				w, h, data->d );

#ifndef TQT_NO_XFTFREETYPE
    if ( tqt_has_xft ) {
	if ( data->d == 1 ) {
	    rendhd = (HANDLE) XftDrawCreateBitmap( x11Display(), hd );
	} else {
	    rendhd = (HANDLE) XftDrawCreate( x11Display(), hd,
					     (Visual *) x11Visual(),
					     x11Colormap() );
	}
    }
#endif // TQT_NO_XFTFREETYPE

}


void TQPixmap::deref()
{
    if ( data && data->deref() ) {			// last reference lost
	delete data->mask;
	delete data->alphapm;
	if ( data->ximage )
	    qSafeXDestroyImage( (XImage*)data->ximage );
	if ( data->maskgc )
	    XFreeGC( x11Display(), (GC)data->maskgc );
	if ( tqApp && hd) {

#ifndef TQT_NO_XFTFREETYPE
	    if (rendhd) {
		XftDrawDestroy( (XftDraw *) rendhd );
		rendhd = 0;
	    }
#endif // TQT_NO_XFTFREETYPE

	    XFreePixmap( x11Display(), hd );
	    hd = 0;
	}
	delete data;
    }
}


/*!
    Constructs a monochrome pixmap, with width \a w and height \a h,
    that is initialized with the data in \a bits. The \a isXbitmap
    indicates whether the data is an X bitmap and defaults to false.
    This constructor is protected and used by the TQBitmap class.
*/

TQPixmap::TQPixmap( int w, int h, const uchar *bits, bool isXbitmap)
    : TQPaintDevice( TQInternal::Pixmap )
{						// for bitmaps only
    init( 0, 0, 0, false, defOptim );
    if ( w <= 0 || h <= 0 )			// create null pixmap
	return;

    data->uninit = false;
    data->w = w;
    data->h = h;
    data->d = 1;
    uchar *flipped_bits;
    if ( isXbitmap ) {
	flipped_bits = 0;
    } else {					// not X bitmap -> flip bits
	flipped_bits = flip_bits( bits, ((w+7)/8)*h );
	bits = flipped_bits;
    }
    hd = (HANDLE)XCreateBitmapFromData( x11Display(),
					RootWindow(x11Display(), x11Screen() ),
					(char *)bits, w, h );

#ifndef TQT_NO_XFTFREETYPE
    if ( tqt_has_xft )
	rendhd = (HANDLE) XftDrawCreateBitmap (x11Display (), hd);
#endif // TQT_NO_XFTFREETYPE

    if ( flipped_bits )				// Avoid purify complaint
	delete [] flipped_bits;
}


/*!
    This is a special-purpose function that detaches the pixmap from
    shared pixmap data.

    A pixmap is automatically detached by TQt whenever its contents is
    about to change. This is done in all TQPixmap member functions
    that modify the pixmap (fill(), resize(), convertFromImage(),
    load(), etc.), in bitBlt() for the destination pixmap and in
    TQPainter::begin() on a pixmap.

    It is possible to modify a pixmap without letting TQt know. You can
    first obtain the system-dependent handle() and then call
    system-specific functions (for instance, BitBlt under Windows)
    that modify the pixmap contents. In such cases, you can call
    detach() to cut the pixmap loose from other pixmaps that share
    data with this one.

    detach() returns immediately if there is just a single reference
    or if the pixmap has not been initialized yet.
*/

void TQPixmap::detach()
{
    if ( data->count != 1 )
        *this = copy();
    data->uninit = false;

    // reset cached data
    if ( data->ximage ) {
        qSafeXDestroyImage( (XImage*)data->ximage );
        data->ximage = 0;
    }
    if ( data->maskgc ) {
        XFreeGC( x11Display(), (GC)data->maskgc );
        data->maskgc = 0;
    }
}


/*!
    Returns the default pixmap depth, i.e. the depth a pixmap gets if
    -1 is specified.

    \sa depth()
*/

int TQPixmap::defaultDepth()
{
    return x11AppDepth();
}


/*!
    \fn TQPixmap::Optimization TQPixmap::optimization() const

    Returns the optimization setting for this pixmap.

    The default optimization setting is \c TQPixmap::NormalOptim. You
    can change this setting in two ways:
    \list
    \i Call setDefaultOptimization() to set the default optimization
    for all new pixmaps.
    \i Call setOptimization() to set the optimization for individual
    pixmaps.
    \endlist

    \sa setOptimization(), setDefaultOptimization(), defaultOptimization()
*/

/*!
    Sets pixmap drawing optimization for this pixmap.

    The \a optimization setting affects pixmap operations, in
    particular drawing of transparent pixmaps (bitBlt() a pixmap with
    a mask set) and pixmap transformations (the xForm() function).

    Pixmap optimization involves keeping intermediate results in a
    cache buffer and using the cache to speed up bitBlt() and xForm().
    The cost is more memory consumption, up to twice as much as an
    unoptimized pixmap.

    Use the setDefaultOptimization() to change the default
    optimization for all new pixmaps.

    \sa optimization(), setDefaultOptimization(), defaultOptimization()
*/

void TQPixmap::setOptimization( Optimization optimization )
{
    if ( optimization == data->optim )
	return;
    detach();
    data->optim = optimization == DefaultOptim ?
	    defOptim : optimization;
    if ( data->optim == MemoryOptim && data->ximage ) {
	qSafeXDestroyImage( (XImage*)data->ximage );
	data->ximage = 0;
    }
}


/*!
    Fills the pixmap with the color \a fillColor.
*/

void TQPixmap::fill( const TQColor &fillColor )
{
    if ( isNull() )
	return;
    detach();					// detach other references
    GC gc = tqt_xget_temp_gc( x11Screen(), depth()==1 );
    XSetForeground( x11Display(), gc, fillColor.pixel(x11Screen()) );
    XFillRectangle( x11Display(), hd, gc, 0, 0, width(), height() );
}


/*!
  Internal implementation of the virtual TQPaintDevice::metric() function.

  Use the TQPaintDeviceMetrics class instead.

  \a m is the metric to get.
*/

int TQPixmap::metric( int m ) const
{
    int val;
    if ( m == TQPaintDeviceMetrics::PdmWidth )
	val = width();
    else if ( m == TQPaintDeviceMetrics::PdmHeight ) {
	val = height();
    } else {
	Display *dpy = x11Display();
	int scr = x11Screen();
	switch ( m ) {
	    case TQPaintDeviceMetrics::PdmDpiX:
	    case TQPaintDeviceMetrics::PdmPhysicalDpiX:
		val = TQPaintDevice::x11AppDpiX( scr );
		break;
	    case TQPaintDeviceMetrics::PdmDpiY:
	    case TQPaintDeviceMetrics::PdmPhysicalDpiY:
		val = TQPaintDevice::x11AppDpiY( scr );
		break;
	    case TQPaintDeviceMetrics::PdmWidthMM:
		val = (DisplayWidthMM(dpy,scr)*width())/
		      DisplayWidth(dpy,scr);
		break;
	    case TQPaintDeviceMetrics::PdmHeightMM:
		val = (DisplayHeightMM(dpy,scr)*height())/
		      DisplayHeight(dpy,scr);
		break;
	    case TQPaintDeviceMetrics::PdmNumColors:
		val = 1 << depth();
		break;
	    case TQPaintDeviceMetrics::PdmDepth:
		val = depth();
		break;
	    default:
		val = 0;
#if defined(QT_CHECK_RANGE)
		tqWarning( "TQPixmap::metric: Invalid metric command" );
#endif
	}
    }
    return val;
}

/*!
    Converts the pixmap to a TQImage. Returns a null image if it fails.

    If the pixmap has 1-bit depth, the returned image will also be 1
    bit deep. If the pixmap has 2- to 8-bit depth, the returned image
    has 8-bit depth. If the pixmap has greater than 8-bit depth, the
    returned image has 32-bit depth.

    Note that for the moment, alpha masks on monochrome images are
    ignored.

    \sa convertFromImage()
*/

TQImage TQPixmap::convertToImage() const
{
    TQImage image;
    if ( isNull() )
	return image; // null image

    int	    w  = width();
    int	    h  = height();
    int	    d  = depth();
    bool    mono = d == 1;
    Visual *visual = (Visual *)x11Visual();
    bool    trucol = (visual->c_class == TrueColor || visual->c_class == DirectColor) && !mono && d > 8;

    if ( d > 1 && d <= 8 )			// set to nearest valid depth
	d = 8;					//   2..8 ==> 8
    // we could run into the situation where d == 8 AND trucol is true, which can
    // cause problems when converting to and from images.  in this case, always treat
    // the depth as 32... from Klaus Schmidinger and qt-bugs/arc-15/31333.
    if ( d > 8 || trucol )
	d = 32;					//   > 8  ==> 32

    XImage *xi = (XImage *)data->ximage;	// any cached ximage?
#ifdef QT_MITSHM_CONVERSIONS
    bool mitshm_ximage = false;
    XShmSegmentInfo shminfo;
#endif
    if ( !xi ) {				// fetch data from X server
#ifdef QT_MITSHM_CONVERSIONS
        xi = tqt_XShmGetImage( this, mono ? XYPixmap : ZPixmap, &shminfo );
        if( xi ) {
            mitshm_ximage = true;
        } else
#endif
	xi = XGetImage( x11Display(), hd, 0, 0, w, h, AllPlanes,
			mono ? XYPixmap : ZPixmap );
    }
    TQ_CHECK_PTR( xi );
    if (!xi)
        return image; // null image

    TQImage::Endian bitOrder = TQImage::IgnoreEndian;
    if ( mono ) {
	bitOrder = xi->bitmap_bit_order == LSBFirst ?
		   TQImage::LittleEndian : TQImage::BigEndian;
    }
    image.create( w, h, d, 0, bitOrder );
    if ( image.isNull() ) {			// could not create image
#ifdef QT_MITSHM_CONVERSIONS
        if( mitshm_ximage )
            tqt_XShmDestroyImage( xi, &shminfo );
        else
#endif
        qSafeXDestroyImage( xi );
	((TQPixmap*)this)->data->ximage = 0;
	return image;
    }

    const TQPixmap* msk = mask();
    const TQPixmap *alf = data->alphapm;

    TQImage alpha;
    if (alf) {
        XImage* axi;
#ifdef QT_MITSHM_CONVERSIONS
        bool mitshm_aximage = false;
        XShmSegmentInfo ashminfo;
        axi = tqt_XShmGetImage( alf, ZPixmap, &ashminfo );
        if( axi ) {
            mitshm_aximage = true;
        } else
#endif
            axi = XGetImage(x11Display(), alf->hd, 0, 0, w, h, AllPlanes, ZPixmap);

	if (axi) {
	    image.setAlphaBuffer( true );
	    alpha.create(w, h, 8);

	    // copy each scanline
	    char *src = axi->data;
	    int bpl = TQMIN(alpha.bytesPerLine(), axi->bytes_per_line);
	    for (int y = 0; y < h; y++ ) {
		memcpy( alpha.scanLine(y), src, bpl );
		src += axi->bytes_per_line;
	    }

#ifdef QT_MITSHM_CONVERSIONS
            if( mitshm_aximage )
                tqt_XShmDestroyImage( axi, &ashminfo );
            else
#endif
	    qSafeXDestroyImage( axi );
	}
    } else if (msk) {
	image.setAlphaBuffer( true );
	alpha = msk->convertToImage();
    }
    bool ale = alpha.bitOrder() == TQImage::LittleEndian;

    if ( trucol ) {				// truecolor
	const uint red_mask	 = (uint)visual->red_mask;
	const uint green_mask	 = (uint)visual->green_mask;
	const uint blue_mask	 = (uint)visual->blue_mask;
	const int  red_shift	 = highest_bit( red_mask )   - 7;
	const int  green_shift = highest_bit( green_mask ) - 7;
	const int  blue_shift	 = highest_bit( blue_mask )  - 7;

	const uint red_bits    = n_bits( red_mask );
	const uint green_bits  = n_bits( green_mask );
	const uint blue_bits   = n_bits( blue_mask );

	static uint red_table_bits   = 0;
	static uint green_table_bits = 0;
	static uint blue_table_bits  = 0;

	if ( red_bits < 8 && red_table_bits != red_bits) {
	    build_scale_table( &red_scale_table, red_bits );
	    red_table_bits = red_bits;
	}
	if ( blue_bits < 8 && blue_table_bits != blue_bits) {
	    build_scale_table( &blue_scale_table, blue_bits );
	    blue_table_bits = blue_bits;
	}
	if ( green_bits < 8 && green_table_bits != green_bits) {
	    build_scale_table( &green_scale_table, green_bits );
	    green_table_bits = green_bits;
	}

	int  r, g, b;

	TQRgb  *dst;
	uchar *src;
	uint   pixel;
	int    bppc = xi->bits_per_pixel;

	if ( bppc > 8 && xi->byte_order == LSBFirst )
	    bppc++;

	for ( int y=0; y<h; y++ ) {
	    uchar* asrc = alf || msk ? alpha.scanLine( y ) : 0;
	    dst = (TQRgb *)image.scanLine( y );
	    src = (uchar *)xi->data + xi->bytes_per_line*y;
	    for ( int x=0; x<w; x++ ) {
		switch ( bppc ) {
		case 8:
		    pixel = *src++;
		    break;
		case 16:			// 16 bit MSB
		    pixel = src[1] | (ushort)src[0] << 8;
		    src += 2;
		    break;
		case 17:			// 16 bit LSB
		    pixel = src[0] | (ushort)src[1] << 8;
		    src += 2;
		    break;
		case 24:			// 24 bit MSB
		    pixel = src[2] | (ushort)src[1] << 8 |
			    (uint)src[0] << 16;
		    src += 3;
		    break;
		case 25:			// 24 bit LSB
		    pixel = src[0] | (ushort)src[1] << 8 |
			    (uint)src[2] << 16;
		    src += 3;
		    break;
		case 32:			// 32 bit MSB
		    pixel = src[3] | (ushort)src[2] << 8 |
			    (uint)src[1] << 16 | (uint)src[0] << 24;
		    src += 4;
		    break;
		case 33:			// 32 bit LSB
		    pixel = src[0] | (ushort)src[1] << 8 |
			    (uint)src[2] << 16 | (uint)src[3] << 24;
		    src += 4;
		    break;
		default:			// should not really happen
		    x = w;			// leave loop
		    y = h;
		    pixel = 0;		// eliminate compiler warning
#if defined(QT_CHECK_RANGE)
		    tqWarning( "TQPixmap::convertToImage: Invalid depth %d",
			      bppc );
#endif
		}
		if ( red_shift > 0 )
		    r = (pixel & red_mask) >> red_shift;
		else
		    r = (pixel & red_mask) << -red_shift;
		if ( green_shift > 0 )
		    g = (pixel & green_mask) >> green_shift;
		else
		    g = (pixel & green_mask) << -green_shift;
		if ( blue_shift > 0 )
		    b = (pixel & blue_mask) >> blue_shift;
		else
		    b = (pixel & blue_mask) << -blue_shift;

		if ( red_bits < 8 )
		    r = red_scale_table[r];
		if ( green_bits < 8 )
		    g = green_scale_table[g];
		if ( blue_bits < 8 )
		    b = blue_scale_table[b];

		if (alf) {
		    *dst++ = tqRgba(r, g, b, asrc[x]);
		} else if (msk) {
		    if ( ale ) {
			*dst++ = (asrc[x >> 3] & (1 << (x & 7)))
				 ? tqRgba(r, g, b, 0xff) : tqRgba(r, g, b, 0x00);
		    } else {
			*dst++ = (asrc[x >> 3] & (1 << (7 -(x & 7))))
				 ? tqRgba(r, g, b, 0xff) : tqRgba(r, g, b, 0x00);
		    }
		} else {
		    *dst++ = tqRgb(r, g, b);
		}
	    }
	}
    } else if ( xi->bits_per_pixel == d ) {	// compatible depth
	char *xidata = xi->data;		// copy each scanline
	int bpl = TQMIN(image.bytesPerLine(),xi->bytes_per_line);
	for ( int y=0; y<h; y++ ) {
	    memcpy( image.scanLine(y), xidata, bpl );
	    xidata += xi->bytes_per_line;
	}
    } else {
	/* Typically 2 or 4 bits display depth */
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQPixmap::convertToImage: Display not supported (bpp=%d)",
		  xi->bits_per_pixel );
#endif
	image.reset();
#ifdef QT_MITSHM_CONVERSIONS
        if( mitshm_ximage )
            tqt_XShmDestroyImage( xi, &shminfo );
        else
#endif
	qSafeXDestroyImage( xi );
	((TQPixmap*)this)->data->ximage = 0;
	return image;
    }

    if ( mono ) {				// bitmap
	image.setNumColors( 2 );
	image.setColor( 0, tqRgb(255,255,255) );
	image.setColor( 1, tqRgb(0,0,0) );
    } else if ( !trucol ) {			// pixmap with colormap
	uchar *p;
	uchar *end;
	uchar  use[256];			// pixel-in-use table
	uchar  pix[256];			// pixel translation table
	int    ncols, i, bpl;
	memset( use, 0, 256 );
	memset( pix, 0, 256 );
	bpl = image.bytesPerLine();

	if (msk) {				// which pixels are used?
	    for ( i=0; i<h; i++ ) {
		uchar* asrc = alpha.scanLine( i );
		p = image.scanLine( i );
		for ( int x = 0; x < w; x++ ) {
		    if ( ale ) {
			if (asrc[x >> 3] & (1 << (x & 7)))
			    use[*p] = 1;
		    } else {
			if (asrc[x >> 3] & (1 << (7 -(x & 7))))
			    use[*p] = 1;
		    }
		    ++p;
		}
	    }
	} else {
	    for ( i=0; i<h; i++ ) {
		p = image.scanLine( i );
		end = p + bpl;
		while ( p < end )
		    use[*p++] = 1;
	    }
	}
	ncols = 0;
	for ( i=0; i<256; i++ ) {		// build translation table
	    if ( use[i] )
		pix[i] = ncols++;
	}
	for ( i=0; i<h; i++ ) {			// translate pixels
	    p = image.scanLine( i );
	    end = p + bpl;
	    while ( p < end ) {
		*p = pix[*p];
		p++;
	    }
	}

	Colormap cmap	= x11Colormap();
	int	 ncells = x11Cells();
	XColor *carr = new XColor[ncells];
	for ( i=0; i<ncells; i++ )
	    carr[i].pixel = i;
	// Get default colormap
	XQueryColors( x11Display(), cmap, carr, ncells );

	if (msk) {
	    int trans;
	    if (ncols < 256) {
		trans = ncols++;
		image.setNumColors( ncols );	// create color table
		image.setColor( trans, 0x00000000 );
	    } else {
		image.setNumColors( ncols );	// create color table
		// oh dear... no spare "transparent" pixel.
		// use first pixel in image (as good as any).
		trans = image.scanLine( i )[0];
	    }
	    for ( i=0; i<h; i++ ) {
		uchar* asrc = alpha.scanLine( i );
		p = image.scanLine( i );
		for ( int x = 0; x < w; x++ ) {
		    if ( ale ) {
			if (!(asrc[x >> 3] & (1 << (x & 7))))
			    *p = trans;
		    } else {
			if (!(asrc[x >> 3] & (1 << (7 -(x & 7)))))
			    *p = trans;
		    }
		    ++p;
		}
	    }
	} else {
	    image.setNumColors( ncols );	// create color table
	}
	int j = 0;
	for ( i=0; i<256; i++ ) {		// translate pixels
	    if ( use[i] ) {
		image.setColor( j++,
				( msk ? 0xff000000 : 0 )
				| tqRgb( (carr[i].red   >> 8) & 255,
					(carr[i].green >> 8) & 255,
					(carr[i].blue  >> 8) & 255 ) );
	    }
	}

	delete [] carr;
    }
    if ( data->optim != BestOptim ) {		// throw away image data
#ifdef QT_MITSHM_CONVERSIONS
        if( mitshm_ximage )
            tqt_XShmDestroyImage( xi, &shminfo );
        else
#endif
	qSafeXDestroyImage( xi );
	((TQPixmap*)this)->data->ximage = 0;
    } else {					// keep ximage data
#ifdef QT_MITSHM_CONVERSIONS
        if( mitshm_ximage ) { // copy the XImage?
            tqt_XShmDestroyImage( xi, &shminfo );
            xi = 0;
        }
#endif
	((TQPixmap*)this)->data->ximage = xi;
    }

    return image;
}


/*!
    Converts image \a img and sets this pixmap. Returns true if
    successful; otherwise returns false.

    The \a conversion_flags argument is a bitwise-OR of the
    \l{TQt::ImageConversionFlags}. Passing 0 for \a conversion_flags
    sets all the default options.

    Note that even though a TQPixmap with depth 1 behaves much like a
    TQBitmap, isTQBitmap() returns false.

    If a pixmap with depth 1 is painted with color0 and color1 and
    converted to an image, the pixels painted with color0 will produce
    pixel index 0 in the image and those painted with color1 will
    produce pixel index 1.

    \sa convertToImage(), isTQBitmap(), TQImage::convertDepth(),
    defaultDepth(), TQImage::hasAlphaBuffer()
*/

bool TQPixmap::convertFromImage( const TQImage &img, int conversion_flags )
{
    if ( img.isNull() ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQPixmap::convertFromImage: Cannot convert a null image" );
#endif
	return false;
    }
    detach();					// detach other references
    TQImage  image = img;
    const uint	 w   = image.width();
    const uint	 h   = image.height();
    int	 d   = image.depth();
    const int	 dd  = x11Depth();
    bool force_mono = (dd == 1 || isTQBitmap() ||
		       (conversion_flags & ColorMode_Mask)==MonoOnly );

    if ( w >= 32768 || h >= 32768 )
        return false;

    // get rid of the mask
    delete data->mask;
    data->mask = 0;

    // get rid of alpha pixmap
    delete data->alphapm;
    data->alphapm = 0;

    // must be monochrome
    if ( force_mono ) {
	if ( d != 1 ) {
	    // dither
	    image = image.convertDepth( 1, conversion_flags );
	    d = 1;
	}
    } else {					// can be both
	bool conv8 = false;
	if ( d > 8 && dd <= 8 ) {		// convert to 8 bit
	    if ( (conversion_flags & DitherMode_Mask) == AutoDither )
		conversion_flags = (conversion_flags & ~DitherMode_Mask)
				   | PreferDither;
	    conv8 = true;
	} else if ( (conversion_flags & ColorMode_Mask) == ColorOnly ) {
	    conv8 = d == 1;			// native depth wanted
	} else if ( d == 1 ) {
	    if ( image.numColors() == 2 ) {
		TQRgb c0 = image.color(0);	// Auto: convert to best
		TQRgb c1 = image.color(1);
		conv8 = TQMIN(c0,c1) != tqRgb(0,0,0) || TQMAX(c0,c1) != tqRgb(255,255,255);
	    } else {
		// eg. 1-color monochrome images (they do exist).
		conv8 = true;
	    }
	}
	if ( conv8 ) {
	    image = image.convertDepth( 8, conversion_flags );
	    d = 8;
	}
    }

    if ( d == 1 ) {				// 1 bit pixmap (bitmap)
	if ( hd ) {				// delete old X pixmap

#ifndef TQT_NO_XFTFREETYPE
	    if (rendhd) {
		XftDrawDestroy( (XftDraw *) rendhd );
		rendhd = 0;
	    }
#endif // TQT_NO_XFTFREETYPE

	    XFreePixmap( x11Display(), hd );
	}

	// make sure image.color(0) == color0 (white) and image.color(1) == color1 (black)
	if (image.color(0) == TQt::black.rgb() && image.color(1) == TQt::white.rgb()) {
	    image.invertPixels();
	    image.setColor(0, TQt::white.rgb());
	    image.setColor(1, TQt::black.rgb());
	}

	char  *bits;
	uchar *tmp_bits;
	int    bpl = (w+7)/8;
	int    ibpl = image.bytesPerLine();
	if ( image.bitOrder() == TQImage::BigEndian || bpl != ibpl ) {
	    tmp_bits = new uchar[bpl*h];
	    TQ_CHECK_PTR( tmp_bits );
	    bits = (char *)tmp_bits;
	    uchar *p, *b, *end;
	    uint y, count;
	    if ( image.bitOrder() == TQImage::BigEndian ) {
		const uchar *f = tqt_get_bitflip_array();
		b = tmp_bits;
		for ( y=0; y<h; y++ ) {
		    p = image.scanLine( y );
		    end = p + bpl;
		    count = bpl;
		    while ( count > 4 ) {
			*b++ = f[*p++];
			*b++ = f[*p++];
			*b++ = f[*p++];
			*b++ = f[*p++];
			count -= 4;
		    }
		    while ( p < end )
			*b++ = f[*p++];
		}
	    } else {				// just copy
		b = tmp_bits;
		p = image.scanLine( 0 );
		for ( y=0; y<h; y++ ) {
		    memcpy( b, p, bpl );
		    b += bpl;
		    p += ibpl;
		}
	    }
	} else {
	    bits = (char *)image.bits();
	    tmp_bits = 0;
	}
	hd = (HANDLE)XCreateBitmapFromData( x11Display(),
					    RootWindow(x11Display(), x11Screen() ),
					    bits, w, h );

#ifndef TQT_NO_XFTFREETYPE
	if ( tqt_has_xft )
	    rendhd = (HANDLE) XftDrawCreateBitmap( x11Display(), hd );
#endif // TQT_NO_XFTFREETYPE

	if ( tmp_bits )				// Avoid purify complaint
	    delete [] tmp_bits;
	data->w = w;  data->h = h;  data->d = 1;

	if ( image.hasAlphaBuffer() ) {
	    TQBitmap m;
	    m = image.createAlphaMask( conversion_flags );
	    setMask( m );
	}
	return true;
    }

    Display *dpy   = x11Display();
    Visual *visual = (Visual *)x11Visual();
    XImage *xi	   = 0;
    bool    trucol = (visual->c_class == TrueColor || visual->c_class == DirectColor);
    int	    nbytes = image.numBytes();
    uchar  *newbits= 0;
#ifdef QT_MITSHM_CONVERSIONS
    int newbits_size = 0;
    bool mitshm_ximage = false;
    XShmSegmentInfo shminfo;
#endif

    if ( trucol ) {				// truecolor display
	TQRgb  pix[256];				// pixel translation table
	const bool  d8 = d == 8;
	const uint  red_mask	  = (uint)visual->red_mask;
	const uint  green_mask  = (uint)visual->green_mask;
	const uint  blue_mask	  = (uint)visual->blue_mask;
	const int   red_shift	  = highest_bit( red_mask )   - 7;
	const int   green_shift = highest_bit( green_mask ) - 7;
	const int   blue_shift  = highest_bit( blue_mask )  - 7;
	const uint  rbits = highest_bit(red_mask) - lowest_bit(red_mask) + 1;
	const uint  gbits = highest_bit(green_mask) - lowest_bit(green_mask) + 1;
	const uint  bbits = highest_bit(blue_mask) - lowest_bit(blue_mask) + 1;

	if ( d8 ) {				// setup pixel translation
	    TQRgb *ctable = image.colorTable();
	    for ( int i=0; i<image.numColors(); i++ ) {
		int r = tqRed  (ctable[i]);
		int g = tqGreen(ctable[i]);
		int b = tqBlue (ctable[i]);
		r = red_shift	> 0 ? r << red_shift   : r >> -red_shift;
		g = green_shift > 0 ? g << green_shift : g >> -green_shift;
		b = blue_shift	> 0 ? b << blue_shift  : b >> -blue_shift;
		pix[i] = (b & blue_mask) | (g & green_mask) | (r & red_mask)
			 | ~(blue_mask | green_mask | red_mask);
	    }
	}

#ifdef QT_MITSHM_CONVERSIONS
	xi = tqt_XShmCreateImage( dpy, visual, dd, ZPixmap, 0, 0, w, h, 32, 0, &shminfo );
	if( xi != NULL ) {
	    mitshm_ximage = true;
	    newbits = (uchar*)xi->data;
	}
	else
#endif
	xi = XCreateImage( dpy, visual, dd, ZPixmap, 0, 0, w, h, 32, 0 );
        if (!xi)
            return false;
	if( newbits == NULL )
	newbits = (uchar *)malloc( xi->bytes_per_line*h );
	TQ_CHECK_PTR( newbits );
	if ( !newbits )				// no memory
	    return false;
	int    bppc = xi->bits_per_pixel;

	bool contig_bits = n_bits(red_mask) == rbits &&
                           n_bits(green_mask) == gbits &&
                           n_bits(blue_mask) == bbits;
	bool dither_tc =
	    // Want it?
	    (conversion_flags & Dither_Mask) != ThresholdDither &&
	    (conversion_flags & DitherMode_Mask) != AvoidDither &&
	    // Need it?
	    bppc < 24 && !d8 &&
	    // Can do it? (Contiguous bits?)
	    contig_bits;

	static bool init=false;
	static int D[16][16];
	if ( dither_tc && !init ) {
	    // I also contributed this code to XV - WWA.
	    /*
	      The dither matrix, D, is obtained with this formula:

	      D2 = [ 0 2 ]
	      [ 3 1 ]


	      D2*n = [ 4*Dn       4*Dn+2*Un ]
	      [ 4*Dn+3*Un  4*Dn+1*Un ]
	    */
	    int n,i,j;
	    init=1;

	    /* Set D2 */
	    D[0][0]=0;
	    D[1][0]=2;
	    D[0][1]=3;
	    D[1][1]=1;

	    /* Expand using recursive definition given above */
	    for (n=2; n<16; n*=2) {
		for (i=0; i<n; i++) {
		    for (j=0; j<n; j++) {
			D[i][j]*=4;
			D[i+n][j]=D[i][j]+2;
			D[i][j+n]=D[i][j]+3;
			D[i+n][j+n]=D[i][j]+1;
		    }
		}
	    }
	    init=true;
	}
        
        enum { BPP8, 
               BPP16_8_3_M3, BPP16_7_2_M3, BPP16_MSB, BPP16_LSB,
               BPP24_MSB, BPP24_LSB,
               BPP32_16_8_0, BPP32_MSB, BPP32_LSB
        } mode = BPP8;

	if ( bppc > 8 && xi->byte_order == LSBFirst )
	    bppc++;

        int wordsize;
        bool bigendian;
        tqSysInfo( &wordsize, &bigendian );
        bool same_msb_lsb = ( xi->byte_order == MSBFirst ) == ( bigendian );
        
        if( bppc == 8 ) // 8 bit
            mode = BPP8;
        else if( bppc == 16 || bppc == 17 ) { // 16 bit MSB/LSB
            if( red_shift == 8 && green_shift == 3 && blue_shift == -3
                && !d8 && same_msb_lsb )
                mode = BPP16_8_3_M3;
            else if( red_shift == 7 && green_shift == 2 && blue_shift == -3
                && !d8 && same_msb_lsb )
                mode = BPP16_7_2_M3;
            else
                mode = bppc == 17 ? BPP16_LSB : BPP16_MSB;
        } else if( bppc == 24 || bppc == 25 ) { // 24 bit MSB/LSB
            mode = bppc == 25 ? BPP24_LSB : BPP24_MSB;
        } else if( bppc == 32 || bppc == 33 ) { // 32 bit MSB/LSB
            if( red_shift == 16 && green_shift == 8 && blue_shift == 0
                && !d8 && same_msb_lsb )
                mode = BPP32_16_8_0;
            else
                mode = bppc == 33 ? BPP32_LSB : BPP32_MSB;
        } else
	    tqFatal("Logic error 3");

#define GET_PIXEL \
                int pixel; \
		if ( d8 ) pixel = pix[*src++]; \
		else { \
		    int r = tqRed  ( *p ); \
		    int g = tqGreen( *p ); \
		    int b = tqBlue ( *p++ ); \
		    r = red_shift   > 0 \
		        ? r << red_shift   : r >> -red_shift; \
		    g = green_shift > 0 \
		        ? g << green_shift : g >> -green_shift; \
		    b = blue_shift  > 0 \
		        ? b << blue_shift  : b >> -blue_shift; \
		    pixel = (r & red_mask)|(g & green_mask) | (b & blue_mask) \
			    | ~(blue_mask | green_mask | red_mask); \
		}

// optimized case - no d8 case, shift only once instead of twice, mask only once instead of twice,
// use direct values instead of variables, and use only one statement
// (*p >> 16), (*p >> 8 ) and (*p) are tqRed(),tqGreen() and tqBlue() without masking
// shifts have to be passed including the shift operator (e.g. '>>3'), because of the direction
#define GET_PIXEL_OPT(red_shift,green_shift,blue_shift,red_mask,green_mask,blue_mask) \
                int pixel = ((( *p >> 16 ) red_shift ) & red_mask ) \
                    | ((( *p >> 8 ) green_shift ) & green_mask ) \
                    | ((( *p ) blue_shift ) & blue_mask ); \
                ++p;

#define GET_PIXEL_DITHER_TC \
		int r = tqRed  ( *p ); \
		int g = tqGreen( *p ); \
		int b = tqBlue ( *p++ ); \
		const int thres = D[x%16][y%16]; \
		if ( r <= (255-(1<<(8-rbits))) && ((r<<rbits) & 255) \
			> thres) \
		    r += (1<<(8-rbits)); \
		if ( g <= (255-(1<<(8-gbits))) && ((g<<gbits) & 255) \
			> thres) \
		    g += (1<<(8-gbits)); \
		if ( b <= (255-(1<<(8-bbits))) && ((b<<bbits) & 255) \
			> thres) \
		    b += (1<<(8-bbits)); \
		r = red_shift   > 0 \
		    ? r << red_shift   : r >> -red_shift; \
		g = green_shift > 0 \
		    ? g << green_shift : g >> -green_shift; \
		b = blue_shift  > 0 \
		    ? b << blue_shift  : b >> -blue_shift; \
		int pixel = (r & red_mask)|(g & green_mask) | (b & blue_mask);

// again, optimized case
// can't be optimized that much :(
#define GET_PIXEL_DITHER_TC_OPT(red_shift,green_shift,blue_shift,red_mask,green_mask,blue_mask, \
                                rbits,gbits,bbits) \
		const int thres = D[x%16][y%16]; \
		int r = tqRed  ( *p ); \
		if ( r <= (255-(1<<(8-rbits))) && ((r<<rbits) & 255) \
			> thres) \
		    r += (1<<(8-rbits)); \
		int g = tqGreen( *p ); \
		if ( g <= (255-(1<<(8-gbits))) && ((g<<gbits) & 255) \
			> thres) \
		    g += (1<<(8-gbits)); \
		int b = tqBlue ( *p++ ); \
		if ( b <= (255-(1<<(8-bbits))) && ((b<<bbits) & 255) \
			> thres) \
		    b += (1<<(8-bbits)); \
                int pixel = (( r red_shift ) & red_mask ) \
                    | (( g green_shift ) & green_mask ) \
                    | (( b blue_shift ) & blue_mask );

#define CYCLE(body) \
	for ( uint y=0; y<h; y++ ) { \
	    uchar* src = image.scanLine( y ); \
	    uchar* dst = newbits + xi->bytes_per_line*y; \
	    TQRgb* p = (TQRgb *)src; \
            body \
        }

        if ( dither_tc ) {
	    switch ( mode ) {
                case BPP16_8_3_M3:
                    CYCLE(
                        TQ_INT16* dst16 = (TQ_INT16*)dst;
		        for ( uint x=0; x<w; x++ ) {
			    GET_PIXEL_DITHER_TC_OPT(<<8,<<3,>>3,0xf800,0x7e0,0x1f,5,6,5)
                            *dst16++ = pixel;
		        }
                    )
		    break;
                case BPP16_7_2_M3:
                    CYCLE(
                        TQ_INT16* dst16 = (TQ_INT16*)dst;
		        for ( uint x=0; x<w; x++ ) {
			    GET_PIXEL_DITHER_TC_OPT(<<7,<<2,>>3,0x7c00,0x3e0,0x1f,5,5,5)
                            *dst16++ = pixel;
		        }
                    )
		    break;
		case BPP16_MSB:			// 16 bit MSB
                    CYCLE(
		        for ( uint x=0; x<w; x++ ) {
			    GET_PIXEL_DITHER_TC
			    *dst++ = (pixel >> 8);
			    *dst++ = pixel;
		        }
                    )
		    break;
		case BPP16_LSB:			// 16 bit LSB
                    CYCLE(
    		        for ( uint x=0; x<w; x++ ) {
			    GET_PIXEL_DITHER_TC
			    *dst++ = pixel;
			    *dst++ = pixel >> 8;
		        }
                    )
		    break;
		default:
		    tqFatal("Logic error");
		}
	} else {
	    switch ( mode ) {
		case BPP8:			// 8 bit
                    CYCLE(
                    Q_UNUSED(p);
		        for ( uint x=0; x<w; x++ ) {
			    int pixel = pix[*src++];
			    *dst++ = pixel;
		        }
                    )
		    break;
                case BPP16_8_3_M3:
                    CYCLE(
                        TQ_INT16* dst16 = (TQ_INT16*)dst;
		        for ( uint x=0; x<w; x++ ) {
			    GET_PIXEL_OPT(<<8,<<3,>>3,0xf800,0x7e0,0x1f)
                            *dst16++ = pixel;
		        }
                    )
		    break;
                case BPP16_7_2_M3:
                    CYCLE(
                        TQ_INT16* dst16 = (TQ_INT16*)dst;
		        for ( uint x=0; x<w; x++ ) {
			    GET_PIXEL_OPT(<<7,<<2,>>3,0x7c00,0x3e0,0x1f)
                            *dst16++ = pixel;
		        }
                    )
		    break;
		case BPP16_MSB:			// 16 bit MSB
                    CYCLE(
		        for ( uint x=0; x<w; x++ ) {
			    GET_PIXEL
			    *dst++ = (pixel >> 8);
			    *dst++ = pixel;
		        }
                    )
		    break;
		case BPP16_LSB:			// 16 bit LSB
                    CYCLE(
		        for ( uint x=0; x<w; x++ ) {
			    GET_PIXEL
			    *dst++ = pixel;
			    *dst++ = pixel >> 8;
		        }
                    )
		    break;
		case BPP24_MSB:			// 24 bit MSB
                    CYCLE(
		        for ( uint x=0; x<w; x++ ) {
			    GET_PIXEL
			    *dst++ = pixel >> 16;
			    *dst++ = pixel >> 8;
			    *dst++ = pixel;
		        }
                    )
		    break;
		case BPP24_LSB:			// 24 bit LSB
                    CYCLE(
		        for ( uint x=0; x<w; x++ ) {
			    GET_PIXEL
			    *dst++ = pixel;
			    *dst++ = pixel >> 8;
			    *dst++ = pixel >> 16;
		        }
                    )
		    break;
                case BPP32_16_8_0:
                    CYCLE(
                        memcpy( dst, p, w * 4 );
                    )
                    break;
		case BPP32_MSB:			// 32 bit MSB
                    CYCLE(
		        for ( uint x=0; x<w; x++ ) {
			    GET_PIXEL
			    *dst++ = pixel >> 24;
			    *dst++ = pixel >> 16;
			    *dst++ = pixel >> 8;
			    *dst++ = pixel;
		        }
                    )
		    break;
		case BPP32_LSB:			// 32 bit LSB
                    CYCLE(
		        for ( uint x=0; x<w; x++ ) {
			    GET_PIXEL
			    *dst++ = pixel;
			    *dst++ = pixel >> 8;
			    *dst++ = pixel >> 16;
			    *dst++ = pixel >> 24;
		        }
                    )
  		    break;
  		default:
  		    tqFatal("Logic error 2");
  	    }
  	}
  	xi->data = (char *)newbits;
    }

    if ( d == 8 && !trucol ) {			// 8 bit pixmap
	int  pop[256];				// pixel popularity

	if ( image.numColors() == 0 )
	    image.setNumColors( 1 );

	memset( pop, 0, sizeof(int)*256 );	// reset popularity array
	uint i;
	for ( i=0; i<h; i++ ) {			// for each scanline...
	    uchar* p = image.scanLine( i );
	    uchar *end = p + w;
	    while ( p < end )			// compute popularity
		pop[*p++]++;
	}

	newbits = (uchar *)malloc( nbytes );	// copy image into newbits
#ifdef QT_MITSHM_CONVERSIONS
        newbits_size = nbytes;
#endif
	TQ_CHECK_PTR( newbits );
	if ( !newbits )				// no memory
	    return false;
	uchar* p = newbits;
	memcpy( p, image.bits(), nbytes );	// copy image data into newbits

	/*
	 * The code below picks the most important colors. It is based on the
	 * diversity algorithm, implemented in XV 3.10. XV is (C) by John Bradley.
	 */

	struct PIX {				// pixel sort element
	    uchar r,g,b,n;			// color + pad
	    int	  use;				// popularity
	    int	  index;			// index in colormap
	    int	  mindist;
	};
	int ncols = 0;
	for ( i=0; i< (uint) image.numColors(); i++ ) { // compute number of colors
	    if ( pop[i] > 0 )
		ncols++;
	}
	for ( i=image.numColors(); i<256; i++ ) // ignore out-of-range pixels
	    pop[i] = 0;

	// works since we make sure above to have at least
	// one color in the image
	if ( ncols == 0 )
	    ncols = 1;

	PIX pixarr[256];			// pixel array
	PIX pixarr_sorted[256];			// pixel array (sorted)
	memset( pixarr, 0, ncols*sizeof(PIX) );
	PIX *px		   = &pixarr[0];
	int  maxpop = 0;
	int  maxpix = 0;
	TQ_CHECK_PTR( pixarr );
	uint j = 0;
	TQRgb* ctable = image.colorTable();
	for ( i=0; i<256; i++ ) {		// init pixel array
	    if ( pop[i] > 0 ) {
		px->r = tqRed  ( ctable[i] );
		px->g = tqGreen( ctable[i] );
		px->b = tqBlue ( ctable[i] );
		px->n = 0;
		px->use = pop[i];
		if ( pop[i] > maxpop ) {	// select most popular entry
		    maxpop = pop[i];
		    maxpix = j;
		}
		px->index = i;
		px->mindist = 1000000;
		px++;
		j++;
	    }
	}
	pixarr_sorted[0] = pixarr[maxpix];
	pixarr[maxpix].use = 0;

	for ( i=1; i< (uint) ncols; i++ ) {		// sort pixels
	    int minpix = -1, mindist = -1;
	    px = &pixarr_sorted[i-1];
	    int r = px->r;
	    int g = px->g;
	    int b = px->b;
	    int dist;
	    if ( (i & 1) || i<10 ) {		// sort on max distance
		for ( int j=0; j<ncols; j++ ) {
		    px = &pixarr[j];
		    if ( px->use ) {
			dist = (px->r - r)*(px->r - r) +
			       (px->g - g)*(px->g - g) +
			       (px->b - b)*(px->b - b);
			if ( px->mindist > dist )
			    px->mindist = dist;
			if ( px->mindist > mindist ) {
			    mindist = px->mindist;
			    minpix = j;
			}
		    }
		}
	    } else {				// sort on max popularity
		for ( int j=0; j<ncols; j++ ) {
		    px = &pixarr[j];
		    if ( px->use ) {
			dist = (px->r - r)*(px->r - r) +
			       (px->g - g)*(px->g - g) +
			       (px->b - b)*(px->b - b);
			if ( px->mindist > dist )
			    px->mindist = dist;
			if ( px->use > mindist ) {
			    mindist = px->use;
			    minpix = j;
			}
		    }
		}
	    }
	    pixarr_sorted[i] = pixarr[minpix];
	    pixarr[minpix].use = 0;
	}

	uint pix[256];				// pixel translation table
	px = &pixarr_sorted[0];
	for ( i=0; i< (uint) ncols; i++ ) {		// allocate colors
	    TQColor c( px->r, px->g, px->b );
	    pix[px->index] = c.pixel(x11Screen());
	    px++;
	}

	p = newbits;
	for ( i=0; i< (uint) nbytes; i++ ) {		// translate pixels
	    *p = pix[*p];
	    p++;
	}
    }

    if ( !xi ) {				// X image not created
#ifdef QT_MITSHM_CONVERSIONS
        xi = tqt_XShmCreateImage( dpy, visual, dd, ZPixmap, 0, 0, w, h, 32, 0, &shminfo );
        if( xi != NULL )
            mitshm_ximage = true;
        else
#endif
	xi = XCreateImage( dpy, visual, dd, ZPixmap, 0, 0, w, h, 32, 0 );
	if ( xi->bits_per_pixel == 16 ) {	// convert 8 bpp ==> 16 bpp
	    ushort *p2;
	    int	    p2inc = xi->bytes_per_line/sizeof(ushort);
	    ushort *newerbits = (ushort *)malloc( xi->bytes_per_line * h );
#ifdef QT_MITSHM_CONVERSIONS
            newbits_size = xi->bytes_per_line * h;
#endif
	    TQ_CHECK_PTR( newerbits );
	    if ( !newerbits )				// no memory
		return false;
	    uchar* p = newbits;
	    for ( uint y=0; y<h; y++ ) {		// OOPS: Do right byte order!!
		p2 = newerbits + p2inc*y;
		for ( uint x=0; x<w; x++ )
		    *p2++ = *p++;
	    }
	    free( newbits );
	    newbits = (uchar *)newerbits;
	} else if ( xi->bits_per_pixel != 8 ) {
#if defined(QT_CHECK_RANGE)
	    tqWarning( "TQPixmap::convertFromImage: Display not supported "
		      "(bpp=%d)", xi->bits_per_pixel );
#endif
	}
#ifdef QT_MITSHM_CONVERSIONS
        if( newbits_size > 0 && mitshm_ximage ) { // need to copy to shared memory
            memcpy( xi->data, newbits, newbits_size );
            free( newbits );
            newbits = (uchar*)xi->data;
        }
        else
#endif
	xi->data = (char *)newbits;
    }

    if ( hd && (width() != (int)w || height() != (int)h || this->depth() != dd) ) {

#ifndef TQT_NO_XFTFREETYPE
	if (rendhd) {
	    XftDrawDestroy( (XftDraw *) rendhd );
	    rendhd = 0;
	}
#endif // TQT_NO_XFTFREETYPE

	XFreePixmap( dpy, hd );			// don't reuse old pixmap
	hd = 0;
    }
    if ( !hd ) {					// create new pixmap
	hd = (HANDLE)XCreatePixmap( x11Display(),
				    RootWindow(x11Display(), x11Screen() ),
				    w, h, dd );

#ifndef TQT_NO_XFTFREETYPE
	if ( tqt_has_xft ) {
	    if ( data->d == 1 ) {
		rendhd = (HANDLE) XftDrawCreateBitmap( x11Display (), hd );
	    } else {
		rendhd = (HANDLE) XftDrawCreate( x11Display (), hd,
						 (Visual *) x11Visual(), x11Colormap() );
	    }
	}
#endif // TQT_NO_XFTFREETYPE

    }

#ifdef QT_MITSHM_CONVERSIONS
    if( mitshm_ximage )
        XShmPutImage( dpy, hd, tqt_xget_readonly_gc( x11Screen(), false ),
                      xi, 0, 0, 0, 0, w, h, False );
    else
#endif
    XPutImage( dpy, hd, tqt_xget_readonly_gc( x11Screen(), false  ),
	       xi, 0, 0, 0, 0, w, h );

    data->w = w;
    data->h = h;
    data->d = dd;

    XImage* axi = NULL;
#ifdef QT_MITSHM_CONVERSIONS
    bool mitshm_aximage = false;
    XShmSegmentInfo ashminfo;
#endif
    if ( image.hasAlphaBuffer() ) {
	TQBitmap m;
	m = image.createAlphaMask( conversion_flags );
	setMask( m );

#ifndef TQT_NO_XFTFREETYPE
	// does this image have an alphamap (and not just a 1bpp mask)?
	bool alphamap = image.depth() == 32;
	if (image.depth() == 8) {
	    const TQRgb * const rgb = image.colorTable();
	    for (int i = 0, count = image.numColors(); i < count; ++i) {
		const int alpha = tqAlpha(rgb[i]);
		if (alpha != 0 && alpha != 0xff) {
		    alphamap = true;
		    break;
		}
	    }
	}

	if (tqt_use_xrender && tqt_has_xft && alphamap) {
	    data->alphapm = new TQPixmap; // create a null pixmap

	    // setup pixmap data
	    data->alphapm->data->w = w;
	    data->alphapm->data->h = h;
	    data->alphapm->data->d = 8;

	    // create 8bpp pixmap and render picture
	    data->alphapm->hd =
		XCreatePixmap(x11Display(), RootWindow(x11Display(), x11Screen()),
			      w, h, 8);

	    data->alphapm->rendhd =
		(HANDLE) XftDrawCreateAlpha( x11Display(), data->alphapm->hd, 8 );

#ifdef QT_MITSHM_CONVERSIONS
	    axi = tqt_XShmCreateImage( x11Display(), (Visual*)x11Visual(),
		    8, ZPixmap, 0, 0, w, h, 8, 0, &ashminfo );
	    if( axi != NULL )
		mitshm_aximage = true;
	    else
#endif
		axi = XCreateImage(x11Display(), (Visual *) x11Visual(),
				       8, ZPixmap, 0, 0, w, h, 8, 0);

	    if (axi) {
		if( axi->data==NULL ) {
		// the data is deleted by qSafeXDestroyImage
                axi->data = (char *) malloc(h * axi->bytes_per_line);
		TQ_CHECK_PTR( axi->data );
		}
                char *aptr = axi->data;

                if (image.depth() == 32) {
                    const int *iptr = (const int *) image.bits();
                    if( axi->bytes_per_line == (int)w ) {
                        int max = w * h;
                        while (max--)
                            *aptr++ = *iptr++ >> 24; // squirt
                    } else {
                        for (uint i = 0; i < h; ++i ) {
                            for (uint j = 0; j < w; ++j )
                                *aptr++ = *iptr++ >> 24; // squirt
                            aptr += ( axi->bytes_per_line - w );
                        }
                    }
                } else if (image.depth() == 8) {
                    const TQRgb * const rgb = image.colorTable();
                    for (uint y = 0; y < h; ++y) {
                        const uchar *iptr = image.scanLine(y);
                        for (uint x = 0; x < w; ++x)
                            *aptr++ = tqAlpha(rgb[*iptr++]);
                        aptr += ( axi->bytes_per_line - w );
                    }
                }

                GC gc = XCreateGC(x11Display(), data->alphapm->hd, 0, 0);
 #ifdef QT_MITSHM_CONVERSIONS
		if( mitshm_aximage )
		    XShmPutImage( dpy, data->alphapm->hd, gc, axi, 0, 0, 0, 0, w, h, False );
		else
#endif
                XPutImage(dpy, data->alphapm->hd, gc, axi, 0, 0, 0, 0, w, h);
                XFreeGC(x11Display(), gc);
	    }
	}
#endif // TQT_NO_XFTFREETYPE
    }

#ifdef QT_MITSHM_CONVERSIONS
    if( mitshm_ximage || mitshm_aximage )
        XSync( x11Display(), False ); // wait until processed
#endif

    if ( data->optim != BestOptim ) {       // throw away image
#ifdef QT_MITSHM_CONVERSIONS
        if( mitshm_ximage )
            tqt_XShmDestroyImage( xi, &shminfo );
        else
#endif
     qSafeXDestroyImage( xi );
     data->ximage = 0;
    } else {      // keep ximage that we created
#ifdef QT_MITSHM_CONVERSIONS
        if( mitshm_ximage ) { // copy the XImage?
            tqt_XShmDestroyImage( xi, &shminfo );
            xi = 0;
        }
#endif
     data->ximage = xi;
    }
    if( axi ) {
#ifdef QT_MITSHM_CONVERSIONS
        if( mitshm_aximage )
            tqt_XShmDestroyImage( axi, &ashminfo );
        else
#endif
        qSafeXDestroyImage(axi);
    }
    return true;
}


/*!
    Grabs the contents of the window \a window and makes a pixmap out
    of it. Returns the pixmap.

    The arguments \a (x, y) specify the offset in the window, whereas
    \a (w, h) specify the width and height of the area to be copied.

    If \a w is negative, the function copies everything to the right
    border of the window. If \a h is negative, the function copies
    everything to the bottom of the window.

    Note that grabWindow() grabs pixels from the screen, not from the
    window. If there is another window partially or entirely over the
    one you grab, you get pixels from the overlying window, too.

    Note also that the mouse cursor is generally not grabbed.

    The reason we use a window identifier and not a TQWidget is to
    enable grabbing of windows that are not part of the application,
    window system frames, and so on.

    \warning Grabbing an area outside the screen is not safe in
    general. This depends on the underlying window system.

    \warning X11 only: If \a window is not the same depth as the root
    window and another window partially or entirely obscures the one
    you grab, you will \e not get pixels from the overlying window.
    The contests of the obscured areas in the pixmap are undefined and
    uninitialized.

    \sa grabWidget()
*/

TQPixmap TQPixmap::grabWindow( WId window, int x, int y, int w, int h )
{
    if ( w == 0 || h == 0 )
	return TQPixmap();

    Display *dpy = x11AppDisplay();
    XWindowAttributes window_attr;
    if ( ! XGetWindowAttributes( dpy, window, &window_attr ) )
	return TQPixmap();

    if ( w < 0 )
	w = window_attr.width - x;
    if ( h < 0 )
	h = window_attr.height - y;

    // determine the screen
    int scr;
    for ( scr = 0; scr < ScreenCount( dpy ); ++scr ) {
	if ( window_attr.root == RootWindow( dpy, scr ) )	// found it
	    break;
    }
    if ( scr >= ScreenCount( dpy ) )		// sanity check
	return TQPixmap();


    // get the depth of the root window
    XWindowAttributes root_attr;
    if ( ! XGetWindowAttributes( dpy, window_attr.root, &root_attr ) )
        return TQPixmap();

    if ( window_attr.depth == root_attr.depth ) {
        // if the depth of the specified window and the root window are the
        // same, grab pixels from the root window (so that we get the any
        // overlapping windows and window manager frames)

        // map x and y to the root window
        WId unused;
        if ( ! XTranslateCoordinates( dpy, window, window_attr.root, x, y,
                                      &x, &y, &unused ) )
            return TQPixmap();

       window = window_attr.root;
    }

    TQPixmap pm( w, h );
    pm.data->uninit = false;
    pm.x11SetScreen( scr );

    GC gc = tqt_xget_temp_gc( scr, false );
    XSetSubwindowMode( dpy, gc, IncludeInferiors );
    XCopyArea( dpy, window, pm.handle(), gc, x, y, w, h, 0, 0 );
    XSetSubwindowMode( dpy, gc, ClipByChildren );

    return pm;
}

/*!
    Returns a copy of the pixmap that is transformed using \a matrix.
    The original pixmap is not changed.

    The transformation \a matrix is internally adjusted to compensate
    for unwanted translation, i.e. xForm() returns the smallest image
    that contains all the transformed points of the original image.

    This function is slow because it involves transformation to a
    TQImage, non-trivial computations and a transformation back to a
    TQPixmap.

    \sa trueMatrix(), TQWMatrix, TQPainter::setWorldMatrix() TQImage::xForm()
*/

TQPixmap TQPixmap::xForm( const TQWMatrix &matrix ) const
{
    uint   w = 0;
    uint   h = 0;				// size of target pixmap
    uint   ws, hs;				// size of source pixmap
    uchar *dptr;				// data in target pixmap
    uint   dbpl, dbytes;			// bytes per line/bytes total
    uchar *sptr;				// data in original pixmap
    int	   sbpl;				// bytes per line in original
    int	   bpp;					// bits per pixel
    bool   depth1 = depth() == 1;
    Display *dpy = x11Display();

    if ( isNull() )				// this is a null pixmap
	return copy();

    ws = width();
    hs = height();

    TQWMatrix mat( matrix.m11(), matrix.m12(), matrix.m21(), matrix.m22(), 0., 0. );

    double scaledWidth;
    double scaledHeight;

    if ( matrix.m12() == 0.0F && matrix.m21() == 0.0F ) {
	if ( matrix.m11() == 1.0F && matrix.m22() == 1.0F )
	    return *this;			// identity matrix
	scaledHeight = matrix.m22()*hs;
	scaledWidth = matrix.m11()*ws;
	h = TQABS( tqRound( scaledHeight ) );
	w = TQABS( tqRound( scaledWidth ) );
    } else {					// rotation or shearing
	TQPointArray a( TQRect(0,0,ws+1,hs+1) );
	a = mat.map( a );
	TQRect r = a.boundingRect().normalize();
	w = r.width()-1;
	h = r.height()-1;
        scaledWidth = w;
        scaledHeight = h;
    }

    mat = trueMatrix( mat, ws, hs ); // true matrix


    bool invertible;
    mat = mat.invert( &invertible );		// invert matrix

    if ( h == 0 || w == 0 || !invertible
         || TQABS(scaledWidth) >= 32768 || TQABS(scaledHeight) >= 32768 ) {	// error, return null pixmap
	TQPixmap pm;
	pm.data->bitmap = data->bitmap;
	return pm;
    }

#if defined(QT_MITSHM_XFORM)
    static bool try_once = true;
    if (try_once) {
	try_once = false;
	if ( !xshminit )
	    tqt_create_mitshm_buffer( this, 800, 600 );
    }

    bool use_mitshm = xshmimg && !depth1 &&
    xshmimg->width >= w && xshmimg->height >= h;
#endif
    XImage *xi = (XImage*)data->ximage;		// any cached ximage?
    if ( !xi )
	xi = XGetImage( x11Display(), handle(), 0, 0, ws, hs, AllPlanes,
			depth1 ? XYPixmap : ZPixmap );

    if ( !xi ) {				// error, return null pixmap
	TQPixmap pm;
	pm.data->bitmap = data->bitmap;
	pm.data->alphapm = data->alphapm;
	return pm;
    }

    sbpl = xi->bytes_per_line;
    sptr = (uchar *)xi->data;
    bpp	 = xi->bits_per_pixel;

    if ( depth1 )
	dbpl = (w+7)/8;
    else
	dbpl = ((w*bpp+31)/32)*4;
    dbytes = dbpl*h;

#if defined(QT_MITSHM_XFORM)
    if ( use_mitshm ) {
	dptr = (uchar *)xshmimg->data;
	uchar fillbyte = bpp == 8 ? white.pixel() : 0xff;
	for ( int y=0; y<h; y++ )
	    memset( dptr + y*xshmimg->bytes_per_line, fillbyte, dbpl );
    } else {
#endif
	dptr = (uchar *)malloc( dbytes );	// create buffer for bits
	TQ_CHECK_PTR( dptr );
	if ( depth1 )				// fill with zeros
	    memset( dptr, 0, dbytes );
	else if ( bpp == 8 )			// fill with background color
	    memset( dptr, TQt::white.pixel( x11Screen() ), dbytes );
	else
	    memset( dptr, 0xff, dbytes );
#if defined(QT_MITSHM_XFORM)
    }
#endif

    // #define QT_DEBUG_XIMAGE
#if defined(QT_DEBUG_XIMAGE)
    tqDebug( "----IMAGE--INFO--------------" );
    tqDebug( "width............. %d", xi->width );
    tqDebug( "height............ %d", xi->height );
    tqDebug( "xoffset........... %d", xi->xoffset );
    tqDebug( "format............ %d", xi->format );
    tqDebug( "byte order........ %d", xi->byte_order );
    tqDebug( "bitmap unit....... %d", xi->bitmap_unit );
    tqDebug( "bitmap bit order.. %d", xi->bitmap_bit_order );
    tqDebug( "depth............. %d", xi->depth );
    tqDebug( "bytes per line.... %d", xi->bytes_per_line );
    tqDebug( "bits per pixel.... %d", xi->bits_per_pixel );
#endif

    int type;
    if ( xi->bitmap_bit_order == MSBFirst )
	type = QT_XFORM_TYPE_MSBFIRST;
    else
	type = QT_XFORM_TYPE_LSBFIRST;
    int	xbpl, p_inc;
    if ( depth1 ) {
	xbpl  = (w+7)/8;
	p_inc = dbpl - xbpl;
    } else {
	xbpl  = (w*bpp)/8;
	p_inc = dbpl - xbpl;
#if defined(QT_MITSHM_XFORM)
	if ( use_mitshm )
	    p_inc = xshmimg->bytes_per_line - xbpl;
#endif
    }

    if ( !qt_xForm_helper( mat, xi->xoffset, type, bpp, dptr, xbpl, p_inc, h, sptr, sbpl, ws, hs ) ){
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQPixmap::xForm: display not supported (bpp=%d)",bpp);
#endif
	TQPixmap pm;
	return pm;
    }

    if ( data->optim == NoOptim ) {		// throw away ximage
	qSafeXDestroyImage( xi );
	data->ximage = 0;
    } else {					// keep ximage that we fetched
	data->ximage = xi;
    }

    if ( depth1 ) {				// mono bitmap
	TQPixmap pm( w, h, dptr, TQImage::systemBitOrder() != TQImage::BigEndian );
	pm.data->bitmap = data->bitmap;
	free( dptr );
	if ( data->mask ) {
	    if ( data->selfmask )		// pixmap == mask
		pm.setMask( *((TQBitmap*)(&pm)) );
	    else
		pm.setMask( data->mask->xForm(matrix) );
	}
	return pm;
    } else {					// color pixmap
	GC gc = tqt_xget_readonly_gc( x11Screen(), false );
	TQPixmap pm( w, h );
	pm.data->uninit = false;
	pm.x11SetScreen( x11Screen() );
#if defined(QT_MITSHM_XFORM)
	if ( use_mitshm ) {
	    XCopyArea( dpy, xshmpm, pm.handle(), gc, 0, 0, w, h, 0, 0 );
	} else {
#endif
	    xi = XCreateImage( dpy, (Visual *)x11Visual(), x11Depth(),
			       ZPixmap, 0, (char *)dptr, w, h, 32, 0 );
	    XPutImage( dpy, pm.handle(), gc, xi, 0, 0, 0, 0, w, h);
	    qSafeXDestroyImage( xi );
#if defined(QT_MITSHM_XFORM)
	}
#endif

	if ( data->mask ) // xform mask, too
	    pm.setMask( data->mask->xForm(matrix) );

#ifndef TQT_NO_XFTFREETYPE
	if ( tqt_use_xrender && tqt_has_xft && data->alphapm ) { // xform the alpha channel
	    XImage *axi = 0;
	    if ((axi = XGetImage(x11Display(), data->alphapm->handle(),
				 0, 0, ws, hs, AllPlanes, ZPixmap))) {
		sbpl = axi->bytes_per_line;
		sptr = (uchar *) axi->data;
		bpp  = axi->bits_per_pixel;
		dbytes = dbpl * h;
		dptr = (uchar *) malloc(dbytes);
		TQ_CHECK_PTR( dptr );
		memset(dptr, 0, dbytes);
		if ( axi->bitmap_bit_order == MSBFirst )
		    type = QT_XFORM_TYPE_MSBFIRST;
		else
		    type = QT_XFORM_TYPE_LSBFIRST;

		if (qt_xForm_helper( mat, axi->xoffset, type, bpp, dptr, w,
				     0, h, sptr, sbpl, ws, hs )) {
		    delete pm.data->alphapm;
		    pm.data->alphapm = new TQPixmap; // create a null pixmap

		    // setup pixmap data
		    pm.data->alphapm->data->w = w;
		    pm.data->alphapm->data->h = h;
		    pm.data->alphapm->data->d = 8;

		    // create 8bpp pixmap and render picture
		    pm.data->alphapm->hd =
			XCreatePixmap(x11Display(),
				      RootWindow(x11Display(), x11Screen()),
				      w, h, 8);

		    pm.data->alphapm->rendhd =
			(HANDLE) XftDrawCreateAlpha( x11Display(),
						     pm.data->alphapm->hd, 8 );

		    XImage *axi2 = XCreateImage(x11Display(), (Visual *) x11Visual(),
						8, ZPixmap, 0, (char *)dptr, w, h, 8, 0);

		    if (axi2) {
			// the data is deleted by qSafeXDestroyImage
			GC gc = XCreateGC(x11Display(), pm.data->alphapm->hd, 0, 0);
			XPutImage(dpy, pm.data->alphapm->hd, gc, axi2, 0, 0, 0, 0, w, h);
			XFreeGC(x11Display(), gc);
			qSafeXDestroyImage(axi2);
		    }
		}
		qSafeXDestroyImage(axi);
	    }
	}
#endif // TQT_NO_XFTFREETYPE

	return pm;
    }
}


/*!
  \internal
*/
int TQPixmap::x11SetDefaultScreen( int screen )
{
    int old = defaultScreen;
    defaultScreen = screen;
    return old;
}

/*!
  \internal
*/
void TQPixmap::x11SetScreen( int screen )
{
    if ( screen < 0 )
	screen = x11AppScreen();

    if ( screen == x11Screen() )
	return; // nothing to do

    if ( isNull() ) {
	TQPaintDeviceX11Data* xd = getX11Data( true );
	xd->x_screen = screen;
	xd->x_depth = TQPaintDevice::x11AppDepth( screen );
	xd->x_cells = TQPaintDevice::x11AppCells( screen );
	xd->x_colormap = TQPaintDevice::x11AppColormap( screen );
	xd->x_defcolormap = TQPaintDevice::x11AppDefaultColormap( screen );
	xd->x_visual = TQPaintDevice::x11AppVisual( screen );
	xd->x_defvisual = TQPaintDevice::x11AppDefaultVisual( screen );
    	setX11Data( xd );
	return;
    }
#if 0
    tqDebug("TQPixmap::x11SetScreen for %p from %d to %d. Size is %d/%d", data, x11Screen(), screen, width(), height() );
#endif

    TQImage img = convertToImage();
    resize(0,0);
    TQPaintDeviceX11Data* xd = getX11Data( true );
    xd->x_screen = screen;
    xd->x_depth = TQPaintDevice::x11AppDepth( screen );
    xd->x_cells = TQPaintDevice::x11AppCells( screen );
    xd->x_colormap = TQPaintDevice::x11AppColormap( screen );
    xd->x_defcolormap = TQPaintDevice::x11AppDefaultColormap( screen );
    xd->x_visual = TQPaintDevice::x11AppVisual( screen );
    xd->x_defvisual = TQPaintDevice::x11AppDefaultVisual( screen );
    setX11Data( xd );
    convertFromImage( img );
}

/*!
    Returns true this pixmap has an alpha channel or a mask.

    \sa hasAlphaChannel() mask()
*/
bool TQPixmap::hasAlpha() const
{
    return data->alphapm || data->mask;
}

/*!
    Returns true if the pixmap has an alpha channel; otherwise it
    returns false.

    NOTE: If the pixmap has a mask but not alpha channel, this
    function returns false.

    \sa hasAlpha() mask()
*/
bool TQPixmap::hasAlphaChannel() const
{
    return data->alphapm != 0;
}

/*!
    \relates TQPixmap

    Copies a block of pixels from \a src to \a dst.  The alpha channel
    and mask data (if any) is also copied from \a src.  NOTE: \a src
    is \e not alpha blended or masked when copied to \a dst.  Use
    bitBlt() or TQPainter::drawPixmap() to perform alpha blending or
    masked drawing.

    \a sx, \a sy is the top-left pixel in \a src (0, 0 by default), \a
    dx, \a dy is the top-left position in \a dst and \a sw, \sh is the
    size of the copied block (all of \a src by default).

    If \a src, \a dst, \a sw or \a sh is 0 (zero), copyBlt() does
    nothing.  If \a sw or \a sh is negative, copyBlt() copies starting
    at \a sx (and respectively, \a sy) and ending at the right edge
    (and respectively, the bottom edge) of \a src.

    copyBlt() does nothing if \a src and \a dst have different depths.
*/
TQ_EXPORT void copyBlt( TQPixmap *dst, int dx, int dy,
		       const TQPixmap *src, int sx, int sy, int sw, int sh )
{
    if ( ! dst || ! src || sw == 0 || sh == 0 || dst->depth() != src->depth() ) {
#ifdef QT_CHECK_NULL
	Q_ASSERT( dst != 0 );
	Q_ASSERT( src != 0 );
#endif
	return;
    }

    // copy pixel data
    bitBlt( dst, dx, dy, src, sx, sy, sw, sh, TQt::CopyROP, true );

    // copy mask data
    if ( src->data->mask ) {
	if ( ! dst->data->mask ) {
	    dst->data->mask = new TQBitmap( dst->width(), dst->height() );

	    // new masks are fully opaque by default
	    dst->data->mask->fill( TQt::color1 );
	}

	bitBlt( dst->data->mask, dx, dy,
		src->data->mask, sx, sy, sw, sh, TQt::CopyROP, true );
    }

#ifndef TQT_NO_XFTFREETYPE
    // copy alpha data
    extern bool tqt_use_xrender; // from qapplication_x11.cpp
    if ( ! tqt_use_xrender || ! src->data->alphapm )
	return;

    if ( sw < 0 )
	sw = src->width() - sx;
    else
	sw = TQMIN( src->width()-sx, sw );
    sw = TQMIN( dst->width()-dx, sw );

    if ( sh < 0 )
	sh = src->height() - sy ;
    else
	sh = TQMIN( src->height()-sy, sh );
    sh = TQMIN( dst->height()-dy, sh );

    if ( sw <= 0 || sh <= 0 )
	return;

    // create an alpha pixmap for dst if it doesn't exist
    bool do_init = false;
    if ( ! dst->data->alphapm ) {
	dst->data->alphapm = new TQPixmap;

	// setup pixmap d
	dst->data->alphapm->data->w = dst->width();
	dst->data->alphapm->data->h = dst->height();
	dst->data->alphapm->data->d = 8;

	// create 8bpp pixmap and render picture
	dst->data->alphapm->hd =
	    XCreatePixmap(dst->x11Display(),
			  RootWindow(dst->x11Display(), dst->x11Screen()),
			  dst->width(), dst->height(), 8);

	// new alpha pixmaps should be fully opaque by default
	do_init = true;

	dst->data->alphapm->rendhd =
	    (TQt::HANDLE) XftDrawCreateAlpha( dst->x11Display(),
					     dst->data->alphapm->hd, 8 );
    }

    GC gc = XCreateGC(dst->x11Display(), dst->data->alphapm->hd, 0, 0);

    if ( do_init ) {
	// the alphapm was just created, make it fully opaque
	XSetForeground( dst->x11Display(), gc, 255 );
	XSetBackground( dst->x11Display(), gc, 255 );
	XFillRectangle( dst->x11Display(), dst->data->alphapm->hd, gc,
			0, 0, dst->data->alphapm->data->w,
			dst->data->alphapm->data->h );
    }

    XCopyArea(dst->x11Display(), src->data->alphapm->hd, dst->data->alphapm->hd, gc,
	      sx, sy, sw, sh, dx, dy);
    XFreeGC(dst->x11Display(), gc);
#endif // TQT_NO_XFTFREETYPE
}
