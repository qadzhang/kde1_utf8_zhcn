/****************************************************************************
**
** Implementation of TQGVector class
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

#include "ntqglobal.h"

#define	 TQGVECTOR_CPP
#include "ntqgvector.h"
#include "ntqglist.h"
#include "ntqstring.h"
#include "ntqdatastream.h"
#include <stdlib.h>

#ifdef TQT_THREAD_SUPPORT
#  include <private/qmutexpool_p.h>
#endif // TQT_THREAD_SUPPORT

#define USE_MALLOC				// comment to use new/delete

#undef NEW
#undef DELETE

#if defined(USE_MALLOC)
#define NEW(type,size)	((type*)malloc(size*sizeof(type)))
#define DELETE(array)	(free((char*)array))
#else
#define NEW(type,size)	(new type[size])
#define DELETE(array)	(delete[] array)
#define DONT_USE_REALLOC			// comment to use realloc()
#endif

/*!
  \class TQGVector
  \reentrant
  \ingroup collection

  \brief The TQGVector class is an internal class for implementing TQt
  collection classes.

  \internal

  TQGVector is an internal class that acts as a base class for the
  TQPtrVector collection class.

  TQGVector has some virtual functions that may be reimplemented in
  subclasses to customize behavior.

  \list
  \i compareItems() compares two collection/vector items.
  \i read() reads a collection/vector item from a TQDataStream.
  \i write() writes a collection/vector item to a TQDataStream.
  \endlist
*/

/*****************************************************************************
  Default implementation of virtual functions
 *****************************************************************************/

/*!
  This virtual function compares two list items.

  Returns:
  <ul>
  <li> 0 if \a d1 == \a d2
  <li> non-zero if \a d1 != \a d2
  </ul>

  This function returns \e int rather than \e bool so that
  reimplementations can return one of three values and use it to sort
  by:
  <ul>
  <li> 0 if \a d1 == \a d2
  <li> \> 0 (positive integer) if \a d1 \> \a d2
  <li> \< 0 (negative integer) if \a d1 \< \a d2
  </ul>

  The TQPtrVector::sort() and TQPtrVector::bsearch() functions require that
  compareItems() is implemented as described here.

  This function should not modify the vector because some const
  functions call compareItems().
*/

int TQGVector::compareItems( Item d1, Item d2 )
{
    return d1 != d2;				// compare pointers
}

#ifndef TQT_NO_DATASTREAM
/*!
  Reads a collection/vector item from the stream \a s and returns a reference
  to the stream.

  The default implementation sets \a d to 0.

  \sa write()
*/

TQDataStream &TQGVector::read( TQDataStream &s, Item &d )
{						// read item from stream
    d = 0;
    return s;
}

/*!
  Writes a collection/vector item to the stream \a s and returns a reference
  to the stream.

  The default implementation does nothing.

  \sa read()
*/

TQDataStream &TQGVector::write( TQDataStream &s, Item ) const
{						// write item to stream
    return s;
}
#endif // TQT_NO_DATASTREAM

/*****************************************************************************
  TQGVector member functions
 *****************************************************************************/

TQGVector::TQGVector()				// create empty vector
{
    vec = 0;
    len = numItems = 0;
}

TQGVector::TQGVector( uint size )			// create vectors with nullptrs
{
    len = size;
    numItems = 0;
    if ( len == 0 ) {				// zero length
	vec = 0;
	return;
    }
    vec = NEW(Item,len);
    TQ_CHECK_PTR( vec );
    memset( (void*)vec, 0, len*sizeof(Item) );	// fill with nulls
}

TQGVector::TQGVector( const TQGVector &a )		// make copy of other vector
    : TQPtrCollection( a )
{
    len = a.len;
    numItems = a.numItems;
    if ( len == 0 ) {
	vec = 0;
	return;
    }
    vec = NEW( Item, len );
    TQ_CHECK_PTR( vec );
    for ( uint i = 0; i < len; i++ ) {
	if ( a.vec[i] ) {
	    vec[i] = newItem( a.vec[i] );
	    TQ_CHECK_PTR( vec[i] );
	} else {
	    vec[i] = 0;
	}
    }
}

TQGVector::~TQGVector()
{
    clear();
}

TQGVector& TQGVector::operator=( const TQGVector &v )
{
    if ( &v == this )
	return *this;

    clear();
    len = v.len;
    numItems = v.numItems;
    if ( len == 0 ) {
	vec = 0;
	return *this;
    }
    vec = NEW( Item, len );
    TQ_CHECK_PTR( vec );
    for ( uint i = 0; i < len; i++ ) {
	if ( v.vec[i] ) {
	    vec[i] = newItem( v.vec[i] );
	    TQ_CHECK_PTR( vec[i] );
	} else {
	    vec[i] = 0;
	}
    }
    return *this;
}


