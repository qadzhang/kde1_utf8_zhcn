/****************************************************************************
**
** Implementation of TQRect class
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

#define	 TQRECT_C
#include "ntqrect.h"
#include "ntqdatastream.h"

/*!
    \class TQRect
    \brief The TQRect class defines a rectangle in the plane.

    \ingroup images
    \ingroup graphics
    \mainclass

    A rectangle is internally represented as an upper-left corner and
    a bottom-right corner, but it is normally expressed as an
    upper-left corner and a size.

    The coordinate type is TQCOORD (defined in \c ntqwindowdefs.h as \c
    int). The minimum value of TQCOORD is TQCOORD_MIN (-2147483648) and
    the maximum value is  TQCOORD_MAX (2147483647).

    Note that the size (width and height) of a rectangle might be
    different from what you are used to. If the top-left corner and
    the bottom-right corner are the same, the height and the width of
    the rectangle will both be 1.

    Generally, \e{width = right - left + 1} and \e{height = bottom -
    top + 1}. We designed it this way to make it correspond to
    rectangular spaces used by drawing functions in which the width
    and height denote a number of pixels. For example, drawing a
    rectangle with width and height 1 draws a single pixel.

    The default coordinate system has origin (0, 0) in the top-left
    corner. The positive direction of the y axis is down, and the
    positive x axis is from left to right.

    A TQRect can be constructed with a set of left, top, width and
    height integers, from two TQPoints or from a TQPoint and a TQSize.
    After creation the dimensions can be changed, e.g. with setLeft(),
    setRight(), setTop() and setBottom(), or by setting sizes, e.g.
    setWidth(), setHeight() and setSize(). The dimensions can also be
    changed with the move functions, e.g. moveBy(), moveCenter(),
    moveBottomRight(), etc. You can also add coordinates to a
    rectangle with addCoords().

    You can test to see if a TQRect contains a specific point with
    contains(). You can also test to see if two TQRects intersect with
    intersects() (see also intersect()). To get the bounding rectangle
    of two TQRects use unite().

    \sa TQPoint, TQSize
*/


/*****************************************************************************
  TQRect member functions
 *****************************************************************************/

/*!
    \fn TQRect::TQRect()

    Constructs an invalid rectangle.
*/

/*!
    Constructs a rectangle with \a topLeft as the top-left corner and
    \a bottomRight as the bottom-right corner.
*/

TQRect::TQRect( const TQPoint &topLeft, const TQPoint &bottomRight )
{
    x1 = (TQCOORD)topLeft.x();
    y1 = (TQCOORD)topLeft.y();
    x2 = (TQCOORD)bottomRight.x();
    y2 = (TQCOORD)bottomRight.y();
}

/*!
    Constructs a rectangle with \a topLeft as the top-left corner and
    \a size as the rectangle size.
*/

TQRect::TQRect( const TQPoint &topLeft, const TQSize &size )
{
    x1 = (TQCOORD)topLeft.x();
    y1 = (TQCOORD)topLeft.y();
    x2 = (TQCOORD)(x1+size.width()-1);
    y2 = (TQCOORD)(y1+size.height()-1);
}

/*!
    \fn TQRect::TQRect( int left, int top, int width, int height )

    Constructs a rectangle with the \a top, \a left corner and \a
    width and \a height.

    Example (creates three identical rectangles):
    \code
	TQRect r1( TQPoint(100,200), TQPoint(110,215) );
	TQRect r2( TQPoint(100,200), TQSize(11,16) );
	TQRect r3( 100, 200, 11, 16 );
    \endcode
*/


/*!
    \fn bool TQRect::isNull() const

    Returns true if the rectangle is a null rectangle; otherwise
    returns false.

    A null rectangle has both the width and the height set to 0, that
    is right() == left() - 1 and bottom() == top() - 1.

    Note that if right() == left() and bottom() == top(), then the
    rectangle has width 1 and height 1.

    A null rectangle is also empty.

    A null rectangle is not valid.

    \sa isEmpty(), isValid()
*/

