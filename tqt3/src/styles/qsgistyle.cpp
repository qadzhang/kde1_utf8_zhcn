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

#include "ntqsgistyle.h"

#if !defined(TQT_NO_STYLE_SGI) || defined(QT_PLUGIN)

#include "ntqpopupmenu.h"
#include "ntqapplication.h"
#include "ntqbutton.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqpixmap.h"
#include "ntqpalette.h"
#include "ntqwidget.h"
#include "ntqpushbutton.h"
#include "ntqscrollbar.h"
#include "ntqcombobox.h"
#include "ntqslider.h"
#include "ntqtextedit.h"
#include "ntqtoolbar.h"
#include "ntqlineedit.h"
#include "ntqmenubar.h"
#include <limits.h>

#ifndef TQT_NO_SLIDER
struct SliderLastPosition
{
    SliderLastPosition() : rect(0,-1,0,-1), slider(0) {}
    TQRect rect;
    const void* slider;
};
#endif

#ifndef TQT_NO_SCROLLBAR
struct ScrollbarLastPosition
{
    ScrollbarLastPosition() : rect( 0,-1, 0,-1 ), scrollbar(0) {}
    TQRect rect;
    const void* scrollbar;
};
#endif

class TQSGIStylePrivate
{
public:
    TQSGIStylePrivate()
	: hotWidget( 0 ), mousePos( -1, -1 )
    {
    }

    const void* hotWidget;
    TQPoint mousePos;
#ifndef TQT_NO_SCROLLBAR
    ScrollbarLastPosition lastScrollbarRect;
#endif
#ifndef TQT_NO_SLIDER
    SliderLastPosition lastSliderRect;
#endif
};

/*!
    \class TQSGIStyle ntqsgistyle.h
    \brief The TQSGIStyle class provides SGI/Irix look and feel.

    \ingroup appearance

    This class implements the SGI look and feel. It resembles the
    SGI/Irix Motif GUI style as closely as TQStyle allows.
*/

/*!
    Constructs a TQSGIStyle.

    If \a useHighlightCols is false (default value), the style will
    polish the application's color palette to emulate the Motif way of
    highlighting, which is a simple inversion between the base and the
    text color.

    \sa TQMotifStyle::useHighlightColors()
*/
TQSGIStyle::TQSGIStyle( bool useHighlightCols ) : TQMotifStyle( useHighlightCols ), isApplicationStyle( 0 )
{
    d = new TQSGIStylePrivate;
}

/*!
    Destroys the style.
*/
TQSGIStyle::~TQSGIStyle()
{
    delete d;
}

/*!
    \reimp

    Changes some application-wide settings to be SGI-like, e.g. sets a
    bold italic font for menu options.
*/
void
TQSGIStyle::applicationPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr )
{
    isApplicationStyle = 1;
    TQMotifStyle::polish( ceData, elementFlags, ptr );

    TQPalette pal = TQApplication::palette();
    // check this on SGI-Boxes
    //pal.setColor( TQColorGroup::Background, pal.active().midlight() );
    if (pal.active().button() == pal.active().background())
        pal.setColor( TQColorGroup::Button, pal.active().button().dark(120) );
    // darker basecolor in list-widgets
    pal.setColor( TQColorGroup::Base, pal.active().base().dark(130) );
    if (! useHighlightColors() ) {
        pal.setColor( TQPalette::Active, TQColorGroup::Highlight, pal.active().text() );
        pal.setColor( TQPalette::Active, TQColorGroup::HighlightedText, pal.active().base() );
        pal.setColor( TQPalette::Inactive, TQColorGroup::Highlight, pal.inactive().text() );
        pal.setColor( TQPalette::Inactive, TQColorGroup::HighlightedText, pal.inactive().base() );
        pal.setColor( TQPalette::Disabled, TQColorGroup::Highlight, pal.disabled().text() );
        pal.setColor( TQPalette::Disabled, TQColorGroup::HighlightedText, pal.disabled().base() );
    }
    TQApplication::setPalette( pal, true );

    // different basecolor and highlighting in Q(Multi)LineEdit
    pal.setColor( TQColorGroup::Base, TQColor(211,181,181) );
    pal.setColor( TQPalette::Active, TQColorGroup::Highlight, pal.active().midlight() );
    pal.setColor( TQPalette::Active, TQColorGroup::HighlightedText, pal.active().text() );
    pal.setColor( TQPalette::Inactive, TQColorGroup::Highlight, pal.inactive().midlight() );
    pal.setColor( TQPalette::Inactive, TQColorGroup::HighlightedText, pal.inactive().text() );
    pal.setColor( TQPalette::Disabled, TQColorGroup::Highlight, pal.disabled().midlight() );
    pal.setColor( TQPalette::Disabled, TQColorGroup::HighlightedText, pal.disabled().text() );

    TQApplication::setPalette( pal, true, "TQLineEdit" );
    TQApplication::setPalette( pal, true, "TQTextEdit" );
    TQApplication::setPalette( pal, true, "TQDateTimeEditBase" );

    pal = TQApplication::palette();
    pal.setColor( TQColorGroup::Button, pal.active().background() );
    TQApplication::setPalette( pal, true, "TQMenuBar" );
    TQApplication::setPalette( pal, true, "TQToolBar" );
    TQApplication::setPalette( pal, true, "TQPopupMenu" );
}

/*! \reimp
*/
void
TQSGIStyle::applicationUnPolish( const TQStyleControlElementData&, ControlElementFlags, void * )
{
    TQFont f = TQApplication::font();
    TQApplication::setFont( f, true ); // get rid of the special fonts for special widget classes
}

