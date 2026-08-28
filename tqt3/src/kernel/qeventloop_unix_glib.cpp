/**
** TQt->glib main event loop integration by Norbert Frese 2005
** code based on qeventloop_unix.cpp 3.3.5
**
*/

/****************************************************************************
** $Id: qt/qeventloop_unix_glib.cpp  
**
** Implementation of TQEventLoop class
**
** Copyright (C) 2000-2005 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.TQPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid TQt Enterprise Edition or TQt Professional Edition
** licenses for Unix/X11 or for TQt/Embedded may use this file in accordance
** with the TQt Commercial License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about TQt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for TQPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/


#include "qeventloop_glib_p.h" // includes qplatformdefs.h
#include "ntqeventloop.h"
#include "ntqapplication.h"
#include "ntqbitarray.h"
#include "ntqmutex.h"

#if defined(TQT_THREAD_SUPPORT)
  #include "ntqthread.h"
#endif

#include <stdlib.h>
#include <sys/types.h>

#include <glib.h>

#ifdef TQT_THREAD_SUPPORT
#ifdef QT_USE_GLIBMAINLOOP
extern TQMutex *tqt_timerListMutex;
#endif // QT_USE_GLIBMAINLOOP
#endif // TQT_THREAD_SUPPORT

/*****************************************************************************
  Timer handling; UNIX has no application timer support so we'll have to
  make our own from scratch.

  NOTE: These functions are for internal use. TQObject::startTimer() and
	TQObject::killTimer() are for public use.
	The TQTimer class provides a high-level interface which translates
	timer events into signals.

  qStartTimer( interval, obj )
	Starts a timer which will run until it is killed with qKillTimer()
	Arguments:
	    int interval	timer interval in milliseconds
	    TQObject *obj	where to send the timer event
	Returns:
	    int			timer identifier, or zero if not successful

  qKillTimer( timerId )
	Stops a timer specified by a timer identifier.
	Arguments:
	    int timerId		timer identifier
	Returns:
	    bool		true if successful

  qKillTimer( obj )
	Stops all timers that are sent to the specified object.
	Arguments:
	    TQObject *obj	object receiving timer events
	Returns:
	    bool		true if successful
 *****************************************************************************/

//
// Internal data structure for timers
//

struct TimerInfo {				// internal timer info
	int	 id;				// - timer identifier
	timeval  interval;			// - timer interval
	timeval  timeout;			// - when to sent event
	TQObject *obj;				// - object to receive event
};

typedef TQPtrList<TimerInfo> TimerList;		// list of TimerInfo structs

static TQBitArray *timerBitVec;			// timer bit vector
static TimerList  *timerList = 0;		// timer list

static void initTimers();
void cleanupTimers();
static timeval watchtime;			// watch if time is turned back
timeval *tqt_wait_timer();
timeval *tqt_wait_timer_max = 0;

//
// Internal operator functions for timevals
//

static inline bool operator<( const timeval &t1, const timeval &t2 )
{
	return (t1.tv_sec < t2.tv_sec) || ((t1.tv_sec == t2.tv_sec) && (t1.tv_usec < t2.tv_usec));
}

static inline bool operator==( const timeval &t1, const timeval &t2 )
{
	return t1.tv_sec == t2.tv_sec && t1.tv_usec == t2.tv_usec;
}

static inline timeval &operator+=( timeval &t1, const timeval &t2 )
{
	t1.tv_sec += t2.tv_sec;
	if ( (t1.tv_usec += t2.tv_usec) >= 1000000 ) {
		t1.tv_sec++;
		t1.tv_usec -= 1000000;
	}
	return t1;
}

static inline timeval operator+( const timeval &t1, const timeval &t2 )
{
	timeval tmp;
	tmp.tv_sec = t1.tv_sec + t2.tv_sec;
	if ( (tmp.tv_usec = t1.tv_usec + t2.tv_usec) >= 1000000 ) {
		tmp.tv_sec++;
		tmp.tv_usec -= 1000000;
	}
	return tmp;
}

static inline timeval operator-( const timeval &t1, const timeval &t2 )
{
	timeval tmp;
	tmp.tv_sec = t1.tv_sec - t2.tv_sec;
	if ( (tmp.tv_usec = t1.tv_usec - t2.tv_usec) < 0 ) {
		tmp.tv_sec--;
		tmp.tv_usec += 1000000;
	}
	return tmp;
}


//
// Internal functions for manipulating timer data structures.
// The timerBitVec array is used for keeping track of timer identifiers.
//

