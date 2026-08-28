/****************************************************************************
**
** Implementation of draw utilities
**
** Created : 950920
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

#include "ntqdrawutil.h"
#ifndef TQT_NO_DRAWUTIL
#include "ntqbitmap.h"
#include "ntqpixmapcache.h"
#include "ntqapplication.h"
#include "ntqpainter.h"

/*!
    \relates TQPainter

    \c{#include <ntqdrawutil.h>}

    Draws a horizontal (\a y1 == \a y2) or vertical (\a x1 == \a x2)
    shaded line using the painter \a p.

    Nothing is drawn if \a y1 != \a y2 and \a x1 != \a x2 (i.e. the
    line is neither horizontal nor vertical).

    The color group argument \a g specifies the shading colors (\link
    TQColorGroup::light() light\endlink, \link TQColorGroup::dark()
    dark\endlink and \link TQColorGroup::mid() middle\endlink colors).

    The line appears sunken if \a sunken is true, or raised if \a
    sunken is false.

    The \a lineWidth argument specifies the line width for each of the
    lines. It is not the total line width.

    The \a midLineWidth argument specifies the width of a middle line
    drawn in the TQColorGroup::mid() color.

    If you want to use a TQFrame widget instead, you can make it
    display a shaded line, for example \c{TQFrame::setFrameStyle(
    TQFrame::HLine | TQFrame::Sunken )}.

    \warning This function does not look at TQWidget::style() or
    TQApplication::style(). Use the drawing functions in TQStyle to make
    widgets that follow the current GUI style.

    \sa qDrawShadeRect(), qDrawShadePanel(), TQStyle::drawPrimitive()
*/

void qDrawShadeLine( TQPainter *p, int x1, int y1, int x2, int y2,
		     const TQColorGroup &g, bool sunken,
		     int lineWidth, int midLineWidth )
{
    if (!( p && lineWidth >= 0 && midLineWidth >= 0 ) )	{
#if defined(QT_CHECK_RANGE)
	tqWarning( "qDrawShadeLine invalid parameters." );
#endif
	return;
    }
    int tlw = lineWidth*2 + midLineWidth;	// total line width
    TQPen oldPen = p->pen();			// save pen
    if ( sunken )
	p->setPen( g.dark() );
    else
	p->setPen( g.light() );
    TQPointArray a;
    int i;
    if ( y1 == y2 ) {				// horizontal line
	int y = y1 - tlw/2;
	if ( x1 > x2 ) {			// swap x1 and x2
	    int t = x1;
	    x1 = x2;
	    x2 = t;
	}
	x2--;
	for ( i=0; i<lineWidth; i++ ) {		// draw top shadow
	    a.setPoints( 3, x1+i, y+tlw-1-i,
			 x1+i, y+i,
			 x2-i, y+i );
	    p->drawPolyline( a );
	}
	if ( midLineWidth > 0 ) {
	    p->setPen( g.mid() );
	    for ( i=0; i<midLineWidth; i++ )	// draw lines in the middle
		p->drawLine( x1+lineWidth, y+lineWidth+i,
			     x2-lineWidth, y+lineWidth+i );
	}
	if ( sunken )
	    p->setPen( g.light() );
	else
	    p->setPen( g.dark() );
	for ( i=0; i<lineWidth; i++ ) {		// draw bottom shadow
	    a.setPoints( 3, x1+i, y+tlw-i-1,
			 x2-i, y+tlw-i-1,
			 x2-i, y+i+1 );
	    p->drawPolyline( a );
	}
    }
    else if ( x1 == x2 ) {			// vertical line
	int x = x1 - tlw/2;
	if ( y1 > y2 ) {			// swap y1 and y2
	    int t = y1;
	    y1 = y2;
	    y2 = t;
	}
	y2--;
	for ( i=0; i<lineWidth; i++ ) {		// draw left shadow
	    a.setPoints( 3, x+i, y2,
			 x+i, y1+i,
			 x+tlw-1, y1+i );
	    p->drawPolyline( a );
	}
	if ( midLineWidth > 0 ) {
	    p->setPen( g.mid() );
	    for ( i=0; i<midLineWidth; i++ )	// draw lines in the middle
		p->drawLine( x+lineWidth+i, y1+lineWidth, x+lineWidth+i, y2 );
	}
	if ( sunken )
	    p->setPen( g.light() );
	else
	    p->setPen( g.dark() );
	for ( i=0; i<lineWidth; i++ ) {		// draw right shadow
	    a.setPoints( 3, x+lineWidth, y2-i,
			 x+tlw-i-1, y2-i,
			 x+tlw-i-1, y1+lineWidth );
	    p->drawPolyline( a );
	}
    }
    p->setPen( oldPen );
}


