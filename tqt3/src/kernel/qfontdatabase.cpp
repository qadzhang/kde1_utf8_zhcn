/****************************************************************************
**
** Implementation of font database class.
**
** Created : 990603
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
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

#include "ntqfontdatabase.h"

#ifndef TQT_NO_FONTDATABASE

#include <ntqtl.h>
#include <ntqapplication.h>

#include <private/qunicodetables_p.h>
#include "qfontengine_p.h"

#include <ntqcleanuphandler.h>

#ifdef TQ_WS_X11
#include <locale.h>
#endif
#include <stdlib.h>

//#define TQFONTDATABASE_DEBUG
#ifdef TQFONTDATABASE_DEBUG
#  define FD_DEBUG tqDebug
#else
#  define FD_DEBUG if (false) tqDebug
#endif

//#define FONT_MATCH_DEBUG
#ifdef FONT_MATCH_DEBUG
#  define FM_DEBUG tqDebug
#else
#  define FM_DEBUG if (false) tqDebug
#endif

#if defined(Q_CC_MSVC) && !defined(Q_CC_MSVC_NET)
#  define for if(0){}else for
#endif

static int ucstricmp( const TQString &as, const TQString &bs )
{
    const TQChar *a = as.unicode();
    const TQChar *b = bs.unicode();
    if ( a == b )
	return 0;
    if ( a == 0 )
	return 1;
    if ( b == 0 )
	return -1;
    int l=TQMIN(as.length(),bs.length());
    while ( l-- && ::lower( *a ) == ::lower( *b ) )
	a++,b++;
    if ( l==-1 )
	return ( as.length()-bs.length() );
    return ::lower( *a ).unicode() - ::lower( *b ).unicode();
}

static int getFontWeight( const TQString &weightString )
{
    TQString s = weightString.lower();

    // Test in decreasing order of commonness
    if (s == "medium" ||
	s == "normal")
	return TQFont::Normal;
    if (s == "bold")
	return TQFont::Bold;
    if (s == "demibold" || s == "demi bold")
	return TQFont::DemiBold;
    if (s == "black")
	return TQFont::Black;
    if (s == "light")
	return TQFont::Light;

    if (s.contains("bold")) {
	if (s.contains("demi"))
	    return (int) TQFont::DemiBold;
	return (int) TQFont::Bold;
    }

    if (s.contains("light"))
	return (int) TQFont::Light;

    if (s.contains("black"))
	return (int) TQFont::Black;

    return (int) TQFont::Normal;
}

#ifdef TQ_WS_X11
struct TQtFontEncoding
{
    signed int encoding : 16;

    uint xpoint   : 16;
    uint xres     : 8;
    uint yres     : 8;
    uint avgwidth : 16;
    uchar pitch   : 8;
};
#endif // TQ_WS_X11

struct TQtFontSize
{
    unsigned short pixelSize;

#ifdef TQ_WS_X11
    int count;
    TQtFontEncoding *encodings;
    TQtFontEncoding *encodingID( int id, uint xpoint = 0, uint xres = 0,
				uint yres = 0, uint avgwidth = 0, bool add = false);
#endif // TQ_WS_X11
};


#ifdef TQ_WS_X11
TQtFontEncoding *TQtFontSize::encodingID( int id, uint xpoint, uint xres,
					uint yres, uint avgwidth, bool add )
{
    // we don't match using the xpoint, xres and yres parameters, only the id
    for ( int i = 0; i < count; ++i ) {
	if ( encodings[i].encoding == id )
	    return encodings + i;
    }

    if ( !add ) return 0;

    if ( !(count % 4) )
	encodings = ( TQtFontEncoding * )
		    realloc( encodings,
			     (((count+4) >> 2 ) << 2 ) * sizeof( TQtFontEncoding ) );
    encodings[count].encoding = id;
    encodings[count].xpoint = xpoint;
    encodings[count].xres = xres;
    encodings[count].yres = yres;
    encodings[count].avgwidth = avgwidth;
    encodings[count].pitch = '*';
    return encodings + count++;
}
#endif // TQ_WS_X11

struct TQtFontStyle
{
    struct Key {
	Key( const TQString &styleString );
	Key() : italic( false ), oblique( false ),
		weight( TQFont::Normal ), stretch( 0 ) { }
	Key( const Key &o ) : italic( o.italic ), oblique( o.oblique ),
			      weight( o.weight ), stretch( o.stretch ) { }
	uint italic : 1;
	uint oblique : 1;
	signed int  weight : 8;
	signed int stretch : 12;

	bool operator==( const Key & other ) {
	    return ( italic == other.italic &&
		     oblique == other.oblique &&
		     weight == other.weight &&
		     (stretch == 0 || other.stretch == 0 || stretch == other.stretch) );
	}
	bool operator!=( const Key &other ) {
	    return !operator==(other);
	}
	bool operator <( const Key &o ) {
	    int x = (italic << 13) + (oblique << 12) + (weight << 14) + stretch;
	    int y = (o.italic << 13) + (o.oblique << 12) + (o.weight << 14) + o.stretch;
	    return ( x < y );
	}
    };

    TQtFontStyle( const Key &k )
	: key( k ), bitmapScalable( false ), smoothScalable( false ),
	  fakeOblique( false ), count( 0 ), pixelSizes( 0 )
    {
#if defined(TQ_WS_X11)
	weightName = setwidthName = 0;
#endif // TQ_WS_X11
    }

    ~TQtFontStyle() {
#ifdef TQ_WS_X11
	delete [] weightName;
	delete [] setwidthName;
	while ( count-- )
	    free(pixelSizes[count].encodings);
#endif
	free( pixelSizes );
    }

    Key key;
    bool bitmapScalable : 1;
    bool smoothScalable : 1;
    bool fakeOblique    : 1;
    int count           : 29;
    TQtFontSize *pixelSizes;

#ifdef TQ_WS_X11
    const char *weightName;
    const char *setwidthName;
#endif // TQ_WS_X11

    TQtFontSize *pixelSize( unsigned short size, bool = false );
};

TQtFontStyle::Key::Key( const TQString &styleString )
    : italic( false ), oblique( false ), weight( TQFont::Normal ), stretch( 0 )
{
    weight = getFontWeight( styleString );

    if ( styleString.contains( "Italic" ) )
	 italic = true;
    else if ( styleString.contains( "Oblique" ) )
	 oblique = true;
}

TQtFontSize *TQtFontStyle::pixelSize( unsigned short size, bool add )
{
    for ( int i = 0; i < count; i++ ) {
	if ( pixelSizes[i].pixelSize == size )
	    return pixelSizes + i;
    }
    if ( !add )
	return 0;

    if ( !(count % 8) )
	pixelSizes = (TQtFontSize *)
		     realloc( pixelSizes,
			      (((count+8) >> 3 ) << 3) * sizeof(TQtFontSize) );
    pixelSizes[count].pixelSize = size;
#ifdef TQ_WS_X11
    pixelSizes[count].count = 0;
    pixelSizes[count].encodings = 0;
#endif
    return pixelSizes + (count++);
}

struct TQtFontFoundry
{
    TQtFontFoundry( const TQString &n ) : name( n ), count( 0 ), styles( 0 ) {}
    ~TQtFontFoundry() {
	while ( count-- )
	    delete styles[count];
	free( styles );
    }

    TQString name;

    int count;
    TQtFontStyle **styles;
    TQtFontStyle *style( const TQtFontStyle::Key &,  bool = false );
};

TQtFontStyle *TQtFontFoundry::style( const TQtFontStyle::Key &key, bool create )
{
    int pos = 0;
    if ( count ) {
	int low = 0;
	int high = count;
	pos = count / 2;
	while ( high > low ) {
	    if ( styles[pos]->key == key )
		return styles[pos];
	    if ( styles[pos]->key < key )
		low = pos + 1;
	    else
		high = pos;
	    pos = (high + low) / 2;
	};
	pos = low;
    }
    if ( !create )
	return 0;

//     tqDebug("adding key (weight=%d, italic=%d, oblique=%d stretch=%d) at %d",  key.weight, key.italic, key.oblique, key.stretch, pos );
    if ( !(count % 8) )
	styles = (TQtFontStyle **)
		 realloc( styles, (((count+8) >> 3 ) << 3) * sizeof( TQtFontStyle * ) );

    memmove( styles + pos + 1, styles + pos, (count-pos)*sizeof(TQtFontStyle *) );
    styles[pos] = new TQtFontStyle( key );
    count++;
    return styles[pos];
}


struct TQtFontFamily
{
    enum ScriptStatus { Unknown = 0, Supported = 1,
			UnSupported_Xft= 2, UnSupported_Xlfd = 4, UnSupported = 6 };

    TQtFontFamily(const TQString &n )
	:
#ifdef TQ_WS_X11
        fixedPitch( true ), hasXft( false ), xftScriptCheck( false ), xlfdLoaded( false ), synthetic(false),
#else
        fixedPitch( false ),
#endif
#ifdef TQ_WS_WIN
	scriptCheck( false ),
#endif
#if defined(Q_OS_MAC) && !defined(TQWS)
	fixedPitchComputed(false),
#endif
	fullyLoaded( false ),
	  name( n ), count( 0 ), foundries( 0 ) {
	memset( scripts, 0, sizeof( scripts ) );
    }
    ~TQtFontFamily() {
	while ( count-- )
	    delete foundries[count];
	free( foundries );
    }

    bool fixedPitch : 1;
#ifdef TQ_WS_X11
    bool hasXft : 1;
    bool xftScriptCheck : 1;
    bool xlfdLoaded : 1;
    bool synthetic : 1;
#endif
#ifdef TQ_WS_WIN
    bool scriptCheck : 1;
#endif
#if defined(Q_OS_MAC) && !defined(TQWS)
    bool fixedPitchComputed : 1;
#endif
    bool fullyLoaded : 1;
    TQString name;
    TQString rawName;
#ifdef TQ_WS_X11
    TQCString fontFilename;
    int fontFileIndex;
#endif
#ifdef TQ_WS_MAC
    FMFontFamily macFamily;
#endif
#ifdef TQ_WS_WIN
    TQString english_name;
#endif
    int count;
    TQtFontFoundry **foundries;

    unsigned char scripts[TQFont::LastPrivateScript];

    TQtFontFoundry *foundry( const TQString &f, bool = false );
};

TQtFontFoundry *TQtFontFamily::foundry( const TQString &f, bool create )
{
    if ( f.isNull() && count == 1 )
	return foundries[0];

    for ( int i = 0; i < count; i++ ) {
	if ( ucstricmp( foundries[i]->name, f ) == 0 )
	    return foundries[i];
    }
    if ( !create )
	return 0;

    if ( !(count % 8) )
	foundries = (TQtFontFoundry **)
		    realloc( foundries,
			     (((count+8) >> 3 ) << 3) * sizeof( TQtFontFoundry * ) );

    foundries[count] = new TQtFontFoundry( f );
    return foundries[count++];
}

class TQFontDatabasePrivate {
public:
    TQFontDatabasePrivate() : count( 0 ), families( 0 ) { }
    ~TQFontDatabasePrivate() {
	while ( count-- )
	    delete families[count];
	free( families );
    }
    TQtFontFamily *family( const TQString &f, bool = false );

    int count;
    TQtFontFamily **families;
};

TQtFontFamily *TQFontDatabasePrivate::family( const TQString &f, bool create )
{
    int low = 0;
    int high = count;
    int pos = count / 2;
    int res = 1;
    if ( count ) {
	while ( (res = ucstricmp( families[pos]->name, f )) && pos != low ) {
	    if ( res > 0 )
		high = pos;
	    else
		low = pos;
	    pos = (high + low) / 2;
	};
	if ( !res )
	    return families[pos];
    }
    if ( !create )
	return 0;

    if ( res < 0 )
	pos++;

    // tqDebug("adding family %s at %d total=%d",  f.latin1(), pos, count);
    if ( !(count % 8) )
	families = (TQtFontFamily **)
		   realloc( families,
			    (((count+8) >> 3 ) << 3) * sizeof( TQtFontFamily * ) );

    memmove( families + pos + 1, families + pos, (count-pos)*sizeof(TQtFontFamily *) );
    families[pos] = new TQtFontFamily( f );
    count++;
    return families[pos];
}




#if defined(TQ_WS_X11) || defined(TQ_WS_WIN)
static const unsigned short sample_chars[TQFont::LastPrivateScript][14] =
{
    // European Alphabetic Scripts
    // Latin,
    { 0x0041, 0x0 },
    // Greek,
    { 0x0391, 0x0 },
    // Cyrillic,
    { 0x0410, 0x0 },
    // Armenian,
    { 0x0540, 0x0 },
    // Georgian,
    { 0x10d0, 0x0 },
    // Runic,
    { 0x16a0, 0x0 },
    // Ogham,
    { 0x1680, 0x0 },
    // SpacingModifiers,
    { 0x02c6, 0x0 },
    // CombiningMarks,
    { 0x0300, 0x0 },

    // Middle Eastern Scripts
    // Hebrew,
    { 0x05d0, 0x0 },
    // Arabic,
    { 0x0630, 0x0 },
    // Syriac,
    { 0x0710, 0x0 },
    // Thaana,
    { 0x0780, 0x0 },

    // South and Southeast Asian Scripts
    // Devanagari,
    { 0x0910, 0x0 },
    // Bengali,
    { 0x0990, 0x0 },
    // Gurmukhi,
    { 0x0a10, 0x0 },
    // Gujarati,
    { 0x0a90, 0x0 },
    // Oriya,
    { 0x0b10, 0x0 },
    // Tamil,
    { 0x0b90, 0x0 },
    // Telugu,
    { 0x0c10, 0x0 },
    // Kannada,
    { 0x0c90, 0x0 },
    // Malayalam,
    { 0x0d10, 0x0 },
    // Sinhala,
    { 0x0d90, 0x0 },
    // Thai,
    { 0x0e10, 0x0 },
    // Lao,
    { 0x0e81, 0x0 },
    // Tibetan,
    { 0x0f00, 0x0 },
    // Myanmar,
    { 0x1000, 0x0 },
    // Khmer,
    { 0x1780, 0x0 },

    // East Asian Scripts
    // Han,
    { 0x4e00, 0x0 },
    // Hiragana,
    {  0x3050, 0x4e00, 0x25EF, 0x3012, 0x3013, 0x30FB, 0x30FC, 0x5CE0, 0 },
    // Katakana,
    { 0x30b0, 0x4e00, 0x25EF, 0x3012, 0x3013, 0x30FB, 0x30FC, 0x5CE0, 0 },
    // Hangul,
    { 0xac00, 0x0 },
    // Bopomofo,
    { 0x3110, 0x0 },
    // Yi,
    { 0xa000, 0x0 },

    // Additional Scripts
    // Ethiopic,
    { 0x1200, 0x0 },
    // Cherokee,
    { 0x13a0, 0x0 },
    // CanadianAboriginal,
    { 0x1410, 0x0 },
    // Mongolian,
    { 0x1800, 0x0 },

    // Symbols
    // CurrencySymbols,
    { 0x20aa, 0x0 },
    // LetterlikeSymbols,
    { 0x2103, 0x0 },
    // NumberForms,
    { 0x2160, 0x0 },
    // MathematicalOperators,
    { 0x222b, 0x0 },
    // TechnicalSymbols,
    { 0x2312, 0x0 },
    // GeometricSymbols,
    { 0x2500, 0x0 },
    // MiscellaneousSymbols,
    { 0x2640, 0x2714, 0x0 },
    // EnclosedAndSquare,
    { 0x2460, 0x0 },
    // Braille,
    { 0x2800, 0x0 },

    // Unicode,
    { 0xfffd, 0x0 },

    // some scripts added in Unicode 3.2
    // Tagalog,
    { 0x1700, 0x0 },
    // Hanunoo,
    { 0x1720, 0x0 },
    // Buhid,
    { 0x1740, 0x0 },
    // Tagbanwa,
    { 0x1770, 0x0 },

    // KatakanaHalfWidth
    { 0xff65, 0x0 },

    // Limbu
    { 0x1901, 0x0 },
    // TaiLe
    { 0x1950, 0x0 },

    // NScripts
    { 0x0000, 0x0 },
    // NoScript
    { 0x0000, 0x0 },

    // Han_Japanese
    { 0x4e00, 0x25EF, 0x3012, 0x3013, 0x30FB, 0x5CE0, 0 },
    // Han_SimplifiedChinese, 0x3400 is optional
    { 0x4e00, 0x201C, 0x3002, 0x6237, 0x9555, 0xFFE5, 0 },
    // Han_TraditionalChinese, 0xF6B1 is optional
    // OR Han_HongkongChinese, 0x3435, 0xE000, 0xF6B1 are optional
    { 0x4e00, 0x201C, 0x3002, 0x6236, 0x9F98, 0xFFE5, 0 },
    // Han_Korean
    { 0x4e00, 0 }
    // Taiwan would be 0x201C, 0x3002, 0x4E00, 0x9F98, 0xFFE5
};

#if defined(TQ_WS_X11) && !defined(TQT_NO_XFTFREETYPE)
static inline bool requiresOpenType(TQFont::Script s)
{
    return (s >= TQFont::Syriac && s <= TQFont::Sinhala)
		 || (s >= TQFont::Myanmar && s <= TQFont::Khmer);
}
#endif

static inline bool canRender( TQFontEngine *fe, TQFont::Script script )
{
    if ( !fe ) return false;

    bool hasChar = true;

    if (!sample_chars[script][0])
        hasChar = false;

    int i = 0;
    while (hasChar && sample_chars[script][i]){
        TQChar sample(sample_chars[script][i]);
        if ( !fe->canRender( &sample, 1  ) ) {
             hasChar = false;
#ifdef FONT_MATCH_DEBUG
	FM_DEBUG("    font has NOT char 0x%04x", sample.unicode() );
        } else {
	FM_DEBUG("    font has char 0x%04x", sample.unicode() );
#endif
        }
        ++i;
    }
#if defined(TQ_WS_X11) && !defined(TQT_NO_XFTFREETYPE)
    if (hasChar && requiresOpenType(script)) {
	TQOpenType *ot = fe->openType();
	if (!ot || !ot->supportsScript(script))
	    return false;
    }
#endif

    return hasChar;
}
#endif // TQ_WS_X11 || TQ_WS_WIN


static TQSingleCleanupHandler<TQFontDatabasePrivate> qfontdatabase_cleanup;
static TQFontDatabasePrivate *db=0;
#define SMOOTH_SCALABLE 0xffff

#if defined( TQ_WS_X11 )
#  include "qfontdatabase_x11.cpp"
#elif defined( TQ_WS_MAC )
#  include "qfontdatabase_mac.cpp"
#elif defined( TQ_WS_WIN )
#  include "qfontdatabase_win.cpp"
#elif defined( TQ_WS_QWS )
#  include "qfontdatabase_qws.cpp"
#endif

static TQtFontStyle *bestStyle(TQtFontFoundry *foundry, const TQtFontStyle::Key &styleKey)
{
    int best = 0;
    int dist = 0xffff;

    for ( int i = 0; i < foundry->count; i++ ) {
	TQtFontStyle *style = foundry->styles[i];

	int d = TQABS( styleKey.weight - style->key.weight );

	if ( styleKey.stretch != 0 && style->key.stretch != 0 ) {
	    d += TQABS( styleKey.stretch - style->key.stretch );
	}

	if ( styleKey.italic ) {
	    if ( !style->key.italic )
		d += style->key.oblique ? 0x0001 : 0x1000;
	} else if ( styleKey.oblique ) {
	    if (!style->key.oblique )
		d += style->key.italic ? 0x0001 : 0x1000;
	} else if ( style->key.italic || style->key.oblique ) {
	    d += 0x1000;
	}

	if ( d < dist ) {
	    best = i;
	    dist = d;
	}
    }

    FM_DEBUG( "          best style has distance 0x%x", dist );
    if (!foundry->count) {
        TQtFontStyle *temp = NULL;
        return temp;
    }
    return foundry->styles[best];
}

#if defined(TQ_WS_X11)
static TQtFontEncoding *findEncoding(TQFont::Script script, int styleStrategy,
                                    TQtFontSize *size, int force_encoding_id)
{
    TQtFontEncoding *encoding = 0;

    if (force_encoding_id >= 0) {
        encoding = size->encodingID(force_encoding_id);
        if (!encoding)
            FM_DEBUG("            required encoding_id not available");
        return encoding;
    }

    if (styleStrategy & (TQFont::OpenGLCompatible | TQFont::PreferBitmap)) {
        FM_DEBUG("            PreferBitmap and/or OpenGL set, skipping Xft");
    } else {
        encoding = size->encodingID(-1); // -1 == prefer Xft
        if (encoding) return encoding;
    }

    // Xft not available, find an XLFD font, trying the default encoding first
    encoding = size->encodingID(TQFontPrivate::defaultEncodingID);

    if (!encoding || !scripts_for_xlfd_encoding[encoding->encoding][script]) {
        // find the first encoding that supports the requested script
        encoding = 0;
        for (int x = 0; !encoding && x < size->count; ++x) {
            const int enc = size->encodings[x].encoding;
            if (scripts_for_xlfd_encoding[enc][script]) {
                encoding = size->encodings + x;
                break;
            }
        }
    }

    return encoding;
}
#endif // TQ_WS_X11


#if defined(TQ_WS_X11) || defined(TQ_WS_WIN)
static
unsigned int bestFoundry( TQFont::Script script, unsigned int score, int styleStrategy,
			  const TQtFontFamily *family, const TQString &foundry_name,
			  TQtFontStyle::Key styleKey, int pixelSize, char pitch,
			  TQtFontFoundry **best_foundry, TQtFontStyle **best_style,
			  TQtFontSize **best_size
#ifdef TQ_WS_X11
			  , TQtFontEncoding **best_encoding, int force_encoding_id
#endif
			  )
{
    Q_UNUSED( script );
    Q_UNUSED( pitch );

    FM_DEBUG( "  REMARK: looking for best foundry for family '%s'", family->name.latin1() );

    for ( int x = 0; x < family->count; ++x ) {
	TQtFontFoundry *foundry = family->foundries[x];
	if ( ! foundry_name.isEmpty() &&
	     ucstricmp( foundry->name, foundry_name ) != 0 )
	    continue;

	FM_DEBUG( "          looking for matching style in foundry '%s'",
		  foundry->name.isEmpty() ? "-- none --" : foundry->name.latin1() );

	TQtFontStyle *style = bestStyle(foundry, styleKey);

	if ( ! style->smoothScalable && ( styleStrategy & TQFont::ForceOutline ) ) {
	    FM_DEBUG( "            ForceOutline set, but not smoothly scalable" );
	    continue;
	}

	int px = -1;
	TQtFontSize *size = 0;

	// 1. see if we have an exact matching size
	if (! (styleStrategy & TQFont::ForceOutline)) {
	    size = style->pixelSize(pixelSize);
	    if (size) {
                FM_DEBUG("          found exact size match (%d pixels)", size->pixelSize);
                px = size->pixelSize;
            }
	}

	// 2. see if we have a smoothly scalable font
	if (! size && style->smoothScalable && ! (styleStrategy & TQFont::PreferBitmap)) {
	    size = style->pixelSize(SMOOTH_SCALABLE);
	    if (size) {
                FM_DEBUG("          found smoothly scalable font (%d pixels)", pixelSize);
                px = pixelSize;
            }
	}

	// 3. see if we have a bitmap scalable font
	if (! size && style->bitmapScalable && (styleStrategy & TQFont::PreferMatch)) {
	    size = style->pixelSize(0);
	    if (size) {
                FM_DEBUG("          found bitmap scalable font (%d pixels)", pixelSize);
                px = pixelSize;
            }
	}

#ifdef TQ_WS_X11
        TQtFontEncoding *encoding = 0;
#endif

	// 4. find closest size match
	if (! size) {
	    unsigned int distance = ~0u;
	    for (int x = 0; x < style->count; ++x) {
#ifdef TQ_WS_X11
                encoding =
                    findEncoding(script, styleStrategy, style->pixelSizes + x, force_encoding_id);
                if (!encoding) {
                    FM_DEBUG("          size %3d does not support the script we want",
                             style->pixelSizes[x].pixelSize);
                    continue;
                }
#endif

		unsigned int d = TQABS(style->pixelSizes[x].pixelSize - pixelSize);
		if (d < distance) {
		    distance = d;
		    size = style->pixelSizes + x;
		    FM_DEBUG("          best size so far: %3d (%d)", size->pixelSize, pixelSize);
		}
	    }

            if (!size) {
                FM_DEBUG("          no size supports the script we want");
                continue;
            }

	    if (style->bitmapScalable && ! (styleStrategy & TQFont::PreferQuality) &&
		(distance * 10 / pixelSize) >= 2) {
		// the closest size is not close enough, go ahead and
		// use a bitmap scaled font
		size = style->pixelSize(0);
		px = pixelSize;
	    } else {
		px = size->pixelSize;
	    }
	}

#ifdef TQ_WS_X11
        if (size) {
            encoding = findEncoding(script, styleStrategy, size, force_encoding_id);
            if (!encoding) size = 0;
        }
	if ( ! encoding ) {
	    FM_DEBUG( "          foundry doesn't support the script we want" );
	    continue;
	}
#endif // TQ_WS_X11

	unsigned int this_score = 0x0000;
        enum {
            PitchMismatch       = 0x4000,
            StyleMismatch       = 0x2000,
            BitmapScaledPenalty = 0x1000,
            EncodingMismatch    = 0x0002,
            XLFDPenalty         = 0x0001
        };

#ifdef TQ_WS_X11
	if ( encoding->encoding != -1 ) {
	    this_score += XLFDPenalty;
	    if ( encoding->encoding != TQFontPrivate::defaultEncodingID )
		this_score += EncodingMismatch;
	}
 	if (pitch != '*') {
 	    if ( !( pitch == 'm' && encoding->pitch == 'c' ) && pitch != encoding->pitch )
 		this_score += PitchMismatch;
 	}
#else
        // ignore pitch for asian fonts, some of them misreport it, and they are all
        // fixed pitch anyway.
        if (pitch != '*' && (script <= TQFont::NScripts && script != TQFont::KatakanaHalfWidth
                             && (script < TQFont::Han || script > TQFont::Yi))) {
	    if ((pitch == 'm' && !family->fixedPitch)
		|| (pitch == 'p' && family->fixedPitch))
		this_score += PitchMismatch;
	}
#endif
	if ( styleKey != style->key )
	    this_score += StyleMismatch;
	if ( !style->smoothScalable && px != size->pixelSize ) // bitmap scaled
	    this_score += BitmapScaledPenalty;
        if (px != pixelSize) // close, but not exact, size match
            this_score += TQABS(px - pixelSize);

	if ( this_score < score ) {
	    FM_DEBUG( "          found a match: score %x best score so far %x",
		      this_score, score );

	    score = this_score;
	    *best_foundry = foundry;
	    *best_style = style;
	    *best_size = size;
#ifdef TQ_WS_X11
	    *best_encoding = encoding;
#endif // TQ_WS_X11
	} else {
	    FM_DEBUG( "          score %x no better than best %x", this_score, score);
	}
    }

    return score;
}

/*!
    \internal
*/
TQFontEngine *
TQFontDatabase::findFont( TQFont::Script script, const TQFontPrivate *fp,
			 const TQFontDef &request, int force_encoding_id )
{
#ifndef TQ_WS_X11
    Q_UNUSED( force_encoding_id );
#endif

    if ( !db )
	initializeDb();

    TQFontEngine *fe = 0;
    if ( fp ) {
	if ( fp->rawMode ) {
	    fe = loadEngine( script, fp, request, 0, 0, 0
#ifdef TQ_WS_X11
			     , 0, 0, false
#endif
		);

	    // if we fail to load the rawmode font, use a 12pixel box engine instead
	    if (! fe) fe = new TQFontEngineBox( 12 );
	    return fe;
	}

	TQFontCache::Key key( request, script,
#ifdef TQ_WS_WIN
			     (int)fp->paintdevice, 
#else
			     fp->screen, 
#endif
                             fp->paintdevice
	    );
	fe = TQFontCache::instance->findEngine( key );
	if ( fe ) return fe;
    }

#ifdef TQ_WS_WIN
    if (request.styleStrategy & TQFont::PreferDevice) {
        TQFontEngine *fe = loadEngine(script, fp, request, 0, 0, 0);
        if(fe)
            return fe;
    }
#endif

    TQString family_name, foundry_name;
    TQtFontStyle::Key styleKey;
    styleKey.italic = request.italic;
    styleKey.weight = request.weight;
    styleKey.stretch = request.stretch;
    char pitch = request.ignorePitch ? '*' : request.fixedPitch ? 'm' : 'p';

    parseFontName( request.family, foundry_name, family_name );

#ifdef TQ_WS_X11
    if (script == TQFont::Han) {
        // modify script according to locale
        static TQFont::Script defaultHan;
        TQCString locale = setlocale(LC_ALL, NULL);

        if (locale.contains("ko"))
            defaultHan = TQFont::Han_Korean;
        else if (locale.contains("zh_TW") || locale.contains("zh_HK"))
            defaultHan = TQFont::Han_TraditionalChinese;
        else if (locale.contains("zh"))
            defaultHan = TQFont::Han_SimplifiedChinese;
        else if (locale.contains("ja"))
            defaultHan = TQFont::Han_Japanese;
        else
            defaultHan = TQFont::Han; // don't change

        script = defaultHan;
    }
#endif

    FM_DEBUG( "TQFontDatabase::findFont\n"
	      "  request:\n"
	      "    family: %s [%s], script: %d (%s)\n"
	      "    weight: %d, italic: %d\n"
	      "    stretch: %d\n"
	      "    pixelSize: %d\n"
	      "    pitch: %c",
	      family_name.isEmpty() ? "-- first in script --" : family_name.latin1(),
	      foundry_name.isEmpty() ? "-- any --" : foundry_name.latin1(),
	      script, scriptName( script ).latin1(),
	      request.weight, request.italic, request.stretch, request.pixelSize, pitch );

    bool usesFontConfig = false;
    if (family_name.isEmpty()
	|| family_name == "Sans Serif"
	|| family_name == "Serif"
	|| family_name == "Monospace") {
	fe = loadFontConfigFont(fp, request, script);
        usesFontConfig = (fe != 0);
    }
    if (!fe)
    {
	TQtFontFamily *best_family = 0;
	TQtFontFoundry *best_foundry = 0;
	TQtFontStyle *best_style = 0;
	TQtFontSize *best_size = 0;
#ifdef TQ_WS_X11
	TQtFontEncoding *best_encoding = 0;
#endif // TQ_WS_X11

	unsigned int score = ~0;

	load( family_name, script );

	for ( int x = 0; x < db->count; ++x ) {
	    TQtFontFamily *try_family = db->families[x];
#ifdef TQ_WS_X11
            if (try_family->synthetic) // skip generated fontconfig fonts
                continue;
#endif

            if ( !family_name.isEmpty() &&
		 ucstricmp( try_family->name, family_name ) != 0
#ifdef TQ_WS_WIN
		 && ucstricmp( try_family->english_name, family_name ) != 0
#endif
                )
		continue;

	    if ( family_name.isEmpty() )
		load( try_family->name, script );

	    uint score_adjust = 0;
	    TQFont::Script override_script = script;
	    if ( ! ( try_family->scripts[script] & TQtFontFamily::Supported )
		 && script != TQFont::Unicode) {
		// family not supported in the script we want
#ifdef TQ_WS_X11
		if (script >= TQFont::Han_Japanese && script <= TQFont::Han_Korean
		    && try_family->scripts[TQFont::Han] == TQtFontFamily::Supported) {
		    // try with the han script instead, give it a penalty
		    if (override_script == TQFont::Han_TraditionalChinese
			&& (try_family->scripts[TQFont::Han_SimplifiedChinese] & TQtFontFamily::Supported)) {
			override_script = TQFont::Han_SimplifiedChinese;
			score_adjust = 200;
		    } else if (override_script == TQFont::Han_SimplifiedChinese
                               && (try_family->scripts[TQFont::Han_TraditionalChinese] & TQtFontFamily::Supported)) {
			override_script = TQFont::Han_TraditionalChinese;
			score_adjust = 200;
		    } else {
			override_script = TQFont::Han;
			score_adjust = 400;
		    }
		} else
#endif
                    if (family_name.isEmpty()) {
                        continue;
                    } else if (try_family->scripts[TQFont::UnknownScript] & TQtFontFamily::Supported) {
                        // try with the unknown script (for a symbol font)
                        override_script = TQFont::UnknownScript;
                    } else if (try_family->scripts[TQFont::Unicode] & TQtFontFamily::Supported) {
                        // try with the unicode script instead
                        override_script = TQFont::Unicode;
                    } else {
                        // family not supported by unicode/unknown scripts
                        continue;
                    }
	    }

	    TQtFontFoundry *try_foundry = 0;
	    TQtFontStyle *try_style = 0;
	    TQtFontSize *try_size = 0;
#ifdef TQ_WS_X11
	    TQtFontEncoding *try_encoding = 0;
#endif // TQ_WS_X11

	    // as we know the script is supported, we can be sure
	    // to find a matching font here.
	    unsigned int newscore =
		bestFoundry( override_script, score, request.styleStrategy,
			     try_family, foundry_name, styleKey, request.pixelSize, pitch,
			     &try_foundry, &try_style, &try_size
#ifdef TQ_WS_X11
			     , &try_encoding, force_encoding_id
#endif
		    );
	    if ( try_foundry == 0 ) {
		// the specific foundry was not found, so look for
		// any foundry matching our requirements
		newscore = bestFoundry( override_script, score, request.styleStrategy, try_family,
					TQString::null, styleKey, request.pixelSize,
					pitch, &try_foundry, &try_style, &try_size
#ifdef TQ_WS_X11
					, &try_encoding, force_encoding_id
#endif
		    );
	    }
	    newscore += score_adjust;

	    if ( newscore < score ) {
		score = newscore;
		best_family = try_family;
		best_foundry = try_foundry;
		best_style = try_style;
		best_size = try_size;
#ifdef TQ_WS_X11
		best_encoding = try_encoding;
#endif // TQ_WS_X11
	    }
	    if ( newscore < 10 ) // xlfd instead of xft... just accept it
		break;
	}

	if ( best_family != 0 && best_foundry != 0 && best_style != 0
#ifdef TQ_WS_X11
	     && best_size != 0 && best_encoding != 0
#endif
	    ) {
	    FM_DEBUG( "  BEST:\n"
		      "    family: %s [%s]\n"
		      "    weight: %d, italic: %d, oblique: %d\n"
		      "    stretch: %d\n"
		      "    pixelSize: %d\n"
		      "    pitch: %c\n"
		      "    encoding: %d\n",
		      best_family->name.latin1(),
		      best_foundry->name.isEmpty() ? "-- none --" : best_foundry->name.latin1(),
		      best_style->key.weight, best_style->key.italic, best_style->key.oblique,
		      best_style->key.stretch, best_size ? best_size->pixelSize : 0xffff,
#ifdef TQ_WS_X11
		      best_encoding->pitch, best_encoding->encoding
#else
		      'p', 0
#endif
		);

	    fe = loadEngine( script, fp, request, best_family, best_foundry, best_style
#ifdef TQ_WS_X11
			     , best_size, best_encoding, ( force_encoding_id >= 0 )
#endif
		);
	}
	if (fe) {
	    fe->fontDef.family = best_family->name;
	    if ( ! best_foundry->name.isEmpty() ) {
		fe->fontDef.family += TQString::fromLatin1( " [" );
		fe->fontDef.family += best_foundry->name;
		fe->fontDef.family += TQString::fromLatin1( "]" );
	    }

	    if ( best_style->smoothScalable )
		fe->fontDef.pixelSize = request.pixelSize;
	    else if ( best_style->bitmapScalable &&
		      ( request.styleStrategy & TQFont::PreferMatch ) )
		fe->fontDef.pixelSize = request.pixelSize;
	    else
		fe->fontDef.pixelSize = best_size->pixelSize;

	    fe->fontDef.styleHint     = request.styleHint;
	    fe->fontDef.styleStrategy = request.styleStrategy;

	    fe->fontDef.weight        = best_style->key.weight;
	    fe->fontDef.italic        = best_style->key.italic || best_style->key.oblique;
	    fe->fontDef.fixedPitch    = best_family->fixedPitch;
	    fe->fontDef.stretch       = best_style->key.stretch;
	    fe->fontDef.ignorePitch   = false;
	}
    }

    if ( fe ) {
	if ( script != TQFont::Unicode && !canRender( fe, script ) ) {
	    FM_DEBUG( "  WARN: font loaded cannot render a sample char" );

	    delete fe;
            fe = 0;
	} else if ( fp ) {
            TQFontDef def = request;
            if (def.family.isEmpty()) {
                def.family = fp->request.family;
                def.family = def.family.left(def.family.find(','));
            }
	    TQFontCache::Key key( def, script,
#ifdef TQ_WS_WIN
				 (int)fp->paintdevice, 
#else
				 fp->screen, 
#endif
                                 fp->paintdevice
		);
	    TQFontCache::instance->insertEngine( key, fe );
            if (!usesFontConfig) {
                for ( int i = 0; i < TQFont::NScripts; ++i ) {
                    if ( i == script ) continue;

                    if (!canRender(fe, (TQFont::Script) i))
                        continue;

                    key.script = i;
                    TQFontCache::instance->insertEngine( key, fe );
                }
            }
	}
    }

    if (!fe) {
	if ( !request.family.isEmpty() )
            return 0;

        FM_DEBUG( "returning box engine" );

        fe = new TQFontEngineBox( request.pixelSize );
        fe->fontDef = request;

        if ( fp ) {
            TQFontCache::Key key( request, script,
#ifdef TQ_WS_WIN
                                 (int)fp->paintdevice, 
#else
                                 fp->screen, 
#endif
                                 fp->paintdevice
                );
            TQFontCache::instance->insertEngine( key, fe );
        }
    }

    if ( fp ) {
#if defined(TQ_WS_X11)
        fe->fontDef.pointSize =
            tqRound(10. * tqt_pointSize(fe->fontDef.pixelSize, fp->paintdevice, fp->screen));
#elif defined(TQ_WS_WIN)
        fe->fontDef.pointSize     = int( double( fe->fontDef.pixelSize ) * 720.0 /
                                         GetDeviceCaps(shared_dc,LOGPIXELSY) );
#else
        fe->fontDef.pointSize     = int( double( fe->fontDef.pixelSize ) * 720.0 /
                                         96.0 );
#endif
    } else {
        fe->fontDef.pointSize = request.pointSize;
    }

    return fe;
}
#endif // TQ_WS_X11 || TQ_WS_WIN




