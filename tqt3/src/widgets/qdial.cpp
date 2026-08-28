/****************************************************************************
**
** Implementation of the dial widget
**
** Created : 979899
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

#include "ntqdial.h"

#ifndef TQT_NO_DIAL

#include "ntqpainter.h"
#include "ntqpointarray.h"
#include "ntqcolor.h"
#include "ntqapplication.h"
#include "ntqregion.h"
#include "ntqbitmap.h"
#include "ntqstyle.h"
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif

#include <math.h> // sin(), cos(), atan()
//### Forutsetter linking med math lib - Jfr kommentar i qpainter_x11.cpp!

static const double m_pi = 3.14159265358979323846;
static const double rad_factor = 180.0 / m_pi;


class TQDialPrivate
{
public:
    TQDialPrivate()
    {
	wrapping = false;
	tracking = true;
	doNotEmit = false;
	target = 3.7;
	mousePressed = false;
    }

    bool wrapping;
    bool tracking;
    bool doNotEmit;
    double target;
    TQRect eraseArea;
    bool eraseAreaValid;
    bool showNotches;
    bool onlyOutside;
    bool mousePressed;

    TQPointArray lines;
};


/*!
    \class TQDial ntqdial.h

    \brief The TQDial class provides a rounded range control (like a speedometer or potentiometer).

    \ingroup basic
    \mainclass

    TQDial is used when the user needs to control a value within a
    program-definable range, and the range either wraps around
    (typically, 0..359 degrees) or the dialog layout needs a square
    widget.

    Both API- and UI-wise, the dial is very similar to a \link TQSlider
    slider. \endlink Indeed, when wrapping() is false (the default)
    there is no real difference between a slider and a dial. They
    have the same signals, slots and member functions, all of which do
    the same things. Which one you use depends only on your taste
    and on the application.

    The dial initially emits valueChanged() signals continuously while
    the slider is being moved; you can make it emit the signal less
    often by calling setTracking(false). dialMoved() is emitted
    continuously even when tracking() is false.

    The slider also emits dialPressed() and dialReleased() signals
    when the mouse button is pressed and released. But note that the
    dial's value can change without these signals being emitted; the
    keyboard and wheel can be used to change the value.

    Unlike the slider, TQDial attempts to draw a "nice" number of
    notches rather than one per lineStep(). If possible, the number
    of notches drawn is one per lineStep(), but if there aren't enough
    pixels to draw every one, TQDial will draw every second, third
    etc., notch. notchSize() returns the number of units per notch,
    hopefully a multiple of lineStep(); setNotchTarget() sets the
    target distance between neighbouring notches in pixels. The
    default is 3.75 pixels.

    Like the slider, the dial makes the TQRangeControl functions
    setValue(), addLine(), subtractLine(), addPage() and
    subtractPage() available as slots.

    The dial's keyboard interface is fairly simple: The left/up and
    right/down arrow keys move by lineStep(), page up and page down by
    pageStep() and Home and End to minValue() and maxValue().

    <img src=qdial-m.png> <img src=qdial-w.png>

    \sa TQScrollBar TQSpinBox
    \link guibooks.html#fowler GUI Design Handbook: Slider\endlink
*/




/*!
    Constructs a dial called \a name with parent \a parent. \a f is
    propagated to the TQWidget constructor. It has the default range of
    a TQRangeControl.
*/

TQDial::TQDial( TQWidget *parent, const char *name, WFlags f )
    : TQWidget( parent, name, f | WNoAutoErase ), TQRangeControl()
{
    d = new TQDialPrivate;
    d->eraseAreaValid = false;
    d->showNotches = false;
    d->onlyOutside = false;
    setFocusPolicy( TQWidget::WheelFocus );
}



/*!
    Constructs a dial called \a name with parent \a parent. The dial's
    value can never be smaller than \a minValue or greater than \a
    maxValue. Its page step size is \a pageStep, and its initial value
    is \a value.

    \a value is forced to be within the legal range.
*/

