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
	TQSqlCursor cur( "prices" ); 
	TQSqlRecord *buffer = cur.primeInsert();
	buffer->setValue( "id",	   53981 );
	buffer->setValue( "name",  "Thingy" );
	buffer->setValue( "price", 105.75 );
	cur.insert();
    }

    return 0;
}
