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
#include "qfiledefs_p.h"
#include "ntqdatetime.h"
#include "ntqdir.h"

#include <limits.h>
#if !defined(TQWS) && defined(Q_OS_MAC)
# include <qt_mac.h>
#endif
#if defined(Q_OS_HURD)
# include <stdlib.h>
#endif

void TQFileInfo::slashify( TQString& )
{
    return;
}


void TQFileInfo::makeAbs( TQString & )
{
    return;
}

/*!
    Returns true if this object points to a file. Returns false if the
    object points to something which isn't a file, e.g. a directory or
    a symlink.

    \sa isDir(), isSymLink()
*/
bool TQFileInfo::isFile() const
{
    if ( !fic || !cache )
	doStat();
    return fic ? (fic->st.st_mode & S_IFMT) == S_IFREG : false;
}

/*!
    Returns true if this object points to a directory or to a symbolic
    link to a directory; otherwise returns false.

    \sa isFile(), isSymLink()
*/
bool TQFileInfo::isDir() const
{
    if ( !fic || !cache )
	doStat();
    return fic ? (fic->st.st_mode & S_IFMT) == S_IFDIR : false;
}

/*!
    Returns true if this object points to a symbolic link (or to a
    shortcut on Windows, or an alias on Mac OS X); otherwise returns
    false.

    \sa isFile(), isDir(), readLink()
*/

bool TQFileInfo::isSymLink() const
{
    if ( !fic || !cache )
	doStat();
    if(symLink)
        return true;
#if !defined(TQWS) && defined(Q_OS_MAC)
    {
        FSRef fref;
        if(FSPathMakeRef((const UInt8 *)TQFile::encodeName(fn).data(), &fref, NULL) == noErr) {
            Boolean isAlias, isFolder;
            if(FSIsAliasFile(&fref, &isAlias, &isFolder) == noErr)
                return isAlias;
        }
    }
#endif
    return false;
}

/*!
    Returns the name a symlink (or shortcut on Windows) points to, or
    a TQString::null if the object isn't a symbolic link.

    This name may not represent an existing file; it is only a string.
    TQFileInfo::exists() returns true if the symlink points to an
    existing file.

    \sa exists(), isSymLink(), isDir(), isFile()
*/

TQString TQFileInfo::readLink() const
{
    if ( !isSymLink() )
        return TQString();
#if defined(Q_OS_UNIX) && !defined(Q_OS_OS2EMX)
#if !defined(PATH_MAX)
    int size = 256;
    char *s = NULL, *s2;

    while (1)
    {
        s2 = (char *) realloc (s, size);
        if (s2 == NULL) {
            free( s );
            return TQString();
        }
        s = s2;
        int len = readlink ( TQFile::encodeName(fn).data(), s, size );
        if ( len < 0 ) {
            free( s );
            return TQString();
        }
        if ( len < size ) {
            s[len] = '\0';
            TQString str = TQFile::decodeName(s);
            free(s);
            return str;
        }
        size *= 2;
    }
#else
    char s[PATH_MAX+1];
    int len = readlink( TQFile::encodeName(fn).data(), s, PATH_MAX );
    if ( len >= 0 ) {
	s[len] = '\0';
	return TQFile::decodeName(s);
    }
#endif /* !PATH_MAX */
#endif /* Q_OS_UNIX && !Q_OS_OS2EMX */
#if !defined(TQWS) && defined(Q_OS_MAC)
    {
        FSRef fref;
        if(FSPathMakeRef((const UInt8 *)TQFile::encodeName(fn).data(), &fref, NULL) == noErr) {
            Boolean isAlias, isFolder;
            if(FSResolveAliasFile(&fref, true, &isFolder, &isAlias) == noErr && isAlias) {
                AliasHandle alias;
                if(FSNewAlias(0, &fref, &alias) == noErr && alias) {
                    CFStringRef cfstr;
                    if(FSCopyAliasInfo(alias, 0, 0, &cfstr, 0, 0) == noErr) {
                        TQString cfstring2qstring(CFStringRef str); //qglobal.cpp
                        return cfstring2qstring(cfstr);
                    }
                }
            }
        }
    }
#endif
    return TQString();
}

static const uint nobodyID = (uint) -2;

/*!
    Returns the owner of the file. On systems where files
    do not have owners, or if an error occurs, TQString::null is
    returned.

    This function can be time consuming under Unix (in the order of
    milliseconds).

    \sa ownerId(), group(), groupId()
*/

TQString TQFileInfo::owner() const
{
    passwd *pw = getpwuid( ownerId() );
    if ( pw )
	return TQFile::decodeName( pw->pw_name );
    return TQString::null;
}

/*!
    Returns the id of the owner of the file.

    On Windows and on systems where files do not have owners this
    function returns ((uint) -2).

    \sa owner(), group(), groupId()
*/

