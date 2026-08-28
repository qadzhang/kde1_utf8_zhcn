/****************************************************************************
**
** Implementation of TQCheckBox class
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

#include "ntqcheckbox.h"
#ifndef TQT_NO_CHECKBOX
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqpixmap.h"
#include "ntqpixmapcache.h"
#include "ntqbitmap.h"
#include "ntqtextstream.h"
#include "ntqapplication.h"
#include "ntqstyle.h"

/*!
    \class TQCheckBox ntqcheckbox.h
    \brief The TQCheckBox widget provides a checkbox with a text label.

    \ingroup basic
    \mainclass

    TQCheckBox and TQRadioButton are both option buttons. That is, they
    can be switched on (checked) or off (unchecked). The classes
    differ in how the choices for the user are restricted. Radio
    buttons define a "one of many" choice, whereas checkboxes provide
    "many of many" choices.

    A TQButtonGroup can be used to group check buttons visually.

    Whenever a checkbox is checked or cleared it emits the signal
    toggled(). Connect to this signal if you want to trigger an action
    each time the checkbox changes state. You can use isChecked() to
    query whether or not a checkbox is checked.

    \warning The toggled() signal can not be trusted for tristate
    checkboxes.

    In addition to the usual checked and unchecked states, TQCheckBox
    optionally provides a third state to indicate "no change". This
    is useful whenever you need to give the user the option of neither
    checking nor unchecking a checkbox. If you need this third state,
    enable it with setTristate() and use state() to query the current
    toggle state. When a tristate checkbox changes state, it emits the
    stateChanged() signal.

    Just like TQPushButton, a checkbox can display text or a pixmap.
    The text can be set in the constructor or with setText(); the
    pixmap is set with setPixmap().

    \important text(), setText(), text(), pixmap(), setPixmap(),
    accel(), setAccel(), isToggleButton(), setDown(), isDown(),
    isOn(), state(), autoRepeat(), isExclusiveToggle(), group(),
    setAutoRepeat(), toggle(), pressed(), released(), clicked(),
    toggled(), state() stateChanged()

    <img src=qchkbox-m.png> <img src=qchkbox-w.png>

    \sa TQButton TQRadioButton
    \link guibooks.html#fowler Fowler: Check Box \endlink
*/

/*!
    \property TQCheckBox::checked
    \brief whether the checkbox is checked

    The default is unchecked, i.e. false.
*/

/*!
    \property TQCheckBox::autoMask
    \brief whether the checkbox is automatically masked

    \sa TQWidget::setAutoMask()
*/

/*!
    \property TQCheckBox::tristate
    \brief whether the checkbox is a tri-state checkbox

    The default is two-state, i.e. tri-state is false.
*/

/*!
    Constructs a checkbox with no text.

    The \a parent and \a name arguments are sent to the TQWidget
    constructor.
*/

TQCheckBox::TQCheckBox( TQWidget *parent, const char *name )
	: TQButton( parent, name, WNoAutoErase | WMouseNoMask )
{
    setToggleButton( true );
    setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Fixed ) );
}

/*!
    Constructs a checkbox with text \a text.

    The \a parent and \a name arguments are sent to the TQWidget
    constructor.
*/

TQCheckBox::TQCheckBox( const TQString &text, TQWidget *parent, const char *name )
	: TQButton( parent, name, WNoAutoErase | WMouseNoMask )
{
    setText( text );
    setToggleButton( true );
    setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Fixed ) );
}

/*!
    Sets the checkbox to the "no change" state.

    \sa setTristate()
*/
void TQCheckBox::setNoChange()
{
    setTristate(true);
    setState( NoChange );
}

void TQCheckBox::setTristate(bool y)
{
    setToggleType( y ? Tristate : Toggle );
}

bool TQCheckBox::isTristate() const
{
    return toggleType() == Tristate;
}


/*!\reimp
*/
TQSize TQCheckBox::sizeHint() const
{
    // NB: TQRadioButton::sizeHint() is similar
    constPolish();

    TQPainter p(this);
    TQSize sz = style().itemRect(&p, TQRect(0, 0, 1, 1), ShowPrefix, false,
				pixmap(), text()).size();

    return (style().sizeFromContents(TQStyle::CT_CheckBox, this, sz).
	    expandedTo(TQApplication::globalStrut()));
}


/*!\reimp
*/

