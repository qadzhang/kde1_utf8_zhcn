/****************************************************************************
**
** Implementation of TQTextStream class
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

#include "ntqtextstream.h"

#ifndef TQT_NO_TEXTSTREAM
#include "ntqtextcodec.h"
#include "ntqregexp.h"
#include "ntqbuffer.h"
#include "ntqfile.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#ifndef Q_OS_TEMP
#include <locale.h>
#endif

#if defined(Q_OS_WIN32)
#include "qt_windows.h"
#endif

/*!
    \class TQTextStream ntqtextstream.h
    \reentrant
    \brief The TQTextStream class provides basic functions for reading
    and writing text using a TQIODevice.

    \ingroup io
    \ingroup text
    \mainclass

    The text stream class has a functional interface that is very
    similar to that of the standard C++ iostream class.

    TQt provides several global functions similar to the ones in iostream:
    \table
    \header \i Function \i Meaning
    \row \i bin \i sets the TQTextStream to read/write binary numbers
    \row \i oct \i sets the TQTextStream to read/write octal numbers
    \row \i dec \i sets the TQTextStream to read/write decimal numbers
    \row \i hex \i sets the TQTextStream to read/write hexadecimal numbers
    \row \i endl \i forces a line break
    \row \i flush \i forces the TQIODevice to flush any buffered data
    \row \i ws \i eats any available whitespace (on input)
    \row \i reset \i resets the TQTextStream to its default mode (see reset())
    \row \i qSetW(int) \i sets the \link width() field width \endlink
    to the given argument
    \row \i qSetFill(int) \i sets the \link fill() fill character
    \endlink to the given argument
    \row \i qSetPrecision(int) \i sets the \link precision() precision
    \endlink to the given argument
    \endtable

    \warning By default TQTextStream will automatically detect whether
    integers in the stream are in decimal, octal, hexadecimal or
    binary format when reading from the stream. In particular, a
    leading '0' signifies octal, i.e. the sequence "0100" will be
    interpreted as 64.

    The TQTextStream class reads and writes text; it is not appropriate
    for dealing with binary data (but TQDataStream is).

    By default, output of Unicode text (i.e. TQString) is done using
    the local 8-bit encoding. This can be changed using the
    setEncoding() method. For input, the TQTextStream will auto-detect
    standard Unicode "byte order marked" text files; otherwise the
    local 8-bit encoding is used.

    The TQIODevice is set in the constructor, or later using
    setDevice(). If the end of the input is reached atEnd() returns
    true. Data can be read into variables of the appropriate type
    using the operator>>() overloads, or read in its entirety into a
    single string using read(), or read a line at a time using
    readLine(). Whitespace can be skipped over using skipWhiteSpace().
    You can set flags for the stream using flags() or setf(). The
    stream also supports width(), precision() and fill(); use reset()
    to reset the defaults.

    \sa TQDataStream
*/

/*!
    \enum TQTextStream::Encoding

    \value Locale
    \value Latin1
    \value Unicode
    \value UnicodeNetworkOrder
    \value UnicodeReverse
    \value RawUnicode
    \value UnicodeUTF8

    See setEncoding() for an explanation of the encodings.
*/

/*
  \class TQTSManip

  \brief The TQTSManip class is an internal helper class for the
  TQTextStream.

  It is generally a very bad idea to use this class directly in
  application programs.

  \internal

  This class makes it possible to give the TQTextStream function objects
  with arguments, like this:
  \code
    TQTextStream cout( stdout, IO_WriteOnly );
    cout << setprecision( 8 );		// TQTSManip used here!
    cout << 3.14159265358979323846;
  \endcode

  The setprecision() function returns a TQTSManip object.
  The TQTSManip object contains a pointer to a member function in
  TQTextStream and an integer argument.
  When serializing a TQTSManip into a TQTextStream, the function
  is executed with the argument.
*/

/*! \fn TQTSManip::TQTSManip( TQTSMFI m, int a )

  Constructs a TQTSManip object which will call \a m (a member function
  in TQTextStream which accepts a single int) with argument \a a when
  TQTSManip::exec() is called. Used internally in e.g. endl:

  \code
    s << "some text" << endl << "more text";
  \endcode
*/

/*! \fn void TQTSManip::exec( TQTextStream& s )

  Calls the member function specified in the constructor, for object
  \a s. Used internally in e.g. endl:

  \code
    s << "some text" << endl << "more text";
  \endcode
*/


/*****************************************************************************
  TQTextStream member functions
 *****************************************************************************/

#if defined(QT_CHECK_STATE)
#undef  CHECK_STREAM_PRECOND
#define CHECK_STREAM_PRECOND  if ( !dev ) {				\
				tqWarning( "TQTextStream: No device" );	\
				return *this; }
#else
#define CHECK_STREAM_PRECOND
#endif


#define I_SHORT		0x0010
#define I_INT		0x0020
#define I_LONG		0x0030
#define I_LONGLONG	0x0040
#define I_TYPE_MASK	0x00f0

#define I_BASE_2	TQTS::bin
#define I_BASE_8	TQTS::oct
#define I_BASE_10	TQTS::dec
#define I_BASE_16	TQTS::hex
#define I_BASE_MASK	(TQTS::bin | TQTS::oct | TQTS::dec | TQTS::hex)

#define I_SIGNED	0x0100
#define I_UNSIGNED	0x0200
#define I_SIGN_MASK	0x0f00


static const TQChar TQEOF = TQChar((ushort)0xffff); //guaranteed not to be a character.
static const uint getline_buf_size = 256; // bufsize used by ts_getline()

const int TQTextStream::basefield   = I_BASE_MASK;
const int TQTextStream::adjustfield = ( TQTextStream::left |
				       TQTextStream::right |
				       TQTextStream::internal );
const int TQTextStream::floatfield  = ( TQTextStream::scientific |
				       TQTextStream::fixed );


class TQTextStreamPrivate {
public:
#ifndef TQT_NO_TEXTCODEC
    TQTextStreamPrivate()
	: decoder( 0 ), encoder( 0 ), sourceType( NotSet ) { }
    ~TQTextStreamPrivate() {
	delete decoder;
	delete encoder;
    }
    TQTextDecoder *decoder;
    TQTextEncoder *encoder;
#else
    TQTextStreamPrivate() : sourceType( NotSet ) { }
    ~TQTextStreamPrivate() { }
#endif
    TQString ungetcBuf;

    enum SourceType { NotSet, IODevice, String, ByteArray, File };
    SourceType sourceType;
};


// skips whitespace and returns the first non-whitespace character
TQChar TQTextStream::eat_ws()
{
    TQChar c;
    do { c = ts_getc(); } while ( c != TQEOF && ts_isspace(c) );
    return c;
}

void TQTextStream::init()
{
    // ### ungetcBuf = TQEOF;
    dev = 0;
    owndev = false;
    mapper = 0;
    d = new TQTextStreamPrivate;
    doUnicodeHeader = true; // autodetect
    latin1 = true; // should use locale?
    internalOrder = TQChar::networkOrdered();
    networkOrder = true;
}

/*!
    Constructs a data stream that has no IO device.
*/

TQTextStream::TQTextStream()
{
    init();
    setEncoding( Locale );
    reset();
    d->sourceType = TQTextStreamPrivate::NotSet;
}

/*!
    Constructs a text stream that uses the IO device \a iod.
*/

TQTextStream::TQTextStream( TQIODevice *iod )
{
    init();
    setEncoding( Locale );
    dev = iod;
    reset();
    d->sourceType = TQTextStreamPrivate::IODevice;
}

// TODO: use special-case handling of this case in TQTextStream, and
//	 simplify this class to only deal with TQChar or TQString data.
class TQStringBuffer : public TQIODevice {
public:
    TQStringBuffer( TQString* str );
    ~TQStringBuffer();
    bool  open( int m );
    void  close();
    void  flush();
    Offset  size() const;
    Offset  at()   const;
    bool  at( Offset pos );
    TQ_LONG readBlock( char *p, TQ_ULONG len );
    TQ_LONG writeBlock( const char *p, TQ_ULONG len );
    int   getch();
    int   putch( int ch );
    int   ungetch( int ch );
protected:
    TQString* s;

private:
    TQStringBuffer( const TQStringBuffer & );
    TQStringBuffer &operator=( const TQStringBuffer & );
};


TQStringBuffer::TQStringBuffer( TQString* str )
{
    s = str;
}

TQStringBuffer::~TQStringBuffer()
{
}


