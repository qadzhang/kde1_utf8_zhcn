/****************************************************************************
**
** Implementation of Windows-like style class
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

#include "ntqwindowsstyle.h"

#if !defined(TQT_NO_STYLE_WINDOWS) || defined(QT_PLUGIN)

#include "ntqpopupmenu.h"
#include "ntqapplication.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h" // for now
#include "ntqpixmap.h" // for now
#include "ntqwidget.h"
#include "ntqlabel.h"
#include "ntqimage.h"
#include "ntqpushbutton.h"
#include "ntqcombobox.h"
#include "ntqlistbox.h"
#include "ntqwidget.h"
#include "ntqrangecontrol.h"
#include "ntqscrollbar.h"
#include "ntqslider.h"
#include "ntqtabwidget.h"
#include "ntqtabbar.h"
#include "ntqlistview.h"
#include "ntqbitmap.h"
#include "ntqcleanuphandler.h"
#include "ntqdockwindow.h"
#include "ntqobjectlist.h"
#include "ntqmenubar.h"

#include <limits.h>


static const int windowsItemFrame		=  2; // menu item frame width
static const int windowsSepHeight		=  2; // separator item height
static const int windowsItemHMargin		=  3; // menu item hor text margin
static const int windowsItemVMargin		=  2; // menu item ver text margin
static const int windowsArrowHMargin		=  6; // arrow horizontal margin
static const int windowsTabSpacing		= 12; // space between text and tab
static const int windowsCheckMarkHMargin	=  2; // horiz. margins of check mark
static const int windowsRightBorder		= 12; // right border on windows
static const int windowsCheckMarkWidth		= 12; // checkmarks width on windows

static bool use2000style = true;

enum TQSliderDirection { SlUp, SlDown, SlLeft, SlRight };

// A friendly class providing access to TQMenuData's protected member.
class FriendlyMenuData : public TQMenuData
{
    friend class TQWindowsStyle;
};

// Private class
class TQWindowsStyle::Private : public TQObject
{
public:
    Private(TQWindowsStyle *parent);

    bool objectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQEvent *e );

private:
    int menuBarTimer;
};

TQWindowsStyle::Private::Private(TQWindowsStyle *parent)
: TQObject(parent, "TQWindowsStylePrivate"), menuBarTimer(0)
{
}

// Records Alt- and Focus events
bool TQWindowsStyle::Private::objectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags, void* source, TQEvent *e )
{
    if (!(ceData.widgetObjectTypes.contains("TQWidget"))) {
	return true;
    }

    TQWidget *widget = reinterpret_cast<TQWidget*>(source);

    switch(e->type()) {
    case TQEvent::FocusIn:
    case TQEvent::FocusOut:
	{
	    // Menubars toggle based on focus
	    TQMenuBar *menuBar = ::tqt_cast<TQMenuBar*>(widget);
	    if (menuBar && !menuBarTimer) // delayed repaint to avoid flicker
		menuBarTimer = menuBar->startTimer(0);
	}
	break;
    case TQEvent::Close:
	// Reset widget when closing
	break;
    case TQEvent::Timer:
	{
	    TQMenuBar *menuBar = ::tqt_cast<TQMenuBar*>(widget);
	    TQTimerEvent *te = (TQTimerEvent*)e;
	    if (menuBar && te->timerId() == menuBarTimer) {
		menuBar->killTimer(te->timerId());
		menuBarTimer = 0;
		menuBar->repaint(false);
		return true;
	    }
	}
	break;
    default:
	break;
    }

    return true;
}

/*!
    \class TQWindowsStyle ntqwindowsstyle.h
    \brief The TQWindowsStyle class provides a Microsoft Windows-like look and feel.

    \ingroup appearance

    This style is TQt's default GUI style on Windows.
*/

/*!
    Constructs a TQWindowsStyle
*/
TQWindowsStyle::TQWindowsStyle() : TQCommonStyle(), d(0)
{
#if defined(Q_OS_WIN32)
    use2000style = qWinVersion() != TQt::WV_NT && qWinVersion() != TQt::WV_95;
#endif
}

/*! \reimp */
TQWindowsStyle::~TQWindowsStyle()
{
    delete d;
}

/*! \reimp */
bool TQWindowsStyle::objectEventHandler( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void* source, TQEvent *e ) {
    if (d) {
        return d->objectEventHandler(ceData, elementFlags, source, e);
    }
    return true;
}

/*! \reimp */
void TQWindowsStyle::applicationPolish(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr)
{
    // We only need the overhead when shortcuts are sometimes hidden
    if (styleHint(SH_UnderlineAccelerator, TQStyleControlElementData(), CEF_None, 0) && styleHint(SH_HideUnderlineAcceleratorWhenAltUp, TQStyleControlElementData(), CEF_None, 0)) {
	d = new Private(this);
	installObjectEventHandler(ceData, elementFlags, ptr, this);
    }
}

/*! \reimp */
void TQWindowsStyle::applicationUnPolish(const TQStyleControlElementData&, ControlElementFlags, void *)
{
    delete d;
    d = 0;
}

/*! \reimp */
void TQWindowsStyle::polish(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr)
{
    TQCommonStyle::polish(ceData, elementFlags, ptr);
}

/*! \reimp */
void TQWindowsStyle::unPolish(const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr)
{
    TQCommonStyle::polish(ceData, elementFlags, ptr);
}

/*! \reimp */
void TQWindowsStyle::polish( TQPalette &pal )
{
    TQCommonStyle::polish(pal);
}

