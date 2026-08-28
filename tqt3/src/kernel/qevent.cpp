/****************************************************************************
**
** Implementation of event classes
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

#include "ntqevent.h"
#include "ntqcursor.h"
#include "ntqapplication.h"


/*!
    \class TQEvent ntqevent.h
    \brief The TQEvent class is the base class of all
    event classes. Event objects contain event parameters.

    \ingroup events
    \ingroup environment

    TQt's main event loop (TQApplication::exec()) fetches native window
    system events from the event queue, translates them into TQEvents
    and sends the translated events to TQObjects.

    In general, events come from the underlying window system
    (spontaneous() returns true) but it is also possible to manually
    send events using TQApplication::sendEvent() and
    TQApplication::postEvent() (spontaneous() returns false).

    TQObjects receive events by having their TQObject::event() function
    called. The function can be reimplemented in subclasses to
    customize event handling and add additional event types;
    TQWidget::event() is a notable example. By default, events are
    dispatched to event handlers like TQObject::timerEvent() and
    TQWidget::mouseMoveEvent(). TQObject::installEventFilter() allows an
    object to intercept events destined for another object.

    The basic TQEvent contains only an event type parameter.
    Subclasses of TQEvent contain additional parameters that describe
    the particular event.

    \sa TQObject::event() TQObject::installEventFilter()
    TQWidget::event() TQApplication::sendEvent()
    TQApplication::postEvent() TQApplication::processEvents()
*/


/*!
    \enum TQt::ButtonState

    This enum type describes the state of the mouse and the modifier
    buttons.

    \value NoButton  used when the button state does not refer to any
    button (see TQMouseEvent::button()).
    \value LeftButton  set if the left button is pressed, or if this
    event refers to the left button. (The left button may be
    the right button on left-handed mice.)
    \value RightButton  the right button.
    \value MidButton  the middle button.
    \value HistoryBackButton  history navigation back button.
    \value HistoryForwardButton  history navigation forward button.
    \value ShiftButton  a Shift key on the keyboard is also pressed.
    \value ControlButton  a Ctrl key on the keyboard is also pressed.
    \value AltButton  an Alt key on the keyboard is also pressed.
    \value MetaButton a Meta key on the keyboard is also pressed.
    \value Keypad  a keypad button is pressed.
    \value KeyButtonMask a mask for ShiftButton, ControlButton,
    AltButton and MetaButton.
    \value MouseButtonMask a mask for LeftButton, RightButton, MidButton,
    HistoryBackButton and HistoryForwardButton.
*/

/*!
    \enum TQEvent::Type

    This enum type defines the valid event types in TQt. The event
    types and the specialized classes for each type are these:

    \value None  Not an event.
    \value Accessibility  Accessibility information is requested
    \value Timer  Regular timer events, \l{TQTimerEvent}.
    \value MouseButtonPress  Mouse press, \l{TQMouseEvent}.
    \value MouseButtonRelease  Mouse release, \l{TQMouseEvent}.
    \value MouseButtonDblClick  Mouse press again, \l{TQMouseEvent}.
    \value MouseMove  Mouse move, \l{TQMouseEvent}.
    \value KeyPress  Key press (including Shift, for example), \l{TQKeyEvent}.
    \value KeyRelease  Key release, \l{TQKeyEvent}.
    \value IMStart  The start of input method composition, \l{TQIMEvent}.
    \value IMCompose  Input method composition is taking place, \l{TQIMEvent}.
    \value IMEnd  The end of input method composition, \l{TQIMEvent}.
    \value FocusIn  Widget gains keyboard focus, \l{TQFocusEvent}.
    \value FocusOut  Widget loses keyboard focus, \l{TQFocusEvent}.
    \value Enter  Mouse enters widget's boundaries.
    \value Leave  Mouse leaves widget's boundaries.
    \value Paint  Screen update necessary, \l{TQPaintEvent}.
    \value Move  Widget's position changed, \l{TQMoveEvent}.
    \value Resize  Widget's size changed, \l{TQResizeEvent}.
    \value Show  Widget was shown on screen, \l{TQShowEvent}.
    \value Hide  Widget was hidden, \l{TQHideEvent}.
    \value ShowToParent  A child widget has been shown.
    \value HideToParent  A child widget has been hidden.
    \value Close  Widget was closed (permanently), \l{TQCloseEvent}.
    \value ShowNormal  Widget should be shown normally (obsolete).
    \value ShowMaximized  Widget should be shown maximized (obsolete).
    \value ShowMinimized  Widget should be shown minimized (obsolete).
    \value ShowFullScreen  Widget should be shown full-screen (obsolete).
    \value ShowWindowRequest  Widget's window should be shown (obsolete).
    \value DeferredDelete  The object will be deleted after it has
    cleaned up.
    \value Accel  Key press in child for shortcut key handling, \l{TQKeyEvent}.
    \value Wheel  Mouse wheel rolled, \l{TQWheelEvent}.
    \value ContextMenu  Context popup menu, \l{TQContextMenuEvent}
    \value AccelOverride  Key press in child, for overriding shortcut key handling, \l{TQKeyEvent}.
    \value AccelAvailable internal.
    \value WindowActivate  Window was activated.
    \value WindowDeactivate  Window was deactivated.
    \value CaptionChange  Widget's caption changed.
    \value IconChange  Widget's icon changed.
    \value ParentFontChange  Font of the parent widget changed.
    \value ApplicationFontChange  Default application font changed.
    \value PaletteChange  Palette of the widget changed.
    \value ParentPaletteChange  Palette of the parent widget changed.
    \value ApplicationPaletteChange  Default application palette changed.
    \value Clipboard  Clipboard contents have changed.
    \value SockAct  Socket activated, used to implement \l{TQSocketNotifier}.
    \value DragEnter  A drag-and-drop enters widget, \l{TQDragEnterEvent}.
    \value DragMove  A drag-and-drop is in progress, \l{TQDragMoveEvent}.
    \value DragLeave  A drag-and-drop leaves widget, \l{TQDragLeaveEvent}.
    \value Drop  A drag-and-drop is completed, \l{TQDropEvent}.
    \value DragResponse  Internal event used by TQt on some platforms.
    \value ChildInserted  Object gets a child, \l{TQChildEvent}.
    \value ChildRemoved  Object loses a child, \l{TQChildEvent}.
    \value LayoutHint  Widget child has changed layout properties.
    \value ActivateControl  Internal event used by TQt on some platforms.
    \value DeactivateControl  Internal event used by TQt on some platforms.
    \value LanguageChange  The application translation changed, \l{TQTranslator}
    \value LayoutDirectionChange  The direction of layouts changed
    \value LocaleChange  The system locale changed
    \value Quit  Reserved.
    \value Create  Reserved.
    \value Destroy  Reserved.
    \value Reparent  Reserved.
    \value Speech  Reserved for speech input.
    \value TabletMove  A Wacom Tablet Move Event.
    \value Style  Internal use only
    \value TabletPress  A Wacom Tablet Press Event
    \value TabletRelease  A Wacom Tablet Release Event
    \value OkRequest  Internal event used by TQt on some platforms.
    \value HelpRequest  Internal event used by TQt on some platforms.
    \value IconDrag     Internal event used by TQt on some platforms when proxy icon is dragged.
    \value WindowStateChange The window's state, i.e. minimized,
    maximized or full-screen, has changed. See \l{TQWidget::windowState()}.
    \value WindowBlocked The window is modally blocked
    \value WindowUnblocked The window leaves modal blocking

    \value User  User defined event.
    \value MaxUser  Last user event id.

    User events should have values between User and MaxUser inclusive.
*/
/*!
    \fn TQEvent::TQEvent( Type type )

    Contructs an event object of type \a type.
*/

/*!
    \fn TQEvent::Type TQEvent::type() const

    Returns the event type.
*/

/*!
    \fn bool TQEvent::spontaneous() const

    Returns true if the event originated outside the application, i.e.
    it is a system event; otherwise returns false.
*/


/*!
    \class TQTimerEvent ntqevent.h
    \brief The TQTimerEvent class contains parameters that describe a
    timer event.

    \ingroup events

    Timer events are sent at regular intervals to objects that have
    started one or more timers. Each timer has a unique identifier. A
    timer is started with TQObject::startTimer().

    The TQTimer class provides a high-level programming interface that
    uses signals instead of events. It also provides one-shot timers.

    The event handler TQObject::timerEvent() receives timer events.

    \sa TQTimer, TQObject::timerEvent(), TQObject::startTimer(),
    TQObject::killTimer(), TQObject::killTimers()
*/

/*!
    \fn TQTimerEvent::TQTimerEvent( int timerId )

    Constructs a timer event object with the timer identifier set to
    \a timerId.
*/

/*!
    \fn int TQTimerEvent::timerId() const

    Returns the unique timer identifier, which is the same identifier
    as returned from TQObject::startTimer().
*/


