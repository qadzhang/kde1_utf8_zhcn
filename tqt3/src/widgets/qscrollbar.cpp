/****************************************************************************
**
** Implementation of TQScrollBar class
**
** Created : 940427
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

#include "ntqscrollbar.h"
#ifndef TQT_NO_SCROLLBAR
#include "ntqpainter.h"
#include "ntqbitmap.h"
#include "ntqapplication.h"
#include "ntqtimer.h"
#include "ntqstyle.h"
#ifndef TQT_NO_CURSOR
#include <ntqcursor.h>
#endif
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif
#include <limits.h>

/*!
    \class TQScrollBar
    \brief The TQScrollBar widget provides a vertical or horizontal scroll bar.

    \ingroup basic

    A scroll bar allows the user to control a value within a
    program-definable range and gives users a visible indication of
    the current value of a \link TQRangeControl range control \endlink.

    Scroll bars include four separate controls:

    \list

    \i The \e line-up and \e line-down controls are little buttons
    which the user can use to move one "line" up or down. The meaning
    of line is configurable. In editors and list boxes it means one
    line of text; in an image viewer it might mean 20 pixels.

    \i The \e slider is the handle that indicates the current value of
    the scroll bar, which the user can drag to change the value. This
    part of the scroll bar is sometimes called the "thumb".

    \i The \e page-up/page-down control is the area on which the
    slider slides (the scroll bar's background). Clicking here moves
    the scroll bar towards the click. The meaning of "page" is also
    configurable: in editors and list boxes it means as many lines as
    there is space for in the widget.

    \endlist

    TQScrollBar has very few of its own functions; it mostly relies on
    TQRangeControl. The most useful functions are setValue() to set the
    scroll bar directly to some value; addPage(), addLine(),
    subtractPage(), and subtractLine() to simulate the effects of
    clicking (useful for accelerator keys); setSteps() to define the
    values of pageStep() and lineStep(); and setRange() to set the
    minValue() and maxValue() of the scroll bar. TQScrollBar has a
    convenience constructor with which you can set most of these
    properties.

    Some GUI styles (for example, the Windows and Motif styles
    provided with TQt), also use the pageStep() value to calculate the
    size of the slider.

    In addition to the access functions from TQRangeControl, TQScrollBar
    provides a comprehensive set of signals:
    \table
    \header \i Signal \i Emitted when
    \row \i \l valueChanged()
	 \i the scroll bar's value has changed. The tracking()
	    determines whether this signal is emitted during user
	    interaction.
    \row \i \l sliderPressed()
	 \i the user starts to drag the slider.
    \row \i \l sliderMoved()
	 \i the user drags the slider.
    \row \i \l sliderReleased()
	 \i the user releases the slider.
    \row \i \l nextLine()
	 \i the scroll bar has moved one line down or right. Line is
	    defined in TQRangeControl.
    \row \i \l prevLine()
	 \i the scroll bar has moved one line up or left.
    \row \i \l nextPage()
	 \i the scroll bar has moved one page down or right.
    \row \i \l prevPage()
	 \i the scroll bar has moved one page up or left.
    \endtable

    TQScrollBar only provides integer ranges. Note that although
    TQScrollBar handles very large numbers, scroll bars on current
    screens cannot usefully control ranges above about 100,000 pixels.
    Beyond that, it becomes difficult for the user to control the
    scroll bar using either the keyboard or the mouse.

    A scroll bar can be controlled by the keyboard, but it has a
    default focusPolicy() of \c NoFocus. Use setFocusPolicy() to
    enable keyboard focus. See keyPressEvent() for a list of key
    bindings.

    If you need to add scroll bars to an interface, consider using the
    TQScrollView class, which encapsulates the common uses for scroll
    bars.

    <img src=qscrbar-m.png> <img src=qscrbar-w.png>

    \sa TQSlider TQSpinBox TQScrollView
    \link guibooks.html#fowler GUI Design Handbook: Scroll Bar\endlink
*/


/*!
    \fn void TQScrollBar::valueChanged( int value )

    This signal is emitted when the scroll bar value has changed, with
    the new scroll bar \a value as an argument.
*/

/*!
    \fn void TQScrollBar::sliderPressed()

    This signal is emitted when the user presses the slider with the
    mouse.
*/

