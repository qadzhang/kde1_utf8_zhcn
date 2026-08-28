/****************************************************************************
**
** Definition of the TQString class, and related Unicode functions.
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

#ifndef TQSTRING_H
#define TQSTRING_H

#ifndef QT_H
#include "ntqcstring.h"
#endif // QT_H

#ifndef TQT_NO_CAST_ASCII
#include <limits.h>
#endif

#ifndef TQT_NO_STL
#if defined ( Q_CC_MSVC_NET ) && _MSC_VER < 1310 // Avoids nasty warning for xlocale, line 450
#  pragma warning ( push )
#  pragma warning ( disable : 4189 )
#  include <string>
#  pragma warning ( pop )
#else
#  include <string>
#endif
#if defined(Q_WRONG_SB_CTYPE_MACROS) && defined(_SB_CTYPE_MACROS)
#undef _SB_CTYPE_MACROS
#endif
#endif

#ifndef TQT_NO_SPRINTF
#include <stdarg.h>
#endif

/*****************************************************************************
  TQString class
 *****************************************************************************/

class TQRegExp;
class TQString;
class TQCharRef;
class TQMutex;
template <class T> class TQDeepCopy;

class TQ_EXPORT TQChar {
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
#if defined(QT_QSTRING_UCS_4)
    ushort grp;
#endif
} TQ_PACKED;

inline TQChar::TQChar() : ucs( 0 )
#ifdef QT_QSTRING_UCS_4
    , grp( 0 )
#endif
{
}
inline TQChar::TQChar( char c ) : ucs( (uchar)c )
#ifdef QT_QSTRING_UCS_4
    , grp( 0 )
#endif
{
}
inline TQChar::TQChar( uchar c ) : ucs( c )
#ifdef QT_QSTRING_UCS_4
    , grp( 0 )
#endif
{
}
inline TQChar::TQChar( uchar c, uchar r ) : ucs( (r << 8) | c )
#ifdef QT_QSTRING_UCS_4
    , grp( 0 )
#endif
{
}
inline TQChar::TQChar( const TQChar& c ) : ucs( c.ucs )
#ifdef QT_QSTRING_UCS_4
   , grp( c.grp )
#endif
{
}

inline TQChar::TQChar( ushort rc ) : ucs( rc )
#ifdef QT_QSTRING_UCS_4
    , grp( 0 )
#endif
{
}
inline TQChar::TQChar( short rc ) : ucs( (ushort) rc )
#ifdef QT_QSTRING_UCS_4
    , grp( 0 )
#endif
{
}
inline TQChar::TQChar( uint rc ) : ucs(  (ushort ) (rc & 0xffff) )
#ifdef QT_QSTRING_UCS_4
    , grp( (ushort) ((rc >> 16) & 0xffff) )
#endif
{
}
inline TQChar::TQChar( int rc ) : ucs( (ushort) (rc & 0xffff) )
#ifdef QT_QSTRING_UCS_4
    , grp( (ushort) ((rc >> 16) & 0xffff) )
#endif
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

// internal
struct TQ_EXPORT TQStringData : public TQShared {
    TQStringData();
    TQStringData(TQChar *u, uint l, uint m);
    ~TQStringData();

    void deleteSelf();
    TQChar *unicode;
    char *ascii;
    void setDirty();
#ifdef Q_OS_MAC9
    uint len;
#else
    uint len : 30;
#endif
    uint issimpletext : 1;
#ifdef Q_OS_MAC9
    uint maxl;
#else
    uint maxl : 30;
#endif
    uint islatin1 : 1;

    bool security_unpaged : 1;

    TQMutex* mutex;
    TQCString *cString;

private:
#if defined(TQ_DISABLE_COPY)
    TQStringData( const TQStringData& );
    TQStringData& operator=( const TQStringData& );
#endif
};