/*!
    \class TQMouseEvent ntqevent.h
    \ingroup events

    \brief The TQMouseEvent class contains parameters that describe a mouse event.

    Mouse events occur when a mouse button is pressed or released
    inside a widget or when the mouse cursor is moved.

    Mouse move events will occur only when a mouse button is pressed
    down, unless mouse tracking has been enabled with
    TQWidget::setMouseTracking().

    TQt automatically grabs the mouse when a mouse button is pressed
    inside a widget; the widget will continue to receive mouse events
    until the last mouse button is released.

    A mouse event contains a special accept flag that indicates
    whether the receiver wants the event. You should call
    TQMouseEvent::ignore() if the mouse event is not handled by your
    widget. A mouse event is propagated up the parent widget chain
    until a widget accepts it with TQMouseEvent::accept() or an event
    filter consumes it.

    The functions pos(), x() and y() give the cursor position relative
    to the widget that receives the mouse event. If you move the
    widget as a result of the mouse event, use the global position
    returned by globalPos() to avoid a shaking motion.

    The TQWidget::setEnabled() function can be used to enable or
    disable mouse and keyboard events for a widget.

    The event handlers TQWidget::mousePressEvent(),
    TQWidget::mouseReleaseEvent(), TQWidget::mouseDoubleClickEvent() and
    TQWidget::mouseMoveEvent() receive mouse events.

    \sa TQWidget::setMouseTracking(), TQWidget::grabMouse(),
    TQCursor::pos()
*/

/*!
    \fn TQMouseEvent::TQMouseEvent( Type type, const TQPoint &pos, int button, int state )

    Constructs a mouse event object.

    The \a type parameter must be one of \c TQEvent::MouseButtonPress,
    \c TQEvent::MouseButtonRelease, \c TQEvent::MouseButtonDblClick or
    \c TQEvent::MouseMove.

    The \a pos parameter specifies the position relative to the
    receiving widget. \a button specifies the \link TQt::ButtonState
    button\endlink that caused the event, which should be \c
    TQt::NoButton (0), if \a type is \c MouseMove. \a state is the
    \link TQt::ButtonState ButtonState\endlink at the time of the
    event.

    The globalPos() is initialized to TQCursor::pos(), which may not be
    appropriate. Use the other constructor to specify the global
    position explicitly.
*/

TQMouseEvent::TQMouseEvent( Type type, const TQPoint &pos, int button, int state )
    : TQEvent(type), p(pos), b(button),s((ushort)state), accpt(true){
	g = TQCursor::pos();
}


/*!
    \fn TQMouseEvent::TQMouseEvent( Type type, const TQPoint &pos, const TQPoint &globalPos,  int button, int state )

    Constructs a mouse event object.

    The \a type parameter must be \c TQEvent::MouseButtonPress, \c
    TQEvent::MouseButtonRelease, \c TQEvent::MouseButtonDblClick or \c
    TQEvent::MouseMove.

    The \a pos parameter specifies the position relative to the
    receiving widget. \a globalPos is the position in absolute
    coordinates. \a button specifies the \link TQt::ButtonState
    button\endlink that caused the event, which should be \c
    TQt::NoButton (0), if \a type is \c MouseMove. \a state is the
    \link TQt::ButtonState ButtonState\endlink at the time of the
    event.

*/

/*!
    \fn const TQPoint &TQMouseEvent::pos() const

    Returns the position of the mouse pointer relative to the widget
    that received the event.

    If you move the widget as a result of the mouse event, use the
    global position returned by globalPos() to avoid a shaking motion.

    \sa x(), y(), globalPos()
*/

/*!
    \fn const TQPoint &TQMouseEvent::globalPos() const

    Returns the global position of the mouse pointer \e{at the time
    of the event}. This is important on asynchronous window systems
    like X11. Whenever you move your widgets around in response to
    mouse events, globalPos() may differ a lot from the current
    pointer position TQCursor::pos(), and from TQWidget::mapToGlobal(
    pos() ).

    \sa globalX(), globalY()
*/

/*!
    \fn int TQMouseEvent::x() const

    Returns the x-position of the mouse pointer, relative to the
    widget that received the event.

    \sa y(), pos()
*/

/*!
    \fn int TQMouseEvent::y() const

    Returns the y-position of the mouse pointer, relative to the
    widget that received the event.

    \sa x(), pos()
*/

/*!
    \fn int TQMouseEvent::globalX() const

    Returns the global x-position of the mouse pointer at the time of
    the event.

    \sa globalY(), globalPos()
*/

/*!
    \fn int TQMouseEvent::globalY() const

    Returns the global y-position of the mouse pointer at the time of
    the event.

    \sa globalX(), globalPos()
*/

/*!
    \fn ButtonState TQMouseEvent::button() const

    Returns the button that caused the event.

    Possible return values are \c LeftButton, \c RightButton, \c
    MidButton and \c NoButton.

    Note that the returned value is always \c NoButton for mouse move
    events.

    \sa state() TQt::ButtonState
*/


/*!
    \fn ButtonState TQMouseEvent::state() const

    Returns the button state (a combination of mouse buttons and
    keyboard modifiers), i.e. what buttons and keys were being pressed
    immediately before the event was generated.

    This means that if you have a \c TQEvent::MouseButtonPress or a \c
    TQEvent::MouseButtonDblClick state() will \e not include the mouse
    button that's pressed. But once the mouse button has been
    released, the \c TQEvent::MouseButtonRelease event will have the
    button() that was pressed.

    This value is mainly interesting for \c TQEvent::MouseMove; for the
    other cases, button() is more useful.

    The returned value is \c LeftButton, \c RightButton, \c MidButton,
    \c ShiftButton, \c ControlButton and \c AltButton OR'ed together.

    \sa button() stateAfter() TQt::ButtonState
*/

/*!
    \fn ButtonState TQMouseEvent::stateAfter() const

    Returns the state of buttons after the event.

    \sa state() TQt::ButtonState
*/
TQt::ButtonState TQMouseEvent::stateAfter() const
{
    return TQt::ButtonState(state()^button());
}



/*!
    \fn bool TQMouseEvent::isAccepted() const

    Returns true if the receiver of the event wants to keep the key;
    otherwise returns false.
*/

/*!
    \fn void TQMouseEvent::accept()

    Sets the accept flag of the mouse event object.

    Setting the accept parameter indicates that the receiver of the
    event wants the mouse event. Unwanted mouse events are sent to the
    parent widget.

    The accept flag is set by default.

    \sa ignore()
*/


/*!
    \fn void TQMouseEvent::ignore()

    Clears the accept flag parameter of the mouse event object.

    Clearing the accept parameter indicates that the event receiver
    does not want the mouse event. Unwanted mouse events are sent to
    the parent widget.

    The accept flag is set by default.

    \sa accept()
*/


/*!
    \class TQWheelEvent ntqevent.h
    \brief The TQWheelEvent class contains parameters that describe a wheel event.

    \ingroup events

    Wheel events are sent to the widget under the mouse, and if that widget
    does not handle the event they are sent to the focus widget. The rotation
    distance is provided by delta(). The functions pos() and globalPos() return
    the mouse pointer location at the time of the event.

    A wheel event contains a special accept flag that indicates
    whether the receiver wants the event. You should call
    TQWheelEvent::accept() if you handle the wheel event; otherwise it
    will be sent to the parent widget.

    The TQWidget::setEnable() function can be used to enable or disable
    mouse and keyboard events for a widget.

    The event handler TQWidget::wheelEvent() receives wheel events.

    \sa TQMouseEvent, TQWidget::grabMouse()
*/

/*!
    \fn Orientation TQWheelEvent::orientation() const

    Returns the wheel's orientation.
*/

/*!
    \fn TQWheelEvent::TQWheelEvent( const TQPoint &pos, int delta, int state, Orientation orient = Vertical );

    Constructs a wheel event object.

    The globalPos() is initialized to TQCursor::pos(), i.e. \a pos,
    which is usually (but not always) right. Use the other constructor
    if you need to specify the global position explicitly. \a delta
    contains the rotation distance, \a state holds the keyboard
    modifier flags at the time of the event and \a orient holds the
    wheel's orientation.

    \sa pos(), delta(), state()
*/
#ifndef TQT_NO_WHEELEVENT
TQWheelEvent::TQWheelEvent( const TQPoint &pos, int delta, int state, Orientation orient )
    : TQEvent(Wheel), p(pos), d(delta), s((ushort)state),
      accpt(true), o(orient)
{
    g = TQCursor::pos();
}
#endif
/*!
    \fn TQWheelEvent::TQWheelEvent( const TQPoint &pos, const TQPoint& globalPos, int delta, int state, Orientation orient = Vertical  )

    Constructs a wheel event object. The position when the event
    occurred is given in \a pos and \a globalPos. \a delta contains
    the rotation distance, \a state holds the keyboard modifier flags
    at the time of the event and \a orient holds the wheel's
    orientation.

    \sa pos(), globalPos(), delta(), state()
*/