TQDial::TQDial( int minValue, int maxValue, int pageStep, int value,
	      TQWidget *parent, const char *name )
    : TQWidget( parent, name, WNoAutoErase ),
      TQRangeControl( minValue, maxValue, 1, pageStep, value )
{
    d = new TQDialPrivate;
    d->eraseAreaValid = false;
    d->showNotches = false;
    d->onlyOutside = false;
    setFocusPolicy( TQWidget::WheelFocus );
}

/*!
    Destroys the dial.
*/
TQDial::~TQDial()
{
    delete d;
}


void TQDial::setTracking( bool enable )
{
    d->tracking = enable;
}


/*!
    \property TQDial::tracking
    \brief whether tracking is enabled

    If true (the default), tracking is enabled. This means that the
    arrow can be moved using the mouse; otherwise the arrow cannot be
    moved with the mouse.
*/

bool TQDial::tracking() const
{
    return d ? d->tracking : true;
}

void TQDial::setValue( int newValue )
{ // ### set doNotEmit?	 Matthias?
    TQRangeControl::setValue( newValue );
}


/*!
    Increments the dial's value() by one lineStep().
*/

void TQDial::addLine()
{
    TQRangeControl::addLine();
}


/*!
    Decrements the dial's value() by one lineStep().
*/

void TQDial::subtractLine()
{
    TQRangeControl::subtractLine();
}


/*! \reimp */

void TQDial::resizeEvent( TQResizeEvent * e )
{
    d->lines.resize( 0 );
    TQWidget::resizeEvent( e );
}


/*!
  \reimp
*/

void TQDial::paintEvent( TQPaintEvent * e )
{
    repaintScreen( &e->rect() );
}

/*!
    Paints the dial using clip region \a cr.
*/

