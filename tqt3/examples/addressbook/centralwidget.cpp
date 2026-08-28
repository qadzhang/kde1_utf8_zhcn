/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "centralwidget.h"

#include <ntqtabwidget.h>
#include <ntqlistview.h>
#include <ntqlayout.h>
#include <ntqwidget.h>
#include <ntqlabel.h>
#include <ntqpushbutton.h>
#include <ntqlineedit.h>
#include <ntqlabel.h>
#include <ntqcheckbox.h>
#include <ntqfile.h>
#include <ntqtextstream.h>

ABCentralWidget::ABCentralWidget( TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    mainGrid = new TQGridLayout( this, 2, 1, 5, 5 );

    setupTabWidget();
    setupListView();

    mainGrid->setRowStretch( 0, 0 );
    mainGrid->setRowStretch( 1, 1 );
}

void ABCentralWidget::save( const TQString &filename )
{
    if ( !listView->firstChild() )
        return;

    TQFile f( filename );
    if ( !f.open( IO_WriteOnly ) )
        return;

    TQTextStream t( &f );
    t.setEncoding(TQTextStream::UnicodeUTF8);

    TQListViewItemIterator it( listView );

    for ( ; it.current(); ++it )
        for ( unsigned int i = 0; i < 4; i++ )
            t << it.current()->text( i ) << "\n";

    f.close();
}

void ABCentralWidget::load( const TQString &filename )
{
    listView->clear();

    TQFile f( filename );
    if ( !f.open( IO_ReadOnly ) )
        return;

    TQTextStream t( &f );
    t.setEncoding(TQTextStream::UnicodeUTF8);

    while ( !t.atEnd() ) {
        TQListViewItem *item = new TQListViewItem( listView );
        for ( unsigned int i = 0; i < 4; i++ )
            item->setText( i, t.readLine() );
    }

    f.close();
}

void ABCentralWidget::setupTabWidget()
{
    tabWidget = new TQTabWidget( this );

    TQWidget *input = new TQWidget( tabWidget );
    TQGridLayout *grid1 = new TQGridLayout( input, 2, 5, 5, 5 );

    TQLabel *liFirstName = new TQLabel( "First &Name", input );
    liFirstName->resize( liFirstName->sizeHint() );
    grid1->addWidget( liFirstName, 0, 0 );

    TQLabel *liLastName = new TQLabel( "&Last Name", input );
    liLastName->resize( liLastName->sizeHint() );
    grid1->addWidget( liLastName, 0, 1 );

    TQLabel *liAddress = new TQLabel( "Add&ress", input );
    liAddress->resize( liAddress->sizeHint() );
    grid1->addWidget( liAddress, 0, 2 );

    TQLabel *liEMail = new TQLabel( "&E-Mail", input );
    liEMail->resize( liEMail->sizeHint() );
    grid1->addWidget( liEMail, 0, 3 );

    add = new TQPushButton( "A&dd", input );
    add->resize( add->sizeHint() );
    grid1->addWidget( add, 0, 4 );
    connect( add, TQ_SIGNAL( clicked() ), this, TQ_SLOT( addEntry() ) );

    iFirstName = new TQLineEdit( input );
    iFirstName->resize( iFirstName->sizeHint() );
    grid1->addWidget( iFirstName, 1, 0 );
    liFirstName->setBuddy( iFirstName );

    iLastName = new TQLineEdit( input );
    iLastName->resize( iLastName->sizeHint() );
    grid1->addWidget( iLastName, 1, 1 );
    liLastName->setBuddy( iLastName );

    iAddress = new TQLineEdit( input );
    iAddress->resize( iAddress->sizeHint() );
    grid1->addWidget( iAddress, 1, 2 );
    liAddress->setBuddy( iAddress );

    iEMail = new TQLineEdit( input );
    iEMail->resize( iEMail->sizeHint() );
    grid1->addWidget( iEMail, 1, 3 );
    liEMail->setBuddy( iEMail );

    change = new TQPushButton( "&Change", input );
    change->resize( change->sizeHint() );
    grid1->addWidget( change, 1, 4 );
    connect( change, TQ_SIGNAL( clicked() ), this, TQ_SLOT( changeEntry() ) );

    tabWidget->addTab( input, "&Add/Change Entry" );

    // --------------------------------------

    TQWidget *search = new TQWidget( this );
    TQGridLayout *grid2 = new TQGridLayout( search, 2, 5, 5, 5 );

    cFirstName = new TQCheckBox( "First &Name", search );
    cFirstName->resize( cFirstName->sizeHint() );
    grid2->addWidget( cFirstName, 0, 0 );
    connect( cFirstName, TQ_SIGNAL( clicked() ), this, TQ_SLOT( toggleFirstName() ) );

    cLastName = new TQCheckBox( "&Last Name", search );
    cLastName->resize( cLastName->sizeHint() );
    grid2->addWidget( cLastName, 0, 1 );
    connect( cLastName, TQ_SIGNAL( clicked() ), this, TQ_SLOT( toggleLastName() ) );

    cAddress = new TQCheckBox( "Add&ress", search );
    cAddress->resize( cAddress->sizeHint() );
    grid2->addWidget( cAddress, 0, 2 );
    connect( cAddress, TQ_SIGNAL( clicked() ), this, TQ_SLOT( toggleAddress() ) );

    cEMail = new TQCheckBox( "&E-Mail", search );
    cEMail->resize( cEMail->sizeHint() );
    grid2->addWidget( cEMail, 0, 3 );
    connect( cEMail, TQ_SIGNAL( clicked() ), this, TQ_SLOT( toggleEMail() ) );

    sFirstName = new TQLineEdit( search );
    sFirstName->resize( sFirstName->sizeHint() );
    grid2->addWidget( sFirstName, 1, 0 );

    sLastName = new TQLineEdit( search );
    sLastName->resize( sLastName->sizeHint() );
    grid2->addWidget( sLastName, 1, 1 );

    sAddress = new TQLineEdit( search );
    sAddress->resize( sAddress->sizeHint() );
    grid2->addWidget( sAddress, 1, 2 );

    sEMail = new TQLineEdit( search );
    sEMail->resize( sEMail->sizeHint() );
    grid2->addWidget( sEMail, 1, 3 );

    find = new TQPushButton( "F&ind", search );
    find->resize( find->sizeHint() );
    grid2->addWidget( find, 1, 4 );
    connect( find, TQ_SIGNAL( clicked() ), this, TQ_SLOT( findEntries() ) );

    cFirstName->setChecked( true );
    sFirstName->setEnabled( true );
    sLastName->setEnabled( false );
    sAddress->setEnabled( false );
    sEMail->setEnabled( false );

    tabWidget->addTab( search, "&Search" );

    mainGrid->addWidget( tabWidget, 0, 0 );
}

