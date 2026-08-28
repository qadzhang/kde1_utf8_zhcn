/**********************************************************************
**
** Implementation of TQLabel widget class
**
** Created : 941215
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

#include "ntqlabel.h"
#ifndef TQT_NO_LABEL
#include "ntqpainter.h"
#include "ntqdrawutil.h"
#include "ntqaccel.h"
#include "ntqmovie.h"
#include "ntqimage.h"
#include "ntqbitmap.h"
#include "ntqpicture.h"
#include "ntqapplication.h"
#include "ntqsimplerichtext.h"
#include "ntqstylesheet.h"
#include "ntqstyle.h"

class TQLabelPrivate
{
public:
    TQLabelPrivate()
	:img(0), pix(0), valid_hints( -1 )
    {}
    TQImage* img; // for scaled contents
    TQPixmap* pix; // for scaled contents
    TQSize sh;
    TQSize msh;
    int valid_hints; // stores the frameWidth() for the stored size hint, -1 otherwise
};


/*!
    \class TQLabel ntqlabel.h
    \brief The TQLabel widget provides a text or image display.

    \ingroup basic
    \ingroup text
    \mainclass

    TQLabel is used for displaying text or an image. No user
    interaction functionality is provided. The visual appearance of
    the label can be configured in various ways, and it can be used
    for specifying a focus accelerator key for another widget.

    A TQLabel can contain any of the following content types:
    \table
    \header \i Content \i Setting
    \row \i Plain text
	 \i Pass a TQString to setText().
    \row \i Rich text
	 \i Pass a TQString that contains rich text to setText().
    \row \i A pixmap
	 \i Pass a TQPixmap to setPixmap().
    \row \i A movie
	 \i Pass a TQMovie to setMovie().
    \row \i A number
	 \i Pass an \e int or a \e double to setNum(), which converts
	    the number to plain text.
    \row \i Nothing
	 \i The same as an empty plain text. This is the default. Set
	    by clear().
    \endtable

    When the content is changed using any of these functions, any
    previous content is cleared.

    The look of a TQLabel can be tuned in several ways. All the
    settings of TQFrame are available for specifying a widget frame.
    The positioning of the content within the TQLabel widget area can
    be tuned with setAlignment() and setIndent(). For example, this
    code sets up a sunken panel with a two-line text in the bottom
    right corner (both lines being flush with the right side of the
    label):
    \code
    TQLabel *label = new TQLabel( this );
    label->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );
    label->setText( "first line\nsecond line" );
    label->setAlignment( AlignBottom | AlignRight );
    \endcode

    A TQLabel is often used as a label for an interactive widget. For
    this use TQLabel provides a useful mechanism for adding an
    accelerator key (see TQAccel) that will set the keyboard focus to
    the other widget (called the TQLabel's "buddy"). For example:
    \code
    TQLineEdit* phoneEdit = new TQLineEdit( this, "phoneEdit" );
    TQLabel* phoneLabel = new TQLabel( phoneEdit, "&Phone:", this, "phoneLabel" );
    \endcode

    In this example, keyboard focus is transferred to the label's
    buddy (the TQLineEdit) when the user presses Alt+P. You can
    also use the setBuddy() function to accomplish the same thing.

    <img src=qlabel-m.png> <img src=qlabel-w.png>

    \sa TQLineEdit, TQTextEdit, TQPixmap, TQMovie,
    \link guibooks.html#fowler GUI Design Handbook: Label\endlink
*/

/*!
    \fn TQPicture * TQLabel::picture() const

    Returns the label's picture or 0 if the label doesn't have a
    picture.
*/


/*!
    Constructs an empty label.

    The \a parent, \a name and widget flag \a f, arguments are passed
    to the TQFrame constructor.

    \sa setAlignment(), setFrameStyle(), setIndent()
*/

TQLabel::TQLabel( TQWidget *parent, const char *name, WFlags f )
    : TQFrame( parent, name, f | WMouseNoMask  )
{
    init();
}


/*!
    Constructs a label that displays the text, \a text.

    The \a parent, \a name and widget flag \a f, arguments are passed
    to the TQFrame constructor.

    \sa setText(), setAlignment(), setFrameStyle(), setIndent()
*/

