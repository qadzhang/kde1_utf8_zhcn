/****************************************************************************
**
** Localization database support.
**
** Created : 980906
**
** Copyright (C) 1998-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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
static inline int tqt_open(const char *pathname, int flags, mode_t mode)
{ return ::open(pathname, flags, mode); }
#if defined(open)
# undef open
#endif

// POSIX Large File Support redefines truncate -> truncate64
#if defined(truncate)
# undef truncate
#endif

#include "ntqtranslator.h"

#ifndef TQT_NO_TRANSLATION

#include "ntqfileinfo.h"
#include "ntqwidgetlist.h"
#include "ntqintdict.h"
#include "ntqstring.h"
#include "ntqapplication.h"
#include "ntqfile.h"
#include "ntqbuffer.h"
#include "ntqdatastream.h"
#include "ntqmap.h"
#include "ntqtl.h"

#if defined(Q_OS_UNIX)
#define QT_USE_MMAP
#endif

// most of the headers below are already included in qplatformdefs.h
// also this lacks Large File support but that's probably irrelevant
#if defined(QT_USE_MMAP)
// for mmap
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
// for htonl
#include <netinet/in.h>
#endif

#include <stdlib.h>

/*
$ mcookie
3cb86418caef9c95cd211cbf60a1bddd
$
*/

// magic number for the file
static const int MagicLength = 16;
static const uchar magic[MagicLength] = {
    0x3c, 0xb8, 0x64, 0x18, 0xca, 0xef, 0x9c, 0x95,
    0xcd, 0x21, 0x1c, 0xbf, 0x60, 0xa1, 0xbd, 0xdd
};

static bool match( const char* found, const char* target )
{
    // 0 means anything, "" means empty
    return found == 0 || qstrcmp( found, target ) == 0;
}

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

/*
  Yes, unfortunately, we have code here that depends on endianness.
  The candidate is big endian (it comes from a .qm file) whereas the
  target endianness depends on the system TQt is running on.
*/
#ifdef Q_OS_TEMP
static int __cdecl cmp_uint32_little( const void* target, const void* candidate )
#else
static int cmp_uint32_little( const void* target, const void* candidate )
#endif
{
    const uchar* t = (const uchar*) target;
    const uchar* c = (const uchar*) candidate;
    return t[3] != c[0] ? (int) t[3] - (int) c[0]
	   : t[2] != c[1] ? (int) t[2] - (int) c[1]
	   : t[1] != c[2] ? (int) t[1] - (int) c[2]
	   : (int) t[0] - (int) c[3];
}

#ifdef Q_OS_TEMP
static int __cdecl cmp_uint32_big( const void* target, const void* candidate )
#else
static int cmp_uint32_big( const void* target, const void* candidate )
#endif
{
    const uchar* t = (const uchar*) target;
    const uchar* c = (const uchar*) candidate;
    return t[0] != c[0] ? (int) t[0] - (int) c[0]
	   : t[1] != c[1] ? (int) t[1] - (int) c[1]
	   : t[2] != c[2] ? (int) t[2] - (int) c[2]
	   : (int) t[3] - (int) c[3];
}

#if defined(Q_C_CALLBACKS)
}
#endif

static int systemWordSize = 0;
static bool systemBigEndian;

static uint elfHash( const char * name )
{
    const uchar *k;
    uint h = 0;
    uint g;

    if ( name ) {
	k = (const uchar *) name;
	while ( *k ) {
	    h = ( h << 4 ) + *k++;
	    if ( (g = (h & 0xf0000000)) != 0 )
		h ^= g >> 24;
	    h &= ~g;
	}
    }
    if ( !h )
	h = 1;
    return h;
}

extern bool tqt_detectRTLLanguage();

class TQTranslatorPrivate {
public:
    struct Offset {
	Offset()
	    : h( 0 ), o( 0 ) { }
	Offset( const TQTranslatorMessage& m, int offset )
	    : h( m.hash() ), o( offset ) { }

	bool operator<( const Offset&k ) const {
	    return ( h != k.h ) ? h < k.h : o < k.o;
	}
	uint h;
	uint o;
    };

    enum { Contexts = 0x2f, Hashes = 0x42, Messages = 0x69 };

    TQTranslatorPrivate() :
	unmapPointer( 0 ), unmapLength( 0 ),
	messageArray( 0 ), offsetArray( 0 ), contextArray( 0 )
#ifndef TQT_NO_TRANSLATION_BUILDER
	, messages( 0 )
#endif
    { }
    // TQTranslator must finalize this before deallocating it

    // for mmap'ed files, this is what needs to be unmapped.
    char * unmapPointer;
    unsigned int unmapLength;

    // for squeezed but non-file data, this is what needs to be deleted
    TQByteArray * messageArray;
    TQByteArray * offsetArray;
    TQByteArray * contextArray;

#ifndef TQT_NO_TRANSLATION_BUILDER
    TQMap<TQTranslatorMessage, void *> * messages;
#endif
#ifdef TQ_WS_WIN
    int oldPermissionLookup;
#endif
};


