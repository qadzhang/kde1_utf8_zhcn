/****************************************************************************
**
** Implementation of compact style class
**
** Created : 006231
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

#include "ntqcompactstyle.h"

#if !defined(TQT_NO_STYLE_COMPACT) || defined(QT_PLUGIN)

#include "ntqfontmetrics.h"
#include "ntqpalette.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqmenudata.h"
#include "ntqpopupmenu.h"

TQCompactStyle::TQCompactStyle()
: TQWindowsStyle()
{
}

/*! \reimp */
int TQCompactStyle::pixelMetric( PixelMetric metric, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQWidget *widget )
{
    int ret;
    switch ( metric ) {
    case PM_ButtonMargin:
	ret = 2;
	break;
	// tws - I added this in to stop this "Windows Scroll behaivor."  Remove it
	// if you don't want it.
    case PM_MaximumDragDistance:
	ret = -1;
	break;
    case PM_MenuIndicatorFrameHBorder:
    case PM_MenuIndicatorFrameVBorder:
    case PM_MenuIconIndicatorFrameHBorder:
    case PM_MenuIconIndicatorFrameVBorder:
	ret = 0;
	break;
    default:
	ret = TQWindowsStyle::pixelMetric( metric, ceData, elementFlags, widget );
	break;
    }
    return ret;
}

static const int motifItemFrame		= 0;	// menu item frame width
static const int motifSepHeight		= 2;	// separator item height
static const int motifItemHMargin	= 1;	// menu item hor text margin
static const int motifItemVMargin	= 2;	// menu item ver text margin
static const int motifArrowHMargin	= 0;	// arrow horizontal margin
static const int motifTabSpacing	= 4;	// space between text and tab
static const int motifCheckMarkHMargin	= 1;	// horiz. margins of check mark
static const int windowsRightBorder	= 8;    // right border on windows
static const int windowsCheckMarkWidth  = 2;    // checkmarks width on windows

#if 0
static int extraPopupMenuItemWidth( bool checkable, int maxpmw, TQMenuItem* mi, const TQFontMetrics& /*fm*/ )
{
    int w = 2*motifItemHMargin + 2*motifItemFrame; // a little bit of border can never harm

    if ( mi->isSeparator() )
	return 10; // arbitrary
    else if ( mi->pixmap() )
	w += mi->pixmap()->width();	// pixmap only

    if ( !mi->text().isNull() ) {
	if ( mi->text().find('\t') >= 0 )	// string contains tab
	    w += motifTabSpacing;
    }

    if ( maxpmw ) { // we have iconsets
	w += maxpmw;
	w += 6; // add a little extra border around the iconset
    }

    if ( checkable && maxpmw < windowsCheckMarkWidth ) {
	w += windowsCheckMarkWidth - maxpmw; // space for the checkmarks
    }

    if ( maxpmw > 0 || checkable ) // we have a check-column ( iconsets or checkmarks)
	w += motifCheckMarkHMargin; // add space to separate the columns

    w += windowsRightBorder; // windows has a strange wide border on the right side

    return w;
}

static int popupMenuItemHeight( bool /*checkable*/, TQMenuItem* mi, const TQFontMetrics& fm )
{
    int h = 0;
    if ( mi->isSeparator() )			// separator height
	h = motifSepHeight;
    else if ( mi->pixmap() )		// pixmap height
	h = mi->pixmap()->height() + 2*motifItemFrame;
    else					// text height
	h = fm.height() + 2*motifItemVMargin + 2*motifItemFrame - 1;

    if ( !mi->isSeparator() && mi->iconSet() != 0 ) {
	h = TQMAX( h, mi->iconSet()->pixmap( TQIconSet::Small, TQIconSet::Normal ).height() + 2*motifItemFrame );
    }
    if ( mi->custom() )
	h = TQMAX( h, mi->custom()->sizeHint().height() + 2*motifItemVMargin + 2*motifItemFrame ) - 1;
    return h;
}
#endif

void drawPopupMenuItem( TQPainter*, bool,
			int, int, TQMenuItem*,
			const TQPalette&, bool,
			bool,
			int, int, int, int)
{

}

