/****************************************************************************
**
** Implementation of TQWhatsThis class
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

#include "ntqwhatsthis.h"
#ifndef TQT_NO_WHATSTHIS
#include "ntqapplication.h"
#include "ntqpaintdevicemetrics.h"
#include "ntqpixmap.h"
#include "ntqpainter.h"
#include "ntqtimer.h"
#include "ntqptrdict.h"
#include "ntqtoolbutton.h"
#include "ntqshared.h"
#include "ntqcursor.h"
#include "ntqbitmap.h"
#include "ntqtooltip.h"
#include "ntqsimplerichtext.h"
#include "ntqstylesheet.h"
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif
#if defined(TQ_WS_WIN)
#include "qt_windows.h"
#ifndef SPI_GETDROPSHADOW
#define SPI_GETDROPSHADOW                   0x1024
#endif
#endif

/*!
    \class TQWhatsThis ntqwhatsthis.h
    \brief The TQWhatsThis class provides a simple description of any
    widget, i.e. answering the question "What's this?".

    \ingroup helpsystem
    \mainclass

    "What's this?" help is part of an application's online help system
    that provides users with information about functionality, usage,
    background etc., in various levels of detail from short tool tips
    to full text browsing help windows.

    TQWhatsThis provides a single window with an explanatory text that
    pops up when the user asks "What's this?". The default way to do
    this is to focus the relevant widget and press Shift+F1. The help
    text appears immediately; it goes away as soon as the user does
    something else.

    (Note that if there is an accelerator for Shift+F1, this mechanism
    will not work.)

    To add "What's this?" text to a widget you simply call
    TQWhatsThis::add() for the widget. For example, to assign text to a
    menu item, call TQMenuData::setWhatsThis(); for a global
    accelerator key, call TQAccel::setWhatsThis() and If you're using
    actions, use TQAction::setWhatsThis().

    The text can be either rich text or plain text. If you specify a
    rich text formatted string, it will be rendered using the default
    stylesheet. This makes it possible to embed images. See
    TQStyleSheet::defaultSheet() for details.

    \quotefile action/application.cpp
    \skipto fileOpenText
    \printuntil setWhatsThis

    An alternative way to enter "What's this?" mode is to use the
    ready-made tool bar tool button from
    TQWhatsThis::whatsThisButton(). By invoking this context help
    button (in the picture below the first one from the right) the
    user switches into "What's this?" mode. If they now click on a
    widget the appropriate help text is shown. The mode is left when
    help is given or when the user presses Esc.

    \img whatsthis.png

    If you are using TQMainWindow you can also use the
    TQMainWindow::whatsThis() slot to invoke the mode from a menu item.

    For more control you can create a dedicated TQWhatsThis object for
    a special widget. By subclassing and reimplementing
    TQWhatsThis::text() it is possible to have different help texts,
    depending on the position of the mouse click. By reimplementing
    TQWhatsThis::clicked() it is possible to have hyperlinks inside the
    help texts.

    If you wish to control the "What's this?" behavior of a widget
    manually see TQWidget::customWhatsThis().

    The What's This object can be removed using TQWhatsThis::remove(),
    although this is rarely necessary because it is automatically
    removed when the widget is destroyed.

    \sa TQToolTip
*/

// a special button
class TQWhatsThisButton: public TQToolButton
{
    TQ_OBJECT

public:
    TQWhatsThisButton( TQWidget * parent, const char * name );
    ~TQWhatsThisButton();

public slots:
    void mouseReleased();

};


class TQWhatsThat : public TQWidget
{
    TQ_OBJECT
public:
    TQWhatsThat( TQWidget* w, const TQString& txt, TQWidget* parent, const char* name );
    ~TQWhatsThat() ;

public slots:
    void hide();
    inline void widgetDestroyed() { widget = 0; }

protected:
    void mousePressEvent( TQMouseEvent* );
    void mouseReleaseEvent( TQMouseEvent* );
    void mouseMoveEvent( TQMouseEvent* );
    void keyPressEvent( TQKeyEvent* );
    void paintEvent( TQPaintEvent* );

private:
    TQString text;
#ifndef TQT_NO_RICHTEXT
    TQSimpleRichText* doc;
#endif
    TQString anchor;
    bool pressed;
    TQWidget* widget;
};


