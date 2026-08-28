/****************************************************************************
**
** Implementation of TQDir class
**
** Created : 950427
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
#include <private/qdir_p.h>
#include "ntqfileinfo.h"
#include "ntqregexp.h"
#include "ntqstringlist.h"
#include "ntqdeepcopy.h"
#include <limits.h>

#if defined(Q_FS_FAT) && !defined(Q_OS_UNIX)
const bool CaseSensitiveFS = false;
#else
const bool CaseSensitiveFS = true;
#endif


/*!
    \class TQDir
    \reentrant
    \brief The TQDir class provides access to directory structures and their contents in a platform-independent way.

    \ingroup io
    \mainclass

    A TQDir is used to manipulate path names, access information
    regarding paths and files, and manipulate the underlying file
    system.

    A TQDir can point to a file using either a relative or an absolute
    path. Absolute paths begin with the directory separator "/"
    (optionally preceded by a drive specification under Windows). If
    you always use "/" as a directory separator, TQt will translate
    your paths to conform to the underlying operating system. Relative
    file names begin with a directory name or a file name and specify
    a path relative to the current directory.

    The "current" path refers to the application's working directory.
    A TQDir's own path is set and retrieved with setPath() and path().

    An example of an absolute path is the string "/tmp/quartz", a
    relative path might look like "src/fatlib". You can use the
    function isRelative() to check if a TQDir is using a relative or an
    absolute file path. Call convertToAbs() to convert a relative TQDir
    to an absolute one. For a simplified path use cleanDirPath(). To
    obtain a path which has no symbolic links or redundant ".."
    elements use canonicalPath(). The path can be set with setPath(),
    and changed with cd() and cdUp().

    TQDir provides several static functions, for example, setCurrent()
    to set the application's working directory and currentDirPath() to
    retrieve the application's working directory. Access to some
    common paths is provided with the static functions, current(),
    home() and root() which return TQDir objects or currentDirPath(),
    homeDirPath() and rootDirPath() which return the path as a string.
    If you want to know about your application's path use
    \l{TQApplication::applicationDirPath()}.

    The number of entries in a directory is returned by count().
    Obtain a string list of the names of all the files and directories
    in a directory with entryList(). If you prefer a list of TQFileInfo
    pointers use entryInfoList(). Both these functions can apply a
    name filter, an attributes filter (e.g. read-only, files not
    directories, etc.), and a sort order. The filters and sort may be
    set with calls to setNameFilter(), setFilter() and setSorting().
    They may also be specified in the entryList() and
    entryInfoList()'s arguments.

    Create a new directory with mkdir(), rename a directory with
    rename() and remove an existing directory with rmdir(). Remove a
    file with remove(). You can interrogate a directory with exists(),
    isReadable() and isRoot().

    To get a path with a filename use filePath(), and to get a
    directory name use dirName(); neither of these functions checks
    for the existence of the file or directory.

    The list of root directories is provided by drives(); on Unix
    systems this returns a list containing one root directory, "/"; on
    Windows the list will usually contain "C:/", and possibly "D:/",
    etc.

    It is easiest to work with "/" separators in TQt code. If you need
    to present a path to the user or need a path in a form suitable
    for a function in the underlying operating system use
    convertSeparators().

    Examples:

    See if a directory exists.
    \code
    TQDir d( "example" );			// "./example"
    if ( !d.exists() )
	tqWarning( "Cannot find the example directory" );
    \endcode

    Traversing directories and reading a file.
    \code
    TQDir d = TQDir::root();			// "/"
    if ( !d.cd("tmp") ) {			// "/tmp"
	tqWarning( "Cannot find the \"/tmp\" directory" );
    } else {
	TQFile f( d.filePath("ex1.txt") );	// "/tmp/ex1.txt"
	if ( !f.open(IO_ReadWrite) )
	    tqWarning( "Cannot create the file %s", f.name() );
    }
    \endcode

    A program that lists all the files in the current directory
    (excluding symbolic links), sorted by size, smallest first:
    \code
    #include <stdio.h>
    #include <ntqdir.h>

    int main( int argc, char **argv )
    {
	TQDir d;
	d.setFilter( TQDir::Files | TQDir::Hidden | TQDir::NoSymLinks );
	d.setSorting( TQDir::Size | TQDir::Reversed );

	const TQFileInfoList *list = d.entryInfoList();
	TQFileInfoListIterator it( *list );
	TQFileInfo *fi;

	printf( "     Bytes Filename\n" );
	while ( (fi = it.current()) != 0 ) {
	    printf( "%10li %s\n", fi->size(), fi->fileName().latin1() );
	    ++it;
	}
	return 0;
    }
    \endcode

    \sa TQApplication::applicationDirPath()
*/

/*!
    Constructs a TQDir pointing to the current directory (".").

    \sa currentDirPath()
*/

TQDir::TQDir()
{
    dPath = TQString::fromLatin1(".");
    init();
}

