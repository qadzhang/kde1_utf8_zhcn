/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqsqldatabase.h>
#include <ntqsqlcursor.h>
#include "../connection.h"

int main( int argc, char *argv[] )
{
    TQApplication app( argc, argv, false );

    if ( createConnections() ) {
    	int count = 0;
	TQSqlCursor cur( "prices" ); 
	TQStringList names = TQStringList() <<
	    "Screwdriver" << "Hammer" << "Wrench" << "Saw";
	int id = 20;
	for ( TQStringList::Iterator name = names.begin();
	      name != names.end(); ++name ) {
	    TQSqlRecord *buffer = cur.primeInsert();
	    buffer->setValue( "id", id );
	    buffer->setValue( "name", *name );
	    buffer->setValue( "price", 100.0 + (double)id );
	    count += cur.insert();
	    id++;
	}
    }

    return 0;
}
