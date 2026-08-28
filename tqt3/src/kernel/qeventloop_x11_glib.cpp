/**
** TQt->glib main event loop integration by Norbert Frese 2005
** code based on qeventloop_x11.cpp 3.3.5
**
*/

/****************************************************************************
** $Id: qt/qeventloop_x11_glib.cpp
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
** licenses for Unix/X11 may use this file in accordance with the TQt Commercial
** License Agreement provided with the Software.
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
#include "qcolor_p.h"
#include "qt_x11_p.h"

#if defined(TQT_THREAD_SUPPORT)
#  include "ntqmutex.h"
#  include "ntqthread.h"
#endif // TQT_THREAD_SUPPORT

#include <errno.h>

#include <glib.h>
#include <sys/syscall.h>

// #define DEBUG_QT_GLIBMAINLOOP 1

// TQt-GSource Structure and Callbacks

typedef struct {
	GSource source;
	TQEventLoop * qeventLoop;
} TQtGSource;

static gboolean tqt_gsource_prepare  ( GSource *source, gint *timeout );
static gboolean tqt_gsource_check    ( GSource *source );
static gboolean tqt_gsource_dispatch ( GSource *source, GSourceFunc  callback, gpointer user_data );

static GSourceFuncs tqt_gsource_funcs = {
	tqt_gsource_prepare,
	tqt_gsource_check,
	tqt_gsource_dispatch,
	NULL,
	NULL,
	NULL
};

// forward main loop callbacks to TQEventLoop methods!

static gboolean tqt_gsource_prepare ( GSource *source, gint *timeout )
{
	TQtGSource * qtGSource = (TQtGSource*) source;
	TQEventLoop* candidateEventLoop = qtGSource->qeventLoop;
	TQEventLoop* activeThreadEventLoop = TQApplication::eventLoop();

	if (candidateEventLoop == activeThreadEventLoop) {
		return candidateEventLoop->gsourcePrepare(source, timeout);
	}
	else {
		// Prepare failed
		return FALSE;
	}
}

static gboolean tqt_gsource_check ( GSource *source )
{
	TQtGSource * qtGSource = (TQtGSource*) source;
	TQEventLoop* candidateEventLoop = qtGSource->qeventLoop;
	TQEventLoop* activeThreadEventLoop = TQApplication::eventLoop();

	if (candidateEventLoop == activeThreadEventLoop) {
		return candidateEventLoop->gsourceCheck(source);
	}
	else {
		// Check failed
		return FALSE;
	}
}

static gboolean tqt_gsource_dispatch ( GSource *source, GSourceFunc callback, gpointer user_data )
{
	Q_UNUSED(callback);
	Q_UNUSED(user_data);

	TQtGSource * qtGSource = (TQtGSource*) source;
	TQEventLoop* candidateEventLoop = qtGSource->qeventLoop;
	TQEventLoop* activeThreadEventLoop = TQApplication::eventLoop();

	if (candidateEventLoop == activeThreadEventLoop) {
		return candidateEventLoop->gsourceDispatch(source);
	}
	else {
		// Dispatch failed
		return FALSE;
	}
}


// -------------------------------------------------

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
	if ( !tqt_preselect_handler ) {
		tqt_preselect_handler = new TQVFuncList;
	}
	tqt_preselect_handler->append( handler );
}

void tqt_remove_preselect_handler( VFPTR handler )
{
	if ( tqt_preselect_handler ) {
		TQVFuncList::Iterator it = tqt_preselect_handler->find( handler );
		if ( it != tqt_preselect_handler->end() ) {
			tqt_preselect_handler->remove( it );
		}
	}
}

void tqt_install_postselect_handler( VFPTR handler )
{
	if ( !tqt_postselect_handler ) {
		tqt_postselect_handler = new TQVFuncList;
	}
	tqt_postselect_handler->prepend( handler );
}

void tqt_remove_postselect_handler( VFPTR handler )
{
	if ( tqt_postselect_handler ) {
		TQVFuncList::Iterator it = tqt_postselect_handler->find( handler );
		if ( it != tqt_postselect_handler->end() ) {
			tqt_postselect_handler->remove( it );
		}
	}
}

void TQEventLoop::init()
{
	// initialize ProcessEventFlags (all events & wait for more)
	d->pev_flags = AllEvents | WaitForMore;

	// initialize the common parts of the event loop
	if (pipe( d->thread_pipe ) < 0) {
		// Error!
	}
	fcntl(d->thread_pipe[0], F_SETFD, FD_CLOEXEC);
	fcntl(d->thread_pipe[1], F_SETFD, FD_CLOEXEC);

	// intitialize the X11 parts of the event loop
	d->xfd = -1;
	if ( tqt_is_gui_used && TQApplication::isGuiThread() ) {
		d->xfd = XConnectionNumber( TQPaintDevice::x11AppDisplay() );
	}

	// new main context for thread
	d->ctx = g_main_context_new();
	g_main_context_push_thread_default(d->ctx);
	d->ctx_is_default = true;

	// new GSource
	TQtGSource * qtGSource = (TQtGSource*) g_source_new(&tqt_gsource_funcs, sizeof(TQtGSource));
	g_source_set_can_recurse ((GSource*)qtGSource, TRUE);
	qtGSource->qeventLoop = this;

	// init main loop and attach gsource
#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("inside init(1)\n");
#endif

	d->mainloop = g_main_loop_new (d->ctx, 1);
	g_source_attach( (GSource*)qtGSource, d->ctx );
	d->gSource = (GSource*)qtGSource;

	// poll for X11 events
	if ( tqt_is_gui_used && TQApplication::isGuiThread() ) {
		d->x_gPollFD.fd = d->xfd;
		d->x_gPollFD.events = G_IO_IN | G_IO_HUP | G_IO_ERR;
		g_source_add_poll(d->gSource, &d->x_gPollFD);
	}

	// poll thread-pipe
	d->threadPipe_gPollFD.fd = d->thread_pipe[0];
	d->threadPipe_gPollFD.events = G_IO_IN | G_IO_HUP | G_IO_ERR;

	g_source_add_poll(d->gSource, &d->threadPipe_gPollFD);

#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("inside init(2)\n");
#endif
}

void TQEventLoop::cleanup()
{
	// cleanup the common parts of the event loop
	close( d->thread_pipe[0] );
	close( d->thread_pipe[1] );
	cleanupTimers();

	// cleanup the X11 parts of the event loop
	d->xfd = -1;

	// stop polling the GSource
	g_source_remove_poll(d->gSource, &d->threadPipe_gPollFD);
	g_source_remove_poll(d->gSource, &d->x_gPollFD);
	g_source_destroy(d->gSource);

	// unref the main loop
	g_main_loop_unref(d->mainloop);
	d->mainloop = nullptr;

	// unref the gsource
	g_source_unref(d->gSource);
	d->gSource = nullptr;

	// unref the main context
	g_main_context_unref(d->ctx);
	d->ctx = nullptr;
}

bool TQEventLoop::processEvents( ProcessEventsFlags flags )
{
#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("inside processEvents(1) looplevel=%d eventloop=%p d->ctx=%p this=%p\n", d->looplevel, TQApplication::eventLoop(), d->ctx, this ); fflush(stdout);
#endif

	ProcessEventsFlags save_flags;
	int rval;
	save_flags = d->pev_flags;

	d->pev_flags = flags;

	rval = g_main_context_iteration(d->ctx, flags & WaitForMore ? TRUE : FALSE);

	d->pev_flags = save_flags;

#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("inside processEvents(2) looplevel=%d eventloop=%p d->ctx=%p this=%p rval=%d\n", d->looplevel, TQApplication::eventLoop(), d->ctx, this, rval ); fflush(stdout);
#endif

	return rval; // were events processed?
}


bool TQEventLoop::processX11Events()
{
	ProcessEventsFlags flags = d->pev_flags;
	// process events from the X server
	XEvent event;
	int nevents = 0;

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
					if ( tqApp->x11ProcessEvent( &event ) == 1 ) {
						return true;
					}
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
	return false;
}


bool TQEventLoop::gsourcePrepare(GSource *gs, int * timeout)
{
	Q_UNUSED(gs);

#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("inside gsourcePrepare(1)\n");
#endif

	ProcessEventsFlags flags = d->pev_flags;

#if defined(TQT_THREAD_SUPPORT)
	TQMutexLocker locker( TQApplication::tqt_mutex );
#endif

	// don't block if exitLoop() or exit()/quit() has been called.
	bool canWait = d->exitloop || d->quitnow ? false : (flags & WaitForMore);

	// Process timers and socket notifiers - the common UNIX stuff

	// return the maximum time we can wait for an event.
	static timeval zerotm;
	timeval *tm = 0;
	if ( ! ( flags & ExcludeTimers ) ) {
		if (TQApplication::isGuiThread()) {
			tm = tqt_wait_timer();				// wait for TQt timers
			if ( !canWait ) {
				if ( !tm ) {
					tm = &zerotm;
				}
				tm->tv_sec  = 0;			// no time to wait
				tm->tv_usec = 0;
			}
		}
	}

	// Include or exclude SocketNotifiers (by setting or cleaning poll events)
	// Socket notifier events are only activated in the core application event loop,
	// so filter them out in other event loops.
	if (!(flags & ExcludeSocketNotifiers) && TQApplication::isGuiThread()) {
		TQPtrListIterator<TQSockNotGPollFD> it( d->sn_list );
		TQSockNotGPollFD *sn;
		while ( (sn=it.current()) ) {
			++it;
			sn->gPollFD.events = sn->events;  // restore poll events
		}
	}
	else {
		TQPtrListIterator<TQSockNotGPollFD> it( d->sn_list );
		TQSockNotGPollFD *sn;
		while ( (sn=it.current()) ) {
			++it;
			sn->gPollFD.events = 0;  // delete poll events
		}
	}

#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("inside gsourcePrepare(2) canwait=%d\n", canWait);
#endif

	if ( canWait ) {
		emit aboutToBlock();
	}

	if ( tqt_preselect_handler ) {
		TQVFuncList::Iterator it, end = tqt_preselect_handler->end();
		for ( it = tqt_preselect_handler->begin(); it != end; ++it )
			(**it)();
	}

#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("inside gsourcePrepare(2.1) canwait=%d\n", canWait);
#endif

	// do we have to dispatch events?
	if (hasPendingEvents()) {
		*timeout = 0; // no time to stay in poll
#ifdef DEBUG_QT_GLIBMAINLOOP
		printf("inside gsourcePrepare(3a)\n");
#endif
		return false;
	}

	// stay in poll until something happens?
	if (!tm) { // fixme
		*timeout = -1; // wait forever
#ifdef DEBUG_QT_GLIBMAINLOOP
		printf("inside gsourcePrepare(3b) timeout=%d \n", *timeout);
#endif
		return false;
	}

	// else timeout >=0
	*timeout = tm->tv_sec * 1000 + tm->tv_usec/1000;

#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("inside gsourcePrepare(3c) timeout=%d \n", *timeout);
#endif

	return false;
}


bool TQEventLoop::gsourceCheck(GSource *gs) {
	Q_UNUSED(gs);

#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("inside gsourceCheck(1)\n");
#endif

	ProcessEventsFlags flags = d->pev_flags;

	// Socketnotifier events?
	if (!(flags & ExcludeSocketNotifiers) && TQApplication::isGuiThread())
	{
		TQPtrListIterator<TQSockNotGPollFD> it( d->sn_list );
		TQSockNotGPollFD *sn;
		while ( (sn=it.current()) ) {
			++it;
			if ( sn->gPollFD.revents ) {
				return true;
			}
		}
	}

	if (d->x_gPollFD.revents) {
#ifdef DEBUG_QT_GLIBMAINLOOP
		printf("inside gsourceCheck(2) xfd!\n");
#endif

		return true;  // we got events!
	}
	if (d->threadPipe_gPollFD.revents) {
#ifdef DEBUG_QT_GLIBMAINLOOP
		printf("inside gsourceCheck(2) threadpipe!!\n");
#endif

		return true;  // we got events!
	}
	if (hasPendingEvents()) {
#ifdef DEBUG_QT_GLIBMAINLOOP
		printf("inside gsourceCheck(2) pendingEvents!\n");
#endif

		return true; // we got more X11 events!
	}

	// check if we have timers to activate?
	if (TQApplication::isGuiThread()) {
		timeval *tm = tqt_wait_timer();
		if (tm && (tm->tv_sec == 0 && tm->tv_usec == 0 )) {
#ifdef DEBUG_QT_GLIBMAINLOOP
			printf("inside gsourceCheck(2) qtwaittimer!\n");
#endif

			return true;
		}
	}

	// nothing to dispatch
#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("inside gsourceCheck(2) nothing to dispatch!\n");
#endif

	return false;
}


bool TQEventLoop::gsourceDispatch(GSource *gs) {
	Q_UNUSED(gs);

	// relock the GUI mutex before processing any pending events
#if defined(TQT_THREAD_SUPPORT)
	TQMutexLocker locker( TQApplication::tqt_mutex );
#endif
#if defined(TQT_THREAD_SUPPORT)
	if (locker.mutex()) locker.mutex()->lock();
#endif

	int nevents=0;
	ProcessEventsFlags flags = d->pev_flags;

#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("inside gsourceDispatch(1)\n");
#endif

	// we are awake, broadcast it
	emit awake();
	emit tqApp->guiThreadAwake();

	// some other thread woke us up... consume the data on the thread pipe so that
	// select doesn't immediately return next time

	if ( d->threadPipe_gPollFD.revents) {
		char c;
		if (::read( d->thread_pipe[0], &c, 1 ) < 0) {
			// Error!
		}
	}

	if ( tqt_postselect_handler ) {
		TQVFuncList::Iterator it, end = tqt_postselect_handler->end();
		for ( it = tqt_postselect_handler->begin(); it != end; ++it )
			(**it)();
	}

	// activate socket notifiers
	if ( ! ( flags & ExcludeSocketNotifiers )) {
		// if select says data is ready on any socket, then set the socket notifier
		// to pending
		// if ( &d->sn_list ) {
			TQPtrList<TQSockNotGPollFD> *list = &d->sn_list;
			TQSockNotGPollFD *sn = list->first();
			while ( sn ) {
				if ( sn->gPollFD.revents )
					setSocketNotifierPending( sn->obj );
				sn = list->next();
			}
		// }

		if (TQApplication::isGuiThread()) {
			nevents += activateSocketNotifiers();
		}
	}

	// activate timers
	if ( ! ( flags & ExcludeTimers ) ) {
		if (TQApplication::isGuiThread()) {
			nevents += activateTimers();
		}
	}

	// return true if we handled events, false otherwise
	//return (nevents > 0);

	// now process x11 events!
#ifdef DEBUG_QT_GLIBMAINLOOP
	printf("inside gsourceDispatch(2) hasPendingEvents=%d\n", hasPendingEvents());
#endif

	if (hasPendingEvents()) {
		// color approx. optimization - only on X11
		tqt_reset_color_avail();

#if defined(TQT_THREAD_SUPPORT)
		if (locker.mutex()) locker.mutex()->unlock();
#endif
		processX11Events();
	}
	else {
#if defined(TQT_THREAD_SUPPORT)
		if (locker.mutex()) locker.mutex()->unlock();
#endif
	}

	if (d->singletoolkit) {
		return true;	// Eat the event
	}
	else {
		return false;	// Pass the event on to GTK
	}
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
	if ( tqt_is_gui_used ) {
		d->xfd = XConnectionNumber( TQPaintDevice::x11AppDisplay() );
		if ( (d->x_gPollFD.fd == -1) && TQApplication::isGuiThread() ) {
			d->x_gPollFD.fd = d->xfd;
			d->x_gPollFD.events = G_IO_IN | G_IO_HUP | G_IO_ERR;
			g_source_add_poll(d->gSource, &d->x_gPollFD);
		}
	}
}

void TQEventLoop::appClosingDown()
{
	d->xfd = -1;
}

void TQEventLoop::setSingleToolkitEventHandling(bool enabled) {
	d->singletoolkit = enabled;

	if (!d->singletoolkit) {
		if (d->ctx_is_default) {
			d->ctx_is_default = false;
			g_main_context_pop_thread_default(d->ctx);
		}
	}
	else {
		if (!d->ctx_is_default) {
			g_main_context_push_thread_default(d->ctx);
			d->ctx_is_default = true;
		}
	}
}
