/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "listviews.h"
#include <ntqapplication.h>

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    ListViews listViews;
    listViews.resize( 640, 480 );
    listViews.setCaption( "TQt Example - Listview" );
    a.setMainWidget( &listViews );
    listViews.show();

    return a.exec();
}