/*!
    \fn int TQWheelEvent::delta() const

    Returns the distance that the wheel is rotated expressed in
    multiples or divisions of the \e{wheel delta}, which is currently
    defined to be 120. A positive value indicates that the wheel was
    rotated forwards away from the user; a negative value indicates
    that the wheel was rotated backwards toward the user.

    The \e{wheel delta} constant was defined to be 120 by wheel mouse
    vendors to allow building finer-resolution wheels in the future,
    including perhaps a freely rotating wheel with no notches. The
    expectation is that such a device would send more messages per
    rotation but with a smaller value in each message.
*/

/*!
    \fn const TQPoint &TQWheelEvent::pos() const

    Returns the position of the mouse pointer, relative to the widget
    that received the event.

    If you move your widgets around in response to mouse
    events, use globalPos() instead of this function.

    \sa x(), y(), globalPos()
*/

/*!
    \fn int TQWheelEvent::x() const

    Returns the x-position of the mouse pointer, relative to the
    widget that received the event.

    \sa y(), pos()
*/

/*!
    \fn int TQWheelEvent::y() const

    Returns the y-position of the mouse pointer, relative to the
    widget that received the event.

    \sa x(), pos()
*/


/*!
    \fn const TQPoint &TQWheelEvent::globalPos() const

    Returns the global position of the mouse pointer \e{at the time
    of the event}. This is important on asynchronous window systems
    such as X11; whenever you move your widgets around in response to
    mouse events, globalPos() can differ a lot from the current
    pointer position TQCursor::pos().

    \sa globalX(), globalY()
*/

/*!
    \fn int TQWheelEvent::globalX() const

    Returns the global x-position of the mouse pointer at the time of
    the event.

    \sa globalY(), globalPos()
*/

/*!
    \fn int TQWheelEvent::globalY() const

    Returns the global y-position of the mouse pointer at the time of
    the event.

    \sa globalX(), globalPos()
*/


/*!
    \fn ButtonState TQWheelEvent::state() const

    Returns the keyboard modifier flags of the event.

    The returned value is \c ShiftButton, \c ControlButton, and \c
    AltButton OR'ed together.
*/

/*!
    \fn bool TQWheelEvent::isAccepted() const

    Returns true if the receiver of the event handles the wheel event;
    otherwise returns false.
*/

/*!
    \fn void TQWheelEvent::accept()

    Sets the accept flag of the wheel event object.

    Setting the accept parameter indicates that the receiver of the
    event wants the wheel event. Unwanted wheel events are sent to the
    parent widget.

    The accept flag is set by default.

    \sa ignore()
*/

/*!
    \fn void TQWheelEvent::ignore()

    Clears the accept flag parameter of the wheel event object.

    Clearing the accept parameter indicates that the event receiver
    does not want the wheel event. Unwanted wheel events are sent to
    the parent widget. The accept flag is set by default.

    \sa accept()
*/


/*!
    \enum TQt::Modifier

    This enum type describes the keyboard modifier keys supported by
    TQt.

    \value SHIFT the Shift keys provided on all standard keyboards.
    \value META the Meta keys.
    \value CTRL the Ctrl keys.
    \value ALT the normal Alt keys, but not e.g. AltGr.
    \value MODIFIER_MASK is a mask of Shift, Ctrl, Alt and Meta.
    \value UNICODE_ACCEL the accelerator is specified as a Unicode code
    point, not as a TQt Key.
*/

/*!
    \class TQKeyEvent ntqevent.h
    \brief The TQKeyEvent class contains describes a key event.

    \ingroup events

    Key events occur when a key is pressed or released when a widget
    has keyboard input focus.

  A key event contains a special accept flag that indicates whether the
  receiver wants the key event.  You should call TQKeyEvent::ignore() if the
  key press or release event is not handled by your widget. A key event is
  propagated up the parent widget chain until a widget accepts it with
  TQKeyEvent::accept() or an event filter consumes it.
  Key events for multi media keys are ignored by default. You should call
  TQKeyEvent::accept() if your widget handles those events.

    The TQWidget::setEnable() function can be used to enable or disable
    mouse and keyboard events for a widget.

    The event handlers TQWidget::keyPressEvent() and
    TQWidget::keyReleaseEvent() receive key events.

    \sa TQFocusEvent, TQWidget::grabKeyboard()
*/

/*!
    \fn TQKeyEvent::TQKeyEvent( Type type, int key, int ascii, int state,
			      const TQString& text, bool autorep, ushort count )

    Constructs a key event object.

    The \a type parameter must be \c TQEvent::KeyPress or \c
    TQEvent::KeyRelease. If \a key is 0 the event is not a result of a
    known key (e.g. it may be the result of a compose sequence or
    keyboard macro). \a ascii is the ASCII code of the key that was
    pressed or released. \a state holds the keyboard modifiers. \a
    text is the Unicode text that the key generated. If \a autorep is
    true, isAutoRepeat() will be true. \a count is the number of
    single keys.

    The accept flag is set to true.
*/

/*!
    \fn int TQKeyEvent::key() const

    Returns the code of the key that was pressed or released.

    See \l TQt::Key for the list of keyboard codes. These codes are
    independent of the underlying window system.

    A value of either 0 or Key_unknown means that the event is not
    the result of a known key (e.g. it may be the result of a compose
    sequence or a keyboard macro, or due to key event compression).

    Applications should not use the TQt latin 1 keycodes between 128
    and 255, but should rather use the TQKeyEvent::text(). This is
    mainly for compatibility.

    \sa TQWidget::setKeyCompression()
*/

/*!
    \fn int TQKeyEvent::ascii() const

    Returns the ASCII code of the key that was pressed or released. We
    recommend using text() instead.

    \sa text()
*/

/*!
    \fn TQString TQKeyEvent::text() const

    Returns the Unicode text that this key generated. The text returned
    migth be empty, which is the case when pressing or
    releasing modifying keys as Shift, Control, Alt and Meta. In these
    cases key() will contain a valid value.

    \sa TQWidget::setKeyCompression()
*/

/*!
    \fn ButtonState TQKeyEvent::state() const

    Returns the keyboard modifier flags that existed immediately
    before the event occurred.

    The returned value is \c ShiftButton, \c ControlButton, \c AltButton
    and \c MetaButton OR'ed together.

    \sa stateAfter()
*/

/*!
    \fn ButtonState TQKeyEvent::stateAfter() const

    Returns the keyboard modifier flags that existed immediately after
    the event occurred.

    \warning This function cannot be trusted.

    \sa state()
*/
//###### We must check with XGetModifierMapping
TQt::ButtonState TQKeyEvent::stateAfter() const
{
    if ( key() == Key_Shift )
	return TQt::ButtonState(state()^ShiftButton);
    if ( key() == Key_Control )
	return TQt::ButtonState(state()^ControlButton);
    if ( key() == Key_Alt )
	return TQt::ButtonState(state()^AltButton);
    if ( key() == Key_Meta )
	return TQt::ButtonState(state()^MetaButton);
    return state();
}

/*!
    \fn bool TQKeyEvent::isAccepted() const

    Returns true if the receiver of the event wants to keep the key;
    otherwise returns false
*/

/*!
    \fn void TQKeyEvent::accept()

    Sets the accept flag of the key event object.

    Setting the accept parameter indicates that the receiver of the
    event wants the key event. Unwanted key events are sent to the
    parent widget.

    The accept flag is set by default.

    \sa ignore()
*/

/*!
    \fn bool TQKeyEvent::isAutoRepeat() const

    Returns true if this event comes from an auto-repeating key and
    false if it comes from an initial key press.

    Note that if the event is a multiple-key compressed event that is
    partly due to auto-repeat, this function could return either true
    or false indeterminately.
*/

/*!
    \fn int TQKeyEvent::count() const

    Returns the number of single keys for this event. If text() is not
    empty, this is simply the length of the string.

    \sa TQWidget::setKeyCompression()
*/

/*!
    \fn void TQKeyEvent::ignore()

    Clears the accept flag parameter of the key event object.

    Clearing the accept parameter indicates that the event receiver
    does not want the key event. Unwanted key events are sent to the
    parent widget.

    The accept flag is set by default.

    \sa accept()
*/

