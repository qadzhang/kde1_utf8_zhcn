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

#include "listboxdnd.h"
#include <ntqwidget.h>
#include <ntqheader.h>
#include <ntqpainter.h>
#include <ntqdragobject.h>
#include <ntqvaluelist.h>

// The Dragobject Declaration ---------------------------------------
class ListBoxItemDrag : public TQStoredDrag
{
public:
    ListBoxItemDrag( ListBoxItemList & items, bool sendPtr = false, TQListBox * parent = 0, const char * name = 0 );
    ~ListBoxItemDrag() {};
    static bool canDecode( TQDragMoveEvent * event );
    static bool decode( TQDropEvent * event, TQListBox * parent, TQListBoxItem * insertPoint );
    enum ItemType { ListBoxText = 1, ListBoxPixmap = 2 };
};
// ------------------------------------------------------------------

ListBoxDnd::ListBoxDnd( TQListBox * eventSource, const char * name )
    : ListDnd( eventSource, name ) { }

void ListBoxDnd::confirmDrop( TQListBoxItem * )
{
    dropConfirmed = true;
}

bool ListBoxDnd::dropEvent( TQDropEvent * event )
{
    if ( dragInside ) {
    
	if ( dMode & NullDrop ) { // combined with Move, a NullDrop will delete an item
	    event->accept();
	    emit dropped( 0 ); // a NullDrop
	    return true;
	}
	
	TQPoint pos = event->pos();
	TQListBoxItem * after = itemAt( pos );

	if ( ListBoxItemDrag::decode( event, (TQListBox *) src, after ) ) {
	    event->accept();
	    TQListBox * src = (TQListBox *) this->src;
	    TQListBoxItem * item = ( after ? after->next() : src->firstItem() );
	    src->setCurrentItem( item );
	    emit dropped( item ); // ###FIX: Supports only one item!
	}
    }

    line->hide();
    dragInside = false;

    return true;
}

bool ListBoxDnd::mouseMoveEvent( TQMouseEvent * event )
{
    if ( event->state() & LeftButton ) {
	if ( ( event->pos() - mousePressPos ).manhattanLength() > 3 ) {

	    ListBoxItemList list;
	    buildList( list );
	    ListBoxItemDrag * dragobject = new ListBoxItemDrag( list, (dMode & Internal), (TQListBox *) src );

	    // Emit signal for all dragged items
	    TQListBoxItem * i = list.first();
	    while ( i ) {
		emit dragged( i );
		i = list.next();
	    }

	    if ( dMode & Move ) {
		removeList( list ); // "hide" items
	    }

	    dragobject->dragCopy();

	    if ( dMode & Move ) {
		if ( dropConfirmed ) {
		    // ###FIX: memleak ? in internal mode, only pointers are transfered...
		    //list.setAutoDelete( true );
		    list.clear();
		    dropConfirmed = false;
		}
		insertList( list ); // "show" items
	    }
	}
    }
    return false;
}

int ListBoxDnd::buildList( ListBoxItemList & list )
{
    TQListBoxItem * i = ((TQListBox *)src)->firstItem();
    while ( i ) {
	if ( i->isSelected() ) {
	    ((TQListBox *)src)->setSelected( i, false );
	    list.append( i );
	}
	i = i->next();
    }
    return list.count();
}

void ListBoxDnd::insertList( ListBoxItemList & list )
{
    TQListBoxItem * i = list.first();
    while ( i ) {
	((TQListBox *)src)->insertItem( i, i->prev() );
	i = list.next();
    }
}

void ListBoxDnd::removeList( ListBoxItemList & list )
{
    TQListBoxItem * i = list.first();
    while ( i ) {
	((TQListBox *)src)->takeItem( i ); // remove item from TQListBox
	i = list.next();
    }
}

void ListBoxDnd::updateLine( const TQPoint & dragPos )
{
    TQListBox * src = (TQListBox *) this->src;
    TQListBoxItem *item = itemAt( dragPos );

    int ypos = item ? 
	( src->itemRect( item ).bottom() - ( line->height() / 2 ) ) : 
	( src->itemRect( ((TQListBox *)src)->firstItem() ).top() );

    line->resize( src->viewport()->width(), line->height() );
    line->move( 0, ypos );
}

