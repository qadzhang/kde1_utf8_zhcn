/****************************************************************************
**
** Implementation of TQEventLoop class
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#include "qeventloop_p.h" // includes qplatformdefs.h
#include "ntqeventloop.h"
#include "ntqapplication.h"
#include "ntqdatetime.h"

#ifdef TQT_THREAD_SUPPORT
#  include "ntqthread.h"
#  include "private/qthreadinstance_p.h"
#endif

/*!
    \class TQEventLoop
    \brief The TQEventLoop class manages the event queue.

    \ingroup application
    \ingroup events

    It receives events from the window system and other sources.  It
    then sends them to TQApplication for processing and delivery.

    TQEventLoop allows the application programmer to have more control
    over event delivery.  Programs that perform long operations can
    call either processOneEvent() or processEvents() with various
    ProcessEvent values OR'ed together to control which events should
    be delivered.

    TQEventLoop also allows the integration of an external event loop
    with the TQt event loop.  The Motif Extension included with TQt
    includes a reimplementation of TQEventLoop for merging TQt and Motif
    events together.

    To use your own instance of TQEventLoop or TQEventLoop subclass create 
    it before you create the TQApplication object.
*/

/*! \enum TQEventLoop::ProcessEvents

    This enum controls the types of events processed by the
    processEvents() functions.

    \value AllEvents - All events are processed
    \value ExcludeUserInput - Do not process user input events.
           ( ButtonPress, KeyPress, etc. )
    \value ExcludeSocketNotifiers - Do not process socket notifier
           events.
    \value WaitForMore - Wait for events if no pending events
           are available.
    \value ExcludeTimers - Do not process X11 timer events.

    \sa processEvents()
*/

/*! \enum TQEventLoop::ProcessEventsFlags
    A \c typedef to allow various ProcessEvents values to be OR'ed together.

    \sa ProcessEvents
 */

/*!
    Creates a TQEventLoop object, this object becomes the global event loop object.
    There can only be one event loop object. The TQEventLoop is usually constructed 
    by calling TQApplication::eventLoop(). To create your own event loop object create 
    it before you instantiate the TQApplication object.
    
    The \a parent and \a name arguments are passed on to the TQObject constructor.
*/
TQEventLoop::TQEventLoop( TQObject *parent, const char *name )
    : TQObject( parent, name )
{
#if defined(QT_CHECK_STATE)
    if ( TQApplication::currentEventLoop() )
	tqFatal( "TQEventLoop: there must be only one event loop object per thread. \nIf this is supposed to be the main GUI event loop, construct it before TQApplication." );
#ifdef TQT_THREAD_SUPPORT
    if (!TQThread::currentThreadObject()) {
	tqFatal( "TQEventLoop: this object can only be used in threads constructed via TQThread." );
    }
#endif // TQT_THREAD_SUPPORT
#endif // QT_CHECK_STATE

    d = new TQEventLoopPrivate;

    init();

#ifdef TQT_THREAD_SUPPORT
    TQThread* thread = TQThread::currentThreadObject();
    if (thread) {
        if (thread->d) {
            thread->d->eventLoop = this;
        }
    }
#else
    TQApplication::eventloop = this;
#endif
}

/*!
    Destructs the TQEventLoop object.
*/
TQEventLoop::~TQEventLoop()
{
    cleanup();
    delete d;
#ifdef TQT_THREAD_SUPPORT
    TQThread* thread = TQThread::currentThreadObject();
    if (thread) {
        if (thread->d) {
            thread->d->eventLoop = 0;
        }
    }
#else
    TQApplication::eventloop = 0;
#endif
}

/*!
    Enters the main event loop and waits until exit() is called, and
    returns the value that was set to exit().

    It is necessary to call this function to start event handling. The
    main event loop receives events from the window system and
    dispatches these to the application widgets.

    Generally speaking, no user interaction can take place before
    calling exec(). As a special case, modal widgets like TQMessageBox
    can be used before calling exec(), because modal widgets call
    exec() to start a local event loop.

    To make your application perform idle processing, i.e. executing a
    special function whenever there are no pending events, use a
    TQTimer with 0 timeout. More advanced idle processing schemes can
    be achieved using processEvents().

    \sa TQApplication::quit(), exit(), processEvents()
*/
int TQEventLoop::exec()
{
    d->reset();

    enterLoop();

    // cleanup
    d->looplevel = 0;
    d->quitnow  = false;
    d->exitloop = false;
    d->shortcut = false;
    // don't reset quitcode!

    return d->quitcode;
}

/*! \fn void TQEventLoop::exit( int retcode = 0 )

    Tells the event loop to exit with a return code.

    After this function has been called, the event loop returns from
    the call to exec(). The exec() function returns \a retcode.

    By convention, a \a retcode of 0 means success, and any non-zero
    value indicates an error.

    Note that unlike the C library function of the same name, this
    function \e does return to the caller -- it is event processing that
    stops.

    \sa TQApplication::quit(), exec()
*/
void TQEventLoop::exit( int retcode )
{
    if ( d->quitnow ) // preserve existing quitcode
	return;
    d->quitcode = retcode;
    d->quitnow  = true;
    d->exitloop = true;
    d->shortcut = true;
}


/*! \fn int TQEventLoop::enterLoop()

    This function enters the main event loop (recursively). Do not call
    it unless you really know what you are doing.
 */
