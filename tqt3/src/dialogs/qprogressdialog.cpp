/****************************************************************************
**
** Implementation of TQProgressDialog class
**
** Created : 970521
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the dialogs module of the TQt GUI Toolkit.
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

#include "ntqprogressdialog.h"

#ifndef TQT_NO_PROGRESSDIALOG

#include "ntqaccel.h"
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqdatetime.h"
#include "ntqapplication.h"
#include "ntqstyle.h"
#include "ntqpushbutton.h"
#include "ntqcursor.h"
#include "ntqtimer.h"
#include <limits.h>

// If the operation is expected to take this long (as predicted by
// progress time), show the progress dialog.
static const int defaultShowTime    = 4000;
// Wait at least this long before attempting to make a prediction.
static const int minWaitTime = 50;

// Various layout values
static const int margin_lr   = 10;
static const int margin_tb   = 10;
static const int spacing     = 4;


class TQProgressDialogData
{
public:
    TQProgressDialogData( TQProgressDialog* that, TQWidget* parent,
		   const TQString& labelText,
		   int totalSteps ) :
	creator( parent ),
	label( new TQLabel(labelText,that,"label") ),
	cancel( 0 ),
	bar( new TQProgressBar(totalSteps,that,"bar") ),
	shown_once( false ),
	cancellation_flag( false ),
	showTime( defaultShowTime )
    {
	label->setAlignment(that->style().styleHint(TQStyle::SH_ProgressDialog_TextLabelAlignment, that));
    }

    TQWidget	 *creator;
    TQLabel	 *label;
    TQPushButton	 *cancel;
    TQProgressBar *bar;
    bool	  shown_once;
    bool	  cancellation_flag;
    TQTime	  starttime;
#ifndef TQT_NO_CURSOR
    TQCursor	  parentCursor;
#endif
    int		  showTime;
    bool autoClose;
    bool autoReset;
    bool forceHide;
};


/*!
  \class TQProgressDialog ntqprogressdialog.h
  \brief The TQProgressDialog class provides feedback on the progress of a slow operation.
  \ingroup dialogs
  \mainclass

  A progress dialog is used to give the user an indication of how long
  an operation is going to take, and to demonstrate that the
  application has not frozen. It can also give the user an opportunity
  to abort the operation.

  A common problem with progress dialogs is that it is difficult to know
  when to use them; operations take different amounts of time on different
  hardware.  TQProgressDialog offers a solution to this problem:
  it estimates the time the operation will take (based on time for
  steps), and only shows itself if that estimate is beyond minimumDuration()
  (4 seconds by default).

  Use setTotalSteps() (or the constructor) to set the number of
  "steps" in the operation and call setProgress() as the operation
  progresses. The step value can be chosen arbitrarily. It can be the
  number of files copied, the number of bytes received, the number of
  iterations through the main loop of your algorithm, or some other
  suitable unit.  Progress starts at 0, and the progress dialog shows
  that the operation has finished when you call setProgress() with
  totalSteps() as its argument.

  The dialog automatically resets and hides itself at the end of the
  operation. Use setAutoReset() and setAutoClose() to change this
  behavior.

  There are two ways of using TQProgressDialog: modal and modeless.

  Using a modal TQProgressDialog is simpler for the programmer, but you
  must call TQApplication::processEvents() or
  TQEventLoop::processEvents(ExcludeUserInput) to keep the event loop
  running to ensure that the application doesn't freeze. Do the
  operation in a loop, call \l setProgress() at intervals, and check
  for cancellation with wasCanceled(). For example:
\code
TQProgressDialog progress( "Copying files...", "Abort Copy", numFiles,
                          this, "progress", true );
for ( int i = 0; i < numFiles; i++ ) {
    progress.setProgress( i );
    tqApp->processEvents();

    if ( progress.wasCanceled() )
        break;
    //... copy one file
}
progress.setProgress( numFiles );
\endcode

  A modeless progress dialog is suitable for operations that take
  place in the background, where the user is able to interact with the
  application. Such operations are typically based on TQTimer (or
  TQObject::timerEvent()), TQSocketNotifier, or TQUrlOperator; or performed
  in a separate thread. A TQProgressBar in the status bar of your main window
  is often an alternative to a modeless progress dialog.

  You need to have an event loop to be running, connect the
  canceled() signal to a slot that stops the operation, and call \l
  setProgress() at intervals. For example:
\code
Operation::Operation( TQObject *parent = 0 )
    : TQObject( parent ), steps( 0 )
{
    pd = new TQProgressDialog( "Operation in progress.", "Cancel", 100 );
    connect( pd, TQ_SIGNAL(canceled()), this, TQ_SLOT(cancel()) );
    t = new TQTimer( this );
    connect( t, TQ_SIGNAL(timeout()), this, TQ_SLOT(perform()) );
    t->start( 0 );
}

void Operation::perform()
{
    pd->setProgress( steps );
    //... perform one percent of the operation
    steps++;
    if ( steps > pd->totalSteps() )
        t->stop();
}

void Operation::cancel()
{
    t->stop();
    //... cleanup
}
\endcode


  In both modes the progress dialog may be customized by
  replacing the child widgets with custom widgets by using setLabel(),
  setBar(), and setCancelButton().
  The functions setLabelText() and setCancelButtonText()
  set the texts shown.

  <img src=qprogdlg-m.png> <img src=qprogdlg-w.png>

  \sa TQDialog TQProgressBar
  \link guibooks.html#fowler GUI Design Handbook: Progress Indicator\endlink
*/


