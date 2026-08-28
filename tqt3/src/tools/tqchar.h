/****************************************************************************
**
** Definition of the TQChar class, and related Unicode functions.
**
** Created : 920609
**
** Copyright (C) 2015 Timothy Pearson. All rights reserved.
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the tools module of the TQt GUI Toolkit.
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

#ifndef TQCHAR_H
#define TQCHAR_H

#ifndef QT_H
#include "tqglobal.h"
#endif // QT_H

class TQString;

class TQ_EXPORT TQChar
{
public:
    TQChar();
    TQChar( char c );
    TQChar( uchar c );
    TQChar( uchar c, uchar r );
    TQChar( const TQChar& c ); // ### remove in 4.0 to allow compiler optimization
    TQChar( ushort rc );
    TQChar( short rc );
    TQChar( uint rc );
    TQChar( int rc );

    static const TQChar null;            // 0000
    static const TQChar replacement;     // FFFD
    static const TQChar byteOrderMark;     // FEFF
    static const TQChar byteOrderSwapped;     // FFFE
    static const TQChar nbsp;            // 00A0

    // Unicode information

    enum Category
    {
        NoCategory,

        Mark_NonSpacing,          //   Mn
        Mark_SpacingCombining,    //   Mc
        Mark_Enclosing,           //   Me

        Number_DecimalDigit,      //   Nd
        Number_Letter,            //   Nl
        Number_Other,             //   No

        Separator_Space,          //   Zs
        Separator_Line,           //   Zl
        Separator_Paragraph,      //   Zp

        Other_Control,            //   Cc
        Other_Format,             //   Cf
        Other_Surrogate,          //   Cs
        Other_PrivateUse,         //   Co
        Other_NotAssigned,        //   Cn

        Letter_Uppercase,         //   Lu
        Letter_Lowercase,         //   Ll
        Letter_Titlecase,         //   Lt
        Letter_Modifier,          //   Lm
        Letter_Other,             //   Lo

        Punctuation_Connector,    //   Pc
        Punctuation_Dash,         //   Pd
        Punctuation_Dask = Punctuation_Dash, // oops
        Punctuation_Open,         //   Ps
        Punctuation_Close,        //   Pe
        Punctuation_InitialQuote, //   Pi
        Punctuation_FinalQuote,   //   Pf
        Punctuation_Other,        //   Po

        Symbol_Math,              //   Sm
        Symbol_Currency,          //   Sc
        Symbol_Modifier,          //   Sk
        Symbol_Other              //   So
    };

    enum Direction
    {
        DirL, DirR, DirEN, DirES, DirET, DirAN, DirCS, DirB, DirS, DirWS, DirON,
        DirLRE, DirLRO, DirAL, DirRLE, DirRLO, DirPDF, DirNSM, DirBN
    };

    enum Decomposition
    {
        Single, Canonical, Font, NoBreak, Initial, Medial,
        Final, Isolated, Circle, Super, Sub, Vertical,
        Wide, Narrow, Small, Square, Compat, Fraction
    };

    enum Joining
    {
        OtherJoining, Dual, Right, Center
    };

    enum CombiningClass
    {
        Combining_BelowLeftAttached       = 200,
        Combining_BelowAttached           = 202,
        Combining_BelowRightAttached      = 204,
        Combining_LeftAttached            = 208,
        Combining_RightAttached           = 210,
        Combining_AboveLeftAttached       = 212,
        Combining_AboveAttached           = 214,
        Combining_AboveRightAttached      = 216,

        Combining_BelowLeft               = 218,
        Combining_Below                   = 220,
        Combining_BelowRight              = 222,
        Combining_Left                    = 224,
        Combining_Right                   = 226,
        Combining_AboveLeft               = 228,
        Combining_Above                   = 230,
        Combining_AboveRight              = 232,

        Combining_DoubleBelow             = 233,
        Combining_DoubleAbove             = 234,
        Combining_IotaSubscript           = 240
    };

    // ****** WHEN ADDING FUNCTIONS, CONSIDER ADDING TO TQCharRef TOO

    int digitValue() const;
    TQChar lower() const;
    TQChar upper() const;

    Category category() const;
    Direction direction() const;
    Joining joining() const;
    bool mirrored() const;
    TQChar mirroredChar() const;
    const TQString &decomposition() const; // ### return just TQString in 4.0
    Decomposition decompositionTag() const;
    unsigned char combiningClass() const;

    char latin1() const { return ucs > 0xff ? 0 : (char) ucs; }
    ushort unicode() const { return ucs; }
#ifdef Q_NO_PACKED_REFERENCE
    ushort &unicode() { return *((ushort*)&ucs); }
#else
    ushort &unicode() { return ucs; }
#endif
#ifndef TQT_NO_CAST_ASCII
    // like all ifdef'd code this is undocumented
    operator char() const { return latin1(); }
#endif

    bool isNull() const { return unicode()==0; }
    bool isPrint() const;
    bool isPunct() const;
    bool isSpace() const;
    bool isMark() const;
    bool isLetter() const;
    bool isNumber() const;
    bool isLetterOrNumber() const;
    bool isDigit() const;
    bool isSymbol() const;

    // Surrogate pairs support
    bool isHighSurrogate() const;
    bool isLowSurrogate() const;
    static bool requiresSurrogates(uint ucs4);
    static ushort highSurrogate(uint ucs4);
    static ushort lowSurrogate(uint ucs4);
    static uint surrogateToUcs4(const TQChar &high, const TQChar &low);

    uchar cell() const { return ((uchar) ucs & 0xff); }
    uchar row() const { return ((uchar) (ucs>>8)&0xff); }
    void setCell( uchar cell ) { ucs = (ucs & 0xff00) + cell; }
    void setRow( uchar row ) { ucs = (((ushort) row)<<8) + (ucs&0xff); }

    static bool networkOrdered() {
	int wordSize;
	bool bigEndian = false;
	tqSysInfo( &wordSize, &bigEndian );
	return bigEndian;
    }

    friend inline bool operator==( char ch, TQChar c );
    friend inline bool operator==( TQChar c, char ch );
    friend inline bool operator==( TQChar c1, TQChar c2 );
    friend inline bool operator!=( TQChar c1, TQChar c2 );
    friend inline bool operator!=( char ch, TQChar c );
    friend inline bool operator!=( TQChar c, char ch );
    friend inline bool operator<=( TQChar c, char ch );
    friend inline bool operator<=( char ch, TQChar c );
    friend inline bool operator<=( TQChar c1, TQChar c2 );

#if defined(__cplusplus) && __cplusplus >= 201103L
    // Explicit declarations to suppress warnings
    // This could be removed when TQChar( const TQChar& c ) is removed
    TQChar& operator=(const TQChar& other) = default;
    ~TQChar() = default;
#endif
private:
    ushort ucs;
} TQ_PACKED;


inline TQChar::TQChar() : ucs( 0 )
{
}

inline TQChar::TQChar( char c ) : ucs( (uchar)c )
{
}

inline TQChar::TQChar( uchar c ) : ucs( c )
{
}

inline TQChar::TQChar( uchar c, uchar r ) : ucs( (r << 8) | c )
{
}

inline TQChar::TQChar( const TQChar& c ) : ucs( c.ucs )
{
}

inline TQChar::TQChar( ushort rc ) : ucs( rc )
{
}

inline TQChar::TQChar( short rc ) : ucs( (ushort) rc )
{
}

inline TQChar::TQChar( uint rc ) : ucs( (ushort) ( rc ) )
{
}

inline TQChar::TQChar( int rc ) : ucs( (ushort) ( rc ) )
{
}

inline bool TQChar::isHighSurrogate() const
{
    return ((ucs & 0xfc00) == 0xd800);
}

inline bool TQChar::isLowSurrogate() const
{
    return ((ucs & 0xfc00) == 0xdc00);
}

inline bool TQChar::requiresSurrogates(uint ucs4)
{
    return (ucs4 >= 0x10000);
}

inline ushort TQChar::highSurrogate(uint ucs4)
{
    return ushort(((ucs4 - 0x10000) >> 10)) | 0xd800;
}

inline ushort TQChar::lowSurrogate(uint ucs4)
{
    return ushort(ucs4 & 0x03FF) | 0xdc00;
}

inline uint TQChar::surrogateToUcs4(const TQChar &high, const TQChar &low)
{
    return (uint(high.ucs & 0x03FF) << 10) | (low.ucs & 0x03FF) | 0x10000;
}

inline bool operator==( char ch, TQChar c )
{
    return ((uchar) ch) == c.ucs;
}

inline bool operator==( TQChar c, char ch )
{
    return ((uchar) ch) == c.ucs;
}

inline bool operator==( TQChar c1, TQChar c2 )
{
    return c1.ucs == c2.ucs;
}

inline bool operator!=( TQChar c1, TQChar c2 )
{
    return c1.ucs != c2.ucs;
}

inline bool operator!=( char ch, TQChar c )
{
    return ((uchar)ch) != c.ucs;
}

inline bool operator!=( TQChar c, char ch )
{
    return ((uchar) ch) != c.ucs;
}

inline bool operator<=( TQChar c, char ch )
{
    return c.ucs <= ((uchar) ch);
}

inline bool operator<=( char ch, TQChar c )
{
    return ((uchar) ch) <= c.ucs;
}

inline bool operator<=( TQChar c1, TQChar c2 )
{
    return c1.ucs <= c2.ucs;
}

inline bool operator>=( TQChar c, char ch ) { return ch <= c; }
inline bool operator>=( char ch, TQChar c ) { return c <= ch; }
inline bool operator>=( TQChar c1, TQChar c2 ) { return c2 <= c1; }
inline bool operator<( TQChar c, char ch ) { return !(ch<=c); }
inline bool operator<( char ch, TQChar c ) { return !(c<=ch); }
inline bool operator<( TQChar c1, TQChar c2 ) { return !(c2<=c1); }
inline bool operator>( TQChar c, char ch ) { return !(ch>=c); }
inline bool operator>( char ch, TQChar c ) { return !(c>=ch); }
inline bool operator>( TQChar c1, TQChar c2 ) { return !(c2>=c1); }

#endif // TQCHAR_H
