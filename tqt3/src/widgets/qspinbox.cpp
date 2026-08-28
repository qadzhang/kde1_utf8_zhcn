/****************************************************************************
**
** Implementation of TQSpinBox widget class
**
** Created : 970101
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

#include "ntqspinbox.h"
#ifndef TQT_NO_SPINBOX

#include "ntqcursor.h"
#include "ntqpushbutton.h"
#include "ntqpainter.h"
#include "ntqbitmap.h"
#include "ntqlineedit.h"
#include "ntqvalidator.h"
#include "ntqpixmapcache.h"
#include "ntqapplication.h"
#include "ntqstyle.h"
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif

static bool sumOutOfRange(int current, int add)
{
    if (add > 0 && INT_MAX - add < current) {
        return true;
    }
    if (add < 0 && INT_MIN - add > current) {
        return true;
    }
    return false;
}

class TQSpinBoxPrivate
{
public:
    TQSpinBoxPrivate() {}
    TQSpinWidget* controls;
    uint selreq	: 1;
};

class TQSpinBoxValidator : public TQIntValidator
{
public:
    TQSpinBoxValidator( TQSpinBox *sb, const char *name )
	: TQIntValidator( sb, name ), spinBox( sb ) { }

    virtual State validate( TQString& str, int& pos ) const;

private:
    TQSpinBox *spinBox;
};

TQValidator::State TQSpinBoxValidator::validate( TQString& str, int& pos ) const
{
    TQString pref = spinBox->prefix();
    TQString suff = spinBox->suffix();
    TQString suffStriped = suff.stripWhiteSpace();
    uint overhead = pref.length() + suff.length();
    State state = Invalid;

    ((TQIntValidator *) this)->setRange( spinBox->minValue(),
					spinBox->maxValue() );
    if ( overhead == 0 ) {
	state = TQIntValidator::validate( str, pos );
    } else {
	bool stripedVersion = false;
	if ( str.length() >= overhead && str.startsWith(pref)
	     && (str.endsWith(suff)
		 || (stripedVersion = str.endsWith(suffStriped))) ) {
	    if ( stripedVersion )
		overhead = pref.length() + suffStriped.length();
	    TQString core = str.mid( pref.length(), str.length() - overhead );
	    int corePos = pos - pref.length();
	    state = TQIntValidator::validate( core, corePos );
	    pos = corePos + pref.length();
	    str.replace( pref.length(), str.length() - overhead, core );
	} else {
	    state = TQIntValidator::validate( str, pos );
	    if ( state == Invalid ) {
		// stripWhiteSpace(), cf. TQSpinBox::interpretText()
		TQString special = spinBox->specialValueText().stripWhiteSpace();
		TQString candidate = str.stripWhiteSpace();

		if ( special.startsWith(candidate) ) {
		    if ( candidate.length() == special.length() ) {
			state = Acceptable;
		    } else {
			state = Intermediate;
		    }
		}
	    }
	}
    }
    return state;
}

/*!
    \class TQSpinBox
    \brief The TQSpinBox class provides a spin box widget (spin button).

    \ingroup basic
    \mainclass

    TQSpinBox allows the user to choose a value either by clicking the
    up/down buttons to increase/decrease the value currently displayed
    or by typing the value directly into the spin box. If the value is
    entered directly into the spin box, Enter (or Return) must be
    pressed to apply the new value. The value is usually an integer.

    Every time the value changes TQSpinBox emits the valueChanged()
    signal. The current value can be fetched with value() and set
    with setValue().

    The spin box keeps the value within a numeric range, and to
    multiples of the lineStep() size (see TQRangeControl for details).
    Clicking the up/down buttons or using the keyboard accelerator's
    up and down arrows will increase or decrease the current value in
    steps of size lineStep(). The minimum and maximum value and the
    step size can be set using one of the constructors, and can be
    changed later with setMinValue(), setMaxValue() and setLineStep().

    Most spin boxes are directional, but TQSpinBox can also operate as
    a circular spin box, i.e. if the range is 0-99 and the current
    value is 99, clicking "up" will give 0. Use setWrapping() if you
    want circular behavior.

    The displayed value can be prepended and appended with arbitrary
    strings indicating, for example, currency or the unit of
    measurement. See setPrefix() and setSuffix(). The text in the spin
    box is retrieved with text() (which includes any prefix() and
    suffix()), or with cleanText() (which has no prefix(), no suffix()
    and no leading or trailing whitespace). currentValueText() returns
    the spin box's current value as text.

    Normally the spin box displays up and down arrows in the buttons.
    You can use setButtonSymbols() to change the display to show
    <b>+</b> and <b>-</b> symbols if you prefer. In either case the up
    and down arrow keys work as expected.

    It is often desirable to give the user a special (often default)
    choice in addition to the range of numeric values. See
    setSpecialValueText() for how to do this with TQSpinBox.

    The default \l TQWidget::focusPolicy() is StrongFocus.

    If using prefix(), suffix() and specialValueText() don't provide
    enough control, you can ignore them and subclass TQSpinBox instead.

    TQSpinBox can easily be subclassed to allow the user to input
    things other than an integer value as long as the allowed input
    can be mapped to a range of integers. This can be done by
    overriding the virtual functions mapValueToText() and
    mapTextToValue(), and setting another suitable validator using
    setValidator().

    For example, these functions could be changed so that the user
    provided values from 0.0 to 10.0, or -1 to signify 'Auto', while
    the range of integers used inside the program would be -1 to 100:

    \code
	class MySpinBox : public TQSpinBox
	{
	    TQ_OBJECT
	public:
	    ...

	    TQString mapValueToText( int value )
	    {
		if ( value == -1 ) // special case
		    return TQString( "Auto" );

		return TQString( "%1.%2" ) // 0.0 to 10.0
		    .arg( value / 10 ).arg( value % 10 );
	    }

	    int mapTextToValue( bool *ok )
	    {
		if ( text() == "Auto" ) // special case
		    return -1;

		return (int) ( 10 * text().toFloat() ); // 0 to 100
	    }
	};
    \endcode

    <img src=qspinbox-m.png> <img src=qspinbox-w.png>

    \sa TQScrollBar TQSlider
    \link guibooks.html#fowler GUI Design Handbook: Spin Box \endlink
*/


