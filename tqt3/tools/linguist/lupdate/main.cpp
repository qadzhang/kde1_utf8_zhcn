/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Linguist.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include <metatranslator.h>
#include <proparser.h>

#include <ntqdir.h>
#include <ntqfile.h>
#include <ntqfileinfo.h>
#include <ntqstring.h>
#include <ntqstringlist.h>
#include <ntqtextstream.h>

#include <errno.h>
#include <string.h>

// defined in fetchtr.cpp
extern void fetchtr_cpp( const char *fileName, MetaTranslator *tor,
			 const char *defaultContext, bool mustExist );
extern void fetchtr_ui( const char *fileName, MetaTranslator *tor,
			const char *defaultContext, bool mustExist );

// defined in merge.cpp
extern void merge( MetaTranslator *tor, const MetaTranslator *virginTor,
		   bool verbose );

typedef TQValueList<MetaTranslatorMessage> TML;

static void printUsage()
{
    fprintf( stderr, "Usage:\n"
	     "    lupdate [options] project-file\n"
	     "    lupdate [options] source-files -ts ts-files\n"
	     "Options:\n"
	     "    -help  Display this information and exit\n"
	     "    -noobsolete\n"
	     "           Drop all obsolete strings\n"
	     "    -verbose\n"
	     "           Explain what is being done\n"
	     "    -version\n"
	     "           Display the version of lupdate and exit\n" );
}

static void updateTsFiles( const MetaTranslator& fetchedTor,
			   const TQStringList& tsFileNames, const TQString& codec,
			   bool noObsolete, bool verbose )
{
    TQStringList::ConstIterator t = tsFileNames.begin();
    while ( t != tsFileNames.end() ) {
	MetaTranslator tor;
	tor.load( *t );
	if ( !codec.isEmpty() )
	    tor.setCodec( codec );
	if ( verbose )
	    fprintf( stderr, "Updating '%s'...\n", (*t).latin1() );
	merge( &tor, &fetchedTor, verbose );
	if ( noObsolete )
	    tor.stripObsoleteMessages();
	tor.stripEmptyContexts();
	if ( !tor.save(*t) )
	    fprintf( stderr, "lupdate error: Cannot save '%s': %s\n",
		     (*t).latin1(), strerror(errno) );
	++t;
    }
}

int main( int argc, char **argv )
{
    TQString defaultContext = "@default";
    MetaTranslator fetchedTor;
    TQCString codec;
    TQStringList tsFileNames;

    bool verbose = false;
    bool noObsolete = false;
    bool metSomething = false;
    int numFiles = 0;
    bool standardSyntax = true;
    bool metTsFlag = false;

    int i;

    for ( i = 1; i < argc; i++ ) {
	if ( qstrcmp(argv[i], "-ts") == 0 )
	    standardSyntax = false;
    }

    for ( i = 1; i < argc; i++ ) {
	if ( qstrcmp(argv[i], "-help") == 0 ) {
	    printUsage();
	    return 0;
	} else if ( qstrcmp(argv[i], "-noobsolete") == 0 ) {
	    noObsolete = true;
	    continue;
	} else if ( qstrcmp(argv[i], "-verbose") == 0 ) {
	    verbose = true;
	    continue;
	} else if ( qstrcmp(argv[i], "-version") == 0 ) {
	    fprintf( stderr, "lupdate version %s\n", TQT_VERSION_STR );
	    return 0;
	} else if ( qstrcmp(argv[i], "-ts") == 0 ) {
	    metTsFlag = true;
	    continue;
	}

	numFiles++;

	TQString fullText;

	if ( !metTsFlag ) {
	    TQFile f( argv[i] );
	    if ( !f.open(IO_ReadOnly) ) {
		fprintf( stderr, "lupdate error: Cannot open file '%s': %s\n",
			 argv[i], strerror(errno) );
		return 1;
	    }

	    TQTextStream t( &f );
	    fullText = t.read();
	    f.close();
	}

	TQString oldDir = TQDir::currentDirPath();
	TQDir::setCurrent( TQFileInfo(argv[i]).dirPath() );

	if ( standardSyntax ) {
	    fetchedTor = MetaTranslator();
	    codec.truncate( 0 );
	    tsFileNames.clear();

	    TQMap<TQString, TQString> tagMap = proFileTagMap( fullText );
	    TQMap<TQString, TQString>::Iterator it;

	    for ( it = tagMap.begin(); it != tagMap.end(); ++it ) {
        	TQStringList toks = TQStringList::split( ' ', it.data() );
		TQStringList::Iterator t;

        	for ( t = toks.begin(); t != toks.end(); ++t ) {
                    if ( it.key() == "HEADERS" || it.key() == "SOURCES" ) {
                	fetchtr_cpp( *t, &fetchedTor, defaultContext, true );
                	metSomething = true;
                    } else if ( it.key() == "INTERFACES" ||
				it.key() == "FORMS" ) {
                	fetchtr_ui( *t, &fetchedTor, defaultContext, true );
			fetchtr_cpp( *t + ".h", &fetchedTor, defaultContext,
				     false );
                	metSomething = true;
                    } else if ( it.key() == "TRANSLATIONS" ) {
                	tsFileNames.append( *t );
                	metSomething = true;
                    } else if ( it.key() == "CODEC" ||
				it.key() == "DEFAULTCODEC" ) {
                	codec = (*t).latin1();
                    }
        	}
            }

	    updateTsFiles( fetchedTor, tsFileNames, codec, noObsolete,
			   verbose );

	    if ( !metSomething ) {
		fprintf( stderr,
			 "lupdate warning: File '%s' does not look like a"
			 " project file\n",
			 argv[i] );
	    } else if ( tsFileNames.isEmpty() ) {
		fprintf( stderr,
			 "lupdate warning: Met no 'TRANSLATIONS' entry in"
			 " project file '%s'\n",
			 argv[i] );
	    }
	} else {
	    if ( metTsFlag ) {
		if ( TQString(argv[i]).lower().endsWith(".ts") ) {
		    TQFileInfo fi( argv[i] );
		    if ( !fi.exists() || fi.isWritable() ) {
			tsFileNames.append( argv[i] );
		    } else {
			fprintf( stderr,
				 "lupdate warning: For some reason, I cannot"
				 " save '%s'\n",
				 argv[i] );
		    }
		} else {
		    fprintf( stderr,
			     "lupdate error: File '%s' lacks .ts extension\n",
			     argv[i] );
		}
	    } else {
		TQFileInfo fi(argv[i]);
		if ( TQString(argv[i]).lower().endsWith(".ui") ) {
        	    fetchtr_ui( fi.fileName(), &fetchedTor, defaultContext, true );
		    fetchtr_cpp( TQString(fi.fileName()) + ".h", &fetchedTor,
				 defaultContext, false );
		} else {
        	    fetchtr_cpp( fi.fileName(), &fetchedTor, defaultContext, true );
		}
	    }
	}
	TQDir::setCurrent( oldDir );
    }

    if ( !standardSyntax )
	updateTsFiles( fetchedTor, tsFileNames, codec, noObsolete, verbose );

    if ( numFiles == 0 ) {
	printUsage();
	return 1;
    }
    return 0;
}
