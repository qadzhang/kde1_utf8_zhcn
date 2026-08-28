/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef RICHTEXT_H
#define RICHTEXT_H

#include <ntqvbox.h>

class TQTextView;
class TQPushButton;

class MyRichText : public TQVBox
{
    TQ_OBJECT

public:
    MyRichText( TQWidget *parent = 0, const char *name = 0 );

protected:
    TQTextView *view;
    TQPushButton *bClose, *bNext, *bPrev;
    int num;

protected slots:
    void prev();
    void next();

};

#endif
