/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "metal.h"

#ifndef TQT_NO_STYLE_WINDOWS

#include "ntqapplication.h"
#include "ntqcombobox.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h" // for now
#include "ntqpixmap.h" // for now
#include "ntqpalette.h" // for now
#include "ntqwidget.h"
#include "ntqlabel.h"
#include "ntqimage.h"
#include "ntqpushbutton.h"
#include "ntqwidget.h"
#include "ntqrangecontrol.h"
#include "ntqscrollbar.h"
#include "ntqslider.h"
#include <limits.h>


/////////////////////////////////////////////////////////
//#include "stonedark.xpm"
#include "stone1.xpm"
#include "marble.xpm"
///////////////////////////////////////////////////////



MetalStyle::MetalStyle() : TQWindowsStyle() { }

/*!
  Reimplementation from TQStyle
 */
void MetalStyle::applicationPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr )
{
	oldPalette = ceData.palette;
	
	// we simply create a nice TQColorGroup with a couple of fancy
	// pixmaps here and apply to it all widgets
	
	TQFont f("times", ceData.font.pointSize() );
	f.setBold( true );
	f.setItalic( true );
	applicationActionRequest(ceData, elementFlags, ptr, AAR_SetFont, TQStyleApplicationActionRequestData(f, true, "TQMenuBar"));
	applicationActionRequest(ceData, elementFlags, ptr, AAR_SetFont, TQStyleApplicationActionRequestData(f, true, "TQPopupMenu"));
	
	//    TQPixmap button( stonedark_xpm );
	
	TQColor gold("#B9B9A5A54040"); //same as topgrad below
	TQPixmap button( 1, 1 ); button.fill( gold );
	
	TQPixmap background(marble_xpm);
	TQPixmap dark( 1, 1 ); dark.fill( red.dark() );
	TQPixmap mid( stone1_xpm );
	TQPixmap light( stone1_xpm );//1, 1 ); light.fill( green );
	
	TQPalette op = ceData.palette;
	
	TQColor backCol( 227,227,227 );
	
	// TQPalette op(white);
	TQColorGroup active (op.active().foreground(),
			TQBrush(op.active().button(),button),
			TQBrush(op.active().light(), light),
			TQBrush(op.active().dark(), dark),
			TQBrush(op.active().mid(), mid),
			op.active().text(),
			TQt::white,
			op.active().base(),//		     TQColor(236,182,120),
			TQBrush(backCol, background)
			);
	active.setColor( TQColorGroup::ButtonText,  TQt::white  );
	active.setColor( TQColorGroup::Shadow,  TQt::black  );
	TQColorGroup disabled (op.disabled().foreground(),
			TQBrush(op.disabled().button(),button),
			TQBrush(op.disabled().light(), light),
			op.disabled().dark(),
			TQBrush(op.disabled().mid(), mid),
			op.disabled().text(),
			TQt::white,
			op.disabled().base(),//		     TQColor(236,182,120),
			TQBrush(backCol, background)
			);
	
	TQPalette newPalette( active, disabled, active );
	applicationActionRequest(ceData, elementFlags, ptr, AAR_SetPalette, TQStyleApplicationActionRequestData(newPalette, true));
}

/*!
  Reimplementation from TQStyle
 */
void MetalStyle::applicationUnPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr )
{
	applicationActionRequest(ceData, elementFlags, ptr, AAR_SetPalette, TQStyleApplicationActionRequestData(oldPalette, true));
	applicationActionRequest(ceData, elementFlags, ptr, AAR_SetFont, TQStyleApplicationActionRequestData(ceData.font, true));
}

/*!
  Reimplementation from TQStyle
 */
void MetalStyle::polish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr )
{
	// the polish function sets some widgets to transparent mode and
	// some to translate background mode in order to get the full
	// benefit from the nice pixmaps in the color group.

	if (ceData.widgetObjectTypes.contains("TQPushButton")) {
		widgetActionRequest(ceData, elementFlags, ptr, WAR_SetBackgroundMode, TQStyleWidgetActionRequestData(TQWidget::NoBackground));
		return;
	}
	
	if ( !(elementFlags & CEF_IsTopLevel) ) {
		if ( !ceData.bgPixmap.isNull() ) {
			widgetActionRequest(ceData, elementFlags, ptr, WAR_SetBackgroundOrigin, TQStyleWidgetActionRequestData(TQWidget::WindowOrigin));
		}
	}
}

