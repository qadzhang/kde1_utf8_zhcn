/****************************************************************************
**
** Implementation of TQWidget class
**
** Created : 931031
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
#include "ntqwidget.h"
#include "ntqwidgetlist.h"
#include "ntqwidgetintdict.h"
#include "ntqptrdict.h"
#include "ntqfocusdata.h"
#include "ntqcursor.h"
#include "ntqpixmap.h"
#include "ntqapplication.h"
#include "qapplication_p.h"
#include "ntqbrush.h"
#include "ntqlayout.h"
#include "ntqstylefactory.h"
#include "ntqcleanuphandler.h"
#include "ntqstyle.h"
#include "ntqmetaobject.h"
#include "ntqguardedptr.h"
#if defined(TQT_THREAD_SUPPORT)
#include "ntqthread.h"
#endif
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif
#if defined(TQ_WS_WIN)
#include "qt_windows.h"
#include "qinputcontext_p.h"
#endif
#if defined(TQ_WS_QWS)
#include "qwsmanager_qws.h"
#endif
#include "qfontdata_p.h"


/*!
    \class TQWidget ntqwidget.h
    \brief The TQWidget class is the base class of all user interface objects.

    \ingroup abstractwidgets
    \mainclass

    The widget is the atom of the user interface: it receives mouse,
    keyboard and other events from the window system, and paints a
    representation of itself on the screen. Every widget is
    rectangular, and they are sorted in a Z-order. A widget is
    clipped by its parent and by the widgets in front of it.

    A widget that isn't embedded in a parent widget is called a
    top-level widget. Usually, top-level widgets are windows with a
    frame and a title bar (although it is also possible to create
    top-level widgets without such decoration if suitable widget flags
    are used). In TQt, TQMainWindow and the various subclasses of
    TQDialog are the most common top-level windows.

    A widget without a parent widget is always a top-level widget.

    Non-top-level widgets are child widgets. These are child windows
    in their parent widgets. You cannot usually distinguish a child
    widget from its parent visually. Most other widgets in TQt are
    useful only as child widgets. (It is possible to make, say, a
    button into a top-level widget, but most people prefer to put
    their buttons inside other widgets, e.g. TQDialog.)

    If you want to use a TQWidget to hold child widgets you will
    probably want to add a layout to the parent TQWidget. (See \link
    layout.html Layouts\endlink.)

    TQWidget has many member functions, but some of them have little
    direct functionality: for example, TQWidget has a font property,
    but never uses this itself. There are many subclasses which
    provide real functionality, such as TQPushButton, TQListBox and
    TQTabDialog, etc.

    \section1 Groups of functions:

    \table
    \header \i Context \i Functions

    \row \i Window functions \i
	show(),
	hide(),
	raise(),
	lower(),
	close().

    \row \i Top level windows \i
	caption(),
	setCaption(),
	icon(),
	setIcon(),
	iconText(),
	setIconText(),
	isActiveWindow(),
	setActiveWindow(),
	showMinimized().
	showMaximized(),
	showFullScreen(),
	showNormal().

    \row \i Window contents \i
	update(),
	repaint(),
	erase(),
	scroll(),
	updateMask().

    \row \i Geometry \i
	pos(),
	size(),
	rect(),
	x(),
	y(),
	width(),
	height(),
	sizePolicy(),
	setSizePolicy(),
	sizeHint(),
	updateGeometry(),
	layout(),
	move(),
	resize(),
	setGeometry(),
	frameGeometry(),
	geometry(),
	childrenRect(),
	adjustSize(),
	mapFromGlobal(),
	mapFromParent()
	mapToGlobal(),
	mapToParent(),
	maximumSize(),
	minimumSize(),
	sizeIncrement(),
	setMaximumSize(),
	setMinimumSize(),
	setSizeIncrement(),
	setBaseSize(),
	setFixedSize()

    \row \i Mode \i
	isVisible(),
	isVisibleTo(),
	isMinimized(),
	isDesktop(),
	isEnabled(),
	isEnabledTo(),
	isModal(),
	isPopup(),
	isTopLevel(),
	setEnabled(),
	hasMouseTracking(),
	setMouseTracking(),
	isUpdatesEnabled(),
	setUpdatesEnabled(),
	clipRegion().

    \row \i Look and feel \i
	style(),
	setStyle(),
	cursor(),
	setCursor()
	font(),
	setFont(),
	palette(),
	setPalette(),
	backgroundMode(),
	setBackgroundMode(),
	colorGroup(),
	fontMetrics(),
	fontInfo().

    \row \i Keyboard focus<br>functions \i
	isFocusEnabled(),
	setFocusPolicy(),
	focusPolicy(),
	hasFocus(),
	setFocus(),
	clearFocus(),
	setTabOrder(),
	setFocusProxy().

    \row \i Mouse and<br>keyboard grabbing \i
	grabMouse(),
	releaseMouse(),
	grabKeyboard(),
	releaseKeyboard(),
	mouseGrabber(),
	keyboardGrabber().

    \row \i Event handlers \i
	event(),
	mousePressEvent(),
	mouseReleaseEvent(),
	mouseDoubleClickEvent(),
	mouseMoveEvent(),
	keyPressEvent(),
	keyReleaseEvent(),
	focusInEvent(),
	focusOutEvent(),
	wheelEvent(),
	enterEvent(),
	leaveEvent(),
	paintEvent(),
	moveEvent(),
	resizeEvent(),
	closeEvent(),
	dragEnterEvent(),
	dragMoveEvent(),
	dragLeaveEvent(),
	dropEvent(),
	childEvent(),
	showEvent(),
	hideEvent(),
	customEvent().

    \row \i Change handlers \i
	enabledChange(),
	fontChange(),
	paletteChange(),
	styleChange(),
	windowActivationChange().

    \row \i System functions \i
	parentWidget(),
	topLevelWidget(),
	reparent(),
	polish(),
	winId(),
	find(),
	metric().

    \row \i What's this help \i
	customWhatsThis()

    \row \i Internal kernel<br>functions \i
	focusNextPrevChild(),
	wmapper(),
	clearWFlags(),
	getWFlags(),
	setWFlags(),
	testWFlags().

    \endtable

    Every widget's constructor accepts two or three standard arguments:
    \list 1
    \i \c{TQWidget *parent = 0} is the parent of the new widget.
    If it is 0 (the default), the new widget will be a top-level window.
    If not, it will be a child of \e parent, and be constrained by \e
    parent's geometry (unless you specify \c WType_TopLevel as
    widget flag).
    \i \c{const char *name = 0} is the widget name of the new
    widget. You can access it using name(). The widget name is little
    used by programmers but is quite useful with GUI builders such as
    \e{TQt Designer} (you can name a widget in \e{TQt Designer}, and
    connect() to it using the name in your code). The dumpObjectTree()
    debugging function also uses it.
    \i \c{WFlags f = 0} (where available) sets the widget flags; the
    default is suitable for almost all widgets, but to get, for
    example, a top-level widget without a window system frame, you
    must use special flags.
    \endlist

    The tictac/tictac.cpp example program is good example of a simple
    widget. It contains a few event handlers (as all widgets must), a
    few custom routines that are specific to it (as all useful widgets
    do), and has a few children and connections. Everything it does
    is done in response to an event: this is by far the most common way
    to design GUI applications.

    You will need to supply the content for your widgets yourself, but
    here is a brief run-down of the events, starting with the most common
    ones:

    \list

    \i paintEvent() - called whenever the widget needs to be
    repainted. Every widget which displays output must implement it,
    and it is wise \e not to paint on the screen outside
    paintEvent().

    \i resizeEvent() - called when the widget has been resized.

    \i mousePressEvent() - called when a mouse button is pressed.
    There are six mouse-related events, but the mouse press and mouse
    release events are by far the most important. A widget receives
    mouse press events when the mouse is inside it, or when it has
    grabbed the mouse using grabMouse().

    \i mouseReleaseEvent() - called when a mouse button is released.
    A widget receives mouse release events when it has received the
    corresponding mouse press event. This means that if the user
    presses the mouse inside \e your widget, then drags the mouse to
    somewhere else, then releases, \e your widget receives the release
    event. There is one exception: if a popup menu appears while the
    mouse button is held down, this popup immediately steals the mouse
    events.

    \i mouseDoubleClickEvent() - not quite as obvious as it might seem.
    If the user double-clicks, the widget receives a mouse press event
    (perhaps a mouse move event or two if they don't hold the mouse
    quite steady), a mouse release event and finally this event. It is
    \e{not possible} to distinguish a click from a double click until you've
    seen whether the second click arrives. (This is one reason why most GUI
    books recommend that double clicks be an extension of single clicks,
    rather than trigger a different action.)

    \endlist

    If your widget only contains child widgets, you probably do not need to
    implement any event handlers. If you want to detect a mouse click in
    a child widget call the child's hasMouse() function inside the
    parent widget's mousePressEvent().

    Widgets that accept keyboard input need to reimplement a few more
    event handlers:

    \list

    \i keyPressEvent() - called whenever a key is pressed, and again
    when a key has been held down long enough for it to auto-repeat.
    Note that the Tab and Shift+Tab keys are only passed to the widget
    if they are not used by the focus-change mechanisms. To force those
    keys to be processed by your widget, you must reimplement
    TQWidget::event().

    \i focusInEvent() - called when the widget gains keyboard focus
    (assuming you have called setFocusPolicy()). Well written widgets
    indicate that they own the keyboard focus in a clear but discreet
    way.

    \i focusOutEvent() - called when the widget loses keyboard focus.

    \endlist

    Some widgets will also need to reimplement some of the less common
    event handlers:

    \list

    \i mouseMoveEvent() - called whenever the mouse moves while a
    button is held down. This is useful for, for example, dragging. If
    you call setMouseTracking(true), you get mouse move events even
    when no buttons are held down. (Note that applications which make
    use of mouse tracking are often not very useful on low-bandwidth X
    connections.) (See also the \link dnd.html drag and drop\endlink
    information.)

    \i keyReleaseEvent() - called whenever a key is released, and also
    while it is held down if the key is auto-repeating. In that case
    the widget receives a key release event and immediately a key press
    event for every repeat. Note that the Tab and Shift+Tab keys are
    only passed to the widget if they are not used by the focus-change
    mechanisms. To force those keys to be processed by your widget, you
    must reimplement TQWidget::event().

    \i wheelEvent() -- called whenever the user turns the mouse wheel
    while the widget has the focus.

    \i enterEvent() - called when the mouse enters the widget's screen
    space. (This excludes screen space owned by any children of the
    widget.)

    \i leaveEvent() - called when the mouse leaves the widget's screen
    space.

    \i moveEvent() - called when the widget has been moved relative to its
    parent.

    \i closeEvent() - called when the user closes the widget (or when
    close() is called).

    \endlist

    There are also some rather obscure events. They are listed in
    \c ntqevent.h and you need to reimplement event() to handle them.
    The default implementation of event() handles Tab and Shift+Tab
    (to move the keyboard focus), and passes on most other events to
    one of the more specialized handlers above.

    When implementing a widget, there are a few more things to
    consider.

    \list

    \i In the constructor, be sure to set up your member variables
    early on, before there's any chance that you might receive an event.

    \i It is almost always useful to reimplement sizeHint() and to set
    the correct size policy with setSizePolicy(), so users of your class
    can set up layout management more easily. A size policy lets you
    supply good defaults for the layout management handling, so that
    other widgets can contain and manage yours easily. sizeHint()
    indicates a "good" size for the widget.

    \i If your widget is a top-level window, setCaption() and setIcon() set
    the title bar and icon respectively.

    \endlist

    \sa TQEvent, TQPainter, TQGridLayout, TQBoxLayout
*/


/*****************************************************************************
  Internal TQWidgetMapper class

  The purpose of this class is to map widget identifiers to TQWidget objects.
  All TQWidget objects register themselves in the TQWidgetMapper when they
  get an identifier. Widgets unregister themselves when they change ident-
  ifier or when they are destroyed. A widget identifier is really a window
  handle.

  The widget mapper is created and destroyed by the main application routines
  in the file qapp_xxx.cpp.
 *****************************************************************************/

#if defined(TQ_WS_QWS) || defined(Q_OS_TEMP)
static const int WDictSize = 163; // plenty for small devices
#else
static const int WDictSize = 1123; // plenty for 5 big complex windows
#endif

class TQWidgetMapper : public TQWidgetIntDict
{						// maps ids -> widgets
public:
    TQWidgetMapper();
   ~TQWidgetMapper();
    TQWidget *find( WId id );		// find widget
    void     insert( const TQWidget * );		// insert widget
    bool     remove( WId id );		// remove widget
private:
    WId	     cur_id;
    TQWidget *cur_widget;
};

TQWidgetMapper *TQWidget::mapper = 0;		// app global widget mapper


TQWidgetMapper::TQWidgetMapper() : TQWidgetIntDict(WDictSize)
{
    cur_id = 0;
    cur_widget = 0;
}

TQWidgetMapper::~TQWidgetMapper()
{
    clear();
}

inline TQWidget *TQWidgetMapper::find( WId id )
{
    if ( id != cur_id ) {			// need to lookup
	cur_widget = TQWidgetIntDict::find((long)id);
	if ( cur_widget )
	    cur_id = id;
	else
	    cur_id = 0;
    }
    return cur_widget;
}

inline void TQWidgetMapper::insert( const TQWidget *widget )
{
    TQWidgetIntDict::insert((long)widget->winId(),widget);
}

inline bool TQWidgetMapper::remove( WId id )
{
    if ( cur_id == id ) {			// reset current widget
	cur_id = 0;
	cur_widget = 0;
    }
    return TQWidgetIntDict::remove((long)id);
}


/*****************************************************************************
  TQWidget utility functions
 *****************************************************************************/

static TQFont tqt_naturalWidgetFont( TQWidget* w ) {
    TQFont naturalfont = TQApplication::font( w );
    if ( ! w->isTopLevel() ) {
	if ( ! naturalfont.isCopyOf( TQApplication::font() ) )
	    naturalfont = naturalfont.resolve( w->parentWidget()->font() );
	else
	    naturalfont = w->parentWidget()->font();
    }
    return naturalfont;
}

#ifndef TQT_NO_PALETTE
static TQPalette tqt_naturalWidgetPalette( TQWidget* w ) {
    TQPalette naturalpalette = TQApplication::palette( w );
    if ( !w->isTopLevel() && naturalpalette.isCopyOf( TQApplication::palette() ) )
	naturalpalette = w->parentWidget()->palette();
    return naturalpalette;
}
#endif

TQSize tqt_naturalWidgetSize( TQWidget *w ) {
    TQSize s = w->sizeHint();
    TQSizePolicy::ExpandData exp;
#ifndef TQT_NO_LAYOUT
    if ( w->layout() ) {
	if ( w->layout()->hasHeightForWidth() )
	    s.setHeight( w->layout()->totalHeightForWidth( s.width() ) );
	exp = w->layout()->expanding();
    } else
#endif
    {
	if ( w->sizePolicy().hasHeightForWidth() )
	    s.setHeight( w->heightForWidth( s.width() ) );
	exp = w->sizePolicy().expanding();
    }
    if ( exp & TQSizePolicy::Horizontally )
	s.setWidth( TQMAX( s.width(), 200 ) );
    if ( exp & TQSizePolicy::Vertically )
	s.setHeight( TQMAX( s.height(), 150 ) );
#if defined(TQ_WS_X11)
    TQRect screen = TQApplication::desktop()->screenGeometry( w->x11Screen() );
#else // all others
    TQRect screen = TQApplication::desktop()->screenGeometry( w->pos() );
#endif
    s.setWidth( TQMIN( s.width(), screen.width()*2/3 ) );
    s.setHeight( TQMIN( s.height(), screen.height()*2/3 ) );
    return s;
}

/*****************************************************************************
  TQWidget member functions
 *****************************************************************************/

/*
    Widget state flags:
  \list
  \i WState_Created The widget has a valid winId().
  \i WState_Disabled The widget does not receive any mouse or keyboard
  events.
  \i WState_ForceDisabled The widget is explicitly disabled, i.e. it
  will remain disabled even when all its ancestors are set to the enabled
  state. This implies WState_Disabled.
  \i WState_Visible The widget is currently visible.
  \i WState_ForceHide The widget is explicitly hidden, i.e. it won't
  become visible unless you call show() on it. WState_ForceHide
  implies !WState_Visible.
  \i WState_OwnCursor A cursor has been set for this widget.
  \i WState_MouseTracking Mouse tracking is enabled.
  \i WState_CompressKeys Compress keyboard events.
  \i WState_BlockUpdates Repaints and updates are disabled.
  \i WState_InPaintEvent Currently processing a paint event.
  \i WState_Reparented The widget has been reparented.
  \i WState_ConfigPending A configuration (resize/move) event is pending.
  \i WState_Resized The widget has been resized.
  \i WState_AutoMask The widget has an automatic mask, see setAutoMask().
  \i WState_Polished The widget has been "polished" (i.e. late
  initialization) by a TQStyle.
  \i WState_DND The widget supports drag and drop, see setAcceptDrops().
  \i WState_Exposed the widget was finally exposed (X11 only,
      helps avoid paint event doubling).
  \i WState_HasMouse The widget is under the mouse cursor.
  \endlist
*/

/*! \enum TQt::WFlags
    \internal */
/*! \enum TQt::WState
    \internal */

/*!
    \enum TQt::WidgetFlags

    \keyword widget flag

    This enum type is used to specify various window-system properties
    for the widget. They are fairly unusual but necessary in a few
    cases. Some of these flags depend on whether the underlying window
    manager supports them. (See the \link toplevel-example.html
    toplevel example\endlink for an explanation and example of their
    use.)

    The main types are

    \value WType_TopLevel  indicates that this widget is a top-level
    widget, usually with a window-system frame and so on.

    \value WType_Dialog  indicates that this widget is a top-level
    window that should be decorated as a dialog (i.e. typically no
    maximize or minimize buttons in the title bar). If you want to use
    it as a modal dialog it should be launched from another window, or
    have a parent and this flag should be combined with \c WShowModal.
    If you make it modal, the dialog will prevent other top-level
    windows in the application from getting any input. \c WType_Dialog
    implies \c WType_TopLevel. We refer to a top-level window that has
    a parent as a \e secondary window. (See also \c WGroupLeader.)

    \value WType_Popup  indicates that this widget is a popup
    top-level window, i.e. that it is modal, but has a window system
    frame appropriate for popup menus. \c WType_Popup implies
    WType_TopLevel.

    \value WType_Desktop  indicates that this widget is the desktop.
    See also \c WPaintDesktop below. \c WType_Desktop implies \c
    WType_TopLevel.

    There are also a number of flags which you can use to customize
    the appearance of top-level windows. These have no effect on other
    windows:

    \value WStyle_Customize  indicates that the \c WStyle_* flags
    should be used to build the window instead of the default flags.

    \value WStyle_NormalBorder  gives the window a normal border.
    This cannot be combined with \c WStyle_DialogBorder or \c
    WStyle_NoBorder.

    \value WStyle_DialogBorder  gives the window a thin dialog border.
    This cannot be combined with \c WStyle_NormalBorder or \c
    WStyle_NoBorder.

    \value WStyle_NoBorder  produces a borderless window. Note that
    the user cannot move or resize a borderless window via the window
    system. This cannot be combined with \c WStyle_NormalBorder or \c
    WStyle_DialogBorder. On Windows, the flag works fine. On X11, the
    result of the flag is dependent on the window manager and its
    ability to understand MOTIF and/or NETWM hints: most existing
    modern window managers can handle this. With \c WX11BypassWM, you
    can bypass the window manager completely. This results in a
    borderless window that is not managed at all (i.e. no keyboard
    input unless you call setActiveWindow() manually).

    \value WStyle_NoBorderEx  this value is obsolete. It has the same
    effect as using \c WStyle_NoBorder.

    \value WStyle_Title  gives the window a title bar.

    \value WStyle_SysMenu  adds a window system menu.

    \value WStyle_Minimize  adds a minimize button. Note that on
    Windows this has to be combined with \c WStyle_SysMenu for it to
    work.

    \value WStyle_Maximize  adds a maximize button. Note that on
    Windows this has to be combined with \c WStyle_SysMenu for it to work.

    \value WStyle_MinMax  is equal to \c
    WStyle_Minimize|WStyle_Maximize. Note that on Windows this has to
    be combined with \c WStyle_SysMenu to work.

    \value WStyle_ContextHelp  adds a context help button to dialogs.

    \value WStyle_Tool  makes the window a tool window. A tool window
    is often a small window with a smaller than usual title bar and
    decoration, typically used for collections of tool buttons. It
    there is a parent, the tool window will always be kept on top of
    it. If there isn't a parent, you may consider passing \c
    WStyle_StaysOnTop as well. If the window system supports it, a
    tool window can be decorated with a somewhat lighter frame. It can
    also be combined with \c WStyle_NoBorder.

    \value WStyle_StaysOnTop  informs the window system that the
    window should stay on top of all other windows. Note that on some
    window managers on X11 you also have to pass \c WX11BypassWM for
    this flag to work correctly.

    \value WStyle_Dialog  indicates that the window is a logical
    subwindow of its parent (i.e. a dialog). The window will not get
    its own taskbar entry and will be kept on top of its parent by the
    window system. Usually it will also be minimized when the parent
    is minimized. If not customized, the window is decorated with a
    slightly simpler title bar. This is the flag TQDialog uses.

    \value WStyle_Splash  indicates that the window is a splash screen.
    On X11, we try to follow NETWM standard for a splash screen window if the
    window manager supports is otherwise it is equivalent to \c WX11BypassWM. On
    other platforms, it is equivalent to \c WStyle_NoBorder \c | \c WMacNoSheet \c |
    \c WStyle_Tool \c | \c WWinOwnDC

    Modifier flags:

    \value WDestructiveClose  makes TQt delete this widget when the
    widget has accepted closeEvent(), or when the widget tried to
    ignore closeEvent() but could not.

    \value WPaintDesktop  gives this widget paint events for the
    desktop.

    \value WPaintUnclipped  makes all painters operating on this
    widget unclipped. Children of this widget or other widgets in
    front of it do not clip the area the painter can paint on.

    \value WPaintClever  indicates that TQt should \e not try to
    optimize repainting for the widget, but instead pass on window
    system repaint events directly. (This tends to produce more events
    and smaller repaint regions.)

    \value WMouseNoMask  indicates that even if the widget has a mask,
    it wants mouse events for its entire rectangle.

    \value WStaticContents  indicates that the widget contents are
    north-west aligned and static. On resize, such a widget will
    receive paint events only for the newly visible part of itself.

    \value WNoAutoErase indicates that the widget paints all its
    pixels. Updating, resizing, scrolling and focus changes should
    therefore not erase the widget. This allows smart-repainting to
    avoid flicker.

    \value WResizeNoErase  this value is obsolete; use WNoAutoErase instead.
    \value WRepaintNoErase  this value is obsolete; use WNoAutoErase instead.
    \value WGroupLeader  makes this window a group leader. A group
    leader should \e not have a parent (i.e. it should be a top-level
    window). Any decendant windows (direct or indirect) of a group
    leader are in its group; other windows are not. If you show a
    secondary window from the group (i.e. show a window whose top-most
    parent is a group leader), that window will be modal with respect
    to the other windows in the group, but modeless with respect to
    windows in other groups.

    Miscellaneous flags

    \value WShowModal see WType_Dialog

    Internal flags.

    \value WNoMousePropagation
    \value WStaticContents
    \value WStyle_Reserved
    \value WSubWindow
    \value WType_Modal
    \value WWinOwnDC
    \value WX11BypassWM
    \value WMacNoSheet
    \value WMacDrawer
    \value WStyle_Mask
    \value WType_Mask

*/