/*!
    \reimp

    Installs an event filter for several widget classes to enable
    hovering.
*/
void
TQSGIStyle::polish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr )
{
    TQMotifStyle::polish(ceData, elementFlags, ptr);

    if ( !isApplicationStyle ) {
        TQPalette sgiPal = TQApplication::palette();

        sgiPal.setColor( TQColorGroup::Background, sgiPal.active().midlight() );
        if (sgiPal.active().button() == sgiPal.active().background())
            sgiPal.setColor( TQColorGroup::Button, sgiPal.active().button().dark(110) );
        sgiPal.setColor( TQColorGroup::Base, sgiPal.active().base().dark(130) );
        if (! useHighlightColors() ) {
            sgiPal.setColor( TQPalette::Active, TQColorGroup::Highlight, sgiPal.active().text() );
            sgiPal.setColor( TQPalette::Active, TQColorGroup::HighlightedText, sgiPal.active().base() );
            sgiPal.setColor( TQPalette::Inactive, TQColorGroup::Highlight, sgiPal.inactive().text() );
            sgiPal.setColor( TQPalette::Inactive, TQColorGroup::HighlightedText, sgiPal.inactive().base() );
            sgiPal.setColor( TQPalette::Disabled, TQColorGroup::Highlight, sgiPal.disabled().text() );
            sgiPal.setColor( TQPalette::Disabled, TQColorGroup::HighlightedText, sgiPal.disabled().base() );
        }

        if ( (ceData.widgetObjectTypes.contains("TQLineEdit")) || (ceData.widgetObjectTypes.contains("TQTextEdit")) ) {
            // different basecolor and highlighting in Q(Multi)LineEdit
            sgiPal.setColor( TQColorGroup::Base, TQColor(211,181,181) );
            sgiPal.setColor( TQPalette::Active, TQColorGroup::Highlight, sgiPal.active().midlight() );
            sgiPal.setColor( TQPalette::Active, TQColorGroup::HighlightedText, sgiPal.active().text() );
            sgiPal.setColor( TQPalette::Inactive, TQColorGroup::Highlight, sgiPal.inactive().midlight() );
            sgiPal.setColor( TQPalette::Inactive, TQColorGroup::HighlightedText, sgiPal.inactive().text() );
            sgiPal.setColor( TQPalette::Disabled, TQColorGroup::Highlight, sgiPal.disabled().midlight() );
            sgiPal.setColor( TQPalette::Disabled, TQColorGroup::HighlightedText, sgiPal.disabled().text() );

        } else if ( (ceData.widgetObjectTypes.contains("TQMenuBar")) || (ceData.widgetObjectTypes.contains("TQToolBar")) ) {
            sgiPal.setColor( TQColorGroup::Button, sgiPal.active().midlight() );
        }

        widgetActionRequest(ceData, elementFlags, ptr, WAR_SetPalette, TQStyleWidgetActionRequestData(sgiPal));
    }

    if ( (ceData.widgetObjectTypes.contains("TQButton")) || (ceData.widgetObjectTypes.contains("TQSlider")) || (ceData.widgetObjectTypes.contains("TQScrollBar")) ) {
        installObjectEventHandler(ceData, elementFlags, ptr, this);
        widgetActionRequest(ceData, elementFlags, ptr, WAR_EnableMouseTracking);
#ifndef TQT_NO_SCROLLBAR
        if ( ceData.widgetObjectTypes.contains("TQScrollBar") ) {
            widgetActionRequest(ceData, elementFlags, ptr, WAR_SetBackgroundMode, TQStyleWidgetActionRequestData(TQWidget::NoBackground));
        }
#endif
    } else if ( ceData.widgetObjectTypes.contains("TQComboBox") ) {
	TQFont f = TQApplication::font();
	f.setBold( true );
	f.setItalic( true );
	widgetActionRequest(ceData, elementFlags, ptr, WAR_SetFont, TQStyleWidgetActionRequestData(f));
#ifndef TQT_NO_MENUBAR
    } else if ( ceData.widgetObjectTypes.contains("TQMenuBar") ) {
	widgetActionRequest(ceData, elementFlags, ptr, WAR_FrameSetStyle, TQStyleWidgetActionRequestData(TQFrame::StyledPanel | TQFrame::Raised));
	widgetActionRequest(ceData, elementFlags, ptr, WAR_SetBackgroundMode, TQStyleWidgetActionRequestData(TQWidget::PaletteBackground));
	TQFont f = TQApplication::font();
	f.setBold( true );
	f.setItalic( true );
	widgetActionRequest(ceData, elementFlags, ptr, WAR_SetFont, TQStyleWidgetActionRequestData(f));
#endif
#ifndef TQT_NO_POPUPMENU
    } else if ( ceData.widgetObjectTypes.contains("TQPopupMenu") ) {
	TQStyleWidgetActionRequestData requestData;
	widgetActionRequest(ceData, elementFlags, ptr, WAR_FrameSetLineWidth, TQStyleWidgetActionRequestData(pixelMetric( PM_DefaultFrameWidth, TQStyleControlElementData(), CEF_None ) + 1));
	TQFont f = TQApplication::font();
	f.setBold( true );
	f.setItalic( true );
	widgetActionRequest(ceData, elementFlags, ptr, WAR_SetFont, TQStyleWidgetActionRequestData(f));
#endif
    } else if ( (ceData.widgetObjectTypes.contains("TQToolBar")) || (ceData.widgetObjectTypes.contains("TQToolBarSeparator")) ) {
	widgetActionRequest(ceData, elementFlags, ptr, WAR_SetBackgroundMode, TQStyleWidgetActionRequestData(TQWidget::PaletteBackground));
    }
}

/*! \reimp */
void
TQSGIStyle::unPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr )
{
    if ( (ceData.widgetObjectTypes.contains("TQButton")) || (ceData.widgetObjectTypes.contains("TQSlider")) || (ceData.widgetObjectTypes.contains("TQScrollBar")) ) {
        removeObjectEventHandler(ceData, elementFlags, ptr, this);
#ifndef TQT_NO_POPUPMENU
    } else if ( ceData.widgetObjectTypes.contains("TQPopupMenu") ) {
	widgetActionRequest(ceData, elementFlags, ptr, WAR_FrameSetLineWidth, TQStyleWidgetActionRequestData(pixelMetric( PM_DefaultFrameWidth, TQStyleControlElementData(), CEF_None )));
	widgetActionRequest(ceData, elementFlags, ptr, WAR_SetFont, TQStyleWidgetActionRequestData(TQApplication::font()));
#endif
#if !defined(TQT_NO_MENUBAR) || !defined(TQT_NO_COMBOBOX)
    } else if ( (ceData.widgetObjectTypes.contains("TQMenuBar")) || (ceData.widgetObjectTypes.contains("TQComboBox")) ) {
	widgetActionRequest(ceData, elementFlags, ptr, WAR_SetFont, TQStyleWidgetActionRequestData(TQApplication::font()));
#endif
    }
}

/*! \reimp */
bool TQSGIStyle::objectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQEvent *e )
{
    if ( (!(ceData.widgetObjectTypes.contains("TQWidget"))) || (e->type() == TQEvent::Paint) )
	return TQMotifStyle::objectEventHandler( ceData, elementFlags, source, e );

    switch ( e->type() ) {
    case TQEvent::MouseButtonPress:
        {
#ifndef TQT_NO_SCROLLBAR
	    if (ceData.widgetObjectTypes.contains("TQScrollBar")) {
		d->lastScrollbarRect.rect = ceData.sliderRect;
		d->lastScrollbarRect.scrollbar = source;
		widgetActionRequest(ceData, elementFlags, source, WAR_Repaint);
	    } else
#endif
	    {
#ifndef TQT_NO_SLIDER
		if (ceData.widgetObjectTypes.contains("TQSlider")) {
		    d->lastSliderRect.rect = ceData.sliderRect;
		    d->lastSliderRect.slider = source;
		    widgetActionRequest(ceData, elementFlags, source, WAR_Repaint);
		}
#endif
	    }
        }
        break;

    case TQEvent::MouseButtonRelease:
        {
	    if ( 0 ) {
#ifndef TQT_NO_SCROLLBAR
	    } else if (ceData.widgetObjectTypes.contains("TQScrollBar")) {
		TQRect oldRect = d->lastScrollbarRect.rect;
		d->lastScrollbarRect.rect = TQRect( 0, -1, 0, -1 );
		widgetActionRequest(ceData, elementFlags, source, WAR_RepaintRect, TQStyleWidgetActionRequestData(oldRect));
#endif
#ifndef TQT_NO_SLIDER
	    } else if (ceData.widgetObjectTypes.contains("TQSlider")) {
		TQRect oldRect = d->lastSliderRect.rect;
		d->lastSliderRect.rect = TQRect( 0, -1, 0, -1 );
		widgetActionRequest(ceData, elementFlags, source, WAR_RepaintRect, TQStyleWidgetActionRequestData(oldRect));
#endif
            }
        }
        break;

    case TQEvent::MouseMove:
	if ( !(elementFlags & CEF_IsActiveWindow) )
	    break;
	if ( ((TQMouseEvent*)e)->button() )
	    break;

	d->hotWidget = source;
        d->mousePos = ((TQMouseEvent*)e)->pos();
	widgetActionRequest(ceData, elementFlags, source, WAR_Repaint);
        break;

    case TQEvent::Enter:
	if ( !(elementFlags & CEF_IsActiveWindow) )
	    break;
        d->hotWidget = source;
        widgetActionRequest(ceData, elementFlags, source, WAR_Repaint);
        break;

    case TQEvent::Leave:
	if ( !(elementFlags & CEF_IsActiveWindow) )
	    break;
        if ( source == d->hotWidget) {
            d->hotWidget = 0;
            widgetActionRequest(ceData, elementFlags, source, WAR_Repaint);
        }
        break;

    default:
        break;
    }
    return TQMotifStyle::objectEventHandler( ceData, elementFlags, source, e );
}

