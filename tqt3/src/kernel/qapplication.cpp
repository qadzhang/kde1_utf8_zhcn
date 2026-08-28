/****************************************************************************
**
** Implementation of TQApplication class
**
** Created : 931107
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

#include "ntqobjectlist.h"
#include "ntqapplication.h"
#include "ntqeventloop.h"
#include "qeventloop_p.h"
#include "ntqwidget.h"
#include "ntqwidgetlist.h"
#include "ntqwidgetintdict.h"
#include "ntqptrdict.h"
#include "ntqcleanuphandler.h"

#include "ntqtranslator.h"
#include "ntqtextcodec.h"
#include "ntqsessionmanager.h"
#include "ntqdragobject.h"
#include "ntqclipboard.h"
#include "ntqcursor.h"
#include "ntqstyle.h"
#include "ntqstylefactory.h"
#include "ntqfile.h"
#include "ntqmessagebox.h"
#include "ntqdir.h"
#include "ntqfileinfo.h"
#ifdef TQ_WS_WIN
#include "qinputcontext_p.h"
#endif
#include "qfontdata_p.h"

#if defined(TQT_THREAD_SUPPORT)
#  include "ntqmutex.h"
#  include "ntqthread.h"
#  include <private/qthreadinstance_p.h>
#endif // TQT_THREAD_SUPPORT

#include <stdlib.h>

#ifdef truncate
# undef truncate
#endif

/*!
  \class TQApplication ntqapplication.h
  \brief The TQApplication class manages the GUI application's control
  flow and main settings.

  \ingroup application
  \mainclass

  It contains the main event loop, where all events from the window
  system and other sources are processed and dispatched. It also
  handles the application's initialization and finalization, and
  provides session management. It also handles most system-wide and
  application-wide settings.

  For any GUI application that uses TQt, there is precisely one
  TQApplication object, no matter whether the application has 0, 1, 2
  or more windows at any time.

  The TQApplication object is accessible through the global pointer \c
  tqApp. Its main areas of responsibility are:
  \list

  \i It initializes the application with the user's desktop settings
  such as palette(), font() and doubleClickInterval(). It keeps track
  of these properties in case the user changes the desktop globally, for
  example through some kind of control panel.

  \i It performs event handling, meaning that it receives events
  from the underlying window system and dispatches them to the relevant
  widgets. By using sendEvent() and postEvent() you can send your own
  events to widgets.

  \i It parses common command line arguments and sets its internal
  state accordingly. See the \link TQApplication::TQApplication()
  constructor documentation\endlink below for more details about this.

  \i It defines the application's look and feel, which is
  encapsulated in a TQStyle object. This can be changed at runtime
  with setStyle().

  \i It specifies how the application is to allocate colors.
  See setColorSpec() for details.

  \i It provides localization of strings that are visible to the user
  via translate().

  \i It provides some magical objects like the desktop() and the
  clipboard().

  \i It knows about the application's windows. You can ask which
  widget is at a certain position using widgetAt(), get a list of
  topLevelWidgets() and closeAllWindows(), etc.

  \i It manages the application's mouse cursor handling,
  see setOverrideCursor() and setGlobalMouseTracking().

  \i On the X window system, it provides functions to flush and sync
  the communication stream, see flushX() and syncX().

  \i It provides support for sophisticated \link
  session.html session management \endlink. This makes it possible
  for applications to terminate gracefully when the user logs out, to
  cancel a shutdown process if termination isn't possible and even to
  preserve the entire application's state for a future session. See
  isSessionRestored(), sessionId() and commitData() and saveState()
  for details.

  \endlist

  The <a href="simple-application.html">Application walk-through
  example</a> contains a typical complete main() that does the usual
  things with TQApplication.

  Since the TQApplication object does so much initialization, it
  <b>must</b> be created before any other objects related to the user
  interface are created.

  Since it also deals with common command line arguments, it is
  usually a good idea to create it \e before any interpretation or
  modification of \c argv is done in the application itself. (Note
  also that for X11, setMainWidget() may change the main widget
  according to the \c -geometry option. To preserve this
  functionality, you must set your defaults before setMainWidget() and
  any overrides after.)

  \table
    \header \i21 Groups of functions
    \row
     \i System settings
     \i
	desktopSettingsAware(),
	setDesktopSettingsAware(),
	cursorFlashTime(),
	setCursorFlashTime(),
	doubleClickInterval(),
	setDoubleClickInterval(),
	wheelScrollLines(),
	setWheelScrollLines(),
	palette(),
	setPalette(),
	font(),
	setFont(),
	fontMetrics().

    \row
     \i Event handling
     \i
	exec(),
	processEvents(),
	enter_loop(),
	exit_loop(),
	exit(),
	quit().
	sendEvent(),
	postEvent(),
	sendPostedEvents(),
	removePostedEvents(),
	hasPendingEvents(),
	notify(),
	macEventFilter(),
	qwsEventFilter(),
	x11EventFilter(),
	x11ProcessEvent(),
	winEventFilter().

    \row
     \i GUI Styles
     \i
	style(),
	setStyle(),
	polish().

    \row
     \i Color usage
     \i
	colorSpec(),
	setColorSpec(),
	qwsSetCustomColors().

    \row
     \i Text handling
     \i
	installTranslator(),
	removeTranslator()
	translate().

    \row
     \i Widgets
     \i
	mainWidget(),
	setMainWidget(),
	allWidgets(),
	topLevelWidgets(),
	desktop(),
	activePopupWidget(),
	activeModalWidget(),
	clipboard(),
	focusWidget(),
	winFocus(),
	activeWindow(),
	widgetAt().

    \row
     \i Advanced cursor handling
     \i
	hasGlobalMouseTracking(),
	setGlobalMouseTracking(),
	overrideCursor(),
	setOverrideCursor(),
	restoreOverrideCursor().

    \row
     \i X Window System synchronization
     \i
	flushX(),
	syncX().

    \row
     \i Session management
     \i
	isSessionRestored(),
	sessionId(),
	commitData(),
	saveState().

    \row
    \i Threading
    \i
	lock(), unlock(), locked(), tryLock(),
	wakeUpGuiThread()

    \row
     \i Miscellaneous
     \i
	closeAllWindows(),
	startingUp(),
	closingDown(),
	type().
  \endtable

  \e {Non-GUI programs:} While TQt is not optimized or
  designed for writing non-GUI programs, it's possible to use
  \link tools.html some of its classes \endlink without creating a
  TQApplication. This can be useful if you wish to share code between
  a non-GUI server and a GUI client.

  \headerfile ntqnamespace.h
  \headerfile ntqwindowdefs.h
  \headerfile ntqglobal.h
*/

/*! \enum TQt::HANDLE
    \internal
*/

/*!
    \enum TQApplication::Type

    \value Tty a console application
    \value GuiClient a GUI client application
    \value GuiServer a GUI server application
*/

/*!
    \enum TQApplication::ColorSpec

    \value NormalColor the default color allocation policy
    \value CustomColor the same as NormalColor for X11; allocates colors
    to a palette on demand under Windows
    \value ManyColor the right choice for applications that use thousands of
    colors

    See setColorSpec() for full details.
*/

/*
  The tqt_init() and tqt_cleanup() functions are implemented in the
  qapplication_xyz.cpp file.
*/

void tqt_init( int *, char **, TQApplication::Type );
void tqt_cleanup();
#if defined(TQ_WS_X11)
void tqt_init( Display* dpy, TQt::HANDLE, TQt::HANDLE );
void tqt_init( int *, char **, Display* dpy, TQt::HANDLE, TQt::HANDLE );
#endif
TQ_EXPORT bool tqt_tryModalHelper( TQWidget *widget, TQWidget **rettop );

TQApplication *tqApp = 0;			// global application object

TQStyle   *TQApplication::app_style      = 0;	// default application style
bool      tqt_explicit_app_style	       = false; // style explicitly set by programmer

int	  TQApplication::app_cspec      = TQApplication::NormalColor;
#ifndef TQT_NO_PALETTE
TQPalette *TQApplication::app_pal	       = 0;	// default application palette
#endif
TQFont	 *TQApplication::app_font       = 0;	// default application font
bool	  tqt_app_has_font	       = false;
#ifndef TQT_NO_CURSOR
TQCursor	 *TQApplication::app_cursor     = 0;	// default application cursor
#endif
int	  TQApplication::app_tracking   = 0;	// global mouse tracking
bool	  TQApplication::is_app_running = false;	// app starting up if false
bool	  TQApplication::is_app_closing = false;	// app closing down if true
int	  TQApplication::loop_level     = 0;	// event loop level
TQWidget	 *TQApplication::main_widget    = 0;	// main application widget
TQWidget	 *TQApplication::focus_widget   = 0;	// has keyboard input focus
TQWidget	 *TQApplication::active_window  = 0;	// toplevel with keyboard focus
bool	  TQApplication::obey_desktop_settings = true;	// use winsys resources
int	  TQApplication::cursor_flash_time = 1000;	// text caret flash time
int	  TQApplication::mouse_double_click_time = 400;	// mouse dbl click limit
#ifndef TQT_NO_WHEELEVENT
int	  TQApplication::wheel_scroll_lines = 3;		// number of lines to scroll
#endif
bool	  tqt_is_gui_used;
bool      TQ_EXPORT tqt_resolve_symlinks = true;
bool      TQ_EXPORT tqt_tab_all_widgets  = true;
TQRect tqt_maxWindowRect;
static int drag_time = 500;
static int drag_distance = 4;
static bool reverse_layout = false;
TQSize     TQApplication::app_strut	= TQSize( 0,0 ); // no default application strut
bool	  TQApplication::animate_ui	= true;
bool	  TQApplication::animate_menu	= false;
bool	  TQApplication::fade_menu	= false;
bool	  TQApplication::animate_combo	= false;
bool	  TQApplication::animate_tooltip	= false;
bool	  TQApplication::fade_tooltip	= false;
bool	  TQApplication::animate_toolbox	= false;
bool	  TQApplication::widgetCount	= false;
TQApplication::Type tqt_appType=TQApplication::Tty;
#ifndef TQT_NO_COMPONENT
TQStringList *TQApplication::app_libpaths = 0;
#endif
bool	  TQApplication::metaComposeUnicode = false;
int	  TQApplication::composedUnicode   = 0;

#ifdef TQT_THREAD_SUPPORT
TQMutex *TQApplication::tqt_mutex		= 0;
TQMutex *tqt_sharedStringMutex			= 0;
TQ_EXPORT TQMutex * tqt_sharedMetaObjectMutex	= 0;

#ifdef QT_USE_GLIBMAINLOOP
TQMutex *tqt_timerListMutex			= 0;
#endif // QT_USE_GLIBMAINLOOP

static TQt::HANDLE tqt_application_thread_id	= 0;
TQ_EXPORT TQt::HANDLE tqt_get_application_thread_id()
{
    return tqt_application_thread_id;
}
#endif // TQT_THREAD_SUPPORT

#ifndef TQT_THREAD_SUPPORT
TQEventLoop *TQApplication::eventloop = 0;	// application event loop
#endif

#ifdef TQT_THREAD_SUPPORT
TQEventLoop* TQApplication::currentEventLoop() {
	TQThread* thread = TQThread::currentThreadObject();
	if (thread) {
		if (thread->d) {
			return thread->d->eventLoop;
		}
	}
	return NULL;
}
#else
TQEventLoop* TQApplication::currentEventLoop() {
	return TQApplication::eventloop;
}
#endif

#ifndef TQT_NO_ACCEL
extern bool tqt_dispatchAccelEvent( TQWidget*, TQKeyEvent* ); // def in qaccel.cpp
extern bool tqt_tryComposeUnicode( TQWidget*, TQKeyEvent* ); // def in qaccel.cpp
#endif

#if defined(QT_TABLET_SUPPORT)
bool chokeMouse = false;
#endif

void tqt_setMaxWindowRect(const TQRect& r)
{
    tqt_maxWindowRect = r;
    // Re-resize any maximized windows
    TQWidgetList* l = TQApplication::topLevelWidgets();
    if ( l ) {
	TQWidget *w = l->first();
	while ( w ) {
	    if ( w->isVisible() && w->isMaximized() )
	    {
		w->showNormal(); //#### flicker
		w->showMaximized();
	    }
	    w = l->next();
	}
	delete l;
    }
}

typedef void (*VFPTR)();
typedef TQValueList<VFPTR> TQVFuncList;
static TQVFuncList *postRList = 0;		// list of post routines

/*!
  \relates TQApplication

  Adds a global routine that will be called from the TQApplication
  destructor. This function is normally used to add cleanup routines
  for program-wide functionality.

  The function given by \a p should take no arguments and return
  nothing, like this:
  \code
    static int *global_ptr = 0;

    static void cleanup_ptr()
    {
	delete [] global_ptr;
	global_ptr = 0;
    }

    void init_ptr()
    {
	global_ptr = new int[100];	// allocate data
	tqAddPostRoutine( cleanup_ptr );	// delete later
    }
  \endcode

  Note that for an application- or module-wide cleanup,
  tqAddPostRoutine() is often not suitable. People have a tendency to
  make such modules dynamically loaded, and then unload those modules
  long before the TQApplication destructor is called, for example.

  For modules and libraries, using a reference-counted initialization
  manager or TQt' parent-child delete mechanism may be better. Here is
  an example of a private class which uses the parent-child mechanism
  to call a cleanup function at the right time:

  \code
    class MyPrivateInitStuff: public TQObject {
    private:
	MyPrivateInitStuff( TQObject * parent ): TQObject( parent) {
	    // initialization goes here
	}
	MyPrivateInitStuff * p;

    public:
	static MyPrivateInitStuff * initStuff( TQObject * parent ) {
	    if ( !p )
		p = new MyPrivateInitStuff( parent );
	    return p;
	}

	~MyPrivateInitStuff() {
	    // cleanup (the "post routine") goes here
	}
    }
  \endcode

  By selecting the right parent widget/object, this can often be made
  to clean up the module's data at the exact right moment.
*/

TQ_EXPORT void tqAddPostRoutine( TQtCleanUpFunction p)
{
    if ( !postRList ) {
	postRList = new TQVFuncList;
	TQ_CHECK_PTR( postRList );
    }
    postRList->prepend( p );
}


TQ_EXPORT void tqRemovePostRoutine( TQtCleanUpFunction p )
{
    if ( !postRList ) return;
    TQVFuncList::Iterator it = postRList->begin();
    while ( it != postRList->end() ) {
	if ( *it == p ) {
	    postRList->remove( it );
	    it = postRList->begin();
	} else {
	    ++it;
	}
    }
}

// Default application palettes and fonts (per widget type)
TQAsciiDict<TQPalette> *TQApplication::app_palettes = 0;
TQAsciiDict<TQFont>    *TQApplication::app_fonts = 0;

#ifndef TQT_NO_SESSIONMANAGER
TQString *TQApplication::session_key = 0;		// ## session key. Should be a member in 4.0
#endif
TQWidgetList *TQApplication::popupWidgets = 0;	// has keyboard input focus

TQDesktopWidget *tqt_desktopWidget = 0;		// root window widgets
#ifndef TQT_NO_CLIPBOARD
TQClipboard	      *tqt_clipboard = 0;	// global clipboard object
#endif
TQWidgetList * tqt_modal_stack=0;		// stack of modal widgets

#ifdef TQT_THREAD_SUPPORT

// thread wrapper for the main() thread
class TQCoreApplicationThread : public TQThread
{
public:
    inline TQCoreApplicationThread()
    {
#ifdef QT_CHECK_STATE
        if ( tqt_gui_thread_self )
            tqWarning( "TQCoreApplicationThread: there should be exactly one main thread object" );
#endif
        tqt_gui_thread_self = this;

        TQThreadInstance::setCurrentThread(this);

        // thread should be running and not finished for the lifetime
        // of the application (even if TQCoreApplication goes away)
        d->running = true;
        d->finished = false;
        d->eventLoop = NULL;
    }

    inline ~TQCoreApplicationThread()
    {
        tqt_gui_thread_self = nullptr;

        // avoid warning from TQThread
        d->running = false;
        // do some cleanup, namely clean up the thread-local storage associated with the GUI thread
        TQThreadInstance::finishGuiThread(d);
    }

    static TQCoreApplicationThread* self() { return tqt_gui_thread_self; }

private:
    inline void run()
    {
        // this function should never be called, it is implemented
        // only so that we can instantiate the object
        tqFatal("TQCoreApplicationThread: internal error");
    }

    static TQCoreApplicationThread* tqt_gui_thread_self;
};

TQCoreApplicationThread* TQCoreApplicationThread::tqt_gui_thread_self = nullptr;

// construct exactly one instance of the core thread with static storage duration. Do it static
// rather than in the heap as we need it to be properly destroyed on the exit from the program.
static TQCoreApplicationThread tqt_main_thread;
#endif

// Definitions for posted events
struct TQPostEvent {
    TQPostEvent( TQObject *r, TQEvent *e ): receiver( r ), event( e ) {}
   ~TQPostEvent() { delete event; }
    TQObject  *receiver;
    TQEvent   *event;
};

class TQ_EXPORT TQPostEventList : public TQPtrList<TQPostEvent>
{
public:
    TQPostEventList(bool with_mutex = false) : TQPtrList<TQPostEvent>(), m_mutex(nullptr)
    {
#ifdef TQT_THREAD_SUPPORT
	if (with_mutex)
	{
	    m_mutex = new TQMutex(true);
	}
#endif
    }

    ~TQPostEventList()
    {
	if (m_mutex)
	{
	    delete m_mutex;
	    m_mutex = nullptr;
	}
	clear();
    }

    TQMutex* mutex() const { return m_mutex; }

private:
    TQMutex *m_mutex;

    TQPostEventList(const TQPostEventList &) = delete;
    TQPostEventList &operator=(const TQPostEventList &) = delete;
};

