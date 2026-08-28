/****************************************************************************
**
** Implementation of TQImage and TQImageIO classes
**
** Created : 950207
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

#include "ntqimage.h"
#include "ntqregexp.h"
#include "ntqfile.h"
#include "ntqdatastream.h"
#include "ntqtextstream.h"
#include "ntqbuffer.h"
#include "ntqptrlist.h"
#include "ntqasyncimageio.h"
#include "ntqpngio.h"
#include "ntqmngio.h"
#include "ntqjpegio.h"
#include "ntqmap.h"
#include <private/qpluginmanager_p.h>
#include "qimageformatinterface_p.h"
#include "ntqwmatrix.h"
#include "ntqapplication.h"
#include "ntqmime.h"
#include "ntqdragobject.h"
#include <ctype.h>
#include <stdlib.h>

#ifdef TQ_WS_QWS
#include "qgfx_qws.h"
#endif

// 16bpp images on supported on TQt/Embedded
#if !defined( TQ_WS_QWS ) && !defined(TQT_NO_IMAGE_16_BIT)
#define TQT_NO_IMAGE_16_BIT
#endif


/*!
    \class TQImage
    \brief The TQImage class provides a hardware-independent pixmap
    representation with direct access to the pixel data.

    \ingroup images
    \ingroup graphics
    \ingroup shared
    \mainclass

    It is one of the two classes TQt provides for dealing with images,
    the other being TQPixmap. TQImage is designed and optimized for I/O
    and for direct pixel access/manipulation. TQPixmap is designed and
    optimized for drawing. There are (slow) functions to convert
    between TQImage and TQPixmap: TQPixmap::convertToImage() and
    TQPixmap::convertFromImage().

    An image has the parameters \link width() width\endlink, \link
    height() height\endlink and \link depth() depth\endlink (bits per
    pixel, bpp), a color table and the actual \link bits()
    pixels\endlink. TQImage supports 1-bpp, 8-bpp and 32-bpp image
    data. 1-bpp and 8-bpp images use a color lookup table; the pixel
    value is a color table index.

    32-bpp images encode an RGB value in 24 bits and ignore the color
    table. The most significant byte is used for the \link
    setAlphaBuffer() alpha buffer\endlink.

    An entry in the color table is an RGB triplet encoded as a \c
    uint. Use the \link ::tqRed() tqRed()\endlink, \link ::tqGreen()
    tqGreen()\endlink and \link ::tqBlue() tqBlue()\endlink functions (\c
    ntqcolor.h) to access the components, and \link ::tqRgb()
    tqRgb\endlink to make an RGB triplet (see the TQColor class
    documentation).

    1-bpp (monochrome) images have a color table with a most two
    colors. There are two different formats: big endian (MSB first) or
    little endian (LSB first) bit order. To access a single bit you
    will must do some bit shifts:

    \code
    TQImage image;
    // sets bit at (x,y) to 1
    if ( image.bitOrder() == TQImage::LittleEndian )
	*(image.scanLine(y) + (x >> 3)) |= 1 << (x & 7);
    else
	*(image.scanLine(y) + (x >> 3)) |= 1 << (7 - (x & 7));
    \endcode

    If this looks complicated, it might be a good idea to convert the
    1-bpp image to an 8-bpp image using convertDepth().

    8-bpp images are much easier to work with than 1-bpp images
    because they have a single byte per pixel:

    \code
    TQImage image;
    // set entry 19 in the color table to yellow
    image.setColor( 19, tqRgb(255,255,0) );
    // set 8 bit pixel at (x,y) to value yellow (in color table)
    *(image.scanLine(y) + x) = 19;
    \endcode

    32-bpp images ignore the color table; instead, each pixel contains
    the RGB triplet. 24 bits contain the RGB value; the most
    significant byte is reserved for the alpha buffer.

    \code
    TQImage image;
    // sets 32 bit pixel at (x,y) to yellow.
    uint *p = (uint *)image.scanLine(y) + x;
    *p = tqRgb(255,255,0);
    \endcode

    On TQt/Embedded, scanlines are aligned to the pixel depth and may
    be padded to any degree, while on all other platforms, the
    scanlines are 32-bit aligned for all depths. The constructor
    taking a \c{uchar*} argument always expects 32-bit aligned data.
    On TQt/Embedded, an additional constructor allows the number of
    bytes-per-line to be specified.

    TQImage supports a variety of methods for getting information about
    the image, for example, colorTable(), allGray(), isGrayscale(),
    bitOrder(), bytesPerLine(), depth(), dotsPerMeterX() and
    dotsPerMeterY(), hasAlphaBuffer(), numBytes(), numColors(), and
    width() and height().

    Pixel colors are retrieved with pixel() and set with setPixel().

    TQImage also supports a number of functions for creating a new
    image that is a transformed version of the original. For example,
    copy(), convertBitOrder(), convertDepth(), createAlphaMask(),
    createHeuristicMask(), mirror(), scale(), smoothScale(), swapRGB()
    and xForm(). There are also functions for changing attributes of
    an image in-place, for example, setAlphaBuffer(), setColor(),
    setDotsPerMeterX() and setDotsPerMeterY() and setNumColors().

    Images can be loaded and saved in the supported formats. Images
    are saved to a file with save(). Images are loaded from a file
    with load() (or in the constructor) or from an array of data with
    loadFromData(). The lists of supported formats are available from
    inputFormatList() and outputFormatList().

    Strings of text may be added to images using setText().

    The TQImage class uses explicit \link shclass.html sharing\endlink,
    similar to that used by TQMemArray.

    New image formats can be added as \link plugins-howto.html
    plugins\endlink.

    \sa TQImageIO TQPixmap \link shclass.html Shared Classes\endlink
*/


/*!
    \enum TQImage::Endian

    This enum type is used to describe the endianness of the CPU and
    graphics hardware.

    \value IgnoreEndian  Endianness does not matter. Useful for some
			 operations that are independent of endianness.
    \value BigEndian  Network byte order, as on SPARC and Motorola CPUs.
    \value LittleEndian  PC/Alpha byte order.
*/

/*!
    \enum TQt::ImageConversionFlags

    The conversion flag is a bitwise-OR of the following values. The
    options marked "(default)" are set if no other values from the
    list are included (since the defaults are zero):

    Color/Mono preference (ignored for TQBitmap)
    \value AutoColor (default) - If the image has \link
	   TQImage::depth() depth\endlink 1 and contains only
	   black and white pixels, the pixmap becomes monochrome.
    \value ColorOnly The pixmap is dithered/converted to the
	   \link TQPixmap::defaultDepth() native display depth\endlink.
    \value MonoOnly The pixmap becomes monochrome. If necessary,
	   it is dithered using the chosen dithering algorithm.

    Dithering mode preference for RGB channels
    \value DiffuseDither (default) - A high-quality dither.
    \value OrderedDither A faster, more ordered dither.
    \value ThresholdDither No dithering; closest color is used.

    Dithering mode preference for alpha channel
    \value ThresholdAlphaDither (default) - No dithering.
    \value OrderedAlphaDither A faster, more ordered dither.
    \value DiffuseAlphaDither A high-quality dither.
    \value NoAlpha Not supported.

    Color matching versus dithering preference
    \value PreferDither (default when converting to a pixmap) - Always dither
	   32-bit images when the image is converted to 8 bits.
    \value AvoidDither (default when converting for the purpose of saving to
	   file) - Dither 32-bit images only if the image has more than 256
	   colors and it is being converted to 8 bits.
    \value AutoDither Not supported.

    The following are not values that are used directly, but masks for
    the above classes:
    \value ColorMode_Mask  Mask for the color mode.
    \value Dither_Mask  Mask for the dithering mode for RGB channels.
    \value AlphaDither_Mask  Mask for the dithering mode for the alpha channel.
    \value DitherMode_Mask  Mask for the mode that determines the preference of
           color matching versus dithering.

    Using 0 as the conversion flag sets all the default options.
*/

#if defined(Q_CC_DEC) && defined(__alpha) && (__DECCXX_VER-0 >= 50190001)
#pragma message disable narrowptr
#endif

#ifndef TQT_NO_IMAGE_TEXT
class TQImageDataMisc {
public:
    TQImageDataMisc() { }
    TQImageDataMisc( const TQImageDataMisc& o ) :
	text_lang(o.text_lang) { }

    TQImageDataMisc& operator=(const TQImageDataMisc& o)
    {
	text_lang = o.text_lang;
	return *this;
    }
    TQValueList<TQImageTextKeyLang> list()
    {
	return text_lang.keys();
    }

    TQStringList languages()
    {
	TQStringList r;
	TQMap<TQImageTextKeyLang,TQString>::Iterator it = text_lang.begin();
	for ( ; it != text_lang.end(); ++it ) {
	    r.remove( it.key().lang );
	    r.append( it.key().lang );
	}
	return r;
    }
    TQStringList keys()
    {
	TQStringList r;
	TQMap<TQImageTextKeyLang,TQString>::Iterator it = text_lang.begin();
	for ( ; it != text_lang.end(); ++it ) {
	    r.remove( it.key().key );
	    r.append( it.key().key );
	}
	return r;
    }

    TQMap<TQImageTextKeyLang,TQString> text_lang;
};
#endif // TQT_NO_IMAGE_TEXT



/*****************************************************************************
  TQImage member functions
 *****************************************************************************/

// table to flip bits
static const uchar bitflip[256] = {
    /*
	open OUT, "| fmt";
	for $i (0..255) {
	    print OUT (($i >> 7) & 0x01) | (($i >> 5) & 0x02) |
		      (($i >> 3) & 0x04) | (($i >> 1) & 0x08) |
		      (($i << 7) & 0x80) | (($i << 5) & 0x40) |
		      (($i << 3) & 0x20) | (($i << 1) & 0x10), ", ";
	}
	close OUT;
    */
    0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240,
    8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248,
    4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244,
    12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252,
    2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 242,
    10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250,
    6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246,
    14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254,
    1, 129, 65, 193, 33, 161, 97, 225, 17, 145, 81, 209, 49, 177, 113, 241,
    9, 137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249,
    5, 133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245,
    13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253,
    3, 131, 67, 195, 35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243,
    11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251,
    7, 135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215, 55, 183, 119, 247,
    15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255
};

const uchar *tqt_get_bitflip_array()			// called from TQPixmap code
{
    return bitflip;
}


/*!
    Constructs a null image.

    \sa isNull()
*/

TQImage::TQImage()
{
    init();
}

/*!
    Constructs an image with \a w width, \a h height, \a depth bits
    per pixel, \a numColors colors and bit order \a bitOrder.

    Using this constructor is the same as first constructing a null
    image and then calling the create() function.

    \sa create()
*/

TQImage::TQImage( int w, int h, int depth, int numColors, Endian bitOrder )
{
    init();
    create( w, h, depth, numColors, bitOrder );
}

/*!
    Constructs an image with size \a size pixels, depth \a depth bits,
    \a numColors and \a bitOrder endianness.

    Using this constructor is the same as first constructing a null
    image and then calling the create() function.

    \sa create()
*/
TQImage::TQImage( const TQSize& size, int depth, int numColors, Endian bitOrder )
{
    init();
    create( size, depth, numColors, bitOrder );
}

#ifndef TQT_NO_IMAGEIO
/*!
    Constructs an image and tries to load the image from the file \a
    fileName.

    If \a format is specified, the loader attempts to read the image
    using the specified format. If \a format is not specified (which
    is the default), the loader reads a few bytes from the header to
    guess the file format.

    If the loading of the image failed, this object is a \link
    isNull() null\endlink image.

    The TQImageIO documentation lists the supported image formats and
    explains how to add extra formats.

    \sa load() isNull() TQImageIO
*/

TQImage::TQImage( const TQString &fileName, const char* format )
{
    init();
    load( fileName, format );
}

#ifndef TQT_NO_IMAGEIO_XPM
// helper
static void read_xpm_image_or_array( TQImageIO *, const char * const *, TQImage & );
#endif
/*!
    Constructs an image from \a xpm, which must be a valid XPM image.

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
    slightly more efficient (e.g. when the code is in a shared
    library) and ROMable when the application is to be stored in ROM.
*/

TQImage::TQImage( const char * const xpm[] )
{
    init();
#ifndef TQT_NO_IMAGEIO_XPM
    read_xpm_image_or_array( 0, xpm, *this );
#else
    // We use a tqFatal rather than disabling the whole function, as this
    // constructor may be ambiguous.
    tqFatal("XPM not supported");
#endif
}

/*!
    Constructs an image from the binary data \a array. It tries to
    guess the file format.

    If the loading of the image failed, this object is a \link
    isNull() null\endlink image.

    \sa loadFromData() isNull() imageFormat()
*/
TQImage::TQImage( const TQByteArray &array )
{
    init();
    loadFromData(array);
}
#endif //TQT_NO_IMAGEIO


/*!
    Constructs a \link shclass.html shallow copy\endlink of \a image.
*/

TQImage::TQImage( const TQImage &image )
{
    data = image.data;
    data->ref();
}

/*!
    Constructs an image \a w pixels wide, \a h pixels high with a
    color depth of \a depth, that uses an existing memory buffer, \a
    yourdata. The buffer must remain valid throughout the life of the
    TQImage. The image does not delete the buffer at destruction.

    If \a colortable is 0, a color table sufficient for \a numColors
    will be allocated (and destructed later).

    Note that \a yourdata must be 32-bit aligned.

    The endianness is given in \a bitOrder.
*/
TQImage::TQImage( uchar* yourdata, int w, int h, int depth,
		TQRgb* colortable, int numColors,
		Endian bitOrder )
{
    init();
    int bpl = ((w*depth+31)/32)*4;	// bytes per scanline
    if ( w <= 0 || h <= 0 || depth <= 0 || numColors < 0
         || INT_MAX / sizeof(uchar *) < uint(h)
         || INT_MAX / uint(depth) < uint(w)
         || bpl <= 0
         || INT_MAX / uint(bpl) < uint(h) )
	return;					// invalid parameter(s)
    data->w = w;
    data->h = h;
    data->d = depth;
    data->ncols = depth != 32 ? numColors : 0;
    if ( !yourdata )
	return;	    // Image header info can be saved without needing to allocate memory.
    data->nbytes = bpl*h;
    if ( colortable || !data->ncols ) {
	data->ctbl = colortable;
	data->ctbl_mine = false;
    } else {
	// calloc since we realloc, etc. later (ick)
	data->ctbl = (TQRgb*)calloc( data->ncols*sizeof(TQRgb), data->ncols );
        TQ_CHECK_PTR(data->ctbl);
	data->ctbl_mine = true;
    }
    uchar** jt = (uchar**)malloc(h*sizeof(uchar*));
    TQ_CHECK_PTR(jt);
    for (int j=0; j<h; j++) {
	jt[j] = yourdata+j*bpl;
    }
    data->bits = jt;
    data->bitordr = bitOrder;
}

#ifdef TQ_WS_QWS

/*!
    Constructs an image that uses an existing memory buffer. The
    buffer must remain valid for the life of the TQImage. The image
    does not delete the buffer at destruction. The buffer is passed as
    \a yourdata. The image's width is \a w and its height is \a h. The
    color depth is \a depth. \a bpl specifies the number of bytes per
    line.

    If \a colortable is 0, a color table sufficient for \a numColors
    will be allocated (and destructed later).

    The endianness is specified by \a bitOrder.

    \warning This constructor is only available on TQt/Embedded.
*/
TQImage::TQImage( uchar* yourdata, int w, int h, int depth,
		int bpl, TQRgb* colortable, int numColors,
		Endian bitOrder )
{
    init();
    if ( !yourdata || w <= 0 || h <= 0 || depth <= 0 || numColors < 0
         || INT_MAX / sizeof(uchar *) < uint(h)
         || INT_MAX / uint(bpl) < uint(h)
         )
	return;					// invalid parameter(s)
    data->w = w;
    data->h = h;
    data->d = depth;
    data->ncols = numColors;
    data->nbytes = bpl * h;
    if ( colortable || !numColors ) {
	data->ctbl = colortable;
	data->ctbl_mine = false;
    } else {
	// calloc since we realloc, etc. later (ick)
	data->ctbl = (TQRgb*)calloc( numColors*sizeof(TQRgb), numColors );
        TQ_CHECK_PTR(data->ctbl);
	data->ctbl_mine = true;
    }
    uchar** jt = (uchar**)malloc(h*sizeof(uchar*));
    TQ_CHECK_PTR(jt);
    for (int j=0; j<h; j++) {
	jt[j] = yourdata+j*bpl;
    }
    data->bits = jt;
    data->bitordr = bitOrder;
}
#endif // TQ_WS_QWS

/*!
    Destroys the image and cleans up.
*/

TQImage::~TQImage()
{
    if ( data && data->deref() ) {
	reset();
	delete data;
    }
}




/*! Convenience function. Gets the data associated with the absolute
  name \a abs_name from the default mime source factory and decodes it
  to an image.

  \sa TQMimeSourceFactory, TQImage::fromMimeSource(), TQImageDrag::decode()
*/
#ifndef TQT_NO_MIME
TQImage TQImage::fromMimeSource( const TQString &abs_name )
{
    const TQMimeSource *m = TQMimeSourceFactory::defaultFactory()->data( abs_name );
    if ( !m ) {
#if defined(QT_CHECK_STATE)
	tqWarning("TQImage::fromMimeSource: Cannot find image \"%s\" in the mime source factory", abs_name.latin1() );
#endif
	return TQImage();
    }
    TQImage img;
    TQImageDrag::decode( m, img );
    return img;
}
#endif


/*!
    Assigns a \link shclass.html shallow copy\endlink of \a image to
    this image and returns a reference to this image.

    \sa copy()
*/

TQImage &TQImage::operator=( const TQImage &image )
{
    image.data->ref();				// avoid 'x = x'
    if ( data->deref() ) {
	reset();
	delete data;
    }
    data = image.data;
    return *this;
}

/*!
    \overload

    Sets the image bits to the \a pixmap contents and returns a
    reference to the image.

    If the image shares data with other images, it will first
    dereference the shared data.

    Makes a call to TQPixmap::convertToImage().
*/

TQImage &TQImage::operator=( const TQPixmap &pixmap )
{
    *this = pixmap.convertToImage();
    return *this;
}

/*!
    Detaches from shared image data and makes sure that this image is
    the only one referring to the data.

    If multiple images share common data, this image makes a copy of
    the data and detaches itself from the sharing mechanism.
    Nothing is done if there is just a single reference.

    \sa copy()
*/

void TQImage::detach()
{
    if ( data->count != 1 )
	*this = copy();
}

/*!
    Returns a \link shclass.html deep copy\endlink of the image.

    \sa detach()
*/

TQImage TQImage::copy() const
{
    if ( isNull() ) {
	// maintain the fields of invalid TQImages when copied
	return TQImage( 0, width(), height(), depth(), colorTable(), numColors(), bitOrder() );
    } else {
	TQImage image;
	image.create( width(), height(), depth(), numColors(), bitOrder() );
#ifdef TQ_WS_QWS
	// TQt/Embedded can create images with non-default bpl
	// make sure we don't crash.
	if ( image.numBytes() != numBytes() )
	    for ( int i = 0; i < height(); i++ )
		memcpy( image.scanLine(i), scanLine(i), image.bytesPerLine() );
	else
#endif
	    memcpy( image.bits(), bits(), numBytes() );
	memcpy( image.colorTable(), colorTable(), numColors() * sizeof(TQRgb) );
	image.setAlphaBuffer( hasAlphaBuffer() );
	image.data->dpmx = dotsPerMeterX();
	image.data->dpmy = dotsPerMeterY();
	image.data->offset = offset();
#ifndef TQT_NO_IMAGE_TEXT
	if ( data->misc ) {
	    image.data->misc = new TQImageDataMisc;
	    *image.data->misc = misc();
	}
#endif
	return image;
    }
}

/*!
    \overload

    Returns a \link shclass.html deep copy\endlink of a sub-area of
    the image.

    The returned image is always \a w by \a h pixels in size, and is
    copied from position \a x, \a y in this image. In areas beyond
    this image pixels are filled with pixel 0.

    If the image needs to be modified to fit in a lower-resolution
    result (e.g. converting from 32-bit to 8-bit), use the \a
    conversion_flags to specify how you'd prefer this to happen.

    \sa bitBlt() TQt::ImageConversionFlags
*/

TQImage TQImage::copy(int x, int y, int w, int h, int conversion_flags) const
{
    int dx = 0;
    int dy = 0;
    if ( w <= 0 || h <= 0 ) return TQImage(); // Nothing to copy

    TQImage image( w, h, depth(), numColors(), bitOrder() );

    if ( x < 0 || y < 0 || x + w > width() || y + h > height() ) {
	// bitBlt will not cover entire image - clear it.
	// ### should deal with each side separately for efficiency
	image.fill(0);
	if ( x < 0 ) {
	    dx = -x;
	    x = 0;
	}
	if ( y < 0 ) {
	    dy = -y;
	    y = 0;
	}
    }

    bool has_alpha = hasAlphaBuffer();
    if ( has_alpha ) {
	// alpha channel should be only copied, not used by bitBlt(), and
	// this is mutable, we will restore the image state before returning
	TQImage *that = (TQImage *) this;
	that->setAlphaBuffer( false );
    }
    memcpy( image.colorTable(), colorTable(), numColors()*sizeof(TQRgb) );
    bitBlt( &image, dx, dy, this, x, y, -1, -1, conversion_flags );
    if ( has_alpha ) {
	// restore image state
	TQImage *that = (TQImage *) this;
	that->setAlphaBuffer( true );
    }
    image.setAlphaBuffer(hasAlphaBuffer());
    image.data->dpmx = dotsPerMeterX();
    image.data->dpmy = dotsPerMeterY();
    image.data->offset = offset();
#ifndef TQT_NO_IMAGE_TEXT
    if ( data->misc ) {
        image.data->misc = new TQImageDataMisc;
        *image.data->misc = misc();
    }
#endif
    return image;
}

/*!
    \overload TQImage TQImage::copy(const TQRect& r) const

    Returns a \link shclass.html deep copy\endlink of a sub-area of
    the image.

    The returned image always has the size of the rectangle \a r. In
    areas beyond this image pixels are filled with pixel 0.
*/

/*!
    \fn bool TQImage::isNull() const

    Returns true if it is a null image; otherwise returns false.

    A null image has all parameters set to zero and no allocated data.
*/


/*!
    \fn int TQImage::width() const

    Returns the width of the image.

    \sa height() size() rect()
*/

/*!
    \fn int TQImage::height() const

    Returns the height of the image.

    \sa width() size() rect()
*/

/*!
    \fn TQSize TQImage::size() const

    Returns the size of the image, i.e. its width and height.

    \sa width() height() rect()
*/

/*!
    \fn TQRect TQImage::rect() const

    Returns the enclosing rectangle (0, 0, width(), height()) of the
    image.

    \sa width() height() size()
*/

/*!
    \fn int TQImage::depth() const

    Returns the depth of the image.

    The image depth is the number of bits used to encode a single
    pixel, also called bits per pixel (bpp) or bit planes of an image.

    The supported depths are 1, 8, 16 (TQt/Embedded only) and 32.

    \sa convertDepth()
*/

/*!
    \fn int TQImage::numColors() const

    Returns the size of the color table for the image.

    Notice that numColors() returns 0 for 16-bpp (TQt/Embedded only)
    and 32-bpp images because these images do not use color tables,
    but instead encode pixel values as RGB triplets.

    \sa setNumColors() colorTable()
*/

/*!
    \fn TQImage::Endian TQImage::bitOrder() const

    Returns the bit order for the image.

    If it is a 1-bpp image, this function returns either
    TQImage::BigEndian or TQImage::LittleEndian.

    If it is not a 1-bpp image, this function returns
    TQImage::IgnoreEndian.

    \sa depth()
*/

/*!
    \fn uchar **TQImage::jumpTable() const

    Returns a pointer to the scanline pointer table.

    This is the beginning of the data block for the image.

    \sa bits() scanLine()
*/

/*!
    \fn TQRgb *TQImage::colorTable() const

    Returns a pointer to the color table.

    \sa numColors()
*/

/*!
    \fn int TQImage::numBytes() const

    Returns the number of bytes occupied by the image data.

    \sa bytesPerLine() bits()
*/