TQLabel::TQLabel( const TQString &text, TQWidget *parent, const char *name,
		WFlags f )
	: TQFrame( parent, name, f | WMouseNoMask  )
{
    init();
    setText( text );
}


/*!
    Constructs a label that displays the text \a text. The label has a
    buddy widget, \a buddy.

    If the \a text contains an underlined letter (a letter preceded by
    an ampersand, \&), and the text is in plain text format, when the
    user presses Alt+ the underlined letter, focus is passed to the
    buddy widget.

    The \a parent, \a name and widget flag, \a f, arguments are passed
    to the TQFrame constructor.

    \sa setText(), setBuddy(), setAlignment(), setFrameStyle(),
    setIndent()
*/
TQLabel::TQLabel( TQWidget *buddy,  const TQString &text,
		TQWidget *parent, const char *name, WFlags f )
    : TQFrame( parent, name, f | WMouseNoMask )
{
    init();
#ifndef TQT_NO_ACCEL
    setBuddy( buddy );
#endif
    setText( text );
}

/*!
    Destroys the label.
*/

TQLabel::~TQLabel()
{
    clearContents();
    delete d;
}


void TQLabel::init()
{
    lpixmap = 0;
#ifndef TQT_NO_MOVIE
    lmovie = 0;
#endif
#ifndef TQT_NO_ACCEL
    lbuddy = 0;
    accel = 0;
#endif
    lpixmap = 0;
#ifndef TQT_NO_PICTURE
    lpicture = 0;
#endif
    align = AlignAuto | AlignVCenter | ExpandTabs;
    extraMargin = -1;
    autoresize = false;
    scaledcontents = false;
    textformat = TQt::AutoText;
#ifndef TQT_NO_RICHTEXT
    doc = 0;
#endif

    setSizePolicy( TQSizePolicy( TQSizePolicy::Preferred, TQSizePolicy::Preferred ) );
    d = new TQLabelPrivate;
}


/*!
    \property TQLabel::text
    \brief the label's text

    If no text has been set this will return an empty string. Setting
    the text clears any previous content, unless they are the same.

    The text will be interpreted either as a plain text or as a rich
    text, depending on the text format setting; see setTextFormat().
    The default setting is \c AutoText, i.e. TQLabel will try to
    auto-detect the format of the text set.

    If the text is interpreted as a plain text and a buddy has been
    set, the buddy accelerator key is updated from the new text.

    The label resizes itself if auto-resizing is enabled.

    Note that Qlabel is well-suited to display small rich text
    documents, i.e. those small documents that get their document
    specific settings (font, text color, link color) from the label's
    palette and font properties. For large documents, use TQTextEdit
    in read-only mode instead. TQTextEdit will flicker less on resize
    and can also provide a scrollbar when necessary.

    \sa text, setTextFormat(), setBuddy(), alignment
*/

void TQLabel::setText( const TQString &text )
{
    if ( ltext == text )
	return;
    TQSize osh = sizeHint();
#ifndef TQT_NO_RICHTEXT
    bool hadRichtext = doc != 0;
#endif
    clearContents();
    ltext = text;
#ifndef TQT_NO_RICHTEXT
    bool useRichText = (textformat == RichText ||
      ( ( textformat == AutoText ) && TQStyleSheet::mightBeRichText(ltext) ) );
#else
    bool useRichText = true;
#endif
#ifndef TQT_NO_ACCEL
    // ### Setting accelerators for rich text labels will not work.
    // Eg. <b>&gt;Hello</b> will return ALT+G which is clearly
    // not intended.
    if ( !useRichText ) {
	int p = TQAccel::shortcutKey( ltext );
	if ( p ) {
	    if ( !accel )
		accel = new TQAccel( this, "accel label accel" );
	    accel->connectItem( accel->insertItem( p ),
				this, TQ_SLOT(acceleratorSlot()) );
	}
    }
#endif
#ifndef TQT_NO_RICHTEXT
    if ( useRichText ) {
	if ( !hadRichtext )
	    align |= WordBreak;
	TQString t = ltext;
	if ( align & AlignRight )
	    t.prepend( "<div align=\"right\">");
	else if ( align & AlignHCenter )
	    t.prepend( "<div align=\"center\">");
	if ( (align & WordBreak) == 0  )
	    t.prepend( "<nobr>" );
	doc = new TQSimpleRichText( t, font() );
    }
#endif

    updateLabel( osh );
}