void TQCheckBox::drawButton( TQPainter *paint )
{
    TQPainter *p = paint;
    TQRect irect = TQStyle::visualRect( style().subRect(TQStyle::SR_CheckBoxIndicator, this), this );
    const TQColorGroup &cg = colorGroup();

#if !defined( TQT_NO_TEXTSTREAM ) && !defined( TQ_WS_MACX )
#   define  SAVE_CHECKBOX_PIXMAPS
#endif
#if defined(SAVE_CHECKBOX_PIXMAPS)
    TQString pmkey;				// pixmap key
    int kf = 0;
    if ( isDown() )
	kf |= 1;
    if ( isEnabled() )
	kf |= 2;
    if ( hasFocus() )
	kf |= 4;				// active vs. normal colorgroup
    if( isActiveWindow() )
	kf |= 8;
    if ( hasMouse() )
	kf |= 16;

    kf |= state() << 5;
    TQTextOStream os(&pmkey);
    os << "$qt_check_" << style().className() << "_"
       << palette().serialNumber() << "_" << irect.width() << "x" << irect.height() << "_" << kf;
    TQPixmap *pm = TQPixmapCache::find( pmkey );
    if ( pm ) {					// pixmap exists
	p->drawPixmap( irect.topLeft(), *pm );
	drawButtonLabel( p );
	return;
    }
    bool use_pm = true;
    TQPainter pmpaint;
    int wx = 0, wy = 0;
    if ( use_pm ) {
	pm = new TQPixmap( irect.size() );	// create new pixmap
	TQ_CHECK_PTR( pm );
	pm->fill( cg.background() );
	TQPainter::redirect(this, pm);
	pmpaint.begin(this);
	p = &pmpaint;				// draw in pixmap
	wx = irect.x();				// save x,y coords
	wy = irect.y();
	irect.moveTopLeft(TQPoint(0, 0));
	p->setBackgroundColor( cg.background() );
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
    else if ( state() == TQButton::NoChange )
	flags |= TQStyle::Style_NoChange;

    style().drawControl(TQStyle::CE_CheckBox, p, this, irect, cg, flags);

#if defined(SAVE_CHECKBOX_PIXMAPS)
    if ( use_pm ) {
	pmpaint.end();
	TQPainter::redirect( this, 0 );
	if ( backgroundPixmap() || backgroundMode() == X11ParentRelative ) {
	    TQBitmap bm( pm->size() );
	    bm.fill( color0 );
	    pmpaint.begin( &bm );
	    style().drawControlMask(TQStyle::CE_CheckBox, &pmpaint, this, irect);
	    pmpaint.end();
	    pm->setMask( bm );
	}
	p = paint;				// draw in default device
	p->drawPixmap( wx, wy, *pm );
	if (!TQPixmapCache::insert(pmkey, pm) )	// save in cache
	    delete pm;
    }
#endif

    drawButtonLabel( paint );
}


/*!\reimp
*/
void TQCheckBox::drawButtonLabel( TQPainter *p )
{
    TQRect r =
	TQStyle::visualRect( style().subRect(TQStyle::SR_CheckBoxContents, this), this );

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
    else if (state() == TQButton::NoChange)
	flags |= TQStyle::Style_NoChange;

    style().drawControl(TQStyle::CE_CheckBoxLabel, p, this, r, colorGroup(), flags);
}

/*!
  \reimp
*/
void TQCheckBox::resizeEvent( TQResizeEvent *e )
{
    TQButton::resizeEvent(e);
    if ( isVisible() ) {
	TQPainter p(this);
	TQSize isz = style().itemRect(&p, TQRect(0, 0, 1, 1), ShowPrefix, false,
				     pixmap(), text()).size();
	TQSize wsz = (style().sizeFromContents(TQStyle::CT_CheckBox, this, isz).
		     expandedTo(TQApplication::globalStrut()));

	update(wsz.width(), isz.width(), 0, wsz.height());
    }
    if (autoMask())
	updateMask();
}

/*!
  \reimp
*/
void TQCheckBox::updateMask()
{
    TQRect irect =
	TQStyle::visualRect( style().subRect(TQStyle::SR_CheckBoxIndicator, this), this );

    TQBitmap bm(width(), height());
    bm.fill(color0);

    TQPainter p( &bm, this );
    style().drawControlMask(TQStyle::CE_CheckBox, &p, this, irect);
    if ( ! text().isNull() || ( pixmap() && ! pixmap()->isNull() ) ) {
	TQRect crect =
	    TQStyle::visualRect( style().subRect( TQStyle::SR_CheckBoxContents,
						 this ), this );
	TQRect frect =
	    TQStyle::visualRect( style().subRect( TQStyle::SR_CheckBoxFocusRect,
						 this ), this );
	TQRect label(crect.unite(frect));
	p.fillRect(label, color1);
    }
    p.end();

    setMask(bm);
}

/*!\reimp*/
bool TQCheckBox::hitButton( const TQPoint &pos ) const
{
    TQRect r = TQStyle::visualRect( style().subRect( TQStyle::SR_CheckBoxFocusRect, this ), this );
    if ( tqApp->reverseLayout() ) {
	r.setRight( width() );
    } else {
	r.setLeft( 0 );
    }
    return r.contains( pos );
}

#endif
