/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqwindowsstyle.h>
#include "themes.h"

#include "metal.h"

int main( int argc, char ** argv )
{
    TQApplication::setColorSpec( TQApplication::ManyColor );
    TQApplication a( argc, argv );

    Themes themes;
    themes.setCaption( "TQt Example - Themes (TQStyle)" );
    themes.resize( 640, 400 );
    a.setMainWidget( &themes );
    themes.show();

    return a.exec();
}
