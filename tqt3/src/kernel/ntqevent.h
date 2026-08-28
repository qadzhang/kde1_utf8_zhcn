/****************************************************************************
**
** Definition of event classes
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

#ifndef TQEVENT_H
#define TQEVENT_H

#ifndef QT_H
#include "ntqwindowdefs.h"
#include "ntqregion.h"
#include "ntqnamespace.h"
#include "ntqmime.h"
#include "ntqpair.h"
#endif // QT_H

class TQ_EXPORT TQEvent: public TQt		// event base class
{
public:
    enum Type {

	/*
	  If you get a strange compiler error on the line with None,
	  it's probably because you're also including X11 headers,
	  which #define the symbol None. Put the X11 includes after
	  the TQt includes to solve this problem.
	*/

	None = 0,				// invalid event


	Timer = 1,				// timer event
	MouseButtonPress = 2,			// mouse button pressed
	MouseButtonRelease = 3,			// mouse button released
	MouseButtonDblClick = 4,		// mouse button double click
	MouseMove = 5,				// mouse move
	KeyPress = 6,				// key pressed
	KeyRelease = 7,				// key released
	FocusIn = 8,				// keyboard focus received
	FocusOut = 9,				// keyboard focus lost
	Enter = 10,				// mouse enters widget
	Leave = 11,				// mouse leaves widget
	Paint = 12,				// paint widget
	Move = 13,				// move widget
	Resize = 14,				// resize widget
	Create = 15,				// after object creation
	Destroy = 16,				// during object destruction
	Show = 17,				// widget is shown
	Hide = 18,				// widget is hidden
	Close = 19,				// request to close widget
	Quit = 20,				// request to quit application
	Reparent = 21,				// widget has been reparented
	ShowMinimized = 22,		       	// widget is shown minimized
	ShowNormal = 23,	       		// widget is shown normal
	WindowActivate = 24,	       		// window was activated
	WindowDeactivate = 25,	       		// window was deactivated
	ShowToParent = 26,	       		// widget is shown to parent
	HideToParent = 27,	       		// widget is hidden to parent
	ShowMaximized = 28,		       	// widget is shown maximized
	ShowFullScreen = 29,			// widget is shown full-screen
	Accel = 30,				// accelerator event
	Wheel = 31,				// wheel event
	AccelAvailable = 32,			// accelerator available event
	CaptionChange = 33,			// caption changed
	IconChange = 34,			// icon changed
	ParentFontChange = 35,			// parent font changed
	ApplicationFontChange = 36,		// application font changed
	ParentPaletteChange = 37,		// parent palette changed
	ApplicationPaletteChange = 38,		// application palette changed
	PaletteChange = 39,			// widget palette changed
	Clipboard = 40,				// internal clipboard event
	Speech = 42,				// reserved for speech input
	SockAct = 50,				// socket activation
	AccelOverride = 51,			// accelerator override event
	DeferredDelete = 52,			// deferred delete event
	DragEnter = 60,				// drag moves into widget
	DragMove = 61,				// drag moves in widget
	DragLeave = 62,				// drag leaves or is cancelled
	Drop = 63,				// actual drop
	DragResponse = 64,			// drag accepted/rejected
	ChildInserted = 70,			// new child widget
	ChildRemoved = 71,			// deleted child widget
	LayoutHint = 72,			// child min/max size changed
	ShowWindowRequest = 73,			// widget's window should be mapped
	WindowBlocked = 74,			// window is about to be blocked modally
	WindowUnblocked = 75,			// windows modal blocking has ended
	ActivateControl = 80,			// ActiveX activation
	DeactivateControl = 81,			// ActiveX deactivation
	ContextMenu = 82,			// context popup menu
	IMStart = 83,				// input method composition start
	IMCompose = 84,				// input method composition
	IMEnd = 85,				// input method composition end
	Accessibility = 86,			// accessibility information is requested
	TabletMove = 87,			// Wacom tablet event
	LocaleChange = 88,			// the system locale changed
	LanguageChange = 89,			// the application language changed
	LayoutDirectionChange = 90,		// the layout direction changed
	Style = 91,				// internal style event
	TabletPress = 92,			// tablet press
	TabletRelease = 93,			// tablet release
	OkRequest = 94,				// CE (Ok) button pressed
	HelpRequest = 95,			// CE (?)  button pressed
	WindowStateChange = 96,			// window state has changed
	IconDrag = 97,                          // proxy icon dragged
	MetaCall = 98,				// meta method call (internal)
	ThreadChange = 99,			// thread changed
	User = 1000,				// first user event id
	MaxUser = 65535				// last user event id
    };


    TQEvent( Type type ) : t(type), posted(false), spont(false) {}
    virtual ~TQEvent();
    Type  type() const	{ return t; }
    bool spontaneous() const 	{ return spont; }