/*!
    Constructs a TQDir with path \a path, that filters its entries by
    name using \a nameFilter and by attributes using \a filterSpec. It
    also sorts the names using \a sortSpec.

    The default \a nameFilter is an empty string, which excludes
    nothing; the default \a filterSpec is \c All, which also means
    exclude nothing. The default \a sortSpec is \c Name|IgnoreCase,
    i.e. sort by name case-insensitively.

    Example that lists all the files in "/tmp":
    \code
    TQDir d( "/tmp" );
    for ( int i = 0; i < d.count(); i++ )
	printf( "%s\n", d[i] );
    \endcode

    If \a path is "" or TQString::null, TQDir uses "." (the current
    directory). If \a nameFilter is "" or TQString::null, TQDir uses the
    name filter "*" (all files).

    Note that \a path need not exist.

    \sa exists(), setPath(), setNameFilter(), setFilter(), setSorting()
*/

TQDir::TQDir( const TQString &path, const TQString &nameFilter,
	    int sortSpec, int filterSpec )
{
    init();
    dPath = cleanDirPath( path );
    if ( dPath.isEmpty() )
	dPath = TQString::fromLatin1(".");
    nameFilt = nameFilter;
    if ( nameFilt.isEmpty() )
	nameFilt = TQString::fromLatin1("*");
    filtS = (FilterSpec)filterSpec;
    sortS = (SortSpec)sortSpec;
}

/*!
    Constructs a TQDir that is a copy of the directory \a d.

    \sa operator=()
*/

TQDir::TQDir( const TQDir &d )
{
    dPath = d.dPath;
    fList = 0;
    fiList = 0;
    nameFilt = d.nameFilt;
    dirty = true;
    allDirs = d.allDirs;
    filtS = d.filtS;
    sortS = d.sortS;
}

/*!
    Refreshes the directory information.
*/
void TQDir::refresh() const
{
    TQDir* that = (TQDir*) this;
    that->dirty = true;
}

void TQDir::init()
{
    fList = 0;
    fiList = 0;
    nameFilt = TQString::fromLatin1("*");
    dirty = true;
    allDirs = false;
    filtS = All;
    sortS = SortSpec(Name | IgnoreCase);
}

/*!
    Destroys the TQDir frees up its resources.
*/

TQDir::~TQDir()
{
    delete fList;
    delete fiList;
}


/*!
    Sets the path of the directory to \a path. The path is cleaned of
    redundant ".", ".." and of multiple separators. No check is made
    to ensure that a directory with this path exists.

    The path can be either absolute or relative. Absolute paths begin
    with the directory separator "/" (optionally preceded by a drive
    specification under Windows). Relative file names begin with a
    directory name or a file name and specify a path relative to the
    current directory. An example of an absolute path is the string
    "/tmp/quartz", a relative path might look like "src/fatlib".

    \sa path(), absPath(), exists(), cleanDirPath(), dirName(),
      absFilePath(), isRelative(), convertToAbs()
*/

void TQDir::setPath( const TQString &path )
{
    dPath = cleanDirPath( path );
    if ( dPath.isEmpty() )
	dPath = TQString::fromLatin1(".");
    dirty = true;
}

/*!
    \fn  TQString TQDir::path() const

    Returns the path, this may contain symbolic links, but never
    contains redundant ".", ".." or multiple separators.

    The returned path can be either absolute or relative (see
    setPath()).

    \sa setPath(), absPath(), exists(), cleanDirPath(), dirName(),
    absFilePath(), convertSeparators()
*/

/*!
    Returns the absolute path (a path that starts with "/" or with a
    drive specification), which may contain symbolic links, but never
    contains redundant ".", ".." or multiple separators.

    \sa setPath(), canonicalPath(), exists(),  cleanDirPath(),
    dirName(), absFilePath()
*/

TQString TQDir::absPath() const
{
    if ( TQDir::isRelativePath(dPath) ) {
	TQString tmp = currentDirPath();
	if ( tmp.right(1) != TQString::fromLatin1("/") )
	    tmp += '/';
	tmp += dPath;
	return cleanDirPath( tmp );
    } else {
	return cleanDirPath( dPath );
    }
}

/*!
    Returns the name of the directory; this is \e not the same as the
    path, e.g. a directory with the name "mail", might have the path
    "/var/spool/mail". If the directory has no name (e.g. it is the
    root directory) TQString::null is returned.

    No check is made to ensure that a directory with this name
    actually exists.

    \sa path(), absPath(), absFilePath(), exists(), TQString::isNull()
*/

TQString TQDir::dirName() const
{
    int pos = dPath.findRev( '/' );
    if ( pos == -1  )
	return dPath;
    return dPath.right( dPath.length() - pos - 1 );
}

