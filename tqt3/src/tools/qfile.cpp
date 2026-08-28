/****************************************************************************
**
** Implementation of TQFile class
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

#include "qplatformdefs.h"

// POSIX Large File Support redefines open -> open64
#if defined(open)
# undef open
#endif

// POSIX Large File Support redefines truncate -> truncate64
#if defined(truncate)
# undef truncate
#endif

#include "ntqfile.h"
#ifndef NO_ERRNO_H
#include <errno.h>
#endif


// Duplicated from ntqobject.h, but we cannot include ntqobject.h here since
// it causes qmake to not build on irix g++
#define TQT_TRANSLATE_NOOP(scope,x) (x)

const char* qt_fileerr_unknown	= TQT_TRANSLATE_NOOP( "TQFile", "Unknown error" );
const char* qt_fileerr_read	= TQT_TRANSLATE_NOOP( "TQFile", "Could not read from the file" );
const char* qt_fileerr_write	= TQT_TRANSLATE_NOOP( "TQFile", "Could not write to the file" );

#define TQFILEERR_EACCES		TQT_TRANSLATE_NOOP( "TQFile", "Permission denied" )
#define TQFILEERR_EMFILE		TQT_TRANSLATE_NOOP( "TQFile", "Too many open files" )
#define TQFILEERR_ENOENT		TQT_TRANSLATE_NOOP( "TQFile", "No such file or directory" )
#define TQFILEERR_ENOSPC		TQT_TRANSLATE_NOOP( "TQFile", "No space left on device" )

class TQFilePrivate
{
public:
    TQString errorString;
};

extern bool qt_file_access( const TQString& fn, int t );

/*!
    \class TQFile ntqfile.h
    \reentrant
    \brief The TQFile class is an I/O device that operates on files.

    \ingroup io
    \mainclass

    TQFile is an I/O device for reading and writing binary and text
    files. A TQFile may be used by itself or more conveniently with a
    TQDataStream or TQTextStream.

    The file name is usually passed in the constructor but can be
    changed with setName(). You can check for a file's existence with
    exists() and remove a file with remove().

    The file is opened with open(), closed with close() and flushed
    with flush(). Data is usually read and written using TQDataStream
    or TQTextStream, but you can read with readBlock() and readLine()
    and write with writeBlock(). TQFile also supports getch(),
    ungetch() and putch().

    The size of the file is returned by size(). You can get the
    current file position or move to a new file position using the
    at() functions. If you've reached the end of the file, atEnd()
    returns true. The file handle is returned by handle().

    Here is a code fragment that uses TQTextStream to read a text file
    line by line. It prints each line with a line number.
    \code
    TQStringList lines;
    TQFile file( "file.txt" );
    if ( file.open( IO_ReadOnly ) ) {
	TQTextStream stream( &file );
	TQString line;
	int i = 1;
	while ( !stream.atEnd() ) {
	    line = stream.readLine(); // line of text excluding '\n'
	    printf( "%3d: %s\n", i++, line.latin1() );
	    lines += line;
	}
	file.close();
    }
    \endcode

    Writing text is just as easy. The following example shows how to
    write the data we read into the string list from the previous
    example:
    \code
    TQFile file( "file.txt" );
    if ( file.open( IO_WriteOnly ) ) {
	TQTextStream stream( &file );
	for ( TQStringList::Iterator it = lines.begin(); it != lines.end(); ++it )
	    stream << *it << "\n";
	file.close();
    }
    \endcode

    The TQFileInfo class holds detailed information about a file, such
    as access permissions, file dates and file types.

    The TQDir class manages directories and lists of file names.

    TQt uses Unicode file names. If you want to do your own I/O on Unix
    systems you may want to use encodeName() (and decodeName()) to
    convert the file name into the local encoding.

    \important readAll() at()

    \sa TQDataStream, TQTextStream
*/

/*!
    \fn TQ_LONG TQFile::writeBlock( const TQByteArray& data )

    \overload
*/


/*!
    Constructs a TQFile with no name.
*/

TQFile::TQFile()
: d(0)
{
    init();
}

/*!
    Constructs a TQFile with a file name \a name.

    \sa setName()
*/

TQFile::TQFile( const TQString &name )
    : fn(name), d(0)
{
    init();
}


/*!
    Destroys a TQFile. Calls close().
*/

TQFile::~TQFile()
{
    close();
    delete d;
}


/*!
  \internal
  Initialize internal data.
*/

void TQFile::init()
{
    delete d;
    d = new TQFilePrivate;
    setFlags( IO_Direct );
    setStatus( IO_Ok );
    setErrorString( qt_fileerr_unknown );
    fh	   = 0;
    fd	   = 0;
    length = 0;
    ioIndex = 0;
    ext_f  = false;				// not an external file handle
}