/*!
    \relates TQPainter

    \c{#include <ntqdrawutil.h>}

    Draws the shaded rectangle specified by (\a x, \a y, \a w, \a h)
    using the painter \a p.

    The color group argument \a g specifies the shading colors (\link
    TQColorGroup::light() light\endlink, \link TQColorGroup::dark()
    dark\endlink and \link TQColorGroup::mid() middle\endlink colors).

    The rectangle appears sunken if \a sunken is true, or raised if \a
    sunken is false.

    The \a lineWidth argument specifies the line width for each of the
    lines. It is not the total line width.

    The \a midLineWidth argument specifies the width of a middle line
    drawn in the TQColorGroup::mid() color.

    The rectangle's interior is filled with the \a fill brush unless
    \a fill is 0.

    If you want to use a TQFrame widget instead, you can make it
    display a shaded rectangle, for example \c{TQFrame::setFrameStyle(
    TQFrame::Box | TQFrame::Raised )}.

    \warning This function does not look at TQWidget::style() or
    TQApplication::style(). Use the drawing functions in TQStyle to make
    widgets that follow the current GUI style.

    \sa qDrawShadeLine(), qDrawShadePanel(), qDrawPlainRect(),
    TQStyle::drawItem(), TQStyle::drawControl()
    TQStyle::drawComplexControl()
*/

void qDrawShadeRect( TQPainter *p, int x, int y, int w, int h,
		     const TQColorGroup &g, bool sunken,
		     int lineWidth, int midLineWidth,
		     const TQBrush *fill )
{
    if ( w == 0 || h == 0 )
	return;
    if ( ! ( w > 0 && h > 0 && lineWidth >= 0 && midLineWidth >= 0 ) ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "qDrawShadeRect(): Invalid parameters" );
#endif
	return;
    }
    TQPen oldPen = p->pen();
    if ( sunken )
	p->setPen( g.dark() );
    else
	p->setPen( g.light() );
    int x1=x, y1=y, x2=x+w-1, y2=y+h-1;
    TQPointArray a;

    if ( lineWidth == 1 && midLineWidth == 0 ) {// standard shade rectangle
	p->drawRect( x1, y1, w-1, h-1 );
	if ( sunken )
	    p->setPen( g.light() );
	else
	    p->setPen( g.dark() );
	a.setPoints( 8, x1+1,y1+1, x2-2,y1+1, x1+1,y1+2, x1+1,y2-2,
		     x1,y2, x2,y2,  x2,y1, x2,y2-1 );
	p->drawLineSegments( a );		// draw bottom/right lines
    } else {					// more complicated
	int m = lineWidth+midLineWidth;
	int i, j=0, k=m;
	for ( i=0; i<lineWidth; i++ ) {		// draw top shadow
	    a.setPoints( 8, x1+i, y2-i, x1+i, y1+i, x1+i, y1+i, x2-i, y1+i,
			 x1+k, y2-k, x2-k, y2-k, x2-k, y2-k, x2-k, y1+k );
	    p->drawLineSegments( a );
	    k++;
	}
	p->setPen( g.mid() );
	j = lineWidth*2;
	for ( i=0; i<midLineWidth; i++ ) {	// draw lines in the middle
	    p->drawRect( x1+lineWidth+i, y1+lineWidth+i, w-j, h-j );
	    j += 2;
	}
	if ( sunken )
	    p->setPen( g.light() );
	else
	    p->setPen( g.dark() );
	k = m;
	for ( i=0; i<lineWidth; i++ ) {		// draw bottom shadow
	    a.setPoints( 8, x1+1+i,y2-i, x2-i, y2-i, x2-i, y2-i, x2-i, y1+i+1,
			 x1+k, y2-k, x1+k, y1+k, x1+k, y1+k, x2-k, y1+k );
	    p->drawLineSegments( a );
	    k++;
	}
    }
    if ( fill ) {
	TQBrush oldBrush = p->brush();
	int tlw = lineWidth + midLineWidth;
	p->setPen( TQt::NoPen );
	p->setBrush( *fill );
	p->drawRect( x+tlw, y+tlw, w-2*tlw, h-2*tlw );
	p->setBrush( oldBrush );
    }
    p->setPen( oldPen );			// restore pen
}


