/****************************************************************************
**
** TQThread class for Unix
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

#if defined(TQT_THREAD_SUPPORT)

#include "qplatformdefs.h"

typedef pthread_mutex_t Q_MUTEX_T;

#include "ntqthread.h"
#include <private/qthreadinstance_p.h>
#include <private/qmutex_p.h>
#include <private/qmutexpool_p.h>
#include <ntqthreadstorage.h>
#include <ntqapplication.h>

#include <errno.h>
#include <sched.h>

#if defined(QT_USE_GLIBMAINLOOP)
#include <glib.h>
#endif // QT_USE_GLIBMAINLOOP

static TQMutexPool *tqt_thread_mutexpool = 0;

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

typedef void*(*TQtThreadCallback)(void*);

static pthread_once_t storage_key_once = PTHREAD_ONCE_INIT;
static pthread_key_t storage_key;
static void create_storage_key()
{
    pthread_key_create( &storage_key, NULL );
}

#if defined(Q_C_CALLBACKS)
}
#endif


/**************************************************************************
 ** TQThreadInstance
 *************************************************************************/

void TQThreadInstance::setCurrentThread(TQThread *thread)
{
    pthread_once(&storage_key_once, create_storage_key);
    pthread_setspecific(storage_key, thread);
}

TQThreadInstance *TQThreadInstance::current()
{
    TQThreadInstance *ret = NULL;
    pthread_once( &storage_key_once, create_storage_key );
    TQThread *thread = (TQThread *) pthread_getspecific( storage_key );
    if (thread) {
        ret = thread->d;
    }
    return ret;
}

void TQThreadInstance::init(unsigned int stackSize)
{
    stacksize = stackSize;
    args[0] = args[1] = 0;
    thread_storage = 0;
    finished = false;
    running = false;
    orphan = false;
    disableThreadPostedEvents = false;

    pthread_cond_init(&thread_done, nullptr);
    thread_id = 0;

    eventLoop = 0;
    cleanupType = TQThread::CleanupMergeObjects;

    // threads have not been initialized yet, do it now
    if (! tqt_thread_mutexpool) TQThread::initialize();
}

void TQThreadInstance::deinit()
{
    pthread_cond_destroy(&thread_done);
}

void *TQThreadInstance::start( void *_arg )
{
    void **arg = (void **) _arg;

#if defined(QT_USE_GLIBMAINLOOP)
    // This is the first time we have access to the native pthread ID of this newly created thread
    ((TQThreadInstance*)arg[1])->thread_id = pthread_self();
    pthread_detach(pthread_self());
#endif // QT_USE_GLIBMAINLOOP

#ifdef QT_DEBUG
    tqDebug("TQThreadInstance::start: Setting thread storage to %p\n", (TQThread *) arg[0]);
#endif // QT_DEBUG
    setCurrentThread( (TQThread *) arg[0] );

    pthread_cleanup_push( TQThreadInstance::finish, arg[1] );
    pthread_testcancel();

    ( (TQThread *) arg[0] )->run();

    pthread_cleanup_pop( true );
    return 0;
}

void TQThreadInstance::finish( void * )
{
    TQThreadInstance *d = current();

    if ( ! d ) {
#ifdef QT_CHECK_STATE
	tqWarning( "TQThread: internal error: zero data for running thread." );
#endif // QT_CHECK_STATE
	return;
    }

#ifdef QT_DEBUG
    tqDebug("TQThreadInstance::finish: In TQThreadInstance::finish for thread %p\n", (TQThread*)d->args[0]);
#endif // QT_DEBUG

    TQApplication::threadTerminationHandler((TQThread*)d->args[0]);

    TQMutexLocker locker( d->mutex() );
    d->running = false;
    d->finished = true;
    d->args[0] = d->args[1] = 0;


    TQThreadStorageData::finish( d->thread_storage );
    d->thread_storage = 0;

    d->thread_id = 0;
    pthread_cond_broadcast(&d->thread_done);

    if (d->orphan) {
        d->deinit();
	delete d;
    }
}

