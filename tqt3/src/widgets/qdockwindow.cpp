/****************************************************************************
**
** Implementation of the TQDockWindow class
**
** Created : 001010
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

#include "ntqdockwindow.h"

#ifndef TQT_NO_MAINWINDOW
#include "ntqdesktopwidget.h"
#include "ntqdockarea.h"
#include "qwidgetresizehandler_p.h"
#include "qtitlebar_p.h"
#include "ntqpainter.h"
#include "ntqapplication.h"
#include "ntqtoolbutton.h"
#include "ntqtoolbar.h"
#include "ntqlayout.h"
#include "ntqmainwindow.h"
#include "ntqtimer.h"
#include "ntqtooltip.h"
#include "ntqguardedptr.h"
#include "ntqcursor.h"
#include "ntqstyle.h"

#if defined(TQ_WS_MAC9)
#define MAC_DRAG_HACK
#endif
#ifdef TQ_WS_MACX
static bool default_opaque = true;
#else
static bool default_opaque = false;
#endif

class TQDockWindowPrivate
{
};

class TQDockWindowResizeHandle : public TQWidget
{
    TQ_OBJECT

public:
    TQDockWindowResizeHandle( TQt::Orientation o, TQWidget *parent, TQDockWindow *w, const char* /*name*/=0 );
    void setOrientation( TQt::Orientation o );
    TQt::Orientation orientation() const { return orient; }

    TQSize sizeHint() const;

protected:
    void paintEvent( TQPaintEvent * );
    void mouseMoveEvent( TQMouseEvent * );
    void mousePressEvent( TQMouseEvent * );
    void mouseReleaseEvent( TQMouseEvent * );

private:
    void startLineDraw();
    void endLineDraw();
    void drawLine( const TQPoint &globalPos );

private:
    TQt::Orientation orient;
    bool mousePressed;
    TQPainter *unclippedPainter;
    TQPoint lastPos, firstPos;
    TQDockWindow *dockWindow;

};

TQDockWindowResizeHandle::TQDockWindowResizeHandle( TQt::Orientation o, TQWidget *parent,
						  TQDockWindow *w, const char * )
    : TQWidget( parent, "qt_dockwidget_internal" ), mousePressed( false ), unclippedPainter( 0 ), dockWindow( w )
{
    setOrientation( o );
}

TQSize TQDockWindowResizeHandle::sizeHint() const
{
    int sw = 2 * style().pixelMetric(TQStyle::PM_SplitterWidth, this) / 3;
    return (style().sizeFromContents(TQStyle::CT_DockWindow, this, TQSize(sw, sw)).
	    expandedTo(TQApplication::globalStrut()));
}

void TQDockWindowResizeHandle::setOrientation( TQt::Orientation o )
{
    orient = o;
    if ( o == TQDockArea::Horizontal ) {
#ifndef TQT_NO_CURSOR
	setCursor( splitVCursor );
#endif
	setSizePolicy( TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Fixed ) );
    } else {
#ifndef TQT_NO_CURSOR
	setCursor( splitHCursor );
#endif
	setSizePolicy( TQSizePolicy( TQSizePolicy::Fixed, TQSizePolicy::Expanding ) );
    }
}

void TQDockWindowResizeHandle::mousePressEvent( TQMouseEvent *e )
{
    e->ignore();
    if ( e->button() != LeftButton )
	return;
    e->accept();
    mousePressed = true;
    if ( !dockWindow->opaqueMoving() )
	startLineDraw();
    lastPos = firstPos = e->globalPos();
    if ( !dockWindow->opaqueMoving() )
	drawLine( e->globalPos() );
}

void TQDockWindowResizeHandle::mouseMoveEvent( TQMouseEvent *e )
{
    if ( !mousePressed )
	return;
    if ( !dockWindow->opaqueMoving() ) {
	if ( orientation() != dockWindow->area()->orientation() ) {
	    if ( orientation() == Horizontal ) {
		int minpos = dockWindow->area()->mapToGlobal( TQPoint( 0, 0 ) ).y();
		int maxpos = dockWindow->area()->mapToGlobal( TQPoint( 0, 0 ) ).y() + dockWindow->area()->height();
		if ( e->globalPos().y() < minpos || e->globalPos().y() > maxpos )
		    return;
	    } else {
		int minpos = dockWindow->area()->mapToGlobal( TQPoint( 0, 0 ) ).x();
		int maxpos = dockWindow->area()->mapToGlobal( TQPoint( 0, 0 ) ).x() + dockWindow->area()->width();
		if ( e->globalPos().x() < minpos || e->globalPos().x() > maxpos )
		    return;
	    }
	} else {
	    TQWidget *w = dockWindow->area()->topLevelWidget();
	    if ( w ) {
		if ( orientation() == Horizontal ) {
		    int minpos = w->mapToGlobal( TQPoint( 0, 0 ) ).y();
		    int maxpos = w->mapToGlobal( TQPoint( 0, 0 ) ).y() + w->height();
		    if ( e->globalPos().y() < minpos || e->globalPos().y() > maxpos )
			return;
		} else {
		    int minpos = w->mapToGlobal( TQPoint( 0, 0 ) ).x();
		    int maxpos = w->mapToGlobal( TQPoint( 0, 0 ) ).x() + w->width();
		    if ( e->globalPos().x() < minpos || e->globalPos().x() > maxpos )
			return;
		}
	    }
	}
    }

    if ( !dockWindow->opaqueMoving() )
	drawLine( lastPos );
    lastPos = e->globalPos();
    if ( dockWindow->opaqueMoving() ) {
	mouseReleaseEvent( e );
	mousePressed = true;
	firstPos = e->globalPos();
    }
    if ( !dockWindow->opaqueMoving() )
	drawLine( e->globalPos() );
}

void TQDockWindowResizeHandle::mouseReleaseEvent( TQMouseEvent *e )
{
    if ( mousePressed ) {
	if ( !dockWindow->opaqueMoving() ) {
	    drawLine( lastPos );
	    endLineDraw();
	}
	if ( orientation() != dockWindow->area()->orientation() )
	    dockWindow->area()->invalidNextOffset( dockWindow );
	if ( orientation() == Horizontal ) {
	    int dy;
	    if ( dockWindow->area()->handlePosition() == TQDockArea::Normal || orientation() != dockWindow->area()->orientation() )
		dy = e->globalPos().y() - firstPos.y();
	    else
		dy =  firstPos.y() - e->globalPos().y();
	    int d = dockWindow->height() + dy;
	    if ( orientation() != dockWindow->area()->orientation() ) {
		dockWindow->setFixedExtentHeight( -1 );
		d = TQMAX( d, dockWindow->minimumHeight() );
		int ms = dockWindow->area()->maxSpace( d, dockWindow );
		d = TQMIN( d, ms );
		dockWindow->setFixedExtentHeight( d );
	    } else {
		dockWindow->area()->setFixedExtent( d, dockWindow );
	    }
	} else {
	    int dx;
	    if ( dockWindow->area()->handlePosition() == TQDockArea::Normal || orientation() != dockWindow->area()->orientation() )
		dx = e->globalPos().x() - firstPos.x();
	    else
		dx = firstPos.x() - e->globalPos().x();
	    int d = dockWindow->width() + dx;
	    if ( orientation() != dockWindow->area()->orientation() ) {
		dockWindow->setFixedExtentWidth( -1 );
		d = TQMAX( d, dockWindow->minimumWidth() );
		int ms = dockWindow->area()->maxSpace( d, dockWindow );
		d = TQMIN( d, ms );
		dockWindow->setFixedExtentWidth( d );
	    } else {
		dockWindow->area()->setFixedExtent( d, dockWindow );
	    }
	}
    }

    TQApplication::postEvent( dockWindow->area(), new TQEvent( TQEvent::LayoutHint ) );
    mousePressed = false;
}

void TQDockWindowResizeHandle::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );
    style().drawPrimitive(TQStyle::PE_DockWindowResizeHandle, &p, rect(), colorGroup(),
			  (isEnabled() ?
			   TQStyle::Style_Enabled : TQStyle::Style_Default) |
			  (orientation() == TQt::Horizontal ?
			   TQStyle::Style_Horizontal : TQStyle::Style_Default ));
}

void TQDockWindowResizeHandle::startLineDraw()
{
    if ( unclippedPainter )
	endLineDraw();
#ifdef MAC_DRAG_HACK
    TQWidget *paint_on = topLevelWidget();
#else
    int scr = TQApplication::desktop()->screenNumber( this );
    TQWidget *paint_on = TQApplication::desktop()->screen( scr );
#endif
    unclippedPainter = new TQPainter( paint_on, true );
    unclippedPainter->setPen( TQPen( gray, orientation() == Horizontal ? height() : width() ) );
    unclippedPainter->setRasterOp( XorROP );
}

void TQDockWindowResizeHandle::endLineDraw()
{
    if ( !unclippedPainter )
	return;
    delete unclippedPainter;
    unclippedPainter = 0;
}

