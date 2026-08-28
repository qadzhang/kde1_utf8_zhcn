/****************************************************************************
**
** Implementation of TQSlider class
**
** Created : 961019
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

#include "ntqslider.h"
#ifndef TQT_NO_SLIDER
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqtimer.h"
#include "ntqbitmap.h"
#include "ntqapplication.h"
#include "ntqstyle.h"
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif

static const int thresholdTime = 300;
static const int repeatTime = 100;

struct TQSliderPrivate
{
    // ### move these to TQSlider in TQt 4.0
    int sliderStartVal;
    TQSliderPrivate() : sliderStartVal( 0 ) { }
};


/*!
    \class TQSlider
    \brief The TQSlider widget provides a vertical or horizontal slider.

    \ingroup basic
    \mainclass

    The slider is the classic widget for controlling a bounded value.
    It lets the user move a slider along a horizontal or vertical
    groove and translates the slider's position into an integer value
    within the legal range.

    TQSlider inherits TQRangeControl, which provides the "integer" side
    of the slider. setRange() and value() are likely to be used by
    practically all slider users; see the \l TQRangeControl
    documentation for information about the many other functions that
    class provides.

    The main functions offered by the slider itself are tickmark and
    orientation control; you can use setTickmarks() to indicate where
    you want the tickmarks to be, setTickInterval() to indicate how
    many of them you want and setOrientation() to indicate whether the
    slider is to be horizontal or vertical.

    A slider accepts focus on Tab and uses the mouse wheel and a
    suitable keyboard interface.

    <img src=qslider-m.png> <img src=qslider-w.png>

    \important setRange

    \sa TQScrollBar TQSpinBox
    \link guibooks.html#fowler GUI Design Handbook: Slider\endlink
*/


/*!
    \enum TQSlider::TickSetting

    This enum specifies where the tickmarks are to be drawn relative
    to the slider's groove and the handle the user moves.

    \value NoMarks do not draw any tickmarks.
    \value Both draw tickmarks on both sides of the groove.
    \value Above draw tickmarks above the (horizontal) slider
    \value Below draw tickmarks below the (horizontal) slider
    \value Left draw tickmarks to the left of the (vertical) slider
    \value Right draw tickmarks to the right of the (vertical) slider
*/


/*!
    Constructs a vertical slider.

    The \a parent and \a name arguments are sent on to the TQWidget
    constructor.
*/

TQSlider::TQSlider( TQWidget *parent, const char *name )
    : TQWidget( parent, name  )
{
    orient = Vertical;
    init();
}

/*!
    Constructs a slider.

    The \a orientation must be \l TQt::Vertical or \l TQt::Horizontal.

    The \a parent and \a name arguments are sent on to the TQWidget
    constructor.
*/

TQSlider::TQSlider( Orientation orientation, TQWidget *parent, const char *name )
    : TQWidget( parent, name )
{
    orient = orientation;
    init();
}

/*!
    Constructs a slider whose value can never be smaller than \a
    minValue or greater than \a maxValue, whose page step size is \a
    pageStep and whose value is initially \a value (which is
    guaranteed to be in range using bound()).

    If \a orientation is \c TQt::Vertical the slider is vertical and if it
    is \c TQt::Horizontal the slider is horizontal.

    The \a parent and \a name arguments are sent on to the TQWidget
    constructor.
*/

TQSlider::TQSlider( int minValue, int maxValue, int pageStep,
		  int value, Orientation orientation,
		  TQWidget *parent, const char *name )
    : TQWidget( parent, name ),
      TQRangeControl( minValue, maxValue, 1, pageStep, value )
{
    orient = orientation;
    init();
    sliderVal = value;
}

/*!
    Destructor.
*/
TQSlider::~TQSlider()
{
    delete d;
}

void TQSlider::init()
{
    d = new TQSliderPrivate;
    timer = 0;
    sliderPos = 0;
    sliderVal = 0;
    clickOffset = 0;
    state = Idle;
    track = true;
    ticks = NoMarks;
    tickInt = 0;
    setFocusPolicy( TabFocus  );
    initTicks();

    TQSizePolicy sp( TQSizePolicy::Expanding, TQSizePolicy::Fixed );
    if ( orient == Vertical )
	sp.transpose();
    setSizePolicy( sp );
    clearWState( WState_OwnSizePolicy );
}


