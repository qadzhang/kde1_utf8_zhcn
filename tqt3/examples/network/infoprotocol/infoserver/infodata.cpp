/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "infodata.h"


// we hard code all nodes and data in InfoData class
InfoData::InfoData() :
    nodes( 17, true ), data( 17, true )
{
    nodes.setAutoDelete(true);
    data.setAutoDelete(true);
    TQStringList *item;

    nodes.insert( "/", item = new TQStringList( ) );
    (*item) << "D network";
    nodes.insert( "/network/", item = new TQStringList() );
    (*item) << "D workstations" << "D printers" << "D fax";
    nodes.insert( "/network/workstations/", item = new TQStringList() );
    (*item) << "D nibble" << "D douglas";
    nodes.insert( "/network/workstations/nibble/", item = new TQStringList() );
    (*item) << "F os" << "F cpu" << "F memory";
    nodes.insert( "/network/workstations/douglas/", item = new TQStringList() );
    (*item) << "F os" << "F cpu" << "F memory";
    nodes.insert( "/network/printers/", item = new TQStringList() );
    (*item) << "D overbitt" << "D kroksleiven";
    nodes.insert( "/network/printers/overbitt/", item = new TQStringList() );
    (*item) << "D jobs" << "F type";
    nodes.insert( "/network/printers/overbitt/jobs/", item = new TQStringList() );
    (*item) << "F job1" << "F job2";
    nodes.insert( "/network/printers/kroksleiven/", item = new TQStringList() );
    (*item) << "D jobs" << "F type";
    nodes.insert( "/network/printers/kroksleiven/jobs/", item = new TQStringList() );
    nodes.insert( "/network/fax/", item = new TQStringList() );
    (*item) << "F last_number";

    data.insert( "/network/workstations/nibble/os", new TQString( "Linux" ) );
    data.insert( "/network/workstations/nibble/cpu", new TQString( "AMD Athlon 1000" ) );
    data.insert( "/network/workstations/nibble/memory", new TQString( "256 MB" ) );
    data.insert( "/network/workstations/douglas/os", new TQString( "Windows 2000" ) );
    data.insert( "/network/workstations/douglas/cpu", new TQString( "2 x Intel Pentium III 800" ) );
    data.insert( "/network/workstations/douglas/memory", new TQString( "256 MB" ) );
    data.insert( "/network/printers/overbitt/type", new TQString( "Lexmark Optra S 1255 PS" ) );
    data.insert( "/network/printers/overbitt/jobs/job1", 
                 new TQString( "TQt manual\n" "A4 size\n" "3000 pages" ) );
    data.insert( "/network/printers/overbitt/jobs/job2", 
		 new TQString( "Monthly report\n" "Letter size\n" "24 pages\n" "8 copies" ) );
    data.insert( "/network/printers/kroksleiven/type", new TQString( "HP C LaserJet 4500-PS" ) );
    data.insert( "/network/fax/last_number", new TQString( "22 22 22 22" ) );
}

TQStringList InfoData::list( TQString path, bool *found ) const
{
    if ( !path.endsWith( "/" ) )
	path += "/";
    if ( !path.startsWith( "/" ) )
	path = "/" + path;
    TQStringList *list = nodes[ path ];
    if ( list ) {
	*found = true;
	return *list;
    } else {
	*found = false;
	TQStringList empty;
	return empty;
    }
}

TQString InfoData::get( TQString path, bool *found ) const
{
    if ( !path.startsWith( "/" ) )
	path = "/" + path;
    TQString *file = data[ path ];
    if ( file ) {
	*found = true;
	return *file;
    } else {
	*found = false;
	TQString empty;
	return empty;
    }
}
