/****************************************************************************
**
** Tool Tips (or Balloon Help) for any widget or rectangle
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
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

#include "ntqtooltip.h"
#ifndef TQT_NO_TOOLTIP
#include "ntqlabel.h"
#include "ntqptrdict.h"
#include "ntqapplication.h"
#include "ntqguardedptr.h"
#include "ntqtimer.h"
#include "qeffects_p.h"

static bool globally_enabled = true;

// Magic value meaning an entire widget - if someone tries to insert a
// tool tip on this part of a widget it will be interpreted as the
// entire widget.

static inline TQRect entireWidget()
{
    return TQRect( -TQWIDGETSIZE_MAX, -TQWIDGETSIZE_MAX,
		  2*TQWIDGETSIZE_MAX, 2*TQWIDGETSIZE_MAX );
}

// Internal class - don't touch

class TQTipLabel : public TQLabel
{
    TQ_OBJECT
public:
    TQTipLabel( TQWidget* parent, const TQString& text) : TQLabel( parent, "toolTipTip",
	     WStyle_StaysOnTop | WStyle_Customize | WStyle_NoBorder | WStyle_Tool | WX11BypassWM )
    {
	setMargin(1);
	setAutoMask( false );
	setFrameStyle( TQFrame::Plain | TQFrame::Box );
	setLineWidth( 1 );
	setAlignment( AlignAuto | AlignTop );
	setIndent(0);
	polish();
	setText(text);
	adjustSize();
        x11SetWindowType( X11WindowTypeTooltip );
    }
    void setWidth( int w ) { resize( sizeForWidth( w ) ); }
};

// Internal class - don't touch

class TQTipManager : public TQObject
{
    TQ_OBJECT
public:
    TQTipManager();
   ~TQTipManager();

    struct Tip
    {
	TQRect		rect;
	TQString		text;
	TQString	        groupText;
	TQToolTipGroup  *group;
	TQToolTip       *tip;
	bool	        autoDelete;
	TQRect 		geometry;
	Tip	       *next;
    };

    bool    eventFilter( TQObject * o, TQEvent * e );
    void    add( const TQRect &gm, TQWidget *, const TQRect &, const TQString& ,
		 TQToolTipGroup *, const TQString& , TQToolTip *, bool );
    void    add( TQWidget *, const TQRect &, const TQString& ,
		 TQToolTipGroup *, const TQString& , TQToolTip *, bool );
    void    remove( TQWidget *, const TQRect &, bool delayhide = false );
    void    remove( TQWidget * );

    void    removeFromGroup( TQToolTipGroup * );

    void    hideTipAndSleep();

    TQString find( TQWidget *, const TQPoint& );
    void setWakeUpDelay(int);

public slots:
    void    hideTip();

private slots:
    void    labelDestroyed();
    void    clientWidgetDestroyed();
    void    showTip();
    void    allowAnimation();

private:
    TQTimer  wakeUp;
    int wakeUpDelay;
    TQTimer  fallAsleep;

    TQPtrDict<Tip> *tips;
    TQTipLabel *label;
    TQPoint pos;
    TQGuardedPtr<TQWidget> widget;
    Tip *currentTip;
    Tip *previousTip;
    bool preventAnimation;
    bool isApplicationFilter;
    TQTimer *removeTimer;
};


// We have a global, internal TQTipManager object

static TQTipManager *tipManager	  = 0;

static void initTipManager()
{
    if ( !tipManager ) {
	tipManager = new TQTipManager;
	TQ_CHECK_PTR( tipManager );
    }
}


TQTipManager::TQTipManager()
    : TQObject( tqApp, "toolTipManager" )
{
    wakeUpDelay = 700;
    tips = new TQPtrDict<TQTipManager::Tip>( 313 );
    currentTip = 0;
    previousTip = 0;
    label = 0;
    preventAnimation = false;
    isApplicationFilter = false;
    connect( &wakeUp, TQ_SIGNAL(timeout()), TQ_SLOT(showTip()) );
    connect( &fallAsleep, TQ_SIGNAL(timeout()), TQ_SLOT(hideTip()) );
    removeTimer = new TQTimer( this );
}


TQTipManager::~TQTipManager()
{
    if ( isApplicationFilter && !tqApp->closingDown() ) {
	tqApp->setGlobalMouseTracking( false );
	tqApp->removeEventFilter( tipManager );
    }

    delete label;
    label = 0;

    if ( tips ) {
	TQPtrDictIterator<TQTipManager::Tip> i( *tips );
	TQTipManager::Tip *t, *n;
	void *k;
	while( (t = i.current()) != 0 ) {
	    k = i.currentKey();
	    ++i;
	    tips->take( k );
	    while ( t ) {
		n = t->next;
		delete t;
		t = n;
	    }
	}
	delete tips;
    }

    tipManager = 0;
}

void TQTipManager::add( const TQRect &gm, TQWidget *w,
		       const TQRect &r, const TQString &s,
		       TQToolTipGroup *g, const TQString& gs,
		       TQToolTip *tt, bool a )
{
    remove( w, r, true );
    TQTipManager::Tip *h = (*tips)[ w ];
    TQTipManager::Tip *t = new TQTipManager::Tip;
    t->next = h;
    t->tip = tt;
    t->autoDelete = a;
    t->text = s;
    t->rect = r;
    t->groupText = gs;
    t->group = g;
    t->geometry = gm;

    if ( h ) {
	tips->take( w );
	if ( h != currentTip && h->autoDelete ) {
	    t->next = h->next;
	    delete h;
	}
    } else
	connect( w, TQ_SIGNAL(destroyed()), this, TQ_SLOT(clientWidgetDestroyed()) );

    tips->insert( w, t );

    if ( a && t->rect.contains( pos ) && (!g || g->enabled()) ) {
	removeTimer->stop();
	showTip();
    }

    if ( !isApplicationFilter && tqApp ) {
	isApplicationFilter = true;
	tqApp->installEventFilter( tipManager );
	tqApp->setGlobalMouseTracking( true );
    }

    if ( t->group ) {
	disconnect( removeTimer, TQ_SIGNAL( timeout() ),
		 t->group, TQ_SIGNAL( removeTip() ) );
	connect( removeTimer, TQ_SIGNAL( timeout() ),
		 t->group, TQ_SIGNAL( removeTip() ) );
    }
}

void TQTipManager::add( TQWidget *w, const TQRect &r, const TQString &s,
		       TQToolTipGroup *g, const TQString& gs,
		       TQToolTip *tt, bool a )
{
    add( TQRect( -1, -1, -1, -1 ), w, r, s, g, gs, tt, a );
}


void TQTipManager::remove( TQWidget *w, const TQRect & r, bool delayhide )
{
    TQTipManager::Tip *t = (*tips)[ w ];
    if ( t == 0 )
	return;

    if ( t == currentTip ) {
	if (!delayhide)
	    hideTip();
	else
	    currentTip->autoDelete = true;
	}

    if ( t == previousTip )
	previousTip = 0;

    if ( ( currentTip != t || !delayhide ) && t->rect == r ) {
	tips->take( w );
	if ( t->next )
	    tips->insert( w, t->next );
	delete t;
    } else {
	while(  t->next && t->next->rect != r && ( currentTip != t->next || !delayhide ))
	    t = t->next;
	if ( t->next ) {
	    TQTipManager::Tip *d = t->next;
	    t->next = t->next->next;
	    delete d;
	}
    }

    if ( (*tips)[ w ] == 0 )
	disconnect( w, TQ_SIGNAL(destroyed()), this, TQ_SLOT(clientWidgetDestroyed()) );
#if 0 // not needed, leads sometimes to crashes
    if ( tips->isEmpty() ) {
	// the manager will be recreated if needed
	delete tipManager;
	tipManager = 0;
    }
#endif
}


/*
  The label was destroyed in the program cleanup phase.
*/

