/****************************************************************************
**
** Implementation of TQProgressBar class
**
** Created : 970521
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

#include "ntqprogressbar.h"
#ifndef TQT_NO_PROGRESSBAR
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqpixmap.h"
#include "ntqstyle.h"
#include "ntqwmatrix.h"
#include "../kernel/qinternal_p.h"
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif
#include <limits.h>

class TQProgressBarPrivate
{
    public:
        TQProgressBarPrivate() : last_painted_progress( 0 ) { }

    int last_painted_progress;
};

/*!
    \class TQProgressBar ntqprogressbar.h
    \brief The TQProgressBar widget provides a horizontal progress bar.

    \ingroup advanced
    \mainclass

    A progress bar is used to give the user an indication of the
    progress of an operation and to reassure them that the application
    is still running.

    The progress bar uses the concept of \e steps; you give it the
    total number of steps and the number of steps completed so far and
    it will display the percentage of steps that have been completed.
    You can specify the total number of steps in the constructor or
    later with setTotalSteps(). The current number of steps is set
    with setProgress(). The progress bar can be rewound to the
    beginning with reset().

    If the total is given as 0 the progress bar shows a busy indicator
    instead of a percentage of steps. This is useful, for example,
    when using TQFtp or TQHttp to download items when they are unable to
    determine the size of the item being downloaded.

    \sa TQProgressDialog

    <img src=qprogbar-m.png> <img src=qprogbar-w.png>

    \sa TQProgressDialog
    \link guibooks.html#fowler GUI Design Handbook: Progress Indicator\endlink
*/


/*!
    Constructs a progress bar.

    The total number of steps is set to 100 by default.

    The \a parent, \a name and widget flags, \a f, are passed on to
    the TQFrame::TQFrame() constructor.

    \sa setTotalSteps()
*/

TQProgressBar::TQProgressBar( TQWidget *parent, const char *name, WFlags f )
    : TQFrame( parent, name, f | WNoAutoErase ),
      total_steps( 100 ),
      progress_val( -1 ),
      percentage( -1 ),
      center_indicator( true ),
      auto_indicator( true ),
      percentage_visible( true ),
      d( new TQProgressBarPrivate ),
      m_orientation( Horizontal )
{
    setSizePolicy( TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Fixed ) );
    initFrame();
}


/*!
    Constructs a progress bar.

    The \a totalSteps is the total number of steps that need to be
    completed for the operation which this progress bar represents.
    For example, if the operation is to examine 50 files, this value
    would be 50. Before examining the first file, call setProgress(0);
    call setProgress(50) after examining the last file.

    The \a parent, \a name and widget flags, \a f, are passed to the
    TQFrame::TQFrame() constructor.

    \sa setTotalSteps(), setProgress()
*/

TQProgressBar::TQProgressBar( int totalSteps,
			    TQWidget *parent, const char *name, WFlags f )
    : TQFrame( parent, name, f | WNoAutoErase ),
      total_steps( totalSteps ),
      progress_val( -1 ),
      percentage( -1 ),
      center_indicator( true ),
      auto_indicator( true ),
      percentage_visible( true ),
      d( new TQProgressBarPrivate ),
      m_orientation( Horizontal )
{
    setSizePolicy( TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Fixed ) );
    initFrame();
}


/*!
    Destroys the object and frees any allocated ressources.
*/
 
TQProgressBar::~TQProgressBar()
{
    delete d;
}


/*!
    Reset the progress bar. The progress bar "rewinds" and shows no
    progress.
*/

void TQProgressBar::reset()
{
    progress_val = -1;
    percentage = -1;
    setIndicator(progress_str, progress_val, total_steps);
    repaint( false );
}


/*!
    \property TQProgressBar::totalSteps
    \brief The total number of steps.

    If totalSteps is 0, the progress bar will display a busy
    indicator.

    \sa totalSteps()
*/

void TQProgressBar::setTotalSteps( int totalSteps )
{
    total_steps = totalSteps;

    // Current progress is invalid if larger than total
    if ( total_steps < progress_val )
	progress_val = -1;

    if ( isVisible() &&
	 ( setIndicator(progress_str, progress_val, total_steps) || !total_steps ) )
	repaint( false );
}


