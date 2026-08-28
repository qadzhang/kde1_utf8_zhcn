/****************************************************************************
**
** Implementation of TQToolButton class
**
** Created : 980320
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
#include "ntqtoolbutton.h"
#ifndef TQT_NO_TOOLBUTTON

#include "ntqdrawutil.h"
#include "ntqpainter.h"
#include "ntqpixmap.h"
#include "ntqwmatrix.h"
#include "ntqapplication.h"
#include "ntqstyle.h"
#include "ntqmainwindow.h"
#include "ntqtooltip.h"
#include "ntqtoolbar.h"
#include "ntqimage.h"
#include "ntqiconset.h"
#include "ntqtimer.h"
#include "ntqpopupmenu.h"
#include "ntqguardedptr.h"

class TQToolButtonPrivate
{
    // ### add tool tip magic here
public:
#ifndef TQT_NO_POPUPMENU
    TQGuardedPtr<TQPopupMenu> popup;
    TQTimer* popupTimer;
    int delay;
#endif
    TQt::ArrowType arrow;
    uint instantPopup	    : 1;
    uint autoraise	    : 1;
    uint repeat		    : 1;
    uint discardNextMouseEvent : 1;
    TQToolButton::TextPosition textPos;
};


/*!
    \class TQToolButton ntqtoolbutton.h
    \brief The TQToolButton class provides a quick-access button to
    commands or options, usually used inside a TQToolBar.

    \ingroup basic
    \mainclass

    A tool button is a special button that provides quick-access to
    specific commands or options. As opposed to a normal command
    button, a tool button usually doesn't show a text label, but shows
    an icon instead. Its classic usage is to select tools, for example
    the "pen" tool in a drawing program. This would be implemented
    with a TQToolButton as toggle button (see setToggleButton() ).

    TQToolButton supports auto-raising. In auto-raise mode, the button
    draws a 3D frame only when the mouse points at it. The feature is
    automatically turned on when a button is used inside a TQToolBar.
    Change it with setAutoRaise().

    A tool button's icon is set as TQIconSet. This makes it possible to
    specify different pixmaps for the disabled and active state. The
    disabled pixmap is used when the button's functionality is not
    available. The active pixmap is displayed when the button is
    auto-raised because the mouse pointer is hovering over it.

    The button's look and dimension is adjustable with
    setUsesBigPixmap() and setUsesTextLabel(). When used inside a
    TQToolBar in a TQMainWindow, the button automatically adjusts to
    TQMainWindow's settings (see TQMainWindow::setUsesTextLabel() and
    TQMainWindow::setUsesBigPixmaps()). The pixmap set on a TQToolButton
    will be set to 22x22 if it is bigger than this size. If
    usesBigPixmap() is true, then the pixmap will be set to 32x32.

    A tool button can offer additional choices in a popup menu. The
    feature is sometimes used with the "Back" button in a web browser.
    After pressing and holding the button down for a while, a menu
    pops up showing a list of possible pages to jump to. With
    TQToolButton you can set a popup menu using setPopup(). The default
    delay is 600ms; you can adjust it with setPopupDelay().

    \img qdockwindow.png Toolbar with Toolbuttons \caption A floating
    TQToolbar with TQToolbuttons

    \sa TQPushButton TQToolBar TQMainWindow \link guibooks.html#fowler
    GUI Design Handbook: Push Button\endlink
*/

/*!
    \enum TQToolButton::TextPosition

    The position of the tool button's textLabel in relation to the
    tool button's icon.

    \value BesideIcon The text appears beside the icon.
    \value BelowIcon The text appears below the icon.
*/


/*!
    Constructs an empty tool button called \a name, with parent \a
    parent.
*/

