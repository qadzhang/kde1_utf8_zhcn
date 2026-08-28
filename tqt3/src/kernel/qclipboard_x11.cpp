/****************************************************************************
**
** Implementation of TQClipboard class for X11
**
** Created : 960430
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

// #define TQCLIPBOARD_DEBUG
// #define TQCLIPBOARD_DEBUG_VERBOSE

#ifdef TQCLIPBOARD_DEBUG
#  define TQDEBUG tqDebug
#else
#  define TQDEBUG if (false) tqDebug
#endif

#ifdef TQCLIPBOARD_DEBUG_VERBOSE
#  define VTQDEBUG tqDebug
#else
#  define VTQDEBUG if (false) tqDebug
#endif

#include "qplatformdefs.h"

// POSIX Large File Support redefines open -> open64
#if defined(open)
# undef open
#endif

#include "ntqclipboard.h"

#ifndef TQT_NO_CLIPBOARD

#include "ntqapplication.h"
#include "ntqeventloop.h"
#include "ntqbitmap.h"
#include "ntqdatetime.h"
#include "ntqdragobject.h"
#include "ntqbuffer.h"
#include "ntqtextcodec.h"
#include "ntqvaluelist.h"
#include "ntqmap.h"
#include "qt_x11_p.h"
#include "qapplication_p.h"


// REVISED: arnt

/*****************************************************************************
  Internal TQClipboard functions for X11.
 *****************************************************************************/

// from qapplication_x11.cpp
typedef int (*QX11EventFilter) (XEvent*);
extern QX11EventFilter tqt_set_x11_event_filter (QX11EventFilter filter);

extern Time tqt_x_time;			// def. in qapplication_x11.cpp
extern Time tqt_x_incr;			// def. in qapplication_x11.cpp
extern Atom tqt_xa_clipboard;
extern Atom tqt_selection_property;
extern Atom tqt_clipboard_sentinel;
extern Atom tqt_selection_sentinel;
extern Atom tqt_utf8_string;

// from qdnd_x11.cpp
extern Atom* tqt_xdnd_str_to_atom( const char *mimeType );
extern const char* tqt_xdnd_atom_to_str( Atom );


static int clipboard_timeout = 5000; // 5s timeout on clipboard operations

static TQWidget * owner = 0;
static TQWidget *requestor = 0;
static bool inSelectionMode_obsolete = false; // ### remove 4.0
static bool timer_event_clear = false;
static int timer_id = 0;

static int pending_timer_id = 0;
static bool pending_clipboard_changed = false;
static bool pending_selection_changed = false;

TQ_EXPORT bool tqt_qclipboard_bailout_hack = false;

// event capture mechanism for tqt_xclb_wait_for_event
static bool waiting_for_data = false;
static bool has_captured_event = false;
static Window capture_event_win = None;
static int capture_event_type = -1;
static XEvent captured_event;

class TQClipboardWatcher; // forward decl
static TQClipboardWatcher *selection_watcher = 0;
static TQClipboardWatcher *clipboard_watcher = 0;

static void cleanup()
{
    delete owner;
    delete requestor;
    owner = 0;
    requestor = 0;
}

static
void setupOwner()
{
    if ( owner )
	return;
    owner = new TQWidget( 0, "internal clipboard owner" );
    requestor = new TQWidget(0, "internal clipboard requestor");
    tqAddPostRoutine( cleanup );
}

static
int sizeof_format(int format)
{
    int sz;
    switch (format) {
    default:
    case  8: sz = sizeof( char); break;
    case 16: sz = sizeof(short); break;
    case 32: sz = sizeof( long); break;
    }
    return sz;
}

class TQClipboardWatcher : public TQMimeSource {
public:
    TQClipboardWatcher( TQClipboard::Mode mode );
    ~TQClipboardWatcher();
    bool empty() const;
    const char* format( int n ) const;
    TQByteArray encodedData( const char* fmt ) const;
    TQByteArray getDataInFormat(Atom fmtatom) const;

    Atom atom;
    mutable TQValueList<const char *> formatList;
};



class TQClipboardData
{
public:
    TQClipboardData();
    ~TQClipboardData();

    void setSource(TQMimeSource* s)
    {
	clear(true);
	src = s;
    }

    TQMimeSource *source() const { return src; }

    void addTransferredPixmap(TQPixmap pm)
    {
	/* TODO: queue them */
	transferred[tindex] = pm;
	tindex=(tindex+1)%2;
    }
    void clearTransfers()
    {
	transferred[0] = TQPixmap();
	transferred[1] = TQPixmap();
    }

    void clear(bool destruct=true);

    TQMimeSource *src;
    Time timestamp;

    TQPixmap transferred[2];
    int tindex;
};

TQClipboardData::TQClipboardData()
{
    src = 0;
    timestamp = CurrentTime;
    tindex=0;
}

TQClipboardData::~TQClipboardData()
{ clear(); }

void TQClipboardData::clear(bool destruct)
{
    if(destruct)
	delete src;
    src = 0;
    timestamp = CurrentTime;
}


static TQClipboardData *internalCbData = 0;
static TQClipboardData *internalSelData = 0;

static void cleanupClipboardData()
{
    delete internalCbData;
    internalCbData = 0;
}

static TQClipboardData *clipboardData()
{
    if ( internalCbData == 0 ) {
	internalCbData = new TQClipboardData;
	TQ_CHECK_PTR( internalCbData );
	tqAddPostRoutine( cleanupClipboardData );
    }
    return internalCbData;
}

void tqt_clipboard_cleanup_mime_source(TQMimeSource *src)
{
    if(internalCbData && internalCbData->source() == src)
	internalCbData->clear(false);
}

static void cleanupSelectionData()
{
    delete internalSelData;
    internalSelData = 0;
}

static TQClipboardData *selectionData()
{
    if (internalSelData == 0) {
	internalSelData = new TQClipboardData;
	TQ_CHECK_PTR(internalSelData);
	tqAddPostRoutine(cleanupSelectionData);
    }
    return internalSelData;
}

class TQClipboardINCRTransaction
{
public:
    TQClipboardINCRTransaction(Window w, Atom p, Atom t, int f, TQByteArray d, unsigned int i);
    ~TQClipboardINCRTransaction(void);

    int x11Event(XEvent *event);

    Window window;
    Atom property, target;
    int format;
    TQByteArray data;
    unsigned int increment;
    unsigned int offset;
};

typedef TQMap<Window,TQClipboardINCRTransaction*> TransactionMap;
static TransactionMap *transactions = 0;
static QX11EventFilter prev_x11_event_filter = 0;
static int incr_timer_id = 0;