/*!
    Constructs a spin box with the default TQRangeControl range and
    step values. It is called \a name and has parent \a parent.

    \sa minValue(), maxValue(), setRange(), lineStep(), setSteps()
*/

TQSpinBox::TQSpinBox( TQWidget * parent , const char *name )
    : TQWidget( parent, name, WNoAutoErase ),
      TQRangeControl()
{
    initSpinBox();
}


/*!
    Constructs a spin box that allows values from \a minValue to \a
    maxValue inclusive, with step amount \a step. The value is
    initially set to \a minValue.

    The spin box is called \a name and has parent \a parent.

    \sa minValue(), maxValue(), setRange(), lineStep(), setSteps()
*/

TQSpinBox::TQSpinBox( int minValue, int maxValue, int step, TQWidget* parent,
		    const char* name )
    : TQWidget( parent, name, WNoAutoErase ),
      TQRangeControl( minValue, maxValue, step, step, minValue )
{
    initSpinBox();
}

/*
  \internal Initialization.
*/

void TQSpinBox::initSpinBox()
{
    d = new TQSpinBoxPrivate;

    d->controls = new TQSpinWidget( this, "controls" );
    connect( d->controls, TQ_SIGNAL( stepUpPressed() ), TQ_SLOT( stepUp() ) );
    connect( d->controls, TQ_SIGNAL( stepDownPressed() ), TQ_SLOT( stepDown() ) );

    wrap = false;
    edited = false;
    d->selreq = false;

    validate = new TQSpinBoxValidator( this, "validator" );
    vi = new TQLineEdit( this, "qt_spinbox_edit" );
    d->controls->setEditWidget( vi );
    vi->setValidator( validate );
    vi->installEventFilter( this );
    vi->setFrame( false );
    setFocusProxy( vi );

    setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Fixed ) );
    setBackgroundMode( PaletteBackground, PaletteBase );

    updateDisplay();

    connect( vi, TQ_SIGNAL(textChanged(const TQString&)), TQ_SLOT(textChanged()) );
}