class TQ_EXPORT TQString
{
public:
    TQString();                                  // make null string
    TQString( TQChar );                           // one-char string
    TQString( const TQString & );                 // impl-shared copy
    TQString( const TQByteArray& );               // deep copy
    TQString( const TQChar* unicode, uint length ); // deep copy
#ifndef TQT_NO_CAST_ASCII
    TQString( const char *str );                 // deep copy
#endif
#ifndef TQT_NO_STL
    TQString( const std::string& );                   // deep copy
#endif
    ~TQString();

    TQString    &operator=( const TQString & );   // impl-shared copy
    TQString    &operator=( const char * );      // deep copy
#ifndef TQT_NO_STL
    TQString    &operator=( const std::string& );     // deep copy
#endif
    TQString    &operator=( const TQCString& );   // deep copy
    TQString    &operator=( TQChar c );
    TQString    &operator=( char c );

    static const TQString null;

    bool        isNull()        const;
    bool        isEmpty()       const;
    uint        length()        const;
    void        truncate( uint pos );

    TQString &   fill( TQChar c, int len = -1 );

    TQString     copy()  const;

    TQString arg( long a, int fieldWidth = 0, int base = 10 ) const;
    TQString arg( ulong a, int fieldWidth = 0, int base = 10 ) const;
    TQString arg( TQ_LLONG a, int fieldwidth=0, int base=10 ) const;
    TQString arg( TQ_ULLONG a, int fieldwidth=0, int base=10 ) const;
    TQString arg( int a, int fieldWidth = 0, int base = 10 ) const;
    TQString arg( uint a, int fieldWidth = 0, int base = 10 ) const;
    TQString arg( short a, int fieldWidth = 0, int base = 10 ) const;
    TQString arg( ushort a, int fieldWidth = 0, int base = 10 ) const;
    TQString arg( double a, int fieldWidth = 0, char fmt = 'g',
		 int prec = -1 ) const;
    TQString arg( char a, int fieldWidth = 0 ) const;
    TQString arg( TQChar a, int fieldWidth = 0 ) const;
    TQString arg( const TQString& a, int fieldWidth = 0 ) const;
    TQString arg( const TQString& a1, const TQString& a2 ) const;
    TQString arg( const TQString& a1, const TQString& a2,
		 const TQString& a3 ) const;
    TQString arg( const TQString& a1, const TQString& a2, const TQString& a3,
		 const TQString& a4 ) const;

#ifndef TQT_NO_SPRINTF
    TQString    &sprintf( const char* format, ... )
#if defined(Q_CC_GNU) && !defined(__INSURE__)
        __attribute__ ((format (printf, 2, 3)))
#endif
        ;
    TQString    &vsprintf(const char *format, va_list ap)
#if defined(Q_CC_GNU) && !defined(__INSURE__)
        __attribute__ ((format (printf, 2, 0)))
#endif
        ;
#endif

    int         find( TQChar c, int index=0, bool cs=true ) const;
    int         find( char c, int index=0, bool cs=true ) const;
    int         find( const TQString &str, int index=0, bool cs=true ) const;
#ifndef TQT_NO_REGEXP
    int         find( const TQRegExp &, int index=0 ) const;
#endif
#ifndef TQT_NO_CAST_ASCII
    int         find( const char* str, int index=0 ) const;
#endif
    int         findRev( TQChar c, int index=-1, bool cs=true) const;
    int         findRev( char c, int index=-1, bool cs=true) const;
    int         findRev( const TQString &str, int index=-1, bool cs=true) const;
#ifndef TQT_NO_REGEXP
    int         findRev( const TQRegExp &, int index=-1 ) const;
#endif
#ifndef TQT_NO_CAST_ASCII
    int         findRev( const char* str, int index=-1 ) const;
#endif
    int         contains( TQChar c, bool cs=true ) const;
    int         contains( char c, bool cs=true ) const
                    { return contains(TQChar(c), cs); }
#ifndef TQT_NO_CAST_ASCII
    int         contains( const char* str, bool cs=true ) const;
#endif
    int         contains( const TQString &str, bool cs=true ) const;
#ifndef TQT_NO_REGEXP
    int         contains( const TQRegExp & ) const;
#endif

