/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef LISTBOX_H
#define LISTBOX_H


class TQSpinBox;
class TQListBox;
class TQButtonGroup;

#include <ntqwidget.h>


class ListBoxDemo: public TQWidget
{
    TQ_OBJECT
public:
    ListBoxDemo();
    ~ListBoxDemo();

private slots:
    void setNumRows();
    void setNumCols();
    void setRowsByHeight();
    void setColsByWidth();
    void setVariableWidth( bool );
    void setVariableHeight( bool );
    void setMultiSelection( bool );
    void sortAscending();
    void sortDescending();
    
private:
    TQListBox * l;
    TQSpinBox * columns;
    TQSpinBox * rows;
    TQButtonGroup * bg;
};


#endif