/*!
    \enum TQt::Key

    The key names used by TQt.

    \value Key_Escape
    \value Key_Tab
    \value Key_Backtab
    \value Key_Backspace
    \value Key_Return
    \value Key_Enter
    \value Key_Insert
    \value Key_Delete
    \value Key_Pause
    \value Key_Print
    \value Key_SysReq
    \value Key_Home
    \value Key_End
    \value Key_Left
    \value Key_Up
    \value Key_Right
    \value Key_Down
    \value Key_Prior
    \value Key_Next
    \value Key_Shift
    \value Key_Control
    \value Key_Meta
    \value Key_Alt
    \value Key_CapsLock
    \value Key_NumLock
    \value Key_ScrollLock
    \value Key_Clear
    \value Key_F1
    \value Key_F2
    \value Key_F3
    \value Key_F4
    \value Key_F5
    \value Key_F6
    \value Key_F7
    \value Key_F8
    \value Key_F9
    \value Key_F10
    \value Key_F11
    \value Key_F12
    \value Key_F13
    \value Key_F14
    \value Key_F15
    \value Key_F16
    \value Key_F17
    \value Key_F18
    \value Key_F19
    \value Key_F20
    \value Key_F21
    \value Key_F22
    \value Key_F23
    \value Key_F24
    \value Key_F25
    \value Key_F26
    \value Key_F27
    \value Key_F28
    \value Key_F29
    \value Key_F30
    \value Key_F31
    \value Key_F32
    \value Key_F33
    \value Key_F34
    \value Key_F35
    \value Key_Super_L
    \value Key_Super_R
    \value Key_Menu
    \value Key_Hyper_L
    \value Key_Hyper_R
    \value Key_Help
    \value Key_Space
    \value Key_Any
    \value Key_Exclam
    \value Key_QuoteDbl
    \value Key_NumberSign
    \value Key_Dollar
    \value Key_Percent
    \value Key_Ampersand
    \value Key_Apostrophe
    \value Key_ParenLeft
    \value Key_ParenRight
    \value Key_Asterisk
    \value Key_Plus
    \value Key_Comma
    \value Key_Minus
    \value Key_Period
    \value Key_Slash
    \value Key_0
    \value Key_1
    \value Key_2
    \value Key_3
    \value Key_4
    \value Key_5
    \value Key_6
    \value Key_7
    \value Key_8
    \value Key_9
    \value Key_Colon
    \value Key_Semicolon
    \value Key_Less
    \value Key_Equal
    \value Key_Greater
    \value Key_Question
    \value Key_At
    \value Key_A
    \value Key_B
    \value Key_C
    \value Key_D
    \value Key_E
    \value Key_F
    \value Key_G
    \value Key_H
    \value Key_I
    \value Key_J
    \value Key_K
    \value Key_L
    \value Key_M
    \value Key_N
    \value Key_O
    \value Key_P
    \value Key_Q
    \value Key_R
    \value Key_S
    \value Key_T
    \value Key_U
    \value Key_V
    \value Key_W
    \value Key_X
    \value Key_Y
    \value Key_Z
    \value Key_BracketLeft
    \value Key_Backslash
    \value Key_BracketRight
    \value Key_AsciiCircum
    \value Key_Underscore
    \value Key_QuoteLeft
    \value Key_BraceLeft
    \value Key_Bar
    \value Key_BraceRight
    \value Key_AsciiTilde

    \value Key_nobreakspace
    \value Key_exclamdown
    \value Key_cent
    \value Key_sterling
    \value Key_currency
    \value Key_yen
    \value Key_brokenbar
    \value Key_section
    \value Key_diaeresis
    \value Key_copyright
    \value Key_ordfeminine
    \value Key_guillemotleft
    \value Key_notsign
    \value Key_hyphen
    \value Key_registered
    \value Key_macron
    \value Key_degree
    \value Key_plusminus
    \value Key_twosuperior
    \value Key_threesuperior
    \value Key_acute
    \value Key_mu
    \value Key_paragraph
    \value Key_periodcentered
    \value Key_cedilla
    \value Key_onesuperior
    \value Key_masculine
    \value Key_guillemotright
    \value Key_onequarter
    \value Key_onehalf
    \value Key_threequarters
    \value Key_questiondown
    \value Key_Agrave
    \value Key_Aacute
    \value Key_Acircumflex
    \value Key_Atilde
    \value Key_Adiaeresis
    \value Key_Aring
    \value Key_AE
    \value Key_Ccedilla
    \value Key_Egrave
    \value Key_Eacute
    \value Key_Ecircumflex
    \value Key_Ediaeresis
    \value Key_Igrave
    \value Key_Iacute
    \value Key_Icircumflex
    \value Key_Idiaeresis
    \value Key_ETH
    \value Key_Ntilde
    \value Key_Ograve
    \value Key_Oacute
    \value Key_Ocircumflex
    \value Key_Otilde
    \value Key_Odiaeresis
    \value Key_multiply
    \value Key_Ooblique
    \value Key_Ugrave
    \value Key_Uacute
    \value Key_Ucircumflex
    \value Key_Udiaeresis
    \value Key_Yacute
    \value Key_THORN
    \value Key_ssharp
    \value Key_agrave
    \value Key_aacute
    \value Key_acircumflex
    \value Key_atilde
    \value Key_adiaeresis
    \value Key_aring
    \value Key_ae
    \value Key_ccedilla
    \value Key_egrave
    \value Key_eacute
    \value Key_ecircumflex
    \value Key_ediaeresis
    \value Key_igrave
    \value Key_iacute
    \value Key_icircumflex
    \value Key_idiaeresis
    \value Key_eth
    \value Key_ntilde
    \value Key_ograve
    \value Key_oacute
    \value Key_ocircumflex
    \value Key_otilde
    \value Key_odiaeresis
    \value Key_division
    \value Key_oslash
    \value Key_ugrave
    \value Key_uacute
    \value Key_ucircumflex
    \value Key_udiaeresis
    \value Key_yacute
    \value Key_thorn
    \value Key_ydiaeresis

    Multimedia keys

    \value Key_Back
    \value Key_Forward
    \value Key_Stop
    \value Key_Refresh

    \value Key_VolumeDown
    \value Key_VolumeMute
    \value Key_VolumeUp
    \value Key_BassBoost
    \value Key_BassUp
    \value Key_BassDown
    \value Key_TrebleUp
    \value Key_TrebleDown

    \value Key_MediaPlay
    \value Key_MediaStop
    \value Key_MediaPrev
    \value Key_MediaNext
    \value Key_MediaRecord

    \value Key_HomePage
    \value Key_Favorites
    \value Key_Search
    \value Key_Standby
    \value Key_OpenUrl

    \value Key_LaunchMail
    \value Key_LaunchMedia
    \value Key_Launch0
    \value Key_Launch1
    \value Key_Launch2
    \value Key_Launch3
    \value Key_Launch4
    \value Key_Launch5
    \value Key_Launch6
    \value Key_Launch7
    \value Key_Launch8
    \value Key_Launch9
    \value Key_LaunchA
    \value Key_LaunchB
    \value Key_LaunchC
    \value Key_LaunchD
    \value Key_LaunchE
    \value Key_LaunchF
    \value Key_MonBrightnessUp
    \value Key_MonBrightnessDown
    \value Key_KeyboardLightOnOff
    \value Key_KeyboardBrightnessUp
    \value Key_KeyboardBrightnessDown

    \value Key_MediaLast

    \value Key_unknown

    \value Key_Direction_L internal use only
    \value Key_Direction_R internal use only

*/


/*!
    \class TQFocusEvent ntqevent.h
    \brief The TQFocusEvent class contains event parameters for widget focus
    events.

    \ingroup events

    Focus events are sent to widgets when the keyboard input focus
    changes. Focus events occur due to mouse actions, keypresses (e.g.
    Tab or Backtab), the window system, popup menus, keyboard
    shortcuts or other application specific reasons. The reason for a
    particular focus event is returned by reason() in the appropriate
    event handler.

    The event handlers TQWidget::focusInEvent() and
    TQWidget::focusOutEvent() receive focus events.

    Use setReason() to set the reason for all focus events, and
    resetReason() to set the reason for all focus events to the reason
    in force before the last setReason() call.

    \sa TQWidget::setFocus(), TQWidget::setFocusPolicy()
*/

/*!
    \fn TQFocusEvent::TQFocusEvent( Type type )

    Constructs a focus event object.

    The \a type parameter must be either \c TQEvent::FocusIn or \c
    TQEvent::FocusOut.
*/



TQFocusEvent::Reason TQFocusEvent::m_reason = TQFocusEvent::Other;
TQFocusEvent::Reason TQFocusEvent::prev_reason = TQFocusEvent::Other;


/*!
    \enum TQFocusEvent::Reason

    This enum specifies why the focus changed.

    \value Mouse  because of a mouse action.
    \value Tab  because of a Tab press.
    \value Backtab  because of a Backtab press
	    (possibly including Shift/Control, e.g. Shift+Tab).
    \value ActiveWindow  because the window system made this window (in)active.
    \value Popup  because the application opened/closed a popup that grabbed/released focus.
    \value Shortcut  because of a keyboard shortcut.
    \value Other  any other reason, usually application-specific.

    See the \link focus.html keyboard focus overview\endlink for more
    about focus.
*/

/*!
    Returns the reason for this focus event.

    \sa setReason()
 */
TQFocusEvent::Reason TQFocusEvent::reason()
{
    return m_reason;
}

/*!
    Sets the reason for all future focus events to \a reason.

    \sa reason(), resetReason()
 */
void TQFocusEvent::setReason( Reason reason )
{
    prev_reason = m_reason;
    m_reason = reason;
}

/*!
    Resets the reason for all future focus events to the value before
    the last setReason() call.

    \sa reason(), setReason()
 */
void TQFocusEvent::resetReason()
{
    m_reason = prev_reason;
}

