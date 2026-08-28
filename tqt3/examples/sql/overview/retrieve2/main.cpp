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
    TQApplication app( argc, argv );

    if ( createConnections() ) {
	TQSqlCursor cur( "staff" ); // Specify the table/view name
	cur.select(); // We'll retrieve every record
	while ( cur.next() ) {
	    tqDebug( cur.value( "id" ).toString() + ": " +
		    cur.value( "surname" ).toString() + " " +
		    cur.value( "salary" ).toString() );
	}
    }

    return 0;
}
