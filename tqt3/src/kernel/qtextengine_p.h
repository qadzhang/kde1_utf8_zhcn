/****************************************************************************
**
** ???
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef TQTEXTENGINE_P_H
#define TQTEXTENGINE_P_H

#ifndef QT_H
#include "ntqglobal.h"
#include "ntqstring.h"
#include "ntqnamespace.h"
#include <private/qfontdata_p.h>
#endif // QT_H

#include <stdlib.h>
#ifndef Q_OS_TEMP
#include <assert.h>
#endif // Q_OS_TEMP

class TQFontPrivate;
class TQString;

class TQOpenType;
class TQPainter;

// this uses the same coordinate system as TQt, but a different one to freetype and Xft.
// * y is usually negative, and is equal to the ascent.
// * negative yoff means the following stuff is drawn higher up.
// the characters bounding rect is given by TQRect( x,y,width,height), it's advance by
// xoo and yoff
struct glyph_metrics_t
{
    inline glyph_metrics_t() {
	x = 100000;
	y = 100000;
	width = 0;
	height = 0;
	xoff = 0;
	yoff = 0;
    }
    inline glyph_metrics_t( int _x, int _y, int _width, int _height, int _xoff, int _yoff ) {
	x = _x;
	y = _y;
	width = _width;
	height = _height;
	xoff = _xoff;
	yoff = _yoff;
    }
    int x;
    int y;
    int width;
    int height;
    int xoff;
    int yoff;
};

typedef unsigned int glyph_t;
typedef int advance_t;

#if defined( TQ_WS_X11 ) || defined ( TQ_WS_QWS ) || defined( TQ_WS_MAC )

struct qoffset_t
{
    short x;
    short y;
};

struct TQScriptAnalysis
{
    unsigned short script    : 7;
    unsigned short bidiLevel : 6;  // Unicode Bidi algorithm embedding level (0-61)
    unsigned short override  : 1;  // Set when in LRO/RLO embedding
    unsigned short reserved  : 2;

    bool operator== ( const TQScriptAnalysis &other )
    {
	return script == other.script && bidiLevel == other.bidiLevel;
    }
};

#elif defined( TQ_WS_WIN )

// do not change the definitions below unless you know what you are doing!
// it is designed to be compatible with the types found in uniscribe.

struct qoffset_t
{
    int x;
    int y;
};

struct TQScriptAnalysis {
    unsigned short script         :10;
    unsigned short rtl            :1;
    unsigned short layoutRTL      :1;
    unsigned short linkBefore     :1;
    unsigned short linkAfter      :1;
    unsigned short logicalOrder   :1;
    unsigned short noGlyphIndex   :1;
    unsigned short bidiLevel         :5;
    unsigned short override          :1;
    unsigned short inhibitSymSwap    :1;
    unsigned short charShape         :1;
    unsigned short digitSubstitute   :1;
    unsigned short inhibitLigate     :1;
    unsigned short fDisplayZWG        :1;
    unsigned short arabicNumContext  :1;
    unsigned short gcpClusters       :1;
    unsigned short reserved          :1;
    unsigned short engineReserved    :2;
};

inline bool operator== ( const TQScriptAnalysis &sa1, const TQScriptAnalysis &sa2 )
{
    return sa1.script == sa2.script && sa1.bidiLevel == sa2.bidiLevel;
}

#endif

// enum and struct are  made to be compatible with Uniscribe, dont change unless you know what you're doing.
struct GlyphAttributes {
    // highest value means highest priority for justification. Justification is done by first inserting kashidas
    // starting with the highest priority positions, then stretching spaces, afterwards extending inter char
    // spacing, and last spacing between arabic words.
    // NoJustification is for example set for arabic where no Kashida can be inserted or for diacritics.
    enum Justification {
        NoJustification= 0,   // Justification can't be applied after this glyph
        Arabic_Space   = 1,   // This glyph represents a space inside arabic text
        Character      = 2,   // Inter-character justification point follows this glyph
        Space          = 4,   // This glyph represents a blank outside an Arabic run
        Arabic_Normal  = 7,   // Normal Middle-Of-Word glyph that connects to the right (begin)
        Arabic_Waw     = 8,    // Next character is final form of Waw/Ain/Qaf/Fa
        Arabic_BaRa    = 9,   // Next two chars are Ba + Ra/Ya/AlefMaksura
        Arabic_Alef    = 10,  // Next character is final form of Alef/Tah/Lam/Kaf/Gaf
        Arabic_HaaDal  = 11,  // Next character is final form of Haa/Dal/Taa Marbutah
        Arabic_Seen    = 12,  // Initial or Medial form Of Seen/Sad
        Arabic_Kashida = 13   // Kashida(U+640) in middle of word
    };
    unsigned short justification   :4;  // Justification class
    unsigned short clusterStart    :1;  // First glyph of representation of cluster
    unsigned short mark            :1;  // needs to be positioned around base char
    unsigned short zeroWidth       :1;  // ZWJ, ZWNJ etc, with no width, currently used as "Don't print" for ZWSP
    unsigned short reserved        :1;
    unsigned short combiningClass  :8;
};

// also this is compatible to uniscribe. Do not change.
struct TQCharAttributes {
    uchar softBreak      :1;     // Potential linebreak point _before_ this character
    uchar whiteSpace     :1;     // A unicode whitespace character, except NBSP, ZWNBSP
    uchar charStop       :1;     // Valid cursor position (for left/right arrow)
    uchar wordStop       :1;     // Valid cursor position (for ctrl + left/right arrow)
    uchar invalid        :1;
    uchar reserved       :3;
};

inline bool qIsZeroWidthChar(ushort uc)
{
    return (uc >= 0x200b && uc <= 0x200f /* ZW Space, ZWNJ, ZWJ, LRM and RLM */)
            || (uc >= 0x2028 && uc <= 0x202f /* LS, PS, LRE, RLE, PDF, LRO, RLO, NNBSP */)
            || (uc >= 0x206a && uc <= 0x206f /* ISS, ASS, IAFS, AFS, NADS, NODS */);
}

