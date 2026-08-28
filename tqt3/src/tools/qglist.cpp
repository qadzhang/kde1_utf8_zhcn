/****************************************************************************
**
** Implementation of TQGList and TQGListIterator classes
**
** Created : 920624
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the tools module of the TQt GUI Toolkit.
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
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "ntqglist.h"
#include "ntqgvector.h"
#include "ntqdatastream.h"
#include "ntqvaluelist.h"

#if defined(TQT_THREAD_SUPPORT)
  #include "ntqmutex.h"
#endif // defined(TQT_THREAD_SUPPORT)

/*!
  \class TQLNode ntqglist.h
  \reentrant
  \ingroup collection
  \brief The TQLNode class is an internal class for the TQPtrList template collection.

  \internal

  TQLNode is a doubly-linked list node. It has three pointers:
  \list 1
  \i Pointer to the previous node.
  \i Pointer to the next node.
  \i Pointer to the actual data.
  \endlist

  It might sometimes be practical to have direct access to the list nodes
  in a TQPtrList, but it is seldom required.

  Be very careful if you want to access the list nodes. The heap can
  easily get corrupted if you make a mistake.

  \sa TQPtrList::currentNode(), TQPtrList::removeNode(), TQPtrList::takeNode()
*/

/*!
  \fn TQPtrCollection::Item TQLNode::getData()
  Returns a pointer (\c void*) to the actual data in the list node.
*/


/*!
  \class TQGList ntqglist.h
  \reentrant
  \ingroup collection
  \brief The TQGList class is an internal class for implementing TQt collection classes.

  \internal

  TQGList is a strictly internal class that acts as a base class for
  several collection classes; TQPtrList, TQPtrQueue and TQPtrStack.

  TQGList has some virtual functions that can be reimplemented to
  customize the subclasses, namely compareItems(), read() and
  write. Normally, you do not have to reimplement any of these
  functions.  If you still want to reimplement them, see the TQStrList
  class (ntqstrlist.h) for an example.
*/


/* Internal helper class for TQGList. Contains some optimization for
   the typically case where only one iterators is activre on the list.
 */
class TQGListIteratorList
{
public:
    TQGListIteratorList()
	: list(0), iterator(0) {
    }
    ~TQGListIteratorList() {
	notifyClear( true );
	delete list;
    }

    void add( TQGListIterator* i ) {
	if ( !iterator ) {
	    iterator = i;
	} else if ( list ) {
	    list->push_front( i );
	} else {
	    list = new TQValueList<TQGListIterator*>;
	    list->push_front( i );
	}
    }

    void remove( TQGListIterator* i ) {
	if ( iterator == i ) {
	    iterator = 0;
	} else if ( list ) {
	    list->remove( i );
	    if ( list->isEmpty() ) {
		delete list;
		list = 0;
	    }
	}
    }

    void notifyClear( bool zeroList ) {
	if ( iterator ) {
	    if ( zeroList )
		iterator->list = 0;
	    iterator->curNode = 0;
	}
	if ( list ) {
	    for ( TQValueList<TQGListIterator*>::Iterator i = list->begin(); i != list->end(); ++i ) {
		if ( zeroList )
		    (*i)->list = 0;
		(*i)->curNode = 0;
	    }
	}
    }

    void notifyRemove( TQLNode* n, TQLNode* curNode ) {
	if ( iterator ) {
	    if ( iterator->curNode == n )
		iterator->curNode = curNode;
	}
	if ( list ) {
	    for ( TQValueList<TQGListIterator*>::Iterator i = list->begin(); i != list->end(); ++i ) {
		if ( (*i)->curNode == n )
		    (*i)->curNode = curNode;
	    }
	}
    }

private:
    TQValueList<TQGListIterator*>* list;
    TQGListIterator* iterator;
};



/*****************************************************************************
  Default implementation of virtual functions
 *****************************************************************************/

/*!
  Documented as TQPtrList::compareItems().

  Compares \a item1 with \a item2.
*/
int TQGList::compareItems( TQPtrCollection::Item item1, TQPtrCollection::Item item2 )
{
    return item1 != item2;			// compare pointers
}

#ifndef TQT_NO_DATASTREAM
/*!
    \overload
  Reads a collection/list item from the stream \a s and returns a reference
  to the stream.

  The default implementation sets \a item to 0.

  \sa write()
*/

