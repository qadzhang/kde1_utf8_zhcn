/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef FTPVIEWITEM_H
#define FTPVIEWITEM_H

#include <ntqlistview.h>
#include <ntqdatetime.h>

class FtpViewItem : public TQListViewItem
{
public:
    enum Type {
	Directory,
	File
    };

    FtpViewItem( TQListView *parent, Type t, const TQString &name, const TQString &size, const TQString &lastModified );

    int compare( TQListViewItem * i, int col, bool ascending ) const;

    bool isDir()
    { return type==Directory; }

private:
    Type type;
};

#endif