bool TQStringBuffer::open( int m )
{
    if ( !s ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQStringBuffer::open: No string" );
#endif
	return false;
    }
    if ( isOpen() ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQStringBuffer::open: Buffer already open" );
#endif
	return false;
    }
    setMode( m );
    if ( m & IO_Truncate )
	s->truncate( 0 );

    if ( m & IO_Append ) {
	ioIndex = s->length()*sizeof(TQChar);
    } else {
	ioIndex = 0;
    }
    setState( IO_Open );
    resetStatus();
    return true;
}

void TQStringBuffer::close()
{
    if ( isOpen() ) {
	setFlags( IO_Direct );
	ioIndex = 0;
    }
}

void TQStringBuffer::flush()
{
}

TQIODevice::Offset TQStringBuffer::size() const
{
    return s ? s->length()*sizeof(TQChar) : 0;
}

TQIODevice::Offset  TQStringBuffer::at()   const
{
    return ioIndex;
}

bool TQStringBuffer::at( Offset pos )
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {
	tqWarning( "TQStringBuffer::at: Buffer is not open" );
	return false;
    }
#endif
    if ( pos >= s->length()*2 ) {
#if defined(QT_CHECK_RANGE)
#if defined(QT_ABI_QT4)
	tqWarning( "TQStringBuffer::at: Index %lld out of range", pos );
#else
	tqWarning( "TQStringBuffer::at: Index %lu out of range", pos );
#endif
#endif
	return false;
    }
    ioIndex = pos;
    return true;
}


TQ_LONG TQStringBuffer::readBlock( char *p, TQ_ULONG len )
{
#if defined(QT_CHECK_STATE)
    TQ_CHECK_PTR( p );
    if ( !isOpen() ) {
	tqWarning( "TQStringBuffer::readBlock: Buffer not open" );
	return -1;
    }
    if ( !isReadable() ) {
	tqWarning( "TQStringBuffer::readBlock: Read operation not permitted" );
	return -1;
    }
#endif
    if ( ioIndex + len > s->length()*sizeof(TQChar) ) {
	// overflow
	if ( (uint)ioIndex >= s->length()*sizeof(TQChar) ) {
	    setStatus( IO_ReadError );
	    return -1;
	} else {
	    len = s->length()*2 - (uint)ioIndex;
	}
    }
    memcpy( p, ((const char*)(s->unicode()))+ioIndex, len );
    ioIndex += len;
    return len;
}

TQ_LONG TQStringBuffer::writeBlock( const char *p, TQ_ULONG len )
{
#if defined(QT_CHECK_NULL)
    if ( p == 0 && len != 0 )
	tqWarning( "TQStringBuffer::writeBlock: Null pointer error" );
#endif
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {
	tqWarning( "TQStringBuffer::writeBlock: Buffer not open" );
	return -1;
    }
    if ( !isWritable() ) {
	tqWarning( "TQStringBuffer::writeBlock: Write operation not permitted" );
	return -1;
    }
    if ( ioIndex&1 ) {
	tqWarning( "TQStringBuffer::writeBlock: non-even index - non Unicode" );
	return -1;
    }
    if ( len&1 ) {
	tqWarning( "TQStringBuffer::writeBlock: non-even length - non Unicode" );
	return -1;
    }
#endif
    s->replace(ioIndex/2, len/2, (TQChar*)p, len/2);
    ioIndex += len;
    return len;
}

int TQStringBuffer::getch()
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {
	tqWarning( "TQStringBuffer::getch: Buffer not open" );
	return -1;
    }
    if ( !isReadable() ) {
	tqWarning( "TQStringBuffer::getch: Read operation not permitted" );
	return -1;
    }
#endif
    if ( (uint)ioIndex >= s->length()*2 ) {           // overflow
	setStatus( IO_ReadError );
	return -1;
    }
    return (int)((const uchar *)s->unicode())[ioIndex++];
}

int TQStringBuffer::putch( int ch )
{
    char c = ch;
    if ( writeBlock(&c,1) < 0 )
	return -1;
    else
	return ch;
}

int TQStringBuffer::ungetch( int ch )
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {
	tqWarning( "TQStringBuffer::ungetch: Buffer not open" );
	return -1;
    }
    if ( !isReadable() ) {
	tqWarning( "TQStringBuffer::ungetch: Read operation not permitted" );
	return -1;
    }
#endif
    if ( ch != -1 ) { // something to do with eof
	if ( ioIndex )
	    ioIndex--;
	else
	    ch = -1;
    }
    return ch;
}


/*!
    Constructs a text stream that operates on the Unicode TQString, \a
    str, through an internal device. The \a filemode argument is
    passed to the device's open() function; see \l{TQIODevice::mode()}.

    If you set an encoding or codec with setEncoding() or setCodec(),
    this setting is ignored for text streams that operate on TQString.

    Example:
    \code
    TQString str;
    TQTextStream ts( &str, IO_WriteOnly );
    ts << "pi = " << 3.14; // str == "pi = 3.14"
    \endcode

    Writing data to the text stream will modify the contents of the
    string. The string will be expanded when data is written beyond
    the end of the string. Note that the string will not be truncated:
    \code
    TQString str = "pi = 3.14";
    TQTextStream ts( &str, IO_WriteOnly );
    ts <<  "2+2 = " << 2+2; // str == "2+2 = 414"
    \endcode

    Note that because TQString is Unicode, you should not use
    readRawBytes() or writeRawBytes() on such a stream.
*/

TQTextStream::TQTextStream( TQString* str, int filemode )
{
    // TODO: optimize for this case as it becomes more common
    //        (see TQStringBuffer above)
    init();
    dev = new TQStringBuffer( str );
    ((TQStringBuffer *)dev)->open( filemode );
    owndev = true;
    setEncoding(RawUnicode);
    reset();
    d->sourceType = TQTextStreamPrivate::String;
}

/*! \obsolete

  This constructor is equivalent to the constructor taking a TQString*
  parameter.
*/

TQTextStream::TQTextStream( TQString& str, int filemode )
{
    init();
    dev = new TQStringBuffer( &str );
    ((TQStringBuffer *)dev)->open( filemode );
    owndev = true;
    setEncoding(RawUnicode);
    reset();
    d->sourceType = TQTextStreamPrivate::String;
}

/*!
    Constructs a text stream that operates on the byte array, \a a,
    through an internal TQBuffer device. The \a mode argument is passed
    to the device's open() function; see \l{TQIODevice::mode()}.

    Example:
    \code
    TQByteArray array;
    TQTextStream ts( array, IO_WriteOnly );
    ts << "pi = " << 3.14 << '\0'; // array == "pi = 3.14"
    \endcode

    Writing data to the text stream will modify the contents of the
    array. The array will be expanded when data is written beyond the
    end of the string.

    Same example, using a TQBuffer:
    \code
    TQByteArray array;
    TQBuffer buf( array );
    buf.open( IO_WriteOnly );
    TQTextStream ts( &buf );
    ts << "pi = " << 3.14 << '\0'; // array == "pi = 3.14"
    buf.close();
    \endcode
*/

TQTextStream::TQTextStream( TQByteArray a, int mode )
{
    init();
    dev = new TQBuffer( a );
    ((TQBuffer *)dev)->open( mode );
    owndev = true;
    setEncoding( Latin1 ); //### Locale???
    reset();
    d->sourceType = TQTextStreamPrivate::ByteArray;
}

/*!
    Constructs a text stream that operates on an existing file handle
    \a fh through an internal TQFile device. The \a mode argument is
    passed to the device's open() function; see \l{TQIODevice::mode()}.

    Note that if you create a TQTextStream \c cout or another name that
    is also used for another variable of a different type, some
    linkers may confuse the two variables, which will often cause
    crashes.
*/

TQTextStream::TQTextStream( FILE *fh, int mode )
{
    init();
    setEncoding( Locale ); //###
    dev = new TQFile;
    ((TQFile *)dev)->open( mode, fh );
    owndev = true;
    reset();
    d->sourceType = TQTextStreamPrivate::File;
}

/*!
    Destroys the text stream.

    The destructor does not affect the current IO device.
*/

TQTextStream::~TQTextStream()
{
    if ( owndev )
	delete dev;
    delete d;
}

/*!
    Positions the read pointer at the first non-whitespace character.
*/
void TQTextStream::skipWhiteSpace()
{
    ts_ungetc( eat_ws() );
}


