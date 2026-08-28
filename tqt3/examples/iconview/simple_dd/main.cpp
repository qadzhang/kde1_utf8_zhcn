/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "main.h"

const char* red_icon[]={
"16 16 2 1",
"r c red",
". c None",
"................",
"................",
"..rrrrrrrrrrrr..",
"..rrrrrrrrrrrr..",
"..rrrrrrrrrrrr..",
"..rrr......rrr..",
"..rrr......rrr..",
"..rrr......rrr..",
"..rrr......rrr..",
"..rrr......rrr..",
"..rrr......rrr..",
"..rrrrrrrrrrrr..",
"..rrrrrrrrrrrr..",
"..rrrrrrrrrrrr..",
"................",
"................"};

const char* blue_icon[]={
"16 16 2 1",
"b c blue",
". c None",
"................",
"................",
"..bbbbbbbbbbbb..",
"..bbbbbbbbbbbb..",
"..bbbbbbbbbbbb..",
"..bbb......bbb..",
"..bbb......bbb..",
"..bbb......bbb..",
"..bbb......bbb..",
"..bbb......bbb..",
"..bbb......bbb..",
"..bbbbbbbbbbbb..",
"..bbbbbbbbbbbb..",
"..bbbbbbbbbbbb..",
"................",
"................"};

const char* green_icon[]={
"16 16 2 1",
"g c green",
". c None",
"................",
"................",
"..gggggggggggg..",
"..gggggggggggg..",
"..gggggggggggg..",
"..ggg......ggg..",
"..ggg......ggg..",
"..ggg......ggg..",
"..ggg......ggg..",
"..ggg......ggg..",
"..ggg......ggg..",
"..gggggggggggg..",
"..gggggggggggg..",
"..gggggggggggg..",
"................",
"................"};


// ListBox -- low level drag and drop

DDListBox::DDListBox( TQWidget * parent, const char * name, WFlags f ) :
    TQListBox( parent, name, f )
{
    setAcceptDrops( true );
    dragging = false;
}


void DDListBox::dragEnterEvent( TQDragEnterEvent *evt )
{
    if ( TQTextDrag::canDecode( evt ) ) 
	evt->accept();
}


void DDListBox::dropEvent( TQDropEvent *evt )
{
    TQString text;

    if ( TQTextDrag::decode( evt, text ) ) 
	insertItem( text );
}


void DDListBox::mousePressEvent( TQMouseEvent *evt )
{
    TQListBox::mousePressEvent( evt );
    dragging = true;
}


void DDListBox::mouseMoveEvent( TQMouseEvent * )
{
    if ( dragging ) {
	TQDragObject *d = new TQTextDrag( currentText(), this );
	d->dragCopy(); // do NOT delete d.
	dragging = false;
    }
}


// IconViewIcon -- high level drag and drop


bool DDIconViewItem::acceptDrop( const TQMimeSource *mime ) const
{
    if ( mime->provides( "text/plain" ) )
	return true;
    return false;
}


void DDIconViewItem::dropped( TQDropEvent *evt, const TQValueList<TQIconDragItem>& )
{
    TQString label;

    if ( TQTextDrag::decode( evt, label ) ) 
	setText( label );
}


// IconView -- high level drag and drop

TQDragObject *DDIconView::dragObject()
{
  return new TQTextDrag( currentItem()->text(), this );
}

void DDIconView::slotNewItem( TQDropEvent *evt, const TQValueList<TQIconDragItem>& )
{
    TQString label;

    if ( TQTextDrag::decode( evt, label ) ) {
	DDIconViewItem *item = new DDIconViewItem( this, label );
	item->setRenameEnabled( true );
    }
}



int main( int argc, char *argv[] )
{
    TQApplication app( argc, argv );

    // Create and show the widgets
    TQSplitter *split = new TQSplitter();
    DDIconView *iv   = new DDIconView( split );
    (void)	       new DDListBox( split );
    app.setMainWidget( split );
    split->resize( 600, 400 );
    split->show();

    // Set up the connection so that we can drop items into the icon view
    TQObject::connect( 
	iv, TQ_SIGNAL(dropped(TQDropEvent*, const TQValueList<TQIconDragItem>&)), 
	iv, TQ_SLOT(slotNewItem(TQDropEvent*, const TQValueList<TQIconDragItem>&)));

    // Populate the TQIconView with icons
    DDIconViewItem *item;
    item = new DDIconViewItem( iv, "Red",   TQPixmap( red_icon ) );
    item->setRenameEnabled( true );
    item = new DDIconViewItem( iv, "Green", TQPixmap( green_icon ) );
    item->setRenameEnabled( true );
    item = new DDIconViewItem( iv, "Blue",  TQPixmap( blue_icon ) );
    item->setRenameEnabled( true );

    return app.exec();
}