/*
    Does what's needed when someone changes the tickmark status.
*/

void TQSlider::initTicks()
{
    tickOffset = style().pixelMetric( TQStyle::PM_SliderTickmarkOffset, this );
}


/*!
    \property TQSlider::tracking
    \brief whether slider tracking is enabled

    If tracking is enabled (the default), the slider emits the
    valueChanged() signal whenever the slider is being dragged. If
    tracking is disabled, the slider emits the valueChanged() signal
    when the user releases the mouse button (unless the value happens
    to be the same as before).
*/

void TQSlider::setTracking( bool enable )
{
    track = enable;
}


/*!
    \fn void TQSlider::valueChanged( int value )

    This signal is emitted when the slider value is changed, with the
    new slider \a value as its argument.
*/

/*!
    \fn void TQSlider::sliderPressed()

    This signal is emitted when the user presses the slider with the
    mouse.
*/

/*!
    \fn void TQSlider::sliderMoved( int value )

    This signal is emitted when the slider is dragged, with the new
    slider \a value as its argument.
*/

/*!
    \fn void TQSlider::sliderReleased()

    This signal is emitted when the user releases the slider with the mouse.
*/

/*
    Calculates slider position corresponding to value \a v.
*/

int TQSlider::positionFromValue( int v ) const
{
    int  a = available();
    int x = TQRangeControl::positionFromValue( v, a );
    if ( orient == Horizontal && TQApplication::reverseLayout() )
	x = a - x;
    return x;
}

/*
    Returns the available space in which the slider can move.
*/

int TQSlider::available() const
{
    return style().pixelMetric( TQStyle::PM_SliderSpaceAvailable, this );
}

/*
    Calculates a value corresponding to slider position \a p.
*/

int TQSlider::valueFromPosition( int p ) const
{
    int a = available();
    int x = TQRangeControl::valueFromPosition( p, a );
    if ( orient == Horizontal && TQApplication::reverseLayout() )
	x = maxValue() + minValue() - x;
    return x;
}

/*!
    Implements the virtual TQRangeControl function.
*/

void TQSlider::rangeChange()
{
    int newPos = positionFromValue( value() );
    if ( newPos != sliderPos ) {
	reallyMoveSlider( newPos );
    }
}

/*!
    Implements the virtual TQRangeControl function.
*/

void TQSlider::valueChange()
{
    if ( sliderVal != value() ) {
	int newPos = positionFromValue( value() );
	sliderVal = value();
	reallyMoveSlider( newPos );
    }
    emit valueChanged(value());
#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::ValueChanged );
#endif
}


/*!
    \reimp
*/
void TQSlider::resizeEvent( TQResizeEvent * )
{
    rangeChange();
    initTicks();
}


/*!
    Reimplements the virtual function TQWidget::setPalette().

    Sets the background color to the mid color for Motif style sliders
    using palette \a p.
*/

void TQSlider::setPalette( const TQPalette &p )
{
    TQWidget::setPalette( p );
}



/*!
    \property TQSlider::orientation
    \brief the slider's orientation

    The orientation must be \l TQt::Vertical (the default) or \l
    TQt::Horizontal.
*/

void TQSlider::setOrientation( Orientation orientation )
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

    rangeChange();
    update();
}

/*!
    \fn int TQSlider::sliderStart() const

    Returns the start position of the slider.
*/


/*!
    Returns the slider handle rectangle. (This is the visual marker
    that the user can move.)
*/

TQRect TQSlider::sliderRect() const
{
    return style().querySubControlMetrics( TQStyle::CC_Slider, this,
					   TQStyle::SC_SliderHandle );
}

/*
    Performs the actual moving of the slider.
*/

void TQSlider::reallyMoveSlider( int newPos )
{
    TQRegion oldR(sliderRect());
    sliderPos = newPos;
    TQRegion newR(sliderRect());

    /* just the one repaint if no background */
    if (backgroundMode() == NoBackground)
	repaint(newR | oldR, false);
    else {
	repaint(oldR.subtract(newR));
	repaint(newR, false);
    }
}