/*! \reimp */
void TQWindowsStyle::drawPrimitive( PrimitiveElement pe,
				   TQPainter *p,
				   const TQStyleControlElementData &ceData,
				   ControlElementFlags elementFlags,
				   const TQRect &r,
				   const TQColorGroup &cg,
				   SFlags flags,
				   const TQStyleOption& opt ) const
{
    bool dis = !(flags & Style_Enabled);
    bool act = flags & Style_Active;

    TQRect rr( r );
    switch (pe) {
    case PE_ButtonCommand:
	{
	    TQBrush fill;

	    if (! (flags & Style_Down) && (flags & Style_On))
		fill = TQBrush(cg.light(), Dense4Pattern);
	    else
		fill = cg.brush(TQColorGroup::Button);

	    if (flags & Style_ButtonDefault && flags & Style_Down) {
		p->setPen(cg.dark());
		p->setBrush(fill);
		p->drawRect(r);
	    } else if (flags & (Style_Raised | Style_Down | Style_On | Style_Sunken))
		qDrawWinButton(p, r, cg, flags & (Style_Sunken | Style_Down |
						  Style_On), &fill);
	    else
		p->fillRect(r, fill);
	    break;
	}

    case PE_ButtonBevel:
    case PE_HeaderSection:
	{
	    TQBrush fill;

	    if (! (flags & Style_Down) && (flags & Style_On))
		fill = TQBrush(cg.light(), Dense4Pattern);
	    else
		fill = cg.brush(TQColorGroup::Button);

	    if (flags & (Style_Raised | Style_Down | Style_On | Style_Sunken))
		qDrawWinButton(p, r, cg, flags & (Style_Down | Style_On), &fill);
	    else
		p->fillRect(r, fill);
	    break;
	}

    case PE_ButtonDefault:
	p->setPen(cg.shadow());
	p->drawRect(r);
	break;

    case PE_ButtonTool:
	{
	    TQBrush fill;
	    bool stippled = false;

	    if (! (flags & (Style_Down | Style_MouseOver)) &&
		(flags & Style_On) &&
		use2000style) {
		fill = TQBrush(cg.light(), Dense4Pattern);
		stippled = true;
	    } else
		fill = cg.brush(TQColorGroup::Button);

	    if (flags & (Style_Raised | Style_Down | Style_On)) {
		if (flags & Style_AutoRaise) {
		    qDrawShadePanel(p, r, cg, flags & (Style_Down | Style_On),
				    1, &fill);

		    if (stippled) {
			p->setPen(cg.button());
			p->drawRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2);
		    }
		} else
		    qDrawWinButton(p, r, cg, flags & (Style_Down | Style_On),
				   &fill);
	    } else
		p->fillRect(r, fill);

	    break;
	}

    case PE_FocusRect:
	if (opt.isDefault())
	    p->drawWinFocusRect(r);
	else
	    p->drawWinFocusRect(r, opt.color());
	break;

    case PE_Indicator:
	{
	    TQBrush fill;
	    if (flags & Style_NoChange) {
		TQBrush b = p->brush();
		TQColor c = p->backgroundColor();
		p->setBackgroundMode( TransparentMode );
		p->setBackgroundColor( green );
		fill = TQBrush(cg.base(), Dense4Pattern);
		p->setBackgroundColor( c );
		p->setBrush( b );
	    } else if (flags & Style_Down)
		fill = cg.brush( TQColorGroup::Button );
	    else if (flags & Style_Enabled)
		fill = cg.brush( TQColorGroup::Base );
	    else
		fill = cg.brush( TQColorGroup::Background );

	    qDrawWinPanel( p, r, cg, true, &fill );

	    if (flags & Style_NoChange )
		p->setPen( cg.dark() );
	    else
		p->setPen( cg.text() );
	} // FALLTHROUGH
    case PE_CheckListIndicator:
	if ( pe == PE_CheckListIndicator ) { //since we fall through from PE_Indicator
	    if ( flags & Style_Enabled )
		p->setPen( TQPen( cg.text(), 1 ) );
	    else
		p->setPen( TQPen( cg.dark(), 1 ) );
	    if ( flags & Style_NoChange )
		p->setBrush( cg.brush( TQColorGroup::Button ) );
	    p->drawRect( r.x()+1, r.y()+1, 11, 11 );
	}
	if (! (flags & Style_Off)) {
	    TQPointArray a( 7*2 );
	    int i, xx, yy;
	    xx = rr.x() + 3;
	    yy = rr.y() + 5;

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

	    p->drawLineSegments( a );
	}
	break;

    case PE_ExclusiveIndicator:
	{
#define TQCOORDARRLEN(x) sizeof(x)/(sizeof(TQCOORD)*2)
	    static const TQCOORD pts1[] = {              // dark lines
		1,9, 1,8, 0,7, 0,4, 1,3, 1,2, 2,1, 3,1, 4,0, 7,0, 8,1, 9,1 };
	    static const TQCOORD pts2[] = {              // black lines
		2,8, 1,7, 1,4, 2,3, 2,2, 3,2, 4,1, 7,1, 8,2, 9,2 };
	    static const TQCOORD pts3[] = {              // background lines
		2,9, 3,9, 4,10, 7,10, 8,9, 9,9, 9,8, 10,7, 10,4, 9,3 };
	    static const TQCOORD pts4[] = {              // white lines
		2,10, 3,10, 4,11, 7,11, 8,10, 9,10, 10,9, 10,8, 11,7,
		11,4, 10,3, 10,2 };
	    static const TQCOORD pts5[] = {              // inner fill
		4,2, 7,2, 9,4, 9,7, 7,9, 4,9, 2,7, 2,4 };

	    // make sure the indicator is square
	    TQRect ir = r;

	    if (r.width() < r.height()) {
		ir.setTop(r.top() + (r.height() - r.width()) / 2);
		ir.setHeight(r.width());
	    } else if (r.height() < r.width()) {
		ir.setLeft(r.left() + (r.width() - r.height()) / 2);
		ir.setWidth(r.height());
	    }

	    p->eraseRect(ir);
	    bool down = flags & Style_Down;
	    bool enabled = flags & Style_Enabled;
	    bool on = flags & Style_On;
	    TQPointArray a;
	    a.setPoints( TQCOORDARRLEN(pts1), pts1 );
	    a.translate( ir.x(), ir.y() );
	    p->setPen( cg.dark() );
	    p->drawPolyline( a );
	    a.setPoints( TQCOORDARRLEN(pts2), pts2 );
	    a.translate( ir.x(), ir.y() );
	    p->setPen( cg.shadow() );
	    p->drawPolyline( a );
	    a.setPoints( TQCOORDARRLEN(pts3), pts3 );
	    a.translate( ir.x(), ir.y() );
	    p->setPen( cg.midlight() );
	    p->drawPolyline( a );
	    a.setPoints( TQCOORDARRLEN(pts4), pts4 );
	    a.translate( ir.x(), ir.y() );
	    p->setPen( cg.light() );
	    p->drawPolyline( a );
	    a.setPoints( TQCOORDARRLEN(pts5), pts5 );
	    a.translate( ir.x(), ir.y() );
	    TQColor fillColor = ( down || !enabled ) ? cg.button() : cg.base();
	    p->setPen( fillColor );
	    p->setBrush( fillColor  ) ;
	    p->drawPolygon( a );
	    if ( on ) {
		p->setPen( NoPen );
		p->setBrush( cg.text() );
		p->drawRect( ir.x() + 5, ir.y() + 4, 2, 4 );
		p->drawRect( ir.x() + 4, ir.y() + 5, 4, 2 );
	    }
	    break;
	}

    case PE_Panel:
    case PE_PanelPopup:
	{
	    int lw = opt.isDefault() ? pixelMetric(PM_DefaultFrameWidth, ceData, elementFlags)
			: opt.lineWidth();

	    if (lw == 2) {
		TQColorGroup popupCG = cg;
		if ( pe == PE_PanelPopup ) {
		    popupCG.setColor( TQColorGroup::Light, cg.background() );
		    popupCG.setColor( TQColorGroup::Midlight, cg.light() );
		}
		qDrawWinPanel(p, r, popupCG, flags & Style_Sunken);
	    } else {
		TQCommonStyle::drawPrimitive(pe, p, ceData, elementFlags, r, cg, flags, opt);
	    }
	    break;
	}

    case PE_Splitter:
	{
	    TQPen oldPen = p->pen();
	    p->setPen( cg.light() );
	    if ( flags & Style_Horizontal ) {
		p->drawLine( r.x() + 1, r.y(), r.x() + 1, r.height() );
		p->setPen( cg.dark() );
		p->drawLine( r.x(), r.y(), r.x(), r.height() );
		p->drawLine( r.right()-1, r.y(), r.right()-1, r.height() );
		p->setPen( cg.shadow() );
		p->drawLine( r.right(), r.y(), r.right(), r.height() );
	    } else {
		p->drawLine( r.x(), r.y() + 1, r.width(), r.y() + 1 );
		p->setPen( cg.dark() );
		p->drawLine( r.x(), r.bottom() - 1, r.width(), r.bottom() - 1 );
		p->setPen( cg.shadow() );
		p->drawLine( r.x(), r.bottom(), r.width(), r.bottom() );
	    }
	    p->setPen( oldPen );
	    break;
	}
    case PE_DockWindowResizeHandle:
	{
	    TQPen oldPen = p->pen();
	    p->setPen( cg.light() );
	    if ( flags & Style_Horizontal ) {
		p->drawLine( r.x(), r.y(), r.width(), r.y() );
		p->setPen( cg.dark() );
		p->drawLine( r.x(), r.bottom() - 1, r.width(), r.bottom() - 1 );
		p->setPen( cg.shadow() );
		p->drawLine( r.x(), r.bottom(), r.width(), r.bottom() );
	    } else {
		p->drawLine( r.x(), r.y(), r.x(), r.height() );
		p->setPen( cg.dark() );
		p->drawLine( r.right()-1, r.y(), r.right()-1, r.height() );
		p->setPen( cg.shadow() );
		p->drawLine( r.right(), r.y(), r.right(), r.height() );
	    }
	    p->setPen( oldPen );
	    break;
	}

    case PE_ScrollBarSubLine:
	if (use2000style) {
	    if (flags & Style_Down) {
		p->setPen( cg.dark() );
		p->setBrush( cg.brush( TQColorGroup::Button ) );
		p->drawRect( r );
	    } else
		drawPrimitive(PE_ButtonBevel, p, ceData, elementFlags, r, cg, flags | Style_Raised);
	} else
	    drawPrimitive(PE_ButtonBevel, p, ceData, elementFlags, r, cg, (flags & Style_Enabled) |
			  ((flags & Style_Down) ? Style_Down : Style_Raised));

	drawPrimitive(((flags & Style_Horizontal) ? PE_ArrowLeft : PE_ArrowUp),
		      p, ceData, elementFlags, r, cg, flags);
	break;

    case PE_ScrollBarAddLine:
	if (use2000style) {
	    if (flags & Style_Down) {
		p->setPen( cg.dark() );
		p->setBrush( cg.brush( TQColorGroup::Button ) );
		p->drawRect( r );
	    } else
		drawPrimitive(PE_ButtonBevel, p, ceData, elementFlags, r, cg, flags | Style_Raised);
	} else
	    drawPrimitive(PE_ButtonBevel, p, ceData, elementFlags, r, cg, (flags & Style_Enabled) |
			  ((flags & Style_Down) ? Style_Down : Style_Raised));

	drawPrimitive(((flags & Style_Horizontal) ? PE_ArrowRight : PE_ArrowDown),
		      p, ceData, elementFlags, r, cg, flags);
	break;

    case PE_ScrollBarAddPage:
    case PE_ScrollBarSubPage:
	{
	    TQBrush br;
	    TQColor c = p->backgroundColor();

	    p->setPen(NoPen);
	    p->setBackgroundMode(OpaqueMode);

	    if (flags & Style_Down) {
		br = TQBrush(cg.shadow(), Dense4Pattern);
		p->setBackgroundColor( cg.dark() );
		p->setBrush( TQBrush(cg.shadow(), Dense4Pattern) );
	    } else {
		br = (cg.brush(TQColorGroup::Light).pixmap() ?
		      cg.brush(TQColorGroup::Light) :
		      TQBrush(cg.light(), Dense4Pattern));
		p->setBrush(br);
	    }

	    p->drawRect(r);
	    p->setBackgroundColor(c);
	    break;
	}

    case PE_ScrollBarSlider:
	if (! (flags & Style_Enabled)) {
	    TQBrush br = (cg.brush(TQColorGroup::Light).pixmap() ?
			 cg.brush(TQColorGroup::Light) :
			 TQBrush(cg.light(), Dense4Pattern));
	    p->setPen(NoPen);
	    p->setBrush(br);
	    p->setBackgroundMode(OpaqueMode);
	    p->drawRect(r);
	} else
	    drawPrimitive(PE_ButtonBevel, p, ceData, elementFlags, r, cg, Style_Enabled | Style_Raised);
	break;

    case PE_WindowFrame:
	{
	    TQColorGroup popupCG = cg;
	    popupCG.setColor( TQColorGroup::Light, cg.background() );
	    popupCG.setColor( TQColorGroup::Midlight, cg.light() );
	    qDrawWinPanel(p, r, popupCG, flags & Style_Sunken);
	}
	break;

    case PE_MenuItemIndicatorFrame:
	{
	    int x, y, w, h;
	    r.rect( &x, &y, &w, &h );
	    int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags, opt, NULL, NULL);

	    TQRect vrect = visualRect( TQRect( x, y, checkcol, h ), r );
	    int xvis = vrect.x();

	    if ( act && !dis ) {
		qDrawShadePanel( p, xvis, y, checkcol, h, cg, true, 1, &cg.brush( TQColorGroup::Button ) );
	    }
	    else {
		TQBrush fill( cg.light(), Dense4Pattern );
		// set the brush origin for the hash pattern to the x/y coordinate
		// of the menu item's checkmark... this way, the check marks have
		// a consistent look
		TQPoint origin = p->brushOrigin();
		p->setBrushOrigin( xvis, y );
		qDrawShadePanel( p, xvis, y, checkcol, h, cg, true, 1, &fill );
		// restore the previous brush origin
		p->setBrushOrigin( origin );
	    }
	}
	break;

    case PE_MenuItemIndicatorIconFrame:
	{
	    int x, y, w, h;
	    r.rect( &x, &y, &w, &h );
	    int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags, opt, NULL, NULL);

	    TQRect vrect = visualRect( TQRect( x, y, checkcol, h ), r );
	    int xvis = vrect.x();

	    qDrawShadePanel( p, xvis, y, w, h, cg, false, 1, &cg.brush( TQColorGroup::Button ) );
	}
	break;

    case PE_MenuItemIndicatorCheck:
	{
	    int x, y, w, h;
	    r.rect( &x, &y, &w, &h );
	    int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags, opt, NULL, NULL);

	    int xp = x + windowsItemFrame;

	    SFlags cflags = Style_Default;
	    if (! dis)
		cflags |= Style_Enabled;
	    if (act)
		cflags |= Style_On;

	    drawPrimitive(PE_CheckMark, p, ceData, elementFlags, visualRect( TQRect(xp, y + windowsItemFrame, checkcol - 2*windowsItemFrame, h - 2*windowsItemFrame), r ), cg, cflags);
	}
	break;

    default:
	if (pe >= PE_ArrowUp && pe <= PE_ArrowLeft) {
	    TQPointArray a;

	    switch ( pe ) {
	    case PE_ArrowUp:
		a.setPoints( 7, -4,1, 2,1, -3,0, 1,0, -2,-1, 0,-1, -1,-2 );
		break;

	    case PE_ArrowDown:
		a.setPoints( 7, -4,-2, 2,-2, -3,-1, 1,-1, -2,0, 0,0, -1,1 );
		break;

	    case PE_ArrowRight:
		a.setPoints( 7, -2,-3, -2,3, -1,-2, -1,2, 0,-1, 0,1, 1,0 );
		break;

	    case PE_ArrowLeft:
		a.setPoints( 7, 0,-3, 0,3, -1,-2, -1,2, -2,-1, -2,1, -3,0 );
		break;

	    default:
		break;
	    }

	    if (a.isNull())
		return;

	    p->save();
	    if ( flags & Style_Down )
		p->translate( pixelMetric( PM_ButtonShiftHorizontal, ceData, elementFlags ),
			      pixelMetric( PM_ButtonShiftVertical, ceData, elementFlags ) );

	    if ( flags & Style_Enabled ) {
		a.translate( r.x() + r.width() / 2, r.y() + r.height() / 2 );
		p->setPen( cg.buttonText() );
		p->drawLineSegments( a, 0, 3 );         // draw arrow
		p->drawPoint( a[6] );
	    } else {
		a.translate( r.x() + r.width() / 2 + 1, r.y() + r.height() / 2 + 1 );
		p->setPen( cg.light() );
		p->drawLineSegments( a, 0, 3 );         // draw arrow
		p->drawPoint( a[6] );
		a.translate( -1, -1 );
		p->setPen( cg.mid() );
		p->drawLineSegments( a, 0, 3 );         // draw arrow
		p->drawPoint( a[6] );
	    }
	    p->restore();
	} else
	    TQCommonStyle::drawPrimitive(pe, p, ceData, elementFlags, r, cg, flags, opt);
    }
}