static int tqt_xclb_transation_event_handler(XEvent *event)
{
    TransactionMap::Iterator it = transactions->find(event->xany.window);
    if (it != transactions->end()) {
	if ((*it)->x11Event(event) != 0)
	    return 1;
    }
    if (prev_x11_event_filter)
	return prev_x11_event_filter(event);
    return 0;
}

/*
  called when no INCR activity has happened for 'clipboard_timeout'
  milliseconds... we assume that all unfinished transactions have
  timed out and remove everything from the transaction map
*/
static void tqt_xclb_incr_timeout(void)
{
    tqWarning("TQClipboard: timed out while sending data");

    while (transactions)
        delete *transactions->begin();
}

TQClipboardINCRTransaction::TQClipboardINCRTransaction(Window w, Atom p, Atom t, int f,
						     TQByteArray d, unsigned int i)
    : window(w), property(p), target(t), format(f), data(d), increment(i), offset(0u)
{
    TQDEBUG("TQClipboard: sending %d bytes (INCR transaction %p)", d.size(), this);

    XSelectInput(TQPaintDevice::x11AppDisplay(), window, PropertyChangeMask);

    if (! transactions) {
	VTQDEBUG("TQClipboard: created INCR transaction map");
	transactions = new TransactionMap;
	prev_x11_event_filter = tqt_set_x11_event_filter(tqt_xclb_transation_event_handler);

	incr_timer_id = TQApplication::clipboard()->startTimer(clipboard_timeout);
    }
    transactions->insert(window, this);
}

TQClipboardINCRTransaction::~TQClipboardINCRTransaction(void)
{
    VTQDEBUG("TQClipboard: destroyed INCR transacton %p", this);

    XSelectInput(TQPaintDevice::x11AppDisplay(), window, NoEventMask);

    transactions->remove(window);
    if (transactions->isEmpty()) {
	VTQDEBUG("TQClipboard: no more INCR transations");
	delete transactions;
	transactions = 0;
	(void)tqt_set_x11_event_filter(prev_x11_event_filter);

	if (incr_timer_id != 0) {
	    TQApplication::clipboard()->killTimer(incr_timer_id);
	    incr_timer_id = 0;
	}
    }
}

int TQClipboardINCRTransaction::x11Event(XEvent *event)
{
    if (event->type != PropertyNotify
	|| (event->xproperty.state != PropertyDelete
	    || event->xproperty.atom != property))
	return 0;

    // restart the INCR timer
    if (incr_timer_id) TQApplication::clipboard()->killTimer(incr_timer_id);
    incr_timer_id = TQApplication::clipboard()->startTimer(clipboard_timeout);

    unsigned int bytes_left = data.size() - offset;
    if (bytes_left > 0) {
	unsigned int xfer = TQMIN(increment, bytes_left);
	VTQDEBUG("TQClipboard: sending %d bytes, %d remaining (INCR transaction %p)",
	       xfer, bytes_left - xfer, this);

	XChangeProperty(TQPaintDevice::x11AppDisplay(), window, property, target, format,
			PropModeReplace, (uchar *) data.data() + offset, xfer);
	offset += xfer;
    } else {
	// INCR transaction finished...
	XChangeProperty(TQPaintDevice::x11AppDisplay(), window, property, target, format,
			PropModeReplace, (uchar *) data.data(), 0);
       	delete this;
    }

    return 1;
}


/*****************************************************************************
  TQClipboard member functions for X11.
 *****************************************************************************/


void TQClipboard::clear( Mode mode )
{ setData(0, mode); }


/*!
    Returns true if the clipboard supports mouse selection; otherwise
    returns false.
*/
bool TQClipboard::supportsSelection() const
{ return true; }


/*!
    Returns true if this clipboard object owns the mouse selection
    data; otherwise returns false.
*/
bool TQClipboard::ownsSelection() const
{ return selectionData()->timestamp != CurrentTime; }

/*!
    Returns true if this clipboard object owns the clipboard data;
    otherwise returns false.
*/
bool TQClipboard::ownsClipboard() const
{ return clipboardData()->timestamp != CurrentTime; }


/*! \obsolete

    Use the TQClipboard::data(), TQClipboard::setData() and related functions
    which take a TQClipboard::Mode argument.

    Sets the clipboard selection mode. If \a enable is true, then
    subsequent calls to TQClipboard::setData() and other functions
    which put data into the clipboard will put the data into the mouse
    selection, otherwise the data will be put into the clipboard.

    \sa supportsSelection(), selectionModeEnabled()
*/
void TQClipboard::setSelectionMode(bool enable)
{ inSelectionMode_obsolete = enable; }


/*! \obsolete

    Use the TQClipboard::data(), TQClipboard::setData() and related functions
    which take a TQClipboard::Mode argument.

    Returns the selection mode.

    \sa setSelectionMode(), supportsSelection()
*/
bool TQClipboard::selectionModeEnabled() const
{ return inSelectionMode_obsolete; }


// event filter function... captures interesting events while
// tqt_xclb_wait_for_event is running the event loop
static int tqt_xclb_event_filter(XEvent *event)
{
    if (event->xany.type == capture_event_type &&
	event->xany.window == capture_event_win) {
	VTQDEBUG( "TQClipboard: event_filter(): caught event type %d", event->type );
	has_captured_event = true;
	captured_event = *event;
	return 1;
    }

    return 0;
}

static Bool checkForClipboardEvents(Display *, XEvent *e, XPointer)
{
    return ((e->type == SelectionRequest && (e->xselectionrequest.selection == XA_PRIMARY
                                             || e->xselectionrequest.selection == tqt_xa_clipboard))
            || (e->type == SelectionClear && (e->xselectionclear.selection == XA_PRIMARY
                                              || e->xselectionclear.selection == tqt_xa_clipboard)));
}

static bool selection_request_pending = false;

static Bool check_selection_request_pending( Display*, XEvent* e, XPointer )
    {
    if( e->type == SelectionRequest && e->xselectionrequest.owner == owner->winId())
        selection_request_pending = true;
    return False;
    }

