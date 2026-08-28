/****************************************************************************
**
** Implementation of TQSpinWidget class
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

#include "ntqrangecontrol.h"

#ifndef TQT_NO_SPINWIDGET

#include "ntqrect.h"
#include "ntqtimer.h"
#include "ntqstyle.h"
#include "ntqpainter.h"

class TQSpinWidgetPrivate
{
public:
    TQSpinWidgetPrivate()
	: upEnabled( true ),
	  downEnabled( true ),
	  theButton( 0 ),
	  buttonDown( 0 ),
	  timerUp( 0 ),
	  bsyms( TQSpinWidget::UpDownArrows ),
	  ed ( 0 ) {}
    uint upEnabled :1;
    uint downEnabled :1;
    uint theButton :2;
    uint buttonDown :2;
    uint timerUp : 1;
    TQRect up;
    TQRect down;
    TQTimer auRepTimer;
    TQSpinWidget::ButtonSymbols bsyms;
    TQWidget *ed;
    void startTimer( int msec ) { auRepTimer.start( msec, true ); }
    void startTimer( bool up, int msec ) { timerUp = up; startTimer( msec ); }
    void stopTimer() { auRepTimer.stop(); }
};

/*!

    \class TQSpinWidget qspinwidget.h
    \brief The TQSpinWidget class is an internal range control related class.

    \internal

    Constructs an empty range control widget with parent \a parent
    called \a name.

*/

TQSpinWidget::TQSpinWidget( TQWidget* parent, const char* name )
    : TQWidget( parent, name )
{
    d = new TQSpinWidgetPrivate();
    connect( &d->auRepTimer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( timerDone() ) );
    setFocusPolicy( StrongFocus );

    arrange();
    updateDisplay();
}


/*! Destroys the object and frees any allocated resources.

*/

TQSpinWidget::~TQSpinWidget()
{
    delete d;
}

/*! */
TQWidget * TQSpinWidget::editWidget()
{
    return d->ed;
}

/*!
    Sets the editing widget to \a w.
*/
void TQSpinWidget::setEditWidget( TQWidget * w )
{
    if ( w ) {
        if (w->parentWidget() != this)
	    w->reparent( this, TQPoint( 0, 0 ) );
	setFocusProxy( w );
    }
    d->ed = w;
    arrange();
    updateDisplay();
}

/*! \reimp

*/

void TQSpinWidget::mousePressEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton ) {
	d->stopTimer();
	d->buttonDown = 0;
	d->theButton = 0;
	repaint( d->down.unite( d->up ), false );
	return;
    }

    uint oldButtonDown = d->buttonDown;

    if ( d->down.contains( e->pos() ) && d->downEnabled )
	d->buttonDown = 1;
    else if ( d->up.contains( e->pos() ) && d->upEnabled )
	d->buttonDown = 2;
    else
	d->buttonDown = 0;

    d->theButton = d->buttonDown;
    if ( oldButtonDown != d->buttonDown ) {
	if ( !d->buttonDown ) {
	    repaint( d->down.unite( d->up ), false );
	} else if ( d->buttonDown & 1 ) {
	    repaint( d->down, false );
	    stepDown();
	    d->startTimer( false, 300 );
	} else if ( d->buttonDown & 2 ) {
	    repaint( d->up, false );
	    stepUp();
	    d->startTimer( true, 300 );
	}
    }
}

/*!

*/

void TQSpinWidget::arrange()
{
    d->up = TQStyle::visualRect( style().querySubControlMetrics( TQStyle::CC_SpinWidget, this,
								TQStyle::SC_SpinWidgetUp ), this );
    d->down = TQStyle::visualRect( style().querySubControlMetrics( TQStyle::CC_SpinWidget, this,
								  TQStyle::SC_SpinWidgetDown ), this );
    if ( d->ed ) {
    	TQRect r = TQStyle::visualRect( style().querySubControlMetrics( TQStyle::CC_SpinWidget, this,
								  TQStyle::SC_SpinWidgetEditField ), this );
	d->ed->setGeometry( r );
    }
}

/*!

*/

void TQSpinWidget::stepUp()
{
    emit stepUpPressed();
}

void TQSpinWidget::resizeEvent( TQResizeEvent* )
{
    arrange();
}

/*!

*/

void TQSpinWidget::stepDown()
{
    emit stepDownPressed();
}


void TQSpinWidget::timerDone()
{
    // we use a double timer to make it possible for users to do
    // something with 0-timer on valueChanged.
    TQTimer::singleShot( 1, this, TQ_SLOT( timerDoneEx() ) );
}

void TQSpinWidget::timerDoneEx()
{
    if ( !d->buttonDown )
	return;
    if ( d->timerUp )
	stepUp();
    else
	stepDown();
    d->startTimer( 100 );
}


void TQSpinWidget::windowActivationChange( bool oldActive )
{
    //was active, but lost focus
    if ( oldActive && d->buttonDown ) {
	d->stopTimer();
	d->buttonDown = 0;
	d->theButton = 0;
    }
    TQWidget::windowActivationChange( oldActive );
}



/*!
    The event is passed in \a e.
*/

