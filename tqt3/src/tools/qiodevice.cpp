/****************************************************************************
**
** Implementation of TQIODevice class
**
** Created : 940913
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

#include "ntqiodevice.h"

/*!
    \class TQIODevice ntqiodevice.h
    \reentrant

    \brief The TQIODevice class is the base class of I/O devices.

    \ingroup io

    An I/O device represents a medium that one can read bytes from
    and/or write bytes to. The TQIODevice class is the abstract
    superclass of all such devices; classes such as TQFile, TQBuffer and
    TQSocket inherit TQIODevice and implement virtual functions such as
    write() appropriately.

    Although applications sometimes use TQIODevice directly, it is
    usually better to use TQTextStream and TQDataStream, which provide
    stream operations on any TQIODevice subclass. TQTextStream provides
    text-oriented stream functionality (for human-readable ASCII
    files, for example), whereas TQDataStream deals with binary data in
    a totally platform-independent manner.

    The public member functions in TQIODevice roughly fall into two
    groups: the action functions and the state access functions. The
    most important action functions are:

    \list

    \i  open() opens a device for reading and/or writing, depending on
    the mode argument.

    \i  close() closes the device and tidies up (e.g. flushes buffered
    data)

    \i  readBlock() reads a block of data from the device.

    \i  writeBlock() writes a block of data to the device.

    \i  readLine() reads a line (of text, usually) from the device.

    \i  flush() ensures that all buffered data are written to the real device.

    \endlist

    There are also some other, less used, action functions:

    \list

    \i  getch() reads a single character.

    \i  ungetch() forgets the last call to getch(), if possible.

    \i  putch() writes a single character.

    \i  size() returns the size of the device, if there is one.

    \i  at() returns the current read/write pointer's position, if there
    is one for this device, or it moves the pointer if given an offset.

    \i  atEnd() indicates whether there is more to read, if this is
    meaningful for this device.

    \i  reset() moves the read/write pointer to the start of the
    device, if that is possible for this device.

    \endlist

    The state access are all "get" functions. The TQIODevice subclass
    calls setState() to update the state, and simple access functions
    tell the user of the device what the device's state is. Here are
    the settings, and their associated access functions:

    \list

    \i  Access type. Some devices are direct access (it is possible
    to read/write anywhere), whereas others are sequential. TQIODevice
    provides the access functions (isDirectAccess(),
    isSequentialAccess(), and isCombinedAccess()) to tell users what a
    given I/O device supports.

    \i  Buffering. Some devices are accessed in raw mode, whereas
    others are buffered. Buffering usually provides greater
    efficiency, particularly for small read/write operations.
    isBuffered() tells the user whether a given device is buffered.
    (This can often be set by the application in the call to open().)

    \i  Synchronicity. Synchronous devices work immediately (for
    example, files). When you read from a file, the file delivers its
    data straight away. Other kinds of device, such as a socket
    connected to a HTTP server, may not deliver the data until seconds
    after you ask to read it. isSynchronous() and isAsynchronous()
    tell the user how this device operates.

    \i  CR/LF translation. For simplicity, applications often like to
    see just a single CR/LF style, and TQIODevice subclasses can
    provide this. isTranslated() returns true if this object
    translates CR/LF to just LF. (This can often be set by the
    application in the call to open().)

    \i  Permissions. Some files cannot be written. For example,
    isReadable(), isWritable() and isReadWrite() tell the application
    whether it can read from and write to a given device. (This can
    often be set by the application in the call to open().)

    \i  Finally, isOpen() returns true if the device is open, i.e.
    after an open() call.

    \endlist

    TQIODevice provides numerous pure virtual functions that you need
    to implement when subclassing it. Here is a skeleton subclass with
    all the members you are sure to need and some that you will
    probably need:

    \code
    class MyDevice : public TQIODevice
    {
    public:
	MyDevice();
	~MyDevice();

	bool open( int mode );
	void close();
	void flush();

	uint size() const;
	int  at() const;	// non-pure virtual
	bool at( int );		// non-pure virtual
	bool atEnd() const;	// non-pure virtual

	int readBlock( char *data, uint maxlen );
	int writeBlock( const char *data, uint len );
	int readLine( char *data, uint maxlen );

	int getch();
	int putch( int );
	int ungetch( int );
    };
    \endcode

    The three non-pure virtual functions need not be reimplemented for
    sequential devices.

    \sa TQDataStream, TQTextStream
*/

/*!
    \enum TQIODevice::Offset

    The offset within the device.
*/


/*!
    Constructs an I/O device.
*/

TQIODevice::TQIODevice()
{
    ioMode = 0;					// initial mode
    ioSt = IO_Ok;
    ioIndex = 0;
}

