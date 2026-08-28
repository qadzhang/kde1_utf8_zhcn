/****************************************************************************
**
** Implementation of TQSize class
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

#include "ntqsize.h"
#include "ntqdatastream.h"


/*!
  \class TQSize
  \brief The TQSize class defines the size of a two-dimensional object.

  \ingroup images
  \ingroup graphics

  A size is specified by a width and a height.

  The coordinate type is TQCOORD (defined in \c <ntqwindowdefs.h> as \c int).
  The minimum value of TQCOORD is TQCOORD_MIN (-2147483648) and the maximum
  value is TQCOORD_MAX (2147483647).

  The size can be set in the constructor and changed with setWidth()
  and setHeight(), or using operator+=(), operator-=(), operator*=()
  and operator/=(), etc. You can swap the width and height with
  transpose(). You can get a size which holds the maximum height and
  width of two sizes using expandedTo(), and the minimum height and
  width of two sizes using boundedTo().


  \sa TQPoint, TQRect
*/


/*****************************************************************************
  TQSize member functions
 *****************************************************************************/

/*!
  \fn TQSize::TQSize()
  Constructs a size with invalid (negative) width and height.
*/

/*!
  \fn TQSize::TQSize( int w, int h )
  Constructs a size with width \a w and height \a h.
*/

/*!
  \fn bool TQSize::isNull() const
  Returns true if the width is 0 and the height is 0; otherwise
  returns false.
*/

/*!
  \fn bool TQSize::isEmpty() const
  Returns true if the width is less than or equal to 0, or the height is
  less than or equal to 0; otherwise returns false.
*/

/*!
  \fn bool TQSize::isValid() const
  Returns true if the width is equal to or greater than 0 and the height is
  equal to or greater than 0; otherwise returns false.
*/

/*!
  \fn int TQSize::width() const
  Returns the width.
  \sa height()
*/

/*!
  \fn int TQSize::height() const
  Returns the height.
  \sa width()
*/

/*!
  \fn void TQSize::setWidth( int w )
  Sets the width to \a w.
  \sa width(), setHeight()
*/

/*!
  \fn void TQSize::setHeight( int h )
  Sets the height to \a h.
  \sa height(), setWidth()
*/

/*!
  Swaps the values of width and height.
*/

void TQSize::transpose()
{
    TQCOORD tmp = wd;
    wd = ht;
    ht = tmp;
}

/*! \enum TQSize::ScaleMode

    This enum type defines the different ways of scaling a size.

    \img scaling.png

    \value ScaleFree  The size is scaled freely. The ratio is not preserved.
    \value ScaleMin  The size is scaled to a rectangle as large as possible
                     inside a given rectangle, preserving the aspect ratio.
    \value ScaleMax  The size is scaled to a rectangle as small as possible
                     outside a given rectangle, preserving the aspect ratio.

    \sa TQSize::scale(), TQImage::scale(), TQImage::smoothScale()
*/

/*!
    Scales the size to a rectangle of width \a w and height \a h according
    to the ScaleMode \a mode.

    \list
    \i If \a mode is \c ScaleFree, the size is set to (\a w, \a h).
    \i If \a mode is \c ScaleMin, the current size is scaled to a rectangle
       as large as possible inside (\a w, \a h), preserving the aspect ratio.
    \i If \a mode is \c ScaleMax, the current size is scaled to a rectangle
       as small as possible outside (\a w, \a h), preserving the aspect ratio.
    \endlist

    Example:
    \code
    TQSize t1( 10, 12 );
    t1.scale( 60, 60, TQSize::ScaleFree );
    // t1 is (60, 60)

    TQSize t2( 10, 12 );
    t2.scale( 60, 60, TQSize::ScaleMin );
    // t2 is (50, 60)

    TQSize t3( 10, 12 );
    t3.scale( 60, 60, TQSize::ScaleMax );
    // t3 is (60, 72)
    \endcode
*/
void TQSize::scale( int w, int h, ScaleMode mode )
{
    if ( mode == ScaleFree ) {
	wd = (TQCOORD)w;
	ht = (TQCOORD)h;
    } else {
	bool useHeight = true;
	int w0 = width();
	int h0 = height();
	int rw = h * w0 / h0;

	if ( mode == ScaleMin ) {
	    useHeight = ( rw <= w );
	} else { // mode == ScaleMax
	    useHeight = ( rw >= w );
	}

	if ( useHeight ) {
	    wd = (TQCOORD)rw;
	    ht = (TQCOORD)h;
	} else {
	    wd = (TQCOORD)w;
	    ht = (TQCOORD)( w * h0 / w0 );
	}
    }
}

/*!
    \overload

    Equivalent to scale(\a{s}.width(), \a{s}.height(), \a mode).
*/
void TQSize::scale( const TQSize &s, ScaleMode mode )
{
    scale( s.width(), s.height(), mode );
}

