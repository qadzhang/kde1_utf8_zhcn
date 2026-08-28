/****************************************************************************
**
** Definition of TQGVector class
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

#ifndef TQGVECTOR_H
#define TQGVECTOR_H

#ifndef QT_H
#include "ntqptrcollection.h"
#endif // QT_H


class TQ_EXPORT TQGVector : public TQPtrCollection	// generic vector
{
friend class TQGList;				// needed by TQGList::toVector
public:
#ifndef TQT_NO_DATASTREAM
    TQDataStream &read( TQDataStream & );		// read vector from stream
    TQDataStream &write( TQDataStream & ) const;	// write vector to stream
#endif
    virtual int compareItems( Item, Item );

protected:
    TQGVector();					// create empty vector
    TQGVector( uint size );			// create vector with nullptrs
    TQGVector( const TQGVector &v );		// make copy of other vector
   ~TQGVector();

    TQGVector &operator=( const TQGVector &v );	// assign from other vector
    bool operator==( const TQGVector &v ) const;

    Item	 *data()    const	{ return vec; }
    uint  size()    const	{ return len; }
    uint  count()   const	{ return numItems; }

    bool  insert( uint index, Item );		// insert item at index
    bool  remove( uint index );			// remove item
    Item	  take( uint index );			// take out item

    void  clear();				// clear vector
    bool  resize( uint newsize );		// resize vector

    bool  fill( Item, int flen );		// resize and fill vector

    void  sort();				// sort vector
    int	  bsearch( Item ) const;			// binary search (when sorted)

    int	  findRef( Item, uint index ) const;	// find exact item in vector
    int	  find( Item, uint index ) const;	// find equal item in vector
    uint  containsRef( Item ) const;		// get number of exact matches
    uint  contains( Item ) const;		// get number of equal matches

    Item	  at( uint index ) const		// return indexed item
    {
#if defined(QT_CHECK_RANGE)
	if ( index >= len )
	    warningIndexRange( index );
#endif
	return vec[index];
    }

    bool insertExpand( uint index, Item );	// insert, expand if necessary

    void toList( TQGList * ) const;		// put items in list

#ifndef TQT_NO_DATASTREAM
    virtual TQDataStream &read( TQDataStream &, Item & );
    virtual TQDataStream &write( TQDataStream &, Item ) const;
#endif
private:
    Item	 *vec;
    uint  len;
    uint  numItems;

    static void warningIndexRange( uint );
};


/*****************************************************************************
  TQGVector stream functions
 *****************************************************************************/

#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQGVector & );
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQGVector & );
#endif

#endif // TQGVECTOR_H
