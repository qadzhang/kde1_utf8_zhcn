/****************************************************************************
**
** Implementation of TQButton widget class
**
** Created : 940206
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

#undef TQT_NO_COMPAT
#include "ntqbutton.h"
#ifndef TQT_NO_BUTTON
#include "ntqbuttongroup.h"
#include "ntqbitmap.h"
#include "ntqpainter.h"
#include "ntqtimer.h"
#include "ntqaccel.h"
#include "ntqpixmapcache.h"
#include "ntqapplication.h"
#include "ntqpushbutton.h"
#include "ntqradiobutton.h"
#include "ntqguardedptr.h"
#include "../kernel/qinternal_p.h"

#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif

#define AUTO_REPEAT_DELAY  300
#define AUTO_REPEAT_PERIOD 100

class TQButtonData
{
public:
    TQButtonData() {
#ifndef TQT_NO_BUTTONGROUP
	group = 0;
#endif
#ifndef TQT_NO_ACCEL
	a = 0;
#endif
    }
#ifndef TQT_NO_BUTTONGROUP
    TQButtonGroup *group;
#endif
    TQTimer timer;
#ifndef TQT_NO_ACCEL
    TQAccel *a;
#endif
};


void TQButton::ensureData()
{
    if ( !d ) {
	d = new TQButtonData;
	TQ_CHECK_PTR( d );
	connect(&d->timer, TQ_SIGNAL(timeout()), this, TQ_SLOT(autoRepeatTimeout()));
    }
}


/*!
    Returns the group that this button belongs to.

    If the button is not a member of any TQButtonGroup, this function
    returns 0.

    \sa TQButtonGroup
*/

TQButtonGroup *TQButton::group() const
{
#ifndef TQT_NO_BUTTONGROUP
    return d ? d->group : 0;
#else
    return 0;
#endif
}


void TQButton::setGroup( TQButtonGroup* g )
{
#ifndef TQT_NO_BUTTONGROUP
    ensureData();
    d->group = g;
#endif
}


TQTimer *TQButton::timer()
{
    ensureData();
    return &d->timer;
}


/*!
    \class TQButton ntqbutton.h
    \brief The TQButton class is the abstract base class of button
    widgets, providing functionality common to buttons.

    \ingroup abstractwidgets

    <b>If you want to create a button use TQPushButton.</b>

    The TQButton class implements an \e abstract button, and lets
    subclasses specify how to reply to user actions and how to draw
    the button.

    TQButton provides both push and toggle buttons. The TQRadioButton
    and TQCheckBox classes provide only toggle buttons; TQPushButton and
    TQToolButton provide both toggle and push buttons.

    Any button can have either a text or pixmap label. setText() sets
    the button to be a text button and setPixmap() sets it to be a
    pixmap button. The text/pixmap is manipulated as necessary to
    create the "disabled" appearance when the button is disabled.

    TQButton provides most of the states used for buttons:
    \list
    \i isDown() indicates whether the button is \e pressed down.
    \i isOn() indicates whether the button is \e on.
       Only toggle buttons can be switched on and off  (see below).
    \i isEnabled() indicates whether the button can be pressed by the
       user.
    \i setAutoRepeat() sets whether the button will auto-repeat
       if the user holds it down.
    \i setToggleButton() sets whether the button is a toggle
       button or not.
    \endlist

    The difference between isDown() and isOn() is as follows: When the
    user clicks a toggle button to toggle it on, the button is first
    \e pressed and then released into the \e on state. When the user
    clicks it again (to toggle it off), the button moves first to the
    \e pressed state, then to the \e off state (isOn() and isDown()
    are both false).

    Default buttons (as used in many dialogs) are provided by
    TQPushButton::setDefault() and TQPushButton::setAutoDefault().

    TQButton provides five signals:
    \list 1
    \i pressed() is emitted when the button is pressed. E.g. with the mouse
       or when animateClick() is called.
    \i released() is emitted when the button is released. E.g. when the mouse
       is released or the cursor is moved outside the widget.
    \i clicked() is emitted when the button is first pressed and then
       released when the accelerator key is typed, or when
       animateClick() is called.
    \i toggled(bool) is emitted when the state of a toggle button changes.
    \i stateChanged(int) is emitted when the state of a tristate
       toggle button changes.
    \endlist

    If the button is a text button with an ampersand (\&) in its text,
    TQButton creates an automatic accelerator key. This code creates a
    push button labelled "Ro<u>c</u>k \& Roll" (where the c is
    underlined). The button gets an automatic accelerator key, Alt+C:

    \code
	TQPushButton *p = new TQPushButton( "Ro&ck && Roll", this );
    \endcode

    In this example, when the user presses Alt+C the button will call
    animateClick().

    You can also set a custom accelerator using the setAccel()
    function. This is useful mostly for pixmap buttons because they
    have no automatic accelerator.

    \code
	p->setPixmap( TQPixmap("print.png") );
	p->setAccel( ALT+Key_F7 );
    \endcode

    All of the buttons provided by TQt (\l TQPushButton, \l TQToolButton,
    \l TQCheckBox and \l TQRadioButton) can display both text and
    pixmaps.

    To subclass TQButton, you must reimplement at least drawButton()
    (to draw the button's outline) and drawButtonLabel() (to draw its
    text or pixmap). It is generally advisable to reimplement
    sizeHint() as well, and sometimes hitButton() (to determine
    whether a button press is within the button).

    To reduce flickering, TQButton::paintEvent() sets up a pixmap that
    the drawButton() function draws in. You should not reimplement
    paintEvent() for a subclass of TQButton unless you want to take
    over all drawing.

    \sa TQButtonGroup
*/


