/****************************************************************************
**
** Implementation of Drag and Drop support
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

// POSIX Large File Support redefines open -> open64
#if defined(open)
# undef open
#endif

#ifndef TQT_NO_MIME

#include "ntqdragobject.h"
#include "ntqtextcodec.h"
#include "ntqapplication.h"
#include "ntqpoint.h"
#include "ntqwidget.h"
#include "ntqbuffer.h"
#include "ntqgif.h"
#include "ntqregexp.h"
#include "ntqdir.h"
#include <ctype.h>

// both a struct for storing stuff in and a wrapper to avoid polluting
// the name space

class TQDragObjectData
{
public:
    TQDragObjectData(): hot(0,0) {}
    TQPixmap pixmap;
    TQPoint hot;
    // store default cursors
    TQPixmap *pm_cursor;
};

static TQWidget* last_target;

/*!
    After the drag completes, this function will return the TQWidget
    which received the drop, or 0 if the data was dropped on another
    application.

    This can be useful for detecting the case where drag and drop is
    to and from the same widget.
*/
TQWidget * TQDragObject::target()
{
    return last_target;
}

/*!
    \internal
    Sets the target.
*/
void TQDragObject::setTarget(TQWidget* t)
{
    last_target = t;
}

class TQStoredDragData
{
public:
    TQStoredDragData() {}
    const char* fmt;
    TQByteArray enc;
};


// These pixmaps approximate the images in the Windows User Interface Guidelines.

// XPM

static const char * const move_xpm[] = {
"11 20 3 1",
".	c None",
#if defined(TQ_WS_WIN)
"a	c #000000",
"X	c #FFFFFF", // Windows cursor is traditionally white
#else
"a	c #FFFFFF",
"X	c #000000", // X11 cursor is traditionally black
#endif
"aa.........",
"aXa........",
"aXXa.......",
"aXXXa......",
"aXXXXa.....",
"aXXXXXa....",
"aXXXXXXa...",
"aXXXXXXXa..",
"aXXXXXXXXa.",
"aXXXXXXXXXa",
"aXXXXXXaaaa",
"aXXXaXXa...",
"aXXaaXXa...",
"aXa..aXXa..",
"aa...aXXa..",
"a.....aXXa.",
"......aXXa.",
".......aXXa",
".......aXXa",
"........aa."};

/* XPM */
static const char * const copy_xpm[] = {
"24 30 3 1",
".	c None",
"a	c #000000",
"X	c #FFFFFF",
#if defined(TQ_WS_WIN) // Windows cursor is traditionally white
"aa......................",
"aXa.....................",
"aXXa....................",
"aXXXa...................",
"aXXXXa..................",
"aXXXXXa.................",
"aXXXXXXa................",
"aXXXXXXXa...............",
"aXXXXXXXXa..............",
"aXXXXXXXXXa.............",
"aXXXXXXaaaa.............",
"aXXXaXXa................",
"aXXaaXXa................",
"aXa..aXXa...............",
"aa...aXXa...............",
"a.....aXXa..............",
"......aXXa..............",
".......aXXa.............",
".......aXXa.............",
"........aa...aaaaaaaaaaa",
#else
"XX......................",
"XaX.....................",
"XaaX....................",
"XaaaX...................",
"XaaaaX..................",
"XaaaaaX.................",
"XaaaaaaX................",
"XaaaaaaaX...............",
"XaaaaaaaaX..............",
"XaaaaaaaaaX.............",
"XaaaaaaXXXX.............",
"XaaaXaaX................",
"XaaXXaaX................",
"XaX..XaaX...............",
"XX...XaaX...............",
"X.....XaaX..............",
"......XaaX..............",
".......XaaX.............",
".......XaaX.............",
"........XX...aaaaaaaaaaa",
#endif
".............aXXXXXXXXXa",
".............aXXXXXXXXXa",
".............aXXXXaXXXXa",
".............aXXXXaXXXXa",
".............aXXaaaaaXXa",
".............aXXXXaXXXXa",
".............aXXXXaXXXXa",
".............aXXXXXXXXXa",
".............aXXXXXXXXXa",
".............aaaaaaaaaaa"};

/* XPM */
static const char * const link_xpm[] = {
"24 30 3 1",
".	c None",
"a	c #000000",
"X	c #FFFFFF",
#if defined(TQ_WS_WIN) // Windows cursor is traditionally white
"aa......................",
"aXa.....................",
"aXXa....................",
"aXXXa...................",
"aXXXXa..................",
"aXXXXXa.................",
"aXXXXXXa................",
"aXXXXXXXa...............",
"aXXXXXXXXa..............",
"aXXXXXXXXXa.............",
"aXXXXXXaaaa.............",
"aXXXaXXa................",
"aXXaaXXa................",
"aXa..aXXa...............",
"aa...aXXa...............",
"a.....aXXa..............",
"......aXXa..............",
".......aXXa.............",
".......aXXa.............",
"........aa...aaaaaaaaaaa",
#else
"XX......................",
"XaX.....................",
"XaaX....................",
"XaaaX...................",
"XaaaaX..................",
"XaaaaaX.................",
"XaaaaaaX................",
"XaaaaaaaX...............",
"XaaaaaaaaX..............",
"XaaaaaaaaaX.............",
"XaaaaaaXXXX.............",
"XaaaXaaX................",
"XaaXXaaX................",
"XaX..XaaX...............",
"XX...XaaX...............",
"X.....XaaX..............",
"......XaaX..............",
".......XaaX.............",
".......XaaX.............",
"........XX...aaaaaaaaaaa",
#endif
".............aXXXXXXXXXa",
".............aXXXaaaaXXa",
".............aXXXXaaaXXa",
".............aXXXaaaaXXa",
".............aXXaaaXaXXa",
".............aXXaaXXXXXa",
".............aXXaXXXXXXa",
".............aXXXaXXXXXa",
".............aXXXXXXXXXa",
".............aaaaaaaaaaa"};

#ifndef TQT_NO_DRAGANDDROP

