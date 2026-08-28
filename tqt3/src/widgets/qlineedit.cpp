/**********************************************************************
**
** Implementation of TQLineEdit widget class
**
** Created : 941011
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

#include "ntqlineedit.h"
#ifndef TQT_NO_LINEEDIT

// Keep this position to avoid patch rejection
#ifndef TQT_NO_IM
#include "ntqinputcontext.h"
#endif

#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqfontmetrics.h"
#include "ntqpixmap.h"
#include "ntqclipboard.h"
#include "ntqapplication.h"
#include "ntqvalidator.h"
#include "ntqdragobject.h"
#include "ntqtimer.h"
#include "ntqpopupmenu.h"
#include "ntqstringlist.h"
#include "ntqguardedptr.h"
#include "ntqstyle.h"
#include "ntqwhatsthis.h"
#include "../kernel/qinternal_p.h"
#include "private/qtextlayout_p.h"
#include "ntqvaluevector.h"
#if defined(Q_OS_LINUX)
#include <sys/mman.h>
#endif
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "ntqaccessible.h"
#endif

#ifndef TQT_NO_ACCEL
#include "ntqkeysequence.h"
#define ACCEL_KEY(k) "\t" + TQString(TQKeySequence( TQt::CTRL | TQt::Key_ ## k ))
#else
#define ACCEL_KEY(k) "\t" + TQString("Ctrl+" #k)
#endif

#if defined(Q_OS_LINUX)
#define LINUX_MEMLOCK_LIMIT_BYTES 16384
#define LINUX_MEMLOCK_LIMIT_CHARACTERS LINUX_MEMLOCK_LIMIT_BYTES/sizeof(TQChar)
#endif

#define innerMargin 1

struct TQLineEditPrivate : public TQt
{
    TQLineEditPrivate( TQLineEdit *q )
	: q(q), cursor(0), cursorTimer(0), tripleClickTimer(0), frame(1),
	  cursorVisible(0), separator(0), readOnly(0), modified(0),
	  direction(TQChar::DirON), dragEnabled(1), alignment(0),
	  echoMode(0), textDirty(0), selDirty(0), validInput(1),
	  ascent(0), maxLength(32767), menuId(0),
	  hscroll(0), validator(0), maskData(0),
	  undoState(0), selstart(0), selend(0),
	  imstart(0), imend(0), imselstart(0), imselend(0)
#ifndef TQT_NO_DRAGANDDROP
	,dndTimer(0)
#endif
	{}
    void init( const TQString&);

    TQLineEdit *q;
    TQString text;
    int cursor;
    int cursorTimer;
    TQPoint tripleClick;
    int tripleClickTimer;
    uint frame : 1;
    uint cursorVisible : 1;
    uint separator : 1;
    uint readOnly : 1;
    uint modified : 1;
    uint direction : 5;
    uint dragEnabled : 1;
    uint alignment : 3;
    uint echoMode : 2;
    uint textDirty : 1;
    uint selDirty : 1;
    uint validInput : 1;
    int ascent;
    int maxLength;
    int menuId;
    int hscroll;
    TQChar passwordChar; // obsolete

    void finishChange( int validateFromState = -1, bool setModified = true );

    const TQValidator* validator;
    struct MaskInputData {
	enum Casemode { NoCaseMode, Upper, Lower };
	TQChar maskChar; // either the separator char or the inputmask
	bool separator;
	Casemode caseMode;
    };
    TQString inputMask;
    TQChar blank;
    MaskInputData *maskData;
    inline int nextMaskBlank( int pos ) {
	int c = findInMask( pos, true, false );
	separator |= ( c != pos );
	return ( c != -1 ?  c : maxLength );
    }
    inline int prevMaskBlank( int pos ) {
	int c = findInMask( pos, false, false );
	separator |= ( c != pos );
	return ( c != -1 ? c : 0 );
    }

    void setCursorVisible( bool visible );


    // undo/redo handling
    enum CommandType { Separator, Insert, Remove, Delete, RemoveSelection, DeleteSelection };
    struct Command {
	inline Command(){}
	inline Command( CommandType type, int pos, TQChar c )
	    :type(type),c(c),pos(pos){}
	uint type : 4;
	TQChar c;
	int pos;
    };
    int undoState;
    TQValueVector<Command> history;
    void addCommand( const Command& cmd );
    void insert( const TQString& s );
    void del( bool wasBackspace = false );
    void remove( int pos );

    inline void separate() { separator = true; }
    inline void undo( int until = -1 ) {
	if ( !isUndoAvailable() )
	    return;
	deselect();
	while ( undoState && undoState > until ) {
	    Command& cmd = history[--undoState];
	    switch ( cmd.type ) {
	    case Insert:
		text.remove( cmd.pos, 1);
		cursor = cmd.pos;
		break;
	    case Remove:
	    case RemoveSelection:
		text.insert( cmd.pos, cmd.c );
		cursor = cmd.pos + 1;
		break;
	    case Delete:
	    case DeleteSelection:
		text.insert( cmd.pos, cmd.c );
		cursor = cmd.pos;
		break;
	    case Separator:
		continue;
	    }
	    if ( until < 0 && undoState ) {
		Command& next = history[undoState-1];
		if ( next.type != cmd.type && next.type < RemoveSelection
		     && !( cmd.type >= RemoveSelection && next.type != Separator ) )
		    break;
	    }
	}
	modified = ( undoState != 0 );
	textDirty = true;
    }
    inline void redo() {
	if ( !isRedoAvailable() )
	    return;
	deselect();
	while ( undoState < (int)history.size() ) {
	    Command& cmd = history[undoState++];
	    switch ( cmd.type ) {
	    case Insert:
		text.insert( cmd.pos, cmd.c );
		cursor = cmd.pos + 1;
		break;
	    case Remove:
	    case Delete:
	    case RemoveSelection:
	    case DeleteSelection:
		text.remove( cmd.pos, 1 );
		cursor = cmd.pos;
		break;
	    case Separator:
		continue;
	    }
	    if ( undoState < (int)history.size() ) {
		Command& next = history[undoState];
		if ( next.type != cmd.type && cmd.type < RemoveSelection
		     && !( next.type >= RemoveSelection && cmd.type != Separator ) )
		    break;
	    }
	}
	textDirty = true;
    }
    inline bool isUndoAvailable() const { return !readOnly && undoState; }
    inline bool isRedoAvailable() const { return !readOnly && undoState < (int)history.size(); }

    // bidi
    inline bool isRightToLeft() const { return direction==TQChar::DirON?text.isRightToLeft():(direction==TQChar::DirR); }

    // selection
    int selstart, selend;
    inline bool allSelected() const { return !text.isEmpty() && selstart == 0 && selend == (int)text.length(); }
    inline bool hasSelectedText() const { return !text.isEmpty() && selend > selstart; }
    inline void deselect() { selDirty |= (selend > selstart); selstart = selend = 0; }
    void removeSelectedText();
#ifndef TQT_NO_CLIPBOARD
    void copy( bool clipboard = true ) const;
#endif
    inline bool inSelection( int x ) const
    { if ( selstart >= selend ) return false;
    int pos = xToPos( x, TQTextItem::OnCharacters );  return pos >= selstart && pos < selend; }

    // masking
    void parseInputMask( const TQString &maskFields );
    bool isValidInput( TQChar key, TQChar mask ) const;
    TQString maskString( uint pos, const TQString &str, bool clear = false ) const;
    TQString clearString( uint pos, uint len ) const;
    TQString stripString( const TQString &str ) const;
    int findInMask( int pos, bool forward, bool findSeparator, TQChar searchChar = TQChar() ) const;

    // input methods
    int imstart, imend, imselstart, imselend;
    bool composeMode() const { return preeditLength(); }
    bool hasIMSelection() const { return imSelectionLength(); }
    int preeditLength() const { return ( imend - imstart ); }
    int imSelectionLength() const { return ( imselend - imselstart ); }

    // complex text layout
    TQTextLayout textLayout;
    void updateTextLayout();
    void moveCursor( int pos, bool mark = false );
    void setText( const TQString& txt );
    int xToPosInternal( int x, TQTextItem::CursorPosition ) const;
    int xToPos( int x, TQTextItem::CursorPosition = TQTextItem::BetweenCharacters ) const;
    inline int visualAlignment() const { return alignment ? alignment : int( isRightToLeft() ? AlignRight : AlignLeft ); }
    TQRect cursorRect() const;
    void updateMicroFocusHint();

#ifndef TQT_NO_DRAGANDDROP
    // drag and drop
    TQPoint dndPos;
    int dndTimer;
    bool drag();
#endif
};


/*!
    \class TQLineEdit
    \brief The TQLineEdit widget is a one-line text editor.

    \ingroup basic
    \mainclass

    A line edit allows the user to enter and edit a single line of
    plain text with a useful collection of editing functions,
    including undo and redo, cut and paste, and drag and drop.

    By changing the echoMode() of a line edit, it can also be used as
    a "write-only" field, for inputs such as passwords.

    The length of the text can be constrained to maxLength(). The text
    can be arbitrarily constrained using a validator() or an
    inputMask(), or both.

    A related class is TQTextEdit which allows multi-line, rich-text
    editing.

    You can change the text with setText() or insert(). The text is
    retrieved with text(); the displayed text (which may be different,
    see \l{EchoMode}) is retrieved with displayText(). Text can be
    selected with setSelection() or selectAll(), and the selection can
    be cut(), copy()ied and paste()d. The text can be aligned with
    setAlignment().

    When the text changes the textChanged() signal is emitted; when
    the Return or Enter key is pressed the returnPressed() signal is
    emitted. Note that if there is a validator set on the line edit,
    the returnPressed() signal will only be emitted if the validator
    returns \c Acceptable.

    By default, TQLineEdits have a frame as specified by the Windows
    and Motif style guides; you can turn it off by calling
    setFrame(false).

    The default key bindings are described below. The line edit also
    provides a context menu (usually invoked by a right mouse click)
    that presents some of these editing options.
    \target desc
    \table
    \header \i Keypress \i Action
    \row \i Left Arrow \i Moves the cursor one character to the left.
    \row \i Shift+Left Arrow \i Moves and selects text one character to the left.
    \row \i Right Arrow \i Moves the cursor one character to the right.
    \row \i Shift+Right Arrow \i Moves and selects text one character to the right.
    \row \i Home \i Moves the cursor to the beginning of the line.
    \row \i End \i Moves the cursor to the end of the line.
    \row \i Backspace \i Deletes the character to the left of the cursor.
    \row \i Ctrl+Backspace \i Deletes the word to the left of the cursor.
    \row \i Delete \i Deletes the character to the right of the cursor.
    \row \i Ctrl+Delete \i Deletes the word to the right of the cursor.
    \row \i Ctrl+A \i Moves the cursor to the beginning of the line.
    \row \i Ctrl+B \i Moves the cursor one character to the left.
    \row \i Ctrl+C \i Copies the selected text to the clipboard.
		      (Windows also supports Ctrl+Insert for this operation.)
    \row \i Ctrl+D \i Deletes the character to the right of the cursor.
    \row \i Ctrl+E \i Moves the cursor to the end of the line.
    \row \i Ctrl+F \i Moves the cursor one character to the right.
    \row \i Ctrl+H \i Deletes the character to the left of the cursor.
    \row \i Ctrl+K \i Deletes to the end of the line.
    \row \i Ctrl+V \i Pastes the clipboard text into line edit.
		      (Windows also supports Shift+Insert for this operation.)
    \row \i Ctrl+X \i Deletes the selected text and copies it to the clipboard.
		      (Windows also supports Shift+Delete for this operation.)
    \row \i Ctrl+Z \i Undoes the last operation.
    \row \i Ctrl+Y \i Redoes the last undone operation.
    \endtable

    Any other key sequence that represents a valid character, will
    cause the character to be inserted into the line edit.

    <img src=qlined-m.png> <img src=qlined-w.png>

    \sa TQTextEdit TQLabel TQComboBox
	\link guibooks.html#fowler GUI Design Handbook: Field, Entry\endlink
*/


