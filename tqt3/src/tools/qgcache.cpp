/****************************************************************************
**
** Implementation of TQGCache and TQGCacheIterator classes
**
** Created : 950208
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

#include "ntqgcache.h"
#include "ntqptrlist.h"
#include "ntqdict.h"
#include "ntqstring.h"

/*!
  \class TQGCache ntqgcache.h
  \reentrant
  \ingroup shared
  \ingroup collection
  \brief The TQGCache class is an internal class for implementing TQCache
  template classes.

  \internal

  TQGCache is a strictly internal class that acts as a base class for the
  \link collection.html collection classes\endlink TQCache and TQIntCache.
*/


/*****************************************************************************
  TQGCacheItem class (internal cache item)
 *****************************************************************************/

struct TQCacheItem
{
    TQCacheItem( void *k, TQPtrCollection::Item d, int c, short p )
	: priority(p), skipPriority(p), cost(c), key(k), data(d), node(0) {}
    short	priority;
    short	skipPriority;
    int		cost;
    void       *key;
    TQPtrCollection::Item data;
    TQLNode     *node;
};


/*****************************************************************************
  TQCList class (internal list of cache items)
 *****************************************************************************/

class TQCList : private TQPtrList<TQCacheItem>
{
friend class TQGCacheIterator;
friend class TQCListIt;
public:
    TQCList()	{}
   ~TQCList();

    void	insert( TQCacheItem * );		// insert according to priority
    void	insert( int, TQCacheItem * );
    void	take( TQCacheItem * );
    void	reference( TQCacheItem * );

    void	setAutoDelete( bool del ) { TQPtrCollection::setAutoDelete(del); }

    bool	removeFirst()	{ return TQPtrList<TQCacheItem>::removeFirst(); }
    bool	removeLast()	{ return TQPtrList<TQCacheItem>::removeLast(); }

    TQCacheItem *first()		{ return TQPtrList<TQCacheItem>::first(); }
    TQCacheItem *last()		{ return TQPtrList<TQCacheItem>::last(); }
    TQCacheItem *prev()		{ return TQPtrList<TQCacheItem>::prev(); }
    TQCacheItem *next()		{ return TQPtrList<TQCacheItem>::next(); }

#if defined(QT_DEBUG)
    int		inserts;			// variables for statistics
    int		insertCosts;
    int		insertMisses;
    int		finds;
    int		hits;
    int		hitCosts;
    int		dumps;
    int		dumpCosts;
#endif
};


TQCList::~TQCList()
{
#if defined(QT_DEBUG)
    Q_ASSERT( count() == 0 );
#endif
}


void TQCList::insert( TQCacheItem *ci )
{
    TQCacheItem *item = first();
    while( item && item->skipPriority > ci->priority ) {
	item->skipPriority--;
	item = next();
    }
    if ( item )
	TQPtrList<TQCacheItem>::insert( at(), ci );
    else
	append( ci );
#if defined(QT_DEBUG)
    Q_ASSERT( ci->node == 0 );
#endif
    ci->node = currentNode();
}

inline void TQCList::insert( int i, TQCacheItem *ci )
{
    TQPtrList<TQCacheItem>::insert( i, ci );
#if defined(QT_DEBUG)
    Q_ASSERT( ci->node == 0 );
#endif
    ci->node = currentNode();
}


void TQCList::take( TQCacheItem *ci )
{
    if ( ci ) {
#if defined(QT_DEBUG)
	Q_ASSERT( ci->node != 0 );
#endif
	takeNode( ci->node );
	ci->node = 0;
    }
}


inline void TQCList::reference( TQCacheItem *ci )
{
#if defined(QT_DEBUG)
    Q_ASSERT( ci != 0 && ci->node != 0 );
#endif
    ci->skipPriority = ci->priority;
    relinkNode( ci->node );			// relink as first item
}


class TQCListIt: public TQPtrListIterator<TQCacheItem>
{
public:
    TQCListIt( const TQCList *p ): TQPtrListIterator<TQCacheItem>( *p ) {}
    TQCListIt( const TQCListIt *p ): TQPtrListIterator<TQCacheItem>( *p ) {}
};


/*****************************************************************************
  TQCDict class (internal dictionary of cache items)
 *****************************************************************************/

//
// Since we need to decide if the dictionary should use an int or const
// char * key (the "bool trivial" argument in the constructor below)
// we cannot use the macro/template dict, but inherit directly from TQGDict.
//

