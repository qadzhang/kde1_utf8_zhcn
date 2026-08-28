/****************************************************************************
**
** Implementation of TQPaintDevice class for X11
**
** Created : 940721
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

#include "ntqpaintdevice.h"
#include "ntqpaintdevicemetrics.h"
#include "ntqpainter.h"
#include "ntqwidget.h"
#include "ntqbitmap.h"
#include "ntqapplication.h"
#include "qt_x11_p.h"


/*!
    \class TQPaintDevice ntqpaintdevice.h
    \brief The TQPaintDevice class is the base class of objects that
    can be painted.

    \ingroup graphics
    \ingroup images

    A paint device is an abstraction of a two-dimensional space that
    can be drawn using a TQPainter. The drawing capabilities are
    implemented by the subclasses TQWidget, TQPixmap, TQPicture and
    TQPrinter.

    The default coordinate system of a paint device has its origin
    located at the top-left position. X increases to the right and Y
    increases downward. The unit is one pixel. There are several ways
    to set up a user-defined coordinate system using the painter, for
    example, using TQPainter::setWorldMatrix().

    Example (draw on a paint device):
    \code
    void MyWidget::paintEvent( TQPaintEvent * )
    {
	TQPainter p;                       // our painter
	p.begin( this );                  // start painting the widget
	p.setPen( red );                  // red outline
	p.setBrush( yellow );             // yellow fill
	p.drawEllipse( 10, 20, 100,100 ); // 100x100 ellipse at position (10, 20)
	p.end();                          // painting done
    }
    \endcode

    The bit block transfer is an extremely useful operation for
    copying pixels from one paint device to another (or to itself). It
    is implemented as the global function bitBlt().

    Example (scroll widget contents 10 pixels to the right):
    \code
    bitBlt( myWidget, 10, 0, myWidget );
    \endcode

    \warning TQt requires that a TQApplication object exists before
    any paint devices can be created. Paint devices access window
    system resources, and these resources are not initialized before
    an application object is created.
*/


//
// Some global variables - these are initialized by TQColor::initialize()
//

Display *TQPaintDevice::x_appdisplay = 0;
int	 TQPaintDevice::x_appscreen;

int	  TQPaintDevice::x_appdepth;
int	  TQPaintDevice::x_appcells;
TQt::HANDLE  TQPaintDevice::x_approotwindow;
TQt::HANDLE  TQPaintDevice::x_appcolormap;
bool	  TQPaintDevice::x_appdefcolormap;
void	 *TQPaintDevice::x_appvisual;
bool	  TQPaintDevice::x_appdefvisual;

// ### in 4.0, remove the above, and use the below
int	 *TQPaintDevice::x_appdepth_arr;
int	 *TQPaintDevice::x_appcells_arr;
TQt::HANDLE *TQPaintDevice::x_approotwindow_arr;
TQt::HANDLE *TQPaintDevice::x_appcolormap_arr;
bool	 *TQPaintDevice::x_appdefcolormap_arr;
void	**TQPaintDevice::x_appvisual_arr;
bool	 *TQPaintDevice::x_appdefvisual_arr;

/*!
    \enum TQPaintDevice::PDevCmd
    \internal
*/

/*!
    Constructs a paint device with internal flags \a devflags. This
    constructor can be invoked only from TQPaintDevice subclasses.
*/

TQPaintDevice::TQPaintDevice( uint devflags )
{
    if ( !tqApp ) {				// global constructor
#if defined(QT_CHECK_STATE)
	tqFatal( "TQPaintDevice: Must construct a TQApplication before a "
		"TQPaintDevice" );
#endif
	return;
    }
    devFlags = devflags;
    painters = 0;
    hd = 0;
    rendhd = 0;
    x11Data = 0;
}

/*!
    Destroys the paint device and frees window system resources.
*/

TQPaintDevice::~TQPaintDevice()
{
#if defined(QT_CHECK_STATE)
    if ( paintingActive() )
	tqWarning( "TQPaintDevice: Cannot destroy paint device that is being "
		  "painted" );
#endif
    if ( x11Data && x11Data->deref() ) {
	delete x11Data;
	x11Data = 0;
    }
}


/*
  \internal
  Makes a shallow copy of the X11-specific data of \a fromDevice, if it is not
  null. Otherwise this function sets it to null.
*/

void TQPaintDevice::copyX11Data( const TQPaintDevice *fromDevice )
{
    setX11Data( fromDevice ? fromDevice->x11Data : 0 );
}