/*!
    Destroys the spin box, freeing all memory and other resources.
*/

TQSpinBox::~TQSpinBox()
{
    delete d;
}


/*!
    \property TQSpinBox::text
    \brief the spin box's text, including any prefix() and suffix()

    There is no default text.

    \sa value()
*/

TQString TQSpinBox::text() const
{
    return vi->text();
}



/*!
    \property TQSpinBox::cleanText
    \brief the spin box's text with no prefix(), suffix() or leading
    or trailing whitespace.

    \sa text, prefix, suffix
*/

TQString TQSpinBox::cleanText() const
{
    TQString s = TQString(text()).stripWhiteSpace();
    if ( !prefix().isEmpty() ) {
	TQString px = TQString(prefix()).stripWhiteSpace();
	int len = px.length();
	if ( len && s.left(len) == px )  // Remove _only_ if it is the prefix
	    s.remove( (uint)0, len );
    }
    if ( !suffix().isEmpty() ) {
	TQString sx = TQString(suffix()).stripWhiteSpace();
	int len = sx.length();
	if ( len && s.right(len) == sx )  // Remove _only_ if it is the suffix
	    s.truncate( s.length() - len );
    }
    return s.stripWhiteSpace();
}


/*!
    \property TQSpinBox::specialValueText
    \brief the special-value text

    If set, the spin box will display this text instead of a numeric
    value whenever the current value is equal to minVal(). Typical use
    is to indicate that this choice has a special (default) meaning.

    For example, if your spin box allows the user to choose the margin
    width in a print dialog and your application is able to
    automatically choose a good margin width, you can set up the spin
    box like this:
    \code
	TQSpinBox marginBox( -1, 20, 1, parent, "marginBox" );
	marginBox->setSuffix( " mm" );
	marginBox->setSpecialValueText( "Auto" );
    \endcode
    The user will then be able to choose a margin width from 0-20
    millimeters or select "Auto" to leave it to the application to
    choose. Your code must then interpret the spin box value of -1 as
    the user requesting automatic margin width.

    All values are displayed with the prefix() and suffix() (if set),
    \e except for the special value, which only shows the special
    value text.

    To turn off the special-value text display, call this function
    with an empty string. The default is no special-value text, i.e.
    the numeric value is shown as usual.

    If no special-value text is set, specialValueText() returns
    TQString::null.
*/

void TQSpinBox::setSpecialValueText( const TQString &text )
{
    specText = text;
    updateDisplay();
}


TQString TQSpinBox::specialValueText() const
{
    if ( specText.isEmpty() )
	return TQString::null;
    else
	return specText;
}


/*!
    \property TQSpinBox::prefix
    \brief the spin box's prefix

    The prefix is prepended to the start of the displayed value.
    Typical use is to display a unit of measurement or a currency
    symbol. For example:

    \code
	sb->setPrefix( "$" );
    \endcode

    To turn off the prefix display, set this property to an empty
    string. The default is no prefix. The prefix is not displayed for
    the minValue() if specialValueText() is not empty.

    If no prefix is set, prefix() returns TQString::null.

    \sa suffix()
*/

void TQSpinBox::setPrefix( const TQString &text )
{
    pfix = text;
    updateDisplay();
}


TQString TQSpinBox::prefix() const
{
    if ( pfix.isEmpty() )
	return TQString::null;
    else
	return pfix;
}


/*!
    \property TQSpinBox::suffix
    \brief the suffix of the spin box

    The suffix is appended to the end of the displayed value. Typical
    use is to display a unit of measurement or a currency symbol. For
    example:

    \code
	sb->setSuffix( " km" );
    \endcode

    To turn off the suffix display, set this property to an empty
    string. The default is no suffix. The suffix is not displayed for
    the minValue() if specialValueText() is not empty.

    If no suffix is set, suffix() returns a TQString::null.

    \sa prefix()
*/

void TQSpinBox::setSuffix( const TQString &text )
{
    sfix = text;
    updateDisplay();
}

TQString TQSpinBox::suffix() const
{
    if ( sfix.isEmpty() )
	return TQString::null;
    else
	return sfix;
}


