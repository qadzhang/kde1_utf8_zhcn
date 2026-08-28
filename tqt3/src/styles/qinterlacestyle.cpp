/****************************************************************************
**
** Implementation of TQInterlaceStyle class
**
** Created : 010122
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
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

#if 0 // ###### not ported to new API yet

#include "ntqinterlacestyle.h"

#if !defined(TQT_NO_STYLE_INTERLACE) || defined(QT_PLUGIN)

#include "ntqapplication.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h" // for now
#include "ntqpalette.h" // for now
#include "ntqwidget.h"
#include "ntqlabel.h"
#include "ntqpushbutton.h"
#include "ntqwidget.h"
#include "ntqrangecontrol.h"
#include "ntqscrollbar.h"
#include "ntqlistbox.h"

#include <limits.h>

/*!
  \class TQInterlaceStyle ntqinterlacestyle.h
  \brief The TQInterlaceStyle class provides a Look and Feel suitable for interlaced displays.
  \ingroup appearance

  This class implements a look and feel that reduces flicker as much as
  possible on interlaced displays (i.e. television).  It is an experimental
  style.  In addition to using this style you will need to select a font
  that does not flicker.
*/

/*!
  Constructs a TQInterlaceStyle
*/
TQInterlaceStyle::TQInterlaceStyle() : TQMotifStyle()
{
    setUseHighlightColors( true );
}

/*! \reimp
*/
int TQInterlaceStyle::buttonDefaultIndicatorWidth() const
{
    return 0;
}

/*! \reimp
*/
int TQInterlaceStyle::setSliderThickness() const
{
    return 18;
}

/*! \reimp
*/
TQSize TQInterlaceStyle::scrollBarExtent() const
{
    return TQSize( 18, 18 );
}

/*! \reimp
*/
int TQInterlaceStyle::defaultFrameWidth() const
{
    return 2;
}

/*!
  \reimp
 */
void TQInterlaceStyle::polish( TQApplication *app)
{
    oldPalette = app->palette();
#if 0
    TQColor bg( 128, 64, 128 );
    TQColor btn( 255, 145, 0 );
    TQColor mid = bg.dark( 120 );
    TQColor low = mid.dark( 120 );
    TQColor fg( white );
#else
    TQColor bg( 224, 224, 224 );
    TQColor btn = bg.dark( 105 );
    TQColor mid = bg.dark( 120 );
    TQColor low = mid.dark( 120 );
    TQColor fg( black );
#endif

    TQColorGroup cg( fg, btn, low, low, mid, black, black, white, bg );
    cg.setColor( TQColorGroup::Highlight, TQColor( 255, 255, 192 ) );
    cg.setColor( TQColorGroup::HighlightedText, black );

    TQColorGroup dcg( cg );
    dcg.setColor( TQColorGroup::ButtonText, low );
    dcg.setColor( TQColorGroup::Text, low );

    app->setPalette( TQPalette( cg, dcg, cg ), true );
}

/*!
  \reimp
 */
void TQInterlaceStyle::unPolish( TQApplication *app)
{
    app->setPalette(oldPalette, true);
}

/*!
  \reimp
 */
void TQInterlaceStyle::polish( TQWidget* w)
{

   // the polish function sets some widgets to transparent mode and
    // some to translate background mode in order to get the full
    // benefit from the nice pixmaps in the color group.

    if ( w->inherits("TQLCDNumber") ){
	return;
    }

    if ( !w->isTopLevel() ) {
	if ( w->inherits("TQGroupBox")
	     || w->inherits("TQTabWidget")
	     || w->inherits("TQPushButton") ) {
	    w->setAutoMask( true );
	    return;
	}
 	if (w->inherits("TQLabel")
 	    || w->inherits("TQSlider")
 	    || w->inherits("TQButton")
	    || w->inherits("TQProgressBar")
	    ){
	    w->setBackgroundOrigin( TQWidget::ParentOrigin );
 	}
    }

    if ( w->inherits( "TQFrame" ) ) {
	TQFrame *f = (TQFrame *)w;
	switch ( f->frameShape() ) {
	    case TQFrame::WinPanel:
		f->setFrameShape( TQFrame::StyledPanel );

	    case TQFrame::Panel:
	    case TQFrame::Box:
	    case TQFrame::StyledPanel:
	    case TQFrame::PopupPanel:
		if ( f->frameWidth() == 1 )
		    f->setLineWidth( 2 );
		break;
	    default:
		break;
	}
    }

    if ( w->inherits( "TQListBox" ) ) {
	// the list box in combos has an ugly border otherwise
	TQFrame *f = (TQFrame *)w;
	if ( f->frameShadow() == TQFrame::Plain ) {
	    f->setFrameShadow( TQFrame::Raised );
	    f->setLineWidth( 1 );
	}
    }
}