void TQDockWindowResizeHandle::drawLine( const TQPoint &globalPos )
{
#ifdef MAC_DRAG_HACK
    TQPoint start = mapTo(topLevelWidget(), TQPoint(0, 0));
    TQPoint starta = dockWindow->area()->mapTo(topLevelWidget(), TQPoint(0, 0));
    TQPoint end = globalPos - topLevelWidget()->pos();
#else
    TQPoint start = mapToGlobal( TQPoint( 0, 0 ) );
    TQPoint starta = dockWindow->area()->mapToGlobal( TQPoint( 0, 0 ) );
    TQPoint end = globalPos;
#endif

    if ( orientation() == Horizontal ) {
	if ( orientation() == dockWindow->orientation() )
	    unclippedPainter->drawLine( starta.x() , end.y(), starta.x() + dockWindow->area()->width(), end.y() );
	else
	    unclippedPainter->drawLine( start.x(), end.y(), start.x() + width(), end.y() );
    } else {
	if ( orientation() == dockWindow->orientation() )
	    unclippedPainter->drawLine( end.x(), starta.y(), end.x(), starta.y() + dockWindow->area()->height() );
	else
	    unclippedPainter->drawLine( end.x(), start.y(), end.x(), start.y() + height() );
    }
}

static TQPoint realWidgetPos( TQDockWindow *w )
{
    if ( !w->parentWidget() || w->place() == TQDockWindow::OutsideDock )
	return w->pos();
    return w->parentWidget()->mapToGlobal( w->geometry().topLeft() );
}

class TQDockWindowHandle : public TQWidget
{
    TQ_OBJECT
    TQ_PROPERTY( TQString caption READ caption )
    friend class TQDockWindow;
    friend class TQDockWindowTitleBar;

public:
    TQDockWindowHandle( TQDockWindow *dw );
    void updateGui();

    TQSize minimumSizeHint() const;
    TQSize minimumSize() const { return minimumSizeHint(); }
    TQSize sizeHint() const { return minimumSize(); }
    TQSizePolicy sizePolicy() const;
    void setOpaqueMoving( bool b ) { opaque = b; }

    TQString caption() const { return dockWindow->caption(); }

signals:
    void doubleClicked();

protected:
    void paintEvent( TQPaintEvent *e );
    void resizeEvent( TQResizeEvent *e );
    void mousePressEvent( TQMouseEvent *e );
    void mouseMoveEvent( TQMouseEvent *e );
    void mouseReleaseEvent( TQMouseEvent *e );
    void mouseDoubleClickEvent( TQMouseEvent *e );
    void keyPressEvent( TQKeyEvent *e );
    void keyReleaseEvent( TQKeyEvent *e );
#ifndef TQT_NO_STYLE
    void styleChange( TQStyle& );
#endif

private slots:
    void minimize();

private:
    TQDockWindow *dockWindow;
    TQPoint offset;
    TQToolButton *closeButton;
    TQTimer *timer;
    uint opaque		: 1;
    uint mousePressed	: 1;
    uint hadDblClick	: 1;
    uint ctrlDown : 1;
    TQGuardedPtr<TQWidget> oldFocus;
};

class TQDockWindowTitleBar : public TQTitleBar
{
    TQ_OBJECT
    friend class TQDockWindow;
    friend class TQDockWindowHandle;

public:
    TQDockWindowTitleBar( TQDockWindow *dw );
    void updateGui();
    void setOpaqueMoving( bool b ) { opaque = b; }

protected:
    void resizeEvent( TQResizeEvent *e );
    void mousePressEvent( TQMouseEvent *e );
    void mouseMoveEvent( TQMouseEvent *e );
    void mouseReleaseEvent( TQMouseEvent *e );
    void mouseDoubleClickEvent( TQMouseEvent *e );
    void keyPressEvent( TQKeyEvent *e );
    void keyReleaseEvent( TQKeyEvent *e );

private:
    TQDockWindow *dockWindow;
    TQPoint offset;
    uint mousePressed : 1;
    uint hadDblClick : 1;
    uint opaque : 1;
    uint ctrlDown : 1;
    TQGuardedPtr<TQWidget> oldFocus;

};

TQDockWindowHandle::TQDockWindowHandle( TQDockWindow *dw )
    : TQWidget( dw, "qt_dockwidget_internal", WNoAutoErase ), dockWindow( dw ),
      closeButton( 0 ), opaque( default_opaque ), mousePressed( false )
{
    ctrlDown = false;
    timer = new TQTimer( this );
    connect( timer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( minimize() ) );
#ifdef TQ_WS_WIN
    setCursor( SizeAllCursor );
#endif
}

void TQDockWindowHandle::paintEvent( TQPaintEvent *e )
{
    if ( (!dockWindow->dockArea || mousePressed) && !opaque )
	return;
    erase();
    TQPainter p( this );
    TQStyle::SFlags flags = TQStyle::Style_Default;
    if ( isEnabled() )
	flags |= TQStyle::Style_Enabled;
    if ( !dockWindow->area() || dockWindow->area()->orientation() == Horizontal )
	flags |= TQStyle::Style_Horizontal;

    style().drawPrimitive( TQStyle::PE_DockWindowHandle, &p,
			   TQStyle::visualRect( style().subRect( TQStyle::SR_DockWindowHandleRect,
								this ), this ),
			   colorGroup(), flags );
    TQWidget::paintEvent( e );
}

void TQDockWindowHandle::keyPressEvent( TQKeyEvent *e )
{
    if ( !mousePressed )
	return;
    if ( e->key() == Key_Control ) {
	ctrlDown = true;
	dockWindow->handleMove( mapFromGlobal(TQCursor::pos()) - offset, TQCursor::pos(), !opaque );
    }
}

void TQDockWindowHandle::keyReleaseEvent( TQKeyEvent *e )
{
    if ( !mousePressed )
	return;
    if ( e->key() == Key_Control ) {
	ctrlDown = false;
	dockWindow->handleMove( mapFromGlobal(TQCursor::pos()) - offset, TQCursor::pos(), !opaque );
    }
}

void TQDockWindowHandle::mousePressEvent( TQMouseEvent *e )
{
    if ( !dockWindow->dockArea )
	return;
    ctrlDown = ( e->state() & ControlButton ) == ControlButton;
    oldFocus = tqApp->focusWidget();
    setFocus();
    e->ignore();
    if ( e->button() != LeftButton )
	return;
    e->accept();
    hadDblClick = false;
    mousePressed = true;
    offset = e->pos();
    dockWindow->startRectDraw( mapToGlobal( e->pos() ), !opaque );
    if ( !opaque )
	tqApp->installEventFilter( dockWindow );
}

void TQDockWindowHandle::mouseMoveEvent( TQMouseEvent *e )
{
    if ( !mousePressed || e->pos() == offset )
	return;
    ctrlDown = ( e->state() & ControlButton ) == ControlButton;
    dockWindow->handleMove( e->pos() - offset, e->globalPos(), !opaque );
    if ( opaque )
	dockWindow->updatePosition( e->globalPos() );
}

void TQDockWindowHandle::mouseReleaseEvent( TQMouseEvent *e )
{
    ctrlDown = false;
    tqApp->removeEventFilter( dockWindow );
    if ( oldFocus )
	oldFocus->setFocus();
    if ( !mousePressed )
	return;
    dockWindow->endRectDraw( !opaque );
    mousePressed = false;
#ifdef TQ_WS_MAC
    releaseMouse();
#endif
    if ( !hadDblClick && offset == e->pos() ) {
	timer->start( TQApplication::doubleClickInterval(), true );
    } else if ( !hadDblClick ) {
	dockWindow->updatePosition( e->globalPos() );
    }
    if ( opaque )
	dockWindow->titleBar->mousePressed = false;
}

void TQDockWindowHandle::minimize()
{
    if ( !dockWindow->area() )
	return;

    TQMainWindow *mw = ::tqt_cast<TQMainWindow*>(dockWindow->area()->parentWidget());
    if ( mw && mw->isDockEnabled( dockWindow, TQt::DockMinimized ) )
	mw->moveDockWindow( dockWindow, TQt::DockMinimized );
}

void TQDockWindowHandle::resizeEvent( TQResizeEvent * )
{
    updateGui();
}

void TQDockWindowHandle::updateGui()
{
    if ( !closeButton ) {
	closeButton = new TQToolButton( this, "qt_close_button1" );
#ifndef TQT_NO_CURSOR
	closeButton->setCursor( arrowCursor );
#endif
	closeButton->setPixmap( style().stylePixmap( TQStyle::SP_DockWindowCloseButton, closeButton ) );
	closeButton->setFixedSize( 12, 12 );
	connect( closeButton, TQ_SIGNAL( clicked() ),
		 dockWindow, TQ_SLOT( hide() ) );
    }

    if ( dockWindow->isCloseEnabled() && dockWindow->area() )
	closeButton->show();
    else
	closeButton->hide();

    if ( !dockWindow->area() )
	return;

    if ( dockWindow->area()->orientation() == Horizontal ) {
	int off = ( width() - closeButton->width() - 1 ) / 2;
	closeButton->move( off, 2 );
    } else {
	int off = ( height() - closeButton->height() - 1 ) / 2;
	int x = TQApplication::reverseLayout() ? 2 : width() - closeButton->width() - 2;
	closeButton->move( x, off );
    }
}