/*
  \internal
  Makes a deep copy of the X11-specific data of \a fromDevice, if it is not
  null. Otherwise this function sets it to null.
*/

void TQPaintDevice::cloneX11Data( const TQPaintDevice *fromDevice )
{
    if ( fromDevice && fromDevice->x11Data ) {
	TQPaintDeviceX11Data *d = new TQPaintDeviceX11Data;
	*d = *fromDevice->x11Data;
	d->count = 0;
	setX11Data( d );
    } else {
	setX11Data( 0 );
    }
}

/*
  \internal
  Makes a shallow copy of the X11-specific data \a d and assigns it to this
  class. This function increments the reference code of \a d.
*/

void TQPaintDevice::setX11Data( const TQPaintDeviceX11Data* d )
{
    if ( x11Data && x11Data->deref() )
	delete x11Data;
    x11Data = (TQPaintDeviceX11Data*)d;
    if ( x11Data )
	x11Data->ref();
}


/*
  \internal
  If \a def is false, returns a deep copy of the x11Data, or 0 if x11Data is 0.
  If \a def is true, makes a TQPaintDeviceX11Data struct filled with the default
  values.

  In either case the caller is responsible for deleting the returned
  struct. But notice that the struct is a shared class, so other
  classes might also have a reference to it. The reference count of
  the returned TQPaintDeviceX11Data* is 0.
*/

TQPaintDeviceX11Data* TQPaintDevice::getX11Data( bool def ) const
{
    TQPaintDeviceX11Data* res = 0;
    if ( def ) {
	res = new TQPaintDeviceX11Data;
	res->x_display = x11AppDisplay();
	res->x_screen = x11AppScreen();
	res->x_depth = x11AppDepth();
	res->x_cells = x11AppCells();
	res->x_colormap = x11Colormap();
	res->x_defcolormap = x11AppDefaultColormap();
	res->x_visual = x11AppVisual();
	res->x_defvisual = x11AppDefaultVisual();
	res->deref();
    } else if ( x11Data ) {
	res = new TQPaintDeviceX11Data;
	*res = *x11Data;
	res->count = 0;
    }
    return res;
}


/*!
    \fn int TQPaintDevice::devType() const

    \internal

    Returns the device type identifier, which is \c TQInternal::Widget
    if the device is a TQWidget, \c TQInternal::Pixmap if it's a
    TQPixmap, \c TQInternal::Printer if it's a TQPrinter, \c
    TQInternal::Picture if it's a TQPicture or \c
    TQInternal::UndefinedDevice in other cases (which should never
    happen).
*/

/*!
    \fn bool TQPaintDevice::isExtDev() const

    Returns true if the device is an external paint device; otherwise
    returns false.

    External paint devices cannot be bitBlt()'ed from. TQPicture and
    TQPrinter are external paint devices.
*/

/*!
    Returns the window system handle of the paint device, for
    low-level access. Using this function is not portable.

    The HANDLE type varies with platform; see \c ntqpaintdevice.h and
    \c ntqwindowdefs.h for details.

    \sa x11Display()
*/
TQt::HANDLE TQPaintDevice::handle() const
{
    return hd;
}

/*!
    Returns the window system handle of the paint device for XRender
    support. Use of this function is not portable. This function will
    return 0 if XRender support is not compiled into TQt, if the
    XRender extension is not supported on the X11 display, or if the
    handle could not be created.
*/
TQt::HANDLE TQPaintDevice::x11RenderHandle() const
{
#ifndef TQT_NO_XFTFREETYPE
    return rendhd ? XftDrawPicture( (XftDraw *) rendhd ) : 0;
#else
    return 0;
#endif // TQT_NO_XFTFREETYPE
}


/*!
    \fn Display *TQPaintDevice::x11AppDisplay()

    Returns a pointer to the X display global to the application (X11
    only). Using this function is not portable.

    \sa handle()
*/

/*!
    \fn int TQPaintDevice::x11AppScreen()

    Returns the screen number on the X display global to the
    application (X11 only). Using this function is not portable.
*/

/*!
    \overload
    \fn int TQPaintDevice::x11AppDepth()

    Returns the depth for the default screen of the X display global
    to the application (X11 only). Using this function is not
    portable.

    \sa TQPixmap::defaultDepth()
*/

/*!
    \fn int TQPaintDevice::x11AppCells()

    Returns the number of entries in the colormap for the default
    screen of the X display global to the application (X11
    only). Using this function is not portable.

    \sa x11Colormap()
*/