/*!
    Tries to read \a len characters from the stream and stores them in
    \a buf. Returns the number of characters really read.

    \warning There will no TQEOF appended if the read reaches the end
    of the file. EOF is reached when the return value does not equal
    \a len.
*/
uint TQTextStream::ts_getbuf( TQChar* buf, uint len )
{
    if ( len < 1 )
	return 0;

    uint rnum = 0;   // the number of TQChars really read

    if ( d && d->ungetcBuf.length() ) {
	while ( rnum < len && rnum < d->ungetcBuf.length() ) {
	    *buf = d->ungetcBuf.constref( rnum );
	    buf++;
	    rnum++;
	}
	d->ungetcBuf = d->ungetcBuf.mid( rnum );
	if ( rnum >= len )
	    return rnum;
    }

    // we use dev->ungetch() for one of the bytes of the unicode
    // byte-order mark, but a local unget hack for the other byte:
    int ungetHack = EOF;

    if ( doUnicodeHeader ) {
	doUnicodeHeader = false; // only at the top
	int c1 = dev->getch();
	if ( c1 == EOF )
	    return rnum;
	int c2 = dev->getch();
	if ( c1 == 0xfe && c2 == 0xff ) {
	    mapper = 0;
	    latin1 = false;
	    internalOrder = TQChar::networkOrdered();
	    networkOrder = true;
	} else if ( c1 == 0xff && c2 == 0xfe ) {
	    mapper = 0;
	    latin1 = false;
	    internalOrder = !TQChar::networkOrdered();
	    networkOrder = false;
	} else {
	    if ( c2 != EOF ) {
	 	dev->ungetch( c2 );
		ungetHack = c1;
	    } else {
		/*
		  A small bug might hide here. If only the first byte
		  of a file has made it so far, and that first byte
		  is half of the byte-order mark, then the utfness
		  will not be detected.
		*/
	 	dev->ungetch( c1 );
	    }
	}
    }

#ifndef TQT_NO_TEXTCODEC
    if ( mapper ) {
	bool shortRead = false;
	if ( !d->decoder )
	    d->decoder = mapper->makeDecoder();
	while( rnum < len ) {
	    TQString s;
	    bool readBlock = !( len == 1+rnum );
	    for (;;) {
		// for efficiency: normally read a whole block
		if ( readBlock ) {
		    // guess buffersize; this may be wrong (too small or too
		    // big). But we can handle this (either iterate reading
		    // or use ungetcBuf).
		    // Note that this might cause problems for codecs where
		    // one byte can result in >1 Unicode Characters if bytes
		    // are written to the stream in the meantime (loss of
		    // synchronicity).
		    uint rlen = len - rnum;
		    char *cbuf = new char[ rlen ];
		    if ( ungetHack != EOF ) {
			rlen = 1+dev->readBlock( cbuf+1, rlen-1 );
			cbuf[0] = (char)ungetHack;
			ungetHack = EOF;
		    } else {
			rlen = dev->readBlock( cbuf, rlen );
		    }
		    s  += d->decoder->toUnicode( cbuf, rlen );
		    delete[] cbuf;
		    // use buffered reading only for the first time, because we
		    // have to get the stream synchronous again (this is easier
		    // with single character reading)
		    readBlock = false;
		}
		// get stream (and codec) in sync
		int c;
		if ( ungetHack == EOF ) {
		    c = dev->getch();
		} else {
		    c = ungetHack;
		    ungetHack = EOF;
		}
		if ( c == EOF ) {
		    shortRead = true;
		    break;
		}
		char b = c;
		uint lengthBefore = s.length();
		s += d->decoder->toUnicode( &b, 1 );
		if ( s.length() > lengthBefore )
		    break; // it seems we are in sync now
	    }
	    uint i = 0;
	    uint end = TQMIN( len-rnum, s.length() );
	    while( i < end ) {
		*buf = s.constref(i++);
		buf++;
	    }
	    rnum += end;
	    if ( s.length() > i ) {
		// could be = but append is clearer
		d->ungetcBuf.append( s.mid( i ) );
	    }
	    if ( shortRead )
		return rnum;
	}
    } else
#endif
    if ( latin1 ) {
	if ( len == 1+rnum ) {
	    // use this method for one character because it is more efficient
	    // (arnt doubts whether it makes a difference, but lets it stand)
	    int c = (ungetHack == EOF) ? dev->getch() : ungetHack;
	    if ( c != EOF ) {
		*buf = (char)c;
		buf++;
		rnum++;
	    }
	} else {
	    if ( ungetHack != EOF ) {
		*buf = (char)ungetHack;
		buf++;
		rnum++;
		ungetHack = EOF;
	    }
	    char *cbuf = new char[len - rnum];
	    while ( !dev->atEnd() && rnum < len ) {
		uint rlen = len - rnum;
		rlen = dev->readBlock( cbuf, rlen );
		char *it = cbuf;
		char *end = cbuf + rlen;
		while ( it < end ) {
		    *buf = *it;
		    buf++;
		    it++;
		}
		rnum += rlen;
	    }
	    delete[] cbuf;
	}
    } else { // UCS-2 or UTF-16
	if ( len == 1+rnum ) {
	    int c1 = (ungetHack == EOF) ? dev->getch() : ungetHack;
	    if ( c1 == EOF )
		return rnum;
	    int c2 = dev->getch();
	    if ( c2 == EOF )
		return rnum;

	    if ( networkOrder ) {
		*buf = TQChar( c2, c1 );
	    } else {
		*buf = TQChar( c1, c2 );
	    }
	    buf++;
	    rnum++;
	} else {
	    char *cbuf = new char[ 2*( len - rnum ) ]; // for paranoids: overflow possible
	    while ( !dev->atEnd() && rnum < len ) {
		uint rlen = 2 * ( len-rnum );
		if ( ungetHack != EOF ) {
		    rlen = 1+dev->readBlock( cbuf+1, rlen-1 );
		    cbuf[0] = (char)ungetHack;
		    ungetHack = EOF;
		} else {
		    rlen = dev->readBlock( cbuf, rlen );
		}
		// We can't use an odd number of bytes, so put it back. But
		// do it only if we are capable of reading more -- normally
		// there should not be an odd number, but the file might be
		// truncated or not in UTF-16...
		if ( (rlen & 1) == 1 )
		    if ( !dev->atEnd() )
			dev->ungetch( cbuf[--rlen] );
		uint i = 0;
		if ( networkOrder ) {
		    while( i < rlen ) {
			*buf = TQChar( cbuf[i+1], cbuf[i] );
			buf++;
			i+=2;
		    }
		} else {
		    while( i < rlen ) {
			*buf = TQChar( cbuf[i], cbuf[i+1] );
			buf++;
			i+=2;
		    }
		}
		rnum += i/2;
	    }
	    delete[] cbuf;
	}
    }
    return rnum;
}

/*!
    Tries to read one line, but at most len characters from the stream
    and stores them in \a buf.

    Returns the number of characters really read. Newlines are not
    stripped.

    There will be a TQEOF appended if the read reaches the end of file;
    this is different to ts_getbuf().

    This function works only if a newline (as byte) is also a newline
    (as resulting character) since it uses TQIODevice::readLine(). So
    use it only for such codecs where this is true!

    This function is (almost) a no-op for UTF 16. Don't use it if
    doUnicodeHeader is true!
*/
uint TQTextStream::ts_getline( TQChar* buf )
{
    uint rnum=0;   // the number of TQChars really read
    char cbuf[ getline_buf_size+1 ];

    if ( d && d->ungetcBuf.length() ) {
	while( rnum < getline_buf_size && rnum < d->ungetcBuf.length() ) {
	    buf[rnum] = d->ungetcBuf.constref(rnum);
	    rnum++;
	}
	d->ungetcBuf = d->ungetcBuf.mid( rnum );
	if ( rnum >= getline_buf_size )
	    return rnum;
    }

#ifndef TQT_NO_TEXTCODEC
    if ( mapper ) {
	if ( !d->decoder )
	    d->decoder = mapper->makeDecoder();
	TQString s;
	bool readBlock = true;
	for (;;) {
	    // for efficiency: try to read a line
	    if ( readBlock ) {
		int rlen = getline_buf_size - rnum;
		rlen = dev->readLine( cbuf, rlen+1 );
		if ( rlen == -1 )
		    rlen = 0;
		s  += d->decoder->toUnicode( cbuf, rlen );
		readBlock = false;
	    }
	    if ( dev->atEnd()
		    || s.at( s.length()-1 ) == '\n'
		    || s.at( s.length()-1 ) == '\r'
	       ) {
		break;
	    } else {
		// get stream (and codec) in sync
		int c;
		c = dev->getch();
		if ( c == EOF ) {
		    break;
		}
		char b = c;
		uint lengthBefore = s.length();
		s  += d->decoder->toUnicode( &b, 1 );
		if ( s.length() > lengthBefore )
		    break; // it seems we are in sync now
	    }
	}
	uint i = 0;
	while( rnum < getline_buf_size && i < s.length() )
	    buf[rnum++] = s.constref(i++);
	if ( s.length() > i )
	    // could be = but append is clearer
	    d->ungetcBuf.append( s.mid( i ) );
	if ( rnum < getline_buf_size && dev->atEnd() )
	    buf[rnum++] = TQEOF;
    } else
#endif
    if ( latin1 ) {
	int rlen = getline_buf_size - rnum;
	rlen = dev->readLine( cbuf, rlen+1 );
	if ( rlen == -1 )
	    rlen = 0;
	char *end = cbuf+rlen;
	char *it = cbuf;
	buf +=rnum;
	while ( it != end ) {
	    buf->setCell( *(it++) );
	    buf->setRow( 0 );
	    buf++;
	}
	rnum += rlen;
	if ( rnum < getline_buf_size && dev->atEnd() )
	    buf[1] = TQEOF;
    }
    return rnum;
}


