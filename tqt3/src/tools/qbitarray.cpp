/****************************************************************************
**
** Implementation of TQBitArray class
**
** Created : 940118
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

#include "ntqbitarray.h"
#include "ntqdatastream.h"

#define SHBLOCK	 ((bitarr_data*)(sharedBlock()))


/*!
    \class TQBitVal ntqbitarray.h
    \reentrant
    \brief The TQBitVal class is an internal class, used with TQBitArray.

    \ingroup collection

    The TQBitVal is required by the indexing [] operator on bit arrays.
    It is not for use in any other context.
*/

/*!
    \fn TQBitVal::TQBitVal (TQBitArray* a, uint i)

    Constructs a reference to element \a i in the TQBitArray \a a.
    This is what TQBitArray::operator[] constructs its return value
    with.
*/

/*!
    \fn TQBitVal::operator int()

    Returns the value referenced by the TQBitVal.
*/

/*!
    \fn TQBitVal& TQBitVal::operator= (const TQBitVal& v)

    Sets the value referenced by the TQBitVal to that referenced by
    TQBitVal \a v.
*/

/*!
    \overload TQBitVal& TQBitVal::operator= (bool v)

    Sets the value referenced by the TQBitVal to \a v.
*/


/*!
    \class TQBitArray ntqbitarray.h
    \reentrant
    \brief The TQBitArray class provides an array of bits.

    \ingroup collection
    \ingroup tools
    \ingroup shared

    Because TQBitArray is a TQMemArray, it uses explicit \link
    shclass.html sharing\endlink with a reference count.

    A TQBitArray is a special byte array that can access individual
    bits and perform bit-operations (AND, OR, XOR and NOT) on entire
    arrays or bits.

    Bits can be manipulated by the setBit() and clearBit() functions,
    but it is also possible to use the indexing [] operator to test
    and set individual bits. The [] operator is a little slower than
    setBit() and clearBit() because some tricks are required to
    implement single-bit assignments.

    Example:
    \code
    TQBitArray a(3);
    a.setBit( 0 );
    a.clearBit( 1 );
    a.setBit( 2 );     // a = [1 0 1]

    TQBitArray b(3);
    b[0] = 1;
    b[1] = 1;
    b[2] = 0;          // b = [1 1 0]

    TQBitArray c;
    c = ~a & b;        // c = [0 1 0]
    \endcode

    When a TQBitArray is constructed the bits are uninitialized. Use
    fill() to set all the bits to 0 or 1. The array can be resized
    with resize() and copied with copy(). Bits can be set with
    setBit() and cleared with clearBit(). Bits can be toggled with
    toggleBit(). A bit's value can be obtained with testBit() and with
    at().

    TQBitArray supports the \& (AND), | (OR), ^ (XOR) and ~ (NOT)
    operators.
*/

/*! \class TQBitArray::bitarr_data
  \brief The TQBitArray::bitarr_data class is internal.
  \internal
*/


/*!
    Constructs an empty bit array.
*/

TQBitArray::TQBitArray() : TQByteArray( 0, 0 )
{
    bitarr_data *x = new bitarr_data;
    TQ_CHECK_PTR( x );
    x->nbits = 0;
    setSharedBlock( x );
}

/*!
    Constructs a bit array of \a size bits. The bits are uninitialized.

    \sa fill()
*/

TQBitArray::TQBitArray( uint size ) : TQByteArray( 0, 0 )
{
    bitarr_data *x = new bitarr_data;
    TQ_CHECK_PTR( x );
    x->nbits = 0;
    setSharedBlock( x );
    resize( size );
}

/*!
    \fn TQBitArray::TQBitArray( const TQBitArray &a )

    Constructs a shallow copy of \a a.
*/

/*!
    \fn TQBitArray &TQBitArray::operator=( const TQBitArray &a )

    Assigns a shallow copy of \a a to this bit array and returns a
    reference to this array.
*/


/*!
    Pad last byte with 0-bits.
*/
void TQBitArray::pad0()
{
    uint sz = size();
    if ( sz && sz%8 )
	*(data()+sz/8) &= (1 << (sz%8)) - 1;
}


