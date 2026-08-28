/****************************************************************************
**
** Definition of TQPixmap class
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

#ifndef TQPIXMAP_H
#define TQPIXMAP_H

#ifndef QT_H
#include "ntqpaintdevice.h"
#include "ntqcolor.h" // char*->TQColor conversion
#include "ntqstring.h" // char*->TQString conversion
#include "ntqnamespace.h"
#endif // QT_H

class TQGfx;
class TQPixmapPrivate;

#if defined(TQ_WS_WIN)
// Internal pixmap memory optimization class for Windows 9x
class TQMultiCellPixmap;
#endif


class TQ_EXPORT TQPixmap : public TQPaintDevice, public TQt
{
public:
    enum ColorMode { Auto, Color, Mono };
    enum Optimization { DefaultOptim, NoOptim, MemoryOptim=NoOptim,
			NormalOptim, BestOptim };

    TQPixmap();
    TQPixmap( const TQImage& image );
    TQPixmap( int w, int h,  int depth = -1, Optimization = DefaultOptim );
    TQPixmap( const TQSize &, int depth = -1, Optimization = DefaultOptim );
#ifndef TQT_NO_IMAGEIO
    TQPixmap( const TQString& fileName, const char *format=0,
	     ColorMode mode=Auto );
    TQPixmap( const TQString& fileName, const char *format,
	     int conversion_flags );
    TQPixmap( const char *xpm[] ); // ### in 4.0, 'const char * const xpm[]'?
    TQPixmap( const TQByteArray &data );
#endif
    TQPixmap( const TQPixmap & );
   ~TQPixmap();

    TQPixmap    &operator=( const TQPixmap & );
    TQPixmap    &operator=( const TQImage	 & );

    bool	isNull()	const;

    int		width()		const { return data->w; }
    int		height()	const { return data->h; }
    TQSize	size()		const { return TQSize(data->w,data->h); }
    TQRect	rect()		const { return TQRect(0,0,data->w,data->h); }
    int		depth()		const { return data->d; }
    static int	defaultDepth();

    void	fill( const TQColor &fillColor = TQt::white );
    void	fill( const TQWidget *, int xofs, int yofs );
    void	fill( const TQWidget *, const TQPoint &ofs );
    void	resize( int width, int height );
    void	resize( const TQSize & );

    const TQBitmap *mask() const;
    void	setMask( const TQBitmap & );
    bool	selfMask() const;
    bool	hasAlpha() const;
    bool	hasAlphaChannel() const;
#ifndef TQT_NO_IMAGE_HEURISTIC_MASK
    TQBitmap	createHeuristicMask( bool clipTight = true ) const;
#endif
#ifndef TQT_NO_MIME
    static TQPixmap fromMimeSource( const TQString& abs_name );
#endif
    static  TQPixmap grabWindow( WId, int x=0, int y=0, int w=-1, int h=-1 );
    static  TQPixmap grabWidget( TQWidget * widget,
				int x=0, int y=0, int w=-1, int h=-1 );

#ifndef TQT_NO_PIXMAP_TRANSFORMATION
    TQPixmap	    xForm( const TQWMatrix & ) const;
    static TQWMatrix trueMatrix( const TQWMatrix &, int w, int h );
#endif

    TQImage	convertToImage() const;
    bool	convertFromImage( const TQImage &, ColorMode mode=Auto );
    bool	convertFromImage( const TQImage &, int conversion_flags );
#ifndef TQT_NO_IMAGEIO
    static const char* imageFormat( const TQString &fileName );
    bool	load( const TQString& fileName, const char *format=0,
		      ColorMode mode=Auto );
    bool	load( const TQString& fileName, const char *format,
		      int conversion_flags );
    bool	loadFromData( const uchar *buf, uint len,
			      const char* format=0,
			      ColorMode mode=Auto );
    bool	loadFromData( const uchar *buf, uint len,
			      const char* format,
			      int conversion_flags );
    bool	loadFromData( const TQByteArray &data,
			      const char* format=0,
			      int conversion_flags=0 );
    bool	save( const TQString& fileName, const char* format, int quality = -1 ) const;
    bool	save( TQIODevice* device, const char* format, int quality = -1 ) const;
#endif

#if defined(TQ_WS_WIN)
    HBITMAP	hbm()		const;
#endif

    int		serialNumber()	const;

    Optimization	optimization() const;
    void		setOptimization( Optimization );
    static Optimization defaultOptimization();
    static void		setDefaultOptimization( Optimization );

    virtual void detach();

    bool	isTQBitmap() const;

#if defined(TQ_WS_WIN)
    // These functions are internal and used by Windows 9x only
    bool	isMultiCellPixmap() const;
    HDC		multiCellHandle() const;
    HBITMAP	multiCellBitmap() const;
    int		multiCellOffset() const;
    int		allocCell();
    void	freeCell( bool = false );
#endif

#if defined(TQ_WS_QWS)
    virtual TQGfx * graphicsContext(bool clip_children=true) const;
    virtual unsigned char * scanLine(int) const;
    virtual int bytesPerLine() const;
    TQRgb * clut() const;
    int numCols() const;
#elif defined(TQ_WS_X11)
    static int x11SetDefaultScreen( int screen );
    void x11SetScreen( int screen );
#endif

protected:
    TQPixmap( int w, int h, const uchar *data, bool isXbitmap );
    int metric( int ) const;

#if defined(TQ_WS_WIN)
    struct TQMCPI {				// mem optim for win9x
	TQMultiCellPixmap *mcp;
	int	offset;
    };
#endif

    struct TQPixmapData : public TQShared {	// internal pixmap data
	TQCOORD	w, h;
	short	d;
	uint	uninit	 : 1;
	uint	bitmap	 : 1;
	uint	selfmask : 1;
#if defined(TQ_WS_WIN)
	uint	mcp	 : 1;
#endif
	int	ser_no;
	TQBitmap *mask;
#if defined(TQ_WS_WIN)
	TQPixmap *maskpm;
	union {
	    HBITMAP hbm;    // if mcp == false
	    TQMCPI  *mcpi;   // if mcp == true
	} hbm_or_mcpi;
	uchar *realAlphaBits;
#ifdef Q_OS_TEMP
	uchar* ppvBits; // Pointer to DIBSection bits
#endif
#elif defined(TQ_WS_X11)
	void   *ximage;
	void   *maskgc;
	TQPixmap *alphapm;
#elif defined(TQ_WS_MAC)
	ColorTable *clut;
	TQPixmap *alphapm;
#elif defined(TQ_WS_QWS)
	int id; // ### should use TQPaintDevice::hd, since it is there
	TQRgb * clut;
	int numcols;
	int rw;
	int rh;
	bool hasAlpha;
#endif
	Optimization optim;
#if defined(TQ_WS_WIN)
	HBITMAP old_hbm;
#endif
    } *data;
private:
#ifndef TQT_NO_IMAGEIO
    bool doImageIO( TQImageIO* io, int quality ) const;
#endif
    TQPixmap( int w, int h, int depth, bool, Optimization );
    void	init( int, int, int, bool, Optimization );
    void	deref();
    TQPixmap	copy( bool ignoreMask = false ) const;
#if defined(TQ_WS_WIN)
    void initAlphaPixmap( uchar *bytes, int length, struct tagBITMAPINFO *bmi );
    void convertToAlphaPixmap( bool initAlpha=true );
    static void bitBltAlphaPixmap( TQPixmap *dst, int dx, int dy,
				   const TQPixmap *src, int sx, int sy,
				   int sw, int sh, bool useDstAlpha );
#endif
    static Optimization defOptim;
    friend TQ_EXPORT void bitBlt( TQPaintDevice *, int, int,
				 const TQPaintDevice *,
				 int, int, int, int, RasterOp, bool );
    friend TQ_EXPORT void bitBlt( TQPaintDevice *, int, int,
				 const TQImage* src,
				 int, int, int, int, int conversion_flags );
    friend TQ_EXPORT void copyBlt( TQPixmap *dst, int dx, int dy,
				  const TQPixmap *src, int sx, int sy,
				  int sw, int sh );

#if defined(TQ_WS_MAC)
    friend void unclippedScaledBitBlt(TQPaintDevice *, int, int, int, int,
				      const TQPaintDevice *, int, int, int, int,
				      TQt::RasterOp, bool, bool);
#endif

    friend class TQBitmap;
    friend class TQPaintDevice;
    friend class TQPainter;
    friend class TQGLWidget;
};


inline bool TQPixmap::isNull() const
{
    return data->w == 0;
}

inline void TQPixmap::fill( const TQWidget *w, const TQPoint &ofs )
{
    fill( w, ofs.x(), ofs.y() );
}

inline void TQPixmap::resize( const TQSize &s )
{
    resize( s.width(), s.height() );
}

inline const TQBitmap *TQPixmap::mask() const
{
    return data->mask;
}

inline bool TQPixmap::selfMask() const
{
    return data->selfmask;
}

#if defined(TQ_WS_WIN)
inline HBITMAP TQPixmap::hbm() const
{
    return data->mcp ? 0 : data->hbm_or_mcpi.hbm;
}
#endif

inline int TQPixmap::serialNumber() const
{
    return data->ser_no;
}

inline TQPixmap::Optimization TQPixmap::optimization() const
{
    return data->optim;
}

inline bool TQPixmap::isTQBitmap() const
{
    return data->bitmap;
}

#if defined(TQ_WS_WIN)
inline bool TQPixmap::isMultiCellPixmap() const
{
    return data->mcp;
}
#endif


/*****************************************************************************
  TQPixmap stream functions
 *****************************************************************************/

#if !defined(TQT_NO_DATASTREAM) && !defined(TQT_NO_IMAGEIO)
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQPixmap & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQPixmap & );
#endif

/*****************************************************************************
  TQPixmap (and TQImage) helper functions
 *****************************************************************************/

#ifndef TQT_NO_PIXMAP_TRANSFORMATION
#  define QT_XFORM_TYPE_MSBFIRST 0
#  define QT_XFORM_TYPE_LSBFIRST 1
#  if defined(TQ_WS_WIN)
#    define QT_XFORM_TYPE_WINDOWSPIXMAP 2
#  endif
bool qt_xForm_helper( const TQWMatrix&, int, int, int, uchar*, int, int, int, uchar*, int, int, int );
#endif

TQ_EXPORT void copyBlt( TQPixmap *dst, int dx, int dy,
		       const TQPixmap *src, int sx = 0, int sy = 0,
		       int sw = -1, int sh = -1 );

#endif // TQPIXMAP_H