/*!
    \fn void TQLineEdit::textChanged( const TQString& )

    This signal is emitted whenever the text changes. The argument is
    the new text.
*/

/*!
    \fn void TQLineEdit::selectionChanged()

    This signal is emitted whenever the selection changes.

    \sa hasSelectedText(), selectedText()
*/

/*!
    \fn void TQLineEdit::lostFocus()

    This signal is emitted when the line edit has lost focus.

    \sa hasFocus(), TQWidget::focusInEvent(), TQWidget::focusOutEvent()
*/



/*!
    Constructs a line edit with no text.

    The maximum text length is set to 32767 characters.

    The \a parent and \a name arguments are sent to the TQWidget constructor.

    \sa setText(), setMaxLength()
*/

TQLineEdit::TQLineEdit( TQWidget* parent, const char* name )
    : TQFrame( parent, name, WNoAutoErase ), d(new TQLineEditPrivate( this ))
{
    d->init( TQString::null );
}

/*!
    Constructs a line edit containing the text \a contents.

    The cursor position is set to the end of the line and the maximum
    text length to 32767 characters.

    The \a parent and \a name arguments are sent to the TQWidget
    constructor.

    \sa text(), setMaxLength()
*/

TQLineEdit::TQLineEdit( const TQString& contents, TQWidget* parent, const char* name )
    : TQFrame( parent, name, WNoAutoErase ), d(new TQLineEditPrivate( this ))
{
    d->init( contents );
}

/*!
    Constructs a line edit with an input \a inputMask and the text \a
    contents.

    The cursor position is set to the end of the line and the maximum
    text length is set to the length of the mask (the number of mask
    characters and separators).

    The \a parent and \a name arguments are sent to the TQWidget
    constructor.

    \sa setMask() text()
*/
TQLineEdit::TQLineEdit( const TQString& contents, const TQString &inputMask, TQWidget* parent, const char* name )
    : TQFrame( parent, name, WNoAutoErase ), d(new TQLineEditPrivate( this ))
{
    d->parseInputMask( inputMask );
    if ( d->maskData ) {
	TQString ms = d->maskString( 0, contents );
	d->init( ms + d->clearString( ms.length(), d->maxLength - ms.length() ) );
	d->cursor = d->nextMaskBlank( ms.length() );
    } else {
	d->init( contents );
    }
}

/*!
    Destroys the line edit.
*/

TQLineEdit::~TQLineEdit()
{
    if ((d->echoMode == NoEcho) || (d->echoMode == Password) || (d->echoMode == PasswordThreeStars)) {
	d->text.fill(TQChar(0));
#if defined(Q_OS_LINUX)
	munlock(d->text.d->unicode, LINUX_MEMLOCK_LIMIT_BYTES);
#endif
    }
    delete [] d->maskData;
    delete d;
}


/*!
    \property TQLineEdit::text
    \brief the line edit's text

    Note that setting this property clears the selection, clears the
    undo/redo history, moves the cursor to the end of the line and
    resets the \c modified property to false. The text is not
    validated when inserted with setText().

    The text is truncated to maxLength() length.

    \sa insert()
*/
TQString TQLineEdit::text() const
{
    TQString res = d->text;
    if ( d->maskData )
	res = d->stripString( d->text );
    return ( res.isNull() ? TQString::fromLatin1("") : res );
}

void TQLineEdit::setText( const TQString& text)
{
    resetInputContext();
    d->setText( text );
    d->modified = false;
    d->finishChange( -1, false );
}


/*!
    \property TQLineEdit::displayText
    \brief the displayed text

    If \c EchoMode is \c Normal this returns the same as text(); if
    \c EchoMode is \c Password it returns a string of asterisks
    text().length() characters long, e.g. "******"; if \c EchoMode is
    \c NoEcho returns an empty string, "".

    \sa setEchoMode() text() EchoMode
*/

TQString TQLineEdit::displayText() const
{
    if ( d->echoMode == NoEcho ) {
	return TQString::fromLatin1("");
    }
    TQString res = d->text;
    if ( d->echoMode == Password ) {
	res.fill( passwordChar() );
    }
    else if ( d->echoMode == PasswordThreeStars ) {
	res.fill( passwordChar(), res.length()*3 );
    }
    return ( res.isNull() ? TQString::fromLatin1("") : res );
}


/*!
    \property TQLineEdit::maxLength
    \brief the maximum permitted length of the text

    If the text is too long, it is truncated at the limit.

    If truncation occurs any selected text will be unselected, the
    cursor position is set to 0 and the first part of the string is
    shown.

    If the line edit has an input mask, the mask defines the maximum
    string length.

    \sa inputMask
*/

int TQLineEdit::maxLength() const
{
    return d->maxLength;
}

void TQLineEdit::setMaxLength( int maxLength )
{
    if ( d->maskData )
	return;
    d->maxLength = maxLength;
    setText( d->text );
}



/*!
    \property TQLineEdit::frame
    \brief whether the line edit draws itself with a frame

    If enabled (the default) the line edit draws itself inside a
    two-pixel frame, otherwise the line edit draws itself without any
    frame.
*/
bool TQLineEdit::frame() const
{
    return frameShape() != NoFrame;
}


void TQLineEdit::setFrame( bool enable )
{
    setFrameStyle( enable ? ( LineEditPanel | Sunken ) : NoFrame  );
}


/*!
    \enum TQLineEdit::EchoMode

    This enum type describes how a line edit should display its
    contents.

    \value Normal   Display characters as they are entered. This is the
		    default.
    \value NoEcho   Do not display anything. This may be appropriate
		    for passwords where even the length of the
		    password should be kept secret.
    \value Password  Display asterisks instead of the characters
		    actually entered.

    \sa setEchoMode() echoMode()
*/


/*!
    \property TQLineEdit::echoMode
    \brief the line edit's echo mode

    The initial setting is \c Normal, but TQLineEdit also supports \c
    NoEcho and \c Password modes.

    The widget's display and the ability to copy or drag the text is
    affected by this setting.

    \sa EchoMode displayText()
*/

TQLineEdit::EchoMode TQLineEdit::echoMode() const
{
    return (EchoMode) d->echoMode;
}

void TQLineEdit::setEchoMode( EchoMode mode )
{
    if (mode == (EchoMode)d->echoMode) {
	return;
    }
#if defined(Q_OS_LINUX)
    if (((mode == NoEcho) || (mode == Password) || (mode == PasswordThreeStars)) && ((EchoMode)d->echoMode == Normal)) {
    	if ((uint)d->maxLength > (LINUX_MEMLOCK_LIMIT_CHARACTERS-1)) {
	    d->maxLength = LINUX_MEMLOCK_LIMIT_CHARACTERS-1;
    	}
        d->text.reserve(LINUX_MEMLOCK_LIMIT_CHARACTERS);
        mlock(d->text.d->unicode, LINUX_MEMLOCK_LIMIT_BYTES);
        d->text.setSecurityUnPaged(true);
    }
    else {
        d->text.setSecurityUnPaged(false);
        munlock(d->text.d->unicode, LINUX_MEMLOCK_LIMIT_BYTES);
    }
#endif
    d->echoMode = mode;
    d->updateTextLayout();
    setInputMethodEnabled( ( mode == Normal ) || ( mode == Password) );
    update();
}



/*!
    Returns a pointer to the current input validator, or 0 if no
    validator has been set.

    \sa setValidator()
*/

const TQValidator * TQLineEdit::validator() const
{
    return d->validator;
}

/*!
    Sets this line edit to only accept input that the validator, \a v,
    will accept. This allows you to place any arbitrary constraints on
    the text which may be entered.

    If \a v == 0, setValidator() removes the current input validator.
    The initial setting is to have no input validator (i.e. any input
    is accepted up to maxLength()).

    \sa validator() TQIntValidator TQDoubleValidator TQRegExpValidator
*/

void TQLineEdit::setValidator( const TQValidator *v )
{
    if ( d->validator )
	disconnect( (TQObject*)d->validator, TQ_SIGNAL( destroyed() ),
		    this, TQ_SLOT( clearValidator() ) );
    d->validator = v;
    if ( d->validator )
	connect( (TQObject*)d->validator, TQ_SIGNAL( destroyed() ),
	         this, TQ_SLOT( clearValidator() ) );
}



/*!
    Returns a recommended size for the widget.

    The width returned, in pixels, is usually enough for about 15 to
    20 characters.
*/

TQSize TQLineEdit::sizeHint() const
{
    constPolish();
    TQFontMetrics fm( font() );
    int h = TQMAX(fm.lineSpacing(), 14) + 2*innerMargin;
    int w = fm.width( 'x' ) * 17; // "some"
    int m = frameWidth() * 2;
    return (style().sizeFromContents(TQStyle::CT_LineEdit, this,
				     TQSize( w + m, h + m ).
				     expandedTo(TQApplication::globalStrut())));
}


/*!
    Returns a minimum size for the line edit.

    The width returned is enough for at least one character.
*/

TQSize TQLineEdit::minimumSizeHint() const
{
    constPolish();
    TQFontMetrics fm = fontMetrics();
    int h = fm.height() + TQMAX( 2*innerMargin, fm.leading() );
    int w = fm.maxWidth();
    int m = frameWidth() * 2;
    return TQSize( w + m, h + m );
}


/*!
    \property TQLineEdit::cursorPosition
    \brief the current cursor position for this line edit

    Setting the cursor position causes a repaint when appropriate.
*/

int TQLineEdit::cursorPosition() const
{
    return d->cursor;
}


void TQLineEdit::setCursorPosition( int pos )
{
    if (pos < 0)
        pos = 0;

    if ( pos <= (int) d->text.length() )
	d->moveCursor( pos );
}


/*! \obsolete Use setText(), setCursorPosition() and setSelection() instead.
*/

bool TQLineEdit::validateAndSet( const TQString &newText, int newPos,
				 int newMarkAnchor, int newMarkDrag )
{
    int priorState = d->undoState;
    d->selstart = 0;
    d->selend = d->text.length();
    d->removeSelectedText();
    d->insert( newText );
    d->finishChange( priorState );
    if ( d->undoState > priorState ) {
	d->cursor = newPos;
	d->selstart = TQMIN( newMarkAnchor, newMarkDrag );
	d->selend = TQMAX( newMarkAnchor, newMarkDrag );
	d->updateMicroFocusHint();
	update();
	return true;
    }
    return false;
}