class TQ_EXPORT TQPostEventListIt : public TQPtrListIterator<TQPostEvent>
{
public:
    TQPostEventListIt( const TQPostEventList &l ) : TQPtrListIterator<TQPostEvent>(l) {}
    TQPostEventListIt &operator=(const TQPostEventListIt &i)
{
    return (TQPostEventListIt&)TQPtrListIterator<TQPostEvent>::operator=(i); }
};

// The global list and its pointer are initialized in different functions
// to optimize access to the list pointer in normal usage
static TQPostEventList* InitGlobalPostedEventsList()
{
    static TQPostEventList _globalEventList(true);
    _globalEventList.setAutoDelete(true);
    return &_globalEventList;
}

static TQPostEventList* GlobalPostedEvents()
{
    static TQPostEventList *_globalPostedEvents = InitGlobalPostedEventsList();
    return _globalPostedEvents;
}

uint qGlobalPostedEventsCount()
{
#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker locker( GlobalPostedEvents()->mutex() );
#endif // TQT_THREAD_SUPPORT

    return GlobalPostedEvents()->count();
}

#ifndef TQT_NO_PALETTE
TQPalette *tqt_std_pal = 0;

void tqt_create_std_palette()
{
    if ( tqt_std_pal )
	delete tqt_std_pal;

    TQColor standardLightGray( 192, 192, 192 );
    TQColor light( 255, 255, 255 );
    TQColor dark( standardLightGray.dark( 150 ) );
    TQColorGroup std_act( TQt::black, standardLightGray,
			 light, dark, TQt::gray,
			 TQt::black, TQt::white );
    TQColorGroup std_dis( TQt::darkGray, standardLightGray,
			 light, dark, TQt::gray,
			 TQt::darkGray, std_act.background() );
    TQColorGroup std_inact( TQt::black, standardLightGray,
			   light, dark, TQt::gray,
			   TQt::black, TQt::white );
    tqt_std_pal = new TQPalette( std_act, std_dis, std_inact );
}

static void tqt_fix_tooltips()
{
    // No resources for this yet (unlike on Windows).
    TQColorGroup cg( TQt::black, TQColor(255,255,220),
		    TQColor(96,96,96), TQt::black, TQt::black,
		    TQt::black, TQColor(255,255,220) );
    TQPalette pal( cg, cg, cg );
    TQApplication::setPalette( pal, true, "TQTipLabel");
}
#endif

void TQApplication::process_cmdline( int* argcptr, char ** argv )
{
    // process platform-indep command line
    if ( !tqt_is_gui_used || !*argcptr)
	return;

    int argc = *argcptr;
    int i, j;

    j = 1;
    for ( i=1; i<argc; i++ ) {
	if ( argv[i] && *argv[i] != '-' ) {
	    argv[j++] = argv[i];
	    continue;
	}
	TQCString arg = argv[i];
	TQCString s;
	if ( arg == "-qdevel" || arg == "-qdebug") {
	    // obsolete argument
	} else if ( arg.find( "-style=", 0, false ) != -1 ) {
	    s = arg.right( arg.length() - 7 );
	} else if ( qstrcmp(arg,"-style") == 0 && i < argc-1 ) {
	    s = argv[++i];
	    s = s.lower();
#ifndef TQT_NO_SESSIONMANAGER
	} else if ( qstrcmp(arg,"-session") == 0 && i < argc-1 ) {
	    TQCString s = argv[++i];
	    if ( !s.isEmpty() ) {
		session_id = TQString::fromLatin1( s );
		int p = session_id.find( '_' );
		if ( p >= 0 ) {
		    if ( !session_key )
			session_key = new TQString;
		    *session_key = session_id.mid( p +1 );
		    session_id = session_id.left( p );
		}
		is_session_restored = true;
	    }
#endif
	} else if ( qstrcmp(arg, "-reverse") == 0 ) {
	    setReverseLayout( true );
	} else if ( qstrcmp(arg, "-widgetcount") == 0 ) {
	    widgetCount = true;;
	} else {
	    argv[j++] = argv[i];
	}
#ifndef TQT_NO_STYLE
	if ( !s.isEmpty() ) {
	    setStyle( s );
	}
#endif
    }

    if(j < argc) {
#ifdef TQ_WS_MACX
	static char* empty = "\0";
	argv[j] = empty;
#else
	argv[j] = 0;
#endif
	*argcptr = j;
    }
}

/*!
  Initializes the window system and constructs an application object
  with \a argc command line arguments in \a argv.

  The global \c tqApp pointer refers to this application object. Only
  one application object should be created.

  This application object must be constructed before any \link
  TQPaintDevice paint devices\endlink (including widgets, pixmaps, bitmaps
  etc.).

  Note that \a argc and \a argv might be changed. TQt removes command
  line arguments that it recognizes. The modified \a argc and \a argv
  can also be accessed later with \c tqApp->argc() and \c tqApp->argv().
  The documentation for argv() contains a detailed description of how
  to process command line arguments.

  TQt debugging options (not available if TQt was compiled with the
  TQT_NO_DEBUG flag defined):
  \list
  \i -nograb, tells TQt that it must never grab the mouse or the keyboard.
  \i -dograb (only under X11), running under a debugger can cause
  an implicit -nograb, use -dograb to override.
  \i -sync (only under X11), switches to synchronous mode for
	debugging.
  \endlist

  See \link debug.html Debugging Techniques \endlink for a more
  detailed explanation.

  All TQt programs automatically support the following command line options:
  \list
  \i -reverse causes text to be formatted for right-to-left languages
       rather than in the usual left-to-right direction.
  \i -style= \e style, sets the application GUI style. Possible values
       are \c motif, \c windows, and \c platinum. If you compiled TQt
       with additional styles or have additional styles as plugins these
       will be available to the \c -style command line option.
  \i -style \e style, is the same as listed above.
  \i -session= \e session, restores the application from an earlier
       \link session.html session \endlink.
  \i -session \e session, is the same as listed above.
  \i -widgetcount, prints debug message at the end about number of widgets left
       undestroyed and maximum number of widgets existed at the same time
  \endlist

  The X11 version of TQt also supports some traditional X11
  command line options:
  \list
  \i -display \e display, sets the X display (default is $DISPLAY).
  \i -geometry \e geometry, sets the client geometry of the
	\link setMainWidget() main widget\endlink.
  \i -fn or \c -font \e font, defines the application font. The
  font should be specified using an X logical font description.
  \i -bg or \c -background \e color, sets the default background color
	and an application palette (light and dark shades are calculated).
  \i -fg or \c -foreground \e color, sets the default foreground color.
  \i -btn or \c -button \e color, sets the default button color.
  \i -name \e name, sets the application name.
  \i -title \e title, sets the application title (caption).
  \i -visual \c TrueColor, forces the application to use a TrueColor visual
       on an 8-bit display.
  \i -ncols \e count, limits the number of colors allocated in the
       color cube on an 8-bit display, if the application is using the
       \c TQApplication::ManyColor color specification. If \e count is
       216 then a 6x6x6 color cube is used (i.e. 6 levels of red, 6 of green,
       and 6 of blue); for other values, a cube
       approximately proportional to a 2x3x1 cube is used.
  \i -cmap, causes the application to install a private color map
       on an 8-bit display.
  \endlist

  \sa argc(), argv()
*/

//######### BINARY COMPATIBILITY constructor
TQApplication::TQApplication( int &argc, char **argv )
{
    construct( argc, argv, GuiClient, true );
}

/*!
  Constructs an application object with \a argc command line arguments
  in \a argv. If \a GUIenabled is true, a GUI application is
  constructed, otherwise a non-GUI (console) application is created.

  Set \a GUIenabled to false for programs without a graphical user
  interface that should be able to run without a window system.

  On X11, the window system is initialized if \a GUIenabled is true.
  If \a GUIenabled is false, the application does not connect to the
  X-server.
  On Windows and Macintosh, currently the window system is always
  initialized, regardless of the value of GUIenabled. This may change in
  future versions of TQt.

  The following example shows how to create an application that
  uses a graphical interface when available.
  \code
  int main( int argc, char **argv )
  {
#ifdef TQ_WS_X11
    bool useGUI = getenv( "DISPLAY" ) != 0;
#else
    bool useGUI = true;
#endif
    TQApplication app(argc, argv, useGUI);

    if ( useGUI ) {
       //start GUI version
       ...
    } else {
       //start non-GUI version
       ...
    }
    return app.exec();
  }
\endcode
*/

TQApplication::TQApplication( int &argc, char **argv, bool GUIenabled  )
{
    construct( argc, argv, GUIenabled ? GuiClient : Tty, true );
}

/*!
  Constructs an application object with \a argc command line arguments
  in \a argv. If \a GUIenabled is true, a GUI application is
  constructed, otherwise a non-GUI (console) application is created.
  If \a SMEnabled is true, session management support is enabled (default).

  Set \a GUIenabled to false for programs without a graphical user
  interface that should be able to run without a window system.

  Set \a SMEnabled to false to disable session management.
  Session management cannot be enabled at a later time if disabled here.

  On X11, the window system is initialized if \a GUIenabled is true.
  If \a GUIenabled is false, the application does not connect to the
  X-server.
  On Windows and Macintosh, currently the window system is always
  initialized, regardless of the value of GUIenabled. This may change in
  future versions of TQt.

  The following example shows how to create an application that
  uses a graphical interface when available.
  \code
  int main( int argc, char **argv )
  {
#ifdef TQ_WS_X11
    bool useGUI = getenv( "DISPLAY" ) != 0;
#else
    bool useGUI = true;
#endif
    TQApplication app(argc, argv, useGUI);

    if ( useGUI ) {
       //start GUI version
       ...
    } else {
       //start non-GUI version
       ...
    }
    return app.exec();
  }
\endcode
*/

TQApplication::TQApplication( int &argc, char **argv, bool GUIenabled, bool SMenabled  )
{
    construct( argc, argv, GUIenabled ? GuiClient : Tty, SMenabled );
}

/*!
  Constructs an application object with \a argc command line arguments
  in \a argv.

  For TQt/Embedded, passing \c TQApplication::GuiServer for \a type
  makes this application the server (equivalent to running with the
  -qws option).
*/
TQApplication::TQApplication( int &argc, char **argv, Type type )
{
    construct( argc, argv, type, true );
}

TQ_EXPORT void tqt_ucm_initialize( TQApplication *theApp )
{
    if ( tqApp )
	return;
    int argc = theApp->argc();
    char **argv = theApp->argv();
    theApp->construct( argc, argv, tqApp->type(), true );

    Q_ASSERT( tqApp == theApp );
}

void TQApplication::construct( int &argc, char **argv, Type type, bool enable_sm )
{
    tqt_appType = type;
    tqt_is_gui_used = (type != Tty);
    init_precmdline();
    static const char *empty = "";
    if ( argc == 0 || argv == 0 ) {
	argc = 0;
	argv = (char **)&empty; // ouch! careful with TQApplication::argv()!
    }
    app_argc = argc;
    app_argv = argv;

    tqt_init( &argc, argv, type );   // Must be called before initialize()
    process_cmdline( &argc, argv );
    initialize( argc, argv, enable_sm );
    if ( tqt_is_gui_used )
	tqt_maxWindowRect = desktop()->rect();
    if ( currentEventLoop() )
	currentEventLoop()->appStartingUp();
}

/*!
    Returns the type of application, Tty, GuiClient or GuiServer.
*/

TQApplication::Type TQApplication::type() const
{
    return tqt_appType;
}

#if defined(TQ_WS_X11)
/*!
  Create an application, given an already open display \a dpy. If \a
  visual and \a colormap are non-zero, the application will use those as
  the default Visual and Colormap contexts.

  \warning TQt only supports TrueColor visuals at depths higher than 8
  bits-per-pixel.

  This is available only on X11.
*/

TQApplication::TQApplication( Display* dpy, HANDLE visual, HANDLE colormap )
{
    static int aargc = 1;
    // ### a string literal is a cont char*
    // ### using it as a char* is wrong and could lead to segfaults
    // ### if aargv is modified someday
    static char *aargv[] = { (char*)"unknown", 0 };

    app_argc = aargc;
    app_argv = aargv;

    tqt_appType = GuiClient;
    tqt_is_gui_used = true;
    tqt_appType = GuiClient;
    init_precmdline();
    // ... no command line.

    if ( ! dpy ) {
#ifdef QT_CHECK_STATE
	tqWarning( "TQApplication: invalid Display* argument." );
#endif // QT_CHECK_STATE

	tqt_init( &aargc, aargv, GuiClient );
    } else {
	tqt_init( dpy, visual, colormap );
    }

    initialize( aargc, aargv );

    if ( tqt_is_gui_used )
	tqt_maxWindowRect = desktop()->rect();
    if ( currentEventLoop() )
	currentEventLoop()->appStartingUp();
}

/*!
  Create an application, given an already open display \a dpy and using
  \a argc command line arguments in \a argv. If \a
  visual and \a colormap are non-zero, the application will use those as
  the default Visual and Colormap contexts.

  \warning TQt only supports TrueColor visuals at depths higher than 8
  bits-per-pixel.

  This is available only on X11.

*/
TQApplication::TQApplication(Display *dpy, int argc, char **argv,
			   HANDLE visual, HANDLE colormap)
{
    tqt_appType = GuiClient;
    tqt_is_gui_used = true;
    tqt_appType = GuiClient;
    init_precmdline();

    app_argc = argc;
    app_argv = argv;

    if ( ! dpy ) {
#ifdef QT_CHECK_STATE
	tqWarning( "TQApplication: invalid Display* argument." );
#endif // QT_CHECK_STATE

	tqt_init( &argc, argv, GuiClient );
    } else {
	tqt_init( &argc, argv, dpy, visual, colormap );
    }

    process_cmdline( &argc, argv );
    initialize(argc, argv);

    if ( tqt_is_gui_used )
	tqt_maxWindowRect = desktop()->rect();
    if ( currentEventLoop() )
	currentEventLoop()->appStartingUp();
}


#endif // TQ_WS_X11

#ifdef TQT_THREAD_SUPPORT
TQThread* TQApplication::guiThread() {
	return TQCoreApplicationThread::self();
}

bool TQApplication::isGuiThread() {
	return (TQThread::currentThreadObject() == guiThread());
}
#else
bool TQApplication::isGuiThread() {
	return true;
}
#endif

void TQApplication::init_precmdline()
{
    translators = 0;
    is_app_closing = false;
#ifndef TQT_NO_SESSIONMANAGER
    is_session_restored = false;
#endif
#if defined(QT_CHECK_STATE)
    if ( tqApp )
	tqWarning( "TQApplication: There should be max one application object" );
#endif
    tqApp = (TQApplication*)this;
}

/*!
  Initializes the TQApplication object, called from the constructors.
*/

void TQApplication::initialize( int argc, char **argv, bool enable_sm )
{
#ifdef TQT_THREAD_SUPPORT
    tqt_mutex = new TQMutex( true );
    tqt_sharedStringMutex = new TQMutex( true );
    tqt_sharedMetaObjectMutex = new TQMutex( true );
#ifdef QT_USE_GLIBMAINLOOP
    tqt_timerListMutex = new TQMutex( true );
#endif // QT_USE_GLIBMAINLOOP
    tqt_application_thread_id = TQThread::currentThread();
#endif // TQT_THREAD_SUPPORT

    app_argc = argc;
    app_argv = argv;
    quit_now = false;
    quit_code = 0;
    TQWidget::createMapper(); // create widget mapper
#ifndef TQT_NO_PALETTE
    (void) palette();  // trigger creation of application palette
#endif
    is_app_running = true; // no longer starting up

#ifndef TQT_NO_SESSIONMANAGER
    if (enable_sm) {
	// connect to the session manager
	if ( !session_key )
	    session_key = new TQString;
	session_manager = new TQSessionManager( tqApp, session_id, *session_key );
    }
    else {
        session_manager = 0;
    }
#endif

}


/*****************************************************************************
  Functions returning the active popup and modal widgets.
 *****************************************************************************/

/*!
  Returns the active popup widget.

  A popup widget is a special top level widget that sets the \c
  WType_Popup widget flag, e.g. the TQPopupMenu widget. When the
  application opens a popup widget, all events are sent to the popup.
  Normal widgets and modal widgets cannot be accessed before the popup
  widget is closed.

  Only other popup widgets may be opened when a popup widget is shown.
  The popup widgets are organized in a stack. This function returns
  the active popup widget at the top of the stack.

  \sa activeModalWidget(), topLevelWidgets()
*/

TQWidget *TQApplication::activePopupWidget()
{
    return popupWidgets ? popupWidgets->getLast() : 0;
}


/*!
  Returns the active modal widget.

  A modal widget is a special top level widget which is a subclass of
  TQDialog that specifies the modal parameter of the constructor as
  true. A modal widget must be closed before the user can continue
  with other parts of the program.

  Modal widgets are organized in a stack. This function returns
  the active modal widget at the top of the stack.

  \sa activePopupWidget(), topLevelWidgets()
*/

TQWidget *TQApplication::activeModalWidget()
{
    return tqt_modal_stack ? tqt_modal_stack->getFirst() : 0;
}

/*!
  Cleans up any window system resources that were allocated by this
  application. Sets the global variable \c tqApp to 0.
*/