TQListBoxItem * ListBoxDnd::itemAt( TQPoint pos )
{
    TQListBox * src = (TQListBox *) this->src;
    TQListBoxItem * result = src->itemAt( pos );
    TQListBoxItem * last = src->item( src->count() - 1 );
    int i = src->index( result );

    if ( result && ( pos.y() < (src->itemRect(result).top() + src->itemHeight(i)/2) ) )
	result = result->prev();
    else if ( !result && pos.y() > src->itemRect( last ).bottom() )
	result = last;

    return result;
}

bool ListBoxDnd::canDecode( TQDragEnterEvent * event )
{
    return ListBoxItemDrag::canDecode( event );
}


// ------------------------------------------------------------------
// The Dragobject Implementation ------------------------------------
// ------------------------------------------------------------------

ListBoxItemDrag::ListBoxItemDrag( ListBoxItemList & items, bool sendPtr, TQListBox * parent, const char * name )
    : TQStoredDrag( "qt/listboxitem", parent, name )
{
    // ### FIX!
    TQByteArray data( sizeof( TQ_INT32 ) + sizeof( TQListBoxItem ) * items.count() );
    TQDataStream stream( data, IO_WriteOnly );

    stream << items.count();
    stream << (TQ_UINT8) sendPtr; // just transfer item pointer; omit data

    TQListBoxItem * i = items.first();

    if ( sendPtr ) {
	
	while ( i ) {
	    
	    stream << (TQ_ULONG) i; //###FIX: demands sizeof(ulong) >= sizeof(void*)
	    i = items.next();
	    
	}
	
    } else {
  
	while ( i ) {
	
	    TQ_UINT8 b = 0;

	    b = (TQ_UINT8) ( i->text() != TQString::null ); // does item have text ?
	    stream << b;
	    if ( b ) {
		stream << i->text();
	    }
    
	    b = (TQ_UINT8) ( !!i->pixmap() ); // does item have a pixmap ?
	    stream << b;
	    if ( b ) {
		stream << ( *i->pixmap() );
	    }

	    stream << (TQ_UINT8) i->isSelectable();
	    
	    i = items.next();
	}
    
    }

    setEncodedData( data );
}

bool ListBoxItemDrag::canDecode( TQDragMoveEvent * event )
{
    return event->provides( "qt/listboxitem" );
}

bool ListBoxItemDrag::decode( TQDropEvent * event, TQListBox * parent, TQListBoxItem * after )
{
    TQByteArray data = event->encodedData( "qt/listboxitem" );

    if ( data.size() ) {
	event->accept();
	TQDataStream stream( data, IO_ReadOnly );

	int count = 0;
	stream >> count;

	TQ_UINT8 recievePtr = 0; // data contains just item pointers; no data
	stream >> recievePtr;

	TQListBoxItem * item = 0;

	if ( recievePtr ) {
	    
	    for( int i = 0; i < count; i++ ) {

		TQ_ULONG p = 0; //###FIX: demands sizeof(ulong) >= sizeof(void*)
		stream >> p;
		item = (TQListBoxItem *) p;
		
		parent->insertItem( item, after );
		
	    }
	    
	} else {		

	    for ( int i = 0; i < count; i++ ) {

		TQ_UINT8 hasText = 0;
		TQString text;
		stream >> hasText;
		if ( hasText ) {
		    stream >> text;
		}
	    
		TQ_UINT8 hasPixmap = 0;
		TQPixmap pixmap;
		stream >> hasPixmap;
		if ( hasPixmap ) {
		    stream >> pixmap;
		}
	    
		TQ_UINT8 isSelectable = 0;
		stream >> isSelectable;

		if ( hasPixmap ) {
		    item = new TQListBoxPixmap( parent, pixmap, text, after );
		} else {
		    item = new TQListBoxText( parent, text, after );
		}

		item->setSelectable( isSelectable );
	    
	    }
	
	}
	
	return true;
    }
    return false;
}
