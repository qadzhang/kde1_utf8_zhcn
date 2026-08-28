/****************************************************************************
**
** Definition of TQPaintDevice class
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

#ifndef TQPAINTDEVICE_H
#define TQPAINTDEVICE_H

#ifndef QT_H
#include "ntqwindowdefs.h"
#include "ntqrect.h"
#endif // QT_H

#if defined(TQ_WS_QWS)
class TQWSDisplay;
class TQGfx;
#endif

class TQIODevice;
class TQString;
class TQTextItem;


#if defined(TQ_WS_X11)
struct TQPaintDeviceX11Data;
#endif

union TQPDevCmdParam {
    int			 ival;
    int			*ivec;
    TQString	        *str;
    const TQPoint	*point;
    const TQRect		*rect;
    const TQPointArray	*ptarr;
    const TQPixmap	*pixmap;
    const TQImage	*image;
    const TQColor	*color;
    const TQFont		*font;
    const TQPen		*pen;
    const TQBrush	*brush;
    const TQRegion	*rgn;
    const TQWMatrix	*matrix;
    const TQTextItem     *textItem;
    TQIODevice		*device;
};



class TQ_EXPORT TQPaintDevice				// device for TQPainter
{
public:
    virtual ~TQPaintDevice();

    int		devType() const;
    bool	isExtDev() const;
    bool	paintingActive() const;

    virtual void setResolution( int );
    virtual int resolution() const;

    // Windows:	  get device context
    // X-Windows: get drawable
#if defined(TQ_WS_WIN)
    virtual HDC		handle() const;
#elif defined(TQ_WS_X11)
    virtual TQt::HANDLE	handle() const;
    virtual TQt::HANDLE  x11RenderHandle() const;
#elif defined(TQ_WS_MAC)
    virtual TQt::HANDLE      handle() const;
#elif defined(TQ_WS_QWS)
    virtual TQt::HANDLE	handle() const;
#endif

#if defined(TQ_WS_X11)
    Display 	   *x11Display() const;
    int		    x11Screen() const;
    int		    x11Depth() const;
    int		    x11Cells() const;
    TQt::HANDLE	    x11Colormap() const;
    bool	    x11DefaultColormap() const;
    void	   *x11Visual() const;
    bool	    x11DefaultVisual() const;

    static Display *x11AppDisplay();
    static int	    x11AppScreen();

    static int      x11AppDpiX();
    static int      x11AppDpiY();
    static void     x11SetAppDpiX(int);
    static void     x11SetAppDpiY(int);
    static int	    x11AppDepth();
    static int	    x11AppCells();
    static TQt::HANDLE   x11AppRootWindow();
    static TQt::HANDLE   x11AppColormap();
    static bool     x11AppDefaultColormap();
    static void    *x11AppVisual();
    static bool	    x11AppDefaultVisual();

    // ### in 4.0, the above need to go away, the below needs to take a -1 default
    // argument, signifying the default screen...
    static int	    x11AppDepth( int screen );
    static int	    x11AppCells( int screen );
    static TQt::HANDLE   x11AppRootWindow( int screen );
    static TQt::HANDLE   x11AppColormap( int screen );
    static void    *x11AppVisual( int screen );
    static bool     x11AppDefaultColormap( int screen );
    static bool	    x11AppDefaultVisual( int screen );
    static int      x11AppDpiX( int );
    static int      x11AppDpiY( int );
    static void     x11SetAppDpiX( int, int );
    static void     x11SetAppDpiY( int, int );
#endif

#if defined(TQ_WS_QWS)
    static TQWSDisplay *qwsDisplay();
    virtual unsigned char * scanLine(int) const;
    virtual int bytesPerLine() const;
    virtual TQGfx * graphicsContext(bool clip_children=true) const;
#endif

    enum PDevCmd {
	PdcNOP = 0, //  <void>
	PdcDrawPoint = 1, // point
	PdcDrawFirst = PdcDrawPoint,
	PdcMoveTo = 2, // point
	PdcLineTo = 3, // point
	PdcDrawLine = 4, // point,point
	PdcDrawRect = 5, // rect
	PdcDrawRoundRect = 6, // rect,ival,ival
	PdcDrawEllipse = 7, // rect
	PdcDrawArc = 8, // rect,ival,ival
	PdcDrawPie = 9, // rect,ival,ival
	PdcDrawChord = 10, // rect,ival,ival
	PdcDrawLineSegments = 11, // ptarr
	PdcDrawPolyline = 12, // ptarr
	PdcDrawPolygon = 13, // ptarr,ival
	PdcDrawCubicBezier = 14, // ptarr
	PdcDrawText = 15, // point,str
	PdcDrawTextFormatted = 16, // rect,ival,str
	PdcDrawPixmap = 17, // rect,pixmap
	PdcDrawImage = 18, // rect,image
	PdcDrawText2 = 19, // point,str
	PdcDrawText2Formatted = 20, // rect,ival,str
	PdcDrawTextItem = 21,
	PdcDrawLast = PdcDrawTextItem,

	// no painting commands below PdcDrawLast.

	PdcBegin = 30, //  <void>
	PdcEnd = 31, //  <void>
	PdcSave = 32, //  <void>
	PdcRestore = 33, //  <void>
	PdcSetdev = 34, // device - PRIVATE
	PdcSetBkColor = 40, // color
	PdcSetBkMode = 41, // ival
	PdcSetROP = 42, // ival
	PdcSetBrushOrigin = 43, // point
	PdcSetFont = 45, // font
	PdcSetPen = 46, // pen
	PdcSetBrush = 47, // brush
	PdcSetTabStops = 48, // ival
	PdcSetTabArray = 49, // ival,ivec
	PdcSetUnit = 50, // ival
	PdcSetVXform = 51, // ival
	PdcSetWindow = 52, // rect
	PdcSetViewport = 53, // rect
	PdcSetWXform = 54, // ival
	PdcSetWMatrix = 55, // matrix,ival
	PdcSaveWMatrix = 56,
	PdcRestoreWMatrix = 57,
	PdcSetClip = 60, // ival
	PdcSetClipRegion = 61, // rgn,cmode
	PdcFlush = 62, //  <void>
	PdcFlushRegion = 63, // rgn,cmode

	PdcReservedStart = 0, // codes 0-199 are reserved
	PdcReservedStop = 199 //   for TQt
    };

protected:
    TQPaintDevice( uint devflags );

#if defined(TQ_WS_WIN)
    HDC		hdc;				// device context
#elif defined(TQ_WS_X11)
    TQt::HANDLE	hd;				// handle to drawable
    TQt::HANDLE  rendhd;                         // handle to RENDER pict

    void		 copyX11Data( const TQPaintDevice * );
    void		 cloneX11Data( const TQPaintDevice * );
    virtual void	 setX11Data( const TQPaintDeviceX11Data* );
    TQPaintDeviceX11Data* getX11Data( bool def=false ) const;
#elif defined(TQ_WS_MAC)
#if !defined( TQMAC_NO_QUARTZ )
    CGContextRef ctx;
#endif
    void * hd;
#elif defined(TQ_WS_QWS)
    TQt::HANDLE hd;
#endif

    virtual bool cmd( int, TQPainter *, TQPDevCmdParam * );
    virtual int	 metric( int ) const;
    virtual int	 fontMet( TQFont *, int, const char * = 0, int = 0 ) const;
    virtual int	 fontInf( TQFont *, int ) const;

    ushort	devFlags;			// device flags
    ushort	painters;			// refcount

    friend class TQPainter;
    friend class TQPaintDeviceMetrics;
#if defined(TQ_WS_MAC)
#ifndef TQMAC_NO_QUARTZ
    virtual CGContextRef macCGContext(bool clipped=true) const;
#endif
    friend TQ_EXPORT void unclippedScaledBitBlt( TQPaintDevice *, int, int, int, int,
						const TQPaintDevice *, int, int, int, int, TQt::RasterOp, bool, bool );
#else
    friend TQ_EXPORT void bitBlt( TQPaintDevice *, int, int,
				 const TQPaintDevice *,
				 int, int, int, int, TQt::RasterOp, bool );
#endif
#if defined(TQ_WS_X11)
    friend void tqt_init_internal( int *, char **, Display *, TQt::HANDLE, TQt::HANDLE );
    friend void tqt_cleanup();
#endif

private:
#if defined(TQ_WS_X11)
    static Display *x_appdisplay;
    static int	    x_appscreen;

    static int	    x_appdepth;
    static int	    x_appcells;
    static TQt::HANDLE   x_approotwindow;
    static TQt::HANDLE   x_appcolormap;
    static bool	    x_appdefcolormap;
    static void	   *x_appvisual;
    static bool     x_appdefvisual;

    // ### in 4.0, remove the above, and replace with the below
    static int	      *x_appdepth_arr;
    static int	      *x_appcells_arr;
    static TQt::HANDLE *x_approotwindow_arr;
    static TQt::HANDLE *x_appcolormap_arr;
    static bool	      *x_appdefcolormap_arr;
    static void	     **x_appvisual_arr;
    static bool       *x_appdefvisual_arr;

    TQPaintDeviceX11Data* x11Data;
#endif

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQPaintDevice( const TQPaintDevice & );
    TQPaintDevice &operator=( const TQPaintDevice & );
#endif
};


TQ_EXPORT
void bitBlt( TQPaintDevice *dst, int dx, int dy,
	     const TQPaintDevice *src, int sx=0, int sy=0, int sw=-1, int sh=-1,
	     TQt::RasterOp = TQt::CopyROP, bool ignoreMask=false );

TQ_EXPORT
void bitBlt( TQPaintDevice *dst, int dx, int dy,
	     const TQImage *src, int sx=0, int sy=0, int sw=-1, int sh=-1,
	     int conversion_flags=0 );


#if defined(TQ_WS_X11)

struct TQ_EXPORT TQPaintDeviceX11Data : public TQShared {
    Display*	x_display;
    int		x_screen;
    int		x_depth;
    int		x_cells;
    TQt::HANDLE	x_colormap;
    bool	x_defcolormap;
    void*	x_visual;
    bool	x_defvisual;
};

#endif

/*****************************************************************************
  Inline functions
 *****************************************************************************/

