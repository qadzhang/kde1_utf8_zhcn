/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef ACLOCK_H
#define ACLOCK_H

#include <ntqwidget.h>
#include <ntqdatetime.h>

class TQTimer;
class AnalogClock : public TQWidget		// analog clock widget
{
    TQ_OBJECT
public:
    AnalogClock( TQWidget *parent=0, const char *name=0 );
    void setAutoMask(bool b);
    
protected:
    void updateMask();
    void paintEvent( TQPaintEvent *);
    void mousePressEvent( TQMouseEvent *);
    void mouseMoveEvent( TQMouseEvent *);
    void drawClock( TQPainter* );
    
private slots:
    void timeout();

public slots:
    void setTime( const TQTime & t );

private:
    TQPoint clickPos;
    TQTime time;
    TQTimer *internalTimer;
};


#endif // ACLOCK_H
