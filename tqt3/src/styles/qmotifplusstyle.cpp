/****************************************************************************
**
** Implementation of TQMotifPlusStyle class
**
** Created : 000727
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqmotifplusstyle.h"

#if !defined(TQT_NO_STYLE_MOTIFPLUS) || defined(QT_PLUGIN)

#include "ntqmenubar.h"
#include "ntqapplication.h"
#include "ntqpainter.h"
#include "ntqpalette.h"
#include "ntqframe.h"
#include "ntqpushbutton.h"
#include "ntqcheckbox.h"
#include "ntqradiobutton.h"
#include "ntqcombobox.h"
#include "ntqlineedit.h"
#include "ntqspinbox.h"
#include "ntqslider.h"
#include "ntqdrawutil.h"
#include "ntqscrollbar.h"
#include "ntqtabbar.h"
#include "ntqtoolbar.h"
#include "ntqguardedptr.h"
#include "ntqlayout.h"


struct TQMotifPlusStylePrivate
{
    TQMotifPlusStylePrivate()
        : hovering(false), sliderActive(false), mousePressed(false),
          scrollbarElement(0), lastElement(0), ref(1)
    { ; }

    bool hovering, sliderActive, mousePressed;
    int scrollbarElement, lastElement, ref;
    TQPoint mousePos;
};

static TQMotifPlusStylePrivate * singleton = 0;


static void drawMotifPlusShade(TQPainter *p,
			       const TQRect &r,
			       const TQColorGroup &g,
			       bool sunken, bool mouseover,
			       const TQBrush *fill = 0)
{
    TQPen oldpen = p->pen();
    TQPointArray a(4);
    TQColor button =
	mouseover ? g.midlight() : g.button();
    TQBrush brush =
	mouseover ? g.brush(TQColorGroup::Midlight) : g.brush(TQColorGroup::Button);
    int x, y, w, h;

    r.rect(&x, &y, &w, &h);

    if (sunken) p->setPen(g.dark()); else p->setPen(g.light());
    a.setPoint(0, x, y + h - 1);
    a.setPoint(1, x, y);
    a.setPoint(2, x, y);
    a.setPoint(3, x + w - 1, y);
    p->drawLineSegments(a);

    if (sunken) p->setPen(TQt::black); else p->setPen(button);
    a.setPoint(0, x + 1, y + h - 2);
    a.setPoint(1, x + 1, y + 1);
    a.setPoint(2, x + 1, y + 1);
    a.setPoint(3, x + w - 2, y + 1);
    p->drawLineSegments(a);

    if (sunken) p->setPen(button); else p->setPen(g.dark());
    a.setPoint(0, x + 2, y + h - 2);
    a.setPoint(1, x + w - 2, y + h - 2);
    a.setPoint(2, x + w - 2, y + h - 2);
    a.setPoint(3, x + w - 2, y + 2);
    p->drawLineSegments(a);

    if (sunken) p->setPen(g.light()); else p->setPen(TQt::black);
    a.setPoint(0, x + 1, y + h - 1);
    a.setPoint(1, x + w - 1, y + h - 1);
    a.setPoint(2, x + w - 1, y + h - 1);
    a.setPoint(3, x + w - 1, y);
    p->drawLineSegments(a);

    if (fill)
	p->fillRect(x + 2, y + 2, w - 4, h - 4, *fill);
    else
	p->fillRect(x + 2, y + 2, w - 4, h - 4, brush);

    p->setPen(oldpen);
}


/*!
    \class TQMotifPlusStyle ntqmotifplusstyle.h
    \brief The TQMotifPlusStyle class provides a more sophisticated Motif-ish look and feel.

    \ingroup appearance

    This class implements a Motif-ish look and feel with the more
    sophisticated bevelling as used by the GIMP Toolkit (GTK+) for
    Unix/X11.
*/

/*!
    Constructs a TQMotifPlusStyle

    If \a hoveringHighlight is true (the default), then the style will
    not highlight push buttons, checkboxes, radiobuttons, comboboxes,
    scrollbars or sliders.
*/
TQMotifPlusStyle::TQMotifPlusStyle(bool hoveringHighlight) : TQMotifStyle(true)
{
    if ( !singleton )
        singleton = new TQMotifPlusStylePrivate;
    else
        singleton->ref++;

    useHoveringHighlight = hoveringHighlight;
}

/*! \reimp */
TQMotifPlusStyle::~TQMotifPlusStyle()
{
    if ( singleton && singleton->ref-- <= 0) {
        delete singleton;
        singleton = 0;
    }
}


/*! \reimp */
void TQMotifPlusStyle::polish(TQPalette &)
{
}


/*! \reimp */
void TQMotifPlusStyle::polish(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr)
{
    if (ceData.widgetObjectTypes.contains("TQWidget")) {
#ifndef TQT_NO_FRAME
        if ((ceData.widgetObjectTypes.contains("TQFrame")) && (ceData.frameStyle == TQFrame::Panel)) {
            widgetActionRequest(ceData, elementFlags, ptr, WAR_FrameSetStyle, TQStyleWidgetActionRequestData(TQFrame::WinPanel));
        }
#endif

#ifndef TQT_NO_MENUBAR
        if ((ceData.widgetObjectTypes.contains("TQMenuBar")) && (ceData.frameStyle != TQFrame::NoFrame)) {
            widgetActionRequest(ceData, elementFlags, ptr, WAR_FrameSetStyle, TQStyleWidgetActionRequestData(TQFrame::StyledPanel | TQFrame::Raised));
        }
#endif

#ifndef TQT_NO_TOOLBAR
        if (ceData.widgetObjectTypes.contains("TQToolBar")) {
            widgetActionRequest(ceData, elementFlags, ptr, WAR_SetLayoutMargin, TQStyleWidgetActionRequestData(2));
        }
#endif
    }
    if (useHoveringHighlight) {
	if ((ceData.widgetObjectTypes.contains("TQButton")) || (ceData.widgetObjectTypes.contains("TQComboBox"))) {
	    installObjectEventHandler(ceData, elementFlags, ptr, this);
	}

	if ((ceData.widgetObjectTypes.contains("TQScrollBar")) || (ceData.widgetObjectTypes.contains("TQSlider"))) {
	    widgetActionRequest(ceData, elementFlags, ptr, WAR_EnableMouseTracking);
	    installObjectEventHandler(ceData, elementFlags, ptr, this);
	}
    }

    TQMotifStyle::polish(ceData, elementFlags, ptr);
}


/*! \reimp */
void TQMotifPlusStyle::unPolish(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr)
{
    removeObjectEventHandler(ceData, elementFlags, ptr, this);
    TQMotifStyle::unPolish(ceData, elementFlags, ptr);
}


/*! \reimp */
void TQMotifPlusStyle::applicationPolish(const TQStyleControlElementData&, ControlElementFlags, void *)
{
}


/*! \reimp */
void TQMotifPlusStyle::applicationUnPolish(const TQStyleControlElementData&, ControlElementFlags, void *)
{
}


