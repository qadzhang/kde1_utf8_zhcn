/****************************************************************************
**
** Implementation of TQDataStream class
**
** Created : 930831
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

#include "ntqdatastream.h"

#ifndef TQT_NO_DATASTREAM
#include "ntqbuffer.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#ifndef Q_OS_TEMP
#include <locale.h>
#else
#include "qt_windows.h"
#endif

/*!
    \class TQDataStream ntqdatastream.h
    \reentrant
    \brief The TQDataStream class provides serialization of binary data
    to a TQIODevice.

    \ingroup io

    A data stream is a binary stream of encoded information which is
    100% independent of the host computer's operating system, CPU or
    byte order. For example, a data stream that is written by a PC
    under Windows can be read by a Sun SPARC running Solaris.

    You can also use a data stream to read/write \link #raw raw
    unencoded binary data\endlink. If you want a "parsing" input
    stream, see TQTextStream.

    The TQDataStream class implements the serialization of C++'s basic
    data types, like \c char, \c short, \c int, \c char*, etc.
    Serialization of more complex data is accomplished by breaking up
    the data into primitive units.

    A data stream cooperates closely with a TQIODevice. A TQIODevice
    represents an input/output medium one can read data from and write
    data to. The TQFile class is an example of an IO device.

    Example (write binary data to a stream):
    \code
    TQFile file( "file.dat" );
    file.open( IO_WriteOnly );
    TQDataStream stream( &file ); // we will serialize the data into the file
    stream << "the answer is";   // serialize a string
    stream << (TQ_INT32)42;       // serialize an integer
    \endcode

    Example (read binary data from a stream):
    \code
    TQFile file( "file.dat" );
    file.open( IO_ReadOnly );
    TQDataStream stream( &file );  // read the data serialized from the file
    TQString str;
    TQ_INT32 a;
    stream >> str >> a;           // extract "the answer is" and 42
    \endcode

    Each item written to the stream is written in a predefined binary
    format that varies depending on the item's type. Supported TQt
    types include TQBrush, TQColor, TQDateTime, TQFont, TQPixmap, TQString,
    TQVariant and many others. For the complete list of all TQt types
    supporting data streaming see the \link datastreamformat.html
    Format of the TQDataStream operators \endlink.

    For integers it is best to always cast to a TQt integer type for
    writing, and to read back into the same TQt integer type. This
    ensures that you get integers of the size you want and insulates
    you from compiler and platform differences.

    To take one example, a \c char* string is written as a 32-bit
    integer equal to the length of the string including the NUL byte
    ('\0'), followed by all the characters of the string including the
    NUL byte. When reading a \c char* string, 4 bytes are read to
    create the 32-bit length value, then that many characters for the
    \c char* string including the NUL are read.

    The initial IODevice is usually set in the constructor, but can be
    changed with setDevice(). If you've reached the end of the data
    (or if there is no IODevice set) atEnd() will return true.

    If you want the data to be compatible with an earlier version of
    TQt use setVersion().

    If you want the data to be human-readable, e.g. for debugging, you
    can set the data stream into printable data mode with
    setPrintableData(). The data is then written slower, in a bloated
    but human readable format.

    If you are producing a new binary data format, such as a file
    format for documents created by your application, you could use a
    TQDataStream to write the data in a portable format. Typically, you
    would write a brief header containing a magic string and a version
    number to give yourself room for future expansion. For example:

    \code
    TQFile file( "file.xxx" );
    file.open( IO_WriteOnly );
    TQDataStream stream( &file );

    // Write a header with a "magic number" and a version
    stream << (TQ_UINT32)0xA0B0C0D0;
    stream << (TQ_INT32)123;

    // Write the data
    stream << [lots of interesting data]
    \endcode

    Then read it in with:

    \code
    TQFile file( "file.xxx" );
    file.open( IO_ReadOnly );
    TQDataStream stream( &file );

    // Read and check the header
    TQ_UINT32 magic;
    stream >> magic;
    if ( magic != 0xA0B0C0D0 )
	return XXX_BAD_FILE_FORMAT;

    // Read the version
    TQ_INT32 version;
    stream >> version;
    if ( version < 100 )
	return XXX_BAD_FILE_TOO_OLD;
    if ( version > 123 )
	return XXX_BAD_FILE_TOO_NEW;
    if ( version <= 110 )
	stream.setVersion(1);

    // Read the data
    stream >> [lots of interesting data];
    if ( version > 120 )
	stream >> [data new in XXX version 1.2];
    stream >> [other interesting data];
    \endcode

    You can select which byte order to use when serializing data. The
    default setting is big endian (MSB first). Changing it to little
    endian breaks the portability (unless the reader also changes to
    little endian). We recommend keeping this setting unless you have
    special requirements.

    \target raw
    \section1 Reading and writing raw binary data

    You may wish to read/write your own raw binary data to/from the
    data stream directly. Data may be read from the stream into a
    preallocated char* using readRawBytes(). Similarly data can be
    written to the stream using writeRawBytes(). Notice that any
    encoding/decoding of the data must be done by you.

    A similar pair of functions is readBytes() and writeBytes(). These
    differ from their \e raw counterparts as follows: readBytes()
    reads a TQ_UINT32 which is taken to be the length of the data to be
    read, then that number of bytes is read into the preallocated
    char*; writeBytes() writes a TQ_UINT32 containing the length of the
    data, followed by the data. Notice that any encoding/decoding of
    the data (apart from the length TQ_UINT32) must be done by you.

    \sa TQTextStream TQVariant
*/

