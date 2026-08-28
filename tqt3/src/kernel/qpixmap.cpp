/****************************************************************************
**
** Implementation of TQPixmap class
**
** Created : 950301
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

#include "ntqpixmap.h"

#include "ntqbitmap.h"
#include "ntqimage.h"
#include "ntqwidget.h"
#include "ntqpainter.h"
#include "ntqdatastream.h"
#include "ntqbuffer.h"
#include "ntqobjectlist.h"
#include "ntqapplication.h"
#include <private/qinternal_p.h>
#include "ntqmime.h"
#include "ntqdragobject.h"
#include "ntqfile.h"

/*!
    \class TQPixmap ntqpixmap.h
    \brief The TQPixmap class is an off-screen, pixel-based paint device.

    \ingroup graphics
    \ingroup images
    \ingroup shared
    \mainclass

    TQPixmap is one of the two classes TQt provides for dealing with
    images; the other is TQImage. TQPixmap is designed and optimized
    for drawing; TQImage is designed and optimized for I/O and for
    direct pixel access/manipulation. There are (slow) functions to
    convert between TQImage and TQPixmap: convertToImage() and
    convertFromImage().

    One common use of the TQPixmap class is to enable smooth updating
    of widgets. Whenever something complex needs to be drawn, you can
    use a pixmap to obtain flicker-free drawing, like this:

    \list 1
    \i Create a pixmap with the same size as the widget.
    \i Fill the pixmap with the widget background color.
    \i Paint the pixmap.
    \i bitBlt() the pixmap contents onto the widget.
    \endlist

    Pixel data in a pixmap is internal and is managed by the
    underlying window system. Pixels can be accessed only through
    TQPainter functions, through bitBlt(), and by converting the
    TQPixmap to a TQImage.

    You can easily display a TQPixmap on the screen using
    TQLabel::setPixmap(). For example, all the TQButton subclasses
    support pixmap use.

    The TQPixmap class uses \link shclass.html copy-on-write\endlink,
    so it is practical to pass TQPixmap objects by value.

    You can retrieve the width(), height(), depth() and size() of a
    pixmap. The enclosing rectangle is given by rect(). Pixmaps can be
    filled with fill() and resized with resize(). You can create and
    set a mask with createHeuristicMask() and setMask(). Use
    selfMask() to see if the pixmap is identical to its mask.

    In addition to loading a pixmap from file using load() you can
    also loadFromData(). You can control optimization with
    setOptimization() and obtain a transformed version of the pixmap
    using xForm()

    Note regarding Windows 95 and 98: on Windows 9x the system crashes
    if you create more than about 1000 pixmaps, independent of the
    size of the pixmaps or installed RAM. Windows NT-systems (including
    2000, XP and following versions) do not have the same limitation,
    but depending on the graphics equipment the system will fail to
    allocate pixmap objects at some point (due to system running out of
    GDI resources).

    TQt tries to work around the resource limitation. If you set the
    pixmap optimization to \c TQPixmap::MemoryOptim and the width of
    your pixmap is less than or equal to 128 pixels, TQt stores the
    pixmap in a way that is very memory-efficient when there are many
    pixmaps.

    If your application uses dozens or hundreds of pixmaps (for
    example on tool bar buttons and in popup menus), and you plan to
    run it on Windows 95 or Windows 98, we recommend using code like
    this:

    \code
	TQPixmap::setDefaultOptimization( TQPixmap::MemoryOptim );
	while ( ... ) {
	    // load tool bar pixmaps etc.
	    TQPixmap *pixmap = new TQPixmap(fileName);
	}
	TQPixmap::setDefaultOptimization( TQPixmap::NormalOptim );
    \endcode

    In general it is recommended to make as much use of TQPixmap's
    implicit sharing and the TQPixmapCache as possible.

    \sa TQBitmap, TQImage, TQImageIO, \link shclass.html Shared Classes\endlink
*/

/*!
    \enum TQPixmap::ColorMode

    This enum type defines the color modes that exist for converting
    TQImage objects to TQPixmap.

    \value Auto  Select \c Color or \c Mono on a case-by-case basis.
    \value Color Always create colored pixmaps.
    \value Mono  Always create bitmaps.
*/

/*!
    \enum TQPixmap::Optimization

    TQPixmap has the choice of optimizing for speed or memory in a few
    places; the best choice varies from pixmap to pixmap but can
    generally be derived heuristically. This enum type defines a
    number of optimization modes that you can set for any pixmap to
    tweak the speed/memory tradeoffs:

    \value DefaultOptim  Whatever TQPixmap::defaultOptimization()
	returns. A pixmap with this optimization will have whatever
	the current default optimization is. If the default
	optimization is changed using setDefaultOptimization(), then
	this will not effect any pixmaps that have already been
	created.

    \value NoOptim  No optimization (currently the same as \c
	MemoryOptim).

    \value MemoryOptim  Optimize for minimal memory use on Windows
	9x and X11 systems.

    \value NormalOptim  Optimize for typical usage. Often uses more
	memory than \c MemoryOptim, and is often faster.

    \value BestOptim  Optimize for pixmaps that are drawn very often
	and where performance is critical. Generally uses more memory
	than \c NormalOptim and may provide a little more speed.

    We recommend using \c DefaultOptim.

*/


TQPixmap::Optimization TQPixmap::defOptim = TQPixmap::NormalOptim;


/*!
  \internal
  Private constructor which takes the bitmap flag, the optimization.and a screen.
*/

TQPixmap::TQPixmap( int w, int h, int depth, bool bitmap,
		  Optimization optimization )
    : TQPaintDevice( TQInternal::Pixmap )
{
    init( w, h, depth, bitmap, optimization );
}


