/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqwidget.h>
#include <ntqtooltip.h>


class DynamicTip : public TQToolTip
{
public:
    DynamicTip( TQWidget * parent );

protected:
    void maybeTip( const TQPoint & );
};


class TellMe : public TQWidget
{
    TQ_OBJECT
public:
    TellMe( TQWidget * parent = 0, const char * name = 0 );
    ~TellMe();

    TQRect tip( const TQPoint & );

protected:
    void paintEvent( TQPaintEvent * );
    void mousePressEvent( TQMouseEvent * );
    void resizeEvent( TQResizeEvent * );

private:
    TQRect randomRect();

    TQRect r1, r2, r3;
    DynamicTip * t;
};