void TQSpinWidget::mouseReleaseEvent( TQMouseEvent *e )
{
    if ( e->button() != LeftButton )
	return;

    uint oldButtonDown = d->theButton;
    d->theButton = 0;
    if ( oldButtonDown != d->theButton ) {
	if ( oldButtonDown & 1 )
	    repaint( d->down, false );
	else if ( oldButtonDown & 2 )
	    repaint( d->up, false );
    }
    d->stopTimer();
    d->buttonDown = 0;
}


/*!
    The event is passed in \a e.
*/

void TQSpinWidget::mouseMoveEvent( TQMouseEvent *e )
{
    if ( !(e->state() & LeftButton ) )
	return;

    uint oldButtonDown = d->theButton;
    if ( oldButtonDown & 1 && !d->down.contains( e->pos() ) ) {
	d->stopTimer();
	d->theButton = 0;
	repaint( d->down, false );
    } else if ( oldButtonDown & 2 && !d->up.contains( e->pos() ) ) {
	d->stopTimer();
	d->theButton = 0;
	repaint( d->up, false );
    } else if ( !oldButtonDown && d->up.contains( e->pos() ) && d->buttonDown & 2 ) {
	d->startTimer( 500 );
	d->theButton = 2;
	repaint( d->up, false );
    } else if ( !oldButtonDown && d->down.contains( e->pos() ) && d->buttonDown & 1 ) {
	d->startTimer( 500 );
	d->theButton = 1;
	repaint( d->down, false );
    }
}


/*!
    The event is passed in \a e.
*/
#ifndef TQT_NO_WHEELEVENT
void TQSpinWidget::wheelEvent( TQWheelEvent *e )
{
    e->accept();
    static float offset = 0;
    static TQSpinWidget* offset_owner = 0;
    if ( offset_owner != this ) {
	offset_owner = this;
	offset = 0;
    }
    offset += -e->delta()/120;
    if ( TQABS( offset ) < 1 )
	return;
    int ioff = int(offset);
    int i;
    for( i=0; i < TQABS( ioff ); i++ )
	offset > 0 ? stepDown() : stepUp();
    offset -= ioff;
}
#endif

/*!

*/
void TQSpinWidget::paintEvent( TQPaintEvent * )
{
    TQPainter p( this );

    TQStyle::SFlags flags = TQStyle::Style_Default;
    if (isEnabled())
	flags |= TQStyle::Style_Enabled;
    if (hasMouse())
	flags |= TQStyle::Style_MouseOver;
    if (hasFocus() || (focusProxy() && focusProxy()->hasFocus()))
	flags |= TQStyle::Style_HasFocus;

    TQStyle::SCFlags active;
    if ( d->theButton & 1 )
	active = TQStyle::SC_SpinWidgetDown;
    else if ( d->theButton & 2 )
	active = TQStyle::SC_SpinWidgetUp;
    else
	active = TQStyle::SC_None;

    TQRect fr = TQStyle::visualRect(
	style().querySubControlMetrics( TQStyle::CC_SpinWidget, this,
					TQStyle::SC_SpinWidgetFrame ), this );
    style().drawComplexControl( TQStyle::CC_SpinWidget, &p, this,
				fr, colorGroup(),
				flags,
				(uint)TQStyle::SC_All,
				active );
}


/*!
    The previous style is passed in \a old.
*/

void TQSpinWidget::styleChange( TQStyle& old )
{
    arrange();
    TQWidget::styleChange( old );
}

/*!
*/

TQRect TQSpinWidget::upRect() const
{
    return d->up;
}

/*!
*/

TQRect TQSpinWidget::downRect() const
{
    return d->down;
}

/*!
*/

void TQSpinWidget::updateDisplay()
{
    if ( !isEnabled() ) {
	d->upEnabled = false;
	d->downEnabled = false;
    }
    if ( d->theButton & 1 && ( d->downEnabled ) == 0 ) {
	d->theButton &= ~1;
	d->buttonDown &= ~1;
    }

    if ( d->theButton & 2 && ( d->upEnabled ) == 0 ) {
	d->theButton &= ~2;
	d->buttonDown &= ~2;
    }
    repaint( false );
}


/*!
    The previous enabled state is passed in \a old.
*/

void TQSpinWidget::enableChanged( bool )
{
    d->upEnabled = isEnabled();
    d->downEnabled = isEnabled();
    updateDisplay();
}


/*!
    Sets up-enabled to \a on.
*/

void TQSpinWidget::setUpEnabled( bool on )
{
    if ( (bool)d->upEnabled != on ) {
	d->upEnabled = on;
	updateDisplay();
    }
}

/*!
*/

bool TQSpinWidget::isUpEnabled() const
{
    return d->upEnabled;
}

/*!
    Sets down-enabled to \a on.
*/

void TQSpinWidget::setDownEnabled( bool on )
{
    if ( (bool)d->downEnabled != on ) {
	d->downEnabled = on;
	updateDisplay();
    }
}

/*!
*/

bool TQSpinWidget::isDownEnabled() const
{
    return d->downEnabled;
}

/*!
    Sets the button symbol to \a bs.
*/

void TQSpinWidget::setButtonSymbols( ButtonSymbols bs )
{
    d->bsyms = bs;
}

/*!
*/

TQSpinWidget::ButtonSymbols TQSpinWidget::buttonSymbols() const
{
    return d->bsyms;
}

#endif