class TQCDict : public TQGDict
{
public:
    TQCDict( uint size, uint kt, bool caseSensitive, bool copyKeys )
	: TQGDict( size, (KeyType)kt, caseSensitive, copyKeys ) {}
    ~TQCDict();

    void clear() { TQGDict::clear(); }

    TQCacheItem *find_string(const TQString &key) const
	{ return (TQCacheItem*)((TQCDict*)this)->look_string(key, 0, 0); }
    TQCacheItem *find_ascii(const char *key) const
	{ return (TQCacheItem*)((TQCDict*)this)->look_ascii(key, 0, 0); }
    TQCacheItem *find_int(long key) const
	{ return (TQCacheItem*)((TQCDict*)this)->look_int(key, 0, 0); }

    TQCacheItem *take_string(const TQString &key)
	{ return (TQCacheItem*)TQGDict::take_string(key); }
    TQCacheItem *take_ascii(const char *key)
	{ return (TQCacheItem*)TQGDict::take_ascii(key); }
    TQCacheItem *take_int(long key)
	{ return (TQCacheItem*)TQGDict::take_int(key); }

    bool  insert_string( const TQString &key, const TQCacheItem *ci )
	{ return TQGDict::look_string(key,(Item)ci,1)!=0;}
    bool  insert_ascii( const char *key, const TQCacheItem *ci )
	{ return TQGDict::look_ascii(key,(Item)ci,1)!=0;}
    bool  insert_int( long key, const TQCacheItem *ci )
	{ return TQGDict::look_int(key,(Item)ci,1)!=0;}

    bool  remove_string( TQCacheItem *item )
	{ return TQGDict::remove_string(*((TQString*)(item->key)),item); }
    bool  remove_ascii( TQCacheItem *item )
	{ return TQGDict::remove_ascii((const char *)item->key,item); }
    bool  remove_int( TQCacheItem *item )
	{ return TQGDict::remove_int((long)item->key,item);}

    void  statistics()			{ TQGDict::statistics(); }

private:
    void deleteItem( void *item )
	{ if ( del_item ) { TQCacheItem *d = (TQCacheItem*)item; delete d; } }
};

inline TQCDict::~TQCDict()
{
    clear();
}

/*****************************************************************************
  TQGDict member functions
 *****************************************************************************/

/*!
  Constructs a cache.
  The maximum cost of the cache is given by \a maxCost and the size by \a
  size. The key type is \a kt which may be \c StringKey, \c AsciiKey,
  \c IntKey or \c PtrKey. The case-sensitivity of lookups is set with
  \a caseSensitive. Keys are copied if \a copyKeys is true.
*/

TQGCache::TQGCache( int maxCost, uint size, KeyType kt, bool caseSensitive,
		  bool copyKeys )
{
    keytype = kt;
    lruList = new TQCList;
    TQ_CHECK_PTR( lruList );
    lruList->setAutoDelete( true );
    copyk   = ((keytype == AsciiKey) && copyKeys);
    dict    = new TQCDict( size, kt, caseSensitive, false );
    TQ_CHECK_PTR( dict );
    mCost   = maxCost;
    tCost   = 0;
#if defined(QT_DEBUG)
    lruList->inserts	  = 0;
    lruList->insertCosts  = 0;
    lruList->insertMisses = 0;
    lruList->finds	  = 0;
    lruList->hits	  = 0;
    lruList->hitCosts	  = 0;
    lruList->dumps	  = 0;
    lruList->dumpCosts	  = 0;
#endif
}

/*!
  Cannot copy a cache.
*/

TQGCache::TQGCache( const TQGCache & )
    : TQPtrCollection()
{
#if defined(QT_CHECK_NULL)
    tqFatal( "TQGCache::TQGCache(TQGCache &): Cannot copy a cache" );
#endif
}

/*!
  Removes all items from the cache and destroys it.
*/

TQGCache::~TQGCache()
{
    clear();
    delete dict;
    delete lruList;
}

/*!
  Cannot assign a cache.
*/

TQGCache &TQGCache::operator=( const TQGCache & )
{
#if defined(QT_CHECK_NULL)
    tqFatal( "TQGCache::operator=: Cannot copy a cache" );
#endif
    return *this;
}


/*!
  Returns the number of items in the cache.
*/

uint TQGCache::count() const
{
    return dict->count();
}

/*!
  Returns the size of the hash array.
*/

uint TQGCache::size() const
{
    return dict->size();
}

/*!
  \fn int TQGCache::maxCost() const

  Returns the maximum cache cost.
*/

