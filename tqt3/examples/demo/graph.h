/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqwidget.h>
class TQStyle;
class TQListBox;
class TQListBoxItem;
class TQWidgetStack;

class GraphWidgetPrivate;

class GraphWidget : public TQWidget 
{
    TQ_OBJECT
public:
    GraphWidget( TQWidget *parent=0, const char *name=0 );
    ~GraphWidget();

private slots:
    void shuffle();
    void setSpeed(int);
    
private:
    GraphWidgetPrivate* d;
};
