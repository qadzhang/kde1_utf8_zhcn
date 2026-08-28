/****************************************************************************
**
** Definition of TQPainter class
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

#ifndef TQPAINTER_H
#define TQPAINTER_H


#ifndef QT_H
#include "ntqcolor.h"
#include "ntqfontmetrics.h"
#include "ntqfontinfo.h"
#include "ntqregion.h"
#include "ntqpen.h"
#include "ntqbrush.h"
#include "ntqpointarray.h"
#include "ntqwmatrix.h"
#endif // QT_H

class TQGfx;
class TQTextCodec;
class TQTextParag;
class TQPaintDevice;
class TQTextItem;
#if defined( TQ_WS_MAC )
class TQMacSavedPortInfo;
#endif
class TQPainterPrivate;

#if defined(TQ_WS_QWS)
class TQScreen;
#endif

class TQ_EXPORT TQPainter : public TQt
{
public:
    enum CoordinateMode { CoordDevice, CoordPainter };

    TQPainter();
    TQPainter( const TQPaintDevice *, bool unclipped = false );
    TQPainter( const TQPaintDevice *, const TQWidget *, bool unclipped = false );
   ~TQPainter();

    bool	begin( const TQPaintDevice *, bool unclipped = false );
    bool	begin( const TQPaintDevice *, const TQWidget *, bool unclipped = false );
    bool	end();
    TQPaintDevice *device() const;

#ifdef TQ_WS_QWS
    TQGfx * internalGfx();
#ifdef QT_QWS_EXPERIMENTAL_SCREENPAINTER
    bool begin(TQScreen *screen);
#endif
#endif

    static void redirect( TQPaintDevice *pdev, TQPaintDevice *replacement );
    static TQPaintDevice *redirect( TQPaintDevice *pdev );

    bool	isActive() const;

    void	flush( const TQRegion &region, CoordinateMode cm = CoordDevice );
    void	flush();
    void	save();
    void	restore();

  // Drawing tools

    TQFontMetrics fontMetrics()	const;
    TQFontInfo	 fontInfo()	const;

    const TQFont &font()		const;
    void	setFont( const TQFont & );
    const TQPen &pen()		const;
    void	setPen( const TQPen & );
    void	setPen( PenStyle );
    void	setPen( const TQColor & );
    const TQBrush &brush()	const;
    void	setBrush( const TQBrush & );
    void	setBrush( BrushStyle );
    void	setBrush( const TQColor & );
    TQPoint	pos() const;

  // Drawing attributes/modes

    const TQColor &backgroundColor() const;
    void	setBackgroundColor( const TQColor & );
    BGMode	backgroundMode() const;
    void	setBackgroundMode( BGMode );
    RasterOp	rasterOp()	const;
    void	setRasterOp( RasterOp );
    const TQPoint &brushOrigin() const;
    void	setBrushOrigin( int x, int y );
    void	setBrushOrigin( const TQPoint & );

  // Scaling and transformations

//    PaintUnit unit()	       const;		// get set painter unit
//    void	setUnit( PaintUnit );		// NOT IMPLEMENTED!!!

    bool	hasViewXForm() const;
    bool	hasWorldXForm() const;

#ifndef TQT_NO_TRANSFORMATIONS
    void	setViewXForm( bool );		// set xform on/off
    TQRect	window()       const;		// get window
    void	setWindow( const TQRect & );	// set window
    void	setWindow( int x, int y, int w, int h );
    TQRect	viewport()   const;		// get viewport
    void	setViewport( const TQRect & );	// set viewport
    void	setViewport( int x, int y, int w, int h );

    void	setWorldXForm( bool );		// set world xform on/off
    const TQWMatrix &worldMatrix() const;	// get/set world xform matrix
    void	setWorldMatrix( const TQWMatrix &, bool combine=false );

    void	saveWorldMatrix();
    void	restoreWorldMatrix();

    void	scale( double sx, double sy );
    void	shear( double sh, double sv );
    void	rotate( double a );
#endif
    void	translate( double dx, double dy );
    void	resetXForm();
    double	translationX() const;
    double	translationY() const;

    TQPoint	xForm( const TQPoint & ) const;	// map virtual -> device
    TQRect	xForm( const TQRect & )	const;
    TQPointArray xForm( const TQPointArray & ) const;
    TQPointArray xForm( const TQPointArray &, int index, int npoints ) const;
    TQPoint	xFormDev( const TQPoint & ) const; // map device -> virtual
    TQRect	xFormDev( const TQRect & )  const;
    TQPointArray xFormDev( const TQPointArray & ) const;
    TQPointArray xFormDev( const TQPointArray &, int index, int npoints ) const;

  // Clipping

    void	setClipping( bool );		// set clipping on/off
    bool	hasClipping() const;
    TQRegion clipRegion( CoordinateMode = CoordDevice ) const;
    void	setClipRect( const TQRect &, CoordinateMode = CoordDevice );	// set clip rectangle
    void	setClipRect( int x, int y, int w, int h, CoordinateMode = CoordDevice );
    void	setClipRegion( const TQRegion &, CoordinateMode = CoordDevice );// set clip region

  // Graphics drawing functions

    void	drawPoint( int x, int y );
    void	drawPoint( const TQPoint & );
    void	drawPoints( const TQPointArray& a,
			    int index=0, int npoints=-1 );
    void	moveTo( int x, int y );
    void	moveTo( const TQPoint & );
    void	lineTo( int x, int y );
    void	lineTo( const TQPoint & );
    void	drawLine( int x1, int y1, int x2, int y2 );
    void	drawLine( const TQPoint &, const TQPoint & );
    void	drawRect( int x, int y, int w, int h );
    void	drawRect( const TQRect & );
    void	drawWinFocusRect( int x, int y, int w, int h );
    void	drawWinFocusRect( int x, int y, int w, int h,
				  const TQColor &bgColor );
    void	drawWinFocusRect( const TQRect & );
    void	drawWinFocusRect( const TQRect &,
				  const TQColor &bgColor );
    void	drawRoundRect( int x, int y, int w, int h, int = 25, int = 25 );
    void	drawRoundRect( const TQRect &, int = 25, int = 25 );
    void	drawEllipse( int x, int y, int w, int h );
    void	drawEllipse( const TQRect & );
    void	drawArc( int x, int y, int w, int h, int a, int alen );
    void	drawArc( const TQRect &, int a, int alen );
    void	drawPie( int x, int y, int w, int h, int a, int alen );
    void	drawPie( const TQRect &, int a, int alen );
    void	drawChord( int x, int y, int w, int h, int a, int alen );
    void	drawChord( const TQRect &, int a, int alen );
    void	drawLineSegments( const TQPointArray &,
				  int index=0, int nlines=-1 );
    void	drawPolyline( const TQPointArray &,
			      int index=0, int npoints=-1 );
    void	drawPolygon( const TQPointArray &, bool winding=false,
			     int index=0, int npoints=-1 );
    void	drawConvexPolygon( const TQPointArray &,
			     int index=0, int npoints=-1 );
#ifndef TQT_NO_BEZIER
    void	drawCubicBezier( const TQPointArray &, int index=0 );
#endif
    void	drawPixmap( int x, int y, const TQPixmap &,
			    int sx=0, int sy=0, int sw=-1, int sh=-1 );
    void	drawPixmap( const TQPoint &, const TQPixmap &,
			    const TQRect &sr );
    void	drawPixmap( const TQPoint &, const TQPixmap & );
    void	drawPixmap( const TQRect &, const TQPixmap & );
    void	drawImage( int x, int y, const TQImage &,
			   int sx = 0, int sy = 0, int sw = -1, int sh = -1,
			   int conversionFlags = 0 );
    void	drawImage( const TQPoint &, const TQImage &,
			   const TQRect &sr, int conversionFlags = 0 );
    void	drawImage( const TQPoint &, const TQImage &,
			   int conversion_flags = 0 );
    void	drawImage( const TQRect &, const TQImage & );
    void	drawTiledPixmap( int x, int y, int w, int h, const TQPixmap &,
				 int sx=0, int sy=0 );
    void	drawTiledPixmap( const TQRect &, const TQPixmap &,
				 const TQPoint & );
    void	drawTiledPixmap( const TQRect &, const TQPixmap & );
#ifndef TQT_NO_PICTURE
    void	drawPicture( const TQPicture & );
    void	drawPicture( int x, int y, const TQPicture & );
    void	drawPicture( const TQPoint &, const TQPicture & );
#endif

    void	fillRect( int x, int y, int w, int h, const TQBrush & );
    void	fillRect( const TQRect &, const TQBrush & );
    void	eraseRect( int x, int y, int w, int h );
    void	eraseRect( const TQRect & );

  // Text drawing functions

    enum TextDirection {
	Auto,
	RTL,
	LTR
    };

    void	drawText( int x, int y, const TQString &, int len = -1, TextDirection dir = Auto );
    void	drawText( const TQPoint &, const TQString &, int len = -1, TextDirection dir = Auto );

    void     drawText( int x, int y, const TQString &, int pos, int len, TextDirection dir = Auto );
    void     drawText( const TQPoint &p, const TQString &, int pos, int len, TextDirection dir = Auto );

    void	drawText( int x, int y, int w, int h, int flags,
			  const TQString&, int len = -1, TQRect *br=0,
			  TQTextParag **intern=0 );
    void	drawText( const TQRect &, int flags,
			  const TQString&, int len = -1, TQRect *br=0,
			  TQTextParag **intern=0 );

    void drawTextItem( int x, int y, const TQTextItem &ti, int textflags = 0 );
    void drawTextItem( const TQPoint& p, const TQTextItem &ti, int textflags = 0 );

    TQRect	boundingRect( int x, int y, int w, int h, int flags,
			      const TQString&, int len = -1, TQTextParag **intern=0 );
    TQRect	boundingRect( const TQRect &, int flags,
			      const TQString&, int len = -1, TQTextParag **intern=0 );

    int		tabStops() const;
    void	setTabStops( int );
    int	       *tabArray() const;
    void	setTabArray( int * );

    // Other functions

#if defined(TQ_WS_WIN)
    HDC		handle() const;
#elif defined(TQ_WS_X11) || defined(TQ_WS_MAC)
    HANDLE	handle() const;
#endif


    static void initialize();
    static void cleanup();

private:
    void	init();
    void        destroy();
    void	updateFont();
    void	updatePen();
    void	updateBrush();
#ifndef TQT_NO_TRANSFORMATIONS
    void	updateXForm();
    void	updateInvXForm();
#endif
    void	map( int, int, int *rx, int *ry ) const;
    void	map( int, int, int, int, int *, int *, int *, int * ) const;
    void	mapInv( int, int, int *, int * ) const;
    void	mapInv( int, int, int, int, int *, int *, int *, int * ) const;
    void	drawPolyInternal( const TQPointArray &, bool close=true );
    void	drawWinFocusRect( int x, int y, int w, int h, bool xorPaint,
				  const TQColor &penColor );

    enum { IsActive=0x01, ExtDev=0x02, IsStartingUp=0x04, NoCache=0x08,
	   VxF=0x10, WxF=0x20, ClipOn=0x40, SafePolygon=0x80, MonoDev=0x100,
	   DirtyFont=0x200, DirtyPen=0x400, DirtyBrush=0x800,
	   RGBColor=0x1000, FontMet=0x2000, FontInf=0x4000, CtorBegin=0x8000,
           UsePrivateCx = 0x10000, VolatileDC = 0x20000, TQt2Compat = 0x40000 };
    uint	flags;
    bool	testf( uint b ) const { return (flags&b)!=0; }
    void	setf( uint b )	{ flags |= b; }
    void	setf( uint b, bool v );
    void	clearf( uint b )	{ flags &= (uint)(~b); }
    void	fix_neg_rect( int *x, int *y, int *w, int *h );

    TQPainterPrivate *d;
    TQPaintDevice *pdev;
    TQColor	bg_col;
    uchar	bg_mode;
    uchar	rop;
    uchar	pu;
    TQPoint	bro;
    TQFont	cfont;
    TQFont *pfont; 	// font used for metrics (might be different for printers)
    TQPen	cpen;
    TQBrush	cbrush;
    TQRegion	crgn;
    int		tabstops;
    int	       *tabarray;
    int		tabarraylen;
    bool	block_ext;	// for temporary blocking of external devices

    // Transformations
#ifndef TQT_NO_TRANSFORMATIONS
    TQCOORD	wx, wy, ww, wh;
    TQCOORD	vx, vy, vw, vh;
    TQWMatrix	wxmat;

    // Cached composition (and inverse) of transformations
    TQWMatrix	xmat;
    TQWMatrix	ixmat;



    double	m11() const { return xmat.m11(); }
    double      m12() const { return xmat.m12(); }
    double      m21() const { return xmat.m21(); }
    double      m22() const { return xmat.m22(); }
    double      dx() const { return xmat.dx(); }
    double      dy() const { return xmat.dy(); }
    double	im11() const { return ixmat.m11(); }
    double      im12() const { return ixmat.m12(); }
    double      im21() const { return ixmat.m21(); }
    double      im22() const { return ixmat.m22(); }
    double      idx() const { return ixmat.dx(); }
    double      idy() const { return ixmat.dy(); }

    int		txop;
    bool	txinv;

#else
    // even without transformations we still have translations
    int		xlatex;
    int		xlatey;
#endif

    void       *penRef;				// pen cache ref
    void       *brushRef;			// brush cache ref
    void       *ps_stack;
    void       *wm_stack;
    void	killPStack();

protected:
#ifdef Q_OS_TEMP
    TQPoint	internalCurrentPos;
    uint	old_pix;			// ### All win platforms in 4.0
#endif
#if defined(TQ_WS_WIN)
    friend class TQFontEngineWin;
    friend class TQFontEngineBox;
    QT_WIN_PAINTER_MEMBERS
#elif defined(TQ_WS_X11)
    friend class TQFontEngineXLFD;
    friend class TQFontEngineXft;
    friend class TQFontEngineBox;
    Display    *dpy;				// current display
    int		scrn;				// current screen
    TQt::HANDLE	hd;				// handle to drawable
    TQt::HANDLE  rendhd;				// handle to Xft draw
    GC		gc;				// graphics context (standard)
    GC		gc_brush;			// graphics contect for brush
    TQPoint	curPt;				// current point
    uint	clip_serial;			// clipping serial number
#elif defined(TQ_WS_MAC)
    TQt::HANDLE	hd;				// handle to drawable
    void initPaintDevice(bool force=false, TQPoint *off=NULL, TQRegion *rgn=NULL);
    friend const TQRegion &tqt_mac_update_painter(TQPainter *, bool);
    friend class TQFontEngineMac;
    friend class TQMacPainter;
#elif defined(TQ_WS_QWS)
    friend class TQFontEngine;
    TQGfx * gfx;
    friend void qwsUpdateActivePainters();
#endif
    friend class TQFontMetrics;
    friend class TQFontInfo;
    friend class TQTextLayout;
    friend void qt_format_text( const TQFont &, const TQRect &r,
		     int tf, const TQString& str, int len, TQRect *brect,
		     int tabstops, int* tabarray, int tabarraylen,
		     TQTextParag **internal, TQPainter* painter );
    friend void qt_draw_background( TQPainter *p, int x, int y, int w,  int h );
    friend void qt_draw_transformed_rect( TQPainter *p,  int x, int y, int w,  int h, bool fill );
    friend class TQPrinter;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQPainter( const TQPainter & );
    TQPainter &operator=( const TQPainter & );
#endif

    enum TransformationCodes {
	TxNone      = 0,		// transformation codes
	TxTranslate = 1,		// copy in qpainter_*.cpp
	TxScale     = 2,
	TxRotShear  = 3
    };
};


/*****************************************************************************
  TQPainter member functions
 *****************************************************************************/

