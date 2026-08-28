/****************************************************************************
**
** Implementation of X11 startup routines and event handling
**
** Created : 931029
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

// ### 4.0: examine TQ_EXPORT's below. The respective symbols had all
// been in use (e.g. in the KDE wm ) before the introduction of a version
// map. One might want to turn some of them into propert public API and
// provide a proper alternative for others. See also the exports in
// qapplication_win.cpp which suggest a unification.

#include "qplatformdefs.h"

// POSIX Large File Support redefines open -> open64
#if defined(open)
# undef open
#endif

// Solaris redefines connect -> __xnet_connect with _XOPEN_SOURCE_EXTENDED.
#if defined(connect)
# undef connect
#endif

// POSIX Large File Support redefines truncate -> truncate64
#if defined(truncate)
# undef truncate
#endif

#include "ntqapplication.h"
#include "qapplication_p.h"
#include "qcolor_p.h"
#include "ntqcursor.h"
#include "ntqwidget.h"
#include "qwidget_p.h"
#include "ntqobjectlist.h"
#include "ntqwidgetlist.h"
#include "ntqwidgetintdict.h"
#include "ntqbitarray.h"
#include "ntqpainter.h"
#include "ntqpixmapcache.h"
#include "ntqdatetime.h"
#include "ntqtextcodec.h"
#include "ntqdatastream.h"
#include "ntqbuffer.h"
#include "ntqsocketnotifier.h"
#include "ntqsessionmanager.h"
#include "ntqvaluelist.h"
#include "ntqdict.h"
#include "ntqguardedptr.h"
#include "ntqclipboard.h"
#include "ntqwhatsthis.h" // ######## dependency
#include "ntqsettings.h"
#include "ntqstylefactory.h"
#include "ntqfileinfo.h"

// Input method stuff - UNFINISHED
#ifndef TQT_NO_IM
#include "ntqinputcontext.h"
#endif // TQT_NO_IM
#include "qinternal_p.h" // shared double buffer cleanup

#if defined(TQT_THREAD_SUPPORT)
# include "ntqthread.h"
#endif

#if defined(QT_DEBUG) && defined(Q_OS_LINUX)
# include "ntqfile.h"
#endif

#include "qt_x11_p.h"

#if !defined(TQT_NO_XFTFREETYPE)
// XFree86 4.0.3 implementation is missing XftInitFtLibrary forward
extern "C" Bool XftInitFtLibrary(void);
#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <cstdlib>

//#define X_NOT_BROKEN
#ifdef X_NOT_BROKEN
// Some X libraries are built with setlocale #defined to _Xsetlocale,
// even though library users are then built WITHOUT such a definition.
// This creates a problem - TQt might setlocale() one value, but then
// X looks and doesn't see the value TQt set. The solution here is to
// implement _Xsetlocale just in case X calls it - redirecting it to
// the real libC version.
//
# ifndef setlocale
extern "C" char *_Xsetlocale(int category, const char *locale);
char *_Xsetlocale(int category, const char *locale)
{
    //tqDebug("_Xsetlocale(%d,%s),category,locale");
    return setlocale(category,locale);
}
# endif // setlocale
#endif // X_NOT_BROKEN


// resolve the conflict between X11's FocusIn and TQEvent::FocusIn
const int XFocusOut = FocusOut;
const int XFocusIn = FocusIn;
#undef FocusOut
#undef FocusIn

const int XKeyPress = KeyPress;
const int XKeyRelease = KeyRelease;
#undef KeyPress
#undef KeyRelease


// Fix old X libraries
#ifndef XK_KP_Home
#define XK_KP_Home              0xFF95
#endif
#ifndef XK_KP_Left
#define XK_KP_Left              0xFF96
#endif
#ifndef XK_KP_Up
#define XK_KP_Up                0xFF97
#endif
#ifndef XK_KP_Right
#define XK_KP_Right             0xFF98
#endif
#ifndef XK_KP_Down
#define XK_KP_Down              0xFF99
#endif
#ifndef XK_KP_Prior
#define XK_KP_Prior             0xFF9A
#endif
#ifndef XK_KP_Next
#define XK_KP_Next              0xFF9B
#endif
#ifndef XK_KP_End
#define XK_KP_End               0xFF9C
#endif
#ifndef XK_KP_Insert
#define XK_KP_Insert            0xFF9E
#endif
#ifndef XK_KP_Delete
#define XK_KP_Delete            0xFF9F
#endif


/*****************************************************************************
  Internal variables and functions
 *****************************************************************************/
static const char *appName;			// application name
static const char *appClass;			// application class
static const char *appFont	= 0;		// application font
static const char *appBGCol	= 0;		// application bg color
static const char *appFGCol	= 0;		// application fg color
static const char *appBTNCol	= 0;		// application btn color
static const char *mwGeometry	= 0;		// main widget geometry
static const char *mwTitle	= 0;		// main widget title
//Ming-Che 10/10
TQ_EXPORT char    *tqt_ximServer	= 0;		// XIM Server will connect to
static bool	mwIconic	= false;	// main widget iconified
//Ming-Che 10/10
static Display *appDpy		= 0;		// X11 application display
static char    *appDpyName	= 0;		// X11 display name
static bool	appForeignDpy	= false;        // we didn't create display
static bool	appSync		= false;	// X11 synchronization
#if defined(QT_DEBUG)
static bool	appNoGrab	= false;	// X11 grabbing enabled
static bool	appDoGrab	= false;	// X11 grabbing override (gdb)
#endif
static int	appScreen;			// X11 screen number
static int	appScreenCount;			// X11 screen count
static bool	app_save_rootinfo = false;	// save root info
static bool	app_do_modal	= false;	// modal mode
static Window	curWin = 0;			// current window

static GC*	app_gc_ro	= 0;		// read-only GC
static GC*	app_gc_tmp	= 0;		// temporary GC
static GC*	app_gc_ro_m	= 0;		// read-only GC (monochrome)
static GC*	app_gc_tmp_m	= 0;		// temporary GC (monochrome)
// symbols needed by extern TQXEmbed class
TQ_EXPORT Atom	tqt_wm_protocols	= 0;	// window manager protocols
TQ_EXPORT Atom	tqt_wm_delete_window	= 0;	// delete window protocol
TQ_EXPORT Atom	tqt_wm_take_focus	= 0;	// take focus window protocol

Atom		tqt_scrolldone	= 0;	// scroll synchronization
Atom		tqt_net_wm_context_help	= 0;	// context help
Atom		tqt_net_wm_ping		= 0;	// _NET_WM_PING protocol

static Atom	tqt_xsetroot_id		= 0;
Atom            tqt_xa_clipboard         = 0;
Atom		tqt_selection_property	= 0;
Atom            tqt_clipboard_sentinel  = 0;
Atom		tqt_selection_sentinel	= 0;
TQ_EXPORT Atom	tqt_wm_state		= 0;
Atom		tqt_wm_change_state	= 0;
static Atom     tqt_settings_timestamp	= 0;    // TQt >=3 settings timestamp
static Atom	tqt_input_encoding	= 0;	// TQt desktop properties
static Atom	tqt_resource_manager	= 0;	// X11 Resource manager
Atom		tqt_sizegrip		= 0;	// sizegrip
Atom		tqt_wm_client_leader	= 0;
TQ_EXPORT Atom	tqt_window_role		= 0;
TQ_EXPORT Atom	tqt_sm_client_id		= 0;
Atom		tqt_xa_motif_wm_hints	= 0;
Atom		tqt_cde_running		= 0;
Atom		tqt_twin_running	= 0;
Atom		tqt_kwm_running	= 0;
Atom		tqt_gbackground_properties	= 0;
Atom		tqt_x_incr		= 0;
Atom		tqt_utf8_string = 0;

// detect broken window managers
Atom            tqt_sgi_desks_manager    = 0;
bool		tqt_broken_wm		= false;
static void tqt_detect_broken_window_manager();

// NET WM support
Atom		tqt_net_supported	= 0;
Atom		tqt_net_wm_name		= 0;
Atom		tqt_net_wm_icon_name	= 0;
Atom		tqt_net_virtual_roots	= 0;
Atom		tqt_net_workarea	= 0;
Atom		tqt_net_wm_state	= 0;
Atom		tqt_net_wm_state_modal	= 0;
Atom		tqt_net_wm_state_max_v	= 0;
Atom		tqt_net_wm_state_max_h	= 0;
Atom		tqt_net_wm_state_fullscreen	= 0;
Atom		tqt_net_wm_state_above		= 0;
Atom		tqt_net_wm_action		= 0;
Atom		tqt_net_wm_action_move		= 0;
Atom		tqt_net_wm_action_resize	= 0;
Atom		tqt_net_wm_action_minimize	= 0;
Atom		tqt_net_wm_action_shade		= 0;
Atom		tqt_net_wm_action_stick		= 0;
Atom		tqt_net_wm_action_max_h		= 0;
Atom		tqt_net_wm_action_max_v		= 0;
Atom		tqt_net_wm_action_fullscreen	= 0;
Atom		tqt_net_wm_action_change_desktop = 0;
Atom		tqt_net_wm_action_close		= 0;
Atom		tqt_net_wm_action_above		= 0;
Atom		tqt_net_wm_action_below		= 0;
Atom            tqt_net_wm_window_type		= 0;
Atom            tqt_net_wm_window_type_normal	= 0;
Atom            tqt_net_wm_window_type_dialog	= 0;
Atom            tqt_net_wm_window_type_toolbar	= 0;
Atom		tqt_net_wm_window_type_menu	= 0;
Atom		tqt_net_wm_window_type_utility	= 0;
Atom            tqt_net_wm_window_type_splash   = 0;
Atom            tqt_net_wm_window_type_override	= 0;	// KDE extension
Atom            tqt_net_wm_window_type_dropdown_menu = 0;
Atom            tqt_net_wm_window_type_popup_menu    = 0;
Atom            tqt_net_wm_window_type_tooltip  = 0;
Atom            tqt_net_wm_window_type_combo    = 0;
Atom            tqt_net_wm_window_type_dnd      = 0;
Atom		tqt_net_wm_frame_strut		= 0;	// KDE extension
Atom		tqt_net_wm_state_stays_on_top	= 0;	// KDE extension
Atom		tqt_net_wm_pid		= 0;
Atom		tqt_net_wm_user_time	= 0;
Atom            tqt_net_wm_full_placement = 0; // KDE extension
// Enlightenment support
Atom		tqt_enlightenment_desktop	= 0;

// window managers list of supported "stuff"
Atom		*tqt_net_supported_list	= 0;
// list of virtual root windows
Window		*tqt_net_virtual_root_list	= 0;


// X11 SYNC support
#ifndef TQT_NO_XSYNC
Atom		tqt_net_wm_sync_request_counter	= 0;
Atom		tqt_net_wm_sync_request     	= 0;
#endif

// client leader window
Window tqt_x11_wm_client_leader = 0;

// function to update the workarea of the screen - in qdesktopwidget_x11.cpp
extern void tqt_desktopwidget_update_workarea();

// current focus model
static const int FocusModel_Unknown = -1;
static const int FocusModel_Other = 0;
static const int FocusModel_PointerRoot = 1;
static int tqt_focus_model = -1;

#ifndef TQT_NO_XRANDR
// true if TQt is compiled w/ XRandR support and XRandR exists on the connected
// Display
bool	tqt_use_xrandr	= false;
static int xrandr_eventbase;
#endif

// true if TQt is compiled w/ XRender support and XRender exists on the connected
// Display
TQ_EXPORT bool tqt_use_xrender = false;

#ifndef TQT_NO_XSYNC
// True if SYNC extension exists on the connected display
bool tqt_use_xsync = false;
static int xsync_eventbase;
static int xsync_errorbase;
#endif

// modifier masks for alt/meta - detected when the application starts
static long tqt_alt_mask = 0;
static long tqt_meta_mask = 0;
// modifier mask to remove mode switch from modifiers that have alt/meta set
// this problem manifests itself on some systems, and without it
// modifiers do not work at all...
static long tqt_mode_switch_remove_mask = 0;

// flags for extensions for special Languages, currently only for RTL languages
static bool 	tqt_use_rtl_extensions = false;
TQ_EXPORT bool tqt_hebrew_keyboard_hack = false;

static Window	mouseActWindow	     = 0;	// window where mouse is
static int	mouseButtonPressed   = 0;	// last mouse button pressed
static int	mouseButtonState     = 0;	// mouse button state
static Time	mouseButtonPressTime = 0;	// when was a button pressed
static short	mouseXPos, mouseYPos;		// mouse pres position in act window
static short	mouseGlobalXPos, mouseGlobalYPos; // global mouse press position

extern TQWidgetList *tqt_modal_stack;		// stack of modal widgets
static bool	    ignoreNextMouseReleaseEvent = false; // ignore the next mouse release
							 // event if return from a modal
							 // widget

static TQWidget     *popupButtonFocus = 0;
static TQWidget     *popupOfPopupButtonFocus = 0;
static bool	    popupCloseDownMode = false;
static bool	    popupGrabOk;

static bool sm_blockUserInput = false;		// session management

int tqt_xfocusout_grab_counter = 0;

#if defined (QT_TABLET_SUPPORT)
// since XInput event classes aren't created until we actually open an XInput
// device, here is a static list that we will use later on...
const int INVALID_EVENT = -1;
const int TOTAL_XINPUT_EVENTS = 7;

XDevice *devStylus = NULL;
XDevice *devEraser = NULL;
XEventClass event_list_stylus[TOTAL_XINPUT_EVENTS];
XEventClass event_list_eraser[TOTAL_XINPUT_EVENTS];

int tqt_curr_events_stylus = 0;
int tqt_curr_events_eraser = 0;

// well, luckily we only need to do this once.
static int xinput_motion = INVALID_EVENT;
static int xinput_key_press = INVALID_EVENT;
static int xinput_key_release = INVALID_EVENT;
static int xinput_button_press = INVALID_EVENT;
static int xinput_button_release = INVALID_EVENT;

// making this assumption on XFree86, since we can only use 1 device,
// the pressure for the eraser and the stylus should be the same, if they aren't
// well, they certainly have a strange pen then...
static int max_pressure;
extern bool chokeMouse;
#endif

// last timestamp read from TQSettings
static uint appliedstamp = 0;


typedef int (*QX11EventFilter) (XEvent*);
QX11EventFilter tqt_set_x11_event_filter(QX11EventFilter filter);

static QX11EventFilter tqt_x11_event_filter = 0;
TQ_EXPORT QX11EventFilter tqt_set_x11_event_filter(QX11EventFilter filter)
{
    QX11EventFilter old_filter = tqt_x11_event_filter;
    tqt_x11_event_filter = filter;
    return old_filter;
}
static bool tqt_x11EventFilter( XEvent* ev )
{
    if ( tqt_x11_event_filter  && tqt_x11_event_filter( ev )  )
	return true;
    return tqApp->x11EventFilter( ev );
}





#if !defined(TQT_NO_XIM)
//XIM		tqt_xim			= 0;
TQ_EXPORT XIMStyle	tqt_xim_style		= 0;
TQ_EXPORT XIMStyle	tqt_xim_preferred_style	= 0;
static XIMStyle xim_default_style	= XIMPreeditCallbacks | XIMStatusNothing;
#endif

TQ_EXPORT int tqt_ximComposingKeycode=0;
TQ_EXPORT TQTextCodec * tqt_input_mapper = 0;

TQ_EXPORT Time	tqt_x_time = CurrentTime;
TQ_EXPORT Time	tqt_x_user_time = CurrentTime;
extern bool     tqt_check_clipboard_sentinel(); //def in qclipboard_x11.cpp
extern bool	tqt_check_selection_sentinel(); //def in qclipboard_x11.cpp

static void	tqt_save_rootinfo();
bool	tqt_try_modal( TQWidget *, XEvent * );

int		tqt_ncols_option  = 216;		// used in qcolor_x11.cpp
int		tqt_visual_option = -1;
bool		tqt_cmap_option	 = false;
TQWidget	       *tqt_button_down	 = 0;		// widget got last button-down

extern bool tqt_tryAccelEvent( TQWidget*, TQKeyEvent* ); // def in qaccel.cpp

struct TQScrollInProgress {
    static long serial;
    TQScrollInProgress( TQWidget* w, int x, int y ) :
    id( serial++ ), scrolled_widget( w ), dx( x ), dy( y ) {}
    long id;
    TQWidget* scrolled_widget;
    int dx, dy;
};
long TQScrollInProgress::serial=0;
static TQPtrList<TQScrollInProgress> *sip_list = 0;


// stuff in qt_xdnd.cpp
// setup
extern void tqt_xdnd_setup();
// x event handling
extern void tqt_handle_xdnd_enter( TQWidget *, const XEvent *, bool );
extern void tqt_handle_xdnd_position( TQWidget *, const XEvent *, bool );
extern void tqt_handle_xdnd_status( TQWidget *, const XEvent *, bool );
extern void tqt_handle_xdnd_leave( TQWidget *, const XEvent *, bool );
extern void tqt_handle_xdnd_drop( TQWidget *, const XEvent *, bool );
extern void tqt_handle_xdnd_finished( TQWidget *, const XEvent *, bool );
extern void tqt_xdnd_handle_selection_request( const XSelectionRequestEvent * );
extern bool tqt_xdnd_handle_badwindow();

extern void tqt_motifdnd_handle_msg( TQWidget *, const XEvent *, bool );
extern void tqt_x11_motifdnd_init();

// client message atoms
extern Atom tqt_xdnd_enter;
extern Atom tqt_xdnd_position;
extern Atom tqt_xdnd_status;
extern Atom tqt_xdnd_leave;
extern Atom tqt_xdnd_drop;
extern Atom tqt_xdnd_finished;
// xdnd selection atom
extern Atom tqt_xdnd_selection;
extern bool tqt_xdnd_dragging;

// gui or non-gui from qapplication.cpp
extern bool tqt_is_gui_used;
extern bool tqt_app_has_font;

static bool tqt_x11_cmdline_font = false;


extern bool tqt_resolve_symlinks; // from qapplication.cpp

// Paint event clipping magic
extern void tqt_set_paintevent_clipping( TQPaintDevice* dev, const TQRegion& region);
extern void tqt_clear_paintevent_clipping();


// Palette handling
extern TQPalette *tqt_std_pal;
extern void tqt_create_std_palette();

void tqt_x11_intern_atom( const char *, Atom * );

static TQPtrList<TQWidget>* deferred_map_list = 0;
static void tqt_deferred_map_cleanup()
{
    delete deferred_map_list;
    deferred_map_list = 0;
}
void tqt_deferred_map_add( TQWidget* w)
{
    if ( !deferred_map_list ) {
	deferred_map_list = new TQPtrList<TQWidget>;
	tqAddPostRoutine( tqt_deferred_map_cleanup );
    }
    deferred_map_list->append( w );
}
void tqt_deferred_map_take( TQWidget* w )
{
    if (deferred_map_list ) {
	deferred_map_list->remove( w );
    }
}
bool tqt_deferred_map_contains( TQWidget* w )
{
    if (!deferred_map_list)
	return false;
    else
	return deferred_map_list->contains( w );
}


class TQETWidget : public TQWidget		// event translator widget
{
public:
    void setWState( WFlags f )		{ TQWidget::setWState(f); }
    void clearWState( WFlags f )	{ TQWidget::clearWState(f); }
    void setWFlags( WFlags f )		{ TQWidget::setWFlags(f); }
    void clearWFlags( WFlags f )	{ TQWidget::clearWFlags(f); }
    bool translateMouseEvent( const XEvent * );
    bool translateKeyEventInternal( const XEvent *, int& count, TQString& text, int& state, char& ascii, int &code, TQEvent::Type &type, bool willRepeat=false, bool statefulTranslation=true );
    bool translateKeyEvent( const XEvent *, bool grab );
    bool translatePaintEvent( const XEvent * );
    bool translateConfigEvent( const XEvent * );
    bool translateCloseEvent( const XEvent * );
    bool translateScrollDoneEvent( const XEvent * );
    bool translateWheelEvent( int global_x, int global_y, int delta, int state, Orientation orient );
#if defined (QT_TABLET_SUPPORT)
    bool translateXinputEvent( const XEvent* );
#endif
    bool translatePropertyEvent(const XEvent *);
};




// ************************************************************************
// Input Method support
// ************************************************************************

/*!
    An identifier name of the default input method.
*/
TQString	TQApplication::defaultIM = "imsw-multi";


/*!
    This function handles the query about location of the widget
    holding the TQInputContext instance for widget \a w.

    The input context is used for text input to widget \a w. By
    default, it returns the top-level widget of \a w.

    If you want to change the mapping of widget \w to TQInputContext
    instance, reimplement both this function and
    TQApplication::icHolderWidgets(). For example, suppose a tabbed web
    browser. The browser should allocate a input context per tab
    widget because users may switch the tabs and input a new text
    during previous input contexts live.

    See also 'Sharing input context between text widgets' and 'Preedit
    preservation' section of the class description of TQInputContext.

    \sa TQInputContext, icHolderWidgets()
*/
TQWidget *TQApplication::locateICHolderWidget( TQWidget *w )
{
    return w->topLevelWidget();
}


/*!
    This function returns all widgets holding TQInputContext.

    By default, This function returns top-level widgets. So if you
    want to change the mapping of a widget to TQInputContext instance,
    you must override this function and locateICHolderWidget().

    \sa locateICHolderWidget()
*/
TQWidgetList *TQApplication::icHolderWidgets()
{
    return TQApplication::topLevelWidgets();
}


/*!
    This function replaces all TQInputContext instances in the
    application. The function's argument is the identifier name of
    the newly selected input method.
*/
void TQApplication::changeAllInputContext( const TQString &identifierName )
{
    TQWidgetList *list = tqApp->icHolderWidgets();
    TQWidgetListIt it(*list);
    while(it.current()) {
	it.current()->changeInputContext( identifierName );
	++it;
    }
    delete list;

    // defaultIM = identifierName ; // Change of defaultIM -- default input method -- may be enabled.
}


/*!
    \internal
    This is an internal function, you should never call this.

    \sa TQInputContext::imEventGenerated()
*/
void TQApplication::postIMEvent( TQObject *receiver, TQIMEvent *event )
{
    if ( event->type() == TQEvent::IMCompose ) {
	// enable event compression to reduce preedit flicker on fast
	// typing
	postEvent( receiver, event );
    } else {
	// cancel queued preedit update
	if ( event->type() == TQEvent::IMEnd )
	    removePostedEvents( receiver, TQEvent::IMCompose );

	// to avoid event receiving order inversion between TQKeyEvent
	// and TQIMEvent, we must send IMStart and IMEnd via
	// sendEvent().
	sendEvent( receiver, event );
	delete event;
    }
}


/*!
    This function returns the identifier name of the default input
    method in this Application. The value is identical to the value of
    TQApplication::defaultIM.
*/
TQString TQApplication::defaultInputMethod()
{
    return TQApplication::defaultIM;
}


#if !defined(TQT_NO_IM_EXTENSIONS)
/*! \internal
    Creates the application input method.
*/
void TQApplication::create_im()
{
#ifndef TQT_NO_XIM
    if ( ! tqt_xim_preferred_style ) // no configured input style, use the default
	tqt_xim_preferred_style = xim_default_style;
#endif // TQT_NO_XIM
}


/*! \internal
  Closes the application input method.
*/
void TQApplication::close_im()
{
    TQWidgetList *list = tqApp->icHolderWidgets();
    TQWidgetListIt it(*list);
    while(it.current()) {
	it.current()->destroyInputContext();
	++it;
    }
    delete list;
}

#else

/*! \internal
    Creates the application input method.
*/
void TQApplication::create_xim()
{
#ifndef TQT_NO_XIM
    if ( ! tqt_xim_preferred_style ) // no configured input style, use the default
	tqt_xim_preferred_style = xim_default_style;
#endif // TQT_NO_XIM

    TQWidgetList *list= tqApp->topLevelWidgets();
    TQWidgetListIt it(*list);
    TQWidget * w;
    while( (w=it.current()) != 0 ) {
	++it;
	w->createTLSysExtra();
    }
    delete list;
}


 /*! \internal
   Closes the application input method.
 */
void TQApplication::close_xim()
{
#ifndef TQT_NO_XIM
    // Calling XCloseIM gives a Purify FMR error
    // XCloseIM( tqt_xim );
    // We prefer a less serious memory leak

    // if ( tqt_xim )
    // 	tqt_xim = 0;

#endif // TQT_NO_XIM
    TQWidgetList *list = tqApp->topLevelWidgets();
    TQWidgetListIt it(*list);
    while(it.current()) {
	it.current()->destroyInputContext();
	++it;
    }
    delete list;
}
#endif

/*****************************************************************************
  Default X error handlers
 *****************************************************************************/

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static bool x11_ignore_badwindow;
static bool x11_badwindow;

    // starts to ignore bad window errors from X
void tqt_ignore_badwindow()
{
    x11_ignore_badwindow = true;
    x11_badwindow = false;
}

    // ends ignoring bad window errors and returns whether an error
    // had happen.
bool tqt_badwindow()
{
    x11_ignore_badwindow = false;
    return x11_badwindow;
}

static int (*original_x_errhandler)( Display *dpy, XErrorEvent * );
static int (*original_xio_errhandler)( Display *dpy );

static int tqt_x_errhandler( Display *dpy, XErrorEvent *err )
{
    if ( err->error_code == BadWindow ) {
	x11_badwindow = true;
	if ( err->request_code == 25 /* X_SendEvent */ &&
	     tqt_xdnd_handle_badwindow() )
	    return 0;
	if ( x11_ignore_badwindow )
	    return 0;
    } else if ( err->error_code == BadMatch &&
		err->request_code == 42 /* X_SetInputFocus */ ) {
	return 0;
    }

    char errstr[256];
    XGetErrorText( dpy, err->error_code, errstr, 256 );
    tqWarning( "X Error: %s %d\n"
	      "  Major opcode:  %d\n"
	      "  Minor opcode:  %d\n"
	      "  Resource id:  0x%lx",
	      errstr, err->error_code,
	      err->request_code,
	      err->minor_code,
	      err->resourceid );

    // ### we really should distinguish between severe, non-severe and
    // ### application specific errors

    return 0;
}


static int tqt_xio_errhandler( Display * )
{
    tqWarning( "%s: Fatal IO error: client killed", appName );
    tqApp = 0;
    exit( 1 );
    //### give the application a chance for a proper shutdown instead,
    //### exit(1) doesn't help.
    return 0;
}

#if defined(Q_C_CALLBACKS)
}
#endif


// Memory leak: if the app exits before tqt_init_internal(), this dict
// isn't released correctly.
static TQAsciiDict<Atom> *atoms_to_be_created = 0;
static bool create_atoms_now = 0;

/*****************************************************************************
  tqt_x11_intern_atom() - efficiently interns an atom, now or later.

  If the application is being initialized, this function stores the
  adddress of the atom and tqt_init_internal will do the actual work
  quickly. If the application is running, the atom is created here.

  Neither argument may point to temporary variables.
 *****************************************************************************/

void tqt_x11_intern_atom( const char *name, Atom *result)
{
    if ( !name || !result || *result )
	return;

    if ( create_atoms_now ) {
	*result = XInternAtom( appDpy, name, False );
    } else {
	if ( !atoms_to_be_created ) {
	    atoms_to_be_created = new TQAsciiDict<Atom>;
	    atoms_to_be_created->setAutoDelete( false );
	}
	atoms_to_be_created->insert( name, result );
	*result = 0;
    }
}


static void qt_x11_process_intern_atoms()
{
    if ( atoms_to_be_created ) {
#if defined(XlibSpecificationRelease) && (XlibSpecificationRelease >= 6)
	int i = atoms_to_be_created->count();
	Atom * res = (Atom *)malloc( i * sizeof( Atom ) );
	Atom ** resp = (Atom **)malloc( i * sizeof( Atom* ) );
	char ** names = (char **)malloc( i * sizeof(const char*));

	i = 0;
	TQAsciiDictIterator<Atom> it( *atoms_to_be_created );
	while( it.current() ) {
	    res[i] = 0;
	    resp[i] = it.current();
	    names[i] = tqstrdup(it.currentKey());
	    i++;
	    ++it;
	}
	XInternAtoms( appDpy, names, i, False, res );
	while( i ) {
	    i--;
	    delete [] names[i];
	    if ( res[i] && resp[i] )
		*(resp[i]) = res[i];
	}
	free( res );
	free( resp );
	free( names );
#else
	TQAsciiDictIterator<Atom> it( *atoms_to_be_created );
	Atom * result;
	const char * name;
	while( (result = it.current()) != 0 ) {
	    name = it.currentKey();
	    ++it;
	    *result = XInternAtom( appDpy, name, False );
	}
#endif
	delete atoms_to_be_created;
	atoms_to_be_created = 0;
	create_atoms_now = true;
    }
}