/*!
    \class TQTranslator

    \brief The TQTranslator class provides internationalization support for text
    output.

    \ingroup i18n
    \ingroup environment
    \mainclass

    An object of this class contains a set of TQTranslatorMessage
    objects, each of which specifies a translation from a source
    language to a target language. TQTranslator provides functions to
    look up translations, add new ones, remove them, load and save
    them, etc.

    The most common use of TQTranslator is to: load a translator file
    created with \link linguist-manual.book TQt Linguist\endlink,
    install it using TQApplication::installTranslator(), and use it via
    TQObject::tr(). For example:

    \code
    int main( int argc, char ** argv )
    {
	TQApplication app( argc, argv );

	TQTranslator translator( 0 );
	translator.load( "french.qm", "." );
	app.installTranslator( &translator );

	MyWidget m;
	app.setMainWidget( &m );
	m.show();

	return app.exec();
    }
    \endcode
    Note that the translator must be created \e before the
    application's main window.

    Most applications will never need to do anything else with this
    class. The other functions provided by this class are useful for
    applications that work on translator files.

    We call a translation a "messsage". For this reason, translation
    files are sometimes referred to as "message files".

    It is possible to lookup a translation using findMessage() (as
    tr() and TQApplication::translate() do) and contains(), to insert a
    new translation messsage using insert(), and to remove one using
    remove().

    Translation tools often need more information than the bare source
    text and translation, for example, context information to help
    the translator. But end-user programs that are using translations
    usually only need lookup. To cater for these different needs,
    TQTranslator can use stripped translator files that use the minimum
    of memory and which support little more functionality than
    findMessage().

    Thus, load() may not load enough information to make anything more
    than findMessage() work. save() has an argument indicating
    whether to save just this minimum of information or to save
    everything.

    "Everything" means that for each translation item the following
    information is kept:

    \list
    \i The \e {translated text} - the return value from tr().
    \i The input key:
	\list
	\i The \e {source text} - usually the argument to tr().
	\i The \e context - usually the class name for the tr() caller.
	\i The \e comment - a comment that helps disambiguate different uses
	   of the same text in the same context.
	\endlist
    \endlist

    The minimum for each item is just the information necessary for
    findMessage() to return the right text. This may include the
    source, context and comment, but usually it is just a hash value
    and the translated text.

    For example, the "Cancel" in a dialog might have "Anuluj" when the
    program runs in Polish (in this case the source text would be
    "Cancel"). The context would (normally) be the dialog's class
    name; there would normally be no comment, and the translated text
    would be "Anuluj".

    But it's not always so simple. The Spanish version of a printer
    dialog with settings for two-sided printing and binding would
    probably require both "Activado" and "Activada" as translations
    for "Enabled". In this case the source text would be "Enabled" in
    both cases, and the context would be the dialog's class name, but
    the two items would have disambiguating comments such as
    "two-sided printing" for one and "binding" for the other. The
    comment enables the translator to choose the appropriate gender
    for the Spanish version, and enables TQt to distinguish between
    translations.

    Note that when TQTranslator loads a stripped file, most functions
    do not work. The functions that do work with stripped files are
    explicitly documented as such.

    \sa TQTranslatorMessage TQApplication::installTranslator()
    TQApplication::removeTranslator() TQObject::tr() TQApplication::translate()
*/

/*!
    \enum TQTranslator::SaveMode

    This enum type defines how TQTranslator writes translation
    files. There are two modes:

    \value Everything  files are saved with all available information
    \value Stripped  files are saved with just enough information for
	end-user applications

    Note that when TQTranslator loads a stripped file, most functions do
    not work. The functions that do work with stripped files are
    explicitly documented as such.
*/

#if defined(TQ_WS_WIN)
extern int tqt_ntfs_permission_lookup;
#endif

/*!
    Constructs an empty message file object that is not connected to
    any file. The object is called \a name with parent \a parent.
*/

TQTranslator::TQTranslator( TQObject * parent, const char * name )
    : TQObject( parent, name )
{
    d = new TQTranslatorPrivate;
#if defined(TQ_WS_WIN)
    d->oldPermissionLookup = tqt_ntfs_permission_lookup;
#endif
}


/*!
    Destroys the object and frees any allocated resources.
*/

TQTranslator::~TQTranslator()
{
    if ( tqApp )
	tqApp->removeTranslator( this );
    clear();
    delete d;
}


extern bool tqt_detectRTLLanguage();

/*!
    Loads \a filename, which may be an absolute file name or relative
    to \a directory. The previous contents of this translator object
    is discarded. Returns true if the file is loaded successfully;
    otherwise returns false.

    If the full file name does not exist, other file names are tried
    in the following order:

    \list 1
    \i File name with \a suffix appended (".qm" if the \a suffix is
       TQString::null).
    \i File name with text after a character in \a search_delimiters
       stripped ("_." is the default for \a search_delimiters if it is
       TQString::null).
    \i File name stripped and \a suffix appended.
    \i File name stripped further, etc.
    \endlist

    For example, an application running in the fr_CA locale
    (French-speaking Canada) might call load("foo.fr_ca",
    "/opt/foolib"). load() would then try to open the first existing
    readable file from this list:

    \list 1
    \i /opt/foolib/foo.fr_ca
    \i /opt/foolib/foo.fr_ca.qm
    \i /opt/foolib/foo.fr
    \i /opt/foolib/foo.fr.qm
    \i /opt/foolib/foo
    \i /opt/foolib/foo.qm
    \endlist

    \sa save()
*/

