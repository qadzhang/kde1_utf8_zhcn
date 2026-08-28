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

#ifndef TQFONTENGINE_P_H
#define TQFONTENGINE_P_H

#ifndef QT_H
#include "ntqglobal.h"
#endif // QT_H

#ifdef TQ_WS_WIN
#include "qt_windows.h"
#include "ntqptrdict.h"
#endif

#include "qtextengine_p.h"

typedef unsigned int glyph_t;
typedef int advance_t;

struct glyph_metrics_t;
struct qoffset_t;
struct TransformedFont;

class TQChar;
class TQOpenType;
class TQPaintDevice;


#if defined( TQ_WS_X11 ) || defined( TQ_WS_WIN) || defined( TQ_WS_MAC )
class TQFontEngine : public TQShared
{
public:
    enum Error {
	NoError,
	OutOfMemory
    };

    enum Type {
	// X11 types
	Box,
	XLFD,
	LatinXLFD,
	Xft,

	// MS Windows types
	Win,
	Uniscribe,

	// Apple MacOS types
	Mac,

	// Trolltech TQWS types
	TQWS
    };

    TQFontEngine() {
	count = 0; cache_count = 0;
#ifdef TQ_WS_X11
	transformed_fonts = 0;
#endif
    }
    virtual ~TQFontEngine();

    /* returns 0 as glyph index for non existant glyphs */
    virtual Error stringToCMap( const TQChar *str, int len, glyph_t *glyphs,
				advance_t *advances, int *nglyphs, bool mirrored ) const = 0;

#ifdef TQ_WS_X11
    virtual int cmap() const { return -1; }
    virtual TQOpenType *openType() const { return 0; }
#endif

    virtual void draw( TQPainter *p, int x, int y, const TQTextEngine *engine, const TQScriptItem *si, int textFlags ) = 0;

    virtual glyph_metrics_t boundingBox( const glyph_t *glyphs,
					 const advance_t *advances,
					 const qoffset_t *offsets, int numGlyphs ) = 0;
    virtual glyph_metrics_t boundingBox( glyph_t glyph ) = 0;

    virtual int ascent() const = 0;
    virtual int descent() const = 0;
    virtual int leading() const = 0;

    virtual int lineThickness() const;
    virtual int underlinePosition() const;

    virtual int maxCharWidth() const = 0;
    virtual int minLeftBearing() const { return 0; }
    virtual int minRightBearing() const { return 0; }

    virtual const char *name() const = 0;

    virtual bool canRender( const TQChar *string,  int len ) = 0;

    virtual void setScale( double ) {}
    virtual double scale() const { return 1.; }

    virtual Type type() const = 0;

    TQFontDef fontDef;
    uint cache_cost; // amount of mem used in kb by the font
    int cache_count;

#ifdef TQ_WS_WIN
    HDC dc() const;
    void getGlyphIndexes( const TQChar *ch, int numChars, glyph_t *glyphs, bool mirrored ) const;
    void getCMap();

    TQCString _name;
    HDC		hdc;
    HFONT	hfont;
    LOGFONT     logfont;
    uint	stockFont   : 1;
    uint	paintDevice : 1;
    uint        useTextOutA : 1;
    uint        ttf         : 1;
    uint        symbol      : 1;
    union {
	TEXTMETRICW	w;
	TEXTMETRICA	a;
    } tm;
    int		lw;
    unsigned char *cmap;
    void *script_cache;
    static TQPtrDict<TQFontEngine> cacheDict;
    short lbearing;
    short rbearing;
#endif // TQ_WS_WIN
#ifdef TQ_WS_X11
    TransformedFont *transformed_fonts;
#endif
};
#elif defined( TQ_WS_QWS )
class TQGfx;

class TQFontEngine : public TQShared
{
public:
    TQFontEngine( const TQFontDef&, const TQPaintDevice * = 0 );
   ~TQFontEngine();
    /*TQMemoryManager::FontID*/ void *handle() const;

    enum Type {
	// X11 types
	Box,
	XLFD,
	Xft,

	// MS Windows types
	Win,
	Uniscribe,

	// Apple MacOS types
	Mac,

	// Trolltech TQWS types
	Qws
    };

    enum TextFlags {
	Underline = 0x01,
	Overline  = 0x02,
	StrikeOut = 0x04
    };

