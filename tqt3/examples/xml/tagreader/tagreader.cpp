/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "structureparser.h"
#include <ntqfile.h>
#include <ntqxml.h>
#include <ntqwindowdefs.h>

int main( int argc, char **argv )
{
    if ( argc < 2 ) {
	fprintf( stderr, "Usage: %s <xmlfile> [<xmlfile> ...]\n", argv[0] );
	return 1;
    }
    StructureParser handler;
    TQXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    for ( int i=1; i < argc; i++ ) {
        TQFile xmlFile( argv[i] );
        TQXmlInputSource source( &xmlFile );
        reader.parse( source );
    }
    return 0;
}
