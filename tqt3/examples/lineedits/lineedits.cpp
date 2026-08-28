/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "lineedits.h"

#include <ntqlineedit.h>
#include <ntqcombobox.h>
#include <ntqframe.h>
#include <ntqvalidator.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqhbox.h>

/*
 * Constructor
 *
 * Creates child widgets of the LineEdits widget
 */

LineEdits::LineEdits( TQWidget *parent, const char *name )
    : TQGroupBox( 0, Horizontal, "Line edits", parent, name )
{
    setMargin( 10 );

    TQVBoxLayout* box = new TQVBoxLayout( layout() );

    TQHBoxLayout *row1 = new TQHBoxLayout( box );
    row1->setMargin( 5 );

    // Create a Label
    TQLabel* label = new TQLabel( "Echo Mode: ", this);
    row1->addWidget( label );

    // Create a Combobox with three items...
    combo1 = new TQComboBox( false, this );
    row1->addWidget( combo1 );
    combo1->insertItem( "Normal" );
    combo1->insertItem( "Password" );
    combo1->insertItem( "No Echo" );
    // ...and connect the activated() TQ_SIGNAL with the slotEchoChanged() TQ_SLOT to be able
    // to react when an item is selected
    connect( combo1, TQ_SIGNAL( activated( int ) ), this, TQ_SLOT( slotEchoChanged( int ) ) );

    // insert the first LineEdit
    lined1 = new TQLineEdit( this );
    box->addWidget( lined1 );

    // another widget which is used for layouting
    TQHBoxLayout *row2 = new TQHBoxLayout( box );
    row2->setMargin( 5 );

    // and the second label
    label = new TQLabel( "Validator: ", this );
    row2->addWidget( label );

    // A second Combobox with again three items...
    combo2 = new TQComboBox( false, this );
    row2->addWidget( combo2 );
    combo2->insertItem( "No Validator" );
    combo2->insertItem( "Integer Validator" );
    combo2->insertItem( "Double Validator" );
    // ...and again the activated() TQ_SIGNAL gets connected with a TQ_SLOT
    connect( combo2, TQ_SIGNAL( activated( int ) ), this, TQ_SLOT( slotValidatorChanged( int ) ) );

    // and the second LineEdit
    lined2 = new TQLineEdit( this );
    box->addWidget( lined2 );

    // yet another widget which is used for layouting
    TQHBoxLayout *row3 = new TQHBoxLayout( box );
    row3->setMargin( 5 );

    // we need a label for this too
    label = new TQLabel( "Alignment: ", this );
    row3->addWidget( label );

    // A combo box for setting alignment
    combo3 = new TQComboBox( false, this );
    row3->addWidget( combo3 );
    combo3->insertItem( "Left" );
    combo3->insertItem( "Centered" );
    combo3->insertItem( "Right" );
    // ...and again the activated() TQ_SIGNAL gets connected with a TQ_SLOT
    connect( combo3, TQ_SIGNAL( activated( int ) ), this, TQ_SLOT( slotAlignmentChanged( int ) ) );

    // and the third lineedit
    lined3 = new TQLineEdit( this );
    box->addWidget( lined3 );

    // exactly the same for the fourth
    TQHBoxLayout *row4 = new TQHBoxLayout( box );
    row4->setMargin( 5 );

    // we need a label for this too
    label = new TQLabel( "Input mask: ", this );
    row4->addWidget( label );

    // A combo box for choosing an input mask
    combo4 = new TQComboBox( false, this );
    row4->addWidget( combo4 );
    combo4->insertItem( "No mask" );
    combo4->insertItem( "Phone number" );
    combo4->insertItem( "ISO date" );
    combo4->insertItem( "License key" );

    // ...this time we use the activated( const TQString & ) signal
    connect( combo4, TQ_SIGNAL( activated( int ) ),
	     this, TQ_SLOT( slotInputMaskChanged( int ) ) );

    // and the fourth lineedit
    lined4 = new TQLineEdit( this );
    box->addWidget( lined4 );

    // last widget used for layouting
    TQHBox *row5 = new TQHBox( this );
    box->addWidget( row5 );
    row5->setMargin( 5 );

    // last label
    (void)new TQLabel( "Read-Only: ", row5 );

    // A combo box for setting alignment
    combo5 = new TQComboBox( false, row5 );
    combo5->insertItem( "False" );
    combo5->insertItem( "True" );
    // ...and again the activated() TQ_SIGNAL gets connected with a TQ_SLOT
    connect( combo5, TQ_SIGNAL( activated( int ) ), this, TQ_SLOT( slotReadOnlyChanged( int ) ) );

    // and the last lineedit
    lined5 = new TQLineEdit( this );
    box->addWidget( lined5 );

    // give the first LineEdit the focus at the beginning
    lined1->setFocus();
}

