/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef LIFEDLG_H
#define LIFEDLG_H

#include <ntqtimer.h>
#include <ntqwidget.h>

class TQSlider;
class TQPushButton;
class TQLabel;
class TQComboBox;

#include "life.h"


class LifeTimer : public TQTimer
{
    TQ_OBJECT
public:
    LifeTimer( TQWidget *parent );
    enum { MAXSPEED = 1000 };

public slots:
    void	setSpeed( int speed );
    void	pause( bool );

private:
    int		interval;
};


class LifeDialog : public TQWidget
{
    TQ_OBJECT
public:
    LifeDialog( int scale = 10, TQWidget *parent = 0, const char *name = 0 );
public slots:
    void	getPattern( int );

protected:
    virtual void resizeEvent( TQResizeEvent * e );

private:
    enum { TOPBORDER = 70, SIDEBORDER = 10 };

    LifeWidget	*life;
    TQPushButton *qb;
    LifeTimer	*timer;
    TQPushButton *pb;
    TQComboBox	*cb;
    TQLabel	*sp;
    TQSlider	*scroll;
};


#endif // LIFEDLG_H
