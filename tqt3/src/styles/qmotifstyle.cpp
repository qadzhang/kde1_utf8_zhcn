/****************************************************************************
**
** Implementation of Motif-like style class
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

#include "ntqmotifstyle.h"

#if !defined(TQT_NO_STYLE_MOTIF) || defined(QT_PLUGIN)

#include "ntqpopupmenu.h"
#include "ntqapplication.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqpixmap.h"
#include "ntqpalette.h"
#include "ntqwidget.h"
#include "ntqpushbutton.h"
#include "ntqscrollbar.h"
#include "ntqtabbar.h"
#include "ntqtabwidget.h"
#include "ntqlistview.h"
#include "ntqsplitter.h"
#include "ntqslider.h"
#include "ntqcombobox.h"
#include "ntqdockwindow.h"
#include "ntqdockarea.h"
#include "ntqprogressbar.h"
#include "ntqimage.h"
#include <limits.h>



// old constants that might still be useful...
static const int motifItemFrame         = 2;    // menu item frame width
static const int motifSepHeight         = 2;    // separator item height
static const int motifItemHMargin       = 3;    // menu item hor text margin
static const int motifItemVMargin       = 2;    // menu item ver text margin
static const int motifArrowHMargin      = 6;    // arrow horizontal margin
static const int motifTabSpacing        = 12;   // space between text and tab
static const int motifCheckMarkHMargin  = 2;    // horiz. margins of check mark
static const int motifCheckMarkSpace    = 12;


/*!
    \class TQMotifStyle ntqmotifstyle.h
    \brief The TQMotifStyle class provides Motif look and feel.

    \ingroup appearance

    This class implements the Motif look and feel. It closely
    resembles the original Motif look as defined by the Open Group,
    but with some minor improvements. The Motif style is TQt's default
    GUI style on UNIX platforms.
*/

/*!
    Constructs a TQMotifStyle.

    If \a useHighlightCols is false (the default), the style will
    polish the application's color palette to emulate the Motif way of
    highlighting, which is a simple inversion between the base and the
    text color.
*/
TQMotifStyle::TQMotifStyle( bool useHighlightCols ) : TQCommonStyle()
{
    highlightCols = useHighlightCols;
}

/*!\reimp
*/
TQMotifStyle::~TQMotifStyle()
{
}

/*!
    If \a arg is false, the style will polish the application's color
    palette to emulate the Motif way of highlighting, which is a
    simple inversion between the base and the text color.

    The effect will show up the next time an application palette is
    set via TQApplication::setPalette(). The current color palette of
    the application remains unchanged.

    \sa TQStyle::polish()
*/
void TQMotifStyle::setUseHighlightColors( bool arg )
{
    highlightCols = arg;
}

/*!
    Returns true if the style treats the highlight colors of the
    palette in a Motif-like manner, which is a simple inversion
    between the base and the text color; otherwise returns false. The
    default is false.
*/
bool TQMotifStyle::useHighlightColors() const
{
    return highlightCols;
}

/*! \reimp */

void TQMotifStyle::polish( TQPalette& pal )
{
    if ( pal.active().light() == pal.active().base() ) {
        TQColor nlight = pal.active().light().dark(108 );
        pal.setColor( TQPalette::Active, TQColorGroup::Light, nlight ) ;
        pal.setColor( TQPalette::Disabled, TQColorGroup::Light, nlight ) ;
        pal.setColor( TQPalette::Inactive, TQColorGroup::Light, nlight ) ;
    }

    if ( highlightCols )
        return;

    // force the ugly motif way of highlighting *sigh*
    TQColorGroup disabled = pal.disabled();
    TQColorGroup active = pal.active();

    pal.setColor( TQPalette::Active, TQColorGroup::Highlight,
                  active.text() );
    pal.setColor( TQPalette::Active, TQColorGroup::HighlightedText,
                  active.base());
    pal.setColor( TQPalette::Disabled, TQColorGroup::Highlight,
                  disabled.text() );
    pal.setColor( TQPalette::Disabled, TQColorGroup::HighlightedText,
                  disabled.base() );
    pal.setColor( TQPalette::Inactive, TQColorGroup::Highlight,
                  active.text() );
    pal.setColor( TQPalette::Inactive, TQColorGroup::HighlightedText,
                  active.base() );
}

/*!
 \reimp
 \internal
 Keep TQStyle::polish() visible.
*/
void TQMotifStyle::polish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr )
{
    TQStyle::polish(ceData, elementFlags, ptr);
}

/*!
 \reimp
 \internal
 Keep TQStyle::polish() visible.
*/
void TQMotifStyle::applicationPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr )
{
    TQStyle::applicationPolish(ceData, elementFlags, ptr);
}

static void rot(TQPointArray& a, int n)
{
    TQPointArray r(a.size());
    for (int i = 0; i < (int)a.size(); i++) {
	switch (n) {
	    case 1: r.setPoint(i,-a[i].y(),a[i].x()); break;
	    case 2: r.setPoint(i,-a[i].x(),-a[i].y()); break;
	    case 3: r.setPoint(i,a[i].y(),-a[i].x()); break;
	}
    }
    a = r;
}


/*!\reimp
*/
void TQMotifStyle::drawPrimitive( PrimitiveElement pe,
				 TQPainter *p,
				 const TQStyleControlElementData &ceData,
				 ControlElementFlags elementFlags,
				 const TQRect &r,
				 const TQColorGroup &cg,
				 SFlags flags,
				 const TQStyleOption& opt ) const
{
    bool dis = ! (flags & Style_Enabled);
    bool act = flags & Style_Active;

    switch( pe ) {
#ifndef TQT_NO_LISTVIEW
    case PE_CheckListExclusiveIndicator: {
	TQCheckListItem *item = opt.checkListItem();
	TQListView *lv = item->listView();
	if(!item)
	    return;

	if ( item->isEnabled() )
	    p->setPen( TQPen( cg.text() ) );
	else
	    p->setPen( TQPen( lv->palette().color( TQPalette::Disabled, TQColorGroup::Text ) ) );
	TQPointArray a;

	int cx = r.width()/2 - 1;
	int cy = r.height()/2;
	int e = r.width()/2 - 1;
	for ( int i = 0; i < 3; i++ ) { //penWidth 2 doesn't quite work
	    a.setPoints( 4, cx-e, cy, cx, cy-e,  cx+e, cy,  cx, cy+e );
	    p->drawPolygon( a );
	    e--;
	}
	if ( item->isOn() ) {
	    if ( item->isEnabled() )
		p->setPen( TQPen( cg.text()) );
	    else
		p->setPen( TQPen( item->listView()->palette().color( TQPalette::Disabled,
								    TQColorGroup::Text ) ) );
	    TQBrush saveBrush = p->brush();
	    p->setBrush( cg.text() );
	    e = e - 2;
	    a.setPoints( 4, cx-e, cy, cx, cy-e,  cx+e, cy,  cx, cy+e );
	    p->drawPolygon( a );
	    p->setBrush( saveBrush );
	}
	break; }
#endif
    case PE_ButtonCommand:
    case PE_ButtonBevel:
    case PE_ButtonTool:
    case PE_HeaderSection:
	qDrawShadePanel( p, r, cg, bool(flags & (Style_Down | Style_On )),
			 pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags),
			 &cg.brush(TQColorGroup::Button) );
	break;

    case PE_Indicator: {
#ifndef TQT_NO_BUTTON
	bool on = flags & Style_On;
	bool down = flags & Style_Down;
	bool showUp = !( down ^ on );
	TQBrush fill = showUp || flags & Style_NoChange ? cg.brush( TQColorGroup::Button ) : cg.brush(TQColorGroup::Mid );
	if ( flags & Style_NoChange ) {
	    qDrawPlainRect( p, r, cg.text(),
			    1, &fill );
	    p->drawLine( r.x() + r.width() - 1, r.y(),
			 r.x(), r.y() + r.height() - 1);
	} else
	    qDrawShadePanel( p, r, cg, !showUp,
			     pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags), &fill );
#endif
	break;
    }

    case PE_ExclusiveIndicator:
	{
#define TQCOORDARRLEN(x) sizeof(x)/(sizeof(TQCOORD)*2)
	    TQCOORD inner_pts[] = { // used for filling diamond
		2,r.height()/2,
		r.width()/2,2,
		r.width()-3,r.height()/2,
		r.width()/2,r.height()-3
	    };
	    TQCOORD top_pts[] = { // top (^) of diamond
		0,r.height()/2,
		r.width()/2,0,
		r.width()-2,r.height()/2-1,
		r.width()-3,r.height()/2-1,
		r.width()/2,1,
		1,r.height()/2,
		2,r.height()/2,
		r.width()/2,2,
		r.width()-4,r.height()/2-1
	    };
	    TQCOORD bottom_pts[] = { // bottom (v) of diamond
		1,r.height()/2+1,
		r.width()/2,r.height()-1,
		r.width()-1,r.height()/2,
		r.width()-2,r.height()/2,
		r.width()/2,r.height()-2,
		2,r.height()/2+1,
		3,r.height()/2+1,
		r.width()/2,r.height()-3,
		r.width()-3,r.height()/2
	    };
	    bool on = flags & Style_On;
	    bool down = flags & Style_Down;
	    bool showUp = !(down ^ on );
	    TQPointArray a( TQCOORDARRLEN(inner_pts), inner_pts );
	    p->eraseRect( r );
	    p->setPen( NoPen );
	    p->setBrush( showUp ? cg.brush( TQColorGroup::Button ) :
			 cg.brush( TQColorGroup::Mid ) );
	    a.translate( r.x(), r.y() );
	    p->drawPolygon( a );
	    p->setPen( showUp ? cg.light() : cg.dark() );
	    p->setBrush( NoBrush );
	    a.setPoints( TQCOORDARRLEN(top_pts), top_pts );
	    a.translate( r.x(), r.y() );
	    p->drawPolyline( a );
	    p->setPen( showUp ? cg.dark() : cg.light() );
	    a.setPoints( TQCOORDARRLEN(bottom_pts), bottom_pts );
	    a.translate( r.x(), r.y() );
	    p->drawPolyline( a );

	    break;
	}

    case PE_ExclusiveIndicatorMask:
	{
	    static TQCOORD inner_pts[] = { // used for filling diamond
		0,r.height()/2,
		r.width()/2,0,
		r.width()-1,r.height()/2,
		r.width()/2,r.height()-1
	    };
	    TQPointArray a(TQCOORDARRLEN(inner_pts), inner_pts);
	    p->setPen(color1);
	    p->setBrush(color1);
	    a.translate(r.x(), r.y());
	    p->drawPolygon(a);
	    break;
	}

    case PE_ArrowUp:
    case PE_ArrowDown:
    case PE_ArrowRight:
    case PE_ArrowLeft:
	{
	    TQRect rect = r;
	    TQPointArray bFill;
	    TQPointArray bTop;
	    TQPointArray bBot;
	    TQPointArray bLeft;
	    bool vertical = pe == PE_ArrowUp || pe == PE_ArrowDown;
	    bool horizontal = !vertical;
	    int dim = rect.width() < rect.height() ? rect.width() : rect.height();
	    int colspec = 0x0000;

	    if ( dim < 2 )
		break;

	    // adjust size and center (to fix rotation below)
	    if ( rect.width() > dim ) {
		rect.setX( rect.x() + ((rect.width() - dim ) / 2) );
		rect.setWidth( dim );
	    }
	    if ( rect.height() > dim ) {
		rect.setY( rect.y() + ((rect.height() - dim ) / 2 ));
		rect.setHeight( dim );
	    }

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
		if ( dim & 1 )                          // odd number size: extra line
		    bBot.putPoints( dim-1, 2, dim-3,dim/2, dim-1,dim/2 );
		if ( dim > 6 ) {                        // dim>6: must fill interior
		    bFill.putPoints( 0, 2, 1,dim-3, 1,2 );
		    if ( dim & 1 )                      // if size is an odd number
			bFill.setPoint( 2, dim - 3, dim / 2 );
		    else
			bFill.putPoints( 2, 2, dim-4,dim/2-1, dim-4,dim/2 );
		}
	    }
	    else {
		if ( dim == 3 ) {                       // 3x3 arrow pattern
		    bLeft.setPoints( 4, 0,0, 0,2, 1,1, 1,1 );
		    bTop .setPoints( 2, 1,0, 1,0 );
		    bBot .setPoints( 2, 1,2, 2,1 );
		}
		else {                                  // 2x2 arrow pattern
		    bLeft.setPoints( 2, 0,0, 0,1 );
		    bTop .setPoints( 2, 1,0, 1,0 );
		    bBot .setPoints( 2, 1,1, 1,1 );
		}
	    }

	    // We use rot() and translate() as it is more efficient that
	    // matrix transformations on the painter, and because it still
	    // works with TQT_NO_TRANSFORMATIONS defined.

	    if ( pe == PE_ArrowUp || pe == PE_ArrowLeft ) {
		if ( vertical ) {
		    rot(bFill,3);
		    rot(bLeft,3);
		    rot(bTop,3);
		    rot(bBot,3);
		    bFill.translate( 0, rect.height() - 1 );
		    bLeft.translate( 0, rect.height() - 1 );
		    bTop.translate( 0, rect.height() - 1 );
		    bBot.translate( 0, rect.height() - 1 );
		} else {
		    rot(bFill,2);
		    rot(bLeft,2);
		    rot(bTop,2);
		    rot(bBot,2);
		    bFill.translate( rect.width() - 1, rect.height() - 1 );
		    bLeft.translate( rect.width() - 1, rect.height() - 1 );
		    bTop.translate( rect.width() - 1, rect.height() - 1 );
		    bBot.translate( rect.width() - 1, rect.height() - 1 );
		}
		if ( flags & Style_Down )
		    colspec = horizontal ? 0x2334 : 0x2343;
		else
		    colspec = horizontal ? 0x1443 : 0x1434;
	    } else {
		if ( vertical ) {
		    rot(bFill,1);
		    rot(bLeft,1);
		    rot(bTop,1);
		    rot(bBot,1);
		    bFill.translate( rect.width() - 1, 0 );
		    bLeft.translate( rect.width() - 1, 0 );
		    bTop.translate( rect.width() - 1, 0 );
		    bBot.translate( rect.width() - 1, 0 );
		}
		if ( flags & Style_Down )
		    colspec = horizontal ? 0x2443 : 0x2434;
		else
		    colspec = horizontal ? 0x1334 : 0x1343;
	    }
	    bFill.translate( rect.x(), rect.y() );
	    bLeft.translate( rect.x(), rect.y() );
	    bTop.translate( rect.x(), rect.y() );
	    bBot.translate( rect.x(), rect.y() );

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

