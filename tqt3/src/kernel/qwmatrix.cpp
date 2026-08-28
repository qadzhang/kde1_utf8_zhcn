/****************************************************************************
**
** Implementation of TQWMatrix class
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

#include "ntqwmatrix.h"
#include "ntqdatastream.h"
#include "ntqregion.h"
#if defined(TQ_WS_X11)
double qsincos( double, bool calcCos );		// defined in qpainter_x11.cpp
#else
#include <math.h>
#endif

#include <limits.h>

#ifndef TQT_NO_WMATRIX

/*!
    \class TQWMatrix ntqwmatrix.h
    \brief The TQWMatrix class specifies 2D transformations of a
    coordinate system.

    \ingroup graphics
    \ingroup images

    The standard coordinate system of a \link TQPaintDevice paint
    device\endlink has the origin located at the top-left position. X
    values increase to the right; Y values increase downward.

    This coordinate system is the default for the TQPainter, which
    renders graphics in a paint device. A user-defined coordinate
    system can be specified by setting a TQWMatrix for the painter.

    Example:
    \code
	MyWidget::paintEvent( TQPaintEvent * )
	{
            TQPainter p;                      // our painter
            TQWMatrix m;                      // our transformation matrix
            m.rotate( 22.5 );                // rotated coordinate system
            p.begin( this );                 // start painting
            p.setWorldMatrix( m );           // use rotated coordinate system
            p.drawText( 30,20, "detator" );  // draw rotated text at 30,20
            p.end();                         // painting done
	}
    \endcode

    A matrix specifies how to translate, scale, shear or rotate the
    graphics; the actual transformation is performed by the drawing
    routines in TQPainter and by TQPixmap::xForm().

    The TQWMatrix class contains a 3x3 matrix of the form:
    <table align=center border=1 cellpadding=1 cellspacing=0>
    <tr align=center><td>m11</td><td>m12</td><td>&nbsp;0 </td></tr>
    <tr align=center><td>m21</td><td>m22</td><td>&nbsp;0 </td></tr>
    <tr align=center><td>dx</td> <td>dy</td> <td>&nbsp;1 </td></tr>
    </table>

    A matrix transforms a point in the plane to another point:
    \code
	x' = m11*x + m21*y + dx
	y' = m22*y + m12*x + dy
    \endcode

    The point \e (x, y) is the original point, and \e (x', y') is the
    transformed point. \e (x', y') can be transformed back to \e (x,
    y) by performing the same operation on the \link
    TQWMatrix::invert() inverted matrix\endlink.

    The elements \e dx and \e dy specify horizontal and vertical
    translation. The elements \e m11 and \e m22 specify horizontal and
    vertical scaling. The elements \e m12 and \e m21 specify
    horizontal and vertical shearing.

    The identity matrix has \e m11 and \e m22 set to 1; all others are
    set to 0. This matrix maps a point to itself.

    Translation is the simplest transformation. Setting \e dx and \e
    dy will move the coordinate system \e dx units along the X axis
    and \e dy units along the Y axis.

    Scaling can be done by setting \e m11 and \e m22. For example,
    setting \e m11 to 2 and \e m22 to 1.5 will double the height and
    increase the width by 50%.

    Shearing is controlled by \e m12 and \e m21. Setting these
    elements to values different from zero will twist the coordinate
    system.

    Rotation is achieved by carefully setting both the shearing
    factors and the scaling factors. The TQWMatrix also has a function
    that sets \link rotate() rotation \endlink directly.

    TQWMatrix lets you combine transformations like this:
    \code
	TQWMatrix m;           // identity matrix
	m.translate(10, -20); // first translate (10,-20)
	m.rotate(25);         // then rotate 25 degrees
	m.scale(1.2, 0.7);    // finally scale it
    \endcode

    Here's the same example using basic matrix operations:
    \code
	double a    = pi/180 * 25;         // convert 25 to radians
	double sina = sin(a);
	double cosa = cos(a);
	TQWMatrix m1(1, 0, 0, 1, 10, -20);  // translation matrix
	TQWMatrix m2( cosa, sina,           // rotation matrix
		    -sina, cosa, 0, 0 );
	TQWMatrix m3(1.2, 0, 0, 0.7, 0, 0); // scaling matrix
	TQWMatrix m;
	m = m3 * m2 * m1;                  // combine all transformations
    \endcode

    \l TQPainter has functions to translate, scale, shear and rotate the
    coordinate system without using a TQWMatrix. Although these
    functions are very convenient, it can be more efficient to build a
    TQWMatrix and call TQPainter::setWorldMatrix() if you want to perform
    more than a single transform operation.

    \sa TQPainter::setWorldMatrix(), TQPixmap::xForm()
*/

