/****************************************************************************
**
** Implementation of the TQSimpleRichText class
**
** Created : 990101
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

#include "ntqsimplerichtext.h"

#ifndef TQT_NO_RICHTEXT
#include "qrichtext_p.h"
#include "ntqapplication.h"

class TQSimpleRichTextData
{
public:
    TQTextDocument *doc;
    TQFont font;
    int cachedWidth;
    bool cachedWidthWithPainter;
    void adjustSize(TQPainter *p = 0);
};

// Pull this private function in from qglobal.cpp
extern unsigned int tqt_int_sqrt( unsigned int n );

void TQSimpleRichTextData::adjustSize(TQPainter *p) {
    TQFontMetrics fm( font );
    int mw =  fm.width( 'x' ) * 80;
    int w = mw;
    doc->doLayout(p, w);
    if ( doc->widthUsed() != 0 ) {
	w = tqt_int_sqrt( 5 * doc->height() * doc->widthUsed() / 3 );
	doc->doLayout(p, TQMIN(w, mw));

	if ( w*3 < 5*doc->height() ) {
	    w = tqt_int_sqrt( 2 * doc->height() * doc->widthUsed() );
	    doc->doLayout(p, TQMIN(w, mw));
	}
    }
    cachedWidth = doc->width();
    cachedWidthWithPainter = false;
}

/*!
    \class TQSimpleRichText ntqsimplerichtext.h
    \brief The TQSimpleRichText class provides a small displayable piece of rich text.

    \ingroup text
    \mainclass

    This class encapsulates simple rich text usage in which a string
    is interpreted as rich text and can be drawn. This is particularly
    useful if you want to display some rich text in a custom widget. A
    TQStyleSheet is needed to interpret the tags and format the rich
    text. TQt provides a default HTML-like style sheet, but you may
    define custom style sheets.

    Once created, the rich text object can be queried for its width(),
    height(), and the actual width used (see widthUsed()). Most
    importantly, it can be drawn on any given TQPainter with draw().
    TQSimpleRichText can also be used to implement hypertext or active
    text facilities by using anchorAt(). A hit test through inText()
    makes it possible to use simple rich text for text objects in
    editable drawing canvases.

    Once constructed from a string the contents cannot be changed,
    only resized. If the contents change, just throw the rich text
    object away and make a new one with the new contents.

    For large documents use TQTextEdit or TQTextBrowser. For very small
    items of rich text you can use a TQLabel.

    If you are using TQSimpleRichText to print in high resolution you
    should call setWidth(TQPainter, int) so that the content will be
    laid out properly on the page.
*/

/*!
    Constructs a TQSimpleRichText from the rich text string \a text and
    the font \a fnt.

    The font is used as a basis for the text rendering. When using
    rich text rendering on a widget \e w, you would normally specify
    the widget's font, for example:

    \code
    TQSimpleRichText myrichtext( contents, mywidget->font() );
    \endcode

    \a context is the optional context of the rich text object. This
    becomes important if \a text contains relative references, for
    example within image tags. TQSimpleRichText always uses the default
    mime source factory (see \l{TQMimeSourceFactory::defaultFactory()})
    to resolve those references. The context will then be used to
    calculate the absolute path. See
    TQMimeSourceFactory::makeAbsolute() for details.

    The \a sheet is an optional style sheet. If it is 0, the default
    style sheet will be used (see \l{TQStyleSheet::defaultSheet()}).
*/

TQSimpleRichText::TQSimpleRichText( const TQString& text, const TQFont& fnt,
				  const TQString& context, const TQStyleSheet* sheet )
{
    d = new TQSimpleRichTextData;
    d->cachedWidth = -1;
    d->cachedWidthWithPainter = false;
    d->font = fnt;
    d->doc = new TQTextDocument( 0 );
    d->doc->setTextFormat( TQt::RichText );
    d->doc->setLeftMargin( 0 );
    d->doc->setRightMargin( 0 );
    d->doc->setFormatter( new TQTextFormatterBreakWords );
    d->doc->setStyleSheet( (TQStyleSheet*)sheet );
    d->doc->setDefaultFormat( fnt, TQColor() );
    d->doc->setText( text, context );
}


