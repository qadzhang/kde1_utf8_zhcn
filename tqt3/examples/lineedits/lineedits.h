/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef LINEDITS_H
#define LINEDITS_H

#include <ntqgroupbox.h>

class TQLineEdit;
class TQComboBox;

class LineEdits : public TQGroupBox
{
    TQ_OBJECT

public:
    LineEdits( TQWidget *parent = 0, const char *name = 0 );

protected:
    TQLineEdit *lined1, *lined2, *lined3, *lined4, *lined5;
    TQComboBox *combo1, *combo2, *combo3, *combo4, *combo5;

protected slots:
    void slotEchoChanged( int );
    void slotValidatorChanged( int );
    void slotAlignmentChanged( int );
    void slotInputMaskChanged( int );
    void slotReadOnlyChanged( int );
};

#endif
