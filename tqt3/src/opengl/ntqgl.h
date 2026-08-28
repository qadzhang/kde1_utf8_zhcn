/****************************************************************************
**
** Definition of OpenGL classes for TQt
**
** Created : 970112
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the opengl module of the TQt GUI Toolkit.
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

#ifndef TQGL_H
#define TQGL_H

#ifndef QT_H
#include "ntqwidget.h"
#include "ntqglcolormap.h"
#endif // QT_H

#if !defined( TQT_MODULE_OPENGL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_OPENGL
#else
#define TQM_EXPORT_OPENGL TQ_EXPORT
#endif

#ifndef TQT_NO_COMPAT
#define TQGL_VERSION	450
#define TQGL_VERSION_STR	"4.5"
TQM_EXPORT_OPENGL inline const char *qGLVersion() {
    tqObsolete( 0, "qGLVersion", "qVersion" );
    return TQGL_VERSION_STR;
}
#endif

#if defined(TQ_WS_WIN)
# include "qt_windows.h"
#endif

#if defined(TQ_WS_MAC)
#if !defined( TQMAC_OPENGL_DOUBLEBUFFER )
/* This macro is different now. If the macro is not defined TQGLWidget will
 * try to determine when you need double buffering.  If set to 0 it will
 * never double buffer and *can* be acclerated. If set to 1 (the default)
 * it will always double buffer. Unlike before the value of this macro does
 * not upset binary compatability either. */
#if QT_MACOSX_VERSION >= 0x1020
# define TQMAC_OPENGL_DOUBLEBUFFER 0
#endif
#endif
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
#else
# include <GL/gl.h>
# include <GL/glu.h>
#endif

#if defined(TQ_WS_WIN) || defined(TQ_WS_MAC)
class TQGLCmap;
#endif

class TQPixmap;
#if defined(TQ_WS_X11)
class TQGLOverlayWidget;
#endif

// Namespace class:
class TQM_EXPORT_OPENGL TQGL
{
public:
    enum FormatOption {
	DoubleBuffer		= 0x0001,
	DepthBuffer		= 0x0002,
	Rgba			= 0x0004,
	AlphaChannel		= 0x0008,
	AccumBuffer		= 0x0010,
	StencilBuffer		= 0x0020,
	StereoBuffers		= 0x0040,
	DirectRendering		= 0x0080,
	HasOverlay		= 0x0100,
	SingleBuffer            = DoubleBuffer  << 16,
	NoDepthBuffer           = DepthBuffer   << 16,
	ColorIndex              = Rgba          << 16,
	NoAlphaChannel          = AlphaChannel  << 16,
	NoAccumBuffer           = AccumBuffer   << 16,
	NoStencilBuffer         = StencilBuffer << 16,
	NoStereoBuffers         = StereoBuffers << 16,
	IndirectRendering       = DirectRendering << 16,
	NoOverlay		= HasOverlay << 16
    };
};



class TQM_EXPORT_OPENGL TQGLFormat : public TQGL
{
public:
    TQGLFormat();
    TQGLFormat( int options, int plane = 0 );

    bool doubleBuffer() const;
    void setDoubleBuffer( bool enable );
    bool depth() const;
    void setDepth( bool enable );
    bool rgba() const;
    void setRgba( bool enable );
    bool alpha() const;
    void setAlpha( bool enable );
    bool accum() const;
    void setAccum( bool enable );
    bool stencil() const;
    void setStencil( bool enable );
    bool stereo() const;
    void setStereo( bool enable );
    bool directRendering() const;
    void setDirectRendering( bool enable );
    bool hasOverlay() const;
    void setOverlay( bool enable );

    int plane() const;
    void setPlane( int plane );

    void setOption( FormatOption opt );
    bool testOption( FormatOption opt ) const;

    static TQGLFormat defaultFormat();
    static void setDefaultFormat( const TQGLFormat& f );

    static TQGLFormat defaultOverlayFormat();
    static void setDefaultOverlayFormat( const TQGLFormat& f );

    static bool hasOpenGL();
    static bool hasOpenGLOverlays();

    friend TQM_EXPORT_OPENGL bool operator==( const TQGLFormat&,
					     const TQGLFormat& );
    friend TQM_EXPORT_OPENGL bool operator!=( const TQGLFormat&,
					     const TQGLFormat& );
private:
    uint opts;
    int pln;
};


TQM_EXPORT_OPENGL bool operator==( const TQGLFormat&, const TQGLFormat& );
TQM_EXPORT_OPENGL bool operator!=( const TQGLFormat&, const TQGLFormat& );

