/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "tabdialog.h"
#include <ntqapplication.h>
#include <ntqstring.h>

int main( int argc, char **argv )
{

    TQApplication a( argc, argv );

    TabDialog tabdialog( 0, "tabdialog", TQString( argc < 2 ? "." : argv[1] ) );
    tabdialog.resize( 450, 350 );
    tabdialog.setCaption( "TQt Example - Tabbed Dialog" );
    a.setMainWidget( &tabdialog );
    tabdialog.show();

    return a.exec();
}