/*!
  \fn int TQGCache::totalCost() const

  Returns the total cache cost.
*/

/*!
  Sets the maximum cache cost to \a maxCost.
*/

void TQGCache::setMaxCost( int maxCost )
{
    if ( maxCost < tCost ) {
	if ( !makeRoomFor(tCost - maxCost) )	// remove excess cost
	    return;
    }
    mCost = maxCost;
}


/*!
    Inserts an item with data \a data into the cache using key \a key.
    The item has cost \a cost and priority \a priority.

  \warning If this function returns false, you must delete \a data
  yourself.  Additionally, be very careful about using \a data after
  calling this function, as any other insertions into the cache, from
  anywhere in the application, or within TQt itself, could cause the
  data to be discarded from the cache, and the pointer to become
  invalid.
*/

bool TQGCache::insert_string( const TQString &key, TQPtrCollection::Item data,
			     int cost, int priority)
{
    if ( tCost + cost > mCost ) {
	if ( !makeRoomFor(tCost + cost - mCost, priority) ) {
#if defined(QT_DEBUG)
	    lruList->insertMisses++;
#endif
	    return false;
	}
    }
#if defined(QT_DEBUG)
    Q_ASSERT( keytype == StringKey );
    lruList->inserts++;
    lruList->insertCosts += cost;
#endif
    if ( priority < -32768 )
	priority = -32768;
    else if ( priority > 32767 )
	priority = 32677;
    TQCacheItem *ci = new TQCacheItem( new TQString(key), newItem(data),
				     cost, (short)priority );
    TQ_CHECK_PTR( ci );
    lruList->insert( 0, ci );
    dict->insert_string( key, ci );
    tCost += cost;
    return true;
}

bool TQGCache::insert_other( const char *key, TQPtrCollection::Item data,
			    int cost, int priority)
{
    if ( tCost + cost > mCost ) {
	if ( !makeRoomFor(tCost + cost - mCost, priority) ) {
#if defined(QT_DEBUG)
	    lruList->insertMisses++;
#endif
	    return false;
	}
    }
#if defined(QT_DEBUG)
    Q_ASSERT( keytype != StringKey );
    lruList->inserts++;
    lruList->insertCosts += cost;
#endif
    if ( keytype == AsciiKey && copyk )
	key = tqstrdup( key );
    if ( priority < -32768 )
	priority = -32768;
    else if ( priority > 32767 )
	priority = 32677;
    TQCacheItem *ci = new TQCacheItem( (void*)key, newItem(data), cost,
				     (short)priority );
    TQ_CHECK_PTR( ci );
    lruList->insert( 0, ci );
    if ( keytype == AsciiKey )
	dict->insert_ascii( key, ci );
    else
	dict->insert_int( (long)key, ci );
    tCost += cost;
    return true;
}


/*!
  Removes the item with key \a key from the cache. Returns true if the
  item was removed; otherwise returns false.
*/

bool TQGCache::remove_string( const TQString &key )
{
    Item d = take_string( key );
    if ( d )
	deleteItem( d );
    return d != 0;
}

bool TQGCache::remove_other( const char *key )
{
    Item d = take_other( key );
    if ( d )
	deleteItem( d );
    return d != 0;
}


/*!
  Takes the item with key \a key out of the cache. The item is not
  deleted. If no item has this \a key 0 is returned.
*/

TQPtrCollection::Item TQGCache::take_string( const TQString &key )
{
    TQCacheItem *ci = dict->take_string( key );	// take from dict
    Item d;
    if ( ci ) {
	d = ci->data;
	tCost -= ci->cost;
	lruList->take( ci );			// take from list
	delete (TQString*)ci->key;
	delete ci;
    } else {
	d = 0;
    }
    return d;
}

/*!
  Takes the item with key \a key out of the cache. The item is not
  deleted. If no item has this \a key 0 is returned.
*/

TQPtrCollection::Item TQGCache::take_other( const char *key )
{
    TQCacheItem *ci;
    if ( keytype == AsciiKey )
	ci = dict->take_ascii( key );
    else
	ci = dict->take_int( (long)key );
    Item d;
    if ( ci ) {
	d = ci->data;
	tCost -= ci->cost;
	lruList->take( ci );			// take from list
	if ( copyk )
	    delete [] (char *)ci->key;
	delete ci;
    } else {
	d = 0;
    }
    return d;
}


/*!
  Clears the cache.
*/