void TQTipManager::labelDestroyed()
{
    label = 0;
}


/*
  Remove sender() from the tool tip data structures.
*/

void TQTipManager::clientWidgetDestroyed()
{
    const TQObject *s = sender();
    if ( s )
	remove( (TQWidget*) s );
}


void TQTipManager::remove( TQWidget *w )
{
    TQTipManager::Tip *t = (*tips)[ w ];
    if ( t == 0 )
	return;

    tips->take( w );
    TQTipManager::Tip * d;
    while ( t ) {
	if ( t == currentTip )
	    hideTip();
	d = t->next;
	delete t;
	t = d;
    }

    disconnect( w, TQ_SIGNAL(destroyed()), this, TQ_SLOT(clientWidgetDestroyed()) );
#if 0
    if ( tips->isEmpty() ) {
	delete tipManager;
	tipManager = 0;
    }
#endif
}


void TQTipManager::removeFromGroup( TQToolTipGroup *g )
{
    TQPtrDictIterator<TQTipManager::Tip> i( *tips );
    TQTipManager::Tip *t;
    while( (t = i.current()) != 0 ) {
	++i;
	while ( t ) {
	    if ( t->group == g ) {
		if ( t->group )
		    disconnect( removeTimer, TQ_SIGNAL( timeout() ),
				t->group, TQ_SIGNAL( removeTip() ) );
		t->group = 0;
	    }
	    t = t->next;
	}
    }
}