    enum Error {
	NoError,
	OutOfMemory
    };
    /* returns 0 as glyph index for non existant glyphs */
    Error stringToCMap( const TQChar *str, int len, glyph_t *glyphs, advance_t *advances, int *nglyphs, bool mirrored ) const;

    void draw( TQPainter *p, int x, int y, const TQTextEngine *engine, const TQScriptItem *si, int textFlags );

    glyph_metrics_t boundingBox( const glyph_t *glyphs,
			       const advance_t *advances, const qoffset_t *offsets, int numGlyphs );
    glyph_metrics_t boundingBox( glyph_t glyph );

    int ascent() const;
    int descent() const;
    int leading() const;
    int maxCharWidth() const;
    int minLeftBearing() const;
    int minRightBearing() const;
    int underlinePosition() const;
    int lineThickness() const;

    Type type() { return Qws; }

    bool canRender( const TQChar *string,  int len );
    inline const char *name() const { return 0; }
    TQFontDef fontDef;
    /*TQMemoryManager::FontID*/ void *id;
    int cache_cost;
    int cache_count;
    int scale;
};
#endif // WIN || X11 || MAC



enum IndicFeatures {
    CcmpFeature,
    InitFeature,
    NuktaFeature,
    AkhantFeature,
    RephFeature,
    BelowFormFeature,
    HalfFormFeature,
    PostFormFeature,
    VattuFeature,
    PreSubstFeature,
    AboveSubstFeature,
    BelowSubstFeature,
    PostSubstFeature,
    HalantFeature
};

#if defined(TQ_WS_X11) || defined(TQ_WS_WIN)
class TQFontEngineBox : public TQFontEngine
{
public:
    TQFontEngineBox( int size );
    ~TQFontEngineBox();

    Error stringToCMap( const TQChar *str,  int len, glyph_t *glyphs, advance_t *advances, int *nglyphs, bool mirrored ) const;

    void draw( TQPainter *p, int x, int y, const TQTextEngine *engine, const TQScriptItem *si, int textFlags );

    virtual glyph_metrics_t boundingBox( const glyph_t *glyphs,
				    const advance_t *advances, const qoffset_t *offsets, int numGlyphs );
    glyph_metrics_t boundingBox( glyph_t glyph );

    int ascent() const;
    int descent() const;
    int leading() const;
    int maxCharWidth() const;
    int minLeftBearing() const { return 0; }
    int minRightBearing() const { return 0; }

#ifdef TQ_WS_X11
    int cmap() const;
#endif
    const char *name() const;

    bool canRender( const TQChar *string,  int len );

    Type type() const;
    inline int size() const { return _size; }

private:
    friend class TQFontPrivate;
    int _size;
};
#endif

#ifdef TQ_WS_X11
#include "qt_x11_p.h"


struct TransformedFont
{
    float xx;
    float xy;
    float yx;
    float yy;
    union {
	Font xlfd_font;
#ifndef TQT_NO_XFTFREETYPE
	XftFont *xft_font;
#endif
    };
    TransformedFont *next;
};

#ifndef TQT_NO_XFTFREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#include "ftxopen.h"

class TQTextCodec;

class TQFontEngineXft : public TQFontEngine
{
public:
    TQFontEngineXft( XftFont *font, XftPattern *pattern, int cmap );
    ~TQFontEngineXft();

    TQOpenType *openType() const;

    Error stringToCMap( const TQChar *str,  int len, glyph_t *glyphs, advance_t *advances, int *nglyphs, bool mirrored ) const;

    void draw( TQPainter *p, int x, int y, const TQTextEngine *engine, const TQScriptItem *si, int textFlags );

    virtual glyph_metrics_t boundingBox( const glyph_t *glyphs,
				    const advance_t *advances, const qoffset_t *offsets, int numGlyphs );
    glyph_metrics_t boundingBox( glyph_t glyph );

    int ascent() const;
    int descent() const;
    int leading() const;
    int lineThickness() const;
    int underlinePosition() const;
    int maxCharWidth() const;
    int minLeftBearing() const;
    int minRightBearing() const;

    int cmap() const;
    const char *name() const;

    void setScale( double scale );
    double scale() const { return _scale; }

    bool canRender( const TQChar *string,  int len );

    Type type() const;
    XftPattern *pattern() const { return _pattern; }
    FT_Face face() const { return _face; }
    XftFont *font() const { return _font; }

