/****************************************************************************
**
** Implementation of TQComLibrary class
**
** Copyright (C) 2001-2008 Trolltech ASA.  All rights reserved.
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

#include "qcomlibrary_p.h"

#ifndef TQT_NO_COMPONENT
#include <ntqapplication.h>
#include <ntqsettings.h>
#include <ntqfileinfo.h>
#include <ntqdatetime.h>
#include <ntqcleanuphandler.h>
#ifndef NO_ERRNO_H
#include <errno.h>
#endif // NO_ERROR_H

#ifdef TQT_THREAD_SUPPORT
#  include "qmutexpool_p.h"
#endif // TQT_THREAD_SUPPORT

#ifndef QT_DEBUG_COMPONENT
# if defined(QT_DEBUG)
#  define QT_DEBUG_COMPONENT 1
# endif
#endif


TQComLibrary::TQComLibrary( const TQString &filename )
    : TQLibrary( filename ), entry( 0 ), libiface( 0 ), tqt_version( 0 )
{
}

TQComLibrary::~TQComLibrary()
{
    if ( autoUnload() )
	unload();
    if ( libiface )
	libiface->release();
    if ( entry )
	entry->release();
}

bool TQComLibrary::unload()
{
    int refs = entry ? entry->release() : 0;
    entry = 0;
    if (refs || !libiface)
	return false;

    libiface->cleanup();
    if ( !libiface->canUnload() )
	return false;
    libiface->release();
    libiface = 0;

    return TQLibrary::unload();
}

static bool qt_verify( const TQString& library, uint version, uint flags,
		    const TQCString &key, bool warn )
{
    uint our_flags = 1;
#if defined(TQT_THREAD_SUPPORT)
    our_flags |= 2;
#endif

    if ( (flags & 1) == 0 ) {
	if ( warn )
	    tqWarning( "Conflict in %s:\n"
		      "  Plugin cannot be queried successfully!",
		      (const char*) TQFile::encodeName(library) );
    } else if ( ( version > TQT_VERSION ) ||
		( ( TQT_VERSION & 0xff0000 ) > ( version & 0xff0000 ) ) ) {
	if ( warn )
	    tqWarning( "Conflict in %s:\n"
		      "  Plugin uses incompatible TQt library (%d.%d.%d)!",
		      (const char*) TQFile::encodeName(library),
		      (version&0xff0000) >> 16, (version&0xff00) >> 8, version&0xff );
    } else if ( (flags & 2) != (our_flags & 2) ) {
	if ( warn )
	    tqWarning( "Conflict in %s:\n"
		      "  Plugin uses %s TQt library!",
		      (const char*) TQFile::encodeName(library),
		      (flags & 2) ? "multi threaded" : "single threaded" );
    } else if ( key != TQT_BUILD_KEY ) {
	if ( warn )
	    tqWarning( "Conflict in %s:\n"
		      "  Plugin uses incompatible TQt library!\n"
		      "  expected build key \"%s\", got \"%s\".",
		      (const char*) TQFile::encodeName(library),
		      TQT_BUILD_KEY,
		      key.isEmpty() ? "<null>" : (const char *) key );
    } else {
	return true;
    }
    return false;
}

struct qt_token_info
{
    qt_token_info( const char *f, const ulong fc )
	: fields( f ), field_count( fc ), results( fc ), lengths( fc )
    {
	results.fill( 0 );
	lengths.fill( 0 );
    }

    const char *fields;
    const ulong field_count;

    TQMemArray<const char *> results;
    TQMemArray<ulong> lengths;
};

/*
  return values:
       1 parse ok
       0 eos
      -1 parse error
*/
static int qt_tokenize( const char *s, ulong s_len, ulong *advance,
			const qt_token_info &token_info )
{
    ulong pos = 0, field = 0, fieldlen = 0;
    char current;
    int ret = -1;
    *advance = 0;
    for (;;) {
	current = s[ pos ];

	// next char
	++pos;
	++fieldlen;
	++*advance;

	if ( ! current || pos == s_len + 1 ) {
	    // save result
	    token_info.results[ (int)field ] = s;
	    token_info.lengths[ (int)field ] = fieldlen - 1;

	    // end of string
	    ret = 0;
	    break;
	}

	if ( current == token_info.fields[ field ] ) {
	    // save result
	    token_info.results[ (int)field ] = s;
	    token_info.lengths[ (int)field ] = fieldlen - 1;

	    // end of field
	    fieldlen = 0;
	    ++field;
	    if ( field == token_info.field_count - 1 ) {
		// parse ok
		ret = 1;
	    }
	    if ( field == token_info.field_count ) {
		// done parsing
		break;
	    }

	    // reset string and its length
	    s = s + pos;
	    s_len -= pos;
	    pos = 0;
	}
    }

    return ret;
}

