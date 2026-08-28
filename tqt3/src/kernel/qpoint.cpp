/****************************************************************************
**
** Implementation of TQPoint class
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

#include "ntqpoint.h"
#include "ntqdatastream.h"


/*!
    \class TQPoint ntqpoint.h
    \brief The TQPoint class defines a point in the plane.

    \ingroup images
    \ingroup graphics
    \mainclass

    A point is specified by an x coordinate and a y coordinate.

    The coordinate type is \c TQCOORD (a 32-bit integer). The minimum
    value of \c TQCOORD is \c TQCOORD_MIN (-2147483648) and the maximum
    value is  \c TQCOORD_MAX (2147483647).

    The coordinates are accessed by the functions x() and y(); they
    can be set by setX() and setY() or by the reference functions rx()
    and ry().

    Given a point \e p, the following statements are all equivalent:
    \code
	p.setX( p.x() + 1 );
	p += TQPoint( 1, 0 );
	p.rx()++;
    \endcode

    A TQPoint can also be used as a vector. Addition and subtraction
    of TQPoints are defined as for vectors (each component is added
    separately). You can divide or multiply a TQPoint by an \c int or a
    \c double. The function manhattanLength() gives an inexpensive
    approximation of the length of the TQPoint interpreted as a vector.

    Example:
    \code
	//TQPoint oldPos is defined somewhere else
	MyWidget::mouseMoveEvent( TQMouseEvent *e )
	{
	    TQPoint vector = e->pos() - oldPos;
	    if ( vector.manhattanLength() > 3 )
	    ... //mouse has moved more than 3 pixels since oldPos
	}
    \endcode

    TQPoints can be compared for equality or inequality, and they can
    be written to and read from a TQStream.

    \sa TQPointArray TQSize, TQRect
*/


/*****************************************************************************
  TQPoint member functions
 *****************************************************************************/

/*!
    \fn TQPoint::TQPoint()

    Constructs a point with coordinates (0, 0) (isNull() returns true).
*/

/*!
    \fn TQPoint::TQPoint( int xpos, int ypos )

    Constructs a point with x value \a xpos and y value \a ypos.
*/

/*!
    \fn bool TQPoint::isNull() const

    Returns true if both the x value and the y value are 0; otherwise
    returns false.
*/

/*!
    \fn int TQPoint::x() const

    Returns the x coordinate of the point.

    \sa setX() y()
*/

/*!
    \fn int TQPoint::y() const

    Returns the y coordinate of the point.

    \sa setY() x()
*/

/*!
    \fn void TQPoint::setX( int x )

    Sets the x coordinate of the point to \a x.

    \sa x() setY()
*/

/*!
    \fn void TQPoint::setY( int y )

    Sets the y coordinate of the point to \a y.

    \sa y() setX()
*/


/*!
    \fn TQCOORD &TQPoint::rx()

    Returns a reference to the x coordinate of the point.

    Using a reference makes it possible to directly manipulate x.

    Example:
    \code
	TQPoint p( 1, 2 );
	p.rx()--;         // p becomes (0, 2)
    \endcode

    \sa ry()
*/

/*!
    \fn TQCOORD &TQPoint::ry()

    Returns a reference to the y coordinate of the point.

    Using a reference makes it possible to directly manipulate y.

    Example:
    \code
	TQPoint p( 1, 2 );
	p.ry()++;         // p becomes (1, 3)
    \endcode

    \sa rx()
*/


/*!
    \fn TQPoint &TQPoint::operator+=( const TQPoint &p )

    Adds point \a p to this point and returns a reference to this
    point.

    Example:
    \code
	TQPoint p(  3, 7 );
	TQPoint q( -1, 4 );
	p += q;            // p becomes (2,11)
    \endcode
*/

/*!
    \fn TQPoint &TQPoint::operator-=( const TQPoint &p )

    Subtracts point \a p from this point and returns a reference to
    this point.

    Example:
    \code
	TQPoint p(  3, 7 );
	TQPoint q( -1, 4 );
	p -= q;            // p becomes (4,3)
    \endcode
*/

/*!
    \fn TQPoint &TQPoint::operator*=( int c )

    Multiplies this point's x and y by \a c, and returns a reference
    to this point.

    Example:
    \code
	TQPoint p( -1, 4 );
	p *= 2;            // p becomes (-2,8)
    \endcode
*/

/*!
    \overload TQPoint &TQPoint::operator*=( double c )

    Multiplies this point's x and y by \a c, and returns a reference
    to this point.

    Example:
    \code
	TQPoint p( -1, 4 );
	p *= 2.5;          // p becomes (-3,10)
    \endcode

    Note that the result is truncated because points are held as
    integers.
*/


