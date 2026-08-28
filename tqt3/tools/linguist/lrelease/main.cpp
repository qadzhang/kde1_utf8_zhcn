/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
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
#include <ntqregexp.h>
#include <ntqstring.h>
#include <ntqstringlist.h>
#include <ntqtextstream.h>

#include <errno.h>

typedef TQValueList<MetaTranslatorMessage> TML;

static void printUsage()
{
    fprintf( stderr, "Usage:\n"
	      "    lrelease [options] project-file\n"
	      "    lrelease [options] ts-files [-qm qm-file]\n"
	      "Options:\n"
	      "    -help  Display this information and exit\n"
	      "    -nocompress\n"
	      "           Do not compress the .qm files\n"
	      "    -verbose\n"
	      "           Explain what is being done\n"
	      "    -version\n"
	      "           Display the version of lrelease and exit\n" );
}

static bool loadTsFile( MetaTranslator& tor, const TQString& tsFileName,
			bool /* verbose */ )
{
    TQString qmFileName = tsFileName;
    qmFileName.replace( TQRegExp("\\.ts$"), "" );
    qmFileName += ".qm";

    bool ok = tor.load( tsFileName );
    if ( !ok )
	fprintf( stderr,
		 "lrelease warning: For some reason, I cannot load '%s'\n",
		 tsFileName.latin1() );
    return ok;
}

static void releaseMetaTranslator( const MetaTranslator& tor,
				   const TQString& qmFileName, bool verbose,
				   bool stripped )
{
    if ( verbose )
	fprintf( stderr, "Updating '%s'...\n", qmFileName.latin1() );
    if ( !tor.release(qmFileName, verbose,
		      stripped ? TQTranslator::Stripped
			       : TQTranslator::Everything) )
	fprintf( stderr,
		 "lrelease warning: For some reason, I cannot save '%s'\n",
		 qmFileName.latin1() );
}

static void releaseTsFile( const TQString& tsFileName, bool verbose,
			   bool stripped )
{
    MetaTranslator tor;
    if ( loadTsFile(tor, tsFileName, verbose) ) {
	TQString qmFileName = tsFileName;
	qmFileName.replace( TQRegExp("\\.ts$"), "" );
	qmFileName += ".qm";
	releaseMetaTranslator( tor, qmFileName, verbose, stripped );
    }
}

int main( int argc, char **argv )
{
    bool verbose = false;
    bool stripped = true;
    bool metTranslations = false;
    MetaTranslator tor;
    TQString outputFile;
    int numFiles = 0;
    int i;

    for ( i = 1; i < argc; i++ ) {
	if ( qstrcmp(argv[i], "-nocompress") == 0 ) {
	    stripped = false;
	    continue;
	} else if ( qstrcmp(argv[i], "-verbose") == 0 ) {
	    verbose = true;
	    continue;
	} else if ( qstrcmp(argv[i], "-version") == 0 ) {
	    fprintf( stderr, "lrelease version %s\n", TQT_VERSION_STR );
	    return 0;
	} else if ( qstrcmp(argv[i], "-qm") == 0 ) {
	    if ( i == argc - 1 ) {
		printUsage();
		return 1;
	    } else {
		i++;
		outputFile = argv[i];
		argv[i][0] = '-';
	    }
	} else if ( qstrcmp(argv[i], "-help") == 0 ) {
	    printUsage();
	    return 0;
	} else if ( argv[i][0] == '-' ) {
	    printUsage();
	    return 1;
	} else {
	    numFiles++;
	}
    }

    if ( numFiles == 0 ) {
	printUsage();
	return 1;
    }

    for ( i = 1; i < argc; i++ ) {
	if ( argv[i][0] == '-' )
	    continue;

	TQFile f( argv[i] );
	if ( !f.open(IO_ReadOnly) ) {
	    fprintf( stderr,
		     "lrelease error: Cannot open file '%s': %s\n", argv[i],
		     strerror(errno) );
	    return 1;
	}

	TQTextStream t( &f );
	TQString fullText = t.read();
	f.close();

	if ( fullText.find(TQString("<!DOCTYPE TS>")) >= 0 ) {
	    if ( outputFile.isEmpty() ) {
		releaseTsFile( argv[i], verbose, stripped );
	    } else {
		loadTsFile( tor, argv[i], verbose );
	    }
	} else {
	    TQString oldDir = TQDir::currentDirPath();
	    TQDir::setCurrent( TQFileInfo(argv[i]).dirPath() );

	    TQMap<TQString, TQString> tagMap = proFileTagMap( fullText );
	    TQMap<TQString, TQString>::Iterator it;

	    for ( it = tagMap.begin(); it != tagMap.end(); ++it ) {
        	TQStringList toks = TQStringList::split( ' ', it.data() );
		TQStringList::Iterator t;

        	for ( t = toks.begin(); t != toks.end(); ++t ) {
		    if ( it.key() == TQString("TRANSLATIONS") ) {
			metTranslations = true;
			releaseTsFile( *t, verbose, stripped );
		    }
		}
	    }
	    if ( !metTranslations )
		fprintf( stderr,
			 "lrelease warning: Met no 'TRANSLATIONS' entry in"
			 " project file '%s'\n",
			 argv[i] );
	    TQDir::setCurrent( oldDir );
	}
    }

    if ( !outputFile.isEmpty() )
	releaseMetaTranslator( tor, outputFile, verbose, stripped );

    return 0;
}