/*
  returns true if the string s was correctly parsed, false otherwise.
*/
static bool qt_parse_pattern( const char *s, uint *version, uint *flags,
				   TQCString *key )
{
    bool ret = true;

    qt_token_info pinfo("=\n", 2);
    int parse;
    ulong at = 0, advance, parselen = tqstrlen( s );
    do {
	parse = qt_tokenize( s + at, parselen, &advance, pinfo );
	if ( parse == -1 ) {
	    ret = false;
	    break;
	}

	at += advance;
	parselen -= advance;

	if ( tqstrncmp( "version", pinfo.results[ 0 ], pinfo.lengths[ 0 ] ) == 0 ) {
	    // parse version string
	    qt_token_info pinfo2("..-", 3);
	    if ( qt_tokenize( pinfo.results[ 1 ], pinfo.lengths[ 1 ],
			      &advance, pinfo2 ) != -1 ) {
		TQCString m( pinfo2.results[ 0 ], pinfo2.lengths[ 0 ] + 1 );
		TQCString n( pinfo2.results[ 1 ], pinfo2.lengths[ 1 ] + 1 );
		TQCString p( pinfo2.results[ 2 ], pinfo2.lengths[ 2 ] + 1 );
		*version  = (m.toUInt() << 16) | (n.toUInt() << 8) | p.toUInt();
	    } else {
		ret = false;
		break;
	    }
	} else if ( tqstrncmp( "flags", pinfo.results[ 0 ], pinfo.lengths[ 0 ] ) == 0 ) {
	    // parse flags string
	    char ch;
	    *flags = 0;
	    ulong p = 0, c = 0, bit = 0;
	    while ( p < pinfo.lengths[ 1 ] ) {
		ch = pinfo.results[ 1 ][ p ];
		bit = pinfo.lengths[ 1 ] - p - 1;
		c = 1 << bit;
		if ( ch == '1' ) {
		    *flags |= c;
		} else if ( ch != '0' ) {
		    ret = false;
		    break;
		}
		++p;
	    }
	} else if ( tqstrncmp( "buildkey", pinfo.results[ 0 ],
			      pinfo.lengths[ 0 ] ) == 0 ){
	    // save buildkey
	    *key = TQCString( pinfo.results[ 1 ], pinfo.lengths[ 1 ] + 1 );
	}
    } while ( parse == 1 && parselen > 0 );

    return ret;
}

#if defined(Q_OS_UNIX)

#if defined(Q_OS_FREEBSD) || defined(Q_OS_LINUX)
#  define USE_MMAP
#  include <sys/types.h>
#  include <sys/mman.h>
#endif // Q_OS_FREEBSD || Q_OS_LINUX