    enum SectionFlags {
	SectionDefault             = 0x00,
	SectionSkipEmpty           = 0x01,
	SectionIncludeLeadingSep   = 0x02,
	SectionIncludeTrailingSep  = 0x04,
	SectionCaseInsensitiveSeps = 0x08
    };
    TQString     section( TQChar sep, int start, int end = 0xffffffff, int flags = SectionDefault ) const;
    TQString     section( char sep, int start, int end = 0xffffffff, int flags = SectionDefault ) const;
#ifndef TQT_NO_CAST_ASCII
    TQString      section( const char *in_sep, int start, int end = 0xffffffff, int flags = SectionDefault ) const;
#endif
    TQString     section( const TQString &in_sep, int start, int end = 0xffffffff, int flags = SectionDefault ) const;
#ifndef TQT_NO_REGEXP
    TQString     section( const TQRegExp &reg, int start, int end = 0xffffffff, int flags = SectionDefault ) const;
#endif

    TQString     left( uint len )  const;
    TQString     right( uint len ) const;
    TQString     mid( uint index, uint len=0xffffffff) const;

    TQString     leftJustify( uint width, TQChar fill=' ', bool trunc=false)const;
    TQString     rightJustify( uint width, TQChar fill=' ',bool trunc=false)const;

    TQString     lower() const;
    TQString     upper() const;

    TQString     stripWhiteSpace()       const;
    TQString     simplifyWhiteSpace()    const;

    TQString    &insert( uint index, const TQString & );
#ifndef TQT_NO_CAST_ASCII
    TQString    &insert( uint index, const TQByteArray & );
    TQString    &insert( uint index, const char * );
#endif
    TQString    &insert( uint index, const TQChar*, uint len );
    TQString    &insert( uint index, TQChar );
    TQString    &insert( uint index, char c ) { return insert(index,TQChar(c)); }
    TQString    &append( char );
    TQString    &append( TQChar );
    TQString    &append( const TQString & );
#ifndef TQT_NO_CAST_ASCII
    TQString    &append( const TQByteArray & );
    TQString    &append( const char * );
#endif
#if !defined(TQT_NO_STL) && !defined(TQT_NO_CAST_ASCII)
    TQString    &append( const std::string& );
#endif
    TQString    &prepend( char );
    TQString    &prepend( TQChar );
    TQString    &prepend( const TQString & );
#ifndef TQT_NO_CAST_ASCII
    TQString    &prepend( const TQByteArray & );
    TQString    &prepend( const char * );
#endif
#if !defined(TQT_NO_STL) && !defined(TQT_NO_CAST_ASCII)
    TQString    &prepend( const std::string& );
#endif
    TQString    &remove( uint index, uint len );
#if defined(Q_QDOC)
    TQString    &remove( const TQString & str, bool cs = true );
#else
    // ### TQt 4.0: merge these two into one, and remove Q_QDOC hack
    TQString    &remove( const TQString & );
    TQString    &remove( const TQString &, bool cs );
#endif
    TQString    &remove( TQChar c );
    TQString    &remove( char c )
    { return remove( TQChar(c) ); }
#ifndef TQT_NO_CAST_ASCII
    TQString    &remove( const char * );
#endif
#ifndef TQT_NO_REGEXP
    TQString    &remove( const TQRegExp & );
#endif
    TQString    &replace( uint index, uint len, const TQString & );
    TQString    &replace( uint index, uint len, const TQChar*, uint clen );
    TQString    &replace( uint index, uint len, TQChar );
    TQString    &replace( uint index, uint len, char c )
    { return replace( index, len, TQChar(c) ); }
#if defined(Q_QDOC)
    TQString    &replace( TQChar c, const TQString & after, bool cs = true );
    TQString    &replace( char c, const TQString & after, bool cs = true );
    TQString    &replace( const TQString & before, const TQString & after,
			 bool cs = true );
#else
    // ### TQt 4.0: merge these two into one, and remove Q_QDOC hack
    TQString    &replace( TQChar c, const TQString & );
    TQString    &replace( TQChar c, const TQString &, bool );