/*!
    Constructs a null pixmap.

    \sa isNull()
*/

TQPixmap::TQPixmap()
    : TQPaintDevice( TQInternal::Pixmap )
{
    init( 0, 0, 0, false, defOptim );
}

/*!
    Constructs a pixmap from the TQImage \a image.

    \sa convertFromImage()
*/

TQPixmap::TQPixmap( const TQImage& image )
    : TQPaintDevice( TQInternal::Pixmap )
{
    init( 0, 0, 0, false, defOptim );
    convertFromImage( image );
}

/*!
    Constructs a pixmap with \a w width, \a h height and \a depth bits
    per pixel. The pixmap is optimized in accordance with the \a
    optimization value.

    The contents of the pixmap is uninitialized.

    The \a depth can be either 1 (monochrome) or the depth of the
    current video mode. If \a depth is negative, then the hardware
    depth of the current video mode will be used.

    If either \a w or \a h is zero, a null pixmap is constructed.

    \sa isNull() TQPixmap::Optimization
*/

TQPixmap::TQPixmap( int w, int h, int depth, Optimization optimization )
    : TQPaintDevice( TQInternal::Pixmap )
{
    init( w, h, depth, false, optimization );
}

/*!
    \overload TQPixmap::TQPixmap( const TQSize &size, int depth, Optimization optimization )

    Constructs a pixmap of size \a size, \a depth bits per pixel,
    optimized in accordance with the \a optimization value.
*/

TQPixmap::TQPixmap( const TQSize &size, int depth, Optimization optimization )
    : TQPaintDevice( TQInternal::Pixmap )
{
    init( size.width(), size.height(), depth, false, optimization );
}

#ifndef TQT_NO_IMAGEIO
/*!
    Constructs a pixmap from the file \a fileName. If the file does
    not exist or is of an unknown format, the pixmap becomes a null
    pixmap.

    The \a fileName, \a format and \a conversion_flags parameters are
    passed on to load(). This means that the data in \a fileName is
    not compiled into the binary. If \a fileName contains a relative
    path (e.g. the filename only) the relevant file must be found
    relative to the runtime working directory.

    If the image needs to be modified to fit in a lower-resolution
    result (e.g. converting from 32-bit to 8-bit), use the \a
    conversion_flags to specify how you'd prefer this to happen.

    \sa TQt::ImageConversionFlags isNull(), load(), loadFromData(), save(), imageFormat()
*/

TQPixmap::TQPixmap( const TQString& fileName, const char *format,
	int conversion_flags )
    : TQPaintDevice( TQInternal::Pixmap )
{
    init( 0, 0, 0, false, defOptim );
    load( fileName, format, conversion_flags );
}

/*!
    Constructs a pixmap from the file \a fileName. If the file does
    not exist or is of an unknown format, the pixmap becomes a null
    pixmap.

    The \a fileName, \a format and \a mode parameters are passed on to
    load(). This means that the data in \a fileName is not compiled
    into the binary. If \a fileName contains a relative path (e.g. the
    filename only) the relevant file must be found relative to the
    runtime working directory.

    \sa TQPixmap::ColorMode isNull(), load(), loadFromData(), save(), imageFormat()
*/

TQPixmap::TQPixmap( const TQString& fileName, const char *format, ColorMode mode )
    : TQPaintDevice( TQInternal::Pixmap )
{
    init( 0, 0, 0, false, defOptim );
    load( fileName, format, mode );
}

/*!
    Constructs a pixmap from \a xpm, which must be a valid XPM image.

    Errors are silently ignored.

    Note that it's possible to squeeze the XPM variable a little bit
    by using an unusual declaration:

    \code
	static const char * const start_xpm[]={
	    "16 15 8 1",
	    "a c #cec6bd",
	....
    \endcode

    The extra \c const makes the entire definition read-only, which is
    slightly more efficient (for example, when the code is in a shared
    library) and ROMable when the application is to be stored in ROM.

    In order to use that sort of declaration you must cast the
    variable back to \c{const char **} when you create the TQPixmap.
*/

TQPixmap::TQPixmap( const char *xpm[] )
    : TQPaintDevice( TQInternal::Pixmap )
{
    init( 0, 0, 0, false, defOptim );
    TQImage image( xpm );
    if ( !image.isNull() )
	convertFromImage( image );
}

/*!
    Constructs a pixmaps by loading from \a img_data. The data can be
    in any image format supported by TQt.

    \sa loadFromData()
*/

TQPixmap::TQPixmap( const TQByteArray & img_data )
    : TQPaintDevice( TQInternal::Pixmap )
{
    init( 0, 0, 0, false, defOptim );
    loadFromData( img_data );
}
#endif //TQT_NO_IMAGEIO

/*!
    Constructs a pixmap that is a copy of \a pixmap.
*/

TQPixmap::TQPixmap( const TQPixmap &pixmap )
    : TQPaintDevice( TQInternal::Pixmap )
{
    if ( pixmap.paintingActive() ) {		// make a deep copy
	data = 0;
	operator=( pixmap.copy() );
    } else {
	data = pixmap.data;
	data->ref();
	devFlags = pixmap.devFlags;		// copy TQPaintDevice flags
#if defined(TQ_WS_WIN)
	hdc = pixmap.hdc;			// copy Windows device context
#elif defined(TQ_WS_X11)
	hd = pixmap.hd;				// copy X11 drawable
	rendhd = pixmap.rendhd;
	copyX11Data( &pixmap );			// copy x11Data
#elif defined(TQ_WS_MAC)
	hd = pixmap.hd;
#endif
    }
}


/*!
    Destroys the pixmap.
*/