static int allocTimerId()			// find avail timer identifier
{
	int i = timerBitVec->size()-1;
	while ( i >= 0 && (*timerBitVec)[i] ) {
		i--;
	}
	if ( i < 0 ) {
		i = timerBitVec->size();
		timerBitVec->resize( 4 * i );
		for( int j=timerBitVec->size()-1; j > i; j-- ) {
			timerBitVec->clearBit( j );
		}
	}
	timerBitVec->setBit( i );
	return i+1;
}

static void insertTimer( const TimerInfo *ti )				// insert timer info into list
{
#if defined(TQT_THREAD_SUPPORT)
	tqt_timerListMutex->lock();
#endif
	TimerInfo *t = timerList->first();
	int index = 0;
#if defined(QT_DEBUG)
	int dangerCount = 0;
#endif
	while ( t && t->timeout < ti->timeout ) {			// list is sorted by timeout
#if defined(QT_DEBUG)
		if ( t->obj == ti->obj ) {
			dangerCount++;
		}
#endif
		t = timerList->next();
		index++;
	}
	timerList->insert( index, ti );					// inserts sorted
#if defined(QT_DEBUG)
	if ( dangerCount > 16 ) {
		tqDebug( "TQObject: %d timers now exist for object %s::%s", dangerCount, ti->obj->className(), ti->obj->name() );
	}
#endif
#if defined(TQT_THREAD_SUPPORT)
	tqt_timerListMutex->unlock();
#endif
}

static inline void getTime( timeval &t )				// get time of day
{
	gettimeofday( &t, 0 );
	while ( t.tv_usec >= 1000000 ) {				// NTP-related fix
		t.tv_usec -= 1000000;
		t.tv_sec++;
	}
	while ( t.tv_usec < 0 ) {
		if ( t.tv_sec > 0 ) {
			t.tv_usec += 1000000;
			t.tv_sec--;
		}
		else {
			t.tv_usec = 0;
			break;
		}
	}
}

static void repairTimer( const timeval &time )				// repair broken timer
{
#if defined(TQT_THREAD_SUPPORT)
	tqt_timerListMutex->lock();
#endif
	timeval diff = watchtime - time;
	TimerInfo *t = timerList->first();
	while ( t ) {							// repair all timers
		t->timeout = t->timeout - diff;
		t = timerList->next();
	}
#if defined(TQT_THREAD_SUPPORT)
	tqt_timerListMutex->unlock();
#endif
}

//
// Timer activation functions (called from the event loop)
//

/*
  Returns the time to wait for the next timer, or null if no timers are
  waiting.

  The result is bounded to tqt_wait_timer_max if this exists.
*/

timeval *tqt_wait_timer()
{
#if defined(TQT_THREAD_SUPPORT)
	tqt_timerListMutex->lock();
#endif
	static timeval tm;
	bool first = true;
	timeval currentTime;
	if ( timerList && timerList->count() ) {			// there are waiting timers
		getTime( currentTime );
		if ( first ) {
			if ( currentTime < watchtime ) {		// clock was turned back
				repairTimer( currentTime );
			}
			first = false;
			watchtime = currentTime;
		}
		TimerInfo *t = timerList->first();			// first waiting timer
		if ( currentTime < t->timeout ) {			// time to wait
			tm = t->timeout - currentTime;
		}
		else {
			tm.tv_sec  = 0;					// no time to wait
			tm.tv_usec = 0;
		}
		if ( tqt_wait_timer_max && *tqt_wait_timer_max < tm ) {
			tm = *tqt_wait_timer_max;
		}
#if defined(TQT_THREAD_SUPPORT)
		tqt_timerListMutex->unlock();
#endif
		return &tm;
	}
	if ( tqt_wait_timer_max ) {
		tm = *tqt_wait_timer_max;
#if defined(TQT_THREAD_SUPPORT)
		tqt_timerListMutex->unlock();
#endif
		return &tm;
	}
#if defined(TQT_THREAD_SUPPORT)
	tqt_timerListMutex->unlock();
#endif
	return 0;							// no timers
}

// Timer initialization
static void initTimers()						// initialize timers
{
	timerBitVec = new TQBitArray( 128 );
	TQ_CHECK_PTR( timerBitVec );
	int i = timerBitVec->size();
	while( i-- > 0 ) {
		timerBitVec->clearBit( i );
	}
	timerList = new TimerList;
#if defined(TQT_THREAD_SUPPORT)
	tqt_timerListMutex = new TQMutex(true);
#endif
	TQ_CHECK_PTR( timerList );
	timerList->setAutoDelete( true );
	gettimeofday( &watchtime, 0 );
}

