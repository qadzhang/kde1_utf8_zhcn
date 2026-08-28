/****************************************************************************
**
** Implementation of some TQt private functions.
**
** Created : 001101
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "qplatformdefs.h"

#include "qtitlebar_p.h"

#ifndef TQT_NO_TITLEBAR

#include <ntqcursor.h>
#include "ntqapplication.h"
#include "ntqstyle.h"
#include "ntqdatetime.h"
#include "private/qapplication_p.h"
#include "ntqtooltip.h"
#include "ntqimage.h"
#include "ntqtimer.h"
#include "ntqpainter.h"
#include "ntqstyle.h"
#include "private/qinternal_p.h"
#ifndef TQT_NO_WORKSPACE
#include "ntqworkspace.h"
#endif
#if defined(TQ_WS_WIN)
#include "qt_windows.h"
#endif

#ifndef TQT_NO_TOOLTIP
class TQTitleBarTip : public TQToolTip
{
public:
    TQTitleBarTip( TQWidget * parent ) : TQToolTip( parent ) { }

    void maybeTip( const TQPoint &pos )
    {
	if ( !::tqt_cast<TQTitleBar*>(parentWidget()) )
	    return;
	TQTitleBar *t = (TQTitleBar *)parentWidget();

	TQString tipstring;
	TQStyle::SubControl ctrl = t->style().querySubControl(TQStyle::CC_TitleBar, t, pos);
	TQSize controlSize = t->style().querySubControlMetrics(TQStyle::CC_TitleBar, t, ctrl).size();

	TQWidget *window = t->window();
	if ( window ) {
	    switch(ctrl) {
	    case TQStyle::SC_TitleBarSysMenu:
		if ( t->testWFlags( WStyle_SysMenu ) )
		    tipstring = TQTitleBar::tr( "System Menu" );
		break;

	    case TQStyle::SC_TitleBarShadeButton:
		if ( t->testWFlags( WStyle_Tool ) && t->testWFlags( WStyle_MinMax ) )
		    tipstring = TQTitleBar::tr( "Shade" );
		break;

	    case TQStyle::SC_TitleBarUnshadeButton:
		if ( t->testWFlags( WStyle_Tool ) && t->testWFlags( WStyle_MinMax ) )
		    tipstring = TQTitleBar::tr( "Unshade" );
		break;

	    case TQStyle::SC_TitleBarNormalButton:
	    case TQStyle::SC_TitleBarMinButton:
		if ( !t->testWFlags( WStyle_Tool ) && t->testWFlags( WStyle_Minimize ) ) {
		    if( window->isMinimized() )
			tipstring = TQTitleBar::tr( "Normalize" );
		    else
			tipstring = TQTitleBar::tr( "Minimize" );
		}
		break;

	    case TQStyle::SC_TitleBarMaxButton:
		if ( !t->testWFlags( WStyle_Tool ) && t->testWFlags( WStyle_Maximize ) )
		    tipstring = TQTitleBar::tr( "Maximize" );
		break;

	    case TQStyle::SC_TitleBarCloseButton:
		if ( t->testWFlags( WStyle_SysMenu ) )
		    tipstring = TQTitleBar::tr( "Close" );
		break;

	    default:
		break;
	    }
	}
#ifndef TQT_NO_WIDGET_TOPEXTRA
	if ( tipstring.isEmpty() ) {
	    if ( t->visibleText() != t->caption() )
		tipstring = t->caption();
	}
#endif
	if(!tipstring.isEmpty())
	    tip( TQRect(pos, controlSize), tipstring );
    }
};
#endif

class TQTitleBarPrivate
{
public:
    TQTitleBarPrivate()
	: toolTip( 0 ), act( 0 ), window( 0 ), movable( 1 ), pressed( 0 ), autoraise(0)
    {
    }

    TQStyle::SCFlags buttonDown;
    TQPoint moveOffset;
    TQToolTip *toolTip;
    bool act		    :1;
    TQWidget* window;
    bool movable            :1;
    bool pressed            :1;
    bool autoraise          :1;
    TQString cuttext;
#ifdef TQT_NO_WIDGET_TOPEXTRA
    TQString cap;
#endif
};

TQTitleBar::TQTitleBar(TQWidget* w, TQWidget* parent, const char* name)
    : TQWidget( parent, name, WStyle_Customize | WStyle_NoBorder | WNoAutoErase )
{
    d = new TQTitleBarPrivate();

#ifndef TQT_NO_TOOLTIP
    d->toolTip = new TQTitleBarTip( this );
#endif
    d->window = w;
    d->buttonDown = TQStyle::SC_None;
    d->act = 0;
    if ( w ) {
	setWFlags( ((TQTitleBar*)w)->getWFlags() | WNoAutoErase );
	if ( w->minimumSize() == w->maximumSize() )
	    clearWFlags( WStyle_Maximize );
#ifndef TQT_NO_WIDGET_TOPEXTRA
    	setCaption( w->caption() );
#endif
    } else {
	setWFlags( WStyle_Customize | WNoAutoErase );
    }

    readColors();
    setSizePolicy( TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Fixed ) );
    setMouseTracking(true);
}

TQTitleBar::~TQTitleBar()
{
#ifndef TQT_NO_TOOLTIP
    delete d->toolTip;
#endif

    delete d;
    d = 0;
}

#ifdef TQ_WS_WIN
extern TQRgb qt_colorref2qrgb(COLORREF col);
#endif

void TQTitleBar::readColors()
{
    TQPalette pal = palette();

    bool colorsInitialized = false;

#ifdef TQ_WS_WIN // ask system properties on windows
#ifndef SPI_GETGRADIENTCAPTIONS
#define SPI_GETGRADIENTCAPTIONS 0x1008
#endif
#ifndef COLOR_GRADIENTACTIVECAPTION
#define COLOR_GRADIENTACTIVECAPTION 27
#endif
#ifndef COLOR_GRADIENTINACTIVECAPTION
#define COLOR_GRADIENTINACTIVECAPTION 28
#endif
    if ( TQApplication::desktopSettingsAware() ) {
	pal.setColor( TQPalette::Active, TQColorGroup::Highlight, qt_colorref2qrgb(GetSysColor(COLOR_ACTIVECAPTION)) );
	pal.setColor( TQPalette::Inactive, TQColorGroup::Highlight, qt_colorref2qrgb(GetSysColor(COLOR_INACTIVECAPTION)) );
	pal.setColor( TQPalette::Active, TQColorGroup::HighlightedText, qt_colorref2qrgb(GetSysColor(COLOR_CAPTIONTEXT)) );
	pal.setColor( TQPalette::Inactive, TQColorGroup::HighlightedText, qt_colorref2qrgb(GetSysColor(COLOR_INACTIVECAPTIONTEXT)) );
	if ( tqt_winver != TQt::WV_95 && tqt_winver != WV_NT ) {
	    colorsInitialized = true;
	    BOOL gradient;
	    QT_WA( {
		SystemParametersInfo( SPI_GETGRADIENTCAPTIONS, 0, &gradient, 0 );
	    } , {
		SystemParametersInfoA( SPI_GETGRADIENTCAPTIONS, 0, &gradient, 0 );
	    } );
	    if ( gradient ) {
		pal.setColor( TQPalette::Active, TQColorGroup::Base, qt_colorref2qrgb(GetSysColor(COLOR_GRADIENTACTIVECAPTION)) );
		pal.setColor( TQPalette::Inactive, TQColorGroup::Base, qt_colorref2qrgb(GetSysColor(COLOR_GRADIENTINACTIVECAPTION)) );
	    } else {
		pal.setColor( TQPalette::Active, TQColorGroup::Base, palette().active().highlight() );
		pal.setColor( TQPalette::Inactive, TQColorGroup::Base, palette().inactive().highlight() );
	    }
	}
    }
#endif // TQ_WS_WIN
    if ( !colorsInitialized ) {
	pal.setColor( TQPalette::Active, TQColorGroup::Highlight, palette().active().highlight() );
	pal.setColor( TQPalette::Active, TQColorGroup::Base, palette().active().highlight() );
	pal.setColor( TQPalette::Inactive, TQColorGroup::Highlight, palette().inactive().dark() );
	pal.setColor( TQPalette::Inactive, TQColorGroup::Base, palette().inactive().dark() );
	pal.setColor( TQPalette::Inactive, TQColorGroup::HighlightedText, palette().inactive().background() );
    }

    setPalette( pal );
    setActive( d->act );
}

void TQTitleBar::mousePressEvent( TQMouseEvent * e)
{
    if ( !d->act )
	emit doActivate();
    if ( e->button() == LeftButton ) {
	d->pressed = true;
	TQStyle::SCFlags ctrl = style().querySubControl(TQStyle::CC_TitleBar, this, e->pos());
	switch (ctrl) {
	case TQStyle::SC_TitleBarSysMenu:
	    if ( testWFlags( WStyle_SysMenu ) && !testWFlags( WStyle_Tool ) ) {
		d->buttonDown = TQStyle::SC_None;
		static TQTime* t = 0;
		static TQTitleBar* tc = 0;
		if ( !t )
		    t = new TQTime;
		if ( tc != this || t->elapsed() > TQApplication::doubleClickInterval() ) {
		    emit showOperationMenu();
		    t->start();
		    tc = this;
		} else {
		    tc = 0;
		    emit doClose();
		    return;
		}
	    }
	    break;

	case TQStyle::SC_TitleBarShadeButton:
	case TQStyle::SC_TitleBarUnshadeButton:
	    if ( testWFlags( WStyle_MinMax ) && testWFlags( WStyle_Tool ) )
		d->buttonDown = ctrl;
	    break;

	case TQStyle::SC_TitleBarNormalButton:
	    if( testWFlags( WStyle_Minimize ) && !testWFlags( WStyle_Tool ) )
		d->buttonDown = ctrl;
	    break;

	case TQStyle::SC_TitleBarMinButton:
	    if( testWFlags( WStyle_Minimize ) && !testWFlags( WStyle_Tool ) )
		d->buttonDown = ctrl;
	    break;

	case TQStyle::SC_TitleBarMaxButton:
	    if ( testWFlags( WStyle_Maximize ) && !testWFlags( WStyle_Tool ) )
		d->buttonDown = ctrl;
	    break;

	case TQStyle::SC_TitleBarCloseButton:
	    if ( testWFlags( WStyle_SysMenu ) )
		d->buttonDown = ctrl;
	    break;

	case TQStyle::SC_TitleBarLabel:
	    d->buttonDown = ctrl;
	    d->moveOffset = mapToParent( e->pos() );
	    break;

	default:
	    break;
	}
	repaint( false );
    } else {
	d->pressed = false;
    }
}

void TQTitleBar::contextMenuEvent( TQContextMenuEvent *e )
{
    TQStyle::SCFlags ctrl = style().querySubControl(TQStyle::CC_TitleBar, this, e->pos());
    if( ctrl == TQStyle::SC_TitleBarLabel || ctrl == TQStyle::SC_TitleBarSysMenu )
	emit popupOperationMenu(e->globalPos());
    else
	e->ignore();
}

void TQTitleBar::mouseReleaseEvent( TQMouseEvent * e)
{
    if ( e->button() == LeftButton && d->pressed) {
	TQStyle::SCFlags ctrl = style().querySubControl(TQStyle::CC_TitleBar, this, e->pos());

	if (ctrl == d->buttonDown) {
	    switch(ctrl) {
	    case TQStyle::SC_TitleBarShadeButton:
	    case TQStyle::SC_TitleBarUnshadeButton:
		if( testWFlags( WStyle_MinMax ) && testWFlags( WStyle_Tool ) )
		    emit doShade();
		break;

	    case TQStyle::SC_TitleBarNormalButton:
		if( testWFlags( WStyle_MinMax ) && !testWFlags( WStyle_Tool ) )
		    emit doNormal();
		break;

	    case TQStyle::SC_TitleBarMinButton:
		if( testWFlags( WStyle_Minimize ) && !testWFlags( WStyle_Tool ) )
		    emit doMinimize();
		break;

	    case TQStyle::SC_TitleBarMaxButton:
		if( d->window && testWFlags( WStyle_Maximize ) && !testWFlags( WStyle_Tool ) ) {
		    if(d->window->isMaximized())
			emit doNormal();
		    else
			emit doMaximize();
		}
		break;

	    case TQStyle::SC_TitleBarCloseButton:
		if( testWFlags( WStyle_SysMenu ) ) {
		    d->buttonDown = TQStyle::SC_None;
		    repaint(false);
		    emit doClose();
		    return;
		}
		break;

	    default:
		break;
	    }
	}
	d->buttonDown = TQStyle::SC_None;
	repaint(false);
	d->pressed = false;
    }
}

void TQTitleBar::mouseMoveEvent( TQMouseEvent * e)
{
    switch (d->buttonDown) {
    case TQStyle::SC_None:
	if(autoRaise())
	    repaint( false );
	break;
    case TQStyle::SC_TitleBarSysMenu:
	break;
    case TQStyle::SC_TitleBarShadeButton:
    case TQStyle::SC_TitleBarUnshadeButton:
    case TQStyle::SC_TitleBarNormalButton:
    case TQStyle::SC_TitleBarMinButton:
    case TQStyle::SC_TitleBarMaxButton:
    case TQStyle::SC_TitleBarCloseButton:
	{
	    TQStyle::SCFlags last_ctrl = d->buttonDown;
	    d->buttonDown = style().querySubControl(TQStyle::CC_TitleBar, this, e->pos());
	    if( d->buttonDown != last_ctrl)
		d->buttonDown = TQStyle::SC_None;
	    repaint(false);
	    d->buttonDown = last_ctrl;
	}
	break;

    case TQStyle::SC_TitleBarLabel:
	if ( d->buttonDown == TQStyle::SC_TitleBarLabel && d->movable && d->pressed ) {
	    if ( (d->moveOffset - mapToParent( e->pos() ) ).manhattanLength() >= 4 ) {
		TQPoint p = mapFromGlobal(e->globalPos());
#ifndef TQT_NO_WORKSPACE
		if(d->window && d->window->parentWidget()->inherits("TQWorkspaceChild")) {
		    TQWorkspace *workspace = ::tqt_cast<TQWorkspace*>(d->window->parentWidget()->parentWidget());
		    if(workspace) {
			p = workspace->mapFromGlobal( e->globalPos() );
			if ( !workspace->rect().contains(p) ) {
			    if ( p.x() < 0 )
				p.rx() = 0;
			    if ( p.y() < 0 )
				p.ry() = 0;
			    if ( p.x() > workspace->width() )
				p.rx() = workspace->width();
			    if ( p.y() > workspace->height() )
				p.ry() = workspace->height();
			}
		    }
		}
#endif
		TQPoint pp = p - d->moveOffset;
		if (!parentWidget()->isMaximized())
		    parentWidget()->move( pp );
	    }
	} else {
	    TQStyle::SCFlags last_ctrl = d->buttonDown;
	    d->buttonDown = TQStyle::SC_None;
	    if( d->buttonDown != last_ctrl)
		repaint(false);
	}
	break;
    }
}

void TQTitleBar::resizeEvent( TQResizeEvent *r)
{
    TQWidget::resizeEvent(r);
    cutText();
}

void TQTitleBar::paintEvent(TQPaintEvent *)
{
    TQStyle::SCFlags ctrls = TQStyle::SC_TitleBarLabel;
    if ( testWFlags( WStyle_SysMenu) ) {
	if ( testWFlags( WStyle_Tool ) ) {
	    ctrls |= TQStyle::SC_TitleBarCloseButton;
	    if ( d->window && testWFlags( WStyle_MinMax ) ) {
		if ( d->window->isMinimized() )
		    ctrls |= TQStyle::SC_TitleBarUnshadeButton;
		else
		    ctrls |= TQStyle::SC_TitleBarShadeButton;
	    }
	} else {
	    ctrls |= TQStyle::SC_TitleBarSysMenu | TQStyle::SC_TitleBarCloseButton;
	    if ( d->window && testWFlags( WStyle_Minimize ) ) {
		if( d->window && d->window->isMinimized() )
		    ctrls |= TQStyle::SC_TitleBarNormalButton;
		else
		    ctrls |= TQStyle::SC_TitleBarMinButton;
	    }
	    if ( d->window && testWFlags( WStyle_Maximize ) && !d->window->isMaximized() )
		ctrls |= TQStyle::SC_TitleBarMaxButton;
	}
    }

    TQStyle::SCFlags under_mouse = TQStyle::SC_None;
    if( autoRaise() && hasMouse() ) {
	TQPoint p(mapFromGlobal(TQCursor::pos()));
	under_mouse = style().querySubControl(TQStyle::CC_TitleBar, this, p);
	ctrls ^= under_mouse;
    }

    TQSharedDoubleBuffer buffer( this, rect() );
    style().drawComplexControl(TQStyle::CC_TitleBar, buffer.painter(), this, rect(),
			       colorGroup(),
			       isEnabled() ? TQStyle::Style_Enabled :
			       TQStyle::Style_Default, ctrls, d->buttonDown);
    if(under_mouse != TQStyle::SC_None)
	style().drawComplexControl(TQStyle::CC_TitleBar, buffer.painter(), this, rect(),
				   colorGroup(),
				   TQStyle::Style_MouseOver |
				   (isEnabled() ? TQStyle::Style_Enabled : 0),
				   under_mouse, d->buttonDown);
}

void TQTitleBar::mouseDoubleClickEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;

    switch(style().querySubControl(TQStyle::CC_TitleBar, this, e->pos())) {
    case TQStyle::SC_TitleBarLabel:
	emit doubleClicked();
	break;

    case TQStyle::SC_TitleBarSysMenu:
	if ( testWFlags( WStyle_SysMenu ) )
	    emit doClose();
	break;

    default:
	break;
    }
}

#ifdef TQT_NO_WIDGET_TOPEXTRA
// We provide one, since titlebar is useless otherwise.
TQString TQTitleBar::caption() const
{
    return d->cap;
}
#endif

void TQTitleBar::cutText()
{
    TQFontMetrics fm( font() );

    int maxw = style().querySubControlMetrics(TQStyle::CC_TitleBar, this,
					      TQStyle::SC_TitleBarLabel).width();
    if ( !d->window )
	maxw = width() - 20;
    const TQString txt = caption();
    d->cuttext = txt;
    if ( fm.width( txt + "m" ) > maxw ) {
	int i = txt.length();
	int dotlength = fm.width( "..." );
	while ( i>0 && fm.width(txt.left( i )) + dotlength > maxw )
	    i--;
	if(i != (int)txt.length())
	    d->cuttext = txt.left( i ) + "...";
    }
}

void TQTitleBar::setCaption( const TQString& title )
{
    if( caption() == title)
	return;
#ifndef TQT_NO_WIDGET_TOPEXTRA
    TQWidget::setCaption( title );
#else
    d->cap = title;
#endif
    cutText();

    update();
}


void TQTitleBar::setIcon( const TQPixmap& icon )
{
#ifndef TQT_NO_WIDGET_TOPEXTRA
#ifndef TQT_NO_IMAGE_SMOOTHSCALE
    TQRect menur = style().querySubControlMetrics(TQStyle::CC_TitleBar, this,
						  TQStyle::SC_TitleBarSysMenu);

    TQPixmap theIcon;
    if (icon.width() > menur.width()) {
	// try to keep something close to the same aspect
	int aspect = (icon.height() * 100) / icon.width();
	int newh = (aspect * menur.width()) / 100;
	theIcon.convertFromImage( icon.convertToImage().smoothScale(menur.width(),
								   newh) );
    } else if (icon.height() > menur.height()) {
	// try to keep something close to the same aspect
	int aspect = (icon.width() * 100) / icon.height();
	int neww = (aspect * menur.height()) / 100;
	theIcon.convertFromImage( icon.convertToImage().smoothScale(neww,
								   menur.height()) );
    } else
	theIcon = icon;

    TQWidget::setIcon( theIcon );
#else
    TQWidget::setIcon( icon );
#endif

    update();
#endif
}

void TQTitleBar::leaveEvent( TQEvent * )
{
    if(autoRaise() && !d->pressed)
	repaint( false );
}

void TQTitleBar::enterEvent( TQEvent * )
{
    if(autoRaise() && !d->pressed)
	repaint( false );
    TQEvent e( TQEvent::Leave );
    TQApplication::sendEvent( parentWidget(), &e );
}

void TQTitleBar::setActive( bool active )
{
    if ( d->act == active )
	return ;

    d->act = active;
    update();
}

bool TQTitleBar::isActive() const
{
    return d->act;
}

bool TQTitleBar::usesActiveColor() const
{
    return ( isActive() && isActiveWindow() ) ||
	   ( !window() && topLevelWidget()->isActiveWindow() );
}

TQString TQTitleBar::visibleText() const
{
    return d->cuttext;
}

TQWidget *TQTitleBar::window() const
{
    return d->window;
}

bool TQTitleBar::event( TQEvent* e )
{
    if ( e->type() == TQEvent::ApplicationPaletteChange ) {
	readColors();
	return true;
    } else if ( e->type() == TQEvent::WindowActivate ) {
	setActive( d->act );
    } else if ( e->type() == TQEvent::WindowDeactivate ) {
	bool wasActive = d->act;
	setActive( false );
	d->act = wasActive;
    }

    return TQWidget::event( e );
}

void TQTitleBar::setMovable(bool b)
{
    d->movable = b;
}

bool TQTitleBar::isMovable() const
{
    return d->movable;
}

void TQTitleBar::setAutoRaise(bool b)
{
    d->autoraise = b;
}

bool TQTitleBar::autoRaise() const
{
    return d->autoraise;
}

TQSize TQTitleBar::sizeHint() const
{
    constPolish();
    TQRect menur = style().querySubControlMetrics(TQStyle::CC_TitleBar, this,
						 TQStyle::SC_TitleBarSysMenu);
    return TQSize( menur.width(), style().pixelMetric( TQStyle::PM_TitleBarHeight, this ) );
}

#endif //TQT_NO_TITLEBAR