TQPixmap::~TQPixmap()
{
    deref();
}

/*! Convenience function. Gets the data associated with the absolute
  name \a abs_name from the default mime source factory and decodes it
  to a pixmap.

  \sa TQMimeSourceFactory, TQImage::fromMimeSource(), TQImageDrag::decode()
*/

#ifndef TQT_NO_MIME
TQPixmap TQPixmap::fromMimeSource( const TQString &abs_name )
{
    const TQMimeSource *m = TQMimeSourceFactory::defaultFactory()->data( abs_name );
    if ( !m ) {
	if ( TQFile::exists( abs_name ) )
	    return TQPixmap( abs_name );
#if defined(QT_CHECK_STATE)
	if ( !abs_name.isEmpty() )
	    tqWarning( "TQPixmap::fromMimeSource: Cannot find pixmap \"%s\" in the mime source factory",
		      abs_name.latin1() );
#endif
	return TQPixmap();
    }
    TQPixmap pix;
    TQImageDrag::decode( m, pix );
    return pix;
}
#endif

/*!
    Returns a \link shclass.html deep copy\endlink of the pixmap using
    the bitBlt() function to copy the pixels.

    \sa operator=()
*/

TQPixmap TQPixmap::copy( bool ignoreMask ) const
{
#if defined(TQ_WS_X11)
    int old = x11SetDefaultScreen( x11Screen() );
#endif // TQ_WS_X11

    TQPixmap pm( data->w, data->h, data->d, data->bitmap, data->optim );

    if ( !pm.isNull() ) {			// copy the bitmap
#if defined(TQ_WS_X11)
	pm.cloneX11Data( this );
#endif // TQ_WS_X11

	if ( ignoreMask )
	    bitBlt( &pm, 0, 0, this, 0, 0, data->w, data->h, TQt::CopyROP, true );
	else
	    copyBlt( &pm, 0, 0, this, 0, 0, data->w, data->h );
    }

#if defined(TQ_WS_X11)
    x11SetDefaultScreen( old );
#endif // TQ_WS_X11

    return pm;
}


/*!
    Assigns the pixmap \a pixmap to this pixmap and returns a
    reference to this pixmap.
*/

TQPixmap &TQPixmap::operator=( const TQPixmap &pixmap )
{
    if ( paintingActive() ) {
#if defined(QT_CHECK_STATE)
	tqWarning("TQPixmap::operator=: Cannot assign to pixmap during painting");
#endif
	return *this;
    }
    pixmap.data->ref();				// avoid 'x = x'
    deref();
    if ( pixmap.paintingActive() ) {		// make a deep copy
	init( pixmap.width(), pixmap.height(), pixmap.depth(),
	      pixmap.data->bitmap, pixmap.data->optim );
	data->uninit = false;
	if ( !isNull() )
	    copyBlt( this, 0, 0, &pixmap, 0, 0, pixmap.width(), pixmap.height() );
	pixmap.data->deref();
    } else {
	data = pixmap.data;
	devFlags = pixmap.devFlags;		// copy TQPaintDevice flags
#if defined(TQ_WS_WIN)
	hdc = pixmap.hdc;
#elif defined(TQ_WS_X11)
	hd = pixmap.hd;				// copy TQPaintDevice drawable
	rendhd = pixmap.rendhd;
	copyX11Data( &pixmap );			// copy x11Data
#elif defined(TQ_WS_MACX) || defined(Q_OS_MAC9)
	hd = pixmap.hd;
#endif
    }
    return *this;
}


/*!
    \overload

    Converts the image \a image to a pixmap that is assigned to this
    pixmap. Returns a reference to the pixmap.

    \sa convertFromImage().
*/

TQPixmap &TQPixmap::operator=( const TQImage &image )
{
    convertFromImage( image );
    return *this;
}


/*!
    \fn bool TQPixmap::isTQBitmap() const

    Returns true if this is a TQBitmap; otherwise returns false.
*/

/*!
    \fn bool TQPixmap::isNull() const

    Returns true if this is a null pixmap; otherwise returns false.

    A null pixmap has zero width, zero height and no contents. You
    cannot draw in a null pixmap or bitBlt() anything to it.

    Resizing an existing pixmap to (0, 0) makes a pixmap into a null
    pixmap.

    \sa resize()
*/

/*!
    \fn int TQPixmap::width() const

    Returns the width of the pixmap.

    \sa height(), size(), rect()
*/

/*!
    \fn int TQPixmap::height() const

    Returns the height of the pixmap.

    \sa width(), size(), rect()
*/

/*!
    \fn TQSize TQPixmap::size() const

    Returns the size of the pixmap.

    \sa width(), height(), rect()
*/

/*!
    \fn TQRect TQPixmap::rect() const

    Returns the enclosing rectangle (0,0,width(),height()) of the pixmap.

    \sa width(), height(), size()
*/

/*!
    \fn int TQPixmap::depth() const

    Returns the depth of the pixmap.

    The pixmap depth is also called bits per pixel (bpp) or bit planes
    of a pixmap. A null pixmap has depth 0.

    \sa defaultDepth(), isNull(), TQImage::convertDepth()
*/


/*!
    \overload void TQPixmap::fill( const TQWidget *widget, const TQPoint &ofs )

    Fills the pixmap with the \a widget's background color or pixmap.
    If the background is empty, nothing is done.

    The \a ofs point is an offset in the widget.

    The point \a ofs is a point in the widget's coordinate system. The
    pixmap's top-left pixel will be mapped to the point \a ofs in the
    widget. This is significant if the widget has a background pixmap;
    otherwise the pixmap will simply be filled with the background
    color of the widget.

    Example:
    \code
    void CuteWidget::paintEvent( TQPaintEvent *e )
    {
	TQRect ur = e->rect();            // rectangle to update
	TQPixmap pix( ur.size() );        // Pixmap for double-buffering
	pix.fill( this, ur.topLeft() );  // fill with widget background

	TQPainter p( &pix );
	p.translate( -ur.x(), -ur.y() ); // use widget coordinate system
					 // when drawing on pixmap
	//    ... draw on pixmap ...

	p.end();

	bitBlt( this, ur.topLeft(), &pix );
    }
    \endcode
*/

