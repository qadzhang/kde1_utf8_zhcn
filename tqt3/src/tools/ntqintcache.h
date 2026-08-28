/****************************************************************************
**
** Definition of TQIntCache template class
**
** Created : 950209
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

#ifndef TQINTCACHE_H
#define TQINTCACHE_H

#ifndef QT_H
#include "ntqgcache.h"
#endif // QT_H


template<class type> 
class TQIntCache
#ifdef Q_QDOC
	: public TQPtrCollection
#else
	: public TQGCache
#endif
{
public:
    TQIntCache( const TQIntCache<type> &c ) : TQGCache(c) {}
    TQIntCache( int maxCost=100, int size=17 )
	: TQGCache( maxCost, size, IntKey, false, false ) {}
   ~TQIntCache()		{ clear(); }
    TQIntCache<type> &operator=( const TQIntCache<type> &c )
			{ return (TQIntCache<type>&)TQGCache::operator=(c); }
    int	  maxCost()   const	{ return TQGCache::maxCost(); }
    int	  totalCost() const	{ return TQGCache::totalCost(); }
    void  setMaxCost( int m)	{ TQGCache::setMaxCost(m); }
    uint  count()     const	{ return TQGCache::count(); }
    uint  size()      const	{ return TQGCache::size(); }
    bool  isEmpty()   const	{ return TQGCache::count() == 0; }
    bool  insert( long k, const type *d, int c=1, int p=0 )
		{ return TQGCache::insert_other((const char*)k,(Item)d,c,p); }
    bool  remove( long k )
		{ return TQGCache::remove_other((const char*)k); }
    type *take( long k )
		{ return (type *)TQGCache::take_other((const char*)k);}
    void  clear()		{ TQGCache::clear(); }
    type *find( long k, bool ref=true ) const
		{ return (type *)TQGCache::find_other( (const char*)k,ref);}
    type *operator[]( long k ) const
		{ return (type *)TQGCache::find_other( (const char*)k); }
    void  statistics() const { TQGCache::statistics(); }
private:
	void  deleteItem( Item d );
};

template<> inline void TQIntCache<void>::deleteItem( TQPtrCollection::Item )
{
}

template<class type> inline void TQIntCache<type>::deleteItem( TQPtrCollection::Item d )
{
    if ( del_item ) delete (type *)d;
}

template<class type> 
class TQIntCacheIterator : public TQGCacheIterator
{
public:
    TQIntCacheIterator( const TQIntCache<type> &c )
	: TQGCacheIterator( (TQGCache &)c ) {}
    TQIntCacheIterator( const TQIntCacheIterator<type> &ci )
			      : TQGCacheIterator((TQGCacheIterator &)ci) {}
    TQIntCacheIterator<type> &operator=( const TQIntCacheIterator<type>&ci )
	{ return ( TQIntCacheIterator<type>&)TQGCacheIterator::operator=( ci );}
    uint  count()   const     { return TQGCacheIterator::count(); }
    bool  isEmpty() const     { return TQGCacheIterator::count() == 0; }
    bool  atFirst() const     { return TQGCacheIterator::atFirst(); }
    bool  atLast()  const     { return TQGCacheIterator::atLast(); }
    type *toFirst()	      { return (type *)TQGCacheIterator::toFirst(); }
    type *toLast()	      { return (type *)TQGCacheIterator::toLast(); }
    operator type *()  const  { return (type *)TQGCacheIterator::get(); }
    type *current()    const  { return (type *)TQGCacheIterator::get(); }
    long  currentKey() const  { return (long)TQGCacheIterator::getKeyInt();}
    type *operator()()	      { return (type *)TQGCacheIterator::operator()();}
    type *operator++()	      { return (type *)TQGCacheIterator::operator++(); }
    type *operator+=(uint j)  { return (type *)TQGCacheIterator::operator+=(j);}
    type *operator--()	      { return (type *)TQGCacheIterator::operator--(); }
    type *operator-=(uint j)  { return (type *)TQGCacheIterator::operator-=(j);}
};


#endif // TQINTCACHE_H
