/****************************************************************************
**
** Definition of TQBitArray class
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

#ifndef TQBITARRAY_H
#define TQBITARRAY_H

#ifndef QT_H
#include "ntqstring.h"
#endif // QT_H


/*****************************************************************************
  TQBitVal class; a context class for TQBitArray::operator[]
 *****************************************************************************/

class TQBitArray;

class TQ_EXPORT TQBitVal
{
private:
    TQBitArray *array;
    uint    index;
public:
    TQBitVal( TQBitArray *a, uint i ) : array(a), index(i) {}
    operator int();
    TQBitVal &operator=( const TQBitVal &v );
    TQBitVal &operator=( bool v );
};


/*****************************************************************************
  TQBitArray class
 *****************************************************************************/

class TQ_EXPORT TQBitArray : public TQByteArray
{
public:
    TQBitArray();
    TQBitArray( uint size );
    TQBitArray( const TQBitArray &a ) : TQByteArray( a ) {}

    TQBitArray &operator=( const TQBitArray & );

    uint    size() const;
    bool    resize( uint size );

    bool    fill( bool v, int size = -1 );

    void    detach();
    TQBitArray copy() const;

    bool    testBit( uint index ) const;
    void    setBit( uint index );
    void    setBit( uint index, bool value );
    void    clearBit( uint index );
    bool    toggleBit( uint index );

    bool    at( uint index ) const;
    TQBitVal operator[]( int index );
    bool operator[]( int index ) const;

    TQBitArray &operator&=( const TQBitArray & );
    TQBitArray &operator|=( const TQBitArray & );
    TQBitArray &operator^=( const TQBitArray & );
    TQBitArray  operator~() const;

protected:
    struct bitarr_data : public TQGArray::array_data {
	uint   nbits;
    };
    array_data *newData()		    { return new bitarr_data; }
    void	deleteData( array_data *d ) { delete (bitarr_data*)d; }
private:
    void    pad0();
};


inline TQBitArray &TQBitArray::operator=( const TQBitArray &a )
{ return (TQBitArray&)assign( a ); }

inline uint TQBitArray::size() const
{ return ((bitarr_data*)sharedBlock())->nbits; }

inline void TQBitArray::setBit( uint index, bool value )
{ if ( value ) setBit(index); else clearBit(index); }

inline bool TQBitArray::at( uint index ) const
{ return testBit(index); }

inline TQBitVal TQBitArray::operator[]( int index )
{ return TQBitVal( (TQBitArray*)this, index ); }

inline bool TQBitArray::operator[]( int index ) const
{ return testBit( index ); }


/*****************************************************************************
  Misc. TQBitArray operator functions
 *****************************************************************************/

TQ_EXPORT TQBitArray operator&( const TQBitArray &, const TQBitArray & );
TQ_EXPORT TQBitArray operator|( const TQBitArray &, const TQBitArray & );
TQ_EXPORT TQBitArray operator^( const TQBitArray &, const TQBitArray & );


inline TQBitVal::operator int()
{
    return array->testBit( index );
}

inline TQBitVal &TQBitVal::operator=( const TQBitVal &v )
{
    array->setBit( index, v.array->testBit(v.index) );
    return *this;
}

inline TQBitVal &TQBitVal::operator=( bool v )
{
    array->setBit( index, v );
    return *this;
}


/*****************************************************************************
  TQBitArray stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQBitArray & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQBitArray & );
#endif

#endif // TQBITARRAY_H
