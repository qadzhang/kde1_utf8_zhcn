/****************************************************************************
**
** Implementation of the TQWorkspace class
**
** Created : 931107
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the workspace module of the TQt GUI Toolkit.
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

#include "ntqworkspace.h"
#ifndef TQT_NO_WORKSPACE
#include "ntqapplication.h"
#include "../widgets/qtitlebar_p.h"
#include "ntqobjectlist.h"
#include "ntqlayout.h"
#include "ntqtoolbutton.h"
#include "ntqlabel.h"
#include "ntqvbox.h"
#include "ntqaccel.h"
#include "ntqcursor.h"
#include "ntqpopupmenu.h"
#include "ntqmenubar.h"
#include "ntqguardedptr.h"
#include "ntqiconset.h"
#include "../widgets/qwidgetresizehandler_p.h"
#include "ntqfocusdata.h"
#include "ntqdatetime.h"
#include "ntqtooltip.h"
#include "ntqwmatrix.h"
#include "ntqimage.h"
#include "ntqscrollbar.h"
#include "ntqstyle.h"
#include "ntqbitmap.h"

// magic non-mdi things
#include "ntqtimer.h"
#include "ntqdockarea.h"
#include "ntqstatusbar.h"
#include "ntqmainwindow.h"
#include "ntqdockwindow.h"
#include "ntqtoolbar.h"

#define BUTTON_WIDTH	16
#define BUTTON_HEIGHT	14

/*!
    \class TQWorkspace ntqworkspace.h
    \brief The TQWorkspace widget provides a workspace window that can
    contain decorated windows, e.g. for MDI.

    \module workspace

    \ingroup application
    \ingroup organizers
    \mainclass

    MDI (multiple document interface) applications typically have one
    main window with a menu bar and toolbar, and a central widget that
    is a TQWorkspace. The workspace itself contains zero, one or more
    document windows, each of which is a widget.

    The workspace itself is an ordinary TQt widget. It has a standard
    constructor that takes a parent widget and an object name. The
    parent window is usually a TQMainWindow, but it need not be.

    Document windows (i.e. MDI windows) are also ordinary TQt widgets
    which have the workspace as their parent widget. When you call
    show(), hide(), showMaximized(), setCaption(), etc. on a document
    window, it is shown, hidden, etc. with a frame, caption, icon and
    icon text, just as you'd expect. You can provide widget flags
    which will be used for the layout of the decoration or the
    behaviour of the widget itself.

    To change or retrieve the geometry of MDI windows you must operate
    on the MDI widget's parentWidget(). (The parentWidget() provides
    access to the decorated window in which the MDI window's widget is
    shown.)

    A document window becomes active when it gets the keyboard focus.
    You can also activate a window in code using setFocus(). The user
    can activate a window by moving focus in the usual ways, for
    example by clicking a window or by pressing Tab. The workspace
    emits a signal windowActivated() when it detects the activation
    change, and the function activeWindow() always returns a pointer
    to the active document window.

    The convenience function windowList() returns a list of all
    document windows. This is useful to create a popup menu
    "<u>W</u>indows" on the fly, for example.

    TQWorkspace provides two built-in layout strategies for child
    windows: cascade() and tile(). Both are slots so you can easily
    connect menu entries to them.

    If you want your users to be able to work with document windows
    larger than the actual workspace, set the scrollBarsEnabled
    property to true.

    If the top-level window contains a menu bar and a document window
    is maximised, TQWorkspace moves the document window's minimize,
    restore and close buttons from the document window's frame to the
    workspace window's menu bar. It then inserts a window operations
    menu at the far left of the menu bar.
*/

static bool inCaptionChange = false;

class TQWorkspaceChild : public TQFrame
{
    TQ_OBJECT

    friend class TQWorkspace;
    friend class TQTitleBar;

public:
    TQWorkspaceChild( TQWidget* window,
		     TQWorkspace* parent=0, const char* name=0 );
    ~TQWorkspaceChild();

    void setActive( bool );
    bool isActive() const;

    void adjustToFullscreen();

    void setStatusBar(TQStatusBar *);
    TQWidget* windowWidget() const;
    TQWidget* iconWidget() const;

    void doResize();
    void doMove();

    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

    TQSize baseSize() const;

signals:
    void showOperationMenu();
    void popupOperationMenu( const TQPoint& );

public slots:
    void activate();
    void showMinimized();
    void showMaximized();
    void showNormal();
    void showShaded();
    void setCaption( const TQString& );
    void internalRaise();
    void titleBarDoubleClicked();

    void move( int x, int y );

protected:
    bool event(TQEvent * );
    void enterEvent( TQEvent * );
    void leaveEvent( TQEvent * );
    void childEvent( TQChildEvent* );
    void resizeEvent( TQResizeEvent * );
    void moveEvent( TQMoveEvent * );
    bool eventFilter( TQObject *, TQEvent * );

    bool focusNextPrevChild( bool );

    void drawFrame( TQPainter * );
    void styleChange( TQStyle & );

private:
    TQWidget* childWidget;
    TQGuardedPtr<TQWidget> lastfocusw;
    TQWidgetResizeHandler *widgetResizeHandler;
    TQTitleBar* titlebar;
    TQGuardedPtr<TQStatusBar> statusbar;
    TQGuardedPtr<TQTitleBar>  iconw;
    TQSize windowSize;
    TQSize shadeRestore;
    TQSize shadeRestoreMin;
    bool act		    :1;
    bool shademode	    :1;
    bool snappedRight	    :1;
    bool snappedDown	    :1;
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQWorkspaceChild( const TQWorkspaceChild & );
    TQWorkspaceChild &operator=( const TQWorkspaceChild & );
#endif
};

class TQMainWindow;

class TQWorkspacePrivate {
public:
    TQWorkspaceChild* active;
    TQPtrList<TQWorkspaceChild> windows;
    TQPtrList<TQWorkspaceChild> focus;
    TQPtrList<TQWidget> icons;
    TQWorkspaceChild* maxWindow;
    TQRect maxRestore;
    TQGuardedPtr<TQFrame> maxcontrols;
    TQGuardedPtr<TQMenuBar> maxmenubar;

    int px;
    int py;
    TQWidget *becomeActive;
    TQGuardedPtr<TQLabel> maxtools;
    TQPopupMenu* popup;
    TQPopupMenu* toolPopup;
    int menuId;
    int controlId;
    TQString topCaption;

    TQScrollBar *vbar, *hbar;
    TQWidget *corner;
    int yoffset, xoffset;

    // toplevel mdi fu
    TQWorkspace::WindowMode wmode;
    TQGuardedPtr<TQMainWindow> mainwindow;
    TQPtrList<TQDockWindow> dockwindows, newdocks;
};

static bool isChildOf( TQWidget * child, TQWidget * parent )
{
    if ( !parent || !child )
	return false;
    TQWidget * w = child;
    while( w && w != parent )
	w = w->parentWidget();
    return w != 0;
}

/*!
    Constructs a workspace with a \a parent and a \a name.
*/
TQWorkspace::TQWorkspace( TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    init();
}

#ifdef QT_WORKSPACE_WINDOWMODE
/*!
    Constructs a workspace with a \a parent and a \a name. This
    constructor will also set the WindowMode to \a mode.

    \sa windowMode()
*/
TQWorkspace::TQWorkspace( TQWorkspace::WindowMode mode, TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    init();
    d->wmode = mode;
}
#endif


/*!
    \internal
*/
void
TQWorkspace::init()
{
    d = new TQWorkspacePrivate;
    d->maxcontrols = 0;
    d->active = 0;
    d->maxWindow = 0;
    d->maxtools = 0;
    d->px = 0;
    d->py = 0;
    d->becomeActive = 0;
#if defined( QT_WORKSPACE_WINDOWMODE ) && defined( TQ_WS_MAC )
    d->wmode = AutoDetect;
#else
    d->wmode = MDI;
#endif
    d->mainwindow = 0;
#if defined(TQ_WS_WIN)
    d->popup = new TQPopupMenu( this, "qt_internal_mdi_popup" );
    d->toolPopup = new TQPopupMenu( this, "qt_internal_mdi_popup" );
#else
    d->popup = new TQPopupMenu( parentWidget(), "qt_internal_mdi_popup" );
    d->toolPopup = new TQPopupMenu( parentWidget(), "qt_internal_mdi_popup" );
#endif

    d->menuId = -1;
    d->controlId = -1;
    connect( d->popup, TQ_SIGNAL( aboutToShow() ), this, TQ_SLOT(operationMenuAboutToShow() ));
    connect( d->popup, TQ_SIGNAL( activated(int) ), this, TQ_SLOT( operationMenuActivated(int) ) );
    d->popup->insertItem(TQIconSet(style().stylePixmap(TQStyle::SP_TitleBarNormalButton)), tr("&Restore"), 1);
    d->popup->insertItem(tr("&Move"), 2);
    d->popup->insertItem(tr("&Size"), 3);
    d->popup->insertItem(TQIconSet(style().stylePixmap(TQStyle::SP_TitleBarMinButton)), tr("Mi&nimize"), 4);
    d->popup->insertItem(TQIconSet(style().stylePixmap(TQStyle::SP_TitleBarMaxButton)), tr("Ma&ximize"), 5);
    d->popup->insertSeparator();
    d->popup->insertItem(TQIconSet(style().stylePixmap(TQStyle::SP_TitleBarCloseButton)),
				  tr("&Close")
#ifndef TQT_NO_ACCEL
					+"\t"+TQAccel::keyToString(CTRL+Key_F4)
#endif
		    , this, TQ_SLOT( closeActiveWindow() ) );

    connect( d->toolPopup, TQ_SIGNAL( aboutToShow() ), this, TQ_SLOT(toolMenuAboutToShow() ));
    connect( d->toolPopup, TQ_SIGNAL( activated(int) ), this, TQ_SLOT( operationMenuActivated(int) ) );
    d->toolPopup->insertItem(tr("&Move"), 2);
    d->toolPopup->insertItem(tr("&Size"), 3);
    d->toolPopup->insertItem(tr("Stay on &Top"), 7);
    d->toolPopup->setItemChecked( 7, true );
    d->toolPopup->setCheckable( true );
    d->toolPopup->insertSeparator();
    d->toolPopup->insertItem(TQIconSet(style().stylePixmap(TQStyle::SP_TitleBarShadeButton)), tr("Sh&ade"), 6);
    d->toolPopup->insertItem(TQIconSet(style().stylePixmap(TQStyle::SP_TitleBarCloseButton)),
				      tr("&Close")
#ifndef TQT_NO_ACCEL
					+"\t"+TQAccel::keyToString( CTRL+Key_F4)
#endif
		, this, TQ_SLOT( closeActiveWindow() ) );

#ifndef TQT_NO_ACCEL
    TQAccel* a = new TQAccel( this );
    a->connectItem( a->insertItem( ALT + Key_Minus),
		    this, TQ_SLOT( showOperationMenu() ) );

    a->connectItem( a->insertItem( CTRL + Key_F6),
		    this, TQ_SLOT( activateNextWindow() ) );
    a->connectItem( a->insertItem( CTRL + Key_Tab),
		    this, TQ_SLOT( activateNextWindow() ) );
    a->connectItem( a->insertItem( Key_Forward ),
		    this, TQ_SLOT( activateNextWindow() ) );

    a->connectItem( a->insertItem( CTRL + SHIFT + Key_F6),
		    this, TQ_SLOT( activatePreviousWindow() ) );
    a->connectItem( a->insertItem( CTRL + SHIFT + Key_Tab),
		    this, TQ_SLOT( activatePreviousWindow() ) );
    a->connectItem( a->insertItem( Key_Back ),
		    this, TQ_SLOT( activatePreviousWindow() ) );

    a->connectItem( a->insertItem( CTRL + Key_F4 ),
		    this, TQ_SLOT( closeActiveWindow() ) );
#endif

    setBackgroundMode( PaletteDark );
    setSizePolicy( TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Expanding ) );

#ifndef TQT_NO_WIDGET_TOPEXTRA
    d->topCaption = topLevelWidget()->caption();
#endif

    d->hbar = d->vbar = 0;
    d->corner = 0;
    d->xoffset = d->yoffset = 0;

    updateWorkspace();

    topLevelWidget()->installEventFilter( this );
}

/*!  Destroys the workspace and frees any allocated resources. */

TQWorkspace::~TQWorkspace()
{
    delete d;
    d = 0;
}

/*!\reimp */
TQSize TQWorkspace::sizeHint() const
{
    TQSize s( TQApplication::desktop()->size() );
    return TQSize( s.width()*2/3, s.height()*2/3);
}

/*! \reimp */
void TQWorkspace::setPaletteBackgroundColor( const TQColor & c )
{
    setEraseColor( c );
}


/*! \reimp */
void TQWorkspace::setPaletteBackgroundPixmap( const TQPixmap & pm )
{
    setErasePixmap( pm );
}