/*!
    \enum TQt::NETWMFlags

    \keyword NETWM flag

    This enum type is used to specify various NETWM properties
    under X11 and similar systems.

    The main types are

    \value WX11DisableMove
    \value WX11DisableClose
    \value WX11DisableResize
    \value WX11DisableMinimize
    \value WX11DisableMaximize
    \value WX11DisableShade

*/

/*!
    \enum TQt::WidgetState

    Internal flags.

    \value WState_Created
    \value WState_Disabled
    \value WState_Visible
    \value WState_ForceHide
    \value WState_OwnCursor
    \value WState_MouseTracking
    \value WState_CompressKeys
    \value WState_BlockUpdates
    \value WState_InPaintEvent
    \value WState_Reparented
    \value WState_ConfigPending
    \value WState_Resized
    \value WState_AutoMask
    \value WState_Polished
    \value WState_DND
    \value WState_Reserved0 \e internal
    \value WState_CreatedHidden
    \value WState_Maximized
    \value WState_Minimized
    \value WState_ForceDisabled
    \value WState_Exposed
    \value WState_HasMouse
    \value WState_CreatedHidden
    \value WState_OwnSizePolicy
    \value WState_FullScreen
*/


/*!
    \enum TQt::WindowState

    \keyword window state

    This enum type is used to specify the current state of a top-level
    window.

    The states are

    \value WindowNoState   The window has no state set (in normal state).
    \value WindowMinimized The window is minimized (i.e. iconified).
    \value WindowMaximized The window is maximized with a frame around it.
    \value WindowFullScreen The window fills the entire screen without any frame around it.
    \value WindowActive The window is the active window, i.e. it has keyboard focus.

*/

/*!
    Constructs a widget which is a child of \a parent, with the name
    \a name, widget flags set to \a f, and NETWM flags set to \a n.

    If \a parent is 0, the new widget becomes a top-level window. If
    \a parent is another widget, this widget becomes a child window
    inside \a parent. The new widget is deleted when its \a parent is
    deleted.

    The \a name is sent to the TQObject constructor.

    The widget flags argument, \a f, is normally 0, but it can be set
    to customize the window frame of a top-level widget (i.e. \a
    parent must be 0). To customize the frame, set the \c
    WStyle_Customize flag OR'ed with any of the \l TQt::WidgetFlags.

    If you add a child widget to an already visible widget you must
    explicitly show the child to make it visible.

    Note that the X11 version of TQt may not be able to deliver all
    combinations of style flags on all systems. This is because on
    X11, TQt can only ask the window manager, and the window manager
    can override the application's settings. On Windows, TQt can set
    whatever flags you want.

    Example:
    \code
    TQLabel *splashScreen = new TQLabel( 0, "mySplashScreen",
				WStyle_Customize | WStyle_Splash );
    \endcode
*/

TQWidget::TQWidget( TQWidget *parent, const char *name, WFlags f, NFlags n )
    : TQObject( parent, name ), TQPaintDevice( TQInternal::Widget )
{
#if defined(QT_CHECK_STATE) && !defined(TQ_WS_WIN)
    if ( tqApp->type() == TQApplication::Tty ) {
	tqWarning( "TQWidget: Cannot create a TQWidget when no GUI "
		  "is being used" );
    }
#endif

#if defined(TQT_THREAD_SUPPORT) && defined(QT_CHECK_STATE)
    if (TQThread::currentThreadObject() != TQApplication::guiThread()) {
	tqFatal( "TQWidget: Cannot create a TQWidget outside of the main GUI thread" );
    }
#endif

    fstrut_dirty = 1;

    isWidget = true;				// is a widget
    winid = 0;					// default attributes
    widget_state = 0;
    widget_flags = f;
    netwm_flags = n;
    focus_policy = 0;
    own_font = 0;
    own_palette = 0;
    sizehint_forced = 0;
    is_closing = 0;
    in_show = 0;
    in_show_maximized = 0;
    im_enabled = false;
#ifndef TQT_NO_LAYOUT
    lay_out = 0;
#endif
    extra = 0;					// no extra widget info
#ifndef TQT_NO_PALETTE
    bg_col = pal.active().background();		// default background color
#endif
    create();					// platform-dependent init
#ifndef TQT_NO_PALETTE
    pal = isTopLevel() ? TQApplication::palette() : parentWidget()->palette();
#endif
    if ( ! isTopLevel() )
	fnt = parentWidget()->font();
#if defined(TQ_WS_X11)
    fnt.x11SetScreen( x11Screen() );
#endif // TQ_WS_X11

    if ( !isDesktop() )
	setBackgroundFromMode(); //### parts of this are done in create but not all (see reparent(...) )
    // make sure move/resize events are sent to all widgets
    TQApplication::postEvent( this, new TQMoveEvent( crect.topLeft(),
						   crect.topLeft() ) );
    TQApplication::postEvent( this, new TQResizeEvent(crect.size(),
						    crect.size()) );
    if ( isTopLevel() ) {
	setWState( WState_ForceHide | WState_CreatedHidden );
	TQFocusData *fd = focusData( true );
	if ( fd->focusWidgets.findRef(this) < 0 )
	    fd->focusWidgets.append( this );
    } else {
	// propagate enabled state
	if ( !parentWidget()->isEnabled() )
	    setWState( WState_Disabled );
	// new widgets do not show up in already visible parents
	if ( parentWidget()->isVisible() )
	    setWState( WState_ForceHide | WState_CreatedHidden );
    }
    if ( ++instanceCounter > maxInstances )
    	maxInstances = instanceCounter;
}

/*!
    Destroys the widget.

    All this widget's children are deleted first. The application
    exits if this widget is the main widget.
*/

TQWidget::~TQWidget()
{
#if defined (QT_CHECK_STATE)
    if ( paintingActive() )
	tqWarning( "%s (%s): deleted while being painted", className(), name() );
#endif

    // Remove myself and all children from the can-take-focus list
    TQFocusData *f = focusData( false );
    if ( f ) {
	TQPtrListIterator<TQWidget> it(f->focusWidgets);
	TQWidget *w;
	while ( (w = it.current()) ) {
	    ++it;
	    TQWidget * p = w;
	    while( p && p != this )
		p = p->parentWidget();
	    if ( p ) // my descendant
		f->focusWidgets.removeRef( w );
	}
    }
    --instanceCounter;

    if ( TQApplication::main_widget == this ) {	// reset main widget
	TQApplication::main_widget = 0;
	if (tqApp)
	    tqApp->quit();
    }

    if ( tqApp && hasFocus() )
        clearFocus();

    if ( isTopLevel() && isShown() && winId() )
	hide();

    // A parent widget must destroy all its children before destroying itself
    if ( childObjects ) {			// delete children objects
	TQObjectListIt it(*childObjects);
	TQObject *obj;
	while ( (obj=it.current()) ) {
	    ++it;
	    obj->parentObj = 0;
	    childObjects->removeRef( obj );
	    delete obj;
	}
	delete childObjects;
	childObjects = 0;
    }

    if ( tqApp )
    {
	TQApplication::removePostedEvents( this );
    }

    destroy();					// platform-dependent cleanup
    if ( extra )
	deleteExtra();
}

int TQWidget::instanceCounter = 0;  // Current number of widget instances
int TQWidget::maxInstances = 0;     // Maximum number of widget instances

/*!
  \internal
  Creates the global widget mapper.
  The widget mapper converts window handles to widget pointers.
  \sa destroyMapper()
*/

void TQWidget::createMapper()
{
    mapper = new TQWidgetMapper;
    TQ_CHECK_PTR( mapper );
}

/*!
  \internal
  Destroys the global widget mapper.
  \sa createMapper()
*/

void TQWidget::destroyMapper()
{
    if ( !mapper )				// already gone
	return;
    TQWidgetIntDictIt it( *((TQWidgetIntDict*)mapper) );
    TQWidgetMapper * myMapper = mapper;
    mapper = 0;
    TQWidget *w;
    while ( (w=it.current()) ) {		// remove parents widgets
	++it;
	if ( !w->parentObj )			// widget is a parent
	    w->destroy( true, true );
    }
    delete myMapper;
}


static TQWidgetList *wListInternal( TQWidgetMapper *mapper, bool onlyTopLevel )
{
    TQWidgetList *list = new TQWidgetList;
    TQ_CHECK_PTR( list );
    if ( mapper ) {
	TQWidget *w;
	TQWidgetIntDictIt it( *((TQWidgetIntDict*)mapper) );
	while ( (w=it.current()) ) {
	    ++it;
	    if ( !onlyTopLevel || w->isTopLevel() )
		list->append( w );
	}
    }
    return list;
}

/*!
  \internal
  Returns a list of all widgets.
  \sa tlwList(), TQApplication::allWidgets()
*/

TQWidgetList *TQWidget::wList()
{
    return wListInternal( mapper, false );
}

/*!
  \internal
  Returns a list of all top level widgets.
  \sa wList(), TQApplication::topLevelWidgets()
*/

TQWidgetList *TQWidget::tlwList()
{
    return wListInternal( mapper, true );
}


void TQWidget::setWinId( WId id )		// set widget identifier
{
    if ( !mapper )				// mapper destroyed
	return;
    if ( winid )
	mapper->remove( winid );
    winid = id;
#if defined(TQ_WS_X11)
    hd = id;					// X11: hd == ident
#endif
    if ( id )
	mapper->insert( this );
}


/*!
  \internal
  Returns a pointer to the block of extra widget data.
*/

TQWExtra *TQWidget::extraData()
{
    return extra;
}


/*!
  \internal
  Returns a pointer to the block of extra top level widget data.

  This data is guaranteed to exist for top level widgets.
*/

TQTLWExtra *TQWidget::topData()
{
    createTLExtra();
    return extra->topextra;
}


void TQWidget::createTLExtra()
{
    if ( !extra )
	createExtra();
    if ( !extra->topextra ) {
	TQTLWExtra* x = extra->topextra = new TQTLWExtra;
#if defined( TQ_WS_WIN ) || defined( TQ_WS_MAC )
	x->opacity = 255;
#endif
#ifndef TQT_NO_WIDGET_TOPEXTRA
	x->icon = 0;
#endif
	x->focusData = 0;
	x->fleft = x->fright = x->ftop = x->fbottom = 0;
	x->incw = x->inch = 0;
	x->basew = x->baseh = 0;
	x->normalGeometry = TQRect(0,0,-1,-1);
#if defined(TQ_WS_X11)
	x->embedded = 0;
	x->parentWinId = 0;
	x->spont_unmapped = 0;
	x->dnd = 0;
	x->uspos = 0;
	x->ussize = 0;
#endif
	x->savedFlags = 0;
#if defined(TQ_WS_QWS) && !defined(TQT_NO_QWS_MANAGER)
	x->decor_allocated_region = TQRegion();
	x->qwsManager = 0;
#endif
	createTLSysExtra();
    }
}

/*!
  \internal
  Creates the widget extra data.
*/

void TQWidget::createExtra()
{
    if ( !extra ) {				// if not exists
	extra = new TQWExtra;
	TQ_CHECK_PTR( extra );
	extra->minw = extra->minh = 0;
	extra->maxw = extra->maxh = TQWIDGETSIZE_MAX;
	extra->bg_pix = 0;
	extra->focus_proxy = 0;
#ifndef TQT_NO_CURSOR
	extra->curs = 0;
#endif
	extra->topextra = 0;
	extra->bg_mode = PaletteBackground;
	extra->bg_mode_visual = PaletteBackground;
	extra->bg_origin = WidgetOrigin;
#ifndef TQT_NO_STYLE
	extra->style = 0;
#endif
	extra->size_policy = TQSizePolicy( TQSizePolicy::Preferred,
					  TQSizePolicy::Preferred );

	extra->m_ceData = NULL;

	createSysExtra();
    }
}


/*!
  \internal
  Deletes the widget extra data.
*/

void TQWidget::deleteExtra()
{
    if ( extra ) {				// if exists
	delete extra->m_ceData;
	delete extra->bg_pix;
#ifndef TQT_NO_CURSOR
	delete extra->curs;
#endif
	deleteSysExtra();
	if ( extra->topextra ) {
	    deleteTLSysExtra();
#ifndef TQT_NO_WIDGET_TOPEXTRA
	    delete extra->topextra->icon;
#endif
	    delete extra->topextra->focusData;
#if defined(TQ_WS_QWS) && !defined(TQT_NO_QWS_MANAGER)
	    delete extra->topextra->qwsManager;
#endif
	    delete extra->topextra;
	}
	delete extra;
	// extra->xic destroyed in TQWidget::destroy()
	extra = 0;
    }
}


/*!
  \internal
  This function is called when a widget is hidden or destroyed.
  It resets some application global pointers that should only refer active,
  visible widgets.
*/

void TQWidget::deactivateWidgetCleanup()
{
    // If this was the active application window, reset it
    if ( this == TQApplication::active_window )
	tqApp->setActiveWindow( 0 );
    // If the is the active mouse press widget, reset it
#ifdef TQ_WS_MAC
    extern TQGuardedPtr<TQWidget> tqt_button_down;
#else
    extern TQWidget *tqt_button_down;
#endif
    if ( this == (TQWidget *)tqt_button_down )
	tqt_button_down = 0;
}


/*!
    Returns a pointer to the widget with window identifer/handle \a
    id.

    The window identifier type depends on the underlying window
    system, see \c ntqwindowdefs.h for the actual definition. If there
    is no widget with this identifier, 0 is returned.
*/

TQWidget *TQWidget::find( WId id )
{
    return mapper ? mapper->find( id ) : 0;
}

/*!
  \fn TQWidgetMapper *TQWidget::wmapper()
  \internal
  Returns a pointer to the widget mapper.

  The widget mapper is an internal dictionary that is used to map from
  window identifiers/handles to widget pointers.
  \sa find(), id()
*/

/*!
    \fn WFlags TQWidget::getWFlags() const

    Returns the widget flags for this this widget.

    Widget flags are a combination of \l{TQt::WidgetFlags}.

    \sa testWFlags(), setWFlags(), clearWFlags()
*/

/*!
    \fn void TQWidget::setWFlags( WFlags f )

    Sets the widget flags \a f.

    Widget flags are a combination of \l{TQt::WidgetFlags}.

    \sa testWFlags(), getWFlags(), clearWFlags()
*/

/*!
    \fn void TQWidget::clearWFlags( WFlags f )

    Clears the widget flags \a f.

    Widget flags are a combination of \l{TQt::WidgetFlags}.

    \sa testWFlags(), getWFlags(), setWFlags()
*/



/*!
    \fn WId TQWidget::winId() const

    Returns the window system identifier of the widget.

    Portable in principle, but if you use it you are probably about to
    do something non-portable. Be careful.

    \sa find()
*/

#ifndef TQT_NO_STYLE
/*!
    Returns the GUI style for this widget

    \sa TQWidget::setStyle(), TQApplication::setStyle(), TQApplication::style()
*/

TQStyle& TQWidget::style() const
{
    if ( extra && extra->style )
	return *extra->style;
    TQStyle &ret = tqApp->style();
    return ret;
}

/*!
    Sets the widget's GUI style to \a style. Ownership of the style
    object is not transferred.

    If no style is set, the widget uses the application's style,
    TQApplication::style() instead.

    Setting a widget's style has no effect on existing or future child
    widgets.

    \warning This function is particularly useful for demonstration
    purposes, where you want to show TQt's styling capabilities. Real
    applications should avoid it and use one consistent GUI style
    instead.

    \sa style(), TQStyle, TQApplication::style(), TQApplication::setStyle()
*/

void TQWidget::setStyle( TQStyle *style )
{
    TQStyle& old  = TQWidget::style();
    createExtra();
    extra->style = style;
    if ( !testWFlags(WType_Desktop) // (except desktop)
	 && testWState(WState_Polished)) { // (and have been polished)
	old.unPolish( this );
	TQWidget::style().polish( this );
    }
    styleChange( old );
}

/*!
    \overload

    Sets the widget's GUI style to \a style using the TQStyleFactory.
*/
TQStyle* TQWidget::setStyle( const TQString &style )
{
    TQStyle *s = TQStyleFactory::create( style );
    setStyle( s );
    return s;
}

/*!
    This virtual function is called when the style of the widgets
    changes. \a oldStyle is the previous GUI style; you can get the
    new style from style().

    Reimplement this function if your widget needs to know when its
    GUI style changes. You will almost certainly need to update the
    widget using update().

    The default implementation updates the widget including its
    geometry.

    \sa TQApplication::setStyle(), style(), update(), updateGeometry()
*/

void TQWidget::styleChange( TQStyle& /* oldStyle */ )
{
    update();
    updateGeometry();
}

#endif

/*!
    \property TQWidget::isTopLevel
    \brief whether the widget is a top-level widget

    A top-level widget is a widget which usually has a frame and a
    \link TQWidget::caption caption (title)\endlink. \link
    TQWidget::isPopup() Popup\endlink and \link TQWidget::isDesktop()
    desktop\endlink widgets are also top-level widgets.

    A top-level widget can have a \link TQWidget::parentWidget() parent
    widget\endlink. It will then be grouped with its parent and deleted
    when the parent is deleted, minimized when the parent is minimized
    etc. If supported by the window manager, it will also have a
    common taskbar entry with its parent.

    TQDialog and TQMainWindow widgets are by default top-level, even if
    a parent widget is specified in the constructor. This behavior is
    specified by the \c WType_TopLevel widget flag.

    \sa topLevelWidget(), isDialog(), isModal(), isPopup(), isDesktop(), parentWidget()
*/

/*!
    \property TQWidget::isDialog
    \brief whether the widget is a dialog widget

    A dialog widget is a secondary top-level widget, i.e. a top-level
    widget with a parent.

    \sa isTopLevel(), TQDialog
*/

/*!
    \property TQWidget::isPopup
    \brief whether the widget is a popup widget

    A popup widget is created by specifying the widget flag \c
    WType_Popup to the widget constructor. A popup widget is also a
    top-level widget.

    \sa isTopLevel()
*/

/*!
    \property TQWidget::isDesktop
    \brief whether the widget is a desktop widget, i.e. represents the desktop

    A desktop widget is also a top-level widget.

    \sa isTopLevel(), TQApplication::desktop()
*/

/*!
    \property TQWidget::isModal
    \brief whether the widget is a modal widget

    This property only makes sense for top-level widgets. A modal
    widget prevents widgets in all other top-level widgets from
    getting any input.

    \sa isTopLevel(), isDialog(), TQDialog
*/

/*!
    \property TQWidget::underMouse
    \brief whether the widget is under the mouse cursor

    This value is not updated properly during drag and drop
    operations.

    \sa TQEvent::Enter, TQEvent::Leave
*/

/*!
    \property TQWidget::minimized
    \brief whether this widget is minimized (iconified)

    This property is only relevant for top-level widgets.

    \sa showMinimized(), visible, show(), hide(), showNormal(), maximized
*/
bool TQWidget::isMinimized() const
{ return testWState(WState_Minimized); }

/*!
    Shows the widget minimized, as an icon.

    Calling this function only affects \link isTopLevel() top-level
    widgets\endlink.

    \sa showNormal(), showMaximized(), show(), hide(), isVisible(),
    isMinimized()
*/
void TQWidget::showMinimized()
{
    bool isMin = isMinimized();
    if (isMin && isVisible()) return;

    if (!isMin)
        setWindowState((windowState() & ~WindowActive) | WindowMinimized);
    show();
    if (!isTopLevel())
	TQApplication::sendPostedEvents(this, TQEvent::ShowMinimized);
}

