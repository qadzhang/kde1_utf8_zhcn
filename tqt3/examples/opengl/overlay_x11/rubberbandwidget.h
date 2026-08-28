/****************************************************************************
**
** Definition of a widget that draws a rubberband. Designed to be used 
** in an X11 overlay visual
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef RUBBERBANDWIDGET_H
#define RUBBERBANDWIDGET_H

#include <ntqwidget.h>


class RubberbandWidget : public TQWidget
{
public:
    RubberbandWidget( TQColor transparentColor, TQWidget *parent=0, 
		      const char *name=0, WFlags f=0 );

protected:
    void mousePressEvent( TQMouseEvent* e );
    void mouseMoveEvent( TQMouseEvent* e );
    void mouseReleaseEvent( TQMouseEvent* e );

    TQColor c;
    TQPoint p1;
    TQPoint p2;
    TQPoint p3;
    bool on;
};

#endif