/*!
    \property TQLineEdit::alignment
    \brief the alignment of the line edit

    Possible Values are \c TQt::AlignAuto, \c TQt::AlignLeft, \c
    TQt::AlignRight and \c TQt::AlignHCenter.

    Attempting to set the alignment to an illegal flag combination
    does nothing.

    \sa TQt::AlignmentFlags
*/

int TQLineEdit::alignment() const
{
    return d->alignment;
}

void TQLineEdit::setAlignment( int flag )
{
    d->alignment = flag & 0x7;
    update();
}


/*!
  \obsolete
  \fn void TQLineEdit::cursorRight( bool, int )

  Use cursorForward() instead.

  \sa cursorForward()
*/

/*!
  \obsolete
  \fn void TQLineEdit::cursorLeft( bool, int )
  For compatibilty with older applications only. Use cursorBackward()
  instead.
  \sa cursorBackward()
*/

/*!
    Moves the cursor forward \a steps characters. If \a mark is true
    each character moved over is added to the selection; if \a mark is
    false the selection is cleared.

    \sa cursorBackward()
*/

void TQLineEdit::cursorForward( bool mark, int steps )
{
    int cursor = d->cursor;
    if ( steps > 0 ) {
	while( steps-- )
	    cursor = d->textLayout.nextCursorPosition( cursor );
    } else if ( steps < 0 ) {
	while ( steps++ )
	    cursor = d->textLayout.previousCursorPosition( cursor );
    }
    d->moveCursor( cursor, mark );
}


/*!
    Moves the cursor back \a steps characters. If \a mark is true each
    character moved over is added to the selection; if \a mark is
    false the selection is cleared.

    \sa cursorForward()
*/
void TQLineEdit::cursorBackward( bool mark, int steps )
{
    cursorForward( mark, -steps );
}

/*!
    Moves the cursor one word forward. If \a mark is true, the word is
    also selected.

    \sa cursorWordBackward()
*/
void TQLineEdit::cursorWordForward( bool mark )
{
    d->moveCursor( d->textLayout.nextCursorPosition(d->cursor, TQTextLayout::SkipWords), mark );
}

/*!
    Moves the cursor one word backward. If \a mark is true, the word
    is also selected.

    \sa cursorWordForward()
*/

void TQLineEdit::cursorWordBackward( bool mark )
{
    d->moveCursor( d->textLayout.previousCursorPosition(d->cursor, TQTextLayout::SkipWords), mark );
}


/*!
    If no text is selected, deletes the character to the left of the
    text cursor and moves the cursor one position to the left. If any
    text is selected, the cursor is moved to the beginning of the
    selected text and the selected text is deleted.

    \sa del()
*/
void TQLineEdit::backspace()
{
    int priorState = d->undoState;
    if ( d->hasSelectedText() ) {
	d->removeSelectedText();
    } else if ( d->cursor ) {
	--d->cursor;
	if ( d->maskData ) {
	    d->cursor = d->prevMaskBlank( d->cursor );
	}
	// second half of a surrogate, check if we have the first half as well,
	// if yes delete both at once
	if (d->cursor > 0 && d->text.at(d->cursor).isLowSurrogate() &&
	        d->text.at(d->cursor - 1).isHighSurrogate()) {
	    d->del(true);
	    --d->cursor;
	}
	d->del(true);
    }
    d->finishChange( priorState );
}

/*!
    If no text is selected, deletes the character to the right of the
    text cursor. If any text is selected, the cursor is moved to the
    beginning of the selected text and the selected text is deleted.

    \sa backspace()
*/

void TQLineEdit::del()
{
    int priorState = d->undoState;
    if ( d->hasSelectedText() ) {
	d->removeSelectedText();
    } else {
	int n = d->textLayout.nextCursorPosition( d->cursor ) - d->cursor;
	while ( n-- )
	    d->del();
    }
    d->finishChange( priorState );
}

/*!
    Moves the text cursor to the beginning of the line unless it is
    already there. If \a mark is true, text is selected towards the
    first position; otherwise, any selected text is unselected if the
    cursor is moved.

    \sa end()
*/

void TQLineEdit::home( bool mark )
{
    d->moveCursor( 0, mark );
}

/*!
    Moves the text cursor to the end of the line unless it is already
    there. If \a mark is true, text is selected towards the last
    position; otherwise, any selected text is unselected if the cursor
    is moved.

    \sa home()
*/

void TQLineEdit::end( bool mark )
{
    d->moveCursor( d->text.length(), mark );
}


/*!
    \property TQLineEdit::modified
    \brief whether the line edit's contents has been modified by the user

    The modified flag is never read by TQLineEdit; it has a default value
    of false and is changed to true whenever the user changes the line
    edit's contents.

    This is useful for things that need to provide a default value but
    do not start out knowing what the default should be (perhaps it
    depends on other fields on the form). Start the line edit without
    the best default, and when the default is known, if modified()
    returns false (the user hasn't entered any text), insert the
    default value.

    Calling clearModified() or setText() resets the modified flag to
    false.
*/

bool TQLineEdit::isModified() const
{
    return d->modified;
}

/*!
    Resets the modified flag to false.

    \sa isModified()
*/
void TQLineEdit::clearModified()
{
    d->modified = false;
    d->history.clear();
    d->undoState = 0;
}

/*!
  \obsolete
  \property TQLineEdit::edited
  \brief whether the line edit has been edited. Use modified instead.
*/
bool TQLineEdit::edited() const { return d->modified; }
void TQLineEdit::setEdited( bool on ) { d->modified = on; }

/*!
    \obsolete
    \property TQLineEdit::hasMarkedText
    \brief whether part of the text has been selected by the user. Use hasSelectedText instead.
*/

/*!
    \property TQLineEdit::hasSelectedText
    \brief whether there is any text selected

    hasSelectedText() returns true if some or all of the text has been
    selected by the user; otherwise returns false.

    \sa selectedText()
*/


bool TQLineEdit::hasSelectedText() const
{
    return d->hasSelectedText();
}

/*!
  \obsolete
  \property TQLineEdit::markedText
  \brief the text selected by the user. Use selectedText instead.
*/

/*!
    \property TQLineEdit::selectedText
    \brief the selected text

    If there is no selected text this property's value is
    TQString::null.

    \sa hasSelectedText()
*/

TQString TQLineEdit::selectedText() const
{
    if ( d->hasSelectedText() )
	return d->text.mid( d->selstart, d->selend - d->selstart );
    return TQString::null;
}

/*!
    selectionStart() returns the index of the first selected character in the
    line edit or -1 if no text is selected.

    \sa selectedText()
*/

int TQLineEdit::selectionStart() const
{
    return d->hasSelectedText() ? d->selstart : -1;
}

/*! \obsolete use selectedText(), selectionStart() */
bool TQLineEdit::getSelection( int *start, int *end )
{
    if ( d->hasSelectedText() && start && end ) {
	*start = d->selstart;
	*end = d->selend;
	return true;
    }
    return false;
}


/*!
    Selects text from position \a start and for \a length characters.

    Note that this function sets the cursor's position to the end of
    the selection regardless of its current position.

    \sa deselect() selectAll() getSelection() cursorForward() cursorBackward()
*/

void TQLineEdit::setSelection( int start, int length )
{
    if ( start < 0 || start > (int)d->text.length() || length < 0 ) {
	d->selstart = d->selend = 0;
    } else {
	d->selstart = start;
	d->selend = TQMIN( start + length, (int)d->text.length() );
	d->cursor = d->selend;
    }
    update();
}


/*!
    \property TQLineEdit::undoAvailable
    \brief whether undo is available
*/

bool TQLineEdit::isUndoAvailable() const
{
    return d->isUndoAvailable();
}

/*!
    \property TQLineEdit::redoAvailable
    \brief whether redo is available
*/

bool TQLineEdit::isRedoAvailable() const
{
    return d->isRedoAvailable();
}

/*!
    \property TQLineEdit::dragEnabled
    \brief whether the lineedit starts a drag if the user presses and
    moves the mouse on some selected text
*/

bool TQLineEdit::dragEnabled() const
{
    return d->dragEnabled;
}

void TQLineEdit::setDragEnabled( bool b )
{
    d->dragEnabled = b;
}

/*!
    \property TQLineEdit::acceptableInput
    \brief whether the input satisfies the inputMask and the
    validator.

    \sa setInputMask(), setValidator()
*/
bool TQLineEdit::hasAcceptableInput() const
{
#ifndef TQT_NO_VALIDATOR
    TQString text = d->text;
    int cursor = d->cursor;
    if ( d->validator && d->validator->validate( text, cursor ) != TQValidator::Acceptable )
	return false;
#endif

    if ( !d->maskData )
	return true;

    if ( d->text.length() != (uint)d->maxLength )
 	return false;

    for ( uint i=0; i < (uint)d->maxLength; i++) {
	if ( d->maskData[i].separator ) {
	    if ( d->text[(int)i] != d->maskData[i].maskChar )
		return false;
	} else {
	    if ( !d->isValidInput( d->text[(int)i], d->maskData[i].maskChar ) )
		return false;
	}
    }
    return true;
}


/*!
    \property TQLineEdit::inputMask
    \brief The validation input mask

    If no mask is set, inputMask() returns TQString::null.

    Sets the TQLineEdit's validation mask. Validators can be used
    instead of, or in conjunction with masks; see setValidator().

    Unset the mask and return to normal TQLineEdit operation by passing
    an empty string ("") or just calling setInputMask() with no
    arguments.

    The mask format understands these mask characters:
    \table
    \header \i Character \i Meaning
    \row \i \c A \i ASCII alphabetic character required. A-Z, a-z.
    \row \i \c a \i ASCII alphabetic character permitted but not required.
    \row \i \c N \i ASCII alphanumeric character required. A-Z, a-z, 0-9.
    \row \i \c n \i ASCII alphanumeric character permitted but not required.
    \row \i \c X \i Any character required.
    \row \i \c x \i Any character permitted but not required.
    \row \i \c 9 \i ASCII digit required. 0-9.
    \row \i \c 0 \i ASCII digit permitted but not required.
    \row \i \c D \i ASCII digit required. 1-9.
    \row \i \c d \i ASCII digit permitted but not required (1-9).
    \row \i \c # \i ASCII digit or plus/minus sign permitted but not required.
    \row \i \c > \i All following alphabetic characters are uppercased.
    \row \i \c < \i All following alphabetic characters are lowercased.
    \row \i \c ! \i Switch off case conversion.
    \row \i <tt>\\</tt> \i Use <tt>\\</tt> to escape the special
			   characters listed above to use them as
			   separators.
    \endtable

    The mask consists of a string of mask characters and separators,
    optionally followed by a semi-colon and the character used for
    blanks: the blank characters are always removed from the text
    after editing. The default blank character is space.

    Examples:
    \table
    \header \i Mask \i Notes
    \row \i \c 000.000.000.000;_ \i IP address; blanks are \c{_}.
    \row \i \c 0000-00-00 \i ISO Date; blanks are \c space
    \row \i \c >AAAAA-AAAAA-AAAAA-AAAAA-AAAAA;# \i License number;
    blanks are \c - and all (alphabetic) characters are converted to
    uppercase.
    \endtable

    To get range control (e.g. for an IP address) use masks together
    with \link setValidator() validators\endlink.

    \sa maxLength
*/
TQString TQLineEdit::inputMask() const
{
    return ( d->maskData ? d->inputMask + ';' + d->blank : TQString::null );
}

