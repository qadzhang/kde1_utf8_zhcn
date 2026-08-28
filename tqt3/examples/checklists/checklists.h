/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef CHECKLISTS_H
#define CHECKLISTS_H

#include <ntqwidget.h>

class TQListView;
class TQLabel;

class CheckLists : public TQWidget
{
    TQ_OBJECT

public:
    CheckLists( TQWidget *parent = 0, const char *name = 0 );

protected:
    TQListView *lv1, *lv2;
    TQLabel *label;

protected slots:
    void copy1to2();
    void copy2to3();

};

#endif
