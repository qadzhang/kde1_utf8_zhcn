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

#include "border.h"

class BorderLayoutIterator : public TQGLayoutIterator
{
public:
    BorderLayoutIterator( const TQPtrList<BorderLayout::BorderLayoutStruct> *l )
	: idx( 0 ) , list( (TQPtrList<BorderLayout::BorderLayoutStruct>*)l )
    {}

    uint count() const;
    TQLayoutItem *current();
    BorderLayout::BorderLayoutStruct *currentStruct();
    void toFirst();
    TQLayoutItem *next();
    TQLayoutItem *takeCurrent();
    BorderLayoutIterator &operator++();

private:
    int idx;
    TQPtrList<BorderLayout::BorderLayoutStruct> *list;

};

uint BorderLayoutIterator::count() const
{
    return list->count();
}

TQLayoutItem *BorderLayoutIterator::current()
{
    return idx < (int)count() ? list->at( idx )->item : 0;
}

BorderLayout::BorderLayoutStruct *BorderLayoutIterator::currentStruct()
{
    return idx < (int)count() ? list->at( idx ) : 0;
}

void BorderLayoutIterator::toFirst()
{
    idx = 0;
}

TQLayoutItem *BorderLayoutIterator::next()
{
    idx++;
    return current();
}

TQLayoutItem *BorderLayoutIterator::takeCurrent()
{
    BorderLayout::BorderLayoutStruct *b
	= idx < int( list->count() ) ? list->take(  idx  ) : 0;
    TQLayoutItem *item =  b ? b->item : 0;
    delete b;
    return item;
}

BorderLayoutIterator &BorderLayoutIterator::operator++()
{
    next();
    return *this;
}

BorderLayout::~BorderLayout()
{
    deleteAllItems();
}


void BorderLayout::addItem( TQLayoutItem *item )
{
    add( item, West );
}

void BorderLayout::addWidget( TQWidget *widget, Position pos )
{
    add( new BorderWidgetItem( widget ), pos );
}

void BorderLayout::add( TQLayoutItem *item, Position pos )
{
    list.append( new BorderLayoutStruct( item, pos ) );
    sizeDirty = true; msizeDirty = true;
    calcSize( SizeHint ); calcSize( Minimum );
}

bool BorderLayout::hasHeightForWidth() const
{
    return false;
}

TQSize BorderLayout::sizeHint() const
{
    return cached;
}

TQSize BorderLayout::minimumSize() const
{
    return cached;
}

TQSizePolicy::ExpandData BorderLayout::expanding() const

{
    return TQSizePolicy::BothDirections;
}

TQLayoutIterator BorderLayout::iterator()
{
    return TQLayoutIterator( new BorderLayoutIterator( &list ) );
}

void BorderLayout::setGeometry( const TQRect &rct )
{
    TQLayout::setGeometry( rct );
    doLayout( rct );
}

void BorderLayout::doLayout( const TQRect &rct, bool /*testonly*/ )
{
    int ew = 0, ww = 0, nh = 0, sh = 0;
    int h = 0;

    BorderLayoutIterator it( &list );
    BorderLayoutStruct *o;
    BorderLayoutStruct *center = 0;
    while ( ( o = it.currentStruct() ) != 0 ) {
	++it;

	if ( o->pos == North ) {
	    o->item->setGeometry( TQRect( rct.x(), nh, rct.width(), o->item->sizeHint().height() ) );
	    nh += o->item->geometry().height() + spacing();
	}
	if ( o->pos == South ) {
	    o->item->setGeometry( TQRect( o->item->geometry().x(), o->item->geometry().y(),
					 rct.width(), o->item->sizeHint().height() ) );
	    sh += o->item->geometry().height() + spacing();
	    o->item->setGeometry( TQRect( rct.x(), rct.y() + rct.height() - sh + spacing(),
					 o->item->geometry().width(), o->item->geometry().height() ) );
	}
	if ( o->pos == Center )
	    center = o;
    }

    h = rct.height() - nh - sh;

    it.toFirst();
    while ( ( o = it.currentStruct() ) != 0 ) {
	++it;

	if ( o->pos == West ) {
	    o->item->setGeometry( TQRect( rct.x() + ww, nh, o->item->sizeHint().width(), h ) );
	    ww += o->item->geometry().width() + spacing();
	}
	if ( o->pos == East ) {
	    o->item->setGeometry( TQRect( o->item->geometry().x(), o->item->geometry().y(),
					 o->item->sizeHint().width(), h ) );
	    ew += o->item->geometry().width() + spacing();
	    o->item->setGeometry( TQRect( rct.x() + rct.width() - ew + spacing(), nh,
					 o->item->geometry().width(), o->item->geometry().height() ) );
	}
    }

    if ( center )
	center->item->setGeometry( TQRect( ww, nh, rct.width() - ew - ww, h ) );
}

void BorderLayout::calcSize( SizeType st )
{
    if ( ( st == Minimum && !msizeDirty ) ||
	 ( st == SizeHint && !sizeDirty ) )
	return;

    int w = 0, h = 0;

    BorderLayoutIterator it( &list );
    BorderLayoutStruct *o;
    while ( ( o = it.currentStruct() ) != 0 ) {
	++it;
	if ( o->pos == North ||
	     o->pos == South ) {
	    if ( st == Minimum )
		h += o->item->minimumSize().height();
	    else
		h += o->item->sizeHint().height();
	}
	else if ( o->pos == West ||
		  o->pos == East ) {
	    if ( st == Minimum )
		w += o->item->minimumSize().width();
	    else
		w += o->item->sizeHint().width();
	} else {
	    if ( st == Minimum ) {
		h += o->item->minimumSize().height();
		w += o->item->minimumSize().width();
	    }
	    else {
		h += o->item->sizeHint().height();
		w += o->item->sizeHint().width();
	    }
	}
    }

    if ( st == Minimum ) {
	msizeDirty = false;
	mcached = TQSize( w, h );
    } else {
	sizeDirty = false;
	cached = TQSize( w, h );
    }

    return;
}