/*!
    \fn int TQImage::bytesPerLine() const

    Returns the number of bytes per image scanline. This is equivalent
    to numBytes()/height().

    \sa numBytes() scanLine()
*/

/*!
    \fn TQRgb TQImage::color( int i ) const

    Returns the color in the color table at index \a i. The first
    color is at index 0.

    A color value is an RGB triplet. Use the \link ::tqRed()
    tqRed()\endlink, \link ::tqGreen() tqGreen()\endlink and \link
    ::tqBlue() tqBlue()\endlink functions (defined in \c ntqcolor.h) to
    get the color value components.

    \sa setColor() numColors() TQColor
*/

/*!
    \fn void TQImage::setColor( int i, TQRgb c )

    Sets a color in the color table at index \a i to \a c.

    A color value is an RGB triplet. Use the \link ::tqRgb()
    tqRgb()\endlink function (defined in \c ntqcolor.h) to make RGB
    triplets.

    \sa color() setNumColors() numColors()
*/

/*!
    \fn uchar *TQImage::scanLine( int i ) const

    Returns a pointer to the pixel data at the scanline with index \a
    i. The first scanline is at index 0.

    The scanline data is aligned on a 32-bit boundary.

    \warning If you are accessing 32-bpp image data, cast the returned
    pointer to \c{TQRgb*} (TQRgb has a 32-bit size) and use it to
    read/write the pixel value. You cannot use the \c{uchar*} pointer
    directly, because the pixel format depends on the byte order on
    the underlying platform. Hint: use \link ::tqRed() tqRed()\endlink,
    \link ::tqGreen() tqGreen()\endlink and \link ::tqBlue()
    tqBlue()\endlink, etc. (ntqcolor.h) to access the pixels.

    \warning If you are accessing 16-bpp image data, you must handle
    endianness yourself. (TQt/Embedded only)

    \sa bytesPerLine() bits() jumpTable()
*/

/*!
    \fn uchar *TQImage::bits() const

    Returns a pointer to the first pixel data. This is equivalent to
    scanLine(0).

    \sa numBytes() scanLine() jumpTable()
*/



void TQImage::warningIndexRange( const char *func, int i )
{
#if defined(QT_CHECK_RANGE)
    tqWarning( "TQImage::%s: Index %d out of range", func, i );
#else
    Q_UNUSED( func )
    Q_UNUSED( i )
#endif
}


/*!
    Resets all image parameters and deallocates the image data.
*/

void TQImage::reset()
{
    freeBits();
    setNumColors( 0 );
#ifndef TQT_NO_IMAGE_TEXT
    delete data->misc;
#endif
    reinit();
}


/*!
    Fills the entire image with the pixel value \a pixel.

    If the \link depth() depth\endlink of this image is 1, only the
    lowest bit is used. If you say fill(0), fill(2), etc., the image
    is filled with 0s. If you say fill(1), fill(3), etc., the image is
    filled with 1s. If the depth is 8, the lowest 8 bits are used.

    If the depth is 32 and the image has no alpha buffer, the \a pixel
    value is written to each pixel in the image. If the image has an
    alpha buffer, only the 24 RGB bits are set and the upper 8 bits
    (alpha value) are left unchanged.

    Note: TQImage::pixel() returns the color of the pixel at the given
    coordinates; TQColor::pixel() returns the pixel value of the
    underlying window system (essentially an index value), so normally
    you will want to use TQImage::pixel() to use a color from an
    existing image or TQColor::rgb() to use a specific color.

    \sa invertPixels() depth() hasAlphaBuffer() create()
*/

void TQImage::fill( uint pixel )
{
    if ( depth() == 1 || depth() == 8 ) {
	if ( depth() == 1 ) {
	    if ( pixel & 1 )
		pixel = 0xffffffff;
	    else
		pixel = 0;
	} else {
	    uint c = pixel & 0xff;
	    pixel = c | ((c << 8) & 0xff00) | ((c << 16) & 0xff0000) |
		    ((c << 24) & 0xff000000);
	}
	int bpl = bytesPerLine();
	for ( int i=0; i<height(); i++ )
	    memset( scanLine(i), pixel, bpl );
#ifndef TQT_NO_IMAGE_16_BIT
    } else if ( depth() == 16 ) {
	for ( int i=0; i<height(); i++ ) {
	    //optimize with 32-bit writes, since image is always aligned
	    uint *p = (uint *)scanLine(i);
	    uint *end = (uint*)(((ushort*)p) + width());
	    uint fill;
	    ushort *f = (ushort*)&fill;
	    f[0]=pixel;
	    f[1]=pixel;
	    while ( p < end )
		*p++ = fill;
	}
#endif	// TQT_NO_IMAGE_16_BIT
#ifndef TQT_NO_IMAGE_TRUECOLOR
    } else if ( depth() == 32 ) {
	if ( hasAlphaBuffer() ) {
	    pixel &= 0x00ffffff;
	    for ( int i=0; i<height(); i++ ) {
		uint *p = (uint *)scanLine(i);
		uint *end = p + width();
		while ( p < end ) {
		    *p = (*p & 0xff000000) | pixel;
		    p++;
		}
	    }
	} else {
	    for ( int i=0; i<height(); i++ ) {
		uint *p = (uint *)scanLine(i);
		uint *end = p + width();
		while ( p < end )
		    *p++ = pixel;
	    }
	}
#endif // TQT_NO_IMAGE_TRUECOLOR
    }
}


/*!
    Inverts all pixel values in the image.

    If the depth is 32: if \a invertAlpha is true, the alpha bits are
    also inverted, otherwise they are left unchanged.

    If the depth is not 32, the argument \a invertAlpha has no
    meaning.

    Note that inverting an 8-bit image means to replace all pixels
    using color index \e i with a pixel using color index 255 minus \e
    i. Similarly for a 1-bit image. The color table is not changed.

    \sa fill() depth() hasAlphaBuffer()
*/

void TQImage::invertPixels( bool invertAlpha )
{
    TQ_UINT32 n = numBytes();
    if ( n % 4 ) {
	TQ_UINT8 *p = (TQ_UINT8*)bits();
	TQ_UINT8 *end = p + n;
	while ( p < end )
	    *p++ ^= 0xff;
    } else {
	TQ_UINT32 *p = (TQ_UINT32*)bits();
	TQ_UINT32 *end = p + n/4;
	uint xorbits = invertAlpha && depth() == 32 ? 0x00ffffff : 0xffffffff;
	while ( p < end )
	    *p++ ^= xorbits;
    }
}


/*!
    Determines the host computer byte order. Returns
    TQImage::LittleEndian (LSB first) or TQImage::BigEndian (MSB first).

    \sa systemBitOrder()
*/

TQImage::Endian TQImage::systemByteOrder()
{
    static Endian sbo = IgnoreEndian;
    if ( sbo == IgnoreEndian ) {		// initialize
	int  ws;
	bool be;
	tqSysInfo( &ws, &be );
	sbo = be ? BigEndian : LittleEndian;
    }
    return sbo;
}


#if defined(TQ_WS_X11)
#include <X11/Xlib.h>				// needed for systemBitOrder
#include <X11/Xutil.h>
#include <X11/Xos.h>
#if defined(Q_OS_WIN32)
#undef open
#undef close
#undef read
#undef write
#endif
#endif

// POSIX Large File Support redefines open -> open64
#if defined(open)
# undef open
#endif

// POSIX Large File Support redefines truncate -> truncate64
#if defined(truncate)
# undef truncate
#endif

/*!
    Determines the bit order of the display hardware. Returns
    TQImage::LittleEndian (LSB first) or TQImage::BigEndian (MSB first).

    \sa systemByteOrder()
*/

TQImage::Endian TQImage::systemBitOrder()
{
#if defined(TQ_WS_X11)
    return BitmapBitOrder(tqt_xdisplay()) == MSBFirst ? BigEndian :LittleEndian;
#else
    return BigEndian;
#endif
}


/*!
    Resizes the color table to \a numColors colors.

    If the color table is expanded all the extra colors will be set to
    black (RGB 0,0,0).

    \sa numColors() color() setColor() colorTable()
*/

void TQImage::setNumColors( int numColors )
{
    if ( numColors == data->ncols )
	return;
    if ( numColors == 0 ) {			// use no color table
	if ( data->ctbl ) {
	    if ( data->ctbl_mine )
		free( data->ctbl );
	    else
		data->ctbl_mine = true;
	    data->ctbl = 0;
	}
	data->ncols = 0;
	return;
    }
    if ( data->ctbl && data->ctbl_mine ) {	// already has color table
	data->ctbl = (TQRgb*)realloc( data->ctbl, numColors*sizeof(TQRgb) );
	if ( data->ctbl && numColors > data->ncols )
	    memset( (char *)&data->ctbl[data->ncols], 0,
		    (numColors-data->ncols)*sizeof(TQRgb) );
    } else {					// create new color table
	data->ctbl = (TQRgb*)calloc( numColors*sizeof(TQRgb), 1 );
        TQ_CHECK_PTR(data->ctbl);
	data->ctbl_mine = true;
    }
    data->ncols = data->ctbl == 0 ? 0 : numColors;
}


/*!
    \fn bool TQImage::hasAlphaBuffer() const

    Returns true if alpha buffer mode is enabled; otherwise returns
    false.

    \sa setAlphaBuffer()
*/

/*!
    Enables alpha buffer mode if \a enable is true, otherwise disables
    it. The default setting is disabled.

    An 8-bpp image has 8-bit pixels. A pixel is an index into the
    \link color() color table\endlink, which contains 32-bit color
    values. In a 32-bpp image, the 32-bit pixels are the color values.

    This 32-bit value is encoded as follows: The lower 24 bits are
    used for the red, green, and blue components. The upper 8 bits
    contain the alpha component.

    The alpha component specifies the transparency of a pixel. 0 means
    completely transparent and 255 means opaque. The alpha component
    is ignored if you do not enable alpha buffer mode.

    The alpha buffer is used to set a mask when a TQImage is translated
    to a TQPixmap.

    \sa hasAlphaBuffer() createAlphaMask()
*/

void TQImage::setAlphaBuffer( bool enable )
{
    data->alpha = enable;
}


/*!
    Sets the image \a width, \a height, \a depth, its number of colors
    (in \a numColors), and bit order. Returns true if successful, or
    false if the parameters are incorrect or if memory cannot be
    allocated.

    The \a width and \a height is limited to 32767. \a depth must be
    1, 8, or 32. If \a depth is 1, \a bitOrder must be set to
    either TQImage::LittleEndian or TQImage::BigEndian. For other depths
    \a bitOrder must be TQImage::IgnoreEndian.

    This function allocates a color table and a buffer for the image
    data. The image data is not initialized.

    The image buffer is allocated as a single block that consists of a
    table of \link scanLine() scanline\endlink pointers (jumpTable())
    and the image data (bits()).

    \sa fill() width() height() depth() numColors() bitOrder()
    jumpTable() scanLine() bits() bytesPerLine() numBytes()
*/

bool TQImage::create( int width, int height, int depth, int numColors,
		    Endian bitOrder )
{
    reset();					// reset old data
    if ( width <= 0 || height <= 0 || depth <= 0 || numColors < 0 )
	return false;				// invalid parameter(s)
    if ( depth == 1 && bitOrder == IgnoreEndian ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQImage::create: Bit order is required for 1 bpp images" );
#endif
	return false;
    }
    if ( depth != 1 )
	bitOrder = IgnoreEndian;

#if defined(QT_CHECK_RANGE)
    if ( depth == 24 )
	tqWarning( "TQImage::create: 24-bpp images no longer supported, "
		  "use 32-bpp instead" );
#endif
    switch ( depth ) {
    case 1:
    case 8:
#ifndef TQT_NO_IMAGE_16_BIT
    case 16:
#endif
#ifndef TQT_NO_IMAGE_TRUECOLOR
    case 32:
#endif
	break;
    default:				// invalid depth
	return false;
    }

    if ( depth == 32 )
	numColors = 0;
    setNumColors( numColors );
    if ( data->ncols != numColors )		// could not alloc color table
	return false;

    if ( INT_MAX / uint(depth) < uint(width) ) { // sanity check for potential overflow
	setNumColors( 0 );
	return false;
    }
// TQt/Embedded doesn't waste memory on unnecessary padding.
#ifdef TQ_WS_QWS
    const int bpl = (width*depth+7)/8;		// bytes per scanline
    const int pad = 0;
#else
    const int bpl = ((width*depth+31)/32)*4;	// bytes per scanline
    // #### WWA: shouldn't this be (width*depth+7)/8:
    const int pad = bpl - (width*depth)/8;	// pad with zeros
#endif
    if ( INT_MAX / uint(bpl) < uint(height)
        || bpl < 0
        || INT_MAX / sizeof(uchar *) < uint(height) ) { // sanity check for potential overflow
	setNumColors( 0 );
	return false;
    }
    int nbytes = bpl*height;			// image size
    int ptbl   = height*sizeof(uchar*);		// pointer table size
    int size   = nbytes + ptbl;			// total size of data block
    uchar **p  = (uchar **)malloc( size );	// alloc image bits
    TQ_CHECK_PTR(p);
    if ( !p ) {					// no memory
	setNumColors( 0 );
	return false;
    }
    data->w = width;
    data->h = height;
    data->d = depth;
    data->nbytes  = nbytes;
    data->bitordr = bitOrder;
    data->bits = p;				// set image pointer
    //uchar *d = (uchar*)p + ptbl;		// setup scanline pointers
    uchar *d = (uchar*)(p + height);		// setup scanline pointers
    while ( height-- ) {
	*p++ = d;
	if ( pad )
	    memset( d+bpl-pad, 0, pad );
	d += bpl;
    }
    return true;
}

/*!
    \overload bool TQImage::create( const TQSize&, int depth, int numColors, Endian bitOrder )
*/
bool TQImage::create( const TQSize& size, int depth, int numColors,
		     TQImage::Endian bitOrder )
{
    return create(size.width(), size.height(), depth, numColors, bitOrder);
}

/*!
  \internal
  Initializes the image data structure.
*/

void TQImage::init()
{
    data = new TQImageData;
    TQ_CHECK_PTR( data );
    reinit();
}

void TQImage::reinit()
{
    data->w = data->h = data->d = data->ncols = 0;
    data->nbytes = 0;
    data->ctbl = 0;
    data->bits = 0;
    data->bitordr = TQImage::IgnoreEndian;
    data->alpha = false;
#ifndef TQT_NO_IMAGE_TEXT
    data->misc = 0;
#endif
    data->dpmx = 0;
    data->dpmy = 0;
    data->offset = TQPoint(0,0);
}

/*!
  \internal
  Deallocates the image data and sets the bits pointer to 0.
*/

void TQImage::freeBits()
{
    if ( data->bits ) {				// dealloc image bits
	free( data->bits );
	data->bits = 0;
    }
}


/*****************************************************************************
  Internal routines for converting image depth.
 *****************************************************************************/

//
// convert_32_to_8:  Converts a 32 bits depth (true color) to an 8 bit
// image with a colormap. If the 32 bit image has more than 256 colors,
// we convert the red,green and blue bytes into a single byte encoded
// as 6 shades of each of red, green and blue.
//
// if dithering is needed, only 1 color at most is available for alpha.
//
#ifndef TQT_NO_IMAGE_TRUECOLOR
struct TQRgbMap {
    TQRgbMap() : rgb(0xffffffff) { }
    bool used() const { return rgb!=0xffffffff; }
    uchar  pix;
    TQRgb  rgb;
};

static bool convert_32_to_8( const TQImage *src, TQImage *dst, int conversion_flags, TQRgb* palette=0, int palette_count=0 )
{
    TQRgb *p;
    uchar  *b;
    bool    do_quant = false;
    int	    y, x;

    if ( !dst->create(src->width(), src->height(), 8, 256) )
	return false;

    const int tablesize = 997; // prime
    TQRgbMap table[tablesize];
    int   pix=0;
    TQRgb amask = src->hasAlphaBuffer() ? 0xffffffff : 0x00ffffff;
    if ( src->hasAlphaBuffer() )
	dst->setAlphaBuffer(true);

    if ( palette ) {
	// Preload palette into table.

	p = palette;
	// Almost same code as pixel insertion below
	while ( palette_count-- > 0 ) {
	    // Find in table...
	    int hash = (*p & amask) % tablesize;
	    for (;;) {
		if ( table[hash].used() ) {
		    if ( table[hash].rgb == (*p & amask) ) {
			// Found previous insertion - use it
			break;
		    } else {
			// Keep searching...
			if (++hash == tablesize) hash = 0;
		    }
		} else {
		    // Cannot be in table
		    Q_ASSERT ( pix != 256 );	// too many colors
		    // Insert into table at this unused position
		    dst->setColor( pix, (*p & amask) );
		    table[hash].pix = pix++;
		    table[hash].rgb = *p & amask;
		    break;
		}
	    }
	    p++;
	}
    }

    if ( (conversion_flags & TQt::DitherMode_Mask) == TQt::PreferDither ) {
	do_quant = true;
    } else {
	for ( y=0; y<src->height(); y++ ) {	// check if <= 256 colors
	    p = (TQRgb *)src->scanLine(y);
	    b = dst->scanLine(y);
	    x = src->width();
	    while ( x-- ) {
		// Find in table...
		int hash = (*p & amask) % tablesize;
		for (;;) {
		    if ( table[hash].used() ) {
			if ( table[hash].rgb == (*p & amask) ) {
			    // Found previous insertion - use it
			    break;
			} else {
			    // Keep searching...
			    if (++hash == tablesize) hash = 0;
			}
		    } else {
			// Cannot be in table
			if ( pix == 256 ) {	// too many colors
			    do_quant = true;
			    // Break right out
			    x = 0;
			    y = src->height();
			} else {
			    // Insert into table at this unused position
			    dst->setColor( pix, (*p & amask) );
			    table[hash].pix = pix++;
			    table[hash].rgb = (*p & amask);
			}
			break;
		    }
		}
		*b++ = table[hash].pix;		// May occur once incorrectly
		p++;
	    }
	}
    }
    int ncols = do_quant ? 256 : pix;

    static uint bm[16][16];
    static int init=0;
    if (!init) {
	// Build a Bayer Matrix for dithering

	init = 1;
	int n, i, j;

	bm[0][0]=0;

	for (n=1; n<16; n*=2) {
	    for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
		    bm[i][j]*=4;
		    bm[i+n][j]=bm[i][j]+2;
		    bm[i][j+n]=bm[i][j]+3;
		    bm[i+n][j+n]=bm[i][j]+1;
		}
	    }
	}

	for (i=0; i<16; i++)
	    for (j=0; j<16; j++)
		bm[i][j]<<=8;
    }

    dst->setNumColors( ncols );

    if ( do_quant ) {				// quantization needed

#define MAX_R 5
#define MAX_G 5
#define MAX_B 5
#define INDEXOF(r,g,b) (((r)*(MAX_G+1)+(g))*(MAX_B+1)+(b))

	int rc, gc, bc;

	for ( rc=0; rc<=MAX_R; rc++ )		// build 6x6x6 color cube
	    for ( gc=0; gc<=MAX_G; gc++ )
		for ( bc=0; bc<=MAX_B; bc++ ) {
		    dst->setColor( INDEXOF(rc,gc,bc),
			(amask&0xff000000)
			| tqRgb( rc*255/MAX_R, gc*255/MAX_G, bc*255/MAX_B ) );
		}

	int sw = src->width();

	int* line1[3];
	int* line2[3];
	int* pv[3];
	if ( ( conversion_flags & TQt::Dither_Mask ) == TQt::DiffuseDither ) {
	    line1[0] = new int[sw];
	    line2[0] = new int[sw];
	    line1[1] = new int[sw];
	    line2[1] = new int[sw];
	    line1[2] = new int[sw];
	    line2[2] = new int[sw];
	    pv[0] = new int[sw];
	    pv[1] = new int[sw];
	    pv[2] = new int[sw];
	}

	for ( y=0; y < src->height(); y++ ) {
	    p = (TQRgb *)src->scanLine(y);
	    b = dst->scanLine(y);
	    TQRgb *end = p + sw;

	    // perform quantization
	    if ( ( conversion_flags & TQt::Dither_Mask ) == TQt::ThresholdDither ) {
#define DITHER(p,m) ((uchar) ((p * (m) + 127) / 255))
		while ( p < end ) {
		    rc = tqRed( *p );
		    gc = tqGreen( *p );
		    bc = tqBlue( *p );

		    *b++ =
			INDEXOF(
			    DITHER(rc, MAX_R),
			    DITHER(gc, MAX_G),
			    DITHER(bc, MAX_B)
			);

		    p++;
		}
#undef DITHER
	    } else if ( ( conversion_flags & TQt::Dither_Mask ) == TQt::OrderedDither ) {
#define DITHER(p,d,m) ((uchar) ((((256 * (m) + (m) + 1)) * (p) + (d)) / 65536 ))

		int x = 0;
		while ( p < end ) {
		    uint d = bm[y&15][x&15];

		    rc = tqRed( *p );
		    gc = tqGreen( *p );
		    bc = tqBlue( *p );

		    *b++ =
			INDEXOF(
			    DITHER(rc, d, MAX_R),
			    DITHER(gc, d, MAX_G),
			    DITHER(bc, d, MAX_B)
			);

		    p++;
		    x++;
		}
#undef DITHER
	    } else { // Diffuse
		int endian = (TQImage::systemByteOrder() == TQImage::BigEndian);
		int x;
		uchar* q = src->scanLine(y);
		uchar* q2 = src->scanLine(y+1 < src->height() ? y + 1 : 0);
		for (int chan = 0; chan < 3; chan++) {
		    b = dst->scanLine(y);
		    int *l1 = (y&1) ? line2[chan] : line1[chan];
		    int *l2 = (y&1) ? line1[chan] : line2[chan];
		    if ( y == 0 ) {
			for (int i=0; i<sw; i++)
			    l1[i] = q[i*4+chan+endian];
		    }
		    if ( y+1 < src->height() ) {
			for (int i=0; i<sw; i++)
			    l2[i] = q2[i*4+chan+endian];
		    }
		    // Bi-directional error diffusion
		    if ( y&1 ) {
			for (x=0; x<sw; x++) {
			    int pix = TQMAX(TQMIN(5, (l1[x] * 5 + 128)/ 255), 0);
			    int err = l1[x] - pix * 255 / 5;
			    pv[chan][x] = pix;

			    // Spread the error around...
			    if ( x+1<sw ) {
				l1[x+1] += (err*7)>>4;
				l2[x+1] += err>>4;
			    }
			    l2[x]+=(err*5)>>4;
			    if (x>1)
				l2[x-1]+=(err*3)>>4;
			}
		    } else {
			for (x=sw; x-->0; ) {
			    int pix = TQMAX(TQMIN(5, (l1[x] * 5 + 128)/ 255), 0);
			    int err = l1[x] - pix * 255 / 5;
			    pv[chan][x] = pix;

			    // Spread the error around...
			    if ( x > 0 ) {
				l1[x-1] += (err*7)>>4;
				l2[x-1] += err>>4;
			    }
			    l2[x]+=(err*5)>>4;
			    if (x+1 < sw)
				l2[x+1]+=(err*3)>>4;
			}
		    }
		}
		if (endian) {
		    for (x=0; x<sw; x++) {
			*b++ = INDEXOF(pv[0][x],pv[1][x],pv[2][x]);
		    }
		} else {
		    for (x=0; x<sw; x++) {
			*b++ = INDEXOF(pv[2][x],pv[1][x],pv[0][x]);
		    }
		}
	    }
	}

#ifndef TQT_NO_IMAGE_DITHER_TO_1
	if ( src->hasAlphaBuffer() ) {
	    const int trans = 216;
	    dst->setColor(trans, 0x00000000);	// transparent
	    TQImage mask = src->createAlphaMask(conversion_flags);
	    uchar* m;
	    for ( y=0; y < src->height(); y++ ) {
		uchar bit = 0x80;
		m = mask.scanLine(y);
		b = dst->scanLine(y);
		int w = src->width();
		for ( x = 0; x<w; x++ ) {
		    if ( !(*m&bit) )
			b[x] = trans;
		    if (!(bit >>= 1)) {
			bit = 0x80;
			while ( x<w-1 && *++m == 0xff ) // skip chunks
			    x+=8;
		    }
		}
	    }
	}
#endif

	if ( ( conversion_flags & TQt::Dither_Mask ) == TQt::DiffuseDither ) {
	    delete [] line1[0];
	    delete [] line2[0];
	    delete [] line1[1];
	    delete [] line2[1];
	    delete [] line1[2];
	    delete [] line2[2];
	    delete [] pv[0];
	    delete [] pv[1];
	    delete [] pv[2];
	}

#undef MAX_R
#undef MAX_G
#undef MAX_B
#undef INDEXOF

    }

    return true;
}


