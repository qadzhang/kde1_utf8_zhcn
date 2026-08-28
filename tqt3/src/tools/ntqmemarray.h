/****************************************************************************
**
** Definition of TQMemArray template/macro class
**
** Created : 930906
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

#ifndef TQMEMARRAY_H
#define TQMEMARRAY_H

#ifndef QT_H
#include "ntqgarray.h"
#endif // QT_H


template<class type>
class TQMemArray : public TQGArray
{
public:
    typedef type* Iterator;
    typedef const type* ConstIterator;
    typedef type ValueType;

protected:
    TQMemArray( int, int ) : TQGArray( 0, 0 ) {}

public:
    TQMemArray() {}
    TQMemArray( int size ) : TQGArray(size*sizeof(type)) {} // ### 4.0 explicit
    TQMemArray( const TQMemArray<type> &a ) : TQGArray(a) {}
   ~TQMemArray() {}
    TQMemArray<type> &operator=(const TQMemArray<type> &a)
				{ return (TQMemArray<type>&)TQGArray::assign(a); }
    type *data()    const	{ return (type *)TQGArray::data(); }
    uint  nrefs()   const	{ return TQGArray::nrefs(); }
    uint  size()    const	{ return TQGArray::size()/sizeof(type); }
    uint  count()   const	{ return size(); }
    bool  isEmpty() const	{ return TQGArray::size() == 0; }
    bool  isNull()  const	{ return TQGArray::data() == 0; }
    bool  resize( uint size )	{ return TQGArray::resize(size*sizeof(type)); }
    bool  resize( uint size, Optimization optim ) { return TQGArray::resize(size*sizeof(type), optim); }
    bool  truncate( uint pos )	{ return TQGArray::resize(pos*sizeof(type)); }
    bool  fill( const type &d, int size = -1 )
	{ return TQGArray::fill((char*)&d,size,sizeof(type) ); }
    void  detach()		{ TQGArray::detach(); }
    TQMemArray<type>   copy() const
	{ TQMemArray<type> tmp; return tmp.duplicate(*this); }
    TQMemArray<type>& assign( const TQMemArray<type>& a )
	{ return (TQMemArray<type>&)TQGArray::assign(a); }
    TQMemArray<type>& assign( const type *a, uint n )
	{ return (TQMemArray<type>&)TQGArray::assign((char*)a,n*sizeof(type)); }
    TQMemArray<type>& duplicate( const TQMemArray<type>& a )
	{ return (TQMemArray<type>&)TQGArray::duplicate(a); }
    TQMemArray<type>& duplicate( const type *a, uint n )
	{ return (TQMemArray<type>&)TQGArray::duplicate((char*)a,n*sizeof(type)); }
    TQMemArray<type>& setRawData( const type *a, uint n )
	{ return (TQMemArray<type>&)TQGArray::setRawData((char*)a,
						     n*sizeof(type)); }
    void resetRawData( const type *a, uint n )
	{ TQGArray::resetRawData((char*)a,n*sizeof(type)); }
    int	 find( const type &d, uint i=0 ) const
	{ return TQGArray::find((char*)&d,i,sizeof(type)); }
    int	 contains( const type &d ) const
	{ return TQGArray::contains((char*)&d,sizeof(type)); }
    void sort() { TQGArray::sort(sizeof(type)); }
    int  bsearch( const type &d ) const
	{ return TQGArray::bsearch((const char*)&d,sizeof(type)); }
    // ### TQt 4.0: maybe provide uint overload as work-around for MSVC bug
    type& operator[]( int i ) const
	{ return (type &)(*(type *)TQGArray::at(i*sizeof(type))); }
    type& at( uint i ) const
	{ return (type &)(*(type *)TQGArray::at(i*sizeof(type))); }
	 operator const type*() const { return (const type *)TQGArray::data(); }
    bool operator==( const TQMemArray<type> &a ) const { return isEqual(a); }
    bool operator!=( const TQMemArray<type> &a ) const { return !isEqual(a); }
    Iterator begin() { return data(); }
    Iterator end() { return data() + size(); }
    ConstIterator begin() const { return data(); }
    ConstIterator end() const { return data() + size(); }
};

#ifndef TQT_NO_COMPAT
#define TQArray TQMemArray
#endif

#define Q_DEFINED_QMEMARRAY
#include "ntqwinexport.h"
#endif // TQARRAY_H
