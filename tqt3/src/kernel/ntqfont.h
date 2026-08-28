/****************************************************************************
**
** Definition of TQFont class
**
** Created : 940514
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

#ifndef TQFONT_H
#define TQFONT_H

#ifndef QT_H
#include "ntqwindowdefs.h"
#include "ntqstring.h"
#endif // QT_H


class TQFontPrivate;                                     /* don't touch */
class TQStringList;
class TQTextFormatCollection;

class TQ_EXPORT TQFont
{
public:
    enum StyleHint {
	Helvetica,  SansSerif = Helvetica,
	Times,      Serif = Times,
	Courier,    TypeWriter = Courier,
	OldEnglish, Decorative = OldEnglish,
	System,
	AnyStyle
    };

    enum StyleStrategy {
	PreferDefault    = 0x0001,
	PreferBitmap     = 0x0002,
	PreferDevice     = 0x0004,
	PreferOutline    = 0x0008,
	ForceOutline     = 0x0010,
	PreferMatch      = 0x0020,
	PreferQuality    = 0x0040,
	PreferAntialias  = 0x0080,
	NoAntialias      = 0x0100,
	OpenGLCompatible = 0x0200
    };

    enum Weight {
	Light    = 25,
	Normal   = 50,
	DemiBold = 63,
	Bold     = 75,
	Black	 = 87
    };

    enum Stretch {
	UltraCondensed =  50,
	ExtraCondensed =  62,
	Condensed      =  75,
	SemiCondensed  =  87,
	Unstretched    = 100,
	SemiExpanded   = 112,
	Expanded       = 125,
	ExtraExpanded  = 150,
	UltraExpanded  = 200
    };

    // default font
    TQFont();
    // specific font
#ifdef Q_QDOC
    TQFont( const TQString &family, int pointSize = 12, int weight = Normal,
	   bool italic = false );
#else
    TQFont( const TQString &family, int pointSize = -1, int weight = -1,
	   bool italic = false );
#endif
    // copy constructor
    TQFont( const TQFont & );

    ~TQFont();

    TQString family() const;
    void setFamily( const TQString &);

    int pointSize() const;
    float pointSizeFloat() const;
    void setPointSize( int );
    void setPointSizeFloat( float );

    int pixelSize() const;
    void setPixelSize( int );
    void setPixelSizeFloat( float );

    int weight() const;
    void setWeight( int );

    bool bold() const;
    void setBold( bool );

    bool italic() const;
    void setItalic( bool );

    bool underline() const;
    void setUnderline( bool );

    bool overline() const;
    void setOverline( bool );

    bool strikeOut() const;
    void setStrikeOut( bool );

    bool fixedPitch() const;
    void setFixedPitch( bool );

    StyleHint styleHint() const;
    StyleStrategy styleStrategy() const;
    void setStyleHint( StyleHint, StyleStrategy = PreferDefault );
    void setStyleStrategy( StyleStrategy s );

    int stretch() const;
    void setStretch( int );

    // is raw mode still needed?
    bool rawMode() const;
    void setRawMode( bool );

    // dupicated from TQFontInfo
    bool exactMatch() const;

    TQFont &operator=( const TQFont & );
    bool operator==( const TQFont & ) const;
    bool operator!=( const TQFont & ) const;
    bool isCopyOf( const TQFont & ) const;


#ifdef TQ_WS_WIN
    HFONT handle() const;
#else // !TQ_WS_WIN
    TQt::HANDLE handle() const;
#endif // TQ_WS_WIN


    // needed for X11
    void setRawName( const TQString & );
    TQString rawName() const;

    TQString key() const;

    TQString toString() const;
    bool fromString(const TQString &);

#ifndef TQT_NO_STRINGLIST
    static TQString substitute(const TQString &);
    static TQStringList substitutes(const TQString &);
    static TQStringList substitutions();
    static void insertSubstitution(const TQString&, const TQString &);
    static void insertSubstitutions(const TQString&, const TQStringList &);
    static void removeSubstitution(const TQString &);
#endif //TQT_NO_STRINGLIST
    static void initialize();
    static void cleanup();
#ifndef TQ_WS_QWS
    static void cacheStatistics();
#endif

#if defined(TQ_WS_QWS)
    void qwsRenderToDisk(bool all=true);
#endif