static bool convert_8_to_32( const TQImage *src, TQImage *dst )
{
    if ( !dst->create(src->width(), src->height(), 32) )
	return false;				// create failed
    dst->setAlphaBuffer( src->hasAlphaBuffer() );
    for ( int y=0; y<dst->height(); y++ ) {	// for each scan line...
	uint *p = (uint *)dst->scanLine(y);
	uchar  *b = src->scanLine(y);
	uint *end = p + dst->width();
	while ( p < end )
	    *p++ = src->color(*b++);
    }
    return true;
}


static bool convert_1_to_32( const TQImage *src, TQImage *dst )
{
    if ( !dst->create(src->width(), src->height(), 32) )
	return false;				// could not create
    dst->setAlphaBuffer( src->hasAlphaBuffer() );
    for ( int y=0; y<dst->height(); y++ ) {	// for each scan line...
	uint *p = (uint *)dst->scanLine(y);
	uchar *b = src->scanLine(y);
	int x;
	if ( src->bitOrder() == TQImage::BigEndian ) {
	    for ( x=0; x<dst->width(); x++ ) {
		*p++ = src->color( (*b >> (7 - (x & 7))) & 1 );
		if ( (x & 7) == 7 )
		    b++;
	    }
	} else {
	    for ( x=0; x<dst->width(); x++ ) {
		*p++ = src->color( (*b >> (x & 7)) & 1 );
		if ( (x & 7) == 7 )
		    b++;
	    }
	}
    }
    return true;
}
#endif // TQT_NO_IMAGE_TRUECOLOR

static bool convert_1_to_8( const TQImage *src, TQImage *dst )
{
    if ( !dst->create(src->width(), src->height(), 8, 2) )
	return false;				// something failed
    dst->setAlphaBuffer( src->hasAlphaBuffer() );
    if (src->numColors() >= 2) {
	dst->setColor( 0, src->color(0) );	// copy color table
	dst->setColor( 1, src->color(1) );
    } else {
	// Unlikely, but they do exist
	if (src->numColors() >= 1)
	    dst->setColor( 0, src->color(0) );
	else
	    dst->setColor( 0, 0xffffffff );
	dst->setColor( 1, 0xff000000 );
    }
    for ( int y=0; y<dst->height(); y++ ) {	// for each scan line...
	uchar *p = dst->scanLine(y);
	uchar *b = src->scanLine(y);
	int x;
	if ( src->bitOrder() == TQImage::BigEndian ) {
	    for ( x=0; x<dst->width(); x++ ) {
		*p++ = (*b >> (7 - (x & 7))) & 1;
		if ( (x & 7) == 7 )
		    b++;
	    }
	} else {
	    for ( x=0; x<dst->width(); x++ ) {
		*p++ = (*b >> (x & 7)) & 1;
		if ( (x & 7) == 7 )
		    b++;
	    }
	}
    }
    return true;
}

#ifndef TQT_NO_IMAGE_DITHER_TO_1
//
// dither_to_1:  Uses selected dithering algorithm.
//

static bool dither_to_1( const TQImage *src, TQImage *dst,
			 int conversion_flags, bool fromalpha )
{
    if ( !dst->create(src->width(), src->height(), 1, 2, TQImage::BigEndian) )
	return false;				// something failed

    enum { Threshold, Ordered, Diffuse } dithermode;

    if ( fromalpha ) {
	if ( ( conversion_flags & TQt::AlphaDither_Mask ) == TQt::DiffuseAlphaDither )
	    dithermode = Diffuse;
	else if ( ( conversion_flags & TQt::AlphaDither_Mask ) == TQt::OrderedAlphaDither )
	    dithermode = Ordered;
	else
	    dithermode = Threshold;
    } else {
	if ( ( conversion_flags & TQt::Dither_Mask ) == TQt::ThresholdDither )
	    dithermode = Threshold;
	else if ( ( conversion_flags & TQt::Dither_Mask ) == TQt::OrderedDither )
	    dithermode = Ordered;
	else
	    dithermode = Diffuse;
    }

    dst->setColor( 0, tqRgb(255, 255, 255) );
    dst->setColor( 1, tqRgb(  0,	  0,   0) );
    int	  w = src->width();
    int	  h = src->height();
    int	  d = src->depth();
    uchar gray[256];				// gray map for 8 bit images
    bool  use_gray = d == 8;
    if ( use_gray ) {				// make gray map
	if ( fromalpha ) {
	    // Alpha 0x00 -> 0 pixels (white)
	    // Alpha 0xFF -> 1 pixels (black)
	    for ( int i=0; i<src->numColors(); i++ )
		gray[i] = (255 - (src->color(i) >> 24));
	} else {
	    // Pixel 0x00 -> 1 pixels (black)
	    // Pixel 0xFF -> 0 pixels (white)
	    for ( int i=0; i<src->numColors(); i++ )
		gray[i] = tqGray( src->color(i) & 0x00ffffff );
	}
    }

    switch ( dithermode ) {
      case Diffuse: {
	int *line1 = new int[w];
	int *line2 = new int[w];
	int bmwidth = (w+7)/8;
	if ( !(line1 && line2) )
	    return false;
	uchar *p;
	uchar *end;
	int *b1, *b2;
	int wbytes = w * (d/8);
	p = src->bits();
	end = p + wbytes;
	b2 = line2;
	if ( use_gray ) {			// 8 bit image
	    while ( p < end )
		*b2++ = gray[*p++];
#ifndef TQT_NO_IMAGE_TRUECOLOR
	} else {				// 32 bit image
	    if ( fromalpha ) {
		while ( p < end ) {
		    *b2++ = 255 - (*(uint*)p >> 24);
		    p += 4;
		}
	    } else {
		while ( p < end ) {
		    *b2++ = tqGray(*(uint*)p);
		    p += 4;
		}
	    }
#endif
	}
	int x, y;
	for ( y=0; y<h; y++ ) {			// for each scan line...
	    int *tmp = line1; line1 = line2; line2 = tmp;
	    bool not_last_line = y < h - 1;
	    if ( not_last_line ) {		// calc. grayvals for next line
		p = src->scanLine(y+1);
		end = p + wbytes;
		b2 = line2;
		if ( use_gray ) {		// 8 bit image
		    while ( p < end )
			*b2++ = gray[*p++];
#ifndef TQT_NO_IMAGE_TRUECOLOR
		} else {			// 24 bit image
		    if ( fromalpha ) {
			while ( p < end ) {
			    *b2++ = 255 - (*(uint*)p >> 24);
			    p += 4;
			}
		    } else {
			while ( p < end ) {
			    *b2++ = tqGray(*(uint*)p);
			    p += 4;
			}
		    }
#endif
		}
	    }

	    int err;
	    p = dst->scanLine( y );
	    memset( p, 0, bmwidth );
	    b1 = line1;
	    b2 = line2;
	    int bit = 7;
	    for ( x=1; x<=w; x++ ) {
		if ( *b1 < 128 ) {		// black pixel
		    err = *b1++;
		    *p |= 1 << bit;
		} else {			// white pixel
		    err = *b1++ - 255;
		}
		if ( bit == 0 ) {
		    p++;
		    bit = 7;
		} else {
		    bit--;
		}
		if ( x < w )
		    *b1 += (err*7)>>4;		// spread error to right pixel
		if ( not_last_line ) {
		    b2[0] += (err*5)>>4;	// pixel below
		    if ( x > 1 )
			b2[-1] += (err*3)>>4;	// pixel below left
		    if ( x < w )
			b2[1] += err>>4;	// pixel below right
		}
		b2++;
	    }
	}
	delete [] line1;
	delete [] line2;
      } break;
      case Ordered: {
	static uint bm[16][16];
	static int init=0;
	if (!init) {
	    // Build a Bayer Matrix for dithering

	    init = 1;
	    int n, i, j;

	    bm[0][0]=0;

	    for (n=1; n<16; n*=2) {
		for (i=0; i<n; i++) {
		    for (j=0; j<n; j++) {
			bm[i][j]*=4;
			bm[i+n][j]=bm[i][j]+2;
			bm[i][j+n]=bm[i][j]+3;
			bm[i+n][j+n]=bm[i][j]+1;
		    }
		}
	    }

	    // Force black to black
	    bm[0][0]=1;
	}

	dst->fill( 0 );
	uchar** mline = dst->jumpTable();
#ifndef TQT_NO_IMAGE_TRUECOLOR
	if ( d == 32 ) {
	    uint** line = (uint**)src->jumpTable();
	    for ( int i=0; i<h; i++ ) {
		uint  *p = line[i];
		uint  *end = p + w;
		uchar *m = mline[i];
		int bit = 7;
		int j = 0;
		if ( fromalpha ) {
		    while ( p < end ) {
			if ( (*p++ >> 24) >= bm[j++&15][i&15] )
			    *m |= 1 << bit;
			if ( bit == 0 ) {
			    m++;
			    bit = 7;
			} else {
			    bit--;
			}
		    }
		} else {
		    while ( p < end ) {
			if ( (uint)tqGray(*p++) < bm[j++&15][i&15] )
			    *m |= 1 << bit;
			if ( bit == 0 ) {
			    m++;
			    bit = 7;
			} else {
			    bit--;
			}
		    }
		}
	    }
	} else
#endif // TQT_NO_IMAGE_TRUECOLOR
	    /* ( d == 8 ) */ {
	    uchar** line = src->jumpTable();
	    for ( int i=0; i<h; i++ ) {
		uchar *p = line[i];
		uchar *end = p + w;
		uchar *m = mline[i];
		int bit = 7;
		int j = 0;
		while ( p < end ) {
		    if ( (uint)gray[*p++] < bm[j++&15][i&15] )
			*m |= 1 << bit;
		    if ( bit == 0 ) {
			m++;
			bit = 7;
		    } else {
			bit--;
		    }
		}
	    }
	}
      } break;
      default: { // Threshold:
	dst->fill( 0 );
	uchar** mline = dst->jumpTable();
#ifndef TQT_NO_IMAGE_TRUECOLOR
	if ( d == 32 ) {
	    uint** line = (uint**)src->jumpTable();
	    for ( int i=0; i<h; i++ ) {
		uint  *p = line[i];
		uint  *end = p + w;
		uchar *m = mline[i];
		int bit = 7;
		if ( fromalpha ) {
		    while ( p < end ) {
			if ( (*p++ >> 24) >= 128 )
			    *m |= 1 << bit;	// Set mask "on"
			if ( bit == 0 ) {
			    m++;
			    bit = 7;
			} else {
			    bit--;
			}
		    }
		} else {
		    while ( p < end ) {
			if ( tqGray(*p++) < 128 )
			    *m |= 1 << bit;	// Set pixel "black"
			if ( bit == 0 ) {
			    m++;
			    bit = 7;
			} else {
			    bit--;
			}
		    }
		}
	    }
	} else
#endif //TQT_NO_IMAGE_TRUECOLOR
	    if ( d == 8 ) {
	    uchar** line = src->jumpTable();
	    for ( int i=0; i<h; i++ ) {
		uchar *p = line[i];
		uchar *end = p + w;
		uchar *m = mline[i];
		int bit = 7;
		while ( p < end ) {
		    if ( gray[*p++] < 128 )
			*m |= 1 << bit;		// Set mask "on"/ pixel "black"
		    if ( bit == 0 ) {
			m++;
			bit = 7;
		    } else {
			bit--;
		    }
		}
	    }
	}
      }
    }
    return true;
}
#endif

#ifndef TQT_NO_IMAGE_16_BIT
//###### Endianness issues!
static inline bool is16BitGray( ushort c )
{
    int r=(c & 0xf800) >> 11;
    int g=(c & 0x07e0) >> 6; //green/2
    int b=(c & 0x001f);
    return r == g && g == b;
}


static bool convert_16_to_32( const TQImage *src, TQImage *dst )
{
    if ( !dst->create(src->width(), src->height(), 32) )
	return false;				// create failed
    dst->setAlphaBuffer( src->hasAlphaBuffer() );
    for ( int y=0; y<dst->height(); y++ ) {	// for each scan line...
	uint *p = (uint *)dst->scanLine(y);
	ushort  *s = (ushort*)src->scanLine(y);
	uint *end = p + dst->width();
	while ( p < end )
	    *p++ = tqt_conv16ToRgb( *s++ );
    }
    return true;
}


static bool convert_32_to_16( const TQImage *src, TQImage *dst )
{
    if ( !dst->create(src->width(), src->height(), 16) )
	return false;				// create failed
    dst->setAlphaBuffer( src->hasAlphaBuffer() );
    for ( int y=0; y<dst->height(); y++ ) {	// for each scan line...
	ushort *p = (ushort *)dst->scanLine(y);
	uint  *s = (uint*)src->scanLine(y);
	ushort *end = p + dst->width();
	while ( p < end )
	    *p++ = tqt_convRgbTo16( *s++ );
    }
    return true;
}


#endif

/*!
    Converts the depth (bpp) of the image to \a depth and returns the
    converted image. The original image is not changed.

    The \a depth argument must be 1, 8, 16 (TQt/Embedded only) or 32.

    Returns \c *this if \a depth is equal to the image depth, or a
    \link isNull() null\endlink image if this image cannot be
    converted.

    If the image needs to be modified to fit in a lower-resolution
    result (e.g. converting from 32-bit to 8-bit), use the \a
    conversion_flags to specify how you'd prefer this to happen.

    \sa TQt::ImageConversionFlags depth() isNull()
*/

TQImage TQImage::convertDepth( int depth, int conversion_flags ) const
{
    TQImage image;
    if ( data->d == depth )
	image = *this;				// no conversion
#ifndef TQT_NO_IMAGE_DITHER_TO_1
    else if ( (data->d == 8 || data->d == 32) && depth == 1 ) // dither
	dither_to_1( this, &image, conversion_flags, false );
#endif
#ifndef TQT_NO_IMAGE_TRUECOLOR
    else if ( data->d == 32 && depth == 8 )	// 32 -> 8
	convert_32_to_8( this, &image, conversion_flags );
    else if ( data->d == 8 && depth == 32 )	// 8 -> 32
	convert_8_to_32( this, &image );
#endif
    else if ( data->d == 1 && depth == 8 )	// 1 -> 8
	convert_1_to_8( this, &image );
#ifndef TQT_NO_IMAGE_TRUECOLOR
    else if ( data->d == 1 && depth == 32 )	// 1 -> 32
	convert_1_to_32( this, &image );
#endif
#ifndef TQT_NO_IMAGE_16_BIT
    else if ( data->d == 16 && depth != 16 ) {
	TQImage tmp;
	convert_16_to_32( this, &tmp );
	image = tmp.convertDepth( depth, conversion_flags );
    } else if ( data->d != 16 && depth == 16 ) {
	TQImage tmp = convertDepth( 32, conversion_flags );
	convert_32_to_16( &tmp, &image );
    }
#endif
    else {
#if defined(QT_CHECK_RANGE)
	if ( isNull() )
	    tqWarning( "TQImage::convertDepth: Image is a null image" );
	else
	    tqWarning( "TQImage::convertDepth: Depth %d not supported", depth );
#endif
    }
    return image;
}

/*!
    \overload
*/

TQImage TQImage::convertDepth( int depth ) const
{
    return convertDepth( depth, 0 );
}

/*!
    Returns true if ( \a x, \a y ) is a valid coordinate in the image;
    otherwise returns false.

    \sa width() height() pixelIndex()
*/

bool TQImage::valid( int x, int y ) const
{
    return x >= 0 && x < width()
	&& y >= 0 && y < height();
}

/*!
    Returns the pixel index at the given coordinates.

    If (\a x, \a y) is not \link valid() valid\endlink, or if the
    image is not a paletted image (depth() \> 8), the results are
    undefined.

    \sa valid() depth()
*/

int TQImage::pixelIndex( int x, int y ) const
{
#if defined(QT_CHECK_RANGE)
    if ( x < 0 || x >= width() ) {
	tqWarning( "TQImage::pixel: x=%d out of range", x );
	return -12345;
    }
#endif
    uchar * s = scanLine( y );
    switch( depth() ) {
    case 1:
	if ( bitOrder() == TQImage::LittleEndian )
	    return (*(s + (x >> 3)) >> (x & 7)) & 1;
	else
	    return (*(s + (x >> 3)) >> (7- (x & 7))) & 1;
    case 8:
	return (int)s[x];
#ifndef TQT_NO_IMAGE_TRUECOLOR
#ifndef TQT_NO_IMAGE_16_BIT
    case 16:
#endif
    case 32:
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQImage::pixelIndex: Not applicable for %d-bpp images "
		 "(no palette)", depth() );
#endif
	return 0;
#endif //TQT_NO_IMAGE_TRUECOLOR
    }
    return 0;
}


/*!
    Returns the color of the pixel at the coordinates (\a x, \a y).

    If (\a x, \a y) is not \link valid() on the image\endlink, the
    results are undefined.

    \sa setPixel() tqRed() tqGreen() tqBlue() valid()
*/

TQRgb TQImage::pixel( int x, int y ) const
{
#if defined(QT_CHECK_RANGE)
    if ( x < 0 || x >= width() ) {
	tqWarning( "TQImage::pixel: x=%d out of range", x );
	return 12345;
    }
#endif
    uchar * s = scanLine( y );
    switch( depth() ) {
    case 1:
	if ( bitOrder() == TQImage::LittleEndian )
	    return color( (*(s + (x >> 3)) >> (x & 7)) & 1 );
	else
	    return color( (*(s + (x >> 3)) >> (7- (x & 7))) & 1 );
    case 8:
	return color( (int)s[x] );
#ifndef TQT_NO_IMAGE_16_BIT
    case 16:
	return tqt_conv16ToRgb(((ushort*)s)[x]);
#endif
#ifndef TQT_NO_IMAGE_TRUECOLOR
    case 32:
	return ((TQRgb*)s)[x];
#endif
    default:
	return 100367;
    }
}


/*!
    Sets the pixel index or color at the coordinates (\a x, \a y) to
    \a index_or_rgb.

    If (\a x, \a y) is not \link valid() valid\endlink, the result is
    undefined.

    If the image is a paletted image (depth() \<= 8) and \a
    index_or_rgb \>= numColors(), the result is undefined.

    \sa pixelIndex() pixel() tqRgb() tqRgba() valid()
*/

void TQImage::setPixel( int x, int y, uint index_or_rgb )
{
    if ( x < 0 || x >= width() ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQImage::setPixel: x=%d out of range", x );
#endif
	return;
    }
    if ( depth() == 1 ) {
	uchar * s = scanLine( y );
	if ( index_or_rgb > 1) {
#if defined(QT_CHECK_RANGE)
	    tqWarning( "TQImage::setPixel: index=%d out of range",
		     index_or_rgb );
#endif
	} else if ( bitOrder() == TQImage::LittleEndian ) {
	    if (index_or_rgb==0)
		*(s + (x >> 3)) &= ~(1 << (x & 7));
	    else
		*(s + (x >> 3)) |= (1 << (x & 7));
	} else {
	    if (index_or_rgb==0)
		*(s + (x >> 3)) &= ~(1 << (7-(x & 7)));
	    else
		*(s + (x >> 3)) |= (1 << (7-(x & 7)));
	}
    } else if ( depth() == 8 ) {
	if (index_or_rgb > (uint)numColors()) {
#if defined(QT_CHECK_RANGE)
	    tqWarning( "TQImage::setPixel: index=%d out of range",
		     index_or_rgb );
#endif
	    return;
	}
	uchar * s = scanLine( y );
	s[x] = index_or_rgb;
#ifndef TQT_NO_IMAGE_16_BIT
    } else if ( depth() == 16 ) {
	ushort * s = (ushort*)scanLine( y );
	s[x] = tqt_convRgbTo16(index_or_rgb);
#endif
#ifndef TQT_NO_IMAGE_TRUECOLOR
    } else if ( depth() == 32 ) {
	TQRgb * s = (TQRgb*)scanLine( y );
	s[x] = index_or_rgb;
#endif
    }
}


/*!
    Converts the bit order of the image to \a bitOrder and returns the
    converted image. The original image is not changed.

    Returns \c *this if the \a bitOrder is equal to the image bit
    order, or a \link isNull() null\endlink image if this image cannot
    be converted.

    \sa bitOrder() systemBitOrder() isNull()
*/

TQImage TQImage::convertBitOrder( Endian bitOrder ) const
{
    if ( isNull() || data->d != 1 ||		// invalid argument(s)
	 !(bitOrder == BigEndian || bitOrder == LittleEndian) ) {
	TQImage nullImage;
	return nullImage;
    }
    if ( data->bitordr == bitOrder )		// nothing to do
	return copy();

    TQImage image( data->w, data->h, 1, data->ncols, bitOrder );

    int bpl = (width() + 7) / 8;
    for ( int y = 0; y < data->h; y++ ) {
	uchar *p = jumpTable()[y];
	uchar *end = p + bpl;
	uchar *b = image.jumpTable()[y];
	while ( p < end )
	    *b++ = bitflip[*p++];
    }
    memcpy( image.colorTable(), colorTable(), numColors()*sizeof(TQRgb) );
    return image;
}

// ### Candidate (renamed) for ntqcolor.h
static
bool isGray(TQRgb c)
{
    return tqRed(c) == tqGreen(c)
	&& tqRed(c) == tqBlue(c);
}

/*!
    Returns true if all the colors in the image are shades of gray
    (i.e. their red, green and blue components are equal); otherwise
    returns false.

    This function is slow for large 16-bit (TQt/Embedded only) and 32-bit images.

    \sa isGrayscale()
*/
bool TQImage::allGray() const
{
#ifndef TQT_NO_IMAGE_TRUECOLOR
    if (depth()==32) {
	int p = width()*height();
	TQRgb* b = (TQRgb*)bits();
	while (p--)
	    if (!isGray(*b++))
		return false;
#ifndef TQT_NO_IMAGE_16_BIT
    } else if (depth()==16) {
	int p = width()*height();
	ushort* b = (ushort*)bits();
	while (p--)
	    if (!is16BitGray(*b++))
		return false;
#endif
    } else
#endif //TQT_NO_IMAGE_TRUECOLOR
	{
	if (!data->ctbl) return true;
	for (int i=0; i<numColors(); i++)
	    if (!isGray(data->ctbl[i]))
		return false;
    }
    return true;
}

/*!
    For 16-bit (TQt/Embedded only) and 32-bit images, this function is
    equivalent to allGray().

    For 8-bpp images, this function returns true if color(i) is
    TQRgb(i,i,i) for all indices of the color table; otherwise returns
    false.

    \sa allGray() depth()
*/
bool TQImage::isGrayscale() const
{
    switch (depth()) {
#ifndef TQT_NO_IMAGE_TRUECOLOR
    case 32:
#ifndef TQT_NO_IMAGE_16_BIT
    case 16:
#endif
	return allGray();
#endif //TQT_NO_IMAGE_TRUECOLOR
    case 8: {
	for (int i=0; i<numColors(); i++)
	    if (data->ctbl[i] != tqRgb(i,i,i))
		return false;
	return true;
	}
    }
    return false;
}

