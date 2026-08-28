/****************************************************************************
**
** Implementation of TQAccel class
**
** Created : 950419
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

#include "ntqaccel.h"

#ifndef TQT_NO_ACCEL

#include "ntqsignal.h"
#include "ntqapplication.h"
#include "ntqwidget.h"
#include "ntqptrlist.h"
#include "ntqwhatsthis.h"
#include "ntqguardedptr.h"
#include "ntqstatusbar.h"
#include "ntqdockwindow.h"
#include "ntqsignalslotimp.h"
/*!
    \class TQAccel ntqaccel.h
    \brief The TQAccel class handles keyboard accelerator and shortcut keys.

    \ingroup misc

    A keyboard accelerator triggers an action when a certain key
    combination is pressed. The accelerator handles all keyboard
    activity for all the children of one top-level widget, so it is
    not affected by the keyboard focus.

    In most cases, you will not need to use this class directly. Use
    the TQAction class to create actions with accelerators that can be
    used in both menus and toolbars. If you're only interested in
    menus use TQMenuData::insertItem() or TQMenuData::setAccel() to make
    accelerators for operations that are also available on menus. Many
    widgets automatically generate accelerators, such as TQButton,
    TQGroupBox, TQLabel (with TQLabel::setBuddy()), TQMenuBar and TQTabBar.
    Example:
    \code
	TQPushButton p( "&Exit", parent ); // automatic shortcut ALT+Key_E
	TQPopupMenu *fileMenu = new fileMenu( parent );
	fileMenu->insertItem( "Undo", parent, TQ_SLOT(undo()), CTRL+Key_Z );
    \endcode

    A TQAccel contains a list of accelerator items that can be
    manipulated using insertItem(), removeItem(), clear(), key() and
    findKey().

    Each accelerator item consists of an identifier and a \l
    TQKeySequence. A single key sequence consists of a keyboard code
    combined with modifiers (\c SHIFT, \c CTRL, \c ALT or \c
    UNICODE_ACCEL). For example, \c{CTRL + Key_P} could be a shortcut
    for printing a document. The key codes are listed in \c
    ntqnamespace.h. As an alternative, use \c UNICODE_ACCEL with the
    unicode code point of the character. For example, \c{UNICODE_ACCEL
    + 'A'} gives the same accelerator as \c Key_A.

    When an accelerator key is pressed, the accelerator sends out the
    signal activated() with a number that identifies this particular
    accelerator item. Accelerator items can also be individually
    connected, so that two different keys will activate two different
    slots (see connectItem() and disconnectItem()).

    The activated() signal is \e not emitted when two or more
    accelerators match the same key.  Instead, the first matching
    accelerator sends out the activatedAmbiguously() signal. By
    pressing the key multiple times, users can navigate between all
    matching accelerators. Some standard controls like TQPushButton and
    TQCheckBox connect the activatedAmbiguously() signal to the
    harmless setFocus() slot, whereas activated() is connected to a
    slot invoking the button's action.	Most controls, like TQLabel and
    TQTabBar, treat activated() and activatedAmbiguously() as
    equivalent.

    Use setEnabled() to enable or disable all the items in an
    accelerator, or setItemEnabled() to enable or disable individual
    items. An item is active only when both the TQAccel and the item
    itself are enabled.

    The function setWhatsThis() specifies a help text that appears
    when the user presses an accelerator key in What's This mode.

    The accelerator will be deleted when \e parent is deleted,
    and will consume relevant key events until then.

    Please note that the accelerator
    \code
	accelerator->insertItem( TQKeySequence("M") );
    \endcode
    can be triggered with both the 'M' key, and with Shift+M,
    unless a second accelerator is defined for the Shift+M
    combination.


    Example:
    \code
	TQAccel *a = new TQAccel( myWindow );	   // create accels for myWindow
	a->connectItem( a->insertItem(Key_P+CTRL), // adds Ctrl+P accelerator
			myWindow,		   // connected to myWindow's
			TQ_SLOT(printDoc()) );	   // printDoc() slot
    \endcode

    \sa TQKeyEvent TQWidget::keyPressEvent() TQMenuData::setAccel()
    TQButton::setAccel() TQLabel::setBuddy() TQKeySequence
    \link guibooks.html#fowler GUI Design Handbook: Keyboard Shortcuts \endlink.
*/


