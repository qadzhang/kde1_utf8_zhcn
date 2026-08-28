/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "canvas.h"

#include <ntqapplication.h>
#include <ntqpainter.h>
#include <ntqevent.h>
#include <ntqrect.h>

const bool no_writing = false;

Canvas::Canvas( TQWidget *parent, const char *name, WFlags fl )
    : TQWidget( parent, name, WStaticContents | fl ),
      pen( TQt::red, 3 ), polyline(3),
      mousePressed( false ), oldPressure( 0 ), saveColor( red ),
      buffer( width(), height() )
{

    if ((tqApp->argc() > 0) && !buffer.load(tqApp->argv()[1]))
	buffer.fill( colorGroup().base() );
    setBackgroundMode( TQWidget::PaletteBase );
#ifndef TQT_NO_CURSOR
    setCursor( TQt::crossCursor );
#endif
}

void Canvas::save( const TQString &filename, const TQString &format )
{
    if ( !no_writing )
	buffer.save( filename, format.upper() );
}

void Canvas::clearScreen()
{
    buffer.fill( colorGroup().base() );
    repaint( false );
}

void Canvas::mousePressEvent( TQMouseEvent *e )
{
    mousePressed = true;
    polyline[2] = polyline[1] = polyline[0] = e->pos();
}

void Canvas::mouseReleaseEvent( TQMouseEvent * )
{
    mousePressed = false;
}

void Canvas::mouseMoveEvent( TQMouseEvent *e )
{
    if ( mousePressed ) {
	TQPainter painter;
	painter.begin( &buffer );
	painter.setPen( pen );
	polyline[2] = polyline[1];
	polyline[1] = polyline[0];
	polyline[0] = e->pos();
	painter.drawPolyline( polyline );
	painter.end();

	TQRect r = polyline.boundingRect();
	r = r.normalize();
	r.setLeft( r.left() - penWidth() );
	r.setTop( r.top() - penWidth() );
	r.setRight( r.right() + penWidth() );
	r.setBottom( r.bottom() + penWidth() );

	bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );
    }
}

void Canvas::tabletEvent( TQTabletEvent *e )
{
    e->accept();
    // change the width based on range of pressure
    if ( e->device() == TQTabletEvent::Stylus )	{
	if ( e->pressure() >= 0 && e->pressure() <= 32 )
	    pen.setColor( saveColor.light(175) );
	else if ( e->pressure() > 32 && e->pressure() <= 64 )
	    pen.setColor( saveColor.light(150) );
	else if ( e->pressure() > 64  && e->pressure() <= 96 )
	    pen.setColor( saveColor.light(125) );
	else if ( e->pressure() > 96 && e->pressure() <= 128 )
	    pen.setColor( saveColor );
	else if ( e->pressure() > 128 && e->pressure() <= 160 )
	    pen.setColor( saveColor.dark(150) );
	else if ( e->pressure() > 160 && e->pressure() <= 192 )
	    pen.setColor( saveColor.dark(200) );
	else if ( e->pressure() > 192 && e->pressure() <= 224 )
	    pen.setColor( saveColor.dark(250) );
	else // pressure > 224
	    pen.setColor( saveColor.dark(300) );
    } else if ( e->device() == TQTabletEvent::Eraser
		&& pen.color() != backgroundColor() ) {
	pen.setColor( backgroundColor() );
    }

    int xt = e->xTilt();
    int yt = e->yTilt();
    if ( ( xt > -15 && xt < 15 ) && ( yt > -15 && yt < 15 ) )
	pen.setWidth( 3 );
    else if ( ((xt < -15 && xt > -30) || (xt > 15 && xt < 30)) &&
	      ((yt < -15 && yt > -30) || (yt > 15 && yt < 30 )) )
	pen.setWidth( 6 );
    else if ( ((xt < -30 && xt > -45) || (xt > 30 && xt < 45)) &&
	      ((yt < -30 && yt > -45) || (yt > 30 && yt < 45)) )
	pen.setWidth( 9 );
    else if (  (xt < -45 || xt > 45 ) && ( yt < -45 || yt > 45 ) )
	pen.setWidth( 12 );

    switch ( e->type() ) {
    case TQEvent::TabletPress:
	mousePressed = true;
	polyline[2] = polyline[1] = polyline[0] = e->pos();
	break;
    case TQEvent::TabletRelease:
	mousePressed = false;
	break;
    case TQEvent::TabletMove:
	if ( mousePressed ) {
	    TQPainter painter;
	    painter.begin( &buffer );
	    painter.setPen( pen );
	    polyline[2] = polyline[1];
	    polyline[1] = polyline[0];
	    polyline[0] = e->pos();
	    painter.drawPolyline( polyline );
	    painter.end();

	    TQRect r = polyline.boundingRect();
	    r = r.normalize();
	    r.setLeft( r.left() - penWidth() );
	    r.setTop( r.top() - penWidth() );
	    r.setRight( r.right() + penWidth() );
	    r.setBottom( r.bottom() + penWidth() );

	    bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(),
		    r.height() );
	}
	break;
    default:
	break;
    }
}

void Canvas::resizeEvent( TQResizeEvent *e )
{
    TQWidget::resizeEvent( e );

    int w = width() > buffer.width() ?
	    width() : buffer.width();
    int h = height() > buffer.height() ?
	    height() : buffer.height();

    TQPixmap tmp( buffer );
    buffer.resize( w, h );
    buffer.fill( colorGroup().base() );
    bitBlt( &buffer, 0, 0, &tmp, 0, 0, tmp.width(), tmp.height() );
}

void Canvas::paintEvent( TQPaintEvent *e )
{
    TQWidget::paintEvent( e );

    TQMemArray<TQRect> rects = e->region().rects();
    for ( uint i = 0; i < rects.count(); i++ ) {
	TQRect r = rects[(int)i];
	bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );
    }
}