TQDataStream &TQGList::read( TQDataStream &s, TQPtrCollection::Item &item )
{
    item = 0;
    return s;
}

/*!
    \overload
  Writes a collection/list item to the stream \a s and
  returns a reference to the stream.

  The default implementation does nothing.

  \sa read()
*/

TQDataStream &TQGList::write( TQDataStream &s, TQPtrCollection::Item ) const
{
    return s;
}
#endif // TQT_NO_DATASTREAM

/*****************************************************************************
  TQGList member functions
 *****************************************************************************/

/*!
  Constructs an empty list.
*/

TQGList::TQGList()
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex = new TQMutex(true);
#endif
    firstNode = lastNode = curNode = 0;		// initialize list
    numNodes  = 0;
    curIndex  = -1;
    iterators = 0;				// initialize iterator list
}

/*!
  Constructs a copy of \a list.
*/

TQGList::TQGList( const TQGList & list )
    : TQPtrCollection( list )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex = new TQMutex(true);
#endif
    firstNode = lastNode = curNode = 0;		// initialize list
    numNodes  = 0;
    curIndex  = -1;
    iterators = 0;				// initialize iterator list
    TQLNode *n = list.firstNode;
    while ( n ) {				// copy all items from list
	append( n->data );
	n = n->next;
    }
}

/*!
  Removes all items from the list and destroys the list.
*/

TQGList::~TQGList()
{
    clear();
    delete iterators;
    // Workaround for GCC 2.7.* bug. Compiler constructs 'static' TQGList
    // instances twice on the same address and therefore tries to destruct
    // twice on the same address! This is insane but let's try not to crash
    // here.
    iterators = 0;
#if defined(TQT_THREAD_SUPPORT)
    //delete mutex;
#endif
}


/*!
  Assigns \a list to this list.
*/

TQGList& TQGList::operator=( const TQGList &list )
{
    if ( &list == this )
	return *this;

    clear();
    if ( list.count() > 0 ) {
	TQLNode *n = list.firstNode;
	while ( n ) {				// copy all items from list
	    append( n->data );
	    n = n->next;
	}
	curNode	 = firstNode;
	curIndex = 0;
    }
    return *this;
}

/*!
  Compares this list with \a list. Returns true if the lists
  contain the same data, otherwise false.
*/

bool TQGList::operator==( const TQGList &list ) const
{
    if ( count() != list.count() ) {
	return false;
    }

    if ( count() == 0 ) {
	return true;
    }

    TQLNode *n1 = firstNode;
    TQLNode *n2 = list.firstNode;
    while ( n1 && n2 ) {
	// should be mutable
	if ( ( (TQGList*)this )->compareItems( n1->data, n2->data ) != 0 )
	    return false;
	n1 = n1->next;
	n2 = n2->next;
    }

    return true;
}

/*!
  \fn uint TQGList::count() const

  Returns the number of items in the list.
*/


/*!
  Returns the node at position \a index.  Sets this node to current.
*/

TQLNode *TQGList::locate( uint index )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    if ( index == (uint)curIndex ) {		// current node ?
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return curNode;
    }
    if ( !curNode && firstNode ) {		// set current node
	curNode	 = firstNode;
	curIndex = 0;
    }
    TQLNode *node;
    int	 distance = index - curIndex;		// node distance to cur node
    bool forward;				// direction to traverse

    if ( index >= numNodes ) {
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return 0;
    }

    if ( distance < 0 ) {
	distance = -distance;
    }
    if ( (uint)distance < index && (uint)distance < numNodes - index ) {
	node = curNode;				// start from current node
	forward = index > (uint)curIndex;
    } else if ( index < numNodes - index ) {	// start from first node
	node = firstNode;
	distance = index;
	forward = true;
    } else {					// start from last node
	node = lastNode;
	distance = numNodes - index - 1;
	if ( distance < 0 )
	    distance = 0;
	forward = false;
    }
    if ( forward ) {				// now run through nodes
	while ( distance-- ) {
	    node = node->next;
	}
    } else {
	while ( distance-- ) {
	    node = node->prev;
	}
    }
    curIndex = index;				// must update index
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return curNode = node;
}