/*!
    \fn HANDLE TQPaintDevice::x11AppRootWindow()

    Returns the root window for the default screen of the X display
    global to the applicatoin (X11 only). Using this function is not
    portable.
*/

/*!
    \fn HANDLE TQPaintDevice::x11AppColormap()

    Returns the colormap for the default screen of the X display
    global to the application (X11 only). Using this function is not
    portable.

    \sa x11Cells()
*/

/*!
    \fn bool TQPaintDevice::x11AppDefaultColormap ()

    Returns the default colormap for the default screen of the X
    display global to the application (X11 only). Using this function
    is not portable.

    \sa x11Cells()
*/

/*!
    \fn void* TQPaintDevice::x11AppVisual ()

    Returns the Visual for the default screen of the X display global
    to the application (X11 only). Using this function is not
    portable.
*/

/*!
    \fn bool TQPaintDevice::x11AppDefaultVisual ()

    Returns true if the Visual used is the default for the default
    screen of the X display global to the application (X11 only);
    otherwise returns false. Using this function is not portable.
*/

/*!
    \fn int TQPaintDevice::x11AppDepth( int screen )

    Returns the depth for screen \a screen of the X display global to
    the application (X11 only). Using this function is not portable.

    \sa TQPixmap::defaultDepth()
*/

/*!
    \overload
    \fn int TQPaintDevice::x11AppCells( int screen )

    Returns the number of entries in the colormap for screen \a screen
    of the X display global to the application (X11 only). Using this
    function is not portable.

    \sa x11Colormap()
*/

/*!
    \overload
    \fn HANDLE TQPaintDevice::x11AppRootWindow( int screen )

    Returns the root window for screen \a screen of the X display
    global to the applicatoin (X11 only). Using this function is not
    portable.
*/

/*!
    \overload
    \fn HANDLE TQPaintDevice::x11AppColormap( int screen )

    Returns the colormap for screen \a screen of the X display global
    to the application (X11 only). Using this function is not
    portable.

    \sa x11Cells()
*/

/*!
    \overload
    \fn bool TQPaintDevice::x11AppDefaultColormap( int screen )

    Returns the default colormap for screen \a screen of the X display
    global to the application (X11 only). Using this function is not
    portable.

    \sa x11Cells()
*/

/*!
    \overload
    \fn void* TQPaintDevice::x11AppVisual( int screen )

    Returns the Visual for screen \a screen of the X display global to
    the application (X11 only). Using this function is not portable.
*/

/*!
    \overload
    \fn bool TQPaintDevice::x11AppDefaultVisual( int screen )

    Returns true if the Visual used is the default for screen
    \a screen of the X display global to the application (X11 only);
    otherwise returns false. Using this function is not portable.
*/


/*!
    \fn Display *TQPaintDevice::x11Display() const

    Returns a pointer to the X display for the paint device (X11
    only). Using this function is not portable.

    \sa handle()
*/

/*!
    \fn int TQPaintDevice::x11Screen () const

    Returns the screen number on the X display for the paint device
    (X11 only). Using this function is not portable.
*/

/*!
    \fn int TQPaintDevice::x11Depth () const

    Returns the depth of the X display for the paint device (X11
    only). Using this function is not portable.

    \sa TQPixmap::defaultDepth()
*/

/*!
    \fn int TQPaintDevice::x11Cells () const

    Returns the number of entries in the colormap of the X display for
    the paint device (X11 only). Using this function is not portable.

    \sa x11Colormap()
*/

/*!
    \fn HANDLE TQPaintDevice::x11Colormap () const

    Returns the colormap of the X display for the paint device (X11
    only). Using this function is not portable.

    \sa x11Cells()
*/

/*!
    \fn bool TQPaintDevice::x11DefaultColormap () const

    Returns the default colormap of the X display for the paint device
    (X11 only). Using this function is not portable.

    \sa x11Cells()
*/

/*!
    \fn void* TQPaintDevice::x11Visual () const

    Returns the Visual of the X display for the paint device (X11
    only). Using this function is not portable.
*/

/*!
    \fn bool TQPaintDevice::x11DefaultVisual () const

    Returns the default Visual of the X display for the paint device
    (X11 only). Using this function is not portable.
*/