/*!
    \fn TQString TQFile::name() const

    Returns the name set by setName().

    \sa setName(), TQFileInfo::fileName()
*/

/*!
    Sets the name of the file to \a name. The name can have no path, a
    relative path or an absolute absolute path.

    Do not call this function if the file has already been opened.

    If the file name has no path or a relative path, the path used
    will be whatever the application's current directory path is
    \e{at the time of the open()} call.

    Example:
    \code
	TQFile file;
	TQDir::setCurrent( "/tmp" );
	file.setName( "readme.txt" );
	TQDir::setCurrent( "/home" );
	file.open( IO_ReadOnly );      // opens "/home/readme.txt" under Unix
    \endcode

    Note that the directory separator "/" works for all operating
    systems supported by TQt.

    \sa name(), TQFileInfo, TQDir
*/

void TQFile::setName( const TQString &name )
{
    if ( isOpen() ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQFile::setName: File is open" );
#endif
	close();
    }
    fn = name;
}

/*!
    \overload

    Returns true if this file exists; otherwise returns false.

    \sa name()
*/

bool TQFile::exists() const
{
    return qt_file_access( fn, F_OK );
}

/*!
    Returns true if the file given by \a fileName exists; otherwise
    returns false.
*/

bool TQFile::exists( const TQString &fileName )
{
    return qt_file_access( fileName, F_OK );
}


/*!
    Removes the file specified by the file name currently set. Returns
    true if successful; otherwise returns false.

    The file is closed before it is removed.
*/

bool TQFile::remove()
{
    close();
    return remove( fn );
}

#if defined(Q_OS_MAC) || defined(Q_OS_MSDOS) || defined(Q_OS_WIN32) || defined(Q_OS_OS2)
# define HAS_TEXT_FILEMODE			// has translate/text filemode
#endif
#if defined(O_NONBLOCK)
# define HAS_ASYNC_FILEMODE
# define OPEN_ASYNC O_NONBLOCK
#elif defined(O_NDELAY)
# define HAS_ASYNC_FILEMODE
# define OPEN_ASYNC O_NDELAY
#endif

/*!
    Flushes the file buffer to the disk.

    close() also flushes the file buffer.
*/

void TQFile::flush()
{
    if ( isOpen() && fh ) {			// can only flush open/buffered file
        if ( fflush( fh )  ) {		// write error
            if ( errno == ENOSPC )			// disk is full
                setStatus( IO_ResourceError );
            else
                setStatus( IO_WriteError );
            setErrorStringErrno( errno );
        }
    }
}

/*! \reimp
    \fn TQIODevice::Offset TQFile::at() const
*/

/*!
    Returns true if the end of file has been reached; otherwise returns false.
    If TQFile has not been open()'d, then the behavior is undefined.

    \sa size()
*/

bool TQFile::atEnd() const
{
    if ( !isOpen() ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQFile::atEnd: File is not open" );
#endif
	return false;
    }
    if ( isDirectAccess() && !isTranslated() ) {
	if ( at() < size() )
	    return false;
    }
    const TQString errorString = d->errorString;
    const bool ret = TQIODevice::atEnd();
    if (errorString != d->errorString)
        d->errorString = errorString;
    return ret;
}

/*!
    Reads a line of text.

    Reads bytes from the file into the char* \a p, until end-of-line
    or \a maxlen bytes have been read, whichever occurs first. Returns
    the number of bytes read, or -1 if there was an error. Any
    terminating newline is not stripped.

    This function is only efficient for buffered files. Avoid
    readLine() for files that have been opened with the \c IO_Raw
    flag.

    \sa readBlock(), TQTextStream::readLine()
*/

TQ_LONG TQFile::readLine( char *p, TQ_ULONG maxlen )
{
    if ( maxlen == 0 )				// application bug?
	return 0;
#if defined(QT_CHECK_STATE)
    TQ_CHECK_PTR( p );
    if ( !isOpen() ) {				// file not open
	tqWarning( "TQFile::readLine: File not open" );
	return -1;
    }
    if ( !isReadable() ) {			// reading not permitted
	tqWarning( "TQFile::readLine: Read operation not permitted" );
	return -1;
    }
#endif
    TQ_LONG nread;				// number of bytes read
    if ( isRaw() ) {				// raw file
	nread = TQIODevice::readLine( p, maxlen );
    } else {					// buffered file
	p = fgets( p, maxlen, fh );
	if ( p ) {
	    nread = tqstrlen( p );
	    if ( !isSequentialAccess() )
		ioIndex += nread;
	} else {
	    nread = -1;
	    setStatus(IO_ReadError);
	    setErrorString( qt_fileerr_read );
	}
    }
    return nread;
}