#ifndef TQT_NO_IMAGE_SMOOTHSCALE
static
void pnmscale(const TQImage& src, TQImage& dst)
{
    TQRgb* xelrow = 0;
    TQRgb* tempxelrow = 0;
    TQRgb* xP;
    TQRgb* nxP;
    int rows, cols, rowsread, newrows, newcols;
    int row, col, needtoreadrow;
    const uchar maxval = 255;
    double xscale, yscale;
    long sxscale, syscale;
    long fracrowtofill, fracrowleft;
    long* as;
    long* rs;
    long* gs;
    long* bs;
    int rowswritten = 0;

    cols = src.width();
    rows = src.height();
    newcols = dst.width();
    newrows = dst.height();

    long SCALE;
    long HALFSCALE;

    if (cols > 4096)
    {
        SCALE = 4096;
        HALFSCALE = 2048;
    }
    else
    {
        int fac = 4096;

        while (cols * fac > 4096)
        {
            fac /= 2;
        }

        SCALE = fac * cols;
        HALFSCALE = fac * cols / 2;
    }

    xscale = (double) newcols / (double) cols;
    yscale = (double) newrows / (double) rows;

    sxscale = (long)(xscale * SCALE);
    syscale = (long)(yscale * SCALE);

    if ( newrows != rows )	/* shortcut Y scaling if possible */
	tempxelrow = new TQRgb[cols];

    if ( src.hasAlphaBuffer() ) {
	dst.setAlphaBuffer(true);
	as = new long[cols];
	for ( col = 0; col < cols; ++col )
	    as[col] = HALFSCALE;
    } else {
	as = 0;
    }
    rs = new long[cols];
    gs = new long[cols];
    bs = new long[cols];
    rowsread = 0;
    fracrowleft = syscale;
    needtoreadrow = 1;
    for ( col = 0; col < cols; ++col )
	rs[col] = gs[col] = bs[col] = HALFSCALE;
    fracrowtofill = SCALE;

    for ( row = 0; row < newrows; ++row ) {
	/* First scale Y from xelrow into tempxelrow. */
	if ( newrows == rows ) {
	    /* shortcut Y scaling if possible */
	    tempxelrow = xelrow = (TQRgb*)src.scanLine(rowsread++);
	} else {
	    while ( fracrowleft < fracrowtofill ) {
		if ( needtoreadrow && rowsread < rows )
		    xelrow = (TQRgb*)src.scanLine(rowsread++);
		for ( col = 0, xP = xelrow; col < cols; ++col, ++xP ) {
		    if (as) {
			as[col] += fracrowleft * tqAlpha( *xP );
			rs[col] += fracrowleft * tqRed( *xP ) * tqAlpha( *xP ) / 255;
			gs[col] += fracrowleft * tqGreen( *xP ) * tqAlpha( *xP ) / 255;
			bs[col] += fracrowleft * tqBlue( *xP ) * tqAlpha( *xP ) / 255;
		    } else {
			rs[col] += fracrowleft * tqRed( *xP );
			gs[col] += fracrowleft * tqGreen( *xP );
			bs[col] += fracrowleft * tqBlue( *xP );
		    }
		}
		fracrowtofill -= fracrowleft;
		fracrowleft = syscale;
		needtoreadrow = 1;
	    }
	    /* Now fracrowleft is >= fracrowtofill, so we can produce a row. */
	    if ( needtoreadrow && rowsread < rows ) {
		xelrow = (TQRgb*)src.scanLine(rowsread++);
		needtoreadrow = 0;
	    }
	    long a=0;
	    for ( col = 0, xP = xelrow, nxP = tempxelrow;
		  col < cols; ++col, ++xP, ++nxP )
	    {
		long r, g, b;

		if ( as ) {
		    r = rs[col] + fracrowtofill * tqRed( *xP ) * tqAlpha( *xP ) / 255;
		    g = gs[col] + fracrowtofill * tqGreen( *xP ) * tqAlpha( *xP ) / 255;
		    b = bs[col] + fracrowtofill * tqBlue( *xP ) * tqAlpha( *xP ) / 255;
		    a = as[col] + fracrowtofill * tqAlpha( *xP );
		    if ( a ) {
			r = r * 255 / a * SCALE;
			g = g * 255 / a * SCALE;
			b = b * 255 / a * SCALE;
		    }
		} else {
		    r = rs[col] + fracrowtofill * tqRed( *xP );
		    g = gs[col] + fracrowtofill * tqGreen( *xP );
		    b = bs[col] + fracrowtofill * tqBlue( *xP );
		}
		r /= SCALE;
		if ( r > maxval ) r = maxval;
		g /= SCALE;
		if ( g > maxval ) g = maxval;
		b /= SCALE;
		if ( b > maxval ) b = maxval;
		if ( as ) {
		    a /= SCALE;
		    if ( a > maxval ) a = maxval;
		    *nxP = tqRgba( (int)r, (int)g, (int)b, (int)a );
		    as[col] = HALFSCALE;
		} else {
		    *nxP = tqRgb( (int)r, (int)g, (int)b );
		}
		rs[col] = gs[col] = bs[col] = HALFSCALE;
	    }
	    fracrowleft -= fracrowtofill;
	    if ( fracrowleft == 0 ) {
		fracrowleft = syscale;
		needtoreadrow = 1;
	    }
	    fracrowtofill = SCALE;
	}

	/* Now scale X from tempxelrow into dst and write it out. */
	if ( newcols == cols ) {
	    /* shortcut X scaling if possible */
	    memcpy(dst.scanLine(rowswritten++), tempxelrow, newcols*4);
	} else {
	    long a, r, g, b;
	    long fraccoltofill, fraccolleft = 0;
	    int needcol;

	    nxP = (TQRgb*)dst.scanLine(rowswritten++);
	    fraccoltofill = SCALE;
	    a = r = g = b = HALFSCALE;
	    needcol = 0;
	    for ( col = 0, xP = tempxelrow; col < cols; ++col, ++xP ) {
		fraccolleft = sxscale;
		while ( fraccolleft >= fraccoltofill ) {
		    if ( needcol ) {
			++nxP;
			a = r = g = b = HALFSCALE;
		    }
		    if ( as ) {
			r += fraccoltofill * tqRed( *xP ) * tqAlpha( *xP ) / 255;
			g += fraccoltofill * tqGreen( *xP ) * tqAlpha( *xP ) / 255;
			b += fraccoltofill * tqBlue( *xP ) * tqAlpha( *xP ) / 255;
			a += fraccoltofill * tqAlpha( *xP );
			if ( a ) {
			    r = r * 255 / a * SCALE;
			    g = g * 255 / a * SCALE;
			    b = b * 255 / a * SCALE;
			}
		    } else {
			r += fraccoltofill * tqRed( *xP );
			g += fraccoltofill * tqGreen( *xP );
			b += fraccoltofill * tqBlue( *xP );
		    }
		    r /= SCALE;
		    if ( r > maxval ) r = maxval;
		    g /= SCALE;
		    if ( g > maxval ) g = maxval;
		    b /= SCALE;
		    if ( b > maxval ) b = maxval;
		    if (as) {
			a /= SCALE;
			if ( a > maxval ) a = maxval;
			*nxP = tqRgba( (int)r, (int)g, (int)b, (int)a );
		    } else {
			*nxP = tqRgb( (int)r, (int)g, (int)b );
		    }
		    fraccolleft -= fraccoltofill;
		    fraccoltofill = SCALE;
		    needcol = 1;
		}
		if ( fraccolleft > 0 ) {
		    if ( needcol ) {
			++nxP;
			a = r = g = b = HALFSCALE;
			needcol = 0;
		    }
		    if (as) {
			a += fraccolleft * tqAlpha( *xP );
			r += fraccolleft * tqRed( *xP ) * tqAlpha( *xP ) / 255;
			g += fraccolleft * tqGreen( *xP ) * tqAlpha( *xP ) / 255;
			b += fraccolleft * tqBlue( *xP ) * tqAlpha( *xP ) / 255;
		    } else {
			r += fraccolleft * tqRed( *xP );
			g += fraccolleft * tqGreen( *xP );
			b += fraccolleft * tqBlue( *xP );
		    }
		    fraccoltofill -= fraccolleft;
		}
	    }
	    if ( fraccoltofill > 0 ) {
		--xP;
		if (as) {
		    a += fraccolleft * tqAlpha( *xP );
		    r += fraccoltofill * tqRed( *xP ) * tqAlpha( *xP ) / 255;
		    g += fraccoltofill * tqGreen( *xP ) * tqAlpha( *xP ) / 255;
		    b += fraccoltofill * tqBlue( *xP ) * tqAlpha( *xP ) / 255;
		    if ( a ) {
			r = r * 255 / a * SCALE;
			g = g * 255 / a * SCALE;
			b = b * 255 / a * SCALE;
		    }
		} else {
		    r += fraccoltofill * tqRed( *xP );
		    g += fraccoltofill * tqGreen( *xP );
		    b += fraccoltofill * tqBlue( *xP );
		}
	    }
	    if ( ! needcol ) {
		r /= SCALE;
		if ( r > maxval ) r = maxval;
		g /= SCALE;
		if ( g > maxval ) g = maxval;
		b /= SCALE;
		if ( b > maxval ) b = maxval;
		if (as) {
		    a /= SCALE;
		    if ( a > maxval ) a = maxval;
		    *nxP = tqRgba( (int)r, (int)g, (int)b, (int)a );
		} else {
		    *nxP = tqRgb( (int)r, (int)g, (int)b );
		}
	    }
	}
    }

    if ( newrows != rows && tempxelrow )// Robust, tempxelrow might be 0 1 day
	delete [] tempxelrow;
    if ( as )				// Avoid purify complaint
	delete [] as;
    if ( rs )				// Robust, rs might be 0 one day
	delete [] rs;
    if ( gs )				// Robust, gs might be 0 one day
	delete [] gs;
    if ( bs )				// Robust, bs might be 0 one day
	delete [] bs;
}
#endif

/*!
    \enum TQImage::ScaleMode

    The functions scale() and smoothScale() use different modes for
    scaling the image. The purpose of these modes is to retain the
    ratio of the image if this is required.

    \img scaling.png

    \value ScaleFree The image is scaled freely: the resulting image
	fits exactly into the specified size; the ratio will not
	necessarily be preserved.
    \value ScaleMin The ratio of the image is preserved and the
	resulting image is guaranteed to fit into the specified size
	(it is as large as possible within these constraints) - the
	image might be smaller than the requested size.
    \value ScaleMax The ratio of the image is preserved and the
	resulting image fills the whole specified rectangle (it is as
	small as possible within these constraints) - the image might
	be larger than the requested size.
*/

#ifndef TQT_NO_IMAGE_SMOOTHSCALE
/*!
    Returns a smoothly scaled copy of the image. The returned image
    has a size of width \a w by height \a h pixels if \a mode is \c
    ScaleFree. The modes \c ScaleMin and \c ScaleMax may be used to
    preserve the ratio of the image: if \a mode is \c ScaleMin, the
    returned image is guaranteed to fit into the rectangle specified
    by \a w and \a h (it is as large as possible within the
    constraints); if \a mode is \c ScaleMax, the returned image fits
    at least into the specified rectangle (it is a small as possible
    within the constraints).

    For 32-bpp images and 1-bpp/8-bpp color images the result will be
    32-bpp, whereas \link allGray() all-gray \endlink images
    (including black-and-white 1-bpp) will produce 8-bit \link
    isGrayscale() grayscale \endlink images with the palette spanning
    256 grays from black to white.

    This function uses code based on pnmscale.c by Jef Poskanzer.

    pnmscale.c - read a portable anymap and scale it

    \legalese

    Copyright (C) 1989, 1991 by Jef Poskanzer.

    Permission to use, copy, modify, and distribute this software and
    its documentation for any purpose and without fee is hereby
    granted, provided that the above copyright notice appear in all
    copies and that both that copyright notice and this permission
    notice appear in supporting documentation. This software is
    provided "as is" without express or implied warranty.

    \sa scale() mirror()
*/
TQImage TQImage::smoothScale( int w, int h, ScaleMode mode ) const
{
    return smoothScale( TQSize( w, h ), mode );
}
#endif

#ifndef TQT_NO_IMAGE_SMOOTHSCALE
/*!
    \overload

    The requested size of the image is \a s.
*/
TQImage TQImage::smoothScale( const TQSize& s, ScaleMode mode ) const
{
    if ( isNull() ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQImage::smoothScale: Image is a null image" );
#endif
	return copy();
    }

    TQSize newSize = size();
    newSize.scale( s, (TQSize::ScaleMode)mode ); // ### remove cast in TQt 4.0
    if ( newSize == size() )
	return copy();

    if ( depth() == 32 ) {
	TQImage img( newSize, 32 );
	// 32-bpp to 32-bpp
	pnmscale( *this, img );
	return img;
    } else if ( depth() != 16 && allGray() && !hasAlphaBuffer() ) {
	// Inefficient
	return convertDepth(32).smoothScale(newSize, mode).convertDepth(8);
    } else {
	// Inefficient
	return convertDepth(32).smoothScale(newSize, mode);
    }
}
#endif

/*!
    Returns a copy of the image scaled to a rectangle of width \a w
    and height \a h according to the ScaleMode \a mode.

    \list
    \i If \a mode is \c ScaleFree, the image is scaled to (\a w,
       \a h).
    \i If \a mode is \c ScaleMin, the image is scaled to a rectangle
       as large as possible inside (\a w, \a h), preserving the aspect
       ratio.
    \i If \a mode is \c ScaleMax, the image is scaled to a rectangle
       as small as possible outside (\a w, \a h), preserving the aspect
       ratio.
    \endlist

    If either the width \a w or the height \a h is 0 or negative, this
    function returns a \link isNull() null\endlink image.

    This function uses a simple, fast algorithm. If you need better
    quality, use smoothScale() instead.

    \sa scaleWidth() scaleHeight() smoothScale() xForm()
*/
#ifndef TQT_NO_IMAGE_TRANSFORMATION
TQImage TQImage::scale( int w, int h, ScaleMode mode ) const
{
    return scale( TQSize( w, h ), mode );
}
#endif

/*!
    \overload

    The requested size of the image is \a s.
*/
#ifndef TQT_NO_IMAGE_TRANSFORMATION
TQImage TQImage::scale( const TQSize& s, ScaleMode mode ) const
{
    if ( isNull() ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQImage::scale: Image is a null image" );
#endif
	return copy();
    }
    if ( s.isEmpty() )
	return TQImage();

    TQSize newSize = size();
    newSize.scale( s, (TQSize::ScaleMode)mode ); // ### remove cast in TQt 4.0
    if ( newSize == size() )
	return copy();

    TQImage img;
    TQWMatrix wm;
    wm.scale( (double)newSize.width() / width(), (double)newSize.height() / height() );
    img = xForm( wm );
    // ### I should test and resize the image if it has not the right size
//    if ( img.width() != newSize.width() || img.height() != newSize.height() )
//	img.resize( newSize.width(), newSize.height() );
    return img;
}
#endif

/*!
    Returns a scaled copy of the image. The returned image has a width
    of \a w pixels. This function automatically calculates the height
    of the image so that the ratio of the image is preserved.

    If \a w is 0 or negative a \link isNull() null\endlink image is
    returned.

    \sa scale() scaleHeight() smoothScale() xForm()
*/
#ifndef TQT_NO_IMAGE_TRANSFORMATION
TQImage TQImage::scaleWidth( int w ) const
{
    if ( isNull() ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQImage::scaleWidth: Image is a null image" );
#endif
	return copy();
    }
    if ( w <= 0 )
	return TQImage();

    TQWMatrix wm;
    double factor = (double) w / width();
    wm.scale( factor, factor );
    return xForm( wm );
}
#endif

/*!
    Returns a scaled copy of the image. The returned image has a
    height of \a h pixels. This function automatically calculates the
    width of the image so that the ratio of the image is preserved.

    If \a h is 0 or negative a \link isNull() null\endlink image is
    returned.

    \sa scale() scaleWidth() smoothScale() xForm()
*/
#ifndef TQT_NO_IMAGE_TRANSFORMATION
TQImage TQImage::scaleHeight( int h ) const
{
    if ( isNull() ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQImage::scaleHeight: Image is a null image" );
#endif
	return copy();
    }
    if ( h <= 0 )
	return TQImage();

    TQWMatrix wm;
    double factor = (double) h / height();
    wm.scale( factor, factor );
    return xForm( wm );
}
#endif


/*!
    Returns a copy of the image that is transformed using the
    transformation matrix, \a matrix.

    The transformation \a matrix is internally adjusted to compensate
    for unwanted translation, i.e. xForm() returns the smallest image
    that contains all the transformed points of the original image.

    \sa scale() TQPixmap::xForm() TQPixmap::trueMatrix() TQWMatrix
*/
#ifndef TQT_NO_IMAGE_TRANSFORMATION
TQImage TQImage::xForm( const TQWMatrix &matrix ) const
{
    // This function uses the same algorithm as (and steals quite some
    // code from) TQPixmap::xForm().

    if ( isNull() )
	return copy();

    if ( depth() == 16 ) {
	// inefficient
	return convertDepth( 32 ).xForm( matrix );
    }

    // source image data
    int ws = width();
    int hs = height();
    int sbpl = bytesPerLine();
    uchar *sptr = bits();

    // target image data
    int wd;
    int hd;

    int bpp = depth();

    // compute size of target image
    TQWMatrix mat = TQPixmap::trueMatrix( matrix, ws, hs );
    if ( mat.m12() == 0.0F && mat.m21() == 0.0F ) {
	if ( mat.m11() == 1.0F && mat.m22() == 1.0F ) // identity matrix
	    return copy();
	hd = tqRound( mat.m22() * hs );
	wd = tqRound( mat.m11() * ws );
	hd = TQABS( hd );
	wd = TQABS( wd );
    } else {					// rotation or shearing
	TQPointArray a( TQRect(0, 0, ws, hs) );
	a = mat.map( a );
	TQRect r = a.boundingRect().normalize();
	wd = r.width();
	hd = r.height();
    }

    bool invertible;
    mat = mat.invert( &invertible );		// invert matrix
    if ( hd == 0 || wd == 0 || !invertible )	// error, return null image
	return TQImage();

    // create target image (some of the code is from TQImage::copy())
    TQImage dImage( wd, hd, depth(), numColors(), bitOrder() );

    // If the image allocation failed, we need to gracefully abort.
    if (dImage.isNull())
        return dImage;

    memcpy( dImage.colorTable(), colorTable(), numColors()*sizeof(TQRgb) );
    dImage.setAlphaBuffer( hasAlphaBuffer() );
    dImage.data->dpmx = dotsPerMeterX();
    dImage.data->dpmy = dotsPerMeterY();

    switch ( bpp ) {
	// initizialize the data
	case 1:
	    memset( dImage.bits(), 0, dImage.numBytes() );
	    break;
	case 8:
	    if ( dImage.data->ncols < 256 ) {
		// colors are left in the color table, so pick that one as transparent
		dImage.setNumColors( dImage.data->ncols+1 );
		dImage.setColor( dImage.data->ncols-1, 0x00 );
		memset( dImage.bits(), dImage.data->ncols-1, dImage.numBytes() );
	    } else {
		memset( dImage.bits(), 0, dImage.numBytes() );
	    }
	    break;
	case 16:
	    memset( dImage.bits(), 0xff, dImage.numBytes() );
	    break;
	case 32:
	    memset( dImage.bits(), 0x00, dImage.numBytes() );
	    break;
    }

    int type;
    if ( bitOrder() == BigEndian )
	type = QT_XFORM_TYPE_MSBFIRST;
    else
	type = QT_XFORM_TYPE_LSBFIRST;
    int dbpl = dImage.bytesPerLine();
    qt_xForm_helper( mat, 0, type, bpp, dImage.bits(), dbpl, 0, hd, sptr, sbpl,
		     ws, hs );
    return dImage;
}
#endif

/*!
    Builds and returns a 1-bpp mask from the alpha buffer in this
    image. Returns a \link isNull() null\endlink image if \link
    setAlphaBuffer() alpha buffer mode\endlink is disabled.

    See TQPixmap::convertFromImage() for a description of the \a
    conversion_flags argument.

    The returned image has little-endian bit order, which you can
    convert to big-endianness using convertBitOrder().

    \sa createHeuristicMask() hasAlphaBuffer() setAlphaBuffer()
*/
#ifndef TQT_NO_IMAGE_DITHER_TO_1
TQImage TQImage::createAlphaMask( int conversion_flags ) const
{
    if ( conversion_flags == 1 ) {
	conversion_flags = TQt::DiffuseAlphaDither;
    }

    if ( isNull() || !hasAlphaBuffer() )
	return TQImage();

    if ( depth() == 1 ) {
	// A monochrome pixmap, with alpha channels on those two colors.
	// Pretty unlikely, so use less efficient solution.
	return convertDepth(8, conversion_flags)
		.createAlphaMask( conversion_flags );
    }

    TQImage mask1;
    dither_to_1( this, &mask1, conversion_flags, true );
    return mask1;
}
#endif

#ifndef TQT_NO_IMAGE_HEURISTIC_MASK
/*!
    Creates and returns a 1-bpp heuristic mask for this image. It
    works by selecting a color from one of the corners, then chipping
    away pixels of that color starting at all the edges.

    The four corners vote for which color is to be masked away. In
    case of a draw (this generally means that this function is not
    applicable to the image), the result is arbitrary.

    The returned image has little-endian bit order, which you can
    convert to big-endianness using convertBitOrder().

    If \a clipTight is true the mask is just large enough to cover the
    pixels; otherwise, the mask is larger than the data pixels.

    This function disregards the \link hasAlphaBuffer() alpha buffer
    \endlink.

    \sa createAlphaMask()
*/

TQImage TQImage::createHeuristicMask( bool clipTight ) const
{
    if ( isNull() ) {
	TQImage nullImage;
	return nullImage;
    }
    if ( depth() != 32 ) {
	TQImage img32 = convertDepth(32);
	return img32.createHeuristicMask(clipTight);
    }

#define PIX(x,y)  (*((TQRgb*)scanLine(y)+x) & 0x00ffffff)

    int w = width();
    int h = height();
    TQImage m(w, h, 1, 2, TQImage::LittleEndian);
    m.setColor( 0, 0xffffff );
    m.setColor( 1, 0 );
    m.fill( 0xff );

    TQRgb background = PIX(0,0);
    if ( background != PIX(w-1,0) &&
	 background != PIX(0,h-1) &&
	 background != PIX(w-1,h-1) ) {
	background = PIX(w-1,0);
	if ( background != PIX(w-1,h-1) &&
	     background != PIX(0,h-1) &&
	     PIX(0,h-1) == PIX(w-1,h-1) ) {
	    background = PIX(w-1,h-1);
	}
    }

    int x,y;
    bool done = false;
    uchar *ypp, *ypc, *ypn;
    while( !done ) {
	done = true;
	ypn = m.scanLine(0);
	ypc = 0;
	for ( y = 0; y < h; y++ ) {
	    ypp = ypc;
	    ypc = ypn;
	    ypn = (y == h-1) ? 0 : m.scanLine(y+1);
	    TQRgb *p = (TQRgb *)scanLine(y);
	    for ( x = 0; x < w; x++ ) {
		// slowness here - it's possible to do six of these tests
		// together in one go. oh well.
		if ( ( x == 0 || y == 0 || x == w-1 || y == h-1 ||
		       !(*(ypc + ((x-1) >> 3)) & (1 << ((x-1) & 7))) ||
		       !(*(ypc + ((x+1) >> 3)) & (1 << ((x+1) & 7))) ||
		       !(*(ypp + (x     >> 3)) & (1 << (x     & 7))) ||
		       !(*(ypn + (x     >> 3)) & (1 << (x     & 7))) ) &&
		     (	(*(ypc + (x     >> 3)) & (1 << (x     & 7))) ) &&
		     ( (*p & 0x00ffffff) == background ) ) {
		    done = false;
		    *(ypc + (x >> 3)) &= ~(1 << (x & 7));
		}
		p++;
	    }
	}
    }

    if ( !clipTight ) {
	ypn = m.scanLine(0);
	ypc = 0;
	for ( y = 0; y < h; y++ ) {
	    ypp = ypc;
	    ypc = ypn;
	    ypn = (y == h-1) ? 0 : m.scanLine(y+1);
	    TQRgb *p = (TQRgb *)scanLine(y);
	    for ( x = 0; x < w; x++ ) {
		if ( (*p & 0x00ffffff) != background ) {
		    if ( x > 0 )
			*(ypc + ((x-1) >> 3)) |= (1 << ((x-1) & 7));
		    if ( x < w-1 )
			*(ypc + ((x+1) >> 3)) |= (1 << ((x+1) & 7));
		    if ( y > 0 )
			*(ypp + (x >> 3)) |= (1 << (x & 7));
		    if ( y < h-1 )
			*(ypn + (x >> 3)) |= (1 << (x & 7));
		}
		p++;
	    }
	}
    }

#undef PIX

    return m;
}
#endif //TQT_NO_IMAGE_HEURISTIC_MASK

#ifndef TQT_NO_IMAGE_MIRROR
/*
  This code is contributed by Philipp Lang,
  GeneriCom Software Germany (www.generi.com)
  under the terms of the TQPL, Version 1.0
*/