// the universe's only drag manager
TQDragManager * tqt_dnd_manager = 0;


TQDragManager::TQDragManager()
    : TQObject( tqApp, "global drag manager" )
{
    n_cursor = 3;
    pm_cursor = new TQPixmap[n_cursor];
    pm_cursor[0] = TQPixmap((const char **)move_xpm);
    pm_cursor[1] = TQPixmap((const char **)copy_xpm);
    pm_cursor[2] = TQPixmap((const char **)link_xpm);
#if defined(TQ_WS_X11)
    createCursors(); // Xcursors cache can hold only 8 bitmaps (4 cursors)
#endif
    object = 0;
    dragSource = 0;
    dropWidget = 0;
    if ( !tqt_dnd_manager )
	tqt_dnd_manager = this;
    beingCancelled = false;
    restoreCursor = false;
    willDrop = false;
}


TQDragManager::~TQDragManager()
{
#ifndef TQT_NO_CURSOR
    if ( restoreCursor )
	TQApplication::restoreOverrideCursor();
#endif
    tqt_dnd_manager = 0;
    delete [] pm_cursor;
}

#endif


/*!
    Constructs a drag object called \a name, which is a child of \a
    dragSource.

    Note that the drag object will be deleted when \a dragSource is
    deleted.
*/

TQDragObject::TQDragObject( TQWidget * dragSource, const char * name )
    : TQObject( dragSource, name )
{
    d = new TQDragObjectData();
    d->pm_cursor = 0;
#ifndef TQT_NO_DRAGANDDROP
    if ( !tqt_dnd_manager && tqApp )
	(void)new TQDragManager();
#endif
}


/*!
    Destroys the drag object, canceling any drag and drop operation in
    which it is involved, and frees up the storage used.
*/

TQDragObject::~TQDragObject()
{
#ifndef TQT_NO_DRAGANDDROP
    if ( tqt_dnd_manager && tqt_dnd_manager->object == this )
	tqt_dnd_manager->cancel( false );
    if ( d->pm_cursor ) {
	for ( int i = 0; i < tqt_dnd_manager->n_cursor; i++ )
	    tqt_dnd_manager->pm_cursor[i] = d->pm_cursor[i];
	delete [] d->pm_cursor;
    }
#endif
    delete d;
}

#ifndef TQT_NO_DRAGANDDROP
/*!
    Set the pixmap \a pm to display while dragging the object. The
    platform-specific implementation will use this where it can - so
    provide a small masked pixmap, and do not assume that the user
    will actually see it. For example, cursors on Windows 95 are of
    limited size.

    The \a hotspot is the point on (or off) the pixmap that should be
    under the cursor as it is dragged. It is relative to the top-left
    pixel of the pixmap.

    \warning We have seen problems with drag cursors on different
    graphics hardware and driver software on Windows. Setting the
    graphics acceleration in the display settings down one tick solved
    the problems in all cases.
*/
void TQDragObject::setPixmap(TQPixmap pm, const TQPoint& hotspot)
{
    d->pixmap = pm;
    d->hot = hotspot;
    if ( tqt_dnd_manager && tqt_dnd_manager->object == this )
	tqt_dnd_manager->updatePixmap();
}

/*!
    \overload
    Uses a hotspot that positions the pixmap below and to the right of
    the mouse pointer. This allows the user to clearly see the point
    on the window which they are dragging the data onto.
*/
void TQDragObject::setPixmap(TQPixmap pm)
{
    setPixmap(pm,TQPoint(-10, -10));
}

/*!
    Returns the currently set pixmap (which \link TQPixmap::isNull()
    isNull()\endlink if none is set).
*/
TQPixmap TQDragObject::pixmap() const
{
    return d->pixmap;
}

/*!
    Returns the currently set pixmap hotspot.
*/
TQPoint TQDragObject::pixmapHotSpot() const
{
    return d->hot;
}

#if 0

// ## reevaluate for TQt 4
/*!
    Set the \a cursor used when dragging in mode \a m.
    Note: X11 only allow bitmaps for cursors.
*/
void TQDragObject::setCursor( DragMode m, const TQPixmap &cursor )
{
    if ( d->pm_cursor == 0 ) {
	// safe default cursors
	d->pm_cursor = new TQPixmap[tqt_dnd_manager->n_cursor];
	for ( int i = 0; i < tqt_dnd_manager->n_cursor; i++ )
	    d->pm_cursor[i] = tqt_dnd_manager->pm_cursor[i];
    }

    int index;
    switch ( m ) {
    case DragCopy:
	index = 1;
	break;
    case DragLink:
	index = 2;
	break;
    default:
	index = 0;
	break;
    }

    // override default cursor
    for ( index = 0; index < tqt_dnd_manager->n_cursor; index++ )
	tqt_dnd_manager->pm_cursor[index] = cursor;
}

/*!
    Returns the cursor used when dragging in mode \a m, or null if no cursor
    has been set for that mode.
*/
TQPixmap *TQDragObject::cursor( DragMode m ) const
{
    if ( !d->pm_cursor )
	return 0;

    int index;
    switch ( m ) {
    case DragCopy:
	index = 1;
	break;
    case DragLink:
	index = 2;
	break;
    default:
	index = 0;
	break;
    }

    return tqt_dnd_manager->pm_cursor+index;
}

#endif // 0

/*!
    Starts a drag operation using the contents of this object, using
    DragDefault mode.

    The function returns true if the caller should delete the original
    copy of the dragged data (but see target()); otherwise returns
    false.

    If the drag contains \e references to information (e.g. file names
    in a TQUriDrag are references) then the return value should always
    be ignored, as the target is expected to manipulate the
    referred-to content directly. On X11 the return value should
    always be correct anyway, but on Windows this is not necessarily
    the case (e.g. the file manager starts a background process to
    move files, so the source \e{must not} delete the files!)
*/
bool TQDragObject::drag()
{
    return drag( DragDefault );
}