/*!
    Puts one character into the stream.
*/
void TQTextStream::ts_putc( TQChar c )
{
#ifndef TQT_NO_TEXTCODEC
    if ( mapper ) {
	if ( !d->encoder )
	    d->encoder = mapper->makeEncoder();
	int len = 1;
	TQString s = c;
	TQCString block = d->encoder->fromUnicode( s, len );
	dev->writeBlock( block, len );
    } else
#endif
    if ( latin1 ) {
	if ( c.row() )
	    dev->putch( '?' ); // unknown character
	else
	    dev->putch( c.cell() );
    } else {
	if ( doUnicodeHeader ) {
	    doUnicodeHeader = false;
	    ts_putc( TQChar::byteOrderMark );
	}
	if ( internalOrder ) {
	    // this case is needed by TQStringBuffer
	    dev->writeBlock( (char*)&c, sizeof(TQChar) );
	} else if ( networkOrder ) {
	    dev->putch( c.row() );
	    dev->putch( c.cell() );
	} else {
	    dev->putch( c.cell() );
	    dev->putch( c.row() );
	}
    }
}

/*!
    Puts one character into the stream.
*/
void TQTextStream::ts_putc( int ch )
{
    ts_putc( TQChar((ushort)ch) );
}

bool TQTextStream::ts_isdigit( TQChar c )
{
    return c.isDigit();
}

bool TQTextStream::ts_isspace( TQChar c )
{
    return c.isSpace();
}

void TQTextStream::ts_ungetc( TQChar c )
{
    if ( c.unicode() == 0xffff )
	return;

    d->ungetcBuf.prepend( c );
}



/*!
    Reads \a len bytes from the stream into \a s and returns a
    reference to the stream.

    The buffer \a s must be preallocated.

    Note that no encoding is done by this function.

    \warning The behavior of this function is undefined unless the
    stream's encoding is set to Unicode or Latin1.

    \sa TQIODevice::readBlock()
*/

TQTextStream &TQTextStream::readRawBytes( char *s, uint len )
{
    dev->readBlock( s, len );
    return *this;
}

/*!
    Writes the \a len bytes from \a s to the stream and returns a
    reference to the stream.

    Note that no encoding is done by this function.

    \sa TQIODevice::writeBlock()
*/

TQTextStream &TQTextStream::writeRawBytes( const char* s, uint len )
{
    dev->writeBlock( s, len );
    return *this;
}


TQTextStream &TQTextStream::writeBlock( const char* p, uint len )
{
    if ( doUnicodeHeader ) {
	doUnicodeHeader = false;
	if ( !mapper && !latin1 )
	    ts_putc( TQChar::byteOrderMark );
    }
    // TQCString and const char * are treated as Latin-1
    if ( !mapper && latin1 ) {
	dev->writeBlock( p, len );
    } else if ( !mapper && internalOrder ) {
	TQChar *u = new TQChar[len];
	for ( uint i = 0; i < len; i++ )
	    u[i] = p[i];
	dev->writeBlock( (char*)u, len * sizeof(TQChar) );
	delete [] u;
    }
#ifndef TQT_NO_TEXTCODEC
    else if (mapper) {
        if (!d->encoder)
            d->encoder = mapper->makeEncoder();
        TQString s = TQString::fromLatin1(p, len);
        int l = len;
        TQCString block = d->encoder->fromUnicode(s, l);
        dev->writeBlock(block, l);
    }
#endif
    else {
	for ( uint i = 0; i < len; i++ )
	    ts_putc( (uchar)p[i] );
    }
    return *this;
}

TQTextStream &TQTextStream::writeBlock( const TQChar* p, uint len )
{
#ifndef TQT_NO_TEXTCODEC
    if ( mapper ) {
	if ( !d->encoder )
	    d->encoder = mapper->makeEncoder();
	TQConstString s( p, len );
	int l = len;
	TQCString block = d->encoder->fromUnicode( s.string(), l );
	dev->writeBlock( block, l );
    } else
#endif
    if ( latin1 ) {
	char *str = TQString::unicodeToLatin1( p, len );
	dev->writeBlock( str, len );
	delete [] str;
    } else if ( internalOrder ) {
	if ( doUnicodeHeader ) {
	    doUnicodeHeader = false;
	    ts_putc( TQChar::byteOrderMark );
	}
	dev->writeBlock( (char*)p, sizeof(TQChar)*len );
    } else {
	for (uint i=0; i<len; i++)
	    ts_putc( p[i] );
    }
    return *this;
}

/*!
    Resets the text stream.

    \list
    \i All flags are set to 0.
    \i The field width is set to 0.
    \i The fill character is set to ' ' (Space).
    \i The precision is set to 6.
    \endlist

    \sa setf(), width(), fill(), precision()
*/

void TQTextStream::reset()
{
    fflags = 0;
    fwidth = 0;
    fillchar = ' ';
    fprec = 6;
}

/*!
    \fn TQIODevice *TQTextStream::device() const

    Returns the IO device currently set.

    \sa setDevice(), unsetDevice()
*/

/*!
    Sets the IO device to \a iod.

    \sa device(), unsetDevice()
*/

void TQTextStream::setDevice( TQIODevice *iod )
{
    if ( owndev ) {
	delete dev;
	owndev = false;
    }
    dev = iod;
    d->sourceType = TQTextStreamPrivate::IODevice;
}

/*!
    Unsets the IO device. Equivalent to setDevice( 0 ).

    \sa device(), setDevice()
*/

void TQTextStream::unsetDevice()
{
    setDevice( 0 );
    d->sourceType = TQTextStreamPrivate::NotSet;
}

/*!
    \fn bool TQTextStream::atEnd() const

    Returns true if the IO device has reached the end position (end of
    the stream or file) or if there is no IO device set; otherwise
    returns false.

    \sa TQIODevice::atEnd()
*/

/*!\fn bool TQTextStream::eof() const

  \obsolete

  This function has been renamed to atEnd().

  \sa TQIODevice::atEnd()
*/

/*****************************************************************************
  TQTextStream read functions
 *****************************************************************************/


/*!
    \overload

    Reads a char \a c from the stream and returns a reference to the
    stream. Note that whitespace is skipped.
*/

TQTextStream &TQTextStream::operator>>( char &c )
{
    CHECK_STREAM_PRECOND
    c = eat_ws();
    return *this;
}

/*!
    Reads a char \a c from the stream and returns a reference to the
    stream. Note that whitespace is \e not skipped.
*/

TQTextStream &TQTextStream::operator>>( TQChar &c )
{
    CHECK_STREAM_PRECOND
    c = ts_getc();
    return *this;
}


ulong TQTextStream::input_bin()
{
    ulong val = 0;
    TQChar ch = eat_ws();
    int dv = ch.digitValue();
    while (  dv == 0 || dv == 1 ) {
	val = ( val << 1 ) + dv;
	ch = ts_getc();
	dv = ch.digitValue();
    }
    if ( ch != TQEOF )
	ts_ungetc( ch );
    return val;
}

ulong TQTextStream::input_oct()
{
    ulong val = 0;
    TQChar ch = eat_ws();
    int dv = ch.digitValue();
    while ( dv >= 0 && dv <= 7 ) {
	val = ( val << 3 ) + dv;
	ch = ts_getc();
	dv = ch.digitValue();
    }
    if ( dv == 8 || dv == 9 ) {
	while ( ts_isdigit(ch) )
	    ch = ts_getc();
    }
    if ( ch != TQEOF )
	ts_ungetc( ch );
    return val;
}