/*!
    \enum TQDataStream::ByteOrder

    The byte order used for reading/writing the data.

    \value BigEndian the default
    \value LittleEndian
*/


/*****************************************************************************
  TQDataStream member functions
 *****************************************************************************/

#if defined(QT_CHECK_STATE)
#undef  CHECK_STREAM_PRECOND
#define CHECK_STREAM_PRECOND  if ( !dev ) {				\
				tqWarning( "TQDataStream: No device" );	\
				return *this; }
#else
#define CHECK_STREAM_PRECOND
#endif

static int  systemWordSize = 0;
static bool systemBigEndian;

static const int DefaultStreamVersion = 6;
// ### On next version bump, TQPen::width() should not be restricted to 8-bit values.
// ### On next version bump, when streaming invalid TQVariants, just the type should
// be written, no "data" after it
// 6 is default in TQt 3.3
// 5 is default in TQt 3.1
// 4 is default in TQt 3.0
// 3 is default in TQt 2.1
// 2 is the TQt 2.0.x format
// 1 is the TQt 1.x format

/*!
    Constructs a data stream that has no IO device.

    \sa setDevice()
*/

TQDataStream::TQDataStream()
{
    if ( systemWordSize == 0 )			// get system features
	tqSysInfo( &systemWordSize, &systemBigEndian );
    dev	      = 0;				// no device set
    owndev    = false;
    byteorder = BigEndian;			// default byte order
    printable = false;
    ver	      = DefaultStreamVersion;
    noswap    = systemBigEndian;
}

/*!
    Constructs a data stream that uses the IO device \a d.

    \warning If you use TQSocket or TQSocketDevice as the IO device \a d
    for reading data, you must make sure that enough data is available
    on the socket for the operation to successfully proceed;
    TQDataStream does not have any means to handle or recover from
    short-reads.

    \sa setDevice(), device()
*/

TQDataStream::TQDataStream( TQIODevice *d )
{
    if ( systemWordSize == 0 )			// get system features
	tqSysInfo( &systemWordSize, &systemBigEndian );
    dev	      = d;				// set device
    owndev    = false;
    byteorder = BigEndian;			// default byte order
    printable = false;
    ver	      = DefaultStreamVersion;
    noswap    = systemBigEndian;
}

/*!
    Constructs a data stream that operates on a byte array, \a a,
    through an internal TQBuffer device. The \a mode is a
    TQIODevice::mode(), usually either \c IO_ReadOnly or \c
    IO_WriteOnly.

    Example:
    \code
    static char bindata[] = { 231, 1, 44, ... };
    TQByteArray a;
    a.setRawData( bindata, sizeof(bindata) );	// a points to bindata
    TQDataStream stream( a, IO_ReadOnly );	// open on a's data
    stream >> [something];			// read raw bindata
    a.resetRawData( bindata, sizeof(bindata) ); // finished
    \endcode

    The TQByteArray::setRawData() function is not for the inexperienced.
*/

