/****************************************************************************
**
** Definition of something or other
**
** Created : 979899
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef FOREVER_H
#define FOREVER_H

#include <ntqwidget.h>


const int numColors = 120;


class Forever : public TQWidget
{
    TQ_OBJECT
public:
    Forever( TQWidget *parent=0, const char *name=0 );
protected:
    void	paintEvent( TQPaintEvent * );
    void	timerEvent( TQTimerEvent * );
private slots:
    void	updateCaption();
private:
    int		rectangles;
    TQColor	colors[numColors];
};


#endif
