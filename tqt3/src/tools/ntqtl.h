/****************************************************************************
**
** Definition of TQt template library classes
**
** Created : 990128
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

#ifndef TQTL_H
#define TQTL_H

#ifndef QT_H
#include "ntqglobal.h"
#include "ntqtextstream.h"
#include "ntqstring.h"
#endif // QT_H

#ifndef TQT_NO_TEXTSTREAM
template <class T>
class TQTextOStreamIterator
{
protected:
    TQTextOStream& stream;
    TQString separator;

public:
    TQTextOStreamIterator( TQTextOStream& s) : stream( s ) {}
    TQTextOStreamIterator( TQTextOStream& s, const TQString& sep )
	: stream( s ), separator( sep )  {}
    TQTextOStreamIterator<T>& operator= ( const T& x ) {
	stream << x;
	if ( !separator.isEmpty() )
	    stream << separator;
	return *this;
    }
    TQTextOStreamIterator<T>& operator*() { return *this; }
    TQTextOStreamIterator<T>& operator++() { return *this; }
    TQTextOStreamIterator<T>& operator++(int) { return *this; }
};
#endif //TQT_NO_TEXTSTREAM

template <class InputIterator, class OutputIterator>
inline OutputIterator tqCopy( InputIterator _begin, InputIterator _end,
			     OutputIterator _dest )
{
    while( _begin != _end )
	*_dest++ = *_begin++;
    return _dest;
}

template <class BiIterator, class BiOutputIterator>
inline BiOutputIterator tqCopyBackward( BiIterator _begin, BiIterator _end,
				       BiOutputIterator _dest )
{
    while ( _begin != _end )
	*--_dest = *--_end;
    return _dest;
}

template <class InputIterator1, class InputIterator2>
inline bool tqEqual( InputIterator1 first1, InputIterator1 last1, InputIterator2 first2 )
{
    // ### compare using !(*first1 == *first2) in TQt 4.0
    for ( ; first1 != last1; ++first1, ++first2 )
	if ( *first1 != *first2 )
	    return false;
    return true;
}

template <class ForwardIterator, class T>
inline void tqFill( ForwardIterator first, ForwardIterator last, const T& val )
{
    for ( ; first != last; ++first )
	*first = val;
}

#if 0
template <class BiIterator, class OutputIterator>
inline OutputIterator qReverseCopy( BiIterator _begin, BiIterator _end,
				    OutputIterator _dest )
{
    while ( _begin != _end ) {
	--_end;
	*_dest = *_end;
	++_dest;
    }
    return _dest;
}
#endif


template <class InputIterator, class T>
inline InputIterator tqFind( InputIterator first, InputIterator last,
			    const T& val )
{
    while ( first != last && *first != val )
	++first;
    return first;
}

template <class InputIterator, class T, class Size>
inline void tqCount( InputIterator first, InputIterator last, const T& value,
		    Size& n )
{
    for ( ; first != last; ++first )
	if ( *first == value )
	    ++n;
}

template <class T>
inline void tqSwap( T& _value1, T& _value2 )
{
    T tmp = _value1;
    _value1 = _value2;
    _value2 = tmp;
}


template <class InputIterator>
void qBubbleSort( InputIterator b, InputIterator e )
{
    // Goto last element;
    InputIterator last = e;
    --last;
    // only one element or no elements ?
    if ( last == b )
	return;

    // So we have at least two elements in here
    while( b != last ) {
	bool swapped = false;
	InputIterator swap_pos = b;
	InputIterator x = e;
	InputIterator y = x;
	y--;
	do {
	    --x;
	    --y;
	    if ( *x < *y ) {
		swapped = true;
		tqSwap( *x, *y );
		swap_pos = y;
	    }
	} while( y != b );
	if ( !swapped )
	    return;
	b = swap_pos;
	b++;
    }
}


template <class Container>
inline void qBubbleSort( Container &c )
{
  qBubbleSort( c.begin(), c.end() );
}


template <class Value>
void qHeapSortPushDown( Value* heap, int first, int last )
{
    int r = first;
    while ( r <= last / 2 ) {
	if ( last == 2 * r ) {
	    // node r has only one child
	    if ( heap[2 * r] < heap[r] )
		tqSwap( heap[r], heap[2 * r] );
	    r = last;
	} else {
	    // node r has two children
	    if ( heap[2 * r] < heap[r] && !(heap[2 * r + 1] < heap[2 * r]) ) {
		// swap with left child
		tqSwap( heap[r], heap[2 * r] );
		r *= 2;
	    } else if ( heap[2 * r + 1] < heap[r]
			&& heap[2 * r + 1] < heap[2 * r] ) {
		// swap with right child
		tqSwap( heap[r], heap[2 * r + 1] );
		r = 2 * r + 1;
	    } else {
		r = last;
	    }
	}
    }
}


template <class InputIterator, class Value>
void qHeapSortHelper( InputIterator b, InputIterator e, Value, uint n )
{
    // Create the heap
    InputIterator insert = b;
    Value* realheap = new Value[n];
    // Wow, what a fake. But I want the heap to be indexed as 1...n
    Value* heap = realheap - 1;
    int size = 0;
    for( ; insert != e; ++insert ) {
	heap[++size] = *insert;
	int i = size;
	while( i > 1 && heap[i] < heap[i / 2] ) {
	    tqSwap( heap[i], heap[i / 2] );
	    i /= 2;
	}
    }

    // Now do the sorting
    for( uint i = n; i > 0; i-- ) {
	*b++ = heap[1];
	if ( i > 1 ) {
	    heap[1] = heap[i];
	    qHeapSortPushDown( heap, 1, (int)i - 1 );
	}
    }

    delete[] realheap;
}


template <class InputIterator>
void qHeapSort( InputIterator b, InputIterator e )
{
    // Empty ?
    if ( b == e )
	return;

    // How many entries have to be sorted ?
    InputIterator it = b;
    uint n = 0;
    while ( it != e ) {
	++n;
	++it;
    }

    // The second last parameter is a hack to retrieve the value type
    // Do the real sorting here
    qHeapSortHelper( b, e, *b, n );
}


template <class Container>
void qHeapSort( Container &c )
{
    if ( c.begin() == c.end() )
	return;

    // The second last parameter is a hack to retrieve the value type
    // Do the real sorting here
    qHeapSortHelper( c.begin(), c.end(), *(c.begin()), (uint)c.count() );
}

template <class Container>
class TQBackInsertIterator
{
public:
    explicit TQBackInsertIterator( Container &c )
	: container( &c )
    {
    }

    TQBackInsertIterator<Container>&
    operator=( const TQ_TYPENAME Container::value_type &value )
    {
	container->push_back( value );
	return *this;
    }

    TQBackInsertIterator<Container>& operator*()
    {
	return *this;
    }

    TQBackInsertIterator<Container>& operator++()
    {
	return *this;
    }

    TQBackInsertIterator<Container>& operator++(int)
    {
	return *this;
    }

protected:
    Container *container;
};

template <class Container>
inline TQBackInsertIterator<Container> qBackInserter( Container &c )
{
    return TQBackInsertIterator<Container>( c );
}

#endif
