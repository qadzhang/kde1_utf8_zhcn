/****************************************************************************
**
** Implementation of the TQTextEdit class
**
** Created : 990101
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

#include "ntqtextedit.h"

#ifndef TQT_NO_TEXTEDIT

// Keep this position to avoid patch rejection
#ifndef TQT_NO_IM
#include "ntqinputcontext.h"
#endif

#include "../kernel/qrichtext_p.h"
#include "ntqpainter.h"
#include "ntqpen.h"
#include "ntqbrush.h"
#include "ntqpixmap.h"
#include "ntqfont.h"
#include "ntqcolor.h"
#include "ntqstyle.h"
#include "ntqsize.h"
#include "ntqevent.h"
#include "ntqtimer.h"
#include "ntqapplication.h"
#include "ntqlistbox.h"
#include "ntqvbox.h"
#include "ntqapplication.h"
#include "ntqclipboard.h"
#include "ntqcolordialog.h"
#include "ntqfontdialog.h"
#include "ntqstylesheet.h"
#include "ntqdragobject.h"
#include "ntqurl.h"
#include "ntqcursor.h"
#include "ntqregexp.h"
#include "ntqpopupmenu.h"
#include "ntqptrstack.h"
#include "ntqmetaobject.h"
#include "ntqtextbrowser.h"
#include <private/qucom_p.h>
#include "private/qsyntaxhighlighter_p.h"
#include <ntqguardedptr.h>

#ifndef TQT_NO_ACCEL
#include <ntqkeysequence.h>
#define ACCEL_KEY(k) "\t" + TQString(TQKeySequence( TQt::CTRL | TQt::Key_ ## k ))
#else
#define ACCEL_KEY(k) "\t" + TQString("Ctrl+" #k)
#endif

#ifdef QT_TEXTEDIT_OPTIMIZATION
#define LOGOFFSET(i) d->logOffset + i
#endif

struct TQUndoRedoInfoPrivate
{
    TQTextString text;
};

class TQTextEditPrivate
{
public:
    TQTextEditPrivate()
	:preeditStart(-1),preeditLength(-1),ensureCursorVisibleInShowEvent(false),
	 tabChangesFocus(false),
#ifndef TQT_NO_CLIPBOARD
	 clipboard_mode( TQClipboard::Clipboard ),
#endif
#ifdef QT_TEXTEDIT_OPTIMIZATION
	 od(0), optimMode(false),
	 maxLogLines(-1),
	 logOffset(0),
#endif
	 autoFormatting( (uint)TQTextEdit::AutoAll )
    {
	for ( int i=0; i<7; i++ )
	    id[i] = 0;
    }
    int id[ 7 ];
    int preeditStart;
    int preeditLength;
    bool composeMode() const { return ( preeditLength > 0 ); }

    uint ensureCursorVisibleInShowEvent : 1;
    uint tabChangesFocus : 1;
    TQString scrollToAnchor; // used to deferr scrollToAnchor() until the show event when we are resized
    TQString pressedName;
    TQString onName;
#ifndef TQT_NO_CLIPBOARD
    TQClipboard::Mode clipboard_mode;
#endif
    TQTimer *trippleClickTimer;
    TQPoint trippleClickPoint;
#ifdef QT_TEXTEDIT_OPTIMIZATION
    TQTextEditOptimPrivate * od;
    bool optimMode : 1;
    int maxLogLines;
    int logOffset;
#endif
    uint autoFormatting;
};

#ifndef TQT_NO_MIME
class TQRichTextDrag : public TQTextDrag
{
public:
    TQRichTextDrag( TQWidget *dragSource = 0, const char *name = 0 );

    void setPlainText( const TQString &txt ) { setText( txt ); }
    void setRichText( const TQString &txt ) { richTxt = txt; }

    virtual TQByteArray encodedData( const char *mime ) const;
    virtual const char* format( int i ) const;

    static bool decode( TQMimeSource *e, TQString &str, const TQCString &mimetype,
			const TQCString &subtype );
    static bool canDecode( TQMimeSource* e );

private:
    TQString richTxt;

};

TQRichTextDrag::TQRichTextDrag( TQWidget *dragSource, const char *name )
    : TQTextDrag( dragSource, name )
{
}

TQByteArray TQRichTextDrag::encodedData( const char *mime ) const
{
    if ( qstrcmp( "application/x-qrichtext", mime ) == 0 ) {
	return richTxt.utf8(); // #### perhaps we should use USC2 instead?
    } else
	return TQTextDrag::encodedData( mime );
}

bool TQRichTextDrag::decode( TQMimeSource *e, TQString &str, const TQCString &mimetype,
			    const TQCString &subtype )
{
    if ( mimetype == "application/x-qrichtext" ) {
	// do richtext decode
	const char *mime;
	int i;
	for ( i = 0; ( mime = e->format( i ) ); ++i ) {
	    if ( qstrcmp( "application/x-qrichtext", mime ) != 0 )
		continue;
	    str = TQString::fromUtf8( e->encodedData( mime ) );
	    return true;
	}
	return false;
    }

    // do a regular text decode
    TQCString subt = subtype;
    return TQTextDrag::decode( e, str, subt );
}

bool TQRichTextDrag::canDecode( TQMimeSource* e )
{
    if ( e->provides( "application/x-qrichtext" ) )
	return true;
    return TQTextDrag::canDecode( e );
}

const char* TQRichTextDrag::format( int i ) const
{
    if ( TQTextDrag::format( i ) )
	return TQTextDrag::format( i );
    if ( TQTextDrag::format( i-1 ) )
	return "application/x-qrichtext";
    return 0;
}

#endif

static bool block_set_alignment = false;

/*!
    \class TQTextEdit ntqtextedit.h
    \brief The TQTextEdit widget provides a powerful single-page rich text editor.

    \ingroup basic
    \ingroup text
    \mainclass

    \tableofcontents

    \section1 Introduction and Concepts

    TQTextEdit is an advanced WYSIWYG viewer/editor supporting rich
    text formatting using HTML-style tags. It is optimized to handle
    large documents and to respond quickly to user input.

    TQTextEdit has four modes of operation:
    \table
    \header \i Mode \i Command \i Notes
    \row \i Plain Text Editor \i setTextFormat(PlainText)
	 \i Set text with setText(); text() returns plain text. Text
	 attributes (e.g. colors) can be set, but plain text is always
	 returned.
    \row \i Rich Text Editor \i setTextFormat(RichText)
	 \i Set text with setText(); text() returns rich text. Rich
	 text editing is fairly limited. You can't set margins or
	 insert images for example (although you can read and
	 correctly display files that have margins set and that
	 include images). This mode is mostly useful for editing small
	 amounts of rich text. <sup>1.</sup>
    \row \i Text Viewer \i setReadOnly(true)
         \i Set text with setText() or append() (which has no undo
	 history so is faster and uses less memory); text() returns
	 plain or rich text depending on the textFormat(). This mode
	 can correctly display a large subset of HTML tags.
    \row \i Log Viewer \i setTextFormat(LogText)
	 \i Append text using append(). The widget is set to be read
	 only and rich text support is disabled although a few HTML
	 tags (for color, bold, italic and underline) may be used.
	 (See \link #logtextmode LogText mode\endlink for details.)
    \endtable

    <sup>1.</sup><small>A more complete API that supports setting
    margins, images, etc., is planned for a later TQt release.</small>

    TQTextEdit can be used as a syntax highlighting editor when used in
    conjunction with TQSyntaxHighlighter.

    We recommend that you always call setTextFormat() to set the mode
    you want to use. If you use \c AutoText then setText() and
    append() will try to determine whether the text they are given is
    plain text or rich text. If you use \c RichText then setText() and
    append() will assume that the text they are given is rich text.
    insert() simply inserts the text it is given.

    TQTextEdit works on paragraphs and characters. A paragraph is a
    formatted string which is word-wrapped to fit into the width of
    the widget. By default when reading plain text, one newline
    signify a paragraph. A document consists of zero or more
    paragraphs, indexed from 0. Characters are indexed on a
    per-paragraph basis, also indexed from 0. The words in the
    paragraph are aligned in accordance with the paragraph's
    alignment(). Paragraphs are separated by hard line breaks. Each
    character within a paragraph has its own attributes, for example,
    font and color.

    The text edit documentation uses the following concepts:
    \list
    \i \e{current format} --
    this is the format at the current cursor position, \e and it
    is the format of the selected text if any.
    \i \e{current paragraph} -- the paragraph which contains the
    cursor.
    \endlist

    TQTextEdit can display images (using TQMimeSourceFactory), lists and
    tables. If the text is too large to view within the text edit's
    viewport, scrollbars will appear. The text edit can load both
    plain text and HTML files (a subset of HTML 3.2 and 4). The
    rendering style and the set of valid tags are defined by a
    styleSheet(). Custom tags can be created and placed in a custom
    style sheet. Change the style sheet with \l{setStyleSheet()}; see
    TQStyleSheet for details. The images identified by image tags are
    displayed if they can be interpreted using the text edit's
    \l{TQMimeSourceFactory}; see setMimeSourceFactory().

    If you want a text browser with more navigation use TQTextBrowser.
    If you just need to display a small piece of rich text use TQLabel
    or TQSimpleRichText.

    If you create a new TQTextEdit, and want to allow the user to edit
    rich text, call setTextFormat(TQt::RichText) to ensure that the
    text is treated as rich text. (Rich text uses HTML tags to set
    text formatting attributes. See TQStyleSheet for information on the
    HTML tags that are supported.). If you don't call setTextFormat()
    explicitly the text edit will guess from the text itself whether
    it is rich text or plain text. This means that if the text looks
    like HTML or XML it will probably be interpreted as rich text, so
    you should call setTextFormat(TQt::PlainText) to preserve such
    text.

    Note that we do not intend to add a full-featured web browser
    widget to TQt (because that would easily double TQt's size and only
    a few applications would benefit from it). The rich
    text support in TQt is designed to provide a fast, portable and
    efficient way to add reasonable online help facilities to
    applications, and to provide a basis for rich text editors.

    \section1 Using TQTextEdit as a Display Widget

    TQTextEdit can display a large HTML subset, including tables and
    images.

    The text is set or replaced using setText() which deletes any
    existing text and replaces it with the text passed in the
    setText() call. If you call setText() with legacy HTML (with
    setTextFormat(RichText) in force), and then call text(), the text
    that is returned may have different markup, but will render the
    same. Text can be inserted with insert(), paste(), pasteSubType()
    and append(). Text that is appended does not go into the undo
    history; this makes append() faster and consumes less memory. Text
    can also be cut(). The entire text is deleted with clear() and the
    selected text is deleted with removeSelectedText(). Selected
    (marked) text can also be deleted with del() (which will delete
    the character to the right of the cursor if no text is selected).

    Loading and saving text is achieved using setText() and text(),
    for example:
    \code
    TQFile file( fileName ); // Read the text from a file
    if ( file.open( IO_ReadOnly ) ) {
	TQTextStream stream( &file );
	textEdit->setText( stream.read() );
    }

    TQFile file( fileName ); // Write the text to a file
    if ( file.open( IO_WriteOnly ) ) {
	TQTextStream stream( &file );
	stream << textEdit->text();
	textEdit->setModified( false );
    }
    \endcode

    By default the text edit wraps words at whitespace to fit within
    the text edit widget. The setWordWrap() function is used to
    specify the kind of word wrap you want, or \c NoWrap if you don't
    want any wrapping. Call setWordWrap() to set a fixed pixel width
    \c FixedPixelWidth, or character column (e.g. 80 column) \c
    FixedColumnWidth with the pixels or columns specified with
    setWrapColumnOrWidth(). If you use word wrap to the widget's width
    \c WidgetWidth, you can specify whether to break on whitespace or
    anywhere with setWrapPolicy().

    The background color is set differently than other widgets, using
    setPaper(). You specify a brush style which could be a plain color
    or a complex pixmap.

    Hypertext links are automatically underlined; this can be changed
    with setLinkUnderline(). The tab stop width is set with
    setTabStopWidth().

    The zoomIn() and zoomOut() functions can be used to resize the
    text by increasing (decreasing for zoomOut()) the point size used.
    Images are not affected by the zoom functions.

    The lines() function returns the number of lines in the text and
    paragraphs() returns the number of paragraphs. The number of lines
    within a particular paragraph is returned by linesOfParagraph().
    The length of the entire text in characters is returned by
    length().

    You can scroll to an anchor in the text, e.g.
    \c{<a name="anchor">} with scrollToAnchor(). The find() function
    can be used to find and select a given string within the text.

    A read-only TQTextEdit provides the same functionality as the
    (obsolete) TQTextView. (TQTextView is still supplied for
    compatibility with old code.)

    \section2 Read-only key bindings

    When TQTextEdit is used read-only the key-bindings are limited to
    navigation, and text may only be selected with the mouse:
    \table
    \header \i Keypresses \i Action
    \row \i UpArrow	\i Move one line up
    \row \i DownArrow	\i Move one line down
    \row \i LeftArrow	\i Move one character left
    \row \i RightArrow	\i Move one character right
    \row \i PageUp	\i Move one (viewport) page up
    \row \i PageDown	\i Move one (viewport) page down
    \row \i Home	\i Move to the beginning of the text
    \row \i End		\i Move to the end of the text
    \row \i Shift+Wheel
	 \i Scroll the page horizontally (the Wheel is the mouse wheel)
    \row \i Ctrl+Wheel	\i Zoom the text
    \endtable

    The text edit may be able to provide some meta-information. For
    example, the documentTitle() function will return the text from
    within HTML \c{<title>} tags.

    The text displayed in a text edit has a \e context. The context is
    a path which the text edit's TQMimeSourceFactory uses to resolve
    the locations of files and images. It is passed to the
    mimeSourceFactory() when quering data. (See TQTextEdit() and
    \l{context()}.)

    \target logtextmode
    \section2 Using TQTextEdit in LogText Mode

    Setting the text format to \c LogText puts the widget in a special
    mode which is optimized for very large texts. Editing, word wrap,
    and rich text support are disabled in this mode (the widget is
    explicitly made read-only). This allows the text to be stored in a
    different, more memory efficient manner. However, a certain degree
    of text formatting is supported through the use of formatting tags.
    A tag is delimited by \c < and \c {>}. The characters \c {<}, \c >
    and \c & are escaped by using \c {&lt;}, \c {&gt;} and \c {&amp;}.
    A tag pair consists of a left and a right tag (or open/close tags).
    Left-tags mark the starting point for formatting, while right-tags
    mark the ending point. A right-tag always start with a \c / before
    the tag keyword. For example \c <b> and \c </b> are a tag pair.
    Tags can be nested, but they have to be closed in the same order as
    they are opened. For example, \c <b><u></u></b> is valid, while \c
    <b><u></b></u> will output an error message.

    By using tags it is possible to change the color, bold, italic and
    underline settings for a piece of text. A color can be specified
    by using the HTML font tag \c {<font color=colorname>}. The color
    name can be one of the color names from the X11 color database, or
    a RGB hex value (e.g \c {#00ff00}). Example of valid color tags:
    \c {<font color=red>}, \c {<font color="light blue">}, \c {<font
    color="#223344">}. Bold, italic and underline settings can be
    specified by the tags \c {<b>}, \c <i> and \c {<u>}. Note that a
    tag does not necessarily have to be closed. A valid example:
    \code
    This is <font color=red>red</font> while <b>this</b> is <font color=blue>blue</font>.
    <font color=green><font color=yellow>Yellow,</font> and <u>green</u>.
    \endcode

    Stylesheets can also be used in LogText mode. To create and use a
    custom tag, you could do the following:
    \code
    TQTextEdit * log = new TQTextEdit( this );
    log->setTextFormat( TQt::LogText );
    TQStyleSheetItem * item = new TQStyleSheetItem( log->styleSheet(), "mytag" );
    item->setColor( "red" );
    item->setFontWeight( TQFont::Bold );
    item->setFontUnderline( true );
    log->append( "This is a <mytag>custom tag</mytag>!" );
    \endcode
    Note that only the color, bold, underline and italic attributes of
    a TQStyleSheetItem is used in LogText mode.

    Note that you can use setMaxLogLines() to limit the number of
    lines the widget can hold in LogText mode.

    There are a few things that you need to be aware of when the
    widget is in this mode:
    \list
    \i Functions that deal with rich text formatting and cursor
    movement will not work or return anything valid.
    \i Lines are equivalent to paragraphs.
    \endlist

    \section1 Using TQTextEdit as an Editor

    All the information about using TQTextEdit as a display widget also
    applies here.

    The current format's attributes are set with setItalic(),
    setBold(), setUnderline(), setFamily() (font family),
    setPointSize(), setColor() and setCurrentFont(). The current
    paragraph's alignment is set with setAlignment().

    Use setSelection() to select text. The setSelectionAttributes()
    function is used to indicate how selected text should be
    displayed. Use hasSelectedText() to find out if any text is
    selected. The currently selected text's position is available
    using getSelection() and the selected text itself is returned by
    selectedText(). The selection can be copied to the clipboard with
    copy(), or cut to the clipboard with cut(). It can be deleted with
    removeSelectedText(). The entire text can be selected (or
    unselected) using selectAll(). TQTextEdit supports multiple
    selections. Most of the selection functions operate on the default
    selection, selection 0. If the user presses a non-selecting key,
    e.g. a cursor key without also holding down Shift, all selections
    are cleared.

    Set and get the position of the cursor with setCursorPosition()
    and getCursorPosition() respectively. When the cursor is moved,
    the signals currentFontChanged(), currentColorChanged() and
    currentAlignmentChanged() are emitted to reflect the font, color
    and alignment at the new cursor position.

    If the text changes, the textChanged() signal is emitted, and if
    the user inserts a new line by pressing Return or Enter,
    returnPressed() is emitted. The isModified() function will return
    true if the text has been modified.

    TQTextEdit provides command-based undo and redo. To set the depth
    of the command history use setUndoDepth() which defaults to 100
    steps. To undo or redo the last operation call undo() or redo().
    The signals undoAvailable() and redoAvailable() indicate whether
    the undo and redo operations can be executed.

    The indent() function is used to reindent a paragraph. It is
    useful for code editors, for example in \link designer-manual.book
    TQt Designer\endlink's code editor \e{Ctrl+I} invokes the indent()
    function.

    \section2 Editing key bindings

    The list of key-bindings which are implemented for editing:
    \table
    \header \i Keypresses \i Action
    \row \i Backspace \i Delete the character to the left of the cursor
    \row \i Delete \i Delete the character to the right of the cursor
    \row \i Ctrl+A \i Move the cursor to the beginning of the line
    \row \i Ctrl+B \i Move the cursor one character left
    \row \i Ctrl+C \i Copy the marked text to the clipboard (also
		      Ctrl+Insert under Windows)
    \row \i Ctrl+D \i Delete the character to the right of the cursor
    \row \i Ctrl+E \i Move the cursor to the end of the line
    \row \i Ctrl+F \i Move the cursor one character right
    \row \i Ctrl+H \i Delete the character to the left of the cursor
    \row \i Ctrl+K \i Delete to end of line
    \row \i Ctrl+N \i Move the cursor one line down
    \row \i Ctrl+P \i Move the cursor one line up
    \row \i Ctrl+V \i Paste the clipboard text into line edit
		      (also Shift+Insert under Windows)
    \row \i Ctrl+X \i Cut the marked text, copy to clipboard
		      (also Shift+Delete under Windows)
    \row \i Ctrl+Z \i Undo the last operation
    \row \i Ctrl+Y \i Redo the last operation
    \row \i LeftArrow	    \i Move the cursor one character left
    \row \i Ctrl+LeftArrow  \i Move the cursor one word left
    \row \i RightArrow	    \i Move the cursor one character right
    \row \i Ctrl+RightArrow \i Move the cursor one word right
    \row \i UpArrow	    \i Move the cursor one line up
    \row \i Ctrl+UpArrow    \i Move the cursor one word up
    \row \i DownArrow	    \i Move the cursor one line down
    \row \i Ctrl+Down Arrow \i Move the cursor one word down
    \row \i PageUp	    \i Move the cursor one page up
    \row \i PageDown	    \i Move the cursor one page down
    \row \i Home	    \i Move the cursor to the beginning of the line
    \row \i Ctrl+Home	    \i Move the cursor to the beginning of the text
    \row \i End		    \i Move the cursor to the end of the line
    \row \i Ctrl+End	    \i Move the cursor to the end of the text
    \row \i Shift+Wheel	    \i Scroll the page horizontally
			    (the Wheel is the mouse wheel)
    \row \i Ctrl+Wheel	    \i Zoom the text
    \endtable

    To select (mark) text hold down the Shift key whilst pressing one
    of the movement keystrokes, for example, \e{Shift+Right Arrow}
    will select the character to the right, and \e{Shift+Ctrl+Right
    Arrow} will select the word to the right, etc.

    By default the text edit widget operates in insert mode so all
    text that the user enters is inserted into the text edit and any
    text to the right of the cursor is moved out of the way. The mode
    can be changed to overwrite, where new text overwrites any text to
    the right of the cursor, using setOverwriteMode().
*/

/*!
    \enum TQTextEdit::AutoFormatting

    \value AutoNone Do not perform any automatic formatting
    \value AutoBulletList Only automatically format bulletted lists
    \value AutoAll Apply all available autoformatting
*/


/*!
    \enum TQTextEdit::KeyboardAction

    This enum is used by doKeyboardAction() to specify which action
    should be executed:

    \value ActionBackspace  Delete the character to the left of the
    cursor.

    \value ActionDelete  Delete the character to the right of the
    cursor.

    \value ActionReturn  Split the paragraph at the cursor position.

    \value ActionKill If the cursor is not at the end of the
    paragraph, delete the text from the cursor position until the end
    of the paragraph. If the cursor is at the end of the paragraph,
    delete the hard line break at the end of the paragraph: this will
    cause this paragraph to be joined with the following paragraph.

    \value ActionWordBackspace Delete the word to the left of the
    cursor position.

    \value ActionWordDelete Delete the word to the right of the
    cursor position

*/

/*!
    \enum TQTextEdit::VerticalAlignment

    This enum is used to set the vertical alignment of the text.

    \value AlignNormal Normal alignment
    \value AlignSuperScript Superscript
    \value AlignSubScript Subscript
*/

/*!
    \enum TQTextEdit::TextInsertionFlags

    \internal

    \value RedoIndentation
    \value CheckNewLines
    \value RemoveSelected
*/


/*!
    \fn void TQTextEdit::copyAvailable(bool yes)

    This signal is emitted when text is selected or de-selected in the
    text edit.

    When text is selected this signal will be emitted with \a yes set
    to true. If no text has been selected or if the selected text is
    de-selected this signal is emitted with \a yes set to false.

    If \a yes is true then copy() can be used to copy the selection to
    the clipboard. If \a yes is false then copy() does nothing.

    \sa selectionChanged()
*/


/*!
    \fn void TQTextEdit::textChanged()

    This signal is emitted whenever the text in the text edit changes.

    \sa setText() append()
*/

/*!
    \fn void TQTextEdit::selectionChanged()

    This signal is emitted whenever the selection changes.

    \sa setSelection() copyAvailable()
*/

/*!  \fn TQTextDocument *TQTextEdit::document() const

    \internal

  This function returns the TQTextDocument which is used by the text
  edit.
*/

/*!  \fn void TQTextEdit::setDocument( TQTextDocument *doc )

    \internal

  This function sets the TQTextDocument which should be used by the text
  edit to \a doc. This can be used, for example, if you want to
  display a document using multiple views. You would create a
  TQTextDocument and set it to the text edits which should display it.
  You would need to connect to the textChanged() and
  selectionChanged() signals of all the text edits and update them all
  accordingly (preferably with a slight delay for efficiency reasons).
*/

/*!
    \enum TQTextEdit::CursorAction

    This enum is used by moveCursor() to specify in which direction
    the cursor should be moved:

    \value MoveBackward  Moves the cursor one character backward

    \value MoveWordBackward Moves the cursor one word backward

    \value MoveForward  Moves the cursor one character forward

    \value MoveWordForward Moves the cursor one word forward

    \value MoveUp  Moves the cursor up one line

    \value MoveDown  Moves the cursor down one line

    \value MoveLineStart  Moves the cursor to the beginning of the line

    \value MoveLineEnd Moves the cursor to the end of the line

    \value MoveHome  Moves the cursor to the beginning of the document

    \value MoveEnd Moves the cursor to the end of the document

    \value MovePgUp  Moves the cursor one viewport page up

    \value MovePgDown  Moves the cursor one viewport page down
*/

/*!
  \enum TQt::AnchorAttribute

  An anchor has one or more of the following attributes:

  \value AnchorName the name attribute of the anchor. This attribute is
  used when scrolling to an anchor in the document.

  \value AnchorHref the href attribute of the anchor. This attribute is
  used when a link is clicked to determine what content to load.
*/

/*!
    \property TQTextEdit::overwriteMode
    \brief the text edit's overwrite mode

    If false (the default) characters entered by the user are inserted
    with any characters to the right being moved out of the way. If
    true, the editor is in overwrite mode, i.e. characters entered by
    the user overwrite any characters to the right of the cursor
    position.
*/

/*!
    \fn void TQTextEdit::setCurrentFont( const TQFont &f )

    Sets the font of the current format to \a f.

    If the widget is in \c LogText mode this function will do
    nothing. Use setFont() instead.

    \sa currentFont() setPointSize() setFamily()
*/

/*!
    \property TQTextEdit::undoDepth
    \brief the depth of the undo history

    The maximum number of steps in the undo/redo history. The default
    is 100.

    \sa undo() redo()
*/

/*!
    \fn void TQTextEdit::undoAvailable( bool yes )

    This signal is emitted when the availability of undo changes. If
    \a yes is true, then undo() will work until undoAvailable( false )
    is next emitted.

    \sa undo() undoDepth()
*/

/*!
    \fn void TQTextEdit::modificationChanged( bool m )

    This signal is emitted when the modification status of the
    document has changed. If \a m is true, the document was modified,
    otherwise the modification state has been reset to unmodified.

    \sa modified
*/

/*!
    \fn void TQTextEdit::redoAvailable( bool yes )

    This signal is emitted when the availability of redo changes. If
    \a yes is true, then redo() will work until redoAvailable( false )
    is next emitted.

    \sa redo() undoDepth()
*/

/*!
    \fn void TQTextEdit::currentFontChanged( const TQFont &f )

    This signal is emitted if the font of the current format has
    changed.

    The new font is \a f.

    \sa setCurrentFont()
*/

/*!
    \fn void TQTextEdit::currentColorChanged( const TQColor &c )

    This signal is emitted if the color of the current format has
    changed.

    The new color is \a c.

    \sa setColor()
*/

/*!
    \fn void TQTextEdit::currentVerticalAlignmentChanged( VerticalAlignment a )

    This signal is emitted if the vertical alignment of the current
    format has changed.

    The new vertical alignment is \a a.

    \sa setVerticalAlignment()
*/

/*!
    \fn void TQTextEdit::currentAlignmentChanged( int a )

    This signal is emitted if the alignment of the current paragraph
    has changed.

    The new alignment is \a a.

    \sa setAlignment()
*/

/*!
    \fn void TQTextEdit::cursorPositionChanged( TQTextCursor *c )

    \internal
*/

/*!
    \fn void TQTextEdit::cursorPositionChanged( int para, int pos )

    This signal is emitted if the position of the cursor has changed.
    \a para contains the paragraph index and \a pos contains the
    character position within the paragraph.

    \sa setCursorPosition()
*/

/*!
    \fn void TQTextEdit::clicked( int para, int pos )

    This signal is emitted when the mouse is clicked on the paragraph
    \a para at character position \a pos.

    \sa doubleClicked()
*/

/*! \fn void TQTextEdit::doubleClicked( int para, int pos )

  This signal is emitted when the mouse is double-clicked on the
  paragraph \a para at character position \a pos.

  \sa clicked()
*/


/*!
    \fn void TQTextEdit::returnPressed()

    This signal is emitted if the user pressed the Return or the Enter
    key.
*/

/*!
    \fn TQTextCursor *TQTextEdit::textCursor() const

    Returns the text edit's text cursor.

    \warning TQTextCursor is not in the public API, but in special
    circumstances you might wish to use it.
*/

/*!
    Constructs an empty TQTextEdit called \a name, with parent \a
    parent.
*/

TQTextEdit::TQTextEdit( TQWidget *parent, const char *name )
    : TQScrollView( parent, name, WStaticContents | WNoAutoErase ),
      doc( new TQTextDocument( 0 ) ), undoRedoInfo( doc )
{
    init();
}