bool TQTipManager::eventFilter( TQObject *obj, TQEvent *e )
{
    // avoid dumping core in case of application madness, and return
    // quickly for some common but irrelevant events
    if ( e->type() == TQEvent::WindowDeactivate &&
	tqApp && !tqApp->activeWindow() &&
	label && label->isVisible() )
	hideTipAndSleep();

    if ( !tqApp
	 || !obj || !obj->isWidgetType() // isWidgetType() catches most stuff
	 || e->type() == TQEvent::Paint
	 || e->type() == TQEvent::Timer
	 || e->type() == TQEvent::SockAct
	 || !tips )
	return false;
    TQWidget *w = (TQWidget *)obj;

    if ( e->type() == TQEvent::FocusOut || e->type() == TQEvent::FocusIn ) {
	// user moved focus somewhere - hide the tip and sleep
	if ( ((TQFocusEvent*)e)->reason() != TQFocusEvent::Popup )
	    hideTipAndSleep();
	return false;
    }

    TQTipManager::Tip *t = 0;
    while( w && !t ) {
	t = (*tips)[ w ];
	if ( !t )
	    w = w->isTopLevel() ? 0 : w->parentWidget();
    }
    if ( !w )
	return false;

    if ( !t && e->type() != TQEvent::MouseMove) {
	if ( ( e->type() >= TQEvent::MouseButtonPress &&
	       e->type() <= TQEvent::FocusOut) || e->type() == TQEvent::Leave )
	    hideTip();
	return false;
    }

    // with that out of the way, let's get down to action

    switch( e->type() ) {
    case TQEvent::MouseButtonPress:
    case TQEvent::MouseButtonRelease:
    case TQEvent::MouseButtonDblClick:
    case TQEvent::KeyPress:
    case TQEvent::KeyRelease:
	// input - turn off tool tip mode
	hideTipAndSleep();
	break;
    case TQEvent::MouseMove:
	{
	    TQMouseEvent * m = (TQMouseEvent *)e;
	    TQPoint mousePos = w->mapFromGlobal( m->globalPos() );

	    if ( currentTip && !currentTip->rect.contains( mousePos ) ) {
		hideTip();
		if ( m->state() == 0 )
		    return false;
	    }

	    wakeUp.stop();
	    if ( m->state() == 0 &&
		    mousePos.x() >= 0 && mousePos.x() < w->width() &&
		    mousePos.y() >= 0 && mousePos.y() < w->height() ) {
		if ( label && label->isVisible() ) {
		    return false;
		} else {
		    if ( fallAsleep.isActive() ) {
			wakeUp.start( 1, true );
		    } else {
			previousTip = 0;
			wakeUp.start( wakeUpDelay, true );
		    }
		    if ( t->group && t->group->ena &&
			 !t->group->del && !t->groupText.isEmpty() ) {
			removeTimer->stop();
			emit t->group->showTip( t->groupText );
			currentTip = t;
		    }
		}
		widget = w;
		pos = mousePos;
		return false;
	    } else {
		hideTip();
	    }
	}
	break;
    case TQEvent::Leave:
    case TQEvent::Hide:
    case TQEvent::Destroy:
	if ( w == widget )
	    hideTip();
	break;
    default:
	break;
    }
    return false;
}



