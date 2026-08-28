/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef HELLO_H
#define HELLO_H

#include <ntqwidget.h>


class Hello : public TQWidget
{
    TQ_OBJECT
public:
    Hello( const char *text, TQWidget *parent=0, const char *name=0 );
signals:
    void clicked();
protected:
    void mouseReleaseEvent( TQMouseEvent * );
    void paintEvent( TQPaintEvent * );
private slots:
    void animate();
private:
    TQString t;
    int     b;
};

#endif