void TQDial::repaintScreen( const TQRect *cr )
{
    TQPainter p;
    p.begin( this );

    bool resetClipping = false;

    // calculate clip-region for erasing background
    if ( cr ) {
	p.setClipRect( *cr );
    } else if ( !d->onlyOutside && d->eraseAreaValid ) {
	TQRegion reg = d->eraseArea;
	double a;
	reg = reg.subtract( calcArrow( a ) );
	p.setClipRegion( reg );
	resetClipping = true;
    }

    TQRect br( calcDial() );
    p.setPen( NoPen );
    // if ( style() == MotifStyle )
    // p.setBrush( colorGroup().brush( TQColorGroup::Mid ) );
    // else {
    TQBrush b;
    if ( colorGroup().brush( TQColorGroup::Light ).pixmap() )
	b = TQBrush( colorGroup().brush( TQColorGroup::Light ) );
    else
	b = TQBrush( colorGroup().light(), Dense4Pattern );
    p.setBrush( b );
    p.setBackgroundMode( OpaqueMode );
    // }

    TQRect te = br;
    te.setWidth(te.width()+2);
    te.setHeight(te.height()+2);
    // erase background of dial
    if ( !d->onlyOutside ) {
	p.drawEllipse( te );
    }

    // erase remaining space around the dial
    TQRegion remaining( 0, 0, width(), height() );
    remaining = remaining.subtract( TQRegion( te, TQRegion::Ellipse ) );
    if ( p.hasClipping() )
	remaining = remaining.intersect( p.clipRegion() );
    erase(remaining);

    if ( resetClipping ) {
	if ( cr )
	    p.setClipRect( *cr );
	else
	    p.setClipRect( TQRect( 0, 0, width(), height() ) );
    }

    // draw notches
    if ( d->showNotches ) {
	calcLines();
	p.setPen( colorGroup().foreground() );
	p.drawLineSegments( d->lines );
    }

    // calculate and paint arrow
    p.setPen( TQPen( colorGroup().dark() ) );
    p.drawArc( te, 60 * 16, 180 * 16 );
    p.setPen( TQPen( colorGroup().light() ) );
    p.drawArc( te, 240 * 16, 180 * 16 );

    double a;
    TQPointArray arrow( calcArrow( a ) );
    TQRect ea( arrow.boundingRect() );
    d->eraseArea = ea;
    d->eraseAreaValid = true;

    p.setPen( NoPen );
    p.setBrush( colorGroup().brush( TQColorGroup::Button ) );
    if ( !d->onlyOutside )
	p.drawPolygon( arrow );

    a = angle( TQPoint( width() / 2, height() / 2 ), arrow[ 0 ] );
    p.setBrush( TQt::NoBrush );

    // that's still a hack...
    if ( a <= 0 || a > 200 ) {
	p.setPen( colorGroup().light() );
	p.drawLine( arrow[ 2 ], arrow[ 0 ] );
	p.drawLine( arrow[ 1 ], arrow[ 2 ] );
	p.setPen( colorGroup().dark() );
	p.drawLine( arrow[ 0 ], arrow[ 1 ] );
    } else if ( a > 0 && a < 45 ) {
	p.setPen( colorGroup().light() );
	p.drawLine( arrow[ 2 ], arrow[ 0 ] );
	p.setPen( colorGroup().dark() );
	p.drawLine( arrow[ 1 ], arrow[ 2 ] );
	p.drawLine( arrow[ 0 ], arrow[ 1 ] );
    } else if ( a >= 45 && a < 135 ) {
	p.setPen( colorGroup().dark() );
	p.drawLine( arrow[ 2 ], arrow[ 0 ] );
	p.drawLine( arrow[ 1 ], arrow[ 2 ] );
	p.setPen( colorGroup().light() );
	p.drawLine( arrow[ 0 ], arrow[ 1 ] );
    } else if ( a >= 135 && a < 200 ) {
	p.setPen( colorGroup().dark() );
	p.drawLine( arrow[ 2 ], arrow[ 0 ] );
	p.setPen( colorGroup().light() );
	p.drawLine( arrow[ 0 ], arrow[ 1 ] );
	p.drawLine( arrow[ 1 ], arrow[ 2 ] );
    }

    // draw focus rect around the dial
    if ( hasFocus() ) {
	p.setClipping( false );
	br.setWidth( br.width() + 2 );
	br.setHeight( br.height() + 2 );
	if ( d->showNotches ) {
	    int r = TQMIN( width(), height() ) / 2;
	    br.moveBy( -r / 6, - r / 6 );
	    br.setWidth( br.width() + r / 3 );
	    br.setHeight( br.height() + r / 3 );
	}
	// strange, but else we get redraw errors on Windows
	p.end();
	p.begin( this );
	p.save();
	p.setPen( TQPen( colorGroup().background() ) );
	p.setBrush( NoBrush );
	p.drawRect( br );
	p.restore();
	style().drawPrimitive( TQStyle::PE_FocusRect, &p, br, colorGroup());
    }
    p.end();
}


/*!
  \reimp
*/

void TQDial::keyPressEvent( TQKeyEvent * e )
{
    switch ( e->key() ) {
    case Key_Left: case Key_Down:
	subtractLine();
	break;
    case Key_Right: case Key_Up:
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
	break;
    }
}


/*!
  \reimp
*/

void TQDial::mousePressEvent( TQMouseEvent * e )
{
    d->mousePressed = true;
    setValue( valueFromPoint( e->pos() ) );
    emit dialPressed();
}


/*!
  \reimp
*/

void TQDial::mouseReleaseEvent( TQMouseEvent * e )
{
    d->mousePressed = false;
    setValue( valueFromPoint( e->pos() ) );
    emit dialReleased();
}


/*!
  \reimp
*/

void TQDial::mouseMoveEvent( TQMouseEvent * e )
{
    if ( !d->mousePressed )
	return;
    if ( !d->tracking || (e->state() & LeftButton) == 0 )
	return;
    d->doNotEmit = true;
    setValue( valueFromPoint( e->pos() ) );
    emit dialMoved( value() );
    d->doNotEmit = false;
}


/*!
  \reimp
*/
#ifndef TQT_NO_WHEELEVENT
void TQDial::wheelEvent( TQWheelEvent *e )
{
    setValue( value() - e->delta() / 120 );
}
#endif

/*!
  \reimp
*/

void TQDial::focusInEvent( TQFocusEvent * )
{
    d->onlyOutside = true;
    repaintScreen();
    d->onlyOutside = false;
}