struct TQAccelItem {				// internal accelerator item
    TQAccelItem( const TQKeySequence &k, int i )
	{ key=k; id=i; enabled=true; signal=0; }
   ~TQAccelItem()	       { delete signal; }
    int		    id;
    TQKeySequence    key;
    bool	    enabled;
    TQSignal	   *signal;
    TQString	    whatsthis;
};


typedef TQPtrList<TQAccelItem> TQAccelList; // internal accelerator list

class TQAccelPrivate : public TQt {
public:
    TQAccelPrivate( TQAccel* p );
    ~TQAccelPrivate();
    TQAccelList aitems;
    bool enabled;
    TQGuardedPtr<TQWidget> watch;
    bool ignorewhatsthis;
    TQAccel* parent;

    void activate( TQAccelItem* item );
    void activateAmbiguously( TQAccelItem* item );
};

class TQAccelManager : public TQt {
public:
    static TQAccelManager* self() { return self_ptr ? self_ptr : new TQAccelManager; }
    void registerAccel( TQAccelPrivate* a ) { accels.append( a ); }
    void unregisterAccel( TQAccelPrivate* a ) { accels.removeRef( a ); if ( accels.isEmpty() ) delete this; }
    bool tryAccelEvent( TQWidget* w, TQKeyEvent* e );
    bool dispatchAccelEvent( TQWidget* w, TQKeyEvent* e );
    bool tryComposeUnicode( TQWidget* w, TQKeyEvent* e );

private:
    TQAccelManager():currentState(TQt::NoMatch), clash(-1) { self_ptr = this; }
    ~TQAccelManager() { self_ptr = 0; }

    bool correctSubWindow( TQWidget *w, TQAccelPrivate* d );
    SequenceMatch match( TQKeyEvent* e, TQAccelItem* item, TQKeySequence& temp );
    int translateModifiers( ButtonState state );

    TQPtrList<TQAccelPrivate> accels;
    static TQAccelManager* self_ptr;
    TQt::SequenceMatch currentState;
    TQKeySequence intermediate;
    int clash;
};
TQAccelManager* TQAccelManager::self_ptr = 0;

bool TQ_EXPORT tqt_tryAccelEvent( TQWidget* w, TQKeyEvent*  e){
    return TQAccelManager::self()->tryAccelEvent( w, e );
}

bool TQ_EXPORT tqt_dispatchAccelEvent( TQWidget* w, TQKeyEvent*  e){
    return TQAccelManager::self()->dispatchAccelEvent( w, e );
}

bool TQ_EXPORT tqt_tryComposeUnicode( TQWidget* w, TQKeyEvent*  e){
    return TQAccelManager::self()->tryComposeUnicode( w, e );
}

#ifdef TQ_WS_MAC
static bool tqt_accel_no_shortcuts = true;
#else
static bool tqt_accel_no_shortcuts = false;
#endif
void TQ_EXPORT tqt_setAccelAutoShortcuts(bool b) { tqt_accel_no_shortcuts = b; }

/*
    \internal
    Returns true if the accel is in the current subwindow, else false.
*/
bool TQAccelManager::correctSubWindow( TQWidget* w, TQAccelPrivate* d ) {
#if !defined ( Q_OS_MACX )
     if ( !d->watch || !d->watch->isVisible() || !d->watch->isEnabled() )
#else
    if ( !d->watch || (!d->watch->isVisible() && !d->watch->inherits( "TQMenuBar" )) || !d->watch->isEnabled() )
#endif
	return false;
    TQWidget* tlw = w->topLevelWidget();
    TQWidget* wtlw = d->watch->topLevelWidget();

    /* if we live in a floating dock window, keep our parent's
     * accelerators working */
#ifndef TQT_NO_MAINWINDOW
    if ( tlw->isDialog() && tlw->parentWidget() && ::tqt_cast<TQDockWindow*>(tlw) )
	return tlw->parentWidget()->topLevelWidget() == wtlw;

    if ( wtlw  != tlw )
	return false;
#endif
    /* if we live in a MDI subwindow, ignore the event if we are
       not the active document window */
    TQWidget* sw = d->watch;
    while ( sw && !sw->testWFlags( WSubWindow ) )
	sw = sw->parentWidget( true );
    if ( sw )  { // we are in a subwindow indeed
	TQWidget* fw = w;
	while ( fw && fw != sw )
	    fw = fw->parentWidget( true );
	if ( fw != sw ) // focus widget not in our subwindow
	    return false;
    }
    return true;
}

