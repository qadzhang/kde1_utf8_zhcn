/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqdialog.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqlineedit.h>
#include <ntqsqldatabase.h>
#include <ntqsqlcursor.h>
#include <ntqsqlform.h>
#include "../connection.h"

class FormDialog : public TQDialog
{
    public:
	FormDialog();
};


FormDialog::FormDialog()
{
    TQLabel *forenameLabel   = new TQLabel( "Forename:", this );
    TQLabel *forenameDisplay = new TQLabel( this );
    TQLabel *surnameLabel    = new TQLabel( "Surname:", this );
    TQLabel *surnameDisplay  = new TQLabel( this );
    TQLabel *salaryLabel	    = new TQLabel( "Salary:", this );
    TQLineEdit *salaryEdit   = new TQLineEdit( this );

    TQGridLayout *grid = new TQGridLayout( this );
    grid->addWidget( forenameLabel,	0, 0 );
    grid->addWidget( forenameDisplay,	0, 1 );
    grid->addWidget( surnameLabel,	1, 0 );
    grid->addWidget( surnameDisplay,	1, 1 );
    grid->addWidget( salaryLabel,	2, 0 );
    grid->addWidget( salaryEdit,	2, 1 );
    grid->activate();

    TQSqlCursor staffCursor( "staff" );
    staffCursor.select();
    staffCursor.next();

    TQSqlForm sqlForm( this );
    sqlForm.setRecord( staffCursor.primeUpdate() );
    sqlForm.insert( forenameDisplay, "forename" );
    sqlForm.insert( surnameDisplay, "surname" );
    sqlForm.insert( salaryEdit, "salary" );
    sqlForm.readFields();
}


int main( int argc, char *argv[] )
{
    TQApplication app( argc, argv );

    if ( ! createConnections() ) return 1;

    FormDialog *formDialog = new FormDialog();
    formDialog->show();
    app.setMainWidget( formDialog );

    return app.exec();
}
