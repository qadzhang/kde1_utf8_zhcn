/****************************************************************************
**
** Implementation of TQPushButton class
**
** Created : 940221
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

#include "ntqpushbutton.h"
#ifndef TQT_NO_PUSHBUTTON
#include "ntqdialog.h"
#include "ntqfontmetrics.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqpixmap.h"
#include "ntqbitmap.h"
#include "ntqpopupmenu.h"
#include "ntqguardedptr.h"
#include "ntqapplication.h"
#include "ntqtoolbar.h"
#include "ntqstyle.h"
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif

/*!
    \class TQPushButton ntqpushbutton.h
    \brief The TQPushButton widget provides a command button.

    \ingroup basic
    \mainclass

    The push button, or command button, is perhaps the most commonly
    used widget in any graphical user interface. Push (click) a button
    to command the computer to perform some action, or to answer a
    question. Typical buttons are OK, Apply, Cancel, Close, Yes, No
    and Help.

    A command button is rectangular and typically displays a text
    label describing its action. An underlined character in the label
    (signified by preceding it with an ampersand in the text)
    indicates an accelerator key, e.g.
    \code
	TQPushButton *pb = new TQPushButton( "&Download", this );
    \endcode
    In this example the accelerator is \e{Alt+D}, and the label text
    will be displayed as <b><u>D</u>ownload</b>.

    Push buttons can display a textual label or a pixmap, and
    optionally a small icon. These can be set using the constructors
    and changed later using setText(), setPixmap() and setIconSet().
    If the button is disabled the appearance of the text or pixmap and
    iconset will be manipulated with respect to the GUI style to make
    the button look "disabled".

    A push button emits the signal clicked() when it is activated by
    the mouse, the Spacebar or by a keyboard accelerator. Connect to
    this signal to perform the button's action. Push buttons also
    provide less commonly used signals, for example, pressed() and
    released().

    Command buttons in dialogs are by default auto-default buttons,
    i.e. they become the default push button automatically when they
    receive the keyboard input focus. A default button is a push
    button that is activated when the user presses the Enter or Return
    key in a dialog. You can change this with setAutoDefault(). Note
    that auto-default buttons reserve a little extra space which is
    necessary to draw a default-button indicator. If you do not want
    this space around your buttons, call setAutoDefault(false).

    Being so central, the button widget has grown to accommodate a
    great many variations in the past decade. The Microsoft style
    guide now shows about ten different states of Windows push buttons
    and the text implies that there are dozens more when all the
    combinations of features are taken into consideration.

    The most important modes or states are:
    \list
    \i Available or not (grayed out, disabled).
    \i Standard push button, toggling push button or menu button.
    \i On or off (only for toggling push buttons).
    \i Default or normal. The default button in a dialog can generally
       be "clicked" using the Enter or Return key.
    \i Auto-repeat or not.
    \i Pressed down or not.
    \endlist

    As a general rule, use a push button when the application or
    dialog window performs an action when the user clicks on it (such
    as Apply, Cancel, Close and Help) \e and when the widget is
    supposed to have a wide, rectangular shape with a text label.
    Small, typically square buttons that change the state of the
    window rather than performing an action (such as the buttons in
    the top-right corner of the TQFileDialog) are not command buttons,
    but tool buttons. TQt provides a special class (TQToolButton) for
    these buttons.

    If you need toggle behavior (see setToggleButton()) or a button
    that auto-repeats the activation signal when being pushed down
    like the arrows in a scroll bar (see setAutoRepeat()), a command
    button is probably not what you want. When in doubt, use a tool
    button.

    A variation of a command button is a menu button. These provide
    not just one command, but several, since when they are clicked
    they pop up a menu of options. Use the method setPopup() to
    associate a popup menu with a push button.

    Other classes of buttons are option buttons (see TQRadioButton) and
    check boxes (see TQCheckBox).

    <img src="qpushbt-m.png"> <img src="qpushbt-w.png">

    In TQt, the TQButton abstract base class provides most of the modes
    and other API, and TQPushButton provides GUI logic. See TQButton for
    more information about the API.

    \important text, setText, text, pixmap, setPixmap, accel, setAccel,
    isToggleButton, setDown, isDown, isOn, state, autoRepeat,
    isExclusiveToggle, group, setAutoRepeat, toggle, pressed, released,
    clicked, toggled, state stateChanged

    \sa TQToolButton, TQRadioButton TQCheckBox
    \link guibooks.html#fowler GUI Design Handbook: Push Button\endlink
*/