/*!
    \fn void TQScrollBar::sliderMoved( int value )

    This signal is emitted when the slider is dragged by the user, with
    the new scroll bar \a value as an argument.

    This signal is emitted even when tracking is turned off.

    \sa tracking() valueChanged() nextLine() prevLine() nextPage()
    prevPage()
*/

/*!
    \fn void TQScrollBar::sliderReleased()

    This signal is emitted when the user releases the slider with the
    mouse.
*/

/*!
    \fn void TQScrollBar::nextLine()

    This signal is emitted when the scroll bar scrolls one line down
    or right.
*/

/*!
    \fn void TQScrollBar::prevLine()

    This signal is emitted when the scroll bar scrolls one line up or
    left.
*/

/*!
    \fn void TQScrollBar::nextPage()

    This signal is emitted when the scroll bar scrolls one page down
    or right.
*/

/*!
    \fn void TQScrollBar::prevPage()

    This signal is emitted when the scroll bar scrolls one page up or
    left.
*/



static const int thresholdTime = 500;
static const int repeatTime	= 50;

#define HORIZONTAL	(orientation() == Horizontal)
#define VERTICAL	!HORIZONTAL
#define MOTIF_BORDER	2
#define SLIDER_MIN	9


/*!
    Constructs a vertical scroll bar.

    The \a parent and \a name arguments are sent on to the TQWidget
    constructor.

    The \c minValue defaults to 0, the \c maxValue to 99, with a \c
    lineStep size of 1 and a \c pageStep size of 10, and an initial
    \c value of 0.
*/

TQScrollBar::TQScrollBar( TQWidget *parent, const char *name )
    : TQWidget( parent, name ), orient( Vertical )
{
    init();
}

/*!
    Constructs a scroll bar.

    The \a orientation must be \c TQt::Vertical or \c TQt::Horizontal.

    The \a parent and \a name arguments are sent on to the TQWidget
    constructor.

    The \c minValue defaults to 0, the \c maxValue to 99, with a \c
    lineStep size of 1 and a \c pageStep size of 10, and an initial
    \c value of 0.
*/

TQScrollBar::TQScrollBar( Orientation orientation, TQWidget *parent,
			const char *name )
    : TQWidget( parent, name ), orient( orientation )
{
    init();
}

/*!
    Constructs a scroll bar whose value can never be smaller than \a
    minValue or greater than \a maxValue, whose line step size is \a
    lineStep and page step size is \a pageStep and whose value is
    initially \a value (which is guaranteed to be in range using
    bound()).

    If \a orientation is \c Vertical the scroll bar is vertical and if
    it is \c Horizontal the scroll bar is horizontal.

    The \a parent and \a name arguments are sent on to the TQWidget
    constructor.
*/

TQScrollBar::TQScrollBar( int minValue, int maxValue, int lineStep, int pageStep,
			int value,  Orientation orientation,
			TQWidget *parent, const char *name )
    : TQWidget( parent, name ),
      TQRangeControl( minValue, maxValue, lineStep, pageStep, value ),
      orient( orientation )
{
    init();
}

/*!
    Destructor.
*/
TQScrollBar::~TQScrollBar()
{
}

void TQScrollBar::init()
{
    track = true;
    sliderPos = 0;
    pressedControl = TQStyle::SC_None;
    clickedAt = false;
    setFocusPolicy( NoFocus );

    repeater = 0;

    setBackgroundMode((TQt::BackgroundMode)
		      style().styleHint(TQStyle::SH_ScrollBar_BackgroundMode));

    TQSizePolicy sp( TQSizePolicy::Minimum, TQSizePolicy::Fixed );
    if ( orient == Vertical )
	sp.transpose();
    setSizePolicy( sp );
    clearWState( WState_OwnSizePolicy );
}


/*!
    \property TQScrollBar::orientation
    \brief the orientation of the scroll bar

    The orientation must be \l TQt::Vertical (the default) or \l
    TQt::Horizontal.
*/

void TQScrollBar::setOrientation( Orientation orientation )
{
    if ( orientation == orient )
	return;
    if ( !testWState( WState_OwnSizePolicy ) ) {
	TQSizePolicy sp = sizePolicy();
	sp.transpose();
	setSizePolicy( sp );
	clearWState( WState_OwnSizePolicy );
    }

    orient = orientation;

    positionSliderFromValue();
    update();
    updateGeometry();
}

