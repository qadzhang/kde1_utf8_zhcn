/****************************************************************************
**
** Implementation of TQFileInfo class
**
** Created : 950628
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

#include "ntqfileinfo.h"
#include "ntqdatetime.h"
#include "ntqdir.h"
#include "qfiledefs_p.h"
#include "ntqdeepcopy.h"
#if defined(QT_LARGEFILE_SUPPORT) && !defined(QT_ABI_QT4)
#include <limits.h>
#endif


extern bool qt_file_access( const TQString& fn, int t );

/*!
    \class TQFileInfo
    \reentrant
    \brief The TQFileInfo class provides system-independent file information.

    \ingroup io

    TQFileInfo provides information about a file's name and position
    (path) in the file system, its access rights and whether it is a
    directory or symbolic link, etc. The file's size and last
    modified/read times are also available.

    A TQFileInfo can point to a file with either a relative or an
    absolute file path. Absolute file paths begin with the directory
    separator "/" (or with a drive specification on Windows). Relative
    file names begin with a directory name or a file name and specify
    a path relative to the current working directory. An example of an
    absolute path is the string "/tmp/quartz". A relative path might
    look like "src/fatlib". You can use the function isRelative() to
    check whether a TQFileInfo is using a relative or an absolute file
    path. You can call the function convertToAbs() to convert a
    relative TQFileInfo's path to an absolute path.

    The file that the TQFileInfo works on is set in the constructor or
    later with setFile(). Use exists() to see if the file exists and
    size() to get its size.

    To speed up performance, TQFileInfo caches information about the
    file. Because files can be changed by other users or programs, or
    even by other parts of the same program, there is a function that
    refreshes the file information: refresh(). If you want to switch
    off a TQFileInfo's caching and force it to access the file system
    every time you request information from it call setCaching(false).

    The file's type is obtained with isFile(), isDir() and
    isSymLink(). The readLink() function provides the name of the file
    the symlink points to.

    Elements of the file's name can be extracted with dirPath() and
    fileName(). The fileName()'s parts can be extracted with
    baseName() and extension().

    The file's dates are returned by created(), lastModified() and
    lastRead(). Information about the file's access permissions is
    obtained with isReadable(), isWritable() and isExecutable(). The
    file's ownership is available from owner(), ownerId(), group() and
    groupId(). You can examine a file's permissions and ownership in a
    single statement using the permission() function.

    If you need to read and traverse directories, see the TQDir class.
*/

/*!
    \enum TQFileInfo::PermissionSpec

    This enum is used by the permission() function to report the
    permissions and ownership of a file. The values may be OR-ed
    together to test multiple permissions and ownership values.

    \value ReadOwner The file is readable by the owner of the file.
    \value WriteOwner The file is writable by the owner of the file.
    \value ExeOwner The file is executable by the owner of the file.
    \value ReadUser The file is readable by the user.
    \value WriteUser The file is writable by the user.
    \value ExeUser The file is executable by the user.
    \value ReadGroup The file is readable by the group.
    \value WriteGroup The file is writable by the group.
    \value ExeGroup The file is executable by the group.
    \value ReadOther The file is readable by anyone.
    \value WriteOther The file is writable by anyone.
    \value ExeOther The file is executable by anyone.

    \warning The semantics of \c ReadUser, \c WriteUser and \c ExeUser are
    unfortunately not platform independent: on Unix, the rights of the owner of
    the file are returned and on Windows the rights of the current user are
    returned. This behavior might change in a future TQt version. If you want to
    find the rights of the owner of the file, you should use the flags \c
    ReadOwner, \c WriteOwner and \c ExeOwner. If you want to find out the
    rights of the current user, you should use isReadable(), isWritable() and
    isExecutable().
*/


/*!
    Constructs a new empty TQFileInfo.
*/

TQFileInfo::TQFileInfo()
{
    fic	  = 0;
    cache = true;
#if defined(Q_OS_UNIX)
    symLink = false;
#endif
}

/*!
    Constructs a new TQFileInfo that gives information about the given
    file. The \a file can also include an absolute or relative path.

    \warning Some functions might behave in a counter-intuitive way
    if \a file has a trailing directory separator.

    \sa setFile(), isRelative(), TQDir::setCurrent(), TQDir::isRelativePath()
*/