/*!
    \fn uint TQBitArray::size() const

    Returns the bit array's size (number of bits).

    \sa resize()
*/

/*!
    Resizes the bit array to \a size bits and returns true if the bit
    array could be resized; otherwise returns false. The array becomes
    a null array if \a size == 0.

    If the array is expanded, the new bits are set to 0.

    \sa size()
*/

bool TQBitArray::resize( uint size )
{
    uint s = this->size();
    if ( !TQByteArray::resize( (size+7)/8 ) )
	return false;				// cannot resize
    SHBLOCK->nbits = size;
    if ( size != 0 ) {				// not null array
	int ds = (int)(size+7)/8 - (int)(s+7)/8;// number of bytes difference
	if ( ds > 0 )				// expanding array
	    memset( data() + (s+7)/8, 0, ds );	//   reset new data
    }
    return true;
}


/*!
    Fills the bit array with \a v (1's if \a v is true, or 0's if \a v
    is false).

    fill() resizes the bit array to \a size bits if \a size is
    nonnegative.

    Returns false if a nonnegative \e size was specified and the bit
    array could not be resized; otherwise returns true.

    \sa resize()
*/

bool TQBitArray::fill( bool v, int size )
{
    if ( size >= 0 ) {				// resize first
	if ( !resize( size ) )
	    return false;			// cannot resize
    } else {
	size = this->size();
    }
    if ( size > 0 )
	memset( data(), v ? 0xff : 0, (size + 7) / 8 );
    if ( v )
	pad0();
    return true;
}


/*!
    Detaches from shared bit array data and makes sure that this bit
    array is the only one referring to the data.

    If multiple bit arrays share common data, this bit array
    dereferences the data and gets a copy of the data. Nothing happens
    if there is only a single reference.

    \sa copy()
*/

void TQBitArray::detach()
{
    int nbits = SHBLOCK->nbits;
    this->duplicate( *this );
    SHBLOCK->nbits = nbits;
}

/*!
    Returns a deep copy of the bit array.

    \sa detach()
*/

TQBitArray TQBitArray::copy() const
{
    TQBitArray tmp;
    tmp.duplicate( *this );
    ((bitarr_data*)(tmp.sharedBlock()))->nbits = SHBLOCK->nbits;
    return tmp;
}


/*!
    Returns true if the bit at position \a index is set, i.e. is 1;
    otherwise returns false.

    \sa setBit(), clearBit()
*/

bool TQBitArray::testBit( uint index ) const
{
#if defined(QT_CHECK_RANGE)
    if ( index >= size() ) {
	tqWarning( "TQBitArray::testBit: Index %d out of range", index );
	return false;
    }
#endif
    return (*(data()+(index>>3)) & (1 << (index & 7))) != 0;
}

/*!
    \overload

    Sets the bit at position \a index to 1.

    \sa clearBit() toggleBit()
*/

void TQBitArray::setBit( uint index )
{
#if defined(QT_CHECK_RANGE)
    if ( index >= size() ) {
	tqWarning( "TQBitArray::setBit: Index %d out of range", index );
	return;
    }
#endif
    *(data()+(index>>3)) |= (1 << (index & 7));
}

/*!
    \fn void TQBitArray::setBit( uint index, bool value )

    Sets the bit at position \a index to \a value.

    Equivalent to:
    \code
    if ( value )
	setBit( index );
    else
	clearBit( index );
    \endcode

    \sa clearBit() toggleBit()
*/

/*!
    Clears the bit at position \a index, i.e. sets it to 0.

    \sa setBit(), toggleBit()
*/

void TQBitArray::clearBit( uint index )
{
#if defined(QT_CHECK_RANGE)
    if ( index >= size() ) {
	tqWarning( "TQBitArray::clearBit: Index %d out of range", index );
	return;
    }
#endif
    *(data()+(index>>3)) &= ~(1 << (index & 7));
}

/*!
    Toggles the bit at position \a index.

    If the previous value was 0, the new value will be 1. If the
    previous value was 1, the new value will be 0.

    \sa setBit(), clearBit()
*/