/*!
    Starts a drag operation using the contents of this object, using
    \c DragMove mode. Be sure to read the constraints described in
    drag().

    \sa drag() dragCopy() dragLink()
*/
bool TQDragObject::dragMove()
{
    return drag( DragMove );
}


/*!
    Starts a drag operation using the contents of this object, using
    \c DragCopy mode. Be sure to read the constraints described in
    drag().

    \sa drag() dragMove() dragLink()
*/
void TQDragObject::dragCopy()
{
    (void)drag( DragCopy );
}

/*!
    Starts a drag operation using the contents of this object, using
    \c DragLink mode. Be sure to read the constraints described in
    drag().

    \sa drag() dragCopy() dragMove()
*/
void TQDragObject::dragLink()
{
    (void)drag( DragLink );
}


/*!
    \enum TQDragObject::DragMode

    This enum describes the possible drag modes.

    \value DragDefault  The mode is determined heuristically.
    \value DragCopy  The data is copied, never moved.
    \value DragMove  The data is moved, if dragged at all.
    \value DragLink  The data is linked, if dragged at all.
    \value DragCopyOrMove  The user chooses the mode by using a
			   control key to switch from the default.
*/


/*!
    \overload
    Starts a drag operation using the contents of this object.

    At this point, the object becomes owned by TQt, not the
    application. You should not delete the drag object or anything it
    references. The actual transfer of data to the target application
    will be done during future event processing - after that time the
    drag object will be deleted.

    Returns true if the dragged data was dragged as a \e move,
    indicating that the caller should remove the original source of
    the data (the drag object must continue to have a copy); otherwise
    returns false.

    The \a mode specifies the drag mode (see
    \l{TQDragObject::DragMode}.) Normally one of the simpler drag(),
    dragMove(), or dragCopy() functions would be used instead.
*/
bool TQDragObject::drag( DragMode mode )
{
    if ( tqt_dnd_manager )
	return tqt_dnd_manager->drag( this, mode );
    else
	return false;
}

#endif


/*!
    Returns a pointer to the drag source where this object originated.
*/

TQWidget * TQDragObject::source()
{
    if ( parent() && parent()->isWidgetType() )
	return (TQWidget *)parent();
    else
	return 0;
}


/*!
    \class TQDragObject ntqdragobject.h

    \brief The TQDragObject class encapsulates MIME-based data
    transfer.

    \ingroup draganddrop

    TQDragObject is the base class for all data that needs to be
    transferred between and within applications, both for drag and
    drop and for the \link ntqclipboard.html clipboard\endlink.

    See the \link dnd.html Drag-and-drop documentation\endlink for an
    overview of how to provide drag and drop in your application.

    See the TQClipboard documentation for an overview of how to provide
    cut-and-paste in your application.

    The drag() function is used to start a drag operation. You can
    specify the \l DragMode in the call or use one of the convenience
    functions dragCopy(), dragMove() or dragLink(). The drag source
    where the data originated is retrieved with source(). If the data
    was dropped on a widget within the application, target() will
    return a pointer to that widget. Specify the pixmap to display
    during the drag with setPixmap().
*/

static
void stripws(TQCString& s)
{
    int f;
    while ( (f=s.find(' ')) >= 0 )
	s.remove(f,1);
}

static
const char * staticCharset(int i)
{
    static TQCString localcharset;

    switch ( i ) {
      case 0:
	return "UTF-8";
      case 1:
	return "ISO-10646-UCS-2";
      case 2:
	return ""; // in the 3rd place - some Xdnd targets might only look at 3
      case 3:
	if ( localcharset.isNull() ) {
	    TQTextCodec *localCodec = TQTextCodec::codecForLocale();
	    if ( localCodec ) {
		localcharset = localCodec->name();
		localcharset = localcharset.lower();
		stripws(localcharset);
	    } else {
		localcharset = "";
	    }
	}
	return localcharset;
    }
    return 0;
}


class TQTextDragPrivate {
public:
    TQTextDragPrivate();

    enum { nfmt=4 };

    TQString txt;
    TQCString fmt[nfmt];
    TQCString subtype;

    void setSubType(const TQCString & st)
    {
	subtype = st.lower();
	for ( int i=0; i<nfmt; i++ ) {
	    fmt[i] = "text/";
	    fmt[i].append(subtype);
	    TQCString cs = staticCharset(i);
	    if ( !cs.isEmpty() ) {
		fmt[i].append(";charset=");
		fmt[i].append(cs);
	    }
	}
    }
};

inline TQTextDragPrivate::TQTextDragPrivate()
{
    setSubType("plain");
}

/*!
    Sets the MIME subtype of the text being dragged to \a st. The
    default subtype is "plain", so the default MIME type of the text
    is "text/plain". You might use this to declare that the text is
    "text/html" by calling setSubtype("html").
*/
void TQTextDrag::setSubtype( const TQCString & st)
{
    d->setSubType(st);
}

/*!
    \class TQTextDrag ntqdragobject.h

    \brief The TQTextDrag class is a drag and drop object for
    transferring plain and Unicode text.

    \ingroup draganddrop

    Plain text is passed in a TQString which may contain multiple lines
    (i.e. may contain newline characters). The drag target will receive
    the newlines according to the runtime environment, e.g. LF on Unix,
    and CRLF on Windows.

    TQt provides no built-in mechanism for delivering only a single-line.

    For more information about drag and drop, see the TQDragObject class
    and the \link dnd.html drag and drop documentation\endlink.
*/


/*!
    Constructs a text drag object and sets its data to \a text. \a
    dragSource must be the drag source; \a name is the object name.
*/

TQTextDrag::TQTextDrag( const TQString &text,
		      TQWidget * dragSource, const char * name )
    : TQDragObject( dragSource, name )
{
    d = new TQTextDragPrivate;
    setText( text );
}


/*!
    Constructs a default text drag object. \a dragSource must be the
    drag source; \a name is the object name.
*/

TQTextDrag::TQTextDrag( TQWidget * dragSource, const char * name )
    : TQDragObject( dragSource, name )
{
    d = new TQTextDragPrivate;
}