/*!
    \property TQSpinBox::wrapping
    \brief whether it is possible to step the value from the highest
    value to the lowest value and vice versa

    By default, wrapping is turned off.

    If you have a range of 0..100 and wrapping is off when the user
    reaches 100 and presses the Up Arrow nothing will happen; but if
    wrapping is on the value will change from 100 to 0, then to 1,
    etc. When wrapping is on, navigating past the highest value takes
    you to the lowest and vice versa.

    \sa minValue, maxValue, setRange()
*/

void TQSpinBox::setWrapping( bool on )
{
    wrap = on;
    updateDisplay();
}

bool TQSpinBox::wrapping() const
{
    return wrap;
}

/*!
    \reimp
*/
TQSize TQSpinBox::sizeHint() const
{
    constPolish();
    TQSize sz = vi->sizeHint();
    int h = sz.height();
    TQFontMetrics fm( font() );
    int w = 35;
    int wx = fm.width( ' ' )*2;
    TQString s;
    s = prefix() + ( (TQSpinBox*)this )->mapValueToText( minValue() ) + suffix();
    w = TQMAX( w, fm.width( s ) + wx);
    s = prefix() + ( (TQSpinBox*)this )->mapValueToText( maxValue() ) + suffix();
    w = TQMAX(w, fm.width( s ) + wx );
    if ( !specialValueText().isEmpty() ) {
	s = specialValueText();
	w = TQMAX( w, fm.width( s ) + wx );
    }
    return style().sizeFromContents(TQStyle::CT_SpinBox, this,
				    TQSize( w + d->controls->downRect().width(),
					   h + style().pixelMetric( TQStyle::PM_DefaultFrameWidth ) * 2).
				    expandedTo( TQApplication::globalStrut() ));
}


/*!
    \reimp
*/
TQSize TQSpinBox::minimumSizeHint() const
{
    int w = vi->minimumSizeHint().width() + d->controls->downRect().width();
    int h = TQMAX( vi->minimumSizeHint().height(), d->controls->minimumSizeHint().height() );
    return TQSize( w, h );
}

// Does the layout of the lineedit and the buttons

void TQSpinBox::arrangeWidgets()
{
    d->controls->arrange();
}

/*!
    \property TQSpinBox::value
    \brief the value of the spin box

    \sa TQRangeControl::setValue()
*/

void TQSpinBox::setValue( int value )
{
    edited = false; // we ignore anything entered and not yet interpreted
    TQRangeControl::setValue( value );
    updateDisplay();
}

int TQSpinBox::value() const
{
    TQSpinBox * that = (TQSpinBox *) this;
    if ( edited ) {
	that->edited = false;  // avoid recursion
	that->interpretText();
    }
    return TQRangeControl::value();
}


/*!
    Increases the spin box's value by one lineStep(), wrapping as
    necessary if wrapping() is true. This is the same as clicking on
    the pointing-up button and can be used for keyboard accelerators,
    for example.

    \sa stepDown(), addLine(), lineStep(), setSteps(), setValue(), value()
*/

void TQSpinBox::stepUp()
{
    if ( edited )
	interpretText();
    if ( wrapping() && ( value()+lineStep() > maxValue() || sumOutOfRange(value(), lineStep() ) ) ) {
	setValue( minValue() );
    } else {
	addLine();
    }
}


/*!
    Decreases the spin box's value one lineStep(), wrapping as
    necessary if wrapping() is true. This is the same as clicking on
    the pointing-down button and can be used for keyboard
    accelerators, for example.

    \sa stepUp(), subtractLine(), lineStep(), setSteps(), setValue(), value()
*/

void TQSpinBox::stepDown()
{
    if ( edited )
	interpretText();
    if ( wrapping() && ( value()-lineStep() < minValue() || sumOutOfRange(value(), -lineStep() ) ) ) {
	setValue( maxValue() );
    } else {
	subtractLine();
    }
}