/*!
  Returns the TQLabel currently being displayed above the progress bar.
  This TQLabel is owned by the TQProgressDialog.

  \sa setLabel()
*/
TQLabel *TQProgressDialog::label() const
{
    return d->label;
}

/*!
  Returns the TQProgressBar currently being used to display progress.
  This TQProgressBar is owned by the TQProgressDialog.

  \sa setBar()
*/
TQProgressBar *TQProgressDialog::bar() const
{
    return d->bar;
}


/*!
  Constructs a progress dialog.

  Default settings:
  \list
  \i The label text is empty.
  \i The cancel button text is (translated) "Cancel".
  \i The total number of steps is 100.
  \endlist

  The \a creator argument is the widget to use as the dialog's parent.
  The \a name, \a modal, and the widget flags, \a f, are
  passed to the TQDialog::TQDialog() constructor. If \a modal is false (the
  default), you must have an event loop proceeding for any redrawing
  of the dialog to occur. If \a modal is true, the dialog ensures that
  events are processed when needed.

  \sa setLabelText(), setLabel(), setCancelButtonText(), setCancelButton(),
  setTotalSteps()
*/

TQProgressDialog::TQProgressDialog( TQWidget *creator, const char *name,
				  bool modal, WFlags f )
    : TQDialog( creator, name, modal, f)
{
    init( creator, TQString::fromLatin1(""), tr("Cancel"), 100 );
}

/*!
  Constructs a progress dialog.

   The \a labelText is text used to remind the user what is progressing.

   The \a cancelButtonText is the text to display on the cancel button,
	    or 0 if no cancel button is to be shown.

   The \a totalSteps is the total number of steps in the operation for
   which this progress dialog shows progress.  For example, if the
   operation is to examine 50 files, this value would be 50. Before
   examining the first file, call setProgress(0). As each file is
   processed call setProgress(1), setProgress(2), etc., finally
   calling setProgress(50) after examining the last file.

   The \a creator argument is the widget to use as the dialog's parent.
   The \a name, \a modal, and widget flags, \a f, are passed to the
   TQDialog::TQDialog() constructor. If \a modal is false (the default),
   you will must have an event loop proceeding for any redrawing of
   the dialog to occur. If \a modal is true, the dialog ensures that
   events are processed when needed.


  \sa setLabelText(), setLabel(), setCancelButtonText(), setCancelButton(),
  setTotalSteps()
*/

TQProgressDialog::TQProgressDialog( const TQString &labelText,
				  const TQString &cancelButtonText,
				  int totalSteps,
				  TQWidget *creator, const char *name,
				  bool modal, WFlags f )
    : TQDialog( creator, name, modal, f)
{
    init( creator, labelText, cancelButtonText, totalSteps );
}


/*!
  Destroys the progress dialog.
*/

TQProgressDialog::~TQProgressDialog()
{
#ifndef TQT_NO_CURSOR
    if ( d->creator )
	d->creator->setCursor( d->parentCursor );
#endif
    delete d;
}

void TQProgressDialog::init( TQWidget *creator,
			    const TQString& lbl, const TQString& canc,
			    int totstps)
{
    d = new TQProgressDialogData(this, creator, lbl, totstps);
    d->autoClose = true;
    d->autoReset = true;
    d->forceHide = false;
    setCancelButtonText( canc );
    connect( this, TQ_SIGNAL(canceled()), this, TQ_SIGNAL(cancelled()) );
    connect( this, TQ_SIGNAL(canceled()), this, TQ_SLOT(cancel()) );
    forceTimer = new TQTimer( this );
    connect( forceTimer, TQ_SIGNAL(timeout()), this, TQ_SLOT(forceShow()) );
    layout();
}

