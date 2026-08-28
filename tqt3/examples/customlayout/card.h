/****************************************************************************
**
** Definition of simple flow layout for custom layout example
**
** Created : 979899
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef CARD_H
#define CARD_H

#include <ntqlayout.h>
#include <ntqptrlist.h>

class CardLayout : public TQLayout
{
public:
    CardLayout( TQWidget *parent, int dist )
	: TQLayout( parent, 0, dist ) {}
    CardLayout( TQLayout* parent, int dist)
	: TQLayout( parent, dist ) {}
    CardLayout( int dist )
	: TQLayout( dist ) {}
    ~CardLayout();

    void addItem( TQLayoutItem *item );
    TQSize sizeHint() const;
    TQSize minimumSize() const;
    TQLayoutIterator iterator();
    void setGeometry( const TQRect &rect );

private:
    TQPtrList<TQLayoutItem> list;

};

#endif