/*!
    \overload

    Returns a mirror of the image, mirrored in the horizontal and/or
    the vertical direction depending on whether \a horizontal and \a
    vertical are set to true or false. The original image is not
    changed.

    \sa smoothScale()
*/
TQImage TQImage::mirror(bool horizontal, bool vertical) const
{
    int w = width();
    int h = height();
    if ( (w <= 1 && h <= 1) || (!horizontal && !vertical) )
	return copy();

    // Create result image, copy colormap
    TQImage result(w, h, depth(), numColors(), bitOrder());
    memcpy(result.colorTable(), colorTable(), numColors()*sizeof(TQRgb));
    result.setAlphaBuffer(hasAlphaBuffer());

    if (depth() == 1)
	w = (w+7)/8;
    int dxi = horizontal ? -1 : 1;
    int dxs = horizontal ? w-1 : 0;
    int dyi = vertical ? -1 : 1;
    int dy = vertical ? h-1: 0;

    // 1 bit, 8 bit
    if (depth() == 1 || depth() == 8) {
	for (int sy = 0; sy < h; sy++, dy += dyi) {
	    TQ_UINT8* ssl = (TQ_UINT8*)(data->bits[sy]);
	    TQ_UINT8* dsl = (TQ_UINT8*)(result.data->bits[dy]);
	    int dx = dxs;
	    for (int sx = 0; sx < w; sx++, dx += dxi)
		dsl[dx] = ssl[sx];
	}
    }
#ifndef TQT_NO_IMAGE_TRUECOLOR
#ifndef TQT_NO_IMAGE_16_BIT
    // 16 bit
    else if (depth() == 16) {
	for (int sy = 0; sy < h; sy++, dy += dyi) {
	    TQ_UINT16* ssl = (TQ_UINT16*)(data->bits[sy]);
	    TQ_UINT16* dsl = (TQ_UINT16*)(result.data->bits[dy]);
	    int dx = dxs;
	    for (int sx = 0; sx < w; sx++, dx += dxi)
		dsl[dx] = ssl[sx];
	}
    }
#endif
    // 32 bit
    else if (depth() == 32) {
	for (int sy = 0; sy < h; sy++, dy += dyi) {
	    TQ_UINT32* ssl = (TQ_UINT32*)(data->bits[sy]);
	    TQ_UINT32* dsl = (TQ_UINT32*)(result.data->bits[dy]);
	    int dx = dxs;
	    for (int sx = 0; sx < w; sx++, dx += dxi)
		dsl[dx] = ssl[sx];
	}
    }
#endif

    // special handling of 1 bit images for horizontal mirroring
    if (horizontal && depth() == 1) {
	int shift = width() % 8;
	for (int y = h-1; y >= 0; y--) {
	    TQ_UINT8* a0 = (TQ_UINT8*)(result.data->bits[y]);
	    // Swap bytes
	    TQ_UINT8* a = a0+dxs;
	    while (a >= a0) {
		*a = bitflip[*a];
		a--;
	    }
	    // Shift bits if unaligned
	    if (shift != 0) {
		a = a0+dxs;
		TQ_UINT8 c = 0;
		if (bitOrder() == TQImage::LittleEndian) {
		    while (a >= a0) {
			TQ_UINT8 nc = *a << shift;
			*a = (*a >> (8-shift)) | c;
			--a;
			c = nc;
		    }
		} else {
		    while (a >= a0) {
			TQ_UINT8 nc = *a >> shift;
			*a = (*a << (8-shift)) | c;
			--a;
			c = nc;
		    }
		}
	    }
	}
    }

    return result;
}

/*!
    Returns a TQImage which is a vertically mirrored copy of this
    image. The original TQImage is not changed.
*/

TQImage TQImage::mirror() const
{
    return mirror(false,true);
}
#endif //TQT_NO_IMAGE_MIRROR

/*!
    Returns a TQImage in which the values of the red and blue
    components of all pixels have been swapped, effectively converting
    an RGB image to a BGR image. The original TQImage is not changed.
*/

TQImage TQImage::swapRGB() const
{
    TQImage res = copy();
    if ( !isNull() ) {
#ifndef TQT_NO_IMAGE_TRUECOLOR
	if ( depth() == 32 ) {
	    for ( int i=0; i < height(); i++ ) {
		uint *p = (uint*)scanLine( i );
		uint *q = (uint*)res.scanLine( i );
		uint *end = p + width();
		while ( p < end ) {
		    *q = ((*p << 16) & 0xff0000) | ((*p >> 16) & 0xff) |
			 (*p & 0xff00ff00);
		    p++;
		    q++;
		}
	    }
#ifndef TQT_NO_IMAGE_16_BIT
	} else if ( depth() == 16 ) {
	    tqWarning( "TQImage::swapRGB not implemented for 16bpp" );
#endif
	} else
#endif //TQT_NO_IMAGE_TRUECOLOR
	    {
	    uint* p = (uint*)colorTable();
	    uint* q = (uint*)res.colorTable();
	    if ( p && q ) {
		for ( int i=0; i < numColors(); i++ ) {
		    *q = ((*p << 16) & 0xff0000) | ((*p >> 16) & 0xff) |
			 (*p & 0xff00ff00);
		    p++;
		    q++;
		}
	    }
	}
    }
    return res;
}

#ifndef TQT_NO_IMAGEIO
/*!
    Returns a string that specifies the image format of the file \a
    fileName, or 0 if the file cannot be read or if the format is not
    recognized.

    The TQImageIO documentation lists the guaranteed supported image
    formats, or use TQImage::inputFormats() and TQImage::outputFormats()
    to get lists that include the installed formats.

    \sa load() save()
*/

const char* TQImage::imageFormat( const TQString &fileName )
{
    return TQImageIO::imageFormat( fileName );
}

/*!
    Returns a list of image formats that are supported for image
    input.

    \sa outputFormats() inputFormatList() TQImageIO
*/
TQStrList TQImage::inputFormats()
{
    return TQImageIO::inputFormats();
}
#ifndef TQT_NO_STRINGLIST
/*!
    Returns a list of image formats that are supported for image
    input.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myImage.inputFormatList();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa outputFormatList() inputFormats() TQImageIO
*/
TQStringList TQImage::inputFormatList()
{
    return TQStringList::fromStrList(TQImageIO::inputFormats());
}


/*!
    Returns a list of image formats that are supported for image
    output.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myImage.outputFormatList();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa inputFormatList() outputFormats() TQImageIO
*/
TQStringList TQImage::outputFormatList()
{
    return TQStringList::fromStrList(TQImageIO::outputFormats());
}
#endif //TQT_NO_STRINGLIST

/*!
    Returns a list of image formats that are supported for image
    output.

    \sa inputFormats() outputFormatList() TQImageIO
*/
TQStrList TQImage::outputFormats()
{
    return TQImageIO::outputFormats();
}


/*!
    Loads an image from the file \a fileName. Returns true if the
    image was successfully loaded; otherwise returns false.

    If \a format is specified, the loader attempts to read the image
    using the specified format. If \a format is not specified (which
    is the default), the loader reads a few bytes from the header to
    guess the file format.

    The TQImageIO documentation lists the supported image formats and
    explains how to add extra formats.

    \sa loadFromData() save() imageFormat() TQPixmap::load() TQImageIO
*/

bool TQImage::load( const TQString &fileName, const char* format )
{
    TQImageIO io( fileName, format );
    bool result = io.read();
    if ( result )
	operator=( io.image() );
    return result;
}

/*!
    Loads an image from the first \a len bytes of binary data in \a
    buf. Returns true if the image was successfully loaded; otherwise
    returns false.

    If \a format is specified, the loader attempts to read the image
    using the specified format. If \a format is not specified (which
    is the default), the loader reads a few bytes from the header to
    guess the file format.

    The TQImageIO documentation lists the supported image formats and
    explains how to add extra formats.

    \sa load() save() imageFormat() TQPixmap::loadFromData() TQImageIO
*/

bool TQImage::loadFromData( const uchar *buf, uint len, const char *format )
{
    TQByteArray a;
    a.setRawData( (char *)buf, len );
    TQBuffer b( a );
    b.open( IO_ReadOnly );
    TQImageIO io( &b, format );
    bool result = io.read();
    b.close();
    a.resetRawData( (char *)buf, len );
    if ( result )
	operator=( io.image() );
    return result;
}

/*!
    \overload

    Loads an image from the TQByteArray \a buf.
*/
bool TQImage::loadFromData( TQByteArray buf, const char *format )
{
    return loadFromData( (const uchar *)(buf.data()), buf.size(), format );
}

/*!
    Saves the image to the file \a fileName, using the image file
    format \a format and a quality factor of \a quality. \a quality
    must be in the range 0..100 or -1. Specify 0 to obtain small
    compressed files, 100 for large uncompressed files, and -1 (the
    default) to use the default settings.

    Returns true if the image was successfully saved; otherwise
    returns false.

    \sa load() loadFromData() imageFormat() TQPixmap::save() TQImageIO
*/

bool TQImage::save( const TQString &fileName, const char* format, int quality ) const
{
    if ( isNull() )
	return false;				// nothing to save
    TQImageIO io( fileName, format );
    return doImageIO( &io, quality );
}

/*!
    \overload

    This function writes a TQImage to the TQIODevice, \a device. This
    can be used, for example, to save an image directly into a
    TQByteArray:
    \code
    TQImage image;
    TQByteArray ba;
    TQBuffer buffer( ba );
    buffer.open( IO_WriteOnly );
    image.save( &buffer, "PNG" ); // writes image into ba in PNG format
    \endcode
*/

bool TQImage::save( TQIODevice* device, const char* format, int quality ) const
{
    if ( isNull() )
	return false;				// nothing to save
    TQImageIO io( device, format );
    return doImageIO( &io, quality );
}

/* \internal
*/

bool TQImage::doImageIO( TQImageIO* io, int quality ) const
{
    if ( !io )
	return false;
    io->setImage( *this );
#if defined(QT_CHECK_RANGE)
    if ( quality > 100  || quality < -1 )
	tqWarning( "TQPixmap::save: quality out of range [-1,100]" );
#endif
    if ( quality >= 0 )
	io->setQuality( TQMIN(quality,100) );
    return io->write();
}
#endif //TQT_NO_IMAGEIO

/*****************************************************************************
  TQImage stream functions
 *****************************************************************************/
#if !defined(TQT_NO_DATASTREAM) && !defined(TQT_NO_IMAGEIO)
/*!
    \relates TQImage

    Writes the image \a image to the stream \a s as a PNG image, or as a
    BMP image if the stream's version is 1.

    Note that writing the stream to a file will not produce a valid image file.

    \sa TQImage::save()
    \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQImage &image )
{
    if ( s.version() >= 5 ) {
	if ( image.isNull() ) {
	    s << (TQ_INT32) 0; // null image marker
	    return s;
	} else {
	    s << (TQ_INT32) 1;
	    // continue ...
	}
    }
    TQImageIO io;
    io.setIODevice( s.device() );
    if ( s.version() == 1 )
	io.setFormat( "BMP" );
    else
	io.setFormat( "PNG" );

    io.setImage( image );
    io.write();
    return s;
}

/*!
    \relates TQImage

    Reads an image from the stream \a s and stores it in \a image.

    \sa TQImage::load()
    \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQImage &image )
{
    if ( s.version() >= 5 ) {
	TQ_INT32 nullMarker;
	s >> nullMarker;
	if ( !nullMarker ) {
	    image = TQImage(); // null image
	    return s;
	}
    }
    TQImageIO io( s.device(), 0 );
    if ( io.read() )
	image = io.image();
    return s;
}
#endif

/*****************************************************************************
  Standard image io handlers (defined below)
 *****************************************************************************/

// standard image io handlers (defined below)
#ifndef TQT_NO_IMAGEIO_BMP
static void read_bmp_image( TQImageIO * );
static void write_bmp_image( TQImageIO * );
#endif
#ifndef TQT_NO_IMAGEIO_PPM
static void read_pbm_image( TQImageIO * );
static void write_pbm_image( TQImageIO * );
#endif
#ifndef TQT_NO_IMAGEIO_XBM
static void read_xbm_image( TQImageIO * );
static void write_xbm_image( TQImageIO * );
#endif
#ifndef TQT_NO_IMAGEIO_XPM
static void read_xpm_image( TQImageIO * );
static void write_xpm_image( TQImageIO * );
#endif

#ifndef TQT_NO_ASYNC_IMAGE_IO
static void read_async_image( TQImageIO * ); // Not in table of handlers
#endif

/*****************************************************************************
  Misc. utility functions
 *****************************************************************************/
#if !defined(TQT_NO_IMAGEIO_XPM) || !defined(TQT_NO_IMAGEIO_XBM)
static TQString fbname( const TQString &fileName ) // get file basename (sort of)
{
    TQString s = fileName;
    if ( !s.isEmpty() ) {
	int i;
	if ( (i = s.findRev('/')) >= 0 )
	    s = s.mid( i );
	if ( (i = s.findRev('\\')) >= 0 )
	    s = s.mid( i );
	TQRegExp r( TQString::fromLatin1("[a-zA-Z][a-zA-Z0-9_]*") );
	int p = r.search( s );
	if ( p == -1 )
	    s.truncate( 0 );
	else
	    s = s.mid( p, r.matchedLength() );
    }
    if ( s.isEmpty() )
	s = TQString::fromLatin1( "dummy" );
    return s;
}
#endif

#ifndef TQT_NO_IMAGEIO_BMP
static void swapPixel01( TQImage *image )	// 1-bpp: swap 0 and 1 pixels
{
    int i;
    if ( image->depth() == 1 && image->numColors() == 2 ) {
	uint *p = (uint *)image->bits();
	int nbytes = image->numBytes();
	for ( i=0; i<nbytes/4; i++ ) {
	    *p = ~*p;
	    p++;
	}
	uchar *p2 = (uchar *)p;
	for ( i=0; i<(nbytes&3); i++ ) {
	    *p2 = ~*p2;
	    p2++;
	}
	TQRgb t = image->color(0);		// swap color 0 and 1
	image->setColor( 0, image->color(1) );
	image->setColor( 1, t );
    }
}
#endif


/*****************************************************************************
  TQImageIO member functions
 *****************************************************************************/

/*!
    \class TQImageIO ntqimage.h

    \brief The TQImageIO class contains parameters for loading and
    saving images.

    \ingroup images
    \ingroup graphics
    \ingroup io

    TQImageIO contains a TQIODevice object that is used for image data
    I/O. The programmer can install new image file formats in addition
    to those that TQt provides.

    TQt currently supports the following image file formats: PNG, BMP,
    XBM, XPM and PNM. It may also support JPEG, MNG and GIF, if
    specially configured during compilation. The different PNM formats
    are: PBM (P1 or P4), PGM (P2 or P5), and PPM (P3 or P6).

    You don't normally need to use this class; TQPixmap::load(),
    TQPixmap::save(), and TQImage contain sufficient functionality.

    For image files that contain sequences of images, only the first
    is read. See TQMovie for loading multiple images.

    PBM, PGM, and PPM format \e output is always in the more condensed
    raw format. PPM and PGM files with more than 256 levels of
    intensity are scaled down when reading.

    \warning If you are in a country which recognizes software patents
    and in which Unisys holds a patent on LZW compression and/or
    decompression and you want to use GIF, Unisys may require you to
    license the technology. Such countries include Canada, Japan, the
    USA, France, Germany, Italy and the UK.

    GIF support may be removed completely in a future version of TQt.
    We recommend using the PNG format.

    \sa TQImage TQPixmap TQFile TQMovie
*/

#ifndef TQT_NO_IMAGEIO
struct TQImageIOData
{
    const char *parameters;
    int quality;
    float gamma;
};

/*!
    Constructs a TQImageIO object with all parameters set to zero.
*/

TQImageIO::TQImageIO()
{
    init();
}

/*!
    Constructs a TQImageIO object with the I/O device \a ioDevice and a
    \a format tag.
*/

TQImageIO::TQImageIO( TQIODevice *ioDevice, const char *format )
    : frmt(format)
{
    init();
    iodev  = ioDevice;
}

/*!
    Constructs a TQImageIO object with the file name \a fileName and a
    \a format tag.
*/

TQImageIO::TQImageIO( const TQString &fileName, const char* format )
    : frmt(format), fname(fileName)
{
    init();
}

/*!
    Contains initialization common to all TQImageIO constructors.
*/

void TQImageIO::init()
{
    d = new TQImageIOData();
    d->parameters = 0;
    d->quality = -1; // default quality of the current format
    d->gamma=0.0f;
    iostat = 0;
    iodev  = 0;
}

/*!
    Destroys the object and all related data.
*/

TQImageIO::~TQImageIO()
{
    if ( d->parameters )
	delete [] (char*)d->parameters;
    delete d;
}


/*****************************************************************************
  TQImageIO image handler functions
 *****************************************************************************/

class TQImageHandler
{
public:
    TQImageHandler( const char *f, const char *h, const TQCString& fl,
		   image_io_handler r, image_io_handler w );
    TQCString	      format;			// image format
    TQRegExp	      header;			// image header pattern
    enum TMode { Untranslated=0, TranslateIn, TranslateInOut } text_mode;
    image_io_handler  read_image;		// image read function
    image_io_handler  write_image;		// image write function
    bool	      obsolete;			// support not "published"
};

TQImageHandler::TQImageHandler( const char *f, const char *h, const TQCString& fl,
			      image_io_handler r, image_io_handler w )
    : format(f), header(TQString::fromLatin1(h))
{
    text_mode = Untranslated;
    if ( fl.contains('t') )
	text_mode = TranslateIn;
    else if ( fl.contains('T') )
	text_mode = TranslateInOut;
    obsolete = fl.contains('O');
    read_image	= r;
    write_image = w;
}

typedef TQPtrList<TQImageHandler> TQIHList;// list of image handlers
static TQIHList *imageHandlers = 0;
#ifndef TQT_NO_COMPONENT
static TQPluginManager<TQImageFormatInterface> *plugin_manager = 0;
#else
static void *plugin_manager = 0;
#endif

void tqt_init_image_plugins()
{
#ifndef TQT_NO_COMPONENT
    if ( plugin_manager )
	return;

    plugin_manager = new TQPluginManager<TQImageFormatInterface>( IID_QImageFormat, TQApplication::libraryPaths(), "/imageformats" );

    TQStringList features = plugin_manager->featureList();
    TQStringList::Iterator it = features.begin();
    while ( it != features.end() ) {
	TQString str = *it;
	++it;
	TQInterfacePtr<TQImageFormatInterface> iface;
	plugin_manager->queryInterface( str, &iface );
	if ( iface )
	    iface->installIOHandler( str );
    }
#endif
}

static void cleanup()
{
    // make sure that image handlers are delete before plugin manager
    delete imageHandlers;
    imageHandlers = 0;
#ifndef TQT_NO_COMPONENT
    delete plugin_manager;
    plugin_manager = 0;
#endif
}

void tqt_init_image_handlers()		// initialize image handlers
{
    if ( !imageHandlers ) {
	imageHandlers = new TQIHList;
	TQ_CHECK_PTR( imageHandlers );
	imageHandlers->setAutoDelete( true );
	tqAddPostRoutine( cleanup );
#ifndef TQT_NO_IMAGEIO_BMP
	TQImageIO::defineIOHandler( "BMP", "^BM", 0,
				   read_bmp_image, write_bmp_image );
#endif
#ifndef TQT_NO_IMAGEIO_PPM
	TQImageIO::defineIOHandler( "PBM", "^P1", "t",
				   read_pbm_image, write_pbm_image );
	TQImageIO::defineIOHandler( "PBMRAW", "^P4", "O",
				   read_pbm_image, write_pbm_image );
	TQImageIO::defineIOHandler( "PGM", "^P2", "t",
				   read_pbm_image, write_pbm_image );
	TQImageIO::defineIOHandler( "PGMRAW", "^P5", "O",
				   read_pbm_image, write_pbm_image );
	TQImageIO::defineIOHandler( "PPM", "^P3", "t",
				   read_pbm_image, write_pbm_image );
	TQImageIO::defineIOHandler( "PPMRAW", "^P6", "O",
				   read_pbm_image, write_pbm_image );
#endif
#ifndef TQT_NO_IMAGEIO_XBM
	TQImageIO::defineIOHandler( "XBM", "^((/\\*(?!.XPM.\\*/))|#define)", "T",
				   read_xbm_image, write_xbm_image );
#endif
#ifndef TQT_NO_IMAGEIO_XPM
	TQImageIO::defineIOHandler( "XPM", "/\\*.XPM.\\*/", "T",
				   read_xpm_image, write_xpm_image );
#endif
#ifndef TQT_NO_IMAGEIO_MNG
	qInitMngIO();
#endif
#ifndef TQT_NO_IMAGEIO_PNG
	qInitPngIO();
#endif
#ifndef TQT_NO_IMAGEIO_JPEG
	qInitJpegIO();
#endif
    }
}

static TQImageHandler *get_image_handler( const char *format )
{						// get pointer to handler
    tqt_init_image_handlers();
    tqt_init_image_plugins();
    TQImageHandler *p = imageHandlers->first();
    while ( p ) {				// traverse list
	if ( p->format == format )
	    return p;
	p = imageHandlers->next();
    }
    return 0;					// no such handler
}


/*!
    Defines an image I/O handler for the image format called \a
    format, which is recognized using the \link ntqregexp.html#details
    regular expression\endlink \a header, read using \a readImage and
    written using \a writeImage.

    \a flags is a string of single-character flags for this format.
    The only flag defined currently is T (upper case), so the only
    legal value for \a flags are "T" and the empty string. The "T"
    flag means that the image file is a text file, and TQt should treat
    all newline conventions as equivalent. (XPM files and some PPM
    files are text files for example.)

    \a format is used to select a handler to write a TQImage; \a header
    is used to select a handler to read an image file.

    If \a readImage is a null pointer, the TQImageIO will not be able
    to read images in \a format. If \a writeImage is a null pointer,
    the TQImageIO will not be able to write images in \a format. If
    both are null, the TQImageIO object is valid but useless.

    Example:
    \code
	void readGIF( TQImageIO *image )
	{
	// read the image using the image->ioDevice()
	}

	void writeGIF( TQImageIO *image )
	{
	// write the image using the image->ioDevice()
	}

	// add the GIF image handler

	TQImageIO::defineIOHandler( "GIF",
				   "^GIF[0-9][0-9][a-z]",
				   0,
				   readGIF,
				   writeGIF );
    \endcode

    Before the regex test, all the 0 bytes in the file header are
    converted to 1 bytes. This is done because when TQt was
    ASCII-based, TQRegExp could not handle 0 bytes in strings.

    The regexp is only applied on the first 14 bytes of the file.

    Note that TQt assumes that there is only one handler per format; if
    two handlers support the same format, TQt will choose one
    arbitrarily. It is not possible to have one handler support
    reading, and another support writing.
*/

void TQImageIO::defineIOHandler( const char *format,
				const char *header,
				const char *flags,
				image_io_handler readImage,
				image_io_handler writeImage )
{
    tqt_init_image_handlers();
    TQImageHandler *p;
    p = new TQImageHandler( format, header, flags,
			   readImage, writeImage );
    TQ_CHECK_PTR( p );
    imageHandlers->insert( 0, p );
}


/*****************************************************************************
  TQImageIO normal member functions
 *****************************************************************************/

/*!
    \fn const TQImage &TQImageIO::image() const

    Returns the image currently set.

    \sa setImage()
*/

/*!
    \fn int TQImageIO::status() const

    Returns the image's IO status. A non-zero value indicates an
    error, whereas 0 means that the IO operation was successful.

    \sa setStatus()
*/

/*!
    \fn const char *TQImageIO::format() const

    Returns the image format string or 0 if no format has been
    explicitly set.
*/

/*!
    \fn TQIODevice *TQImageIO::ioDevice() const

    Returns the IO device currently set.

    \sa setIODevice()
*/

/*!
    \fn TQString TQImageIO::fileName() const

    Returns the file name currently set.

    \sa setFileName()
*/

/*!
    \fn TQString TQImageIO::description() const

    Returns the image description string.

    \sa setDescription()
*/


/*!
    Sets the image to \a image.

    \sa image()
*/

void TQImageIO::setImage( const TQImage &image )
{
    im = image;
}