/*!
    \overload void TQPixmap::fill( const TQWidget *widget, int xofs, int yofs )

    Fills the pixmap with the \a widget's background color or pixmap.
    If the background is empty, nothing is done. \a xofs, \a yofs is
    an offset in the widget.
*/

void TQPixmap::fill( const TQWidget *widget, int xofs, int yofs )
{
    const TQPixmap* bgpm = widget->backgroundPixmap();
    fill( widget->backgroundColor() );
    if ( bgpm ) {
	if ( !bgpm->isNull() ) {
	    TQPoint ofs = widget->backgroundOffset();
	    xofs += ofs.x();
	    yofs += ofs.y();

	    TQPainter p;
	    p.begin( this );
	    p.setPen( NoPen );
	    p.drawTiledPixmap( 0, 0, width(), height(), *widget->backgroundPixmap(), xofs, yofs );
	    p.end();
	}
    }
}


/*!
    \overload void TQPixmap::resize( const TQSize &size )

    Resizes the pixmap to size \a size.
*/

/*!
    Resizes the pixmap to \a w width and \a h height. If either \a w
    or \a h is 0, the pixmap becomes a null pixmap.

    If both \a w and \a h are greater than 0, a valid pixmap is
    created. New pixels will be uninitialized (random) if the pixmap
    is expanded.
*/

void TQPixmap::resize( int w, int h )
{
    if ( w < 1 || h < 1 ) {			// becomes null
	TQPixmap pm( 0, 0, 0, data->bitmap, data->optim );
	*this = pm;
	return;
    }
    int d;
    if ( depth() > 0 )
	d = depth();
    else
	d = isTQBitmap() ? 1 : -1;
    // Create new pixmap
    TQPixmap pm( w, h, d, data->bitmap, data->optim );
#ifdef TQ_WS_X11
    pm.x11SetScreen( x11Screen() );
#endif // TQ_WS_X11
    if ( !data->uninit && !isNull() )		// has existing pixmap
	bitBlt( &pm, 0, 0, this, 0, 0,		// copy old pixmap
		TQMIN(width(), w),
		TQMIN(height(),h), CopyROP, true );
#if defined(TQ_WS_MAC)
    if(data->alphapm) {
	data->alphapm->resize(w, h);
    } else
#elif defined(TQ_WS_X11) && !defined(TQT_NO_XFTFREETYPE)
    if (data->alphapm)
	tqWarning("TQPixmap::resize: TODO: resize alpha data");
    else
#endif // TQ_WS_X11
	if ( data->mask ) {				// resize mask as well
	    if ( data->selfmask ) {			// preserve self-mask
		pm.setMask( *((TQBitmap*)&pm) );
	    } else {				// independent mask
		TQBitmap m = *data->mask;
		m.resize( w, h );
		pm.setMask( m );
	    }
	}
    *this = pm;
}


/*!
    \fn const TQBitmap *TQPixmap::mask() const

    Returns the mask bitmap, or 0 if no mask has been set.

    \sa setMask(), TQBitmap, hasAlpha()
*/

/*!
    Sets a mask bitmap.

    The \a newmask bitmap defines the clip mask for this pixmap. Every
    pixel in \a newmask corresponds to a pixel in this pixmap. Pixel
    value 1 means opaque and pixel value 0 means transparent. The mask
    must have the same size as this pixmap.

    \warning Setting the mask on a pixmap will cause any alpha channel
    data to be cleared. For example:
    \code
	TQPixmap alpha( "image-with-alpha.png" );
	TQPixmap alphacopy = alpha;
	alphacopy.setMask( *alphacopy.mask() );
    \endcode
    Now, alpha and alphacopy are visually different.

    Setting a \link isNull() null\endlink mask resets the mask.

    \sa mask(), createHeuristicMask(), TQBitmap
*/

void TQPixmap::setMask( const TQBitmap &newmask )
{
    const TQPixmap *tmp = &newmask;		// dec cxx bug
    if ( (data == tmp->data) ||
	 ( newmask.handle() && newmask.handle() == handle() ) ) {
	TQPixmap m = tmp->copy( true );
	setMask( *((TQBitmap*)&m) );
	data->selfmask = true;			// mask == pixmap
	return;
    }

    if ( newmask.isNull() ) {			// reset the mask
	if (data->mask) {
	    detach();
	    data->selfmask = false;

	    delete data->mask;
	    data->mask = 0;
	}
	return;
    }

    detach();
    data->selfmask = false;

    if ( newmask.width() != width() || newmask.height() != height() ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQPixmap::setMask: The pixmap and the mask must have "
		  "the same size" );
#endif
	return;
    }
#if defined(TQ_WS_MAC) || (defined(TQ_WS_X11) && !defined(TQT_NO_XFTFREETYPE))
    // when setting the mask, we get rid of the alpha channel completely
    delete data->alphapm;
    data->alphapm = 0;
#endif // TQ_WS_X11 && !TQT_NO_XFTFREETYPE

    delete data->mask;
    TQBitmap* newmaskcopy;
    if ( newmask.mask() )
	newmaskcopy = (TQBitmap*)new TQPixmap( tmp->copy( true ) );
    else
	newmaskcopy = new TQBitmap( newmask );
