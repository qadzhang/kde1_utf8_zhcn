/****************************************************************************
** $Id: qinputcontext.cpp,v 1.6 2004/06/22 06:47:30 daisuke Exp $
**
** Implementation of TQInputContext class
**
** Copyright (C) 2000-2003 Trolltech AS.  All rights reserved.
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

//#define TQT_NO_IM_PREEDIT_RELOCATION

#include "ntqinputcontext.h"

#ifndef TQT_NO_IM

#include "qplatformdefs.h"

#include "ntqapplication.h"
#include "ntqwidget.h"
#include "ntqpopupmenu.h"

#include <stdlib.h>
#include <limits.h>

class TQInputContextPrivate
{
public:
    TQInputContextPrivate()
	: holderWidget( 0 ), composingWidget( 0 ), hasFocus( false ),
	  isComposing( false ) 
#if !defined(TQT_NO_IM_PREEDIT_RELOCATION)
	  , preeditString( TQString::null ),
	  cursorPosition( -1 ), selLength ( 0 )
#endif
    {}

    TQWidget *holderWidget; // widget to which TQInputContext instance belongs.
    TQWidget *composingWidget;
    bool hasFocus;
    bool isComposing;

    void updateComposingState( const TQString &text,
			       int newCursorPosition, int newSelLength ) {
#if !defined(TQT_NO_IM_PREEDIT_RELOCATION)
	preeditString = text;
	cursorPosition = newCursorPosition;
	selLength = newSelLength;
#endif
    }

    void resetComposingState() {
	isComposing = false;
#if !defined(TQT_NO_IM_PREEDIT_RELOCATION)
	preeditString = TQString::null;
	cursorPosition = -1;
	selLength = 0;
#endif
    }

#if !defined(TQT_NO_IM_PREEDIT_RELOCATION)
    TQString preeditString;
    int cursorPosition;
    int selLength;
#endif
};


// UPDATED COMMENT REQUIRED -- 2004-07-08 YamaKen
/*!
    \class TQInputContext ntqinputcontext.h
    \brief The TQInputContext class abstracts the input method dependent data and composing state.

    \ingroup i18n

    An input method is responsible to input complex text that cannot
    be inputted via simple keymap. It converts a sequence of input
    events (typically key events) into a text string through the input
    method specific converting process. The class of the processes are
    widely ranging from simple finite state machine to complex text
    translator that pools a whole paragraph of a text with text
    editing capability to perform grammar and semantic analysis.

    To abstract such different input method specific intermediate
    information, TQt offers the TQInputContext as base class. The
    concept is well known as 'input context' in the input method
    domain. an input context is created for a text widget in response
    to a demand. It is ensured that an input context is prepared for
    an input method before input to a text widget.

    Multiple input contexts that is belonging to a single input method
    may concurrently coexist. Suppose multi-window text editor. Each
    text widget of window A and B holds different TQInputContext
    instance which contains different state information such as
    partially composed text.

    \section1 Groups of functions:

    \table
    \header \i Context \i Functions

    \row \i Receiving information \i
	x11FilterEvent(),
	filterEvent(),
	setMicroFocus(),
	mouseHandler()

    \row \i Sending back composed text \i
	sendIMEvent(),

    \row \i State change notification \i
	setFocus(),
	unsetFocus(),
	reset()

    \row \i Context information \i
	identifierName(),
	language(),
	font(),
	isComposing(),

    \endtable


    \section1 Sharing input context between text widgets

    Any input context can be shared between several text widgets to
    reduce resource consumption. In ideal case, each text widgets
    should be allocated dedicated input context. But some complex
    input contexts require slightly heavy resource such as 100
    kilobytes of memory. It prevents quite many text widgets from
    being used concurrently.

    To resolve such problem, we can share an input context. There is
    one 'input context holder widget' per text widgets that shares
    identical input context. In this model, the holder widget owns the
    shared input context. Other text widgets access the input context
    via TQApplication::locateICHolderWidget(). But the access
    convention is transparently hidden into TQWidget, so developers are
    not required to aware of it.

    What developer should know is only the mapping function
    TQApplication::locateICHolderWidget(). It accepts a widget as
    argument and returns its holder widget. Default implementation
    returns the top-level widget of the widget as reasonable
    assumption.  But some applications should reimplement the function
    to fit application specific usability. See
    TQApplication::locateICHolderWidget() for further information.


    \section1 Preedit preservation

    As described above, input contexts have wide variety of amount of
    the state information in accordance with belonging input
    method. It is ranging from 2-3 keystrokes of sequence in
    deterministic input methods to hundreds of keystrokes with
    semantic text refinement in complex input methods such as ordinary
    Japanese input method. The difference requires the different reset
    policies in losing input focus.

    The former simple input method case, users will prefer resetting
    the context to back to the neutral state when something
    happened. Suppose a web browsing. The user scroll the page by
    scrollbar after he or she has typed a half of the valid key
    sequence into a text widget. In the case, the input context should
    be reset in losing focus when he or she has dragged the
    scrollbar. He or she will be confused if the input context is
    still preserved until focused back to the text widget because he
    or she will restart typing with first key of the sequence as a
    habitual operation.

    On the other hand, we should choose completely different policy
    for the latter complex input method case. Suppose same situation
    as above but he or she is using a complex input method. In the
    case, he or she will be angry if the input context has been lost
    when he or she has dragged the scrollbar because the input context
    contained a valuably composed text made up by considerable input
    cost. So we should not reset the input context in the case. And
    the input context should be preserved until focused back to the
    text widget. This behavior is named as 'preedit preservation'.

    The two policies can be switched by calling or not calling reset()
    in unsetFocus(). Default implementation of unsetFocus() calls
    reset() to fit the simple input methods. The implementation is
    expressed as 'preedit preservation is disabled'.


    \section1 Preedit relocation

    Although the most case of the preedit preservation problem for
    complex input methods is resolved as described above, there is a
    special case. Suppose the case that matches all of the following
    conditions.

    \list

    \i a input focus has been moved from a text widget to another text
    widget directly

    \i the input context is shared between the two text widgets

    \i preedit preservation is enabled for the input context

    \endlist

    In the case, there are the following two requirements that
    contradicts each other. The input context sharing causes it.

    \list

    \i the input context has to be reset to prepare to input to the
    newly focused text widget

    \i the input context has to be preserved until focused back to the
    previous text widget

    \endlist

    A intrinsic feature named 'preedit relocation' is available to
    compromise the requirements. If the feature is enabled for the
    input context, it is simply moved to the new text widget with the
    preedit string. The user continues the input on the new text
    widget, or relocate it to another text widget. The preedit of
    previous text widget is automatically cleared to back to the
    neutral state of the widget.

    This strange behavior is just a compromise. As described in
    previous section, complex input method user should not be exposed
    to the risk losing the input context because it contains valuable
    long text made up with considerable input cost. The user will
    immediately focus back to the previous text widget to continue the
    input in the correct text widget if the preedit relocation
    occurred. The feature is mainly existing as safety.

    The feature properly works even if the focus is moved as
    following. Input method developers are not required to be aware of
    the relocation protocol since TQInputContext transparently handles
    it.

    a text widget -> a non-text widget -> another text widget

    To enable the preedit relocation feature, the input context class
    have to reimplement isPreeditRelocationEnabled() as returns true.
    The implementation requires that the preedit preservation is also
    enabled since preedit relocation is a special case of the preedit
    preservation. If the preedit relocation is disabled, the input
    context is simply reset in the relocation case.


    \section1 Input context instanciation
    \section1 Input method switching

    \section1 Text widget implementor's guide

    Add following code fragment into createPopupMenu() to add input
    method dependent submenus.

    \code
    #ifndef TQT_NO_IM
        TQInputContext *qic = getInputContext();
        if ( qic )
            qic->addMenusTo( popup );
    #endif
    \endcode

    \sa TQInputContextPlugin, TQInputContextFactory, TQApplication::locateICHolderWidget(), TQApplication::defaultInputMethod()
*/


