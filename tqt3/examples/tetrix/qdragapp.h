/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TQDRAGAPP_H
#define TQDRAGAPP_H

#include "ntqapplication.h"

class TQDragger;

class TQDragApplication : public TQApplication
{
    TQ_OBJECT
public:
    TQDragApplication( int &argc, char **argv );
    virtual ~TQDragApplication();

    virtual bool notify( TQObject *, TQEvent * ); // event filter

private:
    TQDragger *dragger;
};


#endif // TQDRAGAPP_H