/*! \internal
    apply the settings to the application
*/
bool TQApplication::x11_apply_settings()
{
    if (! tqt_std_pal)
	tqt_create_std_palette();

    Atom type;
    int format;
    long offset = 0;
    unsigned long nitems, after = 1;
    unsigned char *data = 0;
    TQDateTime timestamp, settingsstamp;
    bool update_timestamp = false;

    if (XGetWindowProperty(appDpy, TQPaintDevice::x11AppRootWindow( 0 ),
			   tqt_settings_timestamp, 0, 0,
			   False, AnyPropertyType, &type, &format, &nitems,
			   &after, &data) == Success && format == 8) {
	if (data)
	    XFree(data);

	TQBuffer ts;
	ts.open(IO_WriteOnly);

	while (after > 0) {
	    XGetWindowProperty(appDpy, TQPaintDevice::x11AppRootWindow( 0 ),
			       tqt_settings_timestamp,
			       offset, 1024, False, AnyPropertyType,
			       &type, &format, &nitems, &after, &data);
	    if (format == 8) {
		ts.writeBlock((const char *) data, nitems);
		offset += nitems / 4;
	    }

	    XFree(data);
	}

	TQDataStream d(ts.buffer(), IO_ReadOnly);
	d >> timestamp;
    }

    TQSettings settings;
    settingsstamp = settings.lastModificationTime( "/qt/font" );
    if (! settingsstamp.isValid())
	return false;

    if ( appliedstamp && appliedstamp == settingsstamp.toTime_t() )
	return true;
    appliedstamp = settingsstamp.toTime_t();

    if (! timestamp.isValid() || settingsstamp > timestamp)
	update_timestamp = true;

    /*
      TQt settings. This is now they are written into the datastream.

      /qt/Palette/ *             - TQPalette
      /qt/font                   - TQFont
      /qt/libraryPath            - TQStringList
      /qt/style                  - TQString
      /qt/doubleClickInterval    - int
      /qt/cursorFlashTime        - int
      /qt/wheelScrollLines       - int
      /qt/colorSpec              - TQString
      /qt/defaultCodec           - TQString
      /qt/globalStrut            - TQSize
      /qt/GUIEffects             - TQStringList
      /qt/Font Substitutions/ *  - TQStringList
      /qt/Font Substitutions/... - TQStringList
    */

    TQString str;
    TQStringList strlist;
    int i, num;
    TQPalette pal(TQApplication::palette());
    strlist = settings.readListEntry("/qt/Palette/active");
    if (strlist.count() == TQColorGroup::NColorRoles) {
	for (i = 0; i < TQColorGroup::NColorRoles; i++)
	    pal.setColor(TQPalette::Active, (TQColorGroup::ColorRole) i,
			 TQColor(strlist[i]));
    }
    strlist = settings.readListEntry("/qt/Palette/inactive");
    if (strlist.count() == TQColorGroup::NColorRoles) {
	for (i = 0; i < TQColorGroup::NColorRoles; i++)
	    pal.setColor(TQPalette::Inactive, (TQColorGroup::ColorRole) i,
			 TQColor(strlist[i]));
    }
    strlist = settings.readListEntry("/qt/Palette/disabled");
    if (strlist.count() == TQColorGroup::NColorRoles) {
	for (i = 0; i < TQColorGroup::NColorRoles; i++)
	    pal.setColor(TQPalette::Disabled, (TQColorGroup::ColorRole) i,
			 TQColor(strlist[i]));
    }

    // workaround for KDE 3.0, which messes up the buttonText value of
    // the disabled palette in TQSettings
    if ( pal.disabled().buttonText() == pal.active().buttonText() ) {
	pal.setColor( TQPalette::Disabled, TQColorGroup::ButtonText,
		      pal.disabled().foreground() );
    }

    if (pal != *tqt_std_pal && pal != TQApplication::palette()) {
	TQApplication::setPalette(pal, true);
	*tqt_std_pal = pal;
    }

    TQFont font(TQApplication::font());
    if ( !tqt_app_has_font && !tqt_x11_cmdline_font ) {
        // read new font
        str = settings.readEntry("/qt/font");
        if (! str.isNull() && ! str.isEmpty()) {
            font.fromString(str);

            if (font != TQApplication::font())
                TQApplication::setFont(font, true);
        }
    }

    // read library (ie. plugin) path list
    TQString libpathkey =
	TQString("/qt/%1.%2/libraryPath").arg( TQT_VERSION >> 16 ).arg( (TQT_VERSION & 0xff00 ) >> 8 );
    TQStringList pathlist = settings.readListEntry(libpathkey, ':');
    if (! pathlist.isEmpty()) {
	TQStringList::ConstIterator it = pathlist.begin();
	while (it != pathlist.end())
	    TQApplication::addLibraryPath(*it++);
    }

    // read new TQStyle
    extern bool tqt_explicit_app_style; // defined in qapplication.cpp
    TQString stylename = settings.readEntry( "/qt/style" );
    if ( !stylename.isEmpty() && !tqt_explicit_app_style ) {
	TQApplication::setStyle( stylename );
	// took the style from the user settings, so mark the explicit flag false
	tqt_explicit_app_style = false;
    }

    num =
	settings.readNumEntry("/qt/doubleClickInterval",
			      TQApplication::doubleClickInterval());
    TQApplication::setDoubleClickInterval(num);

    num =
	settings.readNumEntry("/qt/cursorFlashTime",
			      TQApplication::cursorFlashTime());
    TQApplication::setCursorFlashTime(num);

    num =
	settings.readNumEntry("/qt/wheelScrollLines",
			      TQApplication::wheelScrollLines());
    TQApplication::setWheelScrollLines(num);

    TQString colorspec = settings.readEntry("/qt/colorSpec", "default");
    if (colorspec == "normal")
	TQApplication::setColorSpec(TQApplication::NormalColor);
    else if (colorspec == "custom")
	TQApplication::setColorSpec(TQApplication::CustomColor);
    else if (colorspec == "many")
	TQApplication::setColorSpec(TQApplication::ManyColor);
    else if (colorspec != "default")
	colorspec = "default";

    TQString defaultcodec = settings.readEntry("/qt/defaultCodec", "none");
    if (defaultcodec != "none") {
	TQTextCodec *codec = TQTextCodec::codecForName(defaultcodec);
	if (codec)
	    tqApp->setDefaultCodec(codec);
    }

    TQStringList strut = settings.readListEntry("/qt/globalStrut");
    if (! strut.isEmpty()) {
	if (strut.count() == 2) {
	    TQSize sz(strut[0].toUInt(), strut[1].toUInt());

	    if (sz.isValid())
		TQApplication::setGlobalStrut(sz);
	}
    }

    TQStringList effects = settings.readListEntry("/qt/GUIEffects");

    TQApplication::setEffectEnabled( TQt::UI_General, effects.contains("general") );
    TQApplication::setEffectEnabled( TQt::UI_AnimateMenu, effects.contains("animatemenu") );
    TQApplication::setEffectEnabled( TQt::UI_FadeMenu, effects.contains("fademenu") );
    TQApplication::setEffectEnabled( TQt::UI_AnimateCombo, effects.contains("animatecombo") );
    TQApplication::setEffectEnabled( TQt::UI_AnimateTooltip, effects.contains("animatetooltip") );
    TQApplication::setEffectEnabled( TQt::UI_FadeTooltip, effects.contains("fadetooltip") );
    TQApplication::setEffectEnabled( TQt::UI_AnimateToolBox, effects.contains("animatetoolbox") );

    TQStringList fontsubs =
	settings.entryList("/qt/Font Substitutions");
    if (!fontsubs.isEmpty()) {
	TQStringList subs;
	TQString fam, skey;
	TQStringList::Iterator it = fontsubs.begin();
	while (it != fontsubs.end()) {
	    fam = (*it++);
	    skey = "/qt/Font Substitutions/" + fam;
	    subs = settings.readListEntry(skey);
	    TQFont::insertSubstitutions(fam, subs);
	}
    }

    tqt_broken_wm =
	settings.readBoolEntry("/qt/brokenWindowManager", tqt_broken_wm);

    tqt_resolve_symlinks =
	settings.readBoolEntry("/qt/resolveSymlinks", true);

    tqt_use_rtl_extensions =
    	settings.readBoolEntry("/qt/useRtlExtensions", false);

#ifndef TQT_NO_XIM
    if (tqt_xim_preferred_style == 0) {
        TQString ximInputStyle =
            settings.readEntry( "/qt/XIMInputStyle",
                                TQString::fromLatin1( "On The Spot" ) ).lower();
        if ( ximInputStyle == "over the spot" )
            tqt_xim_preferred_style = XIMPreeditPosition | XIMStatusNothing;
        else if ( ximInputStyle == "off the spot" )
            tqt_xim_preferred_style = XIMPreeditArea | XIMStatusArea;
        else if ( ximInputStyle == "root" )
            tqt_xim_preferred_style = XIMPreeditNothing | XIMStatusNothing;
        else // ximInputStyle == "on the spot" or others
            tqt_xim_preferred_style = XIMPreeditCallbacks | XIMStatusNothing;
    }
#endif

#ifndef TQT_NO_IM
    /*
	The identifier name of an input method is acquired from the
	configuration file as a default. If a environment variable
	"TQT_IM_SWITCHER" is not empty it will overwrite the
	configuration file. The "imsw-multi" becomes the default if the entry
	is not configured.
     */
    if ( getenv( "TQT_IM_SWITCHER" ) )
        defaultIM = getenv( "TQT_IM_SWITCHER" );
#ifndef TQT_NO_IM_EXTENSIONS
    else
        defaultIM = settings.readEntry( "/qt/DefaultInputMethodSwitcher", "imsw-multi" );
#endif

    // defaultIM is restricted to be an IM-switcher. An IM-switcher
    // has a 'imsw-' prefix
    if ( ! defaultIM.startsWith( "imsw-" ) ) {
	defaultIM = "imsw-multi";
    }
#endif

    if (update_timestamp) {
	TQBuffer stamp;
	TQDataStream s(stamp.buffer(), IO_WriteOnly);
	s << settingsstamp;

	XChangeProperty(appDpy, TQPaintDevice::x11AppRootWindow( 0 ),
			tqt_settings_timestamp, tqt_settings_timestamp, 8,
			PropModeReplace, (unsigned char *) stamp.buffer().data(),
			stamp.buffer().size());
    }

    return true;
}


// read the _QT_INPUT_ENCODING property and apply the settings to
// the application
static void tqt_set_input_encoding()
{
    Atom type;
    int format;
    ulong  nitems, after = 1;
    const char *data;

    int e = XGetWindowProperty( appDpy, TQPaintDevice::x11AppRootWindow(),
				tqt_input_encoding, 0, 1024,
				False, XA_STRING, &type, &format, &nitems,
				&after,  (unsigned char**)&data );
    if ( e != Success || !nitems || type == None ) {
	// Always use the locale codec, since we have no examples of non-local
	// XIMs, and since we cannot get a sensible answer about the encoding
	// from the XIM.
	tqt_input_mapper = TQTextCodec::codecForLocale();

    } else {
	if ( !tqstricmp( data, "locale" ) )
	    tqt_input_mapper = TQTextCodec::codecForLocale();
	else
	    tqt_input_mapper = TQTextCodec::codecForName( data );
	// make sure we have an input codec
	if( !tqt_input_mapper )
	    tqt_input_mapper = TQTextCodec::codecForName( "ISO 8859-1" );
    }
    if ( tqt_input_mapper->mibEnum() == 11 ) // 8859-8
	tqt_input_mapper = TQTextCodec::codecForName( "ISO 8859-8-I");
    if( data )
	XFree( (char *)data );
}

// set font, foreground and background from x11 resources. The
// arguments may override the resource settings.
static void tqt_set_x11_resources( const char* font = 0, const char* fg = 0,
				  const char* bg = 0, const char* button = 0 )
{
    if ( !tqt_std_pal )
	tqt_create_std_palette();

    TQCString resFont, resFG, resBG, resEF, sysFont;

    TQApplication::setEffectEnabled( TQt::UI_General, false);
    TQApplication::setEffectEnabled( TQt::UI_AnimateMenu, false);
    TQApplication::setEffectEnabled( TQt::UI_FadeMenu, false);
    TQApplication::setEffectEnabled( TQt::UI_AnimateCombo, false );
    TQApplication::setEffectEnabled( TQt::UI_AnimateTooltip, false );
    TQApplication::setEffectEnabled( TQt::UI_FadeTooltip, false );
    TQApplication::setEffectEnabled( TQt::UI_AnimateToolBox, false );

    if ( TQApplication::desktopSettingsAware() && !TQApplication::x11_apply_settings()  ) {
	int format;
	ulong  nitems, after = 1;
	TQCString res;
	long offset = 0;
	Atom type = None;

	while (after > 0) {
	    uchar *data;
	    XGetWindowProperty( appDpy, TQPaintDevice::x11AppRootWindow( 0 ),
				tqt_resource_manager,
				offset, 8192, False, AnyPropertyType,
				&type, &format, &nitems, &after,
				&data );
	    res += (char*)data;
	    offset += 2048; // offset is in 32bit quantities... 8192/4 == 2048
	    if ( data )
		XFree( (char *)data );
	}

	TQCString key, value;
	int l = 0, r;
	TQCString apn = appName;
	TQCString apc = appClass;
	int apnl = apn.length();
	int apcl = apc.length();
	int resl = res.length();

	while (l < resl) {
	    r = res.find( '\n', l );
	    if ( r < 0 )
		r = resl;
	    while ( isspace((uchar) res[l]) )
		l++;
	    bool mine = false;
	    if ( res[l] == '*' &&
		 (res[l+1] == 'f' || res[l+1] == 'b' || res[l+1] == 'g' ||
		  res[l+1] == 'F' || res[l+1] == 'B' || res[l+1] == 'G' ||
		  res[l+1] == 's' || res[l+1] == 'S' ) ) {
		// OPTIMIZED, since we only want "*[fbgs].."

		TQCString item = res.mid( l, r - l ).simplifyWhiteSpace();
		int i = item.find( ":" );
		key = item.left( i ).stripWhiteSpace().mid(1).lower();
		value = item.right( item.length() - i - 1 ).stripWhiteSpace();
		mine = true;
	    } else if ( res[l] == appName[0] || (appClass && res[l] == appClass[0]) ) {
		if (res.mid(l,apnl) == apn && (res[l+apnl] == '.' || res[l+apnl] == '*')) {
		    TQCString item = res.mid( l, r - l ).simplifyWhiteSpace();
		    int i = item.find( ":" );
		    key = item.left( i ).stripWhiteSpace().mid(apnl+1).lower();
		    value = item.right( item.length() - i - 1 ).stripWhiteSpace();
		    mine = true;
		} else if (res.mid(l,apcl) == apc && (res[l+apcl] == '.' || res[l+apcl] == '*')) {
		    TQCString item = res.mid( l, r - l ).simplifyWhiteSpace();
		    int i = item.find( ":" );
		    key = item.left( i ).stripWhiteSpace().mid(apcl+1).lower();
		    value = item.right( item.length() - i - 1 ).stripWhiteSpace();
		    mine = true;
		}
	    }

	    if ( mine ) {
		if ( !font && key == "systemfont")
		    sysFont = value.left( value.findRev(':') ).copy();
		if ( !font && key == "font")
		    resFont = value.copy();
		else if  ( !fg &&  key == "foreground" )
		    resFG = value.copy();
		else if ( !bg && key == "background")
		    resBG = value.copy();
		else if ( key == "guieffects")
		    resEF = value.copy();
		// NOTE: if you add more, change the [fbg] stuff above
	    }

	    l = r + 1;
	}
    }
    if ( !sysFont.isEmpty() )
	resFont = sysFont;
    if ( resFont.isEmpty() )
	resFont = font;
    if ( resFG.isEmpty() )
	resFG = fg;
    if ( resBG.isEmpty() )
	resBG = bg;
    if ( (!tqt_app_has_font || tqt_x11_cmdline_font) && !resFont.isEmpty() ) { // set application font
        TQFont fnt;
        fnt.setRawName( resFont );

        // the font we get may actually be an alias for another font,
        // so we reset the application font to the real font info.
        if ( ! fnt.exactMatch() ) {
            TQFontInfo fontinfo( fnt );
            fnt.setFamily( fontinfo.family() );
            fnt.setRawMode( fontinfo.rawMode() );

            if ( ! fnt.rawMode() ) {
                fnt.setItalic( fontinfo.italic() );
                fnt.setWeight( fontinfo.weight() );
                fnt.setUnderline( fontinfo.underline() );
                fnt.setStrikeOut( fontinfo.strikeOut() );
                fnt.setStyleHint( fontinfo.styleHint() );

                if ( fnt.pointSize() <= 0 && fnt.pixelSize() <= 0 )
                    // size is all wrong... fix it
                    fnt.setPointSize( (int) ( ( fontinfo.pixelSize() * 72. /
                                                (float) TQPaintDevice::x11AppDpiY() ) +
                                              0.5 ) );
            }
        }

        if ( fnt != TQApplication::font() ) {
            TQApplication::setFont( fnt, true );
        }
    }

    if ( button || !resBG.isEmpty() || !resFG.isEmpty() ) {// set app colors
	TQColor btn;
	TQColor bg;
	TQColor fg;
	if ( !resBG.isEmpty() )
	    bg = TQColor(TQString(resBG));
	else
	    bg = tqt_std_pal->active().background();
	if ( !resFG.isEmpty() )
	    fg = TQColor(TQString(resFG));
	else
	    fg = tqt_std_pal->active().foreground();
	if ( button )
	    btn = TQColor( button );
	else if ( !resBG.isEmpty() )
	    btn = bg;
	else
	    btn = tqt_std_pal->active().button();

	int h,s,v;
	fg.hsv(&h,&s,&v);
	TQColor base = TQt::white;
	bool bright_mode = false;
	if (v >= 255-50) {
	    base = btn.dark(150);
	    bright_mode = true;
	}

	TQColorGroup cg( fg, btn, btn.light(),
			btn.dark(), btn.dark(150), fg, TQt::white, base, bg );
	if (bright_mode) {
	    cg.setColor( TQColorGroup::HighlightedText, base );
	    cg.setColor( TQColorGroup::Highlight, TQt::white );
	} else {
	    cg.setColor( TQColorGroup::HighlightedText, TQt::white );
	    cg.setColor( TQColorGroup::Highlight, TQt::darkBlue );
	}
	TQColor disabled( (fg.red()+btn.red())/2,
			 (fg.green()+btn.green())/2,
			 (fg.blue()+btn.blue())/2);
	TQColorGroup dcg( disabled, btn, btn.light( 125 ), btn.dark(), btn.dark(150),
			 disabled, TQt::white, TQt::white, bg );
	if (bright_mode) {
	    dcg.setColor( TQColorGroup::HighlightedText, base );
	    dcg.setColor( TQColorGroup::Highlight, TQt::white );
	} else {
	    dcg.setColor( TQColorGroup::HighlightedText, TQt::white );
	    dcg.setColor( TQColorGroup::Highlight, TQt::darkBlue );
	}
	TQPalette pal( cg, dcg, cg );
	if ( pal != *tqt_std_pal && pal != TQApplication::palette() )
	    TQApplication::setPalette( pal, true );
	*tqt_std_pal = pal;
    }

    if ( !resEF.isEmpty() ) {
	TQStringList effects = TQStringList::split(" ",resEF);
	TQApplication::setEffectEnabled( TQt::UI_General,  effects.contains("general") );
	TQApplication::setEffectEnabled( TQt::UI_AnimateMenu, effects.contains("animatemenu") );
	TQApplication::setEffectEnabled( TQt::UI_FadeMenu, effects.contains("fademenu") );
	TQApplication::setEffectEnabled( TQt::UI_AnimateCombo, effects.contains("animatecombo") );
	TQApplication::setEffectEnabled( TQt::UI_AnimateTooltip, effects.contains("animatetooltip") );
	TQApplication::setEffectEnabled( TQt::UI_FadeTooltip, effects.contains("fadetooltip") );
	TQApplication::setEffectEnabled( TQt::UI_AnimateToolBox, effects.contains("animatetoolbox") );
    }
}


static void tqt_detect_broken_window_manager()
{
    Atom type;
    int format;
    ulong nitems, after;
    uchar *data = 0;

    // look for SGI's 4Dwm
    int e = XGetWindowProperty(appDpy, TQPaintDevice::x11AppRootWindow(),
                               tqt_sgi_desks_manager, 0, 1, False, XA_WINDOW,
                               &type, &format, &nitems, &after, &data);
    if (data)
        XFree(data);

    if (e == Success && type == XA_WINDOW && format == 32 && nitems == 1 && after == 0) {
        // detected SGI 4Dwm
        tqt_broken_wm = true;
    }
}


// update the supported array
void tqt_get_net_supported()
{
    Atom type;
    int format;
    long offset = 0;
    unsigned long nitems, after;
    unsigned char *data = 0;

    int e = XGetWindowProperty(appDpy, TQPaintDevice::x11AppRootWindow(),
			       tqt_net_supported, 0, 0,
			       False, XA_ATOM, &type, &format, &nitems, &after, &data);
    if (data)
	XFree(data);

    if (tqt_net_supported_list)
	delete [] tqt_net_supported_list;
    tqt_net_supported_list = 0;

    if (e == Success && type == XA_ATOM && format == 32) {
	TQBuffer ts;
	ts.open(IO_WriteOnly);

	while (after > 0) {
	    XGetWindowProperty(appDpy, TQPaintDevice::x11AppRootWindow(),
			       tqt_net_supported, offset, 1024,
			       False, XA_ATOM, &type, &format, &nitems, &after, &data);

	    if (type == XA_ATOM && format == 32) {
		ts.writeBlock((const char *) data, nitems * sizeof(long));
		offset += nitems;
	    } else
		after = 0;
	    if (data)
		XFree(data);
	}

	// compute nitems
	TQByteArray buffer(ts.buffer());
	nitems = buffer.size() / sizeof(Atom);
	tqt_net_supported_list = new Atom[nitems + 1];
	Atom *a = (Atom *) buffer.data();
	uint i;
	for (i = 0; i < nitems; i++)
	    tqt_net_supported_list[i] = a[i];
	tqt_net_supported_list[nitems] = 0;
    }
}


bool tqt_net_supports(Atom atom)
{
    if (! tqt_net_supported_list)
	return false;

    bool supported = false;
    int i = 0;
    while (tqt_net_supported_list[i] != 0) {
	if (tqt_net_supported_list[i++] == atom) {
	    supported = true;
	    break;
	}
    }

    return supported;
}


// update the virtual roots array
void tqt_get_net_virtual_roots()
{
    if (tqt_net_virtual_root_list)
	delete [] tqt_net_virtual_root_list;
    tqt_net_virtual_root_list = 0;

    if (! tqt_net_supports(tqt_net_virtual_roots))
	return;

    Atom type;
    int format;
    long offset = 0;
    unsigned long nitems, after;
    unsigned char *data;

    int e = XGetWindowProperty(appDpy, TQPaintDevice::x11AppRootWindow(),
			       tqt_net_virtual_roots, 0, 0,
			       False, XA_ATOM, &type, &format, &nitems, &after, &data);
    if (data)
	XFree(data);

    if (e == Success && type == XA_ATOM && format == 32) {
	TQBuffer ts;
	ts.open(IO_WriteOnly);

	while (after > 0) {
	    XGetWindowProperty(appDpy, TQPaintDevice::x11AppRootWindow(),
			       tqt_net_virtual_roots, offset, 1024,
			       False, XA_ATOM, &type, &format, &nitems, &after, &data);

	    if (type == XA_ATOM && format == 32) {
		ts.writeBlock((const char *) data, nitems * 4);
		offset += nitems;
	    } else
		after = 0;
	    if (data)
		XFree(data);
	}

	// compute nitems
	TQByteArray buffer(ts.buffer());
	nitems = buffer.size() / sizeof(Window);
	tqt_net_virtual_root_list = new Window[nitems + 1];
	Window *a = (Window *) buffer.data();
	uint i;
	for (i = 0; i < nitems; i++)
	    tqt_net_virtual_root_list[i] = a[i];
	tqt_net_virtual_root_list[nitems] = 0;
    }
}

void tqt_x11_create_wm_client_leader()
{
    if ( tqt_x11_wm_client_leader ) return;

    tqt_x11_wm_client_leader =
	XCreateSimpleWindow( TQPaintDevice::x11AppDisplay(),
			     TQPaintDevice::x11AppRootWindow(),
			     0, 0, 1, 1, 0, 0, 0 );

    // set client leader property to itself
    XChangeProperty( TQPaintDevice::x11AppDisplay(),
		     tqt_x11_wm_client_leader, tqt_wm_client_leader,
		     XA_WINDOW, 32, PropModeReplace,
		     (unsigned char *)&tqt_x11_wm_client_leader, 1 );

    // If we are session managed, inform the window manager about it
    TQCString session = tqApp->sessionId().latin1();
    if ( !session.isEmpty() ) {
	XChangeProperty( TQPaintDevice::x11AppDisplay(),
			 tqt_x11_wm_client_leader, tqt_sm_client_id,
			 XA_STRING, 8, PropModeReplace,
			 (unsigned char *)session.data(), session.length() );
    }
}

static void tqt_net_update_user_time(TQWidget *tlw)
{
    XChangeProperty(TQPaintDevice::x11AppDisplay(), tlw->winId(), tqt_net_wm_user_time, XA_CARDINAL,
		    32, PropModeReplace, (unsigned char *) &tqt_x_user_time, 1);
}

static void tqt_check_focus_model()
{
    Window fw = None;
    int unused;
    XGetInputFocus( appDpy, &fw, &unused );
    if ( fw == PointerRoot )
	tqt_focus_model = FocusModel_PointerRoot;
    else
	tqt_focus_model = FocusModel_Other;
}


/*
  Returns a truecolor visual (if there is one). 8-bit TrueColor visuals
  are ignored, unless the user has explicitly requested -visual TrueColor.
  The SGI X server usually has an 8 bit default visual, but the application
  can also ask for a truecolor visual. This is what we do if
  TQApplication::colorSpec() is TQApplication::ManyColor.
*/

static Visual *find_truecolor_visual( Display *dpy, int scr, int *depth, int *ncols )
{
    XVisualInfo *vi, rvi;
    int best=0, n, i;
    rvi.c_class = TrueColor;
    rvi.screen  = scr;
    vi = XGetVisualInfo( dpy, VisualClassMask | VisualScreenMask,
			 &rvi, &n );
    if ( vi ) {
	for ( i=0; i<n; i++ ) {
	    if ( vi[i].depth > vi[best].depth )
		best = i;
	}
    }
    Visual *v = DefaultVisual(dpy,scr);
    if ( !vi || (vi[best].visualid == XVisualIDFromVisual(v)) ||
	 (vi[best].depth <= 8 && tqt_visual_option != TrueColor) )
	{
	*depth = DefaultDepth(dpy,scr);
	*ncols = DisplayCells(dpy,scr);
    } else {
	v = vi[best].visual;
	*depth = vi[best].depth;
	*ncols = vi[best].colormap_size;
    }
    if ( vi )
	XFree( (char *)vi );
    return v;
}

static KeySym tqt_x11_keycode_to_keysym(Display *dpy, KeyCode kc) {
#ifndef TQT_NO_XKB
	return XkbKeycodeToKeysym(dpy, kc, 0, 0);
#else
	KeySym rv = NoSymbol;
	int keysyms_per_keycode;
	KeySym *keysym_p = XGetKeyboardMapping(dpy, kc, 1, &keysyms_per_keycode);
	if (keysyms_per_keycode>0) { //< Should always be true unless X server is bugged
		rv = keysym_p[0];
	}
	XFree(keysym_p);

	return rv;
#endif // TQT_NO_XKB
}

/*****************************************************************************
  tqt_init() - initializes TQt for X11
 *****************************************************************************/

#define XK_MISCELLANY
#define XK_LATIN1
#define XK_KOREAN
#define XK_XKB_KEYS
#include <X11/keysymdef.h>