void MetalStyle::unPolish( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr )
{
	// the polish function sets some widgets to transparent mode and
	// some to translate background mode in order to get the full
	// benefit from the nice pixmaps in the color group.
	
	if (ceData.widgetObjectTypes.contains("TQPushButton")) {
		widgetActionRequest(ceData, elementFlags, ptr, WAR_SetBackgroundMode, TQStyleWidgetActionRequestData(TQWidget::PaletteButton));
		return;
	}
	if ( !(elementFlags & CEF_IsTopLevel) ) {
		if ( !ceData.bgPixmap.isNull() ) {
			widgetActionRequest(ceData, elementFlags, ptr, WAR_SetBackgroundOrigin, TQStyleWidgetActionRequestData(TQWidget::WidgetOrigin));
		}
	}
}

void MetalStyle::drawPrimitive( PrimitiveElement pe,
				TQPainter *p,
				const TQStyleControlElementData &ceData,
				ControlElementFlags elementFlags,
				const TQRect &r,
				const TQColorGroup &cg,
				SFlags flags, const TQStyleOption& opt ) const
{
    switch( pe ) {
    case PE_HeaderSection:
	if ( flags & Style_Sunken )
	    flags ^= Style_Sunken | Style_Raised;
	// fall through
    case PE_ButtonBevel:
    case PE_ButtonCommand:
	    drawMetalButton( p, r.x(), r.y(), r.width(), r.height(),
			     (flags & (Style_Sunken|Style_On|Style_Down)),
			     true, !(flags & Style_Raised) );
	    break;
    case PE_PanelMenuBar:
	drawMetalFrame( p, r.x(), r.y(), r.width(), r.height() );
	break;
    case PE_ScrollBarAddLine:	
	drawMetalButton( p, r.x(), r.y(), r.width(), r.height(),
			 flags & Style_Down, !( flags & Style_Horizontal ) );
	drawPrimitive( (flags & Style_Horizontal) ? PE_ArrowRight :PE_ArrowDown,
		       p, ceData, elementFlags, r, cg, flags, opt );
	break;
    case PE_ScrollBarSubLine:
	drawMetalButton( p, r.x(), r.y(), r.width(), r.height(),
			 flags & Style_Down, !( flags & Style_Horizontal ) );
	drawPrimitive( (flags & Style_Horizontal) ? PE_ArrowLeft : PE_ArrowUp,
		       p, ceData, elementFlags, r, cg, flags, opt );
	break;

	
    case PE_ScrollBarSlider:
	drawMetalButton( p, r.x(), r.y(), r.width(), r.height(), false,
			 flags & Style_Horizontal );
	break;
    default:
	TQWindowsStyle::drawPrimitive( pe, p, ceData, elementFlags, r, cg, flags, opt );
	break;
    }
}