void TQGCache::clear()
{
    TQCacheItem *ci;
    while ( (ci = lruList->first()) ) {
	switch ( keytype ) {
	    case StringKey:
		dict->remove_string( ci );
		delete (TQString*)ci->key;
		break;
	    case AsciiKey:
		dict->remove_ascii( ci );
		if ( copyk )
		    delete [] (char*)ci->key;
		break;
	    case IntKey:
		dict->remove_int( ci );
		break;
	    case PtrKey:			// unused
		break;
	}
	deleteItem( ci->data );			// delete data
	lruList->removeFirst();			// remove from list
    }
    tCost = 0;
}


/*!
  Finds an item for \a key in the cache and adds a reference if \a ref is true.
*/

TQPtrCollection::Item TQGCache::find_string( const TQString &key, bool ref ) const
{
    TQCacheItem *ci = dict->find_string( key );
#if defined(QT_DEBUG)
    lruList->finds++;
#endif
    if ( ci ) {
#if defined(QT_DEBUG)
	lruList->hits++;
	lruList->hitCosts += ci->cost;
#endif
	if ( ref )
	    lruList->reference( ci );
	return ci->data;
    }
    return 0;
}


/*!
  Finds an item for \a key in the cache and adds a reference if \a ref is true.
*/

TQPtrCollection::Item TQGCache::find_other( const char *key, bool ref ) const
{
    TQCacheItem *ci = keytype == AsciiKey ? dict->find_ascii(key)
					 : dict->find_int((long)key);
#if defined(QT_DEBUG)
    lruList->finds++;
#endif
    if ( ci ) {
#if defined(QT_DEBUG)
	lruList->hits++;
	lruList->hitCosts += ci->cost;
#endif
	if ( ref )
	    lruList->reference( ci );
	return ci->data;
    }
    return 0;
}


/*!
  Allocates cache space for one or more items.
*/

bool TQGCache::makeRoomFor( int cost, int priority )
{
    if ( cost > mCost )				// cannot make room for more
	return false;				//   than maximum cost
    if ( priority == -1 )
	priority = 32767;
    TQCacheItem *ci = lruList->last();
    int cntCost = 0;
    int dumps	= 0;				// number of items to dump
    while ( cntCost < cost && ci && ci->skipPriority <= priority ) {
	cntCost += ci->cost;
	ci	 = lruList->prev();
	dumps++;
    }
    if ( cntCost < cost )			// can enough cost be dumped?
	return false;				// no
#if defined(QT_DEBUG)
    Q_ASSERT( dumps > 0 );
#endif
    while ( dumps-- ) {
	ci = lruList->last();
#if defined(QT_DEBUG)
	lruList->dumps++;
	lruList->dumpCosts += ci->cost;
#endif
	switch ( keytype ) {
	    case StringKey:
		dict->remove_string( ci );
		delete (TQString*)ci->key;
		break;
	    case AsciiKey:
		dict->remove_ascii( ci );
		if ( copyk )
		    delete [] (char *)ci->key;
		break;
	    case IntKey:
		dict->remove_int( ci );
		break;
	    case PtrKey:			// unused
		break;
	}
	deleteItem( ci->data );			// delete data
	lruList->removeLast();			// remove from list
    }
    tCost -= cntCost;
    return true;
}


/*!
  Outputs debug statistics.
*/

void TQGCache::statistics() const
{
#if defined(QT_DEBUG)
    TQString line;
    line.fill( '*', 80 );
    tqDebug( "%s", line.ascii() );
    tqDebug( "CACHE STATISTICS:" );
    tqDebug( "cache contains %d item%s, with a total cost of %d",
	   count(), count() != 1 ? "s" : "", tCost );
    tqDebug( "maximum cost is %d, cache is %d%% full.",
	   mCost, (200*tCost + mCost) / (mCost*2) );
    tqDebug( "find() has been called %d time%s",
	   lruList->finds, lruList->finds != 1 ? "s" : "" );
    tqDebug( "%d of these were hits, items found had a total cost of %d.",
	   lruList->hits,lruList->hitCosts );
    tqDebug( "%d item%s %s been inserted with a total cost of %d.",
	   lruList->inserts,lruList->inserts != 1 ? "s" : "",
	   lruList->inserts != 1 ? "have" : "has", lruList->insertCosts );
    tqDebug( "%d item%s %s too large or had too low priority to be inserted.",
	   lruList->insertMisses, lruList->insertMisses != 1 ? "s" : "",
	   lruList->insertMisses != 1 ? "were" : "was" );
    tqDebug( "%d item%s %s been thrown away with a total cost of %d.",
	   lruList->dumps, lruList->dumps != 1 ? "s" : "",
	   lruList->dumps != 1 ? "have" : "has", lruList->dumpCosts );
    tqDebug( "Statistics from internal dictionary class:" );
    dict->statistics();
    tqDebug( "%s", line.ascii() );
#endif
}