TQApplication::~TQApplication()
{
#ifndef TQT_NO_CLIPBOARD
    // flush clipboard contents
    if ( tqt_clipboard ) {
	TQCustomEvent event( TQEvent::Clipboard );
	TQApplication::sendEvent( tqt_clipboard, &event );
    }
#endif

    if ( currentEventLoop() )
	currentEventLoop()->appClosingDown();
    if ( postRList ) {
	TQVFuncList::Iterator it = postRList->begin();
	while ( it != postRList->end() ) {	// call post routines
	    (**it)();
	    postRList->remove( it );
	    it = postRList->begin();
	}
	delete postRList;
	postRList = 0;
    }

    TQObject *tipmanager = child( "toolTipManager", "TQTipManager", false );
    delete tipmanager;

    delete tqt_desktopWidget;
    tqt_desktopWidget = 0;
    is_app_closing = true;

    // Due to hacks to speed up TQStyle engine (see git hash 523c1fd99) TQObjects now contain a
    // reference to TQStyleControlElementData object which among other contain TQFont members.
    // But for a proper cleanup all fonts should be destroyed before disconnecting from X11 (in
    // tqt_cleanup()). So we will have to cleanup up the data explicitly.
    cleanupControlElementData();

#ifndef TQT_NO_CLIPBOARD
    delete tqt_clipboard;
    tqt_clipboard = 0;
#endif
    TQWidget::destroyMapper();
#ifndef TQT_NO_PALETTE
    delete tqt_std_pal;
    tqt_std_pal = 0;
    delete app_pal;
    app_pal = 0;
    delete app_palettes;
    app_palettes = 0;
#endif
    delete app_font;
    app_font = 0;
    delete app_fonts;
    app_fonts = 0;
#ifndef TQT_NO_STYLE
    delete app_style;
    app_style = 0;
#endif
#ifndef TQT_NO_CURSOR
    delete app_cursor;
    app_cursor = 0;
#endif
#ifndef TQT_NO_TRANSLATION
    delete translators;
#endif

#ifndef TQT_NO_DRAGANDDROP
    extern TQDragManager *tqt_dnd_manager;
    delete tqt_dnd_manager;
#endif

    tqt_cleanup();

#ifndef TQT_NO_COMPONENT
    delete app_libpaths;
    app_libpaths = 0;
#endif

#ifdef TQT_THREAD_SUPPORT
    delete tqt_mutex;
    tqt_mutex = 0;
#endif // TQT_THREAD_SUPPORT

    if( tqApp == this ) {
	if ( postedEvents )
	    removePostedEvents( this );
	tqApp = 0;
    }
    is_app_running = false;

    if ( widgetCount ) {
	tqDebug( "Widgets left: %i    Max widgets: %i \n", TQWidget::instanceCounter, TQWidget::maxInstances );
    }
#ifndef TQT_NO_SESSIONMANAGER
    if ( session_manager ) {
	delete session_manager;
    }
    session_manager = 0;
    if ( session_key ) {
	delete session_key;
    }
    session_key = 0;
#endif //TQT_NO_SESSIONMANAGER

#ifdef TQT_THREAD_SUPPORT
    delete tqt_sharedMetaObjectMutex;
    tqt_sharedMetaObjectMutex = 0;
    delete tqt_sharedStringMutex;
    tqt_sharedStringMutex = 0;
#ifdef QT_USE_GLIBMAINLOOP
    delete tqt_timerListMutex;
    tqt_timerListMutex = 0;
#endif // QT_USE_GLIBMAINLOOP
#endif // TQT_THREAD_SUPPORT

    tqt_explicit_app_style = false;
    tqt_app_has_font = false;
    app_tracking = 0;
    obey_desktop_settings = true;
    cursor_flash_time = 1000;
    mouse_double_click_time = 400;
#ifndef TQT_NO_WHEELEVENT
    wheel_scroll_lines = 3;
#endif
    drag_time = 500;
    drag_distance = 4;
    reverse_layout = false;
    app_strut = TQSize( 0, 0 );
    animate_ui = true;
    animate_menu = false;
    fade_menu = false;
    animate_combo = false;
    animate_tooltip = false;
    fade_tooltip = false;
    widgetCount = false;
}


/*!
    \fn int TQApplication::argc() const

    Returns the number of command line arguments.

    The documentation for argv() describes how to process command line
    arguments.

    \sa argv(), TQApplication::TQApplication()
*/

/*!
    \fn char **TQApplication::argv() const

    Returns the command line argument vector.

    \c argv()[0] is the program name, \c argv()[1] is the first
    argument and \c argv()[argc()-1] is the last argument.

    A TQApplication object is constructed by passing \e argc and \e
    argv from the \c main() function. Some of the arguments may be
    recognized as TQt options and removed from the argument vector. For
    example, the X11 version of TQt knows about \c -display, \c -font
    and a few more options.

    Example:
    \code
	// showargs.cpp - displays program arguments in a list box

	#include <ntqapplication.h>
	#include <ntqlistbox.h>

	int main( int argc, char **argv )
	{
	    TQApplication a( argc, argv );
	    TQListBox b;
	    a.setMainWidget( &b );
	    for ( int i = 0; i < a.argc(); i++ )  // a.argc() == argc
		b.insertItem( a.argv()[i] );      // a.argv()[i] == argv[i]
	    b.show();
	    return a.exec();
	}
    \endcode

    If you run \c{showargs -display unix:0 -font 9x15bold hello world}
    under X11, the list box contains the three strings "showargs",
    "hello" and "world".

    TQt provides a global pointer, \c tqApp, that points to the
    TQApplication object, and through which you can access argc() and
    argv() in functions other than main().

    \sa argc(), TQApplication::TQApplication()
*/

/*!
    \fn void TQApplication::setArgs( int argc, char **argv )
    \internal
*/


#ifndef TQT_NO_STYLE

static TQString *tqt_style_override = 0;

/*!
  Returns the application's style object.

  \sa setStyle(), TQStyle
*/
TQStyle& TQApplication::style()
{
#ifndef TQT_NO_STYLE
    if ( app_style )
	return *app_style;
    if ( !tqt_is_gui_used )
	tqFatal( "No style available in non-gui applications!" );

#if defined(TQ_WS_X11)
    if(!tqt_style_override)
	x11_initialize_style(); // run-time search for default style
#endif
    if ( !app_style ) {
	// Compile-time search for default style
	//
	TQString style;
	if ( tqt_style_override ) {
	    style = *tqt_style_override;
	    delete tqt_style_override;
	    tqt_style_override = 0;
	} else {
#  if defined(TQ_WS_WIN) && defined(Q_OS_TEMP)
	    style = "PocketPC";
#elif defined(TQ_WS_WIN)
	    if ( qWinVersion() >= TQt::WV_XP && qWinVersion() < TQt::WV_NT_based )
		style = "WindowsXP";
	    else
		style = "Windows";		// default styles for Windows
#elif defined(TQ_WS_X11) && defined(Q_OS_SOLARIS)
	    style = "CDE";			// default style for X11 on Solaris
#elif defined(TQ_WS_X11)
		style = "Motif";		// default style for X11
#elif defined(TQ_WS_MAC)
		style = "Macintosh";		// default style for all Mac's
#elif defined(TQ_WS_QWS)
	    style = "Compact";		// default style for small devices
#endif
	}
	app_style = TQStyleFactory::create( style );
	if ( !app_style &&		// platform default style not available, try alternatives
	     !(app_style = TQStyleFactory::create( "Windows" ) ) &&
	     !(app_style = TQStyleFactory::create( "Platinum" ) ) &&
	     !(app_style = TQStyleFactory::create( "MotifPlus" ) ) &&
	     !(app_style = TQStyleFactory::create( "Motif" ) ) &&
	     !(app_style = TQStyleFactory::create( "CDE" ) ) &&
	     !(app_style = TQStyleFactory::create( "Aqua" ) ) &&
	     !(app_style = TQStyleFactory::create( "SGI" ) ) &&
	     !(app_style = TQStyleFactory::create( "Compact" ) )
#ifndef TQT_NO_STRINGLIST
	    && !(app_style = TQStyleFactory::create( TQStyleFactory::keys()[0]  ) )
#endif
	)
	    tqFatal( "No %s style available!", style.latin1() );
    }

    TQPalette app_pal_copy ( *app_pal );
    app_style->polish( *app_pal );

    if ( is_app_running && !is_app_closing && (*app_pal != app_pal_copy) ) {
	TQEvent e( TQEvent::ApplicationPaletteChange );
	TQWidgetIntDictIt it( *((TQWidgetIntDict*)TQWidget::mapper) );
	TQWidget *w;
	while ( (w=it.current()) ) {		// for all widgets...
	    ++it;
	    sendEvent( w, &e );
	}
    }

    app_style->polish( tqApp );
#endif
    return *app_style;
}

/*!
  Sets the application's GUI style to \a style. Ownership of the style
  object is transferred to TQApplication, so TQApplication will delete
  the style object on application exit or when a new style is set.

  Example usage:
  \code
    TQApplication::setStyle( new TQWindowsStyle );
  \endcode

  When switching application styles, the color palette is set back to
  the initial colors or the system defaults. This is necessary since
  certain styles have to adapt the color palette to be fully
  style-guide compliant.

  \sa style(), TQStyle, setPalette(), desktopSettingsAware()
*/
void TQApplication::setStyle( TQStyle *style )
{
    TQStyle* old = app_style;
    app_style = style;
#ifdef TQ_WS_X11
    tqt_explicit_app_style = true;
#endif // TQ_WS_X11

    if ( startingUp() ) {
	delete old;
	return;
    }

    // clean up the old style
    if (old) {
	if ( is_app_running && !is_app_closing ) {
	    TQWidgetIntDictIt it( *((TQWidgetIntDict*)TQWidget::mapper) );
	    TQWidget *w;
	    while ( (w=it.current()) ) {		// for all widgets...
		++it;
		if ( !w->testWFlags(WType_Desktop) &&	// except desktop
		     w->testWState(WState_Polished) ) { // has been polished
		    old->unPolish(w);
		}
	    }
	}
	old->unPolish( tqApp );
    }

    // take care of possible palette requirements of certain gui
    // styles. Do it before polishing the application since the style
    // might call TQApplication::setStyle() itself
    if ( !tqt_std_pal )
	tqt_create_std_palette();
    TQPalette tmpPal = *tqt_std_pal;
    setPalette( tmpPal, true );

    // initialize the application with the new style
    app_style->polish( tqApp );

    // re-polish existing widgets if necessary
    if (old) {
	if ( is_app_running && !is_app_closing ) {
	    TQWidgetIntDictIt it( *((TQWidgetIntDict*)TQWidget::mapper) );
	    TQWidget *w;
	    while ( (w=it.current()) ) {		// for all widgets...
		++it;
		if ( !w->testWFlags(WType_Desktop) ) {	// except desktop
		    if ( w->testWState(WState_Polished) )
			app_style->polish(w);		// repolish
		    w->styleChange( *old );
		    if ( w->isVisible() ){
			w->update();
		    }
		}
	    }
	}
	delete old;
    }
}

/*!
  \overload

  Requests a TQStyle object for \a style from the TQStyleFactory.

  The string must be one of the TQStyleFactory::keys(), typically one
  of "windows", "motif", "cde", "motifplus", "platinum", "sgi" and
  "compact". Depending on the platform, "windowsxp", "aqua" or
  "macintosh" may be available.

  A later call to the TQApplication constructor will override the
  requested style when a "-style" option is passed in as a commandline
  parameter.

  Returns 0 if an unknown \a style is passed, otherwise the TQStyle object
  returned is set as the application's GUI style.
*/
TQStyle* TQApplication::setStyle( const TQString& style )
{
#ifdef TQ_WS_X11
    tqt_explicit_app_style = true;
#endif // TQ_WS_X11

    if ( startingUp() ) {
	if(tqt_style_override)
	    *tqt_style_override = style;
	else
	    tqt_style_override = new TQString(style);
	return 0;
    }
    TQStyle *s = TQStyleFactory::create( style );
    if ( !s )
	return 0;

    setStyle( s );
    return s;
}

#endif


#if 1  /* OBSOLETE */

TQApplication::ColorMode TQApplication::colorMode()
{
    return (TQApplication::ColorMode)app_cspec;
}

void TQApplication::setColorMode( TQApplication::ColorMode mode )
{
    app_cspec = mode;
}
#endif


/*!
  Returns the color specification.
  \sa TQApplication::setColorSpec()
 */

int TQApplication::colorSpec()
{
    return app_cspec;
}

/*!
  Sets the color specification for the application to \a spec.

  The color specification controls how the application allocates colors
  when run on a display with a limited amount of colors, e.g. 8 bit / 256
  color displays.

  The color specification must be set before you create the TQApplication
  object.

  The options are:
  \list
  \i TQApplication::NormalColor.
    This is the default color allocation strategy. Use this option if
    your application uses buttons, menus, texts and pixmaps with few
    colors. With this option, the application uses system global
    colors. This works fine for most applications under X11, but on
    Windows machines it may cause dithering of non-standard colors.
  \i TQApplication::CustomColor.
    Use this option if your application needs a small number of custom
    colors. On X11, this option is the same as NormalColor. On Windows, TQt
    creates a Windows palette, and allocates colors to it on demand.
  \i TQApplication::ManyColor.
    Use this option if your application is very color hungry
    (e.g. it requires thousands of colors).
    Under X11 the effect is:
    \list
    \i For 256-color displays which have at best a 256 color true color
       visual, the default visual is used, and colors are allocated
       from a color cube. The color cube is the 6x6x6 (216 color) "Web
       palette"<sup>*</sup>, but the number of colors can be changed
       by the \e -ncols option. The user can force the application to
       use the true color visual with the \link
       TQApplication::TQApplication() -visual \endlink option.
    \i For 256-color displays which have a true color visual with more
       than 256 colors, use that visual. Silicon Graphics X servers
       have this feature, for example. They provide an 8 bit visual
       by default but can deliver true color when asked.
    \endlist
    On Windows, TQt creates a Windows palette, and fills it with a color cube.
  \endlist

  Be aware that the CustomColor and ManyColor choices may lead to colormap
  flashing: The foreground application gets (most) of the available
  colors, while the background windows will look less attractive.

  Example:
  \code
  int main( int argc, char **argv )
  {
      TQApplication::setColorSpec( TQApplication::ManyColor );
      TQApplication a( argc, argv );
      ...
  }
  \endcode

  TQColor provides more functionality for controlling color allocation and
  freeing up certain colors. See TQColor::enterAllocContext() for more
  information.

  To check what mode you end up with, call TQColor::numBitPlanes() once
  the TQApplication object exists. A value greater than 8 (typically
  16, 24 or 32) means true color.

  <sup>*</sup> The color cube used by TQt has 216 colors whose red,
  green, and blue components always have one of the following values:
  0x00, 0x33, 0x66, 0x99, 0xCC, or 0xFF.

  \sa colorSpec(), TQColor::numBitPlanes(), TQColor::enterAllocContext() */

void TQApplication::setColorSpec( int spec )
{
#if defined(QT_CHECK_STATE)
    if ( tqApp ) {
	tqWarning( "TQApplication::setColorSpec: This function must be "
		 "called before the TQApplication object is created" );
    }
#endif
    app_cspec = spec;
}

/*!
  \fn TQSize TQApplication::globalStrut()

  Returns the application's global strut.

  The strut is a size object whose dimensions are the minimum that any
  GUI element that the user can interact with should have. For example
  no button should be resized to be smaller than the global strut size.

  \sa setGlobalStrut()
*/

/*!
  Sets the application's global strut to \a strut.

  The strut is a size object whose dimensions are the minimum that any
  GUI element that the user can interact with should have. For example
  no button should be resized to be smaller than the global strut size.

  The strut size should be considered when reimplementing GUI controls
  that may be used on touch-screens or similar IO-devices.

  Example:
  \code
  TQSize& WidgetClass::sizeHint() const
  {
      return TQSize( 80, 25 ).expandedTo( TQApplication::globalStrut() );
  }
  \endcode

  \sa globalStrut()
*/

void TQApplication::setGlobalStrut( const TQSize& strut )
{
    app_strut = strut;
}

#if defined( TQ_WS_WIN ) || defined( TQ_WS_MAC )
extern const char *tqAppFileName();
#endif

#ifndef TQT_NO_DIR
#ifndef TQ_WS_WIN
static TQString resolveSymlinks( const TQString& path, int depth = 0 )
{
    bool foundLink = false;
    TQString linkTarget;
    TQString part = path;
    int slashPos = path.length();

    // too deep; we give up
    if ( depth == 128 )
	return TQString::null;

    do {
	part = part.left( slashPos );
	TQFileInfo fileInfo( part );
	if ( fileInfo.isSymLink() ) {
	    foundLink = true;
	    linkTarget = fileInfo.readLink();
	    break;
	}
    } while ( (slashPos = part.findRev('/')) != -1 );

    if ( foundLink ) {
	TQString path2;
	if ( linkTarget[0] == '/' ) {
	    path2 = linkTarget;
	    if ( slashPos < (int) path.length() )
		path2 += "/" + path.right( path.length() - slashPos - 1 );
	} else {
	    TQString relPath;
	    relPath = part.left( part.findRev('/') + 1 ) + linkTarget;
	    if ( slashPos < (int) path.length() ) {
		if ( !linkTarget.endsWith( "/" ) )
		    relPath += "/";
		relPath += path.right( path.length() - slashPos - 1 );
	    }
	    path2 = TQDir::current().absFilePath( relPath );
	}
	path2 = TQDir::cleanDirPath( path2 );
	return resolveSymlinks( path2, depth + 1 );
    } else {
	return path;
    }
}
#endif // TQ_WS_WIN

/*!
    Returns the directory that contains the application executable.

    For example, if you have installed TQt in the \c{C:\Trolltech\TQt}
    directory, and you run the \c{demo} example, this function will
    return "C:/Trolltech/TQt/examples/demo".

    On Mac OS X this will point to the directory actually containing the
    executable, which may be inside of an application bundle (if the
    application is bundled).

    \warning On Unix, this function assumes that argv[0] contains the file
    name of the executable (which it normally does). It also assumes that
    the current directory hasn't been changed by the application.

    \sa applicationFilePath()
*/
TQString TQApplication::applicationDirPath()
{
    return TQFileInfo( applicationFilePath() ).dirPath();
}