/*!
    Constructs a TQTextEdit called \a name, with parent \a parent. The
    text edit will display the text \a text using context \a context.

    The \a context is a path which the text edit's TQMimeSourceFactory
    uses to resolve the locations of files and images. It is passed to
    the mimeSourceFactory() when quering data.

    For example if the text contains an image tag,
    \c{<img src="image.png">}, and the context is "path/to/look/in", the
    TQMimeSourceFactory will try to load the image from
    "path/to/look/in/image.png". If the tag was
    \c{<img src="/image.png">}, the context will not be used (because
    TQMimeSourceFactory recognizes that we have used an absolute path)
    and will try to load "/image.png". The context is applied in exactly
    the same way to \e hrefs, for example,
    \c{<a href="target.html">Target</a>}, would resolve to
    "path/to/look/in/target.html".
*/

TQTextEdit::TQTextEdit( const TQString& text, const TQString& context,
		      TQWidget *parent, const char *name)
    : TQScrollView( parent, name, WStaticContents | WNoAutoErase ),
      doc( new TQTextDocument( 0 ) ), undoRedoInfo( doc )
{
    init();
    setText( text, context );
}

/*!
    \reimp
*/

TQTextEdit::~TQTextEdit()
{
    delete undoRedoInfo.d;
    undoRedoInfo.d = 0;
    delete cursor;
    delete doc;
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode )
	delete d->od;
#endif
    delete d;
}

void TQTextEdit::init()
{
    d = new TQTextEditPrivate;
    doc->formatCollection()->setPaintDevice( this );
    undoEnabled = true;
    readOnly = true;
    setReadOnly( false );
    setFrameStyle( LineEditPanel | Sunken );
    connect( doc, TQ_SIGNAL( minimumWidthChanged(int) ),
	     this, TQ_SLOT( documentWidthChanged(int) ) );

    mousePressed = false;
    inDoubleClick = false;
    modified = false;
    onLink = TQString::null;
    d->onName = TQString::null;
    overWrite = false;
    wrapMode = WidgetWidth;
    wrapWidth = -1;
    wPolicy = AtWhiteSpace;
    mightStartDrag = false;
    inDnD = false;
    doc->setFormatter( new TQTextFormatterBreakWords );
    doc->formatCollection()->defaultFormat()->setFont( TQScrollView::font() );
    doc->formatCollection()->defaultFormat()->setColor( colorGroup().color( TQColorGroup::Text ) );
    currentFormat = doc->formatCollection()->defaultFormat();
    currentAlignment = TQt::AlignAuto;

    setBackgroundMode( PaletteBase );
    viewport()->setBackgroundMode( PaletteBase );
    viewport()->setAcceptDrops( true );
    resizeContents( 0, doc->lastParagraph() ?
		    ( doc->lastParagraph()->paragId() + 1 ) * doc->formatCollection()->defaultFormat()->height() : 0 );

    setKeyCompression( true );
    viewport()->setMouseTracking( true );
#ifndef TQT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
    cursor = new TQTextCursor( doc );

    formatTimer = new TQTimer( this );
    connect( formatTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( formatMore() ) );
    lastFormatted = doc->firstParagraph();

    scrollTimer = new TQTimer( this );
    connect( scrollTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( autoScrollTimerDone() ) );

    interval = 0;
    changeIntervalTimer = new TQTimer( this );
    connect( changeIntervalTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( doChangeInterval() ) );

    cursorVisible = true;
    blinkTimer = new TQTimer( this );
    connect( blinkTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( blinkCursor() ) );

#ifndef TQT_NO_DRAGANDDROP
    dragStartTimer = new TQTimer( this );
    connect( dragStartTimer, TQ_SIGNAL( timeout() ),
	     this, TQ_SLOT( startDrag() ) );
#endif

    d->trippleClickTimer = new TQTimer( this );

    formatMore();

    blinkCursorVisible = false;

    viewport()->setFocusProxy( this );
    viewport()->setFocusPolicy( WheelFocus );
    setInputMethodEnabled( true );
    viewport()->installEventFilter( this );
    connect( this, TQ_SIGNAL(horizontalSliderReleased()), this, TQ_SLOT(sliderReleased()) );
    connect( this, TQ_SIGNAL(verticalSliderReleased()), this, TQ_SLOT(sliderReleased()) );
    installEventFilter( this );
}

void TQTextEdit::paintDocument( bool drawAll, TQPainter *p, int cx, int cy, int cw, int ch )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    Q_ASSERT( !d->optimMode );
    if ( d->optimMode )
	return;
#endif

    bool drawCur = hasFocus() || viewport()->hasFocus();
    if (( hasSelectedText() && !style().styleHint( TQStyle::SH_BlinkCursorWhenTextSelected ) ) ||
	isReadOnly() || !cursorVisible || doc->hasSelection( TQTextDocument::IMSelectionText ))
	drawCur = false;
    TQColorGroup g = colorGroup();
    const TQColorGroup::ColorRole backRole = TQPalette::backgroundRoleFromMode(backgroundMode());
    if ( doc->paper() )
	g.setBrush( backRole, *doc->paper() );

    if ( contentsY() < doc->y() ) {
	p->fillRect( contentsX(), contentsY(), visibleWidth(), doc->y(),
		     g.brush( backRole ) );
    }
    if ( drawAll && doc->width() - contentsX() < cx + cw ) {
	p->fillRect( doc->width() - contentsX(), cy, cx + cw - doc->width() + contentsX(), ch,
		     g.brush( backRole ) );
    }

    p->setBrushOrigin( -contentsX(), -contentsY() );

    lastFormatted = doc->draw( p, cx, cy, cw, ch, g, !drawAll, drawCur, cursor );

    if ( lastFormatted == doc->lastParagraph() )
	resizeContents( contentsWidth(), doc->height() );

    if ( contentsHeight() < visibleHeight() && ( !doc->lastParagraph() || doc->lastParagraph()->isValid() ) && drawAll )
	p->fillRect( 0, contentsHeight(), visibleWidth(),
		     visibleHeight() - contentsHeight(), g.brush( backRole ) );
}

/*!
    \reimp
*/

void TQTextEdit::drawContents( TQPainter *p, int cx, int cy, int cw, int ch )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	optimDrawContents( p, cx, cy, cw, ch );
	return;
    }
#endif
    paintDocument( true, p, cx, cy, cw, ch );
    int v;
    p->setPen( foregroundColor() );
    if ( document()->isPageBreakEnabled() &&  ( v = document()->flow()->pageSize() ) > 0 ) {
	int l = int(cy / v) * v;
	while ( l < cy + ch ) {
	    p->drawLine( cx, l, cx + cw - 1, l );
	    l += v;
	}
    }

    // This invocation is required to follow dragging of active window
    // by the showed candidate window.
    updateMicroFocusHint();
}

/*!
    \reimp
*/

void TQTextEdit::drawContents( TQPainter *p )
{
    if ( horizontalScrollBar()->isVisible() &&
	 verticalScrollBar()->isVisible() ) {
	const TQRect verticalRect = verticalScrollBar()->geometry();
	const TQRect horizontalRect = horizontalScrollBar()->geometry();

	TQRect cornerRect;
	cornerRect.setTop( verticalRect.bottom() );
	cornerRect.setBottom( horizontalRect.bottom() );
	cornerRect.setLeft( verticalRect.left() );
	cornerRect.setRight( verticalRect.right() );

	p->fillRect( cornerRect, colorGroup().background() );
    }
}

/*!
    \reimp
*/