/*!
    \fn bool TQRect::isEmpty() const

    Returns true if the rectangle is empty; otherwise returns false.

    An empty rectangle has a left() \> right() or top() \> bottom().

    An empty rectangle is not valid. \c{isEmpty() == !isValid()}

    \sa isNull(), isValid(), normalize()
*/

/*!
    \fn bool TQRect::isValid() const

    Returns true if the rectangle is valid; otherwise returns false.

    A valid rectangle has a left() \<= right() and top() \<= bottom().

    Note that non-trivial operations like intersections are not defined
    for invalid rectangles.

    \c{isValid() == !isEmpty()}

    \sa isNull(), isEmpty(), normalize()
*/


/*!
    Returns a normalized rectangle, i.e. a rectangle that has a
    non-negative width and height.

    It swaps left and right if left() \> right(), and swaps top and
    bottom if top() \> bottom().

    \sa isValid()
*/

TQRect TQRect::normalize() const
{
    TQRect r;
    if ( x2 < x1 ) {				// swap bad x values
	r.x1 = x2;
	r.x2 = x1;
    } else {
	r.x1 = x1;
	r.x2 = x2;
    }
    if ( y2 < y1 ) {				// swap bad y values
	r.y1 = y2;
	r.y2 = y1;
    } else {
	r.y1 = y1;
	r.y2 = y2;
    }
    return r;
}


/*!
    \fn int TQRect::left() const

    Returns the left coordinate of the rectangle. Identical to x().

    \sa setLeft(), right(), topLeft(), bottomLeft()
*/

/*!
    \fn int TQRect::top() const

    Returns the top coordinate of the rectangle. Identical to y().

    \sa setTop(), bottom(), topLeft(), topRight()
*/

/*!
    \fn int TQRect::right() const

    Returns the right coordinate of the rectangle.

    \sa setRight(), left(), topRight(), bottomRight()
*/

/*!
    \fn int TQRect::bottom() const

    Returns the bottom coordinate of the rectangle.

    \sa setBottom(), top(), bottomLeft(), bottomRight()
*/

/*!
    \fn TQCOORD &TQRect::rLeft()

    Returns a reference to the left coordinate of the rectangle.

    \sa rTop(), rRight(), rBottom()
*/

/*!
    \fn TQCOORD &TQRect::rTop()

    Returns a reference to the top coordinate of the rectangle.

    \sa rLeft(),  rRight(), rBottom()
*/

/*!
    \fn TQCOORD &TQRect::rRight()

    Returns a reference to the right coordinate of the rectangle.

    \sa rLeft(), rTop(), rBottom()
*/

/*!
    \fn TQCOORD &TQRect::rBottom()

    Returns a reference to the bottom coordinate of the rectangle.

    \sa rLeft(), rTop(), rRight()
*/

/*!
    \fn int TQRect::x() const

    Returns the left coordinate of the rectangle. Identical to left().

    \sa left(), y(), setX()
*/

/*!
    \fn int TQRect::y() const

    Returns the top coordinate of the rectangle. Identical to top().

    \sa top(), x(), setY()
*/

/*!
    \fn void TQRect::setLeft( int pos )

    Sets the left edge of the rectangle to \a pos. May change the
    width, but will never change the right edge of the rectangle.

    Identical to setX().

    \sa left(), setTop(), setWidth()
*/

/*!
    \fn void TQRect::setTop( int pos )

    Sets the top edge of the rectangle to \a pos. May change the
    height, but will never change the bottom edge of the rectangle.

    Identical to setY().

    \sa top(), setBottom(), setHeight()
*/

/*!
    \fn void TQRect::setRight( int pos )

    Sets the right edge of the rectangle to \a pos. May change the
    width, but will never change the left edge of the rectangle.

    \sa right(), setLeft(), setWidth()
*/

/*!
    \fn void TQRect::setBottom( int pos )

    Sets the bottom edge of the rectangle to \a pos. May change the
    height, but will never change the top edge of the rectangle.

    \sa bottom(), setTop(), setHeight()
*/

