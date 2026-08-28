/****************************************************************************
**
** Implementation of TQTimer class
**
** Created : 931111
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

#include "ntqtimer.h"
#include "ntqsignal.h"
#include "ntqobjectlist.h"

/*!
    \class TQTimer ntqtimer.h
    \brief The TQTimer class provides timer signals and single-shot timers.

    \ingroup time
    \ingroup events
    \mainclass

    It uses \link TQTimerEvent timer events\endlink internally to
    provide a more versatile timer. TQTimer is very easy to use:
    create a TQTimer, call start() to start it and connect its
    timeout() to the appropriate slots. When the time is up it will
    emit the timeout() signal.

    Note that a TQTimer object is destroyed automatically when its
    parent object is destroyed.

    Example:
    \code
	TQTimer *timer = new TQTimer( myObject );
	connect( timer, TQ_SIGNAL(timeout()), myObject, TQ_SLOT(timerDone()) );
	timer->start( 2000, true ); // 2 seconds single-shot timer
    \endcode

    You can also use the static singleShot() function to create a
    single shot timer.

    As a special case, a TQTimer with timeout 0 times out as soon as
    all the events in the window system's event queue have been
    processed.

    This can be used to do heavy work while providing a snappy
    user interface:
    \code
	TQTimer *t = new TQTimer( myObject );
	connect( t, TQ_SIGNAL(timeout()), TQ_SLOT(processOneThing()) );
	t->start( 0, false );
    \endcode

    myObject->processOneThing() will be called repeatedly and should
    return quickly (typically after processing one data item) so that
    TQt can deliver events to widgets and stop the timer as soon as it
    has done all its work. This is the traditional way of
    implementing heavy work in GUI applications; multi-threading is
    now becoming available on more and more platforms, and we expect
    that null events will eventually be replaced by threading.

    Note that TQTimer's accuracy depends on the underlying operating
    system and hardware. Most platforms support an accuracy of 20ms;
    some provide more. If TQt is unable to deliver the requested
    number of timer clicks, it will silently discard some.

    An alternative to using TQTimer is to call TQObject::startTimer()
    for your object and reimplement the TQObject::timerEvent() event
    handler in your class (which must, of course, inherit TQObject).
    The disadvantage is that timerEvent() does not support such
    high-level features as single-shot timers or signals.

    Some operating systems limit the number of timers that may be
    used; TQt tries to work around these limitations.
*/


static const int INV_TIMER = -1;		// invalid timer id


/*!
    Constructs a timer called \a name, with the parent \a parent.

    Note that the parent object's destructor will destroy this timer
    object.
*/

TQTimer::TQTimer( TQObject *parent, const char *name )
    : TQObject( parent, name ), id(INV_TIMER), single(0), nulltimer(0)
{
}

/*!
    Destroys the timer.
*/

TQTimer::~TQTimer()
{
    if ( id != INV_TIMER )			// stop running timer
	stop();
}


/*!
    \fn void TQTimer::timeout()

    This signal is emitted when the timer is activated.
*/

/*!
    \fn bool TQTimer::isActive() const

    Returns true if the timer is running (pending); otherwise returns
    false.
*/

/*!
    \fn int TQTimer::timerId() const

    Returns the ID of the timer if the timer is running; otherwise returns
    -1.
*/


/*!
    Starts the timer with a \a msec milliseconds timeout, and returns
    the ID of the timer, or zero when starting the timer failed.

    If \a sshot is true, the timer will be activated only once;
    otherwise it will continue until it is stopped.

    Any pending timer will be stopped.

    \sa singleShot() stop(), changeInterval(), isActive()
*/

int TQTimer::start( int msec, bool sshot )
{
    if ( id >=0 && nulltimer && !msec && sshot )
	return id;
    if ( id != INV_TIMER )			// stop running timer
	stop();
    single = sshot;
    nulltimer = ( !msec && sshot );
    return id = startTimer( msec );
}