/*! \reimp */
int TQMotifPlusStyle::pixelMetric(PixelMetric metric, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *widget) const
{
    int ret;

    switch (metric) {
    case PM_ScrollBarExtent:
	ret = 15;
	break;

    case PM_ButtonDefaultIndicator:
	ret = 5;
	break;

    case PM_ButtonMargin:
	ret = 4;
	break;

    case PM_SliderThickness:
	ret = 15;
	break;

    case PM_IndicatorWidth:
    case PM_IndicatorHeight:
	ret = 10;
	break;

    case PM_ExclusiveIndicatorWidth:
    case PM_ExclusiveIndicatorHeight:
	ret = 11;
	break;

    case PM_MenuIndicatorFrameHBorder:
    case PM_MenuIndicatorFrameVBorder:
    case PM_MenuIconIndicatorFrameHBorder:
    case PM_MenuIconIndicatorFrameVBorder:
	ret = 2;
	break;

    default:
	ret = TQMotifStyle::pixelMetric(metric, ceData, elementFlags, widget);
	break;
    }

    return ret;
}


/*! \reimp */
void TQMotifPlusStyle::drawPrimitive( PrimitiveElement pe,
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

    switch (pe) {
    case PE_HeaderSection:

    case PE_ButtonCommand:
    case PE_ButtonBevel:
    case PE_ButtonTool:
	if (flags & (Style_Down | Style_On | Style_Raised | Style_Sunken))
	    drawMotifPlusShade( p, r, cg, bool(flags & (Style_Down | Style_On)),
				bool(flags & Style_MouseOver));
	else if (flags & Style_MouseOver)
	    p->fillRect(r, cg.brush(TQColorGroup::Midlight));
	else
	    p->fillRect(r, cg.brush(TQColorGroup::Button));
	break;

    case PE_Panel:
    case PE_PanelPopup:
    case PE_PanelMenuBar:
    case PE_PanelDockWindow:
	if ( opt.lineWidth() )
	    drawMotifPlusShade( p, r, cg, (flags & Style_Sunken), (flags & Style_MouseOver));
	else if ( flags & Style_MouseOver )
	    p->fillRect(r, cg.brush(TQColorGroup::Midlight));
	else
	    p->fillRect(r, cg.brush(TQColorGroup::Button));
	break;

    case PE_SpinWidgetUp:
	drawPrimitive(PE_ArrowUp, p, ceData, elementFlags, r, cg, flags, opt);
	break;

    case PE_SpinWidgetDown:
	drawPrimitive(PE_ArrowDown, p, ceData, elementFlags, r, cg, flags, opt);
	break;

    case PE_Indicator:
	{
	    TQBrush fill;
	    if (flags & Style_On)
		fill = cg.brush(TQColorGroup::Mid);
	    else if (flags & Style_MouseOver)
		fill = cg.brush(TQColorGroup::Midlight);
	    else
		fill = cg.brush(TQColorGroup::Button);

	    if (flags & Style_NoChange) {
		qDrawPlainRect(p, r, cg.text(), 1, &fill);
		p->drawLine(r.topRight(), r.bottomLeft());
	    } else
		drawMotifPlusShade(p, r, cg, (flags & Style_On),
				   (flags & Style_MouseOver), &fill);
	    break;
	}

    case PE_ExclusiveIndicator:
	{
	    TQPen oldpen =  p->pen();
	    TQPointArray thick(8);
	    TQPointArray thin(4);
	    TQColor button = ((flags & Style_MouseOver) ? cg.midlight() : cg.button());
	    TQBrush brush = ((flags & Style_MouseOver) ?
			    cg.brush(TQColorGroup::Midlight) :
			    cg.brush(TQColorGroup::Button));
	    int x, y, w, h;
	    r.rect(&x, &y, &w, &h);

	    p->fillRect(x, y, w, h, brush);


	    if (flags & Style_On) {
		thick.setPoint(0, x, y + (h / 2));
		thick.setPoint(1, x + (w / 2), y);
		thick.setPoint(2, x + 1, y + (h / 2));
		thick.setPoint(3, x + (w / 2), y + 1);
		thick.setPoint(4, x + (w / 2), y);
		thick.setPoint(5, x + w - 1, y + (h / 2));
		thick.setPoint(6, x + (w / 2), y + 1);
		thick.setPoint(7, x + w - 2, y + (h / 2));
		p->setPen(cg.dark());
		p->drawLineSegments(thick);

		thick.setPoint(0, x + 1, y + (h / 2) + 1);
		thick.setPoint(1, x + (w / 2), y + h - 1);
		thick.setPoint(2, x + 2, y + (h / 2) + 1);
		thick.setPoint(3, x + (w / 2), y + h - 2);
		thick.setPoint(4, x + (w / 2), y + h - 1);
		thick.setPoint(5, x + w - 2, y + (h / 2) + 1);
		thick.setPoint(6, x + (w / 2), y + h - 2);
		thick.setPoint(7, x + w - 3, y + (h / 2) + 1);
		p->setPen(cg.light());
		p->drawLineSegments(thick);

		thin.setPoint(0, x + 2, y + (h / 2));
		thin.setPoint(1, x + (w / 2), y + 2);
		thin.setPoint(2, x + (w / 2), y + 2);
		thin.setPoint(3, x + w - 3, y + (h / 2));
		p->setPen(TQt::black);
		p->drawLineSegments(thin);

		thin.setPoint(0, x + 3, y + (h / 2) + 1);
		thin.setPoint(1, x + (w / 2), y + h - 3);
		thin.setPoint(2, x + (w / 2), y + h - 3);
		thin.setPoint(3, x + w - 4, y + (h / 2) + 1);
		p->setPen(cg.mid());
		p->drawLineSegments(thin);
	    } else {
		thick.setPoint(0, x, y + (h / 2));
		thick.setPoint(1, x + (w / 2), y);
		thick.setPoint(2, x + 1, y + (h / 2));
		thick.setPoint(3, x + (w / 2), y + 1);
		thick.setPoint(4, x + (w / 2), y);
		thick.setPoint(5, x + w - 1, y + (h / 2));
		thick.setPoint(6, x + (w / 2), y + 1);
		thick.setPoint(7, x + w - 2, y + (h / 2));
		p->setPen(cg.light());
		p->drawLineSegments(thick);

		thick.setPoint(0, x + 2, y + (h / 2) + 1);
		thick.setPoint(1, x + (w / 2), y + h - 2);
		thick.setPoint(2, x + 3, y + (h / 2) + 1);
		thick.setPoint(3, x + (w / 2), y + h - 3);
		thick.setPoint(4, x + (w / 2), y + h - 2);
		thick.setPoint(5, x + w - 3, y + (h / 2) + 1);
		thick.setPoint(6, x + (w / 2), y + h - 3);
		thick.setPoint(7, x + w - 4, y + (h / 2) + 1);
		p->setPen(cg.dark());
		p->drawLineSegments(thick);

		thin.setPoint(0, x + 2, y + (h / 2));
		thin.setPoint(1, x + (w / 2), y + 2);
		thin.setPoint(2, x + (w / 2), y + 2);
		thin.setPoint(3, x + w - 3, y + (h / 2));
		p->setPen(button);
		p->drawLineSegments(thin);

		thin.setPoint(0, x + 1, y + (h / 2) + 1);
		thin.setPoint(1, x + (w / 2), y + h - 1);
		thin.setPoint(2, x + (w / 2), y + h - 1);
		thin.setPoint(3, x + w - 2, y + (h / 2) + 1);
		p->setPen(TQt::black);
		p->drawLineSegments(thin);
	    }

	    p->setPen(oldpen);
	    break;
	}



    case PE_ArrowDown:
    case PE_ArrowLeft:
    case PE_ArrowRight:
    case PE_ArrowUp:
	{
	    TQPen oldpen = p->pen();
	    TQBrush oldbrush = p->brush();
	    TQPointArray poly(3);
	    TQColor button = (flags & Style_MouseOver) ? cg.midlight() : cg.button();
	    bool down = (flags & Style_Down);
	    int x, y, w, h;
	    r.rect(&x, &y, &w, &h);

	    p->save();
	    p->setBrush(button);

	    switch (pe) {
	    case PE_ArrowUp:
		{
		    poly.setPoint(0, x + (w / 2), y );
		    poly.setPoint(1, x, y + h - 1);
		    poly.setPoint(2, x + w - 1, y + h - 1);
		    p->drawPolygon(poly);

		    if (down)
			p->setPen(button);
		    else
			p->setPen(cg.dark());
		    p->drawLine(x + 1, y + h - 2, x + w - 2, y + h - 2);

		    if (down)
			p->setPen(cg.light());
		    else
			p->setPen(black);
		    p->drawLine(x, y + h - 1, x + w - 1, y + h - 1);

		    if (down)
			p->setPen(button);
		    else
			p->setPen(cg.dark());
		    p->drawLine(x + w - 2, y + h - 1, x + (w / 2), y + 1);

		    if (down)
			p->setPen(cg.light());
		    else
			p->setPen(black);
		    p->drawLine(x + w - 1, y + h - 1, x + (w / 2), y);

		    if (down)
			p->setPen(black);
		    else
			p->setPen(button);
		    p->drawLine(x + (w / 2), y + 1, x + 1, y + h - 1);

		    if (down)
			p->setPen(cg.dark());
		    else
			p->setPen(cg.light());
		    p->drawLine(x + (w / 2), y, x, y + h - 1);
		    break;
		}

	    case PE_ArrowDown:
		{
		    poly.setPoint(0, x + w - 1, y);
		    poly.setPoint(1, x, y);
		    poly.setPoint(2, x + (w / 2), y + h - 1);
		    p->drawPolygon(poly);

		    if (down)
			p->setPen(black);
		    else
			p->setPen(button);
		    p->drawLine(x + w - 2, y + 1, x + 1, y + 1);

		    if (down)
			p->setPen(cg.dark());
		    else
			p->setPen(cg.light());
		    p->drawLine(x + w - 1, y, x, y);

		    if (down)
			p->setPen(black);
		    else
			p->setPen(button);
		    p->drawLine(x + 1, y, x + (w / 2), y + h - 2);

		    if (down)
			p->setPen(cg.dark());
		    else
			p->setPen(cg.light());
		    p->drawLine(x, y, x + (w / 2), y + h - 1);

		    if (down)
			p->setPen(button);
		    else
			p->setPen(cg.dark());
		    p->drawLine(x + (w / 2), y + h - 2, x + w - 2, y);

		    if (down)
			p->setPen(cg.light());
		    else
			p->setPen(black);
		    p->drawLine(x + (w / 2), y + h - 1, x + w - 1, y);
		    break;
		}

	    case PE_ArrowLeft:
		{
		    poly.setPoint(0, x, y + (h / 2));
		    poly.setPoint(1, x + w - 1, y + h - 1);
		    poly.setPoint(2, x + w - 1, y);
		    p->drawPolygon(poly);

		    if (down)
			p->setPen(button);
		    else
			p->setPen(cg.dark());
		    p->drawLine(x + 1, y + (h / 2), x + w - 1, y + h - 1);

		    if (down)
			p->setPen(cg.light());
		    else
			p->setPen(black);
		    p->drawLine(x, y + (h / 2), x + w - 1, y + h - 1);

		    if (down)
			p->setPen(button);
		    else
			p->setPen(cg.dark());
		    p->drawLine(x + w - 2, y + h - 1, x + w - 2, y + 1);

		    if (down)
			p->setPen(cg.light());
		    else
			p->setPen(black);
		    p->drawLine(x + w - 1, y + h - 1, x + w - 1, y);

		    if (down)
			p->setPen(black);
		    else
			p->setPen(button);
		    p->drawLine(x + w - 1, y + 1, x + 1, y + (h / 2));

		    if (down)
			p->setPen(cg.dark());
		    else
			p->setPen(cg.light());
		    p->drawLine(x + w - 1, y, x, y + (h / 2));
		    break;
		}

	    case PE_ArrowRight:
		{
		    poly.setPoint(0, x + w - 1, y + (h / 2));
		    poly.setPoint(1, x, y);
		    poly.setPoint(2, x, y + h - 1);
		    p->drawPolygon(poly);

		    if (down)
			p->setPen(black);
		    else
			p->setPen(button);
		    p->drawLine( x + w - 1, y + (h / 2), x + 1, y + 1);

		    if (down)
			p->setPen(cg.dark());
		    else
			p->setPen(cg.light());
		    p->drawLine(x + w - 1, y + (h / 2), x, y);

		    if (down)
			p->setPen(black);
		    else
			p->setPen(button);
		    p->drawLine(x + 1, y + 1, x + 1, y + h - 2);

		    if (down)
			p->setPen(cg.dark());
		    else
			p->setPen(cg.light());
		    p->drawLine(x, y, x, y + h - 1);

		    if (down)
			p->setPen(button);
		    else
			p->setPen(cg.dark());
		    p->drawLine(x + 1, y + h - 2, x + w - 1, y + (h / 2));

		    if (down)
			p->setPen(cg.light());
		    else
			p->setPen(black);
		    p->drawLine(x, y + h - 1, x + w - 1, y + (h / 2));
		    break;
		}

	    default:
		break;
	    }

	    p->restore();
	    p->setBrush(oldbrush);
	    p->setPen(oldpen);
   	    break;
	}

	case PE_PanelScrollBar:
	    {
		drawMotifPlusShade(p, r, cg, true, false, &cg.brush(TQColorGroup::Mid));
		break;
	    }

	case PE_MenuItemIndicatorFrame:
	    {
		// Draw nothing
		break;
	    }
	case PE_MenuItemIndicatorIconFrame:
	    {
		int x, y, w, h;
		r.rect(&x, &y, &w, &h);
		int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags, opt, NULL, NULL);

		TQRect vrect = visualRect( TQRect( x+2, y+2, checkcol, h-2 ), r );

		qDrawShadePanel( p, vrect.x(), y+2, checkcol, h-2*2, cg, true, 1, &cg.brush( TQColorGroup::Midlight ) );
		break;
	    }

	case PE_MenuItemIndicatorCheck:
	    {
		int x, y, w, h;
		r.rect(&x, &y, &w, &h);
		int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags, opt, NULL, NULL);

		TQRect vrect = visualRect( TQRect( x+2, y+2, checkcol, h-2 ), r );

		SFlags cflags = Style_Default;
		if (! dis)
		    cflags |= Style_Enabled;
		if (act)
		    cflags |= Style_On;

		drawPrimitive(PE_CheckMark, p, ceData, elementFlags, vrect, cg, cflags);
		break;
	    }

    default:
	TQMotifStyle::drawPrimitive(pe, p, ceData, elementFlags, r, cg, flags, opt);
	break;
    }
}


