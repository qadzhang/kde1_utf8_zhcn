/****************************************************************************
**
** Implementation of TQBuffer class
**
** Created : 930812
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

#include "ntqbuffer.h"
#include <stdlib.h>

/*!
    \class TQBuffer ntqbuffer.h
    \reentrant
    \brief The TQBuffer class is an I/O device that operates on a TQByteArray.

    \ingroup io
    \ingroup collection

    TQBuffer is used to read and write to a memory buffer. It is
    normally used with a TQTextStream or a TQDataStream. TQBuffer has an
    associated TQByteArray which holds the buffer data. The size() of
    the buffer is automatically adjusted as data is written.

    The constructor \c TQBuffer(TQByteArray) creates a TQBuffer using an
    existing byte array. The byte array can also be set with
    setBuffer(). Writing to the TQBuffer will modify the original byte
    array because TQByteArray is \link shclass.html explicitly
    shared.\endlink

    Use open() to open the buffer before use and to set the mode
    (read-only, write-only, etc.). close() closes the buffer. The
    buffer must be closed before reopening or calling setBuffer().

    A common way to use TQBuffer is through \l TQDataStream or \l
    TQTextStream, which have constructors that take a TQBuffer
    parameter. For convenience, there are also TQDataStream and
    TQTextStream constructors that take a TQByteArray parameter. These
    constructors create and open an internal TQBuffer.

    Note that TQTextStream can also operate on a TQString (a Unicode
    string); a TQBuffer cannot.

    You can also use TQBuffer directly through the standard TQIODevice
    functions readBlock(), writeBlock() readLine(), at(), getch(),
    putch() and ungetch().

    \sa TQFile, TQDataStream, TQTextStream, TQByteArray, \link shclass.html Shared Classes\endlink
*/


/*!
    Constructs an empty buffer.
*/

TQBuffer::TQBuffer()
{
    setFlags( IO_Direct );
    a_inc = 16;                                 // initial increment
    a_len = 0;
    ioIndex = 0;
}


/*!
    Constructs a buffer that operates on \a buf.

    If you open the buffer in write mode (\c IO_WriteOnly or
    \c IO_ReadWrite) and write something into the buffer, \a buf
    will be modified.

    Example:
    \code
    TQCString str = "abc";
    TQBuffer b( str );
    b.open( IO_WriteOnly );
    b.at( 3 ); // position at the 4th character (the terminating \0)
    b.writeBlock( "def", 4 ); // write "def" including the terminating \0
    b.close();
    // Now, str == "abcdef" with a terminating \0
    \endcode

    \sa setBuffer()
*/

TQBuffer::TQBuffer( TQByteArray buf ) : a(buf)
{
    setFlags( IO_Direct );
    a_len = a.size();
    a_inc = (a_len > 512) ? 512 : a_len;        // initial increment
    if ( a_inc < 16 )
        a_inc = 16;
    ioIndex = 0;
}

/*!
    Destroys the buffer.
*/

TQBuffer::~TQBuffer()
{
}


/*!
    Replaces the buffer's contents with \a buf and returns true.

    Does nothing (and returns false) if isOpen() is true.

    Note that if you open the buffer in write mode (\c IO_WriteOnly or
    IO_ReadWrite) and write something into the buffer, \a buf is also
    modified because TQByteArray is an explicitly shared class.

    \sa buffer(), open(), close()
*/

bool TQBuffer::setBuffer( TQByteArray buf )
{
    if ( isOpen() ) {
#if defined(QT_CHECK_STATE)
        tqWarning( "TQBuffer::setBuffer: Buffer is open" );
#endif
        return false;
    }
    a = buf;
    a_len = a.size();
    a_inc = (a_len > 512) ? 512 : a_len;        // initial increment
    if ( a_inc < 16 )
        a_inc = 16;
    ioIndex = 0;
    return true;
}

/*!
    \fn TQByteArray TQBuffer::buffer() const

    Returns this buffer's byte array.

    \sa setBuffer()
*/