/*!
    \fn void TQRect::setX( int x )

    Sets the x position of the rectangle (its left end) to \a x. May
    change the width, but will never change the right edge of the
    rectangle.

    Identical to setLeft().

    \sa x(), setY()
*/

/*!
    \fn void TQRect::setY( int y )

    Sets the y position of the rectangle (its top) to \a y. May change
    the height, but will never change the bottom edge of the
    rectangle.

    Identical to setTop().

    \sa y(), setX()
*/

/*!
    Set the top-left corner of the rectangle to \a p. May change
    the size, but will the never change the bottom-right corner of
    the rectangle.

    \sa topLeft(), moveTopLeft(), setBottomRight(), setTopRight(), setBottomLeft()
*/
void TQRect::setTopLeft( const TQPoint &p )
{
    setLeft( p.x() );
    setTop( p.y() );
}

/*!
    Set the bottom-right corner of the rectangle to \a p. May change
    the size, but will the never change the top-left corner of
    the rectangle.

    \sa bottomRight(), moveBottomRight(), setTopLeft(), setTopRight(), setBottomLeft()
*/
void TQRect::setBottomRight( const TQPoint &p )
{
    setRight( p.x() );
    setBottom( p.y() );
}

/*!
    Set the top-right corner of the rectangle to \a p. May change
    the size, but will the never change the bottom-left corner of
    the rectangle.

    \sa topRight(), moveTopRight(), setTopLeft(), setBottomRight(), setBottomLeft()
*/
void TQRect::setTopRight( const TQPoint &p )
{
    setRight( p.x() );
    setTop( p.y() );
}

/*!
    Set the bottom-left corner of the rectangle to \a p. May change
    the size, but will the never change the top-right corner of
    the rectangle.

    \sa bottomLeft(), moveBottomLeft(), setTopLeft(), setBottomRight(), setTopRight()
*/
void TQRect::setBottomLeft( const TQPoint &p )
{
    setLeft( p.x() );
    setBottom( p.y() );
}

/*!
    \fn TQPoint TQRect::topLeft() const

    Returns the top-left position of the rectangle.

    \sa setTopLeft(), moveTopLeft(), bottomRight(), left(), top()
*/

/*!
    \fn TQPoint TQRect::bottomRight() const

    Returns the bottom-right position of the rectangle.

    \sa setBottomRight(), moveBottomRight(), topLeft(), right(), bottom()
*/

/*!
    \fn TQPoint TQRect::topRight() const

    Returns the top-right position of the rectangle.

    \sa setTopRight(), moveTopRight(), bottomLeft(), top(), right()
*/

/*!
    \fn TQPoint TQRect::bottomLeft() const

    Returns the bottom-left position of the rectangle.

    \sa setBottomLeft(), moveBottomLeft(), topRight(), bottom(), left()
*/

/*!
    \fn TQPoint TQRect::center() const

    Returns the center point of the rectangle.

    \sa moveCenter(), topLeft(), bottomRight(), topRight(), bottomLeft()
*/


/*!
    Extracts the rectangle parameters as the position \a *x, \a *y and
    width \a *w and height \a *h.

    \sa setRect(), coords()
*/

void TQRect::rect( int *x, int *y, int *w, int *h ) const
{
    *x = x1;
    *y = y1;
    *w = x2-x1+1;
    *h = y2-y1+1;
}

/*!
    Extracts the rectangle parameters as the top-left point \a *xp1,
    \a *yp1 and the bottom-right point \a *xp2, \a *yp2.

    \sa setCoords(), rect()
*/

void TQRect::coords( int *xp1, int *yp1, int *xp2, int *yp2 ) const
{
    *xp1 = x1;
    *yp1 = y1;
    *xp2 = x2;
    *yp2 = y2;
}


/*!
    Sets the left position of the rectangle to \a pos, leaving the
    size unchanged.

    \sa left(), setLeft(), moveTop(), moveRight(), moveBottom()
*/
void TQRect::moveLeft( int pos )
{
    x2 += (TQCOORD)(pos - x1);
    x1 = (TQCOORD)pos;
}

