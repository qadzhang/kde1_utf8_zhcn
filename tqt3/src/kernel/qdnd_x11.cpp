/****************************************************************************
**
** XDND implementation for TQt.  See http://www.cco.caltech.edu/~jafl/xdnd/
**
** Created : 980320
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

#include "qplatformdefs.h"

#include "ntqapplication.h"

#ifndef TQT_NO_DRAGANDDROP

#include "ntqwidget.h"
#include "ntqintdict.h"
#include "ntqdatetime.h"
#include "ntqdict.h"
#include "ntqguardedptr.h"
#include "ntqdragobject.h"
#include "ntqobjectlist.h"
#include "ntqcursor.h"
#include "ntqbitmap.h"
#include "ntqpainter.h"

#include "qt_x11_p.h"

// conflict resolution

const int XKeyPress = KeyPress;
const int XKeyRelease = KeyRelease;
#undef KeyPress
#undef KeyRelease

// this stuff is copied from qapp_x11.cpp

extern void tqt_x11_intern_atom( const char *, Atom * );

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

extern void tqt_ignore_badwindow();
extern bool tqt_badwindow();
extern void tqt_enter_modal( TQWidget *widget );
extern void tqt_leave_modal( TQWidget *widget );

#if defined(Q_C_CALLBACKS)
}
#endif

extern Window tqt_x11_findClientWindow( Window, Atom, bool );
extern Atom tqt_wm_state;
extern Time tqt_x_time;
extern Time tqt_x_user_time;

// this stuff is copied from qclb_x11.cpp

extern bool tqt_xclb_wait_for_event( Display *dpy, Window win, int type,
				     XEvent *event, int timeout );
extern bool tqt_xclb_read_property( Display *dpy, Window win, Atom property,
				    bool deleteProperty,
				    TQByteArray *buffer, int *size, Atom *type,
				    int *format, bool nullterm );
extern TQByteArray tqt_xclb_read_incremental_property( Display *dpy, Window win,
						       Atom property,
						       int nbytes, bool nullterm );
// and all this stuff is copied -into- qapp_x11.cpp

void tqt_xdnd_setup();
void tqt_handle_xdnd_enter( TQWidget *, const XEvent *, bool );
void tqt_handle_xdnd_position( TQWidget *, const XEvent *, bool );
void tqt_handle_xdnd_status( TQWidget *, const XEvent *, bool );
void tqt_handle_xdnd_leave( TQWidget *, const XEvent *, bool );
void tqt_handle_xdnd_drop( TQWidget *, const XEvent *, bool );
void tqt_handle_xdnd_finished( TQWidget *, const XEvent *, bool );
void tqt_xdnd_handle_selection_request( const XSelectionRequestEvent * );
bool tqt_xdnd_handle_badwindow();
// client messages
Atom tqt_xdnd_enter;
Atom tqt_xdnd_position;
Atom tqt_xdnd_status;
Atom tqt_xdnd_leave;
Atom tqt_xdnd_drop;
Atom tqt_xdnd_finished;
Atom tqt_xdnd_type_list;
const int tqt_xdnd_version = 4;

extern int tqt_x11_translateButtonState( int s );

// Actions
//
// The Xdnd spec allows for user-defined actions. This could be implemented
// with a registration process in TQt. WE SHOULD do that later.
//
Atom tqt_xdnd_action_copy;
Atom tqt_xdnd_action_link;
Atom tqt_xdnd_action_move;
Atom tqt_xdnd_action_private;
static
TQDropEvent::Action xdndaction_to_qtaction(Atom atom)
{
    if ( atom == tqt_xdnd_action_copy || atom == 0 )
	return TQDropEvent::Copy;
    if ( atom == tqt_xdnd_action_link )
	return TQDropEvent::Link;
    if ( atom == tqt_xdnd_action_move )
	return TQDropEvent::Move;
    return TQDropEvent::Private;
}
static
int qtaction_to_xdndaction(TQDropEvent::Action a)
{
    switch ( a ) {
      case TQDropEvent::Copy:
	return tqt_xdnd_action_copy;
      case TQDropEvent::Link:
	return tqt_xdnd_action_link;
      case TQDropEvent::Move:
	return tqt_xdnd_action_move;
      case TQDropEvent::Private:
	return tqt_xdnd_action_private;
      default:
	return tqt_xdnd_action_copy;
    }
}

// clean up the stuff used.
static void tqt_xdnd_cleanup();

static void tqt_xdnd_send_leave();

// XDND selection
Atom tqt_xdnd_selection;
// other selection
static Atom tqt_selection_property;
// INCR
static Atom tqt_incr_atom;

// properties for XDND drop sites
Atom tqt_xdnd_aware;
Atom tqt_xdnd_proxy;

// real variables:
// xid of current drag source
static Atom tqt_xdnd_dragsource_xid = 0;

// the types in this drop. 100 is no good, but at least it's big.
const int tqt_xdnd_max_type = 100;
static Atom tqt_xdnd_types[tqt_xdnd_max_type];

static TQIntDict<TQCString> * tqt_xdnd_drag_types = 0;
static TQDict<Atom> * tqt_xdnd_atom_numbers = 0;

// timer used when target wants "continuous" move messages (eg. scroll)
static int heartbeat = -1;
// rectangle in which the answer will be the same
static TQRect tqt_xdnd_source_sameanswer;
//static TQRect tqt_xdnd_target_sameanswer;
static bool tqt_xdnd_target_answerwas;
// top-level window we sent position to last.
static Window tqt_xdnd_current_target;
// window to send events to (always valid if tqt_xdnd_current_target)
static Window tqt_xdnd_current_proxy_target;
// widget we forwarded position to last, and local position
static TQGuardedPtr<TQWidget> tqt_xdnd_current_widget;
static TQPoint tqt_xdnd_current_position;
// time of this drop, as type Atom to save on casts
static Atom tqt_xdnd_source_current_time;
// timestamp from the XdndPosition and XdndDrop
static Time tqt_xdnd_target_current_time;
// screen number containing the pointer... -1 means default
static int tqt_xdnd_current_screen = -1;
// state of dragging... true if dragging, false if not
bool tqt_xdnd_dragging = false;
// need to check state of keyboard modifiers
static bool need_modifiers_check = false;

// dict of payload data, sorted by type atom
static TQIntDict<TQByteArray> * tqt_xdnd_target_data = 0;

// first drag object, or 0
static TQDragObject * tqt_xdnd_source_object = 0;

// Motif dnd
extern void tqt_motifdnd_enable( TQWidget *, bool );
extern TQByteArray tqt_motifdnd_obtain_data( const char *format );
extern const char *tqt_motifdnd_format( int n );

bool tqt_motifdnd_active = false;
static bool dndCancelled = false;

// Shift/Ctrl handling, and final drop status
static TQDragObject::DragMode drag_mode;
static TQDropEvent::Action global_requested_action = TQDropEvent::Copy;
static TQDropEvent::Action global_accepted_action = TQDropEvent::Copy;

// for embedding only
static TQWidget* current_embedding_widget  = 0;
static XEvent last_enter_event;

// cursors
static TQCursor *noDropCursor = 0;
static TQCursor *moveCursor = 0;
static TQCursor *copyCursor = 0;
static TQCursor *linkCursor = 0;

static TQPixmap *defaultPm = 0;

static const int default_pm_hotx = -2;
static const int default_pm_hoty = -16;
static const char* const default_pm[] = {
"13 9 3 1",
".      c None",
"       c #000000",
"X      c #FFFFFF",
"X X X X X X X",
" X X X X X X ",
"X ......... X",
" X.........X ",
"X ......... X",
" X.........X ",
"X ......... X",
" X X X X X X ",
"X X X X X X X"
};

class TQShapedPixmapWidget : public TQWidget {

public:
    TQShapedPixmapWidget(int screen = -1) :
	TQWidget(TQApplication::desktop()->screen( screen ),
		0, WStyle_Customize | WStyle_Tool | WStyle_NoBorder | WX11BypassWM ), oldpmser( 0 ), oldbmser( 0 )
    {
    x11SetWindowType( X11WindowTypeDND );
    }

    void setPixmap(TQPixmap pm, TQPoint hot)
    {
	int bmser = pm.mask() ? pm.mask()->serialNumber() : 0;
	if( oldpmser == pm.serialNumber() && oldbmser == bmser
	    && oldhot == hot )
	    return;
	oldpmser = pm.serialNumber();
	oldbmser = bmser;
	oldhot = hot;
	bool hotspot_in = !(hot.x() < 0 || hot.y() < 0 || hot.x() >= pm.width() || hot.y() >= pm.height());
// if the pixmap has hotspot in its area, make a "hole" in it at that position
// this will allow XTranslateCoordinates() to find directly the window below the cursor instead
// of finding this pixmap, and therefore there won't be needed any (slow) search for the window
// using findRealWindow()
	if( hotspot_in ) {
	    TQBitmap mask = pm.mask() ? *pm.mask() : TQBitmap( pm.width(), pm.height());
	    if( !pm.mask())
		mask.fill( TQt::color1 );
	    TQPainter p( &mask );
	    p.setPen( TQt::color0 );
	    p.drawPoint( hot.x(), hot.y());
	    p.end();
    	    pm.setMask( mask );
    	    setMask( mask );
	} else if ( pm.mask() ) {
	    setMask( *pm.mask() );
	} else {
	    clearMask();
	}
	resize(pm.width(),pm.height());
	setErasePixmap(pm);
	erase();
    }
private:
    int oldpmser;
    int oldbmser;
    TQPoint oldhot;
};

static TQShapedPixmapWidget * tqt_xdnd_deco = 0;

static TQWidget* desktop_proxy = 0;

class TQExtraWidget : public TQWidget
{
public:
    TQWExtra* extraData() { return TQWidget::extraData(); }
    TQTLWExtra* topData() { return TQWidget::topData(); }
};


static bool tqt_xdnd_enable( TQWidget* w, bool on )
{
    if ( on ) {
	TQWidget * xdnd_widget = 0;
	if ( w->isDesktop() ) {
	    if ( desktop_proxy ) // *WE* already have one.
		return false;

	    // As per Xdnd4, use XdndProxy
	    XGrabServer( w->x11Display() );
	    Atom type = None;
	    int f;
	    unsigned long n, a;
	    WId *proxy_id_ptr;
	    XGetWindowProperty( w->x11Display(), w->winId(),
		tqt_xdnd_proxy, 0, 1, False,
		XA_WINDOW, &type, &f,&n,&a,(uchar**)&proxy_id_ptr );
	    WId proxy_id = 0;
	    if ( type == XA_WINDOW && proxy_id_ptr ) {
		proxy_id = *proxy_id_ptr;
		XFree(proxy_id_ptr);
		proxy_id_ptr = 0;
		// Already exists. Real?
		tqt_ignore_badwindow();
		XGetWindowProperty( w->x11Display(), proxy_id,
		    tqt_xdnd_proxy, 0, 1, False,
		    XA_WINDOW, &type, &f,&n,&a,(uchar**)&proxy_id_ptr );
		if ( tqt_badwindow() || type != XA_WINDOW || !proxy_id_ptr || *proxy_id_ptr != proxy_id ) {
		    // Bogus - we will overwrite.
		    proxy_id = 0;
		}
	    }
	    if ( proxy_id_ptr )
		XFree(proxy_id_ptr);

	    if ( !proxy_id ) {
		xdnd_widget = desktop_proxy = new TQWidget;
		proxy_id = desktop_proxy->winId();
		XChangeProperty ( w->x11Display(),
		    w->winId(), tqt_xdnd_proxy,
		    XA_WINDOW, 32, PropModeReplace,
		    (unsigned char *)&proxy_id, 1 );
		XChangeProperty ( w->x11Display(),
		    proxy_id, tqt_xdnd_proxy,
		    XA_WINDOW, 32, PropModeReplace,
		    (unsigned char *)&proxy_id, 1 );
	    }

	    XUngrabServer( w->x11Display() );
	} else {
	    xdnd_widget = w->topLevelWidget();
	}
	if ( xdnd_widget ) {
	    Atom atm = (Atom)tqt_xdnd_version;
	    XChangeProperty ( xdnd_widget->x11Display(), xdnd_widget->winId(),
			      tqt_xdnd_aware, XA_ATOM, 32, PropModeReplace,
			      (unsigned char *)&atm, 1 );
	    return true;
	} else {
	    return false;
	}
    } else {
	if ( w->isDesktop() ) {
	    XDeleteProperty( w->x11Display(), w->winId(),
			     tqt_xdnd_proxy );
	    delete desktop_proxy;
	    desktop_proxy = 0;
	}
	return true;
    }
}

const char* tqt_xdnd_atom_to_str( Atom a )
{
    if ( !a ) return 0;

    if ( a == XA_STRING )
	return "text/plain"; // some Xdnd clients are dumb

    if ( !tqt_xdnd_drag_types ) {
	tqt_xdnd_drag_types = new TQIntDict<TQCString>( 17 );
	tqt_xdnd_drag_types->setAutoDelete( true );
    }
    TQCString* result;
    if ( !(result=tqt_xdnd_drag_types->find( a )) ) {
	const char* mimeType = XGetAtomName( TQPaintDevice::x11AppDisplay(), a );
	if ( !mimeType )
	    return 0; // only happens on protocol error
	result = new TQCString( mimeType );
	tqt_xdnd_drag_types->insert( (long)a, result );
	XFree((void*)mimeType);
    }
    return *result;
}

Atom* tqt_xdnd_str_to_atom( const char *mimeType )
{
    if ( !mimeType || !*mimeType )
	return 0;
    if ( !tqt_xdnd_atom_numbers ) {
	tqt_xdnd_atom_numbers = new TQDict<Atom>( 17 );
	tqt_xdnd_atom_numbers->setAutoDelete( true );
    }

    Atom * result;
    if ( (result = tqt_xdnd_atom_numbers->find( mimeType )) )
	return result;

    result = new Atom;
    *result = 0;
    tqt_x11_intern_atom( mimeType, result );
    tqt_xdnd_atom_numbers->insert( mimeType, result );
    tqt_xdnd_atom_to_str( *result );

    return result;
}


void tqt_xdnd_setup() {
    // set up protocol atoms
    tqt_x11_intern_atom( "XdndEnter", &tqt_xdnd_enter );
    tqt_x11_intern_atom( "XdndPosition", &tqt_xdnd_position );
    tqt_x11_intern_atom( "XdndStatus", &tqt_xdnd_status );
    tqt_x11_intern_atom( "XdndLeave", &tqt_xdnd_leave );
    tqt_x11_intern_atom( "XdndDrop", &tqt_xdnd_drop );
    tqt_x11_intern_atom( "XdndFinished", &tqt_xdnd_finished );
    tqt_x11_intern_atom( "XdndTypeList", &tqt_xdnd_type_list );

    tqt_x11_intern_atom( "XdndSelection", &tqt_xdnd_selection );

    tqt_x11_intern_atom( "XdndAware", &tqt_xdnd_aware );
    tqt_x11_intern_atom( "XdndProxy", &tqt_xdnd_proxy );


    tqt_x11_intern_atom( "XdndActionCopy", &tqt_xdnd_action_copy );
    tqt_x11_intern_atom( "XdndActionLink", &tqt_xdnd_action_link );
    tqt_x11_intern_atom( "XdndActionMove", &tqt_xdnd_action_move );
    tqt_x11_intern_atom( "XdndActionPrivate", &tqt_xdnd_action_private );

    tqt_x11_intern_atom( "QT_SELECTION", &tqt_selection_property );
    tqt_x11_intern_atom( "INCR", &tqt_incr_atom );

    tqAddPostRoutine( tqt_xdnd_cleanup );
}


void tqt_xdnd_cleanup()
{
    delete tqt_xdnd_drag_types;
    tqt_xdnd_drag_types = 0;
    delete tqt_xdnd_atom_numbers;
    tqt_xdnd_atom_numbers = 0;
    delete tqt_xdnd_target_data;
    tqt_xdnd_target_data = 0;
    delete noDropCursor;
    noDropCursor = 0;
    delete copyCursor;
    copyCursor = 0;
    delete moveCursor;
    moveCursor = 0;
    delete linkCursor;
    linkCursor = 0;
    delete defaultPm;
    defaultPm = 0;
    delete desktop_proxy;
    desktop_proxy = 0;
}


static TQWidget * find_child( TQWidget * tlw, TQPoint & p )
{
    TQWidget * w = tlw;

    p = w->mapFromGlobal( p );
    bool done = false;
    while ( !done ) {
	done = true;
	if ( ((TQExtraWidget*)w)->extraData() &&
	     ((TQExtraWidget*)w)->extraData()->xDndProxy != 0 )
	    break; // stop searching for widgets under the mouse cursor if found widget is a proxy.
	if ( w->children() ) {
	    TQObjectListIt it( *w->children() );
	    it.toLast();
	    TQObject * o;
	    while( (o=it.current()) ) {
		--it;
		if ( o->isWidgetType() &&
		     ((TQWidget*)o)->isVisible() &&
		     ((TQWidget*)o)->geometry().contains( p ) &&
		     !((TQWidget*)o)->isTopLevel()) {
		    w = (TQWidget *)o;
		    done = false;
		    p = w->mapFromParent( p );
		    break;
		}
	    }
	}
    }
    return w;
}


static bool checkEmbedded(TQWidget* w, const XEvent* xe)
{
    if (!w)
	return false;

    if (current_embedding_widget != 0 && current_embedding_widget != w) {
	tqt_xdnd_current_target = ((TQExtraWidget*)current_embedding_widget)->extraData()->xDndProxy;
	tqt_xdnd_current_proxy_target = tqt_xdnd_current_target;
	tqt_xdnd_send_leave();
	tqt_xdnd_current_target = 0;
	tqt_xdnd_current_proxy_target = 0;
	current_embedding_widget = 0;
    }

    TQWExtra* extra = ((TQExtraWidget*)w)->extraData();
    if ( extra && extra->xDndProxy != 0 ) {

	if (current_embedding_widget != w) {

	    last_enter_event.xany.window = extra->xDndProxy;
	    XSendEvent( TQPaintDevice::x11AppDisplay(), extra->xDndProxy, False, NoEventMask,
			&last_enter_event );
	    current_embedding_widget = w;
	}

	((XEvent*)xe)->xany.window = extra->xDndProxy;
	XSendEvent( TQPaintDevice::x11AppDisplay(), extra->xDndProxy, False, NoEventMask,
		    (XEvent*)xe );
	tqt_xdnd_current_widget = w;
	return true;
    }
    current_embedding_widget = 0;
    return false;
}

void tqt_handle_xdnd_enter( TQWidget *, const XEvent * xe, bool /*passive*/ )
{
    //if ( !w->neveHadAChildWithDropEventsOn() )
	//return; // haven't been set up for dnd

    tqt_motifdnd_active = false;

    last_enter_event.xclient = xe->xclient;

    tqt_xdnd_target_answerwas = false;

    const long *l = xe->xclient.data.l;
    int version = (int)(((unsigned long)(l[1])) >> 24);

    if ( version > tqt_xdnd_version )
	return;

    tqt_xdnd_dragsource_xid = l[0];

    int j = 0;
    if ( l[1] & 1 ) {
	// get the types from XdndTypeList
	Atom   type = None;
	int f;
	unsigned long n, a;
	Atom *data;
	XGetWindowProperty( TQPaintDevice::x11AppDisplay(), tqt_xdnd_dragsource_xid,
		    tqt_xdnd_type_list, 0,
		    tqt_xdnd_max_type, False, XA_ATOM, &type, &f,&n,&a,(uchar**)&data );
	for ( ; j<tqt_xdnd_max_type && j < (int)n; j++ ) {
	    tqt_xdnd_types[j] = data[j];
	}
	if ( data )
	    XFree( (uchar*)data );
    } else {
	// get the types from the message
	int i;
	for( i=2; i < 5; i++ ) {
	    tqt_xdnd_types[j++] = l[i];
	}
    }
    tqt_xdnd_types[j] = 0;
}