    // ### TQt 4.0: merge these two into one, and remove Q_QDOC hack
    TQString    &replace( char c, const TQString & after )
    { return replace( TQChar(c), after, true ); }
    TQString    &replace( char c, const TQString & after, bool cs )
    { return replace( TQChar(c), after, cs ); }

    // ### TQt 4.0: merge these two into one, and remove Q_QDOC hack
    TQString    &replace( const TQString &, const TQString & );
    TQString    &replace( const TQString &, const TQString &, bool );
#endif
#ifndef TQT_NO_REGEXP_CAPTURE
    TQString    &replace( const TQRegExp &, const TQString & );
#endif
    TQString    &replace( TQChar, TQChar );

    short       toShort( bool *ok=0, int base=10 )      const;
    ushort      toUShort( bool *ok=0, int base=10 )     const;
    int         toInt( bool *ok=0, int base=10 )        const;
    uint        toUInt( bool *ok=0, int base=10 )       const;
    long        toLong( bool *ok=0, int base=10 )       const;
    ulong       toULong( bool *ok=0, int base=10 )      const;
    TQ_LLONG     toLongLong( bool *ok=0, int base=10 )   const;
    TQ_ULLONG    toULongLong( bool *ok=0, int base=10 )  const;
    float       toFloat( bool *ok=0 )   const;
    double      toDouble( bool *ok=0 )  const;

    TQString    &setNum( short, int base=10 );
    TQString    &setNum( ushort, int base=10 );
    TQString    &setNum( int, int base=10 );
    TQString    &setNum( uint, int base=10 );
    TQString    &setNum( long, int base=10 );
    TQString    &setNum( ulong, int base=10 );
    TQString    &setNum( TQ_LLONG, int base=10 );
    TQString    &setNum( TQ_ULLONG, int base=10 );
    TQString    &setNum( float, char f='g', int prec=6 );
    TQString    &setNum( double, char f='g', int prec=6 );

    static TQString number( long, int base=10 );
    static TQString number( ulong, int base=10);
    static TQString number( TQ_LLONG, int base=10 );
    static TQString number( TQ_ULLONG, int base=10);
    static TQString number( int, int base=10 );
    static TQString number( uint, int base=10);
    static TQString number( double, char f='g', int prec=6 );

    void        setExpand( uint index, TQChar c );

    TQString    &operator+=( const TQString &str );
#ifndef TQT_NO_CAST_ASCII
    TQString    &operator+=( const TQByteArray &str );
    TQString    &operator+=( const char *str );
#endif
#if !defined(TQT_NO_STL) && !defined(TQT_NO_CAST_ASCII)
    TQString    &operator+=( const std::string& );
#endif
    TQString    &operator+=( TQChar c );
    TQString    &operator+=( char c );

    TQChar at( uint i ) const
        { return i < d->len ? d->unicode[i] : TQChar::null; }
    TQChar operator[]( int i ) const { return at((uint)i); }
    TQCharRef at( uint i );
    TQCharRef operator[]( int i );

    TQChar constref(uint i) const
        { return at(i); }
    TQChar& ref(uint i);

    const TQChar* unicode() const { return d->unicode; }
    const char* ascii() const;
    static TQString fromAscii(const char*, int len=-1);
    const char* latin1() const;
    static TQString fromLatin1(const char*, int len=-1);
    TQCString utf8() const;
    static TQString fromUtf8(const char*, int len=-1);
    TQCString local8Bit() const;
    static TQString fromLocal8Bit(const char*, int len=-1);
    bool operator!() const;
#ifndef TQT_NO_ASCII_CAST
    operator const char *() const { return ascii(); }
#endif
#ifndef TQT_NO_STL
    operator std::string() const { return ascii() ? ascii() : ""; }
#endif

    static TQString fromUcs2( const unsigned short *ucs2 );
    const unsigned short *ucs2() const;