/*! \reimp
*/
void TQMotifPlusStyle::drawControl( ControlElement element,
				   TQPainter *p,
				   const TQStyleControlElementData &ceData,
				   ControlElementFlags elementFlags,
				   const TQRect &r,
				   const TQColorGroup &cg,
				   SFlags flags,
				   const TQStyleOption& opt,
				   const TQWidget *widget) const
{
    switch (element) {
    case CE_PushButton:
	{
#ifndef TQT_NO_PUSHBUTTON
	    TQRect br = r;
	    int dbi = pixelMetric(PM_ButtonDefaultIndicator, ceData, elementFlags, widget);

	    if ((elementFlags & CEF_IsDefault) || (elementFlags & CEF_AutoDefault)) {
		if (elementFlags & CEF_IsDefault)
		    drawMotifPlusShade(p, br, cg, true, false,
				       &cg.brush(TQColorGroup::Background));

		br.setCoords(br.left()   + dbi,
			     br.top()    + dbi,
			     br.right()  - dbi,
			     br.bottom() - dbi);
	    }

	    if (flags & Style_HasFocus)
		br.addCoords(1, 1, -1, -1);
	    p->save();
	    p->setBrushOrigin( -ceData.bgOffset.x(),
			       -ceData.bgOffset.y() );
	    drawPrimitive(PE_ButtonCommand, p, ceData, elementFlags, br, cg, flags);
	    p->restore();
#endif
	    break;
	}

    case CE_CheckBoxLabel:
	{
#ifndef TQT_NO_CHECKBOX
	    if (flags & Style_MouseOver) {
		TQRegion r(ceData.rect);
		r -= visualRect(subRect(SR_CheckBoxIndicator, ceData, elementFlags, widget), ceData, elementFlags);
		p->setClipRegion(r);
		p->fillRect(ceData.rect, cg.brush(TQColorGroup::Midlight));
		p->setClipping(false);
	    }

	    int alignment = TQApplication::reverseLayout() ? AlignRight : AlignLeft;
	    drawItem(p, r, alignment | AlignVCenter | ShowPrefix, cg,
		     flags & Style_Enabled, (ceData.fgPixmap.isNull())?NULL:&ceData.fgPixmap, ceData.text);

	    if (elementFlags & CEF_HasFocus) {
		TQRect fr = visualRect(subRect(SR_CheckBoxFocusRect, ceData, elementFlags, widget), ceData, elementFlags);
		drawPrimitive(PE_FocusRect, p, ceData, elementFlags, fr, cg, flags);
	    }
#endif
	    break;
	}

    case CE_RadioButtonLabel:
	{
#ifndef TQT_NO_RADIOBUTTON
	    if (flags & Style_MouseOver) {
		TQRegion r(ceData.rect);
		r -= visualRect(subRect(SR_RadioButtonIndicator, ceData, elementFlags, widget), ceData, elementFlags);
		p->setClipRegion(r);
		p->fillRect(ceData.rect, cg.brush(TQColorGroup::Midlight));
		p->setClipping(false);
	    }

	    int alignment = TQApplication::reverseLayout() ? AlignRight : AlignLeft;
	    drawItem(p, r, alignment | AlignVCenter | ShowPrefix, cg,
		     flags & Style_Enabled, (ceData.fgPixmap.isNull())?NULL:&ceData.fgPixmap, ceData.text);

	    if (elementFlags & CEF_HasFocus) {
		TQRect fr = visualRect(subRect(SR_RadioButtonFocusRect, ceData, elementFlags, widget), ceData, elementFlags);
		drawPrimitive(PE_FocusRect, p, ceData, elementFlags, fr, cg, flags);
	    }
#endif
	    break;
	}

    case CE_MenuBarItem:
	{
#ifndef TQT_NO_MENUDATA
	    if (opt.isDefault())
		break;

	    TQMenuItem *mi = opt.menuItem();
	    if ((flags & Style_Enabled) && (flags & Style_Active))
		drawMotifPlusShade(p, r, cg, false, true);
	    else
		p->fillRect(r, cg.button());

	    drawItem(p, r, AlignCenter | ShowPrefix | DontClip | SingleLine,
		     cg, flags & Style_Enabled, mi->pixmap(), mi->text(), -1,
		     &cg.buttonText());
#endif
	    break;
	}


#ifndef TQT_NO_POPUPMENU
    case CE_PopupMenuItem:
	{
	    if (! widget || opt.isDefault())
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

	    if (checkable)
		maxpmw = TQMAX(maxpmw, 15);

	    int checkcol = maxpmw;

	    if (mi && mi->isSeparator()) {
		p->setPen( cg.dark() );
		p->drawLine( x, y, x+w, y );
		p->setPen( cg.light() );
		p->drawLine( x, y+1, x+w, y+1 );
		return;
	    }

	    if ( act && !dis )
		drawMotifPlusShade(p, TQRect(x, y, w, h), cg, false, true);
	    else
		p->fillRect(x, y, w, h, cg.brush( TQColorGroup::Button ));

	    if ( !mi )
		return;

	    TQRect vrect = visualRect( TQRect( x+2, y+2, checkcol, h-2 ), r );
	    if ( mi->isChecked() ) {
		if ( mi->iconSet() ) {
		    drawPrimitive(PE_MenuItemIndicatorIconFrame, p, ceData, elementFlags, r, cg, flags, opt);
		}
	    } else if ( !act ) {
		p->fillRect(vrect,
			    cg.brush( TQColorGroup::Button ));
	    }

	    if ( mi->iconSet() ) {              // draw iconset
		TQIconSet::Mode mode = (!dis) ? TQIconSet::Normal : TQIconSet::Disabled;

		if (act && !dis)
		    mode = TQIconSet::Active;

		TQPixmap pixmap;
		if ( checkable && mi->isChecked() )
		    pixmap = mi->iconSet()->pixmap( TQIconSet::Small, mode,
						    TQIconSet::On );
		else
		    pixmap = mi->iconSet()->pixmap( TQIconSet::Small, mode );

		int pixw = pixmap.width();
		int pixh = pixmap.height();

		TQRect pmr( 0, 0, pixw, pixh );

		pmr.moveCenter(vrect.center());

		p->setPen( cg.text() );
		p->drawPixmap( pmr.topLeft(), pixmap );

	    } else if (checkable) {
		if (mi->isChecked()) {
		    drawPrimitive(PE_MenuItemIndicatorCheck, p, ceData, elementFlags, r, cg, flags, opt);
		}
	    }

	    p->setPen( cg.buttonText() );

	    TQColor discol;
	    if (dis) {
		discol = cg.text();
		p->setPen( discol );
	    }

	    vrect = visualRect( TQRect(x + checkcol + 4, y + 2,
				      w - checkcol - tab - 3, h - 4), r );
	    if (mi->custom()) {
		p->save();
		mi->custom()->paint(p, cg, act, !dis, vrect.x(), y + 2,
				    w - checkcol - tab - 3, h - 4);
		p->restore();
	    }

	    TQString s = mi->text();
	    if ( !s.isNull() ) {                        // draw text
		int t = s.find( '\t' );
		int m = 2;
		int text_flags = AlignVCenter|ShowPrefix | DontClip | SingleLine;
		text_flags |= (TQApplication::reverseLayout() ? AlignRight : AlignLeft );
		if ( t >= 0 ) {                         // draw tab text
		    TQRect vr = visualRect( TQRect(x+w-tab-2-2,
						 y+m, tab, h-2*m), r );
		    p->drawText( vr.x(),
				 y+m, tab, h-2*m, text_flags, s.mid( t+1 ) );
		}
		p->drawText(vrect.x(), y + 2, w - checkcol -tab - 3, h - 4,
			    text_flags, s, t);
	    } else if (mi->pixmap()) {
		TQPixmap *pixmap = mi->pixmap();

		if (pixmap->depth() == 1) p->setBackgroundMode(OpaqueMode);
		TQRect vr = visualRect( TQRect( x + checkcol + 2, y + 2, w - checkcol - 1, h - 4 ), r );
		p->drawPixmap(vr.x(), y + 2, *pixmap);
		if (pixmap->depth() == 1) p->setBackgroundMode(TransparentMode);
	    }

	    if (mi->popup()) {
		int hh = h / 2;
		TQStyle::PrimitiveElement arrow = (TQApplication::reverseLayout() ? PE_ArrowLeft : PE_ArrowRight);
		vrect = visualRect( TQRect(x + w - hh - 6, y + (hh / 2), hh, hh), r );
		drawPrimitive(arrow, p,
			      ceData, elementFlags,
			      vrect, cg,
			      ((act && !dis) ?
			       Style_Down : Style_Default) |
			      ((!dis) ? Style_Enabled : Style_Default));
	    }
	    break;
	}
#endif // TQT_NO_POPUPMENU

    case CE_TabBarTab:
	{
#ifndef TQT_NO_TABBAR
	    bool selected = flags & Style_Selected;

	    TQColorGroup g = ceData.colorGroup;
	    TQPen oldpen = p->pen();
	    TQRect fr(r);

	    if (! selected) {
		if (ceData.tabBarData.shape == TQTabBar::RoundedAbove ||
		    ceData.tabBarData.shape == TQTabBar::TriangularAbove) {
		    fr.setTop(fr.top() + 2);
		} else {
		    fr.setBottom(fr.bottom() - 2);
		}
	    }

	    fr.setWidth(fr.width() - 3);

	    p->fillRect(fr.left() + 1, fr.top() + 1, fr.width() - 2, fr.height() - 2,
			(selected) ? cg.brush(TQColorGroup::Button)
			: cg.brush(TQColorGroup::Mid));

	    if (ceData.tabBarData.shape == TQTabBar::RoundedAbove) {
		// "rounded" tabs on top
		fr.setBottom(fr.bottom() - 1);

		p->setPen(g.light());
		p->drawLine(fr.left(), fr.top() + 1,
			    fr.left(), fr.bottom() - 1);
		p->drawLine(fr.left() + 1, fr.top(),
			    fr.right() - 1, fr.top());
		if (! selected)
		    p->drawLine(fr.left(), fr.bottom(),
				fr.right() + 3, fr.bottom());

		if (fr.left() == 0)
		    p->drawLine(fr.left(), fr.bottom(),
				fr.left(), fr.bottom() + 1);

		p->setPen(g.dark());
		p->drawLine(fr.right() - 1, fr.top() + 2,
			    fr.right() - 1, fr.bottom() - 1);

		p->setPen(black);
		p->drawLine(fr.right(), fr.top() + 1,
			    fr.right(), fr.bottom() - 1);
	    } else if (ceData.tabBarData.shape == TQTabBar::RoundedBelow) {
		// "rounded" tabs on bottom
		fr.setTop(fr.top() + 1);

		p->setPen(g.dark());
		p->drawLine(fr.right() + 3, fr.top() - 1,
			    fr.right() - 1, fr.top() - 1);
		p->drawLine(fr.right() - 1, fr.top(),
			    fr.right() - 1, fr.bottom() - 2);
		p->drawLine(fr.right() - 1, fr.bottom() - 2,
			    fr.left() + 2,  fr.bottom() - 2);
		if (! selected) {
		    p->drawLine(fr.right(), fr.top() - 1,
				fr.left() + 1,  fr.top() - 1);

		    if (fr.left() != 0)
			p->drawPoint(fr.left(), fr.top() - 1);
		}

		p->setPen(black);
		p->drawLine(fr.right(), fr.top(),
			    fr.right(), fr.bottom() - 2);
		p->drawLine(fr.right() - 1, fr.bottom() - 1,
			    fr.left(), fr.bottom() - 1);
		if (! selected)
		    p->drawLine(fr.right() + 3, fr.top(),
				fr.left(), fr.top());
		else
		    p->drawLine(fr.right() + 3, fr.top(),
				fr.right(), fr.top());

		p->setPen(g.light());
		p->drawLine(fr.left(), fr.top() + 1,
			    fr.left(), fr.bottom() - 2);

		if (selected) {
		    p->drawPoint(fr.left(), fr.top());
		    if (fr.left() == 0)
			p->drawPoint(fr.left(), fr.top() - 1);

		    p->setPen(g.button());
		    p->drawLine(fr.left() + 2, fr.top() - 1,
				fr.left() + 1, fr.top() - 1);
		}
	    } else
		// triangular drawing code
		TQMotifStyle::drawControl(element, p, ceData, elementFlags, r, cg, flags, opt, widget);

	    p->setPen(oldpen);
#endif
	    break;
	}

    default:
	TQMotifStyle::drawControl(element, p, ceData, elementFlags, r, cg, flags, opt, widget);
	break;
    }
}