/*!
    \enum TQButton::ToggleType

    This enum type defines what a button can do in response to a
    mouse/keyboard press:

    \value SingleShot  pressing the button causes an action, then the
    button returns to the unpressed state.

    \value Toggle  pressing the button toggles it between an \c On and
    an \c Off state.

    \value Tristate  pressing the button cycles between the three
    states \c On, \c Off and \c NoChange
*/

/*!
    \enum TQButton::ToggleState

    This enum defines the state of a toggle button.

    \value Off  the button is in the "off" state
    \value NoChange  the button is in the default/unchanged state
    \value On  the button is in the "on" state
*/

/*!
    \property TQButton::accel
    \brief the accelerator associated with the button

    This property is 0 if there is no accelerator set. If you set this
    property to 0 then any current accelerator is removed.
*/

/*!
    \property TQButton::autoRepeat
    \brief whether autoRepeat is enabled

    If autoRepeat is enabled then the clicked() signal is emitted at
    regular intervals if the button is down. This property has no
    effect on toggle buttons. autoRepeat is off by default.
*/

/*! \property TQButton::autoResize
    \brief whether autoResize is enabled
    \obsolete

  If autoResize is enabled then the button will resize itself
  whenever the contents are changed.
*/

/*!
    \property TQButton::down
    \brief whether the button is pressed

    If this property is true, the button is pressed down. The signals
    pressed() and clicked() are not emitted if you set this property
    to true. The default is false.
*/

/*!
    \property TQButton::exclusiveToggle
    \brief whether the button is an exclusive toggle

    If this property is true and the button is in a TQButtonGroup, the
    button can only be toggled off by another one being toggled on.
    The default is false.
*/

/*!
    \property TQButton::on
    \brief whether the button is toggled

    This property should only be set for toggle buttons.
*/

/*!
    \fn void TQButton::setOn( bool on )

    Sets the state of this button to On if \a on is true; otherwise to
    Off.

    \sa toggleState
*/

/*!
    \property TQButton::pixmap
    \brief the pixmap shown on the button

    If the pixmap is monochrome (i.e. it is a TQBitmap or its \link
    TQPixmap::depth() depth\endlink is 1) and it does not have a mask,
    this property will set the pixmap to be its own mask. The purpose
    of this is to draw transparent bitmaps which are important for
    toggle buttons, for example.

    pixmap() returns 0 if no pixmap was set.
*/

/*!
    \property TQButton::text
    \brief the text shown on the button

    This property will return a TQString::null if the button has no
    text. If the text has an ampersand (\&) in it, then an
    accelerator is automatically created for it using the character
    that follows the '\&' as the accelerator key. Any previous
    accelerator will be overwritten, or cleared if no accelerator is
    defined by the text.

    There is no default text.
*/