/*!
    \reimp
*/
void TQSlider::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled())
	flags |= TQStyle::Style_Enabled;
    if (hasFocus())
	flags |= TQStyle::Style_HasFocus;
    if (hasMouse())
	flags |= TQStyle::Style_MouseOver;

    TQStyle::SCFlags sub = TQStyle::SC_SliderGroove | TQStyle::SC_SliderHandle;
    if ( tickmarks() != NoMarks ) {
	sub |= TQStyle::SC_SliderTickmarks;
    }

    style().drawComplexControl( TQStyle::CC_Slider, &p, this, rect(), colorGroup(),
				flags, sub, state == Dragging ? TQStyle::SC_SliderHandle : TQStyle::SC_None );
}


/*!
    \reimp
*/
void TQSlider::mousePressEvent( TQMouseEvent *e )
{
    int slideLength = style().pixelMetric( TQStyle::PM_SliderLength, this );
    resetState();
    d->sliderStartVal = sliderVal;
    TQRect r = sliderRect();

    if ( e->button() == RightButton )
	return;

    if ( r.contains( e->pos() ) ) {
	state = Dragging;
	clickOffset = (TQCOORD)( goodPart( e->pos() ) - sliderPos );
	emit sliderPressed();
    } else if ( e->button() == MidButton ) {
	int pos = goodPart( e->pos() );
	moveSlider( pos - slideLength / 2 );
	state = Dragging;
	clickOffset = slideLength / 2;
    } else if ( ( orient == Horizontal && e->pos().x() < r.left() ) //### goodPart
		|| ( orient == Vertical && e->pos().y() < r.top() ) ) {
	if ( orient == Horizontal && TQApplication::reverseLayout() ) {
	    state = TimingUp;
	    addPage();
	} else {
	    state = TimingDown;
	    subtractPage();
	}
	if ( !timer )
	    timer = new TQTimer( this );
	connect( timer, TQ_SIGNAL(timeout()), TQ_SLOT(repeatTimeout()) );
	timer->start( thresholdTime, true );
    } else if ( ( orient == Horizontal && e->pos().x() > r.right() ) //### goodPart
		|| ( orient == Vertical && e->pos().y() > r.bottom() ) ) {
	if ( orient == Horizontal && TQApplication::reverseLayout() ) {
	    state = TimingDown;
	    subtractPage();
	} else {
	    state = TimingUp;
	    addPage();
	}
	if ( !timer )
	    timer = new TQTimer( this );
	connect( timer, TQ_SIGNAL(timeout()), TQ_SLOT(repeatTimeout()) );
	timer->start( thresholdTime, true );
    }
    update( sliderRect() );
}

/*!
    \reimp
*/
void TQSlider::mouseMoveEvent( TQMouseEvent *e )
{
    if ( state != Dragging )
	return;

    TQRect r = rect();
    int m = style().pixelMetric( TQStyle::PM_MaximumDragDistance,
				 this );
    if ( m >= 0 ) {
	if ( orientation() == Horizontal )
	    r.setRect( r.x() - m, r.y() - 2*m/3,
		       r.width() + 2*m, r.height() + 3*m );
	else
	    r.setRect( r.x() - 2*m/3, r.y() - m,
		       r.width() + 3*m, r.height() + 2*m );
	if ( !r.contains( e->pos() ) ) {
	    moveSlider( positionFromValue(d->sliderStartVal) );
	    return;
	}
    }

    int pos = goodPart( e->pos() );
    moveSlider( pos - clickOffset );
}

/*!
    \reimp
*/
#ifndef TQT_NO_WHEELEVENT
void TQSlider::wheelEvent( TQWheelEvent * e )
{
    if ( e->orientation() != orientation() && !rect().contains(e->pos()) )
	return;

    static float offset = 0;
    static TQSlider* offset_owner = 0;
    if (offset_owner != this){
	offset_owner = this;
	offset = 0;
    }
    offset += -e->delta()*TQMAX(pageStep(),lineStep())/120;
    if (TQABS(offset)<1)
	return;
    setValue( value() + int(offset) );
    offset -= int(offset);
    e->accept();
}
#endif