/*! \reimp
*/
TQRect TQMotifPlusStyle::subRect(SubRect r, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *widget) const
{
    TQRect rect;

    switch (r) {
    case SR_PushButtonFocusRect:
	{
#ifndef TQT_NO_PUSHBUTTON
	    int dfi = pixelMetric(PM_ButtonDefaultIndicator, ceData, elementFlags, widget);

	    rect = ceData.rect;
	    if ((elementFlags & CEF_IsDefault) || (elementFlags & CEF_AutoDefault))
		rect.addCoords(dfi, dfi, -dfi, -dfi);
#endif
	    break;
	}

    case SR_CheckBoxIndicator:
	{
	    int h = pixelMetric( PM_IndicatorHeight, ceData, elementFlags );
	    rect.setRect(( ceData.rect.height() - h ) / 2,
			 ( ceData.rect.height() - h ) / 2,
			 pixelMetric( PM_IndicatorWidth, ceData, elementFlags ), h );
	    break;
	}

    case SR_RadioButtonIndicator:
	{
	    int h = pixelMetric( PM_ExclusiveIndicatorHeight, ceData, elementFlags );
	    rect.setRect( ( ceData.rect.height() - h ) / 2,
			  ( ceData.rect.height() - h ) / 2,
			  pixelMetric( PM_ExclusiveIndicatorWidth, ceData, elementFlags ), h );
	    break;
	}

    case SR_CheckBoxFocusRect:
    case SR_RadioButtonFocusRect:
       	rect = ceData.rect;
	break;

    case SR_ComboBoxFocusRect:
	{
#ifndef TQT_NO_COMBOBOX
	    if (elementFlags & CEF_IsEditable) {
		rect = querySubControlMetrics(CC_ComboBox, ceData, elementFlags,
					      SC_ComboBoxEditField, TQStyleOption::Default, widget);
		rect.addCoords(-3, -3, 3, 3);
	    } else
		rect = ceData.rect;
#endif
	    break;
	}

    case SR_SliderFocusRect:
	{
#ifndef TQT_NO_SLIDER
	    int tickOffset = pixelMetric( PM_SliderTickmarkOffset, ceData, elementFlags, widget );
	    int thickness = pixelMetric( PM_SliderControlThickness, ceData, elementFlags, widget );
	    int x, y, wi, he;

	    if ( ceData.orientation == Horizontal ) {
		x = 0;
		y = tickOffset;
		wi = ceData.rect.width();
		he = thickness;
	    } else {
		x = tickOffset;
		y = 0;
		wi = thickness;
		he = ceData.rect.height();
	    }

	    rect.setRect(x, y, wi, he);
#endif
	    break;
	}

    default:
	rect = TQMotifStyle::subRect(r, ceData, elementFlags, widget);
	break;
    }

    return rect;
}


