/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "structureparser.h"

#include <stdio.h>
#include <ntqstring.h>
 
bool StructureParser::startDocument()
{
    indent = "";
    return true;
}

bool StructureParser::startElement( const TQString&, const TQString&, 
                                    const TQString& qName, 
                                    const TQXmlAttributes& )
{
    printf( "%s%s\n", (const char*)indent, (const char*)qName );
    indent += "    ";
    return true;
}

bool StructureParser::endElement( const TQString&, const TQString&, const TQString& )
{
    indent.remove( (uint)0, 4 );
    return true;
}