/*!
    Returns the path name of a file in the directory. Does \e not
    check if the file actually exists in the directory. If the TQDir is
    relative the returned path name will also be relative. Redundant
    multiple separators or "." and ".." directories in \a fileName
    will not be removed (see cleanDirPath()).

    If \a acceptAbsPath is true a \a fileName starting with a
    separator "/" will be returned without change. If \a acceptAbsPath
    is false an absolute path will be prepended to the fileName and
    the resultant string returned.

    \sa absFilePath(), isRelative(), canonicalPath()
*/

TQString TQDir::filePath( const TQString &fileName,
			bool acceptAbsPath ) const
{
    if ( acceptAbsPath && !isRelativePath(fileName) )
	return TQString(fileName);

    TQString tmp = dPath;
    if ( tmp.isEmpty() || (tmp[(int)tmp.length()-1] != '/' && !!fileName &&
			   fileName[0] != '/') )
	tmp += '/';
    tmp += fileName;
    return tmp;
}

/*!
    Returns the absolute path name of a file in the directory. Does \e
    not check if the file actually exists in the directory. Redundant
    multiple separators or "." and ".." directories in \a fileName
    will not be removed (see cleanDirPath()).

    If \a acceptAbsPath is true a \a fileName starting with a
    separator "/" will be returned without change. If \a acceptAbsPath
    is false an absolute path will be prepended to the fileName and
    the resultant string returned.

    \sa filePath()
*/

TQString TQDir::absFilePath( const TQString &fileName,
			   bool acceptAbsPath ) const
{
    if ( acceptAbsPath && !isRelativePath( fileName ) )
	return fileName;

    TQString tmp = absPath();
#ifdef Q_OS_WIN32
    if ( fileName[0].isLetter() && fileName[1] == ':' ) {
	int drv = fileName.upper()[0].latin1() - 'A' + 1;
	if ( _getdrive() != drv ) {
	    QT_WA( {
		TCHAR buf[PATH_MAX];
		::_wgetdcwd( drv, buf, PATH_MAX );
		tmp.setUnicodeCodes( (ushort*)buf, (uint)::wcslen(buf) );
	    }, {
		char buf[PATH_MAX];
		::_getdcwd( drv, buf, PATH_MAX );
		tmp = buf;
	    } );
	    if ( !tmp.endsWith("\\") )
		tmp += "\\";
	    tmp += fileName.right( fileName.length() - 2 );
	    int x;
	    for ( x = 0; x < (int) tmp.length(); x++ ) {
		if ( tmp[x] == '\\' )
		    tmp[x] = '/';
	    }
	}
    } else
#endif
    {
	if ( tmp.isEmpty() || (tmp[(int)tmp.length()-1] != '/' && !!fileName &&
			       fileName[0] != '/') )
	    tmp += '/';
	tmp += fileName;
    }
    return tmp;
}


/*!
    Returns \a pathName with the '/' separators converted to
    separators that are appropriate for the underlying operating
    system.

    On Windows, convertSeparators("c:/winnt/system32") returns
    "c:\winnt\system32".

    The returned string may be the same as the argument on some
    operating systems, for example on Unix.
*/

TQString TQDir::convertSeparators( const TQString &pathName )
{
    TQString n( pathName );
#if defined(Q_FS_FAT) || defined(Q_OS_OS2EMX)
    for ( int i=0; i<(int)n.length(); i++ ) {
	if ( n[i] == '/' )
	    n[i] = '\\';
    }
#elif defined(Q_OS_MAC9)
	while(n.length() && n[0] == '/' ) n = n.right(n.length()-1);
    for ( int i=0; i<(int)n.length(); i++ ) {
	if ( n[i] == '/' )
	    n[i] = ':';
    }
    if(n.contains(':') && n.left(1) != ':')
	n.prepend(':');
#endif
    return n;
}


/*!
    Changes the TQDir's directory to \a dirName.

    If \a acceptAbsPath is true a path starting with separator "/"
    will cause the function to change to the absolute directory. If \a
    acceptAbsPath is false any number of separators at the beginning
    of \a dirName will be removed and the function will descend into
    \a dirName.

    Returns true if the new directory exists and is readable;
    otherwise returns false. Note that the logical cd() operation is
    not performed if the new directory does not exist.

    Calling cd( ".." ) is equivalent to calling cdUp().

    \sa cdUp(), isReadable(), exists(), path()
*/

bool TQDir::cd( const TQString &dirName, bool acceptAbsPath )
{
    if ( dirName.isEmpty() || dirName == TQString::fromLatin1(".") )
	return true;
    TQString old = dPath;
    if ( acceptAbsPath && !isRelativePath(dirName) ) {
	dPath = cleanDirPath( dirName );
    } else {
	if ( isRoot() ) {
	    if ( dirName == ".." ) {
		dPath = old;
		return false;
	    }
	} else {
	    dPath += '/';
	}

	dPath += dirName;
	if ( dirName.find('/') >= 0
		|| old == TQString::fromLatin1(".")
		|| dirName == TQString::fromLatin1("..") ) {
	    dPath = cleanDirPath( dPath );

	    /*
	      If dPath starts with .., we convert it to absolute to
	      avoid infinite looping on

		  TQDir dir( "." );
		  while ( dir.cdUp() )
		      ;
	    */
	    if ( dPath[0] == TQChar('.') && dPath[1] == TQChar('.') &&
		 (dPath.length() == 2 || dPath[2] == TQChar('/')) )
		convertToAbs();
	}
    }
    if ( !exists() ) {
	dPath = old;			// regret
	return false;
    }
    dirty = true;
    return true;
}