/*!
    Clears any label contents. Equivalent to setText( "" ).
*/

void TQLabel::clear()
{
    setText( TQString::fromLatin1("") );
}

/*!
    \property TQLabel::pixmap
    \brief the label's pixmap

    If no pixmap has been set this will return an invalid pixmap.

    Setting the pixmap clears any previous content, and resizes the
    label if \l TQLabel::autoResize() is true. The buddy accelerator,
    if any, is disabled.
*/
void TQLabel::setPixmap( const TQPixmap &pixmap )
{
    TQSize osh = sizeHint();

    if ( !lpixmap || lpixmap->serialNumber() != pixmap.serialNumber() ) {
	clearContents();
	lpixmap = new TQPixmap( pixmap );
    }

    if ( lpixmap->depth() == 1 && !lpixmap->mask() )
	lpixmap->setMask( *((TQBitmap *)lpixmap) );

    updateLabel( osh );
}

#ifndef TQT_NO_PICTURE
/*!
    Sets the label contents to \a picture. Any previous content is
    cleared.

    The buddy accelerator, if any, is disabled.

    \sa picture(), setBuddy()
*/

void TQLabel::setPicture( const TQPicture &picture )
{
    TQSize osh = sizeHint();
    clearContents();
    lpicture = new TQPicture( picture );

    updateLabel( osh );
}
#endif // TQT_NO_PICTURE

/*!
    Sets the label contents to plain text containing the textual
    representation of integer \a num. Any previous content is cleared.
    Does nothing if the integer's string representation is the same as
    the current contents of the label.

    The buddy accelerator, if any, is disabled.

    The label resizes itself if auto-resizing is enabled.

    \sa setText(), TQString::setNum(), setBuddy()
*/

void TQLabel::setNum( int num )
{
    TQString str;
    str.setNum( num );
	setText( str );
}

/*!
    \overload

    Sets the label contents to plain text containing the textual
    representation of double \a num. Any previous content is cleared.
    Does nothing if the double's string representation is the same as
    the current contents of the label.

    The buddy accelerator, if any, is disabled.

    The label resizes itself if auto-resizing is enabled.

    \sa setText(), TQString::setNum(), setBuddy()
*/

void TQLabel::setNum( double num )
{
    TQString str;
    str.setNum( num );
	setText( str );
}

/*!
    \property TQLabel::alignment
    \brief the alignment of the label's contents

    The alignment is a bitwise OR of \c TQt::AlignmentFlags and \c
    TQt::TextFlags values. The \c ExpandTabs, \c SingleLine and \c
    ShowPrefix flags apply only if the label contains plain text;
    otherwise they are ignored. The \c DontClip flag is always
    ignored. \c WordBreak applies to both rich text and plain text
    labels. The \c BreakAnywhere flag is not supported in TQLabel.

    If the label has a buddy, the \c ShowPrefix flag is forced to
    true.

    The default alignment is \c{AlignAuto | AlignVCenter | ExpandTabs}
    if the label doesn't have a buddy and \c{AlignAuto | AlignVCenter
    | ExpandTabs | ShowPrefix} if the label has a buddy. If the label
    contains rich text, additionally \c WordBreak is turned on.

    \sa TQt::AlignmentFlags, alignment, setBuddy(), text
*/

void TQLabel::setAlignment( int alignment )
{
    if ( alignment == align )
	return;
    TQSize osh = sizeHint();
#ifndef TQT_NO_ACCEL
    if ( lbuddy )
	align = alignment | ShowPrefix;
    else
#endif
	align = alignment;

#ifndef TQT_NO_RICHTEXT
    TQString t = ltext;
    if ( !t.isNull() ) {
	ltext = TQString::null;
	setText( t );
    }
#endif

    updateLabel( osh );
}


/*!
    \property TQLabel::indent
    \brief the label's text indent in pixels

    If a label displays text, the indent applies to the left edge if
    alignment() is \c AlignLeft, to the right edge if alignment() is
    \c AlignRight, to the top edge if alignment() is \c AlignTop, and
    to to the bottom edge if alignment() is \c AlignBottom.

    If indent is negative, or if no indent has been set, the label
    computes the effective indent as follows: If frameWidth() is 0,
    the effective indent becomes 0. If frameWidth() is greater than 0,
    the effective indent becomes half the width of the "x" character
    of the widget's current font().

    \sa alignment, frameWidth(), font()
*/