protected:
    Type  t;
private:
    uint posted : 1;
    uint spont : 1;


    friend class TQApplication;
    friend class TQAccelManager;
    friend class TQBaseApplication;
    friend class TQETWidget;
};


class TQ_EXPORT TQTimerEvent : public TQEvent
{
public:
    TQTimerEvent( int timerId )
	: TQEvent(Timer), id(timerId) {}
    int	  timerId()	const	{ return id; }
protected:
    int	  id;
};


class TQ_EXPORT TQMouseEvent : public TQEvent
{
public:
    TQMouseEvent( Type type, const TQPoint &pos, int button, int state );

    TQMouseEvent( Type type, const TQPoint &pos, const TQPoint&globalPos,
		 int button, int state )
	: TQEvent(type), p(pos), g(globalPos), b((ushort)button),s((ushort)state),accpt(true) {};

    const TQPoint &pos() const	{ return p; }
    const TQPoint &globalPos() const { return g; }
    int	   x()		const	{ return p.x(); }
    int	   y()		const	{ return p.y(); }
    int	   globalX()	const	{ return g.x(); }
    int	   globalY()	const	{ return g.y(); }
    ButtonState button() const	{ return (ButtonState) b; }
    ButtonState state()	const	{ return (ButtonState) s; }
    ButtonState stateAfter() const;
    bool   isAccepted() const	{ return accpt; }
    void   accept()		{ accpt = true; }
    void   ignore()		{ accpt = false; }
protected:
    TQPoint p;
    TQPoint g;
    ushort b;
    ushort s;
    uint   accpt:1;
};


#ifndef TQT_NO_WHEELEVENT
class TQ_EXPORT TQWheelEvent : public TQEvent
{
public:
    TQWheelEvent( const TQPoint &pos, int delta, int state, Orientation orient = Vertical );
    TQWheelEvent( const TQPoint &pos, const TQPoint& globalPos, int delta, int state, Orientation orient = Vertical  )
	: TQEvent(Wheel), p(pos), g(globalPos), d(delta), s((ushort)state),
	  accpt(true), o(orient) {}
    int	   delta()	const	{ return d; }
    const TQPoint &pos() const	{ return p; }
    const TQPoint &globalPos() const	{ return g; }
    int	   x()		const	{ return p.x(); }
    int	   y()		const	{ return p.y(); }
    int	   globalX()	const	{ return g.x(); }
    int	   globalY()	const	{ return g.y(); }
    ButtonState state()	const	{ return ButtonState(s); }
    Orientation orientation() 	const 	{ return o; }
    bool   isAccepted() const	{ return accpt; }
    void   accept()		{ accpt = true; }
    void   ignore()		{ accpt = false; }
protected:
    TQPoint p;
    TQPoint g;
    int d;
    ushort s;
    bool   accpt;
    Orientation o;
};
#endif