/*!
    Destroys the text drag object and frees up all allocated
    resources.
*/
TQTextDrag::~TQTextDrag()
{
    delete d;
}


/*!
    Sets the text to be dragged to \a text. You will need to call this
    if you did not pass the text during construction.
*/
void TQTextDrag::setText( const TQString &text )
{
    d->txt = text;
}


/*!
    \reimp
*/
const char * TQTextDrag::format(int i) const
{
    if ( i >= d->nfmt )
	return 0;
    return d->fmt[i];
}

TQTextCodec* tqt_findcharset(const TQCString& mimetype)
{
    int i=mimetype.find("charset=");
    if ( i >= 0 ) {
	TQCString cs = mimetype.mid(i+8);
	stripws(cs);
	i = cs.find(';');
	if ( i >= 0 )
	    cs = cs.left(i);
	// win98 often has charset=utf16, and we need to get the correct codec for
	// it to be able to get Unicode text drops.
	if ( cs == "utf16" )
	    cs = "ISO-10646-UCS-2";
	// May return 0 if unknown charset
	return TQTextCodec::codecForName(cs);
    }
    // no charset=, use locale
    return TQTextCodec::codecForLocale();
}

static TQTextCodec *codecForHTML(const TQCString &ba)
{
    // determine charset
    int mib = 0;
    int pos;
    TQTextCodec *c = 0;

    if (ba.size() > 1 && (((uchar)ba[0] == 0xfe && (uchar)ba[1] == 0xff)
			  || ((uchar)ba[0] == 0xff && (uchar)ba[1] == 0xfe))) {
	mib = 1000; // utf16
    } else if (ba.size() > 2
	       && (uchar)ba[0] == 0xef
	       && (uchar)ba[1] == 0xbb
	       && (uchar)ba[2] == 0xbf) {
	mib = 106; // utf-8
    } else {
	pos = 0;
	while ((pos = ba.find("<meta http-equiv=", pos, false)) != -1) {
	    int end = ba.find('>', pos+1);
	    if (end == -1)
		break;
	    pos = ba.find("charset=", pos, false) + (int)strlen("charset=");
	    if (pos != -1 && pos < end) {
		int pos2 = ba.find('\"', pos+1);
		TQCString cs = ba.mid(pos, pos2-pos);
		c = TQTextCodec::codecForName(cs);
		if (c)
		    return c;
	    }
	    pos = end;
	}
    }
    if (mib)
	c = TQTextCodec::codecForMib(mib);

    return c;
}

static
TQTextCodec* findcodec(const TQMimeSource* e)
{
    TQTextCodec* r = 0;
    const char* f;
    int i;
    for ( i=0; (f=e->format(i)); i++ ) {
	bool html = !tqstrnicmp(f, "text/html", 9);
	if (html)
	    r = codecForHTML(TQCString(e->encodedData(f)));
	if (!r)
	    r = tqt_findcharset(TQCString(f).lower());
	if (r)
	    return r;
    }
    return 0;
}



/*!
    \reimp
*/
TQByteArray TQTextDrag::encodedData(const char* mime) const
{
    TQCString r;
    if ( 0==tqstrnicmp(mime,"text/",5) ) {
	TQCString m(mime);
	m = m.lower();
	TQTextCodec *codec = tqt_findcharset(m);
	if ( !codec )
	    return r;
	TQString text( d->txt );
#if defined(TQ_WS_WIN)
	int index = text.find( TQString::fromLatin1("\r\n"), 0 );
	while ( index != -1 ) {
	    text.replace( index, 2, TQChar('\n') );
	    index = text.find( "\r\n", index );
	}
#endif
	r = codec->fromUnicode(text);
	if (!codec || codec->mibEnum() != 1000) {
	    // Don't include NUL in size (TQCString::resize() adds NUL)
#if defined(TQ_WS_WIN)
	    // This is needed to ensure the \0 isn't lost on Windows 95
	    if ( qWinVersion() & TQt::WV_DOS_based )
		((TQByteArray&)r).resize(r.length()+1);
	    else
#endif
		((TQByteArray&)r).resize(r.length());
	}
    }
    return r;
}

/*!
    Returns true if the information in \a e can be decoded into a
    TQString; otherwise returns false.

    \sa decode()
*/
bool TQTextDrag::canDecode( const TQMimeSource* e )
{
    const char* f;
    for (int i=0; (f=e->format(i)); i++) {
	if ( 0==tqstrnicmp(f,"text/",5) ) {
	    return findcodec(e) != 0;
	}
    }
    return 0;
}

/*!
    \overload

    Attempts to decode the dropped information in \a e into \a str.
    Returns true if successful; otherwise returns false. If \a subtype
    is null, any text subtype is accepted; otherwise only the
    specified \a subtype is accepted.

    \sa canDecode()
*/
bool TQTextDrag::decode( const TQMimeSource* e, TQString& str, TQCString& subtype )
{
    if(!e)
	return false;
        
    // when subtype is not specified, try text/plain first, otherwise this may read
    // things like text/x-moz-url even though better targets are available
    if( subtype.isNull()) {
        TQCString subtmp = "plain";
        if( decode( e, str, subtmp )) {
            subtype = subtmp;
            return true;
        }
    }

    if ( e->cacheType == TQMimeSource::Text ) {
	str = *e->cache.txt.str;
	subtype = *e->cache.txt.subtype;
	return true;
    }

    const char* mime;
    for (int i=0; (mime = e->format(i)); i++) {
	if ( 0==tqstrnicmp(mime,"text/",5) ) {
	    TQCString m(mime);
	    m = m.lower();
	    int semi = m.find(';');
	    if ( semi < 0 )
		semi = m.length();
	    TQCString foundst = m.mid(5,semi-5);
	    if ( subtype.isNull() || foundst == subtype ) {
		bool html = !tqstrnicmp(mime, "text/html", 9);
		TQTextCodec* codec = 0;
                if (html) {
                    TQByteArray bytes = e->encodedData(mime);
		    // search for the charset tag in the HTML
		    codec = codecForHTML(TQCString(bytes.data(), bytes.size()));
                }
		if (!codec)
		    codec = tqt_findcharset(m);
		if ( codec ) {
		    TQByteArray payload;

		    payload = e->encodedData(mime);
		    if ( payload.size() ) {
			int l;
			if ( codec->mibEnum() != 1000) {
			    // length is at NUL or payload.size()
			    l = 0;
			    while ( l < (int)payload.size() && payload[l] )
				l++;
			} else {
			    l = payload.size();
			}

			str = codec->toUnicode(payload,l);

			if ( subtype.isNull() )
			    subtype = foundst;

			TQMimeSource *m = (TQMimeSource*)e;
			m->clearCache();
			m->cacheType = TQMimeSource::Text;
			m->cache.txt.str = new TQString( str );
			m->cache.txt.subtype = new TQCString( subtype );

			return true;
		    }
		}
	    }
	}
    }
    return false;
}