bool tqt_xclb_wait_for_event( Display *dpy, Window win, int type, XEvent *event,
			     int timeout )
{
    TQTime started = TQTime::currentTime();
    TQTime now = started;

    if (tqApp->eventLoop()->inherits("TQMotif")) { // yes yes, evil hack, we know
        if ( waiting_for_data )
            tqFatal( "TQClipboard: internal error, tqt_xclb_wait_for_event recursed" );

        waiting_for_data = true;
        has_captured_event = false;
        capture_event_win = win;
        capture_event_type = type;

        QX11EventFilter old_event_filter = tqt_set_x11_event_filter(tqt_xclb_event_filter);

        do {
            if ( XCheckTypedWindowEvent(dpy,win,type,event) ) {
                waiting_for_data = false;
                tqt_set_x11_event_filter(old_event_filter);
                return true;
            }

            now = TQTime::currentTime();
            if ( started > now )			// crossed midnight
                started = now;

            tqApp->eventLoop()->processEvents(TQEventLoop::ExcludeUserInput |
                                              TQEventLoop::ExcludeSocketNotifiers |
                                              TQEventLoop::WaitForMore |
                                              TQEventLoop::ExcludeTimers);

            if ( has_captured_event ) {
                waiting_for_data = false;
                *event = captured_event;
                tqt_set_x11_event_filter(old_event_filter);
                return true;
            }
        } while ( started.msecsTo(now) < timeout );

        waiting_for_data = false;
        tqt_set_x11_event_filter(old_event_filter);

        return false;
    }

    bool flushed = false;
    do {
        if ( XCheckTypedWindowEvent(dpy,win,type,event) )
	    return true;
        if( tqt_qclipboard_bailout_hack ) {
            XEvent dummy;
            selection_request_pending = false;
            if ( owner != NULL )
                XCheckIfEvent(dpy,&dummy,check_selection_request_pending,NULL);
            if( selection_request_pending )
	        return true;
        }

        // process other clipboard events, since someone is probably requesting data from us
        XEvent e;
        if (XCheckIfEvent(dpy, &e, checkForClipboardEvents, 0))
            tqApp->x11ProcessEvent(&e);

	now = TQTime::currentTime();
	if ( started > now )			// crossed midnight
	    started = now;

	if(!flushed) {
	    XFlush( dpy );
	    flushed = true;
	}

	// sleep 50ms, so we don't use up CPU cycles all the time.
	struct timeval usleep_tv;
	usleep_tv.tv_sec = 0;
	usleep_tv.tv_usec = 50000;
	select(0, 0, 0, 0, &usleep_tv);
    } while ( started.msecsTo(now) < timeout );

    return false;
}


static inline int maxSelectionIncr( Display *dpy )
{ return XMaxRequestSize(dpy) > 65536 ? 65536*4 : XMaxRequestSize(dpy)*4 - 100; }

// uglyhack: externed into qt_xdnd.cpp. qt is really not designed for
// single-platform, multi-purpose blocks of code...
bool tqt_xclb_read_property( Display *dpy, Window win, Atom property,
			   bool deleteProperty,
			   TQByteArray *buffer, int *size, Atom *type,
			   int *format, bool nullterm )
{
    int	   maxsize = maxSelectionIncr(dpy);
    ulong  bytes_left; // bytes_after
    ulong  length;     // nitems
    uchar *data;
    Atom   dummy_type;
    int    dummy_format;
    int    r;

    if ( !type )				// allow null args
	type = &dummy_type;
    if ( !format )
	format = &dummy_format;

    // Don't read anything, just get the size of the property data
    r = XGetWindowProperty( dpy, win, property, 0, 0, False,
			    AnyPropertyType, type, format,
			    &length, &bytes_left, &data );
    if (r != Success || (type && *type == None)) {
	buffer->resize( 0 );
	return false;
    }
    XFree( (char*)data );

    int  offset = 0, buffer_offset = 0, format_inc = 1, proplen = bytes_left;

    VTQDEBUG("TQClipboard: read_property(): initial property length: %d", proplen);

    switch (*format) {
    case 8:
    default:
	format_inc = sizeof(char) / 1;
	break;

    case 16:
	format_inc = sizeof(short) / 2;
	proplen *= sizeof(short) / 2;
	break;

    case 32:
	format_inc = sizeof(long) / 4;
	proplen *= sizeof(long) / 4;
	break;
    }

    bool ok = buffer->resize( proplen + (nullterm ? 1 : 0) );

    VTQDEBUG("TQClipboard: read_property(): buffer resized to %d", buffer->size());

    if ( ok ) {
	// could allocate buffer

	while ( bytes_left ) {
	    // more to read...

	    r = XGetWindowProperty( dpy, win, property, offset, maxsize/4,
				    False, AnyPropertyType, type, format,
				    &length, &bytes_left, &data );
	    if (r != Success || (type && *type == None))
		break;

	    offset += length / (32 / *format);
	    length *= format_inc * (*format) / 8;

	    // Here we check if we get a buffer overflow and tries to
	    // recover -- this shouldn't normally happen, but it doesn't
	    // hurt to be defensive
	    if (buffer_offset + length > buffer->size()) {
		length = buffer->size() - buffer_offset;

		// escape loop
		bytes_left = 0;
	    }

	    memcpy(buffer->data() + buffer_offset, data, length);
	    buffer_offset += length;

	    XFree( (char*)data );
	}

	static Atom xa_compound_text = *tqt_xdnd_str_to_atom( "COMPOUND_TEXT" );
 	if ( *format == 8 && *type == xa_compound_text ) {
	    // convert COMPOUND_TEXT to a multibyte string
 	    XTextProperty textprop;
 	    textprop.encoding = *type;
 	    textprop.format = *format;
 	    textprop.nitems = length;
 	    textprop.value = (unsigned char *) buffer->data();

 	    char **list_ret = 0;
 	    int count;
	    if ( XmbTextPropertyToTextList( dpy, &textprop, &list_ret,
					    &count ) == Success &&
		 count && list_ret ) {
		offset = strlen( list_ret[0] );
		buffer->resize( offset + ( nullterm ? 1 : 0 ) );
		memcpy( buffer->data(), list_ret[0], offset );
	    }
 	    if (list_ret) XFreeStringList(list_ret);
 	}

	// zero-terminate (for text)
       	if (nullterm)
	    buffer->at(buffer_offset) = '\0';
    }

    // correct size, not 0-term.
    if ( size )
	*size = buffer_offset;

    VTQDEBUG("TQClipboard: read_property(): buffer size %d, buffer offset %d, offset %d",
	   buffer->size(), buffer_offset, offset);

    if ( deleteProperty )
	XDeleteProperty( dpy, win, property );

    XFlush( dpy );

    return ok;
}


