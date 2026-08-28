/****************************************************************************
**
** Definition of TQPoint class
**
** Created : 931028
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

#ifndef TQPOINT_H
#define TQPOINT_H

#ifndef QT_H
#include "ntqwindowdefs.h"
#endif // QT_H


class TQ_EXPORT TQPoint
{
public:
    TQPoint();
    TQPoint( int xpos, int ypos );

    bool   isNull()	const;

    int	   x()		const;
    int	   y()		const;
    void   setX( int x );
    void   setY( int y );

    int manhattanLength() const;

    TQCOORD &rx();
    TQCOORD &ry();

    TQPoint &operator+=( const TQPoint &p );
    TQPoint &operator-=( const TQPoint &p );
    TQPoint &operator*=( int c );
    TQPoint &operator*=( double c );
    TQPoint &operator/=( int c );
    TQPoint &operator/=( double c );

    friend inline bool	 operator==( const TQPoint &, const TQPoint & );
    friend inline bool	 operator!=( const TQPoint &, const TQPoint & );
    friend inline const TQPoint operator+( const TQPoint &, const TQPoint & );
    friend inline const TQPoint operator-( const TQPoint &, const TQPoint & );
    friend inline const TQPoint operator*( const TQPoint &, int );
    friend inline const TQPoint operator*( int, const TQPoint & );
    friend inline const TQPoint operator*( const TQPoint &, double );
    friend inline const TQPoint operator*( double, const TQPoint & );
    friend inline const TQPoint operator-( const TQPoint & );
    friend inline const TQPoint operator/( const TQPoint &, int );
    friend inline const TQPoint operator/( const TQPoint &, double );

private:
    static void warningDivByZero();

#if defined(Q_OS_MAC)
    TQCOORD yp;
    TQCOORD xp;
#else
    TQCOORD xp;
    TQCOORD yp;
#endif
};


/*****************************************************************************
  TQPoint stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQPoint & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQPoint & );
#endif

/*****************************************************************************
  TQPoint inline functions
 *****************************************************************************/

inline TQPoint::TQPoint()
{ xp=0; yp=0; }

inline TQPoint::TQPoint( int xpos, int ypos )
{ xp=(TQCOORD)xpos; yp=(TQCOORD)ypos; }

inline bool TQPoint::isNull() const
{ return xp == 0 && yp == 0; }

inline int TQPoint::x() const
{ return xp; }

inline int TQPoint::y() const
{ return yp; }

inline void TQPoint::setX( int x )
{ xp = (TQCOORD)x; }

inline void TQPoint::setY( int y )
{ yp = (TQCOORD)y; }

inline TQCOORD &TQPoint::rx()
{ return xp; }

inline TQCOORD &TQPoint::ry()
{ return yp; }

inline TQPoint &TQPoint::operator+=( const TQPoint &p )
{ xp+=p.xp; yp+=p.yp; return *this; }

inline TQPoint &TQPoint::operator-=( const TQPoint &p )
{ xp-=p.xp; yp-=p.yp; return *this; }

inline TQPoint &TQPoint::operator*=( int c )
{ xp*=(TQCOORD)c; yp*=(TQCOORD)c; return *this; }

inline TQPoint &TQPoint::operator*=( double c )
{ xp=(TQCOORD)(xp*c); yp=(TQCOORD)(yp*c); return *this; }

inline bool operator==( const TQPoint &p1, const TQPoint &p2 )
{ return p1.xp == p2.xp && p1.yp == p2.yp; }

inline bool operator!=( const TQPoint &p1, const TQPoint &p2 )
{ return p1.xp != p2.xp || p1.yp != p2.yp; }

inline const TQPoint operator+( const TQPoint &p1, const TQPoint &p2 )
{ return TQPoint(p1.xp+p2.xp, p1.yp+p2.yp); }

inline const TQPoint operator-( const TQPoint &p1, const TQPoint &p2 )
{ return TQPoint(p1.xp-p2.xp, p1.yp-p2.yp); }

inline const TQPoint operator*( const TQPoint &p, int c )
{ return TQPoint(p.xp*c, p.yp*c); }

inline const TQPoint operator*( int c, const TQPoint &p )
{ return TQPoint(p.xp*c, p.yp*c); }

inline const TQPoint operator*( const TQPoint &p, double c )
{ return TQPoint((TQCOORD)(p.xp*c), (TQCOORD)(p.yp*c)); }

inline const TQPoint operator*( double c, const TQPoint &p )
{ return TQPoint((TQCOORD)(p.xp*c), (TQCOORD)(p.yp*c)); }

inline const TQPoint operator-( const TQPoint &p )
{ return TQPoint(-p.xp, -p.yp); }

inline TQPoint &TQPoint::operator/=( int c )
{
#if defined(QT_CHECK_MATH)
    if ( c == 0 )
	warningDivByZero();
#endif
    xp/=(TQCOORD)c;
    yp/=(TQCOORD)c;
    return *this;
}

inline TQPoint &TQPoint::operator/=( double c )
{
#if defined(QT_CHECK_MATH)
    if ( c == 0.0 )
	warningDivByZero();
#endif
    xp=(TQCOORD)(xp/c);
    yp=(TQCOORD)(yp/c);
    return *this;
}

inline const TQPoint operator/( const TQPoint &p, int c )
{
#if defined(QT_CHECK_MATH)
    if ( c == 0 )
	TQPoint::warningDivByZero();
#endif
    return TQPoint(p.xp/c, p.yp/c);
}

inline const TQPoint operator/( const TQPoint &p, double c )
{
#if defined(QT_CHECK_MATH)
    if ( c == 0.0 )
	TQPoint::warningDivByZero();
#endif
    return TQPoint((TQCOORD)(p.xp/c), (TQCOORD)(p.yp/c));
}

#define Q_DEFINED_QPOINT
#include "ntqwinexport.h"
#endif // TQPOINT_H