inline int TQAccelManager::translateModifiers( ButtonState state )
{
    int result = 0;
    if ( state & ShiftButton )
	result |= SHIFT;
    if ( state & ControlButton )
	result |= CTRL;
    if ( state & MetaButton )
	result |= META;
    if ( state & AltButton )
	result |= ALT;
    return result;
}

/*
    \internal
    Matches the current intermediate key sequence + the latest
    keyevent, with and AccelItem. Returns Identical,
    PartialMatch or NoMatch, and fills \a temp with the
    resulting key sequence.
*/
TQt::SequenceMatch TQAccelManager::match( TQKeyEvent *e, TQAccelItem* item, TQKeySequence& temp )
{
    SequenceMatch result = TQt::NoMatch;
    int index = intermediate.count();
    temp = intermediate;

    int modifier = translateModifiers( e->state() );

    if ( e->key() && e->key() != Key_unknown) {
	int key = e->key()  | modifier;
	if ( e->key() == Key_BackTab ) {
	    /*
	    In TQApplication, we map shift+tab to shift+backtab.
	    This code here reverts the mapping in a way that keeps
	    backtab and shift+tab accelerators working, in that
	    order, meaning backtab has priority.*/
	    key &= ~SHIFT;

	    temp.setKey( key, index );
	    if ( TQt::NoMatch != (result = temp.matches( item->key )) )
		return result;
	    if ( e->state() & ShiftButton )
		key |= SHIFT;
	    key = Key_Tab | ( key & MODIFIER_MASK );
	    temp.setKey( key, index );
	    if ( TQt::NoMatch != (result = temp.matches( item->key )) )
		return result;
	} else {
	    temp.setKey( key, index );
	    if ( TQt::NoMatch != (result = temp.matches( item->key )) )
		return result;
	}

	if ( key == Key_BackTab ) {
	    if ( e->state() & ShiftButton )
		key |= SHIFT;
	    temp.setKey( key, index );
	    if ( TQt::NoMatch != (result = temp.matches( item->key )) )
		return result;
	}
    }
    if ( !e->text().isEmpty() ) {
	temp.setKey( (int)e->text()[0].unicode() | UNICODE_ACCEL | modifier, index );
	result = temp.matches( item->key );
    }
    return result;
}

bool TQAccelManager::tryAccelEvent( TQWidget* w, TQKeyEvent* e )
{
    if ( TQt::NoMatch == currentState ) {
	e->t = TQEvent::AccelOverride;
	e->ignore();
	TQApplication::sendSpontaneousEvent( w, e );
	if ( e->isAccepted() )
	    return false;
    }
    e->t = TQEvent::Accel;
    e->ignore();
    TQApplication::sendSpontaneousEvent( w, e );
    return e->isAccepted();
}

bool TQAccelManager::tryComposeUnicode( TQWidget* w, TQKeyEvent* e )
{
    if ( TQApplication::metaComposeUnicode ) {
	int value = e->key() - Key_0;
	// Ignore acceloverrides so we don't trigger
	// accels on keypad when Meta compose is on
	if ( (e->type() == TQEvent::AccelOverride) &&
	     (e->state() == TQt::Keypad + TQt::MetaButton) ) {
	    e->accept();
	// Meta compose start/continue
	} else if ( (e->type() == TQEvent::KeyPress) &&
	     (e->state() == TQt::Keypad + TQt::MetaButton) ) {
	    if ( value >= 0 && value <= 9 ) {
		TQApplication::composedUnicode *= 10;
		TQApplication::composedUnicode += value;
		return true;
	    } else {
		// Composing interrupted, dispatch!
		if ( TQApplication::composedUnicode ) {
		    TQChar ch( TQApplication::composedUnicode );
		    TQString s( ch );
		    TQKeyEvent kep( TQEvent::KeyPress, 0, ch.row() ? 0 : ch.cell(), 0, s );
		    TQKeyEvent ker( TQEvent::KeyRelease, 0, ch.row() ? 0 : ch.cell(), 0, s );
		    TQApplication::sendEvent( w, &kep );
		    TQApplication::sendEvent( w, &ker );
		}
		TQApplication::composedUnicode = 0;
		return true;
	    }
	// Meta compose end, dispatch
	} else if ( (e->type() == TQEvent::KeyRelease) &&
		    (e->key() == Key_Meta) &&
		    (TQApplication::composedUnicode != 0) ) {
	    if ( (TQApplication::composedUnicode > 0) && 
		 (TQApplication::composedUnicode < 0xFFFE) ) {
		TQChar ch( TQApplication::composedUnicode );
		TQString s( ch );
		TQKeyEvent kep( TQEvent::KeyPress, 0, ch.row() ? 0 : ch.cell(), 0, s );
		TQKeyEvent ker( TQEvent::KeyRelease, 0, ch.row() ? 0 : ch.cell(), 0, s );
		TQApplication::sendEvent( w, &kep );
		TQApplication::sendEvent( w, &ker );
	    }
	    TQApplication::composedUnicode = 0;
	    return true;
	}
    }
    return false;
}

