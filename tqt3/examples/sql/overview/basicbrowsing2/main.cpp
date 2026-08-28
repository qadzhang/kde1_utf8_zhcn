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
	TQSqlDatabase *oracledb = TQSqlDatabase::database( "ORACLE" );
	// Copy data from the oracle database to the ODBC (default)
	// database
	TQSqlQuery target;
	TQSqlQuery query( "SELECT id, name FROM people", oracledb );
	int count = 0;
        if ( query.isActive() ) {
            while ( query.next() ) {
                target.exec( "INSERT INTO people ( id, name ) VALUES ( " + 
                              query.value(0).toString() + 
                              ", '" + query.value(1).toString() +  "' )" );
                if ( target.isActive() ) 
                    count += target.numRowsAffected();
            }
        }
    }

    return 0;
}