/*!
  \reimp
*/
void TQInterlaceStyle::unPolish( TQWidget* w)
{

   // the polish function sets some widgets to transparent mode and
    // some to translate background mode in order to get the full
    // benefit from the nice pixmaps in the color group.

    if ( w->inherits("TQLCDNumber") ){
	return;
    }

    if ( !w->isTopLevel() ) {
	if ( w->inherits("TQGroupBox")
	     || w->inherits("TQTabWidget")
	     || w->inherits("TQPushButton" ) ) {
	    w->setAutoMask( false );
	    return;
	}
 	if (w->inherits("TQLabel")
 	    || w->inherits("TQSlider")
 	    || w->inherits("TQButton")
	    || w->inherits("TQProgressBar")
	    ){
	    w->setBackgroundOrigin( TQWidget::WidgetOrigin );
 	}
    }

}

/*!
  \reimp
*/
TQRect TQInterlaceStyle::pushButtonContentsRect( TQPushButton *btn )
{
    int fw = 0;
    if ( btn->isDefault() || btn->autoDefault() )
	fw = buttonDefaultIndicatorWidth();

    return buttonRect( fw+5, fw, btn->width()-2*fw-10, btn->height()-2*fw );
}

/*!
  \reimp
*/
void TQInterlaceStyle::drawFocusRect ( TQPainter *p, const TQRect &/*r*/, const TQColorGroup &g, const TQColor * bg, bool /*atBorder*/ )
{
    if (bg ) {
	int h,s,v;
	bg->hsv(&h,&s,&v);
	if (v >= 128)
	    p->setPen( TQt::black );
	else
	    p->setPen( TQt::white );
    }
    else
	p->setPen( g.foreground() );
/*
    p->setBrush( NoBrush );
    if ( atBorder ) {
	p->drawRect( TQRect( r.x()+1, r.y()+2, r.width()-2, r.height()-4 ) );
	p->drawRect( TQRect( r.x()+2, r.y()+1, r.width()-4, r.height()-2 ) );
    } else {
	p->drawRect( TQRect( r.x(), r.y()+1, r.width(), r.height()-2 ) );
	p->drawRect( TQRect( r.x()+1, r.y(), r.width()-2, r.height() ) );
    }
*/
}

/*!
  \reimp
*/
void TQInterlaceStyle::drawButton( TQPainter *p, int x, int y, int w, int h,
				  const TQColorGroup &g, bool /* sunken */,
				  const TQBrush *fill )
{
    const int lineWidth = 2;

    p->setBrush( g.brush( TQColorGroup::Dark ) );
    p->setPen( NoPen );
    p->drawRect( x+1, y+1, 2, 2 );
    p->drawRect( x+w-3, y+1, 2, 2 );
    p->drawRect( x+1, y+h-3, 2, 2 );
    p->drawRect( x+w-3, y+h-3, 2, 2 );

    p->drawRect( x+2, y, w-4, 2 );
    p->drawRect( x+2, y+h-lineWidth, w-4, lineWidth );
    p->drawRect( x, y+2, lineWidth, h-4 );
    p->drawRect( x+w-lineWidth, y+2, lineWidth, h-4 );

    if ( fill ) {
	x += 2;
	y += 2;
	w -= 4;
	h -= 4;
	p->setBrush( *fill );
	p->setPen( NoPen );
	p->drawRect( x+1, y, w-2, 1 );
	p->drawRect( x, y+1, w, h-2 );
	p->drawRect( x+1, y+h-1, w-2, 1 );
    }
}