// this is externed into qt_xdnd.cpp too.
TQByteArray tqt_xclb_read_incremental_property( Display *dpy, Window win,
					        Atom property, int nbytes,
					        bool nullterm )
{
    XEvent event;

    TQByteArray buf;
    TQByteArray tmp_buf;
    bool alloc_error = false;
    int  length;
    int  offset = 0;

    if ( nbytes > 0 ) {
	// Reserve buffer + zero-terminator (for text data)
	// We want to complete the INCR transfer even if we cannot
	// allocate more memory
	alloc_error = !buf.resize(nbytes+1);
    }

    for (;;) {
	XFlush( dpy );
	if ( !tqt_xclb_wait_for_event(dpy,win,PropertyNotify,&event,clipboard_timeout) )
	    break;
	if ( event.xproperty.atom != property ||
	     event.xproperty.state != PropertyNewValue )
	    continue;
	if ( tqt_xclb_read_property(dpy, win, property, true, &tmp_buf,
				   &length,0, 0, false) ) {
	    if ( length == 0 ) {		// no more data, we're done
		if ( nullterm ) {
		    buf.resize( offset+1 );
		    buf.at( offset ) = '\0';
		} else {
		    buf.resize(offset);
		}
		return buf;
	    } else if ( !alloc_error ) {
		if ( offset+length > (int)buf.size() ) {
		    if ( !buf.resize(offset+length+65535) ) {
			alloc_error = true;
			length = buf.size() - offset;
		    }
		}
		memcpy( buf.data()+offset, tmp_buf.data(), length );
		tmp_buf.resize( 0 );
		offset += length;
	    }
	} else {
	    break;
	}
    }

    // timed out ... create a new requestor window, otherwise the requestor
    // could consider next request to be still part of this timed out request
    delete requestor;
    requestor = new TQWidget( 0, "internal clipboard requestor" );

    return TQByteArray();
}

static Atom send_selection(TQClipboardData *d, Atom target, Window window, Atom property,
			   int format = 0, TQByteArray data = TQByteArray());

static Atom send_targets_selection(TQClipboardData *d, Window window, Atom property)
{
    int atoms = 0;
    while (d->source()->format(atoms)) atoms++;
    if (d->source()->provides("image/ppm")) atoms++;
    if (d->source()->provides("image/pbm")) atoms++;
    if (d->source()->provides("text/plain")) atoms+=4;

    VTQDEBUG("TQClipboard: send_targets_selection(): %d provided types", atoms);

    // for 64 bit cleanness... XChangeProperty expects long* for data with format == 32
    TQByteArray data((atoms+3) * sizeof(long)); // plus TARGETS, MULTIPLE and TIMESTAMP
    long *atarget = (long *) data.data();

    const char *fmt;
    int n = 0;
    while ((fmt=d->source()->format(n)) && n < atoms)
	atarget[n++] = *tqt_xdnd_str_to_atom(fmt);

    static Atom xa_text = *tqt_xdnd_str_to_atom("TEXT");
    static Atom xa_compound_text = *tqt_xdnd_str_to_atom("COMPOUND_TEXT");
    static Atom xa_targets = *tqt_xdnd_str_to_atom("TARGETS");
    static Atom xa_multiple = *tqt_xdnd_str_to_atom("MULTIPLE");
    static Atom xa_timestamp = *tqt_xdnd_str_to_atom("TIMESTAMP");

    if (d->source()->provides("image/ppm"))
	atarget[n++] = XA_PIXMAP;
    if (d->source()->provides("image/pbm"))
	atarget[n++] = XA_BITMAP;
    if (d->source()->provides("text/plain")) {
	atarget[n++] = tqt_utf8_string;
	atarget[n++] = xa_text;
	atarget[n++] = xa_compound_text;
	atarget[n++] = XA_STRING;
    }

    atarget[n++] = xa_targets;
    atarget[n++] = xa_multiple;
    atarget[n++] = xa_timestamp;

#if defined(TQCLIPBOARD_DEBUG_VERBOSE)
    for (int index = 0; index < n; index++) {
	VTQDEBUG("    atom %d: 0x%lx (%s)", index, atarget[index],
	       tqt_xdnd_atom_to_str(atarget[index]));
    }
#endif

    XChangeProperty(TQPaintDevice::x11AppDisplay(), window, property, XA_ATOM, 32,
		    PropModeReplace, (uchar *) data.data(), n);
    return property;
}

static Atom send_string_selection(TQClipboardData *d, Atom target, Window window, Atom property)
{
    static Atom xa_text = *tqt_xdnd_str_to_atom("TEXT");
    static Atom xa_compound_text = *tqt_xdnd_str_to_atom("COMPOUND_TEXT");

    TQDEBUG("TQClipboard: send_string_selection():\n"
	  "    property type %lx\n"
	  "    property name '%s'",
	  target, tqt_xdnd_atom_to_str(target));

    if (target == xa_text || target == xa_compound_text) {
	// the ICCCM states that TEXT and COMPOUND_TEXT are in the
	// encoding of choice, so we choose the encoding of the locale
	TQByteArray data = d->source()->encodedData("text/plain");
	if(data.resize(data.size() + 1))
	    data[int(data.size() - 1)] = '\0';
	char *list[] = { data.data(), NULL };

	XICCEncodingStyle style =
	    (target == xa_compound_text) ? XCompoundTextStyle : XStdICCTextStyle;
	XTextProperty textprop;
	if (list[0] != NULL
	    && XmbTextListToTextProperty(TQPaintDevice::x11AppDisplay(),
					 list, 1, style, &textprop) == Success) {
	    TQDEBUG("    textprop type %lx\n"
		  "    textprop name '%s'\n"
		  "    format %d\n"
		  "    %ld items",
		  textprop.encoding, tqt_xdnd_atom_to_str(textprop.encoding),
		  textprop.format, textprop.nitems);

	    int sz = sizeof_format(textprop.format);
	    data.duplicate((const char *) textprop.value, textprop.nitems * sz);
	    XFree(textprop.value);

	    return send_selection(d, textprop.encoding, window, property, textprop.format, data);
	}

	return None;
    }

    Atom xtarget = None;
    const char *fmt = 0;
    if (target == XA_STRING
	|| (target == xa_text && TQTextCodec::codecForLocale()->mibEnum() == 4)) {
	// the ICCCM states that STRING is latin1 plus newline and tab
	// see section 2.6.2
	fmt = "text/plain;charset=ISO-8859-1";
	xtarget = XA_STRING;
    } else if (target == tqt_utf8_string) {
	// proposed UTF8_STRING conversion type
	fmt = "text/plain;charset=UTF-8";
	xtarget = tqt_utf8_string;
    }

    if (xtarget == None) // should not happen
	return None;

    TQByteArray data = d->source()->encodedData(fmt);

    TQDEBUG("    format 8\n    %d bytes", data.size());

    return send_selection(d, xtarget, window, property, 8, data);
}