bool tqt_old_transformations = true;

/*!
    \enum TQWMatrix::TransformationMode

    \keyword transformation matrix

    TQWMatrix offers two transformation modes. Calculations can either
    be done in terms of points (Points mode, the default), or in
    terms of area (Area mode).

    In Points mode the transformation is applied to the points that
    mark out the shape's bounding line. In Areas mode the
    transformation is applied in such a way that the area of the
    contained region is correctly transformed under the matrix.

    \value Points transformations are applied to the shape's points.
    \value Areas transformations are applied (e.g. to the width and
    height) so that the area is transformed.

    Example:

    Suppose we have a rectangle,
    \c{TQRect( 10, 20, 30, 40 )} and a transformation matrix
    \c{TQWMatrix( 2, 0, 0, 2, 0, 0 )} to double the rectangle's size.

    In Points mode, the matrix will transform the top-left (10,20) and
    the bottom-right (39,59) points producing a rectangle with its
    top-left point at (20,40) and its bottom-right point at (78,118),
    i.e. with a width of 59 and a height of 79.

    In Areas mode, the matrix will transform the top-left point in
    the same way as in Points mode to (20/40), and double the width
    and height, so the bottom-right will become (69,99), i.e. a width
    of 60 and a height of 80.

    Because integer arithmetic is used (for speed), rounding
    differences mean that the modes will produce slightly different
    results given the same shape and the same transformation,
    especially when scaling up. This also means that some operations
    are not commutative.

    Under Points mode, \c{matrix * ( region1 | region2 )} is not equal to
    \c{matrix * region1 | matrix * region2}. Under Area mode, \c{matrix *
    (pointarray[i])} is not neccesarily equal to
    \c{(matrix * pointarry)[i]}.

    \img xform.png Comparison of Points and Areas TransformationModes
*/

/*!
    Sets the transformation mode that TQWMatrix and painter
    transformations use to \a m.

    \sa TQWMatrix::TransformationMode
*/
void TQWMatrix::setTransformationMode( TQWMatrix::TransformationMode m )
{
    if ( m == TQWMatrix::Points )
	tqt_old_transformations = true;
    else
	tqt_old_transformations = false;
}


/*!
    Returns the current transformation mode.

    \sa TQWMatrix::TransformationMode
*/
TQWMatrix::TransformationMode TQWMatrix::transformationMode()
{
    return (tqt_old_transformations ? TQWMatrix::Points : TQWMatrix::Areas );
}


// some defines to inline some code
#define MAPDOUBLE( x, y, nx, ny ) \
{ \
    double fx = x; \
    double fy = y; \
    nx = _m11*fx + _m21*fy + _dx; \
    ny = _m12*fx + _m22*fy + _dy; \
}

#define MAPINT( x, y, nx, ny ) \
{ \
    double fx = x; \
    double fy = y; \
    nx = tqRound(_m11*fx + _m21*fy + _dx); \
    ny = tqRound(_m12*fx + _m22*fy + _dy); \
}

/*****************************************************************************
  TQWMatrix member functions
 *****************************************************************************/

/*!
    Constructs an identity matrix. All elements are set to zero except
    \e m11 and \e m22 (scaling), which are set to 1.
*/

TQWMatrix::TQWMatrix()
{
    _m11 = _m22 = 1.0;
    _m12 = _m21 = _dx = _dy = 0.0;
}

/*!
    Constructs a matrix with the elements, \a m11, \a m12, \a m21, \a
    m22, \a dx and \a dy.
*/

TQWMatrix::TQWMatrix( double m11, double m12, double m21, double m22,
		    double dx, double dy )
{
    _m11 = m11;	 _m12 = m12;
    _m21 = m21;	 _m22 = m22;
    _dx	 = dx;	 _dy  = dy;
}


/*!
    Sets the matrix elements to the specified values, \a m11, \a m12,
    \a m21, \a m22, \a dx and \a dy.
*/

void TQWMatrix::setMatrix( double m11, double m12, double m21, double m22,
			  double dx, double dy )
{
    _m11 = m11;	 _m12 = m12;
    _m21 = m21;	 _m22 = m22;
    _dx	 = dx;	 _dy  = dy;
}


