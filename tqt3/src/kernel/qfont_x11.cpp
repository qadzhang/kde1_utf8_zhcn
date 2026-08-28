/****************************************************************************
**
** Implementation of TQFont, TQFontMetrics and TQFontInfo classes for X11
**
** Created : 940515
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

// REVISED: brad

#include "qplatformdefs.h"

#include "ntqfont.h"
#include "ntqapplication.h"
#include "ntqcleanuphandler.h"
#include "ntqfontinfo.h"
#include "ntqfontdatabase.h"
#include "ntqfontmetrics.h"
#include "ntqpaintdevice.h"
#include "ntqpaintdevicemetrics.h"
#include "ntqtextcodec.h"

#include <private/qfontcodecs_p.h>
#include <private/qunicodetables_p.h>
#include "qfontdata_p.h"
#include "qfontengine_p.h"
#include "qtextengine_p.h"

#include "qt_x11_p.h"

#include <time.h>
#include <stdlib.h>
#include <ctype.h>

#define TQFONTLOADER_DEBUG
#define TQFONTLOADER_DEBUG_VERBOSE

TQ_EXPORT bool tqt_has_xft = false;

#ifndef TQT_NO_XFTFREETYPE
TQt::HANDLE qt_xft_handle(const TQFont &font)
{
    TQFontEngine *engine = font.d->engineForScript( TQFontPrivate::defaultScript );
    if (engine->type() != TQFontEngine::Xft)
        return 0;
    return (long)static_cast<TQFontEngineXft *>(engine)->font();
}
#endif

double tqt_pixelSize(double pointSize, TQPaintDevice *paintdevice, int scr)
{
    if (pointSize < 0) return -1.;

    double result = pointSize;
    if (paintdevice && TQPaintDeviceMetrics( paintdevice ).logicalDpiY() != 75)
	result *= TQPaintDeviceMetrics( paintdevice ).logicalDpiY() / 72.;
    else if (TQPaintDevice::x11AppDpiY( scr ) != 75)
	result *= TQPaintDevice::x11AppDpiY( scr ) / 72.;

    return result;
}

double tqt_pointSize(double pixelSize, TQPaintDevice *paintdevice, int scr)
{
    if (pixelSize < 0) return -1.;

    double result = pixelSize;
    if ( paintdevice && TQPaintDeviceMetrics( paintdevice ).logicalDpiY() != 75)
	result *= 72. / TQPaintDeviceMetrics( paintdevice ).logicalDpiY();
    else if (TQPaintDevice::x11AppDpiY(scr) != 75)
	result *= 72. / TQPaintDevice::x11AppDpiY( scr );

    return result;
}

static inline double pixelSize( const TQFontDef &request, TQPaintDevice *paintdevice,
				int scr )
{
    return ((request.pointSize != -1) ?
	    tqt_pixelSize(request.pointSize / 10., paintdevice, scr) :
	    (double)request.pixelSize);
}

static inline double pointSize( const TQFontDef &request, TQPaintDevice *paintdevice,
				int scr )
{
    return ((request.pixelSize != -1) ?
	    tqt_pointSize(request.pixelSize, paintdevice, scr) * 10.:
	    (double)request.pointSize);
}


/*
  Removes wildcards from an XLFD.

  Returns \a xlfd with all wildcards removed if a match for \a xlfd is
  found, otherwise it returns \a xlfd.
*/
static TQCString tqt_fixXLFD( const TQCString &xlfd )
{
    TQCString ret = xlfd;
    int count = 0;
    char **fontNames =
	XListFonts( TQPaintDevice::x11AppDisplay(), xlfd, 32768, &count );
    if ( count > 0 )
	ret = fontNames[0];
    XFreeFontNames( fontNames );
    return ret ;
}

typedef TQMap<TQFont::Script,TQString> FallbackMap;
static FallbackMap *fallbackMap = 0;
static TQSingleCleanupHandler<FallbackMap> tqt_fallback_font_family_cleanup;

static void ensure_fallback_map()
{
    if ( fallbackMap ) return;
    fallbackMap = new FallbackMap;
    tqt_fallback_font_family_cleanup.set( &fallbackMap );
}