bool TQTranslator::load( const TQString & filename, const TQString & directory,
			const TQString & search_delimiters,
			const TQString & suffix )
{
    clear();

    TQString prefix;

    if ( filename[0] == '/'
#ifdef TQ_WS_WIN
	 || (filename[0] && filename[1] == ':') || filename[0] == '\\'
#endif
	 )
	prefix = TQString::fromLatin1( "" );
    else
	prefix = directory;

    if ( prefix.length() ) {
	if ( prefix[int(prefix.length()-1)] != '/' )
	    prefix += TQChar( '/' );
    }

    TQString fname = filename;
    TQString realname;
    TQString delims;
    delims = search_delimiters.isNull() ?
	     TQString::fromLatin1( "_." ) : search_delimiters;

#if defined(TQ_WS_WIN)
    tqt_ntfs_permission_lookup--;
#endif
    for ( ;; ) {
	TQFileInfo fi;

	realname = prefix + fname;
	fi.setFile( realname );
	if ( fi.isReadable() )
	    break;

	realname += suffix.isNull() ? TQString::fromLatin1( ".qm" ) : suffix;
	fi.setFile( realname );
	if ( fi.isReadable() )
	    break;

	int rightmost = 0;
	for ( int i = 0; i < (int)delims.length(); i++ ) {
	    int k = fname.findRev( delims[i] );
	    if ( k > rightmost )
		rightmost = k;
	}

	// no truncations? fail
        if ( rightmost == 0 ) {
#if defined(TQ_WS_WIN)
            if (d->oldPermissionLookup != tqt_ntfs_permission_lookup)
                tqt_ntfs_permission_lookup++;
#endif
	    return false;
        }

	fname.truncate( rightmost );
    }
#if defined(TQ_WS_WIN)
    if (d->oldPermissionLookup != tqt_ntfs_permission_lookup)
        tqt_ntfs_permission_lookup++;
#endif

    // realname is now the fully qualified name of a readable file.

#if defined(QT_USE_MMAP)

#ifndef MAP_FILE
#define MAP_FILE 0
#endif
#ifndef MAP_FAILED
#define MAP_FAILED -1
#endif

    int f;

    f = tqt_open( TQFile::encodeName(realname), O_RDONLY, 0666 );
    if ( f < 0 ) {
	// tqDebug( "can't open %s: %s", realname.ascii(), strerror( errno ) );
	return false;
    }

    struct stat st;
    if ( fstat( f, &st ) ) {
	// tqDebug( "can't stat %s: %s", realname.ascii(), strerror( errno ) );
	return false;
    }
    char * tmp;
    tmp = (char*)mmap( 0, st.st_size, // any address, whole file
		       PROT_READ, // read-only memory
		       MAP_FILE | MAP_PRIVATE, // swap-backed map from file
		       f, 0 ); // from offset 0 of f
    if ( !tmp || tmp == (char*)MAP_FAILED ) {
	// tqDebug( "can't mmap %s: %s", filename.ascii(), strerror( errno ) );
	return false;
    }

    ::close( f );

    d->unmapPointer = tmp;
    d->unmapLength = st.st_size;
#else
    TQFile f( realname );
    if ( !f.exists() )
	return false;
    d->unmapLength = f.size();
    d->unmapPointer = new char[d->unmapLength];
    bool ok = false;
    if ( f.open(IO_ReadOnly) ) {
	ok = d->unmapLength ==
	     (uint)f.readBlock( d->unmapPointer, d->unmapLength );
	f.close();
    }
    if ( !ok ) {
	delete [] d->unmapPointer;
	d->unmapPointer = 0;
	return false;
    }
#endif

    return do_load( (const uchar *) d->unmapPointer, d->unmapLength );
}

/*!
  \overload
  \fn bool TQTranslator::load( const uchar *data, int len )

  Loads the .qm file data \a data of length \a len into the
  translator. Returns true if the data is loaded successfully;
  otherwise returns false.

  The data is not copied. The caller must be able to guarantee that \a data
  will not be deleted or modified.
*/

