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
   	TQSqlCursor cur( "staff" );
	TQStringList fields = TQStringList() << "id" << "forename";
	TQSqlIndex order = cur.index( fields );
	TQSqlIndex filter = cur.index( "surname" );
	cur.setValue( "surname", "Bloggs" );
	cur.select( filter, order ); 
	while ( cur.next() ) {
	    tqDebug( cur.value( "id" ).toString() + ": " +
		    cur.value( "surname" ).toString() + " " +
		    cur.value( "forename" ).toString() );
        }
    }

    return 0;
}
