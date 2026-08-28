/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "tooltip.h"
#include <ntqapplication.h>
#include <ntqpainter.h>
#include <stdlib.h>


DynamicTip::DynamicTip( TQWidget * parent )
    : TQToolTip( parent )
{
    // no explicit initialization needed
}


void DynamicTip::maybeTip( const TQPoint &pos )
{
    if ( !parentWidget()->inherits( "TellMe" ) )
	return;

    TQRect r( ((TellMe*)parentWidget())->tip(pos) );
    if ( !r.isValid() )
	return;

    TQString s;
    s.sprintf( "position: %d,%d", r.center().x(), r.center().y() );
    tip( r, s );
}


TellMe::TellMe( TQWidget * parent , const char * name  )
    : TQWidget( parent, name )
{
    setMinimumSize( 30, 30 );
    r1 = randomRect();
    r2 = randomRect();
    r3 = randomRect();

    t = new DynamicTip( this );

    TQToolTip::add( this, r3, "this color is called red" ); // <- helpful
}


TellMe::~TellMe()
{
    delete t;
    t = 0;
}


void TellMe::paintEvent( TQPaintEvent * e )
{
    TQPainter p( this );

    // I try to be efficient here, and repaint only what's needed

    if ( e->rect().intersects( r1 ) ) {
	p.setBrush( blue );
	p.drawRect( r1 );
    }

    if ( e->rect().intersects( r2 ) ) {
	p.setBrush( blue );
	p.drawRect( r2 );
    }

    if ( e->rect().intersects( r3 ) ) {
	p.setBrush( red );
	p.drawRect( r3 );
    }
}


void TellMe::mousePressEvent( TQMouseEvent * e )
{
    if ( r1.contains( e->pos() ) )
	r1 = randomRect();
    if ( r2.contains( e->pos() ) )
	r2 = randomRect();
    repaint();
}


void TellMe::resizeEvent( TQResizeEvent * )
{
    if ( !rect().contains( r1 ) )
	 r1 = randomRect();
    if ( !rect().contains( r2 ) )
	 r2 = randomRect();
}


TQRect TellMe::randomRect()
{
    return TQRect( ::rand() % (width() - 20), ::rand() % (height() - 20),
		  20, 20 );
}


TQRect TellMe::tip( const TQPoint & p )
{
    if ( r1.contains( p ) )
	return r1;
    else if ( r2.contains( p ) )
	return r2;
    else
	return TQRect( 0,0, -1,-1 );
}