/*!
    \property TQScrollBar::tracking
    \brief whether scroll bar tracking is enabled

    If tracking is enabled (the default), the scroll bar emits the
    valueChanged() signal while the slider is being dragged. If
    tracking is disabled, the scroll bar emits the valueChanged()
    signal only when the user releases the mouse button after moving
    the slider.
*/


/*!
    \property TQScrollBar::draggingSlider
    \brief whether the user has clicked the mouse on the slider and is currently dragging it
*/

bool TQScrollBar::draggingSlider() const
{
    return pressedControl == TQStyle::SC_ScrollBarSlider;
}


/*!
    Reimplements the virtual function TQWidget::setPalette().

    Sets the background color to the mid color for Motif style scroll
    bars using palette \a p.
*/

void TQScrollBar::setPalette( const TQPalette &p )
{
    TQWidget::setPalette( p );
    setBackgroundMode((TQt::BackgroundMode)
		      style().styleHint(TQStyle::SH_ScrollBar_BackgroundMode));
}


/*! \reimp */
TQSize TQScrollBar::sizeHint() const
{
    constPolish();
    int sbextent = style().pixelMetric(TQStyle::PM_ScrollBarExtent, this);

    if ( orient == Horizontal ) {
	return TQSize( 30, sbextent );
    } else {
	return TQSize( sbextent, 30 );
    }
}

/*! \fn void TQScrollBar::setSizePolicy( TQSizePolicy::SizeType, TQSizePolicy::SizeType, bool )
    \reimp
*/

/*! \reimp */
void TQScrollBar::setSizePolicy( TQSizePolicy sp )
{
    //## remove 4.0
    TQWidget::setSizePolicy( sp );
}

/*!
  \internal
  Implements the virtual TQRangeControl function.
*/

void TQScrollBar::valueChange()
{
    int tmp = sliderPos;
    positionSliderFromValue();
    if ( tmp != sliderPos && isVisible() )
	drawControls(TQStyle::SC_ScrollBarAddPage |
		     TQStyle::SC_ScrollBarSubPage |
		     TQStyle::SC_ScrollBarSlider,
		     pressedControl );
    emit valueChanged(value());
#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::ValueChanged );
#endif
}

/*!
  \internal
  Implements the virtual TQRangeControl function.
*/

void TQScrollBar::stepChange()
{
    rangeChange();
}

/*!
  \internal
  Implements the virtual TQRangeControl function.
*/

void TQScrollBar::rangeChange()
{
    positionSliderFromValue();

    if ( isVisible() )
	drawControls(TQStyle::SC_ScrollBarAddLine |
		     TQStyle::SC_ScrollBarSubLine |
		     TQStyle::SC_ScrollBarAddPage |
		     TQStyle::SC_ScrollBarSubPage |
		     TQStyle::SC_ScrollBarFirst   |
		     TQStyle::SC_ScrollBarLast    |
		     TQStyle::SC_ScrollBarSlider,
		     pressedControl );
}


/*!
    Handles timer events for the scroll bar.
*/

void TQScrollBar::doAutoRepeat()
{
    bool sendRepeat = clickedAt;
#if !defined( TQT_NO_CURSOR ) && !defined( TQT_NO_STYLE )
    if(sendRepeat && (pressedControl == TQStyle::SC_ScrollBarAddPage ||
                      pressedControl == TQStyle::SC_ScrollBarSubPage) &&
       style().styleHint(TQStyle::SH_ScrollBar_StopMouseOverSlider, this) &&
       style().querySubControl(TQStyle::CC_ScrollBar, this,
                               mapFromGlobal(TQCursor::pos()) ) == TQStyle::SC_ScrollBarSlider)
        sendRepeat = false;
#endif
    if ( sendRepeat ){
        if ( repeater )
            repeater->changeInterval( repeatTime );
        action( (TQStyle::SubControl) pressedControl );
        TQApplication::syncX();
    } else {
        stopAutoRepeat();
    }
}


/*!
    Starts the auto-repeat logic. Some time after this function is
    called, the auto-repeat starts taking effect and from then on
    repeats until stopAutoRepeat() is called.
*/

void TQScrollBar::startAutoRepeat()
{
    if ( !repeater ) {
	repeater = new TQTimer( this, "auto-repeat timer" );
	connect( repeater, TQ_SIGNAL(timeout()),
		 this, TQ_SLOT(doAutoRepeat()) );
    }
    repeater->start( thresholdTime, false );
}


/*!
    Stops the auto-repeat logic.
*/