inline TQPaintDevice *TQPainter::device() const
{
    return pdev;
}

inline bool TQPainter::isActive() const
{
    return testf(IsActive);
}

inline const TQFont &TQPainter::font() const
{
    return cfont;
}

inline const TQPen &TQPainter::pen() const
{
    return cpen;
}

inline const TQBrush &TQPainter::brush() const
{
    return cbrush;
}

/*
inline PaintUnit TQPainter::unit() const
{
    return (PaintUnit)pu;
}
*/

inline const TQColor &TQPainter::backgroundColor() const
{
    return bg_col;
}

inline TQt::BGMode TQPainter::backgroundMode() const
{
    return (BGMode)bg_mode;
}

inline TQt::RasterOp TQPainter::rasterOp() const
{
    return (RasterOp)rop;
}

inline const TQPoint &TQPainter::brushOrigin() const
{
    return bro;
}

inline bool TQPainter::hasViewXForm() const
{
#ifndef TQT_NO_TRANSFORMATIONS
    return testf(VxF);
#else
    return xlatex || xlatey;
#endif
}

inline bool TQPainter::hasWorldXForm() const
{
#ifndef TQT_NO_TRANSFORMATIONS
    return testf(WxF);
#else
    return xlatex || xlatey;
#endif
}

inline double TQPainter::translationX() const
{
#ifndef TQT_NO_TRANSFORMATIONS
    return worldMatrix().dx();
#else
    return xlatex;
#endif
}