void TQLabel::setIndent( int indent )
{
    extraMargin = indent;
    updateLabel( TQSize( -1, -1 ) );
}


/*!
  \fn bool TQLabel::autoResize() const

  \obsolete

  Returns true if auto-resizing is enabled, or false if auto-resizing
  is disabled.

  Auto-resizing is disabled by default.

  \sa setAutoResize()
*/

/*! \obsolete
  Enables auto-resizing if \a enable is true, or disables it if \a
  enable is false.

  When auto-resizing is enabled the label will resize itself to fit
  the contents whenever the contents change. The top-left corner is
  not moved. This is useful for TQLabel widgets that are not managed by
  a TQLayout (e.g., top-level widgets).

  Auto-resizing is disabled by default.

  \sa autoResize(), adjustSize(), sizeHint()
*/

void TQLabel::setAutoResize( bool enable )
{
    if ( (bool)autoresize != enable ) {
	autoresize = enable;
	if ( autoresize )
	    adjustSize();			// calls resize which repaints
    }
}



/*!
    Returns the size that will be used if the width of the label is \a
    w. If \a w is -1, the sizeHint() is returned.
*/

TQSize TQLabel::sizeForWidth( int w ) const
{
    TQRect br;
    TQPixmap *pix = pixmap();
#ifndef TQT_NO_PICTURE
    TQPicture *pic = picture();
#else
    const int pic = 0;
#endif
#ifndef TQT_NO_MOVIE
    TQMovie *mov = movie();
#else
    const int mov = 0;
#endif
    int hextra = 2 * frameWidth();
    int vextra = hextra;
    TQFontMetrics fm( fontMetrics() );
    int xw = fm.width( 'x' );
    if ( !mov && !pix && !pic ) {
	int m = indent();
	if ( m < 0 && hextra ) // no indent, but we do have a frame
	    m = xw / 2 - margin();
	if ( m >= 0 ) {
	    int horizAlign = TQApplication::horizontalAlignment( align );
	    if ( (horizAlign & AlignLeft) || (horizAlign & AlignRight ) )
		hextra += m;
	    if ( (align & AlignTop) || (align & AlignBottom ) )
		vextra += m;
	}
    }

    if ( pix )
	br = pix->rect();
#ifndef TQT_NO_PICTURE
    else if ( pic )
	br = pic->boundingRect();
#endif
#ifndef TQT_NO_MOVIE
    else if ( mov )
	br = mov->framePixmap().rect();
#endif
#ifndef TQT_NO_RICHTEXT
    else if ( doc ) {
	int oldW = doc->width();
	if ( align & WordBreak ) {
	    if ( w < 0 )
		doc->adjustSize();
	    else
		doc->setWidth( w-hextra );
	}
	br = TQRect( 0, 0, doc->widthUsed(), doc->height() );
	doc->setWidth( oldW );
    }
#endif
    else {
	bool tryWidth = (w < 0) && (align & WordBreak);
	if ( tryWidth )
	    w = xw * 80;
	else if ( w < 0 )
	    w = 2000;
	w -= hextra;
	br = fm.boundingRect( 0, 0, w ,2000, alignment(), text() );
	if ( tryWidth && br.height() < 4*fm.lineSpacing() && br.width() > w/2 )
		br = fm.boundingRect( 0, 0, w/2, 2000, alignment(), text() );
	if ( tryWidth && br.height() < 2*fm.lineSpacing() && br.width() > w/4 )
	    br = fm.boundingRect( 0, 0, w/4, 2000, alignment(), text() );
    }
    int wid = br.width() + hextra;
    int hei = br.height() + vextra;

    return TQSize( wid, hei );
}


/*!
  \reimp
*/

int TQLabel::heightForWidth( int w ) const
{
    if (
#ifndef TQT_NO_RICHTEXT
	doc ||
#endif
	(align & WordBreak) )
	return sizeForWidth( w ).height();
    return TQWidget::heightForWidth( w );
}



/*!\reimp
*/
TQSize TQLabel::sizeHint() const
{
    if ( d->valid_hints != frameWidth() )
	(void) TQLabel::minimumSizeHint();
    return d->sh;
}

