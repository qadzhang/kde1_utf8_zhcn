/****************************************************************************
**
** Implementation of TQSettings class
**
** Created : 000626
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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
#include <stdlib.h>

// POSIX Large File Support redefines open -> open64
static inline int tqt_open( const char *pathname, int flags, mode_t mode )
{ return ::open( pathname, flags, mode ); }
#if defined(open)
# undef open
#endif

// POSIX Large File Support redefines truncate -> truncate64
#if defined(truncate)
# undef truncate
#endif

#include "ntqsettings.h"

#ifndef TQT_NO_SETTINGS

#include "ntqdir.h"
#include "ntqfile.h"
#include "ntqfileinfo.h"
#include "ntqmap.h"
#include "ntqtextstream.h"
#include "ntqregexp.h"
#include <private/qsettings_p.h>
#ifndef NO_ERRNO_H
#include <errno.h>
#endif

/*!
    \class TQSettings
    \brief The TQSettings class provides persistent platform-independent application settings.

    \ingroup io
    \ingroup misc
    \mainclass

    On Unix systems, TQSettings uses text files to store settings. On Windows
    systems, TQSettings uses the system registry.  On Mac OS X, TQSettings uses
    the Carbon preferences API.

    Each setting comprises an identifying key and the data associated with
    the key. A key is a unicode string which consists of \e two or more
    subkeys. A subkey is a slash, '/', followed by one or more unicode
    characters (excluding slashes, newlines, carriage returns and equals,
    '=', signs). The associated data, called the entry or value, may be a
    boolean, an integer, a double, a string or a list of strings. Entry
    strings may contain any unicode characters.

    If you want to save and restore the entire desktop's settings, i.e.
    which applications are running, use TQSettings to save the settings
    for each individual application and TQSessionManager to save the
    desktop's session.

    Example settings:
    \code
    /MyCompany/MyApplication/background color
    /MyCompany/MyApplication/foreground color
    /MyCompany/MyApplication/geometry/x
    /MyCompany/MyApplication/geometry/y
    /MyCompany/MyApplication/geometry/width
    /MyCompany/MyApplication/geometry/height
    /MyCompany/MyApplication/recent files/1
    /MyCompany/MyApplication/recent files/2
    /MyCompany/MyApplication/recent files/3
    \endcode
    Each line above is a complete key, made up of subkeys.

    A typical usage pattern for reading settings at application
    startup:
    \code
    TQSettings settings;
    settings.setPath( "MyCompany.com", "MyApplication" );

    TQString bgColor = settings.readEntry( "/colors/background", "white" );
    int width = settings.readNumEntry( "/geometry/width", 640 );
    // ...
    \endcode

    A typical usage pattern for saving settings at application exit or
    'save preferences':
    \code
    TQSettings settings;
    settings.setPath( "MyCompany.com", "MyApplication" );

    settings.writeEntry( "/colors/background", bgColor );
    settings.writeEntry( "/geometry/width", width );
    // ...
    \endcode

    A key prefix can be prepended to all keys using beginGroup(). The
    application of the prefix is stopped using endGroup(). For
    example:
    \code
    TQSettings settings;

    settings.beginGroup( "/MainWindow" );
        settings.beginGroup( "/Geometry" );
            int x = settings.readEntry( "/x" );
            // ...
        settings.endGroup();
        settings.beginGroup( "/Toolbars" );
            // ...
        settings.endGroup();
    settings.endGroup();
    \endcode

    You can get a list of entry-holding keys by calling entryList(), and
    a list of key-holding keys using subkeyList().

    \code
    TQStringList keys = settings.entryList( "/MyApplication" );
    // keys contains 'background color' and 'foreground color'.

    TQStringList keys = settings.entryList( "/MyApplication/recent files" );
    // keys contains '1', '2' and '3'.

    TQStringList subkeys = settings.subkeyList( "/MyApplication" );
    // subkeys contains 'geometry' and 'recent files'

    TQStringList subkeys = settings.subkeyList( "/MyApplication/recent files" );
    // subkeys is empty.
    \endcode

    Since settings for Windows are stored in the registry there are
    some size limitations as follows:
    \list
    \i A subkey may not exceed 255 characters.
    \i An entry's value may not exceed 16,300 characters.
    \i All the values of a key (for example, all the 'recent files'
    subkeys values), may not exceed 65,535 characters.
    \endlist

    These limitations are not enforced on Unix or Mac OS X.

    \warning Creating multiple, simultaneous instances of TQSettings writing
    to a text file may lead to data loss! This is a known issue which will
    be fixed in a future release of TQt.

    \section1 Notes for Mac OS X Applications

    The location where settings are stored is not formally defined by
    the CFPreferences API.

    At the time of writing settings are stored (either on a global or
    user basis, preferring locally) into a plist file in \c
    $ROOT/System/Library/Preferences (in XML format). TQSettings will
    create an appropriate plist file (\c{com.<first group name>.plist})
    out of the full path to a key.

    For further information on CFPreferences see
    \link http://developer.apple.com/documentation/CoreFoundation/Conceptual/CFPreferences/index.html
    Apple's Specifications\endlink

    \section1 Notes for Unix Applications

    There is no universally accepted place for storing application
    settings under Unix. In the examples the settings file will be
    searched for in the following directories:
    \list 1
    \i \c SYSCONF - the default value is \c INSTALL/etc/settings
    \i \c /opt/MyCompany/share/etc
    \i \c /opt/MyCompany/share/MyApplication/etc
    \i \c $HOME/.qt
    \endlist
    When reading settings the files are searched in the order shown
    above, with later settings overriding earlier settings. Files for
    which the user doesn't have read permission are ignored. When saving
    settings TQSettings works in the order shown above, writing
    to the first settings file for which the user has write permission.
    (\c INSTALL is the directory where TQt was installed.  This can be
    modified by using the configure script's -prefix argument )

    If you want to put the settings in a particular place in the
    filesystem you could do this:
    \code
    settings.insertSearchPath( TQSettings::Unix, "/opt/MyCompany/share" );
    \endcode

    But in practice you may prefer not to use a search path for Unix.
    For example the following code:
    \code
    settings.writeEntry( "/MyApplication/geometry/width", width );
    \endcode
    will end up writing the "geometry/width" setting to the file
    \c{$HOME/.qt/myapplicationrc} (assuming that the application is
    being run by an ordinary user, i.e. not by root).

    For cross-platform applications you should ensure that the
    \link #sizelimit Windows size limitations \endlink are not exceeded.

    \warning TQSettings doesn't write the settings until it is destroyed so
    you should construct the TQSettings object on the stack.
*/

/*!
    \enum TQSettings::System

    \value Mac Macintosh execution environments
    \value Unix Mac OS X, Unix, Linux and Unix-like execution environments
    \value Windows Windows execution environments
*/

/*!
    \enum TQSettings::Format

    \value Native Store the settings in a platform dependent location
    \value Ini Store the settings in a text file
*/

/*!
    \enum TQSettings::Scope

    \value Global Save settings as global as possible
    \value User Save settings in user space
*/