/*!
    Sets the top position of the rectangle to \a pos, leaving the
    size unchanged.

    \sa top(), setTop(), moveLeft(), moveRight(), moveBottom()
*/

void TQRect::moveTop( int pos )
{
    y2 += (TQCOORD)(pos - y1);
    y1 = (TQCOORD)pos;
}

/*!
    Sets the right position of the rectangle to \a pos, leaving the
    size unchanged.

    \sa right(), setRight(), moveLeft(), moveTop(), moveBottom()
*/

void TQRect::moveRight( int pos )
{
    x1 += (TQCOORD)(pos - x2);
    x2 = (TQCOORD)pos;
}

/*!
    Sets the bottom position of the rectangle to \a pos, leaving the
    size unchanged.

    \sa bottom(), setBottom(), moveLeft(), moveTop(), moveRight()
*/

void TQRect::moveBottom( int pos )
{
    y1 += (TQCOORD)(pos - y2);
    y2 = (TQCOORD)pos;
}

/*!
    Sets the top-left position of the rectangle to \a p, leaving the
    size unchanged.

    \sa topLeft(), setTopLeft(), moveBottomRight(), moveTopRight(), moveBottomLeft()
*/

void TQRect::moveTopLeft( const TQPoint &p )
{
    moveLeft( p.x() );
    moveTop( p.y() );
}

/*!
    Sets the bottom-right position of the rectangle to \a p, leaving
    the size unchanged.

    \sa bottomRight(), setBottomRight(), moveTopLeft(), moveTopRight(), moveBottomLeft()
*/

void TQRect::moveBottomRight( const TQPoint &p )
{
    moveRight( p.x() );
    moveBottom( p.y() );
}

/*!
    Sets the top-right position of the rectangle to \a p, leaving the
    size unchanged.

    \sa topRight(), setTopRight(), moveTopLeft(), moveBottomRight(), moveBottomLeft()
*/

void TQRect::moveTopRight( const TQPoint &p )
{
    moveRight( p.x() );
    moveTop( p.y() );
}

/*!
    Sets the bottom-left position of the rectangle to \a p, leaving
    the size unchanged.

    \sa bottomLeft(), setBottomLeft(), moveTopLeft(), moveBottomRight(), moveTopRight()
*/

void TQRect::moveBottomLeft( const TQPoint &p )
{
    moveLeft( p.x() );
    moveBottom( p.y() );
}


/*!
    Sets the center point of the rectangle to \a p, leaving the size
    unchanged.

    \sa center(), moveTopLeft(), moveBottomRight(), moveTopRight(), moveBottomLeft()
*/

void TQRect::moveCenter( const TQPoint &p )
{
    TQCOORD w = x2 - x1;
    TQCOORD h = y2 - y1;
    x1 = (TQCOORD)(p.x() - w/2);
    y1 = (TQCOORD)(p.y() - h/2);
    x2 = x1 + w;
    y2 = y1 + h;
}


/*!
    Moves the rectangle \a dx along the x axis and \a dy along the y
    axis, relative to the current position. Positive values move the
    rectangle to the right and down.

    \sa moveTopLeft()
*/

void TQRect::moveBy( int dx, int dy )
{
    x1 += (TQCOORD)dx;
    y1 += (TQCOORD)dy;
    x2 += (TQCOORD)dx;
    y2 += (TQCOORD)dy;
}

/*!
    Sets the coordinates of the rectangle's top-left corner to \a (x,
    y), and its size to \a (w, h).

    \sa rect(), setCoords()
*/

void TQRect::setRect( int x, int y, int w, int h )
{
    x1 = (TQCOORD)x;
    y1 = (TQCOORD)y;
    x2 = (TQCOORD)(x+w-1);
    y2 = (TQCOORD)(y+h-1);
}

/*!
    Sets the coordinates of the rectangle's top-left corner to \a
    (xp1, yp1), and the coordinates of its bottom-right corner to \a
    (xp2, yp2).

    \sa coords(), setRect()
*/