TQDataStream::TQDataStream( TQByteArray a, int mode )
{
    if ( systemWordSize == 0 )			// get system features
	tqSysInfo( &systemWordSize, &systemBigEndian );
    dev	      = new TQBuffer( a );		// create device
    ((TQBuffer *)dev)->open( mode );		// open device
    owndev    = true;
    byteorder = BigEndian;			// default byte order
    printable = false;
    ver	      = DefaultStreamVersion;
    noswap    = systemBigEndian;
}

/*!
    Destroys the data stream.

    The destructor will not affect the current IO device, unless it is
    an internal IO device processing a TQByteArray passed in the \e
    constructor, in which case the internal IO device is destroyed.
*/

TQDataStream::~TQDataStream()
{
    if ( owndev )
	delete dev;
}


/*!
    \fn TQIODevice *TQDataStream::device() const

    Returns the IO device currently set.

    \sa setDevice(), unsetDevice()
*/

/*!
    void TQDataStream::setDevice(TQIODevice *d )

    Sets the IO device to \a d.

    \sa device(), unsetDevice()
*/

void TQDataStream::setDevice(TQIODevice *d )
{
    if ( owndev ) {
	delete dev;
	owndev = false;
    }
    dev = d;
}

/*!
    Unsets the IO device. This is the same as calling setDevice( 0 ).

    \sa device(), setDevice()
*/

void TQDataStream::unsetDevice()
{
    setDevice( 0 );
}


/*!
    \fn bool TQDataStream::atEnd() const

    Returns true if the IO device has reached the end position (end of
    the stream or file) or if there is no IO device set; otherwise
    returns false, i.e. if the current position of the IO device is
    before the end position.

    \sa TQIODevice::atEnd()
*/

/*!\fn bool TQDataStream::eof() const

  \obsolete

  Returns true if the IO device has reached the end position (end of
  stream or file) or if there is no IO device set.

  Returns false if the current position of the read/write head of the IO
  device is somewhere before the end position.

  \sa TQIODevice::atEnd()
*/

/*!
    \fn int TQDataStream::byteOrder() const

    Returns the current byte order setting -- either \c BigEndian or
    \c LittleEndian.

    \sa setByteOrder()
*/

/*!
    Sets the serialization byte order to \a bo.

    The \a bo parameter can be \c TQDataStream::BigEndian or \c
    TQDataStream::LittleEndian.

    The default setting is big endian. We recommend leaving this
    setting unless you have special requirements.

    \sa byteOrder()
*/

void TQDataStream::setByteOrder( int bo )
{
    byteorder = bo;
    if ( systemBigEndian )
	noswap = byteorder == BigEndian;
    else
	noswap = byteorder == LittleEndian;
}


/*!
    \fn bool TQDataStream::isPrintableData() const

    Returns true if the printable data flag has been set; otherwise
    returns false.

    \sa setPrintableData()
*/

/*!
    \fn void TQDataStream::setPrintableData( bool enable )

    If \a enable is true, data will be output in a human readable
    format. If \a enable is false, data will be output in a binary
    format.

    If \a enable is true, the write functions will generate output
    that consists of printable characters (7 bit ASCII). This output
    will typically be a lot larger than the default binary output, and
    consequently slower to write.

    We recommend only enabling printable data for debugging purposes.
*/


/*!
    \fn int TQDataStream::version() const

    Returns the version number of the data serialization format. In TQt
    3.1, this number is 5.

    \sa setVersion()
*/