/*!
    \overload

    Reads a line of text.

    Reads bytes from the file into string \a s, until end-of-line or
    \a maxlen bytes have been read, whichever occurs first. Returns
    the number of bytes read, or -1 if there was an error, e.g. end of
    file. Any terminating newline is not stripped.

    This function is only efficient for buffered files. Avoid using
    readLine() for files that have been opened with the \c IO_Raw
    flag.

    Note that the string is read as plain Latin1 bytes, not Unicode.

    \sa readBlock(), TQTextStream::readLine()
*/

TQ_LONG TQFile::readLine( TQString& s, TQ_ULONG maxlen )
{
    TQByteArray ba(maxlen);
    TQ_LONG l = readLine(ba.data(),maxlen);
    if ( l >= 0 ) {
	ba.truncate(l);
	s = TQString(ba);
    }
    return l;
}


/*!
    Reads a single byte/character from the file.

    Returns the byte/character read, or -1 if the end of the file has
    been reached.

    \sa putch(), ungetch()
*/

int TQFile::getch()
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {				// file not open
	tqWarning( "TQFile::getch: File not open" );
	return EOF;
    }
    if ( !isReadable() ) {			// reading not permitted
	tqWarning( "TQFile::getch: Read operation not permitted" );
	return EOF;
    }
#endif

    int ch;

    if ( !ungetchBuffer.isEmpty() ) {
	int len = ungetchBuffer.length();
	ch = ungetchBuffer[ len-1 ];
	ungetchBuffer.truncate( len - 1 );
	return ch;
    }

    if ( isRaw() ) {				// raw file (inefficient)
	char buf[1];
	ch = readBlock( buf, 1 ) == 1 ? buf[0] : EOF;
    } else {					// buffered file
	if ( (ch = getc( fh )) != EOF ) {
	    if ( !isSequentialAccess() )
		ioIndex++;
	} else {
	    setStatus(IO_ReadError);
	    setErrorString( qt_fileerr_read );
	}
    }
    return ch;
}

/*!
    Writes the character \a ch to the file.

    Returns \a ch, or -1 if some error occurred.

    \sa getch(), ungetch()
*/

int TQFile::putch( int ch )
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {				// file not open
	tqWarning( "TQFile::putch: File not open" );
	return EOF;
    }
    if ( !isWritable() ) {			// writing not permitted
	tqWarning( "TQFile::putch: Write operation not permitted" );
	return EOF;
    }
#endif
    if ( isRaw() ) {				// raw file (inefficient)
	char buf[1];
	buf[0] = ch;
	ch = writeBlock( buf, 1 ) == 1 ? ch : EOF;
    } else {					// buffered file
	if ( (ch = putc( ch, fh )) != EOF ) {
	    if ( !isSequentialAccess() )
		ioIndex++;
	    if ( ioIndex > length )		// update file length
		length = ioIndex;
	} else {
	    setStatus(IO_WriteError);
	    setErrorString( qt_fileerr_write );
	}
    }
    return ch;
}

/*!
    Puts the character \a ch back into the file and decrements the
    index if it is not zero.

    This function is normally called to "undo" a getch() operation.

    Returns \a ch, or -1 if an error occurred.

    \sa getch(), putch()
*/

int TQFile::ungetch( int ch )
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {				// file not open
	tqWarning( "TQFile::ungetch: File not open" );
	return EOF;
    }
    if ( !isReadable() ) {			// reading not permitted
	tqWarning( "TQFile::ungetch: Read operation not permitted" );
	return EOF;
    }
#endif
    if ( ch == EOF )				// cannot unget EOF
	return ch;

    if ( isSequentialAccess() && !fh) {
	// pipe or similar => we cannot ungetch, so do it manually
	ungetchBuffer += TQChar(ch);
	return ch;
    }

    if ( isRaw() ) {				// raw file (very inefficient)
	char buf[1];
	at( ioIndex-1 );
	buf[0] = ch;
	if ( writeBlock(buf, 1) == 1 )
	    at ( ioIndex-1 );
	else
	    ch = EOF;
    } else {					// buffered file
	if ( (ch = ungetc(ch, fh)) != EOF ) {
	    if ( !isSequentialAccess() )
		ioIndex--;
	} else {
	    setStatus( IO_ReadError );
	    setErrorString( qt_fileerr_read );
	}
    }
    return ch;
}


