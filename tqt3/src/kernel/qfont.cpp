/****************************************************************************
**
** Implementation of TQFont, TQFontMetrics and TQFontInfo classes
**
** Created : 941207
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

#define QT_FATAL_ASSERT

#include "ntqfont.h"
#include "ntqfontdatabase.h"
#include "ntqfontmetrics.h"
#include "ntqfontinfo.h"
#include "ntqpainter.h"
#include "ntqdict.h"
#include "ntqcache.h"
#include "ntqdatastream.h"
#include "ntqapplication.h"
#include "ntqcleanuphandler.h"
#include "ntqstringlist.h"
#ifdef TQ_WS_MAC
#include "ntqpaintdevicemetrics.h"
#endif

#include <private/qunicodetables_p.h>
#include "qfontdata_p.h"
#include "qfontengine_p.h"
#include "qpainter_p.h"
#include "qtextengine_p.h"

// #define TQFONTCACHE_DEBUG
#ifdef TQFONTCACHE_DEBUG
#  define FC_DEBUG tqDebug
#else
#  define FC_DEBUG if (false) tqDebug
#endif




bool TQFontDef::operator==( const TQFontDef &other ) const
{
    /*
      TQFontDef comparison is more complicated than just simple
      per-member comparisons.

      When comparing point/pixel sizes, either point or pixelsize
      could be -1.  in This case we have to compare the non negative
      size value.

      This test will fail if the point-sizes differ by 1/2 point or
      more or they do not round to the same value.  We have to do this
      since our API still uses 'int' point-sizes in the API, but store
      deci-point-sizes internally.

      To compare the family members, we need to parse the font names
      and compare the family/foundry strings separately.  This allows
      us to compare e.g. "Helvetica" and "Helvetica [Adobe]" with
      positive results.
    */
    if (pixelSize != -1 && other.pixelSize != -1) {
	if (pixelSize != other.pixelSize)
	    return false;
    } else if (pointSize != -1 && other.pointSize != -1) {
	if (pointSize != other.pointSize
	    && (TQABS(pointSize - other.pointSize) >= 5
		|| tqRound(pointSize/10.) != tqRound(other.pointSize/10.)))
	    return false;
    } else {
	return false;
    }

    if (!ignorePitch && !other.ignorePitch && fixedPitch != other.fixedPitch)
	return false;

    if (stretch != 0 && other.stretch != 0 && stretch != other.stretch)
	return false;

    TQString this_family, this_foundry, other_family, other_foundry;
    TQFontDatabase::parseFontName(family, this_foundry, this_family);
    TQFontDatabase::parseFontName(other.family, other_foundry, other_family);

    return ( styleHint     == other.styleHint
	    && styleStrategy == other.styleStrategy
	    && weight        == other.weight
	    && italic        == other.italic
	    && this_family   == other_family
	    && (this_foundry.isEmpty()
		|| other_foundry.isEmpty()
		|| this_foundry == other_foundry)
#ifdef TQ_WS_X11
	    && addStyle == other.addStyle
#endif // TQ_WS_X11
	);
}




TQFontPrivate::TQFontPrivate()
    : engineData( 0 ), paintdevice( 0 ),
      rawMode( false ), underline( false ), overline( false ), strikeOut( false ),
      mask( 0 )
{
#ifdef TQ_WS_X11
    screen = TQPaintDevice::x11AppScreen();
#else
    screen = 0;
#endif // TQ_WS_X11
}

TQFontPrivate::TQFontPrivate( const TQFontPrivate &other )
    : TQShared(), request( other.request ), engineData( 0 ),
      paintdevice( other.paintdevice ), screen( other.screen ),
      rawMode( other.rawMode ), underline( other.underline ), overline( other.overline ),
      strikeOut( other.strikeOut ), mask( other.mask )
{
}

TQFontPrivate::~TQFontPrivate()
{
    if ( engineData )
	engineData->deref();
    engineData = 0;
}

void TQFontPrivate::resolve( const TQFontPrivate *other )
{
#ifdef QT_CHECK_STATE
    Q_ASSERT( other != 0 );
#endif

    if ( ( mask & Complete ) == Complete ) return;

    // assign the unset-bits with the set-bits of the other font def
    if ( ! ( mask & Family ) )
	request.family = other->request.family;

    if ( ! ( mask & Size ) ) {
	request.pointSize = other->request.pointSize;
	request.pixelSize = other->request.pixelSize;
    }

    if ( ! ( mask & StyleHint ) )
	request.styleHint = other->request.styleHint;

    if ( ! ( mask & StyleStrategy ) )
	request.styleStrategy = other->request.styleStrategy;

    if ( ! ( mask & Weight ) )
	request.weight = other->request.weight;

    if ( ! ( mask & Italic ) )
	request.italic = other->request.italic;

    if ( ! ( mask & FixedPitch ) )
	request.fixedPitch = other->request.fixedPitch;

    if ( ! ( mask & Stretch ) )
	request.stretch = other->request.stretch;

    if ( ! ( mask & Underline ) )
	underline = other->underline;

    if ( ! ( mask & Overline ) )
	overline = other->overline;

    if ( ! ( mask & StrikeOut ) )
	strikeOut = other->strikeOut;
}




TQFontEngineData::TQFontEngineData()
    : lineWidth( 1 )
{
#if defined(TQ_WS_X11) || defined(TQ_WS_WIN)
    memset( engines, 0, TQFont::LastPrivateScript * sizeof( TQFontEngine * ) );
#else
    engine = 0;
#endif // TQ_WS_X11 || TQ_WS_WIN
#ifndef TQ_WS_MAC
    memset( widthCache, 0, widthCacheSize*sizeof( uchar ) );
#endif
}

TQFontEngineData::~TQFontEngineData()
{
#if defined(TQ_WS_X11) || defined(TQ_WS_WIN)
    for ( int i = 0; i < TQFont::LastPrivateScript; i++ ) {
	if ( engines[i] )
	    engines[i]->deref();
	engines[i] = 0;
    }
#else
    if ( engine )
	engine->deref();
    engine = 0;
#endif // TQ_WS_X11 || TQ_WS_WIN
}




/*!
    \class TQFont ntqfont.h
    \brief The TQFont class specifies a font used for drawing text.

    \ingroup graphics
    \ingroup appearance
    \ingroup shared
    \mainclass

    When you create a TQFont object you specify various attributes that
    you want the font to have. TQt will use the font with the specified
    attributes, or if no matching font exists, TQt will use the closest
    matching installed font. The attributes of the font that is
    actually used are retrievable from a TQFontInfo object. If the
    window system provides an exact match exactMatch() returns true.
    Use TQFontMetrics to get measurements, e.g. the pixel length of a
    string using TQFontMetrics::width().

    Use TQApplication::setFont() to set the application's default font.

    If a choosen X11 font does not include all the characters that
    need to be displayed, TQFont will try to find the characters in the
    nearest equivalent fonts. When a TQPainter draws a character from a
    font the TQFont will report whether or not it has the character; if
    it does not, TQPainter will draw an unfilled square.

    Create TQFonts like this:
    \code
    TQFont serifFont( "Times", 10, Bold );
    TQFont sansFont( "Helvetica [Cronyx]", 12 );
    \endcode

    The attributes set in the constructor can also be set later, e.g.
    setFamily(), setPointSize(), setPointSizeFloat(), setWeight() and
    setItalic(). The remaining attributes must be set after
    contstruction, e.g. setBold(), setUnderline(), setOverline(),
    setStrikeOut() and setFixedPitch(). TQFontInfo objects should be
    created \e after the font's attributes have been set. A TQFontInfo
    object will not change, even if you change the font's
    attributes. The corresponding "get" functions, e.g. family(),
    pointSize(), etc., return the values that were set, even though
    the values used may differ. The actual values are available from a
    TQFontInfo object.

    If the requested font family is unavailable you can influence the
    \link #fontmatching font matching algorithm\endlink by choosing a
    particular \l{TQFont::StyleHint} and \l{TQFont::StyleStrategy} with
    setStyleHint(). The default family (corresponding to the current
    style hint) is returned by defaultFamily().

    The font-matching algorithm has a lastResortFamily() and
    lastResortFont() in cases where a suitable match cannot be found.
    You can provide substitutions for font family names using
    insertSubstitution() and insertSubstitutions(). Substitutions can
    be removed with removeSubstitution(). Use substitute() to retrieve
    a family's first substitute, or the family name itself if it has
    no substitutes. Use substitutes() to retrieve a list of a family's
    substitutes (which may be empty).

    Every TQFont has a key() which you can use, for example, as the key
    in a cache or dictionary. If you want to store a user's font
    preferences you could use TQSettings, writing the font information
    with toString() and reading it back with fromString(). The
    operator<<() and operator>>() functions are also available, but
    they work on a data stream.

    It is possible to set the height of characters shown on the screen
    to a specified number of pixels with setPixelSize(); however using
    setPointSize() has a similar effect and provides device
    independence.

    Under the X Window System you can set a font using its system
    specific name with setRawName().

    Loading fonts can be expensive, especially on X11. TQFont contains
    extensive optimizations to make the copying of TQFont objects fast,
    and to cache the results of the slow window system functions it
    depends upon.

    \target fontmatching
    The font matching algorithm works as follows:
    \list 1
    \i The specified font family is searched for.
    \i If not found, the styleHint() is used to select a replacement
       family.
    \i Each replacement font family is searched for.
    \i If none of these are found or there was no styleHint(), "helvetica"
       will be searched for.
    \i If "helvetica" isn't found TQt will try the lastResortFamily().
    \i If the lastResortFamily() isn't found TQt will try the
       lastResortFont() which will always return a name of some kind.
    \endlist

    Once a font is found, the remaining attributes are matched in order of
    priority:
    \list 1
    \i fixedPitch()
    \i pointSize() (see below)
    \i weight()
    \i italic()
    \endlist

    If you have a font which matches on family, even if none of the
    other attributes match, this font will be chosen in preference to
    a font which doesn't match on family but which does match on the
    other attributes. This is because font family is the dominant
    search criteria.

    The point size is defined to match if it is within 20% of the
    requested point size. When several fonts match and are only
    distinguished by point size, the font with the closest point size
    to the one requested will be chosen.

    The actual family, font size, weight and other font attributes
    used for drawing text will depend on what's available for the
    chosen family under the window system. A TQFontInfo object can be
    used to determine the actual values used for drawing the text.

    Examples:

    \code
    TQFont f("Helvetica");
    \endcode
    If you had both an Adobe and a Cronyx Helvetica, you might get
    either.

    \code
    TQFont f1( "Helvetica [Cronyx]" );  // TQt 3.x
    TQFont f2( "Cronyx-Helvetica" );    // TQt 2.x compatibility
    \endcode
    You can specify the foundry you want in the family name. Both fonts,
    f1 and f2, in the above example will be set to  "Helvetica
    [Cronyx]".

    To determine the attributes of the font actually used in the window
    system, use a TQFontInfo object, e.g.
    \code
    TQFontInfo info( f1 );
    TQString family = info.family();
    \endcode

    To find out font metrics use a TQFontMetrics object, e.g.
    \code
    TQFontMetrics fm( f1 );
    int pixelWidth = fm.width( "How many pixels wide is this text?" );
    int pixelHeight = fm.height();
    \endcode

    For more general information on fonts, see the
    \link http://www.nwalsh.com/comp.fonts/FAQ/ comp.fonts FAQ.\endlink
    Information on encodings can be found from
    \link http://czyborra.com/ Roman Czyborra's\endlink page.

    \sa TQFontMetrics TQFontInfo TQFontDatabase TQApplication::setFont()
    TQWidget::setFont() TQPainter::setFont() TQFont::StyleHint
    TQFont::Weight
*/

