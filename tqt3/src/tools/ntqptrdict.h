/****************************************************************************
**
** Definition of TQPtrDict template class
**
** Created : 970415
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

#ifndef TQPTRDICT_H
#define TQPTRDICT_H

#ifndef QT_H
#include "ntqgdict.h"
#endif // QT_H

template<class type>
class TQPtrDict
#ifdef Q_QDOC
	: public TQPtrCollection
#else
	: public TQGDict
#endif
{
public:
    TQPtrDict(int size=17) : TQGDict(size,PtrKey,0,0) {}
    TQPtrDict( const TQPtrDict<type> &d ) : TQGDict(d) {}
   ~TQPtrDict()				{ clear(); }
    TQPtrDict<type> &operator=(const TQPtrDict<type> &d)
			{ return (TQPtrDict<type>&)TQGDict::operator=(d); }
    uint  count()   const		{ return TQGDict::count(); }
    uint  size()    const		{ return TQGDict::size(); }
    bool  isEmpty() const		{ return TQGDict::count() == 0; }
    void  insert( void *k, const type *d )
					{ TQGDict::look_ptr(k,(Item)d,1); }
    void  replace( void *k, const type *d )
					{ TQGDict::look_ptr(k,(Item)d,2); }
    bool  remove( void *k )		{ return TQGDict::remove_ptr(k); }
    type *take( void *k )		{ return (type*)TQGDict::take_ptr(k); }
    type *find( void *k ) const
		{ return (type *)((TQGDict*)this)->TQGDict::look_ptr(k,0,0); }
    type *operator[]( void *k ) const
		{ return (type *)((TQGDict*)this)->TQGDict::look_ptr(k,0,0); }
    void  clear()			{ TQGDict::clear(); }
    void  resize( uint n )		{ TQGDict::resize(n); }
    void  statistics() const		{ TQGDict::statistics(); }

#ifdef Q_QDOC
protected:
    virtual TQDataStream& read( TQDataStream &, TQPtrCollection::Item & );
    virtual TQDataStream& write( TQDataStream &, TQPtrCollection::Item ) const;
#endif

private:
    void  deleteItem( Item d );
};

template<> inline void TQPtrDict<void>::deleteItem( TQPtrCollection::Item )
{
}

template<class type>
inline void TQPtrDict<type>::deleteItem( TQPtrCollection::Item d )
{
    if ( del_item ) delete (type *)d;
}

template<class type>
class TQPtrDictIterator : public TQGDictIterator
{
public:
    TQPtrDictIterator(const TQPtrDict<type> &d) :TQGDictIterator((TQGDict &)d) {}
   ~TQPtrDictIterator()	      {}
    uint  count()   const     { return dict->count(); }
    bool  isEmpty() const     { return dict->count() == 0; }
    type *toFirst()	      { return (type *)TQGDictIterator::toFirst(); }
    operator type *()  const  { return (type *)TQGDictIterator::get(); }
    type *current()    const  { return (type *)TQGDictIterator::get(); }
    void *currentKey() const  { return TQGDictIterator::getKeyPtr(); }
    type *operator()()	      { return (type *)TQGDictIterator::operator()(); }
    type *operator++()	      { return (type *)TQGDictIterator::operator++(); }
    type *operator+=(uint j)  { return (type *)TQGDictIterator::operator+=(j);}
};

#endif // TQPTRDICT_H
