/****************************************************************************
**
** Definition of TQTextStream class
**
** Created : 940922
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

#ifndef TQTEXTSTREAM_H
#define TQTEXTSTREAM_H

#ifndef QT_H
#include "ntqiodevice.h"
#include "ntqstring.h"
#include <stdio.h>
#endif // QT_H

#ifndef TQT_NO_TEXTSTREAM
class TQTextCodec;
class TQTextDecoder;

class TQTextStreamPrivate;

class TQ_EXPORT TQTextStream				// text stream class
{
public:
    enum Encoding { Locale, Latin1, Unicode, UnicodeNetworkOrder,
		    UnicodeReverse, RawUnicode, UnicodeUTF8 };

    void	setEncoding( Encoding );
#ifndef TQT_NO_TEXTCODEC
    void	setCodec( TQTextCodec* );
    TQTextCodec *codec();
#endif

    TQTextStream();
    TQTextStream( TQIODevice * );
    TQTextStream( TQString*, int mode );
    TQTextStream( TQString&, int mode );		// obsolete
    TQTextStream( TQByteArray, int mode );
    TQTextStream( FILE *, int mode );
    virtual ~TQTextStream();

    TQIODevice	*device() const;
    void	 setDevice( TQIODevice * );
    void	 unsetDevice();

    bool	 atEnd() const;
    bool	 eof() const;

    TQTextStream &operator>>( TQChar & );
    TQTextStream &operator>>( char & );
    TQTextStream &operator>>( signed short & );
    TQTextStream &operator>>( unsigned short & );
    TQTextStream &operator>>( signed int & );
    TQTextStream &operator>>( unsigned int & );
    TQTextStream &operator>>( signed long & );
    TQTextStream &operator>>( unsigned long & );
    TQTextStream &operator>>( float & );
    TQTextStream &operator>>( double & );
    TQTextStream &operator>>( char * );
    TQTextStream &operator>>( TQString & );
    TQTextStream &operator>>( TQCString & );

    TQTextStream &operator<<( TQChar );
    TQTextStream &operator<<( char );
    TQTextStream &operator<<( signed short );
    TQTextStream &operator<<( unsigned short );
    TQTextStream &operator<<( signed int );
    TQTextStream &operator<<( unsigned int );
    TQTextStream &operator<<( signed long );
    TQTextStream &operator<<( unsigned long );
    TQTextStream &operator<<( signed long long );
    TQTextStream &operator<<( unsigned long long );
    TQTextStream &operator<<( float );
    TQTextStream &operator<<( double );
    TQTextStream &operator<<( const char* );
    TQTextStream &operator<<( const TQString & );
    TQTextStream &operator<<( const TQCString & );
    TQTextStream &operator<<( void * );		// any pointer

    TQTextStream &readRawBytes( char *, uint len );
    TQTextStream &writeRawBytes( const char* , uint len );

    TQString	readLine();
    TQString	read();
    void	skipWhiteSpace();

    enum {
	skipws	  = 0x0001,			// skip whitespace on input
	left	  = 0x0002,			// left-adjust output
	right	  = 0x0004,			// right-adjust output
	internal  = 0x0008,			// pad after sign
	bin	  = 0x0010,			// binary format integer
	oct	  = 0x0020,			// octal format integer
	dec	  = 0x0040,			// decimal format integer
	hex	  = 0x0080,			// hex format integer
	showbase  = 0x0100,			// show base indicator
	showpoint = 0x0200,			// force decimal point (float)
	uppercase = 0x0400,			// upper-case hex output
	showpos	  = 0x0800,			// add '+' to positive integers
	scientific= 0x1000,			// scientific float output
	fixed	  = 0x2000			// fixed float output
    };

    static const int basefield;			// bin | oct | dec | hex
    static const int adjustfield;		// left | right | internal
    static const int floatfield;		// scientific | fixed

    int	  flags() const;
    int	  flags( int f );
    int	  setf( int bits );
    int	  setf( int bits, int mask );
    int	  unsetf( int bits );

    void  reset();

    int	  width()	const;
    int	  width( int );
    int	  fill()	const;
    int	  fill( int );
    int	  precision()	const;
    int	  precision( int );

private:
    long	input_int();
    void	init();
    TQTextStream &output_int( int, unsigned long long, bool );
 
    TQIODevice	*dev;

    int		fflags;
    int		fwidth;
    int		fillchar;
    int		fprec;
    bool	doUnicodeHeader;
    bool	owndev;
    TQTextCodec 	*mapper;
    TQTextStreamPrivate * d;
    TQChar	unused1; // ### remove in TQt 4.0
    bool	latin1;
    bool 	internalOrder;
    bool	networkOrder;
    void	*unused2; // ### remove in TQt 4.0

    TQChar	eat_ws();
    uint 	ts_getline( TQChar* );
    void	ts_ungetc( TQChar );
    TQChar	ts_getc();
    uint	ts_getbuf( TQChar*, uint );
    void	ts_putc(int);
    void	ts_putc(TQChar);
    bool	ts_isspace(TQChar);
    bool	ts_isdigit(TQChar);
    ulong	input_bin();
    ulong	input_oct();
    ulong	input_dec();
    ulong	input_hex();
    double	input_double();
    TQTextStream &writeBlock( const char* p, uint len );
    TQTextStream &writeBlock( const TQChar* p, uint len );

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQTextStream( const TQTextStream & );
    TQTextStream &operator=( const TQTextStream & );
#endif
};

typedef TQTextStream TQTS;

class TQ_EXPORT TQTextIStream : public TQTextStream {
public:
    TQTextIStream( const TQString* s ) :
	TQTextStream((TQString*)s,IO_ReadOnly) { }
    TQTextIStream( TQByteArray ba ) :
	TQTextStream(ba,IO_ReadOnly) { }
    TQTextIStream( FILE *f ) :
	TQTextStream(f,IO_ReadOnly) { }

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQTextIStream( const TQTextIStream & );
    TQTextIStream &operator=( const TQTextIStream & );
#endif
};

class TQ_EXPORT TQTextOStream : public TQTextStream {
public:
    TQTextOStream( TQString* s ) :
	TQTextStream(s,IO_WriteOnly) { }
    TQTextOStream( TQByteArray ba ) :
	TQTextStream(ba,IO_WriteOnly) { }
    TQTextOStream( FILE *f ) :
	TQTextStream(f,IO_WriteOnly) { }

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQTextOStream( const TQTextOStream & );
    TQTextOStream &operator=( const TQTextOStream & );
#endif
};

/*****************************************************************************
  TQTextStream inline functions
 *****************************************************************************/