void TQRect::setCoords( int xp1, int yp1, int xp2, int yp2 )
{
    x1 = (TQCOORD)xp1;
    y1 = (TQCOORD)yp1;
    x2 = (TQCOORD)xp2;
    y2 = (TQCOORD)yp2;
}

/*!
    Adds \a xp1, \a yp1, \a xp2 and \a yp2 respectively to the
    existing coordinates of the rectangle.
*/

void TQRect::addCoords( int xp1, int yp1, int xp2, int yp2 )
{
    x1 += (TQCOORD)xp1;
    y1 += (TQCOORD)yp1;
    x2 += (TQCOORD)xp2;
    y2 += (TQCOORD)yp2;
}

/*!
    \fn TQSize TQRect::size() const

    Returns the size of the rectangle.

    \sa width(), height()
*/

/*!
    \fn int TQRect::width() const

    Returns the width of the rectangle. The width includes both the
    left and right edges, i.e. width = right - left + 1.

    \sa height(), size(), setHeight()
*/

/*!
    \fn int TQRect::height() const

    Returns the height of the rectangle. The height includes both the
    top and bottom edges, i.e. height = bottom - top + 1.

    \sa width(), size(), setHeight()
*/

/*!
    Sets the width of the rectangle to \a w. The right edge is
    changed, but not the left edge.

    \sa width(), setLeft(), setRight(), setSize()
*/

void TQRect::setWidth( int w )
{
    x2 = (TQCOORD)(x1 + w - 1);
}

/*!
    Sets the height of the rectangle to \a h. The top edge is not
    moved, but the bottom edge may be moved.

    \sa height(), setTop(), setBottom(), setSize()
*/

void TQRect::setHeight( int h )
{
    y2 = (TQCOORD)(y1 + h - 1);
}

/*!
    Sets the size of the rectangle to \a s. The top-left corner is not
    moved.

    \sa size(), setWidth(), setHeight()
*/

void TQRect::setSize( const TQSize &s )
{
    x2 = (TQCOORD)(s.width() +x1-1);
    y2 = (TQCOORD)(s.height()+y1-1);
}

/*!
    Returns true if the point \a p is inside or on the edge of the
    rectangle; otherwise returns false.

    If \a proper is true, this function returns true only if \a p is
    inside (not on the edge).
*/

bool TQRect::contains( const TQPoint &p, bool proper ) const
{
    if ( proper )
	return p.x() > x1 && p.x() < x2 &&
	       p.y() > y1 && p.y() < y2;
    else
	return p.x() >= x1 && p.x() <= x2 &&
	       p.y() >= y1 && p.y() <= y2;
}

/*!
    \overload bool TQRect::contains( int x, int y, bool proper ) const

    Returns true if the point \a x, \a y is inside this rectangle;
    otherwise returns false.

    If \a proper is true, this function returns true only if the point
    is entirely inside (not on the edge).
*/

/*!
    \overload bool TQRect::contains( int x, int y ) const

    Returns true if the point \a x, \a y is inside this rectangle;
    otherwise returns false.
*/

/*!
    \overload

    Returns true if the rectangle \a r is inside this rectangle;
    otherwise returns false.

    If \a proper is true, this function returns true only if \a r is
    entirely inside (not on the edge).

    \sa unite(), intersect(), intersects()
*/

bool TQRect::contains( const TQRect &r, bool proper ) const
{
    if ( proper )
	return r.x1 > x1 && r.x2 < x2 && r.y1 > y1 && r.y2 < y2;
    else
	return r.x1 >= x1 && r.x2 <= x2 && r.y1 >= y1 && r.y2 <= y2;
}

/*!
    Unites this rectangle with rectangle \a r.
*/
TQRect& TQRect::operator|=(const TQRect &r)
{
    *this = *this | r;
    return *this;
}

/*!
    Intersects this rectangle with rectangle \a r.
*/
TQRect& TQRect::operator&=(const TQRect &r)
{
    *this = *this & r;
    return *this;
}


/*!
    Returns the bounding rectangle of this rectangle and rectangle \a
    r.

    The bounding rectangle of a nonempty rectangle and an empty or
    invalid rectangle is defined to be the nonempty rectangle.

    \sa operator|=(), operator&(), intersects(), contains()
*/