/*!
    \enum TQFont::Script

    This enum represents \link unicode.html Unicode \endlink allocated
    scripts. For exhaustive coverage see \link
    http://www.amazon.com/exec/obidos/ASIN/0201616335/trolltech/t The
    Unicode Standard Version 3.0 \endlink. The following scripts are
    supported:

    Modern European alphabetic scripts (left to right):

    \value Latin consists of most alphabets based on the original Latin alphabet.
    \value Greek covers ancient and modern Greek and Coptic.
    \value Cyrillic covers the Slavic and non-Slavic languages using
	   cyrillic alphabets.
    \value Armenian contains the Armenian alphabet used with the
	   Armenian language.
    \value Georgian covers at least the language Georgian.
    \value Runic covers the known constituents of the Runic alphabets used
	   by the early and medieval societies in the Germanic,
	   Scandinavian, and Anglo-Saxon areas.
    \value Ogham is an alphabetical script used to write a very early
	   form of Irish.
    \value SpacingModifiers are small signs indicating modifications
	   to the preceeding letter.
    \value CombiningMarks consist of diacritical marks not specific to
	   a particular alphabet, diacritical marks used in
	   combination with mathematical and technical symbols, and
	   glyph encodings applied to multiple letterforms.

    Middle Eastern scripts (right to left):

    \value Hebrew is used for writing Hebrew, Yiddish, and some other languages.
    \value Arabic covers the Arabic language as well as Persian, Urdu,
	   Kurdish and some others.
    \value Syriac is used to write the active liturgical languages and
	   dialects of several Middle Eastern and Southeast Indian
	   communities.
    \value Thaana is used to write the Maledivian Dhivehi language.

    South and Southeast Asian scripts (left to right with few historical exceptions):

    \value Devanagari covers classical Sanskrit and modern Hindi as
	   well as several other languages.
    \value Bengali is a relative to Devanagari employed to write the
	   Bengali language used in West Bengal/India and Bangladesh
	   as well as several minority languages.
    \value Gurmukhi is another Devanagari relative used to write Punjabi.
    \value Gujarati is closely related to Devanagari and used to write
	   the Gujarati language of the Gujarat state in India.
    \value Oriya is used to write the Oriya language of Orissa state/India.
    \value Tamil is used to write the Tamil language of Tamil Nadu state/India,
	   Sri Lanka, Singapore and parts of Malaysia as well as some
	   minority languages.
    \value Telugu is used to write the Telugu language of Andhra
	   Pradesh state/India and some minority languages.
    \value Kannada is another South Indian script used to write the
	   Kannada language of Karnataka state/India and some minority
	   languages.
    \value Malayalam is used to write the Malayalam language of Kerala
	   state/India.
    \value Sinhala is used for Sri Lanka's majority language Sinhala
	   and is also employed to write Pali, Sanskrit, and Tamil.
    \value Thai is used to write Thai and other Southeast Asian languages.
    \value Lao is a language and script quite similar to Thai.
    \value Tibetan is the script used to write Tibetan in several
	   countries like Tibet, the bordering Indian regions and
	   Nepal. It is also used in the Buddist philosophy and
	   liturgy of the Mongolian cultural area.
    \value Myanmar is mainly used to write the Burmese language of
	   Myanmar (former Burma).
    \value Khmer is the official language of Kampuchea.

    East Asian scripts (traditionally top-down, right to left, modern
    often horizontal left to right):

    \value Han consists of the CJK (Chinese, Japanese, Korean)
	   idiographic characters.
    \value Hiragana is a cursive syllabary used to indicate phonetics
	   and pronounciation of Japanese words.
    \value Katakana is a non-cursive syllabic script used to write
	   Japanese words with visual emphasis and non-Japanese words
	   in a phonetical manner.
    \value Hangul is a Korean script consisting of alphabetic components.
    \value Bopomofo is a phonetic alphabet for Chinese (mainly Mandarin).
    \value Yi (also called Cuan or Wei) is a syllabary used to write
	   the Yi language of Southwestern China, Myanmar, Laos, and Vietnam.

    Additional scripts that do not fit well into the script categories above:

    \value Ethiopic is a syllabary used by several Central East African languages.
    \value Cherokee is a left-to-right syllabic script used to write
	   the Cherokee language.
    \value CanadianAboriginal consists of the syllabics used by some
	   Canadian aboriginal societies.
    \value Mongolian is the traditional (and recently reintroduced)
	   script used to write Mongolian.

    Symbols:

    \value CurrencySymbols contains currency symbols not encoded in other scripts.
    \value LetterlikeSymbols consists of symbols derived  from
	   ordinary letters of an alphabetical script.
    \value NumberForms are provided for compatibility with other
	   existing character sets.
    \value MathematicalOperators consists of encodings for operators,
	   relations and other symbols like arrows used in a mathematical context.
    \value TechnicalSymbols contains representations for control
	   codes, the space symbol, APL symbols and other symbols
	   mainly used in the context of electronic data processing.
    \value GeometricSymbols covers block elements and geometric shapes.
    \value MiscellaneousSymbols consists of a heterogeneous collection
	   of symbols that do not fit any other Unicode character
	   block, e.g. Dingbats.
    \value EnclosedAndSquare is provided for compatibility with some
	   East Asian standards.
    \value Braille is an international writing system used by blind
	   people. This script encodes the 256 eight-dot patterns with
	   the 64 six-dot patterns as a subset.

    \value Tagalog
    \value Hanunoo
    \value Buhid
    \value Tagbanwa

    \value KatakanaHalfWidth

    \value Limbu (Unicode 4.0)
    \value TaiLe (Unicode 4.0)

    \value Unicode includes all the above scripts.
*/

/*! \internal

    Constructs a font for use on the paint device \a pd using the
    specified font \a data.
*/
TQFont::TQFont( TQFontPrivate *data, TQPaintDevice *pd )
{
    d = new TQFontPrivate( *data );
    TQ_CHECK_PTR( d );
    d->paintdevice = pd;

    // now a single reference
    d->count = 1;
}

/*! \internal
    Detaches the font object from common font data.
*/
void TQFont::detach()
{
    if (d->count == 1) {
	if ( d->engineData )
	    d->engineData->deref();
	d->engineData = 0;

	return;
    }

    TQFontPrivate *old_d = d;
    d = new TQFontPrivate( *old_d );

    /*
      if this font is a copy of the application default font, set the
      fontdef mask to zero to indicate that *nothing* has been
      explicitly set by the programmer.
    */
    const TQFont appfont = TQApplication::font();
    if ( old_d == appfont.d )
	d->mask = 0;

    if ( old_d->deref() )
	delete old_d;
}

/*!
    Constructs a font object that uses the application's default font.

    \sa TQApplication::setFont(), TQApplication::font()
*/
TQFont::TQFont()
{
    const TQFont appfont = TQApplication::font();
    d = appfont.d;
    d->ref();
}

/*!
    Constructs a font object with the specified \a family, \a
    pointSize, \a weight and \a italic settings.

    If \a pointSize is <= 0 it is set to 1.

    The \a family name may optionally also include a foundry name,
    e.g. "Helvetica [Cronyx]". (The TQt 2.x syntax, i.e.
    "Cronyx-Helvetica", is also supported.) If the \a family is
    available from more than one foundry and the foundry isn't
    specified, an arbitrary foundry is chosen. If the family isn't
    available a family will be set using the \link #fontmatching font
    matching\endlink algorithm.

    \sa Weight, setFamily(), setPointSize(), setWeight(), setItalic(),
    setStyleHint() TQApplication::font()
*/
TQFont::TQFont( const TQString &family, int pointSize, int weight, bool italic )
{

    d = new TQFontPrivate;
    TQ_CHECK_PTR( d );

    d->mask = TQFontPrivate::Family;

    if (pointSize <= 0) {
	pointSize = 12;
    } else {
	d->mask |= TQFontPrivate::Size;
    }

    if (weight < 0) {
	weight = Normal;
    } else {
	d->mask |= TQFontPrivate::Weight | TQFontPrivate::Italic;
    }

    d->request.family = family;
    d->request.pointSize = pointSize * 10;
    d->request.pixelSize = -1;
    d->request.weight = weight;
    d->request.italic = italic;
}

/*!
    Constructs a font that is a copy of \a font.
*/
TQFont::TQFont( const TQFont &font )
{
    d = font.d;
    d->ref();
}

/*!
    Destroys the font object and frees all allocated resources.
*/
TQFont::~TQFont()
{
    if ( d->deref() )
	delete d;
    d = 0;
}

/*!
    Assigns \a font to this font and returns a reference to it.
*/
TQFont &TQFont::operator=( const TQFont &font )
{
    if ( font.d != d ) {
	if ( d->deref() )
	    delete d;
	d = font.d;
	d->ref();
    }

    return *this;
}

/*!
    Returns the requested font family name, i.e. the name set in the
    constructor or the last setFont() call.

    \sa setFamily() substitutes() substitute()
*/
TQString TQFont::family() const
{
    return d->request.family;
}

/*!
    Sets the family name of the font. The name is case insensitive and
    may include a foundry name.

    The \a family name may optionally also include a foundry name,
    e.g. "Helvetica [Cronyx]". (The TQt 2.x syntax, i.e.
    "Cronyx-Helvetica", is also supported.) If the \a family is
    available from more than one foundry and the foundry isn't
    specified, an arbitrary foundry is chosen. If the family isn't
    available a family will be set using the \link #fontmatching font
    matching\endlink algorithm.

    \sa family(), setStyleHint(), TQFontInfo
*/
void TQFont::setFamily( const TQString &family )
{
    detach();

    d->request.family = family;
#if defined(TQ_WS_X11)
    d->request.addStyle = TQString::null;
#endif // TQ_WS_X11

    d->mask |= TQFontPrivate::Family;
}

/*!
    Returns the point size in 1/10ths of a point.

    The returned value will be -1 if the font size has been specified
    in pixels.

    \sa pointSize() pointSizeFloat()
  */
int TQFont::deciPointSize() const
{
    return d->request.pointSize;
}

/*!
    Returns the point size of the font. Returns -1 if the font size
    was specified in pixels.

    \sa setPointSize() deciPointSize() pointSizeFloat()
*/
int TQFont::pointSize() const
{
    return d->request.pointSize == -1 ? -1 : (d->request.pointSize + 5) / 10;
}

/*!
    Sets the point size to \a pointSize. The point size must be
    greater than zero.

    \sa pointSize() setPointSizeFloat()
*/
void TQFont::setPointSize( int pointSize )
{
    if ( pointSize <= 0 ) {

#if defined(QT_CHECK_RANGE)
	tqWarning( "TQFont::setPointSize: Point size <= 0 (%d)", pointSize );
#endif

	return;
    }

    detach();

    d->request.pointSize = pointSize * 10;
    d->request.pixelSize = -1;

    d->mask |= TQFontPrivate::Size;
}

/*!
    Sets the point size to \a pointSize. The point size must be
    greater than zero. The requested precision may not be achieved on
    all platforms.

    \sa pointSizeFloat() setPointSize() setPixelSize()
*/
void TQFont::setPointSizeFloat( float pointSize )
{
    if ( pointSize <= 0.0 ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQFont::setPointSize: Point size <= 0 (%f)", pointSize );
#endif
	return;
    }

    detach();

    d->request.pointSize = tqRound(pointSize * 10.0);
    d->request.pixelSize = -1;

    d->mask |= TQFontPrivate::Size;
}