/*!
    \property TQPushButton::autoDefault
    \brief whether the push button is the auto default button

    If this property is set to true then the push button is the auto
    default button in a dialog.

    In some GUI styles a default button is drawn with an extra frame
    around it, up to 3 pixels or more. TQt automatically keeps this
    space free around auto-default buttons, i.e. auto-default buttons
    may have a slightly larger size hint.

    This property's default is true for buttons that have a TQDialog
    parent; otherwise it defaults to false.

    See the \l default property for details of how \l default and
    auto-default interact.
*/

/*!
    \property TQPushButton::autoMask
    \brief whether the button is automatically masked

    \sa TQWidget::setAutoMask()
*/

/*!
    \property TQPushButton::default
    \brief whether the push button is the default button

    If this property is set to true then the push button will be
    pressed if the user presses the Enter (or Return) key in a dialog.

    Regardless of focus, if the user presses Enter: If there is a
    default button the default button is pressed; otherwise, if
    there are one or more \l autoDefault buttons the first \l autoDefault
    button that is next in the tab order is pressed. If there are no
    default or \l autoDefault buttons only pressing Space on a button
    with focus, mouse clicking, or using an accelerator will press a
    button.

    In a dialog, only one push button at a time can be the default
    button. This button is then displayed with an additional frame
    (depending on the GUI style).

    The default button behavior is provided only in dialogs. Buttons
    can always be clicked from the keyboard by pressing Enter (or
    Return) or the Spacebar when the button has focus.

    This property's default is false.
*/

/*!
    \property TQPushButton::flat
    \brief whether the border is disabled

    This property's default is false.
*/

/*!
    \property TQPushButton::iconSet
    \brief the icon set on the push button

    This property will return 0 if the push button has no iconset.
*/

/*!
    \property TQPushButton::on
    \brief whether the push button is toggled

    This property should only be set for toggle push buttons. The
    default value is false.

    \sa isOn(), toggle(), toggled(), isToggleButton()
*/

/*!
    \property TQPushButton::toggleButton
    \brief whether the button is a toggle button

    Toggle buttons have an on/off state similar to \link TQCheckBox
    check boxes. \endlink A push button is initially not a toggle
    button.

    \sa setOn(), toggle(), isToggleButton() toggled()
*/

/*! \property TQPushButton::menuButton
    \brief whether the push button has a menu button on it
    \obsolete

  If this property is set to true, then a down arrow is drawn on the push
  button to indicate that a menu will pop up if the user clicks on the
  arrow.
*/

class TQPushButtonPrivate
{
public:
    TQPushButtonPrivate()
	:iconset( 0 )
    {}
    ~TQPushButtonPrivate()
    {
#ifndef TQT_NO_ICONSET
	delete iconset;
#endif
    }
#ifndef TQT_NO_POPUPMENU
    TQGuardedPtr<TQPopupMenu> popup;
#endif
    TQIconSet* iconset;
};


/*!
    Constructs a push button with no text.

    The \a parent and \a name arguments are sent on to the TQWidget
    constructor.
*/

TQPushButton::TQPushButton( TQWidget *parent, const char *name )
	: TQButton( parent, name )
{
    init();
}

/*!
    Constructs a push button called \a name with the parent \a parent
    and the text \a text.
*/

TQPushButton::TQPushButton( const TQString &text, TQWidget *parent,
			  const char *name )
	: TQButton( parent, name )
{
    init();
    setText( text );
}


/*!
    Constructs a push button with an \a icon and a \a text.

    Note that you can also pass a TQPixmap object as an icon (thanks to
    the implicit type conversion provided by C++).

    The \a parent and \a name arguments are sent to the TQWidget
    constructor.
*/
#ifndef TQT_NO_ICONSET
TQPushButton::TQPushButton( const TQIconSet& icon, const TQString &text,
			  TQWidget *parent, const char *name )
	: TQButton( parent, name )
{
    init();
    setText( text );
    setIconSet( icon );
}
#endif