/*!
    Attempts to decode the dropped information in \a e into \a str.
    Returns true if successful; otherwise returns false.

    \sa canDecode()
*/
bool TQTextDrag::decode( const TQMimeSource* e, TQString& str )
{
    TQCString st;
    return decode(e,str,st);
}


/*
  TQImageDrag could use an internal MIME type for communicating TQPixmaps
  and TQImages rather than always converting to raw data. This is available
  for that purpose and others. It is not currently used.
*/
class TQImageDragData
{
public:
};


/*!
    \class TQImageDrag ntqdragobject.h

    \brief The TQImageDrag class provides a drag and drop object for
    transferring images.

    \ingroup draganddrop

    Images are offered to the receiving application in multiple
    formats, determined by TQt's \link TQImage::outputFormats() output
    formats\endlink.

    For more information about drag and drop, see the TQDragObject
    class and the \link dnd.html drag and drop documentation\endlink.
*/

/*!
    Constructs an image drag object and sets its data to \a image. \a
    dragSource must be the drag source; \a name is the object name.
*/

TQImageDrag::TQImageDrag( TQImage image,
			TQWidget * dragSource, const char * name )
    : TQDragObject( dragSource, name ),
	d(0)
{
    setImage( image );
}

/*!
    Constructs a default image drag object. \a dragSource must be the
    drag source; \a name is the object name.
*/

TQImageDrag::TQImageDrag( TQWidget * dragSource, const char * name )
    : TQDragObject( dragSource, name ),
	d(0)
{
}


/*!
    Destroys the image drag object and frees up all allocated
    resources.
*/

TQImageDrag::~TQImageDrag()
{
    // nothing
}


/*!
    Sets the image to be dragged to \a image. You will need to call
    this if you did not pass the image during construction.
*/
void TQImageDrag::setImage( TQImage image )
{
    img = image; // ### detach?
    ofmts = TQImage::outputFormats();
    ofmts.remove("PBM"); // remove non-raw PPM
    if ( image.depth()!=32 ) {
	// BMP better than PPM for paletted images
	if ( ofmts.remove("BMP") ) // move to front
	    ofmts.insert(0,"BMP");
    }
    // PNG is best of all
    if ( ofmts.remove("PNG") ) // move to front
	ofmts.insert(0,"PNG");

    if(cacheType == TQMimeSource::NoCache) { //cache it
	cacheType = TQMimeSource::Graphics;
	cache.gfx.img = new TQImage( img );
	cache.gfx.pix = 0;
    }
}

/*!
    \reimp
*/
const char * TQImageDrag::format(int i) const
{
    if ( i < (int)ofmts.count() ) {
	static TQCString str;
	str.sprintf("image/%s",(((TQImageDrag*)this)->ofmts).at(i));
	str = str.lower();
	if ( str == "image/pbmraw" )
	    str = "image/ppm";
	return str;
    } else {
	return 0;
    }
}

/*!
    \reimp
*/
TQByteArray TQImageDrag::encodedData(const char* fmt) const
{
    if ( tqstrnicmp( fmt, "image/", 6 )==0 ) {
	TQCString f = fmt+6;
	TQByteArray data;
	TQBuffer w( data );
	w.open( IO_WriteOnly );
	TQImageIO io( &w, f.upper() );
	io.setImage( img );
	if  ( !io.write() )
	    return TQByteArray();
	w.close();
	return data;
    } else {
	return TQByteArray();
    }
}

/*!
    Returns true if the information in mime source \a e can be decoded
    into an image; otherwise returns false.

    \sa decode()
*/
bool TQImageDrag::canDecode( const TQMimeSource* e ) {
    TQStrList fileFormats = TQImageIO::inputFormats();

    fileFormats.first();
    while ( fileFormats.current()) {
	TQCString format = fileFormats.current();
	TQCString type = "image/" + format.lower();
	if ( e->provides(type.data()))
	    return true;
	fileFormats.next();
    }

    return false;
}

/*!
    Attempts to decode the dropped information in mime source \a e
    into \a img. Returns true if successful; otherwise returns false.

    \sa canDecode()
*/
bool TQImageDrag::decode( const TQMimeSource* e, TQImage& img )
{
    if ( !e )
	return false;
    if ( e->cacheType == TQMimeSource::Graphics ) {
	img = *e->cache.gfx.img;
	return true;
    }

    TQByteArray payload;
    TQStrList fileFormats = TQImageIO::inputFormats();
    // PNG is best of all
    if ( fileFormats.remove("PNG") ) // move to front
	fileFormats.insert(0,"PNG");
    fileFormats.first();
    while ( fileFormats.current() ) {
	TQCString format = fileFormats.current();
	fileFormats.next();

       	TQCString type = "image/" + format.lower();
	if ( ! e->provides( type.data() ) ) continue;
	payload = e->encodedData( type.data() );
	if ( !payload.isEmpty() )
	    break;
    }

    if ( payload.isEmpty() )
	return false;

    img.loadFromData(payload);
    if ( img.isNull() )
	return false;
    TQMimeSource *m = (TQMimeSource*)e;
    m->clearCache();
    m->cacheType = TQMimeSource::Graphics;
    m->cache.gfx.img = new TQImage( img );
    m->cache.gfx.pix = 0;
    return true;
}