#ifdef TQ_WS_X11
    newmaskcopy->x11SetScreen( x11Screen() );
#endif
    data->mask = newmaskcopy;
}


/*!
    \fn bool TQPixmap::selfMask() const

    Returns true if the pixmap's mask is identical to the pixmap
    itself; otherwise returns false.

    \sa mask()
*/

#ifndef TQT_NO_IMAGE_HEURISTIC_MASK
/*!
    Creates and returns a heuristic mask for this pixmap. It works by
    selecting a color from one of the corners and then chipping away
    pixels of that color, starting at all the edges.

    The mask may not be perfect but it should be reasonable, so you
    can do things such as the following:
    \code
    pm->setMask( pm->createHeuristicMask() );
    \endcode

    This function is slow because it involves transformation to a
    TQImage, non-trivial computations and a transformation back to a
    TQBitmap.

    If \a clipTight is true the mask is just large enough to cover the
    pixels; otherwise, the mask is larger than the data pixels.

    \sa TQImage::createHeuristicMask()
*/

TQBitmap TQPixmap::createHeuristicMask( bool clipTight ) const
{
    TQBitmap m;
    m.convertFromImage( convertToImage().createHeuristicMask(clipTight) );
    return m;
}
#endif
#ifndef TQT_NO_IMAGEIO
/*!
    Returns a string that specifies the image format of the file \a
    fileName, or 0 if the file cannot be read or if the format cannot
    be recognized.

    The TQImageIO documentation lists the supported image formats.

    \sa load(), save()
*/

const char* TQPixmap::imageFormat( const TQString &fileName )
{
    return TQImageIO::imageFormat(fileName);
}

/*!
    Loads a pixmap from the file \a fileName at runtime. Returns true
    if successful; otherwise returns false.

    If \a format is specified, the loader attempts to read the pixmap
    using the specified format. If \a format is not specified
    (default), the loader reads a few bytes from the header to guess
    the file's format.

    See the convertFromImage() documentation for a description of the
    \a conversion_flags argument.

    The TQImageIO documentation lists the supported image formats and
    explains how to add extra formats.

    \sa loadFromData(), save(), imageFormat(), TQImage::load(),
    TQImageIO
*/

bool TQPixmap::load( const TQString &fileName, const char *format,
		    int conversion_flags )
{
    TQImageIO io( fileName, format );
    bool result = io.read();
    if ( result ) {
	detach(); // ###hanord: Why detach here, convertFromImage does it
	result = convertFromImage( io.image(), conversion_flags );
    }
    return result;
}

/*!
    \overload

    Loads a pixmap from the file \a fileName at runtime.

    If \a format is specified, the loader attempts to read the pixmap
    using the specified format. If \a format is not specified
    (default), the loader reads a few bytes from the header to guess
    the file's format.

    The \a mode is used to specify the color mode of the pixmap.

    \sa TQPixmap::ColorMode
*/

bool TQPixmap::load( const TQString &fileName, const char *format,
		    ColorMode mode )
{
    int conversion_flags = 0;
    switch (mode) {
      case Color:
	conversion_flags |= ColorOnly;
	break;
      case Mono:
	conversion_flags |= MonoOnly;
	break;
      default:
	break;// Nothing.
    }
    return load( fileName, format, conversion_flags );
}
#endif //TQT_NO_IMAGEIO

/*!
    \overload

    Converts \a image and sets this pixmap using color mode \a mode.
    Returns true if successful; otherwise returns false.

    \sa TQPixmap::ColorMode
*/

bool TQPixmap::convertFromImage( const TQImage &image, ColorMode mode )
{
    if ( image.isNull() ) {
	// convert null image to null pixmap
	*this = TQPixmap();
	return true;
    }

    int conversion_flags = 0;
    switch (mode) {
      case Color:
	conversion_flags |= ColorOnly;
	break;
      case Mono:
	conversion_flags |= MonoOnly;
	break;
      default:
	break;// Nothing.
    }
    return convertFromImage( image, conversion_flags );
}

#ifndef TQT_NO_IMAGEIO
/*!
    Loads a pixmap from the binary data in \a buf (\a len bytes).
    Returns true if successful; otherwise returns false.

    If \a format is specified, the loader attempts to read the pixmap
    using the specified format. If \a format is not specified
    (default), the loader reads a few bytes from the header to guess
    the file's format.

    See the convertFromImage() documentation for a description of the
    \a conversion_flags argument.

    The TQImageIO documentation lists the supported image formats and
    explains how to add extra formats.

    \sa load(), save(), imageFormat(), TQImage::loadFromData(),
    TQImageIO
*/

bool TQPixmap::loadFromData( const uchar *buf, uint len, const char *format,
			    int conversion_flags )
{
    TQByteArray a;
    a.setRawData( (char *)buf, len );
    TQBuffer b( a );
    b.open( IO_ReadOnly );
    TQImageIO io( &b, format );
    bool result = io.read();
    b.close();
    a.resetRawData( (char *)buf, len );
    if ( result ) {
	detach();
	result = convertFromImage( io.image(), conversion_flags );
    }
    return result;
}

/*!
    \overload

    Loads a pixmap from the binary data in \a buf (\a len bytes) using
    color mode \a mode. Returns true if successful; otherwise returns
    false.

    If \a format is specified, the loader attempts to read the pixmap
    using the specified format. If \a format is not specified
    (default), the loader reads a few bytes from the header to guess
    the file's format.

    \sa TQPixmap::ColorMode
*/