void MetalStyle::drawControl( ControlElement element,
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
    case CE_PushButton:
	{
	    int x1, y1, x2, y2;
	
	    r.coords( &x1, &y1, &x2, &y2 );
	
	    p->setPen( cg.foreground() );
	    p->setBrush( TQBrush(cg.button(), NoBrush) );

	
	    TQBrush fill;
	    if ( elementFlags & CEF_IsDown )
		fill = cg.brush( TQColorGroup::Mid );
	    else if ( elementFlags & CEF_IsOn )
		fill = TQBrush( cg.mid(), Dense4Pattern );
	    else
		fill = cg.brush( TQColorGroup::Button );
	
	    if ( elementFlags & CEF_IsDefault ) {
		TQPointArray a;
		a.setPoints( 9,
			     x1, y1, x2, y1, x2, y2, x1, y2, x1, y1+1,
			     x2-1, y1+1, x2-1, y2-1, x1+1, y2-1, x1+1, y1+1 );
		p->setPen( TQt::black );
		p->drawPolyline( a );
		x1 += 2;
		y1 += 2;
		x2 -= 2;
		y2 -= 2;
	    }
	    SFlags flags = Style_Default;
	    if ( elementFlags & CEF_IsOn )
		flags |= Style_On;
	    if ( elementFlags & CEF_IsDown )
		flags |= Style_Down;
	    if ( !(elementFlags & CEF_IsFlat) && !(elementFlags & CEF_IsDown) )
		flags |= Style_Raised;
	    drawPrimitive( PE_ButtonCommand, p, ceData, elementFlags,
			   TQRect( x1, y1, x2 - x1 + 1, y2 - y1 + 1),
			   cg, flags, opt );
	
	    if ( (elementFlags & CEF_IsMenuWidget) ) {
		flags = Style_Default;
		if ( elementFlags & CEF_IsEnabled )
		    flags |= Style_Enabled;
		
		int dx = ( y1 - y2 - 4 ) / 3;
		drawPrimitive( PE_ArrowDown, p, ceData, elementFlags,
			       TQRect(x2 - dx, dx, y1, y2 - y1),
			       cg, flags, opt );
	    }
	    if ( p->brush().style() != NoBrush )
		p->setBrush( NoBrush );
	    break;
	}
    case CE_PushButtonLabel:
	{
	    int x, y, w, h;
	    r.rect( &x, &y, &w, &h );
	
	    int x1, y1, x2, y2;
	    r.coords( &x1, &y1, &x2, &y2 );
	    int dx = 0;
	    int dy = 0;
	    if ( (elementFlags & CEF_IsMenuWidget) )
		dx = ( y2 - y1 ) / 3;
	    if ( (elementFlags & CEF_IsOn) || (elementFlags & CEF_IsDown) ) {
		dx--;
		dy--;
	    }
	    if ( dx || dy )
		p->translate( dx, dy );
	    x += 2;
	    y += 2;
	    w -= 4;
	    h -= 4;
	    drawItem( p, TQRect( x, y, w, h ),
		      AlignCenter|ShowPrefix,
		      cg, (elementFlags & CEF_IsEnabled),
		      (ceData.fgPixmap.isNull())?NULL:&ceData.fgPixmap, ceData.text, -1,
		      ((elementFlags & CEF_IsDown) || (elementFlags & CEF_IsOn))? &cg.brightText() : &cg.buttonText() );
	    if ( dx || dy )
		p->translate( -dx, -dy );
	    break;
	}
    default:
	TQWindowsStyle::drawControl( element, p, ceData, elementFlags, r, cg, how, opt, widget );
	break;
    }
}
void MetalStyle::drawComplexControl( ComplexControl cc,
				     TQPainter *p,
				     const TQStyleControlElementData &ceData,
				     ControlElementFlags elementFlags,
				     const TQRect &r,
				     const TQColorGroup &cg,
				     SFlags how,
				     SCFlags sub,
				     SCFlags subActive,
				     const TQStyleOption& opt,
				     const TQWidget *widget ) const
{
    switch ( cc ) {
    case CC_Slider:
	{
	    TQRect handle = querySubControlMetrics( CC_Slider, ceData, elementFlags,
						   SC_SliderHandle, opt, widget);
	    if ( sub & SC_SliderGroove )
		TQWindowsStyle::drawComplexControl( cc, p, ceData, elementFlags, r, cg, how,
						   SC_SliderGroove, subActive, opt, widget );
	    if ( (sub & SC_SliderHandle) && handle.isValid() )
		drawMetalButton( p, handle.x(), handle.y(), handle.width(),
				 handle.height(), false,
				 ceData.orientation == TQSlider::Horizontal);
	    break;
	}
    case CC_ComboBox:
	{
	    qDrawWinPanel( p, r.x(), r.y(), r.width(), r.height(), cg, true,
			   (elementFlags & CEF_IsEnabled) ? &cg.brush( TQColorGroup::Base ) :
			                      &cg.brush( TQColorGroup::Background ) );
	    drawMetalButton( p, r.x() + r.width() - 2 - 16, r.y() + 2, 16, r.height() - 4,
			     how & Style_Sunken, true );
	    drawPrimitive( PE_ArrowDown, p, ceData, elementFlags,
			   TQRect( r.x() + r.width() - 2 - 16 + 2,
				  r.y() + 2 + 2, 16 - 4, r.height() - 4 -4 ),
			   cg,
			   (elementFlags & CEF_IsEnabled) ? Style_Enabled : Style_Default,
			   opt );
	    break;
	}
    default:
	TQWindowsStyle::drawComplexControl( cc, p, ceData, elementFlags, r, cg, how, sub, subActive,
					   opt, widget );
	break;
    }
}
		