ulong TQTextStream::input_dec()
{
    ulong val = 0;
    TQChar ch = eat_ws();
    int dv = ch.digitValue();
    while ( ts_isdigit(ch) ) {
	val = val * 10 + dv;
	ch = ts_getc();
	dv = ch.digitValue();
    }
    if ( ch != TQEOF )
	ts_ungetc( ch );
    return val;
}

ulong TQTextStream::input_hex()
{
    ulong val = 0;
    TQChar ch = eat_ws();
    char c = ch;
    while ( isxdigit((uchar) c) ) {
	val <<= 4;
	if ( ts_isdigit(c) )
	    val += c - '0';
	else
	    val += 10 + tolower( (uchar) c ) - 'a';
	c = ch = ts_getc();
    }
    if ( ch != TQEOF )
	ts_ungetc( ch );
    return val;
}

long TQTextStream::input_int()
{
    long val;
    TQChar ch;
    char c;
    switch ( flags() & basefield ) {
    case bin:
	val = (long)input_bin();
	break;
    case oct:
	val = (long)input_oct();
	break;
    case dec:
	c = ch = eat_ws();
	if ( ch == TQEOF ) {
	    val = 0;
	} else {
	    if ( !(c == '-' || c == '+') )
		ts_ungetc( ch );
	    if ( c == '-' ) {
		ulong v = input_dec();
		if ( v ) {		// ensure that LONG_MIN can be read
		    v--;
		    val = -((long)v) - 1;
		} else {
		    val = 0;
		}
	    } else {
		val = (long)input_dec();
	    }
	}
	break;
    case hex:
	val = (long)input_hex();
	break;
    default:
	val = 0;
	c = ch = eat_ws();
	if ( c == '0' ) {		// bin, oct or hex
	    c = ch = ts_getc();
	    if ( tolower((uchar) c) == 'x' )
		val = (long)input_hex();
	    else if ( tolower((uchar) c) == 'b' )
		val = (long)input_bin();
	    else {			// octal
		ts_ungetc( ch );
		if ( c >= '0' && c <= '7' ) {
		    val = (long)input_oct();
		} else {
		    val = 0;
		}
	    }
	} else if ( ts_isdigit(ch) ) {
	    ts_ungetc( ch );
	    val = (long)input_dec();
	} else if ( c == '-' || c == '+' ) {
	    ulong v = input_dec();
	    if ( c == '-' ) {
		if ( v ) {		// ensure that LONG_MIN can be read
		    v--;
		    val = -((long)v) - 1;
		} else {
		    val = 0;
		}
	    } else {
		val = (long)v;
	    }
	}
    }
    return val;
}

//
// We use a table-driven FSM to parse floating point numbers
// strtod() cannot be used directly since we're reading from a TQIODevice
//

double TQTextStream::input_double()
{
    const int Init	 = 0;			// states
    const int Sign	 = 1;
    const int Mantissa	 = 2;
    const int Dot	 = 3;
    const int Abscissa	 = 4;
    const int ExpMark	 = 5;
    const int ExpSign	 = 6;
    const int Exponent	 = 7;
    const int Done	 = 8;

    const int InputSign	 = 1;			// input tokens
    const int InputDigit = 2;
    const int InputDot	 = 3;
    const int InputExp	 = 4;

    static const uchar table[8][5] = {
     /* None	 InputSign   InputDigit InputDot InputExp */
	{ 0,	    Sign,     Mantissa,	 Dot,	   0,	   }, // Init
	{ 0,	    0,	      Mantissa,	 Dot,	   0,	   }, // Sign
	{ Done,	    Done,     Mantissa,	 Dot,	   ExpMark,}, // Mantissa
	{ 0,	    0,	      Abscissa,	 0,	   0,	   }, // Dot
	{ Done,	    Done,     Abscissa,	 Done,	   ExpMark,}, // Abscissa
	{ 0,	    ExpSign,  Exponent,	 0,	   0,	   }, // ExpMark
	{ 0,	    0,	      Exponent,	 0,	   0,	   }, // ExpSign
	{ Done,	    Done,     Exponent,	 Done,	   Done	   }  // Exponent
    };

    int state = Init;				// parse state
    int input;					// input token

    char buf[256];
    int i = 0;
    TQChar c = eat_ws();

    for (;;) {

	switch ( c ) {
	    case '+':
	    case '-':
		input = InputSign;
		break;
	    case '0': case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7': case '8': case '9':
		input = InputDigit;
		break;
	    case '.':
		input = InputDot;
		break;
	    case 'e':
	    case 'E':
		input = InputExp;
		break;
	    default:
		input = 0;
		break;
	}

	state = table[state][input];

	if  ( state == 0 || state == Done || i > 250 ) {
	    if ( i > 250 ) {			// ignore rest of digits
		do { c = ts_getc(); } while ( c != TQEOF && ts_isdigit(c) );
	    }
	    if ( c != TQEOF )
		ts_ungetc( c );
	    buf[i] = '\0';
	    char *end;
	    return strtod( buf, &end );
	}

	buf[i++] = c;
	c = ts_getc();
    }

#if !defined(Q_CC_EDG)
    return 0.0;
#endif
}


/*!
    \overload

    Reads a signed \c short integer \a i from the stream and returns a
    reference to the stream. See flags() for an explanation of the
    expected input format.
*/

TQTextStream &TQTextStream::operator>>( signed short &i )
{
    CHECK_STREAM_PRECOND
    i = (signed short)input_int();
    return *this;
}


/*!
    \overload

    Reads an unsigned \c short integer \a i from the stream and
    returns a reference to the stream. See flags() for an explanation
    of the expected input format.
*/

TQTextStream &TQTextStream::operator>>( unsigned short &i )
{
    CHECK_STREAM_PRECOND
    i = (unsigned short)input_int();
    return *this;
}


/*!
    \overload

    Reads a signed \c int \a i from the stream and returns a reference
    to the stream. See flags() for an explanation of the expected
    input format.
*/

TQTextStream &TQTextStream::operator>>( signed int &i )
{
    CHECK_STREAM_PRECOND
    i = (signed int)input_int();
    return *this;
}


/*!
    \overload

    Reads an unsigned \c int \a i from the stream and returns a
    reference to the stream. See flags() for an explanation of the
    expected input format.
*/

TQTextStream &TQTextStream::operator>>( unsigned int &i )
{
    CHECK_STREAM_PRECOND
    i = (unsigned int)input_int();
    return *this;
}


/*!
    \overload

    Reads a signed \c long int \a i from the stream and returns a
    reference to the stream. See flags() for an explanation of the
    expected input format.
*/

TQTextStream &TQTextStream::operator>>( signed long &i )
{
    CHECK_STREAM_PRECOND
    i = (signed long)input_int();
    return *this;
}


/*!
    \overload

    Reads an unsigned \c long int \a i from the stream and returns a
    reference to the stream. See flags() for an explanation of the
    expected input format.
*/

TQTextStream &TQTextStream::operator>>( unsigned long &i )
{
    CHECK_STREAM_PRECOND
    i = (unsigned long)input_int();
    return *this;
}


/*!
    \overload

    Reads a \c float \a f from the stream and returns a reference to
    the stream. See flags() for an explanation of the expected input
    format.
*/

TQTextStream &TQTextStream::operator>>( float &f )
{
    CHECK_STREAM_PRECOND
    f = (float)input_double();
    return *this;
}


/*!
    \overload

    Reads a \c double \a f from the stream and returns a reference to
    the stream. See flags() for an explanation of the expected input
    format.
*/

TQTextStream &TQTextStream::operator>>( double &f )
{
    CHECK_STREAM_PRECOND
    f = input_double();
    return *this;
}


/*!
    \overload

    Reads a "word" from the stream into \a s and returns a reference
    to the stream.

    A word consists of characters for which isspace() returns false.
*/

TQTextStream &TQTextStream::operator>>( char *s )
{
    CHECK_STREAM_PRECOND
    int maxlen = width( 0 );
    TQChar c = eat_ws();
    if ( !maxlen )
	maxlen = -1;
    while ( c != TQEOF ) {
	if ( ts_isspace(c) || maxlen-- == 0 ) {
	    ts_ungetc( c );
	    break;
	}
	*s++ = c;
	c = ts_getc();
    }

    *s = '\0';
    return *this;
}

/*!
    \overload

    Reads a "word" from the stream into \a str and returns a reference
    to the stream.

    A word consists of characters for which isspace() returns false.
*/