bool TQPixmap::loadFromData( const uchar *buf, uint len, const char *format,
			    ColorMode mode )
{
    int conversion_flags = 0;
    switch (mode) {
      case Color:
	conversion_flags |= ColorOnly;
	break;
      case Mono:
	conversion_flags |= MonoOnly;
	break;
      default:
	break;// Nothing.
    }
    return loadFromData( buf, len, format, conversion_flags );
}

/*!
    \overload
*/

bool TQPixmap::loadFromData( const TQByteArray &buf, const char *format,
			    int conversion_flags )
{
    return loadFromData( (const uchar *)(buf.data()), buf.size(),
			 format, conversion_flags );
}


/*!
    Saves the pixmap to the file \a fileName using the image file
    format \a format and a quality factor \a quality. \a quality must
    be in the range [0,100] or -1. Specify 0 to obtain small
    compressed files, 100 for large uncompressed files, and -1 to use
    the default settings. Returns true if successful; otherwise
    returns false.

    \sa load(), loadFromData(), imageFormat(), TQImage::save(),
    TQImageIO
*/

bool TQPixmap::save( const TQString &fileName, const char *format, int quality ) const
{
    if ( isNull() )
	return false;				// nothing to save
    TQImageIO io( fileName, format );
    return doImageIO( &io, quality );
}

/*!
    \overload

    This function writes a TQPixmap to the TQIODevice, \a device. This
    can be used, for example, to save a pixmap directly into a
    TQByteArray:
    \code
    TQPixmap pixmap;
    TQByteArray ba;
    TQBuffer buffer( ba );
    buffer.open( IO_WriteOnly );
    pixmap.save( &buffer, "PNG" ); // writes pixmap into ba in PNG format
    \endcode
*/

bool TQPixmap::save( TQIODevice* device, const char* format, int quality ) const
{
    if ( isNull() )
	return false;				// nothing to save
    TQImageIO io( device, format );
    return doImageIO( &io, quality );
}

/*! \internal
*/

bool TQPixmap::doImageIO( TQImageIO* io, int quality ) const
{
    if ( !io )
	return false;
    io->setImage( convertToImage() );
#if defined(QT_CHECK_RANGE)
    if ( quality > 100  || quality < -1 )
	tqWarning( "TQPixmap::save: quality out of range [-1,100]" );
#endif
    if ( quality >= 0 )
	io->setQuality( TQMIN(quality,100) );
    return io->write();
}

#endif //TQT_NO_IMAGEIO

/*!
    \fn int TQPixmap::serialNumber() const

    Returns a number that uniquely identifies the contents of this
    TQPixmap object. This means that multiple TQPixmap objects can have
    the same serial number as long as they refer to the same contents.

    An example of where this is useful is for caching TQPixmaps.

    \sa TQPixmapCache
*/


/*!
    Returns the default pixmap optimization setting.

    \sa setDefaultOptimization(), setOptimization(), optimization()
*/

TQPixmap::Optimization TQPixmap::defaultOptimization()
{
    return defOptim;
}

/*!
    Sets the default pixmap optimization.

    All \e new pixmaps that are created will use this default
    optimization. You may also set optimization for individual pixmaps
    using the setOptimization() function.

    The initial default \a optimization setting is \c TQPixmap::Normal.

    \sa defaultOptimization(), setOptimization(), optimization()
*/

void TQPixmap::setDefaultOptimization( Optimization optimization )
{
    if ( optimization != DefaultOptim )
	defOptim = optimization;
}


// helper for next function.
static TQPixmap grabChildWidgets( TQWidget * w )
{
    TQPixmap res( w->width(), w->height() );
    if ( res.isNull() && w->width() )
	return res;
    res.fill( w, TQPoint( 0, 0 ) );
    TQPaintDevice *oldRedirect = TQPainter::redirect( w );
    TQPainter::redirect( w, &res );
    bool dblbfr = TQSharedDoubleBuffer::isDisabled();
    TQSharedDoubleBuffer::setDisabled( true );
    TQPaintEvent e( w->rect(), false );
    TQApplication::sendEvent( w, &e );
    TQSharedDoubleBuffer::setDisabled( dblbfr );
    TQPainter::redirect( w, oldRedirect );

    const TQObjectList * children = w->children();
    if ( children ) {
	TQPainter p( &res );
	TQObjectListIt it( *children );
	TQObject * child;
	while( (child=it.current()) != 0 ) {
	    ++it;
	    if ( child->isWidgetType() &&
		 !((TQWidget *)child)->isHidden() &&
                 !((TQWidget *)child)->isTopLevel() &&
		 ((TQWidget *)child)->geometry().intersects( w->rect() ) ) {
		// those conditions aren't quite right, it's possible
		// to have a grandchild completely outside its
		// grandparent, but partially inside its parent.  no
		// point in optimizing for that.

		// make sure to evaluate pos() first - who knows what
		// the paint event(s) inside grabChildWidgets() will do.
		TQPoint childpos = ((TQWidget *)child)->pos();
		TQPixmap cpm = grabChildWidgets( (TQWidget *)child );
		if ( cpm.isNull() ) {
		    // Some child pixmap failed - abort and reset
		    res.resize( 0, 0 );
		    break;
		}
		p.drawPixmap( childpos, cpm);
	    }
	}
    }
    return res;
}


