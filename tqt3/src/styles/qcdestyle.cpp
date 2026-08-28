/****************************************************************************
**
** Implementation of CDE-like style class
**
** Created : 981231
**
** Copyright (C) 1998-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
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

#include "ntqcdestyle.h"

#if !defined(TQT_NO_STYLE_CDE) || defined(QT_PLUGIN)

#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqbutton.h"
#include <limits.h>

/*!
    \class TQCDEStyle ntqcdestyle.h
    \brief The TQCDEStyle class provides a CDE look and feel.

    \ingroup appearance

    This style provides a slightly improved Motif look similar to some
    versions of the Common Desktop Environment (CDE). The main
    differences are thinner frames and more modern radio buttons and
    checkboxes. Together with a dark background and a bright
    text/foreground color, the style looks quite attractive (at least
    for Motif fans).

    Note that the functions provided by TQCDEStyle are
    reimplementations of TQStyle functions; see TQStyle for their
    documentation.
*/

/*!
    Constructs a TQCDEStyle.

    If \a useHighlightCols is false (the default), then the style will
    polish the application's color palette to emulate the Motif way of
    highlighting, which is a simple inversion between the base and the
    text color.
*/
TQCDEStyle::TQCDEStyle( bool useHighlightCols ) : TQMotifStyle( useHighlightCols )
{
}

/*!
    Destroys the style.
*/
TQCDEStyle::~TQCDEStyle()
{
}


/*!\reimp
*/
int TQCDEStyle::pixelMetric( PixelMetric metric, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *widget ) const
{
    int ret;

    switch( metric ) {
    case PM_DefaultFrameWidth:
	ret = 1;
	break	;
    case PM_MenuBarFrameWidth:
	ret = 1;
	break;
    case PM_ScrollBarExtent:
	ret = 13;
	break;
    default:
	ret = TQMotifStyle::pixelMetric( metric, ceData, elementFlags, widget );
	break;
    }
    return ret;
}

/*! \reimp
*/
void TQCDEStyle::drawControl( ControlElement element,
			     TQPainter *p,
			     const TQStyleControlElementData &ceData,
			     ControlElementFlags elementFlags,
			     const TQRect &r,
			     const TQColorGroup &cg,
			     SFlags how,
			     const TQStyleOption& opt,
			     const TQWidget *widget ) const
{

    switch( element ) {
    case CE_MenuBarItem:
	{
	    if ( how & Style_Active )  // active item
		qDrawShadePanel( p, r, cg, true, 1,
				 &cg.brush( TQColorGroup::Button ) );
	    else  // other item
		p->fillRect( r, cg.brush( TQColorGroup::Button ) );
	    TQCommonStyle::drawControl( element, p, ceData, elementFlags, r, cg, how, opt, widget );
	    break;
	}
    default:
	TQMotifStyle::drawControl( element, p, ceData, elementFlags, r, cg, how, opt, widget );
    break;
    }


}