bool TQTextEdit::event( TQEvent *e )
{
    if ( e->type() == TQEvent::AccelOverride && !isReadOnly() ) {
	TQKeyEvent* ke = (TQKeyEvent*) e;
	switch((int)(ke->state())) {
	case NoButton:
	case Keypad:
	case ShiftButton:
	    if ( ke->key() < Key_Escape ) {
		ke->accept();
	    } else {
		switch ( ke->key() ) {
		case Key_Return:
		case Key_Enter:
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
	    break;

	case ControlButton:
	case ControlButton|ShiftButton:
	case ControlButton|Keypad:
	case ControlButton|ShiftButton|Keypad:
	    switch ( ke->key() ) {
	    case Key_Tab:
	    case Key_Backtab:
		ke->ignore();
		break;
// Those are too frequently used for application functionality
/*	    case Key_A:
	    case Key_B:
	    case Key_D:
	    case Key_E:
	    case Key_F:
	    case Key_H:
	    case Key_I:
	    case Key_K:
	    case Key_N:
	    case Key_P:
	    case Key_T:
*/
	    case Key_C:
	    case Key_V:
	    case Key_X:
	    case Key_Y:
	    case Key_Z:
	    case Key_Left:
	    case Key_Right:
	    case Key_Up:
	    case Key_Down:
	    case Key_Home:
	    case Key_End:
#if defined (TQ_WS_WIN)
	    case Key_Insert:
	    case Key_Delete:
#endif
		ke->accept();
	    default:
		break;
	    }
	    break;

	default:
	    switch ( ke->key() ) {
#if defined (TQ_WS_WIN)
	    case Key_Insert:
		ke->accept();
#endif
	    default:
		break;
	    }
	    break;
	}
    }

    if ( e->type() == TQEvent::Show ) {
	if (
#ifdef QT_TEXTEDIT_OPTIMIZATION
	     !d->optimMode &&
#endif
	     d->ensureCursorVisibleInShowEvent  ) {
	    ensureCursorVisible();
	    d->ensureCursorVisibleInShowEvent = false;
	}
	if ( !d->scrollToAnchor.isEmpty() ) {
	    scrollToAnchor( d->scrollToAnchor );
	    d->scrollToAnchor = TQString::null;
	}
    }
    return TQWidget::event( e );
}

/*!
    Processes the key event, \a e. By default key events are used to
    provide keyboard navigation and text editing.
*/

void TQTextEdit::keyPressEvent( TQKeyEvent *e )
{
    changeIntervalTimer->stop();
    interval = 10;
    bool unknownKey = false;
    if ( isReadOnly() ) {
	if ( !handleReadOnlyKeyEvent( e ) )
	    TQScrollView::keyPressEvent( e );
	changeIntervalTimer->start( 100, true );
	return;
    }


    bool selChanged = false;
    for ( int i = 1; i < doc->numSelections(); ++i ) // start with 1 as we don't want to remove the Standard-Selection
	selChanged = doc->removeSelection( i ) || selChanged;

    if ( selChanged ) {
	cursor->paragraph()->document()->nextDoubleBuffered = true;
	repaintChanged();
    }

    bool clearUndoRedoInfo = true;


    switch ( e->key() ) {
    case Key_Left:
    case Key_Right: {
	// a bit hacky, but can't change this without introducing new enum values for move and keeping the
	// correct semantics and movement for BiDi and non BiDi text.
	CursorAction a;
	if ( cursor->paragraph()->string()->isRightToLeft() == (e->key() == Key_Right) )
	    a = e->state() & ControlButton ? MoveWordBackward : MoveBackward;
	else
	    a = e->state() & ControlButton ? MoveWordForward : MoveForward;
	moveCursor( a, e->state() & ShiftButton );
	break;
    }
    case Key_Up:
	moveCursor( e->state() & ControlButton ? MovePgUp : MoveUp, e->state() & ShiftButton );
	break;
    case Key_Down:
	moveCursor( e->state() & ControlButton ? MovePgDown : MoveDown, e->state() & ShiftButton );
	break;
    case Key_Home:
	moveCursor( e->state() & ControlButton ? MoveHome : MoveLineStart, e->state() & ShiftButton );
	break;
    case Key_End:
	moveCursor( e->state() & ControlButton ? MoveEnd : MoveLineEnd, e->state() & ShiftButton );
	break;
    case Key_Prior:
	moveCursor( MovePgUp, e->state() & ShiftButton );
	break;
    case Key_Next:
	moveCursor( MovePgDown, e->state() & ShiftButton );
	break;
    case Key_Return: case Key_Enter:
	if ( doc->hasSelection( TQTextDocument::Standard, false ) )
	    removeSelectedText();
	if ( textFormat() == TQt::RichText && ( e->state() & ControlButton ) ) {
	    // Ctrl-Enter inserts a line break in rich text mode
	    insert( TQString( TQChar( 0x2028) ), true, false );
	} else {
#ifndef TQT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	    clearUndoRedoInfo = false;
	    doKeyboardAction( ActionReturn );
	    emit returnPressed();
	}
	break;
    case Key_Delete:
#if defined (TQ_WS_WIN)
	if ( e->state() & ShiftButton ) {
	    cut();
	    break;
	} else
#endif
        if ( doc->hasSelection( TQTextDocument::Standard, true ) ) {
	    removeSelectedText();
	    break;
	}
	doKeyboardAction( e->state() & ControlButton ? ActionWordDelete
			  : ActionDelete );
	clearUndoRedoInfo = false;

	break;
    case Key_Insert:
	if ( e->state() & ShiftButton )
	    paste();
#if defined (TQ_WS_WIN)
	else if ( e->state() & ControlButton )
	    copy();
#endif
	else
	    setOverwriteMode( !isOverwriteMode() );
	break;
    case Key_Backspace:
#if defined (TQ_WS_WIN)
	if ( e->state() & AltButton ) {
	    if (e->state() & ControlButton ) {
		break;
	    } else if ( e->state() & ShiftButton ) {
		redo();
		break;
	    } else {
		undo();
		break;
	    }
	} else
#endif
	if ( doc->hasSelection( TQTextDocument::Standard, true ) ) {
	    removeSelectedText();
	    break;
	}

	doKeyboardAction( e->state() & ControlButton ? ActionWordBackspace
			  : ActionBackspace );
	clearUndoRedoInfo = false;
	break;
    case Key_F16: // Copy key on Sun keyboards
	copy();
	break;
    case Key_F18:  // Paste key on Sun keyboards
	paste();
	break;
    case Key_F20:  // Cut key on Sun keyboards
	cut();
	break;
    case Key_Direction_L:
	if ( doc->textFormat() == TQt::PlainText ) {
	    // change the whole doc
	    TQTextParagraph *p = doc->firstParagraph();
	    while ( p ) {
		p->setDirection( TQChar::DirL );
		p->setAlignment( TQt::AlignLeft );
		p->invalidate( 0 );
		p = p->next();
	    }
	} else {
	    if ( !cursor->paragraph() || cursor->paragraph()->direction() == TQChar::DirL )
		return;
	    cursor->paragraph()->setDirection( TQChar::DirL );
	    if ( cursor->paragraph()->length() <= 1&&
		 ( (cursor->paragraph()->alignment() & (TQt::AlignLeft | TQt::AlignRight) ) != 0 ) )
		setAlignment( TQt::AlignLeft );
	}
	repaintChanged();
	break;
    case Key_Direction_R:
	if ( doc->textFormat() == TQt::PlainText ) {
	    // change the whole doc
	    TQTextParagraph *p = doc->firstParagraph();
	    while ( p ) {
		p->setDirection( TQChar::DirR );
		p->setAlignment( TQt::AlignRight );
		p->invalidate( 0 );
		p = p->next();
	    }
	} else {
	    if ( !cursor->paragraph() || cursor->paragraph()->direction() == TQChar::DirR )
		return;
	    cursor->paragraph()->setDirection( TQChar::DirR );
	    if ( cursor->paragraph()->length() <= 1&&
		 ( (cursor->paragraph()->alignment() & (TQt::AlignLeft | TQt::AlignRight) ) != 0 ) )
		setAlignment( TQt::AlignRight );
	}
	repaintChanged();
	break;
    default: {
	    if ( e->text().length() &&
		( ( !( e->state() & ControlButton ) &&
#ifndef Q_OS_MACX
		  !( e->state() & AltButton ) &&
#endif
		  !( e->state() & MetaButton ) ) ||
		 ( ( (e->state()&ControlButton) | AltButton ) == (ControlButton|AltButton) ) ) &&
		 ( !e->ascii() || e->ascii() >= 32 || e->text() == "\t" ) ) {
		clearUndoRedoInfo = false;
		if ( e->key() == Key_Tab ) {
		    if ( d->tabChangesFocus ) {
			e->ignore();
			break;
		    }
		    if ( textFormat() == TQt::RichText && cursor->index() == 0
			 && ( cursor->paragraph()->isListItem() || cursor->paragraph()->listDepth() ) ) {
			clearUndoRedo();
			undoRedoInfo.type = UndoRedoInfo::Style;
			undoRedoInfo.id = cursor->paragraph()->paragId();
			undoRedoInfo.eid = undoRedoInfo.id;
			undoRedoInfo.styleInformation = TQTextStyleCommand::readStyleInformation( doc, undoRedoInfo.id, undoRedoInfo.eid );
			cursor->paragraph()->setListDepth( cursor->paragraph()->listDepth() +1 );
			clearUndoRedo();
			drawCursor( false );
			repaintChanged();
			drawCursor( true );
			break;
		    }
		} else if ( e->key() == Key_BackTab ) {
		    if ( d->tabChangesFocus ) {
			e->ignore();
			break;
		    }
		}

		if ( ( autoFormatting() & AutoBulletList ) &&
		     textFormat() == TQt::RichText && cursor->index() == 0
		     && !cursor->paragraph()->isListItem()
		     && ( e->text()[0] == '-' || e->text()[0] == '*' ) ) {
			clearUndoRedo();
			undoRedoInfo.type = UndoRedoInfo::Style;
			undoRedoInfo.id = cursor->paragraph()->paragId();
			undoRedoInfo.eid = undoRedoInfo.id;
			undoRedoInfo.styleInformation = TQTextStyleCommand::readStyleInformation( doc, undoRedoInfo.id, undoRedoInfo.eid );
			setParagType( TQStyleSheetItem::DisplayListItem, TQStyleSheetItem::ListDisc );
			clearUndoRedo();
			drawCursor( false );
			repaintChanged();
			drawCursor( true );
			break;
		}
		if (overWrite && !cursor->atParagEnd() && !doc->hasSelection(TQTextDocument::Standard)) {
                    doKeyboardAction(ActionDelete);
                    clearUndoRedoInfo = false;
                }
		TQString t = e->text();
#ifdef TQ_WS_X11
		extern bool tqt_hebrew_keyboard_hack;
		if ( tqt_hebrew_keyboard_hack ) {
		    // the X11 keyboard layout is broken and does not reverse
		    // braces correctly. This is a hack to get halfway correct
		    // behaviour
		    TQTextParagraph *p = cursor->paragraph();
		    if ( p && p->string() && p->string()->isRightToLeft() ) {
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
		insert( t, true, false );
		break;
	    } else if ( e->state() & ControlButton ) {
		switch ( e->key() ) {
		case Key_C: case Key_F16: // Copy key on Sun keyboards
		    copy();
		    break;
		case Key_V:
		    paste();
		    break;
		case Key_X:
		    cut();
		    break;
		case Key_I: case Key_T: case Key_Tab:
		    if ( !d->tabChangesFocus )
			indent();
		    break;
		case Key_A:
#if defined(TQ_WS_X11)
		    moveCursor( MoveLineStart, e->state() & ShiftButton );
#else
		    selectAll( true );
#endif
		    break;
		case Key_B:
		    moveCursor( MoveBackward, e->state() & ShiftButton );
		    break;
		case Key_F:
		    moveCursor( MoveForward, e->state() & ShiftButton );
		    break;
		case Key_D:
		    if ( doc->hasSelection( TQTextDocument::Standard ) ) {
			removeSelectedText();
			break;
		    }
		    doKeyboardAction( ActionDelete );
		    clearUndoRedoInfo = false;
		    break;
		case Key_H:
		    if ( doc->hasSelection( TQTextDocument::Standard ) ) {
			removeSelectedText();
			break;
		    }
		    if ( !cursor->paragraph()->prev() &&
			 cursor->atParagStart() )
			break;

		    doKeyboardAction( ActionBackspace );
		    clearUndoRedoInfo = false;
		    break;
		case Key_E:
		    moveCursor( MoveLineEnd, e->state() & ShiftButton );
		    break;
		case Key_N:
		    moveCursor( MoveDown, e->state() & ShiftButton );
		    break;
		case Key_P:
		    moveCursor( MoveUp, e->state() & ShiftButton );
		    break;
		case Key_Z:
		    if(e->state() & ShiftButton)
			redo();
		    else
			undo();
		    break;
		case Key_Y:
		    redo();
		    break;
		case Key_K:
		    doKeyboardAction( ActionKill );
		    break;
#if defined(TQ_WS_WIN)
		case Key_Insert:
		    copy();
		    break;
		case Key_Delete:
		    del();
		    break;
#endif
		default:
		    unknownKey = false;
		    break;
		}
	    } else {
		unknownKey = true;
	    }
        }
    }

    emit cursorPositionChanged( cursor );
    emit cursorPositionChanged( cursor->paragraph()->paragId(), cursor->index() );
    if ( clearUndoRedoInfo )
	clearUndoRedo();
    changeIntervalTimer->start( 100, true );
    if ( unknownKey )
	e->ignore();
}

/*!
  This function is not intended as polymorphic usage. Just a shared code
  fragment that calls TQWidget::sendMouseEventToInputContext() easily for this
  class.
 */
bool TQTextEdit::sendMouseEventToInputContext( TQMouseEvent *e )
{
#ifndef TQT_NO_IM
    if ( d->composeMode() ) {
	TQTextCursor c( doc );
	if ( c.place( e->pos(), doc->firstParagraph(), false, false, false ) ) {
	    int mousePos = c.index() - d->preeditStart;
	    if ( cursor->globalY() == c.globalY() &&
		 mousePos >= 0 && mousePos < d->preeditLength ) {
		TQWidget::sendMouseEventToInputContext( mousePos, e->type(),
						       e->button(), e->state() );
	    }
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


/*!
    \reimp
*/
void TQTextEdit::imStartEvent( TQIMEvent *e )
{
    if ( isReadOnly() ) {
	e->ignore();
	return;
    }

    if ( hasSelectedText() )
	removeSelectedText();
    d->preeditStart = cursor->index();
    clearUndoRedo();
    undoRedoInfo.type = UndoRedoInfo::IME;
}

/*!
    \reimp
*/
void TQTextEdit::imComposeEvent( TQIMEvent *e )
{
    if ( isReadOnly() ) {
	e->ignore();
	return;
    }

    doc->removeSelection( TQTextDocument::IMCompositionText );
    doc->removeSelection( TQTextDocument::IMSelectionText );

    if ( d->composeMode() && cursor->paragraph() )
	cursor->paragraph()->remove( d->preeditStart, d->preeditLength );
    cursor->setIndex( d->preeditStart );
    d->preeditLength = e->text().length();

    int sellen = e->selectionLength();
    uint insertionFlags = CheckNewLines | RemoveSelected | AsIMCompositionText;
    if ( sellen > 0 ) {
      insertionFlags |= WithIMSelection;
    }
    insert( e->text(), insertionFlags );
    // insert can trigger an imEnd event as it emits a textChanged signal, so better
    // be careful
    if(d->preeditStart != -1) {
	cursor->setIndex( d->preeditStart + d->preeditLength );
	TQTextCursor c = *cursor;
	cursor->setIndex( d->preeditStart );
	doc->setSelectionStart( TQTextDocument::IMCompositionText, *cursor );
	doc->setSelectionEnd( TQTextDocument::IMCompositionText, c );

	cursor->setIndex( d->preeditStart + e->cursorPos() );

	if ( sellen > 0 ) {
	    cursor->setIndex( d->preeditStart + e->cursorPos() + sellen );
	    c = *cursor;
	    cursor->setIndex( d->preeditStart + e->cursorPos() );
	    doc->setSelectionStart( TQTextDocument::IMSelectionText, *cursor );
	    doc->setSelectionEnd( TQTextDocument::IMSelectionText, c );
#if 0
 	    // Disabled for Asian input method that shows candidate
 	    // window. This behavior is same as TQt/E 2.3.7 which supports
 	    // Asian input methods. Asian input methods need start point
 	    // of IM selection text to place candidate window as adjacent
 	    // to the selection text.
	    cursor->setIndex( d->preeditStart + d->preeditLength );
#endif
	}
    }

    updateMicroFocusHint();
    repaintChanged();
}

/*!
    \reimp
*/
void TQTextEdit::imEndEvent( TQIMEvent *e )
{
    if ( isReadOnly() ) {
	e->ignore();
	return;
    }

    doc->removeSelection( TQTextDocument::IMCompositionText );
    doc->removeSelection( TQTextDocument::IMSelectionText );

    if (undoRedoInfo.type == UndoRedoInfo::IME)
        undoRedoInfo.type = UndoRedoInfo::Invalid;

    if ( d->composeMode() && cursor->paragraph() )
	cursor->paragraph()->remove( d->preeditStart, d->preeditLength );
    if ( d->preeditStart >= 0 ) {
        cursor->setIndex( d->preeditStart );
	//TODO: TQt 4 we should use the new virtual insert function
	insert( e->text(), false );
    }
    d->preeditStart = d->preeditLength = -1;

    repaintChanged();
}


static bool qtextedit_ignore_readonly = false;

/*!
    Executes keyboard action \a action. This is normally called by a
    key event handler.
*/

void TQTextEdit::doKeyboardAction( KeyboardAction action )
{
    if ( isReadOnly() && !qtextedit_ignore_readonly )
	return;

    if ( cursor->nestedDepth() != 0 ) // #### for 3.0, disable editing of tables as this is not advanced enough
	return;

    lastFormatted = cursor->paragraph();
    drawCursor( false );
    bool doUpdateCurrentFormat = true;

    switch ( action ) {
    case ActionWordDelete:
    case ActionDelete:
	if ( action == ActionDelete && !cursor->atParagEnd() ) {
	    if ( undoEnabled ) {
		checkUndoRedoInfo( UndoRedoInfo::Delete );
		if ( !undoRedoInfo.valid() ) {
		    undoRedoInfo.id = cursor->paragraph()->paragId();
		    undoRedoInfo.index = cursor->index();
		    undoRedoInfo.d->text = TQString::null;
		}
		int idx = cursor->index();
		do {
		    undoRedoInfo.d->text.insert( undoRedoInfo.d->text.length(), cursor->paragraph()->at( idx++ ), true );
		} while ( !cursor->paragraph()->string()->validCursorPosition( idx ) );
	    }
	    cursor->remove();
	} else {
	    clearUndoRedo();
	    doc->setSelectionStart( TQTextDocument::Temp, *cursor );
	    if ( action == ActionWordDelete && !cursor->atParagEnd() ) {
		cursor->gotoNextWord();
	    } else {
		cursor->gotoNextLetter();
	    }
	    doc->setSelectionEnd( TQTextDocument::Temp, *cursor );
	    removeSelectedText( TQTextDocument::Temp );
	}
	break;
    case ActionWordBackspace:
    case ActionBackspace:
	if ( textFormat() == TQt::RichText
	     && (cursor->paragraph()->isListItem()
		 || cursor->paragraph()->listDepth() )
	     && cursor->index() == 0 ) {
	    if ( undoEnabled ) {
		clearUndoRedo();
		undoRedoInfo.type = UndoRedoInfo::Style;
		undoRedoInfo.id = cursor->paragraph()->paragId();
		undoRedoInfo.eid = undoRedoInfo.id;
		undoRedoInfo.styleInformation = TQTextStyleCommand::readStyleInformation( doc, undoRedoInfo.id, undoRedoInfo.eid );
	    }
	    int ldepth = cursor->paragraph()->listDepth();
	    if ( cursor->paragraph()->isListItem() && ldepth == 1 ) {
		cursor->paragraph()->setListItem( false );
	    } else if ( TQMAX( ldepth, 1 ) == 1 ) {
		cursor->paragraph()->setListItem( false );
		cursor->paragraph()->setListDepth( 0 );
	    } else {
		cursor->paragraph()->setListDepth( ldepth - 1 );
	    }
	    clearUndoRedo();
	    lastFormatted = cursor->paragraph();
	    repaintChanged();
	    drawCursor( true );
	    return;
	}

	if ( action == ActionBackspace && !cursor->atParagStart() ) {
	    if ( undoEnabled ) {
		checkUndoRedoInfo( UndoRedoInfo::Delete );
		if ( !undoRedoInfo.valid() ) {
		    undoRedoInfo.id = cursor->paragraph()->paragId();
		    undoRedoInfo.index = cursor->index();
		    undoRedoInfo.d->text = TQString::null;
		}
		int cur_idx = cursor->index();
		const TQTextParagraph *para = cursor->paragraph();
		if (para->at(cur_idx - 1)->c.isLowSurrogate() && cur_idx > 1 &&
		       para->at(cur_idx - 2)->c.isHighSurrogate())
		{
		    undoRedoInfo.d->text.insert( 0, para->at(cur_idx - 1), true );
		    undoRedoInfo.d->text.insert( 0, para->at(cur_idx - 2), true );
		    undoRedoInfo.index = cursor->index() - 2;
		}
		else
		{
		    undoRedoInfo.d->text.insert( 0, para->at(cur_idx - 1), true );
		    undoRedoInfo.index = cursor->index() - 1;
		}
	    }
	    cursor->removePreviousChar();
	    lastFormatted = cursor->paragraph();
	} else if ( cursor->paragraph()->prev()
		    || (action == ActionWordBackspace
			&& !cursor->atParagStart()) ) {
	    clearUndoRedo();
	    doc->setSelectionStart( TQTextDocument::Temp, *cursor );
	    if ( action == ActionWordBackspace && !cursor->atParagStart() ) {
		cursor->gotoPreviousWord();
	    } else {
		cursor->gotoPreviousLetter();
	    }
	    doc->setSelectionEnd( TQTextDocument::Temp, *cursor );
	    removeSelectedText( TQTextDocument::Temp );
	}
	break;
    case ActionReturn:
	if ( undoEnabled ) {
	    checkUndoRedoInfo( UndoRedoInfo::Return );
	    if ( !undoRedoInfo.valid() ) {
		undoRedoInfo.id = cursor->paragraph()->paragId();
		undoRedoInfo.index = cursor->index();
		undoRedoInfo.d->text = TQString::null;
	    }
	    undoRedoInfo.d->text += "\n";
	}
	cursor->splitAndInsertEmptyParagraph();
	if ( cursor->paragraph()->prev() ) {
	    lastFormatted = cursor->paragraph()->prev();
	    lastFormatted->invalidate( 0 );
	}
	doUpdateCurrentFormat = false;
	break;
    case ActionKill:
	clearUndoRedo();
	doc->setSelectionStart( TQTextDocument::Temp, *cursor );
	if ( cursor->atParagEnd() )
	    cursor->gotoNextLetter();
	else
	    cursor->setIndex( cursor->paragraph()->length() - 1 );
	doc->setSelectionEnd( TQTextDocument::Temp, *cursor );
	removeSelectedText( TQTextDocument::Temp );
	break;
    }

    formatMore();
    repaintChanged();
    ensureCursorVisible();
    drawCursor( true );
    updateMicroFocusHint();
    if ( doUpdateCurrentFormat )
	updateCurrentFormat();
    setModified();
    emit textChanged();
}

void TQTextEdit::readFormats( TQTextCursor &c1, TQTextCursor &c2, TQTextString &text, bool fillStyles )
{
#ifndef TQT_NO_DATASTREAM
    TQDataStream styleStream( undoRedoInfo.styleInformation, IO_WriteOnly );
#endif
    c2.restoreState();
    c1.restoreState();
    int lastIndex = text.length();
    if ( c1.paragraph() == c2.paragraph() ) {
	for ( int i = c1.index(); i < c2.index(); ++i )
	    text.insert( lastIndex + i - c1.index(), c1.paragraph()->at( i ), true );
#ifndef TQT_NO_DATASTREAM
	if ( fillStyles ) {
	    styleStream << (int) 1;
	    c1.paragraph()->writeStyleInformation( styleStream );
	}
#endif
    } else {
	int i;
	for ( i = c1.index(); i < c1.paragraph()->length()-1; ++i )
	    text.insert( lastIndex++, c1.paragraph()->at( i ), true );
	int num = 2; // start and end, being different
	text += "\n"; lastIndex++;

        if (c1.paragraph()->next() != c2.paragraph()) {
            num += text.appendParagraphs(c1.paragraph()->next(), c2.paragraph());
            lastIndex = text.length();
        }

	for ( i = 0; i < c2.index(); ++i )
	    text.insert( i + lastIndex, c2.paragraph()->at( i ), true );
#ifndef TQT_NO_DATASTREAM
	if ( fillStyles ) {
	    styleStream << num;
	    for ( TQTextParagraph *p = c1.paragraph(); --num >= 0; p = p->next() )
		p->writeStyleInformation( styleStream );
	}
#endif
    }
}

/*!
    Removes the selection \a selNum (by default 0). This does not
    remove the selected text.

    \sa removeSelectedText()
*/

void TQTextEdit::removeSelection( int selNum )
{
    doc->removeSelection( selNum );
    repaintChanged();
}

/*!
    Deletes the text of selection \a selNum (by default, the default
    selection, 0). If there is no selected text nothing happens.

    \sa selectedText removeSelection()
*/

void TQTextEdit::removeSelectedText( int selNum )
{
    if(selNum != 0)
        resetInputContext();

    TQTextCursor c1 = doc->selectionStartCursor( selNum );
    c1.restoreState();
    TQTextCursor c2 = doc->selectionEndCursor( selNum );
    c2.restoreState();

    // ### no support for editing tables yet, plus security for broken selections
    if ( c1.nestedDepth() || c2.nestedDepth() )
	return;

    for ( int i = 0; i < (int)doc->numSelections(); ++i ) {
	if ( i == selNum )
	    continue;
	doc->removeSelection( i );
    }

    drawCursor( false );
    if ( undoEnabled ) {
	checkUndoRedoInfo( UndoRedoInfo::RemoveSelected );
	if ( !undoRedoInfo.valid() ) {
	    doc->selectionStart( selNum, undoRedoInfo.id, undoRedoInfo.index );
	    undoRedoInfo.d->text = TQString::null;
	}
	readFormats( c1, c2, undoRedoInfo.d->text, true );
    }

    doc->removeSelectedText( selNum, cursor );
    if ( cursor->isValid() ) {
	lastFormatted = 0; // make sync a noop
	ensureCursorVisible();
	lastFormatted = cursor->paragraph();
	formatMore();
	repaintContents( false );
	ensureCursorVisible();
	drawCursor( true );
	clearUndoRedo();
#if defined(TQ_WS_WIN)
	// there seems to be a problem with repainting or erasing the area
	// of the scrollview which is not the contents on windows
	if ( contentsHeight() < visibleHeight() )
	    viewport()->repaint( 0, contentsHeight(), visibleWidth(), visibleHeight() - contentsHeight(), true );
#endif
#ifndef TQT_NO_CURSOR
	viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	updateMicroFocusHint();
    } else {
	delete cursor;
	cursor = new TQTextCursor( doc );
	drawCursor( true );
	repaintContents( true );
    }
    setModified();
    emit textChanged();
    emit selectionChanged();
    emit copyAvailable( doc->hasSelection( TQTextDocument::Standard ) );
}

/*!
    Moves the text cursor according to \a action. This is normally
    used by some key event handler. \a select specifies whether the
    text between the current cursor position and the new position
    should be selected.
*/

void TQTextEdit::moveCursor( CursorAction action, bool select )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode )
	return;
#endif
#ifdef TQ_WS_MACX
    TQTextCursor c1 = *cursor;
    TQTextCursor c2;
#endif
    drawCursor( false );
    if ( select ) {
	if ( !doc->hasSelection( TQTextDocument::Standard ) )
	    doc->setSelectionStart( TQTextDocument::Standard, *cursor );
	moveCursor( action );
#ifdef TQ_WS_MACX
	c2 = *cursor;
	if (c1 == c2)
	    if (action == MoveDown || action == MovePgDown)
		moveCursor( MoveEnd );
	    else if (action == MoveUp || action == MovePgUp)
		moveCursor( MoveHome );
#endif
	if ( doc->setSelectionEnd( TQTextDocument::Standard, *cursor ) ) {
	    cursor->paragraph()->document()->nextDoubleBuffered = true;
	    repaintChanged();
	} else {
	    drawCursor( true );
	}
	ensureCursorVisible();
	emit selectionChanged();
	emit copyAvailable( doc->hasSelection( TQTextDocument::Standard ) );
    } else {
#ifdef TQ_WS_MACX
	TQTextCursor cStart = doc->selectionStartCursor( TQTextDocument::Standard );
	TQTextCursor cEnd = doc->selectionEndCursor( TQTextDocument::Standard );
	bool redraw = doc->removeSelection( TQTextDocument::Standard );
	if (redraw && action == MoveDown)
	    *cursor = cEnd;
	else if (redraw && action == MoveUp)
	    *cursor = cStart;
	if (redraw && action == MoveForward)
	    *cursor = cEnd;
	else if (redraw && action == MoveBackward)
	    *cursor = cStart;
	else
	    moveCursor( action );
	c2 = *cursor;
	if (c1 == c2)
	    if (action == MoveDown)
		moveCursor( MoveEnd );
	    else if (action == MoveUp)
		moveCursor( MoveHome );
#else
	bool redraw = doc->removeSelection( TQTextDocument::Standard );
	moveCursor( action );
#endif
	if ( !redraw ) {
	    ensureCursorVisible();
	    drawCursor( true );
	} else {
	    cursor->paragraph()->document()->nextDoubleBuffered = true;
	    repaintChanged();
	    ensureCursorVisible();
	    drawCursor( true );
#ifndef TQT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
	if ( redraw ) {
	    emit copyAvailable( doc->hasSelection( TQTextDocument::Standard ) );
	    emit selectionChanged();
	}
    }

    drawCursor( true );
    updateCurrentFormat();
    updateMicroFocusHint();
}

/*!
    \overload
*/

void TQTextEdit::moveCursor( CursorAction action )
{
    resetInputContext();
    switch ( action ) {
    case MoveBackward:
	cursor->gotoPreviousLetter();
	break;
    case MoveWordBackward:
	cursor->gotoPreviousWord();
	break;
    case MoveForward:
	cursor->gotoNextLetter();
	break;
    case MoveWordForward:
	cursor->gotoNextWord();
	break;
    case MoveUp:
	cursor->gotoUp();
	break;
    case MovePgUp:
	cursor->gotoPageUp( visibleHeight() );
	break;
    case MoveDown:
	cursor->gotoDown();
	break;
    case MovePgDown:
	cursor->gotoPageDown( visibleHeight() );
	break;
    case MoveLineStart:
	cursor->gotoLineStart();
	break;
    case MoveHome:
	cursor->gotoHome();
	break;
    case MoveLineEnd:
	cursor->gotoLineEnd();
	break;
    case MoveEnd:
	ensureFormatted( doc->lastParagraph() );
	cursor->gotoEnd();
	break;
    }
    updateMicroFocusHint();
    updateCurrentFormat();
}

/*!
    \reimp
*/

void TQTextEdit::resizeEvent( TQResizeEvent *e )
{
    TQScrollView::resizeEvent( e );
    if ( doc->visibleWidth() == 0 )
	doResize();
}

/*!
    \reimp
*/

void TQTextEdit::viewportResizeEvent( TQResizeEvent *e )
{
    TQScrollView::viewportResizeEvent( e );
    if ( e->oldSize().width() != e->size().width() ) {
	bool stayAtBottom = e->oldSize().height() != e->size().height() &&
	       contentsY() > 0 && contentsY() >= doc->height() - e->oldSize().height();
	doResize();
	if ( stayAtBottom )
	    scrollToBottom();
    }
}

/*!
    Ensures that the cursor is visible by scrolling the text edit if
    necessary.

    \sa setCursorPosition()
*/

void TQTextEdit::ensureCursorVisible()
{
    // Not visible or the user is draging the window, so don't position to caret yet
    if ( !isUpdatesEnabled() || !isVisible() || isHorizontalSliderPressed() || isVerticalSliderPressed() ) {
	d->ensureCursorVisibleInShowEvent = true;
	return;
    }
    sync();
    TQTextStringChar *chr = cursor->paragraph()->at( cursor->index() );
    int h = cursor->paragraph()->lineHeightOfChar( cursor->index() );
    int x = cursor->paragraph()->rect().x() + chr->x + cursor->offsetX();
    int y = 0; int dummy;
    cursor->paragraph()->lineHeightOfChar( cursor->index(), &dummy, &y );
    y += cursor->paragraph()->rect().y() + cursor->offsetY();
    int w = 1;
    ensureVisible( x, y + h / 2, w, h / 2 + 2 );
}

/*!
    \internal
*/
void TQTextEdit::sliderReleased()
{
    if ( d->ensureCursorVisibleInShowEvent && isVisible() ) {
	d->ensureCursorVisibleInShowEvent = false;
	ensureCursorVisible();
    }
}

/*!
    \internal
*/
void TQTextEdit::drawCursor( bool visible )
{
    if ( !isUpdatesEnabled() ||
	 !viewport()->isUpdatesEnabled() ||
	 !cursor->paragraph() ||
	 !cursor->paragraph()->isValid() ||
	 ( !style().styleHint( TQStyle::SH_BlinkCursorWhenTextSelected ) &&
	   ( d->optimMode ? optimHasSelection() : doc->hasSelection( TQTextDocument::Standard, true ))) ||
	 ( visible && !hasFocus() && !viewport()->hasFocus() && !inDnD ) ||
         doc->hasSelection( TQTextDocument::IMSelectionText ) ||
	 isReadOnly() )
	return;

    // Asian users regard selection text as cursor on candidate
    // selection phase of input method, so ordinary cursor should be
    // invisible if IM selection text exists.
    if ( doc->hasSelection( TQTextDocument::IMSelectionText ) ) {
        visible = false;
    }

    TQPainter p( viewport() );
    TQRect r( cursor->topParagraph()->rect() );
    cursor->paragraph()->setChanged( true );
    p.translate( -contentsX() + cursor->totalOffsetX(), -contentsY() + cursor->totalOffsetY() );
    TQPixmap *pix = 0;
    TQColorGroup cg( colorGroup() );
    const TQColorGroup::ColorRole backRole = TQPalette::backgroundRoleFromMode(backgroundMode());
    if ( cursor->paragraph()->background() )
	cg.setBrush( backRole, *cursor->paragraph()->background() );
    else if ( doc->paper() )
	cg.setBrush( backRole, *doc->paper() );
    p.setBrushOrigin( -contentsX(), -contentsY() );
    cursor->paragraph()->document()->nextDoubleBuffered = true;
    if ( !cursor->nestedDepth() ) {
	int h = cursor->paragraph()->lineHeightOfChar( cursor->index() );
	int dist = 5;
	if ( ( cursor->paragraph()->alignment() & TQt::AlignJustify ) == TQt::AlignJustify )
	    dist = 50;
	int x = r.x() - cursor->totalOffsetX() + cursor->x() - dist;
	x = TQMAX( x, 0 );
	p.setClipRect( TQRect( x - contentsX(),
			      r.y() - cursor->totalOffsetY() + cursor->y() - contentsY(), 2 * dist, h ) );
	doc->drawParagraph( &p, cursor->paragraph(), x,
			r.y() - cursor->totalOffsetY() + cursor->y(), 2 * dist, h, pix, cg, visible, cursor );
    } else {
	doc->drawParagraph( &p, cursor->paragraph(), r.x() - cursor->totalOffsetX(),
			r.y() - cursor->totalOffsetY(), r.width(), r.height(),
			pix, cg, visible, cursor );
    }
    cursorVisible = visible;
}

enum {
    IdUndo = 0,
    IdRedo = 1,
    IdCut = 2,
    IdCopy = 3,
    IdPaste = 4,
    IdClear = 5,
    IdSelectAll = 6
};

/*!
    \reimp
*/
#ifndef TQT_NO_WHEELEVENT
void TQTextEdit::contentsWheelEvent( TQWheelEvent *e )
{
    if ( isReadOnly() ) {
	if ( e->state() & ControlButton ) {
	    if ( e->delta() > 0 )
		zoomOut();
	    else if ( e->delta() < 0 )
		zoomIn();
	    return;
	}
    }
    TQScrollView::contentsWheelEvent( e );
}
#endif

/*!
    \reimp
*/

void TQTextEdit::contentsMousePressEvent( TQMouseEvent *e )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	optimMousePressEvent( e );
	return;
    }
#endif

    if ( sendMouseEventToInputContext( e ) )
	return;

    if ( d->trippleClickTimer->isActive() &&
	 ( e->globalPos() - d->trippleClickPoint ).manhattanLength() <
	 TQApplication::startDragDistance() ) {
	TQTextCursor c1 = *cursor;
	TQTextCursor c2 = *cursor;
	c1.gotoLineStart();
	c2.gotoLineEnd();
	doc->setSelectionStart( TQTextDocument::Standard, c1 );
	doc->setSelectionEnd( TQTextDocument::Standard, c2 );
	*cursor = c2;
	repaintChanged();
	mousePressed = true;
	return;
    }

    clearUndoRedo();
    TQTextCursor oldCursor = *cursor;
    TQTextCursor c = *cursor;
    mousePos = e->pos();
    mightStartDrag = false;
    pressedLink = TQString::null;
    d->pressedName = TQString::null;

    if ( e->button() == LeftButton ) {
	mousePressed = true;
	drawCursor( false );
	placeCursor( e->pos() );
	ensureCursorVisible();

	if ( isReadOnly() && linksEnabled() ) {
	    TQTextCursor c = *cursor;
	    placeCursor( e->pos(), &c, true );
	    if ( c.paragraph() && c.paragraph()->at( c.index() ) &&
		 c.paragraph()->at( c.index() )->isAnchor() ) {
		pressedLink = c.paragraph()->at( c.index() )->anchorHref();
		d->pressedName = c.paragraph()->at( c.index() )->anchorName();
	    }
	}

#ifndef TQT_NO_DRAGANDDROP
	if ( doc->inSelection( TQTextDocument::Standard, e->pos() ) ) {
	    mightStartDrag = true;
	    drawCursor( true );
	    dragStartTimer->start( TQApplication::startDragTime(), true );
	    dragStartPos = e->pos();
	    return;
	}
#endif

	bool redraw = false;
	if ( doc->hasSelection( TQTextDocument::Standard ) ) {
	    if ( !( e->state() & ShiftButton ) ) {
		redraw = doc->removeSelection( TQTextDocument::Standard );
		doc->setSelectionStart( TQTextDocument::Standard, *cursor );
	    } else {
		redraw = doc->setSelectionEnd( TQTextDocument::Standard, *cursor ) || redraw;
	    }
	} else {
	    if ( isReadOnly() || !( e->state() & ShiftButton ) ) {
		doc->setSelectionStart( TQTextDocument::Standard, *cursor );
	    } else {
		doc->setSelectionStart( TQTextDocument::Standard, c );
		redraw = doc->setSelectionEnd( TQTextDocument::Standard, *cursor ) || redraw;
	    }
	}

	for ( int i = 1; i < doc->numSelections(); ++i ) // start with 1 as we don't want to remove the Standard-Selection
	    redraw = doc->removeSelection( i ) || redraw;

	if ( !redraw ) {
	    drawCursor( true );
	} else {
	    repaintChanged();
#ifndef TQT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
    } else if ( e->button() == MidButton ) {
	bool redraw = doc->removeSelection( TQTextDocument::Standard );
	if ( !redraw ) {
	    drawCursor( true );
	} else {
	    repaintChanged();
#ifndef TQT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
    }

    if ( *cursor != oldCursor )
	updateCurrentFormat();
}

/*!
    \reimp
*/

void TQTextEdit::contentsMouseMoveEvent( TQMouseEvent *e )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	optimMouseMoveEvent( e );
	return;
    }
#endif
    if ( sendMouseEventToInputContext( e ) ) {
	// don't return from here to avoid cursor vanishing
    } else if ( mousePressed ) {
#ifndef TQT_NO_DRAGANDDROP
	if ( mightStartDrag ) {
	    dragStartTimer->stop();
	    if ( ( e->pos() - dragStartPos ).manhattanLength() > TQApplication::startDragDistance() ) {
		TQGuardedPtr<TQTextEdit> guard( this );
		startDrag();
		if (guard.isNull()) // we got deleted during the dnd
                    return;
	    }
#ifndef TQT_NO_CURSOR
	    if ( !isReadOnly() )
		viewport()->setCursor( ibeamCursor );
#endif
	    return;
	}
#endif
	mousePos = e->pos();
	handleMouseMove( mousePos );
	oldMousePos = mousePos;
    }

#ifndef TQT_NO_CURSOR
    if ( !isReadOnly() && !mousePressed ) {
	if ( doc->hasSelection( TQTextDocument::Standard ) && doc->inSelection( TQTextDocument::Standard, e->pos() ) )
	    viewport()->setCursor( arrowCursor );
	else
	    viewport()->setCursor( ibeamCursor );
    }
#endif
    updateCursor( e->pos() );
}

void TQTextEdit::copyToClipboard()
{
#ifndef TQT_NO_CLIPBOARD
    if (TQApplication::clipboard()->supportsSelection()) {
	d->clipboard_mode = TQClipboard::Selection;

	// don't listen to selection changes
	disconnect( TQApplication::clipboard(), TQ_SIGNAL(selectionChanged()), this, 0);
	copy();
	// listen to selection changes
	connect( TQApplication::clipboard(), TQ_SIGNAL(selectionChanged()),
		 this, TQ_SLOT(clipboardChanged()) );

	d->clipboard_mode = TQClipboard::Clipboard;
    }
#endif
}

/*!
    \reimp
*/

void TQTextEdit::contentsMouseReleaseEvent( TQMouseEvent * e )
{
    if ( !inDoubleClick && !d->composeMode() ) { // could be the release of a dblclick
	int para = 0;
	int index = charAt( e->pos(), &para );
	emit clicked( para, index );
    }
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	optimMouseReleaseEvent( e );
	return;
    }
#endif
    if ( sendMouseEventToInputContext( e ) )
	return;
    TQTextCursor oldCursor = *cursor;
    if ( scrollTimer->isActive() )
	scrollTimer->stop();
#ifndef TQT_NO_DRAGANDDROP
    if ( dragStartTimer->isActive() )
	dragStartTimer->stop();
    if ( mightStartDrag ) {
	selectAll( false );
	mousePressed = false;
    }
#endif
    bool mouseWasPressed = mousePressed;
    if ( mousePressed ) {
	mousePressed = false;
	copyToClipboard();
    }
#ifndef TQT_NO_CLIPBOARD
    else if ( e->button() == MidButton && !isReadOnly() ) {
        // only do middle-click pasting on systems that have selections (ie. X11)
        if (TQApplication::clipboard()->supportsSelection()) {
            drawCursor( false );
            placeCursor( e->pos() );
            ensureCursorVisible();
            doc->setSelectionStart( TQTextDocument::Standard, oldCursor );
            bool redraw = false;
            if ( doc->hasSelection( TQTextDocument::Standard ) ) {
                redraw = doc->removeSelection( TQTextDocument::Standard );
                doc->setSelectionStart( TQTextDocument::Standard, *cursor );
            } else {
                doc->setSelectionStart( TQTextDocument::Standard, *cursor );
            }
            // start with 1 as we don't want to remove the Standard-Selection
            for ( int i = 1; i < doc->numSelections(); ++i )
                redraw = doc->removeSelection( i ) || redraw;
            if ( !redraw ) {
                drawCursor( true );
            } else {
                repaintChanged();
#ifndef TQT_NO_CURSOR
                viewport()->setCursor( ibeamCursor );
#endif
            }
	    d->clipboard_mode = TQClipboard::Selection;
            paste();
	    d->clipboard_mode = TQClipboard::Clipboard;
        }
    }
#endif
    emit cursorPositionChanged( cursor );
    emit cursorPositionChanged( cursor->paragraph()->paragId(), cursor->index() );
    if ( oldCursor != *cursor )
	updateCurrentFormat();
    inDoubleClick = false;

#ifndef TQT_NO_NETWORKPROTOCOL
    if ( (   (!onLink.isEmpty() && onLink == pressedLink)
	  || (!d->onName.isEmpty() && d->onName == d->pressedName))
	 && linksEnabled() && mouseWasPressed ) {
	if (!onLink.isEmpty()) {
	    TQUrl u( doc->context(), onLink, true );
	    emitLinkClicked( u.toString( false, false ) );
	}
	if (::tqt_cast<TQTextBrowser*>(this)) { // change for 4.0
	    TQConnectionList *clist = receivers(
			"anchorClicked(const TQString&,const TQString&)");
	    if (!signalsBlocked() && clist) {
		TQUObject o[3];
		static_QUType_TQString.set(o+1, d->onName);
		static_QUType_TQString.set(o+2, onLink);
		activate_signal( clist, o);
	    }
	}

	// emitting linkClicked() may result in that the cursor winds
	// up hovering over a different valid link - check this and
	// set the appropriate cursor shape
	updateCursor( e->pos() );
    }
#endif
    drawCursor( true );
    if ( !doc->hasSelection( TQTextDocument::Standard, true ) )
	doc->removeSelection( TQTextDocument::Standard );

    emit copyAvailable( doc->hasSelection( TQTextDocument::Standard ) );
    emit selectionChanged();
}

/*!
    \reimp
*/

void TQTextEdit::contentsMouseDoubleClickEvent( TQMouseEvent * e )
{
    if ( e->button() != TQt::LeftButton && !d->composeMode() ) {
	e->ignore();
	return;
    }
    int para = 0;
    int index = charAt( e->pos(), &para );
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	TQString str = d->od->lines[ LOGOFFSET(para) ];
	int startIdx = index, endIdx = index, i;
	if ( !str[ index ].isSpace() ) {
	    i = startIdx;
	    // find start of word
	    while ( i >= 0 && !str[ i ].isSpace() ) {
		startIdx = i--;
	    }
	    i = endIdx;
	    // find end of word..
	    while ( (uint) i < str.length() && !str[ i ].isSpace() ) {
		endIdx = ++i;
	    }
	    // ..and start of next
	    while ( (uint) i < str.length() && str[ i ].isSpace() ) {
		endIdx = ++i;
	    }
	    optimSetSelection( para, startIdx, para, endIdx );
	    repaintContents( false );
	}
    } else
#endif
    {
	if ( sendMouseEventToInputContext( e ) )
	    return;

	TQTextCursor c1 = *cursor;
	TQTextCursor c2 = *cursor;
#if defined(Q_OS_MAC)
	TQTextParagraph *para = cursor->paragraph();
	if ( cursor->isValid() ) {
	    if ( para->at( cursor->index() )->c.isLetterOrNumber() ) {
		while ( c1.index() > 0 &&
			c1.paragraph()->at( c1.index()-1 )->c.isLetterOrNumber() )
		    c1.gotoPreviousLetter();
		while ( c2.paragraph()->at( c2.index() )->c.isLetterOrNumber() &&
			!c2.atParagEnd() )
		    c2.gotoNextLetter();
	    } else if ( para->at( cursor->index() )->c.isSpace() ) {
		while ( c1.index() > 0 &&
			c1.paragraph()->at( c1.index()-1 )->c.isSpace() )
		    c1.gotoPreviousLetter();
		while ( c2.paragraph()->at( c2.index() )->c.isSpace() &&
			!c2.atParagEnd() )
		    c2.gotoNextLetter();
	    } else if ( !c2.atParagEnd() ) {
		c2.gotoNextLetter();
	    }
	}
#else
	if ( cursor->index() > 0 && !cursor->paragraph()->at( cursor->index()-1 )->c.isSpace() )
	    c1.gotoPreviousWord();
	if ( !cursor->paragraph()->at( cursor->index() )->c.isSpace() && !cursor->atParagEnd() )
	    c2.gotoNextWord();
#endif
	doc->setSelectionStart( TQTextDocument::Standard, c1 );
	doc->setSelectionEnd( TQTextDocument::Standard, c2 );

	*cursor = c2;

	repaintChanged();

	d->trippleClickTimer->start( tqApp->doubleClickInterval(), true );
	d->trippleClickPoint = e->globalPos();
    }
    inDoubleClick = true;
    mousePressed = true;
    emit doubleClicked( para, index );
}

#ifndef TQT_NO_DRAGANDDROP

/*!
    \reimp
*/

void TQTextEdit::contentsDragEnterEvent( TQDragEnterEvent *e )
{
    if ( isReadOnly() || !TQTextDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }
    e->acceptAction();
    inDnD = true;
}

/*!
    \reimp
*/

void TQTextEdit::contentsDragMoveEvent( TQDragMoveEvent *e )
{
    if ( isReadOnly() || !TQTextDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }
    drawCursor( false );
    placeCursor( e->pos(),  cursor );
    drawCursor( true );
    e->acceptAction();
}

/*!
    \reimp
*/

void TQTextEdit::contentsDragLeaveEvent( TQDragLeaveEvent * )
{
    drawCursor( false );
    inDnD = false;
}

/*!
    \reimp
*/

void TQTextEdit::contentsDropEvent( TQDropEvent *e )
{
    if ( isReadOnly() )
	return;
    inDnD = false;
    e->acceptAction();
    bool intern = false;
    if ( TQRichTextDrag::canDecode( e ) ) {
	bool hasSel = doc->hasSelection( TQTextDocument::Standard );
	bool internalDrag = e->source() == this || e->source() == viewport();
	int dropId, dropIndex;
	TQTextCursor insertCursor = *cursor;
	dropId = cursor->paragraph()->paragId();
	dropIndex = cursor->index();
	if ( hasSel && internalDrag ) {
	    TQTextCursor c1, c2;
	    int selStartId, selStartIndex;
	    int selEndId, selEndIndex;
	    c1 = doc->selectionStartCursor( TQTextDocument::Standard );
	    c1.restoreState();
	    c2 = doc->selectionEndCursor( TQTextDocument::Standard );
	    c2.restoreState();
	    selStartId = c1.paragraph()->paragId();
	    selStartIndex = c1.index();
	    selEndId = c2.paragraph()->paragId();
	    selEndIndex = c2.index();
	    if ( ( ( dropId > selStartId ) ||
		   ( dropId == selStartId && dropIndex > selStartIndex ) ) &&
		 ( ( dropId < selEndId ) ||
		   ( dropId == selEndId && dropIndex <= selEndIndex ) ) )
		insertCursor = c1;
	    if ( dropId == selEndId && dropIndex > selEndIndex ) {
		insertCursor = c1;
		if ( selStartId == selEndId ) {
		    insertCursor.setIndex( dropIndex -
					   ( selEndIndex - selStartIndex ) );
		} else {
		    insertCursor.setIndex( dropIndex - selEndIndex +
					   selStartIndex );
		}
	    }
	 }

	if ( internalDrag && e->action() == TQDropEvent::Move ) {
	    removeSelectedText();
	    intern = true;
	    doc->removeSelection( TQTextDocument::Standard );
	} else {
	    doc->removeSelection( TQTextDocument::Standard );
#ifndef TQT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
	drawCursor( false );
	cursor->setParagraph( insertCursor.paragraph() );
	cursor->setIndex( insertCursor.index() );
	drawCursor( true );
	if ( !cursor->nestedDepth() ) {
	    TQString subType = "plain";
	    if ( textFormat() != PlainText ) {
		if ( e->provides( "application/x-qrichtext" ) )
		    subType = "x-qrichtext";
	    }
#ifndef TQT_NO_CLIPBOARD
	    pasteSubType( subType.latin1(), e );
#endif
	    // emit appropriate signals.
	    emit selectionChanged();
	    emit cursorPositionChanged( cursor );
	    emit cursorPositionChanged( cursor->paragraph()->paragId(), cursor->index() );
	} else {
	    if ( intern )
		undo();
	    e->ignore();
	}
    }
}

#endif

/*!
    \reimp
*/
void TQTextEdit::contentsContextMenuEvent( TQContextMenuEvent *e )
{
    e->accept();
#ifndef TQT_NO_IM
    if ( d->composeMode() )
	return;
#endif

    clearUndoRedo();
    mousePressed = false;

#ifndef TQT_NO_POPUPMENU
    TQGuardedPtr<TQTextEdit> that = this;
    TQGuardedPtr<TQPopupMenu> popup = createPopupMenu( e->pos() );
    if ( !popup )
	popup = createPopupMenu();
    if ( !popup )
	return;

    int r = popup->exec( e->globalPos() );
    delete popup;
    if (!that)
        return;

    if ( r == d->id[ IdClear ] )
	clear();
    else if ( r == d->id[ IdSelectAll ] ) {
	selectAll();
#ifndef TQT_NO_CLIPBOARD
        // if the clipboard support selections, put the newly selected text into
        // the clipboard
	if (TQApplication::clipboard()->supportsSelection()) {
	    d->clipboard_mode = TQClipboard::Selection;

            // don't listen to selection changes
            disconnect( TQApplication::clipboard(), TQ_SIGNAL(selectionChanged()), this, 0);
	    copy();
            // listen to selection changes
            connect( TQApplication::clipboard(), TQ_SIGNAL(selectionChanged()),
                     this, TQ_SLOT(clipboardChanged()) );

	    d->clipboard_mode = TQClipboard::Clipboard;
	}
#endif
    } else if ( r == d->id[ IdUndo ] )
	undo();
    else if ( r == d->id[ IdRedo ] )
	redo();
#ifndef TQT_NO_CLIPBOARD
    else if ( r == d->id[ IdCut ] )
	cut();
    else if ( r == d->id[ IdCopy ] )
	copy();
    else if ( r == d->id[ IdPaste ] )
	paste();
#endif
#endif
}


void TQTextEdit::autoScrollTimerDone()
{
    if ( mousePressed )
	handleMouseMove(  viewportToContents( viewport()->mapFromGlobal( TQCursor::pos() )  ) );
}

void TQTextEdit::handleMouseMove( const TQPoint& pos )
{
    if ( !mousePressed )
	return;

    if ( (!scrollTimer->isActive() && pos.y() < contentsY()) || pos.y() > contentsY() + visibleHeight() )
	scrollTimer->start( 100, false );
    else if ( scrollTimer->isActive() && pos.y() >= contentsY() && pos.y() <= contentsY() + visibleHeight() )
	scrollTimer->stop();

    drawCursor( false );
    TQTextCursor oldCursor = *cursor;

    placeCursor( pos );

    if ( inDoubleClick ) {
	TQTextCursor cl = *cursor;
	cl.gotoPreviousWord();
	TQTextCursor cr = *cursor;
	cr.gotoNextWord();

	int diff = TQABS( oldCursor.paragraph()->at( oldCursor.index() )->x - mousePos.x() );
	int ldiff = TQABS( cl.paragraph()->at( cl.index() )->x - mousePos.x() );
	int rdiff = TQABS( cr.paragraph()->at( cr.index() )->x - mousePos.x() );


	if ( cursor->paragraph()->lineStartOfChar( cursor->index() ) !=
	     oldCursor.paragraph()->lineStartOfChar( oldCursor.index() ) )
	    diff = 0xFFFFFF;

	if ( rdiff < diff && rdiff < ldiff )
	    *cursor = cr;
	else if ( ldiff < diff && ldiff < rdiff )
	    *cursor = cl;
	else
	    *cursor = oldCursor;

    }
    ensureCursorVisible();

    bool redraw = false;
    if ( doc->hasSelection( TQTextDocument::Standard ) ) {
	redraw = doc->setSelectionEnd( TQTextDocument::Standard, *cursor ) || redraw;
    }

    if ( !redraw ) {
	drawCursor( true );
    } else {
	repaintChanged();
	drawCursor( true );
    }

    if ( currentFormat && currentFormat->key() != cursor->paragraph()->at( cursor->index() )->format()->key() ) {
	currentFormat->removeRef();
	currentFormat = doc->formatCollection()->format( cursor->paragraph()->at( cursor->index() )->format() );
	if ( currentFormat->isMisspelled() ) {
	    currentFormat->removeRef();
	    currentFormat = doc->formatCollection()->format( currentFormat->font(), currentFormat->color() );
	}
	emit currentFontChanged( currentFormat->font() );
	emit currentColorChanged( currentFormat->color() );
	emit currentVerticalAlignmentChanged( (VerticalAlignment)currentFormat->vAlign() );
    }

    if ( currentAlignment != cursor->paragraph()->alignment() ) {
	currentAlignment = cursor->paragraph()->alignment();
	block_set_alignment = true;
	emit currentAlignmentChanged( currentAlignment );
	block_set_alignment = false;
    }
}

/*! \internal */

void TQTextEdit::placeCursor( const TQPoint &pos, TQTextCursor *c, bool link )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode )
	return;
#endif
    if ( !c )
	c = cursor;

    resetInputContext();
    c->restoreState();
    TQTextParagraph *s = doc->firstParagraph();
    c->place( pos, s, link );
    updateMicroFocusHint();
}


void TQTextEdit::updateMicroFocusHint()
{
    TQTextCursor c( *cursor );
#if 0
     // Disabled for Asian input method that shows candidate
     // window. This behavior is same as TQt/E 2.3.7 which supports
     // Asian input methods. Asian input methods need start point of IM
     // selection text to place candidate window as adjacent to the
     // selection text.    
    if ( d->preeditStart != -1 ) {
	c.setIndex( d->preeditStart );
        if(doc->hasSelection(TQTextDocument::IMSelectionText)) {
            int para, index;
            doc->selectionStart(TQTextDocument::IMSelectionText, para, index);
            c.setIndex(index);
        }
    }
#endif
    
    if ( hasFocus() || viewport()->hasFocus() ) {
	int h = c.paragraph()->lineHeightOfChar( cursor->index() );
	if ( !readOnly ) {
	    TQFont f = c.paragraph()->at( c.index() )->format()->font();
	    setMicroFocusHint( c.x() - contentsX() + frameWidth(),
			       c.y() + cursor->paragraph()->rect().y() - contentsY() + frameWidth(), 0, h, true, &f );
	}
    }
}



void TQTextEdit::formatMore()
{
    if ( !lastFormatted )
	return;

    int bottom = contentsHeight();
    int lastTop = -1;
    int lastBottom = -1;
    int to = 20;
    bool firstVisible = false;
    TQRect cr( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
    for ( int i = 0; lastFormatted &&
	  ( i < to || ( firstVisible && lastTop < contentsY()+height() ) );
	  i++ ) {
	lastFormatted->format();
	lastTop = lastFormatted->rect().top();
	lastBottom = lastFormatted->rect().bottom();
	if ( i == 0 )
	    firstVisible = lastBottom < cr.bottom();
	bottom = TQMAX( bottom, lastBottom );
	lastFormatted = lastFormatted->next();
    }

    if ( bottom > contentsHeight() ) {
	resizeContents( contentsWidth(), TQMAX( doc->height(), bottom ) );
    } else if ( !lastFormatted && lastBottom < contentsHeight() ) {
	resizeContents( contentsWidth(), TQMAX( doc->height(), lastBottom ) );
	if ( contentsHeight() < visibleHeight() )
	    updateContents( 0, contentsHeight(), visibleWidth(),
			    visibleHeight() - contentsHeight() );
    }

    if ( lastFormatted )
	formatTimer->start( interval, true );
    else
	interval = TQMAX( 0, interval );
}

void TQTextEdit::doResize()
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( !d->optimMode )
#endif
    {
	if ( wrapMode == FixedPixelWidth )
	    return;
	doc->setMinimumWidth( -1 );
	resizeContents( 0, 0 );
	doc->setWidth( visibleWidth() );
	doc->invalidate();
	lastFormatted = doc->firstParagraph();
	interval = 0;
	formatMore();
    }
    repaintContents( false );
}

/*! \internal */

void TQTextEdit::doChangeInterval()
{
    interval = 0;
}

/*!
    \reimp
*/

bool TQTextEdit::eventFilter( TQObject *o, TQEvent *e )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( !d->optimMode && (o == this || o == viewport()) ) {
#else
    if ( o == this || o == viewport() ) {
#endif
	if ( e->type() == TQEvent::FocusIn ) {
	    if ( TQApplication::cursorFlashTime() > 0 )
		blinkTimer->start( TQApplication::cursorFlashTime() / 2 );
	    drawCursor( true );
	    updateMicroFocusHint();
	} else if ( e->type() == TQEvent::FocusOut ) {
	    blinkTimer->stop();
	    drawCursor( false );
	}
    }

    if ( o == this && e->type() == TQEvent::PaletteChange ) {
	TQColor old( viewport()->colorGroup().color( TQColorGroup::Text ) );
	if ( old != colorGroup().color( TQColorGroup::Text ) ) {
	    TQColor c( colorGroup().color( TQColorGroup::Text ) );
	    doc->setMinimumWidth( -1 );
	    doc->setDefaultFormat( doc->formatCollection()->defaultFormat()->font(), c );
	    lastFormatted = doc->firstParagraph();
	    formatMore();
	    repaintChanged();
	}
    }

    return TQScrollView::eventFilter( o, e );
}

/*!
  \obsolete
 */
void TQTextEdit::insert( const TQString &text, bool indent,
			bool checkNewLine, bool removeSelected )
{
    uint f = 0;
    if ( indent )
	f |= RedoIndentation;
    if ( checkNewLine )
	f |= CheckNewLines;
    if ( removeSelected )
	f |= RemoveSelected;
    insert( text, f );
}

/*!
    Inserts \a text at the current cursor position.

    The \a insertionFlags define how the text is inserted. If \c
    RedoIndentation is set, the paragraph is re-indented. If \c
    CheckNewLines is set, newline characters in \a text result in hard
    line breaks (i.e. new paragraphs). If \c checkNewLine is not set,
    the behaviour of the editor is undefined if the \a text contains
    newlines. (It is not possible to change TQTextEdit's newline handling
    behavior, but you can use TQString::replace() to preprocess text
    before inserting it.) If \c RemoveSelected is set, any selected
    text (in selection 0) is removed before the text is inserted.

    The default flags are \c CheckNewLines | \c RemoveSelected.

    If the widget is in \c LogText mode this function will do nothing.

    \sa paste() pasteSubType()
*/


void TQTextEdit::insert( const TQString &text, uint insertionFlags )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode )
	return;
