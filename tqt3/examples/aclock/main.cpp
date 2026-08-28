/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "aclock.h"
#include <ntqapplication.h>


int main( int argc, char **argv )
{
    TQApplication a( argc, argv );
    AnalogClock *clock = new AnalogClock;
    if ( argc == 2 && strcmp( argv[1], "-transparent" ) == 0 )
	clock->setAutoMask( true );
    clock->resize( 100, 100 );
    a.setMainWidget( clock );
    clock->setCaption("TQt Example - Analog Clock");
    clock->show();
    int result = a.exec();
    delete clock;
    return result;
}