uint TQFileInfo::ownerId() const
{
    if ( !fic || !cache )
	doStat();
    if ( fic )
	return fic->st.st_uid;
    return nobodyID;
}

/*!
    Returns the group of the file. On Windows, on systems where files
    do not have groups, or if an error occurs, TQString::null is
    returned.

    This function can be time consuming under Unix (in the order of
    milliseconds).

    \sa groupId(), owner(), ownerId()
*/

TQString TQFileInfo::group() const
{
    struct group *gr = getgrgid( groupId() );
    if ( gr )
	return TQFile::decodeName( gr->gr_name );
    return TQString::null;
}

/*!
    Returns the id of the group the file belongs to.

    On Windows and on systems where files do not have groups this
    function always returns (uint) -2.

    \sa group(), owner(), ownerId()
*/

uint TQFileInfo::groupId() const
{
    if ( !fic || !cache )
	doStat();
    if ( fic )
	return fic->st.st_gid;
    return nobodyID;
}


/*!
    Tests for file permissions. The \a permissionSpec argument can be
    several flags of type \c PermissionSpec OR-ed together to check
    for permission combinations.

    On systems where files do not have permissions this function
    always returns true.

    Example:
    \code
	TQFileInfo fi( "/tmp/archive.tar.gz" );
	if ( fi.permission( TQFileInfo::WriteUser | TQFileInfo::ReadGroup ) )
	    tqWarning( "I can change the file; my group can read the file" );
	if ( fi.permission( TQFileInfo::WriteGroup | TQFileInfo::WriteOther ) )
	    tqWarning( "The group or others can change the file" );
    \endcode

    \sa isReadable(), isWritable(), isExecutable()
*/

bool TQFileInfo::permission( int permissionSpec ) const
{
    if ( !fic || !cache )
	doStat();
    if ( fic ) {
	uint mask = 0;
	if ( permissionSpec & ReadOwner )
	    mask |= S_IRUSR;
	if ( permissionSpec & WriteOwner )
	    mask |= S_IWUSR;
	if ( permissionSpec & ExeOwner )
	    mask |= S_IXUSR;
	if ( permissionSpec & ReadUser )
	    mask |= S_IRUSR;
	if ( permissionSpec & WriteUser )
	    mask |= S_IWUSR;
	if ( permissionSpec & ExeUser )
	    mask |= S_IXUSR;
	if ( permissionSpec & ReadGroup )
	    mask |= S_IRGRP;
	if ( permissionSpec & WriteGroup )
	    mask |= S_IWGRP;
	if ( permissionSpec & ExeGroup )
	    mask |= S_IXGRP;
	if ( permissionSpec & ReadOther )
	    mask |= S_IROTH;
	if ( permissionSpec & WriteOther )
	    mask |= S_IWOTH;
	if ( permissionSpec & ExeOther )
	    mask |= S_IXOTH;
	if ( mask ) {
	   return (fic->st.st_mode & mask) == mask;
	} else {
#if defined(QT_CHECK_NULL)
	   tqWarning( "TQFileInfo::permission: permissionSpec is 0" );
#endif
	   return true;
	}
    } else {
	return false;
    }
}

void TQFileInfo::doStat() const
{
    TQFileInfo *that = ((TQFileInfo*)this);	// mutable function
    if ( !that->fic )
	that->fic = new TQFileInfoCache;
    that->symLink = false;
    struct stat *b = &that->fic->st;
#if defined(Q_OS_UNIX) && defined(S_IFLNK)
    if ( ::lstat( TQFile::encodeName(fn), b ) == 0 ) {
	if ( S_ISLNK( b->st_mode ) )
	    that->symLink = true;
	else
	    return;
    }
#endif

    int r = ::stat( TQFile::encodeName(fn), b );
    if ( r != 0 && !that->symLink ) {
	delete that->fic;
	that->fic = 0;
    }
}

/*!
    Returns the file's path.

    If \a absPath is true an absolute path is returned.

    \sa dir(), filePath(), fileName(), isRelative()
*/
#ifndef TQT_NO_DIR
TQString TQFileInfo::dirPath( bool absPath ) const
{
    TQString s;
    if ( absPath )
	s = absFilePath();
    else
	s = fn;
    int pos = s.findRev( '/' );
    if ( pos == -1 ) {
	return TQString::fromLatin1( "." );
    } else {
	if ( pos == 0 )
	    return TQString::fromLatin1( "/" );
	return s.left( pos );
    }
}
#endif

/*!
    Returns the name of the file, excluding the path.

    Example:
    \code
	TQFileInfo fi( "/tmp/archive.tar.gz" );
	TQString name = fi.fileName();		// name = "archive.tar.gz"
    \endcode

    \sa isRelative(), filePath(), baseName(), extension()
*/

TQString TQFileInfo::fileName() const
{
    int p = fn.findRev( '/' );
    if ( p == -1 ) {
	return fn;
    } else {
	return fn.mid( p + 1 );
    }
}