/*!
  \reimp
*/

TQSize TQLabel::minimumSizeHint() const
{
    if ( d->valid_hints == frameWidth() )
	return d->msh;

    constPolish();
    d->valid_hints = frameWidth();
    d->sh = sizeForWidth( -1 );
    TQSize sz( -1, -1 );

    if (
#ifndef TQT_NO_RICHTEXT
	 !doc &&
#endif
	 (align & WordBreak) == 0 ) {
	sz = d->sh;
    } else {
	// think about caching these for performance
	sz.rwidth() = sizeForWidth( 0 ).width();
	sz.rheight() = sizeForWidth(TQWIDGETSIZE_MAX).height();
	if ( d->sh.height() < sz.height() )
	    sz.rheight() = d->sh.height();
    }
    if ( sizePolicy().horData() == TQSizePolicy::Ignored )
	sz.rwidth() = -1;
    if ( sizePolicy().verData() == TQSizePolicy::Ignored )
	sz.rheight() = -1;
    d->msh = sz;
    return sz;
}

/*!
  \reimp
*/
void TQLabel::resizeEvent( TQResizeEvent* e )
{
    TQFrame::resizeEvent( e );

#ifdef TQT_NO_RICHTEXT
    static const bool doc = false;
#endif

    // optimize for standard labels
    if ( frameShape() == NoFrame && (align & WordBreak) == 0 && !doc &&
	 ( e->oldSize().width() >= e->size().width() && (align & AlignLeft ) == AlignLeft )
	 && ( e->oldSize().height() >= e->size().height() && (align & AlignTop ) == AlignTop ) ) {
	setWFlags( WResizeNoErase );
	return;
    }

    clearWFlags( WResizeNoErase );
    TQRect cr = contentsRect();
    if ( !lpixmap ||  !cr.isValid() ||
	 // masked pixmaps can only reduce flicker when being top/left
	 // aligned and when we do not perform scaled contents
	 ( lpixmap->hasAlpha() && ( scaledcontents || ( ( align & (AlignLeft|AlignTop) ) != (AlignLeft|AlignTop) ) ) ) )
	return;

    setWFlags( WResizeNoErase );

    if ( !scaledcontents ) {
	// don't we all love TQFrame? Reduce pixmap flicker
	TQRegion reg = TQRect( TQPoint(0, 0), e->size() );
	reg = reg.subtract( cr );
	int x = cr.x();
	int y = cr.y();
	int w = lpixmap->width();
	int h = lpixmap->height();
	if ( (align & TQt::AlignVCenter) == TQt::AlignVCenter )
	    y += cr.height()/2 - h/2;
	else if ( (align & TQt::AlignBottom) == TQt::AlignBottom)
	    y += cr.height() - h;
	if ( (align & TQt::AlignRight) == TQt::AlignRight )
	    x += cr.width() - w;
	else if ( (align & TQt::AlignHCenter) == TQt::AlignHCenter )
	    x += cr.width()/2 - w/2;
	if ( x > cr.x() )
	    reg = reg.unite( TQRect( cr.x(), cr.y(), x - cr.x(), cr.height() ) );
	if ( y > cr.y() )
	    reg = reg.unite( TQRect( cr.x(), cr.y(), cr.width(), y - cr.y() ) );

	if ( x + w < cr.right() )
	    reg = reg.unite( TQRect( x + w, cr.y(),  cr.right() - x - w, cr.height() ) );
	if ( y + h < cr.bottom() )
	    reg = reg.unite( TQRect( cr.x(), y +  h, cr.width(), cr.bottom() - y - h ) );

	erase( reg );
    }
}


/*!
    Draws the label contents using the painter \a p.
*/

