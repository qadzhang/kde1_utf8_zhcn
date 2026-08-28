/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>

#include "composer.h"

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    Composer c;
    a.setMainWidget( &c );
    c.resize( 400, 500 );
    c.show();
    return a.exec();
}