#ifndef TQT_NO_STYLE
void TQDockWindowHandle::styleChange( TQStyle& )
{
    if ( closeButton )
	closeButton->setPixmap( style().stylePixmap( TQStyle::SP_DockWindowCloseButton, closeButton ) );
}
#endif

TQSize TQDockWindowHandle::minimumSizeHint() const
{
    if ( !dockWindow->dockArea )
	return TQSize( 0, 0 );
    int wh = dockWindow->isCloseEnabled() ? 17 : style().pixelMetric( TQStyle::PM_DockWindowHandleExtent, this );
    if ( dockWindow->orientation() == Horizontal )
	return TQSize( wh, 0 );
    return TQSize( 0, wh );
}

TQSizePolicy TQDockWindowHandle::sizePolicy() const
{
    if ( dockWindow->orientation() != Horizontal )
	return TQSizePolicy( TQSizePolicy::Preferred, TQSizePolicy::Fixed );
    return TQSizePolicy( TQSizePolicy::Fixed, TQSizePolicy::Preferred );
}

void TQDockWindowHandle::mouseDoubleClickEvent( TQMouseEvent *e )
{
    e->ignore();
    if ( e->button() != LeftButton )
	return;
    e->accept();
    timer->stop();
    emit doubleClicked();
    hadDblClick = true;
}

TQDockWindowTitleBar::TQDockWindowTitleBar( TQDockWindow *dw )
    : TQTitleBar( 0, dw, "qt_dockwidget_internal" ), dockWindow( dw ),
      mousePressed( false ), hadDblClick( false ), opaque( default_opaque )
{
    setWFlags( getWFlags() | WStyle_Tool );
    ctrlDown = false;
    setMouseTracking( true );
    setFixedHeight( style().pixelMetric( TQStyle::PM_TitleBarHeight, this ) );
    connect( this, TQ_SIGNAL(doClose()), dockWindow, TQ_SLOT(hide()) );
}

void TQDockWindowTitleBar::keyPressEvent( TQKeyEvent *e )
{
    if ( !mousePressed )
	return;
    if ( e->key() == Key_Control ) {
	ctrlDown = true;
	dockWindow->handleMove( mapFromGlobal( TQCursor::pos() ) - offset, TQCursor::pos(), !opaque );
    }
}

void TQDockWindowTitleBar::keyReleaseEvent( TQKeyEvent *e )
{
    if ( !mousePressed )
	return;
    if ( e->key() == Key_Control ) {
	ctrlDown = false;
	dockWindow->handleMove( mapFromGlobal( TQCursor::pos() ) - offset, TQCursor::pos(), !opaque );
    }
}

void TQDockWindowTitleBar::mousePressEvent( TQMouseEvent *e )
{
    TQStyle::SubControl tbctrl = style().querySubControl( TQStyle::CC_TitleBar, this, e->pos() );
    if ( tbctrl > TQStyle::SC_TitleBarLabel ) {
	TQTitleBar::mousePressEvent( e );
	return;
    }

    ctrlDown = ( e->state() & ControlButton ) == ControlButton;
    oldFocus = tqApp->focusWidget();
// setFocus activates the window, which deactivates the main window
// not what we want, and not required anyway on Windows
#ifndef TQ_WS_WIN
    setFocus();
#endif

    e->ignore();
    if ( e->button() != LeftButton )
	return;
    if ( e->y() < 3 && dockWindow->isResizeEnabled() )
	return;

    e->accept();
    bool oldPressed = mousePressed;
    mousePressed = true;
    hadDblClick = false;
    offset = e->pos();
    dockWindow->startRectDraw( mapToGlobal( e->pos() ), !opaque );
// grabMouse resets the Windows mouse press count, so we never receive a double click on Windows
// not required on Windows, and did work on X11, too, but no problem there in the first place
#ifndef TQ_WS_WIN
    if(!oldPressed && dockWindow->opaqueMoving())
	grabMouse();
#else
    Q_UNUSED( oldPressed );
#endif
}

void TQDockWindowTitleBar::mouseMoveEvent( TQMouseEvent *e )
{
    if ( !mousePressed ) {
	TQTitleBar::mouseMoveEvent( e );
	return;
    }

    ctrlDown = ( e->state() & ControlButton ) == ControlButton;
    e->accept();
    dockWindow->handleMove( e->pos() - offset, e->globalPos(), !opaque );
}

void TQDockWindowTitleBar::mouseReleaseEvent( TQMouseEvent *e )
{
    if ( !mousePressed ) {
	TQTitleBar::mouseReleaseEvent( e );
	return;
    }

    ctrlDown = false;
    tqApp->removeEventFilter( dockWindow );
    if ( oldFocus )
	oldFocus->setFocus();

    if ( dockWindow->place() == TQDockWindow::OutsideDock )
	dockWindow->raise();

    if(dockWindow->opaqueMoving())
	releaseMouse();
    if ( !mousePressed )
	return;
    dockWindow->endRectDraw( !opaque );
    mousePressed = false;
    if ( !hadDblClick )
	dockWindow->updatePosition( e->globalPos() );
    if ( opaque ) {
	dockWindow->horHandle->mousePressed = false;
	dockWindow->verHandle->mousePressed = false;
    }
}

void TQDockWindowTitleBar::resizeEvent( TQResizeEvent *e )
{
    updateGui();
    TQTitleBar::resizeEvent( e );
}

void TQDockWindowTitleBar::updateGui()
{
    if ( dockWindow->isCloseEnabled() ) {
	setWFlags( getWFlags() | WStyle_SysMenu );
    } else {
	setWFlags( getWFlags() & ~WStyle_SysMenu );
    }
}

void TQDockWindowTitleBar::mouseDoubleClickEvent( TQMouseEvent * )
{
    emit doubleClicked();
    hadDblClick = true;
}

/*!
    \class TQDockWindow ntqdockwindow.h
    \brief The TQDockWindow class provides a widget which can be docked
    inside a TQDockArea or floated as a top level window on the
    desktop.

    \ingroup application
    \mainclass

    This class handles moving, resizing, docking and undocking dock
    windows. TQToolBar is a subclass of TQDockWindow so the
    functionality provided for dock windows is available with the same
    API for toolbars.

    \img qmainwindow-qdockareas.png TQDockWindows in a TQDockArea
    \caption Two TQDockWindows (\l{TQToolBar}s) in a \l TQDockArea

    \img qdockwindow.png A TQDockWindow
    \caption A Floating TQDockWindow

    If the user drags the dock window into the dock area the dock
    window will be docked. If the user drags the dock area outside any
    dock areas the dock window will be undocked (floated) and will
    become a top level window. Double clicking a floating dock
    window's titlebar will dock the dock window to the last dock area
    it was docked in. Double clicking a docked dock window's handle
    will undock (float) the dock window.
    \omit
    Single clicking a docked dock window's handle will minimize the
    dock window (only its handle will appear, below the menu bar).
    Single clicking the minimized handle will restore the dock window
    to the last dock area that it was docked in.
    \endomit
    If the user clicks the close button (which does not appear on
    dock windows by default - see \l closeMode) the dock window will
    disappear. You can control whether or not a dock window has a
    close button with setCloseMode().

    TQMainWindow provides four dock areas (top, left, right and bottom)
    which can be used by dock windows. For many applications using the
    dock areas provided by TQMainWindow is sufficient. (See the \l
    TQDockArea documentation if you want to create your own dock
    areas.) In TQMainWindow a right-click popup menu (the dock window
    menu) is available which lists dock windows and can be used to
    show or hide them. (The popup menu only lists dock windows that
    have a \link setCaption() caption\endlink.)

    When you construct a dock window you \e must pass it a TQDockArea
    or a TQMainWindow as its parent if you want it docked. Pass 0 for
    the parent if you want it floated.

    \code
    TQToolBar *fileTools = new TQToolBar( this, "File Actions" );
    moveDockWindow( fileTools, Left );
    \endcode

    In the example above we create a new TQToolBar in the constructor
    of a TQMainWindow subclass (so that the \e this pointer points to
    the TQMainWindow). By default the toolbar will be added to the \c
    Top dock area, but we've moved it to the \c Left dock area.

    A dock window is often used to contain a single widget. In these
    cases the widget can be set by calling setWidget(). If you're
    constructing a dock window that contains multiple widgets, e.g. a
    toolbar, arrange the widgets within a box layout inside the dock
    window. To do this use the boxLayout() function to get a pointer
    to the dock window's box layout, then add widgets to the layout
    using the box layout's TQBoxLayout::addWidget() function. The dock
    window will dynamically set the orientation of the layout to be
    vertical or horizontal as necessary, although you can control this
    yourself with setOrientation().

    Although a common use of dock windows is for toolbars, they can be
    used with any widgets. (See the \link designer-manual.book TQt
    Designer\endlink and \link linguist-manual.book TQt
    Linguist\endlink applications, for example.) When using larger
    widgets it may make sense for the dock window to be resizable by
    calling setResizeEnabled(). Resizable dock windows are given
    splitter-like handles to allow the user to resize them within
    their dock area. When resizable dock windows are undocked they
    become top level windows and can be resized like any other top
    level windows, e.g. by dragging a corner or edge.

    Dock windows can be docked and undocked using dock() and undock().
    A dock window's orientation can be set with setOrientation(). You
    can also use TQDockArea::moveDockWindow(). If you're using a
    TQMainWindow, TQMainWindow::moveDockWindow() and
    TQMainWindow::removeDockWindow() are available.

    A dock window can have some preferred settings, for example, you
    can set a preferred offset from the left edge (or top edge for
    vertical dock areas) of the dock area using setOffset(). If you'd
    prefer a dock window to start on a new \link ntqdockarea.html#lines
    line\endlink when it is docked use setNewLine(). The
    setFixedExtentWidth() and setFixedExtentHeight() functions can be
    used to define the dock window's preferred size, and the
    setHorizontallyStretchable() and setVerticallyStretchable()
    functions set whether the dock window can be stretched or not.
    Dock windows can be moved by default, but this can be changed with
    setMovingEnabled(). When a dock window is moved it is shown as a
    rectangular outline, but it can be shown normally using
    setOpaqueMoving().

    When a dock window's visibility changes, i.e. it is shown or
    hidden, the visibilityChanged() signal is emitted. When a dock
    window is docked, undocked or moved inside the dock area the
    placeChanged() signal is emitted.
*/

