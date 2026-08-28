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
#include <ntqimage.h>
#include <ntqpixmap.h>
#include <ntqstringlist.h>

// TQt logo: static const char *qtlogo_xpm[]
#include "qtlogo.xpm"

// Table size

const int numRows = 30;
const int numCols = 10;

// The program starts here.

int main( int argc, char **argv )
{
    TQApplication app( argc, argv );

    TQTable table( numRows, numCols );

    TQHeader *header = table.horizontalHeader();
    header->setLabel( 0, TQObject::tr( "Tiny" ), 40 );
    header->setLabel( 1, TQObject::tr( "Checkboxes" ) );
    header->setLabel( 5, TQObject::tr( "Combos" ) );
    table.setColumnMovingEnabled(true);

    TQImage img( qtlogo_xpm );
    TQPixmap pix = img.scaleHeight( table.rowHeight(3) );
    table.setPixmap( 3, 2, pix );
    table.setText( 3, 2, "A Pixmap" );

    TQStringList comboEntries;
    comboEntries << "one" << "two" << "three" << "four";

    for ( int i = 0; i < numRows; ++i ){
	TQComboTableItem * item = new TQComboTableItem( &table, comboEntries, false );
	item->setCurrentItem( i % 4 );
	table.setItem( i, 5, item );
    }
    for ( int j = 0; j < numRows; ++j )
	table.setItem( j, 1, new TQCheckTableItem( &table, "Check me" ) );

    app.setMainWidget( &table );
    table.show();
    return app.exec();
}
