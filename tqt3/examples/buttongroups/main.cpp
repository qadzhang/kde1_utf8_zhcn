/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "buttongroups.h"
#include <ntqapplication.h>

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    ButtonsGroups buttonsgroups;
    buttonsgroups.resize( 500, 250 );
    buttonsgroups.setCaption( "TQt Example - Buttongroups" );
    a.setMainWidget( &buttonsgroups );
    buttonsgroups.show();

    return a.exec();
}