static int *dpisX=0, *dpisY=0;
static void create_dpis()
{
    if ( dpisX )
	return;

    Display *dpy = TQPaintDevice::x11AppDisplay();
    if ( ! dpy )
	return;

    int i, screens =  ScreenCount( dpy );
    dpisX = new int[ screens ];
    dpisY = new int[ screens ];
    TQ_CHECK_PTR( dpisX );
    TQ_CHECK_PTR( dpisY );
    for ( i = 0; i < screens; i++ ) {
       if (DisplayWidthMM(dpy,i) < 1)
               dpisX[ i ] = 75; // default the dpi to 75.
       else
               dpisX[ i ] = (DisplayWidth(dpy,i) * 254 + DisplayWidthMM(dpy,i)*5)
                     / (DisplayWidthMM(dpy,i)*10);
       if (DisplayHeightMM(dpy,i) < 1)
               dpisY[ i ] = 75; // default the dpi to 75.
       else
               dpisY[ i ] = (DisplayHeight(dpy,i) * 254 + DisplayHeightMM(dpy,i)*5)
                     / (DisplayHeightMM(dpy,i)*10);
    }
}

/*!
    Sets the value returned by x11AppDpiX() to \a dpi for screen
    \a screen. The default is determined by the display configuration.
    Changing this value will alter the scaling of fonts and many other
    metrics and is not recommended. Using this function is not
    portable.

    \sa x11SetAppDpiY()
*/
void TQPaintDevice::x11SetAppDpiX(int dpi, int screen)
{
    create_dpis();
    if ( ! dpisX )
	return;
    if ( screen < 0 )
	screen = TQPaintDevice::x11AppScreen();
    if ( screen > ScreenCount( TQPaintDevice::x11AppDisplay() ) )
	return;
    dpisX[ screen ] = dpi;
}

/*!
    \overload

    Sets the value returned by x11AppDpiX() to \a dpi for the default
    screen. The default is determined by the display configuration.
    Changing this value will alter the scaling of fonts and many other
    metrics and is not recommended. Using this function is not
    portable.

*/
// ### REMOVE 4.0
void TQPaintDevice::x11SetAppDpiX( int dpi )
{
    TQPaintDevice::x11SetAppDpiX( dpi, -1 );
}

/*!
    Sets the value returned by x11AppDpiY() to \a dpi for screen
    \a screen. The default is determined by the display configuration.
    Changing this value will alter the scaling of fonts and many other
    metrics and is not recommended. Using this function is not
    portable.

    \sa x11SetAppDpiX()
*/
void TQPaintDevice::x11SetAppDpiY(int dpi, int screen)
{
    create_dpis();
    if ( ! dpisY )
	return;
    if ( screen < 0 )
	screen = TQPaintDevice::x11AppScreen();
    if ( screen > ScreenCount( TQPaintDevice::x11AppDisplay() ) )
	return;
    dpisY[ screen ] = dpi;
}

/*!
    \overload

    Sets the value returned by x11AppDpiY() to \a dpi for the default
    screen.  The default is determined by the display configuration.
    Changing this value will alter the scaling of fonts and many other
    metrics and is not recommended. Using this function is not
    portable.
*/
// ### REMOVE 4.0
void TQPaintDevice::x11SetAppDpiY( int dpi )
{
    TQPaintDevice::x11SetAppDpiY( dpi, -1 );
}

/*!
    Returns the horizontal DPI of the X display (X11 only) for screen
    \a screen. Using this function is not portable. See
    TQPaintDeviceMetrics for portable access to related information.
    Using this function is not portable.

    \sa x11AppDpiY(), x11SetAppDpiX(), TQPaintDeviceMetrics::logicalDpiX()
*/
int TQPaintDevice::x11AppDpiX(int screen)
{
    create_dpis();
    if ( ! dpisX )
	return 0;
    if ( screen < 0 )
	screen = TQPaintDevice::x11AppScreen();
    if ( screen > ScreenCount( TQPaintDevice::x11AppDisplay() ) )
	return 0;
    return dpisX[ screen ];
}

/*!
    \overload

    Returns the horizontal DPI of the X display (X11 only) for the
    default screen. Using this function is not portable. See
    TQPaintDeviceMetrics for portable access to related information.
    Using this function is not portable.
*/
int TQPaintDevice::x11AppDpiX()
{
    return TQPaintDevice::x11AppDpiX( -1 );
}

/*!
    Returns the vertical DPI of the X11 display (X11 only) for screen
    \a screen.  Using this function is not portable. See
    TQPaintDeviceMetrics for portable access to related information.
    Using this function is not portable.

    \sa x11AppDpiX(), x11SetAppDpiY(), TQPaintDeviceMetrics::logicalDpiY()
*/
int TQPaintDevice::x11AppDpiY( int screen )
{
    create_dpis();
    if ( ! dpisY )
	return 0;
    if ( screen < 0 )
	screen = TQPaintDevice::x11AppScreen();
    if ( screen > ScreenCount( TQPaintDevice::x11AppDisplay() ) )
	return 0;
    return dpisY[ screen ];
}