#define CMID *cols[ (colspec>>12) & 0xf ]
#define CLEFT *cols[ (colspec>>8) & 0xf ]
#define CTOP *cols[ (colspec>>4) & 0xf ]
#define CBOT *cols[ colspec & 0xf ]

	    TQPen savePen = p->pen();
	    TQBrush saveBrush = p->brush();
	    TQPen pen( NoPen );
	    TQBrush brush = cg.brush( flags & Style_Enabled ? TQColorGroup::Button :
				     TQColorGroup::Mid );
	    p->setPen( pen );
	    p->setBrush( brush );
	    p->drawPolygon( bFill );
	    p->setBrush( NoBrush );

	    p->setPen( CLEFT );
	    p->drawLineSegments( bLeft );
	    p->setPen( CTOP );
	    p->drawLineSegments( bTop );
	    p->setPen( CBOT );
	    p->drawLineSegments( bBot );

	    p->setBrush( saveBrush );
	    p->setPen( savePen );
#undef CMID
#undef CLEFT
#undef CTOP
#undef CBOT
	    break;
	}

    case PE_SpinWidgetPlus:
    case PE_SpinWidgetMinus:
	{
	    p->save();
	    int fw = pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags );
	    TQRect br;
	    br.setRect( r.x() + fw, r.y() + fw, r.width() - fw*2,
			r.height() - fw*2 );

	    if ( flags & Style_Sunken )
		p->fillRect( r, cg.brush( TQColorGroup::Dark ) );
	    else
		p->fillRect( r, cg.brush( TQColorGroup::Button ) );

	    p->setPen( cg.buttonText() );
	    p->setBrush( cg.buttonText() );

	    int length;
	    int x = r.x(), y = r.y(), w = r.width(), h = r.height();
	    if ( w <= 8 || h <= 6 )
		length = TQMIN( w-2, h-2 );
	    else
		length = TQMIN( 2*w / 3, 2*h / 3 );

	    if ( !(length & 1) )
		length -=1;
	    int xmarg = ( w - length ) / 2;
	    int ymarg = ( h - length ) / 2;

	    p->drawLine( x + xmarg, ( y + h / 2 - 1 ),
			 x + xmarg + length - 1, ( y + h / 2 - 1 ) );
	    if ( pe == PE_SpinWidgetPlus )
		p->drawLine( ( x+w / 2 ) - 1, y + ymarg,
			     ( x+w / 2 ) - 1, y + ymarg + length - 1 );
	    p->restore();
	    break;
	}

    case PE_SpinWidgetUp:
    case PE_SpinWidgetDown:
	{
	    p->save();
	    int fw = pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags );
	    TQRect br;
	    br.setRect( r.x() + fw, r.y() + fw, r.width() - fw*2,
			r.height() - fw*2 );
	    if ( flags & Style_Sunken )
		p->fillRect( br, cg.brush( TQColorGroup::Mid ) );
	    else
		p->fillRect( br, cg.brush( TQColorGroup::Button ) );

	    int x = r.x(), y = r.y(), w = r.width(), h = r.height();
	    int sw = w-4;
	    if ( sw < 3 )
		return;
	    else if ( !(sw & 1) )
		sw--;
	    sw -= ( sw / 7 ) * 2;	// Empty border
	    int sh = sw/2 + 2;      // Must have empty row at foot of arrow

	    int sx = x + w / 2 - sw / 2 - 1;
	    int sy = y + h / 2 - sh / 2 - 1;

	    TQPointArray a;
	    if ( pe == PE_SpinWidgetDown )
		a.setPoints( 3,  0, 1,  sw-1, 1,  sh-2, sh-1 );
	    else
		a.setPoints( 3,  0, sh-1,  sw-1, sh-1,  sh-2, 1 );
	    int bsx = 0;
	    int bsy = 0;
	    if ( flags & Style_Sunken ) {
		bsx = pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags);
		bsy = pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags);
	    }
	    p->translate( sx + bsx, sy + bsy );
	    p->setPen( cg.buttonText() );
	    p->setBrush( cg.buttonText() );
	    p->drawPolygon( a );
	    p->restore();
	    break;
	}

    case PE_DockWindowHandle:
	{
	    p->save();
	    p->translate( r.x(), r.y() );

	    TQColor dark( cg.dark() );
	    TQColor light( cg.light() );
	    unsigned int i;
	    if ( flags & Style_Horizontal ) {
		int h = r.height();
		if ( h > 6 ) {
		    if ( flags & Style_On )
			p->fillRect( 1, 1, 8, h - 2, cg.highlight() );
		    TQPointArray a( 2 * ((h-6)/3) );
		    int y = 3 + (h%3)/2;
		    p->setPen( dark );
		    p->drawLine( 8, 1, 8, h-2 );
		    for( i=0; 2*i < a.size(); i ++ ) {
			a.setPoint( 2*i, 5, y+1+3*i );
			a.setPoint( 2*i+1, 2, y+2+3*i );
		    }
		    p->drawPoints( a );
		    p->setPen( light );
		    p->drawLine( 9, 1, 9, h-2 );
		    for( i=0; 2*i < a.size(); i++ ) {
			a.setPoint( 2*i, 4, y+3*i );
			a.setPoint( 2*i+1, 1, y+1+3*i );
		    }
		    p->drawPoints( a );
		    // if ( drawBorder ) {
		    // p->setPen( TQPen( TQt::darkGray ) );
		    // p->drawLine( 0, r.height() - 1,
		    // tbExtent, r.height() - 1 );
		    // }
		}
	    } else {
		int w = r.width();
		if ( w > 6 ) {
		    if ( flags & Style_On )
			p->fillRect( 1, 1, w - 2, 9, cg.highlight() );
		    TQPointArray a( 2 * ((w-6)/3) );

		    int x = 3 + (w%3)/2;
		    p->setPen( dark );
		    p->drawLine( 1, 8, w-2, 8 );
		    for( i=0; 2*i < a.size(); i ++ ) {
			a.setPoint( 2*i, x+1+3*i, 6 );
			a.setPoint( 2*i+1, x+2+3*i, 3 );
		    }
		    p->drawPoints( a );
		    p->setPen( light );
		    p->drawLine( 1, 9, w-2, 9 );
		    for( i=0; 2*i < a.size(); i++ ) {
			a.setPoint( 2*i, x+3*i, 5 );
			a.setPoint( 2*i+1, x+1+3*i, 2 );
		    }
		    p->drawPoints( a );
		    // if ( drawBorder ) {
		    // p->setPen( TQPen( TQt::darkGray ) );
		    // p->drawLine( r.width() - 1, 0,
		    // r.width() - 1, tbExtent );
		    // }
		}
	    }
	    p->restore();
	    break;
	}

    case PE_Splitter:
	if (flags & Style_Horizontal)
	    flags &= ~Style_Horizontal;
	else
	    flags |= Style_Horizontal;
	// fall through intended

    case PE_DockWindowResizeHandle:
	{
	    const int motifOffset = 10;
 	    int sw = pixelMetric( PM_SplitterWidth, ceData, elementFlags );
	    if ( flags & Style_Horizontal ) {
 		TQCOORD yPos = r.y() + r.height() / 2;
 		TQCOORD kPos = r.width() - motifOffset - sw;
 		TQCOORD kSize = sw - 2;

 		qDrawShadeLine( p, 0, yPos, kPos, yPos, cg );
 		qDrawShadePanel( p, kPos, yPos - sw / 2 + 1, kSize, kSize,
 				 cg, false, 1, &cg.brush( TQColorGroup::Button ) );
 		qDrawShadeLine( p, kPos + kSize - 1, yPos, r.width(), yPos, cg );
 	    } else {
 		TQCOORD xPos = r.x() + r.width() / 2;
 		TQCOORD kPos = motifOffset;
 		TQCOORD kSize = sw - 2;

 		qDrawShadeLine( p, xPos, kPos + kSize - 1, xPos, r.height(), cg );
 		qDrawShadePanel( p, xPos - sw / 2 + 1, kPos, kSize, kSize, cg,
 				 false, 1, &cg.brush( TQColorGroup::Button ) );
 		qDrawShadeLine( p, xPos, 0, xPos, kPos, cg );
 	    }
 	    break;
	}

    case PE_CheckMark:
	{
	    const int markW = 6;
	    const int markH = 6;
	    int posX = r.x() + ( r.width()  - markW ) / 2 - 1;
	    int posY = r.y() + ( r.height() - markH ) / 2;
	    int dfw = pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags);

	    if (dfw < 2) {
		// Could do with some optimizing/caching...
		TQPointArray a( 7*2 );
		int i, xx, yy;
		xx = posX;
		yy = 3 + posY;
		for ( i=0; i<3; i++ ) {
		    a.setPoint( 2*i,   xx, yy );
		    a.setPoint( 2*i+1, xx, yy+2 );
		    xx++; yy++;
		}
		yy -= 2;
		for ( i=3; i<7; i++ ) {
		    a.setPoint( 2*i,   xx, yy );
		    a.setPoint( 2*i+1, xx, yy+2 );
		    xx++; yy--;
		}
		if ( ! (flags & Style_Enabled) && ! (flags & Style_On) ) {
		    int pnt;
		    p->setPen( cg.highlightedText() );
		    TQPoint offset(1,1);
		    for ( pnt = 0; pnt < (int)a.size(); pnt++ )
			a[pnt] += offset;
		    p->drawLineSegments( a );
		    for ( pnt = 0; pnt < (int)a.size(); pnt++ )
			a[pnt] -= offset;
		}
		p->setPen( cg.text() );
		p->drawLineSegments( a );

		qDrawShadePanel( p, posX-2, posY-2, markW+4, markH+6, cg, true, dfw);
	    } else
		qDrawShadePanel( p, posX, posY, markW, markH, cg, true, dfw,
				 &cg.brush( TQColorGroup::Mid ) );

	    break;
	}

    case PE_ScrollBarSubLine:
	drawPrimitive(((flags & Style_Horizontal) ? PE_ArrowLeft : PE_ArrowUp),
		      p, ceData, elementFlags, r, cg, Style_Enabled | flags);
	break;

    case PE_ScrollBarAddLine:
	drawPrimitive(((flags & Style_Horizontal) ? PE_ArrowRight : PE_ArrowDown),
		      p, ceData, elementFlags, r, cg, Style_Enabled | flags);
	break;

    case PE_ScrollBarSubPage:
    case PE_ScrollBarAddPage:
	p->fillRect(r, cg.brush(TQColorGroup::Mid));
	break;

    case PE_ScrollBarSlider:
	drawPrimitive(PE_ButtonBevel, p, ceData, elementFlags, r, cg,
		      (flags | Style_Raised) & ~Style_Down);
	break;

    case PE_ProgressBarChunk:
	p->fillRect( r.x(), r.y() + 2, r.width() - 2,
		     r.height() - 4, cg.brush(TQColorGroup::Highlight));
	break;

    case PE_PanelScrollBar:
	qDrawShadePanel(p, r, cg, true,
		    pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags),
		    &cg.brush(TQColorGroup::Mid));
	break;

    case PE_MenuItemIndicatorFrame:
	{
	    // Draw nothing
	    break;
	}

    case PE_MenuItemIndicatorIconFrame:
	{
	    int x, y, w, h;
	    r.rect( &x, &y, &w, &h );
	    int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags, opt, NULL, NULL);

	    TQRect vrect = visualRect( TQRect( x+motifItemFrame, y+motifItemFrame, checkcol, h-2*motifItemFrame ), r );
	    int xvis = vrect.x();

	    qDrawShadePanel( p, xvis, y+motifItemFrame, checkcol, h-2*motifItemFrame, cg, true, 1, &cg.brush( TQColorGroup::Midlight ) );
	    break;
	}

    case PE_MenuItemIndicatorCheck:
	{
	    int x, y, w, h;
	    r.rect( &x, &y, &w, &h );
	    int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags, opt, NULL, NULL);

	    TQRect vrect = visualRect( TQRect( x+motifItemFrame, y+motifItemFrame, checkcol, h-2*motifItemFrame ), r );
	    int xvis = vrect.x();
	    int mw = checkcol;
	    int mh = h - 2*motifItemFrame;

	    SFlags cflags = Style_Default;
	    if (! dis)
		cflags |= Style_Enabled;
	    if (act)
		cflags |= Style_On;

	    drawPrimitive(PE_CheckMark, p, ceData, elementFlags, TQRect(xvis, y+motifItemFrame, mw, mh), cg, cflags);
	    break;
	}

    default:
	TQCommonStyle::drawPrimitive( pe, p, ceData, elementFlags, r, cg, flags, opt );
	break;
    }
}