inline double TQPainter::translationY() const
{
#ifndef TQT_NO_TRANSFORMATIONS
    return worldMatrix().dy();
#else
    return xlatey;
#endif
}


inline bool TQPainter::hasClipping() const
{
    return testf(ClipOn);
}

inline int TQPainter::tabStops() const
{
    return tabstops;
}

inline int *TQPainter::tabArray() const
{
    return tabarray;
}

#if defined(TQ_WS_WIN)
inline HDC TQPainter::handle() const
{
    return hdc;
}
#elif defined(TQ_WS_X11) || defined(TQ_WS_MAC)
inline TQt::HANDLE TQPainter::handle() const
{
    return hd;
}
#endif

inline void TQPainter::setBrushOrigin( const TQPoint &p )
{
    setBrushOrigin( p.x(), p.y() );
}

#ifndef TQT_NO_TRANSFORMATIONS
inline void TQPainter::setWindow( const TQRect &r )
{
    setWindow( r.x(), r.y(), r.width(), r.height() );
}

inline void TQPainter::setViewport( const TQRect &r )
{
    setViewport( r.x(), r.y(), r.width(), r.height() );
}
#endif

inline void TQPainter::setClipRect( int x, int y, int w, int h, CoordinateMode m )
{
    setClipRect( TQRect(x,y,w,h), m );
}