/*!
    \relates TQPainter

    \c{#include <ntqdrawutil.h>}

    Draws the shaded panel specified by (\a x, \a y, \a w, \a h) using
    the painter \a p.

    The color group argument \a g specifies the shading colors (\link
    TQColorGroup::light() light\endlink, \link TQColorGroup::dark()
    dark\endlink and \link TQColorGroup::mid() middle\endlink colors).

    The panel appears sunken if \a sunken is true, or raised if \a
    sunken is false.

    The \a lineWidth argument specifies the line width.

    The panel's interior is filled with the \a fill brush unless \a
    fill is 0.

    If you want to use a TQFrame widget instead, you can make it
    display a shaded panel, for example \c{TQFrame::setFrameStyle(
    TQFrame::Panel | TQFrame::Sunken )}.

    \warning This function does not look at TQWidget::style() or
    TQApplication::style(). Use the drawing functions in TQStyle to make
    widgets that follow the current GUI style.

    \sa qDrawWinPanel(), qDrawShadeLine(), qDrawShadeRect(),
    TQStyle::drawPrimitive()
*/

void qDrawShadePanel( TQPainter *p, int x, int y, int w, int h,
		      const TQColorGroup &g, bool sunken,
		      int lineWidth, const TQBrush *fill )
{
    if ( w == 0 || h == 0 )
	return;
    if ( !( w > 0 && h > 0 && lineWidth >= 0 ) ) {
#if defined(QT_CHECK_RANGE)
    	tqWarning( "qDrawShadePanel() Invalid parameters." );
#endif
    }
    TQColor shade = g.dark();
    TQColor light = g.light();
    if ( fill ) {
	if ( fill->color() == shade )
	    shade = g.shadow();
	if ( fill->color() == light )
	    light = g.midlight();
    }
    TQPen oldPen = p->pen();			// save pen
    TQPointArray a( 4*lineWidth );
    if ( sunken )
	p->setPen( shade );
    else
	p->setPen( light );
    int x1, y1, x2, y2;
    int i;
    int n = 0;
    x1 = x;
    y1 = y2 = y;
    x2 = x+w-2;
    for ( i=0; i<lineWidth; i++ ) {		// top shadow
	a.setPoint( n++, x1, y1++ );
	a.setPoint( n++, x2--, y2++ );
    }
    x2 = x1;
    y1 = y+h-2;
    for ( i=0; i<lineWidth; i++ ) {		// left shadow
	a.setPoint( n++, x1++, y1 );
	a.setPoint( n++, x2++, y2-- );
    }
    p->drawLineSegments( a );
    n = 0;
    if ( sunken )
	p->setPen( light );
    else
	p->setPen( shade );
    x1 = x;
    y1 = y2 = y+h-1;
    x2 = x+w-1;
    for ( i=0; i<lineWidth; i++ ) {		// bottom shadow
	a.setPoint( n++, x1++, y1-- );
	a.setPoint( n++, x2, y2-- );
    }
    x1 = x2;
    y1 = y;
    y2 = y+h-lineWidth-1;
    for ( i=0; i<lineWidth; i++ ) {		// right shadow
	a.setPoint( n++, x1--, y1++ );
	a.setPoint( n++, x2--, y2 );
    }
    p->drawLineSegments( a );
    if ( fill ) {				// fill with fill color
	TQBrush oldBrush = p->brush();
	p->setPen( TQt::NoPen );
	p->setBrush( *fill );
	p->drawRect( x+lineWidth, y+lineWidth, w-lineWidth*2, h-lineWidth*2 );
	p->setBrush( oldBrush );
    }
    p->setPen( oldPen );			// restore pen
}


/*!
  \internal
  This function draws a rectangle with two pixel line width.
  It is called from qDrawWinButton() and qDrawWinPanel().

  c1..c4 and fill are used:

    1 1 1 1 1 2
    1 3 3 3 4 2
    1 3 F F 4 2
    1 3 F F 4 2
    1 4 4 4 4 2
    2 2 2 2 2 2
*/