bool TQTranslator::do_load( const uchar *data, int len )
{
    if ( len < MagicLength || memcmp( data, magic, MagicLength ) != 0 ) {
	clear();
	return false;
    }

    TQByteArray array;
    array.setRawData( (const char *) data, len );
    TQDataStream s( array, IO_ReadOnly );
    bool ok = true;

    s.device()->at( MagicLength );

    TQ_UINT8 tag = 0;
    TQ_UINT32 blockLen = 0;
    s >> tag >> blockLen;
    while ( tag && blockLen ) {
	if ( (TQ_UINT32) s.device()->at() + blockLen > (TQ_UINT32) len ) {
	    ok = false;
	    break;
	}

	if ( tag == TQTranslatorPrivate::Contexts && !d->contextArray ) {
	    d->contextArray = new TQByteArray;
	    d->contextArray->setRawData( array.data() + s.device()->at(),
					 blockLen );
	} else if ( tag == TQTranslatorPrivate::Hashes && !d->offsetArray ) {
	    d->offsetArray = new TQByteArray;
	    d->offsetArray->setRawData( array.data() + s.device()->at(),
					blockLen );
	} else if ( tag == TQTranslatorPrivate::Messages && !d->messageArray ) {
	    d->messageArray = new TQByteArray;
	    d->messageArray->setRawData( array.data() + s.device()->at(),
					 blockLen );
	}

	if ( !s.device()->at(s.device()->at() + blockLen) ) {
	    ok = false;
	    break;
	}
	tag = 0;
	blockLen = 0;
	if ( !s.atEnd() )
	    s >> tag >> blockLen;
    }
    array.resetRawData( (const char *) data, len );

    if ( tqApp && tqApp->translators && tqApp->translators->contains(this) )
	tqApp->setReverseLayout( tqt_detectRTLLanguage() );
    return ok;
}

#ifndef TQT_NO_TRANSLATION_BUILDER

/*!
    Saves this message file to \a filename, overwriting the previous
    contents of \a filename. If \a mode is \c Everything (the
    default), all the information is preserved. If \a mode is \c
    Stripped, any information that is not necessary for findMessage()
    is stripped away.

    \sa load()
*/

bool TQTranslator::save( const TQString & filename, SaveMode mode )
{
    TQFile f( filename );
    if ( f.open( IO_WriteOnly ) ) {
	squeeze( mode );

	TQDataStream s( &f );
	s.writeRawBytes( (const char *)magic, MagicLength );
	TQ_UINT8 tag;

	if ( d->offsetArray != 0 ) {
	    tag = (TQ_UINT8) TQTranslatorPrivate::Hashes;
	    TQ_UINT32 oas = (TQ_UINT32) d->offsetArray->size();
	    s << tag << oas;
	    s.writeRawBytes( d->offsetArray->data(), oas );
	}
	if ( d->messageArray != 0 ) {
	    tag = (TQ_UINT8) TQTranslatorPrivate::Messages;
	    TQ_UINT32 mas = (TQ_UINT32) d->messageArray->size();
	    s << tag << mas;
	    s.writeRawBytes( d->messageArray->data(), mas );
	}
	if ( d->contextArray != 0 ) {
	    tag = (TQ_UINT8) TQTranslatorPrivate::Contexts;
	    TQ_UINT32 cas = (TQ_UINT32) d->contextArray->size();
	    s << tag << cas;
	    s.writeRawBytes( d->contextArray->data(), cas );
	}
	return true;
    }
    return false;
}

#endif

/*!
    Empties this translator of all contents.

    This function works with stripped translator files.
*/

void TQTranslator::clear()
{
    if ( d->unmapPointer && d->unmapLength ) {
#if defined(QT_USE_MMAP)
	munmap( d->unmapPointer, d->unmapLength );
#else
	delete [] d->unmapPointer;
#endif
	d->unmapPointer = 0;
	d->unmapLength = 0;
    }

    if ( d->messageArray ) {
	d->messageArray->resetRawData( d->messageArray->data(),
				       d->messageArray->size() );
	delete d->messageArray;
	d->messageArray = 0;
    }
    if ( d->offsetArray ) {
	d->offsetArray->resetRawData( d->offsetArray->data(),
				      d->offsetArray->size() );
	delete d->offsetArray;
	d->offsetArray = 0;
    }
    if ( d->contextArray ) {
	d->contextArray->resetRawData( d->contextArray->data(),
				       d->contextArray->size() );
	delete d->contextArray;
	d->contextArray = 0;
    }
#ifndef TQT_NO_TRANSLATION_BUILDER
    delete d->messages;
    d->messages = 0;
#endif

    if ( tqApp ) {
	tqApp->setReverseLayout( tqt_detectRTLLanguage() );

	TQWidgetList *list = TQApplication::topLevelWidgets();
	TQWidgetListIt it( *list );
	TQWidget *w;
	while ( ( w=it.current() ) != 0 ) {
	    ++it;
	    if (!w->isDesktop())
		tqApp->postEvent( w, new TQEvent( TQEvent::LanguageChange ) );
	}
	delete list;
    }
}

#ifndef TQT_NO_TRANSLATION_BUILDER

/*!
    Converts this message file to the compact format used to store
    message files on disk.

    You should never need to call this directly; save() and other
    functions call it as necessary. \a mode is for internal use.

    \sa save() unsqueeze()
*/