static TQCString locale_encoder( const TQString &fileName )
{
    return fileName.local8Bit();
}


static TQFile::EncoderFn encoder = locale_encoder;

/*!
    When you use TQFile, TQFileInfo, and TQDir to access the file system
    with TQt, you can use Unicode file names. On Unix, these file names
    are converted to an 8-bit encoding. If you want to do your own
    file I/O on Unix, you should convert the file name using this
    function. On Windows NT/2000, Unicode file names are supported
    directly in the file system and this function should be avoided.
    On Windows 95, non-Latin1 locales are not supported.

    By default, this function converts \a fileName to the local 8-bit
    encoding determined by the user's locale. This is sufficient for
    file names that the user chooses. File names hard-coded into the
    application should only use 7-bit ASCII filename characters.

    The conversion scheme can be changed using setEncodingFunction().
    This might be useful if you wish to give the user an option to
    store file names in UTF-8, etc., but be aware that such file names
    would probably then be unrecognizable when seen by other programs.

    \sa decodeName()
*/

TQCString TQFile::encodeName( const TQString &fileName )
{
    return (*encoder)(fileName);
}

/*!
    \enum TQFile::EncoderFn

    This is used by TQFile::setEncodingFunction().
*/

/*!
    \nonreentrant

    Sets the function for encoding Unicode file names to \a f. The
    default encodes in the locale-specific 8-bit encoding.

    \sa encodeName()
*/
void TQFile::setEncodingFunction( EncoderFn f )
{
    encoder = f;
}

static
TQString locale_decoder( const TQCString &localFileName )
{
#ifndef TQ_WS_MAC
    return TQString::fromLocal8Bit(localFileName);
#else
    extern TQString qt_mac_precomposeFileName(const TQString &); // qglobal.cpp
    return qt_mac_precomposeFileName(TQString::fromLocal8Bit(localFileName));
#endif
}

static TQFile::DecoderFn decoder = locale_decoder;

/*!
    This does the reverse of TQFile::encodeName() using \a localFileName.

    \sa setDecodingFunction()
*/
TQString TQFile::decodeName( const TQCString &localFileName )
{
    return (*decoder)(localFileName);
}

/*!
    \enum TQFile::DecoderFn

    This is used by TQFile::setDecodingFunction().
*/

/*!
    \nonreentrant

    Sets the function for decoding 8-bit file names to \a f. The
    default uses the locale-specific 8-bit encoding.

    \sa encodeName(), decodeName()
*/

void TQFile::setDecodingFunction( DecoderFn f )
{
    decoder = f;
}

/*!
    Returns a human-readable description of the reason of an error that occurred
    on the device. The error described by the string corresponds to changes of
    TQIODevice::status(). If the status is reset, the error string is also reset.

    The returned strings are not translated with the TQObject::tr() or
    TQApplication::translate() functions. They are marked as translatable
    strings in the "TQFile" context. Before you show the string to the user you
    should translate it first, for example:

    \code
	TQFile f( "address.dat" );
	if ( !f.open( IO_ReadOnly ) {
	    TQMessageBox::critical(
		this,
		tr("Open failed"),
		tr("Could not open file for reading: %1").arg( tqApp->translate("TQFile",f.errorString()) )
		);
	    return;
	}
    \endcode

    \sa TQIODevice::status(), TQIODevice::resetStatus(), setErrorString()
*/

TQString	TQFile::errorString() const
{
    if ( status() == IO_Ok )
	return qt_fileerr_unknown;
    return d->errorString;
}

/*!
    \nonreentrant

    Sets the error string returned by the errorString() function to \a str.

    \sa errorString(), TQIODevice::status()
*/

void TQFile::setErrorString( const TQString& str )
{
    d->errorString = str;
}

void TQFile::setErrorStringErrno( int errnum )
{
    switch ( errnum ) {
	case EACCES:
	    d->errorString = TQFILEERR_EACCES;
	    break;
	case EMFILE:
	    d->errorString = TQFILEERR_EMFILE;
	    break;
	case ENOENT:
	    d->errorString = TQFILEERR_ENOENT;
	    break;
	case ENOSPC:
	    d->errorString = TQFILEERR_ENOSPC;
	    break;
	default:
#ifndef Q_OS_TEMP
	    d->errorString = TQString::fromLocal8Bit( strerror( errnum ) );
#else
	    {
		unsigned short *string;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
			       NULL,
			       errnum,
			       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			       (LPTSTR)&string,
			       0,
			       NULL );
		d->errorString = TQString::fromUcs2( string );
	        LocalFree( (HLOCAL)string );
	    }
#endif
	    break;
    }
}