/*! \reimp
*/
void TQCDEStyle::drawPrimitive( PrimitiveElement pe,
			       TQPainter *p,
			       const TQStyleControlElementData &ceData,
			       ControlElementFlags elementFlags,
			       const TQRect &r,
			       const TQColorGroup &cg,
			       SFlags flags,
			       const TQStyleOption& opt ) const
{
    switch( pe ) {
    case PE_Indicator: {
#ifndef TQT_NO_BUTTON
	bool down = flags & Style_Down;
	bool on = flags & Style_On;
	bool showUp = !( down ^ on );
	TQBrush fill = showUp || flags & Style_NoChange ? cg.brush( TQColorGroup::Button ) : cg.brush( TQColorGroup::Mid );
	qDrawShadePanel( p, r, cg, !showUp, pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags ), &cg.brush( TQColorGroup::Button ) );

	if ( !( flags & Style_Off ) ) {
	    TQPointArray a( 7 * 2 );
	    int i, xx, yy;
	    xx = r.x() + 3;
	    yy = r.y() + 5;
	    for ( i = 0; i < 3; i++ ) {
		a.setPoint( 2 * i,   xx, yy );
		a.setPoint( 2 * i + 1, xx, yy + 2 );
		xx++; yy++;
	    }
	    yy -= 2;
	    for ( i = 3; i < 7; i++ ) {
		a.setPoint( 2 * i, xx, yy );
		a.setPoint( 2 * i + 1, xx, yy + 2 );
		xx++; yy--;
	    }
	    if ( flags & Style_NoChange )
		p->setPen( cg.dark() );
	    else
		p->setPen( cg.foreground() );
	    p->drawLineSegments( a );
	}
#endif
    }
	break;
    case PE_ExclusiveIndicator:
	{
#define TQCOORDARRLEN(x) sizeof(x)/(sizeof(TQCOORD)*2)
	    static const TQCOORD pts1[] = {              // up left  lines
		1,9, 1,8, 0,7, 0,4, 1,3, 1,2, 2,1, 3,1, 4,0, 7,0, 8,1, 9,1 };
	    static const TQCOORD pts4[] = {              // bottom right  lines
		2,10, 3,10, 4,11, 7,11, 8,10, 9,10, 10,9, 10,8, 11,7,
		11,4, 10,3, 10,2 };
	    static const TQCOORD pts5[] = {              // inner fill
		4,2, 7,2, 9,4, 9,7, 7,9, 4,9, 2,7, 2,4 };
	    bool down = flags & Style_Down;
	    bool on = flags & Style_On;
	    p->eraseRect( r );
	    TQPointArray a( TQCOORDARRLEN(pts1), pts1 );
	    a.translate( r.x(), r.y() );
	    p->setPen( ( down || on ) ? cg.dark() : cg.light() );
	    p->drawPolyline( a );
	    a.setPoints( TQCOORDARRLEN(pts4), pts4 );
	    a.translate( r.x(), r.y() );
	    p->setPen( ( down || on ) ? cg.light() : cg.dark() );
	    p->drawPolyline( a );
	    a.setPoints( TQCOORDARRLEN(pts5), pts5 );
	    a.translate( r.x(), r.y() );
	    TQColor fillColor = on ? cg.dark() : cg.background();
	    p->setPen( fillColor );
	    p->setBrush( on ? cg.brush( TQColorGroup::Dark ) :
			 cg.brush( TQColorGroup::Background ) );
	    p->drawPolygon( a );
	    break;
	}

    case PE_ExclusiveIndicatorMask:
	{
	    static const TQCOORD pts1[] = {
		// up left  lines
		1,9, 1,8, 0,7, 0,4, 1,3, 1,2, 2,1, 3,1, 4,0, 7,0, 8,1, 9,1,
		// bottom right  lines
		10,2, 10,3, 11,4, 11,7, 10,8, 10,9, 9,10, 8,10, 7,11, 4,11, 3,10, 2,10
	    };
	    TQPointArray a(TQCOORDARRLEN(pts1), pts1);
	    a.translate(r.x(), r.y());
	    p->setPen(color1);
	    p->setBrush(color1);
	    p->drawPolygon(a);
	    break;
	}
    case PE_ArrowUp:
    case PE_ArrowDown:
    case PE_ArrowRight:
    case PE_ArrowLeft: {
	TQRect rect = r;
	TQPointArray bFill;                          // fill polygon
	TQPointArray bTop;                           // top shadow.
	TQPointArray bBot;                           // bottom shadow.
	TQPointArray bLeft;                          // left shadow.
	TQWMatrix    matrix;                         // xform matrix
	bool vertical = pe == PE_ArrowUp || pe == PE_ArrowDown;
	bool horizontal = !vertical;
	int  dim = rect.width() < rect.height() ? rect.width() : rect.height();
	int  colspec = 0x0000;                      // color specification array

	if ( dim < 2 )                              // too small arrow
	    return;

	// adjust size and center (to fix rotation below)
	if ( rect.width() >  dim ) {
	    rect.setX( rect.x() + ( ( rect.width() - dim ) / 2 ) );
	    rect.setWidth( dim );
	}
	if ( rect.height() > dim ) {
	    rect.setY( rect.y() + ( ( rect.height() - dim ) / 2 ) );
	    rect.setHeight( dim );
	}

	if ( dim > 3 ) {
	    bFill.resize( dim & 1 ? 3 : 4 );
	    bTop.resize( 2 );
	    bBot.resize( 2 );
	    bLeft.resize( 2 );
	    bLeft.putPoints( 0, 2, 0, 0, 0, dim-1 );
	    bTop.putPoints( 0, 2, 1, 0, dim-1, dim/2 );
	    bBot.putPoints( 0, 2, 1, dim-1, dim-1, dim/2 );

	    if ( dim > 6 ) {                        // dim>6: must fill interior
		bFill.putPoints( 0, 2, 1, dim-1, 1, 1 );
		if ( dim & 1 )                      // if size is an odd number
		    bFill.setPoint( 2, dim - 2, dim / 2 );
		else
		    bFill.putPoints( 2, 2, dim-2, dim/2-1, dim-2, dim/2 );
	    }
	} else {
	    if ( dim == 3 ) {                       // 3x3 arrow pattern
		bLeft.setPoints( 4, 0,0, 0,2, 1,1, 1,1 );
		bTop .setPoints( 2, 1,0, 1,0 );
		bBot .setPoints( 2, 1,2, 2,1 );
	    } else {                                  // 2x2 arrow pattern
		bLeft.setPoints( 2, 0,0, 0,1 );
		bTop .setPoints( 2, 1,0, 1,0 );
		bBot .setPoints( 2, 1,1, 1,1 );
	    }
	}

	if ( pe == PE_ArrowUp || pe == PE_ArrowLeft ) {
	    matrix.translate( rect.x(), rect.y() );
	    if ( vertical ) {
		matrix.translate( 0, rect.height() - 1 );
		matrix.rotate( -90 );
	    } else {
		matrix.translate( rect.width() - 1, rect.height() - 1 );
		matrix.rotate( 180 );
	    }
	    if ( flags & Style_Down )
		colspec = horizontal ? 0x2334 : 0x2343;
	    else
		colspec = horizontal ? 0x1443 : 0x1434;
	} else if ( pe == PE_ArrowDown || pe == PE_ArrowRight ) {
	    matrix.translate( rect.x(), rect.y() );
	    if ( vertical ) {
		matrix.translate( rect.width()-1, 0 );
		matrix.rotate( 90 );
	    }
	    if ( flags & Style_Down )
		colspec = horizontal ? 0x2443 : 0x2434;
	    else
		colspec = horizontal ? 0x1334 : 0x1343;
	}

	TQColor *cols[5];
	if ( flags & Style_Enabled ) {
	    cols[0] = 0;
	    cols[1] = (TQColor *)&cg.button();
	    cols[2] = (TQColor *)&cg.mid();
	    cols[3] = (TQColor *)&cg.light();
	    cols[4] = (TQColor *)&cg.dark();
	} else {
	    cols[0] = 0;
	    cols[1] = (TQColor *)&cg.button();
	    cols[2] = (TQColor *)&cg.button();
	    cols[3] = (TQColor *)&cg.button();
	    cols[4] = (TQColor *)&cg.button();
	}

#define CMID    *cols[ (colspec>>12) & 0xf ]
#define CLEFT   *cols[ (colspec>>8) & 0xf ]
#define CTOP    *cols[ (colspec>>4) & 0xf ]
#define CBOT    *cols[ colspec & 0xf ]

	TQPen     savePen   = p->pen();              // save current pen
	TQBrush   saveBrush = p->brush();            // save current brush
	TQWMatrix wxm = p->worldMatrix();
	TQPen     pen( NoPen );
	TQBrush brush = cg.brush( flags & Style_Enabled ? TQColorGroup::Button :
				 TQColorGroup::Mid );

	p->setPen( pen );
	p->setBrush( brush );
	p->setWorldMatrix( matrix, true );          // set transformation matrix
	p->drawPolygon( bFill );                    // fill arrow
	p->setBrush( NoBrush );                     // don't fill

	p->setPen( CLEFT );
	p->drawLineSegments( bLeft );
	p->setPen( CBOT );
	p->drawLineSegments( bBot );
	p->setPen( CTOP );
	p->drawLineSegments( bTop );

	p->setWorldMatrix( wxm );
	p->setBrush( saveBrush );                   // restore brush
	p->setPen( savePen );                       // restore pen

#undef CMID
#undef CLEFT
#undef CTOP
#undef CBOT

    }
	break;
    default:
	TQMotifStyle::drawPrimitive( pe, p, ceData, elementFlags, r, cg, flags, opt );
    }
}

#endif