/*!
    Constructs a TQSimpleRichText from the rich text string \a text and
    the font \a fnt.

    This is a slightly more complex constructor for TQSimpleRichText
    that takes an additional mime source factory \a factory, a page
    break parameter \a pageBreak and a bool \a linkUnderline. \a
    linkColor is only provided for compatibility, but has no effect,
    as TQColorGroup's TQColorGroup::link() color is used now.

    \a context is the optional context of the rich text object. This
    becomes important if \a text contains relative references, for
    example within image tags. TQSimpleRichText always uses the default
    mime source factory (see \l{TQMimeSourceFactory::defaultFactory()})
    to resolve those references. The context will then be used to
    calculate the absolute path. See
    TQMimeSourceFactory::makeAbsolute() for details.

    The \a sheet is an optional style sheet. If it is 0, the default
    style sheet will be used (see \l{TQStyleSheet::defaultSheet()}).

    This constructor is useful for creating a TQSimpleRichText object
    suitable for printing. Set \a pageBreak to be the height of the
    contents area of the pages.
*/

TQSimpleRichText::TQSimpleRichText( const TQString& text, const TQFont& fnt,
				  const TQString& context,  const TQStyleSheet* sheet,
				  const TQMimeSourceFactory* factory, int pageBreak,
				  const TQColor& /*linkColor*/, bool linkUnderline )
{
    d = new TQSimpleRichTextData;
    d->cachedWidth = -1;
    d->cachedWidthWithPainter = false;
    d->font = fnt;
    d->doc = new TQTextDocument( 0 );
    d->doc->setTextFormat( TQt::RichText );
    d->doc->setFormatter( new TQTextFormatterBreakWords );
    d->doc->setStyleSheet( (TQStyleSheet*)sheet );
    d->doc->setDefaultFormat( fnt, TQColor() );
    d->doc->flow()->setPageSize( pageBreak );
    d->doc->setPageBreakEnabled( true );
#ifndef TQT_NO_MIME
    d->doc->setMimeSourceFactory( (TQMimeSourceFactory*)factory );
#endif
    d->doc->setUnderlineLinks( linkUnderline );
    d->doc->setText( text, context );
}

/*!
    Destroys the rich text object, freeing memory.
*/

TQSimpleRichText::~TQSimpleRichText()
{
    delete d->doc;
    delete d;
}

/*!
    \overload

    Sets the width of the rich text object to \a w pixels.

    \sa height(), adjustSize()
*/

void TQSimpleRichText::setWidth( int w )
{
    if ( w == d->cachedWidth && !d->cachedWidthWithPainter )
	return;
    d->doc->formatter()->setAllowBreakInWords( d->doc->isPageBreakEnabled() );
    d->cachedWidth = w;
    d->cachedWidthWithPainter = false;
    d->doc->doLayout( 0, w );
}

/*!
    Sets the width of the rich text object to \a w pixels,
    recalculating the layout as if it were to be drawn with painter \a
    p.

    Passing a painter is useful when you intend drawing on devices
    other than the screen, for example a TQPrinter.

    \sa height(), adjustSize()
*/

void TQSimpleRichText::setWidth( TQPainter *p, int w )
{
    if ( w == d->cachedWidth  && d->cachedWidthWithPainter )
	return;
    d->doc->formatter()->setAllowBreakInWords( d->doc->isPageBreakEnabled() ||
		       (p && p->device() &&
		     (p->device()->devType() == TQInternal::Printer)) );
    p->save();
    d->cachedWidth = w;
    d->cachedWidthWithPainter = true;
    d->doc->doLayout( p, w );
    p->restore();
}

/*!
    Returns the set width of the rich text object in pixels.

    \sa widthUsed()
*/

int TQSimpleRichText::width() const
{
    if ( d->cachedWidth < 0 )
	d->adjustSize();
    return d->doc->width();
}

/*!
    Returns the width in pixels that is actually used by the rich text
    object. This can be smaller or wider than the set width.

    It may be wider, for example, if the text contains images or
    non-breakable words that are already wider than the available
    space. It's smaller when the object only consists of lines that do
    not fill the width completely.

    \sa width()
*/

