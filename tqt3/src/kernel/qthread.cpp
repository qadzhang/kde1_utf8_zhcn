/****************************************************************************
**
** Cross-platform TQThread implementation.
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

#ifdef TQT_THREAD_SUPPORT

#include "qplatformdefs.h"

#include "ntqthread.h"
#include "ntqeventloop.h"
#include <private/qthreadinstance_p.h>

#ifndef QT_H
#  include "ntqapplication.h"
#endif // QT_H

#if TQT_VERSION >= 0x040000
#  error "Remove TQThread::TQThread() and TQThread::start()."
#endif


/*!
    \class TQThread ntqthread.h
    \threadsafe
    \brief The TQThread class provides platform-independent threads.

    \ingroup thread
    \ingroup environment

    A TQThread represents a separate thread of control within the
    program; it shares data with all the other threads within the
    process but executes independently in the way that a separate
    program does on a multitasking operating system. Instead of
    starting in main(), TQThreads begin executing in run(). You inherit
    run() to include your code. For example:

    \code
    class MyThread : public TQThread {

    public:

	virtual void run();

    };

    void MyThread::run()
    {
	for( int count = 0; count < 20; count++ ) {
	    sleep( 1 );
	    tqDebug( "Ping!" );
	}
    }

    int main()
    {
	MyThread a;
	MyThread b;
	a.start();
	b.start();
	a.wait();
	b.wait();
    }
    \endcode

    This will start two threads, each of which writes Ping! 20 times
    to the screen and exits. The wait() calls at the end of main() are
    necessary because exiting main() ends the program, unceremoniously
    killing all other threads. Each MyThread stops executing when it
    reaches the end of MyThread::run(), just as an application does
    when it leaves main().

    \sa \link threads.html Thread Support in TQt\endlink.
*/

/*!
    \enum TQThread::Priority

    This enum type indicates how the operating system should schedule
    newly created threads.

    \value IdlePriority scheduled only when no other threads are
           running.

    \value LowestPriority scheduled less often than LowPriority.
    \value LowPriority scheduled less often than NormalPriority.

    \value NormalPriority the default priority of the operating
           system.

    \value HighPriority scheduled more often than NormalPriority.
    \value HighestPriority scheduled more often then HighPriority.

    \value TimeCriticalPriority scheduled as often as possible.

    \value InheritPriority use the same priority as the creating
           thread.  This is the default.
*/

TQThread::TQThread()
{
#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker locker( TQApplication::tqt_mutex );
#endif // TQT_THREAD_SUPPORT

    d = new TQThreadInstance;
    d->init(0);
}

/*!
    Constructs a new thread. The thread does not begin executing until
    start() is called.

    If \a stackSize is greater than zero, the maximum stack size is
    set to \a stackSize bytes, otherwise the maximum stack size is
    automatically determined by the operating system.

    \warning Most operating systems place minimum and maximum limits
    on thread stack sizes. The thread will fail to start if the stack
    size is outside these limits.
*/
TQThread::TQThread( unsigned int stackSize )
{
    d = new TQThreadInstance;
    d->init(stackSize);
}

/*!
    TQThread destructor.

    Note that deleting a TQThread object will not stop the execution of
    the thread it represents. Deleting a running TQThread (i.e.
    finished() returns false) will probably result in a program crash.
    You can wait() on a thread to make sure that it has finished.
*/
TQThread::~TQThread()
{
    TQMutexLocker locker( d->mutex() );
    if ( d->running && !d->finished ) {
#ifdef QT_CHECK_STATE
	tqWarning("TQThread object destroyed while thread is still running.");
#endif

	d->orphan = true;
	return;
    }

    d->deinit();
    delete d;
}

/*!
    This function terminates the execution of the thread. The thread
    may or may not be terminated immediately, depending on the
    operating system's scheduling policies. Use TQThread::wait()
    after terminate() for synchronous termination.

    When the thread is terminated, all threads waiting for the
    the thread to finish will be woken up.

    \warning This function is dangerous, and its use is discouraged.
    The thread can be terminated at any point in its code path.  Threads
    can be terminated while modifying data.  There is no chance for
    the thread to cleanup after itself, unlock any held mutexes, etc.
    In short, use this function only if \e absolutely necessary.
*/
void TQThread::terminate()
{
    TQMutexLocker locker( d->mutex() );
    if ( d->finished || !d->running )
	return;
    d->terminate();
}

/*!
    Returns true if the thread is finished; otherwise returns false.
*/
bool TQThread::finished() const
{
    TQMutexLocker locker( d->mutex() );
    return d->finished;
}

/*!
    Returns true if the thread is running; otherwise returns false.
*/
bool TQThread::running() const
{
    TQMutexLocker locker( d->mutex() );
    return d->running;
}

/*!
    Changes the way cross thread signals are handled
    If disable is false, signals emitted from this thread will be
    posted to any other connected threads' event loops (default).

    If disable is true, calls to emit from this thread
    will immediately execute slots in another thread.
    This mode of operation is inherently unsafe and is provided
    solely to support thread management by a third party application.
 */
void TQThread::setThreadPostedEventsDisabled(bool disable)
{
    d->disableThreadPostedEvents = disable;
}

/*!
    Returns true if thread posted events are disabled, false if not
 */
bool TQThread::threadPostedEventsDisabled() const
{
    return d->disableThreadPostedEvents;
}

/*!
    \fn void TQThread::run()

    This method is pure virtual, and must be implemented in derived
    classes in order to do useful work. Returning from this method
    will end the execution of the thread.

    \sa wait()
*/

#ifndef TQT_NO_COMPAT
/*! \obsolete
    Use TQApplication::postEvent() instead.
*/
void TQThread::postEvent( TQObject * receiver, TQEvent * event )
{
    TQApplication::postEvent( receiver, event );
}
#endif

TQEventLoopThread::TQEventLoopThread() : TQThread()
{
    //
}

TQEventLoopThread::~TQEventLoopThread()
{
    //
}

void TQEventLoopThread::run()
{
    TQEventLoop* eventLoop = TQApplication::eventLoop();
    if (eventLoop) eventLoop->exec();
}

#endif // TQT_THREAD_SUPPORT
