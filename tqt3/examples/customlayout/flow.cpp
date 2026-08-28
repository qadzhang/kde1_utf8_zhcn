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

#include "flow.h"

class SimpleFlowIterator :public TQGLayoutIterator
{
public:
    SimpleFlowIterator( TQPtrList<TQLayoutItem> *l ) :idx(0), list(l)  {}
    uint count() const;
    TQLayoutItem *current();
    TQLayoutItem *next();
    TQLayoutItem *takeCurrent();

private:
    int idx;
    TQPtrList<TQLayoutItem> *list;

};

uint SimpleFlowIterator::count() const
{
    return list->count();
}

TQLayoutItem *SimpleFlowIterator::current()
{
    return idx < int(count()) ? list->at(idx) : 0;
}

TQLayoutItem *SimpleFlowIterator::next()
{
    idx++; return current();
}

TQLayoutItem *SimpleFlowIterator::takeCurrent()
{
    return idx < int(count()) ? list->take( idx ) : 0;
}

SimpleFlow::~SimpleFlow()
{
    deleteAllItems();
}


int SimpleFlow::heightForWidth( int w ) const
{
    if ( cached_width != w ) {
	//Not all C++ compilers support "mutable" yet:
	SimpleFlow * mthis = (SimpleFlow*)this;
	int h = mthis->doLayout( TQRect(0,0,w,0), true );
	mthis->cached_hfw = h;
	mthis->cached_width = w;
	return h;
    }
    return cached_hfw;
}

void SimpleFlow::addItem( TQLayoutItem *item)
{
    list.append( item );
}

bool SimpleFlow::hasHeightForWidth() const
{
    return true;
}

TQSize SimpleFlow::sizeHint() const
{
    return minimumSize();
}

TQSizePolicy::ExpandData SimpleFlow::expanding() const
{
    return TQSizePolicy::NoDirection;
}

TQLayoutIterator SimpleFlow::iterator()
{
    return TQLayoutIterator( new SimpleFlowIterator( &list ) );
}

void SimpleFlow::setGeometry( const TQRect &r )
{
    TQLayout::setGeometry( r );
    doLayout( r );
}

int SimpleFlow::doLayout( const TQRect &r, bool testonly )
{
    int x = r.x();
    int y = r.y();
    int h = 0;		//height of this line so far.
    TQPtrListIterator<TQLayoutItem> it(list);
    TQLayoutItem *o;
    while ( (o=it.current()) != 0 ) {
	++it;
	int nextX = x + o->sizeHint().width() + spacing();
	if ( nextX - spacing() > r.right() && h > 0 ) {
	    x = r.x();
	    y = y + h + spacing();
	    nextX = x + o->sizeHint().width() + spacing();
	    h = 0;
	}
	if ( !testonly )
	    o->setGeometry( TQRect( TQPoint( x, y ), o->sizeHint() ) );
	x = nextX;
	h = TQMAX( h,  o->sizeHint().height() );
    }
    return y + h - r.y();
}

TQSize SimpleFlow::minimumSize() const
{
    TQSize s(0,0);
    TQPtrListIterator<TQLayoutItem> it(list);
    TQLayoutItem *o;
    while ( (o=it.current()) != 0 ) {
	++it;
	s = s.expandedTo( o->minimumSize() );
    }
    return s;
}