void TQTipManager::showTip()
{
    if ( !widget || !globally_enabled
#ifndef TQ_WS_X11
	 || !widget->isActiveWindow()
#endif
	)
	return;

    TQTipManager::Tip *t = (*tips)[ widget ];
    while ( t && !t->rect.contains( pos ) )
	t = t->next;
    if ( t == 0 )
	return;

    if ( t == currentTip && label && label->isVisible() )
	return; // nothing to do

    if ( t->tip ) {
	t->tip->maybeTip( pos );
	return;
    }

    if ( t->group && !t->group->ena )
	return;

    int scr;
    if ( TQApplication::desktop()->isVirtualDesktop() )
	scr = TQApplication::desktop()->screenNumber( widget->mapToGlobal( pos ) );
    else
	scr = TQApplication::desktop()->screenNumber( widget );

    if ( label
#if defined(TQ_WS_X11)
	 && label->x11Screen() == widget->x11Screen()
#endif
	 ) {
	// the next two lines are a workaround for TQLabel being too intelligent.
	// TQLabel turns on the wordbreak flag once it gets a richtext. The two lines below
	// ensure we get correct textflags when switching back and forth between a richtext and
	// non richtext tooltip
	label->setText( "" );
	label->setAlignment( AlignAuto | AlignTop );
	label->setText( t->text );
	label->adjustSize();
	if ( t->geometry != TQRect( -1, -1, -1, -1 ) )
	    label->resize( t->geometry.size() );
    } else {
	delete label;
	label = new TQTipLabel( TQApplication::desktop()->screen( scr ), t->text);
	if ( t->geometry != TQRect( -1, -1, -1, -1 ) )
	    label->resize( t->geometry.size() );
	TQ_CHECK_PTR( label );
	connect( label, TQ_SIGNAL(destroyed()), TQ_SLOT(labelDestroyed()) );
    }
    // the above deletion and creation of a TQTipLabel causes events to be sent. We had reports that the widget
    // pointer was 0 after this. This is in principle possible if the wrong kind of events get sent through our event
    // filter in this time. So better be safe and check widget once again here.
    if (!widget)
	return;

#ifdef TQ_WS_X11
    label->x11SetWindowTransient( widget->topLevelWidget());
#endif

#ifdef TQ_WS_MAC
    TQRect screen = TQApplication::desktop()->availableGeometry( scr );
#else
    TQRect screen = TQApplication::desktop()->screenGeometry( scr );
#endif
    TQPoint p;
    if ( t->geometry == TQRect( -1, -1, -1, -1 ) ) {
	p = widget->mapToGlobal( pos ) +
#ifdef TQ_WS_WIN
	    TQPoint( 2, 24 );
#else
	    TQPoint( 2, 16 );
#endif
	if ( p.x() + label->width() > screen.x() + screen.width() )
	    p.rx() -= 4 + label->width();
	if ( p.y() + label->height() > screen.y() + screen.height() )
	    p.ry() -= 24 + label->height();
    } else {
	p = widget->mapToGlobal( t->geometry.topLeft() );
	label->setAlignment( WordBreak | AlignCenter );
	label->setWidth( t->geometry.width() - 4 );
    }
    if ( p.y() < screen.y() )
	p.setY( screen.y() );
    if ( p.x() + label->width() > screen.x() + screen.width() )
	p.setX( screen.x() + screen.width() - label->width() );
    if ( p.x() < screen.x() )
	p.setX( screen.x() );
    if ( p.y() + label->height() > screen.y() + screen.height() )
	p.setY( screen.y() + screen.height() - label->height() );
    if ( label->text().length() ) {
	label->move( p );

#ifndef TQT_NO_EFFECTS
	if ( TQApplication::isEffectEnabled( UI_AnimateTooltip ) == false ||
	     previousTip || preventAnimation )
	    label->show();
	else if ( TQApplication::isEffectEnabled( UI_FadeTooltip ) )
	    qFadeEffect( label );
	else
	    qScrollEffect( label );
#else
	label->show();
#endif

	label->raise();
	fallAsleep.start( 10000, true );
    }

    if ( t->group && t->group->del && !t->groupText.isEmpty() ) {
	removeTimer->stop();
	emit t->group->showTip( t->groupText );
    }

    currentTip = t;
    previousTip = 0;
}


void TQTipManager::hideTip()
{
    TQTimer::singleShot( 250, this, TQ_SLOT(allowAnimation()) );
    preventAnimation = true;

    if ( label && label->isVisible() ) {
	label->hide();
	fallAsleep.start( 2000, true );
	wakeUp.stop();
	if ( currentTip && currentTip->group )
	    removeTimer->start( 100, true );
    } else if ( wakeUp.isActive() ) {
	wakeUp.stop();
	if ( currentTip && currentTip->group &&
	     !currentTip->group->del && !currentTip->groupText.isEmpty() )
	    removeTimer->start( 100, true );
    } else if ( currentTip && currentTip->group ) {
	removeTimer->start( 100, true );
    }

    previousTip = currentTip;
    currentTip = 0;
    if ( previousTip && previousTip->autoDelete )
	remove( widget, previousTip->rect );
    widget = 0;
}

