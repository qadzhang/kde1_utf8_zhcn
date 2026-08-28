/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "scribble.h"
#include "tabletstats.h"
#include <ntqapplication.h>
#include <ntqtabwidget.h>


int main( int argc, char **argv )
{
    TQApplication a( argc, argv );
    TQTabWidget tab;
    Scribble scribble(&tab, "scribble");
    TabletStats tabStats( &tab, "tablet stats" );
    
    scribble.setMinimumSize( 500, 350 );
    tabStats.setMinimumSize( 500, 350 );
    tab.addTab(&scribble, "Scribble" );
    tab.addTab(&tabStats, "Tablet Stats" );
    
    a.setMainWidget( &tab );
    if ( TQApplication::desktop()->width() > 550
	 && TQApplication::desktop()->height() > 366 )
	tab.show();
    else
	tab.showMaximized();
    return a.exec();
}