/*!
    \property TQButton::toggleButton
    \brief whether the button is a toggle button

    The default value is false.
*/

/*!
    \fn TQButton::setToggleButton( bool b )

    If \a b is true, this button becomes a toggle button; if \a b is
    false, this button becomes a command button.

    \sa toggleButton
*/

/*!
    \property TQButton::toggleState
    \brief the state of the toggle button

    If this property is changed then it does not cause the button
    to be repainted.
*/

/*!
    \property TQButton::toggleType
    \brief the type of toggle on the button

    The default toggle type is \c SingleShot.

    \sa TQButton::ToggleType
*/

/*!
    Constructs a standard button called \a name with parent \a parent,
    using the widget flags \a f.

    If \a parent is a TQButtonGroup, this constructor calls
    TQButtonGroup::insert().
*/

TQButton::TQButton( TQWidget *parent, const char *name, WFlags f )
    : TQWidget( parent, name, f )
{
    bpixmap    = 0;
    toggleTyp  = SingleShot;			// button is simple
    buttonDown = false;				// button is up
    stat       = Off;				// button is off
    mlbDown    = false;				// mouse left button up
    autoresize = false;				// not auto resizing
    animation  = false;				// no pending animateClick
    repeat     = false;				// not in autorepeat mode
    d	       = 0;
#ifndef TQT_NO_BUTTONGROUP
    if ( ::tqt_cast<TQButtonGroup*>(parent) ) {
	setGroup((TQButtonGroup*)parent);
	group()->insert( this );		// insert into button group
    }
#endif
    setFocusPolicy( TabFocus );
}

/*!
    Destroys the button.
 */
TQButton::~TQButton()
{
#ifndef TQT_NO_BUTTONGROUP
    if ( group() )
	group()->remove( this );
#endif
    delete bpixmap;
    delete d;
}


/*!
    \fn void TQButton::pressed()

    This signal is emitted when the button is pressed down.

    \sa released(), clicked()
*/

/*!
    \fn void TQButton::released()

    This signal is emitted when the button is released.

    \sa pressed(), clicked(), toggled()
*/

/*!
    \fn void TQButton::clicked()

    This signal is emitted when the button is activated (i.e. first
    pressed down and then released when the mouse cursor is inside the
    button), when the accelerator key is typed or when animateClick()
    is called. This signal is \e not emitted if you call setDown().

    The TQButtonGroup::clicked() signal does the same job, if you want
    to connect several buttons to the same slot.

    \warning Don't launch a model dialog in response to this signal
    for a button that has \c autoRepeat turned on.

    \sa pressed(), released(), toggled() autoRepeat down
*/

/*!
    \fn void TQButton::toggled( bool on )

    This signal is emitted whenever a toggle button changes status. \a
    on is true if the button is on, or false if the button is off.

    This may be the result of a user action, toggle() slot activation,
    or because setOn() was called.

    \sa clicked()
*/

/*!
    \fn void TQButton::stateChanged( int state )

    This signal is emitted whenever a toggle button changes state. \a
    state is \c On if the button is on, \c NoChange if it is in the
    \link TQCheckBox::setTristate() "no change" state\endlink or \c Off
    if the button is off.

    This may be the result of a user action, toggle() slot activation,
    setState(), or because setOn() was called.

    \sa clicked() TQButton::ToggleState
*/

void TQButton::setText( const TQString &text )
{
    if ( btext == text )
	return;
    btext = text;
#ifndef TQT_NO_ACCEL
    setAccel( TQAccel::shortcutKey( text ) );
#endif

    if ( bpixmap ) {
	delete bpixmap;
	bpixmap = 0;
    }

    if ( autoresize )
	adjustSize();

    update();
    updateGeometry();

#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::NameChanged );
#endif
}

