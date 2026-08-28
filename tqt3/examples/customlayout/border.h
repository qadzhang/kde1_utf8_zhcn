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

#ifndef BORDER_H
#define BORDER_H

#include <ntqlayout.h>
#include <ntqptrlist.h>

class BorderWidgetItem : public TQWidgetItem
{
public:
    BorderWidgetItem( TQWidget *w )
	: TQWidgetItem( w )
    {}

    void setGeometry( const TQRect &r )
    { widget()->setGeometry( r ); }

};

class BorderLayout : public TQLayout
{
public:
    enum Position {
	West = 0,
	North,
	South,
	East,
	Center
    };

    struct BorderLayoutStruct
    {
	BorderLayoutStruct( TQLayoutItem *i, Position p ) {
	    item = i;
	    pos = p;
	}

	TQLayoutItem *item;
	Position pos;
    };

    enum SizeType {
	Minimum = 0,
	SizeHint
    };

    BorderLayout( TQWidget *parent, int border = 0, int autoBorder = -1,
		  const char *name = 0 )
	: TQLayout( parent, border, autoBorder, name ), cached( 0, 0 ), mcached( 0, 0 ),
	  sizeDirty( true ), msizeDirty( true )
    {}

    BorderLayout( TQLayout* parent, int autoBorder = -1, const char *name = 0 )
	: TQLayout( parent, autoBorder, name  ), cached( 0, 0 ), mcached( 0, 0 ),
	  sizeDirty( true ), msizeDirty( true )
    {}

    BorderLayout( int autoBorder = -1, const char *name = 0 )
	: TQLayout( autoBorder, name ), cached( 0, 0 ), mcached( 0, 0 ),
	  sizeDirty( true ), msizeDirty( true )
    {}

    ~BorderLayout();

    void addItem( TQLayoutItem *item );

    void addWidget( TQWidget *widget, Position pos );
    void add( TQLayoutItem *item, Position pos );

    bool hasHeightForWidth() const;

    TQSize sizeHint() const;
    TQSize minimumSize() const;

    TQLayoutIterator iterator();

    TQSizePolicy::ExpandData expanding() const;

protected:
    void setGeometry( const TQRect &rect );

private:
    void doLayout( const TQRect &rect, bool testonly = false );
    void calcSize( SizeType st );

    TQPtrList<BorderLayoutStruct> list;
    TQSize cached, mcached;
    bool sizeDirty, msizeDirty;

};

#endif
