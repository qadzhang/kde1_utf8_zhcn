/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqwidget.h>
#include <ntqpainter.h>
#include <ntqapplication.h>
#include <stdlib.h>


const int MAXPOINTS = 2000;			// maximum number of points
const int MAXCOLORS = 40;


//
// ConnectWidget - draws connected lines
//

class ConnectWidget : public TQWidget
{
public:
    ConnectWidget( TQWidget *parent=0, const char *name=0 );
   ~ConnectWidget();
protected:
    void	paintEvent( TQPaintEvent * );
    void	mousePressEvent( TQMouseEvent *);
    void	mouseReleaseEvent( TQMouseEvent *);
    void	mouseMoveEvent( TQMouseEvent *);
private:
    TQPoint     *points;				// point array
    TQColor     *colors;				// color array
    int		count;				// count = number of points
    bool	down;				// true if mouse down
};


//
// Constructs a ConnectWidget.
//

ConnectWidget::ConnectWidget( TQWidget *parent, const char *name )
    : TQWidget( parent, name, WStaticContents )
{
    setBackgroundColor( white );		// white background
    count = 0;
    down = false;
    points = new TQPoint[MAXPOINTS];
    colors = new TQColor[MAXCOLORS];
    for ( int i=0; i<MAXCOLORS; i++ )		// init color array
	colors[i] = TQColor( rand()&255, rand()&255, rand()&255 );
}

ConnectWidget::~ConnectWidget()
{
    delete[] points;				// cleanup
    delete[] colors;
}


//
// Handles paint events for the connect widget.
//

void ConnectWidget::paintEvent( TQPaintEvent * )
{
    TQPainter paint( this );
    for ( int i=0; i<count-1; i++ ) {		// connect all points
	for ( int j=i+1; j<count; j++ ) {
	    paint.setPen( colors[rand()%MAXCOLORS] ); // set random pen color
	    paint.drawLine( points[i], points[j] ); // draw line
	}
    }
}


//
// Handles mouse press events for the connect widget.
//

void ConnectWidget::mousePressEvent( TQMouseEvent * )
{
    down = true;
    count = 0;					// start recording points
    erase();					// erase widget contents
}


//
// Handles mouse release events for the connect widget.
//

void ConnectWidget::mouseReleaseEvent( TQMouseEvent * )
{
    down = false;				// done recording points
    update();					// draw the lines
}


//
// Handles mouse move events for the connect widget.
//

void ConnectWidget::mouseMoveEvent( TQMouseEvent *e )
{
    if ( down && count < MAXPOINTS ) {
	TQPainter paint( this );
	points[count++] = e->pos();		// add point
	paint.drawPoint( e->pos() );		// plot point
    }
}


//
// Create and display a ConnectWidget.
//

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );
    ConnectWidget connect;
#ifndef TQT_NO_WIDGET_TOPEXTRA   // for TQt/Embedded minimal build
    connect.setCaption( "TQt Example - Draw lines");
#endif
    a.setMainWidget( &connect );
    connect.show();
    return a.exec();
}