static void qDrawWinShades( TQPainter *p,
			   int x, int y, int w, int h,
			   const TQColor &c1, const TQColor &c2,
			   const TQColor &c3, const TQColor &c4,
			   const TQBrush *fill )
{
    if ( w < 2 || h < 2 )			// can't do anything with that
	return;
    TQPen oldPen = p->pen();
    TQPointArray a( 3 );
    a.setPoints( 3, x, y+h-2, x, y, x+w-2, y );
    p->setPen( c1 );
    p->drawPolyline( a );
    a.setPoints( 3, x, y+h-1, x+w-1, y+h-1, x+w-1, y );
    p->setPen( c2 );
    p->drawPolyline( a );
    if ( w > 4 && h > 4 ) {
	a.setPoints( 3, x+1, y+h-3, x+1, y+1, x+w-3, y+1 );
	p->setPen( c3 );
	p->drawPolyline( a );
	a.setPoints( 3, x+1, y+h-2, x+w-2, y+h-2, x+w-2, y+1 );
	p->setPen( c4 );
	p->drawPolyline( a );
	if ( fill ) {
	    TQBrush oldBrush = p->brush();
	    p->setBrush( *fill );
	    p->setPen( TQt::NoPen );
	    p->drawRect( x+2, y+2, w-4, h-4 );
	    p->setBrush( oldBrush );
	}
    }
    p->setPen( oldPen );
}


/*!
    \relates TQPainter

    \c{#include <ntqdrawutil.h>}

    Draws the Windows-style button specified by (\a x, \a y, \a w, \a
    h) using the painter \a p.

    The color group argument \a g specifies the shading colors (\link
    TQColorGroup::light() light\endlink, \link TQColorGroup::dark()
    dark\endlink and \link TQColorGroup::mid() middle\endlink colors).

    The button appears sunken if \a sunken is true, or raised if \a
    sunken is false.

    The line width is 2 pixels.

    The button's interior is filled with the \a *fill brush unless \a
    fill is 0.

    \warning This function does not look at TQWidget::style() or
    TQApplication::style(). Use the drawing functions in TQStyle to make
    widgets that follow the current GUI style.

    \sa qDrawWinPanel(), TQStyle::drawControl()
*/

void qDrawWinButton( TQPainter *p, int x, int y, int w, int h,
		     const TQColorGroup &g, bool sunken,
		     const TQBrush *fill )
{
    if ( sunken )
	qDrawWinShades( p, x, y, w, h,
		       g.shadow(), g.light(), g.dark(), g.button(), fill );
    else
	qDrawWinShades( p, x, y, w, h,
		       g.light(), g.shadow(), g.button(), g.dark(), fill );
}

/*!
    \relates TQPainter

    \c{#include <ntqdrawutil.h>}

    Draws the Windows-style panel specified by (\a x, \a y, \a w, \a
    h) using the painter \a p.

    The color group argument \a g specifies the shading colors.

    The panel appears sunken if \a sunken is true, or raised if \a
    sunken is false.

    The line width is 2 pixels.

    The button's interior is filled with the \a fill brush unless \a
    fill is 0.

    If you want to use a TQFrame widget instead, you can make it
    display a shaded panel, for example \c{TQFrame::setFrameStyle(
    TQFrame::WinPanel | TQFrame::Raised )}.

    \warning This function does not look at TQWidget::style() or
    TQApplication::style(). Use the drawing functions in TQStyle to make
    widgets that follow the current GUI style.

    \sa qDrawShadePanel(), qDrawWinButton(), TQStyle::drawPrimitive()
*/

void qDrawWinPanel( TQPainter *p, int x, int y, int w, int h,
		    const TQColorGroup &g, bool	sunken,
		    const TQBrush *fill )
{
    if ( sunken )
	qDrawWinShades( p, x, y, w, h,
			g.dark(), g.light(), g.shadow(), g.midlight(), fill );
    else
	qDrawWinShades( p, x, y, w, h,
			g.light(), g.shadow(), g.midlight(), g.dark(), fill );
}


/*!
    \relates TQPainter

    \c{#include <ntqdrawutil.h>}

    Draws the plain rectangle specified by (\a x, \a y, \a w, \a h)
    using the painter \a p.

    The color argument \a c specifies the line color.

    The \a lineWidth argument specifies the line width.

    The rectangle's interior is filled with the \a fill brush unless
    \a fill is 0.

    If you want to use a TQFrame widget instead, you can make it
    display a plain rectangle, for example \c{TQFrame::setFrameStyle(
    TQFrame::Box | TQFrame::Plain )}.

    \warning This function does not look at TQWidget::style() or
    TQApplication::style(). Use the drawing functions in TQStyle to make
    widgets that follow the current GUI style.

    \sa qDrawShadeRect(), TQStyle::drawPrimitive()
*/

