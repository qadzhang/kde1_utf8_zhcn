/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include "tooltip.h"

int main( int argc, char ** argv ) 
{
    TQApplication a( argc, argv );

    TellMe mw;
    mw.setCaption( "TQt Example - Dynamic Tool Tips" );
    a.setMainWidget( &mw );
    mw.show();
    
    return a.exec();
}