    TQString &setUnicode( const TQChar* unicode, uint len );
    TQString &setUnicodeCodes( const ushort* unicode_as_ushorts, uint len );
    TQString &setAscii( const char*, int len=-1 );
    TQString &setLatin1( const char*, int len=-1 );

    int compare( const TQString& s ) const;
    static int compare( const TQString& s1, const TQString& s2 )
    { return s1.compare( s2 ); }

    int localeAwareCompare( const TQString& s ) const;
    static int localeAwareCompare( const TQString& s1, const TQString& s2 )
    { return s1.localeAwareCompare( s2 ); }

#ifndef TQT_NO_DATASTREAM
    friend TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQString & );
#endif

    void compose();

#ifndef TQT_NO_COMPAT
    const char* data() const { return ascii(); }
#endif

#if defined(Q_QDOC)
    bool startsWith( const TQString& str, bool cs = true ) const;
    bool endsWith( const TQString& str, bool cs = true ) const;
#else
    // ### TQt 4.0: merge these two into one, and remove Q_QDOC hack
    bool startsWith( const TQString& str ) const;
    bool startsWith( const TQString& str, bool cs ) const;

    // ### TQt 4.0: merge these two into one, and remove Q_QDOC hack
    bool endsWith( const TQString& str ) const;
    bool endsWith( const TQString& str, bool cs ) const;
#endif

    void setLength( uint newLength );

    uint capacity() const;
    void reserve( uint minCapacity );
    void squeeze();

    bool simpleText() const { if ( !d->issimpletext ) checkSimpleText(); return (bool)d->issimpletext; }
    bool isRightToLeft() const;


private:
    TQString( int size, bool /* dummy */ );	// allocate size incl. \0

    void deref();
    void real_detach();
    void subat( uint );
    TQString multiArg( int numArgs, const TQString& a1, const TQString& a2,
		      const TQString& a3 = TQString::null,
		      const TQString& a4 = TQString::null ) const;

    void checkSimpleText() const;
    void grow( uint newLength );
#ifndef TQT_NO_CAST_ASCII
    TQString &insertHelper( uint index, const char *s, uint len=UINT_MAX );
    TQString &operatorPlusEqHelper( const char *s, uint len2=UINT_MAX );
#endif

    static TQChar* latin1ToUnicode( const char*, uint * len, uint maxlen=(uint)-1 );
    static TQChar* latin1ToUnicode( const TQByteArray&, uint * len );
    static char* unicodeToLatin1( const TQChar*, uint len );

    TQStringData *d;
    static TQStringData* shared_null;
    static TQStringData* makeSharedNull();

    friend class TQConstString;
    friend class TQTextStream;
    TQString( TQStringData* dd, bool /* dummy */ );

    // needed for TQDeepCopy
    void detach();

    void setSecurityUnPaged(bool lock);
    static char* unicodeToLatin1( const TQChar*, uint len, bool unpaged );

    friend class TQDeepCopy<TQString>;
    friend class TQLineEdit;
};

class TQ_EXPORT TQCharRef {
    friend class TQString;
    TQString& s;
    uint p;
    TQCharRef(TQString* str, uint pos) : s(*str), p(pos) { }

public:
    // most TQChar operations repeated here

    // all this is not documented: We just say "like TQChar" and let it be.
#ifndef Q_QDOC
#if defined(__cplusplus) && __cplusplus >= 201103L
    // tells compiler that we know what we are doing and suppresses -Wdeprecated-copy warnings
    TQCharRef(const TQCharRef&) = default;
    ~TQCharRef() = default;
#endif

    ushort unicode() const { return s.constref(p).unicode(); }
    char latin1() const { return s.constref(p).latin1(); }