void tqt_handle_xdnd_position( TQWidget *w, const XEvent * xe, bool passive )
{
    const unsigned long *l = (const unsigned long *)xe->xclient.data.l;

    TQPoint p( (l[2] & 0xffff0000) >> 16, l[2] & 0x0000ffff );
    TQWidget * c = find_child( w, p ); // changes p to to c-local coordinates

    if (!passive && checkEmbedded(c, xe))
	return;

    if ( !c || ( !c->acceptDrops() && c->isDesktop() ) ) {
	return;
    }

    if ( l[0] != tqt_xdnd_dragsource_xid ) {
	//tqDebug( "xdnd drag position from unexpected source (%08lx not %08lx)",
	//     l[0], tqt_xdnd_dragsource_xid );
	return;
    }

    if (l[3] != 0) {
        // timestamp from the source
        tqt_xdnd_target_current_time = tqt_x_user_time = l[3];
    }

    XClientMessageEvent response;
    response.type = ClientMessage;
    response.window = tqt_xdnd_dragsource_xid;
    response.format = 32;
    response.message_type = tqt_xdnd_status;
    response.data.l[0] = w->winId();
    response.data.l[1] = 0; // flags
    response.data.l[2] = 0; // x, y
    response.data.l[3] = 0; // w, h
    response.data.l[4] = 0; // action

    if ( !passive ) { // otherwise just reject
	while ( c && !c->acceptDrops() && !c->isTopLevel() ) {
	    p = c->mapToParent( p );
	    c = c->parentWidget();
	}

	TQRect answerRect( c->mapToGlobal( p ), TQSize( 1,1 ) );

	TQDragMoveEvent me( p );
	TQDropEvent::Action accepted_action = xdndaction_to_qtaction(l[4]);
	me.setAction(accepted_action);

	if ( c != tqt_xdnd_current_widget ) {
	    tqt_xdnd_target_answerwas = false;
	    if ( tqt_xdnd_current_widget ) {
		TQDragLeaveEvent e;
		TQApplication::sendEvent( tqt_xdnd_current_widget, &e );
	    }
	    if ( c->acceptDrops() ) {
		tqt_xdnd_current_widget = c;
		tqt_xdnd_current_position = p;

		TQDragEnterEvent de( p );
		de.setAction(accepted_action);
		TQApplication::sendEvent( c, &de );
		if ( de.isAccepted() ) {
		    me.accept( de.answerRect() );
		    if ( !de.isActionAccepted() ) // only as a copy (move if we del)
			accepted_action = TQDropEvent::Copy;
		    else
			me.acceptAction(true);
		} else {
		    me.ignore( de.answerRect() );
		}
	    }
	} else {
	    if ( tqt_xdnd_target_answerwas ) {
		me.accept();
		me.acceptAction(global_requested_action == global_accepted_action);
	    }
	}

	if ( !c->acceptDrops() ) {
	    tqt_xdnd_current_widget = 0;
	    answerRect = TQRect( p, TQSize( 1, 1 ) );
	} else if ( xdndaction_to_qtaction(l[4]) < TQDropEvent::Private ) {
	    tqt_xdnd_current_widget = c;
	    tqt_xdnd_current_position = p;

	    TQApplication::sendEvent( c, &me );
	    tqt_xdnd_target_answerwas = me.isAccepted();
	    if ( me.isAccepted() ) {
		response.data.l[1] = 1; // yes
		if ( !me.isActionAccepted() ) // only as a copy (move if we del)
		    accepted_action = TQDropEvent::Copy;
	    } else {
		response.data.l[0] = 0;
	    }
	    answerRect = me.answerRect().intersect( c->rect() );
	} else {
	    response.data.l[0] = 0;
	    answerRect = TQRect( p, TQSize( 1, 1 ) );
	}
	answerRect = TQRect( c->mapToGlobal( answerRect.topLeft() ),
			    answerRect.size() );

	if ( answerRect.left() < 0 )
	    answerRect.setLeft( 0 );
	if ( answerRect.right() > 4096 )
	    answerRect.setRight( 4096 );
	if ( answerRect.top() < 0 )
	    answerRect.setTop( 0 );
	if ( answerRect.bottom() > 4096 )
	    answerRect.setBottom( 4096 );
	if ( answerRect.width() < 0 )
	    answerRect.setWidth( 0 );
	if ( answerRect.height() < 0 )
	    answerRect.setHeight( 0 );

	response.data.l[2] = (answerRect.x() << 16) + answerRect.y();
	response.data.l[3] = (answerRect.width() << 16) + answerRect.height();
	response.data.l[4] = qtaction_to_xdndaction(accepted_action);
	global_accepted_action = accepted_action;
    }

    // reset
    tqt_xdnd_target_current_time = CurrentTime;

    TQWidget * source = TQWidget::find( tqt_xdnd_dragsource_xid );

    if ( source && source->isDesktop() && !source->acceptDrops() )
	source = 0;

    if ( source )
	tqt_handle_xdnd_status( source, (const XEvent *)&response, passive );
    else
	XSendEvent( TQPaintDevice::x11AppDisplay(), tqt_xdnd_dragsource_xid, False,
		    NoEventMask, (XEvent*)&response );
}