void  TQTipManager::hideTipAndSleep()
{
    hideTip();
    fallAsleep.stop();
}


void TQTipManager::allowAnimation()
{
    preventAnimation = false;
}

TQString TQTipManager::find( TQWidget *w, const TQPoint& pos )
{
    Tip *t = (*tips)[ w ];
    while ( t && !t->rect.contains( pos ) )
	t = t->next;

    return t ? t->text : TQString::null;
}

void TQTipManager::setWakeUpDelay ( int i )
{
    wakeUpDelay = i;
}

/*!
    \class TQToolTip ntqtooltip.h
    \brief The TQToolTip class provides tool tips (balloon help) for
    any widget or rectangular part of a widget.

    \ingroup helpsystem
    \mainclass

    The tip is a short, single line of text reminding the user of the
    widget's or rectangle's function. It is drawn immediately below
    the region in a distinctive black-on-yellow combination.

    The tip can be any Rich-Text formatted string.

    TQToolTipGroup provides a way for tool tips to display another text
    elsewhere (most often in a \link TQStatusBar status bar\endlink).

    At any point in time, TQToolTip is either dormant or active. In
    dormant mode the tips are not shown and in active mode they are.
    The mode is global, not particular to any one widget.

    TQToolTip switches from dormant to active mode when the user hovers
    the mouse on a tip-equipped region for a second or so and remains
    active until the user either clicks a mouse button, presses a key,
    lets the mouse hover for five seconds or moves the mouse outside
    \e all tip-equipped regions for at least a second.

    The TQToolTip class can be used in three different ways:
    \list 1
    \i Adding a tip to an entire widget.
    \i Adding a tip to a fixed rectangle within a widget.
    \i Adding a tip to a dynamic rectangle within a widget.
    \endlist

    To add a tip to a widget, call the \e static function
    TQToolTip::add() with the widget and tip as arguments:

    \code
	TQToolTip::add( quitButton, "Leave the application" );
    \endcode

    This is the simplest and most common use of TQToolTip. The tip
    will be deleted automatically when \e quitButton is deleted, but
    you can remove it yourself, too:

    \code
	TQToolTip::remove( quitButton );
    \endcode

    You can also display another text (typically in a \link TQStatusBar
    status bar),\endlink courtesy of \l{TQToolTipGroup}. This example
    assumes that \e grp is a \c{TQToolTipGroup *} and is already
    connected to the appropriate status bar:

    \code
	TQToolTip::add( quitButton, "Leave the application", grp,
		       "Leave the application, prompting to save if necessary" );
	TQToolTip::add( closeButton, "Close this window", grp,
		       "Close this window, prompting to save if necessary" );
    \endcode

    To add a tip to a fixed rectangle within a widget, call the static
    function TQToolTip::add() with the widget, rectangle and tip as
    arguments. (See the \c tooltip/tooltip.cpp example.) Again, you
    can supply a \c{TQToolTipGroup *} and another text if you want.

    Both of these are one-liners and cover the majority of cases. The
    third and most general way to use TQToolTip requires you to
    reimplement a pure virtual function to decide whether to pop up a
    tool tip. The \c tooltip/tooltip.cpp example demonstrates this
    too. This mode can be used to implement tips for text that can
    move as the user scrolls, for example.

    To use TQToolTip like this, you must subclass TQToolTip and
    reimplement maybeTip(). TQToolTip calls maybeTip() when a tip
    should pop up, and maybeTip() decides whether to show a tip.

    Tool tips can be globally disabled using
    TQToolTip::setGloballyEnabled() or disabled in groups with
    TQToolTipGroup::setEnabled().

    You can retrieve the text of a tooltip for a given position within
    a widget using textFor().

    The global tooltip font and palette can be set with the static
    setFont() and setPalette() functions respectively.

    \sa TQStatusBar TQWhatsThis TQToolTipGroup
    \link guibooks.html#fowler GUI Design Handbook: Tool Tip\endlink
*/


/*!
    Returns the font common to all tool tips.

    \sa setFont()
*/