/*!
    \fn bool operator==( const TQPoint &p1, const TQPoint &p2 )

    \relates TQPoint

    Returns true if \a p1 and \a p2 are equal; otherwise returns false.
*/

/*!
    \fn bool operator!=( const TQPoint &p1, const TQPoint &p2 )

    \relates TQPoint

    Returns true if \a p1 and \a p2 are not equal; otherwise returns false.
*/

/*!
    \fn const TQPoint operator+( const TQPoint &p1, const TQPoint &p2 )

    \relates TQPoint

    Returns the sum of \a p1 and \a p2; each component is added separately.
*/

/*!
    \fn const TQPoint operator-( const TQPoint &p1, const TQPoint &p2 )

    \relates TQPoint

    Returns \a p2 subtracted from \a p1; each component is subtracted
    separately.
*/

/*!
    \fn const TQPoint operator*( const TQPoint &p, int c )

    \relates TQPoint

    Returns the TQPoint formed by multiplying both components of \a p
    by \a c.
*/

/*!
    \overload const TQPoint operator*( int c, const TQPoint &p )

    \relates TQPoint

    Returns the TQPoint formed by multiplying both components of \a p
    by \a c.
*/

/*!
    \overload const TQPoint operator*( const TQPoint &p, double c )

    \relates TQPoint

    Returns the TQPoint formed by multiplying both components of \a p
    by \a c.

    Note that the result is truncated because points are held as
    integers.
*/

/*!
    \overload const TQPoint operator*( double c, const TQPoint &p )

    \relates TQPoint

    Returns the TQPoint formed by multiplying both components of \a p
    by \a c.

    Note that the result is truncated because points are held as
    integers.
*/

/*!
    \overload const TQPoint operator-( const TQPoint &p )

    \relates TQPoint

    Returns the TQPoint formed by changing the sign of both components
    of \a p, equivalent to \c{TQPoint(0,0) - p}.
*/

/*!
    \fn TQPoint &TQPoint::operator/=( int c )

    Divides both x and y by \a c, and returns a reference to this
    point.

    Example:
    \code
	TQPoint p( -2, 8 );
	p /= 2;            // p becomes (-1,4)
    \endcode
*/

/*!
    \overload TQPoint &TQPoint::operator/=( double c )

    Divides both x and y by \a c, and returns a reference to this
    point.

    Example:
    \code
	TQPoint p( -3, 10 );
	p /= 2.5;           // p becomes (-1,4)
    \endcode

    Note that the result is truncated because points are held as
    integers.
*/

/*!
    \fn const TQPoint operator/( const TQPoint &p, int c )

    \relates TQPoint

    Returns the TQPoint formed by dividing both components of \a p by
    \a c.
*/

/*!
    \overload const TQPoint operator/( const TQPoint &p, double c )

    \relates TQPoint

    Returns the TQPoint formed by dividing both components of \a p
    by \a c.

    Note that the result is truncated because points are held as
    integers.
*/


void TQPoint::warningDivByZero()
{
#if defined(QT_CHECK_MATH)
    tqWarning( "TQPoint: Division by zero error" );
#endif
}


/*****************************************************************************
  TQPoint stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
/*!
    \relates TQPoint

    Writes point \a p to the stream \a s and returns a reference to
    the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQPoint &p )
{
    if ( s.version() == 1 )
	s << (TQ_INT16)p.x() << (TQ_INT16)p.y();
    else
	s << (TQ_INT32)p.x() << (TQ_INT32)p.y();
    return s;
}

/*!
    \relates TQPoint

    Reads a TQPoint from the stream \a s into point \a p and returns a
    reference to the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQPoint &p )
{
    if ( s.version() == 1 ) {
	TQ_INT16 x, y;
	s >> x;  p.rx() = x;
	s >> y;  p.ry() = y;
    }
    else {
	TQ_INT32 x, y;
	s >> x;  p.rx() = x;
	s >> y;  p.ry() = y;
    }
    return s;
}
#endif // TQT_NO_DATASTREAM
/*!
    Returns the sum of the absolute values of x() and y(),
    traditionally known as the "Manhattan length" of the vector from
    the origin to the point. The tradition arises because such
    distances apply to travelers who can only travel on a rectangular
    grid, like the streets of Manhattan.

    This is a useful, and quick to calculate, approximation to the
    true length: sqrt(pow(x(),2)+pow(y(),2)).
*/
int TQPoint::manhattanLength() const
{
    return TQABS(x())+TQABS(y());
}