/*!
  \reimp
*/
void TQWindowsStyle::drawControl( ControlElement element,
				 TQPainter *p,
				 const TQStyleControlElementData &ceData,
				 ControlElementFlags elementFlags,
				 const TQRect &r,
				 const TQColorGroup &cg,
				 SFlags flags,
				 const TQStyleOption& opt,
				 const TQWidget *widget ) const
{
    switch (element) {
#ifndef TQT_NO_TABBAR
    case CE_TabBarTab:
	{
	    if ( ( elementFlags & CEF_UseGenericParameters ) || (!( elementFlags & CEF_HasParentWidget )) || !opt.tab() )
		break;

	    const TQTab * t = opt.tab();
	    bool selected = flags & Style_Selected;
	    bool lastTab = (ceData.tabBarData.identIndexMap[t->identifier()] == ceData.tabBarData.tabCount-1);
	    TQRect r2( r );
	    if ( ceData.tabBarData.shape  == TQTabBar::RoundedAbove ) {
		p->setPen( cg.midlight() );

		p->drawLine( r2.left(), r2.bottom(), r2.right(), r2.bottom() );
		p->setPen( cg.light() );
		p->drawLine( r2.left(), r2.bottom()-1, r2.right(), r2.bottom()-1 );
		if ( r2.left() == 0 )
		    p->drawPoint( ceData.rect.bottomLeft() );

		if ( selected ) {
		    p->fillRect( TQRect( r2.left()+1, r2.bottom()-1, r2.width()-3, 2),
				 cg.brush( TQColorGroup::Background ));
		    p->setPen( cg.background() );
		    p->drawLine( r2.left()+1, r2.bottom(), r2.left()+1, r2.top()+2 );
		    p->setPen( cg.light() );
		} else {
		    p->setPen( cg.light() );
		    r2.setRect( r2.left() + 2, r2.top() + 2,
			       r2.width() - 4, r2.height() - 2 );
		}

		int x1, x2;
		x1 = r2.left();
		x2 = r2.right() - 2;
		p->drawLine( x1, r2.bottom()-1, x1, r2.top() + 2 );
		x1++;
		p->drawPoint( x1, r2.top() + 1 );
		x1++;
		p->drawLine( x1, r2.top(), x2, r2.top() );
		if ( r2.left() > 0 ) {
		    p->setPen( cg.midlight() );
		}
		x1 = r2.left();
		p->drawPoint( x1, r2.bottom());

		p->setPen( cg.midlight() );
		x1++;
		p->drawLine( x1, r2.bottom(), x1, r2.top() + 2 );
		x1++;
		p->drawLine( x1, r2.top()+1, x2, r2.top()+1 );

		p->setPen( cg.dark() );
		x2 = r2.right() - 1;
		p->drawLine( x2, r2.top() + 2, x2, r2.bottom() - 1 +
			     (selected ? 1:-1) );
		p->setPen( cg.shadow() );
		p->drawPoint( x2, r2.top() + 1 );
		p->drawPoint( x2, r2.top() + 1 );
		x2++;
		p->drawLine( x2, r2.top() + 2, x2, r2.bottom() -
			     (selected ? (lastTab ? 0:1) :2));
	    } else if ( ceData.tabBarData.shape == TQTabBar::RoundedBelow ) {
		bool rightAligned = styleHint( SH_TabBar_Alignment, ceData, elementFlags, TQStyleOption::Default, 0, widget ) == AlignRight;
		bool firstTab = ceData.tabBarData.identIndexMap[t->identifier()] == 0;
		if ( selected ) {
		    p->fillRect( TQRect( r2.left()+1, r2.top(), r2.width()-3, 1),
				 cg.brush( TQColorGroup::Background ));
		    p->setPen( cg.background() );
		    p->drawLine( r2.left()+1, r2.top(), r2.left()+1, r2.bottom()-2 );
		    p->setPen( cg.dark() );
		} else {
		    p->setPen( cg.shadow() );
		    p->drawLine( r2.left() +
				 (rightAligned && firstTab ? 0 : 1),
				 r2.top() + 1,
				 r2.right() - (lastTab ? 0 : 2),
				 r2.top() + 1 );

		    if ( rightAligned && lastTab )
			p->drawPoint( r2.right(), r2.top() );
		    p->setPen( cg.dark() );
		    p->drawLine( r2.left(), r2.top(), r2.right() - 1,
				 r2.top() );
		    r2.setRect( r2.left() + 2, r2.top(),
				r2.width() - 4, r2.height() - 2 );
		}

		p->drawLine( r2.right() - 1, r2.top() + (selected ? 0: 2),
			     r2.right() - 1, r2.bottom() - 2 );
		p->drawPoint( r2.right() - 2, r2.bottom() - 2 );
		p->drawLine( r2.right() - 2, r2.bottom() - 1,
			     r2.left() + 1, r2.bottom() - 1 );

		p->setPen( cg.midlight() );
		p->drawLine( r2.left() + 1, r2.bottom() - 2,
			     r2.left() + 1, r2.top() + (selected ? 0 : 2) );

		p->setPen( cg.shadow() );
		p->drawLine( r2.right(),
			     r2.top() + (lastTab && rightAligned &&
					 selected) ? 0 : 1,
			     r2.right(), r2.bottom() - 1 );
		p->drawPoint( r2.right() - 1, r2.bottom() - 1 );
		p->drawLine( r2.right() - 1, r2.bottom(),
			     r2.left() + 2, r2.bottom() );

		p->setPen( cg.light() );
		p->drawLine( r2.left(), r2.top() + (selected ? 0 : 2),
			     r2.left(), r2.bottom() - 2 );
	    } else {
		TQCommonStyle::drawControl(element, p, ceData, elementFlags, r, cg, flags, opt, widget);
	    }
	    break;
	}
#endif // TQT_NO_TABBAR
    case CE_ToolBoxTab:
	{
	    qDrawShadePanel( p, r, cg, flags & (Style_Sunken | Style_Down | Style_On) , 1,
			     &cg.brush(TQColorGroup::Button));
	    break;
	}

#ifndef TQT_NO_POPUPMENU
    case CE_PopupMenuItem:
	{
	    if (( elementFlags & CEF_UseGenericParameters ) || opt.isDefault())
		break;

	    TQMenuItem *mi = opt.menuItem();
	    if ( !mi )
		break;

	    int tab = opt.tabWidth();
	    int maxpmw = opt.maxIconWidth();
	    bool dis = !(flags&Style_Enabled);
	    bool checkable = ( elementFlags & CEF_IsCheckable );
	    bool act = flags & Style_Active;
	    int x, y, w, h;

	    r.rect(&x, &y, &w, &h);

	    if ( checkable ) {
		// space for the checkmarks
		if (use2000style)
		    maxpmw = TQMAX( maxpmw, 20 );
		else
		    maxpmw = TQMAX( maxpmw, 12 );
	    }

	    int checkcol = maxpmw;

	    if ( mi && mi->isSeparator() ) {                    // draw separator
		p->setPen( cg.dark() );
		p->drawLine( x, y, x+w, y );
		p->setPen( cg.light() );
		p->drawLine( x, y+1, x+w, y+1 );
		return;
	    }

	    TQBrush fill = (act ?
			   cg.brush( TQColorGroup::Highlight ) :
			   cg.brush( TQColorGroup::Button ));
	    p->fillRect( x, y, w, h, fill);

	    if ( !mi )
		return;

	    int xpos = x;
	    TQRect vrect = visualRect( TQRect( xpos, y, checkcol, h ), r );
	    int xvis = vrect.x();
	    if ( mi->isChecked() ) {
		drawPrimitive(PE_MenuItemIndicatorFrame, p, ceData, elementFlags, r, cg, flags, opt);
	    } else if (! act)
		p->fillRect(xvis, y, checkcol , h, cg.brush( TQColorGroup::Button ));

	    if ( mi->iconSet() ) {              // draw iconset
		TQIconSet::Mode mode = dis ? TQIconSet::Disabled : TQIconSet::Normal;
		if (act && !dis )
		    mode = TQIconSet::Active;
		TQPixmap pixmap;
		if ( checkable && mi->isChecked() )
		    pixmap = mi->iconSet()->pixmap( TQIconSet::Small, mode, TQIconSet::On );
		else
		    pixmap = mi->iconSet()->pixmap( TQIconSet::Small, mode );
		int pixw = pixmap.width();
		int pixh = pixmap.height();
		if ( act && !dis && !mi->isChecked() )
		    drawPrimitive(PE_MenuItemIndicatorIconFrame, p, ceData, elementFlags, r, cg, flags, opt);
		TQRect pmr( 0, 0, pixw, pixh );
		pmr.moveCenter( vrect.center() );
		p->setPen( cg.text() );
		p->drawPixmap( pmr.topLeft(), pixmap );

		fill = (act ?
			cg.brush( TQColorGroup::Highlight ) :
			cg.brush( TQColorGroup::Button ));
		int xp = xpos + checkcol + 1;
		p->fillRect( visualRect( TQRect( xp, y, w - checkcol - 1, h ), r ), fill);
	    } else if ( checkable ) {  // just "checking"...
		if ( mi->isChecked() ) {
		    drawPrimitive(PE_MenuItemIndicatorCheck, p, ceData, elementFlags, r, cg, flags, opt);
		}
	    }

	    p->setPen( act ? cg.highlightedText() : cg.buttonText() );

	    TQColor discol;
	    if ( dis ) {
		discol = cg.text();
		p->setPen( discol );
	    }

	    int xm = windowsItemFrame + checkcol + windowsItemHMargin;
	    xpos += xm;

	    vrect = visualRect( TQRect( xpos, y+windowsItemVMargin, w-xm-tab+1, h-2*windowsItemVMargin ), r );
	    xvis = vrect.x();
	    if ( mi->custom() ) {
		p->save();
		if ( dis && !act ) {
		    p->setPen( cg.light() );
		    mi->custom()->paint( p, cg, act, !dis,
					 xvis+1, y+windowsItemVMargin+1, w-xm-tab+1, h-2*windowsItemVMargin );
		    p->setPen( discol );
		}
		mi->custom()->paint( p, cg, act, !dis,
				     xvis, y+windowsItemVMargin, w-xm-tab+1, h-2*windowsItemVMargin );
		p->restore();
	    }
	    TQString s = mi->text();
	    if ( !s.isNull() ) {                        // draw text
		int t = s.find( '\t' );
		int text_flags = AlignVCenter|ShowPrefix | DontClip | SingleLine;
		if ((!styleHint(SH_UnderlineAccelerator, ceData, elementFlags, TQStyleOption::Default, 0, widget)) || ((styleHint(SH_HideUnderlineAcceleratorWhenAltUp, ceData, elementFlags, TQStyleOption::Default, 0, widget)) && (!acceleratorsShown()))) {
		    text_flags |= NoAccel;
		}
		text_flags |= (TQApplication::reverseLayout() ? AlignRight : AlignLeft );
		if ( t >= 0 ) {                         // draw tab text
		    int xp = x + w - tab - windowsItemHMargin - windowsItemFrame + 1;
		    if ( use2000style )
			xp -= 20;
		    else
			xp -= windowsRightBorder;
		    int xoff = visualRect( TQRect( xp, y+windowsItemVMargin, tab, h-2*windowsItemVMargin ), r ).x();
		    if ( dis && !act ) {
			p->setPen( cg.light() );
			p->drawText( xoff+1, y+windowsItemVMargin+1, tab, h-2*windowsItemVMargin, text_flags, s.mid( t+1 ));
			p->setPen( discol );
		    }
		    p->drawText( xoff, y+windowsItemVMargin, tab, h-2*windowsItemVMargin, text_flags, s.mid( t+1 ) );
		    s = s.left( t );
		}
		if ( dis && !act ) {
		    p->setPen( cg.light() );
		    p->drawText( xvis+1, y+windowsItemVMargin+1, w-xm-tab+1, h-2*windowsItemVMargin, text_flags, s, t );
		    p->setPen( discol );
		}
		p->drawText( xvis, y+windowsItemVMargin, w-xm-tab+1, h-2*windowsItemVMargin, text_flags, s, t );
	    } else if ( mi->pixmap() ) {                        // draw pixmap
		TQPixmap *pixmap = mi->pixmap();
		if ( pixmap->depth() == 1 )
		    p->setBackgroundMode( OpaqueMode );
		p->drawPixmap( xvis, y+windowsItemFrame, *pixmap );
		if ( pixmap->depth() == 1 )
		    p->setBackgroundMode( TransparentMode );
	    }
	    if ( mi->popup() ) {                        // draw sub menu arrow
		int dim = (h-2*windowsItemFrame) / 2;
		PrimitiveElement arrow;
		arrow = ( TQApplication::reverseLayout() ? PE_ArrowLeft : PE_ArrowRight );
		xpos = x+w - windowsArrowHMargin - windowsItemFrame - dim;
		vrect = visualRect( TQRect(xpos, y + h / 2 - dim / 2, dim, dim), r );
		if ( act ) {
		    TQColorGroup g2 = cg;
		    g2.setColor( TQColorGroup::ButtonText, g2.highlightedText() );
		    drawPrimitive(arrow, p, ceData, elementFlags, vrect,
				  g2, dis ? Style_Default : Style_Enabled);
		} else {
		    drawPrimitive(arrow, p, ceData, elementFlags, vrect,
				  cg, dis ? Style_Default : Style_Enabled );
		}
	    }

	    break;
	}
#endif

    case CE_MenuBarItem:
	{
	    bool active = flags & Style_Active;
	    bool hasFocus = flags & Style_HasFocus;
	    bool down = flags & Style_Down;
	    TQRect pr = r;

	    p->fillRect( r, cg.brush( TQColorGroup::Button ) );
	    if ( active || hasFocus ) {
		TQBrush b = cg.brush( TQColorGroup::Button );
		if ( active && down )
		    p->setBrushOrigin(p->brushOrigin() + TQPoint(1,1));
		if ( active && hasFocus )
		    qDrawShadeRect( p, r.x(), r.y(), r.width(), r.height(),
				    cg, active && down, 1, 0, &b );
		if ( active && down ) {
		    pr.moveBy( pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags, widget),
			       pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags, widget) );
		    p->setBrushOrigin(p->brushOrigin() - TQPoint(1,1));
		}
	    }
	    TQCommonStyle::drawControl(element, p, ceData, elementFlags, pr, cg, flags, opt, widget);
	    break;
	}

    default:
	TQCommonStyle::drawControl(element, p, ceData, elementFlags, r, cg, flags, opt, widget);
    }
}