#if defined(Q_OS_UNIX)
typedef int HANDLE;
#define Q_LOCKREAD F_RDLCK
#define Q_LOCKWRITE F_WRLCK
/*
  Locks the file specified by name.  The lockfile is created as a
  hidden file in the same directory as the target file, with .lock
  appended to the name. For example, "/etc/settings/onerc" uses a
  lockfile named "/etc/settings/.onerc.lock".  The type argument
  controls the type of the lock, it can be either F_RDLCK for a read
  lock, or F_WRLCK for a write lock.

  A file descriptor for the lock file is returned, and should be
  closed with closelock() when the lock is no longer needed.
 */
static HANDLE openlock( const TQString &name, int type )
{
    TQFileInfo info( name );
    // lockfile should be hidden, and never removed
    TQString lockfile = info.dirPath() + "/." + info.fileName() + ".lock";

    // open the lockfile
    HANDLE fd = tqt_open( TQFile::encodeName( lockfile ),
			 O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );

    if ( fd < 0 ) {
 	// failed to open the lock file, most likely because of permissions
	return fd;
    }

    struct flock fl;
    fl.l_type = type;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    if ( fcntl( fd, F_SETLKW, &fl ) == -1 ) {
	// the lock failed, so we should fail silently, so that people
	// using filesystems that do not support locking don't see
	// numerous warnings about a failed lock
	close( fd );
	fd = -1;
    }

    return fd;
}

/*
  Closes the lock file specified by fd.  fd is the file descriptor
  returned by the openlock() function.
*/
static void closelock( HANDLE fd )
{
    if ( fd < 0 ) {
	// the lock file is not open
	return;
    }

    struct flock fl;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    // ignore the return value, so that the unlock fails silently
    (void) fcntl( fd, F_SETLKW, &fl );

    close( fd );
}
#endif


TQSettingsGroup::TQSettingsGroup()
    : modified(false)
{
}




void TQSettingsHeading::read(const TQString &filename)
{
    if (! TQFileInfo(filename).exists())
	return;

#ifndef TQ_WS_WIN
    HANDLE lockfd = openlock( filename, Q_LOCKREAD );
#endif

    TQFile file(filename);
    if (! file.open(IO_ReadOnly)) {
#if defined(QT_CHECK_STATE)
	tqWarning("TQSettings: failed to open file '%s'", filename.latin1());
#endif
	return;
    }

    git = end();

    TQTextStream stream(&file);
    stream.setEncoding(TQTextStream::UnicodeUTF8);
    while (! stream.atEnd())
	parseLine(stream);

    git = end();

    file.close();

#ifndef TQ_WS_WIN
    closelock( lockfd );
#endif
}


void TQSettingsHeading::parseLine(TQTextStream &stream)
{
    TQString line = stream.readLine();
    if (line.isEmpty())
	// empty line... we'll allow it
	return;

    if (line[0] == TQChar('#'))
	// commented line
	return;

    if (line[0] == TQChar('[')) {
	TQString gname = line;

	gname = gname.remove((uint)0, 1);
	if (gname[(int)gname.length() - 1] == TQChar(']'))
	    gname = gname.remove(gname.length() - 1, 1);

	git = find(gname);
	if (git == end())
	    git = replace(gname, TQSettingsGroup());
    } else {
	if (git == end()) {
#if defined(QT_CHECK_STATE)
	    tqWarning("TQSettings: line '%s' out of group", line.latin1());
#endif
	    return;
	}

	int i = line.find('=');
       	if (i == -1) {
#if defined(QT_CHECK_STATE)
	    tqWarning("TQSettings: malformed line '%s' in group '%s'",
		     line.latin1(), git.key().latin1());
#endif
	    return;
	} else {
	    TQString key, value;
	    key = line.left(i);
	    value = "";
	    bool esc=true;
	    i++;
	    while (esc) {
		esc = false;
		for ( ; i < (int)line.length(); i++ ) {
		    if ( esc ) {
			if ( line[i] == 'n' )
			    value.append('\n'); // escaped newline
			else if ( line[i] == '0' )
			    value = TQString::null; // escaped empty string
			else
			    value.append(line[i]);
			esc = false;
		    } else if ( line[i] == '\\' )
			esc = true;
		    else
			value.append(line[i]);
		}
		if ( esc ) {
		    // Backwards-compatiblity...
		    // still escaped at EOL - manually escaped "newline"
		    if (stream.atEnd()) {
#if defined(QT_CHECK_STATE)
			tqWarning("TQSettings: reached end of file, expected continued line");
#endif
			break;
		    }
		    value.append('\n');
		    line = stream.readLine();
		    i = 0;
		}
	    }

	    (*git).insert(key, value);
	}
    }
}

#ifdef TQ_WS_WIN // for homedirpath reading from registry
#include "qt_windows.h"
#include "ntqlibrary.h"

#ifndef CSIDL_APPDATA
#define CSIDL_APPDATA                   0x001a        // <user name>\Application Data
#endif
#ifndef CSIDL_COMMON_APPDATA
#define CSIDL_COMMON_APPDATA            0x0023        // All Users\Application Data
#endif

#endif

TQSettingsPrivate::TQSettingsPrivate( TQSettings::Format format )
    : groupDirty( true ), modified(false), globalScope(true)
{
#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( format != TQSettings::Ini )
	return;
#else
    Q_UNUSED( format );
#endif

    TQString home;
    home = getenv("QT_HOME_DIR");
    if ( !home.isEmpty() ) {
       home += "/";
       TQFileInfo i( home + "qtrc" );
       if ( !i.isReadable() ) {
         home = TQDir::homeDirPath() + "/.qt/";
       }
    } else {
       home = TQDir::homeDirPath() + "/.qt/";
    }
    TQString appSettings(home);

    TQString defPath;
#ifdef TQ_WS_WIN
#ifdef Q_OS_TEMP
	TCHAR path[MAX_PATH];
	SHGetSpecialFolderPath( 0, path, CSIDL_APPDATA, false );
	appSettings  = TQString::fromUcs2( path );
	SHGetSpecialFolderPath( 0, path, CSIDL_COMMON_APPDATA, false );
	defPath = TQString::fromUcs2( path );
#else
    TQLibrary library( "shell32" );
    library.setAutoUnload( false );
    QT_WA( {
	typedef BOOL (WINAPI*GetSpecialFolderPath)(HWND, LPTSTR, int, BOOL);
	GetSpecialFolderPath SHGetSpecialFolderPath = (GetSpecialFolderPath)library.resolve( "SHGetSpecialFolderPathW" );
	if ( SHGetSpecialFolderPath ) {
	    TCHAR path[MAX_PATH];
	    SHGetSpecialFolderPath( 0, path, CSIDL_APPDATA, false );
	    appSettings  = TQString::fromUcs2( (ushort*)path );
	    SHGetSpecialFolderPath( 0, path, CSIDL_COMMON_APPDATA, false );
	    defPath = TQString::fromUcs2( (ushort*)path );
	}
    } , {
	typedef BOOL (WINAPI*GetSpecialFolderPath)(HWND, char*, int, BOOL);
	GetSpecialFolderPath SHGetSpecialFolderPath = (GetSpecialFolderPath)library.resolve( "SHGetSpecialFolderPathA" );
	if ( SHGetSpecialFolderPath ) {
	    char path[MAX_PATH];
	    SHGetSpecialFolderPath( 0, path, CSIDL_APPDATA, false );
	    appSettings = TQString::fromLocal8Bit( path );
	    SHGetSpecialFolderPath( 0, path, CSIDL_COMMON_APPDATA, false );
	    defPath = TQString::fromLocal8Bit( path );
	}
    } );
#endif // Q_OS_TEMP
#else
    defPath = tqInstallPathSysconf();
#endif
    TQDir dir(appSettings);
    if (! dir.exists()) {
	if (! dir.mkdir(dir.path()))
#if defined(QT_CHECK_STATE)
	    tqWarning("TQSettings: error creating %s", dir.path().latin1());
#else
	    ;
#endif
    }

    if ( !!defPath )
	searchPaths.append(defPath);

    TQString system;
    system = getenv("QT_SYSTEM_DIR");
    if ( !system.isEmpty() && system[0] == '/') {
       TQFileInfo i( system + "/qtrc" );
       if ( i.isReadable() ) {
           searchPaths.append(system);
       }
    }

    searchPaths.append(dir.path());
}

