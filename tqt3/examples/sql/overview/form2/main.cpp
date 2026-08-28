/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "main.h"

FormDialog::FormDialog()
    : staffCursor( "staff" )
{
    staffCursor.setTrimmed( "forename", true );
    staffCursor.setTrimmed( "surname",  true );

    TQLabel	*forenameLabel = new TQLabel( "Forename:", this );
    TQLineEdit	*forenameEdit  = new TQLineEdit( this );
    TQLabel	*surnameLabel  = new TQLabel( "Surname:", this );
    TQLineEdit	*surnameEdit   = new TQLineEdit( this );
    TQLabel	*salaryLabel   = new TQLabel( "Salary:", this );
    TQLineEdit	*salaryEdit    = new TQLineEdit( this );
    TQPushButton *saveButton    = new TQPushButton( "&Save", this );
    connect( saveButton, TQ_SIGNAL(clicked()), this, TQ_SLOT(save()) );

    TQGridLayout *grid = new TQGridLayout( this );
    grid->addWidget( forenameLabel, 0, 0 );
    grid->addWidget( forenameEdit,  0, 1 );
    grid->addWidget( surnameLabel,  1, 0 );
    grid->addWidget( surnameEdit,   1, 1 );
    grid->addWidget( salaryLabel,   2, 0 );
    grid->addWidget( salaryEdit,    2, 1 );
    grid->addWidget( saveButton,    3, 0 );
    grid->activate();

    idIndex = staffCursor.index( "id" );
    staffCursor.select( idIndex );
    staffCursor.first();

    sqlForm = new TQSqlForm( this );
    sqlForm->setRecord( staffCursor.primeUpdate() );
    sqlForm->insert( forenameEdit, "forename" );
    sqlForm->insert( surnameEdit, "surname" );
    sqlForm->insert( salaryEdit, "salary" );
    sqlForm->readFields();
}


FormDialog::~FormDialog()
{

}


void FormDialog::save()
{
    sqlForm->writeFields();
    staffCursor.update();
    staffCursor.select( idIndex );
    staffCursor.first();
}


int main( int argc, char *argv[] )
{
    TQApplication app( argc, argv );

    if ( ! createConnections() )
	return 1;

    FormDialog *formDialog = new FormDialog();
    formDialog->show();
    app.setMainWidget( formDialog );

    return app.exec();
}