/*!
    \fn void TQSpinBox::valueChanged( int value )

    This signal is emitted every time the value of the spin box
    changes; the new value is passed in \a value. This signal will be
    emitted as a result of a call to setValue(), or because the user
    changed the value by using a keyboard accelerator or mouse click,
    etc.

    Note that the valueChanged() signal is emitted \e every time, not
    just for the "last" step; i.e. if the user clicks "up" three
    times, this signal is emitted three times.

    \sa value()
*/


/*!
    \fn void TQSpinBox::valueChanged( const TQString& valueText )

    \overload

    This signal is emitted whenever the valueChanged( int ) signal is
    emitted, i.e. every time the value of the spin box changes
    (whatever the cause, e.g. by setValue(), by a keyboard
    accelerator, by mouse clicks, etc.).

    The \a valueText parameter is the same string that is displayed in
    the edit field of the spin box.

    \sa value() prefix() suffix() specialValueText()
*/



/*!
    Intercepts and handles the events coming to the embedded TQLineEdit
    that have special meaning for the TQSpinBox. The object is passed
    as \a o and the event is passed as \a ev.
*/

bool TQSpinBox::eventFilter( TQObject* o, TQEvent* ev )
{
    if (o != vi)
	return TQWidget::eventFilter(o,ev);

    if ( ev->type() == TQEvent::KeyPress ) {
	TQKeyEvent* k = (TQKeyEvent*)ev;

	bool retval = false; // workaround for MSVC++ optimization bug
	if( (k->key() == Key_Tab) || (k->key() == Key_BackTab) ){
	    if ( k->state() & TQt::ControlButton )
		return false;
	    if ( edited )
		interpretText();
	    tqApp->sendEvent( this, ev );
	    retval = true;
	} if ( k->key() == Key_Up ) {
	    stepUp();
	    retval = true;
	} else if ( k->key() == Key_Down ) {
	    stepDown();
	    retval = true;
	} else if ( k->key() == Key_Enter || k->key() == Key_Return ) {
	    interpretText();
	    return false;
	}
	if ( retval )
	    return retval;
    } else if ( ev->type() == TQEvent::FocusOut || ev->type() == TQEvent::Hide ) {
	if ( edited ) {
	    interpretText();
	}
	return false;
    }
    return false;
}

/*!
    \reimp
 */
void TQSpinBox::setEnabled( bool enabled )
{
    TQWidget::setEnabled( enabled );
    updateDisplay();
}

/*!
    \reimp
*/
void TQSpinBox::leaveEvent( TQEvent* )
{
}


/*!
    \reimp
*/
void TQSpinBox::resizeEvent( TQResizeEvent* )
{
    d->controls->resize( width(), height() );
}

/*!
    \reimp
*/
#ifndef TQT_NO_WHEELEVENT
void TQSpinBox::wheelEvent( TQWheelEvent * e )
{
    e->accept();
    static float offset = 0;
    static TQSpinBox* offset_owner = 0;
    if (offset_owner != this) {
	offset_owner = this;
	offset = 0;
    }
    offset += -e->delta()/120;
    if (TQABS(offset) < 1)
	return;
    int ioff = int(offset);
    int i;
    for (i=0; i<TQABS(ioff); i++)
	offset > 0 ? stepDown() : stepUp();
    offset -= ioff;
}
#endif

/*!
    This virtual function is called by TQRangeControl whenever the
    value has changed. The TQSpinBox reimplementation updates the
    display and emits the valueChanged() signals; if you need
    additional processing, either reimplement this or connect to one
    of the valueChanged() signals.
*/

void TQSpinBox::valueChange()
{
    d->selreq = hasFocus();
    updateDisplay();
    d->selreq = false;
    emit valueChanged( value() );
    emit valueChanged( currentValueText() );
#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::ValueChanged );
#endif
}


/*!
    This virtual function is called by TQRangeControl whenever the
    range has changed. It adjusts the default validator and updates
    the display; if you need additional processing, you can
    reimplement this function.
*/

void TQSpinBox::rangeChange()
{
    updateDisplay();
}


/*!
    Sets the validator to \a v. The validator controls what keyboard
    input is accepted when the user is editing in the value field. The
    default is to use a suitable TQIntValidator.

    Use setValidator(0) to turn off input validation (entered input
    will still be kept within the spin box's range).
*/