void TQThreadInstance::finishGuiThread(TQThreadInstance *d) {
    TQThreadStorageData::finish( d->thread_storage );
    d->thread_storage = 0;
}

TQMutex *TQThreadInstance::mutex() const
{
    return tqt_thread_mutexpool ? tqt_thread_mutexpool->get( (void *) this ) : 0;
}

void TQThreadInstance::terminate()
{
    if ( ! thread_id ) return;
    pthread_cancel( thread_id );
}

/**************************************************************************
 ** TQThread
 *************************************************************************/

/*!
    This returns the thread handle of the currently executing thread.

    \warning The handle returned by this function is used for internal
    purposes and should \e not be used in any application code. On
    Windows, the returned value is a pseudo handle for the current
    thread, and it cannot be used for numerical comparison.
*/
TQt::HANDLE TQThread::currentThread()
{
    return (HANDLE) pthread_self();
}

/*! \internal
  Initializes the TQThread system.
*/
void TQThread::initialize()
{
    if ( ! tqt_global_mutexpool )
	tqt_global_mutexpool = new TQMutexPool( true, 73 );
    if ( ! tqt_thread_mutexpool )
	tqt_thread_mutexpool = new TQMutexPool( false, 127 );
}

/*! \internal
  Cleans up the TQThread system.
*/
void TQThread::cleanup()
{
    delete tqt_global_mutexpool;
    delete tqt_thread_mutexpool;
    tqt_global_mutexpool = 0;
    tqt_thread_mutexpool = 0;
}

/*!
    Ends the execution of the calling thread and wakes up any threads
    waiting for its termination.
*/
void TQThread::exit()
{
    pthread_exit( 0 );
}

/*  \internal
    helper function to do thread sleeps, since usleep()/nanosleep()
    aren't reliable enough (in terms of behavior and availability)
*/
static void thread_sleep( struct timespec *ti )
{
    pthread_mutex_t mtx;
    pthread_cond_t cnd;

    pthread_mutex_init(&mtx, 0);
    pthread_cond_init(&cnd, 0);

    pthread_mutex_lock( &mtx );
    (void) pthread_cond_timedwait( &cnd, &mtx, ti );
    pthread_mutex_unlock( &mtx );

    pthread_cond_destroy( &cnd );
    pthread_mutex_destroy( &mtx );
}

/*!
    System independent sleep. This causes the current thread to sleep
    for \a secs seconds.
*/
void TQThread::sleep( unsigned long secs )
{
    struct timeval tv;
    gettimeofday( &tv, 0 );
    struct timespec ti;
    ti.tv_sec = tv.tv_sec + secs;
    ti.tv_nsec = ( tv.tv_usec * 1000 );
    thread_sleep( &ti );
}

/*!
    System independent sleep. This causes the current thread to sleep
    for \a msecs milliseconds
*/
void TQThread::msleep( unsigned long msecs )
{
    struct timeval tv;
    gettimeofday( &tv, 0 );
    struct timespec ti;

    ti.tv_nsec = ( tv.tv_usec + ( msecs % 1000 ) * 1000 ) * 1000;
    ti.tv_sec = tv.tv_sec + ( msecs / 1000 ) + ( ti.tv_nsec / 1000000000 );
    ti.tv_nsec %= 1000000000;
    thread_sleep( &ti );
}

/*!
    System independent sleep. This causes the current thread to sleep
    for \a usecs microseconds
*/
void TQThread::usleep( unsigned long usecs )
{
    struct timeval tv;
    gettimeofday( &tv, 0 );
    struct timespec ti;

    ti.tv_nsec = ( tv.tv_usec + ( usecs % 1000000 ) ) * 1000;
    ti.tv_sec = tv.tv_sec + ( usecs / 1000000 ) + ( ti.tv_nsec / 1000000000 );
    ti.tv_nsec %= 1000000000;
    thread_sleep( &ti );
}