/*!
    Destroys the push button.
*/
TQPushButton::~TQPushButton()
{
    delete d;
}

void TQPushButton::init()
{
    d = 0;
    defButton = false;
    lastEnabled = false;
    hasMenuArrow = false;
    flt = false;
#ifndef TQT_NO_DIALOG
    autoDefButton = ::tqt_cast<TQDialog*>(topLevelWidget()) != 0;
#else
    autoDefButton = false;
#endif
    setBackgroundMode( PaletteButton );
    setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Fixed ) );
}


/*
  Makes the push button a toggle button if \a enable is true or a normal
  push button if \a enable is false.

  Toggle buttons have an on/off state similar to \link TQCheckBox check
  boxes. \endlink A push button is initially not a toggle button.

  \sa setOn(), toggle(), isToggleButton() toggled()
*/

void TQPushButton::setToggleButton( bool enable )
{
    TQButton::setToggleButton( enable );
}


/*
  Switches a toggle button on if \a enable is true or off if \a enable is
  false.
  \sa isOn(), toggle(), toggled(), isToggleButton()
*/

void TQPushButton::setOn( bool enable )
{
    if ( !isToggleButton() )
	return;
    TQButton::setOn( enable );
}

void TQPushButton::setAutoDefault( bool enable )
{
    if ( (bool)autoDefButton == enable )
	return;
    autoDefButton = enable;
    update();
    updateGeometry();
}


void TQPushButton::setDefault( bool enable )
{
    if ( (bool)defButton == enable )
	return;					// no change
    defButton = enable;
#ifndef TQT_NO_DIALOG
    if ( defButton && ::tqt_cast<TQDialog*>(topLevelWidget()) )
 	((TQDialog*)topLevelWidget())->setMainDefault( this );
#endif
    update();
#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::StateChanged );
#endif
}


/*!
    \reimp
*/
TQSize TQPushButton::sizeHint() const
{
    constPolish();

    int w = 0, h = 0;

    // calculate contents size...
#ifndef TQT_NO_ICONSET
    if ( iconSet() && !iconSet()->isNull() ) {
	int iw = iconSet()->pixmap( TQIconSet::Small, TQIconSet::Normal ).width() + 4;
	int ih = iconSet()->pixmap( TQIconSet::Small, TQIconSet::Normal ).height();
	w += iw;
	h = TQMAX( h, ih );
    }
#endif
    if ( isMenuButton() )
	w += style().pixelMetric(TQStyle::PM_MenuButtonIndicator, this);

    if ( pixmap() ) {
	TQPixmap *pm = (TQPixmap *)pixmap();
	w += pm->width();
	h += pm->height();
    } else {
	TQString s( text() );
	bool empty = s.isEmpty();
	if ( empty )
	    s = TQString::fromLatin1("XXXX");
	TQFontMetrics fm = fontMetrics();
	TQSize sz = fm.size( ShowPrefix, s );
	if(!empty || !w)
	    w += sz.width();
	if(!empty || !h)
	    h = TQMAX(h, sz.height());
    }

    return (style().sizeFromContents(TQStyle::CT_PushButton, this, TQSize(w, h)).
	    expandedTo(TQApplication::globalStrut()));
}


/*!
    \reimp
*/
void TQPushButton::move( int x, int y )
{
    TQWidget::move( x, y );
}

/*!
    \reimp
*/
void TQPushButton::move( const TQPoint &p )
{
    move( p.x(), p.y() );
}

/*!
    \reimp
*/
void TQPushButton::resize( int w, int h )
{
    TQWidget::resize( w, h );
}

/*!
    \reimp
*/
void TQPushButton::resize( const TQSize &s )
{
    resize( s.width(), s.height() );
}

/*!
    \reimp
*/
void TQPushButton::setGeometry( int x, int y, int w, int h )
{
    TQWidget::setGeometry( x, y, w, h );
}

/*!
    \reimp
*/
void TQPushButton::setGeometry( const TQRect &r )
{
    TQWidget::setGeometry( r );
}