/*! \reimp */
void TQInterlaceStyle::drawButtonMask( TQPainter * p, int x, int y, int w, int h )
{
    TQBrush fill( color1 );
    TQColorGroup cg;
    cg.setBrush( TQColorGroup::Dark, color1 );
    drawButton( p, x, y, w, h, cg, false, &fill );
}

/*!
  \reimp
*/
void TQInterlaceStyle::drawBevelButton( TQPainter *p, int x, int y, int w, int h,
				       const TQColorGroup &g, bool sunken, const TQBrush* fill )
{
    TQInterlaceStyle::drawButton(p, x, y, w, h, g, sunken, fill);
}

/*!
  \reimp
*/
void TQInterlaceStyle::drawPushButton( TQPushButton* btn, TQPainter *p)
{
    TQColorGroup g = btn->colorGroup();
    int x1, y1, x2, y2;

    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates

    TQBrush fill( g.button() );
    if ( btn->isDown() || btn->isOn() )
	fill = g.mid();

    if ( btn->hasFocus() )
	g.setBrush( TQColorGroup::Dark, black );
    drawButton( p, x1, y1, x2-x1+1, y2-y1+1, g, false, &fill );

    if ( btn->isMenuButton() ) {
	int dx = (y1-y2-4)/3;
	drawArrow( p, DownArrow, false,
		   x2 - dx, dx, y1, y2 - y1,
		   g, btn->isEnabled() );
    }

    if ( p->brush().style() != NoBrush )
	p->setBrush( NoBrush );
}

/*!
  \reimp
*/
TQSize TQInterlaceStyle::indicatorSize () const
{
    return TQSize(13,13);
}

/*!
  \reimp
*/
void TQInterlaceStyle::drawIndicator( TQPainter * p, int x, int y, int w, int h, const TQColorGroup &g, int s, bool down, bool enabled )
{
    p->fillRect( x, y, w, h, g.brush( TQColorGroup::Background ) );
    TQBrush fill;
    if ( s == TQButton::NoChange ) {
	TQBrush b = p->brush();
	TQColor c = p->backgroundColor();
	p->setBackgroundMode( TransparentMode );
	p->setBackgroundColor( green );
	fill = TQBrush(g.base(), Dense4Pattern);
	p->setBackgroundColor( c );
	p->setBrush( b );
    } else if ( down )
	fill = g.brush( TQColorGroup::Button );
    else
	fill = g.brush( enabled ? TQColorGroup::Base : TQColorGroup::Background );

    drawButton( p, x, y, w, h, g, false, &fill );

    if ( s != TQButton::Off ) {
	TQPointArray a( 7*2 );
	int i, xx, yy;
	xx = x+3;
	yy = y+5;
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
	if ( s == TQButton::NoChange ) {
	    p->setPen( g.dark() );
	} else {
	    p->setPen( g.text() );
	}
	p->drawLineSegments( a );
    }
}

/*!
  \reimp
*/
void TQInterlaceStyle::drawIndicatorMask( TQPainter *p, int x, int y, int w, int h, int )
{
    drawButtonMask( p, x, y, w, h );
}

/*!
  \reimp
*/
TQSize TQInterlaceStyle::exclusiveIndicatorSize() const
{
    return TQSize(13,13);
}

/*!
  \reimp
*/
void TQInterlaceStyle::drawExclusiveIndicator( TQPainter *p, int x, int y, int w, int h, const TQColorGroup &g, bool on, bool down, bool enabled )
{
    p->fillRect( x, y, w, h, g.brush( TQColorGroup::Background ) );
    p->setBrush( g.dark() );
    p->setPen( TQPen( NoPen ) );
    p->drawEllipse( x, y, w, h );

    x += 2;
    y += 2;
    w -= 4;
    h -= 4;
    TQColor fillColor = ( down || !enabled ) ? g.button() : g.base();
    p->setBrush( fillColor );
    p->drawEllipse( x, y, w, h );

    if ( on ) {
	p->setBrush( g.text() );
	p->drawEllipse( x+2, y+2, w-4, h-4 );
    }
}