/*!\reimp
*/
void TQMotifStyle::drawControl( ControlElement element,
			       TQPainter *p,
			       const TQStyleControlElementData &ceData,
			       ControlElementFlags elementFlags,
			       const TQRect &r,
			       const TQColorGroup &cg,
			       SFlags flags,
			       const TQStyleOption& opt,
			       const TQWidget *widget ) const
{
    switch( element ) {
    case CE_PushButton:
	{
#ifndef TQT_NO_PUSHBUTTON
 	    int diw, x1, y1, x2, y2;
	    TQColorGroup newCg = cg;
 	    p->setPen( cg.foreground() );
 	    p->setBrush( TQBrush( cg.button(), NoBrush ) );
 	    diw = pixelMetric( PM_ButtonDefaultIndicator, ceData, elementFlags );
 	    r.coords( &x1, &y1, &x2, &y2 );
 	    if ((elementFlags & CEF_IsDefault) || (elementFlags & CEF_AutoDefault)) {
 		x1 += diw;
 		y1 += diw;
 		x2 -= diw;
 		y2 -= diw;
 	    }
 	    TQBrush fill;
 	    if ( elementFlags & CEF_IsDown )
 		fill = newCg.brush( TQColorGroup::Mid );
 	    else if ( elementFlags & CEF_IsOn )
 		fill = TQBrush( newCg.mid(), Dense4Pattern );
 	    else
 		fill = newCg.brush( TQColorGroup::Button );

	    newCg.setBrush( TQColorGroup::Button, fill );
 	    if ( elementFlags & CEF_IsDefault ) {
 		if ( diw == 0 ) {
		    TQPointArray a;
		    a.setPoints( 9,
				 x1, y1, x2, y1, x2, y2, x1, y2, x1, y1+1,
				 x2-1, y1+1, x2-1, y2-1, x1+1, y2-1, x1+1, y1+1 );
 		    p->setPen( newCg.shadow() );
 		    p->drawPolygon( a );
 		    x1 += 2;
 		    y1 += 2;
 		    x2 -= 2;
 		    y2 -= 2;
 		} else {
 		    qDrawShadePanel( p, r, newCg, true );
 		}
 	    }
 	    if ( !( elementFlags & CEF_IsFlat ) || ( elementFlags & CEF_IsOn ) || ( elementFlags & CEF_IsDown ) ) {
		TQRect tmp( x1, y1, x2 - x1 + 1, y2 - y1 + 1 );
		SFlags flags = Style_Default;
		if ( elementFlags & CEF_IsOn )
		    flags |= Style_On;
		if ( elementFlags & CEF_IsDown )
		    flags |= Style_Down;
		p->save();
		p->setBrushOrigin( -ceData.bgOffset.x(),
				   -ceData.bgOffset.y() );
		drawPrimitive( PE_ButtonCommand, p, ceData, elementFlags,
			       tmp, newCg,
 			       flags );
		p->restore();
	    }
 	    if ( p->brush().style() != NoBrush )
 		p->setBrush( NoBrush );
#endif
	    break;
	}

    case CE_TabBarTab:
	{
#ifndef TQT_NO_TABBAR
	    if ( (elementFlags & CEF_UseGenericParameters) || !(elementFlags & CEF_HasParentWidget) || !opt.tab() )
		break;

	    const TQTab * t = opt.tab();

	    int dfw = pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags, widget );
	    bool selected = flags & Style_Selected;
	    int o =  dfw > 1 ? 1 : 0;
	    bool lastTab = false;

	    TQRect r2( r );
	    if ( ceData.tabBarData.shape == TQTabBar::RoundedAbove ) {
		if ( styleHint( SH_TabBar_Alignment, ceData, elementFlags, TQStyleOption::Default, 0, widget ) == AlignRight &&
		     ceData.tabBarData.identIndexMap[t->identifier()] == ceData.tabBarData.tabCount-1 )
		    lastTab = true;

		if ( o ) {
		    p->setPen( ceData.colorGroup.light() );
		    p->drawLine( r2.left(), r2.bottom(), r2.right(), r2.bottom() );
		    p->setPen( ceData.colorGroup.light() );
		    p->drawLine( r2.left(), r2.bottom()-1, r2.right(), r2.bottom()-1 );
		    if ( r2.left() == 0 )
			p->drawPoint( ceData.rect.bottomLeft() );
		}
		else {
		    p->setPen( ceData.colorGroup.light() );
		    p->drawLine( r2.left(), r2.bottom(), r2.right(), r2.bottom() );
		}

		if ( selected ) {
		    p->fillRect( TQRect( r2.left()+1, r2.bottom()-o, r2.width()-3, 2),
				 ceData.palette.active().brush( TQColorGroup::Background ));
		    p->setPen( ceData.colorGroup.background() );
		    // p->drawLine( r2.left()+1, r2.bottom(), r2.right()-2, r2.bottom() );
		    // if (o)
		    // p->drawLine( r2.left()+1, r2.bottom()-1, r2.right()-2, r2.bottom()-1 );
		    p->drawLine( r2.left()+1, r2.bottom(), r2.left()+1, r2.top()+2 );
		    p->setPen( ceData.colorGroup.light() );
		} else {
		    p->setPen( ceData.colorGroup.light() );
		    r2.setRect( r2.left() + 2, r2.top() + 2,
				r2.width() - 4, r2.height() - 2 );
		}

		p->drawLine( r2.left(), r2.bottom()-1, r2.left(), r2.top() + 2 );
		p->drawPoint( r2.left()+1, r2.top() + 1 );
		p->drawLine( r2.left()+2, r2.top(),
			     r2.right() - 2, r2.top() );
		p->drawPoint( r2.left(), r2.bottom());

		if ( o ) {
		    p->drawLine( r2.left()+1, r2.bottom(), r2.left()+1, r2.top() + 2 );
		    p->drawLine( r2.left()+2, r2.top()+1,
				 r2.right() - 2, r2.top()+1 );
		}

		p->setPen( ceData.colorGroup.dark() );
		p->drawLine( r2.right() - 1, r2.top() + 2,
			     r2.right() - 1, r2.bottom() - 1 + (selected ? o : -o));
		if ( o ) {
		    p->drawPoint( r2.right() - 1, r2.top() + 1 );
		    p->drawLine( r2.right(), r2.top() + 2, r2.right(),
				 r2.bottom() -
				 (selected ? (lastTab ? 0:1):1+o));
		    p->drawPoint( r2.right() - 1, r2.top() + 1 );
		}
	    } else if ( ceData.tabBarData.shape  == TQTabBar::RoundedBelow ) {
		if ( styleHint( SH_TabBar_Alignment, ceData, elementFlags, TQStyleOption::Default, 0, widget ) == AlignLeft &&
		     ceData.tabBarData.identIndexMap[t->identifier()] == ceData.tabBarData.tabCount-1 )
		    lastTab = true;
		if ( selected ) {
		    p->fillRect( TQRect( r2.left()+1, r2.top(), r2.width()-3, 1),
				 ceData.palette.active().brush( TQColorGroup::Background ));
		    p->setPen( ceData.colorGroup.background() );
		    // p->drawLine( r2.left()+1, r2.top(), r2.right()-2, r2.top() );
		    p->drawLine( r2.left()+1, r2.top(), r2.left()+1, r2.bottom()-2 );
		    p->setPen( ceData.colorGroup.dark() );
		} else {
		    p->setPen( ceData.colorGroup.dark() );
		    p->drawLine( r2.left(), r2.top(), r2.right(), r2.top() );
		    p->drawLine( r2.left() + 1, r2.top() + 1,
				 r2.right() - (lastTab ? 0 : 2),
				 r2.top() + 1 );
		    r2.setRect( r2.left() + 2, r2.top(),
				r2.width() - 4, r2.height() - 2 );
		}

		p->drawLine( r2.right() - 1, r2.top(),
			     r2.right() - 1, r2.bottom() - 2 );
		p->drawPoint( r2.right() - 2, r2.bottom() - 2 );
		p->drawLine( r2.right() - 2, r2.bottom() - 1,
			     r2.left() + 1, r2.bottom() - 1 );
		p->drawPoint( r2.left() + 1, r2.bottom() - 2 );

		if (dfw > 1) {
		    p->drawLine( r2.right(), r2.top(),
				 r2.right(), r2.bottom() - 1 );
		    p->drawPoint( r2.right() - 1, r2.bottom() - 1 );
		    p->drawLine( r2.right() - 1, r2.bottom(),
				 r2.left() + 2, r2.bottom() );
		}

		p->setPen( ceData.colorGroup.light() );
		p->drawLine( r2.left(), r2.top() + (selected ? 0 : 2),
			     r2.left(), r2.bottom() - 2 );
		p->drawLine( r2.left() + 1, r2.top() + (selected ? 0 : 2),
			     r2.left() + 1, r2.bottom() - 3 );

	    } else {
		TQCommonStyle::drawControl( element, p, ceData, elementFlags, r, cg, flags, opt, widget );
	    }
#endif
	    break;
	}

    case CE_ProgressBarGroove:
	qDrawShadePanel(p, r, cg, true, 2);
	break;

    case CE_ProgressBarLabel:
	{
#ifndef TQT_NO_PROGRESSBAR
	    const int unit_width = pixelMetric( PM_ProgressBarChunkWidth, ceData, elementFlags, widget );
	    int u = r.width() / unit_width;
	    int p_v = ceData.currentStep;
	    int t_s = ceData.totalSteps;
	    if ( u > 0 && ceData.currentStep >= INT_MAX / u && t_s >= u ) {
		// scale down to something usable.
		p_v /= u;
		t_s /= u;
	    }
	    if ( ceData.percentageVisible && ceData.totalSteps ) {
		int nu = ( u * p_v + t_s/2 ) / t_s;
		int x = unit_width * nu;
		if ((elementFlags & CEF_IndicatorFollowsStyle) || (elementFlags & CEF_CenterIndicator)) {
		    p->setPen( cg.highlightedText() );
		    p->setClipRect( r.x(), r.y(), x, r.height() );
		    p->drawText( r, AlignCenter | SingleLine, ceData.progressText );

		    if ( ceData.currentStep != ceData.totalSteps ) {
			p->setClipRect( r.x() + x, r.y(), r.width() - x, r.height() );
			p->setPen( cg.highlight() );
			p->drawText( r, AlignCenter | SingleLine, ceData.progressText );
		    }
		} else {
		    p->setPen( cg.text() );
		    p->drawText( r, AlignCenter | SingleLine, ceData.progressText );
		}
	    }
#endif
	    break;
	}

#ifndef TQT_NO_POPUPMENU
    case CE_PopupMenuItem:
	{
	    if ((elementFlags & CEF_UseGenericParameters) || opt.isDefault())
		break;

	    TQMenuItem *mi = opt.menuItem();
	    if ( !mi )
		break;

	    int tab = opt.tabWidth();
	    int maxpmw = opt.maxIconWidth();
	    bool dis = ! (flags & Style_Enabled);
	    bool checkable = (elementFlags & CEF_IsCheckable);
	    bool act = flags & Style_Active;
	    int x, y, w, h;

	    r.rect(&x, &y, &w, &h);

	    if ( checkable )
		maxpmw = TQMAX( maxpmw, motifCheckMarkSpace );

	    int checkcol = maxpmw;

	    if ( mi && mi->isSeparator() ) {                    // draw separator
		p->setPen( cg.dark() );
		p->drawLine( x, y, x+w, y );
		p->setPen( cg.light() );
		p->drawLine( x, y+1, x+w, y+1 );
		return;
	    }

	    int pw = motifItemFrame;

	    if ( act && !dis ) {                        // active item frame
		if (pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags ) > 1)
		    qDrawShadePanel( p, x, y, w, h, cg, false, pw,
				     &cg.brush( TQColorGroup::Button ) );
		else
		    qDrawShadePanel( p, x+1, y+1, w-2, h-2, cg, true, 1,
				     &cg.brush( TQColorGroup::Button ) );
	    }
	    else                                // incognito frame
		p->fillRect(x, y, w, h, cg.brush( TQColorGroup::Button ));

	    if ( !mi )
		return;

	    TQRect vrect = visualRect( TQRect( x+motifItemFrame, y+motifItemFrame, checkcol, h-2*motifItemFrame ), r );
	    int xvis = vrect.x();
	    if ( mi->isChecked() ) {
		if ( mi->iconSet() ) {
		    drawPrimitive(PE_MenuItemIndicatorIconFrame, p, ceData, elementFlags, r, cg, flags, opt);
		}
	    } else if ( !act ) {
		p->fillRect(xvis, y+motifItemFrame, checkcol, h-2*motifItemFrame,
			    cg.brush( TQColorGroup::Button ));
	    }

	    if ( mi->iconSet() ) {              // draw iconset
		TQIconSet::Mode mode = TQIconSet::Normal; // no disabled icons in Motif
		if (act && !dis )
		    mode = TQIconSet::Active;
		TQPixmap pixmap;
		if ( checkable && mi->isChecked() )
		    pixmap = mi->iconSet()->pixmap( TQIconSet::Small, mode, TQIconSet::On );
		else
		    pixmap = mi->iconSet()->pixmap( TQIconSet::Small, mode );

		int pixw = pixmap.width();
		int pixh = pixmap.height();
		TQRect pmr( 0, 0, pixw, pixh );
		pmr.moveCenter( vrect.center() );
		p->setPen( cg.text() );
		p->drawPixmap( pmr.topLeft(), pixmap );

	    } else  if ( checkable ) {  // just "checking"...
		if ( mi->isChecked() ) {
		    drawPrimitive(PE_MenuItemIndicatorCheck, p, ceData, elementFlags, r, cg, flags, opt);
		}
	    }


	    p->setPen( cg.buttonText() );

	    TQColor discol;
	    if ( dis ) {
		discol = cg.text();
		p->setPen( discol );
	    }

	    int xm = motifItemFrame + checkcol + motifItemHMargin;

	    vrect = visualRect( TQRect( x+xm, y+motifItemVMargin, w-xm-tab, h-2*motifItemVMargin ), r );
	    xvis = vrect.x();
	    if ( mi->custom() ) {
		int m = motifItemVMargin;
		p->save();
		mi->custom()->paint( p, cg, act, !dis,
				     xvis, y+m, w-xm-tab+1, h-2*m );
		p->restore();
	    }
	    TQString s = mi->text();
	    if ( !s.isNull() ) {                        // draw text
		int t = s.find( '\t' );
		int m = motifItemVMargin;
		int text_flags = AlignVCenter|ShowPrefix | DontClip | SingleLine;
		text_flags |= (TQApplication::reverseLayout() ? AlignRight : AlignLeft );
		if ( t >= 0 ) {                         // draw tab text
		    TQRect vr = visualRect( TQRect( x+w-tab-motifItemHMargin-motifItemFrame,
						  y+motifItemVMargin, tab, h-2*motifItemVMargin ), r );
		    int xv = vr.x();
		    p->drawText( xv, y+m, tab, h-2*m, text_flags, s.mid( t+1 ) );
		    s = s.left( t );
		}
		p->drawText( xvis, y+m, w-xm-tab+1, h-2*m, text_flags, s, t );
	    } else if ( mi->pixmap() ) {                        // draw pixmap
		TQPixmap *pixmap = mi->pixmap();
		if ( pixmap->depth() == 1 )
		    p->setBackgroundMode( OpaqueMode );
		p->drawPixmap( xvis, y+motifItemFrame, *pixmap );
		if ( pixmap->depth() == 1 )
		    p->setBackgroundMode( TransparentMode );
	    }
	    if ( mi->popup() ) {                        // draw sub menu arrow
		int dim = (h-2*motifItemFrame) / 2;
		TQStyle::PrimitiveElement arrow = (TQApplication::reverseLayout() ? PE_ArrowLeft : PE_ArrowRight);
		TQRect vr = visualRect( TQRect(x+w - motifArrowHMargin - motifItemFrame - dim,
					y+h/2-dim/2, dim, dim), r );
		if ( act )
		    drawPrimitive(arrow, p, ceData, elementFlags, vr, cg,
				  (Style_Down |
				  (dis ? Style_Default : Style_Enabled)) );
		else
		    drawPrimitive(arrow, p, ceData, elementFlags, vr, cg,
				  (dis ? Style_Default : Style_Enabled));
	    }

	    break;
	}