class TQFontEngine;

struct TQScriptItem
{
    inline TQScriptItem() : position( 0 ), isSpace( false ), isTab( false ),
			   isObject( false ), hasPositioning( false ),
			   descent( -1 ), ascent( -1 ), width( -1 ),
			   x( 0 ), y( 0 ), num_glyphs( 0 ), glyph_data_offset( 0 ),
			   fontEngine( 0 ) { }
    int position;
    TQScriptAnalysis analysis;
    unsigned short isSpace  : 1;
    unsigned short isTab    : 1;
    unsigned short isObject : 1;
    unsigned short hasPositioning : 1;
    unsigned short complex : 1; // Windows only
    unsigned short private_use : 1; // Windows only
    unsigned short reserved : 10;
    short descent;
    int ascent;
    int width;
    int x;
    int y;
    int num_glyphs;
    int glyph_data_offset;
    TQFontEngine *fontEngine;
};

struct TQScriptItemArrayPrivate
{
    unsigned int alloc;
    unsigned int size;
    TQScriptItem items[1];
};

class TQScriptItemArray
{
public:
    TQScriptItemArray() : d( 0 ) {}
    ~TQScriptItemArray();

    inline TQScriptItem &operator[] (int i) const {return d->items[i];   }
    inline void append( const TQScriptItem &item ) {
	if ( d->size == d->alloc )
	    resize( d->size + 1 );
	d->items[d->size] = item;
	d->size++;
    }
    inline int size() const { return d ? d->size : 0; }

    void resize( int s );
    void clear();

    TQScriptItemArrayPrivate *d;
private:
#ifdef TQ_DISABLE_COPY
    TQScriptItemArray( const TQScriptItemArray & );
    TQScriptItemArray &operator = ( const TQScriptItemArray & );
#endif
};

class TQFontPrivate;

class TQ_EXPORT TQTextEngine {
public:
    TQTextEngine( const TQString &str, TQFontPrivate *f );
    ~TQTextEngine();

    enum Mode {
	Full = 0x00,
	NoBidi = 0x01,
	SingleLine = 0x02,
	WidthOnly = 0x07
    };

    void itemize( int mode = Full );

    static void bidiReorder( int numRuns, const TQ_UINT8 *levels, int *visualOrder );

    const TQCharAttributes *attributes();
    void shape( int item ) const;

    // ### we need something for justification

    enum Edge {
	Leading,
	Trailing
    };
    enum ShaperFlag {
        RightToLeft = 0x0001,
        Mirrored = 0x0001
    };

    int width( int charFrom, int numChars ) const;
    glyph_metrics_t boundingBox( int from,  int len ) const;

    TQScriptItemArray items;
    TQString string;
    TQFontPrivate *fnt;
    int lineWidth;
    int widthUsed;
    int firstItemInLine;
    int currentItem;
    TQChar::Direction direction : 5;
    unsigned int haveCharAttributes : 1;
    unsigned int widthOnly : 1;
    unsigned int reserved : 25;

    int length( int item ) const {
	const TQScriptItem &si = items[item];
	int from = si.position;
	item++;
	return ( item < items.size() ? items[item].position : string.length() ) - from;
    }
    void splitItem( int item, int pos );

    unsigned short *logClustersPtr;
    glyph_t *glyphPtr;
    advance_t *advancePtr;
    qoffset_t *offsetsPtr;
    GlyphAttributes *glyphAttributesPtr;

    inline unsigned short *logClusters( const TQScriptItem *si ) const
	{ return logClustersPtr+si->position; }
    inline glyph_t *glyphs( const TQScriptItem *si ) const
	{ return glyphPtr+si->glyph_data_offset; }
    inline advance_t *advances( const TQScriptItem *si ) const
	{ return advancePtr+si->glyph_data_offset; }
    inline qoffset_t *offsets( const TQScriptItem *si ) const
	{ return offsetsPtr+si->glyph_data_offset; }
    inline GlyphAttributes *glyphAttributes( const TQScriptItem *si ) const
	{ return glyphAttributesPtr+si->glyph_data_offset; }

    void reallocate( int totalGlyphs );
    inline void ensureSpace( int nGlyphs ) const {
	if ( num_glyphs - used < nGlyphs )
	    ((TQTextEngine *)this)->reallocate( ( (used + nGlyphs + 16) >> 4 ) << 4 );
    }

    int allocated;
    void **memory;
    int num_glyphs;
    int used;
};

#endif