void tqt_handle_xdnd_status( TQWidget * w, const XEvent * xe, bool /*passive*/ )
{
    const unsigned long *l = (const unsigned long *)xe->xclient.data.l;
    // Messy:  TQDragResponseEvent is just a call to TQDragManager function
    global_accepted_action = xdndaction_to_qtaction(l[4]);
    TQDragResponseEvent e( (int)(l[1] & 1) );
    TQApplication::sendEvent( w, &e );

    if ( (int)(l[1] & 2) == 0 ) {
	TQPoint p( (l[2] & 0xffff0000) >> 16, l[2] & 0x0000ffff );
	TQSize s( (l[3] & 0xffff0000) >> 16, l[3] & 0x0000ffff );
	tqt_xdnd_source_sameanswer = TQRect( p, s );
	if ( tqt_xdnd_source_sameanswer.isNull() ) {
	    // Application wants "coninutous" move events
	}
    } else {
	tqt_xdnd_source_sameanswer = TQRect();
    }
}


void tqt_handle_xdnd_leave( TQWidget *w, const XEvent * xe, bool /*passive*/ )
{
    //tqDebug( "xdnd leave" );
    if ( !tqt_xdnd_current_widget ||
	 w->topLevelWidget() != tqt_xdnd_current_widget->topLevelWidget() ) {
	return; // sanity
    }

    if (checkEmbedded(current_embedding_widget, xe)) {
	current_embedding_widget = 0;
	tqt_xdnd_current_widget = 0;
	return;
    }

    const unsigned long *l = (const unsigned long *)xe->xclient.data.l;

    TQDragLeaveEvent e;
    TQApplication::sendEvent( tqt_xdnd_current_widget, &e );

    if ( l[0] != tqt_xdnd_dragsource_xid ) {
	// This often happens - leave other-process window quickly
	//tqDebug( "xdnd drag leave from unexpected source (%08lx not %08lx",
	       //l[0], tqt_xdnd_dragsource_xid );
	tqt_xdnd_current_widget = 0;
	return;
    }

    tqt_xdnd_dragsource_xid = 0;
    tqt_xdnd_types[0] = 0;
    tqt_xdnd_current_widget = 0;
}


