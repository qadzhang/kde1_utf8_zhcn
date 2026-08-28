/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef DCLOCK_H
#define DCLOCK_H

#include <ntqlcdnumber.h>


class DigitalClock : public TQLCDNumber		// digital clock widget
{
    TQ_OBJECT
public:
    DigitalClock( TQWidget *parent=0, const char *name=0 );

protected:					// event handlers
    void	timerEvent( TQTimerEvent * );
    void	mousePressEvent( TQMouseEvent * );

private slots:					// internal slots
    void	stopDate();
    void	showTime();

private:					// internal data
    void	showDate();

    bool	showingColon;
    int		normalTimer;
    int		showDateTimer;
};


#endif // DCLOCK_H