/*!
    \fn void TQDataStream::setVersion( int v )

    Sets the version number of the data serialization format to \a v.

    You don't need to set a version if you are using the current
    version of TQt.

    In order to accommodate new functionality, the datastream
    serialization format of some TQt classes has changed in some
    versions of TQt. If you want to read data that was created by an
    earlier version of TQt, or write data that can be read by a program
    that was compiled with an earlier version of TQt, use this function
    to modify the serialization format of TQDataStream.

    \table
    \header \i TQt Version         \i TQDataStream Version
    \row \i TQt 3.3                \i11 6
    \row \i TQt 3.2                \i11 5
    \row \i TQt 3.1                \i11 5
    \row \i TQt 3.0                \i11 4
    \row \i TQt 2.1.x and TQt 2.2.x \i11 3
    \row \i TQt 2.0.x              \i11 2
    \row \i TQt 1.x                \i11 1
    \endtable

    \sa version()
*/

/*****************************************************************************
  TQDataStream read functions
 *****************************************************************************/

static TQ_INT64 read_int_ascii( TQDataStream *s )
{
    int n = 0;
    char buf[40];
    for ( ;; ) {
	buf[n] = s->device()->getch();
	if ( buf[n] == '\n' || n > 38 )		// $-terminator
	    break;
	n++;
    }
    buf[n] = '\0';

#if defined(__LP64__)
    // sizeof(long) == 8
    return strtol(buf, (char **)0, 10);
#else
#  if defined(Q_OS_TEMP)
    return strtol( buf, (char**)0, 10 );
#  elif defined(Q_OS_WIN)
    return _atoi64( buf );
#  elif defined(Q_OS_MACX) && defined(QT_MACOSX_VERSION) && QT_MACOSX_VERSION < 0x1020
    return strtoq( buf, (char**)0, 10 );
#  else
    return strtoll( buf, (char**)0, 10 );	// C99 function
#  endif
#endif
}

/*!
    \overload TQDataStream &TQDataStream::operator>>( TQ_UINT8 &i )

    Reads an unsigned byte from the stream into \a i, and returns a
    reference to the stream.
*/

/*!
    Reads a signed byte from the stream into \a i, and returns a
    reference to the stream.
*/

TQDataStream &TQDataStream::operator>>( TQ_INT8 &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = (TQ_INT8)dev->getch();
	if ( i == '\\' ) {			// read octal code
	    char buf[4];
	    dev->readBlock( buf, 3 );
	    i = (buf[2] & 0x07)+((buf[1] & 0x07) << 3)+((buf[0] & 0x07) << 6);
	}
    } else {					// data or text
	int ret = dev->getch();
	if (ret >= 0) {
		i = (TQ_INT8)ret;
	}
    }
    return *this;
}


/*!
    \overload TQDataStream &TQDataStream::operator>>( TQ_UINT16 &i )

    Reads an unsigned 16-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

/*!
    \overload

    Reads a signed 16-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

TQDataStream &TQDataStream::operator>>( TQ_INT16 &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = (TQ_INT16)read_int_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&i, sizeof(TQ_INT16) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&i);
	char b[2];
	if (dev->readBlock( b, 2 ) >= 2) {
		*p++ = b[1];
		*p   = b[0];
	}
    }
    return *this;
}


/*!
    \overload TQDataStream &TQDataStream::operator>>( TQ_UINT32 &i )

    Reads an unsigned 32-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

/*!
    \overload

    Reads a signed 32-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

TQDataStream &TQDataStream::operator>>( TQ_INT32 &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = (TQ_INT32)read_int_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&i, sizeof(TQ_INT32) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&i);
	char b[4];
	if (dev->readBlock( b, 4 ) >= 4) {
		*p++ = b[3];
		*p++ = b[2];
		*p++ = b[1];
		*p   = b[0];
	}
    }
    return *this;
}

/*!
    \overload TQDataStream &TQDataStream::operator>>( TQ_UINT64 &i )

    Reads an unsigned 64-bit integer from the stream, into \a i, and
    returns a reference to the stream.
*/