TQToolButton::TQToolButton( TQWidget * parent, const char *name )
    : TQButton( parent, name )
{
    init();
#ifndef TQT_NO_TOOLBAR
    TQToolBar* tb = ::tqt_cast<TQToolBar*>(parent);
    if ( tb ) {
	setAutoRaise( true );
	if ( tb->mainWindow() ) {
	    connect( tb->mainWindow(), TQ_SIGNAL(pixmapSizeChanged(bool)),
	             this, TQ_SLOT(setUsesBigPixmap(bool)) );
	    setUsesBigPixmap( tb->mainWindow()->usesBigPixmaps() );
	    connect( tb->mainWindow(), TQ_SIGNAL(usesTextLabelChanged(bool)),
	             this, TQ_SLOT(setUsesTextLabel(bool)) );
	    setUsesTextLabel( tb->mainWindow()->usesTextLabel() );
	} else {
	    setUsesBigPixmap( false );
	}
    } else
#endif
    {
	setUsesBigPixmap( false );
    }
}


/*!
    Constructs a tool button as an arrow button. The \c ArrowType \a
    type defines the arrow direction. Possible values are \c
    LeftArrow, \c RightArrow, \c UpArrow and \c DownArrow.

    An arrow button has auto-repeat turned on by default.

    The \a parent and \a name arguments are sent to the TQWidget
    constructor.
*/
TQToolButton::TQToolButton( ArrowType type, TQWidget *parent, const char *name )
    : TQButton( parent, name )
{
    init();
    setUsesBigPixmap( false );
    setAutoRepeat( true );
    d->arrow = type;
    hasArrow = true;
}


/*  Set-up code common to all the constructors */

void TQToolButton::init()
{
    d = new TQToolButtonPrivate;
    d->textPos = Under;
#ifndef TQT_NO_POPUPMENU
    d->delay = 600;
    d->popup = 0;
    d->popupTimer = 0;
#endif
    d->autoraise = false;
    d->arrow = LeftArrow;
    d->instantPopup = false;
    d->discardNextMouseEvent = false;
    bpID = bp.serialNumber();
    spID = sp.serialNumber();

    utl = false;
    ubp = true;
    hasArrow = false;

    s = 0;

    setFocusPolicy( NoFocus );
    setBackgroundMode( PaletteButton);
    setSizePolicy( TQSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Minimum ) );
}

#ifndef TQT_NO_TOOLBAR

/*!
    Constructs a tool button called \a name, that is a child of \a
    parent (which must be a TQToolBar).

    The tool button will display \a iconSet, with its text label and
    tool tip set to \a textLabel and its status bar message set to \a
    grouptext. It will be connected to the \a slot in object \a
    receiver.
*/

TQToolButton::TQToolButton( const TQIconSet& iconSet, const TQString &textLabel,
			  const TQString& grouptext,
			  TQObject * receiver, const char *slot,
			  TQToolBar * parent, const char *name )
    : TQButton( parent, name )
{
    init();
    setAutoRaise( true );
    setIconSet( iconSet );
    setTextLabel( textLabel );
    if ( receiver && slot )
	connect( this, TQ_SIGNAL(clicked()), receiver, slot );
    if ( parent->mainWindow() ) {
	connect( parent->mainWindow(), TQ_SIGNAL(pixmapSizeChanged(bool)),
		 this, TQ_SLOT(setUsesBigPixmap(bool)) );
	setUsesBigPixmap( parent->mainWindow()->usesBigPixmaps() );
	connect( parent->mainWindow(), TQ_SIGNAL(usesTextLabelChanged(bool)),
		 this, TQ_SLOT(setUsesTextLabel(bool)) );
	setUsesTextLabel( parent->mainWindow()->usesTextLabel() );
    } else {
	setUsesBigPixmap( false );
    }
#ifndef TQT_NO_TOOLTIP
    if ( !textLabel.isEmpty() ) {
	if ( !grouptext.isEmpty() && parent->mainWindow() )
	    TQToolTip::add( this, textLabel,
			   parent->mainWindow()->toolTipGroup(), grouptext );
	else
	    TQToolTip::add( this, textLabel );
    } else if ( !grouptext.isEmpty() && parent->mainWindow() )
	TQToolTip::add( this, TQString::null,
		parent->mainWindow()->toolTipGroup(), grouptext );
#endif
}

#endif


/*!
    Destroys the object and frees any allocated resources.
*/

TQToolButton::~TQToolButton()
{
#ifndef TQT_NO_POPUPMENU
    d->popupTimer = 0;
    d->popup = 0;
#endif
    delete d;
    delete s;
}