static const int sgiItemFrame           = 2;    // menu item frame width
// static const int sgiSepHeight 	= 1;    // separator item height
static const int sgiItemHMargin         = 3;    // menu item hor text margin
static const int sgiItemVMargin         = 2;    // menu item ver text margin
static const int sgiArrowHMargin        = 6;    // arrow horizontal margin
static const int sgiTabSpacing 		= 12;   // space between text and tab
// static const int sgiCheckMarkHMargin = 2;    // horiz. margins of check mark ### not used?!?
static const int sgiCheckMarkSpace      = 20;

/*! \reimp */
int TQSGIStyle::pixelMetric( PixelMetric metric, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *widget ) const
{
    switch ( metric ) {
    case PM_DefaultFrameWidth:
	return 2;

    case PM_ButtonDefaultIndicator:
	return 4;

    case PM_ScrollBarExtent:
	return 21;

    case PM_IndicatorWidth:
    case PM_IndicatorHeight:
	return 14;

    case PM_ExclusiveIndicatorWidth:
    case PM_ExclusiveIndicatorHeight:
	return 12;

    case PM_SplitterWidth:
	return TQMAX( 10, TQApplication::globalStrut().width() );

    case PM_MenuIndicatorFrameHBorder:
	return (sgiItemFrame+2);
    case PM_MenuIndicatorFrameVBorder:
	return (sgiItemFrame+4);
    case PM_MenuIconIndicatorFrameHBorder:
    case PM_MenuIconIndicatorFrameVBorder:
	return sgiItemFrame;

    default:
	break;
    }
    return TQMotifStyle::pixelMetric( metric, ceData, elementFlags, widget );
}