/*!
    \overload

    Reads a signed 64-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

TQDataStream &TQDataStream::operator>>( TQ_INT64 &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = read_int_ascii( this );
    } else if ( version() < 6 ) {
	TQ_UINT32 i1, i2;
	*this >> i2 >> i1;
	i = ((TQ_UINT64)i1 << 32) + i2;
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&i, sizeof(TQ_INT64) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&i);
	char b[8];
	if (dev->readBlock( b, 8 ) >= 8) {
		*p++ = b[7];
		*p++ = b[6];
		*p++ = b[5];
		*p++ = b[4];
		*p++ = b[3];
		*p++ = b[2];
		*p++ = b[1];
		*p   = b[0];
	}
    }
    return *this;
}


/*!
    \overload TQDataStream &TQDataStream::operator>>( TQ_ULONG &i )

    Reads an unsigned integer of the system's word length from the
    stream, into \a i, and returns a reference to the stream.
*/

#if !defined(Q_OS_WIN64)
/*!
    \overload

    Reads a signed integer of the system's word length from the stream
    into \a i, and returns a reference to the stream.

*/

TQDataStream &TQDataStream::operator>>( TQ_LONG &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = (TQ_LONG)read_int_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&i, sizeof(TQ_LONG) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&i);
	char b[sizeof(TQ_LONG)];
	if (dev->readBlock( b, sizeof(TQ_LONG) ) >= (int)sizeof(TQ_LONG)) {
		for ( int j = sizeof(TQ_LONG); j;  ) {
			*p++ = b[--j];
		}
	}
    }
    return *this;
}
#endif

static double read_double_ascii( TQDataStream *s )
{
    int n = 0;
    char buf[80];
    for ( ;; ) {
	buf[n] = s->device()->getch();
	if ( buf[n] == '\n' || n > 78 )		// $-terminator
	    break;
	n++;
    }
    buf[n] = '\0';
    return atof( buf );
}


/*!
    \overload

    Reads a 32-bit floating point number from the stream into \a f,
    using the standard IEEE754 format. Returns a reference to the
    stream.
*/

TQDataStream &TQDataStream::operator>>( float &f )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	f = (float)read_double_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&f, sizeof(float) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&f);
	char b[4];
	if (dev->readBlock( b, 4 ) >= 4) {
		*p++ = b[3];
		*p++ = b[2];
		*p++ = b[1];
		*p   = b[0];
	}
    }
    return *this;
}


/*!
    \overload

    Reads a 64-bit floating point number from the stream into \a f,
    using the standard IEEE754 format. Returns a reference to the
    stream.
*/

TQDataStream &TQDataStream::operator>>( double &f )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	f = read_double_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&f, sizeof(double) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&f);
	char b[8];
	if (dev->readBlock( b, 8 ) >= 8) {
		*p++ = b[7];
		*p++ = b[6];
		*p++ = b[5];
		*p++ = b[4];
		*p++ = b[3];
		*p++ = b[2];
		*p++ = b[1];
		*p   = b[0];
	}
    }
    return *this;
}


/*!
    \overload

    Reads the '\0'-terminated string \a s from the stream and returns
    a reference to the stream.

    Space for the string is allocated using \c new -- the caller must
    destroy it with delete[].
*/

TQDataStream &TQDataStream::operator>>( char *&s )
{
    uint len = 0;
    return readBytes( s, len );
}


/*!
    Reads the buffer \a s from the stream and returns a reference to
    the stream.

    The buffer \a s is allocated using \c new. Destroy it with the \c
    delete[] operator. If the length is zero or \a s cannot be
    allocated, \a s is set to 0.

    The \a l parameter will be set to the length of the buffer.

    The serialization format is a TQ_UINT32 length specifier first,
    then \a l bytes of data. Note that the data is \e not encoded.

    \sa readRawBytes(), writeBytes()
*/

TQDataStream &TQDataStream::readBytes( char *&s, uint &l )
{
    CHECK_STREAM_PRECOND
    TQ_UINT32 len;
    *this >> len;				// first read length spec
    l = (uint)len;
    if ( len == 0 || eof() ) {
	s = 0;
	return *this;
    } else {
	s = new char[len];			// create char array
	TQ_CHECK_PTR( s );
	if ( !s )				// no memory
	    return *this;
	return readRawBytes( s, (uint)len );
    }
}


/*!
    Reads \a len bytes from the stream into \a s and returns a
    reference to the stream.

    The buffer \a s must be preallocated. The data is \e not encoded.

    \sa readBytes(), TQIODevice::readBlock(), writeRawBytes()
*/