/*!
    Changes the timeout interval to \a msec milliseconds.

    If the timer signal is pending, it will be stopped and restarted;
    otherwise it will be started.

    \sa start(), isActive()
*/

void TQTimer::changeInterval( int msec )
{
    if ( id == INV_TIMER ) {			// create new timer
	start( msec );
    } else {
	killTimer( id );			// restart timer
	id = startTimer( msec );
    }
}

/*!
    Stops the timer.

    \sa start()
*/

void TQTimer::stop()
{
    if ( id != INV_TIMER ) {
	killTimer( id );
	id = INV_TIMER;
    }
}


/*!
    \reimp
*/
bool TQTimer::event( TQEvent *e )
{
    if ( e->type() != TQEvent::Timer )		// ignore all other events
	return false;
    if ( single )				// stop single shot timer
	stop();
    emit timeout();				// emit timeout signal
    return true;
}


/*
  The TQSingleShotTimer class is an internal class for implementing
  TQTimer::singleShot(). It starts a timer and emits the signal
  and kills itself when it gets the timeout.
*/

static TQObjectList *sst_list = 0;		// list of single shot timers

static void sst_cleanup()
{
    if ( sst_list ) {
	sst_list->setAutoDelete( true );
	delete sst_list;
	sst_list = 0;
    }
}

static void sst_init()
{
    if ( !sst_list ) {
	sst_list = new TQObjectList;
	TQ_CHECK_PTR( sst_list );
	tqAddPostRoutine( sst_cleanup );
    }
}


class TQSingleShotTimer : public TQObject
{
public:
    ~TQSingleShotTimer();
    bool    start( int msec, TQObject *r, const char * m );
    bool    isActive() const { return timerId > 0; }
protected:
    bool    event( TQEvent * );
private:
    TQSignal signal;
    int	    timerId;
};

extern int  qStartTimer( int interval, TQObject *obj ); // implemented in qapp_xxx.cpp
extern bool qKillTimer( int id );

TQSingleShotTimer::~TQSingleShotTimer()
{
    if (timerId != 0) {
       qKillTimer(timerId);
       timerId = 0;
    }
}

bool TQSingleShotTimer::start( int msec, TQObject *r, const char *m )
{
    timerId = 0;
    if ( signal.connect(r, m) )
	timerId = qStartTimer( msec, (TQObject *)this );
    return timerId != 0;
}

bool TQSingleShotTimer::event( TQEvent * )
{
    qKillTimer( timerId );			// no more timeouts
    signal.activate();				// emit the signal
    signal.disconnect( 0, 0 );
    timerId = 0;                                // mark as inactive
    return true;
}


/*!
    This static function calls a slot after a given time interval.

    It is very convenient to use this function because you do not need
    to bother with a \link TQObject::timerEvent() timerEvent\endlink or
    to create a local TQTimer object.

    Example:
    \code
	#include <ntqapplication.h>
	#include <ntqtimer.h>

	int main( int argc, char **argv )
	{
	    TQApplication a( argc, argv );
	    TQTimer::singleShot( 10*60*1000, &a, TQ_SLOT(quit()) );
		... // create and show your widgets
	    return a.exec();
	}
    \endcode

    This sample program automatically terminates after 10 minutes (i.e.
    600000 milliseconds).

    The \a receiver is the receiving object and the \a member is the
    slot. The time interval is \a msec.
*/

void TQTimer::singleShot( int msec, TQObject *receiver, const char *member )
{
    if ( !sst_list )
	sst_init();
    // search the list for a free ss timer we could reuse
    TQSingleShotTimer *sst = (TQSingleShotTimer*)sst_list->first();
    while ( sst && sst->isActive() )
        sst = (TQSingleShotTimer*)sst_list->next();
    // create a new one if not successful
    if ( !sst ) {
	sst = new TQSingleShotTimer;
        sst_list->append( sst );
    }
    sst->start(msec, receiver, member);
}
