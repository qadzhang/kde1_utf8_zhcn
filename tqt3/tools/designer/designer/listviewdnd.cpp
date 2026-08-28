/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "listviewdnd.h"
#include <ntqwidget.h>
#include <ntqheader.h>
#include <ntqpainter.h>
#include <ntqdragobject.h>
#include <ntqvaluelist.h>

// The Dragobject Declaration ---------------------------------------
class ListViewItemDrag : public TQStoredDrag
{
public:
    enum DropRelation { Sibling, Child };
    ListViewItemDrag( ListViewItemList & items, TQWidget * parent = 0, const char * name = 0 );
    ~ListViewItemDrag() {};
    static bool canDecode( TQDragMoveEvent * event );
    static bool decode( TQDropEvent * event, TQListView * parent, TQListViewItem * insertPoint, DropRelation dr );
};
// ------------------------------------------------------------------

ListViewDnd::ListViewDnd( TQListView * eventSource, const char * name )
    : ListDnd( eventSource, name ) { }

void ListViewDnd::confirmDrop( TQListViewItem * )
{
    dropConfirmed = true;
}

bool ListViewDnd::dropEvent( TQDropEvent * event )
{
    if ( dragInside ) {
    
	if ( dMode & NullDrop ) { // combined with Move, a NullDrop will delete an item
	    event->accept();
	    emit dropped( 0 ); // a NullDrop
	    return true;
	}
	
	TQPoint pos = event->pos();

	ListViewItemDrag::DropRelation dr = ListViewItemDrag::Sibling;
	TQListViewItem *item = itemAt( pos );
	int dpos = dropDepth( item, pos );

	if ( item ) {
	    if ( dpos > item->depth() && !(dMode & Flat) ) {
		// Child node
		dr = ListViewItemDrag::Child;
	    } else if ( dpos < item->depth() ) {
		// Parent(s) Sibling
		while ( item && (item->depth() > dpos) )
		    item = item->parent();
	    }
	}

	if ( ListViewItemDrag::decode( event, (TQListView *) src, item, dr ) ) {
	    event->accept();
	    emit dropped( 0 ); // Use ID instead of item?
	}
    }

    line->hide();
    dragInside = false;

    return true;
}

bool ListViewDnd::mouseMoveEvent( TQMouseEvent * event )
{
    if ( event->state() & LeftButton ) {
	if ( ( event->pos() - mousePressPos ).manhattanLength() > 3 ) {
	    ListViewItemList list;

	    if ( dMode & Flat )
		buildFlatList( list );
	    else
		buildTreeList( list );

	    ListViewItemDrag * dragobject = new ListViewItemDrag( list, (TQListView *) src );

	    if ( dMode & Move ) {
		disabledItems = list;
		setVisibleItems( false );
	    }

	    dragobject->dragCopy();

	    if ( dMode & Move ) {
		// Did the target accept the drop?
		if ( dropConfirmed ) {
		    // Shouldn't autoDelete handle this?
		    for( list.first(); list.current(); list.next() ) 
			delete list.current();
		    dropConfirmed = false;
		} else {
		    // Reenable disabled items since 
		    // drag'n'drop was aborted
		    setVisibleItems( true );
		}
		disabledItems.clear();
	    }
	}
    }
    return false;
}

int ListViewDnd::buildFlatList( ListViewItemList & list )
{
    bool addKids = false;
    TQListViewItem *nextSibling = 0;
    TQListViewItem *nextParent = 0;
    TQListViewItemIterator it = ((TQListView *)src)->firstChild();
    for ( ; *it; it++ ) {
	// Hit the nextSibling, turn of child processing
	if ( (*it) == nextSibling )
	    addKids = false;

	if ( (*it)->isSelected() ) {
	    if ( (*it)->childCount() == 0 ) {
		// Selected, no children
		list.append( *it );
	    } else if ( !addKids ) {
		// Children processing not set, so set it
		// Also find the item were we shall quit
		// processing children...if any such item
		addKids = true;
		nextSibling = (*it)->nextSibling();
		nextParent = (*it)->parent();
		while ( nextParent && !nextSibling ) {
		    nextSibling = nextParent->nextSibling();
		    nextParent = nextParent->parent();
		}
	    }
	} else if ( ((*it)->childCount() == 0) && addKids ) {
	    // Leaf node, and we _do_ process children
	    list.append( *it );
	}
    }
    return list.count();
}

