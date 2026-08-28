/****************************************************************************
**
** Definition of TQImage and TQImageIO classes
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

#ifndef TQIMAGE_H
#define TQIMAGE_H

#ifndef QT_H
#include "ntqpixmap.h"
#include "ntqstrlist.h"
#include "ntqstringlist.h"
#endif // QT_H

class TQImageDataMisc; // internal
#ifndef TQT_NO_IMAGE_TEXT
class TQ_EXPORT TQImageTextKeyLang {
public:
    TQImageTextKeyLang(const char* k, const char* l) : key(k), lang(l) { }
    TQImageTextKeyLang() { }

    TQCString key;
    TQCString lang;

    bool operator< (const TQImageTextKeyLang& other) const
	{ return (key < other.key) || ((key==other.key) && (lang < other.lang)); }
    bool operator== (const TQImageTextKeyLang& other) const
	{ return (key==other.key) && (lang==other.lang); }
};
#endif //TQT_NO_IMAGE_TEXT


class TQ_EXPORT TQImage
{
public:
    enum Endian { IgnoreEndian, BigEndian, LittleEndian };

    TQImage();
    TQImage( int width, int height, int depth, int numColors=0,
	    Endian bitOrder=IgnoreEndian );
    TQImage( const TQSize&, int depth, int numColors=0,
	    Endian bitOrder=IgnoreEndian );
#ifndef TQT_NO_IMAGEIO
    TQImage( const TQString &fileName, const char* format=0 );
    TQImage( const char * const xpm[] );
    TQImage( const TQByteArray &data );
#endif
    TQImage( uchar* data, int w, int h, int depth,
		TQRgb* colortable, int numColors,
		Endian bitOrder );
#ifdef TQ_WS_QWS
    TQImage( uchar* data, int w, int h, int depth, int pbl,
		TQRgb* colortable, int numColors,
		Endian bitOrder );
#endif
    TQImage( const TQImage & );
   ~TQImage();

    TQImage     &operator=( const TQImage & );
    TQImage     &operator=( const TQPixmap & );
    bool	operator==( const TQImage & ) const;
    bool	operator!=( const TQImage & ) const;
    void	detach();
    TQImage	copy()		const;
    TQImage	copy(int x, int y, int w, int h, int conversion_flags=0) const;
    TQImage	copy(const TQRect&)	const;
#ifndef TQT_NO_MIME
    static TQImage fromMimeSource( const TQString& abs_name );
#endif
    bool	isNull()	const	{ return data->bits == 0; }

    int		width()		const	{ return data->w; }
    int		height()	const	{ return data->h; }
    TQSize	size()		const	{ return TQSize(data->w,data->h); }
    TQRect	rect()		const	{ return TQRect(0,0,data->w,data->h); }
    int		depth()		const	{ return data->d; }
    int		numColors()	const	{ return data->ncols; }
    Endian	bitOrder()	const	{ return (Endian) data->bitordr; }

    TQRgb	color( int i )	const;
    void	setColor( int i, TQRgb c );
    void	setNumColors( int );

    bool	hasAlphaBuffer() const;
    void	setAlphaBuffer( bool );

    bool	allGray() const;
    bool        isGrayscale() const;

    uchar      *bits()		const;
    uchar      *scanLine( int ) const;
    uchar     **jumpTable()	const;
    TQRgb       *colorTable()	const;
    int		numBytes()	const;
    int		bytesPerLine()	const;

#ifdef TQ_WS_QWS
    TQGfx * graphicsContext();
#endif

    bool	create( int width, int height, int depth, int numColors=0,
			Endian bitOrder=IgnoreEndian );
    bool	create( const TQSize&, int depth, int numColors=0,
			Endian bitOrder=IgnoreEndian );
    void	reset();

    void	fill( uint pixel );
    void	invertPixels( bool invertAlpha = true );

    TQImage	convertDepth( int ) const;
#ifndef TQT_NO_IMAGE_TRUECOLOR
    TQImage	convertDepthWithPalette( int, TQRgb* p, int pc, int cf=0 ) const;
#endif
    TQImage	convertDepth( int, int conversion_flags ) const;
    TQImage	convertBitOrder( Endian ) const;

    enum ScaleMode {
	ScaleFree,
	ScaleMin,
	ScaleMax
    };
#ifndef TQT_NO_IMAGE_SMOOTHSCALE
    TQImage smoothScale( int w, int h, ScaleMode mode=ScaleFree ) const;
    TQImage smoothScale( const TQSize& s, ScaleMode mode=ScaleFree ) const;
#endif
#ifndef TQT_NO_IMAGE_TRANSFORMATION
    TQImage scale( int w, int h, ScaleMode mode=ScaleFree ) const;
    TQImage scale( const TQSize& s, ScaleMode mode=ScaleFree ) const;
    TQImage scaleWidth( int w ) const;
    TQImage scaleHeight( int h ) const;
    TQImage xForm( const TQWMatrix &matrix ) const;
#endif

#ifndef TQT_NO_IMAGE_DITHER_TO_1
    TQImage	createAlphaMask( int conversion_flags=0 ) const;
#endif
#ifndef TQT_NO_IMAGE_HEURISTIC_MASK
    TQImage	createHeuristicMask( bool clipTight=true ) const;
#endif
#ifndef TQT_NO_IMAGE_MIRROR
    TQImage	mirror() const;
    TQImage	mirror(bool horizontally, bool vertically) const;
#endif
    TQImage	swapRGB() const;

    static Endian systemBitOrder();
    static Endian systemByteOrder();

#ifndef TQT_NO_IMAGEIO
    static const char* imageFormat( const TQString &fileName );
    static TQStrList inputFormats();
    static TQStrList outputFormats();
#ifndef TQT_NO_STRINGLIST
    static TQStringList inputFormatList();
    static TQStringList outputFormatList();
#endif
    bool	load( const TQString &fileName, const char* format=0 );
    bool	loadFromData( const uchar *buf, uint len,
			      const char *format=0 );
    bool	loadFromData( TQByteArray data, const char* format=0 );
    bool	save( const TQString &fileName, const char* format,
		      int quality=-1 ) const;
    bool	save( TQIODevice * device, const char* format,
		      int quality=-1 ) const;
#endif //TQT_NO_IMAGEIO

    bool	valid( int x, int y ) const;
    int		pixelIndex( int x, int y ) const;
    TQRgb	pixel( int x, int y ) const;
    void	setPixel( int x, int y, uint index_or_rgb );

    // Auxiliary data
    int dotsPerMeterX() const;
    int dotsPerMeterY() const;
    void setDotsPerMeterX(int);
    void setDotsPerMeterY(int);
    TQPoint offset() const;
    void setOffset(const TQPoint&);
#ifndef TQT_NO_IMAGE_TEXT
    TQValueList<TQImageTextKeyLang> textList() const;
    TQStringList textLanguages() const;
    TQStringList textKeys() const;
    TQString text(const char* key, const char* lang=0) const;
    TQString text(const TQImageTextKeyLang&) const;
    void setText(const char* key, const char* lang, const TQString&);
#endif
private:
    void	init();
    void	reinit();
    void	freeBits();
    static void	warningIndexRange( const char *, int );

    struct TQImageData : public TQShared {	// internal image data
	int	w;				// image width
	int	h;				// image height
	int	d;				// image depth
	int	ncols;				// number of colors
	int	nbytes;				// number of bytes data
	int	bitordr;			// bit order (1 bit depth)
	TQRgb   *ctbl;				// color table
	uchar **bits;				// image data
	bool	alpha;				// alpha buffer
	int	dpmx;				// dots per meter X (or 0)
	int	dpmy;				// dots per meter Y (or 0)
	TQPoint	offset;				// offset in pixels
#ifndef TQT_NO_IMAGE_TEXT
	TQImageDataMisc* misc;			// less common stuff
#endif
	bool    ctbl_mine;			// this allocated ctbl
    } *data;
#ifndef TQT_NO_IMAGE_TEXT
    TQImageDataMisc& misc() const;
#endif
#ifndef TQT_NO_IMAGEIO
    bool doImageIO( TQImageIO* io, int quality ) const;
#endif
    friend TQ_EXPORT void bitBlt( TQImage* dst, int dx, int dy,
				 const TQImage* src, int sx, int sy,
				 int sw, int sh, int conversion_flags );
};


// TQImage stream functions

#if !defined(TQT_NO_DATASTREAM) && !defined(TQT_NO_IMAGEIO)
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQImage & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQImage & );
#endif

#ifndef TQT_NO_IMAGEIO
class TQIODevice;
typedef void (*image_io_handler)( TQImageIO * ); // image IO handler


struct TQImageIOData;


class TQ_EXPORT TQImageIO
{
public:
    TQImageIO();
    TQImageIO( TQIODevice	 *ioDevice, const char *format );
    TQImageIO( const TQString &fileName, const char* format );
   ~TQImageIO();


    const TQImage &image()	const	{ return im; }
    int		status()	const	{ return iostat; }
    const char *format()	const	{ return frmt; }
    TQIODevice  *ioDevice()	const	{ return iodev; }
    TQString	fileName()	const	{ return fname; }
    int		quality()	const;
    TQString	description()	const	{ return descr; }
    const char *parameters()	const;
    float gamma() const;

    void	setImage( const TQImage & );
    void	setStatus( int );
    void	setFormat( const char * );
    void	setIODevice( TQIODevice * );
    void	setFileName( const TQString & );
    void	setQuality( int );
    void	setDescription( const TQString & );
    void	setParameters( const char * );
    void	setGamma( float );

    bool	read();
    bool	write();

    static const char* imageFormat( const TQString &fileName );
    static const char *imageFormat( TQIODevice * );
    static TQStrList inputFormats();
    static TQStrList outputFormats();

    static void defineIOHandler( const char *format,
				 const char *header,
				 const char *flags,
				 image_io_handler read_image,
				 image_io_handler write_image );

private:
    void	init();

    TQImage	im;				// image
    int		iostat;				// IO status
    TQCString	frmt;				// image format
    TQIODevice  *iodev;				// IO device
    TQString	fname;				// file name
    char       *params;				// image parameters //### change to TQImageIOData *d in 3.0
    TQString     descr;				// image description
    TQImageIOData *d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQImageIO( const TQImageIO & );
    TQImageIO &operator=( const TQImageIO & );
#endif
};

#endif //TQT_NO_IMAGEIO

TQ_EXPORT void bitBlt( TQImage* dst, int dx, int dy, const TQImage* src,
		      int sx=0, int sy=0, int sw=-1, int sh=-1,
		      int conversion_flags=0 );


/*****************************************************************************
  TQImage member functions
 *****************************************************************************/

