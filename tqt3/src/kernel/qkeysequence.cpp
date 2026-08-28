/****************************************************************************
**
** Implementation of TQKeySequence class
**
** Created : 0108007
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

#include "ntqkeysequence.h"

#ifndef TQT_NO_ACCEL

#include "ntqaccel.h"
#include "ntqshared.h"
#include "ntqvaluelist.h"
#ifndef TQT_NO_REGEXP
# include "ntqregexp.h"
#endif

#ifdef TQ_WS_MAC
#define TQMAC_CTRL  (TQString(TQChar(0x2318)))
#define TQMAC_META  (TQString(TQChar(0x2303)))
#define TQMAC_ALT   (TQString(TQChar(0x2325)))
#define TQMAC_SHIFT (TQString(TQChar(0x21E7)))
#endif

/*!
    \class TQKeySequence ntqkeysequence.h
    \brief The TQKeySequence class encapsulates a key sequence as used
    by accelerators.

    \ingroup misc

    A key sequence consists of up to four keyboard codes, each
    optionally combined with modifiers, e.g. \c SHIFT, \c CTRL, \c
    ALT, \c META, or \c UNICODE_ACCEL. For example, \c{CTRL + Key_P}
    might be a sequence used as a shortcut for printing a document.
    The key codes are listed in \c{ntqnamespace.h}. As an alternative,
    use \c UNICODE_ACCEL with the unicode code point of the character.
    For example, \c{UNICODE_ACCEL + 'A'} gives the same key sequence
    as \c Key_A.

    Key sequences can be constructed either from an integer key code,
    or from a human readable translatable string such as
    "Ctrl+X,Alt+Space". A key sequence can be cast to a TQString to
    obtain a human readable translated version of the sequence.
    Translations are done in the "TQAccel" context.

    \sa TQAccel
*/

/*!
    \enum TQt::SequenceMatch

    \value NoMatch Sequences have nothing in common
    \value PartialMatch Sequences match partially, but are not complete
    \value Identical Sequences do not differ
*/