int TQEventLoop::enterLoop()
{
    // save the current exitloop state
    bool old_exitloop = d->exitloop;
    d->exitloop = false;
    d->shortcut = false;

    d->looplevel++;
    while ( ! d->exitloop ) {
	processEvents( AllEvents | WaitForMore );
    }
    d->looplevel--;

    // restore the exitloop state, but if quitnow is true, we need to keep
    // exitloop set so that all other event loops drop out.
    d->exitloop = old_exitloop || d->quitnow;
    d->shortcut = d->quitnow;

    if ( d->looplevel < 1 ) {
	d->quitnow  = false;
	d->exitloop = false;
	d->shortcut = false;
	emit tqApp->aboutToQuit();

	// send deferred deletes
	TQApplication::sendPostedEvents( 0, TQEvent::DeferredDelete );
    }

    return d->looplevel;
}

/*! \fn void TQEventLoop::exitLoop()

    This function exits from a recursive call to the main event loop.
    Do not call it unless you really know what you are doing.
*/
void TQEventLoop::exitLoop()
{
    d->exitloop = true;
    d->shortcut = true;
}

/*! \fn void TQEventLoop::loopLevel() const

    Returns the current loop level.
*/
int TQEventLoop::loopLevel() const
{
    return d->looplevel;
}

/*!
    Process pending events that match \a flags for a maximum of \a
    maxTime milliseconds, or until there are no more events to
    process, which ever is shorter.

    This function is especially useful if you have a long running
    operation and want to show its progress without allowing user
    input, i.e. by using the \c ExcludeUserInput flag.

    NOTE: This function will not process events continuously; it
    returns after all available events are processed.

    NOTE: Specifying the \c WaitForMore flag makes no sense and will
    be ignored.
*/
void TQEventLoop::processEvents( ProcessEventsFlags flags, int maxTime )
{
    TQTime start = TQTime::currentTime();
    TQTime now;
    while ( ! d->quitnow && processEvents( flags & ~WaitForMore ) ) {
	now = TQTime::currentTime();
	if ( start.msecsTo( now ) > maxTime )
	    break;
    }
}

/*!
    \fn bool TQEventLoop::processEvents( ProcessEventsFlags flags )
    \overload

    Processes pending events that match \a flags until there are no
    more events to process.

    This function is especially useful if you have a long running
    operation and want to show its progress without allowing user
    input, i.e. by using the \c ExcludeUserInput flag.

    If the \c WaitForMore flag is set in \a flags, the behavior of
    this function is as follows:

    \list

    \i If events are available, this function returns after processing
    them.

    \i If no events are available, this function will wait until more
    are available and return after processing newly available events.

    \endlist

    If the \c WaitForMore flag is \e not set in \a flags, and no
    events are available, this function will return immediately.

    NOTE: This function will not process events continuously; it
    returns after all available events are processed.

    This function returns true if an event was processed; otherwise it
    returns false.

    \sa ProcessEvents hasPendingEvents()
*/

/*! \fn bool TQEventLoop::hasPendingEvents() const

    Returns true if there is an event waiting, otherwise it returns false.
*/

/*! \fn void TQEventLoop::registerSocketNotifier( TQSocketNotifier *notifier )

    Registers \a notifier with the event loop.  Subclasses need to
    reimplement this method to tie a socket notifier into another
    event loop.  Reimplementations \e MUST call the base
    implementation.
*/

/*! \fn void TQEventLoop::unregisterSocketNotifier( TQSocketNotifier *notifier )

    Unregisters \a notifier from the event loop.  Subclasses need to
    reimplement this method to tie a socket notifier into another
    event loop.  Reimplementations \e MUST call the base
    implementation.
*/

/*! \fn void TQEventLoop::setSocketNotifierPending( TQSocketNotifier *notifier )

    Marks \a notifier as pending.  The socket notifier will be
    activated the next time activateSocketNotifiers() is called.
*/

/*! \fn int TQEventLoop::activateSocketNotifiers()

    Activates all pending socket notifiers and returns the number of
    socket notifiers that were activated.
*/

/*! \fn int TQEventLoop::activateTimers()

    Activates all TQt timers and returns the number of timers that were
    activated.

    TQEventLoop subclasses that do their own timer handling need to
    call this after the time returned by timeToWait() has elapsed.

    Note: This function is only useful on systems where \c select() is
    used to block the eventloop.  On Windows, this function always
    returns 0.  On MacOS X, this function always returns 0 when the
    GUI is enabled.  On MacOS X, this function returns the documented
    value when the GUI is disabled.
*/

/*! \fn int TQEventLoop::timeToWait() const

    Returns the number of milliseconds that TQt needs to handle its
    timers or -1 if there are no timers running.

    TQEventLoop subclasses that do their own timer handling need to use
    this to make sure that TQt's timers continue to work.

    Note: This function is only useful on systems where \c select() is
    used to block the eventloop.  On Windows, this function always
    returns -1.  On MacOS X, this function always returns -1 when the
    GUI is enabled.  On MacOS X, this function returns the documented
    value when the GUI is disabled.
*/

/*! \fn void TQEventLoop::wakeUp()
    \threadsafe

    Wakes up the event loop.

    \sa awake()
*/

/*! \fn void TQEventLoop::awake()

    This signal is emitted after the event loop returns from a
    function that could block.

    \sa wakeUp() aboutToBlock()
*/

/*! \fn void TQEventLoop::aboutToBlock()

    This signal is emitted before the event loop calls a function that
    could block.

    \sa awake()
*/

#if !defined(TQ_WS_X11)
void TQEventLoop::appStartingUp(){}
void TQEventLoop::appClosingDown(){}
#endif // TQ_WS_X11