class TQWhatsThisPrivate: public TQObject
{
    TQ_OBJECT
public:

    // an item for storing texts
    struct WhatsThisItem: public TQShared
    {
	WhatsThisItem(): TQShared() { whatsthis = 0; }
	~WhatsThisItem();
	TQString s;
	TQWhatsThis* whatsthis;
    };

    // the (these days pretty small) state machine
    enum State { Inactive, Waiting };

    TQWhatsThisPrivate();
    ~TQWhatsThisPrivate();

    bool eventFilter( TQObject *, TQEvent * );

    WhatsThisItem* newItem( TQWidget * widget );
    void add( TQWidget * widget, TQWhatsThis* special );
    void add( TQWidget * widget, const TQString& text );

    // say it.
    void say( TQWidget *, const TQString&, const TQPoint&  );

    // setup and teardown
    static void setUpWhatsThis();

    void enterWhatsThisMode();
    void leaveWhatsThisMode();

    // variables
    TQWhatsThat * whatsThat;
    TQPtrDict<WhatsThisItem> * dict;
    TQPtrDict<TQWidget> * tlw;
    TQPtrDict<TQWhatsThisButton> * buttons;
    State state;

private slots:
    void cleanupWidget()
    {
	const TQObject* o = sender();
	if ( o->isWidgetType() ) // sanity
	    TQWhatsThis::remove((TQWidget*)o);
    }

};

// static, but static the less-typing way
static TQWhatsThisPrivate * wt = 0;

// shadowWidth not const, for XP drop-shadow-fu turns it to 0
static int shadowWidth = 6;   // also used as '5' and '6' and even '8' below
const int vMargin = 8;
const int hMargin = 12;

// Lets TQPopupMenu destroy the TQWhatsThat.
void qWhatsThisBDH()
{
    if ( wt && wt->whatsThat )
	wt->whatsThat->hide();
}


TQWhatsThat::TQWhatsThat( TQWidget* w, const TQString& txt, TQWidget* parent, const char* name )
    : TQWidget( parent, name, WType_Popup ), text( txt ), pressed( false ), widget( w )
{

    setBackgroundMode( NoBackground );
    setPalette( TQToolTip::palette() );
    setMouseTracking( true );
#ifndef TQT_NO_CURSOR
    setCursor( arrowCursor );
#endif

    if ( widget )
	connect( widget, TQ_SIGNAL( destroyed() ), this, TQ_SLOT( widgetDestroyed() ) );


    TQRect r;
#ifndef TQT_NO_RICHTEXT
    doc = 0;
    if ( TQStyleSheet::mightBeRichText( text ) ) {
	TQFont f = TQApplication::font( this );
	doc = new TQSimpleRichText( text, f );
	doc->adjustSize();
	r.setRect( 0, 0, doc->width(), doc->height() );
    }
    else
#endif
    {
	int sw = TQApplication::desktop()->width() / 3;
	if ( sw < 200 )
	    sw = 200;
	else if ( sw > 300 )
	    sw = 300;

	r = fontMetrics().boundingRect( 0, 0, sw, 1000,
					AlignAuto + AlignTop + WordBreak + ExpandTabs,
					text );
    }
#if defined(TQ_WS_WIN)
    if ( (qWinVersion()&WV_NT_based) > WV_2000 ) {
	BOOL shadow;
	SystemParametersInfo( SPI_GETDROPSHADOW, 0, &shadow, 0 );
	shadowWidth = shadow ? 0 : 6;
    }
#endif
    resize( r.width() + 2*hMargin + shadowWidth, r.height() + 2*vMargin + shadowWidth );
}

TQWhatsThat::~TQWhatsThat()
{
    if ( wt && wt->whatsThat == this )
	wt->whatsThat = 0;
#ifndef TQT_NO_RICHTEXT
    if ( doc )
	delete doc;
#endif
}

void TQWhatsThat::hide()
{
    TQWidget::hide();
#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::ContextHelpEnd );
#endif
}

void TQWhatsThat::mousePressEvent( TQMouseEvent* e )
{
    pressed = true;
    if ( e->button() == LeftButton && rect().contains( e->pos() ) ) {
#ifndef TQT_NO_RICHTEXT
	if ( doc )
	    anchor = doc->anchorAt( e->pos() -  TQPoint( hMargin, vMargin) );
#endif
	return;
    }
    hide();
}

