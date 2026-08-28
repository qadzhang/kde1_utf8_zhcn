/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef _TABLET_STATS_
#define _TABLET_STATS_

#include <ntqwidget.h>
#include <ntqframe.h>
#include "canvas.h"
#include "tabletstatsbase.h"

class TQLabel;

class MyOrientation : public TQFrame  
{
    TQ_OBJECT
public:
    MyOrientation( TQWidget *parent = 0, const char *name = 0 );
    virtual ~MyOrientation();

public slots:
    void newOrient( int tiltX, int tiltY );

};

class StatsCanvas : public Canvas
{
    TQ_OBJECT
public:
    StatsCanvas( TQWidget *parent = 0, const char* name = 0 );
    ~StatsCanvas();
signals:
    void signalNewPressure( int );
    void signalNewTilt( int, int );
    void signalNewDev( int );
    void signalNewLoc( int, int );

protected:
    void tabletEvent( TQTabletEvent *e );
    void mouseMoveEvent( TQMouseEvent *e );
    void paintEvent( TQPaintEvent *e );
    void mousePressEvent( TQMouseEvent *e );
    void mouseReleaseEvent( TQMouseEvent *e );

private:
    TQRect r;
};

class TabletStats : public TabletStatsBase
{
    TQ_OBJECT
public:
    TabletStats( TQWidget *parent, const char* name );
    ~TabletStats();

private slots:
    void slotTiltChanged( int newTiltX, int newTiltY );
    void slotDevChanged( int newDev );
    void slotLocationChanged( int newX, int newY );

protected:
};

#endif
