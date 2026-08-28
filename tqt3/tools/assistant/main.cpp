/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the TQt Assistant.
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

#include "mainwindow.h"
#include "helpdialogimpl.h"
#include "config.h"

#include <ntqapplication.h>
#include <ntqserversocket.h>
#include <ntqsocket.h>
#include <ntqpixmap.h>
#include <ntqptrlist.h>
#include <ntqstringlist.h>
#include <ntqdir.h>
#include <ntqmessagebox.h>
#include <ntqguardedptr.h>
#include <stdlib.h>
#include <stdio.h>
#include <ntqtextcodec.h>

#ifdef TQ_WS_WIN
#define INDEX_CHECK( text ) if( i+1 >= argc ) { TQMessageBox::information( 0, "TQt Assistant", text ); return 1; }
#else
#define INDEX_CHECK( text ) if( i+1 >= argc ) { fprintf( stderr, text "\n" ); return 1; }
#endif

static bool allowFirstRun = true;

class AssistantSocket : public TQSocket
{
    TQ_OBJECT
public:
    AssistantSocket( int sock, TQObject *parent = 0 );
    ~AssistantSocket() {}

signals:
    void showLinkRequest( const TQString& );

private slots:
    void readClient();
    void connectionClosed();
};


class AssistantServer : public TQServerSocket
{
    TQ_OBJECT
public:
    AssistantServer( TQObject* parent = 0 );
    void newConnection( int socket );
    TQ_UINT16 getPort() const;

signals:
    void showLinkRequest( const TQString& );
    void newConnect();

private:
    TQ_UINT16 p;
};


AssistantSocket::AssistantSocket( int sock, TQObject *parent )
    : TQSocket( parent, 0 )
{
    connect( this, TQ_SIGNAL( readyRead() ),
	     TQ_SLOT( readClient() ) );
    connect( this, TQ_SIGNAL( connectionClosed() ),
	     TQ_SLOT( connectionClosed() ) );
    setSocket( sock );
}

void AssistantSocket::readClient()
{
    TQString link = TQString::null;
    while ( canReadLine() )
	link = readLine();
    if ( !link.isNull() ) {
	link = link.replace( "\n", "" );
	link = link.replace( "\r", "" );
	emit showLinkRequest( link );
    }
}

void AssistantSocket::connectionClosed()
{
    delete this;
}

AssistantServer::AssistantServer( TQObject *parent )
    : TQServerSocket( 0x7f000001, 0, 1, parent )
{
    if ( !ok() ) {
	TQMessageBox::critical( 0, tr( "TQt Assistant" ),
		tr( "Failed to bind to port %1" ).arg( port() ) );
        exit( 1 );
    }
    p = port();
}

TQ_UINT16 AssistantServer::getPort() const
{
    return p;
}

void AssistantServer::newConnection( int socket )
{
    AssistantSocket *as = new AssistantSocket( socket, this );
    connect( as, TQ_SIGNAL( showLinkRequest( const TQString& ) ),
	     this, TQ_SIGNAL( showLinkRequest( const TQString& ) ) );
    emit newConnect();
}