TQFileInfo::TQFileInfo( const TQString &file )
{
    fn	  = file;
    slashify( fn );
    fic	  = 0;
    cache = true;
#if defined(Q_OS_UNIX)
    symLink = false;
#endif
}

/*!
    Constructs a new TQFileInfo that gives information about file \a
    file.

    If the \a file has a relative path, the TQFileInfo will also have a
    relative path.

    \sa isRelative()
*/

TQFileInfo::TQFileInfo( const TQFile &file )
{
    fn	  = file.name();
    slashify( fn );
    fic	  = 0;
    cache = true;
#if defined(Q_OS_UNIX)
    symLink = false;
#endif
}

/*!
    Constructs a new TQFileInfo that gives information about the file
    called \a fileName in the directory \a d.

    If \a d has a relative path, the TQFileInfo will also have a
    relative path.

    \sa isRelative()
*/
#ifndef TQT_NO_DIR
TQFileInfo::TQFileInfo( const TQDir &d, const TQString &fileName )
{
    fn	  = d.filePath( fileName );
    slashify( fn );
    fic	  = 0;
    cache = true;
#if defined(Q_OS_UNIX)
    symLink = false;
#endif
}
#endif
/*!
    Constructs a new TQFileInfo that is a copy of \a fi.
*/

TQFileInfo::TQFileInfo( const TQFileInfo &fi )
{
    fn = fi.fn;
    if ( fi.fic ) {
	fic = new TQFileInfoCache;
	*fic = *fi.fic;
    } else {
	fic = 0;
    }
    cache = fi.cache;
#if defined(Q_OS_UNIX)
    symLink = fi.symLink;
#endif
}

/*!
    Destroys the TQFileInfo and frees its resources.
*/

TQFileInfo::~TQFileInfo()
{
    delete fic;
}


/*!
    Makes a copy of \a fi and assigns it to this TQFileInfo.
*/

TQFileInfo &TQFileInfo::operator=( const TQFileInfo &fi )
{
    fn = fi.fn;
    if ( !fi.fic ) {
	delete fic;
	fic = 0;
    } else {
	if ( !fic ) {
	    fic = new TQFileInfoCache;
	    TQ_CHECK_PTR( fic );
	}
	*fic = *fi.fic;
    }
    cache = fi.cache;
#if defined(Q_OS_UNIX)
    symLink = fi.symLink;
#endif
    return *this;
}


/*!
    Sets the file that the TQFileInfo provides information about to \a
    file.

    The \a file can also include an absolute or relative file path.
    Absolute paths begin with the directory separator (e.g. "/" under
    Unix) or a drive specification (under Windows). Relative file
    names begin with a directory name or a file name and specify a
    path relative to the current directory.

    Example:
    \code
    TQString absolute = "/local/bin";
    TQString relative = "local/bin";
    TQFileInfo absFile( absolute );
    TQFileInfo relFile( relative );

    TQDir::setCurrent( TQDir::rootDirPath() );
    // absFile and relFile now point to the same file

    TQDir::setCurrent( "/tmp" );
    // absFile now points to "/local/bin",
    // while relFile points to "/tmp/local/bin"
    \endcode

    \sa isRelative(), TQDir::setCurrent(), TQDir::isRelativePath()
*/

void TQFileInfo::setFile( const TQString &file )
{
    fn = file;
    slashify( fn );
    delete fic;
    fic = 0;
}

/*!
    \overload

    Sets the file that the TQFileInfo provides information about to \a
    file.

    If \a file includes a relative path, the TQFileInfo will also have
    a relative path.

    \sa isRelative()
*/

void TQFileInfo::setFile( const TQFile &file )
{
    fn	= file.name();
    slashify( fn );
    delete fic;
    fic = 0;
}

/*!
    \overload

    Sets the file that the TQFileInfo provides information about to \a
    fileName in directory \a d.

    If \a fileName includes a relative path, the TQFileInfo will also
    have a relative path.

    \sa isRelative()
*/
#ifndef TQT_NO_DIR
void TQFileInfo::setFile( const TQDir &d, const TQString &fileName )
{
    fn	= d.filePath( fileName );
    slashify( fn );
    delete fic;
    fic = 0;
}
#endif

/*!
    Returns true if the file exists; otherwise returns false.
*/