// Returns the user-configured fallback family for the specified script.
TQString tqt_fallback_font_family( TQFont::Script script )
{
    TQString ret;

    if ( fallbackMap ) {
	FallbackMap::ConstIterator it, end = fallbackMap->end();
	it = fallbackMap->find( script );
	if ( it != end )
	    ret = it.data();
    }

    return ret;
}

// Sets the fallback family for the specified script.
void tqt_set_fallback_font_family( TQFont::Script script, const TQString &family )
{
    ensure_fallback_map();

    if ( ! family.isEmpty() )
	fallbackMap->insert( script, family );
    else
	fallbackMap->remove( script );
}


TQFont::Script TQFontPrivate::defaultScript = TQFont::UnknownScript;
int TQFontPrivate::defaultEncodingID = -1;

/*!
  Internal function that initializes the font system.

  \internal
  The font cache and font dict do not alloc the keys. The key is a TQString
  which is shared between TQFontPrivate and TQXFontName.
*/
void TQFont::initialize()
{
    // create global font cache
    if ( ! TQFontCache::instance ) (void) new TQFontCache;

#ifndef TQT_NO_CODECS
#ifndef TQT_NO_BIG_CODECS
    static bool codecs_once = false;
    if ( ! codecs_once ) {
	(void) new TQFontJis0201Codec;
	(void) new TQFontJis0208Codec;
	(void) new TQFontKsc5601Codec;
	(void) new TQFontGb2312Codec;
	(void) new TQFontGbkCodec;
	(void) new TQFontGb18030_0Codec;
	(void) new TQFontBig5Codec;
	(void) new TQFontBig5hkscsCodec;
	(void) new TQFontLaoCodec;
	codecs_once = true;
    }
#endif // TQT_NO_BIG_CODECS
#endif // TQT_NO_CODECS

    extern int tqt_encoding_id_for_mib( int mib ); // from qfontdatabase_x11.cpp
    TQTextCodec *codec = TQTextCodec::codecForLocale();
    // determine the default encoding id using the locale, otherwise
    // fallback to latin1 ( mib == 4 )
    int mib = codec ? codec->mibEnum() : 4;

    // for asian locales, use the mib for the font codec instead of the locale codec
    switch (mib) {
    case 38: // eucKR
	mib = 36;
	break;

    case 2025: // GB2312
	mib = 57;
	break;

    case 113: // GBK
	mib = -113;
	break;

    case 114: // GB18030
	mib = -114;
	break;

    case 2026: // Big5
	mib = -2026;
	break;

    case 2101: // Big5-HKSCS
	mib = -2101;
	break;

    case 16: // JIS7
	mib = 15;
	break;

    case 17: // SJIS
    case 18: // eucJP
	mib = 63;
	break;
    }

    // get the default encoding id for the locale encoding...
    TQFontPrivate::defaultEncodingID = tqt_encoding_id_for_mib( mib );

    // get some sample text based on the users locale. we use this to determine the
    // default script for the font system
    TQCString oldlctime = setlocale(LC_TIME, 0);
    TQCString lctime = setlocale(LC_TIME, "");

    time_t ttmp = time(0);
    struct tm *tt = 0;
    char samp[64];
    TQString sample;

    if ( ttmp != -1 ) {
#if defined(TQT_THREAD_SUPPORT) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
	// use the reentrant versions of localtime() where available
	tm res;
	tt = localtime_r( &ttmp, &res );
#else
	tt = localtime( &ttmp );
#endif // TQT_THREAD_SUPPORT && _POSIX_THREAD_SAFE_FUNCTIONS

	if ( tt != 0 && strftime( samp, 64, "%A%B", tt ) > 0 )
	    if ( codec )
		sample = codec->toUnicode( samp );
    }

    if ( ! sample.isNull() && ! sample.isEmpty() ) {
	TQFont::Script cs = TQFont::NoScript, tmp;
	const TQChar *uc = sample.unicode();
	TQFontPrivate *priv = new TQFontPrivate;

	for ( uint i = 0; i < sample.length(); i++ ) {
	    SCRIPT_FOR_CHAR( tmp, *uc );
	    uc++;
	    if ( tmp != cs && tmp != TQFont::UnknownScript ) {
		cs = tmp;
		break;
	    }
	}
	delete priv;

	if ( cs != TQFont::UnknownScript )
	    TQFontPrivate::defaultScript = cs;
    }

    setlocale( LC_TIME, oldlctime.data() );
}

