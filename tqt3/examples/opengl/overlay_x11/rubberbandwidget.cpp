/****************************************************************************
**
** Implementation of a widget that draws a rubberband. Designed to be used 
** in an X11 overlay visual
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "rubberbandwidget.h"
#include <ntqpainter.h>


RubberbandWidget::RubberbandWidget( TQColor transparentColor, TQWidget *parent, 
				    const char *name, WFlags f )
    : TQWidget( parent, name, f )
{
    setBackgroundColor( transparentColor );
    on = false;
}


void RubberbandWidget::mousePressEvent( TQMouseEvent* e )
{
    p1 = e->pos();
    p2 = p1;
    p3 = p1;
    on = true;
    setMouseTracking( true );
}


void RubberbandWidget::mouseMoveEvent( TQMouseEvent* e )
{
    if ( on ) {
	p2 = e->pos();
	TQPainter p( this );
	// Erase last drawn rubberband:
	p.setPen( TQPen( backgroundColor(), 3 ) );
	p.drawRect( TQRect( p1, p3 ) );
	// Draw the new one:
	p.setPen( TQPen( white, 3 ) );
	p.drawRect( TQRect(p1, p2) );
	p3 = p2;
    }
}

void RubberbandWidget::mouseReleaseEvent( TQMouseEvent* )
{
    if ( on ) {
	TQPainter p ( this );
	p.eraseRect( rect() );
    }
    on = false;
    setMouseTracking( false );
}