void TQLineEdit::setInputMask( const TQString &inputMask )
{
    d->parseInputMask( inputMask );
    if ( d->maskData )
	d->moveCursor( d->nextMaskBlank( 0 ) );
}

/*!
    Selects all the text (i.e. highlights it) and moves the cursor to
    the end. This is useful when a default value has been inserted
    because if the user types before clicking on the widget, the
    selected text will be deleted.

    \sa setSelection() deselect()
*/

void TQLineEdit::selectAll()
{
    d->selstart = d->selend = d->cursor = 0;
    d->moveCursor( d->text.length(), true );
}

/*!
    Deselects any selected text.

    \sa setSelection() selectAll()
*/

void TQLineEdit::deselect()
{
    d->deselect();
    d->finishChange();
}


/*!
    This slot is equivalent to setValidator(0).
*/

void TQLineEdit::clearValidator()
{
    setValidator( 0 );
}

/*!
    Deletes any selected text, inserts \a newText, and validates the
    result. If it is valid, it sets it as the new contents of the line
    edit.
*/
void TQLineEdit::insert( const TQString &newText )
{
//     q->resetInputContext(); //#### FIX ME IN QT
    int priorState = d->undoState;
    d->removeSelectedText();
    d->insert( newText );
    d->finishChange( priorState );
}

/*!
    Clears the contents of the line edit.
*/
void TQLineEdit::clear()
{
    int priorState = d->undoState;
    resetInputContext();
    d->selstart = 0;
    d->selend = d->text.length();
    d->removeSelectedText();
    d->separate();
    d->finishChange( priorState );
}

/*!
    Undoes the last operation if undo is \link
    TQLineEdit::undoAvailable available\endlink. Deselects any current
    selection, and updates the selection start to the current cursor
    position.
*/
void TQLineEdit::undo()
{
    resetInputContext();
    d->undo();
    d->finishChange( -1, false );
}

/*!
    Redoes the last operation if redo is \link
    TQLineEdit::redoAvailable available\endlink.
*/
void TQLineEdit::redo()
{
    resetInputContext();
    d->redo();
    d->finishChange();
}


/*!
    \property TQLineEdit::readOnly
    \brief whether the line edit is read only.

    In read-only mode, the user can still copy the text to the
    clipboard or drag-and-drop the text (if echoMode() is \c Normal),
    but cannot edit it.

    TQLineEdit does not show a cursor in read-only mode.

    \sa setEnabled()
*/

bool TQLineEdit::isReadOnly() const
{
    return d->readOnly;
}

void TQLineEdit::setReadOnly( bool enable )
{
    d->readOnly = enable;
#ifndef TQT_NO_CURSOR
    setCursor( enable ? arrowCursor : ibeamCursor );
#endif
    update();
}


#ifndef TQT_NO_CLIPBOARD
/*!
    Copies the selected text to the clipboard and deletes it, if there
    is any, and if echoMode() is \c Normal.

    If the current validator disallows deleting the selected text,
    cut() will copy without deleting.

    \sa copy() paste() setValidator()
*/

void TQLineEdit::cut()
{
    if ( hasSelectedText() ) {
	copy();
	del();
    }
}


/*!
    Copies the selected text to the clipboard, if there is any, and if
    echoMode() is \c Normal.

    \sa cut() paste()
*/

void TQLineEdit::copy() const
{
    d->copy();
}

/*!
    Inserts the clipboard's text at the cursor position, deleting any
    selected text, providing the line edit is not \link
    TQLineEdit::readOnly read-only\endlink.

    If the end result would not be acceptable to the current
    \link setValidator() validator\endlink, nothing happens.

    \sa copy() cut()
*/

void TQLineEdit::paste()
{
    insert( TQApplication::clipboard()->text( TQClipboard::Clipboard ) );
}

void TQLineEditPrivate::copy( bool clipboard ) const
{
    TQString t = q->selectedText();
    if ( !t.isEmpty() && echoMode == TQLineEdit::Normal ) {
	q->disconnect( TQApplication::clipboard(), TQ_SIGNAL(selectionChanged()), q, 0);
	TQApplication::clipboard()->setText( t, clipboard ? TQClipboard::Clipboard : TQClipboard::Selection );
	q->connect( TQApplication::clipboard(), TQ_SIGNAL(selectionChanged()),
		 q, TQ_SLOT(clipboardChanged()) );
    }
}

#endif // !TQT_NO_CLIPBOARD

/*!\reimp
*/

void TQLineEdit::resizeEvent( TQResizeEvent *e )
{
    TQFrame::resizeEvent( e );
}

/*! \reimp
*/
bool TQLineEdit::event( TQEvent * e )
{
    if ( e->type() == TQEvent::AccelOverride && !d->readOnly ) {
	TQKeyEvent* ke = (TQKeyEvent*) e;
	if ( ke->state() == NoButton || ke->state() == ShiftButton
	     || ke->state() == Keypad ) {
	    if ( ke->key() < Key_Escape ) {
		ke->accept();
	    } else {
		switch ( ke->key() ) {
  		case Key_Delete:
  		case Key_Home:
  		case Key_End:
  		case Key_Backspace:
 		case Key_Left:
		case Key_Right:
 		    ke->accept();
 		default:
  		    break;
  		}
	    }
	} else if ( ke->state() & ControlButton ) {
	    switch ( ke->key() ) {
// Those are too frequently used for application functionality
/*	    case Key_A:
	    case Key_B:
	    case Key_D:
	    case Key_E:
	    case Key_F:
	    case Key_H:
	    case Key_K:
*/
	    case Key_C:
	    case Key_V:
	    case Key_X:
	    case Key_Y:
	    case Key_Z:
	    case Key_Left:
	    case Key_Right:
#if defined (TQ_WS_WIN)
	    case Key_Insert:
	    case Key_Delete:
#endif
		ke->accept();
	    default:
		break;
	    }
	}
    } else if ( e->type() == TQEvent::Timer ) {
	// should be timerEvent, is here for binary compatibility
	int timerId = ((TQTimerEvent*)e)->timerId();
	if ( timerId == d->cursorTimer ) {
	    if(!hasSelectedText() || style().styleHint( TQStyle::SH_BlinkCursorWhenTextSelected ))
		d->setCursorVisible( !d->cursorVisible );
#ifndef TQT_NO_DRAGANDDROP
	} else if ( timerId == d->dndTimer ) {
	    if( !d->drag() )
                return true;
#endif
	} else if ( timerId == d->tripleClickTimer ) {
	    killTimer( d->tripleClickTimer );
	    d->tripleClickTimer = 0;
	}
    }
    return TQWidget::event( e );
}

/*! \reimp
*/
void TQLineEdit::mousePressEvent( TQMouseEvent* e )
{
    if ( sendMouseEventToInputContext( e ) )
	return;
    if ( e->button() == RightButton )
	return;
    if ( d->tripleClickTimer && ( e->pos() - d->tripleClick ).manhattanLength() <
	 TQApplication::startDragDistance() ) {
	selectAll();
	return;
    }
    bool mark = e->state() & ShiftButton;
    int cursor = d->xToPos( e->pos().x() );
#ifndef TQT_NO_DRAGANDDROP
    if ( !mark && d->dragEnabled && d->echoMode == Normal &&
	 e->button() == LeftButton && d->inSelection( e->pos().x() ) ) {
	d->cursor = cursor;
	d->updateMicroFocusHint();
	update();
	d->dndPos = e->pos();
	if ( !d->dndTimer )
	    d->dndTimer = startTimer( TQApplication::startDragTime() );
    } else
#endif
    {
	d->moveCursor( cursor, mark );
    }
}

/*! \reimp
*/
void TQLineEdit::mouseMoveEvent( TQMouseEvent * e )
{
    if ( sendMouseEventToInputContext( e ) )
	return;
#ifndef TQT_NO_CURSOR
    if ( ( e->state() & MouseButtonMask ) == 0 ) {
	if ( !d->readOnly && d->dragEnabled
#ifndef TQT_NO_WHATSTHIS
	     && !TQWhatsThis::inWhatsThisMode()
#endif
	    )
	    setCursor( ( d->inSelection( e->pos().x() ) ? arrowCursor : ibeamCursor ) );
    }
#endif

    if ( e->state() & LeftButton ) {
#ifndef TQT_NO_DRAGANDDROP
	if ( d->dndTimer ) {
	    if ( ( d->dndPos - e->pos() ).manhattanLength() > TQApplication::startDragDistance() )
		d->drag();
	} else
#endif
	{
	    d->moveCursor( d->xToPos( e->pos().x() ), true );
	}
    }
}

/*! \reimp
*/
void TQLineEdit::mouseReleaseEvent( TQMouseEvent* e )
{
    if ( sendMouseEventToInputContext( e ) )
	return;
#ifndef TQT_NO_DRAGANDDROP
    if ( e->button() == LeftButton ) {
	if ( d->dndTimer ) {
	    killTimer( d->dndTimer );
	    d->dndTimer = 0;
	    deselect();
	    return;
	}
    }
#endif
#ifndef TQT_NO_CLIPBOARD
    if (TQApplication::clipboard()->supportsSelection() ) {
	if ( e->button() == LeftButton ) {
	    d->copy( false );
	} else if ( !d->readOnly && e->button() == MidButton ) {
	    d->deselect();
	    insert( TQApplication::clipboard()->text( TQClipboard::Selection ) );
	}
    }
#endif
}

/*! \reimp
*/
void TQLineEdit::mouseDoubleClickEvent( TQMouseEvent* e )
{
    if ( sendMouseEventToInputContext( e ) )
	return;
    if ( e->button() == TQt::LeftButton ) {
	deselect();
	d->cursor = d->xToPos( e->pos().x() );
	d->cursor = d->textLayout.previousCursorPosition( d->cursor, TQTextLayout::SkipWords );
	// ## text layout should support end of words.
	int end = d->textLayout.nextCursorPosition( d->cursor, TQTextLayout::SkipWords );
	while ( end > d->cursor && d->text[end-1].isSpace() )
	    --end;
	d->moveCursor( end, true );
	d->tripleClickTimer = startTimer( TQApplication::doubleClickInterval() );
	d->tripleClick = e->pos();
    }
}

/*!
    \fn void  TQLineEdit::returnPressed()

    This signal is emitted when the Return or Enter key is pressed.
    Note that if there is a validator() or inputMask() set on the line
    edit, the returnPressed() signal will only be emitted if the input
    follows the inputMask() and the validator() returns \c Acceptable.
*/

/*!
    Converts key press event \a e into a line edit action.

    If Return or Enter is pressed and the current text is valid (or
    can be \link TQValidator::fixup() made valid\endlink by the
    validator), the signal returnPressed() is emitted.

    The default key bindings are listed in the \link #desc detailed
    description.\endlink
*/