inline int TQPaintDevice::devType() const
{ return devFlags & TQInternal::DeviceTypeMask; }

inline bool TQPaintDevice::isExtDev() const
{ return (devFlags & TQInternal::ExternalDevice) != 0; }

inline bool TQPaintDevice::paintingActive() const
{ return painters != 0; }

#if defined(TQ_WS_X11)
inline Display *TQPaintDevice::x11Display() const
{ return x11Data ? x11Data->x_display : x_appdisplay; }

inline int TQPaintDevice::x11Screen() const
{ return x11Data ? x11Data->x_screen : x_appscreen; }

inline int TQPaintDevice::x11Depth() const
{ return x11Data ? x11Data->x_depth : x_appdepth; }

inline int TQPaintDevice::x11Cells() const
{ return x11Data ? x11Data->x_cells : x_appcells; }

inline TQt::HANDLE TQPaintDevice::x11Colormap() const
{ return x11Data ? x11Data->x_colormap : x_appcolormap; }

inline bool TQPaintDevice::x11DefaultColormap() const
{ return x11Data ? x11Data->x_defcolormap : x_appdefcolormap; }

inline void *TQPaintDevice::x11Visual() const
{ return x11Data ? x11Data->x_visual : x_appvisual; }

inline bool TQPaintDevice::x11DefaultVisual() const
{ return x11Data ? x11Data->x_defvisual : x_appdefvisual; }