/*!
    \property TQWidget::maximized
    \brief whether this widget is maximized

    This property is only relevant for top-level widgets.

    Note that due to limitations in some window-systems, this does not
    always report the expected results (e.g. if the user on X11
    maximizes the window via the window manager, TQt has no way of
    distinguishing this from any other resize). This is expected to
    improve as window manager protocols evolve.

    \sa windowState(), showMaximized(), visible, show(), hide(), showNormal(), minimized
*/
bool TQWidget::isMaximized() const
{ return testWState(WState_Maximized); }



/*!  Returns the current window state. The window state is a OR'ed
  combination of TQt::WindowState: \c WindowMinimized, \c
  WindowMaximized, \c WindowFullScreen and \c WindowActive.

  \sa TQt::WindowState setWindowState()
 */
uint TQWidget::windowState() const
{
    uint state = 0;
    if (testWState(WState_Minimized))
	state |= WindowMinimized;
    if (testWState(WState_Maximized))
	state |= WindowMaximized;
    if (testWState(WState_FullScreen))
	state |= WindowFullScreen;
    if (isActiveWindow())
	state |= WindowActive;
    return state;
}

/*!
  \fn void TQWidget::setWindowState(uint windowState)

  Sets the window state to \a windowState. The window state is a OR'ed
  combination of TQt::WindowState: \c WindowMinimized, \c
  WindowMaximized, \c WindowFullScreen and \c WindowActive.

  If the window is not visible (i.e. isVisible() returns false), the
  window state will take effect when show() is called. For visible
  windows, the change is immediate. For example, to toggle between
  full-screen and mormal mode, use the following code:

  \code
	w->setWindowState(w->windowState() ^ WindowFullScreen);
  \endcode

  In order to restore and activate a minimized window (while
  preserving its maximized and/or full-screen state), use the following:

  \code
	w->setWindowState(w->windowState() & ~WindowMinimized | WindowActive);
  \endcode

  Note: On some window systems \c WindowActive is not immediate, and may be
  ignored in certain cases.

  \sa TQt::WindowState windowState()
*/

/*!
    \property TQWidget::fullScreen
    \brief whether the widget is full screen

    \sa windowState(), minimized, maximized
*/
bool TQWidget::isFullScreen() const
{ return testWState(WState_FullScreen); }

/*!
    Shows the widget in full-screen mode.

    Calling this function only affects top-level widgets.

    To return from full-screen mode, call showNormal().

    Full-screen mode works fine under Windows, but has certain
    problems under X. These problems are due to limitations of the
    ICCCM protocol that specifies the communication between X11
    clients and the window manager. ICCCM simply does not understand
    the concept of non-decorated full-screen windows. Therefore, the
    best we can do is to request a borderless window and place and
    resize it to fill the entire screen. Depending on the window
    manager, this may or may not work. The borderless window is
    requested using MOTIF hints, which are at least partially
    supported by virtually all modern window managers.

    An alternative would be to bypass the window manager entirely and
    create a window with the WX11BypassWM flag. This has other severe
    problems though, like totally broken keyboard focus and very
    strange effects on desktop changes or when the user raises other
    windows.

    X11 window managers that follow modern post-ICCCM specifications
    support full-screen mode properly.

    \sa showNormal(), showMaximized(), show(), hide(), isVisible()
*/
void TQWidget::showFullScreen()
{
    bool isFull = isFullScreen();
    if (isFull && isVisible())
	return;

    if (!isFull)
	setWindowState(windowState() | WindowFullScreen);
    show();
    if (!isTopLevel())
	TQApplication::sendPostedEvents(this, TQEvent::ShowFullScreen);
    setActiveWindow();
}

/*!
    Shows the widget maximized.

    Calling this function only affects \link isTopLevel() top-level
    widgets\endlink.

    On X11, this function may not work properly with certain window
    managers. See the \link geometry.html Window Geometry
    documentation\endlink for an explanation.

    \sa setWindowState(), showNormal(), showMinimized(), show(), hide(), isVisible()
*/
void TQWidget::showMaximized()
{
    if (isMaximized() && isVisible() && !isMinimized())
	return;

    setWindowState((windowState() & ~WindowMinimized) | WindowMaximized);
    show();
    if (!isTopLevel())
	TQApplication::sendPostedEvents(this, TQEvent::ShowMaximized);
}

/*!
    Restores the widget after it has been maximized or minimized.

    Calling this function only affects \link isTopLevel() top-level
    widgets\endlink.

    \sa setWindowState(), showMinimized(), showMaximized(), show(), hide(), isVisible()
*/
void TQWidget::showNormal()
{
    setWindowState(WindowNoState);
    show();
    if (!isTopLevel())
	TQApplication::sendPostedEvents(this, TQEvent::ShowNormal);
}

/*!
    Returns true if this widget would become enabled if \a ancestor is
    enabled; otherwise returns false.

    This is the case if neither the widget itself nor every parent up
    to but excluding \a ancestor has been explicitly disabled.

    isEnabledTo(0) is equivalent to isEnabled().

    \sa setEnabled() enabled
*/

bool TQWidget::isEnabledTo( TQWidget* ancestor ) const
{
    const TQWidget * w = this;
    while ( w && !w->testWState(WState_ForceDisabled)
	    && !w->isTopLevel()
	    && w->parentWidget()
	    && w->parentWidget() != ancestor )
	w = w->parentWidget();
    return !w->testWState( WState_ForceDisabled );
}


/*!
  \fn bool TQWidget::isEnabledToTLW() const
  \obsolete

  This function is deprecated. It is equivalent to isEnabled()
*/

/*!
    \property TQWidget::enabled
    \brief whether the widget is enabled

    An enabled widget receives keyboard and mouse events; a disabled
    widget does not. In fact, an enabled widget only receives keyboard
    events when it is in focus.

    Some widgets display themselves differently when they are
    disabled. For example a button might draw its label grayed out. If
    your widget needs to know when it becomes enabled or disabled, you
    can reimplement the enabledChange() function.

    Disabling a widget implicitly disables all its children. Enabling
    respectively enables all child widgets unless they have been
    explicitly disabled.

    \sa isEnabled(), isEnabledTo(), TQKeyEvent, TQMouseEvent, enabledChange()
*/
void TQWidget::setEnabled( bool enable )
{
    if ( enable )
	clearWState( WState_ForceDisabled );
    else
	setWState( WState_ForceDisabled );

    if ( !isTopLevel() && parentWidget() &&
	 !parentWidget()->isEnabled() && enable )
	return; // nothing we can do

    if ( enable ) {
	if ( testWState(WState_Disabled) ) {
	    clearWState( WState_Disabled );
	    setBackgroundFromMode();
	    enabledChange( !enable );
	    if ( children() ) {
		TQObjectListIt it( *children() );
		TQWidget *w;
		while( (w = (TQWidget *)it.current()) != 0 ) {
		    ++it;
		    if ( w->isWidgetType() &&
			 !w->testWState( WState_ForceDisabled ) )
			w->setEnabled( true );
		}
	    }
	}
    } else {
	if ( !testWState(WState_Disabled) ) {
            if (focusWidget() == this) {
                bool parentIsEnabled = (!parentWidget() || parentWidget()->isEnabled());
                if (!parentIsEnabled || !focusNextPrevChild(true))
		    clearFocus();
            }
	    setWState( WState_Disabled );
	    setBackgroundFromMode();
	    enabledChange( !enable );
	    if ( children() ) {
		TQObjectListIt it( *children() );
		TQWidget *w;
		while( (w = (TQWidget *)it.current()) != 0 ) {
		    ++it;
		    if ( w->isWidgetType() && w->isEnabled() ) {
			w->setEnabled( false );
			w->clearWState( WState_ForceDisabled );
		    }
		}
	    }
	}
    }
#if defined(TQ_WS_X11)
    if ( testWState( WState_OwnCursor ) ) {
	// enforce the windows behavior of clearing the cursor on
	// disabled widgets

	extern void tqt_x11_enforce_cursor( TQWidget * w ); // defined in qwidget_x11.cpp
	tqt_x11_enforce_cursor( this );
    }
#endif
#ifdef TQ_WS_WIN
    TQInputContext::enable( this, im_enabled & !((bool)testWState(WState_Disabled)) );
#endif
}

/*!
    Disables widget input events if \a disable is true; otherwise
    enables input events.

    See the \l enabled documentation for more information.

    \sa isEnabledTo(), TQKeyEvent, TQMouseEvent, enabledChange()
*/
void TQWidget::setDisabled( bool disable )
{
    setEnabled( !disable );
}

/*!
    \fn void TQWidget::enabledChange( bool oldEnabled )

    This virtual function is called from setEnabled(). \a oldEnabled
    is the previous setting; you can get the new setting from
    isEnabled().

    Reimplement this function if your widget needs to know when it
    becomes enabled or disabled. You will almost certainly need to
    update the widget using update().

    The default implementation repaints the visible part of the
    widget.

    \sa setEnabled(), isEnabled(), repaint(), update(), clipRegion()
*/

void TQWidget::enabledChange( bool )
{
    update();
#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::StateChanged );
#endif
}

/*!
    \fn void TQWidget::windowActivationChange( bool oldActive )

    This virtual function is called for a widget when its window is
    activated or deactivated by the window system. \a oldActive is the
    previous state; you can get the new setting from isActiveWindow().

    Reimplement this function if your widget needs to know when its
    window becomes activated or deactivated.

    The default implementation updates the visible part of the widget
    if the inactive and the active colorgroup are different for colors
    other than the highlight and link colors.

    \sa setActiveWindow(), isActiveWindow(), update(), palette()
*/

void TQWidget::windowActivationChange( bool )
{
#ifndef TQT_NO_PALETTE
    if ( !isVisible() )
	return;

    const TQColorGroup &acg = palette().active();
    const TQColorGroup &icg = palette().inactive();

    if ( acg != icg ) {
	BackgroundMode bm = backgroundMode();
	TQColorGroup::ColorRole role = TQPalette::backgroundRoleFromMode(bm);
	if ( bm > NoBackground  && acg.brush(role) != icg.brush(role) )
	    setBackgroundFromMode();
	else if ( acg.background() == icg.background() &&
		  acg.base() == icg.base() &&
		  acg.text() == icg.text() &&
		  acg.foreground() == icg.foreground() &&
		  acg.button() == icg.button() &&
		  acg.buttonText() == icg.buttonText() &&
		  acg.brightText() == icg.brightText() &&
		  acg.dark() == icg.dark() &&
		  acg.light() == icg.light() &&
		  acg.mid() == icg.mid() &&
		  acg.midlight() == icg.midlight() &&
		  acg.shadow() == icg.shadow() )
	    return;
	update();
    }
#endif
}

/*!
    \property TQWidget::frameGeometry
    \brief geometry of the widget relative to its parent including any
    window frame

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of geometry issues with top-level widgets.

    \sa geometry() x() y() pos()
*/
TQRect TQWidget::frameGeometry() const
{
    if (isTopLevel() && ! isPopup()) {
	if (fstrut_dirty)
	    updateFrameStrut();
	TQWidget *that = (TQWidget *) this;
	TQTLWExtra *top = that->topData();
	return TQRect(crect.x() - top->fleft,
		     crect.y() - top->ftop,
		     crect.width() + top->fleft + top->fright,
		     crect.height() + top->ftop + top->fbottom);
    }
    return crect;
}

/*! \property TQWidget::x
    \brief the x coordinate of the widget relative to its parent including
    any window frame

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of top-level widget geometry.

    \sa frameGeometry, y, pos
*/
int TQWidget::x() const
{
    if (isTopLevel() && ! isPopup()) {
	if (fstrut_dirty)
	    updateFrameStrut();
	TQWidget *that = (TQWidget *) this;
	return crect.x() - that->topData()->fleft;
    }
    return crect.x();
}

/*!
    \property TQWidget::y
    \brief the y coordinate of the widget relative to its parent and
    including any window frame

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of top-level widget geometry.

    \sa frameGeometry, x, pos
*/
int TQWidget::y() const
{
    if (isTopLevel() && ! isPopup()) {
	if (fstrut_dirty)
	    updateFrameStrut();
	TQWidget *that = (TQWidget *) this;
	return crect.y() - that->topData()->ftop;
    }
    return crect.y();
}

/*!
    \property TQWidget::pos
    \brief the position of the widget within its parent widget

    If the widget is a top-level widget, the position is that of the
    widget on the desktop, including its frame.

    When changing the position, the widget, if visible, receives a
    move event (moveEvent()) immediately. If the widget is not
    currently visible, it is guaranteed to receive an event before it
    is shown.

    move() is virtual, and all other overloaded move() implementations
    in TQt call it.

    \warning Calling move() or setGeometry() inside moveEvent() can
    lead to infinite recursion.

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of top-level widget geometry.

    \sa frameGeometry, size x(), y()
*/
TQPoint TQWidget::pos() const
{
    if (isTopLevel() && ! isPopup()) {
	if (fstrut_dirty)
	    updateFrameStrut();
	TQWidget *that = (TQWidget *) this;
	TQTLWExtra *top = that->topData();
	return TQPoint(crect.x() - top->fleft, crect.y() - top->ftop);
    }
    return crect.topLeft();
}

/*!
    \property TQWidget::geometry
    \brief the geometry of the widget relative to its parent and
    excluding the window frame

    When changing the geometry, the widget, if visible, receives a
    move event (moveEvent()) and/or a resize event (resizeEvent())
    immediately. If the widget is not currently visible, it is
    guaranteed to receive appropriate events before it is shown.

    The size component is adjusted if it lies outside the range
    defined by minimumSize() and maximumSize().

    setGeometry() is virtual, and all other overloaded setGeometry()
    implementations in TQt call it.

    \warning Calling setGeometry() inside resizeEvent() or moveEvent()
    can lead to infinite recursion.

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of top-level widget geometry.

    \sa frameGeometry(), rect(), move(), resize(), moveEvent(),
	resizeEvent(), minimumSize(), maximumSize()
*/

/*!
    \property TQWidget::size
    \brief the size of the widget excluding any window frame

    When resizing, the widget, if visible, receives a resize event
    (resizeEvent()) immediately. If the widget is not currently
    visible, it is guaranteed to receive an event before it is shown.

    The size is adjusted if it lies outside the range defined by
    minimumSize() and maximumSize(). Furthermore, the size is always
    at least TQSize(1, 1). For toplevel widgets, the minimum size
    might be larger, depending on the window manager.

    If you want a top-level window to have a fixed size, call
    setResizeMode( TQLayout::FreeResize ) on its layout.

    resize() is virtual, and all other overloaded resize()
    implementations in TQt call it.

    \warning Calling resize() or setGeometry() inside resizeEvent() can
    lead to infinite recursion.

    \sa pos, geometry, minimumSize, maximumSize, resizeEvent()
*/

/*!
    \property TQWidget::width
    \brief the width of the widget excluding any window frame

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of top-level widget geometry.

    \sa geometry, height, size
*/

/*!
    \property TQWidget::height
    \brief the height of the widget excluding any window frame

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of top-level widget geometry.

    \sa geometry, width, size
*/

/*!
    \property TQWidget::rect
    \brief the internal geometry of the widget excluding any window
    frame

    The rect property equals TQRect(0, 0, width(), height()).

    See the \link geometry.html Window Geometry documentation\endlink
    for an overview of top-level widget geometry.

    \sa size
*/

/*!
    \property TQWidget::childrenRect
    \brief the bounding rectangle of the widget's children

    Hidden children are excluded.

    \sa childrenRegion() geometry()
*/

TQRect TQWidget::childrenRect() const
{
    TQRect r( 0, 0, 0, 0 );
    if ( !children() )
	return r;
    TQObjectListIt it( *children() );
    TQObject *obj;
    while ( (obj = it.current()) ) {
	++it;
	if ( obj->isWidgetType() && !((TQWidget*)obj)->isHidden() && !((TQWidget*)obj)->isTopLevel())
	    r = r.unite( ((TQWidget*)obj)->geometry() );
    }
    return r;
}

/*!
    \property TQWidget::childrenRegion
    \brief the combined region occupied by the widget's children

    Hidden children are excluded.

    \sa childrenRect() geometry()
*/

TQRegion TQWidget::childrenRegion() const
{
    TQRegion r;
    if ( !children() )
	return r;
    TQObjectListIt it( *children() );		// iterate over all children
    TQObject *obj;
    while ( (obj=it.current()) ) {
	++it;
	if ( obj->isWidgetType() && !((TQWidget*)obj)->isHidden() && !((TQWidget*)obj)->isTopLevel())
	    r = r.unite( ((TQWidget*)obj)->geometry() );
    }
    return r;
}


/*!
    \property TQWidget::minimumSize
    \brief the widget's minimum size

    The widget cannot be resized to a smaller size than the minimum
    widget size. The widget's size is forced to the minimum size if
    the current size is smaller.

    If you use a layout inside the widget, the minimum size will be
    set by the layout and not by setMinimumSize(), unless you set the
    layout's resize mode to TQLayout::FreeResize.

    \sa minimumWidth, minimumHeight, maximumSize, sizeIncrement
	TQLayout::setResizeMode()
*/

TQSize TQWidget::minimumSize() const
{
    return extra ? TQSize( extra->minw, extra->minh ) : TQSize( 0, 0 );
}

/*!
    \property TQWidget::maximumSize
    \brief the widget's maximum size

    The widget cannot be resized to a larger size than the maximum
    widget size.

    \sa maximumWidth(), maximumHeight(), setMaximumSize(),
    minimumSize(), sizeIncrement()
*/

TQSize TQWidget::maximumSize() const
{
    return extra ? TQSize( extra->maxw, extra->maxh )
		 : TQSize( TQWIDGETSIZE_MAX, TQWIDGETSIZE_MAX );
}


/*!
    \property TQWidget::minimumWidth
    \brief the widget's minimum width

    This property corresponds to minimumSize().width().

    \sa minimumSize, minimumHeight
*/

/*!
    \property TQWidget::minimumHeight
    \brief the widget's minimum height

    This property corresponds to minimumSize().height().

    \sa minimumSize, minimumWidth
*/

/*!
    \property TQWidget::maximumWidth
    \brief the widget's maximum width

    This property corresponds to maximumSize().width().

    \sa maximumSize, maximumHeight
*/

/*!
    \property TQWidget::maximumHeight
    \brief the widget's maximum height

    This property corresponds to maximumSize().height().

    \sa maximumSize, maximumWidth
*/

/*!
    \property TQWidget::sizeIncrement
    \brief the size increment of the widget

    When the user resizes the window, the size will move in steps of
    sizeIncrement().width() pixels horizontally and
    sizeIncrement.height() pixels vertically, with baseSize() as the
    basis. Preferred widget sizes are for non-negative integers \e i
    and \e j:
    \code
	width = baseSize().width() + i * sizeIncrement().width();
	height = baseSize().height() + j * sizeIncrement().height();
    \endcode

    Note that while you can set the size increment for all widgets, it
    only affects top-level widgets.

    \warning The size increment has no effect under Windows, and may
    be disregarded by the window manager on X.

    \sa size, minimumSize, maximumSize
*/
TQSize TQWidget::sizeIncrement() const
{
    return ( extra && extra->topextra )
	? TQSize( extra->topextra->incw, extra->topextra->inch )
	: TQSize( 0, 0 );
}

/*!
    \property TQWidget::baseSize
    \brief the base size of the widget

    The base size is used to calculate a proper widget size if the
    widget defines sizeIncrement().

    \sa setSizeIncrement()
*/

TQSize TQWidget::baseSize() const
{
    return ( extra != 0 && extra->topextra != 0 )
	? TQSize( extra->topextra->basew, extra->topextra->baseh )
	: TQSize( 0, 0 );
}

/*!
    Sets both the minimum and maximum sizes of the widget to \a s,
    thereby preventing it from ever growing or shrinking.

    \sa setMaximumSize() setMinimumSize()
*/

void TQWidget::setFixedSize( const TQSize & s)
{
    setMinimumSize( s );
    setMaximumSize( s );
    resize( s );
}


/*!
    \overload void TQWidget::setFixedSize( int w, int h )

    Sets the width of the widget to \a w and the height to \a h.
*/

void TQWidget::setFixedSize( int w, int h )
{
    setMinimumSize( w, h );
    setMaximumSize( w, h );
    resize( w, h );
}

void TQWidget::setMinimumWidth( int w )
{
    setMinimumSize( w, minimumSize().height() );
}

void TQWidget::setMinimumHeight( int h )
{
    setMinimumSize( minimumSize().width(), h );
}

void TQWidget::setMaximumWidth( int w )
{
    setMaximumSize( w, maximumSize().height() );
}

void TQWidget::setMaximumHeight( int h )
{
    setMaximumSize( maximumSize().width(), h );
}

/*!
    Sets both the minimum and maximum width of the widget to \a w
    without changing the heights. Provided for convenience.

    \sa sizeHint() minimumSize() maximumSize() setFixedSize()
*/

void TQWidget::setFixedWidth( int w )
{
    setMinimumSize( w, minimumSize().height() );
    setMaximumSize( w, maximumSize().height() );
}


/*!
    Sets both the minimum and maximum heights of the widget to \a h
    without changing the widths. Provided for convenience.

    \sa sizeHint() minimumSize() maximumSize() setFixedSize()
*/

void TQWidget::setFixedHeight( int h )
{
    setMinimumSize( minimumSize().width(), h );
    setMaximumSize( maximumSize().width(), h );
}


/*!
    Translates the widget coordinate \a pos to the coordinate system
    of \a parent. The \a parent must not be 0 and must be a parent
    of the calling widget.

    \sa mapFrom() mapToParent() mapToGlobal() hasMouse()
*/

TQPoint TQWidget::mapTo( TQWidget * parent, const TQPoint & pos ) const
{
    TQPoint p = pos;
    if ( parent ) {
	const TQWidget * w = this;
	while ( w != parent ) {
	    p = w->mapToParent( p );
	    w = w->parentWidget();
	}
    }
    return p;
}