/*!
  \reimp
*/
void TQInterlaceStyle::drawExclusiveIndicatorMask( TQPainter *p, int x, int y, int w, int h, bool )
{
    p->setBrush( color1 );
    p->setPen( TQPen( NoPen ) );
    p->drawEllipse( x, y, w, h );
}

static int get_combo_extra_width( int h, int *return_awh=0 )
{
    int awh;
    if ( h < 8 ) {
	awh = 6;
    } else if ( h < 14 ) {
	awh = h - 2;
    } else {
	awh = h/2;
    }
    if ( return_awh )
	*return_awh = awh;
    return awh*3/2;
}

/*!
  \reimp
*/
TQRect TQInterlaceStyle::comboButtonRect ( int x, int y, int w, int h )
{
    TQRect r = buttonRect( x, y, w, h );
    int ew = get_combo_extra_width( r.height() );
    return TQRect(r.x(), r.y(), r.width()-ew, r.height());
}

static void get_combo_parameters( const TQRect &r,
    int &ew, int &awh, int &ax,
    int &ay, int &sh, int &dh,
    int &sy )
{
    ew = get_combo_extra_width( r.height(), &awh );

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
    ax = r.x() + r.width() - ew +(ew-awh)/2;
}

/*!
  \reimp
*/
void TQInterlaceStyle::drawComboButton( TQPainter *p, int x, int y, int w, int h,
				const TQColorGroup &g,
				bool /* sunken */,
				bool /*editable*/,
				bool /*enabled */,
				const TQBrush *fb )
{
    TQBrush fill = fb ? *fb : g.brush( TQColorGroup::Button );

    int awh, ax, ay, sh, sy, dh, ew;
    get_combo_parameters( buttonRect(x,y,w,h), ew, awh, ax, ay, sh, dh, sy );

    drawButton( p, x, y, w, h, g, false, &fill );

    qDrawArrow( p, DownArrow, MotifStyle, false, ax, ay, awh, awh, g, true );

    p->setPen( g.dark() );
    p->drawRect( ax+1, sy+1, awh-1, sh-1 );
}

/*!
  \reimp
*/
void TQInterlaceStyle::drawPushButtonLabel( TQPushButton* btn, TQPainter *p)
{
    TQRect r = btn->rect();
    int x, y, w, h;
    r.rect( &x, &y, &w, &h );

    int x1, y1, x2, y2;
    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates
    int dx = 0;
    int dy = 0;
    if ( btn->isMenuButton() )
	dx = (y2-y1) / 3;
    if ( btn->isOn() || btn->isDown() ) {
//	dx--;
//	dy--;
    }
    if ( dx || dy )
	p->translate( dx, dy );

    x += 2;  y += 2;  w -= 4;  h -= 4;
    TQColorGroup g = btn->colorGroup();
    const TQColor *col = &btn->colorGroup().buttonText();
    if ( (btn->isDown() || btn->isOn()) )
	col = &btn->colorGroup().brightText();
    else if ( !btn->isEnabled() )
	col = &btn->colorGroup().dark();
    drawItem( p, x, y, w, h,
	      AlignCenter|ShowPrefix,
	      g, btn->isEnabled(),
	      btn->pixmap(), btn->text(), -1, col );

    if ( dx || dy )
	p->translate( -dx, -dy );
}

#define HORIZONTAL	(sb->orientation() == TQScrollBar::Horizontal)
#define VERTICAL	!HORIZONTAL
#define MOTIF_BORDER	defaultFrameWidth()
#define SLIDER_MIN	9 // ### motif says 6 but that's too small


/*! \reimp */

