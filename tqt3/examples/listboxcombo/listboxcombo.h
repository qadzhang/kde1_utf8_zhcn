/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef LISTBOX_COMBO_H
#define LISTBOX_COMBO_H

#include <ntqvbox.h>

class TQListBox;
class TQLabel;

class ListBoxCombo : public TQVBox
{
    TQ_OBJECT

public:
    ListBoxCombo( TQWidget *parent = 0, const char *name = 0 );

protected:
    TQListBox *lb1, *lb2;
    TQLabel *label1, *label2;

protected slots:
    void slotLeft2Right();
    void slotCombo1Activated( const TQString &s );
    void slotCombo2Activated( const TQString &s );

};

#endif
