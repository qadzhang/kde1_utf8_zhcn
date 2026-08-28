/****************************************************************************
**
** Implementation of the TQWidgetResizeHandler class
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

#include "qwidgetresizehandler_p.h"

#ifndef TQT_NO_RESIZEHANDLER
#include "ntqframe.h"
#include "ntqapplication.h"
#include "ntqcursor.h"
#include "ntqsizegrip.h"
#if defined(TQ_WS_WIN)
#include "qt_windows.h"
#endif

#define RANGE 4

static bool resizeHorizontalDirectionFixed = false;
static bool resizeVerticalDirectionFixed = false;

TQWidgetResizeHandler::TQWidgetResizeHandler( TQWidget *parent, TQWidget *cw, const char *name )
    : TQObject( parent, name ), widget( parent ), childWidget( cw ? cw : parent ),
      extrahei( 0 ), buttonDown( false ), moveResizeMode( false ), sizeprotect( true ), moving( true )
{
    mode = Nowhere;
    widget->setMouseTracking( true );
    TQFrame *frame = ::tqt_cast<TQFrame*>(widget);
    range = frame ? frame->frameWidth() : RANGE;
    range = TQMAX( RANGE, range );
    activeForMove = activeForResize = true;
    tqApp->installEventFilter( this );
}

void TQWidgetResizeHandler::setActive( Action ac, bool b )
{
    if ( ac & Move )
	activeForMove = b;
    if ( ac & Resize )
	activeForResize = b; 

    if ( !isActive() )
	setMouseCursor( Nowhere );
}

bool TQWidgetResizeHandler::isActive( Action ac ) const
{
    bool b = false;
    if ( ac & Move ) b = activeForMove;
    if ( ac & Resize ) b |= activeForResize;

    return b;
}

static TQWidget *childOf( TQWidget *w, TQWidget *child )
{
    while ( child ) {
	if ( child == w )
	    return child;
	if ( child->isTopLevel() )
	    break;
	child = child->parentWidget();
    }
    return 0;
}

bool TQWidgetResizeHandler::eventFilter( TQObject *o, TQEvent *ee )
{
    if ( !isActive() || !o->isWidgetType() || !ee->spontaneous())
	return false;

    if ( ee->type() != TQEvent::MouseButtonPress &&
	 ee->type() != TQEvent::MouseButtonRelease &&
	 ee->type() != TQEvent::MouseMove &&
	 ee->type() != TQEvent::KeyPress &&
	 ee->type() != TQEvent::AccelOverride )
	return false;

    TQWidget *w = childOf( widget, (TQWidget*)o );
    if ( !w
#ifndef TQT_NO_SIZEGRIP
	 || ::tqt_cast<TQSizeGrip*>(o)
#endif
	 || tqApp->activePopupWidget() ) {
	if ( buttonDown && ee->type() == TQEvent::MouseButtonRelease )
	    buttonDown = false;
	return false;
    }

    TQMouseEvent *e = (TQMouseEvent*)ee;
    switch ( e->type() ) {
    case TQEvent::MouseButtonPress: {
	if ( w->isMaximized() )
	    break;
	if ( !widget->rect().contains( widget->mapFromGlobal( e->globalPos() ) ) )
	    return false;
	if ( e->button() == LeftButton ) {
	    emit activate();
	    bool me = isMovingEnabled();
	    setMovingEnabled( me && o == widget );
	    mouseMoveEvent( e );
	    setMovingEnabled( me );
	    buttonDown = true;
	    moveOffset = widget->mapFromGlobal( e->globalPos() );
	    invertedMoveOffset = widget->rect().bottomRight() - moveOffset;
	}
    } break;
    case TQEvent::MouseButtonRelease:
	if ( w->isMaximized() )
	    break;
	if ( e->button() == LeftButton ) {
	    moveResizeMode = false;
	    buttonDown = false;
	    widget->releaseMouse();
	    widget->releaseKeyboard();
	}
	break;
    case TQEvent::MouseMove: {
	if ( w->isMaximized() )
	    break;
	bool me = isMovingEnabled();
	setMovingEnabled( me && o == widget );
	mouseMoveEvent( e );
	setMovingEnabled( me );
	if ( buttonDown && mode != Center )
	    return true;
    } break;
    case TQEvent::KeyPress:
	keyPressEvent( (TQKeyEvent*)e );
	break;
    case TQEvent::AccelOverride:
	if ( buttonDown ) {
	    ((TQKeyEvent*)ee)->accept();
	    return true;
	}
	break;
    default:
	break;
    }
    return false;
}

void TQWidgetResizeHandler::mouseMoveEvent( TQMouseEvent *e )
{
    TQPoint pos = widget->mapFromGlobal( e->globalPos() );
    if ( !moveResizeMode && ( !buttonDown || ( e->state() & LeftButton ) == 0 ) ) {
	if ( pos.y() <= range && pos.x() <= range)
	    mode = TopLeft;
	else if ( pos.y() >= widget->height()-range && pos.x() >= widget->width()-range)
	    mode = BottomRight;
	else if ( pos.y() >= widget->height()-range && pos.x() <= range)
	    mode = BottomLeft;
	else if ( pos.y() <= range && pos.x() >= widget->width()-range)
	    mode = TopRight;
	else if ( pos.y() <= range )
	    mode = Top;
	else if ( pos.y() >= widget->height()-range )
	    mode = Bottom;
	else if ( pos.x() <= range )
	    mode = Left;
	else if (  pos.x() >= widget->width()-range )
	    mode = Right;
	else
	    mode = Center;

	if ( widget->isMinimized() || !isActive(Resize) )
	    mode = Center;
#ifndef TQT_NO_CURSOR
	setMouseCursor( mode );
#endif
	return;
    }

    if ( buttonDown && !isMovingEnabled() && mode == Center && !moveResizeMode )
	return;

    if ( widget->testWState( WState_ConfigPending ) )
 	return;

    TQPoint globalPos = widget->parentWidget( true ) ?
		       widget->parentWidget( true )->mapFromGlobal( e->globalPos() ) : e->globalPos();
    if ( widget->parentWidget( true ) && !widget->parentWidget( true )->rect().contains( globalPos ) ) {
	if ( globalPos.x() < 0 )
	    globalPos.rx() = 0;
	if ( globalPos.y() < 0 )
	    globalPos.ry() = 0;
	if ( sizeprotect && globalPos.x() > widget->parentWidget()->width() )
	    globalPos.rx() = widget->parentWidget()->width();
	if ( sizeprotect && globalPos.y() > widget->parentWidget()->height() )
	    globalPos.ry() = widget->parentWidget()->height();
    }

    TQPoint p = globalPos + invertedMoveOffset;
    TQPoint pp = globalPos - moveOffset;

    int fw = 0;
    int mw = TQMAX( childWidget->minimumSizeHint().width(),
		   childWidget->minimumWidth() );
    int mh = TQMAX( childWidget->minimumSizeHint().height(),
		   childWidget->minimumHeight() );
    if ( childWidget != widget ) {
	TQFrame *frame = ::tqt_cast<TQFrame*>(widget);
	if ( frame )
	    fw = frame->frameWidth();
	mw += 2 * fw;
	mh += 2 * fw + extrahei;
    }

    TQSize mpsize( widget->geometry().right() - pp.x() + 1,
		  widget->geometry().bottom() - pp.y() + 1 );
    mpsize = mpsize.expandedTo( widget->minimumSize() ).expandedTo( TQSize(mw, mh) );
    TQPoint mp( widget->geometry().right() - mpsize.width() + 1,
	       widget->geometry().bottom() - mpsize.height() + 1 );

    TQRect geom = widget->geometry();

    switch ( mode ) {
    case TopLeft:
	geom = TQRect( mp, widget->geometry().bottomRight() ) ;
	break;
    case BottomRight:
	geom = TQRect( widget->geometry().topLeft(), p ) ;
	break;
    case BottomLeft:
	geom = TQRect( TQPoint(mp.x(), widget->geometry().y() ), TQPoint( widget->geometry().right(), p.y()) ) ;
	break;
    case TopRight:
	geom = TQRect( TQPoint( widget->geometry().x(), mp.y() ), TQPoint( p.x(), widget->geometry().bottom()) ) ;
	break;
    case Top:
	geom = TQRect( TQPoint( widget->geometry().left(), mp.y() ), widget->geometry().bottomRight() ) ;
	break;
    case Bottom:
	geom = TQRect( widget->geometry().topLeft(), TQPoint( widget->geometry().right(), p.y() ) ) ;
	break;
    case Left:
	geom = TQRect( TQPoint( mp.x(), widget->geometry().top() ), widget->geometry().bottomRight() ) ;
	break;
    case Right:
	geom = TQRect( widget->geometry().topLeft(), TQPoint( p.x(), widget->geometry().bottom() ) ) ;
	break;
    case Center:
	if ( isMovingEnabled() || moveResizeMode )
	    geom.moveTopLeft( pp );
	break;
    default:
	break;
    }

    TQSize maxsize( childWidget->maximumSize() );
    if ( childWidget != widget )
	maxsize += TQSize( 2 * fw, 2 * fw + extrahei );

    geom = TQRect( geom.topLeft(),
		  geom.size().expandedTo( widget->minimumSize() )
			     .expandedTo( TQSize(mw, mh) )
			     .boundedTo( maxsize ) );

    if ( geom != widget->geometry() &&
	( widget->isTopLevel() || widget->parentWidget()->rect().intersects( geom ) ) ) {
	if ( widget->isMinimized() )
	    widget->move( geom.topLeft() );
	else
	    widget->setGeometry( geom );
    }

#if defined(TQ_WS_WIN)
    MSG msg;
    QT_WA( {
	while(PeekMessageW( &msg, widget->winId(), WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE ))
	    ;
    } , {
	while(PeekMessageA( &msg, widget->winId(), WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE ))
	    ;
    } );
#endif

    TQApplication::syncX();
}

void TQWidgetResizeHandler::setMouseCursor( MousePosition m )
{
#ifndef TQT_NO_CURSOR
    switch ( m ) {
    case TopLeft:
    case BottomRight:
	widget->setCursor( sizeFDiagCursor );
	break;
    case BottomLeft:
    case TopRight:
	widget->setCursor( sizeBDiagCursor );
	break;
    case Top:
    case Bottom:
	widget->setCursor( sizeVerCursor );
	break;
    case Left:
    case Right:
	widget->setCursor( sizeHorCursor );
	break;
    default:
	widget->setCursor( arrowCursor );
	break;
    }
#endif
}

void TQWidgetResizeHandler::keyPressEvent( TQKeyEvent * e )
{
    if ( !isMove() && !isResize() )
	return;
    bool is_control = e->state() & ControlButton;
    int delta = is_control?1:8;
    TQPoint pos = TQCursor::pos();
    switch ( e->key() ) {
    case Key_Left:
	pos.rx() -= delta;
	if ( pos.x() <= TQApplication::desktop()->geometry().left() ) {
	    if ( mode == TopLeft || mode == BottomLeft ) {
		moveOffset.rx() += delta;
		invertedMoveOffset.rx() += delta;
	    } else {
		moveOffset.rx() -= delta;
		invertedMoveOffset.rx() -= delta;
	    }
	}
	if ( isResize() && !resizeHorizontalDirectionFixed ) {
	    resizeHorizontalDirectionFixed = true;
	    if ( mode == BottomRight )
		mode = BottomLeft;
	    else if ( mode == TopRight )
		mode = TopLeft;
#ifndef TQT_NO_CURSOR
	    setMouseCursor( mode );
	    widget->grabMouse( widget->cursor() );
#else
	    widget->grabMouse();
#endif
	}
	break;
    case Key_Right:
	pos.rx() += delta;
	if ( pos.x() >= TQApplication::desktop()->geometry().right() ) {
	    if ( mode == TopRight || mode == BottomRight ) {
		moveOffset.rx() += delta;
		invertedMoveOffset.rx() += delta;
	    } else {
		moveOffset.rx() -= delta;
		invertedMoveOffset.rx() -= delta;
	    }
	}
	if ( isResize() && !resizeHorizontalDirectionFixed ) {
	    resizeHorizontalDirectionFixed = true;
	    if ( mode == BottomLeft )
		mode = BottomRight;
	    else if ( mode == TopLeft )
		mode = TopRight;
#ifndef TQT_NO_CURSOR
	    setMouseCursor( mode );
	    widget->grabMouse( widget->cursor() );
#else
	    widget->grabMouse();
#endif
	}
	break;
    case Key_Up:
	pos.ry() -= delta;
	if ( pos.y() <= TQApplication::desktop()->geometry().top() ) {
	    if ( mode == TopLeft || mode == TopRight ) {
		moveOffset.ry() += delta;
		invertedMoveOffset.ry() += delta;
	    } else {
		moveOffset.ry() -= delta;
		invertedMoveOffset.ry() -= delta;
	    }
	}
	if ( isResize() && !resizeVerticalDirectionFixed ) {
	    resizeVerticalDirectionFixed = true;
	    if ( mode == BottomLeft )
		mode = TopLeft;
	    else if ( mode == BottomRight )
		mode = TopRight;
#ifndef TQT_NO_CURSOR
	    setMouseCursor( mode );
	    widget->grabMouse( widget->cursor() );
#else
	    widget->grabMouse();
#endif
	}
	break;
    case Key_Down:
	pos.ry() += delta;
	if ( pos.y() >= TQApplication::desktop()->geometry().bottom() ) {
	    if ( mode == BottomLeft || mode == BottomRight ) {
		moveOffset.ry() += delta;
		invertedMoveOffset.ry() += delta;
	    } else {
		moveOffset.ry() -= delta;
		invertedMoveOffset.ry() -= delta;
	    }
	}
	if ( isResize() && !resizeVerticalDirectionFixed ) {
	    resizeVerticalDirectionFixed = true;
	    if ( mode == TopLeft )
		mode = BottomLeft;
	    else if ( mode == TopRight )
		mode = BottomRight;
#ifndef TQT_NO_CURSOR
	    setMouseCursor( mode );
	    widget->grabMouse( widget->cursor() );
#else
	    widget->grabMouse();
#endif
	}
	break;
    case Key_Space:
    case Key_Return:
    case Key_Enter:
    case Key_Escape:
	moveResizeMode = false;
	widget->releaseMouse();
	widget->releaseKeyboard();
	buttonDown = false;
	break;
    default:
	return;
    }
    TQCursor::setPos( pos );
}


void TQWidgetResizeHandler::doResize()
{
    if ( !activeForResize )
	return;

    moveResizeMode = true;
    buttonDown = true;
    moveOffset = widget->mapFromGlobal( TQCursor::pos() );
    if ( moveOffset.x() < widget->width()/2) {
	if ( moveOffset.y() < widget->height()/2)
	    mode = TopLeft;
	else
	    mode = BottomLeft;
    } else {
	if ( moveOffset.y() < widget->height()/2)
	    mode = TopRight;
	else
	    mode = BottomRight;
    }
    invertedMoveOffset = widget->rect().bottomRight() - moveOffset;
#ifndef TQT_NO_CURSOR
    setMouseCursor( mode );
    widget->grabMouse( widget->cursor()  );
#else
    widget->grabMouse();
#endif
    widget->grabKeyboard();
    resizeHorizontalDirectionFixed = false;
    resizeVerticalDirectionFixed = false;
}

void TQWidgetResizeHandler::doMove()
{
    if ( !activeForMove )
	return;

    mode = Center;
    moveResizeMode = true;
    buttonDown = true;
    moveOffset = widget->mapFromGlobal( TQCursor::pos() );
    invertedMoveOffset = widget->rect().bottomRight() - moveOffset;
#ifndef TQT_NO_CURSOR
    widget->grabMouse( SizeAllCursor );
#else
    widget->grabMouse();
#endif
    widget->grabKeyboard();
}

#endif //TQT_NO_RESIZEHANDLER