/*!
    \overload

    Returns the vertical DPI of the X11 display (X11 only) for the
    default screen.  Using this function is not portable. See
    TQPaintDeviceMetrics for portable access to related information.
    Using this function is not portable.

    \sa x11AppDpiX(), x11SetAppDpiY(), TQPaintDeviceMetrics::logicalDpiY()
*/
int TQPaintDevice::x11AppDpiY()
{
    return TQPaintDevice::x11AppDpiY( -1 );
}

/*!
    \fn bool TQPaintDevice::paintingActive() const

    Returns true if the device is being painted, i.e. someone has
    called TQPainter::begin() but not yet called TQPainter::end() for
    this device; otherwise returns false.

    \sa TQPainter::isActive()
*/

/*!
    Internal virtual function that interprets drawing commands from
    the painter.

    Implemented by subclasses that have no direct support for drawing
    graphics (external paint devices, for example, TQPicture).
*/

bool TQPaintDevice::cmd( int, TQPainter *, TQPDevCmdParam * )
{
#if defined(QT_CHECK_STATE)
    tqWarning( "TQPaintDevice::cmd: Device has no command interface" );
#endif
    return false;
}

/*!
    \internal

    Internal virtual function that returns paint device metrics.

    Please use the TQPaintDeviceMetrics class instead.
*/

int TQPaintDevice::metric( int ) const
{
#if defined(QT_CHECK_STATE)
    tqWarning( "TQPaintDevice::metrics: Device has no metric information" );
#endif
    return 0;
}

/*!
    \internal

    Internal virtual function. Reserved for future use.

    Please use the TQFontMetrics class instead.
*/

int TQPaintDevice::fontMet( TQFont *, int, const char *, int ) const
{
    return 0;
}

/*!
    \internal

    Internal virtual function. Reserved for future use.

    Please use the TQFontInfo class instead.
*/

int TQPaintDevice::fontInf( TQFont *, int ) const
{
    return 0;
}


//
// Internal functions for simple GC caching for blt'ing masked pixmaps.
// This cache is used when the pixmap optimization is set to Normal
// and the pixmap size doesn't exceed 128x128.
//

static bool      init_mask_gc = false;
static const int max_mask_gcs = 11;		// suitable for hashing

struct mask_gc {
    GC	gc;
    int mask_no;
};

static mask_gc gc_vec[max_mask_gcs];


static void cleanup_mask_gc()
{
    Display *dpy = TQPaintDevice::x11AppDisplay();
    init_mask_gc = false;
    for ( int i=0; i<max_mask_gcs; i++ ) {
	if ( gc_vec[i].gc )
	    XFreeGC( dpy, gc_vec[i].gc );
    }
}

static GC cache_mask_gc( Display *dpy, Drawable hd, int mask_no, Pixmap mask )
{
    if ( !init_mask_gc ) {			// first time initialization
	init_mask_gc = true;
	tqAddPostRoutine( cleanup_mask_gc );
	for ( int i=0; i<max_mask_gcs; i++ )
	    gc_vec[i].gc = 0;
    }
    mask_gc *p = &gc_vec[mask_no % max_mask_gcs];
    if ( !p->gc || p->mask_no != mask_no ) {	// not a perfect match
	if ( !p->gc ) {				// no GC
	    p->gc = XCreateGC( dpy, hd, 0, 0 );
	    XSetGraphicsExposures( dpy, p->gc, False );
	}
	XSetClipMask( dpy, p->gc, mask );
	p->mask_no = mask_no;
    }
    return p->gc;
}


/*!
    \relates TQPaintDevice

    Copies a block of pixels from \a src to \a dst, perhaps merging
    each pixel according to the \link TQt::RasterOp raster operation \endlink
    \a rop. \a sx, \a sy
    is the top-left pixel in \a src (0, 0) by default, \a dx, \a dy is
    the top-left position in \a dst and \a sw, \a sh is the size of
    the copied block (all of \a src by default).

    The most common values for \a rop are CopyROP and XorROP; the \l
    TQt::RasterOp documentation defines all the possible values.

    If \a ignoreMask is false (the default) and \a src is a
    masked TQPixmap, the entire blit is masked by \a{src}->mask().

    If \a src, \a dst, \a sw or \a sh is 0, bitBlt() does nothing. If
    \a sw or \a sh is negative bitBlt() copies starting at \a sx (and
    respectively, \a sy) and ending at the right end (respectively,
    bottom) of \a src.

    \a src must be a TQWidget or TQPixmap. You cannot blit from a
    TQPrinter, for example. bitBlt() does nothing if you attempt to
    blit from an unsupported device.

    bitBlt() does nothing if \a src has a greater depth than \e dst.
    If you need to for example, draw a 24-bit pixmap on an 8-bit
    widget, you must use drawPixmap().
*/