void TQWhatsThat::mouseReleaseEvent( TQMouseEvent* e )
{
    if ( !pressed )
	return;
#ifndef TQT_NO_RICHTEXT
    if ( e->button() == LeftButton && doc && rect().contains( e->pos() ) ) {
	TQString a = doc->anchorAt( e->pos() -  TQPoint( hMargin, vMargin ) );
	TQString href;
	if ( anchor == a )
	    href = a;
	anchor = TQString::null;
	if ( widget && wt && wt->dict ) {
	    TQWhatsThisPrivate::WhatsThisItem * i = wt->dict->find( widget );
	    if ( i  && i->whatsthis && !i->whatsthis->clicked( href ) )
		return;
	}
    }
#endif
    hide();
}

void TQWhatsThat::mouseMoveEvent( TQMouseEvent* e)
{
#ifndef TQT_NO_RICHTEXT
#ifndef TQT_NO_CURSOR
    if ( !doc )
	return;
    TQString a = doc->anchorAt( e->pos() -  TQPoint( hMargin, vMargin ) );
    if ( !a.isEmpty() )
	setCursor( pointingHandCursor );
    else
	setCursor( arrowCursor );
#endif
#endif
}


void TQWhatsThat::keyPressEvent( TQKeyEvent* )
{
    hide();
}



void TQWhatsThat::paintEvent( TQPaintEvent* )
{
    bool drawShadow = true;
#if defined(TQ_WS_WIN)
    if ( (qWinVersion()&WV_NT_based) > WV_2000 ) {
	BOOL shadow;
	SystemParametersInfo( SPI_GETDROPSHADOW, 0, &shadow, 0 );
	drawShadow = !shadow;
    }
#elif defined(TQ_WS_MACX)
    drawShadow = false; //never draw it on OS X we get it for free
#endif

    TQRect r = rect();
    if ( drawShadow )
	r.addCoords( 0, 0, -shadowWidth, -shadowWidth );
    TQPainter p( this);
    p.setPen( colorGroup().foreground() );
    p.drawRect( r );
    p.setPen( colorGroup().mid() );
    p.setBrush( colorGroup().brush( TQColorGroup::Background ) );
    int w = r.width();
    int h = r.height();
    p.drawRect( 1, 1, w-2, h-2 );
    if ( drawShadow ) {
	p.setPen( colorGroup().shadow() );
	p.drawPoint( w + 5, 6 );
	p.drawLine( w + 3, 6, w + 5, 8 );
	p.drawLine( w + 1, 6, w + 5, 10 );
	int i;
	for( i=7; i < h; i += 2 )
	    p.drawLine( w, i, w + 5, i + 5 );
	for( i = w - i + h; i > 6; i -= 2 )
	    p.drawLine( i, h, i + 5, h + 5 );
	for( ; i > 0 ; i -= 2 )
	    p.drawLine( 6, h + 6 - i, i + 5, h + 5 );
    }
    p.setPen( colorGroup().foreground() );
    r.addCoords( hMargin, vMargin, -hMargin, -vMargin );

#ifndef TQT_NO_RICHTEXT
    if ( doc ) {
	doc->draw( &p, r.x(), r.y(), r, colorGroup(), 0 );
    }
    else
#endif
    {
	p.drawText( r, AlignAuto + AlignTop + WordBreak + ExpandTabs, text );
    }
}

// the item
TQWhatsThisPrivate::WhatsThisItem::~WhatsThisItem()
{
    if ( count )
	tqFatal( "TQWhatsThis: Internal error (%d)", count );
    delete whatsthis;
}


static const char * const button_image[] = {
"16 16 3 1",
" 	c None",
"o	c #000000",
"a	c #000080",
"o        aaaaa  ",
"oo      aaa aaa ",
"ooo    aaa   aaa",
"oooo   aa     aa",
"ooooo  aa     aa",
"oooooo  a    aaa",
"ooooooo     aaa ",
"oooooooo   aaa  ",
"ooooooooo aaa   ",
"ooooo     aaa   ",
"oo ooo          ",
"o  ooo    aaa   ",
"    ooo   aaa   ",
"    ooo         ",
"     ooo        ",
"     ooo        "};