void TQInterlaceStyle::scrollBarMetrics( const TQScrollBar* sb, int &sliderMin, int &sliderMax, int &sliderLength, int &buttonDim )
{
    int maxLength;
    int b = MOTIF_BORDER;
    int length = HORIZONTAL ? sb->width()  : sb->height();
    int extent = HORIZONTAL ? sb->height() : sb->width();

    if ( length > ( extent - b*2 - 1 )*2 + b*2  )
	buttonDim = extent - b*2;
    else
	buttonDim = ( length - b*2 )/2 - 1;

    sliderMin = b + buttonDim;
    maxLength  = length - b*2 - buttonDim*2;

    if ( sb->maxValue() == sb->minValue() ) {
	sliderLength = maxLength;
    } else {
	uint range = sb->maxValue()-sb->minValue();
	sliderLength = (sb->pageStep()*maxLength)/
			(range + sb->pageStep());
	if ( sliderLength < SLIDER_MIN || range > INT_MAX/2 )
	    sliderLength = SLIDER_MIN;
	if ( sliderLength > maxLength )
	    sliderLength = maxLength;
    }
    sliderMax = sliderMin + maxLength - sliderLength;

}


/*! \reimp */

void TQInterlaceStyle::drawScrollBarControls( TQPainter* p, const TQScrollBar* sb,
					 int sliderStart, uint controls,
					 uint activeControl )
{
#define ADD_LINE_ACTIVE ( activeControl == AddLine )
#define SUB_LINE_ACTIVE ( activeControl == SubLine )
    TQColorGroup g  = sb->colorGroup();

    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    TQBrush fill = g.brush( TQColorGroup::Mid );
    if (sb->backgroundPixmap() ){
	fill = TQBrush( g.mid(), *sb->backgroundPixmap() );
    }

    if ( controls == (AddLine | SubLine | AddPage | SubPage | Slider | First | Last ) )
	drawPanel( p, 0, 0, sb->width(), sb->height(), g, false, 2, &fill );

    if (sliderStart > sliderMax) { // sanity check
	sliderStart = sliderMax;
    }

    int b = MOTIF_BORDER;
    int dimB = buttonDim;
    TQRect addB;
    TQRect subB;
    TQRect addPageR;
    TQRect subPageR;
    TQRect sliderR;
    int addX, addY, subX, subY;
    int length = HORIZONTAL ? sb->width()  : sb->height();
    int extent = HORIZONTAL ? sb->height() : sb->width();

    if ( HORIZONTAL ) {
	subY = addY = ( extent - dimB ) / 2;
	subX = b;
	addX = length - dimB - b;
    } else {
	subX = addX = ( extent - dimB ) / 2;
	subY = b;
	addY = length - dimB - b;
    }

    subB.setRect( subX,subY,dimB,dimB );
    addB.setRect( addX,addY,dimB,dimB );

    int sliderEnd = sliderStart + sliderLength;
    int sliderW = extent - b*2;
    if ( HORIZONTAL ) {
	subPageR.setRect( subB.right() + 1, b,
			  sliderStart - subB.right() , sliderW );
	addPageR.setRect( sliderEnd-1, b, addX - sliderEnd+1, sliderW );
	sliderR .setRect( sliderStart, b, sliderLength, sliderW );
    } else {
	subPageR.setRect( b, subB.bottom()+1, sliderW,
			  sliderStart - subB.bottom() );
	addPageR.setRect( b, sliderEnd-1, sliderW, addY - sliderEnd + 1);
	sliderR .setRect( b, sliderStart, sliderW, sliderLength );
    }

    if ( controls & AddLine )
	drawArrow( p, VERTICAL ? DownArrow : RightArrow,
		   ADD_LINE_ACTIVE, addB.x(), addB.y(),
		   addB.width(), addB.height(), g, true );
    if ( controls & SubLine )
	drawArrow( p, VERTICAL ? UpArrow : LeftArrow,
		   SUB_LINE_ACTIVE, subB.x(), subB.y(),
		   subB.width(), subB.height(), g, true );

    if ( controls & SubPage )
	p->fillRect( subPageR, fill );

    if ( controls & AddPage )
	p->fillRect( addPageR, fill );

    if ( controls & Slider ) {
	TQPoint bo = p->brushOrigin();
	p->setBrushOrigin(sliderR.topLeft());
	if ( sliderR.isValid() )
	    drawButton( p, sliderR.x(), sliderR.y(),
			  sliderR.width(), sliderR.height(), g,
			  false, &g.brush( TQColorGroup::Button ) );
	p->setBrushOrigin(bo);
    }

}