void TQSpinBox::setValidator( const TQValidator* v )
{
    if ( vi )
	vi->setValidator( v );
}


/*!
    Returns the validator that constrains editing for this spin box if
    there is any; otherwise returns 0.

    \sa setValidator() TQValidator
*/

const TQValidator * TQSpinBox::validator() const
{
    return vi ? vi->validator() : 0;
}

/*!
    Updates the contents of the embedded TQLineEdit to reflect the
    current value using mapValueToText(). Also enables/disables the
    up/down push buttons accordingly.

    \sa mapValueToText()
*/
void TQSpinBox::updateDisplay()
{
    vi->setUpdatesEnabled( false );
    vi->setText( currentValueText() );
    if ( d->selreq && isVisible() && ( hasFocus() || vi->hasFocus() ) ) {
	selectAll();
    } else {
	if ( !suffix().isEmpty() && vi->text().endsWith(suffix()) )
	     vi->setCursorPosition( vi->text().length() - suffix().length() );
    }
    vi->setUpdatesEnabled( true );
    vi->repaint( false ); // immediate repaint needed for some reason
    edited = false;

    bool upEnabled = isEnabled() && ( wrapping() || value() < maxValue() );
    bool downEnabled = isEnabled() && ( wrapping() || value() > minValue() );

    d->controls->setUpEnabled( upEnabled );
    d->controls->setDownEnabled( downEnabled );
    vi->setEnabled( isEnabled() );
    repaint( false );
}


/*!
    TQSpinBox calls this after the user has manually edited the
    contents of the spin box (i.e. by typing in the embedded
    TQLineEdit, rather than using the up/down buttons/keys).

    The default implementation of this function interprets the new
    text using mapTextToValue(). If mapTextToValue() is successful, it
    changes the spin box's value; if not, the value is left unchanged.

    \sa editor()
*/

void TQSpinBox::interpretText()
{
    bool ok = true;
    bool done = false;
    int newVal = 0;
    if ( !specialValueText().isEmpty() ) {
	TQString s = text().stripWhiteSpace();
	TQString t = specialValueText().stripWhiteSpace();
	if ( s == t ) {
	    newVal = minValue();
	    done = true;
	}
    }
    if ( !done )
	newVal = mapTextToValue( &ok );
    if ( ok )
	setValue( newVal );
    updateDisplay(); // sometimes redundant
}


/*!
    Returns the geometry of the "up" button.
*/

TQRect TQSpinBox::upRect() const
{
    return d->controls->upRect();
}


/*!
    Returns the geometry of the "down" button.
*/

TQRect TQSpinBox::downRect() const
{
    return d->controls->downRect();
}


/*!
    Returns a pointer to the embedded TQLineEdit.
*/

TQLineEdit* TQSpinBox::editor() const
{
    return vi;
}


/*!
    This slot is called whenever the user edits the spin box's text.
*/

void TQSpinBox::textChanged()
{
    edited = true; // this flag is cleared in updateDisplay()
}


/*!
    This virtual function is used by the spin box whenever it needs to
    display value \a v. The default implementation returns a string
    containing \a v printed in the standard way. Reimplementations may
    return anything. (See the example in the detailed description.)

    Note that TQt does not call this function for specialValueText()
    and that neither prefix() nor suffix() are included in the return
    value.

    If you reimplement this, you may also need to reimplement
    mapTextToValue().

    \sa updateDisplay(), mapTextToValue()
*/

TQString TQSpinBox::mapValueToText( int v )
{
    TQString s;
    s.setNum( v );
    return s;
}


/*!
    This virtual function is used by the spin box whenever it needs to
    interpret text entered by the user as a value. The text is
    available as text() and as cleanText(), and this function must
    parse it if possible. If \a ok is not 0: if it parses the text
    successfully, \a *ok is set to true; otherwise \a *ok is set to
    false.

    Subclasses that need to display spin box values in a non-numeric
    way need to reimplement this function.

    Note that TQt handles specialValueText() separately; this function
    is only concerned with the other values.

    The default implementation tries to interpret the text() as an
    integer in the standard way and returns the integer value.

    \sa interpretText(), mapValueToText()
*/

