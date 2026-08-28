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
#include <ntqdatatable.h>
#include <ntqsqlcursor.h>
#include <ntqmessagebox.h>

/* Modify the following to match your environment */
#define DRIVER       "TQSQLITE"  /* see the TQt SQL documentation for a list of available drivers */
#define DATABASE     ":memory:" /* the name of your database */
#define USER         ""   /* user name with appropriate rights */
#define PASSWORD     ""   /* password for USER */
#define HOST         "" /* host on which the database is running */

class SimpleCursor : public TQSqlCursor
{
public:
    SimpleCursor () : TQSqlCursor( "simpletable" ) {}
protected:
    TQSqlRecord* primeInsert()
    {
	/* a real-world application would use sequences, or the like */
	TQSqlRecord* buf = TQSqlCursor::primeInsert();
	TQSqlQuery q( "select max(id)+1 from simpletable" );
	if ( q.next() )
	       buf->setValue( "id", q.value(0) );
	return buf;
    }
};

int main( int argc, char ** argv )
{
    TQApplication a( argc, argv );

    TQSqlDatabase * db = TQSqlDatabase::addDatabase( DRIVER );
    db->setDatabaseName( DATABASE );
    db->setUserName( USER );
    db->setPassword( PASSWORD );
    db->setHostName( HOST );

    if( !db->open() ){
	db->lastError().showMessage( "An error occured. Please read the README file in the sqltable"
				     "dir for more information.\n\n" );
	return 1;
    }

    if (!db->tables().contains("simpletable")) {
	TQSqlQuery q("create table simpletable(id int, name varchar(20), address varchar(20))", db);
    }

    SimpleCursor cursor;

    TQDataTable table( &cursor ); /* data table uses our cursor */
    table.addColumn( "name", "Name" );
    table.addColumn( "address", "Address" );
    table.setSorting( true );

    a.setMainWidget( &table );
    table.refresh(); /* load data */
    table.show();    /* show widget */

    return a.exec();
}