/*!
    Returns the file path of the application executable.

    For example, if you have installed TQt in the \c{C:\Trolltech\TQt}
    directory, and you run the \c{demo} example, this function will
    return "C:/Trolltech/TQt/examples/demo/demo.exe".

    \warning On Unix, this function assumes that argv[0] contains the file
    name of the executable (which it normally does). It also assumes that
    the current directory hasn't been changed by the application.

    \sa applicationDirPath()
*/
TQString TQApplication::applicationFilePath()
{
#if defined( TQ_WS_WIN )
    TQFileInfo filePath;
    QT_WA({
        WCHAR module_name[256];
        GetModuleFileNameW(0, module_name, sizeof(module_name));
        filePath = TQString::fromUcs2((const unsigned short *)module_name);
    }, {
        char module_name[256];
        GetModuleFileNameA(0, module_name, sizeof(module_name));
        filePath = TQString::fromLocal8Bit(module_name);
    });

    return filePath.filePath();
#elif defined( TQ_WS_MAC )
    return TQDir::cleanDirPath( TQFile::decodeName( tqAppFileName() ) );
#else
    TQString argv0 = TQFile::decodeName( argv()[0] );
    TQString absPath;

    if ( argv0[0] == '/' ) {
	/*
	  If argv0 starts with a slash, it is already an absolute
	  file path.
	*/
	absPath = argv0;
    } else if ( argv0.find('/') != -1 ) {
	/*
	  If argv0 contains one or more slashes, it is a file path
	  relative to the current directory.
	*/
	absPath = TQDir::current().absFilePath( argv0 );
    } else {
	/*
	  Otherwise, the file path has to be determined using the
	  PATH environment variable.
	*/
	char *pEnv = getenv( "PATH" );
	TQStringList paths( TQStringList::split(TQChar(':'), pEnv) );
	TQStringList::const_iterator p = paths.begin();
	while ( p != paths.end() ) {
	    TQString candidate = TQDir::current().absFilePath( *p + "/" + argv0 );
	    if ( TQFile::exists(candidate) ) {
		absPath = candidate;
		break;
	    }
	    ++p;
	}
    }

    absPath = TQDir::cleanDirPath( absPath );
    if ( TQFile::exists(absPath) ) {
	return resolveSymlinks( absPath );
    } else {
	return TQString::null;
    }
#endif
}
#endif // TQT_NO_DIR

#ifndef TQT_NO_COMPONENT

/*!
  Returns a list of paths that the application will search when
  dynamically loading libraries.
  The installation directory for plugins is the only entry if no
  paths have been set.  The default installation directory for plugins
  is \c INSTALL/plugins, where \c INSTALL is the directory where TQt was
  installed. The directory of the application executable (NOT the
  working directory) is also added to the plugin paths.

  If you want to iterate over the list, you should iterate over a
  copy, e.g.
    \code
    TQStringList list = app.libraryPaths();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

  See the \link plugins-howto.html plugins documentation\endlink for a
  description of how the library paths are used.

  \sa setLibraryPaths(), addLibraryPath(), removeLibraryPath(), TQLibrary
*/
TQStringList TQApplication::libraryPaths()
{
    if ( !app_libpaths ) {
	app_libpaths = new TQStringList;
	TQString installPathPlugins = TQString::fromLocal8Bit(tqInstallPathPlugins());
	if ( TQFile::exists(installPathPlugins) ) {
#ifdef TQ_WS_WIN
	    installPathPlugins.replace('\\', '/');
#endif
	    app_libpaths->append(installPathPlugins);
	}

	TQString app_location;
	if (tqApp)
	    app_location = tqApp->applicationFilePath();
#ifdef TQ_WS_WIN
	else {
	    app_location = TQString(tqAppFileName());
	    app_location.replace('\\', '/');
	}
#endif
	if (!app_location.isEmpty()) {
	    app_location.truncate( app_location.findRev( '/' ) );
	    if ( app_location != tqInstallPathPlugins() && TQFile::exists( app_location ) )
		app_libpaths->append( app_location );
	}
    }
    return *app_libpaths;
}


/*!
  Sets the list of directories to search when loading libraries to \a paths.
  All existing paths will be deleted and the path list will consist of the
  paths given in \a paths.

  \sa libraryPaths(), addLibraryPath(), removeLibraryPath(), TQLibrary
 */
void TQApplication::setLibraryPaths( const TQStringList &paths )
{
    delete app_libpaths;
    app_libpaths = new TQStringList( paths );
}

/*!
  Append \a path to the end of the library path list. If \a path is
  empty or already in the path list, the path list is not changed.

  The default path list consists of a single entry, the installation
  directory for plugins.  The default installation directory for plugins
  is \c INSTALL/plugins, where \c INSTALL is the directory where TQt was
  installed.

  \sa removeLibraryPath(), libraryPaths(), setLibraryPaths()
 */
void TQApplication::addLibraryPath( const TQString &path )
{
    if ( path.isEmpty() )
	return;

    // make sure that library paths is initialized
    libraryPaths();

    if ( !app_libpaths->contains( path ) )
	app_libpaths->prepend( path );
}

/*!
  Removes \a path from the library path list. If \a path is empty or not
  in the path list, the list is not changed.

  \sa addLibraryPath(), libraryPaths(), setLibraryPaths()
*/
void TQApplication::removeLibraryPath( const TQString &path )
{
    if ( path.isEmpty() )
	return;

    // make sure that library paths is initialized
    libraryPaths();

    if ( app_libpaths->contains( path ) )
	app_libpaths->remove( path );
}
#endif //TQT_NO_COMPONENT

/*!
  Returns the application palette.

  If a widget is passed in \a w, the default palette for the
  widget's class is returned. This may or may not be the application
  palette. In most cases there isn't a special palette for certain
  types of widgets, but one notable exception is the popup menu under
  Windows, if the user has defined a special background color for
  menus in the display settings.

  \sa setPalette(), TQWidget::palette()
*/
#ifndef TQT_NO_PALETTE
TQPalette TQApplication::palette(const TQWidget* w)
{
#if defined(QT_CHECK_STATE)
    if ( !tqApp )
	tqWarning( "TQApplication::palette: This function can only be "
		  "called after the TQApplication object has been created" );
#endif
    if ( !app_pal ) {
	if ( !tqt_std_pal )
	    tqt_create_std_palette();
	app_pal = new TQPalette( *tqt_std_pal );
	tqt_fix_tooltips();
    }

    if ( w && app_palettes ) {
	TQPalette* wp = app_palettes->find( w->className() );
	if ( wp )
	    return *wp;
	TQAsciiDictIterator<TQPalette> it( *app_palettes );
	const char* name;
	while ( (name=it.currentKey()) != 0 ) {
	    if ( w->inherits( name ) )
		return *it.current();
	    ++it;
	}
    }
    return *app_pal;
}

TQPalette TQApplication::palette(TQStringList objectTypeList)
{
#if defined(QT_CHECK_STATE)
    if ( !tqApp )
	tqWarning( "TQApplication::palette: This function can only be "
		  "called after the TQApplication object has been created" );
#endif
    if ( !app_pal ) {
	if ( !tqt_std_pal )
	    tqt_create_std_palette();
	app_pal = new TQPalette( *tqt_std_pal );
	tqt_fix_tooltips();
    }

	if ( (objectTypeList.count() > 0) && app_palettes ) {
		TQPalette* wp = app_palettes->find( objectTypeList[objectTypeList.count()-1] );
		if ( wp ) {
			return *wp;
		}
		TQAsciiDictIterator<TQPalette> it( *app_palettes );
		const char* name;
		while ( (name=it.currentKey()) != 0 ) {
			if ( objectTypeList.contains(name) ) {
				return *it.current();
			}
			++it;
		}
	}
	return *app_pal;
}

/*!
  Changes the default application palette to \a palette. If \a
  informWidgets is true, then existing widgets are informed about the
  change and may adjust themselves to the new application
  setting. If \a informWidgets is false, the change only affects newly
  created widgets.

  If \a className is passed, the change applies only to widgets that
  inherit \a className (as reported by TQObject::inherits()). If
  \a className is left 0, the change affects all widgets, thus overriding
  any previously set class specific palettes.

  The palette may be changed according to the current GUI style in
  TQStyle::polish().

  \sa TQWidget::setPalette(), palette(), TQStyle::polish()
*/

void TQApplication::setPalette( const TQPalette &palette, bool informWidgets,
			       const char* className )
{
    TQPalette pal = palette;
    TQPalette *oldpal = 0;
#ifndef TQT_NO_STYLE
    if ( !startingUp() ) // on startup this has been done already
	tqApp->style().polish( pal );	// NB: non-const reference
#endif
    bool all = false;
    if ( !className ) {
	if ( !app_pal ) {
	    app_pal = new TQPalette( pal );
	    TQ_CHECK_PTR( app_pal );
	} else {
	    *app_pal = pal;
	}
	all = app_palettes != 0;
	delete app_palettes;
	app_palettes = 0;
	tqt_fix_tooltips();
    } else {
	if ( !app_palettes ) {
	    app_palettes = new TQAsciiDict<TQPalette>;
	    TQ_CHECK_PTR( app_palettes );
	    app_palettes->setAutoDelete( true );
	}
	oldpal = app_palettes->find( className );
	app_palettes->insert( className, new TQPalette( pal ) );
    }
    if ( informWidgets && is_app_running && !is_app_closing ) {
	if ( !oldpal || ( *oldpal != pal ) ) {
	    TQEvent e( TQEvent::ApplicationPaletteChange );
	    TQWidgetIntDictIt it( *((TQWidgetIntDict*)TQWidget::mapper) );
	    TQWidget *w;
	    while ( (w=it.current()) ) {		// for all widgets...
		++it;
		if ( all || (!className && w->isTopLevel() ) || w->inherits(className) ) // matching class
		    sendEvent( w, &e );
	    }
	}
    }
}

#endif // TQT_NO_PALETTE

/*!
  Returns the default font for the widget \a w, or the default
  application font if \a w is 0.

  \sa setFont(), fontMetrics(), TQWidget::font()
*/

TQFont TQApplication::font( const TQWidget *w )
{
    if ( w && app_fonts ) {
	TQFont* wf = app_fonts->find( w->className() );
	if ( wf )
	    return *wf;
	TQAsciiDictIterator<TQFont> it( *app_fonts );
	const char* name;
	while ( (name=it.currentKey()) != 0 ) {
	    if ( w->inherits( name ) )
		return *it.current();
	    ++it;
	}
    }
    if ( !app_font ) {
	app_font = new TQFont( "Helvetica" );
	TQ_CHECK_PTR( app_font );
    }
    return *app_font;
}

/*! Changes the default application font to \a font. If \a
  informWidgets is true, then existing widgets are informed about the
  change and may adjust themselves to the new application
  setting. If \a informWidgets is false, the change only affects newly
  created widgets. If \a className is passed, the change applies only
  to classes that inherit \a className (as reported by
  TQObject::inherits()).

  On application start-up, the default font depends on the window
  system. It can vary depending on both the window system version and
  the locale. This function lets you override the default font; but
  overriding may be a bad idea because, for example, some locales need
  extra-large fonts to support their special characters.

  \sa font(), fontMetrics(), TQWidget::setFont()
*/

void TQApplication::setFont( const TQFont &font, bool informWidgets,
			    const char* className )
{
    bool all = false;
    if ( !className ) {
	tqt_app_has_font = true;
	if ( !app_font ) {
	    app_font = new TQFont( font );
	    TQ_CHECK_PTR( app_font );
	} else {
	    *app_font = font;
	}

	// make sure the application font is complete
	app_font->detach();
	app_font->d->mask = TQFontPrivate::Complete;

	all = app_fonts != 0;
	delete app_fonts;
	app_fonts = 0;
    } else {
	if (!app_fonts){
	    app_fonts = new TQAsciiDict<TQFont>;
	    TQ_CHECK_PTR( app_fonts );
	    app_fonts->setAutoDelete( true );
	}
	TQFont* fnt = new TQFont(font);
	TQ_CHECK_PTR( fnt );
	app_fonts->insert(className, fnt);
    }
    if ( informWidgets && is_app_running && !is_app_closing ) {
	TQEvent e( TQEvent::ApplicationFontChange );
	TQWidgetIntDictIt it( *((TQWidgetIntDict*)TQWidget::mapper) );
	TQWidget *w;
	while ( (w=it.current()) ) {		// for all widgets...
	    ++it;
	    if ( all || (!className && w->isTopLevel() ) || w->inherits(className) ) // matching class
		sendEvent( w, &e );
	}
    }
}


/*!
  Initialization of the appearance of the widget \a w \e before it is first
  shown.

  Usually widgets call this automatically when they are polished. It
  may be used to do some style-based central customization of widgets.

  Note that you are not limited to the public functions of TQWidget.
  Instead, based on meta information like TQObject::className() you are
  able to customize any kind of widget.

  \sa TQStyle::polish(), TQWidget::polish(), setPalette(), setFont()
*/

void TQApplication::polish( TQWidget *w )
{
#ifndef TQT_NO_STYLE
    w->style().polish( w );
#endif
}


/*!
  Returns a list of the top level widgets in the application.

  The list is created using \c new and must be deleted by the caller.

  The list is empty (TQPtrList::isEmpty()) if there are no top level
  widgets.

  Note that some of the top level widgets may be hidden, for example
  the tooltip if no tooltip is currently shown.

  Example:
  \code
    // Show all hidden top level widgets.
    TQWidgetList	 *list = TQApplication::topLevelWidgets();
    TQWidgetListIt it( *list );	// iterate over the widgets
    TQWidget * w;
    while ( (w=it.current()) != 0 ) {	// for each top level widget...
	++it;
	if ( !w->isVisible() )
	    w->show();
    }
    delete list;		// delete the list, not the widgets
  \endcode

  \warning Delete the list as soon you have finished using it.
  The widgets in the list may be deleted by someone else at any time.

  \sa allWidgets(), TQWidget::isTopLevel(), TQWidget::isVisible(),
      TQPtrList::isEmpty()
*/

TQWidgetList *TQApplication::topLevelWidgets()
{
    return TQWidget::tlwList();
}

/*!
  Returns a list of all the widgets in the application.

  The list is created using \c new and must be deleted by the caller.

  The list is empty (TQPtrList::isEmpty()) if there are no widgets.

  Note that some of the widgets may be hidden.

  Example that updates all widgets:
  \code
    TQWidgetList	 *list = TQApplication::allWidgets();
    TQWidgetListIt it( *list );         // iterate over the widgets
    TQWidget * w;
    while ( (w=it.current()) != 0 ) {  // for each widget...
	++it;
	w->update();
    }
    delete list;                      // delete the list, not the widgets
  \endcode

  The TQWidgetList class is defined in the \c ntqwidgetlist.h header
  file.

  \warning Delete the list as soon as you have finished using it.
  The widgets in the list may be deleted by someone else at any time.

  \sa topLevelWidgets(), TQWidget::isVisible(), TQPtrList::isEmpty(),
*/

TQWidgetList *TQApplication::allWidgets()
{
    return TQWidget::wList();
}

/*!
  \fn TQWidget *TQApplication::focusWidget() const

  Returns the application widget that has the keyboard input focus, or
  0 if no widget in this application has the focus.

  \sa TQWidget::setFocus(), TQWidget::hasFocus(), activeWindow()
*/

/*!
  \fn TQWidget *TQApplication::activeWindow() const

  Returns the application top-level window that has the keyboard input
  focus, or 0 if no application window has the focus. Note that
  there might be an activeWindow() even if there is no focusWidget(),
  for example if no widget in that window accepts key events.

  \sa TQWidget::setFocus(), TQWidget::hasFocus(), focusWidget()
*/

/*!
  Returns display (screen) font metrics for the application font.

  \sa font(), setFont(), TQWidget::fontMetrics(), TQPainter::fontMetrics()
*/

TQFontMetrics TQApplication::fontMetrics()
{
    return desktop()->fontMetrics();
}



/*!
  Tells the application to exit with return code 0 (success).
  Equivalent to calling TQApplication::exit( 0 ).

  It's common to connect the lastWindowClosed() signal to quit(), and
  you also often connect e.g. TQButton::clicked() or signals in
  TQAction, TQPopupMenu or TQMenuBar to it.

  Example:
  \code
    TQPushButton *quitButton = new TQPushButton( "Quit" );
    connect( quitButton, TQ_SIGNAL(clicked()), tqApp, TQ_SLOT(quit()) );
  \endcode

  \sa exit() aboutToQuit() lastWindowClosed() TQAction
*/

void TQApplication::quit()
{
    TQApplication::exit( 0 );
}


/*!
  Closes all top-level windows.

  This function is particularly useful for applications with many
  top-level windows. It could, for example, be connected to a "Quit"
  entry in the file menu as shown in the following code example:

  \code
    // the "Quit" menu entry should try to close all windows
    TQPopupMenu* file = new TQPopupMenu( this );
    file->insertItem( "&Quit", tqApp, TQ_SLOT(closeAllWindows()), CTRL+Key_Q );

    // when the last window is closed, the application should quit
    connect( tqApp, TQ_SIGNAL( lastWindowClosed() ), tqApp, TQ_SLOT( quit() ) );
  \endcode

  The windows are closed in random order, until one window does not
  accept the close event.

  \sa TQWidget::close(), TQWidget::closeEvent(), lastWindowClosed(),
  quit(), topLevelWidgets(), TQWidget::isTopLevel()

 */
void TQApplication::closeAllWindows()
{
    bool did_close = true;
    TQWidget *w;
    while((w = activeModalWidget()) && did_close) {
	if(w->isHidden())
	    break;
	did_close = w->close();
    }
    TQWidgetList *list = TQApplication::topLevelWidgets();
    for ( w = list->first(); did_close && w; ) {
	if ( !w->isHidden() ) {
	    did_close = w->close();
	    delete list;
	    list = TQApplication::topLevelWidgets();
	    w = list->first();
	} else {
	    w = list->next();
	}
    }
    delete list;
}