static TQString styleString( int weight, bool italic, bool oblique )
{
    TQString result;
    if ( weight >= TQFont::Black )
	result = "Black";
    else if ( weight >= TQFont::Bold )
	result = "Bold";
    else if ( weight >= TQFont::DemiBold )
	result = "Demi Bold";
    else if ( weight < TQFont::Normal )
	result = "Light";

    if ( italic )
	result += " Italic";
    else if ( oblique )
	result += " Oblique";

    if ( result.isEmpty() )
	result = "Normal";

    return result.simplifyWhiteSpace();
}

/*!
    Returns a string that describes the style of the font \a f. For
    example, "Bold Italic", "Bold", "Italic" or "Normal". An empty
    string may be returned.
*/
TQString TQFontDatabase::styleString( const TQFont &f )
{
    // ### fix oblique here
    return ::styleString( f.weight(), f.italic(), false );
}


/*!
    \class TQFontDatabase ntqfontdatabase.h
    \brief The TQFontDatabase class provides information about the fonts available in the underlying window system.

    \ingroup environment
    \ingroup graphics

    The most common uses of this class are to query the database for
    the list of font families() and for the pointSizes() and styles()
    that are available for each family. An alternative to pointSizes()
    is smoothSizes() which returns the sizes at which a given family
    and style will look attractive.

    If the font family is available from two or more foundries the
    foundry name is included in the family name, e.g. "Helvetica
    [Adobe]" and "Helvetica [Cronyx]". When you specify a family you
    can either use the old hyphenated TQt 2.x "foundry-family" format,
    e.g. "Cronyx-Helvetica", or the new bracketed TQt 3.x "family
    [foundry]" format e.g. "Helvetica [Cronyx]". If the family has a
    foundry it is always returned, e.g. by families(), using the
    bracketed format.

    The font() function returns a TQFont given a family, style and
    point size.

    A family and style combination can be checked to see if it is
    italic() or bold(), and to retrieve its weight(). Similarly we can
    call isBitmapScalable(), isSmoothlyScalable(), isScalable() and
    isFixedPitch().

    A text version of a style is given by styleString().

    The TQFontDatabase class also supports some static functions, for
    example, standardSizes(). You can retrieve the Unicode 3.0
    description of a \link TQFont::Script script\endlink using
    scriptName(), and a sample of characters in a script with
    scriptSample().

    Example:
\code
#include <ntqapplication.h>
#include <ntqfontdatabase.h>
#include <else.h>

int main( int argc, char **argv )
{
    TQApplication app( argc, argv );
    TQFontDatabase fdb;
    TQStringList families = fdb.families();
    for ( TQStringList::Iterator f = families.begin(); f != families.end(); ++f ) {
	TQString family = *f;
	tqDebug( family );
	TQStringList styles = fdb.styles( family );
	for ( TQStringList::Iterator s = styles.begin(); s != styles.end(); ++s ) {
	    TQString style = *s;
	    TQString dstyle = "\t" + style + " (";
	    TQValueList<int> smoothies = fdb.smoothSizes( family, style );
	    for ( TQValueList<int>::Iterator points = smoothies.begin();
		  points != smoothies.end(); ++points ) {
		dstyle += TQString::number( *points ) + " ";
	    }
	    dstyle = dstyle.left( dstyle.length() - 1 ) + ")";
	    tqDebug( dstyle );
	}
    }
    return 0;
}
\endcode
    This example gets the list of font families, then the list of
    styles for each family and the point sizes that are available for
    each family/style combination.
*/
/*!
    \obsolete
    \fn inline TQStringList TQFontDatabase::families( bool ) const
*/
/*!
    \obsolete
    \fn inline TQStringList TQFontDatabase::styles( const TQString &family,
					  const TQString & ) const
*/
/*!
    \obsolete
    \fn inline TQValueList<int> TQFontDatabase::pointSizes( const TQString &family,
						  const TQString &style ,
						  const TQString & )
*/