/*!
    \fn bool TQFocusEvent::gotFocus() const

    Returns true if the widget received the text input focus;
    otherwise returns false.
*/

/*!
    \fn bool TQFocusEvent::lostFocus() const

    Returns true if the widget lost the text input focus; otherwise
    returns false.
*/


/*!
    \class TQPaintEvent ntqevent.h
    \brief The TQPaintEvent class contains event parameters for paint events.

    \ingroup events

    Paint events are sent to widgets that need to update themselves,
    for instance when part of a widget is exposed because a covering
    widget is moved.

    The event contains a region() that needs to be updated, and a
    rect() that is the bounding rectangle of that region. Both are
    provided because many widgets can't make much use of region(), and
    rect() can be much faster than region().boundingRect(). Painting
    is clipped to region() during processing of a paint event.

    The erased() function returns true if the region() has been
    cleared to the widget's background (see
    TQWidget::backgroundMode()), and false if the region's contents are
    arbitrary.

    \sa TQPainter TQWidget::update() TQWidget::repaint()
    TQWidget::paintEvent() TQWidget::backgroundMode() TQRegion
*/

/*!
    \fn TQPaintEvent::TQPaintEvent( const TQRegion &paintRegion, bool erased=true )

    Constructs a paint event object with the region that should be
    updated. The region is given by \a paintRegion. If \a erased is
    true the region will be cleared before repainting.
*/

/*!
    \fn TQPaintEvent::TQPaintEvent( const TQRect &paintRect, bool erased=true )

    Constructs a paint event object with the rectangle that should be
    updated. The region is also given by \a paintRect. If \a erased is
    true the region will be cleared before repainting.
*/

/*!
    \fn TQPaintEvent::TQPaintEvent( const TQRegion &paintRegion, const TQRect &paintRect, bool erased=true )

    Constructs a paint event object with the rectangle \a paintRect
    that should be updated. The region is given by \a paintRegion. If
    \a erased is true the region will be cleared before repainting.
*/

/*!
    \fn const TQRect &TQPaintEvent::rect() const

    Returns the rectangle that should be updated.

    \sa region(), TQPainter::setClipRect()
*/

/*!
    \fn const TQRegion &TQPaintEvent::region() const

    Returns the region that should be updated.

    \sa rect(), TQPainter::setClipRegion()
*/

/*!
    \fn bool TQPaintEvent::erased() const

    Returns true if the paint event region (or rectangle) has been
    erased with the widget's background; otherwise returns false.
*/

/*!
    \class TQMoveEvent ntqevent.h
    \brief The TQMoveEvent class contains event parameters for move events.

    \ingroup events

    Move events are sent to widgets that have been moved to a new position
    relative to their parent.

    The event handler TQWidget::moveEvent() receives move events.

    \sa TQWidget::move(), TQWidget::setGeometry()
*/

/*!
    \fn TQMoveEvent::TQMoveEvent( const TQPoint &pos, const TQPoint &oldPos )

    Constructs a move event with the new and old widget positions, \a
    pos and \a oldPos respectively.
*/

/*!
    \fn const TQPoint &TQMoveEvent::pos() const

    Returns the new position of the widget. This excludes the window
    frame for top level widgets.
*/

/*!
    \fn const TQPoint &TQMoveEvent::oldPos() const

    Returns the old position of the widget.
*/


/*!
    \class TQResizeEvent ntqevent.h
    \brief The TQResizeEvent class contains event parameters for resize events.

    \ingroup events

    Resize events are sent to widgets that have been resized.

    The event handler TQWidget::resizeEvent() receives resize events.

    \sa TQWidget::resize(), TQWidget::setGeometry()
*/

/*!
    \fn TQResizeEvent::TQResizeEvent( const TQSize &size, const TQSize &oldSize )

    Constructs a resize event with the new and old widget sizes, \a
    size and \a oldSize respectively.
*/

/*!
    \fn const TQSize &TQResizeEvent::size() const

    Returns the new size of the widget, which is the same as
    TQWidget::size().
*/

/*!
    \fn const TQSize &TQResizeEvent::oldSize() const

    Returns the old size of the widget.
*/


/*!
    \class TQCloseEvent ntqevent.h
    \brief The TQCloseEvent class contains parameters that describe a close event.

    \ingroup events

    Close events are sent to widgets that the user wants to close,
    usually by choosing "Close" from the window menu, or by clicking
    the `X' titlebar button. They are also sent when you call
    TQWidget::close() to close a widget programmatically.

    Close events contain a flag that indicates whether the receiver
    wants the widget to be closed or not. When a widget accepts the
    close event, it is hidden (and destroyed if it was created with
    the \c WDestructiveClose flag). If it refuses to accept the close
    event nothing happens. (Under X11 it is possible that the window
    manager will forcibly close the window; but at the time of writing
    we are not aware of any window manager that does this.)

    The application's main widget -- TQApplication::mainWidget() --
    is a special case. When it accepts the close event, TQt leaves the
    main event loop and the application is immediately terminated
    (i.e. it returns from the call to TQApplication::exec() in the
    main() function).

    The event handler TQWidget::closeEvent() receives close events. The
    default implementation of this event handler accepts the close
    event. If you do not want your widget to be hidden, or want some
    special handing, you should reimplement the event handler.

    The \link simple-application.html#closeEvent closeEvent() in the
    Application Walkthrough\endlink shows a close event handler that
    asks whether to save a document before closing.

    If you want the widget to be deleted when it is closed, create it
    with the \c WDestructiveClose widget flag. This is very useful for
    independent top-level windows in a multi-window application.

    \l{TQObject}s emits the \link TQObject::destroyed()
    destroyed()\endlink signal when they are deleted.

    If the last top-level window is closed, the
    TQApplication::lastWindowClosed() signal is emitted.

    The isAccepted() function returns true if the event's receiver has
    agreed to close the widget; call accept() to agree to close the
    widget and call ignore() if the receiver of this event does not
    want the widget to be closed.

    \sa TQWidget::close(), TQWidget::hide(), TQObject::destroyed(),
    TQApplication::setMainWidget(), TQApplication::lastWindowClosed(),
    TQApplication::exec(), TQApplication::quit()
*/

/*!
    \fn TQCloseEvent::TQCloseEvent()

    Constructs a close event object with the accept parameter flag set
    to false.

    \sa accept()
*/

/*!
    \fn bool TQCloseEvent::isAccepted() const

    Returns true if the receiver of the event has agreed to close the
    widget; otherwise returns false.

    \sa accept(), ignore()
*/

/*!
    \fn void TQCloseEvent::accept()

    Sets the accept flag of the close event object.

    Setting the accept flag indicates that the receiver of this event
    agrees to close the widget.

    The accept flag is \e not set by default.

    If you choose to accept in TQWidget::closeEvent(), the widget will
    be hidden. If the widget's \c WDestructiveClose flag is set, it
    will also be destroyed.

    \sa ignore(), TQWidget::hide()
*/

/*!
    \fn void TQCloseEvent::ignore()

    Clears the accept flag of the close event object.

    Clearing the accept flag indicates that the receiver of this event
    does not want the widget to be closed.

    The close event is constructed with the accept flag cleared.

    \sa accept()
*/

/*!
   \class TQIconDragEvent ntqevent.h
   \brief The TQIconDragEvent class signals that a main icon drag has begun.

    \ingroup events

    Icon drag events are sent to widgets when the main icon of a window has been dragged away.
    On Mac OS X this is fired when the proxy icon of a window is dragged off titlebar, in response to
    this event is is normal to begin using drag and drop.
*/

/*!
    \fn TQIconDragEvent::TQIconDragEvent()

    Constructs an icon drag event object with the accept parameter
    flag set to false.

    \sa accept()
*/

/*!
    \fn bool TQIconDragEvent::isAccepted() const

    Returns true if the receiver of the event has started a drag and
    drop operation; otherwise returns false.

    \sa accept(), ignore()
*/

/*!
    \fn void TQIconDragEvent::accept()

    Sets the accept flag of the icon drag event object.

    Setting the accept flag indicates that the receiver of this event
    has started a drag and drop oeration.

    The accept flag is \e not set by default.

    \sa ignore(), TQWidget::hide()
*/

/*!
    \fn void TQIconDragEvent::ignore()

    Clears the accept flag of the icon drag object.

    Clearing the accept flag indicates that the receiver of this event
    has not handled the icon drag as a result other events can be sent.

    The icon drag event is constructed with the accept flag cleared.

    \sa accept()
*/

/*!
    \class TQContextMenuEvent ntqevent.h
    \brief The TQContextMenuEvent class contains parameters that describe a context menu event.

    \ingroup events

    Context menu events are sent to widgets when a user triggers a
    context menu. What triggers this is platform dependent. For
    example, on Windows, pressing the menu button or releasing the
    right mouse button will cause this event to be sent.

    When this event occurs it is customary to show a TQPopupMenu with a
    context menu, if this is relevant to the context.

    Context menu events contain a special accept flag that indicates
    whether the receiver accepted the event. If the event handler does
    not accept the event, then whatever triggered the event will be
    handled as a regular input event if possible.

    \sa TQPopupMenu
*/