/*!
    \property TQProgressBar::progress
    \brief The current amount of progress

    This property is -1 if progress counting has not started.
*/

void TQProgressBar::setProgress( int progress )
{
    if ( progress == progress_val ||
	 progress < 0 || ( ( progress > total_steps ) && total_steps ) )
	return;

    progress_val = progress;

    setIndicator( progress_str, progress_val, total_steps );

    if ( isVisible() && repaintRequired() ) {
        repaint( false );
        d->last_painted_progress = progress;
    }

#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::ValueChanged );
#endif
}

/*!
    \overload

    Sets the amount of progress to \a progress and the total number of
    steps to \a totalSteps.

    \sa setTotalSteps()
*/

void TQProgressBar::setProgress( int progress, int totalSteps )
{
    if ( total_steps != totalSteps )
	setTotalSteps( totalSteps );
    setProgress( progress );
}

/*!
  \property TQProgressBar::progressString
  \brief the amount of progress as a string

    This property is TQString::null if progress counting has not started.
*/


/*!
    \reimp
*/
TQSize TQProgressBar::sizeHint() const
{
    constPolish();
    TQFontMetrics fm = fontMetrics();
    int cw = style().pixelMetric(TQStyle::PM_ProgressBarChunkWidth, this);
    TQSize sh = style().sizeFromContents(TQStyle::CT_ProgressBar, this,
				    TQSize( cw * 7 + fm.width( '0' ) * 4,
					   fm.height() + 8));
    if (m_orientation == TQt::Horizontal) {
        return sh;
    }
    else {
        return TQSize(sh.height(), sh.width());
    }
}


/*!
    \reimp
*/
TQSize TQProgressBar::minimumSizeHint() const
{
    return sizeHint();
}

/*!
    \property TQProgressBar::centerIndicator
    \brief whether the indicator string should be centered

    Changing this property sets \l TQProgressBar::indicatorFollowsStyle
    to false. The default is true.
*/

void TQProgressBar::setCenterIndicator( bool on )
{
    if ( !auto_indicator && on == center_indicator )
	return;
    auto_indicator = false;
    center_indicator = on;
    repaint( false );
}

/*!
    \property TQProgressBar::indicatorFollowsStyle
    \brief whether the display of the indicator string should follow the GUI style

    The default is true.

    \sa centerIndicator
*/

void TQProgressBar::setIndicatorFollowsStyle( bool on )
{
    if ( on == auto_indicator )
	return;
    auto_indicator = on;
    repaint( false );
}

/*!
    \property TQProgressBar::percentageVisible
    \brief whether the current progress value is displayed

    The default is true.

    \sa centerIndicator, indicatorFollowsStyle
*/
void TQProgressBar::setPercentageVisible( bool on )
{
    if ( on == percentage_visible )
	return;
    percentage_visible = on;
    repaint( false );
}

/*!
    \reimp
*/
void TQProgressBar::show()
{
    setIndicator( progress_str, progress_val, total_steps );
    TQFrame::show();
}

void TQProgressBar::initFrame()
{
    setFrameStyle(TQFrame::NoFrame);
}

/*!
    \reimp
*/
void TQProgressBar::styleChange( TQStyle& old )
{
    initFrame();
    TQFrame::styleChange( old );
}

/*!
    This method returns whether changing the progress to the \a newValue
    would require a repaint of the progress bar. This allows efficient
    repainting.
*/
bool TQProgressBar::repaintRequired() const
{
    if ( progress_val == d->last_painted_progress ) {
        return false;
    }

    const int width = contentsRect().width();
    if ( width == 0 ) {
        return false;
    }

    float progressPerPixel = 1.0;
    if ( total_steps > width ) {
        progressPerPixel = float( total_steps ) / float( width );
    }

    const int delta = d->last_painted_progress - progress_val;
    return TQABS( delta ) >= progressPerPixel;
}

TQt::Orientation TQProgressBar::orientation() const
{
    return m_orientation;
}

void TQProgressBar::setOrientation(Orientation orient)
{
    m_orientation = orient;
    if (m_orientation == TQt::Horizontal) {
        setSizePolicy( TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Fixed ) );
    }
    else {
        setSizePolicy( TQSizePolicy( TQSizePolicy::Fixed, TQSizePolicy::Expanding ) );
    }
}


