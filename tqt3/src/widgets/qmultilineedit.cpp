/**********************************************************************
**
** Implementation of TQMultiLineEdit widget class
**
** Created : 961005
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

#include "ntqmultilineedit.h"
#ifndef TQT_NO_MULTILINEEDIT
#include "ntqpainter.h"
#include "ntqscrollbar.h"
#include "ntqcursor.h"
#include "ntqclipboard.h"
#include "ntqpixmap.h"
#include "ntqregexp.h"
#include "ntqapplication.h"
#include "ntqdragobject.h"
#include "ntqpopupmenu.h"
#include "ntqtimer.h"
#include "ntqdict.h"
#include "../kernel/qrichtext_p.h"


/*!
  \class TQMultiLineEdit ntqmultilineedit.h
  \obsolete

  \brief The TQMultiLineEdit widget is a simple editor for inputting text.

  \ingroup advanced

  The TQMultiLineEdit was a simple editor widget in former TQt versions.  TQt
  3.0 includes a new richtext engine which obsoletes TQMultiLineEdit. It is
  still included for compatibility reasons. It is now a subclass of
  \l TQTextEdit, and provides enough of the old TQMultiLineEdit API to keep old
  applications working.

  If you implement something new with TQMultiLineEdit, we suggest using
  \l TQTextEdit instead and call TQTextEdit::setTextFormat(TQt::PlainText).

  Although most of the old TQMultiLineEdit API is still available, there is
  a few difference. The old TQMultiLineEdit operated on lines, not on
  paragraphs.  As lines change all the time during wordwrap, the new
  richtext engine uses paragraphs as basic elements in the data structure.
  All functions (numLines(), textLine(), etc.) that operated on lines, now
  operate on paragraphs. Further, getString() has been removed completely.
  It revealed too much of the internal data structure.

  Applications which made normal and reasonable use of TQMultiLineEdit
  should still work without problems. Some odd usage will require some
  porting. In these cases, it may be better to use \l TQTextEdit now.

  <img src=qmlined-m.png> <img src=qmlined-w.png>

  \sa TQTextEdit
*/

/*!
    \fn bool TQMultiLineEdit::autoUpdate() const
    \obsolete
*/

/*!
    \fn virtual void TQMultiLineEdit::setAutoUpdate( bool )
    \obsolete
*/

/*!
    \fn int TQMultiLineEdit::totalWidth() const
    \obsolete
*/

/*!
    \fn int TQMultiLineEdit::totalHeight() const
    \obsolete
*/

/*!
    \fn int TQMultiLineEdit::maxLines() const
    \obsolete
*/

/*!
    \fn void TQMultiLineEdit::setMaxLines( int )
    \obsolete
*/

/*!
    \fn void TQMultiLineEdit::deselect()
    \obsolete
*/


class TQMultiLineEditData
{
};


/*!
  Constructs a new, empty, TQMultiLineEdit with parent \a parent called
  \a name.
*/

TQMultiLineEdit::TQMultiLineEdit( TQWidget *parent , const char *name )
    : TQTextEdit( parent, name )
{
    d = new TQMultiLineEditData;
    setTextFormat( TQt::PlainText );
}

/*! \property TQMultiLineEdit::numLines
  \brief the number of paragraphs in the editor

  The count includes any empty paragraph at top and bottom, so for an
  empty editor this method returns 1.
*/

int TQMultiLineEdit::numLines() const
{
    return document()->lastParagraph()->paragId() + 1;
}

/*! \property TQMultiLineEdit::atEnd
  \brief whether the cursor is placed at the end of the text

  \sa atBeginning
*/

bool TQMultiLineEdit::atEnd() const
{
    return textCursor()->paragraph() == document()->lastParagraph() && textCursor()->atParagEnd();
}


/*! \property TQMultiLineEdit::atBeginning
  \brief whether the cursor is placed at the beginning of the text

  \sa atEnd
*/

bool TQMultiLineEdit::atBeginning() const
{
    return textCursor()->paragraph() == document()->firstParagraph() && textCursor()->atParagStart();
}

/*!  Returns the number of characters at paragraph number \a row. If
  \a row is out of range, -1 is returned.
*/

int TQMultiLineEdit::lineLength( int row ) const
{
    if ( row < 0 || row > numLines() )
	return -1;
    return document()->paragAt( row )->length() - 1;
}