/*!
    Returns the point size of the font. Returns -1 if the font size was
    specified in pixels.

    \sa pointSize() setPointSizeFloat() pixelSize() TQFontInfo::pointSize() TQFontInfo::pixelSize()
*/
float TQFont::pointSizeFloat() const
{
    return float( d->request.pointSize == -1 ? -10 : d->request.pointSize ) / 10.0;
}

/*!
    Sets the font size to \a pixelSize pixels.

    Using this function makes the font device dependent. Use
    setPointSize() or setPointSizeFloat() to set the size of the font
    in a device independent manner.

    \sa pixelSize()
*/
void TQFont::setPixelSize( int pixelSize )
{
    if ( pixelSize <= 0 ) {
#if defined(QT_CHECK_RANGE)
	tqWarning( "TQFont::setPixelSize: Pixel size <= 0 (%d)", pixelSize );
#endif
	return;
    }

    detach();

    d->request.pixelSize = pixelSize;
    d->request.pointSize = -1;

    d->mask |= TQFontPrivate::Size;
}

/*!
    Returns the pixel size of the font if it was set with
    setPixelSize(). Returns -1 if the size was set with setPointSize()
    or setPointSizeFloat().

    \sa setPixelSize() pointSize() TQFontInfo::pointSize() TQFontInfo::pixelSize()
*/
int TQFont::pixelSize() const
{
    return d->request.pixelSize;
}

/*! \obsolete

  Sets the logical pixel height of font characters when shown on
  the screen to \a pixelSize.
*/
void TQFont::setPixelSizeFloat( float pixelSize )
{
    setPixelSize( (int)pixelSize );
}

/*!
    Returns true if italic has been set; otherwise returns false.

    \sa setItalic()
*/
bool TQFont::italic() const
{
    return d->request.italic;
}

/*!
    If \a enable is true, italic is set on; otherwise italic is set
    off.

    \sa italic(), TQFontInfo
*/
void TQFont::setItalic( bool enable )
{
    detach();

    d->request.italic = enable;
    d->mask |= TQFontPrivate::Italic;
}

/*!
    Returns the weight of the font which is one of the enumerated
    values from \l{TQFont::Weight}.

    \sa setWeight(), Weight, TQFontInfo
*/
int TQFont::weight() const
{
    return d->request.weight;
}

/*!
    \enum TQFont::Weight

    TQt uses a weighting scale from 0 to 99 similar to, but not the
    same as, the scales used in Windows or CSS. A weight of 0 is
    ultralight, whilst 99 will be an extremely black.

    This enum contains the predefined font weights:

    \value Light 25
    \value Normal 50
    \value DemiBold 63
    \value Bold 75
    \value Black 87
*/

/*!
    Sets the weight the font to \a weight, which should be a value
    from the \l TQFont::Weight enumeration.

    \sa weight(), TQFontInfo
*/
void TQFont::setWeight( int weight )
{
    if ( weight < 0 || weight > 99 ) {

#if defined(QT_CHECK_RANGE)
	tqWarning( "TQFont::setWeight: Value out of range (%d)", weight );
#endif

	return;
    }

    detach();

    d->request.weight = weight;
    d->mask |= TQFontPrivate::Weight;
}

/*!
    \fn bool TQFont::bold() const

    Returns true if weight() is a value greater than \link Weight
    TQFont::Normal \endlink; otherwise returns false.

    \sa weight(), setBold(), TQFontInfo::bold()
*/

/*!
    \fn void TQFont::setBold( bool enable )

    If \a enable is true sets the font's weight to \link Weight
    TQFont::Bold \endlink; otherwise sets the weight to \link Weight
    TQFont::Normal\endlink.

    For finer boldness control use setWeight().

    \sa bold(), setWeight()
*/

/*!
    Returns true if underline has been set; otherwise returns false.

    \sa setUnderline()
*/
bool TQFont::underline() const
{
    return d->underline;
}

/*!
    If \a enable is true, sets underline on; otherwise sets underline
    off.

    \sa underline(), TQFontInfo
*/
void TQFont::setUnderline( bool enable )
{
    detach();

    d->underline = enable;
    d->mask |= TQFontPrivate::Underline;
}

/*!
    Returns true if overline has been set; otherwise returns false.

    \sa setOverline()
*/
bool TQFont::overline() const
{
    return d->overline;
}

/*!
  If \a enable is true, sets overline on; otherwise sets overline off.

  \sa overline(), TQFontInfo
*/
void TQFont::setOverline( bool enable )
{
    detach();

    d->overline = enable;
    d->mask |= TQFontPrivate::Overline;
}

/*!
    Returns true if strikeout has been set; otherwise returns false.

    \sa setStrikeOut()
*/
bool TQFont::strikeOut() const
{
    return d->strikeOut;
}

/*!
    If \a enable is true, sets strikeout on; otherwise sets strikeout
    off.

    \sa strikeOut(), TQFontInfo
*/
void TQFont::setStrikeOut( bool enable )
{
    detach();

    d->strikeOut = enable;
    d->mask |= TQFontPrivate::StrikeOut;
}

/*!
    Returns true if fixed pitch has been set; otherwise returns false.

    \sa setFixedPitch(), TQFontInfo::fixedPitch()
*/
bool TQFont::fixedPitch() const
{
    return d->request.fixedPitch;
}

/*!
    If \a enable is true, sets fixed pitch on; otherwise sets fixed
    pitch off.

    \sa fixedPitch(), TQFontInfo
*/
void TQFont::setFixedPitch( bool enable )
{
    detach();

    d->request.fixedPitch = enable;
    d->request.ignorePitch = false;
    d->mask |= TQFontPrivate::FixedPitch;
}

/*!
    Returns the StyleStrategy.

    The style strategy affects the \link #fontmatching font
    matching\endlink algorithm. See \l TQFont::StyleStrategy for the
    list of strategies.

    \sa setStyleHint() TQFont::StyleHint
*/
TQFont::StyleStrategy TQFont::styleStrategy() const
{
    return (StyleStrategy) d->request.styleStrategy;
}

/*!
    Returns the StyleHint.

    The style hint affects the \link #fontmatching font
    matching\endlink algorithm. See \l TQFont::StyleHint for the list
    of strategies.

    \sa setStyleHint(), TQFont::StyleStrategy TQFontInfo::styleHint()
*/
TQFont::StyleHint TQFont::styleHint() const
{
    return (StyleHint) d->request.styleHint;
}

/*!
    \enum TQFont::StyleHint

    Style hints are used by the \link #fontmatching font
    matching\endlink algorithm to find an appropriate default family
    if a selected font family is not available.

    \value AnyStyle leaves the font matching algorithm to choose the
	   family. This is the default.

    \value SansSerif the font matcher prefer sans serif fonts.
    \value Helvetica is a synonym for \c SansSerif.

    \value Serif the font matcher prefers serif fonts.
    \value Times is a synonym for \c Serif.

    \value TypeWriter the font matcher prefers fixed pitch fonts.
    \value Courier a synonym for \c TypeWriter.

    \value OldEnglish the font matcher prefers decorative fonts.
    \value Decorative is a synonym for \c OldEnglish.

    \value System the font matcher prefers system fonts.
*/

/*!
    \enum TQFont::StyleStrategy

    The style strategy tells the \link #fontmatching font
    matching\endlink algorithm what type of fonts should be used to
    find an appropriate default family.

    The following strategies are available:

    \value PreferDefault the default style strategy. It does not prefer
	   any type of font.
    \value PreferBitmap prefers bitmap fonts (as opposed to outline
	   fonts).
    \value PreferDevice prefers device fonts.
    \value PreferOutline prefers outline fonts (as opposed to bitmap fonts).
    \value ForceOutline forces the use of outline fonts.
    \value NoAntialias don't antialias the fonts.
    \value PreferAntialias antialias if possible.
    \value OpenGLCompatible forces the use of OpenGL compatible
           fonts.

    Any of these may be OR-ed with one of these flags:

    \value PreferMatch prefer an exact match. The font matcher will try to
	   use the exact font size that has been specified.
    \value PreferQuality prefer the best quality font. The font matcher
	   will use the nearest standard point size that the font
	   supports.
*/

/*!
    Sets the style hint and strategy to \a hint and \a strategy,
    respectively.

    If these aren't set explicitly the style hint will default to
    \c AnyStyle and the style strategy to \c PreferDefault.

    TQt does not support style hints on X11 since this information
    is not provided by the window system.

    \sa StyleHint, styleHint(), StyleStrategy, styleStrategy(), TQFontInfo
*/
void TQFont::setStyleHint( StyleHint hint, StyleStrategy strategy )
{
    detach();

    if ( ( d->mask & ( TQFontPrivate::StyleHint | TQFontPrivate::StyleStrategy ) ) &&
	 (StyleHint) d->request.styleHint == hint &&
	 (StyleStrategy) d->request.styleStrategy == strategy )
	return;

    d->request.styleHint = hint;
    d->request.styleStrategy = strategy;
    d->mask |= TQFontPrivate::StyleHint;
    d->mask |= TQFontPrivate::StyleStrategy;

#if defined(TQ_WS_X11)
    d->request.addStyle = TQString::null;
#endif // TQ_WS_X11
}

/*!
    Sets the style strategy for the font to \a s.

    \sa TQFont::StyleStrategy
*/
void TQFont::setStyleStrategy( StyleStrategy s )
{
    detach();

    if ( ( d->mask & TQFontPrivate::StyleStrategy ) &&
	 s == (StyleStrategy)d->request.styleStrategy )
	return;

    d->request.styleStrategy = s;
    d->mask |= TQFontPrivate::StyleStrategy;
}


/*!
    \enum TQFont::Stretch

    Predefined stretch values that follow the CSS naming convention.

    \value UltraCondensed 50
    \value ExtraCondensed 62
    \value Condensed 75
    \value SemiCondensed 87
    \value Unstretched 100
    \value SemiExpanded 112
    \value Expanded 125
    \value ExtraExpanded 150
    \value UltraExpanded 200

    \sa setStretch() stretch()
*/

/*!
    Returns the stretch factor for the font.

    \sa setStretch()
 */
int TQFont::stretch() const
{
    return d->request.stretch;
}

/*!
    Sets the stretch factor for the font.

    The stretch factor changes the width of all characters in the font
    by \a factor percent.  For example, setting \a factor to 150
    results in all characters in the font being 1.5 times ( ie. 150% )
    wider.  The default stretch factor is 100.  The minimum stretch
    factor is 1, and the maximum stretch factor is 4000.

    The stretch factor is only applied to outline fonts.  The stretch
    factor is ignored for bitmap fonts.

    NOTE: TQFont cannot stretch XLFD fonts.  When loading XLFD fonts on
    X11, the stretch factor is matched against a predefined set of
    values for the SETWIDTH_NAME field of the XLFD.

    \sa stretch() TQFont::StyleStrategy
*/
void TQFont::setStretch( int factor )
{
    if ( factor < 1 || factor > 4000 ) {
#ifdef QT_CHECK_RANGE
	tqWarning( "TQFont::setStretch(): parameter '%d' out of range", factor );
#endif // QT_CHECK_RANGE

	return;
    }

    detach();

    if ( ( d->mask & TQFontPrivate::Stretch ) &&
	 d->request.stretch == (uint)factor )
	return;

    d->request.stretch = (uint)factor;
    d->mask |= TQFontPrivate::Stretch;
}

/*!
    If \a enable is true, turns raw mode on; otherwise turns raw mode
    off. This function only has an effect under X11.

    If raw mode is enabled, TQt will search for an X font with a
    complete font name matching the family name, ignoring all other
    values set for the TQFont. If the font name matches several fonts,
    TQt will use the first font returned by X. TQFontInfo \e cannot be
    used to fetch information about a TQFont using raw mode (it will
    return the values set in the TQFont for all parameters, including
    the family name).

    \warning Do not use raw mode unless you really, really need it! In
    most (if not all) cases, setRawName() is a much better choice.

    \sa rawMode(), setRawName()
*/
void TQFont::setRawMode( bool enable )
{
    detach();

    if ( (bool) d->rawMode == enable ) return;

    d->rawMode = enable;
}