/*!
    Translates the widget coordinate \a pos from the coordinate system
    of \a parent to this widget's coordinate system. The \a parent
    must not be 0 and must be a parent of the calling widget.

    \sa mapTo() mapFromParent() mapFromGlobal() hasMouse()
*/

TQPoint TQWidget::mapFrom( TQWidget * parent, const TQPoint & pos ) const
{
    TQPoint p( pos );
    if ( parent ) {
	const TQWidget * w = this;
	while ( w != parent ) {
	    p = w->mapFromParent( p );
	    w = w->parentWidget();
	}
    }
    return p;
}


/*!
    Translates the widget coordinate \a pos to a coordinate in the
    parent widget.

    Same as mapToGlobal() if the widget has no parent.

    \sa mapFromParent() mapTo() mapToGlobal() hasMouse()
*/

TQPoint TQWidget::mapToParent( const TQPoint &pos ) const
{
    return pos + crect.topLeft();
}

/*!
    Translates the parent widget coordinate \a pos to widget
    coordinates.

    Same as mapFromGlobal() if the widget has no parent.

    \sa mapToParent() mapFrom() mapFromGlobal() hasMouse()
*/

TQPoint TQWidget::mapFromParent( const TQPoint &pos ) const
{
    return pos - crect.topLeft();
}


/*!
    Returns the top-level widget for this widget, i.e. the next
    ancestor widget that has (or could have) a window-system frame.

    If the widget is a top-level, the widget itself is returned.

    Typical usage is changing the window caption:

    \code
	aWidget->topLevelWidget()->setCaption( "New Caption" );
    \endcode

    \sa isTopLevel()
*/

TQWidget *TQWidget::topLevelWidget() const
{
    TQWidget *w = (TQWidget *)this;
    TQWidget *p = w->parentWidget();
    while ( !w->testWFlags(WType_TopLevel) && p ) {
	w = p;
	p = p->parentWidget();
    }
    return w;
}


/*!
    \property TQWidget::paletteForegroundColor
    \brief the foreground color of the widget

    setPaletteForegroundColor() is a convenience function that creates
    and sets a modified TQPalette with setPalette(). The palette is
    modified according to the widget's \e {background mode}. For
    example, if the background mode is \c PaletteButton the palette entry
    \c TQColorGroup::ButtonText is set to color.

    \sa setPalette() TQApplication::setPalette() backgroundMode()
      foregroundColor() setBackgroundMode() setEraseColor()
*/
const TQColor &TQWidget::paletteForegroundColor() const
{
#ifndef TQT_NO_PALETTE
    BackgroundMode mode = extra ? (BackgroundMode) extra->bg_mode_visual : PaletteBackground;
    return colorGroup().color( TQPalette::foregroundRoleFromMode(mode) );
#else
    return TQt::black;
#endif
}

void TQWidget::setPaletteForegroundColor( const TQColor & color )
{
#ifndef TQT_NO_PALETTE
    BackgroundMode mode = extra ? (BackgroundMode) extra->bg_mode_visual : PaletteBackground;
    TQPalette pal = palette();
    TQColorGroup::ColorRole role = TQPalette::foregroundRoleFromMode( mode );
    pal.setColor( TQPalette::Active, role, color );
    pal.setColor( TQPalette::Inactive, role, color );
    pal.setColor( TQPalette::Disabled, role, color );
    setPalette( pal );
#endif
}


/*!
    Same as paletteForegroundColor()
 */
const TQColor &TQWidget::foregroundColor() const
{
    return paletteForegroundColor();
}


/*!
    \fn const TQColor& TQWidget::eraseColor() const

    Returns the erase color of the widget.

    \sa setEraseColor() setErasePixmap() backgroundColor()
*/

/*!
    Sets the erase color of the widget to \a color.

    The erase color is the color the widget is to be cleared to before
    paintEvent() is called. If there is an erase pixmap (set using
    setErasePixmap()), then this property has an indeterminate value.

    \sa erasePixmap(), backgroundColor(), backgroundMode(), palette()
*/
void TQWidget::setEraseColor( const TQColor & color )
{
    setBackgroundModeDirect( FixedColor );
    setBackgroundColorDirect( color );
    update();
}

/*!
    Returns the widget's erase pixmap.

    \sa setErasePixmap() eraseColor()
*/
const TQPixmap *TQWidget::erasePixmap() const
{
    return ( extra && extra->bg_pix ) ? extra->bg_pix : 0;
}

/*!
    Sets the widget's erase pixmap to \a pixmap.

    This pixmap is used to clear the widget before paintEvent() is
    called.
*/
void TQWidget::setErasePixmap( const TQPixmap &pixmap )
{
    // This function is called with a null pixmap by setBackgroundEmpty().
    setBackgroundPixmapDirect( pixmap );
    setBackgroundModeDirect( FixedPixmap );
    update();
}

void TQWidget::setBackgroundFromMode()
{
#ifndef TQT_NO_PALETTE
    TQColorGroup::ColorRole r = TQColorGroup::Background;
    if ( extra ) {
	int i = (BackgroundMode)extra->bg_mode;
	if ( i == FixedColor || i == FixedPixmap || i == NoBackground ) {
	    // Mode is for fixed color, not one based on palette,
	    // so nothing to do.
	    return;
	}
	switch( i ) {
	case PaletteForeground:
	    r = TQColorGroup::Foreground;
	    break;
	case PaletteButton:
	    r = TQColorGroup::Button;
	    break;
	case PaletteLight:
	    r = TQColorGroup::Light;
	    break;
	case PaletteMidlight:
	    r = TQColorGroup::Midlight;
	    break;
	case PaletteDark:
	    r = TQColorGroup::Dark;
	    break;
	case PaletteMid:
	    r = TQColorGroup::Mid;
	    break;
	case PaletteText:
	    r = TQColorGroup::Text;
	    break;
	case PaletteBrightText:
	    r = TQColorGroup::BrightText;
	    break;
	case PaletteBase:
	    r = TQColorGroup::Base;
	    break;
	case PaletteBackground:
	    r = TQColorGroup::Background;
	    break;
	case PaletteShadow:
	    r = TQColorGroup::Shadow;
	    break;
	case PaletteHighlight:
	    r = TQColorGroup::Highlight;
	    break;
	case PaletteHighlightedText:
	    r = TQColorGroup::HighlightedText;
	    break;
	case PaletteButtonText:
	    r = TQColorGroup::ButtonText;
	    break;
	case X11ParentRelative:
#if defined(TQ_WS_X11)
	    setBackgroundX11Relative();
#endif
	    return;
	}
    }
    const TQColorGroup &cg = colorGroup();
    TQPixmap * p = cg.brush( r ).pixmap();
    if ( p )
	setBackgroundPixmapDirect( *p );
    else
	setBackgroundColorDirect( cg.color( r ) );
#endif
}

/*!
    \enum TQt::BackgroundMode

    This enum describes how the background of a widget changes, as the
    widget's palette changes.

    The background is what the widget contains when \link
    TQWidget::paintEvent() paintEvent()\endlink is called. To minimize
    flicker, this should be the most common color or pixmap in the
    widget. For \c PaletteBackground, use colorGroup().brush( \c
    TQColorGroup::Background ), and so on.

    \value PaletteForeground
    \value PaletteBackground
    \value PaletteButton
    \value PaletteLight
    \value PaletteMidlight
    \value PaletteDark
    \value PaletteMid
    \value PaletteText
    \value PaletteBrightText
    \value PaletteButtonText
    \value PaletteBase
    \value PaletteShadow
    \value PaletteHighlight
    \value PaletteHighlightedText
    \value PaletteLink
    \value PaletteLinkVisited
    \value X11ParentRelative (internal use only)

    The final three values have special meaning:

    \value NoBackground the widget is not cleared before paintEvent().
    If the widget's paint event always draws on all the pixels, using
    this mode can be both fast and flicker-free.
    \value FixedColor the widget is cleared to a fixed color, normally
    different from all the ones in the palette(). Set using \link
    TQWidget::setPaletteBackgroundColor()
    setPaletteBackgroundColor()\endlink.
    \value FixedPixmap the widget is cleared to a fixed pixmap,
    normally different from all the ones in the palette(). Set using
    \link TQWidget::setPaletteBackgroundPixmap()
    setPaletteBackgroundPixmap()\endlink.

    Although \c FixedColor and \c FixedPixmap are sometimes just
    right, if you use them, make sure that you test your application
    when the desktop color scheme has been changed. (On X11, a quick
    way to test this is e.g. "./myapp -bg paleblue". On Windows, you
    must use the control panel.)

    \sa TQWidget::setBackgroundMode() TQWidget::backgroundMode()
    TQWidget::setBackgroundPixmap() TQWidget::setPaletteBackgroundColor()
*/

/*!
    \property TQWidget::backgroundMode
    \brief the color role used for painting the background of the widget

    setPaletteBackgroundColor() reads this property to determine which
    entry of the \link TQWidget::palette palette\endlink to set.

    For most widgets the default suffices (\c PaletteBackground,
    typically gray), but some need to use \c PaletteBase (the
    background color for text output, typically white) or another
    role.

    TQListBox, which is "sunken" and uses the base color to contrast
    with its environment, does this in its constructor:

    \code
    setBackgroundMode( PaletteBase );
    \endcode

    You will never need to set the background mode of a built-in
    widget in TQt, but you might consider setting it in your custom
    widgets, so that setPaletteBackgroundColor() works as expected.

    Note that two of the BackgroundMode values make no sense for
    setBackgroundMode(), namely \c FixedPixmap and \c FixedColor. You
    must call setBackgroundPixmap() and setPaletteBackgroundColor()
    instead.
*/
TQt::BackgroundMode TQWidget::backgroundMode() const
{
    return extra ? (BackgroundMode) extra->bg_mode : PaletteBackground;
}

void TQWidget::setBackgroundMode( BackgroundMode m )
{
    setBackgroundMode( m, m );
    if ( (widget_state & (WState_Visible|WState_BlockUpdates)) ==
	 WState_Visible )
	update();
}


/*!
    \overload

    Sets the widget's own background mode to \a m and the visual
    background mode to \a visual. The visual background mode is used
    with the designable properties \c backgroundColor, \c
    foregroundColor and \c backgroundPixmap.

    For complex controls, the logical background mode sometimes
    differs from a widget's own background mode. A spinbox for example
    has \c PaletteBackground as background mode (typically dark gray),
    while it's embedded lineedit control uses \c PaletteBase
    (typically white). Since the lineedit covers most of the visual
    area of a spinbox, it defines \c PaletteBase to be its \a visual
    background mode. Changing the \c backgroundColor property thus
    changes the lineedit control's background, which is exactly what
    the user expects in \e{TQt Designer}.
*/
void TQWidget::setBackgroundMode( BackgroundMode m, BackgroundMode visual )
{
    if ( m == NoBackground ) {
	setBackgroundEmpty();
    } else if ( m == FixedColor || m == FixedPixmap ) {
#if defined(QT_DEBUG)
	tqWarning( "TQWidget::setBackgroundMode: FixedColor or FixedPixmap makes"
		  " no sense" );
#endif
	return;
    }
    setBackgroundModeDirect(m);
    if ( m != visual && !extra )
	createExtra();
    if ( extra )
	extra->bg_mode_visual = visual;
}


/*!
  \internal
*/
void TQWidget::setBackgroundModeDirect( BackgroundMode m )
{
    if ( m == PaletteBackground && !extra )
	return;

    createExtra();
    if ( (BackgroundMode)extra->bg_mode != m ) {
	extra->bg_mode = m;
	extra->bg_mode_visual = m;
	setBackgroundFromMode();
    }
}

/*!
    \property TQWidget::paletteBackgroundColor
    \brief the background color of the widget

    The palette background color is usually set implicitly by
    setBackgroundMode(), although it can also be set explicitly by
    setPaletteBackgroundColor(). setPaletteBackgroundColor() is a
    convenience function that creates and sets a modified TQPalette
    with setPalette(). The palette is modified according to the
    widget's background mode. For example, if the background mode is
    \c PaletteButton the color used for the palette's \c
    TQColorGroup::Button color entry is set.

    If there is a background pixmap (set using
    setPaletteBackgroundPixmap()), then the return value of this
    function is indeterminate.

    \sa paletteBackgroundPixmap, paletteForegroundColor, palette, colorGroup()
*/
const TQColor & TQWidget::paletteBackgroundColor() const
{
#ifndef TQT_NO_PALETTE
    BackgroundMode mode = extra ? (BackgroundMode) extra->bg_mode_visual : PaletteBackground;
    switch( mode ) {
    case FixedColor:
    case FixedPixmap :
    case NoBackground:
    case X11ParentRelative:
	return eraseColor();
    default:
	TQColorGroup::ColorRole role = TQPalette::backgroundRoleFromMode( mode );
	return colorGroup().color( role );
    }
#else
    return eraseColor();
#endif
}

void TQWidget::setPaletteBackgroundColor( const TQColor &color )
{
#ifndef TQT_NO_PALETTE
    BackgroundMode mode = extra ? (BackgroundMode) extra->bg_mode_visual : PaletteBackground;
    switch( mode ) {
    case FixedColor:
    case FixedPixmap :
    case NoBackground:
    case X11ParentRelative:
	setEraseColor( color );
	break;
    default:
	TQPalette pal = palette();
	TQColorGroup::ColorRole role = TQPalette::backgroundRoleFromMode( mode );
	pal.setColor( TQPalette::Active, role, color );
	pal.setColor( TQPalette::Inactive, role, color );
	pal.setColor( TQPalette::Disabled, role, color );
	setPalette( pal );
	break;
    }
#else
    setEraseColor( color );
#endif
}


/*!
    \property TQWidget::paletteBackgroundPixmap
    \brief the background pixmap of the widget

    The palette background pixmap is usually set implicitly by
    setBackgroundMode(), although it can also be set explicitly by
    setPaletteBackgroundPixmap(). setPaletteBackgroundPixmap() is a
    convenience function that creates and sets a modified TQPalette
    with setPalette(). The palette is modified according to the
    widget's background mode. For example, if the background mode is
    \c PaletteButton the pixmap used for the palette's
    \c TQColorGroup::Button color entry is set.

    If there is a plain background color (set using
    setPaletteBackgroundColor()), then this function returns 0.

    \sa paletteBackgroundColor, paletteForegroundColor, palette, colorGroup()
*/
const TQPixmap *TQWidget::paletteBackgroundPixmap() const
{
#ifndef TQT_NO_PALETTE
    BackgroundMode mode = extra ? (BackgroundMode) extra->bg_mode_visual : PaletteBackground;
    switch( mode ) {
    case FixedColor:
    case FixedPixmap :
    case NoBackground:
    case X11ParentRelative:
	return erasePixmap();
    default:
	TQColorGroup::ColorRole role = TQPalette::backgroundRoleFromMode( mode );
	return palette().brush( TQPalette::Active, role ).pixmap();
    }
#else
    return erasePixmap();
#endif
}

void TQWidget::setPaletteBackgroundPixmap( const TQPixmap &pixmap )
{
#ifndef TQT_NO_PALETTE
    BackgroundMode mode = extra ? (BackgroundMode) extra->bg_mode_visual : PaletteBackground;
    switch( mode ) {
    case FixedColor:
    case FixedPixmap :
    case NoBackground:
    case X11ParentRelative:
	setErasePixmap( pixmap );
	break;
    default:
	TQPalette pal = palette();
	TQColorGroup::ColorRole role = TQPalette::backgroundRoleFromMode( mode );
	pal.setBrush( TQPalette::Active, role, TQBrush( pal.color( TQPalette::Active, role ), pixmap ) );
	pal.setBrush( TQPalette::Inactive, role, TQBrush( pal.color( TQPalette::Inactive, role ), pixmap ) );
	pal.setBrush( TQPalette::Disabled, role, TQBrush( pal.color( TQPalette::Disabled, role ), pixmap ) );
	setPalette( pal );
	break;
    }
#else
    setErasePixmap( pixmap );
#endif
}


/*!
    \property TQWidget::backgroundBrush
    \brief the widget's background brush

    The background brush depends on a widget's palette and its
    background mode.

    \sa backgroundColor(), backgroundPixmap(), eraseColor(),  palette,
    TQApplication::setPalette()
*/
const TQBrush& TQWidget::backgroundBrush() const
{
    static TQBrush noBrush;
#ifndef TQT_NO_PALETTE
    BackgroundMode mode = extra ? (BackgroundMode) extra->bg_mode_visual : PaletteBackground;
    switch( mode ) {
    case FixedColor:
    case FixedPixmap :
    case NoBackground:
    case X11ParentRelative:
	return noBrush;
    default:
	TQColorGroup::ColorRole role = TQPalette::backgroundRoleFromMode( mode );
	return colorGroup().brush( role );
    }
#else
    return noBrush;
#endif
}


/*!
    \property TQWidget::colorGroup
    \brief the current color group of the widget palette

    The color group is determined by the state of the widget. A
    disabled widget has the TQPalette::disabled() color group, a widget
    with keyboard focus has the TQPalette::active() color group, and an
    inactive widget has the TQPalette::inactive() color group.

    \sa palette
*/
#ifndef TQT_NO_PALETTE
const TQColorGroup &TQWidget::colorGroup() const
{
    if ( !isEnabled() )
	return palette().disabled();
    else if ( !isVisible() || isActiveWindow() )
	return palette().active();
    else
	return palette().inactive();
}
#endif

/*!
    \property TQWidget::palette
    \brief the widget's palette

    As long as no special palette has been set, or after unsetPalette()
    has been called, this is either a special palette for the widget
    class, the parent's palette or (if this widget is a top level
    widget), the default application palette.

    Instead of defining an entirely new palette, you can also use the
    \link TQWidget::paletteBackgroundColor paletteBackgroundColor\endlink,
    \link TQWidget::paletteBackgroundPixmap paletteBackgroundPixmap\endlink and
    \link TQWidget::paletteForegroundColor paletteForegroundColor\endlink
    convenience properties to change a widget's
    background and foreground appearance only.

    \sa ownPalette, colorGroup(), TQApplication::palette()
*/

#ifndef TQT_NO_PALETTE
void TQWidget::setPalette( const TQPalette &palette )
{
    own_palette = true;
    if ( pal == palette )
	return;
    TQPalette old = pal;
    pal = palette;
    setBackgroundFromMode();
    TQEvent ev( TQEvent::PaletteChange );
    TQApplication::sendEvent( this, &ev );
    if ( children() ) {
	TQEvent e( TQEvent::ParentPaletteChange );
	TQObjectListIt it( *children() );
	TQWidget *w;
	while( (w=(TQWidget *)it.current()) != 0 ) {
	    ++it;
	    if ( w->isWidgetType() )
		TQApplication::sendEvent( w, &e );
	}
    }
    paletteChange( old );
    update();
}

void TQWidget::unsetPalette()
{
    // reset the palette
    setPalette( tqt_naturalWidgetPalette( this ) );
    own_palette = false;
}

/*!
  \fn void TQWidget::setPalette( const TQPalette&, bool )
  \obsolete

  Use setPalette( const TQPalette& p ) instead.
*/

/*!
    \fn void TQWidget::paletteChange( const TQPalette &oldPalette )

    This virtual function is called from setPalette(). \a oldPalette
    is the previous palette; you can get the new palette from
    palette().

    Reimplement this function if your widget needs to know when its
    palette changes.

    \sa setPalette(), palette()
*/

void TQWidget::paletteChange( const TQPalette & )
{
}
#endif // TQT_NO_PALETTE

/*!
    \property TQWidget::font
    \brief the font currently set for the widget

    The fontInfo() function reports the actual font that is being used
    by the widget.

    As long as no special font has been set, or after unsetFont() is
    called, this is either a special font for the widget class, the
    parent's font or (if this widget is a top level widget), the
    default application font.

    This code fragment sets a 12 point helvetica bold font:
    \code
    TQFont f( "Helvetica", 12, TQFont::Bold );
    setFont( f );
    \endcode

    In addition to setting the font, setFont() informs all children
    about the change.

    \sa fontChange() fontInfo() fontMetrics() ownFont()
*/
void TQWidget::setFont( const TQFont &font )
{
    own_font = true;
    if ( fnt == font && fnt.d->mask == font.d->mask )
	return;
    TQFont old = fnt;
    fnt = font.resolve( tqt_naturalWidgetFont( this ) );
#if defined(TQ_WS_X11)
    // make sure the font set on this widget is associated with the correct screen
    fnt.x11SetScreen( x11Screen() );
#endif
    if ( children() ) {
	TQEvent e( TQEvent::ParentFontChange );
	TQObjectListIt it( *children() );
	TQWidget *w;
	while( (w=(TQWidget *)it.current()) != 0 ) {
	    ++it;
	    if ( w->isWidgetType() )
		TQApplication::sendEvent( w, &e );
	}
    }
    if ( hasFocus() )
	setFontSys();
    fontChange( old );
}

void TQWidget::unsetFont()
{
    // reset the font
    setFont( tqt_naturalWidgetFont( this ) );
    own_font = false;
}

/*!
  \fn void TQWidget::setFont( const TQFont&, bool )
  \obsolete

  Use setFont(const TQFont& font) instead.
*/

/*!
    \fn void TQWidget::fontChange( const TQFont &oldFont )

    This virtual function is called from setFont(). \a oldFont is the
    previous font; you can get the new font from font().

    Reimplement this function if your widget needs to know when its
    font changes. You will almost certainly need to update the widget
    using update().

    The default implementation updates the widget including its
    geometry.

    \sa setFont(), font(), update(), updateGeometry()
*/

void TQWidget::fontChange( const TQFont & )
{
    update();
    updateGeometry();
}


/*!
    \fn TQFontMetrics TQWidget::fontMetrics() const

    Returns the font metrics for the widget's current font.
    Equivalent to TQFontMetrics(widget->font()).

    \sa font(), fontInfo(), setFont()
*/

