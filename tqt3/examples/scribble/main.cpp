/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "scribble.h"
#include <ntqapplication.h>


int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    Scribble scribble;

    scribble.resize( 500, 350 );
    scribble.setCaption("TQt Example - Scribble");
    a.setMainWidget( &scribble );
    if ( TQApplication::desktop()->width() > 550
	 && TQApplication::desktop()->height() > 366 )
	scribble.show();
    else
	scribble.showMaximized();
    return a.exec();
}