/*! \reimp */
void TQWorkspace::childEvent( TQChildEvent * e)
{
    if (e->inserted() && e->child()->isWidgetType()) {
	TQWidget* w = (TQWidget*) e->child();
	if ( !w || !w->testWFlags( WStyle_Title | WStyle_NormalBorder | WStyle_DialogBorder) || w->testWFlags(WType_Dialog)
	     || d->icons.contains( w ) || w == d->vbar || w == d->hbar || w == d->corner )
	    return;	    // nothing to do

	bool wasMaximized = w->isMaximized();
	bool wasMinimized = w->isMinimized();
	bool hasBeenHidden = w->isHidden();
	bool hasSize = w->testWState( WState_Resized );
	int x = w->x();
	int y = w->y();
	bool hasPos = x != 0 || y != 0;
	TQSize s = w->size().expandedTo( w->minimumSizeHint() );
	if ( !hasSize && w->sizeHint().isValid() )
	    w->adjustSize();

	TQWorkspaceChild* child = new TQWorkspaceChild( w, this, "qt_workspacechild" );
	child->installEventFilter( this );

	connect( child, TQ_SIGNAL( popupOperationMenu(const TQPoint&) ),
		 this, TQ_SLOT( popupOperationMenu(const TQPoint&) ) );
	connect( child, TQ_SIGNAL( showOperationMenu() ),
		 this, TQ_SLOT( showOperationMenu() ) );
	d->windows.append( child );
	if ( child->isVisibleTo( this ) )
	    d->focus.append( child );
	child->internalRaise();

	if ( !hasPos )
	    place( child );
	if ( hasSize )
	    child->resize( s + child->baseSize() );
	else
	    child->adjustSize();
	if ( hasPos )
	    child->move( x, y );

	if ( hasBeenHidden )
	    w->hide();
	else if ( !isVisible() ) 	// that's a case were we don't receive a showEvent in time. Tricky.
	    child->show();

	if ( wasMaximized )
	    w->showMaximized();
	else if ( wasMinimized )
	    w->showMinimized();
        else if (!hasBeenHidden)
	    activateWindow( w );

	updateWorkspace();
    } else if (e->removed() ) {
	if ( d->windows.contains( (TQWorkspaceChild*)e->child() ) ) {
	    d->windows.removeRef( (TQWorkspaceChild*)e->child() );
	    d->focus.removeRef( (TQWorkspaceChild*)e->child() );
	    if (d->maxWindow == e->child())
		d->maxWindow = 0;
	    updateWorkspace();
	}
    }
}

/*! \reimp
*/
#ifndef TQT_NO_WHEELEVENT
void TQWorkspace::wheelEvent( TQWheelEvent *e )
{
    if ( !scrollBarsEnabled() )
	return;
    if ( d->vbar && d->vbar->isVisible() && !( e->state() & AltButton ) )
	TQApplication::sendEvent( d->vbar, e );
    else if ( d->hbar && d->hbar->isVisible() )
	TQApplication::sendEvent( d->hbar, e );
}
#endif

void TQWorkspace::activateWindow( TQWidget* w, bool change_focus )
{
    if ( !w ) {
	d->active = 0;
	emit windowActivated( 0 );
	return;
    }
    if ( d->wmode == MDI && !isVisibleTo( 0 ) ) {
	d->becomeActive = w;
	return;
    }

    if ( d->active && d->active->windowWidget() == w ) {
	if ( !isChildOf( focusWidget(), w ) ) // child window does not have focus
	    d->active->setActive( true );
	return;
    }

    d->active = 0;
    // First deactivate all other workspace clients
    TQPtrListIterator<TQWorkspaceChild> it( d->windows );
    while ( it.current () ) {
     	TQWorkspaceChild* c = it.current();
	++it;
	if(windowMode() == TQWorkspace::TopLevel && c->isTopLevel() &&
	    c->windowWidget() == w && !c->isActive())
	    c->setActiveWindow();
	if (c->windowWidget() == w)
	    d->active = c;
	else
	    c->setActive( false );
    }

    if (!d->active)
	return;

    // Then activate the new one, so the focus is stored correctly
    d->active->setActive( true );

    if (!d->active)
	return;

    if ( d->maxWindow && d->maxWindow != d->active && d->active->windowWidget() &&
	 d->active->windowWidget()->testWFlags( WStyle_MinMax ) &&
	 !d->active->windowWidget()->testWFlags( WStyle_Tool ) ) {
        d->active->showMaximized();
	if ( d->maxtools ) {
#ifndef TQT_NO_WIDGET_TOPEXTRA
	    if ( w->icon() ) {
		TQPixmap pm(*w->icon());
		int iconSize = d->maxtools->size().height();
		if(pm.width() > iconSize || pm.height() > iconSize) {
		    TQImage im;
		    im = pm;
		    pm = im.smoothScale( TQMIN(iconSize, pm.width()), TQMIN(iconSize, pm.height()) );
		}
		d->maxtools->setPixmap( pm );
	    } else
#endif
	    {
		TQPixmap pm(14,14);
		pm.fill( color1 );
		pm.setMask(pm.createHeuristicMask());
		d->maxtools->setPixmap( pm );
	    }
	}
    } else { // done implicitly in maximizeWindow otherwise        
        d->active->internalRaise();
    }

    if ( change_focus ) {
	if ( d->focus.find( d->active ) >=0 ) {
	    d->focus.removeRef( d->active );
	    d->focus.append( d->active );
	}
    }

    updateWorkspace();
    emit windowActivated( w );
}


/*!
    Returns the active window, or 0 if no window is active.
*/
TQWidget* TQWorkspace::activeWindow() const
{
    return d->active?d->active->windowWidget():0;
}


void TQWorkspace::place( TQWidget* w)
{
    TQPtrList<TQWidget> widgets;
    for(TQPtrListIterator<TQWorkspaceChild> it( d->windows ); it.current(); ++it)
	if ((*it) != w)
	    widgets.append((*it));

    if(d->wmode == TopLevel) {
	for(TQPtrListIterator<TQDockWindow> it( d->dockwindows ); it.current(); ++it)
	    if ((*it) != w)
		widgets.append((*it));
    }

    int overlap, minOverlap = 0;
    int possible;

    TQRect r1(0, 0, 0, 0);
    TQRect r2(0, 0, 0, 0);
    TQRect maxRect = rect();
    if(d->wmode == TopLevel) {
	const TQDesktopWidget *dw = tqApp->desktop();
	maxRect = dw->availableGeometry(dw->screenNumber(topLevelWidget()));
    }
    int x = maxRect.left(), y = maxRect.top();
    TQPoint wpos(maxRect.left(), maxRect.top());

    bool firstPass = true;

    do {
	if ( y + w->height() > maxRect.bottom() ) {
	    overlap = -1;
	} else if( x + w->width() > maxRect.right() ) {
	    overlap = -2;
	} else {
	    overlap = 0;

	    r1.setRect(x, y, w->width(), w->height());

	    TQWidget *l;
	    TQPtrListIterator<TQWidget> it( widgets );
	    while ( it.current () ) {
		l = it.current();
		++it;

		if ( d->maxWindow == l )
		    r2 = d->maxRestore;
		else
		    r2.setRect(l->x(), l->y(), l->width(), l->height());

		if (r2.intersects(r1)) {
		    r2.setCoords(TQMAX(r1.left(), r2.left()),
				 TQMAX(r1.top(), r2.top()),
				 TQMIN(r1.right(), r2.right()),
				 TQMIN(r1.bottom(), r2.bottom())
				 );

		    overlap += (r2.right() - r2.left()) *
			       (r2.bottom() - r2.top());
		}
	    }
	}

	if (overlap == 0) {
	    wpos = TQPoint(x, y);
	    break;
	}

	if (firstPass) {
	    firstPass = false;
	    minOverlap = overlap;
	} else if ( overlap >= 0 && overlap < minOverlap) {
	    minOverlap = overlap;
	    wpos = TQPoint(x, y);
	}

	if ( overlap > 0 ) {
	    possible = maxRect.right();
	    if ( possible - w->width() > x) possible -= w->width();

	    TQWidget *l;
	    TQPtrListIterator<TQWidget> it( widgets );
	    while ( it.current () ) {
		l = it.current();
		++it;
		if ( d->maxWindow == l )
		    r2 = d->maxRestore;
		else
		    r2.setRect(l->x(), l->y(), l->width(), l->height());

		if( ( y < r2.bottom() ) && ( r2.top() < w->height() + y ) ) {
		    if( r2.right() > x )
			possible = possible < r2.right() ?
				   possible : r2.right();

		    if( r2.left() - w->width() > x )
			possible = possible < r2.left() - w->width() ?
				   possible : r2.left() - w->width();
		}
	    }

	    x = possible;
	} else if ( overlap == -2 ) {
	    x = maxRect.left();
	    possible = maxRect.bottom();

	    if ( possible - w->height() > y ) possible -= w->height();

	    TQWidget *l;
	    TQPtrListIterator<TQWidget> it( widgets );
	    while ( it.current () ) {
		l = it.current();
		++it;
		if ( d->maxWindow == l )
		    r2 = d->maxRestore;
		else
		    r2.setRect(l->x(), l->y(), l->width(), l->height());

		if( r2.bottom() > y)
		    possible = possible < r2.bottom() ?
			       possible : r2.bottom();

		if( r2.top() - w->height() > y )
		    possible = possible < r2.top() - w->height() ?
			       possible : r2.top() - w->height();
	    }

	    y = possible;
	}
    }
    while( overlap != 0 && overlap != -1 );

#if 0
    if(windowMode() == TQWorkspace::TopLevel && wpos.y()) {
	TQPoint fr = w->topLevelWidget()->frameGeometry().topLeft(),
		r = w->topLevelWidget()->geometry().topLeft();
	wpos += TQPoint(r.x() - fr.x(), r.y() - fr.y());
    }
#endif
    w->move(wpos);
    updateWorkspace();
}


void TQWorkspace::insertIcon( TQWidget* w )
{
    if ( !w || d->icons.contains( w ) )
	return;
    d->icons.append( w );
    if (w->parentWidget() != this )
	w->reparent( this, 0, TQPoint(0,0), false);
    TQRect cr = updateWorkspace();
    int x = 0;
    int y = cr.height() - w->height();

    TQPtrListIterator<TQWidget> it( d->icons );
    while ( it.current () ) {
	TQWidget* i = it.current();
	++it;
	if ( x > 0 && x + i->width() > cr.width() ){
	    x = 0;
	    y -= i->height();
	}

	if ( i != w &&
	    i->geometry().intersects( TQRect( x, y, w->width(), w->height() ) ) )
	    x += i->width();
    }
    w->move( x, y );

    if ( isVisibleTo( parentWidget() ) ) {
	w->show();
	w->lower();
    }
    updateWorkspace();
}


void TQWorkspace::removeIcon( TQWidget* w)
{
    if ( !d->icons.contains( w ) )
	return;
    d->icons.remove( w );
    w->hide();
}


/*! \reimp  */
void TQWorkspace::resizeEvent( TQResizeEvent * )
{
    if ( d->maxWindow ) {
	d->maxWindow->adjustToFullscreen();
	if (d->maxWindow->windowWidget())
	    ((TQWorkspace*)d->maxWindow->windowWidget())->setWState( WState_Maximized );
    }

    TQRect cr = updateWorkspace();

    TQPtrListIterator<TQWorkspaceChild> it( d->windows );
    while ( it.current () ) {
	TQWorkspaceChild* c = it.current();
	++it;
	if ( c->windowWidget() && !c->windowWidget()->testWFlags( WStyle_Tool ) )
	    continue;

	int x = c->x();
	int y = c->y();
	if ( c->snappedDown )
	    y =  cr.height() - c->height();
	if ( c->snappedRight )
	    x =  cr.width() - c->width();

	if ( x != c->x() || y != c->y() )
	    c->move( x, y );
    }

}

