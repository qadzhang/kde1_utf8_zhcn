/****************************************************************************
**
** Definition of TQAsciiDict template class
**
** Created : 920821
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

#ifndef TQASCIIDICT_H
#define TQASCIIDICT_H

#ifndef QT_H
#include "ntqgdict.h"
#endif // QT_H

template<class type>
class TQAsciiDict
#ifdef Q_QDOC
	: public TQPtrCollection
#else
	: public TQGDict
#endif
{
public:
    TQAsciiDict(int size=17, bool caseSensitive=true, bool copyKeys=true )
	: TQGDict(size,AsciiKey,caseSensitive,copyKeys) {}
    TQAsciiDict( const TQAsciiDict<type> &d ) : TQGDict(d) {}
   ~TQAsciiDict()			{ clear(); }
    TQAsciiDict<type> &operator=(const TQAsciiDict<type> &d)
			{ return (TQAsciiDict<type>&)TQGDict::operator=(d); }
    uint  count()   const		{ return TQGDict::count(); }
    uint  size()    const		{ return TQGDict::size(); }
    bool  isEmpty() const		{ return TQGDict::count() == 0; }

    void  insert( const char *k, const type *d )
					{ TQGDict::look_ascii(k,(Item)d,1); }
    void  replace( const char *k, const type *d )
					{ TQGDict::look_ascii(k,(Item)d,2); }
    bool  remove( const char *k )	{ return TQGDict::remove_ascii(k); }
    type *take( const char *k )		{ return (type *)TQGDict::take_ascii(k); }
    type *find( const char *k ) const
		{ return (type *)((TQGDict*)this)->TQGDict::look_ascii(k,0,0); }
    type *operator[]( const char *k ) const
		{ return (type *)((TQGDict*)this)->TQGDict::look_ascii(k,0,0); }

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

template<> inline void TQAsciiDict<void>::deleteItem( TQPtrCollection::Item )
{
}

template<class type> inline void TQAsciiDict<type>::deleteItem( TQPtrCollection::Item d )
{
    if ( del_item ) delete (type *)d;
}

template<class type>
class TQAsciiDictIterator : public TQGDictIterator
{
public:
    TQAsciiDictIterator(const TQAsciiDict<type> &d)
	: TQGDictIterator((TQGDict &)d) {}
   ~TQAsciiDictIterator()      {}
    uint  count()   const     { return dict->count(); }
    bool  isEmpty() const     { return dict->count() == 0; }
    type *toFirst()	      { return (type *)TQGDictIterator::toFirst(); }
    operator type *() const   { return (type *)TQGDictIterator::get(); }
    type   *current() const   { return (type *)TQGDictIterator::get(); }
    const char *currentKey() const { return TQGDictIterator::getKeyAscii(); }
    type *operator()()	      { return (type *)TQGDictIterator::operator()(); }
    type *operator++()	      { return (type *)TQGDictIterator::operator++(); }
    type *operator+=(uint j)  { return (type *)TQGDictIterator::operator+=(j);}
};

#define Q_DEFINED_QASCIIDICT
#include "ntqwinexport.h"
#endif // TQASCIIDICT_H