TQTextStream &TQTextStream::operator>>( TQString &str )
{
    CHECK_STREAM_PRECOND
    str=TQString::fromLatin1("");
    TQChar	c = eat_ws();

    while ( c != TQEOF ) {
	if ( ts_isspace(c) ) {
	    ts_ungetc( c );
	    break;
	}
	str += c;
	c = ts_getc();
    }
    return *this;
}

/*!
    \overload

    Reads a "word" from the stream into \a str and returns a reference
    to the stream.

    A word consists of characters for which isspace() returns false.
*/

TQTextStream &TQTextStream::operator>>( TQCString &str )
{
    CHECK_STREAM_PRECOND
    TQCString  *dynbuf = 0;
    const int buflen = 256;
    char      buffer[buflen];
    char     *s = buffer;
    int	      i = 0;
    TQChar	      c = eat_ws();

    while ( c != TQEOF ) {
	if ( ts_isspace(c) ) {
	    ts_ungetc( c );
	    break;
	}
	if ( i >= buflen-1 ) {
	    if ( !dynbuf )  {			// create dynamic buffer
		dynbuf = new TQCString(buflen*2);
		memcpy( dynbuf->data(), s, i );	// copy old data
	    } else if ( i >= (int)dynbuf->size()-1 ) {
		dynbuf->resize( dynbuf->size()*2 );
	    }
	    s = dynbuf->data();
	}
	s[i++] = c;
	c = ts_getc();
    }
    str.resize( i+1 );
    memcpy( str.data(), s, i );
    delete dynbuf;
    return *this;
}


/*!
    Reads a line from the stream and returns a string containing the
    text.

    The returned string does not contain any trailing newline or
    carriage return. Note that this is different from
    TQIODevice::readLine(), which does not strip the newline at the end
    of the line.

    On EOF you will get a TQString that is null. On reading an empty
    line the returned TQString is empty but not null.

    \sa TQIODevice::readLine()
*/

TQString TQTextStream::readLine()
{
#if defined(QT_CHECK_STATE)
    if ( !dev ) {
	tqWarning( "TQTextStream::readLine: No device" );
	return TQString::null;
    }
#endif
    bool readCharByChar = true;
    TQString result;
#if 0
    if ( !doUnicodeHeader && (
	    (latin1) ||
	    (mapper != 0 && mapper->mibEnum() == 106 ) // UTF 8
       ) ) {
	readCharByChar = false;
	// use optimized read line
	TQChar c[getline_buf_size];
	int pos = 0;
	bool eof = false;

	for (;;) {
	    pos = ts_getline( c );
	    if ( pos == 0 ) {
		// something went wrong; try fallback
		readCharByChar = true;
		//dev->resetStatus();
		break;
	    }
	    if ( c[pos-1] == TQEOF || c[pos-1] == '\n' ) {
		if ( pos>2 && c[pos-1]==TQEOF && c[pos-2]=='\n' ) {
		    result += TQString( c, pos-2 );
		} else if ( pos > 1 ) {
		    result += TQString( c, pos-1 );
		}
		if ( pos == 1 && c[pos-1] == TQEOF )
		    eof = true;
		break;
	    } else {
		result += TQString( c, pos );
	    }
	}
	if ( eof && result.isEmpty() )
	    return TQString::null;
    }
#endif
    if ( readCharByChar ) {
	const int buf_size = 256;
	TQChar c[buf_size];
	int pos = 0;

	c[pos] = ts_getc();
	if ( c[pos] == TQEOF )
	    return TQString::null;

	while ( c[pos] != TQEOF && c[pos] != '\n' ) {
	    if ( c[pos] == '\r' ) { // ( handle mac and dos )
		TQChar nextc = ts_getc();
		if ( nextc != '\n' )
		    ts_ungetc( nextc );
		break;
	    }
	    pos++;
	    if ( pos >= buf_size ) {
		result += TQString( c, pos );
		pos = 0;
	    }
	    c[pos] = ts_getc();
	}
	result += TQString( c, pos );
    }

    return result;
}


/*!
    Reads the entire stream from the current position, and returns a string
    containing the text.

    \sa TQIODevice::readLine()
*/

TQString TQTextStream::read()
{
#if defined(QT_CHECK_STATE)
    if ( !dev ) {
	tqWarning( "TQTextStream::read: No device" );
	return TQString::null;
    }
#endif
    TQString    result;
    const uint bufsize = 512;
    TQChar      buf[bufsize];
    uint       i, num, start;
    bool       skipped_cr = false;

    for (;;) {
	num = ts_getbuf(buf,bufsize);
	// convert dos (\r\n) and mac (\r) style eol to unix style (\n)
	start = 0;
	for ( i=0; i<num; i++ ) {
	    if ( buf[i] == '\r' ) {
		// Only skip single cr's preceding lf's
		if ( skipped_cr ) {
		    result += buf[i];
		    start++;
		} else {
		    result += TQString( &buf[start], i-start );
		    start = i+1;
		    skipped_cr = true;
		}
	    } else {
		if ( skipped_cr ) {
		    if ( buf[i] != '\n' ) {
			// Should not have skipped it
			result += '\n';
		    }
		    skipped_cr = false;
		}
	    }
	}
	if ( start < num )
	    result += TQString( &buf[start], i-start );
	if ( num != bufsize ) // if ( EOF )
	    break;
    }
    return result;
}



/*****************************************************************************
  TQTextStream write functions
 *****************************************************************************/

/*!
    Writes character \c char to the stream and returns a reference to
    the stream.

    The character \a c is assumed to be Latin1 encoded independent of
    the Encoding set for the TQTextStream.
*/
TQTextStream &TQTextStream::operator<<( TQChar c )
{
    CHECK_STREAM_PRECOND
    ts_putc( c );
    return *this;
}

/*!
    \overload

    Writes character \a c to the stream and returns a reference to the
    stream.
*/
TQTextStream &TQTextStream::operator<<( char c )
{
    CHECK_STREAM_PRECOND
    unsigned char uc = (unsigned char) c;
    ts_putc( uc );
    return *this;
}

TQTextStream &TQTextStream::output_int( int format, unsigned long long n, bool neg )
{
    static const char hexdigits_lower[] = "0123456789abcdef";
    static const char hexdigits_upper[] = "0123456789ABCDEF";
    CHECK_STREAM_PRECOND
    char buf[76];
    char *p;
    int	  len;
    const char *hexdigits;

    switch ( flags() & I_BASE_MASK ) {

	case I_BASE_2:				// output binary number
	    switch ( format & I_TYPE_MASK ) {
		case I_SHORT: len=16; break;
		case I_INT:   len=sizeof(int)*8; break;
		case I_LONG:  len=32; break;
		case I_LONGLONG: len=64; break;
		default:      len = 0;
	    }
	    p = &buf[74];			// go reverse order
	    *p = '\0';
	    while ( len-- ) {
		*--p = (char)(n&1) + '0';
		n >>= 1;
		if ( !n )
		    break;
	    }
	    if ( flags() & showbase ) {		// show base
		*--p = (flags() & uppercase) ? 'B' : 'b';
		*--p = '0';
	    }
	    break;

	case I_BASE_8:				// output octal number
	    p = &buf[74];
	    *p = '\0';
	    do {
		*--p = (char)(n&7) + '0';
		n >>= 3;
	    } while ( n );
	    if ( flags() & showbase )
		*--p = '0';
	    break;

	case I_BASE_16:				// output hexadecimal number
	    p = &buf[74];
	    *p = '\0';
	    hexdigits = (flags() & uppercase) ?
		hexdigits_upper : hexdigits_lower;
	    do {
		*--p = hexdigits[(int)n&0xf];
		n >>= 4;
	    } while ( n );
	    if ( flags() & showbase ) {
		*--p = (flags() & uppercase) ? 'X' : 'x';
		*--p = '0';
	    }
	    break;

	default:				// decimal base is default
	    p = &buf[74];
	    *p = '\0';
	    if ( neg )
		n = (unsigned long long)(-(long long)n);
	    do {
		*--p = ((int)(n%10)) + '0';
		n /= 10;
	    } while ( n );
	    if ( neg )
		*--p = '-';
	    else if ( flags() & showpos )
		*--p = '+';
	    if ( (flags() & internal) && fwidth && !ts_isdigit(*p) ) {
		ts_putc( *p );			// special case for internal
		++p;				//   padding
		fwidth--;
		return *this << (const char*)p;
	    }
    }
    if ( fwidth ) {				// adjustment required
	if ( !(flags() & left) ) {		// but NOT left adjustment
	    len = tqstrlen(p);
	    int padlen = fwidth - len;
	    if ( padlen <= 0 ) {		// no padding required
		writeBlock( p, len );
	    } else if ( padlen < (int)(p-buf) ) { // speeds up padding
		memset( p-padlen, (char)fillchar, padlen );
		writeBlock( p-padlen, padlen+len );
	    }
	    else				// standard padding
		*this << (const char*)p;
	}
	else
	    *this << (const char*)p;
	fwidth = 0;				// reset field width
    }
    else
	writeBlock( p, tqstrlen(p) );
    return *this;
}


