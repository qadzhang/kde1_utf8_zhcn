/****************************************************************************
**
** Implementation of TQEventLoop class
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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
#include "ntqbitarray.h"
#include "qcolor_p.h"
#include "qt_x11_p.h"

#if defined(TQT_THREAD_SUPPORT)
#  include "ntqmutex.h"
#endif // TQT_THREAD_SUPPORT

#include <errno.h>


// resolve the conflict between X11's FocusIn and TQEvent::FocusIn
#undef FocusOut
#undef FocusIn

static const int XKeyPress = KeyPress;
static const int XKeyRelease = KeyRelease;
#undef KeyPress
#undef KeyRelease

// from qapplication.cpp
extern bool tqt_is_gui_used;

// from qeventloop_unix.cpp
extern timeval *tqt_wait_timer();
extern void cleanupTimers();

// ### this needs to go away at some point...
typedef void (*VFPTR)();
typedef TQValueList<VFPTR> TQVFuncList;
void tqt_install_preselect_handler( VFPTR );
void tqt_remove_preselect_handler( VFPTR );
static TQVFuncList *tqt_preselect_handler = 0;
void tqt_install_postselect_handler( VFPTR );
void tqt_remove_postselect_handler( VFPTR );
static TQVFuncList *tqt_postselect_handler = 0;

void tqt_install_preselect_handler( VFPTR handler )
{
    if ( !tqt_preselect_handler )
	tqt_preselect_handler = new TQVFuncList;
    tqt_preselect_handler->append( handler );
}
void tqt_remove_preselect_handler( VFPTR handler )
{
    if ( tqt_preselect_handler ) {
	TQVFuncList::Iterator it = tqt_preselect_handler->find( handler );
	if ( it != tqt_preselect_handler->end() )
		tqt_preselect_handler->remove( it );
    }
}
void tqt_install_postselect_handler( VFPTR handler )
{
    if ( !tqt_postselect_handler )
	tqt_postselect_handler = new TQVFuncList;
    tqt_postselect_handler->prepend( handler );
}
void tqt_remove_postselect_handler( VFPTR handler )
{
    if ( tqt_postselect_handler ) {
	TQVFuncList::Iterator it = tqt_postselect_handler->find( handler );
	if ( it != tqt_postselect_handler->end() )
		tqt_postselect_handler->remove( it );
    }
}


void TQEventLoop::init()
{
    // initialize the common parts of the event loop
    pipe( d->thread_pipe );
    fcntl(d->thread_pipe[0], F_SETFD, FD_CLOEXEC);
    fcntl(d->thread_pipe[1], F_SETFD, FD_CLOEXEC);

    d->sn_highest = -1;

    // intitialize the X11 parts of the event loop
    d->xfd = -1;
    if ( tqt_is_gui_used )
        d->xfd = XConnectionNumber( TQPaintDevice::x11AppDisplay() );
}

void TQEventLoop::cleanup()
{
    // cleanup the common parts of the event loop
    close( d->thread_pipe[0] );
    close( d->thread_pipe[1] );
    cleanupTimers();

    // cleanup the X11 parts of the event loop
    d->xfd = -1;
}

bool TQEventLoop::processEvents( ProcessEventsFlags flags )
{
    // process events from the X server
    XEvent event;
    int	   nevents = 0;

#if defined(TQT_THREAD_SUPPORT)
    TQMutexLocker locker( TQApplication::tqt_mutex );
#endif

    // handle gui and posted events
    if ( tqt_is_gui_used ) {
	TQApplication::sendPostedEvents();

	if (TQApplication::isGuiThread()) {
		// Two loops so that posted events accumulate
		while ( XPending( TQPaintDevice::x11AppDisplay() ) ) {
			// also flushes output buffer
			while ( XPending( TQPaintDevice::x11AppDisplay() ) ) {
				if ( d->shortcut ) {
					return false;
				}
		
				XNextEvent( TQPaintDevice::x11AppDisplay(), &event );
		
				if ( flags & ExcludeUserInput ) {
					switch ( event.type ) {
						case ButtonPress:
						case ButtonRelease:
						case MotionNotify:
						case XKeyPress:
						case XKeyRelease:
						case EnterNotify:
						case LeaveNotify:
							continue;
				
						case ClientMessage:
							{
							// from qapplication_x11.cpp
							extern Atom tqt_wm_protocols;
							extern Atom tqt_wm_take_focus;
							extern Atom tqt_scrolldone;
				
							// only keep the wm_take_focus and
							// tqt_scrolldone protocols, discard all
							// other client messages
							if ( event.xclient.format != 32 )
								continue;
				
							if ( event.xclient.message_type == tqt_wm_protocols ||
								(Atom) event.xclient.data.l[0] == tqt_wm_take_focus )
								break;
							if ( event.xclient.message_type == tqt_scrolldone )
								break;
							}
				
						default: break;
					}
				}
		
				nevents++;
				if ( tqApp->x11ProcessEvent( &event ) == 1 )
				return true;
			}
		}
	}
    }

    if ( d->shortcut ) {
	return false;
    }

    TQApplication::sendPostedEvents();

    const uint exclude_all = ExcludeSocketNotifiers | ExcludeTimers;
    if ( nevents > 0 && ( flags & exclude_all ) == exclude_all && ( flags & WaitForMore ) ) {
	return true;
    }

    // don't block if exitLoop() or exit()/quit() has been called.
    bool canWait = d->exitloop || d->quitnow ? false : (flags & WaitForMore);

    // Process timers and socket notifiers - the common UNIX stuff

    // return the maximum time we can wait for an event.
    static timeval zerotm;
    timeval *tm = 0;
    if ( ! ( flags & ExcludeTimers ) ) {
	if (TQApplication::isGuiThread()) {
	    tm = tqt_wait_timer();			// wait for TQt timers
	    if ( !canWait ) {
		if ( !tm )
		    tm = &zerotm;
		tm->tv_sec  = 0;			// no time to wait
		tm->tv_usec = 0;
	    }
	}
    }

    // Include or exclude SocketNotifiers (by setting or cleaning poll events)
    // Socket notifier events are only activated in the core application event loop,
    // so filter them out in other event loops.
    int highest = 0;
    if (!(flags & ExcludeSocketNotifiers) && TQApplication::isGuiThread()) {
	// return the highest fd we can wait for input on
	if ( d->sn_highest >= 0 ) {			// has socket notifier(s)
	    if ( d->sn_vec[0].list && ! d->sn_vec[0].list->isEmpty() )
		d->sn_vec[0].select_fds = d->sn_vec[0].enabled_fds;
	    else
		FD_ZERO( &d->sn_vec[0].select_fds );

	    if ( d->sn_vec[1].list && ! d->sn_vec[1].list->isEmpty() )
		d->sn_vec[1].select_fds = d->sn_vec[1].enabled_fds;
	    else
		FD_ZERO( &d->sn_vec[1].select_fds );

	    if ( d->sn_vec[2].list && ! d->sn_vec[2].list->isEmpty() )
		d->sn_vec[2].select_fds = d->sn_vec[2].enabled_fds;
	    else
		FD_ZERO( &d->sn_vec[2].select_fds );
	} else {
	    FD_ZERO( &d->sn_vec[0].select_fds );

	    FD_ZERO( &d->sn_vec[1].select_fds );
	    FD_ZERO( &d->sn_vec[2].select_fds );
	}

	highest = d->sn_highest;
    } else {
        FD_ZERO( &d->sn_vec[0].select_fds );
	FD_ZERO( &d->sn_vec[1].select_fds );
	FD_ZERO( &d->sn_vec[2].select_fds );
    }

    if ( tqt_is_gui_used && TQApplication::isGuiThread() ) {
	// select for events on the event socket - only on X11
	FD_SET( d->xfd, &d->sn_vec[0].select_fds );
	highest = TQMAX( highest, d->xfd );
    }

    FD_SET( d->thread_pipe[0], &d->sn_vec[0].select_fds );
    highest = TQMAX( highest, d->thread_pipe[0] );

    if ( canWait )
	emit aboutToBlock();

    if ( tqt_preselect_handler ) {
	TQVFuncList::Iterator it, end = tqt_preselect_handler->end();
	for ( it = tqt_preselect_handler->begin(); it != end; ++it )
	    (**it)();
    }

    // unlock the GUI mutex and select.  when we return from this function, there is
    // something for us to do
#if defined(TQT_THREAD_SUPPORT)
    if ( locker.mutex() ) locker.mutex()->unlock();
    else return false;
#endif

    int nsel;
    do {
	nsel = select( highest + 1,
		       &d->sn_vec[0].select_fds,
		       &d->sn_vec[1].select_fds,
		       &d->sn_vec[2].select_fds,
		       tm );
    } while (nsel == -1 && (errno == EINTR || errno == EAGAIN));

    // relock the GUI mutex before processing any pending events
#if defined(TQT_THREAD_SUPPORT)
    if ( locker.mutex() ) locker.mutex()->lock();
    else return false;
#endif

    // we are awake, broadcast it
    emit awake();
    emit tqApp->guiThreadAwake();

    if (nsel == -1) {
	if (errno == EBADF) {
	    // it seems a socket notifier has a bad fd... find out
	    // which one it is and disable it
	    fd_set fdset;
	    zerotm.tv_sec = zerotm.tv_usec = 0l;

	    for (int type = 0; type < 3; ++type) {
		TQPtrList<TQSockNot> *list = d->sn_vec[type].list;
		if (!list) continue;

		TQSockNot *sn = list->first();
		while (sn) {
		    FD_ZERO(&fdset);
		    FD_SET(sn->fd, &fdset);

		    int ret = -1;
		    do {
			switch (type) {
			case 0: // read
			    ret = select(sn->fd + 1, &fdset, 0, 0, &zerotm);
			    break;
			case 1: // write
			    ret = select(sn->fd + 1, 0, &fdset, 0, &zerotm);
			    break;
			case 2: // except
			    ret = select(sn->fd + 1, 0, 0, &fdset, &zerotm);
			    break;
			}
		    } while (ret == -1 && (errno == EINTR || errno == EAGAIN));

		    if (ret == -1 && errno == EBADF) {
			// disable the invalid socket notifier
			static const char *t[] = { "Read", "Write", "Exception" };
			tqWarning("TQSocketNotifier: invalid socket %d and type '%s', disabling...",
				 sn->fd, t[type]);
			sn->obj->setEnabled(false);
		    }

		    sn = list->next();
		}
	    }
	} else {
	    // EINVAL... shouldn't happen, so let's complain to stderr
	    // and hope someone sends us a bug report
	    perror( "select" );
	}
    }

    // some other thread woke us up... consume the data on the thread pipe so that
    // select doesn't immediately return next time
    if ( nsel > 0 && FD_ISSET( d->thread_pipe[0], &d->sn_vec[0].select_fds ) ) {
	char c;
	::read( d->thread_pipe[0], &c, 1 );
    }

    if ( tqt_postselect_handler ) {
	TQVFuncList::Iterator it, end = tqt_postselect_handler->end();
	for ( it = tqt_postselect_handler->begin(); it != end; ++it )
	    (**it)();
    }

    // activate socket notifiers
    if (!(flags & ExcludeSocketNotifiers) && nsel > 0 && d->sn_highest >= 0 &&
	TQApplication::isGuiThread()) {
	// if select says data is ready on any socket, then set the socket notifier
	// to pending
	int i;
	for ( i=0; i<3; i++ ) {
	    if ( ! d->sn_vec[i].list )
		continue;

	    TQPtrList<TQSockNot> *list = d->sn_vec[i].list;
	    TQSockNot *sn = list->first();
	    while ( sn ) {
		if ( FD_ISSET( sn->fd, &d->sn_vec[i].select_fds ) )
		    setSocketNotifierPending( sn->obj );
		sn = list->next();
	    }
	}

	nevents += activateSocketNotifiers();
    }

    // activate timers
    if ( ! ( flags & ExcludeTimers ) ) {
	if (TQApplication::isGuiThread()) {
	    nevents += activateTimers();
	}
    }

    // color approx. optimization - only on X11
    tqt_reset_color_avail();

    // return true if we handled events, false otherwise
    return (nevents > 0);
}

bool TQEventLoop::hasPendingEvents() const
{
#ifdef TQT_THREAD_SUPPORT
	TQMutexLocker locker( TQApplication::tqt_mutex );
#endif // TQT_THREAD_SUPPORT

    extern uint qGlobalPostedEventsCount(); // from qapplication.cpp
    return ( qGlobalPostedEventsCount() || ( (tqt_is_gui_used && TQApplication::isGuiThread())  ? XPending( TQPaintDevice::x11AppDisplay() ) : 0));
}

void TQEventLoop::appStartingUp()
{
    if ( tqt_is_gui_used )
        d->xfd = XConnectionNumber( TQPaintDevice::x11AppDisplay() );
}

void TQEventLoop::appClosingDown()
{
    d->xfd = -1;
}

void TQEventLoop::setSingleToolkitEventHandling(bool enabled) {
    // Do nothing
}