// ### This should be static but it isn't because of the friend declaration
// ### in ntqpaintdevice.h which then should have a static too but can't have
// ### it because "storage class specifiers invalid in friend function
// ### declarations" :-) Ideas anyone?
void tqt_init_internal( int *argcptr, char **argv,
		       Display *display, TQt::HANDLE visual, TQt::HANDLE colormap )
{
    setlocale( LC_ALL, "" );		// use correct char set mapping
    setlocale( LC_NUMERIC, "C" );	// make sprintf()/scanf() work

#if defined(TQT_THREAD_SUPPORT)
    if (( tqt_is_gui_used ) && ( !display )) {
	// If TQt is running standalone with a GUI, initialize X11 threading
	XInitThreads();
    }
#endif

    if ( display && ((!argcptr) || (!argv)) ) {
	// TQt part of other application

	appForeignDpy = true;
	appDpy  = display;

	// Set application name and class
	appName = tqstrdup( "TQt-subapplication" );
	char *app_class = 0;
	if (argv) {
	    const char* p = strrchr( argv[0], '/' );
	    app_class = tqstrdup(p ? p + 1 : argv[0]);
	    if (app_class[0])
		app_class[0] = toupper(app_class[0]);
	}
	appClass = app_class;

	// Install default error handlers
	original_x_errhandler = XSetErrorHandler( tqt_x_errhandler );
	original_xio_errhandler = XSetIOErrorHandler( tqt_xio_errhandler );
    } else {
	// TQt controls everything (default)

	int argc = *argcptr;
	int j;

	// Install default error handlers
	original_x_errhandler = XSetErrorHandler( tqt_x_errhandler );
	original_xio_errhandler = XSetIOErrorHandler( tqt_xio_errhandler );

	// Set application name and class
	char *app_class = 0;
	if (argv) {
	    const char *p = strrchr( argv[0], '/' );
	    appName = p ? p + 1 : argv[0];
	    app_class = tqstrdup(appName);
	    if (app_class[0])
		app_class[0] = toupper(app_class[0]);
	}
	appClass = app_class;

	// Get command line params
	j = argc ? 1 : 0;
	for ( int i=1; i<argc; i++ ) {
	    if ( argv[i] && *argv[i] != '-' ) {
		argv[j++] = argv[i];
		continue;
	    }
	    TQCString arg = argv[i];
	    if ( arg == "-display" ) {
		if ( ++i < argc )
		    appDpyName = argv[i];
	    } else if ( arg == "-fn" || arg == "-font" ) {
		if ( ++i < argc ) {
		    appFont = argv[i];
		    tqt_x11_cmdline_font = true;
		}
	    } else if ( arg == "-bg" || arg == "-background" ) {
		if ( ++i < argc )
		    appBGCol = argv[i];
	    } else if ( arg == "-btn" || arg == "-button" ) {
		if ( ++i < argc )
		    appBTNCol = argv[i];
	    } else if ( arg == "-fg" || arg == "-foreground" ) {
		if ( ++i < argc )
		    appFGCol = argv[i];
	    } else if ( arg == "-name" ) {
		if ( ++i < argc )
		    appName = argv[i];
	    } else if ( arg == "-title" ) {
		if ( ++i < argc )
		    mwTitle = argv[i];
	    } else if ( arg == "-geometry" ) {
		if ( ++i < argc )
		    mwGeometry = argv[i];
		//Ming-Che 10/10
	    } else if ( arg == "-im" ) {
		if ( ++i < argc )
		    tqt_ximServer = argv[i];
	    } else if ( arg == "-iconic" ) {
		mwIconic = !mwIconic;
	    } else if ( arg == "-ncols" ) {   // xv and netscape use this name
		if ( ++i < argc )
		    tqt_ncols_option = TQMAX(0,atoi(argv[i]));
	    } else if ( arg == "-visual" ) {  // xv and netscape use this name
		if ( ++i < argc ) {
		    TQCString s = TQCString(argv[i]).lower();
		    if ( s == "truecolor" ) {
			tqt_visual_option = TrueColor;
		    } else {
			// ### Should we honor any others?
		    }
		}
#ifndef TQT_NO_XIM
	    } else if ( arg == "-inputstyle" ) {
		if ( ++i < argc ) {
		    TQCString s = TQCString(argv[i]).lower();
		    if ( s == "onthespot" )
			tqt_xim_preferred_style = XIMPreeditCallbacks |
						 XIMStatusNothing;
		    else if ( s == "overthespot" )
			tqt_xim_preferred_style = XIMPreeditPosition |
						 XIMStatusNothing;
		    else if ( s == "offthespot" )
			tqt_xim_preferred_style = XIMPreeditArea |
						 XIMStatusArea;
		    else if ( s == "root" )
			tqt_xim_preferred_style = XIMPreeditNothing |
						 XIMStatusNothing;
		}
#endif
	    } else if ( arg == "-cmap" ) {    // xv uses this name
		tqt_cmap_option = true;
	    }
#if defined(QT_DEBUG)
	    else if ( arg == "-sync" )
		appSync = !appSync;
	    else if ( arg == "-nograb" )
		appNoGrab = !appNoGrab;
	    else if ( arg == "-dograb" )
		appDoGrab = !appDoGrab;
#endif
	    else
		argv[j++] = argv[i];
	}

	*argcptr = j;

#if defined(QT_DEBUG) && defined(Q_OS_LINUX)
	if ( !appNoGrab && !appDoGrab ) {
	    TQCString s;
	    s.sprintf( "/proc/%d/cmdline", getppid() );
	    TQFile f( s );
	    if ( f.open( IO_ReadOnly ) ) {
		s.truncate( 0 );
		int c;
		while ( (c = f.getch()) > 0 ) {
		    if ( c == '/' )
			s.truncate( 0 );
		    else
			s += (char)c;
		}
		if ( s == "gdb" ) {
		    appNoGrab = true;
		    tqDebug( "TQt: gdb: -nograb added to command-line options.\n"
			    "\t Use the -dograb option to enforce grabbing." );
		}
		f.close();
	    }
	}
#endif
	if ( display ) {
		// Display connection already opened by another application

		appForeignDpy = true;
		appDpy  = display;
	}
	else {
		// Connect to X server
	
		if( tqt_is_gui_used ) {
		if ( ( appDpy = XOpenDisplay(appDpyName) ) == 0 ) {
			tqWarning( "%s: cannot connect to X server %s", appName,
				XDisplayName(appDpyName) );
			tqApp = 0;
			exit( 1 );
		}
	
		if ( appSync )				// if "-sync" argument
			XSynchronize( appDpy, true );
		}
	}
    }
    // Common code, regardless of whether display is foreign.

    // Get X parameters

    if( tqt_is_gui_used ) {
	appScreen = DefaultScreen(appDpy);
	appScreenCount = ScreenCount(appDpy);

	TQPaintDevice::x_appdisplay = appDpy;
	TQPaintDevice::x_appscreen = appScreen;

	// allocate the arrays for the TQPaintDevice data
	TQPaintDevice::x_appdepth_arr = new int[ appScreenCount ];
	TQPaintDevice::x_appcells_arr = new int[ appScreenCount ];
	TQPaintDevice::x_approotwindow_arr = new TQt::HANDLE[ appScreenCount ];
	TQPaintDevice::x_appcolormap_arr = new TQt::HANDLE[ appScreenCount ];
	TQPaintDevice::x_appdefcolormap_arr = new bool[ appScreenCount ];
	TQPaintDevice::x_appvisual_arr = new void*[ appScreenCount ];
	TQPaintDevice::x_appdefvisual_arr = new bool[ appScreenCount ];
	TQ_CHECK_PTR( TQPaintDevice::x_appdepth_arr );
	TQ_CHECK_PTR( TQPaintDevice::x_appcells_arr );
	TQ_CHECK_PTR( TQPaintDevice::x_approotwindow_arr );
	TQ_CHECK_PTR( TQPaintDevice::x_appcolormap_arr );
	TQ_CHECK_PTR( TQPaintDevice::x_appdefcolormap_arr );
	TQ_CHECK_PTR( TQPaintDevice::x_appvisual_arr );
	TQ_CHECK_PTR( TQPaintDevice::x_appdefvisual_arr );

	int screen;
	TQString serverVendor( ServerVendor( appDpy) );
	if (serverVendor.contains("XFree86") && VendorRelease(appDpy) < 40300000)
	    tqt_hebrew_keyboard_hack = true;

	for ( screen = 0; screen < appScreenCount; ++screen ) {
	    TQPaintDevice::x_appdepth_arr[ screen ] = DefaultDepth(appDpy, screen);
	    TQPaintDevice::x_appcells_arr[ screen ] = DisplayCells(appDpy, screen);
	    TQPaintDevice::x_approotwindow_arr[ screen ] = RootWindow(appDpy, screen);

	    // setup the visual and colormap for each screen
	    Visual *vis = 0;
	    if ( visual && screen == appScreen ) {
		// use the provided visual on the default screen only
		vis = (Visual *) visual;

		// figure out the depth of the visual we are using
		XVisualInfo *vi, rvi;
		int n;
		rvi.visualid = XVisualIDFromVisual(vis);
		rvi.screen  = screen;
		vi = XGetVisualInfo( appDpy, VisualIDMask | VisualScreenMask, &rvi, &n );
		if (vi) {
		    TQPaintDevice::x_appdepth_arr[ screen ] = vi->depth;
		    TQPaintDevice::x_appcells_arr[ screen ] = vi->visual->map_entries;
		    TQPaintDevice::x_appvisual_arr[ screen ] = vi->visual;
		    TQPaintDevice::x_appdefvisual_arr[ screen ] = false;
		    XFree(vi);
		} else {
		    // couldn't get info about the visual, use the default instead
		    vis = 0;
		}
	    }

	    if (!vis) {
		// use the default visual
		vis = DefaultVisual(appDpy, screen);
		TQPaintDevice::x_appdefvisual_arr[ screen ] = true;

		if ( tqt_visual_option == TrueColor ||
		     TQApplication::colorSpec() == TQApplication::ManyColor ) {
		    // find custom visual

		    int d, c;
		    vis = find_truecolor_visual( appDpy, screen, &d, &c );
		    TQPaintDevice::x_appdepth_arr[ screen ] = d;
		    TQPaintDevice::x_appcells_arr[ screen ] = c;

		    TQPaintDevice::x_appvisual_arr[ screen ] = vis;
		    TQPaintDevice::x_appdefvisual_arr[ screen ] =
			(XVisualIDFromVisual(vis) ==
			 XVisualIDFromVisual(DefaultVisual(appDpy, screen)));
		}

		TQPaintDevice::x_appvisual_arr[ screen ] = vis;
	    }

	    // we assume that 8bpp == pseudocolor, but this is not
	    // always the case (according to the X server), so we need
	    // to make sure that our internal data is setup in a way
	    // that is compatible with our assumptions
	    if ( vis->c_class == TrueColor &&
		 TQPaintDevice::x_appdepth_arr[ screen ] == 8 &&
		 TQPaintDevice::x_appcells_arr[ screen ] == 8 )
		TQPaintDevice::x_appcells_arr[ screen ] = 256;

	    if ( colormap && screen == appScreen ) {
		// use the provided colormap for the default screen only
		TQPaintDevice::x_appcolormap_arr[ screen ] = colormap;
		TQPaintDevice::x_appdefcolormap_arr[ screen ] = false;
	    } else {
		if ( vis->c_class == TrueColor ) {
		    TQPaintDevice::x_appdefcolormap_arr[ screen ] =
			TQPaintDevice::x_appdefvisual_arr[ screen ];
		} else {
		    TQPaintDevice::x_appdefcolormap_arr[ screen ] =
			!tqt_cmap_option && TQPaintDevice::x_appdefvisual_arr[ screen ];
		}

		if ( TQPaintDevice::x_appdefcolormap_arr[ screen ] ) {
		    // use default colormap
		    XStandardColormap *stdcmap;
		    VisualID vid =
			XVisualIDFromVisual((Visual *)
					    TQPaintDevice::x_appvisual_arr[ screen ]);
		    int i, count;

		    TQPaintDevice::x_appcolormap_arr[ screen ] = DefaultColormap(appDpy, screen);
		} else {
		    // create a custom colormap
		    TQPaintDevice::x_appcolormap_arr[ screen ] =
			XCreateColormap(appDpy, TQPaintDevice::x11AppRootWindow( screen ),
					vis, AllocNone);
		}
	    }
	}

	// Set X paintdevice parameters for the default screen
	TQPaintDevice::x_appdepth = TQPaintDevice::x_appdepth_arr[ appScreen ];
	TQPaintDevice::x_appcells = TQPaintDevice::x_appcells_arr[ appScreen ];
	TQPaintDevice::x_approotwindow = TQPaintDevice::x_approotwindow_arr[ appScreen ];
	TQPaintDevice::x_appcolormap = TQPaintDevice::x_appcolormap_arr[ appScreen ];
	TQPaintDevice::x_appdefcolormap = TQPaintDevice::x_appdefcolormap_arr[ appScreen ];
	TQPaintDevice::x_appvisual = TQPaintDevice::x_appvisual_arr[ appScreen ];
	TQPaintDevice::x_appdefvisual = TQPaintDevice::x_appdefvisual_arr[ appScreen ];

	// Support protocols

	tqt_x11_intern_atom( "WM_PROTOCOLS", &tqt_wm_protocols );
	tqt_x11_intern_atom( "WM_DELETE_WINDOW", &tqt_wm_delete_window );
	tqt_x11_intern_atom( "WM_STATE", &tqt_wm_state );
	tqt_x11_intern_atom( "WM_CHANGE_STATE", &tqt_wm_change_state );
	tqt_x11_intern_atom( "WM_TAKE_FOCUS", &tqt_wm_take_focus );
	tqt_x11_intern_atom( "WM_CLIENT_LEADER", &tqt_wm_client_leader);
	tqt_x11_intern_atom( "WM_WINDOW_ROLE", &tqt_window_role);
	tqt_x11_intern_atom( "SM_CLIENT_ID", &tqt_sm_client_id);
	tqt_x11_intern_atom( "CLIPBOARD", &tqt_xa_clipboard );
	tqt_x11_intern_atom( "RESOURCE_MANAGER", &tqt_resource_manager );
	tqt_x11_intern_atom( "INCR", &tqt_x_incr );
	tqt_x11_intern_atom( "_XSETROOT_ID", &tqt_xsetroot_id );
	tqt_x11_intern_atom( "_QT_SELECTION", &tqt_selection_property );
	tqt_x11_intern_atom( "_QT_CLIPBOARD_SENTINEL", &tqt_clipboard_sentinel );
	tqt_x11_intern_atom( "_QT_SELECTION_SENTINEL", &tqt_selection_sentinel );
	tqt_x11_intern_atom( "_QT_SCROLL_DONE", &tqt_scrolldone );
	tqt_x11_intern_atom( "_QT_INPUT_ENCODING", &tqt_input_encoding );
	tqt_x11_intern_atom( "_QT_SIZEGRIP", &tqt_sizegrip );
	tqt_x11_intern_atom( "_NET_WM_CONTEXT_HELP", &tqt_net_wm_context_help );
	tqt_x11_intern_atom( "_NET_WM_PING", &tqt_net_wm_ping );
	tqt_x11_intern_atom( "_MOTIF_WM_HINTS", &tqt_xa_motif_wm_hints );
	tqt_x11_intern_atom( "DTWM_IS_RUNNING", &tqt_cde_running );
	tqt_x11_intern_atom( "KWIN_RUNNING", &tqt_twin_running );
	tqt_x11_intern_atom( "KWM_RUNNING", &tqt_kwm_running );
	tqt_x11_intern_atom( "GNOME_BACKGROUND_PROPERTIES", &tqt_gbackground_properties );

	TQString atomname("_QT_SETTINGS_TIMESTAMP_");
	atomname += XDisplayName(appDpyName);
	tqt_x11_intern_atom( atomname.latin1(), &tqt_settings_timestamp );

	tqt_x11_intern_atom( "_NET_SUPPORTED", &tqt_net_supported );
	tqt_x11_intern_atom( "_NET_VIRTUAL_ROOTS", &tqt_net_virtual_roots );
	tqt_x11_intern_atom( "_NET_WORKAREA", &tqt_net_workarea );
	tqt_x11_intern_atom( "_NET_WM_STATE", &tqt_net_wm_state );
	tqt_x11_intern_atom( "_NET_WM_STATE_MODAL", &tqt_net_wm_state_modal );
	tqt_x11_intern_atom( "_NET_WM_STATE_MAXIMIZED_VERT", &tqt_net_wm_state_max_v );
	tqt_x11_intern_atom( "_NET_WM_STATE_MAXIMIZED_HORZ", &tqt_net_wm_state_max_h );
	tqt_x11_intern_atom( "_NET_WM_STATE_FULLSCREEN", &tqt_net_wm_state_fullscreen );
	tqt_x11_intern_atom( "_NET_WM_STATE_ABOVE", &tqt_net_wm_state_above );
	tqt_x11_intern_atom( "_NET_WM_ALLOWED_ACTIONS", &tqt_net_wm_action );
	tqt_x11_intern_atom( "_NET_WM_ACTION_MOVE", &tqt_net_wm_action_move );
	tqt_x11_intern_atom( "_NET_WM_ACTION_RESIZE", &tqt_net_wm_action_resize );
	tqt_x11_intern_atom( "_NET_WM_ACTION_MINIMIZE", &tqt_net_wm_action_minimize );
	tqt_x11_intern_atom( "_NET_WM_ACTION_SHADE", &tqt_net_wm_action_shade );
	tqt_x11_intern_atom( "_NET_WM_ACTION_STICK", &tqt_net_wm_action_stick );
	tqt_x11_intern_atom( "_NET_WM_ACTION_MAXIMIZE_HORZ", &tqt_net_wm_action_max_h );
	tqt_x11_intern_atom( "_NET_WM_ACTION_MAXIMIZE_VERT", &tqt_net_wm_action_max_v );
	tqt_x11_intern_atom( "_NET_WM_ACTION_FULLSCREEN", &tqt_net_wm_action_fullscreen );
	tqt_x11_intern_atom( "_NET_WM_ACTION_CHANGE_DESKTOP", &tqt_net_wm_action_change_desktop );
	tqt_x11_intern_atom( "_NET_WM_ACTION_CLOSE", &tqt_net_wm_action_close );
	tqt_x11_intern_atom( "_NET_WM_ACTION_ABOVE", &tqt_net_wm_action_above );
	tqt_x11_intern_atom( "_NET_WM_ACTION_BELOW", &tqt_net_wm_action_below );
	tqt_x11_intern_atom( "_NET_WM_WINDOW_TYPE", &tqt_net_wm_window_type );
	tqt_x11_intern_atom( "_NET_WM_WINDOW_TYPE_NORMAL", &tqt_net_wm_window_type_normal );
	tqt_x11_intern_atom( "_NET_WM_WINDOW_TYPE_DIALOG", &tqt_net_wm_window_type_dialog );
	tqt_x11_intern_atom( "_NET_WM_WINDOW_TYPE_TOOLBAR", &tqt_net_wm_window_type_toolbar );
	tqt_x11_intern_atom( "_NET_WM_WINDOW_TYPE_MENU", &tqt_net_wm_window_type_menu );
	tqt_x11_intern_atom( "_NET_WM_WINDOW_TYPE_UTILITY", &tqt_net_wm_window_type_utility );
	tqt_x11_intern_atom( "_NET_WM_WINDOW_TYPE_SPLASH", &tqt_net_wm_window_type_splash );
	tqt_x11_intern_atom( "_KDE_NET_WM_WINDOW_TYPE_OVERRIDE", &tqt_net_wm_window_type_override );
	tqt_x11_intern_atom( "_NET_WM_WINDOW_TYPE_DROPDOWN_MENU", &tqt_net_wm_window_type_dropdown_menu );
	tqt_x11_intern_atom( "_NET_WM_WINDOW_TYPE_POPUP_MENU", &tqt_net_wm_window_type_popup_menu );
	tqt_x11_intern_atom( "_NET_WM_WINDOW_TYPE_TOOLTIP", &tqt_net_wm_window_type_tooltip );
	tqt_x11_intern_atom( "_NET_WM_WINDOW_TYPE_COMBO", &tqt_net_wm_window_type_combo );
	tqt_x11_intern_atom( "_NET_WM_WINDOW_TYPE_DND", &tqt_net_wm_window_type_dnd );
	tqt_x11_intern_atom( "_KDE_NET_WM_FRAME_STRUT", &tqt_net_wm_frame_strut );
	tqt_x11_intern_atom( "_NET_WM_STATE_STAYS_ON_TOP",
			    &tqt_net_wm_state_stays_on_top );
	tqt_x11_intern_atom( "_NET_WM_PID", &tqt_net_wm_pid );
	tqt_x11_intern_atom( "_NET_WM_USER_TIME", &tqt_net_wm_user_time );
	tqt_x11_intern_atom( "_NET_WM_FULL_PLACEMENT", &tqt_net_wm_full_placement );
	tqt_x11_intern_atom( "ENLIGHTENMENT_DESKTOP", &tqt_enlightenment_desktop );
	tqt_x11_intern_atom( "_NET_WM_NAME", &tqt_net_wm_name );
	tqt_x11_intern_atom( "_NET_WM_ICON_NAME", &tqt_net_wm_icon_name );
	tqt_x11_intern_atom( "UTF8_STRING", &tqt_utf8_string );
        tqt_x11_intern_atom( "_SGI_DESKS_MANAGER", &tqt_sgi_desks_manager );

#ifndef TQT_NO_XSYNC
	tqt_x11_intern_atom( "_NET_WM_SYNC_REQUEST_COUNTER", &tqt_net_wm_sync_request_counter );
	tqt_x11_intern_atom( "_NET_WM_SYNC_REQUEST", &tqt_net_wm_sync_request );
#endif

	tqt_xdnd_setup();
	tqt_x11_motifdnd_init();

	// Finally create all atoms
	qt_x11_process_intern_atoms();

        // look for broken window managers
        tqt_detect_broken_window_manager();

	// initialize NET lists
	tqt_get_net_supported();
	tqt_get_net_virtual_roots();

#ifndef TQT_NO_XRANDR
	// See if XRandR is supported on the connected display
	int xrandr_errorbase;
	Q_UNUSED( xrandr_eventbase );
	if ( XRRQueryExtension( appDpy, &xrandr_eventbase, &xrandr_errorbase ) ) {
	    // XRandR is supported
	    tqt_use_xrandr = true;
	}
#endif // TQT_NO_XRANDR

#ifndef TQT_NO_XRENDER
	// See if XRender is supported on the connected display
	int xrender_eventbase, xrender_errorbase;
	if (XRenderQueryExtension(appDpy, &xrender_eventbase, &xrender_errorbase)) {
	    // XRender is supported, let's see if we have a PictFormat for the
	    // default visual
	    XRenderPictFormat *format =
		XRenderFindVisualFormat(appDpy,
					(Visual *) TQPaintDevice::x_appvisual);
	    tqt_use_xrender = (format != 0) && (TQPaintDevice::x_appdepth != 8);
	}
#endif // TQT_NO_XRENDER

#ifndef TQT_NO_XSYNC
	// Try to initialize SYNC extension on the connected display
	int xsync_major, xsync_minor;
	if ( XSyncQueryExtension( appDpy, &xsync_eventbase, &xsync_errorbase ) && 
	     XSyncInitialize( appDpy, &xsync_major,  &xsync_minor ) ) {
	     tqt_use_xsync = true;
	}
#endif 

#ifndef TQT_NO_XKB
	// If XKB is detected, set the GrabsUseXKBState option so input method
	// compositions continue to work (ie. deadkeys)
	unsigned int state = XkbPCF_GrabsUseXKBStateMask;
	(void) XkbSetPerClientControls(appDpy, state, &state);
#endif

#if !defined(TQT_NO_XFTFREETYPE)
	// defined in qfont_x11.cpp
	extern bool tqt_has_xft;
        tqt_has_xft = XftInit(0) && XftInitFtLibrary();

        if (tqt_has_xft) {
            char *dpi_str = XGetDefault(appDpy, "Xft", "dpi");
            if (dpi_str) {
                // use a custom DPI
                char *end = 0;
                int dpi = strtol(dpi_str, &end, 0);
                if (dpi_str != end) {
                    for (int s = 0; s < ScreenCount(appDpy); ++s) {
                        TQPaintDevice::x11SetAppDpiX(dpi, s);
                        TQPaintDevice::x11SetAppDpiY(dpi, s);
                    }
                }
            }
        }
#endif // TQT_NO_XFTFREETYPE

	// look at the modifier mapping, and get the correct masks for alt/meta
	// find the alt/meta masks
	XModifierKeymap *map = XGetModifierMapping(appDpy);
	if (map) {
	    int i, maskIndex = 0, mapIndex = 0;
	    for (maskIndex = 0; maskIndex < 8; maskIndex++) {
		for (i = 0; i < map->max_keypermod; i++) {
		    if (map->modifiermap[mapIndex]) {
			KeySym sym = tqt_x11_keycode_to_keysym(appDpy, map->modifiermap[ mapIndex ]);
			if ( tqt_alt_mask == 0 &&
			     ( sym == XK_Alt_L || sym == XK_Alt_R ) ) {
			    tqt_alt_mask = 1 << maskIndex;
			}
			if ( tqt_meta_mask == 0 &&
			     (sym == XK_Meta_L || sym == XK_Meta_R ) ) {
			    tqt_meta_mask = 1 << maskIndex;
			}
		    }
		    mapIndex++;
		}
	    }

	    // not look for mode_switch in tqt_alt_mask and tqt_meta_mask - if it is
	    // present in one or both, then we set tqt_mode_switch_remove_mask.
	    // see TQETWidget::translateKeyEventInternal for an explanation
	    // of why this is needed
	    mapIndex = 0;
	    for ( maskIndex = 0; maskIndex < 8; maskIndex++ ) {
		if ( tqt_alt_mask  != ( 1 << maskIndex ) &&
		     tqt_meta_mask != ( 1 << maskIndex ) ) {
		    for ( i = 0; i < map->max_keypermod; i++ )
			mapIndex++;
		    continue;
		}

		for ( i = 0; i < map->max_keypermod; i++ ) {
		    if ( map->modifiermap[ mapIndex ] ) {
			KeySym sym = tqt_x11_keycode_to_keysym(appDpy, map->modifiermap[ mapIndex ]);
			if ( sym == XK_Mode_switch ) {
			    tqt_mode_switch_remove_mask |= 1 << maskIndex;
			}
		    }
		    mapIndex++;
		}
	    }

	    XFreeModifiermap(map);
	} else {
	    // assume defaults
	    tqt_alt_mask = Mod1Mask;
	    tqt_meta_mask = Mod4Mask;
	    tqt_mode_switch_remove_mask = 0;
	}

	// Misc. initialization

	TQColor::initialize();
	TQFont::initialize();
	TQCursor::initialize();
	TQPainter::initialize();
    }

#if defined(TQT_THREAD_SUPPORT)
    TQThread::initialize();
#endif

    if( tqt_is_gui_used ) {
	tqApp->setName( appName );

	int screen;
	for ( screen = 0; screen < appScreenCount; ++screen ) {
	    XSelectInput( appDpy, TQPaintDevice::x11AppRootWindow( screen ),
			  KeymapStateMask | EnterWindowMask | LeaveWindowMask |
			  PropertyChangeMask );

#ifndef TQT_NO_XRANDR
	    if (tqt_use_xrandr)
		XRRSelectInput( appDpy, TQPaintDevice::x11AppRootWindow( screen ), True );
#endif // TQT_NO_XRANDR
	}
    }

    if ( tqt_is_gui_used ) {
	tqt_set_input_encoding();

	tqt_set_x11_resources( appFont, appFGCol, appBGCol, appBTNCol);

	// be smart about the size of the default font. most X servers have helvetica
	// 12 point available at 2 resolutions:
	//     75dpi (12 pixels) and 100dpi (17 pixels).
	// At 95 DPI, a 12 point font should be 16 pixels tall - in which case a 17
	// pixel font is a closer match than a 12 pixel font
	int ptsz =
	    (int) ( ( ( TQPaintDevice::x11AppDpiY() >= 95 ? 17. : 12. ) *
		      72. / (float) TQPaintDevice::x11AppDpiY() ) + 0.5 );

	if ( !tqt_app_has_font && !tqt_x11_cmdline_font ) {
	    TQFont f( "Helvetica", ptsz );
	    TQApplication::setFont( f );
	}

#if !defined(TQT_NO_IM)
#if !defined(TQT_NO_IM_EXTENSIONS)
    TQApplication::create_im();
#else
    TQApplication::create_xim();
#endif
#endif

#if defined (QT_TABLET_SUPPORT)
	int ndev,
	    i,
	    j;
	bool gotStylus,
	    gotEraser;
	XDeviceInfo *devices, *devs;
	XInputClassInfo *ip;
	XAnyClassPtr any;
	XValuatorInfoPtr v;
	XAxisInfoPtr a;
	XDevice *dev;
	XEventClass *ev_class;
	int curr_event_count;

	// XFree86 divides a stylus and eraser into 2 devices, so we must do for both...
	const TQString XFREENAMESTYLUS = "stylus";
	const TQString XFREENAMEPEN = "pen";
	const TQString XFREENAMEERASER = "eraser";

	devices = XListInputDevices( appDpy, &ndev);
	if ( devices == NULL ) {
	    tqWarning( "Failed to get list of devices" );
	    ndev = -1;
	}
	dev = NULL;
	for ( devs = devices, i = 0; i < ndev; i++, devs++ ) {
	    gotEraser = false;
	    TQString devName = devs->name;
	    devName = devName.lower();
	    gotStylus = ( devName.startsWith(XFREENAMEPEN)
			  || devName.startsWith(XFREENAMESTYLUS) );
	    if ( !gotStylus )
		gotEraser = devName.startsWith( XFREENAMEERASER );

	    if ( gotStylus || gotEraser ) {
		// I only wanted to do this once, so wrap pointers around these
		curr_event_count = 0;

		if ( gotStylus ) {
		    devStylus = XOpenDevice( appDpy, devs->id );
		    dev = devStylus;
		    ev_class = event_list_stylus;
		} else if ( gotEraser ) {
		    devEraser = XOpenDevice( appDpy, devs->id );
		    dev = devEraser;
		    ev_class = event_list_eraser;
		}
		if ( dev == NULL ) {
		    tqWarning( "Failed to open device" );
		} else {
		    if ( dev->num_classes > 0 ) {
			for ( ip = dev->classes, j = 0; j < devs->num_classes;
			      ip++, j++ ) {
			    switch ( ip->input_class ) {
			    case KeyClass:
				DeviceKeyPress( dev, xinput_key_press,
						ev_class[curr_event_count] );
				curr_event_count++;
				DeviceKeyRelease( dev, xinput_key_release,
						  ev_class[curr_event_count] );
				curr_event_count++;
				break;
			    case ButtonClass:
				DeviceButtonPress( dev, xinput_button_press,
						   ev_class[curr_event_count] );
				curr_event_count++;
				DeviceButtonRelease( dev, xinput_button_release,
						     ev_class[curr_event_count] );
				curr_event_count++;
				break;
			    case ValuatorClass:
				// I'm only going to be interested in motion when the
				// stylus is already down anyway!
				DeviceMotionNotify( dev, xinput_motion,
						    ev_class[curr_event_count] );
				curr_event_count++;
				break;
			    default:
				break;
			    }
			}
		    }
		}
		// get the min/max value for pressure!
		any = (XAnyClassPtr) ( devs->inputclassinfo );
		if ( dev == devStylus ) {
		    tqt_curr_events_stylus = curr_event_count;
		    for (j = 0; j < devs->num_classes; j++) {
			if ( any->c_class == ValuatorClass ) {
			    v = (XValuatorInfoPtr) any;
			    a = (XAxisInfoPtr) ((char *) v +
						sizeof (XValuatorInfo));
			    max_pressure = a[2].max_value;
			    // got the max pressure no need to go further...
			    break;
			}
			any = (XAnyClassPtr) ((char *) any + any->length);
		    }
		} else {
		    tqt_curr_events_eraser = curr_event_count;
		}
		// at this point we are assuming there is only one
		// wacom device...
		    if ( devStylus != NULL && devEraser != NULL ) {
			break;
		    }
		}
	    } // end for loop
	    XFreeDeviceList( devices );
#endif // QT_TABLET_SUPPORT

	} else {
	    // read some non-GUI settings when not using the X server...

	    if ( TQApplication::desktopSettingsAware() ) {
		TQSettings settings;

		// read library (ie. plugin) path list
		TQString libpathkey = TQString("/qt/%1.%2/libraryPath")
				     .arg( TQT_VERSION >> 16 )
				     .arg( (TQT_VERSION & 0xff00 ) >> 8 );
		TQStringList pathlist =
		    settings.readListEntry(libpathkey, ':');
		if (! pathlist.isEmpty()) {
		    TQStringList::ConstIterator it = pathlist.begin();
		    while (it != pathlist.end())
			TQApplication::addLibraryPath(*it++);
		}

		TQString defaultcodec = settings.readEntry("/qt/defaultCodec", "none");
		if (defaultcodec != "none") {
		    TQTextCodec *codec = TQTextCodec::codecForName(defaultcodec);
		    if (codec)
			tqApp->setDefaultCodec(codec);
		}

		tqt_resolve_symlinks =
		    settings.readBoolEntry("/qt/resolveSymlinks", true);
	    }
	}
    }


