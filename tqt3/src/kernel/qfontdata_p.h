/****************************************************************************
**
** Definition of internal TQFontData struct
**
** Created : 941229
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

#ifndef TQFONTDATA_P_H
#define TQFONTDATA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of internal files.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//
//

#include "ntqobject.h"
#include "ntqfont.h"
#include "ntqpaintdevicemetrics.h"

// forwards
class TQFontEngine;
class TQPaintDevice;


struct TQFontDef
{
    inline TQFontDef()
	: pointSize( -1 ), pixelSize( -1 ),
	  styleHint( TQFont::AnyStyle ), styleStrategy( TQFont::PreferDefault ),
	  weight( 50 ), italic( false ), fixedPitch( false ), stretch( 100 ),
	  ignorePitch(true)
#ifdef TQ_WS_MAC
	  ,fixedPitchComputed(false)
#endif
    {
    }

    TQString family;

#ifdef TQ_WS_X11
    TQString addStyle;
#endif // TQ_WS_X11

    int pointSize;
    int pixelSize;

    uint styleHint     : 8;
    uint styleStrategy : 16;

    uint weight     :  7; // 0-99
    uint italic     :  1;
    uint fixedPitch :  1;
    uint stretch    : 12; // 0-400

    uint ignorePitch : 1;
    uint fixedPitchComputed : 1; // for Mac OS X only
    uint reserved   : 14; // for future extensions

    bool operator==( const TQFontDef &other ) const;
    inline bool operator<( const TQFontDef &other ) const
    {
	if ( pixelSize != other.pixelSize ) return pixelSize < other.pixelSize;
	if ( weight != other.weight ) return weight < other.weight;
	if ( italic != other.italic ) return italic < other.italic;
	if ( stretch != other.stretch ) return stretch < other.stretch;
	if ( styleHint != other.styleHint ) return styleHint < other.styleHint;
	if ( styleStrategy != other.styleStrategy ) return styleStrategy < other.styleStrategy;
	if ( family != other.family ) return family < other.family;

#ifdef TQ_WS_X11
	if ( addStyle != other.addStyle ) return addStyle < other.addStyle;
#endif // TQ_WS_X11

	return false;
    }
};

class TQFontEngineData : public TQShared
{
public:
    TQFontEngineData();
    ~TQFontEngineData();

    uint lineWidth;

#if defined(TQ_WS_X11) || defined(TQ_WS_WIN)
    TQFontEngine *engines[TQFont::LastPrivateScript];
#else
    TQFontEngine *engine;
#endif // TQ_WS_X11 || TQ_WS_WIN
#ifndef TQ_WS_MAC
    enum { widthCacheSize = 0x500 };
    uchar widthCache[widthCacheSize];
#endif
};


class TQFontPrivate : public TQShared
{
public:
    static TQFont::Script defaultScript;
#ifdef TQ_WS_X11
    static int defaultEncodingID;
#endif // TQ_WS_X11

    TQFontPrivate();
    TQFontPrivate( const TQFontPrivate &other );
    ~TQFontPrivate();

    void load( TQFont::Script script );
    TQFontEngine *engineForScript( TQFont::Script script ) const {
	if ( script == TQFont::NoScript )
	    script = TQFontPrivate::defaultScript;
#if defined(TQ_WS_X11) || defined(TQ_WS_WIN)
	if ( ! engineData || ! engineData->engines[script] )
	    ((TQFontPrivate *) this)->load( script );
	return engineData->engines[script];
#else
        if ( ! engineData || ! engineData->engine )
	    ((TQFontPrivate *) this)->load( script );
        return engineData->engine;
#endif // TQ_WS_X11 || TQ_WS_WIN
    }

    TQFontDef request;
    TQFontEngineData *engineData;
    TQPaintDevice *paintdevice;
    int screen;

    uint rawMode    :  1;
    uint underline  :  1;
    uint overline   :  1;
    uint strikeOut  :  1;

    enum {
	Family        = 0x0001,
	Size          = 0x0002,
	StyleHint     = 0x0004,
	StyleStrategy = 0x0008,
	Weight        = 0x0010,
	Italic        = 0x0020,
	Underline     = 0x0040,
	Overline      = 0x0080,
	StrikeOut     = 0x0100,
	FixedPitch    = 0x0200,
	Stretch       = 0x0400,
	Complete      = 0x07ff
    };

    uint mask;

    void resolve( const TQFontPrivate *other );
};


class TQFontCache : public TQObject
{
public:
    static TQFontCache *instance;

    TQFontCache();
    ~TQFontCache();

#ifdef TQ_WS_QWS
    void clear();
#endif
    // universal key structure.  TQFontEngineDatas and TQFontEngines are cached using
    // the same keys
    struct Key {
	Key() : script(0), screen( 0 ), dpi(0) { }
	Key( const TQFontDef &d, TQFont::Script c, int s, TQPaintDevice *pdev )
            : script(c), screen(s) {
            def = d;
#ifdef TQ_WS_X11
            dpi = pdev ? TQPaintDeviceMetrics(pdev).logicalDpiY() : 0;
#else
            Q_UNUSED(pdev);
            dpi = 0;
#endif
        }

	TQFontDef def;
	int script;
	int screen;
        int dpi;

	inline bool operator<( const Key &other ) const
	{
	    if ( script != other.script ) return script < other.script;
	    if ( screen != other.screen ) return screen < other.screen;
	    if ( dpi != other.dpi ) return dpi < other.dpi;
	    return def < other.def;
	}
	inline bool operator==( const Key &other ) const
	{ return def == other.def && script == other.script &&
                screen == other.screen && dpi == other.dpi; }
    };

    // TQFontEngineData cache
    typedef TQMap<Key,TQFontEngineData*> EngineDataCache;
    EngineDataCache engineDataCache;

    TQFontEngineData *findEngineData( const Key &key ) const;
    void insertEngineData( const Key &key, TQFontEngineData *engineData );

    // TQFontEngine cache
    struct Engine {
	Engine() : data( 0 ), timestamp( 0 ), hits( 0 ) { }
	Engine( TQFontEngine *d ) : data( d ), timestamp( 0 ), hits( 0 ) { }

	TQFontEngine *data;
	uint timestamp;
	uint hits;
    };

    typedef TQMap<Key,Engine> EngineCache;
    EngineCache engineCache;

    TQFontEngine *findEngine( const Key &key );
    void insertEngine( const Key &key, TQFontEngine *engine );

#if defined(TQ_WS_WIN) || defined(TQ_WS_QWS)
    void cleanupPrinterFonts();
#endif

    private:
    void increaseCost( uint cost );
    void decreaseCost( uint cost );
    void timerEvent( TQTimerEvent *event );

    static const uint min_cost;
    uint total_cost, max_cost;
    uint current_timestamp;
    bool fast;
    int timer_id;
};

#endif // TQFONTDATA_P_H
