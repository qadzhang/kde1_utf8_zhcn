/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include "qwerty.h"


int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    
    bool isSmall =  tqApp->desktop()->size().width() < 450 
		  || tqApp->desktop()->size().height() < 450;
    
    int i;
    for ( i= argc <= 1 ? 0 : 1; i<argc; i++ ) {
	Editor *e = new Editor;
	e->setCaption("TQt Example - TQWERTY");
	if ( i > 0 )
	    e->load( argv[i] );
	if ( isSmall ) {
	    e->showMaximized();
	} else {
	    e->resize( 400, 400 );
	    e->show();
	}
    }
    a.connect( &a, TQ_SIGNAL(lastWindowClosed()), &a, TQ_SLOT(quit()) );
    return a.exec();
}