/*!
    \fn TQFontInfo TQWidget::fontInfo() const

    Returns the font info for the widget's current font.
    Equivalent to TQFontInto(widget->font()).

    \sa font(), fontMetrics(), setFont()
*/


/*!
    \property TQWidget::cursor
    \brief the cursor shape for this widget

    The mouse cursor will assume this shape when it's over this
    widget. See the \link TQt::CursorShape list of predefined cursor
    objects\endlink for a range of useful shapes.

    An editor widget might use an I-beam cursor:
    \code
	setCursor( IbeamCursor );
    \endcode

    If no cursor has been set, or after a call to unsetCursor(), the
    parent's cursor is used. The function unsetCursor() has no effect
    on top-level widgets.

    \sa TQApplication::setOverrideCursor()
*/

#ifndef TQT_NO_CURSOR
const TQCursor &TQWidget::cursor() const
{
    if ( testWState(WState_OwnCursor) )
	return (extra && extra->curs)
	    ? *extra->curs
	    : arrowCursor;
    else
	return (isTopLevel() || !parentWidget()) ? arrowCursor : parentWidget()->cursor();
}
#endif
#ifndef TQT_NO_WIDGET_TOPEXTRA
/*!
    \property TQWidget::caption
    \brief the window caption (title)

    This property only makes sense for top-level widgets. If no
    caption has been set, the caption is TQString::null.

    \sa icon() iconText()
*/
TQString TQWidget::caption() const
{
    return extra && extra->topextra
	? extra->topextra->caption
	: TQString::null;
}

/*!
    \property TQWidget::icon
    \brief the widget's icon

    This property only makes sense for top-level widgets. If no icon
    has been set, icon() returns 0.

    \sa iconText, caption,
      \link appicon.html Setting the Application Icon\endlink
*/
const TQPixmap *TQWidget::icon() const
{
    return ( extra && extra->topextra ) ? extra->topextra->icon : 0;
}

/*!
    \property TQWidget::iconText
    \brief the widget's icon text

    This property only makes sense for top-level widgets. If no icon
    text has been set, this functions returns TQString::null.

    \sa icon, caption
*/

TQString TQWidget::iconText() const
{
    return ( extra && extra->topextra ) ? extra->topextra->iconText
	: TQString::null;
}
#endif //TQT_NO_WIDGET_TOPEXTRA

/*!
    \property TQWidget::mouseTracking
    \brief whether mouse tracking is enabled for the widget

    If mouse tracking is disabled (the default), the widget only
    receives mouse move events when at least one mouse button is
    pressed while the mouse is being moved.

    If mouse tracking is enabled, the widget receives mouse move
    events even if no buttons are pressed.

    \sa mouseMoveEvent(), TQApplication::setGlobalMouseTracking()
*/


/*!
    Sets the widget's focus proxy to widget \a w. If \a w is 0, the
    function resets this widget to have no focus proxy.

    Some widgets, such as TQComboBox, can "have focus", but create a
    child widget to actually handle the focus. TQComboBox, for example,
    creates a TQLineEdit which handles the focus.

    setFocusProxy() sets the widget which will actually get focus when
    "this widget" gets it. If there is a focus proxy, focusPolicy(),
    setFocusPolicy(), setFocus() and hasFocus() all operate on the
    focus proxy.

    \sa focusProxy()
*/

void TQWidget::setFocusProxy( TQWidget * w )
{
    if ( !w && !extra )
	return;

    for ( TQWidget* fp  = w; fp; fp = fp->focusProxy() ) {
	if ( fp == this ) {
#if defined (QT_CHECK_STATE)
	    tqWarning( "%s (%s): already in focus proxy chain", className(), name() );
#endif
	    return;
	}
    }

    createExtra();

    if ( extra->focus_proxy ) {
	disconnect( extra->focus_proxy, TQ_SIGNAL(destroyed()),
		    this, TQ_SLOT(focusProxyDestroyed()) );
	extra->focus_proxy = 0;
    }

    if ( w ) {
	setFocusPolicy( w->focusPolicy() );
	connect( w, TQ_SIGNAL(destroyed()),
		 this, TQ_SLOT(focusProxyDestroyed()) );
    }
    extra->focus_proxy = w;
}


/*!
    Returns the focus proxy, or 0 if there is no focus proxy.

    \sa setFocusProxy()
*/

TQWidget * TQWidget::focusProxy() const
{
    return extra ? extra->focus_proxy : 0;
}


/*!
    \internal

    Internal slot used to clean up if the focus proxy is destroyed.

    \sa setFocusProxy()
*/

void TQWidget::focusProxyDestroyed()
{
    if ( extra )
	extra->focus_proxy = 0;
    setFocusPolicy( NoFocus );
}

/*!
    \property TQWidget::focus
    \brief whether this widget (or its focus proxy) has the keyboard
    input focus

    Effectively equivalent to \c {tqApp->focusWidget() == this}.

    \sa setFocus(), clearFocus(), setFocusPolicy(), TQApplication::focusWidget()
*/
bool TQWidget::hasFocus() const
{
    const TQWidget* w = this;
    while ( w->focusProxy() )
	w = w->focusProxy();
    return tqApp->focusWidget() == w;
}

/*!
    Gives the keyboard input focus to this widget (or its focus
    proxy) if this widget or one of its parents is the \link
    isActiveWindow() active window\endlink.

    First, a focus out event is sent to the focus widget (if any) to
    tell it that it is about to lose the focus. Then a focus in event
    is sent to this widget to tell it that it just received the focus.
    (Nothing happens if the focus in and focus out widgets are the
    same.)

    setFocus() gives focus to a widget regardless of its focus policy,
    but does not clear any keyboard grab (see grabKeyboard()).

    Be aware that if the widget is hidden, it will not accept focus.

    \warning If you call setFocus() in a function which may itself be
    called from focusOutEvent() or focusInEvent(), you may get an
    infinite recursion.

    \sa hasFocus() clearFocus() focusInEvent() focusOutEvent()
    setFocusPolicy() TQApplication::focusWidget() grabKeyboard()
    grabMouse()
*/

void TQWidget::setFocus()
{
    if ( !isEnabled() )
	return;

    if ( focusProxy() ) {
	focusProxy()->setFocus();
	return;
    }

    TQFocusData * f = focusData( true );
    if ( f->it.current() == this && tqApp->focusWidget() == this
#if defined(TQ_WS_WIN)
	&& GetFocus() == winId()
#endif
	)
	return;

    f->it.toFirst();
    while ( f->it.current() != this && !f->it.atLast() )
	++f->it;
    // at this point, the iterator should point to 'this'.  if it
    // does not, 'this' must not be in the list - an error, but
    // perhaps possible.  fix it.
    if ( f->it.current() != this ) {
	f->focusWidgets.append( this );
	f->it.toLast();
    }

    if ( isActiveWindow() ) {
	TQWidget * prev = tqApp->focus_widget;
	if ( prev ) {
	    // This part is never executed when TQ_WS_X11? Preceding XFocusOut
	    // had already reset focus_widget when received XFocusIn

	    // Don't reset input context explicitly here. Whether reset or not
	    // when focusing out is a responsibility of input methods. For
	    // example, Japanese input context should not be reset here. The
	    // context sometimes contains a whole paragraph and has minutes of
	    // lifetime different to ephemeral one in other languages. The
	    // input context should be survived until focused again. So we
	    // delegate the responsibility to input context via
	    // unfocusInputContext().
	    if ( prev != this && prev->isInputMethodEnabled() ) {
#if 0
		prev->resetInputContext();
#else
		prev->unfocusInputContext();
#endif
	    }
	}
#if defined(TQ_WS_WIN)
	else {
	    TQInputContext::endComposition();
	}
#endif
	tqApp->focus_widget = this;
	if( isInputMethodEnabled() )
	    focusInputContext();

#if defined(TQ_WS_WIN)
	if ( !topLevelWidget()->isPopup() )
	    SetFocus( winId() );
	else {
#endif
#if defined(QT_ACCESSIBILITY_SUPPORT)
	    TQAccessible::updateAccessibility( this, 0, TQAccessible::Focus );
#endif
#if defined(TQ_WS_WIN)
	}
#endif

	if ( prev != this ) {
	    if ( prev ) {
		TQFocusEvent out( TQEvent::FocusOut );
		TQApplication::sendEvent( prev, &out );
	    }

	    if ( tqApp->focus_widget == this ) {
		TQFocusEvent in( TQEvent::FocusIn );
		TQApplication::sendEvent( this, &in );
	    }
	}
    }
}

/*!
    Takes keyboard input focus from the widget.

    If the widget has active focus, a \link focusOutEvent() focus out
    event\endlink is sent to this widget to tell it that it is about
    to lose the focus.

    This widget must enable focus setting in order to get the keyboard
    input focus, i.e. it must call setFocusPolicy().

    \sa hasFocus(), setFocus(), focusInEvent(), focusOutEvent(),
    setFocusPolicy(), TQApplication::focusWidget()
*/

void TQWidget::clearFocus()
{
    if ( focusProxy() ) {
	focusProxy()->clearFocus();
	return;
    } else if ( hasFocus() ) {
#if !defined(TQ_WS_X11)
        resetInputContext();
#else
	unfocusInputContext();
#endif
	TQWidget* w = tqApp->focusWidget();
	// clear active focus
	tqApp->focus_widget = 0;
	TQFocusEvent out( TQEvent::FocusOut );
	TQApplication::sendEvent( w, &out );
#if defined(TQ_WS_WIN)
	if ( !isPopup() && GetFocus() == winId() )
	    SetFocus( 0 );
	else {
#endif
#if defined(QT_ACCESSIBILITY_SUPPORT)
	    TQAccessible::updateAccessibility( this, 0, TQAccessible::Focus );
#endif
#if defined(TQ_WS_WIN)
	}
#endif
    }
}


/*!
    Finds a new widget to give the keyboard focus to, as appropriate
    for Tab and Shift+Tab, and returns true if is can find a new
    widget and false if it can't,

    If \a next is true, this function searches "forwards", if \a next
    is false, it searches "backwards".

    Sometimes, you will want to reimplement this function. For
    example, a web browser might reimplement it to move its "current
    active link" forwards or backwards, and call
    TQWidget::focusNextPrevChild() only when it reaches the last or
    first link on the "page".

    Child widgets call focusNextPrevChild() on their parent widgets,
    but only the top-level widget decides where to redirect focus. By
    overriding this method for an object, you thus gain control of
    focus traversal for all child widgets.

    \warning TQScrollView uses it own logic for this function, which
    does the right thing in most cases. But if you are using a
    TQScrollView and want complete control of the focus chain you'll
    need to override TQScrollView::focusNextPrevChild() and your
    top-level widgets' focusNextPrevChild() functions.

    \sa focusData()
*/

bool TQWidget::focusNextPrevChild( bool next )
{
    TQWidget* p = parentWidget();
    if ( !isTopLevel() && p )
	return p->focusNextPrevChild(next);

    TQFocusData *f = focusData( true );

    TQWidget *startingPoint = f->it.current();
    TQWidget *candidate = 0;
    TQWidget *w = next ? f->focusWidgets.last() : f->focusWidgets.first();
    extern bool tqt_tab_all_widgets;
    uint focus_flag = tqt_tab_all_widgets ? TabFocus : StrongFocus;
    do {
	if ( w && w != startingPoint &&
	     ( ( w->focusPolicy() & focus_flag ) == focus_flag )
	     && !w->focusProxy() && w->isVisibleTo(this) && w->isEnabled())
	    candidate = w;
	w = next ? f->focusWidgets.prev() : f->focusWidgets.next();
    } while( w && !(candidate && w==startingPoint) );

    if ( !candidate )
	return false;

    candidate->setFocus();
    return true;
}

/*!
    Returns the focus widget in this widget's window. This is not the
    same as TQApplication::focusWidget(), which returns the focus
    widget in the currently active window.
*/

TQWidget *TQWidget::focusWidget() const
{
    TQWidget *that = (TQWidget *)this;		// mutable
    TQFocusData *f = that->focusData( false );
    if ( f && f->focusWidgets.count() && f->it.current() == 0 )
	f->it.toFirst();
    return ( f && f->it.current() ) ? f->it.current() : 0;
}


/*!
    Returns the focus data for this widget's top-level widget.

    Focus data always belongs to the top-level widget. The focus data
    list contains all the widgets in this top-level widget that can
    accept focus, in tab order. An iterator points to the current
    focus widget (focusWidget() returns a pointer to this widget).

    This information is useful for implementing advanced versions of
    focusNextPrevChild().
*/
TQFocusData * TQWidget::focusData()
{
    return focusData( true );
}

/*!
    \internal

    Internal function which lets us ask for the focus data, creating
    it if it doesn't exist and \a create is true.
*/
TQFocusData * TQWidget::focusData( bool create )
{
    TQWidget * tlw = topLevelWidget();
    TQWExtra * ed = tlw->extraData();
    if ( !ed || !ed->topextra ) {
	if ( !create )
	    return 0;
	tlw->createTLExtra();
	ed = tlw->extraData();
    }
    if ( create && !ed->topextra->focusData )
	ed->topextra->focusData = new TQFocusData;

    return ed->topextra->focusData;
}

/*!
    \property TQWidget::inputMethodEnabled
    \brief enables or disables the use of input methods for this widget.

    Most Widgets (as eg. buttons) that do not handle text input should have
    the input method disabled if they have focus. This is the default.

    If a widget handles text input it should set this property to true.
*/

void TQWidget::setInputMethodEnabled( bool b )
{
    im_enabled = b;
#ifdef TQ_WS_WIN
    TQInputContext::enable( this, im_enabled & !((bool)testWState(WState_Disabled)) );
#endif
}


/*!
    Enables key event compression, if \a compress is true, and
    disables it if \a compress is false.

    Key compression is off by default (except for TQLineEdit and
    TQTextEdit), so widgets receive one key press event for each key
    press (or more, since autorepeat is usually on). If you turn it on
    and your program doesn't keep up with key input, TQt may try to
    compress key events so that more than one character can be
    processed in each event.

    For example, a word processor widget might receive 2, 3 or more
    characters in each TQKeyEvent::text(), if the layout recalculation
    takes too long for the CPU.

    If a widget supports multiple character unicode input, it is
    always safe to turn the compression on.

    TQt performs key event compression only for printable characters.
    Modifier keys, cursor movement keys, function keys and
    miscellaneous action keys (e.g. Escape, Enter, Backspace,
    PrintScreen) will stop key event compression, even if there are
    more compressible key events available.

    Not all platforms support this compression, in which case turning
    it on will have no effect.

    \sa TQKeyEvent::text();
*/

void TQWidget::setKeyCompression(bool compress)
{
    if ( compress )
	setWState( WState_CompressKeys );
    else
	clearWState( WState_CompressKeys );
}

/*!
    \property TQWidget::isActiveWindow
    \brief whether this widget is the active window

    The active window is the window that contains the widget
    that has keyboard focus.

    When popup windows are visible, this property is true for both the
    active window \e and for the popup.

    \sa setActiveWindow(), TQApplication::activeWindow()
*/
bool TQWidget::isActiveWindow() const
{
    TQWidget *tlw = topLevelWidget();
    if(testWFlags(WSubWindow) && parentWidget())
	tlw = parentWidget()->topLevelWidget();
    if(tlw == tqApp->activeWindow() || ( isVisible() && tlw->isPopup() ))
	return true;
#ifndef TQT_NO_STYLE
    const_cast<TQWidget*>(this)->createExtra();
    if (!extra->m_ceData) {
        const_cast<TQWidget*>(this)->extra->m_ceData = new TQStyleControlElementData();
    }
    //extra->m_ceData->widgetObjectTypes = getObjectTypeListForObject(this);
    extra->m_ceData->widgetObjectTypes.clear();
    extra->m_ceData->allDataPopulated = false;
    //if(style().styleHint(TQStyle::SH_Widget_ShareActivation, *extra->m_ceData, getControlElementFlagsForObject(this, extra->m_ceData->widgetObjectTypes, TQStyleOption(), false), TQStyleOption(), NULL, this)) {
    if(style().styleHint(TQStyle::SH_Widget_ShareActivation, *extra->m_ceData, TQStyle::CEF_None, TQStyleOption(), NULL, this)) {
        if((tlw->isDialog() || (tlw->testWFlags(TQt::WStyle_Tool) && !tlw->isPopup())) &&
           !tlw->testWFlags(TQt::WShowModal) &&
           (!tlw->parentWidget() || tlw->parentWidget()->isActiveWindow()))
	   return true;
	TQWidget *w = tqApp->activeWindow();
	if( !testWFlags(WSubWindow) && w && w->testWFlags(WSubWindow) &&
	    w->parentWidget()->topLevelWidget() == tlw)
	    return true;
        while(w && (tlw->isDialog() || tlw->testWFlags(TQt::WStyle_Tool)) &&
              !w->testWFlags(TQt::WShowModal) && w->parentWidget()) {
	    w = w->parentWidget()->topLevelWidget();
	    if( w == tlw )
		return true;
	}
    }
#endif
#if defined(TQ_WS_WIN32)
    HWND parent = tlw->winId();
    HWND topparent = GetActiveWindow();
    while ( parent ) {
	parent = ::GetParent( parent );
	if ( parent && parent == topparent )
	    return true;
    }
#endif

    return false;
}

/*!
    Moves the \a second widget around the ring of focus widgets so
    that keyboard focus moves from the \a first widget to the \a
    second widget when the Tab key is pressed.

    Note that since the tab order of the \a second widget is changed,
    you should order a chain like this:

    \code
	setTabOrder( a, b ); // a to b
	setTabOrder( b, c ); // a to b to c
	setTabOrder( c, d ); // a to b to c to d
    \endcode

    \e not like this:

    \code
	setTabOrder( c, d ); // c to d   WRONG
	setTabOrder( a, b ); // a to b AND c to d
	setTabOrder( b, c ); // a to b to c, but not c to d
    \endcode

    If \a first or \a second has a focus proxy, setTabOrder()
    correctly substitutes the proxy.

    \sa setFocusPolicy(), setFocusProxy()
*/
void TQWidget::setTabOrder( TQWidget* first, TQWidget *second )
{
    if ( !first || !second ||
	first->focusPolicy() == NoFocus || second->focusPolicy() == NoFocus )
	return;

    // If first is redirected, set first to the last child of first
    // that can take keyboard focus so that second is inserted after
    // that last child, and the focus order within first is (more
    // likely to be) preserved.
    if ( first->focusProxy() ) {
	TQObjectList *l = first->queryList( "TQWidget" );
	if ( l && l->count() ) {
	    TQObjectListIt it(*l);
	    it.toLast();
	    while (it.current()) {
		if (((TQWidget*)it.current())->topLevelWidget() == first->topLevelWidget()) {
		    first = (TQWidget*)it.current();
		    if (first->focusPolicy() != NoFocus)
			break;
		}
		--it;
	    }
	}
	delete l;
    }
    while ( first->focusProxy() )
	first = first->focusProxy();
    while ( second->focusProxy() )
	second = second->focusProxy();

    TQFocusData *f = first->focusData( true );
    bool focusThere = (f->it.current() == second );
    f->focusWidgets.removeRef( second );
    if ( f->focusWidgets.findRef( first ) >= 0 )
	f->focusWidgets.insert( f->focusWidgets.at() + 1, second );
    else
	f->focusWidgets.append( second );
    if ( focusThere ) { // reset iterator so tab will work appropriately
	f->it.toFirst();
	while( f->it.current() && f->it.current() != second )
	    ++f->it;
    }
}

/*!\internal

  Moves the relevant subwidgets of this widget from the \a oldtlw's
  tab chain to that of the new parent, if there's anything to move and
  we're really moving

  This function is called from TQWidget::reparent() *after* the widget
  has been reparented.

  \sa reparent()
*/

void TQWidget::reparentFocusWidgets( TQWidget * oldtlw )
{
    if ( oldtlw == topLevelWidget() )
	return; // nothing to do

    TQFocusData * from = oldtlw ? oldtlw->topData()->focusData : 0;
    TQFocusData * to;
    to = focusData();

    if ( from ) {
	from->focusWidgets.first();
	do {
	    TQWidget * pw = from->focusWidgets.current();
	    while( pw && pw != this )
		pw = pw->parentWidget();
	    if ( pw == this ) {
		TQWidget * w = from->focusWidgets.take();
		if ( w == from->it.current() )
		    // probably best to clear keyboard focus, or
		    // the user might become rather confused
		    w->clearFocus();
		if ( !isTopLevel() )
		    to->focusWidgets.append( w );
	    } else {
		from->focusWidgets.next();
	    }
	} while( from->focusWidgets.current() );
    }

    if ( to->focusWidgets.findRef(this) < 0 )
	to->focusWidgets.append( this );

    if ( !isTopLevel() && extra && extra->topextra && extra->topextra->focusData ) {
	// this widget is no longer a top-level widget, so get rid
	// of old focus data
	delete extra->topextra->focusData;
	extra->topextra->focusData = 0;
    }
}

/*!
  \fn void TQWidget::recreate( TQWidget *parent, WFlags f, const TQPoint & p, bool showIt )

  \obsolete

  This method is provided to aid porting from TQt 1.0 to 2.0. It has
  been renamed reparent() in TQt 2.0.
*/

/*!
    \property TQWidget::frameSize
    \brief the size of the widget including any window frame
*/
TQSize TQWidget::frameSize() const
{
    if ( isTopLevel() && !isPopup() ) {
	if ( fstrut_dirty )
	    updateFrameStrut();
	TQWidget *that = (TQWidget *) this;
	TQTLWExtra *top = that->topData();
	return TQSize( crect.width() + top->fleft + top->fright,
		      crect.height() + top->ftop + top->fbottom );
    }
    return crect.size();
}

