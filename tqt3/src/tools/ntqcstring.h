/****************************************************************************
**
** Definition of the extended char array operations,
** and TQByteArray and TQCString classes
**
** Created : 920609
**
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

#ifndef TQCSTRING_H
#define TQCSTRING_H

#ifndef QT_H
#include "ntqmemarray.h"
#endif // QT_H

#include <string.h>


/*****************************************************************************
  Safe and portable C string functions; extensions to standard string.h
 *****************************************************************************/

TQ_EXPORT void *tqmemmove( void *dst, const void *src, uint len );

TQ_EXPORT char *tqstrdup( const char * );

TQ_EXPORT inline uint tqstrlen( const char *str )
{ return str ? (uint)strlen(str) : 0u; }

TQ_EXPORT inline char *qstrcpy( char *dst, const char *src )
{ return src ? strcpy(dst, src) : 0; }

TQ_EXPORT char *tqstrncpy( char *dst, const char *src, uint len );

TQ_EXPORT inline int qstrcmp( const char *str1, const char *str2 )
{
    return ( str1 && str2 ) ? strcmp( str1, str2 )
			    : ( str1 ? 1 : ( str2 ? -1 : 0 ) );
}

TQ_EXPORT inline int tqstrncmp( const char *str1, const char *str2, uint len )
{
    return ( str1 && str2 ) ? strncmp( str1, str2, len )
			    : ( str1 ? 1 : ( str2 ? -1 : 0 ) );
}

TQ_EXPORT int tqstricmp( const char *, const char * );

TQ_EXPORT int tqstrnicmp( const char *, const char *, uint len );


// tqChecksum: Internet checksum

TQ_EXPORT TQ_UINT16 tqChecksum( const char *s, uint len );

/*****************************************************************************
  TQByteArray class
 *****************************************************************************/

#if defined(Q_QDOC)
/*
  We want qdoc to document TQByteArray as a real class that inherits
  TQMemArray<char> and that is inherited by TQBitArray.
*/
class TQByteArray : public TQMemArray<char>
{
public:
    TQByteArray();
    TQByteArray( int size );
};
#else
typedef TQMemArray<char> TQByteArray;
#endif

#ifndef TQT_NO_COMPRESS
TQ_EXPORT TQByteArray tqCompress( const uchar* data, int nbytes );
TQ_EXPORT TQByteArray tqUncompress( const uchar* data, int nbytes );
TQ_EXPORT inline TQByteArray tqCompress( const TQByteArray& data)
{ return tqCompress( (const uchar*)data.data(), data.size() ); }
TQ_EXPORT inline TQByteArray tqUncompress( const TQByteArray& data )
{ return tqUncompress( (const uchar*)data.data(), data.size() ); }
#endif

/*****************************************************************************
  TQByteArray stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQByteArray & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQByteArray & );
#endif

/*****************************************************************************
  TQCString class
 *****************************************************************************/

class TQRegExp;

class TQ_EXPORT TQCString : public TQByteArray	// C string class
{
public:
    TQCString() {}				// make null string
    TQCString( int size );			// allocate size incl. \0
    TQCString( const TQCString &s ) : TQByteArray( s ) {}
    TQCString( const char *str );		// deep copy
    TQCString( const char *str, uint maxlen );	// deep copy, max length
    ~TQCString();

    TQCString    &operator=( const TQCString &s );// shallow copy
    TQCString    &operator=( const char *str );	// deep copy

    bool	isNull()	const;
    bool	isEmpty()	const;
    uint	length()	const;
    bool	resize( uint newlen );
    bool	truncate( uint pos );
    bool	fill( char c, int len = -1 );

    TQCString	copy()	const;

    TQCString    &sprintf( const char *format, ... )
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 2, 3)))
#endif
        ;

    int		find( char c, int index=0, bool cs=true ) const;
    int		find( const char *str, int index=0, bool cs=true ) const;
#ifndef TQT_NO_REGEXP
    int		find( const TQRegExp &, int index=0 ) const;