/*!
    \fn TQContextMenuEvent::TQContextMenuEvent( Reason reason, const TQPoint &pos, const TQPoint &globalPos, int state )

    Constructs a context menu event object with the accept parameter
    flag set to false.

    The \a reason parameter must be \c TQContextMenuEvent::Mouse or \c
    TQContextMenuEvent::Keyboard.

    The \a pos parameter specifies the mouse position relative to the
    receiving widget. \a globalPos is the mouse position in absolute
    coordinates. \a state is the ButtonState at the time of the event.
*/


/*!
    \fn TQContextMenuEvent::TQContextMenuEvent( Reason reason, const TQPoint &pos, int state )

    Constructs a context menu event object with the accept parameter
    flag set to false.

    The \a reason parameter must be \c TQContextMenuEvent::Mouse or \c
    TQContextMenuEvent::Keyboard.

    The \a pos parameter specifies the mouse position relative to the
    receiving widget. \a state is the ButtonState at the time of the
    event.

    The globalPos() is initialized to TQCursor::pos(), which may not be
    appropriate. Use the other constructor to specify the global
    position explicitly.
*/

TQContextMenuEvent::TQContextMenuEvent( Reason reason, const TQPoint &pos, int state )
    : TQEvent( ContextMenu ), p( pos ), accpt(true), consum(true),
    reas( reason ), s((ushort)state)
{
    gp = TQCursor::pos();
}

/*!
    \fn const TQPoint &TQContextMenuEvent::pos() const

    Returns the position of the mouse pointer relative to the widget
    that received the event.

    \sa x(), y(), globalPos()
*/

/*!
    \fn int TQContextMenuEvent::x() const

    Returns the x-position of the mouse pointer, relative to the
    widget that received the event.

    \sa y(), pos()
*/

/*!
    \fn int TQContextMenuEvent::y() const

    Returns the y-position of the mouse pointer, relative to the
    widget that received the event.

    \sa x(), pos()
*/

/*!
    \fn const TQPoint &TQContextMenuEvent::globalPos() const

    Returns the global position of the mouse pointer at the time of
    the event.

    \sa x(), y(), pos()
*/

/*!
    \fn int TQContextMenuEvent::globalX() const

    Returns the global x-position of the mouse pointer at the time of
    the event.

    \sa globalY(), globalPos()
*/

/*!
    \fn int TQContextMenuEvent::globalY() const

    Returns the global y-position of the mouse pointer at the time of
    the event.

    \sa globalX(), globalPos()
*/

/*!
    \fn ButtonState TQContextMenuEvent::state() const

    Returns the button state (a combination of mouse buttons and
    keyboard modifiers), i.e. what buttons and keys were being
    pressed immediately before the event was generated.

    The returned value is \c LeftButton, \c RightButton, \c MidButton,
    \c ShiftButton, \c ControlButton and \c AltButton OR'ed together.
*/

/*!
    \fn bool TQContextMenuEvent::isConsumed() const

    Returns true (which stops propagation of the event) if the
    receiver has blocked the event; otherwise returns false.

    \sa accept(), ignore(), consume()
*/

/*!
    \fn void TQContextMenuEvent::consume()

    Sets the consume flag of the context event object.

    Setting the consume flag indicates that the receiver of this event
    does not want the event to be propagated further (i.e. not sent to
    parent classes.)

    The consumed flag is not set by default.

    \sa ignore() accept()
*/

/*!
    \fn bool TQContextMenuEvent::isAccepted() const

    Returns true if the receiver has processed the event; otherwise
    returns false.

    \sa accept(), ignore(), consume()
*/

/*!
    \fn void TQContextMenuEvent::accept()

    Sets the accept flag of the context event object.

    Setting the accept flag indicates that the receiver of this event
    has processed the event. Processing the event means you did
    something with it and it will be implicitly consumed.

    The accept flag is not set by default.

    \sa ignore() consume()
*/

/*!
    \fn void TQContextMenuEvent::ignore()

    Clears the accept flag of the context event object.

    Clearing the accept flag indicates that the receiver of this event
    does not need to show a context menu. This will implicitly remove
    the consumed flag as well.

    The accept flag is not set by default.

    \sa accept() consume()
*/

/*!
    \enum TQContextMenuEvent::Reason

    This enum describes the reason the ContextMenuEvent was sent. The
    values are:

    \value Mouse The mouse caused the event to be sent. Normally this
    means the right mouse button was clicked, but this is platform
    specific.

    \value Keyboard The keyboard caused this event to be sent. On
    Windows this means the menu button was pressed.

    \value Other The event was sent by some other means (i.e. not by
    the mouse or keyboard).
*/


/*!
    \fn TQContextMenuEvent::Reason TQContextMenuEvent::reason() const

    Returns the reason for this context event.
*/


/*!
    \class TQIMEvent ntqevent.h
    \brief The TQIMEvent class provides parameters for input method events.

    \ingroup events

    Input method events are sent to widgets when an input method is
    used to enter text into a widget. Input methods are widely used to
    enter text in Asian and other complex languages.

    The events are of interest to widgets that accept keyboard input
    and want to be able to correctly handle complex languages. Text
    input in such languages is usually a three step process.

    \list 1
    \i <b>Starting to Compose</b><br>
    When the user presses the first key on a keyboard an input context
    is created. This input context will contain a string with the
    typed characters.

    \i <b>Composing</b><br>
    With every new key pressed, the input method will try to create a
    matching string for the text typed so far. While the input context
    is active, the user can only move the cursor inside the string
    belonging to this input context.

    \i <b>Completing</b><br>
    At some point, e.g. when the user presses the Spacebar, they get
    to this stage, where they can choose from a number of strings that
    match the text they have typed so far. The user can press Enter to
    confirm their choice or Escape to cancel the input; in either case
    the input context will be closed.
    \endlist

    Note that the particular key presses used for a given input
    context may differ from those we've mentioned here, i.e. they may
    not be Spacebar, Enter and Escape.

    These three stages are represented by three different types of
    events. The IMStartEvent, IMComposeEvent and IMEndEvent. When a
    new input context is created, an IMStartEvent will be sent to the
    widget and delivered to the \l TQWidget::imStartEvent() function.
    The widget can then update internal data structures to reflect
    this.

    After this, an IMComposeEvent will be sent to the widget for
    every key the user presses. It will contain the current
    composition string the widget has to show and the current cursor
    position within the composition string. This string is temporary
    and can change with every key the user types, so the widget will
    need to store the state before the composition started (the state
    it had when it received the IMStartEvent). IMComposeEvents will be
    delivered to the \l TQWidget::imComposeEvent() function.

    Usually, widgets try to mark the part of the text that is part of
    the current composition in a way that is visible to the user. A
    commonly used visual cue is to use a dotted underline.

    After the user has selected the final string, an IMEndEvent will
    be sent to the widget. The event contains the final string the
    user selected, and could be empty if they canceled the
    composition. This string should be accepted as the final text the
    user entered, and the intermediate composition string should be
    cleared. These events are delivered to \l TQWidget::imEndEvent().

    If the user clicks another widget, taking the focus out of the
    widget where the composition is taking place the IMEndEvent will
    be sent and the string it holds will be the result of the
    composition up to that point (which may be an empty string).
*/

/*!
    \fn  TQIMEvent::TQIMEvent( Type type, const TQString &text, int cursorPosition )

    Constructs a new TQIMEvent with the accept flag set to false. \a
    type can be one of TQEvent::IMStartEvent, TQEvent::IMComposeEvent
    or TQEvent::IMEndEvent. \a text contains the current compostion
    string and \a cursorPosition the current position of the cursor
    inside \a text.
*/

/*!
    \fn const TQString &TQIMEvent::text() const

    Returns the composition text. This is a null string for an
    IMStartEvent, and contains the final accepted string (which may be
    empty) in the IMEndEvent.
*/

/*!
    \fn int TQIMEvent::cursorPos() const

    Returns the current cursor position inside the composition string.
    Will return -1 for IMStartEvent and IMEndEvent.
*/

/*!
    \fn int TQIMEvent::selectionLength() const

    Returns the number of characters in the composition string (
    starting at cursorPos() ) that should be marked as selected by the
    input widget receiving the event.
    Will return 0 for IMStartEvent and IMEndEvent.
*/

/*!
    \fn bool TQIMEvent::isAccepted() const

    Returns true if the receiver of the event processed the event;
    otherwise returns false.
*/

/*!
    \fn void TQIMEvent::accept()

    Sets the accept flag of the input method event object.

    Setting the accept parameter indicates that the receiver of the
    event processed the input method event.

    The accept flag is not set by default.

    \sa ignore()
*/


/*!
    \fn void TQIMEvent::ignore()

    Clears the accept flag parameter of the input method event object.

    Clearing the accept parameter indicates that the event receiver
    does not want the input method event.

    The accept flag is cleared by default.

    \sa accept()
*/