/*
 * TQ_SLOT slotEchoChanged( int i )
 *
 * i contains the number of the item which the user has been chosen in the
 * first Combobox. According to this value, we set the Echo-Mode for the
 * first LineEdit.
 */

void LineEdits::slotEchoChanged( int i )
{
    switch ( i ) {
    case 0:
	lined1->setEchoMode( TQLineEdit::Normal );
        break;
    case 1:
	lined1->setEchoMode( TQLineEdit::Password );
        break;
    case 2:
	lined1->setEchoMode( TQLineEdit::NoEcho );
        break;
    }

    lined1->setFocus();
}

/*
 * TQ_SLOT slotValidatorChanged( int i )
 *
 * i contains the number of the item which the user has been chosen in the
 * second Combobox. According to this value, we set a validator for the
 * second LineEdit. A validator checks in a LineEdit each character which
 * the user enters and accepts it if it is valid, else the character gets
 * ignored and not inserted into the lineedit.
 */

void LineEdits::slotValidatorChanged( int i )
{
    switch ( i ) {
    case 0:
	lined2->setValidator( 0 );
        break;
    case 1:
	lined2->setValidator( new TQIntValidator( lined2 ) );
        break;
    case 2:
	lined2->setValidator( new TQDoubleValidator( -999.0, 999.0, 2,
						    lined2 ) );
        break;
    }

    lined2->setText( "" );
    lined2->setFocus();
}


/*
 * TQ_SLOT slotAlignmentChanged( int i )
 *
 * i contains the number of the item which the user has been chosen in
 * the third Combobox.  According to this value, we set an alignment
 * third LineEdit.
 */

void LineEdits::slotAlignmentChanged( int i )
{
    switch ( i ) {
    case 0:
	lined3->setAlignment( TQLineEdit::AlignLeft );
        break;
    case 1:
	lined3->setAlignment( TQLineEdit::AlignCenter );
        break;
    case 2:
	lined3->setAlignment( TQLineEdit::AlignRight );
        break;
    }

    lined3->setFocus();
}

/*
 * TQ_SLOT slotInputMaskChanged( const TQString &mask )
 *
 * i contains the number of the item which the user has been chosen in
 * the third Combobox.  According to this value, we set an input mask on
 * third LineEdit.
 */

void LineEdits::slotInputMaskChanged( int i )
{
    switch( i ) {
    case 0:
	lined4->setInputMask( TQString::null );
	break;
    case 1:
	lined4->setInputMask( "+99 99 99 99 99;_" );
	break;
    case 2:
	lined4->setInputMask( "0000-00-00" );
	lined4->setText( "00000000" );
	lined4->setCursorPosition( 0 );
	break;
    case 3:
	lined4->setInputMask( ">AAAAA-AAAAA-AAAAA-AAAAA-AAAAA;#" );
	break;
    }
    lined4->setFocus();
}

/*
 * TQ_SLOT slotReadOnlyChanged( int i )
 *
 * i contains the number of the item which the user has been chosen in
 * the fourth Combobox.  According to this value, we toggle read-only.
 */

void LineEdits::slotReadOnlyChanged( int i )
{
    switch ( i ) {
    case 0:
	lined5->setReadOnly( false );
        break;
    case 1:
	lined5->setReadOnly( true );
        break;
    }

    lined5->setFocus();
}