/*!
    Begins execution of the thread by calling run(), which should be
    reimplemented in a TQThread subclass to contain your code.  The
    operating system will schedule the thread according to the \a
    priority argument.

    If you try to start a thread that is already running, this
    function will wait until the the thread has finished and then
    restart the thread.

    \sa Priority
*/
void TQThread::start(Priority priority)
{
    TQMutexLocker locker( d->mutex() );
    while ( d->running )
    {
	pthread_cond_wait(&d->thread_done, &locker.mutex()->d->handle);
    }

    d->running = true;
    d->finished = false;

    int ret;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING) && (_POSIX_THREAD_PRIORITY_SCHEDULING-0 >= 0)
#if _POSIX_THREAD_PRIORITY_SCHEDULING == 0 && defined _SC_THREAD_PRIORITY_SCHEDULING
    if (sysconf(_SC_THREAD_PRIORITY_SCHEDULING) > 0)
#endif
    switch (priority) {
    case InheritPriority:
	{
	    pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
	    break;
	}

    default:
	{
	    int sched_policy;
	    if (pthread_attr_getschedpolicy(&attr, &sched_policy) != 0) {
                // failed to get the scheduling policy, don't bother
                // setting the priority
                tqWarning("TQThread: cannot determine default scheduler policy");
                break;
	    }

	    int prio_min = sched_get_priority_min(sched_policy);
	    int prio_max = sched_get_priority_max(sched_policy);
            if (prio_min == -1 || prio_max == -1) {
                // failed to get the scheduling parameters, don't
                // bother setting the priority
                tqWarning("TQThread: cannot determine scheduler priority range");
                break;
            }

	    int prio;
	    switch (priority) {
	    case IdlePriority:
		prio = prio_min;
		break;

	    case HighestPriority:
		prio = prio_max;
		break;

	    default:
		// crudely scale our priority enum values to the prio_min/prio_max
		prio = (((prio_max - prio_min) / TimeCriticalPriority) *
			priority) + prio_min;
		prio = TQMAX(prio_min, TQMIN(prio_max, prio));
		break;
	    }

	    sched_param sp;
	    sp.sched_priority = prio;

	    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	    pthread_attr_setschedparam(&attr, &sp);
	    break;
	}
    }
#endif // _POSIX_THREAD_PRIORITY_SCHEDULING

    if ( d->stacksize > 0 ) {
#if defined(_POSIX_THREAD_ATTR_STACKSIZE) && (_POSIX_THREAD_ATTR_STACKSIZE-0 > 0)
	ret = pthread_attr_setstacksize( &attr, d->stacksize );
#else
	ret = ENOSYS; // stack size not supported, automatically fail
#endif // _POSIX_THREAD_ATTR_STACKSIZE

	if ( ret ) {
#ifdef QT_CHECK_STATE
	    tqWarning( "TQThread::start: thread stack size error: %s", strerror( ret ) ) ;
#endif // QT_CHECK_STATE

	    // we failed to set the stacksize, and as the documentation states,
	    // the thread will fail to run...
	    d->running = false;
	    d->finished = false;
	    return;
	}
    }

    d->args[0] = this;
    d->args[1] = d;
#if defined(QT_USE_GLIBMAINLOOP)
    // The correct thread_id is set in TQThreadInstance::start using the value of d->args[1]
    d->thread_id = 0;

    // glib versions < 2.32.0 requires threading system initialization call
    #if GLIB_CHECK_VERSION(2, 32, 0)
        GThread* glib_thread_handle = g_thread_new( NULL, (GThreadFunc)TQThreadInstance::start, d->args );
    #else
        if( !g_thread_get_initialized() );
            g_thread_init(NULL);
        GThread* glib_thread_handle = g_thread_create((GThreadFunc)TQThreadInstance::start, d->args, false, NULL);
    #endif

    if (glib_thread_handle) {
	ret = 0;
    }
    else {
	ret = -1;
    }