static void drawPanel( TQPainter *p, int x, int y, int w, int h,
		const TQColorGroup &g, bool sunken,
		int lineWidth, const TQBrush* fill)
{
    if ( w == 0 || h == 0 )
	return;
#if defined(CHECK_RANGE)
    ASSERT( w > 0 && h > 0 && lineWidth >= 0 );
#endif
    TQPen oldPen = p->pen();			// save pen
    TQPointArray a( 4*lineWidth );
    if ( sunken )
	p->setPen( g.dark() );
    else
	p->setPen( g.light() );
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
	p->setPen( g.light() );
    else
	p->setPen( g.dark() );
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

static void drawSeparator( TQPainter *p, int x1, int y1, int x2, int y2,
			  const TQColorGroup &g )
{
    TQPen oldPen = p->pen();

    p->setPen( g.midlight() );
    p->drawLine( x1, y1, x2, y2 );
    p->setPen( g.shadow() );
    if ( y2-y1 < x2-x1 )
	p->drawLine( x1, y1+1, x2, y2+1 );
    else
	p->drawLine( x1+1, y1, x2+1, y2 );

    p->setPen( oldPen );
}

static void drawSGIPrefix( TQPainter *p, int x, int y, TQString* miText )
{
    if ( miText && (!!(*miText)) ) {
        int amp = 0;
        bool nextAmp = false;
        while ( ( amp = miText->find( '&', amp ) ) != -1 ) {
            if ( (uint)amp == miText->length()-1 )
                return;
            miText->remove( amp,1 );
            nextAmp = (*miText)[amp] == '&';    // next time if &&

            if ( !nextAmp ) {     // draw special underlining
                uint ulx = p->fontMetrics().width(*miText, amp);

                uint ulw = p->fontMetrics().width(*miText, amp+1) - ulx;

                p->drawLine( x+ulx, y, x+ulx+ulw, y );
                p->drawLine( x+ulx, y+1, x+ulx+ulw/2, y+1 );
                p->drawLine( x+ulx, y+2, x+ulx+ulw/4, y+2 );
            }
            amp++;
        }
    }
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
    return awh*2;
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
    if( TQApplication::reverseLayout() )
        ax = r.x();
    else
        ax = r.x() + r.width() - ew;
    ax  += (ew-awh)/2;
}

/*! \reimp */
void TQSGIStyle::drawPrimitive( PrimitiveElement pe,
		    TQPainter *p,
		    const TQStyleControlElementData &ceData,
		    ControlElementFlags elementFlags,
		    const TQRect &r,
		    const TQColorGroup &cg,
		    SFlags flags,
		    const TQStyleOption& opt ) const
{
    const int x = r.x();
    const int y = r.y();
    const int w = r.width();
    const int h = r.height();
    const bool sunken = flags & ( Style_Sunken | Style_Down | Style_On );
    const int defaultFrameWidth = pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags );
    bool hot = ( flags & Style_MouseOver ) && ( flags & Style_Enabled );

    bool dis = ! (flags & Style_Enabled);
    bool act = flags & Style_Active;

    switch ( pe ) {
    case PE_ButtonCommand:
	{
	    TQBrush fill;
	    if ( hot ) {
		if ( sunken )
		    fill = cg.brush( TQColorGroup::Dark );
		else
		    fill = cg.brush( TQColorGroup::Midlight );
	    } else if ( sunken ) {
		fill = cg.brush( TQColorGroup::Mid );
	    } else {
		fill = cg.brush( TQColorGroup::Button );
	    }

	    drawPanel( p, x, y, w, h, cg, sunken, defaultFrameWidth, &fill );
	}
	break;

    case PE_PanelPopup:
    case PE_ButtonBevel:
    case PE_ButtonTool:
	{
	    drawPrimitive( PE_ButtonCommand, p, ceData, elementFlags, TQRect( x+1, y+1, w-2, h-2 ), cg, flags, opt );

	    TQPen oldPen = p->pen();
	    TQPointArray a;

	    // draw twocolored rectangle
	    p->setPen( sunken ? cg.light() : cg.dark().dark(200) );
	    a.setPoints( 3, x, y+h-1, x+w-1, y+h-1, x+w-1, y );
	    p->drawPolyline( a );
	    p->setPen( cg.dark() );
	    a.setPoints( 3, x, y+h-2, x, y, x+w-2, y );
	    p->drawPolyline( a );

	    p->setPen( oldPen );
	}
	break;

    case PE_ArrowUp:
    case PE_ArrowDown:
    case PE_ArrowLeft:
    case PE_ArrowRight:
	{
	    TQPointArray a;				// arrow polygon
	    switch ( pe ) {
	    case PE_ArrowUp:
		a.setPoints( 3, 0,-5, -5,4, 4,4 );
		break;
	    case PE_ArrowDown:
		a.setPoints( 3, 0,4, -4,-4, 4,-4 );
		break;
	    case PE_ArrowLeft:
		a.setPoints( 3, -4,0, 4,-5, 4,4 );
		break;
	    case PE_ArrowRight:
		a.setPoints( 3, 4,0, -4,-5, -4,4 );
		break;
	    default:
		return;
	    }

	    p->save();
	    p->setPen( TQt::NoPen );
	    a.translate( x+w/2, y+h/2 );
	    p->setBrush( flags & Style_Enabled ? cg.dark() : cg.light() );
	    p->drawPolygon( a );			// draw arrow
	    p->restore();
	}
	break;

    case PE_Indicator:
	{
	    TQRect er = r;
	    er.addCoords( 1, 1, -1, -1 );
	    int iflags = flags & ~Style_On;
	    drawPrimitive( PE_ButtonBevel, p, ceData, elementFlags, er, cg, iflags, opt );
	    if ( !(flags & TQStyle::Style_Off) ) {
		er = r;
		er.addCoords( 1, 2, 1, 1 );
		drawPrimitive( PE_CheckMark, p, ceData, elementFlags, er, cg, flags, opt );
	    }
	}
	break;

    case PE_IndicatorMask:
	{
	    TQPen oldPen = p->pen();
	    TQBrush oldBrush = p->brush();

	    p->setPen( TQt::color1 );
	    p->setBrush( TQt::color1 );
	    p->fillRect( x, y, w, h, TQBrush( TQt::color0 ) );
	    TQRect er = r;
	    er.addCoords( 1, 1, -1, -1 );
	    p->fillRect(er, TQBrush(TQt::color1));

	    if ( !(flags & TQStyle::Style_Off) ) {
		er = r;
		er.addCoords( 1, 2, 1, 1 );
		static const TQCOORD check_mark[] = {
			14,0,  10,0,  11,1,  8,1,  9,2,	 7,2,  8,3,  6,3,
			7,4,  1,4,  6,5,  1,5,	6,6,  3,6,  5,7,  4,7,
			5,8,  5,8,  4,3,  2,3,	3,2,  3,2 };

		TQPointArray amark;
		amark = TQPointArray( sizeof(check_mark)/(sizeof(TQCOORD)*2), check_mark );
		amark.translate( er.x()+1, er.y()+1 );
		p->drawLineSegments( amark );
		amark.translate( -1, -1 );
		p->drawLineSegments( amark );
	    }

	    p->setBrush( oldBrush );
	    p->setPen( oldPen );
	}
	break;

    case PE_CheckMark:
	{
	    static const TQCOORD check_mark[] = {
		14,0,  10,0,  11,1,  8,1,  9,2,	 7,2,  8,3,  6,3,
		7,4,  1,4,  6,5,  1,5,	6,6,  3,6,  5,7,  4,7,
		5,8,  5,8,  4,3,  2,3,	3,2,  3,2 };

	    TQPen oldPen = p->pen();

	    TQPointArray amark;
	    amark = TQPointArray( sizeof(check_mark)/(sizeof(TQCOORD)*2), check_mark );
	    amark.translate( x+1, y+1 );

	    if ( flags & Style_On ) {
		p->setPen( flags & Style_Enabled ? cg.shadow() : cg.dark() );
		p->drawLineSegments( amark );
		amark.translate( -1, -1 );
		p->setPen( flags & Style_Enabled ? TQColor(255,0,0) : cg.dark() );
		p->drawLineSegments( amark );
		p->setPen( oldPen );
	    } else {
		p->setPen( flags & Style_Enabled ? cg.dark() : cg.mid() );
		p->drawLineSegments( amark );
		amark.translate( -1, -1 );
		p->setPen( flags & Style_Enabled ? TQColor(230,120,120) : cg.dark() );
		p->drawLineSegments( amark );
		p->setPen( oldPen );
	    }
	}
	break;

    case PE_ExclusiveIndicator:
	{
	    p->save();
	    p->eraseRect( x, y, w, h );
	    p->translate( x, y );

	    p->setPen( cg.button() );
	    p->setBrush( hot ? cg.midlight() : cg.button() );
	    TQPointArray a;
	    a.setPoints( 4, 5,0, 11,6, 6,11, 0,5);
	    p->drawPolygon( a );

	    p->setPen( cg.dark() );
	    p->drawLine( 0,5, 5,0 );
	    p->drawLine( 6,0, 11,5 );
	    p->setPen( flags & Style_Down ? cg.light() : cg.dark() );
	    p->drawLine( 11,6, 6,11 );
	    p->drawLine( 5,11, 0,6 );
	    p->drawLine( 2,7, 5,10 );
	    p->drawLine( 6,10, 9,7 );
	    p->setPen( cg.light() );
	    p->drawLine( 2,5, 5,2 );

	    if ( flags & Style_On ) {
		p->setPen( flags & Style_Enabled ? TQt::blue : TQt::darkGray );
		p->setBrush( flags & Style_Enabled ? TQt::blue : TQt::darkGray  );
		a.setPoints(3, 6,2, 8,4, 6,6 );
		p->drawPolygon( a );
		p->setBrush( TQt::NoBrush );

		p->setPen( cg.shadow() );
		p->drawLine( 7,7, 9,5 );
	    } else {
		p->drawLine( 6,2, 9,5 );
	    }
	    p->restore();
	}
	break;

    case PE_ExclusiveIndicatorMask:
	{
	    p->save();
	    TQPen oldPen = p->pen();
	    TQBrush oldBrush = p->brush();

	    p->setPen( TQt::color1 );
	    p->setBrush( TQt::color1 );
	    TQPointArray a;
	    a.setPoints( 8, 0,5, 5,0, 6,0, 11,5, 11,6, 6,11, 5,11, 0,6 );
	    a.translate( x, y );
	    p->drawPolygon( a );

	    p->setBrush( oldBrush );
	    p->setPen( oldPen );
	    p->restore();
	}
	break;

    case PE_Panel:
	{
	    const int lineWidth = opt.isDefault() ? defaultFrameWidth : opt.lineWidth();
	    drawPanel( p, x, y, w, h, cg, flags & (Style_Sunken | Style_Down | Style_On), lineWidth, 0 );
	    if ( lineWidth <= 1 )
		return;

	    // draw extra shadinglines
	    TQPen oldPen = p->pen();
	    p->setPen( cg.midlight() );
	    p->drawLine( x+1, y+h-3, x+1, y+1 );
	    p->drawLine( x+1, y+1, x+w-3, y+1 );
	    p->setPen( cg.mid() );
	    p->drawLine( x+1, y+h-2, x+w-2, y+h-2 );
	    p->drawLine( x+w-2, y+h-2, x+w-2, y+1 );
	    p->setPen(oldPen);
	}
	break;

    case PE_ScrollBarSubLine:
	if ( !r.contains( d->mousePos ) && !(flags & Style_Active) )
	    flags &= ~Style_MouseOver;
	drawPrimitive( PE_ButtonCommand, p, ceData, elementFlags, r, cg, flags, opt );
	drawPrimitive(((flags & Style_Horizontal) ? PE_ArrowLeft : PE_ArrowUp),
		      p, ceData, elementFlags, r, cg, Style_Enabled | flags);
	break;

    case PE_ScrollBarAddLine:
	if ( !r.contains( d->mousePos ) )
	    flags &= ~Style_MouseOver;
	drawPrimitive( PE_ButtonCommand, p, ceData, elementFlags, r, cg, flags, opt );
	drawPrimitive(((flags & Style_Horizontal) ? PE_ArrowRight : PE_ArrowDown),
		      p, ceData, elementFlags, r, cg, Style_Enabled | flags);
	break;

    case PE_ScrollBarSubPage:
    case PE_ScrollBarAddPage:
	if ( !r.contains( d->mousePos ) )
	    flags &= ~Style_MouseOver;
	if ( r.isValid() )
	    qDrawShadePanel( p, x, y, w, h, cg, false, 1, hot ? &cg.brush( TQColorGroup::Midlight ) : &cg.brush( TQColorGroup::Button ) );
	break;

    case PE_ScrollBarSlider:
	{
	    if ( !r.isValid() )
		break;
	    if ( !(r.contains( d->mousePos ) || flags & Style_Active) || !(flags & Style_Enabled ) )
		flags &= ~Style_MouseOver;

	    TQPixmap pm( r.width(), r.height() );
	    TQPainter bp( &pm );
	    drawPrimitive(PE_ButtonBevel, &bp, ceData, elementFlags, TQRect(0,0,r.width(),r.height()), cg, flags | Style_Enabled | Style_Raised);
	    if ( flags & Style_Horizontal ) {
		const int sliderM =  r.width() / 2;
		if ( r.width() > 20 ) {
		    drawSeparator( &bp, sliderM-5, 2, sliderM-5, r.height()-3, cg );
		    drawSeparator( &bp, sliderM+3, 2, sliderM+3, r.height()-3, cg );
		}
		if ( r.width() > 10 )
		    drawSeparator( &bp, sliderM-1, 2, sliderM-1, r.height()-3, cg );

	    } else {
		const int sliderM =  r.height() / 2;
		if ( r.height() > 20 ) {
		    drawSeparator( &bp, 2, sliderM-5, r.width()-3, sliderM-5, cg );
		    drawSeparator( &bp, 2, sliderM+3, r.width()-3, sliderM+3, cg );
		}
		if ( r.height() > 10 )
		    drawSeparator( &bp, 2, sliderM-1, r.width()-3, sliderM-1, cg );
	    }
	    bp.end();
	    p->drawPixmap( r.x(), r.y(), pm );
	}

	break;

    case PE_Splitter:
	{
	    const int motifOffset = 10;
	    int sw = pixelMetric( PM_SplitterWidth, ceData, elementFlags );
	    if ( flags & Style_Horizontal ) {
		int xPos = x + w/2;
		int kPos = motifOffset;
		int kSize = sw - 2;

		qDrawShadeLine( p, xPos, kPos + kSize - 1 ,
			xPos, h, cg );

		drawPrimitive( PE_ButtonBevel, p, ceData, elementFlags, TQRect(xPos-sw/2+1, kPos, kSize, kSize+1), cg, flags, opt );
		qDrawShadeLine( p, xPos+2, 0, xPos, kPos, cg );
	    } else {
		int yPos = y + h/2;
		int kPos = w - motifOffset - sw;
		int kSize = sw - 2;

		qDrawShadeLine( p, 0, yPos, kPos, yPos, cg );
		drawPrimitive( PE_ButtonBevel, p, ceData, elementFlags, TQRect( kPos, yPos-sw/2+1, kSize+1, kSize ), cg, flags, opt );
		qDrawShadeLine( p, kPos + kSize+1, yPos, w, yPos, cg );
	    }
	}
	break;

    case PE_MenuItemIndicatorFrame:
	{
	    int x, y, w, h;
	    r.rect(&x, &y, &w, &h);

	    SFlags cflags = Style_Default;
	    if (! dis)
		cflags |= Style_Enabled;
	    if (act)
		cflags |= Style_On;

	    TQRect er( x+sgiItemFrame+1, y+sgiItemFrame+3, pixelMetric(PM_IndicatorWidth, ceData, elementFlags), pixelMetric(PM_IndicatorHeight, ceData, elementFlags) );
	    er.addCoords( 1, 1, -1, -1 );
	    drawPrimitive( PE_ButtonBevel, p, ceData, elementFlags, er, cg, cflags, opt );
	}
	break;

    case PE_MenuItemIndicatorIconFrame:
	{
	    int x, y, w, h;
	    r.rect(&x, &y, &w, &h);
	    int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags, opt, NULL, NULL);

	    drawPanel( p, x+sgiItemFrame, y+sgiItemFrame, checkcol, h-2*sgiItemFrame, cg, true, 1, &cg.brush( TQColorGroup::Light ) );
	}
	break;

    case PE_MenuItemIndicatorCheck:
	{
	    int x, y, w, h;
	    r.rect(&x, &y, &w, &h);

	    SFlags cflags = Style_Default;
	    if (! dis)
		cflags |= Style_Enabled;
	    if (act)
		cflags |= Style_On;

	    TQRect er( x+sgiItemFrame+1, y+sgiItemFrame+3, pixelMetric(PM_IndicatorWidth, ceData, elementFlags), pixelMetric(PM_IndicatorHeight, ceData, elementFlags) );
	    er.addCoords( 1, 1, -1, -1 );
	    er.addCoords( 0, 1, 1, 1 );
	    drawPrimitive( PE_CheckMark, p, ceData, elementFlags, er, cg, cflags | Style_On, opt );
	}
	break;

    default:
	TQMotifStyle::drawPrimitive( pe, p, ceData, elementFlags, r, cg, flags, opt );
	break;
    }
}