/*!
    Sets the image IO status to \a status. A non-zero value indicates
    an error, whereas 0 means that the IO operation was successful.

    \sa status()
*/

void TQImageIO::setStatus( int status )
{
    iostat = status;
}

/*!
    Sets the image format to \a format for the image to be read or
    written.

    It is necessary to specify a format before writing an image, but
    it is not necessary to specify a format before reading an image.

    If no format has been set, TQt guesses the image format before
    reading it. If a format is set the image will only be read if it
    has that format.

    \sa read() write() format()
*/

void TQImageIO::setFormat( const char *format )
{
    frmt = format;
}

/*!
    Sets the IO device to be used for reading or writing an image.

    Setting the IO device allows images to be read/written to any
    block-oriented TQIODevice.

    If \a ioDevice is not null, this IO device will override file name
    settings.

    \sa setFileName()
*/

void TQImageIO::setIODevice( TQIODevice *ioDevice )
{
    iodev = ioDevice;
}

/*!
    Sets the name of the file to read or write an image from to \a
    fileName.

    \sa setIODevice()
*/

void TQImageIO::setFileName( const TQString &fileName )
{
    fname = fileName;
}

/*!
    Returns the quality of the written image, related to the
    compression ratio.

    \sa setQuality() TQImage::save()
*/

int TQImageIO::quality() const
{
    return d->quality;
}

/*!
    Sets the quality of the written image to \a q, related to the
    compression ratio.

    \a q must be in the range -1..100. Specify 0 to obtain small
    compressed files, 100 for large uncompressed files. (-1 signifies
    the default compression.)

    \sa quality() TQImage::save()
*/

void TQImageIO::setQuality( int q )
{
    d->quality = q;
}

/*!
    Returns the image's parameters string.

    \sa setParameters()
*/

const char *TQImageIO::parameters() const
{
    return d->parameters;
}

/*!
    Sets the image's parameter string to \a parameters. This is for
    image handlers that require special parameters.

    Although the current image formats supported by TQt ignore the
    parameters string, it may be used in future extensions or by
    contributions (for example, JPEG).

    \sa parameters()
*/

void TQImageIO::setParameters( const char *parameters )
{
    if ( d && d->parameters )
	delete [] (char*)d->parameters;
    d->parameters = tqstrdup( parameters );
}

/*!
    Sets the gamma value at which the image will be viewed to \a
    gamma. If the image format stores a gamma value for which the
    image is intended to be used, then this setting will be used to
    modify the image. Setting to 0.0 will disable gamma correction
    (i.e. any specification in the file will be ignored).

    The default value is 0.0.

    \sa gamma()
*/
void TQImageIO::setGamma( float gamma )
{
    d->gamma=gamma;
}

/*!
    Returns the gamma value at which the image will be viewed.

    \sa setGamma()
*/
float TQImageIO::gamma() const
{
    return d->gamma;
}

/*!
    Sets the image description string for image handlers that support
    image descriptions to \a description.

    Currently, no image format supported by TQt uses the description
    string.
*/

void TQImageIO::setDescription( const TQString &description )
{
    descr = description;
}


/*!
    Returns a string that specifies the image format of the file \a
    fileName, or null if the file cannot be read or if the format is
    not recognized.
*/

const char* TQImageIO::imageFormat( const TQString &fileName )
{
    TQFile file( fileName );
    if ( !file.open(IO_ReadOnly) )
	return 0;
    const char* format = imageFormat( &file );
    file.close();
    return format;
}

/*!
    \overload

    Returns a string that specifies the image format of the image read
    from IO device \a d, or 0 if the device cannot be read or if the
    format is not recognized.

    Make sure that \a d is at the right position in the device (for
    example, at the beginning of the file).

    \sa TQIODevice::at()
*/

const char *TQImageIO::imageFormat( TQIODevice *d )
{
    // if you change this change the documentation for defineIOHandler()
    const int buflen = 14;

    char buf[buflen];
    char buf2[buflen];
    tqt_init_image_handlers();
    tqt_init_image_plugins();
    int pos = d->at();			// save position
    int rdlen = d->readBlock( buf, buflen );	// read a few bytes

    if ( rdlen != buflen )
	return 0;

    memcpy( buf2, buf, buflen );

    const char* format = 0;
    for ( int n = 0; n < rdlen; n++ )
	if ( buf[n] == '\0' )
	    buf[n] = '\001';
    if ( d->status() == IO_Ok && rdlen > 0 ) {
	buf[rdlen - 1] = '\0';
	TQString bufStr = TQString::fromLatin1(buf);
	TQImageHandler *p = imageHandlers->first();
	int bestMatch = -1;
	while ( p ) {
	    if ( p->read_image && p->header.search(bufStr) != -1 ) {
		// try match with header if a read function is available
		if (p->header.matchedLength() > bestMatch) {
		    // keep looking for best match
		    format = p->format;
		    bestMatch = p->header.matchedLength();
		}
	    }
	    p = imageHandlers->next();
	}
    }
    d->at( pos );				// restore position
#ifndef TQT_NO_ASYNC_IMAGE_IO
    if ( !format )
	format = TQImageDecoder::formatName( (uchar*)buf2, rdlen );
#endif

    return format;
}

/*!
    Returns a sorted list of image formats that are supported for
    image input.
*/
TQStrList TQImageIO::inputFormats()
{
    TQStrList result;

    tqt_init_image_handlers();
    tqt_init_image_plugins();

#ifndef TQT_NO_ASYNC_IMAGE_IO
    // Include asynchronous loaders first.
    result = TQImageDecoder::inputFormats();
#endif

    TQImageHandler *p = imageHandlers->first();
    while ( p ) {
	if ( p->read_image
	    && !p->obsolete
	    && !result.contains(p->format) )
	{
	    result.inSort(p->format);
	}
	p = imageHandlers->next();
    }

    return result;
}

/*!
    Returns a sorted list of image formats that are supported for
    image output.
*/
TQStrList TQImageIO::outputFormats()
{
    TQStrList result;

    tqt_init_image_handlers();
    tqt_init_image_plugins();

    // Include asynchronous writers (!) first.
    // (None)

    TQImageHandler *p = imageHandlers->first();
    while ( p ) {
	if ( p->write_image
	    && !p->obsolete
	    && !result.contains(p->format) )
	{
	    result.inSort(p->format);
	}
	p = imageHandlers->next();
    }

    return result;
}



/*!
    Reads an image into memory and returns true if the image was
    successfully read; otherwise returns false.

    Before reading an image you must set an IO device or a file name.
    If both an IO device and a file name have been set, the IO device
    will be used.

    Setting the image file format string is optional.

    Note that this function does \e not set the \link format()
    format\endlink used to read the image. If you need that
    information, use the imageFormat() static functions.

    Example:

    \code
	TQImageIO iio;
	TQPixmap  pixmap;
	iio.setFileName( "vegeburger.bmp" );
	if ( image.read() )        // ok
	    pixmap = iio.image();  // convert to pixmap
    \endcode

    \sa setIODevice() setFileName() setFormat() write() TQPixmap::load()
*/

bool TQImageIO::read()
{
    TQFile	   file;
    const char	  *image_format;
    TQImageHandler *h;

    if ( iodev ) {				// read from io device
	// ok, already open
    } else if ( !fname.isEmpty() ) {		// read from file
	file.setName( fname );
	if ( !file.open(IO_ReadOnly) )
	    return false;			// cannot open file
	iodev = &file;
    } else {					// no file name or io device
	return false;
    }
    if (frmt.isEmpty()) {
	// Try to guess format
	image_format = imageFormat( iodev );	// get image format
	if ( !image_format ) {
	    if ( file.isOpen() ) {			// unknown format
		file.close();
		iodev = 0;
	    }
	    return false;
	}
    } else {
	image_format = frmt;
    }

    h = get_image_handler( image_format );
    if ( file.isOpen() ) {
#if !defined(Q_OS_UNIX)
	if ( h && h->text_mode ) {		// reopen in translated mode
	    file.close();
	    file.open( IO_ReadOnly | IO_Translate );
	}
	else
#endif
	    file.at( 0 );			// position to start
    }
    iostat = 1;					// assume error

    if ( h && h->read_image ) {
	(*h->read_image)( this );
    }
#ifndef TQT_NO_ASYNC_IMAGE_IO
    else {
	// Format name, but no handler - must be an asychronous reader
	read_async_image( this );
    }
#endif

    if ( file.isOpen() ) {			// image was read using file
	file.close();
	iodev = 0;
    }
    return iostat == 0;				// image successfully read?
}


/*!
    Writes an image to an IO device and returns true if the image was
    successfully written; otherwise returns false.

    Before writing an image you must set an IO device or a file name.
    If both an IO device and a file name have been set, the IO device
    will be used.

    The image will be written using the specified image format.

    Example:
    \code
	TQImageIO iio;
	TQImage   im;
	im = pixmap; // convert to image
	iio.setImage( im );
	iio.setFileName( "vegeburger.bmp" );
	iio.setFormat( "BMP" );
	if ( iio.write() )
	    // returned true if written successfully
    \endcode

    \sa setIODevice() setFileName() setFormat() read() TQPixmap::save()
*/

bool TQImageIO::write()
{
    if ( frmt.isEmpty() )
	return false;
    TQImageHandler *h = get_image_handler( frmt );
    if ( !h && !plugin_manager) {
	tqt_init_image_plugins();
	h = get_image_handler( frmt );
    }
    if ( !h || !h->write_image ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQImageIO::write: No such image format handler: %s",
		 format() );
#endif
	return false;
    }
    TQFile file;
    if ( !iodev && !fname.isEmpty() ) {
	file.setName( fname );
	bool translate = h->text_mode==TQImageHandler::TranslateInOut;
	int fmode = translate ? IO_WriteOnly|IO_Translate : IO_WriteOnly;
	if ( !file.open(fmode) )		// couldn't create file
	    return false;
	iodev = &file;
    }
    iostat = 1;
    (*h->write_image)( this );
    if ( file.isOpen() ) {			// image was written using file
	file.close();
	iodev = 0;
    }
    return iostat == 0;				// image successfully written?
}
#endif //TQT_NO_IMAGEIO

#ifndef TQT_NO_IMAGEIO_BMP

/*****************************************************************************
  BMP (DIB) image read/write functions
 *****************************************************************************/

const int BMP_FILEHDR_SIZE = 14;		// size of BMP_FILEHDR data

struct BMP_FILEHDR {				// BMP file header
    char   bfType[2];				// "BM"
    TQ_INT32  bfSize;				// size of file
    TQ_INT16  bfReserved1;
    TQ_INT16  bfReserved2;
    TQ_INT32  bfOffBits;				// pointer to the pixmap bits
};

TQDataStream &operator>>( TQDataStream &s, BMP_FILEHDR &bf )
{						// read file header
    s.readRawBytes( bf.bfType, 2 );
    s >> bf.bfSize >> bf.bfReserved1 >> bf.bfReserved2 >> bf.bfOffBits;
    return s;
}

TQDataStream &operator<<( TQDataStream &s, const BMP_FILEHDR &bf )
{						// write file header
    s.writeRawBytes( bf.bfType, 2 );
    s << bf.bfSize << bf.bfReserved1 << bf.bfReserved2 << bf.bfOffBits;
    return s;
}


const int BMP_OLD  = 12;			// old Windows/OS2 BMP size
const int BMP_WIN  = 40;			// new Windows BMP size
const int BMP_OS2  = 64;			// new OS/2 BMP size

const int BMP_RGB  = 0;				// no compression
const int BMP_RLE8 = 1;				// run-length encoded, 8 bits
const int BMP_RLE4 = 2;				// run-length encoded, 4 bits
const int BMP_BITFIELDS = 3;			// RGB values encoded in data as bit-fields

struct BMP_INFOHDR {				// BMP information header
    TQ_INT32  biSize;				// size of this struct
    TQ_INT32  biWidth;				// pixmap width
    TQ_INT32  biHeight;				// pixmap height
    TQ_INT16  biPlanes;				// should be 1
    TQ_INT16  biBitCount;			// number of bits per pixel
    TQ_INT32  biCompression;			// compression method
    TQ_INT32  biSizeImage;				// size of image
    TQ_INT32  biXPelsPerMeter;			// horizontal resolution
    TQ_INT32  biYPelsPerMeter;			// vertical resolution
    TQ_INT32  biClrUsed;				// number of colors used
    TQ_INT32  biClrImportant;			// number of important colors
};


TQDataStream &operator>>( TQDataStream &s, BMP_INFOHDR &bi )
{
    s >> bi.biSize;
    if ( bi.biSize == BMP_WIN || bi.biSize == BMP_OS2 ) {
	s >> bi.biWidth >> bi.biHeight >> bi.biPlanes >> bi.biBitCount;
	s >> bi.biCompression >> bi.biSizeImage;
	s >> bi.biXPelsPerMeter >> bi.biYPelsPerMeter;
	s >> bi.biClrUsed >> bi.biClrImportant;
    }
    else {					// probably old Windows format
	TQ_INT16 w, h;
	s >> w >> h >> bi.biPlanes >> bi.biBitCount;
	bi.biWidth  = w;
	bi.biHeight = h;
	bi.biCompression = BMP_RGB;		// no compression
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = bi.biYPelsPerMeter = 0;
	bi.biClrUsed = bi.biClrImportant = 0;
    }
    return s;
}

TQDataStream &operator<<( TQDataStream &s, const BMP_INFOHDR &bi )
{
    s << bi.biSize;
    s << bi.biWidth << bi.biHeight;
    s << bi.biPlanes;
    s << bi.biBitCount;
    s << bi.biCompression;
    s << bi.biSizeImage;
    s << bi.biXPelsPerMeter << bi.biYPelsPerMeter;
    s << bi.biClrUsed << bi.biClrImportant;
    return s;
}

static
int calc_shift(int mask)
{
    int result = 0;
    while (!(mask & 1)) {
	result++;
	mask >>= 1;
    }
    return result;
}

static
bool read_dib( TQDataStream& s, int offset, int startpos, TQImage& image )
{
    BMP_INFOHDR bi;
    TQIODevice* d = s.device();

    s >> bi;					// read BMP info header
    if ( d->atEnd() )				// end of stream/file
	return false;
#if 0
    tqDebug( "offset...........%d", offset );
    tqDebug( "startpos.........%d", startpos );
    tqDebug( "biSize...........%d", bi.biSize );
    tqDebug( "biWidth..........%d", bi.biWidth );
    tqDebug( "biHeight.........%d", bi.biHeight );
    tqDebug( "biPlanes.........%d", bi.biPlanes );
    tqDebug( "biBitCount.......%d", bi.biBitCount );
    tqDebug( "biCompression....%d", bi.biCompression );
    tqDebug( "biSizeImage......%d", bi.biSizeImage );
    tqDebug( "biXPelsPerMeter..%d", bi.biXPelsPerMeter );
    tqDebug( "biYPelsPerMeter..%d", bi.biYPelsPerMeter );
    tqDebug( "biClrUsed........%d", bi.biClrUsed );
    tqDebug( "biClrImportant...%d", bi.biClrImportant );
#endif
    int w = bi.biWidth,	 h = bi.biHeight,  nbits = bi.biBitCount;
    int t = bi.biSize,	 comp = bi.biCompression;
    int red_mask, green_mask, blue_mask;
    int red_shift = 0;
    int green_shift = 0;
    int blue_shift = 0;
    int red_scale = 0;
    int green_scale = 0;
    int blue_scale = 0;

    if ( !(nbits == 1 || nbits == 4 || nbits == 8 || nbits == 16 || nbits == 24 || nbits == 32) ||
	bi.biPlanes != 1 || comp > BMP_BITFIELDS )
	return false;					// weird BMP image
    if ( !(comp == BMP_RGB || (nbits == 4 && comp == BMP_RLE4) ||
	(nbits == 8 && comp == BMP_RLE8) || ((nbits == 16 || nbits == 32) && comp == BMP_BITFIELDS)) )
	 return false;				// weird compression type
    if ((w < 0) || ((w * abs(h)) > (16384 * 16384)))
        return false;

    int ncols;
    int depth;
    switch ( nbits ) {
	case 32:
	case 24:
	case 16:
	    depth = 32;
	    break;
	case 8:
	case 4:
	    depth = 8;
	    break;
	default:
	    depth = 1;
    }
    if ( depth == 32 )				// there's no colormap
	ncols = 0;
    else					// # colors used
	ncols = bi.biClrUsed ? bi.biClrUsed : 1 << nbits;

    image.create( w, h, depth, ncols, nbits == 1 ?
		  TQImage::BigEndian : TQImage::IgnoreEndian );
    if ( image.isNull() )			// could not create image
	return false;

    image.setDotsPerMeterX( bi.biXPelsPerMeter );
    image.setDotsPerMeterY( bi.biYPelsPerMeter );

    d->at( startpos + BMP_FILEHDR_SIZE + bi.biSize ); // goto start of colormap

    if ( ncols > 0 ) {				// read color table
	uchar rgb[4];
	int   rgb_len = t == BMP_OLD ? 3 : 4;
	for ( int i=0; i<ncols; i++ ) {
	    if ( d->readBlock( (char *)rgb, rgb_len ) != rgb_len )
		return false;
	    image.setColor( i, tqRgb(rgb[2],rgb[1],rgb[0]) );
	    if ( d->atEnd() )			// truncated file
		return false;
	}
    } else if (comp == BMP_BITFIELDS && (nbits == 16 || nbits == 32)) {
	if ( (TQ_ULONG)d->readBlock( (char *)&red_mask, sizeof(red_mask) ) != sizeof(red_mask) )
	    return false;
	if ( (TQ_ULONG)d->readBlock( (char *)&green_mask, sizeof(green_mask) ) != sizeof(green_mask) )
	    return false;
	if ( (TQ_ULONG)d->readBlock( (char *)&blue_mask, sizeof(blue_mask) ) != sizeof(blue_mask) )
	    return false;
	red_shift = calc_shift(red_mask);
	if (((red_mask >> red_shift) + 1) == 0)
	    return false;
	red_scale = 256 / ((red_mask >> red_shift) + 1);
	green_shift = calc_shift(green_mask);
	if (((green_mask >> green_shift) + 1) == 0)
	    return false;
	green_scale = 256 / ((green_mask >> green_shift) + 1);
	blue_shift = calc_shift(blue_mask);
	if (((blue_mask >> blue_shift) + 1) == 0)
	    return false;
	blue_scale = 256 / ((blue_mask >> blue_shift) + 1);
    } else if (comp == BMP_RGB && (nbits == 24 || nbits == 32)) {
	blue_mask = 0x000000ff;
	green_mask = 0x0000ff00;
	red_mask = 0x00ff0000;
	blue_shift = 0;
	green_shift = 8;
	red_shift = 16;
	blue_scale = green_scale = red_scale = 1;
    } else if (comp == BMP_RGB && nbits == 16)  // don't support RGB values for 15/16 bpp
	return false;

    // offset can be bogus, be careful
    if (offset>=0 && startpos + offset > (TQ_LONG)d->at() )
	d->at( startpos + offset );		// start of image data

    int	     bpl = image.bytesPerLine();
#ifdef TQ_WS_QWS
    //
    // Guess the number of bytes-per-line if we don't know how much
    // image data is in the file (bogus image ?).
    //
    int bmpbpl = bi.biSizeImage > 0 ?
	bi.biSizeImage / bi.biHeight :
	(d->size() - offset) / bi.biHeight;
    int pad = bmpbpl-bpl;
#endif
    uchar **line = image.jumpTable();

    if ( nbits == 1 ) {				// 1 bit BMP image
	while ( --h >= 0 ) {
	    if ( d->readBlock((char*)line[h],bpl) != bpl )
		break;
#ifdef TQ_WS_QWS
	    if ( pad > 0 )
		d->at(d->at()+pad);
#endif
	}
	if ( ncols == 2 && tqGray(image.color(0)) < tqGray(image.color(1)) )
	    swapPixel01( &image );		// pixel 0 is white!
    }

    else if ( nbits == 4 ) {			// 4 bit BMP image
	int    buflen = ((w+7)/8)*4;
	uchar *buf    = new uchar[buflen];
	TQ_CHECK_PTR( buf );
	if ( comp == BMP_RLE4 ) {		// run length compression
	    int x=0, y=0, b, c, i;
	    uchar *p = line[h-1];
	    uchar *endp = line[h-1]+w;
	    while ( y < h ) {
		if ( (b=d->getch()) == EOF )
		    break;
		if ( b == 0 ) {			// escape code
		    switch ( (b=d->getch()) ) {
			case 0:			// end of line
			    x = 0;
			    y++;
			    p = line[h-y-1];
			    break;
			case 1:			// end of image
			case EOF:		// end of file
			    y = h;		// exit loop
			    break;
			case 2:			// delta (jump)
			    x += d->getch();
			    y += d->getch();

			    // Protection
			    if ( (uint)x >= (uint)w )
				x = w-1;
			    if ( (uint)y >= (uint)h )
				y = h-1;

			    p = line[h-y-1] + x;
			    break;
			default:		// absolute mode
			    // Protection
			    if ( p + b > endp )
				b = endp-p;

			    i = (c = b)/2;
			    while ( i-- ) {
				b = d->getch();
				*p++ = b >> 4;
				*p++ = b & 0x0f;
			    }
			    if ( c & 1 )
				*p++ = d->getch() >> 4;
			    if ( (((c & 3) + 1) & 2) == 2 )
				d->getch();	// align on word boundary
			    x += c;
		    }
		} else {			// encoded mode
		    // Protection
		    if ( p + b > endp )
			b = endp-p;

		    i = (c = b)/2;
		    b = d->getch();		// 2 pixels to be repeated
		    while ( i-- ) {
			*p++ = b >> 4;
			*p++ = b & 0x0f;
		    }
		    if ( c & 1 )
			*p++ = b >> 4;
		    x += c;
		}
	    }
	} else if ( comp == BMP_RGB ) {		// no compression
	    while ( --h >= 0 ) {
		if ( d->readBlock((char*)buf,buflen) != buflen )
		    break;
		uchar *p = line[h];
		uchar *b = buf;
		for ( int i=0; i<w/2; i++ ) {	// convert nibbles to bytes
		    *p++ = *b >> 4;
		    *p++ = *b++ & 0x0f;
		}
		if ( w & 1 )			// the last nibble
		    *p = *b >> 4;
	    }
	}
	delete [] buf;
    }

    else if ( nbits == 8 ) {			// 8 bit BMP image
	if ( comp == BMP_RLE8 ) {		// run length compression
	    int x=0, y=0, b;
	    uchar *p = line[h-1];
	    const uchar *endp = line[h-1]+w;
	    while ( y < h ) {
		if ( (b=d->getch()) == EOF )
		    break;
		if ( b == 0 ) {			// escape code
		    switch ( (b=d->getch()) ) {
			case 0:			// end of line
			    x = 0;
			    y++;
			    p = line[h-y-1];
			    break;
			case 1:			// end of image
			case EOF:		// end of file
			    y = h;		// exit loop
			    break;
			case 2:			// delta (jump)
			    x += d->getch();
			    y += d->getch();

			    // Protection
			    if ( (uint)x >= (uint)w )
				x = w-1;
			    if ( (uint)y >= (uint)h )
				y = h-1;

			    p = line[h-y-1] + x;
			    break;
			default:		// absolute mode
			    // Protection
			    if ( p + b > endp )
				b = endp-p;

			    if ( d->readBlock( (char *)p, b ) != b )
				return false;
			    if ( (b & 1) == 1 )
				d->getch();	// align on word boundary
			    x += b;
			    p += b;
		    }
		} else {			// encoded mode
		    // Protection
		    if ( p + b > endp )
			b = endp-p;

		    memset( p, d->getch(), b ); // repeat pixel
		    x += b;
		    p += b;
		}
	    }
	} else if ( comp == BMP_RGB ) {		// uncompressed
	    while ( --h >= 0 ) {
		if ( d->readBlock((char *)line[h],bpl) != bpl )
		    break;
#ifdef TQ_WS_QWS
		if ( pad > 0 )
		    d->at(d->at()+pad);
#endif
	    }
	}
    }

    else if ( nbits == 16 || nbits == 24 || nbits == 32 ) { // 16,24,32 bit BMP image
	TQRgb *p;
	TQRgb  *end;
	uchar *buf24 = new uchar[bpl];
	int    bpl24 = ((w*nbits+31)/32)*4;
	uchar *b;
	int c;

	while ( --h >= 0 ) {
	    p = (TQRgb *)line[h];
	    end = p + w;
	    if ( d->readBlock( (char *)buf24,bpl24) != bpl24 )
		break;
	    b = buf24;
	    while ( p < end ) {
		c = *(uchar*)b | (*(uchar*)(b+1)<<8);
		if (nbits != 16)
		    c |= *(uchar*)(b+2)<<16;
		*p++ = tqRgb(((c & red_mask) >> red_shift) * red_scale,
					((c & green_mask) >> green_shift) * green_scale,
					((c & blue_mask) >> blue_shift) * blue_scale);
		b += nbits/8;
	    }
	}
	delete[] buf24;
    }

    return true;
}