/*! \reimp */
void TQMotifPlusStyle::drawComplexControl(ComplexControl control,
			    TQPainter *p,
			    const TQStyleControlElementData &ceData,
			    ControlElementFlags elementFlags,
			    const TQRect &r,
			    const TQColorGroup &cg,
			    SFlags flags,
			    SCFlags controls,
			    SCFlags active,
			    const TQStyleOption& opt,
			    const TQWidget *widget ) const
{
    switch (control) {
    case CC_ScrollBar:
	{
#ifndef TQT_NO_SCROLLBAR
	    TQRect addline, subline, addpage, subpage, slider, first, last;
	    bool maxedOut = (ceData.minSteps == ceData.maxSteps);

	    subline = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarSubLine, opt, widget);
	    addline = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarAddLine, opt, widget);
	    subpage = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarSubPage, opt, widget);
	    addpage = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarAddPage, opt, widget);
	    slider  = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarSlider,  opt, widget);
	    first   = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarFirst,   opt, widget);
	    last    = querySubControlMetrics(control, ceData, elementFlags, SC_ScrollBarLast,    opt, widget);

	    bool skipUpdate = false;
	    if (singleton->hovering) {
		if (addline.contains(singleton->mousePos)) {
		    skipUpdate =
			(singleton->scrollbarElement == SC_ScrollBarAddLine);
		    singleton->scrollbarElement = SC_ScrollBarAddLine;
		} else if (subline.contains(singleton->mousePos)) {
		    skipUpdate =
			(singleton->scrollbarElement == SC_ScrollBarSubLine);
		    singleton->scrollbarElement = SC_ScrollBarSubLine;
		} else if (slider.contains(singleton->mousePos)) {
		    skipUpdate =
			(singleton->scrollbarElement == SC_ScrollBarSlider);
		    singleton->scrollbarElement = SC_ScrollBarSlider;
		} else {
		    skipUpdate =
			(singleton->scrollbarElement == 0);
		    singleton->scrollbarElement = 0;
		}
	    } else
		singleton->scrollbarElement = 0;

	    if (skipUpdate && singleton->scrollbarElement == singleton->lastElement)
		break;

	    singleton->lastElement = singleton->scrollbarElement;

	    if (controls == (SC_ScrollBarAddLine | SC_ScrollBarSubLine |
			     SC_ScrollBarAddPage | SC_ScrollBarSubPage |
			     SC_ScrollBarFirst | SC_ScrollBarLast | SC_ScrollBarSlider))
		drawPrimitive(PE_PanelScrollBar, p, ceData, elementFlags, ceData.rect, cg,
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarLast) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : Style_Default));

	    if ((controls & SC_ScrollBarSubLine) && subline.isValid())
		drawPrimitive(PE_ScrollBarSubLine, p, ceData, elementFlags, subline, cg,
			      ((active == SC_ScrollBarSubLine ||
				singleton->scrollbarElement == SC_ScrollBarSubLine) ?
			       Style_MouseOver: Style_Default) |
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarSubLine) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : Style_Default));
	    if ((controls & SC_ScrollBarAddLine) && addline.isValid())
		drawPrimitive(PE_ScrollBarAddLine, p, ceData, elementFlags, addline, cg,
			      ((active == SC_ScrollBarAddLine ||
				singleton->scrollbarElement == SC_ScrollBarAddLine) ?
			       Style_MouseOver: Style_Default) |
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarAddLine) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : Style_Default));
	    if ((controls & SC_ScrollBarSubPage) && subpage.isValid())
		drawPrimitive(PE_ScrollBarSubPage, p, ceData, elementFlags, subpage, cg,
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarSubPage) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : Style_Default));
	    if ((controls & SC_ScrollBarAddPage) && addpage.isValid())
		drawPrimitive(PE_ScrollBarAddPage, p, ceData, elementFlags, addpage, cg,
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarAddPage) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : Style_Default));
	    if ((controls & SC_ScrollBarFirst) && first.isValid())
		drawPrimitive(PE_ScrollBarFirst, p, ceData, elementFlags, first, cg,
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarFirst) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : Style_Default));
	    if ((controls & SC_ScrollBarLast) && last.isValid())
		drawPrimitive(PE_ScrollBarLast, p, ceData, elementFlags, last, cg,
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((active == SC_ScrollBarLast) ?
			       Style_Down : Style_Default) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : Style_Default));
	    if ((controls & SC_ScrollBarSlider) && slider.isValid()) {
		drawPrimitive(PE_ScrollBarSlider, p, ceData, elementFlags, slider, cg,
			      ((active == SC_ScrollBarSlider ||
				singleton->scrollbarElement == SC_ScrollBarSlider) ?
			       Style_MouseOver: Style_Default) |
			      ((maxedOut) ? Style_Default : Style_Enabled) |
			      ((ceData.orientation == TQt::Horizontal) ?
			       Style_Horizontal : Style_Default));

		// ### perhaps this should not be able to accept focus if maxedOut?
		if (elementFlags & CEF_HasFocus) {
		    TQRect fr(slider.x() + 2, slider.y() + 2,
			     slider.width() - 5, slider.height() - 5);
		    drawPrimitive(PE_FocusRect, p, ceData, elementFlags, fr, cg, Style_Default);
		}
	    }
