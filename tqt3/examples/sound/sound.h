/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
#ifndef PLAY_H
#define PLAY_H

#include "ntqsound.h"
#include <ntqmainwindow.h>

class SoundPlayer : public TQMainWindow {
    TQ_OBJECT
public:
    SoundPlayer();

public slots:
    void doPlay1();
    void doPlay2();
    void doPlay3();
    void doPlay4();
    void doPlay34();
    void doPlay1234();

private:
    TQSound bucket3;
    TQSound bucket4;
};

#endif
