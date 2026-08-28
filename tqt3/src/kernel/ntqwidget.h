/****************************************************************************
**
** Definition of TQWidget class
**
** Created : 931029
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

#ifndef TQWIDGET_H
#define TQWIDGET_H

#ifndef QT_H
#include "ntqwindowdefs.h"
#include "ntqobject.h"
#include "ntqpaintdevice.h"
#include "ntqpalette.h"
#include "ntqfont.h"
#include "ntqfontmetrics.h"
#include "ntqfontinfo.h"
#include "ntqsizepolicy.h"
#endif // QT_H

#if defined(TQ_WS_X11)
class TQInputContext;
#endif

class TQLayout;
struct TQWExtra;
struct TQTLWExtra;
class TQFocusData;
class TQCursor;
class TQWSRegionManager;
class TQStyle;
class TQStyleControlElementData;

class TQ_EXPORT TQWidget : public TQObject, public TQPaintDevice
{
    TQ_OBJECT
    TQ_ENUMS( BackgroundMode FocusPolicy BackgroundOrigin )
    TQ_PROPERTY( bool isTopLevel READ isTopLevel )
    TQ_PROPERTY( bool isDialog READ isDialog )
    TQ_PROPERTY( bool isModal READ isModal )
    TQ_PROPERTY( bool isPopup READ isPopup )
    TQ_PROPERTY( bool isDesktop READ isDesktop )
    TQ_PROPERTY( bool enabled READ isEnabled WRITE setEnabled )
    TQ_PROPERTY( TQRect geometry READ geometry WRITE setGeometry )
    TQ_PROPERTY( TQRect frameGeometry READ frameGeometry )
    TQ_PROPERTY( int x READ x )
    TQ_PROPERTY( int y READ y )
    TQ_PROPERTY( TQPoint pos READ pos WRITE move DESIGNABLE false STORED false )
    TQ_PROPERTY( TQSize frameSize READ frameSize )
    TQ_PROPERTY( TQSize size READ size WRITE resize DESIGNABLE false STORED false )
    TQ_PROPERTY( int width READ width )
    TQ_PROPERTY( int height READ height )
    TQ_PROPERTY( TQRect rect READ rect )
    TQ_PROPERTY( TQRect childrenRect READ childrenRect )
    TQ_PROPERTY( TQRegion childrenRegion READ childrenRegion )
    TQ_PROPERTY( TQSizePolicy sizePolicy READ sizePolicy WRITE setSizePolicy )
    TQ_PROPERTY( TQSize minimumSize READ minimumSize WRITE setMinimumSize )
    TQ_PROPERTY( TQSize maximumSize READ maximumSize WRITE setMaximumSize )
    TQ_PROPERTY( int minimumWidth READ minimumWidth WRITE setMinimumWidth STORED false DESIGNABLE false )
    TQ_PROPERTY( int minimumHeight READ minimumHeight WRITE setMinimumHeight STORED false DESIGNABLE false )
    TQ_PROPERTY( int maximumWidth READ maximumWidth WRITE setMaximumWidth STORED false DESIGNABLE false )
    TQ_PROPERTY( int maximumHeight READ maximumHeight WRITE setMaximumHeight STORED false DESIGNABLE false )
    TQ_PROPERTY( TQSize sizeIncrement READ sizeIncrement WRITE setSizeIncrement )
    TQ_PROPERTY( TQSize baseSize READ baseSize WRITE setBaseSize )
    TQ_PROPERTY( BackgroundMode backgroundMode READ backgroundMode WRITE setBackgroundMode DESIGNABLE false )
    TQ_PROPERTY( TQColor paletteForegroundColor READ paletteForegroundColor WRITE setPaletteForegroundColor RESET unsetPalette )
    TQ_PROPERTY( TQColor paletteBackgroundColor READ paletteBackgroundColor WRITE setPaletteBackgroundColor RESET unsetPalette )
    TQ_PROPERTY( TQPixmap paletteBackgroundPixmap READ paletteBackgroundPixmap WRITE setPaletteBackgroundPixmap RESET unsetPalette )
    TQ_PROPERTY( TQBrush backgroundBrush READ backgroundBrush )
    TQ_PROPERTY( TQColorGroup colorGroup READ colorGroup )
    TQ_PROPERTY( TQPalette palette READ palette WRITE setPalette RESET unsetPalette  STORED ownPalette )
    TQ_PROPERTY( BackgroundOrigin backgroundOrigin READ backgroundOrigin WRITE setBackgroundOrigin )
    TQ_PROPERTY( bool ownPalette READ ownPalette )
    TQ_PROPERTY( TQFont font READ font WRITE setFont RESET unsetFont STORED ownFont )
    TQ_PROPERTY( bool ownFont READ ownFont )
#ifndef TQT_NO_CURSOR
    TQ_PROPERTY( TQCursor cursor READ cursor WRITE setCursor RESET unsetCursor STORED ownCursor )
    TQ_PROPERTY( bool ownCursor READ ownCursor )
#endif
#ifndef TQT_NO_WIDGET_TOPEXTRA
    TQ_PROPERTY( TQString caption READ caption WRITE setCaption )
    TQ_PROPERTY( TQPixmap icon READ icon WRITE setIcon )
    TQ_PROPERTY( TQString iconText READ iconText WRITE setIconText )
#endif
    TQ_PROPERTY( bool mouseTracking READ hasMouseTracking WRITE setMouseTracking )
    TQ_PROPERTY( bool underMouse READ hasMouse )
    TQ_PROPERTY( bool isActiveWindow READ isActiveWindow )
    TQ_PROPERTY( bool focusEnabled READ isFocusEnabled )
    TQ_PROPERTY( FocusPolicy focusPolicy READ focusPolicy WRITE setFocusPolicy )
    TQ_PROPERTY( bool focus READ hasFocus )
    TQ_PROPERTY( bool updatesEnabled READ isUpdatesEnabled WRITE setUpdatesEnabled DESIGNABLE false )
    TQ_PROPERTY( bool visible READ isVisible )
    TQ_PROPERTY( TQRect visibleRect READ visibleRect ) // obsolete
    TQ_PROPERTY( bool hidden READ isHidden WRITE setHidden DESIGNABLE false SCRIPTABLE false )
    TQ_PROPERTY( bool shown READ isShown WRITE setShown DESIGNABLE false SCRIPTABLE false )
    TQ_PROPERTY( bool minimized READ isMinimized )
    TQ_PROPERTY( bool maximized READ isMaximized )
    TQ_PROPERTY( bool fullScreen READ isFullScreen )
    TQ_PROPERTY( TQSize sizeHint READ sizeHint )
    TQ_PROPERTY( TQSize minimumSizeHint READ minimumSizeHint )
    TQ_PROPERTY( TQRect microFocusHint READ microFocusHint )
    TQ_PROPERTY( bool acceptDrops READ acceptDrops WRITE setAcceptDrops )
    TQ_PROPERTY( bool autoMask READ autoMask WRITE setAutoMask DESIGNABLE false SCRIPTABLE false )
    TQ_PROPERTY( bool customWhatsThis READ customWhatsThis )
    TQ_PROPERTY( bool inputMethodEnabled READ isInputMethodEnabled WRITE setInputMethodEnabled DESIGNABLE false SCRIPTABLE false )
    TQ_PROPERTY( double windowOpacity READ windowOpacity WRITE setWindowOpacity DESIGNABLE false )

public:
    explicit TQWidget( TQWidget* parent=0, const char* name=0, WFlags f=0, NFlags n=0 );
    ~TQWidget();

    WId		 winId() const;
    void	 setName( const char *name );
#ifndef TQT_NO_STYLE
    // GUI style setting

    TQStyle     &style() const;
    void        setStyle( TQStyle * );
    TQStyle*	setStyle( const TQString& );
#endif
    // Widget types and states

    bool	 isTopLevel()	const;
    bool	 isDialog()	const;
    bool	 isPopup()	const;
    bool	 isDesktop()	const;
    bool	 isModal()	const;

    bool	 isEnabled()	const;
    bool	 isEnabledTo(TQWidget*) const;
    bool	 isEnabledToTLW() const;

public slots:
    virtual void setEnabled( bool );
    void setDisabled( bool );

    // Widget coordinates

public:
    TQRect	 frameGeometry() const;
    const TQRect &geometry()	const;
    int		 x()		const;
    int		 y()		const;
    TQPoint	 pos()		const;
    TQSize	 frameSize()    const;
    TQSize	 size()		const;
    int		 width()	const;
    int		 height()	const;
    TQRect	 rect()		const;
    TQRect	 childrenRect() const;
    TQRegion	 childrenRegion() const;

    TQSize	 minimumSize()	 const;
    TQSize	 maximumSize()	 const;
    int		 minimumWidth()	 const;
    int		 minimumHeight() const;
    int		 maximumWidth()	 const;
    int		 maximumHeight() const;
    void	 setMinimumSize( const TQSize & );
    virtual void setMinimumSize( int minw, int minh );
    void	 setMaximumSize( const TQSize & );
    virtual void setMaximumSize( int maxw, int maxh );
    void	 setMinimumWidth( int minw );
    void	 setMinimumHeight( int minh );
    void	 setMaximumWidth( int maxw );
    void	 setMaximumHeight( int maxh );

    TQSize	 sizeIncrement() const;
    void	 setSizeIncrement( const TQSize & );
    virtual void setSizeIncrement( int w, int h );
    TQSize	 baseSize() const;
    void	 setBaseSize( const TQSize & );
    void	 setBaseSize( int basew, int baseh );

    void	setFixedSize( const TQSize & );
    void	setFixedSize( int w, int h );
    void	setFixedWidth( int w );
    void	setFixedHeight( int h );

    // Widget coordinate mapping

    TQPoint	 mapToGlobal( const TQPoint & )	 const;
    TQPoint	 mapFromGlobal( const TQPoint & ) const;
    TQPoint	 mapToParent( const TQPoint & )	 const;
    TQPoint	 mapFromParent( const TQPoint & ) const;
    TQPoint	 mapTo( TQWidget *, const TQPoint & ) const;
    TQPoint	 mapFrom( TQWidget *, const TQPoint & ) const;

    TQWidget	*topLevelWidget()   const;

    // Widget attribute functions

    BackgroundMode	backgroundMode() const;
    virtual void	setBackgroundMode( BackgroundMode );
    void 		setBackgroundMode( BackgroundMode, BackgroundMode );

    const TQColor &	foregroundColor() const;

    const TQColor &	eraseColor() const;
    virtual void	setEraseColor( const TQColor & );

    const TQPixmap *	erasePixmap() const;
    virtual void	setErasePixmap( const TQPixmap & );

#ifndef TQT_NO_PALETTE
    const TQColorGroup & colorGroup() const;
    const TQPalette &	palette()    const;
    bool		ownPalette() const;
    virtual void	setPalette( const TQPalette & );
    void		unsetPalette();
#endif

    const TQColor &	paletteForegroundColor() const;
    void		setPaletteForegroundColor( const TQColor & );

    const TQColor &	paletteBackgroundColor() const;
    virtual void	setPaletteBackgroundColor( const TQColor & );

    const TQPixmap *	paletteBackgroundPixmap() const;
    virtual void 	setPaletteBackgroundPixmap( const TQPixmap & );

    const TQBrush&	backgroundBrush() const;

    TQFont		font() const;
    bool		ownFont() const;
    virtual void	setFont( const TQFont & );
    void		unsetFont();
    TQFontMetrics	fontMetrics() const;
    TQFontInfo	 	fontInfo() const;

#ifndef TQT_NO_CURSOR
    const TQCursor      &cursor() const;
    bool		ownCursor() const;
    virtual void	setCursor( const TQCursor & );
    virtual void	unsetCursor();
#endif
#ifndef TQT_NO_WIDGET_TOPEXTRA
    TQString		caption() const;
    const TQPixmap      *icon() const;
    TQString		iconText() const;
#endif
    bool		hasMouseTracking() const;
    bool		hasMouse() const;

    virtual void	setMask( const TQBitmap & );
    virtual void	setMask( const TQRegion & );
    void		clearMask();

    const TQColor &	backgroundColor() const; // obsolete, use eraseColor()
    virtual void	setBackgroundColor( const TQColor & ); // obsolete, use setEraseColor()
    const TQPixmap *	backgroundPixmap() const; // obsolete, use erasePixmap()
    virtual void	setBackgroundPixmap( const TQPixmap & ); // obsolete, use setErasePixmap()

public slots:
#ifndef TQT_NO_WIDGET_TOPEXTRA
    virtual void	setCaption( const TQString &);
    virtual void	setIcon( const TQPixmap & );
    virtual void	setIconText( const TQString &);
#endif
    virtual void	setMouseTracking( bool enable );

    // Keyboard input focus functions

    virtual void	setFocus();
    void		clearFocus();

public:
    enum FocusPolicy {
	NoFocus = 0,
	TabFocus = 0x1,
	ClickFocus = 0x2,
	StrongFocus = TabFocus | ClickFocus | 0x8,
	WheelFocus = StrongFocus | 0x4
    };

    bool		isActiveWindow() const;
    virtual void	setActiveWindow();
    bool		isFocusEnabled() const;

    FocusPolicy		focusPolicy() const;
    virtual void	setFocusPolicy( FocusPolicy );
    bool		hasFocus() const;
    static void		setTabOrder( TQWidget *, TQWidget * );
    virtual void	setFocusProxy( TQWidget * );
    TQWidget *		focusProxy() const;

    void setInputMethodEnabled( bool b );
    bool isInputMethodEnabled() const;
    // Grab functions

    void		grabMouse();
#ifndef TQT_NO_CURSOR
    void		grabMouse( const TQCursor & );
#endif
    void		releaseMouse();
    void		grabKeyboard();
    void		releaseKeyboard();
    static TQWidget *	mouseGrabber();
    static TQWidget *	keyboardGrabber();

    // Update/refresh functions

    bool	 	isUpdatesEnabled() const;

#if 0 //def TQ_WS_QWS
    void		repaintUnclipped( const TQRegion &, bool erase = true );
#endif
public slots:
    virtual void	setUpdatesEnabled( bool enable );
    void		update();
    void		update( int x, int y, int w, int h );
    void		update( const TQRect& );
    void		repaint();
    void		repaint( bool erase );
    void		repaint( int x, int y, int w, int h, bool erase=true );
    void		repaint( const TQRect &, bool erase = true );
    void		repaint( const TQRegion &, bool erase = true );

    // Widget management functions

    virtual void	show();
    virtual void	hide();
    void		setShown( bool show );
    void		setHidden( bool hide );
#ifndef TQT_NO_COMPAT
    void		iconify() { showMinimized(); }
#endif
    virtual void	showMinimized();
    virtual void	showMaximized();
    void		showFullScreen();
    virtual void	showNormal();
    virtual void	polish();
    void 		constPolish() const;
    bool		close();

    void		raise();
    void		lower();
    void		stackUnder( TQWidget* );
    virtual void	move( int x, int y );
    void		move( const TQPoint & );
    virtual void	resize( int w, int h );
    void		resize( const TQSize & );
    virtual void	setGeometry( int x, int y, int w, int h );
    virtual void	setGeometry( const TQRect & ); // ### make non virtual in TQt 4?

public:
    virtual bool	close( bool alsoDelete );
    bool		isVisible()	const;
    bool		isVisibleTo(TQWidget*) const;
    bool		isVisibleToTLW() const; // obsolete
    TQRect		visibleRect() const; // obsolete
    bool 		isHidden() const;
    bool 		isShown() const;
    bool		isMinimized() const;
    bool		isMaximized() const;
    bool		isFullScreen() const;

    uint windowState() const;
    void setWindowState(uint windowState);

    virtual TQSize	sizeHint() const;
    virtual TQSize	minimumSizeHint() const;
    virtual TQSizePolicy	sizePolicy() const;
    virtual void	setSizePolicy( TQSizePolicy );
    void 		setSizePolicy( TQSizePolicy::SizeType hor, TQSizePolicy::SizeType ver, bool hfw = false );
    virtual int heightForWidth(int) const;

    TQRegion	clipRegion() const;

// ### move together with other slots in TQt 4.0
public slots:
    virtual void  	adjustSize();

public:
#ifndef TQT_NO_LAYOUT
    TQLayout *		layout() const { return lay_out; }
#endif
    void		updateGeometry();
    virtual void 	reparent( TQWidget *parent, WFlags, const TQPoint &,
				  bool showIt=false );
    void		reparent( TQWidget *parent, const TQPoint &,
				  bool showIt=false );
#ifndef TQT_NO_COMPAT
    void		recreate( TQWidget *parent, WFlags f, const TQPoint & p,
				  bool showIt=false ) { reparent(parent,f,p,showIt); }
#endif

    void		erase();
    void		erase( int x, int y, int w, int h );
    void		erase( const TQRect & );
    void		erase( const TQRegion & );
    void		scroll( int dx, int dy );
    void		scroll( int dx, int dy, const TQRect& );

    void		drawText( int x, int y, const TQString &);
    void		drawText( const TQPoint &, const TQString &);

    // Misc. functions

    TQWidget *		focusWidget() const;
    TQRect               microFocusHint() const;

    // drag and drop

    bool		acceptDrops() const;
    virtual void	setAcceptDrops( bool on );

    // transparency and pseudo transparency

    virtual void	setAutoMask(bool);
    bool		autoMask() const;

    enum BackgroundOrigin { WidgetOrigin, ParentOrigin, WindowOrigin, AncestorOrigin };

    virtual void setBackgroundOrigin( BackgroundOrigin );
    BackgroundOrigin backgroundOrigin() const;
    TQPoint backgroundOffset() const;

    // whats this help
    virtual bool customWhatsThis() const;

    TQWidget *		parentWidget( bool sameWindow = false ) const;
    WState		testWState( WState s ) const;
    WFlags		testWFlags( WFlags f ) const;
    NFlags		testNFlags( NFlags f ) const;
    uint		getWState() const;
    WFlags		getWFlags() const;
    NFlags		getNFlags() const;
    static TQWidget *	find( WId );
    static TQWidgetMapper *wmapper();

    TQWidget  *childAt( int x, int y, bool includeThis = false ) const;
    TQWidget  *childAt( const TQPoint &, bool includeThis = false ) const;

#if defined(TQ_WS_QWS)
    virtual TQGfx * graphicsContext(bool clip_children=true) const;
#endif
#if defined(TQ_WS_MAC)
    TQRegion clippedRegion(bool do_children=true);
    uint clippedSerial(bool do_children=true);
#ifndef TQMAC_NO_QUARTZ
    CGContextRef macCGContext(bool clipped=true) const;
#endif
#endif
#if defined(TQ_WS_X11)
    enum X11WindowType {
        X11WindowTypeSelect,
        X11WindowTypeCombo,
        X11WindowTypeDND,
        X11WindowTypeTooltip,
        X11WindowTypeMenu, // torn-off
        X11WindowTypeDropdown,
        X11WindowTypePopup
    };
    void x11SetWindowType( X11WindowType type = X11WindowTypeSelect );
    void x11SetWindowTransient( TQWidget* parent );
#endif
    void setWindowOpacity(double level);
    double windowOpacity() const;

protected:
    // Event handlers
    bool	 event( TQEvent * );
    virtual void mousePressEvent( TQMouseEvent * );
    virtual void mouseReleaseEvent( TQMouseEvent * );
    virtual void mouseDoubleClickEvent( TQMouseEvent * );
    virtual void mouseMoveEvent( TQMouseEvent * );
#ifndef TQT_NO_WHEELEVENT
    virtual void wheelEvent( TQWheelEvent * );
#endif
    virtual void keyPressEvent( TQKeyEvent * );
    virtual void keyReleaseEvent( TQKeyEvent * );
    virtual void focusInEvent( TQFocusEvent * );
    virtual void focusOutEvent( TQFocusEvent * );
    virtual void enterEvent( TQEvent * );
    virtual void leaveEvent( TQEvent * );
    virtual void paintEvent( TQPaintEvent * );
    virtual void moveEvent( TQMoveEvent * );
    virtual void resizeEvent( TQResizeEvent * );
    virtual void closeEvent( TQCloseEvent * );
    virtual void contextMenuEvent( TQContextMenuEvent * );
    virtual void imStartEvent( TQIMEvent * );
    virtual void imComposeEvent( TQIMEvent * );
    virtual void imEndEvent( TQIMEvent * );
    virtual void tabletEvent( TQTabletEvent * );

#ifndef TQT_NO_DRAGANDDROP
    virtual void dragEnterEvent( TQDragEnterEvent * );
    virtual void dragMoveEvent( TQDragMoveEvent * );
    virtual void dragLeaveEvent( TQDragLeaveEvent * );
    virtual void dropEvent( TQDropEvent * );
#endif

    virtual void showEvent( TQShowEvent * );
    virtual void hideEvent( TQHideEvent * );

#if defined(TQ_WS_MAC)
    virtual bool macEvent( MSG * );
#endif
#if defined(TQ_WS_WIN)
    virtual bool winEvent( MSG * );
#endif
#if defined(TQ_WS_X11)
    virtual bool x11Event( XEvent * );
#endif
#if defined(TQ_WS_QWS)
    virtual bool qwsEvent( TQWSEvent * );
    virtual unsigned char *scanLine( int ) const;
    virtual int bytesPerLine() const;
#endif

    virtual void updateMask();

    // Misc. protected functions

#ifndef TQT_NO_STYLE
    virtual void styleChange( TQStyle& );
#endif
    virtual void enabledChange( bool oldEnabled );
#ifndef TQT_NO_PALETTE
    virtual void paletteChange( const TQPalette & );
#endif
    virtual void fontChange( const TQFont & );
    virtual void windowActivationChange( bool oldActive );

    int		 metric( int )	const;

#if defined(TQ_WS_X11)
#if !defined(TQT_NO_IM_EXTENSIONS)
    virtual TQWidget *icHolderWidget();
#else
    TQWidget *icHolderWidget();
#endif
    TQInputContext *getInputContext();
    void	 changeInputContext( const TQString & );
    void	 sendMouseEventToInputContext( int x, TQEvent::Type type,
					       TQt::ButtonState button,
					       TQt::ButtonState state );
#endif
    void	 resetInputContext();

    virtual void create( WId = 0, bool initializeWindow = true,
			 bool destroyOldWindow = true );
    virtual void destroy( bool destroyWindow = true,
			  bool destroySubWindows = true );
    virtual void setWState( uint );
    void	 clearWState( uint n );
    virtual void setWFlags( WFlags );
    void	 clearWFlags( WFlags n );
    virtual void setNFlags( NFlags );
    void	 clearNFlags( NFlags n );

    virtual bool focusNextPrevChild( bool next );

    TQWExtra	*extraData();
    TQTLWExtra	*topData();
    TQFocusData	*focusData();

    virtual void setKeyCompression(bool);
    virtual void setMicroFocusHint(int x, int y, int w, int h, bool text=true, TQFont *f = 0);

#if defined(TQ_WS_MAC)
    void dirtyClippedRegion(bool);
    bool isClippedRegionDirty();
    virtual void setRegionDirty(bool);
    virtual void macWidgetChangedWindow();
#endif

private slots:
    void	 focusProxyDestroyed();
#if defined(TQ_WS_X11)
    void	 destroyInputContext();
#endif

private:
    void	 setFontSys( TQFont *f = 0 );
#if defined(TQ_WS_X11)
    void	 createInputContext();
    void	 focusInputContext();
    void	 unfocusInputContext();
    void	 checkChildrenDnd();

#ifndef TQT_NO_XSYNC
    void        createSyncCounter();
    void        destroySyncCounter();
    void        incrementSyncCounter();
    void        handleSyncRequest( void* ev );
#endif

#elif defined(TQ_WS_MAC)
    uint    own_id : 1, macDropEnabled : 1;
    EventHandlerRef window_event;
    //mac event functions
    void    propagateUpdates(bool update_rgn=true);
    void    update( const TQRegion& );
    //friends, way too many - fix this immediately!
    friend void tqt_clean_root_win();
    friend bool tqt_recreate_root_win();
    friend TQPoint posInWindow(TQWidget *);
    friend bool tqt_mac_update_sizer(TQWidget *, int);
    friend TQWidget *tqt_recursive_match(TQWidget *widg, int x, int y);
    friend bool tqt_paint_children(TQWidget *,TQRegion &, uchar ops);
    friend TQMAC_PASCAL OSStatus tqt_window_event(EventHandlerCallRef er, EventRef event, void *);
    friend void tqt_event_request_updates(TQWidget *, const TQRegion &, bool subtract);
    friend bool tqt_window_rgn(WId, short, RgnHandle, bool);
    friend class TQDragManager;
#endif

#ifndef TQT_NO_LAYOUT
    void 	 setLayout( TQLayout *l );
#endif
    void	 setWinId( WId );
    void	 showWindow();
    void	 hideWindow();
    void	 showChildren( bool spontaneous );
    void	 hideChildren( bool spontaneous );
    void	 reparentSys( TQWidget *parent, WFlags, const TQPoint &,  bool showIt);
    void	 createTLExtra();
    void	 createExtra();
    void	 deleteExtra();
    void	 createSysExtra();
    void	 deleteSysExtra();
    void	 createTLSysExtra();
    void	 deleteTLSysExtra();
    void	 deactivateWidgetCleanup();
    void	 internalSetGeometry( int, int, int, int, bool );
    void	 reparentFocusWidgets( TQWidget * );
    TQFocusData	*focusData( bool create );
    void         setBackgroundFromMode();
    void         setBackgroundColorDirect( const TQColor & );
    void   	 setBackgroundPixmapDirect( const TQPixmap & );
    void         setBackgroundModeDirect( BackgroundMode );
    void         setBackgroundEmpty();
    void	 updateFrameStrut() const;
#if defined(TQ_WS_X11)
    void         setBackgroundX11Relative();
#endif

    WId			winid;
    uint		widget_state;
    uint		widget_flags;
    uint		netwm_flags;
    uint		reserved_1;
    uint		reserved_2;
    uint		reserved_3;
    uint		reserved_4;
    uint		focus_policy : 4;
    uint 		own_font :1;
    uint 		own_palette :1;
    uint 		sizehint_forced :1;
    uint 		is_closing :1;
    uint 		in_show : 1;
    uint 		in_show_maximized : 1;
    uint		fstrut_dirty : 1;
    uint		im_enabled : 1;
    TQRect		crect;
    TQColor		bg_col;
#ifndef TQT_NO_PALETTE
    TQPalette	 pal;
#endif
    TQFont	 fnt;
#ifndef TQT_NO_LAYOUT
    TQLayout 	*lay_out;
#endif
#if defined(TQ_WS_X11) && !defined(TQT_NO_IM) && !defined(TQT_NO_IM_EXTENSIONS)
    TQInputContext *ic;				// Input Context
#endif
    TQWExtra	*extra;
#if defined(TQ_WS_QWS)
    TQRegion	 req_region;			// Requested region
    mutable TQRegion	 paintable_region;	// Paintable region
    mutable bool         paintable_region_dirty;// needs to be recalculated
    mutable TQRegion      alloc_region;          // Allocated region
    mutable bool         alloc_region_dirty;    // needs to be recalculated
    mutable int          overlapping_children;  // Handle overlapping children

    int		 alloc_region_index;
    int		 alloc_region_revision;

    void updateOverlappingChildren() const;
    void setChildrenAllocatedDirty();
    void setChildrenAllocatedDirty( const TQRegion &r, const TQWidget *dirty=0 );
    bool isAllocatedRegionDirty() const;
    void updateRequestedRegion( const TQPoint &gpos );
    TQRegion requestedRegion() const;
    TQRegion allocatedRegion() const;
    TQRegion paintableRegion() const;

    void updateGraphicsContext( TQGfx *qgfx_qws, bool clip_children ) const;
#ifndef TQT_NO_CURSOR
    void updateCursor( const TQRegion &r ) const;
#endif

    // used to accumulate dirty region when children moved/resized.
    TQRegion dirtyChildren;
    bool isSettingGeometry;
    friend class TQWSManager;
#endif
    static int instanceCounter;  // Current number of widget instances
    static int maxInstances;     // Maximum number of widget instances

    static void	 createMapper();
    static void	 destroyMapper();
    static TQWidgetList	 *wList();
    static TQWidgetList	 *tlwList();
    static TQWidgetMapper *mapper;
    friend class TQApplication;
    friend class TQBaseApplication;
    friend class TQPainter;
    friend class TQFontMetrics;
    friend class TQFontInfo;
    friend class TQETWidget;
    friend class TQLayout;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQWidget( const TQWidget & );
    TQWidget &operator=( const TQWidget & );
#endif

public: // obsolete functions to dissappear or to become inline in 3.0
#ifndef TQT_NO_PALETTE
    void setPalette( const TQPalette &p, bool ) { setPalette( p ); }
#endif
    void setFont( const TQFont &f, bool ) { setFont( f ); }
};


inline TQt::WState TQWidget::testWState( WState s ) const
{ return (widget_state & s); }

inline TQt::WFlags TQWidget::testWFlags( WFlags f ) const
{ return (widget_flags & f); }

inline TQt::NFlags TQWidget::testNFlags( NFlags f ) const
{ return (netwm_flags & f); }

inline WId TQWidget::winId() const
{ return winid; }

inline bool TQWidget::isTopLevel() const
{ return testWFlags(WType_TopLevel); }

inline bool TQWidget::isDialog() const
{ return testWFlags(WType_Dialog); }

inline bool TQWidget::isPopup() const
{ return testWFlags(WType_Popup); }

inline bool TQWidget::isDesktop() const
{ return testWFlags(WType_Desktop); }

inline bool TQWidget::isEnabled() const
{ return !testWState(WState_Disabled); }

inline bool TQWidget::isModal() const
{ return testWFlags(WShowModal); }

inline bool TQWidget::isEnabledToTLW() const
{ return isEnabled(); }

inline const TQRect &TQWidget::geometry() const
{ return crect; }

inline TQSize TQWidget::size() const
{ return crect.size(); }

inline int TQWidget::width() const
{ return crect.width(); }

inline int TQWidget::height() const
{ return crect.height(); }

inline TQRect TQWidget::rect() const
{ return TQRect(0,0,crect.width(),crect.height()); }

inline int TQWidget::minimumWidth() const
{ return minimumSize().width(); }

inline int TQWidget::minimumHeight() const
{ return minimumSize().height(); }

inline int TQWidget::maximumWidth() const
{ return maximumSize().width(); }

inline int TQWidget::maximumHeight() const
{ return maximumSize().height(); }

inline void TQWidget::setMinimumSize( const TQSize &s )
{ setMinimumSize(s.width(),s.height()); }

inline void TQWidget::setMaximumSize( const TQSize &s )
{ setMaximumSize(s.width(),s.height()); }

inline void TQWidget::setSizeIncrement( const TQSize &s )
{ setSizeIncrement(s.width(),s.height()); }

inline void TQWidget::setBaseSize( const TQSize &s )
{ setBaseSize(s.width(),s.height()); }

inline const TQColor &TQWidget::eraseColor() const
{ return bg_col; }

#ifndef TQT_NO_PALETTE
inline const TQPalette &TQWidget::palette() const
{ return pal; }
#endif

inline TQFont TQWidget::font() const
{ return fnt; }

inline TQFontMetrics TQWidget::fontMetrics() const
{ return TQFontMetrics(font()); }

inline TQFontInfo TQWidget::fontInfo() const
{ return TQFontInfo(font()); }

inline bool TQWidget::hasMouseTracking() const
{ return testWState(WState_MouseTracking); }

inline bool TQWidget::hasMouse() const
{ return testWState(WState_HasMouse); }

inline bool  TQWidget::isFocusEnabled() const
{ return (FocusPolicy)focus_policy != NoFocus; }

inline TQWidget::FocusPolicy TQWidget::focusPolicy() const
{ return (FocusPolicy)focus_policy; }

inline bool TQWidget::isUpdatesEnabled() const
{ return !testWState(WState_BlockUpdates); }

inline void TQWidget::update( const TQRect &r )
{ update( r.x(), r.y(), r.width(), r.height() ); }

inline void TQWidget::repaint()
{ repaint( true ); }

inline void TQWidget::repaint( const TQRect &r, bool erase )
{ repaint( r.x(), r.y(), r.width(), r.height(), erase ); }

inline void TQWidget::erase()
{ erase( 0, 0, crect.width(), crect.height() ); }

inline void TQWidget::erase( const TQRect &r )
{ erase( r.x(), r.y(), r.width(), r.height() ); }

inline bool TQWidget::close()
{ return close( false ); }

inline bool TQWidget::isVisible() const
{ return testWState(WState_Visible); }

inline bool TQWidget::isVisibleToTLW() const // obsolete
{ return isVisible(); }

inline bool TQWidget::isHidden() const
{ return testWState(WState_ForceHide); }

inline bool TQWidget::isShown() const
{ return !testWState(WState_ForceHide); }

inline void TQWidget::move( const TQPoint &p )
{ move( p.x(), p.y() ); }

inline void TQWidget::resize( const TQSize &s )
{ resize( s.width(), s.height()); }

inline void TQWidget::setGeometry( const TQRect &r )
{ setGeometry( r.left(), r.top(), r.width(), r.height() ); }

inline void TQWidget::drawText( const TQPoint &p, const TQString &s )
{ drawText( p.x(), p.y(), s ); }

inline TQWidget *TQWidget::parentWidget( bool sameWindow ) const
{
    if ( sameWindow )
	return isTopLevel() ? 0 : (TQWidget *)TQObject::parent();
    return (TQWidget *)TQObject::parent();
}

inline TQWidgetMapper *TQWidget::wmapper()
{ return mapper; }

inline uint TQWidget::getWState() const
{ return widget_state; }

inline void TQWidget::setWState( uint f )
{ widget_state |= f; }

inline void TQWidget::clearWState( uint f )
{ widget_state &= ~f; }

inline TQt::WFlags TQWidget::getWFlags() const
{ return widget_flags; }

inline TQt::NFlags TQWidget::getNFlags() const
{ return netwm_flags; }

inline void TQWidget::setWFlags( WFlags f )
{ widget_flags |= f; }

inline void TQWidget::setNFlags( NFlags f )
{ netwm_flags |= f; }

inline void TQWidget::clearWFlags( WFlags f )
{ widget_flags &= ~f; }

inline void TQWidget::clearNFlags( NFlags f )
{ netwm_flags &= ~f; }

inline void TQWidget::constPolish() const
{
    if ( !testWState(WState_Polished) ) {
	TQWidget* that = (TQWidget*) this;
	that->polish();
        that->setWState(WState_Polished); // be on the safe side...
    }
}
#ifndef TQT_NO_CURSOR
inline bool TQWidget::ownCursor() const
{
    return testWState( WState_OwnCursor );
}
#endif
inline bool TQWidget::ownFont() const
{
    return own_font;
}
#ifndef TQT_NO_PALETTE
inline bool TQWidget::ownPalette() const
{
    return own_palette;
}
#endif

inline void TQWidget::setSizePolicy( TQSizePolicy::SizeType hor, TQSizePolicy::SizeType ver, bool hfw )
{
    setSizePolicy( TQSizePolicy( hor, ver, hfw) );
}

inline bool TQWidget::isInputMethodEnabled() const
{
    return (bool)im_enabled;
}

// Extra TQWidget data
//  - to minimize memory usage for members that are seldom used.
//  - top-level widgets have extra extra data to reduce cost further

class TQFocusData;
class TQWSManager;
#if defined(TQ_WS_WIN)
class TQOleDropTarget;
#endif
#if defined(TQ_WS_MAC)
class TQMacDndExtra;
#endif

struct TQ_EXPORT TQTLWExtra {
#ifndef TQT_NO_WIDGET_TOPEXTRA
    TQString  caption;				// widget caption
    TQString  iconText;				// widget icon text
    TQPixmap *icon;				// widget icon
#endif
    TQFocusData *focusData;			// focus data (for TLW)
    short    incw, inch;			// size increments
    // frame strut
    ulong    fleft, fright, ftop, fbottom;
    uint     unused : 8;                       // not used at this point...
#if defined( TQ_WS_WIN ) || defined( TQ_WS_MAC )
    uint     opacity : 8;		       // Stores opacity level on Windows/Mac OS X.
#endif
    uint     savedFlags;			// Save widgetflags while showing fullscreen
    short    basew, baseh;			// base sizes
#if defined(TQ_WS_X11)
    WId  parentWinId;				// parent window Id (valid after reparenting)
    uint     embedded : 1;			// window is embedded in another TQt application
    uint     spont_unmapped: 1;			// window was spontaneously unmapped
    uint     reserved: 1;			// reserved
    uint     dnd : 1;				// DND properties installed
    uint     uspos : 1;				// User defined position
    uint     ussize : 1;			// User defined size
#if defined(TQT_NO_IM_EXTENSIONS)
    void    *xic;				// Input Context
#endif
#ifndef TQT_NO_XSYNC
    ulong    syncCounter;
    uint     syncRequestValue[2];
#endif
#endif
#if defined(TQ_WS_MAC)
    WindowGroupRef group;
    uint     is_moved: 1;
    uint     resizer : 4;
#endif
#if defined(TQ_WS_QWS) && !defined ( TQT_NO_QWS_MANAGER )
    TQRegion decor_allocated_region;		// decoration allocated region
    TQWSManager *qwsManager;
#endif
#if defined(TQ_WS_WIN)
    HICON    winIcon;				// internal Windows icon
#endif
    TQRect    normalGeometry;			// used by showMin/maximized/FullScreen
#ifdef TQ_WS_WIN
    uint style, exstyle;
#endif
};


#define TQWIDGETSIZE_MAX 32767

// dear user: you can see this struct, but it is internal. do not touch.

struct TQ_EXPORT TQWExtra {
    TQ_INT16  minw, minh;			// minimum size
    TQ_INT16  maxw, maxh;			// maximum size
    TQPixmap *bg_pix;				// background pixmap
    TQWidget *focus_proxy;
#ifndef TQT_NO_CURSOR
    TQCursor *curs;
#endif
    TQTLWExtra *topextra;			// only useful for TLWs
#if defined(TQ_WS_WIN)
    TQOleDropTarget *dropTarget;			// drop target
#endif
#if defined(TQ_WS_X11)
    WId xDndProxy;				// XDND forwarding to embedded windows
#endif
#if defined(TQ_WS_MAC)
    TQRegion clip_saved, clip_sibs, clip_children;
    TQMacDndExtra *macDndExtra;
    TQRegion dirty_area;
    uint clip_dirty : 1, clip_serial : 15;
    uint child_dirty : 1, child_serial : 15;
#ifndef TQMAC_NO_QUARTZ
    uint ctx_children_clipped:1;
#endif // TQMAC_NO_QUARTZ
    uint has_dirty_area:1;
#endif // TQ_WS_MAC
    uint bg_origin : 2;
#if defined(TQ_WS_X11)
    uint children_use_dnd : 1;
    uint compress_events : 1;
#endif
#if defined(TQ_WS_QWS) || defined(TQ_WS_MAC)
    TQRegion mask;				// widget mask
#endif
    char     bg_mode;				// background mode
    char     bg_mode_visual;			// visual background mode
#ifndef TQT_NO_STYLE
    TQStyle* style;
#endif
    TQRect micro_focus_hint;			// micro focus hint
    TQSizePolicy size_policy;
    TQStyleControlElementData* m_ceData;
};

#define Q_DEFINED_QWIDGET
#include "ntqwinexport.h"

#endif // TQWIDGET_H
