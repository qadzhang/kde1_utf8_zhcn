/****************************************************************************
**
** Implementation of TQRadioButton class
**
** Created : 940222
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

#include "ntqradiobutton.h"
#ifndef TQT_NO_RADIOBUTTON
#include "ntqbuttongroup.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqpixmap.h"
#include "ntqpixmapcache.h"
#include "ntqbitmap.h"
#include "ntqtextstream.h"
#include "ntqapplication.h"
#include "ntqstyle.h"

/*!
    \class TQRadioButton ntqradiobutton.h
    \brief The TQRadioButton widget provides a radio button with a text or pixmap label.

    \ingroup basic
    \mainclass

    TQRadioButton and TQCheckBox are both option buttons. That is, they
    can be switched on (checked) or off (unchecked). The classes
    differ in how the choices for the user are restricted. Check boxes
    define "many of many" choices, whereas radio buttons provide a
    "one of many" choice. In a group of radio buttons only one radio
    button at a time can be checked; if the user selects another
    button, the previously selected button is switched off.

    The easiest way to implement a "one of many" choice is simply to
    put the radio buttons into TQButtonGroup.

    Whenever a button is switched on or off it emits the signal
    toggled(). Connect to this signal if you want to trigger an action
    each time the button changes state. Otherwise, use isChecked() to
    see if a particular button is selected.

    Just like TQPushButton, a radio button can display text or a
    pixmap. The text can be set in the constructor or with setText();
    the pixmap is set with setPixmap().

    <img src=qradiobt-m.png> <img src=qradiobt-w.png>

    \important text, setText, text, pixmap, setPixmap, accel, setAccel, isToggleButton, setDown, isDown, isOn, state, autoRepeat, isExclusiveToggle, group, setAutoRepeat, toggle, pressed, released, clicked, toggled, state stateChanged

    \sa TQPushButton TQToolButton
    \link guibooks.html#fowler GUI Design Handbook: Radio Button\endlink
*/

/*!
    \property TQRadioButton::checked \brief Whether the radio button is
    checked

    This property will not effect any other radio buttons unless they
    have been placed in the same TQButtonGroup. The default value is
    false (unchecked).
*/

/*!
    \property TQRadioButton::autoMask \brief whether the radio button
    is automatically masked

    \sa TQWidget::setAutoMask()
*/

/*!
    Constructs a radio button with no text.

    The \a parent and \a name arguments are sent on to the TQWidget
    constructor.
*/

TQRadioButton::TQRadioButton( TQWidget *parent, const char *name )
	: TQButton( parent, name, WNoAutoErase | WMouseNoMask )
{
    init();
}

/*!
    Constructs a radio button with the text \a text.

    The \a parent and \a name arguments are sent on to the TQWidget
    constructor.
*/

TQRadioButton::TQRadioButton( const TQString &text, TQWidget *parent,
			    const char *name )
	: TQButton( parent, name, WNoAutoErase | WMouseNoMask )
{
    init();
    setText( text );
}


/*
    Initializes the radio button.
*/

void TQRadioButton::init()
{
    setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Fixed ) );
    setToggleButton( true );
#ifndef TQT_NO_BUTTONGROUP
    TQButtonGroup *bgrp = ::tqt_cast<TQButtonGroup*>(parentWidget());
    if ( bgrp )
	bgrp->setRadioButtonExclusive( true );
#endif
}

void TQRadioButton::setChecked( bool check )
{
    setOn( check );
}




/*!
    \reimp
*/
TQSize TQRadioButton::sizeHint() const
{
    // Any more complex, and we will use style().itemRect()
    // NB: TQCheckBox::sizeHint() is similar
    constPolish();

    TQPainter p(this);
    TQSize sz = style().itemRect(&p, TQRect(0, 0, 1, 1), ShowPrefix, false,
				pixmap(), text()).size();

    return (style().sizeFromContents(TQStyle::CT_RadioButton, this, sz).
	    expandedTo(TQApplication::globalStrut()));
}


/*!
    \reimp
*/
bool TQRadioButton::hitButton( const TQPoint &pos ) const
{
    TQRect r =
	TQStyle::visualRect( style().subRect( TQStyle::SR_RadioButtonFocusRect,
					     this ), this );
    if ( tqApp->reverseLayout() ) {
	r.setRight( width() );
    } else {
	r.setLeft( 0 );
    }
    return r.contains( pos );
}