int ListViewDnd::buildTreeList( ListViewItemList & list )
{
    TQListViewItemIterator it = ((TQListView *)src)->firstChild();
    for ( ; *it; it++ ) {
	if ( (*it)->isSelected() )
	    list.append( *it );
    }
    return list.count();
}

void ListViewDnd::setVisibleItems( bool b )
{
    if ( disabledItems.isEmpty() ) 
	return;
    
    disabledItems.first();
    do {
        disabledItems.current()->setVisible( b );
    } while ( disabledItems.next() );
}

void ListViewDnd::updateLine( const TQPoint & dragPos )
{
    TQListViewItem * item = itemAt(dragPos);
    TQListView * src = (TQListView *) this->src;

    int ypos = item ? 
	( src->itemRect( item ).bottom() - ( line->height() / 2 ) ) : 
	( src->itemRect( src->firstChild() ).top() );

    int xpos = dropDepth( item, dragPos ) * src->treeStepSize();
    line->resize( src->viewport()->width() - xpos, line->height() );
    line->move( xpos, ypos );
}

TQListViewItem * ListViewDnd::itemAt( TQPoint pos )
{
    TQListView * src = (TQListView *) this->src;
    int headerHeight = (int)(src->header()->height());
    pos.ry() -= headerHeight;
    TQListViewItem * result = src->itemAt( pos );

    if ( result && ( pos.ry() < (src->itemPos(result) + result->height()/2) ) )
	result = result->itemAbove();

    // Wind back if has parent, and we're in flat mode
    while ( result && result->parent() && (dMode & Flat) )
	result = result->parent();

    // Wind back if has parent, and we're in flat mode
    while ( result && !result->isVisible() && result->parent() )
	result = result->parent();

    if ( !result && src->firstChild() && (pos.y() > src->itemRect(src->firstChild()).bottom()) ) {
	result = src->lastItem();
	if ( !result->isVisible() )
	    // Handle special case where last item is actually hidden
	    result = result->itemAbove();
    }

    return result;
}

int ListViewDnd::dropDepth( TQListViewItem * item, TQPoint pos )
{
    if ( !item || (dMode & Flat) )
	return 0;

    int result     = 0;
    int itemDepth  = item->depth();
    int indentSize = ((TQListView *)src)->treeStepSize();
    int itemLeft   = indentSize * itemDepth;
    int childMargin  = indentSize*2;
    if ( pos.x() > itemLeft + childMargin ) {
	result = itemDepth + 1;
    } else if ( pos.x() < itemLeft ) {
	result = pos.x() / indentSize;
    } else {
	result = itemDepth;
    }
    return result;
}

bool ListViewDnd::canDecode( TQDragEnterEvent * event )
{
    return ListViewItemDrag::canDecode( event );
}

// ------------------------------------------------------------------
// The Dragobject Implementation ------------------------------------
// ------------------------------------------------------------------

TQDataStream & operator<< ( TQDataStream & stream, const TQListViewItem & item );
TQDataStream & operator>> ( TQDataStream & stream, TQListViewItem & item );

ListViewItemDrag::ListViewItemDrag( ListViewItemList & items, TQWidget * parent, const char * name )
    : TQStoredDrag( "qt/listviewitem", parent, name )
{
    // ### FIX!
    TQByteArray data( sizeof( TQ_INT32 ) + sizeof( TQListViewItem ) * items.count() );
    TQDataStream stream( data, IO_WriteOnly );

    stream << items.count();

    TQListViewItem *i = items.first();
    while ( i ) {
        stream << *i;
	i = items.next();
    }

    setEncodedData( data );
}

bool ListViewItemDrag::canDecode( TQDragMoveEvent * event )
{
    return event->provides( "qt/listviewitem" );
}