/*! \reimp */
void TQCompactStyle::drawControl( ControlElement element, TQPainter *p, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQRect &r,
		  const TQColorGroup &g, SFlags flags, const TQStyleOption& opt, const TQWidget *widget )
{
    switch ( element ) {
    case CE_PopupMenuItem:
	{
	    if ( (elementFlags & CEF_UseGenericParameters) || opt.isDefault())
		break;

	    TQMenuItem *mi = opt.menuItem();
	    if ( !mi )
		break;

	    int tab = opt.tabWidth();
	    int maxpmw = opt.maxIconWidth();
	    bool dis = !(flags & Style_Enabled);
	    bool checkable = (elementFlags & CEF_IsCheckable);
	    bool act = flags & Style_Active;
	    int x, y, w, h;
	    r.rect( &x, &y, &w, &h );

	    TQColorGroup itemg = g;

	    if ( checkable )
		maxpmw = TQMAX( maxpmw, 8 ); // space for the checkmarks

	    int checkcol	  =     maxpmw;

	    if ( mi && mi->isSeparator() ) {			// draw separator
		p->setPen( g.dark() );
		p->drawLine( x, y, x+w, y );
		p->setPen( g.light() );
		p->drawLine( x, y+1, x+w, y+1 );
		return;
	    }

	    TQBrush fill = act? g.brush( TQColorGroup::Highlight ) :
				    g.brush( TQColorGroup::Button );
	    p->fillRect( x, y, w, h, fill);

	    if ( !mi )
		return;

	    if ( mi->isChecked() ) {
		drawPrimitive( PE_MenuItemIndicatorFrame, p, ceData, elementFlags, r, itemg, flags, opt );
	    } else if ( !act ) {
		p->fillRect(x, y, checkcol , h,
			    g.brush( TQColorGroup::Button ));
	    }

	    if ( mi->iconSet() ) {		// draw iconset
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
		if ( act && !dis ) {
		    if ( !mi->isChecked() )
			drawPrimitive( PE_MenuItemIndicatorIconFrame, p, ceData, elementFlags, r, itemg, flags, opt );
		}
		TQRect cr( x, y, checkcol, h );
		TQRect pmr( 0, 0, pixw, pixh );
		pmr.moveCenter( cr.center() );
		p->setPen( itemg.text() );
		p->drawPixmap( pmr.topLeft(), pixmap );

		TQBrush fill = act? g.brush( TQColorGroup::Highlight ) :
				      g.brush( TQColorGroup::Button );
		p->fillRect( x+checkcol + 1, y, w - checkcol - 1, h, fill);
	    } else  if ( checkable ) {	// just "checking"...
		if ( mi->isChecked() ) {
		    drawPrimitive( PE_MenuItemIndicatorCheck, p, ceData, elementFlags, r, itemg, flags, opt );
		}
	    }

	    p->setPen( act ? g.highlightedText() : g.buttonText() );

	    TQColor discol;
	    if ( dis ) {
		discol = itemg.text();
		p->setPen( discol );
	    }

	    int xm = motifItemFrame + checkcol + motifItemHMargin;

	    if ( mi->custom() ) {
		int m = motifItemVMargin;
		p->save();
		if ( dis && !act ) {
		    p->setPen( g.light() );
		    mi->custom()->paint( p, itemg, act, !dis,
					 x+xm+1, y+m+1, w-xm-tab+1, h-2*m );
		    p->setPen( discol );
		}
		mi->custom()->paint( p, itemg, act, !dis,
				     x+xm, y+m, w-xm-tab+1, h-2*m );
		p->restore();
	    }
	    TQString s = mi->text();
	    if ( !s.isNull() ) {			// draw text
		int t = s.find( '\t' );
		int m = motifItemVMargin;
		const int text_flags = AlignVCenter|ShowPrefix | DontClip | SingleLine;
		if ( t >= 0 ) {				// draw tab text
		    if ( dis && !act ) {
			p->setPen( g.light() );
			p->drawText( x+w-tab-windowsRightBorder-motifItemHMargin-motifItemFrame+1,
				     y+m+1, tab, h-2*m, text_flags, s.mid( t+1 ));
			p->setPen( discol );
		    }
		    p->drawText( x+w-tab-windowsRightBorder-motifItemHMargin-motifItemFrame,
				 y+m, tab, h-2*m, text_flags, s.mid( t+1 ) );
		    s = s.left( t );
		}
		if ( dis && !act ) {
		    p->setPen( g.light() );
		    p->drawText( x+xm+1, y+m+1, w-xm+1, h-2*m, text_flags, s, t );
		    p->setPen( discol );
		}
		p->drawText( x+xm, y+m, w-xm-tab+1, h-2*m, text_flags, s, t );
	    } else if ( mi->pixmap() ) {			// draw pixmap
		TQPixmap *pixmap = mi->pixmap();
		if ( pixmap->depth() == 1 )
		    p->setBackgroundMode( OpaqueMode );
		p->drawPixmap( x+xm, y+motifItemFrame, *pixmap );
		if ( pixmap->depth() == 1 )
		    p->setBackgroundMode( TransparentMode );
	    }
	    if ( mi->popup() ) {			// draw sub menu arrow
		int dim = (h-2*motifItemFrame) / 2;
		if ( act ) {
		    if ( !dis )
			discol = white;
		    TQColorGroup g2( discol, g.highlight(),
				    white, white,
				    dis ? discol : white,
				    discol, white );
		    drawPrimitive(PE_ArrowRight, p, ceData, elementFlags, TQRect(x+w - motifArrowHMargin - motifItemFrame - dim, y + h / 2 - dim / 2, dim, dim),
				  g2, Style_Enabled);
		} else {
		    drawPrimitive(PE_ArrowRight, p, ceData, elementFlags, TQRect(x+w - motifArrowHMargin - motifItemFrame - dim, y + h / 2 - dim / 2, dim, dim),
				  g, !dis ? Style_Enabled : Style_Default);
		}
	    }
	}
	break;

    default:
	TQWindowsStyle::drawControl( element, p, ceData, elementFlags, r, g, flags, opt, widget );
	break;
    }
}