/*!
  Inserts item \a d at its sorted position in the list.
*/

void TQGList::inSort( TQPtrCollection::Item d )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    int index = 0;
    TQLNode *n = firstNode;
    while ( n && compareItems(n->data,d) < 0 ){ // find position in list
	n = n->next;
	index++;
    }
    insertAt( index, d );
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
}


/*!
  Inserts item \a d at the start of the list.
*/

void TQGList::prepend( TQPtrCollection::Item d )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    TQLNode *n = new TQLNode( newItem(d) );
    TQ_CHECK_PTR( n );
    n->prev = 0;
    if ( (n->next = firstNode) )		// list is not empty
	firstNode->prev = n;
    else					// initialize list
	lastNode = n;
    firstNode = curNode = n;			// curNode affected
    numNodes++;
    curIndex = 0;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
}


/*!
  Inserts item \a d at the end of the list.
*/

void TQGList::append( TQPtrCollection::Item d )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    TQLNode *n = new TQLNode( newItem(d) );
    TQ_CHECK_PTR( n );
    n->next = 0;
    if ( (n->prev = lastNode) ) {		// list is not empty
	lastNode->next = n;
    }
    else {					// initialize list
	firstNode = n;
    }
    lastNode = curNode = n;			// curNode affected
    curIndex = numNodes;
    numNodes++;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
}


/*!
  Inserts item \a d at position \a index in the list.
*/

bool TQGList::insertAt( uint index, TQPtrCollection::Item d )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    if ( index == 0 ) {
	prepend( d );
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return true;
    }
    else if ( index == numNodes ) {
	append( d );
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return true;
    }
    TQLNode *nextNode = locate( index );
    if ( !nextNode ) {
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return false;
    }
    TQLNode *prevNode = nextNode->prev;
    TQLNode *n = new TQLNode( newItem(d) );
    TQ_CHECK_PTR( n );
    nextNode->prev = n;
    Q_ASSERT( (!((curIndex > 0) && (!prevNode))) );
    prevNode->next = n;
    n->prev = prevNode;				// link new node into list
    n->next = nextNode;
    curNode = n;				// curIndex set by locate()
    numNodes++;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return true;
}


/*!
  Relinks node \a n and makes it the first node in the list.
*/

void TQGList::relinkNode( TQLNode *n )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    if ( n == firstNode ) {			// already first
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return;
    }
    curNode = n;
    unlink();
    n->prev = 0;
    if ( (n->next = firstNode) ) {		// list is not empty
	firstNode->prev = n;
    }
    else {					// initialize list
	lastNode = n;
    }
    firstNode = curNode = n;			// curNode affected
    numNodes++;
    curIndex = 0;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
}


/*!
  Unlinks the current list node and returns a pointer to this node.
*/

TQLNode *TQGList::unlink()
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    if ( curNode == 0 ) {			// null current node
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return 0;
    }
    TQLNode *n = curNode;		// unlink this node
    if ( n == firstNode ) {			// removing first node ?
	if ( (firstNode = n->next) ) {
	    firstNode->prev = 0;
	} else {
	    lastNode = curNode = 0;		// list becomes empty
	    curIndex = -1;
	}
    } else {
	if ( n == lastNode ) {			// removing last node ?
	    lastNode = n->prev;
	    lastNode->next = 0;
	} else {				// neither last nor first node
	    n->prev->next = n->next;
	    n->next->prev = n->prev;
	}
    }

    if ( n->next ) {                            // change current node
        curNode = n->next;
    } else if ( n->prev ) {
        curNode = n->prev;
        curIndex--;
    }

    if ( iterators ) {
	iterators->notifyRemove( n, curNode );
    }
    numNodes--;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return n;
}


/*!
  Removes the node \a n from the list.
*/

bool TQGList::removeNode( TQLNode *n )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
#if defined(QT_CHECK_NULL)
    if ( n == 0 || (n->prev && n->prev->next != n) ||
	 (n->next && n->next->prev != n) ) {
	tqWarning( "TQGList::removeNode: Corrupted node" );
	return false;
    }
#endif
    curNode = n;
    unlink();					// unlink node
    deleteItem( n->data );			// deallocate this node
    delete n;
    curNode  = firstNode;
    curIndex = curNode ? 0 : -1;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return true;
}

