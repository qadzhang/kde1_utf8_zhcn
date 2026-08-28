/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef RANGECONTROLS_H
#define RANGECONTROLS_H

#include <ntqvbox.h>

class TQCheckBox;

class RangeControls : public TQVBox
{
    TQ_OBJECT

public:
    RangeControls( TQWidget *parent = 0, const char *name = 0 );

private:
    TQCheckBox *notches, *wrapping;
};

#endif