void tqt_xdnd_send_leave()
{
    if ( !tqt_xdnd_current_target )
	return;

    XClientMessageEvent leave;
    leave.type = ClientMessage;
    leave.window = tqt_xdnd_current_target;
    leave.format = 32;
    leave.message_type = tqt_xdnd_leave;
    leave.data.l[0] = tqt_xdnd_dragsource_xid;
    leave.data.l[1] = 0; // flags
    leave.data.l[2] = 0; // x, y
    leave.data.l[3] = 0; // w, h
    leave.data.l[4] = 0; // just null

    TQWidget * w = TQWidget::find( tqt_xdnd_current_proxy_target );

    if ( w && w->isDesktop() && !w->acceptDrops() )
	w = 0;

    if ( w )
	tqt_handle_xdnd_leave( w, (const XEvent *)&leave, false );
    else
	XSendEvent( TQPaintDevice::x11AppDisplay(), tqt_xdnd_current_proxy_target, False,
		    NoEventMask, (XEvent*)&leave );
    tqt_xdnd_current_target = 0;
    tqt_xdnd_current_proxy_target = 0;
}



void tqt_handle_xdnd_drop( TQWidget *, const XEvent * xe, bool passive )
{
    if ( !tqt_xdnd_current_widget ) {
	tqt_xdnd_dragsource_xid = 0;
	return; // sanity
    }

    if (!passive && checkEmbedded(tqt_xdnd_current_widget, xe)){
	current_embedding_widget = 0;
	tqt_xdnd_dragsource_xid = 0;
	tqt_xdnd_current_widget = 0;
	return;
    }
    const unsigned long *l = (const unsigned long *)xe->xclient.data.l;

    //tqDebug( "xdnd drop" );

    if ( l[0] != tqt_xdnd_dragsource_xid ) {
	//tqDebug( "xdnd drop from unexpected source (%08lx not %08lx",
	//       l[0], tqt_xdnd_dragsource_xid );
	return;
    }

    if (l[2] != 0) {
        // update the "user time" from the timestamp in the event.
        tqt_xdnd_target_current_time = tqt_x_user_time = l[2];
    }

    if ( tqt_xdnd_source_object )
	tqt_xdnd_source_object->setTarget( tqt_xdnd_current_widget );

    if ( !passive ) {
	TQDropEvent de( tqt_xdnd_current_position );
	de.setAction( global_accepted_action );
	TQApplication::sendEvent( tqt_xdnd_current_widget, &de );
	if ( !de.isAccepted() ) {
	    // Ignore a failed drag
	    global_accepted_action = TQDropEvent::Copy;
	    dndCancelled = true;
	}
	XClientMessageEvent finished;
	finished.type = ClientMessage;
	finished.window = tqt_xdnd_dragsource_xid;
	finished.format = 32;
	finished.message_type = tqt_xdnd_finished;
	finished.data.l[0] = tqt_xdnd_current_widget?tqt_xdnd_current_widget->topLevelWidget()->winId():0;
	finished.data.l[1] = 0; // flags
	XSendEvent( TQPaintDevice::x11AppDisplay(), tqt_xdnd_dragsource_xid, False,
		    NoEventMask, (XEvent*)&finished );
    } else {
	TQDragLeaveEvent e;
	TQApplication::sendEvent( tqt_xdnd_current_widget, &e );
    }
    tqt_xdnd_dragsource_xid = 0;
    tqt_xdnd_current_widget = 0;

    // reset
    tqt_xdnd_target_current_time = CurrentTime;
}