/*!
    Returns true if a window system font exactly matching the settings
    of this font is available.

    \sa TQFontInfo
*/
bool TQFont::exactMatch() const
{
    TQFontEngine *engine = d->engineForScript( TQFont::NoScript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

    return d->rawMode ? engine->type() != TQFontEngine::Box
			     : d->request == engine->fontDef;
}

/*!
    Returns true if this font is equal to \a f; otherwise returns
    false.

    Two TQFonts are considered equal if their font attributes are
    equal. If rawMode() is enabled for both fonts, only the family
    fields are compared.

    \sa operator!=() isCopyOf()
*/
bool TQFont::operator==( const TQFont &f ) const
{
    return f.d == d || ( f.d->request   == d->request   &&
			 f.d->underline == d->underline &&
			 f.d->overline  == d->overline  &&
			 f.d->strikeOut == d->strikeOut );
}

/*!
    Returns true if this font is different from \a f; otherwise
    returns false.

    Two TQFonts are considered to be different if their font attributes
    are different. If rawMode() is enabled for both fonts, only the
    family fields are compared.

    \sa operator==()
*/
bool TQFont::operator!=( const TQFont &f ) const
{
    return !(operator==( f ));
}

/*!
    Returns true if this font and \a f are copies of each other, i.e.
    one of them was created as a copy of the other and neither has
    been modified since. This is much stricter than equality.

    \sa operator=() operator==()
*/
bool TQFont::isCopyOf( const TQFont & f ) const
{
    return d == f.d;
}

/*!
    Returns true if raw mode is used for font name matching; otherwise
    returns false.

    \sa setRawMode() rawName()
*/
bool TQFont::rawMode() const
{
    return d->rawMode;
}

/*!
    Returns a new TQFont that has attributes copied from \a other.
*/
TQFont TQFont::resolve( const TQFont &other ) const
{
    if ( *this == other && d->mask == other.d->mask )
	return *this;

    TQFont font( *this );
    font.detach();

    /*
      if this font is a copy of the application default font, set the
      fontdef mask to zero to indicate that *nothing* has been
      explicitly set by the programmer.
    */
    const TQFont appfont = TQApplication::font();
    if ( d == appfont.d )
	font.d->mask = 0;

    font.d->resolve( other.d );

    return font;
}

#ifndef TQT_NO_COMPAT

/*! \obsolete

  Please use TQApplication::font() instead.
*/
TQFont TQFont::defaultFont()
{
    return TQApplication::font();
}

/*! \obsolete

  Please use TQApplication::setFont() instead.
*/
void TQFont::setDefaultFont( const TQFont &f )
{
    TQApplication::setFont( f );
}


#endif




#ifndef TQT_NO_STRINGLIST

/*****************************************************************************
  TQFont substitution management
 *****************************************************************************/

typedef TQDict<TQStringList> TQFontSubst;
static TQFontSubst *fontSubst = 0;
static TQSingleCleanupHandler<TQFontSubst> qfont_cleanup_fontsubst;


// create substitution dict
static void initFontSubst()
{
    // default substitutions
    static const char *initTbl[] = {

#if defined(TQ_WS_X11)
	"arial",        "helvetica",
	"helv",         "helvetica",
	"tms rmn",      "times",
#elif defined(TQ_WS_WIN)
	"times",        "Times New Roman",
	"courier",      "Courier New",
	"helvetica",    "Arial",
#endif

	0,              0
    };

    if (fontSubst)
	return;

    fontSubst = new TQFontSubst(17, false);
    TQ_CHECK_PTR( fontSubst );
    fontSubst->setAutoDelete( true );
    qfont_cleanup_fontsubst.set(&fontSubst);

    for ( int i=0; initTbl[i] != 0; i += 2 )
	TQFont::insertSubstitution(TQString::fromLatin1(initTbl[i]),
				  TQString::fromLatin1(initTbl[i+1]));
}


/*!
    Returns the first family name to be used whenever \a familyName is
    specified. The lookup is case insensitive.

    If there is no substitution for \a familyName, \a familyName is
    returned.

    To obtain a list of substitutions use substitutes().

    \sa setFamily() insertSubstitutions() insertSubstitution() removeSubstitution()
*/
TQString TQFont::substitute( const TQString &familyName )
{
    initFontSubst();

    TQStringList *list = fontSubst->find(familyName);
    if (list && list->count() > 0)
	return *(list->at(0));

    return familyName;
}


/*!
    Returns a list of family names to be used whenever \a familyName
    is specified. The lookup is case insensitive.

    If there is no substitution for \a familyName, an empty list is
    returned.

    \sa substitute() insertSubstitutions() insertSubstitution() removeSubstitution()
 */
TQStringList TQFont::substitutes(const TQString &familyName)
{
    initFontSubst();

    TQStringList ret, *list = fontSubst->find(familyName);
    if (list)
	ret += *list;
    return ret;
}


/*!
    Inserts the family name \a substituteName into the substitution
    table for \a familyName.

    \sa insertSubstitutions() removeSubstitution() substitutions() substitute() substitutes()
*/
void TQFont::insertSubstitution(const TQString &familyName,
			       const TQString &substituteName)
{
    initFontSubst();

    TQStringList *list = fontSubst->find(familyName);
    if (! list) {
	list = new TQStringList;
	fontSubst->insert(familyName, list);
    }

    if (! list->contains(substituteName))
	list->append(substituteName);
}


/*!
    Inserts the list of families \a substituteNames into the
    substitution list for \a familyName.

    \sa insertSubstitution(), removeSubstitution(), substitutions(), substitute()
*/
void TQFont::insertSubstitutions(const TQString &familyName,
				const TQStringList &substituteNames)
{
    initFontSubst();

    TQStringList *list = fontSubst->find(familyName);
    if (! list) {
	list = new TQStringList;
	fontSubst->insert(familyName, list);
    }

    TQStringList::ConstIterator it = substituteNames.begin();
    while (it != substituteNames.end()) {
	if (! list->contains(*it))
	    list->append(*it);
	it++;
    }
}

// ### mark: should be called removeSubstitutions()
/*!
    Removes all the substitutions for \a familyName.

    \sa insertSubstitutions(), insertSubstitution(), substitutions(), substitute()
*/
void TQFont::removeSubstitution( const TQString &familyName )
{ // ### function name should be removeSubstitutions() or
  // ### removeSubstitutionList()
    initFontSubst();

    fontSubst->remove(familyName);
}


/*!
    Returns a sorted list of substituted family names.

    \sa insertSubstitution(), removeSubstitution(), substitute()
*/
TQStringList TQFont::substitutions()
{
    initFontSubst();

    TQStringList ret;
    TQDictIterator<TQStringList> it(*fontSubst);

    while (it.current()) {
	ret.append(it.currentKey());
	++it;
    }

    ret.sort();

    return ret;
}

#endif // TQT_NO_STRINGLIST


/*  \internal
    Internal function. Converts boolean font settings to an unsigned
    8-bit number. Used for serialization etc.
*/
static TQ_UINT8 get_font_bits( const TQFontPrivate *f )
{
#ifdef QT_CHECK_STATE
    Q_ASSERT( f != 0 );
#endif

    TQ_UINT8 bits = 0;
    if ( f->request.italic )
	bits |= 0x01;
    if ( f->underline )
	bits |= 0x02;
    if ( f->overline )
	bits |= 0x40;
    if ( f->strikeOut )
	bits |= 0x04;
    if ( f->request.fixedPitch )
	bits |= 0x08;
    // if ( f.hintSetByUser )
    // bits |= 0x10;
    if ( f->rawMode )
	bits |= 0x20;
    return bits;
}


#ifndef TQT_NO_DATASTREAM

/*  \internal
    Internal function. Sets boolean font settings from an unsigned
    8-bit number. Used for serialization etc.
*/
static void set_font_bits( TQ_UINT8 bits, TQFontPrivate *f )
{
#ifdef QT_CHECK_STATE
    Q_ASSERT( f != 0 );
#endif

    f->request.italic        = (bits & 0x01) != 0;
    f->underline             = (bits & 0x02) != 0;
    f->overline              = (bits & 0x40) != 0;
    f->strikeOut             = (bits & 0x04) != 0;
    f->request.fixedPitch    = (bits & 0x08) != 0;
    // f->hintSetByUser      = (bits & 0x10) != 0;
    f->rawMode               = (bits & 0x20) != 0;
}

#endif


/*!
    Returns the font's key, a textual representation of a font. It is
    typically used as the key for a cache or dictionary of fonts.

    \sa TQMap
*/
TQString TQFont::key() const
{
    return toString();
}

/*!
    Returns a description of the font. The description is a
    comma-separated list of the attributes, perfectly suited for use
    in TQSettings.

    \sa fromString() operator<<()
 */
TQString TQFont::toString() const
{
    const TQChar comma( ',' );
    return family() + comma +
	TQString::number(  pointSizeFloat() ) + comma +
	TQString::number(       pixelSize() ) + comma +
	TQString::number( (int) styleHint() ) + comma +
	TQString::number(          weight() ) + comma +
	TQString::number( (int)    italic() ) + comma +
	TQString::number( (int) underline() ) + comma +
	TQString::number( (int) strikeOut() ) + comma +
	TQString::number( (int)fixedPitch() ) + comma +
	TQString::number( (int)   rawMode() );
}


/*!
    Sets this font to match the description \a descrip. The description
    is a comma-separated list of the font attributes, as returned by
    toString().

    \sa toString() operator>>()
 */
bool TQFont::fromString(const TQString &descrip)
{
#ifndef TQT_NO_STRINGLIST
    TQStringList l(TQStringList::split(',', descrip));

    int count = (int)l.count();
#else
    int count = 0;
    TQString l[11];
    int from = 0;
    int to = descrip.find( ',' );
    while ( to > 0 && count < 11 ) {
	l[count] = descrip.mid( from, to-from );
	count++;
	from = to+1;
	to = descrip.find( ',', from );
    }
#endif // TQT_NO_STRINGLIST
    if ( !count || ( count > 2 && count < 9 ) || count > 11 ) {

#ifdef QT_CHECK_STATE
	tqWarning("TQFont::fromString: invalid description '%s'",
                 descrip.isEmpty() ? "(empty)" : descrip.latin1());
#endif

	return false;
    }

    setFamily(l[0]);
    if ( count > 1 && l[1].toDouble() > 0.0 )
	setPointSizeFloat(l[1].toDouble());
    if ( count == 9 ) {
	setStyleHint((StyleHint) l[2].toInt());
	setWeight(l[3].toInt());
	setItalic(l[4].toInt());
	setUnderline(l[5].toInt());
	setStrikeOut(l[6].toInt());
	setFixedPitch(l[7].toInt());
	setRawMode(l[8].toInt());
    } else if ( count == 10 ) {
	if ( l[2].toInt() > 0 )
	    setPixelSize( l[2].toInt() );
	setStyleHint((StyleHint) l[3].toInt());
	setWeight(l[4].toInt());
	setItalic(l[5].toInt());
	setUnderline(l[6].toInt());
	setStrikeOut(l[7].toInt());
	setFixedPitch(l[8].toInt());
	setRawMode(l[9].toInt());
    }

    return true;
}

#if !defined( TQ_WS_QWS )
/*! \internal

  Internal function that dumps font cache statistics.
*/
void TQFont::cacheStatistics()
{


}
#endif // !TQ_WS_QWS



/*****************************************************************************
  TQFont stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM

/*!
    \relates TQFont

    Writes the font \a font to the data stream \a s. (toString()
    writes to a text stream.)

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/
TQDataStream &operator<<( TQDataStream &s, const TQFont &font )
{
    if ( s.version() == 1 ) {
	TQCString fam( font.d->request.family.latin1() );
	s << fam;
    } else {
	s << font.d->request.family;
    }

    if ( s.version() <= 3 ) {
	TQ_INT16 pointSize = (TQ_INT16) font.d->request.pointSize;
	if ( pointSize == -1 ) {
#ifdef TQ_WS_X11
	    pointSize = (TQ_INT16)(font.d->request.pixelSize*720/TQPaintDevice::x11AppDpiY());
#else
	    pointSize = (TQ_INT16)TQFontInfo( font ).pointSize() * 10;
#endif
	}
	s << pointSize;
    } else {
	s << (TQ_INT16) font.d->request.pointSize;
	s << (TQ_INT16) font.d->request.pixelSize;
    }

    s << (TQ_UINT8) font.d->request.styleHint;
    if ( s.version() >= 5 )
	s << (TQ_UINT8 ) font.d->request.styleStrategy;
    return s << (TQ_UINT8) 0
	     << (TQ_UINT8) font.d->request.weight
	     << get_font_bits(font.d);
}


/*!
    \relates TQFont

    Reads the font \a font from the data stream \a s. (fromString()
    reads from a text stream.)

    \sa \link datastreamformat.html Format of the TQDataStream operators \endlink
*/
TQDataStream &operator>>( TQDataStream &s, TQFont &font )
{
    if (font.d->deref()) delete font.d;

    font.d = new TQFontPrivate;
    font.d->mask = TQFontPrivate::Complete;

    TQ_INT16 pointSize, pixelSize = -1;
    TQ_UINT8 styleHint, styleStrategy = TQFont::PreferDefault, charSet, weight, bits;

    if ( s.version() == 1 ) {
	TQCString fam;
	s >> fam;
	font.d->request.family = TQString( fam );
    } else {
	s >> font.d->request.family;
    }

    s >> pointSize;
    if ( s.version() >= 4 )
	s >> pixelSize;
    s >> styleHint;
    if ( s.version() >= 5 )
	s >> styleStrategy;
    s >> charSet;
    s >> weight;
    s >> bits;

    font.d->request.pointSize = pointSize;
    font.d->request.pixelSize = pixelSize;
    font.d->request.styleHint = styleHint;
    font.d->request.styleStrategy = styleStrategy;
    font.d->request.weight = weight;

    set_font_bits( bits, font.d );

    return s;
}

#endif // TQT_NO_DATASTREAM




/*****************************************************************************
  TQFontMetrics member functions
 *****************************************************************************/

/*!
    \class TQFontMetrics ntqfontmetrics.h
    \brief The TQFontMetrics class provides font metrics information.

    \ingroup graphics
    \ingroup shared

    TQFontMetrics functions calculate the size of characters and
    strings for a given font. There are three ways you can create a
    TQFontMetrics object:

    \list 1
    \i Calling the TQFontMetrics constructor with a TQFont creates a
    font metrics object for a screen-compatible font, i.e. the font
    cannot be a printer font<sup>*</sup>. If the font is changed
    later, the font metrics object is \e not updated.

    \i TQWidget::fontMetrics() returns the font metrics for a widget's
    font. This is equivalent to TQFontMetrics(widget->font()). If the
    widget's font is changed later, the font metrics object is \e not
    updated.

    \i TQPainter::fontMetrics() returns the font metrics for a
    painter's current font. If the painter's font is changed later, the
    font metrics object is \e not updated.
    \endlist

    <sup>*</sup> If you use a printer font the values returned may be
    inaccurate. Printer fonts are not always accessible so the nearest
    screen font is used if a printer font is supplied.

    Once created, the object provides functions to access the
    individual metrics of the font, its characters, and for strings
    rendered in the font.

    There are several functions that operate on the font: ascent(),
    descent(), height(), leading() and lineSpacing() return the basic
    size properties of the font. The underlinePos(), overlinePos(),
    strikeOutPos() and lineWidth() functions, return the properties of
    the line that underlines, overlines or strikes out the
    characters. These functions are all fast.

    There are also some functions that operate on the set of glyphs in
    the font: minLeftBearing(), minRightBearing() and maxWidth().
    These are by necessity slow, and we recommend avoiding them if
    possible.

    For each character, you can get its width(), leftBearing() and
    rightBearing() and find out whether it is in the font using
    inFont(). You can also treat the character as a string, and use
    the string functions on it.

    The string functions include width(), to return the width of a
    string in pixels (or points, for a printer), boundingRect(), to
    return a rectangle large enough to contain the rendered string,
    and size(), to return the size of that rectangle.

    Example:
    \code
    TQFont font( "times", 24 );
    TQFontMetrics fm( font );
    int pixelsWide = fm.width( "What's the width of this text?" );
    int pixelsHigh = fm.height();
    \endcode

    \sa TQFont TQFontInfo TQFontDatabase
*/

/*!
    Constructs a font metrics object for \a font.

    The font must be screen-compatible, i.e. a font you use when
    drawing text in \link TQWidget widgets\endlink or \link TQPixmap
    pixmaps\endlink, not TQPicture or TQPrinter.

    The font metrics object holds the information for the font that is
    passed in the constructor at the time it is created, and is not
    updated if the font's attributes are changed later.

  Use TQPainter::fontMetrics() to get the font metrics when painting.
  This will give correct results also when painting on paint device
  that is not screen-compatible.
*/
TQFontMetrics::TQFontMetrics( const TQFont &font )
    : d( font.d ), painter( 0 ), fscript( TQFont::NoScript )
{
    d->ref();
}

/*!
    \overload

    Constructs a font metrics object for \a font using the given \a
    script.
*/
TQFontMetrics::TQFontMetrics( const TQFont &font, TQFont::Script script )
    : d( font.d ), painter( 0 ), fscript( script )
{
    d->ref();
}

/*! \internal

  Constructs a font metrics object for the painter's font \a p.
*/
TQFontMetrics::TQFontMetrics( const TQPainter *p )
    : painter ( (TQPainter *) p ), fscript( TQFont::NoScript )
{
#if defined(CHECK_STATE)
    if ( !painter->isActive() )
	tqWarning( "TQFontMetrics: Get font metrics between TQPainter::begin() "
		  "and TQPainter::end()" );
#endif

    if ( painter->testf(TQPainter::DirtyFont) )
	painter->updateFont();

    d = painter->pfont ? painter->pfont->d : painter->cfont.d;

#if defined(TQ_WS_X11)
    if ( d->screen != p->scrn ) {
	TQFontPrivate *new_d = new TQFontPrivate( *d );
	TQ_CHECK_PTR( new_d );
	d = new_d;
	d->screen = p->scrn;
	d->count = 1;
    } else
#endif // TQ_WS_X11
	d->ref();
}

/*!
    Constructs a copy of \a fm.
*/
TQFontMetrics::TQFontMetrics( const TQFontMetrics &fm )
    : d( fm.d ), painter( 0 ),  fscript( fm.fscript )
{
    d->ref();
}

/*!
    Destroys the font metrics object and frees all allocated
    resources.
*/
TQFontMetrics::~TQFontMetrics()
{
    if ( d->deref() )
	delete d;
}

/*!
    Assigns the font metrics \a fm.
*/
TQFontMetrics &TQFontMetrics::operator=( const TQFontMetrics &fm )
{
    if ( d != fm.d ) {
	if ( d->deref() )
	    delete d;
	d = fm.d;
	d->ref();
    }
    painter = fm.painter;
    return *this;
}

/*!
    Returns the ascent of the font.

    The ascent of a font is the distance from the baseline to the
    highest position characters extend to. In practice, some font
    designers break this rule, e.g. when they put more than one accent
    on top of a character, or to accommodate an unusual character in
    an exotic language, so it is possible (though rare) that this
    value will be too small.

    \sa descent()
*/
int TQFontMetrics::ascent() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
    TQFontEngine *latin_engine = d->engineForScript( TQFont::Latin );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
    Q_ASSERT( latin_engine != 0 );
#endif // QT_CHECK_STATE

    return TQMAX(engine->ascent(), latin_engine->ascent());
}


