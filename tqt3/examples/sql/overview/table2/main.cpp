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
#include <ntqdatatable.h>
#include "../connection.h"

int main( int argc, char *argv[] )
{
    TQApplication app( argc, argv );

    if ( createConnections() ) {
	TQSqlCursor staffCursor( "staff" );

	TQDataTable *staffTable = new TQDataTable( &staffCursor );

	app.setMainWidget( staffTable );

	staffTable->addColumn( "forename", "Forename" );
	staffTable->addColumn( "surname",  "Surname" );
	staffTable->addColumn( "salary",   "Annual Salary" );

	TQStringList order = TQStringList() << "surname" << "forename";
	staffTable->setSort( order );

	staffTable->refresh();
	staffTable->show();

	return app.exec();
    }

    return 1;
}