/*!
    \property TQToolButton::backgroundMode
    \brief the toolbutton's background mode

    Get this property with backgroundMode().

    \sa TQWidget::setBackgroundMode()
*/

/*!
    \property TQToolButton::toggleButton
    \brief whether this tool button is a toggle button.

    Toggle buttons have an on/off state similar to \link TQCheckBox
    check boxes. \endlink A tool button is not a toggle button by
    default.

    \sa setOn(), toggle()
*/

void TQToolButton::setToggleButton( bool enable )
{
    TQButton::setToggleButton( enable );
}


/*!
    \reimp
*/
TQSize TQToolButton::sizeHint() const
{
    constPolish();

    int w = 0, h = 0;

    if ( iconSet().isNull() && !text().isNull() && !usesTextLabel() ) {
     	w = fontMetrics().width( text() );
     	h = fontMetrics().height(); // boundingRect()?
    } else if ( usesBigPixmap() ) {
     	TQPixmap pm = iconSet().pixmap( TQIconSet::Large, TQIconSet::Normal );
     	w = pm.width();
     	h = pm.height();
	TQSize iconSize = TQIconSet::iconSize( TQIconSet::Large );
	if ( w < iconSize.width() )
     	    w = iconSize.width();
     	if ( h < iconSize.height() )
     	    h = iconSize.height();
    } else if ( !iconSet().isNull() ) {
	// ### in 3.1, use TQIconSet::iconSize( TQIconSet::Small );
     	TQPixmap pm = iconSet().pixmap( TQIconSet::Small, TQIconSet::Normal );
     	w = pm.width();
     	h = pm.height();
	if ( w < 16 )
	    w = 16;
	if ( h < 16 )
	    h = 16;
    }

    if ( usesTextLabel() ) {
	TQSize textSize = fontMetrics().size( TQt::ShowPrefix, textLabel() );
	textSize.setWidth( textSize.width() + fontMetrics().width(' ')*2 );
	if ( d->textPos == Under ) {
	    h += 4 + textSize.height();
	    if ( textSize.width() > w )
		w = textSize.width();
	} else { // Right
	    w += 4 + textSize.width();
	    if ( textSize.height() > h )
		h = textSize.height();
	}
    }

#ifndef TQT_NO_POPUPMENU
    if ( popup() && ! popupDelay() )
     	w += style().pixelMetric(TQStyle::PM_MenuButtonIndicator, this);
#endif
    return (style().sizeFromContents(TQStyle::CT_ToolButton, this, TQSize(w, h)).
	    expandedTo(TQApplication::globalStrut()));
}

/*!
    \reimp
 */
TQSize TQToolButton::minimumSizeHint() const
{
    return sizeHint();
}

/*!
    \property TQToolButton::usesBigPixmap
    \brief whether this toolbutton uses big pixmaps.

    TQToolButton automatically connects this property to the relevant
    signal in the TQMainWindow in which it resides. We strongly
    recommend that you use TQMainWindow::setUsesBigPixmaps() instead.

    This property's default is true.

    \warning If you set some buttons (in a TQMainWindow) to have big
    pixmaps and others to have small pixmaps, TQMainWindow may not get
    the geometry right.
*/

void TQToolButton::setUsesBigPixmap( bool enable )
{
    if ( (bool)ubp == enable )
	return;

    ubp = enable;
    if ( isVisible() ) {
	update();
	updateGeometry();
    }
}


/*!
    \property TQToolButton::usesTextLabel
    \brief whether the toolbutton displays a text label below the button pixmap.

    The default is false.

    TQToolButton automatically connects this slot to the relevant
    signal in the TQMainWindow in which is resides.
*/

void TQToolButton::setUsesTextLabel( bool enable )
{
    if ( (bool)utl == enable )
	return;

    utl = enable;
    if ( isVisible() ) {
	update();
	updateGeometry();
    }
}


/*!
    \property TQToolButton::on
    \brief whether this tool button is on.

    This property has no effect on \link isToggleButton() non-toggling
    buttons. \endlink The default is false (i.e. off).

    \sa isToggleButton() toggle()
*/

void TQToolButton::setOn( bool enable )
{
    if ( !isToggleButton() )
	return;
    TQButton::setOn( enable );
}