/*!
  \fn void TQProgressDialog::canceled()

  This signal is emitted when the cancel button is clicked.
  It is connected to the cancel() slot by default.

  \sa wasCanceled()
*/

/*!
  \fn void TQProgressDialog::cancelled()

  \obsolete

  Use canceled() instead.
*/


/*!
  Sets the label to \a label. The progress dialog resizes to fit. The
  label becomes owned by the progress dialog and will be deleted when
  necessary, so do not pass the address of an object on the stack.

  \sa setLabelText()
*/

void TQProgressDialog::setLabel( TQLabel *label )
{
    delete d->label;
    d->label = label;
    if (label) {
	if ( label->parentWidget() == this ) {
	    label->hide(); // until we resize
	} else {
	    label->reparent( this, 0, TQPoint(0,0), false );
	}
    }
    int w = TQMAX( isVisible() ? width() : 0, sizeHint().width() );
    int h = TQMAX( isVisible() ? height() : 0, sizeHint().height() );
    resize( w, h );
    if (label)
	label->show();
}


/*!
  \property TQProgressDialog::labelText
  \brief the label's text

  The default text is TQString::null.
*/

TQString TQProgressDialog::labelText() const
{
    if ( label() )
	return label()->text();
    return TQString::null;
}

void TQProgressDialog::setLabelText( const TQString &text )
{
    if ( label() ) {
	label()->setText( text );
	int w = TQMAX( isVisible() ? width() : 0, sizeHint().width() );
	int h = TQMAX( isVisible() ? height() : 0, sizeHint().height() );
	resize( w, h );
    }
}


/*!
  Sets the cancel button to the push button, \a cancelButton. The
  progress dialog takes ownership of this button which will be deleted
  when necessary, so do not pass the address of an object that is on
  the stack, i.e. use new() to create the button.

  \sa setCancelButtonText()
*/

void TQProgressDialog::setCancelButton( TQPushButton *cancelButton )
{
    delete d->cancel;
    d->cancel = cancelButton;
    if (cancelButton) {
	if ( cancelButton->parentWidget() == this ) {
	    cancelButton->hide(); // until we resize
	} else {
	    cancelButton->reparent( this, 0, TQPoint(0,0), false );
	}
	connect( d->cancel, TQ_SIGNAL(clicked()), this, TQ_SIGNAL(canceled()) );
#ifndef TQT_NO_ACCEL
	TQAccel *accel = new TQAccel( this );
	accel->connectItem( accel->insertItem(Key_Escape),
			    d->cancel, TQ_SIGNAL(clicked()) );
#endif
    }
    int w = TQMAX( isVisible() ? width() : 0, sizeHint().width() );
    int h = TQMAX( isVisible() ? height() : 0, sizeHint().height() );
    resize( w, h );
    if (cancelButton)
	cancelButton->show();
}

/*!
  Sets the cancel button's text to \a cancelButtonText.
  \sa setCancelButton()
*/

void TQProgressDialog::setCancelButtonText( const TQString &cancelButtonText )
{
    if ( !cancelButtonText.isNull() ) {
	if ( d->cancel )
	    d->cancel->setText(cancelButtonText);
	else
	    setCancelButton(new TQPushButton(cancelButtonText, this, "cancel"));
    } else {
	setCancelButton(0);
    }
    int w = TQMAX( isVisible() ? width() : 0, sizeHint().width() );
    int h = TQMAX( isVisible() ? height() : 0, sizeHint().height() );
    resize( w, h );
}


/*!
  Sets the progress bar widget to \a bar. The progress dialog resizes to
  fit. The progress dialog takes ownership of the progress \a bar which
  will be deleted when necessary, so do not use a progress bar
  allocated on the stack.
*/

void TQProgressDialog::setBar( TQProgressBar *bar )
{
    if ( progress() > 0 ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQProgrssDialog::setBar: Cannot set a new progress bar "
		 "while the old one is active" );
#endif
    }
    delete d->bar;
    d->bar = bar;
    int w = TQMAX( isVisible() ? width() : 0, sizeHint().width() );
    int h = TQMAX( isVisible() ? height() : 0, sizeHint().height() );
    resize( w, h );
}


