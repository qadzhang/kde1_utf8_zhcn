/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef INFODATA_H
#define INFODATA_H

#include <ntqdict.h>
#include <ntqstringlist.h>


// The InfoData class manages data, organized in tree structure.
class InfoData
{
public:
    InfoData();
    TQStringList list( TQString path, bool *found ) const;
    TQString get( TQString path, bool *found ) const;

private:
    TQDict< TQStringList > nodes;
    TQDict< TQString > data;
};

#endif // INFODATA_H

