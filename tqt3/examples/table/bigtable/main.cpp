/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqtable.h>

// Table size

const int numRows = 1000000;
const int numCols = 1000000;

class MyTable : public TQTable
{
public:
    MyTable( int r, int c ) : TQTable( r, c ) {
	items.setAutoDelete( true );
	widgets.setAutoDelete( true );
	setCaption( tr( "A 1 Million x 1 Million Cell Table" ) );
	setLeftMargin( fontMetrics().width( "W999999W" ) );
    }

    void resizeData( int ) {}
    TQTableItem *item( int r, int c ) const { return items.find( indexOf( r, c ) ); }
    void setItem( int r, int c, TQTableItem *i ) { items.replace( indexOf( r, c ), i ); }
    void clearCell( int r, int c ) { items.remove( indexOf( r, c ) ); }
    void takeItem( TQTableItem *item )
    {
	items.setAutoDelete( false );
	items.remove( indexOf( item->row(), item->col() ) );
	items.setAutoDelete( true );
    }
    void insertWidget( int r, int c, TQWidget *w ) { widgets.replace( indexOf( r, c ), w );  }
    TQWidget *cellWidget( int r, int c ) const { return widgets.find( indexOf( r, c ) ); }
    void clearCellWidget( int r, int c )
    {
	TQWidget *w = widgets.take( indexOf( r, c ) );
	if ( w )
	    w->deleteLater();
    }

private:
    TQIntDict<TQTableItem> items;
    TQIntDict<TQWidget> widgets;

};

// The program starts here.

int main( int argc, char **argv )
{
    TQApplication app( argc, argv );

    MyTable table( numRows, numCols );
    app.setMainWidget( &table );
    table.show();
    return app.exec();
}