bool tqt_read_dib( TQDataStream& s, TQImage& image )
{
    return read_dib(s,-1,-BMP_FILEHDR_SIZE,image);
}


static void read_bmp_image( TQImageIO *iio )
{
    TQIODevice  *d = iio->ioDevice();
    TQDataStream s( d );
    BMP_FILEHDR bf;
    int		startpos = d->at();

    s.setByteOrder( TQDataStream::LittleEndian );// Intel byte order
    s >> bf;					// read BMP file header

    if ( tqstrncmp(bf.bfType,"BM",2) != 0 )	// not a BMP image
	return;

    TQImage image;
    if (read_dib( s, bf.bfOffBits, startpos, image )) {
	iio->setImage( image );
	iio->setStatus( 0 );			// image ok
    }
}

bool tqt_write_dib( TQDataStream& s, TQImage image )
{
    int	nbits;
    int	bpl_bmp;
    int	bpl = image.bytesPerLine();

    TQIODevice* d = s.device();

    if ( image.depth() == 8 && image.numColors() <= 16 ) {
	bpl_bmp = (((bpl+1)/2+3)/4)*4;
	nbits = 4;
    } else if ( image.depth() == 32 ) {
	bpl_bmp = ((image.width()*24+31)/32)*4;
	nbits = 24;
#ifdef TQ_WS_QWS
    } else if ( image.depth() == 1 || image.depth() == 8 ) {
	// TQt/E doesn't word align.
	bpl_bmp = ((image.width()*image.depth()+31)/32)*4;
	nbits = image.depth();
#endif
    } else {
	bpl_bmp = bpl;
	nbits = image.depth();
    }

    BMP_INFOHDR bi;
    bi.biSize	       = BMP_WIN;		// build info header
    bi.biWidth	       = image.width();
    bi.biHeight	       = image.height();
    bi.biPlanes	       = 1;
    bi.biBitCount      = nbits;
    bi.biCompression   = BMP_RGB;
    bi.biSizeImage     = bpl_bmp*image.height();
    bi.biXPelsPerMeter = image.dotsPerMeterX() ? image.dotsPerMeterX()
						: 2834; // 72 dpi default
    bi.biYPelsPerMeter = image.dotsPerMeterY() ? image.dotsPerMeterY() : 2834;
    bi.biClrUsed       = image.numColors();
    bi.biClrImportant  = image.numColors();
    s << bi;					// write info header

    if ( image.depth() != 32 ) {		// write color table
	uchar *color_table = new uchar[4*image.numColors()];
	uchar *rgb = color_table;
	TQRgb *c = image.colorTable();
	for ( int i=0; i<image.numColors(); i++ ) {
	    *rgb++ = tqBlue ( c[i] );
	    *rgb++ = tqGreen( c[i] );
	    *rgb++ = tqRed  ( c[i] );
	    *rgb++ = 0;
	}
	d->writeBlock( (char *)color_table, 4*image.numColors() );
	delete [] color_table;
    }

    if ( image.depth() == 1 && image.bitOrder() != TQImage::BigEndian )
	image = image.convertBitOrder( TQImage::BigEndian );

    int	 y;

    if ( nbits == 1 || nbits == 8 ) {		// direct output
#ifdef TQ_WS_QWS
	// TQt/E doesn't word align.
	int pad = bpl_bmp - bpl;
	char padding[4];
#endif
	for ( y=image.height()-1; y>=0; y-- ) {
	    d->writeBlock( (char*)image.scanLine(y), bpl );
#ifdef TQ_WS_QWS
	    d->writeBlock( padding, pad );
#endif
	}
	return true;
    }

    uchar *buf	= new uchar[bpl_bmp];
    uchar *b, *end;
    uchar *p;

    memset( buf, 0, bpl_bmp );
    for ( y=image.height()-1; y>=0; y-- ) {	// write the image bits
	if ( nbits == 4 ) {			// convert 8 -> 4 bits
	    p = image.scanLine(y);
	    b = buf;
	    end = b + image.width()/2;
	    while ( b < end ) {
		*b++ = (*p << 4) | (*(p+1) & 0x0f);
		p += 2;
	    }
	    if ( image.width() & 1 )
		*b = *p << 4;
	} else {				// 32 bits
	    TQRgb *p   = (TQRgb *)image.scanLine( y );
	    TQRgb *end = p + image.width();
	    b = buf;
	    while ( p < end ) {
		*b++ = tqBlue(*p);
		*b++ = tqGreen(*p);
		*b++ = tqRed(*p);
		p++;
	    }
	}
	if ( bpl_bmp != d->writeBlock( (char*)buf, bpl_bmp ) ) {
	    delete[] buf;
	    return false;
	}
    }
    delete[] buf;
    return true;
}


static void write_bmp_image( TQImageIO *iio )
{
    TQIODevice  *d = iio->ioDevice();
    TQImage	image = iio->image();
    TQDataStream s( d );
    BMP_FILEHDR bf;
    int		bpl_bmp;
    int		bpl = image.bytesPerLine();

    // Code partially repeated in tqt_write_dib
    if ( image.depth() == 8 && image.numColors() <= 16 ) {
	bpl_bmp = (((bpl+1)/2+3)/4)*4;
    } else if ( image.depth() == 32 ) {
	bpl_bmp = ((image.width()*24+31)/32)*4;
    } else {
	bpl_bmp = bpl;
    }

    iio->setStatus( 0 );
    s.setByteOrder( TQDataStream::LittleEndian );// Intel byte order
    strncpy( bf.bfType, "BM", 2 );		// build file header
    bf.bfReserved1 = bf.bfReserved2 = 0;	// reserved, should be zero
    bf.bfOffBits   = BMP_FILEHDR_SIZE + BMP_WIN + image.numColors()*4;
    bf.bfSize	   = bf.bfOffBits + bpl_bmp*image.height();
    s << bf;					// write file header

    if ( !tqt_write_dib( s, image ) )
	iio->setStatus( 1 );

}

#endif // TQT_NO_IMAGEIO_BMP

#ifndef TQT_NO_IMAGEIO_PPM

/*****************************************************************************
  PBM/PGM/PPM (ASCII and RAW) image read/write functions
 *****************************************************************************/

static int read_pbm_int( TQIODevice *d )
{
    int	  c;
    int	  val = -1;
    bool  digit;
    const int buflen = 100;
    char  buf[buflen];
    for ( ;; ) {
	if ( (c=d->getch()) == EOF )		// end of file
	    break;
	digit = isdigit( (uchar) c );
	if ( val != -1 ) {
	    if ( digit ) {
		val = 10*val + c - '0';
		continue;
	    } else {
		if ( c == '#' )			// comment
		    d->readLine( buf, buflen );
		break;
	    }
	}
	if ( digit )				// first digit
	    val = c - '0';
	else if ( isspace((uchar) c) )
	    continue;
	else if ( c == '#' )
	    d->readLine( buf, buflen );
	else
	    break;
    }
    return val;
}

static void read_pbm_image( TQImageIO *iio )	// read PBM image data
{
    const int	buflen = 300;
    char	buf[buflen];
    TQIODevice  *d = iio->ioDevice();
    int		w, h, nbits, mcc, y;
    int		pbm_bpl;
    char	type;
    bool	raw;
    TQImage	image;

    if ( d->readBlock( buf, 3 ) != 3 )			// read P[1-6]<white-space>
	return;
    if ( !(buf[0] == 'P' && isdigit((uchar) buf[1]) && isspace((uchar) buf[2])) )
	return;
    switch ( (type=buf[1]) ) {
	case '1':				// ascii PBM
	case '4':				// raw PBM
	    nbits = 1;
	    break;
	case '2':				// ascii PGM
	case '5':				// raw PGM
	    nbits = 8;
	    break;
	case '3':				// ascii PPM
	case '6':				// raw PPM
	    nbits = 32;
	    break;
	default:
	    return;
    }
    raw = type >= '4';
    w = read_pbm_int( d );			// get image width
    h = read_pbm_int( d );			// get image height
    if ( nbits == 1 )
	mcc = 1;				// ignore max color component
    else
	mcc = read_pbm_int( d );		// get max color component
    if ( w <= 0 || w > 32767 || h <= 0 || h > 32767 || mcc <= 0 || mcc > 0xffff )
	return;					// weird P.M image

    int maxc = mcc;
    if ( maxc > 255 )
	maxc = 255;
    image.create( w, h, nbits, 0,
		  nbits == 1 ? TQImage::BigEndian :  TQImage::IgnoreEndian );
    if ( image.isNull() )
	return;

    pbm_bpl = (nbits*w+7)/8;			// bytes per scanline in PBM

    if ( raw ) {				// read raw data
	if ( nbits == 32 ) {			// type 6
	    pbm_bpl = 3*w;
	    uchar *buf24 = new uchar[pbm_bpl], *b;
	    TQRgb  *p;
	    TQRgb  *end;
	    for ( y=0; y<h; y++ ) {
		if ( d->readBlock( (char *)buf24, pbm_bpl ) != pbm_bpl ) {
		    delete[] buf24;
		    return;
		}
		p = (TQRgb *)image.scanLine( y );
		end = p + w;
		b = buf24;
		while ( p < end ) {
		    *p++ = tqRgb(b[0],b[1],b[2]);
		    b += 3;
		}
	    }
	    delete[] buf24;
	} else {				// type 4,5
	    for ( y=0; y<h; y++ ) {
		if ( d->readBlock( (char *)image.scanLine(y), pbm_bpl )
			!= pbm_bpl )
		    return;
	    }
	}
    } else {					// read ascii data
	uchar *p;
	int n;
	for ( y=0; y<h; y++ ) {
	    p = image.scanLine( y );
	    n = pbm_bpl;
	    if ( nbits == 1 ) {
		int b;
		while ( n-- ) {
		    b = 0;
		    for ( int i=0; i<8; i++ )
			b = (b << 1) | (read_pbm_int(d) & 1);
		    *p++ = b;
		}
	    } else if ( nbits == 8 ) {
		if ( mcc == maxc ) {
		    while ( n-- ) {
			*p++ = read_pbm_int( d );
		    }
		} else {
		    while ( n-- ) {
			*p++ = read_pbm_int( d ) * maxc / mcc;
		    }
		}
	    } else {				// 32 bits
		n /= 4;
		int r, g, b;
		if ( mcc == maxc ) {
		    while ( n-- ) {
			r = read_pbm_int( d );
			g = read_pbm_int( d );
			b = read_pbm_int( d );
			*((TQRgb*)p) = tqRgb( r, g, b );
			p += 4;
		    }
		} else {
		    while ( n-- ) {
			r = read_pbm_int( d ) * maxc / mcc;
			g = read_pbm_int( d ) * maxc / mcc;
			b = read_pbm_int( d ) * maxc / mcc;
			*((TQRgb*)p) = tqRgb( r, g, b );
			p += 4;
		    }
		}
	    }
	}
    }

    if ( nbits == 1 ) {				// bitmap
	image.setNumColors( 2 );
	image.setColor( 0, tqRgb(255,255,255) ); // white
	image.setColor( 1, tqRgb(0,0,0) );	// black
    } else if ( nbits == 8 ) {			// graymap
	image.setNumColors( maxc+1 );
	for ( int i=0; i<=maxc; i++ )
	    image.setColor( i, tqRgb(i*255/maxc,i*255/maxc,i*255/maxc) );
    }

    iio->setImage( image );
    iio->setStatus( 0 );			// image ok
}


static void write_pbm_image( TQImageIO *iio )
{
    TQIODevice* out = iio->ioDevice();
    TQCString str;

    TQImage  image  = iio->image();
    TQCString format = iio->format();
    format = format.left(3);			// ignore RAW part
    bool gray = format == "PGM";

    if ( format == "PBM" ) {
	image = image.convertDepth(1);
    } else if ( image.depth() == 1 ) {
	image = image.convertDepth(8);
    }

    if ( image.depth() == 1 && image.numColors() == 2 ) {
	if ( tqGray(image.color(0)) < tqGray(image.color(1)) ) {
	    // 0=dark/black, 1=light/white - invert
	    image.detach();
	    for ( int y=0; y<image.height(); y++ ) {
		uchar *p = image.scanLine(y);
		uchar *end = p + image.bytesPerLine();
		while ( p < end )
		    *p++ ^= 0xff;
	    }
	}
    }

    uint w = image.width();
    uint h = image.height();

    str.sprintf("P\n%d %d\n", w, h);

    switch (image.depth()) {
	case 1: {
	    str.insert(1, '4');
	    if ((uint)out->writeBlock(str, str.length()) != str.length()) {
		iio->setStatus(1);
		return;
	    }
	    w = (w+7)/8;
	    for (uint y=0; y<h; y++) {
		uchar* line = image.scanLine(y);
		if ( w != (uint)out->writeBlock((char*)line, w) ) {
		    iio->setStatus(1);
		    return;
		}
	    }
	    }
	    break;

	case 8: {
	    str.insert(1, gray ? '5' : '6');
	    str.append("255\n");
	    if ((uint)out->writeBlock(str, str.length()) != str.length()) {
		iio->setStatus(1);
		return;
	    }
	    TQRgb  *color = image.colorTable();
	    uint bpl = w*(gray ? 1 : 3);
	    uchar *buf   = new uchar[bpl];
	    for (uint y=0; y<h; y++) {
		uchar *b = image.scanLine(y);
		uchar *p = buf;
		uchar *end = buf+bpl;
		if ( gray ) {
		    while ( p < end ) {
			uchar g = (uchar)tqGray(color[*b++]);
			*p++ = g;
		    }
		} else {
		    while ( p < end ) {
			TQRgb rgb = color[*b++];
			*p++ = tqRed(rgb);
			*p++ = tqGreen(rgb);
			*p++ = tqBlue(rgb);
		    }
		}
		if ( bpl != (uint)out->writeBlock((char*)buf, bpl) ) {
		    iio->setStatus(1);
		    return;
		}
	    }
	    delete [] buf;
	    }
	    break;

	case 32: {
	    str.insert(1, gray ? '5' : '6');
	    str.append("255\n");
	    if ((uint)out->writeBlock(str, str.length()) != str.length()) {
		iio->setStatus(1);
		return;
	    }
	    uint bpl = w*(gray ? 1 : 3);
	    uchar *buf = new uchar[bpl];
	    for (uint y=0; y<h; y++) {
		TQRgb  *b = (TQRgb*)image.scanLine(y);
		uchar *p = buf;
		uchar *end = buf+bpl;
		if ( gray ) {
		    while ( p < end ) {
			uchar g = (uchar)tqGray(*b++);
			*p++ = g;
		    }
		} else {
		    while ( p < end ) {
			TQRgb rgb = *b++;
			*p++ = tqRed(rgb);
			*p++ = tqGreen(rgb);
			*p++ = tqBlue(rgb);
		    }
		}
		if ( bpl != (uint)out->writeBlock((char*)buf, bpl) ) {
		    iio->setStatus(1);
		    return;
		}
	    }
	    delete [] buf;
	    }
    }

    iio->setStatus(0);
}

#endif // TQT_NO_IMAGEIO_PPM

#ifndef TQT_NO_ASYNC_IMAGE_IO

class TQImageIOFrameGrabber : public TQImageConsumer {
public:
    TQImageIOFrameGrabber() : framecount(0) { }
    virtual ~TQImageIOFrameGrabber() { }

    TQImageDecoder *decoder;
    int framecount;

    void changed(const TQRect&) { }
    void end() { }
    void frameDone(const TQPoint&, const TQRect&) { framecount++; }
    void frameDone() { framecount++; }
    void setLooping(int) { }
    void setFramePeriod(int) { }
    void setSize(int, int) { }
};

static void read_async_image( TQImageIO *iio )
{
    const int buf_len = 2048;
    uchar buffer[buf_len];
    TQIODevice  *d = iio->ioDevice();
    TQImageIOFrameGrabber* consumer = new TQImageIOFrameGrabber();
    TQImageDecoder *decoder = new TQImageDecoder(consumer);
    consumer->decoder = decoder;
    int startAt = d->at();
    int totLen = 0;

    for (;;) {
	int length = d->readBlock((char*)buffer, buf_len);
	if ( length <= 0 ) {
	    iio->setStatus(length);
	    break;
	}
	uchar* b = buffer;
	int r = -1;
	while (length > 0 && consumer->framecount==0) {
	    r = decoder->decode(b, length);
	    if ( r <= 0 ) break;
	    b += r;
	    totLen += r;
	    length -= r;
	}
	if ( consumer->framecount ) {
	    // Stopped after first frame
	    if ( d->isDirectAccess() )
		d->at( startAt + totLen );
	    else {
		// ### We have (probably) read too much from the stream into
		// the buffer, and there is no way to put it back!
	    }
	    iio->setImage(decoder->image());
	    iio->setStatus(0);
	    break;
	}
	if ( r <= 0 ) {
	    iio->setStatus(r);
	    break;
	}
    }

    consumer->decoder = 0;
    delete decoder;
    delete consumer;
}

#endif // TQT_NO_ASYNC_IMAGE_IO

#ifndef TQT_NO_IMAGEIO_XBM

/*****************************************************************************
  X bitmap image read/write functions
 *****************************************************************************/

static inline int hex2byte( char *p )
{
    return ( (isdigit((uchar) *p) ? *p - '0' : toupper((uchar) *p) - 'A' + 10) << 4 ) |
	   ( isdigit((uchar) *(p+1)) ? *(p+1) - '0' : toupper((uchar) *(p+1)) - 'A' + 10 );
}

static void read_xbm_image( TQImageIO *iio )
{
    const int	buflen = 300;
    const int	maxlen = 4096;
    char	buf[buflen];
    TQRegExp	r1, r2;
    TQIODevice  *d = iio->ioDevice();
    int		w=-1, h=-1;
    TQImage	image;
    TQ_INT64	readBytes = 0;
    TQ_INT64	totalReadBytes = 0;

    r1 = TQString::fromLatin1("^#define[ \t]+[a-zA-Z0-9._]+[ \t]+");
    r2 = TQString::fromLatin1("[0-9]+");

    buf[0] = '\0';
    while (buf[0] != '#') { //skip leading comment, if any
        readBytes = d->readLine(buf, buflen);

        // if readBytes >= buflen, it's very probably not a C file
        if ((readBytes <= 0) || (readBytes >= (buflen-1)))
            return;

        // limit xbm headers to the first 4k in the file to prevent
        // excessive reads on non-xbm files
        totalReadBytes += readBytes;
        if (totalReadBytes >= maxlen)
            return;
    }

    TQString sbuf;
    sbuf = TQString::fromLatin1(buf);

   if ( r1.search(sbuf) == 0 &&
	 r2.search(sbuf, r1.matchedLength()) == r1.matchedLength() )
	w = atoi( &buf[r1.matchedLength()] );

    readBytes = d->readLine(buf, buflen );		// "#define .._height <num>"
    if (readBytes <= 0) {
        return;
    }
    sbuf = TQString::fromLatin1(buf);

    if ( r1.search(sbuf) == 0 &&
	 r2.search(sbuf, r1.matchedLength()) == r1.matchedLength() )
	h = atoi( &buf[r1.matchedLength()] );

    if ( w <= 0 || w > 32767 || h <= 0 || h > 32767 )
	return;					// format error

    for ( ;; ) {				// scan for data
	readBytes = d->readLine(buf, buflen);
	if (readBytes <= 0) {	// end of file
	    return;
	}
	buf[readBytes] = '\0';
	if ( strstr(buf,"0x") != 0 )		// does line contain data?
	    break;
    }

    image.create( w, h, 1, 2, TQImage::LittleEndian );
    if ( image.isNull() )
	return;

    image.setColor( 0, tqRgb(255,255,255) );	// white
    image.setColor( 1, tqRgb(0,0,0) );		// black

    int	   x = 0, y = 0;
    uchar *b = image.scanLine(0);
    char  *p = strstr( buf, "0x" );
    w = (w+7)/8;				// byte width

    while ( y < h ) {				// for all encoded bytes...
	if (p && (p < (buf + readBytes - 3))) {      // p = "0x.."
	    if (!isxdigit(p[2]) || !isxdigit(p[3])) {
		return;
	    }
	    *b++ = hex2byte(p+2);
	    p += 2;
	    if ( ++x == w && ++y < h ) {
		b = image.scanLine(y);
		x = 0;
	    }
	    p = strstr( p, "0x" );
	} else {				// read another line
	    readBytes = d->readLine(buf, buflen);
	    if (readBytes <= 0)	// EOF ==> truncated image
		break;
	    buf[readBytes] = '\0';
	    p = strstr( buf, "0x" );
	}
    }

    iio->setImage( image );
    iio->setStatus( 0 );			// image ok
}


static void write_xbm_image( TQImageIO *iio )
{
    TQIODevice *d = iio->ioDevice();
    TQImage     image = iio->image();
    int	       w = image.width();
    int	       h = image.height();
    int	       i;
    TQString    s = fbname(iio->fileName());	// get file base name
    char *buf = new char[s.length() + 100];

    sprintf( buf, "#define %s_width %d\n", s.ascii(), w );
    d->writeBlock( buf, tqstrlen(buf) );
    sprintf( buf, "#define %s_height %d\n", s.ascii(), h );
    d->writeBlock( buf, tqstrlen(buf) );
    sprintf( buf, "static char %s_bits[] = {\n ", s.ascii() );
    d->writeBlock( buf, tqstrlen(buf) );

    iio->setStatus( 0 );

    if ( image.depth() != 1 )
	image = image.convertDepth( 1 );	// dither
    if ( image.bitOrder() != TQImage::LittleEndian )
        image = image.convertBitOrder( TQImage::LittleEndian );

    bool invert = tqGray(image.color(0)) < tqGray(image.color(1));
    char hexrep[16];
    for ( i=0; i<10; i++ )
	hexrep[i] = '0' + i;
    for ( i=10; i<16; i++ )
	hexrep[i] = 'a' -10 + i;
    if ( invert ) {
	char t;
	for ( i=0; i<8; i++ ) {
	    t = hexrep[15-i];
	    hexrep[15-i] = hexrep[i];
	    hexrep[i] = t;
	}
    }
    int bcnt = 0;
    char *p = buf;
    int bpl = (w+7)/8;
    for (int y = 0; y < h; ++y) {
        uchar *b = image.scanLine(y);
        for (i = 0; i < bpl; ++i) {
            *p++ = '0'; *p++ = 'x';
            *p++ = hexrep[*b >> 4];
            *p++ = hexrep[*b++ & 0xf];

            if ( i < bpl - 1 || y < h - 1 ) {
                *p++ = ',';
                if ( ++bcnt > 14 ) {
                    *p++ = '\n';
                    *p++ = ' ';
                    *p   = '\0';
                    if ( (int)tqstrlen(buf) != d->writeBlock( buf, tqstrlen(buf) ) ) {
                        iio->setStatus( 1 );
                        delete [] buf;
                        return;
                    }
                    p = buf;
                    bcnt = 0;
                }
            }
        }
    }
    strcpy( p, " };\n" );
    if ( (int)tqstrlen(buf) != d->writeBlock( buf, tqstrlen(buf) ) )
	iio->setStatus( 1 );
    delete [] buf;
}

#endif // TQT_NO_IMAGEIO_XBM


#ifndef TQT_NO_IMAGEIO_XPM

/*****************************************************************************
  XPM image read/write functions
 *****************************************************************************/


// Skip until ", read until the next ", return the rest in *buf
// Returns false on error, true on success

