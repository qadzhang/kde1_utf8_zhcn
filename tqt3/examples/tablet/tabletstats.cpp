/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqpainter.h>
#include <math.h>

#include "tabletstats.h"

MyOrientation::MyOrientation( TQWidget *parent, const char *name )
    : TQFrame( parent, name, WRepaintNoErase )
{
//    TQSizePolicy mySize( TQSizePolicy::Minimum, TQSizePolicy::Expanding );
//    setSizePolicy( mySize );
    setFrameStyle( TQFrame::Box | TQFrame::Sunken );
}

MyOrientation::~MyOrientation()
{
}

void MyOrientation::newOrient( int tiltX, int tiltY )
{
    double PI = 3.14159265359;
    int realWidth,
        realHeight,
        hypot,	// a faux hypoteneus, to mess with calculations
        shaX,
	shaY;
    static int oldX = 0,
	       oldY = 0;
    realWidth = width() - 2 * frameWidth();
    realHeight = height() - 2 * frameWidth();



    TQRect cr( 0 + frameWidth(), 0 + frameWidth(), realWidth, realHeight );
    TQPixmap pix( cr.size() );
    pix.fill( this, cr.topLeft() );
    TQPainter p( &pix );

    if ( realWidth > realHeight )
	hypot = realHeight / 2;
    else
	hypot = realWidth / 2;

    // create a shadow...
    shaX = int(hypot * sin( tiltX * (PI / 180) ));
    shaY = int(hypot * sin( tiltY * (PI / 180) ));

    p.translate( realWidth / 2, realHeight / 2 );
    p.setPen( backgroundColor() );
    p.drawLine( 0, 0, oldX, oldY );
    p.setPen( foregroundColor() );
    p.drawLine( 0, 0,shaX, shaY );
    oldX = shaX;
    oldY = shaY;
    p.end();

    TQPainter p2( this );
    p2.drawPixmap( cr.topLeft(), pix );
    p2.end();
}


StatsCanvas::StatsCanvas( TQWidget *parent, const char* name )
  : Canvas( parent, name, WRepaintNoErase )
{
    TQSizePolicy mySize( TQSizePolicy::Expanding, TQSizePolicy::Minimum );
    setSizePolicy( mySize );
}

StatsCanvas::~StatsCanvas()
{
}

void StatsCanvas::tabletEvent( TQTabletEvent *e )
{
    static TQRect oldR( -1, -1, -1, -1);
    TQPainter p;

    e->accept();
    switch( e->type() ) {
    case TQEvent::TabletPress:
	tqDebug( "Tablet Press" );
	mousePressed = true;
	break;
    case TQEvent::TabletRelease:
	tqDebug( "Tablet Release" );
	mousePressed = false;
	clearScreen();
	break;
    default:
	break;
    }

    r.setRect( e->x() - e->pressure() / 2,
	e->y() - e->pressure() / 2, e->pressure(), e->pressure() );
    TQRect tmpR = r | oldR;
    oldR = r;

    update( tmpR );
    emit signalNewTilt( e->xTilt(), e->yTilt() );
    emit signalNewDev( e->device() );
    emit signalNewLoc( e->x(), e->y() );
    emit signalNewPressure( e->pressure() );
}

void StatsCanvas::mouseMoveEvent( TQMouseEvent *e )
{
    tqDebug( "Mouse Move" );
    // do nothing
    TQWidget::mouseMoveEvent( e );
}


void StatsCanvas::mousePressEvent( TQMouseEvent *e )
{
    tqDebug( "Mouse Press" );
    TQWidget::mousePressEvent( e );
}

void StatsCanvas::mouseReleaseEvent( TQMouseEvent *e )
{
    tqDebug( "Mouse Release" );
    TQWidget::mouseReleaseEvent( e );
}

void StatsCanvas::paintEvent( TQPaintEvent *e )
{
    TQPainter p;
    p.begin( &buffer );
    p.fillRect( e->rect(), colorGroup().base() );

    // draw a circle if we have the tablet down
    if ( mousePressed ) {
	p.setBrush( red );
	p.drawEllipse( r );
    }
    bitBlt( this, e->rect().x(), e->rect().y(), &buffer, e->rect().x(),
	    e->rect().y(), e->rect().width(), e->rect().height() );
    p.end();
}

TabletStats::TabletStats( TQWidget *parent, const char *name )
	: TabletStatsBase( parent, name )
{
    lblXPos->setMinimumSize( lblXPos->sizeHint() );
    lblYPos->setMinimumSize( lblYPos->sizeHint() );
    lblPressure->setMinimumSize( lblPressure->sizeHint() );
    lblDev->setMinimumSize( lblDev->sizeHint() );
    lblXTilt->setMinimumSize( lblXTilt->sizeHint() );
    lblYTilt->setMinimumSize( lblYTilt->sizeHint() );

    TQObject::connect( statCan, TQ_SIGNAL(signalNewTilt(int, int)),
	              orient, TQ_SLOT(newOrient(int, int)) );
    TQObject::connect( statCan, TQ_SIGNAL(signalNewTilt(int, int)),
	              this, TQ_SLOT(slotTiltChanged(int, int)) );
    TQObject::connect( statCan, TQ_SIGNAL(signalNewDev(int)),
                      this, TQ_SLOT(slotDevChanged(int)) );
    TQObject::connect( statCan, TQ_SIGNAL(signalNewLoc(int,int)),
                      this, TQ_SLOT( slotLocationChanged(int,int)) );
}

TabletStats::~TabletStats()
{
}

void TabletStats::slotDevChanged( int newDev )
{
    if ( newDev == TQTabletEvent::Stylus )
	lblDev->setText( tr("Stylus") );
    else if ( newDev == TQTabletEvent::Eraser )
	lblDev->setText( tr("Eraser") );
}

void TabletStats::slotLocationChanged( int newX, int newY )
{
    lblXPos->setNum( newX );
    lblYPos->setNum( newY );
}

void TabletStats::slotTiltChanged( int newTiltX, int newTiltY )
{
    lblXTilt->setNum( newTiltX );
    lblYTilt->setNum( newTiltY );
}