/*!
    Returns the descent of the font.

    The descent is the distance from the base line to the lowest point
    characters extend to. (Note that this is different from X, which
    adds 1 pixel.) In practice, some font designers break this rule,
    e.g. to accommodate an unusual character in an exotic language, so
    it is possible (though rare) that this value will be too small.

    \sa ascent()
*/
int TQFontMetrics::descent() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
    TQFontEngine *latin_engine = d->engineForScript( TQFont::Latin );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
    Q_ASSERT( latin_engine != 0 );
#endif // QT_CHECK_STATE

    return TQMAX(engine->descent(), latin_engine->descent());
}

/*!
    Returns the height of the font.

    This is always equal to ascent()+descent()+1 (the 1 is for the
    base line).

    \sa leading(), lineSpacing()
*/
int TQFontMetrics::height() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
    TQFontEngine *latin_engine = d->engineForScript( TQFont::Latin );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
    Q_ASSERT( latin_engine != 0 );
#endif // QT_CHECK_STATE

    return (TQMAX(engine->ascent(), latin_engine->ascent()) +
	    TQMAX(engine->descent(), latin_engine->descent()) + 1);
}

/*!
    Returns the leading of the font.

    This is the natural inter-line spacing.

    \sa height(), lineSpacing()
*/
int TQFontMetrics::leading() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
    TQFontEngine *latin_engine = d->engineForScript( TQFont::Latin );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
    Q_ASSERT( latin_engine != 0 );
#endif // QT_CHECK_STATE

    return TQMAX(engine->leading(), latin_engine->leading());
}

/*!
    Returns the distance from one base line to the next.

    This value is always equal to leading()+height().

    \sa height(), leading()
*/
int TQFontMetrics::lineSpacing() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
    TQFontEngine *latin_engine = d->engineForScript( TQFont::Latin );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
    Q_ASSERT( latin_engine != 0 );
#endif // QT_CHECK_STATE

    return (TQMAX(engine->leading(), latin_engine->leading()) +
	    TQMAX(engine->ascent(), latin_engine->ascent()) +
	    TQMAX(engine->descent(), latin_engine->descent()) + 1);
}

/*!
    Returns the minimum left bearing of the font.

    This is the smallest leftBearing(char) of all characters in the
    font.

    Note that this function can be very slow if the font is large.

    \sa minRightBearing(), leftBearing()
*/
int TQFontMetrics::minLeftBearing() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
    TQFontEngine *latin_engine = d->engineForScript( TQFont::Latin );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
    Q_ASSERT( latin_engine != 0 );
#endif // QT_CHECK_STATE

    return TQMIN(engine->minLeftBearing(), latin_engine->minLeftBearing());
}

/*!
    Returns the minimum right bearing of the font.

    This is the smallest rightBearing(char) of all characters in the
    font.

    Note that this function can be very slow if the font is large.

    \sa minLeftBearing(), rightBearing()
*/
int TQFontMetrics::minRightBearing() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
    TQFontEngine *latin_engine = d->engineForScript( TQFont::Latin );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
    Q_ASSERT( latin_engine != 0 );
#endif // QT_CHECK_STATE

    return TQMIN(engine->minRightBearing(), latin_engine->minRightBearing());
}

/*!
    Returns the width of the widest character in the font.
*/
int TQFontMetrics::maxWidth() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
    TQFontEngine *lengine = d->engineForScript( TQFont::Latin );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
    Q_ASSERT( lengine != 0 );
