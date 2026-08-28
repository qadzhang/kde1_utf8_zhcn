/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "wizard.h"

#include <ntqwidget.h>
#include <ntqhbox.h>
#include <ntqvbox.h>
#include <ntqlabel.h>
#include <ntqlineedit.h>
#include <ntqpushbutton.h>
#include <ntqvalidator.h>
#include <ntqapplication.h>

Wizard::Wizard( TQWidget *parent, const char *name )
    : TQWizard( parent, name, true )
{
    setupPage1();
    setupPage2();
    setupPage3();

    key->setFocus();
}

void Wizard::setupPage1()
{
    page1 = new TQHBox( this );
    page1->setSpacing(8);

    TQLabel *info = new TQLabel( page1 );
    info->setMargin( 11 );
    info->setPalette( yellow );
    info->setText( "Enter your personal\n"
                   "key here.\n\n"
                   "Your personal key\n"
                   "consists of 4 digits" );
    info->setMaximumWidth( info->sizeHint().width() );

    TQVBox *page = new TQVBox( page1 );

    TQHBox *row1 = new TQHBox( page );

    (void)new TQLabel( "Key:", row1 );

    key = new TQLineEdit( row1 );
    key->setMaxLength( 4 );
    key->setValidator( new TQIntValidator( 1000, 9999, key ) );

    connect( key, TQ_SIGNAL( textChanged( const TQString & ) ),
	     this, TQ_SLOT( keyChanged( const TQString & ) ) );

    addPage( page1, "Personal Key" );

    setNextEnabled( page1, false );
    setHelpEnabled( page1, false );
}

void Wizard::setupPage2()
{
    page2 = new TQHBox( this );
    page2->setSpacing(8);

    TQLabel *info = new TQLabel( page2 );
    info->setMargin( 11 );
    info->setPalette( yellow );
    info->setText( "\n"
                   "Enter your personal\n"
                   "data here.\n\n"
                   "The required fields are\n"
                   "First Name, Last Name \n"
                   "and E-Mail.\n" );
    info->setMaximumWidth( info->sizeHint().width() );

    TQVBox *page = new TQVBox( page2 );

    TQHBox *row1 = new TQHBox( page );
    TQHBox *row2 = new TQHBox( page );
    TQHBox *row3 = new TQHBox( page );
    TQHBox *row4 = new TQHBox( page );
    TQHBox *row5 = new TQHBox( page );

    TQLabel *label1 = new TQLabel( " First Name: ", row1 );
    label1->setAlignment( TQt::AlignVCenter );
    TQLabel *label2 = new TQLabel( " Last Name: ", row2 );
    label2->setAlignment( TQt::AlignVCenter );
    TQLabel *label3 = new TQLabel( " Address: ", row3 );
    label3->setAlignment( TQt::AlignVCenter );
    TQLabel *label4 = new TQLabel( " Phone Number: ", row4 );
    label4->setAlignment( TQt::AlignVCenter );
    TQLabel *label5 = new TQLabel( " E-Mail: ", row5 );
    label5->setAlignment( TQt::AlignVCenter );

    label1->setMinimumWidth( label4->sizeHint().width() );
    label2->setMinimumWidth( label4->sizeHint().width() );
    label3->setMinimumWidth( label4->sizeHint().width() );
    label4->setMinimumWidth( label4->sizeHint().width() );
    label5->setMinimumWidth( label4->sizeHint().width() );

    firstName = new TQLineEdit( row1 );
    lastName = new TQLineEdit( row2 );
    address = new TQLineEdit( row3 );
    phone = new TQLineEdit( row4 );
    email = new TQLineEdit( row5 );

    connect( firstName, TQ_SIGNAL( textChanged( const TQString & ) ),
	     this, TQ_SLOT( dataChanged( const TQString & ) ) );
    connect( lastName, TQ_SIGNAL( textChanged( const TQString & ) ),
	     this, TQ_SLOT( dataChanged( const TQString & ) ) );
    connect( email, TQ_SIGNAL( textChanged( const TQString & ) ),
	     this, TQ_SLOT( dataChanged( const TQString & ) ) );

    addPage( page2, "Personal Data" );

    setHelpEnabled( page2, false );
}