#ifndef TQT_NO_STYLE
    // run-time search for default style
void TQApplication::x11_initialize_style()
{
    Atom type;
    int format;
    unsigned long length, after;
    uchar *data;
    if ( !app_style &&
	 XGetWindowProperty( appDpy, TQPaintDevice::x11AppRootWindow(), tqt_twin_running,
			     0, 1, False, AnyPropertyType, &type, &format, &length,
			     &after, &data ) == Success && length ) {
	if ( data ) XFree( (char *)data );
	// twin is there. check if KDE's styles are available,
	// otherwise use windows style
	if ( (app_style = TQStyleFactory::create("highcolor") ) == 0 )
	    app_style = TQStyleFactory::create("windows");
    }
    if ( !app_style &&
	 XGetWindowProperty( appDpy, TQPaintDevice::x11AppRootWindow(), tqt_kwm_running,
			     0, 1, False, AnyPropertyType, &type, &format, &length,
			     &after, &data ) == Success && length ) {
	if ( data ) XFree( (char *)data );
	app_style = TQStyleFactory::create("windows");
    }
    if ( !app_style &&
	 XGetWindowProperty( appDpy, TQPaintDevice::x11AppRootWindow(), tqt_cde_running,
			     0, 1, False, AnyPropertyType, &type, &format, &length,
			     &after, &data ) == Success && length ) {
	// DTWM is running, meaning most likely CDE is running...
	if ( data ) XFree( (char *) data );
	app_style = TQStyleFactory::create( "cde" );
    }
    // maybe another desktop?
    if ( !app_style &&
	 XGetWindowProperty( appDpy, TQPaintDevice::x11AppRootWindow(),
			     tqt_gbackground_properties, 0, 1, False, AnyPropertyType,
			     &type, &format, &length, &after, &data ) == Success &&
	 length ) {
	if ( data ) XFree( (char *)data );
	// default to MotifPlus with hovering
	app_style = TQStyleFactory::create("motifplus" );
    }
}
#endif

void tqt_init( int *argcptr, char **argv, TQApplication::Type )
{
    tqt_init_internal( argcptr, argv, 0, 0, 0 );
}

void tqt_init( Display *display, TQt::HANDLE visual, TQt::HANDLE colormap )
{
    tqt_init_internal( 0, 0, display, visual, colormap );
}

void tqt_init( int *argcptr, char **argv, Display *display, TQt::HANDLE visual, TQt::HANDLE colormap )
{
    tqt_init_internal( argcptr, argv, display, visual, colormap );
}


/*****************************************************************************
  tqt_cleanup() - cleans up when the application is finished
 *****************************************************************************/

void tqt_cleanup()
{
    appliedstamp = 0;

    if ( app_save_rootinfo )			// root window must keep state
	tqt_save_rootinfo();

    if ( tqt_is_gui_used ) {
	TQPixmapCache::clear();
	TQPainter::cleanup();
	TQCursor::cleanup();
	TQFont::cleanup();
	TQColor::cleanup();
	TQSharedDoubleBuffer::cleanup();
    }
#if defined(TQT_THREAD_SUPPORT)
    TQThread::cleanup();
#endif

#if defined (QT_TABLET_SUPPORT)
    if ( devStylus != NULL )
	XCloseDevice( appDpy, devStylus );
    if ( devEraser != NULL )
	XCloseDevice( appDpy, devEraser );
#endif

#if !defined(TQT_NO_IM)
#if !defined(TQT_NO_IM_EXTENSIONS)
    TQApplication::close_im();
#else
    TQApplication::close_xim();
#endif
#endif

    if ( tqt_is_gui_used ) {
	int screen;
	for ( screen = 0; screen < appScreenCount; screen++ ) {
	    if ( ! TQPaintDevice::x11AppDefaultColormap( screen ) )
		XFreeColormap( TQPaintDevice::x11AppDisplay(),
			       TQPaintDevice::x11AppColormap( screen ) );
	}
    }

#define QT_CLEANUP_GC(g) if (g) { for (int i=0;i<appScreenCount;i++){if(g[i])XFreeGC(appDpy,g[i]);} delete [] g; g = 0; }
    QT_CLEANUP_GC(app_gc_ro);
    QT_CLEANUP_GC(app_gc_ro_m);
    QT_CLEANUP_GC(app_gc_tmp);
    QT_CLEANUP_GC(app_gc_tmp_m);
#undef QT_CLEANUP_GC

    delete sip_list;
    sip_list = 0;

    // Reset the error handlers
    XSetErrorHandler( original_x_errhandler );
    XSetIOErrorHandler( original_xio_errhandler );

    if ( tqt_is_gui_used && !appForeignDpy )
	XCloseDisplay( appDpy );		// close X display
    appDpy = 0;

    tqt_x11_wm_client_leader = 0;

    if ( TQPaintDevice::x_appdepth_arr )
	delete [] TQPaintDevice::x_appdepth_arr;
    if ( TQPaintDevice::x_appcells_arr )
	delete [] TQPaintDevice::x_appcells_arr;
    if ( TQPaintDevice::x_appcolormap_arr )
	delete []TQPaintDevice::x_appcolormap_arr;
    if ( TQPaintDevice::x_appdefcolormap_arr )
	delete [] TQPaintDevice::x_appdefcolormap_arr;
    if ( TQPaintDevice::x_appvisual_arr )
	delete [] TQPaintDevice::x_appvisual_arr;
    if ( TQPaintDevice::x_appdefvisual_arr )
	delete [] TQPaintDevice::x_appdefvisual_arr;

    if ( appForeignDpy ) {
	delete [] (char *)appName;
	appName = 0;
	delete [] (char *)appClass;
	appClass = 0;
    }

    if (tqt_net_supported_list)
	delete [] tqt_net_supported_list;
    tqt_net_supported_list = 0;

    if (tqt_net_virtual_root_list)
	delete [] tqt_net_virtual_root_list;
    tqt_net_virtual_root_list = 0;
}


/*****************************************************************************
  Platform specific global and internal functions
 *****************************************************************************/

void tqt_save_rootinfo()				// save new root info
{
    Atom type;
    int format;
    unsigned long length, after;
    uchar *data;

    if ( tqt_xsetroot_id ) {			// kill old pixmap
	if ( XGetWindowProperty( appDpy, TQPaintDevice::x11AppRootWindow(),
				 tqt_xsetroot_id, 0, 1,
				 True, AnyPropertyType, &type, &format,
				 &length, &after, &data ) == Success ) {
	    if ( type == XA_PIXMAP && format == 32 && length == 1 &&
		 after == 0 && data ) {
		XKillClient( appDpy, *((Pixmap*)data) );
	    }
	    Pixmap dummy = XCreatePixmap( appDpy, TQPaintDevice::x11AppRootWindow(),
					  1, 1, 1 );
	    XChangeProperty( appDpy, TQPaintDevice::x11AppRootWindow(),
			     tqt_xsetroot_id, XA_PIXMAP, 32,
			     PropModeReplace, (uchar *)&dummy, 1 );
	    XSetCloseDownMode( appDpy, RetainPermanent );
	}
    }
    if ( data )
	XFree( (char *)data );
}

void tqt_updated_rootinfo()
{
    app_save_rootinfo = true;
}

bool tqt_wstate_iconified( WId winid )
{
    Atom type;
    int format;
    unsigned long length, after;
    uchar *data;
    int r = XGetWindowProperty( appDpy, winid, tqt_wm_state, 0, 2,
				 False, AnyPropertyType, &type, &format,
				 &length, &after, &data );
    bool iconic = false;
    if ( r == Success && data && format == 32 ) {
	// TQ_UINT32 *wstate = (TQ_UINT32*)data;
	unsigned long *wstate = (unsigned long *) data;
	iconic = (*wstate == IconicState );
	XFree( (char *)data );
    }
    return iconic;
}

const char *tqAppName()				// get application name
{
    return appName;
}

const char *tqAppClass()				// get application class
{
    return appClass;
}

Display *tqt_xdisplay()				// get current X display
{
    return appDpy;
}

int tqt_xscreen()				// get current X screen
{
    return appScreen;
}

// ### REMOVE 4.0
WId tqt_xrootwin()				// get X root window
{
    return TQPaintDevice::x11AppRootWindow();
}

WId tqt_xrootwin( int scrn )			// get X root window for screen
{
    return TQPaintDevice::x11AppRootWindow( scrn );
}

bool tqt_nograb()				// application no-grab option
{
#if defined(QT_DEBUG)
    return appNoGrab;
#else
    return false;
#endif
}

static GC create_gc( int scrn, bool monochrome )
{
    GC gc;
    if ( monochrome ) {
	Pixmap pm = XCreatePixmap( appDpy, RootWindow( appDpy, scrn ), 8, 8, 1 );
	gc = XCreateGC( appDpy, pm, 0, 0 );
	XFreePixmap( appDpy, pm );
    } else {
	if ( TQPaintDevice::x11AppDefaultVisual( scrn ) ) {
	    gc = XCreateGC( appDpy, RootWindow( appDpy, scrn ), 0, 0 );
	} else {
	    Window w;
	    XSetWindowAttributes a;
	    a.background_pixel = TQt::black.pixel( scrn );
	    a.border_pixel = TQt::black.pixel( scrn );
	    a.colormap = TQPaintDevice::x11AppColormap( scrn );
	    w = XCreateWindow( appDpy, RootWindow( appDpy, scrn ), 0, 0, 100, 100,
			       0, TQPaintDevice::x11AppDepth( scrn ), InputOutput,
			       (Visual*)TQPaintDevice::x11AppVisual( scrn ),
			       CWBackPixel|CWBorderPixel|CWColormap, &a );
	    gc = XCreateGC( appDpy, w, 0, 0 );
	    XDestroyWindow( appDpy, w );
	}
    }
    XSetGraphicsExposures( appDpy, gc, False );
    return gc;
}

GC tqt_xget_readonly_gc( int scrn, bool monochrome )	// get read-only GC
{
    if ( scrn < 0 || scrn >= appScreenCount ) {
	tqFatal("invalid screen %d %d", scrn, appScreenCount );
    }
    GC gc;
    if ( monochrome ) {
	if ( !app_gc_ro_m )			// create GC for bitmap
	    memset( (app_gc_ro_m = new GC[appScreenCount]), 0, appScreenCount * sizeof( GC ) );
	if ( !app_gc_ro_m[scrn] )
	    app_gc_ro_m[scrn] = create_gc( scrn, true );
	gc = app_gc_ro_m[scrn];
    } else {					// create standard GC
	if ( !app_gc_ro )
	    memset( (app_gc_ro = new GC[appScreenCount]), 0, appScreenCount * sizeof( GC ) );
	if ( !app_gc_ro[scrn] )
	    app_gc_ro[scrn] = create_gc( scrn, false );
	gc = app_gc_ro[scrn];
    }
    return gc;
}

GC tqt_xget_temp_gc( int scrn, bool monochrome )		// get temporary GC
{
    if ( scrn < 0 || scrn >= appScreenCount ) {
	tqFatal("invalid screen (tmp) %d %d", scrn, appScreenCount );
    }
    GC gc;
    if ( monochrome ) {
	if ( !app_gc_tmp_m )			// create GC for bitmap
	    memset( (app_gc_tmp_m = new GC[appScreenCount]), 0, appScreenCount * sizeof( GC ) );
	if ( !app_gc_tmp_m[scrn] )
	    app_gc_tmp_m[scrn] = create_gc( scrn, true );
	gc = app_gc_tmp_m[scrn];
    } else {					// create standard GC
	if ( !app_gc_tmp )
	    memset( (app_gc_tmp = new GC[appScreenCount]), 0, appScreenCount * sizeof( GC ) );
	if ( !app_gc_tmp[scrn] )
	    app_gc_tmp[scrn] = create_gc( scrn, false );
	gc = app_gc_tmp[scrn];
    }
    return gc;
}


/*****************************************************************************
  Platform specific TQApplication members
 *****************************************************************************/

/*!
    \fn TQWidget *TQApplication::mainWidget() const

    Returns the main application widget, or 0 if there is no main
    widget.

    \sa setMainWidget()
*/

/*!
    Sets the application's main widget to \a mainWidget.

    In most respects the main widget is like any other widget, except
    that if it is closed, the application exits. Note that
    TQApplication does \e not take ownership of the \a mainWidget, so
    if you create your main widget on the heap you must delete it
    yourself.

    You need not have a main widget; connecting lastWindowClosed() to
    quit() is an alternative.

    For X11, this function also resizes and moves the main widget
    according to the \e -geometry command-line option, so you should
    set the default geometry (using \l TQWidget::setGeometry()) before
    calling setMainWidget().

    \sa mainWidget(), exec(), quit()
*/

void TQApplication::setMainWidget( TQWidget *mainWidget )
{
#if defined(QT_CHECK_STATE)
    if ( mainWidget && mainWidget->parentWidget() &&
	 ! mainWidget->parentWidget()->isDesktop() )
	tqWarning( "TQApplication::setMainWidget(): New main widget (%s/%s) "
		  "has a parent!",
		  mainWidget->className(), mainWidget->name() );
#endif
    main_widget = mainWidget;
    if ( main_widget ) {			// give WM command line
	XSetWMProperties( main_widget->x11Display(), main_widget->winId(),
			  0, 0, app_argv, app_argc, 0, 0, 0 );
	if ( mwTitle )
	    XStoreName( main_widget->x11Display(), main_widget->winId(), (char*)mwTitle );
	if ( mwGeometry ) {			// parse geometry
	    int x, y;
	    int w, h;
	    int m = XParseGeometry( (char*)mwGeometry, &x, &y, (uint*)&w, (uint*)&h );
	    TQSize minSize = main_widget->minimumSize();
	    TQSize maxSize = main_widget->maximumSize();
	    if ( (m & XValue) == 0 )
		x = main_widget->geometry().x();
	    if ( (m & YValue) == 0 )
		y = main_widget->geometry().y();
	    if ( (m & WidthValue) == 0 )
		w = main_widget->width();
	    if ( (m & HeightValue) == 0 )
		h = main_widget->height();
	    w = TQMIN(w,maxSize.width());
	    h = TQMIN(h,maxSize.height());
	    w = TQMAX(w,minSize.width());
	    h = TQMAX(h,minSize.height());
	    if ( (m & XNegative) ) {
		x = desktop()->width()  + x - w;
		tqt_widget_tlw_gravity = NorthEastGravity;
	    }
	    if ( (m & YNegative) ) {
		y = desktop()->height() + y - h;
		tqt_widget_tlw_gravity = (m & XNegative) ? SouthEastGravity : SouthWestGravity;
	    }
	    main_widget->setGeometry( x, y, w, h );
	}
    }
}

#ifndef TQT_NO_CURSOR

/*****************************************************************************
  TQApplication cursor stack
 *****************************************************************************/

extern void tqt_x11_enforce_cursor( TQWidget * w );

typedef TQPtrList<TQCursor> TQCursorList;

static TQCursorList *cursorStack = 0;

/*!
    \fn TQCursor *TQApplication::overrideCursor()

    Returns the active application override cursor.

    This function returns 0 if no application cursor has been defined
    (i.e. the internal cursor stack is empty).

    \sa setOverrideCursor(), restoreOverrideCursor()
*/

/*!
    Sets the application override cursor to \a cursor.

    Application override cursors are intended for showing the user
    that the application is in a special state, for example during an
    operation that might take some time.

    This cursor will be displayed in all the application's widgets
    until restoreOverrideCursor() or another setOverrideCursor() is
    called.

    Application cursors are stored on an internal stack.
    setOverrideCursor() pushes the cursor onto the stack, and
    restoreOverrideCursor() pops the active cursor off the stack.
    Every setOverrideCursor() must eventually be followed by a
    corresponding restoreOverrideCursor(), otherwise the stack will
    never be emptied.

    If \a replace is true, the new cursor will replace the last
    override cursor (the stack keeps its depth). If \a replace is
    false, the new stack is pushed onto the top of the stack.

    Example:
    \code
	TQApplication::setOverrideCursor( TQCursor(TQt::WaitCursor) );
	calculateHugeMandelbrot();              // lunch time...
	TQApplication::restoreOverrideCursor();
    \endcode

    \sa overrideCursor(), restoreOverrideCursor(), TQWidget::setCursor()
*/

void TQApplication::setOverrideCursor( const TQCursor &cursor, bool replace )
{
    if ( !cursorStack ) {
	cursorStack = new TQCursorList;
	TQ_CHECK_PTR( cursorStack );
	cursorStack->setAutoDelete( true );
    }
    app_cursor = new TQCursor( cursor );
    TQ_CHECK_PTR( app_cursor );
    if ( replace )
	cursorStack->removeLast();
    cursorStack->append( app_cursor );

    TQWidgetIntDictIt it( *((TQWidgetIntDict*)TQWidget::mapper) );
    TQWidget *w;
    while ( (w=it.current()) ) {		// for all widgets that have
	if ( w->testWState( WState_OwnCursor ) )
	    tqt_x11_enforce_cursor( w );
	++it;
    }
    XFlush( appDpy );				// make X execute it NOW
}

/*!
    Undoes the last setOverrideCursor().

    If setOverrideCursor() has been called twice, calling
    restoreOverrideCursor() will activate the first cursor set.
    Calling this function a second time restores the original widgets'
    cursors.

    \sa setOverrideCursor(), overrideCursor().
*/

void TQApplication::restoreOverrideCursor()
{
    if ( !cursorStack )				// no cursor stack
	return;
    cursorStack->removeLast();
    app_cursor = cursorStack->last();
    if ( TQWidget::mapper != 0 && !closingDown() ) {
	TQWidgetIntDictIt it( *((TQWidgetIntDict*)TQWidget::mapper) );
	TQWidget *w;
	while ( (w=it.current()) ) {		// set back to original cursors
	    if ( w->testWState( WState_OwnCursor ) )
		tqt_x11_enforce_cursor( w );
	    ++it;
	}
	XFlush( appDpy );
    }
    if ( !app_cursor ) {
	delete cursorStack;
	cursorStack = 0;
    }
}

#endif

/*!
    \fn bool TQApplication::hasGlobalMouseTracking()

    Returns true if global mouse tracking is enabled; otherwise
    returns false.

    \sa setGlobalMouseTracking()
*/

/*!
    Enables global mouse tracking if \a enable is true, or disables it
    if \a enable is false.

    Enabling global mouse tracking makes it possible for widget event
    filters or application event filters to get all mouse move events,
    even when no button is depressed. This is useful for special GUI
    elements, e.g. tooltips.

    Global mouse tracking does not affect widgets and their
    mouseMoveEvent(). For a widget to get mouse move events when no
    button is depressed, it must do TQWidget::setMouseTracking(true).

    This function uses an internal counter. Each
    setGlobalMouseTracking(true) must have a corresponding
    setGlobalMouseTracking(false):
    \code
	// at this point global mouse tracking is off
	TQApplication::setGlobalMouseTracking( true );
	TQApplication::setGlobalMouseTracking( true );
	TQApplication::setGlobalMouseTracking( false );
	// at this point it's still on
	TQApplication::setGlobalMouseTracking( false );
	// but now it's off
    \endcode

    \sa hasGlobalMouseTracking(), TQWidget::hasMouseTracking()
*/

void TQApplication::setGlobalMouseTracking( bool enable )
{
    bool tellAllWidgets;
    if ( enable ) {
	tellAllWidgets = (++app_tracking == 1);
    } else {
	tellAllWidgets = (--app_tracking == 0);
    }
    if ( tellAllWidgets ) {
	TQWidgetIntDictIt it( *((TQWidgetIntDict*)TQWidget::mapper) );
	TQWidget *w;
	while ( (w=it.current()) ) {
	    if ( app_tracking > 0 ) {		// switch on
		if ( !w->testWState(WState_MouseTracking) ) {
		    w->setMouseTracking( true );
		    w->clearWState( WState_MouseTracking );
		}
	    } else {				// switch off
		if ( !w->testWState(WState_MouseTracking) ) {
		    w->setWState( WState_MouseTracking );
		    w->setMouseTracking( false );
		}
	    }
	    ++it;
	}
    }
}


/*****************************************************************************
  Routines to find a TQt widget from a screen position
 *****************************************************************************/

Window tqt_x11_findClientWindow( Window win, Atom property, bool leaf )
{
    Atom   type = None;
    int	   format, i;
    ulong  nitems, after;
    uchar *data;
    Window root, parent, target=0, *children=0;
    uint   nchildren;
    if ( XGetWindowProperty( appDpy, win, property, 0, 0, false, AnyPropertyType,
			     &type, &format, &nitems, &after, &data ) == Success ) {
	if ( data )
	    XFree( (char *)data );
	if ( type )
	    return win;
    }
    if ( !XQueryTree(appDpy,win,&root,&parent,&children,&nchildren) ) {
	if ( children )
	    XFree( (char *)children );
	return 0;
    }
    for ( i=nchildren-1; !target && i >= 0; i-- )
	target = tqt_x11_findClientWindow( children[i], property, leaf );
    if ( children )
	XFree( (char *)children );
    return target;
}


/*!
    Returns a pointer to the widget at global screen position \a
    (x, y), or 0 if there is no TQt widget there.

    If \a child is false and there is a child widget at position \a
    (x, y), the top-level widget containing it is returned. If \a child
    is true the child widget at position \a (x, y) is returned.

    This function is normally rather slow.

    \sa TQCursor::pos(), TQWidget::grabMouse(), TQWidget::grabKeyboard()
*/

TQWidget *TQApplication::widgetAt( int x, int y, bool child )
{
    int screen = TQCursor::x11Screen();
    int lx, ly;

    Window target;
    if ( !XTranslateCoordinates(appDpy,
				TQPaintDevice::x11AppRootWindow(screen),
				TQPaintDevice::x11AppRootWindow(screen),
				x, y, &lx, &ly, &target) ) {
	return 0;
    }
    if ( !target || target == TQPaintDevice::x11AppRootWindow(screen) )
	return 0;
    TQWidget *w, *c;
    w = TQWidget::find( (WId)target );

    if ( !w ) {
	tqt_ignore_badwindow();
	target = tqt_x11_findClientWindow( target, tqt_wm_state, true );
	if (tqt_badwindow() )
	    return 0;
	w = TQWidget::find( (WId)target );
#if 0
	if ( !w ) {
	    // Perhaps the widgets at (x,y) is inside a foreign application?
	    // Search all toplevel widgets to see if one is within target
	    TQWidgetList *list   = topLevelWidgets();
	    TQWidget     *widget = list->first();
	    while ( widget && !w ) {
		Window	ctarget = target;
		if ( widget->isVisible() && !widget->isDesktop() ) {
		    Window wid = widget->winId();
		    while ( ctarget && !w ) {
			XTranslateCoordinates(appDpy,
					      TQPaintDevice::x11AppRootWindow(screen),
					      ctarget, x, y, &lx, &ly, &ctarget);
			if ( ctarget == wid ) {
			    // Found
			    w = widget;
			    XTranslateCoordinates(appDpy,
						  TQPaintDevice::x11AppRootWindow(screen),
						  ctarget, x, y, &lx, &ly, &ctarget);
			}
		    }
		}
		widget = list->next();
	    }
	    delete list;
	}
#endif
    }
    if ( child && w ) {
	if ( (c = w->childAt( w->mapFromGlobal(TQPoint(x, y ) ) ) ) )
	    return c;
    }
    return w;
}

/*!
    \overload TQWidget *TQApplication::widgetAt( const TQPoint &pos, bool child )

    Returns a pointer to the widget at global screen position \a pos,
    or 0 if there is no TQt widget there.

    If \a child is false and there is a child widget at position \a
    pos, the top-level widget containing it is returned. If \a child
    is true the child widget at position \a pos is returned.
*/


/*!
    Flushes the X event queue in the X11 implementation. This normally
    returns almost immediately. Does nothing on other platforms.

    \sa syncX()
*/

void TQApplication::flushX()
{
    if ( appDpy )
	XFlush( appDpy );
}

/*!
    Flushes the window system specific event queues.

    If you are doing graphical changes inside a loop that does not
    return to the event loop on asynchronous window systems like X11
    or double buffered window systems like MacOS X, and you want to
    visualize these changes immediately (e.g. Splash Screens), call
    this function.

    \sa flushX() sendPostedEvents() TQPainter::flush()
*/

void TQApplication::flush()
{
    flushX();
}

/*!
    Synchronizes with the X server in the X11 implementation. This
    normally takes some time. Does nothing on other platforms.

    \sa flushX()
*/

void TQApplication::syncX()
{
    if ( appDpy )
	XSync( appDpy, False );			// don't discard events
}


/*!
    Sounds the bell, using the default volume and sound.
*/

void TQApplication::beep()
{
    if ( appDpy )
	XBell( appDpy, 0 );
}



/*****************************************************************************
  Special lookup functions for windows that have been reparented recently
 *****************************************************************************/

static TQWidgetIntDict *wPRmapper = 0;		// alternative widget mapper

void qPRCreate( const TQWidget *widget, Window oldwin )
{						// TQWidget::reparent mechanism
    if ( !wPRmapper ) {
	wPRmapper = new TQWidgetIntDict;
	TQ_CHECK_PTR( wPRmapper );
    }
    wPRmapper->insert( (long)oldwin, widget );	// add old window to mapper
    TQETWidget *w = (TQETWidget *)widget;
    w->setWState( TQt::WState_Reparented );	// set reparented flag
}

void qPRCleanup( TQWidget *widget )
{
    TQETWidget *etw = (TQETWidget *)widget;
    if ( !(wPRmapper && etw->testWState(TQt::WState_Reparented)) )
	return;					// not a reparented widget
    TQWidgetIntDictIt it(*wPRmapper);
    TQWidget *w;
    while ( (w=it.current()) ) {
	int key = it.currentKey();
	++it;
	if ( w == etw ) {                       // found widget
	    etw->clearWState( TQt::WState_Reparented ); // clear flag
	    wPRmapper->remove( key );// old window no longer needed
	    if ( wPRmapper->count() == 0 ) {	// became empty
		delete wPRmapper;		// then reset alt mapper
		wPRmapper = 0;
		return;
	    }
	}
    }
}

static TQETWidget *qPRFindWidget( Window oldwin )
{
    return wPRmapper ? (TQETWidget*)wPRmapper->find((long)oldwin) : 0;
}

/*!
    \internal
*/
int TQApplication::x11ClientMessage(TQWidget* w, XEvent* event, bool passive_only)
{
    TQETWidget *widget = (TQETWidget*)w;
    if ( event->xclient.format == 32 && event->xclient.message_type ) {
	if ( event->xclient.message_type == tqt_wm_protocols ) {
	    Atom a = event->xclient.data.l[0];
	    if ( a == tqt_wm_delete_window ) {
		if ( passive_only ) return 0;
		widget->translateCloseEvent(event);
	    }
	    else if ( a == tqt_wm_take_focus ) {
		TQWidget * amw = activeModalWidget();
		if ( (ulong) event->xclient.data.l[1] > tqt_x_time )
		    tqt_x_time = event->xclient.data.l[1];
		if ( amw && amw != widget ) {
		    TQWidget* groupLeader = widget;
		    while ( groupLeader && !groupLeader->testWFlags( TQt::WGroupLeader )
                            && groupLeader != amw )
			groupLeader = groupLeader->parentWidget();
		    if ( !groupLeader ) {
                        TQWidget *p = amw->parentWidget();
                        while (p && p != widget)
                            p = p->parentWidget();
                        if (!p || !tqt_net_supported_list)
			    amw->raise(); // help broken window managers
			amw->setActiveWindow();
		    }
		}
#ifndef TQT_NO_WHATSTHIS
	    } else if ( a == tqt_net_wm_context_help ) {
		TQWhatsThis::enterWhatsThisMode();
#endif // TQT_NO_WHATSTHIS
	    } else if ( a == tqt_net_wm_ping ) {
		// avoid send/reply loops
		Window root = TQPaintDevice::x11AppRootWindow( w->x11Screen() );
		if (event->xclient.window != root) {
		    event->xclient.window = root;
		    XSendEvent( event->xclient.display, event->xclient.window,
				False, SubstructureNotifyMask|SubstructureRedirectMask, event );
		}
#ifndef TQT_NO_XSYNC
	    } else if (a == tqt_net_wm_sync_request ) {
		    widget->handleSyncRequest( event );
#endif
	    }
	} else if ( event->xclient.message_type == tqt_scrolldone ) {
	    widget->translateScrollDoneEvent(event);
	} else if ( event->xclient.message_type == tqt_xdnd_position ) {
	    tqt_handle_xdnd_position( widget, event, passive_only );
	} else if ( event->xclient.message_type == tqt_xdnd_enter ) {
	    tqt_handle_xdnd_enter( widget, event, passive_only );
	} else if ( event->xclient.message_type == tqt_xdnd_status ) {
	    tqt_handle_xdnd_status( widget, event, passive_only );
	} else if ( event->xclient.message_type == tqt_xdnd_leave ) {
	    tqt_handle_xdnd_leave( widget, event, passive_only );
	} else if ( event->xclient.message_type == tqt_xdnd_drop ) {
	    tqt_handle_xdnd_drop( widget, event, passive_only );
	} else if ( event->xclient.message_type == tqt_xdnd_finished ) {
	    tqt_handle_xdnd_finished( widget, event, passive_only );
	} else {
	    if ( passive_only ) return 0;
	    // All other are interactions
	}
    } else {
	tqt_motifdnd_handle_msg( widget, event, passive_only );
    }

    return 0;
}