bool TQFileInfo::exists() const
{
    return qt_file_access( fn, F_OK );
}

/*!
    Refreshes the information about the file, i.e. reads in information
    from the file system the next time a cached property is fetched.

    \sa setCaching()
*/

void TQFileInfo::refresh() const
{
    TQFileInfo *that = (TQFileInfo*)this;		// Mutable function
    delete that->fic;
    that->fic = 0;
}

/*!
    \fn bool TQFileInfo::caching() const

    Returns true if caching is enabled; otherwise returns false.

    \sa setCaching(), refresh()
*/

/*!
    If \a enable is true, enables caching of file information. If \a
    enable is false caching is disabled.

    When caching is enabled, TQFileInfo reads the file information from
    the file system the first time it's needed, but generally not
    later.

    Caching is enabled by default.

    \sa refresh(), caching()
*/

void TQFileInfo::setCaching( bool enable )
{
    if ( cache == enable )
	return;
    cache = enable;
    if ( cache ) {
	delete fic;
	fic = 0;
    }
}


/*!
    Returns the file name, including the path (which may be absolute
    or relative).

    \sa isRelative(), absFilePath()
*/

TQString TQFileInfo::filePath() const
{
    return fn;
}

/*!
    Returns the base name of the file.

    If \a complete is false (the default) the base name consists of
    all characters in the file name up to (but not including) the \e
    first '.' character.

    If \a complete is true the base name consists of all characters in
    the file up to (but not including) the \e last '.' character.

    The path is not included in either case.

    Example:
    \code
	TQFileInfo fi( "/tmp/archive.tar.gz" );
	TQString base = fi.baseName();  // base = "archive"
	base = fi.baseName( true );    // base = "archive.tar"
    \endcode

    \sa fileName(), extension()
*/

TQString TQFileInfo::baseName( bool complete ) const
{
    TQString tmp = fileName();
    int pos = complete ? tmp.findRev( '.' ) : tmp.find( '.' );
    if ( pos == -1 )
	return tmp;
    else
	return tmp.left( pos );
}

/*!
    Returns the file's extension name.

    If \a complete is true (the default), extension() returns the
    string of all characters in the file name after (but not
    including) the first '.'  character.

    If \a complete is false, extension() returns the string of all
    characters in the file name after (but not including) the last '.'
    character.

    Example:
    \code
	TQFileInfo fi( "/tmp/archive.tar.gz" );
	TQString ext = fi.extension();  // ext = "tar.gz"
	ext = fi.extension( false );   // ext = "gz"
    \endcode

    \sa fileName(), baseName()
*/

TQString TQFileInfo::extension( bool complete ) const
{
    TQString s = fileName();
    int pos = complete ? s.find( '.' ) : s.findRev( '.' );
    if ( pos < 0 )
	return TQString::fromLatin1( "" );
    else
	return s.right( s.length() - pos - 1 );
}

/*!
    Returns the file's path as a TQDir object.

    If the TQFileInfo is relative and \a absPath is false, the TQDir
    will be relative; otherwise it will be absolute.

    \sa dirPath(), filePath(), fileName(), isRelative()
*/
#ifndef TQT_NO_DIR
TQDir TQFileInfo::dir( bool absPath ) const
{
    return TQDir( dirPath(absPath) );
}
#endif


/*!
    Returns true if the file is readable; otherwise returns false.

    \sa isWritable(), isExecutable(), permission()
*/

bool TQFileInfo::isReadable() const
{
#ifdef TQ_WS_WIN
    return qt_file_access( fn, R_OK ) && permission( ReadUser );
#else
    return qt_file_access( fn, R_OK );
#endif
}

/*!
    Returns true if the file is writable; otherwise returns false.

    \sa isReadable(), isExecutable(), permission()
*/

bool TQFileInfo::isWritable() const
{
#ifdef TQ_WS_WIN
    return qt_file_access( fn, W_OK ) && permission( WriteUser );
#else
    return qt_file_access( fn, W_OK );
#endif
}

/*!
    Returns true if the file is executable; otherwise returns false.

    \sa isReadable(), isWritable(), permission()
*/

bool TQFileInfo::isExecutable() const
{
#ifdef TQ_WS_WIN
#if defined(_MSC_VER) && _MSC_VER >= 1400
    return permission( ExeUser );
#else
    return qt_file_access( fn, X_OK ) && permission( ExeUser );
#endif //_MSC_VER >= 1400
#else
    return qt_file_access( fn, X_OK );
#endif
}