inline TQIODevice *TQTextStream::device() const
{ return dev; }

inline bool TQTextStream::atEnd() const
{ return dev ? dev->atEnd() : false; }

inline bool TQTextStream::eof() const
{ return atEnd(); }

inline int TQTextStream::flags() const
{ return fflags; }

inline int TQTextStream::flags( int f )
{ int oldf = fflags;  fflags = f;  return oldf; }

inline int TQTextStream::setf( int bits )
{ int oldf = fflags;  fflags |= bits;  return oldf; }

inline int TQTextStream::setf( int bits, int mask )
{ int oldf = fflags;  fflags = (fflags & ~mask) | (bits & mask); return oldf; }

inline int TQTextStream::unsetf( int bits )
{ int oldf = fflags;  fflags &= ~bits;	return oldf; }

inline int TQTextStream::width() const
{ return fwidth; }

inline int TQTextStream::width( int w )
{ int oldw = fwidth;  fwidth = w;  return oldw;	 }

inline int TQTextStream::fill() const
{ return fillchar; }

inline int TQTextStream::fill( int f )
{ int oldc = fillchar;	fillchar = f;  return oldc;  }

inline int TQTextStream::precision() const
{ return fprec; }

inline int TQTextStream::precision( int p )
{ int oldp = fprec;  fprec = p;	 return oldp;  }

/*!
  Returns one character from the stream, or EOF.
*/
inline TQChar TQTextStream::ts_getc()
{ TQChar r; return ( ts_getbuf( &r,1 ) == 1 ? r : TQChar((ushort)0xffff) ); }

/*****************************************************************************
  TQTextStream manipulators
 *****************************************************************************/

typedef TQTextStream & (*TQTSFUNC)(TQTextStream &);// manipulator function
typedef int (TQTextStream::*TQTSMFI)(int);	// manipulator w/int argument

class TQ_EXPORT TQTSManip {			// text stream manipulator
public:
    TQTSManip( TQTSMFI m, int a ) { mf=m; arg=a; }
    void exec( TQTextStream &s ) { (s.*mf)(arg); }
private:
    TQTSMFI mf;					// TQTextStream member function
    int	   arg;					// member function argument
};

TQ_EXPORT inline TQTextStream &operator>>( TQTextStream &s, TQTSFUNC f )
{ return (*f)( s ); }

TQ_EXPORT inline TQTextStream &operator<<( TQTextStream &s, TQTSFUNC f )
{ return (*f)( s ); }

TQ_EXPORT inline TQTextStream &operator<<( TQTextStream &s, TQTSManip m )
{ m.exec(s); return s; }

TQ_EXPORT TQTextStream &bin( TQTextStream &s );	// set bin notation
TQ_EXPORT TQTextStream &oct( TQTextStream &s );	// set oct notation
TQ_EXPORT TQTextStream &dec( TQTextStream &s );	// set dec notation
TQ_EXPORT TQTextStream &hex( TQTextStream &s );	// set hex notation
TQ_EXPORT TQTextStream &endl( TQTextStream &s );	// insert EOL ('\n')
TQ_EXPORT TQTextStream &flush( TQTextStream &s );	// flush output
TQ_EXPORT TQTextStream &ws( TQTextStream &s );	// eat whitespace on input
TQ_EXPORT TQTextStream &reset( TQTextStream &s );	// set default flags

TQ_EXPORT inline TQTSManip qSetW( int w )
{
    TQTSMFI func = &TQTextStream::width;
    return TQTSManip(func,w);
}

TQ_EXPORT inline TQTSManip qSetFill( int f )
{
    TQTSMFI func = &TQTextStream::fill;
    return TQTSManip(func,f);
}

TQ_EXPORT inline TQTSManip qSetPrecision( int p )
{
    TQTSMFI func = &TQTextStream::precision;
    return TQTSManip(func,p);
}

#endif // TQT_NO_TEXTSTREAM
#endif // TQTEXTSTREAM_H
