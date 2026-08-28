/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
**1 This file is part of TQt Designer.
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

#include "project.h"
#include "formwindow.h"
#include "designerappiface.h"
#include "../interfaces/languageinterface.h"
#include "pixmapcollection.h"
#ifndef TQT_NO_SQL
#include "dbconnectionimpl.h"
#endif
#include "resource.h"
#include <ntqwidgetfactory.h>
#include "outputwindow.h"

#include <ntqfile.h>
#include <ntqtextstream.h>
#include <ntqurl.h>
#include <ntqobjectlist.h>
#include <ntqfeatures.h>
#include <ntqtextcodec.h>
#include <ntqdom.h>
#include <ntqmessagebox.h>
#include <ntqapplication.h>
#include "mainwindow.h"
#include <ntqworkspace.h>

#ifndef TQT_NO_SQL
#include <ntqsqldatabase.h>
#include <ntqsqlrecord.h>
#include <ntqdatatable.h>
#endif

#include <stdlib.h>
#ifdef Q_OS_UNIX
# include <unistd.h>
#endif

#ifndef TQT_NO_SQL
DatabaseConnection::~DatabaseConnection()
{
    delete iface;
}

bool DatabaseConnection::refreshCatalog()
{
#ifndef TQT_NO_SQL
    if ( loaded )
	return true;
    if ( !open() )
	return false;
    tbls = conn->tables( TQSql::TableType( TQSql::Tables | TQSql::Views ) );
    flds.clear();
    for ( TQStringList::Iterator it = tbls.begin(); it != tbls.end(); ++it ) {
	TQSqlRecord fil = conn->record( *it );
	TQStringList lst;
	for ( uint j = 0; j < fil.count(); ++j )
	    lst << fil.field( j )->name();
	flds.insert( *it, lst );
    }
    loaded = true;
    conn->close();
    return loaded;
#else
    return false;
#endif
}

#ifndef TQT_NO_SQL
void DatabaseConnection::remove()
{
    if ( nm == "(default)" )
	TQSqlDatabase::removeDatabase( TQSqlDatabase::defaultConnection );
    else
	TQSqlDatabase::removeDatabase( nm );
    // the above will effectively delete the current connection
    conn = 0;
}
#endif

bool DatabaseConnection::open( bool suppressDialog )
{
#ifndef TQT_NO_SQL
    // register our name, if nec
    if ( nm == "(default)" ) {
	if ( !TQSqlDatabase::contains() ) // default doesn't exists?
	    conn = TQSqlDatabase::addDatabase( drv );
	else
	    conn = TQSqlDatabase::database();
    } else {
	if ( !TQSqlDatabase::contains( nm ) )
	    conn = TQSqlDatabase::addDatabase( drv, nm );
	else
	    conn = TQSqlDatabase::database( nm );
    }
    conn->setDatabaseName( dbName );
    conn->setUserName( uname );
    conn->setPassword( pword );
    conn->setHostName( hname );
    conn->setPort( prt );
    bool success = conn->open();
    for( ; suppressDialog == false ; ) {
	bool done = false;
	if ( !success ) {
	    DatabaseConnectionEditor dia( this, 0  , 0 , true );
	    switch( dia.exec() ) {
	    case TQDialog::Accepted:
		done = false;
		break;
	    case TQDialog::Rejected:
		done = true;
		break;
	    }
	}
	if ( done )
	    break;
	conn->setUserName( uname );
	conn->setPassword( pword );
	conn->setHostName( hname );
	conn->setPort( prt );
	success = conn->open();
	if ( !success ) {
	    switch( TQMessageBox::warning( project->messageBoxParent(), TQApplication::tr( "Connection" ),
					  TQApplication::tr( "Could not connect to the database.\n"
							    "Press 'OK' to continue or 'Cancel' to "
							    "specify different\nconnection information.\n" )
					  + TQString( "[" + conn->lastError().driverText() + "\n" +
						     conn->lastError().databaseText() + "]\n" ),
					  TQApplication::tr( "&OK" ),
					  TQApplication::tr( "&Cancel" ), TQString::null, 0, 1 ) ) {
	    case 0: // OK or Enter
		continue;
	    case 1: // Cancel or Escape
		done = true;
		break;
	    }
	} else
	    break;
	if ( done )
	    break;
    }
    if ( !success ) {
	dbErr = conn->lastError().driverText() + "\n" + conn->lastError().databaseText();
	remove();
    }
    return success;
#else
    return false;
#endif
}

void DatabaseConnection::close()
{
    if ( !loaded )
	return;
#ifndef TQT_NO_SQL
    if ( conn ) {
	conn->close();
    }
#endif
}

DesignerDatabase *DatabaseConnection::iFace()
{
    if ( !iface )
	iface = new DesignerDatabaseImpl( this );
    return iface;
}

#endif

////////

bool Project::isDummy() const
{
    return isDummyProject;
}

Project::Project( const TQString &fn, const TQString &pName,
		  TQPluginManager<ProjectSettingsInterface> *pm, bool isDummy,
		  const TQString &l )
    : proName( pName ), projectSettingsPluginManager( pm ), isDummyProject( isDummy )
{
    modified = true;
    pixCollection = new PixmapCollection( this );
    iface = 0;
    lang = l;
    is_cpp = lang == "C++";
    cfg.insert( "(all)", "qt warn_on release" );
    templ = "app";
    setFileName( fn );
    if ( !pName.isEmpty() )
	proName = pName;
    sourcefiles.setAutoDelete( true );
    modified = false;
    objs.setAutoDelete( false );
    fakeFormFiles.setAutoDelete( false );
}

Project::~Project()
{
    if ( singleProjectMode() )
	removeTempProject();
    delete iface;
    delete pixCollection;
}

void Project::setModified( bool b )
{
    modified = b;
    emit projectModified();
}

#ifndef TQT_NO_SQL
DatabaseConnection *Project::databaseConnection( const TQString &name )
{
    for ( DatabaseConnection *conn = dbConnections.first();
	  conn;
	  conn = dbConnections.next() ) {
	if ( conn->name() == name )
	    return conn;
    }
    return 0;
}
#endif