class TQ_EXPORT TQTabletEvent : public TQEvent
{
public:
    enum TabletDevice { NoDevice = -1, Puck, Stylus, Eraser };
    TQTabletEvent( Type t, const TQPoint &pos, const TQPoint &globalPos, int device,
		  int pressure, int xTilt, int yTilt, const TQPair<int,int> &uId );
    TQTabletEvent( const TQPoint &pos, const TQPoint &globalPos, int device,
		  int pressure, int xTilt, int yTilt, const TQPair<int,int> &uId )
	: TQEvent( TabletMove ), mPos( pos ), mGPos( globalPos ), mDev( device ),
	  mPress( pressure ), mXT( xTilt ), mYT( yTilt ), mType( uId.first ),
	  mPhy( uId.second ), mbAcc(true)
    {}
    int pressure()	const { return mPress; }
    int xTilt()		const { return mXT; }
    int yTilt()		const { return mYT; }
    const TQPoint &pos()	const { return mPos; }
    const TQPoint &globalPos() const { return mGPos; }
    int x()		const { return mPos.x(); }
    int y()		const { return mPos.y(); }
    int globalX()	const { return mGPos.x(); }
    int globalY()	const { return mGPos.y(); }
    TabletDevice device() 	const { return TabletDevice(mDev); }
    int isAccepted() const { return mbAcc; }
    void accept() { mbAcc = true; }
    void ignore() { mbAcc = false; }
    TQPair<int,int> uniqueId() { return TQPair<int,int>( mType, mPhy); }
protected:
    TQPoint mPos;
    TQPoint mGPos;
    int mDev,
	mPress,
	mXT,
	mYT,
	mType,
	mPhy;
    bool mbAcc;

};

class TQ_EXPORT TQKeyEvent : public TQEvent
{
public:
    TQKeyEvent( Type type, int key, int ascii, int state,
		const TQString& text=TQString::null, bool autorep=false, ushort count=1 )
	: TQEvent(type), txt(text), k((ushort)key), s((ushort)state),
	    a((uchar)ascii), accpt(true), autor(autorep), c(count)
    {
	if ( key >= Key_Back && key <= Key_MediaLast )
	    accpt = false;
    }
    int	   key()	const	{ return k; }
    int	   ascii()	const	{ return a; }
    ButtonState state()	const	{ return ButtonState(s); }
    ButtonState stateAfter() const;
    bool   isAccepted() const	{ return accpt; }
    TQString text()      const   { return txt; }
    bool   isAutoRepeat() const	{ return autor; }
    int   count() const	{ return int(c); }
    void   accept()		{ accpt = true; }
    void   ignore()		{ accpt = false; }

protected:
    TQString txt;
    ushort k, s;
    uchar  a;
    uint   accpt:1;
    uint   autor:1;
    ushort c;
};


class TQ_EXPORT TQFocusEvent : public TQEvent
{
public:

    TQFocusEvent( Type type )
	: TQEvent(type) {}

    bool   gotFocus()	const { return type() == FocusIn; }
    bool   lostFocus()	const { return type() == FocusOut; }

    enum Reason { Mouse, Tab, Backtab, ActiveWindow, Popup, Shortcut, Other };
    static Reason reason();
    static void setReason( Reason reason );
    static void resetReason();

private:
    static Reason m_reason;
    static Reason prev_reason;
};


class TQ_EXPORT TQPaintEvent : public TQEvent
{
public:
    TQPaintEvent( const TQRegion& paintRegion, bool erased = true)
	: TQEvent(Paint),
	  rec(paintRegion.boundingRect()),
	  reg(paintRegion),
	  erase(erased){}
    TQPaintEvent( const TQRect &paintRect, bool erased = true )
	: TQEvent(Paint),
	  rec(paintRect),
	  reg(paintRect),
	  erase(erased){}
    TQPaintEvent( const TQRegion &paintRegion, const TQRect &paintRect, bool erased = true )
	: TQEvent(Paint),
	  rec(paintRect),
	  reg(paintRegion),
	  erase(erased){}

