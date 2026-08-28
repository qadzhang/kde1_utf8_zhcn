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
#include <ntqsqlcursor.h>
#include <ntqfile.h>

#define DRIVER       "TQPSQL7" /* see the TQt SQL documentation for a list of available drivers */
#define DATABASE     "" /* the name of your database */
#define USER         "" /* user name with appropriate rights */
#define PASSWORD     ""	/* password for USER */
#define HOST         "" /* host on which the database is running */

int main( int argc, char ** argv )
{

    TQApplication a( argc, argv, false );
    TQSqlDatabase * db = TQSqlDatabase::addDatabase( DRIVER );
    db->setDatabaseName( DATABASE );
    db->setUserName( USER );
    db->setPassword( PASSWORD );
    db->setHostName( HOST );
    if ( !db->open() ) {
	tqWarning( db->lastError().databaseText() );
	return 1;
    }

    if ( argc < 2 ) {
	tqWarning( "Usage: %s <filename>", argv[0] );
	return 1;
    }
    
    // read a file which we want to insert into the database
    TQFile f( argv[1] );
    if ( !f.open( IO_ReadOnly ) ) {
	tqWarning( "Unable to open data file '%s' - exiting", argv[1] );
	return 1;
    }
    TQByteArray binaryData = f.readAll();
    tqWarning( "Data size: %d", binaryData.size() );
    
    // create a table with a binary field
    TQSqlQuery q;
    if ( !q.exec( "CREATE TABLE blobexample ( id INT PRIMARY KEY, binfield LONGBLOB )" ) ) {
	tqWarning( "Unable to create table - exiting" );
	return 1;
    }

    // insert a BLOB into the table
    if ( !q.prepare( "INSERT INTO blobexample ( id, binfield ) VALUES ( ?, ? )" ) ) {
	tqWarning( "Unable to prepare query - exiting" );
	return 1;
    }
    q.bindValue( 0, 1 );
    q.bindValue( 1, binaryData );
    if ( !q.exec() ) {
	tqWarning( "Unable to execute prepared query - exiting" );
	return 1;
    }

    // read the BLOB back from the database
    if ( !q.exec( "SELECT id, binfield FROM blobexample" ) ) {
	tqWarning( "Unable to execute query - exiting" );
	return 1;
    }
    tqWarning( "\nTQSqlQuery:" );
    while ( q.next() ) {
	tqWarning( "BLOB id: %d", q.value( 0 ).toInt() );
	tqWarning( "BLOB size: %d", q.value( 1 ).toByteArray().size() );
    }

    // write another BLOB using TQSqlCursor
    TQSqlCursor cur( "blobexample" );
    TQSqlRecord * r = cur.primeInsert();
    r->setValue( "id", 2 );
    r->setValue( "binfield", binaryData );
    if ( !cur.insert() ) {
	tqWarning( "Unable to insert BLOB using TQSqlCursor - exiting" );
	return 1;
    }

    // read the BLOBs back using TQSqlCursor
    if ( !cur.select() ) {
	tqWarning( "Unable retrieve blobexample table using TQSqlCursor - exiting" );
	return 1;
    }
    tqWarning( "\nTQSqlCursor:" );
    while ( cur.next() ) {
	tqWarning( "BLOB id: %d", cur.value( "id" ).toInt() );
	tqWarning( "BLOB size: %d", cur.value( "binfield" ).toByteArray().size() );
    }
        
    if ( !q.exec( "DROP TABLE blobexample" ) ) {
	tqWarning( "Unable to drop table - exiting" );
	return 1;
    }
    return 0;
}