TQDataStream &TQDataStream::readRawBytes( char *s, uint len )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	TQ_INT8 *p = (TQ_INT8*)s;
	if ( version() < 4 ) {
	    while ( len-- ) {
		TQ_INT32 tmp;
		*this >> tmp;
		*p++ = tmp;
	    }
	} else {
	    while ( len-- )
		*this >> *p++;
	}
    } else {					// read data char array
	dev->readBlock( s, len );
    }
    return *this;
}


/*****************************************************************************
  TQDataStream write functions
 *****************************************************************************/


/*!
    \overload TQDataStream &TQDataStream::operator<<( TQ_UINT8 i )

    Writes an unsigned byte, \a i, to the stream and returns a
    reference to the stream.
*/

/*!
    Writes a signed byte, \a i, to the stream and returns a reference
    to the stream.
*/

TQDataStream &TQDataStream::operator<<( TQ_INT8 i )
{
    CHECK_STREAM_PRECOND
    if ( printable && (i == '\\' || !isprint((uchar) i)) ) {
	char buf[6];				// write octal code
	buf[0] = '\\';
	buf[1] = '0' + ((i >> 6) & 0x07);
	buf[2] = '0' + ((i >> 3) & 0x07);
	buf[3] = '0' + (i & 0x07);
	buf[4] = '\0';
	dev->writeBlock( buf, 4 );
    } else {
	dev->putch( i );
    }
    return *this;
}


/*!
    \overload TQDataStream &TQDataStream::operator<<( TQ_UINT16 i )

    Writes an unsigned 16-bit integer, \a i, to the stream and returns
    a reference to the stream.
*/

/*!
    \overload

    Writes a signed 16-bit integer, \a i, to the stream and returns a
    reference to the stream.
*/

TQDataStream &TQDataStream::operator<<( TQ_INT16 i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[16];
	sprintf( buf, "%d\n", i );
	dev->writeBlock( buf, strlen(buf) );
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&i, sizeof(TQ_INT16) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&i);
	char b[2];
	b[1] = *p++;
	b[0] = *p;
	dev->writeBlock( b, 2 );
    }
    return *this;
}

/*!
    \overload

    Writes a signed 32-bit integer, \a i, to the stream and returns a
    reference to the stream.
*/

TQDataStream &TQDataStream::operator<<( TQ_INT32 i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[16];
	sprintf( buf, "%d\n", i );
	dev->writeBlock( buf, strlen(buf) );
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&i, sizeof(TQ_INT32) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&i);
	char b[4];
	b[3] = *p++;
	b[2] = *p++;
	b[1] = *p++;
	b[0] = *p;
	dev->writeBlock( b, 4 );
    }
    return *this;
}

/*!
    \overload TQDataStream &TQDataStream::operator<<( TQ_UINT64 i )

    Writes an unsigned 64-bit integer, \a i, to the stream and returns a
    reference to the stream.
*/

/*!
    \overload

    Writes a signed 64-bit integer, \a i, to the stream and returns a
    reference to the stream.
*/

TQDataStream &TQDataStream::operator<<( TQ_INT64 i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[16];
#ifdef Q_OS_WIN
	sprintf( buf, "%I64d\n", i );
#else
	sprintf( buf, "%lld\n", i );
#endif
	dev->writeBlock( buf, strlen(buf) );
    } else if ( version() < 6 ) {
	TQ_UINT32 i1 = i & 0xffffffff;
	TQ_UINT32 i2 = i >> 32;
	*this << i2 << i1;
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&i, sizeof(TQ_INT64) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&i);
	char b[8];
	b[7] = *p++;
	b[6] = *p++;
	b[5] = *p++;
	b[4] = *p++;
	b[3] = *p++;
	b[2] = *p++;
	b[1] = *p++;
	b[0] = *p;
	dev->writeBlock( b, 8 );
    }
    return *this;
}

/*!
    \overload TQDataStream &TQDataStream::operator<<( TQ_ULONG i )

    Writes an unsigned integer \a i, of the system's word length, to
    the stream and returns a reference to the stream.
*/