void qDrawPlainRect( TQPainter *p, int x, int y, int w, int h, const TQColor &c,
		     int lineWidth, const TQBrush *fill )
{
    if ( w == 0 || h == 0 )
	return;
    if ( !( w > 0 && h > 0 && lineWidth >= 0 ) ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "qDrawPlainRect() Invalid parameters." );
#endif
    }
    TQPen   oldPen   = p->pen();
    TQBrush oldBrush = p->brush();
    p->setPen( c );
    p->setBrush( TQt::NoBrush );
    for ( int i=0; i<lineWidth; i++ )
	p->drawRect( x+i, y+i, w-i*2, h-i*2 );
    if ( fill ) {				// fill with fill color
	p->setPen( TQt::NoPen );
	p->setBrush( *fill );
	p->drawRect( x+lineWidth, y+lineWidth, w-lineWidth*2, h-lineWidth*2 );
    }
    p->setPen( oldPen );
    p->setBrush( oldBrush );
}


TQRect qItemRect( TQPainter *p, TQt::GUIStyle gs,
		int x, int y, int w, int h,
		int flags,
		bool enabled,
		const TQPixmap *pixmap,
		const TQString& text, int len )
{
    TQRect result;

    if ( pixmap ) {
	if ( (flags & TQt::AlignVCenter) == TQt::AlignVCenter )
	    y += h/2 - pixmap->height()/2;
	else if ( (flags & TQt::AlignBottom) == TQt::AlignBottom)
	    y += h - pixmap->height();
	if ( (flags & TQt::AlignRight) == TQt::AlignRight )
	    x += w - pixmap->width();
	else if ( (flags & TQt::AlignHCenter) == TQt::AlignHCenter )
	    x += w/2 - pixmap->width()/2;
	else if ( (flags & TQt::AlignLeft) != TQt::AlignLeft && TQApplication::reverseLayout() )
	    x += w - pixmap->width();
	result = TQRect(x, y, pixmap->width(), pixmap->height());
    } else if ( !text.isNull() && p ) {
	result = p->boundingRect( x, y, w, h, flags, text, len );
	if ( gs == TQt::WindowsStyle && !enabled ) {
	    result.setWidth(result.width()+1);
	    result.setHeight(result.height()+1);
	}
    } else {
	result = TQRect(x, y, w, h);
    }

    return result;
}


void qDrawItem( TQPainter *p, TQt::GUIStyle gs,
		int x, int y, int w, int h,
		int flags,
		const TQColorGroup &g, bool enabled,
		const TQPixmap *pixmap,
		const TQString& text, int len , const TQColor* penColor )
{
    p->setPen( penColor?*penColor:g.foreground() );
    if ( pixmap ) {
	TQPixmap  pm( *pixmap );
	bool clip = (flags & TQt::DontClip) == 0;
	if ( clip ) {
	    if ( pm.width() < w && pm.height() < h )
		clip = false;
	    else
		p->setClipRect( x, y, w, h );
	}
	if ( (flags & TQt::AlignVCenter) == TQt::AlignVCenter )
	    y += h/2 - pm.height()/2;
	else if ( (flags & TQt::AlignBottom) == TQt::AlignBottom)
	    y += h - pm.height();
	if ( (flags & TQt::AlignRight) == TQt::AlignRight )
	    x += w - pm.width();
	else if ( (flags & TQt::AlignHCenter) == TQt::AlignHCenter )
	    x += w/2 - pm.width()/2;
	else if ( ((flags & TQt::AlignLeft) != TQt::AlignLeft) && TQApplication::reverseLayout() ) // AlignAuto && rightToLeft
	    x += w - pm.width();

	if ( !enabled ) {
	    if ( pm.mask() ) {			// pixmap with a mask
		if ( !pm.selfMask() ) {		// mask is not pixmap itself
		    TQPixmap pmm( *pm.mask() );
		    pmm.setMask( *((TQBitmap *)&pmm) );
		    pm = pmm;
		}
	    } else if ( pm.depth() == 1 ) {	// monochrome pixmap, no mask
		pm.setMask( *((TQBitmap *)&pm) );
#ifndef TQT_NO_IMAGE_HEURISTIC_MASK
	    } else {				// color pixmap, no mask
		TQString k;
		k.sprintf( "$qt-drawitem-%x", pm.serialNumber() );
		TQPixmap *mask = TQPixmapCache::find(k);
		bool del=false;
		if ( !mask ) {
		    mask = new TQPixmap( pm.createHeuristicMask() );
		    mask->setMask( *((TQBitmap*)mask) );
		    del = !TQPixmapCache::insert( k, mask );
		}
		pm = *mask;
		if (del) delete mask;
#endif
	    }
	    if ( gs == TQt::WindowsStyle ) {
		p->setPen( g.light() );
		p->drawPixmap( x+1, y+1, pm );
		p->setPen( g.text() );
	    }
	}
	p->drawPixmap( x, y, pm );
	if ( clip )
	    p->setClipping( false );
    } else if ( !text.isNull() ) {
	if ( gs == TQt::WindowsStyle && !enabled ) {
	    p->setPen( g.light() );
	    p->drawText( x+1, y+1, w, h, flags, text, len );
	    p->setPen( g.text() );
	}
	p->drawText( x, y, w, h, flags, text, len );
    }
}