#endif
	    break;
	}

    case CC_ComboBox:
	{
#ifndef TQT_NO_COMBOBOX
	    TQRect editfield, arrow;
	    editfield =
		visualRect(querySubControlMetrics(CC_ComboBox,
						  ceData, elementFlags,
						  SC_ComboBoxEditField,
						  opt, widget), ceData, elementFlags);
	    arrow =
		visualRect(querySubControlMetrics(CC_ComboBox,
						  ceData, elementFlags,
						  SC_ComboBoxArrow,
						  opt, widget), ceData, elementFlags);

	    if (elementFlags & CEF_IsEditable) {
		if (controls & SC_ComboBoxEditField && editfield.isValid()) {
		    editfield.addCoords(-3, -3, 3, 3);
		    if (elementFlags & CEF_HasFocus)
			editfield.addCoords(1, 1, -1, -1);
		    drawMotifPlusShade(p, editfield, cg, true, false,
				       ((elementFlags & CEF_IsEnabled) ?
					&cg.brush(TQColorGroup::Base) :
					&cg.brush(TQColorGroup::Background)));
		}

		if (controls & SC_ComboBoxArrow && arrow.isValid()) {
		    drawMotifPlusShade(p, arrow, cg, (active == SC_ComboBoxArrow),
				       (flags & Style_MouseOver));

		    int space = (r.height() - 13) / 2;
		    arrow.addCoords(space, space, -space, -space);

		    if (active == SC_ComboBoxArrow)
			flags |= Style_Sunken;
		    drawPrimitive(PE_ArrowDown, p, ceData, elementFlags, arrow, cg, flags);
		}
	    } else {
		if (controls & SC_ComboBoxEditField && editfield.isValid()) {
		    editfield.addCoords(-3, -3, 3, 3);
		    if (elementFlags & CEF_HasFocus)
			editfield.addCoords(1, 1, -1, -1);
		    drawMotifPlusShade(p, editfield, cg, false,
				       (flags & Style_MouseOver));
		}

		if (controls & SC_ComboBoxArrow && arrow.isValid())
		    drawMotifPlusShade(p, arrow, cg, false, (flags & Style_MouseOver));
	    }

	    if ((elementFlags & CEF_HasFocus) ||
		((elementFlags & CEF_IsEditable) && (ceData.comboBoxLineEditFlags & CEF_HasFocus))) {
		TQRect fr = visualRect(subRect(SR_ComboBoxFocusRect, ceData, elementFlags, widget), ceData, elementFlags);
		drawPrimitive(PE_FocusRect, p, ceData, elementFlags, fr, cg, flags);
	    }
#endif
	    break;
	}

    case CC_SpinWidget:
	{
#ifndef TQT_NO_SPINWIDGET
	    SFlags flags = Style_Default;

	    if (controls & SC_SpinWidgetFrame)
		drawMotifPlusShade(p, r, cg, true, false, &cg.brush(TQColorGroup::Base));

	    if (controls & SC_SpinWidgetUp) {
		flags = Style_Enabled;
		if (active == SC_SpinWidgetUp )
		    flags |= Style_Down;

		PrimitiveElement pe;
		if ( ceData.spinWidgetData.buttonSymbols == TQSpinWidget::PlusMinus )
		    pe = PE_SpinWidgetPlus;
		else
		    pe = PE_SpinWidgetUp;

		TQRect re = ceData.spinWidgetData.upRect;
		TQColorGroup ucg = ceData.spinWidgetData.upEnabled ? cg : ceData.palette.disabled();
		drawPrimitive(pe, p, ceData, elementFlags, re, ucg, flags);
	    }

	    if (controls & SC_SpinWidgetDown) {
		flags = Style_Enabled;
		if (active == SC_SpinWidgetDown )
		    flags |= Style_Down;

		PrimitiveElement pe;
		if ( ceData.spinWidgetData.buttonSymbols == TQSpinWidget::PlusMinus )
		    pe = PE_SpinWidgetMinus;
		else
		    pe = PE_SpinWidgetDown;

		TQRect re = ceData.spinWidgetData.downRect;
		TQColorGroup dcg = ceData.spinWidgetData.downEnabled ? cg : ceData.palette.disabled();
		drawPrimitive(pe, p, ceData, elementFlags, re, dcg, flags);
	    }
#endif
	    break;
	}

    case CC_Slider:
	{
#ifndef TQT_NO_SLIDER
	    bool mouseover = (flags & Style_MouseOver);

	    TQRect groove = querySubControlMetrics(CC_Slider, ceData, elementFlags, SC_SliderGroove,
						  opt, widget),
		  handle = querySubControlMetrics(CC_Slider, ceData, elementFlags, SC_SliderHandle,
						  opt, widget);

	    if ((controls & SC_SliderGroove) && groove.isValid()) {
		drawMotifPlusShade(p, groove, cg, true, false,
				   &cg.brush(TQColorGroup::Mid));

		if ( flags & Style_HasFocus ) {
		    TQRect fr = subRect( SR_SliderFocusRect, ceData, elementFlags, widget );
		    drawPrimitive( PE_FocusRect, p, ceData, elementFlags, fr, cg, flags );
		}
	    }

	    if ((controls & SC_SliderHandle) && handle.isValid()) {
		if ((mouseover && handle.contains(singleton->mousePos)) ||
		    singleton->sliderActive)
		    flags |= Style_MouseOver;
		else
		    flags &= ~Style_MouseOver;
		drawPrimitive(PE_ButtonBevel, p, ceData, elementFlags, handle, cg, flags | Style_Raised);

		if ( ceData.orientation == Horizontal ) {
		    TQCOORD mid = handle.x() + handle.width() / 2;
		    qDrawShadeLine( p, mid,  handle.y() + 1, mid ,
				    handle.y() + handle.height() - 3,
				    cg, true, 1);
		} else {
		    TQCOORD mid = handle.y() + handle.height() / 2;
		    qDrawShadeLine( p, handle.x() + 1, mid,
				    handle.x() + handle.width() - 3, mid,
				    cg, true, 1);
		}
	    }

	    if (controls & SC_SliderTickmarks)
		TQMotifStyle::drawComplexControl(control, p, ceData, elementFlags, r, cg, flags,
						SC_SliderTickmarks, active, opt, widget);
#endif
	    break;
	}

    default:
	TQMotifStyle::drawComplexControl(control, p, ceData, elementFlags, r, cg, flags,
					controls, active, opt, widget);
    }
}