/*! \reimp */

TQMultiLineEdit::~TQMultiLineEdit()
{
    delete d;
}

/*!
  If there is selected text, sets \a line1, \a col1, \a line2 and \a col2
  to the start and end of the selected region and returns true. Returns
  false if there is no selected text.
 */
bool TQMultiLineEdit::getMarkedRegion( int *line1, int *col1,
				      int *line2, int *col2 ) const
{
    int p1,c1, p2, c2;
    getSelection( &p1, &c1, &p2, &c2 );
    if ( p1 == -1 && c1 == -1 && p2 == -1 && c2 == -1 )
	return false;
    if ( line1 )
	*line1 = p1;
    if ( col1 )
	*col1 = c1;
    if ( line2 )
	*line2 = p2;
    if ( col2 )
	*col2 = c2;
    return true;
}


/*!
  Returns true if there is selected text.
*/

bool TQMultiLineEdit::hasMarkedText() const
{
    return hasSelectedText();
}


/*!
  Returns a copy of the selected text.
*/

TQString TQMultiLineEdit::markedText() const
{
    return selectedText();
}

/*!
  Moves the cursor one page down.  If \a mark is true, the text
  is selected.
*/

void TQMultiLineEdit::pageDown( bool mark )
{
    moveCursor( MoveDown, mark );
}


/*!
  Moves the cursor one page up.  If \a mark is true, the text
  is selected.
*/

void TQMultiLineEdit::pageUp( bool mark )
{
    moveCursor( MovePgUp, mark );
}


/*!  Inserts \a txt at paragraph number \a line. If \a line is less
  than zero, or larger than the number of paragraphs, the new text is
  put at the end.  If \a txt contains newline characters, several
  paragraphs are inserted.

  The cursor position is not changed.
*/

void TQMultiLineEdit::insertLine( const TQString &txt, int line )
{
    insertParagraph( txt, line );
}

/*!  Deletes the paragraph at paragraph number \a paragraph. If \a
  paragraph is less than zero or larger than the number of paragraphs,
  nothing is deleted.
*/

void TQMultiLineEdit::removeLine( int paragraph )
{
    removeParagraph( paragraph );
}

/*!  Inserts \a str at the current cursor position and selects the
  text if \a mark is true.
*/

void TQMultiLineEdit::insertAndMark( const TQString& str, bool mark )
{
    insert( str );
    if ( mark )
	document()->setSelectionEnd( TQTextDocument::Standard, *textCursor() );
}

/*!  Splits the paragraph at the current cursor position.
*/

void TQMultiLineEdit::newLine()
{
    insert( "\n" );
}


/*!  Deletes the character on the left side of the text cursor and
  moves the cursor one position to the left. If a text has been selected
  by the user (e.g. by clicking and dragging) the cursor is put at the
  beginning of the selected text and the selected text is removed.  \sa
  del()
*/

void TQMultiLineEdit::backspace()
{
    if ( document()->hasSelection( TQTextDocument::Standard ) ) {
	removeSelectedText();
	return;
    }

    if ( !textCursor()->paragraph()->prev() &&
	 textCursor()->atParagStart() )
	return;

    doKeyboardAction( ActionBackspace );
}


/*!  Moves the text cursor to the left end of the line. If \a mark is
  true, text is selected toward the first position. If it is false and the
  cursor is moved, all selected text is unselected.

  \sa end()
*/

void TQMultiLineEdit::home( bool mark )
{
    moveCursor( MoveLineStart, mark );
}

/*!  Moves the text cursor to the right end of the line. If \a mark is
  true, text is selected toward the last position.  If it is false and the
  cursor is moved, all selected text is unselected.

  \sa home()
*/

void TQMultiLineEdit::end( bool mark )
{
    moveCursor( MoveLineEnd, mark );
}


/*!
  \fn void TQMultiLineEdit::setCursorPosition( int line, int col )
  \reimp
*/

/*!  Sets the cursor position to character number \a col in paragraph
  number \a line.  The parameters are adjusted to lie within the legal
  range.

  If \a mark is false, the selection is cleared. otherwise it is extended.

*/

void TQMultiLineEdit::setCursorPosition( int line, int col, bool mark )
{
    if ( !mark )
	selectAll( false );
    TQTextEdit::setCursorPosition( line, col );
    if ( mark )
	document()->setSelectionEnd( TQTextDocument::Standard, *textCursor() );
}