/*!
    Displays a simple message box about TQt. The message includes the
    version number of TQt being used by the application.

    This is useful for inclusion in the Help menu of an application.
    See the examples/menu/menu.cpp example.

    This function is a convenience slot for TQMessageBox::aboutTQt().
*/
void TQApplication::aboutTQt()
{
#ifndef TQT_NO_MESSAGEBOX
    TQMessageBox::aboutTQt( mainWidget() );
#endif // TQT_NO_MESSAGEBOX
}


/*!
  \fn void TQApplication::lastWindowClosed()

  This signal is emitted when the user has closed the last
  top level window.

  The signal is very useful when your application has many top level
  widgets but no main widget. You can then connect it to the quit()
  slot.

  For convenience, this signal is \e not emitted for transient top level
  widgets such as popup menus and dialogs.

  \sa mainWidget(), topLevelWidgets(), TQWidget::isTopLevel(), TQWidget::close()
*/

/*!
  \fn void TQApplication::aboutToQuit()

  This signal is emitted when the application is about to quit the
  main event loop, e.g. when the event loop level drops to zero.
  This may happen either after a call to quit() from inside the
  application or when the users shuts down the entire desktop session.

  The signal is particularly useful if your application has to do some
  last-second cleanup. Note that no user interaction is possible in
  this state.

  \sa quit()
*/


/*!
  \fn void TQApplication::guiThreadAwake()

  This signal is emitted after the event loop returns from a function
  that could block.

  \sa wakeUpGuiThread()
*/


/*!
  \fn bool TQApplication::sendEvent( TQObject *receiver, TQEvent *event )

  Sends event \a event directly to receiver \a receiver, using the
  notify() function. Returns the value that was returned from the event
  handler.

    The event is \e not deleted when the event has been sent. The normal
    approach is to create the event on the stack, e.g.
    \code
    TQMouseEvent me( TQEvent::MouseButtonPress, pos, 0, 0 );
    TQApplication::sendEvent( mainWindow, &me );
    \endcode
    If you create the event on the heap you must delete it.

  \sa postEvent(), notify()
*/

bool TQApplication::sendEvent( TQObject *receiver, TQEvent *event ) {
	if ( event ) event->spont = false;
	return tqApp ? tqApp->notify( receiver, event ) : false;
}

bool TQApplication::sendSpontaneousEvent( TQObject *receiver, TQEvent *event ) {
	if ( event ) event->spont = true;
	return tqApp ? tqApp->notify( receiver, event ) : false;
}

/*!
  Sends event \a e to \a receiver: \a {receiver}->event(\a e).
  Returns the value that is returned from the receiver's event handler.

  For certain types of events (e.g. mouse and key events),
  the event will be propagated to the receiver's parent and so on up to
  the top-level object if the receiver is not interested in the event
  (i.e., it returns false).

  There are five different ways that events can be processed;
  reimplementing this virtual function is just one of them. All five
  approaches are listed below:
  \list 1
  \i Reimplementing this function. This is very powerful, providing
  complete control; but only one subclass can be tqApp.

  \i Installing an event filter on tqApp. Such an event filter is able
  to process all events for all widgets, so it's just as powerful as
  reimplementing notify(); furthermore, it's possible to have more
  than one application-global event filter. Global event filters even
  see mouse events for \link TQWidget::isEnabled() disabled
  widgets, \endlink and if \link setGlobalMouseTracking() global mouse
  tracking \endlink is enabled, as well as mouse move events for all
  widgets.

  \i Reimplementing TQObject::event() (as TQWidget does). If you do
  this you get Tab key presses, and you get to see the events before
  any widget-specific event filters.

  \i Installing an event filter on the object. Such an event filter
  gets all the events except Tab and Shift-Tab key presses.

  \i Reimplementing paintEvent(), mousePressEvent() and so
  on. This is the commonest, easiest and least powerful way.
  \endlist

  \sa TQObject::event(), installEventFilter()
*/

bool TQApplication::notify( TQObject *receiver, TQEvent *e )
{
    // no events are delivered after ~TQApplication() has started
    if ( is_app_closing ) {
	return false;
    }

    if ( receiver == 0 ) {			// serious error
#if defined(QT_CHECK_NULL)
	tqWarning( "TQApplication::notify: Unexpected null receiver" );
#endif
	return false;
    }

    if ( receiver && (e->type() == TQEvent::Destroy) ) {
	return true;
    }

    if ( e->type() == TQEvent::ChildRemoved && receiver->postedEvents) {
#ifdef TQT_THREAD_SUPPORT
	TQMutexLocker locker( GlobalPostedEvents()->mutex() );
#endif // TQT_THREAD_SUPPORT

	// the TQObject destructor calls TQObject::removeChild, which calls
	// TQApplication::sendEvent() directly.  this can happen while the event
	// loop is in the middle of posting events, and when we get here, we may
	// not have any more posted events for this object.
	if ( receiver->postedEvents ) {
		// if this is a child remove event and the child insert
		// hasn't been dispatched yet, kill that insert
		TQPostEventList * l = receiver->postedEvents;
		TQObject * c = ((TQChildEvent*)e)->child();
		TQPostEvent * pe;
		l->first();
		while( ( pe = l->current()) != 0 ) {
			if ( pe->event && pe->receiver == receiver &&
				pe->event->type() == TQEvent::ChildInserted &&
				((TQChildEvent*)pe->event)->child() == c ) {
				pe->event->posted = false;
				delete pe->event;
				pe->event = 0;
				l->remove();
				continue;
			}
			l->next();
		}
	}
    }

    bool res = false;
    if ( !receiver->isWidgetType() ) {
	res = internalNotify( receiver, e );
    }
    else switch ( e->type() ) {
#ifndef TQT_NO_ACCEL
    case TQEvent::Accel:
	{
	    TQKeyEvent* key = (TQKeyEvent*) e;
	    res = internalNotify( receiver, e );

	    if ( !res && !key->isAccepted() ) {
		res = tqt_dispatchAccelEvent( (TQWidget*)receiver, key );
	    }

	    // next lines are for compatibility with TQt <= 3.0.x: old
	    // TQAccel was listening on toplevel widgets
	    if ( !res && !key->isAccepted() && !((TQWidget*)receiver)->isTopLevel() ) {
		res = internalNotify( ((TQWidget*)receiver)->topLevelWidget(), e );
	    }
	}
    break;
#endif //TQT_NO_ACCEL
    case TQEvent::KeyPress:
    case TQEvent::KeyRelease:
    case TQEvent::AccelOverride:
	{
	    TQWidget* w = (TQWidget*)receiver;
	    TQKeyEvent* key = (TQKeyEvent*) e;
#ifndef TQT_NO_ACCEL
	    if ( tqt_tryComposeUnicode( w, key ) )
		break;
#endif
	    bool def = key->isAccepted();
	    while ( w ) {
		if ( def )
		    key->accept();
		else
		    key->ignore();
		res = internalNotify( w, e );
		if ( res || key->isAccepted() )
		    break;
		w = w->parentWidget( true );
	    }
	}
    break;
    case TQEvent::MouseButtonPress:
	    if ( e->spontaneous() ) {
		TQWidget* fw = (TQWidget*)receiver;
		while ( fw->focusProxy() )
		    fw = fw->focusProxy();
		if ( fw->isEnabled() && fw->focusPolicy() & TQWidget::ClickFocus ) {
		    TQFocusEvent::setReason( TQFocusEvent::Mouse);
		    fw->setFocus();
		    TQFocusEvent::resetReason();
		}
	    }
	    // fall through intended
    case TQEvent::MouseButtonRelease:
    case TQEvent::MouseButtonDblClick:
    case TQEvent::MouseMove:
	{
	    TQWidget* w = (TQWidget*)receiver;
	    TQMouseEvent* mouse = (TQMouseEvent*) e;
	    TQPoint relpos = mouse->pos();
	    while ( w ) {
		TQMouseEvent me(mouse->type(), relpos, mouse->globalPos(), mouse->button(), mouse->state());
		me.spont = mouse->spontaneous();
		res = internalNotify( w, w == receiver ? mouse : &me );
		e->spont = false;
		if (res || w->isTopLevel() || w->testWFlags(WNoMousePropagation))
		    break;

		relpos += w->pos();
		w = w->parentWidget();
	    }
	    if ( res )
		mouse->accept();
	    else
		mouse->ignore();
	}
    break;
#ifndef TQT_NO_WHEELEVENT
    case TQEvent::Wheel:
	{
	    if ( e->spontaneous() ) {
		TQWidget* fw = (TQWidget*)receiver;
		while ( fw->focusProxy() )
		    fw = fw->focusProxy();
		if ( fw->isEnabled() && (fw->focusPolicy() & TQWidget::WheelFocus) == TQWidget::WheelFocus ) {
		    TQFocusEvent::setReason( TQFocusEvent::Mouse);
		    fw->setFocus();
		    TQFocusEvent::resetReason();
		}
	    }

	    TQWidget* w = (TQWidget*)receiver;
	    TQWheelEvent* wheel = (TQWheelEvent*) e;
	    TQPoint relpos = wheel->pos();
	    while ( w ) {
		TQWheelEvent we(relpos, wheel->globalPos(), wheel->delta(), wheel->state(), wheel->orientation());
		we.spont = wheel->spontaneous();
		res = internalNotify( w,  w == receiver ? wheel : &we );
		e->spont = false;
		if (res || w->isTopLevel() || w->testWFlags(WNoMousePropagation))
		    break;

		relpos += w->pos();
		w = w->parentWidget();
	    }
	    if ( res )
		wheel->accept();
	    else
		wheel->ignore();
	}
    break;
#endif
    case TQEvent::ContextMenu:
	{
	    TQWidget* w = (TQWidget*)receiver;
	    TQContextMenuEvent *context = (TQContextMenuEvent*) e;
	    TQPoint relpos = context->pos();
	    while ( w ) {
		TQContextMenuEvent ce(context->reason(), relpos, context->globalPos(), context->state());
		ce.spont = e->spontaneous();
		res = internalNotify( w,  w == receiver ? context : &ce );
		e->spont = false;

		if (res || w->isTopLevel() || w->testWFlags(WNoMousePropagation))
		    break;

		relpos += w->pos();
		w = w->parentWidget();
	    }
	    if ( res )
		context->accept();
	    else
		context->ignore();
	}
    break;
#if defined (QT_TABLET_SUPPORT)
    case TQEvent::TabletMove:
    case TQEvent::TabletPress:
    case TQEvent::TabletRelease:
	{
	    TQWidget *w = (TQWidget*)receiver;
	    TQTabletEvent *tablet = (TQTabletEvent*)e;
	    TQPoint relpos = tablet->pos();
	    while ( w ) {
		TQTabletEvent te(tablet->pos(), tablet->globalPos(), tablet->device(),
				tablet->pressure(), tablet->xTilt(), tablet->yTilt(),
				tablet->uniqueId());
		te.spont = e->spontaneous();
		res = internalNotify( w, w == receiver ? tablet : &te );
		e->spont = false;
		if (res || w->isTopLevel() || w->testWFlags(WNoMousePropagation))
		    break;

		relpos += w->pos();
		w = w->parentWidget();
	    }
	    if ( res )
		tablet->accept();
	    else
		tablet->ignore();
	    chokeMouse = tablet->isAccepted();
	}
    break;
#endif
    default:
	res = internalNotify( receiver, e );
	break;
    }

    return res;
}

/*!\reimp

*/
bool TQApplication::event( TQEvent *e )
{
    if(e->type() == TQEvent::Close) {
	TQCloseEvent *ce = (TQCloseEvent*)e;
	ce->accept();
	closeAllWindows();

	TQWidgetList *list = topLevelWidgets();
	for(TQWidget *w = list->first(); w; w = list->next()) {
	    if ( !w->isHidden() && !w->isDesktop() && !w->isPopup() &&
		 (!w->isDialog() || !w->parentWidget())) {
		ce->ignore();
		break;
	    }
	}
	if(ce->isAccepted())
	    return true;
    } else if (e->type() == TQEvent::Quit) {
	quit();
	return true;
    }
    return TQObject::event(e);
}

#define HOVER_SENSITIVE_WIDGET_SELECT		if ( widget->inherits("TQPushButton")				\
							|| widget->inherits("TQComboBox")			\
							|| widget->inherits("TQSpinWidget")			\
							|| widget->inherits("TQCheckBox")			\
							|| widget->inherits("TQRadioButton")			\
							|| widget->inherits("TQToolButton")			\
							|| widget->inherits("TQSlider")				\
							|| widget->inherits("TQScrollBar")			\
							|| widget->inherits("TQTabBar")				\
							|| widget->inherits("TQDockWindowHandle")		\
							|| widget->inherits("TQSplitterHandle") )

#define FOCUS_SENSITIVE_WIDGET_SELECT		if ( widget->inherits("TQLineEdit") )
#define FOCUS_SENSITIVE_PARENT_WIDGET_SELECT	if ( widget->parentWidget() && widget->parentWidget()->inherits("TQSpinWidget") )

/*!\internal

  Helper function called by notify()
 */
bool TQApplication::internalNotify( TQObject *receiver, TQEvent * e)
{
    if ( eventFilters ) {
	TQObjectListIt it( *eventFilters );
	TQObject *obj;
	while ( (obj=it.current()) != 0 ) {	// send to all filters
	    ++it;				//   until one returns true
	    if ( obj->eventFilter(receiver,e) )
		return true;
	}
    }

    bool consumed = false;
    bool handled = false;
    if ( receiver->isWidgetType() ) {
	TQWidget *widget = (TQWidget*)receiver;

	// toggle HasMouse widget state on enter and leave
	if ( e->type() == TQEvent::Enter || e->type() == TQEvent::DragEnter ) {
	    widget->setWState( WState_HasMouse );
	    HOVER_SENSITIVE_WIDGET_SELECT {
	    	widget->repaint(false);
	    }
	}
	else if ( e->type() == TQEvent::Leave || e->type() == TQEvent::DragLeave ) {
	    widget->clearWState( WState_HasMouse );
	    HOVER_SENSITIVE_WIDGET_SELECT {
	    	widget->repaint(false);
	    }
	}

	// repaint information entry widgets on focus set/unset
	if ( e->type() == TQEvent::FocusIn || e->type() == TQEvent::FocusOut ) {
	    FOCUS_SENSITIVE_WIDGET_SELECT {
	    	widget->repaint(false);
	    }
	    FOCUS_SENSITIVE_PARENT_WIDGET_SELECT {
	    	widget->parentWidget()->repaint(false);
	    }
	}

	// throw away any mouse-tracking-only mouse events
	if ( e->type() == TQEvent::MouseMove &&
	     (((TQMouseEvent*)e)->state()&TQMouseEvent::MouseButtonMask) == 0 &&
	     !widget->hasMouseTracking() ) {
	    handled = true;
	    consumed = true;
	} else if ( !widget->isEnabled() ) { // throw away mouse events to disabled widgets
	    switch(e->type()) {
	    case TQEvent::MouseButtonPress:
	    case TQEvent::MouseButtonRelease:
	    case TQEvent::MouseButtonDblClick:
	    case TQEvent::MouseMove:
		( (TQMouseEvent*) e)->ignore();
		handled = true;
		consumed = true;
		break;
#ifndef TQT_NO_DRAGANDDROP
	    case TQEvent::DragEnter:
	    case TQEvent::DragMove:
		( (TQDragMoveEvent*) e)->ignore();
		handled = true;
		break;

	    case TQEvent::DragLeave:
	    case TQEvent::DragResponse:
		handled = true;
		break;

	    case TQEvent::Drop:
		( (TQDropEvent*) e)->ignore();
		handled = true;
		break;
#endif
#ifndef TQT_NO_WHEELEVENT
	    case TQEvent::Wheel:
		( (TQWheelEvent*) e)->ignore();
		handled = true;
		break;
#endif
	    case TQEvent::ContextMenu:
		( (TQContextMenuEvent*) e)->ignore();
		handled = true;
		break;
	    default:
		break;
	    }
	}

    }

    if (!handled) {
#if defined(TQT_THREAD_SUPPORT)
	int locklevel = 0;
	int llcount;
	if (TQApplication::tqt_mutex) {
	    TQApplication::tqt_mutex->lock();	// 1 of 2
	    locklevel = tqt_mutex->level() - 1;
	    for (llcount=0; llcount<locklevel; llcount++) {
		TQApplication::tqt_mutex->unlock();
	    }
	    TQApplication::tqt_mutex->unlock();	// 2 of 2
	}
#endif
	consumed = receiver->event( e );
#if defined(TQT_THREAD_SUPPORT)
	if (TQApplication::tqt_mutex) {
	    for (llcount=0; llcount<locklevel; llcount++) {
		TQApplication::tqt_mutex->lock();
	    }
	}
#endif
    }
    e->spont = false;
    return consumed;
}

/*!
  Returns true if an application object has not been created yet;
  otherwise returns false.

  \sa closingDown()
*/

bool TQApplication::startingUp()
{
    return !is_app_running;
}

/*!
  Returns true if the application objects are being destroyed;
  otherwise returns false.

  \sa startingUp()
*/

bool TQApplication::closingDown()
{
    return is_app_closing;
}


/*!
    Processes pending events, for 3 seconds or until there are no more
    events to process, whichever is shorter.

    You can call this function occasionally when your program is busy
    performing a long operation (e.g. copying a file).

    \sa exec(), TQTimer, TQEventLoop::processEvents()
*/

void TQApplication::processEvents()
{
    processEvents( 3000 );
}

/*!
    \overload

    Processes pending events for \a maxtime milliseconds or until
    there are no more events to process, whichever is shorter.

    You can call this function occasionally when you program is busy
    doing a long operation (e.g. copying a file).

    \sa exec(), TQTimer, TQEventLoop::processEvents()
*/
void TQApplication::processEvents( int maxtime )
{
    eventLoop()->processEvents( TQEventLoop::AllEvents, maxtime );
}