/*!
    Constructs an input context.

    holderWidget is set immediately after this constructor has been
    returned on the X11 platform.
*/
TQInputContext::TQInputContext( TQObject *parent )
    : TQObject( parent )
{
    d = new TQInputContextPrivate;
}


/*!
    Destroys the input context.
*/
TQInputContext::~TQInputContext()
{
    delete d;
}

#if defined(TQ_WS_X11)
/*!
    \internal
    Returns the owner of this input context. Ordinary input methods
    should not call this function directly to keep platform
    independence and flexible configuration possibility.

    The return value may differ from focusWidget() if the input
    context is shared between several text widgets.

    \sa setHolderWidget(), focusWidget()
*/
TQWidget *TQInputContext::holderWidget() const
{
    return d->holderWidget;
}

/*!
    \internal
    Sets the owner of this input context. Ordinary input methods
    must not call this function directly.

    \sa holderWidget()
*/
void TQInputContext::setHolderWidget( TQWidget *w )
{
    d->holderWidget = w;
}

/*!
    \internal
    Returns the widget that has an input focus for this input
    context. Ordinary input methods should not call this function
    directly to keep platform independence and flexible configuration
    possibility.

    The return value may differ from holderWidget() if the input
    context is shared between several text widgets.

    \sa setFocusWidget(), holderWidget()
*/
TQWidget *TQInputContext::focusWidget() const
{
    return d->hasFocus ? d->composingWidget : 0;
}