/*!
    \reimp
*/
void TQRadioButton::drawButton( TQPainter *paint )
{
    TQPainter *p = paint;
    TQRect irect = TQStyle::visualRect( style().subRect(TQStyle::SR_RadioButtonIndicator, this), this );
    const TQColorGroup &cg = colorGroup();

#if !defined( TQT_NO_TEXTSTREAM ) && !defined( TQ_WS_MACX )
#   define  SAVE_RADIOBUTTON_PIXMAPS
#endif
#if defined(SAVE_RADIOBUTTON_PIXMAPS)
    TQString pmkey;				// pixmap key
    int kf = 0;
    if ( isDown() )
	kf |= 1;
    if ( isOn() )
	kf |= 2;
    if ( isEnabled() )
	kf |= 4;
    if( isActiveWindow() )
	kf |= 8;
    if ( hasMouse() )
	kf |= 16;
    if ( hasFocus() )
	kf |= 32;

    TQTextOStream os(&pmkey);
    os << "$qt_radio_" << style().className() << "_"
       << palette().serialNumber() << "_" << irect.width() << "x" << irect.height() << "_" << kf;
    TQPixmap *pm = TQPixmapCache::find( pmkey );
    if ( pm ) {					// pixmap exists
	drawButtonLabel( p );
	p->drawPixmap( irect.topLeft(), *pm );
	return;
    }
    bool use_pm = true;
    TQPainter pmpaint;
    int wx, wy;
    if ( use_pm ) {
	pm = new TQPixmap( irect.size() );	// create new pixmap
	TQ_CHECK_PTR( pm );
	pm->fill(paletteBackgroundColor());
	TQPainter::redirect(this, pm);
	pmpaint.begin(this);
	p = &pmpaint;				// draw in pixmap
	wx = irect.x();				// save x,y coords
	wy = irect.y();
	irect.moveTopLeft(TQPoint(0, 0));
	p->setBackgroundColor(paletteBackgroundColor());
    }
#endif

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if ( isEnabled() )
	flags |= TQStyle::Style_Enabled;
    if ( hasFocus() )
	flags |= TQStyle::Style_HasFocus;
    if ( isDown() )
	flags |= TQStyle::Style_Down;
    if ( hasMouse() )
	flags |= TQStyle::Style_MouseOver;
    if ( state() == TQButton::On )
	flags |= TQStyle::Style_On;
    else if ( state() == TQButton::Off )
	flags |= TQStyle::Style_Off;

    style().drawControl(TQStyle::CE_RadioButton, p, this, irect, cg, flags);

#if defined(SAVE_RADIOBUTTON_PIXMAPS)
    if ( use_pm ) {
	pmpaint.end();
	TQPainter::redirect(this, NULL);
	if ( backgroundPixmap() || backgroundMode() == X11ParentRelative ) {
	    TQBitmap bm( pm->size() );
	    bm.fill( color0 );
	    pmpaint.begin( &bm );
	    style().drawControlMask(TQStyle::CE_RadioButton, &pmpaint, this, irect);
	    pmpaint.end();
	    pm->setMask( bm );
	}
	p = paint;				// draw in default device
	p->drawPixmap( wx, wy, *pm );
	if (!TQPixmapCache::insert(pmkey, pm) )	// save in cache
	    delete pm;
    }
#endif

    drawButtonLabel( p );
}



/*!
    \reimp
*/
void TQRadioButton::drawButtonLabel( TQPainter *p )
{
    TQRect r =
	TQStyle::visualRect( style().subRect(TQStyle::SR_RadioButtonContents,
					    this), this );

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled())
	flags |= TQStyle::Style_Enabled;
    if (hasFocus())
	flags |= TQStyle::Style_HasFocus;
    if (isDown())
	flags |= TQStyle::Style_Down;
    if (state() == TQButton::On)
	flags |= TQStyle::Style_On;
    else if (state() == TQButton::Off)
	flags |= TQStyle::Style_Off;

    style().drawControl(TQStyle::CE_RadioButtonLabel, p, this, r, colorGroup(), flags);
}


/*!
    \reimp
*/
void TQRadioButton::resizeEvent( TQResizeEvent* e )
{
    TQButton::resizeEvent(e);
    if ( isVisible() ) {
	TQPainter p(this);
	TQSize isz = style().itemRect(&p, TQRect(0, 0, 1, 1), ShowPrefix, false,
				     pixmap(), text()).size();
	TQSize wsz = (style().sizeFromContents(TQStyle::CT_RadioButton, this, isz).
		     expandedTo(TQApplication::globalStrut()));

	update(wsz.width(), isz.width(), 0, wsz.height());
    }
    if (autoMask())
	updateMask();
}

/*!
    \reimp
*/
void TQRadioButton::updateMask()
{
    TQRect irect =
	TQStyle::visualRect( style().subRect( TQStyle::SR_RadioButtonIndicator,
					     this ), this );

    TQBitmap bm(width(), height());
    bm.fill(color0);

    TQPainter p( &bm, this );
    style().drawControlMask(TQStyle::CE_RadioButton, &p, this, irect);
    if ( ! text().isNull() || ( pixmap() && ! pixmap()->isNull() ) ) {
	TQRect crect =
	    TQStyle::visualRect( style().subRect( TQStyle::SR_RadioButtonContents,
						 this ), this );
	TQRect frect =
	    TQStyle::visualRect( style().subRect( TQStyle::SR_RadioButtonFocusRect,
						 this ), this );
	TQRect label(crect.unite(frect));
	p.fillRect(label, color1);
    }
    p.end();

    setMask(bm);
}

#endif
