/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "listdnd.h"
#include <ntqwidget.h>
#include <ntqpainter.h>

ListDnd::ListDnd( TQScrollView * eventSource, const char * name )
    : TQObject( eventSource, name ),
      dragInside( false ), dragDelete( true ), dropConfirmed( false ), dMode( Both )
{
    src = eventSource;
    src->setAcceptDrops( true );
    src->installEventFilter( this );
    src->viewport()->installEventFilter( this );

    line = new TQWidget( src->viewport(), 0, TQt::WStyle_NoBorder | WStyle_StaysOnTop );
    line->setBackgroundColor( TQt::black );
    line->resize( src->viewport()->width(), 2 );
    line->hide();
}

void ListDnd::setDragMode( int mode )
{
    dMode = mode;
}

int ListDnd::dragMode() const
{
    return dMode;
}

bool ListDnd::eventFilter( TQObject *, TQEvent * event )
{
    switch ( event->type() ) {
    case TQEvent::DragEnter:
	return dragEnterEvent( (TQDragEnterEvent *) event );
    case TQEvent::DragLeave:
	return dragLeaveEvent( (TQDragLeaveEvent *) event );
    case TQEvent::DragMove:
	return dragMoveEvent( (TQDragMoveEvent *) event );
    case TQEvent::Drop:
	return dropEvent( (TQDropEvent *) event );
    case TQEvent::MouseButtonPress:
	return mousePressEvent( (TQMouseEvent *) event );
    case TQEvent::MouseMove:
	return mouseMoveEvent( (TQMouseEvent *) event );
    default:
	break;
    }
    return false;
}


bool ListDnd::dragEnterEvent( TQDragEnterEvent * event )
{
    if ( dMode == None ) {
	return true;
    }

    bool ok = ( ( ( dMode & Both ) == Both ) ||
		( ( dMode & Internal ) && ( event->source() == src ) ) ||
		( ( dMode & External ) && ( event->source() != src ) ) );

    if ( ok && canDecode( event ) ) {
	event->accept();
	dragInside = true;
	if ( !( dMode & NullDrop ) ) {
	    updateLine( event->pos() );
	    line->show();
	}
    }
    return true;
}

bool ListDnd::dragLeaveEvent( TQDragLeaveEvent * )
{
    dragInside = false;
    line->hide();
    return true;
}

bool ListDnd::dragMoveEvent( TQDragMoveEvent * event )
{
    if ( dragInside && dMode && !( dMode & NullDrop ) ) {
	updateLine( event->pos() );
    }
    return true;
}

bool ListDnd::dropEvent( TQDropEvent * )
{
    // do nothing
    return false;
}

bool ListDnd::mousePressEvent( TQMouseEvent * event )
{
    if ( event->button() == LeftButton )
	mousePressPos = event->pos();
    return false;
}

bool ListDnd::mouseMoveEvent( TQMouseEvent * )
{
    // do nothing
    return false;
}

void ListDnd::updateLine( const TQPoint & )
{
    // do nothing
}

bool ListDnd::canDecode( TQDragEnterEvent * )
{
    // do nothing
    return false;
}