void TQWorkspace::handleUndock(TQDockWindow *w)
{
    const TQDesktopWidget *dw = tqApp->desktop();
    TQRect maxRect = dw->availableGeometry(dw->screenNumber(d->mainwindow));
    TQPoint wpos(maxRect.left(), maxRect.top());
    int possible = 0;
    if(!::tqt_cast<TQToolBar*>(w)) {
	struct place_score { int o, x, y; } score = {0, 0, 0};
	int left = 1, x = wpos.x(), y = wpos.y();
	TQPtrListIterator<TQDockWindow> it( d->dockwindows );
	while(1) {
	    if(y + w->height() > maxRect.bottom()) {
		if(left) {
		    x = maxRect.right() - w->width();
		    y = maxRect.top();
		    left = 0;
		} else {
		    break;
		}
	    }

	    TQDockWindow *l, *nearest = NULL, *furthest;
	    for ( it.toFirst(); it.current(); ++it ) {
		l = it.current();
		if ( l != w && y == l->y() ) {
		    if(!nearest) {
			nearest = l;
		    } else if(l->x() == x) {
			nearest = l;
			break;
		    } else if(left && (l->x() - x) < (nearest->x() - x)) {
			nearest = l;
		    } else if(!left && (x - l->x()) < (x - nearest->x())) {
			nearest = l;
		    }
		}
	    }
	    TQRect r2(x, y, w->width(), w->height());
	    if(!nearest || !nearest->geometry().intersects(r2)) {
		wpos = TQPoint(x, y); //best possible outcome
		possible = 2;
		break;
	    }

	    TQDockWindow *o = NULL;
	    int overlap = 0;
	    for( it.toFirst(); it.current(); ++it ) {
		l = it.current();
		if ( l != w && l->geometry().intersects(TQRect(TQPoint(x, y), w->size()))) {
		    overlap++;
		    o = l;
		}
	    }
	    if(o && overlap == 1 && w->isVisible() && !o->isVisible()) {
		wpos = TQPoint(x, y);
		possible = 2;
		while(d->dockwindows.remove(o));
		d->newdocks.append(o);
		if(d->newdocks.count() == 1)
		    TQTimer::singleShot(0, this, TQ_SLOT(dockWindowsShow()));
		break;
	    }

	    for ( furthest = nearest, it.toFirst(); it.current(); ++it ) {
		l = it.current();
		if ( l != w && l->y() == nearest->y() &&
		     ((left  && (l->x() == nearest->x() + nearest->width())) ||
		      (!left && (l->x() + l->width() == nearest->x()) )))
		    furthest = l;
	    }
	    if(left)
		x = furthest->x() + furthest->width();
	    else
		x = furthest->x() - w->width();

	    TQPoint sc_pt(x, y);
	    place_score sc;
	    if(left)
		sc.x = (x + w->width()) * 2;
	    else
		sc.x = ((maxRect.width() - x) * 2) + 1;
	    sc.y = sc_pt.y();
	    for( sc.o = 0, it.toFirst(); it.current(); ++it ) {
		l = it.current();
		if ( l != w && l->geometry().intersects(TQRect(sc_pt, w->size())))
		    sc.o++;
	    }
	    if(maxRect.contains(TQRect(sc_pt, w->size())) &&
	       (!possible || (sc.o < score.o) ||
		((score.o || sc.o == score.o) && score.x < sc.x))) {
		wpos = sc_pt;
		score = sc;
		possible = 1;
	    }
	    y += nearest->height();
	    if(left)
		x = maxRect.x();
	    else
		x = maxRect.right() - w->width();
	}
	if(!possible || (possible == 1 && score.o)) { //fallback to less knowledgeable function
	    place(w);
	    wpos = w->pos();
	}
    }

    bool ishidden = w->isHidden();
    TQSize olds(w->size());
    if(w->place() == TQDockWindow::InDock)
	w->undock();
    w->move(wpos);
    w->resize(olds);
    if(!ishidden)
	w->show();
    else
	w->hide();
}

void TQWorkspace::dockWindowsShow()
{
    TQPtrList<TQDockWindow> lst = d->newdocks;
    d->newdocks.clear();
    for(TQPtrListIterator<TQDockWindow> dw_it(lst); (*dw_it); ++dw_it) {
	if(d->dockwindows.find((*dw_it)) != -1)
	    continue;
	handleUndock((*dw_it));
	d->dockwindows.append((*dw_it));
    }
}

/*! \reimp */
void TQWorkspace::showEvent( TQShowEvent *e )
{
    /* This is all magic, be carefull when playing with this code - this tries to allow people to
       use TQWorkspace as a high level abstraction for window management, but removes enforcement that
       TQWorkspace be used as an MDI. */
    if(d->wmode == AutoDetect) {
	d->wmode = MDI;
	TQWidget *o = topLevelWidget();
	if(::tqt_cast<TQMainWindow*>(o)) {
	    d->wmode = TopLevel;
	    const TQObjectList *c = o->children();
	    for(TQObjectListIt it(*c); it; ++it) {
		if((*it)->isWidgetType() && !((TQWidget *)(*it))->isTopLevel() &&
		   !::tqt_cast<TQHBox*>(*it) && !::tqt_cast<TQVBox*>(*it) &&
		   !::tqt_cast<TQWorkspaceChild*>(*it) && !(*it)->inherits("TQHideDock") &&
		   !::tqt_cast<TQDockArea*>(*it) && !::tqt_cast<TQWorkspace*>(*it) &&
		   !::tqt_cast<TQMenuBar*>(*it) && !::tqt_cast<TQStatusBar*>(*it)) {
		    d->wmode = MDI;
		    break;
		}
	    }
	}
    }
    if(d->wmode == TopLevel) {
	TQWidget *o = topLevelWidget();
	d->mainwindow = ::tqt_cast<TQMainWindow*>(o);
	const TQObjectList children = *o->children();
	for(TQObjectListIt it(children); it; ++it) {
	    if(!(*it)->isWidgetType())
		continue;
	    TQWidget *w = (TQWidget *)(*it);
	    if(w->isTopLevel())
		continue;
	    if(::tqt_cast<TQDockArea*>(w)) {
		const TQObjectList *dock_c = w->children();
		if(dock_c) {
		    TQPtrList<TQToolBar> tb_list;
		    for(TQObjectListIt dock_it(*dock_c); dock_it; ++dock_it) {
			if(!(*dock_it)->isWidgetType())
			    continue;
			if(::tqt_cast<TQToolBar*>(*dock_it)) {
			    tb_list.append((TQToolBar *)(*dock_it));
			} else if (::tqt_cast<TQDockWindow*>(*dock_it)) {
			    TQDockWindow *dw = (TQDockWindow*)(*dock_it);
			    dw->move(dw->mapToGlobal(TQPoint(0, 0)));
			    d->newdocks.append(dw);
			} else {
			    tqDebug("not sure what to do with %s %s", (*dock_it)->className(),
				   (*dock_it)->name());
			}
		    }
		    if(tb_list.count() == 1) {
			TQToolBar *tb = tb_list.first();
			tb->move(0, 0);
			d->newdocks.prepend(tb);
		    } else if(tb_list.count()) {
			TQDockWindow *dw = new TQDockWindow(TQDockWindow::OutsideDock,
							  w->parentWidget(),
							  TQString("TQMagicDock_") + w->name());
			dw->installEventFilter(this);
			dw->setResizeEnabled(true);
			dw->setCloseMode( TQDockWindow::Always );
			dw->setResizeEnabled(false);
#ifndef TQT_NO_WIDGET_TOPEXTRA
			dw->setCaption(o->caption());
#endif
			TQSize os(w->size());
			if(w->layout() && w->layout()->hasHeightForWidth()) {
			    w->layout()->invalidate();
			    os.setHeight(w->layout()->heightForWidth(os.width()));
			}
			if(!w->isHidden())
			    dw->show();
			w->reparent(dw, TQPoint(0, 0));
			dw->setWidget(w);
			dw->setSizePolicy(TQSizePolicy(TQSizePolicy::Minimum, TQSizePolicy::Minimum));
			dw->setGeometry(0, 0, os.width(), os.height() + dw->sizeHint().height());
			d->newdocks.prepend(dw);
			((TQDockArea*)w)->setAcceptDockWindow(dw, false);
			w->show();
		    }
		}
		w->installEventFilter(this);
	    } else if(::tqt_cast<TQStatusBar*>(w)) {
		if(activeWindow()) {
		    TQWorkspaceChild *c;
		    if ( ( c = findChild(activeWindow()) ) )
			c->setStatusBar((TQStatusBar*)w);
		}
	    } else if(::tqt_cast<TQWorkspaceChild*>(w)) {
		w->reparent(this, w->testWFlags(~(0)) | WType_TopLevel, w->pos());
	    }
	}
	dockWindowsShow(); //now place and undock windows discovered

	TQWidget *w = new TQWidget(NULL, "TQDoesNotExist",
				 WType_Dialog | WStyle_Customize | WStyle_NoBorder);
//	if(tqApp->mainWidget() == o)
//	    TQObject::connect(tqApp, TQ_SIGNAL(lastWindowClosed()), tqApp, TQ_SLOT(quit()));
	TQDesktopWidget *dw = TQApplication::desktop();
	w->setGeometry(dw->availableGeometry(dw->screenNumber(o)));
	o->reparent(w, TQPoint(0, 0), true);
	{
	    TQMenuBar *mb = 0;
	    if(::tqt_cast<TQMainWindow*>(o))
		mb = ((TQMainWindow *)o)->menuBar();
	    if(!mb)
		mb = (TQMenuBar*)o->child(NULL, "TQMenuBar");
	    if(mb)
		mb->reparent(w, TQPoint(0, 0));
	}
	reparent(w, TQPoint(0,0));
	setGeometry(0, 0, w->width(), w->height());
#if 0
	/* Hide really isn't acceptable because I need to make the rest of TQt
	   think it is visible, so instead I set it to an empty mask. I'm not
	   sure what problems this is going to create, hopefully everything will
	   be happy (or not even notice since this is mostly intended for TQt/Mac) */
//	w->setMask(TQRegion());
//	w->show();
#else
	w->hide();
#endif
    }

    //done with that nastiness, on with your regularly schedueled programming..
    if ( d->maxWindow && !style().styleHint(TQStyle::SH_Workspace_FillSpaceOnMaximize, this))
	showMaximizeControls();
    TQWidget::showEvent( e );
    if ( d->becomeActive ) {
	activateWindow( d->becomeActive );
	d->becomeActive = 0;
    } else if ( d->windows.count() > 0 && !d->active ) {
	activateWindow( d->windows.first()->windowWidget() );
    }

    // force a frame repaint - this is a workaround for what seems to be a bug
    // introduced when changing the TQWidget::show() implementation. Might be
    // a windows bug as well though.
    for (TQPtrListIterator<TQWorkspaceChild> it( d->windows ); it.current(); ++it ) {
	TQWorkspaceChild* c = it.current();
        TQApplication::postEvent(c, new TQPaintEvent(c->rect(), true));
    }

    updateWorkspace();
}

/*! \reimp */
void TQWorkspace::hideEvent( TQHideEvent * )
{
    if ( !isVisibleTo(0) && !style().styleHint(TQStyle::SH_Workspace_FillSpaceOnMaximize, this))
	hideMaximizeControls();
}

void TQWorkspace::minimizeWindow( TQWidget* w)
{
    TQWorkspaceChild* c = findChild( w );

    if ( !w || ( ( w && (!w->testWFlags( WStyle_Minimize ) ) ) || w->testWFlags( WStyle_Tool) ) )
	return;

    if ( c ) {
	TQWorkspace *fake = (TQWorkspace*)w;

	setUpdatesEnabled( false );
	bool wasMax = false;
	if ( c == d->maxWindow ) {
	    wasMax = true;
	    d->maxWindow = 0;
	    inCaptionChange = true;
#ifndef TQT_NO_WIDGET_TOPEXTRA
	    if ( !!d->topCaption )
		topLevelWidget()->setCaption( d->topCaption );
#endif
	    inCaptionChange = false;
	    if ( !style().styleHint(TQStyle::SH_Workspace_FillSpaceOnMaximize, this) )
		hideMaximizeControls();
	    for (TQPtrListIterator<TQWorkspaceChild> it( d->windows ); it.current(); ++it ) {
		TQWorkspaceChild* c = it.current();
		if ( c->titlebar )
		    c->titlebar->setMovable( true );
		c->widgetResizeHandler->setActive( true );
	    }
	}

	insertIcon( c->iconWidget() );
	c->hide();
	if ( wasMax )
	    c->setGeometry( d->maxRestore );
	d->focus.append( c );

        activateWindow(w);

	setUpdatesEnabled( true );
	updateWorkspace();

	fake->clearWState( WState_Maximized );
	fake->setWState( WState_Minimized );
	c->clearWState( WState_Maximized );
	c->setWState( WState_Minimized );
    }
}