#endif // TQT_NO_POPUPMENU

    case CE_MenuBarItem:
	{
	    if ( flags & Style_Active )  // active item
		qDrawShadePanel( p, r, cg, false, motifItemFrame,
				 &cg.brush(TQColorGroup::Button) );
	    else  // other item
		p->fillRect( r, cg.brush(TQColorGroup::Button) );
	    TQCommonStyle::drawControl( element, p, ceData, elementFlags, r, cg, flags, opt, widget );
	    break;
	}

    default:
	TQCommonStyle::drawControl( element, p, ceData, elementFlags, r, cg, flags, opt, widget );
	break;
    }
}

static int get_combo_extra_width( int h, int w, int *return_awh=0 )
{
    int awh,
	tmp;
    if ( h < 8 ) {
        awh = 6;
    } else if ( h < 14 ) {
        awh = h - 2;
    } else {
        awh = h/2;
    }
    tmp = (awh * 3) / 2;
    if ( tmp > w / 2 ) {
	awh = w / 2 - 3;
	tmp = w / 2 + 3;
    }

    if ( return_awh )
        *return_awh = awh;

    return tmp;
}

static void get_combo_parameters( const TQRect &r,
                                  int &ew, int &awh, int &ax,
                                  int &ay, int &sh, int &dh,
                                  int &sy )
{
    ew = get_combo_extra_width( r.height(), r.width(), &awh );

    sh = (awh+3)/4;
    if ( sh < 3 )
        sh = 3;
    dh = sh/2 + 1;

    ay = r.y() + (r.height()-awh-sh-dh)/2;
    if ( ay < 0 ) {
        //panic mode
        ay = 0;
        sy = r.height();
    } else {
        sy = ay+awh+dh;
    }
    ax = r.x() + r.width() - ew;
    ax  += (ew-awh)/2;
}