#endif

    if ( cursor->nestedDepth() != 0 ) // #### for 3.0, disable editing of tables as this is not advanced enough
	return;

    bool indent = insertionFlags & RedoIndentation;
    bool checkNewLine = insertionFlags & CheckNewLines;
    bool removeSelected = insertionFlags & RemoveSelected;
    bool imComposition = insertionFlags & AsIMCompositionText;
    bool imSelection = insertionFlags & WithIMSelection;
    TQString txt( text );
    drawCursor( false );
    if ( !isReadOnly() && doc->hasSelection( TQTextDocument::Standard ) && removeSelected )
	removeSelectedText();
    TQTextCursor c2 = *cursor;
    int oldLen = 0;

    if ( undoEnabled && !isReadOnly() && undoRedoInfo.type != UndoRedoInfo::IME ) {
	checkUndoRedoInfo( UndoRedoInfo::Insert );

        if (undoRedoInfo.valid() && undoRedoInfo.index + undoRedoInfo.d->text.length() != cursor->index()) {
            clearUndoRedo();
            undoRedoInfo.type = UndoRedoInfo::Insert;
        }

	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = cursor->paragraph()->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.d->text = TQString::null;
	}
	oldLen = undoRedoInfo.d->text.length();
    }

    lastFormatted = checkNewLine && cursor->paragraph()->prev() ?
		    cursor->paragraph()->prev() : cursor->paragraph();
    TQTextCursor oldCursor = *cursor;
    cursor->insert( txt, checkNewLine );
    if ( doc->useFormatCollection() && !doc->preProcessor() ) {
	doc->setSelectionStart( TQTextDocument::Temp, oldCursor );
	doc->setSelectionEnd( TQTextDocument::Temp, *cursor );
	doc->setFormat( TQTextDocument::Temp, currentFormat, TQTextFormat::Format );
	doc->removeSelection( TQTextDocument::Temp );
    }

    if ( indent && ( txt == "{" || txt == "}" || txt == ":" || txt == "#" ) )
	cursor->indent();
    formatMore();
    repaintChanged();
    ensureCursorVisible();
    // Asian users regard selection text as cursor on candidate
    // selection phase of input method, so ordinary cursor should be
    // invisible if IM selection text exists.
    drawCursor( !imSelection );

    if ( undoEnabled && !isReadOnly() && undoRedoInfo.type != UndoRedoInfo::IME ) {
	undoRedoInfo.d->text += txt;
	if ( !doc->preProcessor() ) {
	    for ( int i = 0; i < (int)txt.length(); ++i ) {
		if ( txt[ i ] != '\n' && c2.paragraph()->at( c2.index() )->format() ) {
		    c2.paragraph()->at( c2.index() )->format()->addRef();
		    undoRedoInfo.d->text.
			setFormat( oldLen + i,
				   c2.paragraph()->at( c2.index() )->format(), true );
		}
		c2.gotoNextLetter();
	    }
	}
    }

    if ( !removeSelected ) {
	doc->setSelectionStart( TQTextDocument::Standard, oldCursor );
	doc->setSelectionEnd( TQTextDocument::Standard, *cursor );
	repaintChanged();
    }
    // updateMicroFocusHint() should not be invoked here when this
    // function is invoked from imComposeEvent() because cursor
    // postion is incorrect yet. imComposeEvent() invokes
    // updateMicroFocusHint() later.
    if ( !imComposition ) {
      updateMicroFocusHint();
    }
    setModified();
    emit textChanged();
}

/*!
    Inserts \a text in the paragraph \a para at position \a index.
*/

void TQTextEdit::insertAt( const TQString &text, int para, int index )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	optimInsert( text, para, index );
	return;
    }
#endif
    resetInputContext();
    TQTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return;
    removeSelection( TQTextDocument::Standard );
    TQTextCursor tmp = *cursor;
    cursor->setParagraph( p );
    cursor->setIndex( index );
    insert( text, false, true, false );
    *cursor = tmp;
    removeSelection( TQTextDocument::Standard );
}

/*!
    Inserts \a text as a new paragraph at position \a para. If \a para
    is -1, the text is appended. Use append() if the append operation
    is performance critical.
*/

void TQTextEdit::insertParagraph( const TQString &text, int para )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	optimInsert( text + "\n", para, 0 );
	return;
    }
#endif
    resetInputContext();
    for ( int i = 0; i < (int)doc->numSelections(); ++i )
	doc->removeSelection( i );

    TQTextParagraph *p = doc->paragAt( para );

    bool append = !p;
    if ( !p )
	p = doc->lastParagraph();

    TQTextCursor old = *cursor;
    drawCursor( false );

    cursor->setParagraph( p );
    cursor->setIndex( 0 );
    clearUndoRedo();
    qtextedit_ignore_readonly = true;
    if ( append && cursor->paragraph()->length() > 1 ) {
	cursor->setIndex( cursor->paragraph()->length() - 1 );
	doKeyboardAction( ActionReturn );
    }
    insert( text, false, true, true );
    doKeyboardAction( ActionReturn );
    qtextedit_ignore_readonly = false;

    drawCursor( false );
    *cursor = old;
    drawCursor( true );

    repaintChanged();
}

/*!
    Removes the paragraph \a para.
*/

void TQTextEdit::removeParagraph( int para )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode )
	return;
#endif
    resetInputContext();
    TQTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return;

    for ( int i = 0; i < doc->numSelections(); ++i )
	doc->removeSelection( i );

    TQTextCursor start( doc );
    TQTextCursor end( doc );
    start.setParagraph( p );
    start.setIndex( 0 );
    end.setParagraph( p );
    end.setIndex( p->length() - 1 );

    if ( !(p == doc->firstParagraph() && p == doc->lastParagraph()) ) {
	if ( p->next() ) {
	    end.setParagraph( p->next() );
	    end.setIndex( 0 );
	} else if ( p->prev() ) {
	    start.setParagraph( p->prev() );
	    start.setIndex( p->prev()->length() - 1 );
	}
    }

    doc->setSelectionStart( TQTextDocument::Temp, start );
    doc->setSelectionEnd( TQTextDocument::Temp, end );
    removeSelectedText( TQTextDocument::Temp );
}

/*!
    Undoes the last operation.

    If there is no operation to undo, i.e. there is no undo step in
    the undo/redo history, nothing happens.

    \sa undoAvailable() redo() undoDepth()
*/

void TQTextEdit::undo()
{
    clearUndoRedo();
    if ( isReadOnly() || !doc->commands()->isUndoAvailable() || !undoEnabled )
	return;

    resetInputContext();
    for ( int i = 0; i < (int)doc->numSelections(); ++i )
	doc->removeSelection( i );

#ifndef TQT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif

    clearUndoRedo();
    drawCursor( false );
    TQTextCursor *c = doc->undo( cursor );
    if ( !c ) {
	drawCursor( true );
	return;
    }
    lastFormatted = 0;
    repaintChanged();
    ensureCursorVisible();
    drawCursor( true );
    updateMicroFocusHint();
    setModified();
    // ### If we get back to a completely blank textedit, it
    // is possible that cursor is invalid and further actions
    // might not fix the problem, so reset the cursor here.
    // This is copied from removeSeletedText(), it might be
    // okay to just call that.
    if ( !cursor->isValid() ) {
	delete cursor;
	cursor = new TQTextCursor( doc );
	drawCursor( true );
	repaintContents( true );
    }
    emit undoAvailable( isUndoAvailable() );
    emit redoAvailable( isRedoAvailable() );
    emit textChanged();
}

/*!
    Redoes the last operation.

    If there is no operation to redo, i.e. there is no redo step in
    the undo/redo history, nothing happens.

    \sa redoAvailable() undo() undoDepth()
*/

void TQTextEdit::redo()
{
    if ( isReadOnly() || !doc->commands()->isRedoAvailable() || !undoEnabled )
	return;

    resetInputContext();
    for ( int i = 0; i < (int)doc->numSelections(); ++i )
	doc->removeSelection( i );

#ifndef TQT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif

    clearUndoRedo();
    drawCursor( false );
    TQTextCursor *c = doc->redo( cursor );
    if ( !c ) {
	drawCursor( true );
	return;
    }
    lastFormatted = 0;
    ensureCursorVisible();
    repaintChanged();
    ensureCursorVisible();
    drawCursor( true );
    updateMicroFocusHint();
    setModified();
    emit undoAvailable( isUndoAvailable() );
    emit redoAvailable( isRedoAvailable() );
    emit textChanged();
}

/*!
    Pastes the text from the clipboard into the text edit at the
    current cursor position. Only plain text is pasted.

    If there is no text in the clipboard nothing happens.

    \sa pasteSubType() cut() TQTextEdit::copy()
*/

void TQTextEdit::paste()
{
#ifndef TQT_NO_MIMECLIPBOARD
    if ( isReadOnly() )
	return;
    TQString subType = "plain";
    if ( textFormat() != PlainText ) {
	TQMimeSource *m = TQApplication::clipboard()->data( d->clipboard_mode );
	if ( !m )
	    return;
	if ( m->provides( "application/x-qrichtext" ) )
	    subType = "x-qrichtext";
    }

    pasteSubType( subType.latin1() );
    updateMicroFocusHint();
#endif
}

void TQTextEdit::checkUndoRedoInfo( UndoRedoInfo::Type t )
{
    if ( undoRedoInfo.valid() && t != undoRedoInfo.type ) {
	clearUndoRedo();
    }
    undoRedoInfo.type = t;
}

/*!
    Repaints any paragraphs that have changed.

    Although used extensively internally you shouldn't need to call
    this yourself.
*/

void TQTextEdit::repaintChanged()
{
    if ( !isUpdatesEnabled() || !viewport()->isUpdatesEnabled() )
	return;

    TQPainter p( viewport() );
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	optimDrawContents( &p, contentsX(), contentsY(), visibleWidth(), visibleHeight() );
	return;
    }
#endif
    p.translate( -contentsX(), -contentsY() );
    paintDocument( false, &p, contentsX(), contentsY(), visibleWidth(), visibleHeight() );
}

#ifndef TQT_NO_MIME
TQTextDrag *TQTextEdit::dragObject( TQWidget *parent ) const
{
    if ( !doc->hasSelection( TQTextDocument::Standard ) ||
	 doc->selectedText( TQTextDocument::Standard ).isEmpty() )
	return 0;
    if ( textFormat() != RichText )
	return new TQTextDrag( doc->selectedText( TQTextDocument::Standard ), parent );
    TQRichTextDrag *drag = new TQRichTextDrag( parent );
    drag->setPlainText( doc->selectedText( TQTextDocument::Standard ) );
    drag->setRichText( doc->selectedText( TQTextDocument::Standard, true ) );
    return drag;
}
#endif

/*!
    Copies the selected text (from selection 0) to the clipboard and
    deletes it from the text edit.

    If there is no selected text (in selection 0) nothing happens.

    \sa TQTextEdit::copy() paste() pasteSubType()
*/

void TQTextEdit::cut()
{
    if ( isReadOnly() )
	return;
    resetInputContext();
    normalCopy();
    removeSelectedText();
    updateMicroFocusHint();
}

void TQTextEdit::normalCopy()
{
#ifndef TQT_NO_MIME
    TQTextDrag *drag = dragObject();
    if ( !drag )
	return;
#ifndef TQT_NO_MIMECLIPBOARD
    TQApplication::clipboard()->setData( drag, d->clipboard_mode );
#endif // TQT_NO_MIMECLIPBOARD
#endif // TQT_NO_MIME
}

/*!
    Copies any selected text (from selection 0) to the clipboard.

    \sa hasSelectedText() copyAvailable()
*/

void TQTextEdit::copy()
{
#ifndef TQT_NO_CLIPBOARD
# ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode && optimHasSelection() )
	TQApplication::clipboard()->setText( optimSelectedText(), d->clipboard_mode );
    else
	normalCopy();
# else
    normalCopy();
# endif
#endif
}

/*!
    \internal

    Re-indents the current paragraph.
*/

void TQTextEdit::indent()
{
    if ( isReadOnly() )
	return;

    drawCursor( false );
    if ( !doc->hasSelection( TQTextDocument::Standard ) )
	cursor->indent();
    else
	doc->indentSelection( TQTextDocument::Standard );
    repaintChanged();
    drawCursor( true );
    setModified();
    emit textChanged();
}

/*!
    Reimplemented to allow tabbing through links. If \a n is true the
    tab moves the focus to the next child; if \a n is false the tab
    moves the focus to the previous child. Returns true if the focus
    was moved; otherwise returns false.
 */

bool TQTextEdit::focusNextPrevChild( bool n )
{
    if ( !isReadOnly() || !linksEnabled() )
	return false;
    bool b = doc->focusNextPrevChild( n );
    repaintChanged();
    if ( b ) {
        TQTextParagraph *p = doc->focusIndicator.parag;
        int start = doc->focusIndicator.start;
        int len = doc->focusIndicator.len;

        int y = p->rect().y();
        while ( p
                && len == 0
                && p->at( start )->isCustom()
                && p->at( start )->customItem()->isNested() ) {

            TQTextTable *t = (TQTextTable*)p->at( start )->customItem();
            TQPtrList<TQTextTableCell> cells = t->tableCells();
            TQTextTableCell *c;
            for ( c = cells.first(); c; c = cells.next() ) {
                TQTextDocument *cellDoc = c->richText();
                if ( cellDoc->hasFocusParagraph() ) {
                    y += c->geometry().y() + c->verticalAlignmentOffset();

                    p = cellDoc->focusIndicator.parag;
                    start = cellDoc->focusIndicator.start;
                    len = cellDoc->focusIndicator.len;
                    if ( p )
                        y += p->rect().y();

                    break;
                }
            }
        }
        setContentsPos( contentsX(), TQMIN( y, contentsHeight() - visibleHeight() ) );
    }
    return b;
}

/*!
    \internal

  This functions sets the current format to \a f. Only the fields of \a
  f which are specified by the \a flags are used.
*/

void TQTextEdit::setFormat( TQTextFormat *f, int flags )
{
    if ( doc->hasSelection( TQTextDocument::Standard ) ) {
	drawCursor( false );
	TQTextCursor c1 = doc->selectionStartCursor( TQTextDocument::Standard );
	c1.restoreState();
	TQTextCursor c2 = doc->selectionEndCursor( TQTextDocument::Standard );
	c2.restoreState();
	if ( undoEnabled ) {
	    clearUndoRedo();
	    undoRedoInfo.type = UndoRedoInfo::Format;
	    undoRedoInfo.id = c1.paragraph()->paragId();
	    undoRedoInfo.index = c1.index();
	    undoRedoInfo.eid = c2.paragraph()->paragId();
	    undoRedoInfo.eindex = c2.index();
	    readFormats( c1, c2, undoRedoInfo.d->text );
	    undoRedoInfo.format = f;
	    undoRedoInfo.flags = flags;
	    clearUndoRedo();
	}
	doc->setFormat( TQTextDocument::Standard, f, flags );
	repaintChanged();
	formatMore();
	drawCursor( true );
	setModified();
	emit textChanged();
    }
    if ( currentFormat && currentFormat->key() != f->key() ) {
	currentFormat->removeRef();
	currentFormat = doc->formatCollection()->format( f );
	if ( currentFormat->isMisspelled() ) {
	    currentFormat->removeRef();
	    currentFormat = doc->formatCollection()->format( currentFormat->font(),
							     currentFormat->color() );
	}
	emit currentFontChanged( currentFormat->font() );
	emit currentColorChanged( currentFormat->color() );
	emit currentVerticalAlignmentChanged( (VerticalAlignment)currentFormat->vAlign() );
	if ( cursor->index() == cursor->paragraph()->length() - 1 ) {
	    currentFormat->addRef();
	    cursor->paragraph()->string()->setFormat( cursor->index(), currentFormat, true );
	    if ( cursor->paragraph()->length() == 1 ) {
		cursor->paragraph()->invalidate( 0 );
		cursor->paragraph()->format();
		repaintChanged();
	    }
	}
    }
}

/*!
    \reimp
*/

void TQTextEdit::setPalette( const TQPalette &p )
{
    TQScrollView::setPalette( p );
    if ( textFormat() == PlainText ) {
	TQTextFormat *f = doc->formatCollection()->defaultFormat();
	f->setColor( colorGroup().text() );
	updateContents();
    }
}

/*! \internal
  \warning In TQt 3.1 we will provide a cleaer API for the
  functionality which is provided by this function and in TQt 4.0 this
  function will go away.

  Sets the paragraph style of the current paragraph
  to \a dm. If \a dm is TQStyleSheetItem::DisplayListItem, the
  type of the list item is set to \a listStyle.

  \sa setAlignment()
*/