/*!
    Toggles the state of this tool button.

    This function has no effect on \link isToggleButton() non-toggling
    buttons. \endlink

    \sa isToggleButton() toggled()
*/

void TQToolButton::toggle()
{
    if ( !isToggleButton() )
	return;
    TQButton::setOn( !isOn() );
}


/*!
    \reimp
*/
void TQToolButton::drawButton( TQPainter * p )
{
    TQStyle::SCFlags controls = TQStyle::SC_ToolButton;
    TQStyle::SCFlags active = TQStyle::SC_None;

    TQt::ArrowType arrowtype = d->arrow;

    if (isDown())
	active |= TQStyle::SC_ToolButton;

#ifndef TQT_NO_POPUPMENU
    if (d->popup && !d->delay) {
	controls |= TQStyle::SC_ToolButtonMenu;
	if (d->instantPopup || isDown())
	    active |= TQStyle::SC_ToolButtonMenu;
    }
#endif

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled())
	flags |= TQStyle::Style_Enabled;
    if (hasFocus())
	flags |= TQStyle::Style_HasFocus;
    if (isDown())
	flags |= TQStyle::Style_Down;
    if (isOn())
	flags |= TQStyle::Style_On;
    if (hasMouse())
	flags |= TQStyle::Style_MouseOver;
    if (autoRaise()) {
	flags |= TQStyle::Style_AutoRaise;
	if (uses3D()) {
	    flags |= TQStyle::Style_MouseOver;
	    if (! isOn() && ! isDown())
		flags |= TQStyle::Style_Raised;
	}
    } else if (! isOn() && ! isDown())
	flags |= TQStyle::Style_Raised;

    style().drawComplexControl(TQStyle::CC_ToolButton, p, this, rect(), colorGroup(),
			       flags, controls, active,
				hasArrow ? TQStyleOption(arrowtype) :
				    TQStyleOption());
    drawButtonLabel(p);
}


/*!
    \reimp
*/
void TQToolButton::drawButtonLabel(TQPainter *p)
{
    TQRect r =
	TQStyle::visualRect(style().subRect(TQStyle::SR_ToolButtonContents, this), this);

    TQt::ArrowType arrowtype = d->arrow;

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled())
	flags |= TQStyle::Style_Enabled;
    if (hasFocus())
	flags |= TQStyle::Style_HasFocus;
    if (isDown())
	flags |= TQStyle::Style_Down;
    if (isOn())
	flags |= TQStyle::Style_On;
    if (autoRaise()) {
	flags |= TQStyle::Style_AutoRaise;
	if (uses3D()) {
	    flags |= TQStyle::Style_MouseOver;
	    if (! isOn() && ! isDown())
		flags |= TQStyle::Style_Raised;
	}
    } else if (! isOn() && ! isDown())
	flags |= TQStyle::Style_Raised;

    style().drawControl(TQStyle::CE_ToolButtonLabel, p, this, r,
			colorGroup(), flags,
			hasArrow ? TQStyleOption(arrowtype) :
			    TQStyleOption());
}


/*!
    \reimp
 */
void TQToolButton::enterEvent( TQEvent * e )
{
    if ( autoRaise() && isEnabled() )
	repaint(false);

    TQButton::enterEvent( e );
}


/*!
    \reimp
 */
void TQToolButton::leaveEvent( TQEvent * e )
{
    if ( autoRaise() && isEnabled() )
	repaint(false);

    TQButton::leaveEvent( e );
}

/*!
    \reimp
 */
void TQToolButton::moveEvent( TQMoveEvent * )
{
    //   Reimplemented to handle pseudo transparency in case the toolbars
    //   has a fancy pixmap background.
    if ( parentWidget() && parentWidget()->backgroundPixmap() &&
	 autoRaise() && !uses3D() )
	repaint( false );
}