void TQButton::setPixmap( const TQPixmap &pixmap )
{
    if ( bpixmap && bpixmap->serialNumber() == pixmap.serialNumber() )
	return;

    bool newSize;
    if ( bpixmap ) {
	newSize = pixmap.width() != bpixmap->width() ||
		  pixmap.height() != bpixmap->height();
	*bpixmap = pixmap;
    } else {
	newSize = true;
	bpixmap = new TQPixmap( pixmap );
	TQ_CHECK_PTR( bpixmap );
    }
    if ( bpixmap->depth() == 1 && !bpixmap->mask() )
	bpixmap->setMask( *((TQBitmap *)bpixmap) );
    if ( !btext.isNull() ) {
	btext = TQString::null;
#ifndef TQT_NO_ACCEL
	setAccel( TQKeySequence() );
#endif
    }
    if ( autoresize && newSize )
	adjustSize();
    if ( autoMask() )
	updateMask();
    update();
    if ( newSize )
	updateGeometry();
}


#ifndef TQT_NO_ACCEL
TQKeySequence TQButton::accel() const
{
    if ( d && d->a )
	return d->a->key( 0 );
    return TQKeySequence();
}

void TQButton::setAccel( const TQKeySequence& key )
{
    if ( d && d->a )
	d->a->clear();
    if ( key.isEmpty() )
	return;
    ensureData();
    if ( !d->a ) {
	d->a = new TQAccel( this, "buttonAccel" );
	connect( d->a, TQ_SIGNAL( activated(int) ), this, TQ_SLOT( animateClick() ) );
	connect( d->a, TQ_SIGNAL( activatedAmbiguously(int) ), this, TQ_SLOT( setFocus() ) );
    }
    d->a->insertItem( key, 0 );
}
#endif

#ifndef TQT_NO_COMPAT

void TQButton::setAutoResize( bool enable )
{
    if ( (bool)autoresize != enable ) {
	autoresize = enable;
	if ( autoresize )
	    adjustSize();			// calls resize which repaints
    }
}

#endif

void TQButton::setAutoRepeat( bool enable )
{
    repeat = (uint)enable;
    if ( repeat && mlbDown )
	timer()->start( AUTO_REPEAT_DELAY, true );
}

/*!
    Performs an animated click: the button is pressed and released a
    short while later.

    The pressed(), released(), clicked(), toggled(), and
    stateChanged() signals are emitted as appropriate.

    This function does nothing if the button is \link setEnabled()
    disabled. \endlink

    \sa setAccel()
*/

void TQButton::animateClick()
{
    if ( !isEnabled() || animation )
	return;
    animation = true;
    buttonDown = true;
    repaint( false );
    emit pressed();
    TQTimer::singleShot( 100, this, TQ_SLOT(animateTimeout()) );
}

void TQButton::emulateClick()
{
    if ( !isEnabled() || animation )
	return;
    animation = true;
    buttonDown = true;
    emit pressed();
    animateTimeout();
}

void TQButton::setDown( bool enable )
{
    if ( d )
	timer()->stop();
    mlbDown = false;				// the safe setting
    if ( (bool)buttonDown != enable ) {
	buttonDown = enable;
	repaint( false );
#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( this, 0, TQAccessible::StateChanged );
#endif
    }
}

/*!
  Sets the toggle state of the button to \a s. \a s can be \c Off, \c
  NoChange or \c On.
*/

void TQButton::setState( ToggleState s )
{
    if ( !toggleTyp ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQButton::setState() / setOn: (%s) Only toggle buttons "
		 "may be switched", name( "unnamed" ) );
#endif
	return;
    }

    if ( (ToggleState)stat != s ) {		// changed state
	bool was = stat != Off;
	stat = s;
	if ( autoMask() )
	    updateMask();
	repaint( false );
#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( this, 0, TQAccessible::StateChanged );
#endif
	// ### toggled for tristate makes no sense. Don't emit the signal in 4.0
	if ( was != (stat != Off) )
	    emit toggled( stat != Off );
	emit stateChanged( s );
    }
}


/*!
    Returns true if \a pos is inside the clickable button rectangle;
    otherwise returns false.

    By default, the clickable area is the entire widget. Subclasses
    may reimplement it, though.
*/
bool TQButton::hitButton( const TQPoint &pos ) const
{
    return rect().contains( pos );
}