/*!
  Removes the item \a d from the list.	Uses compareItems() to find the item.

  If \a d is 0, removes the current item.
*/

bool TQGList::remove( TQPtrCollection::Item d )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    if ( d && find(d) == -1 ) {
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return false;
    }
    TQLNode *n = unlink();
    if ( !n ) {
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return false;
    }
    deleteItem( n->data );
    delete n;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return true;
}

/*!
  Removes the item \a d from the list.
*/

bool TQGList::removeRef( TQPtrCollection::Item d )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    if ( findRef(d) == -1 ) {
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return false;
    }
    TQLNode *n = unlink();
    if ( !n ) {
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return false;
    }
    deleteItem( n->data );
    delete n;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return true;
}

/*!
  \fn bool TQGList::removeFirst()

  Removes the first item in the list.
*/

/*!
  \fn bool TQGList::removeLast()

  Removes the last item in the list.
*/

/*!
  Removes the item at position \a index from the list.
*/

bool TQGList::removeAt( uint index )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    if ( !locate(index) ) {
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return false;
    }
    TQLNode *n = unlink();
    if ( !n ) {
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return false;
    }
    deleteItem( n->data );
    delete n;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return true;
}


/*!
  Replaces the item at index \a index with \a d.
*/
bool TQGList::replaceAt( uint index, TQPtrCollection::Item d )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    TQLNode *n = locate( index );
    if ( !n ) {
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return false;
    }
    if ( n->data != d ) {
	deleteItem( n->data );
	n->data = newItem( d );
    }
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return true;
}



/*!
  Takes the node \a n out of the list.
*/

TQPtrCollection::Item TQGList::takeNode( TQLNode *n )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
#if defined(QT_CHECK_NULL)
    if ( n == 0 || (n->prev && n->prev->next != n) ||
	 (n->next && n->next->prev != n) ) {
	tqWarning( "TQGList::takeNode: Corrupted node" );
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return 0;
    }
#endif
    curNode = n;
    unlink();					// unlink node
    Item d = n->data;
    delete n;					// delete the node, not data
    curNode  = firstNode;
    curIndex = curNode ? 0 : -1;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return d;
}

/*!
  Takes the current item out of the list.
*/

TQPtrCollection::Item TQGList::take()
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    TQLNode *n = unlink();			// unlink node
    Item d = n ? n->data : 0;
    delete n;					// delete node, keep contents
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return d;
}

/*!
  Takes the item at position \a index out of the list.
*/

TQPtrCollection::Item TQGList::takeAt( uint index )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    if ( !locate(index) ) {
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return 0;
    }
    TQLNode *n = unlink();			// unlink node
    Item d = n ? n->data : 0;
    delete n;					// delete node, keep contents
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return d;
}

/*!
  Takes the first item out of the list.
*/

TQPtrCollection::Item TQGList::takeFirst()
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    first();
    TQLNode *n = unlink();			// unlink node
    Item d = n ? n->data : 0;
    delete n;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return d;
}

/*!
  Takes the last item out of the list.
*/

TQPtrCollection::Item TQGList::takeLast()
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    last();
    TQLNode *n = unlink();			// unlink node
    Item d = n ? n->data : 0;
    delete n;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return d;
}


/*!
  Removes all items from the list.
*/

void TQGList::clear()
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    TQLNode *n = firstNode;

    firstNode = lastNode = curNode = 0;		// initialize list
    numNodes = 0;
    curIndex = -1;

    if ( iterators ) {
	iterators->notifyClear( false );
    }

    TQLNode *prevNode;
    while ( n ) {				// for all nodes ...
	deleteItem( n->data );			// deallocate data
	prevNode = n;
	n = n->next;
	delete prevNode;			// deallocate node
    }
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
}


/*!
  Finds item \a d in the list. If \a fromStart is true the search
  begins at the first node; otherwise it begins at the current node.
*/

int TQGList::findRef( TQPtrCollection::Item d, bool fromStart )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    TQLNode *n;
    int	     index;
    if ( fromStart ) {				// start from first node
	n = firstNode;
	index = 0;
    } else {					// start from current node
	n = curNode;
	index = curIndex;
    }
    while ( n && n->data != d ) {		// find exact match
	n = n->next;
	index++;
    }
    curNode = n;
    curIndex = n ? index : -1;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return curIndex;				// return position of item
}