/*!
    \fn double TQWMatrix::m11() const

    Returns the X scaling factor.
*/

/*!
    \fn double TQWMatrix::m12() const

    Returns the vertical shearing factor.
*/

/*!
    \fn double TQWMatrix::m21() const

    Returns the horizontal shearing factor.
*/

/*!
    \fn double TQWMatrix::m22() const

    Returns the Y scaling factor.
*/

/*!
    \fn double TQWMatrix::dx() const

    Returns the horizontal translation.
*/

/*!
    \fn double TQWMatrix::dy() const

    Returns the vertical translation.
*/


/*!
    \overload

    Transforms ( \a x, \a y ) to ( \a *tx, \a *ty ) using the
    following formulae:

    \code
	*tx = m11*x + m21*y + dx
	*ty = m22*y + m12*x + dy
    \endcode
*/

void TQWMatrix::map( double x, double y, double *tx, double *ty ) const
{
    MAPDOUBLE( x, y, *tx, *ty );
}

/*!
    Transforms ( \a x, \a y ) to ( \a *tx, \a *ty ) using the formulae:

    \code
	*tx = m11*x + m21*y + dx  (rounded to the nearest integer)
	*ty = m22*y + m12*x + dy  (rounded to the nearest integer)
    \endcode
*/

void TQWMatrix::map( int x, int y, int *tx, int *ty ) const
{
    MAPINT( x, y, *tx, *ty );
}

/*!
    \fn TQPoint TQWMatrix::map( const TQPoint &p ) const

    \overload

    Transforms \a p to using the formulae:

    \code
	retx = m11*px + m21*py + dx  (rounded to the nearest integer)
	rety = m22*py + m12*px + dy  (rounded to the nearest integer)
    \endcode
*/

/*!
  \fn TQRect TQWMatrix::map( const TQRect &r ) const

  \obsolete

  Please use \l TQWMatrix::mapRect() instead.

  Note that this method does return the bounding rectangle of the \a r, when
  shearing or rotations are used.
*/

/*!
    \fn TQPointArray TQWMatrix::map( const TQPointArray &a ) const

    \overload

    Returns the point array \a a transformed by calling map for each point.
*/


/*!
    \fn TQRegion TQWMatrix::map( const TQRegion &r ) const

    \overload

    Transforms the region \a r.

    Calling this method can be rather expensive, if rotations or
    shearing are used.
*/

/*!
    \fn TQRegion TQWMatrix::mapToRegion( const TQRect &rect ) const

    Returns the transformed rectangle \a rect.

    A rectangle which has been rotated or sheared may result in a
    non-rectangular region being returned.

    Calling this method can be expensive, if rotations or shearing are
    used. If you just need to know the bounding rectangle of the
    returned region, use mapRect() which is a lot faster than this
    function.

    \sa TQWMatrix::mapRect()
*/


/*!
    Returns the transformed rectangle \a rect.

    The bounding rectangle is returned if rotation or shearing has
    been specified.

    If you need to know the exact region \a rect maps to use \l
    operator*().

    \sa operator*()
*/