/*!
    \class TQTabletEvent ntqevent.h
    \brief The TQTabletEvent class contains parameters that describe a Tablet
    event.

    \ingroup events

    Tablet Events are generated from a Wacom&copy; tablet. Most of
    the time you will want to deal with events from the tablet as if
    they were events from a mouse, for example retrieving the position
    with x(), y(), pos(), globalX(), globalY() and globalPos(). In
    some situations you may wish to retrieve the extra information
    provided by the tablet device driver, for example, you might want
    to adjust color brightness based on pressure. TQTabletEvent allows
    you to get the pressure(), the xTilt() and yTilt(), as well as the
    type of device being used with device() (see \l{TabletDevice}).

    A tablet event contains a special accept flag that indicates
    whether the receiver wants the event. You should call
    TQTabletEvent::accept() if you handle the tablet event; otherwise
    it will be sent to the parent widget.

    The TQWidget::setEnabled() function can be used to enable or
    disable mouse and keyboard events for a widget.

  The event handler TQWidget::tabletEvent() receives all three types of tablet
  events.  TQt will first send a tabletEvent and then, if it is not accepted,
  it will send a mouse event.  This allows applications that don't utilize
  tablets to use a tablet like a mouse while also enabling those who want to
  use both tablets and mouses differently.

*/

/*!
    \enum TQTabletEvent::TabletDevice

    This enum defines what type of device is generating the event.

    \value NoDevice    No device, or an unknown device.
    \value Puck    A Puck (a device that is similar to a flat mouse with
    a transparent circle with cross-hairs).
    \value Stylus  A Stylus (the narrow end of the pen).
    \value Eraser  An Eraser (the broad end of the pen).
    \omit
    \value Menu  A menu button was pressed (currently unimplemented).
*/

/*!
  \fn TQTabletEvent::TQTabletEvent( Type t, const TQPoint &pos,
                                  const TQPoint &globalPos, int device,
                                  int pressure, int xTilt, int yTilt,
				  const TQPair<int,int> &uId )
  Construct a tablet event of type \a t.  The position of when the event occurred is given
  int \a pos and \a globalPos.  \a device contains the \link TabletDevice device type\endlink,
  \a pressure contains the pressure exerted on the \a device, \a xTilt and \a yTilt contain
  \a device's degree of tilt from the X and Y axis respectively.  The \a uId contains an
  event id.

  \sa pos(), globalPos(), device(), pressure(), xTilt(), yTilt()
*/

TQTabletEvent::TQTabletEvent( Type t, const TQPoint &pos, const TQPoint &globalPos, int device,
			    int pressure, int xTilt, int yTilt,
			    const TQPair<int, int> &uId )
    : TQEvent( t ),
      mPos( pos ),
      mGPos( globalPos ),
      mDev( device ),
      mPress( pressure ),
      mXT( xTilt ),
      mYT( yTilt ),
      mType( uId.first ),
      mPhy( uId.second ),
      mbAcc(true)
{}

/*!
  \obsolete
  \fn TQTabletEvent::TQTabletEvent( const TQPoint &pos, const TQPoint &globalPos, int device, int pressure, int xTilt, int yTilt, const TQPair<int,int> &uId )

    Constructs a tablet event object. The position when the event
    occurred is is given in \a pos and \a globalPos. \a device
    contains the \link TabletDevice device type\endlink, \a pressure
    contains the pressure exerted on the \a device, \a xTilt and \a
    yTilt contain the \a device's degrees of tilt from the X and Y
    axis respectively. The \a uId contains an event id.

  \sa pos(), globalPos(), device(), pressure(), xTilt(), yTilt()
*/

/*!
    \fn TabletDevices TQTabletEvent::device() const

    Returns the type of device that generated the event. Useful if you
    want one end of the pen to do something different than the other.

    \sa TabletDevice
*/

/*!
    \fn int TQTabletEvent::pressure() const

    Returns the pressure that is exerted on the device. This number is
    a value from 0 (no pressure) to 255 (maximum pressure). The
    pressure is always scaled to be within this range no matter how
    many pressure levels the underlying hardware supports.
*/

/*!
    \fn int TQTabletEvent::xTilt() const

    Returns the difference from the perpendicular in the X Axis.
    Positive values are towards the tablet's physical right. The angle
    is in the range -60 to +60 degrees.

    \sa yTilt()
*/

/*!
    \fn int TQTabletEvent::yTilt() const

    Returns the difference from the perpendicular in the Y Axis.
    Positive values are towards the bottom of the tablet. The angle is
    within the range -60 to +60 degrees.

    \sa xTilt()
*/

/*!
    \fn const TQPoint &TQTabletEvent::pos() const

    Returns the position of the device, relative to the widget that
    received the event.

    If you move widgets around in response to mouse events, use
    globalPos() instead of this function.

    \sa x(), y(), globalPos()
*/

/*!
    \fn int TQTabletEvent::x() const

    Returns the x-position of the device, relative to the widget that
    received the event.

    \sa y(), pos()
*/

/*!
    \fn int TQTabletEvent::y() const

    Returns the y-position of the device, relative to the widget that
    received the event.

    \sa x(), pos()
*/

/*!
    \fn const TQPoint &TQTabletEvent::globalPos() const

    Returns the global position of the device \e{at the time of the
    event}. This is important on asynchronous windows systems like X11;
    whenever you move your widgets around in response to mouse events,
    globalPos() can differ significantly from the current position
    TQCursor::pos().

    \sa globalX(), globalY()
*/

/*!
    \fn int TQTabletEvent::globalX() const

    Returns the global x-position of the mouse pointer at the time of
    the event.

    \sa globalY(), globalPos()
*/

/*!
    \fn int TQTabletEvent::globalY() const

    Returns the global y-position of the mouse pointer at the time of
    the event.

    \sa globalX(), globalPos()
*/

/*!
    \fn bool TQTabletEvent::isAccepted() const

    Returns true if the receiver of the event handles the tablet
    event; otherwise returns false.
*/

/*!
    \fn void TQTabletEvent::accept()

    Sets the accept flag of the tablet event object.

    Setting the accept flag indicates that the receiver of the event
    wants the tablet event. Unwanted tablet events are sent to the
    parent widget.

    The accept flag is set by default.

    \sa ignore()
*/

/*!
    \fn void TQTabletEvent::ignore()

    Clears the accept flag parameter of the tablet event object.

    Clearing the accept flag indicates that the event receiver does
    not want the tablet event. Unwanted tablet events are sent to the
    parent widget.

    The accept flag is set by default.

    \sa accept()
*/

/*!
    \fn TQPair<int, int> TQTabletEvent::uniqueId()

    Returns a unique ID for the current device. It is possible to
    generate a unique ID for any Wacom&copy; device. This makes it
    possible to differentiate between multiple devices being used at
    the same time on the tablet. The \c first member contains a value
    for the type, the \c second member contains a physical ID obtained
    from the device. Each combination of these values is unique. Note:
    for different platforms, the \c first value is different due to
    different driver implementations.
*/

/*!
    \class TQChildEvent ntqevent.h
    \brief The TQChildEvent class contains event parameters for child object
    events.

    \ingroup events

    Child events are sent to objects when children are inserted or
    removed.

    A \c ChildRemoved event is sent immediately, but a \c
    ChildInserted event is \e posted (with TQApplication::postEvent()).

    Note that if a child is removed immediately after it is inserted,
    the \c ChildInserted event may be suppressed, but the \c
    ChildRemoved event will always be sent. In this case there will be
    a \c ChildRemoved event without a corresponding \c ChildInserted
    event.

    The handler for these events is TQObject::childEvent().
*/

/*!
    \fn TQChildEvent::TQChildEvent( Type type, TQObject *child )

    Constructs a child event object. The \a child is the object that
    is to be removed or inserted.

    The \a type parameter must be either \c TQEvent::ChildInserted or
    \c TQEvent::ChildRemoved.
*/

/*!
    \fn TQObject *TQChildEvent::child() const

    Returns the child widget that was inserted or removed.
*/

/*!
    \fn bool TQChildEvent::inserted() const

    Returns true if the widget received a new child; otherwise returns
    false.
*/

/*!
    \fn bool TQChildEvent::removed() const

    Returns true if the object lost a child; otherwise returns false.
*/