#ifndef TQ_WS_WIN
bool TQFileInfo::isHidden() const
{
    return fileName()[ 0 ] == TQChar( '.' );
}
#endif

/*!
    Returns true if the file path name is relative. Returns false if
    the path is absolute (e.g. under Unix a path is absolute if it
    begins with a "/").
*/
#ifndef TQT_NO_DIR
bool TQFileInfo::isRelative() const
{
    return TQDir::isRelativePath( fn );
}

/*!
    Converts the file's path to an absolute path.

    If it is already absolute, nothing is done.

    \sa filePath(), isRelative()
*/

bool TQFileInfo::convertToAbs()
{
    if ( isRelative() )
	fn = absFilePath();
    return TQDir::isRelativePath( fn );
}
#endif

/*!
    Returns the file size in bytes, or 0 if the file does not exist or
    if the size is 0 or if the size cannot be fetched.
*/
#if defined(QT_ABI_QT4)
TQIODevice::Offset TQFileInfo::size() const
#else
uint TQFileInfo::size() const
#endif
{
    if ( !fic || !cache )
	doStat();
    if ( fic )
#if defined(QT_ABI_QT4)
	return (TQIODevice::Offset)fic->st.st_size;
#elif defined(QT_LARGEFILE_SUPPORT)
	return (uint)fic->st.st_size > UINT_MAX ? UINT_MAX : (uint)fic->st.st_size;
#else
	return (uint)fic->st.st_size;
#endif
    else
	return 0;
}

/*!
    Returns the date and time when the file was created.

    On platforms where this information is not available, returns the
    same as lastModified().

    \sa created() lastModified() lastRead()
*/

TQDateTime TQFileInfo::created() const
{
    TQDateTime dt;
    if ( !fic || !cache )
	doStat();
    if ( fic && fic->st.st_ctime != 0 ) {
	dt.setTime_t( fic->st.st_ctime );
	return dt;
    } else {
	return lastModified();
    }
}

/*!
    Returns the date and time when the file was last modified.

    \sa created() lastModified() lastRead()
*/

TQDateTime TQFileInfo::lastModified() const
{
    TQDateTime dt;
    if ( !fic || !cache )
	doStat();
    if ( fic )
	dt.setTime_t( fic->st.st_mtime );
    return dt;
}

/*!
    Returns the date and time when the file was last read (accessed).

    On platforms where this information is not available, returns the
    same as lastModified().

    \sa created() lastModified() lastRead()
*/

TQDateTime TQFileInfo::lastRead() const
{
    TQDateTime dt;
    if ( !fic || !cache )
	doStat();
    if ( fic && fic->st.st_atime != 0 ) {
	dt.setTime_t( fic->st.st_atime );
	return dt;
    } else {
	return lastModified();
    }
}

#ifndef TQT_NO_DIR

/*!
    Returns the absolute path including the file name.

    The absolute path name consists of the full path and the file
    name. On Unix this will always begin with the root, '/',
    directory. On Windows this will always begin 'D:/' where D is a
    drive letter, except for network shares that are not mapped to a
    drive letter, in which case the path will begin '//sharename/'.

    This function returns the same as filePath(), unless isRelative()
    is true.

    If the TQFileInfo is empty it returns TQDir::currentDirPath().

    This function can be time consuming under Unix (in the order of
    milliseconds).

    \sa isRelative(), filePath()
*/
TQString TQFileInfo::absFilePath() const
{
    TQString tmp;
    if ( TQDir::isRelativePath(fn)
#if defined(Q_OS_WIN32)
	 && fn[1] != ':'
#endif
	 ) {
	tmp = TQDir::currentDirPath();
	tmp += '/';
    }
    tmp += fn;
    makeAbs( tmp );
    return TQDir::cleanDirPath( tmp );
}

/*! \internal
    Detaches all internal data.
*/
void TQFileInfo::detach()
{
    fn = TQDeepCopy<TQString>( fn );
    if ( fic ) {
	TQFileInfoCache *cur = fic;
	fic = new TQFileInfoCache;
	*fic = *cur;
	delete cur;
    }
}

#endif