/*!
    \enum TQDockWindow::Place

    This enum specifies the possible locations for a TQDockWindow:

    \value InDock  Inside a TQDockArea.
    \value OutsideDock  Floating as a top level window on the desktop.
*/

/*!
    \enum TQDockWindow::CloseMode

    This enum type specifies when (if ever) a dock window has a close
    button.

    \value Never  The dock window never has a close button and cannot
    be closed by the user.
    \value Docked  The dock window has a close button only when
    docked.
    \value Undocked  The dock window has a close button only when
    floating.
    \value Always The dock window always has a close button.
    \omit
    Note that dock windows can always be minimized if the user clicks
    their dock window handle when they are docked.
    \endomit
*/

/*!
    \fn void TQDockWindow::setHorizontalStretchable( bool b )
    \obsolete
*/
/*!
    \fn void TQDockWindow::setVerticalStretchable( bool b )
    \obsolete
*/
/*!
    \fn bool TQDockWindow::isHorizontalStretchable() const
    \obsolete
*/
/*!
    \fn bool TQDockWindow::isVerticalStretchable() const
    \obsolete
*/
/*!
    \fn void TQDockWindow::orientationChanged( Orientation o )

    This signal is emitted when the orientation of the dock window is
    changed. The new orientation is \a o.
*/

/*!
    \fn void TQDockWindow::placeChanged( TQDockWindow::Place p )

    This signal is emitted when the dock window is docked (\a p is \c
    InDock), undocked (\a p is \c OutsideDock) or moved inside the
    the dock area.

    \sa TQDockArea::moveDockWindow(), TQDockArea::removeDockWindow(),
    TQMainWindow::moveDockWindow(), TQMainWindow::removeDockWindow()
*/

/*!
    \fn void TQDockWindow::visibilityChanged( bool visible )

    This signal is emitted when the visibility of the dock window
    relatively to its dock area is changed. If \a visible is true, the
    TQDockWindow is now visible to the dock area, otherwise it has been
    hidden.

    A dock window can be hidden if it has a close button which the
    user has clicked. In the case of a TQMainWindow a dock window can
    have its visibility changed (hidden or shown) by clicking its name
    in the dock window menu that lists the TQMainWindow's dock windows.
*/

/*!
    \fn TQDockArea *TQDockWindow::area() const

    Returns the dock area in which this dock window is docked, or 0 if
    the dock window is floating.
*/

// DOC: Can't use \property 'cos it thinks the thing returns a bool.
/*!
    \fn Place TQDockWindow::place() const

    This function returns where the dock window is placed. This is
    either \c InDock or \c OutsideDock.

    \sa TQDockArea::moveDockWindow(), TQDockArea::removeDockWindow(),
    TQMainWindow::moveDockWindow(), TQMainWindow::removeDockWindow()
*/


/*!
    Constructs a TQDockWindow with parent \a parent, called \a name and
    with widget flags \a f.
*/

TQDockWindow::TQDockWindow( TQWidget* parent, const char* name, WFlags f )
    : TQFrame( parent, name, f | WType_Dialog | WStyle_Customize | WStyle_NoBorder )
{
    curPlace = InDock;
    isToolbar = false;
    init();
}

/*!
    Constructs a TQDockWindow with parent \a parent, called \a name and
    with widget flags \a f.

    If \a p is \c InDock, the dock window is docked into a dock area
    and \a parent \e must be a TQDockArea or a TQMainWindow. If the \a
    parent is a TQMainWindow the dock window will be docked in the main
    window's \c Top dock area.

    If \a p is \c OutsideDock, the dock window is created as a floating
    window.

    We recommend creating the dock area \c InDock with a TQMainWindow
    as parent then calling TQMainWindow::moveDockWindow() to move the
    dock window where you want it.
*/

TQDockWindow::TQDockWindow( Place p, TQWidget *parent, const char *name, WFlags f )
    : TQFrame( parent, name, f | WType_Dialog | WStyle_Customize | WStyle_NoBorder )
{
    curPlace = p;
    isToolbar = false;
    init();
}

/*! \internal
*/

TQDockWindow::TQDockWindow( Place p, TQWidget *parent, const char *name, WFlags f, bool toolbar )
    : TQFrame( parent, name, f | WType_Dialog | WStyle_Customize | WStyle_NoBorder )
{
    curPlace = p;
    isToolbar = toolbar;
    init();
}

class TQDockWindowGridLayout : public TQGridLayout
{
public:
    TQDockWindowGridLayout( TQWidget *parent, int nRows, int nCols )
	: TQGridLayout( parent, nRows, nCols ) {};

    TQSizePolicy::ExpandData expanding() const
    {
	return TQSizePolicy::NoDirection;
    }
};