    void recalcAdvances( int len, glyph_t *glyphs, advance_t *advances ) const;

private:
    friend class TQFontPrivate;
    friend class TQOpenType;
    XftFont *_font;
    XftPattern *_pattern;
    FT_Face _face;
    TQOpenType *_openType;
    int _cmap;
    short lbearing;
    short rbearing;
    float _scale;
    enum { widthCacheSize = 0x800, cmapCacheSize = 0x500 };
    unsigned char widthCache[widthCacheSize];
    glyph_t cmapCache[cmapCacheSize];
};
#endif

class TQFontEngineLatinXLFD;

class TQFontEngineXLFD : public TQFontEngine
{
public:
    TQFontEngineXLFD( XFontStruct *fs, const char *name, int cmap );
    ~TQFontEngineXLFD();

    Error stringToCMap( const TQChar *str,  int len, glyph_t *glyphs, advance_t *advances, int *nglyphs, bool mirrored ) const;

    void draw( TQPainter *p, int x, int y, const TQTextEngine *engine, const TQScriptItem *si, int textFlags );

    virtual glyph_metrics_t boundingBox( const glyph_t *glyphs,
				    const advance_t *advances, const qoffset_t *offsets, int numGlyphs );
    glyph_metrics_t boundingBox( glyph_t glyph );

    int ascent() const;
    int descent() const;
    int leading() const;
    int maxCharWidth() const;
    int minLeftBearing() const;
    int minRightBearing() const;

    int cmap() const;
    const char *name() const;

    bool canRender( const TQChar *string,  int len );

    void setScale( double scale );
    double scale() const { return _scale; }
    Type type() const;

    TQt::HANDLE handle() const { return (TQt::HANDLE) _fs->fid; }

private:
    friend class TQFontPrivate;
    XFontStruct *_fs;
    TQCString _name;
    TQTextCodec *_codec;
    float _scale; // needed for printing, to correctly scale font metrics for bitmap fonts
    int _cmap;
    short lbearing;
    short rbearing;
    enum XlfdTransformations {
	XlfdTrUnknown,
	XlfdTrSupported,
	XlfdTrUnsupported
    };
    XlfdTransformations xlfd_transformations;

    friend class TQFontEngineLatinXLFD;
};

class TQFontEngineLatinXLFD : public TQFontEngine
{
public:
    TQFontEngineLatinXLFD( XFontStruct *xfs, const char *name, int cmap );
    ~TQFontEngineLatinXLFD();

    Error stringToCMap( const TQChar *str,  int len, glyph_t *glyphs,
			advance_t *advances, int *nglyphs, bool mirrored ) const;

    void draw( TQPainter *p, int x, int y, const TQTextEngine *engine,
	       const TQScriptItem *si, int textFlags );

    virtual glyph_metrics_t boundingBox( const glyph_t *glyphs,
					 const advance_t *advances,
					 const qoffset_t *offsets, int numGlyphs );
    glyph_metrics_t boundingBox( glyph_t glyph );

    int ascent() const;
    int descent() const;
    int leading() const;
    int maxCharWidth() const;
    int minLeftBearing() const;
    int minRightBearing() const;

    int cmap() const { return -1; } // ###
    const char *name() const;

    bool canRender( const TQChar *string,  int len );

    void setScale( double scale );
    double scale() const { return _engines[0]->scale(); }
    Type type() const { return LatinXLFD; }

    TQt::HANDLE handle() const { return ((TQFontEngineXLFD *) _engines[0])->handle(); }

private:
    void findEngine( const TQChar &ch );

    TQFontEngine **_engines;
    int _count;

    glyph_t   glyphIndices [0x200];
    advance_t glyphAdvances[0x200];
    glyph_t euroIndex;
    advance_t euroAdvance;
};

class TQScriptItem;
class TQTextEngine;

#ifndef TQT_NO_XFTFREETYPE

#include "qscriptengine_p.h"
#include "qtextengine_p.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "ftxopen.h"

enum { PositioningProperties = 0x80000000 };

class TQOpenType
{
public:
    TQOpenType(TQFontEngineXft *fe);
    ~TQOpenType();

    struct Features {
        uint tag;
        uint property;
    };

    bool supportsScript(unsigned int script) {
        Q_ASSERT(script < TQFont::NScripts);
        return supported_scripts[script];
    }
    void selectScript(unsigned int script, const Features *features = 0);

