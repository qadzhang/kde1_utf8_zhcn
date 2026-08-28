/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "qtetrix.h"
#include <ntqapplication.h>
#include <ntqlabel.h>
#include <ntqdatetime.h>


void drawTetrixButton( TQPainter *p, int x, int y, int w, int h,
		       const TQColor *color, TQWidget *widg)
{
    if ( color ) {
        TQPointArray a;
	a.setPoints( 3,  x,y+h-1, x,y, x+w-1,y );
	p->setPen( color->light() );
	p->drawPolyline( a );
	a.setPoints( 3, x+1,y+h-1, x+w-1,y+h-1, x+w-1,y+1 );
	p->setPen( color->dark() );
	p->drawPolyline( a );
	x++;
	y++;
	w -= 2;
	h -= 2;
	p->fillRect( x, y, w, h, *color );
    }
    else if(widg) {
	widg->erase(x, y, w, h);
    } else {
	p->fillRect(x, y, w, h, p->backgroundColor());
    }
}


ShowNextPiece::ShowNextPiece( TQWidget *parent, const char *name )
    : TQFrame( parent, name )
{
    setFrameStyle( TQFrame::Panel | TQFrame::Sunken );
    xOffset = -1;     // -1 until first resizeEvent.
}

void ShowNextPiece::resizeEvent( TQResizeEvent *e )
{
    TQSize sz = e->size();
    blockWidth  = (sz.width()  - 3)/5;
    blockHeight = (sz.height() - 3)/6;
    xOffset     = (sz.width()  - 3)/5;
    yOffset     = (sz.height() - 3)/6;
}


void ShowNextPiece::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );
    drawFrame( &p );
    p.end();			// explicit end() so any slots can paint too
    emit update();
}


void ShowNextPiece::drawNextSquare(int x, int y,TQColor *color)
{
    if (xOffset == -1)		// Before first resizeEvent?
        return;

    TQPainter paint;
    paint.begin(this);
    drawTetrixButton( &paint, xOffset+x*blockWidth, yOffset+y*blockHeight,
		      blockWidth, blockHeight, color, this );
    paint.end();
}


TQTetrix::TQTetrix( TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    TQTime t = TQTime::currentTime();
    TetrixPiece::setRandomSeed( (((double)t.hour())+t.minute()+t.second())/
                                 (24+60+60) );

#define ADD_LABEL( str, x, y, w, h )			\
    { TQLabel *label = new TQLabel(str,this); 		\
      label->setGeometry(x,y,w,h); 			\
      label->setAlignment(AlignCenter|AlignVCenter); }

    ADD_LABEL( "NEXT", 50, 10, 78, 30 );
    ADD_LABEL( "SCORE", 330, 10, 178, 30 );
    ADD_LABEL( "LEVEL", 50, 130, 78, 30 );
    ADD_LABEL( "LINES REMOVED", 330, 130, 178, 30 );

    board       = new TQTetrixBoard(this);
    showNext    = new ShowNextPiece(this);
#ifndef TQT_NO_LCDNUMBER
    showScore   = new TQLCDNumber(5,this);
    showLevel   = new TQLCDNumber(2,this);
    showLines   = new TQLCDNumber(5,this);
#else
    showScore   = new TQLabel("0",this);
    showLevel   = new TQLabel("0",this);
    showLines   = new TQLabel("0",this);
    showScore->setAlignment(AlignCenter);
    showLines->setAlignment(AlignCenter);
    showLevel->setAlignment(AlignCenter);
    showScore->setFrameStyle(TQFrame::Sunken|TQFrame::Box);
    showLines->setFrameStyle(TQFrame::Sunken|TQFrame::Box);
    showLevel->setFrameStyle(TQFrame::Sunken|TQFrame::Box);
#endif    
    quitButton  = new TQPushButton("&Quit",this);
    startButton = new TQPushButton("&New Game",this);
    pauseButton = new TQPushButton("&Pause",this);

    // Don't let the buttons get keyboard focus
    quitButton->setFocusPolicy( TQWidget::NoFocus );
    startButton->setFocusPolicy( TQWidget::NoFocus );
    pauseButton->setFocusPolicy( TQWidget::NoFocus );

    connect( board, TQ_SIGNAL(gameOverSignal()), TQ_SLOT(gameOver()) );
    connect( board, TQ_SIGNAL(drawNextSquareSignal(int,int,TQColor*)), showNext,
	     TQ_SLOT(drawNextSquare(int,int,TQColor*)) );
    connect( showNext, TQ_SIGNAL(update()), board, TQ_SLOT(updateNext()) );
#ifndef TQT_NO_LCDNUMBER
    connect( board, TQ_SIGNAL(updateScoreSignal(int)), showScore,
	     TQ_SLOT(display(int)) );
    connect( board, TQ_SIGNAL(updateLevelSignal(int)), showLevel,
	     TQ_SLOT(display(int)));
    connect( board, TQ_SIGNAL(updateRemovedSignal(int)), showLines,
	     TQ_SLOT(display(int)));
#else
    connect( board, TQ_SIGNAL(updateScoreSignal(int)), showScore,
	     TQ_SLOT(setNum(int)) );
    connect( board, TQ_SIGNAL(updateLevelSignal(int)), showLevel,
	     TQ_SLOT(setNum(int)));
    connect( board, TQ_SIGNAL(updateRemovedSignal(int)), showLines,
	     TQ_SLOT(setNum(int)));
#endif
    connect( startButton, TQ_SIGNAL(clicked()), board, TQ_SLOT(start()) );
    connect( quitButton , TQ_SIGNAL(clicked()), TQ_SLOT(quit()));
    connect( pauseButton, TQ_SIGNAL(clicked()), board, TQ_SLOT(pause()) );

    board->setGeometry( 150, 20, 153, 333 );
    showNext->setGeometry( 50, 40, 78, 94 );
    showScore->setGeometry( 330, 40, 178, 93 );
    showLevel->setGeometry( 50, 160, 78, 93 );
    showLines->setGeometry( 330, 160, 178, 93 );
#ifndef TQT_NO_LCDNUMBER
    showScore->display( 0 );
    showLevel->display( 0 );
    showLines->display( 0 );
#else
    showScore->setNum( 0 );
    showLevel->setNum( 0 );
    showLines->setNum( 0 );
#endif    
    startButton->setGeometry( 46, 288, 90, 30 );
    quitButton->setGeometry( 370, 265, 90, 30 );
    pauseButton->setGeometry( 370, 310, 90, 30 );
    board->revealNextPiece(true);

    resize( 550, 370 );
}

void TQTetrix::gameOver()
{
}


void TQTetrix::quit()
{
    tqApp->quit();
}