/*!
    \reimp
*/
void TQSlider::mouseReleaseEvent( TQMouseEvent * )
{
    resetState();
    update( sliderRect() );
}

/*!
    \reimp
*/
void TQSlider::focusInEvent( TQFocusEvent * e)
{
    TQWidget::focusInEvent( e );
}

/*!
    \reimp
*/
void TQSlider::focusOutEvent( TQFocusEvent * e )
{
    TQWidget::focusOutEvent( e );
}

/*!
    Moves the left (or top) edge of the slider to position \a pos. The
    slider is actually moved to the step position nearest the given \a
    pos.
*/

void TQSlider::moveSlider( int pos )
{
    int  a = available();
    int newPos = TQMIN( a, TQMAX( 0, pos ) );
    int newVal = valueFromPosition( newPos );
    if (style().styleHint(TQStyle::SH_Slider_SnapToValue, this))
	newPos = positionFromValue( newVal );
    if ( sliderPos != newPos )
	reallyMoveSlider( newPos );
    if ( sliderVal != newVal ) {
	sliderVal = newVal;
	emit sliderMoved( sliderVal );
    }
    if ( tracking() && sliderVal != value() )
	setValue( sliderVal );

}


/*
    Resets all state information and stops the timer.
*/

void TQSlider::resetState()
{
    if ( timer ) {
	timer->stop();
	timer->disconnect();
    }
    switch ( state ) {
    case TimingUp:
    case TimingDown:
	break;
    case Dragging: {
	setValue( valueFromPosition( sliderPos ) );
	emit sliderReleased();
	break;
    }
    case Idle:
	break;
    default:
	tqWarning("TQSlider: (%s) in wrong state", name( "unnamed" ) );
    }
    state = Idle;
}


/*!
    \reimp
*/
void TQSlider::keyPressEvent( TQKeyEvent *e )
{
    bool sloppy = bool(style().styleHint(TQStyle::SH_Slider_SloppyKeyEvents, this));
    switch ( e->key() ) {
    case Key_Left:
	if ( sloppy || orient == Horizontal ) {
	    if (TQApplication::reverseLayout())
		addLine();
	    else
		subtractLine();
	}
	break;
    case Key_Right:
	if ( sloppy || orient == Horizontal ) {
	    if (TQApplication::reverseLayout())
		subtractLine();
	    else
		addLine();
	}
	break;
    case Key_Up:
	if ( sloppy || orient == Vertical )
	    subtractLine();
	break;
    case Key_Down:
	if ( sloppy || orient == Vertical )
	    addLine();
	break;
    case Key_Prior:
	subtractPage();
	break;
    case Key_Next:
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
	return;
    }
}

void TQSlider::setValue( int value )
{
    TQRangeControl::setValue( value );
#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::ValueChanged );
#endif
}


/*! \reimp
*/

void TQSlider::addLine()
{
    TQRangeControl::addLine();
}

/*! \reimp
*/

void TQSlider::subtractLine()
{
    TQRangeControl::subtractLine();
}

/*!
    Moves the slider one pageStep() up or right.
*/

void TQSlider::addStep()
{
    addPage();
}


/*!
    Moves the slider one pageStep() down or left.
*/

void TQSlider::subtractStep()
{
    subtractPage();
}


/*
  Waits for autorepeat.
*/

void TQSlider::repeatTimeout()
{
    Q_ASSERT( timer );
    timer->disconnect();
    if ( state == TimingDown )
	connect( timer, TQ_SIGNAL(timeout()), TQ_SLOT(subtractStep()) );
    else if ( state == TimingUp )
	connect( timer, TQ_SIGNAL(timeout()), TQ_SLOT(addStep()) );
    timer->start( repeatTime, false );
}


/*
  Returns the relevant dimension of \a p.
*/

int TQSlider::goodPart( const TQPoint &p ) const
{
    return (orient == Horizontal) ?  p.x() : p.y();
}

