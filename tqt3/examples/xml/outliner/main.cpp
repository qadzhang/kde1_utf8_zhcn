/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include "outlinetree.h"

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    OutlineTree outline( "todos.opml" );
    a.setMainWidget( &outline );
    outline.show();

    return a.exec();
}
