/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "structureparser.h"
#include <ntqapplication.h>
#include <ntqfile.h>
#include <ntqxml.h>
#include <ntqlistview.h>
#include <ntqgrid.h>
#include <ntqmainwindow.h>
#include <ntqlabel.h>

int main( int argc, char **argv )
{
    TQApplication app( argc, argv );

    TQFile xmlFile( argc == 2 ? argv[1] : "fnord.xml" );
    TQXmlInputSource source( &xmlFile );

    TQXmlSimpleReader reader;

    TQGrid * container = new TQGrid( 3 );

    TQListView * nameSpace = new TQListView( container, "table_namespace" );
    StructureParser * handler = new StructureParser( nameSpace );
    reader.setContentHandler( handler );
    reader.parse( source );

    TQListView * namespacePrefix = new TQListView( container,
                                                 "table_namespace_prefix" );
    handler->setListView( namespacePrefix );
    reader.setFeature( "http://xml.org/sax/features/namespace-prefixes",
                       true );
    source.reset();
    reader.parse( source );

    TQListView * prefix = new TQListView( container, "table_prefix");
    handler->setListView( prefix );
    reader.setFeature( "http://xml.org/sax/features/namespaces", false );
    source.reset();
    reader.parse( source );

    // namespace label
    (void) new TQLabel(
             "Default:\n"
             "http://xml.org/sax/features/namespaces: true\n"
             "http://xml.org/sax/features/namespace-prefixes: false\n",
             container );

    // namespace prefix label
    (void) new TQLabel(
             "\n"
             "http://xml.org/sax/features/namespaces: true\n"
             "http://xml.org/sax/features/namespace-prefixes: true\n",
             container );

    // prefix label
    (void) new TQLabel(
             "\n"
             "http://xml.org/sax/features/namespaces: false\n"
             "http://xml.org/sax/features/namespace-prefixes: true\n",
             container );


    app.setMainWidget( container );
    container->show();
    return app.exec();
}
