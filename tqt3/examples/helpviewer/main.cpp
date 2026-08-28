/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "helpwindow.h"
#include <ntqapplication.h>
#include <ntqdir.h>
#include <stdlib.h>


int main( int argc, char ** argv )
{
    TQApplication::setColorSpec( TQApplication::ManyColor );
    TQApplication a(argc, argv);

    TQString home;
    if (argc > 1) {
        home = argv[1];
    } else {
	// Use a hard coded path. It is only an example.
	home = TQDir( "../../doc/html/index.html" ).absPath();
    }

    HelpWindow *help = new HelpWindow(home, ".", 0, "help viewer");
    help->setCaption("TQt Example - Helpviewer");
    if ( TQApplication::desktop()->width() > 400
	 && TQApplication::desktop()->height() > 500 )
	help->show();
    else
	help->showMaximized();

    TQObject::connect( &a, TQ_SIGNAL(lastWindowClosed()),
                      &a, TQ_SLOT(quit()) );

    return a.exec();
}
