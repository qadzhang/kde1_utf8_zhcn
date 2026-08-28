/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqfileinfo.h>
#include <ntqdir.h>
#include "dirview.h"

int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );

    DirectoryView mw;

    mw.addColumn( "Name" );
    mw.addColumn( "Type" );
    mw.setTreeStepSize( 20 );

    const TQFileInfoList* roots = TQDir::drives();
    TQPtrListIterator<TQFileInfo> i(*roots);
    TQFileInfo* fi;
    while ( (fi = *i) ) {
	++i;
	Directory * root = new Directory( &mw, fi->filePath() );
	if ( roots->count() <= 1 )
	    root->setOpen( true ); // be interesting
    }

    mw.resize( 400, 400 );
    mw.setCaption( "TQt Example - Directory Browser" );
    mw.setAllColumnsShowFocus( true );
    a.setMainWidget( &mw );
    mw.show();

    return a.exec();
}