/*!
  \reimp
*/
void TQInterlaceStyle::drawSlider ( TQPainter * p, int x, int y, int w, int h, const TQColorGroup & g, Orientation orient, bool, bool)
{
    p->fillRect( x, y, w, h, g.brush( TQColorGroup::Background ) );
    drawButton( p, x, y, w, h, g, false, &g.brush( TQColorGroup::Button ) );
    if ( orient == Horizontal ) {
	TQCOORD mid = x + w / 2;
	qDrawShadeLine( p, mid,  y , mid,  y + h - 2, g, true, 1);
    } else {
	TQCOORD mid = y +h / 2;
	qDrawShadeLine( p, x, mid,  x + w - 2, mid, g, true, 1);
    }
}


/*!
  \reimp
*/
void TQInterlaceStyle::drawSliderGroove ( TQPainter * p, int x, int y, int w, int h, const TQColorGroup & g, TQCOORD , Orientation o)
{
    p->setBrush( g.brush( TQColorGroup::Dark ) );
    p->setPen( NoPen );

    if ( o == Horizontal )
	drawButton( p, x, y+h/2-3, w, 6, g, false, &g.brush( TQColorGroup::Mid ) );
    else
	drawButton( p, x+w/2-3, y, 6, h, g, false, &g.brush( TQColorGroup::Mid ) );
}


/*!
  \reimp
*/
int TQInterlaceStyle::splitterWidth() const
{
    return TQMAX( 12, TQApplication::globalStrut().width() );
}

/*!
  \reimp
*/
void TQInterlaceStyle::drawSplitter( TQPainter *p, int x, int y, int w, int h,
  const TQColorGroup &g, Orientation orient)
{
    const int motifOffset = 12;
    int sw = splitterWidth();
    if ( orient == Horizontal ) {
	    TQCOORD xPos = x + w/2;
	    TQCOORD kPos = motifOffset;
	    TQCOORD kSize = sw - 4;

	    qDrawShadeLine( p, xPos, kPos + kSize - 1 ,
			    xPos, h, g );
	    drawPanel( p, xPos-sw/2+2, kPos,
			     kSize, kSize, g, false, 2,
			     &g.brush( TQColorGroup::Button ));
	    qDrawShadeLine( p, xPos, 0, xPos, kPos, g );
	} else {
	    TQCOORD yPos = y + h/2;
	    TQCOORD kPos = w - motifOffset - sw;
	    TQCOORD kSize = sw - 4;

	    qDrawShadeLine( p, 0, yPos, kPos, yPos, g );
	    drawPanel( p, kPos, yPos-sw/2+2,
			     kSize, kSize, g, false, 2,
			     &g.brush( TQColorGroup::Button ));
	    qDrawShadeLine( p, kPos + kSize -1, yPos,
			    w, yPos, g );
	}

}

/*!
  \reimp
*/
void TQInterlaceStyle::drawPanel( TQPainter *p, int x, int y, int w, int h,
			    const TQColorGroup &g, bool /*sunken*/,
			    int lineWidth, const TQBrush *fill )
{
    if ( lineWidth < 2 )
	lineWidth = 2;

    p->setBrush( g.brush( TQColorGroup::Dark ) );
    p->setPen( NoPen );

    p->drawRect( x, y, w, lineWidth );
    p->drawRect( x, y+h-lineWidth, w, lineWidth );
    p->drawRect( x, y, lineWidth, h );
    p->drawRect( x+w-lineWidth, y, lineWidth, h );

    if ( fill ) {
	x += lineWidth;
	y += lineWidth;
	w -= 2*lineWidth;
	h -= 2*lineWidth;
	p->setBrush( *fill );
	p->setPen( NoPen );
	p->drawRect( x, y, w, h );
    }
}

#endif // TQT_NO_STYLE_INTERLACE

#endif
