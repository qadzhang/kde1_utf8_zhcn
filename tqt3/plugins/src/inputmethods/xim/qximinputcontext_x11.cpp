/****************************************************************************
** $Id: qximinputcontext_x11.cpp,v 1.10 2004/06/22 06:47:27 daisuke Exp $
**
** Implementation of TQXIMInputContext class
**
** Copyright (C) 2000-2003 Trolltech AS.  All rights reserved.
**
** This file is part of the input method module of the TQt GUI Toolkit.
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


#include "qximinputcontext.h"

const int XKeyPress = KeyPress;
const int XKeyRelease = KeyRelease;
#undef KeyPress
#undef KeyRelease

#if !defined(TQT_NO_IM)

#include "qplatformdefs.h"

#include "ntqapplication.h"
#include "ntqwidget.h"
#include "ntqstring.h"
#include "ntqptrlist.h"
#include "ntqintdict.h"
#include "ntqtextcodec.h"

#include <stdlib.h>
#include <limits.h>

#if !defined(TQT_NO_XIM)

#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>

// #define QT_XIM_DEBUG

// from qapplication_x11.cpp
static XIM	tqt_xim = 0;
extern XIMStyle	tqt_xim_style;
extern XIMStyle	tqt_xim_preferred_style;
extern char    *tqt_ximServer;
static bool isInitXIM = false;
static TQPtrList<TQXIMInputContext> *ximContextList = 0;
#endif
extern int tqt_ximComposingKeycode;
extern TQTextCodec * tqt_input_mapper;


#if !defined(TQT_NO_XIM)

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif // Q_C_CALLBACKS

#ifdef USE_X11R6_XIM
    static void xim_create_callback(XIM /*im*/,
				    XPointer /*client_data*/,
				    XPointer /*call_data*/)
    {
	// tqDebug("xim_create_callback");
	TQXIMInputContext::create_xim();
    }

    static void xim_destroy_callback(XIM /*im*/,
				     XPointer /*client_data*/,
				     XPointer /*call_data*/)
    {
	// tqDebug("xim_destroy_callback");
	TQXIMInputContext::close_xim();
	Display *dpy = TQPaintDevice::x11AppDisplay();
	XRegisterIMInstantiateCallback(dpy, 0, 0, 0,
				       (XIMProc) xim_create_callback, 0);
    }

#endif // USE_X11R6_XIM

#if defined(Q_C_CALLBACKS)
}
#endif // Q_C_CALLBACKS

#endif // TQT_NO_XIM

#ifndef TQT_NO_XIM