/*
    \internal
    Checks for possible accelerators, if no widget
    ate the keypres, or we are in the middle of a
    partial key sequence.
*/
bool TQAccelManager::dispatchAccelEvent( TQWidget* w, TQKeyEvent* e )
{
#ifndef TQT_NO_STATUSBAR
    // Needs to be declared and used here because of "goto doclash"
    TQStatusBar* mainStatusBar = 0;
#endif

    // Modifiers can NOT be accelerators...
    if ( e->key() >= Key_Shift &&
	 e->key() <= Key_Alt )
	 return false;

    SequenceMatch result = TQt::NoMatch;
    TQKeySequence tocheck, partial;
    TQAccelPrivate* accel = 0;
    TQAccelItem* item = 0;
    TQAccelPrivate* firstaccel = 0;
    TQAccelItem* firstitem = 0;
    TQAccelPrivate* lastaccel = 0;
    TQAccelItem* lastitem = 0;
    
    TQKeyEvent pe = *e;
    int n = -1;
    int hasShift = (e->state()&TQt::ShiftButton)?1:0;
    bool identicalDisabled = false;
    bool matchFound = false;
    do {
	accel = accels.first();
	matchFound = false;
	while ( accel ) {
	    if ( correctSubWindow( w, accel ) ) {
		if ( accel->enabled ) {
		    item = accel->aitems.last();
		    while( item ) {
			if ( TQt::Identical == (result = match( &pe, item, tocheck )) ) {
			    if ( item->enabled ) {
				if ( !firstaccel ) {
				    firstaccel = accel;
				    firstitem = item;
				}
				lastaccel = accel;
				lastitem = item;
				n++;
				matchFound = true;
				if ( n > TQMAX(clash,0) )
				    goto doclash;
			    } else {
				identicalDisabled = true;
			    }
			}
			if ( item->enabled && TQt::PartialMatch == result ) {
			    partial = tocheck;
			    matchFound = true;
			}
			item = accel->aitems.prev();
		    }
		} else {
		    item = accel->aitems.last();
		    while( item ) {
			if ( TQt::Identical == match( &pe, item, tocheck ) )
			    identicalDisabled = true;
			item = accel->aitems.prev();
		    }
		}
	    }
	    accel = accels.next();
	}
	pe = TQKeyEvent( TQEvent::Accel, pe.key(), pe.ascii(), pe.state()&~TQt::ShiftButton, pe.text() );
    } while ( hasShift-- && !matchFound && !identicalDisabled );

#ifndef TQT_NO_STATUSBAR
    mainStatusBar = (TQStatusBar*) w->topLevelWidget()->child( 0, "TQStatusBar" );
#endif
    if ( n < 0 ) { // no match found
	currentState = partial.count() ? PartialMatch : NoMatch;
#ifndef TQT_NO_STATUSBAR
	// Only display message if we are, or were, in a partial match
	if ( mainStatusBar && (PartialMatch == currentState || intermediate.count() ) ) {
	    if ( currentState == TQt::PartialMatch ) {
		mainStatusBar->message( (TQString)partial + ", ...", 0 );
	    } else if (!identicalDisabled) {
		TQString message = TQAccel::tr("%1, %2 not defined").
		    arg( (TQString)intermediate ).
		    arg( TQKeySequence::encodeString( e->key() | translateModifiers(e->state()) ) );
		mainStatusBar->message( message, 2000 );
		// Since we're a NoMatch, reset the clash count
		clash = -1;
	    } else {
	    	mainStatusBar->clear();
	    }
	}
#endif

	bool eatKey = (PartialMatch == currentState || intermediate.count() );
	intermediate = partial;
	if ( eatKey )
	    e->accept();
	return eatKey;
    } else if ( n == 0 ) { // found exactly one match
	clash = -1; // reset
#ifndef TQT_NO_STATUSBAR
	if ( currentState == TQt::PartialMatch && mainStatusBar )
		mainStatusBar->clear();
#endif
	currentState = TQt::NoMatch; // Free sequence keylock
	intermediate = TQKeySequence();
	lastaccel->activate( lastitem );
	e->accept();
	return true;
    }

 doclash: // found more than one match
#ifndef TQT_NO_STATUSBAR
    if ( !mainStatusBar ) // if "goto doclash", we need to get statusbar again.
	mainStatusBar = (TQStatusBar*) w->topLevelWidget()->child( 0, "TQStatusBar" );
#endif

    TQString message = TQAccel::tr( "Ambiguous \"%1\" not handled" ).arg( (TQString)tocheck );
    if ( clash >= 0 && n > clash ) { // pick next  match
	intermediate = TQKeySequence();
	currentState = TQt::NoMatch; // Free sequence keylock
	clash++;
#ifndef TQT_NO_STATUSBAR
	if ( mainStatusBar &&
	     !lastitem->signal &&
	     !(lastaccel->parent->receivers( "activatedAmbiguously(int)" )) )
	    mainStatusBar->message( message, 2000 );
#endif
	lastaccel->activateAmbiguously( lastitem );
    } else { // start (or wrap) with the first matching
	intermediate = TQKeySequence();
	currentState = TQt::NoMatch; // Free sequence keylock
	clash = 0;
#ifndef TQT_NO_STATUSBAR
	if ( mainStatusBar &&
	     !firstitem->signal &&
	     !(firstaccel->parent->receivers( "activatedAmbiguously(int)" )) )
	    mainStatusBar->message( message, 2000 );
#endif
	firstaccel->activateAmbiguously( firstitem );
    }
    e->accept();
    return true;
}