/*!
    \internal
    Sets the widget that has an input focus for this input
    context. Ordinary input methods must not call this function
    directly.

    \sa focusWidget()
*/
void TQInputContext::setFocusWidget( TQWidget *w )
{
    if ( w ) {
	bool isFocusingBack = ( w == d->composingWidget );
	bool isPreeditRelocation = ( ! isFocusingBack  && isComposing() &&
				     d->composingWidget );
	// invoke sendIMEventInternal() rather than sendIMEvent() to
	// avoid altering the composing state
	if ( isPreeditRelocation ) {
	    // clear preedit of previously focused text
	    // widget. preserved preedit may be exist even if
	    // isPreeditRelocationEnabled() == false.
	    sendIMEventInternal( TQEvent::IMEnd );
	}
	d->composingWidget = w;  // changes recipient of TQIMEvent
	if ( isPreeditRelocation ) {
#if !defined(TQT_NO_IM_PREEDIT_RELOCATION)
	    if ( isPreeditRelocationEnabled() ) {
		// copy preedit state to the widget that gaining focus
		sendIMEventInternal( TQEvent::IMStart );
		sendIMEventInternal( TQEvent::IMCompose, d->preeditString,
				     d->cursorPosition, d->selLength );
	    } else
#endif
	    {
		// reset input context when the shared context has
		// focused on another text widget
		reset();
	    }
	}
    }
    d->hasFocus = w;
}


/*!
    \internal
    This function is called from TQWidget to keep input state
    consistency. Ordinary input method must not call this function
    directly.
*/
void TQInputContext::releaseComposingWidget( TQWidget *w )
{
    if ( d->composingWidget == w ) {
	d->composingWidget = 0;
	d->hasFocus = false;
    }
}
#endif  // TQ_WS_X11

/*!
    \internal
    This function can be reimplemented in a subclass as returning true
    if you want making your input method enable the preedit
    relocation. See the description for preedit relocation of
    TQInputContext.

    /sa TQInputContext
*/
bool TQInputContext::isPreeditRelocationEnabled()
{
    return false;
}

/*!
    This function indicates whether IMStart event had been sent to the
    text widget. It is ensured that an input context can send IMCompose
    or IMEnd event safely if this function returned true.

    The state is automatically being tracked through sendIMEvent().

    \sa sendIMEvent()
*/
bool TQInputContext::isComposing() const
{
    return d->isComposing;
}


