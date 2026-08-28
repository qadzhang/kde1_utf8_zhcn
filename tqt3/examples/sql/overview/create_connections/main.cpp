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
#include "../connection.h"

int main( int argc, char *argv[] )
{
    TQApplication app( argc, argv, false );

    if ( createConnections() ) {
	// Databases successfully opened; get pointers to them:
	TQSqlDatabase *oracledb = TQSqlDatabase::database( "ORACLE" );
	// Now we can now issue SQL commands to the oracle connection
	// or to the default connection
    }

    return 0;
}