void TQTranslator::squeeze( SaveMode mode )
{
    if ( !d->messages ) {
	if ( mode == Stripped )
	    unsqueeze();
	else
	    return;
    }

    TQMap<TQTranslatorMessage, void *> * messages = d->messages;

    d->messages = 0;
    clear();

    d->messageArray = new TQByteArray;
    d->offsetArray = new TQByteArray;

    TQMap<TQTranslatorPrivate::Offset, void *> offsets;

    TQDataStream ms( *d->messageArray, IO_WriteOnly );
    TQMap<TQTranslatorMessage, void *>::Iterator it = messages->begin(), next;
    int cpPrev = 0, cpNext = 0;
    for ( it = messages->begin(); it != messages->end(); ++it ) {
	cpPrev = cpNext;
	next = it;
	++next;
	if ( next == messages->end() )
	    cpNext = 0;
	else
	    cpNext = (int) it.key().commonPrefix( next.key() );
	offsets.replace( TQTranslatorPrivate::Offset(it.key(),
			 ms.device()->at()), (void*)0 );
	it.key().write( ms, mode == Stripped,
			(TQTranslatorMessage::Prefix) TQMAX(cpPrev, cpNext + 1) );
    }

    d->offsetArray->resize( 0 );
    TQMap<TQTranslatorPrivate::Offset, void *>::Iterator offset;
    offset = offsets.begin();
    TQDataStream ds( *d->offsetArray, IO_WriteOnly );
    while ( offset != offsets.end() ) {
	TQTranslatorPrivate::Offset k = offset.key();
	++offset;
	ds << (TQ_UINT32)k.h << (TQ_UINT32)k.o;
    }

    if ( mode == Stripped ) {
	TQAsciiDict<int> contextSet( 1511 );
	int baudelaire;

	for ( it = messages->begin(); it != messages->end(); ++it )
	    contextSet.replace( it.key().context(), &baudelaire );

	TQ_UINT16 hTableSize;
	if ( contextSet.count() < 200 )
	    hTableSize = ( contextSet.count() < 60 ) ? 151 : 503;
	else if ( contextSet.count() < 2500 )
	    hTableSize = ( contextSet.count() < 750 ) ? 1511 : 5003;
	else
	    hTableSize = 15013;

	TQIntDict<char> hDict( hTableSize );
	TQAsciiDictIterator<int> c = contextSet;
	while ( c.current() != 0 ) {
	    hDict.insert( (long) (elfHash(c.currentKey()) % hTableSize),
			  c.currentKey() );
	    ++c;
	}

	/*
	  The contexts found in this translator are stored in a hash
	  table to provide fast lookup. The context array has the
	  following format:

	      TQ_UINT16 hTableSize;
	      TQ_UINT16 hTable[hTableSize];
	      TQ_UINT8  contextPool[...];

	  The context pool stores the contexts as Pascal strings:

	      TQ_UINT8  len;
	      TQ_UINT8  data[len];

	  Let's consider the look-up of context "FunnyDialog".  A
	  hash value between 0 and hTableSize - 1 is computed, say h.
	  If hTable[h] is 0, "FunnyDialog" is not covered by this
	  translator. Else, we check in the contextPool at offset
	  2 * hTable[h] to see if "FunnyDialog" is one of the
	  contexts stored there, until we find it or we meet the
	  empty string.
	*/
	d->contextArray = new TQByteArray;
	d->contextArray->resize( 2 + (hTableSize << 1) );
	TQDataStream t( *d->contextArray, IO_WriteOnly );
	TQ_UINT16 *hTable = new TQ_UINT16[hTableSize];
	memset( hTable, 0, hTableSize * sizeof(TQ_UINT16) );

	t << hTableSize;
	t.device()->at( 2 + (hTableSize << 1) );
	t << (TQ_UINT16) 0; // the entry at offset 0 cannot be used
	uint upto = 2;

	for ( int i = 0; i < hTableSize; i++ ) {
	    const char *con = hDict.find( i );
	    if ( con == 0 ) {
		hTable[i] = 0;
	    } else {
		hTable[i] = (TQ_UINT16) ( upto >> 1 );
		do {
		    uint len = (uint) tqstrlen( con );
		    len = TQMIN( len, 255 );
		    t << (TQ_UINT8) len;
		    t.writeRawBytes( con, len );
		    upto += 1 + len;
		    hDict.remove( i );
		} while ( (con = hDict.find(i)) != 0 );
		do {
		    t << (TQ_UINT8) 0; // empty string (at least one)
		    upto++;
		} while ( (upto & 0x1) != 0 ); // offsets have to be even
	    }
	}
	t.device()->at( 2 );
	for ( int j = 0; j < hTableSize; j++ )
	    t << hTable[j];
	delete [] hTable;

	if ( upto > 131072 ) {
	    tqWarning( "TQTranslator::squeeze: Too many contexts" );
	    delete d->contextArray;
	    d->contextArray = 0;
	}
    }
    delete messages;
}


/*!
    Converts this message file into an easily modifiable data
    structure, less compact than the format used in the files.

    You should never need to call this function; it is called by
    insert() and friends as necessary.

    \sa squeeze()
*/

void TQTranslator::unsqueeze()
{
    if ( d->messages )
	return;

    d->messages = new TQMap<TQTranslatorMessage, void *>;
    if ( !d->messageArray )
	return;

    TQDataStream s( *d->messageArray, IO_ReadOnly );
    for ( ;; ) {
	TQTranslatorMessage m( s );
	if ( m.hash() == 0 )
	    break;
	d->messages->insert( m, (void *) 0 );
    }
}


/*!
    Returns true if this message file contains a message with the key
    (\a context, \a sourceText, \a comment); otherwise returns false.

    This function works with stripped translator files.

    (This is is a one-liner that calls findMessage().)
*/