TQRect TQWMatrix::mapRect( const TQRect &rect ) const
{
    TQRect result;
    if( tqt_old_transformations ) {
	if ( _m12 == 0.0F && _m21 == 0.0F ) {
	    result = TQRect( map(rect.topLeft()), map(rect.bottomRight()) ).normalize();
	} else {
	    TQPointArray a( rect );
	    a = map( a );
	    result = a.boundingRect();
	}
    } else {
	if ( _m12 == 0.0F && _m21 == 0.0F ) {
	    int x = tqRound( _m11*rect.x() + _dx );
	    int y = tqRound( _m22*rect.y() + _dy );
	    int w = tqRound( _m11*rect.width() );
	    int h = tqRound( _m22*rect.height() );
	    if ( w < 0 ) {
		w = -w;
		x -= w-1;
	    }
	    if ( h < 0 ) {
		h = -h;
		y -= h-1;
	    }
	    result = TQRect( x, y, w, h );
	} else {

	    // see mapToPolygon for explanations of the algorithm.
	    double x0, y0;
	    double x, y;
	    MAPDOUBLE( rect.left(), rect.top(), x0, y0 );
	    double xmin = x0;
	    double ymin = y0;
	    double xmax = x0;
	    double ymax = y0;
	    MAPDOUBLE( rect.right() + 1, rect.top(), x, y );
	    xmin = TQMIN( xmin, x );
	    ymin = TQMIN( ymin, y );
	    xmax = TQMAX( xmax, x );
	    ymax = TQMAX( ymax, y );
	    MAPDOUBLE( rect.right() + 1, rect.bottom() + 1, x, y );
	    xmin = TQMIN( xmin, x );
	    ymin = TQMIN( ymin, y );
	    xmax = TQMAX( xmax, x );
	    ymax = TQMAX( ymax, y );
	    MAPDOUBLE( rect.left(), rect.bottom() + 1, x, y );
	    xmin = TQMIN( xmin, x );
	    ymin = TQMIN( ymin, y );
	    xmax = TQMAX( xmax, x );
	    ymax = TQMAX( ymax, y );
	    double w = xmax - xmin;
	    double h = ymax - ymin;
	    xmin -= ( xmin - x0 ) / w;
	    ymin -= ( ymin - y0 ) / h;
	    xmax -= ( xmax - x0 ) / w;
	    ymax -= ( ymax - y0 ) / h;
	    result = TQRect( tqRound(xmin), tqRound(ymin), tqRound(xmax)-tqRound(xmin)+1, tqRound(ymax)-tqRound(ymin)+1 );
	}
    }
    return result;
}


/*!
  \internal
*/
TQPoint TQWMatrix::operator *( const TQPoint &p ) const
{
    double fx = p.x();
    double fy = p.y();
    return TQPoint( tqRound(_m11*fx + _m21*fy + _dx),
		   tqRound(_m12*fx + _m22*fy + _dy) );
}


struct TQWMDoublePoint {
    double x;
    double y;
};

/*!
  \internal
*/
TQPointArray TQWMatrix::operator *( const TQPointArray &a ) const
{
    if( tqt_old_transformations ) {
	TQPointArray result = a.copy();
	int x, y;
	for ( int i=0; i<(int)result.size(); i++ ) {
	    result.point( i, &x, &y );
	    MAPINT( x, y, x, y );
	    result.setPoint( i, x, y );
	}
	return result;
    } else {
	int size = a.size();
	int i;
	TQMemArray<TQWMDoublePoint> p( size );
	TQPoint *da = a.data();
	TQWMDoublePoint *dp = p.data();
	double xmin = INT_MAX;
	double ymin = xmin;
	double xmax = INT_MIN;
	double ymax = xmax;
	int xminp = 0;
	int yminp = 0;
	for( i = 0; i < size; i++ ) {
	    dp[i].x = da[i].x();
	    dp[i].y = da[i].y();
	    if ( dp[i].x < xmin ) {
		xmin = dp[i].x;
		xminp = i;
	    }
	    if ( dp[i].y < ymin ) {
		ymin = dp[i].y;
		yminp = i;
	    }
	    xmax = TQMAX( xmax, dp[i].x );
	    ymax = TQMAX( ymax, dp[i].y );
	}
	double w = TQMAX( xmax - xmin, 1 );
	double h = TQMAX( ymax - ymin, 1 );
	for( i = 0; i < size; i++ ) {
	    dp[i].x += (dp[i].x - xmin)/w;
	    dp[i].y += (dp[i].y - ymin)/h;
	    MAPDOUBLE( dp[i].x, dp[i].y, dp[i].x, dp[i].y );
	}

	// now apply correction back for transformed values...
	xmin = INT_MAX;
	ymin = xmin;
	xmax = INT_MIN;
	ymax = xmax;
	for( i = 0; i < size; i++ ) {
	    xmin = TQMIN( xmin, dp[i].x );
	    ymin = TQMIN( ymin, dp[i].y );
	    xmax = TQMAX( xmax, dp[i].x );
	    ymax = TQMAX( ymax, dp[i].y );
	}
	w = TQMAX( xmax - xmin, 1 );
	h = TQMAX( ymax - ymin, 1 );

	TQPointArray result( size );
	TQPoint *dr = result.data();
	for( i = 0; i < size; i++ ) {
	    dr[i].setX( tqRound( dp[i].x - (dp[i].x - dp[xminp].x)/w ) );
	    dr[i].setY( tqRound( dp[i].y - (dp[i].y - dp[yminp].y)/h ) );
	}
	return result;
    }
}

