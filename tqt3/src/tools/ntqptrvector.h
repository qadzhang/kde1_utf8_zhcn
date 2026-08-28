/****************************************************************************
**
** Definition of TQPtrVector pointer based template class
**
** Created : 930907
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

#ifndef TQPTRVECTOR_H
#define TQPTRVECTOR_H

#ifndef QT_H
#include "ntqgvector.h"
#endif // QT_H

template<class type>
class TQPtrVector
#ifdef Q_QDOC
	: public TQPtrCollection
#else
	: public TQGVector
#endif
{
public:
    TQPtrVector()				{ }
    TQPtrVector( uint size ) : TQGVector(size) { }
    TQPtrVector( const TQPtrVector<type> &v ) : TQGVector( v ) { }
    ~TQPtrVector()				{ clear(); }
    TQPtrVector<type> &operator=(const TQPtrVector<type> &v)
			{ return (TQPtrVector<type>&)TQGVector::operator=(v); }
    bool operator==( const TQPtrVector<type> &v ) const { return TQGVector::operator==(v); }
    type **data()   const		{ return (type **)TQGVector::data(); }
    uint  size()    const		{ return TQGVector::size(); }
    uint  count()   const		{ return TQGVector::count(); }
    bool  isEmpty() const		{ return TQGVector::count() == 0; }
    bool  isNull()  const		{ return TQGVector::size() == 0; }
    bool  resize( uint size )		{ return TQGVector::resize(size); }
    bool  insert( uint i, const type *d){ return TQGVector::insert(i,(Item)d); }
    bool  remove( uint i )		{ return TQGVector::remove(i); }
    type *take( uint i )		{ return (type *)TQGVector::take(i); }
    void  clear()			{ TQGVector::clear(); }
    bool  fill( const type *d, int size=-1 )
					{ return TQGVector::fill((Item)d,size);}
    void  sort()			{ TQGVector::sort(); }
    int	  bsearch( const type *d ) const{ return TQGVector::bsearch((Item)d); }
    int	  findRef( const type *d, uint i=0 ) const
					{ return TQGVector::findRef((Item)d,i);}
    int	  find( const type *d, uint i= 0 ) const
					{ return TQGVector::find((Item)d,i); }
    uint  containsRef( const type *d ) const
				{ return TQGVector::containsRef((Item)d); }
    uint  contains( const type *d ) const
					{ return TQGVector::contains((Item)d); }
    type *operator[]( int i ) const	{ return (type *)TQGVector::at(i); }
    type *at( uint i ) const		{ return (type *)TQGVector::at(i); }
    void  toList( TQGList *list ) const	{ TQGVector::toList(list); }

#ifdef Q_QDOC
protected:
    virtual int compareItems( TQPtrCollection::Item d1, TQPtrCollection::Item d2 );
    virtual TQDataStream& read( TQDataStream &s, TQPtrCollection::Item &d );
    virtual TQDataStream& write( TQDataStream &s, TQPtrCollection::Item d ) const;
#endif

private:
    void  deleteItem( Item d );
};

template<> inline void TQPtrVector<void>::deleteItem( TQPtrCollection::Item )
{
}

template<class type> inline void TQPtrVector<type>::deleteItem( TQPtrCollection::Item d )
{
    if ( del_item ) delete (type *)d;
}

#define Q_DEFINED_QPTRVECTOR
#include "ntqwinexport.h"
#endif // TQPTRVECTOR_H