void TQLabel::drawContents( TQPainter *p )
{
    TQRect cr = contentsRect();

    TQPixmap *pix = pixmap();
#ifndef TQT_NO_PICTURE
    TQPicture *pic = picture();
#else
    const int pic = 0;
#endif
#ifndef TQT_NO_MOVIE
    TQMovie *mov = movie();
#else
    const int mov = 0;
#endif

    if ( !mov && !pix && !pic ) {
	int m = indent();
	if ( m < 0 && frameWidth() ) // no indent, but we do have a frame
	    m = fontMetrics().width('x') / 2 - margin();
	if ( m > 0 ) {
	    int hAlign = TQApplication::horizontalAlignment( align );
	    if ( hAlign & AlignLeft )
		cr.setLeft( cr.left() + m );
	    if ( hAlign & AlignRight )
		cr.setRight( cr.right() - m );
	    if ( align & AlignTop )
		cr.setTop( cr.top() + m );
	    if ( align & AlignBottom )
		cr.setBottom( cr.bottom() - m );
	}
    }

#ifndef TQT_NO_MOVIE
    if ( mov ) {
	// ### should add movie to qDrawItem
	TQRect r = style().itemRect( p, cr, align, isEnabled(), &(mov->framePixmap()),
				    TQString::null );
	// ### could resize movie frame at this point
	p->drawPixmap(r.x(), r.y(), mov->framePixmap() );
    }
    else
#endif
#ifndef TQT_NO_RICHTEXT
    if ( doc ) {
	doc->setWidth(p, cr.width() );
	int rh = doc->height();
	int yo = 0;
	if ( align & AlignVCenter )
	    yo = (cr.height()-rh)/2;
	else if ( align & AlignBottom )
	    yo = cr.height()-rh;
	if (! isEnabled() &&
	    style().styleHint(TQStyle::SH_EtchDisabledText, this)) {
	    TQColorGroup cg = colorGroup();
	    cg.setColor( TQColorGroup::Text, cg.light() );
	    doc->draw(p, cr.x()+1, cr.y()+yo+1, cr, cg, 0);
	}

	// TQSimpleRichText always draws with TQColorGroup::Text as with
	// background mode PaletteBase. TQLabel typically has
	// background mode PaletteBackground, so we create a temporary
	// color group with the text color adjusted.
	TQColorGroup cg = colorGroup();
	if ( backgroundMode() != PaletteBase && isEnabled() )
	    cg.setColor( TQColorGroup::Text, paletteForegroundColor() );

	doc->draw(p, cr.x(), cr.y()+yo, cr, cg, 0);
    } else
#endif
#ifndef TQT_NO_PICTURE
    if ( pic ) {
	TQRect br = pic->boundingRect();
	int rw = br.width();
	int rh = br.height();
	if ( scaledcontents ) {
	    p->save();
	    p->translate( cr.x(), cr.y() );
#ifndef TQT_NO_TRANSFORMATIONS
	    p->scale( (double)cr.width()/rw, (double)cr.height()/rh );
#endif
	    p->drawPicture( -br.x(), -br.y(), *pic );
	    p->restore();
	} else {
	    int xo = 0;
	    int yo = 0;
	    if ( align & AlignVCenter )
		yo = (cr.height()-rh)/2;
	    else if ( align & AlignBottom )
		yo = cr.height()-rh;
	    if ( align & AlignRight )
		xo = cr.width()-rw;
	    else if ( align & AlignHCenter )
		xo = (cr.width()-rw)/2;
	    p->drawPicture( cr.x()+xo-br.x(), cr.y()+yo-br.y(), *pic );
	}
    } else
#endif
    {
#ifndef TQT_NO_IMAGE_SMOOTHSCALE
	if ( scaledcontents && pix ) {
	    if ( !d->img )
		d->img = new TQImage( lpixmap->convertToImage() );

	    if ( !d->pix )
		d->pix = new TQPixmap;
	    if ( d->pix->size() != cr.size() )
		d->pix->convertFromImage( d->img->smoothScale( cr.width(), cr.height() ) );
	    pix = d->pix;
	}
#endif
	int alignment = align;
	if ((align & ShowPrefix) && ((!style().styleHint(TQStyle::SH_UnderlineAccelerator, this)) || ((style().styleHint(TQStyle::SH_HideUnderlineAcceleratorWhenAltUp, this)) && (!style().acceleratorsShown())))) {
	    alignment |= NoAccel;
	}
	// ordinary text or pixmap label
	style().drawItem( p, cr, alignment, colorGroup(), isEnabled(),
			  pix, ltext );
    }
}


/*!
    Updates the label, but not the frame.
*/

void TQLabel::updateLabel( TQSize oldSizeHint )
{
    d->valid_hints = -1;
    TQSizePolicy policy = sizePolicy();
    bool wordBreak = align & WordBreak;
    policy.setHeightForWidth( wordBreak );
    if ( policy != sizePolicy() )
	setSizePolicy( policy );
    if ( sizeHint() != oldSizeHint )
	updateGeometry();
    if ( autoresize ) {
	adjustSize();
	update( contentsRect() );
    } else {
	update( contentsRect() );
    }
}