/*!  Returns the top center point where the cursor is drawn.
*/

TQPoint TQMultiLineEdit::cursorPoint() const
{
    return TQPoint( textCursor()->x(), textCursor()->y() + textCursor()->paragraph()->rect().y() );
}

/*!  \property TQMultiLineEdit::alignment
  \brief The editor's paragraph alignment

  Sets the alignment to flag, which must be \c AlignLeft, \c
  AlignHCenter or \c AlignRight.

  If flag is an illegal flag nothing happens.

  \sa TQt::AlignmentFlags
*/
void TQMultiLineEdit::setAlignment( int flag )
{
    if ( flag == AlignCenter )
	flag = AlignHCenter;
    if ( flag != AlignLeft && flag != AlignRight && flag != AlignHCenter )
	return;
    TQTextParagraph *p = document()->firstParagraph();
    while ( p ) {
	p->setAlignment( flag );
	p = p->next();
    }
}

int TQMultiLineEdit::alignment() const
{
    return document()->firstParagraph()->alignment();
}


void TQMultiLineEdit::setEdited( bool e )
{
    setModified( e );
}

/*!  \property TQMultiLineEdit::edited
  \brief whether the document has been edited by the user

  This is the same as TQTextEdit's "modifed" property.

  \sa TQTextEdit::modified
*/
bool TQMultiLineEdit::edited() const
{
    return isModified();
}

/*!  Moves the cursor one word to the right.  If \a mark is true, the text
  is selected.

  \sa cursorWordBackward()
*/
void TQMultiLineEdit::cursorWordForward( bool mark )
{
    moveCursor( MoveWordForward, mark );
}

/*!  Moves the cursor one word to the left.  If \a mark is true, the
  text is selected.

  \sa cursorWordForward()
*/
void TQMultiLineEdit::cursorWordBackward( bool mark )
{
    moveCursor( MoveWordBackward, mark );
}

/*!
  \fn TQMultiLineEdit::insertAt( const TQString &s, int line, int col )
  \reimp
*/

/*!  Inserts string \a s at paragraph number \a line, after character
  number \a col in the paragraph.  If \a s contains newline
  characters, new lines are inserted.
  If \a mark is true the inserted string will be selected.

  The cursor position is adjusted.
 */

void TQMultiLineEdit::insertAt( const TQString &s, int line, int col, bool mark )
{
    TQTextEdit::insertAt( s, line, col );
    if ( mark )
	setSelection( line, col, line, col + s.length() );
}

// ### reggie - is this documentation correct?

/*!  Deletes text from the current cursor position to the end of the
  line. (Note that this function still operates on lines, not paragraphs.)
*/

void TQMultiLineEdit::killLine()
{
    doKeyboardAction( ActionKill );
}

/*!  Moves the cursor one character to the left. If \a mark is true,
  the text is selected.
  The \a wrap parameter is currently ignored.

  \sa cursorRight() cursorUp() cursorDown()
*/

void TQMultiLineEdit::cursorLeft( bool mark, bool )
{
    moveCursor( MoveBackward, mark );
}

/*!  Moves the cursor one character to the right.  If \a mark is true,
  the text is selected.
  The \a wrap parameter is currently ignored.

  \sa cursorLeft() cursorUp() cursorDown()
*/

void TQMultiLineEdit::cursorRight( bool mark, bool )
{
    moveCursor( MoveForward, mark );
}

/*!  Moves the cursor up one line.  If \a mark is true, the text is
  selected.

  \sa cursorDown() cursorLeft() cursorRight()
*/

void TQMultiLineEdit::cursorUp( bool mark )
{
    moveCursor( MoveUp, mark );
}

/*!
  Moves the cursor one line down.  If \a mark is true, the text
  is selected.
  \sa cursorUp() cursorLeft() cursorRight()
*/

void TQMultiLineEdit::cursorDown( bool mark )
{
    moveCursor( MoveDown, mark );
}


/*!  Returns the text at line number \a line (possibly the empty
  string), or a \link TQString::operator!() null string\endlink if \a
  line is invalid.
*/

TQString TQMultiLineEdit::textLine( int line ) const
{
    if ( line < 0 || line >= numLines() )
	return TQString::null;
    TQString str = document()->paragAt( line )->string()->toString();
    str.truncate( str.length() - 1 );
    return str;
}

#endif
