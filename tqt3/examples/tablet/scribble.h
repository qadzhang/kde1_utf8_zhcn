/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef SCRIBBLE_H
#define SCRIBBLE_H

#include <ntqmainwindow.h>
#include <ntqpen.h>
#include <ntqpoint.h>
#include <ntqpixmap.h>
#include <ntqwidget.h>
#include <ntqstring.h>
#include <ntqpointarray.h>

class TQMouseEvent;
class TQResizeEvent;
class TQPaintEvent;
class TQSpinBox;
class TQToolButton;
class Canvas;

class Scribble : public TQMainWindow
{
    TQ_OBJECT

public:
    Scribble( TQWidget *parent = 0, const char *name = 0 );

protected:
    Canvas* canvas;

    TQSpinBox *bPWidth;
    TQToolButton *bPColor, *bSave, *bClear;

protected slots:
    void slotSave();
    void slotColor();
    void slotWidth( int );
    void slotClear();

};

#endif