void TQWorkspace::normalizeWindow( TQWidget* w)
{
    TQWorkspaceChild* c = findChild( w );
    if ( !w )
	return;
    if ( c ) {
	TQWorkspace *fake = (TQWorkspace*)w;
	fake->clearWState( WState_Minimized | WState_Maximized );
	if ( !style().styleHint(TQStyle::SH_Workspace_FillSpaceOnMaximize, this) && d->maxWindow ) {
	    hideMaximizeControls();
	} else {
	    if ( w->minimumSize() != w->maximumSize() )
		c->widgetResizeHandler->setActive( true );
	    if ( c->titlebar )
		c->titlebar->setMovable(true);
	}
	fake->clearWState( WState_Minimized | WState_Maximized );
	c->clearWState( WState_Minimized | WState_Maximized );

	if ( c == d->maxWindow ) {
	    c->setGeometry( d->maxRestore );
	    d->maxWindow = 0;
#ifndef TQT_NO_WIDGET_TOPEXTRA
	    inCaptionChange = true;
	    if ( !!d->topCaption )
		topLevelWidget()->setCaption( d->topCaption );
	    inCaptionChange = false;
#endif
	} else {
	    if ( c->iconw )
		removeIcon( c->iconw->parentWidget() );
	    c->show();
	}

	if ( !style().styleHint(TQStyle::SH_Workspace_FillSpaceOnMaximize, this))
	    hideMaximizeControls();
	for (TQPtrListIterator<TQWorkspaceChild> it( d->windows ); it.current(); ++it ) {
	    TQWorkspaceChild* c = it.current();
	    if ( c->titlebar )
		c->titlebar->setMovable( true );
	    if ( c->childWidget && c->childWidget->minimumSize() != c->childWidget->maximumSize() )
		c->widgetResizeHandler->setActive( true );
	}
	activateWindow( w, true );
	updateWorkspace();
    }
}

void TQWorkspace::maximizeWindow( TQWidget* w)
{
    TQWorkspaceChild* c = findChild( w );

    if ( !w || ( ( w && (!w->testWFlags( WStyle_Maximize ) ) ) || w->testWFlags( WStyle_Tool) ) )
	return;

    if ( c ) {
	setUpdatesEnabled( false );
	if (c->iconw && d->icons.contains( c->iconw->parentWidget() ) )
	    normalizeWindow( w );
	TQWorkspace *fake = (TQWorkspace*)w;

	TQRect r( c->geometry() );
	c->adjustToFullscreen();
	c->show();
	c->internalRaise();
	tqApp->sendPostedEvents( c, TQEvent::Resize );
	tqApp->sendPostedEvents( c, TQEvent::Move );
	tqApp->sendPostedEvents( c, TQEvent::ShowWindowRequest );
	if ( d->maxWindow != c ) {
	    if ( d->maxWindow )
		d->maxWindow->setGeometry( d->maxRestore );
	    d->maxWindow = c;
	    d->maxRestore = r;
	}

	activateWindow( w );
	if(!style().styleHint(TQStyle::SH_Workspace_FillSpaceOnMaximize, this)) {
	    showMaximizeControls();
	} else {
	    c->widgetResizeHandler->setActive( false );
	    if ( c->titlebar )
		c->titlebar->setMovable( false );
	}
#ifndef TQT_NO_WIDGET_TOPEXTRA
	inCaptionChange = true;
	if ( !!d->topCaption )
	    topLevelWidget()->setCaption( tr("%1 - [%2]")
		.arg(d->topCaption).arg(c->caption()) );
	inCaptionChange = false;
#endif
	setUpdatesEnabled( true );

	updateWorkspace();

	fake->clearWState( WState_Minimized );
	fake->setWState( WState_Maximized );
	c->clearWState( WState_Minimized );
	c->setWState( WState_Maximized );
    }
}

void TQWorkspace::showWindow( TQWidget* w)
{
    if ( d->maxWindow && w->testWFlags( WStyle_Maximize ) && !w->testWFlags( WStyle_Tool) )
	maximizeWindow(w);
    else if (w->isMinimized() && !w->testWFlags(WStyle_Tool))
	minimizeWindow(w);
    else if ( !w->testWFlags( WStyle_Tool ) )
	normalizeWindow(w);
    else
	w->parentWidget()->show();
    if ( d->maxWindow )
	d->maxWindow->internalRaise();
    updateWorkspace();
}


TQWorkspaceChild* TQWorkspace::findChild( TQWidget* w)
{
    TQPtrListIterator<TQWorkspaceChild> it( d->windows );
    while ( it.current () ) {
	TQWorkspaceChild* c = it.current();
	++it;
	if (c->windowWidget() == w)
	    return c;
    }
    return 0;
}

/*!
  \obsolete
  \overload
 */
TQWidgetList TQWorkspace::windowList() const
{
    return windowList( CreationOrder );
}

/*!
    Returns a list of all windows. If \a order is CreationOrder
    (the default) the windows are listed in the order in which they
    had been inserted into the workspace. If \a order is StackingOrder
    the windows are listed in their stacking order, with the topmost window
    being the last window in the list.

    TQWidgetList is the same as TQPtrList<TQWidget>.

    \sa TQPtrList
*/
TQWidgetList TQWorkspace::windowList( WindowOrder order ) const
{
    TQWidgetList windows;
    if ( order == StackingOrder ) {
	const TQObjectList *cl = children();
	if ( cl ) {
	    TQObjectListIt it( *cl );
	    while (it.current()) {
		TQObject *o = it.current();
		++it;
		TQWorkspaceChild *c = ::tqt_cast<TQWorkspaceChild*>(o);
		if (c && c->windowWidget())
		    windows.append(c->windowWidget());
	    }
	}
    } else {
	TQPtrListIterator<TQWorkspaceChild> it( d->windows );
	while (it.current()) {
	    TQWorkspaceChild* c = it.current();
	    ++it;
	    if ( c->windowWidget() )
		windows.append( c->windowWidget() );
	}
    }
    return windows;
}

/*!\reimp*/
bool TQWorkspace::eventFilter( TQObject *o, TQEvent * e)
{
    if(d->wmode == TopLevel && d->mainwindow && o->parent() == d->mainwindow) {
	if((e->type() == TQEvent::ChildInserted || e->type() == TQEvent::Reparent) &&
	    ::tqt_cast<TQDockArea*>(o) && !((TQWidget*)o)->isVisible()) {
	    TQChildEvent *ce = (TQChildEvent*)e;
	    if(!::tqt_cast<TQDockWindow*>(ce->child())) {
		tqDebug("No idea what to do..");
		return false;
	    }
	    TQDockWindow *w = (TQDockWindow*)ce->child();
	    if(d->newdocks.find(w) == -1 && d->dockwindows.find(w) == -1) {
		if(::tqt_cast<TQToolBar*>(w))
		    d->newdocks.prepend(w);
		else
		    d->newdocks.append(w);
		if(d->newdocks.count() == 1)
		    TQTimer::singleShot(0, this, TQ_SLOT(dockWindowsShow()));
	    }
	} else if(e->type() == TQEvent::Hide && !e->spontaneous() && !tqstrncmp(o->name(), "TQMagicDock_", 11)) {
//	    d->mainwindow->close();
	}
	return TQWidget::eventFilter(o, e);
    }

    static TQTime* t = 0;
    static TQWorkspace* tc = 0;
#ifndef TQT_NO_MENUBAR
    if ( o == d->maxtools && d->menuId != -1 ) {
	switch ( e->type() ) {
	case TQEvent::MouseButtonPress:
	    {
		TQMenuBar* b = (TQMenuBar*)o->parent();
		if ( !t )
		    t = new TQTime;
		if ( tc != this || t->elapsed() > TQApplication::doubleClickInterval() ) {
		    if ( TQApplication::reverseLayout() ) {
			TQPoint p = b->mapToGlobal( TQPoint( b->x() + b->width(), b->y() + b->height() ) );
			p.rx() -= d->popup->sizeHint().width();
			popupOperationMenu( p );
		    } else {
			popupOperationMenu( b->mapToGlobal( TQPoint( b->x(), b->y() + b->height() ) ) );
		    }
		    t->start();
		    tc = this;
		} else {
		    tc = 0;
		    closeActiveWindow();
		}
		return true;
	    }
	default:
	    break;
	}
	return TQWidget::eventFilter( o, e );
    }
#endif
    switch ( e->type() ) {
    case TQEvent::Hide:
    case TQEvent::HideToParent:
	if ( !o->isA( "TQWorkspaceChild" ) || !isVisible() || e->spontaneous() )
	    break;
	if ( d->active == o ) {
	    int a = d->focus.find( d->active );
	    for ( ;; ) {
		if ( --a < 0 )
		    a = d->focus.count()-1;
		TQWorkspaceChild* c = d->focus.at( a );
		if ( !c || c == o ) {
		    if ( c && c->iconw && d->icons.contains( c->iconw->parentWidget() ) )
			break;
		    activateWindow( 0 );
		    break;
		}
		if ( c->isShown() ) {
		    activateWindow( c->windowWidget(), false );
		    break;
		}
	    }
	}
	d->focus.removeRef( (TQWorkspaceChild*)o );
	if ( d->maxWindow == o && d->maxWindow->isHidden() ) {
	    d->maxWindow->setGeometry( d->maxRestore );
	    d->maxWindow = 0;
	    if ( d->active )
		maximizeWindow( d->active );

	    if ( !d->maxWindow ) {

		if ( style().styleHint(TQStyle::SH_Workspace_FillSpaceOnMaximize, this)) {
		    TQWorkspaceChild *wc = (TQWorkspaceChild *)o;
		    wc->widgetResizeHandler->setActive( true );
		    if ( wc->titlebar )
			wc->titlebar->setMovable( true );
		} else {
		    hideMaximizeControls();
		}
#ifndef TQT_NO_WIDGET_TOPEXTRA
		inCaptionChange = true;
		if ( !!d->topCaption )
		    topLevelWidget()->setCaption( d->topCaption );
		inCaptionChange = false;
#endif
	    }
	}
	updateWorkspace();
	break;
    case TQEvent::Show:
	if ( o->isA("TQWorkspaceChild") && !d->focus.containsRef( (TQWorkspaceChild*)o ) )
	    d->focus.append( (TQWorkspaceChild*)o );
	updateWorkspace();
	break;
    case TQEvent::CaptionChange:
	if ( inCaptionChange )
	    break;

#ifndef TQT_NO_WIDGET_TOPEXTRA
	inCaptionChange = true;
	if ( o == topLevelWidget() ) {
	    TQWidget *tlw = (TQWidget*)o;
	    if ( !d->maxWindow
		|| tlw->caption() != tr("%1 - [%2]").arg(d->topCaption).arg(d->maxWindow->caption()) )
		d->topCaption = tlw->caption();
	}

	if ( d->maxWindow && !!d->topCaption )
	    topLevelWidget()->setCaption( tr("%1 - [%2]")
		.arg(d->topCaption).arg(d->maxWindow->caption()));
	inCaptionChange = false;
#endif

	break;
    case TQEvent::Close:
	if ( o == topLevelWidget() )
	{
	    TQPtrListIterator<TQWorkspaceChild> it( d->windows );
	    while ( it.current () ) {
		TQWorkspaceChild* c = it.current();
		++it;
		if ( c->shademode )
		    c->showShaded();
	    }
	} else if ( ::tqt_cast<TQWorkspaceChild*>(o) ) {
	    d->popup->hide();
	}
	updateWorkspace();
	break;
    default:
	break;
    }
    return TQWidget::eventFilter( o, e);
}

