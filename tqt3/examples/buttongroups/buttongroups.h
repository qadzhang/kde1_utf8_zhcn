/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef BUTTONS_GROUPS_H
#define BUTTONS_GROUPS_H

#include <ntqwidget.h>

class TQCheckBox;
class TQRadioButton;

class ButtonsGroups : public TQWidget
{
    TQ_OBJECT

public:
    ButtonsGroups( TQWidget *parent = 0, const char *name = 0 );

protected:
    TQCheckBox *state;
    TQRadioButton *rb21, *rb22, *rb23;

protected slots:    
    void slotChangeGrp3State();

};

#endif