TQAccelPrivate::TQAccelPrivate( TQAccel* p )
    : parent( p )
{
    TQAccelManager::self()->registerAccel( this );
    aitems.setAutoDelete( true );
    ignorewhatsthis = false;
}

TQAccelPrivate::~TQAccelPrivate()
{
    TQAccelManager::self()->unregisterAccel( this );
}

static TQAccelItem *find_id( TQAccelList &list, int id )
{
    TQAccelItem *item = list.first();
    while ( item && item->id != id )
	item = list.next();
    return item;
}

static TQAccelItem *find_key( TQAccelList &list, const TQKeySequence &key )
{
    TQAccelItem *item = list.first();
    while ( item && !( item->key == key ) )
	item = list.next();
    return item;
}

/*!
    Constructs a TQAccel object called \a name, with parent \a parent.
    The accelerator operates on \a parent.
*/

TQAccel::TQAccel( TQWidget *parent, const char *name )
    : TQObject( parent, name )
{
    d = new TQAccelPrivate( this );
    d->enabled = true;
    d->watch = parent;
#if defined(QT_CHECK_NULL)
    if ( !d->watch )
	tqWarning( "TQAccel: An accelerator must have a parent or a watch widget" );
#endif
}

/*!
    Constructs a TQAccel object called \a name, that operates on \a
    watch, and is a child of \a parent.

    This constructor is not needed for normal application programming.
*/
TQAccel::TQAccel( TQWidget* watch, TQObject *parent, const char *name )
    : TQObject( parent, name )
{
    d = new TQAccelPrivate( this );
    d->enabled = true;
    d->watch = watch;
#if defined(QT_CHECK_NULL)
    if ( !d->watch )
	tqWarning( "TQAccel: An accelerator must have a parent or a watch widget" );
#endif
}

/*!
    Destroys the accelerator object and frees all allocated resources.
*/

TQAccel::~TQAccel()
{
    delete d;
}


/*!
    \fn void TQAccel::activated( int id )

    This signal is emitted when an accelerator key is pressed. \a id
    is a number that identifies this particular accelerator item.

    \sa activatedAmbiguously()
*/

/*!
    \fn void TQAccel::activatedAmbiguously( int id )

    This signal is emitted when an accelerator key is pressed. \a id
    is a number that identifies this particular accelerator item.

    \sa activated()
*/


/*!
    Returns true if the accelerator is enabled; otherwise returns
    false.

    \sa setEnabled(), isItemEnabled()
*/

bool TQAccel::isEnabled() const
{
    return d->enabled;
}


/*!
    Enables the accelerator if \a enable is true, or disables it if \a
    enable is false.

    Individual keys can also be enabled or disabled using
    setItemEnabled(). To work, a key must be an enabled item in an
    enabled TQAccel.

    \sa isEnabled(), setItemEnabled()
*/

void TQAccel::setEnabled( bool enable )
{
    d->enabled = enable;
}


/*!
    Returns the number of accelerator items in this accelerator.
*/