void TQLineEdit::keyPressEvent( TQKeyEvent * e )
{
    d->setCursorVisible( true );
    if ( e->key() == Key_Enter || e->key() == Key_Return ) {
	const TQValidator * v = d->validator;
	if ( hasAcceptableInput() ) {
	    emit returnPressed();
	}
#ifndef TQT_NO_VALIDATOR
	else if ( v && v->validate( d->text, d->cursor ) != TQValidator::Acceptable ) {
	    TQString vstr = d->text;
	    v->fixup( vstr );
	    if ( vstr != d->text ) {
		setText( vstr );
		if ( hasAcceptableInput() )
		    emit returnPressed();
	    }
	}
#endif
	e->ignore();
	return;
    }
    if ( !d->readOnly ) {
	TQString t = e->text();
	if ( !t.isEmpty() && (!e->ascii() || e->ascii()>=32) &&
	     e->key() != Key_Delete &&
	     e->key() != Key_Backspace ) {
#ifdef TQ_WS_X11
	    extern bool tqt_hebrew_keyboard_hack;
	    if ( tqt_hebrew_keyboard_hack ) {
		// the X11 keyboard layout is broken and does not reverse
		// braces correctly. This is a hack to get halfway correct
		// behaviour
		if ( d->isRightToLeft() ) {
		    TQChar *c = (TQChar *)t.unicode();
		    int l = t.length();
		    while( l-- ) {
			if ( c->mirrored() )
			    *c = c->mirroredChar();
			c++;
		    }
		}
	    }
#endif
	    insert( t );
	    return;
	}
    }
    bool unknown = false;
    if ( e->state() & ControlButton ) {
	switch ( e->key() ) {
	case Key_A:
#if defined(TQ_WS_X11)
	    home( e->state() & ShiftButton );
#else
	    selectAll();
#endif
	    break;
	case Key_B:
	    cursorForward( e->state() & ShiftButton, -1 );
	    break;
#ifndef TQT_NO_CLIPBOARD
	case Key_C:
	    copy();
	    break;
#endif
	case Key_D:
	    if ( !d->readOnly ) {
		del();
	    }
	    break;
	case Key_E:
	    end( e->state() & ShiftButton );
	    break;
	case Key_F:
	    cursorForward( e->state() & ShiftButton, 1 );
	    break;
	case Key_H:
	    if ( !d->readOnly ) {
		backspace();
	    }
	    break;
	case Key_K:
	    if ( !d->readOnly ) {
		int priorState = d->undoState;
		d->deselect();
		while ( d->cursor < (int) d->text.length() )
		    d->del();
		d->finishChange( priorState );
	    }
	    break;
#if defined(TQ_WS_X11)
        case Key_U:
	    if ( !d->readOnly )
		clear();
	    break;
#endif
#ifndef TQT_NO_CLIPBOARD
	case Key_V:
	    if ( !d->readOnly )
		paste();
	    break;
	case Key_X:
	    if ( !d->readOnly && d->hasSelectedText() && echoMode() == Normal ) {
		copy();
		del();
	    }
	    break;
#if defined (TQ_WS_WIN)
	case Key_Insert:
	    copy();
	    break;
#endif
#endif
	case Key_Delete:
	    if ( !d->readOnly ) {
		cursorWordForward( true );
		del();
	    }
	    break;
	case Key_Backspace:
	    if ( !d->readOnly ) {
		cursorWordBackward( true );
		del();
	    }
	    break;
	case Key_Right:
	case Key_Left:
	    if ( d->isRightToLeft() == (e->key() == Key_Right) ) {
	        if (( echoMode() == Normal ) || ( echoMode() == Password ))
		    cursorWordBackward( e->state() & ShiftButton );
		else
		    home( e->state() & ShiftButton );
	    } else {
		if (( echoMode() == Normal ) || ( echoMode() == Password ))
		    cursorWordForward( e->state() & ShiftButton );
		else
		    end( e->state() & ShiftButton );
	    }
	    break;
	case Key_Z:
	    if ( !d->readOnly ) {
		if(e->state() & ShiftButton)
		    redo();
		else
		    undo();
	    }
	    break;
	case Key_Y:
	    if ( !d->readOnly )
		redo();
	    break;
	default:
	    unknown = true;
	}
    } else { // ### check for *no* modifier
	switch ( e->key() ) {
	case Key_Shift:
	    // ### TODO
	    break;
	case Key_Left:
	case Key_Right: {
	    int step =  (d->isRightToLeft() == (e->key() == Key_Right)) ? -1 : 1;
	    cursorForward( e->state() & ShiftButton, step );
	}
	break;
	case Key_Backspace:
	    if ( !d->readOnly ) {
		backspace();
	    }
	    break;
	case Key_Home:
#ifdef TQ_WS_MACX
	case Key_Up:
#endif
	    home( e->state() & ShiftButton );
	    break;
	case Key_End:
#ifdef TQ_WS_MACX
	case Key_Down:
#endif
	    end( e->state() & ShiftButton );
	    break;
	case Key_Delete:
	    if ( !d->readOnly ) {
#if defined (TQ_WS_WIN)
		if ( e->state() & ShiftButton ) {
		    cut();
		    break;
		}
#endif
		del();
	    }
	    break;
#if defined (TQ_WS_WIN)
	case Key_Insert:
	    if ( !d->readOnly && e->state() & ShiftButton )
		paste();
	    else
		unknown = true;
	    break;
#endif
	case Key_F14: // Undo key on Sun keyboards
	    if ( !d->readOnly )
		undo();
	    break;
#ifndef TQT_NO_CLIPBOARD
	case Key_F16: // Copy key on Sun keyboards
	    copy();
	    break;
	case Key_F18: // Paste key on Sun keyboards
	    if ( !d->readOnly )
		paste();
	    break;
	case Key_F20: // Cut key on Sun keyboards
	    if ( !d->readOnly && hasSelectedText() && echoMode() == Normal ) {
		copy();
		del();
	    }
	    break;
#endif
	default:
	    unknown = true;
	}
    }
    if ( e->key() == Key_Direction_L || e->key() == Key_Direction_R ) {
	d->direction = (e->key() == Key_Direction_L) ? TQChar::DirL : TQChar::DirR;
        d->updateTextLayout();
	update();
    }

    if ( unknown )
	e->ignore();
}


/*!
  This function is not intended as polymorphic usage. Just a shared code
  fragment that calls TQWidget::sendMouseEventToInputContext() easily for this
  class.
 */
bool TQLineEdit::sendMouseEventToInputContext( TQMouseEvent *e )
{
#ifndef TQT_NO_IM
    if ( d->composeMode() ) {
	int cursor = d->xToPosInternal( e->pos().x(), TQTextItem::OnCharacters );
	int mousePos = cursor - d->imstart;
	if ( mousePos >= 0 && mousePos < d->preeditLength() ) {
	    TQWidget::sendMouseEventToInputContext( mousePos, e->type(),
						   e->button(), e->state() );
	} else if ( e->type() != TQEvent::MouseMove ) {
	    // send button events on out of preedit
	    TQWidget::sendMouseEventToInputContext( -1, e->type(),
						   e->button(), e->state() );
	}
	return true;
    }
#endif
    return false;
}


/*! \reimp
 */
void TQLineEdit::imStartEvent( TQIMEvent *e )
{
    if ( d->readOnly ) {
	e->ignore();
	return;
    }
    d->removeSelectedText();
    d->updateMicroFocusHint();
    d->imstart = d->imend = d->imselstart = d->imselend = d->cursor;
}

/*! \reimp
 */
void TQLineEdit::imComposeEvent( TQIMEvent *e )
{
    if ( d->readOnly ) {
	e->ignore();
	return;
    }
    d->text.replace( d->imstart, d->imend - d->imstart, e->text() );
    d->imend = d->imstart + e->text().length();
    d->imselstart = d->imstart + e->cursorPos();
    d->imselend = d->imselstart + e->selectionLength();
    d->cursor = d->imstart + e->cursorPos();
    d->updateTextLayout();
    d->updateMicroFocusHint();
    update();
}

/*! \reimp
 */
void TQLineEdit::imEndEvent( TQIMEvent *e )
{
    if ( d->readOnly) {
	e->ignore();
	return;
    }
    d->text.remove( d->imstart, d->imend - d->imstart );
    d->cursor = d->imselstart = d->imselend = d->imend = d->imstart;
    d->textDirty = true;
    insert( e->text() );
}

/*!\reimp
*/

void TQLineEdit::focusInEvent( TQFocusEvent* )
{
    if ( TQFocusEvent::reason() == TQFocusEvent::Tab ||
	 TQFocusEvent::reason() == TQFocusEvent::Backtab  ||
	 TQFocusEvent::reason() == TQFocusEvent::Shortcut )
	d->maskData ? d->moveCursor( d->nextMaskBlank( 0 ) ) : selectAll();
    if ( !d->cursorTimer ) {
	int cft = TQApplication::cursorFlashTime();
	d->cursorTimer = cft ? startTimer( cft/2 ) : -1;
    }
    if( !hasSelectedText() || style().styleHint( TQStyle::SH_BlinkCursorWhenTextSelected ) )
	d->setCursorVisible( true );
    if ( d->hasIMSelection() )
	d->cursor = d->imselstart;
    d->updateMicroFocusHint();
}

/*!\reimp
*/

void TQLineEdit::focusOutEvent( TQFocusEvent* )
{
    if ( TQFocusEvent::reason() != TQFocusEvent::ActiveWindow &&
	 TQFocusEvent::reason() != TQFocusEvent::Popup )
	deselect();
    d->setCursorVisible( false );
    if ( d->cursorTimer > 0 )
	killTimer( d->cursorTimer );
    d->cursorTimer = 0;
    if (TQFocusEvent::reason() != TQFocusEvent::Popup)
        emit lostFocus();
}

