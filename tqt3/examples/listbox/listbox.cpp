/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "listbox.h"

#include <ntqlabel.h>
#include <ntqradiobutton.h>
#include <ntqcheckbox.h>
#include <ntqspinbox.h>
#include <ntqlistbox.h>
#include <ntqbuttongroup.h>
#include <ntqlayout.h>
#include <ntqpushbutton.h>


ListBoxDemo::ListBoxDemo()
    : TQWidget( 0, 0 )
{
    TQGridLayout * g = new TQGridLayout( this, 2, 2, 6 );

    g->addWidget( new TQLabel( "<b>Configuration:</b>", this ), 0, 0 );
    g->addWidget( new TQLabel( "<b>Result:</b>", this ), 0, 1 );

    l = new TQListBox( this );
    g->addWidget( l, 1, 1 );
    l->setFocusPolicy( TQWidget::StrongFocus );

    TQVBoxLayout * v = new TQVBoxLayout;
    g->addLayout( v, 1, 0 );

    TQRadioButton * b;
    bg = new TQButtonGroup( 0 );

    b = new TQRadioButton( "Fixed number of columns,\n"
                          "as many rows as needed.",
                          this );
    bg->insert( b );
    v->addWidget( b );
    b->setChecked( true );
    connect( b, TQ_SIGNAL(clicked()), this, TQ_SLOT(setNumCols()) );
    TQHBoxLayout * h = new TQHBoxLayout;
    v->addLayout( h );
    h->addSpacing( 30 );
    h->addSpacing( 100 );
    h->addWidget( new TQLabel( "Columns:", this ) );
    columns = new TQSpinBox( this );
    h->addWidget( columns );

    v->addSpacing( 12 );

    b = new TQRadioButton( "As many columns as fit on-screen,\n"
                          "as many rows as needed.",
                          this );
    bg->insert( b );
    v->addWidget( b );
    connect( b, TQ_SIGNAL(clicked()), this, TQ_SLOT(setColsByWidth()) );

    v->addSpacing( 12 );

    b = new TQRadioButton( "Fixed number of rows,\n"
                          "as many columns as needed.",
                          this );
    bg->insert( b );
    v->addWidget( b );
    connect( b, TQ_SIGNAL(clicked()), this, TQ_SLOT(setNumRows()) );
    h = new TQHBoxLayout;
    v->addLayout( h );
    h->addSpacing( 30 );
    h->addSpacing( 100 );
    h->addWidget( new TQLabel( "Rows:", this ) );
    rows = new TQSpinBox( this );
    rows->setEnabled( false );
    h->addWidget( rows );

    v->addSpacing( 12 );

    b = new TQRadioButton( "As many rows as fit on-screen,\n"
                          "as many columns as needed.",
                          this );
    bg->insert( b );
    v->addWidget( b );
    connect( b, TQ_SIGNAL(clicked()), this, TQ_SLOT(setRowsByHeight()) );

    v->addSpacing( 12 );

    TQCheckBox * cb = new TQCheckBox( "Variable-height rows", this );
    cb->setChecked( true );
    connect( cb, TQ_SIGNAL(toggled(bool)), this, TQ_SLOT(setVariableHeight(bool)) );
    v->addWidget( cb );
    v->addSpacing( 6 );

    cb = new TQCheckBox( "Variable-width columns", this );
    connect( cb, TQ_SIGNAL(toggled(bool)), this, TQ_SLOT(setVariableWidth(bool)) );
    v->addWidget( cb );

    cb = new TQCheckBox( "Extended-Selection", this );
    connect( cb, TQ_SIGNAL(toggled(bool)), this, TQ_SLOT(setMultiSelection(bool)) );
    v->addWidget( cb );

    TQPushButton *pb = new TQPushButton( "Sort ascending", this );
    connect( pb, TQ_SIGNAL( clicked() ), this, TQ_SLOT( sortAscending() ) );
    v->addWidget( pb );

    pb = new TQPushButton( "Sort descending", this );
    connect( pb, TQ_SIGNAL( clicked() ), this, TQ_SLOT( sortDescending() ) );
    v->addWidget( pb );

    v->addStretch( 100 );

    int i = 0;
    while( ++i <= 2560 )
        l->insertItem( TQString::fromLatin1( "Item " ) + TQString::number( i ),
                       i );
    columns->setRange( 1, 256 );
    columns->setValue( 1 );
    rows->setRange( 1, 256 );
    rows->setValue( 256 );

    connect( columns, TQ_SIGNAL(valueChanged(int)), this, TQ_SLOT(setNumCols()) );
    connect( rows, TQ_SIGNAL(valueChanged(int)), this, TQ_SLOT(setNumRows()) );
}


ListBoxDemo::~ListBoxDemo()
{
    delete bg;
}


void ListBoxDemo::setNumRows()
{
    columns->setEnabled( false );
    rows->setEnabled( true );
    l->setRowMode( rows->value() );
}


void ListBoxDemo::setNumCols()
{
    columns->setEnabled( true );
    rows->setEnabled( false );
    l->setColumnMode( columns->value() );
}


void ListBoxDemo::setRowsByHeight()
{
    columns->setEnabled( false );
    rows->setEnabled( false );
    l->setRowMode( TQListBox::FitToHeight );
}


void ListBoxDemo::setColsByWidth()
{
    columns->setEnabled( false );
    rows->setEnabled( false );
    l->setColumnMode( TQListBox::FitToWidth );
}


void ListBoxDemo::setVariableWidth( bool b )
{
    l->setVariableWidth( b );
}


void ListBoxDemo::setVariableHeight( bool b )
{
    l->setVariableHeight( b );
}

void ListBoxDemo::setMultiSelection( bool b )
{
    l->clearSelection();
    l->setSelectionMode( b ? TQListBox::Extended : TQListBox::Single );
}

void ListBoxDemo::sortAscending()
{
    l->sort( true );
}

void ListBoxDemo::sortDescending()
{
    l->sort( false );
}