/*!
    \overload

    Attempts to decode the dropped information in mime source \a e
    into pixmap \a pm. Returns true if successful; otherwise returns
    false.

    This is a convenience function that converts to a TQPixmap via a
    TQImage.

    \sa canDecode()
*/
bool TQImageDrag::decode( const TQMimeSource* e, TQPixmap& pm )
{
    if ( !e )
	return false;

    if ( e->cacheType == TQMimeSource::Graphics && e->cache.gfx.pix) {
	pm = *e->cache.gfx.pix;
	return true;
    }

    TQImage img;
    // We avoid dither, since the image probably came from this display
    if ( decode( e, img ) ) {
	if ( !pm.convertFromImage( img, AvoidDither ) )
	    return false;
	// decode initialized the cache for us

	TQMimeSource *m = (TQMimeSource*)e;
	m->cache.gfx.pix = new TQPixmap( pm );
	return true;
    }
    return false;
}




/*!
    \class TQStoredDrag ntqdragobject.h
    \brief The TQStoredDrag class provides a simple stored-value drag object for arbitrary MIME data.

    \ingroup draganddrop

    When a block of data has only one representation, you can use a
    TQStoredDrag to hold it.

    For more information about drag and drop, see the TQDragObject
    class and the \link dnd.html drag and drop documentation\endlink.
*/

/*!
    Constructs a TQStoredDrag. The \a dragSource and \a name are passed
    to the TQDragObject constructor, and the format is set to \a
    mimeType.

    The data will be unset. Use setEncodedData() to set it.
*/
TQStoredDrag::TQStoredDrag( const char* mimeType, TQWidget * dragSource, const char * name ) :
    TQDragObject(dragSource,name)
{
    d = new TQStoredDragData();
    d->fmt = tqstrdup(mimeType);
}

/*!
    Destroys the drag object and frees up all allocated resources.
*/
TQStoredDrag::~TQStoredDrag()
{
    delete [] (char*)d->fmt;
    delete d;
}

/*!
    \reimp
*/
const char * TQStoredDrag::format(int i) const
{
    if ( i==0 )
	return d->fmt;
    else
	return 0;
}


/*!
    Sets the encoded data of this drag object to \a encodedData. The
    encoded data is what's delivered to the drop sites. It must be in
    a strictly defined and portable format.

    The drag object can't be dropped (by the user) until this function
    has been called.
*/

void TQStoredDrag::setEncodedData( const TQByteArray & encodedData )
{
    d->enc = encodedData.copy();
}

/*!
    Returns the stored data. \a m contains the data's format.

    \sa setEncodedData()
*/
TQByteArray TQStoredDrag::encodedData(const char* m) const
{
    if ( !tqstricmp(m,d->fmt) )
	return d->enc;
    else
	return TQByteArray();
}


/*!
    \class TQUriDrag ntqdragobject.h
    \brief The TQUriDrag class provides a drag object for a list of URI references.

    \ingroup draganddrop

    URIs are a useful way to refer to files that may be distributed
    across multiple machines. A URI will often refer to a file on a
    machine local to both the drag source and the drop target, so the
    URI can be equivalent to passing a file name but is more
    extensible.

    Use URIs in Unicode form so that the user can comfortably edit and
    view them. For use in HTTP or other protocols, use the correctly
    escaped ASCII form.

    You can convert a list of file names to file URIs using
    setFileNames(), or into human-readble form with setUnicodeUris().

    Static functions are provided to convert between filenames and
    URIs, e.g. uriToLocalFile() and localFileToUri(), and to and from
    human-readable form, e.g. uriToUnicodeUri(), unicodeUriToUri().
    You can also decode URIs from a mimesource into a list with
    decodeLocalFiles() and decodeToUnicodeUris().
*/

/*!
    Constructs an object to drag the list of URIs in \a uris. The \a
    dragSource and \a name arguments are passed on to TQStoredDrag.
    Note that URIs are always in escaped UTF8 encoding.
*/
TQUriDrag::TQUriDrag( TQStrList uris,
	    TQWidget * dragSource, const char * name ) :
    TQStoredDrag( "text/uri-list", dragSource, name )
{
    setUris(uris);
}

/*!
    Constructs an object to drag. You must call setUris() before you
    start the drag(). Passes \a dragSource and \a name to the
    TQStoredDrag constructor.
*/
TQUriDrag::TQUriDrag( TQWidget * dragSource, const char * name ) :
    TQStoredDrag( "text/uri-list", dragSource, name )
{
}

/*!
    Destroys the object.
*/
TQUriDrag::~TQUriDrag()
{
}

/*!
    Changes the list of \a uris to be dragged.

    Note that URIs are always in escaped UTF8 encoding.
*/
void TQUriDrag::setUris( TQStrList uris )
{
    TQByteArray a;
    int c=0;
    for ( const char* s = uris.first(); s; s = uris.next() ) {
	int l = tqstrlen(s);
	a.resize(c+l+2);
	memcpy(a.data()+c,s,l);
	memcpy(a.data()+c+l,"\r\n",2);
	c+=l+2;
    }
    a.resize(c+1);
    a[c] = 0;
    setEncodedData(a);
}


/*!
    Returns true if decode() would be able to decode \a e; otherwise
    returns false.
*/
bool TQUriDrag::canDecode( const TQMimeSource* e )
{
    return e->provides( "text/uri-list" );
}