#if !defined(Q_OS_WIN64)
/*!
    \overload

    Writes a signed integer \a i, of the system's word length, to the
    stream and returns a reference to the stream.
*/

TQDataStream &TQDataStream::operator<<( TQ_LONG i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[20];
	sprintf( buf, "%ld\n", i );
	dev->writeBlock( buf, strlen(buf) );
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&i, sizeof(TQ_LONG) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&i);
	char b[sizeof(TQ_LONG)];
	for ( int j = sizeof(TQ_LONG); j;  )
	    b[--j] = *p++;
	dev->writeBlock( b, sizeof(TQ_LONG) );
    }
    return *this;
}
#endif


/*!
    \overload TQDataStream &TQDataStream::operator<<( TQ_UINT32 i )

    Writes an unsigned integer, \a i, to the stream as a 32-bit
    unsigned integer (TQ_UINT32). Returns a reference to the stream.
*/

/*!
    \overload

    Writes a 32-bit floating point number, \a f, to the stream using
    the standard IEEE754 format. Returns a reference to the stream.
*/

TQDataStream &TQDataStream::operator<<( float f )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	TQString num = TQString::number((double)f);
	dev->writeBlock(num.latin1(), num.length());
	dev->putch('\n');
    } else {
	float g = f;				// fixes float-on-stack problem
	if ( noswap ) {				// no conversion needed
	    dev->writeBlock( (char *)&g, sizeof(float) );
	} else {				// swap bytes
	    uchar *p = (uchar *)(&g);
	    char b[4];
	    b[3] = *p++;
	    b[2] = *p++;
	    b[1] = *p++;
	    b[0] = *p;
	    dev->writeBlock( b, 4 );
	}
    }
    return *this;
}


/*!
    \overload

    Writes a 64-bit floating point number, \a f, to the stream using
    the standard IEEE754 format. Returns a reference to the stream.
*/

TQDataStream &TQDataStream::operator<<( double f )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	TQString num = TQString::number((double)f);
	dev->writeBlock(num.latin1(), num.length());
	dev->putch('\n');
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&f, sizeof(double) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&f);
	char b[8];
	b[7] = *p++;
	b[6] = *p++;
	b[5] = *p++;
	b[4] = *p++;
	b[3] = *p++;
	b[2] = *p++;
	b[1] = *p++;
	b[0] = *p;
	dev->writeBlock( b, 8 );
    }
    return *this;
}


/*!
    \overload

    Writes the '\0'-terminated string \a s to the stream and returns a
    reference to the stream.

    The string is serialized using writeBytes().
*/

TQDataStream &TQDataStream::operator<<( const char *s )
{
    if ( !s ) {
	*this << (TQ_UINT32)0;
	return *this;
    }
    uint len = tqstrlen( s ) + 1;			// also write null terminator
    *this << (TQ_UINT32)len;			// write length specifier
    return writeRawBytes( s, len );
}


/*!
    Writes the length specifier \a len and the buffer \a s to the
    stream and returns a reference to the stream.

    The \a len is serialized as a TQ_UINT32, followed by \a len bytes
    from \a s. Note that the data is \e not encoded.

    \sa writeRawBytes(), readBytes()
*/

TQDataStream &TQDataStream::writeBytes(const char *s, uint len)
{
    CHECK_STREAM_PRECOND
    *this << (TQ_UINT32)len;			// write length specifier
    if ( len )
	writeRawBytes( s, len );
    return *this;
}


/*!
    Writes \a len bytes from \a s to the stream and returns a
    reference to the stream. The data is \e not encoded.

    \sa writeBytes(), TQIODevice::writeBlock(), readRawBytes()
*/

TQDataStream &TQDataStream::writeRawBytes( const char *s, uint len )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// write printable
	if ( version() < 4 ) {
	    char *p = (char *)s;
	    while ( len-- )
		*this << *p++;
	} else {
	    TQ_INT8 *p = (TQ_INT8*)s;
	    while ( len-- )
		*this << *p++;
	}
    } else {					// write data char array
	dev->writeBlock( s, len );
    }
    return *this;
}

#endif // TQT_NO_DATASTREAM