/*!
    Draws the button. The default implementation does nothing.

    This virtual function is reimplemented by subclasses to draw real
    buttons. At some point, these reimplementations should call
    drawButtonLabel().

    \sa drawButtonLabel(), paintEvent()
*/
#if (TQT_VERSION-0 >= 0x040000)
#error "TQButton. Make pure virtual"
#endif
void TQButton::drawButton( TQPainter * )
{
    return;
}

/*!
    Draws the button text or pixmap.

    This virtual function is reimplemented by subclasses to draw real
    buttons. It is invoked by drawButton().

    \sa drawButton(), paintEvent()
*/

void TQButton::drawButtonLabel( TQPainter * )
{
    return;
}

/*! \reimp */
void TQButton::keyPressEvent( TQKeyEvent *e )
{
    switch ( e->key() ) {
    case Key_Enter:
    case Key_Return:
	{
#ifndef TQT_NO_PUSHBUTTON
	    TQPushButton *pb = (TQPushButton*)tqt_cast( "TQPushButton" );
	    if ( pb && ( pb->autoDefault() || pb->isDefault() ) )
		emit clicked();
	    else
#endif
		e->ignore();
	}
	break;
    case Key_Space:
	if ( !e->isAutoRepeat() ) {
	    setDown( true );
#ifndef TQT_NO_PUSHBUTTON
	    if ( ::tqt_cast<TQPushButton*>(this) )
		emit pressed();
	    else
#endif
		e->ignore();
	}
	break;
    case Key_Up:
    case Key_Left:
#ifndef TQT_NO_BUTTONGROUP
	if ( group() ) {
	    group()->moveFocus( e->key() );
	} else
#endif
	{
	    TQFocusEvent::setReason(TQFocusEvent::Backtab);
	    focusNextPrevChild( false );
	    TQFocusEvent::resetReason();
	}
	break;
    case Key_Right:
    case Key_Down:
#ifndef TQT_NO_BUTTONGROUP
	if ( group() ) {
	    group()->moveFocus( e->key() );
	} else
#endif
	{
	    TQFocusEvent::setReason(TQFocusEvent::Tab);
	    focusNextPrevChild( true );
	    TQFocusEvent::resetReason();
	}
	break;
    case Key_Escape:
	if ( buttonDown ) {
	    buttonDown = false;
	    update();
	    break;
	}
	// fall through
    default:
	e->ignore();
    }
}

/*! \reimp */
void TQButton::keyReleaseEvent( TQKeyEvent * e)
{
    switch ( e->key() ) {
    case Key_Space:
	if ( buttonDown && !e->isAutoRepeat() ) {
	    buttonDown = false;
	    nextState();
	    emit released();
	    emit clicked();
	}
	break;
    default:
	e->ignore();
    }
}

/*! \reimp */
void TQButton::mousePressEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton ) {
	e->ignore();
	return;
    }
    bool hit = hitButton( e->pos() );
    if ( hit ) {				// mouse press on button
	mlbDown = true;				// left mouse button down
	buttonDown = true;
	if ( autoMask() )
	    updateMask();

	repaint( false );
#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( this, 0, TQAccessible::StateChanged );
#endif
	TQGuardedPtr<TQTimer> t = timer();
	emit pressed();
	if ( t && repeat )
	    t->start( AUTO_REPEAT_DELAY, true );
    }
}

/*! \reimp */
void TQButton::mouseReleaseEvent( TQMouseEvent *e)
{
    if ( e->button() != LeftButton ) {

	// clean up apperance if left button has been pressed
	if (mlbDown || buttonDown) {
	    mlbDown = false;
	    buttonDown = false;

	    if ( autoMask() )
		updateMask();
	    repaint( false );
	}

	e->ignore();
	return;
    }
    if ( !mlbDown )
	return;
    if ( d )
	timer()->stop();

    const bool oldButtonDown = buttonDown;
    mlbDown = false;				// left mouse button up
    buttonDown = false;
    if ( hitButton( e->pos() ) ) {		// mouse release on button
	nextState();
#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( this, 0, TQAccessible::StateChanged );
#endif
	emit released();
	emit clicked();
    } else {
	repaint( false );
#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( this, 0, TQAccessible::StateChanged );
#endif
        if (oldButtonDown)
            emit released();
    }
}