/*!
    Decodes URIs from \a e, placing the result in \a l (which is first
    cleared).

    Returns true if \a e contained a valid list of URIs; otherwise
    returns false.
*/
bool TQUriDrag::decode( const TQMimeSource* e, TQStrList& l )
{
    TQByteArray payload = e->encodedData( "text/uri-list" );
    if ( payload.size() ) {
	l.clear();
	l.setAutoDelete(true);
	uint c=0;
	const char* d = payload.data();
	while (c < payload.size() && d[c]) {
	    uint f = c;
	    // Find line end
	    while (c < payload.size() && d[c] && d[c]!='\r'
		    && d[c] != '\n')
		c++;
	    TQCString s(d+f,c-f+1);
	    if ( s[0] != '#' ) // non-comment?
		l.append( s );
	    // Skip junk
	    while (c < payload.size() && d[c] &&
		    (d[c]=='\n' || d[c]=='\r'))
		c++;
	}
	return true;
    }
    return false;
}

static uint htod( int h )
{
    if ( isdigit(h) )
	return h - '0';
    return tolower( h ) - 'a' + 10;
}

/*!
  \fn TQUriDrag::setFilenames( const TQStringList & )
  \obsolete

  Use setFileNames() instead (notice the N).
*/

/*!
    Sets the URIs to be the local-file URIs equivalent to \a fnames.

    \sa localFileToUri(), setUris()
*/
void TQUriDrag::setFileNames( const TQStringList & fnames )
{
    TQStrList uris;
    for ( TQStringList::ConstIterator i = fnames.begin();
    i != fnames.end(); ++i ) {
	TQCString fileUri = localFileToUri(*i);
	if (!fileUri.isEmpty())
	    uris.append(fileUri);
    }
    setUris( uris );
}

/*!
    Sets the URIs in \a uuris to be the Unicode URIs (only useful for
    displaying to humans).

    \sa localFileToUri(), setUris()
*/
void TQUriDrag::setUnicodeUris( const TQStringList & uuris )
{
    TQStrList uris;
    for ( TQStringList::ConstIterator i = uuris.begin();
	    i != uuris.end(); ++i )
	uris.append( unicodeUriToUri(*i) );
    setUris( uris );
}

/*!
    Returns the URI equivalent of the Unicode URI given in \a uuri
    (only useful for displaying to humans).

    \sa uriToLocalFile()
*/
TQCString TQUriDrag::unicodeUriToUri(const TQString& uuri)
{
    TQCString utf8 = uuri.utf8();
    TQCString escutf8;
    int n = utf8.length();
    bool isFile = uuri.startsWith("file://");
    for (int i=0; i<n; i++) {
	if ( (utf8[i] >= 'a' && utf8[i] <= 'z')
	  || utf8[i] == '/'
	  || (utf8[i] >= '0' && utf8[i] <= '9')
	  || (utf8[i] >= 'A' && utf8[i] <= 'Z')

	  || utf8[i] == '-' || utf8[i] == '_'
	  || utf8[i] == '.' || utf8[i] == '!'
	  || utf8[i] == '~' || utf8[i] == '*'
	  || utf8[i] == '(' || utf8[i] == ')'
	  || utf8[i] == '\''

	  // Allow this through, so that all URI-references work.
          || (!isFile && utf8[i] == '#')

	  || utf8[i] == ';'
	  || utf8[i] == '?' || utf8[i] == ':'
	  || utf8[i] == '@' || utf8[i] == '&'
	  || utf8[i] == '=' || utf8[i] == '+'
	  || utf8[i] == '$' || utf8[i] == ',' )
	{
	    escutf8 += utf8[i];
	} else {
	    // Everything else is escaped as %HH
	    TQCString s(4);
	    s.sprintf("%%%02x",(uchar)utf8[i]);
	    escutf8 += s;
	}
    }
    return escutf8;
}

/*!
    Returns the URI equivalent to the absolute local file \a filename.

    \sa uriToLocalFile()
*/
TQCString TQUriDrag::localFileToUri(const TQString& filename)
{
    TQString r = filename;

    //check that it is an absolute file
    if (TQDir::isRelativePath(r))
	return TQCString();

#ifdef TQ_WS_WIN


    bool hasHost = false;
    // convert form network path
    if (r.left(2) == "\\\\" || r.left(2) == "//") {
	r.remove(0, 2);
	hasHost = true;
    }

    // Slosh -> Slash
    int slosh;
    while ( (slosh=r.find('\\')) >= 0 ) {
	r[slosh] = '/';
    }

    // Drive
    if ( r[0] != '/' && !hasHost)
	r.insert(0,'/');

#endif
#if defined ( TQ_WS_X11 ) && 0
    // URL without the hostname is considered to be errorneous by XDnD.
    // See: http://www.newplanetsoftware.com/xdnd/dragging_files.html
    // This feature is not active because this would break dnd between old and new qt apps.
    char hostname[257];
    if ( gethostname( hostname, 255 ) == 0 ) {
	hostname[256] = '\0';
	r.prepend( TQString::fromLatin1( hostname ) );
    }
#endif
    return unicodeUriToUri(TQString("file://" + r));
}

/*!
    Returns the Unicode URI (only useful for displaying to humans)
    equivalent of \a uri.

    Note that URIs are always in escaped UTF8 encoding.

    \sa localFileToUri()
*/
TQString TQUriDrag::uriToUnicodeUri(const char* uri)
{
    TQCString utf8;

    while (*uri) {
	switch (*uri) {
	  case '%': {
		uint ch = (uchar) uri[1];
		if ( ch && uri[2] ) {
		    ch = htod( ch ) * 16 + htod( (uchar) uri[2] );
		    utf8 += (char) ch;
		    uri += 2;
		}
	    }
	    break;
	  default:
	    utf8 += *uri;
	}
	++uri;
    }

    return TQString::fromUtf8(utf8);
}