#else // QT_USE_GLIBMAINLOOP
    ret = pthread_create( &d->thread_id, &attr, (TQtThreadCallback)TQThreadInstance::start, d->args );
    pthread_attr_destroy( &attr );
#endif // QT_USE_GLIBMAINLOOP

    if ( ret ) {
#ifdef QT_CHECK_STATE
	tqWarning( "TQThread::start: thread creation error: %s", strerror( ret ) );
#endif // QT_CHECK_STATE

	d->running = false;
	d->finished = false;
	d->args[0] = d->args[1] = 0;
    }
}

void TQThread::start()
{
    start(InheritPriority);
}

/*!
    A thread calling this function will block until either of these
    conditions is met:

    \list
    \i The thread associated with this TQThread object has finished
       execution (i.e. when it returns from \l{run()}). This function
       will return true if the thread has finished. It also returns
       true if the thread has not been started yet.
    \i \a time milliseconds has elapsed. If \a time is ULONG_MAX (the
    	default), then the wait will never timeout (the thread must
	return from \l{run()}). This function will return false if the
	wait timed out.
    \endlist

    This provides similar functionality to the POSIX \c pthread_join() function.
*/
bool TQThread::wait( unsigned long time )
{
    TQMutexLocker locker( d->mutex() );

    if ( pthread_equal( d->thread_id, pthread_self() ) ) {
#ifdef QT_CHECK_STATE
	tqWarning( "TQThread::wait: thread tried to wait on itself" );
#endif // QT_CHECK_STATE

	return false;
    }

    if ( d->finished || ! d->running ) {
	return true;
    }

    int ret;
    if (time != ULONG_MAX) {
	struct timeval tv;
	gettimeofday(&tv, 0);

	timespec ti;
	ti.tv_nsec = (tv.tv_usec + (time % 1000) * 1000) * 1000;
	ti.tv_sec = tv.tv_sec + (time / 1000) + (ti.tv_nsec / 1000000000);
	ti.tv_nsec %= 1000000000;

	ret = pthread_cond_timedwait(&d->thread_done, &locker.mutex()->d->handle, &ti);
    }
    else {
	ret = pthread_cond_wait(&d->thread_done, &locker.mutex()->d->handle);
    }

#ifdef QT_CHECK_RANGE
    if (ret && ret != ETIMEDOUT) {
	tqWarning("Wait condition wait failure: %s",strerror(ret));
    }
#endif

    return (ret == 0);
}

/*!
    Returns the current cleanup behaviour of the thread.

    \sa setCleanupType
    \sa CleanupType
*/

TQThread::CleanupType TQThread::cleanupType() const {
    return (TQThread::CleanupType)d->cleanupType;
}

/*!
    Sets the current cleanup behaviour of the thread.  The default,
    TQThread::CleanupMergeObjects, will merge any objects owned by this thread
    with the main GUI thread when this thread is terminated.

    If faster thread termination performance is desired, TQThread::CleanupNone
    may be specified instead.  However, this is not recommended as any objects
    owned by this thread on termination can then cause events to become "stuck"
    in the global event queue, leading to high CPU usage and other undesirable
    behavior.  You have been warned!

    \sa cleanupType
    \sa CleanupType
*/

void TQThread::setCleanupType(CleanupType type) {
    d->cleanupType = type;
}

/*!
    Returns a pointer to the currently executing TQThread.  If the
    current thread was not started using the TQThread API, this
    function returns zero.

    Note that TQApplication creates a TQThread object to represent the
    main thread; calling this function from main() after creating
    TQApplication will return a valid pointer.
*/
TQThread *TQThread::currentThreadObject()
{
    pthread_once(&storage_key_once, create_storage_key);
    return reinterpret_cast<TQThread *>(pthread_getspecific(storage_key));
}


#endif // TQT_THREAD_SUPPORT