/*!
    \reimp

    Opens the buffer in mode \a m. Returns true if successful;
    otherwise returns false. The buffer must be opened before use.

    The mode parameter \a m must be a combination of the following flags.
    \list
    \i \c IO_ReadOnly opens the buffer in read-only mode.
    \i \c IO_WriteOnly opens the buffer in write-only mode.
    \i \c IO_ReadWrite opens the buffer in read/write mode.
    \i \c IO_Append sets the buffer index to the end of the buffer.
    \i \c IO_Truncate truncates the buffer.
    \endlist

    \sa close(), isOpen()
*/

bool TQBuffer::open( int m  )
{
    if ( isOpen() ) {                           // buffer already open
#if defined(QT_CHECK_STATE)
        tqWarning( "TQBuffer::open: Buffer already open" );
#endif
        return false;
    }
    setMode( m );
    if ( m & IO_Truncate ) {                    // truncate buffer
        a.resize( 0 );
        a_len = 0;
    }
    if ( m & IO_Append ) {                      // append to end of buffer
        ioIndex = a.size();
    } else {
        ioIndex = 0;
    }
    a_inc = 16;
    setState( IO_Open );
    resetStatus();
    return true;
}

/*!
    \reimp

    Closes an open buffer.

    \sa open()
*/

void TQBuffer::close()
{
    if ( isOpen() ) {
        setFlags( IO_Direct );
        ioIndex = 0;
        a_inc = 16;
    }
}

/*!
    \reimp

    The flush function does nothing for a TQBuffer.
*/

void TQBuffer::flush()
{
    return;
}


/*!
    \fn TQIODevice::Offset TQBuffer::at() const

    \reimp
*/

/*!
    \fn TQIODevice::Offset TQBuffer::size() const

    \reimp
*/

/*!
  \reimp
*/

bool TQBuffer::at( Offset pos )
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {
        tqWarning( "TQBuffer::at: Buffer is not open" );
        return false;
    }
#endif
    if ( pos > a_len ) {
#if defined(QT_CHECK_RANGE)
#if defined(QT_ABI_QT4)
        tqWarning( "TQBuffer::at: Index %lld out of range", pos );
#else
        tqWarning( "TQBuffer::at: Index %lu out of range", pos );
#endif
#endif
        return false;
    }
    ioIndex = pos;
    return true;
}


/*!
  \reimp
*/

TQ_LONG TQBuffer::readBlock( char *p, TQ_ULONG len )
{
#if defined(QT_CHECK_STATE)
    if ( !p ) {
	tqWarning( "TQBuffer::readBlock: Null pointer error" );
	return -1;
    }
    if ( !isOpen() ) {                          // buffer not open
        tqWarning( "TQBuffer::readBlock: Buffer not open" );
        return -1;
    }
    if ( !isReadable() ) {                      // reading not permitted
        tqWarning( "TQBuffer::readBlock: Read operation not permitted" );
        return -1;
    }
#endif
    if ( ioIndex + len > a.size() ) {   // overflow
        if ( ioIndex >= a.size() ) {
            return 0;
        } else {
            len = a.size() - ioIndex;
        }
    }
    memcpy(p, a.data() + ioIndex, len);
    ioIndex += len;
    return len;
}

/*!
    \overload TQ_LONG TQBuffer::writeBlock( const TQByteArray& data )

    This convenience function is the same as calling
    \c{writeBlock( data.data(), data.size() )} with \a data.
*/

/*!
    Writes \a len bytes from \a p into the buffer at the current
    index position, overwriting any characters there and extending the
    buffer if necessary. Returns the number of bytes actually written.

    Returns -1 if an error occurred.

    \sa readBlock()
*/

TQ_LONG TQBuffer::writeBlock( const char *p, TQ_ULONG len )
{
    if ( len == 0 )
        return 0;

#if defined(QT_CHECK_NULL)
    if ( p == 0 ) {
        tqWarning( "TQBuffer::writeBlock: Null pointer error" );
        return -1;
    }
#endif
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
        tqWarning( "TQBuffer::writeBlock: Buffer not open" );
        return -1;
    }
    if ( !isWritable() ) {                      // writing not permitted
        tqWarning( "TQBuffer::writeBlock: Write operation not permitted" );
        return -1;
    }