void TQScrollBar::stopAutoRepeat()
{
    delete repeater;
    repeater = 0;
}


/*!
    \reimp
*/
#ifndef TQT_NO_WHEELEVENT
void TQScrollBar::wheelEvent( TQWheelEvent *e )
{
    static float offset = 0;
    static TQScrollBar* offset_owner = 0;
    if (offset_owner != this){
	offset_owner = this;
	offset = 0;
    }
    if ( e->orientation() != orient && !rect().contains(e->pos()) )
	return;
    e->accept();
    int step = TQMIN( TQApplication::wheelScrollLines()*lineStep(),
		     pageStep() );
    if ( ( e->state() & ControlButton ) || ( e->state() & ShiftButton ) )
	step = pageStep();
    offset += -e->delta()*step/120;
    if (TQABS(offset)<1)
	return;
    setValue( value() + int(offset) );
    offset -= int(offset);
}
#endif

/*!
    \reimp
*/
void TQScrollBar::keyPressEvent( TQKeyEvent *e )
{
    //   \list
    //     \i Left/Right move a horizontal scrollbar by one line.
    //     \i Up/Down move a vertical scrollbar by one line.
    //     \i PageUp moves up one page.
    //     \i PageDown moves down one page.
    //     \i Home moves to the start (minValue()).
    //     \i End moves to the end (maxValue()).
    //   \endlist

    //   Note that unless you call setFocusPolicy(), the default NoFocus
    //   will apply and the user will not be able to use the keyboard to
    //   interact with the scrollbar.
    switch ( e->key() ) {
    case Key_Left:
	if ( orient == Horizontal )
	    subtractLine();
	break;
    case Key_Right:
	if ( orient == Horizontal )
	    addLine();
	break;
    case Key_Up:
	if ( orient == Vertical )
	    subtractLine();
	break;
    case Key_Down:
	if ( orient == Vertical )
	    addLine();
	break;
    case Key_PageUp:
	subtractPage();
	break;
    case Key_PageDown:
	addPage();
	break;
    case Key_Home:
	setValue( minValue() );
	break;
    case Key_End:
	setValue( maxValue() );
	break;
    default:
	e->ignore();
	break;
    }
}


/*!
    \reimp
*/
void TQScrollBar::resizeEvent( TQResizeEvent * )
{
    positionSliderFromValue();
}


/*!
    \reimp
*/
void TQScrollBar::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );
    drawControls(TQStyle::SC_ScrollBarAddLine |
		 TQStyle::SC_ScrollBarSubLine |
		 TQStyle::SC_ScrollBarAddPage |
		 TQStyle::SC_ScrollBarSubPage |
		 TQStyle::SC_ScrollBarFirst   |
		 TQStyle::SC_ScrollBarLast    |
		 TQStyle::SC_ScrollBarSlider,
		 pressedControl, &p );
}

static TQCOORD sliderStartPos = 0;

/*!
    \reimp
 */
void TQScrollBar::contextMenuEvent( TQContextMenuEvent *e )
{
    if(clickedAt)
	e->consume();
    else
	e->ignore();
}