/*!
    \reimp
 */
void TQPushButton::resizeEvent( TQResizeEvent * )
{
    if ( autoMask() )
	updateMask();
}

/*!
    \reimp
*/
void TQPushButton::drawButton( TQPainter *paint )
{
    int diw = 0;
    if ( isDefault() || autoDefault() ) {
	diw = style().pixelMetric(TQStyle::PM_ButtonDefaultIndicator, this);

	if ( diw > 0 ) {
	    if (backgroundMode() == X11ParentRelative) {
		erase( 0, 0, width(), diw );
		erase( 0, 0, diw, height() );
		erase( 0, height() - diw, width(), diw );
		erase( width() - diw, 0, diw, height() );
	    } else if ( parentWidget() && parentWidget()->backgroundPixmap() ){
		// pseudo tranparency
		paint->drawTiledPixmap( 0, 0, width(), diw,
					*parentWidget()->backgroundPixmap(),
					x(), y() );
		paint->drawTiledPixmap( 0, 0, diw, height(),
					*parentWidget()->backgroundPixmap(),
					x(), y() );
		paint->drawTiledPixmap( 0, height()-diw, width(), diw,
					*parentWidget()->backgroundPixmap(),
					x(), y()+height() );
		paint->drawTiledPixmap( width()-diw, 0, diw, height(),
					*parentWidget()->backgroundPixmap(),
					x()+width(), y() );
	    } else {
		paint->fillRect( 0, 0, width(), diw,
				 colorGroup().brush(TQColorGroup::Background) );
		paint->fillRect( 0, 0, diw, height(),
				 colorGroup().brush(TQColorGroup::Background) );
		paint->fillRect( 0, height()-diw, width(), diw,
				 colorGroup().brush(TQColorGroup::Background) );
		paint->fillRect( width()-diw, 0, diw, height(),
				 colorGroup().brush(TQColorGroup::Background) );
	    }

	}
    }

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled())
	flags |= TQStyle::Style_Enabled;
    if (hasFocus())
	flags |= TQStyle::Style_HasFocus;
    if (isDown())
	flags |= TQStyle::Style_Down;
    if (isOn())
	flags |= TQStyle::Style_On;
    if (! isFlat() && ! isDown())
	flags |= TQStyle::Style_Raised;
    if (isDefault())
	flags |= TQStyle::Style_ButtonDefault;
    if (hasMouse())
	flags |= TQStyle::Style_MouseOver;

    style().drawControl(TQStyle::CE_PushButton, paint, this, rect(), colorGroup(), flags);
    drawButtonLabel( paint );

    lastEnabled = isEnabled();
}


/*!
    \reimp
*/
void TQPushButton::drawButtonLabel( TQPainter *paint )
{

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled())
	flags |= TQStyle::Style_Enabled;
    if (hasFocus())
	flags |= TQStyle::Style_HasFocus;
    if (isDown())
	flags |= TQStyle::Style_Down;
    if (isOn())
	flags |= TQStyle::Style_On;
    if (! isFlat() && ! isDown())
	flags |= TQStyle::Style_Raised;
    if (isDefault())
	flags |= TQStyle::Style_ButtonDefault;

    style().drawControl(TQStyle::CE_PushButtonLabel, paint, this,
			style().subRect(TQStyle::SR_PushButtonContents, this),
			colorGroup(), flags);
}


/*!
    \reimp
 */
void TQPushButton::updateMask()
{
    TQBitmap bm( size() );
    bm.fill( color0 );

    {
	TQPainter p( &bm, this );
	style().drawControlMask(TQStyle::CE_PushButton, &p, this, rect());
    }

    setMask( bm );
}

/*!
    \reimp
*/
void TQPushButton::focusInEvent( TQFocusEvent *e )
{
    if (autoDefButton && !defButton) {
	defButton = true;
#ifndef TQT_NO_DIALOG
	if ( defButton && ::tqt_cast<TQDialog*>(topLevelWidget()) )
 	    ((TQDialog*)topLevelWidget())->setDefault( this );
#endif
    }
    TQButton::focusInEvent( e );
}