void TQTextEdit::setParagType( TQStyleSheetItem::DisplayMode dm,
			      TQStyleSheetItem::ListStyle listStyle )
{
    if ( isReadOnly() )
	return;

    drawCursor( false );
    TQTextParagraph *start = cursor->paragraph();
    TQTextParagraph *end = start;
    if ( doc->hasSelection( TQTextDocument::Standard ) ) {
	start = doc->selectionStartCursor( TQTextDocument::Standard ).topParagraph();
	end = doc->selectionEndCursor( TQTextDocument::Standard ).topParagraph();
	if ( end->paragId() < start->paragId() )
	    return; // do not trust our selections
    }

    clearUndoRedo();
    undoRedoInfo.type = UndoRedoInfo::Style;
    undoRedoInfo.id = start->paragId();
    undoRedoInfo.eid = end->paragId();
    undoRedoInfo.styleInformation = TQTextStyleCommand::readStyleInformation( doc, undoRedoInfo.id, undoRedoInfo.eid );

    while ( start != end->next() ) {
	start->setListStyle( listStyle );
	if ( dm == TQStyleSheetItem::DisplayListItem ) {
	    start->setListItem( true );
	    if( start->listDepth() == 0 )
		start->setListDepth( 1 );
	} else if ( start->isListItem() ) {
	    start->setListItem( false );
	    start->setListDepth( TQMAX( start->listDepth()-1, 0 ) );
	}
	start = start->next();
    }

    clearUndoRedo();
    repaintChanged();
    formatMore();
    drawCursor( true );
    setModified();
    emit textChanged();
}

/*!
    Sets the alignment of the current paragraph to \a a. Valid
    alignments are \c TQt::AlignLeft, \c TQt::AlignRight,
    \c TQt::AlignJustify and \c TQt::AlignCenter (which centers
    horizontally).
*/

void TQTextEdit::setAlignment( int a )
{
    if ( isReadOnly() || block_set_alignment )
	return;

    drawCursor( false );
    TQTextParagraph *start = cursor->paragraph();
    TQTextParagraph *end = start;
    if ( doc->hasSelection( TQTextDocument::Standard ) ) {
	start = doc->selectionStartCursor( TQTextDocument::Standard ).topParagraph();
	end = doc->selectionEndCursor( TQTextDocument::Standard ).topParagraph();
	if ( end->paragId() < start->paragId() )
	    return; // do not trust our selections
    }

    clearUndoRedo();
    undoRedoInfo.type = UndoRedoInfo::Style;
    undoRedoInfo.id = start->paragId();
    undoRedoInfo.eid = end->paragId();
    undoRedoInfo.styleInformation = TQTextStyleCommand::readStyleInformation( doc, undoRedoInfo.id, undoRedoInfo.eid );

    while ( start != end->next() ) {
	start->setAlignment( a );
	start = start->next();
    }

    clearUndoRedo();
    repaintChanged();
    formatMore();
    drawCursor( true );
    if ( currentAlignment != a ) {
	currentAlignment = a;
	emit currentAlignmentChanged( currentAlignment );
    }
    setModified();
    emit textChanged();
}

void TQTextEdit::updateCurrentFormat()
{
    int i = cursor->index();
    if ( i > 0 )
	--i;
    if ( doc->useFormatCollection() &&
	 ( !currentFormat || currentFormat->key() != cursor->paragraph()->at( i )->format()->key() ) ) {
	if ( currentFormat )
	    currentFormat->removeRef();
	currentFormat = doc->formatCollection()->format( cursor->paragraph()->at( i )->format() );
	if ( currentFormat->isMisspelled() ) {
	    currentFormat->removeRef();
	    currentFormat = doc->formatCollection()->format( currentFormat->font(), currentFormat->color() );
	}
	emit currentFontChanged( currentFormat->font() );
	emit currentColorChanged( currentFormat->color() );
	emit currentVerticalAlignmentChanged( (VerticalAlignment)currentFormat->vAlign() );
    }

    if ( currentAlignment != cursor->paragraph()->alignment() ) {
	currentAlignment = cursor->paragraph()->alignment();
	block_set_alignment = true;
	emit currentAlignmentChanged( currentAlignment );
	block_set_alignment = false;
    }
}

/*!
    If \a b is true sets the current format to italic; otherwise sets
    the current format to non-italic.

    \sa italic()
*/

void TQTextEdit::setItalic( bool b )
{
    TQTextFormat f( *currentFormat );
    f.setItalic( b );
    TQTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat(f2, TQTextFormat::Italic );
}

/*!
    If \a b is true sets the current format to bold; otherwise sets
    the current format to non-bold.

    \sa bold()
*/

void TQTextEdit::setBold( bool b )
{
    TQTextFormat f( *currentFormat );
    f.setBold( b );
    TQTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, TQTextFormat::Bold );
}

/*!
    If \a b is true sets the current format to underline; otherwise
    sets the current format to non-underline.

    \sa underline()
*/

void TQTextEdit::setUnderline( bool b )
{
    TQTextFormat f( *currentFormat );
    f.setUnderline( b );
    TQTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, TQTextFormat::Underline );
}

/*!
    Sets the font family of the current format to \a fontFamily.

    \sa family() setCurrentFont()
*/

void TQTextEdit::setFamily( const TQString &fontFamily )
{
    TQTextFormat f( *currentFormat );
    f.setFamily( fontFamily );
    TQTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, TQTextFormat::Family );
}

/*!
    Sets the point size of the current format to \a s.

    Note that if \a s is zero or negative, the behaviour of this
    function is not defined.

    \sa pointSize() setCurrentFont() setFamily()
*/

void TQTextEdit::setPointSize( int s )
{
    TQTextFormat f( *currentFormat );
    f.setPointSize( s );
    TQTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, TQTextFormat::Size );
}

/*!
    Sets the color of the current format, i.e. of the text, to \a c.

    \sa color() setPaper()
*/

void TQTextEdit::setColor( const TQColor &c )
{
    TQTextFormat f( *currentFormat );
    f.setColor( c );
    TQTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, TQTextFormat::Color );
}

/*!
    Sets the vertical alignment of the current format, i.e. of the
    text, to \a a.

    \sa color() setPaper()
*/

void TQTextEdit::setVerticalAlignment( VerticalAlignment a )
{
    TQTextFormat f( *currentFormat );
    f.setVAlign( (TQTextFormat::VerticalAlignment)a );
    TQTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, TQTextFormat::VAlign );
}

void TQTextEdit::setFontInternal( const TQFont &f_ )
{
    TQTextFormat f( *currentFormat );
    f.setFont( f_ );
    TQTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, TQTextFormat::Font );
}


TQString TQTextEdit::text() const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode )
	return optimText();
#endif

    TQTextParagraph *p = doc->firstParagraph();
    if ( !p || (!p->next() && p->length() <= 1) )
	return TQString::fromLatin1("");

    if ( isReadOnly() )
	return doc->originalText();
    return doc->text();
}

/*!
    \overload

    Returns the text of paragraph \a para.

    If textFormat() is \c RichText the text will contain HTML
    formatting tags.
*/

TQString TQTextEdit::text( int para ) const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode && (d->od->numLines >= para) ) {
	TQString paraStr = d->od->lines[ LOGOFFSET(para) ];
	if ( paraStr.isEmpty() )
	    paraStr = "\n";
	return paraStr;
    } else
#endif
    return doc->text( para );
}

/*!
    \overload

    Changes the text of the text edit to the string \a text and the
    context to \a context. Any previous text is removed.

    \a text may be interpreted either as plain text or as rich text,
    depending on the textFormat(). The default setting is \c AutoText,
    i.e. the text edit auto-detects the format from \a text.

    For rich text the rendering style and available tags are defined
    by a styleSheet(); see TQStyleSheet for details.

    The optional \a context is a path which the text edit's
    TQMimeSourceFactory uses to resolve the locations of files and
    images. (See \l{TQTextEdit::TQTextEdit()}.) It is passed to the text
    edit's TQMimeSourceFactory when quering data.

    Note that the undo/redo history is cleared by this function.

    \sa text(), setTextFormat()
*/

void TQTextEdit::setText( const TQString &text, const TQString &context )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	optimSetText( text );
	return;
    }
#endif
    resetInputContext();
    if ( !isModified() && isReadOnly() &&
	 this->context() == context && this->text() == text )
	return;

    emit undoAvailable( false );
    emit redoAvailable( false );
    undoRedoInfo.clear();
    doc->commands()->clear();

    lastFormatted = 0;
    int oldCursorPos = cursor->index();
    int oldCursorPar = cursor->paragraph()->paragId();
    cursor->restoreState();
    delete cursor;
    doc->setText( text, context );

    if ( wrapMode == FixedPixelWidth ) {
	resizeContents( wrapWidth, 0 );
	doc->setWidth( wrapWidth );
	doc->setMinimumWidth( wrapWidth );
    } else {
	doc->setMinimumWidth( -1 );
	resizeContents( 0, 0 );
    }

    lastFormatted = doc->firstParagraph();
    cursor = new TQTextCursor( doc );
    updateContents();

    if ( isModified() )
	setModified( false );
    emit textChanged();
    if ( cursor->index() != oldCursorPos || cursor->paragraph()->paragId() != oldCursorPar ) {
	emit cursorPositionChanged( cursor );
	emit cursorPositionChanged( cursor->paragraph()->paragId(), cursor->index() );
    }
    formatMore();
    updateCurrentFormat();
    d->scrollToAnchor = TQString::null;
}

/*!
    \property TQTextEdit::text
    \brief the text edit's text

    There is no default text.

    On setting, any previous text is deleted.

    The text may be interpreted either as plain text or as rich text,
    depending on the textFormat(). The default setting is \c AutoText,
    i.e. the text edit auto-detects the format of the text.

    For richtext, calling text() on an editable TQTextEdit will cause
    the text to be regenerated from the textedit. This may mean that
    the TQString returned may not be exactly the same as the one that
    was set.

    \sa textFormat
*/


/*!
    \property TQTextEdit::readOnly
    \brief whether the text edit is read-only

    In a read-only text edit the user can only navigate through the
    text and select text; modifying the text is not possible.

    This property's default is false.
*/

/*!
    Finds the next occurrence of the string, \a expr. Returns true if
    \a expr was found; otherwise returns false.

    If \a para and \a index are both 0 the search begins from the
    current cursor position. If \a para and \a index are both not 0,
    the search begins from the \a *index character position in the
    \a *para paragraph.

    If \a cs is true the search is case sensitive, otherwise it is
    case insensitive. If \a wo is true the search looks for whole word
    matches only; otherwise it searches for any matching text. If \a
    forward is true (the default) the search works forward from the
    starting position to the end of the text, otherwise it works
    backwards to the beginning of the text.

    If \a expr is found the function returns true. If \a index and \a
    para are not 0, the number of the paragraph in which the first
    character of the match was found is put into \a *para, and the
    index position of that character within the paragraph is put into
    \a *index.

    If \a expr is not found the function returns false. If \a index
    and \a para are not 0 and \a expr is not found, \a *index
    and \a *para are undefined.

    Please note that this function will make the next occurrence of
    the string (if found) the current selection, and will thus
    modify the cursor position.

    Using the \a para and \a index parameters will not work correctly
    in case the document contains tables.
*/

bool TQTextEdit::find( const TQString &expr, bool cs, bool wo, bool forward,
		      int *para, int *index )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode )
	return optimFind( expr, cs, wo, forward, para, index );
#endif
    drawCursor( false );
#ifndef TQT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
    TQTextCursor findcur = *cursor;
    if ( para && index ) {
	if ( doc->paragAt( *para ) )
	    findcur.gotoPosition( doc->paragAt(*para), *index );
	else
	    findcur.gotoEnd();
    } else if ( doc->hasSelection( TQTextDocument::Standard ) ){
	// maks sure we do not find the same selection again
	if ( forward )
	    findcur.gotoNextLetter();
	else
	    findcur.gotoPreviousLetter();
    } else if (!forward && findcur.index() == 0 && findcur.paragraph() == findcur.topParagraph()) {
	findcur.gotoEnd();
    }
    removeSelection( TQTextDocument::Standard );
    bool found = doc->find( findcur, expr, cs, wo, forward );
    if ( found ) {
	if ( para )
	    *para = findcur.paragraph()->paragId();
	if ( index )
	    *index = findcur.index();
	*cursor = findcur;
	repaintChanged();
	ensureCursorVisible();
    }
    drawCursor( true );
    if (found) {
	emit cursorPositionChanged( cursor );
	emit cursorPositionChanged( cursor->paragraph()->paragId(), cursor->index() );
    }
    return found;
}

void TQTextEdit::blinkCursor()
{
    if ( !cursorVisible )
	return;
    bool cv = cursorVisible;
    blinkCursorVisible = !blinkCursorVisible;
    drawCursor( blinkCursorVisible );
    cursorVisible = cv;
}

/*!
    Sets the cursor to position \a index in paragraph \a para.

    \sa getCursorPosition()
*/

void TQTextEdit::setCursorPosition( int para, int index )
{
    TQTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return;

    resetInputContext();
    if ( index > p->length() - 1 )
	index = p->length() - 1;

    drawCursor( false );
    cursor->setParagraph( p );
    cursor->setIndex( index );
    ensureCursorVisible();
    drawCursor( true );
    updateCurrentFormat();
    emit cursorPositionChanged( cursor );
    emit cursorPositionChanged( cursor->paragraph()->paragId(), cursor->index() );
}

/*!
    This function sets the \a *para and \a *index parameters to the
    current cursor position. \a para and \a index must not be 0.

    \sa setCursorPosition()
*/

void TQTextEdit::getCursorPosition( int *para, int *index ) const
{
    if ( !para || !index )
	return;
    *para = cursor->paragraph()->paragId();
    *index = cursor->index();
}

/*!
    Sets a selection which starts at position \a indexFrom in
    paragraph \a paraFrom and ends at position \a indexTo in paragraph
    \a paraTo.

    Any existing selections which have a different id (\a selNum) are
    left alone, but if an existing selection has the same id as \a
    selNum it is removed and replaced by this selection.

    Uses the selection settings of selection \a selNum. If \a selNum
    is 0, this is the default selection.

    The cursor is moved to the end of the selection if \a selNum is 0,
    otherwise the cursor position remains unchanged.

    \sa getSelection() selectedText
*/

void TQTextEdit::setSelection( int paraFrom, int indexFrom,
			      int paraTo, int indexTo, int selNum )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
	optimSetSelection(paraFrom, indexFrom, paraTo, indexTo);
	repaintContents(false);
	return;
    }
#endif
    resetInputContext();
    if ( doc->hasSelection( selNum ) ) {
	doc->removeSelection( selNum );
	repaintChanged();
    }
    if ( selNum > doc->numSelections() - 1 )
	doc->addSelection( selNum );
    TQTextParagraph *p1 = doc->paragAt( paraFrom );
    if ( !p1 )
	return;
    TQTextParagraph *p2 = doc->paragAt( paraTo );
    if ( !p2 )
	return;

    if ( indexFrom > p1->length() - 1 )
	indexFrom = p1->length() - 1;
    if ( indexTo > p2->length() - 1 )
	indexTo = p2->length() - 1;

    drawCursor( false );
    TQTextCursor c = *cursor;
    TQTextCursor oldCursor = *cursor;
    c.setParagraph( p1 );
    c.setIndex( indexFrom );
    cursor->setParagraph( p2 );
    cursor->setIndex( indexTo );
    doc->setSelectionStart( selNum, c );
    doc->setSelectionEnd( selNum, *cursor );
    repaintChanged();
    ensureCursorVisible();
    if ( selNum != TQTextDocument::Standard )
	*cursor = oldCursor;
    drawCursor( true );
}

/*!
    If there is a selection, \a *paraFrom is set to the number of the
    paragraph in which the selection begins and \a *paraTo is set to
    the number of the paragraph in which the selection ends. (They
    could be the same.) \a *indexFrom is set to the index at which the
    selection begins within \a *paraFrom, and \a *indexTo is set to
    the index at which the selection ends within \a *paraTo.

    If there is no selection, \a *paraFrom, \a *indexFrom, \a *paraTo
    and \a *indexTo are all set to -1.

    If \a paraFrom, \a indexFrom, \a paraTo or \a indexTo is 0 this
    function does nothing.

    The \a selNum is the number of the selection (multiple selections
    are supported). It defaults to 0 (the default selection).

    \sa setSelection() selectedText
*/

void TQTextEdit::getSelection( int *paraFrom, int *indexFrom,
			      int *paraTo, int *indexTo, int selNum ) const
{
    if ( !paraFrom || !paraTo || !indexFrom || !indexTo )
	return;
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if (d->optimMode) {
	*paraFrom = d->od->selStart.line;
	*paraTo = d->od->selEnd.line;
	*indexFrom = d->od->selStart.index;
	*indexTo = d->od->selEnd.index;
	return;
    }
#endif
    if ( !doc->hasSelection( selNum ) ) {
	*paraFrom = -1;
	*indexFrom = -1;
	*paraTo = -1;
	*indexTo = -1;
	return;
    }

    doc->selectionStart( selNum, *paraFrom, *indexFrom );
    doc->selectionEnd( selNum, *paraTo, *indexTo );
}

/*!
    \property TQTextEdit::textFormat
    \brief the text format: rich text, plain text, log text or auto text.

    The text format is one of the following:
    \list
    \i PlainText - all characters, except newlines, are displayed
    verbatim, including spaces. Whenever a newline appears in the text
    the text edit inserts a hard line break and begins a new
    paragraph.
    \i RichText - rich text rendering. The available styles are
    defined in the default stylesheet TQStyleSheet::defaultSheet().
    \i LogText -  optimized mode for very large texts. Supports a very
    limited set of formatting tags (color, bold, underline and italic
    settings).
    \i AutoText - this is the default. The text edit autodetects which
    rendering style is best, \c PlainText or \c RichText. This is done
    by using the TQStyleSheet::mightBeRichText() function.
    \endlist
*/

void TQTextEdit::setTextFormat( TextFormat format )
{
    doc->setTextFormat( format );
#ifdef QT_TEXTEDIT_OPTIMIZATION
    checkOptimMode();
#endif
}

TQt::TextFormat TQTextEdit::textFormat() const
{
    return doc->textFormat();
}

/*!
    Returns the number of paragraphs in the text; an empty textedit is always
    considered to have one paragraph, so 1 is returned in this case.
*/

int TQTextEdit::paragraphs() const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	return d->od->numLines;
    }
#endif
    return doc->lastParagraph()->paragId() + 1;
}

/*!
    Returns the number of lines in paragraph \a para, or -1 if there
    is no paragraph with index \a para.
*/

int TQTextEdit::linesOfParagraph( int para ) const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	if ( d->od->numLines >= para )
	    return 1;
	else
	    return -1;
    }
#endif
    TQTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return -1;
    return p->lines();
}

/*!
    Returns the length of the paragraph \a para (i.e. the number of
    characters), or -1 if there is no paragraph with index \a para.

    This function ignores newlines.
*/

int TQTextEdit::paragraphLength( int para ) const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	if ( d->od->numLines >= para ) {
	    if ( d->od->lines[ LOGOFFSET(para) ].isEmpty() ) // CR
		return 1;
	    else
		return d->od->lines[ LOGOFFSET(para) ].length();
	}
	return -1;
    }
#endif
    TQTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return -1;
    return p->length() - 1;
}

/*!
    Returns the number of lines in the text edit; this could be 0.

    \warning This function may be slow. Lines change all the time
    during word wrapping, so this function has to iterate over all the
    paragraphs and get the number of lines from each one individually.
*/

int TQTextEdit::lines() const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	return d->od->numLines;
    }
#endif
    TQTextParagraph *p = doc->firstParagraph();
    int l = 0;
    while ( p ) {
	l += p->lines();
	p = p->next();
    }

    return l;
}

/*!
    Returns the line number of the line in paragraph \a para in which
    the character at position \a index appears. The \a index position is
    relative to the beginning of the paragraph. If there is no such
    paragraph or no such character at the \a index position (e.g. the
    index is out of range) -1 is returned.
*/

int TQTextEdit::lineOfChar( int para, int index )
{
    TQTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return -1;

    int idx, line;
    TQTextStringChar *c = p->lineStartOfChar( index, &idx, &line );
    if ( !c )
	return -1;

    return line;
}

void TQTextEdit::setModified( bool m )
{
    bool oldModified = modified;
    modified = m;
    if ( modified && doc->oTextValid )
	doc->invalidateOriginalText();
    if ( oldModified != modified )
	emit modificationChanged( modified );
}

/*!
    \property TQTextEdit::modified
    \brief whether the document has been modified by the user
*/

bool TQTextEdit::isModified() const
{
    return modified;
}

void TQTextEdit::setModified()
{
    if ( !isModified() )
	setModified( true );
}

/*!
    Returns true if the current format is italic; otherwise returns false.

    \sa setItalic()
*/

bool TQTextEdit::italic() const
{
    return currentFormat->font().italic();
}

/*!
    Returns true if the current format is bold; otherwise returns false.

    \sa setBold()
*/

bool TQTextEdit::bold() const
{
    return currentFormat->font().bold();
}

/*!
    Returns true if the current format is underlined; otherwise returns
    false.

    \sa setUnderline()
*/

bool TQTextEdit::underline() const
{
    return currentFormat->font().underline();
}

/*!
    Returns the font family of the current format.

    \sa setFamily() setCurrentFont() setPointSize()
*/

TQString TQTextEdit::family() const
{
    return currentFormat->font().family();
}

/*!
    Returns the point size of the font of the current format.

    \sa setFamily() setCurrentFont() setPointSize()
*/

int TQTextEdit::pointSize() const
{
    return currentFormat->font().pointSize();
}

/*!
    Returns the color of the current format.

    \sa setColor() setPaper()
*/

TQColor TQTextEdit::color() const
{
    return currentFormat->color();
}

/*!
    \obsolete

    Returns TQScrollView::font()

    \warning In previous versions this function returned the font of
    the current format. This lead to confusion. Please use
    currentFont() instead.
*/

TQFont TQTextEdit::font() const
{
    return TQScrollView::font();
}

/*!
    Returns the font of the current format.

    \sa setCurrentFont() setFamily() setPointSize()
*/

TQFont TQTextEdit::currentFont() const
{
    return currentFormat->font();
}


/*!
    Returns the alignment of the current paragraph.

    \sa setAlignment()
*/

int TQTextEdit::alignment() const
{
    return currentAlignment;
}

void TQTextEdit::startDrag()
{
#ifndef TQT_NO_DRAGANDDROP
    mousePressed = false;
    inDoubleClick = false;
    TQDragObject *drag = dragObject( viewport() );
    if ( !drag )
	return;
    if ( isReadOnly() ) {
	drag->dragCopy();
    } else {
	if ( drag->drag() && TQDragObject::target() != this && TQDragObject::target() != viewport() )
	    removeSelectedText();
    }
#endif
}

/*!
    If \a select is true (the default), all the text is selected as
    selection 0. If \a select is false any selected text is
    unselected, i.e. the default selection (selection 0) is cleared.

    \sa selectedText
*/

void TQTextEdit::selectAll( bool select )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	if ( select )
	    optimSelectAll();
	else
	    optimRemoveSelection();
	return;
    }
#endif
    if ( !select )
	doc->removeSelection( TQTextDocument::Standard );
    else
	doc->selectAll( TQTextDocument::Standard );
    repaintChanged();
    emit copyAvailable( doc->hasSelection( TQTextDocument::Standard ) );
    emit selectionChanged();
#ifndef TQT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
}

void TQTextEdit::UndoRedoInfo::clear()
{
    if ( valid() ) {
	if ( type == Insert || type == Return )
	    doc->addCommand( new TQTextInsertCommand( doc, id, index, d->text.rawData(), styleInformation ) );
	else if ( type == Format )
	    doc->addCommand( new TQTextFormatCommand( doc, id, index, eid, eindex, d->text.rawData(), format, flags ) );
	else if ( type == Style )
	    doc->addCommand( new TQTextStyleCommand( doc, id, eid, styleInformation ) );
	else if ( type != Invalid ) {
	    doc->addCommand( new TQTextDeleteCommand( doc, id, index, d->text.rawData(), styleInformation ) );
	}
    }
    type = Invalid;
    d->text = TQString::null;
    id = -1;
    index = -1;
    styleInformation = TQByteArray();
}


/*!
    If there is some selected text (in selection 0) it is deleted. If
    there is no selected text (in selection 0) the character to the
    right of the text cursor is deleted.

    \sa removeSelectedText() cut()
*/

void TQTextEdit::del()
{
    if ( doc->hasSelection( TQTextDocument::Standard ) ) {
	removeSelectedText();
	return;
    }

    doKeyboardAction( ActionDelete );
}


TQTextEdit::UndoRedoInfo::UndoRedoInfo( TQTextDocument *dc )
    : type( Invalid ), doc( dc )
{
    d = new TQUndoRedoInfoPrivate;
    d->text = TQString::null;
    id = -1;
    index = -1;
}

TQTextEdit::UndoRedoInfo::~UndoRedoInfo()
{
    delete d;
}

bool TQTextEdit::UndoRedoInfo::valid() const
{
    return id >= 0 &&  type != Invalid;
}

/*!
    \internal

  Resets the current format to the default format.
*/

void TQTextEdit::resetFormat()
{
    setAlignment( TQt::AlignAuto );
    setParagType( TQStyleSheetItem::DisplayBlock, TQStyleSheetItem::ListDisc );
    setFormat( doc->formatCollection()->defaultFormat(), TQTextFormat::Format );
}

/*!
    Returns the TQStyleSheet which is being used by this text edit.

    \sa setStyleSheet()
*/

TQStyleSheet* TQTextEdit::styleSheet() const
{
    return doc->styleSheet();
}

/*!
    Sets the stylesheet to use with this text edit to \a styleSheet.
    Changes will only take effect for new text added with setText() or
    append().

    \sa styleSheet()
*/

void TQTextEdit::setStyleSheet( TQStyleSheet* styleSheet )
{
    doc->setStyleSheet( styleSheet );
}

/*!
    \property TQTextEdit::paper
    \brief the background (paper) brush.

    The brush that is currently used to draw the background of the
    text edit. The initial setting is an empty brush.
*/

void TQTextEdit::setPaper( const TQBrush& pap )
{
    doc->setPaper( new TQBrush( pap ) );

    if ( pap.pixmap() ) {
        viewport()->setBackgroundPixmap( *pap.pixmap() );
    } else {
        setPaletteBackgroundColor( pap.color() );
        viewport()->setPaletteBackgroundColor( pap.color() );
    }

#ifdef QT_TEXTEDIT_OPTIMIZATION
    // force a repaint of the entire viewport - using updateContents()
    // would clip the coords to the content size
    if (d->optimMode)
	repaintContents(contentsX(), contentsY(), viewport()->width(), viewport()->height());
    else
#endif
	updateContents();
}

TQBrush TQTextEdit::paper() const
{
    if ( doc->paper() )
	return *doc->paper();
    return TQBrush( colorGroup().base() );
}

/*!
    \property TQTextEdit::linkUnderline
    \brief whether hypertext links will be underlined

    If true (the default) hypertext links will be displayed
    underlined. If false links will not be displayed underlined.
*/

void TQTextEdit::setLinkUnderline( bool b )
{
    if ( doc->underlineLinks() == b )
	return;
    doc->setUnderlineLinks( b );
    repaintChanged();
}

bool TQTextEdit::linkUnderline() const
{
    return doc->underlineLinks();
}

/*!
    Sets the text edit's mimesource factory to \a factory. See
    TQMimeSourceFactory for further details.

    \sa mimeSourceFactory()
 */

#ifndef TQT_NO_MIME
void TQTextEdit::setMimeSourceFactory( TQMimeSourceFactory* factory )
{
    doc->setMimeSourceFactory( factory );
}

/*!
    Returns the TQMimeSourceFactory which is being used by this text
    edit.

    \sa setMimeSourceFactory()
*/

TQMimeSourceFactory* TQTextEdit::mimeSourceFactory() const
{
    return doc->mimeSourceFactory();
}
#endif

/*!
    Returns how many pixels high the text edit needs to be to display
    all the text if the text edit is \a w pixels wide.
*/

int TQTextEdit::heightForWidth( int w ) const
{
    int oldw = doc->width();
    doc->doLayout( 0, w );
    int h = doc->height();
    doc->setWidth( oldw );
    doc->invalidate();
    ( (TQTextEdit*)this )->formatMore();
    return h;
}

/*!
    Appends a new paragraph with \a text to the end of the text edit. Note that
    the undo/redo history is cleared by this function, and no undo
    history is kept for appends which makes them faster than
    insert()s. If you want to append text which is added to the
    undo/redo history as well, use insertParagraph().
*/

void TQTextEdit::append( const TQString &text )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	optimAppend( text );
	return;
    }