static bool read_xpm_string( TQCString &buf, TQIODevice *d,
			     const char * const *source, int &index )
{
    if ( source ) {
	buf = source[index++];
	return true;
    }

    if ( buf.size() < 69 )	    //# just an approximation
	buf.resize( 123 );

    buf[0] = '\0';
    int c;
    int i;
    while ( (c=d->getch()) != EOF && c != '"' ) { }
    if ( c == EOF ) {
	return false;
    }
    i = 0;
    while ( (c=d->getch()) != EOF && c != '"' ) {
	if ( i == (int)buf.size() )
	    buf.resize( i*2+42 );
	buf[i++] = c;
    }
    if ( c == EOF ) {
	return false;
    }

    if ( i == (int)buf.size() ) // always use a 0 terminator
	buf.resize( i+1 );
    buf[i] = '\0';
    return true;
}



static int nextColorSpec(const TQCString & buf)
{
    int i = buf.find(" c ");
    if (i < 0)
        i = buf.find(" g ");
    if (i < 0)
        i = buf.find(" g4 ");
    if (i < 0)
        i = buf.find(" m ");
    if (i < 0)
        i = buf.find(" s ");
    return i;
}

//
// INTERNAL
//
// Reads an .xpm from either the TQImageIO or from the TQString *.
// One of the two HAS to be 0, the other one is used.
//

static void read_xpm_image_or_array( TQImageIO * iio, const char * const * source,
				     TQImage & image)
{
    TQCString buf;
    TQIODevice *d = 0;
    buf.resize( 200 );

    int i, cpp, ncols, w, h, index = 0;

    if ( iio ) {
	iio->setStatus( 1 );
	d = iio ? iio->ioDevice() : 0;
	d->readLine( buf.data(), buf.size() );	// "/* XPM */"
	TQRegExp r( TQString::fromLatin1("/\\*.XPM.\\*/") );
	if ( buf.find(r) == -1 )
	    return;					// bad magic
    } else if ( !source ) {
	return;
    }

    if ( !read_xpm_string( buf, d, source, index ) )
	return;

    if ( sscanf( buf, "%d %d %d %d", &w, &h, &ncols, &cpp ) < 4 )
	return;					// < 4 numbers parsed

    if ( cpp > 15 )
	return;

    if ( ncols > 256 ) {
	image.create( w, h, 32 );
    } else {
	image.create( w, h, 8, ncols );
    }

    if (image.isNull())
        return;

    TQMap<TQString, int> colorMap;
    int currentColor;

    for( currentColor=0; currentColor < ncols; ++currentColor ) {
	if ( !read_xpm_string( buf, d, source, index ) ) {
#if defined(QT_CHECK_RANGE)
	    tqWarning( "TQImage: XPM color specification missing");
#endif
	    return;
	}
	TQString index;
	index = buf.left( cpp );
	buf = buf.mid( cpp ).simplifyWhiteSpace().lower();
	buf.prepend( " " );
	i = nextColorSpec(buf);
	if ( i < 0 ) {
#if defined(QT_CHECK_RANGE)
	    tqWarning( "TQImage: XPM color specification is missing: %s", buf.data());
#endif
	    return;	// no c/g/g4/m/s specification at all
	}
	buf = buf.mid( i+3 );
	// Strip any other colorspec
	int end = nextColorSpec(buf);
	if (end != -1)
	    buf.truncate(end);
	buf = buf.stripWhiteSpace();
	if ( buf == "none" ) {
	    image.setAlphaBuffer( true );
	    int transparentColor = currentColor;
	    if ( image.depth() == 8 ) {
		image.setColor( transparentColor,
				TQT_RGB_MASK & tqRgb(198,198,198) );
		colorMap.insert( index, transparentColor );
	    } else {
		TQRgb rgb = TQT_RGB_MASK & tqRgb(198,198,198);
		colorMap.insert( index, rgb );
	    }
	} else {
	    if ( ((buf.length()-1) % 3) && (buf[0] == '#') ) {
		buf.truncate (((buf.length()-1) / 4 * 3) + 1); // remove alpha channel left by imagemagick
	    }
	    TQColor c( buf.data() );
	    if ( image.depth() == 8 ) {
		image.setColor( currentColor, 0xff000000 | c.rgb() );
		colorMap.insert( index, currentColor );
	    } else {
		TQRgb rgb = 0xff000000 | c.rgb();
		colorMap.insert( index, rgb );
	    }
	}
    }

    // Read pixels
    for( int y=0; y<h; y++ ) {
	if ( !read_xpm_string( buf, d, source, index ) ) {
#if defined(QT_CHECK_RANGE)
	    tqWarning( "TQImage: XPM pixels missing on image line %d", y);
#endif
	    return;
	}
	if ( image.depth() == 8 ) {
	    uchar *p = image.scanLine(y);
	    uchar *d = (uchar *)buf.data();
	    uchar *end = d + buf.length();
	    int x;
	    if ( cpp == 1 ) {
		char b[2];
		b[1] = '\0';
		for ( x=0; x<w && d<end; x++ ) {
		    b[0] = *d++;
		    *p++ = (uchar)colorMap[b];
		}
	    } else {
		char b[16];
		b[cpp] = '\0';
		for ( x=0; x<w && d<end; x++ ) {
		    strncpy( b, (char *)d, cpp );
		    *p++ = (uchar)colorMap[b];
		    d += cpp;
		}
	    }
	} else {
	    TQRgb *p = (TQRgb*)image.scanLine(y);
	    uchar *d = (uchar *)buf.data();
	    uchar *end = d + buf.length();
	    int x;
	    char b[16];
	    b[cpp] = '\0';
	    for ( x=0; x<w && d<end; x++ ) {
		strncpy( b, (char *)d, cpp );
		*p++ = (TQRgb)colorMap[b];
		d += cpp;
	    }
	}
    }
    if ( iio ) {
	iio->setImage( image );
	iio->setStatus( 0 );			// image ok
    }
}


static void read_xpm_image( TQImageIO * iio )
{
    TQImage i;
    (void)read_xpm_image_or_array( iio, 0, i );
    return;
}


static const char* xpm_color_name( int cpp, int index )
{
    static char returnable[5];
    static const char code[] = ".#abcdefghijklmnopqrstuvwxyzABCD"
			       "EFGHIJKLMNOPQRSTUVWXYZ0123456789";
    // cpp is limited to 4 and index is limited to 64^cpp
    if ( cpp > 1 ) {
	if ( cpp > 2 ) {
	    if ( cpp > 3 ) {
		returnable[3] = code[index % 64];
		index /= 64;
	    } else
		returnable[3] = '\0';
	    returnable[2] = code[index % 64];
	    index /= 64;
	} else
	    returnable[2] = '\0';
	// the following 4 lines are a joke!
	if ( index == 0 )
	    index = 64*44+21;
	else if ( index == 64*44+21 )
	    index = 0;
	returnable[1] = code[index % 64];
	index /= 64;
    } else
	returnable[1] = '\0';
    returnable[0] = code[index];

    return returnable;
}


// write XPM image data
static void write_xpm_image( TQImageIO * iio )
{
    if ( iio )
	iio->setStatus( 1 );
    else
	return;

    // ### 8-bit case could be made faster
    TQImage image;
    if ( iio->image().depth() != 32 )
	image = iio->image().convertDepth( 32 );
    else
	image = iio->image();

    TQMap<TQRgb, int> colorMap;

    int w = image.width(), h = image.height(), ncolors = 0;
    int x, y;

    // build color table
    for( y=0; y<h; y++ ) {
	TQRgb * yp = (TQRgb *)image.scanLine( y );
	for( x=0; x<w; x++ ) {
	    TQRgb color = *(yp + x);
	    if ( !colorMap.contains(color) )
		colorMap.insert( color, ncolors++ );
	}
    }

    // number of 64-bit characters per pixel needed to encode all colors
    int cpp = 1;
    for ( int k = 64; ncolors > k; k *= 64 ) {
	++cpp;
	// limit to 4 characters per pixel
	// 64^4 colors is enough for a 4096x4096 image
	 if ( cpp > 4)
	    break;
    }

    TQString line;

    // write header
    TQTextStream s( iio->ioDevice() );
    s << "/* XPM */" << endl
      << "static char *" << fbname(iio->fileName()) << "[]={" << endl
      << "\"" << w << " " << h << " " << ncolors << " " << cpp << "\"";

    // write palette
    TQMap<TQRgb, int>::Iterator c = colorMap.begin();
    while ( c != colorMap.end() ) {
	TQRgb color = c.key();
	if ( image.hasAlphaBuffer() && color == (color & TQT_RGB_MASK) )
	    line.sprintf( "\"%s c None\"",
			  xpm_color_name(cpp, *c) );
	else
	    line.sprintf( "\"%s c #%02x%02x%02x\"",
			  xpm_color_name(cpp, *c),
			  tqRed(color),
			  tqGreen(color),
			  tqBlue(color) );
	++c;
	s << "," << endl << line;
    }

    // write pixels, limit to 4 characters per pixel
    line.truncate( cpp*w );
    for( y=0; y<h; y++ ) {
	TQRgb * yp = (TQRgb *) image.scanLine( y );
	int cc = 0;
	for( x=0; x<w; x++ ) {
	    int color = (int)(*(yp + x));
	    TQCString chars = xpm_color_name( cpp, colorMap[color] );
	    line[cc++] = chars[0];
	    if ( cpp > 1 ) {
		line[cc++] = chars[1];
		if ( cpp > 2 ) {
		    line[cc++] = chars[2];
		    if ( cpp > 3 ) {
			line[cc++] = chars[3];
		    }
		}
	    }
	}
	s << "," << endl << "\"" << line << "\"";
    }
    s << "};" << endl;

    iio->setStatus( 0 );
}

#endif // TQT_NO_IMAGEIO_XPM

/*!
    Returns an image with depth \a d, using the \a palette_count
    colors pointed to by \a palette. If \a d is 1 or 8, the returned
    image will have its color table ordered the same as \a palette.

    If the image needs to be modified to fit in a lower-resolution
    result (e.g. converting from 32-bit to 8-bit), use the \a
    conversion_flags to specify how you'd prefer this to happen.

    Note: currently no closest-color search is made. If colors are
    found that are not in the palette, the palette may not be used at
    all. This result should not be considered valid because it may
    change in future implementations.

    Currently inefficient for non-32-bit images.

    \sa TQt::ImageConversionFlags
*/
#ifndef TQT_NO_IMAGE_TRUECOLOR
TQImage TQImage::convertDepthWithPalette( int d, TQRgb* palette, int palette_count, int conversion_flags ) const
{
    if ( depth() == 1 ) {
	return convertDepth( 8, conversion_flags )
	       .convertDepthWithPalette( d, palette, palette_count, conversion_flags );
    } else if ( depth() == 8 ) {
	// ### this could be easily made more efficient
	return convertDepth( 32, conversion_flags )
	       .convertDepthWithPalette( d, palette, palette_count, conversion_flags );
    } else {
	TQImage result;
	convert_32_to_8( this, &result,
	    (conversion_flags&~TQt::DitherMode_Mask) | TQt::AvoidDither,
	    palette, palette_count );
	return result.convertDepth( d );
    }
}
#endif
static
bool
haveSamePalette(const TQImage& a, const TQImage& b)
{
    if (a.depth() != b.depth()) return false;
    if (a.numColors() != b.numColors()) return false;
    TQRgb* ca = a.colorTable();
    TQRgb* cb = b.colorTable();
    for (int i=a.numColors(); i--; ) {
	if (*ca++ != *cb++) return false;
    }
    return true;
}

/*!
    \relates TQImage

    Copies a block of pixels from \a src to \a dst. The pixels
    copied from source (src) are converted according to
    \a conversion_flags if it is incompatible with the destination
    (\a dst).

    \a sx, \a sy is the top-left pixel in \a src, \a dx, \a dy
    is the top-left position in \a dst and \a sw, \a sh is the
    size of the copied block.

    The copying is clipped if areas outside \a src or \a dst are
    specified.

    If \a sw is -1, it is adjusted to src->width(). Similarly, if \a
    sh is -1, it is adjusted to src->height().

    Currently inefficient for non 32-bit images.
*/
void bitBlt( TQImage* dst, int dx, int dy, const TQImage* src,
		int sx, int sy, int sw, int sh, int conversion_flags )
{
    // Parameter correction
    if ( sw < 0 ) sw = src->width();
    if ( sh < 0 ) sh = src->height();
    if ( sx < 0 ) { dx -= sx; sw += sx; sx = 0; }
    if ( sy < 0 ) { dy -= sy; sh += sy; sy = 0; }
    if ( dx < 0 ) { sx -= dx; sw += dx; dx = 0; }
    if ( dy < 0 ) { sy -= dy; sh += dy; dy = 0; }
    if ( sx + sw > src->width() ) sw = src->width() - sx;
    if ( sy + sh > src->height() ) sh = src->height() - sy;
    if ( dx + sw > dst->width() ) sw = dst->width() - dx;
    if ( dy + sh > dst->height() ) sh = dst->height() - dy;
    if ( sw <= 0 || sh <= 0 ) return; // Nothing left to copy
    if ( (dst->data == src->data) && dx==sx && dy==sy ) return; // Same pixels

    // "Easy" to copy if both same depth and one of:
    //   - 32 bit
    //   - 8 bit, identical palette
    //   - 1 bit, identical palette and byte-aligned area
    //
    if ( haveSamePalette(*dst,*src)
	 && ( (dst->depth() != 1) ||
	      (!( (dx&7) || (sx&7) ||
		 (((sw&7) && (sx+sw < src->width())) ||
		  (dx+sw < dst->width()) ) )) ) )
	{
	    // easy to copy
	} else if ( dst->depth() != 32 ) {
#ifndef TQT_NO_IMAGE_TRUECOLOR

	    TQImage dstconv = dst->convertDepth( 32 );
	    bitBlt( &dstconv, dx, dy, src, sx, sy, sw, sh,
		    (conversion_flags&~TQt::DitherMode_Mask) | TQt::AvoidDither );
	    *dst = dstconv.convertDepthWithPalette( dst->depth(),
						    dst->colorTable(), dst->numColors() );
#endif
	    return;
	}

    // Now assume palette can be ignored

    if ( dst->depth() != src->depth() ) {
	if ( ((sw == src->width()) && (sh == src->height())) || (dst->depth()==32) ) {
	    TQImage srcconv = src->convertDepth( dst->depth(), conversion_flags );
	    bitBlt( dst, dx, dy, &srcconv, sx, sy, sw, sh, conversion_flags );
	} else {
	    TQImage srcconv = src->copy( sx, sy, sw, sh ); // ie. bitBlt
	    bitBlt( dst, dx, dy, &srcconv, 0, 0, sw, sh, conversion_flags );
	}
	return;
    }

    // Now assume both are the same depth.

    // Now assume both are 32-bit or 8-bit with compatible palettes.

    // "Easy"

    switch ( dst->depth() ) {
    case 1:
	{
	    uchar* d = dst->scanLine(dy) + dx/8;
	    uchar* s = src->scanLine(sy) + sx/8;
	    const int bw = (sw+7)/8;
	    if ( bw < 64 ) {
		// Trust ourselves
		const int dd = dst->bytesPerLine() - bw;
		const int ds = src->bytesPerLine() - bw;
		while ( sh-- ) {
		    for ( int t=bw; t--; )
			*d++ = *s++;
		    d += dd;
		    s += ds;
		}
	    } else {
		// Trust libc
		const int dd = dst->bytesPerLine();
		const int ds = src->bytesPerLine();
		while ( sh-- ) {
		    memcpy( d, s, bw );
		    d += dd;
		    s += ds;
		}
	    }
	}
	break;
    case 8:
	{
	    uchar* d = dst->scanLine(dy) + dx;
	    uchar* s = src->scanLine(sy) + sx;
	    if ( sw < 64 ) {
		// Trust ourselves
		const int dd = dst->bytesPerLine() - sw;
		const int ds = src->bytesPerLine() - sw;
		while ( sh-- ) {
		    for ( int t=sw; t--; )
			*d++ = *s++;
		    d += dd;
		    s += ds;
		}
	    } else {
		// Trust libc
		const int dd = dst->bytesPerLine();
		const int ds = src->bytesPerLine();
		while ( sh-- ) {
		    memcpy( d, s, sw );
		    d += dd;
		    s += ds;
		}
	    }
	}
	break;
#ifndef TQT_NO_IMAGE_TRUECOLOR
    case 32:
	if ( src->hasAlphaBuffer() ) {
	    TQRgb* d = (TQRgb*)dst->scanLine(dy) + dx;
	    TQRgb* s = (TQRgb*)src->scanLine(sy) + sx;
	    const int dd = dst->width() - sw;
	    const int ds = src->width() - sw;
	    while ( sh-- ) {
		for ( int t=sw; t--; ) {
		    unsigned char a = tqAlpha(*s);
		    if ( a == 255 )
			*d++ = *s++;
		    else if ( a == 0 )
			++d,++s; // nothing
		    else {
			unsigned char r = ((tqRed(*s)-tqRed(*d)) * a) / 256 + tqRed(*d);
			unsigned char g = ((tqGreen(*s)-tqGreen(*d)) * a) / 256 + tqGreen(*d);
			unsigned char b = ((tqBlue(*s)-tqBlue(*d)) * a) / 256 + tqBlue(*d);
			a = TQMAX(tqAlpha(*d),a); // alternatives...
			*d++ = tqRgba(r,g,b,a);
			++s;
		    }
		}
		d += dd;
		s += ds;
	    }
	} else {
	    TQRgb* d = (TQRgb*)dst->scanLine(dy) + dx;
	    TQRgb* s = (TQRgb*)src->scanLine(sy) + sx;
	    if ( sw < 64 ) {
		// Trust ourselves
		const int dd = dst->width() - sw;
		const int ds = src->width() - sw;
		while ( sh-- ) {
		    for ( int t=sw; t--; )
			*d++ = *s++;
		    d += dd;
		    s += ds;
		}
	    } else {
		// Trust libc
		const int dd = dst->width();
		const int ds = src->width();
		const int b = sw*sizeof(TQRgb);
		while ( sh-- ) {
		    memcpy( d, s, b );
		    d += dd;
		    s += ds;
		}
	    }
	}
	break;
#endif // TQT_NO_IMAGE_TRUECOLOR
    }
}


/*!
    Returns true if this image and image \a i have the same contents;
    otherwise returns false. The comparison can be slow, unless there
    is some obvious difference, such as different widths, in which
    case the function will return quickly.

    \sa operator=()
*/

bool TQImage::operator==( const TQImage & i ) const
{
    // same object, or shared?
    if ( i.data == data )
	return true;
    // obviously different stuff?
    if ( i.data->h != data->h ||
	 i.data->w != data->w )
	return false;
    // not equal if one has alphabuffer and the other does not
    if ( i.hasAlphaBuffer() != hasAlphaBuffer() )
        return false;
    // that was the fast bit...
    TQImage i1 = convertDepth( 32 );
    TQImage i2 = i.convertDepth( 32 );
    int l;
    // if no alpha buffer used, there might still be junk in the
    // alpha bits; thus, we can't do memcmp-style comparison of scanlines
    if ( !hasAlphaBuffer() ) {
        int m;
        TQRgb *i1line;
        TQRgb *i2line;
        for( l=0; l < data->h; l++ ) {
            i1line = (uint *)i1.scanLine( l );
            i2line = (uint *)i2.scanLine( l );
            // compare pixels of scanline individually
            for ( m=0; m < data->w; m++ )
                if ( (i1line[m] ^ i2line[m]) & 0x00FFFFFF )
                    return false;
        }
    } else {
        // yay, we can do fast binary comparison on entire scanlines
        for( l=0; l < data->h; l++ )
            if ( memcmp( i1.scanLine( l ), i2.scanLine( l ), 4*data->w ) )
	        return false;
    }
    return true;
}


/*!
    Returns true if this image and image \a i have different contents;
    otherwise returns false. The comparison can be slow, unless there
    is some obvious difference, such as different widths, in which
    case the function will return quickly.

    \sa operator=()
*/

bool TQImage::operator!=( const TQImage & i ) const
{
    return !(*this == i);
}




/*!
    \fn int TQImage::dotsPerMeterX() const

    Returns the number of pixels that fit horizontally in a physical
    meter. This and dotsPerMeterY() define the intended scale and
    aspect ratio of the image.

    \sa setDotsPerMeterX()
*/

/*!
    \fn int TQImage::dotsPerMeterY() const

    Returns the number of pixels that fit vertically in a physical
    meter. This and dotsPerMeterX() define the intended scale and
    aspect ratio of the image.

    \sa setDotsPerMeterY()
*/

/*!
    Sets the value returned by dotsPerMeterX() to \a x.
*/
void TQImage::setDotsPerMeterX(int x)
{
    data->dpmx = x;
}

/*!
    Sets the value returned by dotsPerMeterY() to \a y.
*/
void TQImage::setDotsPerMeterY(int y)
{
    data->dpmy = y;
}

/*!
    \fn TQPoint TQImage::offset() const

    Returns the number of pixels by which the image is intended to be
    offset by when positioning relative to other images.
*/

/*!
    Sets the value returned by offset() to \a p.
*/
void TQImage::setOffset(const TQPoint& p)
{
    data->offset = p;
}
#ifndef TQT_NO_IMAGE_TEXT
/*!
    \internal

    Returns the internal TQImageDataMisc object. This object will be
    created if it doesn't already exist.
*/
TQImageDataMisc& TQImage::misc() const
{
    if ( !data->misc )
	data->misc = new TQImageDataMisc;
    return *data->misc;
}

/*!
    Returns the string recorded for the keyword \a key in language \a
    lang, or in a default language if \a lang is 0.
*/
TQString TQImage::text(const char* key, const char* lang) const
{
    TQImageTextKeyLang x(key,lang);
    return misc().text_lang[x];
}

/*!
    \overload

    Returns the string recorded for the keyword and language \a kl.
*/
TQString TQImage::text(const TQImageTextKeyLang& kl) const
{
    return misc().text_lang[kl];
}

/*!
    Returns the language identifiers for which some texts are
    recorded.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myImage.textLanguages();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa textList() text() setText() textKeys()
*/
TQStringList TQImage::textLanguages() const
{
    if ( !data->misc )
	return TQStringList();
    return misc().languages();
}

/*!
    Returns the keywords for which some texts are recorded.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = myImage.textKeys();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa textList() text() setText() textLanguages()
*/
TQStringList TQImage::textKeys() const
{
    if ( !data->misc )
	return TQStringList();
    return misc().keys();
}

/*!
    Returns a list of TQImageTextKeyLang objects that enumerate all the
    texts key/language pairs set by setText() for this image.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQValueList<TQImageTextKeyLang> list = myImage.textList();
    TQValueList<TQImageTextKeyLang>::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode
*/
TQValueList<TQImageTextKeyLang> TQImage::textList() const
{
    if ( !data->misc )
	return TQValueList<TQImageTextKeyLang>();
    return misc().list();
}

/*!
    Records string \a s for the keyword \a key. The \a key should be a
    portable keyword recognizable by other software - some suggested
    values can be found in \link
    http://www.libpng.org/pub/png/spec/1.2/png-1.2-pdg.html#C.Anc-text
    the PNG specification \endlink. \a s can be any text. \a lang
    should specify the language code (see
    \link http://www.rfc-editor.org/rfc/rfc1766.txt RFC 1766 \endlink) or 0.
*/
void TQImage::setText(const char* key, const char* lang, const TQString& s)
{
    TQImageTextKeyLang x(key,lang);
    misc().text_lang.replace(x,s);
}

#endif // TQT_NO_IMAGE_TEXT

#ifdef TQ_WS_QWS
/*!
    \internal
*/
TQGfx * TQImage::graphicsContext()
{
    TQGfx * ret=0;
    if(depth()) {
	int w = qt_screen->mapToDevice( TQSize(width(),height()) ).width();
	int h = qt_screen->mapToDevice( TQSize(width(),height()) ).height();
	ret=TQGfx::createGfx(depth(),bits(),w,h,bytesPerLine());
    } else {
	tqDebug("Trying to create image for null depth");
	return 0;
    }
    if(depth()<=8) {
	TQRgb * tmp=colorTable();
	int nc=numColors();
	if(tmp==0) {
	    static TQRgb table[2] = { tqRgb(255,255,255), tqRgb(0,0,0) };
	    tmp=table;
	    nc=2;
	}
	ret->setClut(tmp,nc);
    }
    return ret;
}

#endif