#endif
    int		findRev( char c, int index=-1, bool cs=true) const;
    int		findRev( const char *str, int index=-1, bool cs=true) const;
#ifndef TQT_NO_REGEXP_CAPTURE
    int		findRev( const TQRegExp &, int index=-1 ) const;
#endif
    int		contains( char c, bool cs=true ) const;
    int		contains( const char *str, bool cs=true ) const;
#ifndef TQT_NO_REGEXP
    int		contains( const TQRegExp & ) const;
#endif
    TQCString	left( uint len )  const;
    TQCString	right( uint len ) const;
    TQCString	mid( uint index, uint len=0xffffffff) const;

    TQCString	leftJustify( uint width, char fill=' ', bool trunc=false)const;
    TQCString	rightJustify( uint width, char fill=' ',bool trunc=false)const;

    TQCString	lower() const;
    TQCString	upper() const;

    TQCString	stripWhiteSpace()	const;
    TQCString	simplifyWhiteSpace()	const;

    TQCString    &insert( uint index, const char * );
    TQCString    &insert( uint index, char );
    TQCString    &append( const char * );
    TQCString    &prepend( const char * );
    TQCString    &remove( uint index, uint len );
    TQCString    &replace( uint index, uint len, const char * );
#ifndef TQT_NO_REGEXP
    TQCString    &replace( const TQRegExp &, const char * );
#endif
    TQCString    &replace( char c, const char *after );
    TQCString    &replace( const char *, const char * );
    TQCString    &replace( char, char );

    short	toShort( bool *ok=0 )	const;
    ushort	toUShort( bool *ok=0 )	const;
    int		toInt( bool *ok=0 )	const;
    uint	toUInt( bool *ok=0 )	const;
    long	toLong( bool *ok=0 )	const;
    ulong	toULong( bool *ok=0 )	const;
    float	toFloat( bool *ok=0 )	const;
    double	toDouble( bool *ok=0 )	const;

    TQCString    &setStr( const char *s );
    TQCString    &setNum( short );
    TQCString    &setNum( ushort );
    TQCString    &setNum( int );
    TQCString    &setNum( uint );
    TQCString    &setNum( long );
    TQCString    &setNum( ulong );
    TQCString    &setNum( float, char f='g', int prec=6 );
    TQCString    &setNum( double, char f='g', int prec=6 );

    bool	setExpand( uint index, char c );

		operator const char *() const;
    TQCString    &operator+=( const char *str );
    TQCString    &operator+=( char c );
private:
    int	find( const char *str, int index, bool cs, uint l ) const;
};


/*****************************************************************************
  TQCString stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQCString & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQCString & );
#endif

/*****************************************************************************
  TQCString inline functions
 *****************************************************************************/

inline TQCString &TQCString::operator=( const TQCString &s )
{ return (TQCString&)assign( s ); }

inline TQCString &TQCString::operator=( const char *str )
{ return (TQCString&)duplicate( str, tqstrlen(str)+1 ); }

inline bool TQCString::isNull() const
{ return data() == 0; }

inline bool TQCString::isEmpty() const
{ return data() == 0 || *data() == '\0'; }

inline uint TQCString::length() const
{ return tqstrlen( data() ); }

inline bool TQCString::truncate( uint pos )
{ return resize(pos+1); }

inline TQCString TQCString::copy() const
{ return TQCString( data() ); }

inline TQCString &TQCString::prepend( const char *s )
{ return insert(0,s); }

inline TQCString &TQCString::append( const char *s )
{ return operator+=(s); }

inline TQCString &TQCString::setNum( short n )
{ return setNum((long)n); }

inline TQCString &TQCString::setNum( ushort n )
{ return setNum((ulong)n); }

inline TQCString &TQCString::setNum( int n )
{ return setNum((long)n); }

inline TQCString &TQCString::setNum( uint n )
{ return setNum((ulong)n); }

inline TQCString &TQCString::setNum( float n, char f, int prec )
{ return setNum((double)n,f,prec); }