    // An operator= for each TQChar cast constructors
    TQCharRef operator=(char c ) { s.ref(p)=c; return *this; }
    TQCharRef operator=(uchar c ) { s.ref(p)=c; return *this; }
    TQCharRef operator=(TQChar c ) { s.ref(p)=c; return *this; }
    TQCharRef operator=(const TQCharRef& c ) { s.ref(p)=c.unicode(); return *this; }
    TQCharRef operator=(ushort rc ) { s.ref(p)=rc; return *this; }
    TQCharRef operator=(short rc ) { s.ref(p)=rc; return *this; }
    TQCharRef operator=(uint rc ) { s.ref(p)=rc; return *this; }
    TQCharRef operator=(int rc ) { s.ref(p)=rc; return *this; }

    operator TQChar () const { return s.constref(p); }

    // each function...
    bool isNull() const { return unicode()==0; }
    bool isPrint() const { return s.constref(p).isPrint(); }
    bool isPunct() const { return s.constref(p).isPunct(); }
    bool isSpace() const { return s.constref(p).isSpace(); }
    bool isMark() const { return s.constref(p).isMark(); }
    bool isLetter() const { return s.constref(p).isLetter(); }
    bool isNumber() const { return s.constref(p).isNumber(); }
    bool isLetterOrNumber() { return s.constref(p).isLetterOrNumber(); }
    bool isDigit() const { return s.constref(p).isDigit(); }
    bool isSymbol() const { return s.constref(p).isSymbol(); }

    // Surrogate pairs support
    bool isHighSurrogate() const { return s.constref(p).isHighSurrogate(); }
    bool isLowSurrogate() const { return s.constref(p).isLowSurrogate(); }

    int digitValue() const { return s.constref(p).digitValue(); }
    TQChar lower() const { return s.constref(p).lower(); }
    TQChar upper() const { return s.constref(p).upper(); }

    TQChar::Category category() const { return s.constref(p).category(); }
    TQChar::Direction direction() const { return s.constref(p).direction(); }
    TQChar::Joining joining() const { return s.constref(p).joining(); }
    bool mirrored() const { return s.constref(p).mirrored(); }
    TQChar mirroredChar() const { return s.constref(p).mirroredChar(); }
    const TQString &decomposition() const { return s.constref(p).decomposition(); }
    TQChar::Decomposition decompositionTag() const { return s.constref(p).decompositionTag(); }
    unsigned char combiningClass() const { return s.constref(p).combiningClass(); }

    // Not the non-const ones of these.
    uchar cell() const { return s.constref(p).cell(); }
    uchar row() const { return s.constref(p).row(); }
#endif
};

inline TQCharRef TQString::at( uint i ) { return TQCharRef(this,i); }
inline TQCharRef TQString::operator[]( int i ) { return at((uint)i); }


class TQ_EXPORT TQConstString : private TQString {
public:
    TQConstString( const TQChar* unicode, uint length );
    ~TQConstString();
    const TQString& string() const { return *this; }
};


/*****************************************************************************
  TQString stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQString & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQString & );
#endif

/*****************************************************************************
  TQString inline functions
 *****************************************************************************/

// needed for TQDeepCopy
inline void TQString::detach()
{ real_detach(); }

inline TQString TQString::section( TQChar sep, int start, int end, int flags ) const
{ return section(TQString(sep), start, end, flags); }

inline TQString TQString::section( char sep, int start, int end, int flags ) const
{ return section(TQChar(sep), start, end, flags); }

#ifndef TQT_NO_CAST_ASCII
inline TQString TQString::section( const char *in_sep, int start, int end, int flags ) const
{ return section(TQString(in_sep), start, end, flags); }
#endif

inline TQString &TQString::operator=( TQChar c )
{ *this = TQString(c); return *this; }

inline TQString &TQString::operator=( char c )
{ *this = TQString(TQChar(c)); return *this; }

inline bool TQString::isNull() const
{ return unicode() == 0; }

inline bool TQString::operator!() const
{ return isNull(); }

inline uint TQString::length() const
{ return d->len; }

inline uint TQString::capacity() const
{ return d->maxl; }

inline bool TQString::isEmpty() const
{ return length() == 0; }

inline TQString TQString::copy() const
{ return TQString( *this ); }

