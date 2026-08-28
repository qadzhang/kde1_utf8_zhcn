/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "listboxcombo.h"

#include <ntqcombobox.h>
#include <ntqlistbox.h>
#include <ntqhbox.h>
#include <ntqpushbutton.h>
#include <ntqstring.h>
#include <ntqpixmap.h>
#include <ntqlabel.h>
#include <ntqimage.h>
#include <ntqpainter.h>
#include <ntqstyle.h>


class MyListBoxItem : public TQListBoxItem
{
public:
    MyListBoxItem()
	: TQListBoxItem()
    {
	setCustomHighlighting( true );
    }

protected:
    virtual void paint( TQPainter * );
    virtual int width( const TQListBox* ) const { return 100; }
    virtual int height( const TQListBox* ) const { return 16; }

};

void MyListBoxItem::paint( TQPainter *painter )
{
    // evil trick: find out whether we are painted onto our listbox
    bool in_list_box = listBox() && listBox()->viewport() == painter->device();

    TQRect r ( 0, 0, width( listBox() ), height( listBox() ) );
    if ( in_list_box && isSelected() )
	painter->eraseRect( r );
    painter->fillRect( 5, 5, width( listBox() ) - 10, height( listBox() ) - 10, TQt::red );
    if ( in_list_box && isCurrent() )
	listBox()->style().drawPrimitive( TQStyle::PE_FocusRect, painter, r, listBox()->colorGroup() );
}

/*
 * Constructor
 *
 * Creates child widgets of the ListBoxCombo widget
 */

ListBoxCombo::ListBoxCombo( TQWidget *parent, const char *name )
    : TQVBox( parent, name )
{
    setMargin( 5 );
    setSpacing( 5 );

    unsigned int i;
    TQString str;

    TQHBox *row1 = new TQHBox( this );
    row1->setSpacing( 5 );

    // Create a multi-selection ListBox...
    lb1 = new TQListBox( row1 );
    lb1->setSelectionMode( TQListBox::Multi );

    // ...insert a pixmap item...
    lb1->insertItem( TQPixmap( "qtlogo.png" ) );
    // ...and 100 text items
    for ( i = 0; i < 100; i++ ) {
	str = TQString( "Listbox Item %1" ).arg( i );
	if ( !( i % 4 ) )
	    lb1->insertItem( TQPixmap( "fileopen.xpm" ), str );
	else
	    lb1->insertItem( str );
    }

    // Create a pushbutton...
    TQPushButton *arrow1 = new TQPushButton( " -> ", row1 );
    // ...and connect the clicked TQ_SIGNAL with the TQ_SLOT slotLeft2Right
    connect( arrow1, TQ_SIGNAL( clicked() ), this, TQ_SLOT( slotLeft2Right() ) );

    // create an empty single-selection ListBox
    lb2 = new TQListBox( row1 );

    TQHBox *row2 = new TQHBox( this );
    row2->setSpacing( 5 );

    TQVBox *box1 = new TQVBox( row2 );
    box1->setSpacing( 5 );

    // Create a non-editable Combobox and a label below...
    TQComboBox *cb1 = new TQComboBox( false, box1 );
    label1 = new TQLabel( "Current Item: Combobox Item 0", box1 );
    label1->setMaximumHeight( label1->sizeHint().height() * 2 );
    label1->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );

    //...and insert 50 items into the Combobox
    for ( i = 0; i < 50; i++ ) {
	str = TQString( "Combobox Item %1" ).arg( i );
	if ( i % 9 )
	    cb1->insertItem( str );
	else
	    cb1->listBox()->insertItem( new MyListBoxItem );
    }

    TQVBox *box2 = new TQVBox( row2 );
    box2->setSpacing( 5 );

    // Create an editable Combobox and a label below...
    TQComboBox *cb2 = new TQComboBox( true, box2 );
    label2 = new TQLabel( "Current Item: Combobox Item 0", box2 );
    label2->setMaximumHeight( label2->sizeHint().height() * 2 );
    label2->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );

    // ... and insert 50 items into the Combobox
    for ( i = 0; i < 50; i++ ) {
	str = TQString( "Combobox Item %1" ).arg( i );
	if ( !( i % 4 ) )
	    cb2->insertItem( TQPixmap( "fileopen.xpm" ), str );
	else
	    cb2->insertItem( str );
    }

    // Connect the activated SIGNALs of the Comboboxes with SLOTs
    connect( cb1, TQ_SIGNAL( activated( const TQString & ) ), this, TQ_SLOT( slotCombo1Activated( const TQString & ) ) );
    connect( cb2, TQ_SIGNAL( activated( const TQString & ) ), this, TQ_SLOT( slotCombo2Activated( const TQString & ) ) );
}

/*
 * TQ_SLOT slotLeft2Right
 *
 * Copies all selected items of the first ListBox into the
 * second ListBox
 */

void ListBoxCombo::slotLeft2Right()
{
    // Go through all items of the first ListBox
    for ( unsigned int i = 0; i < lb1->count(); i++ ) {
	TQListBoxItem *item = lb1->item( i );
	// if the item is selected...
	if ( item->isSelected() ) {
	    // ...and it is a text item...
	    if ( item->pixmap() && !item->text().isEmpty() )
		lb2->insertItem( *item->pixmap(), item->text() );
	    else if ( !item->pixmap() )
		lb2->insertItem( item->text() );
	    else if ( item->text().isEmpty() )
		lb2->insertItem( *item->pixmap() );
	}
    }
}

/*
 * TQ_SLOT slotCombo1Activated( const TQString &s )
 *
 * Sets the text of the item which the user just selected
 * in the first Combobox (and is now the value of s) to
 * the first Label.
 */

void ListBoxCombo::slotCombo1Activated( const TQString &s )
{
    label1->setText( TQString( "Current Item: %1" ).arg( s ) );
}

/*
 * TQ_SLOT slotCombo2Activated( const TQString &s )
 *
 * Sets the text of the item which the user just selected
 * in the second Combobox (and is now the value of s) to
 * the second Label.
 */

void ListBoxCombo::slotCombo2Activated( const TQString &s )
{
    label2->setText( TQString( "Current Item: %1" ).arg( s ) );
}