void TQDockWindow::init()
{
    wid = 0;
    unclippedPainter = 0;
    dockArea = 0;
    tmpDockArea = 0;
    resizeEnabled = false;
    moveEnabled = true;
    nl = false;
    opaque = default_opaque;
    cMode = Never;
    offs = 0;
    fExtent = TQSize( -1, -1 );
    dockWindowData = 0;
    lastPos = TQPoint( -1, -1 );
    lastSize = TQSize( -1, -1 );

    widgetResizeHandler = new TQWidgetResizeHandler( this );
    widgetResizeHandler->setMovingEnabled( false );

    titleBar      = new TQDockWindowTitleBar( this );
    verHandle     = new TQDockWindowHandle( this );
    horHandle     = new TQDockWindowHandle( this );

    vHandleLeft   = new TQDockWindowResizeHandle( TQt::Vertical, this, this, "vert. handle" );
    vHandleRight  = new TQDockWindowResizeHandle( TQt::Vertical, this, this, "vert. handle" );
    hHandleTop    = new TQDockWindowResizeHandle( TQt::Horizontal, this, this, "horz. handle" );
    hHandleBottom = new TQDockWindowResizeHandle( TQt::Horizontal, this, this, "horz. handle" );

    // Creating inner layout
    hbox	  = new TQVBoxLayout();
    vbox	  = new TQHBoxLayout();
    childBox	  = new TQBoxLayout(TQBoxLayout::LeftToRight);
    vbox->addWidget( verHandle );
    vbox->addLayout( childBox );

    hbox->setResizeMode( TQLayout::FreeResize );
    hbox->setMargin( isResizeEnabled() || curPlace == OutsideDock ? 2 : 0 );
    hbox->setSpacing( 1 );
    hbox->addWidget( titleBar );
    hbox->addWidget( horHandle );
    hbox->addLayout( vbox );

    // Set up the initial handle layout for Vertical
    // Handle layout will change on calls to setOrienation()
    TQGridLayout *glayout = new TQDockWindowGridLayout( this, 3, 3 );
    glayout->setResizeMode( TQLayout::Minimum );
    glayout->addMultiCellWidget( hHandleTop,    0, 0, 1, 1 );
    glayout->addMultiCellWidget( hHandleBottom, 2, 2, 1, 1 );
    glayout->addMultiCellWidget( vHandleLeft,   0, 2, 0, 0 );
    glayout->addMultiCellWidget( vHandleRight,  0, 2, 2, 2 );
    glayout->addLayout( hbox, 1, 1 );
    glayout->setRowStretch( 1, 1 );
    glayout->setColStretch( 1, 1 );

    hHandleBottom->hide();
    vHandleRight->hide();
    hHandleTop->hide();
    vHandleLeft->hide();
    setFrameStyle( TQFrame::StyledPanel | TQFrame::Raised );
    setLineWidth( 2 );

    if ( parentWidget() )
	parentWidget()->installEventFilter( this );
    TQWidget *mw = parentWidget();
    TQDockArea *da = ::tqt_cast<TQDockArea*>(parentWidget());
    if ( da ) {
	if ( curPlace == InDock )
	    da->moveDockWindow( this );
	mw = da->parentWidget();
    }
    if ( ::tqt_cast<TQMainWindow*>(mw) ) {
	if ( place() == InDock ) {
	    Dock myDock = TQt::DockTop;
	    // make sure we put the window in the correct dock.
	    if ( dockArea ) {
		TQMainWindow *mainw = (TQMainWindow*)mw;
		// I'm not checking if it matches the top because I've
		// done the assignment to it above.
		if ( dockArea == mainw->leftDock() )
		    myDock = TQt::DockLeft;
		else if ( dockArea == mainw->rightDock() )
		    myDock = TQt::DockRight;
		else if ( dockArea == mainw->bottomDock() )
		    myDock = TQt::DockBottom;
	    }
	    ( (TQMainWindow*)mw )->addDockWindow( this, myDock );
	}
	moveEnabled = ((TQMainWindow*)mw)->dockWindowsMovable();
	opaque = ((TQMainWindow*)mw)->opaqueMoving();
    }

    updateGui();
    stretchable[ Horizontal ] = false;
    stretchable[ Vertical ] = false;

    connect( titleBar, TQ_SIGNAL( doubleClicked() ), this, TQ_SLOT( dock() ) );
    connect( verHandle, TQ_SIGNAL( doubleClicked() ), this, TQ_SLOT( undock() ) );
    connect( horHandle, TQ_SIGNAL( doubleClicked() ), this, TQ_SLOT( undock() ) );
    connect( this, TQ_SIGNAL( orientationChanged(Orientation) ),
	     this, TQ_SLOT( setOrientation(Orientation) ) );
}

/*!
    Sets the orientation of the dock window to \a o. The orientation
    is propagated to the layout boxLayout().

    \warning All undocked TQToolBars will always have a horizontal orientation.
*/

void TQDockWindow::setOrientation( Orientation o )
{
    TQGridLayout *glayout = (TQGridLayout*)layout();
    glayout->remove( hHandleTop );
    glayout->remove( hHandleBottom );
    glayout->remove( vHandleLeft );
    glayout->remove( vHandleRight );

    if ( o == Horizontal ) {
	// Set up the new layout as
	//   3 3 3      1 = vHandleLeft   4 = hHandleBottom
	//   1 X 2      2 = vHandleRight  X = Inner Layout
	//   4 4 4      3 = hHandleTop
	glayout->addMultiCellWidget( hHandleTop,    0, 0, 0, 2 );
	glayout->addMultiCellWidget( hHandleBottom, 2, 2, 0, 2 );
	glayout->addMultiCellWidget( vHandleLeft,   1, 1, 0, 0 );
	glayout->addMultiCellWidget( vHandleRight,  1, 1, 2, 2 );
    } else {
	// Set up the new layout as
	//   1 3 2      1 = vHandleLeft   4 = hHandleBottom
	//   1 X 2      2 = vHandleRight  X = Inner Layout
	//   1 4 2      3 = hHandleTop
	glayout->addMultiCellWidget( hHandleTop,    0, 0, 1, 1 );
	glayout->addMultiCellWidget( hHandleBottom, 2, 2, 1, 1 );
	glayout->addMultiCellWidget( vHandleLeft,   0, 2, 0, 0 );
	glayout->addMultiCellWidget( vHandleRight,  0, 2, 2, 2 );
    }
    boxLayout()->setDirection( o == Horizontal ? TQBoxLayout::LeftToRight : TQBoxLayout::TopToBottom );
    TQApplication::sendPostedEvents( this, TQEvent::LayoutHint );
    TQEvent *e = new TQEvent( TQEvent::LayoutHint );
    TQApplication::postEvent( this, e );
}

/*!
    \reimp

    Destroys the dock window and its child widgets.
*/

TQDockWindow::~TQDockWindow()
{
    tqApp->removeEventFilter( this );
    if ( area() )
	area()->removeDockWindow( this, false, false );
    TQDockArea *a = area();
    if ( !a && dockWindowData )
	a = ( (TQDockArea::DockWindowData*)dockWindowData )->area;
    TQMainWindow *mw = a ? ::tqt_cast<TQMainWindow*>(a->parentWidget()) : 0;
    if ( mw )
	mw->removeDockWindow( this );

    delete (TQDockArea::DockWindowData*)dockWindowData;
}

/*!  \reimp
*/

void TQDockWindow::resizeEvent( TQResizeEvent *e )
{
    TQFrame::resizeEvent( e );
    updateGui();
}


void TQDockWindow::swapRect( TQRect &r, TQt::Orientation o, const TQPoint &offset, TQDockArea * )
{
    TQBoxLayout *bl = boxLayout()->createTmpCopy();
    bl->setDirection( o == Horizontal ? TQBoxLayout::LeftToRight : TQBoxLayout::TopToBottom );
    bl->activate();
    r.setSize( bl->sizeHint() );
    bl->data = 0;
    delete bl;
    bool reverse = TQApplication::reverseLayout();
    if ( o == TQt::Horizontal )
	r.moveBy( -r.width()/2, 0 );
    else
	r.moveBy( reverse ? - r.width() : 0, -r.height() / 2  );
    r.moveBy( offset.x(), offset.y() );
}

TQWidget *TQDockWindow::areaAt( const TQPoint &gp )
{
    TQWidget *w = tqApp->widgetAt( gp, true );

    if ( w && ( w == this || w == titleBar ) && parentWidget() )
	w = parentWidget()->childAt( parentWidget()->mapFromGlobal( gp ) );

    while ( w ) {
	if ( ::tqt_cast<TQDockArea*>(w) ) {
	    TQDockArea *a = (TQDockArea*)w;
	    if ( a->isDockWindowAccepted( this ) )
		return w;
	}
	if ( ::tqt_cast<TQMainWindow*>(w) ) {
	    TQMainWindow *mw = (TQMainWindow*)w;
	    TQDockArea *a = mw->dockingArea( mw->mapFromGlobal( gp ) );
	    if ( a && a->isDockWindowAccepted( this ) )
		return a;
	}
	w = w->parentWidget( true );
    }
    return 0;
}

void TQDockWindow::handleMove( const TQPoint &pos, const TQPoint &gp, bool drawRect )
{
    if ( !unclippedPainter )
	return;

    if ( drawRect ) {
	TQRect dr(currRect);
#ifdef MAC_DRAG_HACK
	dr.moveBy(-topLevelWidget()->geometry().x(), -topLevelWidget()->geometry().y());
#endif
	unclippedPainter->drawRect( dr );
    }
    currRect = TQRect( realWidgetPos( this ), size() );
    TQWidget *w = areaAt( gp );
    if ( titleBar->ctrlDown || horHandle->ctrlDown || verHandle->ctrlDown )
	w = 0;
    currRect.moveBy( pos.x(), pos.y() );
    if ( !::tqt_cast<TQDockArea*>(w) ) {
        if ( startOrientation != Horizontal && ::tqt_cast<TQToolBar*>(this) )
	    swapRect( currRect, Horizontal, startOffset, (TQDockArea*)w );
	if ( drawRect ) {
	    unclippedPainter->setPen( TQPen( gray, 3 ) );
	    TQRect dr(currRect);
#ifdef MAC_DRAG_HACK
	    dr.moveBy(-topLevelWidget()->geometry().x(), -topLevelWidget()->geometry().y());
#endif
	    unclippedPainter->drawRect( dr );
	} else {
	    TQPoint mp( mapToGlobal( pos ));
	    if(place() == InDock) {
		undock();
		if(titleBar) {
		    mp = TQPoint(titleBar->width() / 2, titleBar->height() / 2);
		    TQMouseEvent me(TQEvent::MouseButtonPress, mp, LeftButton, 0);
		    TQApplication::sendEvent(titleBar, &me);
		    mp = titleBar->mapToGlobal( mp );
		}
	    }
	    move( mp );
	}
	state = OutsideDock;
        return;
    }

    TQDockArea *area = (TQDockArea*)w;
    if( area->isVisible() ) {
        state = InDock;
	Orientation o = ( area ? area->orientation() :
			  ( boxLayout()->direction() == TQBoxLayout::LeftToRight ||
			    boxLayout()->direction() == TQBoxLayout::RightToLeft ?
			    Horizontal : Vertical ) );
	if ( startOrientation != o )
	    swapRect( currRect, o, startOffset, area );
	if ( drawRect ) {
	    unclippedPainter->setPen( TQPen( gray, 1 ) );
	    TQRect dr(currRect);
#ifdef MAC_DRAG_HACK
	    dr.moveBy(-topLevelWidget()->geometry().x(), -topLevelWidget()->geometry().y());
#endif
	    unclippedPainter->drawRect( dr );
	}
	tmpDockArea = area;
    }
}