/*! \internal

  Internal function that cleans up the font system.
*/
void TQFont::cleanup()
{
    // delete the global font cache
    delete TQFontCache::instance;
    TQFontCache::instance = 0;
}

/*!
  \internal
  X11 Only: Returns the screen with which this font is associated.
*/
int TQFont::x11Screen() const
{
    return d->screen;
}

/*! \internal
    X11 Only: Associate the font with the specified \a screen.
*/
void TQFont::x11SetScreen( int screen )
{
    if ( screen < 0 ) // assume default
	screen = TQPaintDevice::x11AppScreen();

    if ( screen == d->screen )
	return; // nothing to do

    detach();
    d->screen = screen;
}

/*! \internal
    Returns a TQFontEngine for the specified \a script that matches the
    TQFontDef \e request member variable.
*/
void TQFontPrivate::load( TQFont::Script script )
{
    // NOTE: the X11 and Windows implementations of this function are
    // identical... if you change one, change both.

#ifdef QT_CHECK_STATE
    // sanity checks
    if (!TQFontCache::instance)
	tqWarning("Must construct a TQApplication before a TQFont");
    Q_ASSERT( script >= 0 && script < TQFont::LastPrivateScript );
#endif // QT_CHECK_STATE

    TQFontDef req = request;
    req.pixelSize = tqRound(pixelSize(req, paintdevice, screen));
    req.pointSize = 0;

    if ( ! engineData ) {
	TQFontCache::Key key( req, TQFont::NoScript, screen, paintdevice );

	// look for the requested font in the engine data cache
	engineData = TQFontCache::instance->findEngineData( key );

	if ( ! engineData ) {
	    // create a new one
	    engineData = new TQFontEngineData;
	    TQFontCache::instance->insertEngineData( key, engineData );
	} else {
	    engineData->ref();
	}
    }

    // the cached engineData could have already loaded the engine we want
    if ( engineData->engines[script] ) return;

    // load the font
    TQFontEngine *engine = 0;
    //    double scale = 1.0; // ### TODO: fix the scale calculations

    // list of families to try
    TQStringList family_list;

    if (!req.family.isEmpty()) {
	family_list = TQStringList::split( ',', req.family );

	// append the substitute list for each family in family_list
	TQStringList subs_list;
	TQStringList::ConstIterator it = family_list.begin(), end = family_list.end();
	for ( ; it != end; ++it )
	    subs_list += TQFont::substitutes( *it );
	family_list += subs_list;
    }

    // null family means find the first font matching the specified script
    family_list << TQString::null;

    TQStringList::ConstIterator it = family_list.begin(), end = family_list.end();
    for ( ; ! engine && it != end; ++it ) {
	req.family = *it;

	engine = TQFontDatabase::findFont( script, this, req );
	if ( engine ) {
	    if ( engine->type() != TQFontEngine::Box )
		break;

	    if ( ! req.family.isEmpty() )
		engine = 0;

	    continue;
	}
    }

    engine->ref();
    engineData->engines[script] = engine;
}

/*!
    Returns true if the font attributes have been changed and the font
    has to be (re)loaded; otherwise returns false.
*/
bool TQFont::dirty() const
{
    return d->engineData == 0;
}

