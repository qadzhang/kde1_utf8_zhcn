/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "aclock.h"
#include <ntqtimer.h>
#include <ntqpainter.h>
#include <ntqbitmap.h>

//
// Constructs an analog clock widget that uses an internal TQTimer.
//

AnalogClock::AnalogClock( TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    time = TQTime::currentTime();		// get current time
    internalTimer = new TQTimer( this );	// create internal timer
    connect( internalTimer, TQ_SIGNAL(timeout()), TQ_SLOT(timeout()) );
    internalTimer->start( 5000 );		// emit signal every 5 seconds
}

void AnalogClock::mousePressEvent( TQMouseEvent *e )
{
    if(isTopLevel()) 
	clickPos = e->pos() + TQPoint(geometry().topLeft() - frameGeometry().topLeft());
}

void AnalogClock::mouseMoveEvent( TQMouseEvent *e )
{
    if(isTopLevel())
	move( e->globalPos() - clickPos );
}

//
// The TQTimer::timeout() signal is received by this slot.
//

//
// When we set an explicit time we don't want the timeout() slot to be 
// called anymore as this relies on currentTime()
//
void AnalogClock::setTime( const TQTime & t )
{
    time = t;
    disconnect( internalTimer, TQ_SIGNAL(timeout()), this, TQ_SLOT(timeout()) );
    if (autoMask())
	updateMask();
    else
	update();
}


void AnalogClock::timeout()
{
    TQTime old_time = time;
    time = TQTime::currentTime();
    if ( old_time.minute() != time.minute() 
	|| old_time.hour() != time.hour() ) {	// minute or hour has changed
	if (autoMask())
	    updateMask();
	else
	    update();
    }
}


void AnalogClock::paintEvent( TQPaintEvent * )
{
    if ( autoMask() )
	return;
    TQPainter paint( this );
    paint.setBrush( colorGroup().foreground() );
    drawClock( &paint );
}

// If the clock is transparent, we use updateMask()
// instead of paintEvent()

void AnalogClock::updateMask()	// paint clock mask
{
    TQBitmap bm( size() );
    bm.fill( color0 );			//transparent

    TQPainter paint;
    paint.begin( &bm, this );
    paint.setBrush( color1 );		// use non-transparent color
    paint.setPen( color1 );

    drawClock( &paint );

    paint.end();
    setMask( bm );
}

//
// The clock is painted using a 1000x1000 square coordinate system, in
// the a centered square, as big as possible.  The painter's pen and
// brush colors are used.
//
void AnalogClock::drawClock( TQPainter *paint )
{
    paint->save();
    
    paint->setWindow( -500,-500, 1000,1000 );

    TQRect v = paint->viewport();
    int d = TQMIN( v.width(), v.height() );
    paint->setViewport( v.left() + (v.width()-d)/2,
			v.top() + (v.height()-d)/2, d, d );
    
    TQPointArray pts;

    paint->save();
    paint->rotate( 30*(time.hour()%12-3) + time.minute()/2 );
    pts.setPoints( 4, -20,0,  0,-20, 300,0, 0,20 );
    paint->drawConvexPolygon( pts );
    paint->restore();

    paint->save();
    paint->rotate( (time.minute()-15)*6 );
    pts.setPoints( 4, -10,0, 0,-10, 400,0, 0,10 );
    paint->drawConvexPolygon( pts );
    paint->restore();

    for ( int i=0; i<12; i++ ) {
	paint->drawLine( 440,0, 460,0 );
	paint->rotate( 30 );
    }

    paint->restore();
}


void AnalogClock::setAutoMask(bool b)
{
    if (b) 
	setBackgroundMode( PaletteForeground );
    else 
	setBackgroundMode( PaletteBackground );
    TQWidget::setAutoMask(b);
}