/*!
    \obsolete
    \fn inline TQValueList<int> TQFontDatabase::smoothSizes( const TQString &family,
						   const TQString &style,
						   const TQString & )
*/
/*!
    \obsolete
    \fn inline TQFont TQFontDatabase::font( const TQString &familyName,
				  const TQString &style,
				  int pointSize,
				  const TQString &)
*/
/*!
    \obsolete
    \fn inline bool TQFontDatabase::isBitmapScalable( const TQString &family,
					     const TQString &style,
					     const TQString & ) const
*/

/*!
    \obsolete
    \fn inline bool TQFontDatabase::isSmoothlyScalable( const TQString &family,
					       const TQString &style,
					       const TQString & ) const
*/

/*!
    \obsolete
    \fn inline bool TQFontDatabase::isScalable( const TQString &family,
				       const TQString &style,
				       const TQString & ) const
*/

/*!
    \obsolete
    \fn inline bool TQFontDatabase::isFixedPitch( const TQString &family,
					 const TQString &style,
					 const TQString & ) const
*/

/*!
    \obsolete
    \fn inline bool TQFontDatabase::italic( const TQString &family,
				   const TQString &style,
				   const TQString & ) const
*/

/*!
    \obsolete
    \fn inline bool TQFontDatabase::bold( const TQString &family,
				 const TQString &style,
				 const TQString & ) const
*/