void Project::setFileName( const TQString &fn, bool doClear )
{
    if ( fn == filename )
	return;

    if ( singleProjectMode() ) {
	TQString qsa = TQString( getenv( "HOME" ) ) + TQString( "/.qsa" );
	if ( !TQFile::exists( qsa ) ) {
	    TQDir d;
	    d.mkdir( qsa );
	}
	if ( fn == singleProFileName )
	    return;
	singleProFileName = fn;
	static int counter = 0;
	TQString str_counter = TQString::number( counter++ );
	str_counter = "/.qsa/" + str_counter;
	LanguageInterface *iface = MetaDataBase::languageInterface( language() );
	filename = TQString( getenv( "HOME" ) + str_counter + TQString( "tmp_" ) +
			    TQFileInfo( fn ).baseName() + "/" + TQFileInfo( fn ).baseName() + ".pro" );
	removeTempProject();
	if ( iface && iface->supports( LanguageInterface::CompressProject ) ) {
	    filename = iface->uncompressProject( makeAbsolute( singleProFileName ),
						 TQString( getenv( "HOME" ) +
							  str_counter + TQString( "tmp_" ) +
							  TQFileInfo( fn ).baseName() ) );
	    proName = makeAbsolute( singleProFileName );
	}
    } else {
	filename = fn;
	if ( !filename.endsWith( ".pro" ) )
	    filename += ".pro";
	proName = filename;
    }


    if ( proName.contains( '.' ) )
	proName = proName.left( proName.find( '.' ) );

    if ( !doClear )
	return;
    clear();
    if ( TQFile::exists( filename ) )
	parse();
}

TQString Project::fileName( bool singlePro ) const
{
    if ( singlePro )
	return singleProFileName;
    return filename;
}

TQString Project::databaseDescription() const
{
    return dbFile;
}

TQString Project::projectName() const
{
    return proName;
}

static TQString parse_part( const TQString &part )
{
    TQString res;
    bool inName = false;
    TQString currName;
    for ( int i = 0; i < (int)part.length(); ++i ) {
	TQChar c = part[ i ];
	if ( !inName ) {
	    if ( c != ' ' && c != '\t' && c != '\n' && c != '=' && c != '\\' && c != '+' )
		inName = true;
	    else
		continue;
	}
	if ( inName ) {
	    if ( c == '\n' )
		break;
	    res += c;
	}
    }
    return res;
}

TQStringList parse_multiline_part( const TQString &contents, const TQString &key, int *start = 0 )
{
    if ( start )
	*start = -1;
    TQString lastWord;
    // Account for things like win32: SOURCES
    int extraWhiteSpaceCount = 0;
    int braceCount = 0;
    for ( int i = 0; i < (int)contents.length(); ++i ) {
	TQChar c( contents[ i ] );
	switch ( c ) {
	case '{':
	    braceCount++;
	    lastWord = "";
	    break;
	case '}':
	    braceCount--;
	    lastWord = "";
	    break;
	case ' ': case '\t':
	    if (!key.startsWith(lastWord)) {
		lastWord = "";
		extraWhiteSpaceCount = 0;
	    } else {
		extraWhiteSpaceCount++;
	    }
	    break;
	case '\\': case '\n':
	    lastWord = "";
	    break;
        case '#':
            while ( contents[ i ] != '\n' && i < (int)contents.length() )
                ++i;
            break;
	default:
	    lastWord += c;
	}

	// ### we should read the 'bla { SOURCES= ... }' stuff as well (braceCount > 0)
	if ( lastWord == key && braceCount == 0 ) {
	    if ( start )
		*start = i - lastWord.length() - extraWhiteSpaceCount + 1;
	    TQStringList lst;
	    bool inName = false;
	    TQString currName;
	    bool hadEqual = false;
	    for ( ; i < (int)contents.length(); ++i ) {
		c = contents[ i ];
		if ( !hadEqual && c != '=' )
		    continue;
		if ( !hadEqual ) {
		    hadEqual = true;
		    continue;
		}
		if ( ( c.isLetter() ||
                       c.isDigit() ||
                       c == '.' ||
                       c == '/' ||
                       c == '_' ||
                       c == '\\' ||
		       c == '\"' ||
                       c == '\'' ||
                       c == '=' ||
                       c == '$' ||
                       c == '-' ||
                       c == '(' ||
                       c == ')' ||
                       c == ':' ||
                       c == '+' ||
                       c == ',' ||
                       c == '~' ||
                       c == '{' ||
                       c == '}' ||
                       c == '@' ) &&
		     c != ' ' && c != '\t' && c != '\n' ) {
		    if ( !inName )
			currName = TQString::null;
		    if ( c != '\\' || contents[i+1] != '\n' ) {
			currName += c;
			inName = true;
		    }
		} else {
		    if ( inName ) {
			inName = false;
			if ( currName.simplifyWhiteSpace() != "\\" )
			    lst.append( currName );
		    }
		    if ( c == '\n' && i > 0 && contents[ (int)i - 1 ] != '\\' )
			break;
		}
	    }
	    return lst;
	}
    }

    return TQStringList();
}