inline bool TQImage::hasAlphaBuffer() const
{
    return data->alpha;
}

inline uchar *TQImage::bits() const
{
    return data->bits ? data->bits[0] : 0;
}

inline uchar **TQImage::jumpTable() const
{
    return data->bits;
}

inline TQRgb *TQImage::colorTable() const
{
    return data->ctbl;
}

inline int TQImage::numBytes() const
{
    return data->nbytes;
}

inline int TQImage::bytesPerLine() const
{
    return data->h ? data->nbytes/data->h : 0;
}

inline TQImage TQImage::copy(const TQRect& r) const
{
    return copy(r.x(), r.y(), r.width(), r.height());
}

inline TQRgb TQImage::color( int i ) const
{
#if defined(QT_CHECK_RANGE)
    if ( i >= data->ncols )
	warningIndexRange( "color", i );
#endif
    return data->ctbl ? data->ctbl[i] : (TQRgb)-1;
}

inline void TQImage::setColor( int i, TQRgb c )
{
#if defined(QT_CHECK_RANGE)
    if ( i >= data->ncols )
	warningIndexRange( "setColor", i );
#endif
    if ( data->ctbl )
	data->ctbl[i] = c;
}

inline uchar *TQImage::scanLine( int i ) const
{
#if defined(QT_CHECK_RANGE)
    if ( i >= data->h )
	warningIndexRange( "scanLine", i );
#endif
    return data->bits ? data->bits[i] : 0;
}

inline int TQImage::dotsPerMeterX() const
{
    return data->dpmx;
}

inline int TQImage::dotsPerMeterY() const
{
    return data->dpmy;
}

inline TQPoint TQImage::offset() const
{
    return data->offset;
}


#endif // TQIMAGE_H
