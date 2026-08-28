/****************************************************************************
**
** Implementing your own layout: flow example
**
** Copyright (C) 1996-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "card.h"

class CardLayoutIterator :public TQGLayoutIterator
{
public:
    CardLayoutIterator( TQPtrList<TQLayoutItem> *l )
	: idx( 0 ), list( l )  {}

    TQLayoutItem *current();
    TQLayoutItem *next();
    TQLayoutItem *takeCurrent();

private:
    int idx;
    TQPtrList<TQLayoutItem> *list;
};

TQLayoutItem *CardLayoutIterator::current()
{
    return idx < int( list->count() ) ? list->at( idx ) : 0;
}

TQLayoutItem *CardLayoutIterator::next()
{
    idx++; return current();
}

TQLayoutItem *CardLayoutIterator::takeCurrent()
{
    return idx < int( list->count() ) ?list->take( idx ) : 0;
}



TQLayoutIterator CardLayout::iterator()
{
    return TQLayoutIterator(  new CardLayoutIterator( &list )  );
}

CardLayout::~CardLayout()
{
    deleteAllItems();
}

void CardLayout::addItem(  TQLayoutItem *item  )
{
    list.append( item );
}

void CardLayout::setGeometry( const TQRect &rct )
{
    TQLayout::setGeometry( rct );

    TQPtrListIterator<TQLayoutItem> it( list );
    if ( it.count() == 0 )
	return;

    TQLayoutItem *o;

    int i = 0;

    int w = rct.width() - ( list.count() - 1 ) * spacing();
    int h = rct.height() - ( list.count() - 1 ) * spacing();

    while ( ( o=it.current() ) != 0 ) {
	++it;
	TQRect geom( rct.x() + i * spacing(), rct.y() + i * spacing(),
		    w, h  );
	o->setGeometry(  geom  );
	++i;
    }
}

TQSize CardLayout::sizeHint() const
{
    TQSize s(0,0);
    int n = list.count();
    if ( n > 0 )
	s = TQSize(100,70); //start with a nice default size
    TQPtrListIterator<TQLayoutItem> it(list);
    TQLayoutItem *o;
    while ( (o=it.current()) != 0 ) {
	++it;
	s = s.expandedTo( o->minimumSize() );
    }
    return s + n*TQSize(spacing(),spacing());
}

TQSize CardLayout::minimumSize() const
{
    TQSize s(0,0);
    int n = list.count();
    TQPtrListIterator<TQLayoutItem> it(list);
    TQLayoutItem *o;
    while ( (o=it.current()) != 0 ) {
	++it;
	s = s.expandedTo( o->minimumSize() );
    }
    return s + n*TQSize(spacing(),spacing());
}