uint TQAccel::count() const
{
    return d->aitems.count();
}


static int get_seq_id()
{
    static int seq_no = -2;  // -1 is used as return value in findKey()
    return seq_no--;
}

/*!
    Inserts an accelerator item and returns the item's identifier.

    \a key is a key code and an optional combination of SHIFT, CTRL
    and ALT. \a id is the accelerator item id.

    If \a id is negative, then the item will be assigned a unique
    negative identifier less than -1.

    \code
	TQAccel *a = new TQAccel( myWindow );	   // create accels for myWindow
	a->insertItem( CTRL + Key_P, 200 );	   // Ctrl+P, e.g. to print document
	a->insertItem( ALT + Key_X, 201 );	   // Alt+X, e.g. to quit
	a->insertItem( UNICODE_ACCEL + 'q', 202 ); // Unicode 'q', e.g. to quit
	a->insertItem( Key_D );			   // gets a unique negative id < -1
	a->insertItem( CTRL + SHIFT + Key_P );	   // gets a unique negative id < -1
    \endcode
*/

int TQAccel::insertItem( const TQKeySequence& key, int id )
{
    if ( id == -1 )
	id = get_seq_id();
    d->aitems.insert( 0, new TQAccelItem(key,id) );
    return id;
}

/*!
    Removes the accelerator item with the identifier \a id.
*/

void TQAccel::removeItem( int id )
{
    if ( find_id( d->aitems, id) )
	d->aitems.remove();
}


/*!
    Removes all accelerator items.
*/

void TQAccel::clear()
{
    d->aitems.clear();
}


/*!
    Returns the key sequence of the accelerator item with identifier
    \a id, or an invalid key sequence (0) if the id cannot be found.
*/

TQKeySequence TQAccel::key( int id )
{
    TQAccelItem *item = find_id( d->aitems, id);
    return item ? item->key : TQKeySequence( 0 );
}


/*!
    Returns the identifier of the accelerator item with the key code
    \a key, or -1 if the item cannot be found.
*/

int TQAccel::findKey( const TQKeySequence& key ) const
{
    TQAccelItem *item = find_key( d->aitems, key );
    return item ? item->id : -1;
}


/*!
    Returns true if the accelerator item with the identifier \a id is
    enabled. Returns false if the item is disabled or cannot be found.

    \sa setItemEnabled(), isEnabled()
*/

bool TQAccel::isItemEnabled( int id ) const
{
    TQAccelItem *item = find_id( d->aitems, id);
    return item ? item->enabled : false;
}


/*!
    Enables the accelerator item with the identifier \a id if \a
    enable is true, and disables item \a id if \a enable is false.

    To work, an item must be enabled and be in an enabled TQAccel.

    \sa isItemEnabled(), isEnabled()
*/

void TQAccel::setItemEnabled( int id, bool enable )
{
    TQAccelItem *item = find_id( d->aitems, id);
    if ( item )
	item->enabled = enable;
}


/*!
    Connects the accelerator item \a id to the slot \a member of \a
    receiver.

    \code
	a->connectItem( 201, mainView, TQ_SLOT(quit()) );
    \endcode

    Of course, you can also send a signal as \a member.

    Normally accelerators are connected to slots which then receive
    the \c activated(int id) signal with the id of the accelerator
    item that was activated. If you choose to connect a specific
    accelerator item using this function, the \c activated() signal is
    emitted if the associated key sequence is pressed but no \c
    activated(int id) signal is emitted.

    \sa disconnectItem()
*/

bool TQAccel::connectItem( int id, const TQObject *receiver, const char *member )
{
    TQAccelItem *item = find_id( d->aitems, id);
    if ( item ) {
	if ( !item->signal ) {
	    item->signal = new TQSignal;
	    TQ_CHECK_PTR( item->signal );
	}
	return item->signal->connect( receiver, member );
    }
    return false;
}

/*!
    Disconnects an accelerator item with id \a id from the function
    called \a member in the \a receiver object.

    \sa connectItem()
*/

bool TQAccel::disconnectItem( int id, const TQObject *receiver,
			     const char *member )
{
    TQAccelItem *item = find_id( d->aitems, id);
    if ( item && item->signal )
	return item->signal->disconnect( receiver, member );
    return false;
}

void TQAccelPrivate::activate( TQAccelItem* item )
{
#ifndef TQT_NO_WHATSTHIS
    if ( TQWhatsThis::inWhatsThisMode() && !ignorewhatsthis ) {
	TQWhatsThis::leaveWhatsThisMode( item->whatsthis );
	return;
    }
#endif
    if ( item->signal )
	item->signal->activate();
    else
	emit parent->activated( item->id );
}