void TQDockWindow::updateGui()
{
    if ( curPlace == OutsideDock ) {
	hbox->setMargin( 2 );
	horHandle->hide();
	verHandle->hide();
	if ( moveEnabled )
	    titleBar->show();
	else
	    titleBar->hide();
	titleBar->updateGui();
	hHandleTop->hide();
	vHandleLeft->hide();
	hHandleBottom->hide();
	vHandleRight->hide();
	setLineWidth( 2 );
	widgetResizeHandler->setActive( isResizeEnabled() );
    } else {
	hbox->setMargin( isResizeEnabled() ? 0 : 2 );
	titleBar->hide();
	if ( orientation() == Horizontal ) {
	    horHandle->hide();
	    if ( moveEnabled )
		verHandle->show();
	    else
		verHandle->hide();
#ifdef TQ_WS_MAC
	    if(horHandle->mousePressed) {
		horHandle->mousePressed = false;
		verHandle->mousePressed = true;
		verHandle->grabMouse();
	    }
#endif
	    verHandle->updateGui();
	} else {
	    if ( moveEnabled )
		horHandle->show();
	    else
		horHandle->hide();
	    horHandle->updateGui();
#ifdef TQ_WS_MAC
	    if(verHandle->mousePressed) {
		verHandle->mousePressed = false;
		horHandle->mousePressed = true;
		horHandle->grabMouse();
	    }
#endif
	    verHandle->hide();
	}
	if ( isResizeEnabled() ) {
	    if ( orientation() == Horizontal ) {
		hHandleBottom->raise();
		hHandleTop->raise();
	    } else {
		vHandleRight->raise();
		vHandleLeft->raise();
	    }

	    if ( area() ) {
		if ( orientation() == Horizontal ) {
		    if ( area()->handlePosition() == TQDockArea::Normal ) {
			hHandleBottom->show();
			hHandleTop->hide();
		    } else {
			hHandleTop->show();
			hHandleBottom->hide();
		    }
		    if ( !area()->isLastDockWindow( this ) )
			vHandleRight->show();
		    else
			vHandleRight->hide();
		    vHandleLeft->hide();
		} else {
		    if ( (area()->handlePosition() == TQDockArea::Normal) != TQApplication::reverseLayout() ) {
			vHandleRight->show();
			vHandleLeft->hide();
		    } else {
			vHandleLeft->show();
			vHandleRight->hide();
		    }
		    if ( !area()->isLastDockWindow( this ) )
			hHandleBottom->show();
		    else
			hHandleBottom->hide();
		    hHandleTop->hide();
		}
	    }
	} else if ( area() ) { // hide resize handles if resizing is disabled
            if ( orientation() == Horizontal ) {
                hHandleTop->hide();
                hHandleBottom->hide();
            } else {
                vHandleLeft->hide();
                vHandleRight->hide();
            }
        }
#ifndef Q_OS_TEMP
	if ( moveEnabled )
	    setLineWidth( 1 );
	else
	    setLineWidth( 0 );
	hbox->setMargin( lineWidth() );
#else
	hbox->setMargin( 2 );
#endif
	widgetResizeHandler->setActive( false );
    }
}

void TQDockWindow::updatePosition( const TQPoint &globalPos )
{
    if ( curPlace == OutsideDock && state == InDock )
	lastSize = size();

    bool doAdjustSize = curPlace != state && state == OutsideDock;
    bool doUpdate = true;
    bool doOrientationChange = true;
    if ( state != curPlace && state == InDock ) {
        doUpdate = false;
        curPlace = state;
	updateGui();
	TQApplication::sendPostedEvents();
    }
    Orientation oo = orientation();

    if ( state == InDock ) {
	if ( tmpDockArea ) {
	    bool differentDocks = false;
	    if ( dockArea && dockArea != tmpDockArea ) {
		differentDocks = true;
		delete (TQDockArea::DockWindowData*)dockWindowData;
		dockWindowData = dockArea->dockWindowData( this );
		dockArea->removeDockWindow( this, false, false );
	    }
	    dockArea = tmpDockArea;
	    if ( differentDocks ) {
		if ( doUpdate ) {
		    doUpdate = false;
                    curPlace = state;
		    updateGui();
		}
		emit orientationChanged( tmpDockArea->orientation() );
		doOrientationChange = false;
	    } else {
		updateGui();
	    }
	    dockArea->moveDockWindow( this, globalPos, currRect, startOrientation != oo );
	}
    } else {
	if ( dockArea ) {
	    TQMainWindow *mw = (TQMainWindow*)dockArea->parentWidget();
	    if ( ::tqt_cast<TQMainWindow*>(mw) &&
		 ( !mw->isDockEnabled( TQMainWindow::DockTornOff ) ||
		   !mw->isDockEnabled( this, TQMainWindow::DockTornOff ) ) )
		return;
	    delete (TQDockArea::DockWindowData*)dockWindowData;
	    dockWindowData = dockArea->dockWindowData( this );
	    dockArea->removeDockWindow( this, true,
                                        startOrientation != Horizontal && ::tqt_cast<TQToolBar*>(this) );
	}
	dockArea = 0;
	TQPoint topLeft = currRect.topLeft();
	TQRect screen = tqApp->desktop()->availableGeometry( topLeft );
	if ( !screen.contains( topLeft ) ) {
	    topLeft.setY(TQMAX(topLeft.y(), screen.top()));
	    topLeft.setY(TQMIN(topLeft.y(), screen.bottom()-height()));
	    topLeft.setX(TQMAX(topLeft.x(), screen.left()));
	    topLeft.setX(TQMIN(topLeft.x(), screen.right()-width()));
	}
	move( topLeft );
    }

    if ( curPlace == InDock && state == OutsideDock && !::tqt_cast<TQToolBar*>(this) ) {
	if ( lastSize != TQSize( -1, -1 ) )
	    resize( lastSize );
    }

    if ( doUpdate ) {
        curPlace = state;
	updateGui();
    }
    if ( doOrientationChange )
	emit orientationChanged( orientation() );
    tmpDockArea = 0;
    if ( doAdjustSize ) {
	TQApplication::sendPostedEvents( this, TQEvent::LayoutHint );
	if ( ::tqt_cast<TQToolBar*>(this) )
	    adjustSize();
        if (lastSize == TQSize(-1, -1))
            clearWState(WState_Resized); // Ensures size is recalculated (non-opaque).
	show();
	if ( parentWidget() && isTopLevel() )
	    parentWidget()->setActiveWindow();

    }

    emit placeChanged( curPlace );
}

/*!
    Sets the dock window's main widget to \a w.

    \sa boxLayout()
*/

void TQDockWindow::setWidget( TQWidget *w )
{
    wid = w;
    boxLayout()->addWidget( w );
    updateGui();
}

/*!
    Returns the dock window's main widget.

    \sa setWidget()
*/

TQWidget *TQDockWindow::widget() const
{
    return wid;
}

void TQDockWindow::startRectDraw( const TQPoint &so, bool drawRect )
{
    state = place();
    if ( unclippedPainter )
	endRectDraw( !opaque );
#ifdef MAC_DRAG_HACK
    TQWidget *paint_on = topLevelWidget();
#else
    int scr = TQApplication::desktop()->screenNumber( this );
    TQWidget *paint_on = TQApplication::desktop()->screen( scr );
#endif
    unclippedPainter = new TQPainter( paint_on, true );
    unclippedPainter->setPen( TQPen( gray, curPlace == OutsideDock ? 3 : 1 ) );
    unclippedPainter->setRasterOp( XorROP );
    currRect = TQRect( realWidgetPos( this ), size() );
    if ( drawRect ) {
	TQRect dr(currRect);
#ifdef MAC_DRAG_HACK
	dr.moveBy(-topLevelWidget()->geometry().x(), -topLevelWidget()->geometry().y());
#endif
	unclippedPainter->drawRect( dr );
    }
    startOrientation = orientation();
    startOffset = mapFromGlobal( so );
}

void TQDockWindow::endRectDraw( bool drawRect )
{
    if ( !unclippedPainter )
	return;
    if ( drawRect ) {
	TQRect dr(currRect);
#ifdef MAC_DRAG_HACK
	dr.moveBy(-topLevelWidget()->geometry().x(), -topLevelWidget()->geometry().y());
#endif
	unclippedPainter->drawRect( dr );
    }
    delete unclippedPainter;
    unclippedPainter = 0;
}