/*! \reimp */
void TQSGIStyle::drawControl( ControlElement element,
		  TQPainter *p,
		  const TQStyleControlElementData &ceData,
		  ControlElementFlags elementFlags,
		  const TQRect &r,
		  const TQColorGroup &cg,
		  SFlags flags,
		  const TQStyleOption& opt,
		  const TQWidget *widget ) const
{
    if ( widget == d->hotWidget )
	flags |= Style_MouseOver;

    switch ( element ) {
    case CE_PushButton:
	{
#ifndef TQT_NO_PUSHBUTTON
	    int x1, y1, x2, y2;
	    r.coords( &x1, &y1, &x2, &y2 );

	    p->setPen( cg.foreground() );
	    p->setBrush( TQBrush( cg.button(),TQt::NoBrush ) );
	    p->setBrushOrigin( -ceData.bgOffset.x(),
			       -ceData.bgOffset.y() );

	    int diw = pixelMetric( TQStyle::PM_ButtonDefaultIndicator, ceData, elementFlags );
	    if ( ( elementFlags & CEF_IsDefault ) || ( elementFlags & CEF_AutoDefault ) ) {
		x1 += diw;
		y1 += diw;
		x2 -= diw;
		y2 -= diw;
	    }

	    TQPointArray a;
	    if ( elementFlags & CEF_IsDefault ) {
		if ( diw == 0 ) {
		    a.setPoints( 9,
				 x1, y1, x2, y1, x2, y2, x1, y2, x1, y1+1,
				 x2-1, y1+1, x2-1, y2-1, x1+1, y2-1, x1+1, y1+1 );
		    p->setPen( cg.shadow() );
		    p->drawPolyline( a );
		    x1 += 2;
		    y1 += 2;
		    x2 -= 2;
		    y2 -= 2;
		} else {
		    qDrawShadePanel( p, ceData.rect, cg, true );
		}
	    }

	    TQBrush fill = cg.brush( TQColorGroup::Button );
	    if ( (!( elementFlags & CEF_IsFlat )) || ( elementFlags & CEF_IsOn ) || ( elementFlags & CEF_IsDown ) )
		drawPrimitive( PE_ButtonBevel, p, ceData, elementFlags, TQRect( x1, y1, x2-x1+1, y2-y1+1 ), cg, flags, opt );

	    if ( p->brush().style() != TQt::NoBrush )
		p->setBrush( TQt::NoBrush );
#endif
	}
    break;

    case CE_PopupMenuItem:
	{
#ifndef TQT_NO_POPUPMENU
	    if ( ( elementFlags & CEF_UseGenericParameters ) || opt.isDefault())
		break;
	    TQMenuItem *mi = opt.menuItem();
	    if ( !mi )
		break;
	    int tab = opt.tabWidth();
	    int maxpmw = opt.maxIconWidth();
	    bool dis = ! (flags & Style_Enabled);
	    bool checkable = ( elementFlags & CEF_IsCheckable );
	    bool act = flags & Style_Active;
	    int x, y, w, h;

	    r.rect(&x, &y, &w, &h);

	    if ( checkable )
		maxpmw = TQMAX( maxpmw, sgiCheckMarkSpace );
	    int checkcol = maxpmw;

	    if (mi && mi->isSeparator() ) {
		p->setPen( cg.mid() );
		p->drawLine(x, y, x+w, y );
		return;
	    }

	    int pw = sgiItemFrame;

	    if ( act && !dis ) {
		if ( pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags ) > 1 )
		    drawPanel( p, x, y, w, h, cg, false, pw,
				     &cg.brush( TQColorGroup::Light ) );
		else
		    drawPanel( p, x+1, y+1, w-2, h-2, cg, false, 1,
				     &cg.brush( TQColorGroup::Light ) );
	    } else {
		p->fillRect( x, y, w, h, cg.brush( TQColorGroup::Button ) );
	    }

	    if ( !mi )
		return;

	    if ( mi->isChecked() ) {
		if ( mi->iconSet() ) {
		    drawPrimitive(PE_MenuItemIndicatorIconFrame, p, ceData, elementFlags, r, cg, flags, opt);
		}
	    } else {
		if ( !act )
		    p->fillRect( x+sgiItemFrame, y+sgiItemFrame, checkcol, h-2*sgiItemFrame,
				cg.brush( TQColorGroup::Button ) );
	    }

	    if ( mi->iconSet() ) {
		TQIconSet::Mode mode = TQIconSet::Normal;
		if ( act && !dis )
		    mode = TQIconSet::Active;
		TQPixmap pixmap;
		if ( checkable && mi->isChecked() )
		    pixmap = mi->iconSet()->pixmap( TQIconSet::Small, mode, TQIconSet::On );
		else
		    pixmap = mi->iconSet()->pixmap( TQIconSet::Small, mode );

		int pixw = pixmap.width();
		int pixh = pixmap.height();
		TQRect cr( x+sgiItemFrame, y+sgiItemFrame, checkcol, h-2*sgiItemFrame );
		TQRect pmr( 0, 0, pixw, pixh );
		pmr.moveCenter( cr.center() );
		p->setPen( cg.text() );
		p->drawPixmap( pmr.topLeft(), pixmap );
	    } else {
		if ( checkable ) {
		    if ( mi->isChecked() ) {
			drawPrimitive(PE_MenuItemIndicatorFrame, p, ceData, elementFlags, r, cg, flags, opt);
			drawPrimitive(PE_MenuItemIndicatorCheck, p, ceData, elementFlags, r, cg, flags, opt);
		    }
		}
	    }

	    p->setPen( cg.buttonText() );

	    TQColor discol;
	    if ( dis ) {
		discol = cg.text();
		p->setPen( discol );
	    }

	    int xm = sgiItemFrame + checkcol + sgiItemHMargin;

	    if ( mi->custom() ) {
		int m = sgiItemVMargin;
		p->save();
		mi->custom()->paint( p, cg, act, !dis,
				     x+xm, y+m, w-xm-tab+1, h-2*m );
		p->restore();
	    }

	    TQString s = mi->text();
	    if ( !!s ) {
		int t = s.find( '\t' );
		int m = sgiItemVMargin;
		const int text_flags = AlignVCenter | DontClip | SingleLine; //special underline for &x

		TQString miText = s;
		if ( t>=0 ) {
		    p->drawText(x+w-tab-sgiItemHMargin-sgiItemFrame,
				y+m, tab, h-2*m, text_flags, miText.mid( t+1 ) );
		    miText = s.mid( 0, t );
		}
		TQRect br = p->fontMetrics().boundingRect( x+xm, y+m, w-xm-tab+1, h-2*m,
			text_flags, mi->text() );

		drawSGIPrefix( p, br.x()+p->fontMetrics().leftBearing(miText[0]),
			br.y()+br.height()+p->fontMetrics().underlinePos()-2, &miText );
		p->drawText( x+xm, y+m, w-xm-tab+1, h-2*m, text_flags, miText, miText.length() );
	    } else {
		if ( mi->pixmap() ) {
		    TQPixmap *pixmap = mi->pixmap();
		    if ( pixmap->depth() == 1 )
			p->setBackgroundMode( OpaqueMode );
		    p->drawPixmap( x+xm, y+sgiItemFrame, *pixmap );
		    if ( pixmap->depth() == 1 )
			p->setBackgroundMode( TransparentMode );
		}
	    }
	    if ( mi->popup() ) {
		int dim = (h-2*sgiItemFrame) / 2;
		drawPrimitive( PE_ArrowRight, p, ceData, elementFlags, TQRect( x+w-sgiArrowHMargin-sgiItemFrame-dim, y+h/2-dim/2, dim, dim ), cg, flags );
	    }
#endif
	}
	break;

    case CE_MenuBarItem:
	{
#ifndef TQT_NO_MENUDATA
	    if (opt.isDefault())
		break;

	    TQMenuItem *mi = opt.menuItem();

	    bool active = flags & Style_Active;
	    int x, y, w, h;
	    r.rect( &x, &y, &w, &h );

	    if ( active ) {
		p->setPen( TQPen( cg.shadow(), 1) );
		p->drawRect( x, y, w, h );
		qDrawShadePanel( p, TQRect(x+1,y+1,w-2,h-2), cg, false, 2,
				 &cg.brush( TQColorGroup::Light ));
	    } else {
		p->fillRect( x, y, w, h, cg.brush( TQColorGroup::Button ));
	    }

	    if ( mi->pixmap() )
		drawItem( p, r, AlignCenter|DontClip|SingleLine,
			cg, mi->isEnabled(), mi->pixmap(), "", -1, &cg.buttonText() );

	    if ( !!mi->text() ) {
		TQString* text = new TQString(mi->text());
		TQRect br = p->fontMetrics().boundingRect( x, y-2, w+1, h,
			AlignCenter|DontClip|SingleLine|ShowPrefix, mi->text() );

		drawSGIPrefix( p, br.x()+p->fontMetrics().leftBearing((*text)[0]),
			br.y()+br.height()+p->fontMetrics().underlinePos()-2, text );
		p->drawText( x, y-2, w+1, h, AlignCenter|DontClip|SingleLine, *text, text->length() );
		delete text;
	    }
#endif
	}
	break;

    case CE_CheckBox:
	TQMotifStyle::drawControl( element, p, ceData, elementFlags, r, cg, flags, opt, widget );
	break;

    default:
	TQMotifStyle::drawControl( element, p, ceData, elementFlags, r, cg, flags, opt, widget );
	break;
    }
}