/*!
  \fn TQCOORD &TQSize::rwidth()
  Returns a reference to the width.

  Using a reference makes it possible to directly manipulate the width.

  Example:
  \code
    TQSize s( 100, 10 );
    s.rwidth() += 20;		// s becomes (120,10)
  \endcode

  \sa rheight()
*/

/*!
  \fn TQCOORD &TQSize::rheight()
  Returns a reference to the height.

  Using a reference makes it possible to directly manipulate the height.

  Example:
  \code
    TQSize s( 100, 10 );
    s.rheight() += 5;		// s becomes (100,15)
  \endcode

  \sa rwidth()
*/

/*!
  \fn TQSize &TQSize::operator+=( const TQSize &s )

  Adds \a s to the size and returns a reference to this size.

  Example:
  \code
    TQSize s(  3, 7 );
    TQSize r( -1, 4 );
    s += r;			// s becomes (2,11)
\endcode
*/

/*!
  \fn TQSize &TQSize::operator-=( const TQSize &s )

  Subtracts \a s from the size and returns a reference to this size.

  Example:
  \code
    TQSize s(  3, 7 );
    TQSize r( -1, 4 );
    s -= r;			// s becomes (4,3)
  \endcode
*/

/*!
  \fn TQSize &TQSize::operator*=( int c )
  Multiplies both the width and height by \a c and returns a reference to
  the size.
*/

/*!
  \overload TQSize &TQSize::operator*=( double c )

  Multiplies both the width and height by \a c and returns a reference to
  the size.

  Note that the result is truncated.
*/

/*!
  \fn bool operator==( const TQSize &s1, const TQSize &s2 )
  \relates TQSize
  Returns true if \a s1 and \a s2 are equal; otherwise returns false.
*/

/*!
  \fn bool operator!=( const TQSize &s1, const TQSize &s2 )
  \relates TQSize
  Returns true if \a s1 and \a s2 are different; otherwise returns false.
*/

/*!
  \fn const TQSize operator+( const TQSize &s1, const TQSize &s2 )
  \relates TQSize
  Returns the sum of \a s1 and \a s2; each component is added separately.
*/

/*!
  \fn const TQSize operator-( const TQSize &s1, const TQSize &s2 )
  \relates TQSize
  Returns \a s2 subtracted from \a s1; each component is
  subtracted separately.
*/

/*!
  \fn const TQSize operator*( const TQSize &s, int c )
  \relates TQSize
  Multiplies \a s by \a c and returns the result.
*/

/*!
  \overload const TQSize operator*( int c, const TQSize &s )
  \relates TQSize
  Multiplies \a s by \a c and returns the result.
*/

/*!
  \overload const TQSize operator*( const TQSize &s, double c )
  \relates TQSize
  Multiplies \a s by \a c and returns the result.
*/

/*!
  \overload const TQSize operator*( double c, const TQSize &s )
  \relates TQSize
  Multiplies \a s by \a c and returns the result.
*/

/*!
  \fn TQSize &TQSize::operator/=( int c )
  Divides both the width and height by \a c and returns a reference to the
  size.
*/

/*!
  \fn TQSize &TQSize::operator/=( double c )
  \overload
  Divides both the width and height by \a c and returns a reference to the
  size.

  Note that the result is truncated.
*/

/*!
  \fn const TQSize operator/( const TQSize &s, int c )
  \relates TQSize
  Divides \a s by \a c and returns the result.
*/

/*!
  \fn const TQSize operator/( const TQSize &s, double c )
  \relates TQSize
  \overload
  Divides \a s by \a c and returns the result.

  Note that the result is truncated.
*/

/*!
  \fn TQSize TQSize::expandedTo( const TQSize & otherSize ) const

  Returns a size with the maximum width and height of this size and
  \a otherSize.
*/

/*!
  \fn TQSize TQSize::boundedTo( const TQSize & otherSize ) const

  Returns a size with the minimum width and height of this size and
  \a otherSize.
*/


void TQSize::warningDivByZero()
{
#if defined(QT_CHECK_MATH)
    tqWarning( "TQSize: Division by zero error" );
#endif
}


/*****************************************************************************
  TQSize stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
/*!
  \relates TQSize
  Writes the size \a sz to the stream \a s and returns a reference to
  the stream.

  \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQSize &sz )
{
    if ( s.version() == 1 )
	s << (TQ_INT16)sz.width() << (TQ_INT16)sz.height();
    else
	s << (TQ_INT32)sz.width() << (TQ_INT32)sz.height();
    return s;
}

/*!
  \relates TQSize
  Reads the size from the stream \a s into size \a sz and returns a
  reference to the stream.

  \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQSize &sz )
{
    if ( s.version() == 1 ) {
	TQ_INT16 w, h;
	s >> w;  sz.rwidth() = w;
	s >> h;  sz.rheight() = h;
    }
    else {
	TQ_INT32 w, h;
	s >> w;  sz.rwidth() = w;
	s >> h;  sz.rheight() = h;
    }
    return s;
}
#endif // TQT_NO_DATASTREAM