/*!
    \reimp
*/
void TQScrollBar::mousePressEvent( TQMouseEvent *e )
{
    bool midButtonAbsPos =
	style().styleHint(TQStyle::SH_ScrollBar_MiddleClickAbsolutePosition,
			  this);

    if ( !(e->button() == LeftButton ||
	   (midButtonAbsPos && e->button() == MidButton) ) )
	return;

    if ( maxValue() == minValue() ) // nothing to be done
	return;

    if ( e->state() & MouseButtonMask ) // another button was already pressed
	return;

    clickedAt	   = true;
    pressedControl = style().querySubControl(TQStyle::CC_ScrollBar, this, e->pos() );

    if ( (pressedControl == TQStyle::SC_ScrollBarAddPage ||
	  pressedControl == TQStyle::SC_ScrollBarSubPage ||
	  pressedControl == TQStyle::SC_ScrollBarSlider ) &&
	 ((midButtonAbsPos && e->button() == MidButton) ||
	  (style().styleHint(TQStyle::SH_ScrollBar_LeftClickAbsolutePosition) && e->button() == LeftButton)) ) {

	TQRect sr = style().querySubControlMetrics(TQStyle::CC_ScrollBar, this,
						  TQStyle::SC_ScrollBarSlider ),
	      gr = style().querySubControlMetrics(TQStyle::CC_ScrollBar, this,
						  TQStyle::SC_ScrollBarGroove );
	int sliderMin, sliderMax, sliderLength;
	sliderMin = sliderMax = sliderLength = 0;
	if (HORIZONTAL) {
	    sliderMin = gr.x();
	    sliderMax = sliderMin + gr.width();
	    sliderLength = sr.width();
	} else {
	    sliderMin = gr.y();
	    sliderMax = sliderMin + gr.height();
	    sliderLength = sr.height();
	}

	int newSliderPos = (HORIZONTAL ? e->pos().x() : e->pos().y())
			   - sliderLength/2;
	newSliderPos = TQMIN( newSliderPos, sliderMax - sliderLength );
	newSliderPos = TQMAX( newSliderPos, sliderMin );
	setValue( sliderPosToRangeValue(newSliderPos) );
	sliderPos = newSliderPos;
	pressedControl = TQStyle::SC_ScrollBarSlider;
    }

    if ( pressedControl == TQStyle::SC_ScrollBarSlider ) {
	clickOffset = (TQCOORD)( (HORIZONTAL ? e->pos().x() : e->pos().y())
				- sliderPos );
	slidePrevVal   = value();
	sliderStartPos = sliderPos;
	drawControls( pressedControl, pressedControl );
	emit sliderPressed();
#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( this, 0, TQAccessible::ScrollingStart );
#endif
    } else if ( pressedControl != TQStyle::SC_None ) {
	drawControls( pressedControl, pressedControl );
	action( (TQStyle::SubControl) pressedControl );
	startAutoRepeat();
    }
}


/*!
    \reimp
*/
void TQScrollBar::mouseReleaseEvent( TQMouseEvent *e )
{
    if ( !clickedAt )
	return;

    if ( e->stateAfter() & MouseButtonMask ) // some other button is still pressed
	return;

    TQStyle::SubControl tmp = (TQStyle::SubControl) pressedControl;
    clickedAt = false;
    stopAutoRepeat();
    mouseMoveEvent( e );  // Might have moved since last mouse move event.
    pressedControl = TQStyle::SC_None;

    if (tmp == TQStyle::SC_ScrollBarSlider) {
	directSetValue( calculateValueFromSlider() );
	emit sliderReleased();
	if ( value() != prevValue() ) {
	    emit valueChanged( value() );
#if defined(QT_ACCESSIBILITY_SUPPORT)
	    TQAccessible::updateAccessibility( this, 0, TQAccessible::ValueChanged );
#endif
	}
#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( this, 0, TQAccessible::ScrollingEnd );
#endif
    }
    drawControls( tmp, pressedControl );
    if ( e->button() == MidButton )
	repaint( false );
}