/*!
    This function does the core processing of individual X
    \a{event}s, normally by dispatching TQt events to the right
    destination.

    It returns 1 if the event was consumed by special handling, 0 if
    the \a event was consumed by normal handling, and -1 if the \a
    event was for an unrecognized widget.

    \sa x11EventFilter()
*/
int TQApplication::x11ProcessEvent( XEvent* event )
{
    switch ( event->type ) {
    case ButtonPress:
	ignoreNextMouseReleaseEvent = false;
	tqt_x_user_time = event->xbutton.time;
	// fallthrough intended
    case ButtonRelease:
	tqt_x_time = event->xbutton.time;
	break;
    case MotionNotify:
	tqt_x_time = event->xmotion.time;
	break;
    case XKeyPress:
	tqt_x_user_time = event->xkey.time;
	// fallthrough intended
    case XKeyRelease:
	tqt_x_time = event->xkey.time;
	break;
    case PropertyNotify:
	tqt_x_time = event->xproperty.time;
	break;
    case EnterNotify:
    case LeaveNotify:
	tqt_x_time = event->xcrossing.time;
	break;
    case SelectionClear:
	tqt_x_time = event->xselectionclear.time;
	break;
    default:
	break;
    }

    TQETWidget *widget = (TQETWidget*)TQWidget::find( (WId)event->xany.window );

    if ( wPRmapper ) {				// just did a widget reparent?
	if ( widget == 0 ) {			// not in std widget mapper
	    switch ( event->type ) {		// only for mouse/key events
	    case ButtonPress:
	    case ButtonRelease:
	    case MotionNotify:
	    case XKeyPress:
	    case XKeyRelease:
		widget = qPRFindWidget( event->xany.window );
		break;
	    }
	}
	else if ( widget->testWState(WState_Reparented) )
	    qPRCleanup( widget );		// remove from alt mapper
    }

    TQETWidget *keywidget=0;
    bool grabbed=false;
    if ( event->type==XKeyPress || event->type==XKeyRelease ) {
	keywidget = (TQETWidget*)TQWidget::keyboardGrabber();
	if ( keywidget ) {
	    grabbed = true;
	} else {
	    if ( focus_widget )
		keywidget = (TQETWidget*)focus_widget;
	    if ( !keywidget ) {
		if ( inPopupMode() ) // no focus widget, see if we have a popup
		    keywidget = (TQETWidget*) activePopupWidget();
		else if ( widget )
		    keywidget = (TQETWidget*)widget->topLevelWidget();
	    }
	}
    }

#ifndef TQT_NO_IM
    // Filtering input events by the input context. It has to be taken
    // place before any other key event consumers such as eventfilters
    // and accelerators because some input methods require quite
    // various key combination and sequences. It often conflicts with
    // accelerators and so on, so we must give the input context the
    // filtering opportunity first to ensure all input methods work
    // properly regardless of application design.

// #ifndef TQT_NO_IM_EXTENSIONS
    if( keywidget && keywidget->isEnabled() && keywidget->isInputMethodEnabled() ) {
// #else
//    if( keywidget && keywidget->isEnabled() ) {
// #endif
	if( ( event->type==XKeyPress || event->type==XKeyRelease ) &&
	    sm_blockUserInput ) // block user interaction during session management
	    return true;

        // for XIM handling
	TQInputContext *qic = keywidget->getInputContext();
	if( qic && qic->x11FilterEvent( keywidget, event ) )
	    return true;

	// filterEvent() accepts TQEvent *event rather than preexpanded key
	// event attribute values. This is intended to pass other IM-related
	// events in future. The IM-related events are supposed as
	// TQWheelEvent, TQTabletEvent and so on. Other non IM-related events
	// should not be forwarded to input contexts to prevent weird event
	// handling.
	if ( ( event->type == XKeyPress || event->type == XKeyRelease ) ) {
	    int code = -1;
	    int count = 0;
	    int state;
	    char ascii = 0;
	    TQEvent::Type type;
	    TQString text;

	    keywidget->translateKeyEventInternal( event, count, text,
						  state, ascii, code, type,
						  false, false );

	    // both key press/release is required for some complex
	    // input methods. don't eliminate anything.
	    TQKeyEvent keyevent( type, code, ascii, state, text, false, count );

	    if( qic && qic->filterEvent( &keyevent ) )
		return true;
	}
    } else
#endif // TQT_NO_IM
    {
	if ( XFilterEvent( event, None ) )
	    return true;
    }

    if ( tqt_x11EventFilter(event) )		// send through app filter
	return 1;

    if ( event->type == MappingNotify ) {	// keyboard mapping changed
	XRefreshKeyboardMapping( &event->xmapping );
	return 0;
    }

    if ( event->type == PropertyNotify ) {	// some properties changed
	if ( event->xproperty.window == TQPaintDevice::x11AppRootWindow( 0 ) ) {
	    // root properties for the first screen
	    if ( event->xproperty.atom == tqt_clipboard_sentinel ) {
		if (tqt_check_clipboard_sentinel() )
		    emit clipboard()->dataChanged();
	    } else if ( event->xproperty.atom == tqt_selection_sentinel ) {
		if (tqt_check_selection_sentinel() )
		    emit clipboard()->selectionChanged();
	    } else if ( obey_desktop_settings ) {
		if ( event->xproperty.atom == tqt_resource_manager )
		    tqt_set_x11_resources();
		else if ( event->xproperty.atom == tqt_settings_timestamp )
		    TQApplication::x11_apply_settings();
	    }
	}
	if ( event->xproperty.window == TQPaintDevice::x11AppRootWindow() ) {
	    // root properties for the default screen
	    if ( event->xproperty.atom == tqt_input_encoding ) {
		tqt_set_input_encoding();
	    } else if ( event->xproperty.atom == tqt_net_supported ) {
		tqt_get_net_supported();
	    } else if ( event->xproperty.atom == tqt_net_virtual_roots ) {
		tqt_get_net_virtual_roots();
	    } else if ( event->xproperty.atom == tqt_net_workarea ) {
		tqt_desktopwidget_update_workarea();
	    }
	} else if ( widget ) {
	    widget->translatePropertyEvent(event);
	}  else {
	    return -1; // don't know this window
	}
	return 0;
    }

#ifndef TQT_NO_XRANDR
    // XRandR doesn't care if TQt doesn't know about the widget, so handle XRandR stuff before the !widget check below
    if (event->type == xrandr_eventbase + RRScreenChangeNotify
	|| ( event->type == ConfigureNotify && event->xconfigure.window == TQPaintDevice::x11AppRootWindow())) {
	// update Xlib internals with the latest screen configuration
	XRRUpdateConfiguration(event);

	// update the size for desktop widget
	int scr = XRRRootToScreen( appDpy, event->xany.window );
	TQWidget *w = desktop()->screen( scr );

	if (w) {
	    int widgetScr = -1;
	    // make sure the specified widget is on the same screen that received the XRandR event
	    XWindowAttributes widgetAttr;
	    XGetWindowAttributes(appDpy, w->winId(), &widgetAttr);
	    if (widgetAttr.screen) {
		widgetScr = XScreenNumberOfScreen(widgetAttr.screen);
	    }

	    if ((widgetScr < 0) || (widgetScr == scr)) {
		TQSize oldSize( w->size() );
		w->crect.setWidth( DisplayWidth( appDpy, scr ) );
		w->crect.setHeight( DisplayHeight( appDpy, scr ) );
		if ( w->size() != oldSize ) {
		TQResizeEvent e( w->size(), oldSize );
		TQApplication::sendEvent( w, &e );
		    emit desktop()->resized( scr );
		}
	    }
	}
    }
#endif // TQT_NO_XRANDR

    if ( !widget ) {				// don't know this windows
	TQWidget* popup = TQApplication::activePopupWidget();
	if ( popup ) {

	    /*
	      That is more than suboptimal. The real solution should
	      do some keyevent and buttonevent translation, so that
	      the popup still continues to work as the user expects.
	      Unfortunately this translation is currently only
	      possible with a known widget. I'll change that soon
	      (Matthias).
	    */

	    // Danger - make sure we don't lock the server
	    switch ( event->type ) {
	    case ButtonPress:
	    case ButtonRelease:
	    case XKeyPress:
	    case XKeyRelease:
		do {
		    popup->close();
		} while ( (popup = tqApp->activePopupWidget()) );
		return 1;
	    }
	}
	return -1;
    }

    if ( event->type == XKeyPress || event->type == XKeyRelease )
	widget = keywidget; // send XKeyEvents through keywidget->x11Event()

    if ( app_do_modal )				// modal event handling
	if ( !tqt_try_modal(widget, event) ) {
	    if ( event->type == ClientMessage )
		x11ClientMessage( widget, event, true );
	    return 1;
	}


    if ( widget->x11Event(event) )		// send through widget filter
	return 1;
#if defined (QT_TABLET_SUPPORT)
    if ( event->type == xinput_motion ||
	 event->type == xinput_button_release ||
	 event->type == xinput_button_press ) {
	widget->translateXinputEvent( event );
	return 0;
    }
#endif

    switch ( event->type ) {

    case ButtonRelease:			// mouse event
	if ( ignoreNextMouseReleaseEvent ) {
	    ignoreNextMouseReleaseEvent = false;
	    break;
	}
	// fall through intended
    case ButtonPress:
	if (event->xbutton.root != RootWindow(widget->x11Display(), widget->x11Screen())
	    && ! tqt_xdnd_dragging) {
	    while ( activePopupWidget() )
		activePopupWidget()->close();
	    return 1;
	}
	if (event->type == ButtonPress)
	    tqt_net_update_user_time(widget->topLevelWidget());
	// fall through intended
    case MotionNotify:
#if defined(QT_TABLET_SUPPORT)
	if ( !chokeMouse ) {
#endif
	    widget->translateMouseEvent( event );
#if defined(QT_TABLET_SUPPORT)
	} else {
	    chokeMouse = false;
	}
#endif
	break;

    case XKeyPress:				// keyboard event
	tqt_net_update_user_time(widget->topLevelWidget());
	// fallthrough intended
    case XKeyRelease:
	{
	    if ( keywidget && keywidget->isEnabled() ) { // should always exist
	        // tqDebug( "sending key event" );
	        keywidget->translateKeyEvent( event, grabbed );
	    }
	    break;
	}

    case GraphicsExpose:
    case Expose:				// paint event
	widget->translatePaintEvent( event );
	break;

    case ConfigureNotify:			// window move/resize event
	if ( event->xconfigure.event == event->xconfigure.window )
	    widget->translateConfigEvent( event );
	break;

    case XFocusIn: {				// got focus
	if ( widget->isDesktop() )
	    break;
	if ( inPopupMode() ) // some delayed focus event to ignore
	    break;
	if ( !widget->isTopLevel() )
	    break;
	if ( event->xfocus.detail != NotifyAncestor &&
	     event->xfocus.detail != NotifyInferior &&
	     event->xfocus.detail != NotifyNonlinear )
	    break;
	widget->createInputContext();
	setActiveWindow( widget );
	if ( tqt_focus_model == FocusModel_PointerRoot ) {
	    // We got real input focus from somewhere, but we were in PointerRoot
	    // mode, so we don't trust this event.  Check the focus model to make
	    // sure we know what focus mode we are using...
	    tqt_check_focus_model();
	}
    }
	break;

    case XFocusOut:				// lost focus
	if ( widget->isDesktop() )
	    break;
	if ( !widget->isTopLevel() )
	    break;
	if ( event->xfocus.mode == NotifyGrab )
	    tqt_xfocusout_grab_counter++;
	if ( event->xfocus.mode != NotifyNormal )
	    break;
	if ( event->xfocus.detail != NotifyAncestor &&
	     event->xfocus.detail != NotifyNonlinearVirtual &&
	     event->xfocus.detail != NotifyNonlinear )
	    break;
	if ( !inPopupMode() && widget == active_window )
	    setActiveWindow( 0 );
	break;

    case EnterNotify: {			// enter window
	if ( TQWidget::mouseGrabber()  && widget != TQWidget::mouseGrabber() )
	    break;
	if ( inPopupMode() && widget->topLevelWidget() != activePopupWidget() )
	    break;
	if ( event->xcrossing.mode != NotifyNormal ||
	     event->xcrossing.detail == NotifyVirtual  ||
	     event->xcrossing.detail == NotifyNonlinearVirtual )
	    break;
	if ( event->xcrossing.focus &&
	     !widget->isDesktop() && !widget->isActiveWindow() ) {
	    if ( tqt_focus_model == FocusModel_Unknown ) // check focus model
		tqt_check_focus_model();
	    if ( tqt_focus_model == FocusModel_PointerRoot ) // PointerRoot mode
		setActiveWindow( widget );
	}
	tqt_dispatchEnterLeave( widget, TQWidget::find( curWin ) );
	curWin = widget->winId();
	widget->translateMouseEvent( event ); //we don't get MotionNotify, emulate it
    }
	break;

    case LeaveNotify: {			// leave window
	if ( TQWidget::mouseGrabber()  && widget != TQWidget::mouseGrabber() )
	    break;
	if ( curWin && widget->winId() != curWin )
	    break;
	if ( event->xcrossing.mode != NotifyNormal )
	    break;
	if ( !widget->isDesktop() )
	    widget->translateMouseEvent( event ); //we don't get MotionNotify, emulate it

	TQWidget* enter = 0;
	XEvent ev;
	while ( XCheckMaskEvent( widget->x11Display(), EnterWindowMask | LeaveWindowMask , &ev )
		&& !tqt_x11EventFilter( &ev )) {
            TQWidget* event_widget = TQWidget::find( ev.xcrossing.window );
            if( event_widget && event_widget->x11Event( &ev ) )
                break;
	    if ( ev.type == LeaveNotify && ev.xcrossing.mode == NotifyNormal ){
		enter = event_widget;
		XPutBackEvent( widget->x11Display(), &ev );
		break;
	    }
	    if (  ev.xcrossing.mode != NotifyNormal ||
		  ev.xcrossing.detail == NotifyVirtual  ||
		  ev.xcrossing.detail == NotifyNonlinearVirtual )
		continue;
	    enter = event_widget;
	    if ( ev.xcrossing.focus &&
		 enter && !enter->isDesktop() && !enter->isActiveWindow() ) {
		if ( tqt_focus_model == FocusModel_Unknown ) // check focus model
		    tqt_check_focus_model();
		if ( tqt_focus_model == FocusModel_PointerRoot ) // PointerRoot mode
		    setActiveWindow( enter );
	    }
	    break;
	}

	if ( ( ! enter || enter->isDesktop() ) &&
	     event->xcrossing.focus && widget == active_window &&
	     tqt_focus_model == FocusModel_PointerRoot // PointerRoot mode
	     ) {
	    setActiveWindow( 0 );
	}

	if ( !curWin )
	    tqt_dispatchEnterLeave( widget, 0 );

	tqt_dispatchEnterLeave( enter, widget );
	curWin = enter ? enter->winId() : 0;
    }
	break;

    case UnmapNotify:				// window hidden
	if ( widget->isTopLevel() && widget->isShown() ) {
	    widget->topData()->spont_unmapped = 1;
	    TQHideEvent e;
	    TQApplication::sendSpontaneousEvent( widget, &e );
	    widget->hideChildren( true );
	}
	break;

    case MapNotify:				// window shown
	if ( widget->isTopLevel() &&
	     widget->topData()->spont_unmapped ) {
	    widget->topData()->spont_unmapped = 0;
	    widget->showChildren( true );
	    TQShowEvent e;
	    TQApplication::sendSpontaneousEvent( widget, &e );
	}
	break;

    case ClientMessage:			// client message
	return x11ClientMessage(widget,event,False);

    case ReparentNotify:			// window manager reparents
	while ( XCheckTypedWindowEvent( widget->x11Display(),
					widget->winId(),
					ReparentNotify,
					event ) )
	    ;	// skip old reparent events
	if ( event->xreparent.parent == TQPaintDevice::x11AppRootWindow() ) {
	    if ( widget->isTopLevel() ) {
		widget->topData()->parentWinId = event->xreparent.parent;
		if ( tqt_deferred_map_contains( widget ) ) {
		    tqt_deferred_map_take( widget );
		    XMapWindow( appDpy, widget->winId() );
		}
	    }
	} else
	    // store the parent. Useful for many things, embedding for instance.
	    widget->topData()->parentWinId = event->xreparent.parent;
	if ( widget->isTopLevel() ) {
	    // the widget frame strut should also be invalidated
	    widget->topData()->fleft = widget->topData()->fright =
	     widget->topData()->ftop = widget->topData()->fbottom = 0;

	    if ( tqt_focus_model != FocusModel_Unknown ) {
		// toplevel reparented...
		TQWidget *newparent = TQWidget::find( event->xreparent.parent );
		if ( ! newparent || newparent->isDesktop() ) {
		    // we dont' know about the new parent (or we've been
		    // reparented to root), perhaps a window manager
		    // has been (re)started?  reset the focus model to unknown
		    tqt_focus_model = FocusModel_Unknown;
		}
	    }
	}
	break;

    case SelectionRequest: {
	XSelectionRequestEvent *req = &event->xselectionrequest;
	if (! req)
	    break;

	if ( tqt_xdnd_selection && req->selection == tqt_xdnd_selection ) {
	    tqt_xdnd_handle_selection_request( req );

	} else if (tqt_clipboard) {
	    TQCustomEvent e( TQEvent::Clipboard, event );
	    TQApplication::sendSpontaneousEvent( tqt_clipboard, &e );
	}
	break;
    }
    case SelectionClear: {
	XSelectionClearEvent *req = &event->xselectionclear;
	// don't deliver dnd events to the clipboard, it gets confused
	if (! req || ( tqt_xdnd_selection && req->selection ) == tqt_xdnd_selection)
	    break;

	if (tqt_clipboard) {
	    TQCustomEvent e( TQEvent::Clipboard, event );
	    TQApplication::sendSpontaneousEvent( tqt_clipboard, &e );
	}
	break;
    }

    case SelectionNotify: {
	XSelectionEvent *req = &event->xselection;
	// don't deliver dnd events to the clipboard, it gets confused
	if (! req || ( tqt_xdnd_selection && req->selection ) == tqt_xdnd_selection)
	    break;

	if (tqt_clipboard) {
	    TQCustomEvent e( TQEvent::Clipboard, event );
	    TQApplication::sendSpontaneousEvent( tqt_clipboard, &e );
	}
	break;
    }

    default:
	break;
    }

    return 0;
}

/*!
    This virtual function is only implemented under X11.

    If you create an application that inherits TQApplication and
    reimplement this function, you get direct access to all X events
    that the are received from the X server.

    Return true if you want to stop the event from being processed.
    Return false for normal event dispatching.

    \sa x11ProcessEvent()
*/

bool TQApplication::x11EventFilter( XEvent * )
{
    return false;
}



/*****************************************************************************
  Modal widgets; Since Xlib has little support for this we roll our own
  modal widget mechanism.
  A modal widget without a parent becomes application-modal.
  A modal widget with a parent becomes modal to its parent and grandparents..

  tqt_enter_modal()
	Enters modal state
	Arguments:
	    TQWidget *widget	A modal widget

  tqt_leave_modal()
	Leaves modal state for a widget
	Arguments:
	    TQWidget *widget	A modal widget
 *****************************************************************************/

bool tqt_modal_state()
{
    return app_do_modal;
}

void tqt_enter_modal( TQWidget *widget )
{
    if ( !tqt_modal_stack ) {			// create modal stack
	tqt_modal_stack = new TQWidgetList;
	TQ_CHECK_PTR( tqt_modal_stack );
    }
    if (widget->parentWidget()) {
	TQEvent e(TQEvent::WindowBlocked);
	TQApplication::sendEvent(widget->parentWidget(), &e);
    }

    tqt_dispatchEnterLeave( 0, TQWidget::find((WId)curWin) );
    tqt_modal_stack->insert( 0, widget );
    app_do_modal = true;
    curWin = 0;
    ignoreNextMouseReleaseEvent = false;
}


void tqt_leave_modal( TQWidget *widget )
{
    if ( tqt_modal_stack && tqt_modal_stack->removeRef(widget) ) {
	if ( tqt_modal_stack->isEmpty() ) {
	    delete tqt_modal_stack;
	    tqt_modal_stack = 0;
	    TQPoint p( TQCursor::pos() );
	    TQWidget* w = TQApplication::widgetAt( p.x(), p.y(), true );
	    tqt_dispatchEnterLeave( w, TQWidget::find( curWin ) ); // send synthetic enter event
	    curWin = w? w->winId() : 0;
	}
    }
    app_do_modal = tqt_modal_stack != 0;
    ignoreNextMouseReleaseEvent = true;

    if (widget->parentWidget()) {
	TQEvent e(TQEvent::WindowUnblocked);
	TQApplication::sendEvent(widget->parentWidget(), &e);
    }
}


TQ_EXPORT bool tqt_try_modal( TQWidget *widget, XEvent *event )
{
    if (tqt_xdnd_dragging) {
	// allow mouse events while DnD is active
	switch (event->type) {
	case ButtonPress:
	case ButtonRelease:
	case MotionNotify:
	    return true;
	default:
	    break;
	}
    }

    if ( tqt_tryModalHelper( widget ) )
	return true;

    bool block_event  = false;
    switch ( event->type ) {
	case ButtonPress:			// disallow mouse/key events
	case ButtonRelease:
	case MotionNotify:
	case XKeyPress:
	case XKeyRelease:
	case EnterNotify:
	case LeaveNotify:
	case ClientMessage:
	    block_event	 = true;
	    break;
    	default:
            break;
    }

    return !block_event;
}


/*****************************************************************************
  Popup widget mechanism

  openPopup()
	Adds a widget to the list of popup widgets
	Arguments:
	    TQWidget *widget	The popup widget to be added

  closePopup()
	Removes a widget from the list of popup widgets
	Arguments:
	    TQWidget *widget	The popup widget to be removed
 *****************************************************************************/


static int openPopupCount = 0;
void TQApplication::openPopup( TQWidget *popup )
{
    openPopupCount++;
    if ( !popupWidgets ) {			// create list
	popupWidgets = new TQWidgetList;
	TQ_CHECK_PTR( popupWidgets );
    }
    popupWidgets->append( popup );		// add to end of list

    if ( popupWidgets->count() == 1 && !tqt_nograb() ){ // grab mouse/keyboard
	int r = XGrabKeyboard( popup->x11Display(), popup->winId(), false,
			       GrabModeSync, GrabModeAsync, CurrentTime );
	if ( (popupGrabOk = (r == GrabSuccess)) ) {
	    r = XGrabPointer( popup->x11Display(), popup->winId(), true,
			      (uint)(ButtonPressMask | ButtonReleaseMask |
				     ButtonMotionMask | EnterWindowMask |
				     LeaveWindowMask | PointerMotionMask),
			      GrabModeSync, GrabModeAsync,
			      None, None, CurrentTime );

	    if ( (popupGrabOk = (r == GrabSuccess)) )
		XAllowEvents( popup->x11Display(), SyncPointer, CurrentTime );
	    else
		XUngrabKeyboard( popup->x11Display(), CurrentTime );
	}
    } else if ( popupGrabOk ) {
	XAllowEvents(  popup->x11Display(), SyncPointer, CurrentTime );
    }

    // popups are not focus-handled by the window system (the first
    // popup grabbed the keyboard), so we have to do that manually: A
    // new popup gets the focus
    TQFocusEvent::setReason( TQFocusEvent::Popup );
    if ( popup->focusWidget())
	popup->focusWidget()->setFocus();
    else
	popup->setFocus();
    TQFocusEvent::resetReason();
}

void TQApplication::closePopup( TQWidget *popup )
{
    if ( !popupWidgets )
	return;
    popupWidgets->removeRef( popup );
    if (popup == popupOfPopupButtonFocus) {
	popupButtonFocus = 0;
	popupOfPopupButtonFocus = 0;
    }
    if ( popupWidgets->count() == 0 ) {		// this was the last popup
	popupCloseDownMode = true;		// control mouse events
	delete popupWidgets;
	popupWidgets = 0;
	if ( !tqt_nograb() && popupGrabOk ) {	// grabbing not disabled
	    if ( mouseButtonState != 0
		 || popup->geometry(). contains(TQPoint(mouseGlobalXPos, mouseGlobalYPos) ) )
		{	// mouse release event or inside
		    XAllowEvents( popup->x11Display(), AsyncPointer,
				  CurrentTime );
	    } else {				// mouse press event
		mouseButtonPressTime -= 10000;	// avoid double click
		XAllowEvents( popup->x11Display(), ReplayPointer,CurrentTime );
	    }
	    XUngrabPointer( popup->x11Display(), CurrentTime );
	    XFlush( popup->x11Display() );
	}
	if ( active_window ) {
	    TQFocusEvent::setReason( TQFocusEvent::Popup );
	    if ( active_window->focusWidget() )
		active_window->focusWidget()->setFocus();
	    else
		active_window->setFocus();
	    TQFocusEvent::resetReason();
	}
    } else {
	// popups are not focus-handled by the window system (the
	// first popup grabbed the keyboard), so we have to do that
	// manually: A popup was closed, so the previous popup gets
	// the focus.
	 TQFocusEvent::setReason( TQFocusEvent::Popup );
	 TQWidget* aw = popupWidgets->getLast();
	 if (aw->focusWidget())
	     aw->focusWidget()->setFocus();
	 else
	     aw->setFocus();
	 TQFocusEvent::resetReason();
	 if ( popupWidgets->count() == 1 && !tqt_nograb() ){ // grab mouse/keyboard
	     int r = XGrabKeyboard( aw->x11Display(), aw->winId(), false,
				    GrabModeSync, GrabModeAsync, CurrentTime );
	     if ( (popupGrabOk = (r == GrabSuccess)) ) {
		 r = XGrabPointer( aw->x11Display(), aw->winId(), true,
				   (uint)(ButtonPressMask | ButtonReleaseMask |
					  ButtonMotionMask | EnterWindowMask |
					  LeaveWindowMask | PointerMotionMask),
				   GrabModeSync, GrabModeAsync,
				   None, None, CurrentTime );

		 if ( (popupGrabOk = (r == GrabSuccess)) )
		     XAllowEvents( aw->x11Display(), SyncPointer, CurrentTime );
	     }
	 }
     }
}

/*****************************************************************************
  Event translation; translates X11 events to TQt events
 *****************************************************************************/

//
// Mouse event translation
//
// Xlib doesn't give mouse double click events, so we generate them by
// comparing window, time and position between two mouse press events.
//

//
// Keyboard event translation
//

int tqt_x11_translateButtonState( int s )
{
    int bst = 0;
    if ( s & Button1Mask )
	bst |= TQt::LeftButton;
    if ( s & Button2Mask )
	bst |= TQt::MidButton;
    if ( s & Button3Mask )
	bst |= TQt::RightButton;
    if ( s & ShiftMask )
	bst |= TQt::ShiftButton;
    if ( s & ControlMask )
	bst |= TQt::ControlButton;
    if ( s & tqt_alt_mask )
	bst |= TQt::AltButton;
    if ( s & tqt_meta_mask )
	bst |= TQt::MetaButton;
    return bst;
}