/*!
    Changes directory by moving one directory up from the TQDir's
    current directory.

    Returns true if the new directory exists and is readable;
    otherwise returns false. Note that the logical cdUp() operation is
    not performed if the new directory does not exist.

    \sa cd(), isReadable(), exists(), path()
*/

bool TQDir::cdUp()
{
    return cd( TQString::fromLatin1("..") );
}

/*!
    \fn TQString TQDir::nameFilter() const

    Returns the string set by setNameFilter()
*/

/*!
    Sets the name filter used by entryList() and entryInfoList() to \a
    nameFilter.

    The \a nameFilter is a wildcard (globbing) filter that understands
    "*" and "?" wildcards. (See \link ntqregexp.html#wildcard-matching
    TQRegExp wildcard matching\endlink.) You may specify several filter
    entries all separated by a single space " " or by a semi-colon
    ";".

    For example, if you want entryList() and entryInfoList() to list
    all files ending with either ".cpp" or ".h", you would use either
    dir.setNameFilter("*.cpp *.h") or dir.setNameFilter("*.cpp;*.h").

    \sa nameFilter(), setFilter()
*/

void TQDir::setNameFilter( const TQString &nameFilter )
{
    nameFilt = nameFilter;
    if ( nameFilt.isEmpty() )
	nameFilt = TQString::fromLatin1("*");
    dirty = true;
}

/*!
    \fn TQDir::FilterSpec TQDir::filter() const

    Returns the value set by setFilter()
*/

/*!
    \enum TQDir::FilterSpec

    This enum describes the filtering options available to TQDir, e.g.
    for entryList() and entryInfoList(). The filter value is specified
    by OR-ing together values from the following list:

    \value Dirs  List directories only.
    \value Files  List files only.
    \value  Drives  List disk drives (ignored under Unix).
    \value  NoSymLinks  Do not list symbolic links (ignored by operating
    systems that don't support symbolic links).
    \value All List directories, files, drives and symlinks (this does not list
    broken symlinks unless you specify System).
    \value TypeMask A mask for the the Dirs, Files, Drives and
    NoSymLinks flags.
    \value  Readable  List files for which the application has read access.
    \value  Writable  List files for which the application has write access.
    \value  Executable  List files for which the application has execute
    access. Executables needs to be combined with Dirs or Files.
    \value  RWEMask A mask for the Readable, Writable and Executable flags.
    \value  Modified  Only list files that have been modified (ignored
    under Unix).
    \value  Hidden  List hidden files (on Unix, files starting with a .).
    \value  System  List system files (on Unix, FIFOs, sockets and
    device files)
    \value AccessMask A mask for the Readable, Writable, Executable
    Modified, Hidden and System flags
    \value DefaultFilter Internal flag.

    If you do not set any of \c Readable, \c Writable or \c
    Executable, TQDir will set all three of them. This makes the
    default easy to write and at the same time useful.

    Examples: \c Readable|Writable means list all files for which the
    application has read access, write access or both. \c Dirs|Drives
    means list drives, directories, all files that the application can
    read, write or execute, and also symlinks to such
    files/directories.
*/


/*!
    Sets the filter used by entryList() and entryInfoList() to \a
    filterSpec. The filter is used to specify the kind of files that
    should be returned by entryList() and entryInfoList(). See
    \l{TQDir::FilterSpec}.

    \sa filter(), setNameFilter()
*/

void TQDir::setFilter( int filterSpec )
{
    if ( filtS == (FilterSpec) filterSpec )
	return;
    filtS = (FilterSpec) filterSpec;
    dirty = true;
}

/*!
    \fn TQDir::SortSpec TQDir::sorting() const

    Returns the value set by setSorting()

    \sa setSorting() SortSpec
*/

/*!
    \enum TQDir::SortSpec

    This enum describes the sort options available to TQDir, e.g. for
    entryList() and entryInfoList(). The sort value is specified by
    OR-ing together values from the following list:

    \value Name  Sort by name.
    \value Time  Sort by time (modification time).
    \value Size  Sort by file size.
    \value Unsorted  Do not sort.
    \value SortByMask A mask for Name, Time and Size.

    \value DirsFirst  Put the directories first, then the files.
    \value Reversed  Reverse the sort order.
    \value IgnoreCase  Sort case-insensitively.
    \value LocaleAware  Sort names using locale aware compares
    \value DefaultSort Internal flag.

    You can only specify one of the first four.

    If you specify both \c DirsFirst and \c Reversed, directories are
    still put first, but in reverse order; the files will be listed
    after the directories, again in reverse order.
*/