/*! \reimp */
void TQButton::mouseMoveEvent( TQMouseEvent *e )
{
    if ( !((e->state() & LeftButton) && mlbDown) ) {
	e->ignore();
	return;					// left mouse button is up
    }
    if ( hitButton(e->pos()) ) {		// mouse move in button
	if ( !buttonDown ) {
	    buttonDown = true;
	    repaint( false );
#if defined(QT_ACCESSIBILITY_SUPPORT)
	    TQAccessible::updateAccessibility( this, 0, TQAccessible::StateChanged );
#endif
	    emit pressed();
	}
    } else {					// mouse move outside button
	if ( buttonDown ) {
	    buttonDown = false;
	    repaint( false );
#if defined(QT_ACCESSIBILITY_SUPPORT)
	    TQAccessible::updateAccessibility( this, 0, TQAccessible::StateChanged );
#endif
	    emit released();
	}
    }
}


/*!
    Handles paint events for buttons. Small and typically complex
    buttons are painted double-buffered to reduce flicker. The
    actually drawing is done in the virtual functions drawButton() and
    drawButtonLabel().

    \sa drawButton(), drawButtonLabel()
*/
void TQButton::paintEvent( TQPaintEvent *)
{
    TQSharedDoubleBuffer buffer( this );
    drawButton( buffer.painter() );
}

/*! \reimp */
void TQButton::focusInEvent( TQFocusEvent * e)
{
    TQWidget::focusInEvent( e );
}

/*! \reimp */
void TQButton::focusOutEvent( TQFocusEvent * e )
{
    buttonDown = false;
    TQWidget::focusOutEvent( e );
}

/*!
    Internal slot used for auto repeat.
*/
void TQButton::autoRepeatTimeout()
{
    if ( mlbDown && isEnabled() && autoRepeat() ) {
	TQGuardedPtr<TQTimer> t = timer();
	if ( buttonDown ) {
	    emit released();
	    emit clicked();
	    emit pressed();
	}
	if ( t )
	    t->start( AUTO_REPEAT_PERIOD, true );
    }
}

/*!
    Internal slot used for the second stage of animateClick().
*/
void TQButton::animateTimeout()
{
    if ( !animation )
	return;
    animation  = false;
    buttonDown = false;
    nextState();
    emit released();
    emit clicked();
}


void TQButton::nextState()
{
    bool t = isToggleButton() && !( isOn() && isExclusiveToggle() );
    bool was = stat != Off;
    if ( t ) {
	if ( toggleTyp == Tristate )
	    stat = ( stat + 1 ) % 3;
	else
	    stat = stat ? Off : On;
    }
    if ( autoMask() )
        updateMask();
    repaint( false );
    if ( t ) {
#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( this, 0, TQAccessible::StateChanged );
#endif
	if ( was != (stat != Off) )
	    emit toggled( stat != Off );
	emit stateChanged( stat );
    }
}

/*! \reimp */
void TQButton::enabledChange( bool e )
{
    if ( !isEnabled() )
	setDown( false );
    TQWidget::enabledChange( e );
}


/*!
    Toggles the state of a toggle button.

    \sa isOn(), setOn(), toggled(), isToggleButton()
*/
void TQButton::toggle()
{
    if ( isToggleButton() )
	 setOn( !isOn() );
}

/*!
    Sets the toggle type of the button to \a type.

    \a type can be set to \c SingleShot, \c Toggle and \c Tristate.
*/
void TQButton::setToggleType( ToggleType type )
{
    toggleTyp = type;
    if ( type != Tristate && stat == NoChange )
	setState( On );
#if defined(QT_ACCESSIBILITY_SUPPORT)
    else
	TQAccessible::updateAccessibility( this, 0, TQAccessible::StateChanged );
#endif
}

bool TQButton::isExclusiveToggle() const
{
#ifndef TQT_NO_BUTTONGROUP
    return group() && ( group()->isExclusive() ||
			( group()->isRadioButtonExclusive() &&
			::tqt_cast<TQRadioButton*>(this) ) );
#else
    return false;
#endif
}

#endif
