/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "statistics.h"

#include <ntqdir.h>
#include <ntqstringlist.h>
#include <ntqheader.h>
#include <ntqcombobox.h>
#include <stdlib.h>

const char* dirs[] = {
    "kernel",
    "tools",
    "widgets",
    "dialogs",
    "xml",
    "table",
    "network",
    "opengl",
    "canvas",
    0
};

Table::Table()
    : TQTable( 10, 100, 0, "table" )
{
    setSorting( true );
    horizontalHeader()->setLabel( 0, tr( "File" ) );
    horizontalHeader()->setLabel( 1, tr( "Size (bytes)" ) );
    horizontalHeader()->setLabel( 2, tr( "Use in Sum" ) );
    initTable();
    adjustColumn( 0 );

    // if the user edited something we might need to recalculate the sum
    connect( this, TQ_SIGNAL( valueChanged( int, int ) ),
	     this, TQ_SLOT( recalcSum( int, int ) ) );
}

void Table::initTable()
{
    // read all the TQt source and header files into a list
    TQStringList all;
    int i = 0;
    TQString srcdir( "../../../src/" );
    while ( dirs[ i ] ) {
	TQDir dir( srcdir + dirs[ i ] );
	TQStringList lst = dir.entryList( "*.cpp; *.h" );
	for ( TQStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
	    if ( ( *it ).contains( "moc" ) )
		continue;
	    all << (TQString( dirs[ i ] ) + "/" + *it);
	}
	++i;
    }

    // set the number of rows we'll need for the table
    setNumRows( all.count() + 1 );
    i = 0;
    int sum = 0;

    // insert the data into the table
    for ( TQStringList::Iterator it = all.begin(); it != all.end(); ++it ) {
	setText( i, 0, *it );
	TQFile f( srcdir + *it );
	setText( i, 1, TQString::number( (ulong)f.size() ) );
	ComboItem *ci = new ComboItem( this, TQTableItem::WhenCurrent );
	setItem( i++, 2, ci );
	sum += f.size();
    }

    // last row should show the sum
    TableItem *i1 = new TableItem( this, TQTableItem::Never, tr( "Sum" ) );
    setItem( i, 0, i1 );
    TableItem *i2 = new TableItem( this, TQTableItem::Never, TQString::number( sum ) );
    setItem( i, 1, i2 );
}

void Table::recalcSum( int, int col )
{
    // only recalc if a value in the second or third column changed
    if ( col < 1 || col > 2 )
	return;

    // recalc sum
    int sum = 0;
    for ( int i = 0; i < numRows() - 1; ++i ) {
	if ( text( i, 2 ) == "No" )
	    continue;
	sum += text( i, 1 ).toInt();
    }

    // insert calculated data
    TableItem *i1 = new TableItem( this, TQTableItem::Never, tr( "Sum" ) );
    setItem( numRows() - 1, 0, i1 );
    TableItem *i2 = new TableItem( this, TQTableItem::Never, TQString::number( sum ) );
    setItem( numRows() - 1, 1, i2 );
}

void Table::sortColumn( int col, bool ascending, bool /*wholeRows*/ )
{
    // sum row should not be sorted, so get rid of it for now
    clearCell( numRows() - 1, 0 );
    clearCell( numRows() - 1, 1 );
    // do sort
    TQTable::sortColumn( col, ascending, true );
    // re-insert sum row
    recalcSum( 0, 1 );
}



void TableItem::paint( TQPainter *p, const TQColorGroup &cg, const TQRect &cr, bool selected )
{
    TQColorGroup g( cg );
    // last row is the sum row - we want to make it more visible by
    // using a red background
    if ( row() == table()->numRows() - 1 )
	g.setColor( TQColorGroup::Base, red );
    TQTableItem::paint( p, g, cr, selected );
}




ComboItem::ComboItem( TQTable *t, EditType et )
    : TQTableItem( t, et, "Yes" ), cb( 0 )
{
    // we do not want this item to be replaced
    setReplaceable( false );
}

TQWidget *ComboItem::createEditor() const
{
    // create an editor - a combobox in our case
    ( (ComboItem*)this )->cb = new TQComboBox( table()->viewport() );
    TQObject::connect( cb, TQ_SIGNAL( activated( int ) ), table(), TQ_SLOT( doValueChanged() ) );
    cb->insertItem( "Yes" );
    cb->insertItem( "No" );
    // and initialize it
    cb->setCurrentItem( text() == "No" ? 1 : 0 );
    return cb;
}

void ComboItem::setContentFromEditor( TQWidget *w )
{
    // the user changed the value of the combobox, so synchronize the
    // value of the item (its text), with the value of the combobox
    if ( w->inherits( "TQComboBox" ) )
	setText( ( (TQComboBox*)w )->currentText() );
    else
	TQTableItem::setContentFromEditor( w );
}

void ComboItem::setText( const TQString &s )
{
    if ( cb ) {
	// initialize the combobox from the text
	if ( s == "No" )
	    cb->setCurrentItem( 1 );
	else
	    cb->setCurrentItem( 0 );
    }
    TQTableItem::setText( s );
}