/*!
    \class TQCustomEvent ntqevent.h
    \brief The TQCustomEvent class provides support for custom events.

    \ingroup events

    TQCustomEvent is a generic event class for user-defined events.
    User defined events can be sent to widgets or other TQObject
    instances using TQApplication::postEvent() or
    TQApplication::sendEvent(). Subclasses of TQObject can easily
    receive custom events by implementing the TQObject::customEvent()
    event handler function.

    TQCustomEvent objects should be created with a type ID that
    uniquely identifies the event type. To avoid clashes with the
    TQt-defined events types, the value should be at least as large as
    the value of the "User" entry in the TQEvent::Type enum.

    TQCustomEvent contains a generic void* data member that may be used
    for transferring event-specific data to the receiver. Note that
    since events are normally delivered asynchronously, the data
    pointer, if used, must remain valid until the event has been
    received and processed.

    TQCustomEvent can be used as-is for simple user-defined event
    types, but normally you will want to make a subclass of it for
    your event types. In a subclass, you can add data members that are
    suitable for your event type.

    Example:
    \code
    class ColorChangeEvent : public TQCustomEvent
    {
    public:
	ColorChangeEvent( TQColor color )
	    : TQCustomEvent( 65432 ), c( color ) {}
	TQColor color() const { return c; }
    private:
	TQColor c;
    };

    // To send an event of this custom event type:

    ColorChangeEvent* ce = new ColorChangeEvent( blue );
    TQApplication::postEvent( receiver, ce );  // TQt will delete it when done

    // To receive an event of this custom event type:

    void MyWidget::customEvent( TQCustomEvent * e )
    {
	if ( e->type() == 65432 ) {  // It must be a ColorChangeEvent
	    ColorChangeEvent* ce = (ColorChangeEvent*)e;
	    newColor = ce->color();
	}
    }
    \endcode

    \sa TQWidget::customEvent(), TQApplication::notify()
*/


/*!
    Constructs a custom event object with event type \a type. The
    value of \a type must be at least as large as TQEvent::User. The
    data pointer is set to 0.
*/

TQCustomEvent::TQCustomEvent( int type )
    : TQEvent( (TQEvent::Type)type ), d( 0 )
{
}


/*!
    \fn TQCustomEvent::TQCustomEvent( Type type, void *data )

    Constructs a custom event object with the event type \a type and a
    pointer to \a data. (Note that any int value may safely be cast to
    TQEvent::Type).
*/


/*!
    \fn void TQCustomEvent::setData( void* data )

    Sets the generic data pointer to \a data.

    \sa data()
*/

/*!
    \fn void *TQCustomEvent::data() const

    Returns a pointer to the generic event data.

    \sa setData()
*/



/*!
    \fn TQDragMoveEvent::TQDragMoveEvent( const TQPoint& pos, Type type )

    Creates a TQDragMoveEvent for which the mouse is at point \a pos,
    and the event is of type \a type.

    \warning Do not create a TQDragMoveEvent yourself since these
    objects rely on TQt's internal state.
*/

/*!
    \fn void TQDragMoveEvent::accept( const TQRect & r )

    The same as accept(), but also notifies that future moves will
    also be acceptable if they remain within the rectangle \a r on the
    widget: this can improve performance, but may also be ignored by
    the underlying system.

    If the rectangle is \link TQRect::isEmpty() empty\endlink, then
    drag move events will be sent continuously. This is useful if the
    source is scrolling in a timer event.
*/

/*!
    \fn void TQDragMoveEvent::ignore( const TQRect & r)

    The opposite of accept(const TQRect&), i.e. says that moves within
    rectangle \a r are not acceptable (will be ignored).
*/

/*!
    \fn TQRect TQDragMoveEvent::answerRect() const

    Returns the rectangle for which the acceptance of the move event
    applies.
*/



/*!
    \fn const TQPoint& TQDropEvent::pos() const

    Returns the position where the drop was made.
*/

/*!
    \fn bool TQDropEvent::isAccepted () const

    Returns true if the drop target accepts the event; otherwise
    returns false.
*/

/*!
    \fn void TQDropEvent::accept(bool y=true)

    Call this function to indicate whether the event provided data
    which your widget processed. Set \a y to true (the default) if
    your widget could process the data, otherwise set \a y to false.
    To get the data, use encodedData(), or preferably, the decode()
    methods of existing TQDragObject subclasses, such as
    TQTextDrag::decode(), or your own subclasses.

    \sa acceptAction()
*/

/*!
    \fn void TQDropEvent::acceptAction(bool y=true)

    Call this to indicate that the action described by action() is
    accepted (i.e. if \a y is true, which is the default), not merely
    the default copy action. If you call acceptAction(true), there is
    no need to also call accept(true).
*/

/*!
  \fn void TQDragMoveEvent::accept( bool y )
  \reimp
  \internal
  Remove in 3.0
*/

/*!
  \fn void TQDragMoveEvent::ignore()
  \reimp
  \internal
  Remove in 3.0
*/


/*!
    \enum TQDropEvent::Action

    This enum describes the action which a source requests that a
    target perform with dropped data.

    \value Copy The default action. The source simply uses the data
		provided in the operation.
    \value Link The source should somehow create a link to the
		location specified by the data.
    \value Move The source should somehow move the object from the
		location specified by the data to a new location.
    \value Private  The target has special knowledge of the MIME type,
		which the source should respond to in a similar way to
		a Copy.
    \value UserAction  The source and target can co-operate using
		special actions. This feature is not currently
		supported.

    The Link and Move actions only makes sense if the data is a
    reference, for example, text/uri-list file lists (see TQUriDrag).
*/

/*!
    \fn void TQDropEvent::setAction( Action a )

    Sets the action to \a a. This is used internally, you should not
    need to call this in your code: the \e source decides the action,
    not the target.
*/

/*!
    \fn Action TQDropEvent::action() const

    Returns the Action which the target is requesting to be performed
    with the data. If your application understands the action and can
    process the supplied data, call acceptAction(); if your
    application can process the supplied data but can only perform the
    Copy action, call accept().
*/

/*!
    \fn void TQDropEvent::ignore()

    The opposite of accept(), i.e. you have ignored the drop event.
*/

/*!
    \fn bool TQDropEvent::isActionAccepted () const

    Returns true if the drop action was accepted by the drop site;
    otherwise returns false.
*/


/*!
    \fn void TQDropEvent::setPoint (const TQPoint & np)

    Sets the drop to happen at point \a np. You do not normally need
    to use this as it will be set internally before your widget
    receives the drop event.
*/ // ### here too - what coordinate system?


/*!
    \class TQDragEnterEvent ntqevent.h
    \brief The TQDragEnterEvent class provides an event which is sent to the widget when a drag and drop first drags onto the widget.

    \ingroup events
    \ingroup draganddrop

    This event is always immediately followed by a TQDragMoveEvent, so
    you only need to respond to one or the other event. This class
    inherits most of its functionality from TQDragMoveEvent, which in
    turn inherits most of its functionality from TQDropEvent.

    \sa TQDragLeaveEvent, TQDragMoveEvent, TQDropEvent
*/

/*!
    \fn TQDragEnterEvent::TQDragEnterEvent (const TQPoint & pos)

    Constructs a TQDragEnterEvent entering at the given point, \a pos.

    \warning Do not create a TQDragEnterEvent yourself since these
    objects rely on TQt's internal state.
*/

/*!
    \class TQDragLeaveEvent ntqevent.h
    \brief The TQDragLeaveEvent class provides an event which is sent to the widget when a drag and drop leaves the widget.

    \ingroup events
    \ingroup draganddrop

    This event is always preceded by a TQDragEnterEvent and a series of
    \l{TQDragMoveEvent}s. It is not sent if a TQDropEvent is sent
    instead.

    \sa TQDragEnterEvent, TQDragMoveEvent, TQDropEvent
*/

/*!
    \fn TQDragLeaveEvent::TQDragLeaveEvent()

    Constructs a TQDragLeaveEvent.

    \warning Do not create a TQDragLeaveEvent yourself since these
    objects rely on TQt's internal state.
*/

/*!
    \class TQHideEvent ntqevent.h
    \brief The TQHideEvent class provides an event which is sent after a widget is hidden.

    \ingroup events

    This event is sent just before TQWidget::hide() returns, and also
    when a top-level window has been hidden (iconified) by the user.

    If spontaneous() is true the event originated outside the
    application, i.e. the user hid the window using the window manager
    controls, either by iconifying the window or by switching to
    another virtual desktop where the window isn't visible. The window
    will become hidden but not withdrawn. If the window was iconified,
    TQWidget::isMinimized() returns true.

    \sa TQShowEvent
*/

/*!
    \fn TQHideEvent::TQHideEvent()

    Constructs a TQHideEvent.
*/

/*!
    \class TQShowEvent ntqevent.h
    \brief The TQShowEvent class provides an event which is sent when a widget is shown.

    \ingroup events

    There are two kinds of show events: show events caused by the
    window system (spontaneous) and internal show events. Spontaneous
    show events are sent just after the window system shows the
    window, including after a top-level window has been shown
    (un-iconified) by the user. Internal show events are delivered
    just before the widget becomes visible.

    \sa TQHideEvent
*/

/*!
    \fn TQShowEvent::TQShowEvent()

    Constructs a TQShowEvent.
*/


/*!
  \fn TQByteArray TQDropEvent::data(const char* f) const

  \obsolete

  Use TQDropEvent::encodedData().
*/


/*!
  Destroys the event. If it was \link
  TQApplication::postEvent() posted \endlink,
  it will be removed from the list of events to be posted.
*/

TQEvent::~TQEvent()
{
    if ( posted && tqApp )
	TQApplication::removePostedEvent( this );
}