class TQM_EXPORT_OPENGL TQGLContext : public TQGL
{
public:
    TQGLContext( const TQGLFormat& format, TQPaintDevice* device );
    TQGLContext( const TQGLFormat& format );
    virtual ~TQGLContext();

    virtual bool create( const TQGLContext* shareContext = 0 );
    bool isValid() const;
    bool isSharing() const;
    virtual void reset();

    TQGLFormat format() const;
    TQGLFormat requestedFormat() const;
    virtual void setFormat( const TQGLFormat& format );

    virtual void makeCurrent();
    virtual void swapBuffers() const;

    TQPaintDevice* device() const;

    TQColor overlayTransparentColor() const;

    static const TQGLContext* currentContext();

protected:
    virtual bool chooseContext( const TQGLContext* shareContext = 0 );
    virtual void doneCurrent(); // ### 4.0: make this public - needed for multithreading stuff

#if defined(TQ_WS_WIN)
    virtual int choosePixelFormat( void* pfd, HDC pdc );
#endif
#if defined(TQ_WS_X11)
    virtual void* tryVisual( const TQGLFormat& f, int bufDepth = 1 );
    virtual void* chooseVisual();
#endif
#if defined(TQ_WS_MAC)
    virtual void* chooseMacVisual(GDHandle);
#endif

    bool deviceIsPixmap() const;
    bool windowCreated() const;
    void setWindowCreated( bool on );
    bool initialized() const;
    void setInitialized( bool on );
    void generateFontDisplayLists( const TQFont & fnt, int listBase );

    uint colorIndex( const TQColor& c ) const;
    void setValid( bool valid );
    void setDevice( TQPaintDevice *pDev );

protected:
#if  defined(TQ_WS_WIN)
    HGLRC rc;
    HDC dc;
    WId	win;
    int pixelFormatId;
    TQGLCmap* cmap;
#elif defined(TQ_WS_X11) || defined(TQ_WS_MAC)
    void* vi;
    void* cx;
#if defined(TQ_WS_X11)
    TQ_UINT32 gpm;
#endif
#endif
    TQGLFormat glFormat;
    TQGLFormat reqFormat;
    static TQGLContext*	currentCtx;

private:
    void init( TQPaintDevice *dev = 0 );
    class Private {
    public:
	bool valid;
	bool sharing;
	bool initDone;
	bool crWin;
	TQPaintDevice* paintDevice;
	TQColor transpColor;
#ifdef TQ_WS_MAC
	TQRect oldR;
#endif
    };
    Private* d;

    friend class TQGLWidget;
#ifdef TQ_WS_MAC
    void fixBufferRect();
#endif

private:	// Disabled copy constructor and operator=
    TQGLContext() {}
    TQGLContext( const TQGLContext& ) {}
    TQGLContext& operator=( const TQGLContext& ) { return *this; }
};




class TQM_EXPORT_OPENGL TQGLWidget : public TQWidget, public TQGL
{
    TQ_OBJECT
public:
    TQGLWidget( TQWidget* parent=0, const char* name=0,
	       const TQGLWidget* shareWidget = 0, WFlags f=0 );
    TQGLWidget( TQGLContext *context, TQWidget* parent, const char* name=0,
	       const TQGLWidget* shareWidget = 0, WFlags f=0 );
    TQGLWidget( const TQGLFormat& format, TQWidget* parent=0, const char* name=0,
	       const TQGLWidget* shareWidget = 0, WFlags f=0 );
    ~TQGLWidget();

    void qglColor( const TQColor& c ) const;
    void qglClearColor( const TQColor& c ) const;

    bool isValid() const;
    bool isSharing() const;
    virtual void makeCurrent();
    void doneCurrent();
    
    bool doubleBuffer() const;
    virtual void swapBuffers();

    TQGLFormat format() const;
#ifndef Q_QDOC
    virtual void setFormat( const TQGLFormat& format );
#endif

    const TQGLContext* context() const;
#ifndef Q_QDOC
    virtual void setContext( TQGLContext* context,
			     const TQGLContext* shareContext = 0,
			     bool deleteOldContext = true );
#endif

    virtual TQPixmap renderPixmap( int w = 0, int h = 0,
				  bool useContext = false );
    virtual TQImage grabFrameBuffer( bool withAlpha = false );

    virtual void makeOverlayCurrent();
    const TQGLContext* overlayContext() const;

    static TQImage convertToGLFormat( const TQImage& img );

    void setMouseTracking( bool enable );
    virtual void  reparent( TQWidget* parent, WFlags f, const TQPoint& p,
			    bool showIt = false );

    const TQGLColormap & colormap() const;
    void  setColormap( const TQGLColormap & map );