#ifndef TQT_NO_CAST_ASCII
inline TQString &TQString::insert( uint index, const char *s )
{ return insertHelper( index, s ); }

inline TQString &TQString::insert( uint index, const TQByteArray &s )
{
    int pos = s.find( 0 );
    return insertHelper( index, s, pos==-1 ? s.size() : pos );
}
#endif

inline TQString &TQString::prepend( const TQString & s )
{ return insert(0,s); }

inline TQString &TQString::prepend( TQChar c )
{ return insert(0,c); }

inline TQString &TQString::prepend( char c )
{ return insert(0,c); }

#ifndef TQT_NO_CAST_ASCII
inline TQString &TQString::prepend( const TQByteArray & s )
{ return insert(0,s); }
#endif

#ifndef TQT_NO_CAST_ASCII
inline TQString &TQString::operator+=( const TQByteArray &s )
{
    int pos = s.find( 0 );
    return operatorPlusEqHelper( s, pos==-1 ? s.size() : pos );
}
#endif

inline TQString &TQString::append( const TQString & s )
{ return operator+=(s); }

#ifndef TQT_NO_CAST_ASCII
inline TQString &TQString::append( const TQByteArray &s )
{ return operator+=(s); }

inline TQString &TQString::append( const char * s )
{ return operator+=(s); }
#endif

inline TQString &TQString::append( TQChar c )
{ return operator+=(c); }

inline TQString &TQString::append( char c )
{ return operator+=(c); }

#ifndef TQT_NO_STL
inline TQString &TQString::operator=( const std::string& str )
{ return operator=(str.c_str()); }
#ifndef TQT_NO_CAST_ASCII
inline TQString &TQString::operator+=( const std::string& s )
{ return operator+=(s.c_str()); }
inline TQString &TQString::append( const std::string& s )
{ return operator+=(s); }
inline TQString &TQString::prepend( const std::string& s )
{ return insert(0, s); }
#endif
#endif

inline TQString &TQString::setNum( short n, int base )
{ return setNum((TQ_LLONG)n, base); }

inline TQString &TQString::setNum( ushort n, int base )
{ return setNum((TQ_ULLONG)n, base); }

inline TQString &TQString::setNum( int n, int base )
{ return setNum((TQ_LLONG)n, base); }

inline TQString &TQString::setNum( uint n, int base )
{ return setNum((TQ_ULLONG)n, base); }

inline TQString &TQString::setNum( float n, char f, int prec )
{ return setNum((double)n,f,prec); }

inline TQString TQString::arg( int a, int fieldWidth, int base ) const
{ return arg( (TQ_LLONG)a, fieldWidth, base ); }

inline TQString TQString::arg( uint a, int fieldWidth, int base ) const
{ return arg( (TQ_ULLONG)a, fieldWidth, base ); }

inline TQString TQString::arg( short a, int fieldWidth, int base ) const
{ return arg( (TQ_LLONG)a, fieldWidth, base ); }

inline TQString TQString::arg( ushort a, int fieldWidth, int base ) const
{ return arg( (TQ_ULLONG)a, fieldWidth, base ); }

inline TQString TQString::arg( const TQString& a1, const TQString& a2 ) const {
    return multiArg( 2, a1, a2 );
}

inline TQString TQString::arg( const TQString& a1, const TQString& a2,
			     const TQString& a3 ) const {
    return multiArg( 3, a1, a2, a3 );
}

inline TQString TQString::arg( const TQString& a1, const TQString& a2,
			     const TQString& a3, const TQString& a4 ) const {
    return multiArg( 4, a1, a2, a3, a4 );
}

inline int TQString::find( char c, int index, bool cs ) const
{ return find(TQChar(c), index, cs); }

inline int TQString::findRev( char c, int index, bool cs ) const
{ return findRev( TQChar(c), index, cs ); }

#ifndef TQT_NO_CAST_ASCII
inline int TQString::find( const char* str, int index ) const
{ return find(TQString::fromAscii(str), index); }