bool TQTranslator::contains( const char* context, const char* sourceText,
			    const char* comment ) const
{
    return !findMessage( context, sourceText, comment ).translation().isNull();
}


/*!
    Inserts \a message into this message file.

    This function does \e not work with stripped translator files. It
    may appear to, but that is not dependable.

    \sa remove()
*/

void TQTranslator::insert( const TQTranslatorMessage& message )
{
    unsqueeze();
    d->messages->remove( message ); // safer
    d->messages->insert( message, (void *) 0 );
}


/*!
  \fn void TQTranslator::insert( const char *, const char *, const TQString & )
  \overload
  \obsolete
*/

/*!
    Removes \a message from this translator.

    This function works with stripped translator files.

    \sa insert()
*/

void TQTranslator::remove( const TQTranslatorMessage& message )
{
    unsqueeze();
    d->messages->remove( message );
}


/*!
  \fn void TQTranslator::remove( const char *, const char * )
  \overload
  \obsolete

  Removes the translation associated to the key (\a context, \a sourceText,
  "") from this translator.
*/
#endif

/*!
  \fn TQString TQTranslator::find( const char*, const char*, const char* ) const
  \obsolete

  Please use findMessage() instead.

  Returns the translation for the key (\a context, \a sourceText,
  \a comment) or TQString::null if there is none in this translator.
*/

/*!  Returns the TQTranslatorMessage for the key
     (\a context, \a sourceText, \a comment). If none is found,
     also tries (\a context, \a sourceText, "").
*/

TQTranslatorMessage TQTranslator::findMessage( const char* context,
					     const char* sourceText,
					     const char* comment ) const
{
    if ( context == 0 )
	context = "";
    if ( sourceText == 0 )
	sourceText = "";
    if ( comment == 0 )
	comment = "";

#ifndef TQT_NO_TRANSLATION_BUILDER
    if ( d->messages ) {
	TQMap<TQTranslatorMessage, void *>::ConstIterator it;

	it = d->messages->find( TQTranslatorMessage(context, sourceText,
						   comment) );
	if ( it != d->messages->end() )
	    return it.key();

	if ( comment[0] ) {
	    it = d->messages->find( TQTranslatorMessage(context, sourceText,
						       "") );
	    if ( it != d->messages->end() )
		return it.key();
	}
	return TQTranslatorMessage();
    }
#endif

    if ( !d->offsetArray )
	return TQTranslatorMessage();

    /*
      Check if the context belongs to this TQTranslator.  If many translators are
      installed, this step is necessary.
    */
    if ( d->contextArray ) {
	TQ_UINT16 hTableSize = 0;
	TQDataStream t( *d->contextArray, IO_ReadOnly );
	t >> hTableSize;
	uint g = elfHash( context ) % hTableSize;
	t.device()->at( 2 + (g << 1) );
	TQ_UINT16 off;
	t >> off;
	if ( off == 0 )
	    return TQTranslatorMessage();
	t.device()->at( 2 + (hTableSize << 1) + (off << 1) );

	TQ_UINT8 len;
	char con[256];
	for ( ;; ) {
	    t >> len;
	    if ( len == 0 || t.atEnd() )
		return TQTranslatorMessage();
	    t.readRawBytes( con, len );
	    con[len] = '\0';
	    if ( qstrcmp(con, context) == 0 )
		break;
	}
    }

    size_t numItems = d->offsetArray->size() / ( 2 * sizeof(TQ_UINT32) );
    if ( !numItems )
	return TQTranslatorMessage();

    if ( systemWordSize == 0 )
	tqSysInfo( &systemWordSize, &systemBigEndian );

    for ( ;; ) {
	TQ_UINT32 h = elfHash( TQCString(sourceText) + comment );

	char *r = (char *) bsearch( &h, d->offsetArray->data(), numItems,
				    2 * sizeof(TQ_UINT32),
				    systemBigEndian ? cmp_uint32_big
				    : cmp_uint32_little );
	if ( r != 0 ) {
	    // go back on equal key
	    while ( r != d->offsetArray->data() &&
		    cmp_uint32_big(r - 8, r) == 0 )
		r -= 8;

	    TQDataStream s( *d->offsetArray, IO_ReadOnly );
	    s.device()->at( r - d->offsetArray->data() );

	    TQ_UINT32 rh, ro;
	    s >> rh >> ro;

	    TQDataStream ms( *d->messageArray, IO_ReadOnly );
	    while ( rh == h ) {
		ms.device()->at( ro );
		TQTranslatorMessage m( ms );
		if ( match(m.context(), context)
			&& match(m.sourceText(), sourceText)
			&& match(m.comment(), comment) )
		    return m;
		if ( s.atEnd() )
		    break;
		s >> rh >> ro;
	    }
	}
	if ( !comment[0] )
	    break;
	comment = "";
    }
    return TQTranslatorMessage();
}

/*!
    Returns true if this translator is empty, otherwise returns false.
    This function works with stripped and unstripped translation files.
*/
bool TQTranslator::isEmpty() const
{
    return !( d->unmapPointer || d->unmapLength || d->messageArray ||
	      d->offsetArray || d->contextArray
#ifndef TQT_NO_TRANSLATION_BUILDER
              || (d->messages && d->messages->count())
#endif
        );
}