TQSettingsPrivate::~TQSettingsPrivate()
{
}

TQSettingsGroup TQSettingsPrivate::readGroup()
{
    TQSettingsHeading hd;
    TQSettingsGroup grp;

    TQMap<TQString,TQSettingsHeading>::Iterator headingsit = headings.find(heading);
    if (headingsit != headings.end())
	hd = *headingsit;

    TQSettingsHeading::Iterator grpit = hd.find(group);
    if (grpit == hd.end()) {
	TQStringList::Iterator it = searchPaths.begin();
	if ( !globalScope )
	    ++it;
	while (it != searchPaths.end()) {
            TQString filebase = heading.lower().replace(TQRegExp(TQString::fromLatin1("\\s+")), "_");
	    TQString fn((*it++) + "/" + filebase + "rc");
	    if (! hd.contains(fn + "cached")) {
		hd.read(fn);
		hd.insert(fn + "cached", TQSettingsGroup());
	    }
	}

	headings.replace(heading, hd);

	grpit = hd.find(group);
	if (grpit != hd.end())
	    grp = *grpit;
    } else if (hd.count() != 0)
	grp = *grpit;

    return grp;
}


void TQSettingsPrivate::removeGroup(const TQString &key)
{
    TQSettingsHeading hd;
    TQSettingsGroup grp;
    bool found = false;

    TQMap<TQString,TQSettingsHeading>::Iterator headingsit = headings.find(heading);
    if (headingsit != headings.end())
	hd = *headingsit;

    TQSettingsHeading::Iterator grpit = hd.find(group);
    if (grpit == hd.end()) {
	TQStringList::Iterator it = searchPaths.begin();
	if ( !globalScope )
	    ++it;
	while (it != searchPaths.end()) {
            TQString filebase = heading.lower().replace(TQRegExp(TQString::fromLatin1("\\s+")), "_");
	    TQString fn((*it++) + "/" + filebase + "rc");
	    if (! hd.contains(fn + "cached")) {
		hd.read(fn);
		hd.insert(fn + "cached", TQSettingsGroup());
	    }
	}

	headings.replace(heading, hd);

	grpit = hd.find(group);
	if (grpit != hd.end()) {
	    found = true;
	    grp = *grpit;
	}
    } else if (hd.count() != 0) {
	found = true;
	grp = *grpit;
    }

    if (found) {
	grp.remove(key);

	if (grp.count() > 0)
	    hd.replace(group, grp);
	else
	    hd.remove(group);

	if (hd.count() > 0)
	    headings.replace(heading, hd);
	else
	    headings.remove(heading);

	modified = true;
    }
}


void TQSettingsPrivate::writeGroup(const TQString &key, const TQString &value)
{
    TQSettingsHeading hd;
    TQSettingsGroup grp;

    TQMap<TQString,TQSettingsHeading>::Iterator headingsit = headings.find(heading);
    if (headingsit != headings.end())
	hd = *headingsit;

    TQSettingsHeading::Iterator grpit = hd.find(group);
    if (grpit == hd.end()) {
	TQStringList::Iterator it = searchPaths.begin();
	if ( !globalScope )
	    ++it;
	while (it != searchPaths.end()) {
            TQString filebase = heading.lower().replace(TQRegExp(TQString::fromLatin1("\\s+")), "_");
	    TQString fn((*it++) + "/" + filebase + "rc");
	    if (! hd.contains(fn + "cached")) {
		hd.read(fn);
		hd.insert(fn + "cached", TQSettingsGroup());
	    }
	}

	headings.replace(heading, hd);

	grpit = hd.find(group);
	if (grpit != hd.end())
	    grp = *grpit;
    } else if (hd.count() != 0)
	grp = *grpit;

    grp.modified = true;
    grp.replace(key, value);
    hd.replace(group, grp);
    headings.replace(heading, hd);

    modified = true;
}


TQDateTime TQSettingsPrivate::modificationTime()
{
    TQSettingsHeading hd = headings[heading];
    TQSettingsGroup grp = hd[group];

    TQDateTime datetime;

    TQStringList::Iterator it = searchPaths.begin();
    if ( !globalScope )
	++it;
    while (it != searchPaths.end()) {
	TQFileInfo fi((*it++) + "/" + heading + "rc");
	if (fi.exists() && fi.lastModified() > datetime)
	    datetime = fi.lastModified();
    }

    return datetime;
}

bool qt_verify_key( const TQString &key )
{
    if ( key.isEmpty() || key[0] != '/' || key.contains( TQRegExp(TQString::fromLatin1("[=\\r\\n]")) ) )
	return false;
    return true;
}

static TQString groupKey( const TQString &group, const TQString &key )
{
    TQString grp_key;
    if ( group.isEmpty() || ( group.length() == 1 && group[0] == '/' ) ) {
	// group is empty, or it contains a single '/', so we just return the key
	if ( key.startsWith( "/" ) )
	    grp_key = key;
	else
	    grp_key = "/" + key;
    } else if ( group.endsWith( "/" ) || key.startsWith( "/" ) ) {
	grp_key = group + key;
    } else {
	grp_key = group + "/" + key;
    }
    return grp_key;
}

