/****************************************************************************
**
** Definition of TQGList and TQGListIterator classes
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

#ifndef TQGLIST_H
#define TQGLIST_H

#ifndef QT_H
#include "ntqptrcollection.h"
#endif // QT_H

class TQ_EXPORT TQLNode
{
friend class TQGList;
friend class TQGListIterator;
friend class TQGListStdIterator;
public:
    TQPtrCollection::Item getData()	{ return data; }
private:
    TQPtrCollection::Item data;
    TQLNode *prev;
    TQLNode *next;
    TQLNode( TQPtrCollection::Item d ) { data = d; }
};

class TQMutex;
class TQGListPrivate;
class TQGListIteratorList; // internal helper class

class TQ_EXPORT TQGList : public TQPtrCollection	// doubly linked generic list
{
friend class TQGListIterator;
friend class TQGListIteratorList;
friend class TQGVector;				// needed by TQGVector::toList
public:
    uint  count() const;			// return number of nodes

#ifndef TQT_NO_DATASTREAM
    TQDataStream &read( TQDataStream & );		// read list from stream
    TQDataStream &write( TQDataStream & ) const;	// write list to stream
#endif
protected:
    TQGList();					// create empty list
    TQGList( const TQGList & );			// make copy of other list
    virtual ~TQGList();

    TQGList &operator=( const TQGList & );	// assign from other list
    bool operator==( const TQGList& ) const;

    void inSort( TQPtrCollection::Item );		// add item sorted in list
    void append( TQPtrCollection::Item );		// add item at end of list
    bool insertAt( uint index, TQPtrCollection::Item ); // add item at i'th position
    void relinkNode( TQLNode * );		// relink as first item
    bool removeNode( TQLNode * );		// remove node
    bool remove( TQPtrCollection::Item = 0 );	// remove item (0=current)
    bool removeRef( TQPtrCollection::Item = 0 );	// remove item (0=current)
    bool removeFirst();				// remove first item
    bool removeLast();				// remove last item
    bool removeAt( uint );			// remove item at i'th position
    bool replaceAt( uint, TQPtrCollection::Item ); // replace item at position i with item
    TQPtrCollection::Item takeNode( TQLNode * );	// take out node
    TQPtrCollection::Item take();		// take out current item
    TQPtrCollection::Item takeAt( uint index );	// take out item at i'th pos
    TQPtrCollection::Item takeFirst();		// take out first item
    TQPtrCollection::Item takeLast();		// take out last item

    void sort();                        	// sort all items;
    void clear();				// remove all items

    int	 findRef( TQPtrCollection::Item, bool = true ); // find exact item in list
    int	 find( TQPtrCollection::Item, bool = true ); // find equal item in list

    uint containsRef( TQPtrCollection::Item ) const;	// get number of exact matches
    uint contains( TQPtrCollection::Item ) const;	// get number of equal matches

    TQPtrCollection::Item at( uint index );	// access item at i'th pos
    int	  at() const;				// get current index
    TQLNode *currentNode() const;		// get current node

    TQPtrCollection::Item get() const;		// get current item

    TQPtrCollection::Item cfirst() const;	// get ptr to first list item
    TQPtrCollection::Item clast()  const;	// get ptr to last list item
    TQPtrCollection::Item first();		// set first item in list curr
    TQPtrCollection::Item last();		// set last item in list curr
    TQPtrCollection::Item next();		// set next item in list curr
    TQPtrCollection::Item prev();		// set prev item in list curr

    void  toVector( TQGVector * ) const;		// put items in vector

    virtual int compareItems( TQPtrCollection::Item, TQPtrCollection::Item );

#ifndef TQT_NO_DATASTREAM
    virtual TQDataStream &read( TQDataStream &, TQPtrCollection::Item & );
    virtual TQDataStream &write( TQDataStream &, TQPtrCollection::Item ) const;
#endif

    TQLNode* begin() const { return firstNode; }
    TQLNode* end() const { return 0; }
    TQLNode* erase( TQLNode* it );

private:
    void  prepend( TQPtrCollection::Item );	// add item at start of list

    void heapSortPushDown( TQPtrCollection::Item* heap, int first, int last );

    TQLNode *firstNode;				// first node
    TQLNode *lastNode;				// last node
    TQLNode *curNode;				// current node
    int curIndex;				// current index
    uint numNodes;				// number of nodes
    TQGListIteratorList *iterators; 		// list of iterators

    TQLNode *locate( uint );			// get node at i'th pos
    TQLNode *unlink();				// unlink node

#if defined(TQT_THREAD_SUPPORT)
//     TQMutex* mutex;
#endif

//     TQGListPrivate* d;
};


inline uint TQGList::count() const
{
    return numNodes;
}

inline bool TQGList::removeFirst()
{
    first();
    return remove();
}

inline bool TQGList::removeLast()
{
    last();
    return remove();
}

inline int TQGList::at() const
{
    return curIndex;
}

inline TQPtrCollection::Item TQGList::at( uint index )
{
    TQLNode *n = locate( index );
    return n ? n->data : 0;
}

inline TQLNode *TQGList::currentNode() const
{
    return curNode;
}

inline TQPtrCollection::Item TQGList::get() const
{
    return curNode ? curNode->data : 0;
}

inline TQPtrCollection::Item TQGList::cfirst() const
{
    return firstNode ? firstNode->data : 0;
}

inline TQPtrCollection::Item TQGList::clast() const
{
    return lastNode ? lastNode->data : 0;
}


/*****************************************************************************
  TQGList stream functions
 *****************************************************************************/

#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQGList & );
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQGList & );
#endif

/*****************************************************************************
  TQGListIterator class
 *****************************************************************************/

class TQ_EXPORT TQGListIterator			// TQGList iterator
{
friend class TQGList;
friend class TQGListIteratorList;
protected:
    TQGListIterator( const TQGList & );
    TQGListIterator( const TQGListIterator & );
    TQGListIterator &operator=( const TQGListIterator & );
   ~TQGListIterator();

    bool  atFirst() const;			// test if at first item
    bool  atLast()  const;			// test if at last item
    TQPtrCollection::Item	  toFirst();				// move to first item
    TQPtrCollection::Item	  toLast();				// move to last item

    TQPtrCollection::Item	  get() const;				// get current item
    TQPtrCollection::Item	  operator()();				// get current and move to next
    TQPtrCollection::Item	  operator++();				// move to next item (prefix)
    TQPtrCollection::Item	  operator+=(uint);			// move n positions forward
    TQPtrCollection::Item	  operator--();				// move to prev item (prefix)
    TQPtrCollection::Item	  operator-=(uint);			// move n positions backward

protected:
    TQGList *list;				// reference to list

private:
    TQLNode  *curNode;				// current node in list
};


inline bool TQGListIterator::atFirst() const
{
    return curNode == list->firstNode;
}

inline bool TQGListIterator::atLast() const
{
    return curNode == list->lastNode;
}

inline TQPtrCollection::Item TQGListIterator::get() const
{
    return curNode ? curNode->data : 0;
}

class TQ_EXPORT TQGListStdIterator
{
public:
    inline TQGListStdIterator( TQLNode* n ) : node( n ){}
    inline operator TQLNode* () { return node; }
protected:
    inline TQLNode *next() { return node->next; }
    TQLNode *node;
};


#endif	// TQGLIST_H