// Timer cleanup
void cleanupTimers()
{
	delete timerList;
	timerList = 0;
	delete timerBitVec;
	timerBitVec = 0;
}

// Main timer functions for starting and killing timers
int qStartTimer( int interval, TQObject *obj )
{
#if defined(TQT_THREAD_SUPPORT)
	if (tqt_timerListMutex) tqt_timerListMutex->lock();
#endif
	if ( !timerList ) {						// initialize timer data
		initTimers();
#if defined(TQT_THREAD_SUPPORT)
		if (tqt_timerListMutex) tqt_timerListMutex->lock();
#endif
	}
	int id = allocTimerId();					// get free timer id
	if ( (id <= 0) || (id > (int)timerBitVec->size()) || (!obj) ) {	// cannot create timer
#if defined(TQT_THREAD_SUPPORT)
		if (tqt_timerListMutex) tqt_timerListMutex->unlock();
#endif
		return 0;
	}
	timerBitVec->setBit( id-1 );					// set timer active
	TimerInfo *t = new TimerInfo;					// create timer
	TQ_CHECK_PTR( t );
	t->id = id;
	t->interval.tv_sec  = interval/1000;
	t->interval.tv_usec = (interval%1000)*1000;
	timeval currentTime;
	getTime( currentTime );
	t->timeout = currentTime + t->interval;
	t->obj = obj;
	insertTimer( t );						// put timer in list
#if defined(TQT_THREAD_SUPPORT)
	if (tqt_timerListMutex) tqt_timerListMutex->unlock();
#endif
	return id;
}

bool qKillTimer( int id )
{
#if defined(TQT_THREAD_SUPPORT)
	if (tqt_timerListMutex) tqt_timerListMutex->lock();
#endif
	TimerInfo *t;
	if ( (!timerList) || (id <= 0) || (id > (int)timerBitVec->size()) || (!timerBitVec->testBit( id-1 )) ) {
#if defined(TQT_THREAD_SUPPORT)
		if (tqt_timerListMutex) tqt_timerListMutex->unlock();
#endif
		return false;						// not init'd or invalid timer
	}
	t = timerList->first();
	while ( t && t->id != id ) {					// find timer info in list
		t = timerList->next();
	}
	if ( t ) {							// id found
		bool ret;
		timerBitVec->clearBit( id-1 );				// set timer inactive
		ret = timerList->remove();
#if defined(TQT_THREAD_SUPPORT)
		if (tqt_timerListMutex) tqt_timerListMutex->unlock();
#endif
		return ret;
	}
	else {								// id not found
#if defined(TQT_THREAD_SUPPORT)
		if (tqt_timerListMutex) tqt_timerListMutex->unlock();
#endif
		return false;
	}
}

bool qKillTimer( TQObject *obj )
{
#if defined(TQT_THREAD_SUPPORT)
	if (tqt_timerListMutex) tqt_timerListMutex->lock();
#endif
	TimerInfo *t;
	if ( !timerList ) {						// not initialized
#if defined(TQT_THREAD_SUPPORT)
		if (tqt_timerListMutex) tqt_timerListMutex->unlock();
#endif
		return false;
	}
	t = timerList->first();
	while ( t ) {							// check all timers
		if ( t->obj == obj ) {					// object found
			timerBitVec->clearBit( t->id-1 );
			timerList->remove();
			t = timerList->current();
		}
		else {
			t = timerList->next();
		}
	}
#if defined(TQT_THREAD_SUPPORT)
	if (tqt_timerListMutex) tqt_timerListMutex->unlock();
#endif
	return true;
}


TQEventLoopPrivate::TQEventLoopPrivate() {
#if defined(TQ_WS_X11)
	xfd = -1;
	x_gPollFD.fd = -1;
	x_gPollFD.events = 0;
	x_gPollFD.revents = 0;
#endif // TQ_WS_X11
	gSource = nullptr;
	singletoolkit = true;
	ctx = nullptr;
	mainloop = nullptr;
	ctx_is_default = false;
	reset();
}

TQEventLoopPrivate::~TQEventLoopPrivate() {
	//
}

void TQEventLoopPrivate::reset() {
	looplevel = 0;
	quitcode = 0;
	quitnow = false;
	exitloop = false;
	shortcut = false;
}


/*****************************************************************************
 TQEventLoop implementations for Glib-Main-Loop
 *****************************************************************************/
 
 
 
 