/*!\reimp
*/
void TQMotifStyle::drawComplexControl( ComplexControl control,
				     TQPainter *p,
				     const TQStyleControlElementData &ceData,
				     ControlElementFlags elementFlags,
				     const TQRect &r,
				     const TQColorGroup &cg,
				     SFlags flags,
				     SCFlags sub,
				     SCFlags subActive,
				     const TQStyleOption& opt,
				     const TQWidget *widget ) const
{
    switch ( control ) {
    case CC_SpinWidget: {
	SCFlags drawSub = SC_None;
	if ( sub & SC_SpinWidgetFrame )
	    qDrawShadePanel( p, r, cg, true,
			     pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags ) );

	if ( sub & SC_SpinWidgetUp || sub & SC_SpinWidgetDown ) {
	    if ( sub & SC_SpinWidgetUp )
		drawSub |= SC_SpinWidgetUp;
	    if ( sub & SC_SpinWidgetDown )
		drawSub |= SC_SpinWidgetDown;

	    TQCommonStyle::drawComplexControl( control, p, ceData, elementFlags, r, cg, flags,
					      drawSub, subActive, opt, widget );
	}
	break; }

    case CC_Slider:
	{
#ifndef TQT_NO_SLIDER
	    TQRect groove = querySubControlMetrics(CC_Slider, ceData, elementFlags, SC_SliderGroove,
						  opt, widget),
		  handle = querySubControlMetrics(CC_Slider, ceData, elementFlags, SC_SliderHandle,
						  opt, widget);

	    if ((sub & SC_SliderGroove) && groove.isValid()) {
		qDrawShadePanel( p, groove, cg, true, 2,
				 &cg.brush( TQColorGroup::Mid ) );


		if ( flags & Style_HasFocus ) {
		    TQRect fr = subRect( SR_SliderFocusRect, ceData, elementFlags, widget );
		    drawPrimitive( PE_FocusRect, p, ceData, elementFlags, fr, cg );
		}
	    }

	    if (( sub & SC_SliderHandle ) && handle.isValid()) {
		drawPrimitive( PE_ButtonBevel, p, ceData, elementFlags, handle, cg );

		if ( ceData.orientation == Horizontal ) {
		    TQCOORD mid = handle.x() + handle.width() / 2;
		    qDrawShadeLine( p, mid, handle.y(), mid,
				    handle.y() + handle.height() - 2,
				    cg, true, 1);
		} else {
		    TQCOORD mid = handle.y() + handle.height() / 2;
		    qDrawShadeLine( p, handle.x(), mid,
				    handle.x() + handle.width() - 2, mid,
				    cg, true, 1);
		}
	    }

	    if ( sub & SC_SliderTickmarks )
		TQCommonStyle::drawComplexControl( control, p, ceData, elementFlags, r, cg, flags,
						  SC_SliderTickmarks, subActive,
						  opt, widget );
#endif
	    break;
	}

    case CC_ComboBox:
#ifndef TQT_NO_COMBOBOX
	if ( sub & SC_ComboBoxArrow ) {
	    const TQComboBox * cb = (const TQComboBox *) widget;
	    int awh, ax, ay, sh, sy, dh, ew;
	    int fw = pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags, cb);

	    drawPrimitive( PE_ButtonCommand, p, ceData, elementFlags, r, cg, flags );
	    TQRect ar = TQStyle::visualRect( querySubControlMetrics( CC_ComboBox, ceData, elementFlags, SC_ComboBoxArrow,
								   opt, cb ), ceData, elementFlags );
	    drawPrimitive( PE_ArrowDown, p, ceData, elementFlags, ar, cg, flags | Style_Enabled );

	    TQRect tr = r;
	    tr.addCoords( fw, fw, -fw, -fw );
	    get_combo_parameters( tr, ew, awh, ax, ay, sh, dh, sy );

	    // draws the shaded line under the arrow
	    p->setPen( cg.light() );
	    p->drawLine( ar.x(), sy, ar.x()+awh-1, sy );
	    p->drawLine( ar.x(), sy, ar.x(), sy+sh-1 );
	    p->setPen( cg.dark() );
	    p->drawLine( ar.x()+1, sy+sh-1, ar.x()+awh-1, sy+sh-1 );
	    p->drawLine( ar.x()+awh-1, sy+1, ar.x()+awh-1, sy+sh-1 );

	    if ( cb->hasFocus() ) {
		TQRect re = TQStyle::visualRect( subRect( SR_ComboBoxFocusRect, ceData, elementFlags, cb ), ceData, elementFlags );
		drawPrimitive( PE_FocusRect, p, ceData, elementFlags, re, cg );
	    }
	}

	if ( sub & SC_ComboBoxEditField ) {
	    TQComboBox * cb = (TQComboBox *) widget;
	    if ( cb->editable() ) {
		TQRect er = TQStyle::visualRect( querySubControlMetrics( CC_ComboBox, ceData, elementFlags,
								       SC_ComboBoxEditField, cb ), ceData, elementFlags );
		er.addCoords( -1, -1, 1, 1);
		qDrawShadePanel( p, er, cg, true, 1,
				 &cg.brush( TQColorGroup::Button ));
	    }
	}
#endif
	p->setPen(cg.buttonText());
	break;

    case CC_ScrollBar:
	{
	    if (sub == (SC_ScrollBarAddLine | SC_ScrollBarSubLine | SC_ScrollBarAddPage |
			SC_ScrollBarSubPage | SC_ScrollBarFirst | SC_ScrollBarLast |
			SC_ScrollBarSlider))
		drawPrimitive(PE_PanelScrollBar, p, ceData, elementFlags, ceData.rect, cg,
			      ((ceData.orientation == TQt::Horizontal) ?
			      Style_Horizontal : Style_Default));

	    TQCommonStyle::drawComplexControl(control, p, ceData, elementFlags, r, cg, flags, sub,
					     subActive, opt, widget);
	    break;
	}

#ifndef TQT_NO_LISTVIEW
    case CC_ListView:
	{
	    if ( sub & SC_ListView ) {
		TQCommonStyle::drawComplexControl( control, p, ceData, elementFlags, r, cg, flags, sub, subActive, opt, widget );
	    }
	    if ( sub & ( SC_ListViewBranch | SC_ListViewExpand ) ) {
		if (opt.isDefault())
		    break;

		TQListViewItem *item = opt.listViewItem();
		TQListViewItem *child = item->firstChild();

		int y = r.y();
		int c;
		TQPointArray dotlines;
		if ( subActive == (uint)SC_All && sub == SC_ListViewExpand ) {
		    c = 2;
		    dotlines.resize(2);
		    dotlines[0] = TQPoint( r.right(), r.top() );
		    dotlines[1] = TQPoint( r.right(), r.bottom() );
		} else {
		    int linetop = 0, linebot = 0;
		    // each branch needs at most two lines, ie. four end points
		    dotlines.resize( item->childCount() * 4 );
		    c = 0;

		    // skip the stuff above the exposed rectangle
		    while ( child && y + child->height() <= 0 ) {
			y += child->totalHeight();
			child = child->nextSibling();
		    }

		    int bx = r.width() / 2;

		    // paint stuff in the magical area
		    TQListView* v = item->listView();
		    while ( child && y < r.height() ) {
			if (child->isVisible()) {
			    int lh;
			    if ( !item->multiLinesEnabled() )
				lh = child->height();
			    else
				lh = p->fontMetrics().height() + 2 * v->itemMargin();
			    lh = TQMAX( lh, TQApplication::globalStrut().height() );
			    if ( lh % 2 > 0 )
				lh++;
			    linebot = y + lh/2;
			    if ( (child->isExpandable() || child->childCount()) &&
				 (child->height() > 0) ) {
				// needs a box
				p->setPen( cg.text() );
				p->drawRect( bx-4, linebot-4, 9, 9 );
				TQPointArray a;
				if ( child->isOpen() )
				    a.setPoints( 3, bx-2, linebot-2,
						 bx, linebot+2,
						 bx+2, linebot-2 ); //RightArrow
				else
				    a.setPoints( 3, bx-2, linebot-2,
						 bx+2, linebot,
						 bx-2, linebot+2 ); //DownArrow
				p->setBrush( cg.text() );
				p->drawPolygon( a );
				p->setBrush( NoBrush );
				// dotlinery
				dotlines[c++] = TQPoint( bx, linetop );
				dotlines[c++] = TQPoint( bx, linebot - 5 );
				dotlines[c++] = TQPoint( bx + 5, linebot );
				dotlines[c++] = TQPoint( r.width(), linebot );
				linetop = linebot + 5;
			    } else {
				// just dotlinery
				dotlines[c++] = TQPoint( bx+1, linebot );
				dotlines[c++] = TQPoint( r.width(), linebot );
			    }
			    y += child->totalHeight();
			}
			child = child->nextSibling();
		    }

		    // Expand line height to edge of rectangle if there's any
		    // visible child below
		    while ( child && child->height() <= 0)
			child = child->nextSibling();
		    if ( child )
			linebot = r.height();

		    if ( linetop < linebot ) {
			dotlines[c++] = TQPoint( bx, linetop );
			dotlines[c++] = TQPoint( bx, linebot );
		    }
		}

		int line; // index into dotlines
		p->setPen( cg.text() );
		if ( sub & SC_ListViewBranch ) for( line = 0; line < c; line += 2 ) {
		    p->drawLine( dotlines[line].x(), dotlines[line].y(),
				 dotlines[line+1].x(), dotlines[line+1].y() );
		}
	    }

	    break;
	}