// ### Unsorted+DirsFirst ? Unsorted+Reversed?

/*!
    Sets the sort order used by entryList() and entryInfoList().

    The \a sortSpec is specified by OR-ing values from the enum
    \l{TQDir::SortSpec}.

    \sa sorting() SortSpec
*/

void TQDir::setSorting( int sortSpec )
{
    if ( sortS == (SortSpec) sortSpec )
	return;
    sortS = (SortSpec) sortSpec;
    dirty = true;
}

/*!
    \fn bool TQDir::matchAllDirs() const

    Returns the value set by setMatchAllDirs()

    \sa setMatchAllDirs()
*/

/*!
    If \a enable is true then all directories are included (e.g. in
    entryList()), and the nameFilter() is only applied to the files.
    If \a enable is false then the nameFilter() is applied to both
    directories and files.

    \sa matchAllDirs()
*/

void TQDir::setMatchAllDirs( bool enable )
{
    if ( (bool)allDirs == enable )
	return;
    allDirs = enable;
    dirty = true;
}


/*!
    Returns the total number of directories and files that were found.

    Equivalent to entryList().count().

    \sa operator[](), entryList()
*/

uint TQDir::count() const
{
    return (uint)entryList().count();
}

/*!
    Returns the file name at position \a index in the list of file
    names. Equivalent to entryList().at(index).

    Returns a TQString::null if the \a index is out of range or if the
    entryList() function failed.

    \sa count(), entryList()
*/

TQString TQDir::operator[]( int index ) const
{
    entryList();
    return fList && index >= 0 && index < (int)fList->count() ?
	(*fList)[index] : TQString::null;
}


/*!
    \obsolete
  This function is included to easy porting from TQt 1.x to TQt 2.0,
  it is the same as entryList(), but encodes the filenames as 8-bit
  strings using TQFile::encodedName().

  It is more efficient to use entryList().
*/
TQStrList TQDir::encodedEntryList( int filterSpec, int sortSpec ) const
{
    TQStrList r;
    TQStringList l = entryList(filterSpec,sortSpec);
    for ( TQStringList::Iterator it = l.begin(); it != l.end(); ++it ) {
	r.append( TQFile::encodeName(*it) );
    }
    return r;
}

/*!
    \obsolete
    \overload
  This function is included to easy porting from TQt 1.x to TQt 2.0,
  it is the same as entryList(), but encodes the filenames as 8-bit
  strings using TQFile::encodedName().

  It is more efficient to use entryList().
*/
TQStrList TQDir::encodedEntryList( const TQString &nameFilter,
			   int filterSpec,
			   int sortSpec ) const
{
    TQStrList r;
    TQStringList l = entryList(nameFilter,filterSpec,sortSpec);
    for ( TQStringList::Iterator it = l.begin(); it != l.end(); ++it ) {
	r.append( TQFile::encodeName(*it) );
    }
    return r;
}



/*!
    \overload

    Returns a list of the names of all the files and directories in
    the directory, ordered in accordance with setSorting() and
    filtered in accordance with setFilter() and setNameFilter().

    The filter and sorting specifications can be overridden using the
    \a filterSpec and \a sortSpec arguments.

    Returns an empty list if the directory is unreadable or does not
    exist.

    \sa entryInfoList(), setNameFilter(), setSorting(), setFilter()
*/

TQStringList TQDir::entryList( int filterSpec, int sortSpec ) const
{
    if ( !dirty && filterSpec == (int)DefaultFilter &&
		   sortSpec   == (int)DefaultSort )
	return *fList;
    return entryList( nameFilt, filterSpec, sortSpec );
}

/*!
    Returns a list of the names of all the files and directories in
    the directory, ordered in accordance with setSorting() and
    filtered in accordance with setFilter() and setNameFilter().

    The filter and sorting specifications can be overridden using the
    \a nameFilter, \a filterSpec and \a sortSpec arguments.

    Returns an empty list if the directory is unreadable or does not
    exist.

    \sa entryInfoList(), setNameFilter(), setSorting(), setFilter()
*/

TQStringList TQDir::entryList( const TQString &nameFilter,
				 int filterSpec, int sortSpec ) const
{
    if ( filterSpec == (int)DefaultFilter )
	filterSpec = filtS;
    if ( sortSpec == (int)DefaultSort )
	sortSpec = sortS;
    TQDir *that = (TQDir*)this;			// mutable function
    if ( that->readDirEntries(nameFilter, filterSpec, sortSpec) ) {
	if ( that->fList )
	    return *that->fList;
    }
    return TQStringList();
}