/*!\reimp
*/
void TQLineEdit::drawContents( TQPainter *p )
{
    const TQColorGroup& cg = colorGroup();
    TQRect cr = contentsRect();
    TQFontMetrics fm = fontMetrics();
    TQRect lineRect( cr.x() + innerMargin, cr.y() + (cr.height() - fm.height() + 1) / 2,
		    cr.width() - 2*innerMargin, fm.height() );
    TQBrush bg = TQBrush( paletteBackgroundColor() );
    if ( paletteBackgroundPixmap() )
	bg = TQBrush( cg.background(), *paletteBackgroundPixmap() );
    else if ( !isEnabled() )
	bg = cg.brush( TQColorGroup::Background );
    TQPoint brushOrigin = p->brushOrigin();
    p->save();
    p->setClipRegion( TQRegion(cr) - lineRect );
    p->setBrushOrigin(brushOrigin - backgroundOffset());
    p->fillRect( cr, bg );
    p->restore();
    TQSharedDoubleBuffer buffer( p, lineRect.x(), lineRect.y(),
 				lineRect.width(), lineRect.height(),
 				hasFocus() ? TQSharedDoubleBuffer::Force : 0 );
    p = buffer.painter();
    brushOrigin = p->brushOrigin();
    p->setBrushOrigin(brushOrigin - backgroundOffset());
    p->fillRect( lineRect, bg );
    p->setBrushOrigin(brushOrigin);

    // locate cursor position
    int cix = 0;
    TQTextItem ci = d->textLayout.findItem( d->cursor );
    if ( ci.isValid() ) {
	if ( d->cursor != (int)d->text.length() && d->cursor == ci.from() + ci.length()
	     && ci.isRightToLeft() != d->isRightToLeft() )
	    ci = d->textLayout.findItem( d->cursor + 1 );
	cix = ci.x() + ci.cursorToX( d->cursor - ci.from() );
    }

    // horizontal scrolling
    int minLB = TQMAX( 0, -fm.minLeftBearing() );
    int minRB = TQMAX( 0, -fm.minRightBearing() );
    int widthUsed = d->textLayout.widthUsed() + 1 + minRB;
    if ( (minLB + widthUsed) <=  lineRect.width() ) {
	switch ( d->visualAlignment() ) {
	case AlignRight:
	    d->hscroll = widthUsed - lineRect.width() + 1;
	    break;
	case AlignHCenter:
	    d->hscroll = ( widthUsed - lineRect.width() ) / 2;
	    break;
	default:
	    d->hscroll = 0;
	    break;
	}
	d->hscroll -= minLB;
    } else if ( cix - d->hscroll >= lineRect.width() ) {
	d->hscroll = cix - lineRect.width() + 1;
    } else if ( cix - d->hscroll < 0 ) {
	d->hscroll = cix;
    } else if ( widthUsed - d->hscroll < lineRect.width() ) {
	d->hscroll = widthUsed - lineRect.width() + 1;
    } else if (d->hscroll < 0) {
        d->hscroll = 0;
    }
    // This updateMicroFocusHint() is corresponding to update() at
    // IMCompose event. Although the function is invoked from various
    // other points, some situations such as "candidate selection on
    // AlignHCenter'ed text" need this invocation because
    // updateMicroFocusHint() requires updated contentsRect(), and
    // there are no other chances in such situation that invoke the
    // function.
    d->updateMicroFocusHint();
    // the y offset is there to keep the baseline constant in case we have script changes in the text.
    TQPoint topLeft = lineRect.topLeft() - TQPoint(d->hscroll, d->ascent-fm.ascent());

    // draw text, selections and cursors
    p->setPen( cg.text() );
    bool supressCursor = d->readOnly, hasRightToLeft = d->isRightToLeft();
    int textflags = 0;
    if ( font().underline() )
	textflags |= TQt::Underline;
    if ( font().strikeOut() )
	textflags |= TQt::StrikeOut;
    if ( font().overline() )
	textflags |= TQt::Overline;

    for ( int i = 0; i < d->textLayout.numItems(); i++ ) {
	TQTextItem ti = d->textLayout.itemAt( i );
	hasRightToLeft |= ti.isRightToLeft();
	int tix = topLeft.x() + ti.x();
	int first = ti.from();
	int last = ti.from() + ti.length() - 1;

	// text and selection
	if ( d->selstart < d->selend && (last >= d->selstart && first < d->selend ) ) {
	    TQRect highlight = TQRect( TQPoint( tix + ti.cursorToX( TQMAX( d->selstart - first, 0 ) ),
					     lineRect.top() ),
				     TQPoint( tix + ti.cursorToX( TQMIN( d->selend - first, last - first + 1 ) ) - 1,
					     lineRect.bottom() ) ).normalize();
	    p->save();
  	    p->setClipRegion( TQRegion( lineRect ) - highlight, TQPainter::CoordPainter );
 	    p->drawTextItem( topLeft, ti, textflags );
 	    p->setClipRect( lineRect & highlight, TQPainter::CoordPainter );
	    p->fillRect( highlight, cg.highlight() );
 	    p->setPen( cg.highlightedText() );
	    p->drawTextItem( topLeft, ti, textflags );
	    p->restore();
	} else {
	    p->drawTextItem( topLeft, ti, textflags );
	}

	// input method edit area
	if ( d->composeMode() && (last >= d->imstart && first < d->imend ) ) {
	    TQRect highlight = TQRect( TQPoint( tix + ti.cursorToX( TQMAX( d->imstart - first, 0 ) ), lineRect.top() ),
			      TQPoint( tix + ti.cursorToX( TQMIN( d->imend - first, last - first + 1 ) )-1, lineRect.bottom() ) ).normalize();
	    p->save();
 	    p->setClipRect( lineRect & highlight, TQPainter::CoordPainter );

	    int h1, s1, v1, h2, s2, v2;
	    cg.color( TQColorGroup::Base ).hsv( &h1, &s1, &v1 );
	    cg.color( TQColorGroup::Background ).hsv( &h2, &s2, &v2 );
	    TQColor imCol;
	    imCol.setHsv( h1, s1, ( v1 + v2 ) / 2 );
	    p->fillRect( highlight, imCol );
	    p->drawTextItem( topLeft, ti, textflags );
            // draw preedit's underline
            if (d->imend - d->imstart > 0) {
                p->setPen( cg.text() );
                p->drawLine( highlight.bottomLeft(), highlight.bottomRight() );
            }
	    p->restore();
	}

	// input method selection
	if ( d->hasIMSelection() && (last >= d->imselstart && first < d->imselend ) ) {
	    TQRect highlight = TQRect( TQPoint( tix + ti.cursorToX( TQMAX( d->imselstart - first, 0 ) ), lineRect.top() ),
			      TQPoint( tix + ti.cursorToX( TQMIN( d->imselend - first, last - first + 1 ) )-1, lineRect.bottom() ) ).normalize();
	    p->save();
	    p->setClipRect( lineRect & highlight, TQPainter::CoordPainter );
	    p->fillRect( highlight, cg.text() );
	    p->setPen( paletteBackgroundColor() );
	    p->drawTextItem( topLeft, ti, textflags );
	    p->restore();
            supressCursor = true;
	}

	// overwrite cursor
	if ( d->cursorVisible && d->maskData &&
	     d->selend <= d->selstart && (last >= d->cursor && first <= d->cursor ) ) {
	    TQRect highlight = TQRect( TQPoint( tix + ti.cursorToX( TQMAX( d->cursor - first, 0 ) ), lineRect.top() ),
				     TQPoint( tix + ti.cursorToX( TQMIN( d->cursor + 1 - first, last - first + 1 ) )-1, lineRect.bottom() ) ).normalize();
	    p->save();
	    p->setClipRect( lineRect & highlight, TQPainter::CoordPainter );
	    p->fillRect( highlight, cg.text() );
	    p->setPen( paletteBackgroundColor() );
	    p->drawTextItem( topLeft, ti, textflags );
	    p->restore();
	    supressCursor = true;
	}
    }

    // draw cursor
    // 
    // Asian users regard IM selection text as cursor on candidate
    // selection phase of input method, so ordinary cursor should be
    // invisible if IM selection text exists.
    if ( d->cursorVisible && !supressCursor && !d->hasIMSelection() && (d->echoMode != PasswordThreeStars) ) {
	TQPoint from( topLeft.x() + cix, lineRect.top() );
	TQPoint to = from + TQPoint( 0, lineRect.height() );
	p->drawLine( from, to );
	if ( hasRightToLeft ) {
	    bool rtl = ci.isValid() ? ci.isRightToLeft() : true;
	    to = from + TQPoint( (rtl ? -2 : 2), 2 );
	    p->drawLine( from, to );
	    from.ry() += 4;
	    p->drawLine( from, to );
	}
    }
    buffer.end();
}


#ifndef TQT_NO_DRAGANDDROP
/*!\reimp
*/
void TQLineEdit::dragMoveEvent( TQDragMoveEvent *e )
{
    if ( !d->readOnly && TQTextDrag::canDecode(e) ) {
	e->acceptAction();
	d->cursor = d->xToPos( e->pos().x() );
	d->cursorVisible = true;
	update();
    }
}

/*!\reimp */
void TQLineEdit::dragEnterEvent( TQDragEnterEvent * e )
{
    TQLineEdit::dragMoveEvent( e );
}

/*!\reimp */
void TQLineEdit::dragLeaveEvent( TQDragLeaveEvent *)
{
    if ( d->cursorVisible ) {
	d->cursorVisible = false;
	update();
    }
}

/*!\reimp */
void TQLineEdit::dropEvent( TQDropEvent* e )
{
    TQString str;
    // try text/plain
    TQCString plain = "plain";
    bool decoded = TQTextDrag::decode(e, str, plain);
    // otherwise we'll accept any kind of text (like text/uri-list)
    if (! decoded)
	decoded = TQTextDrag::decode(e, str);

    if ( decoded && !d->readOnly ) {
	if ( e->source() == this && e->action() == TQDropEvent::Copy )
 	    deselect();
	d->cursor =d->xToPos( e->pos().x() );
	int selStart = d->cursor;
	int oldSelStart = d->selstart;
	int oldSelEnd = d->selend;
	d->cursorVisible = false;
	e->acceptAction();
	insert( str );
	if ( e->source() == this ) {
	    if ( e->action() == TQDropEvent::Move ) {
		if ( selStart > oldSelStart && selStart <= oldSelEnd )
		    setSelection( oldSelStart, str.length() );
		else if ( selStart > oldSelEnd )
		    setSelection( selStart - str.length(), str.length() );
		else
		    setSelection( selStart, str.length() );
	    } else {
		setSelection( selStart, str.length() );
	    }
	}
    } else {
	e->ignore();
	update();
    }
}

bool TQLineEditPrivate::drag()
{
    q->killTimer( dndTimer );
    dndTimer = 0;
    TQTextDrag *tdo = new TQTextDrag( q->selectedText(), q );

    TQGuardedPtr<TQLineEdit> gptr = q;
    bool r = tdo->drag();
    if ( !gptr )
        return false;

    // ### fix the check TQDragObject::target() != q in TQt4 (should not be needed)
    if ( r && !readOnly && TQDragObject::target() != q ) {
	int priorState = undoState;
	removeSelectedText();
	finishChange( priorState );
    }
#ifndef TQT_NO_CURSOR
    q->setCursor( readOnly ? arrowCursor : ibeamCursor );
#endif
    return true;
}

#endif // TQT_NO_DRAGANDDROP

enum { IdUndo, IdRedo, IdSep1, IdCut, IdCopy, IdPaste, IdClear, IdSep2, IdSelectAll };

/*!\reimp
*/
void TQLineEdit::contextMenuEvent( TQContextMenuEvent * e )
{
#ifndef TQT_NO_POPUPMENU
#ifndef TQT_NO_IM
    if ( d->composeMode() )
	return;
#endif
    d->separate();
    TQPopupMenu *menu = createPopupMenu();
    if (!menu)
        return;
    TQGuardedPtr<TQPopupMenu> popup = menu;
    TQGuardedPtr<TQLineEdit> that = this;
    TQPoint pos = e->reason() == TQContextMenuEvent::Mouse ? e->globalPos() :
		 mapToGlobal( TQPoint(e->pos().x(), 0) ) + TQPoint( width() / 2, height() / 2 );
    int r = popup->exec( pos );
    delete (TQPopupMenu*)popup;
    if ( that && d->menuId ) {
	switch ( d->menuId - r ) {
	case IdClear: clear(); break;
	case IdSelectAll: selectAll(); break;
	case IdUndo: undo(); break;
	case IdRedo: redo(); break;
#ifndef TQT_NO_CLIPBOARD
	case IdCut: cut(); break;
	case IdCopy: copy(); break;
	case IdPaste: paste(); break;
#endif
	default:
	    ; // nothing selected or lineedit destroyed. Be careful.
	}
    }
#endif //TQT_NO_POPUPMENU
}