#ifndef TQT_NO_TRANSLATION_BUILDER

/*!
    Returns a list of the messages in the translator. This function is
    rather slow. Because it is seldom called, it's optimized for
    simplicity and small size, rather than speed.

    If you want to iterate over the list, you should iterate over a
    copy, e.g.
    \code
    TQValueList<TQTranslatorMessage> list = myTranslator.messages();
    TQValueList<TQTranslatorMessage>::Iterator it = list.begin();
    while ( it != list.end() ) {
	process_message( *it );
	++it;
    }
  \endcode
*/

TQValueList<TQTranslatorMessage> TQTranslator::messages() const
{
    if ( d->messages )
        return d->messages->keys();

    ((TQTranslator *) this)->unsqueeze();
    TQValueList<TQTranslatorMessage> result = d->messages->keys();
    delete d->messages;
    d->messages = 0;
    return result;
}

#endif

/*!
    \class TQTranslatorMessage

    \brief The TQTranslatorMessage class contains a translator message and its
    properties.

    \ingroup i18n
    \ingroup environment

    This class is of no interest to most applications. It is useful
    for translation tools such as \link linguist-manual.book TQt
    Linguist\endlink. It is provided simply to make the API complete
    and regular.

    For a TQTranslator object, a lookup key is a triple (\e context, \e
    {source text}, \e comment) that uniquely identifies a message. An
    extended key is a quadruple (\e hash, \e context, \e {source
    text}, \e comment), where \e hash is computed from the source text
    and the comment. Unless you plan to read and write messages
    yourself, you need not worry about the hash value.

    TQTranslatorMessage stores this triple or quadruple and the relevant
    translation if there is any.

    \sa TQTranslator
*/

/*!
    Constructs a translator message with the extended key (0, 0, 0, 0)
    and TQString::null as translation.
*/

TQTranslatorMessage::TQTranslatorMessage()
    : h( 0 ), cx( 0 ), st( 0 ), cm( 0 )
{
}


/*!
    Constructs an translator message with the extended key (\e h, \a
    context, \a sourceText, \a comment), where \e h is computed from
    \a sourceText and \a comment, and possibly with a \a translation.
*/

TQTranslatorMessage::TQTranslatorMessage( const char * context,
					const char * sourceText,
					const char * comment,
					const TQString& translation )
    : cx( context ), st( sourceText ), cm( comment ), tn( translation )
{
    // 0 means we don't know, "" means empty
    if ( cx == (const char*)0 )
	cx = "";
    if ( st == (const char*)0 )
	st = "";
    if ( cm == (const char*)0 )
	cm = "";
    h = elfHash( st + cm );
}


/*!
    Constructs a translator message read from the \a stream. The
    resulting message may have any combination of content.

    \sa TQTranslator::save()
*/

TQTranslatorMessage::TQTranslatorMessage( TQDataStream & stream )
    : cx( 0 ), st( 0 ), cm( 0 )
{
    TQString str16;
    char tag;
    TQ_UINT8 obs1;

    for ( ;; ) {
	tag = 0;
	if ( !stream.atEnd() )
	    stream.readRawBytes( &tag, 1 );
	switch( (Tag)tag ) {
	case Tag_End:
	    if ( h == 0 )
		h = elfHash( st + cm );
	    return;
	case Tag_SourceText16: // obsolete
	    stream >> str16;
	    st = str16.latin1();
	    break;
	case Tag_Translation:
	    stream >> tn;
	    break;
	case Tag_Context16: // obsolete
	    stream >> str16;
	    cx = str16.latin1();
	    break;
	case Tag_Hash:
	    stream >> h;
	    break;
	case Tag_SourceText:
	    stream >> st;
	    break;
	case Tag_Context:
	    stream >> cx;
	    if ( cx == "" ) // for compatibility
		cx = 0;
	    break;
	case Tag_Comment:
	    stream >> cm;
	    break;
	case Tag_Obsolete1: // obsolete
	    stream >> obs1;
	    break;
	default:
	    h = 0;
	    st = 0;
	    cx = 0;
	    cm = 0;
	    tn = TQString::null;
	    return;
	}
    }
}


/*!
    Constructs a copy of translator message \a m.
*/

TQTranslatorMessage::TQTranslatorMessage( const TQTranslatorMessage & m )
    : cx( m.cx ), st( m.st ), cm( m.cm ), tn( m.tn )
{
    h = m.h;
}


/*!
    Assigns message \a m to this translator message and returns a
    reference to this translator message.
*/

TQTranslatorMessage & TQTranslatorMessage::operator=(
	const TQTranslatorMessage & m )
{
    h = m.h;
    cx = m.cx;
    st = m.st;
    cm = m.cm;
    tn = m.tn;
    return *this;
}


/*!
    \fn uint TQTranslatorMessage::hash() const

    Returns the hash value used internally to represent the lookup
    key. This value is zero only if this translator message was
    constructed from a stream containing invalid data.

    The hashing function is unspecified, but it will remain unchanged
    in future versions of TQt.
*/