/*****************************************************************************
  Overloaded functions.
 *****************************************************************************/

/*!
    \overload void qDrawShadeLine( TQPainter *p, const TQPoint &p1, const TQPoint &p2, const TQColorGroup &g, bool sunken, int lineWidth, int midLineWidth )
*/

void qDrawShadeLine( TQPainter *p, const TQPoint &p1, const TQPoint &p2,
		     const TQColorGroup &g, bool sunken,
		     int lineWidth, int midLineWidth )
{
    qDrawShadeLine( p, p1.x(), p1.y(), p2.x(), p2.y(), g, sunken,
		    lineWidth, midLineWidth );
}

/*!
    \overload void qDrawShadeRect( TQPainter *p, const TQRect &r, const TQColorGroup &g, bool sunken, int lineWidth, int midLineWidth, const TQBrush *fill )
*/

void qDrawShadeRect( TQPainter *p, const TQRect &r,
		     const TQColorGroup &g, bool sunken,
		     int lineWidth, int midLineWidth,
		     const TQBrush *fill )
{
    qDrawShadeRect( p, r.x(), r.y(), r.width(), r.height(), g, sunken,
		    lineWidth, midLineWidth, fill );
}

/*!
    \overload void qDrawShadePanel( TQPainter *p, const TQRect &r, const TQColorGroup &g, bool sunken, int lineWidth, const TQBrush *fill )
*/

void qDrawShadePanel( TQPainter *p, const TQRect &r,
		      const TQColorGroup &g, bool sunken,
		      int lineWidth, const TQBrush *fill )
{
    qDrawShadePanel( p, r.x(), r.y(), r.width(), r.height(), g, sunken,
		     lineWidth, fill );
}

/*!
    \overload void qDrawWinButton( TQPainter *p, const TQRect &r, const TQColorGroup &g, bool sunken, const TQBrush *fill )
*/

void qDrawWinButton( TQPainter *p, const TQRect &r,
		     const TQColorGroup &g, bool sunken,
		     const TQBrush *fill )
{
    qDrawWinButton( p, r.x(), r.y(), r.width(), r.height(), g, sunken, fill );
}

/*!
    \overload void qDrawWinPanel( TQPainter *p, const TQRect &r, const TQColorGroup &g, bool sunken, const TQBrush *fill )
*/

void qDrawWinPanel( TQPainter *p, const TQRect &r,
		    const TQColorGroup &g, bool sunken,
		    const TQBrush *fill )
{
    qDrawWinPanel( p, r.x(), r.y(), r.width(), r.height(), g, sunken, fill );
}

/*!
    \overload void qDrawPlainRect( TQPainter *p, const TQRect &r, const TQColor &c, int lineWidth, const TQBrush *fill )
*/

void qDrawPlainRect( TQPainter *p, const TQRect &r, const TQColor &c,
		     int lineWidth, const TQBrush *fill )
{
    qDrawPlainRect( p, r.x(), r.y(), r.width(), r.height(), c,
		    lineWidth, fill );
}