/*!
  \reimp
*/
int TQWindowsStyle::pixelMetric(PixelMetric metric, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *widget) const
{
    int ret;

    switch (metric) {
    case PM_ButtonDefaultIndicator:
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
	ret = 1;
	break;

    case PM_MaximumDragDistance:
	ret = 60;
	break;

#ifndef TQT_NO_SLIDER
    case PM_SliderLength:
	ret = 11;
	break;

	// Returns the number of pixels to use for the business part of the
	// slider (i.e., the non-tickmark portion). The remaining space is shared
	// equally between the tickmark regions.
    case PM_SliderControlThickness:
	{
	    int space = (ceData.orientation == Horizontal) ? ceData.rect.height()
			: ceData.rect.width();
	    int ticks = ceData.tickMarkSetting;
	    int n = 0;
	    if ( ticks & TQSlider::Above ) n++;
	    if ( ticks & TQSlider::Below ) n++;
	    if ( !n ) {
		ret = space;
		break;
	    }

	    int thick = 6;	// Magic constant to get 5 + 16 + 5
	    if ( ticks != TQSlider::Both && ticks != TQSlider::NoMarks )
		thick += pixelMetric( PM_SliderLength, ceData, elementFlags, widget ) / 4;

	    space -= thick;
	    //### the two sides may be unequal in size
	    if ( space > 0 )
		thick += (space * 2) / (n + 2);
	    ret = thick;
	    break;
	}
#endif // TQT_NO_SLIDER

    case PM_MenuBarFrameWidth:
	ret = 0;
	break;

    case PM_SplitterWidth:
	ret = TQMAX( 6, TQApplication::globalStrut().width() );
	break;

    case PM_MenuIndicatorFrameHBorder:
    case PM_MenuIndicatorFrameVBorder:
    case PM_MenuIconIndicatorFrameHBorder:
    case PM_MenuIconIndicatorFrameVBorder:
	ret = windowsItemFrame;
	break;

    default:
	ret = TQCommonStyle::pixelMetric(metric, ceData, elementFlags, widget);
	break;
    }

    return ret;
}