bool TQBitArray::toggleBit( uint index )
{
#if defined(QT_CHECK_RANGE)
    if ( index >= size() ) {
	tqWarning( "TQBitArray::toggleBit: Index %d out of range", index );
	return false;
    }
#endif
    uchar *p = (uchar *)data() + (index>>3);
    uchar b = (1 << (index & 7));		// bit position
    uchar c = *p & b;				// read bit
    *p ^= b;					// toggle bit
    return c;
}


/*!
    \fn bool TQBitArray::at( uint index ) const

    Returns the value (0 or 1) of the bit at position \a index.

    \sa operator[]()
*/

/*!
    \fn TQBitVal TQBitArray::operator[]( int index )

    Implements the [] operator for bit arrays.

    The returned TQBitVal is a context object. It makes it possible to
    get and set a single bit value by its \a index position.

    Example:
    \code
    TQBitArray a( 3 );
    a[0] = 0;
    a[1] = 1;
    a[2] = a[0] ^ a[1];
    \endcode

    The functions testBit(), setBit() and clearBit() are faster.

    \sa at()
*/

/*!
    \overload bool TQBitArray::operator[]( int index ) const

    Implements the [] operator for constant bit arrays.
*/


/*!
    Performs the AND operation between all bits in this bit array and
    \a a. Returns a reference to this bit array.

    The result has the length of the longest of the two bit arrays,
    with any missing bits (i.e. if one array is shorter than the
    other), taken to be 0.
    \code
    TQBitArray a( 3 ), b( 2 );
    a[0] = 1;  a[1] = 0;  a[2] = 1;     // a = [1 0 1]
    b[0] = 1;  b[1] = 0;                // b = [1 0]
    a &= b;                             // a = [1 0 0]
    \endcode

    \sa operator|=(), operator^=(), operator~()
*/

TQBitArray &TQBitArray::operator&=( const TQBitArray &a )
{
    resize( TQMAX(size(), a.size()) );
    uchar *a1 = (uchar *)data();
    uchar *a2 = (uchar *)a.data();
    int n = TQMIN( TQByteArray::size(), a.TQByteArray::size() );
    int p = TQMAX( TQByteArray::size(), a.TQByteArray::size() ) - n;
    while ( n-- > 0 )
	*a1++ &= *a2++;
    while ( p-- > 0 )
	*a1++ = 0;
    return *this;
}

/*!
    Performs the OR operation between all bits in this bit array and
    \a a. Returns a reference to this bit array.

    The result has the length of the longest of the two bit arrays,
    with any missing bits (i.e. if one array is shorter than the
    other), taken to be 0.
    \code
    TQBitArray a( 3 ), b( 2 );
    a[0] = 1;  a[1] = 0;  a[2] = 1;     // a = [1 0 1]
    b[0] = 1;  b[1] = 0;                // b = [1 0]
    a |= b;                             // a = [1 0 1]
    \endcode

    \sa operator&=(), operator^=(), operator~()
*/

TQBitArray &TQBitArray::operator|=( const TQBitArray &a )
{
    resize( TQMAX(size(), a.size()) );
    uchar *a1 = (uchar *)data();
    uchar *a2 = (uchar *)a.data();
    int n = TQMIN( TQByteArray::size(), a.TQByteArray::size() );
    while ( n-- > 0 )
	*a1++ |= *a2++;
    return *this;
}

/*!
    Performs the XOR operation between all bits in this bit array and
    \a a. Returns a reference to this bit array.

    The result has the length of the longest of the two bit arrays,
    with any missing bits (i.e. if one array is shorter than the
    other), taken to be 0.
    \code
    TQBitArray a( 3 ), b( 2 );
    a[0] = 1;  a[1] = 0;  a[2] = 1;     // a = [1 0 1]
    b[0] = 1;  b[1] = 0;                // b = [1 0]
    a ^= b;                             // a = [0 0 1]
    \endcode

    \sa operator&=(), operator|=(), operator~()
*/