/*!
    \internal

    Recursive function that updates \a widget and all its children,
    if they have some parent background origin.
*/
static void tqt_update_bg_recursive( TQWidget *widget )
{
    if ( !widget || widget->isHidden() || widget->backgroundOrigin() == TQWidget::WidgetOrigin || !widget->backgroundPixmap() )
	return;

    const TQObjectList *lst = widget->children();

    if ( lst ) {
	TQObjectListIterator it( *lst );
	TQWidget *widget;
	while ( (widget = (TQWidget*)it.current()) ) {
	    ++it;
	    if ( widget->isWidgetType() && !widget->isHidden() && !widget->isTopLevel() && !widget->testWFlags(TQt::WSubWindow) )
		    tqt_update_bg_recursive( widget );
	}
    }
    TQApplication::postEvent( widget, new TQPaintEvent( widget->clipRegion(), !widget->testWFlags(TQt::WRepaintNoErase) ) );
}

/*!
    \overload

    This corresponds to move( TQPoint(\a x, \a y) ).
*/

void TQWidget::move( int x, int y )
{
    TQPoint oldp(pos());
    internalSetGeometry( x + geometry().x() - TQWidget::x(),
			 y + geometry().y() - TQWidget::y(),
			 width(), height(), true );
    if ( isVisible() && oldp != pos() )
	tqt_update_bg_recursive( this );
}

/*!
    \overload

    This corresponds to resize( TQSize(\a w, \a h) ).
*/
void TQWidget::resize( int w, int h )
{
    internalSetGeometry( geometry().x(), geometry().y(), w, h, false );
    setWState( WState_Resized );
}

/*!
    \overload

    This corresponds to setGeometry( TQRect(\a x, \a y, \a w, \a h) ).
*/
void TQWidget::setGeometry( int x, int y, int w, int h )
{
    TQPoint oldp( pos( ));
    internalSetGeometry( x, y, w, h, true );
    setWState( WState_Resized );
    if ( isVisible() && oldp != pos() )
	tqt_update_bg_recursive( this );
}

/*!
    \property TQWidget::focusEnabled
    \brief whether the widget accepts keyboard focus

    Keyboard focus is initially disabled (i.e. focusPolicy() ==
    \c TQWidget::NoFocus).

    You must enable keyboard focus for a widget if it processes
    keyboard events. This is normally done from the widget's
    constructor. For instance, the TQLineEdit constructor calls
    setFocusPolicy(TQWidget::StrongFocus).

    \sa setFocusPolicy(), focusInEvent(), focusOutEvent(), keyPressEvent(),
      keyReleaseEvent(), isEnabled()
*/

/*!
    \enum TQWidget::FocusPolicy

    This enum type defines the various policies a widget can have with
    respect to acquiring keyboard focus.

    \value TabFocus  the widget accepts focus by tabbing.
    \value ClickFocus  the widget accepts focus by clicking.
    \value StrongFocus  the widget accepts focus by both tabbing
			and clicking. On Mac OS X this will also
			be indicate that the widget accepts tab focus
			when in 'Text/List focus mode'.
    \value WheelFocus  like StrongFocus plus the widget accepts
			focus by using the mouse wheel.
    \value NoFocus  the widget does not accept focus.

*/

/*!
    \property TQWidget::focusPolicy
    \brief the way the widget accepts keyboard focus

    The policy is \c TQWidget::TabFocus if the widget accepts keyboard
    focus by tabbing, \c TQWidget::ClickFocus if the widget accepts
    focus by clicking, \c TQWidget::StrongFocus if it accepts both, and
    \c TQWidget::NoFocus (the default) if it does not accept focus at
    all.

    You must enable keyboard focus for a widget if it processes
    keyboard events. This is normally done from the widget's
    constructor. For instance, the TQLineEdit constructor calls
    setFocusPolicy(TQWidget::StrongFocus).

    \sa focusEnabled, focusInEvent(), focusOutEvent(), keyPressEvent(),
      keyReleaseEvent(), enabled
*/

void TQWidget::setFocusPolicy( FocusPolicy policy )
{
    if ( focusProxy() )
	focusProxy()->setFocusPolicy( policy );
    if ( policy != NoFocus ) {
	TQFocusData * f = focusData( true );
	if ( f->focusWidgets.findRef( this ) < 0 )
	    f->focusWidgets.append( this );
    }
    focus_policy = (uint) policy;
}

/*!
    \property TQWidget::updatesEnabled
    \brief whether updates are enabled

    Calling update() and repaint() has no effect if updates are
    disabled. Paint events from the window system are processed
    normally even if updates are disabled.

    setUpdatesEnabled() is normally used to disable updates for a
    short period of time, for instance to avoid screen flicker during
    large changes.

    Example:
    \code
	setUpdatesEnabled( false );
	bigVisualChanges();
	setUpdatesEnabled( true );
	repaint();
    \endcode

    \sa update(), repaint(), paintEvent()
*/
void TQWidget::setUpdatesEnabled( bool enable )
{
    if ( enable )
	clearWState( WState_BlockUpdates );
    else
	setWState( WState_BlockUpdates );
}

/*!
    Shows the widget and its child widgets.

    If its size or position has changed, TQt guarantees that a widget
    gets move and resize events just before it is shown.

    You almost never have to reimplement this function. If you need to
    change some settings before a widget is shown, use showEvent()
    instead. If you need to do some delayed initialization use
    polish().

    \sa showEvent(), hide(), showMinimized(), showMaximized(),
    showNormal(), isVisible(), polish()
*/

void TQWidget::show()
{
    if ( testWState(WState_Visible) )
	return;

    bool wasHidden = isHidden();
    bool postLayoutHint = !isTopLevel() && wasHidden;
    clearWState( WState_ForceHide | WState_CreatedHidden );

    if ( !isTopLevel() && !parentWidget()->isVisible() ) {
	// we should become visible, but one of our ancestors is
	// explicitly hidden. Since we cleared the ForceHide flag, our
	// immediate parent will call show() on us again during its
	// own processing of show().
	if ( wasHidden ) {
	    TQEvent showToParentEvent( TQEvent::ShowToParent );
	    TQApplication::sendEvent( this, &showToParentEvent );
	}
	if ( postLayoutHint )
	    TQApplication::postEvent( parentWidget(),
				     new TQEvent(TQEvent::LayoutHint) );
	return;
    }

    in_show = true; // set qws recursion watch

    TQApplication::sendPostedEvents( this, TQEvent::ChildInserted );

    uint state = isTopLevel() ? windowState() : 0;
#ifndef Q_OS_TEMP
    if ( isTopLevel() && !testWState( WState_Resized ) ) {
	// do this before sending the posted resize events. Otherwise
	// the layout would catch the resize event and may expand the
	// minimum size.
	TQSize s = tqt_naturalWidgetSize( this );
	if ( !s.isEmpty() )
	    resize( s );
    }
#endif // Q_OS_TEMP

    TQApplication::sendPostedEvents( this, TQEvent::Move );
    TQApplication::sendPostedEvents( this, TQEvent::Resize );

    // the resizing and layouting might have changed the window state
    if (isTopLevel() && windowState() != state)
	setWindowState(state);

    setWState( WState_Visible );

    if ( parentWidget() )
	TQApplication::sendPostedEvents( parentWidget(),
					TQEvent::ChildInserted );

    if ( extra ) {
	int w = crect.width();
	int h = crect.height();
	if ( w < extra->minw || h < extra->minh ||
	     w > extra->maxw || h > extra->maxh ) {
	    w = TQMAX( extra->minw, TQMIN( w, extra->maxw ));
	    h = TQMAX( extra->minh, TQMIN( h, extra->maxh ));
	    resize( w, h );			// deferred resize
	}
    }

    if ( testWFlags(WStyle_Tool) || isPopup() ) {
	raise();
    } else if ( testWFlags(WType_TopLevel) ) {
	while ( TQApplication::activePopupWidget() ) {
	    if ( !TQApplication::activePopupWidget()->close() )
		break;
	}
    }

    if ( !testWState(WState_Polished) )
	polish();

    showChildren( false );

    if ( postLayoutHint )
	TQApplication::postEvent( parentWidget(),
				 new TQEvent(TQEvent::LayoutHint) );

    // Required for Mac, not sure whether we should always do that
    if( isTopLevel() )
	TQApplication::sendPostedEvents(0, TQEvent::LayoutHint);

    // On Windows, show the popup now so that our own focus handling
    // stores the correct old focus widget even if it's stolen in the showevent
#if defined(TQ_WS_WIN)
    if ( testWFlags(WType_Popup) )
	tqApp->openPopup( this );
#endif

    TQShowEvent showEvent;
    TQApplication::sendEvent( this, &showEvent );

    if ( testWFlags(WShowModal) ) {
	// tqt_enter_modal *before* show, otherwise the initial
	// stacking might be wrong
	tqt_enter_modal( this );
    }

    // do not show the window directly, but post a show-window request
    // to reduce flicker with widgets in layouts
    if ( postLayoutHint )
	TQApplication::postEvent( this, new TQEvent( TQEvent::ShowWindowRequest ) );
    else
	showWindow();

#if !defined(TQ_WS_WIN)
    if ( testWFlags(WType_Popup) )
	tqApp->openPopup( this );
#endif

#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::ObjectShow );
#endif

    in_show = false;  // reset qws recursion watch
}

/*! \fn void TQWidget::iconify()
    \obsolete
*/

/*!
    Hides the widget.

    You almost never have to reimplement this function. If you need to
    do something after a widget is hidden, use hideEvent() instead.

    \sa hideEvent(), isHidden(), show(), showMinimized(), isVisible(), close()
*/

void TQWidget::hide()
{
    clearWState( WState_CreatedHidden );
    if ( testWState(WState_ForceHide) )
	return;

    setWState( WState_ForceHide );

    if ( testWFlags(WType_Popup) )
	tqApp->closePopup( this );

    // Move test modal here.  Otherwise, a modal dialog could get
    // destroyed and we lose all access to its parent because we haven't
    // left modality.  (Eg. modal Progress Dialog)
    if ( testWFlags(WShowModal) )
	tqt_leave_modal( this );

#if defined(TQ_WS_WIN)
    if ( isTopLevel() && !isPopup() && parentWidget() && isActiveWindow() )
	parentWidget()->setActiveWindow();	// Activate parent
#endif

    hideWindow();

    if ( testWState(WState_Visible) ) {
	clearWState( WState_Visible );

	// next bit tries to move the focus if the focus widget is now
	// hidden.
	if ( tqApp && tqApp->focusWidget() == this )
  	    focusNextPrevChild( true );
	TQHideEvent hideEvent;
	TQApplication::sendEvent( this, &hideEvent );
	hideChildren( false );

#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( this, 0, TQAccessible::ObjectHide );
#endif
    } else {
	TQEvent hideToParentEvent( TQEvent::HideToParent );
	TQApplication::sendEvent( this, &hideToParentEvent );
    }

    // post layout hint for non toplevels. The parent widget check is
    // necessary since the function is called in the destructor
    if ( !isTopLevel() && parentWidget() )
	TQApplication::postEvent( parentWidget(),
				 new TQEvent( TQEvent::LayoutHint) );
}

void TQWidget::setShown( bool show )
{
    if ( show )
	this->show();
    else
	hide();
}

void TQWidget::setHidden( bool hide )
{
    if ( hide )
	this->hide();
    else
	show();
}

void TQWidget::showChildren( bool spontaneous )
{
     if ( children() ) {
	TQObjectListIt it(*children());
	TQObject *object;
	TQWidget *widget;
	while ( it ) {
	    object = it.current();
	    ++it;
	    if ( object->isWidgetType() ) {
		widget = (TQWidget*)object;
		if ( !widget->isTopLevel() && widget->isShown() ) {
		    if ( spontaneous ) {
			widget->showChildren( spontaneous );
			TQShowEvent e;
			TQApplication::sendSpontaneousEvent( widget, &e );
		    } else {
			widget->show();
		    }
		}
	    }
	}
    }
}

void TQWidget::hideChildren( bool spontaneous )
{
     if ( children() ) {
	TQObjectListIt it(*children());
	TQObject *object;
	TQWidget *widget;
	while ( it ) {
	    object = it.current();
	    ++it;
	    if ( object->isWidgetType() ) {
		widget = (TQWidget*)object;
		if ( !widget->isTopLevel() && widget->isShown() ) {
		    if ( !spontaneous )
			widget->clearWState( WState_Visible );
		    widget->hideChildren( spontaneous );
		    TQHideEvent e;
		    if ( spontaneous )
			TQApplication::sendSpontaneousEvent( widget, &e );
		    else
			TQApplication::sendEvent( widget, &e );
		}
	    }
	}
    }
}


/*!
    Delayed initialization of a widget.

    This function will be called \e after a widget has been fully
    created and \e before it is shown the very first time.

    Polishing is useful for final initialization which depends on
    having an instantiated widget. This is something a constructor
    cannot guarantee since the initialization of the subclasses might
    not be finished.

    After this function, the widget has a proper font and palette and
    TQApplication::polish() has been called.

    Remember to call TQWidget's implementation first when reimplementing this
    function to ensure that your program does not end up in infinite recursion.

    \sa constPolish(), TQApplication::polish()
*/

void TQWidget::polish()
{
#ifndef TQT_NO_WIDGET_TOPEXTRA
    if ( isTopLevel() ) {
	const TQPixmap *pm = icon();
	if ( !pm || pm->isNull() ) {
	    TQWidget *mw = (TQWidget *)parent();
	    pm = mw ? mw->icon() : 0;
	    if ( pm && !pm->isNull() )
		setIcon( *pm );
	    else {
		mw = mw ? mw->topLevelWidget() : 0;
		pm = mw ? mw->icon() : 0;
		if ( pm && !pm->isNull() )
		    setIcon( *pm );
		else {
		    mw = tqApp ? tqApp->mainWidget() : 0;
		    pm = mw ? mw->icon() : 0;
		    if ( pm && !pm->isNull() )
			setIcon( *pm );
		}
	    }
	}
    }
#endif
    if ( !testWState(WState_Polished) ) {
	if ( ! own_font &&
	     ! TQApplication::font( this ).isCopyOf( TQApplication::font() ) )
	    unsetFont();
#ifndef TQT_NO_PALETTE
	if ( ! own_palette &&
	     ! TQApplication::palette( this ).isCopyOf( TQApplication::palette() ) )
	    unsetPalette();
#endif
	setWState(WState_Polished);
	tqApp->polish( this );
	TQApplication::sendPostedEvents( this, TQEvent::ChildInserted );
    }
}


/*!
    \fn void TQWidget::constPolish() const

    Ensures that the widget is properly initialized by calling
    polish().

    Call constPolish() from functions like sizeHint() that depends on
    the widget being initialized, and that may be called before
    show().

    \warning Do not call constPolish() on a widget from inside that
    widget's constructor.

    \sa polish()
*/

/*!
    \overload

    Closes this widget. Returns true if the widget was closed;
    otherwise returns false.

    If \a alsoDelete is true or the widget has the \c
    WDestructiveClose widget flag, the widget is also deleted. The
    widget can prevent itself from being closed by rejecting the
    \l TQCloseEvent it gets. A close events is delivered to the widget
    no matter if the widget is visible or not.

    The TQApplication::lastWindowClosed() signal is emitted when the
    last visible top level widget is closed.

    Note that closing the \l TQApplication::mainWidget() terminates the
    application.

    \sa closeEvent(), TQCloseEvent, hide(), TQApplication::quit(),
    TQApplication::setMainWidget(), TQApplication::lastWindowClosed()
*/

bool TQWidget::close( bool alsoDelete )
{
    if ( is_closing )
	return true;
    is_closing = 1;
    WId id	= winId();
    bool isMain = tqApp->mainWidget() == this;
    bool checkLastWindowClosed = isTopLevel() && !isPopup();
    bool deleted = false;
    TQCloseEvent e;
    TQApplication::sendEvent( this, &e );
    deleted = !TQWidget::find(id);
    if ( !deleted && !e.isAccepted() ) {
	is_closing = 0;
	return false;
    }
    if ( !deleted && !isHidden() )
	hide();
    if ( checkLastWindowClosed
	 && tqApp->receivers(TQ_SIGNAL(lastWindowClosed())) ) {
	/* if there is no non-withdrawn top level window left (except
	   the desktop, popups, or dialogs with parents), we emit the
	   lastWindowClosed signal */
	TQWidgetList *list   = tqApp->topLevelWidgets();
	TQWidget     *widget = list->first();
	while ( widget ) {
	    if ( !widget->isHidden()
		 && !widget->isDesktop()
		 && !widget->isPopup()
		 && (!widget->isDialog() || !widget->parentWidget()))
		break;
	    widget = list->next();
	}
	delete list;
	if ( widget == 0 )
	    emit tqApp->lastWindowClosed();
    }
    if ( isMain )
	tqApp->quit();
    if ( deleted )
	return true;
    is_closing = 0;
    if ( alsoDelete )
	delete this;
    else if ( testWFlags(WDestructiveClose) ) {
	clearWFlags(WDestructiveClose);
	deleteLater();
    }
    return true;
}


/*!
    \fn bool TQWidget::close()

    Closes this widget. Returns true if the widget was closed;
    otherwise returns false.

    First it sends the widget a TQCloseEvent. The widget is \link
    hide() hidden\endlink if it \link TQCloseEvent::accept()
    accepts\endlink the close event. The default implementation of
    TQWidget::closeEvent() accepts the close event.

    The \l TQApplication::lastWindowClosed() signal is emitted when the
    last visible top level widget is closed.

*/

/*!
    \property TQWidget::visible
    \brief whether the widget is visible

    Calling show() sets the widget to visible status if all its parent
    widgets up to the top-level widget are visible. If an ancestor is
    not visible, the widget won't become visible until all its
    ancestors are shown.

    Calling hide() hides a widget explicitly. An explicitly hidden
    widget will never become visible, even if all its ancestors become
    visible, unless you show it.

    A widget receives show and hide events when its visibility status
    changes. Between a hide and a show event, there is no need to
    waste CPU cycles preparing or displaying information to the user.
    A video application, for example, might simply stop generating new
    frames.

    A widget that happens to be obscured by other windows on the
    screen is considered to be visible. The same applies to iconified
    top-level widgets and windows that exist on another virtual
    desktop (on platforms that support this concept). A widget
    receives spontaneous show and hide events when its mapping status
    is changed by the window system, e.g. a spontaneous hide event
    when the user minimizes the window, and a spontaneous show event
    when the window is restored again.

    \sa show(), hide(), isHidden(), isVisibleTo(), isMinimized(),
    showEvent(), hideEvent()
*/


/*!
    Returns true if this widget would become visible if \a ancestor is
    shown; otherwise returns false.

    The true case occurs if neither the widget itself nor any parent
    up to but excluding \a ancestor has been explicitly hidden.

    This function will still return true if the widget is obscured by
    other windows on the screen, but could be physically visible if it
    or they were to be moved.

    isVisibleTo(0) is identical to isVisible().

    \sa show() hide() isVisible()
*/

bool TQWidget::isVisibleTo(TQWidget* ancestor) const
{
    if ( !ancestor )
	return isVisible();
    const TQWidget * w = this;
    while ( w
	    && w->isShown()
	    && !w->isTopLevel()
	    && w->parentWidget()
	    && w->parentWidget() != ancestor )
	w = w->parentWidget();
    return w->isShown();
}


/*!
  \fn bool TQWidget::isVisibleToTLW() const
  \obsolete

  This function is deprecated. It is equivalent to isVisible()
*/

/*!
    \property TQWidget::hidden
    \brief whether the widget is explicitly hidden

    If false, the widget is visible or would become visible if all its
    ancestors became visible.

    \sa hide(), show(), isVisible(), isVisibleTo(), shown
*/

/*!
    \property TQWidget::shown
    \brief whether the widget is shown

    If true, the widget is visible or would become visible if all its
    ancestors became visible.

    \sa hide(), show(), isVisible(), isVisibleTo(), hidden
*/

/*!
    \property TQWidget::visibleRect
    \brief the visible rectangle

    \obsolete

    No longer necessary, you can simply call repaint(). If you do not
    need the rectangle for repaint(), use clipRegion() instead.
*/
TQRect TQWidget::visibleRect() const
{
    TQRect r = rect();
    const TQWidget * w = this;
    int ox = 0;
    int oy = 0;
    while ( w
	    && w->isVisible()
	    && !w->isTopLevel()
	    && w->parentWidget() ) {
	ox -= w->x();
	oy -= w->y();
	w = w->parentWidget();
	r = r.intersect( TQRect( ox, oy, w->width(), w->height() ) );
    }
    if ( !w->isVisible() )
	return TQRect();
    return r;
}

/*!
    Returns the unobscured region where paint events can occur.

    For visible widgets, this is an approximation of the area not
    covered by other widgets; otherwise, this is an empty region.

    The repaint() function calls this function if necessary, so in
    general you do not need to call it.

*/
TQRegion TQWidget::clipRegion() const
{
    return visibleRect();
}


/*!
    Adjusts the size of the widget to fit the contents.

    Uses sizeHint() if valid (i.e if the size hint's width and height
    are \>= 0), otherwise sets the size to the children rectangle (the
    union of all child widget geometries).

    \sa sizeHint(), childrenRect()
*/

