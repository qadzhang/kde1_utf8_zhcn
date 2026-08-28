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
class TQToolButton;
class TQSpinBox;

class Canvas : public TQWidget
{
    TQ_OBJECT

public:
    Canvas( TQWidget *parent = 0, const char *name = 0 );

    void setPenColor( const TQColor &c )
    { pen.setColor( c ); }

    void setPenWidth( int w )
    { pen.setWidth( w ); }

    TQColor penColor()
    { return pen.color(); }

    int penWidth()
    { return pen.width(); }

    void save( const TQString &filename, const TQString &format );

    void clearScreen();

protected:
    void mousePressEvent( TQMouseEvent *e );
    void mouseReleaseEvent( TQMouseEvent *e );
    void mouseMoveEvent( TQMouseEvent *e );
    void resizeEvent( TQResizeEvent *e );
    void paintEvent( TQPaintEvent *e );

    TQPen pen;
    TQPointArray polyline;

    bool mousePressed;

    TQPixmap buffer;

};

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