void tqt_handle_xdnd_finished( TQWidget *, const XEvent * xe, bool passive )
{
    const unsigned long *l = (const unsigned long *)xe->xclient.data.l;

    if ( l[0] && (l[0] == tqt_xdnd_current_target
	    || l[0] == tqt_xdnd_current_proxy_target) ) {
	//
	if ( !passive )
	    (void ) checkEmbedded( tqt_xdnd_current_widget, xe);
	current_embedding_widget = 0;
	tqt_xdnd_current_target = 0;
	tqt_xdnd_current_proxy_target = 0;
	delete tqt_xdnd_source_object;
	tqt_xdnd_source_object = 0;
    }
}


void TQDragManager::timerEvent( TQTimerEvent* e )
{
    if ( e->timerId() == heartbeat ) {
        if( need_modifiers_check ) {
            Window root, child;
            int root_x, root_y, win_x, win_y;
            unsigned int mask;
            XQueryPointer( tqt_xdisplay(), tqt_xrootwin( tqt_xdnd_current_screen ),
                &root, &child, &root_x, &root_y, &win_x, &win_y, &mask );
            if( updateMode( (ButtonState)tqt_x11_translateButtonState( mask )))
                tqt_xdnd_source_sameanswer = TQRect(); // force move
        }
        need_modifiers_check = true;
        if( tqt_xdnd_source_sameanswer.isNull() )
	    move( TQCursor::pos() );
    }
}

static bool tqt_xdnd_was_move = false;
static bool tqt_xdnd_found = false;
// check whole incoming X queue for move events
// checking whole queue is done by always returning False in the predicate
// if there's another move event in the queue, and there's not a mouse button
// or keyboard or ClientMessage event before it, the current move event
// may be safely discarded
// this helps avoiding being overloaded by being flooded from many events
// from the XServer
static
Bool tqt_xdnd_predicate( Display*, XEvent* ev, XPointer )
{
    if( tqt_xdnd_found )
	return False;
    if( ev->type == MotionNotify )
    {
	tqt_xdnd_was_move = true;
	tqt_xdnd_found = true;
    }
    if( ev->type == ButtonPress || ev->type == ButtonRelease
	|| ev->type == XKeyPress || ev->type == XKeyRelease
	|| ev->type == ClientMessage )
    {
	tqt_xdnd_was_move = false;
	tqt_xdnd_found = true;
    }
    return False;
}

static
bool tqt_xdnd_another_movement()
{
    tqt_xdnd_was_move = false;
    tqt_xdnd_found = false;
    XEvent dummy;
    XCheckIfEvent( tqt_xdisplay(), &dummy, tqt_xdnd_predicate, NULL );
    return tqt_xdnd_was_move;
}

bool TQDragManager::eventFilter( TQObject * o, TQEvent * e)
{
    if ( beingCancelled ) {
	if ( e->type() == TQEvent::KeyRelease &&
	     ((TQKeyEvent*)e)->key() == Key_Escape ) {
	    tqApp->removeEventFilter( this );
	    object = 0;
	    dragSource = 0;
	    beingCancelled = false;
	    tqApp->exit_loop();
	    return true; // block the key release
	}
	return false;
    }

    Q_ASSERT( object != 0 );

    if ( !o->isWidgetType() )
	return false;

    if ( e->type() == TQEvent::MouseMove ) {
	TQMouseEvent* me = (TQMouseEvent *)e;
	if( !tqt_xdnd_another_movement()) {
	    updateMode(me->stateAfter());
	    move( me->globalPos() );
	}
        need_modifiers_check = false;
	return true;
    } else if ( e->type() == TQEvent::MouseButtonRelease ) {
	tqApp->removeEventFilter( this );
	if ( willDrop )
	    drop();
	else
	    cancel();
	object = 0;
	dragSource = 0;
	beingCancelled = false;
	tqApp->exit_loop();
	return true;
    } else if ( e->type() == TQEvent::DragResponse ) {
	if ( ((TQDragResponseEvent *)e)->dragAccepted() ) {
	    if ( !willDrop ) {
		willDrop = true;
	    }
	} else {
	    if ( willDrop ) {
		willDrop = false;
	    }
	}
	updateCursor();
	return true;
    }

    if ( e->type() == TQEvent::KeyPress
      || e->type() == TQEvent::KeyRelease )
    {
	TQKeyEvent *ke = ((TQKeyEvent*)e);
	if ( ke->key() == Key_Escape && e->type() == TQEvent::KeyPress ) {
	    cancel();
	    tqApp->removeEventFilter( this );
	    object = 0;
	    dragSource = 0;
	    beingCancelled = false;
	    tqApp->exit_loop();
	} else {
	    if( updateMode(ke->stateAfter())) {
	        tqt_xdnd_source_sameanswer = TQRect(); // force move
	        move( TQCursor::pos() );
            }
            need_modifiers_check = false;
	}
	return true; // Eat all key events
    }

    // ### We bind modality to widgets, so we have to do this
    // ###  "manually".
    // DnD is modal - eat all other interactive events
    switch ( e->type() ) {
      case TQEvent::MouseButtonPress:
      case TQEvent::MouseButtonRelease:
      case TQEvent::MouseButtonDblClick:
      case TQEvent::MouseMove:
      case TQEvent::KeyPress:
      case TQEvent::KeyRelease:
      case TQEvent::Wheel:
      case TQEvent::Accel:
      case TQEvent::AccelAvailable:
      case TQEvent::AccelOverride:
	return true;
      default:
	return false;
    }
}


static TQt::ButtonState oldstate;
bool TQDragManager::updateMode( ButtonState newstate )
{
    if ( newstate == oldstate )
	return false;
    const int both = ShiftButton|ControlButton;
    if ( (newstate & both) == both ) {
	global_requested_action = TQDropEvent::Link;
    } else {
	bool local = tqt_xdnd_source_object != 0;
	if ( drag_mode == TQDragObject::DragMove )
	    global_requested_action = TQDropEvent::Move;
	else if ( drag_mode == TQDragObject::DragCopy )
	    global_requested_action = TQDropEvent::Copy;
	else if ( drag_mode == TQDragObject::DragLink )
	    global_requested_action = TQDropEvent::Link;
	else {
	    if ( drag_mode == TQDragObject::DragDefault && local )
		global_requested_action = TQDropEvent::Move;
	    else
		global_requested_action = TQDropEvent::Copy;
	    if ( newstate & ShiftButton )
		global_requested_action = TQDropEvent::Move;
	    else if ( newstate & ControlButton )
		global_requested_action = TQDropEvent::Copy;
	}
    }
    oldstate = newstate;
    return true;
}


void TQDragManager::createCursors()
{
    if ( !noDropCursor ) {
	noDropCursor = new TQCursor( ForbiddenCursor );
	if ( !pm_cursor[0].isNull() )
	    moveCursor = new TQCursor(pm_cursor[0], 0,0);
	if ( !pm_cursor[1].isNull() )
	    copyCursor = new TQCursor(pm_cursor[1], 0,0);
	if ( !pm_cursor[2].isNull() )
	    linkCursor = new TQCursor(pm_cursor[2], 0,0);
    }
}

void TQDragManager::updateCursor()
{
    TQCursor *c;
    if ( willDrop ) {
	if ( global_accepted_action == TQDropEvent::Copy ) {
	    if ( global_requested_action == TQDropEvent::Move )
		c = moveCursor; // (source can delete)
	    else
		c = copyCursor;
	} else if ( global_accepted_action == TQDropEvent::Link ) {
	    c = linkCursor;
	} else {
	    c = moveCursor;
	}
	if ( tqt_xdnd_deco ) {
	    tqt_xdnd_deco->show();
	    tqt_xdnd_deco->raise();
	}
    } else {
	c = noDropCursor;
	//if ( tqt_xdnd_deco )
	//    tqt_xdnd_deco->hide();
    }
#ifndef TQT_NO_CURSOR
    if ( c )
	tqApp->setOverrideCursor( *c, true );
#endif
}