static struct {
    int key;
    const char* name;
} keyname[] = {
    { TQt::Key_Space,       TQT_TRANSLATE_NOOP( "TQAccel", "Space" ) },
    { TQt::Key_Escape,      TQT_TRANSLATE_NOOP( "TQAccel", "Esc" ) },
    { TQt::Key_Tab,         TQT_TRANSLATE_NOOP( "TQAccel", "Tab" ) },
    { TQt::Key_Backtab,     TQT_TRANSLATE_NOOP( "TQAccel", "Backtab" ) },
    { TQt::Key_Backspace,   TQT_TRANSLATE_NOOP( "TQAccel", "Backspace" ) },
    { TQt::Key_Return,      TQT_TRANSLATE_NOOP( "TQAccel", "Return" ) },
    { TQt::Key_Enter,       TQT_TRANSLATE_NOOP( "TQAccel", "Enter" ) },
    { TQt::Key_Insert,      TQT_TRANSLATE_NOOP( "TQAccel", "Ins" ) },
    { TQt::Key_Delete,      TQT_TRANSLATE_NOOP( "TQAccel", "Del" ) },
    { TQt::Key_Pause,       TQT_TRANSLATE_NOOP( "TQAccel", "Pause" ) },
    { TQt::Key_Print,       TQT_TRANSLATE_NOOP( "TQAccel", "Print" ) },
    { TQt::Key_SysReq,      TQT_TRANSLATE_NOOP( "TQAccel", "SysReq" ) },
    { TQt::Key_Home,        TQT_TRANSLATE_NOOP( "TQAccel", "Home" ) },
    { TQt::Key_End,         TQT_TRANSLATE_NOOP( "TQAccel", "End" ) },
    { TQt::Key_Left,        TQT_TRANSLATE_NOOP( "TQAccel", "Left" ) },
    { TQt::Key_Up,          TQT_TRANSLATE_NOOP( "TQAccel", "Up" ) },
    { TQt::Key_Right,       TQT_TRANSLATE_NOOP( "TQAccel", "Right" ) },
    { TQt::Key_Down,        TQT_TRANSLATE_NOOP( "TQAccel", "Down" ) },
    { TQt::Key_Prior,       TQT_TRANSLATE_NOOP( "TQAccel", "PgUp" ) },
    { TQt::Key_Next,        TQT_TRANSLATE_NOOP( "TQAccel", "PgDown" ) },
    { TQt::Key_CapsLock,    TQT_TRANSLATE_NOOP( "TQAccel", "CapsLock" ) },
    { TQt::Key_NumLock,     TQT_TRANSLATE_NOOP( "TQAccel", "NumLock" ) },
    { TQt::Key_ScrollLock,  TQT_TRANSLATE_NOOP( "TQAccel", "ScrollLock" ) },
    { TQt::Key_Menu,        TQT_TRANSLATE_NOOP( "TQAccel", "Menu" ) },
    { TQt::Key_Help,        TQT_TRANSLATE_NOOP( "TQAccel", "Help" ) },

    // Multimedia keys
    { TQt::Key_Back,        TQT_TRANSLATE_NOOP( "TQAccel", "Back" ) },
    { TQt::Key_Forward,     TQT_TRANSLATE_NOOP( "TQAccel", "Forward" ) },
    { TQt::Key_Stop,        TQT_TRANSLATE_NOOP( "TQAccel", "Stop" ) },
    { TQt::Key_Refresh,     TQT_TRANSLATE_NOOP( "TQAccel", "Refresh" ) },
    { TQt::Key_VolumeDown,  TQT_TRANSLATE_NOOP( "TQAccel", "Volume Down" ) },
    { TQt::Key_VolumeMute,  TQT_TRANSLATE_NOOP( "TQAccel", "Volume Mute" ) },
    { TQt::Key_VolumeUp,    TQT_TRANSLATE_NOOP( "TQAccel", "Volume Up" ) },
    { TQt::Key_BassBoost,   TQT_TRANSLATE_NOOP( "TQAccel", "Bass Boost" ) },
    { TQt::Key_BassUp,      TQT_TRANSLATE_NOOP( "TQAccel", "Bass Up" ) },
    { TQt::Key_BassDown,    TQT_TRANSLATE_NOOP( "TQAccel", "Bass Down" ) },
    { TQt::Key_TrebleUp,    TQT_TRANSLATE_NOOP( "TQAccel", "Treble Up" ) },
    { TQt::Key_TrebleDown,  TQT_TRANSLATE_NOOP( "TQAccel", "Treble Down" ) },
    { TQt::Key_MediaPlay,   TQT_TRANSLATE_NOOP( "TQAccel", "Media Play" ) },
    { TQt::Key_MediaStop,   TQT_TRANSLATE_NOOP( "TQAccel", "Media Stop" ) },
    { TQt::Key_MediaPrev,   TQT_TRANSLATE_NOOP( "TQAccel", "Media Previous" ) },
    { TQt::Key_MediaNext,   TQT_TRANSLATE_NOOP( "TQAccel", "Media Next" ) },
    { TQt::Key_MediaRecord, TQT_TRANSLATE_NOOP( "TQAccel", "Media Record" ) },
    { TQt::Key_HomePage,    TQT_TRANSLATE_NOOP( "TQAccel", "Home" ) },
    { TQt::Key_Favorites,   TQT_TRANSLATE_NOOP( "TQAccel", "Favorites" ) },
    { TQt::Key_Search,      TQT_TRANSLATE_NOOP( "TQAccel", "Search" ) },
    { TQt::Key_Standby,     TQT_TRANSLATE_NOOP( "TQAccel", "Standby" ) },
    { TQt::Key_OpenUrl,     TQT_TRANSLATE_NOOP( "TQAccel", "Open URL" ) },
    { TQt::Key_LaunchMail,  TQT_TRANSLATE_NOOP( "TQAccel", "Launch Mail" ) },
    { TQt::Key_LaunchMedia, TQT_TRANSLATE_NOOP( "TQAccel", "Launch Media" ) },
    { TQt::Key_Launch0,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (0)" ) },
    { TQt::Key_Launch1,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (1)" ) },
    { TQt::Key_Launch2,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (2)" ) },
    { TQt::Key_Launch3,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (3)" ) },
    { TQt::Key_Launch4,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (4)" ) },
    { TQt::Key_Launch5,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (5)" ) },
    { TQt::Key_Launch6,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (6)" ) },
    { TQt::Key_Launch7,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (7)" ) },
    { TQt::Key_Launch8,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (8)" ) },
    { TQt::Key_Launch9,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (9)" ) },
    { TQt::Key_LaunchA,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (A)" ) },
    { TQt::Key_LaunchB,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (B)" ) },
    { TQt::Key_LaunchC,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (C)" ) },
    { TQt::Key_LaunchD,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (D)" ) },
    { TQt::Key_LaunchE,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (E)" ) },
    { TQt::Key_LaunchF,     TQT_TRANSLATE_NOOP( "TQAccel", "Launch (F)" ) },
    { TQt::Key_MonBrightnessUp,        TQT_TRANSLATE_NOOP( "TQAccel", "Monitor Brightness Up" ) },
    { TQt::Key_MonBrightnessDown,      TQT_TRANSLATE_NOOP( "TQAccel", "Monitor Brightness Down" ) },
    { TQt::Key_KeyboardLightOnOff,     TQT_TRANSLATE_NOOP( "TQAccel", "Keyboard Light On Off" ) },
    { TQt::Key_KeyboardBrightnessUp,   TQT_TRANSLATE_NOOP( "TQAccel", "Keyboard Brightness Up" ) },
    { TQt::Key_KeyboardBrightnessDown, TQT_TRANSLATE_NOOP( "TQAccel", "Keyboard Brightness Down" ) },

    // --------------------------------------------------------------
    // More consistent namings
    { TQt::Key_Print,       TQT_TRANSLATE_NOOP( "TQAccel", "Print Screen" ) },
    { TQt::Key_Prior,       TQT_TRANSLATE_NOOP( "TQAccel", "Page Up" ) },
    { TQt::Key_Next,        TQT_TRANSLATE_NOOP( "TQAccel", "Page Down" ) },
    { TQt::Key_CapsLock,    TQT_TRANSLATE_NOOP( "TQAccel", "Caps Lock" ) },
    { TQt::Key_NumLock,     TQT_TRANSLATE_NOOP( "TQAccel", "Num Lock" ) },
    { TQt::Key_NumLock,     TQT_TRANSLATE_NOOP( "TQAccel", "Number Lock" ) },
    { TQt::Key_ScrollLock,  TQT_TRANSLATE_NOOP( "TQAccel", "Scroll Lock" ) },
    { TQt::Key_Insert,      TQT_TRANSLATE_NOOP( "TQAccel", "Insert" ) },
    { TQt::Key_Delete,      TQT_TRANSLATE_NOOP( "TQAccel", "Delete" ) },
    { TQt::Key_Escape,      TQT_TRANSLATE_NOOP( "TQAccel", "Escape" ) },
    { TQt::Key_SysReq,      TQT_TRANSLATE_NOOP( "TQAccel", "System Request" ) },

    { 0, 0 }
};


class TQKeySequencePrivate : public TQShared
{
public:
    inline TQKeySequencePrivate()
    {
	key[0] = key[1] = key[2] = key[3] =  0;
    }
    inline TQKeySequencePrivate( TQKeySequencePrivate *copy )
    {
	key[0] = copy->key[0];
	key[1] = copy->key[1];
	key[2] = copy->key[2];
	key[3] = copy->key[3];
    }
    int key[4];
};


/*!
    Constructs an empty key sequence.
*/
TQKeySequence::TQKeySequence()
{
    d = new TQKeySequencePrivate();
    TQ_CHECK_PTR( d );
}

/*!
    Creates a key sequence from the string \a key. For example
    "Ctrl+O" gives CTRL+UNICODE_ACCEL+'O'. The strings "Ctrl",
    "Shift", "Alt" and "Meta" are recognized, as well as their
    translated equivalents in the "TQAccel" context (using
    TQObject::tr()).

    Multiple key codes (up to four) may be entered by separating them
    with commas, e.g. "Alt+X,Ctrl+S,Q".

    This contructor is typically used with \link TQObject::tr() tr
    \endlink(), so that accelerator keys can be replaced in
    translations:

    \code
	TQPopupMenu *file = new TQPopupMenu( this );
	file->insertItem( tr("&Open..."), this, TQ_SLOT(open()),
			  TQKeySequence( tr("Ctrl+O", "File|Open") ) );
    \endcode

    Note the \c "File|Open" translator comment. It is by no means
    necessary, but it provides some context for the human translator.
*/
TQKeySequence::TQKeySequence( const TQString& key )
{
    d = new TQKeySequencePrivate();
    TQ_CHECK_PTR( d );
    assign( key );
}


// ### BCI: Merge with constructor below for 4.0
/*!
    Constructs a key sequence that has a single \a key.

    The key codes are listed in \c{ntqnamespace.h} and can be
    combined with modifiers, e.g. with \c SHIFT, \c CTRL, \c
    ALT, \c META or \c UNICODE_ACCEL.
*/
TQKeySequence::TQKeySequence( int key )
{
    d = new TQKeySequencePrivate();
    TQ_CHECK_PTR( d );
    d->key[0] = key;
}

/*!
    Constructs a key sequence with up to 4 keys \a k1, \a k2,
    \a k3 and \a k4.

    The key codes are listed in \c{ntqnamespace.h} and can be
    combined with modifiers, e.g. with \c SHIFT, \c CTRL, \c
    ALT, \c META or \c UNICODE_ACCEL.
*/
TQKeySequence::TQKeySequence( int k1, int k2, int k3, int k4 )
{
    d = new TQKeySequencePrivate();
    TQ_CHECK_PTR( d );
    d->key[0] = k1;
    d->key[1] = k2;
    d->key[2] = k3;
    d->key[3] = k4;
}

/*!
    Copy constructor. Makes a copy of \a keysequence.
 */
TQKeySequence::TQKeySequence( const TQKeySequence& keysequence )
    : d( keysequence.d )
{
    d->ref();
}


/*!
    Destroys the key sequence.
 */
TQKeySequence::~TQKeySequence()
{
    if ( d->deref() )
	delete d;
}

/*!
    \internal
    KeySequences should never be modified, but rather just created.
    Internally though we do need to modify to keep pace in event
    delivery.
*/

void TQKeySequence::setKey( int key, int index )
{
#ifdef QT_CHECK_STATE
    if ( 0 > index && 4 < index ) {
	tqWarning( "TQKeySequence::setKey: index %u out of range", index );
	return;
    }
#endif // QT_CHECK_STATE

    if ( 1 < d->count ) {
	TQKeySequencePrivate *newd = new TQKeySequencePrivate( d );
	d->deref();
	d = newd;
    }
    d->key[index] = key;
}

/*!
    Returns the number of keys in the key sequence.
    The maximum is 4.
 */
uint TQKeySequence::count() const
{
    if ( ! d->key[0] )
	return 0;
    if ( ! d->key[1] )
	return 1;
    if ( ! d->key[2] )
	return 2;
    if ( ! d->key[3] )
	return 3;
    return 4;
}


/*!
    Returns true if the key sequence is empty; otherwise returns
    false.
*/
bool TQKeySequence::isEmpty() const
{
    return !d->key[0];
}


/*!
    Adds the string \a keyseq to the key sequence. \a keyseq may
    contain up to four key codes, provided they are seperated by a
    comma, e.g. "Alt+X,Ctrl+S,Z"). Returns the number of key codes
    added.
*/
int TQKeySequence::assign( TQString keyseq )
{
    TQString part;
    int n = 0;
    int p = 0, diff = 0;

    // Run through the whole string, but stop
    // if we have 4 keys before the end.
    while ( keyseq.length() && n < 4 ) {
	// We MUST use something to seperate each sequence, and space
	// does not cut it, since some of the key names have space
	// in them.. (Let's hope no one translate with a comma in it:)
	p = keyseq.find( ',' );
	if ( -1 != p ) {
	    if ( ',' == keyseq[p+1] ) // e.g. 'Ctrl+,, Shift+,,'
		p++;
	    if ( ' ' == keyseq[p+1] ) { // Space after comma
		diff = 1;
		p++;
	    } else if ( '\0' == keyseq[p+1] ) { // Last comma 'Ctrl+,'
                p = -1;
	    } else {
		diff = 0;
	    }
	}
	part = keyseq.left( -1==p?keyseq.length():p-diff );
	keyseq = keyseq.right( -1==p?0:keyseq.length() - ( p + 1 ) );
	d->key[n] = decodeString( part );
	n++;
    }
    return n;
}

struct ModifKeyName {
    ModifKeyName() { }
    ModifKeyName(int q, TQString n) : tqt_key(q), name(n) { }
    int tqt_key;
    TQString name;
};

/*!
    Constructs a single key from the string \str.
 */
int TQKeySequence::decodeString( const TQString& str )
{
    int ret = 0;
    TQString accel = str;

    TQValueList<ModifKeyName> modifs;
#ifdef TQMAC_CTRL
    modifs << ModifKeyName( CTRL, TQMAC_CTRL );
#endif
#ifdef TQMAC_ALT
    modifs << ModifKeyName( ALT, TQMAC_ALT );
#endif
#ifdef TQMAC_META
    modifs << ModifKeyName( META, TQMAC_META );
#endif
#ifdef TQMAC_SHIFT
    modifs << ModifKeyName( SHIFT, TQMAC_SHIFT );
#endif
    modifs << ModifKeyName( CTRL, "ctrl+" ) << ModifKeyName( CTRL, TQAccel::tr("Ctrl").lower().append('+') );
    modifs << ModifKeyName( SHIFT, "shift+" ) << ModifKeyName( SHIFT, TQAccel::tr("Shift").lower().append('+') );
    modifs << ModifKeyName( ALT, "alt+" ) << ModifKeyName( ALT, TQAccel::tr("Alt").lower().append('+') );
    modifs << ModifKeyName( META, "meta+" ) << ModifKeyName( ALT, TQAccel::tr("Meta").lower().append('+') );
    TQString sl = accel.lower();
    for( TQValueList<ModifKeyName>::iterator it = modifs.begin(); it != modifs.end(); ++it ) {
	if ( sl.contains( (*it).name ) ) {
	    ret |= (*it).tqt_key;
#ifndef TQT_NO_REGEXP
	    accel.remove( TQRegExp(TQRegExp::escape((*it).name), false) );
#else
	    accel.remove( (*it).name );
#endif
	    sl = accel.lower();
	}
    }

    int p = accel.findRev( '+', str.length() - 2 ); // -2 so that Ctrl++ works
    if( p > 0 )
	accel = accel.mid( p + 1 );

    int fnum = 0;
    if ( accel.length() == 1 ) {
	char ltr = accel[0].upper().latin1();
	// We can only upper A-Z without problems.
	if ( ltr < (char)Key_A || ltr > (char)Key_Z )
	    ret |= accel[0].unicode();
	else
	    ret |= accel[0].upper().unicode();
	ret |= UNICODE_ACCEL;
    } else if ( accel[0] == 'F' && (fnum = accel.mid(1).toInt()) && (fnum >= 1) && (fnum <= 35) ) {
        ret |= Key_F1 + fnum - 1;
    } else {
	// Check through translation table for the correct key name
	// ...or fall back on english table.
	bool found = false;
	for ( int tran = 0; tran < 2; tran++ ) {
	    for ( int i = 0; keyname[i].name; i++ ) {
		if ( tran ? accel == TQAccel::tr(keyname[i].name)
			  : accel == keyname[i].name ) {
		    ret |= keyname[i].key;
		    found = true;
		    break;
		}
	    }
	    if(found)
		break;
	}
    }
    return ret;
}


/*!
    Creates an accelerator string for \a key. For example,
    CTRL+Key_O gives "Ctrl+O". The strings, "Ctrl", "Shift", etc. are
    translated (using TQObject::tr()) in the "TQAccel" context.
 */
TQString TQKeySequence::encodeString( int key )
{
    TQString s;
#if defined(Q_OS_MAC) && !defined(TQWS)
    // On MAC the order is Meta, Alt, Shift, Control.
    if ( (key & META) == META )
	s += TQMAC_META;
    if ( (key & ALT) == ALT )
	s += TQMAC_ALT;
    if ( (key & SHIFT) == SHIFT )
	s += TQMAC_SHIFT;
    if ( (key & CTRL) == CTRL )
	s += TQMAC_CTRL;
#else
    // On other systems the order is Meta, Control, Alt, Shift
    if ( (key & META) == META )
	s += TQAccel::tr( "Meta" );
    if ( (key & CTRL) == CTRL ) {
	if ( !s.isEmpty() )
	    s += TQAccel::tr( "+" );
	s += TQAccel::tr( "Ctrl" );
    }
    if ( (key & ALT) == ALT ) {
	if ( !s.isEmpty() )
	    s += TQAccel::tr( "+" );
	s += TQAccel::tr( "Alt" );
    }
    if ( (key & SHIFT) == SHIFT ) {
	if ( !s.isEmpty() )
	    s += TQAccel::tr( "+" );
	s += TQAccel::tr( "Shift" );
    }
#endif


    key &= ~(SHIFT | CTRL | ALT | META );
    TQString p;

    if ( (key & UNICODE_ACCEL) == UNICODE_ACCEL ) {
	// Note: This character should NOT be upper()'ed, since
	// the encoded string should indicate EXACTLY what the
	// key represents! Hence a 'Ctrl+Shift+c' is posible to
	// represent, but is clearly impossible to trigger...
	p = TQChar(key & 0xffff);
    } else if ( key >= Key_F1 && key <= Key_F35 ) {
	p = TQAccel::tr( "F%1" ).arg(key - Key_F1 + 1);
    } else if ( key > Key_Space && key <= Key_AsciiTilde ) {
	p.sprintf( "%c", key );
    } else {
	int i=0;
	while (keyname[i].name) {
	    if ( key == keyname[i].key ) {
		p = TQAccel::tr(keyname[i].name);
		break;
	    }
	    ++i;
	}
	// If we can't find the actual translatable keyname,
	// fall back on the unicode representation of it...
	// Or else characters like Key_aring may not get displayed
	// ( Really depends on you locale )
	if ( !keyname[i].name )
	    // Note: This character should NOT be upper()'ed, see above!
	    p = TQChar(key & 0xffff);
    }

#ifndef Q_OS_MAC
    if ( !s.isEmpty() )
	s += TQAccel::tr( "+" );
#endif

    s += p;
    return s;
}

/*!
    Matches the sequence with \a seq. Returns \c TQt::Identical if
    successful, \c TQt::PartialMatch for matching but incomplete \a seq,
    and \c TQt::NoMatch if the sequences have nothing in common.
    Returns \c TQt::NoMatch if \a seq is shorter.
*/
TQt::SequenceMatch TQKeySequence::matches( const TQKeySequence& seq ) const
{
    uint userN = count(),
	  seqN = seq.count();

    if ( userN > seqN )
	return NoMatch;

    // If equal in length, we have a potential Identical sequence,
    // else we already know it can only be partial.
    SequenceMatch match = ( userN == seqN ? Identical : PartialMatch );

    for ( uint i = 0; i < userN; i++ ) {
	int userKey      = (*this)[i],
	    sequenceKey  = seq[i];

	if ( (userKey & ~TQt::UNICODE_ACCEL) != 
	     (sequenceKey & ~TQt::UNICODE_ACCEL) )
	    return NoMatch;
    }
    return match;
}


/*!
    Creates an accelerator string for the key sequence.
    For instance CTRL+Key_O gives "Ctrl+O". If the key sequence has
    multiple key codes they are returned comma-separated, e.g.
    "Alt+X, Ctrl+Y, Z". The strings, "Ctrl", "Shift", etc. are
    translated (using TQObject::tr()) in the "TQAccel" scope. If the key
    sequence has no keys, TQString::null is returned.

    On Mac OS X, the string returned resembles the sequence that is shown in
    the menubar.
*/
TQKeySequence::operator TQString() const
{
    int end = count();
    if ( !end ) return TQString::null;

    TQString complete;
    int i = 0;
    while ( i < end ) {
	complete += encodeString( d->key[i] );
	i++;
	if ( i != end)
	    complete += ", ";
    }
    return complete;
}


/*!
    \obsolete
    For backward compatibility: returns the first keycode
    as integer. If the key sequence is empty, 0 is returned.
 */
TQKeySequence::operator int () const
{
    if ( 1 <= count() )
	return d->key[0];
    return 0;
}


/*!
    Returns a reference to the element at position \a index in the key
    sequence. This can only be used to read an element.
 */
int TQKeySequence::operator[]( uint index ) const
{
#ifdef QT_CHECK_STATE
    if ( index > 4 ) {
	tqWarning( "TQKeySequence::operator[]: index %u out of range", index );
	return 0;
    }
#endif // QT_CHECK_STATE
    return d->key[index];
}


/*!
    Assignment operator. Assigns \a keysequence to this
    object.
 */
TQKeySequence &TQKeySequence::operator=( const TQKeySequence & keysequence )
{
    keysequence.d->ref();
    if ( d->deref() )
	delete d;
    d = keysequence.d;
    return *this;
}


/*!
    Returns true if \a keysequence is equal to this key
    sequence; otherwise returns false.
 */


bool TQKeySequence::operator==( const TQKeySequence& keysequence ) const
{
    return ( (d->key[0]&~UNICODE_ACCEL) == (keysequence.d->key[0]&~UNICODE_ACCEL) &&
	     (d->key[1]&~UNICODE_ACCEL) == (keysequence.d->key[1]&~UNICODE_ACCEL) &&
	     (d->key[2]&~UNICODE_ACCEL) == (keysequence.d->key[2]&~UNICODE_ACCEL) &&
	     (d->key[3]&~UNICODE_ACCEL) == (keysequence.d->key[3]&~UNICODE_ACCEL) );
}


/*!
    Returns true if \a keysequence is not equal to this key sequence;
    otherwise returns false.
*/
bool TQKeySequence::operator!= ( const TQKeySequence& keysequence ) const
{
    TQKeySequence *that = (TQKeySequence*)this;
    return !( (*that) == keysequence );
}


/*****************************************************************************
  TQKeySequence stream functions
 *****************************************************************************/
#if !defined(TQT_NO_DATASTREAM) && !defined(TQT_NO_IMAGEIO)
/*!
    \relates TQKeySequence

    Writes the key sequence \a keysequence to the stream \a s.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/
TQDataStream &operator<<( TQDataStream &s, const TQKeySequence &keysequence )
{
    TQValueList<int> list;
    list += keysequence.d->key[0];
    list += keysequence.d->key[1];
    list += keysequence.d->key[2];
    list += keysequence.d->key[3];
    s << list;

    return s;
}


/*!
    \relates TQKeySequence

    Reads a key sequence from the stream \a s into the key sequence \a
    keysequence.

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/
TQDataStream &operator>>( TQDataStream &s, TQKeySequence &keysequence )
{
    TQValueList<int> list;
    s >> list;

#ifdef QT_CHECK_STATE
    if ( 1 != list.count() && 4 != list.count() ) {
	tqWarning( "Invalid TQKeySequence data in the datastream." );
	return s;
    }
#endif

    if ( 1 == list.count() ) {
	keysequence.d->key[0] = *list.at( 0 );
	keysequence.d->key[1] =
	    keysequence.d->key[2] =
	    keysequence.d->key[3] = 0;
    } else {
	keysequence.d->key[0] = *list.at( 0 );
	keysequence.d->key[1] = *list.at( 1 );
	keysequence.d->key[2] = *list.at( 2 );
	keysequence.d->key[3] = *list.at( 3 );
    }
    return s;
}

#endif //TQT_NO_DATASTREAM

#endif //TQT_NO_ACCEL