/*!
  \property TQProgressDialog::wasCancelled
  \brief whether the dialog was canceled

  \obsolete

  Use \l wasCanceled instead.
*/

/*!
  \property TQProgressDialog::wasCanceled
  \brief whether the dialog was canceled

  \sa setProgress()
*/

bool TQProgressDialog::wasCancelled() const
{
    return d->cancellation_flag;
}


/*!
  \property TQProgressDialog::totalSteps
  \brief the total number of steps

  The default is 0.
*/

int TQProgressDialog::totalSteps() const
{
    if ( d && d->bar )
	return bar()->totalSteps();
    return 0;
}

void TQProgressDialog::setTotalSteps( int totalSteps )
{
    bar()->setTotalSteps( totalSteps );
}


/*!
  Resets the progress dialog.
  The progress dialog becomes hidden if autoClose() is true.

  \sa setAutoClose(), setAutoReset()
*/

void TQProgressDialog::reset()
{
#ifndef TQT_NO_CURSOR
    if ( progress() >= 0 ) {
	if ( d->creator )
	    d->creator->setCursor( d->parentCursor );
    }
#endif
    if ( d->autoClose || d->forceHide )
	hide();
    bar()->reset();
    d->cancellation_flag = false;
    d->shown_once = false;
    forceTimer->stop();
}

/*!
  Resets the progress dialog. wasCanceled() becomes true until
  the progress dialog is reset.
  The progress dialog becomes hidden.
*/

void TQProgressDialog::cancel()
{
    d->forceHide = true;
    reset();
    d->forceHide = false;
    d->cancellation_flag = true;
}

/*!
  \property TQProgressDialog::progress
  \brief the current amount of progress made.

  For the progress dialog to work as expected, you should initially set
  this property to 0 and finally set it to
  TQProgressDialog::totalSteps(); you can call setProgress() any number of times
  in-between.

  \warning If the progress dialog is modal
    (see TQProgressDialog::TQProgressDialog()),
    this function calls TQApplication::processEvents(), so take care that
    this does not cause undesirable re-entrancy in your code. For example,
    don't use a TQProgressDialog inside a paintEvent()!

  \sa totalSteps
*/

int TQProgressDialog::progress() const
{
    return bar()->progress();
}

void TQProgressDialog::setProgress( int progress )
{
    if ( progress == bar()->progress() ||
	 ( bar()->progress() == -1 && progress == bar()->totalSteps() ) )
	return;

    bar()->setProgress(progress);

    if ( d->shown_once ) {
	if (testWFlags(WShowModal))
	    tqApp->processEvents();
    } else {
	if ( progress == 0 ) {
#ifndef TQT_NO_CURSOR
	    if ( d->creator ) {
		d->parentCursor = d->creator->cursor();
		d->creator->setCursor( waitCursor );
	    }
#endif
	    d->starttime.start();
	    forceTimer->start( d->showTime );
	    return;
	} else {
	    bool need_show;
	    int elapsed = d->starttime.elapsed();
	    if ( elapsed >= d->showTime ) {
		need_show = true;
	    } else {
		if ( elapsed > minWaitTime ) {
		    int estimate;
		    if ( (totalSteps() - progress) >= INT_MAX / elapsed )
			estimate = (totalSteps() - progress) / progress * elapsed;
		    else
			estimate = elapsed * (totalSteps() - progress) / progress;
		    need_show = estimate >= d->showTime;
		} else {
		    need_show = false;
		}
	    }
	    if ( need_show ) {
		int w = TQMAX( isVisible() ? width() : 0, sizeHint().width() );
		int h = TQMAX( isVisible() ? height() : 0, sizeHint().height() );
		resize( w, h );
		show();
		d->shown_once = true;
	    }
	}
#ifdef TQ_WS_MACX
	TQApplication::flush();
#endif
    }

    if ( progress == bar()->totalSteps() && d->autoReset )
	reset();
}

/*!
  \overload

  Sets the current amount of progress to \a progress and the total number of
  steps to \a totalSteps.

  \sa setTotalSteps()
*/

void TQProgressDialog::setProgress( int progress, int totalSteps )
{
    setTotalSteps( totalSteps );
    setProgress( progress );
}

/*!
  Returns a size that fits the contents of the progress dialog.
  The progress dialog resizes itself as required, so you should not
  need to call this yourself.
*/