/*!
    Returns the name of a local file equivalent to \a uri or a null
    string if \a uri is not a local file.

    Note that URIs are always in escaped UTF8 encoding.

    \sa localFileToUri()
*/
TQString TQUriDrag::uriToLocalFile(const char* uri)
{
    TQString file;

    if (!uri)
	return file;
    if (0==tqstrnicmp(uri,"file:/",6)) // It is a local file uri
	uri += 6;
    else if (TQString(uri).find(":/") != -1) // It is a different scheme uri
	return file;

    bool local = uri[0] != '/' || ( uri[0] != '\0' && uri[1] == '/' );
#ifdef TQ_WS_X11
    // do we have a hostname?
    if ( !local && uri[0] == '/' && uri[2] != '/' ) {
	// then move the pointer to after the 'hostname/' part of the uri
	const char* hostname_end = strchr( uri+1, '/' );
	if ( hostname_end != NULL ) {
	    char hostname[ 257 ];
	    if ( gethostname( hostname, 255 ) == 0 ) {
		hostname[ 256 ] = '\0';
		if ( tqstrncmp( uri+1, hostname, hostname_end - ( uri+1 )) == 0 ) {
		    uri = hostname_end + 1; // point after the slash
		    local = true;
		}
	    }
	}
    }
#endif
    if ( local ) {
	file = uriToUnicodeUri(uri);
	if ( uri[1] == '/' ) {
	    file.remove((uint)0,1);
	} else {
		file.insert(0,'/');
	}
#ifdef TQ_WS_WIN
	if ( file.length() > 2 && file[0] == '/' && file[2] == '|' ) {
	    file[2] = ':';
	    file.remove(0,1);
	} else if (file.length() > 2 && file[0] == '/' && file[1].isLetter() && file[2] == ':') {
	    file.remove(0, 1);
	}
	// Leave slash as slashes.
#endif
    }
#ifdef TQ_WS_WIN
    else {
	file = uriToUnicodeUri(uri);
	// convert to network path
	file.insert(1, '/'); // leave as forward slashes
    }
#endif

    return file;
}

/*!
    Decodes URIs from the mime source event \a e, converts them to
    local files if they refer to local files, and places them in \a l
    (which is first cleared).

    Returns true if \e contained a valid list of URIs; otherwise
    returns false. The list will be empty if no URIs were local files.
*/
bool TQUriDrag::decodeLocalFiles( const TQMimeSource* e, TQStringList& l )
{
    TQStrList u;
    if ( !decode( e, u ) )
	return false;

    l.clear();
    for (const char* s=u.first(); s; s=u.next()) {
	TQString lf = uriToLocalFile(s);
	if ( !lf.isNull() )
	    l.append( lf );
    }
    return true;
}

/*!
    Decodes URIs from the mime source event \a e, converts them to
    Unicode URIs (only useful for displaying to humans), placing them
    in \a l (which is first cleared).

    Returns true if \e contained a valid list of URIs; otherwise
    returns false.
*/
bool TQUriDrag::decodeToUnicodeUris( const TQMimeSource* e, TQStringList& l )
{
    TQStrList u;
    if ( !decode( e, u ) )
	return false;

    l.clear();
    for (const char* s=u.first(); s; s=u.next())
	l.append( uriToUnicodeUri(s) );

    return true;
}


#ifndef TQT_NO_DRAGANDDROP
/*!
    If the source of the drag operation is a widget in this
    application, this function returns that source, otherwise it
    returns 0. The source of the operation is the first parameter to
    drag object subclasses.

    This is useful if your widget needs special behavior when dragging
    to itself, etc.

    See TQDragObject::TQDragObject() and subclasses.
*/
TQWidget* TQDropEvent::source() const
{
    return tqt_dnd_manager ? tqt_dnd_manager->dragSource : 0;
}
#endif

/*!
    \class TQColorDrag ntqdragobject.h

    \brief The TQColorDrag class provides a drag and drop object for
    transferring colors.

    \ingroup draganddrop

    This class provides a drag object which can be used to transfer data
    about colors for drag and drop and in the clipboard. For example, it
    is used in TQColorDialog.

    The color is set in the constructor but can be changed with
    setColor().

    For more information about drag and drop, see the TQDragObject class
    and the \link dnd.html drag and drop documentation\endlink.
*/

/*!
    Constructs a color drag object with the color \a col. Passes \a
    dragsource and \a name to the TQStoredDrag constructor.
*/

TQColorDrag::TQColorDrag( const TQColor &col, TQWidget *dragsource, const char *name )
    : TQStoredDrag( "application/x-color", dragsource, name )
{
    setColor( col );
}

/*!
    Constructs a color drag object with a white color. Passes \a
    dragsource and \a name to the TQStoredDrag constructor.
*/

TQColorDrag::TQColorDrag( TQWidget *dragsource, const char *name )
    : TQStoredDrag( "application/x-color", dragsource, name )
{
    setColor( TQt::white );
}

/*!
    Sets the color of the color drag to \a col.
*/

void TQColorDrag::setColor( const TQColor &col )
{
    unsigned short r = (col.red()   << 8) | col.red();
    unsigned short g = (col.green() << 8) | col.green();
    unsigned short b = (col.blue()  << 8) | col.blue();

    // make sure we transmit data in network order
    r = htons(r);
    g = htons(g);
    b = htons(b);

    ushort rgba[4] = {
	r, g, b,
	0xffff // Alpha not supported yet.
    };
    TQByteArray data(sizeof(rgba));
    memcpy(data.data(), rgba, sizeof(rgba));
    setEncodedData(data);
}

/*!
    Returns true if the color drag object can decode the mime source
    \a e; otherwise returns false.
*/

bool TQColorDrag::canDecode( TQMimeSource *e )
{
    return e->provides( "application/x-color" );
}

/*!
    Decodes the mime source \a e and sets the decoded values to \a
    col.
*/

bool TQColorDrag::decode( TQMimeSource *e, TQColor &col )
{
    TQByteArray data = e->encodedData("application/x-color");
    ushort rgba[4];
    if (data.size() != sizeof(rgba))
	return false;

    memcpy(rgba, data.data(), sizeof(rgba));

    short r = rgba[0];
    short g = rgba[1];
    short b = rgba[2];

    // data is in network order
    r = ntohs(r);
    g = ntohs(g);
    b = ntohs(b);

    r = (r >> 8) & 0xff;
    g = (g >> 8) & 0xff;
    b = (b >> 8) & 0xff;

    col.setRgb(r, g, b);
    return true;
}

#endif // TQT_NO_MIME