/*!
  Inserts \a path into the settings search path. The semantics of \a
  path depends on the system \a s. It is usually easier and better to
  use setPath() instead of this function.

  When \a s is \e Windows and the execution environment is \e not
  Windows the function does nothing. Similarly when \a s is \e Unix and
  the execution environment is \e not Unix the function does nothing.

  When \a s is \e Windows, and the execution environment is Windows, the
  search path list will be used as the first subfolder of the "Software"
  folder in the registry.

  When reading settings the folders are searched forwards from the
  first folder (listed below) to the last, returning the first
  settings found, and ignoring any folders for which the user doesn't
  have read permission.
  \list 1
  \i HKEY_CURRENT_USER/Software/MyCompany/MyApplication
  \i HKEY_LOCAL_MACHINE/Software/MyCompany/MyApplication
  \i HKEY_CURRENT_USER/Software/MyApplication
  \i HKEY_LOCAL_MACHINE/Software/MyApplication
  \endlist

  \code
  TQSettings settings;
  settings.insertSearchPath( TQSettings::Windows, "/MyCompany" );
  settings.writeEntry( "/MyApplication/Tip of the day", true );
  \endcode
  The code above will write the subkey "Tip of the day" into the \e
  first of the registry folders listed below that is found and for
  which the user has write permission.
  \list 1
  \i HKEY_LOCAL_MACHINE/Software/MyCompany/MyApplication
  \i HKEY_CURRENT_USER/Software/MyCompany/MyApplication
  \i HKEY_LOCAL_MACHINE/Software/MyApplication
  \i HKEY_CURRENT_USER/Software/MyApplication
  \endlist
  If a setting is found in the HKEY_CURRENT_USER space, this setting
  is overwritten independently of write permissions in the
  HKEY_LOCAL_MACHINE space.

  When \a s is \e Unix, and the execution environment is Unix, the
  search path list will be used when trying to determine a suitable
  filename for reading and writing settings files. By default, there are
  two entries in the search path:

  \list 1
  \i \c SYSCONF - where \c SYSCONF is a directory specified when
  configuring TQt; by default it is INSTALL/etc/settings.
  \i \c $HOME/.qt/ - where \c $HOME is the user's home directory.
  \endlist

  All insertions into the search path will go before $HOME/.qt/.
  For example:
  \code
  TQSettings settings;
  settings.insertSearchPath( TQSettings::Unix, "/opt/MyCompany/share/etc" );
  settings.insertSearchPath( TQSettings::Unix, "/opt/MyCompany/share/MyApplication/etc" );
  // ...
  \endcode
  Will result in a search path of:
  \list 1
  \i SYSCONF
  \i /opt/MyCompany/share/etc
  \i /opt/MyCompany/share/MyApplication/etc
  \i $HOME/.qt
  \endlist
    When reading settings the files are searched in the order shown
    above, with later settings overriding earlier settings. Files for
    which the user doesn't have read permission are ignored. When saving
    settings TQSettings works in the order shown above, writing
    to the first settings file for which the user has write permission.

    Note that paths in the file system are not created by this
    function, so they must already exist to be useful.

  Settings under Unix are stored in files whose names are based on the
  first subkey of the key (not including the search path). The algorithm
  for creating names is essentially: lowercase the first subkey, replace
  spaces with underscores and add 'rc', e.g.
  <tt>/MyCompany/MyApplication/background color</tt> will be stored in
  <tt>myapplicationrc</tt> (assuming that <tt>/MyCompany</tt> is part of
  the search path).

  \sa removeSearchPath()

*/
void TQSettings::insertSearchPath( System s, const TQString &path)
{
#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd ) {
	d->sysInsertSearchPath( s, path );
	return;
    }
#endif

#if !defined(TQ_WS_WIN)
    if ( s == Windows )
	return;
#endif
#if !defined(Q_OS_MAC)
    if ( s == Mac )
	return;