#endif
    if ( ioIndex + len > a_len ) {              // overflow
        TQ_ULONG new_len = a_len + a_inc*((ioIndex+len-a_len)/a_inc+1);
        if ( !a.resize( new_len ) ) {           // could not resize
#if defined(QT_CHECK_NULL)
            tqWarning( "TQBuffer::writeBlock: Memory allocation error" );
#endif
            setStatus( IO_ResourceError );
            return -1;
        }
        a_inc *= 2;                             // double increment
        a_len = new_len;
        a.shd->len = ioIndex + len;
    }
    memcpy( a.data()+ioIndex, p, len );
    ioIndex += len;
    if ( a.shd->len < ioIndex )
        a.shd->len = ioIndex;                   // fake (not alloc'd) length
    return len;
}


/*!
  \reimp
*/

TQ_LONG TQBuffer::readLine( char *p, TQ_ULONG maxlen )
{
#if defined(QT_CHECK_NULL)
    if ( p == 0 ) {
        tqWarning( "TQBuffer::readLine: Null pointer error" );
        return -1;
    }
#endif
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
        tqWarning( "TQBuffer::readLine: Buffer not open" );
        return -1;
    }
    if ( !isReadable() ) {                      // reading not permitted
        tqWarning( "TQBuffer::readLine: Read operation not permitted" );
        return -1;
    }
#endif
    if ( maxlen == 0 )
        return 0;
    TQ_ULONG start = ioIndex;
    char *d = a.data() + ioIndex;
    maxlen--;                                   // make room for 0-terminator
    if ( a.size() - ioIndex < maxlen )
        maxlen = a.size() - ioIndex;
    while ( maxlen-- ) {
        if ( (*p++ = *d++) == '\n' )
            break;
    }
    *p = '\0';
    ioIndex = d - a.data();
    return ioIndex - start;
}


/*!
  \reimp
*/

int TQBuffer::getch()
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
        tqWarning( "TQBuffer::getch: Buffer not open" );
        return -1;
    }
    if ( !isReadable() ) {                      // reading not permitted
        tqWarning( "TQBuffer::getch: Read operation not permitted" );
        return -1;
    }
#endif
    if ( ioIndex+1 > a.size() ) {               // overflow
        setStatus( IO_ReadError );
        return -1;
    }
    return uchar(*(a.data()+ioIndex++));
}

/*!
    \reimp

    Writes the character \a ch into the buffer at the current index
    position, overwriting any existing character and extending the
    buffer if necessary.

    Returns \a ch, or -1 if an error occurred.

    \sa getch(), ungetch()
*/

int TQBuffer::putch( int ch )
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
        tqWarning( "TQBuffer::putch: Buffer not open" );
        return -1;
    }
    if ( !isWritable() ) {                      // writing not permitted
        tqWarning( "TQBuffer::putch: Write operation not permitted" );
        return -1;
    }
#endif
    if ( ioIndex + 1 > a_len ) {                // overflow
        char buf[1];
        buf[0] = (char)ch;
        if ( writeBlock(buf,1) != 1 )
            return -1;                          // write error
    } else {
        *(a.data() + ioIndex++) = (char)ch;
        if ( a.shd->len < ioIndex )
            a.shd->len = ioIndex;
    }
    return ch;
}

/*!
  \reimp
*/

int TQBuffer::ungetch( int ch )
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
        tqWarning( "TQBuffer::ungetch: Buffer not open" );
        return -1;
    }
    if ( !isReadable() ) {                      // reading not permitted
        tqWarning( "TQBuffer::ungetch: Read operation not permitted" );
        return -1;
    }
#endif
    if ( ch != -1 ) {
        if ( ioIndex )
            ioIndex--;
        else
            ch = -1;
    }
    return ch;
}