void bitBlt( TQPaintDevice *dst, int dx, int dy,
	     const TQPaintDevice *src, int sx, int sy, int sw, int sh,
	     TQt::RasterOp rop, bool ignoreMask )
{
    if ( !src || !dst ) {
#if defined(QT_CHECK_NULL)
	Q_ASSERT( src != 0 );
	Q_ASSERT( dst != 0 );
#endif
	return;
    }
    if ( !src->handle() || src->isExtDev() )
	return;

    TQPaintDevice *pdev = TQPainter::redirect( dst );
    if ( pdev )
	dst = pdev;

    int ts = src->devType();			// from device type
    int td = dst->devType();			// to device type
    Display *dpy = src->x11Display();

    if ( sw <= 0 ) {				// special width
	if ( sw < 0 )
	    sw = src->metric( TQPaintDeviceMetrics::PdmWidth ) - sx;
	else
	    return;
    }
    if ( sh <= 0 ) {				// special height
	if ( sh < 0 )
	    sh = src->metric( TQPaintDeviceMetrics::PdmHeight ) - sy;
	else
	    return;
    }

    if ( dst->paintingActive() && dst->isExtDev() ) {
	TQPixmap *pm;				// output to picture/printer
	bool	 tmp_pm = true;
	if ( ts == TQInternal::Pixmap ) {
	    pm = (TQPixmap*)src;
	    if ( sx != 0 || sy != 0 ||
		 sw != pm->width() || sh != pm->height() || ignoreMask ) {
		TQPixmap *tmp = new TQPixmap( sw, sh, pm->depth() );
		bitBlt( tmp, 0, 0, pm, sx, sy, sw, sh, TQt::CopyROP, true );
		if ( pm->mask() && !ignoreMask ) {
		    TQBitmap mask( sw, sh );
		    bitBlt( &mask, 0, 0, pm->mask(), sx, sy, sw, sh,
			    TQt::CopyROP, true );
		    tmp->setMask( mask );
		}
		pm = tmp;
	    } else {
		tmp_pm = false;
	    }
	} else if ( ts == TQInternal::Widget ) {// bitBlt to temp pixmap
	    pm = new TQPixmap( sw, sh );
	    TQ_CHECK_PTR( pm );
	    bitBlt( pm, 0, 0, src, sx, sy, sw, sh );
	} else {
#if defined(QT_CHECK_RANGE)
	    tqWarning( "bitBlt: Cannot bitBlt from device" );
#endif
	    return;
	}
	TQPDevCmdParam param[3];
    TQRect r(dx, dy, pm->width(), pm->height());
	param[0].rect	= &r;
	param[1].pixmap = pm;
	dst->cmd( TQPaintDevice::PdcDrawPixmap, 0, param );
	if ( tmp_pm )
	    delete pm;
	return;
    }

    switch ( ts ) {
    case TQInternal::Widget:
    case TQInternal::Pixmap:
    case TQInternal::System:			// OK, can blt from these
	break;
    default:
#if defined(QT_CHECK_RANGE)
	tqWarning( "bitBlt: Cannot bitBlt from device type %x", ts );
#endif
	return;
    }
    switch ( td ) {
    case TQInternal::Widget:
    case TQInternal::Pixmap:
    case TQInternal::System:			// OK, can blt to these
	break;
    default:
#if defined(QT_CHECK_RANGE)
	tqWarning( "bitBlt: Cannot bitBlt to device type %x", td );
#endif
	return;
    }

    static const short ropCodes[] = {			// ROP translation table
	GXcopy, GXor, GXxor, GXandInverted,
	GXcopyInverted, GXorInverted, GXequiv, GXand,
	GXinvert, GXclear, GXset, GXnoop,
	GXandReverse, GXorReverse, GXnand, GXnor
    };
    if ( rop > TQt::LastROP ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "bitBlt: Invalid ROP code" );
#endif
	return;
    }

    if ( dst->handle() == 0 ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "bitBlt: Cannot bitBlt to device" );