/*!
    Destroys the I/O device.
*/

TQIODevice::~TQIODevice()
{
}


/*!
    \fn int TQIODevice::flags() const

    Returns the current I/O device flags setting.

    Flags consists of mode flags and state flags.

    \sa mode(), state()
*/

/*!
    \fn int TQIODevice::mode() const

    Returns bits OR'ed together that specify the current operation
    mode.

    These are the flags that were given to the open() function.

    The flags are \c IO_ReadOnly, \c IO_WriteOnly, \c IO_ReadWrite,
    \c IO_Append, \c IO_Truncate and \c IO_Translate.
*/

/*!
    \fn int TQIODevice::state() const

    Returns bits OR'ed together that specify the current state.

    The flags are: \c IO_Open.

    Subclasses may define additional flags.
*/

/*!
    \fn bool TQIODevice::isDirectAccess() const

    Returns true if the I/O device is a direct access device;
    otherwise returns false, i.e. if the device is a sequential access
    device.

    \sa isSequentialAccess()
*/

/*!
    \fn bool TQIODevice::isSequentialAccess() const

    Returns true if the device is a sequential access device;
    otherwise returns false, i.e. if the device is a direct access
    device.

    Operations involving size() and at(int) are not valid on
    sequential devices.

    \sa isDirectAccess()
*/

/*!
    \fn bool TQIODevice::isCombinedAccess() const

    Returns true if the I/O device is a combined access (both direct
    and sequential) device; otherwise returns false.

    This access method is currently not in use.
*/

/*!
    \fn bool TQIODevice::isBuffered() const

    Returns true if the I/O device is a buffered device; otherwise
    returns false, i.e. the device is a raw device.

    \sa isRaw()
*/

/*!
    \fn bool TQIODevice::isRaw() const

    Returns true if the device is a raw device; otherwise returns
    false, i.e. if the device is a buffered device.

    \sa isBuffered()
*/

/*!
    \fn bool TQIODevice::isSynchronous() const

    Returns true if the I/O device is a synchronous device; otherwise
    returns false, i.e. the device is an asynchronous device.

    \sa isAsynchronous()
*/

/*!
    \fn bool TQIODevice::isAsynchronous() const

    Returns true if the device is an asynchronous device; otherwise
    returns false, i.e. if the device is a synchronous device.

    This mode is currently not in use.

    \sa isSynchronous()
*/

/*!
    \fn bool TQIODevice::isTranslated() const

    Returns true if the I/O device translates carriage-return and
    linefeed characters; otherwise returns false.

    A TQFile is translated if it is opened with the \c IO_Translate
    mode flag.
*/

/*!
    \fn bool TQIODevice::isReadable() const

    Returns true if the I/O device was opened using \c IO_ReadOnly or
    \c IO_ReadWrite mode; otherwise returns false.

    \sa isWritable(), isReadWrite()
*/

/*!
    \fn bool TQIODevice::isWritable() const

    Returns true if the I/O device was opened using \c IO_WriteOnly or
    \c IO_ReadWrite mode; otherwise returns false.

    \sa isReadable(), isReadWrite()
*/

/*!
    \fn bool TQIODevice::isReadWrite() const

    Returns true if the I/O device was opened using \c IO_ReadWrite
    mode; otherwise returns false.

    \sa isReadable(), isWritable()
*/

/*!
    \fn bool TQIODevice::isInactive() const

    Returns true if the I/O device state is 0, i.e. the device is not
    open; otherwise returns false.

    \sa isOpen()
*/

/*!
    \fn bool TQIODevice::isOpen() const

    Returns true if the I/O device has been opened; otherwise returns
    false.

    \sa isInactive()
*/


/*!
    \fn int TQIODevice::status() const

    Returns the I/O device status.

    The I/O device status returns an error code. If open() returns
    false or readBlock() or writeBlock() return -1, this function can
    be called to find out the reason why the operation failed.

    \keyword IO_Ok
    \keyword IO_ReadError
    \keyword IO_WriteError
    \keyword IO_FatalError
    \keyword IO_OpenError
    \keyword IO_ConnectError
    \keyword IO_AbortError
    \keyword IO_TimeOutError
    \keyword IO_UnspecifiedError

    The status codes are:
    \table
    \header \i Status code \i Meaning
    \row \i \c IO_Ok \i The operation was successful.
    \row \i \c IO_ReadError \i Could not read from the device.
    \row \i \c IO_WriteError \i Could not write to the device.
    \row \i \c IO_FatalError \i A fatal unrecoverable error occurred.
    \row \i \c IO_OpenError \i Could not open the device.
    \row \i \c IO_ConnectError \i Could not connect to the device.
    \row \i \c IO_AbortError \i The operation was unexpectedly aborted.
    \row \i \c IO_TimeOutError \i The operation timed out.
    \row \i \c IO_UnspecifiedError \i An unspecified error happened on close.
    \endtable

    \sa resetStatus()
*/