TQFont TQToolTip::font()
{
    TQTipLabel l(0,"");
    return TQApplication::font( &l );
}


/*!
    Sets the font for all tool tips to \a font.

    \sa font()
*/

void TQToolTip::setFont( const TQFont &font )
{
    TQApplication::setFont( font, true, "TQTipLabel" );
}


/*!
    Returns the palette common to all tool tips.

    \sa setPalette()
*/

TQPalette TQToolTip::palette()
{
    TQTipLabel l(0,"");
    return TQApplication::palette( &l );
}


/*!
    Sets the palette for all tool tips to \a palette.

    \sa palette()
*/

void TQToolTip::setPalette( const TQPalette &palette )
{
    TQApplication::setPalette( palette, true, "TQTipLabel" );
}

/*!
    Constructs a tool tip object. This is only necessary if you need
    tool tips on regions that can move within the widget (most often
    because the widget's contents can scroll).

    \a widget is the widget you want to add dynamic tool tips to and
    \a group (optional) is the tool tip group they should belong to.

    \warning TQToolTip is not a subclass of TQObject, so the instance of
    TQToolTip is not deleted when \a widget is deleted.

    \warning If you delete the tool tip before you have deleted
    \a widget then you need to make sure you call remove() yourself from
    \a widget in your reimplemented TQToolTip destructor.

    \code
	MyToolTip::~MyToolTip()
	{
	    remove( widget );
	}
    \endcode

    \sa maybeTip().
*/

TQToolTip::TQToolTip( TQWidget * widget, TQToolTipGroup * group )
{
    p = widget;
    g = group;
    initTipManager();
    tipManager->add( p, entireWidget(),
		     TQString::null, g, TQString::null, this, false );
}

TQToolTip::~TQToolTip()
{
}


/*!
    Adds a tool tip to \a widget. \a text is the text to be shown in
    the tool tip.

    This is the most common entry point to the TQToolTip class; it is
    suitable for adding tool tips to buttons, checkboxes, comboboxes
    and so on.
*/

void TQToolTip::add( TQWidget *widget, const TQString &text )
{
    initTipManager();
    tipManager->add( widget, entireWidget(),
		     text, 0, TQString::null, 0, false );
}


/*!
    \overload

    Adds a tool tip to \a widget and to tool tip group \a group.

    \a text is the text shown in the tool tip and \a longText is the
    text emitted from \a group.

    Normally, \a longText is shown in a \link TQStatusBar status
    bar\endlink or similar.
*/

void TQToolTip::add( TQWidget *widget, const TQString &text,
		    TQToolTipGroup *group, const TQString& longText )
{
    initTipManager();
    tipManager->add( widget, entireWidget(), text, group, longText, 0, false );
}


/*!
    Removes the tool tip from \a widget.

    If there is more than one tool tip on \a widget, only the one
    covering the entire widget is removed.
*/

void TQToolTip::remove( TQWidget * widget )
{
    if ( tipManager )
	tipManager->remove( widget, entireWidget() );
}

/*!
    \overload

    Adds a tool tip to a fixed rectangle, \a rect, within \a widget.
    \a text is the text shown in the tool tip.
*/

void TQToolTip::add( TQWidget * widget, const TQRect & rect, const TQString &text )
{
    initTipManager();
    tipManager->add( widget, rect, text, 0, TQString::null, 0, false );
}


/*!
    \overload

    Adds a tool tip to an entire \a widget and to tool tip group \a
    group. The tooltip will disappear when the mouse leaves the \a
    rect.

    \a text is the text shown in the tool tip and \a groupText is the
    text emitted from \a group.

    Normally, \a groupText is shown in a \link TQStatusBar status
    bar\endlink or similar.
*/

void TQToolTip::add( TQWidget *widget, const TQRect &rect,
		    const TQString& text,
		    TQToolTipGroup *group, const TQString& groupText )
{
    initTipManager();
    tipManager->add( widget, rect, text, group, groupText, 0, false );
}


/*!
    \overload

    Removes any tool tip for \a rect from \a widget.

    If there is more than one tool tip on \a widget, only the one
    covering rectangle \a rect is removed.
*/

void TQToolTip::remove( TQWidget * widget, const TQRect & rect )
{
    if ( tipManager )
	tipManager->remove( widget, rect );
}

/*!
    Returns the tool tip text for \a widget at position \a pos, or
    TQString::null if there is no tool tip for the given widget and
    position.
*/