/*!
  \reimp
*/

void TQDial::focusOutEvent( TQFocusEvent * )
{
    d->onlyOutside = true;
    repaintScreen();
    d->onlyOutside = false;
}

/*!
    Reimplemented to ensure the display is correct and to emit the
    valueChanged(int) signal when appropriate.
*/

void TQDial::valueChange()
{
    d->lines.resize( 0 );
    repaintScreen();
    if ( d->tracking || !d->doNotEmit ) {
	emit valueChanged( value() );
#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( this, 0, TQAccessible::ValueChanged );
#endif
    }
}


/*!
    Reimplemented to ensure tick-marks are consistent with the new range.
*/

void TQDial::rangeChange()
{
    d->lines.resize( 0 );
    repaintScreen();
}


/*!
  \internal
*/

int TQDial::valueFromPoint( const TQPoint & p ) const
{
    double yy = (double)height()/2.0 - p.y();
    double xx = (double)p.x() - width()/2.0;
    double a = (xx || yy) ? atan2(yy, xx) : 0;
        
    if ( a < m_pi/-2 )
	a = a + m_pi*2;

    int dist = 0;
    int minv = minValue(), maxv = maxValue();

    if ( minValue() < 0 ) {
	dist = -minValue();
	minv = 0;
	maxv = maxValue() + dist;
    }

    int r = maxv - minv;
    int v;
    if ( d->wrapping )
	v =  (int)(0.5 + minv + r*(m_pi*3/2-a)/(2*m_pi));
    else
	v =  (int)(0.5 + minv + r*(m_pi*4/3-a)/(m_pi*10/6));

    if ( dist > 0 )
	v -= dist;

    return bound( v );
}


/*!
  \internal
*/

double TQDial::angle( const TQPoint &p1, const TQPoint &p2 ) const
{
    double _angle = 0.0;

    if ( p1.x() == p2.x() ) {
	if ( p1.y() < p2.y() )
	    _angle = 270.0;
	else
	    _angle = 90.0;
    } else  {
	double x1, x2, y1, y2;

	if ( p1.x() <= p2.x() ) {
	    x1 = p1.x(); y1 = p1.y();
	    x2 = p2.x(); y2 = p2.y();
	} else {
	    x2 = p1.x(); y2 = p1.y();
	    x1 = p2.x(); y1 = p2.y();
	}

	double m = -( y2 - y1 ) / ( x2 - x1 );
	_angle = atan( m ) *  rad_factor;

	if ( p1.x() < p2.x() )
	    _angle = 180.0 - _angle;
	else
	    _angle = -_angle;
    }

    return _angle;
}

void TQDial::setWrapping( bool enable )
{
    if ( d->wrapping == enable )
	return;
    d->lines.resize( 0 );
    d->wrapping = enable;
    d->eraseAreaValid = false;
    repaintScreen();
}


/*!
    \property TQDial::wrapping
    \brief whether wrapping is enabled

    If true, wrapping is enabled. This means that the arrow can be
    turned around 360°. Otherwise there is some space at the bottom of
    the dial which is skipped by the arrow.

    This property's default is false.
*/

bool TQDial::wrapping() const
{
    return d->wrapping;
}


/*!
    \property TQDial::notchSize
    \brief the current notch size

    The notch size is in range control units, not pixels, and if
    possible it is a multiple of lineStep() that results in an
    on-screen notch size near notchTarget().

    \sa notchTarget() lineStep()
*/

int TQDial::notchSize() const
{
    // radius of the arc
    int r = TQMIN( width(), height() )/2;
    // length of the whole arc
    int l = (int)(r*(d->wrapping ? 6 : 5)*m_pi/6);
    // length of the arc from minValue() to minValue()+pageStep()
    if ( maxValue() > minValue()+pageStep() )
	l = (int)(0.5 + l * pageStep() / (maxValue()-minValue()));
    // length of a lineStep() arc
    l = l * lineStep() / pageStep();
    if ( l < 1 )
	l = 1;
    // how many times lineStep can be draw in d->target pixels
    l = (int)(0.5 + d->target / l);
    // we want notchSize() to be a non-zero multiple of lineStep()
    if ( !l )
	l = 1;
    return lineStep() * l;
}