/*!
    \fn void TQIODevice::resetStatus()

    Sets the I/O device status to \c IO_Ok.

    \sa status()
*/


/*!
  \fn void TQIODevice::setFlags( int flags )

  Used by subclasses to set the device flags to the \a flags specified.
*/

/*!
  \fn void TQIODevice::setType( int type )

  Used by subclasses to set the device type to the \a type specified.
*/

void TQIODevice::setType( int t )
{
#if defined(QT_CHECK_RANGE)
    if ( (t & IO_TypeMask) != t )
	tqWarning( "TQIODevice::setType: Specified type out of range" );
#endif
    ioMode &= ~IO_TypeMask;			// reset type bits
    ioMode |= t;
}

/*!
  \fn void TQIODevice::setMode( int mode )

  Used by subclasses to set the device mode to the \a mode specified.
*/

void TQIODevice::setMode( int m )
{
#if defined(QT_CHECK_RANGE)
    if ( (m & IO_ModeMask) != m )
	tqWarning( "TQIODevice::setMode: Specified mode out of range" );
#endif
    ioMode &= ~IO_ModeMask;			// reset mode bits
    ioMode |= m;
}

/*!
  \fn void TQIODevice::setState( int state )

  Used by subclasses to set the device state to the \a state specified.
*/

void TQIODevice::setState( int s )
{
#if defined(QT_CHECK_RANGE)
    if ( ((uint)s & IO_StateMask) != (uint)s )
	tqWarning( "TQIODevice::setState: Specified state out of range" );
#endif
    ioMode &= ~IO_StateMask;			// reset state bits
    ioMode |= (uint)s;
}

/*!
  Used by subclasses to set the device status (not state) to \a s.
*/

void TQIODevice::setStatus( int s )
{
    ioSt = s;
}


/*!
    \fn bool TQIODevice::open( int mode )

    Opens the I/O device using the specified \a mode. Returns true if
    the device was successfully opened; otherwise returns false.

    The mode parameter \a mode must be an OR'ed combination of the
    following flags.
    \table
    \header \i Mode flags \i Meaning
    \row \i \c IO_Raw \i specifies raw (unbuffered) file access.
    \row \i \c IO_ReadOnly \i opens a file in read-only mode.
    \row \i \c IO_WriteOnly \i opens a file in write-only mode.
    \row \i \c IO_ReadWrite \i opens a file in read/write mode.
    \row \i \c IO_Append \i sets the file index to the end of the file.
    \row \i \c IO_Truncate \i truncates the file.
    \row \i \c IO_Translate \i enables carriage returns and linefeed
    translation for text files under MS-DOS, Windows and Macintosh. On
    Unix systems this flag has no effect. Use with caution as it will
    also transform every linefeed written to the file into a CRLF
    pair. This is likely to corrupt your file if you write write
    binary data. Cannot be combined with \c IO_Raw.
    \endtable

    This virtual function must be reimplemented by all subclasses.

    \sa close()
*/

/*!
    \fn void TQIODevice::close()

    Closes the I/O device.

    This virtual function must be reimplemented by all subclasses.

    \sa open()
*/

/*!
    \fn void TQIODevice::flush()

    Flushes an open I/O device.

    This virtual function must be reimplemented by all subclasses.
*/


/*!
    \fn TQIODevice::Offset TQIODevice::size() const

    Virtual function that returns the size of the I/O device.

    \sa at()
*/

/*!
    Virtual function that returns the current I/O device position.

    This is the position of the data read/write head of the I/O
    device.

    \sa size()
*/

TQIODevice::Offset TQIODevice::at() const
{
    return ioIndex;
}


/*
    The following is a "bad" overload, since it does "not behave essentially
    the same" like the above. So don't use  \overload in the documentation of
    this function and we have to live with the qdoc warning which is generated
    for this.
*/
/*!
    Virtual function that sets the I/O device position to \a pos.
    Returns true if the position was successfully set, i.e. \a pos is
    within range and the seek was successful; otherwise returns false.

    \sa size()
*/

bool TQIODevice::at( Offset pos )
{
#if defined(QT_CHECK_RANGE)
    if ( pos > size() ) {
#if defined(QT_ABI_QT4)
	tqWarning( "TQIODevice::at: Index %lld out of range", pos );
#else
	tqWarning( "TQIODevice::at: Index %lu out of range", pos );
#endif
	return false;
    }
#endif
    ioIndex = pos;
    return true;
}

