/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "checklists.h"
#include <ntqapplication.h>

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    CheckLists checklists;
    checklists.resize( 650, 350 );
    checklists.setCaption( "TQt Example - CheckLists" );
    a.setMainWidget( &checklists );
    checklists.show();

    return a.exec();
}
