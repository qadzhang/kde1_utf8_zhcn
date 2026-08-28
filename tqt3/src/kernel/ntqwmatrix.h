/****************************************************************************
**
** Definition of TQWMatrix class
**
** Created : 941020
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

#ifndef TQWMATRIX_H
#define TQWMATRIX_H

#ifndef QT_H
#include "ntqwindowdefs.h"
#include "ntqpointarray.h"
#include "ntqrect.h"
#include "ntqregion.h"
#endif // QT_H

#ifndef TQT_NO_WMATRIX


class TQ_EXPORT TQWMatrix					// 2D transform matrix
{
public:
    TQWMatrix();
    TQWMatrix( double m11, double m12, double m21, double m22,
	      double dx, double dy );

    void	setMatrix( double m11, double m12, double m21, double m22,
			   double dx,  double dy );

    double	m11() const { return _m11; }
    double	m12() const { return _m12; }
    double	m21() const { return _m21; }
    double	m22() const { return _m22; }
    double	dx()  const { return _dx; }
    double	dy()  const { return _dy; }

    void	map( int x, int y, int *tx, int *ty )	      const;
    void	map( double x, double y, double *tx, double *ty ) const;
    TQRect	mapRect( const TQRect & )	const;

    TQPoint	map( const TQPoint &p )	const { return operator *( p ); }
    TQRect	map( const TQRect &r )	const { return mapRect ( r ); }
    TQPointArray map( const TQPointArray &a ) const { return operator * ( a ); }
    TQRegion     map( const TQRegion &r ) const { return operator *( r ); }
    TQRegion     mapToRegion( const TQRect &r ) const { return operator *( r ); }
    TQPointArray	mapToPolygon( const TQRect &r )	const;

    void	reset();
    bool	isIdentity() const;

    TQWMatrix   &translate( double dx, double dy );
    TQWMatrix   &scale( double sx, double sy );
    TQWMatrix   &shear( double sh, double sv );
    TQWMatrix   &rotate( double a );

    bool isInvertible() const { return (_m11*_m22 - _m12*_m21) != 0; }
    double det() const { return _m11*_m22 - _m12*_m21; }

    TQWMatrix	invert( bool * = 0 ) const;

    bool	operator==( const TQWMatrix & ) const;
    bool	operator!=( const TQWMatrix & ) const;
    TQWMatrix   &operator*=( const TQWMatrix & );

    /* we use matrix multiplication semantics here */
    TQPoint operator * (const TQPoint & ) const;
    TQRegion operator * (const TQRect & ) const;
    TQRegion operator * (const TQRegion & ) const;
    TQPointArray operator *  ( const TQPointArray &a ) const;

    enum TransformationMode {
	Points, Areas
    };
    static void setTransformationMode( TQWMatrix::TransformationMode m );
    static TransformationMode transformationMode();
private:
    double	_m11, _m12;
    double	_m21, _m22;
    double	_dx,  _dy;
};

TQ_EXPORT TQWMatrix operator*( const TQWMatrix &, const TQWMatrix & );


/*****************************************************************************
  TQWMatrix stream functions
 *****************************************************************************/

TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQWMatrix & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQWMatrix & );


#endif // TQT_NO_WMATRIX

#endif // TQWMATRIX_H