static long qt_find_pattern( const char *s, ulong s_len,
			     const char *pattern, ulong p_len )
{
    /*
      this uses the same algorithm as TQString::findRev...

      we search from the end of the file because on the supported
      systems, the read-only data/text segments are placed at the end
      of the file.  HOWEVER, when building with debugging enabled, all
      the debug symbols are placed AFTER the data/text segments.

      what does this mean?  when building in release mode, the search
      is fast because the data we are looking for is at the end of the
      file... when building in debug mode, the search is slower
      because we have to skip over all the debugging symbols first
    */

    if ( ! s || ! pattern || p_len > s_len ) return -1;
    ulong i, hs = 0, hp = 0, delta = s_len - p_len;

    for (i = 0; i < p_len; ++i ) {
	hs += s[delta + i];
	hp += pattern[i];
    }
    i = delta;
    for (;;) {
	if ( hs == hp && tqstrncmp( s + i, pattern, p_len ) == 0 )
	    return i;
	if ( i == 0 )
	    break;
	--i;
	hs -= s[i + p_len];
	hs += s[i];
    }

    return -1;
}

/*
  This opens the specified library, mmaps it into memory, and searches
  for the QT_UCM_VERIFICATION_DATA.  The advantage of this approach is that
  we can get the verification data without have to actually load the library.
  This lets us detect mismatches more safely.

  Returns false if version/flags/key information is not present, or if the
                information could not be read.
  Returns  true if version/flags/key information is present and succesfully read.
*/
static bool qt_unix_query( const TQString &library, uint *version, uint *flags,
			   TQCString *key )
{
    TQFile file( library );
    if (! file.open( IO_ReadOnly ) ) {
	tqWarning( "%s: %s", (const char*) TQFile::encodeName(library),
	    strerror( errno ) );
	return false;
    }

    TQByteArray data;
    char *filedata = 0;
    ulong fdlen = 0;

#ifdef USE_MMAP
    char *mapaddr = 0;
    size_t maplen = file.size();
    mapaddr = (char *) mmap( mapaddr, maplen, PROT_READ, MAP_PRIVATE, file.handle(), 0 );
    if ( mapaddr != MAP_FAILED ) {
	// mmap succeeded
	filedata = mapaddr;
	fdlen = maplen;
    } else {
	// mmap failed
	tqWarning( "mmap: %s", strerror( errno ) );
#endif // USE_MMAP
	// try reading the data into memory instead
	data = file.readAll();
	filedata = data.data();
	fdlen = data.size();
#ifdef USE_MMAP
    }
#endif // USE_MMAP

    // verify that the pattern is present in the plugin
    const char *pattern = "pattern=QT_UCM_VERIFICATION_DATA";
    const ulong plen = tqstrlen( pattern );
    long pos = qt_find_pattern( filedata, fdlen, pattern, plen );

    bool ret = false;
    if ( pos >= 0 ) {
	ret = qt_parse_pattern( filedata + pos, version, flags, key );
    }

#ifdef USE_MMAP
    if ( mapaddr != MAP_FAILED && munmap(mapaddr, maplen) != 0 ) {
	tqWarning( "munmap: %s", strerror( errno ) );
    }
#endif // USE_MMAP

    file.close();
    return ret;
}

#endif // Q_OS_UNIX


static TQSettings *cache = 0;
static TQSingleCleanupHandler<TQSettings> cleanup_cache;

