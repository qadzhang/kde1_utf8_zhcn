/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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

#include "qplatformdefs.h"

#include "mainwindow.h"
#include "formwindow.h"

#include "designerapp.h"

#include <ntqtextstream.h>
#include <ntqobjectlist.h>
#include <ntqsettings.h>
#include <ntqsplashscreen.h>
#include <ntqdir.h>

#include <stdlib.h>
#include <signal.h>

#if defined(TQ_WS_WIN)
#include <qt_windows.h>
#include <process.h>
#endif

#if defined(Q_OS_UNIX)
#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static void signalHandler( QT_SIGNAL_ARGS )
{
    TQFile f( TQDir::homeDirPath() + "/.designerargs" );
    f.open( IO_ReadOnly );
    TQString args;
    f.readLine( args, f.size() );
    TQStringList lst = TQStringList::split( " ", args );
    for ( TQStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
	TQString arg = (*it).stripWhiteSpace();
	if ( arg[0] != '-' ) {
	    TQObjectList* l = MainWindow::self->queryList( "FormWindow" );
	    FormWindow* fw = (FormWindow*) l->first();
#if 0 // ### what's this dead code for?
	    FormWindow* totop;
#endif
	    bool haveit = false;
	    while ( fw ) {
		haveit = haveit || fw->fileName() == arg;
#if 0 // ### what's this dead code for?
		if ( haveit )
		    totop = fw;
#endif
		fw = (FormWindow*) l->next();
	    }
	    if ( !haveit )
		MainWindow::self->openFormWindow( arg );
	}
    }
    MainWindow::self->raise();
    MainWindow::self->setActiveWindow();
}

#if defined(Q_C_CALLBACKS)
}
#endif

#endif

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static void exitHandler( QT_SIGNAL_ARGS )
{
    TQDir d( TQDir::homeDirPath() );
    d.remove( ".designerpid" );
    exit( -1 );
}

#if defined(Q_C_CALLBACKS)
}
#endif

int main( int argc, char *argv[] )
{
    TQApplication::setColorSpec( TQApplication::ManyColor );

    DesignerApplication a( argc, argv );

    DesignerApplication::setOverrideCursor( TQt::WaitCursor );

    bool creatPid = false;
    if ( a.argc() > 1 ) {
	TQString arg1 = a.argv()[ 1 ];
	if ( arg1 == "-client" ) {
	    TQFile pf( TQDir::homeDirPath() + "/.designerpid" );
	    if ( pf.exists() && pf.open( IO_ReadOnly ) ) {
		TQString pidStr;
		pf.readLine( pidStr, pf.size() );
		TQFile f( TQDir::homeDirPath() + "/.designerargs" );
		f.open( IO_WriteOnly );
		TQTextStream ts( &f );
		for ( int i = 1; i < a.argc(); ++i )
		    ts << a.argv()[ i ] << " ";
		ts << endl;
		f.close();
#if defined(Q_OS_UNIX)
		if ( kill( pidStr.toInt(), SIGUSR1 ) == -1 )
		    creatPid = true;
		else
		    return 0;
#elif defined(Q_OS_WIN32)
		if ( !GetProcessVersion( pidStr.toUInt() ) ) {
		    creatPid = true;
		} else {
		    if ( a.winVersion() & TQt::WV_NT_based )
			    SendMessage( HWND_BROADCAST, RegisterWindowMessage((TCHAR*)"QT_DESIGNER_OPEN_FILE"), 0, 0 );
		    else
			    SendMessage( HWND_BROADCAST, RegisterWindowMessageA("QT_DESIGNER_OPEN_FILE"), 0, 0 );
		    return 0;
		}
#endif
	    } else {
		creatPid = true;
	    }
	} else if(arg1 == "-debug_stderr") {
	    extern bool debugToStderr; //outputwindow.cpp
	    debugToStderr = true;
	}
    }

    if ( creatPid ) {
	TQFile pf( TQDir::homeDirPath() + "/.designerpid" );
	pf.open( IO_WriteOnly );
	TQTextStream ts( &pf );
#if defined(Q_OS_UNIX)
	signal( SIGUSR1, signalHandler );
#endif
	ts << getpid() << endl;

	pf.close();
	signal( SIGABRT, exitHandler );
	signal( SIGFPE, exitHandler );
	signal( SIGILL, exitHandler );
	signal( SIGINT, exitHandler );
	signal( SIGSEGV, exitHandler );
	signal( SIGTERM, exitHandler );
    }

    extern void qInitImages_tqtdesignercore();
    qInitImages_tqtdesignercore();

    TQSettings config;
    TQString keybase = DesignerApplication::settingsKey();
    config.insertSearchPath( TQSettings::Windows, "/Trolltech" );
    TQStringList pluginPaths = config.readListEntry( keybase + "PluginPaths" );
    if (pluginPaths.count())
	TQApplication::setLibraryPaths(pluginPaths);

    TQSplashScreen *splash = a.showSplash();

    MainWindow *mw = new MainWindow( creatPid );
    a.setMainWidget( mw );
    mw->setCaption( "TQt Designer by Trolltech" );
    if ( config.readBoolEntry( keybase + "Geometries/MainwindowMaximized", false ) ) {
	int x = config.readNumEntry( keybase + "Geometries/MainwindowX", 0 );
	int y = config.readNumEntry( keybase + "Geometries/MainwindowY", 0 );
	mw->move( x, y );
	mw->showMaximized();
    } else {
	mw->show();
    }
    if ( splash )
	splash->finish( mw );
    delete splash;

    TQApplication::restoreOverrideCursor();
    for ( int i = 1; i < a.argc(); ++i ) {
	TQString arg = a.argv()[ i ];
	if ( arg[0] != '-' )
	    mw->fileOpen( "", "", arg );
    }

    return a.exec();
}
