/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqpen.h>
#include <ntqpixmap.h>
#include <ntqpoint.h>
#include <ntqpointarray.h>
#include <ntqwidget.h>


#ifndef _MY_CANVAS_
#define _MY_CANVAS_


class Canvas : public TQWidget
{
    TQ_OBJECT

public:
    Canvas( TQWidget *parent = 0, const char *name = 0, WFlags fl = 0 );
    virtual ~Canvas() {};

    void setPenColor( const TQColor &c )
    { 	saveColor = c;
	pen.setColor( saveColor ); }

    void setPenWidth( int w )
    { pen.setWidth( w ); }

    TQColor penColor()
    { return pen.color(); }

    int penWidth()
    { return pen.width(); }

    void save( const TQString &filename, const TQString &format );

    void clearScreen();

protected:
    virtual void mousePressEvent( TQMouseEvent *e );
    virtual void mouseReleaseEvent( TQMouseEvent *e );
    virtual void mouseMoveEvent( TQMouseEvent *e );
    virtual void resizeEvent( TQResizeEvent *e );
    virtual void paintEvent( TQPaintEvent *e );
    virtual void tabletEvent( TQTabletEvent *e );

    TQPen pen;
    TQPointArray polyline;

    bool mousePressed;
    int oldPressure;
    TQColor saveColor;

    TQPixmap buffer;

};

#endif