/*!
  \reimp
*/
void TQDockWindow::drawFrame( TQPainter *p )
{
    if ( place() == InDock ) {
	TQFrame::drawFrame( p );
	return;
    }

    TQStyle::SFlags flags = TQStyle::Style_Default;
    TQStyleOption opt(lineWidth(),midLineWidth());

    if ( titleBar->isActive() )
	flags |= TQStyle::Style_Active;

    style().drawPrimitive( TQStyle::PE_WindowFrame, p, rect(), colorGroup(), flags, opt );
}

/*!
  \reimp
*/
void TQDockWindow::drawContents( TQPainter *p )
{
    TQStyle::SFlags flags = TQStyle::Style_Default;
    if ( titleBar->isActive() )
	flags |= TQStyle::Style_Active;
    style().drawControl( TQStyle::CE_DockWindowEmptyArea, p, this,
			 rect(), colorGroup(), flags );
}

/*!
    \property TQDockWindow::resizeEnabled
    \brief whether the dock window is resizeable

    A resizeable dock window can be resized using splitter-like
    handles inside a dock area and like every other top level window
    when floating.

    A dock window is both horizontally and vertically stretchable if
    you call setResizeEnabled(true).

    This property is false by default.

    \sa setVerticallyStretchable() setHorizontallyStretchable()
*/

void TQDockWindow::setResizeEnabled( bool b )
{
    resizeEnabled = b;
    hbox->setMargin( b ? 0 : 2 );
    updateGui();
}

/*!
    \property TQDockWindow::movingEnabled
    \brief whether the user can move the dock window within the dock
    area, move the dock window to another dock area, or float the dock
    window.

    This property is true by default.
*/

void TQDockWindow::setMovingEnabled( bool b )
{
    moveEnabled = b;
    updateGui();
}

bool TQDockWindow::isResizeEnabled() const
{
    return resizeEnabled;
}

bool TQDockWindow::isMovingEnabled() const
{
    return moveEnabled;
}

/*!
    \property TQDockWindow::closeMode
    \brief the close mode of a dock window

    Defines when (if ever) the dock window has a close button. The
    choices are \c Never, \c Docked (i.e. only when docked), \c
    Undocked (only when undocked, i.e. floated) or \c Always.

    The default is \c Never.
*/

void TQDockWindow::setCloseMode( int m )
{
    cMode = m;
    if ( place() == InDock ) {
	horHandle->updateGui();
	verHandle->updateGui();
    } else {
	titleBar->updateGui();
    }
}

/*!
    Returns true if the dock window has a close button; otherwise
    returns false. The result depends on the dock window's \l Place
    and its \l CloseMode.

    \sa setCloseMode()
*/

bool TQDockWindow::isCloseEnabled() const
{
    return  ( ( ( cMode & Docked ) == Docked && place() == InDock ) ||
	      ( ( cMode & Undocked ) == Undocked && place() == OutsideDock ) );
}

int TQDockWindow::closeMode() const
{
    return cMode;
}

/*!
    \property TQDockWindow::horizontallyStretchable
    \brief whether the dock window is horizontally stretchable.

    A dock window is horizontally stretchable if you call
    setHorizontallyStretchable(true) or setResizeEnabled(true).

    \sa setResizeEnabled()

    \bug Strecthability is broken. You must call setResizeEnabled(true) to get
    proper behavior and even then TQDockWindow does not limit stretchablilty.
*/

void TQDockWindow::setHorizontallyStretchable( bool b )
{
    stretchable[ Horizontal ] = b;
}

/*!
    \property TQDockWindow::verticallyStretchable
    \brief whether the dock window is vertically stretchable.

    A dock window is vertically stretchable if you call
    setVerticallyStretchable(true) or setResizeEnabled(true).

    \sa setResizeEnabled()

    \bug Strecthability is broken. You must call setResizeEnabled(true) to get
    proper behavior and even then TQDockWindow does not limit stretchablilty.
*/

void TQDockWindow::setVerticallyStretchable( bool b )
{
    stretchable[ Vertical ] = b;
}

bool TQDockWindow::isHorizontallyStretchable() const
{
    return isResizeEnabled() || stretchable[ Horizontal ];
}

bool TQDockWindow::isVerticallyStretchable() const
{
    return isResizeEnabled() || stretchable[ Vertical ];
}

/*!
    \property TQDockWindow::stretchable
    \brief whether the dock window is stretchable in the current
    orientation()

    This property can be set using setHorizontallyStretchable() and
    setVerticallyStretchable(), or with setResizeEnabled().

    \sa setResizeEnabled()

    \bug Strecthability is broken. You must call setResizeEnabled(true) to get
    proper behavior and even then TQDockWindow does not limit stretchablilty.
*/

bool TQDockWindow::isStretchable() const
{
    if ( orientation() == Horizontal )
	return isHorizontallyStretchable();
    return isVerticallyStretchable();
}

/*!
    Returns the orientation of the dock window.

    \sa orientationChanged()
*/

TQt::Orientation TQDockWindow::orientation() const
{
    if ( dockArea )
	return dockArea->orientation();
    if ( ::tqt_cast<TQToolBar*>(this) )
	return Horizontal;
    return ( ((TQDockWindow*)this)->boxLayout()->direction() == TQBoxLayout::LeftToRight ||
	     ((TQDockWindow*)this)->boxLayout()->direction() == TQBoxLayout::RightToLeft ?
	     Horizontal : Vertical );
}

int TQDockWindow::offset() const
{
    return offs;
}

/*!
    \property TQDockWindow::offset
    \brief the dock window's preferred offset from the dock area's
    left edge (top edge for vertical dock areas)

    The default is 0.
*/

void TQDockWindow::setOffset( int o )
{
    offs = o;
}

/*!
    Returns the dock window's preferred size (fixed extent).

    \sa setFixedExtentWidth() setFixedExtentHeight()
*/

TQSize TQDockWindow::fixedExtent() const
{
    return fExtent;
}

/*!
    Sets the dock window's preferred width for its fixed extent (size)
    to \a w.

    \sa setFixedExtentHeight()
*/

void TQDockWindow::setFixedExtentWidth( int w )
{
    fExtent.setWidth( w );
}

/*!
    Sets the dock window's preferred height for its fixed extent
    (size) to \a h.

    \sa setFixedExtentWidth()
*/

void TQDockWindow::setFixedExtentHeight( int h )
{
    fExtent.setHeight( h );
}

/*!
    \property TQDockWindow::newLine
    \brief whether the dock window prefers to start a new line in the
    dock area.

    The default is false, i.e. the dock window doesn't require a new
    line in the dock area.
*/

void TQDockWindow::setNewLine( bool b )
{
    nl = b;
}

bool TQDockWindow::newLine() const
{
    return nl;
}

/*!
    Returns the layout which is used for adding widgets to the dock
    window. The layout's orientation is set automatically to match the
    orientation of the dock window. You can add widgets to the layout
    using the box layout's TQBoxLayout::addWidget() function.

    If the dock window only needs to contain a single widget use
    setWidget() instead.

    \sa setWidget() setOrientation()
*/

TQBoxLayout *TQDockWindow::boxLayout()
{
    return childBox;
}

/*! \reimp
 */

TQSize TQDockWindow::sizeHint() const
{
    TQSize sh( TQFrame::sizeHint() );
    if ( place() == InDock )
	sh = sh.expandedTo( fixedExtent() );
    sh = sh.expandedTo( TQSize( 16, 16 ) );
    if ( area() ) {
	if ( area()->orientation() == Horizontal && !vHandleRight->isVisible() )
	    sh.setWidth( sh.width() + 2 * style().pixelMetric(TQStyle::PM_SplitterWidth, this) / 3 );
	else if ( area()->orientation() == Vertical && !hHandleBottom->isVisible() )
	    sh.setHeight( sh.height() + 2 * style().pixelMetric(TQStyle::PM_SplitterWidth, this) / 3 );
    }
    return sh;
}

/*! \reimp
 */

TQSize TQDockWindow::minimumSize() const
{
    TQSize ms( TQFrame::minimumSize() );
    if ( place() == InDock )
	ms = ms.expandedTo( fixedExtent() );
    ms = ms.expandedTo( TQSize( 16, 16 ) );
    if ( area() ) {
	if ( area()->orientation() == Horizontal && !vHandleRight->isVisible() )
	    ms.setWidth( ms.width() + 2 * style().pixelMetric(TQStyle::PM_SplitterWidth, this) / 3 );
	else if ( area()->orientation() == Vertical && !hHandleBottom->isVisible() )
	    ms.setHeight( ms.height() + 2 * style().pixelMetric(TQStyle::PM_SplitterWidth, this) / 3 );
    }
    return ms;
}

/*! \reimp
 */