/*!
  \internal

  Internal slot, used to set focus for accelerator labels.
*/
#ifndef TQT_NO_ACCEL
void TQLabel::acceleratorSlot()
{
    if ( !lbuddy )
	return;
    TQWidget * w = lbuddy;
    while ( w->focusProxy() )
	w = w->focusProxy();
    if ( !w->hasFocus() &&
	 w->isEnabled() &&
	 w->isVisible() &&
	 w->focusPolicy() != NoFocus ) {
	TQFocusEvent::setReason( TQFocusEvent::Shortcut );
	w->setFocus();
	TQFocusEvent::resetReason();
    }
}
#endif

/*!
  \internal

  Internal slot, used to clean up if the buddy widget dies.
*/
#ifndef TQT_NO_ACCEL
void TQLabel::buddyDied() // I can't remember if I cried.
{
    lbuddy = 0;
}

/*!
    Sets this label's buddy to \a buddy.

    When the user presses the accelerator key indicated by this label,
    the keyboard focus is transferred to the label's buddy widget.

    The buddy mechanism is only available for TQLabels that contain
    plain text in which one letter is prefixed with an ampersand, \&.
    This letter is set as the accelerator key. The letter is displayed
    underlined, and the '\&' is not displayed (i.e. the \c ShowPrefix
    alignment flag is turned on; see setAlignment()).

    In a dialog, you might create two data entry widgets and a label
    for each, and set up the geometry layout so each label is just to
    the left of its data entry widget (its "buddy"), for example:
    \code
    TQLineEdit *nameEd  = new TQLineEdit( this );
    TQLabel    *nameLb  = new TQLabel( "&Name:", this );
    nameLb->setBuddy( nameEd );
    TQLineEdit *phoneEd = new TQLineEdit( this );
    TQLabel    *phoneLb = new TQLabel( "&Phone:", this );
    phoneLb->setBuddy( phoneEd );
    // ( layout setup not shown )
    \endcode

    With the code above, the focus jumps to the Name field when the
    user presses Alt+N, and to the Phone field when the user presses
    Alt+P.

    To unset a previously set buddy, call this function with \a buddy
    set to 0.

    \sa buddy(), setText(), TQAccel, setAlignment()
*/

void TQLabel::setBuddy( TQWidget *buddy )
{
    if ( buddy )
	setAlignment( alignment() | ShowPrefix );
    else
	setAlignment( alignment() & ~ShowPrefix );

    if ( lbuddy )
	disconnect( lbuddy, TQ_SIGNAL(destroyed()), this, TQ_SLOT(buddyDied()) );

    lbuddy = buddy;

    if ( !lbuddy )
	return;
#ifndef TQT_NO_RICHTEXT
    if ( !( textformat == RichText || (textformat == AutoText &&
				       TQStyleSheet::mightBeRichText(ltext) ) ) )
#endif
    {
	int p = TQAccel::shortcutKey( ltext );
	if ( p ) {
	    if ( !accel )
		accel = new TQAccel( this, "accel label accel" );
	    accel->connectItem( accel->insertItem( p ),
				this, TQ_SLOT(acceleratorSlot()) );
	}
    }

    connect( lbuddy, TQ_SIGNAL(destroyed()), this, TQ_SLOT(buddyDied()) );
}


/*!
    Returns this label's buddy, or 0 if no buddy is currently set.

    \sa setBuddy()
*/

TQWidget * TQLabel::buddy() const
{
    return lbuddy;
}
#endif //TQT_NO_ACCEL


#ifndef TQT_NO_MOVIE
void TQLabel::movieUpdated(const TQRect& rect)
{
    TQMovie *mov = movie();
    if ( mov && !mov->isNull() ) {
	TQRect r = contentsRect();
	r = style().itemRect( 0, r, align, isEnabled(), &(mov->framePixmap()),
			      TQString::null );
	r.moveBy(rect.x(), rect.y());
	r.setWidth(TQMIN(r.width(), rect.width()));
	r.setHeight(TQMIN(r.height(), rect.height()));
	repaint( r, mov->framePixmap().mask() != 0 );
    }
}