#endif // QT_CHECK_STATE

    return TQMAX(engine->maxCharWidth(), lengine->maxCharWidth());
}

/*!
    Returns true if character \a ch is a valid character in the font;
    otherwise returns false.
*/
bool TQFontMetrics::inFont(TQChar ch) const
{
    TQFont::Script script;
    SCRIPT_FOR_CHAR( script, ch );

    TQFontEngine *engine = d->engineForScript( script );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

    if ( engine->type() == TQFontEngine::Box ) return false;
    return engine->canRender( &ch, 1 );
}

/*! \fn int TQFontMetrics::leftBearing( TQChar ch ) const
    Returns the left bearing of character \a ch in the font.

    The left bearing is the right-ward distance of the left-most pixel
    of the character from the logical origin of the character. This
    value is negative if the pixels of the character extend to the
    left of the logical origin.

    See width(TQChar) for a graphical description of this metric.

    \sa rightBearing(), minLeftBearing(), width()
*/
#if !defined(TQ_WS_WIN) && !defined(TQ_WS_QWS)
int TQFontMetrics::leftBearing(TQChar ch) const
{
    TQFont::Script script;
    SCRIPT_FOR_CHAR( script, ch );

    TQFontEngine *engine = d->engineForScript( script );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

    if ( engine->type() == TQFontEngine::Box ) return 0;

    glyph_t glyphs[10];
    int nglyphs = 9;
    engine->stringToCMap( &ch, 1, glyphs, 0, &nglyphs, false );
    // ### can nglyphs != 1 happen at all? Not currently I think
    glyph_metrics_t gi = engine->boundingBox( glyphs[0] );
    return gi.x;
}
#endif // !TQ_WS_WIN

/*! \fn int TQFontMetrics::rightBearing(TQChar ch) const
    Returns the right bearing of character \a ch in the font.

    The right bearing is the left-ward distance of the right-most
    pixel of the character from the logical origin of a subsequent
    character. This value is negative if the pixels of the character
    extend to the right of the width() of the character.

    See width() for a graphical description of this metric.

    \sa leftBearing(), minRightBearing(), width()
*/
#if !defined(TQ_WS_WIN) && !defined(TQ_WS_QWS)
int TQFontMetrics::rightBearing(TQChar ch) const
{
    TQFont::Script script;
    SCRIPT_FOR_CHAR( script, ch );

    TQFontEngine *engine = d->engineForScript( script );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

    if ( engine->type() == TQFontEngine::Box ) return 0;

    glyph_t glyphs[10];
    int nglyphs = 9;
    engine->stringToCMap( &ch, 1, glyphs, 0, &nglyphs, false );
    // ### can nglyphs != 1 happen at all? Not currently I think
    glyph_metrics_t gi = engine->boundingBox( glyphs[0] );
    return gi.xoff - gi.x - gi.width;
}
#endif // !TQ_WS_WIN


#ifndef TQ_WS_QWS
/*!
    Returns the width in pixels of the first \a len characters of \a
    str. If \a len is negative (the default), the entire string is
    used.

    Note that this value is \e not equal to boundingRect().width();
    boundingRect() returns a rectangle describing the pixels this
    string will cover whereas width() returns the distance to where
    the next string should be drawn.

    \sa boundingRect()
*/
int TQFontMetrics::width( const TQString &str, int len ) const
{
    if (len < 0)
	len = str.length();
    if (len == 0)
	return 0;

    int pos = 0;
    int width = 0;
#ifndef TQ_WS_MAC
    const TQChar *ch = str.unicode();

    while (pos < len) {
	unsigned short uc = ch->unicode();
	if (uc < TQFontEngineData::widthCacheSize && d->engineData && d->engineData->widthCache[uc])
	    width += d->engineData->widthCache[uc];
	else {
	    TQFont::Script script;
	    SCRIPT_FOR_CHAR( script, *ch );

	    if (script >= TQFont::Arabic && script <= TQFont::Khmer)
		break;
            if ( ::category( *ch ) != TQChar::Mark_NonSpacing && !qIsZeroWidthChar(ch->unicode())) {
                TQFontEngine *engine = d->engineForScript( script );
#ifdef QT_CHECK_STATE
                Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

                glyph_t glyphs[8];
                advance_t advances[8];
                int nglyphs = 7;
                engine->stringToCMap( ch, 1, glyphs, advances, &nglyphs, false );

                // ### can nglyphs != 1 happen at all? Not currently I think
                if ( uc < TQFontEngineData::widthCacheSize && advances[0] > 0 && advances[0] < 0x100 )
                    d->engineData->widthCache[ uc ] = advances[0];
                width += advances[0];
            }
	}
	++pos;
	++ch;
    }
    if ( pos < len ) {
#endif
	TQTextEngine layout( str, d );
	layout.itemize( TQTextEngine::WidthOnly );
	width += layout.width( pos, len-pos );
#ifndef TQ_WS_MAC
    }
#endif
    return width;
}
#endif

/*! \fn int TQFontMetrics::width( TQChar ch ) const

    <img src="bearings.png" align=right>

    Returns the logical width of character \a ch in pixels. This is a
    distance appropriate for drawing a subsequent character after \a
    ch.

    Some of the metrics are described in the image to the right. The
    central dark rectangles cover the logical width() of each
    character. The outer pale rectangles cover the leftBearing() and
    rightBearing() of each character. Notice that the bearings of "f"
    in this particular font are both negative, while the bearings of
    "o" are both positive.

    \warning This function will produce incorrect results for Arabic
    characters or non spacing marks in the middle of a string, as the
    glyph shaping and positioning of marks that happens when
    processing strings cannot be taken into account. Use charWidth()
    instead if you aren't looking for the width of isolated
    characters.

    \sa boundingRect(), charWidth()
*/

/*! \fn int TQFontMetrics::width( char c ) const

  \overload
  \obsolete

  Provided to aid porting from TQt 1.x.
*/

/*! \fn int TQFontMetrics::charWidth( const TQString &str, int pos ) const
    Returns the width of the character at position \a pos in the
    string \a str.

    The whole string is needed, as the glyph drawn may change
    depending on the context (the letter before and after the current
    one) for some languages (e.g. Arabic).

    This function also takes non spacing marks and ligatures into
    account.
*/

#ifndef TQ_WS_QWS
/*!
    Returns the bounding rectangle of the first \a len characters of
    \a str, which is the set of pixels the text would cover if drawn
    at (0, 0).

    If \a len is negative (the default), the entire string is used.

    Note that the bounding rectangle may extend to the left of (0, 0),
    e.g. for italicized fonts, and that the text output may cover \e
    all pixels in the bounding rectangle.

    Newline characters are processed as normal characters, \e not as
    linebreaks.

    Due to the different actual character heights, the height of the
    bounding rectangle of e.g. "Yes" and "yes" may be different.

    \sa width(), TQPainter::boundingRect()
*/
TQRect TQFontMetrics::boundingRect( const TQString &str, int len ) const
{
    if (len < 0)
	len = str.length();
    if (len == 0)
	return TQRect();

    TQTextEngine layout( str, d );
    layout.itemize( TQTextEngine::NoBidi|TQTextEngine::SingleLine );
    glyph_metrics_t gm = layout.boundingBox( 0, len );
    return TQRect( gm.x, gm.y, gm.width, gm.height );
}
#endif

/*!
    Returns the rectangle that is covered by ink if the character
    specified by \a ch were to be drawn at the origin of the coordinate
    system.

    Note that the bounding rectangle may extend to the left of (0, 0),
    e.g. for italicized fonts, and that the text output may cover \e
    all pixels in the bounding rectangle. For a space character the rectangle
    will usually be empty.

    Note that the rectangle usually extends both above and below the
    base line.
    
    \warning The width of the returned rectangle is not the advance width
    of the character. Use boundingRect(const TQString &) or width() instead.
    
    \sa width() 
*/
TQRect TQFontMetrics::boundingRect( TQChar ch ) const
{
    TQFont::Script script;
    SCRIPT_FOR_CHAR( script, ch );

    TQFontEngine *engine = d->engineForScript( script );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

    glyph_t glyphs[10];
    int nglyphs = 9;
    engine->stringToCMap( &ch, 1, glyphs, 0, &nglyphs, false );
    glyph_metrics_t gi = engine->boundingBox( glyphs[0] );
    return TQRect( gi.x, gi.y, gi.width, gi.height );
}

/*!
    \overload

    Returns the bounding rectangle of the first \a len characters of
    \a str, which is the set of pixels the text would cover if drawn
    at (0, 0). The drawing, and hence the bounding rectangle, is
    constrained to the rectangle (\a x, \a y, \a w, \a h).

    If \a len is negative (which is the default), the entire string is
    used.

    The \a flgs argument is the bitwise OR of the following flags:
    \list
    \i \c AlignAuto aligns to the left border for all languages except
	  Arabic and Hebrew where it aligns to the right.
    \i \c AlignLeft aligns to the left border.
    \i \c AlignRight aligns to the right border.
    \i \c AlignJustify produces justified text.
    \i \c AlignHCenter aligns horizontally centered.
    \i \c AlignTop aligns to the top border.
    \i \c AlignBottom aligns to the bottom border.
    \i \c AlignVCenter aligns vertically centered
    \i \c AlignCenter (== \c{AlignHCenter | AlignVCenter})
    \i \c SingleLine ignores newline characters in the text.
    \i \c ExpandTabs expands tabs (see below)
    \i \c ShowPrefix interprets "&amp;x" as "<u>x</u>", i.e. underlined.
    \i \c WordBreak breaks the text to fit the rectangle.
    \endlist

    Horizontal alignment defaults to \c AlignAuto and vertical
    alignment defaults to \c AlignTop.

    If several of the horizontal or several of the vertical alignment
    flags are set, the resulting alignment is undefined.

    These flags are defined in \c ntqnamespace.h.

    If \c ExpandTabs is set in \a flgs, then: if \a tabarray is
    non-null, it specifies a 0-terminated sequence of pixel-positions
    for tabs; otherwise if \a tabstops is non-zero, it is used as the
    tab spacing (in pixels).

    Note that the bounding rectangle may extend to the left of (0, 0),
    e.g. for italicized fonts, and that the text output may cover \e
    all pixels in the bounding rectangle.

    Newline characters are processed as linebreaks.

    Despite the different actual character heights, the heights of the
    bounding rectangles of "Yes" and "yes" are the same.

    The bounding rectangle given by this function is somewhat larger
    than that calculated by the simpler boundingRect() function. This
    function uses the \link minLeftBearing() maximum left \endlink and
    \link minRightBearing() right \endlink font bearings as is
    necessary for multi-line text to align correctly. Also,
    fontHeight() and lineSpacing() are used to calculate the height,
    rather than individual character heights.

    The \a intern argument should not be used.

    \sa width(), TQPainter::boundingRect(), TQt::AlignmentFlags
*/
TQRect TQFontMetrics::boundingRect( int x, int y, int w, int h, int flgs,
				  const TQString& str, int len, int tabstops,
				  int *tabarray, TQTextParag **intern ) const
{
    if ( len < 0 )
	len = str.length();

    int tabarraylen=0;
    if (tabarray)
	while (tabarray[tabarraylen])
	    tabarraylen++;

    TQRect rb;
    TQRect r(x, y, w, h);
    qt_format_text( TQFont( d, d->paintdevice ), r, flgs|TQt::DontPrint, str, len, &rb,
		    tabstops, tabarray, tabarraylen, intern, 0 );

    return rb;
}