void TQWidget::adjustSize()
{
    TQApplication::sendPostedEvents( 0, TQEvent::ChildInserted );
    TQApplication::sendPostedEvents( 0, TQEvent::LayoutHint );
    if ( !testWState(WState_Polished) )
	polish();
    TQSize s = sizeHint();

    if ( isTopLevel() ) {

#if defined(TQ_WS_X11)
	TQRect screen = TQApplication::desktop()->screenGeometry( x11Screen() );
#else // all others
	TQRect screen = TQApplication::desktop()->screenGeometry( pos() );
#endif

#ifndef TQT_NO_LAYOUT
	if ( layout() ) {
	    if ( layout()->hasHeightForWidth() ) {
		s = s.boundedTo( screen.size() );
		s.setHeight( layout()->totalHeightForWidth( s.width() ) );
	    }
	} else
#endif
	{
	    if ( sizePolicy().hasHeightForWidth() ) {
		s = s.boundedTo( screen.size() );
		s.setHeight( heightForWidth( s.width() ) );
	    }
	}
    }
    if ( s.isValid() ) {
	resize( s );
	return;
    }
    TQRect r = childrenRect();			// get children rectangle
    if ( r.isNull() )				// probably no widgets
	return;
    resize( r.width() + 2 * r.x(), r.height() + 2 * r.y() );
}


/*!
    \property TQWidget::sizeHint
    \brief the recommended size for the widget

    If the value of this property is an invalid size, no size is
    recommended.

    The default implementation of sizeHint() returns an invalid size
    if there is no layout for this widget, and returns the layout's
    preferred size otherwise.

    \sa TQSize::isValid(), minimumSizeHint(), sizePolicy(),
    setMinimumSize(), updateGeometry()
*/

TQSize TQWidget::sizeHint() const
{
#ifndef TQT_NO_LAYOUT
    if ( layout() )
	return layout()->totalSizeHint();
#endif
    return TQSize( -1, -1 );
}

/*!
    \property TQWidget::minimumSizeHint
    \brief the recommended minimum size for the widget

    If the value of this property is an invalid size, no minimum size
    is recommended.

    The default implementation of minimumSizeHint() returns an invalid
    size if there is no layout for this widget, and returns the
    layout's minimum size otherwise. Most built-in widgets reimplement
    minimumSizeHint().

    \l TQLayout will never resize a widget to a size smaller than
    minimumSizeHint.

    \sa TQSize::isValid(), resize(), setMinimumSize(), sizePolicy()
*/
TQSize TQWidget::minimumSizeHint() const
{
#ifndef TQT_NO_LAYOUT
    if ( layout() )
	return layout()->totalMinimumSize();
#endif
    return TQSize( -1, -1 );
}


/*!
    \fn TQWidget *TQWidget::parentWidget( bool sameWindow ) const

    Returns the parent of this widget, or 0 if it does not have any
    parent widget. If \a sameWindow is true and the widget is top
    level returns 0; otherwise returns the widget's parent.
*/

/*!
    \fn WFlags TQWidget::testWFlags( WFlags f ) const

    Returns the bitwise AND of the widget flags and \a f.

    Widget flags are a combination of \l{TQt::WidgetFlags}.

    If you want to test for the presence of multiple flags (or
    composite flags such as \c WStyle_Splash), test the
    return value for equality against the argument. For example:

    \code
    int flags = WStyle_Tool | WStyle_NoBorder;
    if ( testWFlags(flags) )
	... // WStyle_Tool or WStyle_NoBorder or both are set
    if ( testWFlags(flags) == flags )
        ... // both WStyle_Tool and WStyle_NoBorder are set
    \endcode

    \sa getWFlags(), setWFlags(), clearWFlags()
*/

/*!
  \fn WState TQWidget::testWState( WState s ) const
  \internal

  Returns the bitwise AND of the widget states and \a s.
*/

/*!
  \fn uint TQWidget::getWState() const

  \internal

  Returns the current widget state.
*/
/*!
  \fn void TQWidget::clearWState( uint n )

  \internal

  Clears the widgets states \a n.
*/
/*!
  \fn void TQWidget::setWState( uint n )

  \internal

  Sets the widgets states \a n.
*/



/*****************************************************************************
  TQWidget event handling
 *****************************************************************************/

/*!
    This is the main event handler; it handles event \a e. You can
    reimplement this function in a subclass, but we recommend using
    one of the specialized event handlers instead.

    The main event handler first passes an event through all \link
    TQObject::installEventFilter() event filters\endlink that have been
    installed. If none of the filters intercept the event, it calls
    one of the specialized event handlers.

    Key press and release events are treated differently from other
    events. event() checks for Tab and Shift+Tab and tries to move the
    focus appropriately. If there is no widget to move the focus to
    (or the key press is not Tab or Shift+Tab), event() calls
    keyPressEvent().

    This function returns true if it is able to pass the event over to
    someone (i.e. someone wanted the event); otherwise returns false.

    \sa closeEvent(), focusInEvent(), focusOutEvent(), enterEvent(),
    keyPressEvent(), keyReleaseEvent(), leaveEvent(),
    mouseDoubleClickEvent(), mouseMoveEvent(), mousePressEvent(),
    mouseReleaseEvent(), moveEvent(), paintEvent(), resizeEvent(),
    TQObject::event(), TQObject::timerEvent()
*/

bool TQWidget::event( TQEvent *e )
{
    if ( TQObject::event( e ) )
	return true;

    switch ( e->type() ) {
	case TQEvent::MouseMove:
	    mouseMoveEvent( (TQMouseEvent*)e );
	    if ( ! ((TQMouseEvent*)e)->isAccepted() )
		return false;
	    break;

	case TQEvent::MouseButtonPress:
	    // Don't reset input context here. Whether reset or not is
	    // a responsibility of input method. reset() will be
	    // called by mouseHandler() of input method if necessary
	    // via mousePressEvent() of text widgets.
#if 0
	    resetInputContext();
#endif
	    mousePressEvent( (TQMouseEvent*)e );
	    if ( ! ((TQMouseEvent*)e)->isAccepted() )
		return false;
	    break;

	case TQEvent::MouseButtonRelease:
	    mouseReleaseEvent( (TQMouseEvent*)e );
	    if ( ! ((TQMouseEvent*)e)->isAccepted() )
		return false;
	    break;

	case TQEvent::MouseButtonDblClick:
	    mouseDoubleClickEvent( (TQMouseEvent*)e );
	    if ( ! ((TQMouseEvent*)e)->isAccepted() )
		return false;
	    break;
#ifndef TQT_NO_WHEELEVENT
	case TQEvent::Wheel:
	    wheelEvent( (TQWheelEvent*)e );
	    if ( ! ((TQWheelEvent*)e)->isAccepted() )
		return false;
	    break;
#endif
	case TQEvent::TabletMove:
	case TQEvent::TabletPress:
	case TQEvent::TabletRelease:
	    tabletEvent( (TQTabletEvent*)e );
	    if ( ! ((TQTabletEvent*)e)->isAccepted() )
		return false;
	    break;
	case TQEvent::Accel:
	    ((TQKeyEvent*)e)->ignore();
	    return false;
	case TQEvent::KeyPress: {
	    TQKeyEvent *k = (TQKeyEvent *)e;
	    bool res = false;
	    if ( !(k->state() & ControlButton || k->state() & AltButton) ) {
		if ( k->key() == Key_Backtab ||
		     (k->key() == Key_Tab &&
		      (k->state() & ShiftButton)) ) {
		    TQFocusEvent::setReason( TQFocusEvent::Backtab );
		    res = focusNextPrevChild( false );
		    TQFocusEvent::resetReason();

		} else if ( k->key() == Key_Tab ) {
		    TQFocusEvent::setReason( TQFocusEvent::Tab );
		    res = focusNextPrevChild( true );
		    TQFocusEvent::resetReason();
		}
		if ( res )
		    break;
	    }
	    keyPressEvent( k );
	    if ( !k->isAccepted() )
		return false;
	    }
	    break;

	case TQEvent::KeyRelease:
	    keyReleaseEvent( (TQKeyEvent*)e );
	    if ( ! ((TQKeyEvent*)e)->isAccepted() )
		return false;
	    break;

	case TQEvent::IMStart: {
	    TQIMEvent *i = (TQIMEvent *) e;
	    imStartEvent(i);
	    if (! i->isAccepted())
		return false;
	    }
	    break;

	case TQEvent::IMCompose: {
	    TQIMEvent *i = (TQIMEvent *) e;
	    imComposeEvent(i);
	    if (! i->isAccepted())
		return false;
	    }
	    break;

	case TQEvent::IMEnd: {
	    TQIMEvent *i = (TQIMEvent *) e;
	    imEndEvent(i);
	    if (! i->isAccepted())
		return false;
	    }
	    break;

	case TQEvent::FocusIn:
	    focusInEvent( (TQFocusEvent*)e );
	    setFontSys();
	    break;

	case TQEvent::FocusOut:
	    focusOutEvent( (TQFocusEvent*)e );
	    break;

	case TQEvent::Enter:
	    enterEvent( e );
	    break;

	case TQEvent::Leave:
	     leaveEvent( e );
	    break;

	case TQEvent::Paint:
	    // At this point the event has to be delivered, regardless
	    // whether the widget isVisible() or not because it
	    // already went through the filters
	    paintEvent( (TQPaintEvent*)e );
	    break;

	case TQEvent::Move:
	    moveEvent( (TQMoveEvent*)e );
	    break;

	case TQEvent::Resize:
	    resizeEvent( (TQResizeEvent*)e );
	    break;

	case TQEvent::Close: {
	    TQCloseEvent *c = (TQCloseEvent *)e;
	    closeEvent( c );
	    if ( !c->isAccepted() )
		return false;
	    }
	    break;

	case TQEvent::ContextMenu: {
	    TQContextMenuEvent *c = (TQContextMenuEvent *)e;
	    contextMenuEvent( c );
	    if ( !c->isAccepted() )
		return false;
	    }
	    break;

#ifndef TQT_NO_DRAGANDDROP
	case TQEvent::Drop:
	    dropEvent( (TQDropEvent*) e);
	    break;

	case TQEvent::DragEnter:
	    dragEnterEvent( (TQDragEnterEvent*) e);
	    break;

	case TQEvent::DragMove:
	    dragMoveEvent( (TQDragMoveEvent*) e);
	    break;

	case TQEvent::DragLeave:
	    dragLeaveEvent( (TQDragLeaveEvent*) e);
	    break;
#endif

	case TQEvent::Show:
	    showEvent( (TQShowEvent*) e);
	    break;

	case TQEvent::Hide:
	    hideEvent( (TQHideEvent*) e);
	    break;

	case TQEvent::ShowWindowRequest:
	    if ( isShown() )
		showWindow();
	    break;

	case TQEvent::ParentFontChange:
	    if ( isTopLevel() )
		break;
	    // fall through
	case TQEvent::ApplicationFontChange:
	    if ( own_font )
		setFont( fnt.resolve( tqt_naturalWidgetFont( this ) ) );
	    else
		unsetFont();
	    break;

#ifndef TQT_NO_PALETTE
	case TQEvent::ParentPaletteChange:
	    if ( isTopLevel() )
		break;
	    // fall through
	case TQEvent::ApplicationPaletteChange:
	    if ( !own_palette && !isDesktop() )
		unsetPalette();
# if defined(TQ_WS_QWS) && !defined (TQT_NO_QWS_MANAGER)
	    if ( isTopLevel() && topData()->qwsManager ) {
		TQRegion r( topData()->qwsManager->region() );
		TQApplication::postEvent(topData()->qwsManager, new TQPaintEvent(r, false) );
	    }
# endif
	    break;
#endif

	case TQEvent::WindowActivate:
	case TQEvent::WindowDeactivate:
	    windowActivationChange( e->type() != TQEvent::WindowActivate );
	    if ( children() ) {
		TQObjectListIt it( *children() );
		TQObject *o;
		while( ( o = it.current() ) != 0 ) {
		    ++it;
		    if ( o->isWidgetType() &&
			 ((TQWidget*)o)->isVisible() &&
			 !((TQWidget*)o)->isTopLevel() )
			TQApplication::sendEvent( o, e );
		}
	    }
	    break;

	case TQEvent::LanguageChange:
	case TQEvent::LocaleChange:
	    if ( children() ) {
		TQObjectListIt it( *children() );
		TQObject *o;
		while( ( o = it.current() ) != 0 ) {
		    ++it;
		    TQApplication::sendEvent( o, e );
		}
	    }
	    if ( e->type() == TQEvent::LanguageChange ) {
		int index = metaObject()->findSlot( "languageChange()", true );
		if ( index >= 0 )
		    tqt_invoke( index, 0 );
	    }
	    update();
	    break;
#ifndef TQT_NO_LAYOUT
	case TQEvent::LayoutDirectionChange:
	    if ( layout() ) {
		layout()->activate();
	    } else {
		TQObjectList* llist = queryList( "TQLayout", 0, true, true );
		TQObjectListIt lit( *llist );
		TQLayout *lay;
		while ( ( lay = (TQLayout*)lit.current() ) != 0 ) {
		    ++lit;
		    lay->activate();
		}
		delete llist;
	    }
	    update();
	    break;
#endif

    case TQEvent::WindowStateChange:
	{
	    TQEvent::Type type;
	    if (isMinimized())
		type = TQEvent::ShowMinimized;
	    else if (isFullScreen())
		type = TQEvent::ShowFullScreen;
	    else if (isMaximized())
		type = TQEvent::ShowMaximized;
	    else
		type = TQEvent::ShowNormal;

	    TQApplication::postEvent(this, new TQEvent(type));
	    break;
	}

    case TQEvent::WindowBlocked:
    case TQEvent::WindowUnblocked:
	if ( children() ) {
	    TQObjectListIt it( *children() );
	    TQObject *o;
	    while( ( o = it.current() ) != 0 ) {
		++it;
                TQWidget *w = ::tqt_cast<TQWidget*>(o);
                if (w && !w->testWFlags(TQt::WShowModal))
	            TQApplication::sendEvent( o, e );
	    }
	}
	break;

    default:
	return false;
    }
    return true;
}

/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive mouse move events for the widget.

    If mouse tracking is switched off, mouse move events only occur if
    a mouse button is pressed while the mouse is being moved. If mouse
    tracking is switched on, mouse move events occur even if no mouse
    button is pressed.

    TQMouseEvent::pos() reports the position of the mouse cursor,
    relative to this widget. For press and release events, the
    position is usually the same as the position of the last mouse
    move event, but it might be different if the user's hand shakes.
    This is a feature of the underlying window system, not TQt.

    \sa setMouseTracking(), mousePressEvent(), mouseReleaseEvent(),
    mouseDoubleClickEvent(), event(), TQMouseEvent
*/

void TQWidget::mouseMoveEvent( TQMouseEvent * e)
{
    e->ignore();
}

/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive mouse press events for the widget.

    If you create new widgets in the mousePressEvent() the
    mouseReleaseEvent() may not end up where you expect, depending on
    the underlying window system (or X11 window manager), the widgets'
    location and maybe more.

    The default implementation implements the closing of popup widgets
    when you click outside the window. For other widget types it does
    nothing.

    \sa mouseReleaseEvent(), mouseDoubleClickEvent(),
    mouseMoveEvent(), event(), TQMouseEvent
*/

void TQWidget::mousePressEvent( TQMouseEvent *e )
{
    e->ignore();
    if ( isPopup() ) {
	e->accept();
	TQWidget* w;
	while ( (w = tqApp->activePopupWidget() ) && w != this ){
	    w->close();
	    if (tqApp->activePopupWidget() == w) // widget does not want to dissappear
		w->hide(); // hide at least
	}
	if (!rect().contains(e->pos()) ){
	    close();
	}
    }
}

/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive mouse release events for the widget.

    \sa mouseReleaseEvent(), mouseDoubleClickEvent(),
    mouseMoveEvent(), event(),  TQMouseEvent
*/

void TQWidget::mouseReleaseEvent( TQMouseEvent * e )
{
    e->ignore();
}

/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive mouse double click events for the widget.

    The default implementation generates a normal mouse press event.

    Note that the widgets gets a mousePressEvent() and a
    mouseReleaseEvent() before the mouseDoubleClickEvent().

    \sa mousePressEvent(), mouseReleaseEvent() mouseMoveEvent(),
    event(), TQMouseEvent
*/

void TQWidget::mouseDoubleClickEvent( TQMouseEvent *e )
{
    mousePressEvent( e );			// try mouse press event
}

#ifndef TQT_NO_WHEELEVENT
/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive wheel events for the widget.

    If you reimplement this handler, it is very important that you
    \link TQWheelEvent ignore()\endlink the event if you do not handle
    it, so that the widget's parent can interpret it.

    The default implementation ignores the event.

    \sa TQWheelEvent::ignore(), TQWheelEvent::accept(), event(),
    TQWheelEvent
*/

void TQWidget::wheelEvent( TQWheelEvent *e )
{
    e->ignore();
}
#endif

/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive tablet events for the widget.

    If you reimplement this handler, it is very important that you
    \link TQTabletEvent ignore()\endlink the event if you do not handle
    it, so that the widget's parent can interpret it.

    The default implementation ignores the event.

    \sa TQTabletEvent::ignore(), TQTabletEvent::accept(), event(),
    TQTabletEvent
*/

void TQWidget::tabletEvent( TQTabletEvent *e )
{
    e->ignore();
}

/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive key press events for the widget.

    A widget must call setFocusPolicy() to accept focus initially and
    have focus in order to receive a key press event.

    If you reimplement this handler, it is very important that you
    explicitly \link TQKeyEvent::ignore() ignore\endlink the event
    if you do not understand it, so that the widget's parent can
    interpret it; otherwise, the event will be implicitly accepted.
    Although top-level widgets are able to choose whether to accept
    or ignore unknown events because they have no parent widgets that
    could otherwise handle them, it is good practice to explicitly
    ignore events to make widgets as reusable as possible.

    The default implementation closes popup widgets if the user
    presses <b>Esc</b>. Otherwise the event is ignored.

    \sa keyReleaseEvent(), TQKeyEvent::ignore(), setFocusPolicy(),
    focusInEvent(), focusOutEvent(), event(), TQKeyEvent
*/

void TQWidget::keyPressEvent( TQKeyEvent *e )
{
    if ( isPopup() && e->key() == Key_Escape ) {
	e->accept();
	close();
    } else {
	e->ignore();
    }
}

/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive key release events for the widget.

    A widget must \link setFocusPolicy() accept focus\endlink
    initially and \link hasFocus() have focus\endlink in order to
    receive a key release event.

    If you reimplement this handler, it is very important that you
    \link TQKeyEvent ignore()\endlink the release if you do not
    understand it, so that the widget's parent can interpret it.

    The default implementation ignores the event.

    \sa keyPressEvent(), TQKeyEvent::ignore(), setFocusPolicy(),
    focusInEvent(), focusOutEvent(), event(), TQKeyEvent
*/

void TQWidget::keyReleaseEvent( TQKeyEvent *e )
{
    e->ignore();
}

/*!
    This event handler can be reimplemented in a subclass to receive
    keyboard focus events (focus received) for the widget.

    A widget normally must setFocusPolicy() to something other than
    \c NoFocus in order to receive focus events. (Note that the
    application programmer can call setFocus() on any widget, even
    those that do not normally accept focus.)

    The default implementation updates the widget (except for toplevel
    widgets that do not specify a focusPolicy() ). It also calls
    setMicroFocusHint(), hinting any system-specific input tools about
    the focus of the user's attention.

    \sa focusOutEvent(), setFocusPolicy(), keyPressEvent(),
    keyReleaseEvent(), event(), TQFocusEvent
*/

void TQWidget::focusInEvent( TQFocusEvent * )
{
    if ( focusPolicy() != NoFocus || !isTopLevel() ) {
	update();
	if ( testWState(WState_AutoMask) )
	    updateMask();
	setMicroFocusHint(width()/2, 0, 1, height(), false);
    }
}

/*!
    This event handler can be reimplemented in a subclass to receive
    keyboard focus events (focus lost) for the widget.

    A widget normally must setFocusPolicy() to something other than
    \c NoFocus in order to receive focus events. (Note that the
    application programmer can call setFocus() on any widget, even
    those that do not normally accept focus.)

    The default implementation updates the widget (except for toplevel
    widgets that do not specify a focusPolicy() ). It also calls
    setMicroFocusHint(), hinting any system-specific input tools about
    the focus of the user's attention.

    \sa focusInEvent(), setFocusPolicy(), keyPressEvent(),
    keyReleaseEvent(), event(), TQFocusEvent
*/

void TQWidget::focusOutEvent( TQFocusEvent * )
{
    if ( focusPolicy() != NoFocus || !isTopLevel() ){
	update();
	if ( testWState(WState_AutoMask) )
	    updateMask();
    }
}

/*!
    \property TQWidget::microFocusHint
    \brief the currently set micro focus hint for this widget.

    See the documentation of setMicroFocusHint() for more information.
*/
TQRect TQWidget::microFocusHint() const
{
    if ( !extra || extra->micro_focus_hint.isEmpty() )
	return TQRect(width()/2, 0, 1, height() );
    else
	return extra->micro_focus_hint;
}

/*!
    This event handler can be reimplemented in a subclass to receive
    widget enter events.

    An event is sent to the widget when the mouse cursor enters the
    widget.

    \sa leaveEvent(), mouseMoveEvent(), event()
*/

void TQWidget::enterEvent( TQEvent * )
{
}

/*!
    This event handler can be reimplemented in a subclass to receive
    widget leave events.

    A leave event is sent to the widget when the mouse cursor leaves
    the widget.

    \sa enterEvent(), mouseMoveEvent(), event()
*/

void TQWidget::leaveEvent( TQEvent * )
{
}

