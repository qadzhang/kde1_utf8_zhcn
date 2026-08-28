/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TQTETRIXB_H
#define TQTETRIXB_H

#include "gtetrix.h"
#include <ntqframe.h>

class TQTimer;

class TQTetrixBoard : public TQFrame, public GenericTetrix
{
    TQ_OBJECT
public:
    TQTetrixBoard( TQWidget *parent=0, const char *name=0 );

    void      gameOver();
    void      startGame(int gameType = 0,int fillRandomLines = 0);

public slots:
    void      timeout();
    void      updateNext()	{ GenericTetrix::updateNext(); }
    void      key(TQKeyEvent *e) { keyPressEvent(e); }
    void      start()		{ startGame(); }
    void      pause();

signals:
    void      gameOverSignal();
    void      drawNextSquareSignal(int x,int y,TQColor *color1);
    void      updateRemovedSignal(int noOfLines);
    void      updateScoreSignal(int score);
    void      updateLevelSignal(int level);

public:       // until we have keyboard focus, should be protected
    void      keyPressEvent( TQKeyEvent * );

private:
    void      drawContents( TQPainter * );
    void      resizeEvent( TQResizeEvent * );
    void      drawSquare(int x,int y,int value);
    void      drawNextSquare(int x,int y,int value);
    void      updateRemoved(int noOfLines);
    void      updateScore(int newScore);
    void      updateLevel(int newLlevel);
    void      pieceDropped(int dropHeight);
    void      updateTimeoutTime();

    TQTimer   *timer;

    int       xOffset,yOffset;
    int       blockWidth,blockHeight;
    int       timeoutTime;
    bool      noGame;
    bool      isPaused;
    bool      waitingAfterLine;

    TQColor    colors[7];
    TQPainter *paint;
    TQWidget *paint_widget;
};

#endif