void Project::parse()
{
    TQFile f( filename );
    if ( !f.exists() || !f.open( IO_ReadOnly ) )
	return;
    TQTextStream ts( &f );
    TQString contents = ts.read();
    f.close();

    proName = TQFileInfo( filename ).baseName();

    TQStringList::ConstIterator it;

    int i = contents.find( "LANGUAGE" );
    if ( i != -1 ) {
	lang = "";
	is_cpp = false;
	TQString part = contents.mid( i + TQString( "LANGUAGE" ).length() );
	lang = parse_part( part );
	is_cpp = lang == "C++";
    }

    i = contents.find( "DBFILE" );
    if ( i != -1 ) {
	dbFile = "";
	TQString part = contents.mid( i + TQString( "DBFILE" ).length() );
	dbFile = parse_part( part );
    }

    TQStringList uifiles = parse_multiline_part( contents, "FORMS" );
    uifiles += parse_multiline_part( contents, "INTERFACES" ); // compatibility
    for ( it = uifiles.begin(); it != uifiles.end(); ++it ) {
	if ( (*it).startsWith( "__APPOBJ" ) )
	    continue;
	(void) new FormFile( *it, false, this );
    }


    i = contents.find( "TEMPLATE" );
    if ( i != -1 ) {
	templ = "";
	TQString part = contents.mid( i + TQString( "TEMPLATE" ).length() );
	templ = parse_part( part );
    }

    readPlatformSettings( contents, "CONFIG", cfg );
    readPlatformSettings( contents, "LIBS", lbs );
    readPlatformSettings( contents, "INCLUDEPATH", inclPath );
    readPlatformSettings( contents, "DEFINES", defs );
    readPlatformSettings( contents, "SOURCES", sources );
    readPlatformSettings( contents, "HEADERS", headers );

    LanguageInterface *iface = MetaDataBase::languageInterface( lang );
    if ( iface ) {
	TQStringList sourceKeys;
	iface->sourceProjectKeys( sourceKeys );
	for ( TQStringList::Iterator it = sourceKeys.begin(); it != sourceKeys.end(); ++it ) {
	    TQStringList lst = parse_multiline_part( contents, *it );
	    for ( TQStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
		(void) new SourceFile( *it, false, this );
	}
    }

    updateCustomSettings();

    for ( it = csList.begin(); it != csList.end(); ++it ) {
	i = contents.find( *it );
	if ( i != -1 ) {
	    TQString val = "";
	    TQString part = contents.mid( i + TQString( *it ).length() );
	    val = parse_part( part );
	    customSettings.replace( *it, val );
	}
    }

    loadConnections();

    TQStringList images = parse_multiline_part( contents, "IMAGES" );

    // ### remove that for the final - this is beta-compatibility
    if ( images.isEmpty() && TQDir( TQFileInfo( filename ).dirPath( true ) + "/images" ).exists() ) {
	    images = TQDir( TQFileInfo( filename ).dirPath( true ) + "/images" ).entryList();
	    for ( int i = 0; i < (int)images.count(); ++i )
		images[ i ].prepend( "images/" );
	    modified = true;
    }

    for ( TQStringList::ConstIterator pit = images.begin(); pit != images.end(); ++pit )
	pixCollection->load( *pit );
}

void Project::clear()
{
    dbFile = "";
    proName = "unnamed";
    desc = "";
}

bool Project::removeSourceFile( SourceFile *sf )
{
    if ( !sourcefiles.containsRef( sf ) )
	return false;
    if ( !sf->close() )
	return false;
    sourcefiles.removeRef( sf );
    modified = true;
    emit sourceFileRemoved( sf );
    return true;
}

void Project::setDatabaseDescription( const TQString &db )
{
    dbFile = db;
}

void Project::setDescription( const TQString &s )
{
    desc = s;
}

TQString Project::description() const
{
    return desc;
}


bool Project::isValid() const
{
     // #### do more checking here?
    if ( filename.isEmpty() || proName.isEmpty() )
	return false;

    return true;
}

TQString Project::makeAbsolute( const TQString &f )
{
    if ( isDummy() )
	return f;
    TQString encodedUrl = TQFileInfo( filename ).dirPath( true );
    TQUrl::encode( encodedUrl );
    TQUrl u( encodedUrl, f );
    return u.path();
}

TQString Project::makeRelative( const TQString &f )
{
    if ( isDummy() )
	return f;
    TQString p = TQFileInfo( filename ).dirPath( true );    
    TQString f2 = f;
#if defined(TQ_WS_WIN32)
    if ( p.endsWith("/") )
        p = p.left( p.length() - 1 );
    if ( f2.left( p.length() ).lower() == p.lower() )
#else
    if ( f2.left( p.length() ) == p )
#endif
	    f2.remove( 0, p.length() + 1 );
    return f2;
}

static void remove_contents( TQString &contents, const TQString &s )
{
    int i = contents.find( s );
    if ( i != -1 ) {
	int start = i;
	int end = contents.find( '\n', i );
	if ( end == -1 )
	    end = contents.length() - 1;
	contents.remove( start, end - start + 1 );
    }
}

static void remove_multiline_contents( TQString &contents, const TQString &s, int *strt = 0 )
{
    int i = contents.find( s );
    int startBrace = contents.findRev("{", i);
    int endBrace = contents.findRev("}", i);
    // We found a startBrace before i, but no endBrace after the startBrace before i
    if (startBrace != -1 && endBrace < startBrace)
        return;
    if ( strt )
        *strt = i;
    int start = i;
    bool lastWasBackspash = true;
    if ( i != -1 && ( i == 0 || contents[ i - 1 ] != '{' || contents[ i - 1 ] != ':' ) && !(i > 0 && contents[i-1] != '\n')) {
	for ( ; i < (int)contents.length(); ++i ) {
	    if ( contents[ i ] == '\n' && !lastWasBackspash )
		break;
	    lastWasBackspash = ( contents[ i ] == '\\' ||
				 ( lastWasBackspash && ( contents[ i ] == ' ' || contents[ i ] == '\t' ) ) );
	}
	contents.remove( start, i - start + 1 );
    }
}

void Project::save( bool onlyProjectFile )
{
    bool anythingModified = false;

    //  save sources and forms
    if ( !onlyProjectFile ) {

	saveConnections();

	for ( SourceFile *sf = sourcefiles.first(); sf; sf = sourcefiles.next() ) {
	    anythingModified = anythingModified || sf->isModified();
	    if ( !sf->save() )
		return;
	}

	for ( FormFile *ff = formfiles.first(); ff; ff = formfiles.next() ) {
	    anythingModified = anythingModified || ff->isModified();
	    if ( !ff->save() )
		return;
	}
    }

    if ( isDummy() || filename.isEmpty() )
	return;

    if ( !modified ) {
	if ( singleProjectMode() ) {
	    LanguageInterface *iface = MetaDataBase::languageInterface( language() );
	    if ( iface && iface->supports( LanguageInterface::CompressProject ) )
		iface->compressProject( makeAbsolute( filename ), singleProFileName, anythingModified );
	}
 	return;
    }

    TQFile f( filename );
    TQString original = "";

    // read the existing file
    bool hasPreviousContents = false;
    if ( f.open( IO_ReadOnly ) ) {
	TQTextStream ts( &f );
	original = ts.read();
	f.close();
        hasPreviousContents = true;
	remove_contents( original, "{SOURCES+=" ); // ### compatibility with early 3.0 betas
	remove_contents( original, "DBFILE" );
	remove_contents( original, "LANGUAGE" );
	remove_contents( original, "TEMPLATE" );
	removePlatformSettings( original, "CONFIG" );
	removePlatformSettings( original, "DEFINES" );
	removePlatformSettings( original, "LIBS" );
	removePlatformSettings( original, "INCLUDEPATH" );
	removePlatformSettings( original, "SOURCES" );
	removePlatformSettings( original, "HEADERS" );
	remove_multiline_contents( original, "FORMS" );
	remove_multiline_contents( original, "INTERFACES" ); // compatibility
	remove_multiline_contents( original, "IMAGES" );
	for ( TQStringList::Iterator it = csList.begin(); it != csList.end(); ++it )
	    remove_contents( original, *it );
    }

    if (!original.isEmpty()) {
	// Removes any new lines at the beginning of the file
	while (original.startsWith("\n"))
	    original.remove(0, 1);
    }

    // the contents of the saved file
    TQString contents;

    // template
    contents += "TEMPLATE\t= " + templ + "\n";

    // language
    contents += "LANGUAGE\t= " + lang + "\n";
    contents += "\n";

    // config
    writePlatformSettings( contents, "CONFIG", cfg );
    LanguageInterface *iface = MetaDataBase::languageInterface( lang );
    if ( iface ) {
	TQStringList sourceKeys;
	iface->sourceProjectKeys( sourceKeys );
	for ( TQStringList::Iterator spit = sourceKeys.begin(); spit != sourceKeys.end(); ++spit )
	    remove_multiline_contents( contents, *spit );
    }

    // libs, defines, includes
    writePlatformSettings( contents, "LIBS", lbs );
    writePlatformSettings( contents, "DEFINES", defs );
    writePlatformSettings( contents, "INCLUDEPATH", inclPath );
    writePlatformSettings( contents, "SOURCES", sources );
    writePlatformSettings( contents, "HEADERS", headers );

    // unix
    if ( !hasPreviousContents ) {
 	contents +=
 	    "unix {\n"
 	    "  UI_DIR = .ui\n"
 	    "  MOC_DIR = .moc\n"
 	    "  OBJECTS_DIR = .obj\n"
 	    "}\n\n";
    }

    // sources
    if ( !sourcefiles.isEmpty() && iface ) {
	TQMap<TQString, TQStringList> sourceToKey;
	for ( SourceFile *f = sourcefiles.first(); f; f = sourcefiles.next() ) {
	    TQString key = iface->projectKeyForExtension( TQFileInfo( f->fileName() ).extension() );
	    TQStringList lst = sourceToKey[ key ];
	    lst << makeRelative( f->fileName() );
	    sourceToKey.replace( key, lst );
	}

	for ( TQMap<TQString, TQStringList>::Iterator skit = sourceToKey.begin();
	      skit != sourceToKey.end(); ++skit ) {
	    TQString part = skit.key() + "\t+= ";
	    TQStringList lst = *skit;
	    for ( TQStringList::Iterator sit = lst.begin(); sit != lst.end(); ++sit ) {
		part += *sit;
		part += ++sit != lst.end() ? " \\\n\t" : "";
		--sit;
	    }
	    part += "\n\n";
	    contents += part;
	}
    }

    // forms and interfaces
    if ( !formfiles.isEmpty() ) {
	contents += "FORMS\t= ";
	for ( TQPtrListIterator<FormFile> fit = formfiles; fit.current(); ++fit ) {
	    contents += fit.current()->fileName() +
		 (fit != formfiles.last() ? " \\\n\t" : "");
	}
	contents += "\n\n";
    }

    // images
     if ( !pixCollection->isEmpty() ) {
	contents += "IMAGES\t= ";
	TQValueList<PixmapCollection::Pixmap> pixmaps = pixCollection->pixmaps();
	for ( TQValueList<PixmapCollection::Pixmap>::Iterator it = pixmaps.begin();
	      it != pixmaps.end(); ++it ) {
		  contents += makeRelative( (*it).absname );
		  contents += ++it != pixmaps.end() ? " \\\n\t" : "";
		  --it;
	}
	contents += "\n\n";
    }

    // database
    if ( !dbFile.isEmpty() )
	contents += "DBFILE\t= " + dbFile + "\n";

    // custom settings
    for ( TQStringList::Iterator it = csList.begin(); it != csList.end(); ++it ) {
	TQString val = *customSettings.find( *it );
	if ( !val.isEmpty() )
	    contents += *it + "\t= " + val + "\n";
    }

    if ( !f.open( IO_WriteOnly | IO_Translate ) ) {
	TQMessageBox::warning( messageBoxParent(),
			      "Save Project Failed", "Couldn't write project file " + filename );
	return;
    }

    TQTextStream os( &f );
    os << contents;
    if (hasPreviousContents)
        os << original;

    f.close();

    setModified( false );

    if ( singleProjectMode() ) {
	LanguageInterface *iface = MetaDataBase::languageInterface( language() );
	if ( iface && iface->supports( LanguageInterface::CompressProject ) )
	    iface->compressProject( makeAbsolute( filename ), singleProFileName, true );
    }
}

#ifndef TQT_NO_SQL
TQPtrList<DatabaseConnection> Project::databaseConnections() const
{
    return dbConnections;
}
#endif

#ifndef TQT_NO_SQL
void Project::setDatabaseConnections( const TQPtrList<DatabaseConnection> &lst )
{
    dbConnections = lst;
}
#endif

#ifndef TQT_NO_SQL
void Project::addDatabaseConnection( DatabaseConnection *conn )
{
    dbConnections.append( conn );
    modified = true;
}
#endif

#ifndef TQT_NO_SQL
void Project::removeDatabaseConnection( const TQString &c )
{
    for ( DatabaseConnection *conn = dbConnections.first(); conn; conn = dbConnections.next() ) {
	if ( conn->name() == c ) {
	    conn->remove();
	    dbConnections.removeRef( conn );
	    delete conn;
	    return;
	}
    }
}
#endif

#ifndef TQT_NO_SQL
TQStringList Project::databaseConnectionList()
{
    TQStringList lst;
    for ( DatabaseConnection *conn = dbConnections.first(); conn; conn = dbConnections.next() )
	lst << conn->name();
    return lst;
}
#endif

#ifndef TQT_NO_SQL
TQStringList Project::databaseTableList( const TQString &connection )
{
    DatabaseConnection *conn = databaseConnection( connection );
    if ( !conn ) {
	return TQStringList();
    }
    return conn->tables();
}
#endif

#ifndef TQT_NO_SQL
TQStringList Project::databaseFieldList( const TQString &connection, const TQString &table )
{
    DatabaseConnection *conn = databaseConnection( connection );
    if ( !conn )
	return TQStringList();
    return conn->fields( table );
}
#endif

#ifndef TQT_NO_SQL
static TQString makeIndent( int indent )
{
    TQString s;
    s.fill( ' ', indent * 4 );
    return s;
}
#endif

#ifndef TQT_NO_SQL
static void saveSingleProperty( TQTextStream &ts, const TQString& name, const TQString& value, int indent )
{
    ts << makeIndent( indent ) << "<property name=\"" << name << "\">" << endl;
    ++indent;
    ts << makeIndent( indent ) << "<string>" << value << "</string>" << endl;
    --indent;
    ts << makeIndent( indent ) << "</property>" << endl;
}
#endif

void Project::saveConnections()
{
#ifndef TQT_NO_SQL
    if ( dbFile.isEmpty() ) {
	TQFileInfo fi( fileName() );
	setDatabaseDescription( fi.baseName() + ".db" );
    }

    TQFile f( makeAbsolute( dbFile ) );

    if ( dbConnections.isEmpty() ) {
	if ( f.exists() )
	    f.remove();
	setDatabaseDescription( "" );
	modified = true;
	return;
    }

    /* .db xml */
    if ( f.open( IO_WriteOnly | IO_Translate ) ) {
	TQTextStream ts( &f );
	ts.setCodec( TQTextCodec::codecForName( "UTF-8" ) );
	ts << "<!DOCTYPE DB><DB version=\"1.0\">" << endl;

	/* db connections */
	int indent = 0;
	for ( DatabaseConnection *conn = dbConnections.first(); conn; conn = dbConnections.next() ) {
	    ts << makeIndent( indent ) << "<connection>" << endl;
	    ++indent;
	    saveSingleProperty( ts, "name", conn->name(), indent );
	    saveSingleProperty( ts, "driver", conn->driver(), indent );
	    saveSingleProperty( ts, "database", conn->database(), indent );
	    saveSingleProperty( ts, "username", conn->username(), indent );
	    saveSingleProperty( ts, "hostname", conn->hostname(), indent );
	    saveSingleProperty( ts, "port", TQString::number( conn->port() ), indent );

	    /* connection tables */
	    TQStringList tables = conn->tables();
	    for ( TQStringList::Iterator it = tables.begin();
		  it != tables.end(); ++it ) {
		ts << makeIndent( indent ) << "<table>" << endl;
		++indent;
		saveSingleProperty( ts, "name", (*it), indent );

		/* tables fields */
		TQStringList fields = conn->fields( *it );
		for ( TQStringList::Iterator it2 = fields.begin();
		      it2 != fields.end(); ++it2 ) {
		    ts << makeIndent( indent ) << "<field>" << endl;
		    ++indent;
		    saveSingleProperty( ts, "name", (*it2), indent );
		    --indent;
		    ts << makeIndent( indent ) << "</field>" << endl;
		}

		--indent;
		ts << makeIndent( indent ) << "</table>" << endl;
	    }

	    --indent;
	    ts << makeIndent( indent ) << "</connection>" << endl;
	}

	ts << "</DB>" << endl;
	f.close();
    }
#endif
}

#ifndef TQT_NO_SQL
static TQDomElement loadSingleProperty( TQDomElement e, const TQString& name )
{
    TQDomElement n;
    for ( n = e.firstChild().toElement();
	  !n.isNull();
	  n = n.nextSibling().toElement() ) {
	if ( n.tagName() == "property" && n.toElement().attribute("name") == name )
	    return n;
    }
    return n;
}
#endif

void Project::loadConnections()
{
#ifndef TQT_NO_SQL
    if ( dbFile.isEmpty() || !TQFile::exists( makeAbsolute( dbFile ) ) )
	return;

    TQFile f( makeAbsolute( dbFile ) );
    if ( f.open( IO_ReadOnly ) ) {
	TQDomDocument doc;
	TQString errMsg;
	int errLine;
	if ( doc.setContent( &f, &errMsg, &errLine ) ) {
	    TQDomElement e;
	    e = doc.firstChild().toElement();

	    /* connections */
	    TQDomNodeList connections = e.toElement().elementsByTagName( "connection" );
	    for ( uint i = 0; i <  connections.length(); i++ ) {
		TQDomElement connection = connections.item(i).toElement();
		TQDomElement connectionName = loadSingleProperty( connection, "name" );
		TQDomElement connectionDriver = loadSingleProperty( connection, "driver" );
		TQDomElement connectionDatabase = loadSingleProperty( connection,
								     "database" );
		TQDomElement connectionUsername = loadSingleProperty( connection,
								     "username" );
		TQDomElement connectionHostname = loadSingleProperty( connection,
								     "hostname" );
		TQDomElement connectionPort = loadSingleProperty( connection,
								     "port" );

		DatabaseConnection *conn = new DatabaseConnection( this );
		conn->setName( connectionName.firstChild().firstChild().toText().data() );
		conn->setDriver( connectionDriver.firstChild().firstChild().toText().data() );
		conn->setDatabase( connectionDatabase.firstChild().firstChild().toText().data() );
		conn->setUsername( connectionUsername.firstChild().firstChild().toText().data() );
		conn->setHostname( connectionHostname.firstChild().firstChild().toText().data() );
		conn->setPort( TQString( connectionPort.firstChild().firstChild().toText().data() ).toInt() );

		/* connection tables */
		TQDomNodeList tables = connection.toElement().elementsByTagName( "table" );
		for ( uint j = 0; j <  tables.length(); j++ ) {
		    TQDomElement table = tables.item(j).toElement();
		    TQDomElement tableName = loadSingleProperty( table, "name" );
		    conn->addTable( tableName.firstChild().firstChild().toText().data() );

		    /* table fields */
		    TQStringList fieldList;
		    TQDomNodeList fields = table.toElement().elementsByTagName( "field" );
		    for ( uint k = 0; k <  fields.length(); k++ ) {
			TQDomElement field = fields.item(k).toElement();
			TQDomElement fieldName = loadSingleProperty( field, "name" );
			fieldList.append( fieldName.firstChild().firstChild().toText().data() );
		    }
		    conn->setFields( tableName.firstChild().firstChild().toText().data(),
					 fieldList );
		}

		dbConnections.append( conn );
	    }
	} else {
	    tqDebug( TQString("Parse error: ") + errMsg + TQString(" in line %d"), errLine );
	}
	f.close();
    }
#endif
}

/*! Opens the database \a connection.  The connection remains open and
can be closed again with closeDatabase().
*/

bool Project::openDatabase( const TQString &connection, bool suppressDialog )
{
#ifndef TQT_NO_SQL
    DatabaseConnection *conn = databaseConnection( connection );
    if ( connection.isEmpty() && !conn )
	conn = databaseConnection( "(default)" );
    if ( !conn )
	return false;
    bool b = conn->open( suppressDialog );
    return b;
#else
    Q_UNUSED( connection );
    Q_UNUSED( suppressDialog );
    return false;
#endif
}

/*! Closes the database \a connection.
*/
void Project::closeDatabase( const TQString &connection )
{
#ifndef TQT_NO_SQL
    DatabaseConnection *conn = databaseConnection( connection );
    if ( connection.isEmpty() && !conn )
	conn = databaseConnection( "(default)" );
    if ( !conn )
	return;
    conn->close();
#else
    Q_UNUSED( connection );
#endif
}

// void Project::formClosed( FormWindow *fw )
// {
//     formWindows.remove( fw );
// }

TQObjectList *Project::formList( bool resolveFakeObjects ) const
{
    TQObjectList *l = new TQObjectList;
    for ( TQPtrListIterator<FormFile> forms(formfiles);   forms.current(); ++forms ) {
	FormFile* f = forms.current();
	if ( f->formWindow() ) {
	    if ( resolveFakeObjects && f->formWindow()->isFake() )
		l->append( objectForFakeForm( f->formWindow() ) );
	    else
		l->append( f->formWindow()->child( 0, "TQWidget" ) );
	} else if ( f->isFake() ) {
	    l->append( objectForFakeFormFile( f ) );
	}
    }
    return l;
}

DesignerProject *Project::iFace()
{
    if ( !iface )
	iface = new DesignerProjectImpl( this );
    return iface;
}

void Project::setLanguage( const TQString &l )
{
    if ( l == lang )
	return;
    lang = l;
    is_cpp = lang == "C++";
    updateCustomSettings();
    modified = true;
}

TQString Project::language() const
{
    return lang;
}

void Project::setCustomSetting( const TQString &key, const TQString &value )
{
    customSettings.remove( key );
    customSettings.insert( key, value );
    modified = true;
}

TQString Project::customSetting( const TQString &key ) const
{
    return *customSettings.find( key );
}

void Project::updateCustomSettings()
{
    if ( !projectSettingsPluginManager )
	return;

/*
    ProjectSettingsInterface *iface = 0;
    projectSettingsPluginManager->queryInterface( lang, (TQUnknownInterface**)&iface );
    if ( !iface )
	return;
    csList = iface->projectSettings();
    iface->release();
*/

    TQInterfacePtr<ProjectSettingsInterface> iface;
    projectSettingsPluginManager->queryInterface( lang, &iface );
    if ( !iface )
	return;
    csList = iface->projectSettings();
    customSettings.clear();

}

void Project::setActive( bool b )
{
    pixCollection->setActive( b );
}

void Project::addSourceFile( SourceFile *sf )
{
    sourcefiles.append( sf );
    modified = true;
    emit sourceFileAdded( sf );
}


SourceFile* Project::findSourceFile( const TQString& filename, SourceFile *ignore ) const
{
    TQPtrListIterator<SourceFile> it(sourcefiles);
    while ( it.current() ) {
	if ( it.current() != ignore && it.current()->fileName() == filename )
	    return it.current();
	++it;
    }
    return 0;
}

FormFile* Project::findFormFile( const TQString& filename, FormFile *ignore ) const
{
    TQPtrListIterator<FormFile> it(formfiles);
    while ( it.current() ) {
	if ( it.current() != ignore && it.current()->fileName() == filename )
	    return it.current();
	++it;
    }
    return 0;
}

void Project::setIncludePath( const TQString &platform, const TQString &path )
{
    if ( inclPath[platform] == path )
	return;
    inclPath.replace( platform, path );
    modified = true;
}

void Project::setLibs( const TQString &platform, const TQString &path )
{
    lbs.replace( platform, path );
}

void Project::setDefines( const TQString &platform, const TQString &path )
{
    defs.replace( platform, path );
}

void Project::setConfig( const TQString &platform, const TQString &config )
{
    cfg.replace( platform, config );
}

TQString Project::config( const TQString &platform ) const
{
    return cfg[ platform ];
}

TQString Project::libs( const TQString &platform ) const
{
    return lbs[ platform ];
}

TQString Project::defines( const TQString &platform ) const
{
    return defs[ platform ];
}

TQString Project::includePath( const TQString &platform ) const
{
    return inclPath[ platform ];
}

TQString Project::templte() const
{
    return templ;
}

void Project::setTemplate( const TQString &t )
{
    templ = t;
}

void Project::readPlatformSettings( const TQString &contents,
				    const TQString &setting,
				    TQMap<TQString, TQString> &res )
{
    const TQString platforms[] = { "", "win32", "unix", "mac", TQString::null };
    for ( int i = 0; platforms[ i ] != TQString::null; ++i ) {
	TQString p = platforms[ i ];
	if ( !p.isEmpty() )
	    p += ":";
	TQStringList lst = parse_multiline_part( contents, p + setting );
	TQString s = lst.join( " " );
	TQString key = platforms[ i ];
	if ( key.isEmpty() )
	    key = "(all)";
	res.replace( key, s );
    }
}

void Project::removePlatformSettings( TQString &contents, const TQString &setting )
{
    const TQString platforms[] = { "win32", "unix", "mac", "", TQString::null };
    for ( int i = 0; platforms[ i ] != TQString::null; ++i ) {
	TQString p = platforms[ i ];
	if ( !p.isEmpty() )
	    p += ":";
	remove_multiline_contents( contents, p + setting );
    }
}

void Project::writePlatformSettings( TQString &contents, const TQString &setting,
				     const TQMap<TQString, TQString> &input )
{
    const TQString platforms[] = { "", "win32", "unix", "mac", TQString::null };
    int i;
    LanguageInterface *iface = MetaDataBase::languageInterface( lang );
    if (iface && (setting == "SOURCES" || setting == "HEADERS")) // The (all) part will be saved later on
        i = 1;
    else
	i = 0;
    for (; platforms[ i ] != TQString::null; ++i ) {
	TQString p = platforms[ i ];
	if ( !p.isEmpty() )
	    p += ":";
	TQString key = platforms[ i ];
	if ( key.isEmpty() )
	    key = "(all)";
	TQMap<TQString, TQString>::ConstIterator it = input.find( key );
	if ( it == input.end() || (*it).isEmpty() )
	    continue;
	contents += p + setting + "\t+= " + *it + "\n\n";
    }
}

void Project::addFormFile( FormFile *ff )
{
    formfiles.append( ff );
    modified = true;
    emit formFileAdded( ff );
}

bool Project::removeFormFile( FormFile *ff )
{
    if ( !formfiles.containsRef( ff ) )
	return false;
    if ( !ff->close() )
	return false;
    formfiles.removeRef( ff );
    modified = true;
    emit formFileRemoved( ff );
    return true;
}

void Project::addObject( TQObject *o )
{
    bool wasModified = modified;
    objs.append( o );
    FormFile *ff = new FormFile( "", false, this, "qt_fakewindow" );
    ff->setFileName( "__APPOBJ" + TQString( o->name() ) + ".ui" );
    fakeFormFiles.insert( (void*)o, ff );
    MetaDataBase::addEntry( o );
    if ( hasGUI() ) {
	TQWidget *parent = MainWindow::self ? MainWindow::self->qWorkspace() : 0;
	FormWindow *fw = new FormWindow( ff, MainWindow::self, parent, "qt_fakewindow" );
	fw->setProject( this );
	if ( TQFile::exists( ff->absFileName() ) )
	    Resource::loadExtraSource( ff, ff->absFileName(),
				       MetaDataBase::languageInterface( language() ), false );
	if ( MainWindow::self )
	    fw->setMainWindow( MainWindow::self );
	if ( MainWindow::self ) {
	    TQApplication::sendPostedEvents( MainWindow::self->qWorkspace(), TQEvent::ChildInserted );
	    connect( fw,
		     TQ_SIGNAL( undoRedoChanged( bool, bool, const TQString &, const TQString & ) ),
		     MainWindow::self,
		     TQ_SLOT( updateUndoRedo( bool, bool, const TQString &, const TQString & ) )
		);
	}
	if ( fw->parentWidget() ) {
	    fw->parentWidget()->setFixedSize( 1, 1 );
	    fw->show();
	}
    } else {
	if ( TQFile::exists( ff->absFileName() ) )
	    Resource::loadExtraSource( ff, ff->absFileName(),
				       MetaDataBase::languageInterface( language() ), false );
    }
    emit objectAdded( o );
    modified = wasModified;
}

void Project::setObjects( const TQObjectList &ol )
{
    for ( TQObjectListIt it( ol ); it.current(); ++it )
	addObject( it.current() );
}

void Project::removeObject( TQObject *o )
{
    bool wasModified = modified;
    objs.removeRef( o );
    MetaDataBase::removeEntry( o );
    fakeFormFiles.remove( (void*)o );
    emit objectRemoved( o );
    modified = wasModified;
}

TQObjectList Project::objects() const
{
    return objs;
}

FormFile *Project::fakeFormFileFor( TQObject *o ) const
{
    return fakeFormFiles.find( (void*)o );
}

TQObject *Project::objectForFakeForm( FormWindow *fw ) const
{
    for ( TQPtrDictIterator<FormFile> it( fakeFormFiles ); it.current(); ++it ) {
	if ( it.current()->formWindow() == fw ||
	    it.current() == fw->formFile() )
	    return (TQObject*)it.currentKey();
    }
    return 0;
}

TQObject *Project::objectForFakeFormFile( FormFile *ff ) const
{
    for ( TQPtrDictIterator<FormFile> it( fakeFormFiles ); it.current(); ++it ) {
	if ( it.current() == ff )
	    return (TQObject*)it.currentKey();
    }
    return 0;
}

void Project::removeTempProject()
{
    if ( !singleProjectMode() )
	return;
    TQDir d( TQFileInfo( filename ).dirPath() );
    if ( !d.exists( TQFileInfo( filename ).dirPath() ) )
	return;
    TQStringList files = d.entryList( TQDir::Files );
    TQStringList::Iterator it;
    for ( it = files.begin(); it != files.end(); ++it ) {
	d.remove( *it );
    }
    if ( d.exists( TQFileInfo( filename ).dirPath() + "/images" ) ) {
	d = TQDir( TQFileInfo( filename ).dirPath() + "/images" );
	files = d.entryList( TQDir::Files );
	for ( it = files.begin(); it != files.end(); ++it )
	    d.remove( *it );
	d = TQDir( TQFileInfo( filename ).dirPath() );
	d.remove( "images" );
    }
    d.remove( TQFileInfo( filename ).dirPath() );
#if defined(Q_OS_UNIX)
    // ##### implement for all platforms, ideally should be in TQt
    ::rmdir( d.absPath().latin1() );
#endif
}

void Project::addAndEditFunction( const TQString &function, const TQString &functionBody, bool openDeveloper )
{
    for ( SourceFile *f = sourcefiles.first(); f; f = sourcefiles.next() ) {
	if ( TQFileInfo( f->fileName() ).baseName() == "main" ) {
	    TQValueList<LanguageInterface::Function> funcs;
	    LanguageInterface *iface = MetaDataBase::languageInterface( language() );
	    if ( !iface )
		return;
	    iface->functions( f->text(), &funcs );
	    TQString func = function;
	    int i = func.find( '(' );
	    if ( i != -1 )
		func = func.left( i );

	    bool found = false;
	    for ( TQValueList<LanguageInterface::Function>::Iterator it = funcs.begin();
		  it != funcs.end(); ++it ) {
		if ( (*it).name.left( (*it).name.find( '(' ) ) == func ) {
		    found = true;
		    break;
		}
	    }

	    if ( !found ) {
		TQString code = f->text();
		if ( functionBody.isEmpty() )
		    code += "\n\n" + iface->createFunctionStart( "", func, "", "" ) + "()\n{\n\n}\n";
		else
		    code += "\n\n" + iface->createFunctionStart( "", func, "", "" ) +
			    "()\n" + functionBody + "\n";
		f->setText( code );
		if ( f->editor() )
		    f->editor()->refresh( false );
	    }

	    if ( openDeveloper ) {
		if ( MainWindow::self )
		    MainWindow::self->editSource( f );
		f->editor()->setFunction( func, "" );
	    }

	    break;
	}
    }
}

bool Project::hasParentObject( TQObject *o )
{
    for ( TQObject *p = objs.first(); p; p = objs.next() ) {
	TQObject *c = p->child( o->name(), o->className() );
	if ( c )
	    return true;
    }
    return false;
}

TQString Project::qualifiedName( TQObject *o )
{
    TQString name = o->name();
    TQObject *p = o->parent();
    while ( p ) {
	name.prepend( TQString( p->name() ) + "." );
	if ( objs.findRef( p ) != -1 )
	    break;
	p = p->parent();
    }
    return name;
}

bool Project::singleProjectMode() const
{
    return !MainWindow::self || MainWindow::self->singleProjectMode();
}

TQWidget *Project::messageBoxParent() const
{
    return MainWindow::self;
}

void Project::designerCreated()
{
    for ( FormFile *ff = formfiles.first(); ff; ff = formfiles.next() ) {
	FormWindow *fw = ff->formWindow();
	if ( !fw || fw->mainWindow() )
	    continue;
	fw->setMainWindow( MainWindow::self );
	connect( fw, TQ_SIGNAL( undoRedoChanged( bool, bool, const TQString &,
					      const TQString & ) ),
		 MainWindow::self, TQ_SLOT( updateUndoRedo( bool, bool,
					 const TQString &, const TQString & ) ) );
	fw->reparent( MainWindow::self->qWorkspace(), TQPoint( 0, 0 ), false );
	TQApplication::sendPostedEvents( MainWindow::self->qWorkspace(),
					TQEvent::ChildInserted );
	fw->parentWidget()->setFixedSize( 1, 1 );
	fw->show();
    }
}

void Project::formOpened( FormWindow *fw )
{
    if ( fw->isFake() )
	return;
    emit newFormOpened( fw );
}

TQString Project::locationOfObject( TQObject *o )
{
    if ( !o )
	return TQString::null;

    if ( MainWindow::self ) {
	TQWidgetList windows = MainWindow::self->qWorkspace()->windowList();
	for ( TQWidget *w = windows.first(); w; w = windows.next() ) {
	    FormWindow *fw = ::tqt_cast<FormWindow*>(w);
	    SourceEditor *se = ::tqt_cast<SourceEditor*>(w);
	    if ( fw ) {
		if ( fw->isFake() )
		    return objectForFakeForm( fw )->name() + TQString( " [Source]" );
		else
		    return fw->name() + TQString( " [Source]" );
	    } else if ( se ) {
		if ( !se->object() )
		    continue;
		if ( se->formWindow() )
		    return se->formWindow()->name() + TQString( " [Source]" );
		else
		    return makeRelative( se->sourceFile()->fileName() );
	    }
	}
    }

    if ( ::tqt_cast<SourceFile*>(o) ) {
	for ( TQPtrListIterator<SourceFile> sources = sourceFiles();
	      sources.current(); ++sources ) {
	    SourceFile* f = sources.current();
	    if ( f == o )
		return makeRelative( f->fileName() );
	}
    }

    extern TQMap<TQWidget*, TQString> *qwf_forms;
    if ( !qwf_forms ) {
	tqWarning( "Project::locationOfObject: qwf_forms is NULL!" );
	return TQString::null;
    }

    TQString s = makeRelative( *qwf_forms->find( (TQWidget*)o ) );
    s += " [Source]";
    return s;
}

bool Project::hasGUI() const
{
    return tqApp->type() != TQApplication::Tty;
}