/* The cache here is needed, as X11 leaks a few kb for every
   XFreeFontSet call, so we avoid creating and deletion of fontsets as
   much as possible
*/
static XFontSet fontsetCache[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static int fontsetCacheRefCount = 0;

static const char * const fontsetnames[] = {
    "-*-fixed-medium-r-*-*-16-*,-*-*-medium-r-*-*-16-*",
    "-*-fixed-medium-i-*-*-16-*,-*-*-medium-i-*-*-16-*",
    "-*-fixed-bold-r-*-*-16-*,-*-*-bold-r-*-*-16-*",
    "-*-fixed-bold-i-*-*-16-*,-*-*-bold-i-*-*-16-*",
    "-*-fixed-medium-r-*-*-24-*,-*-*-medium-r-*-*-24-*",
    "-*-fixed-medium-i-*-*-24-*,-*-*-medium-i-*-*-24-*",
    "-*-fixed-bold-r-*-*-24-*,-*-*-bold-r-*-*-24-*",
    "-*-fixed-bold-i-*-*-24-*,-*-*-bold-i-*-*-24-*"
};

static XFontSet getFontSet( const TQFont &f )
{
    int i = 0;
    if (f.italic())
	i |= 1;
    if (f.bold())
	i |= 2;

    if ( f.pointSize() > 20 )
	i += 4;

    if ( !fontsetCache[i] ) {
	Display* dpy = TQPaintDevice::x11AppDisplay();
	int missCount;
	char** missList;
	fontsetCache[i] = XCreateFontSet(dpy, fontsetnames[i], &missList, &missCount, 0);
	if(missCount > 0)
	    XFreeStringList(missList);
	if ( !fontsetCache[i] ) {
	    fontsetCache[i] = XCreateFontSet(dpy,  "-*-fixed-*-*-*-*-16-*", &missList, &missCount, 0);
	    if(missCount > 0)
		XFreeStringList(missList);
	    if ( !fontsetCache[i] )
		fontsetCache[i] = (XFontSet)-1;
	}
    }
    return (fontsetCache[i] == (XFontSet)-1) ? 0 : fontsetCache[i];
}


#ifdef Q_C_CALLBACKS
extern "C" {
#endif // Q_C_CALLBACKS

    // These static functions should be rewritten as member of
    // TQXIMInputContext

    static int xic_start_callback(XIC, XPointer client_data, XPointer) {
	TQXIMInputContext *qic = (TQXIMInputContext *) client_data;
	if (! qic) {
#ifdef QT_XIM_DEBUG
	    tqDebug("compose start: no qic");
#endif // QT_XIM_DEBUG

	    return 0;
	}

	qic->resetClientState();
	qic->sendIMEvent( TQEvent::IMStart );

#ifdef QT_XIM_DEBUG
	tqDebug("compose start");
#endif // QT_XIM_DEBUG

	return 0;
    }

    static int xic_draw_callback(XIC, XPointer client_data, XPointer call_data) {
	TQXIMInputContext *qic = (TQXIMInputContext *) client_data;
	if (! qic) {
#ifdef QT_XIM_DEBUG
	    tqDebug("compose event: invalid compose event %p", qic);
#endif // QT_XIM_DEBUG

	    return 0;
	}

	bool send_imstart = false;
	if( ! qic->isComposing() && qic->hasFocus() ) {
	    qic->resetClientState();
	    send_imstart = true;
	} else if ( ! qic->isComposing() || ! qic->hasFocus() ) {
#ifdef QT_XIM_DEBUG
	    tqDebug( "compose event: invalid compose event composing=%d hasFocus=%d",
		    qic->isComposing(), qic->hasFocus() );
#endif // QT_XIM_DEBUG

	    return 0;
	}

	if ( send_imstart )
	    qic->sendIMEvent( TQEvent::IMStart );

	XIMPreeditDrawCallbackStruct *drawstruct =
	    (XIMPreeditDrawCallbackStruct *) call_data;
	XIMText *text = (XIMText *) drawstruct->text;
	int cursor = drawstruct->caret, sellen = 0;

	if ( ! drawstruct->caret && ! drawstruct->chg_first &&
	     ! drawstruct->chg_length && ! text ) {
	    if( qic->composingText.isEmpty() ) {
#ifdef QT_XIM_DEBUG
		tqDebug( "compose emptied" );
#endif // QT_XIM_DEBUG
		// if the composition string has been emptied, we need
		// to send an IMEnd event
		qic->sendIMEvent( TQEvent::IMEnd );
		qic->resetClientState();
		// if the commit string has coming after here, IMStart
		// will be sent dynamically
	    }
	    return 0;
	}

	if (text) {
	    char *str = 0;
	    if (text->encoding_is_wchar) {
		int l = wcstombs(NULL, text->string.wide_char, text->length);
		if (l != -1) {
		    str = new char[l + 1];
		    wcstombs(str, text->string.wide_char, l);
		    str[l] = 0;
		}
	    } else
		str = text->string.multi_byte;

	    if (! str)
		return 0;

	    TQString s = TQString::fromLocal8Bit(str);

	    if (text->encoding_is_wchar)
		delete [] str;

	    if (drawstruct->chg_length < 0)
		qic->composingText.replace(drawstruct->chg_first, UINT_MAX, s);
	    else
		qic->composingText.replace(drawstruct->chg_first, drawstruct->chg_length, s);

	    if ( qic->selectedChars.size() < qic->composingText.length() ) {
		// expand the selectedChars array if the compose string is longer
		uint from = qic->selectedChars.size();
		qic->selectedChars.resize( qic->composingText.length() );
		for ( uint x = from; from < qic->selectedChars.size(); ++x )
		    qic->selectedChars[x] = 0;
	    }

	    uint x;
	    bool *p = qic->selectedChars.data() + drawstruct->chg_first;
	    // determine if the changed chars are selected based on text->feedback
	    for ( x = 0; x < s.length(); ++x )
		*p++ = ( text->feedback ? ( text->feedback[x] & XIMReverse ) : 0 );

	    // figure out where the selection starts, and how long it is
	    p = qic->selectedChars.data();
	    bool started = false;
	    for ( x = 0; x < TQMIN(qic->composingText.length(), qic->selectedChars.size()); ++x ) {
		if ( started ) {
		    if ( *p ) ++sellen;
		    else break;
		} else {
		    if ( *p ) {
			cursor = x;
			started = true;
			sellen = 1;
		    }
		}
		++p;
	    }
	} else {
	    if (drawstruct->chg_length == 0)
		drawstruct->chg_length = -1;

	    qic->composingText.remove(drawstruct->chg_first, drawstruct->chg_length);
	    bool qt_compose_emptied = qic->composingText.isEmpty();
	    if ( qt_compose_emptied ) {
#ifdef QT_XIM_DEBUG
		tqDebug( "compose emptied" );
#endif // QT_XIM_DEBUG
		// if the composition string has been emptied, we need
		// to send an IMEnd event
		qic->sendIMEvent( TQEvent::IMEnd );
		qic->resetClientState();
		// if the commit string has coming after here, IMStart
		// will be sent dynamically
		return 0;
	    }
	}

	qic->sendIMEvent( TQEvent::IMCompose,
			  qic->composingText, cursor, sellen );

	return 0;
    }

    static int xic_done_callback(XIC, XPointer client_data, XPointer) {
	TQXIMInputContext *qic = (TQXIMInputContext *) client_data;
	if (! qic)
	    return 0;

	// Don't send IMEnd here. TQXIMInputContext::x11FilterEvent()
	// handles IMEnd with commit string.
#if 0
	if ( qic->isComposing() )
	    qic->sendIMEvent( TQEvent::IMEnd );
	qic->resetClientState();
#endif

	return 0;
    }

#ifdef Q_C_CALLBACKS
}
#endif // Q_C_CALLBACKS

#endif // !TQT_NO_XIM



TQXIMInputContext::TQXIMInputContext()
    : TQInputContext(), ic(0), fontset(0)
{
    if(!isInitXIM)
	TQXIMInputContext::init_xim();

#if !defined(TQT_NO_XIM)
    fontsetCacheRefCount++;

    if( ! ximContextList )
	ximContextList = new TQPtrList<TQXIMInputContext>;
    ximContextList->append( this );
#endif // !TQT_NO_XIM
}


void TQXIMInputContext::setHolderWidget( TQWidget *widget )
{
    if ( ! widget )
	return;

    TQInputContext::setHolderWidget( widget );

#if !defined(TQT_NO_XIM)
    if (! tqt_xim) {
	tqWarning("TQInputContext: no input method context available");
	return;
    }

    if (! widget->isTopLevel()) {
       // tqWarning("TQInputContext: cannot create input context for non-toplevel widgets");
	return;
    }

    XPoint spot;
    XRectangle rect;
    XVaNestedList preedit_attr = 0;
    XIMCallback startcallback, drawcallback, donecallback;

    font = widget->font();
    fontset = getFontSet( font );

    if (tqt_xim_style & XIMPreeditArea) {
	rect.x = 0;
	rect.y = 0;
	rect.width = widget->width();
	rect.height = widget->height();

	preedit_attr = XVaCreateNestedList(0,
					   XNArea, &rect,
					   XNFontSet, fontset,
					   (char *) 0);
    } else if (tqt_xim_style & XIMPreeditPosition) {
	spot.x = 1;
	spot.y = 1;

	preedit_attr = XVaCreateNestedList(0,
					   XNSpotLocation, &spot,
					   XNFontSet, fontset,
					   (char *) 0);
    } else if (tqt_xim_style & XIMPreeditCallbacks) {
	startcallback.client_data = (XPointer) this;
	startcallback.callback = (XIMProc) xic_start_callback;
	drawcallback.client_data = (XPointer) this;
	drawcallback.callback = (XIMProc)xic_draw_callback;
	donecallback.client_data = (XPointer) this;
	donecallback.callback = (XIMProc) xic_done_callback;

	preedit_attr = XVaCreateNestedList(0,
					   XNPreeditStartCallback, &startcallback,
					   XNPreeditDrawCallback, &drawcallback,
					   XNPreeditDoneCallback, &donecallback,
					   (char *) 0);
    }

    if (preedit_attr) {
	ic = XCreateIC(tqt_xim,
		       XNInputStyle, tqt_xim_style,
		       XNClientWindow, widget->winId(),
		       XNPreeditAttributes, preedit_attr,
		       (char *) 0);
	XFree(preedit_attr);
    } else
	ic = XCreateIC(tqt_xim,
		       XNInputStyle, tqt_xim_style,
		       XNClientWindow, widget->winId(),
		       (char *) 0);

    if (! ic)
	tqFatal("Failed to create XIM input context!");

    // when resetting the input context, preserve the input state
    (void) XSetICValues((XIC) ic, XNResetState, XIMPreserveState, (char *) 0);
#endif // !TQT_NO_XIM
}


TQXIMInputContext::~TQXIMInputContext()
{

#if !defined(TQT_NO_XIM)
    if (ic)
	XDestroyIC((XIC) ic);

    if ( --fontsetCacheRefCount == 0 ) {
	Display *dpy = TQPaintDevice::x11AppDisplay();
	for ( int i = 0; i < 8; i++ ) {
	    if ( fontsetCache[i] && fontsetCache[i] != (XFontSet)-1 ) {
		XFreeFontSet(dpy, fontsetCache[i]);
		fontsetCache[i] = 0;
	    }
	}
    }

    if( ximContextList ) {
	ximContextList->remove( this );
	if(ximContextList->isEmpty()) {
	    // Calling XCloseIM gives a Purify FMR error
	    // XCloseIM( tqt_xim );
	    // We prefer a less serious memory leak
	    if( tqt_xim ) {
		tqt_xim = 0;
		isInitXIM = false;
	    }

	    delete ximContextList;
	    ximContextList = 0;
	}
    }
#endif // !TQT_NO_XIM

    ic = 0;
}

void TQXIMInputContext::init_xim()
{
#ifndef TQT_NO_XIM
    if(!isInitXIM)
	isInitXIM = true;

    tqt_xim = 0;
    TQString ximServerName(tqt_ximServer);
    if (tqt_ximServer)
	ximServerName.prepend("@im=");
    else
	ximServerName = "";

    if ( !XSupportsLocale() )
	tqWarning("TQt: Locales not supported on X server");

#ifdef USE_X11R6_XIM
    else if ( XSetLocaleModifiers (ximServerName.ascii()) == 0 )
	tqWarning( "TQt: Cannot set locale modifiers: %s",
		  ximServerName.ascii());
    else {
	Display *dpy = TQPaintDevice::x11AppDisplay();
	XWindowAttributes attr; // XIM unselects all events on the root window
	XGetWindowAttributes( dpy, TQPaintDevice::x11AppRootWindow(),&attr );
	XRegisterIMInstantiateCallback(dpy, 0, 0, 0,
				       (XIMProc) xim_create_callback, 0);
	XSelectInput( dpy, TQPaintDevice::x11AppRootWindow(), attr.your_event_mask );
    }
#else // !USE_X11R6_XIM
    else if ( XSetLocaleModifiers ("") == 0 )
	tqWarning("TQt: Cannot set locale modifiers");
    else
	TQXIMInputContext::create_xim();
#endif // USE_X11R6_XIM
#endif // TQT_NO_XIM
}


/*! \internal
  Creates the application input method.
 */
void TQXIMInputContext::create_xim()
{
#ifndef TQT_NO_XIM
    Display *appDpy = TQPaintDevice::x11AppDisplay();
    tqt_xim = XOpenIM( appDpy, 0, 0, 0 );
    if ( tqt_xim ) {

#ifdef USE_X11R6_XIM
	XIMCallback destroy;
	destroy.callback = (XIMProc) xim_destroy_callback;
	destroy.client_data = 0;
	if ( XSetIMValues( tqt_xim, XNDestroyCallback, &destroy, (char *) 0 ) != 0 )
	    tqWarning( "Xlib doesn't support destroy callback");
#endif // USE_X11R6_XIM

	XIMStyles *styles = 0;
	XGetIMValues(tqt_xim, XNQueryInputStyle, &styles, (char *) 0, (char *) 0);
	if ( styles ) {
	    int i;
	    for ( i = 0; !tqt_xim_style && i < styles->count_styles; i++ ) {
		if ( styles->supported_styles[i] == tqt_xim_preferred_style ) {
		    tqt_xim_style = tqt_xim_preferred_style;
		    break;
		}
	    }
	    // if the preferred input style couldn't be found, look for
	    // Nothing
	    for ( i = 0; !tqt_xim_style && i < styles->count_styles; i++ ) {
		if ( styles->supported_styles[i] == (XIMPreeditNothing |
						     XIMStatusNothing) ) {
		    tqt_xim_style = XIMPreeditNothing | XIMStatusNothing;
		    break;
		}
	    }
	    // ... and failing that, None.
	    for ( i = 0; !tqt_xim_style && i < styles->count_styles; i++ ) {
		if ( styles->supported_styles[i] == (XIMPreeditNone |
						     XIMStatusNone) ) {
		    tqt_xim_style = XIMPreeditNone | XIMStatusNone;
		    break;
		}
	    }

	    // tqDebug("TQApplication: using im style %lx", tqt_xim_style);
	    XFree( (char *)styles );
	}

	if ( tqt_xim_style ) {

#ifdef USE_X11R6_XIM
	    XUnregisterIMInstantiateCallback(appDpy, 0, 0, 0,
					     (XIMProc) xim_create_callback, 0);
#endif // USE_X11R6_XIM

	} else {
	    // Give up
	    tqWarning( "No supported input style found."
		      "  See InputMethod documentation.");
	    TQXIMInputContext::close_xim();
	}
    }
#endif // TQT_NO_XIM
}


/*! \internal
  Closes the application input method.
*/
void TQXIMInputContext::close_xim()
{
#ifndef TQT_NO_XIM
    TQString errMsg( "TQXIMInputContext::close_xim() has been called" );

    // Calling XCloseIM gives a Purify FMR error
    // XCloseIM( tqt_xim );
    // We prefer a less serious memory leak

    tqt_xim = 0;
    if( ximContextList ) {
	TQPtrList<TQXIMInputContext> contexts( *ximContextList );
	TQPtrList<TQXIMInputContext>::Iterator it = contexts.begin();
	while( it != contexts.end() ) {
	    (*it)->close( errMsg );
	    ++it;
	}
	// ximContextList will be deleted in ~TQXIMInputContext
    }
#endif // TQT_NO_XIM
}


bool TQXIMInputContext::x11FilterEvent( TQWidget *keywidget, XEvent *event )
{
#ifndef TQT_NO_XIM
    int xkey_keycode = event->xkey.keycode;
    if ( XFilterEvent( event, keywidget->topLevelWidget()->winId() ) ) {
	tqt_ximComposingKeycode = xkey_keycode; // ### not documented in xlib

	// Cancel of the composition is realizable even if
	// follwing codes don't exist
#if 0
	if ( event->type != XKeyPress || ! (tqt_xim_style & XIMPreeditCallbacks) )
	    return true;

	/*
	 * The Solaris htt input method will transform a ClientMessage
	 * event into a filtered KeyPress event, in which case our
	 * keywidget is still zero.
	 */
	TQETWidget *widget = (TQETWidget*)TQWidget::find( (WId)event->xany.window );
        if ( ! keywidget ) {
 	    keywidget = (TQETWidget*)TQWidget::keyboardGrabber();
	    if ( keywidget ) {
	        grabbed = true;
	    } else {
	        if ( focus_widget )
		    keywidget = (TQETWidget*)focus_widget;
	        if ( !keywidget ) {
		    if ( tqApp->inPopupMode() ) // no focus widget, see if we have a popup
		        keywidget = (TQETWidget*) tqApp->activePopupWidget();
		    else if ( widget )
		        keywidget = (TQETWidget*)widget->topLevelWidget();
	        }
	    }
        }

	/*
	  if the composition string has been emptied, we need to send
	  an IMEnd event.  however, we have no way to tell if the user
	  has cancelled input, or if the user has accepted the
	  composition.

	  so, we have to look for the next keypress and see if it is
	  the 'commit' key press (keycode == 0).  if it is, we deliver
	  an IMEnd event with the final text, otherwise we deliver an
	  IMEnd with empty text (meaning the user has cancelled the
	  input).
	*/
	if ( composing && focusWidget && qt_compose_emptied ) {
	    XEvent event2;
	    bool found = false;
	    if ( XCheckTypedEvent( TQPaintDevice::x11AppDisplay(),
				   XKeyPress, &event2 ) ) {
		if ( event2.xkey.keycode == 0 ) {
		    // found a key event with the 'commit' string
		    found = true;
		    XPutBackEvent( TQPaintDevice::x11AppDisplay(), &event2 );
		}
	    }

	    if ( !found ) {
		// no key event, so the user must have cancelled the composition
		TQIMEvent endevent( TQEvent::IMEnd, TQString::null, -1 );
		TQApplication::sendEvent( focusWidget, &endevent );

		focusWidget = 0;
	    }

	    qt_compose_emptied = false;
	}
#endif
	return true;
    } else if ( focusWidget() ) {
        if ( event->type == XKeyPress && event->xkey.keycode == 0 ) {
	    // input method has sent us a commit string
	    TQCString data(513);
	    KeySym sym;    // unused
	    Status status; // unused
	    TQString inputText;
	    int count = lookupString( &(event->xkey), data, &sym, &status );
	    if ( count > 0 )
	        inputText = tqt_input_mapper->toUnicode( data, count );

	    if ( ! ( tqt_xim_style & XIMPreeditCallbacks ) || ! isComposing() ) {
		// there is no composing state
		sendIMEvent( TQEvent::IMStart );
	    }

	    sendIMEvent( TQEvent::IMEnd, inputText );
	    resetClientState();

	    return true;
	}
    }
#endif // !TQT_NO_XIM

    return false;
}


void TQXIMInputContext::sendIMEvent( TQEvent::Type type, const TQString &text,
				    int cursorPosition, int selLength )
{
    TQInputContext::sendIMEvent( type, text, cursorPosition, selLength );
    if ( type == TQEvent::IMCompose )
	composingText = text;
}


void TQXIMInputContext::reset()
{
#if !defined(TQT_NO_XIM)
    if ( focusWidget() && isComposing() && ! composingText.isNull() ) {
#ifdef QT_XIM_DEBUG
	tqDebug("TQXIMInputContext::reset: composing - sending IMEnd (empty) to %p",
	       focusWidget() );
#endif // QT_XIM_DEBUG

	TQInputContext::reset();
	resetClientState();

	char *mb = XmbResetIC((XIC) ic);
	if (mb)
	    XFree(mb);
    }
#endif // !TQT_NO_XIM
}


void TQXIMInputContext::resetClientState()
{
#if !defined(TQT_NO_XIM)
    composingText = TQString::null;
    if ( selectedChars.size() < 128 )
	selectedChars.resize( 128 );
    selectedChars.fill( 0 );
#endif // !TQT_NO_XIM
}


void TQXIMInputContext::close( const TQString &errMsg )
{
    tqDebug( "%s", errMsg.latin1() );
    emit deletionRequested();
}


bool TQXIMInputContext::hasFocus() const
{
    return ( focusWidget() != 0 );
}


void TQXIMInputContext::setMicroFocus(int x, int y, int, int h, TQFont *f)
{
    TQWidget *widget = focusWidget();
    if ( tqt_xim && widget ) {
	TQPoint p( x, y );
	TQPoint p2 = widget->mapTo( widget->topLevelWidget(), TQPoint( 0, 0 ) );
	p = widget->topLevelWidget()->mapFromGlobal( p );
	setXFontSet( f ? *f : widget->font() );
	setComposePosition(p.x(), p.y() + h);
	setComposeArea(p2.x(), p2.y(), widget->width(), widget->height());
    }

}

void TQXIMInputContext::mouseHandler( int , TQEvent::Type type,
				     TQt::ButtonState button,
				     TQt::ButtonState)
{
    if ( type == TQEvent::MouseButtonPress ||
	 type == TQEvent::MouseButtonDblClick ) {
	// Don't reset Japanese input context here. Japanese input
	// context sometimes contains a whole paragraph and has
	// minutes of lifetime different to ephemeral one in other
	// languages. The input context should be survived until
	// focused again.
	if ( ! isPreeditPreservationEnabled() )
	    reset();
    }
}

void TQXIMInputContext::setComposePosition(int x, int y)
{
#if !defined(TQT_NO_XIM)
    if (tqt_xim && ic) {
	XPoint point;
	point.x = x;
	point.y = y;

	XVaNestedList preedit_attr =
	    XVaCreateNestedList(0,
				XNSpotLocation, &point,

				(char *) 0);
	XSetICValues((XIC) ic, XNPreeditAttributes, preedit_attr, (char *) 0);
	XFree(preedit_attr);
    }
#endif // !TQT_NO_XIM
}


void TQXIMInputContext::setComposeArea(int x, int y, int w, int h)
{
#if !defined(TQT_NO_XIM)
    if (tqt_xim && ic) {
	XRectangle rect;
	rect.x = x;
	rect.y = y;
	rect.width = w;
	rect.height = h;

	XVaNestedList preedit_attr = XVaCreateNestedList(0,
							 XNArea, &rect,

							 (char *) 0);
	XSetICValues((XIC) ic, XNPreeditAttributes, preedit_attr, (char *) 0);
	XFree(preedit_attr);
    }
#endif
}


void TQXIMInputContext::setXFontSet(const TQFont &f)
{
#if !defined(TQT_NO_XIM)
    if (font == f) return; // nothing to do
    font = f;

    XFontSet fs = getFontSet(font);
    if (fontset == fs) return; // nothing to do
    fontset = fs;

    XVaNestedList preedit_attr = XVaCreateNestedList(0, XNFontSet, fontset, (char *) 0);
    XSetICValues((XIC) ic, XNPreeditAttributes, preedit_attr, (char *) 0);
    XFree(preedit_attr);
#else
    Q_UNUSED( f );
#endif
}


int TQXIMInputContext::lookupString(XKeyEvent *event, TQCString &chars,
				KeySym *key, Status *status) const
{
    int count = 0;

#if !defined(TQT_NO_XIM)
    if (tqt_xim && ic) {
	count = XmbLookupString((XIC) ic, event, chars.data(),
				chars.size(), key, status);

	if ((*status) == XBufferOverflow ) {
	    chars.resize(count + 1);
	    count = XmbLookupString((XIC) ic, event, chars.data(),
				    chars.size(), key, status);
	}
    }

#endif // TQT_NO_XIM

    return count;
}

void TQXIMInputContext::setFocus()
{
#if !defined(TQT_NO_XIM)
    if ( tqt_xim && ic )
	XSetICFocus((XIC) ic);
#endif // !TQT_NO_XIM
}

void TQXIMInputContext::unsetFocus()
{
#if !defined(TQT_NO_XIM)
    if (tqt_xim && ic)
	XUnsetICFocus((XIC) ic);
#endif // !TQT_NO_XIM

    // Don't reset Japanese input context here. Japanese input context
    // sometimes contains a whole paragraph and has minutes of
    // lifetime different to ephemeral one in other languages. The
    // input context should be survived until focused again.
    if ( ! isPreeditPreservationEnabled() )
	reset();
}


bool TQXIMInputContext::isPreeditRelocationEnabled()
{
    return ( language() == "ja" );
}


bool TQXIMInputContext::isPreeditPreservationEnabled()
{
    return ( language() == "ja" );
}


TQString TQXIMInputContext::identifierName()
{
    // the name should be "xim" rather than "XIM" to be consistent
    // with corresponding immodule of GTK+
    return "xim";
}


TQString TQXIMInputContext::language()
{
#if !defined(TQT_NO_XIM)
    if ( tqt_xim ) {
	TQString locale( XLocaleOfIM( tqt_xim ) );

	if ( locale.startsWith( "zh" ) ) {
	    // Chinese language should be formed as "zh_CN", "zh_TW", "zh_HK"
	    _language = locale.left( 5 );
	} else {
	    // other languages should be two-letter ISO 639 language code
	    _language = locale.left( 2 );
	}
    }
#endif
    return _language;
}

#endif //TQT_NO_IM