void ABCentralWidget::setupListView()
{
    listView = new TQListView( this );
    listView->addColumn( "First Name" );
    listView->addColumn( "Last Name" );
    listView->addColumn( "Address" );
    listView->addColumn( "E-Mail" );

    listView->setSelectionMode( TQListView::Single );

    connect( listView, TQ_SIGNAL( clicked( TQListViewItem* ) ), this, TQ_SLOT( itemSelected( TQListViewItem* ) ) );

    mainGrid->addWidget( listView, 1, 0 );
    listView->setAllColumnsShowFocus( true );
}

void ABCentralWidget::addEntry()
{
    if ( !iFirstName->text().isEmpty() || !iLastName->text().isEmpty() ||
         !iAddress->text().isEmpty() || !iEMail->text().isEmpty() ) {
        TQListViewItem *item = new TQListViewItem( listView );
        item->setText( 0, iFirstName->text() );
        item->setText( 1, iLastName->text() );
        item->setText( 2, iAddress->text() );
        item->setText( 3, iEMail->text() );
    }

    iFirstName->setText( "" );
    iLastName->setText( "" );
    iAddress->setText( "" );
    iEMail->setText( "" );
}

void ABCentralWidget::changeEntry()
{
    TQListViewItem *item = listView->currentItem();

    if ( item &&
         ( !iFirstName->text().isEmpty() || !iLastName->text().isEmpty() ||
           !iAddress->text().isEmpty() || !iEMail->text().isEmpty() ) ) {
        item->setText( 0, iFirstName->text() );
        item->setText( 1, iLastName->text() );
        item->setText( 2, iAddress->text() );
        item->setText( 3, iEMail->text() );
    }
}

void ABCentralWidget::selectionChanged()
{
    iFirstName->setText( "" );
    iLastName->setText( "" );
    iAddress->setText( "" );
    iEMail->setText( "" );
}

void ABCentralWidget::itemSelected( TQListViewItem *item )
{
    if ( !item )
	return;
    item->setSelected( true );
    item->repaint();

    iFirstName->setText( item->text( 0 ) );
    iLastName->setText( item->text( 1 ) );
    iAddress->setText( item->text( 2 ) );
    iEMail->setText( item->text( 3 ) );
}

void ABCentralWidget::toggleFirstName()
{
    sFirstName->setText( "" );

    if ( cFirstName->isChecked() ) {
        sFirstName->setEnabled( true );
        sFirstName->setFocus();
    }
    else
        sFirstName->setEnabled( false );
}

void ABCentralWidget::toggleLastName()
{
    sLastName->setText( "" );

    if ( cLastName->isChecked() ) {
        sLastName->setEnabled( true );
        sLastName->setFocus();
    }
    else
        sLastName->setEnabled( false );
}

void ABCentralWidget::toggleAddress()
{
    sAddress->setText( "" );

    if ( cAddress->isChecked() ) {
        sAddress->setEnabled( true );
        sAddress->setFocus();
    }
    else
        sAddress->setEnabled( false );
}

void ABCentralWidget::toggleEMail()
{
    sEMail->setText( "" );

    if ( cEMail->isChecked() ) {
        sEMail->setEnabled( true );
        sEMail->setFocus();
    }
    else
        sEMail->setEnabled( false );
}

void ABCentralWidget::findEntries()
{
    if ( !cFirstName->isChecked() &&
         !cLastName->isChecked() &&
         !cAddress->isChecked() &&
         !cEMail->isChecked() ) {
        listView->clearSelection();
        return;
    }

    TQListViewItemIterator it( listView );

    for ( ; it.current(); ++it ) {
        bool select = true;

        if ( cFirstName->isChecked() ) {
            if ( select && it.current()->text( 0 ).contains( sFirstName->text() ) )
                select = true;
            else
                select = false;
        }
        if ( cLastName->isChecked() ) {
            if ( select && it.current()->text( 1 ).contains( sLastName->text() ) )
                select = true;
            else
                select = false;
        }
        if ( cAddress->isChecked() ) {
            if ( select && it.current()->text( 2 ).contains( sAddress->text() ) )
                select = true;
            else
                select = false;
        }
        if ( cEMail->isChecked() ) {
            if ( select && it.current()->text( 3 ).contains( sEMail->text() ) )
                select = true;
            else
                select = false;
        }

        if ( select )
            it.current()->setSelected( true );
        else
            it.current()->setSelected( false );
        it.current()->repaint();
    }
}