/*!
  \reimp
*/
TQSize TQWindowsStyle::sizeFromContents( ContentsType contents,
				       const TQStyleControlElementData &ceData,
				       ControlElementFlags elementFlags,
				       const TQSize &contentsSize,
				       const TQStyleOption& opt,
				       const TQWidget *widget ) const
{
    TQSize sz(contentsSize);

    switch (contents) {
    case CT_PushButton:
	{
#ifndef TQT_NO_PUSHBUTTON
	    sz = TQCommonStyle::sizeFromContents(contents, ceData, elementFlags, contentsSize, opt, widget);
	    int w = sz.width(), h = sz.height();

	    int defwidth = 0;
	    if ((elementFlags & CEF_IsDefault) || (elementFlags & CEF_AutoDefault))
		defwidth = 2*pixelMetric( PM_ButtonDefaultIndicator, ceData, elementFlags, widget );

	    if (w < 80+defwidth && ceData.fgPixmap.isNull())
		w = 80+defwidth;
	    if (h < 23+defwidth)
		h = 23+defwidth;

	    sz = TQSize(w, h);
#endif
	    break;
	}

    case CT_PopupMenuItem:
	{
#ifndef TQT_NO_POPUPMENU
	    if (( elementFlags & CEF_UseGenericParameters ) || opt.isDefault())
		break;

	    bool checkable = ( elementFlags & CEF_IsCheckable );
	    TQMenuItem *mi = opt.menuItem();
	    int maxpmw = opt.maxIconWidth();
	    int w = sz.width(), h = sz.height();

	    if (mi->custom()) {
		w = mi->custom()->sizeHint().width();
		h = mi->custom()->sizeHint().height();
		if (! mi->custom()->fullSpan())
		    h += 2*windowsItemVMargin + 2*windowsItemFrame;
	    } else if ( mi->widget() ) {
	    } else if (mi->isSeparator()) {
		w = 10; // arbitrary
		h = windowsSepHeight;
	    } else {
		if (mi->pixmap())
		    h = TQMAX(h, mi->pixmap()->height() + 2*windowsItemFrame);
		else if (! mi->text().isNull())
		    h = TQMAX(h, TQFontMetrics(ceData.font).height() + 2*windowsItemVMargin +
			     2*windowsItemFrame);

		if (mi->iconSet() != 0)
		    h = TQMAX(h, mi->iconSet()->pixmap(TQIconSet::Small,
						      TQIconSet::Normal).height() +
			     2*windowsItemFrame);
	    }

	    if (! mi->text().isNull() && mi->text().find('\t') >= 0) {
		if ( use2000style )
		    w += 20;
		else
		    w += windowsTabSpacing;
	    } else if (mi->popup()) {
		w += 2*windowsArrowHMargin;
	    }

	    if (use2000style) {
		if (checkable && maxpmw < 20)
		    w += 20 - maxpmw;
	    } else {
		if (checkable && maxpmw < windowsCheckMarkWidth)
		    w += windowsCheckMarkWidth - maxpmw;
	    }
	    if (checkable || maxpmw > 0)
		w += windowsCheckMarkHMargin;
	    if (use2000style)
		w += 20;
	    else
		w += windowsRightBorder;

	    sz = TQSize(w, h);
#endif
	    break;
	}

    default:
	sz = TQCommonStyle::sizeFromContents(contents, ceData, elementFlags, sz, opt, widget);
	break;
    }

    return sz;
}