inline Display *TQPaintDevice::x11AppDisplay()
{ return x_appdisplay; }

inline int TQPaintDevice::x11AppScreen()
{ return x_appscreen; }

inline int TQPaintDevice::x11AppDepth( int screen )
{ return x_appdepth_arr[ screen == -1 ? x_appscreen : screen ]; }

inline int TQPaintDevice::x11AppCells( int screen )
{ return x_appcells_arr[ screen == -1 ? x_appscreen : screen ]; }

inline TQt::HANDLE TQPaintDevice::x11AppRootWindow( int screen )
{ return x_approotwindow_arr[ screen == -1 ? x_appscreen : screen ]; }

inline TQt::HANDLE TQPaintDevice::x11AppColormap( int screen )
{ return x_appcolormap_arr[ screen == -1 ? x_appscreen : screen ]; }

inline bool TQPaintDevice::x11AppDefaultColormap( int screen )
{ return x_appdefcolormap_arr[ screen == -1 ? x_appscreen : screen ]; }

inline void *TQPaintDevice::x11AppVisual( int screen )
{ return x_appvisual_arr[ screen == -1 ? x_appscreen : screen ]; }

inline bool TQPaintDevice::x11AppDefaultVisual( int screen )
{ return x_appdefvisual_arr[ screen == -1 ? x_appscreen : screen ]; }

inline int TQPaintDevice::x11AppDepth()
{ return x_appdepth; }

inline int TQPaintDevice::x11AppCells()
{ return x_appcells; }

inline TQt::HANDLE TQPaintDevice::x11AppRootWindow()
{ return x_approotwindow; }

inline TQt::HANDLE TQPaintDevice::x11AppColormap()
{ return x_appcolormap; }

inline bool TQPaintDevice::x11AppDefaultColormap()
{ return x_appdefcolormap; }

inline void *TQPaintDevice::x11AppVisual()
{ return x_appvisual; }

inline bool TQPaintDevice::x11AppDefaultVisual()
{ return x_appdefvisual; }

#endif // TQ_WS_X11


TQ_EXPORT
inline void bitBlt( TQPaintDevice *dst, const TQPoint &dp,
		    const TQPaintDevice *src, const TQRect &sr =TQRect(0,0,-1,-1),
		    TQt::RasterOp rop=TQt::CopyROP, bool ignoreMask=false )
{
    bitBlt( dst, dp.x(), dp.y(), src, sr.x(), sr.y(), sr.width(), sr.height(),
	    rop, ignoreMask );
}




#endif // TQPAINTDEVICE_H