static Atom send_pixmap_selection(TQClipboardData *d, Atom target, Window window, Atom property)
{
    TQPixmap pm;

    if ( target == XA_PIXMAP ) {
	TQByteArray data = d->source()->encodedData("image/ppm");
	pm.loadFromData(data);
    } else if ( target == XA_BITMAP ) {
	TQByteArray data = d->source()->encodedData("image/pbm");
	TQImage img;
	img.loadFromData(data);
	if ( img.depth() != 1 )
	    img = img.convertDepth(1);
    }

    if (pm.isNull()) // should never happen
	return None;

    Pixmap handle = pm.handle();
    XChangeProperty(TQPaintDevice::x11AppDisplay(), window, property,
		    target, 32, PropModeReplace, (uchar *) &handle, 1);
    d->addTransferredPixmap(pm);
    return property;

}

static Atom send_selection(TQClipboardData *d, Atom target, Window window, Atom property,
			   int format, TQByteArray data)
{
    if (format == 0) format = 8;

    if (data.isEmpty()) {
	const char *fmt = tqt_xdnd_atom_to_str(target);
	TQDEBUG("TQClipboard: send_selection(): converting to type '%s'", fmt);
	if (fmt && !d->source()->provides(fmt)) // Not a MIME type we can produce
	    return None;
	data = d->source()->encodedData(fmt);
    }

    TQDEBUG("TQClipboard: send_selection():\n"
	  "    property type %lx\n"
	  "    property name '%s'\n"
	  "    format %d\n"
	  "    %d bytes",
	  target, tqt_xdnd_atom_to_str(target), format, data.size());

    // don't allow INCR transfers when using MULTIPLE or to
    // Motif clients (since Motif doesn't support INCR)
    static Atom motif_clip_temporary = *tqt_xdnd_str_to_atom("CLIP_TEMPORARY");
    bool allow_incr = property != motif_clip_temporary;

    // X_ChangeProperty protocol request is 24 bytes
    const unsigned int increment = (XMaxRequestSize(TQPaintDevice::x11AppDisplay()) * 4) - 24;
    if (data.size() > increment && allow_incr) {
	long bytes = data.size();
	XChangeProperty(TQPaintDevice::x11AppDisplay(), window, property,
			tqt_x_incr, 32, PropModeReplace, (uchar *) &bytes, 1);

	(void)new TQClipboardINCRTransaction(window, property, target, format, data, increment);
	return tqt_x_incr;
    }

    // make sure we can perform the XChangeProperty in a single request
    if (data.size() > increment)
        return None; // ### perhaps use several XChangeProperty calls w/ PropModeAppend?

    // use a single request to transfer data
    XChangeProperty(TQPaintDevice::x11AppDisplay(), window, property, target,
		    format, PropModeReplace, (uchar *) data.data(),
		    data.size() / sizeof_format(format));
    return property;
}

/*! \internal
    Internal cleanup for Windows.
*/
void TQClipboard::ownerDestroyed()
{ }


/*! \internal
    Internal optimization for Windows.
*/
void TQClipboard::connectNotify( const char * )
{ }


/*! \reimp
 */
