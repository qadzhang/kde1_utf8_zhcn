/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "display.h"

#include <ntqpainter.h>
#include <ntqlayout.h>
#include <ntqtimer.h>
#include <ntqpushbutton.h>
#include <ntqframe.h>
#include <ntqdial.h>
#include <ntqlcdnumber.h>
#include <ntqprogressbar.h>
#include <ntqspinbox.h>

#include <math.h>

Screen::Screen(  TQWidget *parent, const char *name )
    : TQFrame( parent, name )
{
    setLineWidth( FrameWidth );
    setFrameStyle( Panel | Sunken );
    setBackgroundMode( PaletteBase );
    setSizePolicy( TQSizePolicy::MinimumExpanding, TQSizePolicy::MinimumExpanding );
    setPaletteBackgroundColor( black );
    setPaletteForegroundColor( blue );

    yval = new int[width()];
    memset( yval, 0, sizeof(int)*width() );
    pos0 = 0;
    t0 = 0;
    step = 0;
}

Screen::~Screen()
{
    delete yval;
}

void Screen::resizeEvent( TQResizeEvent *e )
{
    delete yval;
    int w = e->size().width();
    yval = new int[w];
    memset( yval, 0, sizeof(int)*w);
}

void Screen::animate()
{
    if ( step == 0 )
	return;

    int t = t0;
    int p = pos0;
    if ( step < 0 ) {
	t += width() + step;
    } else {
	t -= step;
	p -= step;
	if ( p < 0 )
	    p += width();
   }

    for ( int i = 0; i < TQABS( step ); i++ ) {
	int y = (int)((height()-FrameWidth)/2 * sin( 3.1415*(double)t/180.0 ));
	yval[ p ] = y;
	++t;
	t %= 360;
	++p;
	p %= width();
    }
    t0 -= step;
    if ( t0 < 0 )
	t0 += 360;
    pos0 = (pos0 - step) % width();
    if ( pos0 < 0 )
	pos0 += width();

    scroll( step, 0, TQRect( FrameWidth, FrameWidth, width()-2*FrameWidth, height()-2*FrameWidth ));
}

void Screen::setStep( int s )
{
    step = s;
}

void Screen::drawContents( TQPainter *p )
{
    TQRect r = p->hasClipping() ?
	      p->clipRegion().boundingRect() : contentsRect();

    int vp = ( r.left() - FrameWidth + pos0 ) % width();
    int y0 = FrameWidth + height()/2;
    
    for ( int x = r.left(); x <= r.right(); x++ ) {
	p->drawLine( x, y0 + yval[ vp ], x, r.bottom());
	++vp;
	vp %= width();
    }
}

/***********************************************************************/

Curve::Curve( TQWidget *parent, const char *name )
    : TQFrame( parent, name )
{
    setLineWidth( FrameWidth );
    setFrameStyle( Panel | Sunken );
    setBackgroundMode( PaletteBase );
    setPaletteBackgroundColor(black);
    setPaletteForegroundColor(red);
    setSizePolicy( TQSizePolicy::MinimumExpanding, TQSizePolicy::MinimumExpanding );

    shift = 0;
    n = 1;
}

void Curve::drawContents( TQPainter *p )
{
    p->moveTo( width()/2, height()/2 + (int)(90.0*sin( double(shift)*3.1415/180.0)));

    for ( double a = 0.0; a < 360.0; a += 1.0 ) {
	double rad = 3.1415 / 180.0 * a;
	double x = width()/2 + 90.0 * sin(rad);
	double y = height()/2 + 90.0 * sin(n * rad + double(shift)*3.1415/180.0);
	p->lineTo( int(x), int(y) );
    }
}

void Curve::animate()
{
    shift = (shift + 1) % 360;
    update( FrameWidth, FrameWidth, width() - 2*FrameWidth, height() - 2*FrameWidth );
}

void Curve::setFactor( int f )
{
    n = f;
}

/***********************************************************************/

DisplayWidget::DisplayWidget( TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    timer = 0;

    TQVBoxLayout *vbox = new TQVBoxLayout( this, 10 );

    TQHBoxLayout *hbox = new TQHBoxLayout( vbox );
    screen = new Screen( this );
    dial = new TQDial( this );
    dial->setNotchesVisible( true );
    dial->setRange( -10, 10 );
    dial->setValue( 1 );
    screen->setStep( dial->value() );
    connect( dial, TQ_SIGNAL( valueChanged( int )),
	     screen, TQ_SLOT( setStep( int )));
    lcd = new TQLCDNumber( 2, this );
    lcd->setSizePolicy( TQSizePolicy::MinimumExpanding, TQSizePolicy::Preferred );
    lcdval = 0;
    
    hbox->addWidget( screen );

    TQVBoxLayout *vb2 = new TQVBoxLayout( hbox );

    curve = new Curve( this );
    spin = new TQSpinBox( 1, 10, 1, this );
    connect( spin, TQ_SIGNAL( valueChanged( int )), curve, TQ_SLOT( setFactor( int )));
    spin->setValue( 2 );
    vb2->addWidget( curve );
    vb2->addWidget( spin );
    
    TQHBoxLayout *hbox2 = new TQHBoxLayout( vb2 );
    
    hbox2->addWidget( dial );
    hbox2->addWidget( lcd );
    
    bar = new TQProgressBar( 10, this );
    tbar = 0;

    vbox->addWidget( bar );
}

void DisplayWidget::run()
{
    if ( !timer ) {
	timer = new TQTimer( this );
	connect( timer, TQ_SIGNAL( timeout() ), TQ_SLOT( tick() ) );
    }

    timer->start( 5 );
}

void DisplayWidget::stop()
{
    timer->stop();
}

void DisplayWidget::tick()
{
    // sine
    screen->animate();
    // Lissajous
    curve->animate();
    // lcd display
    lcd->display( ++lcdval % 100 );
    // progress bar
    bar->setProgress( 5 + (int)(5*sin( 3.1415 * (double)tbar / 180.0 )));
    ++tbar;
    tbar %= 360;
}

void DisplayWidget::showEvent( TQShowEvent * )
{
    run();
    screen->repaint();
}

void DisplayWidget::hideEvent( TQHideEvent * )
{
    stop();
}