/*!
    \obsolete
    \fn inline int TQFontDatabase::weight( const TQString &family,
				  const TQString &style,
				  const TQString & ) const
*/


/*!
    Creates a font database object.
*/
TQFontDatabase::TQFontDatabase()
{
    createDatabase();

    d = db;
}


/*! Returns a sorted list of the names of the available font families.

    If a family exists in several foundries, the returned name for
    that font is in the form "family [foundry]". Examples: "Times
    [Adobe]", "Times [Cronyx]", "Palatino".
*/
TQStringList TQFontDatabase::families() const
{
    load();

    TQStringList flist;
    for ( int i = 0; i < d->count; i++ ) {
	TQtFontFamily *f = d->families[i];
	if ( f->count == 0 )
	    continue;
	if ( f->count == 1 ) {
	    flist.append( f->name );
	} else {
	    for ( int j = 0; j < f->count; j++ ) {
		TQString str = f->name;
		TQString foundry = f->foundries[j]->name;
		if ( !foundry.isEmpty() ) {
		    str += " [";
		    str += foundry;
		    str += "]";
		}
		flist.append( str );
	    }
	}
    }
    return flist;
}

/*!
    \overload

    Returns a sorted list of the available font families which support
    the Unicode script \a script.

    If a family exists in several foundries, the returned name for
    that font is in the form "family [foundry]". Examples: "Times
    [Adobe]", "Times [Cronyx]", "Palatino".
*/
TQStringList TQFontDatabase::families( TQFont::Script script ) const
{
    load();

    TQStringList flist;
    for ( int i = 0; i < d->count; i++ ) {
	TQtFontFamily *f = d->families[i];
	if ( f->count == 0 )
	    continue;
	if (!(f->scripts[script] & TQtFontFamily::Supported))
	    continue;
	if ( f->count == 1 ) {
	    flist.append( f->name );
	} else {
	    for ( int j = 0; j < f->count; j++ ) {
		TQString str = f->name;
		TQString foundry = f->foundries[j]->name;
		if ( !foundry.isEmpty() ) {
		    str += " [";
		    str += foundry;
		    str += "]";
		}
		flist.append( str );
	    }
	}
    }
    return flist;
}