int TQSpinBox::mapTextToValue( bool* ok )
{
    TQString s = text();
    int newVal = s.toInt( ok );
    if ( !(*ok) && !( !prefix() && !suffix() ) ) {// Try removing any pre/suffix
	s = cleanText();
	newVal = s.toInt( ok );
    }
    return newVal;
}


/*!
    Returns the full text calculated from the current value, including
    any prefix and suffix. If there is special value text and the
    value is minValue() the specialValueText() is returned.
*/

TQString TQSpinBox::currentValueText()
{
    TQString s;
    if ( (value() == minValue()) && !specialValueText().isEmpty() ) {
	s = specialValueText();
    } else {
	s = prefix();
	s.append( mapValueToText( value() ) );
	s.append( suffix() );
    }
    return s;
}

/*!
    \reimp
*/

void TQSpinBox::styleChange( TQStyle& old )
{
    arrangeWidgets();
    TQWidget::styleChange( old );
}


/*!
    \enum TQSpinBox::ButtonSymbols

    This enum type determines what the buttons in a spin box show.

    \value UpDownArrows the buttons show little arrows in the classic
    style.

    \value PlusMinus the buttons show <b>+</b> and <b>-</b> symbols.

    \sa TQSpinBox::buttonSymbols
*/

/*!
    \property TQSpinBox::buttonSymbols

    \brief the current button symbol mode

    The possible values can be either \c UpDownArrows or \c PlusMinus.
    The default is \c UpDownArrows.

    \sa ButtonSymbols
*/

void TQSpinBox::setButtonSymbols( ButtonSymbols newSymbols )
{
    if ( buttonSymbols() == newSymbols )
	return;

    switch ( newSymbols ) {
    case UpDownArrows:
	d->controls->setButtonSymbols( TQSpinWidget::UpDownArrows );
	break;
    case PlusMinus:
	d->controls->setButtonSymbols( TQSpinWidget::PlusMinus );
	break;
    }
    //    repaint( false );
}

TQSpinBox::ButtonSymbols TQSpinBox::buttonSymbols() const
{
    switch( d->controls->buttonSymbols() ) {
    case TQSpinWidget::UpDownArrows:
	return UpDownArrows;
    case TQSpinWidget::PlusMinus:
	return PlusMinus;
    }
    return UpDownArrows;
}

/*!
    \property TQSpinBox::minValue

    \brief the minimum value of the spin box

    When setting this property, \l TQSpinBox::maxValue is adjusted, if
    necessary, to ensure that the range remains valid.

    \sa setRange() setSpecialValueText()
*/

int TQSpinBox::minValue() const
{
    return TQRangeControl::minValue();
}

void TQSpinBox::setMinValue( int minVal )
{
    TQRangeControl::setMinValue( minVal );
}

/*!
    \property TQSpinBox::maxValue
    \brief the maximum value of the spin box

    When setting this property, \l TQSpinBox::minValue is adjusted, if
    necessary, to ensure that the range remains valid.

    \sa setRange() setSpecialValueText()
*/

int TQSpinBox::maxValue() const
{
    return TQRangeControl::maxValue();
}

void TQSpinBox::setMaxValue( int maxVal )
{
    TQRangeControl::setMaxValue( maxVal );
}

/*!
    \property TQSpinBox::lineStep
    \brief the line step

    When the user uses the arrows to change the spin box's value the
    value will be incremented/decremented by the amount of the line
    step.

    The setLineStep() function calls the virtual stepChange() function
    if the new line step is different from the previous setting.

    \sa TQRangeControl::setSteps() setRange()
*/

int TQSpinBox::lineStep() const
{
    return TQRangeControl::lineStep();
}

void TQSpinBox::setLineStep( int i )
{
    setSteps( i, pageStep() );
}

/*!
    Selects all the text in the spin box's editor.
*/

void TQSpinBox::selectAll()
{
    int overhead = prefix().length() + suffix().length();
    if ( !overhead || currentValueText() == specialValueText() ) {
	vi->selectAll();
    } else {
	vi->setSelection( prefix().length(), vi->text().length() - overhead );
    }
}

#endif