void TQDragManager::cancel( bool deleteSource )
{
    killTimer( heartbeat );
    heartbeat = -1;
    if ( object ) {
	beingCancelled = true;
	object = 0;
    }

    if ( tqt_xdnd_current_target ) {
	tqt_xdnd_send_leave();
    }

#ifndef TQT_NO_CURSOR
    if ( restoreCursor ) {
	TQApplication::restoreOverrideCursor();
	restoreCursor = false;
    }
#endif

    if ( deleteSource )
	delete tqt_xdnd_source_object;
    tqt_xdnd_source_object = 0;
    delete tqt_xdnd_deco;
    tqt_xdnd_deco = 0;

    dndCancelled = true;
}

static
Window findRealWindow( const TQPoint & pos, Window w, int md )
{
    if ( tqt_xdnd_deco && w == tqt_xdnd_deco->winId() )
	return 0;

    if ( md ) {
        tqt_ignore_badwindow();
	XWindowAttributes attr;
	XGetWindowAttributes( TQPaintDevice::x11AppDisplay(), w, &attr );
        if (tqt_badwindow())
            return 0;

	if ( attr.map_state == IsViewable
	    && TQRect(attr.x,attr.y,attr.width,attr.height)
             .contains(pos) )
        {
	    {
		Atom   type = None;
		int f;
		unsigned long n, a;
		unsigned char *data;

		XGetWindowProperty( TQPaintDevice::x11AppDisplay(), w, tqt_xdnd_aware, 0,
		    0, False, AnyPropertyType, &type, &f,&n,&a,&data );
		if ( data ) XFree(data);
		if ( type ) return w;
	    }

	    Window r, p;
	    Window* c;
	    uint nc;
	    if ( XQueryTree( TQPaintDevice::x11AppDisplay(), w, &r, &p, &c, &nc ) ) {
		r=0;
		for (uint i=nc; !r && i--; ) {
		    r = findRealWindow( pos-TQPoint(attr.x,attr.y),
					c[i], md-1 );
		}
		XFree(c);
		if ( r )
		    return r;

		// We didn't find a client window!  Just use the
		// innermost window.
	    }

	    // No children!
	    return w;
	}
    }
    return 0;
}

void TQDragManager::move( const TQPoint & globalPos )
{
    if (!object || !object->source()) {
        // perhaps the target crashed?
        return;
    }

    int screen = TQCursor::x11Screen();
    if ( ( tqt_xdnd_current_screen == -1 && screen != TQPaintDevice::x11AppScreen() ) ||
	 ( screen != tqt_xdnd_current_screen ) ) {
	// recreate the pixmap on the new screen...
	delete tqt_xdnd_deco;
	tqt_xdnd_deco = new TQShapedPixmapWidget( screen );
        tqt_xdnd_deco->x11SetWindowTransient( dragSource->topLevelWidget());
	if (!TQWidget::mouseGrabber()) {
	    updatePixmap();
	    tqt_xdnd_deco->grabMouse();
	}
    }
    updatePixmap( globalPos );

    if ( tqt_xdnd_source_sameanswer.contains( globalPos ) &&
	 tqt_xdnd_source_sameanswer.isValid() ) {
	return;
    }

    tqt_xdnd_current_screen = screen;
    Window rootwin = TQPaintDevice::x11AppRootWindow( tqt_xdnd_current_screen );
    Window target = 0;
    int lx = 0, ly = 0;
    if ( !XTranslateCoordinates( TQPaintDevice::x11AppDisplay(), rootwin, rootwin,
				 globalPos.x(), globalPos.y(),
				 &lx, &ly, &target) )
	// some wierd error...
	return;

    if ( target == rootwin ) {
	// Ok.
    } else if ( target ) {
	//me
        Window src = rootwin;
        while (target != 0) {
            int lx2, ly2;
            Window t;
            // translate coordinates
            if (!XTranslateCoordinates(TQPaintDevice::x11AppDisplay(), src, target,
                                       lx, ly, &lx2, &ly2, &t)) {
                target = 0;
                break;
            }
            lx = lx2;
            ly = ly2;
            src = target;

	    // check if it has XdndAware
	    Atom type = None;
	    int f;
	    unsigned long n, a;
	    unsigned char *data = 0;
	    XGetWindowProperty(TQPaintDevice::x11AppDisplay(), target, tqt_xdnd_aware, 0, 0, False,
                               AnyPropertyType, &type, &f,&n,&a,&data);
	    if (data)
                XFree(data);
	    if (type)
                break;

            // find child at the coordinates
            if (!XTranslateCoordinates( TQPaintDevice::x11AppDisplay(), src, src,
                                        lx, ly, &lx2, &ly2, &target)) {
                target = 0;
                break;
            }
        }
	if ( tqt_xdnd_deco && (!target || target == tqt_xdnd_deco->winId()) ) {
	    target = findRealWindow(globalPos,rootwin,6);
	}
    }

    TQWidget* w;
    if ( target ) {
	w = TQWidget::find( (WId)target );
	if ( w && w->isDesktop() && !w->acceptDrops() )
	    w = 0;
    } else {
	w = 0;
	target = rootwin;
    }

    WId proxy_target = target;
    int target_version = 1;

    {
	Atom   type = None;
	int r, f;
	unsigned long n, a;
	WId *proxy_id;
	tqt_ignore_badwindow();
	r = XGetWindowProperty( tqt_xdisplay(), target, tqt_xdnd_proxy, 0,
				1, False, XA_WINDOW, &type, &f,&n,&a,(uchar**)&proxy_id );
	if ( ( r != Success ) || tqt_badwindow() ) {
	    proxy_target = target = 0;
	} else if ( type == XA_WINDOW && proxy_id ) {
	    proxy_target = *proxy_id;
	    XFree(proxy_id);
	    proxy_id = 0;
	    r = XGetWindowProperty( tqt_xdisplay(), proxy_target, tqt_xdnd_proxy, 0,
				    1, False, XA_WINDOW, &type, &f,&n,&a,(uchar**)&proxy_id );
	    if ( ( r != Success ) || tqt_badwindow() || !type || !proxy_id || *proxy_id != proxy_target ) {
		// Bogus
		proxy_target = 0;
		target = 0;
	    }
	    if ( proxy_id )
		XFree(proxy_id);
	}
	if ( proxy_target ) {
	    int *tv;
	    tqt_ignore_badwindow();
	    r = XGetWindowProperty( tqt_xdisplay(), proxy_target, tqt_xdnd_aware, 0,
				    1, False, AnyPropertyType, &type, &f,&n,&a,(uchar**)&tv );
	    if ( r != Success ) {
		target = 0;
	    } else {
		target_version = TQMIN(tqt_xdnd_version,tv ? *tv : 1);
		if ( tv )
		    XFree( tv );
		if (!(!tqt_badwindow() && type))
		    target = 0;
	    }
	}
    }

    if ( target != tqt_xdnd_current_target ) {
	if ( tqt_xdnd_current_target )
	    tqt_xdnd_send_leave();

	tqt_xdnd_current_target = target;
	tqt_xdnd_current_proxy_target = proxy_target;
	if ( target ) {
	    TQMemArray<Atom> type;
	    int flags = target_version << 24;
	    const char* fmt;
	    int nfmt=0;
	    for (nfmt=0; (fmt=object->format(nfmt)); nfmt++) {
		type.resize(nfmt+1);
		type[nfmt] = *tqt_xdnd_str_to_atom( fmt );
	    }
	    if ( nfmt >= 3 ) {
		XChangeProperty( TQPaintDevice::x11AppDisplay(),
				 object->source()->winId(), tqt_xdnd_type_list,
				 XA_ATOM, 32, PropModeReplace,
				 (unsigned char *)type.data(),
				 type.size() );
		flags |= 0x0001;
	    }
	    XClientMessageEvent enter;
	    enter.type = ClientMessage;
	    enter.window = target;
	    enter.format = 32;
	    enter.message_type = tqt_xdnd_enter;
	    enter.data.l[0] = object->source()->winId();
	    enter.data.l[1] = flags;
	    enter.data.l[2] = type.size()>0 ? type[0] : 0;
	    enter.data.l[3] = type.size()>1 ? type[1] : 0;
	    enter.data.l[4] = type.size()>2 ? type[2] : 0;
	    // provisionally set the rectangle to 5x5 pixels...
	    tqt_xdnd_source_sameanswer = TQRect( globalPos.x() - 2,
					       globalPos.y() -2 , 5, 5 );

	    if ( w ) {
		tqt_handle_xdnd_enter( w, (const XEvent *)&enter, false );
	    } else if ( target ) {
		XSendEvent( TQPaintDevice::x11AppDisplay(), proxy_target, False, NoEventMask,
			    (XEvent*)&enter );
	    }
	}
    }

    if ( target ) {
	XClientMessageEvent move;
	move.type = ClientMessage;
	move.window = target;
	move.format = 32;
	move.message_type = tqt_xdnd_position;
	move.window = target;
	move.data.l[0] = object->source()->winId();
	move.data.l[1] = 0; // flags
	move.data.l[2] = (globalPos.x() << 16) + globalPos.y();
	move.data.l[3] = tqt_x_time;
	move.data.l[4] = qtaction_to_xdndaction( global_requested_action );

	if ( w )
	    tqt_handle_xdnd_position( w, (const XEvent *)&move, false );
	else
	    XSendEvent( TQPaintDevice::x11AppDisplay(), proxy_target, False, NoEventMask,
			(XEvent*)&move );
    } else {
	if ( willDrop ) {
	    willDrop = false;
	    updateCursor();
	}
    }
}