static void qDrawWinArrow( TQPainter *p, TQt::ArrowType type, bool down,
			   int x, int y, int w, int h,
			   const TQColorGroup &g, bool enabled )
{
    TQPointArray a;				// arrow polygon
    switch ( type ) {
    case TQt::UpArrow:
	a.setPoints( 7, -3,1, 3,1, -2,0, 2,0, -1,-1, 1,-1, 0,-2 );
	break;
    case TQt::DownArrow:
	a.setPoints( 7, -3,-1, 3,-1, -2,0, 2,0, -1,1, 1,1, 0,2 );
	break;
    case TQt::LeftArrow:
	a.setPoints( 7, 1,-3, 1,3, 0,-2, 0,2, -1,-1, -1,1, -2,0 );
	break;
    case TQt::RightArrow:
	a.setPoints( 7, -1,-3, -1,3, 0,-2, 0,2, 1,-1, 1,1, 2,0 );
	break;
    }
    if ( a.isNull() )
	return;

    if ( down ) {
	x++;
	y++;
    }

    TQPen savePen = p->pen();			// save current pen
    if (down)
	p->setBrushOrigin(p->brushOrigin() + TQPoint(1,1));
    p->fillRect( x, y, w, h, g.brush( TQColorGroup::Button ) );
    if (down)
	p->setBrushOrigin(p->brushOrigin() - TQPoint(1,1));
    if ( enabled ) {
	a.translate( x+w/2, y+h/2 );
	p->setPen( g.foreground() );
	p->drawLineSegments( a, 0, 3 );		// draw arrow
	p->drawPoint( a[6] );
    } else {
	a.translate( x+w/2+1, y+h/2+1 );
	p->setPen( g.light() );
	p->drawLineSegments( a, 0, 3 );		// draw arrow
	p->drawPoint( a[6] );
	a.translate( -1, -1 );
	p->setPen( g.mid() );
	p->drawLineSegments( a, 0, 3 );		// draw arrow
	p->drawPoint( a[6] );
    }
    p->setPen( savePen );			// restore pen
}


#if defined(Q_CC_MSVC)
#pragma warning(disable: 4244)
#endif