bool TQClipboard::event( TQEvent *e )
{
    if ( e->type() == TQEvent::Timer ) {
	TQTimerEvent *te = (TQTimerEvent *) e;

	if ( waiting_for_data ) // should never happen
	    return false;

	if (te->timerId() == timer_id) {
	    killTimer(timer_id);
	    timer_id = 0;

	    timer_event_clear = true;
	    if ( selection_watcher ) // clear selection
		selectionData()->clear();
	    if ( clipboard_watcher ) // clear clipboard
		clipboardData()->clear();
	    timer_event_clear = false;

	    return true;
	} else if ( te->timerId() == pending_timer_id ) {
	    // I hate klipper
	    killTimer( pending_timer_id );
	    pending_timer_id = 0;

	    if ( pending_clipboard_changed ) {
		pending_clipboard_changed = false;
		clipboardData()->clear();
		emit dataChanged();
	    }
	    if ( pending_selection_changed ) {
		pending_selection_changed = false;
		selectionData()->clear();
		emit selectionChanged();
	    }

	    return true;
	} else if (te->timerId() == incr_timer_id) {
	    killTimer(incr_timer_id);
	    incr_timer_id = 0;

	    tqt_xclb_incr_timeout();

	    return true;
	} else {
	    return TQObject::event( e );
	}
    } else if ( e->type() != TQEvent::Clipboard ) {
	return TQObject::event( e );
    }

    XEvent *xevent = (XEvent *)(((TQCustomEvent *)e)->data());
    Display *dpy = TQPaintDevice::x11AppDisplay();

    if ( !xevent )
	return true;

    switch ( xevent->type ) {

    case SelectionClear:
	// new selection owner
	if (xevent->xselectionclear.selection == XA_PRIMARY) {
	    TQClipboardData *d = selectionData();

	    // ignore the event if it was generated before we gained selection ownership
	    if (d->timestamp != CurrentTime && xevent->xselectionclear.time < d->timestamp)
		break;

	    TQDEBUG("TQClipboard: new selection owner 0x%lx at time %lx (ours %lx)",
		  XGetSelectionOwner(dpy, XA_PRIMARY),
		  xevent->xselectionclear.time, d->timestamp);

	    if ( ! waiting_for_data ) {
		d->clear();
		emit selectionChanged();
	    } else {
		pending_selection_changed = true;
		if ( ! pending_timer_id )
		    pending_timer_id = TQApplication::clipboard()->startTimer( 0 );
	    }
	} else if (xevent->xselectionclear.selection == tqt_xa_clipboard) {
	    TQClipboardData *d = clipboardData();

	    // ignore the event if it was generated before we gained selection ownership
	    if (d->timestamp != CurrentTime && xevent->xselectionclear.time < d->timestamp)
		break;

	    TQDEBUG("TQClipboard: new clipboard owner 0x%lx at time %lx (%lx)",
		  XGetSelectionOwner(dpy, tqt_xa_clipboard),
		  xevent->xselectionclear.time, d->timestamp);

	    if ( ! waiting_for_data ) {
		d->clear();
		emit dataChanged();
	    } else {
		pending_clipboard_changed = true;
		if ( ! pending_timer_id )
		    pending_timer_id = TQApplication::clipboard()->startTimer( 0 );
	    }
	} else {
#ifdef QT_CHECK_STATE
	    tqWarning("TQClipboard: Unknown SelectionClear event received.");
#endif
	    return false;
	}
	break;

    case SelectionNotify:
	/*
	  Something has delivered data to us, but this was not caught
	  by TQClipboardWatcher::getDataInFormat()

	  Just skip the event to prevent Bad Things (tm) from
	  happening later on...
	*/
	break;

    case SelectionRequest:
	{
	    // someone wants our data
	    XSelectionRequestEvent *req = &xevent->xselectionrequest;

	    if (requestor && req->requestor == requestor->winId())
                break;

	    XEvent event;
	    event.xselection.type      = SelectionNotify;
	    event.xselection.display   = req->display;
	    event.xselection.requestor = req->requestor;
	    event.xselection.selection = req->selection;
	    event.xselection.target    = req->target;
	    event.xselection.property  = None;
	    event.xselection.time      = req->time;

	    TQDEBUG("TQClipboard: SelectionRequest from %lx\n"
		  "    selection 0x%lx (%s) target 0x%lx (%s)",
		  req->requestor,
		  req->selection,
		  tqt_xdnd_atom_to_str(req->selection),
		  req->target,
		  tqt_xdnd_atom_to_str(req->target));

	    TQClipboardData *d;

	    if ( req->selection == XA_PRIMARY ) {
		d = selectionData();
	    } else if ( req->selection == tqt_xa_clipboard ) {
		d = clipboardData();
	    } else {
#ifdef QT_CHECK_RANGE
		tqWarning("TQClipboard: unknown selection '%lx'", req->selection);
#endif // QT_CHECK_RANGE

		XSendEvent(dpy, req->requestor, False, NoEventMask, &event);
		break;
	    }

	    if (! d->source()) {
#ifdef QT_CHECK_STATE
		tqWarning("TQClipboard: cannot transfer data, no data available");
#endif // QT_CHECK_STATE

		XSendEvent(dpy, req->requestor, False, NoEventMask, &event);
		break;
	    }

	    TQDEBUG("TQClipboard: SelectionRequest at time %lx (ours %lx)",
		  req->time, d->timestamp);

	    if (d->timestamp == CurrentTime // we don't own the selection anymore
		|| (req->time != CurrentTime && req->time < d->timestamp)) {
		TQDEBUG("TQClipboard: SelectionRequest too old");
		XSendEvent(dpy, req->requestor, False, NoEventMask, &event);
		break;
	    }

	    static Atom xa_text = *tqt_xdnd_str_to_atom("TEXT");
	    static Atom xa_compound_text = *tqt_xdnd_str_to_atom("COMPOUND_TEXT");
	    static Atom xa_targets = *tqt_xdnd_str_to_atom("TARGETS");
	    static Atom xa_multiple = *tqt_xdnd_str_to_atom("MULTIPLE");
	    static Atom xa_timestamp = *tqt_xdnd_str_to_atom("TIMESTAMP");

	    struct AtomPair { Atom target; Atom property; } *multi = 0;
	    Atom multi_type = None;
	    int multi_format = 0;
	    int nmulti = 0;
	    int imulti = -1;
	    bool multi_writeback = false;

	    if ( req->target == xa_multiple ) {
		TQByteArray multi_data;
		if (req->property == None
		    || !tqt_xclb_read_property(dpy, req->requestor, req->property,
					      false, &multi_data, 0, &multi_type, &multi_format, 0)
		    || multi_format != 32) {
		    // MULTIPLE property not formatted correctly
		    XSendEvent(dpy, req->requestor, False, NoEventMask, &event);
		    break;
		}
		nmulti = multi_data.size()/sizeof(*multi);
		multi = new AtomPair[nmulti];
		memcpy(multi,multi_data.data(),multi_data.size());
		imulti = 0;
	    }

	    for (; imulti < nmulti; ++imulti) {
		Atom target;
		Atom property;

		if ( multi ) {
		    target = multi[imulti].target;
		    property = multi[imulti].property;
		} else {
		    target = req->target;
		    property = req->property;
		    if (property == None) // obsolete client
			property = target;
		}

		Atom ret = None;
		if (target == None || property == None) {
		    ;
		} else if (target == xa_timestamp) {
		    if (d->timestamp != CurrentTime) {
			XChangeProperty(dpy, req->requestor, property, xa_timestamp, 32,
					PropModeReplace, (uchar *) &d->timestamp, 1);
			ret = property;
		    } else {

#ifdef QT_CHECK_STATE
			tqWarning("TQClipboard: invalid data timestamp");
#endif // QT_CHECK_STATE
		    }
		} else if (target == xa_targets) {
		    ret = send_targets_selection(d, req->requestor, property);
		} else if (target == XA_STRING
			   || target == xa_text
			   || target == xa_compound_text
			   || target == tqt_utf8_string) {
		    ret = send_string_selection(d, target, req->requestor, property);
		} else if (target == XA_PIXMAP
			   || target == XA_BITMAP) {
		    ret = send_pixmap_selection(d, target, req->requestor, property);
		} else {
		    ret = send_selection(d, target, req->requestor, property);
		}

		if (nmulti > 0) {
		    if (ret == None) {
			multi[imulti].property = None;
			multi_writeback = true;
		    }
		} else {
		    event.xselection.property = ret;
		    break;
		}
	    }

	    if (nmulti > 0) {
		if (multi_writeback) {
		    // according to ICCCM 2.6.2 says to put None back
		    // into the original property on the requestor window
		    XChangeProperty(dpy, req->requestor, req->property, multi_type, 32,
				    PropModeReplace, (uchar *) multi, nmulti * 2);
		}

		delete [] multi;
		event.xselection.property = req->property;
	    }

	    // send selection notify to requestor
	    XSendEvent(dpy, req->requestor, False, NoEventMask, &event);

	    TQDEBUG("TQClipboard: SelectionNotify to 0x%lx\n"
		  "    property 0x%lx (%s)",
		  req->requestor, event.xselection.property,
		  tqt_xdnd_atom_to_str(event.xselection.property));
	}
	break;
    }

    return true;
}






TQClipboardWatcher::TQClipboardWatcher( TQClipboard::Mode mode )
{
    switch ( mode ) {
    case TQClipboard::Selection:
	atom = XA_PRIMARY;
	break;

    case TQClipboard::Clipboard:
	atom = tqt_xa_clipboard;
	break;

#ifdef QT_CHECK_RANGE
    default:
	tqWarning( "TQClipboardWatcher: internal error, unknown clipboard mode" );
	break;
#endif // QT_CHECK_RANGE
    }

    setupOwner();
}

TQClipboardWatcher::~TQClipboardWatcher()
{
    if( selection_watcher == this )
        selection_watcher = 0;
    if( clipboard_watcher == this )
        clipboard_watcher = 0;
}

bool TQClipboardWatcher::empty() const
{
    Display *dpy = TQPaintDevice::x11AppDisplay();
    Window win = XGetSelectionOwner( dpy, atom );

#ifdef QT_CHECK_STATE
    if( win == requestor->winId()) {
        tqWarning( "TQClipboardWatcher::empty: internal error, app owns the selection" );
        return true;
    }
#endif // QT_CHECK_STATE

    return win == None;
}