/*!
  Draw a metallic button, sunken if \a sunken is true, horizontal if
  /a horz is true.
*/

void MetalStyle::drawMetalButton( TQPainter *p, int x, int y, int w, int h,
				  bool sunken, bool horz, bool flat  ) const
{

    drawMetalFrame( p, x, y, w, h );
    drawMetalGradient( p, x, y, w, h, sunken, horz, flat );
}




void MetalStyle::drawMetalFrame( TQPainter *p, int x, int y, int w, int h ) const
{
    TQColor top1("#878769691515");
    TQColor top2("#C6C6B4B44949");

    TQColor bot2("#70705B5B1414");
    TQColor bot1("#56564A4A0E0E"); //first from the bottom


    int x2 = x + w - 1;
    int y2 = y + h - 1;

    //frame:

    p->setPen( top1 );
    p->drawLine( x, y2, x, y );
    p->drawLine( x, y, x2-1, y );
    p->setPen( top2 );
    p->drawLine( x+1, y2 -1, x+1, y+1 );
    p->drawLine( x+1, y+1 , x2-2, y+1 );

    p->setPen( bot1 );
    p->drawLine( x+1, y2, x2, y2 );
    p->drawLine( x2, y2, x2, y );
    p->setPen( bot2 );
    p->drawLine( x+1, y2-1, x2-1, y2-1 );
    p->drawLine( x2-1, y2-1, x2-1, y+1 );


}


void MetalStyle::drawMetalGradient( TQPainter *p, int x, int y, int w, int h,
				    bool sunken, bool horz, bool flat  ) const

{
    TQColor highlight("#E8E8DDDD6565");
    TQColor subh1("#CECEBDBD5151");
    TQColor subh2("#BFBFACAC4545");

    TQColor topgrad("#B9B9A5A54040");
    TQColor botgrad("#89896C6C1A1A");



    if ( flat && !sunken ) {
	    p->fillRect( x + 2, y + 2, w - 4,h -4, topgrad );
    } else {
	// highlight:
	int i = 0;
	int x1 = x + 2;
	int y1 = y + 2;
	int x2 = x + w - 1;
	int y2 = y + h - 1;
	if ( horz )
	    x2 = x2 - 2;
	else
	    y2 = y2 - 2;
	
#define DRAWLINE if (horz) \
                    p->drawLine( x1, y1+i, x2, y1+i ); \
		 else \
                    p->drawLine( x1+i, y1, x1+i, y2 ); \
                 i++;

	if ( !sunken ) {
	    p->setPen( highlight );
	    DRAWLINE;
	    DRAWLINE;
	    p->setPen( subh1 );
	    DRAWLINE;
	    p->setPen( subh2 );
	    DRAWLINE;
	}
	// gradient:
	int ng = (horz ? h : w) - 8; // how many lines for the gradient?
	
	int h1, h2, s1, s2, v1, v2;
	if ( !sunken ) {
	    topgrad.hsv( &h1, &s1, &v1 );
	    botgrad.hsv( &h2, &s2, &v2 );
	} else {
	    botgrad.hsv( &h1, &s1, &v1 );
	    topgrad.hsv( &h2, &s2, &v2 );
	}
	
	if ( ng > 1 ) {	
	    for ( int j =0; j < ng; j++ ) {
		p->setPen( TQColor( h1 + ((h2-h1)*j)/(ng-1),
				   s1 + ((s2-s1)*j)/(ng-1),
				   v1 + ((v2-v1)*j)/(ng-1),  TQColor::Hsv ) );
		DRAWLINE;
	    }
	} else if ( ng == 1 ) {
	    p->setPen( TQColor((h1+h2)/2, (s1+s2)/2, (v1+v2)/2, TQColor::Hsv) );
	    DRAWLINE;
	}
	if ( sunken ) {
	    p->setPen( subh2 );
	    DRAWLINE;
	    
	    p->setPen( subh1 );
	    DRAWLINE;
	    
	    p->setPen( highlight );
	    DRAWLINE;
	    DRAWLINE;
	}
    }    
}



int MetalStyle::pixelMetric( PixelMetric metric, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *w ) const
{
    switch ( metric ) {
    case PM_MenuBarFrameWidth:
	return 2;
    default:
	return TQWindowsStyle::pixelMetric( metric, ceData, elementFlags, w );
    }
}

#endif