/*!
\internal
*/
TQRegion TQWMatrix::operator * (const TQRect &rect ) const
{
    TQRegion result;
    if ( isIdentity() ) {
	result = rect;
    } else if ( _m12 == 0.0F && _m21 == 0.0F ) {
	if( tqt_old_transformations ) {
	    result = TQRect( map(rect.topLeft()), map(rect.bottomRight()) ).normalize();
	} else {
	    int x = tqRound( _m11*rect.x() + _dx );
	    int y = tqRound( _m22*rect.y() + _dy );
	    int w = tqRound( _m11*rect.width() );
	    int h = tqRound( _m22*rect.height() );
	    if ( w < 0 ) {
		w = -w;
		x -= w - 1;
	    }
	    if ( h < 0 ) {
		h = -h;
		y -= h - 1;
	    }
	    result = TQRect( x, y, w, h );
	}
    } else {
	result = TQRegion( mapToPolygon( rect ) );
    }
    return result;

}

/*!
    Returns the transformed rectangle \a rect as a polygon.

    Polygons and rectangles behave slightly differently
    when transformed (due to integer rounding), so
    \c{matrix.map( TQPointArray( rect ) )} is not always the same as
    \c{matrix.mapToPolygon( rect )}.
*/
TQPointArray TQWMatrix::mapToPolygon( const TQRect &rect ) const
{
    TQPointArray a( 4 );
    if ( tqt_old_transformations ) {
	a = TQPointArray( rect );
	return operator *( a );
    }
    double x[4], y[4];
    if ( _m12 == 0.0F && _m21 == 0.0F ) {
	x[0] = tqRound( _m11*rect.x() + _dx );
        y[0] = tqRound( _m22*rect.y() + _dy );
	double w = tqRound( _m11*rect.width() );
	double h = tqRound( _m22*rect.height() );
	if ( w < 0 ) {
	    w = -w;
	    x[0] -= w - 1.;
	}
	if ( h < 0 ) {
	    h = -h;
	    y[0] -= h - 1.;
	}
	x[1] = x[0]+w-1;
	x[2] = x[1];
	x[3] = x[0];
	y[1] = y[0];
	y[2] = y[0]+h-1;
	y[3] = y[2];
    } else {
	MAPINT( rect.left(), rect.top(), x[0], y[0] );
	MAPINT( rect.right() + 1, rect.top(), x[1], y[1] );
	MAPINT( rect.right() + 1, rect.bottom() + 1, x[2], y[2] );
	MAPINT( rect.left(), rect.bottom() + 1, x[3], y[3] );

	/*
	Including rectangles as we have are evil.

        We now have a rectangle that is one pixel to wide and one to
        high. the tranformed position of the top-left corner is
        correct. All other points need some adjustments.

	Doing this mathematically exact would force us to calculate some square roots,
	something we don't want for the sake of speed.

        Instead we use an approximation, that converts to the correct
        answer when m12 -> 0 and m21 -> 0, and accept smaller
        errors in the general transformation case.

        The solution is to calculate the width and height of the
        bounding rect, and scale the points 1/2/3 by (xp-x0)/xw pixel direction
        to point 0.
        */

	double xmin = x[0];
	double ymin = y[0];
	double xmax = x[0];
	double ymax = y[0];
	int i;
	for( i = 1; i< 4; i++ ) {
	    xmin = TQMIN( xmin, x[i] );
	    ymin = TQMIN( ymin, y[i] );
	    xmax = TQMAX( xmax, x[i] );
	    ymax = TQMAX( ymax, y[i] );
	}
	double w = xmax - xmin;
	double h = ymax - ymin;

	for( i = 1; i < 4; i++ ) {
	    x[i] -= (x[i] - x[0])/w;
	    y[i] -= (y[i] - y[0])/h;
	}
    }
#if 0
    int i;
    for( i = 0; i< 4; i++ )
	tqDebug("coords(%d) = (%f/%f) (%d/%d)", i, x[i], y[i], tqRound(x[i]), tqRound(y[i]) );
    tqDebug( "width=%f, height=%f", sqrt( (x[1]-x[0])*(x[1]-x[0]) + (y[1]-y[0])*(y[1]-y[0]) ),
	    sqrt( (x[0]-x[3])*(x[0]-x[3]) + (y[0]-y[3])*(y[0]-y[3]) ) );
#endif
    // all coordinates are correctly, tranform to a pointarray
    // (rounding to the next integer)
    a.setPoints( 4, tqRound( x[0] ), tqRound( y[0] ),
		 tqRound( x[1] ), tqRound( y[1] ),
		 tqRound( x[2] ), tqRound( y[2] ),
		 tqRound( x[3] ), tqRound( y[3] ) );
    return a;
}

