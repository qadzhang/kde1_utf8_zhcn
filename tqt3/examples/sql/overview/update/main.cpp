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
	cur.select( "id=202" );
	if ( cur.next() ) {
	    TQSqlRecord *buffer = cur.primeUpdate();
	    double price = buffer->value( "price" ).toDouble();
	    double newprice = price * 1.05;
	    buffer->setValue( "price", newprice );
	    cur.update();
	}
    }

    return 0;
}