/*!
    \overload

    Returns a list of TQFileInfo objects for all the files and
    directories in the directory, ordered in accordance with
    setSorting() and filtered in accordance with setFilter() and
    setNameFilter().

    The filter and sorting specifications can be overridden using the
    \a filterSpec and \a sortSpec arguments.

    Returns 0 if the directory is unreadable or does not exist.

    The returned pointer is a const pointer to a TQFileInfoList. The
    list is owned by the TQDir object and will be reused on the next
    call to entryInfoList() for the same TQDir instance. If you want to
    keep the entries of the list after a subsequent call to this
    function you must copy them.

    Note: TQFileInfoList is really a TQPtrList<TQFileInfo>.

    \sa entryList(), setNameFilter(), setSorting(), setFilter()
*/

const TQFileInfoList *TQDir::entryInfoList( int filterSpec, int sortSpec ) const
{
    if ( !dirty && filterSpec == (int)DefaultFilter &&
		   sortSpec   == (int)DefaultSort )
	return fiList;
    return entryInfoList( nameFilt, filterSpec, sortSpec );
}

/*!
    Returns a list of TQFileInfo objects for all the files and
    directories in the directory, ordered in accordance with
    setSorting() and filtered in accordance with setFilter() and
    setNameFilter().

    The filter and sorting specifications can be overridden using the
    \a nameFilter, \a filterSpec and \a sortSpec arguments.

    Returns 0 if the directory is unreadable or does not exist.

    The returned pointer is a const pointer to a TQFileInfoList. The
    list is owned by the TQDir object and will be reused on the next
    call to entryInfoList() for the same TQDir instance. If you want to
    keep the entries of the list after a subsequent call to this
    function you must copy them.

    Note: TQFileInfoList is really a TQPtrList<TQFileInfo>.

    \sa entryList(), setNameFilter(), setSorting(), setFilter()
*/

const TQFileInfoList *TQDir::entryInfoList( const TQString &nameFilter,
					  int filterSpec, int sortSpec ) const
{
    if ( filterSpec == (int)DefaultFilter )
	filterSpec = filtS;
    if ( sortSpec == (int)DefaultSort )
	sortSpec = sortS;
    TQDir *that = (TQDir*)this;			// mutable function
    if ( that->readDirEntries(nameFilter, filterSpec, sortSpec) )
	return that->fiList;
    else
	return 0;
}

/*!
    \overload

    Returns true if the \e directory exists; otherwise returns false.
    (If a file with the same name is found this function will return
    false).

    \sa TQFileInfo::exists(), TQFile::exists()
*/

bool TQDir::exists() const
{
    TQFileInfo fi( dPath );
    return fi.exists() && fi.isDir();
}

/*!
    Returns true if the directory path is relative to the current
    directory and returns false if the path is absolute (e.g. under
    UNIX a path is relative if it does not start with a "/").

    \sa convertToAbs()
*/

bool TQDir::isRelative() const
{
    return isRelativePath( dPath );
}

/*!
    Converts the directory path to an absolute path. If it is already
    absolute nothing is done.

    \sa isRelative()
*/

void TQDir::convertToAbs()
{
    dPath = absPath();
}

/*!
    Makes a copy of TQDir \a d and assigns it to this TQDir.
*/

TQDir &TQDir::operator=( const TQDir &d )
{
    dPath    = d.dPath;
    delete fList;
    fList    = 0;
    delete fiList;
    fiList   = 0;
    nameFilt = d.nameFilt;
    dirty    = true;
    allDirs  = d.allDirs;
    filtS    = d.filtS;
    sortS    = d.sortS;
    return *this;
}

/*!
    \overload

    Sets the directory path to be the given \a path.
*/

TQDir &TQDir::operator=( const TQString &path )
{
    dPath = cleanDirPath( path );
    dirty = true;
    return *this;
}


/*!
    \fn bool TQDir::operator!=( const TQDir &d ) const

    Returns true if directory \a d and this directory have different
    paths or different sort or filter settings; otherwise returns
    false.

    Example:
    \code
    // The current directory is "/usr/local"
    TQDir d1( "/usr/local/bin" );
    TQDir d2( "bin" );
    if ( d1 != d2 )
	tqDebug( "They differ" );
    \endcode
*/

/*!
    Returns true if directory \a d and this directory have the same
    path and their sort and filter settings are the same; otherwise
    returns false.

    Example:
    \code
    // The current directory is "/usr/local"
    TQDir d1( "/usr/local/bin" );
    TQDir d2( "bin" );
    d2.convertToAbs();
    if ( d1 == d2 )
	tqDebug( "They're the same" );
    \endcode
*/

bool TQDir::operator==( const TQDir &d ) const
{
    return dPath    == d.dPath &&
	   nameFilt == d.nameFilt &&
	   allDirs  == d.allDirs &&
	   filtS    == d.filtS &&
	   sortS    == d.sortS;
}


