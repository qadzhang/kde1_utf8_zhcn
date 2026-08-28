/****************************************************************************
**
** Definition of TQPtrList template/macro class
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

#ifndef TQPTRLIST_H
#define TQPTRLIST_H

#ifndef QT_H
#include "ntqglist.h"
#endif // QT_H

template<class type>
class TQPtrListStdIterator : public TQGListStdIterator
{
public:
    inline TQPtrListStdIterator( TQLNode* n ): TQGListStdIterator(n) {}
    type *operator*() { return node ? (type *)node->getData() : 0; }
    inline TQPtrListStdIterator<type> operator++()
    { node = next(); return *this; }
    inline TQPtrListStdIterator<type> operator++(int)
    { TQLNode* n = node; node = next(); return TQPtrListStdIterator<type>( n ); }
    inline bool operator==( const TQPtrListStdIterator<type>& it ) const { return node == it.node; }
    inline bool operator!=( const TQPtrListStdIterator<type>& it ) const { return node != it.node; }
};


template<class type>
class TQPtrList
#ifdef Q_QDOC
	: public TQPtrCollection
#else
	: public TQGList
#endif
{
public:

    TQPtrList()				{}
    TQPtrList( const TQPtrList<type> &l ) : TQGList(l) {}
    ~TQPtrList()				{ clear(); }
    TQPtrList<type> &operator=(const TQPtrList<type> &l)
			{ return (TQPtrList<type>&)TQGList::operator=(l); }
    bool operator==( const TQPtrList<type> &list ) const
    { return TQGList::operator==( list ); }
    bool operator!=( const TQPtrList<type> &list ) const
    { return !TQGList::operator==( list ); }
    uint  count()   const		{ return TQGList::count(); }
    bool  isEmpty() const		{ return TQGList::count() == 0; }
    bool  insert( uint i, const type *d){ return TQGList::insertAt(i,(TQPtrCollection::Item)d); }
    void  inSort( const type *d )	{ TQGList::inSort((TQPtrCollection::Item)d); }
    void  prepend( const type *d )	{ TQGList::insertAt(0,(TQPtrCollection::Item)d); }
    void  append( const type *d )	{ TQGList::append((TQPtrCollection::Item)d); }
    bool  remove( uint i )		{ return TQGList::removeAt(i); }
    bool  remove()			{ return TQGList::remove((TQPtrCollection::Item)0); }
    bool  remove( const type *d )	{ return TQGList::remove((TQPtrCollection::Item)d); }
    bool  removeRef( const type *d )	{ return TQGList::removeRef((TQPtrCollection::Item)d); }
    void  removeNode( TQLNode *n )	{ TQGList::removeNode(n); }
    bool  removeFirst()			{ return TQGList::removeFirst(); }
    bool  removeLast()			{ return TQGList::removeLast(); }
    type *take( uint i )		{ return (type *)TQGList::takeAt(i); }
    type *take()			{ return (type *)TQGList::take(); }
    type *takeNode( TQLNode *n )		{ return (type *)TQGList::takeNode(n); }
    void  clear()			{ TQGList::clear(); }
    void  sort()			{ TQGList::sort(); }
    int	  find( const type *d )		{ return TQGList::find((TQPtrCollection::Item)d); }
    int	  findNext( const type *d )	{ return TQGList::find((TQPtrCollection::Item)d,false); }
    int	  findRef( const type *d )	{ return TQGList::findRef((TQPtrCollection::Item)d); }
    int	  findNextRef( const type *d ){ return TQGList::findRef((TQPtrCollection::Item)d,false);}
    uint  contains( const type *d ) const { return TQGList::contains((TQPtrCollection::Item)d); }
    uint  containsRef( const type *d ) const
					{ return TQGList::containsRef((TQPtrCollection::Item)d); }
    bool replace( uint i, const type *d ) { return TQGList::replaceAt( i, (TQPtrCollection::Item)d ); }
    type *operator[]( uint i )		{ return (type *)TQGList::at(i); }
    type *at( uint i )			{ return (type *)TQGList::at(i); }
    int	  at() const			{ return TQGList::at(); }
    type *current()  const		{ return (type *)TQGList::get(); }
    TQLNode *currentNode()  const	{ return TQGList::currentNode(); }
    type *getFirst() const		{ return (type *)TQGList::cfirst(); }
    type *getLast()  const		{ return (type *)TQGList::clast(); }
    type *first()			{ return (type *)TQGList::first(); }
    type *last()			{ return (type *)TQGList::last(); }
    type *next()			{ return (type *)TQGList::next(); }
    type *prev()			{ return (type *)TQGList::prev(); }
    void  toVector( TQGVector *vec )const{ TQGList::toVector(vec); }


    // standard iterators
    typedef TQPtrListStdIterator<type> Iterator;
    typedef TQPtrListStdIterator<type> ConstIterator;
    inline Iterator begin() { return TQGList::begin(); }
    inline ConstIterator begin() const { return TQGList::begin(); }
    inline ConstIterator constBegin() const { return TQGList::begin(); }
    inline Iterator end() { return TQGList::end(); }
    inline ConstIterator end() const { return TQGList::end(); }
    inline ConstIterator constEnd() const { return TQGList::end(); }
    inline Iterator erase( Iterator it ) { return TQGList::erase( it ); }
    // stl syntax compatibility
    typedef Iterator iterator;
    typedef ConstIterator const_iterator;


#ifdef Q_QDOC
protected:
    virtual int compareItems( TQPtrCollection::Item, TQPtrCollection::Item );
    virtual TQDataStream& read( TQDataStream&, TQPtrCollection::Item& );
    virtual TQDataStream& write( TQDataStream&, TQPtrCollection::Item ) const;
#endif

private:
    void  deleteItem( Item d );
};

template<> inline void TQPtrList<void>::deleteItem( TQPtrCollection::Item )
{
}

template<class type> inline void TQPtrList<type>::deleteItem( TQPtrCollection::Item d )
{
    if ( del_item ) delete (type *)d;
}

template<class type>
class TQPtrListIterator : public TQGListIterator
{
public:
    TQPtrListIterator(const TQPtrList<type> &l) :TQGListIterator((TQGList &)l) {}
   ~TQPtrListIterator()	      {}
    uint  count()   const     { return list->count(); }
    bool  isEmpty() const     { return list->count() == 0; }
    bool  atFirst() const     { return TQGListIterator::atFirst(); }
    bool  atLast()  const     { return TQGListIterator::atLast(); }
    type *toFirst()	      { return (type *)TQGListIterator::toFirst(); }
    type *toLast()	      { return (type *)TQGListIterator::toLast(); }
    operator type *() const   { return (type *)TQGListIterator::get(); }
    type *operator*()         { return (type *)TQGListIterator::get(); }

    // No good, since TQPtrList<char> (ie. TQStrList fails...
    //
    // MSVC++ gives warning
    // Sunpro C++ 4.1 gives error
    //    type *operator->()        { return (type *)TQGListIterator::get(); }

    type *current()   const   { return (type *)TQGListIterator::get(); }
    type *operator()()	      { return (type *)TQGListIterator::operator()();}
    type *operator++()	      { return (type *)TQGListIterator::operator++(); }
    type *operator+=(uint j)  { return (type *)TQGListIterator::operator+=(j);}
    type *operator--()	      { return (type *)TQGListIterator::operator--(); }
    type *operator-=(uint j)  { return (type *)TQGListIterator::operator-=(j);}
    TQPtrListIterator<type>& operator=(const TQPtrListIterator<type>&it)
			      { TQGListIterator::operator=(it); return *this; }
};

#define Q_DEFINED_QPTRLIST
#include "ntqwinexport.h"

#endif // TQPTRLIST_H
