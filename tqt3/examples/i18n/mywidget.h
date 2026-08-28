/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <ntqmainwindow.h>
#include <ntqstring.h>

class MyWidget : public TQMainWindow
{
    TQ_OBJECT

public:
    MyWidget( TQWidget* parent=0, const char* name = 0 );

signals:
    void closed();

protected:
    void closeEvent(TQCloseEvent*);

private:
    static void initChoices(TQWidget* parent);
};

#endif