/*!
    \reimp
*/
TQSize TQSlider::sizeHint() const
{
    constPolish();
    const int length = 84, tickSpace = 5;
    int thick = style().pixelMetric( TQStyle::PM_SliderThickness, this );
    if ( ticks & Above )
	thick += tickSpace;
    if ( ticks & Below )
	thick += tickSpace;
    int w = thick, h = length;
    if ( orient == Horizontal ) {
	w = length;
	h = thick;
    }
    return (style().sizeFromContents(TQStyle::CT_Slider, this,
				     TQSize(w, h)).expandedTo(TQApplication::globalStrut()));
}



/*!
    \reimp
*/

TQSize TQSlider::minimumSizeHint() const
{
    TQSize s = sizeHint();
    int length = style().pixelMetric(TQStyle::PM_SliderLength, this);
    if ( orient == Horizontal )
	s.setWidth( length );
    else
	s.setHeight( length );

    return s;
}

/*! \fn void TQSlider::setSizePolicy( TQSizePolicy::SizeType, TQSizePolicy::SizeType, bool )
    \reimp
*/

/*! \reimp */
void TQSlider::setSizePolicy( TQSizePolicy sp )
{
    // ## remove 4.0
    TQWidget::setSizePolicy( sp );
}

/*! \reimp */
TQSizePolicy TQSlider::sizePolicy() const
{
    // ### 4.0 remove this reimplementation
    return TQWidget::sizePolicy();
}

/*!
    \property TQSlider::tickmarks
    \brief the tickmark settings for this slider

    The valid values are in \l{TQSlider::TickSetting}. The default is
    \c NoMarks.

    \sa tickInterval
*/

void TQSlider::setTickmarks( TickSetting s )
{
    ticks = s;
    initTicks();
    update();
}


/*!
    \property TQSlider::tickInterval
    \brief the interval between tickmarks

    This is a value interval, not a pixel interval. If it is 0, the
    slider will choose between lineStep() and pageStep(). The initial
    value of tickInterval is 0.

    \sa TQRangeControl::lineStep(), TQRangeControl::pageStep()
*/

void TQSlider::setTickInterval( int i )
{
    tickInt = TQMAX( 0, i );
    update();
}


/*!
    \reimp
*/
void TQSlider::styleChange( TQStyle& old )
{
    TQWidget::styleChange( old );
}

/*!
    \property TQSlider::minValue
    \brief the current minimum value of the slider

    When setting this property, the \l TQSlider::maxValue is adjusted,
    if necessary, to ensure that the range remains valid.

    \sa setRange()
*/
int TQSlider::minValue() const
{
    return TQRangeControl::minValue();
}

/*!
    \property TQSlider::maxValue
    \brief the current maximum value of the slider

    When setting this property, the \l TQSlider::minValue is adjusted,
    if necessary, to ensure that the range remains valid.

    \sa setRange()
*/
int TQSlider::maxValue() const
{
    return TQRangeControl::maxValue();
}

void TQSlider::setMinValue( int minVal )
{
    TQRangeControl::setMinValue( minVal );
}

void TQSlider::setMaxValue( int maxVal )
{
    TQRangeControl::setMaxValue( maxVal );
}

/*!
    \property TQSlider::lineStep
    \brief the current line step

    When setting lineStep, the virtual stepChange() function will be
    called if the new line step is different from the previous
    setting.

    \sa setSteps() TQRangeControl::pageStep() setRange()
*/
int TQSlider::lineStep() const
{
    return TQRangeControl::lineStep();
}

/*!
    \property TQSlider::pageStep
    \brief the current page step

    When setting pageStep, the virtual stepChange() function will be
    called if the new page step is different from the previous
    setting.

    \sa TQRangeControl::setSteps() setLineStep() setRange()
*/

int TQSlider::pageStep() const
{
    return TQRangeControl::pageStep();
}

void TQSlider::setLineStep( int i )
{
    setSteps( i, pageStep() );
}

void TQSlider::setPageStep( int i )
{
    setSteps( lineStep(), i );
}

/*!
    \property TQSlider::value
    \brief the current slider value

    \sa TQRangeControl::value() prevValue()
*/

int TQSlider::value() const
{
    return TQRangeControl::value();
}

#endif
