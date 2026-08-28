/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "main.h"
#include <ntqdatatable.h>

InvoiceItemCursor::InvoiceItemCursor() :
    TQSqlCursor( "invoiceitem" )
{
    TQSqlFieldInfo productName( "productname", TQVariant::String );
    append( productName );
    setCalculated( productName.name(), true );
}


TQVariant InvoiceItemCursor::calculateField( const TQString & name )
{
    if ( name == "productname" ) {
	TQSqlQuery query( "SELECT name FROM prices WHERE id=" +
		     field( "pricesid" )->value().toString() );
	if ( query.next() )
	    return query.value( 0 );
    }

    return TQVariant( TQString::null );
}


int main( int argc, char *argv[] )
{
    TQApplication app( argc, argv );

    if ( createConnections() ) {
	InvoiceItemCursor invoiceItemCursor;

	TQDataTable *invoiceItemTable = new TQDataTable( &invoiceItemCursor );

	app.setMainWidget( invoiceItemTable );

	invoiceItemTable->addColumn( "productname", "Product" );
	invoiceItemTable->addColumn( "quantity",    "Quantity" );
	invoiceItemTable->addColumn( "paiddate",    "Paid" );

	invoiceItemTable->refresh();
	invoiceItemTable->show();

	return app.exec();
    }

    return 1;
}