#endif
	return;
    }

    bool mono_src;
    bool mono_dst;
    bool include_inferiors = false;
    bool graphics_exposure = false;
    TQPixmap *src_pm;
    TQBitmap *mask;

    if ( ts == TQInternal::Pixmap ) {
	src_pm = (TQPixmap*)src;
    	if ( src_pm->x11Screen() != dst->x11Screen() )
    	    src_pm->x11SetScreen( dst->x11Screen() );
	mono_src = src_pm->depth() == 1;
	mask = ignoreMask ? 0 : src_pm->data->mask;
    } else {
	src_pm = 0;
	mono_src = false;
	mask = 0;
	include_inferiors = ((TQWidget*)src)->testWFlags(TQt::WPaintUnclipped);
	graphics_exposure = td == TQInternal::Widget;
    }
    if ( td == TQInternal::Pixmap ) {
      	if ( dst->x11Screen() != src->x11Screen() )
      	    ((TQPixmap*)dst)->x11SetScreen( src->x11Screen() );
	mono_dst = ((TQPixmap*)dst)->depth() == 1;
	((TQPixmap*)dst)->detach();		// changes shared pixmap
    } else {
	mono_dst = false;
	include_inferiors = include_inferiors ||
			    ((TQWidget*)dst)->testWFlags(TQt::WPaintUnclipped);
    }

    if ( mono_dst && !mono_src ) {	// dest is 1-bit pixmap, source is not
#if defined(QT_CHECK_RANGE)
	tqWarning( "bitBlt: Incompatible destination pixmap" );
#endif
	return;
    }

#ifndef TQT_NO_XRENDER
    if (src_pm && !mono_src && src_pm->data->alphapm && !ignoreMask ) {
	// use RENDER to do the blit
	TQPixmap *alpha = src_pm->data->alphapm;
	if (src->x11RenderHandle() &&
	    alpha->x11RenderHandle() &&
	    dst->x11RenderHandle()) {
	    XRenderPictureAttributes pattr;
	    ulong picmask = 0;
	    if (include_inferiors) {
		pattr.subwindow_mode = IncludeInferiors;
		picmask |= CPSubwindowMode;
	    }
	    if (graphics_exposure) {
		pattr.graphics_exposures = true;
		picmask |= CPGraphicsExposure;
	    }
	    if (picmask)
		XRenderChangePicture(dpy, dst->x11RenderHandle(), picmask, &pattr);
	    XRenderComposite(dpy, PictOpOver, src->x11RenderHandle(),
			     alpha->x11RenderHandle(), dst->x11RenderHandle(),
			     sx, sy, sx, sy, dx, dy, sw, sh);
	    // restore attributes
	    pattr.subwindow_mode = ClipByChildren;
	    pattr.graphics_exposures = false;
	    if (picmask)
		XRenderChangePicture(dpy, dst->x11RenderHandle(), picmask, &pattr);
	    return;
	}
    }