void TQDial::setNotchTarget( double target )
{
    d->lines.resize( 0 );
    d->target = target;
    d->eraseAreaValid = false;
    d->onlyOutside = true;
    repaintScreen();
    d->onlyOutside = false;
}


/*!
    \property TQDial::notchTarget
    \brief the target number of pixels between notches

    The notch target is the number of pixels TQDial attempts to put
    between each notch.

    The actual size may differ from the target size.
*/

double TQDial::notchTarget() const
{
    return d->target;
}


/*!
    Increments the dial's value() by one pageStep() of steps.
*/

void TQDial::addPage()
{
    TQRangeControl::addPage();
}


/*!
    Decrements the dial's value() by one pageStep() of steps.
*/

void TQDial::subtractPage()
{
    TQRangeControl::subtractPage();
}


/*!
    \fn void TQDial::valueChanged( int value )

    This signal is emitted whenever the dial's \a value changes. The
    frequency of this signal is influenced by setTracking().
*/

/*!
    \fn void TQDial::dialPressed()

    This signal is emitted when the user begins mouse interaction with
    the dial.

    \sa dialReleased()
*/

/*!
    \fn void TQDial::dialMoved( int value )

    This signal is emitted whenever the dial \a value changes. The
    frequency of this signal is \e not influenced by setTracking().

    \sa valueChanged()
*/

/*!
    \fn void TQDial::dialReleased()

    This signal is emitted when the user ends mouse interaction with
    the dial.

    \sa dialPressed()
*/

void TQDial::setNotchesVisible( bool b )
{
    d->showNotches = b;
    d->eraseAreaValid = false;
    d->onlyOutside = true;
    repaintScreen();
    d->onlyOutside = false;
}

/*!
    \property TQDial::notchesVisible
    \brief whether the notches are shown

    If true, the notches are shown. If false (the default) notches are
    not shown.
*/
bool TQDial::notchesVisible() const
{
    return d->showNotches;
}

/*!
  \reimp
*/

TQSize TQDial::minimumSizeHint() const
{
    return TQSize( 50, 50 );
}

/*!
  \reimp
*/

TQSize TQDial::sizeHint() const
{
    return TQSize( 100, 100 ).expandedTo( TQApplication::globalStrut() );
}



/*!
  \internal
*/

TQPointArray TQDial::calcArrow( double &a ) const
{
    int r = TQMIN( width(), height() ) / 2;
    if ( maxValue() == minValue() )
	a = m_pi / 2;
    else if ( d->wrapping )
	a = m_pi * 3 / 2 - ( value() - minValue() ) * 2 * m_pi / ( maxValue() - minValue() );
    else
	a = ( m_pi * 8 - ( value() - minValue() ) * 10 * m_pi / ( maxValue() - minValue() ) ) / 6;

    int xc = width() / 2;
    int yc = height() / 2;

    int len = r - calcBigLineSize() - 5;
    if ( len < 5 )
	len = 5;
    int back = len / 4;
    if ( back < 1 )
	back = 1;

    TQPointArray arrow( 3 );
    arrow[0] = TQPoint( (int)( 0.5 + xc + len * cos(a) ),
		       (int)( 0.5 + yc -len * sin( a ) ) );
    arrow[1] = TQPoint( (int)( 0.5 + xc + back * cos( a + m_pi * 5 / 6 ) ),
		       (int)( 0.5 + yc - back * sin( a + m_pi * 5 / 6 ) ) );
    arrow[2] = TQPoint( (int)( 0.5 + xc + back * cos( a - m_pi * 5 / 6 ) ),
		       (int)( 0.5 + yc - back * sin( a - m_pi * 5 / 6 ) ) );
    return arrow;
}

/*!
  \internal
*/

