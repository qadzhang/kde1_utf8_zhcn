/****************************************************************************
**
** ...
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the tools module of the TQt GUI Toolkit.
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

#include "ntqapplication.h"
#include "ntqthread.h"
#include "qplatformdefs.h"

#include "ntqthreadstorage.h"
#include <private/qthreadinstance_p.h>

#include <string.h>

// #define TQTHREADSTORAGE_DEBUG


// keep this in sync with the implementation in qthreadstorage.cpp
static const int MAX_THREAD_STORAGE = 257; // 256 maximum + 1 used in TQRegExp

static pthread_mutex_t thread_storage_mutex = PTHREAD_MUTEX_INITIALIZER;

static bool thread_storage_init = false;
static struct {
    bool used;
    void (*func)( void * );
} thread_storage_usage[MAX_THREAD_STORAGE];


TQThreadStorageData::TQThreadStorageData( void (*func)( void * ) )
    : id( 0 )
{
    pthread_mutex_lock( &thread_storage_mutex );

    // make sure things are initialized
    if ( ! thread_storage_init )
	memset( thread_storage_usage, 0, sizeof( thread_storage_usage ) );
    thread_storage_init = true;

    for ( ; id < MAX_THREAD_STORAGE; ++id ) {
	if ( !thread_storage_usage[id].used )
	    break;
    }

    Q_ASSERT( id >= 0 && id < MAX_THREAD_STORAGE );
    thread_storage_usage[id].used = true;
    thread_storage_usage[id].func = func;

#ifdef TQTHREADSTORAGE_DEBUG
    tqDebug( "TQThreadStorageData: allocated id %d", id );
#endif // TQTHREADSTORAGE_DEBUG

    pthread_mutex_unlock( &thread_storage_mutex );
}

TQThreadStorageData::~TQThreadStorageData()
{
    // The Gui thread has static storage duration, TQThreadStorage are almost always static (it's
    // technically possible to allocate those in the heap, but it's quite unusual). It's impossible
    // to predict whichever of those one gets destroyed first, but usually it's a TQThreadStorage.
    // In that case we have to do the cleanup of its storage ourself as it won't be possible after
    // nullifying the destructor below.
    TQThread *guiThread = TQApplication::guiThread();
    if (guiThread) {
	TQThreadInstance *d = guiThread->d;
	TQMutexLocker locker( d->mutex() );
	if (d->thread_storage && d->thread_storage[id]) {
	    thread_storage_usage[id].func( d->thread_storage[id] );
	    d->thread_storage[id] = nullptr;
	}
    }

    pthread_mutex_lock( &thread_storage_mutex );
    thread_storage_usage[id].used = false;
    thread_storage_usage[id].func = 0;

#ifdef TQTHREADSTORAGE_DEBUG
    tqDebug( "TQThreadStorageData: released id %d", id );
#endif // TQTHREADSTORAGE_DEBUG

    pthread_mutex_unlock( &thread_storage_mutex );
}

void **TQThreadStorageData::get() const
{
    TQThreadInstance *d = TQThreadInstance::current();
    if (!d) {
        tqWarning("TQThreadStorage can only be used with threads started with TQThread");
        return 0;
    }
    TQMutexLocker locker( d->mutex() );
    return d->thread_storage && d->thread_storage[id] ? &d->thread_storage[id] : 0;
}

void **TQThreadStorageData::set( void *p )
{
    TQThreadInstance *d = TQThreadInstance::current();
    if (!d) {
        tqWarning("TQThreadStorage can only be used with threads started with TQThread");
        return 0;
    }
    TQMutexLocker locker( d->mutex() );
    if ( !d->thread_storage ) {
#ifdef TQTHREADSTORAGE_DEBUG
	tqDebug( "TQThreadStorageData: allocating storage for thread %lx",
		(unsigned long) pthread_self() );
#endif // TQTHREADSTORAGE_DEBUG

	d->thread_storage = new void*[MAX_THREAD_STORAGE];
	memset( d->thread_storage, 0, sizeof( void* ) * MAX_THREAD_STORAGE );
    }

    // delete any previous data
    if ( d->thread_storage[id] )
	thread_storage_usage[id].func( d->thread_storage[id] );

    // store new data
    d->thread_storage[id] = p;
    return &d->thread_storage[id];
}

void TQThreadStorageData::finish( void **thread_storage )
{
    if ( ! thread_storage ) return; // nothing to do

#ifdef TQTHREADSTORAGE_DEBUG
    tqDebug( "TQThreadStorageData: destroying storage for thread %lx",
	    (unsigned long) pthread_self() );
#endif // TQTHREADSTORAGE_DEBUG

    for ( int i = 0; i < MAX_THREAD_STORAGE; ++i ) {
	if ( ! thread_storage[i] ) continue;
	if ( ! thread_storage_usage[i].used ) {
#ifdef QT_CHECK_STATE
	    tqWarning( "TQThreadStorage: thread %lx exited after TQThreadStorage destroyed",
		      (unsigned long) pthread_self() );
#endif // QT_CHECK_STATE
	    continue;
	}

	thread_storage_usage[i].func( thread_storage[i] );
    }

    delete [] thread_storage;
}


/*!
    \class TQThreadStorage
    \brief The TQThreadStorage class provides per-thread data storage.

    \threadsafe
    \ingroup thread
    \ingroup environment

    TQThreadStorage is a template class that provides per-thread data
    storage.

    \e{Note that due to compiler limitations, TQThreadStorage can only
    store pointers.}

    The setLocalData() function stores a single thread-specific value
    for the calling thread. The data can be accessed later using the
    localData() functions. TQThreadStorage takes ownership of the
    data (which must be created on the heap with \e new) and deletes
    it when the thread exits (either normally or via termination).

    The hasLocalData() function allows the programmer to determine if
    data has previously been set using the setLocalData() function.
    This is useful for lazy initializiation.

    For example, the following code uses TQThreadStorage to store a
    single cache for each thread that calls the \e cacheObject() and
    \e removeFromCache() functions. The cache is automatically
    deleted when the calling thread exits (either normally or via
    termination).

    \code
    TQThreadStorage<TQCache<SomeClass> *> caches;

    void cacheObject( const TQString &key, SomeClass *object )
    {
        if ( ! caches.hasLocalData() )
	    caches.setLocalData( new TQCache<SomeClass> );

	caches.localData()->insert( key, object );
    }

    void removeFromCache( const TQString &key )
    {
        if ( ! caches.hasLocalData() )
            return; // nothing to do

	caches.localData()->remove( key );
    }
    \endcode

    \section1 Caveats

    \list

    \i As noted above, TQThreadStorage can only store pointers due to
    compiler limitations. Support for value-based objects will be
    added when the majority of compilers are able to support partial
    template specialization.

    \i The \link ~TQThreadStorage() destructor\endlink does \e not
    delete per-thread data. TQThreadStorage only deletes per-thread
    data when the thread exits or when setLocalData() is called
    multiple times.

    \i TQThreadStorage can only be used with threads started with
    TQThread. It \e cannot be used with threads started with
    platform-specific APIs.

    \i As a corollary to the above, platform-specific APIs cannot be
    used to exit or terminate a TQThread using TQThreadStorage. Doing so
    will cause all per-thread data to be leaked. See TQThread::exit()
    and TQThread::terminate().

    \i TQThreadStorage \e can be used to store data for the \e main()
    thread \e after TQApplication has been constructed. TQThreadStorage
    deletes all data set for the \e main() thread when TQApplication is
    destroyed, regardless of whether or not the \e main() thread has
    actually finished.

    \i The implementation of TQThreadStorage limits the total number of
    TQThreadStorage objects to 256. An unlimited number of threads
    can store per-thread data in each TQThreadStorage object.

    \endlist
*/