void TQWorkspace::showMaximizeControls()
{
#ifndef TQT_NO_MENUBAR
    Q_ASSERT(d->maxWindow);
    TQMenuBar* b = 0;

    // Do a breadth-first search first on every parent,
    TQWidget* w = parentWidget();
    TQObjectList * l = 0;
    while ( !l && w ) {
	l = w->queryList( "TQMenuBar", 0, false, false );
	w = w->parentWidget();
	if ( l && !l->count() ) {
	    delete l;
	    l = 0;
	}
    }

    // and query recursively if nothing is found.
    if ( !l || !l->count() ) {
	if ( l )
	    delete l;
	l = topLevelWidget()->queryList( "TQMenuBar", 0, 0, true );
    }
    if ( l && l->count() )
	b = (TQMenuBar *)l->first();
    delete l;

    if ( !b )
	return;

    if ( !d->maxcontrols ) {
	d->maxmenubar = b;
	d->maxcontrols = new TQFrame( topLevelWidget(), "qt_maxcontrols" );
	TQHBoxLayout* l = new TQHBoxLayout( d->maxcontrols,
					  d->maxcontrols->frameWidth(), 0 );
	if ( d->maxWindow->windowWidget() &&
	     d->maxWindow->windowWidget()->testWFlags(WStyle_Minimize) ) {
	    TQToolButton* iconB = new TQToolButton( d->maxcontrols, "iconify" );
#ifndef TQT_NO_TOOLTIP
	    TQToolTip::add( iconB, tr( "Minimize" ) );
#endif
	    l->addWidget( iconB );
	    iconB->setFocusPolicy( NoFocus );
	    iconB->setIconSet(style().stylePixmap(TQStyle::SP_TitleBarMinButton));
	    iconB->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	    connect( iconB, TQ_SIGNAL( clicked() ),
		     this, TQ_SLOT( minimizeActiveWindow() ) );
	}

	TQToolButton* restoreB = new TQToolButton( d->maxcontrols, "restore" );
#ifndef TQT_NO_TOOLTIP
	TQToolTip::add( restoreB, tr( "Restore Down" ) );
#endif
	l->addWidget( restoreB );
	restoreB->setFocusPolicy( NoFocus );
	restoreB->setIconSet( style().stylePixmap(TQStyle::SP_TitleBarNormalButton));
	restoreB->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	connect( restoreB, TQ_SIGNAL( clicked() ),
		 this, TQ_SLOT( normalizeActiveWindow() ) );

	l->addSpacing( 2 );
	TQToolButton* closeB = new TQToolButton( d->maxcontrols, "close" );
#ifndef TQT_NO_TOOLTIP
	TQToolTip::add( closeB, tr( "Close" ) );
#endif
	l->addWidget( closeB );
	closeB->setFocusPolicy( NoFocus );
	closeB->setIconSet( style().stylePixmap(TQStyle::SP_TitleBarCloseButton) );
	closeB->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	connect( closeB, TQ_SIGNAL( clicked() ),
		 this, TQ_SLOT( closeActiveWindow() ) );

	d->maxcontrols->setFixedSize( d->maxcontrols->minimumSizeHint() );
    }

    if ( d->controlId == -1 || b->indexOf( d->controlId ) == -1 ) {
	TQFrame* dmaxcontrols = d->maxcontrols;
	d->controlId = b->insertItem( dmaxcontrols, -1, b->count() );
    }
    if ( !d->active && d->becomeActive ) {
	d->active = (TQWorkspaceChild*)d->becomeActive->parentWidget();
	d->active->setActive( true );
	d->becomeActive = 0;
	emit windowActivated( d->active->windowWidget() );
    }
    if ( d->active && ( d->menuId == -1 || b->indexOf( d->menuId ) == -1 ) ) {
	if ( !d->maxtools ) {
	    d->maxtools = new TQLabel( topLevelWidget(), "qt_maxtools" );
	    d->maxtools->installEventFilter( this );
	}
#ifndef TQT_NO_WIDGET_TOPEXTRA
	if ( d->active->windowWidget() && d->active->windowWidget()->icon() ) {
	    TQPixmap pm(*d->active->windowWidget()->icon());
	    int iconSize = d->maxcontrols->size().height();
	    if(pm.width() > iconSize || pm.height() > iconSize) {
		TQImage im;
		im = pm;
		pm = im.smoothScale( TQMIN(iconSize, pm.width()), TQMIN(iconSize, pm.height()) );
	    }
	    d->maxtools->setPixmap( pm );
	} else
#endif
	{
	    TQPixmap pm(14,14);
	    pm.fill( color1 );
	    pm.setMask(pm.createHeuristicMask());
	    d->maxtools->setPixmap( pm );
	}
	d->menuId = b->insertItem( d->maxtools, -1, 0 );
    }
#endif
}


void TQWorkspace::hideMaximizeControls()
{
#ifndef TQT_NO_MENUBAR
    if ( d->maxmenubar ) {
	int mi = d->menuId;
	if ( mi != -1 ) {
	    if ( d->maxmenubar->indexOf( mi ) != -1 )
		d->maxmenubar->removeItem( mi );
	    d->maxtools = 0;
	}
	int ci = d->controlId;
	if ( ci != -1 && d->maxmenubar->indexOf( ci ) != -1 )
	    d->maxmenubar->removeItem( ci );
    }
    d->maxcontrols = 0;
    d->menuId = -1;
    d->controlId = -1;
#endif
}

/*!
    Closes the child window that is currently active.

    \sa closeAllWindows()
*/
void TQWorkspace::closeActiveWindow()
{
    setUpdatesEnabled( false );
    if ( d->maxWindow && d->maxWindow->windowWidget() )
	d->maxWindow->windowWidget()->close();
    else if ( d->active && d->active->windowWidget() )
	d->active->windowWidget()->close();
    setUpdatesEnabled( true );
    updateWorkspace();
}

/*!
    Closes all child windows.

    The windows are closed in random order. The operation stops if a
    window does not accept the close event.

    \sa closeActiveWindow()
*/
void TQWorkspace::closeAllWindows()
{
    bool did_close = true;
    TQPtrListIterator<TQWorkspaceChild> it( d->windows );
    TQWorkspaceChild *c = 0;
    while ( ( c = it.current() ) && did_close ) {
	++it;
	if ( c->windowWidget() )
	    did_close = c->windowWidget()->close();
    }
}

void TQWorkspace::normalizeActiveWindow()
{
    if  ( d->maxWindow )
	d->maxWindow->showNormal();
    else if ( d->active )
	d->active->showNormal();
}

void TQWorkspace::minimizeActiveWindow()
{
    if ( d->maxWindow )
	d->maxWindow->showMinimized();
    else if ( d->active )
	d->active->showMinimized();
}

void TQWorkspace::showOperationMenu()
{
    if  ( !d->active || !d->active->windowWidget() )
	return;
    Q_ASSERT( d->active->windowWidget()->testWFlags( WStyle_SysMenu ) );
    TQPoint p;
    TQPopupMenu *popup = d->active->windowWidget()->testWFlags( WStyle_Tool ) ? d->toolPopup : d->popup;
    if ( TQApplication::reverseLayout() ) {
	p = TQPoint( d->active->windowWidget()->mapToGlobal( TQPoint(d->active->windowWidget()->width(),0) ) );
	p.rx() -= popup->sizeHint().width();
    } else {
	p = TQPoint( d->active->windowWidget()->mapToGlobal( TQPoint(0,0) ) );
    }
    if ( !d->active->isVisible() ) {
	p = d->active->iconWidget()->mapToGlobal( TQPoint(0,0) );
	p.ry() -= popup->sizeHint().height();
    }
    popupOperationMenu( p );
}

void TQWorkspace::popupOperationMenu( const TQPoint&  p)
{
    if ( !d->active || !d->active->windowWidget() || !d->active->windowWidget()->testWFlags( WStyle_SysMenu ) )
	return;
    if ( d->active->windowWidget()->testWFlags( WStyle_Tool ))
	d->toolPopup->popup( p );
    else
	d->popup->popup( p );
}

void TQWorkspace::operationMenuAboutToShow()
{
    for ( int i = 1; i < 6; i++ ) {
	bool enable = d->active != 0;
	d->popup->setItemEnabled( i, enable );
    }

    if ( !d->active || !d->active->windowWidget() )
	return;

    TQWidget *windowWidget = d->active->windowWidget();
    bool canResize = windowWidget->maximumSize() != windowWidget->minimumSize();
    d->popup->setItemEnabled( 3, canResize );
    d->popup->setItemEnabled( 4, windowWidget->testWFlags( WStyle_Minimize ) );
    d->popup->setItemEnabled( 5, windowWidget->testWFlags( WStyle_Maximize ) && canResize );

    if ( d->active == d->maxWindow ) {
	d->popup->setItemEnabled( 2, false );
	d->popup->setItemEnabled( 3, false );
	d->popup->setItemEnabled( 5, false );
    } else if ( d->active->isVisible() ){
	d->popup->setItemEnabled( 1, false );
    } else {
	d->popup->setItemEnabled( 2, false );
	d->popup->setItemEnabled( 3, false );
	d->popup->setItemEnabled( 4, false );
    }
}

void TQWorkspace::toolMenuAboutToShow()
{
    if ( !d->active || !d->active->windowWidget() )
	return;

    TQWidget *windowWidget = d->active->windowWidget();
    bool canResize = windowWidget->maximumSize() != windowWidget->minimumSize();

    d->toolPopup->setItemEnabled( 3, !d->active->shademode && canResize );
    if ( d->active->shademode )
	d->toolPopup->changeItem( 6,
				  TQIconSet(style().stylePixmap(TQStyle::SP_TitleBarUnshadeButton)), tr("&Unshade") );
    else
	d->toolPopup->changeItem( 6, TQIconSet(style().stylePixmap(TQStyle::SP_TitleBarShadeButton)), tr("Sh&ade") );
    d->toolPopup->setItemEnabled( 6, d->active->windowWidget()->testWFlags( WStyle_MinMax ) );
    d->toolPopup->setItemChecked( 7, d->active->windowWidget()->testWFlags( WStyle_StaysOnTop ) );
}

void TQWorkspace::operationMenuActivated( int a )
{
    if ( !d->active )
	return;
    switch ( a ) {
    case 1:
	d->active->showNormal();
	break;
    case 2:
	d->active->doMove();
	break;
    case 3:
	if ( d->active->shademode )
	    d->active->showShaded();
	d->active->doResize();
	break;
    case 4:
	d->active->showMinimized();
	break;
    case 5:
	d->active->showMaximized();
	break;
    case 6:
	d->active->showShaded();
	break;
    case 7:
	{
	    TQWorkspace* w = (TQWorkspace*)d->active->windowWidget();
	    if ( !w )
		break;
	    if ( w->testWFlags( WStyle_StaysOnTop ) ) {
		w->clearWFlags( WStyle_StaysOnTop );
	    } else {
		w->setWFlags( WStyle_StaysOnTop );
		w->parentWidget()->raise();
	    }
	}
	break;
    default:
	break;
    }
}

/*!
    Activates the next window in the child window chain.

    \sa activatePrevWindow()
*/
void TQWorkspace::activateNextWindow()
{
    if ( d->focus.isEmpty() )
	return;
    if ( !d->active ) {
	if ( d->focus.first() )
	    activateWindow( d->focus.first()->windowWidget(), false );
	return;
    }

    int a = d->focus.find( d->active ) + 1;

    a = a % d->focus.count();

    if ( d->focus.at( a ) )
	activateWindow( d->focus.at( a )->windowWidget(), false );
    else
	activateWindow(0);
}

void TQWorkspace::activatePreviousWindow()
{
    activatePrevWindow();
}

/*!
    Activates the previous window in the child window chain.

    \sa activateNextWindow()
*/
void TQWorkspace::activatePrevWindow()
{
    if ( d->focus.isEmpty() )
	return;
    if ( !d->active ) {
	if ( d->focus.last() )
	    activateWindow( d->focus.first()->windowWidget(), false );
	else
	    activateWindow( 0 );

	return;
    }

    int a = d->focus.find( d->active ) - 1;
    if ( a < 0 )
	a = d->focus.count()-1;

    if ( d->focus.at( a ) )
	activateWindow( d->focus.at( a )->windowWidget(), false );
    else
	activateWindow( 0 );
}


/*!
  \fn void TQWorkspace::windowActivated( TQWidget* w )

  This signal is emitted when the window widget \a w becomes active.
  Note that \a w can be null, and that more than one signal may be
  emitted for a single activation event.

  \sa activeWindow(), windowList()
*/



/*!
    Arranges all the child windows in a cascade pattern.

    \sa tile()
*/
void TQWorkspace::cascade()
{
    blockSignals(true);
    if  ( d->maxWindow )
	d->maxWindow->showNormal();

    if ( d->vbar ) {
	d->vbar->blockSignals( true );
	d->vbar->setValue( 0 );
	d->vbar->blockSignals( false );
	d->hbar->blockSignals( true );
	d->hbar->setValue( 0 );
	d->hbar->blockSignals( false );
	scrollBarChanged();
    }

    const int xoffset = 13;
    const int yoffset = 20;

    // make a list of all relevant mdi clients
    TQPtrList<TQWorkspaceChild> widgets;
    TQWorkspaceChild* wc = 0;
    for ( wc = d->focus.first(); wc; wc = d->focus.next() )
	if ( wc->windowWidget()->isVisibleTo( this ) && !wc->windowWidget()->testWFlags( WStyle_Tool ) && !wc->iconw )
	    widgets.append( wc );

    int x = 0;
    int y = 0;

    setUpdatesEnabled( false );
    TQPtrListIterator<TQWorkspaceChild> it( widgets );
    while ( it.current () ) {
	TQWorkspaceChild *child = it.current();
	++it;
	child->setUpdatesEnabled( false );
	TQSize prefSize = child->windowWidget()->sizeHint().expandedTo( child->windowWidget()->minimumSizeHint() );

	if ( !prefSize.isValid() )
	    prefSize = child->windowWidget()->size();
	prefSize = prefSize.expandedTo( child->windowWidget()->minimumSize() ).boundedTo( child->windowWidget()->maximumSize() );
	if (prefSize.isValid())
	    prefSize += TQSize( child->baseSize().width(), child->baseSize().height() );

	int w = prefSize.width();
	int h = prefSize.height();

	child->showNormal();
	tqApp->sendPostedEvents( 0, TQEvent::ShowNormal );
	if ( y + h > height() )
	    y = 0;
	if ( x + w > width() )
	    x = 0;
	child->setGeometry( x, y, w, h );
	x += xoffset;
	y += yoffset;
	child->internalRaise();
	child->setUpdatesEnabled( true );
    }
    setUpdatesEnabled( true );
    updateWorkspace();
    blockSignals(false);
}