#endif

    if ( !qt_verify_key( path ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::insertSearchPath: Invalid key: '%s'", path.isNull() ? "(null)" : path.latin1() );
#endif
	return;
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd && s != Unix ) {
#else
    if ( s != Unix ) {
#endif
#if !defined(TQWS) && defined(Q_OS_MAC)
	if(s != Mac) //mac is respected on the mac as well
#endif
	    return;
    }

    TQString realPath = path;
#if defined(TQ_WS_WIN)
    TQString defPath = d->globalScope ? d->searchPaths.first() : d->searchPaths.last();
    realPath = defPath + path;
#endif

    TQStringList::Iterator it = d->searchPaths.find(d->searchPaths.last());
    if (it != d->searchPaths.end()) {
	d->searchPaths.insert(it, realPath);
    }
}


/*!
  Removes all occurrences of \a path (using exact matching) from the
  settings search path for system \a s. Note that the default search
  paths cannot be removed.

  \sa insertSearchPath()
*/
void TQSettings::removeSearchPath( System s, const TQString &path)
{
    if ( !qt_verify_key( path ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::insertSearchPath: Invalid key: '%s'", path.isNull() ? "(null)" : path.latin1() );
#endif
	return;
    }

#ifdef TQ_WS_WIN
    if ( d->sysd ) {
	d->sysRemoveSearchPath( s, path );
	return;
    }
#endif
#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd && s != Unix ) {
#else
    if ( s != Unix ) {
#endif
#if !defined(TQWS) && defined(Q_OS_MAC)
	if(s != Mac) //mac is respected on the mac as well
#endif
	    return;
    }

    if (path == d->searchPaths.first() || path == d->searchPaths.last())
	return;

    d->searchPaths.remove(path);
}


/*!
  Creates a settings object.

  Be aware that you must call setPath() or insertSearchPath() before
  you can use the TQSettings object.
*/
TQSettings::TQSettings()
{
    d = new TQSettingsPrivate( Native );
    TQ_CHECK_PTR(d);

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    d->sysd = 0;
    d->sysInit();
#endif
}

/*!
  Creates a settings object. If \a format is 'Ini' the settings will
  be stored in a text file, using the Unix strategy (see above). If \a format
  is 'Native', the settings will be stored in a platform specific way
  (ie. the Windows registry).

  Be aware that you must call setPath() or insertSearchPath() before
  you can use the TQSettings object.
*/
TQSettings::TQSettings( Format format )
{
    d = new TQSettingsPrivate( format );
    TQ_CHECK_PTR(d);

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    d->sysd = 0;
    if ( format == Native )
	d->sysInit();
#else
    Q_UNUSED(format);
#endif
}

/*!
  Destroys the settings object.  All modifications made to the settings
  will automatically be saved.

*/
TQSettings::~TQSettings()
{
    sync();

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	d->sysClear();
#endif

    delete d;
}


/*! \internal
  Writes all modifications to the settings to disk.  If any errors are
  encountered, this function returns false, otherwise it will return true.
*/
bool TQSettings::sync()
{
#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return d->sysSync();
#endif
    if (! d->modified)
	// fake success
	return true;

    bool success = true;
    TQMap<TQString,TQSettingsHeading>::Iterator it = d->headings.begin();

    while (it != d->headings.end()) {
	// determine filename
	TQSettingsHeading hd(*it);
	TQSettingsHeading::Iterator hdit = hd.begin();
	TQString filename;

	TQStringList::Iterator pit = d->searchPaths.begin();
	if ( !d->globalScope )
	    ++pit;
	while (pit != d->searchPaths.end()) {
            TQString filebase = it.key().lower().replace(TQRegExp(TQString::fromLatin1("\\s+")), "_");
	    TQFileInfo di(*pit);
	    if ( !di.exists() ) {
		TQDir dir;
		dir.mkdir( *pit );
	    }

	    TQFileInfo fi((*pit++) + "/" + filebase + "rc");

	    if ((fi.exists() && fi.isFile() && fi.isWritable()) ||
		(! fi.exists() && di.isDir()
#ifndef TQ_WS_WIN
		&& di.isWritable()
#else
		&& ((qWinVersion()&TQt::WV_NT_based) > TQt::WV_2000 || di.isWritable())
#endif
		)) {
		filename = fi.filePath();
		break;
	    }
	}

	++it;

	if ( filename.isEmpty() ) {

#ifdef QT_CHECK_STATE
	    tqWarning("TQSettings::sync: filename is null/empty");
#endif // QT_CHECK_STATE

	    success = false;
	    continue;
	}

#ifndef TQ_WS_WIN
	HANDLE lockfd = openlock( filename, Q_LOCKWRITE );
#endif

	TQFile file( filename + ".tmp" );
	if (! file.open(IO_WriteOnly)) {

#ifdef QT_CHECK_STATE
	    tqWarning("TQSettings::sync: failed to open '%s' for writing",
		     file.name().latin1());
#endif // QT_CHECK_STATE

	    success = false;
	    continue;
	}

	// spew to file
	TQTextStream stream(&file);
	stream.setEncoding(TQTextStream::UnicodeUTF8);

	while (hdit != hd.end()) {
	    if ((*hdit).count() > 0) {
		stream << "[" << hdit.key() << "]" << endl;

		TQSettingsGroup grp(*hdit);
		TQSettingsGroup::Iterator grpit = grp.begin();

		while (grpit != grp.end()) {
		    TQString v = grpit.data();
		    if ( v.isNull() ) {
                        v = TQString::fromLatin1("\\0"); // escape null string
		    } else {
                        v.replace('\\', TQString::fromLatin1("\\\\")); // escape backslash
                        v.replace('\n', TQString::fromLatin1("\\n")); // escape newlines
		    }

		    stream << grpit.key() << "=" << v << endl;
		    ++grpit;
		}

		stream << endl;
	    }

	    ++hdit;
	}

	if (file.status() != IO_Ok) {

#ifdef QT_CHECK_STATE
	    tqWarning("TQSettings::sync: error at end of write");
#endif // QT_CHECK_STATE

	    success = false;
	}

	file.close();

	if ( success ) {
	    TQDir dir( TQFileInfo( file ).dir( true ) );
	    if ( ( dir.exists( filename ) && !dir.remove( filename ) ) ||
		 !dir.rename( file.name(), filename, true ) ) {

#ifdef QT_CHECK_STATE
		tqWarning( "TQSettings::sync: error writing file '%s'",
			  TQFile::encodeName( filename ).data() );
#endif // QT_CHECK_STATE

		success = false;
	    }
	}

	// remove temporary file
	file.remove();

#ifndef TQ_WS_WIN
	closelock( lockfd );
#endif
    }

    d->modified = false;

    return success;
}


/*!
  \fn bool TQSettings::readBoolEntry(const TQString &key, bool def, bool *ok ) const

  Reads the entry specified by \a key, and returns a bool, or the
  default value, \a def, if the entry couldn't be read.
  If \a ok is non-null, *ok is set to true if the key was read, false
  otherwise.

  \sa readEntry(), readNumEntry(), readDoubleEntry(), writeEntry(), removeEntry()
*/

/*!
    \internal
*/
bool TQSettings::readBoolEntry(const TQString &key, bool def, bool *ok )
{
    TQString grp_key( groupKey( group(), key ) );
    if ( !qt_verify_key( grp_key ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::readBoolEntry: Invalid key: '%s'", grp_key.isNull() ? "(null)" : grp_key.latin1() );
#endif
    	if ( ok )
	    *ok = false;

	return def;
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return d->sysReadBoolEntry( grp_key, def, ok );
#endif

    TQString value = readEntry( key, ( def ? "true" : "false" ), ok );

    if (value.lower() == "true")
	return true;
    else if (value.lower() == "false")
	return false;
    else if (value == "1")
	return true;
    else if (value == "0")
	return false;

    if (! value.isEmpty())
	tqWarning("TQSettings::readBoolEntry: '%s' is not 'true' or 'false'",
		 value.latin1());
    if ( ok )
	*ok = false;
    return def;
}


/*!
    \fn double TQSettings::readDoubleEntry(const TQString &key, double def, bool *ok ) const

  Reads the entry specified by \a key, and returns a double, or the
  default value, \a def, if the entry couldn't be read.
  If \a ok is non-null, *ok is set to true if the key was read, false
  otherwise.

  \sa readEntry(), readNumEntry(), readBoolEntry(), writeEntry(), removeEntry()
*/

/*!
    \internal
*/
double TQSettings::readDoubleEntry(const TQString &key, double def, bool *ok )
{
    TQString grp_key( groupKey( group(), key ) );
    if ( !qt_verify_key( grp_key ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::readDoubleEntry: Invalid key: '%s'", grp_key.isNull() ? "(null)" : grp_key.latin1() );
#endif
    	if ( ok )
	    *ok = false;

	return def;
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return d->sysReadDoubleEntry( grp_key, def, ok );
#endif

    TQString value = readEntry( key, TQString::number(def), ok );
    bool conv_ok;
    double retval = value.toDouble( &conv_ok );
    if ( conv_ok )
	return retval;
    if ( ! value.isEmpty() )
	tqWarning( "TQSettings::readDoubleEntry: '%s' is not a number",
		  value.latin1() );
    if ( ok )
	*ok = false;
    return def;
}


/*!
    \fn int TQSettings::readNumEntry(const TQString &key, int def, bool *ok ) const

  Reads the entry specified by \a key, and returns an integer, or the
  default value, \a def, if the entry couldn't be read.
  If \a ok is non-null, *ok is set to true if the key was read, false
  otherwise.

  \sa readEntry(), readDoubleEntry(), readBoolEntry(), writeEntry(), removeEntry()
*/

/*!
    \internal
*/
int TQSettings::readNumEntry(const TQString &key, int def, bool *ok )
{
    TQString grp_key( groupKey( group(), key ) );
    if ( !qt_verify_key( grp_key ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::readNumEntry: Invalid key: '%s'", grp_key.isNull() ? "(null)" : grp_key.latin1() );
#endif
	if ( ok )
	    *ok = false;
	return def;
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return d->sysReadNumEntry( grp_key, def, ok );
#endif

    TQString value = readEntry( key, TQString::number( def ), ok );
    bool conv_ok;
    int retval = value.toInt( &conv_ok );
    if ( conv_ok )
	return retval;
    if ( ! value.isEmpty() )
	tqWarning( "TQSettings::readNumEntry: '%s' is not a number",
		  value.latin1() );
    if ( ok )
	*ok = false;
    return def;
}


/*!
    \fn TQString TQSettings::readEntry(const TQString &key, const TQString &def, bool *ok ) const

  Reads the entry specified by \a key, and returns a TQString, or the
  default value, \a def, if the entry couldn't be read.
  If \a ok is non-null, *ok is set to true if the key was read, false
  otherwise.

  \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), writeEntry(), removeEntry()
*/

/*!
    \internal
*/
TQString TQSettings::readEntry(const TQString &key, const TQString &def, bool *ok )
{
    TQString grp_key( groupKey( group(), key ) );
    if ( !qt_verify_key( grp_key ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::readEntry: Invalid key: '%s'", grp_key.isNull() ? "(null)" : grp_key.latin1() );
#endif
	if ( ok )
	    *ok = false;

	return def;
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return d->sysReadEntry( grp_key, def, ok );
#endif

    if ( ok ) // no, everything is not ok
	*ok = false;

    TQString realkey;

    if (grp_key[0] == '/') {
	// parse our key
	TQStringList list(TQStringList::split('/', grp_key));

	if (list.count() < 2) {
#ifdef QT_CHECK_STATE
	    tqWarning("TQSettings::readEntry: invalid key '%s'", grp_key.latin1());
#endif // QT_CHECK_STATE
	    if ( ok )
		*ok = false;
	    return def;
	}

	if (list.count() == 2) {
	    d->heading = list[0];
	    d->group = "General";
	    realkey = list[1];
	} else {
	    d->heading = list[0];
	    d->group = list[1];

	    // remove the group from the list
	    list.remove(list.at(1));
	    // remove the heading from the list
	    list.remove(list.at(0));

	    realkey = list.join("/");
	}
    } else {
    	realkey = grp_key;
    }

    TQSettingsGroup grp = d->readGroup();
    TQSettingsGroup::const_iterator it = grp.find( realkey ), end = grp.end();
    TQString retval = def;
    if ( it != end ) {
	// found the value we needed
	retval = *it;
	if ( ok ) *ok = true;
    }
    return retval;
}


/*!
    Writes the boolean entry \a value into key \a key. The \a key is
    created if it doesn't exist. Any previous value is overwritten by \a
    value.

    If an error occurs the settings are left unchanged and false is
    returned; otherwise true is returned.

    \warning On certain platforms, keys are required to contain at least
    two components (e.g., "/foo/bar"). This limitation does not apply to
    TQt 4.

  \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), removeEntry()
*/
bool TQSettings::writeEntry(const TQString &key, bool value)
{
    TQString grp_key( groupKey( group(), key ) );
    if ( !qt_verify_key( grp_key ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::writeEntry: Invalid key: '%s'", grp_key.isNull() ? "(null)" : grp_key.latin1() );
#endif
	return false;
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return d->sysWriteEntry( grp_key, value );
#endif
    TQString s(value ? "true" : "false");
    return writeEntry(key, s);
}


/*!
    \overload
    Writes the double entry \a value into key \a key. The \a key is
    created if it doesn't exist. Any previous value is overwritten by \a
    value.

    If an error occurs the settings are left unchanged and false is
    returned; otherwise true is returned.

  \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), removeEntry()
*/
bool TQSettings::writeEntry(const TQString &key, double value)
{
    TQString grp_key( groupKey( group(), key ) );
    if ( !qt_verify_key( grp_key ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::writeEntry: Invalid key: '%s'", grp_key.isNull() ? "(null)" : grp_key.latin1() );
#endif
	return false;
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return d->sysWriteEntry( grp_key, value );
#endif
    TQString s(TQString::number(value));
    return writeEntry(key, s);
}


/*!
    \overload
    Writes the integer entry \a value into key \a key. The \a key is
    created if it doesn't exist. Any previous value is overwritten by \a
    value.

    If an error occurs the settings are left unchanged and false is
    returned; otherwise true is returned.

  \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), removeEntry()
*/
bool TQSettings::writeEntry(const TQString &key, int value)
{
    TQString grp_key( groupKey( group(), key ) );
    if ( !qt_verify_key( grp_key ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::writeEntry: Invalid key: '%s'", grp_key.isNull() ? "(null)" : grp_key.latin1() );
#endif
	return false;
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return d->sysWriteEntry( grp_key, value );
#endif
    TQString s(TQString::number(value));
    return writeEntry(key, s);
}


/*!
    \internal

  Writes the entry specified by \a key with the string-literal \a value,
  replacing any previous setting.  If \a value is zero-length or null, the
  entry is replaced by an empty setting.

  \e NOTE: This function is provided because some compilers use the
  writeEntry (const TQString &, bool) overload for this code:
  writeEntry ("/foo/bar", "baz")

  If an error occurs, this functions returns false and the object is left
  unchanged.

  \sa readEntry(), removeEntry()
*/
bool TQSettings::writeEntry(const TQString &key, const char *value)
{
    return writeEntry(key, TQString(value));
}


/*!
    \overload
    Writes the string entry \a value into key \a key. The \a key is
    created if it doesn't exist. Any previous value is overwritten by \a
    value. If \a value is an empty string or a null string the key's
    value will be an empty string.

    If an error occurs the settings are left unchanged and false is
    returned; otherwise true is returned.

  \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), removeEntry()
*/
bool TQSettings::writeEntry(const TQString &key, const TQString &value)
{
    TQString grp_key( groupKey( group(), key ) );
    if ( !qt_verify_key( grp_key ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::writeEntry: Invalid key: '%s'", grp_key.isNull() ? "(null)" : grp_key.latin1() );
#endif
	return false;
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return d->sysWriteEntry( grp_key, value );
#endif
    // NOTE: we *do* allow value to be a null/empty string

    TQString realkey;

    if (grp_key[0] == '/') {
	// parse our key
	TQStringList list(TQStringList::split('/', grp_key));

	if (list.count() < 2) {
#ifdef QT_CHECK_STATE
	    tqWarning("TQSettings::writeEntry: invalid key '%s'", grp_key.latin1());
#endif // QT_CHECK_STATE

	    return false;
	}

	if (list.count() == 2) {
	    d->heading = list[0];
	    d->group = "General";
	    realkey = list[1];
	} else {
	    d->heading = list[0];
	    d->group = list[1];

	    // remove the group from the list
	    list.remove(list.at(1));
	    // remove the heading from the list
	    list.remove(list.at(0));

	    realkey = list.join("/");
	}
    } else {
	realkey = grp_key;
    }

    d->writeGroup(realkey, value);
    return true;
}


/*!
    Removes the entry specified by \a key.

    Returns true if the entry was successfully removed; otherwise
    returns false. Note that removing the last entry in any given
    folder, will also remove the folder.

    \sa readEntry(), writeEntry()
*/
bool TQSettings::removeEntry(const TQString &key)
{
    TQString grp_key( groupKey( group(), key ) );
    if ( !qt_verify_key( grp_key ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::removeEntry: Invalid key: '%s'", grp_key.isNull() ? "(null)" : grp_key.latin1() );
#endif
	return false;
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return d->sysRemoveEntry( grp_key );
#endif

    TQString realkey;
    if (grp_key[0] == '/') {
	// parse our key
	TQStringList list(TQStringList::split('/', grp_key));

	if (list.count() < 2) {
#ifdef QT_CHECK_STATE
	    tqWarning("TQSettings::removeEntry: invalid key '%s'", grp_key.latin1());
#endif // QT_CHECK_STATE

	    return false;
	}

	if (list.count() == 2) {
	    d->heading = list[0];
	    d->group = "General";
	    realkey = list[1];
	} else {
	    d->heading = list[0];
	    d->group = list[1];

	    // remove the group from the list
	    list.remove(list.at(1));
	    // remove the heading from the list
	    list.remove(list.at(0));

	    realkey = list.join("/");
	}
    } else {
	realkey = grp_key;
    }

    d->removeGroup(realkey);
    return true;
}


/*!
  Returns a list of the keys which contain entries under \a key. Does \e
  not return any keys that contain subkeys.

    Example settings:
    \code
    /MyCompany/MyApplication/background color
    /MyCompany/MyApplication/foreground color
    /MyCompany/MyApplication/geometry/x
    /MyCompany/MyApplication/geometry/y
    /MyCompany/MyApplication/geometry/width
    /MyCompany/MyApplication/geometry/height
    \endcode
    \code
    TQStringList keys = settings.entryList( "/MyCompany/MyApplication" );
    \endcode

    In the above example, \c keys will contain 'background color' and
    'foreground color'. It will not contain 'geometry' because this key
    contains subkeys not entries.

    To access the geometry values, you could either use subkeyList()
    to read the keys then read each entry, or simply read each entry
    directly by specifying its full key, e.g.
    "/MyCompany/MyApplication/geometry/y".

  \sa subkeyList()
*/
TQStringList TQSettings::entryList(const TQString &key) const
{
    TQString grp_key( groupKey( group(), key ) );
    if ( !qt_verify_key( grp_key ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::entryList: Invalid key: %s", grp_key.isNull() ? "(null)" : grp_key.latin1() );
#endif
	return TQStringList();
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return d->sysEntryList( grp_key );
#endif

    TQString realkey;
    if (grp_key[0] == '/') {
	// parse our key
	TQStringList list(TQStringList::split('/', grp_key));

	if (list.count() < 1) {
#ifdef QT_CHECK_STATE
	    tqWarning("TQSettings::listEntries: invalid key '%s'", grp_key.latin1());
#endif // QT_CHECK_STATE

	    return TQStringList();
	}

	if (list.count() == 1) {
	    d->heading = list[0];
	    d->group = "General";
	} else {
	    d->heading = list[0];
	    d->group = list[1];

	    // remove the group from the list
	    list.remove(list.at(1));
	    // remove the heading from the list
	    list.remove(list.at(0));

	    realkey = list.join("/");
	}
    } else
	realkey = grp_key;

    TQSettingsGroup grp = d->readGroup();
    TQSettingsGroup::Iterator it = grp.begin();
    TQStringList ret;
    TQString itkey;
    while (it != grp.end()) {
	itkey = it.key();
	++it;

	if ( realkey.length() > 0 ) {
	    if ( itkey.left( realkey.length() ) != realkey )
		continue;
	    else
		itkey.remove( 0, realkey.length() + 1 );
	}

	if ( itkey.find( '/' ) != -1 )
	    continue;

	ret << itkey;
    }

    return ret;
}


/*!
  Returns a list of the keys which contain subkeys under \a key. Does \e
  not return any keys that contain entries.

    Example settings:
    \code
    /MyCompany/MyApplication/background color
    /MyCompany/MyApplication/foreground color
    /MyCompany/MyApplication/geometry/x
    /MyCompany/MyApplication/geometry/y
    /MyCompany/MyApplication/geometry/width
    /MyCompany/MyApplication/geometry/height
    /MyCompany/MyApplication/recent files/1
    /MyCompany/MyApplication/recent files/2
    /MyCompany/MyApplication/recent files/3
    \endcode
    \code
    TQStringList keys = settings.subkeyList( "/MyCompany/MyApplication" );
    \endcode

    In the above example, \c keys will contain 'geometry' and
    'recent files'. It will not contain 'background color' or
    'foreground color' because those keys contain entries not
    subkeys. To get a list of keys that contain entries rather than
    subkeys use entryList() instead.

    \warning In the above example, if TQSettings is writing to an Ini file,
    then a call to
    \code subkeyList("/MyCompany") \endcode
    will return an empty list. This happens because a key like
    \code /MyCompany/MyApplication/background color \endcode
    is written to the file \e{"mycompanyrc"}, under the section \e{[MyApplication]}.
    This call is therefore a request to list the sections in an ini file, which
    is not supported in this version of TQSettings. This is a known issue which
    will be fixed in TQt-4.

  \sa entryList()
*/
TQStringList TQSettings::subkeyList(const TQString &key) const
{
    TQString grp_key( groupKey( group(), key ) );
    if ( !qt_verify_key( grp_key ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::subkeyList: Invalid key: %s", grp_key.isNull() ? "(null)" : grp_key.latin1() );
#endif
	return TQStringList();
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return d->sysSubkeyList( grp_key );
#endif

    TQString realkey;
    int subkeycount = 2;
    if (grp_key[0] == '/') {
	// parse our key
	TQStringList list(TQStringList::split('/', grp_key));

	if (list.count() < 1) {
#ifdef QT_CHECK_STATE
	    tqWarning("TQSettings::subkeyList: invalid key '%s'", grp_key.latin1());
#endif // QT_CHECK_STATE

	    return TQStringList();
	}

	subkeycount = (int)list.count();

	if (list.count() == 1) {
	    d->heading = list[0];
	    d->group = "General";
	} else {
	    d->heading = list[0];
	    d->group = list[1];

	    // remove the group from the list
	    list.remove(list.at(1));
	    // remove the heading from the list
	    list.remove(list.at(0));

	    realkey = list.join("/");
	}

    } else
	realkey = grp_key;

    TQStringList ret;
    if ( subkeycount == 1 ) {
	TQMap<TQString,TQSettingsHeading>::Iterator it = d->headings.begin();
	while ( it != d->headings.end() ) {
	    if ( it.key() != "General" && ! ret.contains( it.key() ) )
		ret << it.key();
	    ++it;
	}

	return ret;
    }

    TQSettingsGroup grp = d->readGroup();
    TQSettingsGroup::Iterator it = grp.begin();
    TQString itkey;
    while (it != grp.end()) {
	itkey = it.key();
	++it;

	if ( realkey.length() > 0 ) {
	    if ( itkey.left( realkey.length() ) != realkey
            || itkey.length()+1 < realkey.length()
            || itkey[(int)realkey.length()] != '/')
		continue;
	    else
		itkey.remove( 0, realkey.length() + 1 );
	}

	int slash = itkey.find( '/' );
	if ( slash == -1 )
	    continue;
	itkey.truncate( slash );

	if ( ! ret.contains( itkey ) )
	    ret << itkey;
    }

    return ret;
}


/*!
    \internal

  This function returns the time of last modification for \a key.
*/
TQDateTime TQSettings::lastModificationTime( const TQString &key )
{
    TQString grp_key( groupKey( group(), key ) );
    if ( !qt_verify_key( grp_key ) ) {
#if defined(QT_CHECK_STATE)
	tqWarning( "TQSettings::lastModificationTime: Invalid key '%s'", grp_key.isNull() ? "(null)" : grp_key.latin1() );
#endif
	return TQDateTime();
    }

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    if ( d->sysd )
	return TQDateTime();
#endif

    if (grp_key[0] == '/') {
	// parse our key
	TQStringList list(TQStringList::split('/', grp_key));

	if (list.count() < 2) {
#ifdef QT_CHECK_STATE
	    tqWarning("TQSettings::lastModificationTime: Invalid key '%s'", grp_key.latin1());
#endif // QT_CHECK_STATE

	    return TQDateTime();
	}

	if (list.count() == 2) {
	    d->heading = list[0];
	    d->group = "General";
	} else {
	    d->heading = list[0];
	    d->group = list[1];
	}
    }

    return d->modificationTime();
}


/*!
    \overload
    \obsolete

    Writes the string list entry \a value into key \a key. The \a key
    is created if it doesn't exist. Any previous value is overwritten
    by \a value. The list is stored as a sequence of strings separated
    by \a separator (using TQStringList::join()), so none of the
    strings in the list should contain the separator. If the list is
    empty or null the key's value will be an empty string.

    \warning The list should not contain empty or null strings, as
    readListEntry() will use TQStringList::split() to recreate the
    list.  As the documentation states, TQStringList::split() will omit
    empty strings from the list.  Because of this, it is impossible to
    retrieve identical list data that is stored with this function.
    We recommend using the writeEntry() and readListEntry() overloads
    that do not take a \a separator argument.


    If an error occurs the settings are left unchanged and false is
    returned; otherwise returns true.

    \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), removeEntry(), TQStringList::join()
*/
bool TQSettings::writeEntry(const TQString &key, const TQStringList &value,
			   const TQChar &separator)
{
    TQString s(value.join(separator));
    return writeEntry(key, s);
}

/*!
    \overload

    Writes the string list entry \a value into key \a key. The \a key
    is created if it doesn't exist. Any previous value is overwritten
    by \a value.

    If an error occurs the settings are left unchanged and false is
    returned; otherwise returns true.

    \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), removeEntry()
*/
bool TQSettings::writeEntry(const TQString &key, const TQStringList &value)
{
    TQString s;
    for (TQStringList::ConstIterator it=value.begin(); it!=value.end(); ++it) {
	TQString el = *it;
	if ( el.isNull() ) {
	    el = "^0";
	} else {
	    el.replace("^", "^^");
	}
	s+=el;
	s+="^e"; // end of element
    }
    return writeEntry(key, s);
}


/*!
    \overload TQStringList TQSettings::readListEntry(const TQString &key, const TQChar &separator, bool *ok ) const
    \obsolete

    Reads the entry specified by \a key as a string. The \a separator
    is used to create a TQStringList by calling TQStringList::split(\a
    separator, entry). If \a ok is not 0: \a *ok is set to true
    if the key was read, otherwise \a *ok is set to false.

    \warning As the documentation states, TQStringList::split() will
    omit empty strings from the list.  Because of this, it is
    impossible to retrieve identical list data with this function.  We
    recommend using the readListEntry() and writeEntry() overloads
    that do not take a \a separator argument.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = mySettings.readListEntry( "size", " " );
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa readEntry(), readDoubleEntry(), readBoolEntry(), writeEntry(), removeEntry(), TQStringList::split()
*/

/*!
    \internal
*/
TQStringList TQSettings::readListEntry(const TQString &key, const TQChar &separator, bool *ok )
{
    TQString value = readEntry( key, TQString::null, ok );
    if ( ok && !*ok )
	return TQStringList();

    return TQStringList::split(separator, value);
}

/*!
    \fn TQStringList TQSettings::readListEntry(const TQString &key, bool *ok ) const
    Reads the entry specified by \a key as a string. If \a ok is not
    0, \a *ok is set to true if the key was read, otherwise \a *ok is
    set to false.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    TQStringList list = mySettings.readListEntry( "recentfiles" );
    TQStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa readEntry(), readDoubleEntry(), readBoolEntry(), writeEntry(), removeEntry(), TQStringList::split()
*/

/*!
    \internal
*/
TQStringList TQSettings::readListEntry(const TQString &key, bool *ok )
{
    TQString value = readEntry( key, TQString::null, ok );
    if ( ok && !*ok )
	return TQStringList();
    TQStringList l;
    TQString s;
    bool esc=false;
    for (int i=0; i<(int)value.length(); i++) {
	if ( esc ) {
	    if ( value[i] == 'e' ) { // end-of-string
		l.append(s);
		s="";
	    } else if ( value[i] == '0' ) { // null string
		s=TQString::null;
	    } else {
		s.append(value[i]);
	    }
	    esc=false;
	} else if ( value[i] == '^' ) {
	    esc = true;
	} else {
	    s.append(value[i]);
	    if ( i == (int)value.length()-1 )
		l.append(s);
	}
    }
    return l;
}

#ifdef Q_OS_MAC
void qt_setSettingsBasePath(const TQString &); //qsettings_mac.cpp
#endif

/*!
    Insert platform-dependent paths from platform-independent information.

    The \a domain should be an Internet domain name
    controlled by the producer of the software, eg. Trolltech products
    use "trolltech.com".

    The \a product should be the official name of the product.

    The \a scope should be
    TQSettings::User for user-specific settings, or
    TQSettings::Global for system-wide settings (generally
    these will be read-only to many users).

    Not all information is relevant on all systems.
*/

void TQSettings::setPath( const TQString &domain, const TQString &product, Scope scope )
{
//    On Windows, any trailing ".com(\..*)" is stripped from the domain. The
//    Global scope corresponds to HKEY_LOCAL_MACHINE, and User corresponds to
//    HKEY_CURRENT_USER. Note that on some installations, not all users can
//    write to the Global scope. On UNIX, any trailing ".com(\..*)" is stripped
//    from the domain. The Global scope corresponds to "/opt" (this would be
//    configurable at library build time - eg. to "/usr/local" or "/usr"),
//    while the User scope corresponds to $HOME/.*rc.
//    Note that on most installations, not all users can write to the System
//    scope.
//
//    On MacOS X, if there is no "." in domain, append ".com", then reverse the
//    order of the elements (Mac OS uses "com.apple.finder" as domain+product).
//    The Global scope corresponds to /Library/Preferences/*.plist, while the
//    User scope corresponds to ~/Library/Preferences/*.plist.
//    Note that on most installations, not all users can write to the System
//    scope.
    d->globalScope = scope == Global;

    TQString actualSearchPath;
    int lastDot = domain.findRev( '.' );

#if defined(TQ_WS_WIN)
    actualSearchPath = "/" + domain.mid( 0, lastDot ) + "/" + product;
    insertSearchPath( Windows, actualSearchPath );
#elif !defined(TQWS) && defined(Q_OS_MAC)
    if(lastDot != -1) {
	TQString topLevelDomain = domain.right( domain.length() - lastDot - 1 ) + ".";
	if ( !topLevelDomain.isEmpty() )
	    qt_setSettingsBasePath( topLevelDomain );
    }
    actualSearchPath = "/" + domain.left( lastDot ) + "." + product;
    insertSearchPath( Mac, actualSearchPath );
#else
    if (scope == User)
	actualSearchPath = TQDir::homeDirPath() + "/.";
    else
	actualSearchPath = TQString(tqInstallPathSysconf()) + "/";
    actualSearchPath += domain.mid( 0, lastDot ) + "/" + product;
    insertSearchPath( Unix, actualSearchPath );
#endif
}

/*!
    Appends \a group to the current key prefix.

    \code
    TQSettings settings;
    settings.beginGroup( "/MainWindow" );
    // read values
    settings.endGroup();
    \endcode
*/
void TQSettings::beginGroup( const TQString &group )
{
    d->groupStack.push( group );
    d->groupDirty = true;
}

/*!
    Undo previous calls to beginGroup(). Note that a single beginGroup("a/b/c") is undone
    by a single call to endGroup().

    \code
    TQSettings settings;
    settings.beginGroup( "/MainWindow/Geometry" );
    // read values
    settings.endGroup();
    \endcode
*/
void TQSettings::endGroup()
{
    d->groupStack.pop();
    d->groupDirty = true;
}

/*!
    Set the current key prefix to the empty string.
*/
void TQSettings::resetGroup()
{
    d->groupStack.clear();
    d->groupDirty = false;
    d->groupPrefix = TQString::null;
}

/*!
    Returns the current key prefix, or a null string if there is no key prefix set.

    \sa beginGroup();
*/
TQString TQSettings::group() const
{
    if ( d->groupDirty ) {
	d->groupDirty = false;
	d->groupPrefix = TQString::null;

	TQValueStack<TQString>::Iterator it = d->groupStack.begin();
	while ( it != d->groupStack.end() ) {
	    TQString group = *it;
	    ++it;
	    if ( group[0] != '/' )
		group.prepend( "/" );
	    d->groupPrefix += group;
	}
    }
    return d->groupPrefix;
}

#endif