int main( int argc, char ** argv )
{
    bool withGUI = true;
    if ( argc > 1 ) {
	TQString arg( argv[1] );
	arg = arg.lower();
	if ( arg == "-addcontentfile"
	    || arg == "-removecontentfile"
#ifndef TQ_WS_WIN
	    || arg == "-help"
#endif
	    )
	    withGUI = false;
    }
    TQApplication a(argc, argv, withGUI);

    TQString resourceDir;
    AssistantServer *as = 0;
    TQStringList catlist;
    TQString file, profileName, aDocPath;
    bool server = false;
    bool hideSidebar = false;
    bool startClean = false;
    bool configLoaded = false;
    if ( argc == 2 ) {
	if ( (argv[1])[0] != '-' )
	    file = argv[1];
    }
    if ( file.isEmpty() ) {
	for ( int i = 1; i < argc; i++ ) {
        if ( TQString( argv[i] ).lower() == "-file" ) {
		INDEX_CHECK( "Missing file argument!" );
		i++;
		file = argv[i];
	    } else if ( TQString( argv[i] ).lower() == "-server" ) {
	        server = true;
	    } else if ( TQString( argv[i] ).lower() == "-profile" ) {
		INDEX_CHECK( "Missing profile argument!" );
		profileName = argv[++i];
	    } else if ( TQString( argv[i] ).lower() == "-addcontentfile" ) {
		INDEX_CHECK( "Missing content file!" );
		Config *c = Config::loadConfig( TQString::null );
		TQFileInfo file( argv[i+1] );
		if( !file.exists() ) {
		    fprintf( stderr, "Could not locate content file: '%s'\n",
			     file.absFilePath().latin1() );
		    fflush( stderr );
		    return 1;
		}
		DocuParser *parser = DocuParser::createParser( file.absFilePath() );
		if( parser ) {
		    TQFile f( argv[i+1] );
		    if( !parser->parse( &f ) ) {
			fprintf( stderr, "Failed to parse file: '%s'\n, ",
				 file.absFilePath().latin1() );
			fflush( stderr );
			return 1;
		    }
		    parser->addTo( c->profile() );
		    c->setDocRebuild( true );
		    c->save();
		}
		return 0;
	    } else if ( TQString( argv[i] ).lower() == "-removecontentfile" ) {
		INDEX_CHECK( "Missing content file!" );
		Config *c = Config::loadConfig( TQString::null );
		Profile *profile = c->profile();
		TQStringList entries = profile->docs.grep(argv[i+1]);
		if (entries.count() == 0) {
		    fprintf(stderr, "Could not locate content file: '%s'\n",
			    argv[i+1]);
		    fflush(stderr);
		    return 1;
        } else if (entries.count() > 1) {
		    fprintf(stderr, "More than one entry matching file name found, "
			"please specify full path to file");
		    fflush(stderr);
		    return 1;
        } else {
		    TQFileInfo file(entries[0]);
		    if( !file.exists() ) {
			fprintf( stderr, "Could not locate content file: '%s'\n",
			    file.absFilePath().latin1() );
			fflush( stderr );
			return 1;
		    }
		    profile->removeDocFileEntry( file.absFilePath() );
		    c->setDocRebuild( true );
		    c->save();
        }
		return 0;
        } else if ( TQString( argv[i] ).lower() == "-docpath" ) {
            INDEX_CHECK( "Missing path!" );
            TQDir dir( argv[i+1] );
            if ( dir.exists() ) {
                Config *c = Config::loadConfig( TQString::null );
                c->saveProfile(Profile::createDefaultProfile(dir.absPath()));
                c->loadDefaultProfile();
                c->setDocRebuild( true );
                c->save();
                configLoaded = true;
                ++i;
            } else {
                fprintf( stderr, "The specified path does not exist!\n");
                fflush( stderr );
                return 1;
            }
        } else if ( TQString( argv[i] ).lower() == "-hidesidebar" ) {
		hideSidebar = true;
	    } else if ( TQString( argv[i] ).lower() == "-help" ) {
		TQString helpText( "Usage: assistant [option]\n"
				  "Options:\n"
				  " -file Filename             assistant opens the specified file\n"
				  " -server                    reads commands from a socket after\n"
				  "                            assistant has started\n"
				  " -profile fileName          starts assistant and displays the\n"
				  "                            profile specified in the file fileName.\n"
				  " -addContentFile file       adds the content file 'file' to the set of\n"
				  "                            documentation available by default\n"
				  " -removeContentFile file    removes the content file 'file' from the\n"
				  "                            documentation available by default\n"
                  " -docPath path              sets the TQt documentation root path to\n"
                  "                            'path' and starts assistant\n"
				  " -hideSidebar               assistant will hide the sidebar.\n"
				  " -resourceDir               assistant will load translations from\n"
                  "                            this directory.\n"
				  " -help                      shows this help.");
#ifdef TQ_WS_WIN
		TQMessageBox::information( 0, "TQt Assistant", "<pre>" + helpText + "</pre>" );
#else
		printf( "%s\n", helpText.latin1() );
#endif
		exit( 0 );
	    } else if ( TQString( argv[i] ).lower() == "-resourcedir" ) {
		INDEX_CHECK( "Missing resource directory argument!" );
		resourceDir = TQString( argv[++i] );
        } else {
            fprintf( stderr, "Unrecognized option '%s'. Try -help to get help.\n",
                argv[i] );
            fflush( stderr );
            return 1;
        }
	}
    }

    if( resourceDir.isNull() )
	resourceDir = tqInstallPathTranslations();

    TQTranslator translator( 0 );
    translator.load( TQString("assistant_") + TQTextCodec::locale(), resourceDir );
    a.installTranslator( &translator );

    TQTranslator qtTranslator( 0 );
    qtTranslator.load( TQString("qt_") + TQTextCodec::locale(), resourceDir );
    a.installTranslator( &qtTranslator );


    Config *conf = 0;
    if (configLoaded)
        conf = Config::configuration();
    else
        conf = Config::loadConfig( profileName );
    if (!conf) {
        fprintf( stderr, "Profile '%s' does not exist!\n", profileName.latin1() );
        fflush( stderr );
        return -1;
    }

    bool max = conf->isMaximized();
    TQStringList links = conf->source();
    conf->hideSideBar( hideSidebar );

    TQGuardedPtr<MainWindow> mw = new MainWindow( 0, "Assistant" );

    if ( server ) {
	as = new AssistantServer();
	printf("%d\n", as->port() );
	fflush( stdout );
	as->connect( as, TQ_SIGNAL( showLinkRequest( const TQString& ) ),
		     mw, TQ_SLOT( showLinkFromClient( const TQString& ) ) );
    }

    if ( max )
	mw->showMaximized();
    else
	mw->show();

    if ( !file.isEmpty() )
	mw->showLink( file );
    else if ( file.isEmpty() )
	mw->showLinks( links );

    a.connect( &a, TQ_SIGNAL( lastWindowClosed() ), &a, TQ_SLOT( quit() ) );

    int appExec = a.exec();
    delete (MainWindow*)mw;
    return appExec;
}

#include "main.moc"