/*!
    Arranges all child windows in a tile pattern.

    \sa cascade()
*/
void TQWorkspace::tile()
{
    blockSignals(true);
    TQWidget *oldActive = d->active ? d->active->windowWidget() : 0;
    if  ( d->maxWindow )
	d->maxWindow->showNormal();

    if ( d->vbar ) {
	d->vbar->blockSignals( true );
	d->vbar->setValue( 0 );
	d->vbar->blockSignals( false );
	d->hbar->blockSignals( true );
	d->hbar->setValue( 0 );
	d->hbar->blockSignals( false );
	scrollBarChanged();
    }

    int rows = 1;
    int cols = 1;
    int n = 0;
    TQWorkspaceChild* c;

    TQPtrListIterator<TQWorkspaceChild> it( d->windows );
    while ( it.current () ) {
	c = it.current();
	++it;
	if ( !c->windowWidget()->isHidden() &&
	     !c->windowWidget()->testWFlags( WStyle_StaysOnTop ) &&
	     !c->windowWidget()->testWFlags( WStyle_Tool ) &&
             !c->iconw )
	    n++;
    }

    while ( rows * cols < n ) {
	if ( cols <= rows )
	    cols++;
	else
	    rows++;
    }
    int add = cols * rows - n;
    bool* used = new bool[ cols*rows ];
    for ( int i = 0; i < rows*cols; i++ )
	used[i] = false;

    int row = 0;
    int col = 0;
    int w = width() / cols;
    int h = height() / rows;

    it.toFirst();
    while ( it.current () ) {
	c = it.current();
	++it;
	if ( c->iconw || c->windowWidget()->isHidden() || c->windowWidget()->testWFlags( WStyle_Tool ) )
	    continue;
        if (!row && !col) {
            w -= c->baseSize().width();
            h -= c->baseSize().height();
        }
	if ( c->windowWidget()->testWFlags( WStyle_StaysOnTop ) ) {
	    TQPoint p = c->pos();
	    if ( p.x()+c->width() < 0 )
		p.setX( 0 );
	    if ( p.x() > width() )
		p.setX( width() - c->width() );
	    if ( p.y() + 10 < 0 )
		p.setY( 0 );
	    if ( p.y() > height() )
		p.setY( height() - c->height() );

	    if ( p != c->pos() )
		c->TQFrame::move( p );
	} else {
	    c->showNormal();
	    tqApp->sendPostedEvents( 0, TQEvent::ShowNormal );
	    used[row*cols+col] = true;
            TQSize sz(w, h);
            TQSize bsize(c->baseSize());
            sz = sz.expandedTo(c->windowWidget()->minimumSize()).boundedTo(c->windowWidget()->maximumSize());
            sz += bsize;

	    if ( add ) {
                if (sz.height() == h + bsize.height()) // no relevant constrains
                    sz.rheight() *= 2;
		c->setGeometry(col*w + col*bsize.width(), row*h + row*bsize.height(), sz.width(), sz.height());
		used[(row+1)*cols+col] = true;
		add--;
	    } else {
		c->setGeometry(col*w + col*bsize.width(), row*h + row*bsize.height(), sz.width(), sz.height());
	    }
	    while( row < rows && col < cols && used[row*cols+col] ) {
		col++;
		if ( col == cols ) {
		    col = 0;
		    row++;
		}
	    }
        }
    }
    delete [] used;

    activateWindow( oldActive );
    updateWorkspace();
    blockSignals(false);
}

TQWorkspaceChild::TQWorkspaceChild( TQWidget* window, TQWorkspace *parent,
				  const char *name )
    : TQFrame( parent, name,
	      (parent->windowMode() == TQWorkspace::TopLevel ? (WStyle_MinMax | WStyle_SysMenu | WType_TopLevel) :
	       WStyle_NoBorder ) | WStyle_Customize | WDestructiveClose | WNoMousePropagation | WSubWindow )
{
    statusbar = 0;
    setMouseTracking( true );
    act = false;
    iconw = 0;
    lastfocusw = 0;
    shademode = false;
    titlebar = 0;
    snappedRight = false;
    snappedDown = false;

    if (window) {
	switch (window->focusPolicy()) {
	case TQWidget::NoFocus:
	    window->setFocusPolicy(TQWidget::ClickFocus);
	    break;
	case TQWidget::TabFocus:
	    window->setFocusPolicy(TQWidget::StrongFocus);
	    break;
	default:
	    break;
	}
    }

    if ( window && window->testWFlags( WStyle_Title ) && parent->windowMode() != TQWorkspace::TopLevel ) {
	titlebar = new TQTitleBar( window, this, "qt_ws_titlebar" );
	connect( titlebar, TQ_SIGNAL( doActivate() ),
		 this, TQ_SLOT( activate() ) );
	connect( titlebar, TQ_SIGNAL( doClose() ),
		 window, TQ_SLOT( close() ) );
	connect( titlebar, TQ_SIGNAL( doMinimize() ),
		 this, TQ_SLOT( showMinimized() ) );
	connect( titlebar, TQ_SIGNAL( doNormal() ),
		 this, TQ_SLOT( showNormal() ) );
	connect( titlebar, TQ_SIGNAL( doMaximize() ),
		 this, TQ_SLOT( showMaximized() ) );
	connect( titlebar, TQ_SIGNAL( popupOperationMenu(const TQPoint&) ),
		 this, TQ_SIGNAL( popupOperationMenu(const TQPoint&) ) );
	connect( titlebar, TQ_SIGNAL( showOperationMenu() ),
		 this, TQ_SIGNAL( showOperationMenu() ) );
	connect( titlebar, TQ_SIGNAL( doShade() ),
		 this, TQ_SLOT( showShaded() ) );
	connect( titlebar, TQ_SIGNAL( doubleClicked() ),
		 this, TQ_SLOT( titleBarDoubleClicked() ) );
    }

    setFrameStyle( TQFrame::StyledPanel | TQFrame::Raised );
    setLineWidth( style().pixelMetric(TQStyle::PM_MDIFrameWidth, this) );
    setMinimumSize( 128, 0 );

    childWidget = window;
    if (!childWidget)
	return;

#ifndef TQT_NO_WIDGET_TOPEXTRA
    setCaption( childWidget->caption() );
#endif

    TQPoint p;
    TQSize s;
    TQSize cs;

    bool hasBeenResized = childWidget->testWState( WState_Resized );

    if ( !hasBeenResized )
	cs = childWidget->sizeHint().expandedTo( childWidget->minimumSizeHint() );
    else
	cs = childWidget->size();

    int th = titlebar ? titlebar->sizeHint().height() : 0;
    if ( titlebar ) {
#ifndef TQT_NO_WIDGET_TOPEXTRA
	int iconSize = th;
	if( childWidget->icon() ) {
	    TQPixmap pm(*childWidget->icon());
	    if(pm.width() > iconSize || pm.height() > iconSize) {
		TQImage im;
		im = pm;
		pm = im.smoothScale( TQMIN(iconSize, pm.width()), TQMIN(iconSize, pm.height()) );
	    }
	    titlebar->setIcon( pm );
	}
#endif
	if ( !style().styleHint( TQStyle::SH_TitleBar_NoBorder, titlebar ) )
	    th += frameWidth();
	else
	    th -= contentsRect().y();

	p = TQPoint( contentsRect().x(),
		    th + contentsRect().y() );
	s = TQSize( cs.width() + 2*frameWidth(),
		   cs.height() + 2*frameWidth() + th );
    } else {
	p = TQPoint( contentsRect().x(), contentsRect().y() );
	s = TQSize( cs.width() + 2*frameWidth(),
		   cs.height() + 2*frameWidth() );
    }

    childWidget->reparent( this, p);
    resize( s );

    childWidget->installEventFilter( this );

    widgetResizeHandler = new TQWidgetResizeHandler( this, window );
    widgetResizeHandler->setSizeProtection( !parent->scrollBarsEnabled() );
    connect( widgetResizeHandler, TQ_SIGNAL( activate() ),
	     this, TQ_SLOT( activate() ) );
    if ( !style().styleHint( TQStyle::SH_TitleBar_NoBorder, titlebar ) )
	widgetResizeHandler->setExtraHeight( th + contentsRect().y() - 2*frameWidth() );
    else
	widgetResizeHandler->setExtraHeight( th + contentsRect().y() - frameWidth() );
    if(parent->windowMode() == TQWorkspace::TopLevel && isTopLevel()) {
	move(0, 0);
	widgetResizeHandler->setActive( false );
    }
    if ( childWidget->minimumSize() == childWidget->maximumSize() )
	widgetResizeHandler->setActive( TQWidgetResizeHandler::Resize, false );
    setBaseSize( baseSize() );
}

TQWorkspaceChild::~TQWorkspaceChild()
{
    if ( iconw )
	delete iconw->parentWidget();
    TQWorkspace *workspace = ::tqt_cast<TQWorkspace*>(parentWidget());
    if ( workspace ) {
        workspace->d->focus.removeRef(this);
	if ( workspace->d->active == this ) {
	    workspace->activatePrevWindow();
            if (workspace->d->active == this) {
                workspace->activateWindow(0);
            }
        }
	if ( workspace->d->maxWindow == this ) {
	    workspace->hideMaximizeControls();
	    workspace->d->maxWindow = 0;
	}
    }
}

bool TQWorkspaceChild::event( TQEvent *e )
{
    if(((TQWorkspace*)parentWidget())->windowMode() == TQWorkspace::TopLevel) {
	switch(e->type()) {
	case TQEvent::Close:
	    if(windowWidget()) {
		if(!windowWidget()->close()) {
		    if(((TQWorkspace*) parentWidget() )->d->active == this)
			((TQWorkspace*) parentWidget() )->activatePrevWindow();
		    return true;
		}
	    }
	    break;
#if 0
	case TQEvent::WindowDeactivate:
	    if(statusbar) {
		TQSize newsize(width(), height() - statusbar->height());
		if(statusbar->parentWidget() == this)
		    statusbar->hide();
		statusbar = 0;
		resize(newsize);
	    }
	    break;
#endif
	case TQEvent::WindowActivate:
	    if(((TQWorkspace*)parentWidget())->activeWindow() == windowWidget())
		activate();
	    if(statusbar)
		statusbar->show();
	    else if(((TQWorkspace*) parentWidget() )->d->mainwindow)
		setStatusBar(((TQWorkspace*) parentWidget() )->d->mainwindow->statusBar());
	    break;
	default:
	    break;
	}
    }
    return TQWidget::event(e);
}

void TQWorkspaceChild::setStatusBar( TQStatusBar *sb )
{
    if(((TQWorkspace*) parentWidget() )->windowMode() == TQWorkspace::TopLevel) {
	TQSize newsize;
	if(sb) {
	    sb->show();
	    if(sb != statusbar) {
		sb->reparent(this, TQPoint(0, height()), true);
		newsize = TQSize(width(), height() + sb->height());
	    }
	}
	statusbar = sb;
	if(!newsize.isNull())
	    resize(newsize);
    }
}

void TQWorkspaceChild::moveEvent( TQMoveEvent *e )
{
    if(((TQWorkspace*) parentWidget() )->windowMode() == TQWorkspace::TopLevel && !e->spontaneous()) {
	TQPoint p = parentWidget()->topLevelWidget()->pos();
	if(x() < p.x() || y() < p.y())
	    move(TQMAX(x(), p.x()), TQMAX(y(), p.y()));
    }
    ((TQWorkspace*) parentWidget() )->updateWorkspace();
}

void TQWorkspaceChild::resizeEvent( TQResizeEvent * )
{
    TQRect r = contentsRect();
    TQRect cr;

    if ( titlebar ) {
	int th = titlebar->sizeHint().height();
	TQRect tbrect( 0, 0, width(), th );
	if ( !style().styleHint( TQStyle::SH_TitleBar_NoBorder ) )
	    tbrect = TQRect( r.x(), r.y(), r.width(), th );
	titlebar->setGeometry( tbrect );

	if ( style().styleHint( TQStyle::SH_TitleBar_NoBorder, titlebar ) )
	    th -= frameWidth();
	cr = TQRect( r.x(), r.y() + th + (shademode ? (frameWidth() * 3) : 0),
		    r.width(), r.height() - th );
    } else {
	cr = r;
    }

    if(statusbar && statusbar->isVisible()) {
	int sh = statusbar->height();
	statusbar->setGeometry(r.x(), r.bottom() - sh, r.width(), sh);
	cr.setBottom(cr.bottom() - sh);
    }

    if (!childWidget)
	return;

    windowSize = cr.size();
    childWidget->setGeometry( cr );
    ((TQWorkspace*) parentWidget() )->updateWorkspace();
}

TQSize TQWorkspaceChild::baseSize() const
{
    int th = titlebar ? titlebar->sizeHint().height() : 0;
    if ( style().styleHint( TQStyle::SH_TitleBar_NoBorder, titlebar ) )
	th -= frameWidth();
    return TQSize( 2*frameWidth(), 2*frameWidth() + th );
}