bool TQGVector::insert( uint index, Item d )	// insert item at index
{
#if defined(QT_CHECK_RANGE)
    if ( index >= len ) {			// range error
	tqWarning( "TQGVector::insert: Index %d out of range", index );
	return false;
    }
#endif
    if ( vec[index] ) {				// remove old item
	deleteItem( vec[index] );
	numItems--;
    }
    if ( d ) {
	vec[index] = newItem( d );
	TQ_CHECK_PTR( vec[index] );
	numItems++;
	return vec[index] != 0;
    } else {
	vec[index] = 0;				// reset item
    }
    return true;
}

bool TQGVector::remove( uint index )		// remove item at index
{
#if defined(QT_CHECK_RANGE)
    if ( index >= len ) {			// range error
	tqWarning( "TQGVector::remove: Index %d out of range", index );
	return false;
    }
#endif
    if ( vec[index] ) {				// valid item
	deleteItem( vec[index] );		// delete it
	vec[index] = 0;				// reset pointer
	numItems--;
    }
    return true;
}

TQPtrCollection::Item TQGVector::take( uint index )		// take out item
{
#if defined(QT_CHECK_RANGE)
    if ( index >= len ) {			// range error
	tqWarning( "TQGVector::take: Index %d out of range", index );
	return 0;
    }
#endif
    Item d = vec[index];				// don't delete item
    if ( d )
	numItems--;
    vec[index] = 0;
    return d;
}

void TQGVector::clear()				// clear vector
{
    if ( vec ) {
	for ( uint i=0; i<len; i++ ) {		// delete each item
	    if ( vec[i] )
		deleteItem( vec[i] );
	}
	DELETE(vec);
	vec = 0;
	len = numItems = 0;
    }
}

bool TQGVector::resize( uint newsize )		// resize array
{
    if ( newsize == len )			// nothing to do
	return true;
    if ( vec ) {				// existing data
	if ( newsize < len ) {			// shrink vector
	    uint i = newsize;
	    while ( i < len ) {			// delete lost items
		if ( vec[i] ) {
		    deleteItem( vec[i] );
		    numItems--;
		}
		i++;
	    }
	}
	if ( newsize == 0 ) {			// vector becomes empty
	    DELETE(vec);
	    vec = 0;
	    len = numItems = 0;
	    return true;
	}
#if defined(DONT_USE_REALLOC)
	if ( newsize == 0 ) {
	    DELETE(vec);
	    vec = 0;
	    return false;
	}
	Item *newvec = NEW(Item,newsize);		// manual realloc
        if (!newvec)
            return false;
	memcpy( newvec, vec, (len < newsize ? len : newsize)*sizeof(Item) );
	DELETE(vec);
	vec = newvec;
#else
	Item *newvec = (Item*)realloc( (char *)vec, newsize*sizeof(Item) );
        if (!newvec)
            return false;
        vec = newvec;
#endif
    } else {					// create new vector
	vec = NEW(Item,newsize);
        if (!vec)
            return false;
	len = numItems = 0;
    }
    if ( newsize > len )			// init extra space added
	memset( (void*)&vec[len], 0, (newsize-len)*sizeof(Item) );
    len = newsize;
    return true;
}


bool TQGVector::fill( Item d, int flen )		// resize and fill vector
{
    if ( flen < 0 )
	flen = len;				// default: use vector length
    else if ( !resize( flen ) )
	return false;
    for ( uint i=0; i<(uint)flen; i++ )		// insert d at every index
	insert( i, d );
    return true;
}


static TQGVector *sort_vec=0;			// current sort vector


#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

#ifdef Q_OS_TEMP
static int _cdecl cmp_vec( const void *n1, const void *n2 )
#else
static int cmp_vec( const void *n1, const void *n2 )
#endif
{
    return sort_vec->compareItems( *((TQPtrCollection::Item*)n1), *((TQPtrCollection::Item*)n2) );
}

#if defined(Q_C_CALLBACKS)
}
#endif


void TQGVector::sort()				// sort vector
{
    if ( count() == 0 )				// no elements
	return;
    Item *start = &vec[0];
    Item *end	= &vec[len-1];
    Item tmp;
    for (;;) {				// put all zero elements behind
	while ( start < end && *start != 0 )
	    start++;
	while ( end > start && *end == 0 )
	    end--;
	if ( start < end ) {
	    tmp = *start;
	    *start = *end;
	    *end = tmp;
	} else {
	    break;
	}
    }

#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker locker( tqt_global_mutexpool ?
			 tqt_global_mutexpool->get( &sort_vec ) : 0 );
#endif // TQT_THREAD_SUPPORT

    sort_vec = (TQGVector*)this;
    qsort( vec, count(), sizeof(Item), cmp_vec );
    sort_vec = 0;
}