/*****************************************************************************
  TQGCacheIterator member functions
 *****************************************************************************/

/*!
  \class TQGCacheIterator ntqgcache.h
  \reentrant
  \ingroup shared
  \ingroup collection
  \brief The TQGCacheIterator class is an internal class for implementing TQCacheIterator and
  TQIntCacheIterator.

  \internal

  TQGCacheIterator is a strictly internal class that does the heavy work for
  TQCacheIterator and TQIntCacheIterator.
*/

/*!
  Constructs an iterator that operates on the cache \a c.
*/

TQGCacheIterator::TQGCacheIterator( const TQGCache &c )
{
    it = new TQCListIt( c.lruList );
#if defined(QT_DEBUG)
    Q_ASSERT( it != 0 );
#endif
}

/*!
  Constructs an iterator that operates on the same cache as \a ci.
*/

TQGCacheIterator::TQGCacheIterator( const TQGCacheIterator &ci )
{
    it = new TQCListIt( ci.it );
#if defined(QT_DEBUG)
    Q_ASSERT( it != 0 );
#endif
}

/*!
  Destroys the iterator.
*/

TQGCacheIterator::~TQGCacheIterator()
{
    delete it;
}

/*!
  Assigns the iterator \a ci to this cache iterator.
*/

TQGCacheIterator &TQGCacheIterator::operator=( const TQGCacheIterator &ci )
{
    *it = *ci.it;
    return *this;
}

/*!
  Returns the number of items in the cache.
*/

uint TQGCacheIterator::count() const
{
    return it->count();
}

/*!
  Returns true if the iterator points to the first item.
*/

bool  TQGCacheIterator::atFirst() const
{
    return it->atFirst();
}

/*!
  Returns true if the iterator points to the last item.
*/

bool TQGCacheIterator::atLast() const
{
    return it->atLast();
}

/*!
  Sets the list iterator to point to the first item in the cache.
*/

TQPtrCollection::Item TQGCacheIterator::toFirst()
{
    TQCacheItem *item = it->toFirst();
    return item ? item->data : 0;
}

/*!
  Sets the list iterator to point to the last item in the cache.
*/

TQPtrCollection::Item TQGCacheIterator::toLast()
{
    TQCacheItem *item = it->toLast();
    return item ? item->data : 0;
}

/*!
  Returns the current item.
*/

TQPtrCollection::Item TQGCacheIterator::get() const
{
    TQCacheItem *item = it->current();
    return item ? item->data : 0;
}

/*!
  Returns the key of the current item.
*/

TQString TQGCacheIterator::getKeyString() const
{
    TQCacheItem *item = it->current();
    return item ? *((TQString*)item->key) : TQString::null;
}

/*!
  Returns the key of the current item, as a \0-terminated C string.
*/

const char *TQGCacheIterator::getKeyAscii() const
{
    TQCacheItem *item = it->current();
    return item ? (const char *)item->key : 0;
}

/*!
  Returns the key of the current item, as a long.
*/

long TQGCacheIterator::getKeyInt() const
{
    TQCacheItem *item = it->current();
    return item ? (long)item->key : 0;
}

/*!
  Moves to the next item (postfix).
*/

TQPtrCollection::Item TQGCacheIterator::operator()()
{
    TQCacheItem *item = it->operator()();
    return item ? item->data : 0;
}

/*!
  Moves to the next item (prefix).
*/

TQPtrCollection::Item TQGCacheIterator::operator++()
{
    TQCacheItem *item = it->operator++();
    return item ? item->data : 0;
}

/*!
  Moves \a jump positions forward.
*/

TQPtrCollection::Item TQGCacheIterator::operator+=( uint jump )
{
    TQCacheItem *item = it->operator+=(jump);
    return item ? item->data : 0;
}

/*!
  Moves to the previous item (prefix).
*/

TQPtrCollection::Item TQGCacheIterator::operator--()
{
    TQCacheItem *item = it->operator--();
    return item ? item->data : 0;
}

/*!
  Moves \a jump positions backward.
*/

TQPtrCollection::Item TQGCacheIterator::operator-=( uint jump )
{
    TQCacheItem *item = it->operator-=(jump);
    return item ? item->data : 0;
}
