/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqtimer.h>
#include <ntqpainter.h>
#include <ntqapplication.h>
#include <stdlib.h>				// defines rand() function

#include "forever.h"


//
// Forever - a widget that draws rectangles forever.
//

//
// Constructs a Forever widget.
//

Forever::Forever( TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    for (int a=0; a<numColors; a++) {
	colors[a] = TQColor( rand()&255,
			    rand()&255,
			    rand()&255 );
    }
    rectangles = 0;
    startTimer( 0 );				// run continuous timer
    TQTimer * counter = new TQTimer( this );
    connect( counter, TQ_SIGNAL(timeout()),
	     this, TQ_SLOT(updateCaption()) );
    counter->start( 1000 );
}


void Forever::updateCaption()
{
    TQString s;
    s.sprintf( "TQt Example - Forever - %d rectangles/second", rectangles );
    rectangles = 0;
    setCaption( s );
}


//
// Handles paint events for the Forever widget.
//

void Forever::paintEvent( TQPaintEvent * )
{
    TQPainter paint( this );			// painter object
    int w = width();
    int h = height();
    if(w <= 0 || h <= 0) 
	return;
    paint.setPen( NoPen );			// do not draw outline
    paint.setBrush( colors[rand() % numColors]);// set random brush color
    
    TQPoint p1( rand()%w, rand()%h );	// p1 = top left
    TQPoint p2( rand()%w, rand()%h );	// p2 = bottom right
    
    TQRect r( p1, p2 );
    paint.drawRect( r );			// draw filled rectangle
}

//
// Handles timer events for the Forever widget.
//

void Forever::timerEvent( TQTimerEvent * )
{
    for ( int i=0; i<100; i++ ) {
	repaint( false );			// repaint, don't erase
	rectangles++;
    }
}


//
// Create and display Forever widget.
//

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );		// create application object
    Forever always;				// create widget
    always.resize( 400, 250 );			// start up with size 400x250
    a.setMainWidget( &always );			// set as main widget
    always.setCaption("TQt Example - Forever");
    always.show();				// show widget
    return a.exec();				// run event loop
}