/*! \obsolete
  Waits for an event to occur, processes it, then returns.

  This function is useful for adapting TQt to situations where the
  event processing must be grafted onto existing program loops.

  Using this function in new applications may be an indication of design
  problems.

  \sa processEvents(), exec(), TQTimer
*/

void TQApplication::processOneEvent()
{
    eventLoop()->processEvents( TQEventLoop::AllEvents |
				TQEventLoop::WaitForMore );
}

/*****************************************************************************
  Main event loop wrappers
 *****************************************************************************/

/*!
    Returns the application event loop. This function will return
    zero if called during and after destroying TQApplication.

    To create your own instance of TQEventLoop or TQEventLoop subclass create
    it before you create the TQApplication object.

    \sa TQEventLoop
*/
TQEventLoop *TQApplication::eventLoop()
{
    if ( !currentEventLoop() && !is_app_closing ) {
	(void) new TQEventLoop( tqApp, "default event loop" );
    }
    return currentEventLoop();
}


/*!
    Enters the main event loop and waits until exit() is called or the
    main widget is destroyed, and returns the value that was set to
    exit() (which is 0 if exit() is called via quit()).

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

    \sa quit(), exit(), processEvents(), setMainWidget()
*/
int TQApplication::exec()
{
    return eventLoop()->exec();
}

/*!
  Tells the application to exit with a return code.

  After this function has been called, the application leaves the main
  event loop and returns from the call to exec(). The exec() function
  returns \a retcode.

  By convention, a \a retcode of 0 means success, and any non-zero
  value indicates an error.

  Note that unlike the C library function of the same name, this
  function \e does return to the caller -- it is event processing that
  stops.

  \sa quit(), exec()
*/
void TQApplication::exit( int retcode )
{
#ifdef TQT_THREAD_SUPPORT
    TQThread* thread = tqApp->guiThread();
    if (thread) {
        if (thread->d) {
            if (thread->d->eventLoop) {
                thread->d->eventLoop->exit( retcode );
            }
        }
    }
#else
    tqApp->eventLoop()->exit( retcode );
#endif // TQT_THREAD_SUPPORT
}

/*!
    \obsolete

    This function enters the main event loop (recursively). Do not call
    it unless you really know what you are doing.

    Use TQApplication::eventLoop()->enterLoop() instead.

*/
int TQApplication::enter_loop()
{
    return eventLoop()->enterLoop();
}

/*!
    \obsolete

    This function exits from a recursive call to the main event loop.
    Do not call it unless you are an expert.

    Use TQApplication::eventLoop()->exitLoop() instead.

*/
void TQApplication::exit_loop()
{
    eventLoop()->exitLoop();
}

/*!
    \obsolete

    Returns the current loop level.

    Use TQApplication::eventLoop()->loopLevel() instead.

*/
int TQApplication::loopLevel() const
{
    return eventLoop()->loopLevel();
}

/*!

  Wakes up the GUI thread.

  \sa guiThreadAwake() \link threads.html Thread Support in TQt\endlink
*/
void TQApplication::wakeUpGuiThread()
{
    eventLoop()->wakeUp();
}

/*!
    This function returns true if there are pending events; otherwise
    returns false. Pending events can be either from the window system
    or posted events using TQApplication::postEvent().
*/
bool TQApplication::hasPendingEvents()
{
    return eventLoop()->hasPendingEvents();
}

#if !defined(TQ_WS_X11)

// The doc and X implementation of these functions is in qapplication_x11.cpp

void TQApplication::flushX()	{}		// do nothing

void TQApplication::syncX()	{}		// do nothing

#endif

/*!
  \fn void TQApplication::setWinStyleHighlightColor( const TQColor & )
  \obsolete

  Sets the color used to mark selections in windows style for all widgets
  in the application. Will repaint all widgets if the color is changed.

  The default color is \c darkBlue.
  \sa winStyleHighlightColor()
*/

/*!
  \fn const TQColor& TQApplication::winStyleHighlightColor()
  \obsolete

  Returns the color used to mark selections in windows style.

  \sa setWinStyleHighlightColor()
*/

/*!
  Returns the version of the Windows operating system that is running:

  \list
  \i TQt::WV_95 - Windows 95
  \i TQt::WV_98 - Windows 98
  \i TQt::WV_Me - Windows Me
  \i TQt::WV_NT - Windows NT 4.x
  \i TQt::WV_2000 - Windows 2000 (NT5)
  \i TQt::WV_XP - Windows XP
  \i TQt::WV_2003 - Windows Server 2003 family
  \i TQt::WV_CE - Windows CE
  \i TQt::WV_CENET - Windows CE.NET
  \endlist

  Note that this function is implemented for the Windows version
  of TQt only.
*/

#if defined(Q_OS_CYGWIN)
TQt::WindowsVersion TQApplication::winVersion()
{
    return tqt_winver;
}
#endif

#ifndef TQT_NO_TRANSLATION

bool tqt_detectRTLLanguage()
{
    return TQApplication::tr( "QT_LAYOUT_DIRECTION",
	    "Translate this string to the string 'LTR' in left-to-right"
	    " languages or to 'RTL' in right-to-left languages (such as Hebrew"
	    " and Arabic) to get proper widget layout." ) == "RTL";
}

/*!
  Adds the message file \a mf to the list of message files to be used
  for translations.

  Multiple message files can be installed. Translations are searched
  for in the last installed message file, then the one from last, and
  so on, back to the first installed message file. The search stops as
  soon as a matching translation is found.

  \sa removeTranslator() translate() TQTranslator::load()
*/

void TQApplication::installTranslator( TQTranslator * mf )
{
    if ( !mf )
	return;
    if ( !translators )
	translators = new TQValueList<TQTranslator*>;

    translators->prepend( mf );

#ifndef TQT_NO_TRANSLATION_BUILDER
    if ( mf->isEmpty() )
	return;
#endif

    // hook to set the layout direction of dialogs
    setReverseLayout( tqt_detectRTLLanguage() );

    TQWidgetList *list = topLevelWidgets();
    TQWidgetListIt it( *list );
    TQWidget *w;
    while ( ( w=it.current() ) != 0 ) {
	++it;
	if (!w->isDesktop())
	    postEvent( w, new TQEvent( TQEvent::LanguageChange ) );
    }
    delete list;
}

/*!
  Removes the message file \a mf from the list of message files used by
  this application. (It does not delete the message file from the file
  system.)

  \sa installTranslator() translate(), TQObject::tr()
*/

void TQApplication::removeTranslator( TQTranslator * mf )
{
    if ( !translators || !mf )
	return;

    if ( translators->remove( mf ) && ! tqApp->closingDown() ) {
	setReverseLayout( tqt_detectRTLLanguage() );

	TQWidgetList *list = topLevelWidgets();
	TQWidgetListIt it( *list );
	TQWidget *w;
	while ( ( w=it.current() ) != 0 ) {
	    ++it;
	    postEvent( w, new TQEvent( TQEvent::LanguageChange ) );
	}
	delete list;
    }
}

#ifndef TQT_NO_TEXTCODEC
/*! \obsolete
  This is the same as TQTextCodec::setCodecForTr().
*/
void TQApplication::setDefaultCodec( TQTextCodec* codec )
{
    TQTextCodec::setCodecForTr( codec );
}

/*! \obsolete
  Returns TQTextCodec::codecForTr().
*/
TQTextCodec* TQApplication::defaultCodec() const
{
    return TQTextCodec::codecForTr();
}
#endif //TQT_NO_TEXTCODEC

/*! \enum TQApplication::Encoding

  This enum type defines the 8-bit encoding of character string
  arguments to translate():

  \value DefaultCodec - the encoding specified by
  TQTextCodec::codecForTr() (Latin-1 if none has been set)
  \value UnicodeUTF8 - UTF-8

  \sa TQObject::tr(), TQObject::trUtf8(), TQString::fromUtf8()
*/

/*! \reentrant
  Returns the translation text for \a sourceText, by querying the
  installed messages files. The message files are searched from the most
  recently installed message file back to the first installed message
  file.

  TQObject::tr() and TQObject::trUtf8() provide this functionality more
  conveniently.

  \a context is typically a class name (e.g., "MyDialog") and
  \a sourceText is either English text or a short identifying text, if
  the output text will be very long (as for help texts).

  \a comment is a disambiguating comment, for when the same \a
  sourceText is used in different roles within the same context. By
  default, it is null. \a encoding indicates the 8-bit encoding of
  character stings

  See the \l TQTranslator documentation for more information about
  contexts and comments.

  If none of the message files contain a translation for \a
  sourceText in \a context, this function returns a TQString
  equivalent of \a sourceText. The encoding of \a sourceText is
  specified by \e encoding; it defaults to \c DefaultCodec.

  This function is not virtual. You can use alternative translation
  techniques by subclassing \l TQTranslator.

  \warning This method is reentrant only if all translators are
  installed \e before calling this method.  Installing or removing
  translators while performing translations is not supported.  Doing
  so will most likely result in crashes or other undesirable behavior.

  \sa TQObject::tr() installTranslator() defaultCodec()
*/

TQString TQApplication::translate( const char * context, const char * sourceText,
				 const char * comment, Encoding encoding ) const
{
    if ( !sourceText )
	return TQString::null;

    if ( translators ) {
	TQValueList<TQTranslator*>::iterator it;
	TQTranslator * mf;
	TQString result;
	for ( it = translators->begin(); it != translators->end(); ++it ) {
	    mf = *it;
	    result = mf->findMessage( context, sourceText, comment ).translation();
	    if ( !result.isNull() )
		return result;
	}
    }
#ifndef TQT_NO_TEXTCODEC
    if ( encoding == UnicodeUTF8 )
	return TQString::fromUtf8( sourceText );
    else if ( TQTextCodec::codecForTr() != 0 )
	return TQTextCodec::codecForTr()->toUnicode( sourceText );
    else
#endif
	return TQString::fromLatin1( sourceText );
}

#endif

/*****************************************************************************
  TQApplication management of posted events
 *****************************************************************************/

//see also notify(), which does the removal of ChildInserted when ChildRemoved.

/*!
  Adds the event \a event with the object \a receiver as the receiver of the
  event, to an event queue and returns immediately.

  The event must be allocated on the heap since the post event queue
  will take ownership of the event and delete it once it has been posted.

  When control returns to the main event loop, all events that are
  stored in the queue will be sent using the notify() function.

  \threadsafe

  \sa sendEvent(), notify()
*/

void TQApplication::postEvent( TQObject *receiver, TQEvent *event )
{
    if ( receiver == 0 ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQApplication::postEvent: Unexpected null receiver" );
#endif
	delete event;
	return;
    }

#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker locker( GlobalPostedEvents()->mutex() );
#endif // TQT_THREAD_SUPPORT

    if ( !receiver->postedEvents ) {
	receiver->postedEvents = new TQPostEventList;
    }
    TQPostEventList * l = receiver->postedEvents;

    // if this is one of the compressible events, do compression
    if ( event->type() == TQEvent::Paint ||
	 event->type() == TQEvent::LayoutHint ||
	 event->type() == TQEvent::Resize ||
	 event->type() == TQEvent::Move ||
	 event->type() == TQEvent::LanguageChange ) {
	l->first();
	TQPostEvent * cur = 0;
	for ( ;; ) {
	    while ( (cur=l->current()) != 0 &&
		    ( cur->receiver != receiver ||
		      cur->event == 0 ||
		      cur->event->type() != event->type() ) )
		l->next();
	    if ( l->current() != 0 ) {
		if ( cur->event->type() == TQEvent::Paint ) {
		    TQPaintEvent * p = (TQPaintEvent*)(cur->event);
		    if ( p->erase != ((TQPaintEvent*)event)->erase ) {
			l->next();
			continue;
		    }
		    p->reg = p->reg.unite( ((TQPaintEvent *)event)->reg );
		    p->rec = p->rec.unite( ((TQPaintEvent *)event)->rec );
		    delete event;
		    return;
		} else if ( cur->event->type() == TQEvent::LayoutHint ) {
		    delete event;
		    return;
		} else if ( cur->event->type() == TQEvent::Resize ) {
		    ((TQResizeEvent *)(cur->event))->s = ((TQResizeEvent *)event)->s;
		    delete event;
		    return;
		} else if ( cur->event->type() == TQEvent::Move ) {
		    ((TQMoveEvent *)(cur->event))->p = ((TQMoveEvent *)event)->p;
		    delete event;
		    return;
		} else if ( cur->event->type() == TQEvent::LanguageChange ) {
		    delete event;
		    return;
		}
	    }
	    break;
	};
    }

#if !defined(TQT_NO_IM)
    // if this is one of the compressible IM events, do compression
    else if ( event->type() == TQEvent::IMCompose ) {
	l->last();
	TQPostEvent * cur = 0;
	for ( ;; ) {
	    while ( (cur=l->current()) != 0 &&
		    ( cur->receiver != receiver ||
		      cur->event == 0 ||
		      cur->event->type() != event->type() || 
		      cur->event->type() != TQEvent::IMStart ) )
		l->prev();
	    if ( l->current() != 0 ) {
		// IMCompose must not be compressed with another one
		// beyond its IMStart boundary
		if ( cur->event->type() == TQEvent::IMStart ) {
		    break;
		} else if ( cur->event->type() == TQEvent::IMCompose ) {
		    TQIMComposeEvent * e = (TQIMComposeEvent *)(cur->event);
		    *e = *(TQIMComposeEvent *)event;
		    delete event;
		    return;
		}
	    }
	    break;
	};
    }
#endif

    // if no compression could be done, just append something
    event->posted = true;
    TQPostEvent * pe = new TQPostEvent( receiver, event );
    l->append( pe );
    GlobalPostedEvents()->append( pe );

#ifdef TQT_THREAD_SUPPORT
     // Wake up the receiver thread event loop
     TQThread* thread = receiver->contextThreadObject();
     if (thread) {
	if (thread->d) {
		if (thread->d->eventLoop) {
			thread->d->eventLoop->wakeUp();
			return;
		}
	}
     }
     if ( event->type() == TQEvent::MetaCall ) {
	return;
     }
#endif

    if (currentEventLoop()) {
	currentEventLoop()->wakeUp();
    }
}


/*! \overload

    Dispatches all posted events, i.e. empties the event queue.
*/
void TQApplication::sendPostedEvents()
{
    sendPostedEvents( 0, 0 );
}



/*!
  Immediately dispatches all events which have been previously queued
  with TQApplication::postEvent() and which are for the object \a receiver
  and have the event type \a event_type.

  Note that events from the window system are \e not dispatched by this
  function, but by processEvents().

  If \a receiver is null, the events of \a event_type are sent for all
  objects. If \a event_type is 0, all the events are sent for \a receiver.
*/

void TQApplication::sendPostedEvents( TQObject *receiver, int event_type )
{
    // Make sure the object hierarchy is stable before processing events
    // to avoid endless loops
    if ( receiver == 0 && event_type == 0 ) {
	sendPostedEvents( 0, TQEvent::ChildInserted );
    }

#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker locker( GlobalPostedEvents()->mutex() );
#endif

    bool sent = true;
    while ( sent ) {
	sent = false;

	if (receiver && !receiver->postedEvents) {
	    return;
	}

	// if we have a receiver, use the local list. Otherwise, use the
	// global list
	TQPostEventList * l = receiver ? receiver->postedEvents : GlobalPostedEvents();

	// okay. here is the tricky loop. be careful about optimizing
	// this, it looks the way it does for good reasons.
	TQPostEventListIt it( *l );
	TQPostEvent *pe;
	while ( (pe=it.current()) != 0 ) {
	    ++it;
	    Q_ASSERT(pe->receiver);
	    if ( pe->event // hasn't been sent yet
		 && ( receiver == 0 // we send to all receivers
		      || receiver == pe->receiver ) // we send to THAT receiver
		 && ( event_type == 0 // we send all types
		      || event_type == pe->event->type() ) // we send THAT type
		 && ( !pe->receiver->wasDeleted ) // don't send if receiver was deleted
#ifdef TQT_THREAD_SUPPORT
		 // only send if active thread is receiver object owning thread
		 && ( pe->receiver->contextThreadObject() == TQThread::currentThreadObject() )
#endif
		) {
		// first, we diddle the event so that we can deliver
		// it, and that noone will try to touch it later.
		pe->event->posted = false;
		TQEvent * e = pe->event;
		TQObject * r = pe->receiver;
		pe->event = 0;

		// next, update the data structure so that we're ready
		// for the next event.

		// look for the local list, and take whatever we're
		// delivering out of it. r->postedEvents maybe *l
		if ( r->postedEvents ) {
		    r->postedEvents->removeRef( pe );
		    // if possible, get rid of that list. this is not
		    // ideal - we will create and delete a list for
		    // each update() call. it would be better if we'd
		    // leave the list empty here, and delete it
		    // somewhere else if it isn't being used.
		    if ( r->postedEvents->isEmpty() ) {
			delete r->postedEvents;
			r->postedEvents = 0;
		    }
		}

#ifdef TQT_THREAD_SUPPORT
		if ( locker.mutex() ) locker.mutex()->unlock();
#endif // TQT_THREAD_SUPPORT
		// after all that work, it's time to deliver the event.
		if ( e->type() == TQEvent::Paint && r->isWidgetType() ) {
		    TQWidget * w = (TQWidget*)r;
		    TQPaintEvent * p = (TQPaintEvent*)e;
		    if ( w->isVisible() ) {
			w->repaint( p->reg, p->erase );
		    }
		} else {
		    sent = true;
		    TQApplication::sendEvent( r, e );
		}
#ifdef TQT_THREAD_SUPPORT
		if ( locker.mutex() ) locker.mutex()->lock();
#endif // TQT_THREAD_SUPPORT

		delete e;
		// careful when adding anything below this point - the
		// sendEvent() call might invalidate any invariants this
		// function depends on.
	    }
	}

	// clear the global list, i.e. remove everything that was
	// delivered.
	if ( l == GlobalPostedEvents() ) {
	    GlobalPostedEvents()->first();
	    while( (pe=GlobalPostedEvents()->current()) != 0 ) {
		if ( pe->event ) {
		    GlobalPostedEvents()->next();
		}
		else {
		    GlobalPostedEvents()->remove();
		}
	    }
	}
    }
}

