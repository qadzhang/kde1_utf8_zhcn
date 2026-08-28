/****************************************************************************
**
** Implementation of TQDir class
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
#include "ntqdir.h"

#ifndef TQT_NO_DIR

#include "qdir_p.h"
#include "ntqfileinfo.h"
#include "ntqregexp.h"
#include "ntqstringlist.h"

#ifdef TQT_THREAD_SUPPORT
#  include <private/qmutexpool_p.h>
#endif // TQT_THREAD_SUPPORT

#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <sys/param.h>


void TQDir::slashify( TQString& )
{
}

TQString TQDir::homeDirPath()
{
    TQString d;
    d = TQFile::decodeName(getenv("HOME"));
    slashify( d );
    if ( d.isNull() )
	d = rootDirPath();
    return d;
}

TQString TQDir::canonicalPath() const
{
    TQString r;
#if !defined(PATH_MAX)
    char *cur = ::get_current_dir_name();
    if ( cur ) {
        char *tmp = canonicalize_file_name( TQFile::encodeName( dPath ).data() );
        if ( tmp ) {
            r = TQFile::decodeName( tmp );
            free( tmp );
        }
        slashify( r );
    	// always make sure we go back to the current dir
        ::chdir( cur );
        free( cur );
    }
#else
    char cur[PATH_MAX+1];
    if ( ::getcwd( cur, PATH_MAX ) ) {
	char tmp[PATH_MAX+1];
	// need the cast for old solaris versions of realpath that doesn't take
	// a const char*.
	if( ::realpath( (char*)TQFile::encodeName( dPath ).data(), tmp ) )
	    r = TQFile::decodeName( tmp );
	slashify( r );

    	// always make sure we go back to the current dir
	if (::chdir( cur ) < 0) {
		// Error!
		// FIXME
	}
    }
#endif  /* !PATH_MAX */
    return r;
}

bool TQDir::mkdir( const TQString &dirName, bool acceptAbsPath ) const
{
#if defined(Q_OS_MACX)  // Mac X doesn't support trailing /'s
    TQString name = dirName;
    if (dirName[dirName.length() - 1] == "/")
	name = dirName.left( dirName.length() - 1 );
    int status =
	::mkdir( TQFile::encodeName(filePath(name,acceptAbsPath)), 0777 );
#else
    int status =
	::mkdir( TQFile::encodeName(filePath(dirName,acceptAbsPath)), 0777 );
#endif
    return status == 0;
}

bool TQDir::rmdir( const TQString &dirName, bool acceptAbsPath ) const
{
    return ::rmdir( TQFile::encodeName(filePath(dirName,acceptAbsPath)) ) == 0;
}

bool TQDir::isReadable() const
{
    return ::access( TQFile::encodeName(dPath), R_OK | X_OK ) == 0;
}

bool TQDir::isRoot() const
{
    return dPath == TQString::fromLatin1("/");
}

bool TQDir::rename( const TQString &name, const TQString &newName,
		   bool acceptAbsPaths	)
{
    if ( name.isEmpty() || newName.isEmpty() ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQDir::rename: Empty or null file name(s)" );
#endif
	return false;
    }
    TQString fn1 = filePath( name, acceptAbsPaths );
    TQString fn2 = filePath( newName, acceptAbsPaths );
    return ::rename( TQFile::encodeName(fn1),
		     TQFile::encodeName(fn2) ) == 0;
}

bool TQDir::setCurrent( const TQString &path )
{
    int r;
    r = ::chdir( TQFile::encodeName(path) );
    return r >= 0;
}

TQString TQDir::currentDirPath()
{
    TQString result;

    struct stat st;
    if ( ::stat( ".", &st ) == 0 ) {
#if !defined(PATH_MAX)
        char *currentName = ::get_current_dir_name();
        if ( currentName ) {
            result = TQFile::decodeName(currentName);
            free( currentName );
        }
#else
	char currentName[PATH_MAX+1];
	if ( ::getcwd( currentName, PATH_MAX ) )
	    result = TQFile::decodeName(currentName);
#endif /* !PATH_MAX */
#if defined(QT_DEBUG)
	if ( result.isNull() )
	    tqWarning( "TQDir::currentDirPath: getcwd() failed" );
#endif
    } else {
#if defined(QT_DEBUG)
	tqWarning( "TQDir::currentDirPath: stat(\".\") failed" );
#endif
    }
    slashify( result );
    return result;
}

TQString TQDir::rootDirPath()
{
    TQString d = TQString::fromLatin1( "/" );
    return d;
}

bool TQDir::isRelativePath( const TQString &path )
{
    int len = path.length();
    if ( len == 0 )
	return true;
    return path[0] != '/';
}