/*!
    This function can be reimplemented in a subclass to filter input
    events.

    Return true if the \a event has been consumed. Otherwise, the
    unfiltered \a event will be forwarded to widgets as ordinary
    way. Although the input events have accept() and ignore()
    methods, leave it untouched.

    \a event is currently restricted to TQKeyEvent. But some input
    method related events such as TQWheelEvent or TQTabletEvent may be
    added in future.

    The filtering opportunity is always given to the input context as
    soon as possible. It has to be taken place before any other key
    event consumers such as eventfilters and accelerators because some
    input methods require quite various key combination and
    sequences. It often conflicts with accelerators and so on, so we
    must give the input context the filtering opportunity first to
    ensure all input methods work properly regardless of application
    design.

    Ordinary input methods require discrete key events to work
    properly, so TQt's key compression is always disabled for any input
    contexts.

    \sa TQKeyEvent, x11FilterEvent()
*/
bool TQInputContext::filterEvent( const TQEvent *event )
{
    Q_UNUSED(event);
    return false;
}


/*!
    \fn void TQInputContext::deletionRequested()

    Emit this signal when a fatal error has been caused in the input
    context. The input context will be deleted by the owner which is
    usually the holder widget.
*/

/*!
    \fn void TQInputContext::imEventGenerated( TQObject *receiver, TQIMEvent *e )

    \internal
    This signal is emitted when the user has sent a TQIMEvent through
    sendIMEvent(). Ordinary input methods should not emit this signal
    directly.

    \a receiver is a platform dependent destination of the \a e.

    \sa TQIMEvent, sendIMEvent(), sendIMEventInternal(), 
*/

/*!
    \internal
    Sends a TQIMEvent to the client via imEventGenerated()
    signal. Ordinary input method should not call this function
    directly.

    \sa TQIMEvent, TQIMComposeEvent, sendIMEvent(), imEventGenerated()
*/
void TQInputContext::sendIMEventInternal( TQEvent::Type type,
					 const TQString &text,
					 int cursorPosition, int selLength )
{
    TQObject *receiver = 0;
    TQIMEvent *event = 0;

#if defined(TQ_WS_X11)
    receiver = d->composingWidget;
#elif defined(TQ_WS_QWS)
    // just a placeholder
#endif
    if ( ! receiver )
	return;

    if ( type == TQEvent::IMStart ) {
	tqDebug( "sending IMStart with %d chars to %p",
		text.length(), receiver );
	event = new TQIMEvent( type, text, cursorPosition );
    } else if ( type == TQEvent::IMEnd ) {
	tqDebug( "sending IMEnd with %d chars to %p, text=%s",
		text.length(), receiver, (const char*)text.local8Bit() );
	event = new TQIMEvent( type, text, cursorPosition );
    } else if ( type == TQEvent::IMCompose ) {
	tqDebug( "sending IMCompose to %p with %d chars, cpos=%d, sellen=%d, text=%s",
		receiver, text.length(), cursorPosition, selLength,
		(const char*)text.local8Bit() );
	event = new TQIMComposeEvent( type, text, cursorPosition, selLength );
    }

    if ( event )
        emit imEventGenerated( receiver, event );
}


/*!
    Call this function to send TQIMEvent to the text widget. This
    function constructs a TQIMEvent based on the arguments and send it
    to the appropriate widget. Ordinary input method should not
    reimplement this function.

    \a type is either \c TQEvent::IMStart or \c TQEvent::IMCompose or \c
    TQEvent::IMEnd. You have to send a \c TQEvent::IMStart to start
    composing, then send several \c TQEvent::IMCompose to update the
    preedit of the widget, and finalize the composition with sending
    \c TQEvent::IMEnd.

    \c TQEvent::IMStart should always be sent without arguments as:
    \code
    sendIMEvent( TQEvent::IMStart )
    \endcode

    And \c TQEvent::IMCompose can be sent without cursor:
    \code
    sendIMEvent( TQEvent::IMCompose, TQString( "a text" ) )
    \endcode

    Or optionally with cursor with \a cursorPosition:
    \code
    sendIMEvent( TQEvent::IMCompose, TQString( "a text with cursor" ), 12 )
    \endcode
    Note that \a cursorPosition also specifies microfocus position.

    Or optionally with selection text:
    \code
    sendIMEvent( TQEvent::IMCompose, TQString( "a text with selection" ), 12, 9 )
    \endcode
    \a cursorPosition and \a selLength must be within the \a text. The
    \a cursorPosition also specifies microfocus position in the case:

    \c TQEvent::IMEnd can be sent without arguments to terminate the
    composition with null string:
    \code
    sendIMEvent( TQEvent::IMEnd )
    \endcode

    Or optionally accepts \a text to commit a string:
    \code
    sendIMEvent( TQEvent::IMEnd, TQString( "a text" ) )
    \endcode

    \sa TQIMEvent, TQIMComposeEvent, setMicroFocus()
*/
void TQInputContext::sendIMEvent( TQEvent::Type type, const TQString &text,
                                 int cursorPosition, int selLength )
{
#if defined(TQ_WS_X11)
    if ( !focusWidget() )
	return;
#endif

    if ( type == TQEvent::IMStart ) {
	sendIMEventInternal( type, text, cursorPosition, selLength );
	d->isComposing = true;
    } else if ( type == TQEvent::IMEnd ) {
	d->resetComposingState();
	sendIMEventInternal( type, text, cursorPosition, selLength );
    } else if ( type == TQEvent::IMCompose ) {
	d->updateComposingState( text, cursorPosition, selLength );
	sendIMEventInternal( type, text, cursorPosition, selLength );
    }
}


