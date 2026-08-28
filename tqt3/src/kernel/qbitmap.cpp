/****************************************************************************
**
** Implementation of TQBitmap class
**
** Created : 941020
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

#include "ntqbitmap.h"
#include "ntqimage.h"


/*!
    \class TQBitmap ntqbitmap.h
    \brief The TQBitmap class provides monochrome (1-bit depth) pixmaps.

    \ingroup graphics
    \ingroup images
    \ingroup shared

    The TQBitmap class is a monochrome off-screen paint device used
    mainly for creating custom TQCursor and TQBrush objects, in
    TQPixmap::setMask() and for TQRegion.

    A TQBitmap is a TQPixmap with a \link TQPixmap::depth() depth\endlink
    of 1. If a pixmap with a depth greater than 1 is assigned to a
    bitmap, the bitmap will be dithered automatically. A TQBitmap is
    guaranteed to always have the depth 1, unless it is
    TQPixmap::isNull() which has depth 0.

    When drawing in a TQBitmap (or TQPixmap with depth 1), we recommend
    using the  TQColor objects \c TQt::color0 and \c TQt::color1.
    Painting with \c color0 sets the bitmap bits to 0, and painting
    with \c color1 sets the bits to 1. For a bitmap, 0-bits indicate
    background (or transparent) and 1-bits indicate foreground (or
    opaque). Using the \c black and \c white TQColor objects make no
    sense because the TQColor::pixel() value is not necessarily 0 for
    black and 1 for white.

    The TQBitmap can be transformed (translated, scaled, sheared or
    rotated) using xForm().

    Just like the TQPixmap class, TQBitmap is optimized by the use of
    \link shclass.html implicit sharing\endlink, so it is very
    efficient to pass TQBitmap objects as arguments.

    \sa TQPixmap, TQPainter::drawPixmap(), bitBlt(), \link shclass.html Shared Classes\endlink
*/


/*!
    Constructs a null bitmap.

    \sa TQPixmap::isNull()
*/

TQBitmap::TQBitmap()
{
    data->bitmap = true;
}


/*!
    Constructs a bitmap with width \a w and height \a h.

    The contents of the bitmap is uninitialized if \a clear is false;
    otherwise it is filled with pixel value 0 (the TQColor \c
    TQt::color0).

    The optional \a optimization argument specifies the optimization
    setting for the bitmap. The default optimization should be used in
    most cases. Games and other pixmap-intensive applications may
    benefit from setting this argument; see \l{TQPixmap::Optimization}.

    \sa TQPixmap::setOptimization(), TQPixmap::setDefaultOptimization()
*/

TQBitmap::TQBitmap( int w, int h, bool clear,
		  TQPixmap::Optimization optimization )
    : TQPixmap( w, h, 1, optimization )
{
    data->bitmap = true;
    if ( clear )
	fill( TQt::color0 );
}


/*!
    \overload

    Constructs a bitmap with the size \a size.

    The contents of the bitmap is uninitialized if \a clear is false;
    otherwise it is filled with pixel value 0 (the TQColor \c
    TQt::color0).

    The optional \a optimization argument specifies the optimization
    setting for the bitmap. The default optimization should be used in
    most cases. Games and other pixmap-intensive applications may
    benefit from setting this argument; see \l{TQPixmap::Optimization}.
*/

TQBitmap::TQBitmap( const TQSize &size, bool clear,
		  TQPixmap::Optimization optimization )
    : TQPixmap( size, 1, optimization )
{
    data->bitmap = true;
    if ( clear )
	fill( TQt::color0 );
}


/*!
    Constructs a bitmap with width \a w and height \a h and sets the
    contents to \a bits.

    The \a isXbitmap flag should be true if \a bits was generated by
    the X11 bitmap program. The X bitmap bit order is little endian.
    The TQImage documentation discusses bit order of monochrome images.

    Example (creates an arrow bitmap):
    \code
	uchar arrow_bits[] = { 0x3f, 0x1f, 0x0f, 0x1f, 0x3b, 0x71, 0xe0, 0xc0 };
	TQBitmap bm( 8, 8, arrow_bits, true );
    \endcode
*/

TQBitmap::TQBitmap( int w, int h, const uchar *bits, bool isXbitmap )
    : TQPixmap( w, h, bits, isXbitmap )
{
    data->bitmap = true;
}


