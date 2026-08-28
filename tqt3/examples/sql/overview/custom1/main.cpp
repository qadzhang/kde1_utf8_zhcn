/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "main.h"


CustomEdit::CustomEdit( TQWidget *parent, const char *name ) : 
    TQLineEdit( parent, name )
{
    connect( this, TQ_SIGNAL(textChanged(const TQString &)), 
	     this, TQ_SLOT(changed(const TQString &)) );
}


void CustomEdit::changed( const TQString &line )
{
    setUpperLine( line );
}


void CustomEdit::setUpperLine( const TQString &line )
{
    upperLineText = line.upper();
    setText( upperLineText );
}


TQString CustomEdit::upperLine() const 
{
    return upperLineText;
}


FormDialog::FormDialog()
{
    TQLabel	*forenameLabel	= new TQLabel( "Forename:", this );
    CustomEdit	*forenameEdit	= new CustomEdit( this );
    TQLabel	*surnameLabel   = new TQLabel( "Surname:", this );
    CustomEdit	*surnameEdit	= new CustomEdit( this );
    TQLabel	*salaryLabel	= new TQLabel( "Salary:", this );
    TQLineEdit	*salaryEdit	= new TQLineEdit( this );
    salaryEdit->setAlignment( TQt::AlignRight );
    TQPushButton *saveButton	= new TQPushButton( "&Save", this );
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

    staffCursor = new TQSqlCursor( "staff" );
    staffCursor->setTrimmed( "forename", true );
    staffCursor->setTrimmed( "surname",  true );
    idIndex = staffCursor->index( "id" );
    staffCursor->select( idIndex );
    staffCursor->first();

    propMap = new TQSqlPropertyMap;
    propMap->insert( forenameEdit->className(), "upperLine" );

    sqlForm = new TQSqlForm( this );
    sqlForm->setRecord( staffCursor->primeUpdate() );
    sqlForm->installPropertyMap( propMap );
    sqlForm->insert( forenameEdit, "forename" );
    sqlForm->insert( surnameEdit, "surname" );
    sqlForm->insert( salaryEdit, "salary" );
    sqlForm->readFields();
}


FormDialog::~FormDialog()
{
    delete staffCursor;
}


void FormDialog::save()
{
    sqlForm->writeFields();
    staffCursor->update();
    staffCursor->select( idIndex );
    staffCursor->first();
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