bool TQETWidget::translateMouseEvent( const XEvent *event )
{
    static bool manualGrab = false;
    TQEvent::Type type;				// event parameters
    TQPoint pos;
    TQPoint globalPos;
    int button = 0;
    int state;
    XEvent nextEvent;

    if ( sm_blockUserInput ) // block user interaction during session management
	return true;

    static int x_root_save = -1, y_root_save = -1;

    if ( event->type == MotionNotify ) { // mouse move
	if (event->xmotion.root != RootWindow(appDpy, x11Screen()) &&
	    ! tqt_xdnd_dragging )
	    return false;

	XMotionEvent lastMotion = event->xmotion;
	while( XPending( appDpy ) )  { // compres mouse moves
	    XNextEvent( appDpy, &nextEvent );
	    if ( nextEvent.type == ConfigureNotify
		 || nextEvent.type == PropertyNotify
		 || nextEvent.type == Expose
		 || nextEvent.type == NoExpose ) {
		tqApp->x11ProcessEvent( &nextEvent );
		continue;
	    } else if ( nextEvent.type != MotionNotify ||
			nextEvent.xmotion.window != event->xmotion.window ||
			nextEvent.xmotion.state != event->xmotion.state ) {
		XPutBackEvent( appDpy, &nextEvent );
		break;
	    }
	    if ( !tqt_x11EventFilter(&nextEvent)
		 && !x11Event( &nextEvent ) ) // send event through filter
		lastMotion = nextEvent.xmotion;
	    else
		break;
	}
	type = TQEvent::MouseMove;
	pos.rx() = lastMotion.x;
	pos.ry() = lastMotion.y;
	globalPos.rx() = lastMotion.x_root;
	globalPos.ry() = lastMotion.y_root;
	state = tqt_x11_translateButtonState( lastMotion.state );
	if ( tqt_button_down && (state & (LeftButton |
					 MidButton |
					 RightButton ) ) == 0 )
	    tqt_button_down = 0;

	// throw away mouse move events that are sent multiple times to the same
	// position
	bool throw_away = false;
	if ( x_root_save == globalPos.x() &&
	     y_root_save == globalPos.y() )
	    throw_away = true;
	x_root_save = globalPos.x();
	y_root_save = globalPos.y();
	if ( throw_away )
	    return true;
    } else if ( event->type == EnterNotify || event->type == LeaveNotify) {
	XEvent *xevent = (XEvent *)event;
	//unsigned int xstate = event->xcrossing.state;
	type = TQEvent::MouseMove;
	pos.rx() = xevent->xcrossing.x;
	pos.ry() = xevent->xcrossing.y;
	globalPos.rx() = xevent->xcrossing.x_root;
	globalPos.ry() = xevent->xcrossing.y_root;
	state = tqt_x11_translateButtonState( xevent->xcrossing.state );
	if ( tqt_button_down && (state & (LeftButton |
					 MidButton |
					 RightButton ) ) == 0 )
	    tqt_button_down = 0;
	if ( !tqt_button_down )
	    state = state & ~(LeftButton | MidButton | RightButton );
    } else {					// button press or release
	pos.rx() = event->xbutton.x;
	pos.ry() = event->xbutton.y;
	globalPos.rx() = event->xbutton.x_root;
	globalPos.ry() = event->xbutton.y_root;
	state = tqt_x11_translateButtonState( event->xbutton.state );
	switch ( event->xbutton.button ) {
	case Button1: button = LeftButton; break;
	case Button2: button = MidButton; break;
	case Button3: button = RightButton; break;
	case Button4:
	case Button5:
	case 6:
	case 7:
	    // the fancy mouse wheel.

	    // take care about grabbing. We do this here since it
	    // is clear that we return anyway
	    if ( tqApp->inPopupMode() && popupGrabOk )
		XAllowEvents( x11Display(), SyncPointer, CurrentTime );

	    // We are only interested in ButtonPress.
	    if (event->type == ButtonPress ){

		// compress wheel events (the X Server will simply
		// send a button press for each single notch,
		// regardless whether the application can catch up
		// or not)
		int delta = 1;
		XEvent xevent;
		while ( XCheckTypedWindowEvent(x11Display(),winId(),
					       ButtonPress,&xevent) ){
		    if (xevent.xbutton.button != event->xbutton.button){
			XPutBackEvent(x11Display(), &xevent);
			break;
		    }
		    delta++;
		}

		// the delta is defined as multiples of
		// WHEEL_DELTA, which is set to 120. Future wheels
		// may offer a finer-resolution. A positive delta
		// indicates forward rotation, a negative one
		// backward rotation respectively.
		int btn = event->xbutton.button;
		delta *= 120 * ( (btn == Button4 || btn == 6) ? 1 : -1 );
		bool hor = ( ( (btn == Button4 || btn == Button5) && (state&AltButton) ) ||
			     (btn == 6 || btn == 7) );
		translateWheelEvent( globalPos.x(), globalPos.y(), delta, state, (hor)?Horizontal:Vertical );
	    }
	    return true;

        // history navigation buttons
        case 8: button = HistoryBackButton; break;
        case 9: button = HistoryForwardButton; break;
	}
	if ( event->type == ButtonPress ) {	// mouse button pressed
	    tqt_button_down = childAt( pos );	//magic for masked widgets
	    if ( !tqt_button_down || !tqt_button_down->testWFlags(WMouseNoMask) )
		tqt_button_down = this;
	    if ( mouseActWindow == event->xbutton.window &&
		 mouseButtonPressed == button &&
		 (long)event->xbutton.time -(long)mouseButtonPressTime
		 < TQApplication::doubleClickInterval() &&
		 TQABS(event->xbutton.x - mouseXPos) < 5 &&
		 TQABS(event->xbutton.y - mouseYPos) < 5 ) {
		type = TQEvent::MouseButtonDblClick;
		mouseButtonPressTime -= 2000;	// no double-click next time
	    } else {
		type = TQEvent::MouseButtonPress;
		mouseButtonPressTime = event->xbutton.time;
	    }
	    mouseButtonPressed = button;	// save event params for
	    mouseXPos = pos.x();		// future double click tests
	    mouseYPos = pos.y();
	    mouseGlobalXPos = globalPos.x();
	    mouseGlobalYPos = globalPos.y();
	} else {				// mouse button released
	    if ( manualGrab ) {			// release manual grab
		manualGrab = false;
		XUngrabPointer( x11Display(), CurrentTime );
		XFlush( x11Display() );
	    }

	    type = TQEvent::MouseButtonRelease;
	}
    }
    mouseActWindow = winId();			// save some event params
    mouseButtonState = state;
    if ( type == 0 )				// don't send event
	return false;

    if ( tqApp->inPopupMode() ) {			// in popup mode
	TQWidget *popup = tqApp->activePopupWidget();
	if ( popup != this ) {
	    if ( testWFlags(WType_Popup) && rect().contains(pos) )
		popup = this;
	    else				// send to last popup
		pos = popup->mapFromGlobal( globalPos );
	}
	bool releaseAfter = false;
	TQWidget *popupChild  = popup->childAt( pos );
	TQWidget *popupTarget = popupChild ? popupChild : popup;

	if (popup != popupOfPopupButtonFocus){
	    popupButtonFocus = 0;
	    popupOfPopupButtonFocus = 0;
	}

	if ( !popupTarget->isEnabled() ) {
	    if ( popupGrabOk )
		XAllowEvents( x11Display(), SyncPointer, CurrentTime );
	}

	switch ( type ) {
	case TQEvent::MouseButtonPress:
	case TQEvent::MouseButtonDblClick:
	    popupButtonFocus = popupChild;
	    popupOfPopupButtonFocus = popup;
	    break;
	case TQEvent::MouseButtonRelease:
	    releaseAfter = true;
	    break;
	default:
	    break;				// nothing for mouse move
	}

	Display* dpy = x11Display(); // store display, send() may destroy us


	int oldOpenPopupCount = openPopupCount;

	if ( popupButtonFocus ) {
	    TQMouseEvent e( type, popupButtonFocus->mapFromGlobal(globalPos),
			   globalPos, button, state );
	    TQApplication::sendSpontaneousEvent( popupButtonFocus, &e );
	    if ( releaseAfter ) {
		popupButtonFocus = 0;
		popupOfPopupButtonFocus = 0;
	    }
	} else if ( popupChild ) {
	    TQMouseEvent e( type, popupChild->mapFromGlobal(globalPos),
			   globalPos, button, state );
	    TQApplication::sendSpontaneousEvent( popupChild, &e );
	} else {
	    TQMouseEvent e( type, pos, globalPos, button, state );
	    TQApplication::sendSpontaneousEvent( popup, &e );
	}

	if ( type == TQEvent::MouseButtonPress && button == RightButton && ( openPopupCount == oldOpenPopupCount ) ) {
	    TQWidget *popupEvent = popup;
	    if(popupButtonFocus)
		popupEvent = popupButtonFocus;
	    else if(popupChild)
		popupEvent = popupChild;
	    TQContextMenuEvent e( TQContextMenuEvent::Mouse, pos, globalPos, state );
	    TQApplication::sendSpontaneousEvent( popupEvent, &e );
	}

	if ( releaseAfter )
	    tqt_button_down = 0;

	if ( tqApp->inPopupMode() ) { // still in popup mode
	    if ( popupGrabOk )
		XAllowEvents( dpy, SyncPointer, CurrentTime );
	} else {
	    if ( type != TQEvent::MouseButtonRelease && state != 0 &&
		 TQWidget::find((WId)mouseActWindow) ) {
		manualGrab = true;		// need to manually grab
		XGrabPointer( dpy, mouseActWindow, False,
			      (uint)(ButtonPressMask | ButtonReleaseMask |
				     ButtonMotionMask |
				     EnterWindowMask | LeaveWindowMask),
			      GrabModeAsync, GrabModeAsync,
			      None, None, CurrentTime );
	    }
	}

    } else {
	TQWidget *widget = this;
	TQWidget *w = TQWidget::mouseGrabber();
	if ( !w )
	    w = tqt_button_down;
	if ( w && w != this ) {
	    widget = w;
	    pos = w->mapFromGlobal( globalPos );
	}

	if ( popupCloseDownMode ) {
	    popupCloseDownMode = false;
	    if ( testWFlags(WType_Popup) )	// ignore replayed event
		return true;
	}

	if ( type == TQEvent::MouseButtonRelease &&
	     (state & (~button) & ( LeftButton |
				    MidButton |
				    RightButton)) == 0 ) {
	    tqt_button_down = 0;
	}

	int oldOpenPopupCount = openPopupCount;

	TQMouseEvent e( type, pos, globalPos, button, state );
	TQApplication::sendSpontaneousEvent( widget, &e );

	if ( type == TQEvent::MouseButtonPress && button == RightButton && ( openPopupCount == oldOpenPopupCount ) ) {
	    TQContextMenuEvent e( TQContextMenuEvent::Mouse, pos, globalPos, state );
	    TQApplication::sendSpontaneousEvent( widget, &e );
	}
    }
    return true;
}


//
// Wheel event translation
//
bool TQETWidget::translateWheelEvent( int global_x, int global_y, int delta, int state, Orientation orient )
{
    // send the event to the widget or its ancestors
    {
	TQWidget* popup = tqApp->activePopupWidget();
	if ( popup && topLevelWidget() != popup )
	    popup->close();
	TQWheelEvent e( mapFromGlobal(TQPoint( global_x, global_y)),
		       TQPoint(global_x, global_y), delta, state, orient );
	if ( TQApplication::sendSpontaneousEvent( this, &e ) )
	    return true;
    }

    // send the event to the widget that has the focus or its ancestors, if different
    TQWidget *w = this;
    if ( w != tqApp->focusWidget() && ( w = tqApp->focusWidget() ) ) {
	TQWidget* popup = tqApp->activePopupWidget();
	if ( popup && w != popup )
	    popup->hide();
	TQWheelEvent e( mapFromGlobal(TQPoint( global_x, global_y)),
		       TQPoint(global_x, global_y), delta, state, orient );
	if ( TQApplication::sendSpontaneousEvent( w, &e ) )
	    return true;
    }
    return false;
}


//
// XInput Translation Event
//
#if defined (QT_TABLET_SUPPORT)
bool TQETWidget::translateXinputEvent( const XEvent *ev )
{
    TQWidget *w = this;
    TQPoint global,
	curr;
    static int pressure = 0;
    static int xTilt = 0,
	       yTilt = 0;
    int deviceType = TQTabletEvent::NoDevice;
    TQPair<int, int> tId;
    XEvent xinputMotionEvent;
    XEvent mouseMotionEvent;
    const XDeviceMotionEvent *motion = 0;
    XDeviceButtonEvent *button = 0;
    TQEvent::Type t;

    if ( ev->type == xinput_motion ) {
	motion = (const XDeviceMotionEvent*)ev;
	for (;;) {
	    if (!XCheckTypedWindowEvent(x11Display(), winId(), MotionNotify, &mouseMotionEvent))
		break;
	    if (!XCheckTypedWindowEvent(x11Display(), winId(), xinput_motion, &xinputMotionEvent)) {
		XPutBackEvent(x11Display(), &mouseMotionEvent);
		break;
	    }
	    if (mouseMotionEvent.xmotion.time != motion->time) {
		XPutBackEvent(x11Display(), &mouseMotionEvent);
		XPutBackEvent(x11Display(), &xinputMotionEvent);
		break;
	    }
	    motion = ((const XDeviceMotionEvent*)&xinputMotionEvent);
	}
	t = TQEvent::TabletMove;
	curr = TQPoint( motion->x, motion->y );
    } else {
	if ( ev->type == xinput_button_press ) {
	    t = TQEvent::TabletPress;
        } else {
	    t = TQEvent::TabletRelease;
	}
	button = (XDeviceButtonEvent*)ev;
/*
	tqDebug( "\n\nXInput Button Event" );
	tqDebug( "serial:\t%d", button->serial );
	tqDebug( "send_event:\t%d", button->send_event );
	tqDebug( "display:\t%p", button->display );
	tqDebug( "window:\t%d", button->window );
	tqDebug( "deviceID:\t%d", button->deviceid );
	tqDebug( "root:\t%d", button->root );
	tqDebug( "subwindot:\t%d", button->subwindow );
	tqDebug( "x:\t%d", button->x );
	tqDebug( "y:\t%d", button->y );
	tqDebug( "x_root:\t%d", button->x_root );
	tqDebug( "y_root:\t%d", button->y_root );
	tqDebug( "state:\t%d", button->state );
	tqDebug( "button:\t%d", button->button );
	tqDebug( "same_screen:\t%d", button->same_screen );
	tqDebug( "time:\t%d", button->time );
*/
	curr = TQPoint( button->x, button->y );
    }
    if ( ev->type == xinput_motion ) {
	if ( motion->deviceid == devStylus->device_id ) {
	    deviceType = TQTabletEvent::Stylus;
	} else if ( motion->deviceid == devEraser->device_id ) {
	    deviceType = TQTabletEvent::Eraser;
	}
    } else {
	if ( button->deviceid == devStylus->device_id ) {
	    deviceType = TQTabletEvent::Stylus;
	} else if ( button->deviceid == devEraser->device_id ) {
	    deviceType = TQTabletEvent::Eraser;
	}
    }

    const int PRESSURE_LEVELS = 255;
    // we got the maximum pressure at start time, since various tablets have
    // varying levels of distinguishing pressure changes, let's standardize and
    // scale everything to 256 different levels...
    static int scaleFactor = -1;
    if ( scaleFactor == -1 ) {
	if ( max_pressure > PRESSURE_LEVELS )
	    scaleFactor = max_pressure / PRESSURE_LEVELS;
	else
	    scaleFactor = PRESSURE_LEVELS / max_pressure;
    }
    if ( motion ) {
	xTilt = short(motion->axis_data[3]);
	yTilt = short(motion->axis_data[4]);
	if ( max_pressure > PRESSURE_LEVELS )
	    pressure = motion->axis_data[2] / scaleFactor;
	else
	    pressure = motion->axis_data[2] * scaleFactor;
	global = TQPoint( motion->axis_data[0], motion->axis_data[1] );
    } else {
	xTilt = short(button->axis_data[3]);
	yTilt = short(button->axis_data[4]);
	if ( max_pressure > PRESSURE_LEVELS )
	    pressure = button->axis_data[2]  / scaleFactor;
	else
	    pressure = button->axis_data[2] * scaleFactor;
	global = TQPoint( button->axis_data[0], button->axis_data[1] );
    }
    // The only way to get these Ids is to scan the XFree86 log, which I'm not going to do.
    tId.first = tId.second = -1;

    TQTabletEvent e( t, curr, global, deviceType, pressure, xTilt, yTilt, tId );
    TQApplication::sendSpontaneousEvent( w, &e );
    return true;
}
#endif

bool TQETWidget::translatePropertyEvent(const XEvent *event)
{
    if (!isTopLevel()) return true;

    Atom ret;
    int format, e;
    unsigned char *data = 0;
    unsigned long nitems, after;

    if (event->xproperty.atom == tqt_net_wm_frame_strut) {
	topData()->fleft = topData()->fright = topData()->ftop = topData()->fbottom = 0;
	fstrut_dirty = 1;

	if (event->xproperty.state == PropertyNewValue) {
	    e = XGetWindowProperty(appDpy, event->xproperty.window, tqt_net_wm_frame_strut,
				   0, 4, // struts are 4 longs
				   False, XA_CARDINAL, &ret, &format, &nitems, &after, &data);

	    if (e == Success && ret == XA_CARDINAL &&
		format == 32 && nitems == 4) {
		long *strut = (long *) data;
		topData()->fleft   = strut[0];
		topData()->fright  = strut[1];
		topData()->ftop    = strut[2];
		topData()->fbottom = strut[3];
		fstrut_dirty = 0;
	    }
	}
    } else if (event->xproperty.atom == tqt_net_wm_state) {
	bool max = false;
	bool full = false;

	if (event->xproperty.state == PropertyNewValue) {
	    // using length of 1024 should be safe for all current and
	    // possible NET states...
	    e = XGetWindowProperty(appDpy, event->xproperty.window, tqt_net_wm_state, 0, 1024,
				   False, XA_ATOM, &ret, &format, &nitems, &after, &data);

	    if (e == Success && ret == XA_ATOM && format == 32 && nitems > 0) {
		Atom *states = (Atom *) data;

		unsigned long i;
		for (i = 0; i < nitems; i++) {
		    if (states[i] == tqt_net_wm_state_max_v || states[i] == tqt_net_wm_state_max_h)
			max = true;
		    else if (states[i] == tqt_net_wm_state_fullscreen)
			full = true;
		}
	    }
	}

	bool send_event = false;

        if (tqt_net_supports(tqt_net_wm_state_max_v)
            && tqt_net_supports(tqt_net_wm_state_max_h)) {
            if (max && !isMaximized()) {
                setWState(WState_Maximized);
                send_event = true;
            } else if (!max && isMaximized()) {
                clearWState(WState_Maximized);
                send_event = true;
            }
        }

        if (tqt_net_supports(tqt_net_wm_state_fullscreen)) {
            if (full && !isFullScreen()) {
                setWState(WState_FullScreen);
                send_event = true;
            } else if (!full && isFullScreen()) {
                clearWState(WState_FullScreen);
                send_event = true;
            }
        }

	if (send_event) {
	    TQEvent e(TQEvent::WindowStateChange);
	    TQApplication::sendSpontaneousEvent(this, &e);
	}
    } else if (event->xproperty.atom == tqt_wm_state) {
	// the widget frame strut should also be invalidated
	topData()->fleft = topData()->fright = topData()->ftop = topData()->fbottom = 0;
	fstrut_dirty = 1;

	if (event->xproperty.state == PropertyDelete) {
	    // the window manager has removed the WM State property,
	    // so it is now in the withdrawn state (ICCCM 4.1.3.1) and
	    // we are free to reuse this window
	    topData()->parentWinId = 0;
	    // map the window if we were waiting for a transition to
	    // withdrawn
	    if ( tqt_deferred_map_contains( this ) ) {
		tqt_deferred_map_take( this );
		XMapWindow( appDpy, winId() );
	    }
	} else if (topData()->parentWinId != TQPaintDevice::x11AppRootWindow(x11Screen())) {
	    // the window manager has changed the WM State property...
	    // we are wanting to see if we are withdrawn so that we
	    // can reuse this window... we only do this check *IF* we
	    // haven't been reparented to root - (the parentWinId !=
	    // TQPaintDevice::x11AppRootWindow(x11Screen())) check
	    // above

	    e = XGetWindowProperty(appDpy, winId(), tqt_wm_state, 0, 2, False, tqt_wm_state,
				   &ret, &format, &nitems, &after, &data );

	    if (e == Success && ret == tqt_wm_state && format == 32 && nitems > 0) {
		long *state = (long *) data;
		switch (state[0]) {
		case WithdrawnState:
		    // if we are in the withdrawn state, we are free
		    // to reuse this window provided we remove the
		    // WM_STATE property (ICCCM 4.1.3.1)
		    XDeleteProperty(appDpy, winId(), tqt_wm_state);

		    // set the parent id to zero, so that show() will
		    // work again
		    topData()->parentWinId = 0;
		    // map the window if we were waiting for a
		    // transition to withdrawn
		    if ( tqt_deferred_map_contains( this ) ) {
			tqt_deferred_map_take( this );
			XMapWindow( appDpy, winId() );
		    }
		    break;

		case IconicState:
		    if (!isMinimized()) {
			// window was minimized
			setWState(WState_Minimized);
			TQEvent e(TQEvent::WindowStateChange);
			TQApplication::sendSpontaneousEvent(this, &e);
		    }
		    break;

		default:
		    if (isMinimized()) {
			// window was un-minimized
			clearWState(WState_Minimized);
			TQEvent e(TQEvent::WindowStateChange);
			TQApplication::sendSpontaneousEvent(this, &e);
		    }
		    break;
		}
	    }
	}
    }

    if (data)
	XFree(data);

    return true;
}

#ifndef XK_ISO_Left_Tab
#define	XK_ISO_Left_Tab					0xFE20
#endif

// the next lines are taken from XFree > 4.0 (X11/XF86keysyms.h), defining some special
// multimedia keys. They are included here as not every system has them.
#define XF86XK_Standby		0x1008FF10
#define XF86XK_AudioLowerVolume	0x1008FF11
#define XF86XK_AudioMute	0x1008FF12
#define XF86XK_AudioRaiseVolume	0x1008FF13
#define XF86XK_AudioPlay	0x1008FF14
#define XF86XK_AudioStop	0x1008FF15
#define XF86XK_AudioPrev	0x1008FF16
#define XF86XK_AudioNext	0x1008FF17
#define XF86XK_HomePage		0x1008FF18
#define XF86XK_Calculator	0x1008FF1D
#define XF86XK_Mail		0x1008FF19
#define XF86XK_Start		0x1008FF1A
#define XF86XK_Search		0x1008FF1B
#define XF86XK_AudioRecord	0x1008FF1C
#define XF86XK_Back		0x1008FF26
#define XF86XK_Forward		0x1008FF27
#define XF86XK_Stop		0x1008FF28
#define XF86XK_Refresh		0x1008FF29
#define XF86XK_Favorites	0x1008FF30
#define XF86XK_AudioPause	0x1008FF31
#define XF86XK_AudioMedia	0x1008FF32
#define XF86XK_MyComputer	0x1008FF33
#define XF86XK_OpenURL		0x1008FF38
#define XF86XK_Launch0		0x1008FF40
#define XF86XK_Launch1		0x1008FF41
#define XF86XK_Launch2		0x1008FF42
#define XF86XK_Launch3		0x1008FF43
#define XF86XK_Launch4		0x1008FF44
#define XF86XK_Launch5		0x1008FF45
#define XF86XK_Launch6		0x1008FF46
#define XF86XK_Launch7		0x1008FF47
#define XF86XK_Launch8		0x1008FF48
#define XF86XK_Launch9		0x1008FF49
#define XF86XK_LaunchA		0x1008FF4A
#define XF86XK_LaunchB		0x1008FF4B
#define XF86XK_LaunchC		0x1008FF4C
#define XF86XK_LaunchD		0x1008FF4D
#define XF86XK_LaunchE		0x1008FF4E
#define XF86XK_LaunchF		0x1008FF4F
#define XF86XK_MonBrightnessUp   0x1008FF02  /* Monitor/panel brightness */
#define XF86XK_MonBrightnessDown 0x1008FF03  /* Monitor/panel brightness */
#define XF86XK_KbdLightOnOff     0x1008FF04  /* Keyboards may be lit     */
#define XF86XK_KbdBrightnessUp   0x1008FF05  /* Keyboards may be lit     */
#define XF86XK_KbdBrightnessDown 0x1008FF06  /* Keyboards may be lit     */
// end of XF86keysyms.h



static const KeySym KeyTbl[] = {		// keyboard mapping table
    XK_Escape,		TQt::Key_Escape,		// misc keys
    XK_Tab,		TQt::Key_Tab,
    XK_ISO_Left_Tab,    TQt::Key_Backtab,
    XK_BackSpace,	TQt::Key_Backspace,
    XK_Return,		TQt::Key_Return,
    XK_Insert,		TQt::Key_Insert,
    XK_KP_Insert,	TQt::Key_Insert,
    XK_Delete,		TQt::Key_Delete,
    XK_KP_Delete,	TQt::Key_Delete,
    XK_Clear,		TQt::Key_Delete,
    XK_Pause,		TQt::Key_Pause,
    XK_Print,		TQt::Key_Print,
    XK_KP_Begin,	TQt::Key_Clear,
    0x1005FF60,		TQt::Key_SysReq,		// hardcoded Sun SysReq
    0x1007ff00,		TQt::Key_SysReq,		// hardcoded X386 SysReq
    XK_Home,		TQt::Key_Home,		// cursor movement
    XK_End,		TQt::Key_End,
    XK_Left,		TQt::Key_Left,
    XK_Up,		TQt::Key_Up,
    XK_Right,		TQt::Key_Right,
    XK_Down,		TQt::Key_Down,
    XK_Prior,		TQt::Key_Prior,
    XK_Next,		TQt::Key_Next,
    XK_KP_Home,		TQt::Key_Home,
    XK_KP_End,		TQt::Key_End,
    XK_KP_Left,		TQt::Key_Left,
    XK_KP_Up,		TQt::Key_Up,
    XK_KP_Right,	TQt::Key_Right,
    XK_KP_Down,		TQt::Key_Down,
    XK_KP_Prior,	TQt::Key_Prior,
    XK_KP_Next,		TQt::Key_Next,
    XK_Shift_L,		TQt::Key_Shift,		// modifiers
    XK_Shift_R,		TQt::Key_Shift,
    XK_Shift_Lock,	TQt::Key_Shift,
    XK_Control_L,	TQt::Key_Control,
    XK_Control_R,	TQt::Key_Control,
    XK_Meta_L,		TQt::Key_Meta,
    XK_Meta_R,		TQt::Key_Meta,
    XK_Alt_L,		TQt::Key_Alt,
    XK_Alt_R,		TQt::Key_Alt,
    XK_Caps_Lock,	TQt::Key_CapsLock,
    XK_Num_Lock,	TQt::Key_NumLock,
    XK_Scroll_Lock,	TQt::Key_ScrollLock,
    XK_KP_Space,	TQt::Key_Space,		// numeric keypad
    XK_KP_Tab,		TQt::Key_Tab,
    XK_KP_Enter,	TQt::Key_Enter,
    XK_KP_Equal,	TQt::Key_Equal,
    XK_KP_Multiply,	TQt::Key_Asterisk,
    XK_KP_Add,		TQt::Key_Plus,
    XK_KP_Separator,	TQt::Key_Comma,
    XK_KP_Subtract,	TQt::Key_Minus,
    XK_KP_Decimal,	TQt::Key_Period,
    XK_KP_Divide,	TQt::Key_Slash,
    XK_Super_L,		TQt::Key_Super_L,
    XK_Super_R,		TQt::Key_Super_R,
    XK_Menu,		TQt::Key_Menu,
    XK_Hyper_L,		TQt::Key_Hyper_L,
    XK_Hyper_R,		TQt::Key_Hyper_R,
    XK_Help,		TQt::Key_Help,
    0x1000FF74,         TQt::Key_BackTab,     // hardcoded HP backtab
    0x1005FF10,         TQt::Key_F11,         // hardcoded Sun F36 (labeled F11)
    0x1005FF11,         TQt::Key_F12,         // hardcoded Sun F37 (labeled F12)

    // International input method support keys

    // International & multi-key character composition
    XK_Multi_key,		TQt::Key_Multi_key,
    XK_Codeinput,		TQt::Key_Codeinput,
    XK_SingleCandidate,		TQt::Key_SingleCandidate,
    XK_MultipleCandidate,	TQt::Key_MultipleCandidate,
    XK_PreviousCandidate,	TQt::Key_PreviousCandidate,

    // Misc Functions
    XK_Mode_switch,		TQt::Key_Mode_switch,
    //XK_script_switch,		TQt::Key_script_switch,
    XK_script_switch,		TQt::Key_Mode_switch,

    // Japanese keyboard support
    XK_Kanji,			TQt::Key_Kanji,
    XK_Muhenkan,		TQt::Key_Muhenkan,
    //XK_Henkan_Mode,		TQt::Key_Henkan_Mode,
    XK_Henkan_Mode,		TQt::Key_Henkan,
    XK_Henkan,			TQt::Key_Henkan,
    XK_Romaji,			TQt::Key_Romaji,
    XK_Hiragana,		TQt::Key_Hiragana,
    XK_Katakana,		TQt::Key_Katakana,
    XK_Hiragana_Katakana,	TQt::Key_Hiragana_Katakana,
    XK_Zenkaku,			TQt::Key_Zenkaku,
    XK_Hankaku,			TQt::Key_Hankaku,
    XK_Zenkaku_Hankaku,		TQt::Key_Zenkaku_Hankaku,
    XK_Touroku,			TQt::Key_Touroku,
    XK_Massyo,			TQt::Key_Massyo,
    XK_Kana_Lock,		TQt::Key_Kana_Lock,
    XK_Kana_Shift,		TQt::Key_Kana_Shift,
    XK_Eisu_Shift,		TQt::Key_Eisu_Shift,
    XK_Eisu_toggle,		TQt::Key_Eisu_toggle,
    //XK_Kanji_Bangou,		TQt::Key_Kanji_Bangou,
    //XK_Zen_Koho,		TQt::Key_Zen_Koho,
    //XK_Mae_Koho,		TQt::Key_Mae_Koho,
    XK_Kanji_Bangou,		TQt::Key_Codeinput,
    XK_Zen_Koho,		TQt::Key_MultipleCandidate,
    XK_Mae_Koho,		TQt::Key_PreviousCandidate,

#ifdef XK_KOREAN
    // Korean keyboard support
    XK_Hangul,			TQt::Key_Hangul,
    XK_Hangul_Start,		TQt::Key_Hangul_Start,
    XK_Hangul_End,		TQt::Key_Hangul_End,
    XK_Hangul_Hanja,		TQt::Key_Hangul_Hanja,
    XK_Hangul_Jamo,		TQt::Key_Hangul_Jamo,
    XK_Hangul_Romaja,		TQt::Key_Hangul_Romaja,
    //XK_Hangul_Codeinput,	TQt::Key_Hangul_Codeinput,
    XK_Hangul_Codeinput,	TQt::Key_Codeinput,
    XK_Hangul_Jeonja,		TQt::Key_Hangul_Jeonja,
    XK_Hangul_Banja,		TQt::Key_Hangul_Banja,
    XK_Hangul_PreHanja,		TQt::Key_Hangul_PreHanja,
    XK_Hangul_PostHanja,	TQt::Key_Hangul_PostHanja,
    //XK_Hangul_SingleCandidate,	TQt::Key_Hangul_SingleCandidate,
    //XK_Hangul_MultipleCandidate, TQt::Key_Hangul_MultipleCandidate,
    //XK_Hangul_PreviousCandidate, TQt::Key_Hangul_PreviousCandidate,
    XK_Hangul_SingleCandidate,	TQt::Key_SingleCandidate,
    XK_Hangul_MultipleCandidate, TQt::Key_MultipleCandidate,
    XK_Hangul_PreviousCandidate, TQt::Key_PreviousCandidate,
    XK_Hangul_Special,		TQt::Key_Hangul_Special,
    //XK_Hangul_switch,		TQt::Key_Hangul_switch,
    XK_Hangul_switch,		TQt::Key_Mode_switch,
#endif  // XK_KOREAN

    // dead keys
    XK_dead_grave,              TQt::Key_Dead_Grave,
    XK_dead_acute,              TQt::Key_Dead_Acute,
    XK_dead_circumflex,         TQt::Key_Dead_Circumflex,
    XK_dead_tilde,              TQt::Key_Dead_Tilde,
    XK_dead_macron,             TQt::Key_Dead_Macron,
    XK_dead_breve,              TQt::Key_Dead_Breve,
    XK_dead_abovedot,           TQt::Key_Dead_Abovedot,
    XK_dead_diaeresis,          TQt::Key_Dead_Diaeresis,
    XK_dead_abovering,          TQt::Key_Dead_Abovering,
    XK_dead_doubleacute,        TQt::Key_Dead_Doubleacute,
    XK_dead_caron,              TQt::Key_Dead_Caron,
    XK_dead_cedilla,            TQt::Key_Dead_Cedilla,
    XK_dead_ogonek,             TQt::Key_Dead_Ogonek,
    XK_dead_iota,               TQt::Key_Dead_Iota,
    XK_dead_voiced_sound,       TQt::Key_Dead_Voiced_Sound,
    XK_dead_semivoiced_sound,   TQt::Key_Dead_Semivoiced_Sound,
    XK_dead_belowdot,           TQt::Key_Dead_Belowdot,
    XK_dead_hook,               TQt::Key_Dead_Hook,
    XK_dead_horn,               TQt::Key_Dead_Horn,

    // Special multimedia keys
    // currently only tested with MS internet keyboard

    // browsing keys
    XF86XK_Back,	TQt::Key_Back,
    XF86XK_Forward,	TQt::Key_Forward,
    XF86XK_Stop,	TQt::Key_Stop,
    XF86XK_Refresh,	TQt::Key_Refresh,
    XF86XK_Favorites,	TQt::Key_Favorites,
    XF86XK_AudioMedia,	TQt::Key_LaunchMedia,
    XF86XK_OpenURL,	TQt::Key_OpenUrl,
    XF86XK_HomePage,	TQt::Key_HomePage,
    XF86XK_Search,	TQt::Key_Search,

    // media keys
    XF86XK_AudioLowerVolume, TQt::Key_VolumeDown,
    XF86XK_AudioMute,	TQt::Key_VolumeMute,
    XF86XK_AudioRaiseVolume, TQt::Key_VolumeUp,
    XF86XK_AudioPlay,	TQt::Key_MediaPlay,
    XF86XK_AudioStop,	TQt::Key_MediaStop,
    XF86XK_AudioPrev,	TQt::Key_MediaPrev,
    XF86XK_AudioNext,	TQt::Key_MediaNext,
    XF86XK_AudioRecord,	TQt::Key_MediaRecord,

    // launch keys
    XF86XK_Mail,	TQt::Key_LaunchMail,
    XF86XK_MyComputer,	TQt::Key_Launch0,
    XF86XK_Calculator,	TQt::Key_Launch1,
    XF86XK_Standby, 	TQt::Key_Standby,

    XF86XK_Launch0,	TQt::Key_Launch2,
    XF86XK_Launch1,	TQt::Key_Launch3,
    XF86XK_Launch2,	TQt::Key_Launch4,
    XF86XK_Launch3,	TQt::Key_Launch5,
    XF86XK_Launch4,	TQt::Key_Launch6,
    XF86XK_Launch5,	TQt::Key_Launch7,
    XF86XK_Launch6,	TQt::Key_Launch8,
    XF86XK_Launch7,	TQt::Key_Launch9,
    XF86XK_Launch8,	TQt::Key_LaunchA,
    XF86XK_Launch9,	TQt::Key_LaunchB,
    XF86XK_LaunchA,	TQt::Key_LaunchC,
    XF86XK_LaunchB,	TQt::Key_LaunchD,
    XF86XK_LaunchC,	TQt::Key_LaunchE,
    XF86XK_LaunchD,	TQt::Key_LaunchF,
    XF86XK_MonBrightnessUp,	TQt::Key_MonBrightnessUp,
    XF86XK_MonBrightnessDown,	TQt::Key_MonBrightnessDown,
    XF86XK_KbdLightOnOff,	TQt::Key_KeyboardLightOnOff,
    XF86XK_KbdBrightnessUp,	TQt::Key_KeyboardBrightnessUp,
    XF86XK_KbdBrightnessDown,	TQt::Key_KeyboardBrightnessDown,

    0,			0
};


