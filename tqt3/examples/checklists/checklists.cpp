/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "checklists.h"

#include <ntqlistview.h>
#include <ntqvbox.h>
#include <ntqlabel.h>
#include <ntqvaluelist.h>
#include <ntqstring.h>
#include <ntqpushbutton.h>
#include <ntqlayout.h>

/*
 * Constructor
 *
 * Create all child widgets of the CheckList Widget
 */

CheckLists::CheckLists( TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    TQHBoxLayout *lay = new TQHBoxLayout( this );
    lay->setMargin( 5 );

    // create a widget which layouts its childs in a column
    TQVBoxLayout *vbox1 = new TQVBoxLayout( lay );
    vbox1->setMargin( 5 );

    // First child: a Label
    vbox1->addWidget( new TQLabel( "Check some items!", this ) );

    // Second child: the ListView
    lv1 = new TQListView( this );
    vbox1->addWidget( lv1 );
    lv1->addColumn( "Items" );
    lv1->setRootIsDecorated( true );

    // create a list with 4 ListViewItems which will be parent items of other ListViewItems
    TQValueList<TQListViewItem *> parentList;

    parentList.append( new TQCheckListItem( lv1, "Parent Item 1", TQCheckListItem::CheckBoxController ) );
    parentList.append( new TQCheckListItem( lv1, "Parent Item 2", TQCheckListItem::CheckBoxController ) );
    parentList.append( new TQCheckListItem( lv1, "Parent Item 3", TQCheckListItem::CheckBoxController ) );
    parentList.append( new TQCheckListItem( lv1, "Parent Item 4", TQCheckListItem::CheckBoxController ) );

    TQListViewItem *item = 0;
    unsigned int num = 1;
    // go through the list of parent items...
    for ( TQValueList<TQListViewItem*>::Iterator it = parentList.begin(); it != parentList.end();
	  ( *it )->setOpen( true ), ++it, num++ ) {
	item = *it;
	// ...and create 5 checkable child ListViewItems for each parent item
	for ( unsigned int i = 1; i <= 5; i++ )
	    (void)new TQCheckListItem( item, TQString( "%1. Child of Parent %2" ).arg( i ).arg( num ), TQCheckListItem::CheckBox );
    }

    // Create another widget for layouting
    TQVBoxLayout *tmp = new TQVBoxLayout( lay );
    tmp->setMargin( 5 );

    // create a pushbutton
    TQPushButton *copy1 = new TQPushButton( "  ->  ", this );
    tmp->addWidget( copy1 );
    copy1->setMaximumWidth( copy1->sizeHint().width() );
    // connect the TQ_SIGNAL clicked() of the pushbutton with the TQ_SLOT copy1to2()
    connect( copy1, TQ_SIGNAL( clicked() ), this, TQ_SLOT( copy1to2() ) );

    // another widget for layouting
    TQVBoxLayout *vbox2 = new TQVBoxLayout( lay );
    vbox2->setMargin( 5 );

    // and another label
    vbox2->addWidget( new TQLabel( "Check one item!", this ) );

    // create the second listview
    lv2 = new TQListView( this );
    vbox2->addWidget( lv2 );
    lv2->addColumn( "Items" );
    lv2->setRootIsDecorated( true );

    // another widget needed for layouting only
    tmp = new TQVBoxLayout( lay );
    tmp->setMargin( 5 );

    // create another pushbutton...
    TQPushButton *copy2 = new TQPushButton( "  ->  ", this );
    lay->addWidget( copy2 );
    copy2->setMaximumWidth( copy2->sizeHint().width() );
    // ...and connect its clicked() TQ_SIGNAL to the copy2to3() TQ_SLOT
    connect( copy2, TQ_SIGNAL( clicked() ), this, TQ_SLOT( copy2to3() ) );

    tmp = new TQVBoxLayout( lay );
    tmp->setMargin( 5 );

    // and create a label which will be at the right of the window
    label = new TQLabel( "No Item yet...", this );
    tmp->addWidget( label );
}

/*
 * TQ_SLOT copy1to2()
 *
 * Copies all checked ListViewItems from the first ListView to
 * the second one, and inserts them as Radio-ListViewItem.
 */

void CheckLists::copy1to2()
{
    // create an iterator which operates on the first ListView
    TQListViewItemIterator it( lv1 );

    lv2->clear();

    // Insert first a controller Item into the second ListView. Always if Radio-ListViewItems
    // are inserted into a Listview, the parent item of these MUST be a controller Item!
    TQCheckListItem *item = new TQCheckListItem( lv2, "Controller", TQCheckListItem::Controller );
    item->setOpen( true );

    // iterate through the first ListView...
    for ( ; it.current(); ++it )
	// ...check state of childs, and...
	if ( it.current()->parent() )
	    // ...if the item is checked...
	    if ( ( (TQCheckListItem*)it.current() )->isOn() )
		// ...insert a Radio-ListViewItem with the same text into the second ListView
		(void)new TQCheckListItem( item, it.current()->text( 0 ), TQCheckListItem::RadioButton );

    if ( item->firstChild() )
	( ( TQCheckListItem* )item->firstChild() )->setOn( true );
}

/*
 * TQ_SLOT copy2to3()
 *
 * Copies the checked item of the second ListView into the
 * Label at the right.
 */

void CheckLists::copy2to3()
{
    // create an iterator which operates on the second ListView
    TQListViewItemIterator it( lv2 );

    label->setText( "No Item checked" );

    // iterate through the second ListView...
    for ( ; it.current(); ++it )
	// ...check state of childs, and...
	if ( it.current()->parent() )
	    // ...if the item is checked...
	    if ( ( (TQCheckListItem*)it.current() )->isOn() )
		// ...set the text of the item to the label
		label->setText( it.current()->text( 0 ) );
}