    // a copy of this lives in qunicodetables.cpp, as we can't include
    // ntqfont.h it in tools/. Do not modify without changing the script
    // enum in qunicodetable_p.h aswell.
    enum Script {
	// European Alphabetic Scripts
	Latin,
	Greek,
	Cyrillic,
	Armenian,
	Georgian,
	Runic,
	Ogham,
	SpacingModifiers,
	CombiningMarks,

	// Middle Eastern Scripts
	Hebrew,
	Arabic,
	Syriac,
	Thaana,

	// South and Southeast Asian Scripts
	Devanagari,
	Bengali,
	Gurmukhi,
	Gujarati,
	Oriya,
	Tamil,
	Telugu,
	Kannada,
	Malayalam,
	Sinhala,
	Thai,
	Lao,
	Tibetan,
	Myanmar,
	Khmer,

	// East Asian Scripts
	Han,
	Hiragana,
	Katakana,
	Hangul,
	Bopomofo,
	Yi,

	// Additional Scripts
	Ethiopic,
	Cherokee,
	CanadianAboriginal,
	Mongolian,

	// Symbols
	CurrencySymbols,
	LetterlikeSymbols,
	NumberForms,
	MathematicalOperators,
	TechnicalSymbols,
	GeometricSymbols,
	MiscellaneousSymbols,
	EnclosedAndSquare,
	Braille,

	Unicode,

	// some scripts added in Unicode 3.2
	Tagalog,
	Hanunoo,
	Buhid,
	Tagbanwa,

	KatakanaHalfWidth,

	// from Unicode 4.0
	Limbu,
	TaiLe,

	// End
#if !defined(Q_QDOC)
	NScripts,
	UnknownScript = NScripts,

	NoScript,

	// ----------------------------------------
	// Dear User, you can see values > NScript,
	// but they are internal - do not touch.

	Han_Japanese,
	Han_SimplifiedChinese,
	Han_TraditionalChinese,
	Han_Korean,

	LastPrivateScript
#endif
    };

    TQString defaultFamily() const;
    TQString lastResortFamily() const;
    TQString lastResortFont() const;

#ifndef TQT_NO_COMPAT

    static TQFont defaultFont();
    static void setDefaultFont( const TQFont & );

#endif // TQT_NO_COMPAT

    TQFont resolve( const TQFont & ) const;

protected:
    // why protected?
    bool dirty() const;
    int deciPointSize() const;

private:
    TQFont( TQFontPrivate *, TQPaintDevice *pd );

    void detach();

#if defined(TQ_WS_MAC)
    void macSetFont(TQPaintDevice *);
#elif defined(TQ_WS_X11)
    void x11SetScreen( int screen = -1 );
    int x11Screen() const;
#endif

    friend class TQFontMetrics;
    friend class TQFontInfo;
    friend class TQPainter;
    friend class TQPSPrinterFont;
    friend class TQApplication;
    friend class TQWidget;
    friend class TQTextFormatCollection;
    friend class TQTextLayout;
    friend class TQTextItem;
    friend class TQGLContext;
#if defined(TQ_WS_X11) && !defined(TQT_NO_XFTFREETYPE)
    friend TQt::HANDLE qt_xft_handle(const TQFont &font);
#endif
#ifndef TQT_NO_DATASTREAM
    friend TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQFont & );
    friend TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQFont & );
#endif

    TQFontPrivate *d;
};


inline bool TQFont::bold() const
{ return weight() > Normal; }


inline void TQFont::setBold( bool enable )
{ setWeight( enable ? Bold : Normal ); }




/*****************************************************************************
  TQFont stream functions
 *****************************************************************************/

#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQFont & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQFont & );
#endif


#endif // TQFONT_H