TQString TQToolTip::textFor( TQWidget *widget, const TQPoint& pos )
{
    if ( tipManager )
	return tipManager->find( widget, pos );
    return TQString::null;
}

/*!
    Hides any tip that is currently being shown.

    Normally, there is no need to call this function; TQToolTip takes
    care of showing and hiding the tips as the user moves the mouse.
*/

void TQToolTip::hide()
{
    if ( tipManager )
	tipManager->hideTipAndSleep();
}

/*!
    \fn virtual void TQToolTip::maybeTip( const TQPoint & p);

    This pure virtual function is half of the most versatile interface
    TQToolTip offers.

    It is called when there is a possibility that a tool tip should be
    shown and must decide whether there is a tool tip for the point \a
    p in the widget that this TQToolTip object relates to. If so,
    maybeTip() must call tip() with the rectangle the tip applies to,
    the tip's text and optionally the TQToolTipGroup details and the
    geometry in screen coordinates.

    \a p is given in that widget's local coordinates. Most maybeTip()
    implementations will be of the form:

    \code
	if ( <something> ) {
	    tip( <something>, <something> );
	}
    \endcode

    The first argument to tip() (a rectangle) must encompass \a p,
    i.e. the tip must apply to the current mouse position; otherwise
    TQToolTip's operation is undefined.

    Note that the tip will disappear once the mouse moves outside the
    rectangle you give to tip(), and will not reappear if the mouse
    moves back in: maybeTip() is called again instead.

    \sa tip()
*/


/*!
    Immediately pops up a tip saying \a text and removes the tip once
    the cursor moves out of rectangle \a rect (which is given in the
    coordinate system of the widget this TQToolTip relates to).

    The tip will not reappear if the cursor moves back; your
    maybeTip() must reinstate it each time.
*/

void TQToolTip::tip( const TQRect & rect, const TQString &text )
{
    initTipManager();
    tipManager->add( parentWidget(), rect, text, 0, TQString::null, 0, true );
}

/*!
    \overload

    Immediately pops up a tip saying \a text and removes that tip once
    the cursor moves out of rectangle \a rect (which is given in the
    coordinate system of the widget this TQToolTip relates to). \a
    groupText is the text emitted from the group.

    The tip will not reappear if the cursor moves back; your
    maybeTip() must reinstate it each time.
*/

void TQToolTip::tip( const TQRect & rect, const TQString &text,
		    const TQString& groupText )
{
    initTipManager();
    tipManager->add( parentWidget(), rect, text, group(), groupText, 0, true );
}

/*!
    \overload

    Immediately pops up a tip within the rectangle \a geometry, saying
    \a text and removes the tip once the cursor moves out of rectangle
    \a rect. Both rectangles are given in the coordinate system of the
    widget this TQToolTip relates to.

    The tip will not reappear if the cursor moves back; your
    maybeTip() must reinstate it each time.

    If the tip does not fit inside \a geometry, the tip expands.
*/

void TQToolTip::tip( const TQRect &rect, const TQString &text, const TQRect &geometry )
{
    initTipManager();
    tipManager->add( geometry, parentWidget(), rect, text, 0, TQString::null, 0, true );
}

/*!
    \overload

    Immediately pops up a tip within the rectangle \a geometry, saying
    \a text and removes the tip once the cursor moves out of rectangle
    \a rect. \a groupText is the text emitted from the group. Both
    rectangles are given in the coordinate system of the widget this
    TQToolTip relates to.

    The tip will not reappear if the cursor moves back; your
    maybeTip() must reinstate it each time.

    If the tip does not fit inside \a geometry, the tip expands.
*/

void TQToolTip::tip( const TQRect &rect, const TQString &text, const TQString& groupText, const TQRect &geometry )
{
    initTipManager();
    tipManager->add( geometry, parentWidget(), rect, text, group(), groupText, 0, true );
}



/*!
    Immediately removes all tool tips for this tooltip's parent
    widget.
*/

void TQToolTip::clear()
{
    if ( tipManager )
	tipManager->remove( parentWidget() );
}


/*!
    \fn TQWidget * TQToolTip::parentWidget() const

    Returns the widget this TQToolTip applies to.

    The tool tip is destroyed automatically when the parent widget is
    destroyed.

    \sa group()
*/


