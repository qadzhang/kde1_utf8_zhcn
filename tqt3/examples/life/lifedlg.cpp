/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "lifedlg.h"
#include <ntqapplication.h>
#include <ntqpushbutton.h>
#include <ntqlabel.h>
#include <ntqslider.h>
#include <ntqcombobox.h>
#include <ntqdatetime.h>
#include <stdlib.h>

#include "patterns.cpp"


// A simple timer which has a pause and a setSpeed slot

LifeTimer::LifeTimer( TQWidget *parent ) : TQTimer( parent ), interval( 500 )
{
    start( interval );
}


void LifeTimer::pause( bool stopIt )
{
    if ( stopIt )
	stop();
    else
	start( interval );
}


void LifeTimer::setSpeed( int speed )
{
    interval = MAXSPEED - speed; 
    if ( isActive() )
	changeInterval( interval );
}


// A top-level container widget to organize the others

LifeDialog::LifeDialog( int scale, TQWidget * parent, const char * name )
    : TQWidget( parent, name )
{
    qb = new TQPushButton( "Quit!", this );
    cb = new TQComboBox( this, "comboBox" );
    life = new LifeWidget(scale, this);
    life->move( SIDEBORDER, TOPBORDER );


    connect( qb, TQ_SIGNAL(clicked()), tqApp, TQ_SLOT(quit()) );
    qb->setGeometry( SIDEBORDER, SIDEBORDER, qb->sizeHint().width(), 25 );
    timer = new LifeTimer( this );

    connect( timer, TQ_SIGNAL(timeout()), life, TQ_SLOT(nextGeneration()) );
    pb = new TQPushButton( "Pause", this );
    pb->setToggleButton( true );
    connect( pb, TQ_SIGNAL(toggled(bool)), timer, TQ_SLOT(pause(bool)) );
    pb->resize( pb->sizeHint().width(), 25 );
    pb->move( width() - SIDEBORDER - pb->width(), SIDEBORDER );

    sp = new TQLabel( "Speed:", this );
    sp->adjustSize();
    sp->move( SIDEBORDER, 45 );
    scroll = new TQSlider( 0, LifeTimer::MAXSPEED, 50,
			     LifeTimer::MAXSPEED / 2,
			     TQSlider::Horizontal, this );
    connect( scroll, TQ_SIGNAL(valueChanged(int)),
	     timer,  TQ_SLOT(setSpeed(int)) );

    scroll->move( sp->width() + 2 * SIDEBORDER, 45 );
    scroll->resize( 200, 15 );

    life->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );
    life->show();

    srand( TQTime(0,0,0).msecsTo(TQTime::currentTime()) );
    int sel =  rand() % NPATS;
    getPattern( sel );

    cb->move( 2*SIDEBORDER + qb->width(), SIDEBORDER);
    cb->insertItem( "Glider Gun " );
    cb->insertItem( "Figure Eight " );
    cb->insertItem( "Pulsar " );
    cb->insertItem( "Barber Pole P2 " );
    cb->insertItem( "Achim P5 " );
    cb->insertItem( "Hertz P4 " );
    cb->insertItem( "Tumbler " );
    cb->insertItem( "Pulse1 P4" );
    cb->insertItem( "Shining Flower P5 " );
    cb->insertItem( "Pulse2 P6 " );
    cb->insertItem( "Pinwheel, Clock P4 " );
    cb->insertItem( "Pentadecatholon " );
    cb->insertItem( "Piston " );
    cb->insertItem( "Piston2 " );
    cb->insertItem( "Switch Engine " );
    cb->insertItem( "Gears (Gear, Flywheel, Blinker) " );
    cb->insertItem( "Turbine8 " );
    cb->insertItem( "P16 " );
    cb->insertItem( "Puffer " );
    cb->insertItem( "Escort " );
    cb->insertItem( "Dart Speed 1/3 " );
    cb->insertItem( "Period 4 Speed 1/2 " );
    cb->insertItem( "Another Period 4 Speed 1/2 " );
    cb->insertItem( "Smallest Known Period 3 Spaceship Speed 1/3 " );
    cb->insertItem( "Turtle Speed 1/3 " );
    cb->insertItem( "Smallest Known Period 5 Speed 2/5 " );
    cb->insertItem( "Sym Puffer " );
    cb->insertItem( "], Near Ship, Pi Heptomino " );
    cb->insertItem( "R Pentomino " );
    cb->setAutoResize( false );
    cb->setCurrentItem( sel );
    cb->show();
    connect( cb, TQ_SIGNAL(activated(int)), TQ_SLOT(getPattern(int)) );

    TQSize s;
    s = life->minimumSize();
    setMinimumSize( s.width() + 2 * SIDEBORDER, 
		    s.height() + TOPBORDER + SIDEBORDER );
    s = life->maximumSize();
    setMaximumSize( s.width() + 2 * SIDEBORDER, 
		    s.height() + TOPBORDER + SIDEBORDER );
    s = life->sizeIncrement();
    setSizeIncrement( s.width(), s.height() );

    resize( TQMIN(512, tqApp->desktop()->width()),
	    TQMIN(480, tqApp->desktop()->height()) );
}


void LifeDialog::resizeEvent( TQResizeEvent * e )
{
    life->resize( e->size() - TQSize( 2 * SIDEBORDER, TOPBORDER + SIDEBORDER ));
    pb->move( e->size().width() - SIDEBORDER - pb->width(), SIDEBORDER );
    scroll->resize( e->size().width() - sp->width() - 3 * SIDEBORDER,
		    scroll->height() );
    cb->resize( width() - 4*SIDEBORDER - qb->width() - pb->width()  , 25 );
}


// Adapted from xlock, see pattern.cpp for copyright info.

void LifeDialog::getPattern( int pat )
{
    life->clear();
    int i = pat % NPATS;
    int col;
    int * patptr = &patterns[i][0];
    while ( (col = *patptr++) != 127 ) {
	int row = *patptr++;
	col += life->maxCol() / 2;
	row += life->maxRow() / 2;
	life->setPoint( col, row );
    }
}