TQSize TQDockWindow::minimumSizeHint() const
{
    TQSize msh( TQFrame::minimumSize() );
    if ( place() == InDock )
	msh = msh.expandedTo( fixedExtent() );
    msh = msh.expandedTo( TQSize( 16, 16 ) );
    if ( area() ) {
	if ( area()->orientation() == Horizontal && !vHandleRight->isVisible() )
	    msh.setWidth( msh.width() + 2 * style().pixelMetric(TQStyle::PM_SplitterWidth, this) / 3 );
	else if ( area()->orientation() == Vertical && !hHandleBottom->isVisible() )
	    msh.setHeight( msh.height() + 2 * style().pixelMetric(TQStyle::PM_SplitterWidth, this) / 3 );
    }
    return msh;
}

/*! \internal */
void TQDockWindow::undock( TQWidget *w )
{
    TQMainWindow *mw = 0;
    if ( area() )
	mw = ::tqt_cast<TQMainWindow*>(area()->parentWidget());
    if ( mw && !mw->isDockEnabled( this, DockTornOff ) )
	return;
    if ( (place() == OutsideDock && !w) )
	return;

    TQPoint p( 50, 50 );
    if ( topLevelWidget() )
	p = topLevelWidget()->pos() + TQPoint( 20, 20 );
    if ( dockArea ) {
	delete (TQDockArea::DockWindowData*)dockWindowData;
	dockWindowData = dockArea->dockWindowData( this );
	dockArea->removeDockWindow( this, true, orientation() != Horizontal && ::tqt_cast<TQToolBar*>(this) );
    }
    dockArea = 0;
    if ( lastPos != TQPoint( -1, -1 ) && lastPos.x() > 0 && lastPos.y() > 0 )
	move( lastPos );
    else
	move( p );
    if ( lastSize != TQSize( -1, -1 ) )
	resize( lastSize );
    curPlace = OutsideDock;
    updateGui();
    emit orientationChanged( orientation() );
    TQApplication::sendPostedEvents( this, TQEvent::LayoutHint );
    if ( ::tqt_cast<TQToolBar*>(this) )
	adjustSize();
    if ( !w ) {
	if ( !parentWidget() || parentWidget()->isVisible() ) {
            if (lastSize == TQSize(-1, -1))
                clearWState(WState_Resized); // Ensures size is recalculated (opaque).
	    show();
	}
    } else {
	reparent( w, 0, TQPoint( 0, 0 ), false );
	move( -width() - 5, -height() - 5 );
	resize( 1, 1 );
	show();
    }
    if ( parentWidget() && isTopLevel() )
	parentWidget()->setActiveWindow();
    emit placeChanged( place() );
}

/*!
    \fn void TQDockWindow::undock()

    Undocks the TQDockWindow from its current dock area if it is
    docked; otherwise does nothing.

    \sa dock() TQDockArea::moveDockWindow(),
    TQDockArea::removeDockWindow(), TQMainWindow::moveDockWindow(),
    TQMainWindow::removeDockWindow()
*/

void TQDockWindow::removeFromDock( bool fixNewLines )
{
    if ( dockArea )
	dockArea->removeDockWindow( this, false, false, fixNewLines );
}

/*!
    Docks the dock window into the last dock area in which it was
    docked.

    If the dock window has no last dock area (e.g. it was created as a
    floating window and has never been docked), or if the last dock
    area it was docked in does not exist (e.g. the dock area has been
    deleted), nothing happens.

    The dock window will dock with the dock area regardless of the return value
    of TQDockArea::isDockWindowAccepted().

    \sa undock() TQDockArea::moveDockWindow(),
    TQDockArea::removeDockWindow(), TQMainWindow::moveDockWindow(),
    TQMainWindow::removeDockWindow(), TQDockArea::isDockWindowAccepted()

*/

void TQDockWindow::dock()
{
    if ( !(TQDockArea::DockWindowData*)dockWindowData ||
	 !( (TQDockArea::DockWindowData*)dockWindowData )->area )
	return;
    curPlace = InDock;
    lastPos = pos();
    lastSize = size();
    ( (TQDockArea::DockWindowData*)dockWindowData )->
	area->dockWindow( this, (TQDockArea::DockWindowData*)dockWindowData );
    emit orientationChanged( orientation() );
    emit placeChanged( place() );
}

/*! \reimp
 */

void TQDockWindow::hideEvent( TQHideEvent *e )
{
    TQFrame::hideEvent( e );
}

/*! \reimp
 */

void TQDockWindow::showEvent( TQShowEvent *e )
{
    if (curPlace == OutsideDock && (parent() && strcmp(parent()->name(), "qt_hide_dock") != 0)) {
	TQRect sr = tqApp->desktop()->availableGeometry( this );
	if ( !sr.contains( pos() ) ) {
	    int nx = TQMIN( TQMAX( x(), sr.x() ), sr.right()-width() );
	    int ny = TQMIN( TQMAX( y(), sr.y() ), sr.bottom()-height() );
	    move( nx, ny );
	}
    }

    TQFrame::showEvent( e );
}

/*!
    \property TQDockWindow::opaqueMoving
    \brief whether the dock window will be shown normally whilst it is
    being moved.

    If this property is false, (the default), the dock window will be
    represented by an outline rectangle whilst it is being moved.

    \warning Currently opaque moving has some problems and we do not
    recommend using it at this time. We expect to fix these problems
    in a future release.
*/

void TQDockWindow::setOpaqueMoving( bool b )
{
    opaque = b;
    horHandle->setOpaqueMoving( b );
    verHandle->setOpaqueMoving( b );
    titleBar->setOpaqueMoving( b );
}

bool TQDockWindow::opaqueMoving() const
{
    return opaque;
}

/*! \reimp */

void TQDockWindow::setCaption( const TQString &s )
{
    titleBar->setCaption( s );
    verHandle->update();
    horHandle->update();
#ifndef TQT_NO_WIDGET_TOPEXTRA
    TQFrame::setCaption( s );
#endif
#ifndef TQT_NO_TOOLTIP
    TQToolTip::remove( horHandle );
    TQToolTip::remove( verHandle );
    if ( !s.isEmpty() ) {
	TQToolTip::add( horHandle, s );
	TQToolTip::add( verHandle, s );
    }
#endif
}

void TQDockWindow::updateSplitterVisibility( bool visible )
{
    if ( area() && isResizeEnabled() ) {
	if ( orientation() == Horizontal ) {
	    if ( visible )
		vHandleRight->show();
	    else
		vHandleRight->hide();
	    vHandleLeft->hide();
	} else {
	    if ( visible )
		hHandleBottom->show();
	    else
		hHandleBottom->hide();
	    hHandleTop->hide();
	}
    }
}

/*! \reimp */
bool TQDockWindow::eventFilter( TQObject * o, TQEvent *e )
{
    if ( !o->isWidgetType() )
	return false;

    if ( e->type() == TQEvent::KeyPress &&
	( horHandle->mousePressed ||
	  verHandle->mousePressed ||
	  titleBar->mousePressed ) ) {
	TQKeyEvent *ke = (TQKeyEvent*)e;
	if ( ke->key() == Key_Escape ) {
	    horHandle->mousePressed =
		verHandle->mousePressed =
		    titleBar->mousePressed = false;
	    endRectDraw( !opaque );
	    tqApp->removeEventFilter( this );
	    return true;
	}
    } else if ( ((TQWidget*)o)->topLevelWidget() != this && place() == OutsideDock && isTopLevel() ) {
	if ( (e->type() == TQEvent::WindowDeactivate ||
	    e->type() == TQEvent::WindowActivate ) )
	    event( e );
    }
    return false;
}

/*! \reimp */
bool TQDockWindow::event( TQEvent *e )
{
    switch ( e->type() ) {
    case TQEvent::WindowDeactivate:
	if ( place() == OutsideDock && isTopLevel() && parentWidget()
	     && parentWidget()->isActiveWindow() )
	    return true;
    case TQEvent::Hide:
	if ( !isHidden() )
	    break;
	// fall through
    case TQEvent::HideToParent:
	emit visibilityChanged( false );
	break;
    case TQEvent::Show:
	if ( e->spontaneous() )
	    break;
    case TQEvent::ShowToParent:
	emit visibilityChanged( true );
	break;
    default:
	break;
    }
    return TQFrame::event( e );
}

#ifdef TQT_NO_WIDGET_TOPEXTRA
TQString TQDockWindow::caption() const
{
    return titleBar->caption();
}
#endif

/*! \reimp */
void TQDockWindow::contextMenuEvent( TQContextMenuEvent *e )
{
    TQObject *o = this;
    while ( o ) {
	if ( ::tqt_cast<TQMainWindow*>(o) )
	    break;
	o = o->parent();
    }
    if ( !o || ! ( (TQMainWindow*)o )->showDockMenu( e->globalPos() ) )
	e->ignore();
}

#include "qdockwindow.moc"

#endif //TQT_NO_MAINWINDOW