TQRect TQDial::calcDial() const
{
    double r = TQMIN( width(), height() ) / 2.0;
    double d_ = r / 6.0;
    double dx = d_ + ( width() - 2 * r ) / 2.0 + 1;
    double dy = d_ + ( height() - 2 * r ) / 2.0 + 1;
    return TQRect( int(dx), int(dy),
		int(r * 2 - 2 * d_ - 2), int(r * 2 - 2 * d_ - 2) );
}

/*!
  \internal
*/

int TQDial::calcBigLineSize() const
{
    int r = TQMIN( width(), height() ) / 2;
    int bigLineSize = r / 6;
    if ( bigLineSize < 4 )
	bigLineSize = 4;
    if ( bigLineSize > r / 2 )
	bigLineSize = r / 2;
    return bigLineSize;
}

/*!
  \internal
*/

void TQDial::calcLines()
{
    if ( !d->lines.size() ) {
	double r = TQMIN( width(), height() ) / 2.0;
	int bigLineSize = calcBigLineSize();
	double xc = width() / 2.0;
	double yc = height() / 2.0;
	int ns = notchSize();
	int notches = ( maxValue() + ns - 1 - minValue() ) / ns;
	d->lines.resize( 2 + 2 * notches );
	int smallLineSize = bigLineSize / 2;
	int i;
	for( i = 0; i <= notches; i++ ) {
	    double angle = d->wrapping
		? m_pi * 3 / 2 - i * 2 * m_pi / notches
		: (m_pi * 8 - i * 10 * m_pi / notches) / 6;

	    double s = sin( angle ); // sin/cos aren't defined as const...
	    double c = cos( angle );
	    if ( i == 0 || ( ((ns * i ) % pageStep() ) == 0 ) ) {
		d->lines[2*i] = TQPoint( (int)( xc + ( r - bigLineSize ) * c ),
					(int)( yc - ( r - bigLineSize ) * s ) );
		d->lines[2*i+1] = TQPoint( (int)( xc + r * c ),
					  (int)( yc - r * s ) );
	    } else {
		d->lines[2*i] = TQPoint( (int)( xc + ( r - 1 - smallLineSize ) * c ),
					(int)( yc - ( r - 1 - smallLineSize ) * s ) );
		d->lines[2*i+1] = TQPoint( (int)( xc + ( r - 1 ) * c ),
					  (int)( yc -( r - 1 ) * s ) );
	    }
	}
    }
}

/*!
    \property TQDial::minValue
    \brief the current minimum value

    When setting this property, the \l TQDial::maxValue is adjusted if
    necessary to ensure that the range remains valid.

    \sa setRange()
*/
int TQDial::minValue() const
{
    return TQRangeControl::minValue();
}

/*!
    \property TQDial::maxValue
    \brief the current maximum value

    When setting this property, the \l TQDial::minValue is adjusted if
    necessary to ensure that the range remains valid.

    \sa setRange()
*/
int TQDial::maxValue() const
{
    return TQRangeControl::maxValue();
}

void TQDial::setMinValue( int minVal )
{
    TQRangeControl::setMinValue( minVal );
}

void TQDial::setMaxValue( int maxVal )
{
    TQRangeControl::setMaxValue( maxVal );
}

/*!
    \property TQDial::lineStep
    \brief the current line step

    setLineStep() calls the virtual stepChange() function if the new
    line step is different from the previous setting.

    \sa TQRangeControl::setSteps() pageStep setRange()
*/

int TQDial::lineStep() const
{
    return TQRangeControl::lineStep();
}

/*!
    \property TQDial::pageStep
    \brief the current page step

    setPageStep() calls the virtual stepChange() function if the new
    page step is different from the previous setting.

    \sa stepChange()
*/
int TQDial::pageStep() const
{
    return TQRangeControl::pageStep();
}

void TQDial::setLineStep( int i )
{
    setSteps( i, pageStep() );
}

void TQDial::setPageStep( int i )
{
    setSteps( lineStep(), i );
}

/*!
    \property TQDial::value
    \brief the current dial value

    This is guaranteed to be within the range
    \l{TQDial::minValue}..\l{TQDial::maxValue}.

    \sa minValue maxValue
*/

int TQDial::value() const
{
    return TQRangeControl::value();
}

#endif // QT_FEATURE_DIAL