    const TQRect &rect() const	  { return rec; }
    const TQRegion &region() const { return reg; }
    bool erased() const { return erase; }
protected:
    friend class TQApplication;
    friend class TQBaseApplication;
    TQRect rec;
    TQRegion reg;
    bool erase;
};


class TQ_EXPORT TQMoveEvent : public TQEvent
{
public:
    TQMoveEvent( const TQPoint &pos, const TQPoint &oldPos )
	: TQEvent(Move), p(pos), oldp(oldPos) {}
    const TQPoint &pos()	  const { return p; }
    const TQPoint &oldPos()const { return oldp;}
protected:
    TQPoint p, oldp;
    friend class TQApplication;
    friend class TQBaseApplication;
};


class TQ_EXPORT TQResizeEvent : public TQEvent
{
public:
    TQResizeEvent( const TQSize &size, const TQSize &oldSize )
	: TQEvent(Resize), s(size), olds(oldSize) {}
    const TQSize &size()	  const { return s; }
    const TQSize &oldSize()const { return olds;}
protected:
    TQSize s, olds;
    friend class TQApplication;
    friend class TQBaseApplication;
};


class TQ_EXPORT TQCloseEvent : public TQEvent
{
public:
    TQCloseEvent()
	: TQEvent(Close), accpt(false) {}
    bool   isAccepted() const	{ return accpt; }
    void   accept()		{ accpt = true; }
    void   ignore()		{ accpt = false; }
protected:
    bool   accpt;
};


class TQ_EXPORT TQIconDragEvent : public TQEvent
{
public:
    TQIconDragEvent() 
	: TQEvent(IconDrag), accpt(false) {}

    bool   isAccepted() const	{ return accpt; }
    void   accept()		{ accpt = true; }
    void   ignore()		{ accpt = false; }
protected:
    bool   accpt;
};

class TQ_EXPORT TQShowEvent : public TQEvent
{
public:
    TQShowEvent()
	: TQEvent(Show) {}
};


class TQ_EXPORT TQHideEvent : public TQEvent
{
public:
    TQHideEvent()
	: TQEvent(Hide) {}
};

class TQ_EXPORT TQContextMenuEvent : public TQEvent
{
public:
    enum Reason { Mouse, Keyboard, Other };
    TQContextMenuEvent( Reason reason, const TQPoint &pos, const TQPoint &globalPos, int state )
	: TQEvent( ContextMenu ), p( pos ), gp( globalPos ), accpt( true ), consum( true ),
	reas( reason ), s((ushort)state) {}
    TQContextMenuEvent( Reason reason, const TQPoint &pos, int state );

    int	    x() const { return p.x(); }
    int	    y() const { return p.y(); }
    int	    globalX() const { return gp.x(); }
    int	    globalY() const { return gp.y(); }

    const TQPoint&   pos() const { return p; }
    const TQPoint&   globalPos() const { return gp; }

    ButtonState state()	const	{ return (ButtonState) s; }
    bool    isAccepted() const	{ return accpt; }
    bool    isConsumed() const	{ return consum; }
    void    consume()		{ accpt = false; consum = true; }
    void    accept()		{ accpt = true; consum = true; }
    void    ignore()		{ accpt = false; consum = false; }

    Reason  reason() const { return Reason( reas ); }

protected:
    TQPoint  p;
    TQPoint  gp;
    bool    accpt;
    bool    consum;
    uint    reas:8;
    ushort s;
};


class TQ_EXPORT TQIMEvent : public TQEvent
{
public:
    TQIMEvent( Type type, const TQString &text, int cursorPosition )
	: TQEvent(type), txt(text), cpos(cursorPosition), a(true) {}
    const TQString &text() const { return txt; }
    int cursorPos() const { return cpos; }
    bool isAccepted() const { return a; }
    void accept() { a = true; }
    void ignore() { a = false; }
    int selectionLength() const;

private:
    TQString txt;
    int cpos;
    bool a;
};