#endif // TQT_NO_LISTVIEW

    default:
	TQCommonStyle::drawComplexControl( control, p, ceData, elementFlags, r, cg, flags,
					  sub, subActive, opt, widget );
    }
}


/*! \reimp */
int TQMotifStyle::pixelMetric( PixelMetric metric, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *widget ) const
{
     int ret;

    switch( metric ) {
    case PM_ButtonDefaultIndicator:
	ret = 3;
	break;

    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
	ret = 0;
	break;

    case PM_SplitterWidth:
	ret = TQMAX( 10, TQApplication::globalStrut().width() );
	break;

    case PM_SliderLength:
	ret = 30;
	break;

    case PM_SliderThickness:
	ret = 24;
	break;

    case PM_SliderControlThickness:
	{
#ifndef TQT_NO_SLIDER
	    const TQSlider * sl = (const TQSlider *) widget;
	    int space = (sl->orientation() == Horizontal) ? sl->height()
			: sl->width();
	    int ticks = sl->tickmarks();
	    int n = 0;
	    if ( ticks & TQSlider::Above ) n++;
	    if ( ticks & TQSlider::Below ) n++;
	    if ( !n ) {
		ret = space;
		break;
	    }

	    int thick = 6;	// Magic constant to get 5 + 16 + 5

	    space -= thick;
	    //### the two sides may be unequal in size
	    if ( space > 0 )
		thick += (space * 2) / (n + 2);
	    ret = thick;
#endif
	    break;
	}

    case PM_SliderSpaceAvailable:
	{
#ifndef TQT_NO_SLIDER
	    const TQSlider * sl = (const TQSlider *) widget;
	    if ( sl->orientation() == Horizontal )
		ret = sl->width() - pixelMetric( PM_SliderLength, ceData, elementFlags, sl ) - 6;
	    else
		ret = sl->height() - pixelMetric( PM_SliderLength, ceData, elementFlags, sl ) - 6;
#endif
	    break;
	}

    case PM_DockWindowHandleExtent:
	ret = 9;
	break;

    case PM_ProgressBarChunkWidth:
	ret = 1;
	break;

    case PM_ExclusiveIndicatorWidth:
    case PM_ExclusiveIndicatorHeight:
	ret = 13;
	break;

    case PM_MenuIndicatorFrameHBorder:
    case PM_MenuIndicatorFrameVBorder:
    case PM_MenuIconIndicatorFrameHBorder:
    case PM_MenuIconIndicatorFrameVBorder:
	ret = motifItemFrame;
	break;

    default:
	ret =  TQCommonStyle::pixelMetric( metric, ceData, elementFlags, widget );
	break;
    }
    return ret;
}


/*!\reimp
*/
TQRect TQMotifStyle::querySubControlMetrics( ComplexControl control,
					   const TQStyleControlElementData &ceData,
					   ControlElementFlags elementFlags,
					   SubControl sc,
					   const TQStyleOption& opt,
					   const TQWidget *widget ) const
{
    switch ( control ) {
    case CC_SpinWidget: {
	if ( elementFlags & CEF_UseGenericParameters )
	    return TQRect();
	int fw = pixelMetric( PM_SpinBoxFrameWidth, ceData, elementFlags, 0 );
	TQSize bs;
	bs.setHeight( ceData.rect.height()/2 );
	if ( bs.height() < 8 )
	    bs.setHeight( 8 );
	bs.setWidth( TQMIN( bs.height() * 8 / 5, ceData.rect.width() / 4 ) ); // 1.6 -approximate golden mean
	bs = bs.expandedTo( TQApplication::globalStrut() );
	int y = 0;
	int x, lx, rx;
	x = ceData.rect.width() - y - bs.width();
	lx = fw;
	rx = x - fw * 2;
	switch ( sc ) {
	case SC_SpinWidgetUp:
	    return TQRect(x, y, bs.width(), bs.height());
	case SC_SpinWidgetDown:
	    return TQRect(x, y + bs.height(), bs.width(), bs.height());
	case SC_SpinWidgetButtonField:
	    return TQRect(x, y, bs.width(), ceData.rect.height() - 2*fw);
	case SC_SpinWidgetEditField:
	    return TQRect(lx, fw, rx, ceData.rect.height() - 2*fw);
	case SC_SpinWidgetFrame:
	    return TQRect( 0, 0,
			  ceData.rect.width() - bs.width(), ceData.rect.height() );
	default:
	    break;
	}
	break; }

#ifndef TQT_NO_SLIDER
    case CC_Slider: {
	if (sc == SC_SliderHandle) {
	    int tickOffset  = pixelMetric( PM_SliderTickmarkOffset, ceData, elementFlags, widget );
	    int thickness   = pixelMetric( PM_SliderControlThickness, ceData, elementFlags, widget );
	    int sliderPos   = ceData.startStep;
	    int len         = pixelMetric( PM_SliderLength, ceData, elementFlags, widget );
	    int motifBorder = 3;

	    if ( ceData.orientation == Horizontal )
		return TQRect( sliderPos + motifBorder, tickOffset + motifBorder, len,
			      thickness - 2*motifBorder );
	    return TQRect( tickOffset + motifBorder, sliderPos + motifBorder,
			  thickness - 2*motifBorder, len );
	}
	break; }
#endif

#ifndef TQT_NO_SCROLLBAR
    case CC_ScrollBar: {
	if (elementFlags & CEF_UseGenericParameters)
	    return TQRect();

	int sliderstart = ceData.startStep;
	int sbextent = pixelMetric(PM_ScrollBarExtent, ceData, elementFlags, widget);
	int fw = pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags, widget);
	int buttonw = sbextent - (fw * 2);
	int buttonh = sbextent - (fw * 2);
	int maxlen = ((ceData.orientation == TQt::Horizontal) ?
		      ceData.rect.width() : ceData.rect.height()) -
		     (buttonw * 2) - (fw * 2);
	int sliderlen;

	// calculate slider length
	if (ceData.maxSteps != ceData.minSteps) {
	    uint range = ceData.maxSteps - ceData.minSteps;
	    sliderlen = (ceData.pageStep * maxlen) /
			(range + ceData.pageStep);

	    if ( sliderlen < 9 || range > INT_MAX/2 )
		sliderlen = 9;
	    if ( sliderlen > maxlen )
		sliderlen = maxlen;
	} else
	    sliderlen = maxlen;

	switch (sc) {
	case SC_ScrollBarSubLine:
	    // top/left button
	    if (ceData.orientation == TQt::Horizontal) {
		if ( ceData.rect.width()/2 < sbextent )
		    buttonw = ceData.rect.width()/2 - (fw*2);
		return TQRect(fw, fw, buttonw, buttonh);
	    } else {
		if ( ceData.rect.height()/2 < sbextent )
		    buttonh = ceData.rect.height()/2 - (fw*2);
		return TQRect(fw, fw, buttonw, buttonh);
	    }
	case SC_ScrollBarAddLine:
	    // bottom/right button
	    if (ceData.orientation == TQt::Horizontal) {
		if ( ceData.rect.width()/2 < sbextent )
		    buttonw = ceData.rect.width()/2 - (fw*2);
		return TQRect(ceData.rect.width() - buttonw - fw, fw,
			     buttonw, buttonh);
	    } else {
		if ( ceData.rect.height()/2 < sbextent )
		    buttonh = ceData.rect.height()/2 - (fw*2);
		return TQRect(fw, ceData.rect.height() - buttonh - fw,
			     buttonw, buttonh);
	    }
	case SC_ScrollBarSubPage:
	    if (ceData.orientation == TQt::Horizontal)
		return TQRect(buttonw + fw, fw, sliderstart - buttonw - fw, buttonw);
	    return TQRect(fw, buttonw + fw, buttonw, sliderstart - buttonw - fw);

	case SC_ScrollBarAddPage:
	    if (ceData.orientation == TQt::Horizontal)
		return TQRect(sliderstart + sliderlen, fw,
			     maxlen - sliderstart - sliderlen + buttonw + fw, buttonw);
	    return TQRect(fw, sliderstart + sliderlen, buttonw,
			 maxlen - sliderstart - sliderlen + buttonw + fw);

	case SC_ScrollBarGroove:
	    if (ceData.orientation == TQt::Horizontal)
		return TQRect(buttonw + fw, fw, maxlen, buttonw);
	    return TQRect(fw, buttonw + fw, buttonw, maxlen);

	case SC_ScrollBarSlider:
	    if (ceData.orientation == TQt::Horizontal)
		return TQRect(sliderstart, fw, sliderlen, buttonw);
	    return TQRect(fw, sliderstart, buttonw, sliderlen);

	default:
	    break;
	}
	break; }
#endif

#ifndef TQT_NO_COMBOBOX
    case CC_ComboBox:

	switch ( sc ) {
	case SC_ComboBoxArrow: {
	    int ew, awh, sh, dh, ax, ay, sy;
	    int fw = pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags, widget );
	    TQRect cr = ceData.rect;
	    cr.addCoords( fw, fw, -fw, -fw );
	    get_combo_parameters( cr, ew, awh, ax, ay, sh, dh, sy );
	    return TQRect( ax, ay, awh, awh ); }

	case SC_ComboBoxEditField: {
	    int fw = pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags, widget );
	    TQRect rect = ceData.rect;
	    rect.addCoords( fw, fw, -fw, -fw );
	    int ew = get_combo_extra_width( rect.height(), rect.width() );
	    rect.addCoords( 1, 1, -1-ew, -1 );
	    return rect; }

	default:
	    break;
	}
	break;