void TQDragManager::drop()
{
    killTimer( heartbeat );
    heartbeat = -1;
    if ( !tqt_xdnd_current_target )
	return;

    delete tqt_xdnd_deco;
    tqt_xdnd_deco = 0;

    XClientMessageEvent drop;
    drop.type = ClientMessage;
    drop.window = tqt_xdnd_current_target;
    drop.format = 32;
    drop.message_type = tqt_xdnd_drop;
    drop.data.l[0] = object->source()->winId();
    drop.data.l[1] = 0; // flags
    drop.data.l[2] = tqt_x_time;
    drop.data.l[3] = 0;
    drop.data.l[4] = 0;

    TQWidget * w = TQWidget::find( tqt_xdnd_current_proxy_target );

    if ( w && w->isDesktop() && !w->acceptDrops() )
	w = 0;

    if ( w )
	tqt_handle_xdnd_drop( w, (const XEvent *)&drop, false );
    else
	XSendEvent( TQPaintDevice::x11AppDisplay(), tqt_xdnd_current_proxy_target, False,
		    NoEventMask, (XEvent*)&drop );

#ifndef TQT_NO_CURSOR
    if ( restoreCursor ) {
	TQApplication::restoreOverrideCursor();
	restoreCursor = false;
    }
#endif
}



bool tqt_xdnd_handle_badwindow()
{
    if ( tqt_xdnd_source_object && tqt_xdnd_current_target ) {
	tqt_xdnd_current_target = 0;
	tqt_xdnd_current_proxy_target = 0;
	delete tqt_xdnd_source_object;
	tqt_xdnd_source_object = 0;
	delete tqt_xdnd_deco;
	tqt_xdnd_deco = 0;
	return true;
    }
    if ( tqt_xdnd_dragsource_xid ) {
	tqt_xdnd_dragsource_xid = 0;
	if ( tqt_xdnd_current_widget ) {
	    TQDragLeaveEvent e;
	    TQApplication::sendEvent( tqt_xdnd_current_widget, &e );
	    tqt_xdnd_current_widget = 0;
	}
	return true;
    }
    return false;
}


/*!
    \class TQDragMoveEvent ntqevent.h
    \ingroup events
    \ingroup draganddrop
    \brief The TQDragMoveEvent class provides an event which is sent while a drag and drop is in progress.

    When a widget \link TQWidget::setAcceptDrops() accepts drop
    events\endlink, it will receive this event repeatedly while the
    drag is within the widget's boundaries. The widget should examine
    the event to see what data it \link TQDragMoveEvent::provides()
    provides\endlink, and accept() the drop if appropriate.

    Note that this class inherits most of its functionality from
    TQDropEvent.
*/


/*!
    Returns true if this event provides format \a mimeType; otherwise
    returns false.

    \sa data()
*/

bool TQDropEvent::provides( const char *mimeType ) const
{
    if ( tqt_motifdnd_active && tqstrnicmp( mimeType, "text/", 5 ) == 0 )
	return true;

    int n=0;
    const char* f;
    do {
	f = format( n );
	if ( !f )
	    return false;
	n++;
    } while( tqstricmp( mimeType, f ) );
    return true;
}

void tqt_xdnd_handle_selection_request( const XSelectionRequestEvent * req )
{
    if ( !req )
	return;
    XEvent evt;
    evt.xselection.type = SelectionNotify;
    evt.xselection.display = req->display;
    evt.xselection.requestor = req->requestor;
    evt.xselection.selection = req->selection;
    evt.xselection.target = req->target;
    evt.xselection.property = None;
    evt.xselection.time = req->time;
    const char* format = tqt_xdnd_atom_to_str( req->target );
    if ( format && tqt_xdnd_source_object &&
	 tqt_xdnd_source_object->provides( format ) ) {
	TQByteArray a = tqt_xdnd_source_object->encodedData(format);
	XChangeProperty ( TQPaintDevice::x11AppDisplay(), req->requestor, req->property,
			  req->target, 8, PropModeReplace,
			  (unsigned char *)a.data(), a.size() );
	evt.xselection.property = req->property;
    }
    // ### this can die if req->requestor crashes at the wrong
    // ### moment
    XSendEvent( TQPaintDevice::x11AppDisplay(), req->requestor, False, 0, &evt );
}

/*
	XChangeProperty ( TQPaintDevice::x11AppDisplay(), req->requestor, req->property,
			  XA_STRING, 8,
			  PropModeReplace,
			  (uchar *)d->text(), strlen(d->text()) );
	evt.xselection.property = req->property;
*/

static TQByteArray tqt_xdnd_obtain_data( const char *format )
{
    TQByteArray result;

    TQWidget* w;
    if ( tqt_xdnd_dragsource_xid && tqt_xdnd_source_object &&
	 (w=TQWidget::find( tqt_xdnd_dragsource_xid ))
	   && (!w->isDesktop() || w->acceptDrops()) )
    {
	TQDragObject * o = tqt_xdnd_source_object;
	if ( o->provides( format ) )
	    result = o->encodedData(format);
	return result;
    }

    Atom * a = tqt_xdnd_str_to_atom( format );
    if ( !a || !*a )
	return result;

    if ( !tqt_xdnd_target_data )
	tqt_xdnd_target_data = new TQIntDict<TQByteArray>( 17 );

    if ( tqt_xdnd_target_data->find( (int)*a ) ) {
	result = *(tqt_xdnd_target_data->find( (int)*a ));
    } else {
	if ( XGetSelectionOwner( TQPaintDevice::x11AppDisplay(),
				 tqt_xdnd_selection ) == None )
	    return result; // should never happen?

	TQWidget* tw = tqt_xdnd_current_widget;
	if ( !tqt_xdnd_current_widget ||
	     tqt_xdnd_current_widget->isDesktop() ) {
	    tw = new TQWidget;
	}
	XConvertSelection( TQPaintDevice::x11AppDisplay(),
			   tqt_xdnd_selection,
                           *a,
			   tqt_xdnd_selection,
			   tw->winId(),
                           tqt_xdnd_target_current_time );
	XFlush( TQPaintDevice::x11AppDisplay() );

	XEvent xevent;
	bool got=tqt_xclb_wait_for_event( TQPaintDevice::x11AppDisplay(),
				      tw->winId(),
				      SelectionNotify, &xevent, 5000);
	if ( got ) {
	    Atom type;

	    if ( tqt_xclb_read_property( TQPaintDevice::x11AppDisplay(),
					tw->winId(),
					tqt_xdnd_selection, true,
					&result, 0, &type, 0, false ) ) {
		if ( type == tqt_incr_atom ) {
		    int nbytes = result.size() >= 4 ? *((int*)result.data()) : 0;
		    result = tqt_xclb_read_incremental_property( TQPaintDevice::x11AppDisplay(),
								tw->winId(),
								tqt_xdnd_selection,
								nbytes, false );
		} else if ( type != *a ) {
		    // (includes None) tqDebug( "TQt clipboard: unknown atom %ld", type);
		}
#if 0
		// this needs to be matched by a tqt_xdnd_target_data->clear()
		// when each drag is finished. for 2.0, we do the safe thing
		// and disable the entire caching.
		if ( type != None )
		    tqt_xdnd_target_data->insert( (int)((long)a), new TQByteArray(result) );
#endif
	    }
	}
	if ( !tqt_xdnd_current_widget ||
	     tqt_xdnd_current_widget->isDesktop() ) {
	    delete tw;
	}
    }

    return result;
}


