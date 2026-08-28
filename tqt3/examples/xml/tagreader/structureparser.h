/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef STRUCTUREPARSER_H
#define STRUCTUREPARSER_H   

#include <ntqxml.h>

class TQString;

class StructureParser : public TQXmlDefaultHandler
{
public:
    bool startDocument();
    bool startElement( const TQString&, const TQString&, const TQString& , 
                       const TQXmlAttributes& );
    bool endElement( const TQString&, const TQString&, const TQString& );

private:
    TQString indent;
};                   

#endif 