/*!
  Finds item \a d in the list using compareItems(). If \a fromStart is
  true the search begins at the first node; otherwise it begins at the
  current node.
*/

int TQGList::find( TQPtrCollection::Item d, bool fromStart )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    TQLNode *n;
    int	     index;
    if ( fromStart ) {				// start from first node
	n = firstNode;
	index = 0;
    } else {					// start from current node
	n = curNode;
	index = curIndex;
    }
    while ( n && compareItems(n->data,d) ){	// find equal match
	n = n->next;
	index++;
    }
    curNode = n;
    curIndex = n ? index : -1;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return curIndex;				// return position of item
}


/*!
  Counts the number item \a d occurs in the list.
*/

uint TQGList::containsRef( TQPtrCollection::Item d ) const
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    TQLNode *n = firstNode;
    uint     count = 0;
    while ( n ) {				// for all nodes...
	if ( n->data == d )			// count # exact matches
	    count++;
	n = n->next;
    }
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return count;
}

/*!
  Counts the number of times item \a d occurs in the list. Uses
  compareItems().
*/

uint TQGList::contains( TQPtrCollection::Item d ) const
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    TQLNode *n = firstNode;
    uint     count = 0;
    TQGList  *that = (TQGList*)this;		// mutable for compareItems()
    while ( n ) {				// for all nodes...
	if ( !that->compareItems(n->data,d) )	// count # equal matches
	    count++;
	n = n->next;
    }
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return count;
}


/*!
  \overload TQPtrCollection::Item TQGList::at( uint index )

  Sets the item at position \a index to the current item.
*/

/*!
  \fn int TQGList::at() const

  Returns the current index.
*/

/*!
  \fn TQLNode *TQGList::currentNode() const

  Returns the current node.
*/

/*!
  \fn TQPtrCollection::Item TQGList::get() const

  Returns the current item.
*/

/*!
  \fn TQPtrCollection::Item TQGList::cfirst() const

  Returns the first item in the list.
*/

/*!
  \fn TQPtrCollection::Item TQGList::clast() const

  Returns the last item in the list.
*/


/*!
  Returns the first list item.	Sets this to current.
*/

TQPtrCollection::Item TQGList::first()
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    if ( firstNode ) {
	curIndex = 0;
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return (curNode=firstNode)->data;
    }
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return 0;
}

/*!
  Returns the last list item.  Sets this to current.
*/

TQPtrCollection::Item TQGList::last()
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    if ( lastNode ) {
	curIndex = numNodes-1;
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return (curNode=lastNode)->data;
    }
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return 0;
}

/*!
  Returns the next list item (after current).  Sets this to current.
*/

TQPtrCollection::Item TQGList::next()
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    if ( curNode ) {
	if ( curNode->next ) {
	    curIndex++;
	    curNode = curNode->next;
#if defined(TQT_THREAD_SUPPORT)
	    //mutex->unlock();
#endif
	    return curNode->data;
	}
	curIndex = -1;
	curNode = 0;
    }
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return 0;
}

/*!
  Returns the previous list item (before current).  Sets this to current.
*/

TQPtrCollection::Item TQGList::prev()
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    if ( curNode ) {
	if ( curNode->prev ) {
	    curIndex--;
	    curNode = curNode->prev;
#if defined(TQT_THREAD_SUPPORT)
	    //mutex->unlock();
#endif
	    return curNode->data;
	}
	curIndex = -1;
	curNode = 0;
    }
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return 0;
}


/*!
  Converts the list to a vector, \a vector.
*/

void TQGList::toVector( TQGVector *vector ) const
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    vector->clear();
    if ( !vector->resize( count() ) ) {
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return;
    }
    TQLNode *n = firstNode;
    uint i = 0;
    while ( n ) {
	vector->insert( i, n->data );
	n = n->next;
	i++;
    }
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
}