/*!
  Removes all events posted using postEvent() for \a receiver.

  The events are \e not dispatched, instead they are removed from the
  queue. You should never need to call this function. If you do call it,
  be aware that killing events may cause \a receiver to break one or
  more invariants.

  \threadsafe
*/

void TQApplication::removePostedEvents( TQObject *receiver )
{
    removePostedEvents( receiver, 0 );
}

/*!
  Removes all events that have the event type \a event_type posted
  using postEvent() for \a receiver.

  The events are \e not dispatched, instead they are removed from the
  queue.

  If \a event_type is 0, all the events are removed from the queue.

  \threadsafe
*/

void TQApplication::removePostedEvents( TQObject *receiver, int event_type )
{
    if ( !receiver ) {
	return;
    }

#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker locker( GlobalPostedEvents()->mutex() );
#endif // TQT_THREAD_SUPPORT

    // the TQObject destructor calls this function directly.  this can
    // happen while the event loop is in the middle of posting events,
    // and when we get here, we may not have any more posted events
    // for this object.
    if ( !receiver->postedEvents ) {
	return;
    }

    // iterate over the object-specifc list and delete the events.
    // leave the TQPostEvent objects; they'll be deleted by
    // sendPostedEvents().
    TQPostEventList * l = receiver->postedEvents;
    l->first();
    TQPostEvent * pe;
    while( (pe=l->current()) != 0 ) {
	if ( !event_type || pe->event->type() == event_type ) {
	    if ( pe->event ) {
		pe->event->posted = false;
		delete pe->event;
		pe->event = 0;
	    }
	    l->remove();
	} else {
	    l->next();
	}
    }
    if ( !event_type || !l->count() ) {
	receiver->postedEvents = 0;
	delete l;
    }
}


/*!
  Removes \a event from the queue of posted events, and emits a
  warning message if appropriate.

  \warning This function can be \e really slow. Avoid using it, if
  possible.

  \threadsafe
*/

void TQApplication::removePostedEvent( TQEvent *  event )
{
    if ( !event || !event->posted ) {
	return;
    }

#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker locker( GlobalPostedEvents()->mutex() );
#endif // TQT_THREAD_SUPPORT

    TQPostEventListIt it( *GlobalPostedEvents() );
    TQPostEvent * pe;
    while( (pe = it.current()) != 0 ) {
	++it;
	if ( pe->event == event ) {
#if defined(QT_DEBUG)
	    const char *n;
	    switch ( event->type() ) {
	    case TQEvent::Timer:
		n = "Timer";
		break;
	    case TQEvent::MouseButtonPress:
		n = "MouseButtonPress";
		break;
	    case TQEvent::MouseButtonRelease:
		n = "MouseButtonRelease";
		break;
	    case TQEvent::MouseButtonDblClick:
		n = "MouseButtonDblClick";
		break;
	    case TQEvent::MouseMove:
		n = "MouseMove";
		break;
#ifndef TQT_NO_WHEELEVENT
	    case TQEvent::Wheel:
		n = "Wheel";
		break;
#endif
	    case TQEvent::KeyPress:
		n = "KeyPress";
		break;
	    case TQEvent::KeyRelease:
		n = "KeyRelease";
		break;
	    case TQEvent::FocusIn:
		n = "FocusIn";
		break;
	    case TQEvent::FocusOut:
		n = "FocusOut";
		break;
	    case TQEvent::Enter:
		n = "Enter";
		break;
	    case TQEvent::Leave:
		n = "Leave";
		break;
	    case TQEvent::Paint:
		n = "Paint";
		break;
	    case TQEvent::Move:
		n = "Move";
		break;
	    case TQEvent::Resize:
		n = "Resize";
		break;
	    case TQEvent::Create:
		n = "Create";
		break;
	    case TQEvent::Destroy:
		n = "Destroy";
		break;
	    case TQEvent::Close:
		n = "Close";
		break;
	    case TQEvent::Quit:
		n = "Quit";
		break;
	    default:
		n = "<other>";
		break;
	    }
	    tqWarning("TQEvent: Warning: %s event deleted while posted to %s %s",
		     n,
		     pe->receiver ? pe->receiver->className() : "null",
		     pe->receiver ? pe->receiver->name() : "object" );
	    // note the beautiful uglehack if !pe->receiver :)
#endif
	    event->posted = false;
	    delete pe->event;
	    pe->event = 0;
	    return;
	}
    }
}

#ifdef TQT_THREAD_SUPPORT
static void tqThreadTerminationHandlerRecursive( TQObject* object, TQThread* originThread, TQThread* destinationThread ) {
	TQThread* objectThread = object->contextThreadObject();
	if (objectThread && (objectThread == originThread)) {
		TQThread::CleanupType cleanupType = objectThread->cleanupType();
		if (cleanupType == TQThread::CleanupMergeObjects) {
			object->moveToThread(destinationThread);
		}
		else if (cleanupType == TQThread::CleanupNone) {
			// Do nothing
#if defined(QT_DEBUG)
			tqDebug( "TQApplication::threadTerminationHandler: object %p still owned by thread %p at thread termination!",  object, objectThread);
#endif // QT_DEBUG
		}
		else {
			// Do nothing
#if defined(QT_DEBUG)
			tqDebug( "TQApplication::threadTerminationHandler: invalid thread termination cleanup type %d specified",  cleanupType);
#endif // QT_DEBUG
		}
	}
	TQObjectList children = object->childrenListObject();
	TQObject *childObject;
	for ( childObject = children.first(); childObject; childObject = children.next() ) {
		tqThreadTerminationHandlerRecursive(childObject, originThread, destinationThread);
	}
}

/*!\internal

  Migrates all objects from the specified thread in preparation
  for thread destruction.
 */
void TQApplication::threadTerminationHandler( TQThread *originThread ) {
	TQMutexLocker locker( tqt_mutex );
	TQThread* destinationThread = guiThread();
	const TQObjectList* objects = TQObject::objectTrees();
	for ( TQObjectListIt objectit( *objects ) ; *objectit; ++objectit ) {
		tqThreadTerminationHandlerRecursive((*objectit), originThread, destinationThread);
	}
}
#endif // TQT_THREAD_SUPPORT

/*!\internal

  Sets the active window in reaction to a system event. Call this
  from the platform specific event handlers.

  It sets the activeWindow() and focusWidget() attributes and sends
  proper WindowActivate/WindowDeactivate and FocusIn/FocusOut events
  to all appropriate widgets.

  \sa activeWindow()
 */
void TQApplication::setActiveWindow( TQWidget* act )
{
    TQWidget* window = act?act->topLevelWidget():0;

    if ( active_window == window )
	return;

    // first the activation/deactivation events
    if ( active_window ) {
	TQWidgetList deacts;
#ifndef TQT_NO_STYLE
	if ( style().styleHint(TQStyle::SH_Widget_ShareActivation, active_window ) ) {
	    TQWidgetList *list = topLevelWidgets();
	    if ( list ) {
		for ( TQWidget *w = list->first(); w; w = list->next() ) {
		    if ( w->isVisible() && w->isActiveWindow() )
			deacts.append(w);
		}
		delete list;
	    }
	} else
#endif
	    deacts.append(active_window);
	active_window = 0;
	TQEvent e( TQEvent::WindowDeactivate );
	for(TQWidget *w = deacts.first(); w; w = deacts.next())
	    TQApplication::sendSpontaneousEvent( w, &e );
    }

    active_window = window;
    if ( active_window ) {
	TQEvent e( TQEvent::WindowActivate );
	TQWidgetList acts;
#ifndef TQT_NO_STYLE
	if ( style().styleHint(TQStyle::SH_Widget_ShareActivation, active_window ) ) {
	    TQWidgetList *list = topLevelWidgets();
	    if ( list ) {
		for ( TQWidget *w = list->first(); w; w = list->next() ) {
		    if ( w->isVisible() && w->isActiveWindow() )
			acts.append(w);
		}
		delete list;
	    }
	} else
#endif
	    acts.append(active_window);
	for(TQWidget *w = acts.first(); w; w = acts.next())
	    TQApplication::sendSpontaneousEvent( w, &e );
    }

    // then focus events
    TQFocusEvent::setReason( TQFocusEvent::ActiveWindow );
    if ( !active_window && focus_widget ) {
	TQFocusEvent out( TQEvent::FocusOut );
	TQWidget *tmp = focus_widget;
	focus_widget = 0;
#ifdef TQ_WS_WIN
	TQInputContext::accept( tmp );
#elif defined(TQ_WS_X11)
	tmp->unfocusInputContext();
#endif
	TQApplication::sendSpontaneousEvent( tmp, &out );
    } else if ( active_window ) {
	TQWidget *w = active_window->focusWidget();
	if ( w && w->focusPolicy() != TQWidget::NoFocus )
	    w->setFocus();
	else
	    active_window->focusNextPrevChild( true );
    }
    TQFocusEvent::resetReason();
}


/*!\internal

  Creates the proper Enter/Leave event when widget \a enter is entered
  and widget \a leave is left.
 */
TQ_EXPORT void tqt_dispatchEnterLeave( TQWidget* enter, TQWidget* leave ) {
#if 0
    if ( leave ) {
	TQEvent e( TQEvent::Leave );
	TQApplication::sendEvent( leave, & e );
    }
    if ( enter ) {
	TQEvent e( TQEvent::Enter );
	TQApplication::sendEvent( enter, & e );
    }
    return;
#endif

    TQWidget* w ;
    if ( !enter && !leave )
	return;
    TQWidgetList leaveList;
    TQWidgetList enterList;

    bool sameWindow = leave && enter && leave->topLevelWidget() == enter->topLevelWidget();
    if ( leave && !sameWindow ) {
	w = leave;
	do {
	    leaveList.append( w );
	} while ( (w = w->parentWidget( true ) ) );
    }
    if ( enter && !sameWindow ) {
	w = enter;
	do {
	    enterList.prepend( w );
	} while ( (w = w->parentWidget(true) ) );
    }
    if ( sameWindow ) {
	int enterDepth = 0;
	int leaveDepth = 0;
	w = enter;
	while ( ( w = w->parentWidget( true ) ) )
	    enterDepth++;
	w = leave;
	while ( ( w = w->parentWidget( true ) ) )
	    leaveDepth++;
	TQWidget* wenter = enter;
	TQWidget* wleave = leave;
	while ( enterDepth > leaveDepth ) {
	    wenter = wenter->parentWidget();
	    enterDepth--;
	}
	while ( leaveDepth > enterDepth ) {
	    wleave = wleave->parentWidget();
	    leaveDepth--;
	}
	while ( !wenter->isTopLevel() && wenter != wleave ) {
	    wenter = wenter->parentWidget();
	    wleave = wleave->parentWidget();
	}

	w = leave;
	while ( w != wleave ) {
	    leaveList.append( w );
	    w = w->parentWidget();
	}
	w = enter;
	while ( w != wenter ) {
	    enterList.prepend( w );
	    w = w->parentWidget();
	}
    }

    TQEvent leaveEvent( TQEvent::Leave );
    for ( w = leaveList.first(); w; w = leaveList.next() ) {
	if ( !tqApp->activeModalWidget() || tqt_tryModalHelper( w, 0 ))
	    TQApplication::sendEvent( w, &leaveEvent );
    }
    TQEvent enterEvent( TQEvent::Enter );
    for ( w = enterList.first(); w; w = enterList.next() ) {
	if ( !tqApp->activeModalWidget() || tqt_tryModalHelper( w, 0 ))
	    TQApplication::sendEvent( w, &enterEvent );
    }
}


#ifdef TQ_WS_MACX
extern TQWidget *tqt_tryModalHelperMac( TQWidget * top ); //qapplication_mac.cpp
#endif


/*!\internal

  Called from qapplication_<platform>.cpp, returns true
  if the widget should accept the event.
 */
TQ_EXPORT bool tqt_tryModalHelper( TQWidget *widget, TQWidget **rettop ) {
    TQWidget *modal=0, *top=TQApplication::activeModalWidget();
    if ( rettop ) *rettop = top;

    if ( tqApp->activePopupWidget() )
	return true;

#ifdef TQ_WS_MACX
    top = tqt_tryModalHelperMac( top );
    if ( rettop ) *rettop = top;
#endif

    TQWidget* groupLeader = widget;
    widget = widget->topLevelWidget();

    if ( widget->testWFlags(TQt::WShowModal) )	// widget is modal
	modal = widget;
    if ( !top || modal == top )			// don't block event
	return true;

    TQWidget * p = widget->parentWidget(); // Check if the active modal widget is a parent of our widget
    while ( p ) {
	if ( p == top )
	    return true;
	p = p->parentWidget();
    }

    while ( groupLeader && !groupLeader->testWFlags( TQt::WGroupLeader ) )
	groupLeader = groupLeader->parentWidget();

    if ( groupLeader ) {
	// Does groupLeader have a child in tqt_modal_stack?
	bool unrelated = true;
	modal = tqt_modal_stack->first();
	while (modal && unrelated) {
	    TQWidget* p = modal->parentWidget();
	    while ( p && p != groupLeader && !p->testWFlags( TQt::WGroupLeader) ) {
		p = p->parentWidget();
	    }
	    modal = tqt_modal_stack->next();
	    if ( p == groupLeader ) unrelated = false;
	}

	if ( unrelated )
	    return true;		// don't block event
    }
    return false;
}


/*!
  Returns the desktop widget (also called the root window).

  The desktop widget is useful for obtaining the size of the screen.
  It may also be possible to draw on the desktop. We recommend against
  assuming that it's possible to draw on the desktop, since this does
  not work on all operating systems.

  \code
    TQDesktopWidget *d = TQApplication::desktop();
    int w = d->width();	    // returns desktop width
    int h = d->height();    // returns desktop height
  \endcode
*/

TQDesktopWidget *TQApplication::desktop()
{
    if ( !tqt_desktopWidget || // not created yet
	 !tqt_desktopWidget->isDesktop() ) { // reparented away
	tqt_desktopWidget = new TQDesktopWidget();
	TQ_CHECK_PTR( tqt_desktopWidget );
    }
    return tqt_desktopWidget;
}

#ifndef TQT_NO_CLIPBOARD
/*!
  Returns a pointer to the application global clipboard.
*/
TQClipboard *TQApplication::clipboard()
{
    if ( tqt_clipboard == 0 ) {
	tqt_clipboard = new TQClipboard;
	TQ_CHECK_PTR( tqt_clipboard );
    }
    return tqt_clipboard;
}
#endif // TQT_NO_CLIPBOARD

/*!
  By default, TQt will try to use the current standard colors, fonts
  etc., from the underlying window system's desktop settings,
  and use them for all relevant widgets. This behavior can be switched off
  by calling this function with \a on set to false.

  This static function must be called before creating the TQApplication
  object, like this:

  \code
  int main( int argc, char** argv ) {
    TQApplication::setDesktopSettingsAware( false ); // I know better than the user
    TQApplication myApp( argc, argv ); // Use default fonts & colors
    ...
  }
  \endcode

  \sa desktopSettingsAware()
*/

void TQApplication::setDesktopSettingsAware( bool on )
{
    obey_desktop_settings = on;
}

/*!
  Returns the value set by setDesktopSettingsAware(); by default true.

  \sa setDesktopSettingsAware()
*/

bool TQApplication::desktopSettingsAware()
{
    return obey_desktop_settings;
}

/*! \fn void TQApplication::lock()

  Lock the TQt Library Mutex. If another thread has already locked the
  mutex, the calling thread will block until the other thread has
  unlocked the mutex.

  \sa unlock() locked() \link threads.html Thread Support in TQt\endlink
*/


/*! \fn void TQApplication::unlock(bool wakeUpGui)

  Unlock the TQt Library Mutex. If \a wakeUpGui is true (the default),
  then the GUI thread will be woken with TQApplication::wakeUpGuiThread().

  \sa lock(), locked() \link threads.html Thread Support in TQt\endlink
*/


/*! \fn bool TQApplication::locked()

  Returns true if the TQt Library Mutex is locked by a different thread;
  otherwise returns false.

  \warning Due to different implementations of recursive mutexes on
  the supported platforms, calling this function from the same thread
  that previously locked the mutex will give undefined results.

  \sa lock() unlock() \link threads.html Thread Support in TQt\endlink
*/

/*! \fn bool TQApplication::tryLock()

  Attempts to lock the TQt Library Mutex, and returns immediately. If
  the lock was obtained, this function returns true. If another thread
  has locked the mutex, this function returns false, instead of
  waiting for the lock to become available.

  The mutex must be unlocked with unlock() before another thread can
  successfully lock it.

  \sa lock(), unlock() \link threads.html Thread Support in TQt\endlink
*/

#if defined(TQT_THREAD_SUPPORT)
void TQApplication::lock()
{
    tqt_mutex->lock();
}

void TQApplication::unlock(bool wakeUpGui)
{
    tqt_mutex->unlock();

    if (wakeUpGui)
	wakeUpGuiThread();
}

bool TQApplication::locked()
{
    return tqt_mutex->locked();
}

bool TQApplication::tryLock()
{
    return tqt_mutex->tryLock();
}
#endif


/*!
  \fn bool TQApplication::isSessionRestored() const

  Returns true if the application has been restored from an earlier
  \link session.html session\endlink; otherwise returns false.

  \sa sessionId(), commitData(), saveState()
*/