void TQLabel::movieResized( const TQSize& size )
{
    d->valid_hints = -1;
    if ( autoresize )
	adjustSize();
    movieUpdated( TQRect( TQPoint(0,0), size ) );
    updateGeometry();
}

/*!
    Sets the label contents to \a movie. Any previous content is
    cleared.

    The buddy accelerator, if any, is disabled.

    The label resizes itself if auto-resizing is enabled.

    \sa movie(), setBuddy()
*/

void TQLabel::setMovie( const TQMovie& movie )
{
    TQSize osh = sizeHint();
    clearContents();

    lmovie = new TQMovie( movie );
	lmovie->connectResize(this, TQ_SLOT(movieResized(const TQSize&)));
	lmovie->connectUpdate(this, TQ_SLOT(movieUpdated(const TQRect&)));

    if ( !lmovie->running() )	// Assume that if the movie is running,
	updateLabel( osh );	// resize/update signals will come soon enough
}

#endif // TQT_NO_MOVIE

/*!
  \internal

  Clears any contents, without updating/repainting the label.
*/

void TQLabel::clearContents()
{
#ifndef TQT_NO_RICHTEXT
    delete doc;
    doc = 0;
#endif

    delete lpixmap;
    lpixmap = 0;
#ifndef TQT_NO_PICTURE
    delete lpicture;
    lpicture = 0;
#endif
    delete d->img;
    d->img = 0;
    delete d->pix;
    d->pix = 0;

    ltext = TQString::null;
#ifndef TQT_NO_ACCEL
    if ( accel )
	accel->clear();
#endif
#ifndef TQT_NO_MOVIE
    if ( lmovie ) {
	lmovie->disconnectResize(this, TQ_SLOT(movieResized(const TQSize&)));
	lmovie->disconnectUpdate(this, TQ_SLOT(movieUpdated(const TQRect&)));
	delete lmovie;
	lmovie = 0;
    }
#endif
}


#ifndef TQT_NO_MOVIE

/*!
    Returns a pointer to the label's movie, or 0 if no movie has been
    set.

    \sa setMovie()
*/

TQMovie* TQLabel::movie() const
{
    return lmovie;
}

#endif  // TQT_NO_MOVIE

/*!
    \property TQLabel::backgroundMode
    \brief the label's background mode

    Get this property with backgroundMode().

    \sa TQWidget::setBackgroundMode()
*/

/*!
    \property TQLabel::textFormat
    \brief the label's text format

    See the \c TQt::TextFormat enum for an explanation of the possible
    options.

    The default format is \c AutoText.

    \sa text
*/

TQt::TextFormat TQLabel::textFormat() const
{
    return textformat;
}

void TQLabel::setTextFormat( TQt::TextFormat format )
{
    if ( format != textformat ) {
	textformat = format;
	TQString t = ltext;
	if ( !t.isNull() ) {
	    ltext = TQString::null;
	    setText( t );
	}
    }
}

/*!
  \reimp
*/

void TQLabel::fontChange( const TQFont & )
{
    if ( !ltext.isEmpty() ) {
#ifndef TQT_NO_RICHTEXT
	if ( doc )
	    doc->setDefaultFont( font() );
#endif
	updateLabel( TQSize( -1, -1 ) );
    }
}

#ifndef TQT_NO_IMAGE_SMOOTHSCALE
/*!
    \property TQLabel::scaledContents
    \brief whether the label will scale its contents to fill all
    available space.

    When enabled and the label shows a pixmap, it will scale the
    pixmap to fill the available space.

    This property's default is false.

    \sa setScaledContents()
*/
bool TQLabel::hasScaledContents() const
{
    return scaledcontents;
}

void TQLabel::setScaledContents( bool enable )
{
    if ( (bool)scaledcontents == enable )
	return;
    scaledcontents = enable;
    if ( !enable ) {
	delete d->img;
	d->img = 0;
	delete d->pix;
	d->pix = 0;
    }
    update( contentsRect() );
}

#endif // TQT_NO_IMAGE_SMOOTHSCALE

/*!
    Sets the font used on the TQLabel to font \a f.
*/

void TQLabel::setFont( const TQFont &f )
{
    TQFrame::setFont( f );
}

#endif // TQT_NO_LABEL