/*! \reimp
*/
TQRect TQMotifPlusStyle::querySubControlMetrics(ComplexControl control,
					      const TQStyleControlElementData &ceData,
					      ControlElementFlags elementFlags,
					      SubControl subcontrol,
					      const TQStyleOption& opt,
					      const TQWidget *widget) const
{
    switch (control) {
    case CC_SpinWidget: {
	    int fw = pixelMetric( PM_SpinBoxFrameWidth, ceData, elementFlags, 0 );
	    TQSize bs;
	    bs.setHeight( (ceData.rect.height() + 1)/2 );
	    if ( bs.height() < 10 )
		bs.setHeight( 10 );
	    bs.setWidth( bs.height() ); // 1.6 -approximate golden mean
	    bs = bs.expandedTo( TQApplication::globalStrut() );
	    int y = 0;
	    int x, lx, rx, h;
	    x = ceData.rect.width() - y - bs.width();
	    lx = fw;
	    rx = x - fw * 2;
	    h = bs.height() * 2;

	    switch ( subcontrol ) {
	    case SC_SpinWidgetUp:
		return TQRect(x + 1, y, bs.width(), bs.height() - 1);
	    case SC_SpinWidgetDown:
		return TQRect(x + 1, y + bs.height() + 1, bs.width(), bs.height());
	    case SC_SpinWidgetButtonField:
		return TQRect(x, y, bs.width(), h - 2*fw);
	    case SC_SpinWidgetEditField:
		return TQRect(lx, fw, rx, h - 2*fw);
	    case SC_SpinWidgetFrame:
		return TQRect( 0, 0, ceData.rect.width() - bs.width(), h);
	    default:
		break;
	    }
	    break; }

#ifndef TQT_NO_COMBOBOX
    case CC_ComboBox: {
	if (elementFlags & CEF_IsEditable) {
	    int space = (ceData.rect.height() - 13) / 2;
	    switch (subcontrol) {
	    case SC_ComboBoxFrame:
		return TQRect();
	    case SC_ComboBoxEditField: {
		TQRect rect = ceData.rect;
		rect.setWidth(rect.width() - 13 - space * 2);
		rect.addCoords(3, 3, -3, -3);
		return rect; }
	    case SC_ComboBoxArrow:
		return TQRect(ceData.rect.width() - 13 - space * 2, 0,
			     13 + space * 2, ceData.rect.height());
	    default: break;		// shouldn't get here
	    }

	} else {
	    int space = (ceData.rect.height() - 7) / 2;
	    switch (subcontrol) {
	    case SC_ComboBoxFrame:
		return TQRect();
	    case SC_ComboBoxEditField: {
		TQRect rect = ceData.rect;
		rect.addCoords(3, 3, -3, -3);
		return rect; }
	    case SC_ComboBoxArrow:		// 12 wide, 7 tall
		return TQRect(ceData.rect.width() - 12 - space, space, 12, 7);
	    default: break;		// shouldn't get here
	    }
	}
	break; }
#endif

#ifndef TQT_NO_SLIDER
    case CC_Slider: {

	if (subcontrol == SC_SliderHandle) {
	    int tickOffset  = pixelMetric( PM_SliderTickmarkOffset, ceData, elementFlags, widget );
	    int thickness   = pixelMetric( PM_SliderControlThickness, ceData, elementFlags, widget );
	    int len         = pixelMetric( PM_SliderLength, ceData, elementFlags, widget ) + 2;
	    int sliderPos   = ceData.startStep;
	    int motifBorder = 2;

	    if ( ceData.orientation == Horizontal )
		return TQRect( sliderPos + motifBorder, tickOffset + motifBorder, len,
			      thickness - 2*motifBorder );
	    return TQRect( tickOffset + motifBorder, sliderPos + motifBorder,
			  thickness - 2*motifBorder, len);
	}
	break; }
#endif
    default: break;
    }
    return TQMotifStyle::querySubControlMetrics(control, ceData, elementFlags, subcontrol, opt, widget);
}