TQSize TQWorkspaceChild::sizeHint() const
{
    if ( !childWidget )
	return TQFrame::sizeHint() + baseSize();

    TQSize prefSize = windowWidget()->sizeHint().expandedTo( windowWidget()->minimumSizeHint() );
    prefSize = prefSize.expandedTo( windowWidget()->minimumSize() ).boundedTo( windowWidget()->maximumSize() );
    prefSize += baseSize();

    return prefSize;
}

TQSize TQWorkspaceChild::minimumSizeHint() const
{
    if ( !childWidget )
	return TQFrame::minimumSizeHint() + baseSize();
    TQSize s = childWidget->minimumSize();
    if ( s.isEmpty() )
	s = childWidget->minimumSizeHint();
    return s + baseSize();
}

void TQWorkspaceChild::activate()
{
    ((TQWorkspace*)parentWidget())->activateWindow( windowWidget() );
}

bool TQWorkspaceChild::eventFilter( TQObject * o, TQEvent * e)
{
    if ( !isActive() && ( e->type() == TQEvent::MouseButtonPress ||
	e->type() == TQEvent::FocusIn ) ) {
	if ( iconw ) {
	    ((TQWorkspace*)parentWidget())->normalizeWindow( windowWidget() );
	    if ( iconw ) {
		((TQWorkspace*)parentWidget())->removeIcon( iconw->parentWidget() );
		delete iconw->parentWidget();
		iconw = 0;
	    }
	}
	activate();
    }

    // for all widgets except the window, that's the only thing we
    // process, and if we have no childWidget we skip totally
    if ( o != childWidget || childWidget == 0 )
	return false;

    switch ( e->type() ) {
    case TQEvent::Show:
	if ( ((TQWorkspace*)parentWidget())->d->focus.find( this ) < 0 )
	    ((TQWorkspace*)parentWidget())->d->focus.append( this );
	if ( isVisibleTo( parentWidget() ) )
	    break;
	if (( (TQShowEvent*)e)->spontaneous() )
	    break;
	// fall through
    case TQEvent::ShowToParent:
	if ( windowWidget() && windowWidget()->testWFlags( WStyle_StaysOnTop ) ) {
	    internalRaise();
	    show();
	}
	((TQWorkspace*)parentWidget())->showWindow( windowWidget() );
	break;
    case TQEvent::ShowMaximized:
	if ( windowWidget()->maximumSize().isValid() &&
	     ( windowWidget()->maximumWidth() < parentWidget()->width() ||
	       windowWidget()->maximumHeight() < parentWidget()->height() ) ) {
	    windowWidget()->resize( windowWidget()->maximumSize() );
            ((TQWorkspace*)windowWidget())->clearWState(WState_Maximized);
            if (titlebar)
                titlebar->repaint(false);
	    break;
	}
	if ( windowWidget()->testWFlags( WStyle_Maximize ) && !windowWidget()->testWFlags( WStyle_Tool ) )
	    ((TQWorkspace*)parentWidget())->maximizeWindow( windowWidget() );
	else
	    ((TQWorkspace*)parentWidget())->normalizeWindow( windowWidget() );
	break;
    case TQEvent::ShowMinimized:
	((TQWorkspace*)parentWidget())->minimizeWindow( windowWidget() );
	break;
    case TQEvent::ShowNormal:
	((TQWorkspace*)parentWidget())->normalizeWindow( windowWidget() );
	if (iconw) {
	    ((TQWorkspace*)parentWidget())->removeIcon( iconw->parentWidget() );
	    delete iconw->parentWidget();
	}
	break;
    case TQEvent::Hide:
    case TQEvent::HideToParent:
	if ( !childWidget->isVisibleTo( this ) ) {
	    TQWidget * w = iconw;
	    if ( w && ( w = w->parentWidget() ) ) {
		((TQWorkspace*)parentWidget())->removeIcon( w );
		delete w;
	    }
	    hide();
	}
	break;
    case TQEvent::CaptionChange:
#ifndef TQT_NO_WIDGET_TOPEXTRA
	setCaption( childWidget->caption() );
	if ( iconw )
	    iconw->setCaption( childWidget->caption() );
#endif
	break;
    case TQEvent::IconChange:
	{
	    TQWorkspace* ws = (TQWorkspace*)parentWidget();
	    if ( !titlebar )
		break;

	    TQPixmap pm;
	    int iconSize = titlebar->size().height();
#ifndef TQT_NO_WIDGET_TOPEXTRA
	    if ( childWidget->icon() ) {
		pm = *childWidget->icon();
		if(pm.width() > iconSize || pm.height() > iconSize) {
		    TQImage im;
		    im = pm;
		    pm = im.smoothScale( TQMIN(iconSize, pm.width()), TQMIN(iconSize, pm.height()) );
		}
	    } else
#endif
	    {
		pm.resize( iconSize, iconSize );
		pm.fill( color1 );
		pm.setMask(pm.createHeuristicMask());
	    }
	    titlebar->setIcon( pm );
	    if ( iconw )
		iconw->setIcon( pm );

	    if ( ws->d->maxWindow != this )
		break;

	    if ( ws->d->maxtools )
		ws->d->maxtools->setPixmap( pm );
	}
	break;
    case TQEvent::Resize:
	{
	    TQResizeEvent* re = (TQResizeEvent*)e;
	    if ( re->size() != windowSize && !shademode )
		resize( re->size() + baseSize() );
	}
	break;

    case TQEvent::WindowDeactivate:
	if ( titlebar )
	    titlebar->setActive( false );
	repaint( false );
	break;

    case TQEvent::WindowActivate:
	if ( titlebar )
	    titlebar->setActive( act );
	repaint( false );
	break;

    default:
	break;
    }

    return TQFrame::eventFilter(o, e);
}

bool TQWorkspaceChild::focusNextPrevChild( bool next )
{
    TQFocusData *f = focusData();

    TQWidget *startingPoint = f->home();
    TQWidget *candidate = 0;
    TQWidget *w = next ? f->next() : f->prev();
    while( !candidate && w != startingPoint ) {
	if ( w != startingPoint &&
	     (w->focusPolicy() & TabFocus) == TabFocus
	     && w->isEnabled() &&!w->focusProxy() && w->isVisible() )
	    candidate = w;
	w = next ? f->next() : f->prev();
    }

    if ( candidate ) {
	TQObjectList *ol = queryList();
	bool ischild = ol->findRef( candidate ) != -1;
	delete ol;
	if ( !ischild ) {
	    startingPoint = f->home();
	    TQWidget *nw = next ? f->prev() : f->next();
	    TQObjectList *ol2 = queryList();
	    TQWidget *lastValid = 0;
	    candidate = startingPoint;
	    while ( nw != startingPoint ) {
		if ( ( candidate->focusPolicy() & TabFocus ) == TabFocus
		    && candidate->isEnabled() &&!candidate->focusProxy() && candidate->isVisible() )
		    lastValid = candidate;
		if ( ol2->findRef( nw ) == -1 ) {
		    candidate = lastValid;
		    break;
		}
		candidate = nw;
		nw = next ? f->prev() : f->next();
	    }
	    delete ol2;
	}
    }

    if ( !candidate )
	return false;

    candidate->setFocus();
    return true;
}

void TQWorkspaceChild::childEvent( TQChildEvent*  e)
{
    if ( e->type() == TQEvent::ChildRemoved && e->child() == childWidget ) {
	childWidget = 0;
	if ( iconw ) {
	    ((TQWorkspace*)parentWidget())->removeIcon( iconw->parentWidget() );
	    delete iconw->parentWidget();
	}
	close();
    }
}


void TQWorkspaceChild::doResize()
{
    widgetResizeHandler->doResize();
}

void TQWorkspaceChild::doMove()
{
    widgetResizeHandler->doMove();
}

void TQWorkspaceChild::enterEvent( TQEvent * )
{
}

void TQWorkspaceChild::leaveEvent( TQEvent * )
{
#ifndef TQT_NO_CURSOR
    if ( !widgetResizeHandler->isButtonDown() )
	setCursor( arrowCursor );
#endif
}

void TQWorkspaceChild::drawFrame( TQPainter *p )
{
    TQStyle::SFlags flags = TQStyle::Style_Default;
    TQStyleOption opt(lineWidth(),midLineWidth());

    if ( titlebar && titlebar->isActive() )
	flags |= TQStyle::Style_Active;

    style().drawPrimitive( TQStyle::PE_WindowFrame, p, rect(), colorGroup(), flags, opt );
}

void TQWorkspaceChild::styleChange( TQStyle & )
{
    resizeEvent( 0 );
    if ( iconw ) {
	TQVBox *vbox = (TQVBox*)iconw->parentWidget()->tqt_cast( "TQVBox" );
	Q_ASSERT(vbox);
	if ( !style().styleHint( TQStyle::SH_TitleBar_NoBorder ) ) {
	    vbox->setFrameStyle( TQFrame::WinPanel | TQFrame::Raised );
	    vbox->resize( 196+2*vbox->frameWidth(), 20 + 2*vbox->frameWidth() );
	} else {
	    vbox->resize( 196, 20 );
	}
    }
}

void TQWorkspaceChild::setActive( bool b )
{
    if ( !childWidget )
	return;

    bool hasFocus = isChildOf( focusWidget(), childWidget );
    if ( act == b && hasFocus )
	return;

    act = b;

    if ( titlebar )
	titlebar->setActive( act );
    if ( iconw )
	iconw->setActive( act );
    repaint( false );

    TQObjectList* ol = childWidget->queryList( "TQWidget" );
    if ( act ) {
	TQObject *o;
	for ( o = ol->first(); o; o = ol->next() )
	    o->removeEventFilter( this );
	if ( !hasFocus ) {
	    if ( lastfocusw && ol->contains( lastfocusw ) &&
		 lastfocusw->focusPolicy() != NoFocus ) {
		// this is a bug if lastfocusw has been deleted, a new
		// widget has been created, and the new one is a child
		// of the same window as the old one. but even though
		// it's a bug the behaviour is reasonable
		lastfocusw->setFocus();
	    } else if ( childWidget->focusPolicy() != NoFocus ) {
		childWidget->setFocus();
	    } else {
		// find something, anything, that accepts focus, and use that.
		o = ol->first();
		while( o && ((TQWidget*)o)->focusPolicy() == NoFocus )
		    o = ol->next();
		if ( o )
		    ((TQWidget*)o)->setFocus();
	    }
	}
    } else {
	if ( isChildOf( focusWidget(), childWidget ) )
	    lastfocusw = focusWidget();
	TQObject * o;
	for ( o = ol->first(); o; o = ol->next() ) {
	    o->removeEventFilter( this );
	    o->installEventFilter( this );
	}
    }
    delete ol;
}

bool TQWorkspaceChild::isActive() const
{
    return act;
}

TQWidget* TQWorkspaceChild::windowWidget() const
{
    return childWidget;
}


TQWidget* TQWorkspaceChild::iconWidget() const
{
    if ( !iconw ) {
	TQWorkspaceChild* that = (TQWorkspaceChild*) this;

	// ### why do we even need the vbox? -Brad
	TQVBox* vbox = new TQVBox(that, "qt_vbox", WType_TopLevel );
	TQTitleBar *tb = new TQTitleBar( windowWidget(), vbox, "_workspacechild_icon_");
	int th = style().pixelMetric( TQStyle::PM_TitleBarHeight, tb );
	int iconSize = style().pixelMetric( TQStyle::PM_MDIMinimizedWidth, this );
	if ( !style().styleHint( TQStyle::SH_TitleBar_NoBorder ) ) {
	    vbox->setFrameStyle( TQFrame::WinPanel | TQFrame::Raised );
	    vbox->resize( iconSize+2*vbox->frameWidth(), th+2*vbox->frameWidth() );
	} else {
	    vbox->resize( iconSize, th );
	}
	that->iconw = tb;
	iconw->setActive( isActive() );

	connect( iconw, TQ_SIGNAL( doActivate() ),
		 this, TQ_SLOT( activate() ) );
	connect( iconw, TQ_SIGNAL( doClose() ),
		 windowWidget(), TQ_SLOT( close() ) );
	connect( iconw, TQ_SIGNAL( doNormal() ),
		 this, TQ_SLOT( showNormal() ) );
	connect( iconw, TQ_SIGNAL( doMaximize() ),
		 this, TQ_SLOT( showMaximized() ) );
	connect( iconw, TQ_SIGNAL( popupOperationMenu(const TQPoint&) ),
		 this, TQ_SIGNAL( popupOperationMenu(const TQPoint&) ) );
	connect( iconw, TQ_SIGNAL( showOperationMenu() ),
		 this, TQ_SIGNAL( showOperationMenu() ) );
	connect( iconw, TQ_SIGNAL( doubleClicked() ),
		 this, TQ_SLOT( titleBarDoubleClicked() ) );
    }
#ifndef TQT_NO_WIDGET_TOPEXTRA
    if ( windowWidget() ) {
	iconw->setCaption( windowWidget()->caption() );
	if ( windowWidget()->icon() ) {
	    int iconSize = iconw->sizeHint().height();

	    TQPixmap pm(*childWidget->icon());
	    if(pm.width() > iconSize || pm.height() > iconSize) {
		TQImage im;
		im = pm;
		pm = im.smoothScale( TQMIN(iconSize, pm.width()), TQMIN(iconSize, pm.height()) );
	    }
	    iconw->setIcon( pm );
	}
    }
#endif
    return iconw->parentWidget();
}