void TQGList::heapSortPushDown( TQPtrCollection::Item* heap, int first, int last )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    int r = first;
    while( r <= last/2 ) {
	// Node r has only one child ?
	if ( last == 2*r ) {
	    // Need for swapping ?
	    if ( compareItems( heap[r], heap[ 2*r ] ) > 0 ) {
		TQPtrCollection::Item tmp = heap[r];
		heap[ r ] = heap[ 2*r ];
		heap[ 2*r ] = tmp;
	    }
	    // That's it ...
	    r = last;
	} else {
	    // Node has two children
	    if ( compareItems( heap[r], heap[ 2*r ] ) > 0 &&
		 compareItems( heap[ 2*r ], heap[ 2*r+1 ] ) <= 0 ) {
		// Swap with left child
		TQPtrCollection::Item tmp = heap[r];
		heap[ r ] = heap[ 2*r ];
		heap[ 2*r ] = tmp;
		r *= 2;
	    } else if ( compareItems( heap[r], heap[ 2*r+1 ] ) > 0 &&
			compareItems( heap[ 2*r+1 ], heap[ 2*r ] ) < 0 ) {
		// Swap with right child
		TQPtrCollection::Item tmp = heap[r];
		heap[ r ] = heap[ 2*r+1 ];
		heap[ 2*r+1 ] = tmp;
		r = 2*r+1;
	    } else {
		// We are done
		r = last;
	    }
	}
    }
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
}


/*! Sorts the list by the result of the virtual compareItems() function.

  The Heap-Sort algorithm is used for sorting.  It sorts n items with
  O(n*log n) compares.  This is the asymptotic optimal solution of the
  sorting problem.
*/

void TQGList::sort()
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    uint n = count();
    if ( n < 2 ) {
#if defined(TQT_THREAD_SUPPORT)
	//mutex->unlock();
#endif
	return;
    }

    // Create the heap
    TQPtrCollection::Item* realheap = new TQPtrCollection::Item[ n ];
    // Wow, what a fake. But I want the heap to be indexed as 1...n
    TQPtrCollection::Item* heap = realheap - 1;
    int size = 0;
    TQLNode* insert = firstNode;
    for( ; insert != 0; insert = insert->next ) {
	heap[++size] = insert->data;
	int i = size;
	while( i > 1 && compareItems( heap[i], heap[ i / 2 ] ) < 0 ) {
	    TQPtrCollection::Item tmp = heap[ i ];
	    heap[ i ] = heap[ i/2 ];
	    heap[ i/2 ] = tmp;
	    i /= 2;
	}
    }

    insert = firstNode;
    // Now do the sorting
    for ( int i = n; i > 0; i-- ) {
	insert->data = heap[1];
	insert = insert->next;
	if ( i > 1 ) {
	    heap[1] = heap[i];
	    heapSortPushDown( heap, 1, i - 1 );
	}
    }

    delete [] realheap;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
}


/*****************************************************************************
  TQGList stream functions
 *****************************************************************************/

#ifndef TQT_NO_DATASTREAM
TQDataStream &operator>>( TQDataStream &s, TQGList &list )
{						// read list
    return list.read( s );
}

TQDataStream &operator<<( TQDataStream &s, const TQGList &list )
{						// write list
    return list.write( s );
}

/*!
  Reads a list from the stream \a s.
*/

TQDataStream &TQGList::read( TQDataStream &s )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    uint num;
    s >> num;					// read number of items
    clear();					// clear list
    while ( num-- ) {				// read all items
	Item d;
	read( s, d );
	TQ_CHECK_PTR( d );
	if ( !d )				// no memory
	    break;
	TQLNode *n = new TQLNode( d );
	TQ_CHECK_PTR( n );
	if ( !n )				// no memory
	    break;
	n->next = 0;
	if ( (n->prev = lastNode) )		// list is not empty
	    lastNode->next = n;
	else					// initialize list
	    firstNode = n;
	lastNode = n;
	numNodes++;
    }
    curNode  = firstNode;
    curIndex = curNode ? 0 : -1;
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return s;
}

/*!
  Writes the list to the stream \a s.
*/

TQDataStream &TQGList::write( TQDataStream &s ) const
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    s << count();				// write number of items
    TQLNode *n = firstNode;
    while ( n ) {				// write all items
	write( s, n->data );
	n = n->next;
    }
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return s;
}

#endif // TQT_NO_DATASTREAM



/*! \internal
 */
TQLNode* TQGList::erase( TQLNode* it )
{
#if defined(TQT_THREAD_SUPPORT)
    //mutex->lock();
#endif
    TQLNode* n = it;
    it = it->next;
    removeNode( n );
#if defined(TQT_THREAD_SUPPORT)
    //mutex->unlock();
#endif
    return it;
}