static TQIntDict<void>    *keyDict  = 0;
static TQIntDict<void>    *textDict = 0;

static void deleteKeyDicts()
{
    if ( keyDict )
	delete keyDict;
    keyDict = 0;
    if ( textDict )
	delete textDict;
    textDict = 0;
}

#if !defined(TQT_NO_XIM)
static const unsigned short katakanaKeysymsToUnicode[] = {
    0x0000, 0x3002, 0x300C, 0x300D, 0x3001, 0x30FB, 0x30F2, 0x30A1,
    0x30A3, 0x30A5, 0x30A7, 0x30A9, 0x30E3, 0x30E5, 0x30E7, 0x30C3,
    0x30FC, 0x30A2, 0x30A4, 0x30A6, 0x30A8, 0x30AA, 0x30AB, 0x30AD,
    0x30AF, 0x30B1, 0x30B3, 0x30B5, 0x30B7, 0x30B9, 0x30BB, 0x30BD,
    0x30BF, 0x30C1, 0x30C4, 0x30C6, 0x30C8, 0x30CA, 0x30CB, 0x30CC,
    0x30CD, 0x30CE, 0x30CF, 0x30D2, 0x30D5, 0x30D8, 0x30DB, 0x30DE,
    0x30DF, 0x30E0, 0x30E1, 0x30E2, 0x30E4, 0x30E6, 0x30E8, 0x30E9,
    0x30EA, 0x30EB, 0x30EC, 0x30ED, 0x30EF, 0x30F3, 0x309B, 0x309C
};

static const unsigned short cyrillicKeysymsToUnicode[] = {
    0x0000, 0x0452, 0x0453, 0x0451, 0x0454, 0x0455, 0x0456, 0x0457,
    0x0458, 0x0459, 0x045a, 0x045b, 0x045c, 0x0000, 0x045e, 0x045f,
    0x2116, 0x0402, 0x0403, 0x0401, 0x0404, 0x0405, 0x0406, 0x0407,
    0x0408, 0x0409, 0x040a, 0x040b, 0x040c, 0x0000, 0x040e, 0x040f,
    0x044e, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
    0x0445, 0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e,
    0x043f, 0x044f, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432,
    0x044c, 0x044b, 0x0437, 0x0448, 0x044d, 0x0449, 0x0447, 0x044a,
    0x042e, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413,
    0x0425, 0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e,
    0x041f, 0x042f, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412,
    0x042c, 0x042b, 0x0417, 0x0428, 0x042d, 0x0429, 0x0427, 0x042a
};

static const unsigned short greekKeysymsToUnicode[] = {
    0x0000, 0x0386, 0x0388, 0x0389, 0x038a, 0x03aa, 0x0000, 0x038c,
    0x038e, 0x03ab, 0x0000, 0x038f, 0x0000, 0x0000, 0x0385, 0x2015,
    0x0000, 0x03ac, 0x03ad, 0x03ae, 0x03af, 0x03ca, 0x0390, 0x03cc,
    0x03cd, 0x03cb, 0x03b0, 0x03ce, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397,
    0x0398, 0x0399, 0x039a, 0x039b, 0x039c, 0x039d, 0x039e, 0x039f,
    0x03a0, 0x03a1, 0x03a3, 0x0000, 0x03a4, 0x03a5, 0x03a6, 0x03a7,
    0x03a8, 0x03a9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7,
    0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf,
    0x03c0, 0x03c1, 0x03c3, 0x03c2, 0x03c4, 0x03c5, 0x03c6, 0x03c7,
    0x03c8, 0x03c9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

static const unsigned short technicalKeysymsToUnicode[] = {
    0x0000, 0x23B7, 0x250C, 0x2500, 0x2320, 0x2321, 0x2502, 0x23A1,
    0x23A3, 0x23A4, 0x23A6, 0x239B, 0x239D, 0x239E, 0x23A0, 0x23A8,
    0x23AC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x2264, 0x2260, 0x2265, 0x222B,
    0x2234, 0x221D, 0x221E, 0x0000, 0x0000, 0x2207, 0x0000, 0x0000,
    0x223C, 0x2243, 0x0000, 0x0000, 0x0000, 0x21D4, 0x21D2, 0x2261,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x221A, 0x0000,
    0x0000, 0x0000, 0x2282, 0x2283, 0x2229, 0x222A, 0x2227, 0x2228,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2202,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0192, 0x0000,
    0x0000, 0x0000, 0x0000, 0x2190, 0x2191, 0x2192, 0x2193, 0x0000
};

static const unsigned short specialKeysymsToUnicode[] = {
    0x25C6, 0x2592, 0x2409, 0x240C, 0x240D, 0x240A, 0x0000, 0x0000,
    0x2424, 0x240B, 0x2518, 0x2510, 0x250C, 0x2514, 0x253C, 0x23BA,
    0x23BB, 0x2500, 0x23BC, 0x23BD, 0x251C, 0x2524, 0x2534, 0x252C,
    0x2502, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

static const unsigned short publishingKeysymsToUnicode[] = {
    0x0000, 0x2003, 0x2002, 0x2004, 0x2005, 0x2007, 0x2008, 0x2009,
    0x200a, 0x2014, 0x2013, 0x0000, 0x0000, 0x0000, 0x2026, 0x2025,
    0x2153, 0x2154, 0x2155, 0x2156, 0x2157, 0x2158, 0x2159, 0x215a,
    0x2105, 0x0000, 0x0000, 0x2012, 0x2329, 0x0000, 0x232a, 0x0000,
    0x0000, 0x0000, 0x0000, 0x215b, 0x215c, 0x215d, 0x215e, 0x0000,
    0x0000, 0x2122, 0x2613, 0x0000, 0x25c1, 0x25b7, 0x25cb, 0x25af,
    0x2018, 0x2019, 0x201c, 0x201d, 0x211e, 0x0000, 0x2032, 0x2033,
    0x0000, 0x271d, 0x0000, 0x25ac, 0x25c0, 0x25b6, 0x25cf, 0x25ae,
    0x25e6, 0x25ab, 0x25ad, 0x25b3, 0x25bd, 0x2606, 0x2022, 0x25aa,
    0x25b2, 0x25bc, 0x261c, 0x261e, 0x2663, 0x2666, 0x2665, 0x0000,
    0x2720, 0x2020, 0x2021, 0x2713, 0x2717, 0x266f, 0x266d, 0x2642,
    0x2640, 0x260e, 0x2315, 0x2117, 0x2038, 0x201a, 0x201e, 0x0000
};

static const unsigned short aplKeysymsToUnicode[] = {
    0x0000, 0x0000, 0x0000, 0x003c, 0x0000, 0x0000, 0x003e, 0x0000,
    0x2228, 0x2227, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x00af, 0x0000, 0x22a5, 0x2229, 0x230a, 0x0000, 0x005f, 0x0000,
    0x0000, 0x0000, 0x2218, 0x0000, 0x2395, 0x0000, 0x22a4, 0x25cb,
    0x0000, 0x0000, 0x0000, 0x2308, 0x0000, 0x0000, 0x222a, 0x0000,
    0x2283, 0x0000, 0x2282, 0x0000, 0x22a2, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x22a3, 0x0000, 0x0000, 0x0000
};

static const unsigned short koreanKeysymsToUnicode[] = {
    0x0000, 0x3131, 0x3132, 0x3133, 0x3134, 0x3135, 0x3136, 0x3137,
    0x3138, 0x3139, 0x313a, 0x313b, 0x313c, 0x313d, 0x313e, 0x313f,
    0x3140, 0x3141, 0x3142, 0x3143, 0x3144, 0x3145, 0x3146, 0x3147,
    0x3148, 0x3149, 0x314a, 0x314b, 0x314c, 0x314d, 0x314e, 0x314f,
    0x3150, 0x3151, 0x3152, 0x3153, 0x3154, 0x3155, 0x3156, 0x3157,
    0x3158, 0x3159, 0x315a, 0x315b, 0x315c, 0x315d, 0x315e, 0x315f,
    0x3160, 0x3161, 0x3162, 0x3163, 0x11a8, 0x11a9, 0x11aa, 0x11ab,
    0x11ac, 0x11ad, 0x11ae, 0x11af, 0x11b0, 0x11b1, 0x11b2, 0x11b3,
    0x11b4, 0x11b5, 0x11b6, 0x11b7, 0x11b8, 0x11b9, 0x11ba, 0x11bb,
    0x11bc, 0x11bd, 0x11be, 0x11bf, 0x11c0, 0x11c1, 0x11c2, 0x316d,
    0x3171, 0x3178, 0x317f, 0x3181, 0x3184, 0x3186, 0x318d, 0x318e,
    0x11eb, 0x11f0, 0x11f9, 0x0000, 0x0000, 0x0000, 0x0000, 0x20a9
};


static TQChar keysymToUnicode(unsigned char byte3, unsigned char byte4)
{
    if ( byte3 == 0x04 ) {
        // katakana
        if ( byte4 > 0xa0 && byte4 < 0xe0 )
           return TQChar( katakanaKeysymsToUnicode[byte4 - 0xa0] );
        else if ( byte4 == 0x7e )
            return TQChar( 0x203e ); // Overline
    } else if ( byte3 == 0x06 ) {
	// russian, use lookup table
	if ( byte4 > 0xa0 )
	    return TQChar( cyrillicKeysymsToUnicode[byte4 - 0xa0] );
    } else if ( byte3 == 0x07 ) {
	// greek
	if ( byte4 > 0xa0 )
	    return TQChar( greekKeysymsToUnicode[byte4 - 0xa0] );
    } else if ( byte3 == 0x08 ) {
       // technical
       if ( byte4 > 0xa0 )
           return TQChar( technicalKeysymsToUnicode[byte4 - 0xa0] );
    } else if ( byte3 == 0x09 ) {
       // special
       if ( byte4 >= 0xe0 )
           return TQChar( specialKeysymsToUnicode[byte4 - 0xe0] );
    } else if ( byte3 == 0x0a ) {
       // publishing
       if ( byte4 > 0xa0 )
           return TQChar( publishingKeysymsToUnicode[byte4 - 0xa0] );
    } else if ( byte3 == 0x0b ) {
       // APL
       if ( byte4 > 0xa0 )
           return TQChar( aplKeysymsToUnicode[byte4 - 0xa0] );
    } else if ( byte3 == 0x0e ) {
       // Korean
       if ( byte4 > 0xa0 )
           return TQChar( koreanKeysymsToUnicode[byte4 - 0xa0] );
    }
    return TQChar(0x0);
}
#endif


bool TQETWidget::translateKeyEventInternal( const XEvent *event, int& count,
					   TQString& text,
					   int& state,
					   char& ascii, int& code, TQEvent::Type &type, bool willRepeat, bool statefulTranslation )
{
    TQTextCodec *mapper = tqt_input_mapper;
    // some XmbLookupString implementations don't return buffer overflow correctly,
    // so we increase the input buffer to allow for long strings...
    // 256 chars * 2 bytes + 1 null-term == 513 bytes
    TQCString chars(513);
    TQChar converted;
    KeySym key = 0;

    if ( !keyDict ) {
	keyDict = new TQIntDict<void>( 13 );
	keyDict->setAutoDelete( false );
	textDict = new TQIntDict<void>( 13 );
	textDict->setAutoDelete( false );
	tqAddPostRoutine( deleteKeyDicts );
    }

    XKeyEvent xkeyevent = event->xkey;

    // save the modifier state, we will use the keystate uint later by passing
    // it to tqt_x11_translateButtonState
    uint keystate = event->xkey.state;
    // remove the modifiers where mode_switch exists... some machines seem
    // to have alt *AND* mode_switch both in Mod1Mask, which causes
    // XLookupString to return things like '�' (aring) for ALT-A.  This
    // completely breaks modifiers.  If we remove the modifier for Mode_switch,
    // then things work correctly...
    xkeyevent.state &= ~tqt_mode_switch_remove_mask;

    type = (event->type == XKeyPress)
           ? TQEvent::KeyPress : TQEvent::KeyRelease;
#if defined(TQT_NO_XIM)

    count = XLookupString( &xkeyevent, chars.data(), chars.size(), &key, 0 );

    if ( count == 1 )
	ascii = chars[0];

#else
    // Implementation for X11R5 and newer, using XIM

    int	       keycode = event->xkey.keycode;

    if ( type == TQEvent::KeyPress ) {
	bool mb=false;
	// commit string handling is done by
	// TQXIMInputContext::x11FilterEvent() and are passed to
	// widgets via TQIMEvent regardless of XIM style, so the
	// following code is commented out.
#if 0
	if ( tqt_xim ) {
	    TQTLWExtra*  xd = tlw->topData();
	    TQInputContext *qic = (TQInputContext *) xd->xic;
	    if ( qic ) {
		mb=true;
		count = qic->lookupString(&xkeyevent, chars, &key, &status);
	    }
	}
#endif
	if ( !mb ) {
	    count = XLookupString( &xkeyevent,
				   chars.data(), chars.size(), &key, 0 );
	}
	if ( count && !keycode ) {
	    keycode = tqt_ximComposingKeycode;
	    tqt_ximComposingKeycode = 0;
	}
	if ( key )
	    keyDict->replace( keycode, (void*)key );
	// all keysyms smaller than that are actally keys that can be mapped
	// to unicode chars
	if ( count == 0 && key < 0xff00 ) {
	    unsigned char byte3 = (unsigned char )(key >> 8);
	    int mib = -1;
	    switch( byte3 ) {
	    case 0: // Latin 1
	    case 1: // Latin 2
	    case 2: //latin 3
	    case 3: // latin4
		mib = byte3 + 4; break;
	    case 5: // arabic
		mib = 82; break;
	    case 12: // Hebrew
		mib = 85; break;
	    case 13: // Thai
		mib = 2259; break;
	    case 4: // kana
	    case 6: // cyrillic
	    case 7: // greek
	    case 8: // technical, no mapping here at the moment
	    case 9: // Special
	    case 10: // Publishing
	    case 11: // APL
	    case 14: // Korean, no mapping
		mib = -1; // manual conversion
		mapper = 0;
		converted = keysymToUnicode( byte3, key & 0xff );
	    case 0x20:
		// currency symbols
		if ( key >= 0x20a0 && key <= 0x20ac ) {
		    mib = -1; // manual conversion
		    mapper = 0;
		    converted = (uint)key;
		}
		break;
	    default:
		break;
	    }
	    if ( mib != -1 ) {
		mapper = TQTextCodec::codecForMib( mib );
		chars[0] = (unsigned char) (key & 0xff); // get only the fourth bit for conversion later
		count++;
	    }
	} else if ( key >= 0x1000000 && key <= 0x100ffff ) {
	    converted = (ushort) (key - 0x1000000);
	    mapper = 0;
	}
	if ( count < (int)chars.size()-1 )
	    chars[count] = '\0';
	if ( count == 1 ) {
	    ascii = chars[0];
	    // +256 so we can store all eight-bit codes, including ascii 0,
	    // and independent of whether char is signed or not.
	    textDict->replace( keycode, (void*)(long)(256+ascii) );
	}
    } else {
	key = (int)(long)keyDict->find( keycode );
	if ( key )
	    if( !willRepeat && statefulTranslation ) // Take out key of dictionary only if this call.
		keyDict->take( keycode );
	long s = (long)textDict->find( keycode );
	if ( s ) {
	     if( statefulTranslation )
		textDict->take( keycode );
	    ascii = (char)(s-256);
	}
    }
#endif // !TQT_NO_XIM

    state = tqt_x11_translateButtonState( keystate );

    static int directionKeyEvent = 0;
    static unsigned int lastWinId = 0;
    if ( tqt_use_rtl_extensions && type == TQEvent::KeyRelease && statefulTranslation ) {
	if (directionKeyEvent == Key_Direction_R || directionKeyEvent == Key_Direction_L ) {
	    type = TQEvent::KeyPress;
	    code = directionKeyEvent;
	    chars[0] = 0;
	    directionKeyEvent = 0;
	    lastWinId = 0;
	    return true;
	} else {
	    directionKeyEvent = 0;
	    lastWinId = 0;
	}
    }

    // Watch for keypresses and if its a key belonging to the Ctrl-Shift
    // direction-changing accel, remember it.
    // We keep track of those keys instead of using the event's state
    // (to figure out whether the Ctrl modifier is held while Shift is pressed,
    // or Shift is held while Ctrl is pressed) since the 'state' doesn't tell
    // us whether the modifier held is Left or Right.
    if ( tqt_use_rtl_extensions && type  == TQEvent::KeyPress && statefulTranslation ) {
        if (key == XK_Control_L || key == XK_Control_R || key == XK_Shift_L || key == XK_Shift_R) {
	    if (!directionKeyEvent) {
	      directionKeyEvent = key;
	      // This code exists in order to check that 
	      // the event is occurred in the same widget.
	      lastWinId = winId();
	    }
        } else {
           // this can no longer be a direction-changing accel.
	   // if any other key was pressed.
           directionKeyEvent = Key_Space;
        }
	}

    // Commentary in X11/keysymdef says that X codes match ASCII, so it
    // is safe to use the locale functions to process X codes in ISO8859-1.
    //
    // This is mainly for compatibility - applications should not use the
    // TQt keycodes between 128 and 255, but should rather use the
    // TQKeyEvent::text().
    //
    if ( key < 128 || (key < 256 && (!tqt_input_mapper || tqt_input_mapper->mibEnum()==4)) ) {
	code = isprint((int)key) ? toupper((int)key) : 0; // upper-case key, if known
    } else if ( key >= XK_F1 && key <= XK_F35 ) {
	code = Key_F1 + ((int)key - XK_F1);	// function keys
    } else if ( key >= XK_KP_0 && key <= XK_KP_9) {
	code = Key_0 + ((int)key - XK_KP_0);	// numeric keypad keys
	state |= Keypad;
    } else {
	int i = 0;				// any other keys
	while ( KeyTbl[i] ) {
	    if ( key == KeyTbl[i] ) {
		code = (int)KeyTbl[i+1];
		break;
	    }
	    i += 2;
	}
	switch ( key ) {
	case XK_KP_Insert:
	case XK_KP_Delete:
	case XK_KP_Home:
	case XK_KP_End:
	case XK_KP_Left:
	case XK_KP_Up:
	case XK_KP_Right:
	case XK_KP_Down:
	case XK_KP_Prior:
	case XK_KP_Next:
	case XK_KP_Space:
	case XK_KP_Tab:
	case XK_KP_Enter:
	case XK_KP_Equal:
	case XK_KP_Multiply:
	case XK_KP_Add:
	case XK_KP_Separator:
	case XK_KP_Subtract:
	case XK_KP_Decimal:
	case XK_KP_Divide:
	    state |= Keypad;
	    break;
	default:
	    break;
	}

	if ( code == Key_Tab &&
	     (state & ShiftButton) == ShiftButton ) {
            // map shift+tab to shift+backtab, TQAccel knows about it
            // and will handle it.
	    code = Key_Backtab;
	    chars[0] = 0;
	}

	if ( tqt_use_rtl_extensions && type  == TQEvent::KeyPress && statefulTranslation ) {
	    if ( directionKeyEvent && lastWinId == winId() ) {
		if ( ( key == XK_Shift_L && directionKeyEvent == XK_Control_L ) ||
		     ( key == XK_Control_L && directionKeyEvent == XK_Shift_L ) ) {
		    directionKeyEvent = Key_Direction_L;
		} else if ( ( key == XK_Shift_R && directionKeyEvent == XK_Control_R ) ||
			    ( key == XK_Control_R && directionKeyEvent == XK_Shift_R ) ) {
		    directionKeyEvent = Key_Direction_R;
		}
	    }
	    else if ( directionKeyEvent == Key_Direction_L || directionKeyEvent == Key_Direction_R ) {
		directionKeyEvent = Key_Space; // invalid
	    }
	}
    }

#if 0
#ifndef Q_EE
    static int c  = 0;
    extern void tqt_dialog_default_key();
#define Q_EE(x) c = (c == x || (!c && x == 0x1000) )? x+1 : 0
    if ( tlw && state == '0' ) {
	switch ( code ) {
	case 0x4f: Q_EE(Key_Backtab); break;
	case 0x52: Q_EE(Key_Tab); break;
	case 0x54: Q_EE(Key_Escape); break;
	case 0x4c:
	    if (c == Key_Return )
		tqt_dialog_default_key();
	    else
		Q_EE(Key_Backspace);
	    break;
	}
    }
#undef Q_EE
#endif
#endif

    // convert chars (8bit) to text (unicode).
    if ( mapper )
	text = mapper->toUnicode(chars,count);
    else if ( !mapper && converted.unicode() != 0x0 )
	text = converted;
    else
	text = chars;
    return true;
}


struct tqt_auto_repeat_data
{
    // match the window and keycode with timestamp delta of 10ms
    Window window;
    KeyCode keycode;
    Time timestamp;

    // queue scanner state
    bool release;
    bool error;
};

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static Bool tqt_keypress_scanner(Display *, XEvent *event, XPointer arg)
{
    if (event->type != XKeyPress && event->type != XKeyRelease)
        return false;

    tqt_auto_repeat_data *d = (tqt_auto_repeat_data *) arg;
    if (d->error ||
        event->xkey.window  != d->window ||
        event->xkey.keycode != d->keycode) {
        d->error = true;
        return false;
    }

    if (event->type == XKeyPress) {
        d->error = (! d->release || event->xkey.time - d->timestamp > 10);
        return (! d->error);
    }

    // must be XKeyRelease event
    if (d->release) {
        // found a second release
        d->error = true;
        return false;
    }

    // found a single release
    d->release = true;
    d->timestamp = event->xkey.time;

    return false;
}

static Bool tqt_keyrelease_scanner(Display *, XEvent *event, XPointer arg)
{
    const tqt_auto_repeat_data *d = (const tqt_auto_repeat_data *) arg;
    return (event->type == XKeyRelease &&
            event->xkey.window  == d->window &&
            event->xkey.keycode == d->keycode);
}

#if defined(Q_C_CALLBACKS)
}
#endif

bool TQETWidget::translateKeyEvent( const XEvent *event, bool grab )
{
    int	   code = -1;
    int	   count = 0;
    int	   state;
    char   ascii = 0;

    if ( sm_blockUserInput ) // block user interaction during session management
	return true;

    Display *dpy = x11Display();

    if ( !isEnabled() )
	return true;

    TQEvent::Type type;
    bool    autor = false;
    TQString text;

    translateKeyEventInternal( event, count, text, state, ascii, code, type,
			       tqt_mode_switch_remove_mask != 0 );

    static uint curr_autorep = 0;
    // was this the last auto-repeater?
    tqt_auto_repeat_data auto_repeat_data;
    auto_repeat_data.window = event->xkey.window;
    auto_repeat_data.keycode = event->xkey.keycode;
    auto_repeat_data.timestamp = event->xkey.time;

    if ( event->type == XKeyPress ) {
        if ( curr_autorep == event->xkey.keycode ) {
            autor = true;
            curr_autorep = 0;
        }
    } else {
	// look ahead for auto-repeat
        XEvent nextpress;

        auto_repeat_data.release = true;
        auto_repeat_data.error = false;
        if (XCheckIfEvent(dpy, &nextpress, &tqt_keypress_scanner,
                          (XPointer) &auto_repeat_data)) {
            autor = true;

	    // Put it back... we COULD send the event now and not need
	    // the static curr_autorep variable.
	    XPutBackEvent(dpy,&nextpress);
	}
	curr_autorep = autor ? event->xkey.keycode : 0;
    }

    // process accelerators before doing key compression
    if ( type == TQEvent::KeyPress && !grab ) {
	// send accel events if the keyboard is not grabbed
	TQKeyEvent a( type, code, ascii, state, text, autor,
		     TQMAX( TQMAX(count,1), int(text.length())) );
	if ( tqt_tryAccelEvent( this, &a ) )
	    return true;
    }

    long save = 0;
    if ( tqt_mode_switch_remove_mask != 0 ) {
	save = tqt_mode_switch_remove_mask;
	tqt_mode_switch_remove_mask = 0;

	// translate the key event again, but this time apply any Mode_switch
	// modifiers
	translateKeyEventInternal( event, count, text, state, ascii, code, type );
    }

#ifndef TQT_NO_IM
    TQInputContext *qic = getInputContext();
#endif

    // compress keys
    if ( !text.isEmpty() && testWState(WState_CompressKeys) &&
#ifndef TQT_NO_IM
	 // Ordinary input methods require discrete key events to work
	 // properly, so key compression has to be disabled when input
	 // context exists.
	 //
	 // And further consideration, some complex input method
	 // require all key press/release events discretely even if
	 // the input method awares of key compression and compressed
	 // keys are ordinary alphabets. For example, the uim project
	 // is planning to implement "combinational shift" feature for
	 // a Japanese input method, uim-skk. It will work as follows.
	 //
	 // 1. press "r"
	 // 2. press "u"
	 // 3. release both "r" and "u" in arbitrary order
	 // 4. above key sequence generates "Ru"
	 //
	 // Of course further consideration about other participants
	 // such as key repeat mechanism is required to implement such
	 // feature.
	 ! qic &&
#endif // TQT_NO_IM
	 // do not compress keys if the key event we just got above matches
	 // one of the key ranges used to compute stopCompression
	 ! ( ( code >= Key_Escape && code <= Key_SysReq ) ||
	     ( code >= Key_Home && code <= Key_Next ) ||
	     ( code >= Key_Super_L && code <= Key_Direction_R ) ||
	     ( ( code == 0 ) && ( ascii == '\n' ) ) ) ) {
	// the widget wants key compression so it gets it
	int	codeIntern = -1;
	int	countIntern = 0;
	int	stateIntern;
	char	asciiIntern = 0;
	XEvent	evRelease;
	XEvent	evPress;

	// sync the event queue, this makes key compress work better
	XSync( dpy, false );

	for (;;) {
	    TQString textIntern;
	    if ( !XCheckTypedWindowEvent(dpy,event->xkey.window,
					 XKeyRelease,&evRelease) )
		break;
	    if ( !XCheckTypedWindowEvent(dpy,event->xkey.window,
					 XKeyPress,&evPress) ) {
		XPutBackEvent(dpy, &evRelease);
		break;
	    }
	    TQEvent::Type t;
	    translateKeyEventInternal( &evPress, countIntern, textIntern,
				       stateIntern, asciiIntern, codeIntern, t );
	    // use stopCompression to stop key compression for the following
	    // key event ranges:
	    bool stopCompression =
		// 1) misc keys
		( codeIntern >= Key_Escape && codeIntern <= Key_SysReq ) ||
		// 2) cursor movement
		( codeIntern >= Key_Home && codeIntern <= Key_Next ) ||
		// 3) extra keys
		( codeIntern >= Key_Super_L && codeIntern <= Key_Direction_R ) ||
		// 4) something that a) doesn't translate to text or b) translates
		//    to newline text
		((codeIntern == 0) && (asciiIntern == '\n'));
	    if (stateIntern == state && !textIntern.isEmpty() && !stopCompression) {
		text += textIntern;
		count += countIntern;
	    } else {
		XPutBackEvent(dpy, &evPress);
		XPutBackEvent(dpy, &evRelease);
		break;
	    }
	}
    }

    if ( save != 0 )
	tqt_mode_switch_remove_mask = save;

    // autorepeat compression makes sense for all widgets (Windows
    // does it automatically .... )
    if ( event->type == XKeyPress && text.length() <= 1
#ifndef TQT_NO_IM
	 // input methods need discrete key events
	 && ! qic
#endif// TQT_NO_IM
	 ) {
	XEvent dummy;

        for (;;) {
            auto_repeat_data.release = false;
            auto_repeat_data.error = false;
            if (! XCheckIfEvent(dpy, &dummy, &tqt_keypress_scanner,
                                (XPointer) &auto_repeat_data))
                break;
            if (! XCheckIfEvent(dpy, &dummy, &tqt_keyrelease_scanner,
                                (XPointer) &auto_repeat_data))
                break;

	    count++;
	    if (!text.isEmpty())
		text += text[0];
	}
    }

    if (code == 0 && ascii == '\n') {
	code = Key_Return;
	ascii = '\r';
	text = "\r";
    }

    // try the menukey first
    if ( type == TQEvent::KeyPress && code == TQt::Key_Menu ) {
	TQContextMenuEvent e( TQContextMenuEvent::Keyboard, TQPoint( 5, 5 ), mapToGlobal( TQPoint( 5, 5 ) ), 0 );
	TQApplication::sendSpontaneousEvent( this, &e );
	if( e.isAccepted() )
	    return true;
    }

    TQKeyEvent e( type, code, ascii, state, text, autor,
		 TQMAX(TQMAX(count,1), int(text.length())) );
    return TQApplication::sendSpontaneousEvent( this, &e );
}


//
// Paint event translation
//
// When receiving many expose events, we compress them (union of all expose
// rectangles) into one event which is sent to the widget.

struct PaintEventInfo {
    Window window;
};

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static Bool isPaintOrScrollDoneEvent( Display *, XEvent *ev, XPointer a )
{
    PaintEventInfo *info = (PaintEventInfo *)a;
    if ( ev->type == Expose || ev->type == GraphicsExpose
      ||  ( ev->type == ClientMessage
	 && ev->xclient.message_type == tqt_scrolldone ) )
    {
	if ( ev->xexpose.window == info->window )
	    return True;
    }
    return False;
}

#if defined(Q_C_CALLBACKS)
}
#endif