/*!
    Returns the size in pixels of the first \a len characters of \a
    str.

    If \a len is negative (the default), the entire string is used.

    The \a flgs argument is the bitwise OR of the following flags:
    \list
    \i \c SingleLine ignores newline characters.
    \i \c ExpandTabs expands tabs (see below)
    \i \c ShowPrefix interprets "&amp;x" as "<u>x</u>", i.e. underlined.
    \i \c WordBreak breaks the text to fit the rectangle.
    \endlist

    These flags are defined in \c ntqnamespace.h.

    If \c ExpandTabs is set in \a flgs, then: if \a tabarray is
    non-null, it specifies a 0-terminated sequence of pixel-positions
    for tabs; otherwise if \a tabstops is non-zero, it is used as the
    tab spacing (in pixels).

    Newline characters are processed as linebreaks.

    Despite the different actual character heights, the heights of the
    bounding rectangles of "Yes" and "yes" are the same.

    The \a intern argument should not be used.

    \sa boundingRect()
*/
TQSize TQFontMetrics::size( int flgs, const TQString &str, int len, int tabstops,
			  int *tabarray, TQTextParag **intern ) const
{
    return boundingRect(0,0,0,0,flgs,str,len,tabstops,tabarray,intern).size();
}

/*!
    Returns the distance from the base line to where an underscore
    should be drawn.

    \sa overlinePos(), strikeOutPos(), lineWidth()
*/
int TQFontMetrics::underlinePos() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

    return engine->underlinePosition();
}

/*!
    Returns the distance from the base line to where an overline
    should be drawn.

    \sa underlinePos(), strikeOutPos(), lineWidth()
*/
int TQFontMetrics::overlinePos() const
{
    int pos = ascent() + 1;
    return pos > 0 ? pos : 1;
}

/*!
    Returns the distance from the base line to where the strikeout
    line should be drawn.

    \sa underlinePos(), overlinePos(), lineWidth()
*/
int TQFontMetrics::strikeOutPos() const
{
    int pos = ascent() / 3;
    return pos > 0 ? pos : 1;
}

/*!
    Returns the width of the underline and strikeout lines, adjusted
    for the point size of the font.

    \sa underlinePos(), overlinePos(), strikeOutPos()
*/
int TQFontMetrics::lineWidth() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

    return engine->lineThickness();
}




/*****************************************************************************
  TQFontInfo member functions
 *****************************************************************************/

/*!
    \class TQFontInfo ntqfontinfo.h

    \brief The TQFontInfo class provides general information about fonts.

    \ingroup graphics
    \ingroup shared

    The TQFontInfo class provides the same access functions as TQFont,
    e.g. family(), pointSize(), italic(), weight(), fixedPitch(),
    styleHint() etc. But whilst the TQFont access functions return the
    values that were set, a TQFontInfo object returns the values that
    apply to the font that will actually be used to draw the text.

    For example, when the program asks for a 25pt Courier font on a
    machine that has a non-scalable 24pt Courier font, TQFont will
    (normally) use the 24pt Courier for rendering. In this case,
    TQFont::pointSize() returns 25 and TQFontInfo::pointSize() returns
    24.

    There are three ways to create a TQFontInfo object.
    \list 1
    \i Calling the TQFontInfo constructor with a TQFont creates a font
    info object for a screen-compatible font, i.e. the font cannot be
    a printer font<sup>*</sup>. If the font is changed later, the font
    info object is \e not updated.

    \i TQWidget::fontInfo() returns the font info for a widget's font.
    This is equivalent to calling TQFontInfo(widget->font()). If the
    widget's font is changed later, the font info object is \e not
    updated.

    \i TQPainter::fontInfo() returns the font info for a painter's
    current font. If the painter's font is changed later, the font
    info object is \e not updated.
    \endlist

    <sup>*</sup> If you use a printer font the values returned may be
    inaccurate. Printer fonts are not always accessible so the nearest
    screen font is used if a printer font is supplied.

    \sa TQFont TQFontMetrics TQFontDatabase
*/

/*!
    Constructs a font info object for \a font.

    The font must be screen-compatible, i.e. a font you use when
    drawing text in \link TQWidget widgets\endlink or \link TQPixmap
    pixmaps\endlink, not TQPicture or TQPrinter.

    The font info object holds the information for the font that is
    passed in the constructor at the time it is created, and is not
    updated if the font's attributes are changed later.

    Use TQPainter::fontInfo() to get the font info when painting.
    This will give correct results also when painting on paint device
    that is not screen-compatible.
*/
TQFontInfo::TQFontInfo( const TQFont &font )
    : d( font.d ), painter( 0 ), fscript( TQFont::NoScript )
{
    d->ref();
}

/*!
    Constructs a font info object for \a font using the specified \a
    script.
*/
TQFontInfo::TQFontInfo( const TQFont &font, TQFont::Script script )
    : d( font.d ), painter( 0 ), fscript( script )
{
    d->ref();
}

/*! \internal

  Constructs a font info object from the painter's font \a p.
*/
TQFontInfo::TQFontInfo( const TQPainter *p )
    : painter( 0 ), fscript( TQFont::NoScript )
{
    TQPainter *painter = (TQPainter *) p;

#if defined(CHECK_STATE)
    if ( !painter->isActive() )
	tqWarning( "TQFontInfo: Get font info between TQPainter::begin() "
		  "and TQPainter::end()" );
#endif

    painter->setf( TQPainter::FontInf );
    if ( painter->testf(TQPainter::DirtyFont) )
	painter->updateFont();
    if ( painter->pfont )
	d = painter->pfont->d;
    else
	d = painter->cfont.d;
    d->ref();
}

/*!
    Constructs a copy of \a fi.
*/
TQFontInfo::TQFontInfo( const TQFontInfo &fi )
    : d(fi.d), painter(0), fscript( fi.fscript )
{
    d->ref();
}

/*!
    Destroys the font info object.
*/
TQFontInfo::~TQFontInfo()
{
    if ( d->deref() )
	delete d;
}

/*!
    Assigns the font info in \a fi.
*/
TQFontInfo &TQFontInfo::operator=( const TQFontInfo &fi )
{
    if ( d != fi.d ) {
	if ( d->deref() )
	    delete d;
	d = fi.d;
	d->ref();
    }
    painter = 0;
    fscript = fi.fscript;
    return *this;
}

/*!
    Returns the family name of the matched window system font.

    \sa TQFont::family()
*/
TQString TQFontInfo::family() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE
    return engine->fontDef.family;
}

/*!
    Returns the point size of the matched window system font.

    \sa TQFont::pointSize()
*/
int TQFontInfo::pointSize() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE
    return ( engine->fontDef.pointSize + 5 ) / 10;
}

/*!
    Returns the pixel size of the matched window system font.

    \sa TQFont::pointSize()
*/
int TQFontInfo::pixelSize() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE
    return engine->fontDef.pixelSize;
}

/*!
    Returns the italic value of the matched window system font.

    \sa TQFont::italic()
*/
bool TQFontInfo::italic() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE
    return engine->fontDef.italic;
}

/*!
    Returns the weight of the matched window system font.

    \sa TQFont::weight(), bold()
*/
int TQFontInfo::weight() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE
    return engine->fontDef.weight;

}

/*!
    \fn bool TQFontInfo::bold() const

    Returns true if weight() would return a value greater than \c
    TQFont::Normal; otherwise returns false.

    \sa weight(), TQFont::bold()
*/

/*!
    Returns the underline value of the matched window system font.

  \sa TQFont::underline()

  \internal

  Here we read the underline flag directly from the TQFont.
  This is OK for X11 and for Windows because we always get what we want.
*/
bool TQFontInfo::underline() const
{
    return d->underline;
}

/*!
    Returns the overline value of the matched window system font.

    \sa TQFont::overline()

    \internal

    Here we read the overline flag directly from the TQFont.
    This is OK for X11 and for Windows because we always get what we want.
*/
bool TQFontInfo::overline() const
{
    return d->overline;
}

/*!
    Returns the strikeout value of the matched window system font.

  \sa TQFont::strikeOut()

  \internal Here we read the strikeOut flag directly from the TQFont.
  This is OK for X11 and for Windows because we always get what we want.
*/
bool TQFontInfo::strikeOut() const
{
    return d->strikeOut;
}

/*!
    Returns the fixed pitch value of the matched window system font.

    \sa TQFont::fixedPitch()
*/
bool TQFontInfo::fixedPitch() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE
#ifdef Q_OS_MAC
    if (!engine->fontDef.fixedPitchComputed) {
	TQChar ch[2] = { TQChar('i'), TQChar('m') };
	glyph_t g[2];
	int l = 2;
	advance_t a[2];
	engine->stringToCMap(ch, 2, g, a, &l, false);
	engine->fontDef.fixedPitch = a[0] == a[1];
	engine->fontDef.fixedPitchComputed = true;
    }
#endif
    return engine->fontDef.fixedPitch;
}

/*!
    Returns the style of the matched window system font.

    Currently only returns the style hint set in TQFont.

    \sa TQFont::styleHint() TQFont::StyleHint
*/
TQFont::StyleHint TQFontInfo::styleHint() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE
    return (TQFont::StyleHint) engine->fontDef.styleHint;
}

/*!
    Returns true if the font is a raw mode font; otherwise returns
    false.

    If it is a raw mode font, all other functions in TQFontInfo will
    return the same values set in the TQFont, regardless of the font
    actually used.

    \sa TQFont::rawMode()
*/
bool TQFontInfo::rawMode() const
{
    return d->rawMode;
}

