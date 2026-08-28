/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef OUTLINETREE_H
#define OUTLINETREE_H

#include <ntqlistview.h>
#include <ntqdom.h>

class OutlineTree : public TQListView
{
    TQ_OBJECT

public:
    OutlineTree( const TQString fileName, TQWidget *parent = 0, const char *name = 0 );
    ~OutlineTree();

private:
    TQDomDocument domTree;
    void getHeaderInformation( const TQDomElement &header );
    void buildTree( TQListViewItem *parentItem, const TQDomElement &parentElement );
};

#endif // OUTLINETREE_H
