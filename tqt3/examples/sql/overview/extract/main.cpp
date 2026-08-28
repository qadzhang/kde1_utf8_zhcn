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
	TQSqlCursor cur( "creditors" );

	TQStringList orderFields = TQStringList() << "surname" << "forename";
	TQSqlIndex order = cur.index( orderFields );

	TQStringList filterFields = TQStringList() << "surname" << "city";
	TQSqlIndex filter = cur.index( filterFields );
	cur.setValue( "surname", "Chirac" );
	cur.setValue( "city", "Paris" );

	cur.select( filter, order );

	while ( cur.next() ) {
	    int id = cur.value( "id" ).toInt();
	    TQString name = cur.value( "forename" ).toString() + " " +
			   cur.value( "surname" ).toString();
	    tqDebug( TQString::number( id ) + ": " + name );
	}
    }

    return 0;
}