/*!
    Creates a pixmap and paints \a widget in it.

    If the \a widget has any children, then they are also painted in
    the appropriate positions.

    If you specify \a x, \a y, \a w or \a h, only the rectangle you
    specify is painted. The defaults are 0, 0 (top-left corner) and
    -1,-1 (which means the entire widget).

    (If \a w is negative, the function copies everything to the right
    border of the window. If \a h is negative, the function copies
    everything to the bottom of the window.)

    If \a widget is 0, or if the rectangle defined by \a x, \a y, the
    modified \a w and the modified \a h does not overlap the \a
    {widget}->rect(), this function will return a null TQPixmap.

    This function actually asks \a widget to paint itself (and its
    children to paint themselves). TQPixmap::grabWindow() grabs pixels
    off the screen, which is a bit faster and picks up \e exactly
    what's on-screen. This function works by calling paintEvent() with
    painter redirection turned on. If there are overlaying windows,
    grabWindow() will see them, but not this function.

    If there is overlap, it returns a pixmap of the size you want,
    containing a rendering of \a widget. If the rectangle you ask for
    is a superset of \a widget, the areas outside \a widget are
    covered with the widget's background.

    If an error occurs when trying to grab the widget, such as the
    size of the widget being too large to fit in memory, an isNull()
    pixmap is returned.

    \sa grabWindow() TQPainter::redirect() TQWidget::paintEvent()
*/

TQPixmap TQPixmap::grabWidget( TQWidget * widget, int x, int y, int w, int h )
{
    TQPixmap res;
    if ( !widget )
	return res;

    if ( w < 0 )
	w = widget->width() - x;
    if ( h < 0 )
	h = widget->height() - y;

    TQRect wr( x, y, w, h );
    if ( wr == widget->rect() )
	return grabChildWidgets( widget );
    if ( !wr.intersects( widget->rect() ) )
	return res;

    res.resize( w, h );
    if( res.isNull() )
	return res;
    res.fill( widget, TQPoint( w,h ) );
    TQPixmap tmp( grabChildWidgets( widget ) );
    if( tmp.isNull() )
	return tmp;
    ::bitBlt( &res, 0, 0, &tmp, x, y, w, h );
    return res;
}

/*!
    Returns the actual matrix used for transforming a pixmap with \a w
    width and \a h height and matrix \a matrix.

    When transforming a pixmap with xForm(), the transformation matrix
    is internally adjusted to compensate for unwanted translation,
    i.e. xForm() returns the smallest pixmap containing all
    transformed points of the original pixmap.

    This function returns the modified matrix, which maps points
    correctly from the original pixmap into the new pixmap.

    \sa xForm(), TQWMatrix
*/
#ifndef TQT_NO_PIXMAP_TRANSFORMATION
TQWMatrix TQPixmap::trueMatrix( const TQWMatrix &matrix, int w, int h )
{
    const double dt = (double)0.;
    double x1,y1, x2,y2, x3,y3, x4,y4;		// get corners
    double xx = (double)w;
    double yy = (double)h;

    TQWMatrix mat( matrix.m11(), matrix.m12(), matrix.m21(), matrix.m22(), 0., 0. );

    mat.map( dt, dt, &x1, &y1 );
    mat.map( xx, dt, &x2, &y2 );
    mat.map( xx, yy, &x3, &y3 );
    mat.map( dt, yy, &x4, &y4 );

    double ymin = y1;				// lowest y value
    if ( y2 < ymin ) ymin = y2;
    if ( y3 < ymin ) ymin = y3;
    if ( y4 < ymin ) ymin = y4;
    double xmin = x1;				// lowest x value
    if ( x2 < xmin ) xmin = x2;
    if ( x3 < xmin ) xmin = x3;
    if ( x4 < xmin ) xmin = x4;

    double ymax = y1;				// lowest y value
    if ( y2 > ymax ) ymax = y2;
    if ( y3 > ymax ) ymax = y3;
    if ( y4 > ymax ) ymax = y4;
    double xmax = x1;				// lowest x value
    if ( x2 > xmax ) xmax = x2;
    if ( x3 > xmax ) xmax = x3;
    if ( x4 > xmax ) xmax = x4;

    if ( xmax-xmin > 1.0 )
	xmin -= xmin/(xmax-xmin);
    if ( ymax-ymin > 1.0 )
	ymin -= ymin/(ymax-ymin);

    mat.setMatrix( matrix.m11(), matrix.m12(), matrix.m21(), matrix.m22(), -xmin, -ymin );
    return mat;
}
#endif // TQT_NO_WMATRIX





/*****************************************************************************
  TQPixmap stream functions
 *****************************************************************************/
#if !defined(TQT_NO_DATASTREAM) && !defined(TQT_NO_IMAGEIO)
/*!
    \relates TQPixmap

    Writes the pixmap \a pixmap to the stream \a s as a PNG image.

    Note that writing the stream to a file will not produce a valid image file.

    \sa TQPixmap::save()
    \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQPixmap &pixmap )
{
    s << pixmap.convertToImage();
    return s;
}

/*!
    \relates TQPixmap

    Reads a pixmap from the stream \a s into the pixmap \a pixmap.

    \sa TQPixmap::load()
    \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQPixmap &pixmap )
{
    TQImage img;
    s >> img;
    pixmap.convertFromImage( img );
    return s;
}

#endif //TQT_NO_DATASTREAM




/*****************************************************************************
  TQPixmap (and TQImage) helper functions
 *****************************************************************************/
/*
  This internal function contains the common (i.e. platform independent) code
  to do a transformation of pixel data. It is used by TQPixmap::xForm() and by
  TQImage::xForm().

  \a trueMat is the true transformation matrix (see TQPixmap::trueMatrix()) and
  \a xoffset is an offset to the matrix.

  \a msbfirst specifies for 1bpp images, if the MSB or LSB comes first and \a
  depth specifies the colordepth of the data.

  \a dptr is a pointer to the destination data, \a dbpl specifies the bits per
  line for the destination data, \a p_inc is the offset that we advance for
  every scanline and \a dHeight is the height of the destination image.

  \a sprt is the pointer to the source data, \a sbpl specifies the bits per
  line of the source data, \a sWidth and \a sHeight are the width and height of
  the source data.
*/
#ifndef TQT_NO_PIXMAP_TRANSFORMATION
#undef IWX_MSB
#define IWX_MSB(b)	if ( trigx < maxws && trigy < maxhs ) {			      \
			    if ( *(sptr+sbpl*(trigy>>16)+(trigx>>19)) &		      \
				 (1 << (7-((trigx>>16)&7))) )			      \
				*dptr |= b;					      \
			}							      \
			trigx += m11;						      \
			trigy += m12;
	// END OF MACRO