bool TQDir::readDirEntries( const TQString &nameFilter,
			   int filterSpec, int sortSpec )
{
    int i;
    if ( !fList ) {
	fList  = new TQStringList;
	TQ_CHECK_PTR( fList );
	fiList = new TQFileInfoList;
	TQ_CHECK_PTR( fiList );
	fiList->setAutoDelete( true );
    } else {
	fList->clear();
	fiList->clear();
    }

    TQValueList<TQRegExp> filters = tqt_makeFilterList( nameFilter );

    bool doDirs	    = (filterSpec & Dirs)	!= 0;
    bool doFiles    = (filterSpec & Files)	!= 0;
    bool noSymLinks = (filterSpec & NoSymLinks) != 0;
    bool doReadable = (filterSpec & Readable)	!= 0;
    bool doWritable = (filterSpec & Writable)	!= 0;
    bool doExecable = (filterSpec & Executable) != 0;
    bool doHidden   = (filterSpec & Hidden)	!= 0;
    bool doSystem   = (filterSpec & System)     != 0;

    TQFileInfo fi;
    DIR	     *dir;
    dirent   *file;

    dir = opendir( TQFile::encodeName(dPath) );
    if ( !dir )
	return false; // cannot read the directory

#if defined(TQT_THREAD_SUPPORT) && defined(_POSIX_THREAD_SAFE_FUNCTIONS) && !defined(Q_OS_CYGWIN)
    union {
	struct dirent mt_file;
	char b[sizeof(struct dirent) + MAXNAMLEN + 1];
    } u;
    while ( readdir_r(dir, &u.mt_file, &file ) == 0 && file )
#else
    while ( (file = readdir(dir)) )
#endif // TQT_THREAD_SUPPORT && _POSIX_THREAD_SAFE_FUNCTIONS
    {
	TQString fn = TQFile::decodeName(file->d_name);
	fi.setFile( *this, fn );
	if ( !tqt_matchFilterList(filters, fn) && !(allDirs && fi.isDir()) )
	     continue;
	if  ( (doDirs && fi.isDir()) || (doFiles && fi.isFile()) ||
	      (doSystem && (!fi.isFile() && !fi.isDir())) ) {
	    if ( noSymLinks && fi.isSymLink() )
	        continue;
	    if ( (filterSpec & RWEMask) != 0 )
	        if ( (doReadable && !fi.isReadable()) ||
	             (doWritable && !fi.isWritable()) ||
	             (doExecable && !fi.isExecutable()) )
	            continue;
	    if ( !doHidden && fn[0] == '.' &&
	         fn != TQString::fromLatin1(".")
	         && fn != TQString::fromLatin1("..") )
	        continue;
	    fiList->append( new TQFileInfo( fi ) );
	}
    }
    if ( closedir(dir) != 0 ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQDir::readDirEntries: Cannot close the directory: %s",
		  dPath.local8Bit().data() );
#endif
    }

    // Sort...
    if(fiList->count()) {
	TQDirSortItem* si= new TQDirSortItem[fiList->count()];
	TQFileInfo* itm;
	i=0;
	for (itm = fiList->first(); itm; itm = fiList->next())
	    si[i++].item = itm;
	tqt_cmp_si_sortSpec = sortSpec;
	qsort( si, i, sizeof(si[0]), tqt_cmp_si );
	// put them back in the list
	fiList->setAutoDelete( false );
	fiList->clear();
	int j;
	for ( j=0; j<i; j++ ) {
	    fiList->append( si[j].item );
	    fList->append( si[j].item->fileName() );
	}
	delete [] si;
	fiList->setAutoDelete( true );
    }

    if ( filterSpec == (FilterSpec)filtS && sortSpec == (SortSpec)sortS &&
	 nameFilter == nameFilt )
	dirty = false;
    else
	dirty = true;
    return true;
}

const TQFileInfoList * TQDir::drives()
{
    // at most one instance of TQFileInfoList is leaked, and this variable
    // points to that list
    static TQFileInfoList * knownMemoryLeak = 0;

    if ( !knownMemoryLeak ) {

#ifdef TQT_THREAD_SUPPORT
	TQMutexLocker locker( tqt_global_mutexpool ?
			     tqt_global_mutexpool->get( &knownMemoryLeak ) : 0 );
#endif // TQT_THREAD_SUPPORT

	if ( !knownMemoryLeak ) {
	    knownMemoryLeak = new TQFileInfoList;
	    // non-win32 versions both use just one root directory
	    knownMemoryLeak->append( new TQFileInfo( rootDirPath() ) );
	}
    }

    return knownMemoryLeak;
}
#endif //TQT_NO_DIR