/*
  Enable drag and drop for widget w by installing the proper
  properties on w's toplevel widget.
*/
bool tqt_dnd_enable( TQWidget* w, bool on )
{
    w = w->topLevelWidget();

    if ( on ) {
	if ( ( (TQExtraWidget*)w)->topData()->dnd )
	    return true; // been there, done that
	((TQExtraWidget*)w)->topData()->dnd  = 1;
    }

    tqt_motifdnd_enable( w, on );
    return tqt_xdnd_enable( w, on );
}


/*!
    \class TQDropEvent ntqevent.h
    \ingroup events
    \ingroup draganddrop

    \brief The TQDropEvent class provides an event which is sent when a drag and drop is completed.

    When a widget \link TQWidget::setAcceptDrops() accepts drop
    events\endlink, it will receive this event if it has accepted the
    most recent TQDragEnterEvent or TQDragMoveEvent sent to it.

    The widget should use data() to extract the data in an appropriate
    format.
*/


/*!
    \fn TQDropEvent::TQDropEvent (const TQPoint & pos, Type typ)

    Constructs a drop event that drops a drop of type \a typ on point
    \a pos.
*/ // ### pos is in which coordinate system?


/*!
    Returns a byte array containing the drag's data, in \a format.

    data() normally needs to get the data from the drag source, which
    is potentially very slow, so it's advisable to call this function
    only if you're sure that you will need the data in \a format.

    The resulting data will have a size of 0 if the format was not
    available.

    \sa format() TQByteArray::size()
*/

TQByteArray TQDropEvent::encodedData( const char *format ) const
{
    if ( tqt_motifdnd_active )
	return tqt_motifdnd_obtain_data( format );
    return tqt_xdnd_obtain_data( format );
}

/*!
    Returns a string describing one of the available data types for
    this drag. Common examples are "text/plain" and "image/gif". If \a
    n is less than zero or greater than the number of available data
    types, format() returns 0.

    This function is provided mainly for debugging. Most drop targets
    will use provides().

    \sa data() provides()
*/

const char* TQDropEvent::format( int n ) const
{
    if ( tqt_motifdnd_active )
	return tqt_motifdnd_format( n );

    int i = 0;
    while( i<n && tqt_xdnd_types[i] )
	i++;
    if ( i < n )
	return 0;

    const char* name = tqt_xdnd_atom_to_str( tqt_xdnd_types[i] );
    if ( !name )
	return 0; // should never happen

    return name;
}

bool TQDragManager::drag( TQDragObject * o, TQDragObject::DragMode mode )
{
    if ( object == o || !o || !o->parent() )
	return false;

    if ( object ) {
	cancel();
	tqApp->removeEventFilter( this );
	beingCancelled = false;
    }

    if ( tqt_xdnd_source_object ) {
	// the last drag and drop operation hasn't finished, so we are going to wait
	// for one second to see if it does... if the finish message comes after this,
	// then we could still have problems, but this is highly unlikely
	TQApplication::flushX();

	TQTime started = TQTime::currentTime();
	TQTime now = started;
	do {
	    XEvent event;
	    if ( XCheckTypedEvent( TQPaintDevice::x11AppDisplay(),
				   ClientMessage, &event ) )
		tqApp->x11ProcessEvent( &event );

	    now = TQTime::currentTime();
	    if ( started > now ) // crossed midnight
		started = now;

	    // sleep 50ms, so we don't use up CPU cycles all the time.
	    struct timeval usleep_tv;
	    usleep_tv.tv_sec = 0;
	    usleep_tv.tv_usec = 50000;
	    select(0, 0, 0, 0, &usleep_tv);
	} while ( tqt_xdnd_source_object && started.msecsTo(now) < 1000 );
    }

    tqt_xdnd_source_object = o;
    tqt_xdnd_source_object->setTarget( 0 );
    tqt_xdnd_deco = new TQShapedPixmapWidget();

    willDrop = false;

    object = o;
    updatePixmap();

    dragSource = (TQWidget *)(object->parent());

    tqt_xdnd_deco->x11SetWindowTransient( dragSource->topLevelWidget());
    tqApp->installEventFilter( this );
    tqt_xdnd_source_current_time = tqt_x_time;
    XSetSelectionOwner( TQPaintDevice::x11AppDisplay(), tqt_xdnd_selection,
			dragSource->topLevelWidget()->winId(),
			tqt_xdnd_source_current_time );
    oldstate = ButtonState(-1); // #### Should use state that caused the drag
    drag_mode = mode;
    global_accepted_action = TQDropEvent::Copy;
    updateMode(ButtonState(0));
    tqt_xdnd_source_sameanswer = TQRect();
    move(TQCursor::pos());
    heartbeat = startTimer(200);
    need_modifiers_check = false;

#ifndef TQT_NO_CURSOR
    tqApp->setOverrideCursor( arrowCursor );
    restoreCursor = true;
    updateCursor();
#endif

    dndCancelled = false;
    tqt_xdnd_dragging = true;

    if (!TQWidget::mouseGrabber())
	tqt_xdnd_deco->grabMouse();

    tqApp->enter_loop(); // Do the DND.

#ifndef TQT_NO_CURSOR
    tqApp->restoreOverrideCursor();
#endif

    delete tqt_xdnd_deco;
    tqt_xdnd_deco = 0;
    killTimer( heartbeat );
    heartbeat = -1;
    tqt_xdnd_current_screen = -1;
    tqt_xdnd_dragging = false;

    return ((! dndCancelled) && // source del?
	    (global_accepted_action == TQDropEvent::Copy &&
	     global_requested_action == TQDropEvent::Move));

    // tqt_xdnd_source_object persists until we get an xdnd_finish message
}

void TQDragManager::updatePixmap( const TQPoint& cursorPos )
{
    if ( tqt_xdnd_deco ) {
	TQPixmap pm;
	TQPoint pm_hot(default_pm_hotx,default_pm_hoty);
	if ( object ) {
	    pm = object->pixmap();
	    if ( !pm.isNull() )
		pm_hot = object->pixmapHotSpot();
	}
	if ( pm.isNull() ) {
	    if ( !defaultPm )
		defaultPm = new TQPixmap(default_pm);
	    pm = *defaultPm;
	}
	tqt_xdnd_deco->setPixmap(pm, pm_hot);
	tqt_xdnd_deco->move(cursorPos-pm_hot);
	    //if ( willDrop ) {
	    tqt_xdnd_deco->show();
	    //} else {
	    //    tqt_xdnd_deco->hide();
	    //}
    }
}

void TQDragManager::updatePixmap()
{
    updatePixmap( TQCursor::pos());
}

#endif // TQT_NO_DRAGANDDROP