/*!
    This method is called to generate the text displayed in the center
    (or in some styles, to the left) of the progress bar.

    The \a progress may be negative, indicating that the progress bar
    is in the "reset" state before any progress is set.

    The default implementation is the percentage of completion or
    blank in the reset state. The percentage is calculated based on
    the \a progress and \a totalSteps. You can set the \a indicator
    text if you wish.

    To allow efficient repainting of the progress bar, this method
    should return false if the string is unchanged from the last call
    to this function.
*/

bool TQProgressBar::setIndicator( TQString & indicator, int progress,
				 int totalSteps )
{
    if ( !totalSteps )
	return false;
    if ( progress < 0 ) {
	indicator = TQString::fromLatin1("");
	return true;
    } else {
	// Get the values down to something usable.
	if ( totalSteps > INT_MAX/1000 ) {
	    progress /= 1000;
	    totalSteps /= 1000;
	}

	int np = progress * 100 / totalSteps;
	if ( np != percentage ) {
	    percentage = np;
	    indicator.sprintf( "%d%%", np );
	    return true;
	} else {
	    return false;
	}
    }
}


/*!
    \reimp
*/
void TQProgressBar::drawContents( TQPainter *p )
{
	const TQRect bar = contentsRect();
	
	TQSharedDoubleBuffer buffer( p, bar.x(), bar.y(), bar.width(), bar.height() );
	
	TQPoint pn = backgroundOffset();
	buffer.painter()->setBrushOrigin( -pn.x(), -pn.y() );

	const TQPixmap *bpm = paletteBackgroundPixmap();
	if ( bpm ) {
		buffer.painter()->fillRect( bar, TQBrush( paletteBackgroundColor(), *bpm ) );
	}
	else {
		buffer.painter()->fillRect( bar, paletteBackgroundColor() );
	}
	buffer.painter()->setFont( p->font() );

	TQStyle::SFlags flags = TQStyle::Style_Default;
	if (isEnabled()) {
		flags |= TQStyle::Style_Enabled;
	}
	if (hasFocus()) {
		flags |= TQStyle::Style_HasFocus;
	}
	if (hasMouse()) {
		flags |= TQStyle::Style_MouseOver;
	}

	style().drawControl(TQStyle::CE_ProgressBarGroove, buffer.painter(), this,
			TQStyle::visualRect(style().subRect(TQStyle::SR_ProgressBarGroove, this), this ),
			colorGroup(), flags);

	TQWMatrix oldMatrix = buffer.painter()->worldMatrix();

	const TQStyleControlElementData &ceData = populateControlElementDataFromWidget(this, TQStyleOption());
	TQStyle::ControlElementFlags elementFlags = getControlElementFlagsForObject(this, TQStyleOption());

	TQRect ceDataRectOrig = ceData.rect;

	// Draw contents
	if (m_orientation == TQt::Vertical) {
		// If oriented vertically, apply a 90 degree rotation matrix to the painter
		TQWMatrix m;

// 		// Upside down
// 		m.rotate(90.0);
// 		m.translate(0, (bar.width())*(-1.0));

		// Right side up
		m.rotate(-90.0);
		m.translate((bar.height())*(-1.0), 0);

		buffer.painter()->setWorldMatrix(m, true);

		const_cast<TQStyleControlElementData&>(ceData).rect = TQRect(ceData.rect.y(), ceData.rect.x(), ceData.rect.height(), ceData.rect.width());
	}

	style().drawControl(TQStyle::CE_ProgressBarContents, buffer.painter(), ceData, elementFlags,
			TQStyle::visualRect(style().subRect(TQStyle::SR_ProgressBarContents, ceData, elementFlags, this), ceData, elementFlags),
			colorGroup(), flags, TQStyleOption(), this);

	buffer.painter()->setWorldMatrix(oldMatrix, true);

	if (percentageVisible()) {
		style().drawControl(TQStyle::CE_ProgressBarLabel, buffer.painter(), this,
				TQStyle::visualRect(style().subRect(TQStyle::SR_ProgressBarLabel, this), this ),
				colorGroup(), flags);
	}

	const_cast<TQStyleControlElementData&>(ceData).rect = ceDataRectOrig;
}

#endif