int TQGVector::bsearch( Item d ) const		// binary search; when sorted
{
    if ( !len )
	return -1;
    if ( !d ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQGVector::bsearch: Cannot search for null object" );
#endif
	return -1;
    }
    int n1 = 0;
    int n2 = len - 1;
    int mid = 0;
    bool found = false;
    while ( n1 <= n2 ) {
	int  res;
	mid = (n1 + n2)/2;
	if ( vec[mid] == 0 )			// null item greater
	    res = -1;
	else
	    res = ((TQGVector*)this)->compareItems( d, vec[mid] );
	if ( res < 0 )
	    n2 = mid - 1;
	else if ( res > 0 )
	    n1 = mid + 1;
	else {					// found it
	    found = true;
	    break;
	}
    }
    if ( !found )
	return -1;
    // search to first of equal items
    while ( (mid - 1 >= 0) && !((TQGVector*)this)->compareItems(d, vec[mid-1]) )
	mid--;
    return mid;
}

int TQGVector::findRef( Item d, uint index) const // find exact item in vector
{
#if defined(QT_CHECK_RANGE)
    if ( index > len ) {			// range error
	tqWarning( "TQGVector::findRef: Index %d out of range", index );
	return -1;
    }
#endif
    for ( uint i=index; i<len; i++ ) {
	if ( vec[i] == d )
	    return i;
    }
    return -1;
}

int TQGVector::find( Item d, uint index ) const	// find equal item in vector
{
#if defined(QT_CHECK_RANGE)
    if ( index >= len ) {			// range error
	tqWarning( "TQGVector::find: Index %d out of range", index );
	return -1;
    }
#endif
    for ( uint i=index; i<len; i++ ) {
	if ( vec[i] == 0 && d == 0 )		// found null item
	    return i;
	if ( vec[i] && ((TQGVector*)this)->compareItems( vec[i], d ) == 0 )
	    return i;
    }
    return -1;
}

uint TQGVector::containsRef( Item d ) const	// get number of exact matches
{
    uint count = 0;
    for ( uint i=0; i<len; i++ ) {
	if ( vec[i] == d )
	    count++;
    }
    return count;
}

uint TQGVector::contains( Item d ) const		// get number of equal matches
{
    uint count = 0;
    for ( uint i=0; i<len; i++ ) {
	if ( vec[i] == 0 && d == 0 )		// count null items
	    count++;
	if ( vec[i] && ((TQGVector*)this)->compareItems( vec[i], d ) == 0 )
	    count++;
    }
    return count;
}

bool TQGVector::insertExpand( uint index, Item d )// insert and grow if necessary
{
    if ( index >= len ) {
	if ( !resize( index+1 ) )		// no memory
	    return false;
    }
    insert( index, d );
    return true;
}

void TQGVector::toList( TQGList *list ) const	// store items in list
{
    list->clear();
    for ( uint i=0; i<len; i++ ) {
	if ( vec[i] )
	    list->append( vec[i] );
    }
}


void TQGVector::warningIndexRange( uint i )
{
#if defined(QT_CHECK_RANGE)
    tqWarning( "TQGVector::operator[]: Index %d out of range", i );
#else
    Q_UNUSED( i )
#endif
}


/*****************************************************************************
  TQGVector stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
TQDataStream &operator>>( TQDataStream &s, TQGVector &vec )
{						// read vector
    return vec.read( s );
}

TQDataStream &operator<<( TQDataStream &s, const TQGVector &vec )
{						// write vector
    return vec.write( s );
}

TQDataStream &TQGVector::read( TQDataStream &s )	// read vector from stream
{
    uint num;
    s >> num;					// read number of items
    clear();					// clear vector
    resize( num );
    for (uint i=0; i<num; i++) {		// read all items
	Item d;
	read( s, d );
	TQ_CHECK_PTR( d );
	if ( !d )				// no memory
	    break;
	vec[i] = d;
    }
    return s;
}

TQDataStream &TQGVector::write( TQDataStream &s ) const
{						// write vector to stream
    uint num = count();
    s << num;					// number of items to write
    num = size();
    for (uint i=0; i<num; i++) {		// write non-null items
	if ( vec[i] )
	    write( s, vec[i] );
    }
    return s;
}

/* Returns whether v equals this vector or not */

bool TQGVector::operator==( const TQGVector &v ) const
{
    if ( size() != v.size() )
	return false;
    if ( count() != v.count() )
	return false;
    for ( int i = 0; i < (int)size(); ++i ) {
	if ( ( (TQGVector*)this )->compareItems( at( i ), v.at( i ) ) != 0 )
	    return false;
    }
    return true;
}

#endif // TQT_NO_DATASTREAM