// the button class
TQWhatsThisButton::TQWhatsThisButton( TQWidget * parent, const char * name )
    : TQToolButton( parent, name )
{
    TQPixmap p( (const char**)button_image );
    setPixmap( p );
    setToggleButton( true );
    setAutoRaise( true );
    setFocusPolicy( NoFocus );
    setTextLabel( tr( "What's this?" ) );
    wt->buttons->insert( (void *)this, this );
    connect( this, TQ_SIGNAL( released() ),
	     this, TQ_SLOT( mouseReleased() ) );
}


TQWhatsThisButton::~TQWhatsThisButton()
{
    if ( wt && wt->buttons )
	wt->buttons->take( (void *)this );
}


void TQWhatsThisButton::mouseReleased()
{
    if ( wt->state == TQWhatsThisPrivate::Inactive && isOn() ) {
	TQWhatsThisPrivate::setUpWhatsThis();
#ifndef TQT_NO_CURSOR
	TQApplication::setOverrideCursor( whatsThisCursor, false );
#endif
	wt->state = TQWhatsThisPrivate::Waiting;
	tqApp->installEventFilter( wt );
    }
}

static void qWhatsThisPrivateCleanup()
{
    if( wt ) {
	delete wt;
	wt = 0;
    }
}

// the what's this manager class
TQWhatsThisPrivate::TQWhatsThisPrivate()
    : TQObject( 0, "global what's this object" )
{
    whatsThat = 0;
    dict = new TQPtrDict<TQWhatsThisPrivate::WhatsThisItem>;
    tlw = new TQPtrDict<TQWidget>;
    wt = this;
    buttons = new TQPtrDict<TQWhatsThisButton>;
    state = Inactive;
}

TQWhatsThisPrivate::~TQWhatsThisPrivate()
{
#ifndef TQT_NO_CURSOR
    if ( state == Waiting && tqApp )
	TQApplication::restoreOverrideCursor();
#endif
    // the two straight-and-simple dicts
    delete tlw;
    delete buttons;

    // then delete the complex one.
    TQPtrDictIterator<WhatsThisItem> it( *dict );
    WhatsThisItem * i;
    TQWidget * w;
    while( (i=it.current()) != 0 ) {
	w = (TQWidget *)it.currentKey();
	++it;
	dict->take( w );
	if ( i->deref() )
	    delete i;
    }
    delete dict;
    if ( whatsThat && !whatsThat->parentWidget() ) {
	delete whatsThat;
    }
    // and finally lose wt
    wt = 0;
}

bool TQWhatsThisPrivate::eventFilter( TQObject * o, TQEvent * e )
{
    switch( state ) {
    case Waiting:
	if ( e->type() == TQEvent::MouseButtonPress && o->isWidgetType() ) {
	    TQWidget * w = (TQWidget *) o;
	    if ( ( (TQMouseEvent*)e)->button() == RightButton )
		return false; // ignore RMB
	    if ( w->customWhatsThis() )
		return false;
	    TQWhatsThisPrivate::WhatsThisItem * i = 0;
	    TQMouseEvent* me = (TQMouseEvent*) e;
	    TQPoint p = me->pos();
	    while( w && !i ) {
		i = dict->find( w );
		if ( !i ) {
		    p += w->pos();
		    w = w->parentWidget( true );
		}
	    }
	    leaveWhatsThisMode();
	    if (!i ) {
#if defined(QT_ACCESSIBILITY_SUPPORT)
		TQAccessible::updateAccessibility( this, 0, TQAccessible::ContextHelpEnd );
#endif
		return true;
	    }
	    if ( i->whatsthis )
		say( w, i->whatsthis->text( p ), me->globalPos() );
	    else
		say( w, i->s, me->globalPos() );
	    return true;
	} else if ( e->type() == TQEvent::MouseButtonRelease ) {
	    if ( ( (TQMouseEvent*)e)->button() == RightButton )
		return false; // ignore RMB
	    return !o->isWidgetType() || !((TQWidget*)o)->customWhatsThis();
	} else if ( e->type() == TQEvent::MouseMove ) {
	    return !o->isWidgetType() || !((TQWidget*)o)->customWhatsThis();
	} else if ( e->type() == TQEvent::KeyPress ) {
	    TQKeyEvent* kev = (TQKeyEvent*)e;

	    if ( kev->key() == TQt::Key_Escape ) {
		leaveWhatsThisMode();
		return true;
	    } else if ( o->isWidgetType() && ((TQWidget*)o)->customWhatsThis() ) {
		return false;
	    } else if ( kev->key() == Key_Menu ||
			( kev->key() == Key_F10 &&
			  kev->state() == ShiftButton ) ) {
		// we don't react to these keys, they are used for context menus
		return false;
	    } else if ( kev->state() == kev->stateAfter() &&
			kev->key() != Key_Meta ) {  // not a modifier key
		leaveWhatsThisMode();
	    }
	} else if ( e->type() == TQEvent::MouseButtonDblClick ) {
	    return true;
	}
	break;
    case Inactive:
 	if ( e->type() == TQEvent::Accel &&
	     ((TQKeyEvent *)e)->key() == Key_F1 &&
 	     o->isWidgetType() &&
	     ((TQKeyEvent *)e)->state() == ShiftButton ) {
 	    TQWidget * w = ((TQWidget *)o)->focusWidget();
	    if ( !w )
		break;
	    TQString s = TQWhatsThis::textFor( w, TQPoint(0,0), true );
 	    if ( !s.isNull() ) {
		say ( w, s, w->mapToGlobal( w->rect().center() ) );
		((TQKeyEvent *)e)->accept();
		return true;
 	    }
 	}
	break;
    }
    return false;
}