/*!
    \overload

    Constructs a bitmap with the size \a size and sets the contents to
    \a bits.

    The \a isXbitmap flag should be true if \a bits was generated by
    the X11 bitmap program. The X bitmap bit order is little endian.
    The TQImage documentation discusses bit order of monochrome images.
*/

TQBitmap::TQBitmap( const TQSize &size, const uchar *bits, bool isXbitmap )
    : TQPixmap( size.width(), size.height(), bits, isXbitmap )
{
    data->bitmap = true;
}


/*!
    Constructs a bitmap that is a copy of \a bitmap.
*/

TQBitmap::TQBitmap( const TQBitmap &bitmap )
    : TQPixmap( bitmap )
{
}

#ifndef TQT_NO_IMAGEIO
/*!
    Constructs a bitmap from the file \a fileName. If the file does
    not exist or is of an unknown format, the bitmap becomes a null
    bitmap.

    The parameters \a fileName and \a format are passed on to
    TQPixmap::load(). Dithering will be performed if the file format
    uses more than 1 bit per pixel.

    \sa TQPixmap::isNull(), TQPixmap::load(), TQPixmap::loadFromData(),
    TQPixmap::save(), TQPixmap::imageFormat()
*/

TQBitmap::TQBitmap( const TQString& fileName, const char *format )
    : TQPixmap() // Will set bitmap to null bitmap, explicit call for clarity
{
    data->bitmap = true;
    load( fileName, format, Mono );
}
#endif

/*!
    Assigns the bitmap \a bitmap to this bitmap and returns a
    reference to this bitmap.
*/

TQBitmap &TQBitmap::operator=( const TQBitmap &bitmap )
{
    TQPixmap::operator=(bitmap);
#if defined(QT_CHECK_STATE)
    Q_ASSERT( data->bitmap );
#endif
    return *this;
}


/*!
    \overload

    Assigns the pixmap \a pixmap to this bitmap and returns a
    reference to this bitmap.

    Dithering will be performed if the pixmap has a TQPixmap::depth()
    greater than 1.
*/

TQBitmap &TQBitmap::operator=( const TQPixmap &pixmap )
{
    if ( pixmap.isNull() ) {			// a null pixmap
	TQBitmap bm( 0, 0, false, pixmap.optimization() );
	TQBitmap::operator=(bm);
    } else if ( pixmap.depth() == 1 ) {		// 1-bit pixmap
	if ( pixmap.isTQBitmap() ) {		// another TQBitmap
	    TQPixmap::operator=(pixmap);		// shallow assignment
	} else {				// not a TQBitmap, but 1-bit
	    TQBitmap bm( pixmap.size(), false, pixmap.optimization() );
	    bitBlt( &bm, 0,0, &pixmap, 0,0,pixmap.width(),pixmap.height() );
	    TQBitmap::operator=(bm);
	}
    } else {					// n-bit depth pixmap
	TQImage image;
	image = pixmap;				// convert pixmap to image
	*this = image;				// will dither image
    }
    return *this;
}


/*!
    \overload

    Converts the image \a image to a bitmap and assigns the result to
    this bitmap. Returns a reference to the bitmap.

    Dithering will be performed if the image has a TQImage::depth()
    greater than 1.
*/

TQBitmap &TQBitmap::operator=( const TQImage &image )
{
    convertFromImage( image );
    return *this;
}


#ifndef TQT_NO_PIXMAP_TRANSFORMATION
/*!
    Returns a transformed copy of this bitmap by using \a matrix.

    This function does exactly the same as TQPixmap::xForm(), except
    that it returns a TQBitmap instead of a TQPixmap.

    \sa TQPixmap::xForm()
*/

TQBitmap TQBitmap::xForm( const TQWMatrix &matrix ) const
{
    TQPixmap pm = TQPixmap::xForm( matrix );
    TQBitmap bm;
    // Here we fake the pixmap to think it's a TQBitmap. With this trick,
    // the TQBitmap::operator=(const TQPixmap&) will just refer the
    // pm.data and we do not need to perform a bitBlt.
    pm.data->bitmap = true;
    bm = pm;
    return bm;
}
#endif // TQT_NO_TRANSFORMATIONS



