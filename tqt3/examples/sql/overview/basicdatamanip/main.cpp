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

bool createConnections();


int main( int argc, char *argv[] )
{
    TQApplication app( argc, argv, false );

    int rows = 0;

    if ( createConnections() ) {
	TQSqlQuery query( "INSERT INTO staff ( id, forename, surname, salary ) "
		     "VALUES ( 1155, 'Ginger', 'Davis', 50000 )" );
	if ( query.isActive() ) rows += query.numRowsAffected() ;

	query.exec( "UPDATE staff SET salary=60000 WHERE id=1155" );
	if ( query.isActive() ) rows += query.numRowsAffected() ;

	query.exec( "DELETE FROM staff WHERE id=1155" ); 
	if ( query.isActive() ) rows += query.numRowsAffected() ;
    }

    return ( rows == 3 ) ? 0 : 1; 
}