#endif
    // flush and clear the undo/redo stack if necessary
    undoRedoInfo.clear();
    doc->commands()->clear();

    doc->removeSelection( TQTextDocument::Standard );
    TextFormat f = doc->textFormat();
    if ( f == AutoText ) {
	if ( TQStyleSheet::mightBeRichText( text ) )
	    f = RichText;
	else
	    f = PlainText;
    }

    drawCursor( false );
    TQTextCursor oldc( *cursor );
    ensureFormatted( doc->lastParagraph() );
    bool atBottom = contentsY() >= contentsHeight() - visibleHeight();
    cursor->gotoEnd();
    if ( cursor->index() > 0 )
	cursor->splitAndInsertEmptyParagraph();
    TQTextCursor oldCursor2 = *cursor;

    if ( f == TQt::PlainText ) {
	cursor->insert( text, true );
	if ( doc->useFormatCollection() && !doc->preProcessor() &&
	     currentFormat != cursor->paragraph()->at( cursor->index() )->format() ) {
	    doc->setSelectionStart( TQTextDocument::Temp, oldCursor2 );
	    doc->setSelectionEnd( TQTextDocument::Temp, *cursor );
	    doc->setFormat( TQTextDocument::Temp, currentFormat, TQTextFormat::Format );
	    doc->removeSelection( TQTextDocument::Temp );
	}
    } else {
	cursor->paragraph()->setListItem( false );
	cursor->paragraph()->setListDepth( 0 );
	if ( cursor->paragraph()->prev() )
	    cursor->paragraph()->prev()->invalidate(0); // vertical margins might have to change
	doc->setRichTextInternal( text );
    }
    formatMore();
    repaintChanged();
    if ( atBottom )
        scrollToBottom();
    *cursor = oldc;
    if ( !isReadOnly() )
	cursorVisible = true;
    setModified();
    emit textChanged();
}

/*!
    \property TQTextEdit::hasSelectedText
    \brief whether some text is selected in selection 0
*/

bool TQTextEdit::hasSelectedText() const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode )
	return optimHasSelection();
    else
#endif
	return doc->hasSelection( TQTextDocument::Standard );
}

/*!
    \property TQTextEdit::selectedText
    \brief The selected text (from selection 0) or an empty string if
    there is no currently selected text (in selection 0).

    The text is always returned as \c PlainText if the textFormat() is
    \c PlainText or \c AutoText, otherwise it is returned as HTML.

    \sa hasSelectedText
*/

TQString TQTextEdit::selectedText() const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode )
	return optimSelectedText();
    else
#endif
	return doc->selectedText( TQTextDocument::Standard, textFormat() == RichText );
}

bool TQTextEdit::handleReadOnlyKeyEvent( TQKeyEvent *e )
{
    switch( e->key() ) {
    case Key_Down:
	setContentsPos( contentsX(), contentsY() + 10 );
	break;
    case Key_Up:
	setContentsPos( contentsX(), contentsY() - 10 );
	break;
    case Key_Left:
	setContentsPos( contentsX() - 10, contentsY() );
	break;
    case Key_Right:
	setContentsPos( contentsX() + 10, contentsY() );
	break;
    case Key_PageUp:
	setContentsPos( contentsX(), contentsY() - visibleHeight() );
	break;
    case Key_PageDown:
	setContentsPos( contentsX(), contentsY() + visibleHeight() );
	break;
    case Key_Home:
	setContentsPos( contentsX(), 0 );
	break;
    case Key_End:
	setContentsPos( contentsX(), contentsHeight() - visibleHeight() );
	break;
    case Key_F16: // Copy key on Sun keyboards
	copy();
	break;
#ifndef TQT_NO_NETWORKPROTOCOL
    case Key_Return:
    case Key_Enter:
    case Key_Space: {
	if (!doc->focusIndicator.href.isEmpty()
		|| !doc->focusIndicator.name.isEmpty()) {
	    if (!doc->focusIndicator.href.isEmpty()) {
		TQUrl u( doc->context(), doc->focusIndicator.href, true );
		emitLinkClicked( u.toString( false, false ) );
	    }
	    if (!doc->focusIndicator.name.isEmpty()) {
		if (::tqt_cast<TQTextBrowser*>(this)) { // change for 4.0
		    TQConnectionList *clist = receivers(
			    "anchorClicked(const TQString&,const TQString&)");
		    if (!signalsBlocked() && clist) {
			TQUObject o[3];
			static_QUType_TQString.set(o+1,
				doc->focusIndicator.name);
			static_QUType_TQString.set(o+2,
				doc->focusIndicator.href);
			activate_signal( clist, o);
		    }
		}
	    }
#ifndef TQT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
    } break;
#endif
    default:
	if ( e->state() & ControlButton ) {
	    switch ( e->key() ) {
	    case Key_C: case Key_F16: // Copy key on Sun keyboards
		copy();
		break;
#ifdef TQ_WS_WIN
	    case Key_Insert:
		copy();
		break;
	    case Key_A:
		selectAll();
		break;
#endif
	    }

	}
	return false;
    }
    return true;
}

/*!
    Returns the context of the text edit. The context is a path which
    the text edit's TQMimeSourceFactory uses to resolve the locations
    of files and images.

    \sa text
*/

TQString TQTextEdit::context() const
{
    return doc->context();
}

/*!
    \property TQTextEdit::documentTitle
    \brief the title of the document parsed from the text.

    For \c PlainText the title will be an empty string. For \c
    RichText the title will be the text between the \c{<title>} tags,
    if present, otherwise an empty string.
*/

TQString TQTextEdit::documentTitle() const
{
    return doc->attributes()[ "title" ];
}

void TQTextEdit::makeParagVisible( TQTextParagraph *p )
{
    setContentsPos( contentsX(), TQMIN( p->rect().y(), contentsHeight() - visibleHeight() ) );
}

/*!
    Scrolls the text edit to make the text at the anchor called \a
    name visible, if it can be found in the document. If the anchor
    isn't found no scrolling will occur. An anchor is defined using
    the HTML anchor tag, e.g. \c{<a name="target">}.
*/

void TQTextEdit::scrollToAnchor( const TQString& name )
{
    if ( !isVisible() ) {
	d->scrollToAnchor = name;
	return;
    }
    if ( name.isEmpty() )
	return;
    sync();
    TQTextCursor cursor( doc );
    TQTextParagraph* last = doc->lastParagraph();
    for (;;) {
	TQTextStringChar* c = cursor.paragraph()->at( cursor.index() );
	if( c->isAnchor() ) {
	    TQString a = c->anchorName();
	    if ( a == name ||
		 (a.contains( '#' ) && TQStringList::split( '#', a ).contains( name ) ) ) {
		setContentsPos( contentsX(), TQMIN( cursor.paragraph()->rect().top() + cursor.totalOffsetY(), contentsHeight() - visibleHeight() ) );
		break;
	    }
	}
	if ( cursor.paragraph() == last && cursor.atParagEnd()  )
	    break;
	cursor.gotoNextLetter();
    }
}

#if (TQT_VERSION-0 >= 0x040000)
#error "function anchorAt(const TQPoint& pos) should be merged into function anchorAt(const TQPoint& pos, AnchorAttribute attr)"
#endif

/*!
    \overload

    If there is an anchor at position \a pos (in contents
    coordinates), its \c href is returned, otherwise TQString::null is
    returned.
*/

TQString TQTextEdit::anchorAt( const TQPoint& pos )
{
    return anchorAt(pos, AnchorHref);
}

/*!
    If there is an anchor at position \a pos (in contents
    coordinates), the text for attribute \a attr is returned,
    otherwise TQString::null is returned.
*/

TQString TQTextEdit::anchorAt( const TQPoint& pos, AnchorAttribute attr )
{
    TQTextCursor c( doc );
    placeCursor( pos, &c );
    switch(attr) {
	case AnchorName:
	    return c.paragraph()->at( c.index() )->anchorName();
	case AnchorHref:
	    return c.paragraph()->at( c.index() )->anchorHref();
    }
    // incase the compiler is really dumb about determining if a function
    // returns something :)
    return TQString::null;
}

void TQTextEdit::documentWidthChanged( int w )
{
    resizeContents( TQMAX( visibleWidth(), w), contentsHeight() );
}

/*! \internal

  This function does nothing
*/

void TQTextEdit::updateStyles()
{
}

void TQTextEdit::setDocument( TQTextDocument *dc )
{
    if ( dc == 0 ) {
        tqWarning( "Q3TextEdit::setDocument() called with null Q3TextDocument pointer" );
        return;
    }
    if ( dc == doc )
	return;
    resetInputContext();
    doc = dc;
    delete cursor;
    cursor = new TQTextCursor( doc );
    clearUndoRedo();
    undoRedoInfo.doc = doc;
    lastFormatted = 0;
}

#ifndef TQT_NO_CLIPBOARD

/*!
    Pastes the text with format \a subtype from the clipboard into the
    text edit at the current cursor position. The \a subtype can be
    "plain" or "html".

    If there is no text with format \a subtype in the clipboard
    nothing happens.

    \sa paste() cut() TQTextEdit::copy()
*/

void TQTextEdit::pasteSubType( const TQCString &subtype )
{
#ifndef TQT_NO_MIMECLIPBOARD
    TQMimeSource *m = TQApplication::clipboard()->data( d->clipboard_mode );
    pasteSubType( subtype, m );
#endif
}

/*! \internal */

void TQTextEdit::pasteSubType( const TQCString& subtype, TQMimeSource *m )
{
#ifndef TQT_NO_MIME
    TQCString st = subtype;
    if ( subtype != "x-qrichtext" )
	st.prepend( "text/" );
    else
	st.prepend( "application/" );
    if ( !m )
	return;
    if ( doc->hasSelection( TQTextDocument::Standard ) )
	removeSelectedText();
    if ( !TQRichTextDrag::canDecode( m ) )
	return;
    TQString t;
    if ( !TQRichTextDrag::decode( m, t, st.data(), subtype ) )
	return;
    if ( st == "application/x-qrichtext" ) {
	int start;
	if ( (start = t.find( "<!--StartFragment-->" )) != -1 ) {
	    start += 20;
	    int end = t.find( "<!--EndFragment-->" );
	    TQTextCursor oldC = *cursor;

	    // during the setRichTextInternal() call the cursors
	    // paragraph might get joined with the provious one, so
	    // the cursors one would get deleted and oldC.paragraph()
	    // would be a dnagling pointer. To avoid that try to go
	    // one letter back and later go one forward again.
	    oldC.gotoPreviousLetter();
	    bool couldGoBack = oldC != *cursor;
	    // first para might get deleted, so remember to reset it
	    bool wasAtFirst = oldC.paragraph() == doc->firstParagraph();

	    if ( start < end )
		t = t.mid( start, end - start );
	    else
		t = t.mid( start );
	    lastFormatted = cursor->paragraph();
	    if ( lastFormatted->prev() )
		lastFormatted = lastFormatted->prev();
	    doc->setRichTextInternal( t, cursor );

	    // the first para might have been deleted in
	    // setRichTextInternal(). To be sure, reset it if
	    // necessary.
	    if ( wasAtFirst ) {
		int index = oldC.index();
		oldC.setParagraph( doc->firstParagraph() );
		oldC.setIndex( index );
	    }

	    // if we went back one letter before (see last comment),
	    // go one forward to point to the right position
	    if ( couldGoBack )
		oldC.gotoNextLetter();

	    if ( undoEnabled && !isReadOnly() ) {
		doc->setSelectionStart( TQTextDocument::Temp, oldC );
		doc->setSelectionEnd( TQTextDocument::Temp, *cursor );

		checkUndoRedoInfo( UndoRedoInfo::Insert );
		if ( !undoRedoInfo.valid() ) {
		    undoRedoInfo.id = oldC.paragraph()->paragId();
		    undoRedoInfo.index = oldC.index();
		    undoRedoInfo.d->text = TQString::null;
		}
		int oldLen = undoRedoInfo.d->text.length();
		if ( !doc->preProcessor() ) {
		    TQString txt = doc->selectedText( TQTextDocument::Temp );
		    undoRedoInfo.d->text += txt;
		    for ( int i = 0; i < (int)txt.length(); ++i ) {
			if ( txt[ i ] != '\n' && oldC.paragraph()->at( oldC.index() )->format() ) {
			    oldC.paragraph()->at( oldC.index() )->format()->addRef();
			    undoRedoInfo.d->text.
				setFormat( oldLen + i, oldC.paragraph()->at( oldC.index() )->format(), true );
			}
			oldC.gotoNextLetter();
		    }
		}
		undoRedoInfo.clear();
		removeSelection( TQTextDocument::Temp );
	    }

	    formatMore();
	    setModified();
	    emit textChanged();
	    repaintChanged();
	    ensureCursorVisible();
	    return;
	}
    } else {
#if defined(Q_OS_WIN32)
	// Need to convert CRLF to LF
	t.replace( "\r\n", "\n" );
#elif defined(Q_OS_MAC)
	//need to convert CR to LF
	t.replace( '\r', '\n' );
#endif
	TQChar *uc = (TQChar *)t.unicode();
	for ( int i=0; (uint) i<t.length(); i++ ) {
	    if ( uc[ i ] < ' ' && uc[ i ] != '\n' && uc[ i ] != '\t' )
		uc[ i ] = ' ';
	}
	if ( !t.isEmpty() )
	    insert( t, false, true );
    }
#endif //TQT_NO_MIME
}

#ifndef TQT_NO_MIMECLIPBOARD
/*!
    Prompts the user to choose a type from a list of text types
    available, then copies text from the clipboard (if there is any)
    into the text edit at the current text cursor position. Any
    selected text (in selection 0) is first deleted.
*/
void TQTextEdit::pasteSpecial( const TQPoint& pt )
{
    TQCString st = pickSpecial( TQApplication::clipboard()->data( d->clipboard_mode ),
			       true, pt );
    if ( !st.isEmpty() )
	pasteSubType( st );
}
#endif
#ifndef TQT_NO_MIME
TQCString TQTextEdit::pickSpecial( TQMimeSource* ms, bool always_ask, const TQPoint& pt )
{
    if ( ms )  {
#ifndef TQT_NO_POPUPMENU
	TQPopupMenu popup( this, "qt_pickspecial_menu" );
	TQString fmt;
	int n = 0;
	TQDict<void> done;
	for (int i = 0; !( fmt = ms->format( i ) ).isNull(); i++) {
	    int semi = fmt.find( ";" );
	    if ( semi >= 0 )
		fmt = fmt.left( semi );
	    if ( fmt.left( 5 ) == "text/" ) {
		fmt = fmt.mid( 5 );
		if ( !done.find( fmt ) ) {
		    done.insert( fmt,(void*)1 );
		    popup.insertItem( fmt, i );
		    n++;
		}
	    }
	}
	if ( n ) {
	    int i = n ==1 && !always_ask ? popup.idAt( 0 ) : popup.exec( pt );
	    if ( i >= 0 )
		return popup.text(i).latin1();
	}
#else
	TQString fmt;
	for (int i = 0; !( fmt = ms->format( i ) ).isNull(); i++) {
	    int semi = fmt.find( ";" );
	    if ( semi >= 0 )
		fmt = fmt.left( semi );
	    if ( fmt.left( 5 ) == "text/" ) {
		fmt = fmt.mid( 5 );
		return fmt.latin1();
	    }
	}
#endif
    }
    return TQCString();
}
#endif // TQT_NO_MIME
#endif // TQT_NO_CLIPBOARD

/*!
    \enum TQTextEdit::WordWrap

    This enum defines the TQTextEdit's word wrap modes.

    \value NoWrap Do not wrap the text.

    \value WidgetWidth Wrap the text at the current width of the
    widget (this is the default). Wrapping is at whitespace by
    default; this can be changed with setWrapPolicy().

    \value FixedPixelWidth Wrap the text at a fixed number of pixels
    from the widget's left side. The number of pixels is set with
    wrapColumnOrWidth().

    \value FixedColumnWidth Wrap the text at a fixed number of
    character columns from the widget's left side. The number of
    characters is set with wrapColumnOrWidth(). This is useful if you
    need formatted text that can also be displayed gracefully on
    devices with monospaced fonts, for example a standard VT100
    terminal, where you might set wrapColumnOrWidth() to 80.

    \sa setWordWrap() wordWrap()
*/

/*!
    \property TQTextEdit::wordWrap
    \brief the word wrap mode

    The default mode is \c WidgetWidth which causes words to be
    wrapped at the right edge of the text edit. Wrapping occurs at
    whitespace, keeping whole words intact. If you want wrapping to
    occur within words use setWrapPolicy(). If you set a wrap mode of
    \c FixedPixelWidth or \c FixedColumnWidth you should also call
    setWrapColumnOrWidth() with the width you want.

    \sa WordWrap, wrapColumnOrWidth, wrapPolicy,
*/

void TQTextEdit::setWordWrap( WordWrap mode )
{
    if ( wrapMode == mode )
	return;
    wrapMode = mode;
    switch ( mode ) {
    case NoWrap:
	document()->formatter()->setWrapEnabled( false );
	document()->formatter()->setWrapAtColumn( -1 );
	doc->setWidth( visibleWidth() );
	doc->setMinimumWidth( -1 );
	doc->invalidate();
	updateContents();
	lastFormatted = doc->firstParagraph();
	interval = 0;
	formatMore();
	break;
    case WidgetWidth:
	document()->formatter()->setWrapEnabled( true );
	document()->formatter()->setWrapAtColumn( -1 );
	doResize();
	break;
    case FixedPixelWidth:
	document()->formatter()->setWrapEnabled( true );
	document()->formatter()->setWrapAtColumn( -1 );
	if ( wrapWidth < 0 )
	    wrapWidth = 200;
	setWrapColumnOrWidth( wrapWidth );
	break;
    case FixedColumnWidth:
	if ( wrapWidth < 0 )
	    wrapWidth = 80;
	document()->formatter()->setWrapEnabled( true );
	document()->formatter()->setWrapAtColumn( wrapWidth );
	setWrapColumnOrWidth( wrapWidth );
	break;
    }
#ifdef QT_TEXTEDIT_OPTIMIZATION
    checkOptimMode();
#endif
}

TQTextEdit::WordWrap TQTextEdit::wordWrap() const
{
    return wrapMode;
}

/*!
    \property TQTextEdit::wrapColumnOrWidth
    \brief the position (in pixels or columns depending on the wrap mode) where text will be wrapped

    If the wrap mode is \c FixedPixelWidth, the value is the number of
    pixels from the left edge of the text edit at which text should be
    wrapped. If the wrap mode is \c FixedColumnWidth, the value is the
    column number (in character columns) from the left edge of the
    text edit at which text should be wrapped.

    \sa wordWrap
*/
void TQTextEdit::setWrapColumnOrWidth( int value )
{
    wrapWidth = value;
    if ( wrapMode == FixedColumnWidth ) {
	document()->formatter()->setWrapAtColumn( wrapWidth );
	resizeContents( 0, 0 );
	doc->setWidth( visibleWidth() );
	doc->setMinimumWidth( -1 );
    } else if (wrapMode == FixedPixelWidth ) {
	document()->formatter()->setWrapAtColumn( -1 );
	resizeContents( wrapWidth, 0 );
	doc->setWidth( wrapWidth );
	doc->setMinimumWidth( wrapWidth );
    } else {
	return;
    }
    doc->invalidate();
    updateContents();
    lastFormatted = doc->firstParagraph();
    interval = 0;
    formatMore();
}

int TQTextEdit::wrapColumnOrWidth() const
{
    if ( wrapMode == WidgetWidth )
	return visibleWidth();
    return wrapWidth;
}


/*!
    \enum TQTextEdit::WrapPolicy

    This enum defines where text can be wrapped in word wrap mode.

    \value AtWhiteSpace Don't use this deprecated value (it is a
    synonym for \c AtWordBoundary which you should use instead).
    \value Anywhere  Break anywhere, including within words.
    \value AtWordBoundary Break lines at word boundaries, e.g. spaces or
    newlines
    \value AtWordOrDocumentBoundary Break lines at whitespace, e.g.
    spaces or newlines if possible. Break it anywhere otherwise.

    \sa setWrapPolicy()
*/

/*!
    \property TQTextEdit::wrapPolicy
    \brief the word wrap policy, at whitespace or anywhere

    Defines where text can be wrapped when word wrap mode is not \c
    NoWrap. The choices are \c AtWordBoundary (the default), \c
    Anywhere and \c AtWordOrDocumentBoundary

    \sa wordWrap
*/

void TQTextEdit::setWrapPolicy( WrapPolicy policy )
{
    if ( wPolicy == policy )
	return;
    wPolicy = policy;
    TQTextFormatter *formatter;
    if ( policy == AtWordBoundary || policy == AtWordOrDocumentBoundary ) {
	formatter = new TQTextFormatterBreakWords;
	formatter->setAllowBreakInWords( policy == AtWordOrDocumentBoundary );
    } else {
	formatter = new TQTextFormatterBreakInWords;
    }
    formatter->setWrapAtColumn( document()->formatter()->wrapAtColumn() );
    formatter->setWrapEnabled( document()->formatter()->isWrapEnabled( 0 ) );
    document()->setFormatter( formatter );
    doc->invalidate();
    updateContents();
    lastFormatted = doc->firstParagraph();
    interval = 0;
    formatMore();
}

TQTextEdit::WrapPolicy TQTextEdit::wrapPolicy() const
{
    return wPolicy;
}

/*!
    Deletes all the text in the text edit.

    \sa cut() removeSelectedText() setText()
*/

void TQTextEdit::clear()
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	optimSetText("");
    } else
#endif
    {
	// make clear undoable
	doc->selectAll( TQTextDocument::Temp );
	removeSelectedText( TQTextDocument::Temp );
	setContentsPos( 0, 0 );
	if ( cursor->isValid() )
	    cursor->restoreState();
	doc->clear( true );
	delete cursor;
	cursor = new TQTextCursor( doc );
	lastFormatted = 0;
    }
    updateContents();

    emit cursorPositionChanged( cursor );
    emit cursorPositionChanged( cursor->paragraph()->paragId(), cursor->index() );
}

int TQTextEdit::undoDepth() const
{
    return document()->undoDepth();
}

/*!
    \property TQTextEdit::length
    \brief the number of characters in the text
*/

int TQTextEdit::length() const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode )
	return d->od->len;
    else
#endif
	return document()->length();
}

/*!
    \property TQTextEdit::tabStopWidth
    \brief the tab stop width in pixels
*/

int TQTextEdit::tabStopWidth() const
{
    return document()->tabStopWidth();
}

void TQTextEdit::setUndoDepth( int d )
{
    document()->setUndoDepth( d );
}

void TQTextEdit::setTabStopWidth( int ts )
{
    document()->setTabStops( ts );
    doc->invalidate();
    lastFormatted = doc->firstParagraph();
    interval = 0;
    formatMore();
    updateContents();
}

/*!
    \reimp
*/

TQSize TQTextEdit::sizeHint() const
{
    // cf. TQScrollView::sizeHint()
    constPolish();
    int f = 2 * frameWidth();
    int h = fontMetrics().height();
    TQSize sz( f, f );
    return sz.expandedTo( TQSize(12 * h, 8 * h) );
}

void TQTextEdit::clearUndoRedo()
{
    if ( !undoEnabled )
	return;
    undoRedoInfo.clear();
    emit undoAvailable( doc->commands()->isUndoAvailable() );
    emit redoAvailable( doc->commands()->isRedoAvailable() );
}

/*!  \internal
  \warning In TQt 3.1 we will provide a cleaer API for the
  functionality which is provided by this function and in TQt 4.0 this
  function will go away.

  This function gets the format of the character at position \a
  index in paragraph \a para. Sets \a font to the character's font, \a
  color to the character's color and \a verticalAlignment to the
  character's vertical alignment.

  Returns false if \a para or \a index is out of range otherwise
  returns true.
*/

bool TQTextEdit::getFormat( int para, int index, TQFont *font, TQColor *color, VerticalAlignment *verticalAlignment )
{
    if ( !font || !color )
	return false;
    TQTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return false;
    if ( index < 0 || index >= p->length() )
	return false;
    *font = p->at( index )->format()->font();
    *color = p->at( index )->format()->color();
    *verticalAlignment = (VerticalAlignment)p->at( index )->format()->vAlign();
    return true;
}

/*!  \internal
  \warning In TQt 3.1 we will provide a cleaer API for the
  functionality which is provided by this function and in TQt 4.0 this
  function will go away.

  This function gets the format of the paragraph \a para. Sets \a
  font to the paragraphs's font, \a color to the paragraph's color, \a
  verticalAlignment to the paragraph's vertical alignment, \a
  alignment to the paragraph's alignment, \a displayMode to the
  paragraph's display mode, \a listStyle to the paragraph's list style
  (if the display mode is TQStyleSheetItem::DisplayListItem) and \a
  listDepth to the depth of the list (if the display mode is
  TQStyleSheetItem::DisplayListItem).

  Returns false if \a para is out of range otherwise returns true.
*/

bool TQTextEdit::getParagraphFormat( int para, TQFont *font, TQColor *color,
				    VerticalAlignment *verticalAlignment, int *alignment,
				    TQStyleSheetItem::DisplayMode *displayMode,
				    TQStyleSheetItem::ListStyle *listStyle,
				    int *listDepth )
{
    if ( !font || !color || !alignment || !displayMode || !listStyle )
	return false;
    TQTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return false;
    *font = p->at(0)->format()->font();
    *color = p->at(0)->format()->color();
    *verticalAlignment = (VerticalAlignment)p->at(0)->format()->vAlign();
    *alignment = p->alignment();
    *displayMode = p->isListItem() ? TQStyleSheetItem::DisplayListItem : TQStyleSheetItem::DisplayBlock;
    *listStyle = p->listStyle();
    *listDepth = p->listDepth();
    return true;
}



/*!
    This function is called to create a right mouse button popup menu
    at the document position \a pos. If you want to create a custom
    popup menu, reimplement this function and return the created popup
    menu. Ownership of the popup menu is transferred to the caller.

    \warning The TQPopupMenu ID values 0-7 are reserved, and they map to the
    standard operations. When inserting items into your custom popup menu, be
    sure to specify ID values larger than 7.
*/

TQPopupMenu *TQTextEdit::createPopupMenu( const TQPoint& pos )
{
    Q_UNUSED( pos )
#ifndef TQT_NO_POPUPMENU
    TQPopupMenu *popup = new TQPopupMenu( this, "qt_edit_menu" );
    if ( !isReadOnly() ) {
	d->id[ IdUndo ] = popup->insertItem( tr( "&Undo" ) + ACCEL_KEY( Z ) );
	d->id[ IdRedo ] = popup->insertItem( tr( "&Redo" ) + ACCEL_KEY( Y ) );
	popup->insertSeparator();
    }
#ifndef TQT_NO_CLIPBOARD
    if ( !isReadOnly() )
	d->id[ IdCut ] = popup->insertItem( tr( "Cu&t" ) + ACCEL_KEY( X ) );
    d->id[ IdCopy ] = popup->insertItem( tr( "&Copy" ) + ACCEL_KEY( C ) );
    if ( !isReadOnly() )
	d->id[ IdPaste ] = popup->insertItem( tr( "&Paste" ) + ACCEL_KEY( V ) );
#endif
    if ( !isReadOnly() ) {
	d->id[ IdClear ] = popup->insertItem( tr( "Clear" ) );
	popup->insertSeparator();
    }
#if defined(TQ_WS_X11)
    d->id[ IdSelectAll ] = popup->insertItem( tr( "Select All" ) );
#else
    d->id[ IdSelectAll ] = popup->insertItem( tr( "Select All" ) + ACCEL_KEY( A ) );
#endif

#ifndef TQT_NO_IM
    TQInputContext *qic = getInputContext();
    if ( qic )
	qic->addMenusTo( popup );
#endif
    
    popup->setItemEnabled( d->id[ IdUndo ], !isReadOnly() && doc->commands()->isUndoAvailable() );
    popup->setItemEnabled( d->id[ IdRedo ], !isReadOnly() && doc->commands()->isRedoAvailable() );
#ifndef TQT_NO_CLIPBOARD
    popup->setItemEnabled( d->id[ IdCut ], !isReadOnly() && doc->hasSelection( TQTextDocument::Standard, true ) );
#ifdef QT_TEXTEDIT_OPTIMIZATION
    popup->setItemEnabled( d->id[ IdCopy ], d->optimMode ? optimHasSelection() : doc->hasSelection( TQTextDocument::Standard, true ) );
#else
    popup->setItemEnabled( d->id[ IdCopy ], doc->hasSelection( TQTextDocument::Standard, true ) );
#endif
    popup->setItemEnabled( d->id[ IdPaste ], !isReadOnly() && !TQApplication::clipboard()->text( d->clipboard_mode ).isEmpty() );
#endif
    const bool isEmptyDocument = (length() == 0);
    popup->setItemEnabled( d->id[ IdClear ], !isReadOnly() && !isEmptyDocument );
    popup->setItemEnabled( d->id[ IdSelectAll ], !isEmptyDocument );
    return popup;
#else
    return 0;
#endif
}

/*! \overload
    \obsolete
    This function is called to create a right mouse button popup menu.
    If you want to create a custom popup menu, reimplement this function
    and return the created popup menu. Ownership of the popup menu is
    transferred to the caller.

    This function is only called if createPopupMenu( const TQPoint & )
    returns 0.
*/