/*!
    \reimp
*/
void TQToolButton::mousePressEvent( TQMouseEvent *e )
{
    TQRect popupr =
	TQStyle::visualRect( style().querySubControlMetrics(TQStyle::CC_ToolButton, this,
				       TQStyle::SC_ToolButtonMenu), this );
    d->instantPopup = (popupr.isValid() && popupr.contains(e->pos()));

#ifndef TQT_NO_POPUPMENU
    if ( d->discardNextMouseEvent ) {
	d->discardNextMouseEvent = false;
	d->instantPopup = false;
	d->popup->removeEventFilter( this );
	return;
    }
    if ( e->button() == LeftButton && d->delay <= 0 && d->popup && d->instantPopup && !d->popup->isVisible() ) {
	openPopup();
	return;
    }
#endif

    d->instantPopup = false;
    TQButton::mousePressEvent( e );
}

/*!
    \reimp
*/
bool TQToolButton::eventFilter( TQObject *o, TQEvent *e )
{
#ifndef TQT_NO_POPUPMENU
    if ( o != d->popup )
	return TQButton::eventFilter( o, e );
    switch ( e->type() ) {
    case TQEvent::MouseButtonPress:
    case TQEvent::MouseButtonDblClick:
	{
	    TQMouseEvent *me = (TQMouseEvent*)e;
	    TQPoint p = me->globalPos();
	    if ( TQApplication::widgetAt( p, true ) == this )
		d->discardNextMouseEvent = true;
	}
	break;
    default:
	break;
    }
#endif
    return TQButton::eventFilter( o, e );
}

/*!
    Returns true if this button should be drawn using raised edges;
    otherwise returns false.

    \sa drawButton()
*/

bool TQToolButton::uses3D() const
{
    return style().styleHint(TQStyle::SH_ToolButton_Uses3D)
	&& (!autoRaise() || ( hasMouse() && isEnabled() )
#ifndef TQT_NO_POPUPMENU
	    || ( d->popup && d->popup->isVisible() && d->delay <= 0 ) || d->instantPopup
#endif
	    );
}


/*!
    \property TQToolButton::textLabel
    \brief the label of this button.

    Setting this property automatically sets the text as a tool tip
    too. There is no default text.
*/

void TQToolButton::setTextLabel( const TQString &newLabel )
{
    setTextLabel( newLabel, true );
}

/*!
    \overload

    Sets the label of this button to \a newLabel and automatically
    sets it as a tool tip if \a tipToo is true.
*/

void TQToolButton::setTextLabel( const TQString &newLabel , bool tipToo )
{
    if ( tl == newLabel )
	return;

#ifndef TQT_NO_TOOLTIP
    if ( tipToo ) {
        TQToolTip::remove( this );
        TQToolTip::add( this, newLabel );
    }
#endif

    tl = newLabel;
    if ( usesTextLabel() && isVisible() ) {
	update();
	updateGeometry();
    }

}

#ifndef TQT_NO_COMPAT

TQIconSet TQToolButton::onIconSet() const
{
    return iconSet();
}

TQIconSet TQToolButton::offIconSet( ) const
{
    return iconSet();
}


/*!
  \property TQToolButton::onIconSet
  \brief the icon set that is used when the button is in an "on" state

  \obsolete

  Since TQt 3.0, TQIconSet contains both the On and Off icons. There is
  now an \l TQToolButton::iconSet property that replaces both \l
  TQToolButton::onIconSet and \l TQToolButton::offIconSet.

  For ease of porting, this property is a synonym for \l
  TQToolButton::iconSet. You probably want to go over your application
  code and use the TQIconSet On/Off mechanism.

  \sa iconSet TQIconSet::State
*/
void TQToolButton::setOnIconSet( const TQIconSet& set )
{
    setIconSet( set );
    /*
      ### Get rid of all tqWarning in this file in 4.0.
      Also consider inlining the obsolete functions then.
    */
    tqWarning( "TQToolButton::setOnIconSet(): This function is not supported"
	      " anymore" );
}

/*!
  \property TQToolButton::offIconSet
  \brief the icon set that is used when the button is in an "off" state

  \obsolete

  Since TQt 3.0, TQIconSet contains both the On and Off icons. There is
  now an \l TQToolButton::iconSet property that replaces both \l
  TQToolButton::onIconSet and \l TQToolButton::offIconSet.

  For ease of porting, this property is a synonym for \l
  TQToolButton::iconSet. You probably want to go over your application
  code and use the TQIconSet On/Off mechanism.

  \sa iconSet TQIconSet::State
*/
void TQToolButton::setOffIconSet( const TQIconSet& set )
{
    setIconSet( set );
}