int TQSimpleRichText::widthUsed() const
{
    if ( d->cachedWidth < 0 )
	d->adjustSize();
    return d->doc->widthUsed();
}

/*!
    Returns the height of the rich text object in pixels.

    \sa setWidth()
*/

int TQSimpleRichText::height() const
{
    if ( d->cachedWidth < 0 )
	d->adjustSize();
    return d->doc->height();
}

/*!
    Adjusts the richt text object to a reasonable size.

    \sa setWidth()
*/

void TQSimpleRichText::adjustSize()
{
    d->adjustSize();
}

/*!
    Draws the formatted text with painter \a p, at position (\a x, \a
    y), clipped to \a clipRect. The clipping rectangle is given in the
    rich text object's coordinates translated by (\a x, \a y). Passing
    an null rectangle results in no clipping. Colors from the color
    group \a cg are used as needed, and if not 0, \a *paper is used as
    the background brush.

    Note that the display code is highly optimized to reduce flicker,
    so passing a brush for \a paper is preferable to simply clearing
    the area to be painted and then calling this without a brush.
*/

void TQSimpleRichText::draw( TQPainter *p,  int x, int y, const TQRect& clipRect,
			    const TQColorGroup& cg, const TQBrush* paper ) const
{
    p->save();
    if ( d->cachedWidth < 0 )
	d->adjustSize(p);

    TQRect r = clipRect;
    if ( !r.isNull() )
	r.moveBy( -x, -y );

    if ( paper )
	d->doc->setPaper( new TQBrush( *paper ) );
    TQColorGroup g = cg;
    if ( d->doc->paper() )
	g.setBrush( TQColorGroup::Base, *d->doc->paper() );

    if ( !clipRect.isNull() )
	p->setClipRect( clipRect, TQPainter::CoordPainter );
    p->translate( x, y );
    d->doc->draw( p, r, g, paper );
    p->translate( -x, -y );
    p->restore();
}


/*! \fn void TQSimpleRichText::draw( TQPainter *p,  int x, int y, const TQRegion& clipRegion,
  const TQColorGroup& cg, const TQBrush* paper ) const

  \obsolete

  Use the version with clipRect instead. The region version has
  problems with larger documents on some platforms (on X11 regions
  internally are represented with 16bit coordinates).
*/



/*!
    Returns the context of the rich text object. If no context has
    been specified in the constructor, a null string is returned. The
    context is the path to use to look up relative links, such as
    image tags and anchor references.
*/

TQString TQSimpleRichText::context() const
{
    return d->doc->context();
}

/*!
    Returns the anchor at the requested position, \a pos. An empty
    string is returned if no anchor is specified for this position.
*/

TQString TQSimpleRichText::anchorAt( const TQPoint& pos ) const
{
    if ( d->cachedWidth < 0 )
	d->adjustSize();
    TQTextCursor c( d->doc );
    c.place( pos, d->doc->firstParagraph(), true );
    return c.paragraph()->at( c.index() )->anchorHref();
}

/*!
    Returns true if \a pos is within a text line of the rich text
    object; otherwise returns false.
*/

bool TQSimpleRichText::inText( const TQPoint& pos ) const
{
    if ( d->cachedWidth < 0 )
	d->adjustSize();
    if ( pos.y()  > d->doc->height() )
	return false;
    TQTextCursor c( d->doc );
    c.place( pos, d->doc->firstParagraph() );
    return c.totalOffsetX() + c.paragraph()->at( c.index() )->x +
	c.paragraph()->at( c.index() )->format()->width( c.paragraph()->at( c.index() )->c ) > pos.x();
}

/*!
    Sets the default font for the rich text object to \a f
*/

void TQSimpleRichText::setDefaultFont( const TQFont &f )
{
    if ( d->font == f )
	return;
    d->font = f;
    d->cachedWidth = -1;
    d->cachedWidthWithPainter = false;
    d->doc->setDefaultFormat( f, TQColor() );
    d->doc->setText( d->doc->originalText(), d->doc->context() );
}

#endif //TQT_NO_RICHTEXT