/*! \reimp */
bool TQMotifPlusStyle::objectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQEvent *event )
{
    switch(event->type()) {
    case TQEvent::MouseButtonPress:
        {
	    singleton->mousePressed = true;

            if (!ceData.widgetObjectTypes.contains("TQSlider"))
		break;

	    singleton->sliderActive = true;
            break;
        }

    case TQEvent::MouseButtonRelease:
        {
	    singleton->mousePressed = false;

            if (!ceData.widgetObjectTypes.contains("TQSlider"))
		break;

	    singleton->sliderActive = false;
	    widgetActionRequest(ceData, elementFlags, source, WAR_Repaint);
	    break;
        }

    case TQEvent::Enter:
        {
            if (!ceData.widgetObjectTypes.contains("TQWidget"))
		break;

	    widgetActionRequest(ceData, elementFlags, source, WAR_Repaint);
	    break;
	}

    case TQEvent::Leave:
	{
            if (!ceData.widgetObjectTypes.contains("TQWidget"))
		break;

	    widgetActionRequest(ceData, elementFlags, source, WAR_Repaint);
	    break;
	}

    case TQEvent::MouseMove:
	{
	    if ((!ceData.widgetObjectTypes.contains("TQWidget")))
		break;

	    if ((!ceData.widgetObjectTypes.contains("TQScrollBar")) && (!ceData.widgetObjectTypes.contains("TQSlider")))
		break;

	    singleton->mousePos = ((TQMouseEvent *) event)->pos();
	    if (! singleton->mousePressed) {
		singleton->hovering = true;
		widgetActionRequest(ceData, elementFlags, source, WAR_Repaint);
		singleton->hovering = false;
	    }

	    break;
	}

    default:
	break;
    }

    return TQMotifStyle::objectEventHandler(ceData, elementFlags, source, event);
}


/*! \reimp */
int TQMotifPlusStyle::styleHint(StyleHint hint,
			       const TQStyleControlElementData &ceData,
			       ControlElementFlags elementFlags,
			       const TQStyleOption &opt,
			       TQStyleHintReturn *returnData,
			       const TQWidget *widget) const
{
    int ret;
    switch (hint) {
    case SH_PopupMenu_MouseTracking:
	ret = 1;
	break;
    case SH_MenuIndicatorColumnWidth:
	{
	    int maxpmw = opt.maxIconWidth();
	    bool checkable = (elementFlags & CEF_IsCheckable);

	    if (checkable)
		maxpmw = TQMAX(maxpmw, 15);

	    ret = maxpmw;
	    break;
	}
    default:
	ret = TQMotifStyle::styleHint(hint, ceData, elementFlags, opt, returnData, widget);
	break;
    }
    return ret;
}


#endif // TQT_NO_STYLE_MOTIFPLUS