#endif

/*! \property TQToolButton::pixmap
    \brief the pixmap of the button

    The pixmap property has no meaning for tool buttons. Use the
    iconSet property instead.
*/

/*!
    \property TQToolButton::iconSet
    \brief the icon set providing the icon shown on the button

    Setting this property sets \l TQToolButton::pixmap to a null
    pixmap. There is no default iconset.

    \sa pixmap(), setToggleButton(), isOn()
*/
void TQToolButton::setIconSet( const TQIconSet & set )
{
    if ( s )
	delete s;
    setPixmap( TQPixmap() );
    s = new TQIconSet( set );
    if ( isVisible() )
	update();
}

/*! \overload
    \obsolete

  Since TQt 3.0, TQIconSet contains both the On and Off icons.

  For ease of porting, this function ignores the \a on parameter and
  sets the \l iconSet property. If you relied on the \a on parameter,
  you probably want to update your code to use the TQIconSet On/Off
  mechanism.

  \sa iconSet TQIconSet::State
*/

#ifndef TQT_NO_COMPAT

void TQToolButton::setIconSet( const TQIconSet & set, bool /* on */ )
{
    setIconSet( set );
    tqWarning( "TQToolButton::setIconSet(): 'on' parameter ignored" );
}

#endif

TQIconSet TQToolButton::iconSet() const
{
    TQToolButton *that = (TQToolButton *) this;

    if ( pixmap() && !pixmap()->isNull() &&
	 (!that->s || (that->s->pixmap().serialNumber() !=
	 pixmap()->serialNumber())) ) {
	if ( that->s )
	    delete that->s;
	that->s = new TQIconSet( *pixmap() );
    }
    if ( that->s )
	return *that->s;
    /*
      In 2.x, we used to return a temporary nonnull TQIconSet. If you
      revert to the old behavior, you will break calls to
      TQIconSet::isNull() in this file.
    */
    return TQIconSet();
}

#ifndef TQT_NO_COMPAT
/*! \overload
    \obsolete

  Since TQt 3.0, TQIconSet contains both the On and Off icons.

  For ease of porting, this function ignores the \a on parameter and
  returns the \l iconSet property. If you relied on the \a on
  parameter, you probably want to update your code to use the TQIconSet
  On/Off mechanism.
*/
TQIconSet TQToolButton::iconSet( bool /* on */ ) const
{
    return iconSet();
}

#endif

#ifndef TQT_NO_POPUPMENU
/*!
    Associates the popup menu \a popup with this tool button.

    The popup will be shown each time the tool button has been pressed
    down for a certain amount of time. A typical application example
    is the "back" button in some web browsers's tool bars. If the user
    clicks it, the browser simply browses back to the previous page.
    If the user presses and holds the button down for a while, the
    tool button shows a menu containing the current history list.

    Ownership of the popup menu is not transferred to the tool button.

    \sa popup()
*/
void TQToolButton::setPopup( TQPopupMenu* popup )
{
    if ( popup && !d->popupTimer ) {
	connect( this, TQ_SIGNAL( pressed() ), this, TQ_SLOT( popupPressed() ) );
	d->popupTimer = new TQTimer( this );
	connect( d->popupTimer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( popupTimerDone() ) );
    }
    d->popup = popup;

    update();
}

/*!
    Returns the associated popup menu, or 0 if no popup menu has been
    defined.

    \sa setPopup()
*/
TQPopupMenu* TQToolButton::popup() const
{
    return d->popup;
}

/*!
    Opens (pops up) the associated popup menu. If there is no such
    menu, this function does nothing. This function does not return
    until the popup menu has been closed by the user.
*/
void TQToolButton::openPopup()
{
    if ( !d->popup )
	return;

    d->instantPopup = true;
    repaint( false );
    if ( d->popupTimer )
	d->popupTimer->stop();
    TQGuardedPtr<TQToolButton> that = this;
    popupTimerDone();
    if ( !that )
	return;
    d->instantPopup = false;
    repaint( false );
}