/*! \reimp
*/
void TQWindowsStyle::polishPopupMenu( const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, void *ptr )
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
"# c #000000",
". c None",
"............",
"............",
"..##....##..",
"...##..##...",
"....####....",
".....##.....",
"....####....",
"...##..##...",
"..##....##..",
"............",
"............",
"............"};

static const char * const qt_maximize_xpm[]={
"12 12 2 1",
"# c #000000",
". c None",
"............",
".#########..",
".#########..",
".#.......#..",
".#.......#..",
".#.......#..",
".#.......#..",
".#.......#..",
".#.......#..",
".#########..",
"............",
"............"};


static const char * const qt_minimize_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"............",
"............",
"............",
"............",
"............",
"............",
"............",
"..######....",
"..######....",
"............",
"............"};

static const char * const qt_normalizeup_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"....######..",
"....######..",
"....#....#..",
"..######.#..",
"..######.#..",
"..#....###..",
"..#....#....",
"..#....#....",
"..######....",
"............",
"............"};


static const char * const qt_shade_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"............",
"............",
"............",
"............",
".....#......",
"....###.....",
"...#####....",
"..#######...",
"............",
"............",
"............"};

static const char * const qt_unshade_xpm[] = {
"12 12 2 1",
"# c #000000",
". c None",
"............",
"............",
"............",
"............",
"..#######...",
"...#####....",
"....###.....",
".....#......",
"............",
"............",
"............",
"............"};

static const char * dock_window_close_xpm[] = {
"8 8 2 1",
"# c #000000",
". c None",
"........",
".##..##.",
"..####..",
"...##...",
"..####..",
".##..##.",
"........",
"........"};

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
#endif //TQT_NO_IMAGEIO_XPM

/*!
 \reimp
 */
TQPixmap TQWindowsStyle::stylePixmap(StylePixmap stylepixmap,
				   const TQStyleControlElementData &ceData,
				   ControlElementFlags elementFlags,
				   const TQStyleOption& opt,
				   const TQWidget *widget) const
{
#ifndef TQT_NO_IMAGEIO_XPM
    switch (stylepixmap) {
    case SP_TitleBarShadeButton:
	return TQPixmap( (const char **)qt_shade_xpm );
    case SP_TitleBarUnshadeButton:
	return TQPixmap( (const char **)qt_unshade_xpm );
    case SP_TitleBarNormalButton:
	return TQPixmap( (const char **)qt_normalizeup_xpm );
    case SP_TitleBarMinButton:
	return TQPixmap( (const char **)qt_minimize_xpm );
    case SP_TitleBarMaxButton:
	return TQPixmap( (const char **)qt_maximize_xpm );
    case SP_TitleBarCloseButton:
	return TQPixmap( (const char **)qt_close_xpm );
    case SP_DockWindowCloseButton:
	return TQPixmap( (const char **)dock_window_close_xpm );
    case SP_MessageBoxInformation:
	return TQPixmap( (const char **)information_xpm);
    case SP_MessageBoxWarning:
	return TQPixmap( (const char **)warning_xpm );
    case SP_MessageBoxCritical:
	return TQPixmap( (const char **)critical_xpm );
    case SP_MessageBoxQuestion:
	return TQPixmap( (const char **)question_xpm );
    default:
	break;
    }
#endif //TQT_NO_IMAGEIO_XPM
    return TQCommonStyle::stylePixmap(stylepixmap, ceData, elementFlags, opt, widget);
}