void TQWorkspaceChild::showMinimized()
{
    windowWidget()->setWindowState(WindowMinimized | windowWidget()->windowState());
}

void TQWorkspaceChild::showMaximized()
{
    windowWidget()->setWindowState(WindowMaximized | (windowWidget()->windowState() & ~WindowMinimized));
}

void TQWorkspaceChild::showNormal()
{
    windowWidget()->setWindowState(windowWidget()->windowState() & ~(WindowMinimized|WindowMaximized));
}

void TQWorkspaceChild::showShaded()
{
    if ( !titlebar)
	return;
    Q_ASSERT( windowWidget()->testWFlags( WStyle_MinMax ) && windowWidget()->testWFlags( WStyle_Tool ) );
    ((TQWorkspace*)parentWidget())->activateWindow( windowWidget() );
    if ( shademode ) {
	TQWorkspaceChild* fake = (TQWorkspaceChild*)windowWidget();
	fake->clearWState( WState_Minimized );
	clearWState( WState_Minimized );

	shademode = false;
	resize( shadeRestore );
	setMinimumSize( shadeRestoreMin );
	style().polish(this);
    } else {
	shadeRestore = size();
	shadeRestoreMin = minimumSize();
	setMinimumHeight(0);
	shademode = true;
	TQWorkspaceChild* fake = (TQWorkspaceChild*)windowWidget();
	fake->setWState( WState_Minimized );
	setWState( WState_Minimized );

	if ( style().styleHint( TQStyle::SH_TitleBar_NoBorder ) )
	    resize( width(), titlebar->height() );
	else
	    resize( width(), titlebar->height() + 2*lineWidth() + 1 );
	style().polish(this);
    }
    titlebar->update();
}

void TQWorkspaceChild::titleBarDoubleClicked()
{
    if ( !windowWidget() )
	return;
    if ( windowWidget()->testWFlags( WStyle_MinMax ) ) {
	if ( windowWidget()->testWFlags( WStyle_Tool ) )
	    showShaded();
	else if ( iconw )
	    showNormal();
	else if ( windowWidget()->testWFlags( WStyle_Maximize ) )
	    showMaximized();
    }
}

void TQWorkspaceChild::adjustToFullscreen()
{
    if ( !childWidget )
	return;

    tqApp->sendPostedEvents( this, TQEvent::Resize );
    tqApp->sendPostedEvents( childWidget, TQEvent::Resize );
    tqApp->sendPostedEvents( childWidget, TQEvent::Move );
    if( style().styleHint(TQStyle::SH_Workspace_FillSpaceOnMaximize, this) ) {
	setGeometry( 0, 0, parentWidget()->width(), parentWidget()->height());
    } else {
	int w = parentWidget()->width() + width() - childWidget->width();
	int h = parentWidget()->height() + height() - childWidget->height();
	w = TQMAX( w, childWidget->minimumWidth() );
	h = TQMAX( h, childWidget->minimumHeight() );
	setGeometry( -childWidget->x(), -childWidget->y(), w, h );
    }
    setWState( WState_Maximized );
    ((TQWorkspaceChild*)childWidget)->setWState( WState_Maximized );
}

void TQWorkspaceChild::setCaption( const TQString& cap )
{
    if ( titlebar )
	titlebar->setCaption( cap );
#ifndef TQT_NO_WIDGET_TOPEXTRA
    TQWidget::setCaption( cap );
#endif
}

void TQWorkspaceChild::internalRaise()
{
    setUpdatesEnabled( false );
    if ( iconw )
	iconw->parentWidget()->raise();
    raise();

    if ( !windowWidget() || windowWidget()->testWFlags( WStyle_StaysOnTop ) ) {
	setUpdatesEnabled( true );
	return;
    }

    TQPtrListIterator<TQWorkspaceChild> it( ((TQWorkspace*)parent())->d->windows );
    while ( it.current () ) {
     	TQWorkspaceChild* c = it.current();
	++it;
	if ( c->windowWidget() &&
	    !c->windowWidget()->isHidden() &&
	     c->windowWidget()->testWFlags( WStyle_StaysOnTop ) )
	     c->raise();
    }
    setUpdatesEnabled( true );
}

void TQWorkspaceChild::move( int x, int y )
{
    int nx = x;
    int ny = y;

    if ( windowWidget() && windowWidget()->testWFlags( WStyle_Tool ) ) {
	int dx = 10;
	int dy = 10;

	if ( TQABS( x ) < dx )
	    nx = 0;
	if ( TQABS( y ) < dy )
	    ny = 0;
	if ( TQABS( x + width() - parentWidget()->width() ) < dx ) {
	    nx = parentWidget()->width() - width();
	    snappedRight = true;
	} else
	    snappedRight = false;

	if ( TQABS( y + height() - parentWidget()->height() ) < dy ) {
	    ny = parentWidget()->height() - height();
	    snappedDown = true;
	} else
	    snappedDown = false;
    }
    TQFrame::move( nx, ny );
}

bool TQWorkspace::scrollBarsEnabled() const
{
    return d->vbar != 0;
}

/*!
    \property TQWorkspace::scrollBarsEnabled
    \brief whether the workspace provides scrollbars

    If this property is set to true, it is possible to resize child
    windows over the right or the bottom edge out of the visible area
    of the workspace. The workspace shows scrollbars to make it
    possible for the user to access those windows. If this property is
    set to false (the default), resizing windows out of the visible
    area of the workspace is not permitted.
*/
void TQWorkspace::setScrollBarsEnabled( bool enable )
{
    if ( (d->vbar != 0) == enable )
	return;

    d->xoffset = d->yoffset = 0;
    if ( enable ) {
	d->vbar = new TQScrollBar( Vertical, this, "vertical scrollbar" );
	connect( d->vbar, TQ_SIGNAL( valueChanged(int) ), this, TQ_SLOT( scrollBarChanged() ) );
	d->hbar = new TQScrollBar( Horizontal, this, "horizontal scrollbar" );
	connect( d->hbar, TQ_SIGNAL( valueChanged(int) ), this, TQ_SLOT( scrollBarChanged() ) );
	d->corner = new TQWidget( this, "qt_corner" );
	updateWorkspace();
    } else {
	delete d->vbar;
	delete d->hbar;
	delete d->corner;
	d->vbar = d->hbar = 0;
	d->corner = 0;
    }

    TQPtrListIterator<TQWorkspaceChild> it( d->windows );
    while ( it.current () ) {
	TQWorkspaceChild *child = it.current();
	++it;
	child->widgetResizeHandler->setSizeProtection( !enable );
    }
}

TQRect TQWorkspace::updateWorkspace()
{
    if ( !isUpdatesEnabled() )
	return rect();

    TQRect cr( rect() );

    if ( scrollBarsEnabled() && !d->maxWindow ) {
	d->corner->raise();
	d->vbar->raise();
	d->hbar->raise();
	if ( d->maxWindow )
	    d->maxWindow->internalRaise();

	TQRect r( 0, 0, 0, 0 );
	TQPtrListIterator<TQWorkspaceChild> it( d->windows );
	while ( it.current () ) {
	    TQWorkspaceChild *child = it.current();
	    ++it;
	    if ( !child->isHidden() )
		r = r.unite( child->geometry() );
	}
	d->vbar->blockSignals( true );
	d->hbar->blockSignals( true );

	int hsbExt = d->hbar->sizeHint().height();
	int vsbExt = d->vbar->sizeHint().width();


	bool showv = d->yoffset || d->yoffset + r.bottom() - height() + 1 > 0 || d->yoffset + r.top() < 0;
	bool showh = d->xoffset || d->xoffset + r.right() - width() + 1 > 0 || d->xoffset + r.left() < 0;

	if ( showh && !showv)
	    showv = d->yoffset + r.bottom() - height() + hsbExt + 1 > 0;
	if ( showv && !showh )
	    showh = d->xoffset + r.right() - width() + vsbExt  + 1 > 0;

	if ( !showh )
	    hsbExt = 0;
	if ( !showv )
	    vsbExt = 0;

	if ( showv ) {
	    d->vbar->setSteps( TQMAX( height() / 12, 30 ), height()  - hsbExt );
	    d->vbar->setRange( TQMIN( 0, d->yoffset + TQMIN( 0, r.top() ) ), TQMAX( 0, d->yoffset + TQMAX( 0, r.bottom() - height() + hsbExt + 1) ) );
	    d->vbar->setGeometry( width() - vsbExt, 0, vsbExt, height() - hsbExt );
	    d->vbar->setValue( d->yoffset );
	    d->vbar->show();
	} else {
	    d->vbar->hide();
	}

	if ( showh ) {
	    d->hbar->setSteps( TQMAX( width() / 12, 30 ), width() - vsbExt );
	    d->hbar->setRange( TQMIN( 0, d->xoffset + TQMIN( 0, r.left() ) ), TQMAX( 0, d->xoffset + TQMAX( 0, r.right() - width() + vsbExt  + 1) ) );
	    d->hbar->setGeometry( 0, height() - hsbExt, width() - vsbExt, hsbExt );
	    d->hbar->setValue( d->xoffset );
	    d->hbar->show();
	} else {
	    d->hbar->hide();
	}

	if ( showh && showv ) {
	    d->corner->setGeometry( width() - vsbExt, height() - hsbExt, vsbExt, hsbExt );
	    d->corner->show();
	} else {
	    d->corner->hide();
	}

	d->vbar->blockSignals( false );
	d->hbar->blockSignals( false );

	cr.setRect( 0, 0, width() - vsbExt, height() - hsbExt );
    }

    TQPtrListIterator<TQWidget> ii( d->icons );
    while ( ii.current() ) {
	TQWorkspaceChild* w = (TQWorkspaceChild*)ii.current();
	++ii;
	int x = w->x();
	int y = w->y();
	bool m = false;
	if ( x+w->width() > cr.width() ) {
	    m = true;
	    x =  cr.width() - w->width();
	}
	if ( y+w->height() >  cr.height() ) {
	    y =  cr.height() - w->height();
	    m = true;
	}
	if ( m )
	    w->move( x, y );
    }

    return cr;

}

void TQWorkspace::scrollBarChanged()
{
    int ver = d->yoffset - d->vbar->value();
    int hor = d->xoffset - d->hbar->value();
    d->yoffset = d->vbar->value();
    d->xoffset = d->hbar->value();

    TQPtrListIterator<TQWorkspaceChild> it( d->windows );
    while ( it.current () ) {
	TQWorkspaceChild *child = it.current();
	++it;
	// we do not use move() due to the reimplementation in TQWorkspaceChild
	child->setGeometry( child->x() + hor, child->y() + ver, child->width(), child->height() );
    }
    updateWorkspace();
}

/*!
    \enum TQWorkspace::WindowOrder

    Specifies the order in which windows are returned from windowList().

    \value CreationOrder The windows are returned in the order of their creation
    \value StackingOrder The windows are returned in the order of their stacking
*/

#ifdef QT_WORKSPACE_WINDOWMODE
/*!
    \enum TQWorkspace::WindowMode

    Determines the Windowing Model TQWorkspace will use for sub-windows.

    \value TopLevel Subwindows are treated as toplevel windows
    \value MDI Subwindows are organized in an MDI interface
    \value AutoDetect TQWorkspace will detect whether TopLevel or MDI
    is appropriate
*/

/*!
    The windowing model influences how the subwindows are actually
    created. For most platforms the default behavior of a workspace is
    to operate in MDI mode, with TQt/Mac the default mode is
    AutoDetect.
*/
#else
/*! \internal */
#endif
TQWorkspace::WindowMode TQWorkspace::windowMode() const
{
    return d->wmode;
}

#ifndef TQT_NO_STYLE
/*!\reimp */
void TQWorkspace::styleChange( TQStyle &olds )
{
    int fs = style().styleHint(TQStyle::SH_Workspace_FillSpaceOnMaximize, this);
    if ( isVisibleTo(0) && d->maxWindow &&
	 fs != olds.styleHint(TQStyle::SH_Workspace_FillSpaceOnMaximize, this)) {
	if( fs )
	    hideMaximizeControls();
	else
	    showMaximizeControls();
    }
    TQWidget::styleChange(olds);
}
#endif




#include "qworkspace.moc"
#endif // TQT_NO_WORKSPACE