/*!
    \fn TQToolTipGroup * TQToolTip::group() const

    Returns the tool tip group this TQToolTip is a member of or 0 if it
    isn't a member of any group.

    The tool tip group is the object responsible for maintaining
    contact between tool tips and a \link TQStatusBar status
    bar\endlink or something else which can show the longer help text.

    \sa parentWidget(), TQToolTipGroup
*/


/*!
    \class TQToolTipGroup ntqtooltip.h
    \brief The TQToolTipGroup class collects tool tips into related groups.

    \ingroup helpsystem

    Tool tips can display \e two texts: one in the tip and
    (optionally) one that is typically in a \link TQStatusBar status
    bar\endlink. TQToolTipGroup provides a way to link tool tips to
    this status bar.

    TQToolTipGroup has practically no API; it is only used as an
    argument to TQToolTip's member functions, for example like this:

    \code
	TQToolTipGroup * grp = new TQToolTipGroup( this, "tool tip relay" );
	connect( grp, TQ_SIGNAL(showTip(const TQString&)),
		 myLabel, TQ_SLOT(setText(const TQString&)) );
	connect( grp, TQ_SIGNAL(removeTip()),
		 myLabel, TQ_SLOT(clear()) );
	TQToolTip::add( giraffeButton, "feed giraffe",
		       grp, "Give the giraffe a meal" );
	TQToolTip::add( gorillaButton, "feed gorilla",
		       grp, "Give the gorilla a meal" );
    \endcode

    This example makes the object myLabel (which you must supply)
    display (one assumes, though you can make myLabel do anything, of
    course) the strings "Give the giraffe a meal" and "Give the
    gorilla a meal" while the relevant tool tips are being displayed.

    Deleting a tool tip group removes the tool tips in it.
*/

/*!
    \fn void TQToolTipGroup::showTip (const TQString &longText)

    This signal is emitted when one of the tool tips in the group is
    displayed. \a longText is the extra text for the displayed tool
    tip.

    \sa removeTip()
*/

/*!
    \fn void TQToolTipGroup::removeTip ()

    This signal is emitted when a tool tip in this group is hidden.
    See the TQToolTipGroup documentation for an example of use.

    \sa showTip()
*/


/*!
    Constructs a tool tip group called \a name, with parent \a parent.
*/

TQToolTipGroup::TQToolTipGroup( TQObject *parent, const char *name )
    : TQObject( parent, name )
{
    del = true;
    ena = true;
}


/*!
    Destroys this tool tip group and all tool tips in it.
*/

TQToolTipGroup::~TQToolTipGroup()
{
    if ( tipManager )
	tipManager->removeFromGroup( this );
}


/*!
    \property TQToolTipGroup::delay
    \brief whether the display of the group text is delayed.

    If set to true (the default), the group text is displayed at the
    same time as the tool tip. Otherwise, the group text is displayed
    immediately when the cursor enters the widget.
*/

bool TQToolTipGroup::delay() const
{
    return del;
}

void TQToolTipGroup::setDelay( bool enable )
{
#if 0
    if ( enable && !del ) {
	// maybe we should show the text at once?
    }
#endif
    del = enable;
}

/*!
    \fn static void TQToolTip::setEnabled( bool enable )

    \obsolete
*/
/*!
    \fn static bool TQToolTip::enabled()

    \obsolete
*/
/*!
    \property TQToolTipGroup::enabled
    \brief whether tool tips in the group are enabled.

    This property's default is true.
*/

void TQToolTipGroup::setEnabled( bool enable )
{
    ena = enable;
}

bool TQToolTipGroup::enabled() const
{
    return (bool)ena;
}

/*!
    If \a enable is true sets all tool tips to be enabled (shown when
    needed); if \a enable is false sets all tool tips to be disabled
    (never shown).

    By default, tool tips are enabled. Note that this function affects
    all tool tips in the entire application.

    \sa TQToolTipGroup::setEnabled()
*/

void TQToolTip::setGloballyEnabled( bool enable )
{
    globally_enabled = enable;
}

/*!
    Returns whether tool tips are enabled globally.

    \sa setGloballyEnabled()
*/
bool TQToolTip::isGloballyEnabled()
{
    return globally_enabled;
}

/*!
  Sets the wakeup delay for all tooltips to \a i
  milliseconds.
*/
void TQToolTip::setWakeUpDelay ( int i )
{
    initTipManager();
    tipManager->setWakeUpDelay(i);
}


#include "qtooltip.moc"
#endif
