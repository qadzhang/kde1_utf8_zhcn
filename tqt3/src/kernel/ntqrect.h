/****************************************************************************
**
** Definition of TQRect class
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

#ifndef TQRECT_H
#define TQRECT_H

#ifndef QT_H
#include "ntqsize.h"
#endif // QT_H

#if defined(topLeft)
#error "Macro definition of topLeft conflicts with TQRect"
// don't just silently undo people's defines: #undef topLeft
#endif

class TQ_EXPORT TQRect					// rectangle class
{
public:
    TQRect()	{ x1 = y1 = 0; x2 = y2 = -1; }
    TQRect( const TQPoint &topleft, const TQPoint &bottomright );
    TQRect( const TQPoint &topleft, const TQSize &size );
    TQRect( int left, int top, int width, int height );

    bool   isNull()	const;
    bool   isEmpty()	const;
    bool   isValid()	const;
    TQRect  normalize()	const;

    int	   left()	const;
    int	   top()	const;
    int	   right()	const;
    int	   bottom()	const;

    TQCOORD &rLeft();
    TQCOORD &rTop();
    TQCOORD &rRight();
    TQCOORD &rBottom();

    int	   x()		const;
    int	   y()		const;
    void   setLeft( int pos );
    void   setTop( int pos );
    void   setRight( int pos );
    void   setBottom( int pos );
    void   setX( int x );
    void   setY( int y );

    void   setTopLeft( const TQPoint &p );
    void   setBottomRight( const TQPoint &p );
    void   setTopRight( const TQPoint &p );
    void   setBottomLeft( const TQPoint &p );

    TQPoint topLeft()	 const;
    TQPoint bottomRight() const;
    TQPoint topRight()	 const;
    TQPoint bottomLeft()	 const;
    TQPoint center()	 const;

    void   rect( int *x, int *y, int *w, int *h ) const;
    void   coords( int *x1, int *y1, int *x2, int *y2 ) const;

    void   moveLeft( int pos );
    void   moveTop( int pos );
    void   moveRight( int pos );
    void   moveBottom( int pos );
    void   moveTopLeft( const TQPoint &p );
    void   moveBottomRight( const TQPoint &p );
    void   moveTopRight( const TQPoint &p );
    void   moveBottomLeft( const TQPoint &p );
    void   moveCenter( const TQPoint &p );
    void   moveBy( int dx, int dy );

    void   setRect( int x, int y, int w, int h );
    void   setCoords( int x1, int y1, int x2, int y2 );
    void   addCoords( int x1, int y1, int x2, int y2 );

    TQSize  size()	const;
    int	   width()	const;
    int	   height()	const;
    void   setWidth( int w );
    void   setHeight( int h );
    void   setSize( const TQSize &s );

    TQRect  operator|(const TQRect &r) const;
    TQRect  operator&(const TQRect &r) const;
    TQRect&  operator|=(const TQRect &r);
    TQRect&  operator&=(const TQRect &r);

    bool   contains( const TQPoint &p, bool proper=false ) const;
    bool   contains( int x, int y ) const; // inline methods, _don't_ merge these
    bool   contains( int x, int y, bool proper ) const;
    bool   contains( const TQRect &r, bool proper=false ) const;
    TQRect  unite( const TQRect &r ) const;
    TQRect  intersect( const TQRect &r ) const;
    bool   intersects( const TQRect &r ) const;

    friend TQ_EXPORT bool operator==( const TQRect &, const TQRect & );
    friend TQ_EXPORT bool operator!=( const TQRect &, const TQRect & );

private:
#if defined(TQ_WS_X11) || defined(Q_OS_TEMP)
    friend void qt_setCoords( TQRect *r, int xp1, int yp1, int xp2, int yp2 );
#endif
#if defined(Q_OS_MAC)
    TQCOORD y1;
    TQCOORD x1;
    TQCOORD y2;
    TQCOORD x2;
#else
    TQCOORD x1;
    TQCOORD y1;
    TQCOORD x2;
    TQCOORD y2;
#endif
};

TQ_EXPORT bool operator==( const TQRect &, const TQRect & );
TQ_EXPORT bool operator!=( const TQRect &, const TQRect & );


/*****************************************************************************
  TQRect stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQRect & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQRect & );
#endif

/*****************************************************************************
  TQRect inline member functions
 *****************************************************************************/

inline TQRect::TQRect( int left, int top, int width, int height )
{
    x1 = (TQCOORD)left;
    y1 = (TQCOORD)top;
    x2 = (TQCOORD)(left+width-1);
    y2 = (TQCOORD)(top+height-1);
}

inline bool TQRect::isNull() const
{ return x2 == x1-1 && y2 == y1-1; }

inline bool TQRect::isEmpty() const
{ return x1 > x2 || y1 > y2; }

inline bool TQRect::isValid() const
{ return x1 <= x2 && y1 <= y2; }

inline int TQRect::left() const
{ return x1; }

inline int TQRect::top() const
{ return y1; }

inline int TQRect::right() const
{ return x2; }

inline int TQRect::bottom() const
{ return y2; }

inline TQCOORD &TQRect::rLeft()
{ return x1; }

inline TQCOORD & TQRect::rTop()
{ return y1; }

inline TQCOORD & TQRect::rRight()
{ return x2; }

inline TQCOORD & TQRect::rBottom()
{ return y2; }

inline int TQRect::x() const
{ return x1; }

inline int TQRect::y() const
{ return y1; }

inline void TQRect::setLeft( int pos )
{ x1 = (TQCOORD)pos; }

inline void TQRect::setTop( int pos )
{ y1 = (TQCOORD)pos; }

inline void TQRect::setRight( int pos )
{ x2 = (TQCOORD)pos; }

inline void TQRect::setBottom( int pos )
{ y2 = (TQCOORD)pos; }

inline void TQRect::setX( int x )
{ x1 = (TQCOORD)x; }

inline void TQRect::setY( int y )
{ y1 = (TQCOORD)y; }

inline TQPoint TQRect::topLeft() const
{ return TQPoint(x1, y1); }

inline TQPoint TQRect::bottomRight() const
{ return TQPoint(x2, y2); }

inline TQPoint TQRect::topRight() const
{ return TQPoint(x2, y1); }

inline TQPoint TQRect::bottomLeft() const
{ return TQPoint(x1, y2); }

inline TQPoint TQRect::center() const
{ return TQPoint((x1+x2)/2, (y1+y2)/2); }

inline int TQRect::width() const
{ return  x2 - x1 + 1; }

inline int TQRect::height() const
{ return  y2 - y1 + 1; }

inline TQSize TQRect::size() const
{ return TQSize(x2-x1+1, y2-y1+1); }

inline bool TQRect::contains( int x, int y, bool proper ) const
{
    if ( proper )
        return x > x1 && x < x2 &&
               y > y1 && y < y2;
    else
        return x >= x1 && x <= x2 &&
               y >= y1 && y <= y2;
}

inline bool TQRect::contains( int x, int y ) const
{
    return x >= x1 && x <= x2 &&
	   y >= y1 && y <= y2;
}
#define Q_DEFINED_QRECT
#include "ntqwinexport.h"
#endif // TQRECT_H