/*!
    \overload

    Writes a \c short integer \a i to the stream and returns a
    reference to the stream.
*/

TQTextStream &TQTextStream::operator<<( signed short i )
{
    return output_int( I_SHORT | I_SIGNED, i, i < 0 );
}


/*!
    \overload

    Writes an \c unsigned \c short integer \a i to the stream and
    returns a reference to the stream.
*/

TQTextStream &TQTextStream::operator<<( unsigned short i )
{
    return output_int( I_SHORT | I_UNSIGNED, i, false );
}


/*!
    \overload

    Writes an \c int \a i to the stream and returns a reference to the
    stream.
*/

TQTextStream &TQTextStream::operator<<( signed int i )
{
    return output_int( I_INT | I_SIGNED, i, i < 0 );
}


/*!
    \overload

    Writes an \c unsigned \c int \a i to the stream and returns a
    reference to the stream.
*/

TQTextStream &TQTextStream::operator<<( unsigned int i )
{
    return output_int( I_INT | I_UNSIGNED, i, false );
}


/*!
    \overload

    Writes a \c long \c int \a i to the stream and returns a reference
    to the stream.
*/

TQTextStream &TQTextStream::operator<<( signed long i )
{
    return output_int( I_LONG | I_SIGNED, i, i < 0 );
}


/*!
    \overload

    Writes an \c unsigned \c long \c int \a i to the stream and
    returns a reference to the stream.
*/

TQTextStream &TQTextStream::operator<<( unsigned long i )
{
    return output_int( I_LONG | I_UNSIGNED, i, false );
}

/*!
    \overload

    Writes a \c long long \c int \a i to the stream and returns a reference
    to the stream.
*/

TQTextStream &TQTextStream::operator<<( signed long long i )
{
    return output_int( I_LONGLONG | I_SIGNED, i, i < 0 );
}


/*!
    \overload

    Writes an \c unsigned \c long \c int \a i to the stream and
    returns a reference to the stream.
*/

TQTextStream &TQTextStream::operator<<( unsigned long long i )
{
    return output_int( I_LONGLONG | I_UNSIGNED, i, false );
}


/*!
    \overload

    Writes a \c float \a f to the stream and returns a reference to
    the stream.
*/

TQTextStream &TQTextStream::operator<<( float f )
{
    return *this << (double)f;
}

/*!
    \overload

    Writes a \c double \a f to the stream and returns a reference to
    the stream.
*/

TQTextStream &TQTextStream::operator<<( double f )
{
    CHECK_STREAM_PRECOND
    char f_char;
    char format[16];
    if ( (flags()&floatfield) == fixed )
	f_char = 'f';
    else if ( (flags()&floatfield) == scientific )
	f_char = (flags() & uppercase) ? 'E' : 'e';
    else
	f_char = (flags() & uppercase) ? 'G' : 'g';
    char *fs = format;			// generate format string
    *fs++ = '%';				//   "%.<prec>l<f_char>"
    *fs++ = '.';
    int prec = precision();
    if ( prec > 99 )
	prec = 99;
    if ( prec >= 10 ) {
	*fs++ = prec / 10 + '0';
	*fs++ = prec % 10 + '0';
    } else {
	*fs++ = prec + '0';
    }
    *fs++ = 'l';
    *fs++ = f_char;
    *fs = '\0';
    TQString num;
    num.sprintf(format, f);			// convert to text
    if ( fwidth )				// padding
	*this << num.latin1();
    else					// just write it
	writeBlock(num.latin1(), num.length());
    return *this;
}


/*!
    \overload

    Writes a string to the stream and returns a reference to the
    stream.

    The string \a s is assumed to be Latin1 encoded independent of the
    Encoding set for the TQTextStream.
*/

TQTextStream &TQTextStream::operator<<( const char* s )
{
    CHECK_STREAM_PRECOND
    char padbuf[48];
    uint len = tqstrlen( s );			// don't write null terminator
    if ( fwidth ) {				// field width set
	int padlen = fwidth - len;
	fwidth = 0;				// reset width
	if ( padlen > 0 ) {
	    char *ppad;
	    if ( padlen > 46 ) {		// create extra big fill buffer
		ppad = new char[padlen];
		TQ_CHECK_PTR( ppad );
	    } else {
		ppad = padbuf;
	    }
	    memset( ppad, (char)fillchar, padlen );	// fill with fillchar
	    if ( !(flags() & left) ) {
		writeBlock( ppad, padlen );
		padlen = 0;
	    }
	    writeBlock( s, len );
	    if ( padlen )
		writeBlock( ppad, padlen );
	    if ( ppad != padbuf )		// delete extra big fill buf
		delete[] ppad;
	    return *this;
	}
    }
    writeBlock( s, len );
    return *this;
}

/*!
    \overload

    Writes \a s to the stream and returns a reference to the stream.

    The string \a s is assumed to be Latin1 encoded independent of the
    Encoding set for the TQTextStream.
*/

TQTextStream &TQTextStream::operator<<( const TQCString & s )
{
    return operator<<(s.data());
}

/*!
    \overload

    Writes \a s to the stream and returns a reference to the stream.
*/

TQTextStream &TQTextStream::operator<<( const TQString& s )
{
    if ( !mapper && latin1 )
	return operator<<(s.latin1());
    CHECK_STREAM_PRECOND
    TQString s1 = s;
    if ( fwidth ) {				// field width set
	if ( !(flags() & left) ) {
	    s1 = s.rightJustify(fwidth, (char)fillchar);
	} else {
	    s1 = s.leftJustify(fwidth, (char)fillchar);
	}
	fwidth = 0;				// reset width
    }
    writeBlock( s1.unicode(), s1.length() );
    return *this;
}


/*!
    \overload

    Writes a pointer to the stream and returns a reference to the
    stream.

    The \a ptr is output as an unsigned long hexadecimal integer.
*/

TQTextStream &TQTextStream::operator<<( void *ptr )
{
    int f = flags();
    setf( hex, basefield );
    setf( showbase );
    unsetf( uppercase );
    output_int( I_LONG | I_UNSIGNED, (ulong)ptr, false );
    flags( f );
    return *this;
}


/*!
    \fn int TQTextStream::flags() const

    Returns the current stream flags. The default value is 0.

    \table
    \header \i Flag \i Meaning
    \row \i \c skipws \i Not currently used; whitespace always skipped
    \row \i \c left \i Numeric fields are left-aligned
    \row \i \c right
	 \i Not currently used (by default, numerics are right-aligned)
    \row \i \c internal \i Puts any padding spaces between +/- and value
    \row \i \c bin \i Output \e and input only in binary
    \row \i \c oct \i Output \e and input only in octal
    \row \i \c dec \i Output \e and input only in decimal
    \row \i \c hex \i Output \e and input only in hexadecimal
    \row \i \c showbase
	 \i Annotates numeric outputs with 0b, 0, or 0x if in \c bin,
	 \c oct, or \c hex format
    \row \i \c showpoint \i Not currently used
    \row \i \c uppercase \i Uses 0B and 0X rather than 0b and 0x
    \row \i \c showpos \i Shows + for positive numeric values
    \row \i \c scientific \i Uses scientific notation for floating point values
    \row \i \c fixed \i Uses fixed-point notation for floating point values
    \endtable

    Note that unless \c bin, \c oct, \c dec, or \c hex is set, the
    input base is octal if the value starts with 0, hexadecimal if it
    starts with 0x, binary if it starts with 0b, and decimal
    otherwise.

    \sa setf(), unsetf()
*/

/*!
    \fn int TQTextStream::flags( int f )

    \overload

    Sets the stream flags to \a f. Returns the previous stream flags.

    \sa setf(), unsetf(), flags()
*/

/*!
    \fn int TQTextStream::setf( int bits )

    Sets the stream flag bits \a bits. Returns the previous stream
    flags.

    Equivalent to \c{flags( flags() | bits )}.

    \sa setf(), unsetf()
*/