    void renderText( int x, int y, const TQString & str,
		     const TQFont & fnt = TQFont(), int listBase = 2000 );
    void renderText( double x, double y, double z, const TQString & str,
		     const TQFont & fnt = TQFont(), int listBase = 2000 );
public slots:
    virtual void updateGL();
    virtual void updateOverlayGL();

protected:
    virtual void initializeGL();
    virtual void resizeGL( int w, int h );
    virtual void paintGL();

    virtual void initializeOverlayGL();
    virtual void resizeOverlayGL( int w, int h );
    virtual void paintOverlayGL();

    void setAutoBufferSwap( bool on );
    bool autoBufferSwap() const;

    void paintEvent( TQPaintEvent* );
    void resizeEvent( TQResizeEvent* );

    virtual void glInit();
    virtual void glDraw();

private:
    int displayListBase( const TQFont & fnt, int listBase );
    void cleanupColormaps();
    void init( TQGLContext *context, const TQGLWidget* shareWidget );
    bool renderCxPm( TQPixmap* pm );
    TQGLContext* glcx;
    bool autoSwap;

    TQGLColormap cmap;

#if defined(TQ_WS_WIN) || defined(TQ_WS_MAC)
    TQGLContext* olcx;
#elif defined(TQ_WS_X11)
    TQGLOverlayWidget*	olw;
    friend class TQGLOverlayWidget;
#endif

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQGLWidget( const TQGLWidget& );
    TQGLWidget& operator=( const TQGLWidget& );
#endif

#if defined(TQ_WS_MAC)
private:
    const TQGLContext *slcx;
    uint pending_fix : 1,
	 glcx_dblbuf : 2,
	 dblbuf : 1,
	 clp_serial : 15;
    TQPixmap *gl_pix;
    TQGLFormat req_format;

    void macInternalRecreateContext( TQGLContext *ctx,
				     const TQGLContext* = NULL,
				     bool update = true );
    bool macInternalDoubleBuffer( bool fix = true );
    virtual void setRegionDirty( bool );
    virtual void macWidgetChangedWindow();
#endif
private slots:
    void macInternalFixBufferRect();
};


//
// TQGLFormat inline functions
//

inline bool TQGLFormat::doubleBuffer() const
{
    return testOption( DoubleBuffer );
}

inline bool TQGLFormat::depth() const
{
    return testOption( DepthBuffer );
}

inline bool TQGLFormat::rgba() const
{
    return testOption( Rgba );
}

inline bool TQGLFormat::alpha() const
{
    return testOption( AlphaChannel );
}

inline bool TQGLFormat::accum() const
{
    return testOption( AccumBuffer );
}

inline bool TQGLFormat::stencil() const
{
    return testOption( StencilBuffer );
}

inline bool TQGLFormat::stereo() const
{
    return testOption( StereoBuffers );
}

inline bool TQGLFormat::directRendering() const
{
    return testOption( DirectRendering );
}

inline bool TQGLFormat::hasOverlay() const
{
    return testOption( HasOverlay );
}

//
// TQGLContext inline functions
//

inline bool TQGLContext::isValid() const
{
    return d->valid;
}

inline void TQGLContext::setValid( bool valid )
{
    d->valid = valid;
}

inline bool TQGLContext::isSharing() const
{
    return d->sharing;
}

inline TQGLFormat TQGLContext::format() const
{
    return glFormat;
}

inline TQGLFormat TQGLContext::requestedFormat() const
{
    return reqFormat;
}

inline TQPaintDevice* TQGLContext::device() const
{
    return d->paintDevice;
}

inline bool TQGLContext::deviceIsPixmap() const
{
    return d->paintDevice->devType() == TQInternal::Pixmap;
}


inline bool TQGLContext::windowCreated() const
{
    return d->crWin;
}


inline void TQGLContext::setWindowCreated( bool on )
{
    d->crWin = on;
}

inline bool TQGLContext::initialized() const
{
    return d->initDone;
}

inline void TQGLContext::setInitialized( bool on )
{
    d->initDone = on;
}

inline const TQGLContext* TQGLContext::currentContext()
{
    return currentCtx;
}

//
// TQGLWidget inline functions
//

inline TQGLFormat TQGLWidget::format() const
{
    return glcx->format();
}

inline const TQGLContext *TQGLWidget::context() const
{
    return glcx;
}

inline bool TQGLWidget::doubleBuffer() const
{
    return glcx->format().doubleBuffer();
}

inline void TQGLWidget::setAutoBufferSwap( bool on )
{
    autoSwap = on;
}

inline bool TQGLWidget::autoBufferSwap() const
{
    return autoSwap;
}

#endif