/*!
    \fn TQThreadStorage::TQThreadStorage()

    Constructs a new per-thread data storage object.
*/

/*!
    \fn TQThreadStorage::~TQThreadStorage()

    Destroys the per-thread data storage object.

    Note: The per-thread data stored is \e not deleted. Any data left
    in TQThreadStorage is leaked. Make sure that all threads using
    TQThreadStorage have exited before deleting the TQThreadStorage.

    \sa hasLocalData()
*/

/*!
    \fn bool TQThreadStorage::hasLocalData() const

    Returns true if the calling thread has non-zero data available;
    otherwise returns false.

    \sa localData()
*/

/*!
    \fn T& TQThreadStorage::localData()

    Returns a reference to the data that was set by the calling
    thread.

    Note: TQThreadStorage can only store pointers. This function
    returns a \e reference to the pointer that was set by the calling
    thread. The value of this reference is 0 if no data was set by
    the calling thread,

    \sa hasLocalData()
*/
/*
  ### addition to the above documentation when we start supporting
  ### partial template specialization, and TQThreadStorage can store
  ### values *and* pointers

  When using TQThreadStorage to store values (not pointers), this
  function stores an object of type \e T (created with its default
  constructor) and returns a reference to that object.
*/

/*!
    \fn const T TQThreadStorage::localData() const
    \overload

    Returns a copy of the data that was set by the calling thread.

    Note: TQThreadStorage can only store pointers. This function
    returns a pointer to the data that was set by the calling thread.
    If no data was set by the calling thread, this function returns 0.

    \sa hasLocalData()
*/
/*
  ### addition to the above documentation when we start supporting
  ### partial template specialization, and TQThreadStorage can store
  ### values *and* pointers

  When using TQThreadStorage to store values (not pointers), this
  function returns an object of type \e T (created with its default
  constructor). Unlike the above function, this object is \e not
  stored automatically. You will need to call setLocalData() to store
  the object.
*/

/*!
    \fn void TQThreadStorage::setLocalData( T data )

    Sets the local data for the calling thread to \a data. It can be
    accessed later using the localData() functions.

    If \a data is 0, this function deletes the previous data (if
    any) and returns immediately.

    If \a data is non-zero, TQThreadStorage takes ownership of the \a
    data and deletes it automatically either when the thread exits
    (either normally or via termination) or when setLocalData() is
    called again.

    Note: TQThreadStorage can only store pointers. The \a data
    argument must be either a pointer to an object created on the heap
    (i.e. using \e new) or 0. You should not delete \a data
    yourself; TQThreadStorage takes ownership and will delete the \a
    data itself.

    \sa localData() hasLocalData()
*/

#endif // TQT_THREAD_SUPPORT