// declared above: static TQPtrList<TQScrollInProgress> *sip_list = 0;

void tqt_insert_sip( TQWidget* scrolled_widget, int dx, int dy )
{
    if ( !sip_list ) {
	sip_list = new TQPtrList<TQScrollInProgress>;
	sip_list->setAutoDelete( true );
    }

    TQScrollInProgress* sip = new TQScrollInProgress( scrolled_widget, dx, dy );
    sip_list->append( sip );

    XClientMessageEvent client_message = {};
    client_message.type = ClientMessage;
    client_message.window = scrolled_widget->winId();
    client_message.format = 32;
    client_message.message_type = tqt_scrolldone;
    client_message.data.l[0] = sip->id;

    XSendEvent( appDpy, scrolled_widget->winId(), False, NoEventMask,
	(XEvent*)&client_message );
}

int tqt_sip_count( TQWidget* scrolled_widget )
{
    if ( !sip_list )
	return 0;

    int sips=0;

    for (TQScrollInProgress* sip = sip_list->first();
	sip; sip=sip_list->next())
    {
	if ( sip->scrolled_widget == scrolled_widget )
	    sips++;
    }

    return sips;
}

static
bool translateBySips( TQWidget* that, TQRect& paintRect )
{
    if ( sip_list ) {
	int dx=0, dy=0;
	int sips=0;
	for (TQScrollInProgress* sip = sip_list->first();
	    sip; sip=sip_list->next())
	{
	    if ( sip->scrolled_widget == that ) {
		if ( sips ) {
		    dx += sip->dx;
		    dy += sip->dy;
		}
		sips++;
	    }
	}
	if ( sips > 1 ) {
	    paintRect.moveBy( dx, dy );
	    return true;
	}
    }
    return false;
}

bool TQETWidget::translatePaintEvent( const XEvent *event )
{
    setWState( WState_Exposed );
    TQRect  paintRect( event->xexpose.x,	   event->xexpose.y,
		      event->xexpose.width, event->xexpose.height );
    bool   merging_okay = !testWFlags(WPaintClever);
    XEvent xevent;
    PaintEventInfo info;
    info.window = winId();
    bool should_clip = translateBySips( this, paintRect );

    TQRegion paintRegion( paintRect );

    if ( merging_okay ) {
	// WARNING: this is O(number_of_events * number_of_matching_events)
	while ( XCheckIfEvent(x11Display(),&xevent,isPaintOrScrollDoneEvent,
			      (XPointer)&info) &&
		!tqt_x11EventFilter(&xevent)  &&
		!x11Event( &xevent ) ) // send event through filter
	{
	    if ( xevent.type == Expose || xevent.type == GraphicsExpose ) {
		TQRect exposure(xevent.xexpose.x,
			       xevent.xexpose.y,
			       xevent.xexpose.width,
			       xevent.xexpose.height);
		if ( translateBySips( this, exposure ) )
		    should_clip = true;
		paintRegion = paintRegion.unite( exposure );
	    } else {
		translateScrollDoneEvent( &xevent );
	    }
	}
    }

    if ( should_clip ) {
	paintRegion = paintRegion.intersect( rect() );
	if ( paintRegion.isEmpty() )
	    return true;
    }

    TQPaintEvent e( paintRegion );
    setWState( WState_InPaintEvent );
    if ( !isTopLevel() && backgroundOrigin() != WidgetOrigin )
	erase( paintRegion );
    tqt_set_paintevent_clipping( this, paintRegion );
    TQApplication::sendSpontaneousEvent( this, &e );
    tqt_clear_paintevent_clipping();
    clearWState( WState_InPaintEvent );
    return true;
}

//
// Scroll-done event translation.
//

bool TQETWidget::translateScrollDoneEvent( const XEvent *event )
{
    if ( !sip_list ) return false;

    long id = event->xclient.data.l[0];

    // Remove any scroll-in-progress record for the given id.
    for (TQScrollInProgress* sip = sip_list->first(); sip; sip=sip_list->next()) {
	if ( sip->id == id ) {
	    sip_list->remove( sip_list->current() );
	    return true;
	}
    }

    return false;
}

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif
#ifndef TQT_NO_XSYNC
static Bool tqt_net_wm_sync_request_scanner(Display*, XEvent* event, XPointer arg)
{
    return (event->type == ClientMessage && event->xclient.window == *(Window*)arg
        && event->xclient.message_type == tqt_wm_protocols
        && ((unsigned int)event->xclient.data.l[ 0 ]) == tqt_net_wm_sync_request );
}
#endif

#if defined(Q_C_CALLBACKS)
}
#endif

//
// ConfigureNotify (window move and resize) event translation

bool TQETWidget::translateConfigEvent( const XEvent *event )
{
    // config pending is only set on resize, see qwidget_x11.cpp, internalSetGeometry()
    bool was_resize = testWState( WState_ConfigPending );

    clearWState(WState_ConfigPending);

    if ( isTopLevel() ) {
	TQPoint newCPos( geometry().topLeft() );
	TQSize  newSize( event->xconfigure.width, event->xconfigure.height );

	bool trust = (topData()->parentWinId == None ||
		      topData()->parentWinId == TQPaintDevice::x11AppRootWindow());

	if (event->xconfigure.send_event || trust ) {
	    // if a ConfigureNotify comes from a real sendevent request, we can
	    // trust its values.
	    newCPos.rx() = event->xconfigure.x + event->xconfigure.border_width;
	    newCPos.ry() = event->xconfigure.y + event->xconfigure.border_width;
	}

	if ( isVisible() )
	    TQApplication::syncX();

        if (! extra || extra->compress_events) {
            // ConfigureNotify compression for faster opaque resizing
            XEvent otherEvent;
            int compressed_configs = 0;
            while ( XCheckTypedWindowEvent( x11Display(), winId(), ConfigureNotify,
                                            &otherEvent ) ) {
                if ( tqt_x11EventFilter( &otherEvent ) )
                    continue;

                if (x11Event( &otherEvent ) )
                    continue;

                if ( otherEvent.xconfigure.event != otherEvent.xconfigure.window )
                    continue;

                newSize.setWidth( otherEvent.xconfigure.width );
                newSize.setHeight( otherEvent.xconfigure.height );

                if ( otherEvent.xconfigure.send_event || trust ) {
                    newCPos.rx() = otherEvent.xconfigure.x +
                                   otherEvent.xconfigure.border_width;
                    newCPos.ry() = otherEvent.xconfigure.y +
                                   otherEvent.xconfigure.border_width;
                }
                ++compressed_configs;
            }
#ifndef TQT_NO_XSYNC
            // _NET_WM_SYNC_REQUEST compression 
            Window wid = winId();
            while ( compressed_configs &&
                    XCheckIfEvent( x11Display(), &otherEvent,
                    tqt_net_wm_sync_request_scanner, (XPointer)&wid ) ) {
                handleSyncRequest( (void*)&otherEvent );
                --compressed_configs;
            }
#endif
        }

	TQRect cr ( geometry() );
	if ( newSize != cr.size() ) { // size changed
	    was_resize = true;
	    TQSize oldSize = size();
	    cr.setSize( newSize );
	    crect = cr;

	    if ( isVisible() ) {
		TQResizeEvent e( newSize, oldSize );
		TQApplication::sendSpontaneousEvent( this, &e );
	    } else {
		TQResizeEvent * e = new TQResizeEvent( newSize, oldSize );
		TQApplication::postEvent( this, e );
	    }
	}

	if ( newCPos != cr.topLeft() ) { // compare with cpos (exluding frame)
	    TQPoint oldPos = geometry().topLeft();
	    cr.moveTopLeft( newCPos );
	    crect = cr;
	    if ( isVisible() ) {
		TQMoveEvent e( newCPos, oldPos ); // pos (including frame), not cpos
		TQApplication::sendSpontaneousEvent( this, &e );
	    } else {
		TQMoveEvent * e = new TQMoveEvent( newCPos, oldPos );
		TQApplication::postEvent( this, e );
	    }
	}
    } else {
	XEvent xevent;
	while ( XCheckTypedWindowEvent(x11Display(),winId(), ConfigureNotify,&xevent) &&
		!tqt_x11EventFilter(&xevent)  &&
		!x11Event( &xevent ) ) // send event through filter
	    ;
    }

    bool transbg = backgroundOrigin() != WidgetOrigin;
    // we ignore NorthWestGravity at the moment for reversed layout
    if ( transbg ||
	 (!testWFlags( WStaticContents ) &&
	  testWState( WState_Exposed ) && was_resize ) ||
	 TQApplication::reverseLayout() ) {
	// remove unnecessary paint events from the queue
	XEvent xevent;
	while ( XCheckTypedWindowEvent( x11Display(), winId(), Expose, &xevent ) &&
		! tqt_x11EventFilter( &xevent )  &&
		! x11Event( &xevent ) ) // send event through filter
	    ;
	repaint( !testWFlags(WResizeNoErase) || transbg );
    }

    incrementSyncCounter();

    return true;
}


//
// Close window event translation.
//
bool TQETWidget::translateCloseEvent( const XEvent * )
{
    return close(false);
}


/*!
    Sets the text cursor's flash (blink) time to \a msecs
    milliseconds. The flash time is the time required to display,
    invert and restore the caret display. Usually the text cursor is
    displayed for \a msecs/2 milliseconds, then hidden for \a msecs/2
    milliseconds, but this may vary.

    Note that on Microsoft Windows, calling this function sets the
    cursor flash time for all windows.

    \sa cursorFlashTime()
*/
void  TQApplication::setCursorFlashTime( int msecs )
{
    cursor_flash_time = msecs;
}


/*!
    Returns the text cursor's flash (blink) time in milliseconds. The
    flash time is the time required to display, invert and restore the
    caret display.

    The default value on X11 is 1000 milliseconds. On Windows, the
    control panel value is used.

    Widgets should not cache this value since it may be changed at any
    time by the user changing the global desktop settings.

    \sa setCursorFlashTime()
*/
int TQApplication::cursorFlashTime()
{
    return cursor_flash_time;
}

/*!
    Sets the time limit that distinguishes a double click from two
    consecutive mouse clicks to \a ms milliseconds.

    Note that on Microsoft Windows, calling this function sets the
    double click interval for all windows.

    \sa doubleClickInterval()
*/

void TQApplication::setDoubleClickInterval( int ms )
{
    mouse_double_click_time = ms;
}


/*!
    Returns the maximum duration for a double click.

    The default value on X11 is 400 milliseconds. On Windows, the
    control panel value is used.

    \sa setDoubleClickInterval()
*/

int TQApplication::doubleClickInterval()
{
    return mouse_double_click_time;
}


/*!
    Sets the number of lines to scroll when the mouse wheel is rotated
    to \a n.

    If this number exceeds the number of visible lines in a certain
    widget, the widget should interpret the scroll operation as a
    single page up / page down operation instead.

    \sa wheelScrollLines()
*/
void TQApplication::setWheelScrollLines( int n )
{
    wheel_scroll_lines = n;
}

/*!
    Returns the number of lines to scroll when the mouse wheel is
    rotated.

    \sa setWheelScrollLines()
*/
int TQApplication::wheelScrollLines()
{
    return wheel_scroll_lines;
}

/*!
    Enables the UI effect \a effect if \a enable is true, otherwise
    the effect will not be used.

    Note: All effects are disabled on screens running at less than
    16-bit color depth.

    \sa isEffectEnabled(), TQt::UIEffect, setDesktopSettingsAware()
*/
void TQApplication::setEffectEnabled( TQt::UIEffect effect, bool enable )
{
    switch (effect) {
    case UI_AnimateMenu:
	if ( enable ) fade_menu = false;
	animate_menu = enable;
	break;
    case UI_FadeMenu:
	if ( enable )
	    animate_menu = true;
	fade_menu = enable;
	break;
    case UI_AnimateCombo:
	animate_combo = enable;
	break;
    case UI_AnimateTooltip:
	if ( enable ) fade_tooltip = false;
	animate_tooltip = enable;
	break;
    case UI_FadeTooltip:
	if ( enable )
	    animate_tooltip = true;
	fade_tooltip = enable;
	break;
    case UI_AnimateToolBox:
	animate_toolbox = enable;
	break;
    default:
	animate_ui = enable;
	break;
    }
}

/*!
    Returns true if \a effect is enabled; otherwise returns false.

    By default, TQt will try to use the desktop settings. Call
    setDesktopSettingsAware(false) to prevent this.

    Note: All effects are disabled on screens running at less than
    16-bit color depth.

    \sa setEffectEnabled(), TQt::UIEffect
*/
bool TQApplication::isEffectEnabled( TQt::UIEffect effect )
{
    if ( TQColor::numBitPlanes() < 16 || !animate_ui )
	return false;

    switch( effect ) {
    case UI_AnimateMenu:
	return animate_menu;
    case UI_FadeMenu:
	return fade_menu;
    case UI_AnimateCombo:
	return animate_combo;
    case UI_AnimateTooltip:
	return animate_tooltip;
    case UI_FadeTooltip:
	return fade_tooltip;
    case UI_AnimateToolBox:
	return animate_toolbox;
    default:
	return animate_ui;
    }
}

/*****************************************************************************
  Session management support
 *****************************************************************************/

#ifndef TQT_NO_SM_SUPPORT

#include <X11/SM/SMlib.h>

class TQSessionManagerData
{
public:
    TQSessionManagerData( TQSessionManager* mgr, TQString& id, TQString& key )
	: sm( mgr ), sessionId( id ), sessionKey( key ) {}
    TQSessionManager* sm;
    TQStringList restartCommand;
    TQStringList discardCommand;
    TQString& sessionId;
    TQString& sessionKey;
    TQSessionManager::RestartHint restartHint;
};

class TQSmSocketReceiver : public TQObject
{
    TQ_OBJECT
public:
    TQSmSocketReceiver( int socket )
	: TQObject(0,0)
	{
	    TQSocketNotifier* sn = new TQSocketNotifier( socket, TQSocketNotifier::Read, this );
	    connect( sn, TQ_SIGNAL( activated(int) ), this, TQ_SLOT( socketActivated(int) ) );
	}

public slots:
     void socketActivated(int);
};


static SmcConn smcConnection = 0;
static bool sm_interactionActive;
static bool sm_smActive;
static int sm_interactStyle;
static int sm_saveType;
static bool sm_cancel;
// static bool sm_waitingForPhase2;  ### never used?!?
static bool sm_waitingForInteraction;
static bool sm_isshutdown;
// static bool sm_shouldbefast;  ### never used?!?
static bool sm_phase2;
static bool sm_in_phase2;

static TQSmSocketReceiver* sm_receiver = 0;

static void resetSmState();
static void sm_setProperty( const char* name, const char* type,
			    int num_vals, SmPropValue* vals);
static void sm_saveYourselfCallback( SmcConn smcConn, SmPointer clientData,
				  int saveType, Bool shutdown , int interactStyle, Bool fast);
static void sm_saveYourselfPhase2Callback( SmcConn smcConn, SmPointer clientData ) ;
static void sm_dieCallback( SmcConn smcConn, SmPointer clientData ) ;
static void sm_shutdownCancelledCallback( SmcConn smcConn, SmPointer clientData );
static void sm_saveCompleteCallback( SmcConn smcConn, SmPointer clientData );
static void sm_interactCallback( SmcConn smcConn, SmPointer clientData );
static void sm_performSaveYourself( TQSessionManagerData* );

static void resetSmState()
{
//    sm_waitingForPhase2 = false; ### never used?!?
    sm_waitingForInteraction = false;
    sm_interactionActive = false;
    sm_interactStyle = SmInteractStyleNone;
    sm_smActive = false;
    sm_blockUserInput = false;
    sm_isshutdown = false;
//    sm_shouldbefast = false; ### never used?!?
    sm_phase2 = false;
    sm_in_phase2 = false;
}


// theoretically it's possible to set several properties at once. For
// simplicity, however, we do just one property at a time
static void sm_setProperty( const char* name, const char* type,
			    int num_vals, SmPropValue* vals)
{
    if (num_vals ) {
      SmProp prop;
      prop.name = (char*)name;
      prop.type = (char*)type;
      prop.num_vals = num_vals;
      prop.vals = vals;

      SmProp* props[1];
      props[0] = &prop;
      SmcSetProperties( smcConnection, 1, props );
    }
    else {
      char* names[1];
      names[0] = (char*) name;
      SmcDeleteProperties( smcConnection, 1, names );
    }
}

static void sm_setProperty( const TQString& name, const TQString& value)
{
    SmPropValue prop;
    prop.length = value.length();
    prop.value = (SmPointer) value.latin1();
    sm_setProperty( name.latin1(), SmARRAY8, 1, &prop );
}

static void sm_setProperty( const TQString& name, const TQStringList& value)
{
    SmPropValue *prop = new SmPropValue[ value.count() ];
    int count = 0;
    for ( TQStringList::ConstIterator it = value.begin(); it != value.end(); ++it ) {
      prop[ count ].length = (*it).length();
      prop[ count ].value = (char*)(*it).latin1();
      ++count;
    }
    sm_setProperty( name.latin1(), SmLISTofARRAY8, count, prop );
    delete [] prop;
}


// workaround for broken libsm, see below
struct QT_smcConn {
    unsigned int save_yourself_in_progress : 1;
    unsigned int shutdown_in_progress : 1;
};

static void sm_saveYourselfCallback( SmcConn smcConn, SmPointer clientData,
				  int saveType, Bool shutdown , int interactStyle, Bool /*fast*/)
{
    if (smcConn != smcConnection )
	return;
    sm_cancel = false;
    sm_smActive = true;
    sm_isshutdown = shutdown;
    sm_saveType = saveType;
    sm_interactStyle = interactStyle;
//    sm_shouldbefast = fast; ### never used?!?

    // ugly workaround for broken libSM. libSM should do that _before_
    // actually invoking the callback in sm_process.c
    ( (QT_smcConn*)smcConn )->save_yourself_in_progress = true;
    if ( sm_isshutdown )
	( (QT_smcConn*)smcConn )->shutdown_in_progress = true;

    sm_performSaveYourself( (TQSessionManagerData*) clientData );
    if ( !sm_isshutdown ) // we cannot expect a confirmation message in that case
	resetSmState();
}

static void sm_performSaveYourself( TQSessionManagerData* smd )
{
    if ( sm_isshutdown )
	sm_blockUserInput = true;

    TQSessionManager* sm = smd->sm;

    // generate a new session key
    timeval tv;
    gettimeofday( &tv, 0 );
    smd->sessionKey  = TQString::number( tv.tv_sec ) + "_" + TQString::number(tv.tv_usec);

    // tell the session manager about our program in best POSIX style
    sm_setProperty( SmProgram, TQString( tqApp->argv()[0] ) );
    // tell the session manager about our user as well.
    struct passwd* entry = getpwuid( geteuid() );
    if ( entry )
	sm_setProperty( SmUserID, TQString::fromLatin1( entry->pw_name ) );

    // generate a restart and discard command that makes sense
    TQStringList restart;
    restart  << tqApp->argv()[0] << "-session" << smd->sessionId + "_" + smd->sessionKey;
    if (tqstricmp(tqAppName(), tqAppClass()) != 0)
	restart << "-name" << tqAppName();
    sm->setRestartCommand( restart );
    TQStringList discard;
    sm->setDiscardCommand( discard );

    switch ( sm_saveType ) {
    case SmSaveBoth:
	tqApp->commitData( *sm );
	if ( sm_isshutdown && sm_cancel)
	    break; // we cancelled the shutdown, no need to save state
    // fall through
    case SmSaveLocal:
	tqApp->saveState( *sm );
	break;
    case SmSaveGlobal:
	tqApp->commitData( *sm );
	break;
    default:
	break;
    }

    if ( sm_phase2 && !sm_in_phase2 ) {
	SmcRequestSaveYourselfPhase2( smcConnection, sm_saveYourselfPhase2Callback, (SmPointer*) smd );
	sm_blockUserInput = false;
    }
    else {
	// close eventual interaction monitors and cancel the
	// shutdown, if required. Note that we can only cancel when
	// performing a shutdown, it does not work for checkpoints
	if ( sm_interactionActive ) {
	    SmcInteractDone( smcConnection, sm_isshutdown && sm_cancel);
	    sm_interactionActive = false;
	}
	else if ( sm_cancel && sm_isshutdown ) {
	    if ( sm->allowsErrorInteraction() ) {
		SmcInteractDone( smcConnection, True );
		sm_interactionActive = false;
	    }
	}

	// set restart and discard command in session manager
	sm_setProperty( SmRestartCommand, sm->restartCommand() );
	sm_setProperty( SmDiscardCommand, sm->discardCommand() );

	// set the restart hint
	SmPropValue prop;
	prop.length = sizeof( int );
	int value = sm->restartHint();
	prop.value = (SmPointer) &value;
	sm_setProperty( SmRestartStyleHint, SmCARD8, 1, &prop );

	// we are done
	SmcSaveYourselfDone( smcConnection, !sm_cancel );
    }
}

static void sm_dieCallback( SmcConn smcConn, SmPointer /* clientData  */)
{
    if (smcConn != smcConnection )
	return;
    resetSmState();
    TQEvent quitEvent(TQEvent::Quit);
    TQApplication::sendEvent(tqApp, &quitEvent);
}

static void sm_shutdownCancelledCallback( SmcConn smcConn, SmPointer /* clientData */)
{
    if (smcConn != smcConnection )
	return;
    if ( sm_waitingForInteraction )
	tqApp->exit_loop();
    resetSmState();
}

static void sm_saveCompleteCallback( SmcConn smcConn, SmPointer /*clientData */)
{
    if (smcConn != smcConnection )
	return;
    resetSmState();
}

static void sm_interactCallback( SmcConn smcConn, SmPointer /* clientData */ )
{
    if (smcConn != smcConnection )
	return;
    if ( sm_waitingForInteraction )
	tqApp->exit_loop();
}

static void sm_saveYourselfPhase2Callback( SmcConn smcConn, SmPointer clientData )
{
    if (smcConn != smcConnection )
	return;
    sm_in_phase2 = true;
    sm_performSaveYourself( (TQSessionManagerData*) clientData );
}


void TQSmSocketReceiver::socketActivated(int)
{
    IceProcessMessages( SmcGetIceConnection( smcConnection ), 0, 0);
}


#undef Bool
#include "qapplication_x11.moc"

TQSessionManager::TQSessionManager( TQApplication * app, TQString &id, TQString& key )
    : TQObject( app, "session manager" )
{
    d = new TQSessionManagerData( this, id, key );
    d->restartHint = RestartIfRunning;

    resetSmState();
    char cerror[256];
    char* myId = 0;
    char* prevId = (char*)id.latin1(); // we know what we are doing

    SmcCallbacks cb;
    cb.save_yourself.callback = sm_saveYourselfCallback;
    cb.save_yourself.client_data = (SmPointer) d;
    cb.die.callback = sm_dieCallback;
    cb.die.client_data = (SmPointer) d;
    cb.save_complete.callback = sm_saveCompleteCallback;
    cb.save_complete.client_data = (SmPointer) d;
    cb.shutdown_cancelled.callback = sm_shutdownCancelledCallback;
    cb.shutdown_cancelled.client_data = (SmPointer) d;

    // avoid showing a warning message below
    const char* session_manager = getenv("SESSION_MANAGER");
    if ( !session_manager || !session_manager[0] )
	return;

    smcConnection = SmcOpenConnection( 0, 0, 1, 0,
				       SmcSaveYourselfProcMask |
				       SmcDieProcMask |
				       SmcSaveCompleteProcMask |
				       SmcShutdownCancelledProcMask,
				       &cb,
				       prevId,
				       &myId,
				       256, cerror );

    id = TQString::fromLatin1( myId );
    ::free( myId ); // it was allocated by C

    TQString error = cerror;
    if (!smcConnection ) {
	tqWarning("Session management error: %s", error.latin1() );
    }
    else {
	sm_receiver = new TQSmSocketReceiver(  IceConnectionNumber( SmcGetIceConnection( smcConnection ) ) );
    }
}

TQSessionManager::~TQSessionManager()
{
    if ( smcConnection )
      SmcCloseConnection( smcConnection, 0, 0 );
    smcConnection = 0;
    delete sm_receiver;
    delete d;
}

TQString TQSessionManager::sessionId() const
{
    return d->sessionId;
}

TQString TQSessionManager::sessionKey() const
{
    return d->sessionKey;
}


void* TQSessionManager::handle() const
{
    return (void*) smcConnection;
}


bool TQSessionManager::allowsInteraction()
{
    if ( sm_interactionActive )
	return true;

    if ( sm_waitingForInteraction )
	return false;

    if ( sm_interactStyle == SmInteractStyleAny ) {
	sm_waitingForInteraction =  SmcInteractRequest( smcConnection, SmDialogNormal,
							sm_interactCallback, (SmPointer*) this );
    }
    if ( sm_waitingForInteraction ) {
	tqApp->enter_loop();
	sm_waitingForInteraction = false;
	if ( sm_smActive ) { // not cancelled
	    sm_interactionActive = true;
	    sm_blockUserInput = false;
	    return true;
	}
    }
    return false;
}

bool TQSessionManager::allowsErrorInteraction()
{
    if ( sm_interactionActive )
	return true;

    if ( sm_waitingForInteraction )
	return false;

    if ( sm_interactStyle == SmInteractStyleAny || sm_interactStyle == SmInteractStyleErrors ) {
	sm_waitingForInteraction =  SmcInteractRequest( smcConnection, SmDialogError,
							sm_interactCallback, (SmPointer*) this );
    }
    if ( sm_waitingForInteraction ) {
	tqApp->enter_loop();
	sm_waitingForInteraction = false;
	if ( sm_smActive ) { // not cancelled
	    sm_interactionActive = true;
	    sm_blockUserInput = false;
	    return true;
	}
    }
    return false;
}

void TQSessionManager::release()
{
    if ( sm_interactionActive ) {
	SmcInteractDone( smcConnection, False );
	sm_interactionActive = false;
	if ( sm_smActive && sm_isshutdown )
	    sm_blockUserInput = true;
    }
}

void TQSessionManager::cancel()
{
    sm_cancel = true;
}

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

void TQSessionManager::setManagerProperty( const TQString& name, const TQString& value)
{
    SmPropValue prop;
    prop.length = value.length();
    prop.value = (SmPointer) value.utf8().data();
    sm_setProperty( name.latin1(), SmARRAY8, 1, &prop );
}

void TQSessionManager::setManagerProperty( const TQString& name, const TQStringList& value)
{
    SmPropValue *prop = new SmPropValue[ value.count() ];
    int count = 0;
    for ( TQStringList::ConstIterator it = value.begin(); it != value.end(); ++it ) {
      prop[ count ].length = (*it).length();
      prop[ count ].value = (char*)(*it).utf8().data();
      ++count;
    }
    sm_setProperty( name.latin1(), SmLISTofARRAY8, count, prop );
    delete [] prop;
}

bool TQSessionManager::isPhase2() const
{
    return sm_in_phase2;
}

void TQSessionManager::requestPhase2()
{
    sm_phase2 = true;
}


#endif // TQT_NO_SM_SUPPORT