TQPopupMenu *TQTextEdit::createPopupMenu()
{
    return 0;
}

/*!
    \reimp
*/

void TQTextEdit::setFont( const TQFont &f )
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	TQScrollView::setFont( f );
	doc->setDefaultFormat( f, doc->formatCollection()->defaultFormat()->color() );
	// recalculate the max string width
	TQFontMetrics fm(f);
	int i, sw;
	d->od->maxLineWidth = 0;
	for ( i = 0; i < d->od->numLines; i++ ) {
	    sw = fm.width(d->od->lines[LOGOFFSET(i)]);
	    if (d->od->maxLineWidth < sw)
		d->od->maxLineWidth = sw;
	}
	resizeContents(d->od->maxLineWidth + 4, d->od->numLines * fm.lineSpacing() + 1);
	return;
    }
#endif
    TQScrollView::setFont( f );
    doc->setMinimumWidth( -1 );
    doc->setDefaultFormat( f, doc->formatCollection()->defaultFormat()->color() );
    lastFormatted = doc->firstParagraph();
    formatMore();
    repaintChanged();
}

/*!
    \fn TQTextEdit::zoomIn()

    \overload

    Zooms in on the text by making the base font size one point
    larger and recalculating all font sizes to be the new size. This
    does not change the size of any images.

    \sa zoomOut()
*/

/*!
    \fn TQTextEdit::zoomOut()

    \overload

    Zooms out on the text by making the base font size one point
    smaller and recalculating all font sizes to be the new size. This
    does not change the size of any images.

    \sa zoomIn()
*/


/*!
    Zooms in on the text by making the base font size \a range
    points larger and recalculating all font sizes to be the new size.
    This does not change the size of any images.

    \sa zoomOut()
*/

void TQTextEdit::zoomIn( int range )
{
    TQFont f( TQScrollView::font() );
    TQFontInfo fi(f);
    if (fi.pointSize() <= 0) {
       f.setPixelSize( fi.pixelSize() + range );
    } else {
       f.setPointSize( fi.pointSize() + range );
    }
    setFont( f );
}

/*!
    Zooms out on the text by making the base font size \a range points
    smaller and recalculating all font sizes to be the new size. This
    does not change the size of any images.

    \sa zoomIn()
*/

void TQTextEdit::zoomOut( int range )
{
    TQFont f( TQScrollView::font() );
    TQFontInfo fi(f);
    if (fi.pointSize() <= 0) {
       f.setPixelSize( TQMAX( 1, fi.pixelSize() - range ) );
    } else {
       f.setPointSize( TQMAX( 1, fi.pointSize() - range ) );
    }
    setFont( f );
}

/*!
    Zooms the text by making the base font size \a size points and
    recalculating all font sizes to be the new size. This does not
    change the size of any images.
*/

void TQTextEdit::zoomTo( int size )
{
    TQFont f( TQScrollView::font() );
    f.setPointSize( size );
    setFont( f );
}

/*!
   TQTextEdit is optimized for large amounts text. One of its
   optimizations is to format only the visible text, formatting the rest
   on demand, e.g. as the user scrolls, so you don't usually need to
   call this function.

    In some situations you may want to force the whole text
    to be formatted. For example, if after calling setText(), you wanted
    to know the height of the document (using contentsHeight()), you
    would call this function first.
*/

void TQTextEdit::sync()
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	TQFontMetrics fm( TQScrollView::font() );
	resizeContents( d->od->maxLineWidth + 4, d->od->numLines * fm.lineSpacing() + 1 );
    } else
#endif
    {
	while ( lastFormatted ) {
	    lastFormatted->format();
	    lastFormatted = lastFormatted->next();
	}
	resizeContents( contentsWidth(), doc->height() );
    }
    updateScrollBars();
}

/*!
    \reimp
*/

void TQTextEdit::setEnabled( bool b )
{
    TQScrollView::setEnabled( b );
    if ( textFormat() == PlainText ) {
	TQTextFormat *f = doc->formatCollection()->defaultFormat();
	f->setColor( colorGroup().text() );
	updateContents();
    }
}

/*!
    Sets the background color of selection number \a selNum to \a back
    and specifies whether the text of this selection should be
    inverted with \a invertText.

    This only works for \a selNum > 0. The default selection (\a
    selNum == 0) gets its attributes from the text edit's
    colorGroup().
*/

void TQTextEdit::setSelectionAttributes( int selNum, const TQColor &back, bool invertText )
{
    if ( selNum < 1 )
	return;
    if ( selNum > doc->numSelections() )
	doc->addSelection( selNum );
    doc->setSelectionColor( selNum, back );
    doc->setInvertSelectionText( selNum, invertText );
}

/*!
    \reimp
*/
void TQTextEdit::windowActivationChange( bool oldActive )
{
    if ( oldActive && scrollTimer )
	scrollTimer->stop();
    if ( palette().active() != palette().inactive() )
	updateContents();
    TQScrollView::windowActivationChange( oldActive );
}

void TQTextEdit::setReadOnly( bool b )
{
    if ( (bool) readOnly == b )
	return;
    readOnly = b;
#ifndef TQT_NO_CURSOR
    if ( readOnly )
	viewport()->setCursor( arrowCursor );
    else
	viewport()->setCursor( ibeamCursor );
    setInputMethodEnabled( !readOnly );
#endif
#ifdef QT_TEXTEDIT_OPTIMIZATION
    checkOptimMode();
#endif
}

/*!
    Scrolls to the bottom of the document and does formatting if
    required.
*/

void TQTextEdit::scrollToBottom()
{
    sync();
    setContentsPos( contentsX(), contentsHeight() - visibleHeight() );
}

/*!
    Returns the rectangle of the paragraph \a para in contents
    coordinates, or an invalid rectangle if \a para is out of range.
*/

TQRect TQTextEdit::paragraphRect( int para ) const
{
    TQTextEdit *that = (TQTextEdit *)this;
    that->sync();
    TQTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return TQRect( -1, -1, -1, -1 );
    return p->rect();
}

/*!
    Returns the paragraph which is at position \a pos (in contents
    coordinates).
*/

int TQTextEdit::paragraphAt( const TQPoint &pos ) const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	TQFontMetrics fm( TQScrollView::font() );
	int parag = pos.y() / fm.lineSpacing();
	if ( parag <= d->od->numLines )
	    return parag;
	else
	    return 0;
    }
#endif
    TQTextCursor c( doc );
    c.place( pos, doc->firstParagraph() );
    if ( c.paragraph() )
	return c.paragraph()->paragId();
    return -1; // should never happen..
}

/*!
    Returns the index of the character (relative to its paragraph) at
    position \a pos (in contents coordinates). If \a para is not 0,
    \a *para is set to the character's paragraph.
*/

int TQTextEdit::charAt( const TQPoint &pos, int *para ) const
{
#ifdef QT_TEXTEDIT_OPTIMIZATION
    if ( d->optimMode ) {
	int par = paragraphAt( pos );
	if ( para )
	    *para = par;
	return optimCharIndex( d->od->lines[ LOGOFFSET(par) ], pos.x() );
    }
#endif
    TQTextCursor c( doc );
    c.place( pos, doc->firstParagraph() );
    if ( c.paragraph() ) {
	if ( para )
	    *para = c.paragraph()->paragId();
	return c.index();
    }
    return -1; // should never happen..
}

/*!
    Sets the background color of the paragraph \a para to \a bg.
*/

void TQTextEdit::setParagraphBackgroundColor( int para, const TQColor &bg )
{
    TQTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return;
    p->setBackgroundColor( bg );
    repaintChanged();
}

/*!
    Clears the background color of the paragraph \a para, so that the
    default color is used again.
*/

void TQTextEdit::clearParagraphBackground( int para )
{
    TQTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return;
    p->clearBackgroundColor();
    repaintChanged();
}

/*!
    Returns the background color of the paragraph \a para or an
    invalid color if \a para is out of range or the paragraph has no
    background set
*/

TQColor TQTextEdit::paragraphBackgroundColor( int para ) const
{
    TQTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return TQColor();
    TQColor *c = p->backgroundColor();
    if ( c )
	return *c;
    return TQColor();
}

/*!
    \property TQTextEdit::undoRedoEnabled
    \brief whether undo/redo is enabled

    When changing this property, the undo/redo history is cleared.

    The default is true.
*/

void TQTextEdit::setUndoRedoEnabled( bool b )
{
    undoRedoInfo.clear();
    doc->commands()->clear();

    undoEnabled = b;
}

bool TQTextEdit::isUndoRedoEnabled() const
{
    return undoEnabled;
}

/*!
    Returns true if undo is available; otherwise returns false.
*/

bool TQTextEdit::isUndoAvailable() const
{
    return undoEnabled && (doc->commands()->isUndoAvailable() || undoRedoInfo.valid());
}

/*!
    Returns true if redo is available; otherwise returns false.
*/

bool TQTextEdit::isRedoAvailable() const
{
    return undoEnabled && doc->commands()->isRedoAvailable();
}

void TQTextEdit::ensureFormatted( TQTextParagraph *p )
{
    while ( !p->isValid() ) {
	if ( !lastFormatted )
	    return;
	formatMore();
    }
}

/*! \internal */
void TQTextEdit::updateCursor( const TQPoint & pos )
{
    if ( isReadOnly() && linksEnabled() ) {
	TQTextCursor c = *cursor;
	placeCursor( pos, &c, true );

#ifndef TQT_NO_NETWORKPROTOCOL
	bool insideParagRect = true;
	if (c.paragraph() == doc->lastParagraph()
	    && c.paragraph()->rect().y() + c.paragraph()->rect().height() < pos.y())
	    insideParagRect = false;
	if (insideParagRect && c.paragraph() && c.paragraph()->at( c.index() ) &&
	    c.paragraph()->at( c.index() )->isAnchor()) {
	    if (!c.paragraph()->at( c.index() )->anchorHref().isEmpty()
		    && c.index() < c.paragraph()->length() - 1 )
		onLink = c.paragraph()->at( c.index() )->anchorHref();
	    else
		onLink = TQString::null;

	    if (!c.paragraph()->at( c.index() )->anchorName().isEmpty()
		    && c.index() < c.paragraph()->length() - 1 )
		d->onName = c.paragraph()->at( c.index() )->anchorName();
	    else
		d->onName = TQString::null;

	    if (!c.paragraph()->at( c.index() )->anchorHref().isEmpty() ) {
#ifndef TQT_NO_CURSOR
		viewport()->setCursor( onLink.isEmpty() ? arrowCursor : pointingHandCursor );
#endif
		TQUrl u( doc->context(), onLink, true );
		emitHighlighted( u.toString( false, false ) );
	    }
	} else {
#ifndef TQT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	    onLink = TQString::null;
	    emitHighlighted( TQString::null );
	}
#endif
    }
}

/*!
  Places the cursor \a c at the character which is closest to position
  \a pos (in contents coordinates). If \a c is 0, the default text
  cursor is used.

  \sa setCursorPosition()
*/
void TQTextEdit::placeCursor( const TQPoint &pos, TQTextCursor *c )
{
    placeCursor( pos, c, false );
}

/*! \internal */
void TQTextEdit::clipboardChanged()
{
#ifndef TQT_NO_CLIPBOARD
    // don't listen to selection changes
    disconnect( TQApplication::clipboard(), TQ_SIGNAL(selectionChanged()), this, 0);
#endif
    selectAll(false);
}

/*! \property TQTextEdit::tabChangesFocus
  \brief whether TAB changes focus or is accepted as input

  In some occasions text edits should not allow the user to input
  tabulators or change indentation using the TAB key, as this breaks
  the focus chain. The default is false.

*/

void TQTextEdit::setTabChangesFocus( bool b )
{
    d->tabChangesFocus = b;
}

bool TQTextEdit::tabChangesFocus() const
{
    return d->tabChangesFocus;
}

#ifdef QT_TEXTEDIT_OPTIMIZATION
/* Implementation of optimized LogText mode follows */