inline void TQPainter::drawPoint( const TQPoint &p )
{
    drawPoint( p.x(), p.y() );
}

inline void TQPainter::moveTo( const TQPoint &p )
{
    moveTo( p.x(), p.y() );
}

inline void TQPainter::lineTo( const TQPoint &p )
{
    lineTo( p.x(), p.y() );
}

inline void TQPainter::drawLine( const TQPoint &p1, const TQPoint &p2 )
{
    drawLine( p1.x(), p1.y(), p2.x(), p2.y() );
}

inline void TQPainter::drawRect( const TQRect &r )
{
    drawRect( r.x(), r.y(), r.width(), r.height() );
}

inline void TQPainter::drawWinFocusRect( const TQRect &r )
{
    drawWinFocusRect( r.x(), r.y(), r.width(), r.height() );
}

inline void TQPainter::drawWinFocusRect( const TQRect &r,const TQColor &penColor )
{
    drawWinFocusRect( r.x(), r.y(), r.width(), r.height(), penColor );
}

inline void TQPainter::drawRoundRect( const TQRect &r, int xRnd, int yRnd )
{
    drawRoundRect( r.x(), r.y(), r.width(), r.height(), xRnd, yRnd );
}

inline void TQPainter::drawEllipse( const TQRect &r )
{
    drawEllipse( r.x(), r.y(), r.width(), r.height() );
}

