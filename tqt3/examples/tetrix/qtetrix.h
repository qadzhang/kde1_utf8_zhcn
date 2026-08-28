/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TQTETRIX_H
#define TQTETRIX_H

#include "qtetrixb.h"
#include <ntqframe.h>
#include <ntqlcdnumber.h>
#include <ntqlabel.h>
#include <ntqpushbutton.h>
#include <ntqpainter.h>


class ShowNextPiece : public TQFrame
{
    TQ_OBJECT
    friend class TQTetrix;
public:
    ShowNextPiece( TQWidget *parent=0, const char *name=0  );
public slots:
    void drawNextSquare( int x, int y,TQColor *color );
signals:
    void update();
private:
    void paintEvent( TQPaintEvent * );
    void resizeEvent( TQResizeEvent * );
    
    int      blockWidth,blockHeight;
    int      xOffset,yOffset;
};


class TQTetrix : public TQWidget
{
    TQ_OBJECT
public:
    TQTetrix( TQWidget *parent=0, const char *name=0 );
    void startGame() { board->startGame(); }

public slots:
    void gameOver();
    void quit();
private:
    void keyPressEvent( TQKeyEvent *e ) { board->keyPressEvent(e); }

    TQTetrixBoard  *board;
    ShowNextPiece *showNext;
#ifndef TQT_NO_LCDNUMBER
    TQLCDNumber    *showScore;
    TQLCDNumber    *showLevel;
    TQLCDNumber    *showLines;
#else
    TQLabel    *showScore;
    TQLabel    *showLevel;
    TQLabel    *showLines;
#endif
    TQPushButton   *quitButton;
    TQPushButton   *startButton;
    TQPushButton   *pauseButton;
};


void drawTetrixButton( TQPainter *, int x, int y, int w, int h,
		       const TQColor *color, TQWidget *widg);


#endif