/*!
  \fn TQString TQApplication::sessionId() const

  Returns the current \link session.html session's\endlink identifier.

  If the application has been restored from an earlier session, this
  identifier is the same as it was in that previous session.

  The session identifier is guaranteed to be unique both for different
  applications and for different instances of the same application.

  \sa isSessionRestored(), sessionKey(), commitData(), saveState()
 */

/*!
  \fn TQString TQApplication::sessionKey() const

  Returns the session key in the current \link session.html
  session\endlink.

  If the application has been restored from an earlier session, this
  key is the same as it was when the previous session ended.

  The session key changes with every call of commitData() or
  saveState().

  \sa isSessionRestored(), sessionId(), commitData(), saveState()
 */


/*!
  \fn void TQApplication::commitData( TQSessionManager& sm )

  This function deals with \link session.html session
  management\endlink. It is invoked when the TQSessionManager wants the
  application to commit all its data.

  Usually this means saving all open files, after getting
  permission from the user. Furthermore you may want to provide a means
  by which the user can cancel the shutdown.

  Note that you should not exit the application within this function.
  Instead, the session manager may or may not do this afterwards,
  depending on the context.

  \warning Within this function, no user interaction is possible, \e
  unless you ask the session manager \a sm for explicit permission.
  See TQSessionManager::allowsInteraction() and
  TQSessionManager::allowsErrorInteraction() for details and example
  usage.

  The default implementation requests interaction and sends a close
  event to all visible top level widgets. If any event was
  rejected, the shutdown is canceled.

  \sa isSessionRestored(), sessionId(), saveState(), \link session.html the Session Management overview\endlink
*/
#ifndef TQT_NO_SESSIONMANAGER
void TQApplication::commitData( TQSessionManager& sm  )
{

    if ( sm.allowsInteraction() ) {
	TQWidgetList done;
	TQWidgetList *list = TQApplication::topLevelWidgets();
	bool cancelled = false;
	TQWidget* w = list->first();
	while ( !cancelled && w ) {
	    if ( !w->isHidden() ) {
		TQCloseEvent e;
		sendEvent( w, &e );
		cancelled = !e.isAccepted();
		if ( !cancelled )
		    done.append( w );
		delete list; // one never knows...
		list = TQApplication::topLevelWidgets();
		w = list->first();
	    } else {
		w = list->next();
	    }
	    while ( w && done.containsRef( w ) )
		w = list->next();
	}
	delete list;
	if ( cancelled )
	    sm.cancel();
    }
}


/*!
  \fn void TQApplication::saveState( TQSessionManager& sm )

  This function deals with \link session.html session
  management\endlink. It is invoked when the
  \link TQSessionManager session manager \endlink wants the application
  to preserve its state for a future session.

  For example, a text editor would create a temporary file that
  includes the current contents of its edit buffers, the location of
  the cursor and other aspects of the current editing session.

  Note that you should never exit the application within this
  function. Instead, the session manager may or may not do this
  afterwards, depending on the context. Futhermore, most session
  managers will very likely request a saved state immediately after
  the application has been started. This permits the session manager
  to learn about the application's restart policy.

  \warning Within this function, no user interaction is possible, \e
  unless you ask the session manager \a sm for explicit permission.
  See TQSessionManager::allowsInteraction() and
  TQSessionManager::allowsErrorInteraction() for details.

  \sa isSessionRestored(), sessionId(), commitData(), \link session.html the Session Management overview\endlink
*/

void TQApplication::saveState( TQSessionManager& /* sm */ )
{
}
#endif //TQT_NO_SESSIONMANAGER
/*!
  Sets the time after which a drag should start to \a ms ms.

  \sa startDragTime()
*/

void TQApplication::setStartDragTime( int ms )
{
    drag_time = ms;
}

/*!
  If you support drag and drop in you application and a drag should
  start after a mouse click and after a certain time elapsed, you
  should use the value which this method returns as the delay (in ms).

  TQt also uses this delay internally, e.g. in TQTextEdit and TQLineEdit,
  for starting a drag.

  The default value is 500 ms.

  \sa setStartDragTime(), startDragDistance()
*/

int TQApplication::startDragTime()
{
    return drag_time;
}

/*!
  Sets the distance after which a drag should start to \a l pixels.

  \sa startDragDistance()
*/

void TQApplication::setStartDragDistance( int l )
{
    drag_distance = l;
}

/*!
  If you support drag and drop in you application and a drag should
  start after a mouse click and after moving the mouse a certain
  distance, you should use the value which this method returns as the
  distance.

  For example, if the mouse position of the click is stored in \c
  startPos and the current position (e.g. in the mouse move event) is
  \c currPos, you can find out if a drag should be started with code
  like this:
  \code
  if ( ( startPos - currPos ).manhattanLength() >
       TQApplication::startDragDistance() )
    startTheDrag();
  \endcode

  TQt uses this value internally, e.g. in TQFileDialog.

  The default value is 4 pixels.

  \sa setStartDragDistance(), startDragTime(), TQPoint::manhattanLength()
*/

int TQApplication::startDragDistance()
{
    return drag_distance;
}

/*!
  If \a b is true, all dialogs and widgets will be laid out in a
  mirrored fashion, as required by right to left languages such as
  Arabic and Hebrew. If \a b is false, dialogs and widgets are laid
  out left to right.

  Changing this flag in runtime does not cause a relayout of already
  instantiated widgets.

  \sa reverseLayout()
*/
void TQApplication::setReverseLayout( bool b )
{
    if ( reverse_layout == b )
	return;

    reverse_layout = b;

    TQWidgetList *list = topLevelWidgets();
    TQWidgetListIt it( *list );
    TQWidget *w;
    while ( ( w=it.current() ) != 0 ) {
	++it;
	postEvent( w, new TQEvent( TQEvent::LayoutDirectionChange ) );
    }
    delete list;
}

/*!
    Returns true if all dialogs and widgets will be laid out in a
    mirrored (right to left) fashion. Returns false if dialogs and
    widgets will be laid out left to right.

  \sa setReverseLayout()
*/
bool TQApplication::reverseLayout()
{
    return reverse_layout;
}


/*!
  \class TQSessionManager ntqsessionmanager.h
  \brief The TQSessionManager class provides access to the session manager.

  \ingroup application
  \ingroup environment

  The session manager is responsible for session management, most
  importantly for interruption and resumption. A "session" is a kind
  of record of the state of the system, e.g. which applications were
  run at start up and which applications are currently running. The
  session manager is used to save the session, e.g. when the machine
  is shut down; and to restore a session, e.g. when the machine is
  started up. Use TQSettings to save and restore an individual
  application's settings, e.g. window positions, recently used files,
  etc.

  TQSessionManager provides an interface between the application and
  the session manager so that the program can work well with the
  session manager. In TQt, session management requests for action
  are handled by the two virtual functions TQApplication::commitData()
  and TQApplication::saveState(). Both provide a reference to
  a session manager object as argument, to allow the application
  to communicate with the session manager.

  During a session management action (i.e. within commitData() and
  saveState()), no user interaction is possible \e unless the
  application got explicit permission from the session manager. You
  ask for permission by calling allowsInteraction() or, if it's really
  urgent, allowsErrorInteraction(). TQt does not enforce this, but the
  session manager may.

  You can try to abort the shutdown process by calling cancel(). The
  default commitData() function does this if some top-level window
  rejected its closeEvent().

  For sophisticated session managers provided on Unix/X11, TQSessionManager
  offers further possibilites to fine-tune an application's session
  management behavior: setRestartCommand(), setDiscardCommand(),
  setRestartHint(), setProperty(), requestPhase2(). See the respective
  function descriptions for further details.
*/

/*! \enum TQSessionManager::RestartHint

  This enum type defines the circumstances under which this
  application wants to be restarted by the session manager. The
  current values are

  \value RestartIfRunning  if the application is still running when
  the session is shut down, it wants to be restarted at the start of
  the next session.

  \value RestartAnyway  the application wants to be started at the
  start of the next session, no matter what. (This is useful for
  utilities that run just after startup and then quit.)

  \value RestartImmediately  the application wants to be started
  immediately whenever it is not running.

  \value RestartNever  the application does not want to be restarted
  automatically.

  The default hint is \c RestartIfRunning.
*/


/*!
  \fn TQString TQSessionManager::sessionId() const

  Returns the identifier of the current session.

  If the application has been restored from an earlier session, this
  identifier is the same as it was in that earlier session.

  \sa sessionKey(), TQApplication::sessionId()
 */

/*!
  \fn TQString TQSessionManager::sessionKey() const

  Returns the session key in the current session.

  If the application has been restored from an earlier session, this
  key is the same as it was when the previous session ended.

  The session key changes with every call of commitData() or
  saveState().

  \sa sessionId(), TQApplication::sessionKey()
 */

// ### Note: This function is undocumented, since it is #ifdef'd.

/*!
  \fn void* TQSessionManager::handle() const

  X11 only: returns a handle to the current \c SmcConnection.
*/


/*!
  \fn bool TQSessionManager::allowsInteraction()

  Asks the session manager for permission to interact with the
  user. Returns true if interaction is permitted; otherwise
  returns false.

  The rationale behind this mechanism is to make it possible to
  synchronize user interaction during a shutdown. Advanced session
  managers may ask all applications simultaneously to commit their
  data, resulting in a much faster shutdown.

  When the interaction is completed we strongly recommend releasing the
  user interaction semaphore with a call to release(). This way, other
  applications may get the chance to interact with the user while your
  application is still busy saving data. (The semaphore is implicitly
  released when the application exits.)

  If the user decides to cancel the shutdown process during the
  interaction phase, you must tell the session manager that this has
  happened by calling cancel().

  Here's an example of how an application's TQApplication::commitData()
  might be implemented:

\code
void MyApplication::commitData( TQSessionManager& sm ) {
    if ( sm.allowsInteraction() ) {
	switch ( TQMessageBox::warning(
		    yourMainWindow,
		    tr("Application Name"),
		    tr("Save changes to document Foo?"),
		    tr("&Yes"),
		    tr("&No"),
		    tr("Cancel"),
		    0, 2) ) {
	case 0: // yes
	    sm.release();
	    // save document here; if saving fails, call sm.cancel()
	    break;
	case 1: // continue without saving
	    break;
	default: // cancel
	    sm.cancel();
	    break;
	}
    } else {
	// we did not get permission to interact, then
	// do something reasonable instead.
    }
}
\endcode

  If an error occurred within the application while saving its data,
  you may want to try allowsErrorInteraction() instead.

  \sa TQApplication::commitData(), release(), cancel()
*/


/*!
  \fn bool TQSessionManager::allowsErrorInteraction()

  This is similar to allowsInteraction(), but also tells the session
  manager that an error occurred. Session managers may give error
  interaction request higher priority, which means that it is more likely
  that an error interaction is permitted. However, you are still not
  guaranteed that the session manager will allow interaction.

  \sa allowsInteraction(), release(), cancel()
*/

/*!
  \fn void TQSessionManager::release()

  Releases the session manager's interaction semaphore after an
  interaction phase.

  \sa allowsInteraction(), allowsErrorInteraction()
*/

/*!
  \fn void TQSessionManager::cancel()

  Tells the session manager to cancel the shutdown process.  Applications
  should not call this function without first asking the user.

  \sa allowsInteraction(), allowsErrorInteraction()

*/

/*!
  \fn void TQSessionManager::setRestartHint( RestartHint hint )

  Sets the application's restart hint to \a hint. On application
  startup the hint is set to \c RestartIfRunning.

  Note that these flags are only hints, a session manager may or may
  not respect them.

  We recommend setting the restart hint in TQApplication::saveState()
  because most session managers perform a checkpoint shortly after an
  application's startup.

  \sa restartHint()
*/

/*!
  \fn TQSessionManager::RestartHint TQSessionManager::restartHint() const

  Returns the application's current restart hint. The default is
  \c RestartIfRunning.

  \sa setRestartHint()
*/

/*!
  \fn void TQSessionManager::setRestartCommand( const TQStringList& command )

  If the session manager is capable of restoring sessions it will
  execute \a command in order to restore the application. The command
  defaults to

  \code
	appname -session id
  \endcode

  The \c -session option is mandatory; otherwise TQApplication cannot
  tell whether it has been restored or what the current session
  identifier is. See TQApplication::isSessionRestored() and
  TQApplication::sessionId() for details.

  If your application is very simple, it may be possible to store the
  entire application state in additional command line options. This
  is usually a very bad idea because command lines are often limited
  to a few hundred bytes. Instead, use TQSettings, or temporary files
  or a database for this purpose. By marking the data with the unique
  sessionId(), you will be able to restore the application in a future
  session.

  \sa restartCommand(), setDiscardCommand(), setRestartHint()
*/

/*!
  \fn TQStringList TQSessionManager::restartCommand() const

  Returns the currently set restart command.

  Note that if you want to iterate over the list, you should
  iterate over a copy, e.g.
    \code
    TQStringList list = mySession.restartCommand();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

  \sa setRestartCommand(), restartHint()
*/

/*!
  \fn void TQSessionManager::setDiscardCommand( const TQStringList& )

  \sa discardCommand(), setRestartCommand()
*/


/*!
  \fn TQStringList TQSessionManager::discardCommand() const

  Returns the currently set discard command.

  Note that if you want to iterate over the list, you should
  iterate over a copy, e.g.
    \code
    TQStringList list = mySession.discardCommand();
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

  \sa setDiscardCommand(), restartCommand(), setRestartCommand()
*/

/*!
  \overload void TQSessionManager::setManagerProperty( const TQString& name,
						      const TQString& value )

  Low-level write access to the application's identification and state
  records are kept in the session manager.

    The property called \a name has its value set to the string \a value.
*/

/*!
  \fn void TQSessionManager::setManagerProperty( const TQString& name,
						const TQStringList& value )

  Low-level write access to the application's identification and state
  record are kept in the session manager.

    The property called \a name has its value set to the string list \a value.
*/

/*!
  \fn bool TQSessionManager::isPhase2() const

  Returns true if the session manager is currently performing a second
  session management phase; otherwise returns false.

  \sa requestPhase2()
*/

/*!
  \fn void TQSessionManager::requestPhase2()

  Requests a second session management phase for the application. The
  application may then return immediately from the
  TQApplication::commitData() or TQApplication::saveState() function,
  and they will be called again once most or all other applications have
  finished their session management.

  The two phases are useful for applications such as the X11 window manager
  that need to store information about another application's windows
  and therefore have to wait until these applications have completed their
  respective session management tasks.

  Note that if another application has requested a second phase it
  may get called before, simultaneously with, or after your
  application's second phase.

  \sa isPhase2()
*/

/*!
  \fn int TQApplication::horizontalAlignment( int align )

  Strips out vertical alignment flags and transforms an
  alignment \a align of AlignAuto into AlignLeft or
  AlignRight according to the language used. The other horizontal
  alignment flags are left untouched.
*/


/*****************************************************************************
  Stubbed session management support
 *****************************************************************************/
#ifndef TQT_NO_SESSIONMANAGER
#if defined( TQT_NO_SM_SUPPORT ) || defined( TQ_WS_WIN ) || defined( TQ_WS_MAC ) || defined( TQ_WS_QWS )

class TQSessionManagerData
{
public:
    TQStringList restartCommand;
    TQStringList discardCommand;
    TQString sessionId;
    TQString sessionKey;
    TQSessionManager::RestartHint restartHint;
};

TQSessionManager* tqt_session_manager_self = 0;
TQSessionManager::TQSessionManager( TQApplication * app, TQString &id, TQString &key )
    : TQObject( app, "tqt_sessionmanager" )
{
    tqt_session_manager_self = this;
    d = new TQSessionManagerData;
#if defined(TQ_WS_WIN) && !defined(Q_OS_TEMP)
    wchar_t guidstr[40];
    GUID guid;
    CoCreateGuid( &guid );
    StringFromGUID2(guid, guidstr, 40);
    id = TQString::fromUcs2((ushort*)guidstr);
    CoCreateGuid( &guid );
    StringFromGUID2(guid, guidstr, 40);
    key = TQString::fromUcs2((ushort*)guidstr);
#endif
    d->sessionId = id;
    d->sessionKey = key;
    d->restartHint = RestartIfRunning;
}

TQSessionManager::~TQSessionManager()
{
    delete d;
    tqt_session_manager_self = 0;
}

TQString TQSessionManager::sessionId() const
{
    return d->sessionId;
}

TQString TQSessionManager::sessionKey() const
{
    return d->sessionKey;
}


#if defined(TQ_WS_X11) || defined(TQ_WS_MAC)
void* TQSessionManager::handle() const
{
    return 0;
}
#endif

#if !defined(TQ_WS_WIN)
bool TQSessionManager::allowsInteraction()
{
    return true;
}

bool TQSessionManager::allowsErrorInteraction()
{
    return true;
}
void TQSessionManager::release()
{
}

void TQSessionManager::cancel()
{
}
#endif


void TQSessionManager::setRestartHint( TQSessionManager::RestartHint hint)
{
    d->restartHint = hint;
}

TQSessionManager::RestartHint TQSessionManager::restartHint() const
{
    return d->restartHint;
}

void TQSessionManager::setRestartCommand( const TQStringList& command)
{
    d->restartCommand = command;
}

TQStringList TQSessionManager::restartCommand() const
{
    return d->restartCommand;
}

void TQSessionManager::setDiscardCommand( const TQStringList& command)
{
    d->discardCommand = command;
}

TQStringList TQSessionManager::discardCommand() const
{
    return d->discardCommand;
}

void TQSessionManager::setManagerProperty( const TQString&, const TQString&)
{
}

void TQSessionManager::setManagerProperty( const TQString&, const TQStringList& )
{
}

bool TQSessionManager::isPhase2() const
{
    return false;
}

void TQSessionManager::requestPhase2()
{
}

#endif // TQT_NO_SM_SUPPORT
#endif //TQT_NO_SESSIONMANAGER
