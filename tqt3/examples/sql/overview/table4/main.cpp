/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "main.h"


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


void CustomTable::paintField( TQPainter * p, const TQSqlField* field,
			      const TQRect & cr, bool b)
{
    if ( !field )
	return;
    if ( field->name() == "statusid" ) {
	TQSqlQuery query( "SELECT name FROM status WHERE id=" +
		     field->value().toString() );
	TQString text;
	if ( query.next() ) {
	    text = query.value( 0 ).toString();
	}
	p->drawText( 2,2, cr.width()-4, cr.height()-4, fieldAlignment( field ), text );
    }
    else {
	TQDataTable::paintField( p, field, cr, b) ;
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

	CustomTable		*staffTable	= new CustomTable( &staffCursor );
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