const char* TQClipboardWatcher::format( int n ) const
{
    if ( empty() )
	return 0;

    if (! formatList.count()) {
	// get the list of targets from the current clipboard owner - we do this
	// once so that multiple calls to this function don't require multiple
	// server round trips...
	static Atom xa_targets = *tqt_xdnd_str_to_atom( "TARGETS" );

	TQByteArray ba = getDataInFormat(xa_targets);
	if (ba.size() > 0) {
	    Atom *unsorted_target = (Atom *) ba.data();
	    static Atom xa_text = *tqt_xdnd_str_to_atom( "TEXT" );
	    static Atom xa_compound_text = *tqt_xdnd_str_to_atom( "COMPOUND_TEXT" );
	    int i, size = ba.size() / sizeof(Atom);

	    // sort TARGETS to prefer some types over others.  some apps
	    // will report XA_STRING before COMPOUND_TEXT, and we want the
	    // latter, not the former (if it is present).
	    Atom* target = new Atom[size+4];
	    memset( target, 0, (size+4) * sizeof(Atom) );

	    for ( i = 0; i < size; ++i ) {
		if ( unsorted_target[i] == tqt_utf8_string )
		    target[0] = unsorted_target[i];
		else if ( unsorted_target[i] == xa_compound_text )
		    target[1] = unsorted_target[i];
		else if ( unsorted_target[i] == xa_text )
		    target[2] = unsorted_target[i];
		else if ( unsorted_target[i] == XA_STRING )
		    target[3] = unsorted_target[i];
		else
		    target[i + 4] = unsorted_target[i];
	    }

	    for (i = 0; i < size + 4; ++i) {
		if ( target[i] == 0 ) continue;

		VTQDEBUG("    format: %s", tqt_xdnd_atom_to_str(target[i]));

		if ( target[i] == XA_PIXMAP )
		    formatList.append("image/ppm");
		else if ( target[i] == XA_STRING )
		    formatList.append( "text/plain;charset=ISO-8859-1" );
		else if ( target[i] == tqt_utf8_string )
		    formatList.append( "text/plain;charset=UTF-8" );
		else if ( target[i] == xa_text ||
			  target[i] == xa_compound_text )
		    formatList.append( "text/plain" );
		else
		    formatList.append(tqt_xdnd_atom_to_str(target[i]));
	    }
	    delete []target;

	    TQDEBUG("TQClipboardWatcher::format: %d formats available",
		  int(formatList.count()));
	}
    }

    if (n >= 0 && n < (signed) formatList.count())
	return formatList[n];
    if (n == 0)
	return "text/plain";
    return 0;
}

TQByteArray TQClipboardWatcher::encodedData( const char* fmt ) const
{
    if ( !fmt || empty() )
	return TQByteArray( 0 );

    TQDEBUG("TQClipboardWatcher::encodedData: fetching format '%s'", fmt);

    Atom fmtatom = 0;

    if ( 0==tqstricmp(fmt,"text/plain;charset=iso-8859-1") ) {
	// ICCCM section 2.6.2 says STRING is latin1 text
	fmtatom = XA_STRING;
    } else if ( 0==tqstricmp(fmt,"text/plain;charset=utf-8") ) {
	// proprosed UTF8_STRING conversion type
	fmtatom = *tqt_xdnd_str_to_atom( "UTF8_STRING" );
    } else if ( 0==qstrcmp(fmt,"text/plain") ) {
	fmtatom = *tqt_xdnd_str_to_atom( "COMPOUND_TEXT" );
    } else if ( 0==qstrcmp(fmt,"image/ppm") ) {
	fmtatom = XA_PIXMAP;
	TQByteArray pmd = getDataInFormat(fmtatom);
	if ( pmd.size() == sizeof(Pixmap) ) {
	    Pixmap xpm = *((Pixmap*)pmd.data());
	    Display *dpy = TQPaintDevice::x11AppDisplay();
	    Window r;
	    int x,y;
	    uint w,h,bw,d;
	    if ( ! xpm )
		return TQByteArray( 0 );
	    XGetGeometry(dpy,xpm, &r,&x,&y,&w,&h,&bw,&d);
	    TQImageIO iio;
	    GC gc = XCreateGC( dpy, xpm, 0, 0 );
	    if ( d == 1 ) {
		TQBitmap qbm(w,h);
		XCopyArea(dpy,xpm,qbm.handle(),gc,0,0,w,h,0,0);
		iio.setFormat("PBMRAW");
		iio.setImage(qbm.convertToImage());
	    } else {
		TQPixmap qpm(w,h);
		XCopyArea(dpy,xpm,qpm.handle(),gc,0,0,w,h,0,0);
		iio.setFormat("PPMRAW");
		iio.setImage(qpm.convertToImage());
	    }
	    XFreeGC(dpy,gc);
	    TQBuffer buf;
	    buf.open(IO_WriteOnly);
	    iio.setIODevice(&buf);
	    iio.write();
	    return buf.buffer();
	} else {
	    fmtatom = *tqt_xdnd_str_to_atom(fmt);
	}
    } else {
	fmtatom = *tqt_xdnd_str_to_atom(fmt);
    }
    return getDataInFormat(fmtatom);
}

TQByteArray TQClipboardWatcher::getDataInFormat(Atom fmtatom) const
{
    TQByteArray buf;

    Display *dpy = TQPaintDevice::x11AppDisplay();
    Window   win = requestor->winId();

    TQDEBUG("TQClipboardWatcher::getDataInFormat: selection '%s' format '%s'",
	  tqt_xdnd_atom_to_str(atom), tqt_xdnd_atom_to_str(fmtatom));

    XSelectInput(dpy, win, NoEventMask); // don't listen for any events

    XDeleteProperty(dpy, win, tqt_selection_property);
    XConvertSelection(dpy, atom, fmtatom, tqt_selection_property, win, tqt_x_time);
    XSync(dpy, false);

    VTQDEBUG("TQClipboardWatcher::getDataInFormat: waiting for SelectionNotify event");

    XEvent xevent;
    if ( !tqt_xclb_wait_for_event(dpy,win,SelectionNotify,&xevent,clipboard_timeout) ||
	 xevent.xselection.property == None ) {
	TQDEBUG("TQClipboardWatcher::getDataInFormat: format not available");
	return buf;
    }

    VTQDEBUG("TQClipboardWatcher::getDataInFormat: fetching data...");

    Atom   type;
    XSelectInput(dpy, win, PropertyChangeMask);

    if ( tqt_xclb_read_property(dpy,win,tqt_selection_property,true,
			       &buf,0,&type,0,false) ) {
	if ( type == tqt_x_incr ) {
	    int nbytes = buf.size() >= 4 ? *((int*)buf.data()) : 0;
	    buf = tqt_xclb_read_incremental_property( dpy, win,
						     tqt_selection_property,
						     nbytes, false );
	}
    }

    XSelectInput(dpy, win, NoEventMask);

    TQDEBUG("TQClipboardWatcher::getDataInFormat: %d bytes received", buf.size());

    return buf;
}


