/****************************************************************************
**
** Definition of something or other
**
** Created : 979899
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef ROT13_H
#define ROT13_H

#include <ntqwidget.h>

class TQMultiLineEdit;

class Rot13: public TQWidget {
    TQ_OBJECT
public:
    Rot13();

    TQString rot13( const TQString & ) const;

private slots:
    void changeLeft();
    void changeRight();

private:
    TQMultiLineEdit * left, * right;
};

#endif