/*!
    Virtual function that returns true if the I/O device position is
    at the end of the input; otherwise returns false.
*/

bool TQIODevice::atEnd() const
{
    if ( isSequentialAccess() || isTranslated() ) {
	TQIODevice* that = (TQIODevice*)this;
        const int oldStatus = ioSt;
	int c = that->getch();
	bool result = c < 0;
	that->ungetch(c);
        if (ioSt != oldStatus)
            that->ioSt = oldStatus;
	return result;
    } else {
	return at() == size();
    }
}

/*!
    \fn bool TQIODevice::reset()

    Sets the device index position to 0.

    \sa at()
*/


/*!
    \fn int TQIODevice::readBlock( char *data, TQ_ULONG maxlen )

    Reads at most \a maxlen bytes from the I/O device into \a data and
    returns the number of bytes actually read.

    This function should return -1 if a fatal error occurs and should
    return 0 if there are no bytes to read.

    The device must be opened for reading, and \a data must not be 0.

    This virtual function must be reimplemented by all subclasses.

    \sa writeBlock() isOpen() isReadable()
*/

/*!
    This convenience function returns all of the remaining data in the
    device.
*/
TQByteArray TQIODevice::readAll()
{
    if ( isDirectAccess() ) {
	// we know the size
	int n = size()-at(); // ### fix for 64-bit or large files?
	int totalRead = 0;
	TQByteArray ba( n );
	char* c = ba.data();
	while ( n ) {
	    int r = readBlock( c, n );
	    if ( r < 0 )
		return TQByteArray();
	    n -= r;
	    c += r;
	    totalRead += r;
	    // If we have a translated file, then it is possible that
	    // we read less bytes than size() reports
	    if ( atEnd() ) {
		ba.resize( totalRead );
		break;
	    }
	}
	return ba;
    } else {
	// read until we reach the end
	const int blocksize = 512;
	int nread = 0;
	TQByteArray ba;
	while ( !atEnd() ) {
	    ba.resize( nread + blocksize );
	    int r = readBlock( ba.data()+nread, blocksize );
	    if ( r < 0 )
		return TQByteArray();
	    nread += r;
	}
	ba.resize( nread );
	return ba;
    }
}

/*!
    \fn int TQIODevice::writeBlock( const char *data, TQ_ULONG len )

    Writes \a len bytes from \a data to the I/O device and returns the
    number of bytes actually written.

    This function should return -1 if a fatal error occurs.

    This virtual function must be reimplemented by all subclasses.

    \sa readBlock()
*/

/*!
    \overload

    This convenience function is the same as calling writeBlock(
    data.data(), data.size() ).
*/
TQ_LONG TQIODevice::writeBlock( const TQByteArray& data )
{
    return writeBlock( data.data(), data.size() );
}

/*!
    Reads a line of text, (or up to \a maxlen bytes if a newline isn't
    encountered) plus a terminating '\0' into \a data. If there is a
    newline at the end if the line, it is not stripped.

    Returns the number of bytes read including the terminating '\0',
    or -1 if an error occurred.

    This virtual function can be reimplemented much more efficiently
    by the most subclasses.

    \sa readBlock(), TQTextStream::readLine()
*/

TQ_LONG TQIODevice::readLine( char *data, TQ_ULONG maxlen )
{
    if ( maxlen == 0 )				// application bug?
	return 0;
    char *p = data;
    while ( --maxlen && (readBlock(p,1)>0) ) {	// read one byte at a time
	if ( *p++ == '\n' )			// end of line
	    break;
    }
    *p++ = '\0';
    return p - data;
}


/*!
    \fn int TQIODevice::getch()

    Reads a single byte/character from the I/O device.

    Returns the byte/character read, or -1 if the end of the I/O
    device has been reached.

    This virtual function must be reimplemented by all subclasses.

    \sa putch(), ungetch()
*/

/*!
    \fn int TQIODevice::putch( int ch )

    Writes the character \a ch to the I/O device.

    Returns \a ch, or -1 if an error occurred.

    This virtual function must be reimplemented by all subclasses.

    \sa getch(), ungetch()
*/

/*!
    \fn int TQIODevice::ungetch( int ch )

    Puts the character \a ch back into the I/O device and decrements
    the index position if it is not zero.

    This function is normally called to "undo" a getch() operation.

    Returns \a ch, or -1 if an error occurred.

    This virtual function must be reimplemented by all subclasses.

    \sa getch(), putch()
*/
