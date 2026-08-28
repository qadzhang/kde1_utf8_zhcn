/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "main.h"
#include <ntqdatatable.h>

StatusPicker::StatusPicker( TQWidget *parent, const char *name )
    : TQComboBox( parent, name )
{
    TQSqlCursor cur( "status" );
    cur.select( cur.index( "name" ) );

    int i = 0;
    while ( cur.next() ) {
	insertItem( cur.value( "name" ).toString(), i );
	index2id[i] = cur.value( "id" ).toInt();
	i++;
    }
}


int StatusPicker::statusId() const
{
    return index2id[ currentItem() ];
}


void StatusPicker::setStatusId( int statusid )
{
    TQMap<int,int>::Iterator it;
    for ( it = index2id.begin(); it != index2id.end(); ++it ) {
	if ( it.data() == statusid ) {
	    setCurrentItem( it.key() );
	    break;
	}
    }
}



TQWidget *CustomSqlEditorFactory::createEditor(
    TQWidget *parent, const TQSqlField *field )
{
    if ( field->name() == "statusid" ) {
	TQWidget *editor = new StatusPicker( parent );
	return editor;
    }

    return TQSqlEditorFactory::createEditor( parent, field );
}


int main( int argc, char *argv[] )
{
    TQApplication app( argc, argv );

    if ( createConnections() ) {
	TQSqlCursor staffCursor( "staff" );

	TQDataTable		*staffTable	= new TQDataTable( &staffCursor );
	TQSqlPropertyMap		*propMap	= new TQSqlPropertyMap();
	CustomSqlEditorFactory	*editorFactory	= new CustomSqlEditorFactory();
	propMap->insert( "StatusPicker", "statusid" );
	staffTable->installPropertyMap( propMap );
	staffTable->installEditorFactory( editorFactory );

	app.setMainWidget( staffTable );

	staffTable->addColumn( "forename", "Forename" );
	staffTable->addColumn( "surname",  "Surname" );
	staffTable->addColumn( "salary",   "Annual Salary" );
	staffTable->addColumn( "statusid", "Status" );

	TQStringList order = TQStringList() << "surname" << "forename";
	staffTable->setSort( order );

	staffTable->refresh();
	staffTable->show();

	return app.exec();
    }

    return 1;
}
