/****************************************************************************
**
** Definition of TQPointArray class
**
** Created : 940213
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#ifndef TQPOINTARRAY_H
#define TQPOINTARRAY_H

#ifndef QT_H
#include "ntqmemarray.h"
#include "ntqpoint.h"
#endif // QT_H


#if defined(Q_TEMPLATEDLL)
//Q_TEMPLATE_EXTERN template class TQ_EXPORT TQMemArray<TQPoint>;
#endif

class TQ_EXPORT TQPointArray : public TQMemArray<TQPoint>
{
public:
    TQPointArray() {}
    ~TQPointArray() {}
    TQPointArray( int size ) : TQMemArray<TQPoint>( size ) {}
    TQPointArray( const TQPointArray &a ) : TQMemArray<TQPoint>( a ) {}
    TQPointArray( const TQRect &r, bool closed=false );
    TQPointArray( int nPoints, const TQCOORD *points );

    TQPointArray	 &operator=( const TQPointArray &a )
	{ return (TQPointArray&)assign( a ); }

    TQPointArray copy() const
	{ TQPointArray tmp; return *((TQPointArray*)&tmp.duplicate(*this)); }

    void    translate( int dx, int dy );
    TQRect   boundingRect() const;

    void    point( uint i, int *x, int *y ) const;
    TQPoint  point( uint i ) const;
    void    setPoint( uint i, int x, int y );
    void    setPoint( uint i, const TQPoint &p );
    bool    setPoints( int nPoints, const TQCOORD *points );
    bool    setPoints( int nPoints, int firstx, int firsty, ... );
    bool    putPoints( int index, int nPoints, const TQCOORD *points );
    bool    putPoints( int index, int nPoints, int firstx, int firsty, ... );
    bool    putPoints( int index, int nPoints,
		       const TQPointArray & from, int fromIndex=0 );

    void    makeArc( int x, int y, int w, int h, int a1, int a2 );
    void    makeEllipse( int x, int y, int w, int h );
    void    makeArc( int x, int y, int w, int h, int a1, int a2,
		     const TQWMatrix& );
#ifndef TQT_NO_BEZIER
    TQPointArray cubicBezier() const;
#endif
    void*  shortPoints( int index = 0, int nPoints = -1 ) const;
    static void cleanBuffers();

protected:
    static uint splen;
    static void* sp;
};


/*****************************************************************************
  TQPointArray stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQPointArray & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQPointArray & );
#endif

/*****************************************************************************
  Misc. TQPointArray functions
 *****************************************************************************/

inline void TQPointArray::setPoint( uint i, const TQPoint &p )
{
    setPoint( i, p.x(), p.y() );
}


#endif // TQPOINTARRAY_H