/*! \reimp */
void TQSGIStyle::drawComplexControl( ComplexControl control,
			 TQPainter *p,
			 const TQStyleControlElementData &ceData,
			 ControlElementFlags elementFlags,
			 const TQRect& r,
			 const TQColorGroup& cg,
			 SFlags flags,
			 SCFlags sub,
			 SCFlags subActive,
			 const TQStyleOption& opt,
			 const TQWidget* widget ) const
{
    if ( widget == d->hotWidget )
	flags |= Style_MouseOver;

    switch ( control ) {
    case CC_Slider:
	{
#ifndef TQT_NO_SLIDER
	    TQRect groove = querySubControlMetrics(CC_Slider, ceData, elementFlags, SC_SliderGroove,
						  opt, widget),
		  handle = querySubControlMetrics(CC_Slider, ceData, elementFlags, SC_SliderHandle,
						  opt, widget);

	    if ((sub & SC_SliderGroove) && groove.isValid()) {
		TQRegion region( groove );
		if ( ( sub & SC_SliderHandle ) && handle.isValid() )
		    region = region.subtract( handle );
		if ( d->lastSliderRect.slider == widget && d->lastSliderRect.rect.isValid() )
		    region = region.subtract( d->lastSliderRect.rect );
		p->setClipRegion( region );

		TQRect grooveTop = groove;
		grooveTop.addCoords( 1, 1, -1, -1 );
		drawPrimitive( PE_ButtonBevel, p, ceData, elementFlags, grooveTop, cg, flags & ~Style_MouseOver, opt );

		if ( flags & Style_HasFocus ) {
		    TQRect fr = subRect( SR_SliderFocusRect, ceData, elementFlags, widget );
		    drawPrimitive( PE_FocusRect, p, ceData, elementFlags, fr, cg, flags & ~Style_MouseOver );
		}

		if ( d->lastSliderRect.slider == widget && d->lastSliderRect.rect.isValid() ) {
		    if ( ( sub & SC_SliderHandle ) && handle.isValid() ) {
			region = ceData.rect;
			region = region.subtract( handle );
			p->setClipRegion( region );
		    } else {
			p->setClipping( false );
		    }
		    qDrawShadePanel( p, d->lastSliderRect.rect, cg, true, 1, &cg.brush( TQColorGroup::Dark ) );
		}
		p->setClipping( false );
	    }

	    if (( sub & SC_SliderHandle ) && handle.isValid()) {
		if ( flags & Style_MouseOver && !handle.contains( d->mousePos ) && subActive != SC_SliderHandle )
		    flags &= ~Style_MouseOver;
		drawPrimitive( PE_ButtonBevel, p, ceData, elementFlags, handle, cg, flags );

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
		TQMotifStyle::drawComplexControl( control, p, ceData, elementFlags, r, cg, flags,
						 SC_SliderTickmarks, subActive,
						 opt, widget );
#endif
	    break;
	}
    case CC_ComboBox:
	{
#ifndef TQT_NO_COMBOBOX
	    if (sub & SC_ComboBoxFrame) {
		TQRect fr =
		    TQStyle::visualRect( querySubControlMetrics( CC_ComboBox, ceData, elementFlags,
								SC_ComboBoxFrame, TQStyleOption::Default, widget ), ceData, elementFlags );
		drawPrimitive( PE_ButtonBevel, p, ceData, elementFlags, fr, cg, flags );
	    }

	    if ( sub & SC_ComboBoxArrow ) {
		p->save();
		TQRect er =
		    TQStyle::visualRect( querySubControlMetrics( CC_ComboBox, ceData, elementFlags, SC_ComboBoxArrow, TQStyleOption::Default, widget ), ceData, elementFlags );

		er.addCoords( 0, 3, 0, 0 );

		drawPrimitive( PE_ArrowDown, p, ceData, elementFlags, er, cg, flags | Style_Enabled, opt );

		int awh, ax, ay, sh, sy, dh, ew;
		get_combo_parameters( ceData.rect, ew, awh, ax, ay, sh, dh, sy );

		TQBrush arrow = cg.brush( TQColorGroup::Dark );
		p->fillRect( ax, sy-1, awh, sh, arrow );

		p->restore();
		if ( elementFlags & CEF_HasFocus ) {
		    TQRect re = TQStyle::visualRect( subRect( SR_ComboBoxFocusRect, ceData, elementFlags, widget ), ceData, elementFlags );
		    drawPrimitive( PE_FocusRect, p, ceData, elementFlags, re, cg );
		}
	    }
	    if ( sub & SC_ComboBoxEditField ) {
		if ( elementFlags & CEF_IsEditable ) {
		    TQRect er =
			TQStyle::visualRect( querySubControlMetrics( CC_ComboBox, ceData, elementFlags,
								    SC_ComboBoxEditField, TQStyleOption::Default, widget ), ceData, elementFlags );
		    er.addCoords( -1, -1, 1, 1);
		    qDrawShadePanel( p, TQRect( er.x()-1, er.y()-1,
					       er.width()+2, er.height()+2 ),
				     cg, true, 1, &cg.brush( TQColorGroup::Button ) );
		}
	    }
#endif
	    p->setPen(cg.buttonText());
	    break;
	}

    case CC_ScrollBar:
	{
#ifndef TQT_NO_SCROLLBAR
	    bool maxedOut = (ceData.minSteps == ceData.maxSteps);
	    if ( maxedOut )
		flags &= ~Style_Enabled;

	    TQRect handle = TQStyle::visualRect( querySubControlMetrics( CC_ScrollBar, ceData, elementFlags, SC_ScrollBarSlider, opt, widget ), ceData, elementFlags );

	    if ( sub & SC_ScrollBarGroove ) {
	    }
	    if ( sub & SC_ScrollBarAddLine ) {
		TQRect er = TQStyle::visualRect( querySubControlMetrics( CC_ScrollBar, ceData, elementFlags, SC_ScrollBarAddLine, opt, widget ), ceData, elementFlags );
		drawPrimitive( PE_ScrollBarAddLine, p, ceData, elementFlags, er, cg, flags, opt );
	    }
	    if ( sub & SC_ScrollBarSubLine ) {
		TQRect er = TQStyle::visualRect( querySubControlMetrics( CC_ScrollBar, ceData, elementFlags, SC_ScrollBarSubLine, opt, widget ), ceData, elementFlags );
		drawPrimitive( PE_ScrollBarSubLine, p, ceData, elementFlags, er, cg, flags, opt );
	    }
	    if ( sub & SC_ScrollBarAddPage ) {
		TQRect er = TQStyle::visualRect( querySubControlMetrics( CC_ScrollBar, ceData, elementFlags, SC_ScrollBarAddPage, opt, widget ), ceData, elementFlags );
		TQRegion region( er );
		if ( d->lastScrollbarRect.scrollbar == widget &&
		     d->lastScrollbarRect.rect.isValid() &&
		     er.intersects( d->lastScrollbarRect.rect ) ) {
		    region = region.subtract( d->lastScrollbarRect.rect );
		    p->setClipRegion( region );
		}
		if ( sub & SC_ScrollBarSlider && er.intersects( handle ) ) {
		    region = region.subtract( handle );
		    p->setClipRegion( region );
		}

		drawPrimitive( PE_ScrollBarAddPage, p, ceData, elementFlags, er, cg, flags & ~Style_MouseOver, opt );

		if ( d->lastScrollbarRect.scrollbar == widget &&
		     d->lastScrollbarRect.rect.isValid() &&
		     er.intersects( d->lastScrollbarRect.rect ) ) {
		    if ( sub & SC_ScrollBarSlider && handle.isValid() ) {
			region = er;
			region.subtract( handle );
			p->setClipRegion( region );
		    } else {
			p->setClipping( false );
		    }
		    qDrawShadePanel( p, d->lastScrollbarRect.rect, cg, true, 1, &cg.brush( TQColorGroup::Dark ) );
		}
		p->setClipping( false );
	    }
	    if ( sub & SC_ScrollBarSubPage ) {
		TQRect er = TQStyle::visualRect( querySubControlMetrics( CC_ScrollBar, ceData, elementFlags, SC_ScrollBarSubPage, opt, widget ), ceData, elementFlags );
		TQRegion region( er );
		if ( d->lastScrollbarRect.scrollbar == widget &&
		     d->lastScrollbarRect.rect.isValid() &&
		     er.intersects( d->lastScrollbarRect.rect ) ) {
		    region = region.subtract( d->lastScrollbarRect.rect );
		    p->setClipRegion( region );
		}
		if ( sub & SC_ScrollBarSlider && er.intersects( handle ) ) {
		    region = region.subtract( handle );
		    p->setClipRegion( region );
		}
		drawPrimitive( PE_ScrollBarSubPage, p, ceData, elementFlags, er, cg, flags & ~Style_MouseOver, opt );
		if ( d->lastScrollbarRect.scrollbar == widget &&
		     d->lastScrollbarRect.rect.isValid() &&
		     er.intersects( d->lastScrollbarRect.rect ) ) {
		    if ( sub & SC_ScrollBarSlider && handle.isValid() ) {
			region = er;
			region.subtract( handle );
			p->setClipRegion( region );
		    } else {
			p->setClipping( false );
		    }
		    qDrawShadePanel( p, d->lastScrollbarRect.rect, cg, true, 1, &cg.brush( TQColorGroup::Dark ) );
		}
		p->setClipping( false );
	    }
	    if ( sub & SC_ScrollBarSlider ) {
		p->setClipping( false );
		if ( subActive == SC_ScrollBarSlider )
		    flags |= Style_Active;

		drawPrimitive( PE_ScrollBarSlider, p, ceData, elementFlags, handle, cg, flags, opt );
	    }
#endif
	}
	break;

    default:
 	TQMotifStyle::drawComplexControl( control, p, ceData, elementFlags, r, cg, flags, sub, subActive, opt, widget );
	break;
    }
}

/*!\reimp
*/
TQSize TQSGIStyle::sizeFromContents( ContentsType contents,
				     const TQStyleControlElementData &ceData,
				     ControlElementFlags elementFlags,
				     const TQSize &contentsSize,
				     const TQStyleOption& opt,
				     const TQWidget *widget ) const
{
    TQSize sz(contentsSize);

    switch(contents) {
    case CT_PopupMenuItem:
	{
#ifndef TQT_NO_POPUPMENU
	    if ((elementFlags & CEF_UseGenericParameters) || opt.isDefault())
		break;

	    TQMenuItem *mi = opt.menuItem();
	    sz = TQMotifStyle::sizeFromContents( contents, ceData, elementFlags, contentsSize,
						opt, widget );
	    // SGI checkmark items needs a bit more room
	    if ( elementFlags & CEF_IsCheckable )
		sz.setWidth( sz.width() + 8 );
	    // submenu indicator needs a bit more room
	    if (mi->popup())
		sz.setWidth( sz.width() + sgiTabSpacing );
#endif
	    break;
	}
    case CT_ComboBox:
        sz.rwidth() += 30;
        break;

    default:
	sz = TQMotifStyle::sizeFromContents( contents, ceData, elementFlags, contentsSize, opt, widget );
	break;
    }

    return sz;
}

/*! \reimp */
TQRect TQSGIStyle::subRect( SubRect r, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *widget ) const
{
    TQRect rect;

    switch ( r ) {
    case SR_ComboBoxFocusRect:
	{
	    int awh, ax, ay, sh, sy, dh, ew;
	    int fw = pixelMetric( PM_DefaultFrameWidth, ceData, elementFlags, widget );
	    TQRect tr = ceData.rect;

	    tr.addCoords( fw, fw, -fw, -fw );
	    get_combo_parameters( tr, ew, awh, ax, ay, sh, dh, sy );
	    rect.setRect(ax-2, ay-2, awh+4, awh+sh+dh+4);
	}
	break;
    default:
	return TQMotifStyle::subRect( r, ceData, elementFlags, widget );
    }

    return rect;
}

/*! \reimp */
TQRect TQSGIStyle::querySubControlMetrics( ComplexControl control,
					   const TQStyleControlElementData &ceData,
					   ControlElementFlags elementFlags,
					   SubControl sub,
					   const TQStyleOption& opt,
					   const TQWidget *widget ) const
{
    switch ( control ) {
    case CC_ComboBox:
	switch ( sub ) {
	case SC_ComboBoxFrame:
	    return ceData.rect;

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
	    int ew = get_combo_extra_width( rect.height() );
	    rect.addCoords( 1, 1, -1-ew, -1 );
	    return rect; }

	default:
	    break;
	}
	break;
	case CC_ScrollBar:
	    return TQCommonStyle::querySubControlMetrics( control, ceData, elementFlags, sub, opt, widget );
	default: break;
    }
    return TQMotifStyle::querySubControlMetrics( control, ceData, elementFlags, sub, opt, widget );
}

/*! \reimp */
int TQSGIStyle::styleHint(StyleHint sh, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQStyleOption &opt, TQStyleHintReturn *returnData, const TQWidget *w) const
{
	int ret;

	switch (sh) {
		case SH_MenuIndicatorColumnWidth:
			{
				int maxpmw = opt.maxIconWidth();
				bool checkable = ( elementFlags & CEF_IsCheckable );
			
				if ( checkable )
					maxpmw = TQMAX( maxpmw, sgiCheckMarkSpace );
				ret = maxpmw;
			}
			break;
		default:
			ret = TQMotifStyle::styleHint(sh, ceData, elementFlags, opt, returnData, w);
			break;
	}

	return ret;
}

#endif // TQT_NO_STYLE_SGI