inline int TQString::findRev( const char* str, int index ) const
{ return findRev(TQString::fromAscii(str), index); }
#endif


/*****************************************************************************
  TQString non-member operators
 *****************************************************************************/

TQ_EXPORT bool operator!=( const TQString &s1, const TQString &s2 );
TQ_EXPORT bool operator<( const TQString &s1, const TQString &s2 );
TQ_EXPORT bool operator<=( const TQString &s1, const TQString &s2 );
TQ_EXPORT bool operator==( const TQString &s1, const TQString &s2 );
TQ_EXPORT bool operator>( const TQString &s1, const TQString &s2 );
TQ_EXPORT bool operator>=( const TQString &s1, const TQString &s2 );
#ifndef TQT_NO_CAST_ASCII
TQ_EXPORT bool operator!=( const TQString &s1, const char *s2 );
TQ_EXPORT bool operator<( const TQString &s1, const char *s2 );
TQ_EXPORT bool operator<=( const TQString &s1, const char *s2 );
TQ_EXPORT bool operator==( const TQString &s1, const char *s2 );
TQ_EXPORT bool operator>( const TQString &s1, const char *s2 );
TQ_EXPORT bool operator>=( const TQString &s1, const char *s2 );
TQ_EXPORT bool operator!=( const char *s1, const TQString &s2 );
TQ_EXPORT bool operator<( const char *s1, const TQString &s2 );
TQ_EXPORT bool operator<=( const char *s1, const TQString &s2 );
TQ_EXPORT bool operator==( const char *s1, const TQString &s2 );
//TQ_EXPORT bool operator>( const char *s1, const TQString &s2 ); // MSVC++
TQ_EXPORT bool operator>=( const char *s1, const TQString &s2 );
#endif

TQ_EXPORT inline const TQString operator+( const TQString &s1, const TQString &s2 )
{
    TQString tmp( s1 );
    tmp += s2;
    return tmp;
}

#ifndef TQT_NO_CAST_ASCII
TQ_EXPORT inline const TQString operator+( const TQString &s1, const char *s2 )
{
    TQString tmp( s1 );
    tmp += TQString::fromAscii(s2);
    return tmp;
}

TQ_EXPORT inline const TQString operator+( const char *s1, const TQString &s2 )
{
    TQString tmp = TQString::fromAscii( s1 );
    tmp += s2;
    return tmp;
}
#endif

TQ_EXPORT inline const TQString operator+( const TQString &s1, TQChar c2 )
{
    TQString tmp( s1 );
    tmp += c2;
    return tmp;
}

TQ_EXPORT inline const TQString operator+( const TQString &s1, char c2 )
{
    TQString tmp( s1 );
    tmp += c2;
    return tmp;
}

TQ_EXPORT inline const TQString operator+( TQChar c1, const TQString &s2 )
{
    TQString tmp;
    tmp += c1;
    tmp += s2;
    return tmp;
}

TQ_EXPORT inline const TQString operator+( char c1, const TQString &s2 )
{
    TQString tmp;
    tmp += c1;
    tmp += s2;
    return tmp;
}

#ifndef TQT_NO_STL
TQ_EXPORT inline const TQString operator+(const TQString& s1, const std::string& s2)
{
    return s1 + TQString(s2);
}

TQ_EXPORT inline const TQString operator+(const std::string& s1, const TQString& s2)
{
    TQString tmp(s2);
    return TQString(tmp.prepend(s1));
}
#endif


#if defined(Q_OS_WIN32)
extern TQ_EXPORT TQString qt_winTQString(void*);
extern TQ_EXPORT const void* qt_winTchar(const TQString& str, bool addnul);
extern TQ_EXPORT void* qt_winTchar_new(const TQString& str);
extern TQ_EXPORT TQCString qt_winTQString2MB( const TQString& s, int len=-1 );
extern TQ_EXPORT TQString qt_winMB2TQString( const char* mb, int len=-1 );
#endif

#define Q_DEFINED_QSTRING
#include "ntqwinexport.h"
#endif // TQSTRING_H