void TQAccelPrivate::activateAmbiguously( TQAccelItem* item )
{
    if ( item->signal )
	item->signal->activate();
    else
	emit parent->activatedAmbiguously( item->id );
}


/*!
    Returns the shortcut key sequence for \a str, or an invalid key
    sequence (0) if \a str has no shortcut sequence.

    For example, shortcutKey("E&amp;xit") returns ALT+Key_X,
    shortcutKey("&amp;Quit") returns ALT+Key_Q and shortcutKey("Quit")
    returns 0. (In code that does not inherit the TQt namespace class,
    you must write e.g. TQt::ALT+TQt::Key_Q.)

    We provide a \link accelerators.html list of common accelerators
    \endlink in English. At the time of writing, Microsoft and Open
    Group do not appear to have issued equivalent recommendations for
    other languages.
*/

TQKeySequence TQAccel::shortcutKey( const TQString &str )
{
    if(tqt_accel_no_shortcuts)
	return TQKeySequence();

    int p = 0;
    while ( p >= 0 ) {
	p = str.find( '&', p ) + 1;
	if ( p <= 0 || p >= (int)str.length() )
	    return 0;
	if ( str[p] != '&' ) {
	    TQChar c = str[p];
	    if ( c.isPrint() ) {
	        char ltr = c.upper().latin1();
	        if ( ltr >= (char)Key_A && ltr <= (char)Key_Z )
                    c = ltr;
                else
                    c = c.lower();
		return TQKeySequence( c.unicode() + ALT + UNICODE_ACCEL );
	    }
	}
	p++;
    }
    return TQKeySequence();
}

/*! \obsolete

   Creates an accelerator string for the key \a k.
   For instance CTRL+Key_O gives "Ctrl+O". The "Ctrl" etc.
   are translated (using TQObject::tr()) in the "TQAccel" context.

   The function is superfluous. Cast the TQKeySequence \a k to a
   TQString for the same effect.
*/
TQString TQAccel::keyToString( TQKeySequence k )
{
    return (TQString) k;
}

/*!\obsolete

  Returns an accelerator code for the string \a s. For example
  "Ctrl+O" gives CTRL+UNICODE_ACCEL+'O'. The strings "Ctrl",
  "Shift", "Alt" are recognized, as well as their translated
  equivalents in the "TQAccel" context (using TQObject::tr()). Returns 0
  if \a s is not recognized.

  This function is typically used with \link TQObject::tr() tr
  \endlink(), so that accelerator keys can be replaced in
  translations:

  \code
    TQPopupMenu *file = new TQPopupMenu( this );
    file->insertItem( p1, tr("&Open..."), this, TQ_SLOT(open()),
		      TQAccel::stringToKey(tr("Ctrl+O", "File|Open")) );
  \endcode

  Notice the \c "File|Open" translator comment. It is by no means
  necessary, but it provides some context for the human translator.


  The function is superfluous. Construct a TQKeySequence from the
  string \a s for the same effect.

  \sa TQObject::tr()
      \link i18n.html Internationalization with TQt \endlink
*/
TQKeySequence TQAccel::stringToKey( const TQString & s )
{
    return TQKeySequence( s );
}


/*!
    Sets a What's This help text for the accelerator item \a id to \a
    text.

    The text will be shown when the application is in What's This mode
    and the user hits the accelerator key.

    To set What's This help on a menu item (with or without an
    accelerator key), use TQMenuData::setWhatsThis().

    \sa whatsThis(), TQWhatsThis::inWhatsThisMode(),
    TQMenuData::setWhatsThis(), TQAction::setWhatsThis()
*/
void TQAccel::setWhatsThis( int id, const TQString& text )
{

    TQAccelItem *item = find_id( d->aitems, id);
    if ( item )
	item->whatsthis = text;
}

/*!
    Returns the What's This help text for the specified item \a id or
    TQString::null if no text has been specified.

    \sa setWhatsThis()
*/
TQString TQAccel::whatsThis( int id ) const
{

    TQAccelItem *item = find_id( d->aitems, id);
    return item? item->whatsthis : TQString::null;
}

/*!\internal */
void TQAccel::setIgnoreWhatsThis( bool b)
{
    d->ignorewhatsthis = b;
}