/*!
    This function is called to create the popup menu which is shown
    when the user clicks on the line edit with the right mouse button.
    If you want to create a custom popup menu, reimplement this
    function and return the popup menu you create. The popup menu's
    ownership is transferred to the caller.
*/

TQPopupMenu *TQLineEdit::createPopupMenu()
{
#ifndef TQT_NO_POPUPMENU
    TQPopupMenu *popup = new TQPopupMenu( this, "qt_edit_menu" );
    int id = d->menuId = popup->insertItem( tr( "&Undo" ) + ACCEL_KEY( Z ) );
    popup->insertItem( tr( "&Redo" ) + ACCEL_KEY( Y ) );
    popup->insertSeparator();
    popup->insertItem( tr( "Cu&t" ) + ACCEL_KEY( X ) );
    popup->insertItem( tr( "&Copy" ) + ACCEL_KEY( C ) );
    popup->insertItem( tr( "&Paste" ) + ACCEL_KEY( V ) );
    popup->insertItem( tr( "Clear" ) );
    popup->insertSeparator();
    popup->insertItem( tr( "Select All" )
#ifndef TQ_WS_X11
    + ACCEL_KEY( A )
#endif
	);

#ifndef TQT_NO_IM
    TQInputContext *qic = getInputContext();
    if ( qic )
	qic->addMenusTo( popup );
#endif
    
    popup->setItemEnabled( id - IdUndo, d->isUndoAvailable() );
    popup->setItemEnabled( id - IdRedo, d->isRedoAvailable() );
#ifndef TQT_NO_CLIPBOARD
    popup->setItemEnabled( id - IdCut, !d->readOnly && d->hasSelectedText() );
    popup->setItemEnabled( id - IdCopy, d->hasSelectedText() );
    popup->setItemEnabled( id - IdPaste, !d->readOnly && !TQApplication::clipboard()->text().isEmpty() );
#else
    popup->setItemVisible( id - IdCut, false );
    popup->setItemVisible( id - IdCopy, false );
    popup->setItemVisible( id - IdPaste, false );
#endif
    popup->setItemEnabled( id - IdClear, !d->readOnly && !d->text.isEmpty() );
    popup->setItemEnabled( id - IdSelectAll, !d->text.isEmpty() && !d->allSelected() );
    return popup;
#else
    return 0;
#endif
}

/*! \reimp */
void TQLineEdit::windowActivationChange( bool b )
{
    //### remove me with WHighlightSelection attribute
    if ( palette().active() != palette().inactive() )
	update();
    TQWidget::windowActivationChange( b );
}

/*! \reimp */

void TQLineEdit::setPalette( const TQPalette & p )
{
    //### remove me with WHighlightSelection attribute
    TQWidget::setPalette( p );
    update();
}

/*!
  \obsolete
  \fn void TQLineEdit::repaintArea( int from, int to )
  Repaints all characters from \a from to \a to. If cursorPos is
  between from and to, ensures that cursorPos is visible.
*/

/*! \reimp
 */
void TQLineEdit::setFont( const TQFont & f )
{
    TQWidget::setFont( f );
    d->updateTextLayout();
}

/*! \obsolete
*/
int TQLineEdit::characterAt( int xpos, TQChar *chr ) const
{
    int pos = d->xToPos( xpos + contentsRect().x() - d->hscroll + innerMargin );
    if ( chr && pos < (int) d->text.length() )
	*chr = d->text.at( pos );
    return pos;
}

/*!
    \internal

    Sets the password character to \a c.

    \sa passwordChar()
*/

void TQLineEdit::setPasswordChar( TQChar c )
{
    d->passwordChar = c;
}

/*!
    \internal

    Returns the password character.

    \sa setPasswordChar()
*/
TQChar TQLineEdit::passwordChar() const
{
    return ( d->passwordChar.isNull() ? TQChar( style().styleHint( TQStyle::SH_LineEdit_PasswordCharacter, this ) ) : d->passwordChar );
}

void TQLineEdit::clipboardChanged()
{
}

void TQLineEditPrivate::init( const TQString& txt )
{
#ifndef TQT_NO_CURSOR
    q->setCursor( readOnly ? arrowCursor : ibeamCursor );
#endif
    q->setFocusPolicy( TQWidget::StrongFocus );
    q->setInputMethodEnabled( true );
    //   Specifies that this widget can use more, but is able to survive on
    //   less, horizontal space; and is fixed vertically.
    q->setSizePolicy( TQSizePolicy( TQSizePolicy::Expanding, TQSizePolicy::Fixed ) );
    q->setBackgroundMode( PaletteBase );
    q->setKeyCompression( true );
    q->setMouseTracking( true );
    q->setAcceptDrops( true );
    q->setFrame( true );
    text = txt;
    updateTextLayout();
    cursor = text.length();
}

void TQLineEditPrivate::updateTextLayout()
{
    // replace all non-printable characters with spaces (to avoid
    // drawing boxes when using fonts that don't have glyphs for such
    // characters)
    const TQString &displayText = q->displayText();
    TQString str(displayText.unicode(), displayText.length());
    TQChar* uc = (TQChar*)str.unicode();
    for (int i = 0; i < (int)str.length(); ++i) {
	if (! uc[i].isPrint())
	    uc[i] = TQChar(0x0020);
    }
    textLayout.setText( str, q->font() );
    textLayout.setDirection((TQChar::Direction)direction);
    textLayout.beginLayout(TQTextLayout::SingleLine);
    textLayout.beginLine( INT_MAX );
    while ( !textLayout.atEnd() )
	textLayout.addCurrentItem();
    ascent = 0;
    textLayout.endLine(0, 0, TQt::AlignLeft|TQt::SingleLine, &ascent);
}

int TQLineEditPrivate::xToPosInternal( int x, TQTextItem::CursorPosition betweenOrOn ) const
{
    x-= q->contentsRect().x() - hscroll + innerMargin;
    for ( int i = 0; i < textLayout.numItems(); ++i ) {
	TQTextItem ti = textLayout.itemAt( i );
	TQRect tir = ti.rect();
	if ( x >= tir.left() && x <= tir.right() )
	    return ti.xToCursor( x - tir.x(), betweenOrOn ) + ti.from();
    }
    return x < 0 ? -1 : text.length();
}

int TQLineEditPrivate::xToPos( int x, TQTextItem::CursorPosition betweenOrOn ) const
{
    int pos = xToPosInternal( x, betweenOrOn );
    return ( pos < 0 ) ? 0 : pos;
}


TQRect TQLineEditPrivate::cursorRect() const
{
    TQRect cr = q->contentsRect();
    int cix = cr.x() - hscroll + innerMargin;
    TQTextItem ci = textLayout.findItem( cursor );
    if ( ci.isValid() ) {
	if ( cursor != (int)text.length() && cursor == ci.from() + ci.length()
	     && ci.isRightToLeft() != isRightToLeft() )
	    ci = textLayout.findItem( cursor + 1 );
	cix += ci.x() + ci.cursorToX( cursor - ci.from() );
    }
    int ch = q->fontMetrics().height();
    return TQRect( cix-4, cr.y() + ( cr.height() -  ch + 1) / 2, 8, ch + 1 );
}

void TQLineEditPrivate::updateMicroFocusHint()
{
    // To reduce redundant microfocus update notification, we remember
    // the old rect and update the microfocus if actual update is
    // required. The rect o is intentionally static because some
    // notifyee requires the microfocus information as global update
    // rather than per notifyee update to place shared widget around
    // microfocus.
    static TQRect o;
    if ( q->hasFocus() ) {
        TQRect r = cursorRect();
        if ( o != r ) {
	    o = r;
	    q->setMicroFocusHint( r.x(), r.y(), r.width(), r.height() );
	}
    }
}

void TQLineEditPrivate::moveCursor( int pos, bool mark )
{
    if ( pos != cursor )
	separate();
    if ( maskData && pos > cursor )
	pos = nextMaskBlank( pos );
    else if ( maskData && pos < cursor )
	pos = prevMaskBlank( pos );
    bool fullUpdate = mark || hasSelectedText();
    if ( mark ) {
	int anchor;
	if ( selend > selstart && cursor == selstart )
	    anchor = selend;
	else if ( selend > selstart && cursor == selend )
	    anchor = selstart;
	else
	    anchor = cursor;
	selstart = TQMIN( anchor, pos );
	selend = TQMAX( anchor, pos );
    } else {
	deselect();
    }
    if ( fullUpdate ) {
	cursor = pos;
	q->update();
    } else {
	setCursorVisible( false );
	cursor = pos;
	setCursorVisible( true );
    }
    updateMicroFocusHint();
    if ( mark && !q->style().styleHint( TQStyle::SH_BlinkCursorWhenTextSelected ) )
	setCursorVisible( false );
    if ( mark || selDirty ) {
	selDirty = false;
	emit q->selectionChanged();
    }
}

void TQLineEditPrivate::finishChange( int validateFromState, bool setModified )
{
    bool lineDirty = selDirty;
    if ( textDirty ) {
	// do validation
	bool wasValidInput = validInput;
	validInput = true;
#ifndef TQT_NO_VALIDATOR
	if ( validator && validateFromState >= 0 ) {
	    TQString textCopy = text;
	    int cursorCopy = cursor;
	    validInput = ( validator->validate( textCopy, cursorCopy ) != TQValidator::Invalid );
	    if ( validInput ) {
		if ( text != textCopy ) {
		    q->setText( textCopy );
		    cursor = cursorCopy;
		    return;
		}
		cursor = cursorCopy;
	    }
	}
#endif
	if ( validateFromState >= 0 && wasValidInput && !validInput ) {
	    undo( validateFromState );
	    history.resize( undoState );
	    validInput = true;
	    textDirty = setModified = false;
	}
	updateTextLayout();
	updateMicroFocusHint();
	lineDirty |= textDirty;
	if ( setModified )
	    modified = true;
	if ( textDirty ) {
	    textDirty = false;
	    emit q->textChanged( maskData ? stripString(text) : text );
	}
#if defined(QT_ACCESSIBILITY_SUPPORT)
	TQAccessible::updateAccessibility( q, 0, TQAccessible::ValueChanged );
#endif
    }
    if ( selDirty ) {
	selDirty = false;
	emit q->selectionChanged();
    }
    if ( lineDirty || !setModified )
	q->update();
}

void TQLineEditPrivate::setText( const TQString& txt )
{
    deselect();
    TQString oldText = text;
    if ( maskData ) {
	text = maskString( 0, txt, true );
	text += clearString( text.length(), maxLength - text.length() );
    } else {
 	text = txt.isEmpty() ? txt : txt.left( maxLength );
    }
    history.clear();
    undoState = 0;
    cursor = text.length();
    textDirty = ( oldText != text );
}