TQSize TQProgressDialog::sizeHint() const
{
    TQSize sh = label()->sizeHint();
    TQSize bh = bar()->sizeHint();
    int h = margin_tb*2 + bh.height() + sh.height() + spacing;
    if ( d->cancel )
	h += d->cancel->sizeHint().height() + spacing;
    return TQSize( TQMAX(200, sh.width() + 2*margin_lr), h );
}

/*!\reimp
*/
void TQProgressDialog::resizeEvent( TQResizeEvent * )
{
    layout();
}

/*!
  \reimp
*/
void TQProgressDialog::styleChange(TQStyle& s)
{
    TQDialog::styleChange(s);
    layout();
}

void TQProgressDialog::layout()
{
    int sp = spacing;
    int mtb = margin_tb;
    int mlr = TQMIN(width()/10, margin_lr);
    const bool centered =
	bool(style().styleHint(TQStyle::SH_ProgressDialog_CenterCancelButton, this));

    TQSize cs = d->cancel ? d->cancel->sizeHint() : TQSize(0,0);
    TQSize bh = bar()->sizeHint();
    int cspc;
    int lh = 0;

    // Find spacing and sizes that fit.  It is important that a progress
    // dialog can be made very small if the user demands it so.
    for (int attempt=5; attempt--; ) {
	cspc = d->cancel ? cs.height() + sp : 0;
	lh = TQMAX(0, height() - mtb - bh.height() - sp - cspc);

	if ( lh < height()/4 ) {
	    // Getting cramped
	    sp /= 2;
	    mtb /= 2;
	    if ( d->cancel ) {
		cs.setHeight(TQMAX(4,cs.height()-sp-2));
	    }
	    bh.setHeight(TQMAX(4,bh.height()-sp-1));
	} else {
	    break;
	}
    }

    if ( d->cancel ) {
	d->cancel->setGeometry(
	    centered ? width()/2 - cs.width()/2 : width() - mlr - cs.width(),
	    height() - mtb - cs.height() + sp,
	    cs.width(), cs.height() );
    }

    label()->setGeometry( mlr, 0, width()-mlr*2, lh );
    bar()->setGeometry( mlr, lh+sp, width()-mlr*2, bh.height() );
}

/*!
    \property TQProgressDialog::minimumDuration
    \brief the time that must pass before the dialog appears

    If the expected duration of the task is less than the
    minimumDuration, the dialog will not appear at all. This prevents
    the dialog popping up for tasks that are quickly over. For tasks
    that are expected to exceed the minimumDuration, the dialog will
    pop up after the minimumDuration time or as soon as any progress
    is set.

    If set to 0, the dialog is always shown as soon as any progress is
    set. The default is 4000 milliseconds.
*/
void TQProgressDialog::setMinimumDuration( int ms )
{
    d->showTime = ms;
    if ( bar()->progress() == 0 ) {
	forceTimer->stop();
	forceTimer->start( ms );
    }
}

int TQProgressDialog::minimumDuration() const
{
    return d->showTime;
}


/*!
  \reimp
*/

void TQProgressDialog::closeEvent( TQCloseEvent *e )
{
    emit canceled();
    TQDialog::closeEvent( e );
}

/*!
  \property TQProgressDialog::autoReset
  \brief whether the progress dialog calls reset() as soon as progress() equals totalSteps()

  The default is true.

  \sa setAutoClose()
*/

void TQProgressDialog::setAutoReset( bool b )
{
    d->autoReset = b;
}

bool TQProgressDialog::autoReset() const
{
    return d->autoReset;
}

/*!
  \property TQProgressDialog::autoClose
  \brief whether the dialog gets hidden by reset()

  The default is true.

  \sa setAutoReset()
*/

void TQProgressDialog::setAutoClose( bool b )
{
    d->autoClose = b;
}

bool TQProgressDialog::autoClose() const
{
    return d->autoClose;
}

/*!
  \reimp
*/

void TQProgressDialog::showEvent( TQShowEvent *e )
{
    TQDialog::showEvent( e );
    int w = TQMAX( isVisible() ? width() : 0, sizeHint().width() );
    int h = TQMAX( isVisible() ? height() : 0, sizeHint().height() );
    resize( w, h );
    forceTimer->stop();
}

/*!
  Shows the dialog if it is still hidden after the algorithm has been started
  and minimumDuration milliseconds have passed.

  \sa setMinimumDuration()
*/

void TQProgressDialog::forceShow()
{
    if ( d->shown_once || d->cancellation_flag )
	return;

    show();
    d->shown_once = true;
}


#endif