/*!
    Returns true if the matched window system font is exactly the same
    as the one specified by the font; otherwise returns false.

    \sa TQFont::exactMatch()
*/
bool TQFontInfo::exactMatch() const
{
    TQFontEngine *engine = d->engineForScript( (TQFont::Script) fscript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

    return d->rawMode ? engine->type() != TQFontEngine::Box
			     : d->request == engine->fontDef;
}




// **********************************************************************
// TQFontCache
// **********************************************************************

#ifdef TQFONTCACHE_DEBUG
// fast timeouts for debugging
static const int fast_timeout =   1000;  // 1s
static const int slow_timeout =   5000;  // 5s
#else
static const int fast_timeout =  10000; // 10s
static const int slow_timeout = 300000; //  5m
#endif // TQFONTCACHE_DEBUG

TQFontCache *TQFontCache::instance = 0;
const uint TQFontCache::min_cost = 4*1024; // 4mb

static TQSingleCleanupHandler<TQFontCache> cleanup_fontcache;


TQFontCache::TQFontCache()
    : TQObject( tqApp, "global font cache" ), total_cost( 0 ), max_cost( min_cost ),
      current_timestamp( 0 ), fast( false ), timer_id( -1 )
{
    Q_ASSERT( instance == 0 );
    instance = this;
    cleanup_fontcache.set( &instance );
}

TQFontCache::~TQFontCache()
{
    {
	EngineDataCache::Iterator it = engineDataCache.begin(),
				 end = engineDataCache.end();
	while ( it != end ) {
	    if ( it.data()->count == 0 )
		delete it.data();
	    else
		FC_DEBUG("TQFontCache::~TQFontCache: engineData %p still has refcount %d",
			 it.data(), it.data()->count);
	    ++it;
	}
    }
    EngineCache::Iterator it = engineCache.begin(),
			 end = engineCache.end();
    while ( it != end ) {
	if ( it.data().data->count == 0 ) {
	    if ( --it.data().data->cache_count == 0 ) {
		FC_DEBUG("TQFontCache::~TQFontCache: deleting engine %p key=(%d / %d %d %d %d %d)",
			 it.data().data, it.key().script, it.key().def.pointSize,
			 it.key().def.pixelSize, it.key().def.weight, it.key().def.italic,
			 it.key().def.fixedPitch);

		delete it.data().data;
	    }
	} else {
	    FC_DEBUG("TQFontCache::~TQFontCache: engine = %p still has refcount %d",
		     it.data().data, it.data().data->count);
	}
	++it;
    }
    instance = 0;
}

#ifdef TQ_WS_QWS
void TQFontCache::clear()
{
    {
	EngineDataCache::Iterator it = engineDataCache.begin(),
				 end = engineDataCache.end();
	while ( it != end ) {
	    TQFontEngineData *data = it.data();
	    if ( data->engine )
		data->engine->deref();
	    data->engine = 0;
	    ++it;
	}
    }

    EngineCache::Iterator it = engineCache.begin(),
			 end = engineCache.end();
    while ( it != end ) {
	if ( it.data().data->count == 0 ) {
	    if ( --it.data().data->cache_count == 0 ) {
		FC_DEBUG("TQFontCache::~TQFontCache: deleting engine %p key=(%d / %d %d %d %d %d)",
			 it.data().data, it.key().script, it.key().def.pointSize,
			 it.key().def.pixelSize, it.key().def.weight, it.key().def.italic,
			 it.key().def.fixedPitch);
		delete it.data().data;
	    }
	} else {
	    FC_DEBUG("TQFontCache::~TQFontCache: engine = %p still has refcount %d",
		     it.data().data, it.data().data->count);
	}
	++it;
    }
}
#endif

TQFontEngineData *TQFontCache::findEngineData( const Key &key ) const
{
    EngineDataCache::ConstIterator it = engineDataCache.find( key ),
				  end = engineDataCache.end();
    if ( it == end ) return 0;

    // found
    return it.data();
}

void TQFontCache::insertEngineData( const Key &key, TQFontEngineData *engineData )
{
    FC_DEBUG( "TQFontCache: inserting new engine data %p", engineData );

    engineDataCache.insert( key, engineData );
    increaseCost( sizeof( TQFontEngineData ) );
}

TQFontEngine *TQFontCache::findEngine( const Key &key )
{
    EngineCache::Iterator it = engineCache.find( key ),
			 end = engineCache.end();
    if ( it == end ) return 0;

    // found... update the hitcount and timestamp
    it.data().hits++;
    it.data().timestamp = ++current_timestamp;

    FC_DEBUG( "TQFontCache: found font engine\n"
	    "  %p: timestamp %4u hits %3u ref %2d/%2d, type '%s'",
	    it.data().data, it.data().timestamp, it.data().hits,
	    it.data().data->count, it.data().data->cache_count,
	    it.data().data->name() );

    return it.data().data;
}

void TQFontCache::insertEngine( const Key &key, TQFontEngine *engine )
{
    FC_DEBUG( "TQFontCache: inserting new engine %p", engine );

    Engine data( engine );
    data.timestamp = ++current_timestamp;

    engineCache.insert( key, data );

    // only increase the cost if this is the first time we insert the engine
    if ( engine->cache_count == 0 )
	increaseCost( engine->cache_cost );

    ++engine->cache_count;
}

void TQFontCache::increaseCost( uint cost )
{
    cost = ( cost + 512 ) / 1024; // store cost in kb
    cost = cost > 0 ? cost : 1;
    total_cost += cost;

    FC_DEBUG( "  COST: increased %u kb, total_cost %u kb, max_cost %u kb",
	    cost, total_cost, max_cost );

    if ( total_cost > max_cost) {
	max_cost = total_cost;

	if ( timer_id == -1 || ! fast ) {
	    FC_DEBUG( "  TIMER: starting fast timer (%d ms)", fast_timeout );

	    if (timer_id != -1) killTimer( timer_id );
	    timer_id = startTimer( fast_timeout );
	    fast = true;
	}
    }
}

void TQFontCache::decreaseCost( uint cost )
{
    cost = ( cost + 512 ) / 1024; // cost is stored in kb
    cost = cost > 0 ? cost : 1;
    Q_ASSERT( cost <= total_cost );
    total_cost -= cost;

    FC_DEBUG( "  COST: decreased %u kb, total_cost %u kb, max_cost %u kb",
	    cost, total_cost, max_cost );
}

#if defined(TQ_WS_WIN ) || defined (TQ_WS_QWS)
void TQFontCache::cleanupPrinterFonts()
{
    FC_DEBUG( "TQFontCache::cleanupPrinterFonts" );

    {
	FC_DEBUG( "  CLEAN engine data:" );

	// clean out all unused engine datas
	EngineDataCache::Iterator it = engineDataCache.begin(),
				 end = engineDataCache.end();
	while ( it != end ) {
	    if ( it.key().screen == 0 ) {
		++it;
		continue;
	    }

	    if( it.data()->count > 0 ) {
#ifdef TQ_WS_WIN
		for(int i = 0; i < TQFont::LastPrivateScript; ++i) {
		    if( it.data()->engines[i] ) {
			it.data()->engines[i]->deref();
			it.data()->engines[i] = 0;
		    }
		}
#else
		if ( it.data()->engine ) {
		    it.data()->engine->deref();
		    it.data()->engine = 0;
		}
#endif
		++it;
	    } else {

		EngineDataCache::Iterator rem = it++;

		decreaseCost( sizeof( TQFontEngineData ) );

		FC_DEBUG( "    %p", rem.data() );

		delete rem.data();
		engineDataCache.remove( rem );
	    }
	}
    }

    EngineCache::Iterator it = engineCache.begin(),
			 end = engineCache.end();
    while( it != end ) {
	if ( it.data().data->count > 0 || it.key().screen == 0) {
	    ++it;
	    continue;
	}

	FC_DEBUG( "    %p: timestamp %4u hits %2u ref %2d/%2d, type '%s'",
		  it.data().data, it.data().timestamp, it.data().hits,
		  it.data().data->count, it.data().data->cache_count,
		  it.data().data->name() );

	if ( --it.data().data->cache_count == 0 ) {
	    FC_DEBUG( "    DELETE: last occurence in cache" );

	    decreaseCost( it.data().data->cache_cost );
	    delete it.data().data;
	}

	engineCache.remove( it++ );
    }
}
#endif

void TQFontCache::timerEvent( TQTimerEvent * )
{
    FC_DEBUG( "TQFontCache::timerEvent: performing cache maintenance (timestamp %u)",
	      current_timestamp );

    if ( total_cost <= max_cost && max_cost <= min_cost ) {
	FC_DEBUG( "  cache redused sufficiently, stopping timer" );

	killTimer( timer_id );
	timer_id = -1;
	fast = false;

	return;
    }

    // go through the cache and count up everything in use
    uint in_use_cost = 0;

    {
	FC_DEBUG( "  SWEEP engine data:" );

	// make sure the cost of each engine data is at least 1kb
        const uint engine_data_cost =
	    sizeof( TQFontEngineData ) > 1024 ? sizeof( TQFontEngineData ) : 1024;

	EngineDataCache::ConstIterator it = engineDataCache.begin(),
	                              end = engineDataCache.end();
	for ( ; it != end; ++it ) {
#ifdef TQFONTCACHE_DEBUG
	    FC_DEBUG( "    %p: ref %2d", it.data(), it.data()->count );

#  if defined(TQ_WS_X11) || defined(TQ_WS_WIN)
	    // print out all engines
	    for ( int i = 0; i < TQFont::LastPrivateScript; ++i ) {
		if ( ! it.data()->engines[i] ) continue;
		FC_DEBUG( "      contains %p", it.data()->engines[i] );
	    }
#  endif // TQ_WS_X11 || TQ_WS_WIN
#endif // TQFONTCACHE_DEBUG

	    if ( it.data()->count > 0 )
		in_use_cost += engine_data_cost;
	}
    }

    {
	FC_DEBUG( "  SWEEP engine:" );

	EngineCache::ConstIterator it = engineCache.begin(),
				  end = engineCache.end();
	for ( ; it != end; ++it ) {
	    FC_DEBUG( "    %p: timestamp %4u hits %2u ref %2d/%2d, cost %u bytes",
		      it.data().data, it.data().timestamp, it.data().hits,
		      it.data().data->count, it.data().data->cache_count,
		      it.data().data->cache_cost );

	    if ( it.data().data->count > 0 )
		in_use_cost += it.data().data->cache_cost / it.data().data->cache_count;
	}

	// attempt to make up for rounding errors
	in_use_cost += (uint)engineCache.count();
    }

    in_use_cost = ( in_use_cost + 512 ) / 1024; // cost is stored in kb

    /*
      calculate the new maximum cost for the cache

      NOTE: in_use_cost is *not* correct due to rounding errors in the
      above algorithm.  instead of worrying about getting the
      calculation correct, we are more interested in speed, and use
      in_use_cost as a floor for new_max_cost
    */
    uint new_max_cost = TQMAX( TQMAX( max_cost / 2, in_use_cost ), min_cost );

    FC_DEBUG( "  after sweep, in use %u kb, total %u kb, max %u kb, new max %u kb",
	      in_use_cost, total_cost, max_cost, new_max_cost );

    if ( new_max_cost == max_cost ) {
	if ( fast ) {
	    FC_DEBUG( "  cannot shrink cache, slowing timer" );

	    killTimer( timer_id );
	    timer_id = startTimer( slow_timeout );
	    fast = false;
	}

	return;
    } else if ( ! fast ) {
	FC_DEBUG( "  dropping into passing gear" );

	killTimer( timer_id );
	timer_id = startTimer( fast_timeout );
	fast = true;
    }

    max_cost = new_max_cost;

    {
	FC_DEBUG( "  CLEAN engine data:" );

	// clean out all unused engine datas
	EngineDataCache::Iterator it = engineDataCache.begin(),
				 end = engineDataCache.end();
	while ( it != end ) {
	    if ( it.data()->count > 0 ) {
		++it;
		continue;
	    }

	    EngineDataCache::Iterator rem = it++;

	    decreaseCost( sizeof( TQFontEngineData ) );

	    FC_DEBUG( "    %p", rem.data() );

	    delete rem.data();
	    engineDataCache.remove( rem );
	}
    }

    // clean out the engine cache just enough to get below our new max cost
    uint current_cost;
    do {
	current_cost = total_cost;

	EngineCache::Iterator it = engineCache.begin(),
			     end = engineCache.end();
	// determine the oldest and least popular of the unused engines
	uint oldest = ~0;
	uint least_popular = ~0;

	for ( ; it != end; ++it ) {
	    if ( it.data().data->count > 0 ) continue;

	    if ( it.data().timestamp < oldest &&
		 it.data().hits <= least_popular ) {
		oldest = it.data().timestamp;
		least_popular = it.data().hits;
	    }
	}

	FC_DEBUG( "    oldest %u least popular %u", oldest, least_popular );

	for ( it = engineCache.begin(); it != end; ++it ) {
	    if ( it.data().data->count == 0 &&
		 it.data().timestamp == oldest &&
		 it.data().hits == least_popular)
		break;
	}

	if ( it != end ) {
	    FC_DEBUG( "    %p: timestamp %4u hits %2u ref %2d/%2d, type '%s'",
		      it.data().data, it.data().timestamp, it.data().hits,
		      it.data().data->count, it.data().data->cache_count,
		      it.data().data->name() );

	    if ( --it.data().data->cache_count == 0 ) {
		FC_DEBUG( "    DELETE: last occurence in cache" );

		decreaseCost( it.data().data->cache_cost );
		delete it.data().data;
	    } else {
		/*
		  this particular font engine is in the cache multiple
		  times...  set current_cost to zero, so that we can
		  keep looping to get rid of all occurences
		*/
		current_cost = 0;
	    }

	    engineCache.remove( it );
	}
    } while ( current_cost != total_cost && total_cost > max_cost );
}