/*!
    Removes the file, \a fileName.

    If \a acceptAbsPath is true a path starting with separator "/"
    will remove the file with the absolute path. If \a acceptAbsPath
    is false any number of separators at the beginning of \a fileName
    will be removed and the resultant file name will be removed.

    Returns true if the file is removed successfully; otherwise
    returns false.
*/

bool TQDir::remove( const TQString &fileName, bool acceptAbsPath )
{
    if ( fileName.isEmpty() ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQDir::remove: Empty or null file name" );
#endif
	return false;
    }
    TQString p = filePath( fileName, acceptAbsPath );
    return TQFile::remove( p );
}

/*!
    Checks for the existence of the file \a name.

    If \a acceptAbsPath is true a path starting with separator "/"
    will check the file with the absolute path. If \a acceptAbsPath is
    false any number of separators at the beginning of \a name will be
    removed and the resultant file name will be checked.

    Returns true if the file exists; otherwise returns false.

    \sa TQFileInfo::exists(), TQFile::exists()
*/

bool TQDir::exists( const TQString &name, bool acceptAbsPath ) //### const in 4.0
{
    if ( name.isEmpty() ) {
#if defined(QT_CHECK_NULL)
	tqWarning( "TQDir::exists: Empty or null file name" );
#endif
	return false;
    }
    TQString tmp = filePath( name, acceptAbsPath );
    return TQFile::exists( tmp );
}

/*!
    Returns the native directory separator; "/" under UNIX (including
    Mac OS X) and "\" under Windows.

    You do not need to use this function to build file paths. If you
    always use "/", TQt will translate your paths to conform to the
    underlying operating system.
*/

char TQDir::separator()
{
#if defined(Q_OS_UNIX)
    return '/';
#elif defined (Q_FS_FAT) || defined(TQ_WS_WIN)
    return '\\';
#elif defined (Q_OS_MAC)
    return ':';
#else
    return '/';
#endif
}

/*!
    Returns the application's current directory.

    Use path() to access a TQDir object's path.

    \sa currentDirPath(), TQDir::TQDir()
*/

TQDir TQDir::current()
{
    return TQDir( currentDirPath() );
}

/*!
    Returns the home directory.

    Under Windows the \c HOME environment variable is used. If this
    does not exist the \c USERPROFILE environment variable is used. If
    that does not exist the path is formed by concatenating the \c
    HOMEDRIVE and \c HOMEPATH environment variables. If they don't
    exist the rootDirPath() is used (this uses the \c SystemDrive
    environment variable). If none of these exist "C:\" is used.

    Under non-Windows operating systems the \c HOME environment
    variable is used if it exists, otherwise rootDirPath() is used.

    \sa homeDirPath()
*/

TQDir TQDir::home()
{
    return TQDir( homeDirPath() );
}

/*!
    Returns the root directory.

    \sa rootDirPath() drives()
*/

TQDir TQDir::root()
{
    return TQDir( rootDirPath() );
}

/*!
    \fn TQString TQDir::homeDirPath()

    Returns the absolute path of the user's home directory.

    \sa home()
*/

TQValueList<TQRegExp> tqt_makeFilterList( const TQString &filter )
{
    TQValueList<TQRegExp> regExps;
    if ( filter.isEmpty() )
	return regExps;

    TQChar sep( ';' );
    int i = filter.find( sep, 0 );
    if ( i == -1 && filter.find( ' ', 0 ) != -1 )
	sep = TQChar( ' ' );

    TQStringList list = TQStringList::split( sep, filter );
    TQStringList::Iterator it = list.begin();
    while ( it != list.end() ) {
	regExps << TQRegExp( (*it).stripWhiteSpace(), CaseSensitiveFS, true );
	++it;
    }
    return regExps;
}

bool tqt_matchFilterList( const TQValueList<TQRegExp>& filters,
			 const TQString &fileName )
{
    TQValueList<TQRegExp>::ConstIterator rit = filters.begin();
    while ( rit != filters.end() ) {
	if ( (*rit).exactMatch(fileName) )
	    return true;
	++rit;
    }
    return false;
}


/*!
    \overload

    Returns true if the \a fileName matches any of the wildcard (glob)
    patterns in the list of \a filters; otherwise returns false.

    (See \link ntqregexp.html#wildcard-matching TQRegExp wildcard
    matching.\endlink)
    \sa TQRegExp::match()
*/

bool TQDir::match( const TQStringList &filters, const TQString &fileName )
{
    TQStringList::ConstIterator sit = filters.begin();
    while ( sit != filters.end() ) {
	TQRegExp rx( *sit, CaseSensitiveFS, true );
	if ( rx.exactMatch(fileName) )
	    return true;
	++sit;
    }
    return false;
}

/*!
    Returns true if the \a fileName matches the wildcard (glob)
    pattern \a filter; otherwise returns false. The \a filter may
    contain multiple patterns separated by spaces or semicolons.

    (See \link ntqregexp.html#wildcard-matching TQRegExp wildcard
    matching.\endlink)
    \sa TQRegExp::match()
*/