/*!
\internal
*/
TQRegion TQWMatrix::operator * (const TQRegion &r ) const
{
    if ( isIdentity() )
	return r;
    TQMemArray<TQRect> rects = r.rects();
    TQRegion result;
    TQRect *rect = rects.data();
    int i = rects.size();
    if ( _m12 == 0.0F && _m21 == 0.0F && _m11 > 1.0F && _m22 > 1.0F ) {
	// simple case, no rotation
	while ( i ) {
	    int x = tqRound( _m11*rect->x() + _dx );
	    int y = tqRound( _m22*rect->y() + _dy );
	    int w = tqRound( _m11*rect->width() );
	    int h = tqRound( _m22*rect->height() );
	    if ( w < 0 ) {
		w = -w;
		x -= w-1;
	    }
	    if ( h < 0 ) {
		h = -h;
		y -= h-1;
	    }
	    *rect = TQRect( x, y, w, h );
	    rect++;
	    i--;
	}
	result.setRects( rects.data(), rects.size() );
    } else {
	while ( i ) {
	    result |= operator *( *rect );
	    rect++;
	    i--;
	}
    }
    return result;
}

/*!
    Resets the matrix to an identity matrix.

    All elements are set to zero, except \e m11 and \e m22 (scaling)
    which are set to 1.

    \sa isIdentity()
*/

void TQWMatrix::reset()
{
    _m11 = _m22 = 1.0;
    _m12 = _m21 = _dx = _dy = 0.0;
}

/*!
    Returns true if the matrix is the identity matrix; otherwise returns false.

    \sa reset()
*/
bool TQWMatrix::isIdentity() const
{
    return _m11 == 1.0 && _m22 == 1.0 && _m12 == 0.0 && _m21 == 0.0
	&& _dx == 0.0 && _dy == 0.0;
}

/*!
    Moves the coordinate system \a dx along the X-axis and \a dy along
    the Y-axis.

    Returns a reference to the matrix.

    \sa scale(), shear(), rotate()
*/

TQWMatrix &TQWMatrix::translate( double dx, double dy )
{
    _dx += dx*_m11 + dy*_m21;
    _dy += dy*_m22 + dx*_m12;
    return *this;
}

/*!
    Scales the coordinate system unit by \a sx horizontally and \a sy
    vertically.

    Returns a reference to the matrix.

    \sa translate(), shear(), rotate()
*/

TQWMatrix &TQWMatrix::scale( double sx, double sy )
{
    _m11 *= sx;
    _m12 *= sx;
    _m21 *= sy;
    _m22 *= sy;
    return *this;
}

/*!
    Shears the coordinate system by \a sh horizontally and \a sv
    vertically.

    Returns a reference to the matrix.

    \sa translate(), scale(), rotate()
*/

TQWMatrix &TQWMatrix::shear( double sh, double sv )
{
    double tm11 = sv*_m21;
    double tm12 = sv*_m22;
    double tm21 = sh*_m11;
    double tm22 = sh*_m12;
    _m11 += tm11;
    _m12 += tm12;
    _m21 += tm21;
    _m22 += tm22;
    return *this;
}

const double deg2rad = 0.017453292519943295769;	// pi/180

/*!
    Rotates the coordinate system \a a degrees counterclockwise.

    Returns a reference to the matrix.

    \sa translate(), scale(), shear()
*/

TQWMatrix &TQWMatrix::rotate( double a )
{
    double b = deg2rad*a;			// convert to radians
#if defined(TQ_WS_X11)
    double sina = qsincos(b,false);		// fast and convenient
    double cosa = qsincos(b,true);
#else
    double sina = sin(b);
    double cosa = cos(b);
#endif
    double tm11 = cosa*_m11 + sina*_m21;
    double tm12 = cosa*_m12 + sina*_m22;
    double tm21 = -sina*_m11 + cosa*_m21;
    double tm22 = -sina*_m12 + cosa*_m22;
    _m11 = tm11; _m12 = tm12;
    _m21 = tm21; _m22 = tm22;
    return *this;
}

/*!
    \fn bool TQWMatrix::isInvertible() const

    Returns true if the matrix is invertible; otherwise returns false.

    \sa invert()
*/