/*!\reimp
*/
void TQWindowsStyle::drawComplexControl( ComplexControl ctrl, TQPainter *p,
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
    switch (ctrl) {
#ifndef TQT_NO_LISTVIEW
    case CC_ListView:
	{
	    if ( sub & SC_ListView ) {
		TQCommonStyle::drawComplexControl( ctrl, p, ceData, elementFlags, r, cg, flags, sub, subActive, opt, widget );
	    }
	    if ( sub & ( SC_ListViewBranch | SC_ListViewExpand ) ) {
		if (opt.isDefault())
		    break;

		TQListViewItem *item = opt.listViewItem(),
			     *child = item->firstChild();

		int y = r.y();
		int c;
		int dotoffset = 0;
		TQPointArray dotlines;
		if ( subActive == (uint)SC_All && sub == SC_ListViewExpand ) {
		    c = 2;
		    dotlines.resize(2);
		    dotlines[0] = TQPoint( r.right(), r.top() );
		    dotlines[1] = TQPoint( r.right(), r.bottom() );
		} else {
		    int linetop = 0, linebot = 0;
		    // each branch needs at most two lines, ie. four end points
		    dotoffset = (item->itemPos() + item->height() - y) %2;
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
				p->setPen( cg.mid() );
				p->drawRect( bx-4, linebot-4, 9, 9 );
				// plus or minus
				p->setPen( cg.text() );
				p->drawLine( bx - 2, linebot, bx + 2, linebot );
				if ( !child->isOpen() )
				    p->drawLine( bx, linebot - 2, bx, linebot + 2 );
				// dotlinery
				p->setPen( cg.mid() );
				dotlines[c++] = TQPoint( bx, linetop );
				dotlines[c++] = TQPoint( bx, linebot - 4 );
				dotlines[c++] = TQPoint( bx + 5, linebot );
				dotlines[c++] = TQPoint( r.width(), linebot );
				linetop = linebot + 5;
			    } else {
				// just dotlinery
				dotlines[c++] = TQPoint( bx+1, linebot -1);
				dotlines[c++] = TQPoint( r.width(), linebot -1);
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
		p->setPen( cg.text() );

		static TQBitmap *verticalLine = 0, *horizontalLine = 0;
		static TQCleanupHandler<TQBitmap> qlv_cleanup_bitmap;
		if ( !verticalLine ) {
		    // make 128*1 and 1*128 bitmaps that can be used for
		    // drawing the right sort of lines.
		    verticalLine = new TQBitmap( 1, 129, true );
		    horizontalLine = new TQBitmap( 128, 1, true );
		    TQPointArray a( 64 );
		    TQPainter p;
		    p.begin( verticalLine );
		    int i;
		    for( i=0; i<64; i++ )
			a.setPoint( i, 0, i*2+1 );
		    p.setPen( color1 );
		    p.drawPoints( a );
		    p.end();
		    TQApplication::flushX();
		    verticalLine->setMask( *verticalLine );
		    p.begin( horizontalLine );
		    for( i=0; i<64; i++ )
			a.setPoint( i, i*2+1, 0 );
		    p.setPen( color1 );
		    p.drawPoints( a );
		    p.end();
		    TQApplication::flushX();
		    horizontalLine->setMask( *horizontalLine );
		    qlv_cleanup_bitmap.add( &verticalLine );
		    qlv_cleanup_bitmap.add( &horizontalLine );
		}

		int line; // index into dotlines
		if ( sub & SC_ListViewBranch ) for( line = 0; line < c; line += 2 ) {
		    // assumptions here: lines are horizontal or vertical.
		    // lines always start with the numerically lowest
		    // coordinate.

		    // point ... relevant coordinate of current point
		    // end ..... same coordinate of the end of the current line
		    // other ... the other coordinate of the current point/line
		    if ( dotlines[line].y() == dotlines[line+1].y() ) {
			int end = dotlines[line+1].x();
			int point = dotlines[line].x();
			int other = dotlines[line].y();
			while( point < end ) {
			    int i = 128;
			    if ( i+point > end )
				i = end-point;
			    p->drawPixmap( point, other, *horizontalLine,
					   0, 0, i, 1 );
			    point += i;
			}
		    } else {
			int end = dotlines[line+1].y();
			int point = dotlines[line].y();
			int other = dotlines[line].x();
			int pixmapoffset = ((point & 1) != dotoffset ) ? 1 : 0;
			while( point < end ) {
			    int i = 128;
			    if ( i+point > end )
				i = end-point;
			    p->drawPixmap( other, point, *verticalLine,
					   0, pixmapoffset, 1, i );
			    point += i;
			}
		    }
		}
	    }
	}
	break;
#endif //TQT_NO_LISTVIEW

#ifndef TQT_NO_COMBOBOX
    case CC_ComboBox:
	if ( sub & SC_ComboBoxArrow ) {
	    SFlags flags = Style_Default;

	    qDrawWinPanel( p, r, cg, true, ( elementFlags & CEF_IsEnabled ) ?
			   &cg.brush( TQColorGroup::Base ):
			   &cg.brush( TQColorGroup::Background ) );

	    TQRect ar =
		TQStyle::visualRect( querySubControlMetrics( CC_ComboBox, ceData, elementFlags,
							    SC_ComboBoxArrow, TQStyleOption::Default, widget ), ceData, elementFlags );
	    if ( subActive == SC_ComboBoxArrow ) {
		p->setPen( cg.dark() );
		p->setBrush( cg.brush( TQColorGroup::Button ) );
		p->drawRect( ar );
	    } else
		qDrawWinPanel( p, ar, cg, false,
			       &cg.brush( TQColorGroup::Button ) );

	    ar.addCoords( 2, 2, -2, -2 );
	    if ( elementFlags & CEF_IsEnabled )
		flags |= Style_Enabled;

	    if ( subActive == SC_ComboBoxArrow ) {
		flags |= Style_Sunken;
	    }
	    drawPrimitive( PE_ArrowDown, p, ceData, elementFlags, ar, cg, flags );
	}

	if ( sub & SC_ComboBoxEditField ) {
	    TQRect re =
		TQStyle::visualRect( querySubControlMetrics( CC_ComboBox, ceData, elementFlags,
							    SC_ComboBoxEditField, TQStyleOption::Default, widget ), ceData, elementFlags );
	    if ( ( elementFlags & CEF_HasFocus ) && (!( elementFlags & CEF_IsEditable )) )
		p->fillRect( re.x(), re.y(), re.width(), re.height(),
			     cg.brush( TQColorGroup::Highlight ) );

	    if ( elementFlags & CEF_HasFocus ) {
		p->setPen( cg.highlightedText() );
		p->setBackgroundColor( cg.highlight() );

	    } else {
		p->setPen( cg.text() );
		p->setBackgroundColor( cg.background() );
	    }

	    if ( ( elementFlags & CEF_HasFocus ) && (!( elementFlags & CEF_IsEditable )) ) {
		TQRect re =
		    TQStyle::visualRect( subRect( SR_ComboBoxFocusRect, ceData, elementFlags, widget ), ceData, elementFlags );
		drawPrimitive( PE_FocusRect, p, ceData, elementFlags, re, cg, Style_FocusAtBorder, TQStyleOption(cg.highlight()));
	    }
	}

	break;
#endif	// TQT_NO_COMBOBOX

#ifndef TQT_NO_SLIDER
    case CC_Slider:
	{
	    int thickness  = pixelMetric( PM_SliderControlThickness, ceData, elementFlags, widget );
	    int len        = pixelMetric( PM_SliderLength, ceData, elementFlags, widget );
	    int ticks = ceData.tickMarkSetting;

	    TQRect groove = querySubControlMetrics(CC_Slider, ceData, elementFlags, SC_SliderGroove,
						  opt, widget),
		  handle = querySubControlMetrics(CC_Slider, ceData, elementFlags, SC_SliderHandle,
						  opt, widget);

	    if ((sub & SC_SliderGroove) && groove.isValid()) {
		int mid = thickness / 2;

		if ( ticks & TQSlider::Above )
		    mid += len / 8;
		if ( ticks & TQSlider::Below )
		    mid -= len / 8;

		p->setPen( cg.shadow() );
		if ( ceData.orientation == Horizontal ) {
		    qDrawWinPanel( p, groove.x(), groove.y() + mid - 2,
				   groove.width(), 4, cg, true );
		    p->drawLine( groove.x() + 1, groove.y() + mid - 1,
				 groove.x() + groove.width() - 3, groove.y() + mid - 1 );
		} else {
		    qDrawWinPanel( p, groove.x() + mid - 2, groove.y(),
				   4, groove.height(), cg, true );
		    p->drawLine( groove.x() + mid - 1, groove.y() + 1,
				 groove.x() + mid - 1,
				 groove.y() + groove.height() - 3 );
		}
	    }

	    if (sub & SC_SliderTickmarks)
		TQCommonStyle::drawComplexControl(ctrl, p, ceData, elementFlags, r, cg, flags,
						 SC_SliderTickmarks, subActive,
						 opt, widget );

	    if ( sub & SC_SliderHandle ) {
		// 4444440
		// 4333310
		// 4322210
		// 4322210
		// 4322210
		// 4322210
		// *43210*
		// **410**
		// ***0***
		const TQColor c0 = cg.shadow();
		const TQColor c1 = cg.dark();
		// const TQColor c2 = g.button();
		const TQColor c3 = cg.midlight();
		const TQColor c4 = cg.light();

		int x = handle.x(), y = handle.y(),
		   wi = handle.width(), he = handle.height();

		int x1 = x;
		int x2 = x+wi-1;
		int y1 = y;
		int y2 = y+he-1;

		Orientation orient = ceData.orientation;
		bool tickAbove = ceData.tickMarkSetting == TQSlider::Above;
		bool tickBelow = ceData.tickMarkSetting == TQSlider::Below;

		p->fillRect( x, y, wi, he, cg.brush( TQColorGroup::Background ) );

		if ( flags & Style_HasFocus ) {
		    TQRect re = subRect( SR_SliderFocusRect, ceData, elementFlags, widget );
		    drawPrimitive( PE_FocusRect, p, ceData, elementFlags, re, cg );
		}

		if ( (tickAbove && tickBelow) || (!tickAbove && !tickBelow) ) {
		    qDrawWinButton( p, TQRect(x,y,wi,he), cg, false,
				    &cg.brush( TQColorGroup::Button ) );
		    return;
		}

		TQSliderDirection dir;

		if ( orient == Horizontal )
		    if ( tickAbove )
			dir = SlUp;
		    else
			dir = SlDown;
		else
		    if ( tickAbove )
			dir = SlLeft;
		    else
			dir = SlRight;

		TQPointArray a;

		int d = 0;
		switch ( dir ) {
		case SlUp:
		    y1 = y1 + wi/2;
		    d =  (wi + 1) / 2 - 1;
		    a.setPoints(5, x1,y1, x1,y2, x2,y2, x2,y1, x1+d,y1-d );
		    break;
		case SlDown:
		    y2 = y2 - wi/2;
		    d =  (wi + 1) / 2 - 1;
		    a.setPoints(5, x1,y1, x1,y2, x1+d,y2+d, x2,y2, x2,y1 );
		    break;
		case SlLeft:
		    d =  (he + 1) / 2 - 1;
		    x1 = x1 + he/2;
		    a.setPoints(5, x1,y1, x1-d,y1+d, x1,y2, x2,y2, x2,y1);
		    break;
		case SlRight:
		    d =  (he + 1) / 2 - 1;
		    x2 = x2 - he/2;
		    a.setPoints(5, x1,y1, x1,y2, x2,y2, x2+d,y1+d, x2,y1 );
		    break;
		}

		TQBrush oldBrush = p->brush();
		p->setBrush( cg.brush( TQColorGroup::Button ) );
		p->setPen( NoPen );
		p->drawRect( x1, y1, x2-x1+1, y2-y1+1 );
		p->drawPolygon( a );
		p->setBrush( oldBrush );

		if ( dir != SlUp ) {
		    p->setPen( c4 );
		    p->drawLine( x1, y1, x2, y1 );
		    p->setPen( c3 );
		    p->drawLine( x1, y1+1, x2, y1+1 );
		}
		if ( dir != SlLeft ) {
		    p->setPen( c3 );
		    p->drawLine( x1+1, y1+1, x1+1, y2 );
		    p->setPen( c4 );
		    p->drawLine( x1, y1, x1, y2 );
		}
		if ( dir != SlRight ) {
		    p->setPen( c0 );
		    p->drawLine( x2, y1, x2, y2 );
		    p->setPen( c1 );
		    p->drawLine( x2-1, y1+1, x2-1, y2-1 );
		}
		if ( dir != SlDown ) {
		    p->setPen( c0 );
		    p->drawLine( x1, y2, x2, y2 );
		    p->setPen( c1 );
		    p->drawLine( x1+1, y2-1, x2-1, y2-1 );
		}

		switch ( dir ) {
		case SlUp:
		    p->setPen( c4 );
		    p->drawLine( x1, y1, x1+d, y1-d);
		    p->setPen( c0 );
		    d = wi - d - 1;
		    p->drawLine( x2, y1, x2-d, y1-d);
		    d--;
		    p->setPen( c3 );
		    p->drawLine( x1+1, y1, x1+1+d, y1-d );
		    p->setPen( c1 );
		    p->drawLine( x2-1, y1, x2-1-d, y1-d);
		    break;
		case SlDown:
		    p->setPen( c4 );
		    p->drawLine( x1, y2, x1+d, y2+d);
		    p->setPen( c0 );
		    d = wi - d - 1;
		    p->drawLine( x2, y2, x2-d, y2+d);
		    d--;
		    p->setPen( c3 );
		    p->drawLine( x1+1, y2, x1+1+d, y2+d );
		    p->setPen( c1 );
		    p->drawLine( x2-1, y2, x2-1-d, y2+d);
		    break;
		case SlLeft:
		    p->setPen( c4 );
		    p->drawLine( x1, y1, x1-d, y1+d);
		    p->setPen( c0 );
		    d = he - d - 1;
		    p->drawLine( x1, y2, x1-d, y2-d);
		    d--;
		    p->setPen( c3 );
		    p->drawLine( x1, y1+1, x1-d, y1+1+d );
		    p->setPen( c1 );
		    p->drawLine( x1, y2-1, x1-d, y2-1-d);
		    break;
		case SlRight:
		    p->setPen( c4 );
		    p->drawLine( x2, y1, x2+d, y1+d);
		    p->setPen( c0 );
		    d = he - d - 1;
		    p->drawLine( x2, y2, x2+d, y2-d);
		    d--;
		    p->setPen( c3 );
		    p->drawLine(  x2, y1+1, x2+d, y1+1+d );
		    p->setPen( c1 );
		    p->drawLine( x2, y2-1, x2+d, y2-1-d);
		    break;
		}
	    }

	    break;
	}
#endif // TQT_NO_SLIDER

    default:
	TQCommonStyle::drawComplexControl( ctrl, p, ceData, elementFlags, r, cg, flags, sub,
					  subActive, opt, widget );
	break;
    }
}


/*! \reimp */
int TQWindowsStyle::styleHint( StyleHint hint,
			      const TQStyleControlElementData &ceData,
			      ControlElementFlags elementFlags,
			      const TQStyleOption &opt,
			      TQStyleHintReturn *returnData,
			      const TQWidget *widget ) const
{
    int ret;

    switch (hint) {
    case SH_EtchDisabledText:
    case SH_Slider_SnapToValue:
    case SH_PrintDialog_RightAlignButtons:
    case SH_MainWindow_SpaceBelowMenuBar:
    case SH_FontDialog_SelectAssociatedText:
    case SH_PopupMenu_AllowActiveAndDisabled:
    case SH_MenuBar_AltKeyNavigation:
    case SH_MenuBar_MouseTracking:
    case SH_PopupMenu_MouseTracking:
    case SH_ComboBox_ListMouseTracking:
    case SH_ScrollBar_StopMouseOverSlider:
	ret = 1;
	break;

    case SH_ItemView_ChangeHighlightOnFocus:
	    ret = 0;
	break;

    case SH_ToolBox_SelectedPageTitleBold:
	ret = 0;
	break;

    case SH_MenuIndicatorColumnWidth:
        {
	    int maxpmw = opt.maxIconWidth();
	    bool checkable = ( elementFlags & CEF_IsCheckable );
	
	    if ( checkable ) {
		// space for the checkmarks
	    if (use2000style)
		maxpmw = TQMAX( maxpmw, 20 );
	    else
		maxpmw = TQMAX( maxpmw, 12 );
	    }
	
	    ret = maxpmw;
	    break;
	}

    default:
	ret = TQCommonStyle::styleHint(hint, ceData, elementFlags, opt, returnData, widget);
	break;
    }

    return ret;
}

/*! \reimp */
TQRect TQWindowsStyle::subRect(SubRect r, const TQStyleControlElementData &ceData, const ControlElementFlags elementFlags, const TQWidget *widget) const
{
    TQRect rect;

    switch (r) {
#ifndef TQT_NO_SLIDER
    case SR_SliderFocusRect:
	{
	    rect = ceData.rect;
	    break;
	}
#endif // TQT_NO_SLIDER
    case SR_ToolBoxTabContents:
	rect = ceData.rect;
	break;
    default:
	rect = TQCommonStyle::subRect( r, ceData, elementFlags, widget );
	break;
    }

    return rect;
}

#endif
