/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "richtext.h"
#include <ntqapplication.h>

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    MyRichText richtext;
    richtext.resize( 450, 350 );
    richtext.setCaption( "TQt Example - Richtext" );
    a.setMainWidget( &richtext );
    richtext.show();

    return a.exec();
}