#endif

    GC gc;

    if ( mask && !mono_src ) {			// fast masked blt
	bool temp_gc = false;
	if ( mask->data->maskgc ) {
	    gc = (GC)mask->data->maskgc;	// we have a premade mask GC
	} else {
	    if ( false && src_pm->optimization() == TQPixmap::NormalOptim ) { // #### cache disabled
		// Compete for the global cache
		gc = cache_mask_gc( dpy, dst->handle(),
				    mask->data->ser_no,
				    mask->handle() );
	    } else {
		// Create a new mask GC. If BestOptim, we store the mask GC
		// with the mask (not at the pixmap). This way, many pixmaps
		// which have a common mask will be optimized at no extra cost.
		gc = XCreateGC( dpy, dst->handle(), 0, 0 );
		XSetGraphicsExposures( dpy, gc, False );
		XSetClipMask( dpy, gc, mask->handle() );
		if ( src_pm->optimization() == TQPixmap::BestOptim ) {
		    mask->data->maskgc = gc;
		} else {
		    temp_gc = true;
		}
	    }
	}
	XSetClipOrigin( dpy, gc, dx-sx, dy-sy );
	if ( rop != TQt::CopyROP )		// use non-default ROP code
	    XSetFunction( dpy, gc, ropCodes[rop] );
	if ( include_inferiors ) {
	    XSetSubwindowMode( dpy, gc, IncludeInferiors );
	    XCopyArea( dpy, src->handle(), dst->handle(), gc, sx, sy, sw, sh,
		       dx, dy );
	    XSetSubwindowMode( dpy, gc, ClipByChildren );
	} else {
	    XCopyArea( dpy, src->handle(), dst->handle(), gc, sx, sy, sw, sh,
		       dx, dy );
	}

	if ( temp_gc )				// delete temporary GC
	    XFreeGC( dpy, gc );
	else if ( rop != TQt::CopyROP )		// restore ROP
	    XSetFunction( dpy, gc, GXcopy );
	return;
    }

    gc = tqt_xget_temp_gc( dst->x11Screen(), mono_dst );		// get a reusable GC

    if ( rop != TQt::CopyROP )			// use non-default ROP code
	XSetFunction( dpy, gc, ropCodes[rop] );

    if ( mono_src && mono_dst && src == dst ) { // dst and src are the same bitmap
	XCopyArea( dpy, src->handle(), dst->handle(), gc, sx, sy, sw, sh, dx, dy );
    } else if ( mono_src ) {			// src is bitmap
	XGCValues gcvals;
	ulong	  valmask = GCBackground | GCForeground | GCFillStyle |
			    GCStipple | GCTileStipXOrigin | GCTileStipYOrigin;
	if ( td == TQInternal::Widget ) {	// set GC colors
	    TQWidget *w = (TQWidget *)dst;
	    gcvals.background = w->backgroundColor().pixel( dst->x11Screen() );
	    gcvals.foreground = w->foregroundColor().pixel( dst->x11Screen() );
	    if ( include_inferiors ) {
		valmask |= GCSubwindowMode;
		gcvals.subwindow_mode = IncludeInferiors;
	    }
	} else if ( mono_dst ) {
	    gcvals.background = 0;
	    gcvals.foreground = 1;
	} else {
	    gcvals.background = TQt::white.pixel( dst->x11Screen() );
	    gcvals.foreground = TQt::black.pixel( dst->x11Screen() );
	}

	gcvals.fill_style  = FillOpaqueStippled;
	gcvals.stipple	   = src->handle();
	gcvals.ts_x_origin = dx - sx;
	gcvals.ts_y_origin = dy - sy;

	bool clipmask = false;
	if ( mask ) {
	    if ( ((TQPixmap*)src)->data->selfmask ) {
		gcvals.fill_style = FillStippled;
	    } else {
		XSetClipMask( dpy, gc, mask->handle() );
		XSetClipOrigin( dpy, gc, dx-sx, dy-sy );
		clipmask = true;
	    }
	}

	XChangeGC( dpy, gc, valmask, &gcvals );
	XFillRectangle( dpy,dst->handle(), gc, dx, dy, sw, sh );

	valmask = GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin;
	gcvals.fill_style  = FillSolid;
	gcvals.ts_x_origin = 0;
	gcvals.ts_y_origin = 0;
	if ( include_inferiors ) {
	    valmask |= GCSubwindowMode;
	    gcvals.subwindow_mode = ClipByChildren;
	}
	XChangeGC( dpy, gc, valmask, &gcvals );

	if ( clipmask ) {
	    XSetClipOrigin( dpy, gc, 0, 0 );
	    XSetClipMask( dpy, gc, None );
	}

    } else {					// src is pixmap/widget

	if ( graphics_exposure )		// widget to widget
	    XSetGraphicsExposures( dpy, gc, True );
	if ( include_inferiors ) {
	    XSetSubwindowMode( dpy, gc, IncludeInferiors );
	    XCopyArea( dpy, src->handle(), dst->handle(), gc, sx, sy, sw, sh,
		       dx, dy );
	    XSetSubwindowMode( dpy, gc, ClipByChildren );
	} else {
	    XCopyArea( dpy, src->handle(), dst->handle(), gc, sx, sy, sw, sh,
		       dx, dy );
	}
	if ( graphics_exposure )		// reset graphics exposure
	    XSetGraphicsExposures( dpy, gc, False );
    }

    if ( rop != TQt::CopyROP )			// restore ROP
	XSetFunction( dpy, gc, GXcopy );
}


/*!
    \relates TQPaintDevice

    \overload void bitBlt( TQPaintDevice *dst, const TQPoint &dp, const TQPaintDevice *src, const TQRect &sr, RasterOp rop )

    Overloaded bitBlt() with the destination point \a dp and source
    rectangle \a sr.
*/


/*!
  \internal
*/
// makes it possible to add a setResolution as we have in TQPrinter for all
// paintdevices without breaking bin compatibility.
void TQPaintDevice::setResolution( int )
{
}

/*!\internal
*/
int TQPaintDevice::resolution() const
{
    return metric( TQPaintDeviceMetrics::PdmDpiY );
}