void Wizard::setupPage3()
{
    page3 = new TQHBox( this );
    page3->setSpacing(8);

    TQLabel *info = new TQLabel( page3 );
    info->setPalette( yellow );
    info->setText( "\n"
                   "Look here to see of\n"
                   "the data you entered\n"
                   "is correct. To confirm,\n"
                   "press the [Finish] button\n"
                   "else go back to correct\n"
                   "mistakes." );
    info->setMargin( 11 );
    info->setAlignment( AlignTop|AlignLeft );
    info->setMaximumWidth( info->sizeHint().width() );

    TQVBox *page = new TQVBox( page3 );

    TQHBox *row1 = new TQHBox( page );
    TQHBox *row2 = new TQHBox( page );
    TQHBox *row3 = new TQHBox( page );
    TQHBox *row4 = new TQHBox( page );
    TQHBox *row5 = new TQHBox( page );
    TQHBox *row6 = new TQHBox( page );

    TQLabel *label1 = new TQLabel( " Personal Key: ", row1 );
    label1->setAlignment( TQt::AlignVCenter );
    TQLabel *label2 = new TQLabel( " First Name: ", row2 );
    label2->setAlignment( TQt::AlignVCenter );
    TQLabel *label3 = new TQLabel( " Last Name: ", row3 );
    label3->setAlignment( TQt::AlignVCenter );
    TQLabel *label4 = new TQLabel( " Address: ", row4 );
    label4->setAlignment( TQt::AlignVCenter );
    TQLabel *label5 = new TQLabel( " Phone Number: ", row5 );
    label5->setAlignment( TQt::AlignVCenter );
    TQLabel *label6 = new TQLabel( " E-Mail: ", row6 );
    label6->setAlignment( TQt::AlignVCenter );

    label1->setMinimumWidth( label1->sizeHint().width() );
    label2->setMinimumWidth( label1->sizeHint().width() );
    label3->setMinimumWidth( label1->sizeHint().width() );
    label4->setMinimumWidth( label1->sizeHint().width() );
    label5->setMinimumWidth( label1->sizeHint().width() );
    label6->setMinimumWidth( label1->sizeHint().width() );

    lKey = new TQLabel( row1 );
    lFirstName = new TQLabel( row2 );
    lLastName = new TQLabel( row3 );
    lAddress = new TQLabel( row4 );
    lPhone = new TQLabel( row5 );
    lEmail = new TQLabel( row6 );

    addPage( page3, "Finish" );

    setFinishEnabled( page3, true );
    setHelpEnabled( page3, false );
}

void Wizard::showPage( TQWidget* page )
{
    if ( page == page1 ) {
    } else if ( page == page2 ) {
    } else if ( page == page3 ) {
        lKey->setText( key->text() );
        lFirstName->setText( firstName->text() );
        lLastName->setText( lastName->text() );
        lAddress->setText( address->text() );
        lPhone->setText( phone->text() );
        lEmail->setText( email->text() );
    }

    TQWizard::showPage(page);

    if ( page == page1 ) {
        keyChanged( key->text() );
        key->setFocus();
    } else if ( page == page2 ) {
        dataChanged( firstName->text() );
        firstName->setFocus();
    } else if ( page == page3 ) {
        finishButton()->setEnabled( true );
        finishButton()->setFocus();
    }
}

void Wizard::keyChanged( const TQString &text )
{
    TQString t = text;
    int p = 0;
    bool on = ( key->validator()->validate(t, p) == TQValidator::Acceptable );
    nextButton()->setEnabled( on );
}

void Wizard::dataChanged( const TQString & )
{
    if ( !firstName->text().isEmpty() &&
         !lastName->text().isEmpty() &&
         !email->text().isEmpty() )
        nextButton()->setEnabled( true );
    else
        nextButton()->setEnabled( false );
}
