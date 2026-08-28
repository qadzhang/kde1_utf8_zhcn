/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "buttongroups.h"

#include <ntqpopupmenu.h>
#include <ntqbuttongroup.h>
#include <ntqlayout.h>
#include <ntqradiobutton.h>
#include <ntqcheckbox.h>
#include <ntqgroupbox.h>
#include <ntqpushbutton.h>

/*
 * Constructor
 *
 * Creates all child widgets of the ButtonGroups window
 */

ButtonsGroups::ButtonsGroups( TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    // Create Widgets which allow easy layouting
    TQVBoxLayout *vbox = new TQVBoxLayout( this, 11, 6 );
    TQHBoxLayout *box1 = new TQHBoxLayout( vbox );
    TQHBoxLayout *box2 = new TQHBoxLayout( vbox );

    // ------- first group

    // Create an exclusive button group
    TQButtonGroup *bgrp1 = new TQButtonGroup( 1, TQGroupBox::Horizontal, "Button Group 1 (exclusive)", this);
    box1->addWidget( bgrp1 );
    bgrp1->setExclusive( true );

    // insert 3 radiobuttons
    TQRadioButton *rb11 = new TQRadioButton( "&Radiobutton 1", bgrp1 );
    rb11->setChecked( true );
    (void)new TQRadioButton( "R&adiobutton 2", bgrp1 );
    (void)new TQRadioButton( "Ra&diobutton 3", bgrp1 );

    // ------- second group

    // Create a non-exclusive buttongroup
    TQButtonGroup *bgrp2 = new TQButtonGroup( 1, TQGroupBox::Horizontal, "Button Group 2 (non-exclusive)", this );
    box1->addWidget( bgrp2 );
    bgrp2->setExclusive( false );

    // insert 3 checkboxes
    (void)new TQCheckBox( "&Checkbox 1", bgrp2 );
    TQCheckBox *cb12 = new TQCheckBox( "C&heckbox 2", bgrp2 );
    cb12->setChecked( true );
    TQCheckBox *cb13 = new TQCheckBox( "Triple &State Button", bgrp2 );
    cb13->setTristate( true );
    cb13->setChecked( true );

    // ------------ third group

    // create a buttongroup which is exclusive for radiobuttons and non-exclusive for all other buttons
    TQButtonGroup *bgrp3 = new TQButtonGroup( 1, TQGroupBox::Horizontal, "Button Group 3 (Radiobutton-exclusive)", this );
    box2->addWidget( bgrp3 );
    bgrp3->setRadioButtonExclusive( true );

    // insert three radiobuttons
    rb21 = new TQRadioButton( "Rad&iobutton 1", bgrp3 );
    rb22 = new TQRadioButton( "Radi&obutton 2", bgrp3 );
    rb23 = new TQRadioButton( "Radio&button 3", bgrp3 );
    rb23->setChecked( true );

    // insert a checkbox...
    state = new TQCheckBox( "E&nable Radiobuttons", bgrp3 );
    state->setChecked( true );
    // ...and connect its TQ_SIGNAL clicked() with the TQ_SLOT slotChangeGrp3State()
    connect( state, TQ_SIGNAL( clicked() ), this, TQ_SLOT( slotChangeGrp3State() ) );

    // ------------ fourth group

    // create a groupbox which layouts its childs in a columns
    TQGroupBox *bgrp4 = new TQButtonGroup( 1, TQGroupBox::Horizontal, "Groupbox with normal buttons", this );
    box2->addWidget( bgrp4 );

    // insert four pushbuttons...
    (void)new TQPushButton( "&Push Button", bgrp4, "push" );

    // now make the second one a toggle button
    TQPushButton *tb2 = new TQPushButton( "&Toggle Button", bgrp4, "toggle" );
    tb2->setToggleButton( true );
    tb2->setOn( true );

    // ... and make the third one a flat button
    TQPushButton *tb3 = new TQPushButton( "&Flat Button", bgrp4, "flat" );
    tb3->setFlat(true);

    // .. and the fourth a button with a menu
    TQPushButton *tb4 = new TQPushButton( "Popup Button", bgrp4, "popup" );
    TQPopupMenu *menu = new TQPopupMenu(tb4);
    menu->insertItem("Item1", 0);
    menu->insertItem("Item2", 1);
    menu->insertItem("Item3", 2);
    menu->insertItem("Item4", 3);
    tb4->setPopup(menu);
}

/*
 * TQ_SLOT slotChangeGrp3State()
 *
 * enables/disables the radiobuttons of the third buttongroup
 */

void ButtonsGroups::slotChangeGrp3State()
{
    rb21->setEnabled( state->isChecked() );
    rb22->setEnabled( state->isChecked() );
    rb23->setEnabled( state->isChecked() );
}