/*!
    Returns a list of the styles available for the font family \a
    family. Some example styles: "Light", "Light Italic", "Bold",
    "Oblique", "Demi". The list may be empty.
*/
TQStringList TQFontDatabase::styles( const TQString &family ) const
{
    TQString familyName,  foundryName;
    parseFontName( family, foundryName, familyName );

    load( familyName );

    TQStringList l;
    TQtFontFamily *f = d->family( familyName );
    if ( !f )
	return l;

    TQtFontFoundry allStyles( foundryName );
    for ( int j = 0; j < f->count; j++ ) {
	TQtFontFoundry *foundry = f->foundries[j];
	if ( foundryName.isEmpty() || ucstricmp( foundry->name, foundryName ) == 0 ) {
	    for ( int k = 0; k < foundry->count; k++ ) {
		TQtFontStyle::Key ke( foundry->styles[k]->key );
		ke.stretch = 0;
		allStyles.style( ke,  true );
	    }
	}
    }

    for ( int i = 0; i < allStyles.count; i++ )
	l.append( ::styleString( allStyles.styles[i]->key.weight,
				 allStyles.styles[i]->key.italic,
				 allStyles.styles[i]->key.oblique ) );
    return l;
}

/*!
    Returns true if the font that has family \a family and style \a
    style is fixed pitch; otherwise returns false.
*/