#endif
    default: break;
    }
    return TQCommonStyle::querySubControlMetrics( control, ceData, elementFlags, sc, opt, widget );
}

/*!\reimp
*/
TQSize TQMotifStyle::sizeFromContents( ContentsType contents,
				     const TQStyleControlElementData &ceData,
				     ControlElementFlags elementFlags,
				     const TQSize &contentsSize,
				     const TQStyleOption& opt,
				     const TQWidget *widget ) const
{
    TQSize sz(contentsSize);

    switch(contents) {
    case CT_PushButton:
	{
#ifndef TQT_NO_PUSHBUTTON
	    sz = TQCommonStyle::sizeFromContents(contents, ceData, elementFlags, contentsSize, opt, widget);
	    if (((elementFlags & CEF_IsDefault) || (elementFlags & CEF_AutoDefault)) &&
		sz.width() < 80 && ceData.fgPixmap.isNull())
		sz.setWidth(80);
#endif
	    break;
	}

    case CT_PopupMenuItem:
	{
#ifndef TQT_NO_POPUPMENU
	    if ((elementFlags & CEF_UseGenericParameters) || opt.isDefault())
		break;

	    bool checkable = (elementFlags & CEF_IsCheckable);
	    TQMenuItem *mi = opt.menuItem();
	    int maxpmw = opt.maxIconWidth();
	    int w = sz.width(), h = sz.height();

	    if (mi->custom()) {
		w = mi->custom()->sizeHint().width();
		h = mi->custom()->sizeHint().height();
		if (! mi->custom()->fullSpan())
		    h += 2*motifItemVMargin + 2*motifItemFrame;
	    } else if ( mi->widget() ) {
	    } else if ( mi->isSeparator() ) {
		w = 10;
		h = motifSepHeight;
	    } else if (mi->pixmap() || ! mi->text().isNull())
		h += 2*motifItemVMargin + 2*motifItemFrame;

	    // a little bit of border can never harm
	    w += 2*motifItemHMargin + 2*motifItemFrame;

	    if ( !mi->text().isNull() && mi->text().find('\t') >= 0 )
		// string contains tab
		w += motifTabSpacing;
	    else if (mi->popup())
		// submenu indicator needs some room if we don't have a tab column
		w += motifArrowHMargin + 4*motifItemFrame;

	    if ( checkable && maxpmw <= 0)
		// if we are checkable and have no iconsets, add space for a checkmark
		w += motifCheckMarkSpace;
	    else if (checkable && maxpmw < motifCheckMarkSpace)
		// make sure the check-column is wide enough if we have iconsets
		w += (motifCheckMarkSpace - maxpmw);

	    // if we have a check-column ( iconsets of checkmarks), add space
	    // to separate the columns
	    if ( maxpmw > 0 || checkable )
		w += motifCheckMarkHMargin;

	    sz = TQSize(w, h);
#endif
	    break;
	}

    default:
	sz = TQCommonStyle::sizeFromContents( contents, ceData, elementFlags, contentsSize, opt, widget );
	break;
    }

    return sz;
}

/*!\reimp
*/
TQRect TQMotifStyle::subRect( SubRect r, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *widget ) const
{
    TQRect rect;
    TQRect wrect = ceData.rect;

    switch ( r ) {
    case SR_SliderFocusRect:
	rect = TQCommonStyle::subRect( r, ceData, elementFlags, widget );
	rect.addCoords( 2, 2, -2, -2 );
	break;

    case SR_ComboBoxFocusRect:
	{
	    int awh, ax, ay, sh, sy, dh, ew;
	    int fw = pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags, widget );
	    TQRect tr = wrect;

	    tr.addCoords( fw, fw, -fw, -fw );
	    get_combo_parameters( tr, ew, awh, ax, ay, sh, dh, sy );
	    rect.setRect(ax-2, ay-2, awh+4, awh+sh+dh+4);
	    break;
	}

    case SR_DockWindowHandleRect:
	{
#ifndef TQT_NO_MAINWINDOW
	    if ( (elementFlags & CEF_UseGenericParameters) || !(elementFlags & CEF_HasParentWidget) )
		break;

	    if ( !ceData.dwData.hasDockArea || !ceData.dwData.closeEnabled )
		rect.setRect( 0, 0, ceData.rect.width(), ceData.rect.height() );
	    else {
		if ( ceData.dwData.areaOrientation == Horizontal )
		    rect.setRect(2, 15, ceData.rect.width()-2, ceData.rect.height() - 15);
		else
		    rect.setRect(0, 2, ceData.rect.width() - 15, ceData.rect.height() - 2);
	    }
#endif
	    break;
	}

    case SR_ProgressBarGroove:
    case SR_ProgressBarContents:
	{
#ifndef TQT_NO_PROGRESSBAR
	    TQFontMetrics fm( ( (!(elementFlags & CEF_UseGenericParameters)) ? TQFontMetrics(ceData.font) :
			       TQApplication::fontMetrics() ) );
	    int textw = 0;
	    if (ceData.percentageVisible)
		textw = fm.width("100%") + 6;

	    if ((elementFlags & CEF_IndicatorFollowsStyle) ||
		(elementFlags & CEF_CenterIndicator))
		rect = wrect;
	    else
		rect.setCoords(wrect.left(), wrect.top(),
			       wrect.right() - textw, wrect.bottom());
#endif
	    break;
	}

    case SR_ProgressBarLabel:
	{
#ifndef TQT_NO_PROGRESSBAR
	    TQFontMetrics fm( ( (!(elementFlags & CEF_UseGenericParameters)) ? TQFontMetrics(ceData.font) :
			       TQApplication::fontMetrics() ) );
	    int textw = 0;
	    if (ceData.percentageVisible)
		textw = fm.width("100%") + 6;

	    if ((elementFlags & CEF_IndicatorFollowsStyle) ||
		(elementFlags & CEF_CenterIndicator))
		rect = wrect;
	    else
		rect.setCoords(wrect.right() - textw, wrect.top(),
			       wrect.right(), wrect.bottom());
#endif
	    break;
	}

    case SR_CheckBoxContents:
	{
#ifndef TQT_NO_CHECKBOX
	    TQRect ir = subRect(SR_CheckBoxIndicator, ceData, elementFlags, widget);
	    rect.setRect(ir.right() + 10, wrect.y(),
			 wrect.width() - ir.width() - 10, wrect.height());
#endif
	    break;
	}

    case SR_RadioButtonContents:
	{
	    TQRect ir = subRect(SR_RadioButtonIndicator, ceData, elementFlags, widget);
	    rect.setRect(ir.right() + 10, wrect.y(),
			 wrect.width() - ir.width() - 10, wrect.height());
	    break;
	}

    default:
	rect = TQCommonStyle::subRect( r, ceData, elementFlags, widget );
    }

    return rect;
}

/*! \reimp
*/
void TQMotifStyle::polishPopupMenu( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr )
{
#ifndef TQT_NO_POPUPMENU
    if ( !(ceData.windowState & WState_Polished ) ) {
        widgetActionRequest(ceData, elementFlags, ptr, WAR_SetCheckable);
    }
#endif
}


#ifndef TQT_NO_IMAGEIO_XPM
static const char * const qt_close_xpm[] = {
"12 12 2 1",
"       s None  c None",
".      c black",
"            ",
"            ",
"   .    .   ",
"  ...  ...  ",
"   ......   ",
"    ....    ",
"    ....    ",
"   ......   ",
"  ...  ...  ",
"   .    .   ",
"            ",
"            "};

static const char * const qt_maximize_xpm[] = {
"12 12 2 1",
"       s None  c None",
".      c black",
"            ",
"            ",
"            ",
"     .      ",
"    ...     ",
"   .....    ",
"  .......   ",
" .........  ",
"            ",
"            ",
"            ",
"            "};

static const char * const qt_minimize_xpm[] = {
"12 12 2 1",
"       s None  c None",
".      c black",
"            ",
"            ",
"            ",
"            ",
" .........  ",
"  .......   ",
"   .....    ",
"    ...     ",
"     .      ",
"            ",
"            ",
"            "};

#if 0 // ### not used???
static const char * const qt_normalize_xpm[] = {
"12 12 2 1",
"       s None  c None",
".      c black",
"            ",
"            ",
"  .         ",
"  ..        ",
"  ...       ",
"  ....      ",
"  .....     ",
"  ......    ",
"  .......   ",
"            ",
"            ",
"            "};
#endif

static const char * const qt_normalizeup_xpm[] = {
"12 12 2 1",
"       s None  c None",
".      c black",
"            ",
"            ",
"            ",
"  .......   ",
"   ......   ",
"    .....   ",
"     ....   ",
"      ...   ",
"       ..   ",
"        .   ",
"            ",
"            "};

static const char * const qt_shade_xpm[] = {
"12 12 2 1", "# c #000000",
". c None",
"............",
"............",
".#########..",
".#########..",
"............",
"............",
"............",
"............",
"............",
"............",
"............",
"............"};


static const char * const qt_unshade_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"............",
".#########..",
".#########..",
".#.......#..",
".#.......#..",
".#.......#..",
".#.......#..",
".#.......#..",
".#########..",
"............",
"............"};


static const char * dock_window_close_xpm[] = {
"8 8 2 1",
"# c #000000",
". c None",
"##....##",
".##..##.",
"..####..",
"...##...",
"..####..",
".##..##.",
"##....##",
"........"};

// Message box icons, from page 210 of the Windows style guide.

