/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "rangecontrols.h"
#include <ntqapplication.h>

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    RangeControls rangecontrols;
    rangecontrols.resize( 500, 300 );
    rangecontrols.setCaption( "TQt Example - Range Control Widgets" );
    a.setMainWidget( &rangecontrols );
    rangecontrols.show();

    return a.exec();
}