/*!
    \fn int TQTextStream::setf( int bits, int mask )

    \overload

    Sets the stream flag bits \a bits with a bit mask \a mask. Returns
    the previous stream flags.

    Equivalent to \c{flags( (flags() & ~mask) | (bits & mask) )}.

    \sa setf(), unsetf()
*/

/*!
    \fn int TQTextStream::unsetf( int bits )

    Clears the stream flag bits \a bits. Returns the previous stream
    flags.

    Equivalent to \c{flags( flags() & ~mask )}.

    \sa setf()
*/

/*!
    \fn int TQTextStream::width() const

    Returns the field width. The default value is 0.
*/

/*!
    \fn int TQTextStream::width( int w )

    \overload

    Sets the field width to \a w. Returns the previous field width.
*/

/*!
    \fn int TQTextStream::fill() const

    Returns the fill character. The default value is ' ' (space).
*/

/*!
    \overload int TQTextStream::fill( int f )

    Sets the fill character to \a f. Returns the previous fill character.
*/

/*!
    \fn int TQTextStream::precision() const

    Returns the precision. The default value is 6.
*/

/*!
    \fn int TQTextStream::precision( int p )

    \overload

    Sets the precision to \a p. Returns the previous precision setting.
*/


 /*****************************************************************************
  TQTextStream manipulators
 *****************************************************************************/

TQTextStream &bin( TQTextStream &s )
{
    s.setf(TQTS::bin,TQTS::basefield);
    return s;
}

TQTextStream &oct( TQTextStream &s )
{
    s.setf(TQTS::oct,TQTS::basefield);
    return s;
}

TQTextStream &dec( TQTextStream &s )
{
    s.setf(TQTS::dec,TQTS::basefield);
    return s;
}

TQTextStream &hex( TQTextStream &s )
{
    s.setf(TQTS::hex,TQTS::basefield);
    return s;
}

TQTextStream &endl( TQTextStream &s )
{
    return s << '\n';
}

TQTextStream &flush( TQTextStream &s )
{
    if ( s.device() )
	s.device()->flush();
    return s;
}

TQTextStream &ws( TQTextStream &s )
{
    s.skipWhiteSpace();
    return s;
}

TQTextStream &reset( TQTextStream &s )
{
    s.reset();
    return s;
}


/*!
    \class TQTextIStream ntqtextstream.h
    \reentrant
    \brief The TQTextIStream class is a convenience class for input streams.

    \ingroup io
    \ingroup text

    This class provides a shorthand for creating simple input
    \l{TQTextStream}s without having to pass a \e mode argument to the
    constructor.

    This class makes it easy, for example, to write things like this:
    \code
    TQString data = "123 456";
    int a, b;
    TQTextIStream(&data) >> a >> b;
    \endcode

    \sa TQTextOStream
*/

/*!
    \fn TQTextIStream::TQTextIStream( const TQString *s )

    Constructs a stream to read from the string \a s.
*/
/*!
    \fn TQTextIStream::TQTextIStream( TQByteArray ba )

    Constructs a stream to read from the array \a ba.
*/
/*!
    \fn TQTextIStream::TQTextIStream( FILE *f )

    Constructs a stream to read from the file \a f.
*/


/*!
    \class TQTextOStream
    \reentrant
    \brief The TQTextOStream class is a convenience class for output streams.

    \ingroup io
    \ingroup text

    This class provides a shorthand for creating simple output
    \l{TQTextStream}s without having to pass a \e mode argument to the
    constructor.

    This makes it easy for example, to write things like this:
    \code
    TQString result;
    TQTextOStream(&result) << "pi = " << 3.14;
    \endcode
*/

/*!
    \fn TQTextOStream::TQTextOStream( TQString *s )

    Constructs a stream to write to string \a s.
*/
/*!
    \fn TQTextOStream::TQTextOStream( TQByteArray ba )

    Constructs a stream to write to the array \a ba.
*/
/*!
    \fn TQTextOStream::TQTextOStream( FILE *f )

    Constructs a stream to write to the file \a f.
*/



/*!
  Sets the encoding of this stream to \a e, where \a e is one of the
  following values:
  \table
  \header \i Encoding \i Meaning
  \row \i Locale
       \i Uses local file format (Latin1 if locale is not set), but
       autodetecting Unicode(utf16) on input.
  \row \i Unicode
       \i Uses Unicode(utf16) for input and output. Output will be
       written in the order most efficient for the current platform
       (i.e. the order used internally in TQString).
  \row \i UnicodeUTF8
       \i Using Unicode(utf8) for input and output. If you use it for
       input it will autodetect utf16 and use it instead of utf8.
  \row \i Latin1
       \i ISO-8859-1. Will not autodetect utf16.
  \row \i UnicodeNetworkOrder
       \i Uses network order Unicode(utf16) for input and output.
       Useful when reading Unicode data that does not start with the
       byte order marker.
  \row \i UnicodeReverse
       \i Uses reverse network order Unicode(utf16) for input and
       output. Useful when reading Unicode data that does not start
       with the byte order marker or when writing data that should be
       read by buggy Windows applications.
  \row \i RawUnicode
       \i Like Unicode, but does not write the byte order marker nor
       does it auto-detect the byte order. Useful only when writing to
       non-persistent storage used by a single process.
  \endtable

  \c Locale and all Unicode encodings, except \c RawUnicode, will look
  at the first two bytes in an input stream to determine the byte
  order. The initial byte order marker will be stripped off before
  data is read.

  Note that this function should be called before any data is read to
  or written from the stream.

  \sa setCodec()
*/

void TQTextStream::setEncoding( Encoding e )
{
    if ( d->sourceType == TQTextStreamPrivate::String )
	return;

    switch ( e ) {
    case Unicode:
	mapper = 0;
	latin1 = false;
	doUnicodeHeader = true;
	internalOrder = true;
	networkOrder = TQChar::networkOrdered();
	break;
    case UnicodeUTF8:
#ifndef TQT_NO_TEXTCODEC
	mapper = TQTextCodec::codecForMib( 106 );
	latin1 = false;
	doUnicodeHeader = true;
	internalOrder = true;
	networkOrder = TQChar::networkOrdered();
#else
	mapper = 0;
	latin1 = true;
	doUnicodeHeader = true;
#endif
	break;
    case UnicodeNetworkOrder:
	mapper = 0;
	latin1 = false;
	doUnicodeHeader = true;
	internalOrder = TQChar::networkOrdered();
	networkOrder = true;
	break;
    case UnicodeReverse:
	mapper = 0;
	latin1 = false;
	doUnicodeHeader = true;
	internalOrder = !TQChar::networkOrdered();
	networkOrder = false;
	break;
    case RawUnicode:
	mapper = 0;
	latin1 = false;
	doUnicodeHeader = false;
	internalOrder = true;
	networkOrder = TQChar::networkOrdered();
	break;
    case Locale:
	latin1 = true; // fallback to Latin-1
#ifndef TQT_NO_TEXTCODEC
	mapper = TQTextCodec::codecForLocale();
	// optimized Latin-1 processing
#if defined(Q_OS_WIN32)
	if ( GetACP() == 1252 )
	    mapper = 0;
#endif
	if ( mapper && mapper->mibEnum() == 4 )
#endif
	    mapper = 0;

	doUnicodeHeader = true; // If it reads as Unicode, accept it
	break;
    case Latin1:
	mapper = 0;
	doUnicodeHeader = false;
	latin1 = true;
	break;
    }
}


#ifndef TQT_NO_TEXTCODEC
/*!
    Sets the codec for this stream to \a codec. Will not try to
    autodetect Unicode.

    Note that this function should be called before any data is read
    to/written from the stream.

    \sa setEncoding(), codec()
*/

void TQTextStream::setCodec( TQTextCodec *codec )
{
    if ( d->sourceType == TQTextStreamPrivate::String )
	return; // TQString does not need any codec
    mapper = codec;
    latin1 = ( codec->mibEnum() == 4 );
    if ( latin1 )
	mapper = 0;
    doUnicodeHeader = false;
}

/*!
  Returns the codec actually used for this stream.

  If Unicode is automatically detected in input, a codec with \link
  TQTextCodec::name() name() \endlink "ISO-10646-UCS-2" is returned.

  \sa setCodec()
*/

TQTextCodec *TQTextStream::codec()
{
    if ( mapper ) {
	return mapper;
    } else {
	// 4 is "ISO 8859-1", 1000 is "ISO-10646-UCS-2"
	return TQTextCodec::codecForMib( latin1 ? 4 : 1000 );
    }
}

#endif

#endif // TQT_NO_TEXTSTREAM