TQMimeSource* TQClipboard::data( Mode mode ) const
{
    TQClipboardData *d;
    switch ( mode ) {
    case Selection: d = selectionData(); break;
    case Clipboard: d = clipboardData(); break;

    default:
#ifdef QT_CHECK_RANGE
	tqWarning( "TQClipboard::data: invalid mode '%d'", mode );
#endif // QT_CHECK_RANGE
	return 0;
    }

    if ( ! d->source() && ! timer_event_clear ) {
	if ( mode == Selection ) {
	    if ( ! selection_watcher )
		selection_watcher = new TQClipboardWatcher( mode );
	    d->setSource( selection_watcher );
	} else {
	    if ( ! clipboard_watcher )
		clipboard_watcher = new TQClipboardWatcher( mode );
	    d->setSource( clipboard_watcher );
	}

	if (! timer_id) {
	    // start a zero timer - we will clear cached data when the timer
	    // times out, which will be the next time we hit the event loop...
	    // that way, the data is cached long enough for calls within a single
	    // loop/function, but the data doesn't linger around in case the selection
	    // changes
	    TQClipboard *that = ((TQClipboard *) this);
	    timer_id = that->startTimer(0);
	}
    }

    return d->source();
}


void TQClipboard::setData( TQMimeSource* src, Mode mode )
{
    Atom atom, sentinel_atom;
    TQClipboardData *d;
    switch ( mode ) {
    case Selection:
	atom = XA_PRIMARY;
	sentinel_atom = tqt_selection_sentinel;
	d = selectionData();
	break;

    case Clipboard:
	atom = tqt_xa_clipboard;
	sentinel_atom = tqt_clipboard_sentinel;
	d = clipboardData();
	break;

    default:
#ifdef QT_CHECK_RANGE
	tqWarning( "TQClipboard::data: invalid mode '%d'", mode );
#endif // QT_CHECK_RANGE
	return;
    }

    Display *dpy = TQPaintDevice::x11AppDisplay();
    Window newOwner;

    if (! src) { // no data, clear clipboard contents
	newOwner = None;
	d->clear();
    } else {
	setupOwner();

	newOwner = owner->winId();

	d->setSource(src);
	d->timestamp = tqt_x_time;
    }

    Window prevOwner = XGetSelectionOwner( dpy, atom );
    // use tqt_x_time, since d->timestamp == CurrentTime when clearing
    XSetSelectionOwner(dpy, atom, newOwner, tqt_x_time);

    if ( mode == Selection )
	emit selectionChanged();
    else
	emit dataChanged();

    if (XGetSelectionOwner(dpy, atom) != newOwner) {
#ifdef QT_CHECK_STATE
	tqWarning("TQClipboard::setData: Cannot set X11 selection owner for %s",
		 tqt_xdnd_atom_to_str(atom));
#endif // QT_CHECK_STATE

	d->clear();
	return;
    }

    // Signal to other TQt processes that the selection has changed
    Window owners[2];
    owners[0] = newOwner;
    owners[1] = prevOwner;
    XChangeProperty( dpy, TQApplication::desktop()->screen(0)->winId(),
		     sentinel_atom, XA_WINDOW, 32, PropModeReplace,
		     (unsigned char*)&owners, 2 );
}


/*
  Called by the main event loop in qapplication_x11.cpp when the
  _QT_SELECTION_SENTINEL property has been changed (i.e. when some TQt
  process has performed TQClipboard::setData(). If it returns true, the
  TQClipBoard dataChanged() signal should be emitted.
*/

bool tqt_check_selection_sentinel()
{
    bool doIt = true;
    if ( owner ) {
	/*
	  Since the X selection mechanism cannot give any signal when
	  the selection has changed, we emulate it (for TQt processes) here.
	  The notification should be ignored in case of either
	  a) This is the process that did setData (because setData()
	  then has already emitted dataChanged())
	  b) This is the process that owned the selection when dataChanged()
	  was called (because we have then received a SelectionClear event,
	  and have already emitted dataChanged() as a result of that)
	*/

	Window* owners;
	Atom actualType;
	int actualFormat;
	ulong nitems;
	ulong bytesLeft;

	if (XGetWindowProperty(TQPaintDevice::x11AppDisplay(),
			       TQApplication::desktop()->screen(0)->winId(),
			       tqt_selection_sentinel, 0, 2, False, XA_WINDOW,
			       &actualType, &actualFormat, &nitems,
			       &bytesLeft, (unsigned char**)&owners) == Success) {
	    if ( actualType == XA_WINDOW && actualFormat == 32 && nitems == 2 ) {
		Window win = owner->winId();
		if ( owners[0] == win || owners[1] == win )
		    doIt = false;
	    }

	    XFree( owners );
	}
    }

    if (doIt) {
	if ( waiting_for_data ) {
	    pending_selection_changed = true;
	    if ( ! pending_timer_id )
		pending_timer_id = TQApplication::clipboard()->startTimer( 0 );
	    doIt = false;
	} else {
	    selectionData()->clear();
	}
    }

    return doIt;
}


bool tqt_check_clipboard_sentinel()
{
    bool doIt = true;
    if (owner) {
	Window *owners;
	Atom actualType;
	int actualFormat;
	unsigned long nitems, bytesLeft;

	if (XGetWindowProperty(TQPaintDevice::x11AppDisplay(),
			       TQApplication::desktop()->screen(0)->winId(),
			       tqt_clipboard_sentinel, 0, 2, False, XA_WINDOW,
			       &actualType, &actualFormat, &nitems, &bytesLeft,
			       (unsigned char **) &owners) == Success) {
	    if (actualType == XA_WINDOW && actualFormat == 32 && nitems == 2) {
		Window win = owner->winId();
		if (owners[0] == win || owners[1] == win)
		    doIt = false;
	    }

	    XFree(owners);
	}
    }

    if (doIt) {
	if ( waiting_for_data ) {
	    pending_clipboard_changed = true;
	    if ( ! pending_timer_id )
		pending_timer_id = TQApplication::clipboard()->startTimer( 0 );
	    doIt = false;
	} else {
	    clipboardData()->clear();
	}
    }

    return doIt;
}

#endif // TQT_NO_CLIPBOARD
