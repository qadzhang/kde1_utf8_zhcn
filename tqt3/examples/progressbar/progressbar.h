/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <ntqbuttongroup.h>
#include <ntqtimer.h>

class TQRadioButton;
class TQPushButton;
class TQProgressBar;

class ProgressBar : public TQButtonGroup
{
    TQ_OBJECT

public:
    ProgressBar( TQWidget *parent = 0, const char *name = 0 );

protected:
    TQRadioButton *slow, *normal, *fast;
    TQPushButton *start, *pause, *reset;
    TQProgressBar *progress;
    TQTimer timer;

protected slots:
    void slotStart();
    void slotReset();
    void slotTimeout();

};

#endif