/*!
    \fn const char *TQTranslatorMessage::context() const

    Returns the context for this message (e.g. "MyDialog").

    \warning This may return 0 if the TQTranslator object is stripped
    (compressed).
*/

/*!
    \fn const char *TQTranslatorMessage::sourceText() const

    Returns the source text of this message (e.g. "&Save").

    \warning This may return 0 if the TQTranslator object is stripped
    (compressed).
*/

/*!
    \fn const char *TQTranslatorMessage::comment() const

    Returns the comment for this message (e.g. "File|Save").

    \warning This may return 0 if the TQTranslator object is stripped
    (compressed).
*/

/*!
    \fn void TQTranslatorMessage::setTranslation( const TQString & translation )

    Sets the translation of the source text to \a translation.

    \sa translation()
*/

/*!
    \fn TQString TQTranslatorMessage::translation() const

    Returns the translation of the source text (e.g., "&Sauvegarder").

    \sa setTranslation()
*/

/*!
    \enum TQTranslatorMessage::Prefix

    Let (\e h, \e c, \e s, \e m) be the extended key. The possible
    prefixes are

    \value NoPrefix  no prefix
    \value Hash  only (\e h)
    \value HashContext  only (\e h, \e c)
    \value HashContextSourceText  only (\e h, \e c, \e s)
    \value HashContextSourceTextComment  the whole extended key, (\e
	h, \e c, \e s, \e m)

    \sa write() commonPrefix()
*/

/*!
    Writes this translator message to the \a stream. If \a strip is
    false (the default), all the information in the message is
    written. If \a strip is true, only the part of the extended key
    specified by \a prefix is written with the translation (\c
    HashContextSourceTextComment by default).

    \sa commonPrefix()
*/

void TQTranslatorMessage::write( TQDataStream & stream, bool strip,
				Prefix prefix ) const
{
    char tag;

    tag = (char)Tag_Translation;
    stream.writeRawBytes( &tag, 1 );
    stream << tn;

    bool mustWriteHash = true;
    if ( !strip )
	prefix = HashContextSourceTextComment;

    switch ( prefix ) {
    case HashContextSourceTextComment:
	tag = (char)Tag_Comment;
	stream.writeRawBytes( &tag, 1 );
	stream << cm;
	// fall through
    case HashContextSourceText:
	tag = (char)Tag_SourceText;
	stream.writeRawBytes( &tag, 1 );
	stream << st;
	// fall through
    case HashContext:
	tag = (char)Tag_Context;
	stream.writeRawBytes( &tag, 1 );
	stream << cx;
	// fall through
    default:
	if ( mustWriteHash ) {
	    tag = (char)Tag_Hash;
	    stream.writeRawBytes( &tag, 1 );
	    stream << h;
	}
    }

    tag = (char)Tag_End;
    stream.writeRawBytes( &tag, 1 );
}


/*!
    Returns the widest lookup prefix that is common to this translator
    message and to message \a m.

    For example, if the extended key is for this message is (71,
    "PrintDialog", "Yes", "Print?") and that for \a m is (71,
    "PrintDialog", "No", "Print?"), this function returns \c
    HashContext.

    \sa write()
*/

TQTranslatorMessage::Prefix TQTranslatorMessage::commonPrefix(
	const TQTranslatorMessage& m ) const
{
    if ( h != m.h )
	return NoPrefix;
    if ( cx != m.cx )
	return Hash;
    if ( st != m.st )
	return HashContext;
    if ( cm != m.cm )
	return HashContextSourceText;
    return HashContextSourceTextComment;
}


/*!
 Returns true if the extended key of this object is equal to that of
 \a m; otherwise returns false.
*/

bool TQTranslatorMessage::operator==( const TQTranslatorMessage& m ) const
{
    return h == m.h && cx == m.cx && st == m.st && cm == m.cm;
}


/*!
    \fn bool TQTranslatorMessage::operator!=( const TQTranslatorMessage& m ) const

    Returns true if the extended key of this object is different from
    that of \a m; otherwise returns false.
*/


/*!
    Returns true if the extended key of this object is
    lexicographically before than that of \a m; otherwise returns
    false.
*/

bool TQTranslatorMessage::operator<( const TQTranslatorMessage& m ) const
{
    return h != m.h ? h < m.h
	   : ( cx != m.cx ? cx < m.cx
	     : (st != m.st ? st < m.st : cm < m.cm) );
}


/*!
    \fn bool TQTranslatorMessage::operator<=( const TQTranslatorMessage& m ) const

    Returns true if the extended key of this object is
    lexicographically before that of \a m or if they are equal;
    otherwise returns false.
*/

/*!
    \fn bool TQTranslatorMessage::operator>( const TQTranslatorMessage& m ) const

    Returns true if the extended key of this object is
    lexicographically after that of \a m; otherwise returns false.
*/

/*!
    \fn bool TQTranslatorMessage::operator>=( const TQTranslatorMessage& m ) const

    Returns true if the extended key of this object is
    lexicographically after that of \a m or if they are equal;
    otherwise returns false.
*/

#endif // TQT_NO_TRANSLATION