void TQWhatsThisPrivate::setUpWhatsThis()
{
    if ( !wt ) {
	wt = new TQWhatsThisPrivate();

	// It is necessary to use a post routine, because
	// the destructor deletes pixmaps and other stuff that
	// needs a working X connection under X11.
	tqAddPostRoutine( qWhatsThisPrivateCleanup );
    }
}


void TQWhatsThisPrivate::enterWhatsThisMode()
{
#if defined(QT_ACCESSIBILITY_SUPPORT)
    TQAccessible::updateAccessibility( this, 0, TQAccessible::ContextHelpStart );
#endif
}


void TQWhatsThisPrivate::leaveWhatsThisMode()
{
    if ( state == Waiting ) {
	TQPtrDictIterator<TQWhatsThisButton> it( *(wt->buttons) );
	TQWhatsThisButton * b;
	while( (b=it.current()) != 0 ) {
	    ++it;
	    b->setOn( false );
	}
#ifndef TQT_NO_CURSOR
	TQApplication::restoreOverrideCursor();
#endif
	state = Inactive;
	tqApp->removeEventFilter( this );
    }
}



void TQWhatsThisPrivate::say( TQWidget * widget, const TQString &text, const TQPoint& ppos)
{
    if ( text.isEmpty() )
	return;
    // make a fresh widget, and set it up
    delete whatsThat;
    whatsThat = new TQWhatsThat(
			       widget, text,
#if defined(TQ_WS_X11)
			       TQApplication::desktop()->screen( widget ?
								widget->x11Screen() :
								TQCursor::x11Screen() ),
#else
			       0,
#endif
			       "automatic what's this? widget" );


    // okay, now to find a suitable location

    int scr = ( widget ?
		TQApplication::desktop()->screenNumber( widget ) :
#if defined(TQ_WS_X11)
		TQCursor::x11Screen()
#else
		TQApplication::desktop()->screenNumber( ppos )
#endif // TQ_WS_X11
		);
    TQRect screen = TQApplication::desktop()->screenGeometry( scr );

    int x;
    int w = whatsThat->width();
    int h = whatsThat->height();
    int sx = screen.x();
    int sy = screen.y();

    // first try locating the widget immediately above/below,
    // with nice alignment if possible.
    TQPoint pos;
    if ( widget )
	pos = widget->mapToGlobal( TQPoint( 0,0 ) );

    if ( widget && w > widget->width() + 16 )
	x = pos.x() + widget->width()/2 - w/2;
    else
	x = ppos.x() - w/2;

	// squeeze it in if that would result in part of what's this
	// being only partially visible
    if ( x + w  + shadowWidth > sx+screen.width() )
	x = (widget? (TQMIN(screen.width(),
			   pos.x() + widget->width())
		      ) : screen.width() )
	    - w;

    if ( x < sx )
	x = sx;

    int y;
    if ( widget && h > widget->height() + 16 ) {
	y = pos.y() + widget->height() + 2; // below, two pixels spacing
	// what's this is above or below, wherever there's most space
	if ( y + h + 10 > sy+screen.height() )
	    y = pos.y() + 2 - shadowWidth - h; // above, overlap
    }
    y = ppos.y() + 2;

	// squeeze it in if that would result in part of what's this
	// being only partially visible
    if ( y + h + shadowWidth > sy+screen.height() )
	y = ( widget ? (TQMIN(screen.height(),
			     pos.y() + widget->height())
			) : screen.height() )
	    - h;
    if ( y < sy )
	y = sy;

    whatsThat->move( x, y );
    whatsThat->show();
}

