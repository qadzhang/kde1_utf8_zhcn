/****************************************************************************
**
** Implementation of TQSizeGrip class
**
** Created : 980119
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

#include "ntqsizegrip.h"

#ifndef TQT_NO_SIZEGRIP

#include "ntqpainter.h"
#include "ntqapplication.h"
#include "ntqstyle.h"

#if defined(TQ_WS_X11)
#include "qt_x11_p.h"
extern Atom tqt_sizegrip;			// defined in qapplication_x11.cpp
#elif defined (TQ_WS_WIN )
#include "ntqobjectlist.h"
#include "qt_windows.h"
#elif defined(TQ_WS_MAC)
bool tqt_mac_update_sizer(TQWidget *, int); //qwidget_mac.cpp
#endif


static TQWidget *tqt_sizegrip_topLevelWidget( TQWidget* w)
{
    TQWidget *p = w->parentWidget();
    while ( !w->isTopLevel() && p && !p->inherits("TQWorkspace") ) {
	w = p;
	p = p->parentWidget();
    }
    return w;
}

static TQWidget *tqt_sizegrip_workspace( TQWidget* w )
{
    while ( w && !w->inherits("TQWorkspace" ) ) {
	if ( w->isTopLevel() )
	    return 0;
	w = w->parentWidget();
    }
    return w;
}


/*!
    \class TQSizeGrip ntqsizegrip.h

    \brief The TQSizeGrip class provides a corner-grip for resizing a top-level window.

    \ingroup application
    \ingroup basic
    \ingroup appearance

    This widget works like the standard Windows resize handle. In the
    X11 version this resize handle generally works differently from
    the one provided by the system; we hope to reduce this difference
    in the future.

    Put this widget anywhere in a widget tree and the user can use it
    to resize the top-level window. Generally, this should be in the
    lower right-hand corner. Note that TQStatusBar already uses this
    widget, so if you have a status bar (e.g. you are using
    TQMainWindow), then you don't need to use this widget explicitly.

    <img src=qsizegrip-m.png> <img src=qsizegrip-w.png>

    \sa TQStatusBar
*/


/*!
    Constructs a resize corner called \a name, as a child widget of \a
    parent.
*/
TQSizeGrip::TQSizeGrip( TQWidget * parent, const char* name )
    : TQWidget( parent, name )
{
#ifndef TQT_NO_CURSOR
#ifndef TQ_WS_MAC
    if ( TQApplication::reverseLayout() )
	setCursor( sizeBDiagCursor );
    else
	setCursor( sizeFDiagCursor );
#endif
#endif
    setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Fixed ) );
#if defined(TQ_WS_X11)
    if ( !tqt_sizegrip_workspace( this ) ) {
	WId id = winId();
	XChangeProperty(tqt_xdisplay(), topLevelWidget()->winId(),
			tqt_sizegrip, XA_WINDOW, 32, PropModeReplace,
			(unsigned char *)&id, 1);
    }
#endif
    tlw = tqt_sizegrip_topLevelWidget( this );
    if ( tlw )
	tlw->installEventFilter( this );
    installEventFilter( this ); //for binary compatibility fix in 4.0 with an event() ###
}


/*!
    Destroys the size grip.
*/
TQSizeGrip::~TQSizeGrip()
{
#if defined(TQ_WS_X11)
    if ( !TQApplication::closingDown() && parentWidget() ) {
	WId id = None;
 	XChangeProperty(tqt_xdisplay(), topLevelWidget()->winId(),
 			tqt_sizegrip, XA_WINDOW, 32, PropModeReplace,
 			(unsigned char *)&id, 1);
    }
#endif
}

/*!
    Returns the size grip's size hint; this is a small size.
*/
TQSize TQSizeGrip::sizeHint() const
{
    return (style().sizeFromContents(TQStyle::CT_SizeGrip, this, TQSize(13, 13)).
	    expandedTo(TQApplication::globalStrut()));
}

/*!
    Paints the resize grip. Resize grips are usually rendered as small
    diagonal textured lines in the lower-right corner. The event is in
    \a e.
*/
void TQSizeGrip::paintEvent( TQPaintEvent *e )
{
    TQPainter painter( this );
    painter.setClipRegion(e->region());
    style().drawPrimitive(TQStyle::PE_SizeGrip, &painter, rect(), colorGroup());
}

/*!
    Primes the resize operation. The event is in \a e.
*/
void TQSizeGrip::mousePressEvent( TQMouseEvent * e )
{
    p = e->globalPos();
    s = tqt_sizegrip_topLevelWidget(this)->size();
}


/*!
    Resizes the top-level widget containing this widget. The event is
    in \a e.
*/
void TQSizeGrip::mouseMoveEvent( TQMouseEvent * e )
{
    if ( e->state() != LeftButton )
	return;

    TQWidget* tlw = tqt_sizegrip_topLevelWidget(this);
    if ( tlw->testWState(WState_ConfigPending) )
	return;

    TQPoint np( e->globalPos() );

    TQWidget* ws = tqt_sizegrip_workspace( this );
    if ( ws ) {
	TQPoint tmp( ws->mapFromGlobal( np ) );
	if ( tmp.x() > ws->width() )
	    tmp.setX( ws->width() );
	if ( tmp.y() > ws->height() )
	    tmp.setY( ws->height() );
	np = ws->mapToGlobal( tmp );
    }

    int w;
    int h = np.y() - p.y() + s.height();

    if ( TQApplication::reverseLayout() )
	w = s.width() - ( np.x() - p.x() );
    else
	w = np.x() - p.x() + s.width();

    if ( w < 1 )
	w = 1;
    if ( h < 1 )
	h = 1;
    TQSize ms( tlw->minimumSizeHint() );
    ms = ms.expandedTo( minimumSize() );
    if ( w < ms.width() )
	w = ms.width();
    if ( h < ms.height() )
	h = ms.height();

    if (TQApplication::reverseLayout()) {
	tlw->resize( w, h );
	if (tlw->size() == TQSize(w,h))
	    tlw->move( tlw->x() + ( np.x()-p.x() ), tlw->y() );
    } else {
	tlw->resize( w, h );
    }
#ifdef TQ_WS_WIN
    MSG msg;
    while( PeekMessage( &msg, winId(), WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE ) )
      ;
#endif
    TQApplication::syncX();

    if ( TQApplication::reverseLayout() && tlw->size() == TQSize(w,h) ) {
	s.rwidth() = tlw->size().width();
	p.rx() = np.x();
    }
}

/*! \reimp */
bool TQSizeGrip::eventFilter( TQObject *o, TQEvent *e )
{
    if ( o == tlw ) {
	switch ( e->type() ) {
#ifndef TQ_WS_MAC
	/* The size grip goes no where on Mac OS X when you maximize!  --Sam */
	case TQEvent::ShowMaximized:
#endif
	case TQEvent::ShowFullScreen:
	    hide();
	    break;
	case TQEvent::ShowNormal:
	    show();
	    break;
	default:
	    break;
	}
    } else if(o == this) {
#if defined(TQ_WS_MAC)
	switch(e->type()) {
	case TQEvent::Hide:
	case TQEvent::Show:
	    if(!TQApplication::closingDown() && parentWidget() && !tqt_sizegrip_workspace(this)) {
		if(TQWidget *w = tqt_sizegrip_topLevelWidget(this)) {
		    if(w->isTopLevel()) 
			tqt_mac_update_sizer(w, e->type() == TQEvent::Hide ? -1 : 1);
		}
	    }
	    break;
	default:
	    break;
	}
 #endif	    
    }
    return false;
}

#endif //TQT_NO_SIZEGRIP