bool ListViewItemDrag::decode( TQDropEvent * event, TQListView * parent, TQListViewItem * insertPoint, DropRelation dr )
{
    TQByteArray data = event->encodedData( "qt/listviewitem" );
    TQListViewItem* itemParent = insertPoint ? insertPoint->parent() : 0;

    // Change from sibling (default) to child creation
    if ( insertPoint && dr == Child ) {
	itemParent = insertPoint;
	insertPoint = 0;
    }

    if ( data.size() ) {
	event->accept();
	TQDataStream stream( data, IO_ReadOnly );

	int count = 0;
	stream >> count;

	for( int i = 0; i < count; i++ ) {
	    if ( itemParent ) {
		insertPoint = new TQListViewItem( itemParent, insertPoint );
		itemParent->setOpen( true );
	    } else { // No parent for insertPoint, use TQListView
		insertPoint = new TQListViewItem( parent, insertPoint );
	    }
	    stream >> *insertPoint;
	}
	return true;
    }
    return false;
}


TQDataStream & operator<< ( TQDataStream & stream, const TQListViewItem & item )
{
    int columns = item.listView()->columns();
    stream << columns;
 
    TQ_UINT8 b = 0;

    int i;
    for ( i = 0; i < columns; i++ ) {
	b = (TQ_UINT8) ( item.text( i ) != TQString::null ); // does column i have a string ?
	stream << b;
	if ( b ) {
	    stream << item.text( i );
	}
    }
    
    for ( i = 0; i < columns; i++ ) {
	b = (TQ_UINT8) ( !!item.pixmap( i ) ); // does column i have a pixmap ?
	stream << b;
	if ( b ) {
	    stream << ( *item.pixmap( i ) );
	}
    }

    stream << (TQ_UINT8) item.isOpen();
    stream << (TQ_UINT8) item.isSelectable();
    stream << (TQ_UINT8) item.isExpandable();
    stream << (TQ_UINT8) item.dragEnabled();
    stream << (TQ_UINT8) item.dropEnabled();
    stream << (TQ_UINT8) item.isVisible();

    for ( i = 0; i < columns; i++ ) {
	stream << (TQ_UINT8) item.renameEnabled( i );
    }

    stream << (TQ_UINT8) item.multiLinesEnabled();
    stream << item.childCount();

    if ( item.childCount() > 0 ) {
	TQListViewItem * child = item.firstChild();
	while ( child ) {
	    stream << ( *child ); // recursive call
	    child = child->nextSibling();
	}
    }

    return stream;
}
    
TQDataStream & operator>> ( TQDataStream & stream, TQListViewItem & item )
{
    TQ_INT32 columns;
    stream >> columns;

    TQ_UINT8 b = 0;

    TQString text;
    int i;
    for ( i = 0; i < columns; i++ ) {
	stream >> b;
	if ( b ) { // column i has string ?
	    stream >> text;
	    item.setText( i, text );
	}
    }

    TQPixmap pixmap;
    for ( i = 0; i < columns; i++ ) {
	stream >> b; // column i has pixmap ?
	if ( b ) {
	    stream >> pixmap;
	    item.setPixmap( i, pixmap );
	}
    }

    stream >> b;
    item.setOpen( b );

    stream >> b;
    item.setSelectable( b );

    stream >> b;
    item.setExpandable( b );

    stream >> b;
    item.setDragEnabled( b );

    stream >> b;
    item.setDropEnabled( b );

    stream >> b;
    item.setVisible( b );

    for ( i = 0; i < columns; i++ ) {
	stream >> b;
	item.setRenameEnabled( i, b );
    }

    stream >> b;
    item.setMultiLinesEnabled( b );

    int childCount;
    stream >> childCount;

    TQListViewItem *child = 0;
    TQListViewItem *prevchild = 0;
    for ( i = 0; i < childCount; i++ ) {
	child = new TQListViewItem( &item, prevchild );
	stream >> ( *child );
	item.insertItem( child );
	prevchild = child;
    }

    return stream;
}