void TQComLibrary::createInstanceInternal()
{
    if ( library().isEmpty() )
	return;

    TQFileInfo fileinfo( library() );
    TQString lastModified  = fileinfo.lastModified().toString(TQt::ISODate);
    TQString regkey = TQString("/TQt Plugins %1.%2/%3")
		     .arg( ( TQT_VERSION & 0xff0000 ) >> 16 )
		     .arg( ( TQT_VERSION & 0xff00 ) >> 8 )
		     .arg( library() );
    TQStringList reg;
    uint flags = 0;
    TQCString key;
    bool query_done = false;
    bool warn_mismatch = true;

#ifdef TQT_THREAD_SUPPORT
    TQMutexLocker locker( tqt_global_mutexpool ?
			 tqt_global_mutexpool->get( &cache ) : 0 );
#endif // TQT_THREAD_SUPPORT

    if ( ! cache ) {
	cache = new TQSettings;
	cache->insertSearchPath( TQSettings::Windows, "/Trolltech" );
	cleanup_cache.set( &cache );
    }

    reg = cache->readListEntry( regkey );
    if ( reg.count() == 4 ) {
	// check timestamp
	if ( lastModified == reg[3] ) {
	    tqt_version = reg[0].toUInt(0, 16);
	    flags = reg[1].toUInt(0, 16);
	    key = reg[2].latin1();

	    query_done = true;
	    warn_mismatch = false;
	}
    }

#if defined(Q_OS_UNIX)
    if ( ! query_done ) {
	// get the query information directly from the plugin without loading
	if ( qt_unix_query( library(), &tqt_version, &flags, &key ) ) {
 	    // info read succesfully from library
 	    query_done = true;
	}
    }
#else // !Q_OS_UNIX
    if ( ! query_done ) {
	// get the query information by loading the plugin
	if ( !isLoaded() ) {
	    Q_ASSERT( entry == 0 );
	    if ( !load() )
		return;
	}

	typedef const char * (*UCMQueryVerificationDataProc)();
	UCMQueryVerificationDataProc ucmQueryVerificationdataProc;
	ucmQueryVerificationdataProc =
	    (UCMQueryVerificationDataProc) resolve( "qt_ucm_query_verification_data" );

	if ( !ucmQueryVerificationdataProc ||
	     !qt_parse_pattern( ucmQueryVerificationdataProc(),
				&tqt_version, &flags, &key ) ) {
	    tqt_version = flags = 0;
	    key = "unknown";
	} else {
	    query_done = true;
	}
    }
#endif // Q_OS_UNIX

    TQStringList queried;
    queried << TQString::number( tqt_version,16 )
	    << TQString::number( flags, 16 )
	    <<  key
	    << lastModified;

    if ( queried != reg ) {
	cache->writeEntry( regkey, queried );
	// delete the cache, which forces the settings to be written
	delete cache;
	cache = 0;
    }

    if ( ! query_done ) {
	if ( warn_mismatch ) {
	    tqWarning( "Conflict in %s:\n Plugin cannot be queried successfully!",
		      (const char*) TQFile::encodeName( library() ) );
	}
	unload();
	return;
    }

    if ( ! qt_verify( library(), tqt_version, flags, key, warn_mismatch ) ) {
	unload();
	return;
    } else if ( !isLoaded() ) {
	Q_ASSERT( entry == 0 );
	if ( !load() )
	    return;
    }

    typedef TQUnknownInterface* (*UCMInstanceProc)();
    UCMInstanceProc ucmInstanceProc;
    ucmInstanceProc = (UCMInstanceProc) resolve( "ucm_instantiate" );
#if defined(QT_DEBUG_COMPONENT)
    if ( !ucmInstanceProc )
	tqWarning( "%s: Not a UCOM library.", (const char*) TQFile::encodeName(library()) );
#endif
    entry = ucmInstanceProc ? ucmInstanceProc() : 0;

    if ( entry ) {
	if ( entry->queryInterface( IID_QLibrary, (TQUnknownInterface**)&libiface ) == TQS_OK ) {
	    if ( libiface && !libiface->init() ) {
		libiface->release();
		libiface = 0;
		unload();
		return;
	    }
	}
    } else {
#if defined(QT_DEBUG_COMPONENT)
	tqWarning( "%s: No exported component provided.", (const char*) TQFile::encodeName(library()) );
#endif
	unload();
    }
}

TQRESULT TQComLibrary::queryInterface( const TQUuid& request, TQUnknownInterface** iface )
{
    if ( !entry )
	createInstanceInternal();
    return entry ? entry->queryInterface( request, iface ) : TQE_NOCOMPONENT;
}

uint TQComLibrary::qtVersion()
{
    if ( !entry )
	createInstanceInternal();
    return entry ? tqt_version : 0;
}


#endif // TQT_NO_COMPONENT