class TQ_EXPORT TQIMComposeEvent : public TQIMEvent
{
public:
    TQIMComposeEvent( Type type, const TQString &text, int cursorPosition,
		     int selLength )
	: TQIMEvent( type, text, cursorPosition ), selLen( selLength ) { }

private:
    int selLen;

    friend class TQIMEvent;
};

inline int TQIMEvent::selectionLength() const
{
    if ( type() != IMCompose ) return 0;
    TQIMComposeEvent *that = (TQIMComposeEvent *) this;
    return that->selLen;
}


#ifndef TQT_NO_DRAGANDDROP

// This class is rather closed at the moment.  If you need to create your
// own DND event objects, write to qt-bugs@trolltech.com and we'll try to
// find a way to extend it so it covers your needs.

class TQ_EXPORT TQDropEvent : public TQEvent, public TQMimeSource
{
public:
    TQDropEvent( const TQPoint& pos, Type typ=Drop )
	: TQEvent(typ), p(pos),
	  act(0), accpt(0), accptact(0), resv(0),
	  d(0)
	{}
    const TQPoint &pos() const	{ return p; }
    bool isAccepted() const	{ return accpt || accptact; }
    void accept(bool y=true)	{ accpt = y; }
    void ignore()		{ accpt = false; }

    bool isActionAccepted() const { return accptact; }
    void acceptAction(bool y=true) { accptact = y; }
    enum Action { Copy, Link, Move, Private, UserAction=100 };
    void setAction( Action a ) { act = (uint)a; }
    Action action() const { return Action(act); }

    TQWidget* source() const;
    const char* format( int n = 0 ) const;
    TQByteArray encodedData( const char* ) const;
    bool provides( const char* ) const;

    TQByteArray data(const char* f) const { return encodedData(f); }

    void setPoint( const TQPoint& np ) { p = np; }

protected:
    TQPoint p;
    uint act:8;
    uint accpt:1;
    uint accptact:1;
    uint resv:5;
    void * d;
};



class TQ_EXPORT TQDragMoveEvent : public TQDropEvent
{
public:
    TQDragMoveEvent( const TQPoint& pos, Type typ=DragMove )
	: TQDropEvent(pos,typ),
	  rect( pos, TQSize( 1, 1 ) ) {}
    TQRect answerRect() const { return rect; }
    void accept( bool y=true ) { TQDropEvent::accept(y); }
    void accept( const TQRect & r) { accpt = true; rect = r; }
    void ignore( const TQRect & r) { accpt =false; rect = r; }
    void ignore()		{ TQDropEvent::ignore(); }

protected:
    TQRect rect;
};


class TQ_EXPORT TQDragEnterEvent : public TQDragMoveEvent
{
public:
    TQDragEnterEvent( const TQPoint& pos ) :
	TQDragMoveEvent(pos, DragEnter) { }
};


/* An internal class */
class TQ_EXPORT TQDragResponseEvent : public TQEvent
{
public:
    TQDragResponseEvent( bool accepted )
	: TQEvent(DragResponse), a(accepted) {}
    bool   dragAccepted() const	{ return a; }
protected:
    bool a;
};


class TQ_EXPORT TQDragLeaveEvent : public TQEvent
{
public:
    TQDragLeaveEvent()
	: TQEvent(DragLeave) {}
};

#endif // TQT_NO_DRAGANDDROP

class TQ_EXPORT TQChildEvent : public TQEvent
{
public:
    TQChildEvent( Type type, TQObject *child )
	: TQEvent(type), c(child) {}
    TQObject *child() const	{ return c; }
    bool inserted() const { return t == ChildInserted; }
    bool removed() const { return t == ChildRemoved; }
protected:
    TQObject *c;
};


class TQ_EXPORT TQCustomEvent : public TQEvent
{
public:
    TQCustomEvent( int type );
    TQCustomEvent( Type type, void *data )
	: TQEvent(type), d(data) {};
    void       *data()	const	{ return d; }
    void	setData( void* data )	{ d = data; }
private:
    void       *d;
};

#endif // TQEVENT_H