/*!\internal */
bool TQAccel::ignoreWhatsThis() const
{
    return d->ignorewhatsthis;
}


/*!

\page accelerators.html

\title Standard Accelerator Keys

Applications invariably need to define accelerator keys for actions.
TQt fully supports accelerators, for example with \l TQAccel::shortcutKey().

Here are Microsoft's recommendations for accelerator keys, with
comments about the Open Group's recommendations where they exist
and differ. For most commands, the Open Group either has no advice or
agrees with Microsoft.

The emboldened letter plus Alt is Microsoft's recommended choice, and
we recommend supporting it. For an Apply button, for example, we
recommend TQButton::setText( \link TQWidget::tr() tr \endlink("&amp;Apply") );

If you have conflicting commands (e.g. About and Apply buttons in the
same dialog), you must decide for yourself.

\list
\i <b><u>A</u></b>bout
\i Always on <b><u>T</u></b>op
\i <b><u>A</u></b>pply
\i <b><u>B</u></b>ack
\i <b><u>B</u></b>rowse
\i <b><u>C</u></b>lose (CDE: Alt+F4; Alt+F4 is "close window" in Windows)
\i <b><u>C</u></b>opy (CDE: Ctrl+C, Ctrl+Insert)
\i <b><u>C</u></b>opy Here
\i Create <b><u>S</u></b>hortcut
\i Create <b><u>S</u></b>hortcut Here
\i Cu<b><u>t</u></b>
\i <b><u>D</u></b>elete
\i <b><u>E</u></b>dit
\i <b><u>E</u></b>xit (CDE: E<b><u>x</u></b>it)
\i <b><u>E</u></b>xplore
\i <b><u>F</u></b>ile
\i <b><u>F</u></b>ind
\i <b><u>H</u></b>elp
\i Help <b><u>T</u></b>opics
\i <b><u>H</u></b>ide
\i <b><u>I</u></b>nsert
\i Insert <b><u>O</u></b>bject
\i <b><u>L</u></b>ink Here
\i Ma<b><u>x</u></b>imize
\i Mi<b><u>n</u></b>imize
\i <b><u>M</u></b>ove
\i <b><u>M</u></b>ove Here
\i <b><u>N</u></b>ew
\i <b><u>N</u></b>ext
\i <b><u>N</u></b>o
\i <b><u>O</u></b>pen
\i Open <b><u>W</u></b>ith
\i Page Set<b><u>u</u></b>p
\i <b><u>P</u></b>aste
\i Paste <b><u>L</u></b>ink
\i Paste <b><u>S</u></b>hortcut
\i Paste <b><u>S</u></b>pecial
\i <b><u>P</u></b>ause
\i <b><u>P</u></b>lay
\i <b><u>P</u></b>rint
\i <b><u>P</u></b>rint Here
\i P<b><u>r</u></b>operties
\i <b><u>Q</u></b>uick View
\i <b><u>R</u></b>edo (CDE: Ctrl+Y, Shift+Alt+Backspace)
\i <b><u>R</u></b>epeat
\i <b><u>R</u></b>estore
\i <b><u>R</u></b>esume
\i <b><u>R</u></b>etry
\i <b><u>R</u></b>un
\i <b><u>S</u></b>ave
\i Save <b><u>A</u></b>s
\i Select <b><u>A</u></b>ll
\i Se<b><u>n</u></b>d To
\i <b><u>S</u></b>how
\i <b><u>S</u></b>ize
\i S<b><u>p</u></b>lit
\i <b><u>S</u></b>top
\i <b><u>U</u></b>ndo (CDE: Ctrl+Z or Alt+Backspace)
\i <b><u>V</u></b>iew
\i <b><u>W</u></b>hat's This?
\i <b><u>W</u></b>indow
\i <b><u>Y</u></b>es
\endlist

There are also a lot of other keys and actions (that use other
modifier keys than Alt). See the Microsoft and The Open Group
documentation for details.

The \link http://www.amazon.com/exec/obidos/ASIN/0735605661/trolltech/t
Microsoft book \endlink has ISBN 0735605661. The corresponding Open Group
book is very hard to find, rather expensive and we cannot recommend
it. However, if you really want it, OGPubs@opengroup.org might be able
to help. Ask them for ISBN 1859121047.

*/

/*! \obsolete  serves no purpose anymore */
void TQAccel::repairEventFilter() {}
/*! \obsolete	serves no purpose anymore */
bool TQAccel::eventFilter( TQObject *, TQEvent * ) { return false; }
#endif // TQT_NO_ACCEL