/*!
    This function can be reimplemented in a subclass to detect
    that the input context has been focused on.

    The input context will receive input events through
    x11FilterEvent() and filterEvent() after setFocus() until
    unsetFocus() has been called.

    an input context is ensured that setFocus() is called exactly once
    until unsetFocus() has been called even if preedit relocation has
    occurred. This means that an input focus will survive between
    several widgets that sharing the input context.

    On the X11 platform, focusWidget is already set before this
    function has been called.

    \sa unsetFocus()
*/
void TQInputContext::setFocus()
{
}


/*!
    This function can be reimplemented in a subclass to detect
    that the input context has lost the focus.

    an input context is ensured that unsetFocus() is not called during
    preedit relocation. This means that an input focus will survive
    between several widgets that sharing the input context.

    Default implementation that calls reset() is sufficient for simple
    input methods. You can override this function to alter the
    behavior. For example, most Japanese input contexts should not be
    reset on losing focus. The context sometimes contains a whole
    paragraph and has minutes of lifetime different to ephemeral one
    in other languages. The piled input context should be survived
    until focused again since Japanese user naturally expects so.

    On the X11 platform, focusWidget is valid until this function has
    been returned.

    \sa setFocus()
*/
void TQInputContext::unsetFocus()
{
    reset();
}


/*!
    This function can be implemented in a subclass to handle
    microfocus changes.

    'microfocus' stands for the input method focus point in the
    preedit (XIM "spot" point) for complex language input handling. It
    can be used to place auxiliary GUI widgets such as candidate
    selection window.

    \a x, \a y, \a w and \a h represents the position and size of the
    cursor in the preedit string. \a f is the font on the location of
    the cursor.
*/
void TQInputContext::setMicroFocus( int x, int y, int w, int h, TQFont *f )
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(w);
    Q_UNUSED(h);
    Q_UNUSED(f);
}


/*!
    This function can be reimplemented in a subclass to handle mouse
    presses/releases/doubleclicks/moves within the preedit text. You
    can use the function to implement mouse-oriented user interface
    such as text selection or popup menu for candidate selection.

    The parameter \a x is the offset within the string that was sent
    with the IMCompose event. The alteration boundary of \a x is
    ensured as character boundary of preedit string accurately.

    \a type is either \c TQEvent::MouseButtonPress or \c
    TQEvent::MouseButtonRelease or \c TQEvent::MouseButtonDblClick or \c
    TQEvent::MouseButtonMove. Refer \a button and \a state to determine
    what operation has performed.

    The method interface is imported from
    TQWSInputMethod::mouseHandler() of TQt/Embedded 2.3.7 and extended
    for desktop system.
 */
void TQInputContext::mouseHandler( int x, TQEvent::Type type,
				  TQt::ButtonState button,
				  TQt::ButtonState state )
{
    Q_UNUSED(x);
    Q_UNUSED(button);
    Q_UNUSED(state);
    // Default behavior for simple ephemeral input contexts. Some
    // complex input contexts should not be reset here.
    if ( type == TQEvent::MouseButtonPress ||
	 type == TQEvent::MouseButtonDblClick )
	reset();
}