static void tqSwap( int * a, int * b )
{
    if ( !a || !b )
	return;
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/*! \internal */
bool TQTextEdit::checkOptimMode()
{
    bool oldMode = d->optimMode;
    if ( textFormat() == LogText ) {
	setReadOnly( true );
	d->optimMode = true;
    } else {
	d->optimMode = false;
    }

    // when changing mode - try to keep selections and text
    if ( oldMode != d->optimMode ) {
	if ( d->optimMode ) {
	    d->od = new TQTextEditOptimPrivate;
	    connect( scrollTimer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( optimDoAutoScroll() ) );
	    disconnect( doc, TQ_SIGNAL( minimumWidthChanged(int) ), this, TQ_SLOT( documentWidthChanged(int) ) );
	    disconnect( scrollTimer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( autoScrollTimerDone() ) );
	    disconnect( formatTimer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( formatMore() ) );
	    optimSetText( doc->originalText() );
	    doc->clear(true);
	    delete cursor;
	    cursor = new TQTextCursor( doc );
	} else {
	    disconnect( scrollTimer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( optimDoAutoScroll() ) );
	    connect( doc, TQ_SIGNAL( minimumWidthChanged(int) ), this, TQ_SLOT( documentWidthChanged(int) ) );
	    connect( scrollTimer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( autoScrollTimerDone() ) );
	    connect( formatTimer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( formatMore() ) );
	    setText( optimText() );
	    delete d->od;
	    d->od = 0;
	}
    }
    return d->optimMode;
}

/*! \internal */
TQString TQTextEdit::optimText() const
{
    TQString str, tmp;

    if ( d->od->len == 0 )
	return str;

    // concatenate all strings
    int i;
    int offset;
    TQMapConstIterator<int,TQTextEditOptimPrivate::Tag *> it;
    TQTextEditOptimPrivate::Tag * ftag = 0;
    for ( i = 0; i < d->od->numLines; i++ ) {
	if ( d->od->lines[ LOGOFFSET(i) ].isEmpty() ) { // CR lines are empty
	    str += "\n";
	} else {
	    tmp = d->od->lines[ LOGOFFSET(i) ] + "\n";
	    // inject the tags for this line
	    if ( (it = d->od->tagIndex.find( LOGOFFSET(i) )) != d->od->tagIndex.end() )
		ftag = it.data();
	    offset = 0;
	    while ( ftag && ftag->line == i ) {
		tmp.insert( ftag->index + offset, "<" + ftag->tag + ">" );
		offset += ftag->tag.length() + 2; // 2 -> the '<' and '>' chars
		ftag = ftag->next;
	    }
	    str += tmp;
	}
    }
    return str;
}

/*! \internal */
void TQTextEdit::optimSetText( const TQString &str )
{
    optimRemoveSelection();
// this is just too slow - but may have to go in due to compatibility reasons
//     if ( str == optimText() )
//	return;
    d->od->numLines = 0;
    d->od->lines.clear();
    d->od->maxLineWidth = 0;
    d->od->len = 0;
    d->od->clearTags();
    TQFontMetrics fm( TQScrollView::font() );
    if ( !(str.isEmpty() || str.isNull() || d->maxLogLines == 0) ) {
	TQStringList strl = TQStringList::split( '\n', str, true );
	int lWidth = 0;
	for ( TQStringList::Iterator it = strl.begin(); it != strl.end(); ++it ) {
	    optimParseTags( &*it );
	    optimCheckLimit( *it );
	    lWidth = fm.width( *it );
	    if ( lWidth > d->od->maxLineWidth )
		d->od->maxLineWidth = lWidth;
	}
    }
    resizeContents( d->od->maxLineWidth + 4, d->od->numLines * fm.lineSpacing() + 1 );
    repaintContents();
    emit textChanged();
}

/*! \internal

  Append \a tag to the tag list.
*/
TQTextEditOptimPrivate::Tag * TQTextEdit::optimAppendTag( int index, const TQString & tag )
{
    TQTextEditOptimPrivate::Tag * t = new TQTextEditOptimPrivate::Tag, * tmp;

    if ( d->od->tags == 0 )
	d->od->tags = t;
    t->bold = t->italic = t->underline = false;
    t->line  = d->od->numLines;
    t->index = index;
    t->tag   = tag;
    t->leftTag = 0;
    t->parent  = 0;
    t->prev = d->od->lastTag;
    if ( d->od->lastTag )
	d->od->lastTag->next = t;
    t->next = 0;
    d->od->lastTag = t;
    tmp = d->od->tagIndex[ LOGOFFSET(t->line) ];
    if ( !tmp || (tmp && tmp->index > t->index) ) {
	d->od->tagIndex.replace( LOGOFFSET(t->line), t );
    }
    return t;
}

 /*! \internal

   Insert \a tag in the tag - according to line and index numbers
*/

TQTextEditOptimPrivate::Tag *TQTextEdit::optimInsertTag(int line, int index, const TQString &tag)
{
    TQTextEditOptimPrivate::Tag *t = new TQTextEditOptimPrivate::Tag, *tmp;

    if (d->od->tags == 0)
	d->od->tags = t;
    t->bold = t->italic = t->underline = false;
    t->line  = line;
    t->index = index;
    t->tag   = tag;
    t->leftTag = 0;
    t->parent  = 0;
    t->next = 0;
    t->prev = 0;

    // find insertion pt. in tag struct.
    TQMap<int,TQTextEditOptimPrivate::Tag *>::ConstIterator it;
    if ((it = d->od->tagIndex.find(LOGOFFSET(line))) != d->od->tagIndex.end()) {
	tmp = *it;
	if (tmp->index >= index) { // the exisiting tag may be placed AFTER the one we want to insert
	    tmp = tmp->prev;
	} else {
	    while (tmp && tmp->next && tmp->next->line == line && tmp->next->index <= index)
		tmp = tmp->next;
	}
    } else {
	tmp = d->od->tags;
	while (tmp && tmp->next && tmp->next->line < line)
	    tmp = tmp->next;
	if (tmp == d->od->tags)
	    tmp = 0;
    }

    t->prev = tmp;
    t->next = tmp ? tmp->next : 0;
    if (t->next)
	t->next->prev = t;
    if (tmp)
	tmp->next = t;

    tmp = d->od->tagIndex[LOGOFFSET(t->line)];
    if (!tmp || (tmp && tmp->index >= t->index)) {
	d->od->tagIndex.replace(LOGOFFSET(t->line), t);
    }
    return t;
}


/*! \internal

  Find tags in \a line, remove them from \a line and put them in a
  structure.

  A tag is delimited by '<' and '>'. The characters '<', '>' and '&'
  are escaped by using '&lt;', '&gt;' and '&amp;'. Left-tags marks
  the starting point for formatting, while right-tags mark the ending
  point. A right-tag is the same as a left-tag, but with a '/'
  appearing before the tag keyword.  E.g a valid left-tag: <b>, and
  a valid right-tag: </b>.  Tags can be nested, but they have to be
  closed in the same order as they are opened. E.g:
  <font color=red><font color=blue>blue</font>red</font> - is valid, while:
  <font color=red><b>bold red</font> just bold</b> - is invalid since the font tag is
  closed before the bold tag. Note that a tag does not have to be
  closed: '<font color=blue>Lots of text - and then some..'  is perfectly valid for
  setting all text appearing after the tag to blue.  A tag can be used
  to change the color of a piece of text, or set one of the following
  formatting attributes: bold, italic and underline.  These attributes
  are set using the <b>, <i> and <u> tags.  Example of valid tags:
  <font color=red>, </font>, <b>, <u>, <i>, </i>.
  Example of valid text:
  This is some <font color=red>red text</font>, while this is some <font color=green>green
  text</font>. <font color=blue><font color=yellow>This is yellow</font>, while this is
  blue.</font>

  Note that only the color attribute of the HTML font tag is supported.

  Limitations:
  1. A tag cannot span several lines.
  2. Very limited error checking - mismatching left/right-tags is the
  only thing that is detected.

*/
void TQTextEdit::optimParseTags( TQString * line, int lineNo, int indexOffset )
{
    int len = line->length();
    int i, startIndex = -1, endIndex = -1, escIndex = -1;
    int state = 0; // 0 = outside tag, 1 = inside tag
    bool tagOpen, tagClose;
    int bold = 0, italic = 0, underline = 0;
    TQString tagStr;
    TQPtrStack<TQTextEditOptimPrivate::Tag> tagStack;

    for ( i = 0; i < len; i++ ) {
	tagOpen = (*line)[i] == '<';
	tagClose = (*line)[i] == '>';

	// handle '&lt;' and '&gt;' and '&amp;'
	if ( (*line)[i] == '&' ) {
	    escIndex = i;
	    continue;
	} else if ( escIndex != -1 && (*line)[i] == ';' ) {
	    TQString esc = line->mid( escIndex, i - escIndex + 1 );
	    TQString c;
	    if ( esc == "&lt;" )
		c = '<';
	    else if ( esc == "&gt;" )
		c = '>';
	    else if ( esc == "&amp;" )
		c = '&';
	    line->replace( escIndex, i - escIndex + 1, c );
	    len = line->length();
	    i -= i-escIndex;
	    escIndex = -1;
	    continue;
	}

	if ( state == 0 && tagOpen ) {
	    state = 1;
	    startIndex = i;
	    continue;
	}
	if ( state == 1 && tagClose ) {
	    state = 0;
	    endIndex = i;
	    if ( !tagStr.isEmpty() ) {
		TQTextEditOptimPrivate::Tag * tag, * cur, * tmp;
		bool format = true;

		if ( tagStr == "b" )
		    bold++;
		else if ( tagStr == "/b" )
		    bold--;
		else if ( tagStr == "i" )
		    italic++;
		else if ( tagStr == "/i" )
		    italic--;
		else if ( tagStr == "u" )
		    underline++;
		else if ( tagStr == "/u" )
		    underline--;
		else
		    format = false;
		if ( lineNo > -1 )
		    tag = optimInsertTag( lineNo, startIndex + indexOffset, tagStr );
		else
		    tag = optimAppendTag( startIndex, tagStr );
		// everything that is not a b, u or i tag is considered
		// to be a color tag.
		tag->type = format ? TQTextEditOptimPrivate::Format
			    : TQTextEditOptimPrivate::Color;
		if ( tagStr[0] == '/' ) {
		    // this is a right-tag - search for the left-tag
		    // and possible parent tag
		    cur = tag->prev;
		    if ( !cur ) {
#ifdef QT_CHECK_RANGE
			tqWarning( "TQTextEdit::optimParseTags: no left-tag for '<%s>' in line %d.", tag->tag.ascii(), tag->line + 1 );
#endif
			return; // something is wrong - give up
		    }
		    while ( cur ) {
			if ( cur->leftTag ) { // push right-tags encountered
			    tagStack.push( cur );
			} else {
			    tmp = tagStack.pop();
			    if ( !tmp ) {
				if ( (("/" + cur->tag) == tag->tag) ||
				     (tag->tag == "/font" && cur->tag.left(4) == "font") ) {
				    // set up the left and parent of this tag
				    tag->leftTag = cur;
				    tmp = cur->prev;
				    if ( tmp && tmp->parent ) {
					tag->parent = tmp->parent;
				    } else if ( tmp && !tmp->leftTag ) {
					tag->parent = tmp;
				    }
				    break;
				} else if ( !cur->leftTag ) {
#ifdef QT_CHECK_RANGE
				    tqWarning( "TQTextEdit::optimParseTags: mismatching %s-tag for '<%s>' in line %d.", cur->tag[0] == '/' ? "left" : "right", cur->tag.ascii(), cur->line + 1 );
#endif
				    return; // something is amiss - give up
				}
			    }
			}
			cur = cur->prev;
		    }
		} else {
		    tag->bold = bold > 0;
		    tag->italic = italic > 0;
		    tag->underline = underline > 0;
		    tmp = tag->prev;
		    while ( tmp && tmp->leftTag ) {
			tmp = tmp->leftTag->parent;
		    }
		    if ( tmp ) {
			tag->bold |= tmp->bold;
			tag->italic |= tmp->italic;
			tag->underline |= tmp->underline;
		    }
		}
	    }
	    if ( startIndex != -1 ) {
		int l = (endIndex == -1) ?
			line->length() - startIndex : endIndex - startIndex;
		line->remove( startIndex, l+1 );
		len = line->length();
		i -= l+1;
	    }
	    tagStr = "";
	    continue;
	}

	if ( state == 1 ) {
	    tagStr += (*line)[i];
	}
    }
}

// calculate the width of a string in pixels inc. tabs
static int qStrWidth(const TQString& str, int tabWidth, const TQFontMetrics& fm)
{
    int tabs = str.contains('\t');

    if (!tabs)
	return fm.width(str);

    int newIdx = 0;
    int lastIdx = 0;
    int strWidth = 0;
    int tn;
    for (tn = 1; tn <= tabs; ++tn) {
	newIdx = str.find('\t', newIdx);
	strWidth += fm.width(str.mid(lastIdx, newIdx - lastIdx));
	if (strWidth >= tn * tabWidth) {
	    int u = tn;
	    while (strWidth >= u * tabWidth)
		++u;
	    strWidth = u * tabWidth;
	} else {
	    strWidth = tn * tabWidth;
	}
	lastIdx = ++newIdx;
    }
    if ((int)str.length() > newIdx)
	strWidth += fm.width(str.mid(newIdx));
    return strWidth;
}

bool TQTextEdit::optimHasBoldMetrics(int line)
{
    TQTextEditOptimPrivate::Tag *t;
    TQMapConstIterator<int,TQTextEditOptimPrivate::Tag *> it;
    if ((it = d->od->tagIndex.find(line)) != d->od->tagIndex.end()) {
	t = *it;
	while (t && t->line == line) {
	    if (t->bold)
		return true;
	    t = t->next;
	}
    } else if ((t = optimPreviousLeftTag(line)) && t->bold) {
	return true;
    }
    return false;
}

/*! \internal

  Append \a str to the current text buffer. Parses each line to find
  formatting tags.
*/
void TQTextEdit::optimAppend( const TQString &str )
{
    if ( str.isEmpty() || str.isNull() || d->maxLogLines == 0 )
	return;

    TQStringList strl = TQStringList::split( '\n', str, true );
    TQStringList::Iterator it = strl.begin();

    TQFontMetrics fm(TQScrollView::font());
    int lWidth = 0;

    for ( ; it != strl.end(); ++it ) {
	optimParseTags( &*it );
	optimCheckLimit( *it );
	if (optimHasBoldMetrics(d->od->numLines-1)) {
	    TQFont fnt = TQScrollView::font();
	    fnt.setBold(true);
	    fm = TQFontMetrics(fnt);
	}
	lWidth = qStrWidth(*it, tabStopWidth(), fm) + 4;
	if ( lWidth > d->od->maxLineWidth )
	    d->od->maxLineWidth = lWidth;
    }
    bool scrollToEnd = contentsY() >= contentsHeight() - visibleHeight();
    resizeContents( d->od->maxLineWidth + 4, d->od->numLines * fm.lineSpacing() + 1 );
    if ( scrollToEnd ) {
	updateScrollBars();
	ensureVisible( contentsX(), contentsHeight(), 0, 0 );
    }
    // when a max log size is set, the text may not be redrawn because
    // the size of the viewport may not have changed
    if ( d->maxLogLines > -1 )
	viewport()->update();
    emit textChanged();
}


static void qStripTags(TQString *line)
{
    int len = line->length();
    int i, startIndex = -1, endIndex = -1, escIndex = -1;
    int state = 0; // 0 = outside tag, 1 = inside tag
    bool tagOpen, tagClose;

    for ( i = 0; i < len; i++ ) {
	tagOpen = (*line)[i] == '<';
	tagClose = (*line)[i] == '>';

	// handle '&lt;' and '&gt;' and '&amp;'
	if ( (*line)[i] == '&' ) {
	    escIndex = i;
	    continue;
	} else if ( escIndex != -1 && (*line)[i] == ';' ) {
	    TQString esc = line->mid( escIndex, i - escIndex + 1 );
	    TQString c;
	    if ( esc == "&lt;" )
		c = '<';
	    else if ( esc == "&gt;" )
		c = '>';
	    else if ( esc == "&amp;" )
		c = '&';
	    line->replace( escIndex, i - escIndex + 1, c );
	    len = line->length();
	    i -= i-escIndex;
	    escIndex = -1;
	    continue;
	}

	if ( state == 0 && tagOpen ) {
	    state = 1;
	    startIndex = i;
	    continue;
	}
	if ( state == 1 && tagClose ) {
	    state = 0;
	    endIndex = i;
	    if ( startIndex != -1 ) {
		int l = (endIndex == -1) ?
			line->length() - startIndex : endIndex - startIndex;
		line->remove( startIndex, l+1 );
		len = line->length();
		i -= l+1;
	    }
	    continue;
	}
    }
}

/*! \internal

    Inserts the text into \a line at index \a index.
*/

void TQTextEdit::optimInsert(const TQString& text, int line, int index)
{
    if (text.isEmpty() || d->maxLogLines == 0)
	return;
    if (line < 0)
	line = 0;
    if (line > d->od->numLines-1)
	line = d->od->numLines-1;
    if (index < 0)
	index = 0;
    if (index > (int) d->od->lines[line].length())
	index = d->od->lines[line].length();

    TQStringList strl = TQStringList::split('\n', text, true);
    int numNewLines = (int)strl.count() - 1;
    TQTextEditOptimPrivate::Tag *tag = 0;
    TQMap<int,TQTextEditOptimPrivate::Tag *>::ConstIterator ii;
    int x;

    if (numNewLines == 0) {
	// Case 1. Fast single line case - just inject it!
	TQString stripped = text;
	qStripTags( &stripped );
	d->od->lines[LOGOFFSET(line)].insert(index, stripped);
	// move the tag indices following the insertion pt.
	if ((ii = d->od->tagIndex.find(LOGOFFSET(line))) != d->od->tagIndex.end()) {
	    tag = *ii;
	    while (tag && (LOGOFFSET(tag->line) == line && tag->index < index))
		tag = tag->next;
	    while (tag && (LOGOFFSET(tag->line) == line)) {
		tag->index += stripped.length();
		tag = tag->next;
	    }
	}
	stripped = text;
	optimParseTags(&stripped, line, index);
    } else if (numNewLines > 0) {
        // Case 2. We have at least 1 newline char - split at
        // insertion pt. and make room for new lines - complex and slow!
	TQString left = d->od->lines[LOGOFFSET(line)].left(index);
	TQString right = d->od->lines[LOGOFFSET(line)].mid(index);

	// rearrange lines for insertion
	for (x = d->od->numLines - 1; x > line; x--)
	    d->od->lines[x + numNewLines] = d->od->lines[x];
	d->od->numLines += numNewLines;

	// fix the tag index and the tag line/index numbers - this
	// might take a while..
	for (x = line; x < d->od->numLines; x++) {
	    if ((ii = d->od->tagIndex.find(LOGOFFSET(line))) != d->od->tagIndex.end()) {
		tag = ii.data();
		if (LOGOFFSET(tag->line) == line)
		    while (tag && (LOGOFFSET(tag->line) == line && tag->index < index))
			tag = tag->next;
	    }
	}

	// relabel affected tags with new line numbers and new index
	// positions
	while (tag) {
	    if (LOGOFFSET(tag->line) == line)
		tag->index -= index;
	    tag->line += numNewLines;
	    tag = tag->next;
	}

	// generate a new tag index
	d->od->tagIndex.clear();
	tag = d->od->tags;
	while (tag) {
	    if (!((ii = d->od->tagIndex.find(LOGOFFSET(tag->line))) != d->od->tagIndex.end()))
		d->od->tagIndex[LOGOFFSET(tag->line)] = tag;
	    tag = tag->next;
	}

	// update the tag indices on the spliced line - needs to be done before new tags are added
	TQString stripped = strl[strl.count() - 1];
	qStripTags(&stripped);
	if ((ii = d->od->tagIndex.find(LOGOFFSET(line + numNewLines))) != d->od->tagIndex.end()) {
	    tag = *ii;
	    while (tag && (LOGOFFSET(tag->line) == line + numNewLines)) {
		tag->index += stripped.length();
		tag = tag->next;
	    }
	}

	// inject the new lines
	TQStringList::Iterator it = strl.begin();
	x = line;
	int idx;
	for (; it != strl.end(); ++it) {
	    stripped = *it;
	    qStripTags(&stripped);
	    if (x == line) {
		stripped = left + stripped;
		idx = index;
	    } else {
		idx = 0;
	    }
	    d->od->lines[LOGOFFSET(x)] = stripped;
	    optimParseTags(&*it, x++, idx);
	}
	d->od->lines[LOGOFFSET(x - 1)] += right;
    }
    // recalculate the pixel width of the longest injected line -
    TQFontMetrics fm(TQScrollView::font());
    int lWidth = 0;

    for (x = line; x < line + numNewLines; x++) {
	if (optimHasBoldMetrics(x)) {
	    TQFont fnt = TQScrollView::font();
	    fnt.setBold(true);
	    fm = TQFontMetrics(fnt);
	}
	lWidth = fm.width(d->od->lines[x]) + 4;
	if (lWidth > d->od->maxLineWidth)
	    d->od->maxLineWidth = lWidth;
    }
    resizeContents(d->od->maxLineWidth + 4, d->od->numLines * fm.lineSpacing() + 1);
    repaintContents();
    emit textChanged();
}



/*! \internal

  Returns the first open left-tag appearing before line \a line.
 */
TQTextEditOptimPrivate::Tag * TQTextEdit::optimPreviousLeftTag( int line )
{
    TQTextEditOptimPrivate::Tag * ftag = 0;
    TQMapConstIterator<int,TQTextEditOptimPrivate::Tag *> it;
    if ( (it = d->od->tagIndex.find( LOGOFFSET(line) )) != d->od->tagIndex.end() )
	ftag = it.data();
    if ( !ftag ) {
	// start searching for an open tag
	ftag = d->od->tags;
	while ( ftag ) {
	    if ( ftag->line > line || ftag->next == 0 ) {
		if ( ftag->line > line )
		    ftag = ftag->prev;
		break;
	    }
	    ftag = ftag->next;
	}
    } else {
	ftag = ftag->prev;
    }

    if ( ftag ) {
	if ( ftag && ftag->parent ) // use the open parent tag
	    ftag = ftag->parent;
	else if ( ftag && ftag->leftTag ) // this is a right-tag with no parent
	    ftag = 0;
    }
    return ftag;
}

/*! \internal

  Set the format for the string starting at index \a start and ending
  at \a end according to \a tag. If \a tag is a Format tag, find the
  first open color tag appearing before \a tag and use that tag to
  color the string.
*/
void TQTextEdit::optimSetTextFormat( TQTextDocument * td, TQTextCursor * cur,
				    TQTextFormat * f, int start, int end,
				    TQTextEditOptimPrivate::Tag * tag )
{
    int formatFlags = TQTextFormat::Bold | TQTextFormat::Italic |
		      TQTextFormat::Underline;
    cur->setIndex( start );
    td->setSelectionStart( 0, *cur );
    cur->setIndex( end );
    td->setSelectionEnd( 0, *cur );
    TQStyleSheetItem * ssItem = styleSheet()->item( tag->tag );
    if ( !ssItem || tag->type == TQTextEditOptimPrivate::Format ) {
	f->setBold( tag->bold );
	f->setItalic( tag->italic );
	f->setUnderline( tag->underline );
	if ( tag->type == TQTextEditOptimPrivate::Format ) {
	    // check to see if there are any open color tags prior to
	    // this format tag
	    tag = tag->prev;
	    while ( tag && (tag->type == TQTextEditOptimPrivate::Format ||
			    tag->leftTag) ) {
		tag = tag->leftTag ? tag->parent : tag->prev;
	    }
	}
	if ( tag ) {
	    TQString col = tag->tag.simplifyWhiteSpace();
	    if ( col.left( 10 ) == "font color" ) {
		int i = col.find( '=', 10 );
		col = col.mid( i + 1 ).simplifyWhiteSpace();
		if ( col[0] == '\"' )
		    col = col.mid( 1, col.length() - 2 );
	    }
	    TQColor color = TQColor( col );
	    if ( color.isValid() ) {
		formatFlags |= TQTextFormat::Color;
		f->setColor( color );
	    }
	}
    } else { // use the stylesheet tag definition
	if ( ssItem->color().isValid() ) {
	    formatFlags |= TQTextFormat::Color;
	    f->setColor( ssItem->color() );
	}
	f->setBold( ssItem->fontWeight() == TQFont::Bold );
	f->setItalic( ssItem->fontItalic() );
	f->setUnderline( ssItem->fontUnderline() );
    }
    td->setFormat( 0, f, formatFlags );
    td->removeSelection( 0 );
}

/*! \internal */
void TQTextEdit::optimDrawContents( TQPainter * p, int clipx, int clipy,
				   int clipw, int cliph )
{
    TQFontMetrics fm( TQScrollView::font() );
    int startLine = clipy / fm.lineSpacing();

    // we always have to fetch at least two lines for drawing because the
    // painter may be translated so that parts of two lines cover the area
    // of a single line
    int nLines = (cliph / fm.lineSpacing()) + 2;
    int endLine = startLine + nLines;

    if ( startLine >= d->od->numLines )
	return;
    if ( (startLine + nLines) > d->od->numLines )
	nLines = d->od->numLines - startLine;

    int i = 0;
    TQString str;
    for ( i = startLine; i < (startLine + nLines); i++ )
	str.append( d->od->lines[ LOGOFFSET(i) ] + "\n" );

    TQTextDocument * td = new TQTextDocument( 0 );
    td->setDefaultFormat( TQScrollView::font(), TQColor() );
    td->setPlainText( str );
    td->setFormatter( new TQTextFormatterBreakWords ); // deleted by TQTextDoc
    td->formatter()->setWrapEnabled( false );
    td->setTabStops(doc->tabStopWidth());

    // get the current text color from the current format
    td->selectAll( TQTextDocument::Standard );
    TQTextFormat f;
    f.setColor( colorGroup().text() );
    f.setFont( TQScrollView::font() );
    td->setFormat( TQTextDocument::Standard, &f,
		   TQTextFormat::Color | TQTextFormat::Font );
    td->removeSelection( TQTextDocument::Standard );

    // add tag formatting
    if ( d->od->tags ) {
	int i = startLine;
	TQMapConstIterator<int,TQTextEditOptimPrivate::Tag *> it;
	TQTextEditOptimPrivate::Tag * tag = 0, * tmp = 0;
	TQTextCursor cur( td );
	// Step 1 - find previous left-tag
	tmp = optimPreviousLeftTag( i );
	for ( ; i < startLine + nLines; i++ ) {
	    if ( (it = d->od->tagIndex.find( LOGOFFSET(i) )) != d->od->tagIndex.end() )
		tag = it.data();
	    // Step 2 - iterate over tags on the current line
	    int lastIndex = 0;
	    while ( tag && tag->line == i ) {
		tmp = 0;
		if ( tag->prev && !tag->prev->leftTag ) {
		    tmp = tag->prev;
		} else if ( tag->prev && tag->prev->parent ) {
		    tmp = tag->prev->parent;
		}
		if ( (tag->index - lastIndex) > 0 && tmp ) {
		    optimSetTextFormat( td, &cur, &f, lastIndex, tag->index, tmp );
		}
		lastIndex = tag->index;
		tmp = tag;
		tag = tag->next;
	    }
	    // Step 3 - color last part of the line - if necessary
	    if ( tmp && tmp->parent )
		tmp = tmp->parent;
	    if ( (cur.paragraph()->length()-1 - lastIndex) > 0 && tmp && !tmp->leftTag ) {
		optimSetTextFormat( td, &cur, &f, lastIndex,
				    cur.paragraph()->length() - 1, tmp );
	    }
	    cur.setParagraph( cur.paragraph()->next() );
	}
        // useful debug info
	//
//	tag = d->od->tags;
//	tqWarning("###");
//	while ( tag ) {
//	    tqWarning( "Tag: %p, parent: %09p, leftTag: %09p, Name: %-15s, ParentName: %s, %d%d%d", tag,
//		       tag->parent, tag->leftTag, tag->tag.latin1(), tag->parent ? tag->parent->tag.latin1():"<none>",
//		      tag->bold, tag->italic, tag->underline );
//	    tag = tag->next;
//	}
    }

    // if there is a selection, make sure that the selection in the
    // part we need to redraw is set correctly
    if ( optimHasSelection() ) {
	TQTextCursor c1( td );
	TQTextCursor c2( td );
	int selStart = d->od->selStart.line;
	int idxStart = d->od->selStart.index;
	int selEnd = d->od->selEnd.line;
	int idxEnd = d->od->selEnd.index;
	if ( selEnd < selStart ) {
	    tqSwap( &selStart, &selEnd );
	    tqSwap( &idxStart, &idxEnd );
	}
	if ( selEnd > d->od->numLines-1 ) {
	    selEnd = d->od->numLines-1;
	}
	if ( startLine <= selStart && endLine >= selEnd ) {
	    // case 1: area to paint covers entire selection
	    int paragS = selStart - startLine;
	    int paragE = paragS + (selEnd - selStart);
	    TQTextParagraph * parag = td->paragAt( paragS );
	    if ( parag ) {
		c1.setParagraph( parag );
		if ( td->text( paragS ).length() >= (uint) idxStart )
		    c1.setIndex( idxStart );
	    }
	    parag = td->paragAt( paragE );
	    if ( parag ) {
		c2.setParagraph( parag );
		if ( td->text( paragE ).length() >= (uint) idxEnd )
		    c2.setIndex( idxEnd );
	    }
	} else if ( startLine > selStart && endLine < selEnd ) {
	    // case 2: area to paint is all part of the selection
	    td->selectAll( TQTextDocument::Standard );
	} else if ( startLine > selStart && endLine >= selEnd &&
		    startLine <= selEnd ) {
	    // case 3: area to paint starts inside a selection, ends past it
	    c1.setParagraph( td->firstParagraph() );
	    c1.setIndex( 0 );
	    int paragE = selEnd - startLine;
	    TQTextParagraph * parag = td->paragAt( paragE );
	    if ( parag ) {
		c2.setParagraph( parag );
		if ( td->text( paragE ).length() >= (uint) idxEnd )
		    c2.setIndex( idxEnd );
	    }
	} else if ( startLine <= selStart && endLine < selEnd &&
		    endLine > selStart ) {
	    // case 4: area to paint starts before a selection, ends inside it
	    int paragS = selStart - startLine;
	    TQTextParagraph * parag = td->paragAt( paragS );
	    if ( parag ) {
		c1.setParagraph( parag );
		c1.setIndex( idxStart );
	    }
	    c2.setParagraph( td->lastParagraph() );
	    c2.setIndex( td->lastParagraph()->string()->toString().length() - 1 );

	}
	// previously selected?
	if ( !td->hasSelection( TQTextDocument::Standard ) ) {
	    td->setSelectionStart( TQTextDocument::Standard, c1 );
	    td->setSelectionEnd( TQTextDocument::Standard, c2 );
	}
    }
    td->doLayout( p, contentsWidth() );

    // have to align the painter so that partly visible lines are
    // drawn at the correct position within the area that needs to be
    // painted
    int offset = clipy % fm.lineSpacing() + 2;
    TQRect r( clipx, 0, clipw, cliph + offset );
    p->translate( 0, clipy - offset );
    td->draw( p, r.x(), r.y(), r.width(), r.height(), colorGroup() );
    p->translate( 0, -(clipy - offset) );
    delete td;
}

/*! \internal */
void TQTextEdit::optimMousePressEvent( TQMouseEvent * e )
{
    if ( e->button() != LeftButton )
	return;

    TQFontMetrics fm( TQScrollView::font() );
    mousePressed = true;
    mousePos = e->pos();
    d->od->selStart.line = e->y() / fm.lineSpacing();
    if ( d->od->selStart.line > d->od->numLines-1 ) {
	d->od->selStart.line = d->od->numLines-1;
	d->od->selStart.index = d->od->lines[ LOGOFFSET(d->od->numLines-1) ].length();
    } else {
	TQString str = d->od->lines[ LOGOFFSET(d->od->selStart.line) ];
	d->od->selStart.index = optimCharIndex( str, mousePos.x() );
    }
    d->od->selEnd.line = d->od->selStart.line;
    d->od->selEnd.index = d->od->selStart.index;
    oldMousePos = e->pos();
    repaintContents( false );
}

/*! \internal */
void TQTextEdit::optimMouseReleaseEvent( TQMouseEvent * e )
{
    if ( e->button() != LeftButton )
	return;

    if ( scrollTimer->isActive() )
	scrollTimer->stop();
    if ( !inDoubleClick ) {
	TQFontMetrics fm( TQScrollView::font() );
	d->od->selEnd.line = e->y() / fm.lineSpacing();
	if ( d->od->selEnd.line > d->od->numLines-1 ) {
	    d->od->selEnd.line = d->od->numLines-1;
	}
	TQString str = d->od->lines[ LOGOFFSET(d->od->selEnd.line) ];
	mousePos = e->pos();
	d->od->selEnd.index = optimCharIndex( str, mousePos.x() );
	if ( d->od->selEnd.line < d->od->selStart.line ) {
	    tqSwap( &d->od->selStart.line, &d->od->selEnd.line );
	    tqSwap( &d->od->selStart.index, &d->od->selEnd.index );
	} else if ( d->od->selStart.line == d->od->selEnd.line &&
		    d->od->selStart.index > d->od->selEnd.index ) {
	    tqSwap( &d->od->selStart.index, &d->od->selEnd.index );
	}
	oldMousePos = e->pos();
	repaintContents( false );
    }
    if ( mousePressed ) {
	mousePressed = false;
	copyToClipboard();
    }

    inDoubleClick = false;
    emit copyAvailable( optimHasSelection() );
    emit selectionChanged();
}

/*! \internal */
void TQTextEdit::optimMouseMoveEvent( TQMouseEvent * e )
{
    mousePos = e->pos();
    optimDoAutoScroll();
    oldMousePos = mousePos;
}

/*! \internal */
void TQTextEdit::optimDoAutoScroll()
{
    if ( !mousePressed )
	return;

    TQFontMetrics fm( TQScrollView::font() );
    TQPoint pos( mapFromGlobal( TQCursor::pos() ) );
    bool doScroll = false;
    int xx = contentsX() + pos.x();
    int yy = contentsY() + pos.y();

    // find out how much we have to scroll in either dir.
    if ( pos.x() < 0 || pos.x() > viewport()->width() ||
	 pos.y() < 0 || pos.y() > viewport()->height() ) {
	int my = yy;
	if ( pos.x() < 0 )
	    xx = contentsX() - fm.width( 'w');
	else if ( pos.x() > viewport()->width() )
	    xx = contentsX() + viewport()->width() + fm.width('w');

	if ( pos.y() < 0 ) {
	    my = contentsY() - 1;
	    yy = (my / fm.lineSpacing()) * fm.lineSpacing() + 1;
	} else if ( pos.y() > viewport()->height() ) {
	    my = contentsY() + viewport()->height() + 1;
	    yy = (my / fm.lineSpacing() + 1) * fm.lineSpacing() - 1;
	}
	d->od->selEnd.line = my / fm.lineSpacing();
	mousePos.setX( xx );
	mousePos.setY( my );
	doScroll = true;
    } else {
	d->od->selEnd.line = mousePos.y() / fm.lineSpacing();
    }

    if ( d->od->selEnd.line < 0 ) {
	d->od->selEnd.line = 0;
    } else if ( d->od->selEnd.line > d->od->numLines-1 ) {
	d->od->selEnd.line = d->od->numLines-1;
    }

    TQString str = d->od->lines[ LOGOFFSET(d->od->selEnd.line) ];
    d->od->selEnd.index = optimCharIndex( str, mousePos.x() );

    // have to have a valid index before generating a paint event
    if ( doScroll )
	ensureVisible( xx, yy, 1, 1 );

    // if the text document is smaller than the height of the viewport
    // - redraw the whole thing otherwise calculate the rect that
    // needs drawing.
    if ( d->od->numLines * fm.lineSpacing() < viewport()->height() ) {
	repaintContents( contentsX(), contentsY(), width(), height(), false );
    } else {
	int h = TQABS(mousePos.y() - oldMousePos.y()) + fm.lineSpacing() * 2;
	int y;
	if ( oldMousePos.y() < mousePos.y() ) {
	    y = oldMousePos.y() - fm.lineSpacing();
	} else {
	    // expand paint area for a fully selected line
	    h += fm.lineSpacing();
	    y = mousePos.y() - fm.lineSpacing()*2;
	}
	if ( y < 0 )
	    y = 0;
	repaintContents( contentsX(), y, width(), h, false );
    }

    if ( (!scrollTimer->isActive() && pos.y() < 0) || pos.y() > height() )
	scrollTimer->start( 100, false );
    else if ( scrollTimer->isActive() && pos.y() >= 0 && pos.y() <= height() )
	scrollTimer->stop();
}

/*! \internal

  Returns the index of the character in the string \a str that is
  currently under the mouse pointer.
*/
int TQTextEdit::optimCharIndex( const TQString &str, int mx ) const
{
    TQFontMetrics fm(TQScrollView::font());
    uint i = 0;
    int dd, dist = 10000000;
    int curpos = 0;
    int strWidth;
    mx = mx - 4; // ### get the real margin from somewhere

    if (!str.contains('\t') && mx > fm.width(str))
	return str.length();

    while (i < str.length()) {
	strWidth = qStrWidth(str.left(i), tabStopWidth(), fm);
	dd = strWidth - mx;
	if (TQABS(dd) <= dist) {
	    dist = TQABS(dd);
	    if (mx >= strWidth)
		curpos = i;
	}
	++i;
    }
    return curpos;
}

/*! \internal */
void TQTextEdit::optimSelectAll()
{
    d->od->selStart.line = d->od->selStart.index = 0;
    d->od->selEnd.line = d->od->numLines - 1;
    d->od->selEnd.index = d->od->lines[ LOGOFFSET(d->od->selEnd.line) ].length();

    repaintContents( false );
    emit copyAvailable( optimHasSelection() );
    emit selectionChanged();
}

/*! \internal */
void TQTextEdit::optimRemoveSelection()
{
    d->od->selStart.line = d->od->selEnd.line = -1;
    d->od->selStart.index = d->od->selEnd.index = -1;
    repaintContents( false );
}

/*! \internal */
void TQTextEdit::optimSetSelection( int startLine, int startIdx,
				       int endLine, int endIdx )
{
    d->od->selStart.line = startLine;
    d->od->selEnd.line = endLine;
    d->od->selStart.index = startIdx;
    d->od->selEnd.index = endIdx;
}

/*! \internal */
bool TQTextEdit::optimHasSelection() const
{
    return d->od->selStart.line != d->od->selEnd.line ||
	 d->od->selStart.index != d->od->selEnd.index;
}

/*! \internal */
TQString TQTextEdit::optimSelectedText() const
{
    TQString str;

    if ( !optimHasSelection() )
	return str;

    // concatenate all strings
    if ( d->od->selStart.line == d->od->selEnd.line ) {
	str = d->od->lines[ LOGOFFSET(d->od->selEnd.line) ].mid( d->od->selStart.index,
			   d->od->selEnd.index - d->od->selStart.index );
    } else {
	int i = d->od->selStart.line;
	str = d->od->lines[ LOGOFFSET(i) ].right( d->od->lines[ LOGOFFSET(i) ].length() -
				  d->od->selStart.index ) + "\n";
	i++;
	for ( ; i < d->od->selEnd.line; i++ ) {
	    if ( d->od->lines[ LOGOFFSET(i) ].isEmpty() ) // CR lines are empty
		str += "\n";
	    else
		str += d->od->lines[ LOGOFFSET(i) ] + "\n";
	}
	str += d->od->lines[ LOGOFFSET(d->od->selEnd.line) ].left( d->od->selEnd.index );
    }
    return str;
}

/*! \internal */
bool TQTextEdit::optimFind( const TQString & expr, bool cs, bool /*wo*/,
			       bool fw, int * para, int * index )
{
    bool found = false;
    int parag = para ? *para : d->od->search.line,
	  idx = index ? *index : d->od->search.index, i;

    if ( d->od->len == 0 )
	return false;

    for ( i = parag; fw ? i < d->od->numLines : i >= 0; fw ? i++ : i-- ) {
	idx = fw ? d->od->lines[ LOGOFFSET(i) ].find( expr, idx, cs ) :
	      d->od->lines[ LOGOFFSET(i) ].findRev( expr, idx, cs );
	if ( idx != -1 ) {
	    found = true;
	    break;
	} else if ( fw )
	    idx = 0;
    }

    if ( found ) {
	if ( index )
	    *index = idx;
	if ( para )
	    *para = i;
	d->od->search.index = idx + 1;
	d->od->search.line = i;
	optimSetSelection( i, idx, i, idx + expr.length() );
	TQFontMetrics fm( TQScrollView::font() );
	int h = fm.lineSpacing();
	int x = fm.width( d->od->lines[ LOGOFFSET(i) ].left( idx + expr.length()) ) + 4;
	ensureVisible( x, i * h + h / 2, 1, h / 2 + 2 );
	repaintContents(); // could possibly be optimized
    }
    return found;
}

/*! \reimp */
void TQTextEdit::polish()
{
    // this will ensure that the last line is visible if text have
    // been added to the widget before it is shown
    if ( d->optimMode )
	scrollToBottom();
    TQWidget::polish();
}

/*!
    Sets the maximum number of lines a TQTextEdit can hold in \c
    LogText mode to \a limit. If \a limit is -1 (the default), this
    signifies an unlimited number of lines.

    \warning Never use formatting tags that span more than one line
    when the maximum log lines is set. When lines are removed from the
    top of the buffer it could result in an unbalanced tag pair, i.e.
    the left formatting tag is removed before the right one.
 */
void TQTextEdit::setMaxLogLines( int limit )
{
    d->maxLogLines = limit;
    if ( d->maxLogLines < -1 )
	d->maxLogLines = -1;
    if ( d->maxLogLines == -1 )
	d->logOffset = 0;
}

/*!
    Returns the maximum number of lines TQTextEdit can hold in \c
    LogText mode. By default the number of lines is unlimited, which
    is signified by a value of -1.
 */
int TQTextEdit::maxLogLines()
{
    return d->maxLogLines;
}

/*!
    Check if the number of lines in the buffer is limited, and uphold
    that limit when appending new lines.
 */
void TQTextEdit::optimCheckLimit( const TQString& str )
{
    if ( d->maxLogLines > -1 && d->maxLogLines <= d->od->numLines ) {
	// NB! Removing the top line in the buffer will potentially
	// destroy the structure holding the formatting tags - if line
	// spanning tags are used.
	TQTextEditOptimPrivate::Tag *t = d->od->tags, *tmp, *itr;
	TQPtrList<TQTextEditOptimPrivate::Tag> lst;
	while ( t ) {
	    t->line -= 1;
	    // unhook the ptr from the tag structure
	    if ( ((uint) LOGOFFSET(t->line) < (uint) d->logOffset &&
		  (uint) LOGOFFSET(t->line) < (uint) LOGOFFSET(d->od->numLines) &&
		  (uint) LOGOFFSET(d->od->numLines) > (uint) d->logOffset) )
	    {
		if ( t->prev )
		    t->prev->next = t->next;
		if ( t->next )
		    t->next->prev = t->prev;
		if ( d->od->tags == t )
		    d->od->tags = t->next;
		if ( d->od->lastTag == t ) {
		    if ( t->prev )
			d->od->lastTag = t->prev;
		    else
			d->od->lastTag = d->od->tags;
		}
		tmp = t;
		t = t->next;
		lst.append( tmp );
		delete tmp;
	    } else {
		t = t->next;
	    }
	}
	// Remove all references to the ptrs we just deleted
	itr = d->od->tags;
	while ( itr ){
	    for ( tmp = lst.first(); tmp; tmp = lst.next() ) {
		if ( itr->parent == tmp )
		    itr->parent = 0;
		if ( itr->leftTag == tmp )
		    itr->leftTag = 0;
	    }
	    itr = itr->next;
	}
	// ...in the tag index as well
	TQMapIterator<int, TQTextEditOptimPrivate::Tag *> idx;
	if ( (idx = d->od->tagIndex.find( d->logOffset )) != d->od->tagIndex.end() )
	    d->od->tagIndex.remove( idx );

	TQMapIterator<int,TQString> it;
	if ( (it = d->od->lines.find( d->logOffset )) != d->od->lines.end() ) {
	    d->od->len -= (*it).length();
	    d->od->lines.remove( it );
	    d->od->numLines--;
	    d->logOffset = LOGOFFSET(1);
	}
    }
    d->od->len += str.length();
    d->od->lines[ LOGOFFSET(d->od->numLines++) ] = str;
}

#endif // QT_TEXTEDIT_OPTIMIZATION

/*!
    \property TQTextEdit::autoFormatting
    \brief the enabled set of auto formatting features

    The value can be any combination of the values in the \c
    AutoFormatting enum.  The default is \c AutoAll. Choose \c AutoNone
    to disable all automatic formatting.

    Currently, the only automatic formatting feature provided is \c
    AutoBulletList; future versions of TQt may offer more.
*/

void TQTextEdit::setAutoFormatting( uint features )
{
    d->autoFormatting = features;
}

uint TQTextEdit::autoFormatting() const
{
    return d->autoFormatting;
}

/*!
    Returns the TQSyntaxHighlighter set on this TQTextEdit. 0 is
    returned if no syntax highlighter is set.
 */
TQSyntaxHighlighter * TQTextEdit::syntaxHighlighter() const
{
    if (document()->preProcessor())
	return ((TQSyntaxHighlighterInternal *) document()->preProcessor())->highlighter;
    else
	return 0;
}

#endif //TQT_NO_TEXTEDIT