inline TQCString::operator const char *() const
{ return (const char *)data(); }


/*****************************************************************************
  TQCString non-member operators
 *****************************************************************************/

TQ_EXPORT inline bool operator==( const TQCString &s1, const TQCString &s2 )
{ return qstrcmp( s1.data(), s2.data() ) == 0; }

TQ_EXPORT inline bool operator==( const TQCString &s1, const char *s2 )
{ return qstrcmp( s1.data(), s2 ) == 0; }

TQ_EXPORT inline bool operator==( const char *s1, const TQCString &s2 )
{ return qstrcmp( s1, s2.data() ) == 0; }

TQ_EXPORT inline bool operator!=( const TQCString &s1, const TQCString &s2 )
{ return qstrcmp( s1.data(), s2.data() ) != 0; }

TQ_EXPORT inline bool operator!=( const TQCString &s1, const char *s2 )
{ return qstrcmp( s1.data(), s2 ) != 0; }

TQ_EXPORT inline bool operator!=( const char *s1, const TQCString &s2 )
{ return qstrcmp( s1, s2.data() ) != 0; }

TQ_EXPORT inline bool operator<( const TQCString &s1, const TQCString& s2 )
{ return qstrcmp( s1.data(), s2.data() ) < 0; }

TQ_EXPORT inline bool operator<( const TQCString &s1, const char *s2 )
{ return qstrcmp( s1.data(), s2 ) < 0; }

TQ_EXPORT inline bool operator<( const char *s1, const TQCString &s2 )
{ return qstrcmp( s1, s2.data() ) < 0; }

TQ_EXPORT inline bool operator<=( const TQCString &s1, const TQCString &s2 )
{ return qstrcmp( s1.data(), s2.data() ) <= 0; }

TQ_EXPORT inline bool operator<=( const TQCString &s1, const char *s2 )
{ return qstrcmp( s1.data(), s2 ) <= 0; }

TQ_EXPORT inline bool operator<=( const char *s1, const TQCString &s2 )
{ return qstrcmp( s1, s2.data() ) <= 0; }

TQ_EXPORT inline bool operator>( const TQCString &s1, const TQCString &s2 )
{ return qstrcmp( s1.data(), s2.data() ) > 0; }

TQ_EXPORT inline bool operator>( const TQCString &s1, const char *s2 )
{ return qstrcmp( s1.data(), s2 ) > 0; }

TQ_EXPORT inline bool operator>( const char *s1, const TQCString &s2 )
{ return qstrcmp( s1, s2.data() ) > 0; }

TQ_EXPORT inline bool operator>=( const TQCString &s1, const TQCString& s2 )
{ return qstrcmp( s1.data(), s2.data() ) >= 0; }

TQ_EXPORT inline bool operator>=( const TQCString &s1, const char *s2 )
{ return qstrcmp( s1.data(), s2 ) >= 0; }

TQ_EXPORT inline bool operator>=( const char *s1, const TQCString &s2 )
{ return qstrcmp( s1, s2.data() ) >= 0; }

TQ_EXPORT inline const TQCString operator+( const TQCString &s1,
					  const TQCString &s2 )
{
    TQCString tmp( s1.data() );
    tmp += s2;
    return tmp;
}

TQ_EXPORT inline const TQCString operator+( const TQCString &s1, const char *s2 )
{
    TQCString tmp( s1.data() );
    tmp += s2;
    return tmp;
}

TQ_EXPORT inline const TQCString operator+( const char *s1, const TQCString &s2 )
{
    TQCString tmp( s1 );
    tmp += s2;
    return tmp;
}

TQ_EXPORT inline const TQCString operator+( const TQCString &s1, char c2 )
{
    TQCString tmp( s1.data() );
    tmp += c2;
    return tmp;
}

TQ_EXPORT inline const TQCString operator+( char c1, const TQCString &s2 )
{
    TQCString tmp;
    tmp += c1;
    tmp += s2;
    return tmp;
}
#include "ntqwinexport.h"
#endif // TQCSTRING_H