/*!
    \reimp
*/
void TQPushButton::focusOutEvent( TQFocusEvent *e )
{
#ifndef TQT_NO_DIALOG
    if ( defButton && autoDefButton ) {
	if ( ::tqt_cast<TQDialog*>(topLevelWidget()) )
	    ((TQDialog*)topLevelWidget())->setDefault( 0 );
    }
#endif

    TQButton::focusOutEvent( e );
#ifndef TQT_NO_POPUPMENU
    if ( popup() && popup()->isVisible() )	// restore pressed status
	setDown( true );
#endif
}


#ifndef TQT_NO_POPUPMENU
/*!
    Associates the popup menu \a popup with this push button. This
    turns the button into a menu button.

    Ownership of the popup menu is \e not transferred to the push
    button.

    \sa popup()
*/
void TQPushButton::setPopup( TQPopupMenu* popup )
{
    if ( !d )
	d = new TQPushButtonPrivate;
    if ( popup && !d->popup )
	connect( this, TQ_SIGNAL( pressed() ), this, TQ_SLOT( popupPressed() ) );

    d->popup = popup;
    setIsMenuButton( popup != 0 );
}
#endif //TQT_NO_POPUPMENU
#ifndef TQT_NO_ICONSET
void TQPushButton::setIconSet( const TQIconSet& icon )
{
    if ( !d )
	d = new TQPushButtonPrivate;
    if ( !icon.isNull() ) {
	if ( d->iconset )
	    *d->iconset = icon;
	else
	    d->iconset = new TQIconSet( icon );
    } else if ( d->iconset) {
	delete d->iconset;
	d->iconset = 0;
    }

    update();
    updateGeometry();
}


TQIconSet* TQPushButton::iconSet() const
{
    return d ? d->iconset : 0;
}
#endif // TQT_NO_ICONSET
#ifndef TQT_NO_POPUPMENU
/*!
    Returns the button's associated popup menu or 0 if no popup menu
    has been set.

    \sa setPopup()
*/
TQPopupMenu* TQPushButton::popup() const
{
    return d ? (TQPopupMenu*)d->popup : 0;
}

void TQPushButton::popupPressed()
{
    TQPopupMenu* popup = d ? (TQPopupMenu*) d->popup : 0;
    TQGuardedPtr<TQPushButton> that = this;
    if ( isDown() && popup ) {
	bool horizontal = true;
	bool topLeft = true;			// ### always true
#ifndef TQT_NO_TOOLBAR
	TQToolBar *tb = ::tqt_cast<TQToolBar*>(parentWidget());
	if ( tb && tb->orientation() == Vertical )
	    horizontal = false;
#endif
	if ( horizontal ) {
	    if ( topLeft ) {
		if ( mapToGlobal( TQPoint( 0, rect().bottom() ) ).y() + popup->sizeHint().height() <= tqApp->desktop()->height() )
		    popup->exec( mapToGlobal( rect().bottomLeft() ) );
		else
		    popup->exec( mapToGlobal( rect().topLeft() - TQPoint( 0, popup->sizeHint().height() ) ) );
	    } else {
		TQSize sz( popup->sizeHint() );
		TQPoint p = mapToGlobal( rect().topLeft() );
		p.ry() -= sz.height();
		popup->exec( p );
	    }
	} else {
	    if ( topLeft ) {
		if ( mapToGlobal( TQPoint( rect().right(), 0 ) ).x() + popup->sizeHint().width() <= tqApp->desktop()->width() )
		    popup->exec( mapToGlobal( rect().topRight() ) );
		else
		    popup->exec( mapToGlobal( rect().topLeft() - TQPoint( popup->sizeHint().width(), 0 ) ) );
	    } else {
		TQSize sz( popup->sizeHint() );
		TQPoint p = mapToGlobal( rect().topLeft() );
		p.rx() -= sz.width();
		popup->exec( p );
	    }
	}
        if (that)
            setDown( false );
    }
}
#endif

void TQPushButton::setFlat( bool f )
{
    flt = f;
    update();
}

bool TQPushButton::isFlat() const
{
    return flt;
}

/*!
    \obsolete
    \fn virtual void TQPushButton::setIsMenuButton( bool enable )
*/

#endif