void TQToolButton::popupPressed()
{
    if ( d->popupTimer && d->delay > 0 )
	d->popupTimer->start( d->delay, true );
}

void TQToolButton::popupTimerDone()
{
    if ( (!isDown() && d->delay > 0 ) || !d->popup )
	return;

    d->popup->installEventFilter( this );
    d->repeat = autoRepeat();
    setAutoRepeat( false );
    bool horizontal = true;
#ifndef TQT_NO_TOOLBAR
    TQToolBar *tb = ::tqt_cast<TQToolBar*>(parentWidget());
    if ( tb && tb->orientation() == Vertical )
	horizontal = false;
#endif
    TQPoint p;
    TQRect screen = tqApp->desktop()->availableGeometry( this );
    if ( horizontal ) {
	if ( TQApplication::reverseLayout() ) {
	    if ( mapToGlobal( TQPoint( 0, rect().bottom() ) ).y() + d->popup->sizeHint().height() <= screen.height() ) {
		p = mapToGlobal( rect().bottomRight() );
	    } else {
		p = mapToGlobal( rect().topRight() - TQPoint( 0, d->popup->sizeHint().height() ) );
	    }
	    p.rx() -= d->popup->sizeHint().width();
	} else {
	    if ( mapToGlobal( TQPoint( 0, rect().bottom() ) ).y() + d->popup->sizeHint().height() <= screen.height() ) {
		p = mapToGlobal( rect().bottomLeft() );
	    } else {
		p = mapToGlobal( rect().topLeft() - TQPoint( 0, d->popup->sizeHint().height() ) );
	    }
	}
    } else {
	if ( TQApplication::reverseLayout() ) {
	    if ( mapToGlobal( TQPoint( rect().left(), 0 ) ).x() - d->popup->sizeHint().width() <= screen.x() ) {
		p = mapToGlobal( rect().topRight() );
	    } else {
		p = mapToGlobal( rect().topLeft() );
		p.rx() -= d->popup->sizeHint().width();
	    }
	} else {
	    if ( mapToGlobal( TQPoint( rect().right(), 0 ) ).x() + d->popup->sizeHint().width() <= screen.width() ) {
		p = mapToGlobal( rect().topRight() );
	    } else {
		p = mapToGlobal( rect().topLeft() - TQPoint( d->popup->sizeHint().width(), 0 ) );
	    }
	}
    }
    TQGuardedPtr<TQToolButton> that = this;
    d->popup->exec( p, -1 );
    if ( !that )
	return;

    setDown( false );
    if ( d->repeat )
	setAutoRepeat( true );
}

/*!
    \property TQToolButton::popupDelay
    \brief the time delay between pressing the button and the appearance of the associated popup menu in milliseconds.

    Usually this is around half a second. A value of 0 draws the down
    arrow button to the side of the button which can be used to open
    up the popup menu.

    \sa setPopup()
*/
void TQToolButton::setPopupDelay( int delay )
{
    d->delay = delay;

    update();
}

int TQToolButton::popupDelay() const
{
    return d->delay;
}
#endif


/*!
    \property TQToolButton::autoRaise
    \brief whether auto-raising is enabled or not.

    The default is disabled (i.e. false).
*/
void TQToolButton::setAutoRaise( bool enable )
{
    d->autoraise = enable;

    update();
}

bool TQToolButton::autoRaise() const
{
    return d->autoraise;
}

/*!
    \property TQToolButton::textPosition
    \brief the position of the text label of this button.
*/

TQToolButton::TextPosition TQToolButton::textPosition() const
{
    return d->textPos;
}

void TQToolButton::setTextPosition( TextPosition pos )
{
    d->textPos = pos;
    updateGeometry();
    update();
}

/*! \reimp */

void TQToolButton::setText( const TQString &txt )
{
    TQButton::setText( txt );
    if ( !text().isEmpty() ) {
	delete s;
	s = 0;
    }
}

#ifndef TQT_NO_PALETTE
/*!
    \reimp
*/
void TQToolButton::paletteChange( const TQPalette & )
{
    if ( s )
	s->clearGenerated();
}
#endif

#endif