TQWhatsThisPrivate::WhatsThisItem* TQWhatsThisPrivate::newItem( TQWidget * widget )
{
    WhatsThisItem * i = dict->find( (void *)widget );
    if ( i )
	TQWhatsThis::remove( widget );
    i = new WhatsThisItem;
    dict->insert( (void *)widget, i );
    TQWidget * t = widget->topLevelWidget();
    if ( !tlw->find( (void *)t ) ) {
	tlw->insert( (void *)t, t );
	t->installEventFilter( this );
    }
    connect( widget, TQ_SIGNAL(destroyed()), this, TQ_SLOT(cleanupWidget()) );
    return i;
}

void TQWhatsThisPrivate::add( TQWidget * widget, TQWhatsThis* special )
{
    newItem( widget )->whatsthis = special;
}

void TQWhatsThisPrivate::add( TQWidget * widget, const TQString &text )
{
    newItem( widget )->s = text;
}


// and finally the What's This class itself

/*!
    Adds \a text as "What's this" help for \a widget. If the text is
    rich text formatted (i.e. it contains markup) it will be rendered
    with the default stylesheet TQStyleSheet::defaultSheet().

    The text is destroyed if the widget is later destroyed, so it need
    not be explicitly removed.

    \sa remove()
*/
void TQWhatsThis::add( TQWidget * widget, const TQString &text )
{
    if ( text.isEmpty() )
	return; // pointless
    TQWhatsThisPrivate::setUpWhatsThis();
    wt->add(widget,text);
}


/*!
    Removes the "What's this?" help associated with the \a widget.
    This happens automatically if the widget is destroyed.

    \sa add()
*/
void TQWhatsThis::remove( TQWidget * widget )
{
    TQWhatsThisPrivate::setUpWhatsThis();
    TQWhatsThisPrivate::WhatsThisItem * i = wt->dict->find( (void *)widget );
    if ( !i )
	return;

    wt->dict->take( (void *)widget );

    i->deref();
    if ( !i->count )
	delete i;
}


/*!
    Returns the what's this text for widget \a w or TQString::null if
    there is no "What's this?" help for the widget. \a pos contains
    the mouse position; this is useful, for example, if you've
    subclassed to make the text that is displayed position dependent.

    If \a includeParents is true, parent widgets are taken into
    consideration as well when looking for what's this help text.

    \sa add()
*/
TQString TQWhatsThis::textFor( TQWidget * w, const TQPoint& pos, bool includeParents )
{
    TQWhatsThisPrivate::setUpWhatsThis();
    TQWhatsThisPrivate::WhatsThisItem * i = 0;
    TQPoint p = pos;
    while( w && !i ) {
	i = wt->dict->find( w );
	if ( !includeParents )
	    break;
	if ( !i ) {
	    p += w->pos();
	    w = w->parentWidget( true );
	}
    }
    if (!i)
	return TQString::null;
    if ( i->whatsthis )
	return i->whatsthis->text( p );
    return i->s;
}


/*!
    Creates a TQToolButton preconfigured to enter "What's this?" mode
    when clicked. You will often use this with a tool bar as \a
    parent:
    \code
	(void) TQWhatsThis::whatsThisButton( my_help_tool_bar );
    \endcode
*/
TQToolButton * TQWhatsThis::whatsThisButton( TQWidget * parent )
{
    TQWhatsThisPrivate::setUpWhatsThis();
    return new TQWhatsThisButton( parent,
				 "automatic what's this? button" );
}