/*!
    \fn double TQWMatrix::det() const

    Returns the matrix's determinant.
*/


/*!
    Returns the inverted matrix.

    If the matrix is singular (not invertible), the identity matrix is
    returned.

    If \a invertible is not 0: the value of \a *invertible is set
    to true if the matrix is invertible; otherwise \a *invertible is
    set to false.

    \sa isInvertible()
*/

TQWMatrix TQWMatrix::invert( bool *invertible ) const
{
    double determinant = det();
    if ( determinant == 0.0 ) {
	if ( invertible )
	    *invertible = false;		// singular matrix
	TQWMatrix defaultMatrix;
	return defaultMatrix;
    }
    else {					// invertible matrix
	if ( invertible )
	    *invertible = true;
	double dinv = 1.0/determinant;
	TQWMatrix imatrix( (_m22*dinv),	(-_m12*dinv),
			  (-_m21*dinv), ( _m11*dinv),
			  ((_m21*_dy - _m22*_dx)*dinv),
			  ((_m12*_dx - _m11*_dy)*dinv) );
	return imatrix;
    }
}


/*!
    Returns true if this matrix is equal to \a m; otherwise returns false.
*/

bool TQWMatrix::operator==( const TQWMatrix &m ) const
{
    return _m11 == m._m11 &&
	   _m12 == m._m12 &&
	   _m21 == m._m21 &&
	   _m22 == m._m22 &&
	   _dx == m._dx &&
	   _dy == m._dy;
}

/*!
    Returns true if this matrix is not equal to \a m; otherwise returns false.
*/

bool TQWMatrix::operator!=( const TQWMatrix &m ) const
{
    return _m11 != m._m11 ||
	   _m12 != m._m12 ||
	   _m21 != m._m21 ||
	   _m22 != m._m22 ||
	   _dx != m._dx ||
	   _dy != m._dy;
}

/*!
    Returns the result of multiplying this matrix by matrix \a m.
*/

TQWMatrix &TQWMatrix::operator*=( const TQWMatrix &m )
{
    double tm11 = _m11*m._m11 + _m12*m._m21;
    double tm12 = _m11*m._m12 + _m12*m._m22;
    double tm21 = _m21*m._m11 + _m22*m._m21;
    double tm22 = _m21*m._m12 + _m22*m._m22;

    double tdx  = _dx*m._m11  + _dy*m._m21 + m._dx;
    double tdy =  _dx*m._m12  + _dy*m._m22 + m._dy;

    _m11 = tm11; _m12 = tm12;
    _m21 = tm21; _m22 = tm22;
    _dx = tdx; _dy = tdy;
    return *this;
}

/*!
    \overload
    \relates TQWMatrix
    Returns the product of \a m1 * \a m2.

    Note that matrix multiplication is not commutative, i.e. a*b !=
    b*a.
*/

TQWMatrix operator*( const TQWMatrix &m1, const TQWMatrix &m2 )
{
    TQWMatrix result = m1;
    result *= m2;
    return result;
}

/*****************************************************************************
  TQWMatrix stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
/*!
    \relates TQWMatrix

    Writes the matrix \a m to the stream \a s and returns a reference
    to the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator<<( TQDataStream &s, const TQWMatrix &m )
{
    if ( s.version() == 1 )
	s << (float)m.m11() << (float)m.m12() << (float)m.m21()
	  << (float)m.m22() << (float)m.dx()  << (float)m.dy();
    else
	s << m.m11() << m.m12() << m.m21() << m.m22()
	  << m.dx() << m.dy();
    return s;
}

/*!
    \relates TQWMatrix

    Reads the matrix \a m from the stream \a s and returns a reference
    to the stream.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/

TQDataStream &operator>>( TQDataStream &s, TQWMatrix &m )
{
    if ( s.version() == 1 ) {
	float m11, m12, m21, m22, dx, dy;
	s >> m11;  s >> m12;  s >> m21;  s >> m22;
	s >> dx;   s >> dy;
	m.setMatrix( m11, m12, m21, m22, dx, dy );
    }
    else {
	double m11, m12, m21, m22, dx, dy;
	s >> m11;  s >> m12;  s >> m21;  s >> m22;
	s >> dx;   s >> dy;
	m.setMatrix( m11, m12, m21, m22, dx, dy );
    }
    return s;
}
#endif // TQT_NO_DATASTREAM

#endif // TQT_NO_WMATRIX