/*****************************************************************************
  TQGListIterator member functions
 *****************************************************************************/

/*!
  \class TQGListIterator ntqglist.h
  \reentrant
  \ingroup collection
  \brief The TQGListIterator class is an internal class for implementing TQPtrListIterator.

  \internal

  TQGListIterator is a strictly internal class that does the heavy work for
  TQPtrListIterator.
*/

/*!
  \internal
  Constructs an iterator that operates on the list \a l.
*/

TQGListIterator::TQGListIterator( const TQGList &l )
{
    list = (TQGList *)&l;			// get reference to list
    curNode = list->firstNode;			// set to first node
    if ( !list->iterators ) {
	list->iterators = new TQGListIteratorList;		// create iterator list
	TQ_CHECK_PTR( list->iterators );
    }
    list->iterators->add( this );		// attach iterator to list
}

/*!
  \internal
  Constructs a copy of the iterator \a it.
*/

TQGListIterator::TQGListIterator( const TQGListIterator &it )
{
    list = it.list;
    curNode = it.curNode;
    if ( list )
	list->iterators->add( this );	// attach iterator to list
}

/*!
  \internal
  Assigns a copy of the iterator \a it and returns a reference to this
  iterator.
*/

TQGListIterator &TQGListIterator::operator=( const TQGListIterator &it )
{
    if ( list )					// detach from old list
	list->iterators->remove( this );
    list = it.list;
    curNode = it.curNode;
    if ( list )
	list->iterators->add( this );	// attach to new list
    return *this;
}

/*!
  \internal
  Destroys the iterator.
*/

TQGListIterator::~TQGListIterator()
{
    if ( list )					// detach iterator from list
	list->iterators->remove(this);
}


/*!
  \fn bool TQGListIterator::atFirst() const
  \internal
  Returns true if the iterator points to the first item, otherwise false.
*/

/*!
  \fn bool TQGListIterator::atLast() const
  \internal
  Returns true if the iterator points to the last item, otherwise false.
*/


/*!
  \internal
  Sets the list iterator to point to the first item in the list.
*/

TQPtrCollection::Item TQGListIterator::toFirst()
{
    if ( !list ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQGListIterator::toFirst: List has been deleted" );
#endif
	return 0;
    }
    return list->firstNode ? (curNode = list->firstNode)->getData() : 0;
}

/*!
  \internal
  Sets the list iterator to point to the last item in the list.
*/

TQPtrCollection::Item TQGListIterator::toLast()
{
    if ( !list ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQGListIterator::toLast: List has been deleted" );
#endif
	return 0;
    }
    return list->lastNode ? (curNode = list->lastNode)->getData() : 0;
}


/*!
  \fn TQPtrCollection::Item TQGListIterator::get() const
  \internal
  Returns the iterator item.
*/


/*!
  \internal
  Moves to the next item (postfix).
*/

TQPtrCollection::Item TQGListIterator::operator()()
{
    if ( !curNode )
	return 0;
    TQPtrCollection::Item d = curNode->getData();
    curNode = curNode->next;
    return  d;
}

/*!
  \internal
  Moves to the next item (prefix).
*/

TQPtrCollection::Item TQGListIterator::operator++()
{
    if ( !curNode )
	return 0;
    curNode = curNode->next;
    return curNode ? curNode->getData() : 0;
}

/*!
  \internal
  Moves \a jumps positions forward.
*/

TQPtrCollection::Item TQGListIterator::operator+=( uint jumps )
{
    while ( curNode && jumps-- )
	curNode = curNode->next;
    return curNode ? curNode->getData() : 0;
}

/*!
  \internal
  Moves to the previous item (prefix).
*/

TQPtrCollection::Item TQGListIterator::operator--()
{
    if ( !curNode )
	return 0;
    curNode = curNode->prev;
    return curNode ? curNode->getData() : 0;
}

/*!
  \internal
  Moves \a jumps positions backward.
*/

TQPtrCollection::Item TQGListIterator::operator-=( uint jumps )
{
    while ( curNode && jumps-- )
	curNode = curNode->prev;
    return curNode ? curNode->getData() : 0;
}