/*!
    Returns the window system handle to the font, for low-level
    access. Using this function is \e not portable.
*/
TQt::HANDLE TQFont::handle() const
{
    TQFontEngine *engine = d->engineForScript( TQFontPrivate::defaultScript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

    switch ( engine->type() ) {
    case TQFontEngine::XLFD:
	return ((TQFontEngineXLFD *) engine)->handle();
    case TQFontEngine::LatinXLFD:
	return ((TQFontEngineLatinXLFD *) engine)->handle();

    default: break;
    }
    return 0;
}

/*!
    Returns the name of the font within the underlying window system.

    On Windows, this is usually just the family name of a TrueType
    font.

    On X11, it is an XLFD (X Logical Font Description).  When TQt is
    build with Xft support on X11, the return value can be an Xft
    pattern or an XLFD.

    Using the return value of this function is usually \e not \e
    portable.

    \sa setRawName()
*/
TQString TQFont::rawName() const
{
    TQFontEngine *engine = d->engineForScript( TQFontPrivate::defaultScript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

    return TQString::fromLatin1( engine->name() );
}

/*!
    Sets a font by its system specific name. The function is
    particularly useful under X, where system font settings (for
    example X resources) are usually available in XLFD (X Logical Font
    Description) form only. You can pass an XLFD as \a name to this
    function.

    A font set with setRawName() is still a full-featured TQFont. It can
    be queried (for example with italic()) or modified (for example with
    setItalic()) and is therefore also suitable for rendering rich text.

    If TQt's internal font database cannot resolve the raw name, the
    font becomes a raw font with \a name as its family.

    Note that the present implementation does not handle wildcards in
    XLFDs well, and that font aliases (file \c fonts.alias in the font
    directory on X11) are not supported.

    \sa rawName(), setRawMode(), setFamily()
*/
void TQFont::setRawName( const TQString &name )
{
    detach();

    // from qfontdatabase_x11.cpp
    extern bool tqt_fillFontDef( const TQCString &xlfd, TQFontDef *fd, int screen );

    if ( ! tqt_fillFontDef( tqt_fixXLFD( name.latin1() ), &d->request, d->screen ) ) {
#ifdef QT_CHECK_STATE
	tqWarning("TQFont::setRawName(): Invalid XLFD: \"%s\"", name.latin1());
#endif // QT_CHECK_STATE

	setFamily( name );
	setRawMode( true );
    } else {
	d->mask = TQFontPrivate::Complete;
    }
}

/*!
    Returns the "last resort" font family name.

    The current implementation tries a wide variety of common fonts,
    returning the first one it finds. Is is possible that no family is
    found in which case a null string is returned.

    \sa lastResortFont()
*/
TQString TQFont::lastResortFamily() const
{
    return TQString::fromLatin1( "Helvetica" );
}

/*!
    Returns the family name that corresponds to the current style
    hint.

    \sa StyleHint styleHint() setStyleHint()
*/
TQString TQFont::defaultFamily() const
{
    switch ( d->request.styleHint ) {
    case TQFont::Times:
	return TQString::fromLatin1( "Times" );

    case TQFont::Courier:
	return TQString::fromLatin1( "Courier" );

    case TQFont::Decorative:
	return TQString::fromLatin1( "Old English" );

    case TQFont::Helvetica:
    case TQFont::System:
    default:
	return TQString::fromLatin1( "Helvetica" );
    }
}

/*
  Returns a last resort raw font name for the font matching algorithm.
  This is used if even the last resort family is not available. It
  returns \e something, almost no matter what.  The current
  implementation tries a wide variety of common fonts, returning the
  first one it finds. The implementation may change at any time.
*/
static const char * const tryFonts[] = {
    "-*-helvetica-medium-r-*-*-*-120-*-*-*-*-*-*",
    "-*-courier-medium-r-*-*-*-120-*-*-*-*-*-*",
    "-*-times-medium-r-*-*-*-120-*-*-*-*-*-*",
    "-*-lucida-medium-r-*-*-*-120-*-*-*-*-*-*",
    "-*-helvetica-*-*-*-*-*-120-*-*-*-*-*-*",
    "-*-courier-*-*-*-*-*-120-*-*-*-*-*-*",
    "-*-times-*-*-*-*-*-120-*-*-*-*-*-*",
    "-*-lucida-*-*-*-*-*-120-*-*-*-*-*-*",
    "-*-helvetica-*-*-*-*-*-*-*-*-*-*-*-*",
    "-*-courier-*-*-*-*-*-*-*-*-*-*-*-*",
    "-*-times-*-*-*-*-*-*-*-*-*-*-*-*",
    "-*-lucida-*-*-*-*-*-*-*-*-*-*-*-*",
    "-*-fixed-*-*-*-*-*-*-*-*-*-*-*-*",
    "6x13",
    "7x13",
    "8x13",
    "9x15",
    "fixed",
    0
};

// Returns true if the font exists, false otherwise
static bool fontExists( const TQString &fontName )
{
    int count;
    char **fontNames = XListFonts( TQPaintDevice::x11AppDisplay(),
				   (char*)fontName.latin1(), 32768, &count );
    if ( fontNames ) XFreeFontNames( fontNames );

    return count != 0;
}

/*!
    Returns a "last resort" font name for the font matching algorithm.
    This is used if the last resort family is not available. It will
    always return a name, if necessary returning something like
    "fixed" or "system".

    The current implementation tries a wide variety of common fonts,
    returning the first one it finds. The implementation may change
    at any time, but this function will always return a string
    containing something.

    It is theoretically possible that there really isn't a
    lastResortFont() in which case TQt will abort with an error
    message. We have not been able to identify a case where this
    happens. Please \link bughowto.html report it as a bug\endlink if
    it does, preferably with a list of the fonts you have installed.

    \sa lastResortFamily() rawName()
*/
TQString TQFont::lastResortFont() const
{
    static TQString last;

    // already found
    if ( ! last.isNull() )
	return last;

    int i = 0;
    const char* f;

    while ( ( f = tryFonts[i] ) ) {
	last = TQString::fromLatin1( f );

	if ( fontExists( last ) )
	    return last;

	i++;
    }

#if defined(CHECK_NULL)
    tqFatal( "TQFontPrivate::lastResortFont: Cannot find any reasonable font" );
#endif

    return last;
}




// **********************************************************************
// TQFontMetrics member methods
// **********************************************************************

int TQFontMetrics::width( TQChar ch ) const
{
    unsigned short uc = ch.unicode();
    if ( uc < TQFontEngineData::widthCacheSize &&
	 d->engineData && d->engineData->widthCache[ uc ] )
	return d->engineData->widthCache[ uc ];

    if ( ::category( ch ) == TQChar::Mark_NonSpacing || qIsZeroWidthChar(ch.unicode()))
	return 0;

    TQFont::Script script;
    SCRIPT_FOR_CHAR( script, ch );

    TQFontEngine *engine = d->engineForScript( script );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

    glyph_t glyphs[8];
    advance_t advances[8];
    int nglyphs = 7;
    engine->stringToCMap( &ch, 1, glyphs, advances, &nglyphs, false );

    // ### can nglyphs != 1 happen at all? Not currently I think
    if ( uc < TQFontEngineData::widthCacheSize && advances[0] > 0 && advances[0] < 0x100 )
	d->engineData->widthCache[ uc ] = advances[0];

    return advances[0];
}

int TQFontMetrics::charWidth( const TQString &str, int pos ) const
{
    if ( pos < 0 || pos >= (int)str.length() )
	return 0;

    uint uc;
    bool isSurrogate;
    if (str[pos].isHighSurrogate() && pos < (str.length() - 1) && str[pos + 1].isLowSurrogate())
    {
	isSurrogate = true;
	uc = TQChar::surrogateToUcs4(str[pos], str[pos + 1]);
    }
    else
    {
	isSurrogate = false;
	uc = str[pos].unicode();
    }
    if ( uc < TQFontEngineData::widthCacheSize &&
	 d->engineData && d->engineData->widthCache[ uc ] )
	return d->engineData->widthCache[ uc ];

    const TQChar &ch = str.unicode()[ pos ];
    TQFont::Script script;
    SCRIPT_FOR_CHAR( script, ch );

    int width;
    if ( script >= TQFont::Arabic && script <= TQFont::Khmer ) {
	// complex script shaping. Have to do some hard work
	int from = TQMAX( 0,  pos - 8 );
	int to = TQMIN( (int)str.length(), pos + 8 );
	TQConstString cstr( str.unicode()+from, to-from);
	TQTextEngine layout( cstr.string(), d );
	layout.itemize( TQTextEngine::WidthOnly );
	width = layout.width( pos-from, 1 );
    } else if ( ::category( ch ) == TQChar::Mark_NonSpacing || qIsZeroWidthChar(ch.unicode())) {
	width = 0;
    } else {
	TQFontEngine *engine = d->engineForScript( script );
#ifdef QT_CHECK_STATE
	Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

	glyph_t glyphs[8];
	advance_t advances[8];
	int nglyphs = 7;
	engine->stringToCMap( &ch, isSurrogate ? 2 : 1, glyphs, advances, &nglyphs, false );
	width = advances[0];
    }
    if ( uc < TQFontEngineData::widthCacheSize && width > 0 && width < 0x100 )
	d->engineData->widthCache[ uc ] = width;
    return width;
}