inline void TQPainter::drawArc( const TQRect &r, int a, int alen )
{
    drawArc( r.x(), r.y(), r.width(), r.height(), a, alen );
}

inline void TQPainter::drawPie( const TQRect &r, int a, int alen )
{
    drawPie( r.x(), r.y(), r.width(), r.height(), a, alen );
}

inline void TQPainter::drawChord( const TQRect &r, int a, int alen )
{
    drawChord( r.x(), r.y(), r.width(), r.height(), a, alen );
}

inline void TQPainter::drawPixmap( const TQPoint &p, const TQPixmap &pm,
				  const TQRect &sr )
{
    drawPixmap( p.x(), p.y(), pm, sr.x(), sr.y(), sr.width(), sr.height() );
}

inline void TQPainter::drawImage( const TQPoint &p, const TQImage &pm,
                                 const TQRect &sr, int conversionFlags )
{
    drawImage( p.x(), p.y(), pm,
	       sr.x(), sr.y(), sr.width(), sr.height(), conversionFlags );
}

inline void TQPainter::drawTiledPixmap( const TQRect &r, const TQPixmap &pm,
				       const TQPoint &sp )
{
    drawTiledPixmap( r.x(), r.y(), r.width(), r.height(), pm, sp.x(), sp.y() );
}

