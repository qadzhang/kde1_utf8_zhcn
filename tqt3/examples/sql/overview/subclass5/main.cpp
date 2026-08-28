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

    TQSqlFieldInfo productPrice( "price", TQVariant::Double );
    append( productPrice );
    setCalculated( productPrice.name(), true );

    TQSqlFieldInfo productCost( "cost", TQVariant::Double );
    append( productCost );
    setCalculated( productCost.name(), true );
}


TQVariant InvoiceItemCursor::calculateField( const TQString & name )
{

    if ( name == "productname" ) {
	TQSqlQuery query( "SELECT name FROM prices WHERE id=" +
		     field( "pricesid" )->value().toString() );
	if ( query.next() )
	    return query.value( 0 );
    }
    else if ( name == "price" ) {
	TQSqlQuery query( "SELECT price FROM prices WHERE id=" +
		     field( "pricesid" )->value().toString() );
	if ( query.next() )
	    return query.value( 0 );
    }
    else if ( name == "cost" ) {
	TQSqlQuery query( "SELECT price FROM prices WHERE id=" +
		     field( "pricesid" )->value().toString() );
	if ( query.next() )
	    return TQVariant( query.value( 0 ).toDouble() *
			     value( "quantity").toDouble() );
    }

    return TQVariant( TQString::null );
}


TQSqlRecord *InvoiceItemCursor::primeInsert()
{
    TQSqlRecord *buffer = editBuffer();
    TQSqlQuery query( "SELECT NEXTVAL( 'invoiceitem_seq' )" );
    if ( query.next() )
	buffer->setValue( "id", query.value( 0 ) );
    buffer->setValue( "paiddate", TQDate::currentDate() );
    buffer->setValue( "quantity", 1 );

    return buffer;
}


int main( int argc, char *argv[] )
{
    TQApplication app( argc, argv );

    if ( createConnections() ) {
	InvoiceItemCursor invoiceItemCursor;

	TQDataTable *invoiceItemTable = new TQDataTable( &invoiceItemCursor );

	app.setMainWidget( invoiceItemTable );

	invoiceItemTable->addColumn( "productname", "Product" );
	invoiceItemTable->addColumn( "price",	    "Price" );
	invoiceItemTable->addColumn( "quantity",    "Quantity" );
	invoiceItemTable->addColumn( "cost",	    "Cost" );
	invoiceItemTable->addColumn( "paiddate",    "Paid" );

	invoiceItemTable->refresh();
	invoiceItemTable->show();

	return app.exec();
    }

    return 1;
}