void TQEventLoop::registerSocketNotifier( TQSocketNotifier *notifier )
{
    int sockfd = notifier->socket();
    int type = notifier->type();
    if ( sockfd < 0 || type < 0 || type > 2 || notifier == 0 ) {
#if defined(QT_CHECK_RANGE)
         tqWarning( "TQSocketNotifier: Internal error" );
#endif
         return;
    }

#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("register socket notifier %d\n", sockfd);
#endif
	
    TQPtrList<TQSockNotGPollFD>  *list = &d->sn_list;
    TQSockNotGPollFD *sn;

	/*
	if ( ! list ) {
		// create new list, the TQSockNotType destructor will delete it for us
		list = new TQPtrList<TQSockNot>;
		TQ_CHECK_PTR( list );
		list->setAutoDelete( true );
		d->sn_list = list;
	}
	*/
	
	gushort events=0;
	switch (type) {
		case 0: 
			events = G_IO_IN | G_IO_HUP;
			break;
		case 1:
			events = G_IO_OUT;
			break;
		case 2:
			events = G_IO_PRI | G_IO_ERR | G_IO_NVAL;
			break;
	}
	
	sn = new TQSockNotGPollFD;
	TQ_CHECK_PTR( sn );
	sn->obj = notifier;
	sn->gPollFD.fd = sockfd;
	sn->gPollFD.events = events; 
	sn->events = events; // save events!
	sn->pending = false;
	
	list->append( sn );
	
	g_source_add_poll(d->gSource, &sn->gPollFD); 
	
}

void TQEventLoop::unregisterSocketNotifier( TQSocketNotifier *notifier )
{
    int sockfd = notifier->socket();
    int type = notifier->type();
    if ( sockfd < 0 || type < 0 || type > 2 || notifier == 0 ) {
#if defined(QT_CHECK_RANGE)
         tqWarning( "TQSocketNotifier: Internal error" );
#endif
         return;
    }
	
#ifdef DEBUG_QT_GLIBMAINLOOP 
	printf("unregister socket notifier %d\n", sockfd);
#endif	

	TQPtrList<TQSockNotGPollFD> *list = &d->sn_list;
	TQSockNotGPollFD *sn;
	if ( ! list ) {
		return;
	}
	sn = list->first();
	while ( sn && !(sn->obj == notifier) ) {
		sn = list->next();
	}
	if ( !sn ) {					// not found
		return;
	}
	
	d->sn_pending_list.removeRef( sn );	
	list->remove();					// remove notifier found above
	g_source_remove_poll(d->gSource, &sn->gPollFD);
	
	delete sn;					// we don't autodelete - lets do it manually
	
}

void TQEventLoop::setSocketNotifierPending( TQSocketNotifier *notifier )
{
    int sockfd = notifier->socket();
    int type = notifier->type();
    if ( sockfd < 0 || type < 0 || type > 2 || notifier == 0 ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQSocketNotifier: Internal error" );
#endif
	return;
    }
	
#ifdef DEBUG_QT_GLIBMAINLOOP 
	printf("set socket notifier pending %d\n", sockfd);
#endif	

    TQPtrList<TQSockNotGPollFD> *list = &d->sn_list;
    TQSockNotGPollFD *sn;
    if ( ! list )
        return;
    sn = list->first();
    while ( sn && !(sn->obj == notifier) )
        sn = list->next();
    if ( ! sn ) { // not found
        return;
    }

    // We choose a random activation order to be more fair under high load.
    // If a constant order is used and a peer early in the list can
    // saturate the IO, it might grab our attention completely.
    // Also, if we're using a straight list, the callback routines may
    // delete other entries from the list before those other entries are
    // processed.
    if ( !sn->pending ) {
         d->sn_pending_list.insert( (rand() & 0xff) %
                 (d->sn_pending_list.count()+1), sn );
	     sn->pending = true;  // add it only once!
    }
}

void TQEventLoop::wakeUp()
{
    /*
      Apparently, there is not consistency among different operating
      systems on how to use FIONREAD.

      FreeBSD, Linux and Solaris all expect the 3rd argument to
      ioctl() to be an int, which is normally 32-bit even on 64-bit
      machines.

      IRIX, on the other hand, expects a size_t, which is 64-bit on
      64-bit machines.

      So, the solution is to use size_t initialized to zero to make
      sure all bits are set to zero, preventing underflow with the
      FreeBSD/Linux/Solaris ioctls.
    */
    size_t nbytes = 0;
    char c = 0;
    if ( ::ioctl( d->thread_pipe[0], FIONREAD, (char*)&nbytes ) >= 0 && nbytes == 0 ) {
	if (::write(  d->thread_pipe[1], &c, 1  ) < 0) {
		// Failed!
	}
    }
}

