/****************************************************************************
**
** Definition of TQGCache and TQGCacheIterator classes
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

#ifndef TQGCACHE_H
#define TQGCACHE_H

#ifndef QT_H
#include "ntqptrcollection.h"
#include "ntqglist.h"
#include "ntqgdict.h"
#endif // QT_H


class TQCList;					// internal classes
class TQCListIt;
class TQCDict;


class TQ_EXPORT TQGCache : public TQPtrCollection	// generic LRU cache
{
friend class TQGCacheIterator;
protected:
    enum KeyType { StringKey, AsciiKey, IntKey, PtrKey };
      // identical to TQGDict's, but PtrKey is not used at the moment

    TQGCache( int maxCost, uint size, KeyType kt, bool caseSensitive,
	     bool copyKeys );
    TQGCache( const TQGCache & );			// not allowed, calls fatal()
   ~TQGCache();
    TQGCache &operator=( const TQGCache & );	// not allowed, calls fatal()

    uint    count()	const;
    uint    size()	const;
    int	    maxCost()	const	{ return mCost; }
    int	    totalCost() const	{ return tCost; }
    void    setMaxCost( int maxCost );
    void    clear();

    bool    insert_string( const TQString &key, TQPtrCollection::Item,
			   int cost, int priority );
    bool    insert_other( const char *key, TQPtrCollection::Item,
			  int cost, int priority );
    bool    remove_string( const TQString &key );
    bool    remove_other( const char *key );
    TQPtrCollection::Item take_string( const TQString &key );
    TQPtrCollection::Item take_other( const char *key );

    TQPtrCollection::Item find_string( const TQString &key, bool ref=true ) const;
    TQPtrCollection::Item find_other( const char *key, bool ref=true ) const;

    void    statistics() const;

private:
    bool    makeRoomFor( int cost, int priority = -1 );
    KeyType keytype;
    TQCList *lruList;
    TQCDict *dict;
    int	    mCost;
    int	    tCost;
    bool    copyk;
};


class TQ_EXPORT TQGCacheIterator			// generic cache iterator
{
protected:
    TQGCacheIterator( const TQGCache & );
    TQGCacheIterator( const TQGCacheIterator & );
   ~TQGCacheIterator();
    TQGCacheIterator &operator=( const TQGCacheIterator & );

    uint	      count()   const;
    bool	      atFirst() const;
    bool	      atLast()  const;
    TQPtrCollection::Item toFirst();
    TQPtrCollection::Item toLast();

    TQPtrCollection::Item get() const;
    TQString	      getKeyString() const;
    const char       *getKeyAscii()  const;
    long	      getKeyInt()    const;

    TQPtrCollection::Item operator()();
    TQPtrCollection::Item operator++();
    TQPtrCollection::Item operator+=( uint );
    TQPtrCollection::Item operator--();
    TQPtrCollection::Item operator-=( uint );

protected:
    TQCListIt *it;				// iterator on cache list
};


#endif // TQGCACHE_H