/*!
    \reimp
*/
void TQScrollBar::mouseMoveEvent( TQMouseEvent *e )
{
    if ( !isVisible() ) {
	clickedAt = false;
	return;
    }

    bool mcab = style().styleHint(TQStyle::SH_ScrollBar_MiddleClickAbsolutePosition,
				  this);
    if ( ! clickedAt || ! (e->state() & LeftButton ||
			   ((e->state() & MidButton) && mcab)))
	return;

    int newSliderPos;
    if ( pressedControl == TQStyle::SC_ScrollBarSlider ) {
	TQRect gr = style().querySubControlMetrics(TQStyle::CC_ScrollBar, this,
						  TQStyle::SC_ScrollBarGroove ),
	      sr = style().querySubControlMetrics(TQStyle::CC_ScrollBar, this,
						  TQStyle::SC_ScrollBarSlider );
	int sliderMin, sliderMax, sliderLength;

	if (HORIZONTAL) {
	    sliderLength = sr.width();
	    sliderMin = gr.x();
	    sliderMax = gr.right() - sliderLength + 1;
	} else {
	    sliderLength = sr.height();
	    sliderMin = gr.y();
	    sliderMax = gr.bottom() - sliderLength + 1;
	}

	TQRect r = rect();
	int m = style().pixelMetric(TQStyle::PM_MaximumDragDistance, this);
	if ( m >= 0 ) {
	    if ( orientation() == Horizontal )
		r.setRect( r.x() - m, r.y() - 2*m, r.width() + 2*m, r.height() + 4*m );
	    else
		r.setRect( r.x() - 2*m, r.y() - m, r.width() + 4*m, r.height() + 2*m );
	    if (! r.contains( e->pos()))
		newSliderPos = sliderStartPos;
	    else
		newSliderPos = (HORIZONTAL ? e->pos().x() :
				e->pos().y()) -clickOffset;
	} else
	    newSliderPos = (HORIZONTAL ? e->pos().x() :
			    e->pos().y()) -clickOffset;

	if ( newSliderPos < sliderMin )
	    newSliderPos = sliderMin;
	else if ( newSliderPos > sliderMax )
	    newSliderPos = sliderMax;
	int newVal = sliderPosToRangeValue(newSliderPos);
	if ( newVal != slidePrevVal )
	    emit sliderMoved( newVal );
	if ( track && newVal != value() ) {
	    directSetValue( newVal ); // Set directly, painting done below
	    emit valueChanged( value() );
#if defined(QT_ACCESSIBILITY_SUPPORT)
	    TQAccessible::updateAccessibility( this, 0, TQAccessible::ValueChanged );
#endif
	}
	slidePrevVal = newVal;
	sliderPos = (TQCOORD)newSliderPos;
	drawControls( TQStyle::SC_ScrollBarAddPage |
		      TQStyle::SC_ScrollBarSlider  |
		      TQStyle::SC_ScrollBarSubPage,
		      pressedControl );
    } else if (! style().styleHint(TQStyle::SH_ScrollBar_ScrollWhenPointerLeavesControl)) {
	// stop scrolling when the mouse pointer leaves a control
	// similar to push buttons
	if ( pressedControl != (uint)style().querySubControl(TQStyle::CC_ScrollBar, this, e->pos() ) ) {
	    drawControls( pressedControl, TQStyle::SC_None );
	    stopAutoRepeat();
	} else if ( !repeater ) {
	    drawControls( pressedControl, pressedControl );
	    action( (TQStyle::SubControl) pressedControl );
	    startAutoRepeat();
	}
    }
}


/*!
    \fn int TQScrollBar::sliderStart() const

    Returns the pixel position where the scroll bar slider starts.

    This is equivalent to sliderRect().y() for vertical scroll bars or
    sliderRect().x() for horizontal scroll bars.
*/

/*!
    Returns the scroll bar slider rectangle.

    \sa sliderStart()
*/

TQRect TQScrollBar::sliderRect() const
{
    return style().querySubControlMetrics(TQStyle::CC_ScrollBar, this,
					  TQStyle::SC_ScrollBarSlider );
}

void TQScrollBar::positionSliderFromValue()
{
    sliderPos = (TQCOORD)rangeValueToSliderPos( value() );
}

int TQScrollBar::calculateValueFromSlider() const
{
    return sliderPosToRangeValue( sliderPos );
}

int TQScrollBar::rangeValueToSliderPos( int v ) const
{
    TQRect gr = style().querySubControlMetrics(TQStyle::CC_ScrollBar, this,
					      TQStyle::SC_ScrollBarGroove );
    TQRect sr = style().querySubControlMetrics(TQStyle::CC_ScrollBar, this,
					      TQStyle::SC_ScrollBarSlider );
    int sliderMin, sliderMax, sliderLength;

    if (HORIZONTAL) {
	sliderLength = sr.width();
	sliderMin = gr.x();
	sliderMax = gr.right() - sliderLength + 1;
    } else {
	sliderLength = sr.height();
	sliderMin = gr.y();
	sliderMax = gr.bottom() - sliderLength + 1;
    }

    return positionFromValue( v, sliderMax-sliderMin ) + sliderMin;
}

int TQScrollBar::sliderPosToRangeValue( int pos ) const
{
    TQRect gr = style().querySubControlMetrics(TQStyle::CC_ScrollBar, this,
					      TQStyle::SC_ScrollBarGroove );
    TQRect sr = style().querySubControlMetrics(TQStyle::CC_ScrollBar, this,
					      TQStyle::SC_ScrollBarSlider );
    int sliderMin, sliderMax, sliderLength;

    if (HORIZONTAL) {
	sliderLength = sr.width();
	sliderMin = gr.x();
	sliderMax = gr.right() - sliderLength + 1;
    } else {
	sliderLength = sr.height();
	sliderMin = gr.y();
	sliderMax = gr.bottom() - sliderLength + 1;
    }

    return  valueFromPosition( pos - sliderMin, sliderMax - sliderMin );
}