void TQLineEditPrivate::setCursorVisible( bool visible )
{
    if ( (bool)cursorVisible == visible )
	return;
    if ( cursorTimer )
	cursorVisible = visible;
    TQRect r = cursorRect();
    if ( maskData || !q->contentsRect().contains( r ) )
	q->update();
    else
	q->update( r );
}

void TQLineEditPrivate::addCommand( const Command& cmd )
{
    if ( separator && undoState && history[undoState-1].type != Separator ) {
	history.resize( undoState + 2 );
	history[undoState++] = Command( Separator, 0, 0 );
    } else {
	history.resize( undoState + 1);
    }
    separator = false;
    history[ undoState++ ] = cmd;
}

void TQLineEditPrivate::insert( const TQString& s )
{
    if ( maskData ) {
	TQString ms = maskString( cursor, s );
	for ( int i = 0; i < (int) ms.length(); ++i ) {
	    addCommand ( Command( DeleteSelection, cursor+i, text.at(cursor+i) ) );
	    addCommand( Command( Insert, cursor+i, ms.at(i) ) );
	}
	text.replace( cursor, ms.length(), ms );
	cursor += ms.length();
	cursor = nextMaskBlank( cursor );
    } else {
	int remaining = maxLength - text.length();
	text.insert( cursor, s.left(remaining) );
	for ( int i = 0; i < (int) s.left(remaining).length(); ++i )
	    addCommand( Command( Insert, cursor++, s.at(i) ) );
    }
    textDirty = true;
}

void TQLineEditPrivate::del( bool wasBackspace )
{
    if ( cursor < (int) text.length() ) {
	addCommand ( Command( (CommandType)((maskData?2:0)+(wasBackspace?Remove:Delete)), cursor, text.at(cursor) ) );
	if ( maskData ) {
	    text.replace( cursor, 1, clearString( cursor, 1 ) );
	    addCommand( Command( Insert, cursor, text.at( cursor ) ) );
	} else {
	    text.remove( cursor, 1 );
	}
	textDirty = true;
    }
}

void TQLineEditPrivate::removeSelectedText()
{
    if ( selstart < selend && selend <= (int) text.length() ) {
	separate();
	int i ;
	if ( selstart <= cursor && cursor < selend ) {
	    // cursor is within the selection. Split up the commands
	    // to be able to restore the correct cursor position
	    for ( i = cursor; i >= selstart; --i )
		addCommand ( Command( DeleteSelection, i, text.at(i) ) );
	    for ( i = selend - 1; i > cursor; --i )
		addCommand ( Command( DeleteSelection, i - cursor + selstart - 1, text.at(i) ) );
	} else {
	    for ( i = selend-1; i >= selstart; --i )
		addCommand ( Command( RemoveSelection, i, text.at(i) ) );
	}
	if ( maskData ) {
	    text.replace( selstart, selend - selstart,  clearString( selstart, selend - selstart ) );
	    for ( int i = 0; i < selend - selstart; ++i )
		addCommand( Command( Insert, selstart + i, text.at( selstart + i ) ) );
	} else {
	    text.remove( selstart, selend - selstart );
	}
	if ( cursor > selstart )
	    cursor -= TQMIN( cursor, selend ) - selstart;
	deselect();
	textDirty = true;
    }
}

void TQLineEditPrivate::parseInputMask( const TQString &maskFields )
{
    if ( maskFields.isEmpty() || maskFields.section( ';', 0, 0 ).isEmpty() ) {
	if ( maskData ) {
	    delete [] maskData;
	    maskData = 0;
	    maxLength = 32767;
	    q->setText( TQString::null );
	}
	return;
    }

    inputMask =  maskFields.section( ';', 0, 0 );
    blank = maskFields.section( ';', 1, 1 ).at(0);
    if ( blank.isNull() )
	blank = ' ';

    // calculate maxLength / maskData length
    maxLength = 0;
    TQChar c = 0;
    uint i;
    for ( i=0; i<inputMask.length(); i++ ) {
	c = inputMask.at(i);
	if ( i > 0 && inputMask.at( i-1 ) == '\\' ) {
	    maxLength++;
	    continue;
	}
	if ( c != '\\' && c != '!' &&
	     c != '<' && c != '>' &&
	     c != '{' && c != '}' &&
	     c != '[' && c != ']' )
	    maxLength++;
    }

    delete [] maskData;
    maskData = new MaskInputData[ maxLength ];

    MaskInputData::Casemode m = MaskInputData::NoCaseMode;
    c = 0;
    bool s;
    bool escape = false;
    int index = 0;
    for ( i = 0; i < inputMask.length(); i++ ) {
	c = inputMask.at(i);
	if ( escape ) {
	    s = true;
	    maskData[ index ].maskChar = c;
	    maskData[ index ].separator = s;
	    maskData[ index ].caseMode = m;
	    index++;
	    escape = false;
	} else if ( c == '<' || c == '>' || c == '!') {
	    switch ( c ) {
	    case '<':
		m = MaskInputData::Lower;
		break;
	    case '>':
		m = MaskInputData::Upper;
		break;
	    case '!':
		m = MaskInputData::NoCaseMode;
		break;
	    }
	} else if ( c != '{' && c != '}' && c != '[' && c != ']' ) {
	    switch ( c ) {
	    case 'A':
	    case 'a':
	    case 'N':
	    case 'n':
	    case 'X':
	    case 'x':
	    case '9':
	    case '0':
	    case 'D':
	    case 'd':
	    case '#':
		s = false;
		break;
	    case '\\':
		escape = true;
	    default:
		s = true;
		break;
	    }

	    if ( !escape ) {
		maskData[ index ].maskChar = c;
		maskData[ index ].separator = s;
		maskData[ index ].caseMode = m;
		index++;
	    }
	}
    }
    q->setText( TQString::null );
}


/* checks if the key is valid compared to the inputMask */
bool TQLineEditPrivate::isValidInput( TQChar key, TQChar mask ) const
{
    switch ( mask ) {
    case 'A':
	if ( key.isLetter() && key != blank )
	    return true;
	break;
    case 'a':
	if ( key.isLetter() || key == blank )
	    return true;
	break;
    case 'N':
	if ( key.isLetterOrNumber() && key != blank )
	    return true;
	break;
    case 'n':
	if ( key.isLetterOrNumber() || key == blank )
	    return true;
	break;
    case 'X':
	if ( key.isPrint() && key != blank )
	    return true;
	break;
    case 'x':
	if ( key.isPrint() || key == blank )
	    return true;
	break;
    case '9':
	if ( key.isNumber() && key != blank )
	    return true;
	break;
    case '0':
	if ( key.isNumber() || key == blank )
	    return true;
	break;
    case 'D':
	if ( key.isNumber() && key.digitValue() > 0 && key != blank )
	    return true;
	break;
    case 'd':
	if ( (key.isNumber() && key.digitValue() > 0) || key == blank )
	    return true;
	break;
    case '#':
	if ( key.isNumber() || key == '+' || key == '-' || key == blank )
	    return true;
	break;
    default:
	break;
    }
    return false;
}

/*
  Applies the inputMask on \a str starting from position \a pos in the mask. \a clear
  specifies from where characters should be gotten when a separator is met in \a str - true means
  that blanks will be used, false that previous input is used.
  Calling this when no inputMask is set is undefined.
*/
TQString TQLineEditPrivate::maskString( uint pos, const TQString &str, bool clear) const
{
    if ( pos >= (uint)maxLength )
	return TQString::fromLatin1("");

    TQString fill;
    fill = clear ? clearString( 0, maxLength ) : text;

    uint strIndex = 0;
    TQString s = TQString::fromLatin1("");
    int i = pos;
    while ( i < maxLength ) {
	if ( strIndex < str.length() ) {
	    if ( maskData[ i ].separator ) {
		s += maskData[ i ].maskChar;
		if ( str[(int)strIndex] == maskData[ i ].maskChar )
		    strIndex++;
		++i;
	    } else {
		if ( isValidInput( str[(int)strIndex], maskData[ i ].maskChar ) ) {
		    switch ( maskData[ i ].caseMode ) {
		    case MaskInputData::Upper:
			s += str[(int)strIndex].upper();
			break;
		    case MaskInputData::Lower:
			s += str[(int)strIndex].lower();
			break;
		    default:
			s += str[(int)strIndex];
		    }
		    ++i;
		} else {
		    // search for separator first
		    int n = findInMask( i, true, true, str[(int)strIndex] );
		    if ( n != -1 ) {
			if ( str.length() != 1 || i == 0 || (i > 0 && (!maskData[i-1].separator || maskData[i-1].maskChar != str[(int)strIndex])) ) {
			    s += fill.mid( i, n-i+1 );
			    i = n + 1; // update i to find + 1
			}
		    } else {
			// search for valid blank if not
			n = findInMask( i, true, false, str[(int)strIndex] );
			if ( n != -1 ) {
			    s += fill.mid( i, n-i );
			    switch ( maskData[ n ].caseMode ) {
			    case MaskInputData::Upper:
				s += str[(int)strIndex].upper();
				break;
			    case MaskInputData::Lower:
				s += str[(int)strIndex].lower();
				break;
			    default:
				s += str[(int)strIndex];
			    }
			    i = n + 1; // updates i to find + 1
			}
		    }
		}
		strIndex++;
	    }
	} else
	    break;
    }

    return s;
}



/*
  Returns a "cleared" string with only separators and blank chars.
  Calling this when no inputMask is set is undefined.
*/
TQString TQLineEditPrivate::clearString( uint pos, uint len ) const
{
    if ( pos >= (uint)maxLength )
	return TQString::null;

    TQString s;
    int end = TQMIN( (uint)maxLength, pos + len );
    for ( int i=pos; i<end; i++ )
	if ( maskData[ i ].separator )
	    s += maskData[ i ].maskChar;
	else
	    s += blank;

    return s;
}

/*
  Strips blank parts of the input in a TQLineEdit when an inputMask is set,
  separators are still included. Typically "127.0__.0__.1__" becomes "127.0.0.1".
*/
TQString TQLineEditPrivate::stripString( const TQString &str ) const
{
    if ( !maskData )
	return str;

    TQString s;
    int end = TQMIN( maxLength, (int)str.length() );
    for (int i=0; i < end; i++ )
	if ( maskData[ i ].separator )
	    s += maskData[ i ].maskChar;
	else
	    if ( str[i] != blank )
		s += str[i];

    return s;
}

/* searches forward/backward in maskData for either a separator or a blank */
int TQLineEditPrivate::findInMask( int pos, bool forward, bool findSeparator, TQChar searchChar ) const
{
    if ( pos >= maxLength || pos < 0 )
	return -1;

    int end = forward ? maxLength : -1;
    int step = forward ? 1 : -1;
    int i = pos;

    while ( i != end ) {
	if ( findSeparator ) {
	    if ( maskData[ i ].separator && maskData[ i ].maskChar == searchChar )
		return i;
	} else {
	    if ( !maskData[ i ].separator ) {
 		if ( searchChar.isNull() )
		    return i;
 		else if ( isValidInput( searchChar, maskData[ i ].maskChar ) )
 		    return i;
	    }
	}
	i += step;
    }
    return -1;
}


#endif // TQT_NO_LINEEDIT