TQRect TQRect::operator|(const TQRect &r) const
{
    if ( isValid() ) {
	if ( r.isValid() ) {
	    TQRect tmp;
	    tmp.setLeft(   TQMIN( x1, r.x1 ) );
	    tmp.setRight(  TQMAX( x2, r.x2 ) );
	    tmp.setTop(	   TQMIN( y1, r.y1 ) );
	    tmp.setBottom( TQMAX( y2, r.y2 ) );
	    return tmp;
	} else {
	    return *this;
	}
    } else {
	return r;
    }
}

/*!
    Returns the bounding rectangle of this rectangle and rectangle \a
    r. \c{r.unite(s)} is equivalent to \c{r|s}.
*/
TQRect TQRect::unite( const TQRect &r ) const
{
    return *this | r;
}


/*!
    Returns the intersection of this rectangle and rectangle \a r.

    Returns an empty rectangle if there is no intersection.

    \sa operator&=(), operator|(), isEmpty(), intersects(), contains()
*/

TQRect TQRect::operator&( const TQRect &r ) const
{
    TQRect tmp;
    tmp.x1 = TQMAX( x1, r.x1 );
    tmp.x2 = TQMIN( x2, r.x2 );
    tmp.y1 = TQMAX( y1, r.y1 );
    tmp.y2 = TQMIN( y2, r.y2 );
    return tmp;
}

/*!
    Returns the intersection of this rectangle and rectangle \a r.
    \c{r.intersect(s)} is equivalent to \c{r&s}.
*/
TQRect TQRect::intersect( const TQRect &r ) const
{
    return *this & r;
}

/*!
    Returns true if this rectangle intersects with rectangle \a r
    (there is at least one pixel that is within both rectangles);
    otherwise returns false.

    \sa intersect(), contains()
*/

bool TQRect::intersects( const TQRect &r ) const
{
    return ( TQMAX( x1, r.x1 ) <= TQMIN( x2, r.x2 ) &&
	     TQMAX( y1, r.y1 ) <= TQMIN( y2, r.y2 ) );
}


/*!
    \relates TQRect

    Returns true if \a r1 and \a r2 are equal; otherwise returns false.
*/

bool operator==( const TQRect &r1, const TQRect &r2 )
{
    return r1.x1==r2.x1 && r1.x2==r2.x2 && r1.y1==r2.y1 && r1.y2==r2.y2;
}

/*!
    \relates TQRect

    Returns true if \a r1 and \a r2 are different; otherwise returns false.
*/

bool operator!=( const TQRect &r1, const TQRect &r2 )
{
    return r1.x1!=r2.x1 || r1.x2!=r2.x2 || r1.y1!=r2.y1 || r1.y2!=r2.y2;
}


/*****************************************************************************
  TQRect stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
/*!
    \relates TQRect

    Writes the TQRect, \a r, to the stream \a s, and returns a
    reference to the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQRect &r )
{
    if ( s.version() == 1 )
	s << (TQ_INT16)r.left() << (TQ_INT16)r.top()
	  << (TQ_INT16)r.right() << (TQ_INT16)r.bottom();
    else
	s << (TQ_INT32)r.left() << (TQ_INT32)r.top()
	  << (TQ_INT32)r.right() << (TQ_INT32)r.bottom();
    return s;
}

/*!
    \relates TQRect

    Reads a TQRect from the stream \a s into rect \a r and returns a
    reference to the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQRect &r )
{
    if ( s.version() == 1 ) {
	TQ_INT16 x1, y1, x2, y2;
	s >> x1; s >> y1; s >> x2; s >> y2;
	r.setCoords( x1, y1, x2, y2 );
    }
    else {
	TQ_INT32 x1, y1, x2, y2;
	s >> x1; s >> y1; s >> x2; s >> y2;
	r.setCoords( x1, y1, x2, y2 );
    }
    return s;
}
#endif // TQT_NO_DATASTREAM