/*!
    Constructs a dynamic "What's this?" object for \a widget. The
    object is deleted when the \a widget is destroyed.

    When the widget is queried by the user the text() function of this
    TQWhatsThis will be called to provide the appropriate text, rather
    than using the text assigned by add().
*/
TQWhatsThis::TQWhatsThis( TQWidget * widget)
{
    TQWhatsThisPrivate::setUpWhatsThis();
    wt->add(widget,this);
}


/*!
    Destroys the object and frees any allocated resources.
*/
TQWhatsThis::~TQWhatsThis()
{
}


/*!
    This virtual function returns the text for position \e p in the
    widget that this "What's this?" object documents. If there is no
    "What's this?" text for the position, TQString::null is returned.

    The default implementation returns TQString::null.
*/
TQString TQWhatsThis::text( const TQPoint & )
{
    return TQString::null;
}

/*!
    \fn bool TQWhatsThis::clicked( const TQString& href )

    This virtual function is called when the user clicks inside the
    "What's this?" window. \a href is the link the user clicked on, or
    TQString::null if there was no link.

    If the function returns true (the default), the "What's this?"
    window is closed, otherwise it remains visible.

    The default implementation ignores \a href and returns true.
*/
bool TQWhatsThis::clicked( const TQString& )
{
    return true;
}


/*!
    Enters "What's this?" mode and returns immediately.

    TQt will install a special cursor and take over mouse input until
    the user clicks somewhere. It then shows any help available and
    ends "What's this?" mode. Finally, TQt removes the special cursor
    and help window and then restores ordinary event processing, at
    which point the left mouse button is no longer pressed.

    The user can also use the Esc key to leave "What's this?" mode.

    \sa inWhatsThisMode(), leaveWhatsThisMode()
*/

void TQWhatsThis::enterWhatsThisMode()
{
    TQWhatsThisPrivate::setUpWhatsThis();
    if ( wt->state == TQWhatsThisPrivate::Inactive ) {
	wt->enterWhatsThisMode();
#ifndef TQT_NO_CURSOR
	TQApplication::setOverrideCursor( whatsThisCursor, false );
#endif
	wt->state = TQWhatsThisPrivate::Waiting;
	tqApp->installEventFilter( wt );
    }
}


/*!
    Returns true if the application is in "What's this?" mode;
    otherwise returns false.

    \sa enterWhatsThisMode(), leaveWhatsThisMode()
*/
bool TQWhatsThis::inWhatsThisMode()
{
    if (!wt)
	return false;
    return wt->state == TQWhatsThisPrivate::Waiting;
}


/*!
    Leaves "What's this?" question mode.

    This function is used internally by widgets that support
    TQWidget::customWhatsThis(); applications do not usually call it.
    An example of such a widget is TQPopupMenu: menus still work
    normally in "What's this?" mode but also provide help texts for
    individual menu items.

    If \a text is not TQString::null, a "What's this?" help window is
    displayed at the global screen position \a pos. If widget \a w is
    not 0 and has its own dedicated TQWhatsThis object, this object
    will receive clicked() messages when the user clicks on hyperlinks
    inside the help text.

    \sa inWhatsThisMode(), enterWhatsThisMode(), TQWhatsThis::clicked()
*/
void TQWhatsThis::leaveWhatsThisMode( const TQString& text, const TQPoint& pos, TQWidget* w )
{
    if ( !inWhatsThisMode() )
	return;

    wt->leaveWhatsThisMode();
    if ( !text.isNull() )
	wt->say( w, text, pos );
}

/*!
    Display \a text in a help window at the global screen position \a
    pos.

    If widget \a w is not 0 and has its own dedicated TQWhatsThis
    object, this object will receive clicked() messages when the user
    clicks on hyperlinks inside the help text.

    \sa TQWhatsThis::clicked()
*/
void TQWhatsThis::display( const TQString& text, const TQPoint& pos, TQWidget* w )
{
    if ( inWhatsThisMode() ) {
	leaveWhatsThisMode( text, pos, w );
	return;
    }
    TQWhatsThisPrivate::setUpWhatsThis();
    wt->say( w, text, pos );
}

/*!
  Sets the font for all "What's this?" helps to \a font.
*/
void TQWhatsThis::setFont( const TQFont &font )
{
    TQApplication::setFont( font, true, "TQWhatsThat" );
}

#include "qwhatsthis.moc"
#endif