void TQScrollBar::action( int control )
{
    switch( control ) {
    case TQStyle::SC_ScrollBarAddLine:
	addLine();
	emit nextLine();
	break;
    case TQStyle::SC_ScrollBarSubLine:
	subtractLine();
	emit prevLine();
	break;
    case TQStyle::SC_ScrollBarAddPage:
	addPage();
	emit nextPage();
	break;
    case TQStyle::SC_ScrollBarSubPage:
	subtractPage();
	emit prevPage();
	break;
    case TQStyle::SC_ScrollBarFirst:
	setValue( minValue() );
#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( this, 0, TQAccessible::ValueChanged );
#endif
	emit valueChanged( minValue() );
	break;
    case TQStyle::SC_ScrollBarLast:
	setValue( maxValue() );
#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( this, 0, TQAccessible::ValueChanged );
#endif
	emit valueChanged( maxValue() );
	break;
    default:
	break;
    }
}


void TQScrollBar::drawControls( uint controls, uint activeControl ) const
{
    TQPainter p ( this );
    drawControls( controls, activeControl, &p );
}


void TQScrollBar::drawControls( uint controls, uint activeControl,
			       TQPainter *p ) const
{
    if ( !isUpdatesEnabled() )
	return;

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled())
	flags |= TQStyle::Style_Enabled;
    if (hasFocus())
	flags |= TQStyle::Style_HasFocus;
    if (hasMouse())
	flags |= TQStyle::Style_MouseOver;
    if ( orientation() == Horizontal )
	flags |= TQStyle::Style_Horizontal;

    style().drawComplexControl(TQStyle::CC_ScrollBar, p, this, rect(), colorGroup(),
			       flags, (TQStyle::SubControl) controls,
			       (TQStyle::SubControl) activeControl );
}

/*!
    \reimp
*/
void TQScrollBar::styleChange( TQStyle& old )
{
    positionSliderFromValue();
    setBackgroundMode((TQt::BackgroundMode)
		      style().styleHint(TQStyle::SH_ScrollBar_BackgroundMode));
    TQWidget::styleChange( old );
}

/*!
    \property TQScrollBar::minValue
    \brief the scroll bar's minimum value

    When setting this property, the \l TQScrollBar::maxValue is
    adjusted if necessary to ensure that the range remains valid.

    \sa setRange()
*/
int TQScrollBar::minValue() const
{
    return TQRangeControl::minValue();
}

void TQScrollBar::setMinValue( int minVal )
{
    TQRangeControl::setMinValue( minVal );
}

/*!
    \property TQScrollBar::maxValue
    \brief the scroll bar's maximum value

    When setting this property, the \l TQScrollBar::minValue is
    adjusted if necessary to ensure that the range remains valid.

    \sa setRange()
*/
int TQScrollBar::maxValue() const
{
    return TQRangeControl::maxValue();
}

void TQScrollBar::setMaxValue( int maxVal )
{
    TQRangeControl::setMaxValue( maxVal );
}

/*!
    \property TQScrollBar::lineStep
    \brief the line step

    When setting lineStep, the virtual stepChange() function will be
    called if the new line step is different from the previous
    setting.

    \sa setSteps() TQRangeControl::pageStep() setRange()
*/

int TQScrollBar::lineStep() const
{
    return TQRangeControl::lineStep();
}

/*!
    \property TQScrollBar::pageStep
    \brief the page step

    When setting pageStep, the virtual stepChange() function will be
    called if the new page step is different from the previous
    setting.

    \sa TQRangeControl::setSteps() setLineStep() setRange()
*/

int TQScrollBar::pageStep() const
{
    return TQRangeControl::pageStep();
}

void TQScrollBar::setLineStep( int i )
{
    setSteps( i, pageStep() );
}

void TQScrollBar::setPageStep( int i )
{
    setSteps( lineStep(), i );
}

/*!
    \property TQScrollBar::value
    \brief the scroll bar's value

    \sa TQRangeControl::value() prevValue()
*/

int TQScrollBar::value() const
{
    return TQRangeControl::value();
}

void TQScrollBar::setValue( int i )
{
    TQRangeControl::setValue( i );
}


/*!
    This function is called when the scrollbar is hidden.
*/
void TQScrollBar::hideEvent( TQHideEvent* )
{
    pressedControl = TQStyle::SC_None;
    clickedAt = false;
}


#undef ADD_LINE_ACTIVE
#undef SUB_LINE_ACTIVE
#endif
