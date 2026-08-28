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
#include <ntqsqlquery.h>
#include "../connection.h"

int main( int argc, char *argv[] )
{
    TQApplication app( argc, argv, false );

    if ( createConnections() ) {
	TQSqlQuery query( "SELECT id, surname FROM staff" );
	if ( query.isActive() ) {
	    while ( query.next() ) {
		tqDebug( query.value(0).toString() + ": " +
		        query.value(1).toString() );
	    }
	}
    }

    return 0;
}