    bool shape(TQShaperItem *item, const unsigned int *properties = 0);
    bool positionAndAdd(TQShaperItem *item, int availableGlyphs, bool doLogClusters = true);

    OTL_GlyphItem glyphs() const { return otl_buffer->in_string; }
    int len() const { return otl_buffer->in_length; }
    void setProperty(int index, uint property) { otl_buffer->in_string[index].properties = property; }


private:
    bool checkScript(unsigned int script);
    TQFontEngine *fontEngine;
    FT_Face face;
    TTO_GDEF gdef;
    TTO_GSUB gsub;
    TTO_GPOS gpos;
    bool supported_scripts[TQFont::NScripts];
    FT_ULong current_script;
    bool positioned : 1;
    OTL_Buffer otl_buffer;
    GlyphAttributes *tmpAttributes;
    unsigned int *tmpLogClusters;
    int length;
    int orig_nglyphs;
    int loadFlags;
};

#endif // TQT_NO_XFTFREETYPE

#elif defined( TQ_WS_MAC )
#include "qt_mac.h"
#include <ntqmap.h>
#include <ntqcache.h>

class TQFontEngineMac : public TQFontEngine
{
#if 0
    ATSFontMetrics *info;
#else
    FontInfo *info;
#endif
    int psize;
    FMFontFamily fmfam;
    TQMacFontInfo *internal_fi;
    mutable ATSUTextLayout mTextLayout;
    enum { widthCacheSize = 0x500 };
    mutable unsigned char widthCache[widthCacheSize];
    friend class TQFont;
    friend class TQGLContext;
    friend class TQFontPrivate;
    friend class TQMacSetFontInfo;

public:
    TQFontEngineMac();
    ~TQFontEngineMac();

    Error stringToCMap( const TQChar *str, int len, glyph_t *glyphs, advance_t *advances, int *nglyphs, bool mirrored ) const;

    void draw( TQPainter *p, int x, int y, const TQTextEngine *engine, const TQScriptItem *si, int textFlags );

    glyph_metrics_t boundingBox( const glyph_t *glyphs,
			       const advance_t *advances, const qoffset_t *offsets, int numGlyphs );
    glyph_metrics_t boundingBox( glyph_t glyph );

    int ascent() const { return (int)info->ascent; }
    int descent() const { return (int)info->descent; }
    int leading() const { return (int)info->leading; }
#if 0
    int maxCharWidth() const { return (int)info->maxAdvanceWidth; }
#else
    int maxCharWidth() const { return info->widMax; }
#endif

    const char *name() const { return "ATSUI"; }

    bool canRender( const TQChar *string,  int len );

    Type type() const { return TQFontEngine::Mac; }

    void calculateCost();

    enum { WIDTH=0x01, DRAW=0x02, EXISTS=0x04 };
    int doTextTask(const TQChar *s, int pos, int use_len, int len, uchar task, int =-1, int y=-1,
		   TQPaintDevice *dev=NULL, const TQRegion *rgn=NULL) const;
};

#elif defined( TQ_WS_WIN )

class TQFontEngineWin : public TQFontEngine
{
public:
    TQFontEngineWin( const char *name, HDC, HFONT, bool, LOGFONT );

    Error stringToCMap( const TQChar *str, int len, glyph_t *glyphs, advance_t *advances, int *nglyphs, bool mirrored ) const;

    void draw( TQPainter *p, int x, int y, const TQTextEngine *engine, const TQScriptItem *si, int textFlags );

    glyph_metrics_t boundingBox( const glyph_t *glyphs,
			       const advance_t *advances, const qoffset_t *offsets, int numGlyphs );
    glyph_metrics_t boundingBox( glyph_t glyph );

    int ascent() const;
    int descent() const;
    int leading() const;
    int maxCharWidth() const;
    int minLeftBearing() const;
    int minRightBearing() const;

    const char *name() const;

    bool canRender( const TQChar *string,  int len );

    Type type() const;

    enum { widthCacheSize = 0x800, cmapCacheSize = 0x500 };
    unsigned char widthCache[widthCacheSize];
};

#if 0
class TQFontEngineUniscribe : public TQFontEngineWin
{
public:
    void draw( TQPainter *p, int x, int y, const TQTextEngine *engine, const TQScriptItem *si, int textFlags );
    bool canRender( const TQChar *string,  int len );

    Type type() const;
};
#endif

#endif // TQ_WS_WIN

#endif