#ifndef TQT_NO_STYLE_MOTIF
// motif arrows look the same whether they are used or not
// is this correct?
static void qDrawMotifArrow( TQPainter *p, TQt::ArrowType type, bool down,
			     int x, int y, int w, int h,
			     const TQColorGroup &g, bool )
{
    TQPointArray bFill;				// fill polygon
    TQPointArray bTop;				// top shadow.
    TQPointArray bBot;				// bottom shadow.
    TQPointArray bLeft;				// left shadow.
#ifndef TQT_NO_TRANSFORMATIONS
    TQWMatrix	matrix;				// xform matrix
#endif
    bool vertical = type == TQt::UpArrow || type == TQt::DownArrow;
    bool horizontal = !vertical;
    int	 dim = w < h ? w : h;
    int	 colspec = 0x0000;			// color specification array

    if ( dim < 2 )				// too small arrow
	return;

    if ( dim > 3 ) {
	if ( dim > 6 )
	    bFill.resize( dim & 1 ? 3 : 4 );
	bTop.resize( (dim/2)*2 );
	bBot.resize( dim & 1 ? dim + 1 : dim );
	bLeft.resize( dim > 4 ? 4 : 2 );
	bLeft.putPoints( 0, 2, 0,0, 0,dim-1 );
	if ( dim > 4 )
	    bLeft.putPoints( 2, 2, 1,2, 1,dim-3 );
	bTop.putPoints( 0, 4, 1,0, 1,1, 2,1, 3,1 );
	bBot.putPoints( 0, 4, 1,dim-1, 1,dim-2, 2,dim-2, 3,dim-2 );

	for( int i=0; i<dim/2-2 ; i++ ) {
	    bTop.putPoints( i*2+4, 2, 2+i*2,2+i, 5+i*2, 2+i );
	    bBot.putPoints( i*2+4, 2, 2+i*2,dim-3-i, 5+i*2,dim-3-i );
	}
	if ( dim & 1 )				// odd number size: extra line
	    bBot.putPoints( dim-1, 2, dim-3,dim/2, dim-1,dim/2 );
	if ( dim > 6 ) {			// dim>6: must fill interior
	    bFill.putPoints( 0, 2, 1,dim-3, 1,2 );
	    if ( dim & 1 )			// if size is an odd number
		bFill.setPoint( 2, dim - 3, dim / 2 );
	    else
		bFill.putPoints( 2, 2, dim-4,dim/2-1, dim-4,dim/2 );
	}
    }
    else {
	if ( dim == 3 ) {			// 3x3 arrow pattern
	    bLeft.setPoints( 4, 0,0, 0,2, 1,1, 1,1 );
	    bTop .setPoints( 2, 1,0, 1,0 );
	    bBot .setPoints( 2, 1,2, 2,1 );
	}
	else {					// 2x2 arrow pattern
	    bLeft.setPoints( 2, 0,0, 0,1 );
	    bTop .setPoints( 2, 1,0, 1,0 );
	    bBot .setPoints( 2, 1,1, 1,1 );
	}
    }

    if ( type == TQt::UpArrow || type == TQt::LeftArrow ) {
#ifndef TQT_NO_TRANSFORMATIONS	// #### fix me!
	matrix.translate( x, y );
	if ( vertical ) {
	    matrix.translate( 0, h - 1 );
	    matrix.rotate( -90 );
	} else {
	    matrix.translate( w - 1, h - 1 );
	    matrix.rotate( 180 );
	}
#endif
	if ( down )
	    colspec = horizontal ? 0x2334 : 0x2343;
	else
	    colspec = horizontal ? 0x1443 : 0x1434;
    }
    else if ( type == TQt::DownArrow || type == TQt::RightArrow ) {
#ifndef TQT_NO_TRANSFORMATIONS	// #### fix me!
	matrix.translate( x, y );
	if ( vertical ) {
	    matrix.translate( w-1, 0 );
	    matrix.rotate( 90 );
	}
#endif
	if ( down )
	    colspec = horizontal ? 0x2443 : 0x2434;
	else
	    colspec = horizontal ? 0x1334 : 0x1343;
    }

    TQColor *cols[5];
    cols[0] = 0;
    cols[1] = (TQColor *)&g.button();
    cols[2] = (TQColor *)&g.mid();
    cols[3] = (TQColor *)&g.light();
    cols[4] = (TQColor *)&g.dark();
#define CMID	*cols[ (colspec>>12) & 0xf ]
#define CLEFT	*cols[ (colspec>>8) & 0xf ]
#define CTOP	*cols[ (colspec>>4) & 0xf ]
#define CBOT	*cols[ colspec & 0xf ]

    TQPen     savePen   = p->pen();		// save current pen
    TQBrush   saveBrush = p->brush();		// save current brush
#ifndef TQT_NO_TRANSFORMATIONS
    TQWMatrix wxm = p->worldMatrix();
#endif
    TQPen     pen( TQt::NoPen );
    const TQBrush &brush = g.brush( TQColorGroup::Button );

    p->setPen( pen );
    p->setBrush( brush );
#ifndef TQT_NO_TRANSFORMATIONS
    p->setWorldMatrix( matrix, true );		// set transformation matrix
#endif
    p->drawPolygon( bFill );			// fill arrow
    p->setBrush( TQt::NoBrush );			// don't fill

    p->setPen( CLEFT );
    p->drawLineSegments( bLeft );
    p->setPen( CTOP );
    p->drawLineSegments( bTop );
    p->setPen( CBOT );
    p->drawLineSegments( bBot );

#ifndef TQT_NO_TRANSFORMATIONS
    p->setWorldMatrix( wxm );
#endif
    p->setBrush( saveBrush );			// restore brush
    p->setPen( savePen );			// restore pen

#undef CMID
#undef CLEFT
#undef CTOP
#undef CBOT
}
#endif

void qDrawArrow( TQPainter *p, TQt::ArrowType type, TQt::GUIStyle style, bool down,
		 int x, int y, int w, int h,
		 const TQColorGroup &g, bool enabled )
{
    switch ( style ) {
	case TQt::WindowsStyle:
	    qDrawWinArrow( p, type, down, x, y, w, h, g, enabled );
	    break;
#ifndef TQT_NO_STYLE_MOTIF
	case TQt::MotifStyle:
	    qDrawMotifArrow( p, type, down, x, y, w, h, g, enabled );
	    break;
#endif
	default:
#if defined(QT_CHECK_RANGE)
	    tqWarning( "qDrawArrow: Requested GUI style not supported" );
#else
	    ;
#endif
    }
}
#endif //TQT_NO_DRAWUTIL