/*! \reimp */
void TQCompactStyle::drawPrimitive( PrimitiveElement pe,
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

	int x, y, w, h;
	r.rect( &x, &y, &w, &h );
	
	switch (pe) {
		case PE_MenuItemIndicatorFrame:
			{
				int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags, opt, NULL, NULL);

				if ( act && !dis ) {
					qDrawShadePanel( p, x, y, checkcol, h, cg, true, 1, &cg.brush( TQColorGroup::Button ) );
				} else {
					qDrawShadePanel( p, x, y, checkcol, h, cg, true, 1, &cg.brush( TQColorGroup::Midlight ) );
				}
			}
			break;
		case PE_MenuItemIndicatorIconFrame:
			{
				int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags, opt, NULL, NULL);

				qDrawShadePanel( p, x, y, checkcol, h, cg, false,  1, &cg.brush( TQColorGroup::Button ) );
			}
			break;
		case PE_MenuItemIndicatorCheck:
			{
				int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags, opt, NULL, NULL);

				int mw = checkcol + motifItemFrame;
				int mh = h - 2*motifItemFrame;

				SFlags cflags = Style_Default;
				if (! dis) {
					cflags |= Style_Enabled;
				}
				if (act) {
					cflags |= Style_On;
				}
	
				drawPrimitive( PE_CheckMark, p, ceData, elementFlags, TQRect(x + motifItemFrame + 2, y + motifItemFrame, mw, mh), cg, cflags, opt );
			}
			break;
		default:
			break;
	}
}

/*! \reimp */
int TQCompactStyle::styleHint(StyleHint sh, const TQStyleControlElementData &ceData, ControlElementFlags elementFlags, const TQStyleOption &opt, TQStyleHintReturn *returnData, const TQWidget *w) const
{
	int ret;

	switch (sh) {
		case SH_MenuIndicatorColumnWidth:
			{
				int maxpmw = opt.maxIconWidth();
				bool checkable = (elementFlags & CEF_IsCheckable);
			
				if ( checkable )
					maxpmw = TQMAX( maxpmw, 8 ); // space for the checkmarks
			
				ret = maxpmw;
			}
			break;
		default:
			ret = TQWindowsStyle::styleHint(sh, ceData, elementFlags, opt, returnData, w);
			break;
	}

	return ret;
}

#endif