// Hand-drawn to resemble Microsoft's icons, but in the Mac/Netscape palette.
// Thanks to TrueColor displays, it is slightly more efficient to have
// them duplicated.
/* XPM */
static const char * const information_xpm[]={
"32 32 5 1",
". c None",
"c c #000000",
"* c #999999",
"a c #ffffff",
"b c #0000ff",
"...........********.............",
"........***aaaaaaaa***..........",
"......**aaaaaaaaaaaaaa**........",
".....*aaaaaaaaaaaaaaaaaa*.......",
"....*aaaaaaaabbbbaaaaaaaac......",
"...*aaaaaaaabbbbbbaaaaaaaac.....",
"..*aaaaaaaaabbbbbbaaaaaaaaac....",
".*aaaaaaaaaaabbbbaaaaaaaaaaac...",
".*aaaaaaaaaaaaaaaaaaaaaaaaaac*..",
"*aaaaaaaaaaaaaaaaaaaaaaaaaaaac*.",
"*aaaaaaaaaabbbbbbbaaaaaaaaaaac*.",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaabbbbbaaaaaaaaaaac**",
".*aaaaaaaaaaabbbbbaaaaaaaaaac***",
".*aaaaaaaaaaabbbbbaaaaaaaaaac***",
"..*aaaaaaaaaabbbbbaaaaaaaaac***.",
"...caaaaaaabbbbbbbbbaaaaaac****.",
"....caaaaaaaaaaaaaaaaaaaac****..",
".....caaaaaaaaaaaaaaaaaac****...",
"......ccaaaaaaaaaaaaaacc****....",
".......*cccaaaaaaaaccc*****.....",
"........***cccaaaac*******......",
"..........****caaac*****........",
".............*caaac**...........",
"...............caac**...........",
"................cac**...........",
".................cc**...........",
"..................***...........",
"...................**..........."};
/* XPM */
static const char* const warning_xpm[]={
"32 32 4 1",
". c None",
"a c #ffff00",
"* c #000000",
"b c #999999",
".............***................",
"............*aaa*...............",
"...........*aaaaa*b.............",
"...........*aaaaa*bb............",
"..........*aaaaaaa*bb...........",
"..........*aaaaaaa*bb...........",
".........*aaaaaaaaa*bb..........",
".........*aaaaaaaaa*bb..........",
"........*aaaaaaaaaaa*bb.........",
"........*aaaa***aaaa*bb.........",
".......*aaaa*****aaaa*bb........",
".......*aaaa*****aaaa*bb........",
"......*aaaaa*****aaaaa*bb.......",
"......*aaaaa*****aaaaa*bb.......",
".....*aaaaaa*****aaaaaa*bb......",
".....*aaaaaa*****aaaaaa*bb......",
"....*aaaaaaaa***aaaaaaaa*bb.....",
"....*aaaaaaaa***aaaaaaaa*bb.....",
"...*aaaaaaaaa***aaaaaaaaa*bb....",
"...*aaaaaaaaaa*aaaaaaaaaa*bb....",
"..*aaaaaaaaaaa*aaaaaaaaaaa*bb...",
"..*aaaaaaaaaaaaaaaaaaaaaaa*bb...",
".*aaaaaaaaaaaa**aaaaaaaaaaa*bb..",
".*aaaaaaaaaaa****aaaaaaaaaa*bb..",
"*aaaaaaaaaaaa****aaaaaaaaaaa*bb.",
"*aaaaaaaaaaaaa**aaaaaaaaaaaa*bb.",
"*aaaaaaaaaaaaaaaaaaaaaaaaaaa*bbb",
"*aaaaaaaaaaaaaaaaaaaaaaaaaaa*bbb",
".*aaaaaaaaaaaaaaaaaaaaaaaaa*bbbb",
"..*************************bbbbb",
"....bbbbbbbbbbbbbbbbbbbbbbbbbbb.",
".....bbbbbbbbbbbbbbbbbbbbbbbbb.."};
/* XPM */
static const char* const critical_xpm[]={
"32 32 4 1",
". c None",
"a c #999999",
"* c #ff0000",
"b c #ffffff",
"...........********.............",
".........************...........",
".......****************.........",
"......******************........",
".....********************a......",
"....**********************a.....",
"...************************a....",
"..*******b**********b*******a...",
"..******bbb********bbb******a...",
".******bbbbb******bbbbb******a..",
".*******bbbbb****bbbbb*******a..",
"*********bbbbb**bbbbb*********a.",
"**********bbbbbbbbbb**********a.",
"***********bbbbbbbb***********aa",
"************bbbbbb************aa",
"************bbbbbb************aa",
"***********bbbbbbbb***********aa",
"**********bbbbbbbbbb**********aa",
"*********bbbbb**bbbbb*********aa",
".*******bbbbb****bbbbb*******aa.",
".******bbbbb******bbbbb******aa.",
"..******bbb********bbb******aaa.",
"..*******b**********b*******aa..",
"...************************aaa..",
"....**********************aaa...",
"....a********************aaa....",
".....a******************aaa.....",
"......a****************aaa......",
".......aa************aaaa.......",
".........aa********aaaaa........",
"...........aaaaaaaaaaa..........",
".............aaaaaaa............"};
/* XPM */
static const char *const question_xpm[] = {
"32 32 5 1",
". c None",
"c c #000000",
"* c #999999",
"a c #ffffff",
"b c #0000ff",
"...........********.............",
"........***aaaaaaaa***..........",
"......**aaaaaaaaaaaaaa**........",
".....*aaaaaaaaaaaaaaaaaa*.......",
"....*aaaaaaaaaaaaaaaaaaaac......",
"...*aaaaaaaabbbbbbaaaaaaaac.....",
"..*aaaaaaaabaaabbbbaaaaaaaac....",
".*aaaaaaaabbaaaabbbbaaaaaaaac...",
".*aaaaaaaabbbbaabbbbaaaaaaaac*..",
"*aaaaaaaaabbbbaabbbbaaaaaaaaac*.",
"*aaaaaaaaaabbaabbbbaaaaaaaaaac*.",
"*aaaaaaaaaaaaabbbbaaaaaaaaaaac**",
"*aaaaaaaaaaaaabbbaaaaaaaaaaaac**",
"*aaaaaaaaaaaaabbaaaaaaaaaaaaac**",
"*aaaaaaaaaaaaabbaaaaaaaaaaaaac**",
"*aaaaaaaaaaaaaaaaaaaaaaaaaaaac**",
".*aaaaaaaaaaaabbaaaaaaaaaaaac***",
".*aaaaaaaaaaabbbbaaaaaaaaaaac***",
"..*aaaaaaaaaabbbbaaaaaaaaaac***.",
"...caaaaaaaaaabbaaaaaaaaaac****.",
"....caaaaaaaaaaaaaaaaaaaac****..",
".....caaaaaaaaaaaaaaaaaac****...",
"......ccaaaaaaaaaaaaaacc****....",
".......*cccaaaaaaaaccc*****.....",
"........***cccaaaac*******......",
"..........****caaac*****........",
".............*caaac**...........",
"...............caac**...........",
"................cac**...........",
".................cc**...........",
"..................***...........",
"...................**...........",
};
#endif

/*!
 \reimp
 */
TQPixmap TQMotifStyle::stylePixmap(StylePixmap sp,
				 const TQStyleControlElementData &ceData,
				 ControlElementFlags elementFlags,
				 const TQStyleOption& opt,
				 const TQWidget *widget) const
{
#ifndef TQT_NO_IMAGEIO_XPM
    switch (sp) {
    case SP_TitleBarShadeButton:
	return TQPixmap((const char **)qt_shade_xpm);
    case SP_TitleBarUnshadeButton:
	return TQPixmap((const char **)qt_unshade_xpm);
    case SP_TitleBarNormalButton:
	return TQPixmap((const char **)qt_normalizeup_xpm);
    case SP_TitleBarMinButton:
	return TQPixmap((const char **)qt_minimize_xpm);
    case SP_TitleBarMaxButton:
	return TQPixmap((const char **)qt_maximize_xpm);
    case SP_TitleBarCloseButton:
	return TQPixmap((const char **)qt_close_xpm);
    case SP_DockWindowCloseButton:
	return TQPixmap((const char **)dock_window_close_xpm );

    case SP_MessageBoxInformation:
    case SP_MessageBoxWarning:
    case SP_MessageBoxCritical:
    case SP_MessageBoxQuestion:
	{
	    const char * const * xpm_data;
	    switch ( sp ) {
	    case SP_MessageBoxInformation:
		xpm_data = information_xpm;
		break;
	    case SP_MessageBoxWarning:
		xpm_data = warning_xpm;
		break;
	    case SP_MessageBoxCritical:
		xpm_data = critical_xpm;
		break;
	    case SP_MessageBoxQuestion:
		xpm_data = question_xpm;
		break;
	    default:
		xpm_data = 0;
		break;
	    }
	    TQPixmap pm;
	    if ( xpm_data ) {
		TQImage image( (const char **) xpm_data);
		// All that color looks ugly in Motif
		TQColorGroup g = TQApplication::palette().active();
		switch ( sp ) {
		case SP_MessageBoxInformation:
		case SP_MessageBoxQuestion:
		    image.setColor( 2, 0xff000000 | g.dark().rgb() );
		    image.setColor( 3, 0xff000000 | g.base().rgb() );
		    image.setColor( 4, 0xff000000 | g.text().rgb() );
		    break;
		case SP_MessageBoxWarning:
		    image.setColor( 1, 0xff000000 | g.base().rgb() );
		    image.setColor( 2, 0xff000000 | g.text().rgb() );
		    image.setColor( 3, 0xff000000 | g.dark().rgb() );
		    break;
		case SP_MessageBoxCritical:
		    image.setColor( 1, 0xff000000 | g.dark().rgb() );
		    image.setColor( 2, 0xff000000 | g.text().rgb() );
		    image.setColor( 3, 0xff000000 | g.base().rgb() );
		    break;
		default:
		    break;
		}
		pm.convertFromImage(image);
	    }
	    return pm;
	}

    default:
	break;
    }
#endif

    return TQCommonStyle::stylePixmap(sp, ceData, elementFlags, opt, widget);
}


/*! \reimp */
int TQMotifStyle::styleHint(StyleHint hint,
			   const TQStyleControlElementData &ceData,
			   ControlElementFlags elementFlags,
			   const TQStyleOption &opt,
			   TQStyleHintReturn *returnData,
			   const TQWidget *widget) const
{
    int ret;

    switch (hint) {
    case SH_GUIStyle:
	ret = MotifStyle;
	break;

    case SH_ScrollBar_BackgroundMode:
	ret = TQWidget::PaletteMid;
	break;

    case SH_ScrollBar_MiddleClickAbsolutePosition:
    case SH_Slider_SloppyKeyEvents:
    case SH_ProgressDialog_CenterCancelButton:
    case SH_PopupMenu_SpaceActivatesItem:
    case SH_ScrollView_FrameOnlyAroundContents:
	ret = 1;
	break;

    case SH_PopupMenu_SubMenuPopupDelay:
	ret = 96;
	break;

    case SH_ProgressDialog_TextLabelAlignment:
	ret = AlignAuto | AlignVCenter;
	break;

    case SH_ItemView_ChangeHighlightOnFocus:
	ret = 0;
	break;

    case SH_MenuIndicatorColumnWidth:
	{
	    int maxpmw = opt.maxIconWidth();
	    bool checkable = (elementFlags & CEF_IsCheckable);

	    if ( checkable )
		maxpmw = TQMAX( maxpmw, motifCheckMarkSpace );

	    ret = maxpmw;
	    break;
	}

    default:
	ret = TQCommonStyle::styleHint(hint, ceData, elementFlags, opt, returnData, widget);
	break;
    }

    return ret;
}


#endif
