/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "outlinetree.h"
#include <ntqfile.h>
#include <ntqmessagebox.h>

OutlineTree::OutlineTree( const TQString fileName, TQWidget *parent, const char *name )
    : TQListView( parent, name )
{
    // div. configuration of the list view
    addColumn( "Outlines" );
    setSorting( -1 );
    setRootIsDecorated( true );

    // read the XML file and create DOM tree
    TQFile opmlFile( fileName );
    if ( !opmlFile.open( IO_ReadOnly ) ) {
	TQMessageBox::critical( 0,
		tr( "Critical Error" ),
		tr( "Cannot open file %1" ).arg( fileName ) );
	return;
    }
    if ( !domTree.setContent( &opmlFile ) ) {
	TQMessageBox::critical( 0,
		tr( "Critical Error" ),
		tr( "Parsing error for file %1" ).arg( fileName ) );
	opmlFile.close();
	return;
    }
    opmlFile.close();

    // get the header information from the DOM
    TQDomElement root = domTree.documentElement();
    TQDomNode node;
    node = root.firstChild();
    while ( !node.isNull() ) {
	if ( node.isElement() && node.nodeName() == "head" ) {
	    TQDomElement header = node.toElement();
	    getHeaderInformation( header );
	    break;
	}
	node = node.nextSibling();
    }
    // create the tree view out of the DOM
    node = root.firstChild();
    while ( !node.isNull() ) {
	if ( node.isElement() && node.nodeName() == "body" ) {
	    TQDomElement body = node.toElement();
	    buildTree( 0, body );
	    break;
	}
	node = node.nextSibling();
    }
}

OutlineTree::~OutlineTree()
{
}

void OutlineTree::getHeaderInformation( const TQDomElement &header )
{
    // visit all children of the header element and look if you can make
    // something with it
    TQDomNode node = header.firstChild();
    while ( !node.isNull() ) {
	if ( node.isElement() ) {
	    // case for the different header entries
	    if ( node.nodeName() == "title" ) {
		TQDomText textChild = node.firstChild().toText();
		if ( !textChild.isNull() ) {
		    setColumnText( 0, textChild.nodeValue() );
		}
	    }
	}
	node = node.nextSibling();
    }
}

void OutlineTree::buildTree( TQListViewItem *parentItem, const TQDomElement &parentElement )
{
    TQListViewItem *thisItem = 0;
    TQDomNode node = parentElement.firstChild();
    while ( !node.isNull() ) {
	if ( node.isElement() && node.nodeName() == "outline" ) {
	    // add a new list view item for the outline
	    if ( parentItem == 0 )
		thisItem = new TQListViewItem( this, thisItem );
	    else
		thisItem = new TQListViewItem( parentItem, thisItem );
	    thisItem->setText( 0, node.toElement().attribute( "text" ) );
	    // recursive build of the tree
	    buildTree( thisItem, node.toElement() );
	}
	node = node.nextSibling();
    }
}