int TQEventLoop::timeToWait() const
{
	timeval *tm = tqt_wait_timer();
	if ( !tm ) {			// no active timers
		return -1;
	}
	return (tm->tv_sec*1000) + (tm->tv_usec/1000);
}

int TQEventLoop::activateTimers()
{
#if defined(TQT_THREAD_SUPPORT)
	if (tqt_timerListMutex) tqt_timerListMutex->lock();
#endif
	if ( !timerList || !timerList->count() ) {		// no timers
#if defined(TQT_THREAD_SUPPORT)
		if (tqt_timerListMutex) tqt_timerListMutex->unlock();
#endif
		return 0;
	}
	bool first = true;
	timeval currentTime;
	int n_act = 0, maxCount = timerList->count();
	TimerInfo *begin = 0;
	TimerInfo *t;
	
	for ( ;; ) {
		if ( ! maxCount-- ) {
			break;
		}
		getTime( currentTime );				// get current time
		if ( first ) {
			if ( currentTime < watchtime ) {	// clock was turned back
				repairTimer( currentTime );
			}
			first = false;
			watchtime = currentTime;
		}
		t = timerList->first();
		if ( !t || currentTime < t->timeout ) {		// no timer has expired
			break;
		}
		if ( ! begin ) {
			begin = t;
		}
		else if ( begin == t ) {
			// avoid sending the same timer multiple times
			break;
		}
		else if ( t->interval <  begin->interval || t->interval == begin->interval ) {
			begin = t;
		}
		timerList->take();				// unlink from list
		t->timeout += t->interval;
		if ( t->timeout < currentTime ) {
			t->timeout = currentTime + t->interval;
		}
		insertTimer( t );				// relink timer
		if ( t->interval.tv_usec > 0 || t->interval.tv_sec > 0 ) {
			n_act++;
		}
#if defined(TQT_THREAD_SUPPORT)
		if (tqt_timerListMutex) tqt_timerListMutex->unlock();
#endif
		TQTimerEvent e( t->id );
#if defined(TQT_THREAD_SUPPORT)
		// Be careful...the current thread may not be the target object's thread!
		if ((!t->obj) || 
		    (TQThread::currentThreadObject() && TQThread::currentThreadObject()->threadPostedEventsDisabled()) ||
		    (t->obj && t->obj->contextThreadObject() == TQThread::currentThreadObject())) {
			TQApplication::sendEvent( t->obj, &e );				// send event
		}
		else {
			TQApplication::postEvent( t->obj, new TQTimerEvent(e) );	// post event to correct thread
		}
#else // defined(TQT_THREAD_SUPPORT)
		TQApplication::sendEvent( t->obj, &e );					// send event
#endif // defined(TQT_THREAD_SUPPORT)
#if defined(TQT_THREAD_SUPPORT)
		if (tqt_timerListMutex) tqt_timerListMutex->lock();
#endif
		if ( timerList->findRef( begin ) == -1 ) {
			begin = 0;
		}
	}
#if defined(TQT_THREAD_SUPPORT)
	if (tqt_timerListMutex) tqt_timerListMutex->unlock();
#endif
	return n_act;
}

int TQEventLoop::activateSocketNotifiers()
{
    if ( d->sn_pending_list.isEmpty() ) {
	return 0;
    }

    // activate entries
    int n_act = 0;
    TQEvent event( TQEvent::SockAct );
    TQPtrListIterator<TQSockNotGPollFD> it( d->sn_pending_list );
    TQSockNotGPollFD *sn;
    while ( (sn=it.current()) ) {
        ++it;
        d->sn_pending_list.removeRef( sn );
        if ( sn->pending ) {	
#ifdef DEBUG_QT_GLIBMAINLOOP 
		printf("activate sn  : send event fd=%d\n", sn->gPollFD.fd );
#endif	
		sn->pending = false;
#if defined(TQT_THREAD_SUPPORT)
		// Be careful...the current thread may not be the target object's thread!
		if ((!sn->obj) || 
		    (TQThread::currentThreadObject() && TQThread::currentThreadObject()->threadPostedEventsDisabled()) ||
		    (sn->obj && sn->obj->contextThreadObject() == TQThread::currentThreadObject())) {
			TQApplication::sendEvent( sn->obj, &event );			// send event
		}
		else {
			TQApplication::postEvent( sn->obj, new TQEvent(event) );	// post event to correct thread
		}
#else // defined(TQT_THREAD_SUPPORT)
		TQApplication::sendEvent( sn->obj, &event );				// send event
#endif // defined(TQT_THREAD_SUPPORT)
		n_act++;
	}
    }

    return n_act;
}
