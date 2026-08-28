/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "qtetrixb.h"
#include "qtetrix.h"
#include <ntqtimer.h>
#include <ntqpainter.h>

const int waitAfterLineTime = 500;

TQTetrixBoard::TQTetrixBoard( TQWidget *p, const char *name )
    : TQFrame( p, name )
{
    setFrameStyle( TQFrame::Panel | TQFrame::Sunken );
    paint = 0;
    paint_widget = 0;
    timer = new TQTimer(this);
    connect( timer, TQ_SIGNAL(timeout()), TQ_SLOT(timeout()) );

    colors[0].setRgb(200,100,100);
    colors[1].setRgb(100,200,100);
    colors[2].setRgb(100,100,200);
    colors[3].setRgb(200,200,100);
    colors[4].setRgb(200,100,200);
    colors[5].setRgb(100,200,200);
    colors[6].setRgb(218,170,  0);

    xOffset          = -1;      // -1 until a resizeEvent is received.
    blockWidth       = 20;
    yOffset          = 30;
    blockHeight      = 20;
    noGame           = true;
    isPaused         = false;
    waitingAfterLine = false;
    updateTimeoutTime();   // Sets timeoutTime
}

void TQTetrixBoard::startGame(int gameType,int fillRandomLines)
{
    if ( isPaused )
        return;		// ignore if game is paused
    noGame = false;
    GenericTetrix::startGame( gameType, fillRandomLines );
    // Note that the timer is started by updateLevel!
}


void TQTetrixBoard::pause()
{
    if ( noGame )			// game not active
        return;
    isPaused = !isPaused;
    if ( isPaused ) {
	timer->stop();
        hideBoard();
    }
    else
	timer->start(timeoutTime);
    update();
}


void TQTetrixBoard::drawSquare(int x,int y,int value)
{
    if (xOffset == -1)    // Before first resizeEvent?
        return;

    const int X = xOffset  + x*blockWidth;
    const int Y = yOffset  + (y - 1)*blockHeight;

    bool localPainter = paint == 0;
    TQPainter *p;
    TQWidget *w;
    if ( localPainter ) {
	p = new TQPainter( this );
	w = this;
    } else {
	p = paint;
	w = paint_widget;
    }
    drawTetrixButton( p, X, Y, blockWidth, blockHeight,
		      value == 0 ? 0 : &colors[value-1], w);
    /*
    if ( value != 0 ) {
	TQColor tc, bc;
	tc = colors[value-1].light();
	bc = colors[value-1].dark();
	p->drawShadePanel( X, Y, blockWidth, blockHeight,
			   tc, bc, 1, colors[value-1], true );
    }
    else
	p->fillRect( X, Y, blockWidth, blockHeight, backgroundColor() );
	*/
    if ( localPainter )
	delete p;
}

void TQTetrixBoard::drawNextSquare( int x, int y, int value )
{
    if ( value == 0 )
        emit drawNextSquareSignal (x, y, 0 );
    else
        emit drawNextSquareSignal( x, y, &colors[value-1] );
}

void TQTetrixBoard::updateRemoved( int noOfLines )
{
    if ( noOfLines > 0 ) {
        timer->stop();
        timer->start( waitAfterLineTime );
        waitingAfterLine = true;
    }
    emit updateRemovedSignal( noOfLines );
}

void TQTetrixBoard::updateScore( int newScore )
{
    emit updateScoreSignal( newScore );
}

void TQTetrixBoard::updateLevel( int newLevel )
{
    timer->stop();
    updateTimeoutTime();
    timer->start( timeoutTime );
    emit updateLevelSignal( newLevel );
}

void TQTetrixBoard::pieceDropped(int)
{
    if ( waitingAfterLine ) // give player a break if a line has been removed
        return;
    newPiece();
}

void TQTetrixBoard::gameOver()
{
    timer->stop();
    noGame = true;
    emit gameOverSignal();
}

void TQTetrixBoard::timeout()
{
    if ( waitingAfterLine ) {
	timer->stop();
	waitingAfterLine = false;
	newPiece();
	timer->start( timeoutTime );
    } else {
        oneLineDown();
    }
}

void TQTetrixBoard::drawContents( TQPainter *p )
{
    const char *text = "Press \"Pause\"";
    TQRect r = contentsRect();
    paint = p;				// set widget painter
    if ( isPaused ) {
	p->drawText( r, AlignCenter | AlignVCenter, text );
        return;
    }
    int x1,y1,x2,y2;
    x1 = (r.left() - xOffset) / blockWidth;
    if (x1 < 0)
        x1 = 0;
    if (x1 >= boardWidth())
        x1 = boardWidth() - 1;

    x2 = (r.right() - xOffset) / blockWidth;
    if (x2 < 0)
        x2 = 0;
    if (x2 >= boardWidth())
        x2 = boardWidth() - 1;

    y1 = (r.top() - yOffset) / blockHeight;
    if (y1 < 0)
        y1 = 0;
    if (y1 >= boardHeight())
        y1 = boardHeight() - 1;

    y2 = (r.bottom() - yOffset) / blockHeight;
    if (y2 < 0)
        y2 = 0;
    if (y2 >= boardHeight())
        y2 = boardHeight() - 1;

    updateBoard( x1, y1, x2, y2, true );
    paint = 0;				// reset widget painter
    return;
}

void TQTetrixBoard::resizeEvent(TQResizeEvent *e)
{
    TQSize sz = e->size();
    blockWidth  = (sz.width() - 3)/10;
    blockHeight = (sz.height() - 3)/22;
    xOffset     = 1;
    yOffset     = 1;
}

void TQTetrixBoard::keyPressEvent( TQKeyEvent *e )
{
    if ( noGame || isPaused || waitingAfterLine )
        return;
    switch( e->key() ) {
	case Key_Left :
	    moveLeft();
	    break;
	case Key_Right :
	    moveRight();
	    break;
	case Key_Down :
	    rotateRight();
	    break;
	case Key_Up :
	    rotateLeft();
	    break;
	case Key_Space :
	    dropDown();
	    break;
	case Key_D :
	    oneLineDown();
	    break;
        default:
	    return;
    }
    e->accept();
}

void TQTetrixBoard::updateTimeoutTime()
{
    timeoutTime = 1000/(1 + getLevel());
}
