/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "lifedlg.h"
#include <ntqapplication.h>
#include <stdlib.h>
 
void usage()
{
    tqWarning( "Usage: life [-scale scale]" );
}

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    int scale = 10;

    for ( int i = 1; i < argc; i++ ){
        TQString arg = argv[i];
	if ( arg == "-scale" )
	    scale = atoi( argv[++i] );
	else {
	    usage();
	    exit(1);
	}
    }

    if ( scale < 2 )
	scale = 2;

    LifeDialog *life = new LifeDialog( scale );
    a.setMainWidget( life );
    life->setCaption("TQt Example - Life");
    life->show();

    return a.exec();
}