#undef IWX_LSB
#define IWX_LSB(b)	if ( trigx < maxws && trigy < maxhs ) {			      \
			    if ( *(sptr+sbpl*(trigy>>16)+(trigx>>19)) &		      \
				 (1 << ((trigx>>16)&7)) )			      \
				*dptr |= b;					      \
			}							      \
			trigx += m11;						      \
			trigy += m12;
	// END OF MACRO
#undef IWX_PIX
#define IWX_PIX(b)	if ( trigx < maxws && trigy < maxhs ) {			      \
			    if ( (*(sptr+sbpl*(trigy>>16)+(trigx>>19)) &	      \
				 (1 << (7-((trigx>>16)&7)))) == 0 )		      \
				*dptr &= ~b;					      \
			}							      \
			trigx += m11;						      \
			trigy += m12;
	// END OF MACRO
bool qt_xForm_helper( const TQWMatrix &trueMat, int xoffset,
	int type, int depth,
	uchar *dptr, int dbpl, int p_inc, int dHeight,
	uchar *sptr, int sbpl, int sWidth, int sHeight
	)
{
    int m11 = int(trueMat.m11()*65536.0 + 1.);
    int m12 = int(trueMat.m12()*65536.0 + 1.);
    int m21 = int(trueMat.m21()*65536.0 + 1.);
    int m22 = int(trueMat.m22()*65536.0 + 1.);
    int dx  = tqRound(trueMat.dx() *65536.0);
    int dy  = tqRound(trueMat.dy() *65536.0);

    int m21ydx = dx + (xoffset<<16);
    int m22ydy = dy;
    uint trigx;
    uint trigy;
    uint maxws = sWidth<<16;
    uint maxhs = sHeight<<16;

    for ( int y=0; y<dHeight; y++ ) {		// for each target scanline
	trigx = m21ydx;
	trigy = m22ydy;
	uchar *maxp = dptr + dbpl;
	if ( depth != 1 ) {
	    switch ( depth ) {
		case 8:				// 8 bpp transform
		while ( dptr < maxp ) {
		    if ( trigx < maxws && trigy < maxhs )
			*dptr = *(sptr+sbpl*(trigy>>16)+(trigx>>16));
		    trigx += m11;
		    trigy += m12;
		    dptr++;
		}
		break;

		case 16:			// 16 bpp transform
		while ( dptr < maxp ) {
		    if ( trigx < maxws && trigy < maxhs )
			*((ushort*)dptr) = *((ushort *)(sptr+sbpl*(trigy>>16) +
						     ((trigx>>16)<<1)));
		    trigx += m11;
		    trigy += m12;
		    dptr++;
		    dptr++;
		}
		break;

		case 24: {			// 24 bpp transform
		uchar *p2;
		while ( dptr < maxp ) {
		    if ( trigx < maxws && trigy < maxhs ) {
			p2 = sptr+sbpl*(trigy>>16) + ((trigx>>16)*3);
			dptr[0] = p2[0];
			dptr[1] = p2[1];
			dptr[2] = p2[2];
		    }
		    trigx += m11;
		    trigy += m12;
		    dptr += 3;
		}
		}
		break;

		case 32:			// 32 bpp transform
		while ( dptr < maxp ) {
		    if ( trigx < maxws && trigy < maxhs )
			*((uint*)dptr) = *((uint *)(sptr+sbpl*(trigy>>16) +
						   ((trigx>>16)<<2)));
		    trigx += m11;
		    trigy += m12;
		    dptr += 4;
		}
		break;

		default: {
		return false;
		}
	    }
	} else  {
	    switch ( type ) {
		case QT_XFORM_TYPE_MSBFIRST:
		    while ( dptr < maxp ) {
			IWX_MSB(128);
			IWX_MSB(64);
			IWX_MSB(32);
			IWX_MSB(16);
			IWX_MSB(8);
			IWX_MSB(4);
			IWX_MSB(2);
			IWX_MSB(1);
			dptr++;
		    }
		    break;
		case QT_XFORM_TYPE_LSBFIRST:
		    while ( dptr < maxp ) {
			IWX_LSB(1);
			IWX_LSB(2);
			IWX_LSB(4);
			IWX_LSB(8);
			IWX_LSB(16);
			IWX_LSB(32);
			IWX_LSB(64);
			IWX_LSB(128);
			dptr++;
		    }
		    break;
#  if defined(TQ_WS_WIN)
		case QT_XFORM_TYPE_WINDOWSPIXMAP:
		    while ( dptr < maxp ) {
			IWX_PIX(128);
			IWX_PIX(64);
			IWX_PIX(32);
			IWX_PIX(16);
			IWX_PIX(8);
			IWX_PIX(4);
			IWX_PIX(2);
			IWX_PIX(1);
			dptr++;
		    }
		    break;
#  endif
	    }
	}
	m21ydx += m21;
	m22ydy += m22;
	dptr += p_inc;
    }
    return true;
}
#undef IWX_MSB
#undef IWX_LSB
#undef IWX_PIX
#endif // TQT_NO_PIXMAP_TRANSFORMATION