bool TQFontDatabase::isFixedPitch(const TQString &family,
				 const TQString &style) const
{
    Q_UNUSED(style);

    TQString familyName,  foundryName;
    parseFontName( family, foundryName, familyName );

    load( familyName );

    TQtFontFamily *f = d->family( familyName );
#if defined(Q_OS_MAC) && !defined(TQWS)
    if (f) {
	if (!f->fixedPitchComputed) {
	    TQFontMetrics fm(familyName);
	    f->fixedPitch = fm.width('i') == fm.width('m');
	    f->fixedPitchComputed = true;
	}
    }
#endif

    return ( f && f->fixedPitch );
}

/*!
    Returns true if the font that has family \a family and style \a
    style is a scalable bitmap font; otherwise returns false. Scaling
    a bitmap font usually produces an unattractive hardly readable
    result, because the pixels of the font are scaled. If you need to
    scale a bitmap font it is better to scale it to one of the fixed
    sizes returned by smoothSizes().

    \sa isScalable(), isSmoothlyScalable()
*/
bool TQFontDatabase::isBitmapScalable( const TQString &family,
				      const TQString &style) const
{
    bool bitmapScalable = false;
    TQString familyName,  foundryName;
    parseFontName( family, foundryName, familyName );

    load( familyName );

    TQtFontStyle::Key styleKey( style );

    TQtFontFamily *f = d->family( familyName );
    if ( !f ) return bitmapScalable;

    for ( int j = 0; j < f->count; j++ ) {
	TQtFontFoundry *foundry = f->foundries[j];
	if ( foundryName.isEmpty() || ucstricmp( foundry->name, foundryName ) == 0 ) {
	    for ( int k = 0; k < foundry->count; k++ )
		if ((style.isEmpty() || foundry->styles[k]->key == styleKey) &&
                    foundry->styles[k]->bitmapScalable && !foundry->styles[k]->smoothScalable) {
		    bitmapScalable = true;
		    goto end;
		}
	}
    }
 end:
    return bitmapScalable;
}


/*!
    Returns true if the font that has family \a family and style \a
    style is smoothly scalable; otherwise returns false. If this
    function returns true, it's safe to scale this font to any size,
    and the result will always look attractive.

    \sa isScalable(), isBitmapScalable()
*/
bool  TQFontDatabase::isSmoothlyScalable( const TQString &family,
                                         const TQString &style) const
{
    bool smoothScalable = false;
    TQString familyName,  foundryName;
    parseFontName( family, foundryName, familyName );

    load( familyName );

    TQtFontStyle::Key styleKey( style );

    TQtFontFamily *f = d->family( familyName );
    if ( !f ) return smoothScalable;

    for ( int j = 0; j < f->count; j++ ) {
	TQtFontFoundry *foundry = f->foundries[j];
	if ( foundryName.isEmpty() || ucstricmp( foundry->name, foundryName ) == 0 ) {
	    for ( int k = 0; k < foundry->count; k++ )
		if ((style.isEmpty() || foundry->styles[k]->key == styleKey) && foundry->styles[k]->smoothScalable) {
		    smoothScalable = true;
		    goto end;
		}
	}
    }
 end:
    return smoothScalable;
}

/*!
    Returns true if the font that has family \a family and style \a
    style is scalable; otherwise returns false.

    \sa isBitmapScalable(), isSmoothlyScalable()
*/
bool  TQFontDatabase::isScalable( const TQString &family,
                                 const TQString &style) const
{
    if ( isSmoothlyScalable( family, style) )
        return true;

    return isBitmapScalable( family, style);
}


/*!
    Returns a list of the point sizes available for the font that has
    family \a family and style \a style. The list may be empty.

    \sa smoothSizes(), standardSizes()
*/
TQValueList<int> TQFontDatabase::pointSizes( const TQString &family,
					   const TQString &style)
{
#if defined(TQ_WS_MAC)
    // windows and macosx are always smoothly scalable
    Q_UNUSED( family );
    Q_UNUSED( style );
    return standardSizes();
#else
    bool smoothScalable = false;
    TQString familyName,  foundryName;
    parseFontName( family, foundryName, familyName );

    load( familyName );

    TQtFontStyle::Key styleKey( style );

    TQValueList<int> sizes;

    TQtFontFamily *fam = d->family( familyName );
    if ( !fam ) return sizes;

    for ( int j = 0; j < fam->count; j++ ) {
	TQtFontFoundry *foundry = fam->foundries[j];
	if ( foundryName.isEmpty() || ucstricmp( foundry->name, foundryName ) == 0 ) {
	    TQtFontStyle *style = foundry->style( styleKey );
	    if ( !style ) continue;

	    if ( style->smoothScalable ) {
		smoothScalable = true;
		goto end;
	    }
	    for ( int l = 0; l < style->count; l++ ) {
		const TQtFontSize *size = style->pixelSizes + l;

		if (size->pixelSize != 0 && size->pixelSize != USHRT_MAX) {
#ifdef TQ_WS_X11
		    const uint pointSize = tqRound(tqt_pointSize(size->pixelSize, 0, -1));
#else
		    const uint pointSize = size->pixelSize; // embedded uses 72dpi
#endif
		    if (! sizes.contains(pointSize))
			sizes.append(pointSize);
		}
	    }
	}
    }
 end:
    if ( smoothScalable )
	return standardSizes();

    qHeapSort( sizes );
    return sizes;
#endif
}

/*!
    Returns a TQFont object that has family \a family, style \a style
    and point size \a pointSize. If no matching font could be created,
    a TQFont object that uses the application's default font is
    returned.
*/
TQFont TQFontDatabase::font( const TQString &family, const TQString &style,
                           int pointSize)
{
    TQString familyName,  foundryName;
    parseFontName( family, foundryName, familyName );

    load( familyName );

    TQtFontFoundry allStyles( foundryName );
    TQtFontFamily *f = d->family( familyName );
    if ( !f ) return TQApplication::font();

    for ( int j = 0; j < f->count; j++ ) {
	TQtFontFoundry *foundry = f->foundries[j];
	if ( foundryName.isEmpty() || ucstricmp( foundry->name, foundryName ) == 0 ) {
	    for ( int k = 0; k < foundry->count; k++ )
		allStyles.style( foundry->styles[k]->key,  true );
	}
    }

    TQtFontStyle::Key styleKey( style );
    TQtFontStyle *s = bestStyle(&allStyles, styleKey);

    if ( !s ) // no styles found?
	return TQApplication::font();
    return TQFont( family, pointSize, s->key.weight,
		  s->key.italic ? true : s->key.oblique );
}


/*!
    Returns the point sizes of a font that has family \a family and
    style \a style that will look attractive. The list may be empty.
    For non-scalable fonts and bitmap scalable fonts, this function
    is equivalent to pointSizes().

  \sa pointSizes(), standardSizes()
*/
TQValueList<int> TQFontDatabase::smoothSizes( const TQString &family,
					    const TQString &style)
{
#ifdef TQ_WS_WIN
    Q_UNUSED( family );
    Q_UNUSED( style );
    return TQFontDatabase::standardSizes();
#else
    bool smoothScalable = false;
    TQString familyName,  foundryName;
    parseFontName( family, foundryName, familyName );

    load( familyName );

    TQtFontStyle::Key styleKey( style );

    TQValueList<int> sizes;

    TQtFontFamily *fam = d->family( familyName );
    if ( !fam )
	return sizes;

    for ( int j = 0; j < fam->count; j++ ) {
	TQtFontFoundry *foundry = fam->foundries[j];
	if ( foundryName.isEmpty() ||
	     ucstricmp( foundry->name, foundryName ) == 0 ) {
	    TQtFontStyle *style = foundry->style( styleKey );
	    if ( !style ) continue;

	    if ( style->smoothScalable ) {
		smoothScalable = true;
		goto end;
	    }
	    for ( int l = 0; l < style->count; l++ ) {
		const TQtFontSize *size = style->pixelSizes + l;

		if ( size->pixelSize != 0 && size->pixelSize != USHRT_MAX ) {
#ifdef TQ_WS_X11
		    const uint pointSize = tqRound(tqt_pointSize(size->pixelSize, 0, -1));
#else
		    const uint pointSize = size->pixelSize; // embedded uses 72dpi
#endif
		    if (! sizes.contains(pointSize))
			sizes.append( pointSize );
		}
	    }
	}
    }
 end:
    if ( smoothScalable )
	return TQFontDatabase::standardSizes();

    qHeapSort( sizes );
    return sizes;
#endif
}