bool TQDir::match( const TQString &filter, const TQString &fileName )
{
    return tqt_matchFilterList( tqt_makeFilterList(filter), fileName );
}


/*!
    Removes all multiple directory separators "/" and resolves any
    "."s or ".."s found in the path, \a filePath.

    Symbolic links are kept. This function does not return the
    canonical path, but rather the simplest version of the input.
    For example, "./local" becomes "local", "local/../bin" becomes
    "bin" and "/local/usr/../bin" becomes "/local/bin".

    \sa absPath() canonicalPath()
*/

TQString TQDir::cleanDirPath( const TQString &filePath )
{
    TQString name = filePath;
    TQString newPath;

    if ( name.isEmpty() )
	return name;

    slashify( name );

    bool addedSeparator = isRelativePath( name );
    if ( addedSeparator )
	name.insert( 0, '/' );

    int ePos, pos, upLevel;

    pos = ePos = name.length();
    upLevel = 0;
    int len;

    while ( pos && (pos = name.findRev('/', pos - 1)) != -1 ) {
	len = ePos - pos - 1;
	if ( len == 2 && name.at(pos + 1) == '.'
		      && name.at(pos + 2) == '.' ) {
	    upLevel++;
	} else {
	    if ( len != 0 && (len != 1 || name.at(pos + 1) != '.') ) {
		if ( !upLevel )
		    newPath = TQString::fromLatin1("/")
			+ name.mid(pos + 1, len) + newPath;
		else
		    upLevel--;
	    }
	}
	ePos = pos;
    }
    if ( addedSeparator ) {
	while ( upLevel-- )
	    newPath.insert( 0, TQString::fromLatin1("/..") );
	if ( !newPath.isEmpty() )
	    newPath.remove( (uint)0, (uint)1 );
	else
	    newPath = TQString::fromLatin1(".");
    } else {
	if ( newPath.isEmpty() )
	    newPath = TQString::fromLatin1("/");
#if defined(Q_FS_FAT) || defined(Q_OS_OS2EMX)
	if ( name[0] == '/' ) {
	    if ( name[1] == '/' )		// "\\machine\x\ ..."
		newPath.insert( 0, '/' );
	} else {
	    newPath = name.left(2) + newPath;
	}
#endif
    }
    return newPath;
}

int tqt_cmp_si_sortSpec;

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

#ifdef Q_OS_TEMP
int __cdecl tqt_cmp_si( const void *n1, const void *n2 )
#else
int tqt_cmp_si( const void *n1, const void *n2 )
#endif
{
    if ( !n1 || !n2 )
	return 0;

    TQDirSortItem* f1 = (TQDirSortItem*)n1;
    TQDirSortItem* f2 = (TQDirSortItem*)n2;

    if ( tqt_cmp_si_sortSpec & TQDir::DirsFirst )
	if ( f1->item->isDir() != f2->item->isDir() )
	    return f1->item->isDir() ? -1 : 1;

    int r = 0;
    int sortBy = tqt_cmp_si_sortSpec & TQDir::SortByMask;

    switch ( sortBy ) {
      case TQDir::Time:
	r = f1->item->lastModified().secsTo(f2->item->lastModified());
	break;
      case TQDir::Size:
	r = f2->item->size() - f1->item->size();
	break;
      default:
	;
    }

    if ( r == 0 && sortBy != TQDir::Unsorted ) {
	// Still not sorted - sort by name
	bool ic = tqt_cmp_si_sortSpec & TQDir::IgnoreCase;

	if ( f1->filename_cache.isNull() )
	    f1->filename_cache = ic ? f1->item->fileName().lower()
				    : f1->item->fileName();
	if ( f2->filename_cache.isNull() )
	    f2->filename_cache = ic ? f2->item->fileName().lower()
				    : f2->item->fileName();

	r = tqt_cmp_si_sortSpec & TQDir::LocaleAware
            ? f1->filename_cache.localeAwareCompare(f2->filename_cache)
            : f1->filename_cache.compare(f2->filename_cache);
    }

    if ( r == 0 ) {
	// Enforce an order - the order the items appear in the array
	r = (char*)n1 - (char*)n2;
    }

    if ( tqt_cmp_si_sortSpec & TQDir::Reversed )
	return -r;
    else
	return r;
}

#if defined(Q_C_CALLBACKS)
}
#endif

/*! \internal
    Detaches all internal data.
*/
void TQDir::detach()
{
    // deepcopy
    dPath = TQDeepCopy<TQString>(dPath);
    nameFilt = TQDeepCopy<TQString>(nameFilt);

    if ( fList )
	*fList = TQDeepCopy<TQStringList>( *fList );

    if ( fiList ) {
	TQFileInfoList *newlist = new TQFileInfoList( *fiList );
	delete fiList;
	fiList = newlist;
    }
}

#endif // TQT_NO_DIR