/*!
    Returns the font of the current input widget
 */
TQFont TQInputContext::font() const
{
    if ( !focusWidget() )
        return TQApplication::font(); //### absolutely last resort

    return focusWidget()->font();
}


/*!
    This function can be reimplemented in a subclass to reset the
    state of the input method.

    This function is called by several widgets to reset input
    state. For example, a text widget call this function before
    inserting a text to make widget ready to accept a text.

    Default implementation is sufficient for simple input method. You
    can override this function to reset external input method engines
    in complex input method. In the case, call TQInputContext::reset()
    to ensure proper termination of inputting.

    You must not send any TQIMEvent except empty IMEnd event using
    TQInputContext::reset() at reimplemented reset(). It will break
    input state consistency.
*/
void TQInputContext::reset()
{
    if ( isComposing() )
        sendIMEvent( TQEvent::IMEnd );
}


/*!
    This function must be implemented in any subclasses to return the
    identifier name of the input method.

    Return value is the name to identify and specify input methods for
    the input method switching mechanism and so on. The name has to be
    consistent with TQInputContextPlugin::keys(). The name has to
    consist of ASCII characters only.

    There are two different names with different responsibility in the
    input method domain. This function returns one of them. Another
    name is called 'display name' that stands for the name for
    endusers appeared in a menu and so on.

    \sa TQInputContextPlugin::keys(), TQInputContextPlugin::displayName()
*/
TQString TQInputContext::identifierName()
{
    return "";
}


/*!
    This function must be implemented in any subclasses to return a
    language code (e.g. "zh_CN", "zh_TW", "zh_HK", "ja", "ko", ...)
    of the input context. If the input context can handle multiple
    languages, return the currently used one. The name has to be
    consistent with TQInputContextPlugin::language().

    This information will be used by language tagging feature in
    TQIMEvent. It is required to distinguish unified han characters
    correctly. It enables proper font and character code
    handling. Suppose CJK-awared multilingual web browser
    (that automatically modifies fonts in CJK-mixed text) and XML editor
    (that automatically inserts lang attr).

    \sa TQInputContextPlugin::language()
*/
TQString TQInputContext::language()
{
    return "";
}


#if (TQT_VERSION-0 >= 0x040000)
/*!
    This is a preliminary interface for TQt4
 */
TQPtrList<TQAction *> TQInputContext::actions()
{
}
#else
/*!
    This function can be reimplemented in a subclass to provide input
    method dependent popup menus. Return 0 if the menus are
    unnecessary.

    Ownership of the object and children are transferred to the
    caller, and the result must not be called
    setAutoDelete(). TQInputContextMenu::title is used for label text
    of the popup menu as submenu.

    \sa addMenusTo()
*/
TQPtrList<TQInputContextMenu> *TQInputContext::menus()
{
    return 0;
}
#endif

/*!
    Appends input method dependent submenus into \a popup. A separator
    is also inserted into \a popup if \a action is InsertSeparator.

    This is an utility function only for convenience in limited
    situation. This function is used by input context owner such as
    text widgets to add the submenus to its own context menu. If you
    want to insert the submenus in more flexible way, use
    TQInputContext::menus() manually. \a popup is not restricted to
    context menu of a text widget. For example, the owner may be a
    input method menu of TQtopia taskbar in TQt/Embedded platform.

    \sa menus(), TQInputContextMenu::Action
*/
void TQInputContext::addMenusTo( TQPopupMenu *popup, TQInputContextMenu::Action action )
{
    if ( ! popup )
	return;

    TQPtrList<TQInputContextMenu> *imMenus = menus();
    if ( imMenus ) {
	if ( action == TQInputContextMenu::InsertSeparator )
	    popup->insertSeparator();
	for ( TQPtrList<TQInputContextMenu>::Iterator it = imMenus->begin();
	      it != imMenus->end();
	      ++it ) {
	    TQInputContextMenu *imMenu = *it;
	    popup->insertItem( imMenu->title, imMenu->popup );
	}
	imMenus->clear();
	delete imMenus;
    }
}

#endif //Q_NO_IM
