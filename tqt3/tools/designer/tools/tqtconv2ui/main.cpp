/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt GUI Designer.
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

#include <ntqapplication.h>
#include <private/qpluginmanager_p.h>
#include <ntqstringlist.h>
#include <ntqdir.h>
#include <stdio.h>
#include "filterinterface.h"
#include <ntqfileinfo.h>

class Conv2ui {
public:
    Conv2ui();
    ~Conv2ui();
    bool reinit();
    bool convert( const TQString & filename, const TQDir & dest );
    TQStringList featureList();
private:
    TQString getTag( const TQString & filename );
    TQString absName( const TQString & filename );
    TQPluginManager<ImportFilterInterface> * importFiltersManager;
};

Conv2ui::Conv2ui() : importFiltersManager( 0 )
{
    reinit();
}

Conv2ui::~Conv2ui()
{
    if ( importFiltersManager )
	delete importFiltersManager;
}

bool Conv2ui::reinit()
{
    if ( importFiltersManager )
	delete importFiltersManager;
    importFiltersManager = new TQPluginManager<ImportFilterInterface>( IID_ImportFilter,
						      TQApplication::libraryPaths(),
						      "/designer");
    return !!importFiltersManager;
}

bool Conv2ui::convert( const TQString & filename, const TQDir & dest )
{
    if ( !importFiltersManager ) {
	tqWarning( "Conv2ui: no TQPluginManager was found" );
	return false;
    }

    if ( !TQFile::exists( absName( filename ) ) ) {
	tqWarning( "Conv2ui: can not find file %s", filename.latin1() );
	return false;
    }

    TQString tag = getTag( filename );
    TQStringList lst = importFiltersManager->featureList();

    for ( TQStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {

	ImportFilterInterface * interface = 0;
	importFiltersManager->queryInterface( *it, &interface );

	if ( !interface ) continue;

	if ( (*it).contains( tag, false ) ) {
	    // Make sure we output the files in the destination dir
	    TQString absfile = absName( filename );
	    TQString current = TQDir::currentDirPath();
	    TQDir::setCurrent( dest.absPath() );
	    interface->import( *it, absfile ); // resulting files go into the current dir
	    TQDir::setCurrent( current );
	}

	interface->release();

    }

    return true;
}

TQStringList Conv2ui::featureList()
{
    if ( !importFiltersManager ) {
	return 0;
    }
    return importFiltersManager->featureList();
}

TQString Conv2ui::getTag( const TQString & filename )
{
    TQFileInfo info( filename );
    return info.extension();
}

TQString Conv2ui::absName( const TQString & filename )
{
    TQString absFileName = TQDir::isRelativePath( filename ) ?
			  TQDir::currentDirPath() +
			  TQDir::separator() +
			  filename :
			  filename;
    return absFileName;
}

void printHelpMessage( TQStringList & formats )
{
    printf( "Usage: tqtconv2ui [options] <file> <destination directory>\n\n" );
    printf( "Options:\n");
    printf( "\t-help\t\tDisplay this information\n" );
    printf( "\t-silent\t\tDon't write any messages\n");
    printf( "\t-version\tDisplay version of tqtconv2ui\n");
    printf( "Supported file formats:\n" );
    for ( TQStringList::Iterator it = formats.begin(); it != formats.end(); ++it )
	printf( "\t%s\n", (*it).latin1() );
}

void printVersion()
{
    printf( "TQt user interface file converter for TQt version %s\n", TQT_VERSION_STR );
}

int main( int argc, char ** argv )
{
    Conv2ui conv;

    bool help = true;
    bool silent = false;
    bool version = false;
    bool unrecognized = false;

    // parse options
    int argi = 1;
    while ( argi < argc && argv[argi][0] == '-' ) {
	int i = 1;
	while ( argv[argi][i] != '\0' ) {

	    if ( argv[argi][i] == 's' ) {
		silent = true;
		break;
	    } else if ( argv[argi][i] == 'h' ) {
		help = true;
		break;
	    } else if ( argv[argi][i] == 'v' ) {
		version = true;
		silent = true;
		break;
	    } else {
		unrecognized = true;
	    }
	    i++;
	}
	argi++;
    }

    // get source file
    TQString src;
    if ( argv[argi] ) {
	src = argv[argi++];
    }

    // get destination directory
    TQDir dst;
    if ( argv[argi] ) {
	dst.setPath( argv[argi++] );
	help = false;
    }

    if ( !silent ) {
        printf( "TQt user interface file converter.\n" );
	if ( unrecognized ) {
	    printf( "tqtconv2ui: Unrecognized option\n" );
	}
    }
    if ( version ) {
	printVersion();
	return 0;
    } else if ( help ) {
	TQStringList formats = conv.featureList();
	printHelpMessage( formats );
	return 0;
    }

    // do the magic
    if ( conv.convert( src, dst ) )
	return 0;
    return -1;
}