/*!
    Returns a list of standard font sizes.

    \sa smoothSizes(), pointSizes()
*/
TQValueList<int> TQFontDatabase::standardSizes()
{
    TQValueList<int> ret;
    static const unsigned short standard[] =
	{ 6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72, 0 };
    const unsigned short *sizes = standard;
    while ( *sizes ) ret << *sizes++;
    return ret;
}


/*!
    Returns true if the font that has family \a family and style \a
    style is italic; otherwise returns false.

    \sa weight(), bold()
*/
bool TQFontDatabase::italic( const TQString &family,
                            const TQString &style) const
{
    TQString familyName,  foundryName;
    parseFontName( family, foundryName, familyName );

    load( familyName );

    TQtFontFoundry allStyles( foundryName );
    TQtFontFamily *f = d->family( familyName );
    if ( !f ) return false;

    for ( int j = 0; j < f->count; j++ ) {
	TQtFontFoundry *foundry = f->foundries[j];
	if ( foundryName.isEmpty() || ucstricmp( foundry->name, foundryName ) == 0 ) {
	    for ( int k = 0; k < foundry->count; k++ )
		allStyles.style( foundry->styles[k]->key,  true );
	}
    }

    TQtFontStyle::Key styleKey( style );
    TQtFontStyle *s = allStyles.style( styleKey );
    return s && s->key.italic;
}


/*!
    Returns true if the font that has family \a family and style \a
    style is bold; otherwise returns false.

    \sa italic(), weight()
*/
bool TQFontDatabase::bold( const TQString &family,
			  const TQString &style) const
{
    TQString familyName,  foundryName;
    parseFontName( family, foundryName, familyName );

    load( familyName );

    TQtFontFoundry allStyles( foundryName );
    TQtFontFamily *f = d->family( familyName );
    if ( !f ) return false;

    for ( int j = 0; j < f->count; j++ ) {
	TQtFontFoundry *foundry = f->foundries[j];
	if ( foundryName.isEmpty() ||
	     ucstricmp( foundry->name, foundryName ) == 0 ) {
	    for ( int k = 0; k < foundry->count; k++ )
		allStyles.style( foundry->styles[k]->key,  true );
	}
    }

    TQtFontStyle::Key styleKey( style );
    TQtFontStyle *s = allStyles.style( styleKey );
    return s && s->key.weight >= TQFont::Bold;
}


/*!
    Returns the weight of the font that has family \a family and style
    \a style. If there is no such family and style combination,
    returns -1.

    \sa italic(), bold()
*/
int TQFontDatabase::weight( const TQString &family,
                           const TQString &style) const
{
    TQString familyName,  foundryName;
    parseFontName( family, foundryName, familyName );

    load( familyName );

    TQtFontFoundry allStyles( foundryName );
    TQtFontFamily *f = d->family( familyName );
    if ( !f ) return -1;

    for ( int j = 0; j < f->count; j++ ) {
	TQtFontFoundry *foundry = f->foundries[j];
	if ( foundryName.isEmpty() ||
	     ucstricmp( foundry->name, foundryName ) == 0 ) {
	    for ( int k = 0; k < foundry->count; k++ )
		allStyles.style( foundry->styles[k]->key,  true );
	}
    }

    TQtFontStyle::Key styleKey( style );
    TQtFontStyle *s = allStyles.style( styleKey );
    return s ? s->key.weight : -1;
}


/*!
    Returns a string that gives a default description of the \a script
    (e.g. for displaying to the user in a dialog).  The name matches
    the name of the script as defined by the Unicode 3.0 standard.

    \sa TQFont::Script
*/
TQString TQFontDatabase::scriptName(TQFont::Script script)
{
    const char *name = 0;

    switch (script) {
    case TQFont::Latin:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Latin");
	break;
    case TQFont::Greek:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Greek" );
	break;
    case TQFont::Cyrillic:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Cyrillic" );
	break;
    case TQFont::Armenian:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Armenian" );
	break;
    case TQFont::Georgian:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Georgian" );
	break;
    case TQFont::Runic:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Runic" );
	break;
    case TQFont::Ogham:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Ogham" );
	break;
    case TQFont::SpacingModifiers:
	name = TQT_TRANSLATE_NOOP("TQFont",  "SpacingModifiers" );
	break;
    case TQFont::CombiningMarks:
	name = TQT_TRANSLATE_NOOP("TQFont",  "CombiningMarks" );
	break;
    case TQFont::Hebrew:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Hebrew" );
	break;
    case TQFont::Arabic:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Arabic" );
	break;
    case TQFont::Syriac:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Syriac" );
	break;
    case TQFont::Thaana:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Thaana" );
	break;
    case TQFont::Devanagari:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Devanagari" );
	break;
    case TQFont::Bengali:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Bengali" );
	break;
    case TQFont::Gurmukhi:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Gurmukhi" );
	break;
    case TQFont::Gujarati:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Gujarati" );
	break;
    case TQFont::Oriya:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Oriya" );
	break;
    case TQFont::Tamil:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Tamil" );
	break;
    case TQFont::Telugu:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Telugu" );
	break;
    case TQFont::Kannada:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Kannada" );
	break;
    case TQFont::Malayalam:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Malayalam" );
	break;
    case TQFont::Sinhala:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Sinhala" );
	break;
    case TQFont::Thai:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Thai" );
	break;
    case TQFont::Lao:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Lao" );
	break;
    case TQFont::Tibetan:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Tibetan" );
	break;
    case TQFont::Myanmar:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Myanmar" );
	break;
    case TQFont::Khmer:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Khmer" );
	break;
    case TQFont::Han:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Han" );
	break;
    case TQFont::Hiragana:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Hiragana" );
	break;
    case TQFont::Katakana:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Katakana" );
	break;
    case TQFont::Hangul:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Hangul" );
	break;
    case TQFont::Bopomofo:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Bopomofo" );
	break;
    case TQFont::Yi:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Yi" );
	break;
    case TQFont::Ethiopic:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Ethiopic" );
	break;
    case TQFont::Cherokee:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Cherokee" );
	break;
    case TQFont::CanadianAboriginal:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Canadian Aboriginal" );
	break;
    case TQFont::Mongolian:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Mongolian" );
	break;

    case TQFont::CurrencySymbols:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Currency Symbols" );
	break;

    case TQFont::LetterlikeSymbols:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Letterlike Symbols" );
	break;

    case TQFont::NumberForms:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Number Forms" );
	break;

    case TQFont::MathematicalOperators:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Mathematical Operators" );
	break;

    case TQFont::TechnicalSymbols:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Technical Symbols" );
	break;

    case TQFont::GeometricSymbols:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Geometric Symbols" );
	break;

    case TQFont::MiscellaneousSymbols:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Miscellaneous Symbols" );
	break;

    case TQFont::EnclosedAndSquare:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Enclosed and Square" );
	break;

    case TQFont::Braille:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Braille" );
	break;

    case TQFont::Unicode:
	name = TQT_TRANSLATE_NOOP("TQFont",  "Unicode" );
	break;

    case TQFont::Tagalog:
	name = TQT_TRANSLATE_NOOP( "TQFont", "Tagalog" );
	break;

    case TQFont::Hanunoo:
	name = TQT_TRANSLATE_NOOP( "TQFont", "Hanunoo" );
	break;

    case TQFont::Buhid:
	name = TQT_TRANSLATE_NOOP( "TQFont", "Buhid" );
    	break;

    case TQFont::Tagbanwa:
	name = TQT_TRANSLATE_NOOP( "TQFont", "Tagbanwa" );
	break;

    case TQFont::KatakanaHalfWidth:
	name = TQT_TRANSLATE_NOOP( "TQFont", "Katakana Half-Width Forms" );
	break;

    case TQFont::Han_Japanese:
	name = TQT_TRANSLATE_NOOP( "TQFont", "Han (Japanese)" );
	break;

    case TQFont::Han_SimplifiedChinese:
	name = TQT_TRANSLATE_NOOP( "TQFont", "Han (Simplified Chinese)" );
	break;

    case TQFont::Han_TraditionalChinese:
	name = TQT_TRANSLATE_NOOP( "TQFont", "Han (Traditional Chinese)" );
	break;

    case TQFont::Han_Korean:
	name = TQT_TRANSLATE_NOOP( "TQFont", "Han (Korean)" );
	break;

    default:
	name = TQT_TRANSLATE_NOOP( "TQFont", "Unknown Script" );
	break;
    }

    return tqApp ? tqApp->translate("TQFont", name) : TQString::fromLatin1(name);
}


