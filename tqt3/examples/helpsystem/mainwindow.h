/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqmainwindow.h>

class HeaderToolTip;
class TableToolTip;
class TQAssistantClient;

class MainWindow : public TQMainWindow
{
    TQ_OBJECT
public:
    MainWindow();
    ~MainWindow();

public slots:
    void assistantSlot();

private:
    HeaderToolTip *horizontalTip;
    HeaderToolTip *verticalTip;    
    TableToolTip *cellTip;
    TQAssistantClient *assistant;
};
