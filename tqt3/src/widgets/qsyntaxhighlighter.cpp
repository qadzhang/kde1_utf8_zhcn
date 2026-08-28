/****************************************************************************
**
** Implementation of the TQSyntaxHighlighter class
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

#include "ntqsyntaxhighlighter.h"
#include "private/qsyntaxhighlighter_p.h"

#ifndef TQT_NO_SYNTAXHIGHLIGHTER
#include "../kernel/qrichtext_p.h"
#include "ntqtextedit.h"
#include "ntqtimer.h"

/*!
    \class TQSyntaxHighlighter ntqsyntaxhighlighter.h
    \brief The TQSyntaxHighlighter class is a base class for
    implementing TQTextEdit syntax highlighters.

    \ingroup basic
    \ingroup text

    A syntax highligher automatically highlights parts of the text in
    a TQTextEdit. Syntax highlighters are often used when the user is
    entering text in a specific format (for example, source code) and
    help the user to read the text and identify syntax errors.

    To provide your own syntax highlighting for TQTextEdit, you must
    subclass TQSyntaxHighlighter and reimplement highlightParagraph().

    When you create an instance of your TQSyntaxHighlighter subclass,
    pass it the TQTextEdit that you want the syntax highlighting to be
    applied to. After this your highlightParagraph() function will be
    called automatically whenever necessary. Use your
    highlightParagraph() function to apply formatting (e.g. setting
    the font and color) to the text that is passed to it.
*/

/*!
    Constructs the TQSyntaxHighlighter and installs it on \a textEdit.

    It is the caller's responsibility to delete the
    TQSyntaxHighlighter when it is no longer needed.
*/

TQSyntaxHighlighter::TQSyntaxHighlighter( TQTextEdit *textEdit )
    : para( 0 ), edit( textEdit ), d( new TQSyntaxHighlighterPrivate )
{
    textEdit->document()->setPreProcessor( new TQSyntaxHighlighterInternal( this ) );
    textEdit->document()->invalidate();
    TQTimer::singleShot( 0, textEdit->viewport(), TQ_SLOT( update() ) );
}

/*!
    Destructor. Uninstalls this syntax highlighter from the textEdit()
*/

TQSyntaxHighlighter::~TQSyntaxHighlighter()
{
    delete d;
    textEdit()->document()->setPreProcessor( 0 );
}

/*!
    \fn int TQSyntaxHighlighter::highlightParagraph( const TQString &text, int endStateOfLastPara )

    This function is called when necessary by the rich text engine,
    i.e. on paragraphs which have changed.

    In your reimplementation you should parse the paragraph's \a text
    and call setFormat() as often as necessary to apply any font and
    color changes that you require. Your function must return a value
    which indicates the paragraph's end state: see below.

    Some syntaxes can have constructs that span paragraphs. For
    example, a C++ syntax highlighter should be able to cope with
    \c{/}\c{*...*}\c{/} comments that span paragraphs. To deal
    with these cases it is necessary to know the end state of the
    previous paragraph (e.g. "in comment").

    If your syntax does not have paragraph spanning constructs, simply
    ignore the \a endStateOfLastPara parameter and always return 0.

    Whenever highlightParagraph() is called it is passed a value for
    \a endStateOfLastPara. For the very first paragraph this value is
    always -2. For any other paragraph the value is the value returned
    by the most recent highlightParagraph() call that applied to the
    preceding paragraph.

    The value you return is up to you. We recommend only returning 0
    (to signify that this paragraph's syntax highlighting does not
    affect the following paragraph), or a positive integer (to signify
    that this paragraph has ended in the middle of a paragraph
    spanning construct).

    To find out which paragraph is highlighted, call
    currentParagraph().

    For example, if you're writing a simple C++ syntax highlighter,
    you might designate 1 to signify "in comment". For a paragraph
    that ended in the middle of a comment you'd return 1, and for
    other paragraphs you'd return 0. In your parsing code if \a
    endStateOfLastPara was 1, you would highlight the text as a C++
    comment until you reached the closing \c{*}\c{/}.
*/

/*!
    This function is applied to the syntax highlighter's current
    paragraph (the text of which is passed to the highlightParagraph()
    function).

    The specified \a font and \a color are applied to the text from
    position \a start for \a count characters. (If \a count is 0,
    nothing is done.)
*/

void TQSyntaxHighlighter::setFormat( int start, int count, const TQFont &font, const TQColor &color )
{
    if ( !para || count <= 0 )
	return;
    TQTextFormat *f = 0;
    f = para->document()->formatCollection()->format( font, color );
    para->setFormat( start, count, f );
    f->removeRef();
}

/*! \overload */

void TQSyntaxHighlighter::setFormat( int start, int count, const TQColor &color )
{
    if ( !para || count <= 0 )
	return;
    TQTextFormat *f = 0;
    TQFont fnt = textEdit()->TQWidget::font();
    f = para->document()->formatCollection()->format( fnt, color );
    para->setFormat( start, count, f );
    f->removeRef();
}

/*! \overload */

void TQSyntaxHighlighter::setFormat( int start, int count, const TQFont &font )
{
    if ( !para || count <= 0 )
	return;
    TQTextFormat *f = 0;
    TQColor c = textEdit()->viewport()->paletteForegroundColor();
    f = para->document()->formatCollection()->format( font, c );
    para->setFormat( start, count, f );
    f->removeRef();
}

/*!
    \fn TQTextEdit *TQSyntaxHighlighter::textEdit() const

    Returns the TQTextEdit on which this syntax highlighter is
    installed
*/

/*!  Redoes the highlighting of the whole document.
*/

void TQSyntaxHighlighter::rehighlight()
{
    TQTextParagraph *s = edit->document()->firstParagraph();
    while ( s ) {
	s->invalidate( 0 );
	s->state = -1;
	s->needPreProcess = true;
	s = s->next();
    }
    edit->repaintContents( false );
}

/*!
    Returns the id of the paragraph which is highlighted, or -1 of no
    paragraph is currently highlighted.

    Usually this function is called from within highlightParagraph().
*/

int TQSyntaxHighlighter::currentParagraph() const
{
    return d->currentParagraph;
}

#endif