/*!
    Returns a string with sample characters from \a script.

    \sa TQFont::Script
*/
TQString TQFontDatabase::scriptSample(TQFont::Script script)
{
    TQString sample = "AaBb";

    switch (script) {
    case TQFont::Latin:
	// This is cheating... we only show latin-1 characters so that we don't
	// end up loading lots of fonts - at least on X11...
	sample += TQChar(0x00C3);
	sample += TQChar(0x00E1);
	sample += "Zz";
	break;
    case TQFont::Greek:
	sample += TQChar(0x0393);
	sample += TQChar(0x03B1);
	sample += TQChar(0x03A9);
	sample += TQChar(0x03C9);
	break;
    case TQFont::Cyrillic:
	sample += TQChar(0x0414);
	sample += TQChar(0x0434);
	sample += TQChar(0x0436);
	sample += TQChar(0x0402);
	break;
    case TQFont::Armenian:
	sample += TQChar(0x053f);
	sample += TQChar(0x054f);
	sample += TQChar(0x056f);
	sample += TQChar(0x057f);
	break;
    case TQFont::Georgian:
	sample += TQChar(0x10a0);
	sample += TQChar(0x10b0);
	sample += TQChar(0x10c0);
	sample += TQChar(0x10d0);
	break;
    case TQFont::Runic:
	sample += TQChar(0x16a0);
	sample += TQChar(0x16b0);
	sample += TQChar(0x16c0);
	sample += TQChar(0x16d0);
	break;
    case TQFont::Ogham:
	sample += TQChar(0x1681);
	sample += TQChar(0x1687);
	sample += TQChar(0x1693);
	sample += TQChar(0x168d);
	break;



    case TQFont::Hebrew:
        sample += TQChar(0x05D0);
        sample += TQChar(0x05D1);
        sample += TQChar(0x05D2);
        sample += TQChar(0x05D3);
	break;
    case TQFont::Arabic:
	sample += TQChar(0x0628);
	sample += TQChar(0x0629);
        sample += TQChar(0x062A);
        sample += TQChar(0x063A);
	break;
    case TQFont::Syriac:
	sample += TQChar(0x0715);
	sample += TQChar(0x0725);
	sample += TQChar(0x0716);
	sample += TQChar(0x0726);
	break;
    case TQFont::Thaana:
	sample += TQChar(0x0784);
	sample += TQChar(0x0794);
	sample += TQChar(0x078c);
	sample += TQChar(0x078d);
	break;



    case TQFont::Devanagari:
	sample += TQChar(0x0905);
	sample += TQChar(0x0915);
	sample += TQChar(0x0925);
	sample += TQChar(0x0935);
	break;
    case TQFont::Bengali:
	sample += TQChar(0x0986);
	sample += TQChar(0x0996);
	sample += TQChar(0x09a6);
	sample += TQChar(0x09b6);
	break;
    case TQFont::Gurmukhi:
	sample += TQChar(0x0a05);
	sample += TQChar(0x0a15);
	sample += TQChar(0x0a25);
	sample += TQChar(0x0a35);
	break;
    case TQFont::Gujarati:
	sample += TQChar(0x0a85);
	sample += TQChar(0x0a95);
	sample += TQChar(0x0aa5);
	sample += TQChar(0x0ab5);
	break;
    case TQFont::Oriya:
	sample += TQChar(0x0b06);
	sample += TQChar(0x0b16);
	sample += TQChar(0x0b2b);
	sample += TQChar(0x0b36);
	break;
    case TQFont::Tamil:
	sample += TQChar(0x0b89);
	sample += TQChar(0x0b99);
	sample += TQChar(0x0ba9);
	sample += TQChar(0x0bb9);
	break;
    case TQFont::Telugu:
	sample += TQChar(0x0c05);
	sample += TQChar(0x0c15);
	sample += TQChar(0x0c25);
	sample += TQChar(0x0c35);
	break;
    case TQFont::Kannada:
	sample += TQChar(0x0c85);
	sample += TQChar(0x0c95);
	sample += TQChar(0x0ca5);
	sample += TQChar(0x0cb5);
	break;
    case TQFont::Malayalam:
	sample += TQChar(0x0d05);
	sample += TQChar(0x0d15);
	sample += TQChar(0x0d25);
	sample += TQChar(0x0d35);
	break;
    case TQFont::Sinhala:
	sample += TQChar(0x0d90);
	sample += TQChar(0x0da0);
	sample += TQChar(0x0db0);
	sample += TQChar(0x0dc0);
	break;
    case TQFont::Thai:
	sample += TQChar(0x0e02);
	sample += TQChar(0x0e12);
	sample += TQChar(0x0e22);
	sample += TQChar(0x0e32);
	break;
    case TQFont::Lao:
	sample += TQChar(0x0e8d);
	sample += TQChar(0x0e9d);
	sample += TQChar(0x0ead);
	sample += TQChar(0x0ebd);
	break;
    case TQFont::Tibetan:
	sample += TQChar(0x0f00);
	sample += TQChar(0x0f01);
	sample += TQChar(0x0f02);
	sample += TQChar(0x0f03);
	break;
    case TQFont::Myanmar:
	sample += TQChar(0x1000);
	sample += TQChar(0x1001);
	sample += TQChar(0x1002);
	sample += TQChar(0x1003);
	break;
    case TQFont::Khmer:
	sample += TQChar(0x1780);
	sample += TQChar(0x1790);
	sample += TQChar(0x17b0);
	sample += TQChar(0x17c0);
	break;



    case TQFont::Han:
	sample += TQChar(0x6f84);
	sample += TQChar(0x820a);
	sample += TQChar(0x61a9);
	sample += TQChar(0x9781);
	break;
    case TQFont::Hiragana:
	sample += TQChar(0x3050);
	sample += TQChar(0x3060);
	sample += TQChar(0x3070);
	sample += TQChar(0x3080);
	break;
    case TQFont::Katakana:
	sample += TQChar(0x30b0);
	sample += TQChar(0x30c0);
	sample += TQChar(0x30d0);
	sample += TQChar(0x30e0);
	break;
    case TQFont::Hangul:
	sample += TQChar(0xac00);
	sample += TQChar(0xac11);
	sample += TQChar(0xac1a);
	sample += TQChar(0xac2f);
	break;
    case TQFont::Bopomofo:
	sample += TQChar(0x3105);
	sample += TQChar(0x3115);
	sample += TQChar(0x3125);
	sample += TQChar(0x3129);
	break;
    case TQFont::Yi:
	sample += TQChar(0xa1a8);
	sample += TQChar(0xa1a6);
	sample += TQChar(0xa200);
	sample += TQChar(0xa280);
	break;



    case TQFont::Ethiopic:
	sample += TQChar(0x1200);
	sample += TQChar(0x1240);
	sample += TQChar(0x1280);
	sample += TQChar(0x12c0);
	break;
    case TQFont::Cherokee:
	sample += TQChar(0x13a0);
	sample += TQChar(0x13b0);
	sample += TQChar(0x13c0);
	sample += TQChar(0x13d0);
	break;
    case TQFont::CanadianAboriginal:
	sample += TQChar(0x1410);
	sample += TQChar(0x1500);
	sample += TQChar(0x15f0);
	sample += TQChar(0x1650);
	break;
    case TQFont::Mongolian:
	sample += TQChar(0x1820);
	sample += TQChar(0x1840);
	sample += TQChar(0x1860);
	sample += TQChar(0x1880);
	break;


    case TQFont::CurrencySymbols:
    case TQFont::LetterlikeSymbols:
    case TQFont::NumberForms:
    case TQFont::MathematicalOperators:
    case TQFont::TechnicalSymbols:
    case TQFont::GeometricSymbols:
    case TQFont::MiscellaneousSymbols:
    case TQFont::EnclosedAndSquare:
    case TQFont::Braille:
	break;


    case TQFont::Unicode:
	sample += TQChar(0x0174);
	sample += TQChar(0x0628);
	sample += TQChar(0x0e02);
	sample += TQChar(0x263A);
	sample += TQChar(0x3129);
	sample += TQChar(0x61a9);
	sample += TQChar(0xac2f);
	break;



    default:
	sample += TQChar(0xfffd);
	sample += TQChar(0xfffd);
	sample += TQChar(0xfffd);
	sample += TQChar(0xfffd);
	break;
    }

    return sample;
}




/*!
  \internal

  This makes sense of the font family name:

  1) if the family name contains a '-' (ie. "Adobe-Courier"), then we
  split at the '-', and use the string as the foundry, and the string to
  the right as the family

  2) if the family name contains a '[' and a ']', then we take the text
  between the square brackets as the foundry, and the text before the
  square brackets as the family (ie. "Arial [Monotype]")
*/
void TQFontDatabase::parseFontName(const TQString &name, TQString &foundry, TQString &family)
{
    if ( name.contains('-') ) {
	int i = name.find('-');
	foundry = name.left( i );
	family = name.right( name.length() - i - 1 );
    } else if ( name.contains('[') && name.contains(']')) {
	int i = name.find('[');
	int li = name.findRev(']');

	if (i < li) {
	    foundry = name.mid(i + 1, li - i - 1);
	    if (name[i - 1] == ' ')
		i--;
	    family = name.left(i);
	}
    } else {
	foundry = TQString::null;
	family = name;
    }
}

#endif // TQT_NO_FONTDATABASE