inline void TQPainter::drawTiledPixmap( const TQRect &r, const TQPixmap &pm )
{
    drawTiledPixmap( r.x(), r.y(), r.width(), r.height(), pm, 0, 0 );
}

inline void TQPainter::fillRect( const TQRect &r, const TQBrush &brush )
{
    fillRect( r.x(), r.y(), r.width(), r.height(), brush );
}

inline void TQPainter::eraseRect( int x, int y, int w, int h )
{
    fillRect( x, y, w, h, backgroundColor() );
}

inline void TQPainter::eraseRect( const TQRect &r )
{
    fillRect( r.x(), r.y(), r.width(), r.height(), backgroundColor() );
}

inline void TQPainter::drawText( const TQPoint &p, const TQString &s, int len, TextDirection dir )
{
    drawText( p.x(), p.y(), s, 0, len, dir );
}

inline void TQPainter::drawText( const TQPoint &p, const TQString &s, int pos, int len, TextDirection dir )
{
    drawText( p.x(), p.y(), s, pos, len, dir );
}

inline void TQPainter::drawText( int x, int y, int w, int h, int tf,
				const TQString& str, int len, TQRect *br, TQTextParag **i )
{
    TQRect r(x, y, w, h);
    drawText( r, tf, str, len, br, i );
}

inline void TQPainter::drawTextItem( const TQPoint& p, const TQTextItem &ti, int textflags )
{
    drawTextItem( p.x(), p.y(), ti, textflags );
}

inline TQRect TQPainter::boundingRect( int x, int y, int w, int h, int tf,
				     const TQString& str, int len, TQTextParag **i )
{
    TQRect r(x, y, w, h);
    return boundingRect( r, tf, str, len, i );
}

#if defined(TQ_WS_QWS)
inline TQGfx * TQPainter::internalGfx()
{
    return gfx;
}
#endif

#endif // TQPAINTER_H