/*!
    This event handler can be reimplemented in a subclass to receive
    paint events.

    A paint event is a request to repaint all or part of the widget.
    It can happen as a result of repaint() or update(), or because the
    widget was obscured and has now been uncovered, or for many other
    reasons.

    Many widgets can simply repaint their entire surface when asked
    to, but some slow widgets need to optimize by painting only the
    requested region: TQPaintEvent::region(). This speed optimization
    does not change the result, as painting is clipped to that region
    during event processing. TQListView and TQCanvas do this, for
    example.

    TQt also tries to speed up painting by merging multiple paint
    events into one. When update() is called several times or the
    window system sends several paint events, TQt merges these events
    into one event with a larger region (see TQRegion::unite()).
    repaint() does not permit this optimization, so we suggest using
    update() when possible.

    When the paint event occurs, the update region has normally been
    erased, so that you're painting on the widget's background. There
    are a couple of exceptions and TQPaintEvent::erased() tells you
    whether the widget has been erased or not.

    The background can be set using setBackgroundMode(),
    setPaletteBackgroundColor() or setBackgroundPixmap(). The
    documentation for setBackgroundMode() elaborates on the
    background; we recommend reading it.

    \sa event(), repaint(), update(), TQPainter, TQPixmap, TQPaintEvent
*/

void TQWidget::paintEvent( TQPaintEvent * )
{
}


/*!
    This event handler can be reimplemented in a subclass to receive
    widget move events. When the widget receives this event, it is
    already at the new position.

    The old position is accessible through TQMoveEvent::oldPos().

    \sa resizeEvent(), event(), move(), TQMoveEvent
*/

void TQWidget::moveEvent( TQMoveEvent * )
{
}


/*!
    This event handler can be reimplemented in a subclass to receive
    widget resize events. When resizeEvent() is called, the widget
    already has its new geometry. The old size is accessible through
    TQResizeEvent::oldSize().

    The widget will be erased and receive a paint event immediately
    after processing the resize event. No drawing need be (or should
    be) done inside this handler.

    Widgets that have been created with the \c WNoAutoErase flag
    will not be erased. Nevertheless, they will receive a paint event
    for their entire area afterwards. Again, no drawing needs to be
    done inside this handler.

    The default implementation calls updateMask() if the widget has
    \link TQWidget::setAutoMask() automatic masking\endlink enabled.

    \sa moveEvent(), event(), resize(), TQResizeEvent, paintEvent()
*/

void TQWidget::resizeEvent( TQResizeEvent * )
{
    if ( testWState(WState_AutoMask) )
	updateMask();
}

/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive widget close events.

    The default implementation calls e->accept(), which hides this
    widget. See the \l TQCloseEvent documentation for more details.

    \sa event(), hide(), close(), TQCloseEvent
*/

void TQWidget::closeEvent( TQCloseEvent *e )
{
    e->accept();
}


/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive widget context menu events.

    The default implementation calls e->ignore(), which rejects the
    context event. See the \l TQContextMenuEvent documentation for
    more details.

    \sa event(), TQContextMenuEvent
*/

void TQWidget::contextMenuEvent( TQContextMenuEvent *e )
{
    e->ignore();
}


/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive Input Method composition events. This handler
    is called when the user begins entering text using an Input Method.

    The default implementation calls e->ignore(), which rejects the
    Input Method event. See the \l TQIMEvent documentation for more
    details.

    \sa event(), TQIMEvent
*/
void TQWidget::imStartEvent( TQIMEvent *e )
{
    e->ignore();
}

/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive Input Method composition events. This handler
    is called when the user has entered some text using an Input Method.

    The default implementation calls e->ignore(), which rejects the
    Input Method event. See the \l TQIMEvent documentation for more
    details.

    \sa event(), TQIMEvent
*/
void TQWidget::imComposeEvent( TQIMEvent *e )
{
    e->ignore();
}


/*!
    This event handler, for event \a e, can be reimplemented in a
    subclass to receive Input Method composition events. This handler
    is called when the user has finished inputting text via an Input
    Method.

    The default implementation calls e->ignore(), which rejects the
    Input Method event. See the \l TQIMEvent documentation for more
    details.

    \sa event(), TQIMEvent
*/
void TQWidget::imEndEvent( TQIMEvent *e )
{
    e->ignore();
}


#ifndef TQT_NO_DRAGANDDROP

/*!
    This event handler is called when a drag is in progress and the
    mouse enters this widget.

    See the \link dnd.html Drag-and-drop documentation\endlink for an
    overview of how to provide drag-and-drop in your application.

    \sa TQTextDrag, TQImageDrag, TQDragEnterEvent
*/
void TQWidget::dragEnterEvent( TQDragEnterEvent * )
{
}

/*!
    This event handler is called when a drag is in progress and the
    mouse enters this widget, and whenever it moves within the widget.

    See the \link dnd.html Drag-and-drop documentation\endlink for an
    overview of how to provide drag-and-drop in your application.

    \sa TQTextDrag, TQImageDrag, TQDragMoveEvent
*/
void TQWidget::dragMoveEvent( TQDragMoveEvent * )
{
}

/*!
    This event handler is called when a drag is in progress and the
    mouse leaves this widget.

    See the \link dnd.html Drag-and-drop documentation\endlink for an
    overview of how to provide drag-and-drop in your application.

    \sa TQTextDrag, TQImageDrag, TQDragLeaveEvent
*/
void TQWidget::dragLeaveEvent( TQDragLeaveEvent * )
{
}

/*!
    This event handler is called when the drag is dropped on this
    widget.

    See the \link dnd.html Drag-and-drop documentation\endlink for an
    overview of how to provide drag-and-drop in your application.

    \sa TQTextDrag, TQImageDrag, TQDropEvent
*/
void TQWidget::dropEvent( TQDropEvent * )
{
}

#endif // TQT_NO_DRAGANDDROP

/*!
    This event handler can be reimplemented in a subclass to receive
    widget show events.

    Non-spontaneous show events are sent to widgets immediately before
    they are shown. The spontaneous show events of top-level widgets
    are delivered afterwards.

    \sa event(), TQShowEvent
*/
void TQWidget::showEvent( TQShowEvent * )
{
}

/*!
    This event handler can be reimplemented in a subclass to receive
    widget hide events.

    Hide events are sent to widgets immediately after they have been
    hidden.

    \sa event(), TQHideEvent
*/
void TQWidget::hideEvent( TQHideEvent * )
{
}

/*
    \fn TQWidget::x11Event( MSG * )

    This special event handler can be reimplemented in a subclass to
    receive native X11 events.

    In your reimplementation of this function, if you want to stop the
    event being handled by TQt, return true. If you return false, this
    native event is passed back to TQt, which translates the event into
    a TQt event and sends it to the widget.

    \warning This function is not portable.

    \sa TQApplication::x11EventFilter()
*/


#if defined(TQ_WS_MAC)

/*!
    This special event handler can be reimplemented in a subclass to
    receive native Macintosh events.

    In your reimplementation of this function, if you want to stop the
    event being handled by TQt, return true. If you return false, this
    native event is passed back to TQt, which translates the event into
    a TQt event and sends it to the widget.

    \warning This function is not portable.

    \sa TQApplication::macEventFilter()
*/

bool TQWidget::macEvent( MSG * )
{
    return false;
}

#endif
#if defined(TQ_WS_WIN)

/*!
    This special event handler can be reimplemented in a subclass to
    receive native Windows events.

    In your reimplementation of this function, if you want to stop the
    event being handled by TQt, return true. If you return false, this
    native event is passed back to TQt, which translates the event into
    a TQt event and sends it to the widget.

    \warning This function is not portable.

    \sa TQApplication::winEventFilter()
*/
bool TQWidget::winEvent( MSG * )
{
    return false;
}

#endif
#if defined(TQ_WS_X11)

/*!
    This special event handler can be reimplemented in a subclass to
    receive native X11 events.

    In your reimplementation of this function, if you want to stop the
    event being handled by TQt, return true. If you return false, this
    native event is passed back to TQt, which translates the event into
    a TQt event and sends it to the widget.

    \warning This function is not portable.

    \sa TQApplication::x11EventFilter()
*/
bool TQWidget::x11Event( XEvent * )
{
    return false;
}

#endif
#if defined(TQ_WS_QWS)

/*!
    This special event handler can be reimplemented in a subclass to
    receive native TQt/Embedded events.

    In your reimplementation of this function, if you want to stop the
    event being handled by TQt, return true. If you return false, this
    native event is passed back to TQt, which translates the event into
    a TQt event and sends it to the widget.

    \warning This function is not portable.

    \sa TQApplication::qwsEventFilter()
*/
bool TQWidget::qwsEvent( TQWSEvent * )
{
    return false;
}

#endif

/*!
    \property TQWidget::autoMask
    \brief whether the auto mask feature is enabled for the widget

    Transparent widgets use a mask to define their visible region.
    TQWidget has some built-in support to make the task of
    recalculating the mask easier. When setting auto mask to true,
    updateMask() will be called whenever the widget is resized or
    changes its focus state. Note that you must reimplement
    updateMask() (which should include a call to setMask()) or nothing
    will happen.

    Note: when you re-implement resizeEvent(), focusInEvent() or
    focusOutEvent() in your custom widgets and still want to ensure
    that the auto mask calculation works, you should add:

    \code
	if ( autoMask() )
	    updateMask();
    \endcode

    at the end of your event handlers. This is true for all member
    functions that change the appearance of the widget in a way that
    requires a recalculation of the mask.

    While being a technically appealing concept, masks have a big
    drawback: when using complex masks that cannot be expressed easily
    with relatively simple regions, they can be very slow on some
    window systems. The classic example is a transparent label. The
    complex shape of its contents makes it necessary to represent its
    mask by a bitmap, which consumes both memory and time. If all you
    want is to blend the background of several neighboring widgets
    together seamlessly, you will probably want to use
    setBackgroundOrigin() rather than a mask.

    \sa autoMask() updateMask() setMask() clearMask() setBackgroundOrigin()
*/

bool TQWidget::autoMask() const
{
    return testWState(WState_AutoMask);
}

void TQWidget::setAutoMask( bool enable )
{
    if ( enable == autoMask() )
	return;

    if ( enable ) {
	setWState(WState_AutoMask);
	updateMask();
    } else {
	clearWState(WState_AutoMask);
	clearMask();
    }
}

/*!
    \enum TQWidget::BackgroundOrigin

    This enum defines the origin used to draw a widget's background
    pixmap.

    The pixmap is drawn using the:
    \value WidgetOrigin  widget's coordinate system.
    \value ParentOrigin  parent's coordinate system.
    \value WindowOrigin  top-level window's coordinate system.
    \value AncestorOrigin  same origin as the parent uses.
*/

/*!
    \property TQWidget::backgroundOrigin
    \brief the origin of the widget's background

    The origin is either WidgetOrigin (the default), ParentOrigin,
    WindowOrigin or AncestorOrigin.

    This only makes a difference if the widget has a background
    pixmap, in which case positioning matters. Using \c WindowOrigin
    for several neighboring widgets makes the background blend
    together seamlessly. \c AncestorOrigin allows blending backgrounds
    seamlessly when an ancestor of the widget has an origin other than
    \c WindowOrigin.

    \sa backgroundPixmap(), setBackgroundMode()
*/
TQWidget::BackgroundOrigin TQWidget::backgroundOrigin() const
{
    return extra ? (BackgroundOrigin)extra->bg_origin : WidgetOrigin;
}

void TQWidget::setBackgroundOrigin( BackgroundOrigin origin )
{
    if ( origin == backgroundOrigin() )
	return;
    createExtra();
    extra->bg_origin = origin;
    update();
}

/*!
    This function can be reimplemented in a subclass to support
    transparent widgets. It should be called whenever a widget changes
    state in a way that means that the shape mask must be recalculated.

    \sa setAutoMask(), updateMask(), setMask(), clearMask()
*/
void TQWidget::updateMask()
{
}

/*!
  \internal
  Returns the offset of the widget from the backgroundOrigin.

  \sa setBackgroundMode(), backgroundMode(),
*/
TQPoint TQWidget::backgroundOffset() const
{
    if (!isTopLevel()) {
	switch(backgroundOrigin()) {
	    case WidgetOrigin:
		break;
	    case ParentOrigin:
		return pos();
	    case WindowOrigin:
		{
		    const TQWidget *topl = this;
		    while(topl && !topl->isTopLevel() && !topl->testWFlags(TQt::WSubWindow))
			topl = topl->parentWidget(true);
		    return mapTo((TQWidget *)topl, TQPoint(0, 0) );
		}
	    case AncestorOrigin:
		{
		    const TQWidget *topl = this;
		    bool ancestorIsWindowOrigin = false;
		    while(topl && !topl->isTopLevel() && !topl->testWFlags(TQt::WSubWindow))
		    {
			if (!ancestorIsWindowOrigin) {
			    if (topl->backgroundOrigin() == TQWidget::WidgetOrigin)
				break;
			    if (topl->backgroundOrigin() == TQWidget::ParentOrigin)
			    {
				topl = topl->parentWidget(true);
				break;
			    }
			    if (topl->backgroundOrigin() == TQWidget::WindowOrigin)
				ancestorIsWindowOrigin = true;
			}
			topl = topl->parentWidget(true);
		    }

		    return mapTo((TQWidget *) topl, TQPoint(0,0) );
		}
	}
    }
    // fall back
    return TQPoint(0,0);
}

/*!
    \fn TQLayout* TQWidget::layout () const

    Returns the layout engine that manages the geometry of this
    widget's children.

    If the widget does not have a layout, layout() returns 0.

    \sa  sizePolicy()
*/


/*  Sets this widget to use layout \a l to manage the geometry of its
  children.

  If the widget already had a layout, the old layout is
  forgotten. (Note that it is not deleted.)

  \sa layout() TQLayout sizePolicy()
*/
#ifndef TQT_NO_LAYOUT
void TQWidget::setLayout( TQLayout *l )
{
    lay_out = l;
}
#endif

/*!
    \property TQWidget::sizePolicy
    \brief the default layout behavior of the widget

    If there is a TQLayout that manages this widget's children, the
    size policy specified by that layout is used. If there is no such
    TQLayout, the result of this function is used.

    The default policy is Preferred/Preferred, which means that the
    widget can be freely resized, but prefers to be the size
    sizeHint() returns. Button-like widgets set the size policy to
    specify that they may stretch horizontally, but are fixed
    vertically. The same applies to lineedit controls (such as
    TQLineEdit, TQSpinBox or an editable TQComboBox) and other
    horizontally orientated widgets (such as TQProgressBar).
    TQToolButton's are normally square, so they allow growth in both
    directions. Widgets that support different directions (such as
    TQSlider, TQScrollBar or TQHeader) specify stretching in the
    respective direction only. Widgets that can provide scrollbars
    (usually subclasses of TQScrollView) tend to specify that they can
    use additional space, and that they can make do with less than
    sizeHint().

    \sa sizeHint() TQLayout TQSizePolicy updateGeometry()
*/
TQSizePolicy TQWidget::sizePolicy() const
{
    return extra ? extra->size_policy
	: TQSizePolicy( TQSizePolicy::Preferred, TQSizePolicy::Preferred );
}

void TQWidget::setSizePolicy( TQSizePolicy policy )
{
    setWState( WState_OwnSizePolicy );
    if ( policy == sizePolicy() )
	return;
    createExtra();
    extra->size_policy = policy;
    updateGeometry();
}

/*!
    \overload void TQWidget::setSizePolicy( TQSizePolicy::SizeType hor, TQSizePolicy::SizeType ver, bool hfw )

    Sets the size policy of the widget to \a hor, \a ver and \a hfw
    (height for width).

    \sa TQSizePolicy::TQSizePolicy()
*/

/*!
    Returns the preferred height for this widget, given the width \a
    w. The default implementation returns 0, indicating that the
    preferred height does not depend on the width.

    \warning Does not look at the widget's layout.
*/

int TQWidget::heightForWidth( int w ) const
{
    (void)w;
    return 0;
}

/*!
    \property TQWidget::customWhatsThis
    \brief whether the widget wants to handle What's This help manually

    The default implementation of customWhatsThis() returns false,
    which means the widget will not receive any events in Whats This
    mode.

    The widget may leave What's This mode by calling
    TQWhatsThis::leaveWhatsThisMode(), with or without actually
    displaying any help text.

    You can also reimplement customWhatsThis() if your widget is a
    "passive interactor" supposed to work under all circumstances.
    Simply don't call TQWhatsThis::leaveWhatsThisMode() in that case.

    \sa TQWhatsThis::inWhatsThisMode() TQWhatsThis::leaveWhatsThisMode()
*/
bool TQWidget::customWhatsThis() const
{
    return false;
}

/*!
    Returns the visible child widget at pixel position \a (x, y) in
    the widget's own coordinate system.

    If \a includeThis is true, and there is no child visible at \a (x,
    y), the widget itself is returned.
*/
TQWidget  *TQWidget::childAt( int x, int y, bool includeThis ) const
{
    if ( !rect().contains( x, y ) )
	return 0;
    if ( children() ) {
	TQObjectListIt it( *children() );
	it.toLast();
	TQWidget *w, *t;
	while( (w=(TQWidget *)it.current()) != 0 ) {
	    --it;
	    if ( w->isWidgetType() && !w->isTopLevel() && !w->isHidden() ) {
		if ( ( t = w->childAt( x - w->x(), y - w->y(), true ) ) )
		    return t;
	    }
	}
    }
    if ( includeThis )
	return (TQWidget*)this;
    return 0;
}

/*!
    \overload

    Returns the visible child widget at point \a p in the widget's own
    coordinate system.

    If \a includeThis is true, and there is no child visible at \a p,
    the widget itself is returned.

*/
TQWidget  *TQWidget::childAt( const TQPoint & p, bool includeThis ) const
{
    return childAt( p.x(), p.y(), includeThis );
}


/*!
    Notifies the layout system that this widget has changed and may
    need to change geometry.

    Call this function if the sizeHint() or sizePolicy() have changed.

    For explicitly hidden widgets, updateGeometry() is a no-op. The
    layout system will be notified as soon as the widget is shown.
*/

void TQWidget::updateGeometry()
{
  TQWidget *parent = parentWidget();
  if (parent && !isTopLevel() && isShown())
	  TQApplication::postEvent(parent, new TQEvent(TQEvent::LayoutHint));
}


/*!
    Reparents the widget. The widget gets a new \a parent, new widget
    flags (\a f, but as usual, use 0) at a new position in its new
    parent (\a p).

    If \a showIt is true, show() is called once the widget has been
    reparented.

    If the new parent widget is in a different top-level widget, the
    reparented widget and its children are appended to the end of the
    \link setFocusPolicy() tab chain \endlink of the new parent
    widget, in the same internal order as before. If one of the moved
    widgets had keyboard focus, reparent() calls clearFocus() for that
    widget.

    If the new parent widget is in the same top-level widget as the
    old parent, reparent doesn't change the tab order or keyboard
    focus.

    \warning It is extremely unlikely that you will ever need this
    function. If you have a widget that changes its content
    dynamically, it is far easier to use \l TQWidgetStack or \l
    TQWizard.

    \sa getWFlags()
*/

void TQWidget::reparent( TQWidget *parent, WFlags f, const TQPoint &p,
			bool showIt )
{
    reparentSys( parent, f, p, showIt );
    TQEvent e( TQEvent::Reparent );
    TQApplication::sendEvent( this, &e );
    if (!own_font)
	unsetFont();
    else
	setFont( fnt.resolve( tqt_naturalWidgetFont( this ) ) );
#ifndef TQT_NO_PALETTE
    if (!own_palette)
	unsetPalette();
#endif
}

/*!
    \overload

    A convenience version of reparent that does not take widget flags
    as argument.

    Calls reparent(\a parent, getWFlags() \& ~\l WType_Mask, \a p, \a
    showIt).
*/
void  TQWidget::reparent( TQWidget *parent, const TQPoint & p,
			 bool showIt )
{
    reparent( parent, getWFlags() & ~WType_Mask, p, showIt );
}

/*!
    \property TQWidget::ownCursor
    \brief whether the widget uses its own cursor

    If false, the widget uses its parent widget's cursor.

    \sa cursor
*/

/*!
    \property TQWidget::ownFont
    \brief whether the widget uses its own font

    If false, the widget uses its parent widget's font.

    \sa font
*/

/*!
    \property TQWidget::ownPalette
    \brief whether the widget uses its own palette

    If false, the widget uses its parent widget's palette.

    \sa palette
*/


void TQWidget::repaint( bool erase )
{
    repaint( visibleRect(), erase );
}




/*!\obsolete  Use paletteBackgroundColor() or eraseColor() instead. */
const TQColor & TQWidget::backgroundColor() const { return eraseColor(); }
/*!\obsolete  Use setPaletteBackgroundColor() or setEraseColor() instead. */
void TQWidget::setBackgroundColor( const TQColor &c ) { setEraseColor( c ); }
/*!\obsolete  Use paletteBackgroundPixmap()  or erasePixmap() instead. */
const TQPixmap *TQWidget::backgroundPixmap() const { return erasePixmap(); }
/*!\obsolete  Use setPaletteBackgroundPixmap() or setErasePixmap() instead. */
void TQWidget::setBackgroundPixmap( const TQPixmap &pm ) { setErasePixmap( pm ); }


// documentation in qdesktopwidget_win.cpp
void TQDesktopWidget::insertChild( TQObject *obj )
{
    if ( obj->isWidgetType() )
	return;
    TQWidget::insertChild( obj );
}

/*!
  \property TQWidget::windowOpacity

  \brief The level of opacity for the window.

  The valid range of opacity is from 1.0 (completely opaque) to
  0.0 (completely transparent).

  By default the value of this property is 1.0.

  This feature is only present on Mac OS X and Windows 2000 and up.

  \warning Changing this property from opaque to transparent might issue a
  paint event that needs to be processed before the window is displayed
  correctly. This affects mainly the use of TQPixmap::grabWindow(). Also note
  that semi-transparent windows update and resize significantely slower than
  opaque windows.
*/