TQBitArray &TQBitArray::operator^=( const TQBitArray &a )
{
    resize( TQMAX(size(), a.size()) );
    uchar *a1 = (uchar *)data();
    uchar *a2 = (uchar *)a.data();
    int n = TQMIN( TQByteArray::size(), a.TQByteArray::size() );
    while ( n-- > 0 )
	*a1++ ^= *a2++;
    return *this;
}

/*!
    Returns a bit array that contains the inverted bits of this bit array.

    Example:
    \code
    TQBitArray a( 3 ), b;
    a[0] = 1;  a[1] = 0; a[2] = 1;	// a = [1 0 1]
    b = ~a;				// b = [0 1 0]
    \endcode
*/

TQBitArray TQBitArray::operator~() const
{
    TQBitArray a( size() );
    uchar *a1 = (uchar *)data();
    uchar *a2 = (uchar *)a.data();
    int n = TQByteArray::size();
    while ( n-- )
	*a2++ = ~*a1++;
    a.pad0();
    return a;
}


/*!
    \relates TQBitArray

    Returns the AND result between the bit arrays \a a1 and \a a2.

    The result has the length of the longest of the two bit arrays,
    with any missing bits (i.e. if one array is shorter than the
    other), taken to be 0.

    \sa TQBitArray::operator&=()
*/

TQBitArray operator&( const TQBitArray &a1, const TQBitArray &a2 )
{
    TQBitArray tmp = a1.copy();
    tmp &= a2;
    return tmp;
}

/*!
    \relates TQBitArray

    Returns the OR result between the bit arrays \a a1 and \a a2.

    The result has the length of the longest of the two bit arrays,
    with any missing bits (i.e. if one array is shorter than the
    other), taken to be 0.

    \sa TQBitArray::operator|=()
*/

TQBitArray operator|( const TQBitArray &a1, const TQBitArray &a2 )
{
    TQBitArray tmp = a1.copy();
    tmp |= a2;
    return tmp;
}

/*!
    \relates TQBitArray

    Returns the XOR result between the bit arrays \a a1 and \a a2.

    The result has the length of the longest of the two bit arrays,
    with any missing bits (i.e. if one array is shorter than the
    other), taken to be 0.

    \sa TQBitArray::operator^()
*/

TQBitArray operator^( const TQBitArray &a1, const TQBitArray &a2 )
{
    TQBitArray tmp = a1.copy();
    tmp ^= a2;
    return tmp;
}


/* \enum TQGArray::array_data

  \warning This will be renamed in the next major release of TQt.  Until
  then it is undocumented and we recommend against its use.

  \internal

  ### 3.0 rename ###
  ### 3.0 move it to TQGArray? ###
*/


/*!
    \fn TQBitArray::array_data * TQBitArray::newData()

    \internal

    Returns data specific to TQBitArray that extends what TQGArray provides.
    TQPtrCollection mechanism for allowing extra/different data.
*/


/*!
    \fn void  TQBitArray::deleteData ( array_data * d )

    \internal

    Deletes data specific to TQBitArray that extended what TQGArray provided.

    TQPtrCollection mechanism for allowing extra/different data.
*/


/*****************************************************************************
  TQBitArray stream functions
 *****************************************************************************/

/*!
    \relates TQBitArray

    Writes bit array \a a to stream \a s.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/
#ifndef TQT_NO_DATASTREAM
TQDataStream &operator<<( TQDataStream &s, const TQBitArray &a )
{
    TQ_UINT32 len = a.size();
    s << len;					// write size of array
    if ( len > 0 )				// write data
	s.writeRawBytes( a.data(), a.TQByteArray::size() );
    return s;
}

/*!
    \relates TQBitArray

    Reads a bit array into \a a from stream \a s.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQBitArray &a )
{
    TQ_UINT32 len;
    s >> len;					// read size of array
    if ( !a.resize( (uint)len ) ) {		// resize array
#if defined(QT_CHECK_NULL)
	tqWarning( "TQDataStream: Not enough memory to read TQBitArray" );
#endif
	len = 0;
    }
    if ( len > 0 )				// read data
	s.readRawBytes( a.data(), a.TQByteArray::size() );
    return s;
}

#endif // TQT_NO_DATASTREAM
